/*
 * Author:      William Chia-Wei Cheng (bill.cheng@acm.org)
 *
 * Copyright (C) 2001-2009, William Chia-Wei Cheng.
 *
 * This file may be distributed under the terms of the Q Public License
 * as defined by Trolltech AS of Norway and appearing in the file
 * LICENSE.QPL included in the packaging of this file.
 *
 * THIS FILE IS PROVIDED AS IS WITH NO WARRANTY OF ANY KIND, INCLUDING
 * THE WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL,
 * INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING
 * FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT,
 * NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION
 * WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/import.c,v 1.52 2011/06/14 03:22:14 william Exp $
 */

#define _INCLUDE_FROM_IMPORT_C_

#include "tgifdefs.h"
#include "cmdids.h"

#include "attr.e"
#include "auxtext.e"
#include "box.e"
#include "choice.e"
#include "choose.e"
#include "cmd.e"
#include "color.e"
#include "cursor.e"
#include "cutpaste.e"
#include "dialog.e"
#include "drawing.e"
#include "dup.e"
#include "eps.e"
#include "file.e"
#include "font.e"
#include "grid.e"
#include "imgproc.e"
#include "import.e"
#include "mainloop.e"
#include "mainmenu.e"
#include "mark.e"
#include "msg.e"
#include "menu.e"
#include "move.e"
#include "names.e"
#include "navigate.e"
#include "obj.e"
#include "page.e"
#include "pngtrans.e"
#include "raster.e"
#include "rect.e"
#include "remote.e"
#include "scroll.e"
#include "select.e"
#include "setup.e"
#include "special.e"
#include "stretch.e"
#include "strtbl.e"
#include "text.e"
#include "util.e"
#include "xbitmap.e"
#include "xpixmap.e"
#include "z_intrf.e"

typedef struct ImportInfoRec {
   char *name, *ext, *cmd;
   struct ImportInfoRec *next;
} *ImportInfoPtr;

static struct ImportInfoRec *topImportInfo=NULL, *botImportInfo=NULL;
static int gnMaxImportFilters=0;
static int showFileNameOnBrowse=TRUE;

static int thumbnailW=160;
static int thumbnailH=120;
static int thumbnailPadding=8;
static int thumbnailXGap=16;
static int thumbnailYGap=0;
static int thumbnailX=32;
static int thumbnailY=32;

void MillisecondSleep(ms)
   int ms;
{
   struct timeval timeout;
   fd_set fdset;

   timeout.tv_sec = 0;
   timeout.tv_usec = ms*1000;
   FD_ZERO(&fdset);
#ifdef __hpux
   select(0, (int*)&fdset, NULL, NULL, &timeout);
#else /* !__hpux */
   select(0, &fdset, NULL, NULL, &timeout);
#endif /* __hpux */
}

int ExtensionMatch(Spec, DirName)
   char *Spec, *DirName;
{
   char *c_ptr, other_ext_str[MAXSTRING];
   int len;

   if (Spec == NULL || *Spec == '\0') return FALSE;
   len = strlen(DirName);
   strcpy(other_ext_str, Spec);
   for (c_ptr=strtok(other_ext_str,";"); c_ptr != NULL;
         c_ptr=strtok(NULL, ";")) {
      int other_ext_len=strlen(c_ptr);

      if (len > other_ext_len &&
            UtilStrICmp(c_ptr, &DirName[len-other_ext_len]) == 0) {
         return TRUE;
      }
   }
   return FALSE;
}

char *SetUpExtStr(cbBuf, pszExt, pszOtherExt)
   int cbBuf;
   char *pszExt, *pszOtherExt;
   /* pszExt="gif;GIF", pszOtherExt=".jpg;.jpeg" */
{
   char *c_ptr, *buf=(char*)malloc(cbBuf*sizeof(char)), *start_ptr, *dest_ptr;

   if (buf == NULL) {
      FailAllocMessage();
      return NULL;
   }
   *buf = '\0';
   dest_ptr = buf;

   start_ptr = pszExt;
   while (*start_ptr == ' ') start_ptr++;
   c_ptr = strchr(start_ptr, ';');
   while (start_ptr != NULL) {
      if (c_ptr != NULL) *c_ptr = '\0';
      if(dest_ptr == buf) {
         sprintf(dest_ptr, ".%s", start_ptr);
      } else {
         sprintf(dest_ptr, ";.%s", start_ptr);
      }
      dest_ptr = (&dest_ptr[strlen(dest_ptr)]);
      if (c_ptr != NULL) *c_ptr++ = ';';
      if (c_ptr == NULL) break;
      start_ptr = c_ptr;
      while (*start_ptr == ' ') start_ptr++;
      c_ptr = strchr(start_ptr, ';');
   }
   if (dest_ptr == buf) {
      strcpy(buf, pszOtherExt);
   } else if (*pszOtherExt == '\0') {
      *dest_ptr = '\0';
   } else {
      sprintf(dest_ptr, ";%s", pszOtherExt);
   }
   return buf;
}

void CleanUpImport()
{
   struct ImportInfoRec *next_import;

   while (topImportInfo != NULL) {
      next_import = topImportInfo->next;

      if(topImportInfo->name != NULL) free(topImportInfo->name);
      if(topImportInfo->ext != NULL) free(topImportInfo->ext);
      if(topImportInfo->cmd != NULL) free(topImportInfo->cmd);
      free(topImportInfo);
      topImportInfo = next_import;
   }
   topImportInfo = botImportInfo = NULL;
}

#define INVALID_FILTER_SPEC_DEFAULT 0
#define INVALID_FILTER_SPEC_MISSING_PERC_S 1
#define INVALID_FILTER_SPEC_TOO_MANY_PERC_S 2

static
void InvalidFilterSpecMsg(pszEntry, pszValue, nExplain)
   char *pszEntry, *pszValue;
   int nExplain;
{
   switch (nExplain) {
   case INVALID_FILTER_SPEC_DEFAULT:
      sprintf(gszMsgBox, TgLoadString(STID_BAD_FLTR_SPEC_DEF),
            TOOL_NAME, pszEntry, pszValue);
      break;
   case INVALID_FILTER_SPEC_MISSING_PERC_S:
      sprintf(gszMsgBox, TgLoadString(STID_BAD_FLTR_SPEC_MISS_PERC_S),
            TOOL_NAME, pszEntry, pszValue);
      break;
   case INVALID_FILTER_SPEC_TOO_MANY_PERC_S:
      sprintf(gszMsgBox, TgLoadString(STID_BAD_FLTR_SPEC_MANY_PERC_S),
            TOOL_NAME, pszEntry, pszValue);
      break;
   }
   MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
}

static
int CountPercentS(buf)
   char *buf;
{
   int count=0;
   char *c_ptr;

   for (c_ptr=strstr(buf, "%s"); c_ptr != NULL; c_ptr=strstr(++c_ptr, "%s")) {
      count++;
   }
   return count;
}

static
char *FindBlank(pszStr, ppszEnd)
   char *pszStr, **ppszEnd;
{
   char *c_ptr=pszStr, ch;

   if (!(*c_ptr == '"' || *c_ptr == '\'')) {
      *ppszEnd = strchr(pszStr, ' ');
      return pszStr;
   }
   for (ch=(*c_ptr++); *c_ptr != '\0'; c_ptr++) {
      if (*c_ptr == '\\') {
         c_ptr++;
      } else if (*c_ptr == ch) {
         *ppszEnd = c_ptr;
         return &pszStr[1];
      }
   }
   *ppszEnd = NULL;
   return pszStr;
}

void InitImport()
{
   char *c_ptr;
   int max_filters=0;

   showFileNameOnBrowse = TRUE;
   if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"ShowFileNameOnBrowse")) !=
         NULL) {
      if (UtilStrICmp(c_ptr, "false") == 0) {
         showFileNameOnBrowse = FALSE;
      }
   }
   if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"MaxImportFilters")) != NULL) {
      max_filters = atoi(c_ptr);
      if (max_filters <= 0) {
         sprintf(gszMsgBox, TgLoadString(STID_BAD_XDEF_MUST_BE_GT_0),
               TOOL_NAME, "MaxImportFilters", c_ptr);
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      }
   }
   gnMaxImportFilters = 0;
   if (max_filters > 0) {
      int i;

      for (i=0; i < max_filters; i++) {
         char buf[80];

         sprintf(buf, "ImportFilter%1d", i);
         if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,buf)) == NULL) {
            sprintf(gszMsgBox, TgLoadString(STID_CANT_FIND_XDEF_CONT_LOOK_FLTR),
                  TOOL_NAME, buf);
            if (MsgBox(gszMsgBox, TOOL_NAME, YNC_MB) != MB_ID_YES) {
               break;
            }
         } else {
            char *name_ptr=UtilStrDup(c_ptr), *dup_buf=UtilStrDup(c_ptr);
            char *ext_ptr;

            if (name_ptr == NULL || dup_buf == NULL) FailAllocMessage();
            for (c_ptr=name_ptr; *c_ptr != '\0'; c_ptr++) {
               if (*c_ptr == '\t' || *c_ptr == '\n' || *c_ptr == '\r') {
                  *c_ptr = ' ';
               }
            }
            for (c_ptr=dup_buf; *c_ptr != '\0'; c_ptr++) {
               if (*c_ptr == '\t' || *c_ptr == '\n' || *c_ptr == '\r') {
                  *c_ptr = ' ';
               }
            }
            UtilTrimBlanks(name_ptr);
            name_ptr = FindBlank(name_ptr, &ext_ptr);
            if (ext_ptr == NULL) {
               InvalidFilterSpecMsg(buf, dup_buf, INVALID_FILTER_SPEC_DEFAULT);
            } else {
               char *cmd_ptr;

               *ext_ptr++ = '\0';
               UtilTrimBlanks(ext_ptr);
               ext_ptr = FindBlank(ext_ptr, &cmd_ptr);
               if (cmd_ptr == NULL) {
                  InvalidFilterSpecMsg(buf, dup_buf,
                        INVALID_FILTER_SPEC_DEFAULT);
               } else {
                  int len, count;

                  *cmd_ptr++ = '\0';
                  UtilTrimBlanks(cmd_ptr);
                  len = strlen(cmd_ptr);
                  if (len >= 2 &&
                        ((*cmd_ptr == '"' && cmd_ptr[len-1] == '"') ||
                        (*cmd_ptr == '\'' && cmd_ptr[len-1] == '\''))) {
                     cmd_ptr[len-1] = '\0';
                     cmd_ptr++;
                     UtilTrimBlanks(cmd_ptr);
                  }
                  count = CountPercentS(cmd_ptr);
                  if (count < 1) {
                     InvalidFilterSpecMsg(buf, dup_buf,
                           INVALID_FILTER_SPEC_MISSING_PERC_S);
                  } else if (count > 1) {
                     InvalidFilterSpecMsg(buf, dup_buf,
                           INVALID_FILTER_SPEC_TOO_MANY_PERC_S);
                  } else {
                     struct ImportInfoRec *pii=(struct ImportInfoRec*)malloc(
                           sizeof(struct ImportInfoRec));

                     if (pii == NULL) FailAllocMessage();
                     memset(pii, 0, sizeof(struct ImportInfoRec));
                     pii->next = NULL;
                     pii->name = UtilStrDup(name_ptr);
                     pii->ext = UtilStrDup(ext_ptr);
                     pii->cmd = UtilStrDup(cmd_ptr);
                     if (pii->name==NULL || pii->ext==NULL || pii->cmd==NULL) {
                        FailAllocMessage();
                        if (pii->name != NULL) free(pii->name);
                        if (pii->ext != NULL) free(pii->ext);
                        if (pii->cmd != NULL) free(pii->cmd);
                     } else {
                        if (botImportInfo != NULL) {
                           botImportInfo->next = pii;
                        } else {
                           topImportInfo = pii;
                        }
                        botImportInfo = pii;
                        gnMaxImportFilters++;
                     }
                  }
               }
            }
            if (name_ptr != NULL) free(name_ptr);
            if (dup_buf != NULL) free(dup_buf);
         }
      }
   }
   thumbnailW = 160;
   thumbnailH = 120;
   if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"ThumbnailGeometry")) != NULL) {
      int w=0, h=0;

      if (ParseWHSpec(c_ptr, &w, &h) && w >= 40 && h >= 30) {
         if (w >= 40 && h >= 30) {
            thumbnailW = w;
            thumbnailH = h;
         } else {
            fprintf(stderr, TgLoadString(STID_INVALID_GEOM_USE_ALT_GEOM),
                  TOOL_NAME, "ThumbnailGeometry", c_ptr, "40x30", "160x120");
            fprintf(stderr, "\n");
         }
      }
   }
   thumbnailPadding = 8;
   if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"ThumbnailPadding")) != NULL) {
      int padding=0;

      UtilTrimBlanks(c_ptr);
      if (sscanf(c_ptr, "%d", &padding) != 1) {
         fprintf(stderr, TgLoadString(STID_INVALID_XDEF_USE_ALT_VALUE),
               TOOL_NAME, "ThumbnailPadding", c_ptr, 8);
         fprintf(stderr, "\n");
      } else if (padding < 0 || padding > 100) {
         fprintf(stderr, TgLoadString(STID_INVALID_XDEF_RNG_USE_ALT_VAL),
               TOOL_NAME, "ThumbnailPadding", c_ptr, 0, 100, 8);
         fprintf(stderr, "\n");
      } else {
         thumbnailPadding = padding;
      }
   }
   thumbnailXGap = 16;
   if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"ThumbnailXGap")) != NULL) {
      int gap=0;

      UtilTrimBlanks(c_ptr);
      if (sscanf(c_ptr, "%d", &gap) != 1) {
         fprintf(stderr, TgLoadString(STID_INVALID_XDEF_USE_ALT_VALUE),
               TOOL_NAME, "ThumbnailXGap", c_ptr, 16);
         fprintf(stderr, "\n");
      } else if (gap < 0) {
         fprintf(stderr, TgLoadString(STID_INVALID_XDEF_USE_ALT_VALUE),
               TOOL_NAME, "ThumbnailXGap", c_ptr, 16);
         fprintf(stderr, "\n");
      } else {
         thumbnailXGap = gap;
      }
   }
   thumbnailYGap = 0;
   if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"ThumbnailYGap")) != NULL) {
      int gap=0;

      UtilTrimBlanks(c_ptr);
      if (sscanf(c_ptr, "%d", &gap) != 1) {
         fprintf(stderr, TgLoadString(STID_INVALID_XDEF_USE_ALT_VALUE),
               TOOL_NAME, "ThumbnailYGap", c_ptr, 16);
         fprintf(stderr, "\n");
      } else if (gap < 0) {
         fprintf(stderr, TgLoadString(STID_INVALID_XDEF_USE_ALT_VALUE),
               TOOL_NAME, "ThumbnailYGap", c_ptr, 16);
         fprintf(stderr, "\n");
      } else {
         thumbnailYGap = gap;
      }
   }
   thumbnailX = 32;
   if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"ThumbnailX")) != NULL) {
      int x=0;

      UtilTrimBlanks(c_ptr);
      if (sscanf(c_ptr, "%d", &x) != 1) {
         fprintf(stderr, TgLoadString(STID_INVALID_XDEF_USE_ALT_VALUE),
               TOOL_NAME, "ThumbnailX", c_ptr, 32);
         fprintf(stderr, "\n");
      } else {
         thumbnailX = x;
      }
   }
   thumbnailY = 32;
   if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"ThumbnailY")) != NULL) {
      int y=0;

      UtilTrimBlanks(c_ptr);
      if (sscanf(c_ptr, "%d", &y) != 1) {
         fprintf(stderr, TgLoadString(STID_INVALID_XDEF_USE_ALT_VALUE),
               TOOL_NAME, "ThumbnailY", c_ptr, 32);
         fprintf(stderr, "\n");
      } else {
         thumbnailY = y;
      }
   }
}

static
int ImportGivenXBitmapFile(remote_file, tmp_fname, xbm_fname, stid_success,
      stid_failure, pn_image_w, pn_image_h)
   int remote_file, stid_success, stid_failure, *pn_image_w, *pn_image_h;
   char *tmp_fname, *xbm_fname;
{
   char *rest=NULL, mag_spec[MAXSTRING+1];
   unsigned int tmp_w, tmp_h;
   int rc=0, short_name=FALSE, x=0, y=0, w=0, h=0;
   int x_hot=0, y_hot=0, orig_w=0, orig_h=0;
   float mag=(float)0;
   Pixmap orig_bitmap=None, bitmap=None;
   XImage *image=NULL;
   struct ObjRec *obj_ptr=NULL;

   if (pn_image_w != NULL) *pn_image_w = 0;
   if (pn_image_h != NULL) *pn_image_h = 0;
   SetWatchCursor(drawWindow);
   SetWatchCursor(mainWindow);
   rc = XReadBitmapFile(mainDisplay, mainWindow,
         (remote_file ? tmp_fname : xbm_fname), &tmp_w, &tmp_h,
         &orig_bitmap, &x_hot, &y_hot);
   orig_w = tmp_w; orig_h = tmp_h;
   SetDefaultCursor(mainWindow);
   ShowCursor();

   if (remote_file) {
      short_name = FALSE;
   } else {
      if ((short_name=IsPrefix(bootDir, xbm_fname, &rest))) ++rest;
   }
   if (rc != BitmapSuccess) {
      sprintf(gszMsgBox, TgLoadString(stid_failure),
            (short_name ? rest : xbm_fname));
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      if (remote_file) unlink(tmp_fname);
      return FALSE;
   }
   x = 0;
   y = 0;
   w = orig_w;
   h = orig_h;
   mag = 1.0;
   if (askForXBmSpec) {
      sprintf(gszMsgBox, TgLoadString(STID_ENTER_GEOM_SPEC_ORIG_SIZE),
            orig_w, orig_h);
      *mag_spec = '\0';
      Dialog(gszMsgBox, TgLoadCachedString(CSTID_DLG_ACCEPT_CONTINUE),
            mag_spec);
      if (*mag_spec != '\0') {
         ParseCutSpec(mag_spec, orig_w, orig_h, &mag, &x, &y, &w, &h);
      }
      if (x==0 && y==0 && w==orig_w && h==orig_h && mag==1.0) {
         bitmap = orig_bitmap;
      } else {
         orig_w = w;
         orig_h = h;
         if (!ExtractBitmap(orig_bitmap, NULL, x, y, w, h, &bitmap, &image)) {
            if (remote_file) unlink(tmp_fname);
            XFreePixmap(mainDisplay, orig_bitmap);
            return FALSE;
         }
         XFreePixmap(mainDisplay, orig_bitmap);
         w = (int)(((float)w) * mag);
         h = (int)(((float)h) * mag);
      }
   } else {
      bitmap = orig_bitmap;
   }
   obj_ptr = CreateXBmObj(orig_w, orig_h, w, h, bitmap, image);
   AddObj(NULL, topObj, obj_ptr);
   PlaceTopObj(obj_ptr, NULL, NULL);

   SelectTopObj();
   RecordNewObjCmd();
   SetFileModified(TRUE);
   justDupped = FALSE;

   if (!importFromLibrary && !remote_file) SetCurImportDir(xbm_fname);

   if (stid_success != INVALID) {
      sprintf(gszMsgBox, TgLoadString(stid_success),
            orig_w, orig_h, (short_name ? rest : xbm_fname));
      Msg(gszMsgBox);
   }
   if (pn_image_w != NULL) *pn_image_w = orig_w;
   if (pn_image_h != NULL) *pn_image_h = orig_h;

   return TRUE;
}

