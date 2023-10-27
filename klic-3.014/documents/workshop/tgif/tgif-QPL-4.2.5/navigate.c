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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/navigate.c,v 1.12 2011/05/16 16:21:58 william Exp $
 */

#define _INCLUDE_FROM_NAVIGATE_C_

#include "tgifdefs.h"
#include "cmdids.h"

#include "attr.e"
#include "auxtext.e"
#include "button.e"
#include "choice.e"
#include "choose.e"
#include "cmd.e"
#include "color.e"
#include "cursor.e"
#include "cutpaste.e"
#include "dialog.e"
#include "drawing.e"
#include "exec.e"
#include "file.e"
#include "font.e"
#include "mainloop.e"
#include "mainmenu.e"
#include "menu.e"
#include "menuinfo.e"
#include "msg.e"
#include "names.e"
#include "navigate.e"
#include "obj.e"
#include "page.e"
#include "pattern.e"
#include "raster.e"
#include "rect.e"
#include "remote.e"
#include "scroll.e"
#include "select.e"
#include "setup.e"
#include "stk.e"
#include "strtbl.e"
#include "util.e"
#include "xpixmap.e"

int navigatingBackAndForth=FALSE;
int inHyperSpace=FALSE;
int inSlideShow=FALSE;
int visibleGridInSlideShow=FALSE;
int slideShowInfoValid=FALSE;
char *slideShowBorderColor=NULL;
int slideShowXOffset=0;
int slideShowYOffset=0;
int autoHyperSpaceOnRemote=TRUE;
int allowLaunchInHyperSpace=FALSE;
int navigateRefresh=FALSE;
int goHyperSpaceInSlideShow=FALSE;

static int validHotListFileName=FALSE;
static char *hotListFileName=NULL;

struct NavigateRec {
   struct StkRec *stk;
   struct NavigateRec *next, *prev;
   char *full_fname;
   char *doc_name;
   int cur_page_num, orig_x, orig_y, zoom_scale, zoomed_in;
};

static struct NavigateRec *firstNavigate=NULL, *lastNavigate=NULL;
static struct NavigateRec *curNavigate=NULL;

static struct NavigateRec *firstSessionHistory=NULL, *lastSessionHistory=NULL;

static struct URLCacheRec *firstURLCache=NULL, *lastURLCache=NULL;
static int maxURLCache=(-1), curURLCache=0;

static int lineWidthIndexInSlideShow=4;
static int makeUnsavableInSlideShow=FALSE;
static int ignoreSlideShowOffsetsInFile=TRUE;
static int hideWindowsInSlideShow=TRUE;

static int savedSlideShowXOffset=0;
static int savedSlideShowYOffset=0;
static int savedSlideShowOffsets=FALSE;

int ParseXYSpec(spec, pnx, pny)
   char *spec;
   int *pnx, *pny;
{
   char *psz_dx=NULL, *psz_dy=NULL, *psz_dup_spec=NULL;
   int dx, dy;

   UtilTrimBlanks(spec);
   if (*spec == '\0') return FALSE;
   if ((psz_dup_spec=UtilStrDup(spec)) == NULL) return FailAllocMessage();

   if ((psz_dx=strtok(psz_dup_spec, " ,\t\n\r")) == NULL ||
         sscanf(psz_dx, "%d", &dx) != 1) {
      sprintf(gszMsgBox, TgLoadString(STID_ERR_IN_PARSE_STR_FOR_2_VAL), spec);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      free(psz_dup_spec);
      return FALSE;
   }
   psz_dy = strtok(NULL, " ,\t\n\r");
   if (psz_dy == NULL) {
      dy = dx;
   } else {
      if (sscanf(psz_dy, "%d", &dy) != 1) {
         sprintf(gszMsgBox, TgLoadString(STID_ERR_IN_PARSE_STR_FOR_2_VAL),
               spec);
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
         free(psz_dup_spec);
         return FALSE;
      }
   }
   if (pnx != NULL) *pnx = dx;
   if (pny != NULL) *pny = dy;

   free(psz_dup_spec);
   return TRUE;
}

static
int ParseWHSpecError(spec, psz_dup_spec, rc)
   char *spec;
   char *psz_dup_spec;
   int rc;
{
   if (!rc) {
      sprintf(gszMsgBox, TgLoadString(STID_ERR_IN_PARSE_STR_FOR_2_VAL), spec);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
   }
   UtilFree(psz_dup_spec);

   return rc;
}

int ParseWHSpec(spec, pnw, pnh)
   char *spec;
   int *pnw, *pnh;
{
   char *psz_h=NULL, *psz_dup_spec=NULL;
   int w=0, h=0;

   UtilTrimBlanks(spec);
   if (*spec == '\0') return FALSE;
   if ((psz_dup_spec=UtilStrDup(spec)) == NULL) return FailAllocMessage();

   UtilStrLower(psz_dup_spec);
   if ((psz_h=strchr(psz_dup_spec, 'x')) == NULL) {
      if ((psz_h=strchr(psz_dup_spec, ' ')) == NULL) {
         return ParseWHSpecError(spec, psz_dup_spec, FALSE);
      }
      *psz_h++ = '\0';
   } else {
      *psz_h++ = '\0';
   }
   UtilTrimBlanks(psz_dup_spec);
   UtilTrimBlanks(psz_h);
   if (!GetDimension(psz_dup_spec, FALSE, &w)) {
      return ParseWHSpecError(spec, psz_dup_spec, FALSE);
   }
   if (!GetDimension(psz_h, FALSE, &h)) {
      return ParseWHSpecError(spec, psz_dup_spec, FALSE);
   }
   if (pnw != NULL) *pnw = w;
   if (pnh != NULL) *pnh = h;

   free(psz_dup_spec);
   return TRUE;
}

void CleanUpSlideShowInfo()
{
   if (slideShowBorderColor != NULL) UtilFree(slideShowBorderColor);
   slideShowBorderColor = NULL;
   slideShowXOffset = slideShowYOffset = 0;
   slideShowInfoValid = FALSE;
}

static
void InsertNavigate(pnr_prev, pnr_next, pnr)
   struct NavigateRec *pnr_prev, *pnr_next, *pnr;
{
   pnr->prev = pnr_prev;
   pnr->next = pnr_next;

   if (pnr_prev == NULL) {
      firstNavigate = pnr;
   } else {
      pnr_prev->next = pnr;
   }

   if (pnr_next == NULL) {
      lastNavigate = pnr;
   } else {
      pnr_next->prev = pnr;
   }
}

static
void DeleteNavigate(pnr)
   struct NavigateRec *pnr;
{
   if (pnr->full_fname != NULL) free(pnr->full_fname);
   if (pnr->doc_name != NULL) free(pnr->doc_name);
   free(pnr);
}

static
void ClearNavigateRecords(pnr)
   struct NavigateRec *pnr;
{
   struct NavigateRec *pnr_next;

   for (; pnr != NULL; pnr=pnr_next) {
      pnr_next = pnr->next;
      DeleteNavigate(pnr);
   }
   lastNavigate = curNavigate;
   if (lastNavigate == NULL) firstNavigate = NULL;
}

static
void ClearSessionHistory()
{
   struct NavigateRec *pnr, *pnr_prev;

   for (pnr=lastSessionHistory; pnr != NULL; pnr=pnr_prev) {
      pnr_prev = pnr->prev;
      if (pnr->full_fname != NULL) free(pnr->full_fname);
      if (pnr->doc_name != NULL) free(pnr->doc_name);
      free(pnr);
   }
   lastSessionHistory = firstSessionHistory = NULL;
}

static
void FreeAnURLCache(url_cache)
   struct URLCacheRec *url_cache;
{
   if (url_cache == NULL) return;
   if (url_cache->remote_buf != NULL) free(url_cache->remote_buf);
   if (url_cache->content_type != NULL) free(url_cache->content_type);
   if (url_cache->simple_url_name != NULL) free(url_cache->simple_url_name);
   free(url_cache);
}

static
void CleanUpURLCache()
{
   struct URLCacheRec *next_cache;

   for ( ; firstURLCache != NULL; firstURLCache=next_cache) {
      next_cache = firstURLCache->next;
      FreeAnURLCache(firstURLCache);
   }
   curURLCache = 0;
   firstURLCache = lastURLCache = NULL;
}

void CleanUpNavigate()
{
   curNavigate = NULL;
   ClearNavigateRecords(firstNavigate);
   if (hotListFileName != NULL) free(hotListFileName);
   hotListFileName = NULL;
   validHotListFileName = FALSE;
   ClearSessionHistory();

   CleanUpURLCache();
   CleanUpSlideShowInfo();

   savedSlideShowXOffset = 0;
   savedSlideShowYOffset = 0;
   savedSlideShowOffsets = FALSE;
}

void InitNavigate()
{
   char *c_ptr=NULL, spec[MAXSTRING];

   visibleGridInSlideShow = FALSE;
   if ((c_ptr=XGetDefault(mainDisplay, TOOL_NAME,
         "VisibleGridInSlideShow")) != NULL &&
         UtilStrICmp(c_ptr, "true") == 0) {
      visibleGridInSlideShow = TRUE;
   }
   goHyperSpaceInSlideShow = FALSE;
   if ((c_ptr=XGetDefault(mainDisplay, TOOL_NAME, "GoHyperSpaceInSlideShow")) !=
         NULL && UtilStrICmp(c_ptr, "true") == 0) {
      goHyperSpaceInSlideShow = TRUE;
   }
   lineWidthIndexInSlideShow = 4;
   if ((c_ptr=XGetDefault(mainDisplay, TOOL_NAME,
         "LineWidthIndexInSlideShow")) != NULL) {
      lineWidthIndexInSlideShow = atoi(c_ptr);
      if (lineWidthIndexInSlideShow < 0 ||
            lineWidthIndexInSlideShow >= maxLineWidths) {
         fprintf(stderr, TgLoadString(STID_INVALID_XDEF_USE_ALT_VALUE),
               TOOL_NAME, "LineWidthIndexInSlideShow", c_ptr, 4);
         lineWidthIndexInSlideShow = 4;
      }
   }
   if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"SlideShowWindowOffsets")) !=
         NULL) {
      UtilStrCpyN(spec, sizeof(spec), c_ptr);
      UtilTrimBlanks(spec);
      if (ParseXYSpec(spec, &slideShowXOffset, &slideShowYOffset)) {
         slideShowInfoValid = TRUE;

         savedSlideShowXOffset = slideShowXOffset;
         savedSlideShowYOffset = slideShowYOffset;
         savedSlideShowOffsets = TRUE;
      } else {
         fprintf(stderr, TgLoadString(STID_INVALID_XDEF_USE_ALT_STR),
               TOOL_NAME, "SlideShowWindowOffsets", spec, "0,0");
         fprintf(stderr, "\n");
      }
   }
   if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"SlideShowBorderColor")) !=
         NULL) {
      int new_alloc=0;

      UtilStrCpyN(spec, sizeof(spec), c_ptr);
      UtilTrimBlanks(spec);

      if (QuickFindColorIndex(NULL, spec, &new_alloc, FALSE) == INVALID) {
         fprintf(stderr, TgLoadString(STID_INVALID_XDEF_COLORXPM_GET),
               TOOL_NAME, "SlideShowBorderColor", spec);
         fprintf(stderr, "\n");
         return;
      }
      if (slideShowBorderColor != NULL) UtilFree(slideShowBorderColor);
      slideShowBorderColor = UtilStrDup(spec);
      if (slideShowBorderColor == NULL) FailAllocMessage();
      slideShowInfoValid = TRUE;
   }
   makeUnsavableInSlideShow = FALSE;
   if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"MakeUnsavableInSlideShow")) !=
         NULL && UtilStrICmp(c_ptr, "true") == 0) {
      makeUnsavableInSlideShow = TRUE;
   }
   ignoreSlideShowOffsetsInFile = TRUE;
   if ((c_ptr=XGetDefault(mainDisplay, TOOL_NAME,
         "IgnoreSlideShowOffsetsInFile")) != NULL &&
         UtilStrICmp(c_ptr, "false") == 0) {
      ignoreSlideShowOffsetsInFile = FALSE;
   }
   hideWindowsInSlideShow = TRUE;
   if ((c_ptr=XGetDefault(mainDisplay, TOOL_NAME,
         "HideWindowsInSlideShow")) != NULL &&
         UtilStrICmp(c_ptr, "false") == 0) {
      hideWindowsInSlideShow = FALSE;
   }
}

static
void InitURLCache()
{
   if (maxURLCache == (-1)) {
      char *c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"MaxNavigateCacheBuffers");

      maxURLCache = 40;
      if (c_ptr != NULL) {
         maxURLCache = atoi(c_ptr);
         if (maxURLCache < 0) {
            fprintf(stderr, TgLoadString(STID_INVALID_XDEF_USE_ALT_VALUE),
                  TOOL_NAME, "MaxNavigateCacheBuffers", c_ptr, 40);
            maxURLCache = 40;
         }
      }
      curURLCache = 0;
      firstURLCache = lastURLCache = NULL;
   }
}

static
char *SimpleURLName(psz_url)
   char *psz_url;
{
   char *c_ptr=strchr(psz_url, '#');

   if (c_ptr != NULL) {
      char *return_buf;

      *c_ptr = '\0';
      return_buf = UtilStrDup(psz_url);
      *c_ptr = '#';
      return return_buf;
   }
   return UtilStrDup(psz_url);
}

static
void UnlinkURLCache(url_cache)
   struct URLCacheRec *url_cache;
{
   if (url_cache == NULL) return;
   if (url_cache->prev == NULL) {
      firstURLCache = url_cache->next;
   } else {
      url_cache->prev->next = url_cache->next;
   }
   if (url_cache->next == NULL) {
      lastURLCache = url_cache->prev;
   } else {
      url_cache->next->prev = url_cache->prev;
   }
   url_cache->prev = url_cache->next = NULL;
   curURLCache--;
}

static
void InsertURLCache(prev_url, next_url, url_cache)
   struct URLCacheRec *prev_url, *next_url, *url_cache;
{
   url_cache->prev = prev_url;
   url_cache->next = next_url;
   if (prev_url == NULL) {
      firstURLCache = url_cache;
   } else {
      prev_url->next = url_cache;
   }
   if (next_url == NULL) {
      lastURLCache = url_cache;
   } else {
      next_url->prev = url_cache;
   }
   curURLCache++;
}

void UpdateLRU(url_cache)
   struct URLCacheRec *url_cache;
{
   UnlinkURLCache(url_cache);
   InsertURLCache(lastURLCache, NULL, url_cache);
}

struct URLCacheRec *FindURLCache(psz_url, update_lru)
   char *psz_url;
   int update_lru;
{
   char *simple_url_name=NULL;
   struct URLCacheRec *url_cache;

   if (PRTGIF) return NULL;

   simple_url_name = SimpleURLName(psz_url);
   InitURLCache();
   if (simple_url_name == NULL) return NULL;
   for (url_cache=lastURLCache; url_cache != NULL; url_cache=url_cache->prev) {
      if (strcmp(simple_url_name, url_cache->simple_url_name) == 0) {
         free(simple_url_name);
         if (!navigatingBackAndForth) {
            /*
             * Should get HEAD of the URL to see if Last-modified has changed.
             */
         }
         if (update_lru) UpdateLRU(url_cache);
         return url_cache;
      }
   }
   free(simple_url_name);
   return NULL;
}

void UpdateURLCache(psz_url, psz_remote_buf, psz_content_type, remote_buf_sz,
      is_html)
   char *psz_url, *psz_remote_buf, *psz_content_type;
   int remote_buf_sz, is_html;
{
   char *simple_url_name=NULL;
   struct URLCacheRec *url_cache;

   if (PRTGIF) return;

   simple_url_name = SimpleURLName(psz_url);
   InitURLCache();
   if (simple_url_name == NULL) return;
   for (url_cache=lastURLCache; url_cache != NULL; url_cache=url_cache->prev) {
      if (strcmp(simple_url_name, url_cache->simple_url_name) == 0) {
         break;
      }
   }
   if (url_cache != NULL) {
      UnlinkURLCache(url_cache);
      FreeAnURLCache(url_cache);
   } else {
      if (curURLCache >= maxURLCache) {
         url_cache = firstURLCache;
         UnlinkURLCache(url_cache);
         FreeAnURLCache(url_cache);
      }
   }
   url_cache = (struct URLCacheRec*)malloc(sizeof(struct URLCacheRec));
   if (url_cache == NULL) {
      FailAllocMessage();
      free(simple_url_name);
      return;
   }
   memset(url_cache, 0, sizeof(struct URLCacheRec));
   url_cache->remote_buf_sz = remote_buf_sz;
   url_cache->is_html = is_html;
   url_cache->remote_buf = UtilStrDup(psz_remote_buf);
   url_cache->content_type = UtilStrDup(psz_content_type);
   url_cache->simple_url_name = simple_url_name;
   InsertURLCache(lastURLCache, NULL, url_cache);
}