void ImportXBitmapFile()
{
   char xbm_fname[MAXPATHLENGTH+1], tmp_fname[MAXPATHLENGTH+1];
   int remote_file=FALSE;
   XEvent ev;

   MakeQuiescent();

   importingFile = TRUE;
   *xbm_fname = *tmp_fname = '\0';
   if (importFromLibrary) {
      char name[MAXSTRING+1], path[MAXSTRING+1];

      if (SelectFromLibrary(TgLoadString(STID_SEL_XBM_FILE_TO_IMPORT),
            XBM_FILE_EXT, name, path) == INVALID) {
         importingFile = FALSE;
         return;
      }
      sprintf(xbm_fname, "%s%c%s", path, DIR_SEP, name);
   } else if (SelectFileNameToImport(TgLoadString(STID_SEL_XBM_FILE_TO_IMPORT),
         XBM_FILE_EXT, xbm_fname) == INVALID) {
      importingFile = FALSE;
      return;
   } else if (FileIsRemote(xbm_fname)) {
      int is_html=FALSE;

      if (!DownloadRemoteFile(xbm_fname, NULL, NULL, &is_html, tmp_fname, NULL,
            0) || *tmp_fname == '\0') {
         importingFile = FALSE;
         return;
      }
      remote_file = TRUE;
   }
   XSync(mainDisplay, False);
   if (XCheckMaskEvent(mainDisplay, ExposureMask, &ev)) {
      ExposeEventHandler(&ev, TRUE);
   }
   ImportGivenXBitmapFile(remote_file, tmp_fname, xbm_fname,
         STID_GIVEN_XBM_SIZE_FILE_IMPORTED, STID_CANNOT_IMPORT_XBM_FILE,
         NULL, NULL);

   if (remote_file) unlink(tmp_fname);
   importingFile = FALSE;
}

static
int ImportGivenXPixmapFile(remote_file, tmp_fname, xpm_fname, stid_success,
      stid_failure, pn_image_w, pn_image_h)
   int remote_file, stid_success, stid_failure, *pn_image_w, *pn_image_h;
   char *tmp_fname, *xpm_fname;
{
   char *rest=NULL;
   int rc=0, ncolors=0, chars_per_pixel=0, *pixels=NULL, short_name=FALSE;
   int first_pixel_is_bg=FALSE, image_w=0, image_h=0, w=0, h=0;
   Pixmap pixmap=None, bitmap=None;
   XImage *image=NULL, *bitmap_image=NULL;
   char *color_char, **color_str=NULL, *xpm_data=NULL;
   struct ObjRec *obj_ptr=NULL;

   if (pn_image_w != NULL) *pn_image_w = 0;
   if (pn_image_h != NULL) *pn_image_h = 0;
   SetWatchCursor(drawWindow);
   SetWatchCursor(mainWindow);
   rc = MyReadPixmapFile(remote_file ? tmp_fname : xpm_fname,
         &image_w, &image_h, &w, &h, &pixmap, &image, &bitmap, &bitmap_image,
         &ncolors, &chars_per_pixel, &first_pixel_is_bg, &color_char,
         &color_str, &pixels, &xpm_data);
   SetDefaultCursor(mainWindow);
   ShowCursor();

   if (remote_file) {
      short_name = FALSE;
   } else {
      if ((short_name=IsPrefix(bootDir, xpm_fname, &rest))) ++rest;
   }
   if (rc != BitmapSuccess) {
      if (stid_failure != INVALID) {
         sprintf(gszMsgBox, TgLoadString(STID_CANNOT_IMPORT_XPM_FILE),
               (short_name ? rest : xpm_fname));
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      }
      return FALSE;
   }
   obj_ptr = CreateXPmObj(image_w, image_h, w, h, pixmap, image, bitmap,
         bitmap_image, ncolors, chars_per_pixel, first_pixel_is_bg, color_char,
         color_str, pixels, xpm_data);
   AddObj(NULL, topObj, obj_ptr);
   PlaceTopObj(obj_ptr, NULL, NULL);

   SelectTopObj();
   RecordNewObjCmd();
   SetFileModified(TRUE);
   justDupped = FALSE;
   if (!PRTGIF && colorLayers && needToRedrawColorWindow) {
      RedrawColorWindow();
   }
   if (!importFromLibrary && !remote_file && tmp_fname != NULL) {
      SetCurImportDir(xpm_fname);
   }
   if (stid_success != INVALID) {
      sprintf(gszMsgBox, TgLoadString(STID_GIVEN_XPM_SIZE_FILE_IMPORTED),
            image_w, image_h, (short_name ? rest : xpm_fname));
      Msg(gszMsgBox);
   }
   if (pn_image_w != NULL) *pn_image_w = image_w;
   if (pn_image_h != NULL) *pn_image_h = image_h;

   return TRUE;
}

void ImportXPixmapFile()
{
   char xpm_fname[MAXPATHLENGTH+1], tmp_fname[MAXPATHLENGTH+1];
   int remote_file=FALSE;
   XEvent ev;

   MakeQuiescent();

   importingFile = TRUE;
   *xpm_fname = *tmp_fname = '\0';
   if (importFromLibrary) {
      char name[MAXSTRING+1], path[MAXSTRING+1];

      if (SelectFromLibrary(TgLoadString(STID_SEL_XPM_FILE_TO_IMPORT),
            XPM_FILE_EXT, name, path) == INVALID) {
         importingFile = FALSE;
         return;
      }
      sprintf(xpm_fname, "%s%c%s", path, DIR_SEP, name);
   } else if (SelectFileNameToImport(TgLoadString(STID_SEL_XPM_FILE_TO_IMPORT),
         XPM_FILE_EXT, xpm_fname) == INVALID) {
      importingFile = FALSE;
      return;
   } else if (FileIsRemote(xpm_fname)) {
      int is_html=FALSE;

      if (!DownloadRemoteFile(xpm_fname, NULL, NULL, &is_html, tmp_fname, NULL,
            0) || *tmp_fname == '\0') {
         importingFile = FALSE;
         return;
      }
      remote_file = TRUE;
   }
   XSync(mainDisplay, False);
   if (XCheckMaskEvent(mainDisplay, ExposureMask, &ev)) {
      ExposeEventHandler(&ev, TRUE);
   }
   ImportGivenXPixmapFile(remote_file, tmp_fname, xpm_fname,
         STID_GIVEN_XPM_SIZE_FILE_IMPORTED, STID_CANNOT_IMPORT_XPM_FILE,
         NULL, NULL);

   if (remote_file) unlink(tmp_fname);
   importingFile = FALSE;
}

void ImportEPSFile(Embed, psz_fname_spec)
   int Embed;
   char *psz_fname_spec;
{
   char eps_fname[MAXPATHLENGTH+1], *rest=NULL, tmp_fname[MAXPATHLENGTH+1];
   char **lines=NULL, write_date[32];
   int i, rc, short_name, num_lines=0, epsf_level, image_w, image_h;
   int remote_file=FALSE;
   float llx, lly, urx, ury;
   Pixmap bitmap=None;
   XImage *image=NULL;
   XEvent ev;
   struct ObjRec *obj_ptr=NULL;

   MakeQuiescent();

   importingFile = TRUE;
   *eps_fname = *tmp_fname = '\0';
   if (psz_fname_spec != NULL && strcmp(psz_fname_spec, "-1") != 0) {
      int len=strlen(psz_fname_spec);

      if (len > 0 && psz_fname_spec[len-1] == ')') {
         psz_fname_spec[len-1] = '\0';
         UtilStrCpyN(tmp_fname, sizeof(tmp_fname), psz_fname_spec);
         psz_fname_spec[len-1] = ')';
      } else {
         UtilStrCpyN(tmp_fname, sizeof(tmp_fname), psz_fname_spec);
      }
      if (*tmp_fname == DIR_SEP) {
         strcpy(eps_fname, tmp_fname);
      } else if (curDirIsLocal) {
         sprintf(eps_fname, "%s%c%s", curDir, DIR_SEP, tmp_fname);
      } else {
         sprintf(eps_fname, "%s%c%s", curLocalDir, DIR_SEP, tmp_fname);
      }
      *tmp_fname = '\0';
   } else {
      if (importFromLibrary) {
         char name[MAXSTRING+1], path[MAXSTRING+1];

         if (SelectFromLibrary(TgLoadString(STID_SEL_EPS_FILE_TO_IMPORT),
               EPSF_FILE_EXT, name, path) == INVALID) {
            importingFile = FALSE;
            return;
         }
         sprintf(eps_fname, "%s%c%s", path, DIR_SEP, name);
      } else if (SelectFileNameToImport(
            TgLoadString(STID_SEL_EPS_FILE_TO_IMPORT), EPSF_FILE_EXT,
            eps_fname) == INVALID) {
         importingFile = FALSE;
         return;
      }
   }
   if (FileIsRemote(eps_fname)) {
      int is_html=FALSE;

      if (!Embed) {
         sprintf(gszMsgBox, TgLoadString(STID_IMPORT_REM_FILE_EMBED_INSTEAD),
               "EPS", "EPS");
         if (MsgBox(gszMsgBox, TOOL_NAME, YNC_MB) != MB_ID_YES) {
            importingFile = FALSE;
            return;
         }
         Embed = TRUE;
      }
      if (!DownloadRemoteFile(eps_fname, NULL, NULL, &is_html, tmp_fname,
           NULL, 0) || *tmp_fname == '\0') {
         importingFile = FALSE;
         return;
      }
      remote_file = TRUE;
   }
   XSync(mainDisplay, False);
   if (XCheckMaskEvent(mainDisplay, ExposureMask, &ev)) {
      ExposeEventHandler(&ev, TRUE);
   }
   SetWatchCursor(drawWindow);
   SetWatchCursor(mainWindow);
   rc = MyReadEPSFile(remote_file ? tmp_fname : eps_fname, &image_w, &image_h,
         &bitmap, &image, (Embed ? &num_lines : NULL), (Embed ? &lines : NULL),
         &epsf_level, &llx, &lly, &urx, &ury, write_date);

   if (remote_file) {
      short_name = FALSE;
   } else {
      if ((short_name=IsPrefix(bootDir, eps_fname, &rest))) ++rest;
   }
   if (rc != BitmapSuccess) {
      SetDefaultCursor(mainWindow);
      ShowCursor();
      sprintf(gszMsgBox, TgLoadString(STID_CANNOT_IMPORT_GIVEN_EPS),
            (short_name ? rest : eps_fname));
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      if (remote_file) unlink(tmp_fname);
      importingFile = FALSE;
      return;
   }

   if (Embed) saveEPSLines = TRUE;
   if (short_name) {
      obj_ptr = CreateEPSObj(rest, image_w, image_h, bitmap, image,
            num_lines, lines, epsf_level, &llx, &lly, &urx, &ury, write_date);
   } else {
      obj_ptr = CreateEPSObj(remote_file ? tmp_fname : eps_fname,
            image_w, image_h, bitmap, image, num_lines, lines, epsf_level,
            &llx, &lly, &urx, &ury, write_date);
   }
   saveEPSLines = FALSE;

   if (!Embed && lines != NULL) {
      for (i=0; i < num_lines; i++) {
         if (lines[i] != NULL) free(lines[i]);
      }
      free(lines);
   }
   if (strcmp(defaultEPSScalingStr,"1") != 0) {
      ScaleAnEPSObj(obj_ptr, &defaultEPSScaling);
   }
   SetDefaultCursor(mainWindow);
   ShowCursor();

   AddObj(NULL, topObj, obj_ptr);
   if (psz_fname_spec == NULL) {
      PlaceTopObj(obj_ptr, NULL, NULL);
   } else {
      MoveObj(obj_ptr, 0-obj_ptr->obbox.ltx, 0-obj_ptr->obbox.lty);
   }
   SelectTopObj();
   RecordNewObjCmd();
   SetFileModified(TRUE);
   justDupped = FALSE;

   if (!importFromLibrary && !remote_file) SetCurImportDir(eps_fname);

   sprintf(gszMsgBox, TgLoadString(STID_GIVEN_EPS_FILE_IMPORTED),
         (short_name ? rest : eps_fname));
   Msg(gszMsgBox);
   if (remote_file) unlink(tmp_fname);
   importingFile = FALSE;
}

static char gifToXpmCmd[MAXSTRING+1];
static char pngToXpmCmd[MAXSTRING+1];
static char jpegToXpmCmd[MAXSTRING+1];
static char jpegToPpm6Cmd[MAXSTRING+1];
static char pngToPpm6Cmd[MAXSTRING+1];
static char gifToPpm6Cmd[MAXSTRING+1];
static char pbmToXbmCmd[MAXSTRING+1];
static char pgmToXpmCmd[MAXSTRING+1];
static char ppmToXpmCmd[MAXSTRING+1];
static char gifAnimExplodeCmd[MAXSTRING+1];

static char defGifToXpm[]="giftopnm %s | ppmtoxpm";
static char defPngToXpm[]=
      "pngtopnm %s | pnmdepth 255 | ppmquant 222 | ppmtoxpm";
static char defJpegToXpm[]="djpeg -gif -color 222 %s | giftopnm | ppmtoxpm";
static char defJpegToPpm6[]="djpeg -ppm \"%s\"";
static char defPngToPpm6[]="pngtopnm \"%s\"";
static char defGifToPpm6[]="giftopnm \"%s\"";
static char defPbmToXbm[]="pbmtoxbm %s";
static char defPgmToXpm[]="ppmtoxpm %s";
static char defPpmToXpm[]="ppmquant 222 %s | ppmtoxpm";

static
void InitGifToXpm()
{
   static int nInitialized=FALSE;

   if (!nInitialized) {
      char *c_ptr;

      nInitialized = TRUE;
      if (mainDisplay != NULL &&
            (c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"GifToXpm")) != NULL) {
         int count=0;

         strcpy(gifToXpmCmd, c_ptr);
         for (c_ptr=strstr(gifToXpmCmd,"%s"); c_ptr!=NULL;
               c_ptr=strstr(++c_ptr,"%s")) {
            count++;
         }
         if (count != 1) {
            sprintf(gszMsgBox, TgLoadString(STID_INVALID_XDEF_USE_ALT_STR),
                  TOOL_NAME, "GifToXpm", gifToXpmCmd, defGifToXpm);
            MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
            strcpy(gifToXpmCmd, defGifToXpm);
         }
      } else {
         strcpy(gifToXpmCmd, defGifToXpm);
      }
      if (mainDisplay != NULL &&
            (c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"PngToXpm")) != NULL) {
         int count=0;

         strcpy(pngToXpmCmd, c_ptr);
         for (c_ptr=strstr(pngToXpmCmd,"%s"); c_ptr!=NULL;
               c_ptr=strstr(++c_ptr,"%s")) {
            count++;
         }
         if (count != 1) {
            sprintf(gszMsgBox, TgLoadString(STID_INVALID_XDEF_USE_ALT_STR),
                  TOOL_NAME, "PngToXpm", pngToXpmCmd, defPngToXpm);
            MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
            strcpy(pngToXpmCmd, defPngToXpm);
         }
      } else {
         strcpy(pngToXpmCmd, defPngToXpm);
      }
      if (mainDisplay != NULL &&
            (c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"JpegToXpm")) != NULL) {
         int count=0;

         strcpy(jpegToXpmCmd, c_ptr);
         for (c_ptr=strstr(jpegToXpmCmd,"%s"); c_ptr!=NULL;
               c_ptr=strstr(++c_ptr,"%s")) {
            count++;
         }
         if (count != 1) {
            sprintf(gszMsgBox, TgLoadString(STID_INVALID_XDEF_USE_ALT_STR),
                  TOOL_NAME, "JpegToXpm", jpegToXpmCmd, defJpegToXpm);
            MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
            strcpy(jpegToXpmCmd, defJpegToXpm);
         }
      } else {
         strcpy(jpegToXpmCmd, defJpegToXpm);
      }
      if (mainDisplay != NULL &&
            (c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"JpegToPpm6")) != NULL) {
         int count=0;

         strcpy(jpegToPpm6Cmd, c_ptr);
         for (c_ptr=strstr(jpegToPpm6Cmd,"%s"); c_ptr!=NULL;
               c_ptr=strstr(++c_ptr,"%s")) {
            count++;
         }
         if (count != 1) {
            sprintf(gszMsgBox, TgLoadString(STID_INVALID_XDEF_USE_ALT_STR),
                  TOOL_NAME, "JpegToPpm6", jpegToPpm6Cmd, defJpegToPpm6);
            MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
            strcpy(jpegToPpm6Cmd, defJpegToPpm6);
         }
      } else {
         strcpy(jpegToPpm6Cmd, defJpegToPpm6);
      }
      if (mainDisplay != NULL &&
            (c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"PngToPpm6")) != NULL) {
         int count=0;

         strcpy(pngToPpm6Cmd, c_ptr);
         for (c_ptr=strstr(pngToPpm6Cmd,"%s"); c_ptr!=NULL;
               c_ptr=strstr(++c_ptr,"%s")) {
            count++;
         }
         if (count != 1) {
            sprintf(gszMsgBox, TgLoadString(STID_INVALID_XDEF_USE_ALT_STR),
                  TOOL_NAME, "PngToPpm6", pngToPpm6Cmd, defPngToPpm6);
            MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
            strcpy(pngToPpm6Cmd, defPngToPpm6);
         }
      } else {
         strcpy(pngToPpm6Cmd, defPngToPpm6);
      }
      if (mainDisplay != NULL &&
            (c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"GifToPpm6")) != NULL) {
         int count=0;

         strcpy(gifToPpm6Cmd, c_ptr);
         for (c_ptr=strstr(gifToPpm6Cmd,"%s"); c_ptr!=NULL;
               c_ptr=strstr(++c_ptr,"%s")) {
            count++;
         }
         if (count != 1) {
            sprintf(gszMsgBox, TgLoadString(STID_INVALID_XDEF_USE_ALT_STR),
                  TOOL_NAME, "GifToPpm6", gifToPpm6Cmd, defPngToPpm6);
            MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
            strcpy(gifToPpm6Cmd, defGifToPpm6);
         }
      } else {
         strcpy(gifToPpm6Cmd, defGifToPpm6);
      }
      if (mainDisplay != NULL &&
            (c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"PbmToXbm")) != NULL) {
         int count=0;

         strcpy(pbmToXbmCmd, c_ptr);
         for (c_ptr=strstr(pbmToXbmCmd,"%s"); c_ptr!=NULL;
               c_ptr=strstr(++c_ptr,"%s")) {
            count++;
         }
         if (count != 1) {
            sprintf(gszMsgBox, TgLoadString(STID_INVALID_XDEF_USE_ALT_STR),
                  TOOL_NAME, "PbmToXbm", pbmToXbmCmd, defPbmToXbm);
            MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
            strcpy(pbmToXbmCmd, defPbmToXbm);
         }
      } else {
         strcpy(pbmToXbmCmd, defPbmToXbm);
      }
      if (mainDisplay != NULL &&
            (c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"PgmToXpm")) != NULL) {
         int count=0;

         strcpy(pgmToXpmCmd, c_ptr);
         for (c_ptr=strstr(pgmToXpmCmd,"%s"); c_ptr!=NULL;
               c_ptr=strstr(++c_ptr,"%s")) {
            count++;
         }
         if (count != 1) {
            sprintf(gszMsgBox, TgLoadString(STID_INVALID_XDEF_USE_ALT_STR),
                  TOOL_NAME, "PgmToXpm", pgmToXpmCmd, defPgmToXpm);
            MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
            strcpy(pgmToXpmCmd, defPgmToXpm);
         }
      } else {
         strcpy(pgmToXpmCmd, defPgmToXpm);
      }
      if (mainDisplay != NULL &&
            (c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"PpmToXpm")) != NULL) {
         int count=0;

         strcpy(ppmToXpmCmd, c_ptr);
         for (c_ptr=strstr(ppmToXpmCmd,"%s"); c_ptr!=NULL;
               c_ptr=strstr(++c_ptr,"%s")) {
            count++;
         }
         if (count != 1) {
            sprintf(gszMsgBox, TgLoadString(STID_INVALID_XDEF_USE_ALT_STR),
                  TOOL_NAME, "PpmToXpm", ppmToXpmCmd, defPpmToXpm);
            MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
            strcpy(ppmToXpmCmd, defPpmToXpm);
         }
      } else {
         strcpy(ppmToXpmCmd, defPpmToXpm);
      }
      if (mainDisplay != NULL &&
            (c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"GifAnimExplode")) !=
            NULL) {
         strcpy(gifAnimExplodeCmd, c_ptr);
         UtilTrimBlanks(gifAnimExplodeCmd);
         if (strstr(gifAnimExplodeCmd, "%s") != NULL) {
            sprintf(gszMsgBox, TgLoadString(STID_INVALID_XDEF_USE_ALT_STR),
                  TOOL_NAME, "GifAnimExplode", gifAnimExplodeCmd,
                  "gifsicle -eU");
            MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
            strcpy(gifAnimExplodeCmd, "gifsicle -eU");
         }
      } else {
         strcpy(gifAnimExplodeCmd, "gifsicle -eU");
      }
   }
}