static
int InitHotListFileName()
{
   if (!validHotListFileName)
   {
      char *c_ptr;

      hotListFileName = NULL;
      if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"HotListFileName")) !=
            NULL) {
         hotListFileName = UtilStrDup(c_ptr);
      } else {
         int len=strlen(homeDir)+strlen(TOOL_NAME)+20;

         hotListFileName = (char*)malloc((len+1)*sizeof(char));
         if (hotListFileName == NULL) FailAllocMessage();
         sprintf(hotListFileName, "%s%c.%s_hotlist", homeDir, DIR_SEP,
               TOOL_NAME);
      }
   }
   validHotListFileName = TRUE;
   return TRUE;
}

void BeforeNavigate()
{
   if (curFileDefined && curNavigate != NULL) {
      curNavigate->cur_page_num = curPageNum;
      curNavigate->orig_x = drawOrigX;
      curNavigate->orig_y = drawOrigY;
      curNavigate->zoom_scale = zoomScale;
      curNavigate->zoomed_in = zoomedIn;
   }
}

static
void AddToSessionHistory(cur_pnr)
   struct NavigateRec *cur_pnr;
{
   struct NavigateRec *pnr;

   pnr = (struct NavigateRec *)malloc(sizeof(struct NavigateRec));
   if (pnr == NULL) FailAllocMessage();
   memset(pnr, 0, sizeof(struct NavigateRec));

   pnr->prev = lastSessionHistory;
   pnr->next = NULL;
   pnr->stk = NULL;
   pnr->full_fname = (cur_pnr->full_fname==NULL ? NULL :
         UtilStrDup(cur_pnr->full_fname));
   pnr->doc_name = (cur_pnr->doc_name==NULL ? NULL :
         UtilStrDup(cur_pnr->doc_name));

   if (lastSessionHistory == NULL) {
      firstSessionHistory = pnr;
   } else {
      lastSessionHistory->next = pnr;
   }
   lastSessionHistory = pnr;
}

void CommitNavigate()
   /* add a navigation record at the end */
{
   int len;

   if (curNavigate == NULL) {
      ClearNavigateRecords(firstNavigate);
   } else if (curNavigate != lastNavigate) {
      ClearNavigateRecords(curNavigate->next);
   }

   if (!curFileDefined) return;

   curNavigate = (struct NavigateRec *)malloc(sizeof(struct NavigateRec));
   if (curNavigate == NULL) FailAllocMessage();
   memset(curNavigate, 0, sizeof(struct NavigateRec));
   curNavigate->stk = NULL;
   curNavigate->next = curNavigate->prev = NULL;
   curNavigate->full_fname = curNavigate->doc_name = NULL;
   curNavigate->cur_page_num = 1;
   curNavigate->orig_x = curNavigate->orig_y = 0;
   curNavigate->zoom_scale = 0;
   curNavigate->zoomed_in = FALSE;

   len = strlen(curDir)+1+strlen(curFileName);

   curNavigate->full_fname = (char*)malloc((len+1)*sizeof(char));
   if (curNavigate->full_fname == NULL) FailAllocMessage();
   sprintf(curNavigate->full_fname, "%s%c%s", curDir, DIR_SEP, curFileName);
   if (firstPage != NULL && firstPage->name != NULL) {
      curNavigate->doc_name = UtilStrDup(firstPage->name);
   } else {
      curNavigate->doc_name = NULL;
   }
   AddARecentlyUsedFile(curNavigate->full_fname);
   curNavigate->stk = topStk;

   InsertNavigate(lastNavigate, NULL, curNavigate);
   curNavigate = lastNavigate;
   AddToSessionHistory(curNavigate);
}

static
void PostNavigate(pnr)
   struct NavigateRec *pnr;
{
   struct AttrRec *exec_attr=FindFileAttrWithName("auto_exec=");

   if (exec_attr != NULL) {
      ClearObjCachesInAllPages();
      DoExecLoop(NULL, exec_attr);
   } else if (pnr != NULL) {
      ScrollToSpecifiedOrigin(pnr->cur_page_num, pnr->orig_x, pnr->orig_y,
            pnr->zoom_scale, pnr->zoomed_in);
      ClearObjCachesInAllPages();
   }
}

static
void NavigateTo(full_fname, do_not_save, force_load)
   char *full_fname;
   int do_not_save, force_load;
{
   if (force_load) navigateRefresh = TRUE;

   if (FileIsRemote(full_fname)) {
      char *buf=NULL, *content_type=NULL, final_url[MAXPATHLENGTH+1];
      int rc, buf_sz=0, is_html=FALSE;

      *final_url = '\0';
      SetWatchCursor(drawWindow);
      SetWatchCursor(mainWindow);
      SaveStatusStrings();
      rc = LoadRemoteFileInMem(full_fname, &buf, &content_type, &buf_sz,
            &is_html, force_load, final_url, sizeof(final_url));
      RestoreStatusStrings();
      SetDefaultCursor(mainWindow);
      ShowCursor();
      if (rc && buf != NULL) {
         navigatingBackAndForth = TRUE;
         if (*final_url != '\0') {
            LoadRemoteFileFromMem(final_url, buf, content_type, buf_sz,
                  is_html);
         } else {
            LoadRemoteFileFromMem(full_fname, buf, content_type, buf_sz,
                  is_html);
         }
         navigatingBackAndForth = FALSE;
      } else if (do_not_save) {
         SetFileModified(TRUE);
      }
      if (content_type != NULL) FreeRemoteBuf(content_type);
      if (buf != NULL) FreeRemoteBuf(buf);
   } else {
      int obj_file=FALSE, gzipped=FALSE;

      navigatingBackAndForth = TRUE;
      obj_file = FileNameHasExtension(full_fname, OBJ_FILE_TYPE, &gzipped,
            NULL);
      if (!LoadFile(full_fname, obj_file, obj_file && gzipped)) {
         if (do_not_save) {
            SetFileModified(TRUE);
         }
      }
      navigatingBackAndForth = FALSE;
   }
   navigateRefresh = FALSE;
}

void NavigateBack()
{
   int do_not_save=FALSE;
   struct NavigateRec nr;

   if (curNavigate != NULL && curNavigate->stk != NULL &&
         curNavigate->stk == topStk && (curNavigate->prev == NULL ||
         (curNavigate->prev != NULL && curNavigate->prev->stk != topStk))) {
      /* curNavigate->stk = NULL; */
      /* if (curFileDefined) curNavigate = curNavigate->prev; */
      PopIcon();
      return;
   }
   while (!DirIsRemote(curDir) && fileModified && !IsFiletUnSavable()) {
      XBell(mainDisplay, 0);
      switch (MsgBox(TgLoadString(STID_FILE_MOD_SAVE_BEFORE_BACK), TOOL_NAME,
            YNC_MB)) {
      case MB_ID_YES: SaveFile(); break;
      case MB_ID_NO: do_not_save=TRUE; SetFileModified(FALSE); break;
      case MB_ID_CANCEL: return;
      }
   }
   if (!DirIsRemote(curDir) && fileModified && IsFiletUnSavable()) {
      do_not_save = TRUE;
      SetFileModified(FALSE);
   }
   MakeQuiescent();
   if (curNavigate == NULL || (curFileDefined && curNavigate->prev == NULL)) {
      MsgBox(TgLoadString(STID_NOTHING_TO_GO_BACK_TO), TOOL_NAME, INFO_MB);
      if (do_not_save) SetFileModified(TRUE);
      return;
   }
   BeforeNavigate();
   if (curFileDefined) curNavigate = curNavigate->prev;
   memcpy(&nr, curNavigate, sizeof(struct NavigateRec));
   NavigateTo(curNavigate->full_fname, do_not_save, FALSE);
   PostNavigate(&nr);
   SetCurChoice(curChoiceBeforeMakeQuiescent);
}

void NavigateForward()
{
   int do_not_save=FALSE;
   struct NavigateRec nr;

   while (!DirIsRemote(curDir) && fileModified && !IsFiletUnSavable()) {
      XBell(mainDisplay, 0);
      switch (MsgBox(TgLoadString(STID_FILE_MOD_SAVE_BEFORE_FORWARD), TOOL_NAME,
            YNC_MB)) {
      case MB_ID_YES: SaveFile(); break;
      case MB_ID_NO: do_not_save=TRUE; SetFileModified(FALSE); break;
      case MB_ID_CANCEL: return;
      }
   }
   if (!DirIsRemote(curDir) && fileModified && IsFiletUnSavable()) {
      do_not_save = TRUE;
      SetFileModified(FALSE);
   }
   MakeQuiescent();
   if (curNavigate == lastNavigate) {
      MsgBox(TgLoadString(STID_NOTHING_TO_GO_FORWARD_TO), TOOL_NAME, INFO_MB);
      if (do_not_save) SetFileModified(TRUE);
      return;
   }
   BeforeNavigate();
   if (curFileDefined) curNavigate = curNavigate->next;
   memcpy(&nr, curNavigate, sizeof(struct NavigateRec));
   NavigateTo(curNavigate->full_fname, do_not_save, FALSE);
   PostNavigate(&nr);
   SetCurChoice(curChoiceBeforeMakeQuiescent);
}

void AdjustNavigate()
{
   while (curNavigate != NULL && curNavigate->stk != NULL &&
         curNavigate->stk == topStk) {
      curNavigate->stk = NULL;
      if (curNavigate->prev == NULL || (curNavigate->prev != NULL &&
            curNavigate->prev->stk != topStk)) {
         if (curFileDefined && curNavigate->prev != NULL) {
            curNavigate = curNavigate->prev;
         }
         return;
      } else {
         curNavigate = curNavigate->prev;
      }
   }
   sprintf(gszMsgBox, TgLoadString(STID_FAIL_TO_FIND_RECORD_IN_FUNC),
         "AdjustNavigate()");
   FatalUnexpectedError(gszMsgBox,
         TgLoadString(STID_FURTHER_NAVIGATE_MAY_CRASH));
}

void NavigateRefresh()
{
   if (!curFileDefined) {
      MsgBox(TgLoadString(STID_CANT_REFRESH_UNDEFINED_FILE), TOOL_NAME,
            INFO_MB);
      return;
   }
   while (!DirIsRemote(curDir) && fileModified && !IsFiletUnSavable()) {
      XBell(mainDisplay, 0);
      switch (MsgBox(TgLoadString(STID_FILE_MOD_SAVE_BEFORE_RELOAD), TOOL_NAME,
            YNC_MB)) {
      case MB_ID_YES: SaveFile(); break;
      case MB_ID_NO: SetFileModified(FALSE); break;
      case MB_ID_CANCEL: return;
      }
   }
   if (!DirIsRemote(curDir) && fileModified && IsFiletUnSavable()) {
      SetFileModified(FALSE);
   }
   NavigateTo(curNavigate->full_fname, TRUE, TRUE);
   PostNavigate(NULL);
}

static
char **ReadHotListFile(pn_count)
   int *pn_count;
{
   FILE *fp;
   char **ppsz_buf=NULL, *buf;
   int num_lines=0;

   if (pn_count != NULL) *pn_count = 0;
   if (!InitHotListFileName() || hotListFileName==NULL) {
      sprintf(gszMsgBox, TgLoadString(STID_HOT_LIST_FILE_UNDEF_PLS_SPEC),
            TOOL_NAME, "HotListFileName");
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return NULL;
   }
   if ((ppsz_buf=(char**)malloc((num_lines+1)*sizeof(char*))) == NULL) {
      FailAllocMessage();
      return NULL;
   }
   if ((fp=fopen(hotListFileName, "r")) == NULL) {
      ppsz_buf[num_lines] = NULL;
      return ppsz_buf;
   }
   while ((buf=UtilGetALine(fp)) != NULL) {
      if ((ppsz_buf=(char**)realloc(ppsz_buf,
            ((++num_lines)+1)*sizeof(char*))) == NULL) {
         FailAllocMessage();
         return NULL;
      }
      ppsz_buf[num_lines-1] = buf;
   }
   ppsz_buf[num_lines] = NULL;
   if ((num_lines & 0x1) != 0) {
      ppsz_buf[num_lines-1] = NULL;
      sprintf(gszMsgBox, TgLoadString(STID_MALFORMED_NAMED_HOT_LIST_FILE),
            hotListFileName);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
   }
   fclose(fp);
   if (pn_count != NULL) *pn_count = (num_lines>>1);
   return ppsz_buf;
}

static
DspList *HotListListing(ppsz_buf, pn_count)
   char **ppsz_buf;
   int *pn_count;
{
   int i;
   char **s_ptr;
   DspList *pdl, *dsp_ptr;

   *pn_count = 0;
   for (s_ptr=ppsz_buf; *s_ptr != NULL; s_ptr=(&s_ptr[2])) {
      (*pn_count)++;
   }
   pdl = (DspList*)malloc((*pn_count)*sizeof(DspList));
   if (pdl == NULL) FailAllocMessage();
   memset(pdl, 0, (*pn_count)*sizeof(DspList));
   for (i=(*pn_count)-1, dsp_ptr=pdl, s_ptr=ppsz_buf; *s_ptr != NULL;
         i--, dsp_ptr++, s_ptr=(&s_ptr[2])) {
      if (**s_ptr == '\0') {
         UtilStrCpyN(dsp_ptr->itemstr, sizeof(dsp_ptr->itemstr), s_ptr[1]);
         dsp_ptr->directory = FALSE; /* use file name as title */
      } else {
         UtilStrCpyN(dsp_ptr->itemstr, sizeof(dsp_ptr->itemstr), s_ptr[0]);
         dsp_ptr->directory = TRUE; /* file has a title */
      }
      UtilStrCpyN(dsp_ptr->pathstr, sizeof(dsp_ptr->pathstr), s_ptr[1]);
      dsp_ptr->next = (i==0 ? NULL : &dsp_ptr[1]);
   }
   return pdl;
}

static
DspList *HistoryListing(pn_count)
   int *pn_count;
{
   int i;
   struct NavigateRec *pnr;
   DspList *pdl, *dsp_ptr;

   *pn_count = 0;
   for (pnr=lastSessionHistory; pnr != NULL; pnr=pnr->prev) {
      (*pn_count)++;
   }
   if (*pn_count == 0) return NULL;
   pdl = (DspList*)malloc((*pn_count)*sizeof(DspList));
   if (pdl == NULL) FailAllocMessage();
   memset(pdl, 0, (*pn_count)*sizeof(DspList));
   for (i=0, dsp_ptr=pdl, pnr=firstSessionHistory; pnr != NULL;
         i++, dsp_ptr++, pnr=pnr->next) {
      if (pnr->doc_name == NULL) {
         UtilStrCpyN(dsp_ptr->itemstr, sizeof(dsp_ptr->itemstr),
               pnr->full_fname);
         dsp_ptr->directory = FALSE; /* use file name as title */
      } else {
         UtilStrCpyN(dsp_ptr->itemstr, sizeof(dsp_ptr->itemstr), pnr->doc_name);
         dsp_ptr->directory = TRUE; /* file has a title */
      }
      UtilStrCpyN(dsp_ptr->pathstr, sizeof(dsp_ptr->pathstr), pnr->full_fname);
      dsp_ptr->next = (i==(*pn_count)-1 ? NULL : &dsp_ptr[1]);
   }
   return pdl;
}

struct HotListInfoRec {
   char **ppsz_buf;
};

#define GOTO_BUTTON 101
#define DELETE_BUTTON 102
#define CLOSE_BUTTON 103

static
int GetHotListEntries(p_dsp_ptr, p_entries, pn_num_entries, pn_marked_index,
      pp_check_array, cur_buf, p_void)
   DspList **p_dsp_ptr;
   char ***p_entries, *cur_buf;
   int *pn_num_entries, *pn_marked_index;
   struct CheckArrayRec **pp_check_array;
   void *p_void;
{
   struct HotListInfoRec *p_hlinfo=(struct HotListInfoRec *)p_void;

   p_hlinfo->ppsz_buf = ReadHotListFile(NULL);

   if (p_hlinfo->ppsz_buf == NULL) {
      return FALSE;
   } else if (*p_hlinfo->ppsz_buf == NULL) {
      MsgBox(TgLoadString(STID_HOT_LIST_FILE_IS_EMPTY), TOOL_NAME, INFO_MB);
      free(p_hlinfo->ppsz_buf);
      p_hlinfo->ppsz_buf = NULL;
      return FALSE;
   } else {
      *p_dsp_ptr = HotListListing(p_hlinfo->ppsz_buf, pn_num_entries);

      ignoreDirectoryFlag = TRUE;
      *p_entries = MakeNameDspItemArray(*pn_num_entries, *p_dsp_ptr);
      ignoreDirectoryFlag = FALSE;
   }
   return TRUE;
}