int ConvertGifToXpm(pszGifPath, pszXpmPath, xpm_path_sz)
   char *pszGifPath, *pszXpmPath;
   int xpm_path_sz;
{
   FILE *pFile=NULL, *pPipe=NULL;
   char *pszCmd=NULL, szBuf[MAXSTRING];
   int bytes_read=0, watch_cursor=watchCursorOnMainWindow;

   InitGifToXpm();
   if (MkTempFile(pszXpmPath, xpm_path_sz, tmpDir, TOOL_NAME) == NULL) {
      return FALSE;
   }
   pszCmd = (char*)malloc(
         (strlen(gifToXpmCmd)+strlen(pszGifPath)+10)*sizeof(char));
   if (pszCmd == NULL) {
      FailAllocMessage();
      return FALSE;
   }
   sprintf(pszCmd, gifToXpmCmd, pszGifPath);
   if (!FindProgramInPath(pszCmd, NULL, FALSE)) {
      free(pszCmd);
      return FALSE;
   }
   if ((pFile=fopen(pszXpmPath,"w")) == NULL) {
      sprintf(gszMsgBox, TgLoadString(STID_CANNOT_OPEN_FILE_FOR_WRITING),
            pszXpmPath);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      free(pszCmd);
      return FALSE;
   }
   sprintf(gszMsgBox, TgLoadCachedString(CSTID_EXECUTING_GIVEN_PROGRAM),
         pszCmd);
   SetStringStatus(gszMsgBox);
   XSync(mainDisplay, False);
   if ((pPipe=(FILE*)popen(pszCmd,"r")) == NULL) {
      sprintf(gszMsgBox, TgLoadString(STID_FAIL_TO_EXECUTE_CMD), pszCmd);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      free(pszCmd);
      fclose(pFile);
      unlink(pszXpmPath);
      return FALSE;
   }
   if (!watch_cursor) {
      SetWatchCursor(drawWindow);
      SetWatchCursor(mainWindow);
   }
   writeFileFailed = FALSE;
   while ((bytes_read=fread(szBuf, sizeof(char), sizeof(szBuf), pPipe)) > 0) {
      if ((int)fwrite(szBuf, sizeof(char), bytes_read, pFile) <= 0) {
         writeFileFailed = TRUE;
         break;
      }
   }
   pclose(pPipe);
   if (!watch_cursor) {
      SetDefaultCursor(mainWindow);
      ShowCursor();
   }
   SetStringStatus(TgLoadCachedString(CSTID_DOTS_DONE));
   free(pszCmd);
   fclose(pFile);
   if (writeFileFailed) {
      FailToWriteFileMessage(pszXpmPath);
      unlink(pszXpmPath);
      return FALSE;
   }
   return TRUE;
}

int ConvertPngToXpm(pszPngPath, pszXpmPath, xpm_path_sz)
   char *pszPngPath, *pszXpmPath;
   int xpm_path_sz;
{
   FILE *pFile=NULL, *pPipe=NULL;
   char *pszCmd=NULL, szBuf[MAXSTRING];
   int bytes_read=0, watch_cursor=watchCursorOnMainWindow;

   InitGifToXpm();
   if (*pszXpmPath == '\0' && xpm_path_sz > 0) {
      if (MkTempFile(pszXpmPath, xpm_path_sz, tmpDir, TOOL_NAME) == NULL) {
         return FALSE;
      }
   }
   pszCmd = (char*)malloc(
         (strlen(pngToXpmCmd)+strlen(pszPngPath)+10)*sizeof(char));
   if (pszCmd == NULL) {
      FailAllocMessage();
      return FALSE;
   }
   sprintf(pszCmd, pngToXpmCmd, pszPngPath);
   if (!FindProgramInPath(pszCmd, NULL, FALSE)) {
      free(pszCmd);
      return FALSE;
   }
   if ((pFile=fopen(pszXpmPath,"w")) == NULL) {
      sprintf(gszMsgBox, TgLoadString(STID_CANNOT_OPEN_FILE_FOR_WRITING),
            pszXpmPath);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      free(pszCmd);
      return FALSE;
   }
   sprintf(gszMsgBox, TgLoadCachedString(CSTID_EXECUTING_GIVEN_PROGRAM),
         pszCmd);
   SetStringStatus(gszMsgBox);
   XSync(mainDisplay, False);
   if ((pPipe=(FILE*)popen(pszCmd,"r")) == NULL) {
      sprintf(gszMsgBox, TgLoadString(STID_FAIL_TO_EXECUTE_CMD), pszCmd);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      free(pszCmd);
      fclose(pFile);
      unlink(pszXpmPath);
      return FALSE;
   }
   if (!watch_cursor) {
      SetWatchCursor(drawWindow);
      SetWatchCursor(mainWindow);
   }
   writeFileFailed = FALSE;
   while ((bytes_read=fread(szBuf, sizeof(char), sizeof(szBuf), pPipe)) > 0) {
      if ((int)fwrite(szBuf, sizeof(char), bytes_read, pFile) <= 0) {
         writeFileFailed = TRUE;
         break;
      }
   }
   pclose(pPipe);
   if (!watch_cursor) {
      SetDefaultCursor(mainWindow);
      ShowCursor();
   }
   SetStringStatus(TgLoadCachedString(CSTID_DOTS_DONE));
   free(pszCmd);
   fclose(pFile);
   if (writeFileFailed) {
      FailToWriteFileMessage(pszXpmPath);
      unlink(pszXpmPath);
      return FALSE;
   }
   return TRUE;
}

int ConvertJpegToXpm(pszJpegPath, pszXpmPath, xpm_path_sz)
   char *pszJpegPath, *pszXpmPath;
   int xpm_path_sz;
{
   FILE *pFile=NULL, *pPipe=NULL;
   char *pszCmd=NULL, szBuf[MAXSTRING];
   int bytes_read=0, watch_cursor=watchCursorOnMainWindow;

   InitGifToXpm();
   if (*pszXpmPath == '\0' && xpm_path_sz > 0) {
      if (MkTempFile(pszXpmPath, xpm_path_sz, tmpDir, TOOL_NAME) == NULL) {
         return FALSE;
      }
   }
   pszCmd = (char*)malloc(
         (strlen(jpegToXpmCmd)+strlen(pszJpegPath)+10)*sizeof(char));
   if (pszCmd == NULL) {
      FailAllocMessage();
      return FALSE;
   }
   sprintf(pszCmd, jpegToXpmCmd, pszJpegPath);
   if (!FindProgramInPath(pszCmd, NULL, FALSE)) {
      free(pszCmd);
      return FALSE;
   }
   if ((pFile=fopen(pszXpmPath,"w")) == NULL) {
      sprintf(gszMsgBox, TgLoadString(STID_CANNOT_OPEN_FILE_FOR_WRITING),
            pszXpmPath);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      free(pszCmd);
      return FALSE;
   }
   sprintf(gszMsgBox, TgLoadCachedString(CSTID_EXECUTING_GIVEN_PROGRAM),
         pszCmd);
   SetStringStatus(gszMsgBox);
   if (mainDisplay != NULL) XSync(mainDisplay, False);
   if ((pPipe=(FILE*)popen(pszCmd,"r")) == NULL) {
      sprintf(gszMsgBox, TgLoadString(STID_FAIL_TO_EXECUTE_CMD), pszCmd);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      free(pszCmd);
      fclose(pFile);
      unlink(pszXpmPath);
      return FALSE;
   }
   if (!watch_cursor) {
      SetWatchCursor(drawWindow);
      SetWatchCursor(mainWindow);
   }
   writeFileFailed = FALSE;
   while ((bytes_read=fread(szBuf, sizeof(char), sizeof(szBuf), pPipe)) > 0) {
      if ((int)fwrite(szBuf, sizeof(char), bytes_read, pFile) <= 0) {
         writeFileFailed = TRUE;
         break;
      }
   }
   pclose(pPipe);
   if (!watch_cursor) {
      SetDefaultCursor(mainWindow);
      ShowCursor();
   }
   SetStringStatus(TgLoadCachedString(CSTID_DOTS_DONE));
   free(pszCmd);
   fclose(pFile);
   if (writeFileFailed) {
      FailToWriteFileMessage(pszXpmPath);
      unlink(pszXpmPath);
      return FALSE;
   }
   return TRUE;
}

int ConvertImageFileToPpm6(pszImageFilePath, pszPpm6Path, ppm6_path_sz, format)
   char *pszImageFilePath, *pszPpm6Path;
   int ppm6_path_sz, format;
{
   FILE *pFile=NULL, *pPipe=NULL;
   char *pszCmd=NULL, szBuf[MAXSTRING], *psz_convert_cmd=NULL;
   int bytes_read=0, watch_cursor=watchCursorOnMainWindow;

   InitGifToXpm();
   if (MkTempFile(pszPpm6Path, ppm6_path_sz, tmpDir, TOOL_NAME) == NULL) {
      return FALSE;
   }
   switch (format) {
   case JPEG_FILE:
      if (PRTGIF && !cmdLineOpenDisplay && *cmdLineJpegToPpm6Cmd != '\0') {
         psz_convert_cmd = cmdLineJpegToPpm6Cmd;
      } else {
         psz_convert_cmd = jpegToPpm6Cmd;
      }
      break;
   case PNG_FILE:
      psz_convert_cmd = pngToPpm6Cmd;
      break;
   case GIF_FILE:
      psz_convert_cmd = gifToPpm6Cmd;
      break;
   }
   pszCmd = (char*)malloc(
         (strlen(psz_convert_cmd)+strlen(pszImageFilePath)+10)*sizeof(char));
   if (pszCmd == NULL) {
      FailAllocMessage();
      return FALSE;
   }
   sprintf(pszCmd, psz_convert_cmd, pszImageFilePath);
   if (!FindProgramInPath(pszCmd, NULL, FALSE)) {
      free(pszCmd);
      return FALSE;
   }
   if ((pFile=fopen(pszPpm6Path,"w")) == NULL) {
      sprintf(gszMsgBox, TgLoadString(STID_CANNOT_OPEN_FILE_FOR_WRITING),
            pszPpm6Path);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      free(pszCmd);
      return FALSE;
   }
   sprintf(gszMsgBox, TgLoadCachedString(CSTID_EXECUTING_GIVEN_PROGRAM),
         pszCmd);
   SetStringStatus(gszMsgBox);
   if (mainDisplay != NULL) XSync(mainDisplay, False);
   if ((pPipe=(FILE*)popen(pszCmd,"r")) == NULL) {
      sprintf(gszMsgBox, TgLoadString(STID_FAIL_TO_EXECUTE_CMD), pszCmd);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      free(pszCmd);
      fclose(pFile);
      unlink(pszPpm6Path);
      return FALSE;
   }
   if (!watch_cursor) {
      SetWatchCursor(drawWindow);
      SetWatchCursor(mainWindow);
   }
   writeFileFailed = FALSE;
   while ((bytes_read=fread(szBuf, sizeof(char), sizeof(szBuf), pPipe)) > 0) {
      if ((int)fwrite(szBuf, sizeof(char), bytes_read, pFile) <= 0) {
         writeFileFailed = TRUE;
         break;
      }
   }
   pclose(pPipe);
   if (!watch_cursor) {
      SetDefaultCursor(mainWindow);
      ShowCursor();
   }
   SetStringStatus(TgLoadCachedString(CSTID_DOTS_DONE));
   free(pszCmd);
   fclose(pFile);
   if (writeFileFailed) {
      FailToWriteFileMessage(pszPpm6Path);
      unlink(pszPpm6Path);
      return FALSE;
   }
   return TRUE;
}

int ConvertJpegToPpm6(pszJpegPath, pszPpm6Path, ppm6_path_sz)
   char *pszJpegPath, *pszPpm6Path;
   int ppm6_path_sz;
{
   return ConvertImageFileToPpm6(pszJpegPath, pszPpm6Path, ppm6_path_sz,
         JPEG_FILE);
}

int ConvertPngToPpm6(pszPngPath, pszPpm6Path, ppm6_path_sz)
   char *pszPngPath, *pszPpm6Path;
   int ppm6_path_sz;
{
   return ConvertImageFileToPpm6(pszPngPath, pszPpm6Path, ppm6_path_sz,
         PNG_FILE);
}

int ConvertGifToPpm6(pszGifPath, pszPpm6Path, ppm6_path_sz)
   char *pszGifPath, *pszPpm6Path;
   int ppm6_path_sz;
{
   return ConvertImageFileToPpm6(pszGifPath, pszPpm6Path, ppm6_path_sz,
         GIF_FILE);
}

#define IMPORTING_JPEG 0
#define IMPORTING_PPM  1
#define IMPORTING_PNG  2

static
int FinishImport(remote_file, remote_tmp_fname, local_fname, which, pn_image_w,
      pn_image_h)
   int remote_file, *pn_image_w, *pn_image_h;
   char *remote_tmp_fname, *local_fname;
{
   char xpm_fname[MAXPATHLENGTH+1], *rest=NULL, *psz_format=NULL;
   int rc=0, short_name=FALSE, image_w=0, image_h=0;
   XEvent ev;

   switch (which) {
   case IMPORTING_JPEG: psz_format = "JPEG"; break;
   case IMPORTING_PPM: psz_format = "PPM"; break;
   case IMPORTING_PNG: psz_format = "PNG"; break;
   }
   XSync(mainDisplay, False);
   if (XCheckMaskEvent(mainDisplay, ExposureMask, &ev)) {
      ExposeEventHandler(&ev, TRUE);
   }
   if (remote_file) {
      short_name = FALSE;
   } else {
      if ((short_name=IsPrefix(bootDir, local_fname, &rest))) ++rest;
   }
   SetWatchCursor(drawWindow);
   SetWatchCursor(mainWindow);
   SaveStatusStrings();
   if (fullTrueColorMode && HasZlibSupport()) {
      struct ObjRec *obj_ptr=NULL;
      struct XPmRec *xpm_ptr=NULL;
      char ppm6_fname[MAXPATHLENGTH+1];
      char deflated_fname[MAXPATHLENGTH+1];

      switch (which) {
      case IMPORTING_JPEG:
         rc = ConvertJpegToPpm6((remote_file ? remote_tmp_fname : local_fname),
               ppm6_fname, sizeof(ppm6_fname));
         if (rc) {
            ResetPngHeaderInfo(&gPngHeaderInfo);
            obj_ptr = CreatePpmTrueObjFromFile(ppm6_fname);
            unlink(ppm6_fname);
         }
         if (obj_ptr == NULL) {
            RestoreStatusStrings();
            SetDefaultCursor(mainWindow);
            ShowCursor();
            if (remote_file) unlink(remote_tmp_fname);
            importingFile = FALSE;
            return FALSE;
         }
         xpm_ptr = obj_ptr->detail.xpm;
         xpm_ptr->real_type = PPM_TRUE;
         xpm_ptr->ppm_data_compress = PPM_JPEG_COMPRESS;
         xpm_ptr->ppm_data = ReadFileIntoBuf(
               (remote_file ? remote_tmp_fname : local_fname),
               &xpm_ptr->ppm_data_size);
         xpm_ptr->ppm_mask_data = NULL;
         xpm_ptr->ppm_mask_size = 0;
         break;
      case IMPORTING_PPM:
         ResetPngHeaderInfo(&gPngHeaderInfo);
         obj_ptr = CreatePpmTrueObjFromFile(remote_file ? remote_tmp_fname :
               local_fname);
         if (obj_ptr != NULL &&
               MkTempFile(deflated_fname, sizeof(deflated_fname), tmpDir,
               TOOL_NAME) != NULL &&
               DeflateFile((remote_file ? remote_tmp_fname : local_fname),
               deflated_fname)) {
         } else {
            if (obj_ptr != NULL) FreeObj(obj_ptr);
            RestoreStatusStrings();
            SetDefaultCursor(mainWindow);
            ShowCursor();
            if (remote_file) unlink(remote_tmp_fname);
            importingFile = FALSE;
            return FALSE;
         }
         xpm_ptr = obj_ptr->detail.xpm;
         xpm_ptr->real_type = PPM_TRUE;
         xpm_ptr->ppm_data_compress = PPM_DATA_DEFLATED;
         xpm_ptr->ppm_data = ReadFileIntoBuf(deflated_fname,
               &xpm_ptr->ppm_data_size);
         xpm_ptr->ppm_mask_data = NULL;
         xpm_ptr->ppm_mask_size = 0;
         unlink(deflated_fname);
         break;
      case IMPORTING_PNG:
         rc = ConvertPngToPpm6((remote_file ? remote_tmp_fname : local_fname),
               ppm6_fname, sizeof(ppm6_fname));
         if (rc) {
            ResetPngHeaderInfo(&gPngHeaderInfo);
            if (PngFileGetTransColorInit(&gPngHeaderInfo,
                  (remote_file ? remote_tmp_fname : local_fname)) &&
                  PngFileGetTransColor(&gPngHeaderInfo)) {
            }
            obj_ptr = CreatePpmTrueObjFromFile(ppm6_fname);
         }
         if (obj_ptr != NULL &&
               MkTempFile(deflated_fname, sizeof(deflated_fname), tmpDir,
               TOOL_NAME) != NULL && DeflateFile(ppm6_fname, deflated_fname)) {
            /* nothing to do here */
         } else {
            unlink(ppm6_fname);
            if (obj_ptr != NULL) FreeObj(obj_ptr);
            RestoreStatusStrings();
            SetDefaultCursor(mainWindow);
            ShowCursor();
            if (remote_file) unlink(remote_tmp_fname);
            importingFile = FALSE;
            return FALSE;
         }
         unlink(ppm6_fname);
         xpm_ptr = obj_ptr->detail.xpm;
         xpm_ptr->real_type = PPM_TRUE;
         xpm_ptr->ppm_data_compress = PPM_DATA_DEFLATED;
         xpm_ptr->ppm_data = ReadFileIntoBuf(deflated_fname,
               &xpm_ptr->ppm_data_size);
         xpm_ptr->ppm_mask_data = NULL;
         xpm_ptr->ppm_mask_size = 0;
         if (gPngHeaderInfo.valid && gPngHeaderInfo.trans_color_pixel_found) {
            xpm_ptr->has_transparent_color = TRUE;
            xpm_ptr->transparent_color[0] = gPngHeaderInfo.trans_color_pixel_red;
            xpm_ptr->transparent_color[1] = gPngHeaderInfo.trans_color_pixel_green;
            xpm_ptr->transparent_color[2] = gPngHeaderInfo.trans_color_pixel_blue;
         }
         unlink(deflated_fname);
         break;
      }
      RestoreStatusStrings();
      SetDefaultCursor(mainWindow);
      ShowCursor();
      if (obj_ptr == NULL) {
         sprintf(gszMsgBox, TgLoadString(STID_CANT_IMPORT_GIVEN_TYPE_FILE),
               psz_format, (short_name ? rest : local_fname));
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
         if (remote_file) unlink(remote_tmp_fname);
         importingFile = FALSE;
         return FALSE;
      }
      AddObj(NULL, topObj, obj_ptr);
      PlaceTopObj(obj_ptr, NULL, NULL);
   
      SelectTopObj();
      RecordNewObjCmd();
      SetFileModified(TRUE);
      justDupped = FALSE;

      if (!importFromLibrary && !remote_file) {
         SetCurImportDir(local_fname);
      }
      if (pn_image_w != NULL) *pn_image_w = xpm_ptr->image_w;
      if (pn_image_h != NULL) *pn_image_h = xpm_ptr->image_h;

      return TRUE;
   }
   *xpm_fname = '\0';
   switch (which) {
   case IMPORTING_JPEG:
      rc = ConvertJpegToXpm((remote_file ? remote_tmp_fname : local_fname),
            xpm_fname, sizeof(xpm_fname));
      break;
   case IMPORTING_PPM:
      rc = ConvertPpmToXpm((remote_file ? remote_tmp_fname : local_fname),
            xpm_fname, sizeof(xpm_fname));
      break;
   case IMPORTING_PNG:
      rc = ConvertPngToXpm((remote_file ? remote_tmp_fname : local_fname),
            xpm_fname, sizeof(xpm_fname));
      break;
   }
   RestoreStatusStrings();
   SetDefaultCursor(mainWindow);
   ShowCursor();
   if (!rc) {
      if (remote_file) unlink(remote_tmp_fname);
      importingFile = FALSE;
      return FALSE;
   }
   if (!ImportGivenXPixmapFile(FALSE, NULL, xpm_fname, INVALID, INVALID,
         &image_w, &image_h)) {
      sprintf(gszMsgBox, TgLoadString(STID_CANT_IMPORT_GIVEN_TYPE_FILE),
            psz_format, (short_name ? rest : local_fname));
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      unlink(xpm_fname);
      if (remote_file) unlink(remote_tmp_fname);
      importingFile = FALSE;
      return FALSE;
   }
   unlink(xpm_fname);

   return TRUE;
}