static
int HotListAfterLoop(p_dsp_ptr, p_entries, pn_num_entries, pn_marked_index,
      pp_check_array, cur_buf, btn_id, selected_index, p_void)
   DspList **p_dsp_ptr;
   char ***p_entries, *cur_buf;
   int *pn_num_entries, *pn_marked_index, btn_id, selected_index;
   struct CheckArrayRec **pp_check_array;
   void *p_void;
{
   int i, modified=FALSE, something_deleted=FALSE, goto_something=FALSE;
   int navigated_to=FALSE;
   struct HotListInfoRec *p_hlinfo=(struct HotListInfoRec *)p_void;
   char **s_ptr;

   switch (btn_id) {
   case GOTO_BUTTON: goto_something=TRUE; break;
   case DELETE_BUTTON: something_deleted=TRUE; break;
   }
   for (s_ptr=p_hlinfo->ppsz_buf, i=0; *s_ptr != NULL; s_ptr=(&s_ptr[2]), i++) {
      if (something_deleted && i == (*pn_marked_index)) {
         sprintf(gszMsgBox, TgLoadString(STID_Q_DEL_NAMED_FROM_HOT_LIST),
               (**s_ptr == '\0' ? s_ptr[1] : *s_ptr));
         if (MsgBox(gszMsgBox, TOOL_NAME, YNC_MB)==MB_ID_YES) {
            *s_ptr[0] = *s_ptr[1] = '\0';
            modified = TRUE;
            continue;
         }
      }
      if (*s_ptr[0] == '\0') {
         if (strcmp((*p_entries)[i], s_ptr[1]) != 0) {
            free(*s_ptr);
            if (((*s_ptr)=UtilStrDup((*p_entries)[i])) == NULL) {
               FailAllocMessage();
            }
            modified = TRUE;
         }
      } else {
         if (strcmp((*p_entries)[i], *s_ptr) != 0) {
            free(*s_ptr);
            if (((*s_ptr)=UtilStrDup((*p_entries)[i])) == NULL) {
               FailAllocMessage();
            }
            modified = TRUE;
         }
      }
   }
   if (modified || something_deleted) {
      FILE *fp;

      if ((fp=fopen(hotListFileName, "w")) == NULL) {
         sprintf(gszMsgBox, TgLoadString(STID_CANNOT_OPEN_FILE_FOR_WRITING),
               hotListFileName);
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      } else {
         for (s_ptr=p_hlinfo->ppsz_buf; *s_ptr != NULL; s_ptr=(&s_ptr[2])) {
            if (!(**s_ptr == '\0' && *s_ptr[1] == '\0')) {
               fprintf(fp, "%s\n%s\n", *s_ptr, s_ptr[1]);
            }
         }
         fclose(fp);
      }
   }
   free(*p_dsp_ptr);
   free(**p_entries);
   free(*p_entries);
   *p_entries = NULL;

   if (goto_something) {
      int do_not_save=FALSE, canceled=FALSE;

      while (!canceled && !DirIsRemote(curDir) && fileModified &&
            !IsFiletUnSavable()) {
         switch (MsgBox(TgLoadString(STID_FILE_MOD_SAVE_BEFORE_OPEN_ANO),
               TOOL_NAME, YNC_MB)) {
         case MB_ID_YES: SaveFile(); break;
         case MB_ID_NO: do_not_save=TRUE; SetFileModified(FALSE); break;
         case MB_ID_CANCEL: canceled=TRUE; break;
         }
      }
      if (!canceled && !DirIsRemote(curDir) && fileModified &&
            IsFiletUnSavable()) {
         do_not_save = TRUE;
         SetFileModified(FALSE);
      }
      if (!canceled) {
         for (i=0, s_ptr=p_hlinfo->ppsz_buf; *s_ptr != NULL;
               s_ptr=(&s_ptr[2]), i++) {
            if (i == (*pn_marked_index)) {
               NavigateTo(s_ptr[1], do_not_save, FALSE);
               CommitNavigate();
               navigated_to = TRUE;
               break;
            }
         }
      }
   }
   for (s_ptr=p_hlinfo->ppsz_buf; *s_ptr != NULL; s_ptr++) free(*s_ptr);
   free(p_hlinfo->ppsz_buf);
   p_hlinfo->ppsz_buf = NULL;

   if (something_deleted) {
      if (modified) {
         (*pn_marked_index)--;
      }
   } else if (!goto_something) {
      (*pn_marked_index) = (*pn_num_entries)-1;
   }
   if ((*pn_marked_index) < 0) (*pn_marked_index) = INVALID;

   if (navigated_to) {
      PostNavigate(NULL);
   }
   return TRUE;
}

static
int GetHistoryEntries(p_dsp_ptr, p_entries, pn_num_entries, pn_marked_index,
      cur_buf, p_void)
   DspList **p_dsp_ptr;
   char ***p_entries, *cur_buf;
   int *pn_num_entries, *pn_marked_index;
   void *p_void;
{
   *p_dsp_ptr = HistoryListing(pn_num_entries);
   if (*p_dsp_ptr == NULL) {
      MsgBox(TgLoadString(STID_SESSION_HIST_NOT_ESTABLISHED), TOOL_NAME,
            INFO_MB);
      return FALSE;
   } else {
      ignoreDirectoryFlag = TRUE;
      *p_entries = MakeNameDspItemArray(*pn_num_entries, *p_dsp_ptr);
      ignoreDirectoryFlag = FALSE;
      if (*pn_marked_index == INVALID) {
         *pn_marked_index = (*pn_num_entries)-1;
      }
   }
   return TRUE;
}

static
int HistoryAfterLoop(p_dsp_ptr, p_entries, pn_num_entries, pn_marked_index,
      cur_buf, btn_id, selected_index, p_void)
   DspList **p_dsp_ptr;
   char ***p_entries, *cur_buf;
   int *pn_num_entries, *pn_marked_index, btn_id, selected_index;
   void *p_void;
{
   int goto_something=(btn_id == GOTO_BUTTON), navigated_to=FALSE;

   free(*p_dsp_ptr);
   free(**p_entries);
   free(*p_entries);
   *p_entries = NULL;

   if (goto_something) {
      int do_not_save=FALSE, canceled=FALSE;

      while (!canceled && !DirIsRemote(curDir) && fileModified &&
            !IsFiletUnSavable()) {
         switch (MsgBox(TgLoadString(STID_FILE_MOD_SAVE_BEFORE_OPEN_ANO),
               TOOL_NAME, YNC_MB)) {
         case MB_ID_YES: SaveFile(); break;
         case MB_ID_NO: do_not_save=TRUE; SetFileModified(FALSE); break;
         case MB_ID_CANCEL: canceled=TRUE; break;
         }
      }
      if (!canceled && !DirIsRemote(curDir) && fileModified &&
            IsFiletUnSavable()) {
         do_not_save = TRUE;
         SetFileModified(FALSE);
      }
      if (!canceled) {
         int i;
         struct NavigateRec *pnr;

         pnr = firstSessionHistory;
         for (i=0; pnr != NULL; pnr=pnr->next, i++) {
            if (i == (*pn_marked_index)) {
               NavigateTo(pnr->full_fname, do_not_save, FALSE);
               CommitNavigate();
               navigated_to = TRUE;
               break;
            }
         }
      }
   }
   if (navigated_to) {
      PostNavigate(NULL);
   }
   return TRUE;
}

static
void SelectForNavigate(TopStr, Which)
   char *TopStr;
   int Which; /* either NAVIGATE_HOTLIST or NAVIGATE_HISTORY */
{
   char win_name[128];
   struct HotListInfoRec hlinfo;

   memset(&hlinfo, 0, sizeof(struct HotListInfoRec));

   ResetNamesInfo();
   NamesSetTitle(TopStr);
   NamesSetDefaultBtnId(GOTO_BUTTON, GOTO_BUTTON);
   NamesSetEntries(NULL, NULL, 0, NULL, TRUE, INVALID, 0);
   switch (Which) {
   case NAVIGATE_HOTLIST:
      NamesAddButton(TgLoadCachedString(CSTID_GOTO), GOTO_BUTTON);
      NamesAddButton(TgLoadCachedString(CSTID_DELETE), DELETE_BUTTON);
      NamesAddButton(TgLoadCachedString(CSTID_CLOSE), BUTTON_CANCEL);
      NamesSetStyle(NAMES_EDIT_NAME, NAMES_LOOP_MANY);
      NamesSetCallback((GetEntriesFunc*)GetHotListEntries,
            (AfterLoopFunc*)HotListAfterLoop, NULL);
      sprintf(win_name, TgLoadString(STID_TOOL_HOT_LIST), TOOL_NAME);
      break;
   case NAVIGATE_HISTORY:
      NamesAddButton(TgLoadCachedString(CSTID_GOTO), GOTO_BUTTON);
      NamesAddButton(TgLoadCachedString(CSTID_CLOSE), BUTTON_CANCEL);
      NamesSetStyle(NAMES_COMPLEX_SELECT_NAME, NAMES_LOOP_MANY);
      NamesSetCallback((GetEntriesFunc*)GetHistoryEntries,
            (AfterLoopFunc*)HistoryAfterLoop, NULL);
      sprintf(win_name, TgLoadString(STID_TOOL_HOT_LIST), TOOL_NAME);
      break;
   default: return;
   }
   Names(win_name, NULL, NULL, 0, &hlinfo);
}