void ImportPNGFile()
{
   char *rest=NULL, tmp_fname[MAXPATHLENGTH+1];
   char szPngPath[MAXPATHLENGTH+1];
   int short_name=FALSE, image_w=0, image_h=0, remote_file=FALSE;

   MakeQuiescent();

   importingFile = TRUE;
   *szPngPath = *tmp_fname = '\0';
   if (importFromLibrary) {
      char name[MAXSTRING+1], path[MAXSTRING+1];

      if (SelectFromLibrary(TgLoadString(STID_SEL_PNG_FILE_TO_IMPORT),
            "png", name, path) == INVALID) {
         importingFile = FALSE;
         return;
      }
      sprintf(szPngPath, "%s%c%s", path, DIR_SEP, name);
   } else if (SelectFileNameToImport(TgLoadString(STID_SEL_PNG_FILE_TO_IMPORT),
         "png", szPngPath) == INVALID) {
      importingFile = FALSE;
      return;
   } else if (FileIsRemote(szPngPath)) {
      int is_html=FALSE;

      if (!DownloadRemoteFile(szPngPath, NULL, NULL, &is_html, tmp_fname, NULL,
           0) || *tmp_fname == '\0') {
         importingFile = FALSE;
         return;
      }
      remote_file = TRUE;
   }
   if (!FinishImport(remote_file, tmp_fname, szPngPath, IMPORTING_PNG,
         &image_w, &image_h)) {
      if (remote_file) unlink(tmp_fname);
      importingFile = FALSE;
      return;
   }
   if (!importFromLibrary && !remote_file) SetCurImportDir(szPngPath);

   sprintf(gszMsgBox, TgLoadString(STID_GIVEN_PNG_SIZE_FILE_IMPORTED),
         image_w, image_h, (short_name ? rest : szPngPath));
   Msg(gszMsgBox);

   if (remote_file) unlink(tmp_fname);
   importingFile = FALSE;
}

void ImportJPEGFile(Embed, psz_fname_spec)
   int Embed;
   char *psz_fname_spec;
{
   char *rest=NULL, tmp_fname[MAXPATHLENGTH+1];
   char szJpegPath[MAXPATHLENGTH+1];
   int short_name=FALSE, image_w=0, image_h=0, remote_file=FALSE;

   MakeQuiescent();

   importingFile = TRUE;
   *szJpegPath = *tmp_fname = '\0';
   if (importFromLibrary) {
      char name[MAXSTRING+1], path[MAXSTRING+1];

      if (SelectFromLibrary(TgLoadString(STID_SEL_JPEG_FILE_TO_IMPORT),
            "jpeg", name, path) == INVALID) {
         importingFile = FALSE;
         return;
      }
      sprintf(szJpegPath, "%s%c%s", path, DIR_SEP, name);
   } else if (SelectFileNameToImport(TgLoadString(Embed ?
         STID_SEL_JPEG_FILE_TO_IMPORT : STID_SEL_JPEG_FILE_TO_LINKTO),
         "jpeg", szJpegPath) == INVALID) {
      importingFile = FALSE;
      return;
   } else if (FileIsRemote(szJpegPath)) {
      int is_html=FALSE;

      if (!Embed) {
         sprintf(gszMsgBox, TgLoadString(STID_IMPORT_REM_FILE_EMBED_INSTEAD),
               "JPEG", "JPEG");
         if (MsgBox(gszMsgBox, TOOL_NAME, YNC_MB) != MB_ID_YES) {
            importingFile = FALSE;
            return;
         }
         Embed = TRUE;
      }
      if (!DownloadRemoteFile(szJpegPath, NULL, NULL, &is_html, tmp_fname, NULL,
           0) || *tmp_fname == '\0') {
         importingFile = FALSE;
         return;
      }
      remote_file = TRUE;
   }
   if (!FinishImport(remote_file, tmp_fname, szJpegPath, IMPORTING_JPEG,
         &image_w, &image_h)) {
      if (remote_file) unlink(tmp_fname);
      importingFile = FALSE;
      return;
   }
   if (!Embed) {
      topObj->detail.xpm->real_type = XPM_JPEG;
      topObj->detail.xpm->linked_jpeg = TRUE;
      topObj->detail.xpm->filename = UtilStrDup(short_name ? rest : szJpegPath);
   }
   if (!importFromLibrary && !remote_file) SetCurImportDir(szJpegPath);

   sprintf(gszMsgBox, TgLoadString(STID_GIVEN_JPEG_SIZE_FILE_IMPORTED),
         image_w, image_h, (short_name ? rest : szJpegPath));
   Msg(gszMsgBox);

   if (remote_file) unlink(tmp_fname);
   importingFile = FALSE;
}

void ImportGIFFile()
{
   char file_name[MAXPATHLENGTH+1], *rest=NULL, tmp_fname[MAXPATHLENGTH+1];
   char szGifPath[MAXPATHLENGTH+1];
   int rc, short_name=FALSE, image_w=0, image_h=0, remote_file=FALSE;
   XEvent ev;

   MakeQuiescent();

   importingFile = TRUE;
   *szGifPath = *tmp_fname = *file_name = '\0';
   if (importFromLibrary) {
      char name[MAXSTRING+1], path[MAXSTRING+1];

      if (SelectFromLibrary(TgLoadString(STID_SEL_GIF_FILE_TO_IMPORT),
            "gif", name, path) == INVALID) {
         importingFile = FALSE;
         return;
      }
      sprintf(szGifPath, "%s%c%s", path, DIR_SEP, name);
   } else if (SelectFileNameToImport(TgLoadString(STID_SEL_GIF_FILE_TO_IMPORT),
         "gif", szGifPath) == INVALID) {
      importingFile = FALSE;
      return;
   } else if (FileIsRemote(szGifPath)) {
      int is_html=FALSE;

      if (!DownloadRemoteFile(szGifPath, NULL, NULL, &is_html, tmp_fname, NULL,
           0) || *tmp_fname == '\0') {
         importingFile = FALSE;
         return;
      }
      remote_file = TRUE;
   }
   XSync(mainDisplay, False);
   if (XCheckMaskEvent(mainDisplay, ExposureMask, &ev)) {
      ExposeEventHandler(&ev, TRUE);
   }
   SetWatchCursor(drawWindow);
   SetWatchCursor(mainWindow);
   SaveStatusStrings();
   if (fullTrueColorMode && HasZlibSupport()) {
      struct ObjRec *obj_ptr=NULL;
      struct XPmRec *xpm_ptr=NULL;
      char ppm6_fname[MAXPATHLENGTH+1];
      char deflated_fname[MAXPATHLENGTH+1];

      rc = ConvertGifToPpm6((remote_file ? tmp_fname : szGifPath),
            ppm6_fname, sizeof(ppm6_fname));
      if (rc) {
         ResetPngHeaderInfo(&gPngHeaderInfo);
         if (PngFileGetTransColorInit(&gPngHeaderInfo,
               (remote_file ? tmp_fname : szGifPath)) &&
               PngFileGetTransColor(&gPngHeaderInfo)) {
         }
         obj_ptr = CreatePpmTrueObjFromFile(ppm6_fname);
      }
      if (obj_ptr != NULL &&
            MkTempFile(deflated_fname, sizeof(deflated_fname), tmpDir,
            TOOL_NAME) != NULL && DeflateFile(ppm6_fname, deflated_fname)) {
         /* nothing to do here */
      } else {
         unlink(ppm6_fname);
         if (obj_ptr != NULL) FreeObj(obj_ptr);
         RestoreStatusStrings();
         SetDefaultCursor(mainWindow);
         ShowCursor();
         if (remote_file) unlink(tmp_fname);
         importingFile = FALSE;
         return;
      }
      unlink(ppm6_fname);
      xpm_ptr = obj_ptr->detail.xpm;
      xpm_ptr->real_type = PPM_TRUE;
      xpm_ptr->ppm_data_compress = PPM_DATA_DEFLATED;
      xpm_ptr->ppm_data = ReadFileIntoBuf(deflated_fname,
            &xpm_ptr->ppm_data_size);
      xpm_ptr->ppm_mask_data = NULL;
      xpm_ptr->ppm_mask_size = 0;
      if (gPngHeaderInfo.valid && gPngHeaderInfo.trans_color_pixel_found) {
         xpm_ptr->has_transparent_color = TRUE;
         xpm_ptr->transparent_color[0] = gPngHeaderInfo.trans_color_pixel_red;
         xpm_ptr->transparent_color[1] = gPngHeaderInfo.trans_color_pixel_green;
         xpm_ptr->transparent_color[2] = gPngHeaderInfo.trans_color_pixel_blue;
      }
      unlink(deflated_fname);

      RestoreStatusStrings();
      SetDefaultCursor(mainWindow);
      ShowCursor();
      if (obj_ptr == NULL) {
         sprintf(gszMsgBox, TgLoadString(STID_CANNOT_IMPORT_GIVEN_GIF),
               (short_name ? rest : szGifPath));
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
         if (remote_file) unlink(tmp_fname);
         importingFile = FALSE;
         return;
      }
      AddObj(NULL, topObj, obj_ptr);
      PlaceTopObj(obj_ptr, NULL, NULL);

      SelectTopObj();
      RecordNewObjCmd();
      SetFileModified(TRUE);
      justDupped = FALSE;

      if (!importFromLibrary && !remote_file) {
         SetCurImportDir(szGifPath);
      }
      sprintf(gszMsgBox, TgLoadString(STID_GIVEN_GIF_SIZE_FILE_IMPORTED),
            xpm_ptr->image_w, xpm_ptr->image_h,
            (short_name ? rest : szGifPath));
      Msg(gszMsgBox);

      return;
   }
   *file_name = '\0';
   rc = ConvertGifToXpm((remote_file ? tmp_fname : szGifPath), file_name,
         sizeof(file_name));
   RestoreStatusStrings();
   SetDefaultCursor(mainWindow);
   ShowCursor();
   if (!rc) {
      if (remote_file) unlink(tmp_fname);
      importingFile = FALSE;
      return;
   }
   if (remote_file) {
      short_name = FALSE;
   } else {
      if ((short_name=IsPrefix(bootDir, szGifPath, &rest))) ++rest;
   }
   if (!ImportGivenXPixmapFile(FALSE, NULL, file_name, INVALID, INVALID,
         &image_w, &image_h)) {
      sprintf(gszMsgBox, TgLoadString(STID_CANNOT_IMPORT_GIVEN_GIF),
            (short_name ? rest : szGifPath));
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      unlink(file_name);
      if (remote_file) unlink(tmp_fname);
      importingFile = FALSE;
      return;
   }
   unlink(file_name);

   if (!importFromLibrary && !remote_file) SetCurImportDir(szGifPath);

   sprintf(gszMsgBox, TgLoadString(STID_GIVEN_GIF_SIZE_FILE_IMPORTED),
         image_w, image_h, (short_name ? rest : szGifPath));
   Msg(gszMsgBox);

   if (remote_file) unlink(tmp_fname);
   importingFile = FALSE;
}

int ConvertPbmToXbm(pszPbmPath, pszXbmPath, xbm_path_sz)
   char *pszPbmPath, *pszXbmPath;
   int xbm_path_sz;
{
   FILE *pFile=NULL, *pPipe=NULL;
   char *pszCmd=NULL, szBuf[MAXSTRING];
   int bytes_read=0, watch_cursor=watchCursorOnMainWindow;;

   InitGifToXpm();
   if (MkTempFile(pszXbmPath, xbm_path_sz, tmpDir, TOOL_NAME) == NULL) {
      return FALSE;
   }
   pszCmd = (char*)malloc(
         (strlen(pbmToXbmCmd)+strlen(pszPbmPath)+10)*sizeof(char));
   if (pszCmd == NULL) {
      FailAllocMessage();
      return FALSE;
   }
   sprintf(pszCmd, pbmToXbmCmd, pszPbmPath);
   if (!FindProgramInPath(pszCmd, NULL, FALSE)) {
      free(pszCmd);
      return FALSE;
   }
   if ((pFile=fopen(pszXbmPath,"w")) == NULL) {
      sprintf(gszMsgBox, TgLoadString(STID_CANNOT_OPEN_FILE_FOR_WRITING),
            pszXbmPath);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      free(pszCmd);
      return FALSE;
   }
   sprintf(gszMsgBox, TgLoadCachedString(CSTID_EXECUTING_GIVEN_PROGRAM),
         pszCmd);
   SetStringStatus(gszMsgBox);
   XSync(mainDisplay, False);
   if ((pPipe=(FILE*)popen(pszCmd,"r")) == NULL) {
      sprintf(gszMsgBox, TgLoadString(STID_FAIL_TO_EXECUTE_CMD), pszCmd);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      free(pszCmd);
      fclose(pFile);
      unlink(pszXbmPath);
      return FALSE;
   }
   if (!watch_cursor) {
      SetWatchCursor(drawWindow);
      SetWatchCursor(mainWindow);
   }
   writeFileFailed = FALSE;
   while ((bytes_read=fread(szBuf, sizeof(char), sizeof(szBuf), pPipe)) > 0) {
      if ((int)fwrite(szBuf, sizeof(char), bytes_read, pFile) <= 0) {
         writeFileFailed = TRUE;
         break;
      }
   }
   pclose(pPipe);
   if (!watch_cursor) {
      SetDefaultCursor(mainWindow);
      ShowCursor();
   }
   SetStringStatus(TgLoadCachedString(CSTID_DOTS_DONE));
   free(pszCmd);
   fclose(pFile);
   if (writeFileFailed) {
      FailToWriteFileMessage(pszXbmPath);
      unlink(pszXbmPath);
      return FALSE;
   }
   return TRUE;
}

void ImportPBMFile()
{
   char file_name[MAXPATHLENGTH+1], *rest=NULL, tmp_fname[MAXPATHLENGTH+1];
   char szPbmPath[MAXPATHLENGTH+1];
   int rc, short_name=FALSE, image_w=0, image_h=0, remote_file=FALSE;
   XEvent ev;

   MakeQuiescent();

   importingFile = TRUE;
   *szPbmPath = *tmp_fname = '\0';
   if (importFromLibrary) {
      char name[MAXSTRING+1], path[MAXSTRING+1];

      if (SelectFromLibrary(TgLoadString(STID_SEL_PBM_FILE_TO_IMPORT),
            "pbm", name, path) == INVALID) {
         importingFile = FALSE;
         return;
      }
      sprintf(szPbmPath, "%s%c%s", path, DIR_SEP, name);
   } else if (SelectFileNameToImport(TgLoadString(STID_SEL_PBM_FILE_TO_IMPORT),
         "pbm", szPbmPath) == INVALID) {
      importingFile = FALSE;
      return;
   } else if (FileIsRemote(szPbmPath)) {
      int is_html=FALSE;

      if (!DownloadRemoteFile(szPbmPath, NULL, NULL, &is_html, tmp_fname, NULL,
           0) || *tmp_fname == '\0') {
         importingFile = FALSE;
         return;
      }
      remote_file = TRUE;
   }
   XSync(mainDisplay, False);
   if (XCheckMaskEvent(mainDisplay, ExposureMask, &ev)) {
      ExposeEventHandler(&ev, TRUE);
   }
   SetWatchCursor(drawWindow);
   SetWatchCursor(mainWindow);
   SaveStatusStrings();
   rc = ConvertPbmToXbm((remote_file ? tmp_fname : szPbmPath), file_name,
         sizeof(file_name));
   RestoreStatusStrings();
   SetDefaultCursor(mainWindow);
   ShowCursor();
   if (!rc) {
      if (remote_file) unlink(tmp_fname);
      importingFile = FALSE;
      return;
   }
   if (remote_file) {
      short_name = FALSE;
   } else {
      if ((short_name=IsPrefix(bootDir, szPbmPath, &rest))) ++rest;
   }
   if (!ImportGivenXBitmapFile(FALSE, NULL, file_name, INVALID, INVALID,
         &image_w, &image_h)) {
      sprintf(gszMsgBox, TgLoadString(STID_CANNOT_IMPORT_GIVEN_PBM),
            (short_name ? rest : szPbmPath));
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      unlink(file_name);
      if (remote_file) unlink(tmp_fname);
      importingFile = FALSE;
      return;
   }
   unlink(file_name);

   if (!importFromLibrary && !remote_file) SetCurImportDir(szPbmPath);

   sprintf(gszMsgBox, TgLoadString(STID_GIVEN_PBM_SIZE_FILE_IMPORTED),
         image_w, image_h, (short_name ? rest : szPbmPath));
   Msg(gszMsgBox);

   if (remote_file) unlink(tmp_fname);
   importingFile = FALSE;
}

int ConvertPgmToXpm(pszPgmPath, pszXpmPath, xpm_path_sz)
   char *pszPgmPath, *pszXpmPath;
   int xpm_path_sz;
{
   FILE *pFile=NULL, *pPipe=NULL;
   char *pszCmd=NULL, szBuf[MAXSTRING];
   int bytes_read=0, watch_cursor=watchCursorOnMainWindow;

   InitGifToXpm();
   if (MkTempFile(pszXpmPath, xpm_path_sz, tmpDir, TOOL_NAME) == NULL) {
      return FALSE;
   }
   pszCmd = (char*)malloc(
         (strlen(pgmToXpmCmd)+strlen(pszPgmPath)+10)*sizeof(char));
   if (pszCmd == NULL) {
      FailAllocMessage();
      return FALSE;
   }
   sprintf(pszCmd, pgmToXpmCmd, pszPgmPath);
   if (!FindProgramInPath(pszCmd, NULL, FALSE)) {
      free(pszCmd);
      return FALSE;
   }
   if ((pFile=fopen(pszXpmPath,"w")) == NULL) {
      sprintf(gszMsgBox, TgLoadString(STID_CANNOT_OPEN_FILE_FOR_WRITING),
            pszXpmPath);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      free(pszCmd);
      return FALSE;
   }
   sprintf(gszMsgBox, TgLoadCachedString(CSTID_EXECUTING_GIVEN_PROGRAM),
         pszCmd);
   SetStringStatus(gszMsgBox);
   XSync(mainDisplay, False);
   if ((pPipe=(FILE*)popen(pszCmd,"r")) == NULL) {
      sprintf(gszMsgBox, TgLoadString(STID_FAIL_TO_EXECUTE_CMD), pszCmd);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      free(pszCmd);
      fclose(pFile);
      unlink(pszXpmPath);
      return FALSE;
   }
   if (!watch_cursor) {
      SetWatchCursor(drawWindow);
      SetWatchCursor(mainWindow);
   }
   writeFileFailed = FALSE;
   while ((bytes_read=fread(szBuf, sizeof(char), sizeof(szBuf), pPipe)) > 0) {
      if ((int)fwrite(szBuf, sizeof(char), bytes_read, pFile) <= 0) {
         writeFileFailed = TRUE;
         break;
      }
   }
   pclose(pPipe);
   if (!watch_cursor) {
      SetDefaultCursor(mainWindow);
      ShowCursor();
   }
   SetStringStatus(TgLoadCachedString(CSTID_DOTS_DONE));
   free(pszCmd);
   fclose(pFile);
   if (writeFileFailed) {
      FailToWriteFileMessage(pszXpmPath);
      unlink(pszXpmPath);
      return FALSE;
   }
   return TRUE;
}

void ImportPGMFile()
{
   char file_name[MAXPATHLENGTH+1], *rest=NULL, tmp_fname[MAXPATHLENGTH+1];
   char szPgmPath[MAXPATHLENGTH+1];
   int rc, short_name=FALSE, image_w=0, image_h=0, remote_file=FALSE;
   XEvent ev;

   MakeQuiescent();

   importingFile = TRUE;
   *szPgmPath = *tmp_fname = '\0';
   if (importFromLibrary) {
      char name[MAXSTRING+1], path[MAXSTRING+1];

      if (SelectFromLibrary(TgLoadString(STID_SEL_PGM_FILE_TO_IMPORT),
            "pgm", name, path) == INVALID) {
         importingFile = FALSE;
         return;
      }
      sprintf(szPgmPath, "%s%c%s", path, DIR_SEP, name);
   } else if (SelectFileNameToImport(TgLoadString(STID_SEL_PGM_FILE_TO_IMPORT),
         "pgm", szPgmPath) == INVALID) {
      importingFile = FALSE;
      return;
   } else if (FileIsRemote(szPgmPath)) {
      int is_html=FALSE;

      if (!DownloadRemoteFile(szPgmPath, NULL, NULL, &is_html, tmp_fname, NULL,
           0) || *tmp_fname == '\0') {
         importingFile = FALSE;
         return;
      }
      remote_file = TRUE;
   }
   XSync(mainDisplay, False);
   if (XCheckMaskEvent(mainDisplay, ExposureMask, &ev)) {
      ExposeEventHandler(&ev, TRUE);
   }
   SetWatchCursor(drawWindow);
   SetWatchCursor(mainWindow);
   SaveStatusStrings();
   rc = ConvertPgmToXpm((remote_file ? tmp_fname : szPgmPath), file_name,
         sizeof(file_name));
   RestoreStatusStrings();
   SetDefaultCursor(mainWindow);
   ShowCursor();
   if (!rc) {
      if (remote_file) unlink(tmp_fname);
      importingFile = FALSE;
      return;
   }
   if (remote_file) {
      short_name = FALSE;
   } else {
      if ((short_name=IsPrefix(bootDir, szPgmPath, &rest))) ++rest;
   }
   if (!ImportGivenXPixmapFile(FALSE, NULL, file_name, INVALID, INVALID,
         &image_w, &image_h)) {
      sprintf(gszMsgBox, TgLoadString(STID_CANNOT_IMPORT_GIVEN_PGM),
            (short_name ? rest : szPgmPath));
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      unlink(file_name);
      if (remote_file) unlink(tmp_fname);
      importingFile = FALSE;
      return;
   }
   unlink(file_name);

   if (!importFromLibrary && !remote_file) SetCurImportDir(szPgmPath);

   sprintf(gszMsgBox, TgLoadString(STID_GIVEN_PGM_SIZE_FILE_IMPORTED),
         image_w, image_h, (short_name ? rest : szPgmPath));
   Msg(gszMsgBox);

   if (remote_file) unlink(tmp_fname);
   importingFile = FALSE;
}

int ConvertPpmToXpm(pszPpmPath, pszXpmPath, xpm_path_sz)
   char *pszPpmPath, *pszXpmPath;
   int xpm_path_sz;
{
   FILE *pFile=NULL, *pPipe=NULL;
   char *pszCmd=NULL, szBuf[MAXSTRING];
   int bytes_read=0, watch_cursor=watchCursorOnMainWindow;

   InitGifToXpm();
   if (MkTempFile(pszXpmPath, xpm_path_sz, tmpDir, TOOL_NAME) == NULL) {
      return FALSE;
   }
   pszCmd = (char*)malloc(
         (strlen(ppmToXpmCmd)+strlen(pszPpmPath)+10)*sizeof(char));
   if (pszCmd == NULL) {
      FailAllocMessage();
      return FALSE;
   }
   sprintf(pszCmd, ppmToXpmCmd, pszPpmPath);
   if (!FindProgramInPath(pszCmd, NULL, FALSE)) {
      free(pszCmd);
      return FALSE;
   }
   if ((pFile=fopen(pszXpmPath,"w")) == NULL) {
      sprintf(gszMsgBox, TgLoadString(STID_CANNOT_OPEN_FILE_FOR_WRITING),
            pszXpmPath);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      free(pszCmd);
      return FALSE;
   }
   sprintf(gszMsgBox, TgLoadCachedString(CSTID_EXECUTING_GIVEN_PROGRAM),
         pszCmd);
   SetStringStatus(gszMsgBox);
   XSync(mainDisplay, False);
   if ((pPipe=(FILE*)popen(pszCmd,"r")) == NULL) {
      sprintf(gszMsgBox, TgLoadString(STID_FAIL_TO_EXECUTE_CMD), pszCmd);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      free(pszCmd);
      fclose(pFile);
      unlink(pszXpmPath);
      return FALSE;
   }
   if (!watch_cursor) {
      SetWatchCursor(drawWindow);
      SetWatchCursor(mainWindow);
   }
   writeFileFailed = FALSE;
   while ((bytes_read=fread(szBuf, sizeof(char), sizeof(szBuf), pPipe)) > 0) {
      if ((int)fwrite(szBuf, sizeof(char), bytes_read, pFile) <= 0) {
         writeFileFailed = TRUE;
         break;
      }
   }
   pclose(pPipe);
   if (!watch_cursor) {
      SetDefaultCursor(mainWindow);
      ShowCursor();
   }
   SetStringStatus(TgLoadCachedString(CSTID_DOTS_DONE));
   free(pszCmd);
   fclose(pFile);
   if (writeFileFailed) {
      FailToWriteFileMessage(pszXpmPath);
      unlink(pszXpmPath);
      return FALSE;
   }
   return TRUE;
}

void ImportPPMFile()
{
   char *rest=NULL, tmp_fname[MAXPATHLENGTH+1];
   char szPpmPath[MAXPATHLENGTH+1];
   int short_name=FALSE, image_w=0, image_h=0, remote_file=FALSE;

   MakeQuiescent();

   importingFile = TRUE;
   *szPpmPath = *tmp_fname = '\0';
   if (importFromLibrary) {
      char name[MAXSTRING+1], path[MAXSTRING+1];

      if (SelectFromLibrary(TgLoadString(STID_SEL_PPM_FILE_TO_IMPORT),
            "ppm", name, path) == INVALID) {
         importingFile = FALSE;
         return;
      }
      sprintf(szPpmPath, "%s%c%s", path, DIR_SEP, name);
   } else if (SelectFileNameToImport(TgLoadString(STID_SEL_PPM_FILE_TO_IMPORT),
         "ppm", szPpmPath) == INVALID) {
      importingFile = FALSE;
      return;
   } else if (FileIsRemote(szPpmPath)) {
      int is_html=FALSE;

      if (!DownloadRemoteFile(szPpmPath, NULL, NULL, &is_html, tmp_fname, NULL,
           0) || *tmp_fname == '\0') {
         importingFile = FALSE;
         return;
      }
      remote_file = TRUE;
   }
   if (!FinishImport(remote_file, tmp_fname, szPpmPath, IMPORTING_PPM, &image_w,
         &image_h)) {
      if (remote_file) unlink(tmp_fname);
      importingFile = FALSE;
      return;
   }
   if (!importFromLibrary && !remote_file) SetCurImportDir(szPpmPath);

   sprintf(gszMsgBox, TgLoadString(STID_GIVEN_PPM_SIZE_FILE_IMPORTED),
         image_w, image_h, (short_name ? rest : szPpmPath));
   Msg(gszMsgBox);

   if (remote_file) unlink(tmp_fname);
   importingFile = FALSE;
}

DspList *ImportFilterListing(pnEntries)
   int *pnEntries;
{
   int i;
   struct ImportInfoRec *pii=topImportInfo;
   DspList *pdl, *dsp_ptr;

   if (gnMaxImportFilters == 0) return NULL;
   pdl = (DspList*)malloc(gnMaxImportFilters*sizeof(DspList));
   if (pdl == NULL) {
      FailAllocMessage();
      return NULL;
   }
   for (i=0, dsp_ptr=pdl; i < gnMaxImportFilters; i++, dsp_ptr++) {
      sprintf(gszMsgBox, "%s (%s)", pii->name, pii->ext);
      UtilStrCpyN(dsp_ptr->itemstr, sizeof(dsp_ptr->itemstr), gszMsgBox);
      UtilStrCpyN(dsp_ptr->pathstr, sizeof(dsp_ptr->pathstr), pii->cmd);
      dsp_ptr->directory = FALSE;
      dsp_ptr->next = ((i == gnMaxImportFilters-1) ? NULL : (&dsp_ptr[1]));
      pii = pii->next;
   }
   if (pnEntries != NULL) *pnEntries = gnMaxImportFilters;
   return pdl;
}

static
int ChooseAnImportFilter(top_str, entries, num_entries)
   char *top_str, **entries;
   int num_entries;
{
   char win_name[128];
   int selected_index=INVALID;

   ResetNamesInfo();
   NamesSetTitle(top_str);
   NamesAddButton(TgLoadCachedString(CSTID_OK), BUTTON_OK);
   NamesAddButton(TgLoadCachedString(CSTID_CANCEL), BUTTON_CANCEL);
   NamesSetEntries(NULL, entries, num_entries, NULL, TRUE, INVALID, 0);
   NamesSetStyle(NAMES_COMPLEX_SELECT_NAME, NAMES_LOOP_ONCE);
   sprintf(win_name, "%s - %s", TOOL_NAME, top_str);
   if (Names(win_name, &selected_index, NULL, 0, NULL) == BUTTON_OK) {
      return selected_index;
   }
   return INVALID;
}

static
int SelectAnImportFilter(pszSelected)
   char *pszSelected;
{
   int num_entries=0, index=0;
   DspList *dsp_ptr=ImportFilterListing(&num_entries);
   char **entries;

   if (pszSelected != NULL) *pszSelected = '\0';
   if (dsp_ptr == NULL) {
      sprintf(gszMsgBox, TgLoadString(STID_CANT_FIND_ANY_IMP_FLTR_SPEC),
            TOOL_NAME, "MaxImportFilters", TOOL_NAME, "ImportFilter#");
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return INVALID;
   }
   entries = MakeNameDspItemArray(num_entries, dsp_ptr);
   if (entries == NULL) {
      free(dsp_ptr);
      return INVALID;
   }
   if ((index=ChooseAnImportFilter(TgLoadString(STID_SEL_AN_IMPORT_FILTER),
         entries, num_entries)) == INVALID) {
      if (pszSelected != NULL) *pszSelected = '\0';
   } else {
      if (pszSelected != NULL) {
         strcpy(pszSelected, entries[index]);
      }
   }
   free(*entries);
   free(entries);
   free(dsp_ptr);
   return index;
}

static
int ConvertAnyToXpm(pii, pszAnyPath, pszXpmPath, xpm_path_sz)
   struct ImportInfoRec *pii;
   char *pszAnyPath, *pszXpmPath;
   int xpm_path_sz;
{
   FILE *pFile=NULL, *pPipe=NULL;
   char *pszCmd=NULL, szBuf[MAXSTRING];
   int bytes_read=0, watch_cursor=watchCursorOnMainWindow;

   if (MkTempFile(pszXpmPath, xpm_path_sz, tmpDir, TOOL_NAME) == NULL) {
      return FALSE;
   }
   pszCmd =
         (char*)malloc((strlen(pii->cmd)+strlen(pszAnyPath)+10)*sizeof(char));
   if (pszCmd == NULL) {
      FailAllocMessage();
      return FALSE;
   }
   sprintf(pszCmd, pii->cmd, pszAnyPath);
   if (!FindProgramInPath(pszCmd, NULL, FALSE)) {
      free(pszCmd);
      return FALSE;
   }
   if ((pFile=fopen(pszXpmPath,"w")) == NULL) {
      sprintf(gszMsgBox, TgLoadString(STID_CANNOT_OPEN_FILE_FOR_WRITING),
            pszXpmPath);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      free(pszCmd);
      return FALSE;
   }
   sprintf(gszMsgBox, TgLoadCachedString(CSTID_EXECUTING_GIVEN_PROGRAM),
         pszCmd);
   SetStringStatus(gszMsgBox);
   XSync(mainDisplay, False);
   if ((pPipe=(FILE*)popen(pszCmd,"r")) == NULL) {
      sprintf(gszMsgBox, TgLoadString(STID_FAIL_TO_EXECUTE_CMD), pszCmd);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      free(pszCmd);
      fclose(pFile);
      unlink(pszXpmPath);
      return FALSE;
   }
   if (!watch_cursor) {
      SetWatchCursor(drawWindow);
      SetWatchCursor(mainWindow);
   }
   writeFileFailed = FALSE;
   while ((bytes_read=fread(szBuf, sizeof(char), sizeof(szBuf), pPipe)) > 0) {
      if ((int)fwrite(szBuf, sizeof(char), bytes_read, pFile) <= 0) {
         writeFileFailed = TRUE;
         break;
      }
   }
   pclose(pPipe);
   if (!watch_cursor) {
      SetDefaultCursor(mainWindow);
      ShowCursor();
   }
   SetStringStatus(TgLoadCachedString(CSTID_DOTS_DONE));
   free(pszCmd);
   fclose(pFile);
   if (writeFileFailed) {
      FailToWriteFileMessage(pszXpmPath);
      unlink(pszXpmPath);
      return FALSE;
   }
   return TRUE;
}

static int gnEnableFailedImportMsgBox=TRUE;

int GetEnableFailedImportMsgBox()
{
   return gnEnableFailedImportMsgBox;
}

int SetEnableFailedImportMsgBox(int enable)
{
   int saved_enable_failed_import_msgbox=gnEnableFailedImportMsgBox;

   gnEnableFailedImportMsgBox = enable;
   return saved_enable_failed_import_msgbox;
}

static
int ReadFourBytes(psz_path, magic_buf, magic_buf_sz)
   char *psz_path, *magic_buf;
   int magic_buf_sz;
{
   FILE *fp=fopen(psz_path, "r");
   int rc=TRUE;

   if (fp == NULL) {
      return FailToOpenMessage(psz_path, "r", NULL);
   }
   if (fread(magic_buf, sizeof(char), magic_buf_sz, fp) != magic_buf_sz) {
      rc = FALSE;
   }
   fclose(fp);

   return rc;
}

static
int DoImportOtherFile(pii, psz_path)
   struct ImportInfoRec *pii;
   char *psz_path;
{
   char szTop[MAXSTRING+1], tmp_fname[MAXPATHLENGTH+1];
   char szOtherPath[MAXPATHLENGTH+1], szXpmPath[MAXPATHLENGTH+1], magic_buf[4];
   int rc, ncolors, chars_per_pixel, *pixels=NULL, short_name=FALSE;
   int first_pixel_is_bg, image_w, image_h, w, h, remote_file=FALSE, ppm=FALSE;
   Pixmap pixmap=None, bitmap=None;
   XImage *image=NULL, *bitmap_image=NULL;
   char *color_char=NULL, **color_str=NULL, *xpm_data=NULL, *rest=NULL;
   struct ObjRec *obj_ptr=NULL;
   XEvent ev;

   importingFile = TRUE;
   *szOtherPath = *tmp_fname = '\0';
   if (psz_path != NULL) {
      UtilStrCpyN(szOtherPath, sizeof(szOtherPath), psz_path);
   } else {
      MakeQuiescent();

      sprintf(szTop, TgLoadString(STID_SEL_A_TYPE_FILE_TO_IMPORT), pii->name);
      if (importFromLibrary) {
         char name[MAXSTRING+1], dir_name[MAXSTRING+1];

         if (SelectFromLibrary(szTop, pii->ext, name, dir_name) == INVALID) {
            importingFile = FALSE;
            return FALSE;
         }
         sprintf(szOtherPath, "%s%c%s", dir_name, DIR_SEP, name);
      } else if (SelectFileNameToImport(szTop, pii->ext, szOtherPath) ==
            INVALID) {
         importingFile = FALSE;
         return FALSE;
      } else if (FileIsRemote(szOtherPath)) {
         int is_html=FALSE;

         if (!DownloadRemoteFile(szOtherPath, NULL, NULL, &is_html, tmp_fname,
               NULL, 0) || *tmp_fname == '\0') {
            importingFile = FALSE;
            return FALSE;
         }
         remote_file = TRUE;
      }
      XSync(mainDisplay, False);
      if (XCheckMaskEvent(mainDisplay, ExposureMask, &ev)) {
         ExposeEventHandler(&ev, TRUE);
      }
   }
   SetWatchCursor(drawWindow);
   SetWatchCursor(mainWindow);
   SaveStatusStrings();
   rc = ConvertAnyToXpm(pii, (remote_file ? tmp_fname : szOtherPath), szXpmPath,
         sizeof(szXpmPath));
   RestoreStatusStrings();
   SetDefaultCursor(mainWindow);
   ShowCursor();
   if (!rc) return FALSE;

   if (ReadFourBytes(szXpmPath, magic_buf, sizeof(magic_buf))) {
      if (magic_buf[0] == 'P' && (magic_buf[1] == '3' ||
            magic_buf[1] == '5' || magic_buf[1] == '6')) {
         ppm = TRUE;
      }
      SetWatchCursor(drawWindow);
      SetWatchCursor(mainWindow);
      if (ppm) {
         char deflated_fname[MAXPATHLENGTH+1];

         ResetPngHeaderInfo(&gPngHeaderInfo);
         obj_ptr = CreatePpmTrueObjFromFile(szXpmPath);
         if (obj_ptr != NULL &&
               MkTempFile(deflated_fname, sizeof(deflated_fname), tmpDir,
               TOOL_NAME) != NULL &&
               DeflateFile(szXpmPath, deflated_fname)) {
            struct XPmRec *xpm_ptr=obj_ptr->detail.xpm;

            xpm_ptr->real_type = PPM_TRUE;
            xpm_ptr->ppm_data_compress = PPM_DATA_DEFLATED;
            xpm_ptr->ppm_data = ReadFileIntoBuf(deflated_fname,
                  &xpm_ptr->ppm_data_size);
            xpm_ptr->ppm_mask_data = NULL;
            xpm_ptr->ppm_mask_size = 0;
            unlink(deflated_fname);

            rc = BitmapSuccess;
         } else {
            rc = BitmapFileInvalid;
         }
      } else {
         rc = MyReadPixmapFile(szXpmPath, &image_w, &image_h, &w, &h, &pixmap,
               &image, &bitmap, &bitmap_image, &ncolors, &chars_per_pixel,
               &first_pixel_is_bg, &color_char, &color_str, &pixels, &xpm_data);
      }
      SetDefaultCursor(mainWindow);
      ShowCursor();
   } else {
      rc = BitmapFileInvalid;
   }
   if (remote_file) {
      short_name = FALSE;
   } else {
      if ((short_name=IsPrefix(bootDir, szOtherPath, &rest))) ++rest;
   }
   if (rc != BitmapSuccess) {
      sprintf(gszMsgBox, TgLoadString(STID_CANT_IMPORT_GIVEN_TYPE_FILE),
            pii->name, (short_name ? rest : szOtherPath));
      if (gnEnableFailedImportMsgBox) {
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      }
      unlink(szXpmPath);
      if (remote_file) unlink(tmp_fname);
      importingFile = FALSE;
      return FALSE;
   }
   unlink(szXpmPath);
   if (!ppm) {
      obj_ptr = CreateXPmObj(image_w, image_h, w, h, pixmap, image, bitmap,
            bitmap_image, ncolors, chars_per_pixel, first_pixel_is_bg, color_char,
            color_str, pixels, xpm_data);
   }
   AddObj(NULL, topObj, obj_ptr);

   if (psz_path == NULL) {
      PlaceTopObj(obj_ptr, NULL, NULL);

      SelectTopObj();
      RecordNewObjCmd();
      SetFileModified(TRUE);
      justDupped = FALSE;
      if (!importFromLibrary && !remote_file) SetCurImportDir(szOtherPath);

      sprintf(gszMsgBox, TgLoadString(STID_GIVEN_TYPE_SIZE_FILE_IMPORTED),
            pii->name, image_w, image_h, (short_name ? rest : szOtherPath));
      Msg(gszMsgBox);
   }
   if (!PRTGIF && colorLayers && needToRedrawColorWindow) {
      RedrawColorWindow();
   }
   if (remote_file) unlink(tmp_fname);
   importingFile = FALSE;

   return TRUE;
}