void NavigateHotList()
{
   MakeQuiescent();
   if (!InitHotListFileName() || hotListFileName==NULL) {
      sprintf(gszMsgBox, TgLoadString(STID_HOT_LIST_FILE_UNDEF_PLS_SPEC),
            TOOL_NAME, "HotListFileName");
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return;
   }
   SelectForNavigate(TgLoadString(STID_HOT_LIST_DOTS), NAVIGATE_HOTLIST);
   SetCurChoice(curChoiceBeforeMakeQuiescent);
}

void NavigateAddToHotList()
{
   int len=strlen(curDir)+1+strlen(curFileName), num_entries=0;
   char **s_ptr, **ppsz_buf, *full_fname;
   FILE *fp;

   if (!curFileDefined) {
      MsgBox(TgLoadString(STID_CANT_ADD_UNDEF_TO_HOT_LIST), TOOL_NAME, INFO_MB);
      return;
   }
   if ((full_fname=(char*)malloc((len+1)*sizeof(char))) == NULL) {
      FailAllocMessage();
      return;
   }
   sprintf(full_fname, "%s%c%s", curDir, DIR_SEP, curFileName);
   if ((ppsz_buf=ReadHotListFile(&num_entries)) == NULL) return;
   if ((fp=fopen(hotListFileName, "w")) == NULL) {
      sprintf(gszMsgBox, TgLoadString(STID_CANNOT_OPEN_FILE_FOR_WRITING),
            hotListFileName);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      free(full_fname);
      return;
   }
   for (s_ptr=ppsz_buf; *s_ptr != NULL; s_ptr=(&s_ptr[2])) {
      if (strcmp(s_ptr[1], full_fname) == 0) {
         char **s_ptr1;

         if (s_ptr[2] != NULL) {
            for (s_ptr1=ppsz_buf; s_ptr1 != s_ptr; s_ptr1++) {
               fprintf(fp, "%s\n", *s_ptr1);
            }
            for (s_ptr1=(&s_ptr[2]); *s_ptr1 != NULL; s_ptr1++) {
               fprintf(fp, "%s\n", *s_ptr1);
            }
         } else {
            for (s_ptr1=ppsz_buf; s_ptr1 != s_ptr; s_ptr1++) {
               fprintf(fp, "%s\n", *s_ptr1);
            }
         }
         fprintf(fp, "%s\n", (firstPage->name==NULL ? "" : firstPage->name));
         fprintf(fp, "%s\n", full_fname);
         break;
      }
   }
   if (*s_ptr == NULL) {
      for (s_ptr=ppsz_buf; *s_ptr != NULL; s_ptr++) {
         fprintf(fp, "%s\n", *s_ptr);
      }
      fprintf(fp, "%s\n", (firstPage->name==NULL ? "" : firstPage->name));
      fprintf(fp, "%s\n", full_fname);
      num_entries++;
   }
   for (s_ptr=ppsz_buf; *s_ptr != NULL; s_ptr++) free(*s_ptr);
   free(ppsz_buf);
   fclose(fp);
   free(full_fname);
   Msg(TgLoadString(STID_HOT_LIST_FILE_UPDATED));
}

void NavigateSessionHistory()
{
   MakeQuiescent();
   SelectForNavigate(TgLoadString(STID_SESSION_HISTORY_DOTS), NAVIGATE_HISTORY);
   SetCurChoice(curChoiceBeforeMakeQuiescent);
}

void ToggleHyperSpace(KeepSelected)
   int KeepSelected;
{
   inHyperSpace = !inHyperSpace;
   if (inHyperSpace) {
      if (!KeepSelected) MakeQuiescent();
      Msg(TgLoadString(STID_ENTER_HYPERSPACE_DOTS));
   } else {
      ShowCursor();
      Msg(TgLoadString(STID_LEAVING_HYPERSPACE_DOTS));
   }
   UpdatePinnedMenu(MENU_NAVIGATE);
   RedrawDummyWindow1();
}

void NextSlide()
{
   struct ObjRec *obj_ptr=FindTopLevelObjWithType("page_defaultplay");

   if (obj_ptr != NULL) {
      struct AttrRec *exec_attr=FindObjAttrWithName(obj_ptr, "exec=");
      struct AttrRec *attr_ptr=FindObjAttrWithName(obj_ptr, "finished=");

      if (exec_attr != NULL && attr_ptr != NULL) {
         char *attr_value=attr_ptr->attr_value.s;

         if (strcmp(attr_value, "0") == 0) {
            DoExecLoop(obj_ptr, exec_attr);
            /*
             * if (!userAbortExec) {
             *    ReplaceAttrFirstValue(obj_ptr, attr_ptr, "1");
             * }
             */
            return;
         }
      }
   }
   NextPage();
}

void PrevSlide()
{
   struct ObjRec *obj_ptr=FindTopLevelObjWithType("page_defaultrewind");

   if (obj_ptr != NULL) {
      struct AttrRec *exec_attr=FindObjAttrWithName(obj_ptr, "exec=");
      struct AttrRec *attr_ptr=FindObjAttrWithName(obj_ptr, "finished=");

      if (exec_attr != NULL && attr_ptr != NULL) {
         char *attr_value=attr_ptr->attr_value.s;

         if (strcmp(attr_value, "0") == 0) {
            DoExecLoop(obj_ptr, exec_attr);
            /*
             * if (!userAbortExec) {
             *    ReplaceAttrFirstValue(obj_ptr, attr_ptr, "1");
             * }
             */
            return;
         }
      }
   }
   PrevPage();
}

struct SlideShowInfoRec {
   int saved_in_hyperspace;
   int saved_fill, saved_pen;
   int saved_line_width, saved_line_style, saved_cur_dash;
};
static struct SlideShowInfoRec gstSlideShow;

static
void HideAllWindowsForSlideShow()
{
   int i=0;

   if (canvasWindowOnly || !hideWindowsInSlideShow) return;

   XUnmapWindow(mainDisplay, titleWindow);
   if (menubarWindow != None) XUnmapWindow(mainDisplay, menubarWindow);
   if (!noChoiceWindow) XUnmapWindow(mainDisplay, msgWindow);
   if (!noChoiceWindow) XUnmapWindow(mainDisplay, choiceWindow);

   XUnmapWindow(mainDisplay, hRuleWindow);
   XUnmapWindow(mainDisplay, vRuleWindow);

   XUnmapWindow(mainDisplay, colorWindow);
   XUnmapWindow(mainDisplay, colorDummyWindow);
   XUnmapWindow(mainDisplay, vSBarWindow);
   XUnmapWindow(mainDisplay, pageWindow);
   XUnmapWindow(mainDisplay, pageDummyWindow);
   XUnmapWindow(mainDisplay, hSBarWindow);
   XUnmapWindow(mainDisplay, dummyWindow1);
   XUnmapWindow(mainDisplay, dummyWindow2);
   if (!noChatWindow) XUnmapWindow(mainDisplay, chatWindow);
   if (!noStatusWindow) {
      XUnmapWindow(mainDisplay, userRedrawWindow);
      XUnmapWindow(mainDisplay, statusWindow);
      for (i=0; i < MAX_STATUS_BTNS; i++) {
         XUnmapWindow(mainDisplay, statusSubWindow[i]);
      }
   }
   if (!noModeWindow) XUnmapWindow(mainDisplay, modeWindow);
}

static
void ShowAllWindowsForSlideShow()
{
   int i=0;

   if (canvasWindowOnly || !hideWindowsInSlideShow) return;

   XMapWindow(mainDisplay, titleWindow);
   if (menubarWindow != None) XMapWindow(mainDisplay, menubarWindow);
   if (!noChoiceWindow) XMapWindow(mainDisplay, msgWindow);
   if (!noChoiceWindow) XMapWindow(mainDisplay, choiceWindow);

   XMapWindow(mainDisplay, hRuleWindow);
   XMapWindow(mainDisplay, vRuleWindow);

   XMapWindow(mainDisplay, colorWindow);
   XMapWindow(mainDisplay, colorDummyWindow);
   XMapWindow(mainDisplay, vSBarWindow);
   XMapWindow(mainDisplay, pageWindow);
   XMapWindow(mainDisplay, pageDummyWindow);
   XMapWindow(mainDisplay, hSBarWindow);
   XMapWindow(mainDisplay, dummyWindow1);
   XMapWindow(mainDisplay, dummyWindow2);
   if (!noChatWindow) XMapWindow(mainDisplay, chatWindow);
   if (!noStatusWindow) {
      XMapWindow(mainDisplay, userRedrawWindow);
      XMapWindow(mainDisplay, statusWindow);
      for (i=0; i < MAX_STATUS_BTNS; i++) {
         XMapWindow(mainDisplay, statusSubWindow[i]);
      }
   }
   if (!noModeWindow) XMapWindow(mainDisplay, modeWindow);
}

static
void StartSlideShow()
{
   struct AttrRec *exec_attr=FindFileAttrWithName("start_slide_show=");

   gstSlideShow.saved_in_hyperspace = inHyperSpace;
   gstSlideShow.saved_fill = objFill;
   gstSlideShow.saved_pen = penPat;
   gstSlideShow.saved_line_width = lineWidth;
   gstSlideShow.saved_line_style = lineStyle;
   gstSlideShow.saved_cur_dash = curDash;
   objFill = NONEPAT;
   penPat = SOLIDPAT;
   lineStyle = LS_PLAIN;
   curDash = 0;
   lineWidth = lineWidthIndexInSlideShow;

   if (!goHyperSpaceInSlideShow) {
      SetCurChoice(FREEHAND);
   }
   if (slideShowBorderColor != NULL) {
      int new_alloc=FALSE, index=0;

      index = QuickFindColorIndex(NULL, slideShowBorderColor, &new_alloc,
            FALSE);
      if (index != INVALID) {
         XSetWindowBackground(mainDisplay, mainWindow, colorPixels[index]);
      }
   } else if (myFileBgColorStr != NULL && myFileBgPixel != INVALID) {
      XSetWindowBackground(mainDisplay, mainWindow, myFileBgPixel);
   }
   if (goHyperSpaceInSlideShow && !inHyperSpace) {
      ToggleHyperSpace(FALSE);
   }
   FixPageNumbersForSlideShow();
   ClearObjCachesInAllPages();
   HideAllWindowsForSlideShow();
   HidePopupMenusForSlideShow();

   RecordWBSlideShow(TRUE);

   if (exec_attr != NULL) {
      DoExecLoop(NULL, exec_attr);
   }
}

static
void EndSlideShow()
{
   XSetWindowBackground(mainDisplay, mainWindow, myBgPixel);

   RecordWBSlideShow(FALSE);

   ShowAllWindowsForSlideShow();
   ShowPopupMenusForSlideShow();
   ClearObjCachesInAllPages();
   if (gstSlideShow.saved_in_hyperspace != inHyperSpace) {
      ToggleHyperSpace(FALSE);
   }
   objFill = gstSlideShow.saved_fill;
   penPat = gstSlideShow.saved_pen;
   lineWidth = gstSlideShow.saved_line_width;
   lineStyle = gstSlideShow.saved_line_style;
   curDash = gstSlideShow.saved_cur_dash;
   ShowFill();
   ShowPen();
   ShowLineWidth();
   ShowLineStyle();
   ShowDash();
   UpdatePinnedMenu(MENU_FILL);
   UpdatePinnedMenu(MENU_PEN);
   UpdatePinnedMenu(MENU_LINEWIDTH);
   UpdatePinnedMenu(MENU_LINESTYLE);
   UpdatePinnedMenu(MENU_LINEDASH);
   MakeQuiescent();
}

static
void ToggleSlideShow()
{
   int need_to_restore_cur_choice=FALSE;

   if (!inSlideShow) {
      if (pageLayoutMode == PAGE_TILE) {
         switch (MsgBox(TgLoadString(STID_CANT_SLIDESHOW_IN_TILED_PAGE),
               TOOL_NAME, YNC_MB)) {
         case MB_ID_YES: SetUnSavableFile(TRUE); break;
         case MB_ID_NO: return;
         case MB_ID_CANCEL: return;
         }
         PageLayoutSubMenu(PAGE_STACK);
         if (pageLayoutMode != PAGE_STACK) return;
      } else {
         if (fileModified && !IsFiletUnSavable() &&
               ((curFileDefined && !DirIsRemote(curDir)) || !curFileDefined)) {
            XBell(mainDisplay, 0);
            switch (MsgBox(TgLoadString(STID_FILE_MOD_SAVE_BEFORE_SLIDE),
                  TOOL_NAME, YNC_MB)) {
            case MB_ID_YES: SaveFile(); break;
            case MB_ID_NO: break;
            case MB_ID_CANCEL: return;
            }
         }
         if (!IsFiletUnSavable() && (makeUnsavableInSlideShow ||
               NeedToFixPageNumbersForSlideShow())) {
            if (!makeUnsavableInSlideShow) {
               sprintf(gszMsgBox,
                     TgLoadString(STID_Q_PAGE_NUM_MOD_IN_SLIDE_YNC), TOOL_NAME);
               switch (MsgBox(gszMsgBox, TOOL_NAME, YNC_MB)) {
               case MB_ID_YES: break;
               case MB_ID_NO: return;
               case MB_ID_CANCEL: return;
               }
            }
            SetUnSavableFile(TRUE);
         }
      }
      MakeQuiescent();
      need_to_restore_cur_choice = TRUE;
   }
   inSlideShow = !inSlideShow;

   if (inSlideShow) {
      Msg(TgLoadString(STID_ENTERING_SLIDESHOW_DOTS));
      StartSlideShow();
   } else {
      ShowCursor();
      Msg(TgLoadString(STID_LEAVING_SLIDESHOW_DOTS));
      EndSlideShow();
   }
   RedrawDummyWindow1();
   if (need_to_restore_cur_choice) SetCurChoice(curChoiceBeforeMakeQuiescent);
}

void EnterSlideShow()
{
   if (!inSlideShow) ToggleSlideShow();
}

void LeaveSlideShow()
{
   if (inSlideShow) ToggleSlideShow();
}

void SetSlideShowBorderColor()
{
   char spec[MAXSTRING];
   int new_alloc=0;

   *spec = '\0';
   if (Dialog(TgLoadString(STID_ENTER_A_COLOR_FOR_SLIDE_BRDR),
         TgLoadString(STID_PRESS_ENTER_FOR_DEF_COLOR), spec) == INVALID) {
      return;
   }
   UtilTrimBlanks(spec);
   if (*spec == '\0') {
      if (slideShowBorderColor != NULL) UtilFree(slideShowBorderColor);
      slideShowBorderColor = NULL;
      Msg(TgLoadString(STID_SLIDE_WILL_USE_DEF_BRDR_COLOR));
      if (slideShowXOffset == 0 && slideShowYOffset == 0) {
         slideShowInfoValid = FALSE;
      }
      return;
   }
   if (QuickFindColorIndex(NULL, spec, &new_alloc, FALSE) == INVALID) {
      sprintf(gszMsgBox, TgLoadString(STID_FAIL_ALLOC_NAMED_COLOR), spec);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return;
   }
   if (slideShowBorderColor != NULL) UtilFree(slideShowBorderColor);
   slideShowBorderColor = UtilStrDup(spec);
   if (slideShowBorderColor == NULL) FailAllocMessage();
   slideShowInfoValid = TRUE;

   sprintf(gszMsgBox, TgLoadString(STID_SLIDE_BRDR_COLOR_SET_TO_NAMED),
         slideShowBorderColor);
   Msg(gszMsgBox);
}