void ImportOtherFile()
{
   int i, index;
   struct ImportInfoRec *pii;
   XEvent ev;

   if ((index=SelectAnImportFilter(NULL)) == INVALID) {
      return;
   }
   for (i=0, pii=topImportInfo; i < index && pii != NULL; i++, pii=pii->next) {
   }
   if (pii == NULL) return;

   XSync(mainDisplay, False);
   if (XCheckMaskEvent(mainDisplay, ExposureMask, &ev)) {
      ExposeEventHandler(&ev, TRUE);
   }
   DoImportOtherFile(pii, NULL);
}

void ImportOtherFileType(pszName)
   char *pszName;
{
   char *paren_ptr=(pszName == NULL ? NULL : strchr(pszName, ')'));
   struct ImportInfoRec *pii;

   if (paren_ptr == NULL) {
      sprintf(gszMsgBox, TgLoadString(STID_INVALID_FMT_IN_SHORTCUT_SPEC),
            "ImportOtherFileType", (pszName == NULL ? "" : pszName));
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return;
   }
   *paren_ptr = '\0';
   for (pii=topImportInfo; pii != NULL; pii=pii->next) {
      if (strcmp(pii->name, pszName) == 0) {
         break;
      }
   }
   if (pii == NULL) {
      sprintf(gszMsgBox, TgLoadString(STID_CANT_FIND_NAMED_IMP_FLTR), pszName);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return;
   }
   DoImportOtherFile(pii, NULL);
}

int ImportSpecifiedFileType(psz_path, psz_filter)
   char *psz_path, *psz_filter;
{
   struct ImportInfoRec *pii=NULL;

   for (pii=topImportInfo; pii != NULL; pii=pii->next) {
      if (strcmp(pii->name, psz_filter) == 0) {
         break;
      }
   }
   if (pii == NULL) {
      sprintf(gszMsgBox, TgLoadString(STID_CANT_FIND_NAMED_IMP_FLTR),
            psz_filter);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return FALSE;
   }
   return DoImportOtherFile(pii, psz_path);
}

int ConvertOtherToXpm(pszFile, pszFormat, pszXpmPath, xpm_path_sz)
   char *pszFile, *pszFormat, *pszXpmPath;
   int xpm_path_sz;
{
   struct ImportInfoRec *pii=NULL;

   for (pii=topImportInfo; pii != NULL; pii=pii->next) {
      if (strcmp(pii->name, pszFormat) == 0) {
         break;
      }
   }
   if (pii == NULL) {
      sprintf(gszMsgBox, TgLoadString(STID_CANT_FIND_NAMED_IMP_FLTR),
            pszFormat);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return FALSE;
   }
   return ConvertAnyToXpm(pii, pszFile, pszXpmPath, xpm_path_sz);
}

void ImportGIFToXPixmapDeck()
{
   char szTop[MAXSTRING+1], cmd[MAXSTRING<<1], part_fname_format[MAXSTRING];
   char gif_fname[MAXPATHLENGTH+1], name[MAXSTRING], tmp_fname[MAXPATHLENGTH+1];
   int i=0, import_other_index=0, rc=0, placed=FALSE;
   int ltx=0, lty=0, rbx=0, rby=0, remote_file=FALSE, real_remote_file=FALSE;
   XEvent ev;
   struct ImportInfoRec ii;
   struct ObjRec *saved_top_obj=topObj, *saved_bot_obj=botObj;
   struct AttrRec *exec_attr=NULL;

   MakeQuiescent();

   memset(&ii, 0, sizeof(struct ImportInfoRec));

   /* pick an animated GIF file */
   sprintf(szTop, TgLoadString(STID_SEL_ANIM_GIF_FILE_TO_IMPORT));
   importingFile = TRUE;
   *gif_fname = *tmp_fname = '\0';
   if (importFromLibrary) {
      char name[MAXSTRING+1], dir_name[MAXSTRING+1];

      if (SelectFromLibrary(szTop, "gif", name, dir_name) == INVALID) {
         importingFile = FALSE;
         return;
      }
      sprintf(gif_fname, "%s%c%s", dir_name, DIR_SEP, name);
   } else if (SelectFileNameToImport(szTop, "gif", gif_fname) == INVALID) {
      importingFile = FALSE;
      return;
   } else if (FileIsRemote(gif_fname)) {
      int is_html=FALSE;

      if (!DownloadRemoteFile(gif_fname, NULL, NULL, &is_html, tmp_fname, NULL,
            0) || *tmp_fname == '\0') {
         importingFile = FALSE;
         return;
      }
      remote_file = real_remote_file = TRUE;
   }
   XSync(mainDisplay, False);
   if (XCheckMaskEvent(mainDisplay, ExposureMask, &ev)) {
      ExposeEventHandler(&ev, TRUE);
   }
   if (!remote_file) {
      if (MkTempFile(tmp_fname, sizeof(tmp_fname), tmpDir, TOOL_NAME) == NULL) {
         importingFile = FALSE;
         return;
      }
      if (UtilCopyFile(gif_fname, tmp_fname) != TG_REMOTE_STATUS_OK) {
         sprintf(gszMsgBox, TgLoadString(STID_CANT_CREATE_NAMED_TMP_FILE),
               tmp_fname);
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
         importingFile = FALSE;
         return;
      }
      remote_file = TRUE;
   }
   /* explode the animated GIF file into parts */
   InitGifToXpm();
   sprintf(cmd, "cd %s; %s %s", tmpDir, gifAnimExplodeCmd,
         (remote_file ? tmp_fname : gif_fname));
   SetWatchCursor(drawWindow);
   SetWatchCursor(mainWindow);
   rc = ExecuteCmd(cmd, FALSE);
   SetDefaultCursor(mainWindow);
   ShowCursor();
   if (!rc) {
      sprintf(gszMsgBox, TgLoadString(STID_CANT_EXEC_CMD_IMPORT_ANIM_GIF), cmd);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      if (remote_file) unlink(tmp_fname);
      importingFile = FALSE;
      return;
   }
   /* determine how many to import */
   strcpy(name, tmp_fname);
   /* pick a GIF import filter */
   if (gnMaxImportFilters > 0) {
      struct ImportInfoRec *pii=NULL;

      switch (MsgBox(TgLoadString(STID_Q_LIKE_TO_USE_AN_IMP_FLTR_GIF),
            TOOL_NAME, YNC_MB)) {
      case MB_ID_YES:
         if ((import_other_index=SelectAnImportFilter(NULL)) == INVALID) {
            if (remote_file) unlink(tmp_fname);
            importingFile = FALSE;
            return;
         }
         for (i=0, pii=topImportInfo; i < import_other_index && pii != NULL;
               i++, pii=pii->next) {
         }
         if (pii == NULL) {
            if (remote_file) unlink(tmp_fname);
            importingFile = FALSE;
            return;
         }

         XSync(mainDisplay, False);
         if (XCheckMaskEvent(mainDisplay, ExposureMask, &ev)) {
            ExposeEventHandler(&ev, TRUE);
         }
         memcpy(&ii, pii, sizeof(struct ImportInfoRec));
         ii.name = NULL;
         ii.next = NULL;
         break;
      case MB_ID_NO:
         ii.ext = "gif";
         ii.cmd = gifToXpmCmd;
         break;
      case MB_ID_CANCEL:
         if (remote_file) unlink(tmp_fname);
         importingFile = FALSE;
         return;
      }
   } else {
      ii.ext = "gif";
      ii.cmd = gifToXpmCmd;
   }
   curPage->top = curPage->bot = topObj = botObj = NULL;
   /* import the GIF parts */
   strcpy(part_fname_format, "%s.%1d");
   SaveStatusStrings();
   SetWatchCursor(drawWindow);
   SetWatchCursor(mainWindow);
   for (i=0; ; i++) {
      char part_fname[MAXPATHLENGTH], xpm_fname[MAXPATHLENGTH];
      char *color_char=NULL, **color_str=NULL, *xpm_data=NULL;
      int image_w=0, image_h=0, w=0, h=0, ncolors=0, chars_per_pixel=0;
      int first_pixel_is_bg=FALSE, *pixels=NULL;
      Pixmap pixmap=None, bitmap=None;
      XImage *image=NULL, *bitmap_image=NULL;
      struct ObjRec *obj_ptr=NULL;
      struct SelRec *sel_ptr=NULL;

      *xpm_fname = '\0';
      sprintf(part_fname, part_fname_format, name, i);
      if (i == 0) {
         if (!UtilPathExists(part_fname)) {
            strcpy(part_fname_format, "%s.%03d");
            sprintf(part_fname, part_fname_format, name, i);
            if (!UtilPathExists(part_fname)) {
               if (i == 0 &&
                     !FindProgramInPath(gifAnimExplodeCmd, NULL, TRUE)) {
                  char *psz=strchr(gifAnimExplodeCmd, ' ');

                  if (psz != NULL) *psz = '\0';
                  sprintf(gszMsgBox,
                        TgLoadString(STID_CANT_IMP_ANIM_GIF_DIR_OR_PATH),
                        part_fname, tmpDir, gifAnimExplodeCmd);
                  if (psz != NULL) *psz = ' ';
                  MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
               } else {
                  sprintf(gszMsgBox,
                        TgLoadString(STID_CANT_IMP_ANIM_GIF_TMP_DIR),
                        part_fname, tmpDir);
                  MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
               }
               break;
            }
         }
      } else if (!UtilPathExists(part_fname)) {
         /* got all the GIF parts */
         break;
      }
      rc = ConvertAnyToXpm(&ii, part_fname, xpm_fname, sizeof(xpm_fname));
      if (!rc) {
         unlink(part_fname);
         break;
      }
      unlink(part_fname);
      rc = MyReadPixmapFile(xpm_fname, &image_w, &image_h, &w, &h, &pixmap,
            &image, &bitmap, &bitmap_image, &ncolors, &chars_per_pixel,
            &first_pixel_is_bg, &color_char, &color_str, &pixels, &xpm_data);
      if (rc != BitmapSuccess) {
         sprintf(gszMsgBox, TgLoadString(STID_CANT_IMP_GIVEN_ANIM_GIF),
               part_fname);
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
         unlink(xpm_fname);
         break;
      }
      unlink(xpm_fname);
      obj_ptr = CreateXPmObj(image_w, image_h, w, h, pixmap, image, bitmap,
            bitmap_image, ncolors, chars_per_pixel, first_pixel_is_bg,
            color_char, color_str, pixels, xpm_data);
      AddObj(botObj, NULL, obj_ptr);
      if (!placed) {
         PlaceTopObj(obj_ptr, NULL, NULL);
         placed = TRUE;
         ltx = obj_ptr->bbox.ltx; lty = obj_ptr->bbox.lty;
         rbx = obj_ptr->bbox.rbx; rby = obj_ptr->bbox.rby;
      } else {
         MoveObj(obj_ptr, ltx-obj_ptr->bbox.ltx, lty-obj_ptr->bbox.lty);
         if (obj_ptr->bbox.rbx > rbx) rbx = obj_ptr->bbox.rbx;
         if (obj_ptr->bbox.rby > rby) rby = obj_ptr->bbox.rby;
      }
      DrawObj(drawWindow, obj_ptr);
      sel_ptr = SelectThisObject(obj_ptr);
      AddSel(botSel, NULL, sel_ptr);
   }
   SetDefaultCursor(mainWindow);
   ShowCursor();
   if (remote_file) unlink(tmp_fname);

   if (topObj != NULL) {
      curPage->top = topObj;
   } else {
      curPage->top = topObj = saved_top_obj;
   }
   if (botObj != NULL) {
      botObj->next = saved_top_obj;
   }
   if (saved_top_obj != NULL) {
      saved_top_obj->prev = botObj;
   }
   if (saved_bot_obj != NULL) {
      curPage->bot = botObj = saved_bot_obj;
   } else {
      curPage->bot = botObj;
   }
   if (topSel == NULL) {
      RestoreStatusStrings();
      return;
   }
   UpdSelBBox();
   if (topSel == botSel) {
      sprintf(gszMsgBox, TgLoadString(STID_ONLY_ONE_GIF_COMP_EXTRACTED),
            gif_fname);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      RecordNewObjCmd();
   } else {
      int saved_history_depth=historyDepth;

      HighLightForward();

      historyDepth = 0;
      GroupSelObj(TRUE, TRUE, TRUE);
      historyDepth = saved_history_depth;

      HighLightReverse();
      exec_attr = AddAttrByNameAndValue(topObj, "exec=",
            "flip_deck(infinite,10,linear)");
      exec_attr->shown = FALSE;
      MoveObj(exec_attr->obj, ltx, lty);
      UpdTextBBox(exec_attr->obj);
      AdjObjBBox(topObj);
      UpdSelBBox();
      RecordNewObjCmd();
   }
   RedrawAnArea(botObj, ltx-GRID_ABS_SIZE(1), lty-GRID_ABS_SIZE(1),
         rbx+GRID_ABS_SIZE(1), rby+GRID_ABS_SIZE(1));
   SetFileModified(TRUE);
   HighLightForward();
   if (!importFromLibrary && !real_remote_file) {
      SetCurImportDir(gif_fname);
   }
   if (colorLayers && needToRedrawColorWindow) {
      RedrawColorWindow();
   }
   importingFile = FALSE;
   if (exec_attr != NULL) {
      sprintf(gszMsgBox, TgLoadString(STID_ANIMATING_GIVEN), gif_fname);
      TwoLineMsg(gszMsgBox, TgLoadString(STID_PRESS_ESC_TO_STOP));
      DoExecLoop(topObj, exec_attr);
      Msg(TgLoadString(STID_GIF_ANIMATION_STOPPED));
   }
   RestoreStatusStrings();
}

/* ----------------------- Browse Functions ----------------------- */

static int origBrowseX=0, origBrowseY=0, maxBrowseX=0, maxBrowseY=0;
static int curBrowseX=0, curBrowseY=0, curBrowseRowHeight=0;
static int savedDirNameLen=0;
static int browseObjType=INVALID;
static char savedDirName[MAXPATHLENGTH+1];

static
void InitBrowse(pszDir, ObjType)
   char *pszDir;
   int ObjType;
{
   browseObjType = ObjType;
   if (ObjType == OBJ_ICON) {
      curBrowseX = origBrowseX = thumbnailX;
      maxBrowseX = paperWidth - curBrowseX;
      curBrowseY = origBrowseY = thumbnailY;
      maxBrowseY = paperHeight - curBrowseY;
      curBrowseRowHeight = 0;
   } else {
      curBrowseX = origBrowseX = drawOrigX+ABS_SIZE(EIGHTH_INCH);
      maxBrowseX = drawOrigX+drawWinW;
      curBrowseY = origBrowseY = drawOrigY+ABS_SIZE(EIGHTH_INCH);
      maxBrowseY = drawOrigY+drawWinH;
      curBrowseRowHeight = 0;
   }
   if (pszDir != NULL) {
      strcpy(savedDirName, pszDir);
      savedDirNameLen = strlen(savedDirName);
   }
}

static
int CheckExecInterrupt()
{
   XEvent ev;

   while (XCheckMaskEvent(mainDisplay, StructureNotifyMask, &ev)) {
      if (iconWindowShown) {
         if ((ev.xany.window == iconBaseWindow && ev.type == UnmapNotify) ||
               (ev.xany.window == mainWindow && ev.type == MapNotify)) {
            XPutBackEvent(mainDisplay, &ev);
            return TRUE;
         }
      } else if ((ev.xany.window == iconBaseWindow && ev.type == MapNotify) ||
            (ev.xany.window == mainWindow && ev.type == UnmapNotify)) {
         XPutBackEvent(mainDisplay, &ev);
         return TRUE;
      } else if (ev.type == ConfigureNotify) {
         Reconfigure(FALSE);
         if (browseObjType != OBJ_ICON) {
            maxBrowseX = drawOrigX+drawWinW;
         }
      }
   }
   while (XCheckMaskEvent(mainDisplay, VisibilityChangeMask, &ev)) {
      if (iconWindowShown) {
         if (ev.xany.window == mainWindow && ev.type == VisibilityNotify &&
               ev.xvisibility.state == VisibilityUnobscured) {
            XPutBackEvent(mainDisplay, &ev);
            return TRUE;
         } else {
            ExposeEventHandler(&ev, TRUE);
         }
      } else {
         if (ev.xany.window == iconBaseWindow && ev.type == VisibilityNotify &&
               ev.xvisibility.state == VisibilityUnobscured) {
            XPutBackEvent(mainDisplay, &ev);
            return TRUE;
         } else {
            ExposeEventHandler(&ev, TRUE);
         }
      }
   }
   if (XCheckMaskEvent(mainDisplay, ExposureMask, &ev)) {
      ExposeEventHandler(&ev, TRUE);
      while (XCheckMaskEvent(mainDisplay, ExposureMask, &ev)) ;
   }
   if (ESCPressed() || CheckInterrupt(TRUE)) {
      return TRUE;
   }
   while (XCheckMaskEvent(mainDisplay, ButtonPressMask|KeyPressMask, &ev)) ;
   return FALSE;
}

static
struct ObjRec *ImportForThumbnails(fname, p_bbox_w, p_bbox_h)
   char *fname;
   int *p_bbox_w, *p_bbox_h;
{
   struct ObjRec *imported_obj=NULL;
   struct ObjRec *saved_top_obj=topObj, *saved_bot_obj=botObj;
   struct BBRec saved_draw_win_bbox;

   (*p_bbox_w) = (*p_bbox_h) = 0;
   curPage->top = curPage->bot = topObj = botObj = NULL;

   importingFile = TRUE;
   pastingFile = TRUE;
   memcpy(&saved_draw_win_bbox, &drawWinBBox, sizeof(struct BBRec));
   drawWinBBox.ltx = drawWinBBox.rbx = drawOrigX-128;
   drawWinBBox.lty = drawWinBBox.rby = drawOrigY-128;
   if (ImportGivenFile(fname, FALSE, FALSE) == TRUE) {
      if (numObjSelected > 0) {
         struct BoxRec *box_ptr=NULL;
         struct ObjRec *obj_ptr=NULL;
         int changed=FALSE;

         for (obj_ptr=topObj; obj_ptr != NULL; obj_ptr=obj_ptr->next) {
            if (obj_ptr->type == OBJ_SYM) {
               obj_ptr->type = OBJ_GROUP;
               AdjObjBBox(obj_ptr);
               changed = TRUE;
            }
         }
         if (changed) {
            UpdSelBBox();
         }
         CreateBoxObj(selLtX, selLtY, selRbX, selRbY, TRUE);
         box_ptr = topObj->detail.b;
         box_ptr->fill = NONEPAT;
         box_ptr->pen = NONEPAT;
         RemoveAllSel();
         SelAllObj(FALSE, FALSE);
         GroupSelObj(FALSE, FALSE, FALSE);

         imported_obj = topObj;
         *p_bbox_w = imported_obj->bbox.rbx-imported_obj->bbox.ltx;
         *p_bbox_h = imported_obj->bbox.rby-imported_obj->bbox.lty;
#ifdef _TGIF_DBG /* debug, do not translate */
         TgAssert(topObj == botObj,
               "too many objects found in ImportForThumbnails()", NULL);
#endif /* _TGIF_DBG */
         RemoveAllSel();
      }
      MakeQuiescent();
   }
   memcpy(&drawWinBBox, &saved_draw_win_bbox, sizeof(struct BBRec));
   importingFile = FALSE;
   pastingFile = FALSE;

   curPage->top = topObj = saved_top_obj;
   curPage->bot = botObj = saved_bot_obj;

   return imported_obj;
}

static
int RegenerateImageFile(pszPath)
   char *pszPath;
{
   int saved_colordump=colorDump, saved_wheretoprint=whereToPrint;

   *gszImageProcXPmFile = '\0';
   gnConvolving = FALSE;

   colorDump = TRUE;
   whereToPrint = XBM_FILE;
   gnInImageProc = TRUE;

   SetWatchCursor(drawWindow);
   SetWatchCursor(mainWindow);
   DumpXBitmapFile(gnInImageProc, FALSE, TRUE);
   SetDefaultCursor(mainWindow);
   ShowCursor();

   gnInImageProc = FALSE;
   whereToPrint = saved_wheretoprint;
   colorDump = saved_colordump;
   if (*gszImageProcXPmFile == '\0') return FALSE;
   strcpy(pszPath, gszImageProcXPmFile);
   return TRUE;
}

static
struct ObjRec *MyRegenerateImage(obj_ptr)
   struct ObjRec *obj_ptr;
{
   int image_w=0, image_h=0, w=0, h=0, short_name=FALSE, rc=INVALID;
   int ltx=0, lty=0;
   int ncolors=0, chars_per_pixel=0, first_pixel_is_bg=0, *pixels=NULL;
   char *color_char=NULL, **color_str=NULL, *xpm_data=NULL, *rest=NULL;
   char szPath[MAXPATHLENGTH+1];
   Pixmap pixmap=None, bitmap=None;
   XImage *image=NULL, *bitmap_image=NULL;
   struct ObjRec *new_obj=NULL;

   ltx = obj_ptr->obbox.ltx;
   lty = obj_ptr->obbox.lty;

   AddObj(NULL, topObj, obj_ptr);
   PushPageInfo();

   rc = RegenerateImageFile(szPath);

   DelAllObj();
   PopPageInfo();
   if (!rc) {
      return NULL;
   }
   SetWatchCursor(drawWindow);
   SetWatchCursor(mainWindow);
   rc = MyReadPixmapFile(szPath, &image_w, &image_h, &w, &h, &pixmap,
         &image, &bitmap, &bitmap_image, &ncolors, &chars_per_pixel,
         &first_pixel_is_bg, &color_char, &color_str, &pixels, &xpm_data);
   SetDefaultCursor(mainWindow);
   ShowCursor();

   if ((short_name=IsPrefix(bootDir, szPath, &rest))) ++rest;
   if (rc != BitmapSuccess) {
      sprintf(gszMsgBox, TgLoadString(STID_CANNOT_IMPORT_XPM_FILE),
            (short_name ? rest : szPath));
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      unlink(szPath);
      return NULL;
   }
   unlink(szPath);
   new_obj = CreateXPmObj(image_w, image_h, w, h, pixmap, image, bitmap,
         bitmap_image, ncolors, chars_per_pixel, first_pixel_is_bg, color_char,
         color_str, pixels, xpm_data);
   MoveObj(new_obj, ltx-new_obj->obbox.ltx, lty-new_obj->obbox.lty);
   new_obj->tmp_parent = NULL;
   SetFileModified(TRUE);
   justDupped = FALSE;

   return new_obj;
}

static
struct ObjRec *GenerateXpmThumbnail(imported_obj)
   struct ObjRec *imported_obj;
{
   struct ObjRec *saved_top_obj=topObj, *saved_bot_obj=botObj, *new_obj=NULL;
   double orig_aspect_ratio=(double)0, aspect_ratio=(double)0;
   struct BBRec saved_draw_win_bbox;
   int bbox_w=(imported_obj->bbox.rbx-imported_obj->bbox.ltx);
   int bbox_h=(imported_obj->bbox.rby-imported_obj->bbox.lty);
   int final_w=0, final_h=0;

   curPage->top = curPage->bot = topObj = botObj = NULL;

   aspect_ratio = (((double)thumbnailW)/((double)thumbnailH));
   orig_aspect_ratio = (((double)bbox_w)/((double)bbox_h));

   memcpy(&saved_draw_win_bbox, &drawWinBBox, sizeof(struct BBRec));
   drawWinBBox.ltx = drawWinBBox.rbx = drawOrigX-128;
   drawWinBBox.lty = drawWinBBox.rby = drawOrigY-128;

   if (bbox_w <= thumbnailW && bbox_h <= thumbnailH) {
      /* do not scale */
      final_w = thumbnailW;
      final_h = thumbnailH;
   } else {
      double scale=(double)0;

      if (orig_aspect_ratio > aspect_ratio) {
         final_w = thumbnailW;
         final_h = round(((double)thumbnailW) / orig_aspect_ratio);
         scale = ((double)bbox_w) / ((double)thumbnailW);
      } else {
         final_h = thumbnailH;
         final_w = round(((double)thumbnailH) * orig_aspect_ratio);
         scale = ((double)bbox_h) / ((double)thumbnailH);
      }
      ScaleObjLikeScaleEverything(imported_obj, scale, FALSE);
   }
   new_obj = MyRegenerateImage(imported_obj); /* imported_obj is freed here */
   if (new_obj != NULL) {
      struct BoxRec *box_ptr=NULL;
      int obj_w=(new_obj->obbox.rbx-new_obj->obbox.ltx);
      int obj_h=(new_obj->obbox.rby-new_obj->obbox.lty);
      int new_ltx=((thumbnailW+(thumbnailPadding<<1)-obj_w)>>1);
      int new_lty=((thumbnailH+(thumbnailPadding<<1)-obj_h)>>1);

      CreateBoxObj(0, 0, thumbnailW+(thumbnailPadding<<1),
            thumbnailH+(thumbnailPadding<<1), TRUE);
      box_ptr = topObj->detail.b;
      box_ptr->fill = BACKPAT;
      box_ptr->pen = SOLIDPAT;
      box_ptr->dash = 0;
      box_ptr->width = 0;
      strcpy(box_ptr->width_spec, "0");
      AdjObjBBox(topObj);
      MoveObj(new_obj, new_ltx-new_obj->obbox.ltx, new_lty-new_obj->obbox.lty);

      AddObj(NULL, topObj, new_obj);
      SelAllObj(FALSE, FALSE);
      GroupSelObj(FALSE, FALSE, FALSE);

      RemoveAllSel();
      new_obj = topObj;
      UnlinkObj(topObj);
   }
   memcpy(&drawWinBBox, &saved_draw_win_bbox, sizeof(struct BBRec));

   curPage->top = topObj = saved_top_obj;
   curPage->bot = botObj = saved_bot_obj;

   return new_obj;
}

static
int BrowseDir(DirName, ExtStr, ExtStrLen, ObjType, pii)
   char *DirName, *ExtStr;
   int ExtStrLen, ObjType;
   struct ImportInfoRec *pii;
   /* returns TRUE if interrupted */
{
   char name[MAXPATHLENGTH+1];
   int interrupted=FALSE, esave_len=0;
#ifdef VMS
   int len;
#endif /* VMS */
   DIR *dirp;
   DIR_ENTRY *d;

   if ((dirp=opendir(DirName)) == NULL) return interrupted;
   esave_len = strlen("EmergencySave.");
   if (pii == NULL) {
      sprintf(gszMsgBox, TgLoadString(STID_BROWSING_DIR_FOR_TYPE_FILES),
            DirName, ExtStr);
   } else {
      sprintf(gszMsgBox, TgLoadString(STID_BROWSING_DIR_FOR_TYPE_FILES),
            DirName, pii->name);
   }
   Msg(gszMsgBox);
   while ((d=readdir(dirp)) != NULL) {
      int ncolors, chars_per_pixel, first_pixel_is_bg, * pixels;
      int image_w=0, image_h=0, w=0, h=0, x_hot, y_hot, rc=INVALID;
      unsigned int tmp_w, tmp_h;
      char *color_char, **color_str, *xpm_data=NULL;
      Pixmap bitmap=None, pixmap=None;
      XImage *image=NULL, *bitmap_image=NULL;
      struct ObjRec *imported_obj=NULL;

      if (CheckExecInterrupt()) {
         interrupted = TRUE;
         break;
      }
      if (!ExtensionMatch(ExtStr, d->d_name)) {
         continue;
      }
      if (strncmp(d->d_name, "EmergencySave.", esave_len) == 0) {
         continue;
      }
      sprintf(name, "%s%c%s", DirName, DIR_SEP, d->d_name);
      sprintf(gszMsgBox, TgLoadCachedString(CSTID_OPENING_GIVEN),
            &name[savedDirNameLen+1]);
      SetStringStatus(gszMsgBox);
      if (ObjType == OBJ_XBM) {
         rc = XReadBitmapFile(mainDisplay, mainWindow, name, &tmp_w, &tmp_h,
               &bitmap, &x_hot, &y_hot);
         w = (int)tmp_w; h = (int)tmp_h;
      } else if (ObjType == OBJ_XPM) {
         if (pii != NULL) {
            char szXpmPath[MAXPATHLENGTH+1];

            SaveStatusStrings();
            rc = ConvertAnyToXpm(pii, name, szXpmPath, sizeof(szXpmPath));
            RestoreStatusStrings();
            if (!rc) continue;
            rc = MyReadPixmapFile(szXpmPath, &image_w, &image_h, &w, &h,
                  &pixmap, &image, &bitmap, &bitmap_image, &ncolors,
                  &chars_per_pixel, &first_pixel_is_bg, &color_char,
                  &color_str, &pixels, &xpm_data);
            unlink(szXpmPath);
         } else {
            rc = MyReadPixmapFile(name, &image_w, &image_h, &w, &h,
                  &pixmap, &image, &bitmap, &bitmap_image, &ncolors,
                  &chars_per_pixel, &first_pixel_is_bg, &color_char,
                  &color_str, &pixels, &xpm_data);
         }
      } else if (ObjType == OBJ_ICON) {
         imported_obj = ImportForThumbnails(name, (int*)&tmp_w, (int*)&tmp_h);
         if (imported_obj == NULL) continue;
         rc = BitmapSuccess;
         w = (int)tmp_w;
         h = (int)tmp_h;
      } else {
      }
      if (rc == BitmapSuccess) {
         struct ObjRec *obj_ptr=NULL;
         int short_name;
         char *rest=NULL;

         if (ObjType == OBJ_XBM) {
            obj_ptr = CreateXBmObj(w, h, w, h, bitmap, NULL);
         } else if (ObjType == OBJ_XPM) {
            obj_ptr = CreateXPmObj(image_w, image_h, w, h, pixmap, image,
                  bitmap, bitmap_image, ncolors, chars_per_pixel,
                  first_pixel_is_bg, color_char, color_str, pixels, xpm_data);
         } else if (ObjType == OBJ_ICON) {
            obj_ptr = GenerateXpmThumbnail(imported_obj);
            /* imported_obj is deleted in GenerateXpmThumbnail() */
            if (obj_ptr == NULL) {
               continue;
            }
            w = (int)(obj_ptr->obbox.rbx-obj_ptr->obbox.ltx);
            h = (int)(obj_ptr->obbox.rby-obj_ptr->obbox.lty);
         } else {
         }
         if ((short_name=IsPrefix((curDirIsLocal ? curDir : curLocalDir), name,
               &rest))) {
            ++rest;
         }
         if (ObjType == OBJ_ICON) {
            if (imported_obj->type != OBJ_TEXT) {
               AddAttrByNameAndValue(obj_ptr, "href=",
                     (short_name ? rest : name));
            }
         } else {
            AddAttrByNameAndValue(obj_ptr, "file_name=",
                  (short_name ? rest : name));
         }
         if (showFileNameOnBrowse && obj_ptr->fattr != NULL &&
               obj_ptr->fattr == obj_ptr->lattr) {
            struct AttrRec *attr_ptr=obj_ptr->fattr;
            int attr_h, attr_w, attr_ltx, attr_lty;

            attr_ptr->shown = TRUE;
            attr_ptr->nameshown = FALSE;
            UpdAttr(attr_ptr);
            AdjObjBBox(attr_ptr->obj);
            attr_w = attr_ptr->obj->obbox.rbx-attr_ptr->obj->obbox.ltx;
            attr_h = attr_ptr->obj->bbox.rby-attr_ptr->obj->bbox.lty;
            attr_ltx = ((obj_ptr->obbox.ltx+obj_ptr->obbox.rbx)>>1)-(attr_w>>1);
            attr_lty = obj_ptr->bbox.rby;
            MoveObj(attr_ptr->obj, attr_ltx-attr_ptr->obj->obbox.ltx,
                  attr_lty-attr_ptr->obj->obbox.lty);
            if (attr_w > w && ObjType != OBJ_ICON) w = attr_w;
            h += attr_h;
         }
         AdjObjBBox(obj_ptr);
         if (ObjType == OBJ_ICON) {
            if (curBrowseX+w > maxBrowseX-origBrowseX) {
               curBrowseX = origBrowseX;
               curBrowseY += curBrowseRowHeight+thumbnailYGap;
               while (curBrowseY > maxBrowseY) {
                  ForceScrollDown(TRUE);
                  maxBrowseY = drawOrigY+drawWinH;
               }
               curBrowseRowHeight = h;
            } else if (h > curBrowseRowHeight) {
               curBrowseRowHeight = h;
            }
         } else {
            if (curBrowseX+w > maxBrowseX) {
               curBrowseX = origBrowseX;
               curBrowseY += curBrowseRowHeight;
               while (curBrowseY > maxBrowseY) {
                  ForceScrollDown(TRUE);
                  maxBrowseY = drawOrigY+drawWinH;
               }
               curBrowseRowHeight = h;
            } else if (h > curBrowseRowHeight) {
               curBrowseRowHeight = h;
            }
         }
         obj_ptr->tmp_parent = NULL;
         AddObj(NULL, topObj, obj_ptr);

         MoveObj(topObj, curBrowseX-topObj->obbox.ltx,
               curBrowseY-topObj->obbox.lty);
         numRedrawBBox = 0;
         DrawObj(drawWindow, topObj);
         RecordNewObjCmd();
         SetFileModified(TRUE);
         justDupped = FALSE;
         XSync(mainDisplay, False);
         if (ObjType == OBJ_ICON) {
            curBrowseX += w + thumbnailXGap;
         } else {
            curBrowseX += w;
         }
      } else {
      }
   }
   closedir(dirp);
   if ((dirp=opendir(DirName)) == NULL) return interrupted;
   while (!interrupted && (d=readdir(dirp)) != NULL) {
      if (CheckExecInterrupt()) {
         interrupted = TRUE;
         break;
      }
#ifdef VMS
      len = strlen(d->d_name);
      if (len > 4 && (strcmp(".dir", &d->d_name[len-4]) == 0)) {
         sprintf(name, "%s%c%s", DirName, DIR_SEP, d->d_name);
         if (BrowseDir(name, ExtStr, ExtStrLen, ObjType, pii)) {
            interrupted = TRUE;
            break;
         }
      }
#endif /* VMS */
      if (strcmp(d->d_name, ".") != 0 && strcmp(d->d_name, "..") != 0) {
         struct stat stat_buf;

         sprintf(name, "%s%c%s", DirName, DIR_SEP, d->d_name);
         if (stat(name, &stat_buf) == 0 && (stat_buf.st_mode & S_IFDIR)) {
            int skip=FALSE;

#ifndef _NO_LSTAT
            if (lstat(name, &stat_buf) == 0 &&
                  ((stat_buf.st_mode & S_IFLNK) == S_IFLNK)) {
               sprintf(gszMsgBox, TgLoadString(STID_SKIP_GIVEN_FILE_SYM_LINK),
                     &name[savedDirNameLen+1]);
               Msg(gszMsgBox);
               skip = TRUE;
            }
#endif /* _NO_LSTAT */
            if (!interrupted && !skip &&
                  BrowseDir(name, ExtStr, ExtStrLen, ObjType, pii)) {
               interrupted = TRUE;
               break;
            }
         }
      }
   }
   closedir(dirp);
   return interrupted;
}

static
void StartBrowse(DirName, ExtStr, ExtStrLen, ObjType, pii)
   char *DirName, *ExtStr;
   int ExtStrLen, ObjType;
   struct ImportInfoRec *pii;
{
   int saved_text_just=textJust;

   if (!CurFontCheck()) {
      MsgBox(TgLoadString(STID_CHECK_FONT_FAILED_FOR_BROWSE), TOOL_NAME,
            INFO_MB);
      return;
   }
   SetWatchCursor(drawWindow);
   SetWatchCursor(mainWindow);
   MakeQuiescent();
   SaveStatusStrings();
   InitBrowse(DirName, ObjType);
   StartCompositeCmd();
   ShowInterrupt(1);
   textJust = JUST_C;
   if (BrowseDir(DirName, ExtStr, ExtStrLen, ObjType, pii)) {
      Msg(TgLoadString(STID_USER_INTR));
   }
   textJust = saved_text_just;
   if (!PRTGIF && colorLayers && needToRedrawColorWindow) {
      RedrawColorWindow();
   }
   HideInterrupt();
   EndCompositeCmd();
   RestoreStatusStrings();
   SetDefaultCursor(mainWindow);
   ShowCursor();
   XFlush(mainDisplay);
}

void CreateThumbnails()
{
   static char stszObjFileExt[20], stszGzObjFileExt[20];
   static char stszSymFileExt[20], stszPinFileExt[20];
   static int initialized=FALSE;
   char buf[MAXSTRING];
   int len=0;

   MakeQuiescent();

   if (firstCmd != NULL) {
      if (!OkToFlushUndoBuffer(
            TgLoadString(STID_CREATE_THUMBNAIL_CAUSE_FLUSH))) {
         SetCurChoice(curChoiceBeforeMakeQuiescent);
         return;
      }
   }
   if (!initialized) {
      sprintf(stszObjFileExt, ".%s", OBJ_FILE_EXT);
      sprintf(stszGzObjFileExt, ".%s.gz", OBJ_FILE_EXT);
      sprintf(stszSymFileExt, ".%s", SYM_FILE_EXT);
      sprintf(stszPinFileExt, ".%s", PIN_FILE_EXT);
      initialized = TRUE;
   }
   sprintf(buf, "%s;%s;%s;%s", stszObjFileExt, stszGzObjFileExt,
         stszSymFileExt, stszPinFileExt);
   len = strlen(buf);
   if (strcmp(OBJ_FILE_EXT,"obj") != 0) {
      strcat(buf, ";.obj");
   }
   strcat(buf, ";.obj.gz;.tgo;.tgo.gz");
   StartBrowse((curDirIsLocal ? curDir : curLocalDir), buf, strlen(buf),
         OBJ_ICON, NULL);
   CleanUpCmds();
}

void BrowseXBitmap()
{
   StartBrowse((curDirIsLocal ? curDir : curLocalDir), XBM_FILE_EXT,
         strlen(XBM_FILE_EXT), OBJ_XBM, NULL);
}