void SetSlideShowWindowOffsets()
{
   char spec[MAXSTRING];

   *spec = '\0';
   sprintf(gszMsgBox, TgLoadString(STID_ENTER_XY_OFFSET_FOR_SLIDE_WIN),
         slideShowXOffset, slideShowYOffset);
   if (Dialog(gszMsgBox, NULL, spec) == INVALID) return;
   UtilTrimBlanks(spec);
   if (*spec == '\0') return;

   if (ParseXYSpec(spec, &slideShowXOffset, &slideShowYOffset)) {
      sprintf(gszMsgBox, TgLoadString(STID_SLIDE_WIN_XY_OFFSETS_SET_TO),
            slideShowXOffset, slideShowYOffset);
      Msg(gszMsgBox);

      savedSlideShowXOffset = slideShowXOffset;
      savedSlideShowYOffset = slideShowYOffset;
      savedSlideShowOffsets = TRUE;
   }
   slideShowInfoValid = TRUE;
}

void ToggleVisibleGridInSlideShow()
{
   visibleGridInSlideShow = (!visibleGridInSlideShow);
   Msg(TgLoadString(visibleGridInSlideShow ? STID_GRID_BE_VISIBLE_IN_SLIDE :
         STID_GRID_NOT_BE_VISIBLE_IN_SLIDE));
}

void GoHyperSpaceInSlideShow()
{
   if (goHyperSpaceInSlideShow) return;

   goHyperSpaceInSlideShow = TRUE;
   Msg(TgLoadString(STID_GO_HYPERSPACE_IN_SLIDE));
   if (inSlideShow) {
      if (!inHyperSpace) {
         inSlideShow = FALSE;
         ToggleHyperSpace(FALSE);
         inSlideShow = TRUE;
      }
   }
}

void FreehandModeInSlideShow()
{
   if (!goHyperSpaceInSlideShow) return;

   goHyperSpaceInSlideShow = FALSE;
   Msg(TgLoadString(STID_DONT_GO_HYPERSPACE_IN_SLIDE));
   if (inSlideShow) {
      if (inHyperSpace) {
         ToggleHyperSpace(FALSE);
      }
      SetCurChoice(FREEHAND);
   }
}

int RefreshSlideShowModeMenu(menu)
   TgMenu *menu;
{
   int ok=TRUE;

   /* GoHyperSpaceInSlideShow */
   ok &= TgSetMenuItemRadioById(menu, CMDID_GOHYPERSPACEINSLIDESHOW,
         goHyperSpaceInSlideShow);
   /* FreehandModeInSlideShow */
   ok &= TgSetMenuItemRadioById(menu, CMDID_FREEHANDMODEINSLIDESHOW,
         !goHyperSpaceInSlideShow);

   /* ColorMenu */
   if (goHyperSpaceInSlideShow) {
      ok &= TgEnableMenuItemById(menu, MENU_COLOR, FALSE);
   }
   return ok;
}

TgMenu *CreateSlideShowModeMenu(parent_menu, X, Y, menu_info, status_str_xlated)
   TgMenu *parent_menu;
   int X, Y;
   TgMenuInfo *menu_info;
   int status_str_xlated; /* ignored, always 0 */
{
   TgMenu *menu=TgCreateMenuFromMenuInfo(parent_menu, X, Y, menu_info, FALSE);

   if (menu != NULL) {
      if (!RefreshSlideShowModeMenu(menu)) {
         return TgDestroyMenu(menu, TRUE);
      }
      menu->refresh_proc =
            ((RefreshMenuFunc*)RefreshSlideShowModeMenu);
   }
   return menu;
}

int SlideShowModeMenu(X, Y, TrackMenubar)
   int X, Y, TrackMenubar;
{
   int rc=INVALID;
   TgMenu *menu=(slideShowModeMenuInfo.create_proc)(NULL, X, Y,
         &slideShowModeMenuInfo, INVALID);

   activeMenu = INVALID;
   if (menu != NULL) {
      menu->track_menubar = TrackMenubar;

      rc = TgMenuLoop(menu);
      TgDestroyMenu(menu, TRUE);
   }
   return rc;
}

int RefreshSlideShowMenu(menu)
   TgMenu *menu;
{
   int ok=TRUE;

   /* SlideShow */
   ok &= TgSetMenuItemCheckById(menu, CMDID_TOGGLESLIDESHOW, inSlideShow);
   /* VisibleGridInSlideShow */
   ok &= TgSetMenuItemCheckById(menu, CMDID_VISIBLEGRIDINSLIDESHOW,
         visibleGridInSlideShow);
   /* GoHyperSpaceInSlideShow */
   ok &= TgSetMenuItemRadioById(menu, CMDID_GOHYPERSPACEINSLIDESHOW,
         goHyperSpaceInSlideShow);
   /* FreehandModeInSlideShow */
   ok &= TgSetMenuItemRadioById(menu, CMDID_FREEHANDMODEINSLIDESHOW,
         !goHyperSpaceInSlideShow);
   if (inSlideShow) {
      ok &= TgEnableMenuItemById(menu, CMDID_GOHYPERSPACEINSLIDESHOW, FALSE);
      ok &= TgEnableMenuItemById(menu, CMDID_FREEHANDMODEINSLIDESHOW, FALSE);
   }
   return ok;
}

TgMenu *CreateSlideShowMenu(parent_menu, x, y, menu_info, status_str_xlated)
   TgMenu *parent_menu;
   int x, y;
   TgMenuInfo *menu_info;
   int status_str_xlated; /* ignored, always 0 */
{
   TgMenu *menu=TgCreateMenuFromMenuInfo(parent_menu, x, y, menu_info, FALSE);

   if (menu != NULL) {
      if (!RefreshSlideShowMenu(menu)) {
         return TgDestroyMenu(menu, TRUE);
      }
   }
   return menu;
}

int ReadSlideShowInfo(buf)
   char *buf;
{
   if (!importingFile) {
      int x=0, y=0;
      char color_str[40], *psz=FindChar((int)'(', buf);

      *color_str = '\0';
      psz = ParseStr(psz, (int)',', color_str, sizeof(color_str));
      InitScan(psz, "\t\n, []");
      if (GETINT("slideshow_info", x, "x offset") == INVALID ||
          GETINT("slideshow_info", y, "y offset") == INVALID) {
         return FALSE;
      }
      CleanUpSlideShowInfo();
      UtilTrimBlanks(color_str);
      if (*color_str != '\0') {
         slideShowBorderColor = UtilStrDup(color_str);
         if (slideShowBorderColor == NULL) FailAllocMessage();
      }
      if (!ignoreSlideShowOffsetsInFile) {
         slideShowXOffset = x;
         slideShowYOffset = y;
      } else if (savedSlideShowOffsets) {
         slideShowXOffset = savedSlideShowXOffset;
         slideShowYOffset = savedSlideShowYOffset;
      }
      slideShowInfoValid = TRUE;
   }
   return TRUE;
}

void RefreshNavigateMenu(menu)
   TgMenu *menu;
{
   TgMenuItem *menu_item=FindMenuItemByCmdId(menu, CMDID_TOGGLEHYPERSPACE);
   TgMenuItem stMenuItem;

   if (menu_item == NULL) return;

   memset(&stMenuItem, 0, sizeof(TgMenuItem));
   stMenuItem.checked = inHyperSpace;
   TgSetMenuItemInfo(menu_item, TGMU_MASK_CHECK, &stMenuItem);
}

TgMenu *CreateNavigateMenu(parent_menu, x, y, menu_info, status_str_xlated)
   TgMenu *parent_menu;
   int x, y;
   TgMenuInfo *menu_info;
   int status_str_xlated; /* ignored, always 0 */
{
   TgMenu *menu=TgCreateMenuFromMenuInfo(parent_menu, x, y, menu_info, FALSE);

   if (menu != NULL) {
      TgMenuItem *menu_item=FindMenuItemByCmdId(menu, CMDID_TOGGLEHYPERSPACE);
      TgMenuItem stMenuItem;

      if (menu_item == NULL) return TgDestroyMenu(menu, TRUE);

      memset(&stMenuItem, 0, sizeof(TgMenuItem));
      stMenuItem.checked = inHyperSpace;

      if (!TgSetMenuItemInfo(menu_item, TGMU_MASK_CHECK, &stMenuItem)) {
         return TgDestroyMenu(menu, TRUE);
      }
   }
   return menu;
}

int NavigateMenu(X, Y, TrackMenubar)
   int X, Y, TrackMenubar;
{
   int rc=INVALID;
   TgMenu *menu=(navigateMenuInfo.create_proc)(NULL, X, Y, &navigateMenuInfo,
         INVALID);

   activeMenu = MENU_NAVIGATE;
   if (menu != NULL) {
      menu->track_menubar = TrackMenubar;

      rc = TgMenuLoop(menu);
      TgDestroyMenu(menu, TRUE);
   }
   return rc;
}