void BrowseXPixmap()
{
   StartBrowse((curDirIsLocal ? curDir : curLocalDir), XPM_FILE_EXT,
         strlen(XPM_FILE_EXT), OBJ_XPM, NULL);
}

void BrowseOther()
{
   int i, index;
   char *ext_str;
   struct ImportInfoRec *pii;
   XEvent ev;

   sprintf(gszMsgBox, TgLoadString(STID_WORKING_DIR_IS_GIVEN),
         (curDirIsLocal ? curDir : curLocalDir));
   SetStringStatus(gszMsgBox);
   if ((index=SelectAnImportFilter(NULL)) == INVALID) {
      return;
   }
   for (i=0, pii=topImportInfo; i < index && pii != NULL; i++, pii=pii->next) {
   }
   if (pii == NULL) return;
   XSync(mainDisplay, False);
   if (XCheckMaskEvent(mainDisplay, ExposureMask, &ev)) {
      ExposeEventHandler(&ev, TRUE);
   }
   if ((ext_str=SetUpExtStr((strlen(pii->ext)<<1)+3, pii->ext, "")) != NULL) {
      StartBrowse((curDirIsLocal ? curDir : curLocalDir), ext_str,
            strlen(pii->ext), OBJ_XPM, pii);
      free(ext_str);
   }
}

void BrowseOtherType(pszName)
   char *pszName;
{
   char *paren_ptr=(pszName == NULL ? NULL : strchr(pszName, ')')), *ext_str;
   struct ImportInfoRec *pii;

   if (paren_ptr == NULL) {
      sprintf(gszMsgBox, TgLoadString(STID_INVALID_FMT_IN_SHORTCUT_SPEC),
            "ImportOtherFileType", (pszName == NULL ? "" : pszName));
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return;
   }
   *paren_ptr = '\0';
   for (pii=topImportInfo; pii != NULL; pii=pii->next) {
      if (strcmp(pii->name, pszName) == 0) {
         break;
      }
   }
   if (pii == NULL) {
      sprintf(gszMsgBox, TgLoadString(STID_CANT_FIND_NAMED_IMP_FLTR), pszName);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return;
   }
   if ((ext_str=SetUpExtStr((strlen(pii->ext)<<1)+3, pii->ext, "")) != NULL) {
      StartBrowse((curDirIsLocal ? curDir : curLocalDir), ext_str,
            strlen(pii->ext), OBJ_XPM, pii);
      free(ext_str);
   }
}

/* ----------------------- Screen Capture Functions ----------------------- */

static
void FlushAllRedrawEvents()
{
   XEvent ev;

   XFlush(mainDisplay);
   XSync(mainDisplay, False);
   if (XCheckMaskEvent(mainDisplay, ExposureMask, &ev) ||
         XCheckMaskEvent(mainDisplay, VisibilityChangeMask, &ev)) {
      ExposeEventHandler(&ev, TRUE);
   }
}

static int hideDuringCapture=FALSE;

static XImage *capturedImage=NULL;
static Colormap *installedColormaps=NULL;
static int numInstalledColormaps=0;
static int capturedWidth=0, capturedHeight=0;

static
void CaptureCleanUp()
{
   if (capturedImage != NULL) XDestroyImage(capturedImage);
   if (installedColormaps != NULL) XFree(installedColormaps);
   capturedImage = NULL;
   installedColormaps = NULL;
   numInstalledColormaps = 0;
   capturedWidth = capturedHeight = 0;
}

static
int EnumPopupMenuProcToHide(menu_index, win, pUserData)
   int menu_index;
   Window win;
   void *pUserData;
   /* return FALSE will stop the enumeration */
{
   XUnmapWindow(mainDisplay, win);
   return TRUE;
}

static
void BeginHideDuringCapture()
{
   CaptureCleanUp();

   MakeQuiescent();
   if (hideDuringCapture) {
      EnumPopupMenuWindow(EnumPopupMenuProcToHide, NULL);
      XUnmapWindow(mainDisplay, mainWindow);
      FlushAllRedrawEvents();
   } else {
      SetWatchCursor(drawWindow);
      SetWatchCursor(mainWindow);
   }
   XBell(mainDisplay, 0);
}

static
int EnumPopupMenuProcToShow(menu_index, win, pUserData)
   int menu_index;
   Window win;
   void *pUserData;
   /* return FALSE will stop the enumeration */
{
   XMapWindow(mainDisplay, win);
   return TRUE;
}

static
int EnumPopupMenuProcToUpdate(menu_index, win, pUserData)
   int menu_index;
   Window win;
   void *pUserData;
   /* return FALSE will stop the enumeration */
{
   UpdatePinnedMenu(menu_index);
   return TRUE;
}

static
void EndHideDuringCapture()
{
   if (hideDuringCapture) {
      XEvent ev;

      EnumPopupMenuWindow(EnumPopupMenuProcToShow, NULL);
      XMapWindow(mainDisplay, mainWindow);
      XSync(mainDisplay, False);
      while (XCheckMaskEvent(mainDisplay, VisibilityChangeMask, &ev)) {
         ExposeEventHandler(&ev, TRUE);
      }
      EnumPopupMenuWindow(EnumPopupMenuProcToUpdate, NULL);
   } else {
      SetDefaultCursor(mainWindow);
      ShowCursor();
   }
   ShowCurChoiceMouseStatus(curChoice, 0, FALSE);
   XBell(mainDisplay, 0);
   XSync(mainDisplay, False);
   MillisecondSleep(120);
   XBell(mainDisplay, 0);
}

struct ObjRec *CreatePpmTrueObjFromImage(image, image_w, image_h,
      ppm_data, data_size)
   XImage *image;
   int image_w, image_h;
   char *ppm_data;
   unsigned int data_size;
{
   Pixmap pixmap=None, bitmap=None;
   XImage *bitmap_image=NULL;
   struct ObjRec *obj_ptr=NULL;
   struct XPmRec *xpm_ptr=NULL;

   pixmap = XCreatePixmap(mainDisplay, mainWindow, image_w, image_h, mainDepth);
   bitmap = XCreatePixmap(mainDisplay, dummyBitmap, image_w, image_h, 1);
   if (pixmap == None || bitmap == None) {
      if (pixmap != None) XFreePixmap(mainDisplay, pixmap);
      if (bitmap != None) XFreePixmap(mainDisplay, bitmap);
      sprintf(gszMsgBox, TgLoadString(STID_CANNOT_ALLOC_PIXMAP_OF_SIZE),
            image_w, image_h);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return NULL;
   }
   XSetForeground(mainDisplay, xbmGC, 1);
   XFillRectangle(mainDisplay, bitmap, xbmGC, 0, 0, image_w, image_h);
   XSetForeground(mainDisplay, xbmGC, 0);
   bitmap_image =  XGetImage(mainDisplay, bitmap, 0, 0, image_w, image_h, 1,
            ZPixmap);
   XPutImage(mainDisplay, pixmap, xpmGC, image, 0, 0, 0, 0, image_w, image_h);
   image = XGetImage(mainDisplay, pixmap, 0, 0, image_w, image_h, AllPlanes,
         ZPixmap);
   if (image == NULL || bitmap_image == NULL) {
      XFreePixmap(mainDisplay, pixmap);
      XFreePixmap(mainDisplay, bitmap);
      if (image != NULL) XDestroyImage(image);
      if (bitmap_image != NULL) XDestroyImage(bitmap_image);
      MsgBox(TgLoadString(STID_XGETIMAGE_MAY_RUN_OUT_VMEM), TOOL_NAME, INFO_MB);
      return NULL;
   }
   obj_ptr = CreateXPmObj(image_w, image_h, image_w, image_h, pixmap, image,
         bitmap, bitmap_image, 0, 0, FALSE, NULL, NULL, NULL, NULL);
   xpm_ptr = obj_ptr->detail.xpm;
   xpm_ptr->real_type = PPM_TRUE;
   xpm_ptr->ppm_data_compress = PPM_DATA_DEFLATED;
   xpm_ptr->ppm_data = ppm_data;
   xpm_ptr->ppm_data_size = data_size;
   xpm_ptr->ppm_mask_data = NULL;
   xpm_ptr->ppm_mask_size = 0;

   return obj_ptr;
}

static
void ImportCapturedImage()
{
   int no_image=FALSE;
   unsigned int data_size=0;
   char tmp_fname[MAXPATHLENGTH], ext[MAXPATHLENGTH], *ppm_data=NULL;
   Colormap saved_colormap=None;

   SetWatchCursor(drawWindow);
   SetWatchCursor(mainWindow);

   if (MkTempFile(tmp_fname, sizeof(tmp_fname), tmpDir, TOOL_NAME) == NULL) {
      return;
   }
   if (installedColormaps != NULL) {
      saved_colormap = mainColormap;
      mainColormap = (*installedColormaps);
   }
   if (!DumpXImageToFile(capturedImage, capturedWidth, capturedHeight,
         tmp_fname, ext)) {
      no_image = TRUE;
   } else if (strcmp(ext, ".ppm.z") == 0) {
      char deflated_fname[MAXPATHLENGTH];

      snprintf(deflated_fname, sizeof(deflated_fname), "%s%s", tmp_fname, ext);
      ppm_data = ReadFileIntoBuf(deflated_fname, &data_size);
      unlink(deflated_fname);
   }
   if (saved_colormap != None) {
      mainColormap = saved_colormap;
   }
   SetDefaultCursor(mainWindow);
   ShowCursor();

   if (!no_image) {
      if (ppm_data == NULL) {
         ImportGivenXPixmapFile(FALSE, NULL, tmp_fname,
               STID_GIVEN_XPM_SIZE_FILE_IMPORTED, STID_CANNOT_IMPORT_XPM_FILE,
               NULL, NULL);
      } else {
         struct ObjRec *obj_ptr=CreatePpmTrueObjFromImage(capturedImage,
               capturedWidth, capturedHeight, ppm_data, data_size);

         if (obj_ptr != NULL) {
            AddObj(NULL, topObj, obj_ptr);
            PlaceTopObj(obj_ptr, NULL, NULL);

            SelectTopObj();
            RecordNewObjCmd();
            SetFileModified(TRUE);
            justDupped = FALSE;
            if (!PRTGIF && colorLayers && needToRedrawColorWindow) {
               RedrawColorWindow();
            }
         }
      }
   }
   CaptureCleanUp();

   unlink(tmp_fname);
}

static
void CaptureUpdateBox(OrigX, OrigY, end_x, end_y)
   int OrigX, OrigY, end_x, end_y;
{
   char buf[80];
   int w=abs(OrigX-end_x), h=abs(OrigY-end_y);

   SelBox(rootWindow, revDefaultGC, OrigX, OrigY, end_x, end_y);
   sprintf(buf, "%dx%d", w, h);
   XDrawString(mainDisplay, rootWindow, revDefaultGC, end_x+18,
         end_y+defaultFontAsc, buf, strlen(buf));
}

static
void ContinueScreenCapture(OrigX, OrigY)
   int OrigX, OrigY;
{
   int done=FALSE;
   int end_x=OrigX, end_y=OrigY;

   XSetSubwindowMode(mainDisplay, revDefaultGC, IncludeInferiors);
   XGrabPointer(mainDisplay, rootWindow, FALSE,
         PointerMotionMask | ButtonReleaseMask,
         GrabModeAsync, GrabModeAsync, None, handCursor, CurrentTime);
   CaptureUpdateBox(OrigX, OrigY, end_x, end_y);
   while (!done) {
      XEvent input;

      XNextEvent(mainDisplay, &input);

      if (input.type == ButtonRelease) {
         XUngrabPointer(mainDisplay, CurrentTime);
         CaptureUpdateBox(OrigX, OrigY, end_x, end_y);
         done = TRUE;
      } else if (input.type == MotionNotify) {
         XEvent ev;
         int cur_x=input.xmotion.x, cur_y=input.xmotion.y;

         if (cur_x != end_x || cur_y != end_y) {
            CaptureUpdateBox(OrigX, OrigY, end_x, end_y);
            end_x = cur_x;
            end_y = cur_y;
            CaptureUpdateBox(OrigX, OrigY, end_x, end_y);
         }
         while (XCheckMaskEvent(mainDisplay, PointerMotionMask, &ev)) ;
      }
   }
   XSetSubwindowMode(mainDisplay, revDefaultGC, ClipByChildren);
   XSync(mainDisplay, False);

   if (OrigX != end_x && OrigY != end_y) {
      struct BBRec bbox;
      int dest_w=0, dest_h=0;

      SetBBRec(&bbox, OrigX, OrigY, end_x, end_y);
      dest_w = bbox.rbx - bbox.ltx;
      dest_h = bbox.rby - bbox.lty;

      capturedImage = XGetImage(mainDisplay, rootWindow, bbox.ltx, bbox.lty,
            dest_w, dest_h, AllPlanes, ZPixmap);
      if (capturedImage == NULL) {
         FailAllocMessage();
      } else {
         capturedWidth = dest_w;
         capturedHeight = dest_h;
         installedColormaps = XListInstalledColormaps(mainDisplay, rootWindow,
               &numInstalledColormaps);
         if (numInstalledColormaps == 0) {
            if (installedColormaps != NULL) XFree(installedColormaps);
            installedColormaps = NULL;
         } else if (numInstalledColormaps > 1) {
            TwoLineMsg(TgLoadString(STID_MORE_THAN_ONE_CMAP_INSTALLED),
                  TgLoadString(STID_FIRST_ONE_ON_LIST_USED));
         }
      }
   }
}

void ScreenCapture()
{
   int mouse_x=0, mouse_y=0;

   if (!colorDisplay && mainDepth <= 1) return;

   BeginHideDuringCapture();
   SetMouseStatus(TgLoadString(STID_SPECIFY_AREA),
         TgLoadCachedString(CSTID_CANCEL), TgLoadCachedString(CSTID_CANCEL));
   if (CornerLoop(&mouse_x, &mouse_y) == Button1) {
      ContinueScreenCapture(mouse_x, mouse_y);
   }
   EndHideDuringCapture();
   if (capturedImage != NULL) {
      ImportCapturedImage();
   }
}

static
void DoFullScreenCapture()
{
   struct BBRec bbox;
   int dpy_w=DisplayWidth(mainDisplay,mainScreen);
   int dpy_h=DisplayHeight(mainDisplay,mainScreen);

   SetBBRec(&bbox, 0, 0, dpy_w, dpy_h);

   capturedImage = XGetImage(mainDisplay, rootWindow, bbox.ltx, bbox.lty,
         dpy_w, dpy_h, AllPlanes, ZPixmap);
   if (capturedImage == NULL) {
      FailAllocMessage();
   } else {
      capturedWidth = dpy_w;
      capturedHeight = dpy_h;
      installedColormaps = XListInstalledColormaps(mainDisplay, rootWindow,
            &numInstalledColormaps);
      if (numInstalledColormaps == 0) {
         if (installedColormaps != NULL) XFree(installedColormaps);
         installedColormaps = NULL;
      } else if (numInstalledColormaps > 1) {
         TwoLineMsg(TgLoadString(STID_MORE_THAN_ONE_CMAP_INSTALLED),
               TgLoadString(STID_FIRST_ONE_ON_LIST_USED));
      }
   }
}

void FullScreenCapture()
{
   if (!colorDisplay && mainDepth <= 1) return;

   if (MsgBox(TgLoadString(STID_OK_TO_FULL_SCREEN_CAPTURE_YNC), TOOL_NAME,
         YNC_MB) != MB_ID_YES) {
      return;
   }
   FlushAllRedrawEvents();
   BeginHideDuringCapture();
   DoFullScreenCapture();
   EndHideDuringCapture();
   if (capturedImage != NULL) {
      ImportCapturedImage();
   }
}

void DelayedFullScreenCapture()
{
   char spec[80];
   int delay=0;

   if (!colorDisplay && mainDepth <= 1) return;

   *spec = '\0';
   if (Dialog(TgLoadString(STID_SPECIFY_DELAY_FULL_SCR_CAP), NULL, spec) ==
         INVALID) {
      return;
   }
   UtilTrimBlanks(spec);
   if (*spec == '\0') return;

   if (sscanf(spec, "%d", &delay) != 1) {
      sprintf(gszMsgBox, TgLoadString(STID_INVALID_SPEC_NUM_EXPECTED), spec);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return;
   }
   FlushAllRedrawEvents();
   BeginHideDuringCapture();
   if (delay != 0) {
      sleep(delay);
   }
   DoFullScreenCapture();
   EndHideDuringCapture();
   if (capturedImage != NULL) {
      ImportCapturedImage();
   }
}

void ToggleHideDuringCapture()
{
   if (!colorDisplay && mainDepth <= 1) return;

   hideDuringCapture = !hideDuringCapture;
   sprintf(gszMsgBox, TgLoadString(hideDuringCapture ?
         STID_WILL_HIDE_TOOL_WHILE_CAPTURE : STID_WILL_SHOW_TOOL_WHILE_CAPTURE),
         TOOL_NAME);
   Msg(gszMsgBox);
}

int RefreshScreenCaptureMenu(menu)
   TgMenu *menu;
{
   int ok=TRUE;

   if (!colorDisplay && mainDepth <= 1) {
      /* Screen Capture */
      ok &= TgEnableMenuItemById(menu, CMDID_SCREENCAPTURE, FALSE);
      /* Full Screen Capture */
      ok &= TgEnableMenuItemById(menu, CMDID_FULLSCREENCAPTURE, FALSE);
      /* Delayed Full Screen Capture */
      ok &= TgEnableMenuItemById(menu, CMDID_DELAYEDFULLSCREENCAPTURE, FALSE);
      /* Hide During Capture */
      ok &= TgEnableMenuItemById(menu, CMDID_TOGGLEHIDEDURINGCAPTURE, FALSE);
   }
   /* Hide During Capture */
   ok &= TgSetMenuItemCheckById(menu, CMDID_TOGGLEHIDEDURINGCAPTURE,
         hideDuringCapture);

   return ok;
}

TgMenu *CreateScreenCaptureMenu(parent_menu, x, y, menu_info, status_str_xlated)
   TgMenu *parent_menu;
   int x, y;
   TgMenuInfo *menu_info;
   int status_str_xlated; /* ignored, always 0 */
{
   TgMenu *menu=TgCreateMenuFromMenuInfo(parent_menu, x, y, menu_info, FALSE);

   if (menu != NULL) {
      if (!RefreshScreenCaptureMenu(menu)) {
         return TgDestroyMenu(menu, TRUE);
      }
      menu->refresh_proc = ((RefreshMenuFunc*)RefreshScreenCaptureMenu);
   }
   return menu;
}

void ToggleAutoEPSPreviewBitmap()
{
   autoEPSPreviewBitmap = !autoEPSPreviewBitmap;
   sprintf(gszMsgBox, TgLoadString(autoEPSPreviewBitmap ?
         STID_WILL_AUTO_GEN_PREVIEW_BITMAP : STID_WONT_AUTO_GEN_PREVIEW_BITMAP),
         TOOL_NAME);
   Msg(gszMsgBox);
}

int RefreshImportMenu(menu)
   TgMenu *menu;
{
   int ok=TRUE;

   /* Auto EPS Preview Bitmap */
   ok &= TgSetMenuItemCheckById(menu, CMDID_TOGGLEAUTOEPSPREVIEWBITMAP,
         autoEPSPreviewBitmap);

   return ok;
}

TgMenu *CreateImportMenu(parent_menu, x, y, menu_info, status_str_xlated)
   TgMenu *parent_menu;
   int x, y;
   TgMenuInfo *menu_info;
   int status_str_xlated; /* ignored, always 0 */
{
   TgMenu *menu=TgCreateMenuFromMenuInfo(parent_menu, x, y, menu_info, FALSE);

   if (menu != NULL) {
      if (!RefreshImportMenu(menu)) {
         return TgDestroyMenu(menu, TRUE);
      }
      menu->refresh_proc = ((RefreshMenuFunc*)RefreshImportMenu);
   }
   return menu;
}
