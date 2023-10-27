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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/xbitmap.c,v 1.43 2011/05/16 16:22:00 william Exp $
 */

#define _INCLUDE_FROM_XBITMAP_C_

#include "tgifdefs.h"
#include "expfdefs.h"
#include "cmdids.h"

#include "attr.e"
#include "auxtext.e"
#include "choice.e"
#include "cmd.e"
#include "color.e"
#include "cursor.e"
#include "cutpaste.e"
#include "dialog.e"
#include "drawing.e"
#include "dup.e"
#include "edit.e"
#include "eps.e"
#include "file.e"
#include "font.e"
#include "grid.e"
#include "imgproc.e"
#include "mainloop.e"
#include "mark.e"
#include "menu.e"
#include "miniline.e"
#include "move.e"
#include "msg.e"
#include "names.e"
#include "obj.e"
#include "page.e"
#include "pattern.e"
#include "ps.e"
#include "raster.e"
#include "rect.e"
#include "remote.e"
#include "select.e"
#include "setup.e"
#include "stretch.e"
#include "strtbl.e"
#include "util.e"
#include "version.e"
#include "xbitmap.e"
#include "xpixmap.e"
#include "xprtfltr.e"
#include "z_intrf.e"

GC xbmGC=NULL;

int askForXBmSpec=FALSE;
int stripEPSComments=FALSE;
int saveEPSLines=FALSE;

int leftExportPixelTrim=0;
int topExportPixelTrim=0;
int rightExportPixelTrim=0;
int bottomExportPixelTrim=0;

#ifdef _XPM3TOPPM
int xpmOutputVersion=3;
#else /* ~_XPM3TOPPM */
int xpmOutputVersion=1;
#endif /* _XPM3TOPPM */

Pixmap dummyBitmap=None;

char gszHhtmlExportTemplate[MAXPATHLENGTH];

/* do not translate -- program constants */
static char hexValue[]="0123456789abcdef";

static int importXBmRV=FALSE;
static int numColorsToDump=0;
static int charsPerPixel=1;
static int *pixelValue=NULL;
static int *colorIndexToDumpIndex=NULL;
static int *dumpIndexToColorIndex=NULL;
static char *colorChar=NULL;
static char **colorStr=NULL;

static int xpmInXGrabSCFormat=FALSE;

static int halfToneBitmap=FALSE; /* has precedence over thresholdBitmap */
static int thresholdBitmap=FALSE;
static float bitmapThreshold=0.5;
static char bitmapThresholdStr[MAXSTRING+1];

static int unsignedInXBmExport=FALSE;
static int commentInBitmapExport=FALSE;
static int useImagePixelsForTrueColorExport=FALSE;

#define IMF_FORMAT_NCSA 0
#define IMF_FORMAT_CERN 1
#define IMF_FORMAT_SPYG 2

static int generateImageMap=INVALID;
static char xpmToGifCmd[MAXSTRING+1];
static char xpmToPngCmd[MAXSTRING+1];
static char xpmToJpegCmd[MAXSTRING+1];
static char ppmToGifCmd[MAXSTRING+1];
static char ppmToPngCmd[MAXSTRING+1];
static char ppmToPngWithTransCmd[MAXSTRING+1];
static char ppmToJpegCmd[MAXSTRING+1];
static char ppm6ToXpm3Cmd[MAXSTRING+1];
static char xpmDeckToGifAnimCmd[MAXSTRING+1];
static char imageMapFileExtension[MAXSTRING+1];
static char htmlFileExtension[MAXSTRING+1];
static char gifFileExtension[MAXSTRING+1];
static char pngFileExtension[MAXSTRING+1];
static char jpegFileExtension[MAXSTRING+1];
static int imageMapFileFormat=IMF_FORMAT_NCSA;

#ifdef _XPM3TOPPM
static int useXPmVersion1ForImageMap=FALSE;
#else /* ~_XPM3TOPPM */
static int useXPmVersion1ForImageMap=TRUE;
#endif /* _XPM3TOPPM */

static int useXPmVersion1ForXPmDeck=TRUE;
static int generateHtmlHref=TRUE;

static int epsiThresholdPreviewBitmap=FALSE;
static double epsiPreviewBitmapThreshold=(double)0.5;

void SetHtmlExportTemplate()
{
   char spec[MAXSTRING<<1];

   if (*gszHhtmlExportTemplate == '\0') {
      sprintf(gszMsgBox, TgLoadString(STID_ENTER_HTML_TEMPLATE));
   } else {
      sprintf(gszMsgBox, TgLoadString(STID_ENTER_HTML_TEMPLATE_CUR_IS),
            gszHhtmlExportTemplate);
   }
   *spec = '\0';
   strcpy(spec, gszHhtmlExportTemplate);
   if (Dialog(gszMsgBox, TgLoadCachedString(CSTID_DLG_ACCEPT_CANCEL), spec) ==
         INVALID) {
      return;
   }
   UtilTrimBlanks(spec);
   if (strcmp(gszHhtmlExportTemplate, spec) == 0) {
      Msg(TgLoadString(STID_HTML_TEMPLATE_UNCHANGED));
      return;
   }
   if (*spec != '\0' && !UtilPathExists(spec)) {
      sprintf(gszMsgBox, TgLoadString(STID_Q_FILE_NOT_EXIST_USE_ANYWAY), spec);
      if (MsgBox(gszMsgBox, TOOL_NAME, YNC_MB) != MB_ID_YES) {
         Msg(TgLoadString(STID_HTML_TEMPLATE_UNCHANGED));
         return;
      }
   }
   UtilStrCpyN(gszHhtmlExportTemplate, sizeof(gszHhtmlExportTemplate), spec);
   if (*gszHhtmlExportTemplate == '\0') {
      sprintf(gszMsgBox, TgLoadString(STID_NO_HTML_TEMPLATE_FILE));
   } else {
      sprintf(gszMsgBox, TgLoadString(STID_HTML_TEMPLATE_SET_TO_GIVEN),
            gszHhtmlExportTemplate);
   }
   Msg(gszMsgBox);
   SetFileModified(TRUE);
}

int ReadHtmlExportTemplate(buf)
   char *buf;
{
   if (!importingFile) {
      char *s=FindChar((int)'(', buf), *c_ptr=NULL;

      c_ptr = FindChar((int)'"', s);
      s = ReadString(c_ptr);
      *(--s) = '\0';
      *gszHhtmlExportTemplate = '\0';
      UtilStrCpyN(gszHhtmlExportTemplate, sizeof(gszHhtmlExportTemplate),
            c_ptr);
   }
   return TRUE;
}

static
void ExpandTmpStorage()
{
   /* the size before are all maxColors+2 */
   pixelValue = (int*)realloc(pixelValue, (maxColors+3)*sizeof(int));
   colorIndexToDumpIndex = (int*)realloc(colorIndexToDumpIndex,
         (maxColors+3)*sizeof(int));
   dumpIndexToColorIndex = (int*)realloc(dumpIndexToColorIndex,
         (maxColors+3)*sizeof(int));
   if (maxColors > 20) {
      charsPerPixel = 2;
      colorChar = (char*)realloc(colorChar, ((maxColors<<1)+4)*sizeof(char));
   } else {
      charsPerPixel = 1;
      colorChar = (char*)realloc(colorChar, (maxColors+3)*sizeof(char));
   }
   colorStr = (char**)realloc(colorStr, (maxColors+3)*sizeof(char*));
   if (colorStr == NULL) FailAllocMessage();
   colorStr[maxColors+2] = NULL;
   colorIndexToDumpIndex[maxColors+2] = INVALID;
   dumpIndexToColorIndex[maxColors+2] = INVALID;
}

static
void ParseExportPixelTrim(pszSpec, nInit)
   char *pszSpec;
   int nInit;
{
   char *pszLeft, *pszTop=NULL, *pszRight=NULL, *pszBottom=NULL;
   int left, top, right, bottom;

   pszLeft = strtok(pszSpec, " ,\t");
   if (pszLeft == NULL) return;

   pszTop = strtok(NULL, " ,\t");
   if (pszTop != NULL) {
      pszRight = strtok(NULL, " ,\t");
      if (pszRight != NULL) {
         pszBottom = strtok(NULL, " ,\t");
      }
   }
   left = (pszLeft==NULL ? 0 : atoi(pszLeft));
   top = (pszTop==NULL ? 0 : atoi(pszTop));
   right = (pszRight==NULL ? 0 : atoi(pszRight));
   bottom = (pszBottom==NULL ? 0 : atoi(pszBottom));
   if (left < 0 || top < 0 || right < 0 || bottom < 0) {
      sprintf(gszMsgBox, TgLoadString(STID_NEG_VAL_IN_EXP_PXL_TRM_ALT),
            leftExportPixelTrim, topExportPixelTrim, rightExportPixelTrim,
            bottomExportPixelTrim);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
   } else {
      leftExportPixelTrim = left;
      topExportPixelTrim = top;
      rightExportPixelTrim = right;
      bottomExportPixelTrim = bottom;
      sprintf(gszMsgBox, TgLoadString(STID_EXP_PXL_TRM_VAL_SET_TO),
            leftExportPixelTrim, topExportPixelTrim, rightExportPixelTrim,
            bottomExportPixelTrim);
      if (!nInit) Msg(gszMsgBox);
   }
}

void SetExportPixelTrim(cur_val_is_too_large)
   int cur_val_is_too_large;
{
   char szSpec[MAXSTRING+1];

   *szSpec = '\0';
   sprintf(gszMsgBox, TgLoadString(STID_SPECIFY_TO_TRIM_EXP_PXL_TRM),
         leftExportPixelTrim, topExportPixelTrim, rightExportPixelTrim,
         bottomExportPixelTrim);
   Dialog(gszMsgBox, TgLoadString(cur_val_is_too_large ?
         STID_ENTER_4_NUM_CUR_TOO_LARGE : STID_ENTER_4_NUM_OR_CR_ESC), szSpec);
   ParseExportPixelTrim(szSpec, FALSE);
}

int GetXpmOutputVersion()
{
   return xpmOutputVersion;
}

void SetXpmOutputVersion(xpm_output_version)
   int xpm_output_version;
{
   xpmOutputVersion = xpm_output_version;
}

void InitXBm()
{
   XGCValues values;
   char *c_ptr;

   dummyBitmap = XCreatePixmap(mainDisplay, mainWindow, 1, 1, 1);

   values.foreground = 0;
   values.background = 0;
   values.function = GXcopy;
   values.fill_style = FillSolid;
   xbmGC = XCreateGC(mainDisplay, dummyBitmap,
         GCForeground | GCBackground | GCFunction | GCFillStyle, &values);

   *gszHhtmlExportTemplate = '\0';

   importXBmRV = FALSE;
   if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"XBmReverseVideo")) != NULL &&
         UtilStrICmp(c_ptr, "true") == 0) {
      importXBmRV = TRUE;
   }
   askForXBmSpec = FALSE;
   if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"AskForXBmSpec")) != NULL &&
         UtilStrICmp(c_ptr, "true") == 0) {
      askForXBmSpec = TRUE;
   }
   stripEPSComments = FALSE;
   if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"StripEPSComments")) != NULL &&
         UtilStrICmp(c_ptr, "false") != 0) {
      fprintf(stderr, TgLoadString(STID_OBSOLETE_XDEF_USE_IGNORED),
            TOOL_NAME, "StripEPSComments");
      fprintf(stderr, "\n");
   }
#ifdef _XPM3TOPPM
   xpmOutputVersion = 3;
#else /* ~_XPM3TOPPM */
   xpmOutputVersion = 1;
#endif /* _XPM3TOPPM */
   if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"XPmOutputVersion")) != NULL) {
      xpmOutputVersion = atoi(c_ptr);
      if (xpmOutputVersion != 1 && xpmOutputVersion != 3) {
#ifdef _XPM3TOPPM
         int def_version=3;
#else /* ~_XPM3TOPPM */
         int def_version=1;
#endif /* _XPM3TOPPM */

         fprintf(stderr, TgLoadString(STID_INVALID_XDEF_USE_ALT_VALUE),
               TOOL_NAME, "XPmOutputVersion", c_ptr, def_version);
         fprintf(stderr, "\n");
         xpmOutputVersion = def_version;
      }
   }
   xpmInXGrabSCFormat = FALSE;
   if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"XPmInXGrabSCFormat"))!=NULL &&
         UtilStrICmp(c_ptr, "true") == 0) {
      xpmInXGrabSCFormat = TRUE;
   }
   halfToneBitmap = FALSE;
   if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"HalfToneBitmap"))!=NULL &&
         UtilStrICmp(c_ptr, "true") == 0) {
      halfToneBitmap = TRUE;
   }
   thresholdBitmap = FALSE;
   if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"ThresholdBitmap")) != NULL &&
         !halfToneBitmap && UtilStrICmp(c_ptr, "true") == 0) {
      thresholdBitmap = TRUE;
   }
   bitmapThreshold = (halfToneBitmap) ? 0.5 : 1.0;
   strcpy(bitmapThresholdStr, ((halfToneBitmap) ? "0.5" : "1.0"));
   if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"BitmapThreshold"))!=NULL) {
      strcpy(bitmapThresholdStr, c_ptr);
      bitmapThreshold = (float) atof(c_ptr);
      if (bitmapThreshold < 0 || bitmapThreshold > 1) {
         fprintf(stderr, TgLoadString(STID_INVALID_XDEF_USE_ALT_STR),
               TOOL_NAME, "BitmapThreshold", c_ptr,
               ((halfToneBitmap) ? "0.5" : "1.0"));
         fprintf(stderr, "\n");
         bitmapThreshold = (halfToneBitmap) ? 0.5 : 1.0;
         strcpy(bitmapThresholdStr, ((halfToneBitmap) ? "0.5" : "1.0"));
      }
   }
   if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"InitExportPixelTrim")) !=
         NULL) {
      ParseExportPixelTrim(c_ptr, TRUE);
   }
   unsignedInXBmExport = FALSE;
   if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"UnsignedInXBmExport")) !=
         NULL && UtilStrICmp(c_ptr, "true") == 0) {
      unsignedInXBmExport = TRUE;
   }
   commentInBitmapExport = FALSE;
   if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"CommentInBitmapExport")) !=
         NULL && UtilStrICmp(c_ptr, "true") == 0) {
      commentInBitmapExport = TRUE;
   }
   useImagePixelsForTrueColorExport = FALSE;
   if ((c_ptr=XGetDefault(mainDisplay, TOOL_NAME,
         "UseImagePixelsForTrueColorExport")) != NULL &&
         UtilStrICmp(c_ptr, "true") == 0) {
      useImagePixelsForTrueColorExport = TRUE;
   }
   epsiThresholdPreviewBitmap = FALSE;
   if ((c_ptr=XGetDefault(mainDisplay, TOOL_NAME,
         "EPSIThresholdPreviewBitmap")) != NULL &&
         UtilStrICmp(c_ptr, "true") == 0) {
      epsiThresholdPreviewBitmap = TRUE;
   }
   epsiPreviewBitmapThreshold = (float)0.5;
   if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,
         "EPSIPreviewBitmapThreshold")) != NULL) {
      epsiPreviewBitmapThreshold = (float) atof(c_ptr);
      if (epsiPreviewBitmapThreshold < ((float)0) ||
            epsiPreviewBitmapThreshold > (((float)1)+INT_TOL)) {
         fprintf(stderr, TgLoadString(STID_INVALID_XDEF_USE_ALT_STR),
               TOOL_NAME, "EPSIPreviewBitmapThreshold", c_ptr, "0.5");
         fprintf(stderr, "\n");
         epsiPreviewBitmapThreshold = (float)0.5;
      }
   }
   InitEPS();
}

void ExportHalfToneBitmap()
{
   halfToneBitmap = !halfToneBitmap;
   Msg(TgLoadString(halfToneBitmap ?
         STID_WILL_USE_FS_HALFTONE_BMP : STID_WILL_NOT_USE_FS_HALFTONE_BMP));
}

void ExportThresholdBitmap()
{
   thresholdBitmap = !thresholdBitmap;
   if (thresholdBitmap) {
      sprintf(gszMsgBox, TgLoadString(STID_WILL_USE_GIVE_SMPLE_THRESHOLD),
            bitmapThresholdStr);
   } else {
      sprintf(gszMsgBox, TgLoadString(STID_WILL_NOT_USE_SIMPLE_THRESHOLD));
   }
   Msg(gszMsgBox);
}

void SetExportBitmapThreshold(buf)
   char *buf;
{
   float threshold=(float)0;
   char spec[MAXSTRING<<1];

   *spec = '\0';
   if (buf != NULL && strcmp(buf, "-1") != 0) {
      int len=0;
      UtilStrCpyN(spec, sizeof(spec), buf);
      UtilTrimBlanks(spec);
      len = strlen(spec);
      if (len > 0 && spec[len-1] == ')') spec[len-1] = '\0';
   } else {
      sprintf(gszMsgBox, TgLoadString(STID_ENTER_BMP_THRESHOLD_CUR_VAL),
            bitmapThresholdStr);
      *spec = '\0';
      if (Dialog(gszMsgBox, NULL, spec) == INVALID) {
         return;
      }
   }
   UtilTrimBlanks(spec);
   if (sscanf(spec, "%f", &threshold) != 1) {
      sprintf(gszMsgBox, TgLoadString(STID_INVALID_THRESHOLD_REMAINS),
            spec, bitmapThresholdStr);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
   } else if (threshold < (float)0 || threshold > (float)1) {
      sprintf(gszMsgBox, TgLoadString(STID_INVALID_THRESHOLD_REMAINS),
            spec, bitmapThresholdStr);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
   } else {
      strcpy(bitmapThresholdStr, spec);
      bitmapThreshold = threshold;
      sprintf(gszMsgBox, TgLoadString(STID_USE_GIVEN_AS_BMP_THRESHOLD),
            bitmapThresholdStr);
      Msg(gszMsgBox);
   }
}

int RefreshBitPixmapMenu(menu)
   TgMenu *menu;
{
   int ok=TRUE;

   /* ExportHalfToneBitmap */
   ok &= TgSetMenuItemCheckById(menu, CMDID_EXPORTHALFTONEBITMAP,
         halfToneBitmap);
   /* ExportThresholdBitmap */
   ok &= TgSetMenuItemCheckById(menu, CMDID_EXPORTTHRESHOLDBITMAP,
         thresholdBitmap);

   ok &= TgEnableMenuItemById(menu, CMDID_EXPORTHALFTONEBITMAP,
         (whereToPrint == XBM_FILE && !colorDump));
   ok &= TgEnableMenuItemById(menu, CMDID_EXPORTTHRESHOLDBITMAP,
         (whereToPrint == XBM_FILE && !colorDump));
   ok &= TgEnableMenuItemById(menu, CMDID_SETEXPORTBITMAPTHRESHOLD,
         (whereToPrint == XBM_FILE && !colorDump) && thresholdBitmap);

   return ok;
}

TgMenu *CreateBitPixmapMenu(parent_menu, x, y, menu_info, status_str_xlated)
   TgMenu *parent_menu;
   int x, y;
   TgMenuInfo *menu_info;
   int status_str_xlated; /* ignored, always 0 */
{
   TgMenu *menu=TgCreateMenuFromMenuInfo(parent_menu, x, y, menu_info, FALSE);

   if (menu != NULL) {
      menu->track_menubar = TRUE;
      if (!RefreshBitPixmapMenu(menu)) {
         return TgDestroyMenu(menu, TRUE);
      }
   }
   return menu;
}

static int transparentIndex=INVALID;

static
void FreeCachedStrings()
{
   register int i;

   if (colorChar != NULL) {
      for (i = 0; i < numColorsToDump+3; i++) {
         if (colorStr[i] != NULL) {
            free(colorStr[i]);
         } else {
            break;
         }
      }
      free(colorStr);
      free(colorChar);
      free(pixelValue);
      free(colorIndexToDumpIndex);
      free(dumpIndexToColorIndex);
   }
   colorStr = NULL;
   colorChar = NULL;
   pixelValue = colorIndexToDumpIndex = dumpIndexToColorIndex = NULL;
   transparentIndex = INVALID;
}

void CleanUpXBm()
{
   FreeCachedStrings();
   CleanUpEPS();

   if (xbmGC != NULL) XFreeGC(mainDisplay, xbmGC);
   XFreePixmap(mainDisplay, dummyBitmap);

   importXBmRV = FALSE;
   askForXBmSpec = FALSE;
}

static
void MultiplyTwoByTwo(m1, m2)
   float m1[2][2], m2[2][2];
{
   float m3[2][2];

   m3[0][0] = m1[0][0]*m2[0][0] + m1[0][1]*m2[1][0];
   m3[0][1] = m1[0][0]*m2[0][1] + m1[0][1]*m2[1][1];
   m3[1][0] = m1[1][0]*m2[0][0] + m1[1][1]*m2[1][0];
   m3[1][1] = m1[1][0]*m2[0][1] + m1[1][1]*m2[1][1];

   m1[0][0]=m3[0][0]; m1[0][1]=m3[0][1]; m1[1][0]=m3[1][0]; m1[1][1]=m3[1][1];
}

void CalcTransform(mtrx)
   register struct MtrxRec *mtrx;
{
   float m1[2][2], m2[2][2], tmp_val;

   switch (mtrx->rotate) {
   case ROTATE0:
   case ROTATE180:
      mtrx->h_scale = mtrx->w / mtrx->image_w;
      mtrx->v_scale = mtrx->h / mtrx->image_h;
      break;
   case ROTATE90:
   case ROTATE270:
      mtrx->h_scale = mtrx->h / mtrx->image_w;
      mtrx->v_scale = mtrx->w / mtrx->image_h;
      break;
   }
   m1[0][0]=1.0; m1[0][1]=0.0; m1[1][0]=0.0; m1[1][1]=1.0;
   if (mtrx->flip & HORI_EVEN) {
      m2[0][0]=(-1.0); m2[0][1]=0.0; m2[1][0]=0.0; m2[1][1]=1.0;
      MultiplyTwoByTwo(m1, m2);
   }
   if (mtrx->flip & VERT_EVEN) {
      m2[0][0]=1.0; m2[0][1]=0.0; m2[1][0]=0.0; m2[1][1]=(-1.0);
      MultiplyTwoByTwo(m1, m2);
   }
   switch (mtrx->rotate) {
   case ROTATE0:
      if (mtrx->flip & (HORI_ODD | VERT_ODD)) {
         m2[0][0]=rotatedCosine[ROTATE90]; m2[0][1]=rotatedSine[ROTATE90];
         m2[1][0]=(-rotatedSine[ROTATE90]); m2[1][1]=rotatedCosine[ROTATE90];
         MultiplyTwoByTwo(m1, m2);
         if (mtrx->flip & HORI_ODD) {
            m2[0][0]=(-1.0); m2[0][1]=0.0; m2[1][0]=0.0; m2[1][1]=1.0;
            MultiplyTwoByTwo(m1, m2);
         }
         if (mtrx->flip & VERT_ODD) {
            m2[0][0]=1.0; m2[0][1]=0.0; m2[1][0]=0.0; m2[1][1]=(-1.0);
            MultiplyTwoByTwo(m1, m2);
         }
         m2[0][0]=rotatedCosine[ROTATE270];
         m2[0][1]=rotatedSine[ROTATE270];
         m2[1][0]=(-rotatedSine[ROTATE270]);
         m2[1][1]=rotatedCosine[ROTATE270];
         MultiplyTwoByTwo(m1, m2);
      }
      break;
   case ROTATE90:
      m2[0][0]=rotatedCosine[ROTATE90]; m2[0][1]=rotatedSine[ROTATE90];
      m2[1][0]=(-rotatedSine[ROTATE90]); m2[1][1]=rotatedCosine[ROTATE90];
      MultiplyTwoByTwo(m1, m2);
      if (mtrx->flip & HORI_ODD) {
         m2[0][0]=(-1.0); m2[0][1]=0.0; m2[1][0]=0.0; m2[1][1]=1.0;
         MultiplyTwoByTwo(m1, m2);
      }
      if (mtrx->flip & VERT_ODD) {
         m2[0][0]=1.0; m2[0][1]=0.0; m2[1][0]=0.0; m2[1][1]=(-1.0);
         MultiplyTwoByTwo(m1, m2);
      }
      break;
   case ROTATE180:
      m2[0][0]=rotatedCosine[ROTATE90]; m2[0][1]=rotatedSine[ROTATE90];
      m2[1][0]=(-rotatedSine[ROTATE90]); m2[1][1]=rotatedCosine[ROTATE90];
      MultiplyTwoByTwo(m1, m2);
      if (mtrx->flip & HORI_ODD) {
         m2[0][0]=(-1.0); m2[0][1]=0.0; m2[1][0]=0.0; m2[1][1]=1.0;
         MultiplyTwoByTwo(m1, m2);
      }
      if (mtrx->flip & VERT_ODD) {
         m2[0][0]=1.0; m2[0][1]=0.0; m2[1][0]=0.0; m2[1][1]=(-1.0);
         MultiplyTwoByTwo(m1, m2);
      }
      m2[0][0]=rotatedCosine[ROTATE90]; m2[0][1]=rotatedSine[ROTATE90];
      m2[1][0]=(-rotatedSine[ROTATE90]); m2[1][1]=rotatedCosine[ROTATE90];
      MultiplyTwoByTwo(m1, m2);
      break;
   case ROTATE270:
      m2[0][0]=rotatedCosine[ROTATE270]; m2[0][1]=rotatedSine[ROTATE270];
      m2[1][0]=(-rotatedSine[ROTATE270]); m2[1][1]=rotatedCosine[ROTATE270];
      MultiplyTwoByTwo(m1, m2);
      if (mtrx->flip & HORI_ODD) {
         m2[0][0]=(-1.0); m2[0][1]=0.0; m2[1][0]=0.0; m2[1][1]=1.0;
         MultiplyTwoByTwo(m1, m2);
      }
      if (mtrx->flip & VERT_ODD) {
         m2[0][0]=1.0; m2[0][1]=0.0; m2[1][0]=0.0; m2[1][1]=(-1.0);
         MultiplyTwoByTwo(m1, m2);
      }
      break;
   }
   m2[0][0]=mtrx->h_scale; m2[0][1]=0.0; m2[1][0]=0.0; m2[1][1]=mtrx->v_scale;
   MultiplyTwoByTwo(m2, m1);

   mtrx->m[0][0]=m2[0][0]; mtrx->m[0][1]=m2[0][1];
   mtrx->m[1][0]=m2[1][0]; mtrx->m[1][1]=m2[1][1];
   tmp_val = m2[0][0]*m2[1][1] - m2[0][1]*m2[1][0];
   if (fabs((double) tmp_val) < 1.0e-6) {
      tmp_val = ((float)1000000);
   } else {
      tmp_val = ((float)1.0)/tmp_val;
   }
   mtrx->rev_m[0][0] = tmp_val*m2[1][1];
   mtrx->rev_m[0][1] = (-tmp_val)*m2[0][1];
   mtrx->rev_m[1][0] = (-tmp_val)*m2[1][0];
   mtrx->rev_m[1][1] = tmp_val*m2[0][0];

   mtrx->transformed_w = mtrx->image_w*m2[0][0] + mtrx->image_h*m2[1][0];
   mtrx->transformed_h = mtrx->image_w*m2[0][1] + mtrx->image_h*m2[1][1];

   switch (mtrx->rotate) {
   case ROTATE0:
      mtrx->dump_h_scale=mtrx->m[0][0]; mtrx->dump_v_scale=mtrx->m[1][1];
      mtrx->degree=0;
      break;
   case ROTATE90:
      mtrx->dump_h_scale=mtrx->m[1][0]; mtrx->dump_v_scale=(-mtrx->m[0][1]);
      mtrx->degree=(-90);
      break;
   case ROTATE180:
      mtrx->dump_h_scale=(-mtrx->m[0][0]);
      mtrx->dump_v_scale=(-mtrx->m[1][1]);
      mtrx->degree=(-180);
      break;
   case ROTATE270:
      mtrx->dump_h_scale=(-mtrx->m[1][0]); mtrx->dump_v_scale=mtrx->m[0][1];
      mtrx->degree=(-270);
      break;
   }
}

void MakeCachedBitmap(ObjPtr)
   struct ObjRec *ObjPtr;
{
   register int r, c;
   int w, h, flip, target_percent;
   int num_cols, num_rows, image_w, image_h, watch_cursor;
   int start_col, start_row, do_msg;
   struct XBmRec *xbm_ptr=ObjPtr->detail.xbm;
   struct MtrxRec mtrx;
   Pixmap dest_bitmap;
   XImage *src_image, *dest_image;

   if (xbm_ptr->real_type==XBM_EPS && xbm_ptr->bitmap==None) return;

   w = ObjPtr->obbox.rbx - ObjPtr->obbox.ltx;
   h = ObjPtr->obbox.rby - ObjPtr->obbox.lty;
   num_cols = (zoomedIn) ? (w<<zoomScale) : (w>>zoomScale);
   num_rows = (zoomedIn) ? (h<<zoomScale) : (h>>zoomScale);

   if (ObjPtr->ctm==NULL && xbm_ptr->cached_bitmap!=None &&
         xbm_ptr->cached_zoomed==zoomedIn && xbm_ptr->cached_zoom==zoomScale &&
         xbm_ptr->cached_w==num_cols && xbm_ptr->cached_h==num_rows &&
         xbm_ptr->cached_flip==xbm_ptr->flip) {
      return;
   }
   if ((w>>zoomScale)==0 || (h>>zoomScale)==0) {
      if (xbm_ptr->cached_bitmap != None) {
         XFreePixmap(mainDisplay, xbm_ptr->cached_bitmap);
      }
      xbm_ptr->cached_bitmap = None;
      return;
   }
   watch_cursor = watchCursorOnMainWindow;
   if (!watch_cursor && !RedrawDuringScrolling()) {
      SetWatchCursor(drawWindow);
      SetWatchCursor(mainWindow);
   } else {
      CheckInterrupt(TRUE);
   }
   src_image = xbm_ptr->image;
   flip = xbm_ptr->flip;
   image_w = xbm_ptr->image_w;
   image_h = xbm_ptr->image_h;
   if (xbm_ptr->cached_bitmap != None) {
      XFreePixmap(mainDisplay, xbm_ptr->cached_bitmap);
   }
   xbm_ptr->cached_bitmap = None;

   if (src_image == NULL) {
      src_image = xbm_ptr->image = XGetImage(mainDisplay, xbm_ptr->bitmap,
            0, 0, image_w, image_h, 1, ZPixmap);
   }
   do_msg = (((num_rows*num_cols)>=0x4000) && !RedrawDuringScrolling());
   if (do_msg) {
      SaveStatusStrings();
      SetStringStatus(TgLoadCachedString(CSTID_CACHING_BITMAP));
      XSync(mainDisplay, False);
   }
   dest_bitmap = XCreatePixmap(mainDisplay,dummyBitmap,num_cols,num_rows,1);
   XFillRectangle(mainDisplay,dest_bitmap,xbmGC,0,0,num_cols,num_rows);
   dest_image = XGetImage(mainDisplay, dest_bitmap, 0, 0, num_cols,
         num_rows, 1, ZPixmap);

   if (ObjPtr->ctm == NULL) {
      mtrx.image_w = (float)image_w; mtrx.image_h = (float)image_h;
      mtrx.w = (float)num_cols; mtrx.h = (float)num_rows;
      mtrx.rotate = ROTATE0; mtrx.flip = flip;

      CalcTransform(&mtrx);

      start_col = (mtrx.transformed_w >= 0.0) ? 0 : (-num_cols)+1;
      start_row = (mtrx.transformed_h >= 0.0) ? 0 : (-num_rows)+1;

      target_percent = 5;
      for (r = 0; r < num_rows; r++) {
         double part_x, part_y;

         if (do_msg && ((r & 0xf) == 0)) {
            int percent=(r*10000/num_rows)/100;

            if (percent >= target_percent) {
               sprintf(gszMsgBox, TgLoadCachedString(CSTID_PROGRESS_PERCENT),
                     percent);
               SetStringStatus(gszMsgBox);
               XSync(mainDisplay, False);
               while (target_percent <= percent) target_percent += 5;
            }
         }
         part_x = ((double)(r+start_row)+0.5)*(mtrx.rev_m[1][0]);
         part_y = ((double)(r+start_row)+0.5)*(mtrx.rev_m[1][1]);
         for (c = 0; c < num_cols; c++) {
            double x, y;

            x = part_x+((double)(c+start_col)+0.5)*(mtrx.rev_m[0][0]);
            y = part_y+((double)(c+start_col)+0.5)*(mtrx.rev_m[0][1]);
            if (x >= ((double)0) && x < ((double)image_w) &&
                  y >= ((double)0) && y < ((double)image_h)) {
               if (XGetPixel(src_image, (int)x, (int)y) == 1) {
                  XPutPixel(dest_image, c, r, 1);
               }
            }
         }
      }
   } else {
      int abs_offset_x=ObjPtr->obbox.ltx-ObjPtr->x;
      int abs_offset_y=ObjPtr->obbox.lty-ObjPtr->y;
      double sx=(double)0.0, sy=(double)0.0;

      if (xbm_ptr->real_type != XBM_XBM) {
         sx = (((double)xbm_ptr->eps_w)/((double)image_w));
         sy = (((double)xbm_ptr->eps_h)/((double)image_h));
      }
      target_percent = 5;
      for (r=0; r < num_rows; r++) {
         int y=abs_offset_y+ABS_SIZE(r);
         double dy=((double)y)+0.5;

         if (do_msg && ((r & 0xf) == 0)) {
            int percent=(r*10000/num_rows)/100;

            if (percent >= target_percent) {
               sprintf(gszMsgBox, TgLoadCachedString(CSTID_PROGRESS_PERCENT),
                     percent);
               SetStringStatus(gszMsgBox);
               XSync(mainDisplay, False);
               while (target_percent <= percent) target_percent += 5;
            }
         }
         for (c=0; c < num_cols; c++) {
            int x=abs_offset_x+ABS_SIZE(c);
            double dx=((double)x)+0.5;
            double new_dx=(double)0, new_dy=(double)0;

            ReverseTransformDoublePointThroughCTM(dx, dy, ObjPtr->ctm,
                  &new_dx, &new_dy);
            if (xbm_ptr->real_type != XBM_XBM) {
               double tmp_dx=((double)new_dx)/sx;
               double tmp_dy=((double)new_dy)/sy;

               new_dx = tmp_dx + ((double)(ObjPtr->x - ObjPtr->orig_obbox.ltx));
               new_dy = tmp_dy + ((double)(ObjPtr->y - ObjPtr->orig_obbox.lty));
            } else {
               new_dx += (double)(ObjPtr->x-ObjPtr->orig_obbox.ltx);
               new_dy += (double)(ObjPtr->y-ObjPtr->orig_obbox.lty);
            }
            if (new_dx >= ((double)0) && new_dx < ((double)image_w) &&
                  new_dy >= ((double)0) && new_dy < ((double)image_h)) {
               int new_x=(int)new_dx, new_y=(int)new_dy;

               if (new_x < 0) new_x = 0;
               if (new_x >= image_w) new_x = image_w-1;
               if (new_y < 0) new_y = 0;
               if (new_y >= image_h) new_y = image_h-1;
               if (XGetPixel(src_image, new_x, new_y) == 1) {
                  XPutPixel(dest_image, c, r, 1);
               }
            }
         }
      }
      memcpy(&xbm_ptr->cached_ctm, ObjPtr->ctm, sizeof(struct XfrmMtrxRec));
   }
   if (do_msg) {
      SetStringStatus(TgLoadCachedString(CSTID_FINISHED_CACHEING_BITMAP));
      XSync(mainDisplay, False);
   }
   XPutImage(mainDisplay, dest_bitmap, xbmGC, dest_image, 0, 0, 0, 0,
         num_cols, num_rows);
   if (do_msg) RestoreStatusStrings();

   xbm_ptr->cached_bitmap = dest_bitmap;
   xbm_ptr->cached_zoomed = zoomedIn;
   xbm_ptr->cached_zoom = zoomScale;
   xbm_ptr->cached_flip = xbm_ptr->flip;
   xbm_ptr->cached_w = num_cols;
   xbm_ptr->cached_h = num_rows;

   XDestroyImage(dest_image);

   if (!watch_cursor && !RedrawDuringScrolling()) {
      SetDefaultCursor(mainWindow);
      ShowCursor();
   } else {
      CheckInterrupt(TRUE);
   }
}

int ExtractBitmap(OrigBitmap, OrigImage, X, Y, W, H, Bitmap, Image)
   Pixmap OrigBitmap, *Bitmap;
   XImage *OrigImage, **Image;
   int X, Y, W, H;
{
   register int j, i;
   XImage *src_image;

   SetWatchCursor(drawWindow);
   SetWatchCursor(mainWindow);

   if ((*Bitmap=XCreatePixmap(mainDisplay,mainWindow,W,H,1)) == None) {
      FailAllocBitmapMessage(W, H);
      SetDefaultCursor(mainWindow);
      SetDefaultCursor(drawWindow);
      return FALSE;
   }

   if ((*Image=XGetImage(mainDisplay, *Bitmap, 0, 0, W, H, 1,
         ZPixmap)) == NULL) {
      MsgBox(TgLoadString(STID_XGETIMAGE_MAY_RUN_OUT_VMEM), TOOL_NAME,
            INFO_MB);
      XFreePixmap(mainDisplay, *Bitmap); *Bitmap = None;
      SetDefaultCursor(mainWindow);
      SetDefaultCursor(drawWindow);
      return FALSE;
   }
   if (OrigImage != NULL && X == 0 && Y == 0) {
      src_image = OrigImage;
   } else {
      if ((src_image=XGetImage(mainDisplay, OrigBitmap, X, Y, W, H, 1,
            ZPixmap)) == NULL) {
         MsgBox(TgLoadString(STID_XGETIMAGE_MAY_RUN_OUT_VMEM), TOOL_NAME,
               INFO_MB);
         XFreePixmap(mainDisplay, *Bitmap);
         *Bitmap = None;
         XDestroyImage(*Image); *Image = NULL;
         SetDefaultCursor(mainWindow);
         SetDefaultCursor(drawWindow);
         return FALSE;
      }
   }
   for (i=0; i < H; i++) {
      for (j=0; j < W; j++) {
         XPutPixel(*Image, j, i, XGetPixel(src_image, j, i));
      }
   }
   XPutImage(mainDisplay, *Bitmap, xbmGC, *Image, 0, 0, 0, 0, W, H);
   SetDefaultCursor(mainWindow);
   SetDefaultCursor(drawWindow);

   if (!(OrigImage != NULL && X == 0 && Y == 0)) XDestroyImage(src_image);
   return TRUE;
}

int AutoTrimBitmap(OrigBitmap, pnImageW, pnImageH, pBitmap, pImage,
      pnEmptyImage, pnDontFreeOrigBitmap)
   Pixmap OrigBitmap, *pBitmap;
   int *pnImageW, *pnImageH, *pnEmptyImage, *pnDontFreeOrigBitmap;
   XImage **pImage;
{
   int i=0, orig_w=(*pnImageW), orig_h=(*pnImageH);
   int top=orig_h, left=orig_w, bottom=(-1), right=(-1);
   XImage *image=NULL;

   *pnEmptyImage = *pnDontFreeOrigBitmap = FALSE;

   SetWatchCursor(drawWindow);
   SetWatchCursor(mainWindow);
   image = XGetImage(mainDisplay, OrigBitmap, 0, 0, orig_w, orig_h, 1, ZPixmap);

   if (image == NULL) {
      MsgBox(TgLoadString(STID_XGETIMAGE_MAY_RUN_OUT_VMEM), TOOL_NAME,
            INFO_MB);
      SetDefaultCursor(mainWindow);
      SetDefaultCursor(drawWindow);
      return FALSE;
   }
   for (i=0; i < orig_h; i++) {
      int j=0;

      for (j=0; j < orig_w; j++) {
         if (XGetPixel(image, j, i) != 0) {
            if (i < top) top = i;
            if (i > bottom) bottom = i;
            if (j < left) left = j;
            if (j > right) right = j;
         }
      }
   }
   SetDefaultCursor(mainWindow);
   SetDefaultCursor(drawWindow);

   if (top == orig_h && left == orig_w && bottom == (-1) && right == (-1)) {
      /* no image */
      *pnEmptyImage = TRUE;
      XDestroyImage(image);
      return FALSE;
   } else if (top == 0 && left == 0 && bottom == orig_w-1 &&
         right == orig_h-1) {
      *pnImageW = orig_w;
      *pnImageH = orig_h;
      *pBitmap = OrigBitmap;
      *pImage = image;
      *pnDontFreeOrigBitmap = TRUE;
   } else {
      /* trim */
      int rc=0;

      *pnImageW = right-left+1;
      *pnImageH = bottom-top+1;
      rc = ExtractBitmap(OrigBitmap, image, left, top, *pnImageW, *pnImageH,
            pBitmap, pImage);
      XDestroyImage(image);

      return rc;
   }
   return TRUE;
}

static
void InvertXBmObject(ObjPtr)
   struct ObjRec *ObjPtr;
{
   int i, j, image_w, image_h, pixel, watch_cursor=watchCursorOnMainWindow;
   Pixmap bitmap=None;
   XImage *image=NULL;

   if (!watch_cursor) {
      SetWatchCursor(drawWindow);
      SetWatchCursor(mainWindow);
   }
   bitmap = ObjPtr->detail.xbm->bitmap;

   image_w = ObjPtr->detail.xbm->image_w;
   image_h = ObjPtr->detail.xbm->image_h;
   if (ObjPtr->detail.xbm->image == NULL) {
      if ((image=ObjPtr->detail.xbm->image = XGetImage(mainDisplay, bitmap,
            0, 0, image_w, image_h, 1, ZPixmap)) == NULL) {
         fprintf(stderr, "%s\n",
               TgLoadString(STID_XGETIMAGE_MAY_RUN_OUT_VMEM));
         fflush(stderr);
         SetDefaultCursor(mainWindow);
         SetDefaultCursor(drawWindow);
         if (!watch_cursor) {
            SetDefaultCursor(mainWindow);
            ShowCursor(); 
         }
         return;
      }
   } else {
      image = ObjPtr->detail.xbm->image;
   }
   for (i=0; i < image_h; i++) {
      for (j=0; j < image_w; j++) {
         pixel = XGetPixel(image, j, i);
         XPutPixel(image, j, i, ((pixel==1) ? 0 : 1));
      }
   }
   XPutImage(mainDisplay, bitmap, xbmGC, image, 0, 0, 0, 0, image_w, image_h);

   if (ObjPtr->detail.xbm->cached_bitmap != None) {
      XFreePixmap(mainDisplay, ObjPtr->detail.xbm->cached_bitmap);
   }
   ObjPtr->detail.xbm->cached_bitmap = None;
   if (zoomScale != 0) {
      ObjPtr->detail.xbm->cached_zoom = 0;
   }
   if (!watch_cursor) {
      SetDefaultCursor(mainWindow);
      ShowCursor(); 
   }
}

static
int ObjListInvertable(LastObjPtr)
   struct ObjRec *LastObjPtr;
{
   struct ObjRec *obj_ptr;

   for (obj_ptr=LastObjPtr; obj_ptr != NULL; obj_ptr=obj_ptr->prev) {
      switch (obj_ptr->type) {
      case OBJ_XBM: return TRUE;

      case OBJ_GROUP:
      case OBJ_ICON:
      case OBJ_SYM:
         if (ObjListInvertable(obj_ptr->detail.r->last)) return TRUE;
         break;
      case OBJ_PIN:
         if (ObjListInvertable(GetPinObj(obj_ptr)->detail.r->last)) return TRUE;
         break;
      }
   }
   return FALSE;
}

static
void InvertObjListXBitmap(LastObjPtr)
   struct ObjRec *LastObjPtr;
{
   struct ObjRec *obj_ptr;

   for (obj_ptr=LastObjPtr; obj_ptr != NULL; obj_ptr=obj_ptr->prev) {
      switch (obj_ptr->type) {
      case OBJ_XBM: InvertXBmObject(obj_ptr); break;

      case OBJ_GROUP:
      case OBJ_ICON:
      case OBJ_SYM:
         InvertObjListXBitmap(obj_ptr->detail.r->last);
         break;
      case OBJ_PIN:
         InvertObjListXBitmap(GetPinObj(obj_ptr)->detail.r->last);
         break;
      }
   }
}

void InvertXBitmaps()
{
   struct SelRec *sel_ptr;
   int changed=FALSE;

   if (topSel == NULL) {
      MsgBox(TgLoadCachedString(CSTID_NO_OBJ_SELECTED), TOOL_NAME, INFO_MB);
      return;
   }
   StartCompositeCmd();
   for (sel_ptr=botSel; sel_ptr != NULL; sel_ptr=sel_ptr->prev) {
      switch (sel_ptr->obj->type) {
      case OBJ_XBM:
         changed = TRUE;
         PrepareToReplaceAnObj(sel_ptr->obj);
         InvertXBmObject(sel_ptr->obj);
         RecordReplaceAnObj(sel_ptr->obj);
         break;

      case OBJ_GROUP:
      case OBJ_ICON:
      case OBJ_SYM:
      case OBJ_PIN:
         if (ObjListInvertable(sel_ptr->obj->detail.r->last)) {
            changed = TRUE;
            PrepareToReplaceAnObj(sel_ptr->obj);
            InvertObjListXBitmap(sel_ptr->obj->detail.r->last);
            RecordReplaceAnObj(sel_ptr->obj);
         }
         break;
      }
   }
   EndCompositeCmd();

   if (changed) {
      SetFileModified(TRUE);
      HighLightReverse();
      RedrawAnArea(botObj, selLtX-GRID_ABS_SIZE(1), selLtY-GRID_ABS_SIZE(1),
            selRbX+GRID_ABS_SIZE(1), selRbY+GRID_ABS_SIZE(1));
      HighLightForward();
   } else {
      MsgBox(TgLoadString(STID_NO_X11_BITMAP_OBJ_SELECTED), TOOL_NAME, INFO_MB);
   }
}

static
void ParseGeomSpec(geom_spec, image_w, image_h, src_x, src_y, src_w, src_h)
   char *geom_spec;
   int image_w, image_h, *src_x, *src_y, *src_w, *src_h;
{
   int bitmask;
   XSizeHints sizehints;

   *src_x = *src_y = 0;
   *src_w = image_w;
   *src_h = image_h;

   bitmask = XParseGeometry(geom_spec, &(sizehints.x), &(sizehints.y),
         (unsigned int *)&(sizehints.width),
         (unsigned int *)&(sizehints.height));
   if (bitmask & WidthValue) *src_w = sizehints.width;
   if (bitmask & HeightValue) *src_h = sizehints.height;
   if (bitmask & XValue) {
      *src_x = (bitmask & XNegative) ? image_w+sizehints.x : sizehints.x;
      if (bitmask & XNegative) {
         if (*src_w > image_w) *src_w = image_w;
         if (*src_x >= image_w || *src_x < 0) *src_x = image_w;
         if (*src_x-*src_w < 0) {
            *src_w = *src_x;
            *src_x = 0;
         } else {
            *src_x -= *src_w;
         }
      } else {
         if (*src_w > image_w) *src_w = image_w;
         if (*src_x >= image_w || *src_x < 0) *src_x = 0;
         if (*src_x+*src_w > image_w) *src_w = image_w-*src_x;
      }
   }
   if (bitmask & YValue) {
      *src_y = (bitmask & YNegative) ? image_h+sizehints.y : sizehints.y;
      if (bitmask & YNegative) {
         if (*src_h > image_h) *src_h = image_h;
         if (*src_y >= image_h || *src_y < 0) *src_y = image_h;
         if (*src_y-*src_h < 0) {
            *src_h = *src_y;
            *src_y = 0;
         } else {
            *src_y -= *src_h;
         }
      } else {
         if (*src_h > image_h) *src_h = image_h;
         if (*src_y >= image_h || *src_y < 0) *src_y = 0;
         if (*src_y+*src_h > image_h) *src_h = image_h-*src_y;
      }
   }
   if ((bitmask&WidthValue) && *src_x+*src_w>image_w) *src_w = image_w-*src_x;
   if ((bitmask&HeightValue) && *src_y+*src_h>image_h) *src_h = image_h-*src_y;
}

static
char *FindEqual(s)
   register char *s;
{
   while (*s != '=' && *s != '\0') s++;
   return ((*s == '=') ? (s) : (char*)NULL);
}

void ParseCutSpec(spec, image_w, image_h, mag, src_x, src_y, src_w, src_h)
   char *spec;
   int image_w, image_h, * src_x, * src_y, * src_w, * src_h;
   float *mag;
{
   char *geom_spec;

   *mag = 1.0;

   if ((geom_spec=FindEqual(spec)) == NULL) {
      ParseGeomSpec(spec,image_w,image_h,src_x,src_y,src_w,src_h);
   } else {
      *geom_spec = '\0';
      geom_spec++;
      ParseGeomSpec(geom_spec,image_w,image_h,src_x,src_y,src_w,src_h);

      sscanf(spec, "%f", mag);
      if (*mag <= 0.0) *mag = 1.0;
   }
}

void CutXBitmap()
{
   int w, h, ltx, lty, rbx, rby, new_w, new_h;
   int src_x, src_y, src_w, src_h, image_w, image_h;
   char mag_spec[MAXSTRING];
   float h_scale=1.0, v_scale=1.0, mag;
   Pixmap dest_bitmap=None;
   XImage *dest_image=NULL;
   struct ObjRec *obj_ptr=topSel->obj, *new_obj_ptr;
   struct XBmRec *new_xbm_ptr;

   if (obj_ptr->detail.xbm->real_type==XBM_EPS) {
      MsgBox(TgLoadString(STID_CANT_CUT_AN_EPS_OBJECT), TOOL_NAME, INFO_MB);
      return;
   }

   src_x = 0;
   src_y = 0;
   src_w = image_w = obj_ptr->detail.xbm->image_w;
   src_h = image_h = obj_ptr->detail.xbm->image_h;
   mag = 1.0;

   h_scale = ((float)((float)(obj_ptr->obbox.rbx-obj_ptr->obbox.ltx)) /
         ((float)image_w));
   v_scale = ((float)((float)(obj_ptr->obbox.rby-obj_ptr->obbox.lty)) /
         ((float)image_h));

   *mag_spec = '\0';
   sprintf(gszMsgBox, TgLoadString(STID_ENTER_GEOM_SPEC_ORIG_SIZE),
         image_w, image_h);
   Dialog(gszMsgBox, TgLoadCachedString(CSTID_DLG_ACCEPT_CANCEL), mag_spec);
   UtilTrimBlanks(mag_spec);
   if (*mag_spec == '\0') return;

   ParseCutSpec(mag_spec,image_w,image_h,&mag,&src_x,&src_y,&src_w,&src_h);
   if (src_x==0 && src_y==0 && src_w==image_w && src_h==image_h && mag==1.0) {
      return;
   }
   if (src_w==0 || src_h==0) {
      MsgBox(TgLoadString(STID_XBM_CANT_HAVE_0_W_OR_H), TOOL_NAME, INFO_MB);
      return;
   }

   PrepareToRecord(CMD_REPLACE, topSel, botSel, numObjSelected);
   if (!ExtractBitmap(obj_ptr->detail.xbm->bitmap,
         obj_ptr->detail.xbm->image, src_x, src_y, src_w, src_h,
         &dest_bitmap, &dest_image)) {
      AbortPrepareCmd(CMD_REPLACE);
      return;
   }

   sprintf(gszMsgBox, TgLoadCachedString(CSTID_NEW_BMP_SIZE_IS_W_X_H),
         src_w, src_h);
   Msg(gszMsgBox);

   UnlinkObj(obj_ptr);

   ltx = selLtX; lty = selLtY; rbx = selRbX; rby = selRbY;
   HighLightReverse();

   w = new_w = (int)(((float)src_w) * mag);
   h = new_h = (int)(((float)src_h) * mag);

   new_obj_ptr = (struct ObjRec *)malloc(sizeof(struct ObjRec));
   if (new_obj_ptr == NULL) FailAllocMessage();
   memset(new_obj_ptr, 0, sizeof(struct ObjRec));
   DupObjBasics(obj_ptr, new_obj_ptr);

   new_xbm_ptr = (struct XBmRec *)malloc(sizeof(struct XBmRec));
   if (new_xbm_ptr == NULL) FailAllocMessage();
   memset(new_xbm_ptr, 0, sizeof(struct XBmRec));
   new_obj_ptr->detail.xbm = new_xbm_ptr;

   new_xbm_ptr->image = dest_image;
   new_xbm_ptr->image_w = src_w;
   new_xbm_ptr->image_h = src_h;
   new_xbm_ptr->bitmap = dest_bitmap;
   new_xbm_ptr->data = NULL;
   new_xbm_ptr->fill = obj_ptr->detail.xbm->fill;
   new_xbm_ptr->flip = obj_ptr->detail.xbm->flip;
   new_xbm_ptr->cached_zoom = 0;
   new_xbm_ptr->cached_bitmap = None;
   new_xbm_ptr->cached_flip = 0;
   new_xbm_ptr->cached_w = 0;
   new_xbm_ptr->cached_h = 0;

   new_w = round(h_scale * ((float)w));
   new_h = round(v_scale * ((float)h));

   new_obj_ptr->obbox.ltx = obj_ptr->obbox.ltx;
   new_obj_ptr->obbox.lty = obj_ptr->obbox.lty;
   new_obj_ptr->obbox.rbx = new_obj_ptr->bbox.rbx = obj_ptr->obbox.ltx+new_w;
   new_obj_ptr->obbox.rby = new_obj_ptr->bbox.rby = obj_ptr->obbox.lty+new_h;

   AdjObjBBox(new_obj_ptr);

   topSel->obj = botSel->obj = new_obj_ptr;
   AddObj(NULL, topObj, new_obj_ptr);
   RecordCmd(CMD_REPLACE, NULL, topSel, botSel, numObjSelected);
   FreeObj(obj_ptr);

   UpdSelBBox();
   RedrawAreas(botObj, ltx-GRID_ABS_SIZE(1), lty-GRID_ABS_SIZE(1),
         rbx+GRID_ABS_SIZE(1), rby+GRID_ABS_SIZE(1),
         selLtX-GRID_ABS_SIZE(1), selLtY-GRID_ABS_SIZE(1),
         selRbX+GRID_ABS_SIZE(1), selRbY+GRID_ABS_SIZE(1));
   HighLightForward();
   SetFileModified(TRUE);
   justDupped = FALSE;
}

static
void UpdateColorsLookupTable(color_index)
   int color_index;
{
   if (colorIndexToDumpIndex[color_index] == INVALID) {
      if (colorStr[numColorsToDump] != NULL) {
         free(colorStr[numColorsToDump]);
      }
      pixelValue[numColorsToDump] = colorPixels[color_index];
      colorIndexToDumpIndex[color_index] = numColorsToDump;
      dumpIndexToColorIndex[numColorsToDump] = color_index;
      colorStr[numColorsToDump] = (char *) malloc(
            (strlen(colorMenuItems[color_index])+1)*sizeof(char));
      if (colorStr[numColorsToDump] == NULL) FailAllocMessage();
      strcpy(colorStr[numColorsToDump], colorMenuItems[color_index]);
      numColorsToDump++;
   }
}

static
void BuildStrSegColors(pStrSeg, pUserData)
   StrSegInfo *pStrSeg;
   void *pUserData;
{
   UpdateColorsLookupTable(pStrSeg->color);
}

static
void BuildObjXPmColors(ObjPtr)
   struct ObjRec *ObjPtr;
{
   register int i, color_index;
   register struct ObjRec *obj_ptr;
   register struct AttrRec *attr_ptr;
   struct XPmRec *xpm_ptr;
   int start_index, new_alloc;

   switch (ObjPtr->type) {
   case OBJ_POLY:
   case OBJ_BOX:
   case OBJ_OVAL:
   case OBJ_POLYGON:
   case OBJ_ARC:
   case OBJ_RCBOX:
   case OBJ_XBM:
      UpdateColorsLookupTable(ObjPtr->color);
      if (ObjPtr->type == OBJ_XBM) {
         if (ObjPtr->detail.xbm->real_type==XBM_EPS &&
               ObjPtr->detail.xbm->bitmap==None) {
            char color_str[COLORSTRLEN];

            new_alloc = FALSE;
            GetDrawingFgColorStr(INVALID, INVALID, color_str,
                  sizeof(color_str));
            color_index = QuickFindColorIndex(NULL, color_str, &new_alloc,
                  FALSE);
            if (color_index != INVALID) {
               if (new_alloc) ExpandTmpStorage();
               UpdateColorsLookupTable(color_index);
            }
         }
      }
      break;

   case OBJ_TEXT:
      UpdateColorsLookupTable(ObjPtr->color);
      DoFuncOnStrSegForMiniLines(&ObjPtr->detail.t->minilines,
            BuildStrSegColors, NULL);
      break;

   case OBJ_XPM:
      xpm_ptr = ObjPtr->detail.xpm;
      start_index = (xpm_ptr->first_pixel_is_bg) ? 1 : 0;
      for (i=start_index; i < xpm_ptr->ncolors; i++) {
         /* do not translate -- program constants */
         if (UtilStrICmp(xpm_ptr->color_str[i], "None") == 0) {
            if (whereToPrint == XBM_FILE && colorDump &&
                  transparentIndex == (-1)) {
               /*
                * Transparent stuff pretty much only works for straight
                *         XPM output and nothing else.  None of the
                *         convertors (e.g., xpmtoppm) can handle the
                *         None color in an XPM file.
                */
               if (colorStr[numColorsToDump] != NULL) {
                  free(colorStr[numColorsToDump]);
               }
               pixelValue[numColorsToDump] = (-1);
               colorStr[numColorsToDump] = UtilStrDup("None");
               transparentIndex = numColorsToDump++;
            }
         } else if ((color_index=QuickFindColorIndex(NULL,
               xpm_ptr->color_str[i], &new_alloc, TRUE)) != INVALID) {
            if (new_alloc) ExpandTmpStorage();
            UpdateColorsLookupTable(color_index);
         }
      }
      if (!PRTGIF && colorLayers && needToRedrawColorWindow) {
         RedrawColorWindow();
      }
      break;

   case OBJ_GROUP:
   case OBJ_ICON:
   case OBJ_SYM:
      for (obj_ptr=ObjPtr->detail.r->first; obj_ptr != NULL;
            obj_ptr=obj_ptr->next) {
         BuildObjXPmColors(obj_ptr);
      }
      break;
   case OBJ_PIN:
      obj_ptr = GetPinObj(ObjPtr);
      BuildObjXPmColors(obj_ptr);
      break;
   }
   for (attr_ptr=ObjPtr->fattr; attr_ptr!=NULL; attr_ptr=attr_ptr->next) {
      BuildObjXPmColors(attr_ptr->obj);
   }
}

typedef struct tagTmpBucketInfo {
   int pixel;
   struct tagTmpBucketInfo *next;
} TmpBucketInfo;

static int numTmpBuckets=0;
static TmpBucketInfo **ppTmpBuckets=NULL;
static int tmpRedShift=0, tmpGreenShift=0, tmpBlueShift=0;

static
void CleanUpTmpBuckets()
{
   int i=0;

   if (ppTmpBuckets != NULL) {
      for (i=0; i < numTmpBuckets; i++) {
         TmpBucketInfo *ptbi=NULL, *ptbi_next=NULL;

         for (ptbi=ppTmpBuckets[i]; ptbi != NULL; ptbi=ptbi_next) {
            ptbi_next = ptbi->next;
            free(ptbi);
         }
      }
      free(ppTmpBuckets);
   }
   tmpRedShift = tmpGreenShift = tmpBlueShift = 0;
   numTmpBuckets = 0;
   ppTmpBuckets = NULL;
}

static
int SetTmpShift(mask, pn_shifts)
   unsigned long mask;
   int *pn_shifts;
{
   int i=0;

   if (mask == 0L) return FALSE;

   while ((mask & 0x1) == 0) {
      i++;
      mask >>= 1;
   }
   *pn_shifts = i;

   return TRUE;
}

static
int BadMask(cWhich, mask)
   char cWhich;
   unsigned long mask;
{
   switch (cWhich) {
   case 'r':
      sprintf(gszMsgBox, TgLoadString(STID_BAD_RED_MASK_TRUE_COLOR_DPY),
            (long)mask, TOOL_NAME, "UseImagePixelsForTrueColorExport");
      break;
   case 'g':
      sprintf(gszMsgBox, TgLoadString(STID_BAD_GREEN_MASK_TRUE_COLOR_DPY),
            (long)mask, TOOL_NAME, "UseImagePixelsForTrueColorExport");
      break;
   case 'b':
      sprintf(gszMsgBox, TgLoadString(STID_BAD_BLUE_MASK_TRUE_COLOR_DPY),
            (long)mask, TOOL_NAME, "UseImagePixelsForTrueColorExport");
      break;
   }
   if (PRTGIF) {
      fprintf(stderr, "%s\n", gszMsgBox);
   } else {
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
   }
   CleanUpTmpBuckets();

   return FALSE;
}

static
int InitTmpBuckets()
{
   numTmpBuckets = 257;
   ppTmpBuckets = (TmpBucketInfo**)malloc(numTmpBuckets*sizeof(TmpBucketInfo*));
   if (ppTmpBuckets == NULL) FailAllocMessage();
   memset(ppTmpBuckets, 0, numTmpBuckets*sizeof(TmpBucketInfo*));

   if (mainVisual->class != TrueColor) {
      return TRUE;
   }
   if (!SetTmpShift(mainVisual->red_mask, &tmpRedShift)) {
      return BadMask('r', mainVisual->red_mask);
   }
   if (!SetTmpShift(mainVisual->green_mask, &tmpGreenShift)) {
      return BadMask('g', mainVisual->green_mask);
   }
   if (!SetTmpShift(mainVisual->blue_mask, &tmpBlueShift)) {
      return BadMask('b', mainVisual->blue_mask);
   }
   return TRUE;
}

static
int TmpPixelHash(pixel)
   int pixel;
{
   return (((pixel)==(-1)) ? (numTmpBuckets-1) : (pixel % numTmpBuckets));
}

static
void GetTmpValue(pixel, mask, shift, pn_value)
   int pixel, shift, *pn_value;
   unsigned long mask;
{
   double d_val=(double)0;
   int value=0;

   pixel = (int)(pixel & mask);
   pixel >>= shift;
   mask >>= shift;
   d_val = ((double)pixel) / ((double)mask) * ((double)0x0ff);
   value = round(d_val);
   if (value > 0x0ff) value = 0x0ff;
   if (value < 0) value = 0;
   *pn_value = value;
}

static
void GetTmpColorStr(pixel, color_str)
   int pixel;
   char *color_str;
{
   int red_value=0, green_value=0, blue_value=0;

   if (mainVisual->class == TrueColor) {
      GetTmpValue(pixel, mainVisual->red_mask, tmpRedShift, &red_value);
      GetTmpValue(pixel, mainVisual->green_mask, tmpGreenShift, &green_value);
      GetTmpValue(pixel, mainVisual->blue_mask, tmpBlueShift, &blue_value);
      sprintf(color_str, "#%c%c%c%c%c%c",
            hexValue[(red_value>>4)&0x0f], hexValue[red_value&0x0f],
            hexValue[(green_value>>4)&0x0f], hexValue[green_value&0x0f],
            hexValue[(blue_value>>4)&0x0f], hexValue[blue_value&0x0f]);
   } else {
      XColor xcolor;

      memset(&xcolor, 0, sizeof(XColor));
      xcolor.pixel = pixel;
      XQueryColor(mainDisplay, mainColormap, &xcolor);
      red_value = xcolor.red;
      green_value = xcolor.green;
      blue_value = xcolor.blue;
      sprintf(color_str, "#%c%c%c%c%c%c",
            hexValue[(red_value>>12)&0x0f], hexValue[(red_value>>8)&0x0f],
            hexValue[(green_value>>12)&0x0f], hexValue[(green_value>>8)&0x0f],
            hexValue[(blue_value>>12)&0x0f], hexValue[(blue_value>>8)&0x0f]);
   }
}

static
void UpdateColorsLookupTableForPixel(pixel, can_alloc)
   int pixel, can_alloc;
{
   char color_str[COLORSTRLEN];
   int bucket=TmpPixelHash(pixel);
   TmpBucketInfo *ptbi=NULL;
   
   for (ptbi=ppTmpBuckets[bucket]; ptbi != NULL; ptbi=ptbi->next) {
      if (ptbi->pixel == pixel) {
         return;
      }
   }
   ptbi = (TmpBucketInfo*)malloc(sizeof(TmpBucketInfo));
   if (ptbi == NULL) FailAllocMessage();
   memset(ptbi, 0, sizeof(TmpBucketInfo));
   ptbi->next = ppTmpBuckets[bucket];
   ppTmpBuckets[bucket] = ptbi;
   ptbi->pixel = pixel;

   if (can_alloc && numColorsToDump >= maxColors) {
      maxColors++;
      ExpandTmpStorage();
   }
   UtilFree(colorStr[numColorsToDump]);
   pixelValue[numColorsToDump] = pixel;
   *color_str = '\0';
   GetTmpColorStr(pixel, color_str);
   colorStr[numColorsToDump] = UtilStrDup(color_str);
   numColorsToDump++;
}

static
void BuildXPmColors(image, w, h, left, top, right, bottom, use_pixels)
   XImage *image;
   int w, h, left, top, right, bottom, use_pixels;
{
   int i;

   FreeCachedStrings();
   if (colorChar == NULL) {
      pixelValue = (int*)malloc((maxColors+3)*sizeof(int));
      if (pixelValue == NULL) FailAllocMessage();
      colorIndexToDumpIndex = (int*)malloc((maxColors+3)*sizeof(int));
      dumpIndexToColorIndex = (int*)malloc((maxColors+3)*sizeof(int));
      if (colorIndexToDumpIndex == NULL || dumpIndexToColorIndex == NULL) {
         FailAllocMessage();
      }
      if (maxColors > 20) {
         charsPerPixel = 2;
         colorChar = (char*)malloc(((maxColors<<1)+6)*sizeof(char));
      } else {
         charsPerPixel = 1;
         colorChar = (char*)malloc((maxColors+3)*sizeof(char));
      }
      if (colorChar == NULL) FailAllocMessage();
      colorStr = (char**)malloc((maxColors+3)*sizeof(char*));
      if (colorStr == NULL) FailAllocMessage();
      memset(colorStr, 0, (maxColors+3)*sizeof(char*));
   }
   for (i = 0; i < maxColors+3; i++) {
      colorIndexToDumpIndex[i] = dumpIndexToColorIndex[i] = INVALID;
   }
   pixelValue[0] = GetDrawingBgPixel(INVALID, INVALID);
   if (myFileBgColorStr == NULL) {
      colorStr[0] = (char*)malloc((strlen(myBgColorStr)+1)*sizeof(char));
      if (colorStr[0] == NULL) FailAllocMessage();
      strcpy(colorStr[0], myBgColorStr);
   } else {
      colorStr[0] = (char*)malloc((strlen(myFileBgColorStr)+1)*sizeof(char));
      if (colorStr[0] == NULL) FailAllocMessage();
      strcpy(colorStr[0], myFileBgColorStr);
   }
   numColorsToDump = 1;

   if ((use_pixels || (mainVisual->class == TrueColor &&
         useImagePixelsForTrueColorExport)) && !InitTmpBuckets()) {
      if (!use_pixels) {
         useImagePixelsForTrueColorExport = FALSE;
      } else {
         use_pixels = FALSE;
      }
   }
   if ((use_pixels || (mainVisual->class == TrueColor &&
         useImagePixelsForTrueColorExport))) {
      SetStringStatus(TgLoadCachedString(CSTID_BLD_COLOR_TBL_FROM_PIXEL_DOTS));
      for (i=top; i < h-bottom; i++) {
         int j=0;

         for (j=left; j < w-right; j++) {
            int pixel=XGetPixel(image,j-left,i-top);

            UpdateColorsLookupTableForPixel(pixel, FALSE);
         }
      }
      CleanUpTmpBuckets();
   } else {
      struct ObjRec *obj_ptr=NULL;

      for (obj_ptr=botObj; obj_ptr != NULL; obj_ptr=obj_ptr->prev) {
         BuildObjXPmColors(obj_ptr);
      }
   }
   colorChar[0] = '`';
   if (charsPerPixel > 1) colorChar[1] = '`';
   if (numColorsToDump >= 256) {
      for (i=1; i < numColorsToDump; i++) {
         if (charsPerPixel == 1) {
            colorChar[i] = (char)(((int)('a'))+i-1);
         } else {
            int left=(int)(i/80), right=(i%80);

            if (left >= 31) {
               colorChar[i*2] = (char)(((int)('/'))+left-31);
            } else {
               colorChar[i*2] = (char)(((int)('`'))+left);
            }
            if (right >= 31) {
               colorChar[i*2+1] = (char)(((int)('/'))+right-31);
            } else {
               colorChar[i*2+1] = (char)(((int)('`'))+right);
            }
         }
      }
   } else {
      for (i=1; i < numColorsToDump; i++) {
         if (charsPerPixel == 1) {
            colorChar[i] = (char)(((int)('a'))+i-1);
         } else {
            colorChar[i*2] = (char)(((int)('a'))+(int)(i/10));
            colorChar[i*2+1] = (char)(((int)('0'))+(i%10));
         }
      }
   }
}

static
void ColorStrToXPmStr(index, color_str)
   int index;
   char *color_str;
{
   char s[3];
   int i, value;

   if (*(colorStr[index]) == '#') {
      strcpy(color_str, colorStr[index]);
   } else {
      for (i = 0; i < maxColors; i++) {
         if (colorPixels[i] ==  pixelValue[index]) {
            break;
         }
      }
      if (i == maxColors) {
         strcpy(color_str, colorStr[index]);
      } else {
         strcpy(color_str, "#");
         value = (int)(((float)tgifColors[i].red/maxRGB) * 0x100);
         if (value > 255) {
            value = 255;
         } else if (value < 255) {
            value = 0;
         }
         sprintf(s, "%c%c", hexValue[(value>>4)&0x0f], hexValue[value&0x0f]);
         strcat(color_str, s);
         strcat(color_str, s);

         value = (int)(((float)tgifColors[i].green/maxRGB) * 0x100);
         if (value > 255) {
            value = 255;
         } else if (value < 255) {
            value = 0;
         }
         sprintf(s, "%c%c", hexValue[(value>>4)&0x0f], hexValue[value&0x0f]);
         strcat(color_str, s);
         strcat(color_str, s);

         value = (int)(((float)tgifColors[i].blue/maxRGB) * 0x100);
         if (value > 255) {
            value = 255;
         } else if (value < 255) {
            value = 0;
         }
         sprintf(s, "%c%c", hexValue[(value>>4)&0x0f], hexValue[value&0x0f]);
         strcat(color_str, s);
         strcat(color_str, s);
      }
   }
}

static
void DumpXPmColors(FP)
   FILE *FP;
{
   int i, j;
   char s[MAXSTRING];

   if (xpmOutputVersion == 1) {
      if (xpmInXGrabSCFormat) {
         for (i=0; i < numColorsToDump-1; i++) {
            if (fprintf(FP, "\"") == EOF) writeFileFailed = TRUE;
            for (j = 0; j < charsPerPixel; j++) {
               if (fprintf(FP, "%c", colorChar[i*charsPerPixel+j]) == EOF) {
                  writeFileFailed = TRUE;
               }
            }
            ColorStrToXPmStr(i, s);
            if (fprintf(FP, "\", \"%s\"\n", s) == EOF) writeFileFailed = TRUE;
         }
         if (fprintf(FP, "\"") == EOF) writeFileFailed = TRUE;
         for (j = 0; j < charsPerPixel; j++) {
            if (fprintf(FP, "%c", colorChar[i*charsPerPixel+j]) == EOF) {
               writeFileFailed = TRUE;
            }
         }
         ColorStrToXPmStr(i, s);
         if (fprintf(FP, "\", \"%s\"\n} ;\n", s) == EOF) {
            writeFileFailed = TRUE;
         }
      } else {
         for (i=0; i < numColorsToDump-1; i++) {
            if (fprintf(FP, "   \"") == EOF) writeFileFailed = TRUE;
            for (j = 0; j < charsPerPixel; j++) {
               if (fprintf(FP, "%c", colorChar[i*charsPerPixel+j]) == EOF) {
                  writeFileFailed = TRUE;
               }
            }
            if (fprintf(FP, "\", \"%s\",\n", colorStr[i]) == EOF) {
               writeFileFailed = TRUE;
            }
         }
         if (fprintf(FP, "   \"") == EOF) writeFileFailed = TRUE;
         for (j = 0; j < charsPerPixel; j++) {
            if (fprintf(FP, "%c", colorChar[i*charsPerPixel+j]) == EOF) {
               writeFileFailed = TRUE;
            }
         }
         if (fprintf(FP, "\", \"%s\"\n};\n", colorStr[i]) == EOF) {
            writeFileFailed = TRUE;
         }
      }
   } else {
      /* xpmOutputVersion is 3 */
      for (i=0; i < numColorsToDump; i++) {
         if (fprintf(FP, "\"") == EOF) writeFileFailed = TRUE;
         for (j = 0; j < charsPerPixel; j++) {
            if (fprintf(FP, "%c", colorChar[i*charsPerPixel+j]) == EOF) {
               writeFileFailed = TRUE;
            }
         }
         if (fprintf(FP, " c %s\",\n", colorStr[i]) == EOF) {
            writeFileFailed = TRUE;
         }
      }
   }
}

static
int SetupTrueColorInfoFromImage(image, ptci)
   XImage *image;
   TrueColorInfo *ptci;
{
   unsigned int r_mask=0, g_mask=0, b_mask=0;
   unsigned int r_maxval=0, g_maxval=0, b_maxval=0;
   unsigned int r_bits=0, g_bits=0, b_bits=0;
   int r_shift=(-1), g_shift=(-1), b_shift=(-1);
   unsigned int shifts=0;

   ptci->r_mask = r_mask = image->red_mask;
   ptci->g_mask = g_mask = image->green_mask;
   ptci->b_mask = b_mask = image->blue_mask;
   if (r_mask == 0 && g_mask == 0 && b_mask == 0) {
      if (PRTGIF && !cmdLineOpenDisplay) {
         FatalUnexpectedError(
               /* debug, do not translate */
               "Unimplemented in SetupTrueColorInfoFromImage().", NULL);
         return FALSE;
      }
      return SetupTrueColorInfo(ptci);
   }
   for (shifts=0; r_mask|g_mask|b_mask; shifts++) {
      if (r_mask & 0x1) {
         if (r_shift == (-1)) {
            r_shift = shifts;
            r_maxval = r_mask;
         }
         r_bits++;
      }
      if (g_mask & 0x1) {
         if (g_shift == (-1)) {
            g_shift = shifts;
            g_maxval = g_mask;
         }
         g_bits++;
      }
      if (b_mask & 0x1) {
         if (b_shift == (-1)) {
            b_shift = shifts;
            b_maxval = b_mask;
         }
         b_bits++;
      }
      r_mask >>= 1;
      g_mask >>= 1;
      b_mask >>= 1;
   }
   ptci->r_shift = (unsigned int)r_shift;
   ptci->g_shift = (unsigned int)g_shift;
   ptci->b_shift = (unsigned int)b_shift;
   ptci->dr_maxval = (double)r_maxval;
   ptci->dg_maxval = (double)g_maxval;
   ptci->db_maxval = (double)b_maxval;
   ptci->dr_maxval_div255 = ptci->dr_maxval / ((double)255);
   ptci->dg_maxval_div255 = ptci->dg_maxval / ((double)255);
   ptci->db_maxval_div255 = ptci->db_maxval / ((double)255);
   ptci->num_r_bits = r_bits;
   ptci->num_g_bits = g_bits;
   ptci->num_b_bits = b_bits;

   if (r_shift==(-1) || g_shift==(-1) || b_shift==(-1)) {
      return FALSE;
   }
   return TRUE;
}

int InitTrueColorInfo(image, ptci, image_w)
   XImage *image;
   TrueColorInfo *ptci;
   int image_w;
{
   unsigned int bytes_per_pixel=image->bits_per_pixel/8;

   memset(ptci, 0, sizeof(TrueColorInfo));
   if (bytes_per_pixel > 4) {
#ifdef _TGIF_DBG /* debug, do not translate */
      fprintf(stderr, "bytes_per_pixel (%1d) > 4 in InitTrueColorInfo().\n",
            bytes_per_pixel);
#endif /* _TGIF_DBG */
      return FALSE;
   }
   if ((unsigned int)(image->bytes_per_line/image_w) != bytes_per_pixel) {
#ifdef _TGIF_DBG /* debug, do not translate */
      fprintf(stderr, "bytes_per_pixel (%1d) != %1d %s.\n",
            bytes_per_pixel, (unsigned int)(image->bytes_per_line/image_w),
            "in InitTrueColorInfo()");
#endif /* _TGIF_DBG */
      return FALSE;
   }
   return SetupTrueColorInfoFromImage(image, ptci);
}

/*
 * static
 * void MapPixToEPSI(SrcX, SrcY, SrcW, SrcH, DestW, DestH, DestX, DestY)
 *    int SrcX, SrcY, SrcW, SrcH, DestW, DestH, * DestX, * DestY;
 * {
 *    if (pageStyle == LANDSCAPE)
 *    {
 *       *DestY = (int)round(((float)DestH) -
 *             ((((float)DestH)*((float)SrcX))/((float)SrcW)));
 *       *DestX = (int)round((((float)DestW)*((float)SrcY))/((float)SrcH));
 *    }
 *    else
 *    {
 *       *DestX = (int)round((((float)DestW)*((float)SrcX))/((float)SrcW));
 *       *DestY = (int)round((((float)DestH)*((float)SrcY))/((float)SrcH));
 *    }
 *    if (*DestX >= DestW) *DestX = DestW-1; if (*DestX < 0) *DestX = 0;
 *    if (*DestY >= DestH) *DestY = DestH-1; if (*DestY < 0) *DestY = 0;
 * }
 */

static
void MapEPSIToPix(DestX, DestY, DestW, DestH, SrcW, SrcH, SrcX, SrcY)
   int DestX, DestY, DestW, DestH, SrcW, SrcH, * SrcX, * SrcY;
{
   if (pageStyle == LANDSCAPE) {
      *SrcX = (int)round(((float)(DestH-DestY))*((float)SrcW)/((float)DestH));
      *SrcY = (int)round((((float)SrcH)*((float)DestX))/((float)DestW));
   } else {
      *SrcX = (int)round((((float)SrcW)*((float)DestX))/((float)DestW));
      *SrcY = (int)round((((float)SrcH)*((float)DestY))/((float)DestH));
   }
   if (*SrcX >= SrcW) *SrcX = SrcW-1; if (*SrcX < 0) *SrcX = 0;
   if (*SrcY >= SrcH) *SrcY = SrcH-1; if (*SrcY < 0) *SrcY = 0;
}

void GenPreviewBitmap(FP, llxPage, llyPage, urxPage, uryPage)
   FILE *FP;
   int llxPage, llyPage, urxPage, uryPage;
{
   int col=0, row=0, ltx=0, lty=0, w=0, h=0, x=0, y=0;
   int **data=NULL, num_image_bytes_per_row=0;
   int image_w=urxPage-llxPage, image_h=uryPage-llyPage, num_lines=0;
   Pixmap pixmap=None;
   XImage *image=NULL;
   TrueColorInfo tci;

   if (image_w == 0 || image_h == 0) return;
   if ((pixmap=DrawAllOnPixmap(&ltx, &lty, &w, &h, TRUE)) == None) return;

   image = XGetImage(mainDisplay, pixmap, 0, 0, w, h, AllPlanes, ZPixmap);
   if (image == NULL) {
      MsgBox(TgLoadString(STID_XGETIMAGE_MAY_RUN_OUT_VMEM), TOOL_NAME, INFO_MB);
      XFreePixmap(mainDisplay, pixmap);
      return;
   }
   if (fullTrueColorMode) {
      if (!InitTrueColorInfo(image, &tci, w)) {
         XDestroyImage(image);
         XFreePixmap(mainDisplay, pixmap);
         return;
      }
   }
   if (image_w < 0) image_w = (-image_w);
   if (image_h < 0) image_h = (-image_h);
   num_image_bytes_per_row = ((image_w & 0x7) ? (image_w>>3)+1 : (image_w>>3));
   num_lines = ((image_w & 0x7) ? (((image_w>>3)+1)<<1) : ((image_w>>3)<<1));
   num_lines = ((num_lines & 0x3f) ? (num_lines>>6)+1 : (num_lines>>6));

   if ((data=(int**)malloc(image_h*sizeof(int*))) == NULL) {
      FailAllocMessage();
      return;
   }
   for (row=0; row < image_h; row++) {
      if ((data[row]=(int*)malloc(num_image_bytes_per_row*sizeof(int))) ==
            NULL) {
         FailAllocMessage();
         return;
      } else {
         for (col=0; col<num_image_bytes_per_row; col++) data[row][col] = 0;
      }
   }
   fprintf(FP, "%%%%BeginPreview: %1d %1d 1 %1d\n", image_w, image_h,
         num_lines*image_h);
   for (row=0; row < image_h; row++) {
      int bg_pixel=GetDrawingBgPixel(INVALID, INVALID);

      for (col=0; col < image_w; col++) {
         int pixel=XGetPixel(image, x, y);

         MapEPSIToPix(col, row, image_w, image_h, w, h, &x, &y);
         if (epsiThresholdPreviewBitmap) {
            double gray=(double)0;

            if (fullTrueColorMode) {
               uint32_t pix=(uint32_t)(unsigned int)pixel;
               unsigned int r=0, g=0, b=0;
               double dr=(double)0, dg=(double)0, db=(double)0;

               r = (pix & tci.r_mask) >> tci.r_shift;
               g = (pix & tci.g_mask) >> tci.g_shift;
               b = (pix & tci.b_mask) >> tci.b_shift;
               dr = ((double)r) / tci.dr_maxval;
               dg = ((double)g) / tci.dg_maxval;
               db = ((double)b) / tci.db_maxval;
               gray = (double)(0.299*dr + 0.587*dg + 0.114*db);
            } else {
               int k=0, found_index=INVALID;

               for (k=0; k < maxColors; k++) {
                  if (colorPixels[k] == pixel) {
                     found_index = k;
                     break;
                  }
               }
               if (found_index == INVALID) {
                  if (pixel == bg_pixel) {
                     gray = (double)1.0;
                  } else {
                     sprintf(gszMsgBox,
                           TgLoadString(STID_UNRECOG_GIVEN_PIXEL_VAL_1_USE),
                           pixel, (long)pixel);
                     Msg(gszMsgBox);
                     gray = (double)0.0;
                  }
               } else {
                  gray = 0.299*((double)tgifColors[found_index].red/maxRGB) +
                        0.587*((double)tgifColors[found_index].green/maxRGB) +
                        0.114*((double)tgifColors[found_index].blue/maxRGB);
               }
            }
            if (gray < epsiPreviewBitmapThreshold) {
               data[row][col>>3] |= (1<<(7 - (col & 0x7)));
            }
         } else {
            if (pixel != bg_pixel) {
               data[row][col>>3] |= (1<<(7 - (col & 0x7)));
            }
         }
      }
   }
   for (row=0; row < image_h; row++) {
      int byte_count=0;

      fprintf(FP, "%% ");
      for (col=0; col < num_image_bytes_per_row; col++) {
         if (byte_count++ == 32) {
            byte_count = 1;
            fprintf(FP, "\n%% ");
         }
         fprintf(FP, "%c", hexValue[(data[row][col]>>4) & 0xf]);
         fprintf(FP, "%c", hexValue[data[row][col] & 0xf]);
      }
      fprintf(FP, "\n");
   }
   fprintf(FP, "%%%%EndImage\n");
   fprintf(FP, "%%%%EndPreview\n");

   for (row=0; row < image_h; row++) free(data[row]);
   free(data);
   XDestroyImage(image);
   XFreePixmap(mainDisplay, pixmap);
}

static char defXpmToGif[]="xpmtoppm %s | ppmtogif";
static char defXpmToPng[]="xpmtoppm %s | pnmtopng";
static char defXpmToJpeg[]="xpmtoppm %s | cjpeg";
static char defPpmToGif[]="ppmquant 222 %s | ppmtogif";
static char defPpmToPng[]="pnmtopng %s";
static char defPpmToPngWithTrans[]="pnmtopng -transparent %s %s";
static char defPpmToJpeg[]="cjpeg %s";
static char defPpm6ToXpm3[]="ppmtoxpm %s";
static char defXpmDeckToGifAnim[]="gifsicle -lforever --delay 10";

static
void InitImageMap()
{
   char *c_ptr;

   if (generateImageMap != INVALID) return;

   generateImageMap = FALSE;
   if (!PRTGIF || cmdLineOpenDisplay) {
      if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"GenerateImageMap")) !=
            NULL && UtilStrICmp(c_ptr, "true") == 0) {
         generateImageMap = TRUE;
      }
   }
   strcpy(xpmToGifCmd, defXpmToGif);
   if (!PRTGIF || cmdLineOpenDisplay) {
      if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"XpmToGif")) != NULL) {
         int count=0;

         strcpy(xpmToGifCmd, c_ptr);
         UtilTrimBlanks(xpmToGifCmd);
         for (c_ptr=strstr(xpmToGifCmd,"%s"); c_ptr!=NULL;
               c_ptr=strstr(++c_ptr,"%s")) {
            count++;
         }
         if (count != 1) {
            sprintf(gszMsgBox, TgLoadString(STID_INVALID_XDEF_USE_ALT_STR),
                  TOOL_NAME, "XpmToGif", xpmToGifCmd, defXpmToGif);
            MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
            strcpy(xpmToGifCmd, defXpmToGif);
         }
      }
   }
   strcpy(ppmToGifCmd, defPpmToGif);
   if (!PRTGIF || cmdLineOpenDisplay) {
      if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"PpmToGif")) != NULL) {
         int count=0;

         strcpy(ppmToGifCmd, c_ptr);
         UtilTrimBlanks(ppmToGifCmd);
         for (c_ptr=strstr(ppmToGifCmd,"%s"); c_ptr!=NULL;
               c_ptr=strstr(++c_ptr,"%s")) {
            count++;
         }
         if (count != 1) {
            sprintf(gszMsgBox, TgLoadString(STID_INVALID_XDEF_USE_ALT_STR),
                  TOOL_NAME, "PpmToGif", ppmToGifCmd, defPpmToGif);
            MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
            strcpy(ppmToGifCmd, defPpmToGif);
         }
      }
   }
   strcpy(gifFileExtension, "gif");
   if (!PRTGIF || cmdLineOpenDisplay) {
      if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"GifFileExtension")) !=
            NULL) {
         strcpy(gifFileExtension, c_ptr);
      }
   }
   strcpy(xpmToPngCmd, defXpmToPng);
   if (!PRTGIF || cmdLineOpenDisplay) {
      if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"XpmToPng")) != NULL) {
         int count=0;

         strcpy(xpmToPngCmd, c_ptr);
         UtilTrimBlanks(xpmToPngCmd);
         for (c_ptr=strstr(xpmToPngCmd,"%s"); c_ptr!=NULL;
               c_ptr=strstr(++c_ptr,"%s")) {
            count++;
         }
         if (count != 1) {
            sprintf(gszMsgBox, TgLoadString(STID_INVALID_XDEF_USE_ALT_STR),
                  TOOL_NAME, "XpmToPng", xpmToPngCmd, defXpmToPng);
            MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
            strcpy(xpmToPngCmd, defXpmToPng);
         }
      }
   }
   strcpy(ppmToPngCmd, defPpmToPng);
   if (!PRTGIF || cmdLineOpenDisplay) {
      if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"PpmToPng")) != NULL) {
         int count=0;

         strcpy(ppmToPngCmd, c_ptr);
         UtilTrimBlanks(ppmToPngCmd);
         for (c_ptr=strstr(ppmToPngCmd,"%s"); c_ptr!=NULL;
               c_ptr=strstr(++c_ptr,"%s")) {
            count++;
         }
         if (count != 1) {
            sprintf(gszMsgBox, TgLoadString(STID_INVALID_XDEF_USE_ALT_STR),
                  TOOL_NAME, "PpmToPng", ppmToPngCmd, defPpmToPng);
            MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
            strcpy(ppmToPngCmd, defPpmToPng);
         }
      }
   }
   strcpy(ppmToPngWithTransCmd, defPpmToPngWithTrans);
   if (!PRTGIF || cmdLineOpenDisplay) {
      if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,
            "PpmToPngWithTransparentColor")) != NULL) {
         int count=0;

         strcpy(ppmToPngWithTransCmd, c_ptr);
         UtilTrimBlanks(ppmToPngWithTransCmd);
         for (c_ptr=strstr(ppmToPngWithTransCmd,"%s"); c_ptr!=NULL;
               c_ptr=strstr(++c_ptr,"%s")) {
            count++;
         }
         if (count != 2) {
            sprintf(gszMsgBox, TgLoadString(STID_INVALID_XDEF_USE_ALT_STR),
                  TOOL_NAME, "PpmToPngWithTransparentColor",
                  ppmToPngWithTransCmd, defPpmToPngWithTrans);
            MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
            strcpy(ppmToPngWithTransCmd, defPpmToPngWithTrans);
         }
      }
   }
   strcpy(pngFileExtension, "png");
   if (!PRTGIF || cmdLineOpenDisplay) {
      if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"PngFileExtension")) !=
            NULL) {
         strcpy(pngFileExtension, c_ptr);
      }
   }
   strcpy(xpmToJpegCmd, defXpmToJpeg);
   if (!PRTGIF || cmdLineOpenDisplay) {
      if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"XpmToJpeg")) != NULL) {
         int count=0;

         strcpy(xpmToJpegCmd, c_ptr);
         UtilTrimBlanks(xpmToJpegCmd);
         for (c_ptr=strstr(xpmToJpegCmd,"%s"); c_ptr!=NULL;
               c_ptr=strstr(++c_ptr,"%s")) {
            count++;
         }
         if (count != 1) {
            sprintf(gszMsgBox, TgLoadString(STID_INVALID_XDEF_USE_ALT_STR),
                  TOOL_NAME, "XpmToJpeg", xpmToJpegCmd, defXpmToJpeg);
            MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
            strcpy(xpmToJpegCmd, defXpmToJpeg);
         }
      }
   }
   strcpy(ppmToJpegCmd, defPpmToJpeg);
   if (!PRTGIF || cmdLineOpenDisplay) {
      if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"PpmToJpeg")) != NULL) {
         int count=0;

         strcpy(ppmToJpegCmd, c_ptr);
         UtilTrimBlanks(ppmToJpegCmd);
         for (c_ptr=strstr(ppmToJpegCmd,"%s"); c_ptr!=NULL;
               c_ptr=strstr(++c_ptr,"%s")) {
            count++;
         }
         if (count != 1) {
            sprintf(gszMsgBox, TgLoadString(STID_INVALID_XDEF_USE_ALT_STR),
                  TOOL_NAME, "PpmToJpeg", ppmToJpegCmd, defPpmToJpeg);
            MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
            strcpy(ppmToJpegCmd, defPpmToJpeg);
         }
      }
   }
   strcpy(ppm6ToXpm3Cmd, defPpm6ToXpm3);
   if (!PRTGIF || cmdLineOpenDisplay) {
      if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"Ppm6ToXpm3")) != NULL) {
         int count=0;

         strcpy(ppm6ToXpm3Cmd, c_ptr);
         UtilTrimBlanks(ppm6ToXpm3Cmd);
         for (c_ptr=strstr(ppm6ToXpm3Cmd,"%s"); c_ptr!=NULL;
               c_ptr=strstr(++c_ptr,"%s")) {
            count++;
         }
         if (count != 1) {
            sprintf(gszMsgBox, TgLoadString(STID_INVALID_XDEF_USE_ALT_STR),
                  TOOL_NAME, "PpmToJpeg", ppm6ToXpm3Cmd, defPpm6ToXpm3);
            MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
            strcpy(ppm6ToXpm3Cmd, defPpm6ToXpm3);
         }
      }
   }
   strcpy(jpegFileExtension, "jpeg");
   if (!PRTGIF || cmdLineOpenDisplay) {
      if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"JpegFileExtension")) !=
            NULL) {
         strcpy(jpegFileExtension, c_ptr);
      }
   }
#ifdef _XPM3TOPPM
   useXPmVersion1ForImageMap = FALSE;
#else /* ~_XPM3TOPPM */
   useXPmVersion1ForImageMap = TRUE;
#endif /* _XPM3TOPPM */
   if (!PRTGIF || cmdLineOpenDisplay) {
      if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,
            "UseXPmVersion1ForImageMap")) != NULL) {
         if (UtilStrICmp("false",c_ptr) == 0) {
            useXPmVersion1ForImageMap = FALSE;
         } else if (UtilStrICmp("true",c_ptr) == 0) {
            useXPmVersion1ForImageMap = TRUE;
         }
      }
   }
   useXPmVersion1ForXPmDeck = TRUE;
   if (!PRTGIF || cmdLineOpenDisplay) {
      if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,
            "UseXPmVersion1ForXPmDeck")) != NULL &&
            UtilStrICmp("false",c_ptr) == 0) {
         useXPmVersion1ForXPmDeck = FALSE;
      }
   }
   strcpy(imageMapFileExtension, "map");
   if (!PRTGIF || cmdLineOpenDisplay) {
      if ((c_ptr=XGetDefault(mainDisplay, TOOL_NAME,
            "ImageMapFileExtension")) != NULL) {
         strcpy(imageMapFileExtension, c_ptr);
      }
   }
   imageMapFileFormat = IMF_FORMAT_NCSA;
   if (!PRTGIF || cmdLineOpenDisplay) {
      if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"ImageMapFileFormat")) !=
            NULL) {
         if (strcmp(c_ptr, "NCSA") == 0) {
            imageMapFileFormat = IMF_FORMAT_NCSA;
         } else if (strcmp(c_ptr, "CERN") == 0) {
            imageMapFileFormat = IMF_FORMAT_CERN;
         } else {
            sprintf(gszMsgBox, TgLoadString(STID_INVALID_XDEF_USE_ALT_STR),
                  TOOL_NAME, "ImageMapFileFormat", c_ptr, "NCSA");
            MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
         }
      }
   }
   strcpy(htmlFileExtension, "html");
   if (!PRTGIF || cmdLineOpenDisplay) {
      if ((c_ptr=XGetDefault(mainDisplay, TOOL_NAME,
            "HtmlFileExtension")) != NULL && (*c_ptr != '\0')) {
         strcpy(htmlFileExtension, c_ptr);
      }
   }
   generateHtmlHref = TRUE;
   if (!PRTGIF || cmdLineOpenDisplay) {
      if ((c_ptr=XGetDefault(mainDisplay, TOOL_NAME, "GenerateHtmlHref")) !=
            NULL) {
         if (UtilStrICmp(c_ptr, "false") == 0) {
            generateHtmlHref = FALSE;
         }
      }
   }
   strcpy(xpmDeckToGifAnimCmd, defXpmDeckToGifAnim);
   if (!PRTGIF || cmdLineOpenDisplay) {
      if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"XpmDeckToGifAnim")) !=
            NULL) {
         strcpy(xpmDeckToGifAnimCmd, c_ptr);
         UtilTrimBlanks(xpmDeckToGifAnimCmd);
         if (strstr(xpmDeckToGifAnimCmd, "%s") != NULL) {
            sprintf(gszMsgBox, TgLoadString(STID_INVALID_XDEF_USE_ALT_STR),
                  TOOL_NAME, "XpmDeckToGifAnim", xpmDeckToGifAnimCmd,
                  defXpmDeckToGifAnim);
            MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
            strcpy(xpmDeckToGifAnimCmd, defXpmDeckToGifAnim);
         }
      }
   }
}

static
char *ModifyToGenerateHtmlHref(value_str)
   char *value_str;
   /* data in value_str is not supposed to be touched on return */
{
   char *href=NULL, *name=NULL, *pound=NULL, *dot=NULL;
   int len=0;

   if (generateHtmlHref && imageMapFileFormat==IMF_FORMAT_SPYG) {
      if (value_str[0] == '#' && strchr(value_str, DIR_SEP) == NULL &&
            (((!(PRTGIF && !cmdLineOpenDisplay)) && dumpOneFilePerPage) ||
            (PRTGIF && !cmdLineOpenDisplay && cmdLineOneFilePerPage))) {
         char page_spec[MAXSTRING];
         int page_num=0;

         if (GetPageNumFromPageSpec(&value_str[1], &page_num)) {
            sprintf(page_spec, "%1d", page_num);

            if ((name=UtilStrRChr(curFileName, DIR_SEP)) == NULL) {
               name = curFileName;
            } else {
               name++;
            }
            pound = strchr(name, '#');
            if (pound != NULL) *pound = '\0';
            dot = UtilStrRChr(name, '.');
            if (dot != NULL) {
               *dot = '\0';
               len = strlen(name)+strlen(page_spec)+strlen(htmlFileExtension)+3;
               href = (char*)malloc((len+1)*sizeof(char));
               if (href == NULL) FailAllocMessage();
               sprintf(href, "%s-%1d.%s", name, page_num, htmlFileExtension);
               *dot = '.';
            }
            if (pound != NULL) *pound = '#';
         } else {
            /* Cannot get page number information, may be it's okay! */
         }
      } else {
         if ((name=UtilStrRChr(value_str, DIR_SEP)) == NULL) {
            name = value_str;
         } else {
            name++;
         }
         pound = strchr(name, '#');
         if (pound != NULL) *pound = '\0';
         dot = UtilStrRChr(name, '.');
         if (dot != NULL && ((UtilStrICmp(&dot[1], "obj")==0 ||
               UtilStrICmp(&dot[1], OBJ_FILE_EXT)==0) ||
               UtilStrICmp(&dot[1], "sym")==0 ||
               UtilStrICmp(&dot[1], SYM_FILE_EXT)==0)) {
            *dot = '\0';
            len = strlen(value_str)+strlen(htmlFileExtension)+2;
            if (pound != NULL) len += strlen(&pound[1])+1;
            href = (char*)malloc((len+1)*sizeof(char));
            if (href == NULL) FailAllocMessage();
            if (pound != NULL) {
               sprintf(href, "%s.%s#%s", value_str, htmlFileExtension,
                     &pound[1]);
            } else {
               sprintf(href, "%s.%s", value_str, htmlFileExtension);
            }
            *dot = '.';
         }
         if (pound != NULL) *pound = '#';
      }
   }
   return href;
}

static
int GenerateObjImageMap(FP, ObjPtr, LtX, LtY)
   FILE *FP;
   struct ObjRec *ObjPtr;
   int LtX, LtY;
{
   register int i;
   int n, something_generated=FALSE;
   struct AttrRec *attr_ptr;
   struct ObjRec *obj_ptr;
   IntPoint *v=NULL;

   if ((attr_ptr=FindAttrWithName(ObjPtr, "href=", NULL)) != NULL) {
      char *href=ModifyToGenerateHtmlHref(attr_ptr->attr_value.s);

      switch (ObjPtr->type) {
      case OBJ_POLY:
      case OBJ_POLYGON:
         if (ObjPtr->type == OBJ_POLY) {
            n = ObjPtr->detail.p->n;
            v = ObjPtr->detail.p->vlist;
         } else {
            n = ObjPtr->detail.g->n;
            v = ObjPtr->detail.g->vlist;
         }
         switch (imageMapFileFormat) {
         case IMF_FORMAT_NCSA:
            fprintf(FP, "\npoly %s", attr_ptr->attr_value.s);
            for (i=0; i < n; i++) {
               fprintf(FP, " %1d,%1d", v[i].x-LtX, v[i].y-LtY);
            }
            fprintf(FP, "\n");
            break;
         case IMF_FORMAT_CERN:
            fprintf(FP, "\npoly");
            for (i=0; i < n; i++) {
               fprintf(FP, " (%1d,%1d)", v[i].x-LtX, v[i].y-LtY);
            }
            fprintf(FP, " %s\n", attr_ptr->attr_value.s);
            break;
         case IMF_FORMAT_SPYG:
            fprintf(FP, "<AREA SHAPE=\"POLY\" COORDS=\"");
            for (i=0; i < n; i++) {
               fprintf(FP, "%s%1d,%1d", (i==0 ? "" : ","),
                     v[i].x-LtX, v[i].y-LtY);
            }
            fprintf(FP, "\" HREF=\"%s\">\n",
                  (href != NULL ? href : attr_ptr->attr_value.s));
            break;
         default: break;
         }
         break;
      case OBJ_OVAL:
         if (ObjPtr->obbox.rbx-ObjPtr->obbox.ltx ==
               ObjPtr->obbox.rby-ObjPtr->obbox.lty) {
            switch (imageMapFileFormat) {
            case IMF_FORMAT_NCSA:
               fprintf(FP, "\ncircle %s %1d,%1d %1d,%1d\n",
                     attr_ptr->attr_value.s,
                     ((ObjPtr->obbox.ltx+ObjPtr->obbox.rbx)>>1)-LtX,
                     ((ObjPtr->obbox.lty+ObjPtr->obbox.rby)>>1)-LtY,
                     ObjPtr->obbox.rbx-LtX,
                     ((ObjPtr->obbox.lty+ObjPtr->obbox.rby)>>1)-LtY);
               break;
            case IMF_FORMAT_CERN:
               fprintf(FP, "\ncircle (%1d,%1d) %1d %s\n",
                     ((ObjPtr->obbox.ltx+ObjPtr->obbox.rbx)>>1)-LtX,
                     ((ObjPtr->obbox.lty+ObjPtr->obbox.rby)>>1)-LtY,
                     (ObjPtr->obbox.rbx-ObjPtr->obbox.ltx)>>1,
                     attr_ptr->attr_value.s);
               break;
            case IMF_FORMAT_SPYG:
               fprintf(FP, "<AREA SHAPE=\"CIRCLE\" COORDS=\"");
               fprintf(FP, "%1d,%1d,%1d",
                     ((ObjPtr->obbox.ltx+ObjPtr->obbox.rbx)>>1)-LtX,
                     ((ObjPtr->obbox.lty+ObjPtr->obbox.rby)>>1)-LtY,
                     (ObjPtr->obbox.rbx-ObjPtr->obbox.ltx)>>1);
               fprintf(FP, "\" HREF=\"%s\">\n",
                  (href != NULL ? href : attr_ptr->attr_value.s));
               break;
            default: break;
            }
         } else {
            switch (imageMapFileFormat) {
            case IMF_FORMAT_NCSA:
               fprintf(FP, "\nrect %s %1d,%1d %1d,%1d\n",
                     attr_ptr->attr_value.s,
                     ObjPtr->obbox.ltx-LtX, ObjPtr->obbox.lty-LtY,
                     ObjPtr->obbox.rbx-LtX, ObjPtr->obbox.rby-LtY);
               break;
            case IMF_FORMAT_CERN:
               fprintf(FP, "\nrect (%1d,%1d) (%1d,%1d) %s\n",
                     ObjPtr->obbox.ltx-LtX, ObjPtr->obbox.lty-LtY,
                     ObjPtr->obbox.rbx-LtX, ObjPtr->obbox.rby-LtY,
                     attr_ptr->attr_value.s);
               break;
            case IMF_FORMAT_SPYG:
               fprintf(FP, "<AREA SHAPE=\"RECT\" COORDS=\"");
               fprintf(FP, "%1d,%1d,%1d,%1d",
                     ObjPtr->obbox.ltx-LtX, ObjPtr->obbox.lty-LtY,
                     ObjPtr->obbox.rbx-LtX, ObjPtr->obbox.rby-LtY);
               fprintf(FP, "\" HREF=\"%s\">\n",
                  (href != NULL ? href : attr_ptr->attr_value.s));
               break;
            default: break;
            }
         }
         break;
      default:
         switch (imageMapFileFormat) {
         case IMF_FORMAT_NCSA:
            fprintf(FP, "\nrect %s %1d,%1d %1d,%1d\n",
                  attr_ptr->attr_value.s,
                  ObjPtr->obbox.ltx-LtX, ObjPtr->obbox.lty-LtY,
                  ObjPtr->obbox.rbx-LtX, ObjPtr->obbox.rby-LtY);
            break;
         case IMF_FORMAT_CERN:
            fprintf(FP, "\nrect (%1d,%1d) (%1d,%1d) %s\n",
                  ObjPtr->obbox.ltx-LtX, ObjPtr->obbox.lty-LtY,
                  ObjPtr->obbox.rbx-LtX, ObjPtr->obbox.rby-LtY,
                  attr_ptr->attr_value.s);
            break;
         case IMF_FORMAT_SPYG:
            fprintf(FP, "<AREA SHAPE=\"RECT\" COORDS=\"");
            fprintf(FP, "%1d,%1d,%1d,%1d",
                  ObjPtr->obbox.ltx-LtX, ObjPtr->obbox.lty-LtY,
                  ObjPtr->obbox.rbx-LtX, ObjPtr->obbox.rby-LtY);
            fprintf(FP, "\" HREF=\"%s\">\n",
                  (href != NULL ? href : attr_ptr->attr_value.s));
            break;
         default: break;
         }
         break;
      }
      if (href != NULL) free(href);
      return TRUE;
   }
   switch (ObjPtr->type) {
   case OBJ_GROUP:
   case OBJ_ICON:
   case OBJ_SYM:
      for (obj_ptr=ObjPtr->detail.r->first; obj_ptr != NULL;
            obj_ptr=obj_ptr->next) {
         if (GenerateObjImageMap (FP, obj_ptr, LtX, LtY)) {
            something_generated = TRUE;;
         }
      }
      break;
   case OBJ_PIN:
      obj_ptr = GetPinObj(ObjPtr);
      if (GenerateObjImageMap(FP, obj_ptr, LtX, LtY)) {
         something_generated = TRUE;;
      }
      break;
   }
   return something_generated;
}

static
void GenerateHtmlHeader(map_fp)
   FILE *map_fp;
{
   struct AttrRec *attr_ptr;

   fprintf(map_fp, "<HTML>\n<HEAD>\n");
   if (curPage->name != NULL && *curPage->name != '\0') {
      fprintf(map_fp, "<TITLE>\n%s\n</TITLE>\n", curPage->name);
   } else if ((attr_ptr=FindFileAttrWithName("title=")) != NULL) {
      fprintf(map_fp, "<TITLE>\n%s\n</TITLE>\n",
            attr_ptr->attr_value.s);
   }
   if ((attr_ptr=FindFileAttrWithName("base=")) != NULL) {
      fprintf(map_fp, "<BASE HREF=\"%s\">\n", attr_ptr->attr_value.s);
   }
   if ((attr_ptr=FindFileAttrWithName("made=")) != NULL) {
      fprintf(map_fp, "<LINK REV=\"made\" HREF=\"%s\">\n",
            attr_ptr->attr_value.s);
   }
   if ((attr_ptr=FindFileAttrWithName("keywords=")) != NULL) {
      fprintf(map_fp, "<META HTTP-EQUIV=\"Keywords\" CONTENT=\"%s\">\n",
            attr_ptr->attr_value.s);
   }
   fprintf(map_fp, "</HEAD>\n<BODY>\n");
}

static
void GenerateUseMapHeader(map_fp, gif_fname, map_fname)
   FILE *map_fp;
   char *gif_fname, *map_fname;
{
   char *gif_fname_ptr=UtilStrRChr(gif_fname, DIR_SEP);
   char *map_fname_ptr=UtilStrRChr(map_fname, DIR_SEP);
   int page_num=0;
   struct AttrRec *attr_ptr;

   if (gif_fname_ptr != NULL) gif_fname_ptr++;
   if (map_fname_ptr != NULL) map_fname_ptr++;
   fprintf(map_fp, "<H1>\n");
   attr_ptr = FindFileAttrWithName("alt=");
   if (attr_ptr != NULL) {
      fprintf(map_fp, "<IMG ALT=\"%s\" SRC=\"%s\" USEMAP=\"%s#p%1d\">\n",
            attr_ptr->attr_value.s,
            (gif_fname_ptr==NULL ? "unknown" : gif_fname_ptr),
            (map_fname_ptr==NULL ? "unknown" : map_fname_ptr), page_num);
   } else {
      fprintf(map_fp, "<IMG SRC=\"%s\" USEMAP=\"%s#p%1d\">\n",
            (gif_fname_ptr==NULL ? "unknown" : gif_fname_ptr),
            (map_fname_ptr==NULL ? "unknown" : map_fname_ptr), page_num);
   }
   fprintf(map_fp, "</H1>\n");
   fprintf(map_fp, "<MAP NAME=\"p%1d\">\n", page_num);
}

static
void GenerateUseMapTrailer(map_fp, W, H)
   FILE *map_fp;
   int W, H;
{
   struct AttrRec *attr_ptr=FindFileAttrWithName("href=");

   if (attr_ptr != NULL) {
      char *href=ModifyToGenerateHtmlHref(attr_ptr->attr_value.s);

      fprintf(map_fp, "<AREA SHAPE=\"RECT\" COORDS=\"");
      fprintf(map_fp, "0,0,%1d,%1d", W, H);
      fprintf(map_fp, "\" HREF=\"%s\">\n",
            (href != NULL ? href : attr_ptr->attr_value.s));
      if (href != NULL) free(href);
   }
   fprintf(map_fp, "</MAP>\n");
}

static
void GenerateHtmlTrailer(map_fp)
   FILE *map_fp;
{
   fprintf(map_fp, "</BODY>\n</HTML>\n");
}

static
FILE *OpenMapFile(pszMapFile)
   char *pszMapFile;
{
   FILE *pFile=fopen(pszMapFile, "w");

   if (pFile == NULL) {
      sprintf(gszMsgBox, TgLoadString(STID_CANNOT_OPEN_FILE_FOR_WRITING),
            pszMapFile);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return NULL;
   }
   return pFile;
}

static
void DumpCommentInBitmapExport(fp)
   FILE *fp;
{
   if (commentInBitmapExport) {
      fprintf(fp, "/*\n");
      fprintf(fp, " * @%s%s\n", "(#)$H", "eader$");
      fprintf(fp, " */\n");
   }
}

static
void ColorNameToXPmStr(color_name, s)
   char *color_name, *s;
{
   if (*color_name == '#') {
      strcpy(s, color_name);
   } else {
      XColor exact_def;

      if (TgifParseColor(color_name, &exact_def)) {
         unsigned int r=0, g=0, b=0;
         double dr=(double)0, dg=(double)0, db=(double)0;

         dr = ((double)exact_def.red) / ((double)maxRGB) * ((double)255);
         dg = ((double)exact_def.green) / ((double)maxRGB) * ((double)255);
         db = ((double)exact_def.blue) / ((double)maxRGB) * ((double)255);
         r = (unsigned int)round(dr);
         g = (unsigned int)round(dg);
         b = (unsigned int)round(db);
         if (r > 255) r = 255;
         if (g > 255) g = 255;
         if (b > 255) b = 255;
         sprintf(s, "#%02x%02x%02x", r, g, b);
      } else {
         strcpy(s, color_name);
      }
   }
}

static
int Xpm3ToXpm1(gif_fname)
   char *gif_fname;
{
   int i=0, j=0, rc=0, ncolors=0, chars_per_pixel=0, line_len=0;
   int first_pixel_is_bg=FALSE, image_w=0, image_h=0, w=0, h=0;
   char *color_char=NULL, **color_str=NULL, *xpm_data=NULL, *xpm_data_ptr=NULL;
   char *psz=NULL, name[MAXPATHLENGTH], s[MAXSTRING];
   int watch_cursor=watchCursorOnMainWindow, saved_color_display=colorDisplay;
   FILE *fp=NULL;

   psz = UtilStrRChr(gif_fname, DIR_SEP);
   if (psz == NULL) {
      UtilStrCpyN(name, sizeof(name), gif_fname);
   } else {
      UtilStrCpyN(name, sizeof(name), &psz[1]);
   }
   psz = UtilStrRChr(name, '.');
   if (psz != NULL) {
      *psz = '\0';
   }
   UtilTrimBlanks(name);
   if (*name == '\0') {
#ifdef _TGIF_DBG /* debug, do not translate */
      fprintf(stderr, "Unexpected empty string in Xpm3ToXpm1().\n");
#endif /* _TGIF_DBG */
      return FALSE;
   }
   if (!watch_cursor) {
      SetWatchCursor(drawWindow);
      SetWatchCursor(mainWindow);
   }
   colorDisplay = FALSE;
   rc = MyReadPixmapFile(gif_fname,
         &image_w, &image_h, &w, &h, NULL, NULL, NULL, NULL,
         &ncolors, &chars_per_pixel, &first_pixel_is_bg, &color_char,
         &color_str, NULL, &xpm_data);
   colorDisplay = saved_color_display;
   if (rc != BitmapSuccess) {
      if (!watch_cursor) {
         SetDefaultCursor(mainWindow);
         ShowCursor();
      }
      sprintf(gszMsgBox, TgLoadString(STID_CANNOT_IMPORT_XPM_FILE), gif_fname);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return FALSE;
   }
   if ((fp=fopen(gif_fname, "w")) == NULL) {
      if (!watch_cursor) {
         SetDefaultCursor(mainWindow);
         ShowCursor();
      }
      sprintf(gszMsgBox, TgLoadString(STID_CANNOT_OPEN_FILE_FOR_WRITING),
            gif_fname);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return FALSE;
   }
   writeFileFailed = FALSE;

   /* DumpXpmHeader(fp, w, h, name, 0, 0, 0, 0); */

   DumpCommentInBitmapExport(fp);
   /* do not translate -- program constants */
   fprintf(fp, "#define %s_format 1\n", name);
   fprintf(fp, "#define %s_width %1d\n", name, image_w);
   fprintf(fp, "#define %s_height %1d\n", name, image_h);
   fprintf(fp, "#define %s_ncolors %1d\n", name, ncolors);
   fprintf(fp, "#define %s_chars_per_pixel %1d\n", name, chars_per_pixel);
   if (xpmInXGrabSCFormat) {
      fprintf(fp, "static char * %s_colors[] = {\n", name);
   } else {
      fprintf(fp, "static char *%s_colors[] = {\n", name);
   }
   /* DumpXPmColors(fp); */
   if (xpmInXGrabSCFormat) {
      for (i=0; i < ncolors-1; i++) {
         if (fprintf(fp, "\"") == EOF) writeFileFailed = TRUE;
         for (j = 0; j < chars_per_pixel; j++) {
            if (fprintf(fp, "%c", color_char[i*chars_per_pixel+j]) == EOF) {
               writeFileFailed = TRUE;
            }
         }
         ColorNameToXPmStr(color_str[i], s);
         if (fprintf(fp, "\", \"%s\"\n", s) == EOF) writeFileFailed = TRUE;
      }
      if (fprintf(fp, "\"") == EOF) writeFileFailed = TRUE;
      for (j = 0; j < chars_per_pixel; j++) {
         if (fprintf(fp, "%c", color_char[i*chars_per_pixel+j]) == EOF) {
            writeFileFailed = TRUE;
         }
      }
      ColorNameToXPmStr(color_str[i], s);
      if (fprintf(fp, "\", \"%s\"\n} ;\n", s) == EOF) {
         writeFileFailed = TRUE;
      }
   } else {
      for (i=0; i < ncolors-1; i++) {
         if (fprintf(fp, "   \"") == EOF) writeFileFailed = TRUE;
         for (j = 0; j < chars_per_pixel; j++) {
            if (fprintf(fp, "%c", color_char[i*chars_per_pixel+j]) == EOF) {
               writeFileFailed = TRUE;
            }
         }
         if (fprintf(fp, "\", \"%s\",\n", color_str[i]) == EOF) {
            writeFileFailed = TRUE;
         }
      }
      if (fprintf(fp, "   \"") == EOF) writeFileFailed = TRUE;
      for (j = 0; j < chars_per_pixel; j++) {
         if (fprintf(fp, "%c", color_char[i*chars_per_pixel+j]) == EOF) {
            writeFileFailed = TRUE;
         }
      }
      if (fprintf(fp, "\", \"%s\"\n};\n", color_str[i]) == EOF) {
         writeFileFailed = TRUE;
      }
   }

   if (xpmInXGrabSCFormat) {
      fprintf(fp, "static char * %s_pixels[] = {\n", name);
   } else {
      fprintf(fp, "static char *%s_pixels[] = {\n", name);
   }
   line_len = chars_per_pixel * image_w;
   xpm_data_ptr = xpm_data;
   for (i=0; i < image_h; i++) {
      char saved_ch=xpm_data_ptr[line_len];

      xpm_data_ptr[line_len] = '\0';
      fprintf(fp, "\"%s\"", xpm_data_ptr);
      xpm_data_ptr[line_len] = saved_ch;
      xpm_data_ptr += line_len;

      if (i == image_h-1) {
         if (xpmInXGrabSCFormat) {
            if (fprintf(fp, ",\n} ;\n") == EOF) writeFileFailed = TRUE;
         } else {
            if (fprintf(fp, "\n};\n") == EOF) writeFileFailed = TRUE;
         }
      } else if (fprintf(fp, ",\n") == EOF) {
         writeFileFailed = TRUE;
      }
   }
   fclose(fp);

   if (!watch_cursor) {
      SetDefaultCursor(mainWindow);
      ShowCursor();
   }
   return TRUE;
}

static
int ConvertXpmToGif(xpm_fname, gif_fname, src_is_ppm, where_to_print)
   char *xpm_fname, *gif_fname;
   int src_is_ppm, where_to_print;
{
   char cmd[MAXSTRING<<1], netpbm_msg[MAXSTRING<<1], buf[1024];
   FILE *gif_fp=fopen(gif_fname,"w"), *pfp=NULL;
   int bytes_read=0, watch_cursor=watchCursorOnMainWindow;

   if (gif_fp == NULL) {
      sprintf(gszMsgBox, TgLoadString(STID_CANNOT_OPEN_FILE_FOR_WRITING),
            gif_fname);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return FALSE;
   }
   switch (where_to_print) {
   case GIF_FILE:
   case HTML_FILE:
      *netpbm_msg = '\0';
      if (src_is_ppm) {
         sprintf(cmd, ppmToGifCmd, xpm_fname);
         if (strncmp(cmd, "ppm", 3) == 0) {
            sprintf(netpbm_msg, TgLoadString(STID_NETPBM_PROGS_VISIT_HOME_PAGE),
                  TOOL_NAME, homePageURL);
         }
      } else {
         sprintf(cmd, xpmToGifCmd, xpm_fname);
         if (strncmp(cmd, "xpmtop", 6) == 0) {
            sprintf(netpbm_msg, TgLoadString(STID_NETPBM_PROGS_VISIT_HOME_PAGE),
                  TOOL_NAME, homePageURL);
         }
      }
      if (!FindProgramInPath(cmd, netpbm_msg, FALSE)) {
         fclose(gif_fp);
         unlink(gif_fname);
         return FALSE;
      }
      break;
   case PNG_FILE:
      *netpbm_msg = '\0';
      if (src_is_ppm) {
         if (pngExportHasTransparentColor) {
            sprintf(cmd, ppmToPngWithTransCmd, pngExportTransparentColor,
                  xpm_fname);
            if (strncmp(cmd, "pnmtop", 6) == 0) {
               sprintf(netpbm_msg,
                     TgLoadString(STID_NETPBM_PNMTOPNG_VST_HOME_PAGE),
                     TOOL_NAME, homePageURL);
            }
         } else {
            sprintf(cmd, ppmToPngCmd, xpm_fname);
            if (strncmp(cmd, "pnmtop", 6) == 0) {
               sprintf(netpbm_msg,
                     TgLoadString(STID_NETPBM_PNMTOPNG_VST_HOME_PAGE),
                     TOOL_NAME, homePageURL);
            }
         }
      } else {
         sprintf(cmd, xpmToPngCmd, xpm_fname);
         if (strncmp(cmd, "xpmtop", 6) == 0) {
            sprintf(netpbm_msg,
                  TgLoadString(STID_NETPBM_PNMTOPNG_VST_HOME_PAGE), TOOL_NAME,
                  homePageURL);
         }
      }
      if (!FindProgramInPath(cmd, netpbm_msg, FALSE)) {
         fclose(gif_fp);
         unlink(gif_fname);
         return FALSE;
      }
      break;
   case JPEG_FILE:
      *netpbm_msg = '\0';
      if (src_is_ppm) {
         sprintf(cmd, ppmToJpegCmd, xpm_fname);
         if (strncmp(cmd, "cjpeg", 5) == 0) {
            sprintf(netpbm_msg, TgLoadString(STID_NETPBM_JPEG_VST_HOME_PAGE),
                  TOOL_NAME, homePageURL);
         }
      } else {
         sprintf(cmd, xpmToJpegCmd, xpm_fname);
         if (strncmp(cmd, "xpmtop", 6) == 0) {
            sprintf(netpbm_msg, TgLoadString(STID_NETPBM_JPEG_VST_HOME_PAGE),
                  TOOL_NAME, homePageURL);
         }
      }
      if (!FindProgramInPath(cmd, netpbm_msg, FALSE)) {
         fclose(gif_fp);
         unlink(gif_fname);
         return FALSE;
      }
      break;
   case XBM_FILE:
      sprintf(cmd, ppm6ToXpm3Cmd, xpm_fname);
      *netpbm_msg = '\0';
      if (strncmp(cmd, "ppmtox", 6) == 0) {
         sprintf(netpbm_msg, TgLoadString(STID_NETPBM_JPEG_VST_HOME_PAGE),
               TOOL_NAME, homePageURL);
      }
      if (!FindProgramInPath(cmd, netpbm_msg, FALSE)) {
         fclose(gif_fp);
         unlink(gif_fname);
         return FALSE;
      }
      break;
   }
   sprintf(gszMsgBox, TgLoadCachedString(CSTID_EXECUTING_GIVEN_PROGRAM), cmd);
   Msg(gszMsgBox);
   SetStringStatus(gszMsgBox);
   XSync(mainDisplay, False);
   if ((pfp=(FILE*)popen(cmd, "r")) == NULL) {
      fclose(gif_fp);
      unlink(gif_fname);
      sprintf(gszMsgBox, TgLoadString(STID_FAIL_TO_EXECUTE_CMD), cmd);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return FALSE;
   }
   if (!watch_cursor) {
      SetWatchCursor(drawWindow);
      SetWatchCursor(mainWindow);
   }
   while ((bytes_read=fread(buf, sizeof(char), sizeof(buf), pfp)) > 0) {
      if ((int)fwrite(buf, sizeof(char), bytes_read, gif_fp) <= 0) {
         writeFileFailed = TRUE;
         break;
      }
   }
   pclose(pfp);
   if (!watch_cursor) {
      SetDefaultCursor(mainWindow);
      ShowCursor();
   }
   SetStringStatus(TgLoadCachedString(CSTID_DOTS_DONE));
   fclose(gif_fp);
   if (writeFileFailed) {
      return FailToWriteFileMessage(gif_fname);
   }
   if (whereToPrint == XBM_FILE && xpmOutputVersion == 1) {
      return Xpm3ToXpm1(gif_fname);
   }
   return TRUE;
}

#define TGV_FILE_NAME         0
#define TGV_CUR_NUM           1
#define TGV_FIRST_NUM         2
#define TGV_LAST_NUM          3
#define TGV_PREV_NUM          4
#define TGV_PREV_NUM_NO_WRAP  5
#define TGV_NEXT_NUM          6
#define TGV_NEXT_NUM_NO_WRAP  7
#define TGV_TITLE             8
#define TGV_MAP_WIDTH         9
#define TGV_MAP_HEIGHT       10
#define TGV_MAP_OBJS         11

struct tagTgVars {
   char *var_name;
   int var_type;
} gszTgVars[] = {
   { "&tgvfilename", TGV_FILE_NAME },
   { "&tgvcurnum", TGV_CUR_NUM },
   { "&tgvfirstnum", TGV_FIRST_NUM },
   { "&tgvlastnum", TGV_LAST_NUM },
   { "&tgvprevnum", TGV_PREV_NUM },
   { "&tgvprevnumnowrap", TGV_PREV_NUM_NO_WRAP },
   { "&tgvnextnum", TGV_NEXT_NUM },
   { "&tgvnextnumnowrap", TGV_NEXT_NUM_NO_WRAP },
   { "&tgvtitle", TGV_TITLE },
   { "&tgvmapwidth", TGV_MAP_WIDTH },
   { "&tgvmapheight", TGV_MAP_HEIGHT },
   { "&tgvmapobjs", TGV_MAP_OBJS },
   { NULL, INVALID }
};

static
int FindTgVarType(buf)
   char *buf;
{
   struct tagTgVars *ptv=NULL;

   for (ptv=gszTgVars; ptv->var_name != NULL; ptv++) {
      if (UtilStrICmp(buf, ptv->var_name) == 0) {
         return ptv->var_type;
      }
   }
   return INVALID;
}

static
void GenerateTemplateHtmlVar(map_fp, var_type, gif_fname, map_fname, fname,
      LtX, LtY, RbX, RbY)
   FILE *map_fp;
   char *gif_fname, *map_fname, *fname;
   int var_type, LtX, LtY, RbX, RbY;
{
   struct ObjRec *obj_ptr=NULL;
   struct AttrRec *attr_ptr=NULL;

   switch (var_type) {
   case TGV_FILE_NAME: fprintf(map_fp, "%s", fname); break;
   case TGV_CUR_NUM: fprintf(map_fp, "%1d", curPageNum); break;
   case TGV_FIRST_NUM: fprintf(map_fp, "1"); break;
   case TGV_LAST_NUM: fprintf(map_fp, "%1d", lastPageNum); break;
   case TGV_PREV_NUM:
      if (curPageNum == 1) {
         fprintf(map_fp, "%1d", lastPageNum);
      } else {
         fprintf(map_fp, "%1d", curPageNum-1);
      }
      break;
   case TGV_PREV_NUM_NO_WRAP:
      if (curPageNum == 1) {
         fprintf(map_fp, "1");
      } else {
         fprintf(map_fp, "%1d", curPageNum-1);
      }
      break;
   case TGV_NEXT_NUM:
      if (curPageNum == lastPageNum) {
         fprintf(map_fp, "1");
      } else {
         fprintf(map_fp, "%1d", curPageNum+1);
      }
      break;
   case TGV_NEXT_NUM_NO_WRAP:
      if (curPageNum == lastPageNum) {
         fprintf(map_fp, "%1d", lastPageNum);
      } else {
         fprintf(map_fp, "%1d", curPageNum+1);
      }
      break;
   case TGV_TITLE:
      if (curPage->name != NULL && *curPage->name != '\0') {
         fprintf(map_fp, "%s", curPage->name);
      } else if ((attr_ptr=FindFileAttrWithName("title=")) != NULL) {
         fprintf(map_fp, "%s", attr_ptr->attr_value.s);
      } else {
         fprintf(map_fp, TgLoadCachedString(CSTID_PARANED_UNKNOWN));
      }
      break;
   case TGV_MAP_WIDTH: fprintf(map_fp, "%1d", RbX-LtX); break;
   case TGV_MAP_HEIGHT: fprintf(map_fp, "%1d", RbY-LtY); break;
   case TGV_MAP_OBJS:
      for (obj_ptr=topObj; obj_ptr != NULL; obj_ptr=obj_ptr->next) {
         if (GenerateObjImageMap(map_fp, obj_ptr, LtX, LtY)) {
         }
      }
      break;
   }
}

static
void GenerateTemplateHtmlFile(map_fp, tmpl_fp, gif_fname, map_fname,
      LtX, LtY, RbX, RbY)
   FILE *map_fp, *tmpl_fp;
   char *gif_fname, *map_fname;
   int LtX, LtY, RbX, RbY;
{
   char *buf=NULL, *fname=NULL, *c_ptr=NULL;
   int line_num=0;

   if ((c_ptr=UtilStrRChr(curFileName, DIR_SEP)) == NULL) {
      fname = UtilStrDup(curFileName);
   } else {
      fname = UtilStrDup(++c_ptr);
   }
   if (fname == NULL) FailAllocMessage();
   if ((c_ptr=strchr(fname, '#')) != NULL) *c_ptr = '\0';
   GetRealBaseName(fname);
   if ((c_ptr=UtilStrRChr(fname, '.')) != NULL) *c_ptr = '\0';

   while ((buf=UtilGetALine(tmpl_fp)) != NULL) {
      char *psz_start=buf, *psz=NULL, *psz1=NULL;

      line_num++;
      if (*psz_start == '\0') {
         fprintf(map_fp, "%s\n", psz_start);
         UtilFree(buf);
         continue;
      }
      while ((psz=strstr(psz_start,"&tgv")) != NULL &&
            (psz1=strchr(psz,';')) != NULL) {
         int var_type=INVALID;

         *psz1 = '\0';
         if ((var_type=FindTgVarType(psz)) != INVALID) {
            *psz = '\0';
            fprintf(map_fp, "%s", psz_start);
            *psz = '&';
            psz_start = &psz1[1];
            GenerateTemplateHtmlVar(map_fp, var_type, gif_fname, map_fname,
                  fname, LtX, LtY, RbX, RbY);
         } else {
            fprintf(map_fp, "%s", psz_start);
            psz_start = psz1;
         }
         *psz1 = ';';
      }
      if (psz_start != NULL) {
         fprintf(map_fp, "%s\n", psz_start);
      } else {
         fprintf(map_fp, "\n");
      }
      UtilFree(buf);
   }
   UtilFree(fname);
}

static
void GenerateImageMap(XpmFileName, src_is_ppm, LtX, LtY, RbX, RbY,
      nPageNumInFileName)
   char *XpmFileName;
   int src_is_ppm, LtX, LtY, RbX, RbY, nPageNumInFileName;
{
   char map_fname[MAXPATHLENGTH+1], gif_fname[MAXPATHLENGTH+1];
   char tmpl_fname[MAXPATHLENGTH+1];
   struct ObjRec *obj_ptr=NULL;
   FILE *map_fp=NULL, *tmpl_fp=NULL;
   int xpm_ext_len, len, something_generated=FALSE, short_name=FALSE;
   struct AttrRec *attr_ptr=NULL;
   char *psz=UtilStrRChr(XpmFileName,DIR_SEP), *rest=NULL;

   *map_fname = *gif_fname = *tmpl_fname = '\0';
   len = strlen(XpmFileName);
   if (psz == NULL) {
      psz = UtilStrRChr(XpmFileName, '.');
      if (psz == NULL) {
         sprintf(gszMsgBox, TgLoadString(STID_FNAME_FORMAT_ERROR_IN_FUNC),
               "GenerateImageMap()");
         FatalUnexpectedError(gszMsgBox, NULL);
         return;
      }
      xpm_ext_len = strlen(psz);
   } else {
      char *psz1=NULL;

      *psz = '\0';
      psz1 = UtilStrRChr(&psz[1], '.');
      if (psz1 == NULL) {
         *psz = DIR_SEP;
         sprintf(gszMsgBox, TgLoadString(STID_FNAME_FORMAT_ERROR_IN_FUNC),
               "GenerateImageMap()");
         FatalUnexpectedError(gszMsgBox, NULL);
         return;
      }
      xpm_ext_len = strlen(psz1);
      *psz = DIR_SEP;
   }
   if (XpmFileName[len-xpm_ext_len] != '.') {
      sprintf(gszMsgBox, TgLoadString(STID_FNAME_FORMAT_ERROR_IN_FUNC),
            "GenerateImageMap()");
      FatalUnexpectedError(gszMsgBox, NULL);
      return;
   }
   XpmFileName[len-xpm_ext_len] = '\0';
   switch (whereToPrint) {
   case GIF_FILE:
      sprintf(map_fname, "%s%s%s", XpmFileName,
            *imageMapFileExtension == '\0' ? "" : ".", imageMapFileExtension);
      sprintf(gif_fname, "%s%s%s", XpmFileName,
            *gifFileExtension == '\0' ? "" : ".", gifFileExtension);
      break;
   case HTML_FILE:
      if (*gszHhtmlExportTemplate != '\0') {
         if (*gszHhtmlExportTemplate == DIR_SEP) {
            strcpy(tmpl_fname, gszHhtmlExportTemplate);
         } else {
            char *psz=UtilStrRChr(XpmFileName, DIR_SEP);

            if (psz == NULL) {
               strcpy(tmpl_fname, gszHhtmlExportTemplate);
            } else {
               *psz = '\0';
               sprintf(tmpl_fname, "%s%c%s", XpmFileName, DIR_SEP,
                     gszHhtmlExportTemplate);
               *psz = DIR_SEP;
            }
         }
      }
      sprintf(map_fname, "%s%s%s", XpmFileName,
            *htmlFileExtension == '\0' ? "" : ".", htmlFileExtension);
      sprintf(gif_fname, "%s%s%s", XpmFileName,
            *gifFileExtension == '\0' ? "" : ".", gifFileExtension);
      break;
   case PNG_FILE:
      *map_fname = '\0';
      sprintf(gif_fname, "%s%s%s", XpmFileName,
            *pngFileExtension == '\0' ? "" : ".", pngFileExtension);
      break;
   case JPEG_FILE:
      *map_fname = '\0';
      sprintf(gif_fname, "%s%s%s", XpmFileName,
            *jpegFileExtension == '\0' ? "" : ".", jpegFileExtension);
      break;
   case XBM_FILE:
      *map_fname = '\0';
      sprintf(gif_fname, "%s.xpm", XpmFileName);
      break;
   }
   XpmFileName[len-xpm_ext_len] = '.';

   if ((short_name=IsPrefix(bootDir, gif_fname, &rest))) ++rest;
   sprintf(gszMsgBox, TgLoadString(STID_CONVERTING_INTO_NAMED_FILE), rest);
   if (PRTGIF && !cmdLineStdOut) {
      if (!cmdLineQuiet) {
         fprintf(stderr, "%s\n", gszMsgBox);
      }
   } else {
      Msg(gszMsgBox);
   }
   if (*tmpl_fname != '\0' && whereToPrint == HTML_FILE &&
         ((tmpl_fp=fopen(tmpl_fname,"r")) == NULL)) {
      sprintf(gszMsgBox, TgLoadString(STID_CANNOT_OPEN_HTML_TMPL_READ),
            tmpl_fname);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return;
   }
   if (!ConvertXpmToGif(XpmFileName, gif_fname, src_is_ppm, whereToPrint)) {
      if (tmpl_fp != NULL) fclose(tmpl_fp);
      return;
   }
   switch (whereToPrint) {
   case GIF_FILE:
      if ((attr_ptr=FindFileAttrWithName("href=")) != NULL) {
         map_fp = OpenMapFile(map_fname);
         if (map_fp != NULL) {
            Msg(TgLoadCachedString(CSTID_GENERATING_IMAGEMAP_FILE_DOTS));
            fprintf(map_fp, "default %s\n", attr_ptr->attr_value.s);
            for (obj_ptr=topObj; obj_ptr != NULL; obj_ptr=obj_ptr->next) {
               if (GenerateObjImageMap(map_fp, obj_ptr, LtX, LtY)) {
                  something_generated = TRUE;
               }
            }
            fclose(map_fp);
            Msg("");
            sprintf(gszMsgBox, TgLoadString(STID_NAMED_IMAGEMAP_FILE_GEN),
                  map_fname);
            Msg(gszMsgBox);
         }
      } else if (generateImageMap) {
         if (nPageNumInFileName) {
            /* surpress the error message if we are print one file per page */
         } else if (something_generated) {
            sprintf(gszMsgBox, TgLoadString(STID_CANT_FND_FATTR_IMGMAP_NOT_GEN),
                  "href=");
            MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
         } else {
            sprintf(gszMsgBox, TgLoadString(STID_CANT_FND_FATTR_NO_IMGMAP_GIF),
                  "href=");
            MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
         }
      }
      break;
   case HTML_FILE:
      map_fp = OpenMapFile(map_fname);
      if (map_fp != NULL) {
         int saved_image_map_file_format=imageMapFileFormat;

         if (tmpl_fp == NULL) {
            Msg(TgLoadCachedString(CSTID_GENERATING_HTML_FILE_DOTS));
            GenerateHtmlHeader(map_fp);
            GenerateUseMapHeader(map_fp, gif_fname, map_fname);
            imageMapFileFormat = IMF_FORMAT_SPYG;
            for (obj_ptr=topObj; obj_ptr != NULL; obj_ptr=obj_ptr->next) {
               if (GenerateObjImageMap(map_fp, obj_ptr, LtX, LtY)) {
                  something_generated = TRUE;
               }
            }
            GenerateUseMapTrailer(map_fp, RbX-LtX, RbY-LtY);
            GenerateHtmlTrailer(map_fp);
         } else {
            sprintf(gszMsgBox,
                  TgLoadCachedString(CSTID_GEN_TEMPLATE_HTML_FILE_DOTS),
                  tmpl_fname);
            Msg(gszMsgBox);
            imageMapFileFormat = IMF_FORMAT_SPYG;
            GenerateTemplateHtmlFile(map_fp, tmpl_fp, gif_fname, map_fname,
                  LtX, LtY, RbX, RbY);
            fclose(tmpl_fp);
         }
         Msg("");
         fclose(map_fp);
         imageMapFileFormat = saved_image_map_file_format;
         sprintf(gszMsgBox, TgLoadString(STID_NAMED_HTML_FILE_GEN),
               map_fname);
         Msg(gszMsgBox);
      }
      break;

   case PNG_FILE: break;
   case JPEG_FILE: break;
   case PPM_FILE: break;
   case XBM_FILE: break;
   }
}

#ifdef NOT_DEFINED
#ifdef _TGIF_DBG /* debug, do not translate */
static
void DebugXPmColors()
{
   int i=0;

   fprintf(stderr, "\nnumColorsToDump = %1d\n", numColorsToDump);
   fprintf(stderr, "maxColors = %1d\n", maxColors);
   fprintf(stderr, "\nThe following indices ranges from 0 to maxColors-1:\n");
   for (i=0; i < maxColors; i++) {
      if (mainVisual->class == TrueColor) {
         fprintf(stderr, "%s[%3d]=%3d, %s[%3d]=0x%06lx\n",
               "colorIndexToDumpIndex", i, colorIndexToDumpIndex[i],
               "colorPixels", i, (long)colorPixels[i]);
      } else {
         fprintf(stderr, "%s[%3d]=%3d, %s[%3d]=%3d\n",
               "colorIndexToDumpIndex", i, colorIndexToDumpIndex[i],
               "colorPixels", i, colorPixels[i]);
      }
   }
   fprintf(stderr, "\n%s 0 to numColorsToDump-1:\n",
         "The following indices ranges from");
   for (i=0; i < numColorsToDump; i++) {
      int j=0;

      if (mainVisual->class == TrueColor) {
         fprintf(stderr, "%s[%3d]=0x%06lx, %s[%3d]=",
               "pixelValue", i, (long)pixelValue[i], "colorChar", i);
      } else {
         fprintf(stderr, "%s[%3d]=%3d, %s[%3d]=",
               "pixelValue", i, pixelValue[i], "colorChar", i);
      }
      for (j = 0; j < charsPerPixel; j++) {
         fprintf(stderr, "%c", colorChar[i*charsPerPixel+j]);
      }
      fprintf(stderr, ", %s[%3d]=%s\n", "colorStr", i, colorStr[i]);
   }
}
#endif /* _TGIF_DBG */

#ifdef _TGIF_DBG /* debug, do not translate */
static
void DebugXPmColorsAndExportImage(image, w, h, left, top, right, bottom)
   XImage *image;
   int w, h, left, top, right, bottom;
{
   int i=0;

   DebugXPmColors();

   fprintf(stderr, "\nHere is the image:\n");
   fprintf(stderr, "    (%s\n    %s\n    %s)\n",
         "recall that if colorIndexToDumpIndex[color_index] = index",
         "then pixelValue[index] == pixel, where pixel is a value seen below",
         "and color_index indexes colorPixels and colorMenuItems");
   for (i=top; i < h-bottom; i++) {
      int j=0;

      for (j=left; j < w-right; j++) {
         int data=XGetPixel(image,j-left,i-top);

         if (j+1 == w-right) {
            if (mainVisual->class == TrueColor) {
               fprintf(stderr, "%06lx\n", (long)data);
            } else {
               fprintf(stderr, "%3d\n", data);
            }
         } else {
            if (mainVisual->class == TrueColor) {
               fprintf(stderr, "%06lx ", (long)data);
            } else {
               fprintf(stderr, "%3d ", data);
            }
         }
      }
   }
}
#endif /* _TGIF_DBG */
#endif /* NOT_DEFINED */

static
void AdjustForRealBaseName(fname, gzipped, no_name)
   char *fname;
   int gzipped, no_name;
{
   char *psz=NULL, *psz1=NULL;

   if (no_name) return;
   if ((psz1=UtilStrRChr(fname, '.')) == NULL) return;
   *psz1 = '\0';
   if (!gzipped) return;
   if ((psz=UtilStrRChr(fname, '.')) == NULL) {
      *psz1 = '.';
      return;
   }
   *psz = '\0';
}

void GetRealBaseName(fname)
   char *fname;
{
   int gzipped=FALSE, no_name=FALSE;

   if (FileNameHasExtension(fname, OBJ_FILE_TYPE, &gzipped, &no_name)) {
      AdjustForRealBaseName(fname, gzipped, no_name);
      return;
   }
   gzipped = no_name = FALSE;
   if (FileNameHasExtension(fname, SYM_FILE_TYPE, &gzipped, &no_name)) {
      AdjustForRealBaseName(fname, gzipped, no_name);
      return;
   }
   gzipped = no_name = FALSE;
   if (FileNameHasExtension(fname, PIN_FILE_TYPE, &gzipped, &no_name)) {
      AdjustForRealBaseName(fname, gzipped, no_name);
      return;
   }
}

static
XImage *RotateImageForLandscape(src_image, p_w, p_h, p_left, p_top, p_right,
      p_bottom)
   XImage *src_image;
   int *p_w, *p_h, *p_left, *p_top, *p_right, *p_bottom;
{
   int x=0, y=0;
   int new_w=(*p_h), new_h=(*p_w), new_left=(*p_top), new_top=(*p_right);
   int new_right=(*p_bottom), new_bottom=(*p_left);
   Pixmap dest_pixmap=None;
   XImage *dest_image=NULL;

   dest_pixmap = XCreatePixmap(mainDisplay, mainWindow, new_w, new_h,
         mainDepth);
   dest_image = (dest_pixmap==None ? NULL : XGetImage(mainDisplay, dest_pixmap,
         0, 0, new_w, new_h, AllPlanes, ZPixmap));
   if (dest_pixmap == None || dest_image == NULL) {
      FailAllocPixmapMessage(new_w, new_h);
      if (dest_pixmap != None) XFreePixmap(mainDisplay, dest_pixmap);
      if (dest_image != NULL) XDestroyImage(dest_image);
      return NULL;
   }
   for (y=0; y < new_w; y++) {
      for (x=0; x < new_h; x++) {
         XPutPixel(dest_image, y, (new_h-1)-x, XGetPixel(src_image, x, y));
      }
   }
   *p_w = new_w;
   *p_h = new_h;
   *p_left = new_left;
   *p_top = new_top;
   *p_right = new_right;
   *p_bottom = new_bottom;

   XFreePixmap(mainDisplay, dest_pixmap);

   return dest_image;
}

static int dumpPpmPreferred=FALSE;

static
void DumpXpmHeader(fp, w, h, name, left, top, right, bottom)
   FILE *fp;
   int w, h, left, top, right, bottom;
   char *name;
{
   if (whereToPrint == PPM_FILE ||
         (whereToPrint != XBM_FILE && dumpPpmPreferred)) {
      fprintf(fp, "P6\n%1d %1d\n255\n", w-left-right, h-top-bottom);
   } else if (xpmOutputVersion == 1) {
      DumpCommentInBitmapExport(fp);
      /* do not translate -- program constants */
      fprintf(fp, "#define %s_format 1\n", name);
      fprintf(fp, "#define %s_width %1d\n", name, w-left-right);
      fprintf(fp, "#define %s_height %1d\n", name, h-top-bottom);
      fprintf(fp, "#define %s_ncolors %1d\n", name, numColorsToDump);
      fprintf(fp, "#define %s_chars_per_pixel %1d\n", name, charsPerPixel);
      if (xpmInXGrabSCFormat) {
         fprintf(fp, "static char * %s_colors[] = {\n", name);
      } else {
         fprintf(fp, "static char *%s_colors[] = {\n", name);
      }
      DumpXPmColors(fp);
      if (xpmInXGrabSCFormat) {
         fprintf(fp, "static char * %s_pixels[] = {\n", name);
      } else {
         fprintf(fp, "static char *%s_pixels[] = {\n", name);
      }
   } else {
      /* xpmOutputVersion is 3 */
      fprintf(fp, "/* XPM */\n");
      DumpCommentInBitmapExport(fp);
      fprintf(fp, "static char * %s[] = {\n", name);
      fprintf(fp, "\"%1d %1d %1d %1d\",\n",
            w-left-right, h-top-bottom, numColorsToDump, charsPerPixel);
      DumpXPmColors(fp);
   }
}

static
void DumpXbmHeader(fp, w, h, name, left, top, right, bottom)
   FILE *fp;
   int w, h, left, top, right, bottom;
   char *name;
{
   if (whereToPrint == PPM_FILE) {
      fprintf(fp, "P4\n%1d %1d\n", w-left-right, h-top-bottom);
   } else {
      DumpCommentInBitmapExport(fp);
      /* do not translate -- program constants */
      fprintf(fp, "#define %s_width %1d\n", name, w-left-right);
      fprintf(fp, "#define %s_height %1d\n", name, h-top-bottom);
      fprintf(fp, "#define %s_x_hot 0\n", name);
      fprintf(fp, "#define %s_y_hot 0\n", name);
      fprintf(fp, "static %schar %s_bits[] = {\n   ",
            (unsignedInXBmExport ? "unsigned " : ""), name);
   }
}

void SetAColorByteInfo(pcb, pXColor)
   ColorBytes *pcb;
   XColor *pXColor;
{
   double red=0, green=0, blue=0;
   int r=0, g=0, b=0;

   red = ((double)255)*((double)(pXColor->red))/((double)maxRGB);
   green = ((double)255)*((double)(pXColor->green))/((double)maxRGB);
   blue = ((double)255)*((double)(pXColor->blue))/((double)maxRGB);

   r = round(red);
   g = round(green);
   b = round(blue);

   if (r > 255) r = 255;
   if (g > 255) g = 255;
   if (b > 255) b = 255;

   if (r < 0) r = 0;
   if (g < 0) g = 0;
   if (b < 0) b = 0;

   pcb->r = (unsigned char)(r & 0x0ff);
   pcb->g = (unsigned char)(g & 0x0ff);
   pcb->b = (unsigned char)(b & 0x0ff);

   pcb->valid = TRUE;
}

static
void SetColorBytes(pcb, index, table_size)
   ColorBytes *pcb;
   int index, table_size;
{
#ifdef _TGIF_DBG /* debug, do not translate */
   TgAssert(index >= 0 && index < table_size,
         "index out of range in SetColorBytes()", NULL);
#endif /* _TGIF_DBG */
   if (pcb[index].valid) return;

   SetAColorByteInfo(&pcb[index], &tgifColors[index]);
}

static
int DumpPpmBody(fp, image, bitmap_image, w, h, left, top, right,
      bottom, nInImageProc, bg_pixel)
   FILE *fp;
   XImage *image, *bitmap_image;
   int w, h, left, top, right, bottom, nInImageProc, bg_pixel;
{
   XColor *saved_tgif_colors=tgifColors;
   ColorBytes *pcb=(ColorBytes*)malloc((maxColors+3)*sizeof(ColorBytes));
   ColorBytes my_bg_cb;
   int i=0, target_percent=5;

   if (pcb == NULL) FailAllocMessage();
   memset(pcb, 0, (maxColors+3)*sizeof(ColorBytes));
   memset(&my_bg_cb, 0, sizeof(ColorBytes));
   if (printUsingRequestedColor) tgifColors = tgifRequestedColors;

   SetAColorByteInfo(&my_bg_cb, &myBgColor);

   for (i=top; i < h-bottom; i++) {
      int j=0, percent=((i-top)*10000/(h-top-bottom))/100;

      if (percent >= target_percent) {
         sprintf(gszMsgBox, TgLoadCachedString(CSTID_PROGRESS_PERCENT),
               percent);
         SetStringStatus(gszMsgBox);
         XSync(mainDisplay, False);
         while (target_percent <= percent) target_percent += 5;
      }
      for (j=left; j < w-right; j++) {
         int transparent=FALSE, data=0;

         if (nInImageProc && bitmap_image != NULL) {
            if (XGetPixel(bitmap_image,j-left,i-top) != 0) {
               data = XGetPixel(image,j-left,i-top);
            } else {
               transparent = TRUE;
            }
         } else {
            data = XGetPixel(image,j-left,i-top);
         }
         if (!nInImageProc && data == bg_pixel) {
            if (fprintf(fp, "%c%c%c",
                  my_bg_cb.r, my_bg_cb.g, my_bg_cb.b) == EOF) {
               writeFileFailed = TRUE;
            }
         } else {
            int index=INVALID, color_index=BAD;

            if (transparent) {
               index = transparentIndex;
            } else { 
               index = XPmLookUp(data, INVALID, NULL, &color_index);
            }
            if (index == INVALID || color_index == BAD ||
                  color_index == INVALID) {
               sprintf(gszMsgBox,
                     TgLoadString(transparent ?
                     STID_UNRECOG_GIVEN_TRPIX_VAL_PRINT :
                     STID_UNRECOG_GIVEN_PIXEL_VAL_PRINT),
                     j-left, i-top, data, (long)data);
               MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);

               if (printUsingRequestedColor) tgifColors = saved_tgif_colors;
               free(pcb);
               return FALSE;
            }
            SetColorBytes(pcb, color_index, (maxColors+3));
            if (fprintf(fp, "%c%c%c", pcb[color_index].r, pcb[color_index].g,
                  pcb[color_index].b) == EOF) {
               writeFileFailed = TRUE;
            }
         }
      }
   }
   if (printUsingRequestedColor) tgifColors = saved_tgif_colors;
   free(pcb);

   return TRUE;
}

static
int DumpXpmBody(fp, image, bitmap_image, w, h, left, top, right,
      bottom, nInImageProc, bg_pixel)
   FILE *fp;
   XImage *image, *bitmap_image;
   int w, h, left, top, right, bottom, nInImageProc, bg_pixel;
{
   int i=0, target_percent=5;

   if (whereToPrint == PPM_FILE ||
         (whereToPrint != XBM_FILE && dumpPpmPreferred)) {
      return DumpPpmBody(fp, image, bitmap_image, w, h, left, top, right,
            bottom, nInImageProc, bg_pixel);
   }
   for (i=top; i < h-bottom; i++) {
      int j=0, percent=((i-top)*10000/(h-top-bottom))/100;

      if (percent >= target_percent) {
         sprintf(gszMsgBox, TgLoadCachedString(CSTID_PROGRESS_PERCENT),
               percent);
         SetStringStatus(gszMsgBox);
         XSync(mainDisplay, False);
         while (target_percent <= percent) target_percent += 5;
      }
      if (fprintf(fp, "\"") == EOF) writeFileFailed = TRUE;
      for (j=left; j < w-right; j++) {
         int transparent=FALSE, data=0;

         if (nInImageProc && bitmap_image != NULL) {
            if (XGetPixel(bitmap_image,j-left,i-top) != 0) {
               data = XGetPixel(image,j-left,i-top);
            } else {
               transparent = TRUE;
            }
         } else {
            data = XGetPixel(image,j-left,i-top);
         }
         if (!nInImageProc && data == bg_pixel) {
            switch (charsPerPixel) {
            case 1:
               if (fprintf(fp, "`") == EOF) writeFileFailed = TRUE;
               break;
            case 2:
               if (fprintf(fp, "``") == EOF) writeFileFailed = TRUE;
               break;
            }
         } else {
            int k=0, index=0;

            if (transparent) {
               index = transparentIndex;
            } else { 
               index = XPmLookUp(data, INVALID, NULL, NULL);
            }
            if (index == INVALID) {
               sprintf(gszMsgBox,
                     TgLoadString(transparent ?
                     STID_UNRECOG_GIVEN_TRPIX_VAL_PRINT :
                     STID_UNRECOG_GIVEN_PIXEL_VAL_PRINT),
                     j-left, i-top, data, (long)data);
               MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);

               return FALSE;
            }
            for (k=0; k < charsPerPixel; k++) {
               if (fprintf(fp, "%c", colorChar[index*charsPerPixel+k]) == EOF) {
                  writeFileFailed = TRUE;
               }
            }
         }
      }
      if (i == h-bottom-1) {
         if (xpmInXGrabSCFormat) {
            if (fprintf(fp, "\",\n} ;\n") == EOF) {
               writeFileFailed = TRUE;
            }
         } else {
            if (fprintf(fp, "\"\n};\n") == EOF) {
               writeFileFailed = TRUE;
            }
         }
      } else if (fprintf(fp, "\",\n") == EOF) {
         writeFileFailed = TRUE;
      }
   }
   return TRUE;
}

#define FS_SCALE 0x400
#define HALF_FS_SCALE 0x200

static
void DumpXbmHalfToneBody(fp, image, w, h, left, top, right, bottom, bg_pixel)
   FILE *fp;
   XImage *image;
   int w, h, left, top, right, bottom, bg_pixel;
{
   int i=0, j=0, k=0, bit_count=0, data=0, pixel=0, byte_count=0;
   TrueColorInfo tci;
   ProgressInfo pi;

   /*--------------------------------------------------*/
   /* the halftoning code is adapted from 'pgmopbm.c', */
   /*       which is part of the pbmplus package.      */
   /*                                                  */
   /*       Copyright (C) 1989 by Jef Poskanzer        */
   /*--------------------------------------------------*/
   long *thiserr, *nexterr, *tmperr, threshval, sum;
   int fs_forward=TRUE, col, limitcol, found_index, *bits;
   float gray;

   sprintf(gszMsgBox,
         TgLoadCachedString(CSTID_FS_HALFTONE_BMP_THRESH_DOTS),
         bitmapThresholdStr);
   Msg(gszMsgBox);

   srand(0);
   thiserr = (long*)malloc((w-left-right+2)*sizeof(long));
   nexterr = (long*)malloc((w-left-right+2)*sizeof(long));
   bits = (int*)malloc((w-left-right+2)*sizeof(int));
   if (thiserr==NULL || nexterr==NULL || bits==NULL) FailAllocMessage();
   for (j = 0; j < w-left-right+2; j++) {
      thiserr[j] = (rand() % FS_SCALE - HALF_FS_SCALE) >> 2;
   }
   threshval = (long)(bitmapThreshold * (float)FS_SCALE);
   if (fullTrueColorMode) {
      if (!InitTrueColorInfo(image, &tci, w)) {
         return;
      }
   }
   byte_count = 0;
   BeginProgress(&pi, h-top-bottom);
   for (i=top; i < h-bottom; i++) {
      UpdateProgress(&pi, i-top);

      for (col = 0; col < w-left-right+2; col++) {
         nexterr[col] = bits[col] = 0;
      }
      bit_count = 0;
      col = (fs_forward) ? 0 : w-left-right-1;
      limitcol = (fs_forward) ? w-left-right : -1;
      do {
         found_index = INVALID;
         pixel = XGetPixel(image, col, i);

         if (fullTrueColorMode) {
            uint32_t pix=(uint32_t)(unsigned int)pixel;
            unsigned int r=0, g=0, b=0;
            double dr=(double)0, dg=(double)0, db=(double)0;

            r = (pix & tci.r_mask) >> tci.r_shift;
            g = (pix & tci.g_mask) >> tci.g_shift;
            b = (pix & tci.b_mask) >> tci.b_shift;
            dr = ((double)r) / tci.dr_maxval;
            dg = ((double)g) / tci.dg_maxval;
            db = ((double)b) / tci.db_maxval;
            gray = (float)(0.299*dr + 0.587*dg + 0.114*db);
         } else {
            for (k = 0; k < maxColors; k++) {
               if (colorPixels[k] == pixel) {
                  found_index = k;
                  break;
               }
            }
            if (found_index == INVALID) {
               if (pixel == bg_pixel) {
                  gray = (float)1.0;
               } else {
                  sprintf(gszMsgBox,
                        TgLoadString(STID_UNRECOG_GIVEN_PIXEL_VAL_1_USE),
                        pixel, (long)pixel);
                  Msg(gszMsgBox);
                  gray = (float)0.0;
               }
            } else {
               gray = 0.299*((float)tgifColors[found_index].red/maxRGB) +
                     0.587*((float)tgifColors[found_index].green/maxRGB) +
                     0.114*((float)tgifColors[found_index].blue/maxRGB);
            }
         }
         sum = ((long)(gray * FS_SCALE)) + thiserr[col+1];
         if (sum >= threshval) {
            sum = sum-threshval-HALF_FS_SCALE;
         } else {
            bits[col] = 1; /* black bit */
         }
         if (fs_forward) {
            thiserr[col+2] += (sum*7)>>4;
            nexterr[col  ] += (sum*3)>>4;
            nexterr[col+1] += (sum*5)>>4;
            nexterr[col+2] += (sum  )>>4;
            col++;
         } else {
            thiserr[col  ] += (sum*7)>>4;
            nexterr[col+2] += (sum*3)>>4;
            nexterr[col+1] += (sum*5)>>4;
            nexterr[col  ] += (sum  )>>4;
            col--;
         }
      } while (col != limitcol);

      tmperr = thiserr;
      thiserr = nexterr;
      nexterr = tmperr;
      fs_forward = !fs_forward;

      bit_count = 0;
      data = 0;

      for (j = left; j < w-right; j++) {
         if (bits[j]) {
            if (whereToPrint == XBM_FILE ||
                  (whereToPrint == EPSI_FILE && generateTiffEPSI)) {
               data |= (1<<bit_count);
            } else {
               data |= (1<<(7-bit_count));
            }
         }
         if (++bit_count == 8) {
            if (byte_count++ == 12) {
               byte_count = 1;
               if (whereToPrint == XBM_FILE ||
                     (whereToPrint == EPSI_FILE && generateTiffEPSI)) {
                  if (fprintf(fp, "\n   ") == EOF) writeFileFailed = TRUE;
               }
            }
            if (whereToPrint == XBM_FILE ||
                  (whereToPrint == EPSI_FILE && generateTiffEPSI)) {
               if (fprintf(fp, "0x%c", hexValue[(data>>4) & 0xf]) == EOF) {
                  writeFileFailed = TRUE;
               }
               if (i == h-bottom-1 && j == w-right-1) {
                  if (fprintf(fp, "%c};\n", hexValue[data & 0xf]) == EOF) {
                     writeFileFailed = TRUE;
                  }
               } else {
                  if (fprintf(fp, "%c, ", hexValue[data & 0xf]) == EOF) {
                     writeFileFailed = TRUE;
                  }
               }
            } else {
               if (fprintf(fp, "%c", (unsigned char)(data & 0x0ff)) == EOF) {
                  writeFileFailed = TRUE;
               }
            }
            bit_count = 0;
            data = 0;
         }
      }
      if (((w-left-right) % 8) != 0) {
         if (byte_count++ == 12) {
            byte_count = 1;
            if (whereToPrint == XBM_FILE ||
                  (whereToPrint == EPSI_FILE && generateTiffEPSI)) {
               if (fprintf(fp, "\n   ") == EOF) writeFileFailed = TRUE;
            }
         }
         if (whereToPrint == XBM_FILE ||
               (whereToPrint == EPSI_FILE && generateTiffEPSI)) {
            for (j=((w-left-right)%8); j < 8; j++) data |= (1<<j);

            if (fprintf(fp, "0x%c", hexValue[(data>>4) & 0xf]) == EOF) {
               writeFileFailed = TRUE;
            }
            if (i == h-bottom-1) {
               if (fprintf(fp, "%c};\n", hexValue[data & 0xf]) == EOF) {
                  writeFileFailed = TRUE;
               }
            } else {
               if (fprintf(fp, "%c, ", hexValue[data & 0xf]) == EOF) {
                  writeFileFailed = TRUE;
               }
            }
         } else {
            if (fprintf(fp, "%c", (unsigned char)(data & 0x0ff)) == EOF) {
               writeFileFailed = TRUE;
            }
         }
      }
   }
}

static
void DumpXbmNoneHalfToneBody(fp, image, w, h, left, top, right, bottom,
      bg_pixel)
   FILE *fp;
   XImage *image;
   int w, h, left, top, right, bottom, bg_pixel;
{
   int i=0, j=0, k=0, bit_count=0, data=0, pixel=0, byte_count=0;
   TrueColorInfo tci;
   ProgressInfo pi;

   if (thresholdBitmap) {
      sprintf(gszMsgBox,
            TgLoadCachedString(CSTID_THRESHOLD_BMP_THRESH_DOTS),
            bitmapThresholdStr);
      Msg(gszMsgBox);
   }
   if (fullTrueColorMode) {
      if (!InitTrueColorInfo(image, &tci, w)) {
         return;
      }
   }
   byte_count = 0;
   BeginProgress(&pi, h-top-bottom);
   for (i=top; i < h-bottom; i++) {
      UpdateProgress(&pi, i-top);

      bit_count = 0;
      data = 0;

      for (j = left; j < w-right; j++) {
         if (thresholdBitmap) {
            pixel = XGetPixel(image, j, i);

            if (fullTrueColorMode) {
               uint32_t pix=(uint32_t)(unsigned int)pixel;
               unsigned int r=0, g=0, b=0;
               double dr=(double)0, dg=(double)0, db=(double)0;
               double dgray=(double)0;

               r = (pix & tci.r_mask) >> tci.r_shift;
               g = (pix & tci.g_mask) >> tci.g_shift;
               b = (pix & tci.b_mask) >> tci.b_shift;
               dr = ((double)r) / tci.dr_maxval;
               dg = ((double)g) / tci.dg_maxval;
               db = ((double)b) / tci.db_maxval;
               dgray = 0.299*dr + 0.587*dg + 0.114*db;
               if (dgray < (double)bitmapThreshold) {
                  if (whereToPrint == XBM_FILE ||
                        (whereToPrint == EPSI_FILE && generateTiffEPSI)) {
                     data |= (1<<bit_count);
                  } else {
                     data |= (1<<(7-bit_count));
                  }
               }
            } else {
               int found_index=INVALID;
               float gray=(float)0;

               for (k = 0; k < maxColors; k++) {
                  if (colorPixels[k] == pixel) {
                     found_index = k;
                     break;
                  }
               }
               if (found_index == INVALID) {
                  if (pixel != bg_pixel) {
                     sprintf(gszMsgBox,
                           TgLoadString(STID_UNRECOG_GIVEN_PIXEL_VAL_1_USE),
                           pixel, (long)pixel);
                     Msg(gszMsgBox);
                     if (whereToPrint == XBM_FILE ||
                           (whereToPrint == EPSI_FILE && generateTiffEPSI)) {
                        data |= (1<<bit_count);
                     } else {
                        data |= (1<<(7-bit_count));
                     }
                     }
               } else {
                  gray = 0.299*((float)tgifColors[found_index].red/maxRGB) +
                        0.587*((float)tgifColors[found_index].green/maxRGB) +
                        0.114*((float)tgifColors[found_index].blue/maxRGB);
                  if (gray < bitmapThreshold) {
                     if (whereToPrint == XBM_FILE ||
                           (whereToPrint == EPSI_FILE && generateTiffEPSI)) {
                        data |= (1<<bit_count);
                     } else {
                        data |= (1<<(7-bit_count));
                     }
                  }
               }
            }
         } else if (XGetPixel(image,j,i) != bg_pixel) {
            if (whereToPrint == XBM_FILE ||
                  (whereToPrint == EPSI_FILE && generateTiffEPSI)) {
               data |= (1<<bit_count);
            } else {
               data |= (1<<(7-bit_count));
            }
         }
         if (++bit_count == 8) {
            if (byte_count++ == 12) {
               byte_count = 1;
               if (whereToPrint == XBM_FILE ||
                     (whereToPrint == EPSI_FILE && generateTiffEPSI)) {
                  if (fprintf(fp, "\n   ") == EOF) writeFileFailed = TRUE;
               }
            }
            if (whereToPrint == XBM_FILE ||
                  (whereToPrint == EPSI_FILE && generateTiffEPSI)) {
               if (fprintf(fp, "0x%c", hexValue[(data>>4) & 0xf]) == EOF) {
                  writeFileFailed = TRUE;
               }
               if (i == h-bottom-1 && j == w-right-1) {
                  if (fprintf(fp, "%c};\n", hexValue[data & 0xf]) == EOF) {
                     writeFileFailed = TRUE;
                  }
               } else {
                  if (fprintf(fp, "%c, ", hexValue[data & 0xf]) == EOF) {
                     writeFileFailed = TRUE;
                  }
               }
            } else {
               if (fprintf(fp, "%c", (unsigned char)(data & 0x0ff)) == EOF) {
                  writeFileFailed = TRUE;
               }
            }
            bit_count = 0;
            data = 0;
         }
      }
      if (((w-left-right) % 8) != 0) {
         if (byte_count++ == 12) {
            byte_count = 1;
            if (whereToPrint == XBM_FILE ||
                  (whereToPrint == EPSI_FILE && generateTiffEPSI)) {
               if (fprintf(fp, "\n   ") == EOF) writeFileFailed = TRUE;
            }
         }
         if (whereToPrint == XBM_FILE ||
               (whereToPrint == EPSI_FILE && generateTiffEPSI)) {
            for (j=((w-left-right)%8); j < 8; j++) data |= (1<<j);

            if (fprintf(fp, "0x%c", hexValue[(data>>4) & 0xf]) == EOF) {
               writeFileFailed = TRUE;
            }
            if (i == h-bottom-1) {
               if (fprintf(fp, "%c};\n", hexValue[data & 0xf]) == EOF) {
                  writeFileFailed = TRUE;
               }
            } else {
               if (fprintf(fp, "%c, ", hexValue[data & 0xf]) == EOF) {
                  writeFileFailed = TRUE;
               }
            }
         } else {
            if (fprintf(fp, "%c", (unsigned char)(data & 0x0ff)) == EOF) {
               writeFileFailed = TRUE;
            }
         }
      }
   }
}

static
void DumpXbmBody(fp, image, w, h, left, top, right, bottom, bg_pixel)
   FILE *fp;
   XImage *image;
   int w, h, left, top, right, bottom, bg_pixel;
{
   if (halfToneBitmap) {
      DumpXbmHalfToneBody(fp, image, w, h, left, top, right, bottom, bg_pixel);
   } else {
      DumpXbmNoneHalfToneBody(fp, image, w, h, left, top, right, bottom,
            bg_pixel);
   }
}

static
void GetPixelTrims(w, h, pnLeft, pnTop, pnRight, pnBottom)
   int w, h, *pnLeft, *pnTop, *pnRight, *pnBottom;
{
   *pnLeft = leftExportPixelTrim;
   *pnTop = topExportPixelTrim;
   *pnRight = rightExportPixelTrim;
   *pnBottom = bottomExportPixelTrim;
   if (leftExportPixelTrim+rightExportPixelTrim >= w ||
         topExportPixelTrim+bottomExportPixelTrim >= h) {
      SetExportPixelTrim(TRUE);
      if (leftExportPixelTrim+rightExportPixelTrim >= w ||
            topExportPixelTrim+bottomExportPixelTrim >= h) {
         sprintf(gszMsgBox, TgLoadString(STID_LARGE_EXP_PXL_TRM_WH_0_USED),
               leftExportPixelTrim, topExportPixelTrim, rightExportPixelTrim,
               bottomExportPixelTrim, w, h);
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
         *pnLeft = *pnTop = *pnRight = *pnBottom = 0;
      } else {
         *pnLeft = leftExportPixelTrim;
         *pnTop = topExportPixelTrim;
         *pnRight = rightExportPixelTrim;
         *pnBottom = bottomExportPixelTrim;
      }
   }
}

static
void BuildXPmColorsFromImage(image, w, h)
   XImage *image;
   int w, h;
{
   int i=0, saved_max_colors=maxColors;

   FreeCachedStrings();

   maxColors = MAXXPMEXPORTCOLORS;

   pixelValue = (int*)malloc((maxColors+3)*sizeof(int));
   if (pixelValue == NULL) FailAllocMessage();
   colorIndexToDumpIndex = (int*)malloc((maxColors+3)*sizeof(int));
   dumpIndexToColorIndex = (int*)malloc((maxColors+3)*sizeof(int));
   if (colorIndexToDumpIndex == NULL || dumpIndexToColorIndex == NULL) {
      FailAllocMessage();
   }
   if (maxColors > 20) {
      charsPerPixel = 2;
      colorChar = (char*)malloc(((maxColors<<1)+6)*sizeof(char));
   } else {
      charsPerPixel = 1;
      colorChar = (char*)malloc((maxColors+3)*sizeof(char));
   }
   if (colorChar == NULL) FailAllocMessage();
   colorStr = (char**)malloc((maxColors+3)*sizeof(char*));
   if (colorStr == NULL) FailAllocMessage();
   memset(colorStr, 0, (maxColors+3)*sizeof(char*));

   for (i = 0; i < maxColors+3; i++) {
      colorIndexToDumpIndex[i] = dumpIndexToColorIndex[i] = INVALID;
   }
   pixelValue[0] = GetDrawingBgPixel(INVALID, INVALID);
   if (myFileBgColorStr == NULL) {
      colorStr[0] = (char*)malloc((strlen(myBgColorStr)+1)*sizeof(char));
      if (colorStr[0] == NULL) FailAllocMessage();
      strcpy(colorStr[0], myBgColorStr);
   } else {
      colorStr[0] = (char*)malloc((strlen(myFileBgColorStr)+1)*sizeof(char));
      if (colorStr[0] == NULL) FailAllocMessage();
      strcpy(colorStr[0], myFileBgColorStr);
   }
   numColorsToDump = 1;

   if (InitTmpBuckets()) {
      SetStringStatus(TgLoadCachedString(CSTID_BLD_COLOR_TBL_FROM_PIXEL_DOTS));
      for (i=0; i < h; i++) {
         int j=0;

         for (j=0; j < w; j++) {
            int pixel=XGetPixel(image,j,i);

            UpdateColorsLookupTableForPixel(pixel, TRUE);
         }
      }
      CleanUpTmpBuckets();
   }
   colorChar[0] = '`';
   if (charsPerPixel > 1) colorChar[1] = '`';
   if (numColorsToDump >= 256) {
      for (i=1; i < numColorsToDump; i++) {
         if (charsPerPixel == 1) {
            colorChar[i] = (char)(((int)('a'))+i-1);
         } else {
            int left=(int)(i/80), right=(i%80);

            if (left >= 31) {
               colorChar[i*2] = (char)(((int)('/'))+left-31);
            } else {
               colorChar[i*2] = (char)(((int)('`'))+left);
            }
            if (right >= 31) {
               colorChar[i*2+1] = (char)(((int)('/'))+right-31);
            } else {
               colorChar[i*2+1] = (char)(((int)('`'))+right);
            }
         }
      }
   } else {
      for (i=1; i < numColorsToDump; i++) {
         if (charsPerPixel == 1) {
            colorChar[i] = (char)(((int)('a'))+i-1);
         } else {
            colorChar[i*2] = (char)(((int)('a'))+(int)(i/10));
            colorChar[i*2+1] = (char)(((int)('0'))+(i%10));
         }
      }
   }
   maxColors = saved_max_colors;
}

static
int DumpXImageToXpmFile(image, w, h, fname)
   XImage *image;
   int w, h;
   char *fname;
{
   FILE *fp=fopen(fname, "w");
   int bg_pixel=GetDrawingBgPixel(INVALID, INVALID);
   int saved_xpm_output_version=xpmOutputVersion;

   BuildXPmColorsFromImage(image, w, h);

   if (numColorsToDump >= MAXXPMEXPORTCOLORS) {
      sprintf(gszMsgBox, TgLoadString(STID_TOO_MANY_COLORS_LIMIT_IS),
            numColorsToDump, MAXXPMEXPORTCOLORS);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return FALSE;
   }
   if (fp == NULL) {
      sprintf(gszMsgBox, TgLoadString(STID_CANNOT_OPEN_FILE_FOR_WRITING),
            fname);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return FALSE;
   }
   if (useXPmVersion1ForImageMap) {
      xpmOutputVersion = 1;
   }
   writeFileFailed = FALSE;

   DumpXpmHeader(fp, w, h, TOOL_NAME, 0, 0, 0, 0);

#ifdef NOT_DEFINED
#ifdef _TGIF_DBG
   DebugXPmColorsAndExportImage(image, w, h, 0, 0, 0, 0);
#endif /* _TGIF_DBG */
#endif /* NOT_DEFINED */
   if (!BuildXPmBuckets(numColorsToDump, pixelValue, dumpIndexToColorIndex,
         INVALID, NULL, NULL)) {
      fclose(fp);
      xpmOutputVersion = saved_xpm_output_version;
      return FALSE;
   }
   if (!DumpXpmBody(fp, image, NULL, w, h, 0, 0, 0, 0, FALSE, bg_pixel)) {
      if (!(PRTGIF && cmdLineStdOut &&
            !(whereToPrint == EPSI_FILE && generateTiffEPSI) &&
            !preDumpSetup)) {
         fclose(fp);
      }
      xpmOutputVersion = saved_xpm_output_version;
      return FALSE;
   }
   fclose(fp);
   xpmOutputVersion = saved_xpm_output_version;
   if (writeFileFailed) {
      return FailAllocPixmapMessage(w, h);
   }
   return TRUE;
}

int DumpXImageToPpmFile(image, w, h, fname, deflate)
   XImage *image;
   int w, h, deflate;
   char *fname;
{
   int row=0, col=0;
   TrueColorInfo tci;
   double dmaxval=(double)255;
   unsigned int bytes_per_pixel=image->bits_per_pixel/8;
   unsigned char *image_ptr=(unsigned char *)(image->data);
   char deflated_fname[MAXPATHLENGTH];
   FILE *fp=NULL;
   ProgressInfo pi;

   *deflated_fname = '\0';
   if (!InitTrueColorInfo(image, &tci, w)) {
      return FALSE;
   }
   fp = fopen(fname, "w");
   if (fp == NULL) {
      sprintf(gszMsgBox, TgLoadString(STID_CANNOT_OPEN_FILE_FOR_WRITING),
            fname);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return FALSE;
   }
   writeFileFailed = FALSE;
   if (fprintf(fp, "P6\n%1d %1d\n255\n", w, h) == EOF) writeFileFailed = TRUE;

   BeginProgress(&pi, h);
   for (row=0; row < h; row++) {
      UpdateProgress(&pi, row);
      for (col=0; col < w; col++, image_ptr+=bytes_per_pixel) {
         uint32_t pixel=0;
         unsigned int r=0, g=0, b=0;
         double dr=(double)0, dg=(double)0, db=(double)0;
         unsigned char buf[3];

         memcpy(&pixel, image_ptr, bytes_per_pixel);
         r = (pixel & tci.r_mask) >> tci.r_shift;
         g = (pixel & tci.g_mask) >> tci.g_shift;
         b = (pixel & tci.b_mask) >> tci.b_shift;
         dr = ((double)r) * dmaxval / tci.dr_maxval;
         dg = ((double)g) * dmaxval / tci.dg_maxval;
         db = ((double)b) * dmaxval / tci.db_maxval;
         r = round(dr);
         g = round(dg);
         b = round(db);
         buf[0] = (unsigned char)(r&0xff);
         buf[1] = (unsigned char)(g&0xff);
         buf[2] = (unsigned char)(b&0xff);

         if ((int)fwrite(buf, sizeof(char), 3, fp) <= 0) writeFileFailed = TRUE;
      }
   }
   EndProgress(&pi);
   fclose(fp);
   if (writeFileFailed) {
      return FailToWriteFileMessage(fname);
   }
   if (deflate) {
      snprintf(deflated_fname, sizeof(deflated_fname), "%s.ppm.z", fname);
      if (!DeflateFile(fname, deflated_fname)) {
         unlink(fname);
         return FALSE;
      }
      unlink(fname);
   }
   return TRUE;
}

static
int DumpXImageToPpm6(fp, image, bitmap_image, w, h, left, top, right, bottom)
   FILE *fp;
   XImage *image, *bitmap_image;
   int w, h, left, top, right, bottom;
{
   int i=0;
   TrueColorInfo tci;
   double dmaxval=(double)255;
   unsigned int bytes_per_pixel=image->bits_per_pixel/8;
   unsigned char *image_ptr=(unsigned char *)(image->data);
   ProgressInfo pi;

   if (!InitTrueColorInfo(image, &tci, w)) {
      return FALSE;
   }
   if (fprintf(fp, "P6\n%1d %1d\n255\n", w, h) == EOF) writeFileFailed = TRUE;

   image_ptr += (bytes_per_pixel*top*w);
   BeginProgress(&pi, h-top-bottom);
   for (i=top; i < h-bottom; i++, image_ptr+=bytes_per_pixel*right) {
      int j=0;

      UpdateProgress(&pi, i-top);
      for (j=left, image_ptr+=(bytes_per_pixel*left); j < w-right; j++,
            image_ptr+=bytes_per_pixel) {
         uint32_t pixel=0;
         unsigned int r=0, g=0, b=0;
         double dr=(double)0, dg=(double)0, db=(double)0;
         unsigned char buf[3];

         memcpy(&pixel, image_ptr, bytes_per_pixel);
         r = (pixel & tci.r_mask) >> tci.r_shift;
         g = (pixel & tci.g_mask) >> tci.g_shift;
         b = (pixel & tci.b_mask) >> tci.b_shift;
         dr = ((double)r) * dmaxval / tci.dr_maxval;
         dg = ((double)g) * dmaxval / tci.dg_maxval;
         db = ((double)b) * dmaxval / tci.db_maxval;
         r = round(dr);
         g = round(dg);
         b = round(db);
         buf[0] = (unsigned char)(r&0xff);
         buf[1] = (unsigned char)(g&0xff);
         buf[2] = (unsigned char)(b&0xff);

         if ((int)fwrite(buf, sizeof(char), 3, fp) <= 0) writeFileFailed = TRUE;
      }
   }
   EndProgress(&pi);

   return TRUE;
}

void DumpXBitmapFile(nInImageProc, nDumpPpmPreferred, nPageNumInFileName)
   int nInImageProc, nDumpPpmPreferred, nPageNumInFileName;
{  /* called when printing in the xbm or xpm format */
   int ltx=0, lty=0, w=0, h=0, short_name=FALSE;
   int left=0, top=0, right=0, bottom=0;
   int bg_pixel=GetDrawingBgPixel(INVALID, INVALID);
   char xbm_file_name[MAXPATHLENGTH<<1], *rest=NULL, name[MAXPATHLENGTH+1];
   FILE *fp=NULL;
   Pixmap pixmap=None;
   XImage *image=NULL, *bitmap_image=NULL;

   dumpPpmPreferred = nDumpPpmPreferred;

   if (nInImageProc) {
      *gszImageProcXPmFile = '\0';
      if (MkTempFile(xbm_file_name, sizeof(xbm_file_name), tmpDir,
            TOOL_NAME) == NULL) {
         return;
      }
      strcpy(name, "unnamed");
      if ((short_name=IsPrefix(bootDir, xbm_file_name, &rest))) ++rest;
   } else {
      strcpy(name, curFileName);
      GetRealBaseName(name);
      sprintf(xbm_file_name, "%s%c%s", curDir, DIR_SEP, name);
      if (PRTGIF) {
         char *tmp_name=(char*)name;

         if (whereToPrint == PPM_FILE || dumpPpmPreferred) {
            SetOutputFileName(xbm_file_name,
                  colorDump?"ppm":"pbm", NULL, &tmp_name);
         } else {
            SetOutputFileName(xbm_file_name,
                  colorDump?XPM_FILE_EXT:XBM_FILE_EXT, NULL, &tmp_name);
         }
      } else {
         if (whereToPrint == PPM_FILE || dumpPpmPreferred) {
            SetOutputFileName(xbm_file_name,
                  colorDump?"ppm":"pbm", &short_name, &rest);
         } else {
            SetOutputFileName(xbm_file_name,
                  colorDump?XPM_FILE_EXT:XBM_FILE_EXT, &short_name, &rest);
         }
      }
   }
   if (PRTGIF && cmdLineStdOut &&
         !(whereToPrint == EPSI_FILE && generateTiffEPSI) && !preDumpSetup) {
      fp = stdout;
   } else if ((fp=fopen(xbm_file_name, "w")) == NULL) {
      sprintf(gszMsgBox, TgLoadString(STID_CANNOT_OPEN_FOR_WRITE_PRINT),
            (short_name ? rest : xbm_file_name));
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return;
   }
   if (colorDump) InitImageMap();
   Msg(TgLoadCachedString(CSTID_GENERATING_IMAGE));

   /* if we are print one page per file, don't redraw */
   if ((pixmap=DrawAllOnPixmap(&ltx, &lty, &w, &h, !nPageNumInFileName)) ==
         None) {
      if (!(PRTGIF && cmdLineStdOut &&
            !(whereToPrint == EPSI_FILE && generateTiffEPSI) &&
            !preDumpSetup)) {
         fclose(fp);
         unlink(xbm_file_name);
      }
      return;
   }

   sprintf(gszMsgBox, TgLoadString(STID_PRINTING_INTO_NAMED_FILE),
         (short_name ? rest : xbm_file_name));
   Msg(gszMsgBox);

   GetPixelTrims(w, h, &left, &top, &right, &bottom);

   if ((image=XGetImage(mainDisplay, pixmap, left, top,
         w-left-right, h-top-bottom, AllPlanes, ZPixmap)) == NULL) {
      MsgBox(TgLoadString(STID_XGETIMAGE_MAY_RUN_OUT_VMEM), TOOL_NAME, INFO_MB);
      if (!(PRTGIF && cmdLineStdOut &&
            !(whereToPrint == EPSI_FILE && generateTiffEPSI) &&
            !preDumpSetup)) {
         fclose(fp);
         unlink(xbm_file_name);
      }
      XFreePixmap(mainDisplay, pixmap);
      return;
   }
   if (nInImageProc && topSel != NULL && topSel == botSel &&
         topSel->obj->type == OBJ_XPM) {
/*    if ((bitmap_image=XGetImage(mainDisplay,
            topSel->obj->detail.xpm->bitmap, left, top,
            w-left-right, h-top-bottom, 1, ZPixmap)) == NULL) {
         Msg("Cannot generate image, print aborted!");
         if (!(PRTGIF && cmdLineStdOut &&
               !(whereToPrint == EPSI_FILE && generateTiffEPSI) &&
               !preDumpSetup)) {
            fclose(fp);
            unlink(xbm_file_name);
         }
         XDestroyImage(image);
         XFreePixmap(mainDisplay, pixmap);
         return;
      } */
   }
   if (left != 0 || top != 0 || right != 0 || bottom != 0) {
      sprintf(gszMsgBox, TgLoadString(STID_APPLY_EXP_PXL_TRM_VALS),
            left, top, right, bottom);
      Msg(gszMsgBox);
   }
   if (whereToPrint == EPSI_FILE && generateTiffEPSI &&
         pageStyle == LANDSCAPE) {
      /* for landscape mode TiffEPSI, we need to rotate the image */
      XImage *rotated_image=RotateImageForLandscape(image, &w, &h, &left, &top,
            &right, &bottom);

      if (rotated_image == NULL) {
         MsgBox(TgLoadString(STID_XGETIMAGE_MAY_RUN_OUT_VMEM), TOOL_NAME,
               INFO_MB);
         if (!(PRTGIF && cmdLineStdOut &&
               !(whereToPrint == EPSI_FILE && generateTiffEPSI) &&
               !preDumpSetup)) {
            fclose(fp);
            unlink(xbm_file_name);
         }
         if (bitmap_image != NULL) XDestroyImage(bitmap_image);
         XDestroyImage(image);
         XFreePixmap(mainDisplay, pixmap);
         return;
      }
      XDestroyImage(image);
      image = rotated_image;
   }
   writeFileFailed = FALSE;
   if (colorDump && nInImageProc && gnConvolving) {
#ifdef _TGIF_DBG /* debug, do not translate */
      fprintf(stderr,
            "Warning (unexpected): gnConvolving is TRUE in DumpXBitmapFile.\n");
#endif /* _TGIF_DBG */
      if (!DoConvolution(fp, image, bitmap_image, w, h, NULL)) {
         if (!(PRTGIF && cmdLineStdOut &&
               !(whereToPrint == EPSI_FILE && generateTiffEPSI) &&
               !preDumpSetup)) {
            fclose(fp);
            unlink(xbm_file_name);
         }
         if (bitmap_image != NULL) XDestroyImage(bitmap_image);
         XDestroyImage(image);
         XFreePixmap(mainDisplay, pixmap);
         return;
      }
   } else if (colorDump) {
      if (dumpPpmPreferred) {
         /* don't do anything here, do it below */
      } else {
         int saved_xpm_output_version=xpmOutputVersion;

         if (whereToPrint==GIF_FILE || whereToPrint==HTML_FILE ||
               whereToPrint==PNG_FILE || whereToPrint==JPEG_FILE ||
               whereToPrint==PPM_FILE) {
            if (useXPmVersion1ForImageMap) {
               xpmOutputVersion = 1;
            }
         }
         BuildXPmColors(image, w, h, left, top, right, bottom, FALSE);

         if (numColorsToDump >= MAXXPMEXPORTCOLORS) {
            sprintf(gszMsgBox, TgLoadString(STID_TOO_MANY_COLORS_LIMIT_IS),
                  numColorsToDump, MAXXPMEXPORTCOLORS);
            if (PRTGIF) {
               fprintf(stderr, "%s\n", gszMsgBox);
            } else {
               MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
            }
            if (!(PRTGIF && cmdLineStdOut &&
                  !(whereToPrint == EPSI_FILE && generateTiffEPSI) &&
                  !preDumpSetup)) {
               fclose(fp);
               unlink(xbm_file_name);
            }
            if (bitmap_image != NULL) XDestroyImage(bitmap_image);
            XDestroyImage(image);
            XFreePixmap(mainDisplay, pixmap);
            return;
         }
         DumpXpmHeader(fp, w, h, name, left, top, right, bottom);

         xpmOutputVersion = saved_xpm_output_version;
      }
   } else {
      DumpXbmHeader(fp, w, h, name, left, top, right, bottom);
   }
   SaveStatusStrings();
   if (colorDump && nInImageProc && gnConvolving) {
   } else if (colorDump) {
      if (dumpPpmPreferred) {
         if (!DumpXImageToPpm6(fp, image, bitmap_image, w, h, left, top,
               right, bottom)) {
            if (!(PRTGIF && cmdLineStdOut &&
                  !(whereToPrint == EPSI_FILE && generateTiffEPSI) &&
                  !preDumpSetup)) {
               fclose(fp);
            }
            if (!(PRTGIF && cmdLineStdOut &&
                  !(whereToPrint == EPSI_FILE && generateTiffEPSI) &&
                  !preDumpSetup)) {
               unlink(xbm_file_name);
            }
            if (bitmap_image != NULL) XDestroyImage(bitmap_image);
            XDestroyImage(image);
            XFreePixmap(mainDisplay, pixmap);

            return;
         }
      } else {
#ifdef NOT_DEFINED
#ifdef _TGIF_DBG
         DebugXPmColorsAndExportImage(image, w, h, left, top, right, bottom);
#endif /* _TGIF_DBG */
#endif /* NOT_DEFINED */
         if (!BuildXPmBuckets(numColorsToDump, pixelValue,
               dumpIndexToColorIndex, INVALID, NULL, NULL)) {
            if (!(PRTGIF && cmdLineStdOut && !(whereToPrint == EPSI_FILE &&
                  generateTiffEPSI) && !preDumpSetup)) {
               fclose(fp);
               unlink(xbm_file_name);
            }
            if (bitmap_image != NULL) XDestroyImage(bitmap_image);
            XDestroyImage(image);
            XFreePixmap(mainDisplay, pixmap);
            return;
         }
         if (!DumpXpmBody(fp, image, bitmap_image, w, h, left, top, right,
               bottom, nInImageProc, bg_pixel)) {
            if (!(PRTGIF && cmdLineStdOut &&
                  !(whereToPrint == EPSI_FILE && generateTiffEPSI) &&
                  !preDumpSetup)) {
               fclose(fp);
            }
            if (!(PRTGIF && cmdLineStdOut &&
                  !(whereToPrint == EPSI_FILE && generateTiffEPSI) &&
                  !preDumpSetup)) {
               unlink(xbm_file_name);
            }
            if (bitmap_image != NULL) XDestroyImage(bitmap_image);
            XDestroyImage(image);
            XFreePixmap(mainDisplay, pixmap);

            return;
         }
      }
   } else {
      DumpXbmBody(fp, image, w, h, left, top, right, bottom, bg_pixel);
   }
   RestoreStatusStrings();
   if (!(PRTGIF && cmdLineStdOut &&
         !(whereToPrint == EPSI_FILE && generateTiffEPSI) && !preDumpSetup)) {
      fclose(fp);
   }
   XDestroyImage(image);
   XFreePixmap(mainDisplay, pixmap);

   if (writeFileFailed) {
      writeFileFailed = FALSE;
      FailToWriteFileMessage(xbm_file_name);
   } else {
      if (whereToPrint == PPM_FILE || dumpPpmPreferred) {
         sprintf(gszMsgBox, TgLoadString(STID_FORMAT_FILE_WH_PRINTED_INTO),
               (colorDump ? "PPM" : "PBM"), w-left-right, h-top-bottom,
               (short_name ? rest : xbm_file_name));
      } else {
         sprintf(gszMsgBox, TgLoadString(STID_FORMAT_FILE_WH_PRINTED_INTO),
               (colorDump ? "XPM" : "XBM"), w-left-right, h-top-bottom,
               (short_name ? rest : xbm_file_name));
      }
      if (PRTGIF && !cmdLineStdOut) {
         if (!cmdLineQuiet) {
            fprintf(stderr, "%s\n", gszMsgBox);
         }
      } else {
         Msg(gszMsgBox);
      }
      if (colorDump && (whereToPrint==GIF_FILE || whereToPrint==HTML_FILE ||
            whereToPrint==PNG_FILE || whereToPrint==JPEG_FILE ||
            (dumpPpmPreferred && whereToPrint==XBM_FILE))) {
         int src_is_ppm=dumpPpmPreferred;

         SaveStatusStrings();
         GenerateImageMap(xbm_file_name, src_is_ppm, ltx+left, lty+top,
               ltx+w-right, lty+h-bottom, nPageNumInFileName);
         RestoreStatusStrings();
         if (!(PRTGIF && cmdLineStdOut &&
               !(whereToPrint == EPSI_FILE && generateTiffEPSI) &&
               !preDumpSetup)) {
            unlink(xbm_file_name);
         }
      }
   }
   if (nInImageProc) {
      strcpy(gszImageProcXPmFile, xbm_file_name);
      if (gnConvolving) {
         CleanUpConvolution();
      }
   }
   if (bitmap_image != NULL) XDestroyImage(bitmap_image);
}

int DumpXImageToFile(image, w, h, fname, ext)
   XImage *image;
   int w, h;
   char *fname, *ext;
{
   if (fullTrueColorMode && HasZlibSupport()) {
      if (DumpXImageToPpmFile(image, w, h, fname, TRUE)) {
         if (ext != NULL) strcpy(ext, ".ppm.z");
         return TRUE;
      }
   }
   if (ext != NULL) strcpy(ext, ".xpm");

   return DumpXImageToXpmFile(image, w, h, fname);
}

static
void FailedExportXPixmapDeckToGIF(count, gif_fp, gif_fname, tmp_fnames)
   int count;
   FILE *gif_fp;
   char *gif_fname, *tmp_fnames;
{
   if (gif_fp != NULL) fclose(gif_fp);
   if (*gif_fname != '\0') unlink(gif_fname);
   if (tmp_fnames != NULL) {
      int i=0, index=0;

      for (i=0, index=0; i < count; i++, index += MAXPATHLENGTH) {
         char *psz=NULL;

         if (tmp_fnames[index] == '\0') {
            break;
         }
         unlink(&tmp_fnames[index]);
         if ((psz=UtilStrRChr(&tmp_fnames[index], '.')) != NULL) {
            *psz = '\0';
            unlink(&tmp_fnames[index]);
         }
      }
      free(tmp_fnames);
   }
}

void ExportXPixmapDeckToGIF()
{
   struct ObjRec *obj_ptr=NULL, *xpm_obj=NULL;
   struct ObjRec *saved_top_obj=topObj, *saved_bot_obj=botObj;
   int i=0, first_one=TRUE, w=(-1), h=(-1), count=0, index=0, len=0, ok=TRUE;
   int short_name=FALSE, bytes_read=0, watch_cursor=watchCursorOnMainWindow;
   char *tmp_fnames=NULL, *cmd=NULL, *rest=NULL, gif_fname[MAXPATHLENGTH];
   char buf[1024];
   FILE *gif_fp=NULL, *pfp=NULL;

   if (!curDirIsLocal) {
      MsgBox(TgLoadString(STID_CANNOT_PRINT_REMOTE_FILE), TOOL_NAME, INFO_MB);
      return;
   } else if (!curFileDefined) {
      sprintf(gszMsgBox, TgLoadString(STID_NO_CUR_FILE_CANNOT_GEN_FORMAT),
            "GIF"),
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return;
   }
   if (topSel == NULL || topSel != botSel) {
      MsgBox(TgLoadString(STID_SEL_XPM_DECK_TO_EXPORT), TOOL_NAME, INFO_MB);
      return;
   }
   obj_ptr = topSel->obj;
   if (obj_ptr->type != OBJ_GROUP && obj_ptr->type != OBJ_SYM &&
         obj_ptr->type != OBJ_ICON) {
      MsgBox(TgLoadString(STID_SEL_XPM_DECK_TO_EXPORT), TOOL_NAME, INFO_MB);
      return;
   }
   for (xpm_obj=obj_ptr->detail.r->first; xpm_obj != NULL;
         xpm_obj=xpm_obj->next) {
      if (xpm_obj->type == OBJ_XPM) {
         if (first_one) {
            w = xpm_obj->obbox.rbx-xpm_obj->obbox.ltx;
            h = xpm_obj->obbox.rby-xpm_obj->obbox.lty;
            first_one = FALSE;
         } else {
            if (w != xpm_obj->obbox.rbx-xpm_obj->obbox.ltx ||
                  h != xpm_obj->obbox.rby-xpm_obj->obbox.lty) {
               sprintf(gszMsgBox,
                     TgLoadString(STID_MANY_SZ_FAIL_XPM_DESK_EXPORT), w, h);
               MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
               return;
            }
         }
         count++;
      }
   }
   tmp_fnames = (char*)malloc(count*MAXPATHLENGTH*sizeof(char));
   if (tmp_fnames == NULL) FailAllocMessage();

   curPage->top = curPage->bot = topObj = botObj = NULL;

   SaveStatusStrings();
   HighLightReverse();
   RemoveAllSel();
   PushPageInfo();

   InitImageMap();

   SetWatchCursor(drawWindow);
   SetWatchCursor(mainWindow);

   len = strlen(xpmDeckToGifAnimCmd)+5;
   for (xpm_obj=obj_ptr->detail.r->first; xpm_obj != NULL;
         xpm_obj=xpm_obj->next) {
      if (xpm_obj->type == OBJ_XPM) {
         int saved_colordump=colorDump, saved_wheretoprint=whereToPrint;
         int saved_xpm_output_version=xpmOutputVersion;
         int dump_ppm_preferred=FALSE;

         curPage->top = curPage->bot = topObj = botObj = DupObj(xpm_obj);
         topSel = botSel = SelectThisObject(topObj);
         UpdSelBBox();

         if (useXPmVersion1ForXPmDeck) {
            xpmOutputVersion = 1;
         }
         *gszImageProcXPmFile = '\0';
         gnConvolving = FALSE;

         colorDump = TRUE;
         whereToPrint = XBM_FILE;
         gnInImageProc = TRUE;

         dump_ppm_preferred = fullTrueColorMode &&
               (whereToPrint==XBM_FILE && xpmOutputVersion == 3);
         DumpXBitmapFile(gnInImageProc, dump_ppm_preferred, FALSE);

         RemoveAllSel();
         DelAllObj();
         gnInImageProc = FALSE;
         whereToPrint = saved_wheretoprint;
         colorDump = saved_colordump;
         xpmOutputVersion = saved_xpm_output_version;
         if (*gszImageProcXPmFile == '\0') {
            break;
         }
         sprintf(&tmp_fnames[index], "%s.gif", gszImageProcXPmFile);
         if (!ConvertXpmToGif(gszImageProcXPmFile, &tmp_fnames[index], FALSE,
               GIF_FILE)) {
            unlink(gszImageProcXPmFile);
            break;
         }
         len += strlen(&tmp_fnames[index])+1;

         index += MAXPATHLENGTH;
      }
   }
   SetDefaultCursor(mainWindow);
   ShowCursor(); 

   *gif_fname = '\0';
   SetOutputFileName(gif_fname, GetExportExt(GIF_FILE), &short_name, &rest);
   gif_fp = fopen(gif_fname,"w");
   if (gif_fp == NULL) {
      sprintf(gszMsgBox, TgLoadString(STID_CANNOT_OPEN_FILE_FOR_WRITING),
            ((short_name && *outputDir=='\0') ? rest : gif_fname));
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      FailedExportXPixmapDeckToGIF(count, gif_fp, gif_fname, tmp_fnames);
      ok = FALSE;
   }
   if (ok) {
      cmd = (char*)malloc(len+1);
      strcpy(cmd, xpmDeckToGifAnimCmd);
      len = strlen(cmd);
      for (i=0, index=0; i < count; i++, index += MAXPATHLENGTH) {
         if (tmp_fnames[index] == '\0') {
            break;
         }
         sprintf(&cmd[len], " %s", &tmp_fnames[index]);
         len += strlen(&cmd[len]);
      }
      if (FindProgramInPath(cmd, NULL, FALSE)) {
         sprintf(gszMsgBox, TgLoadCachedString(CSTID_EXECUTING_GIVEN_PROGRAM),
               xpmDeckToGifAnimCmd);
         SetStringStatus(gszMsgBox);
         XSync(mainDisplay, False);
         if ((pfp=(FILE*)popen(cmd, "r")) == NULL) {
            sprintf(gszMsgBox, TgLoadString(STID_CANNOT_EXEC_FORMAT_NOT_GEN),
                  cmd, "GIF");
            MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
            FailedExportXPixmapDeckToGIF(count, gif_fp, gif_fname, tmp_fnames);
            ok = FALSE;
         } else {
            if (!watch_cursor) {
               SetWatchCursor(drawWindow);
               SetWatchCursor(mainWindow);
            }
            while ((bytes_read=fread(buf, sizeof(char), sizeof(buf), pfp)) >
                  0) {
               if ((int)fwrite(buf, sizeof(char), bytes_read, gif_fp) <= 0) {
                  writeFileFailed = TRUE;
                  break;
               }
            }
            pclose(pfp);
            if (!watch_cursor) {
               SetDefaultCursor(mainWindow);
               ShowCursor();
            }
            fclose(gif_fp);
            gif_fp = NULL;
            if (writeFileFailed) {
               FailToWriteFileMessage(
                     ((short_name && *outputDir=='\0') ? rest : gif_fname));
               FailedExportXPixmapDeckToGIF(count, gif_fp, gif_fname,
                     tmp_fnames);
               ok = FALSE;
            }
         }
      }
   }
   if (ok) {
      /*
       * GIF generated, set gif_fname to NULL so that the GIF file
       * doesn't get deleted FailedExportXPixmapDeckToGIF()
       */
      *gif_fname = '\0';
      FailedExportXPixmapDeckToGIF(count, gif_fp, gif_fname, tmp_fnames);
   }
   curPage->top = topObj = saved_top_obj;
   curPage->bot = botObj = saved_bot_obj;
   PopPageInfo();

   RedrawAnArea(botObj, obj_ptr->bbox.ltx-GRID_ABS_SIZE(1),
         obj_ptr->bbox.lty-GRID_ABS_SIZE(1),
         obj_ptr->bbox.rbx+GRID_ABS_SIZE(1),
         obj_ptr->bbox.rby+GRID_ABS_SIZE(1));
   topSel = botSel = SelectThisObject(obj_ptr);
   UpdSelBBox();
   HighLightForward();
   RestoreStatusStrings();
}

void DumpBitmap(FP, image, data, image_w, image_h, transformed, orig_x, orig_y,
      adj_x, pmtrx, indent)
   FILE *FP;
   XImage *image;
   char *data;
   int image_w, image_h, transformed, orig_x, orig_y, adj_x, indent;
   struct MtrxRec *pmtrx;
{
   register int m;
   int row, h_blocks, v_blocks, nibbles_per_row;

   h_blocks = ((image_w&0xff) == 0) ? (image_w>>8) : ((image_w>>8)+1);
   v_blocks = ((image_h&0xff) == 0) ? (image_h>>8) : ((image_h>>8)+1);
   nibbles_per_row = ((image_w%4)==0) ? (int)(image_w>>2) : (int)(image_w>>2)+1;

   for (m=0; m < indent; m++) fputc(' ', FP);
   fprintf(FP, "%s\n", gPsCmd[PS_GSAVE]);

   for (m=0; m < indent; m++) fputc(' ', FP);
   if (transformed) {
      fprintf(FP, "   %1d %1d %s\n", orig_x, orig_y, gPsCmd[PS_TRANSLATE]);
   } else {
      fprintf(FP, "   %1d %1d %s %.3f %.3f %s %1d %s\n\n",
            orig_x, orig_y, gPsCmd[PS_TRANSLATE], pmtrx->dump_h_scale,
            pmtrx->dump_v_scale, gPsCmd[PS_SCALE], pmtrx->degree,
            gPsCmd[PS_ROTATE]);
   }

   for (row=0; row < v_blocks; row++) {
      int y, col, block_h;

      y = row<<8;
      block_h = (row == v_blocks-1) ? image_h-y : 0x100;

      for (col=0; col < h_blocks; col++) {
         int i, j, x, num_nibbles, nibble_count, block_w;

         x = col<<8;
         block_w = (col == h_blocks-1) ? image_w-x : 0x100;

         num_nibbles = ((block_w%4) == 0) ? (int)(block_w>>2) :
               (int)(block_w>>2)+1;

         for (m=0; m < indent; m++) fputc(' ', FP);
         fprintf(FP, "   %s\n", gPsCmd[PS_GSAVE]);

         for (m=0; m < indent; m++) fputc(' ', FP);
         fprintf(FP, "   %1d %1d %s\n", x+adj_x, y, gPsCmd[PS_TRANSLATE]);

         for (m=0; m < indent; m++) fputc(' ', FP);
         fprintf(FP, "   %1d %1d true [1 0 0 1 0 0]\n", block_w, block_h);

         for (m=0; m < indent; m++) fputc(' ', FP);
         fprintf(FP, "   {<");

         if (PRTGIF && !cmdLineOpenDisplay) {
            nibble_count = 0;
            for (i=0; i < block_h; i++) {
               for (j=0; j < num_nibbles; j++) {
                  if (nibble_count++ == 64) {
                     nibble_count = 1;
                     fprintf(FP, "\n");
                     for (m=0; m < indent; m++) fputc(' ', FP);
                     fprintf(FP, "     ");
                  }
                  fprintf(FP, "%c", data[(i+y)*nibbles_per_row+j+(x>>2)]);
               }
               if ((num_nibbles & 0x1) == 1) {
                  if (nibble_count++ == 64) {
                     nibble_count = 1;
                     fprintf(FP, "\n");
                     for (m=0; m < indent; m++) fputc(' ', FP);
                     fprintf(FP, "     ");
                  }
                  fprintf(FP, "0");
               }
            }
         } else {
            nibble_count = 0;
            for (i=0; i < block_h; i++) {
               int bit_count=0, data;

               data = 0;
               for (j=0; j < block_w; j++) {
                  data = (XGetPixel(image, j+x, i+y) == 1) ? (data<<1) | 1 :
                        (data<<1);
                  if (++bit_count == 4) {
                     if (nibble_count++ == 64) {
                        nibble_count = 1;
                        fprintf(FP, "\n");
                        for (m=0; m < indent; m++) fputc(' ', FP);
                        fprintf(FP, "     ");
                     }
                     fprintf(FP, "%c", hexValue[data]);
                     bit_count = 0;
                     data = 0;
                  }
               }
               if ((block_w % 4) != 0) {
                  data <<= (4 - (block_w % 4));
                  if (nibble_count++ == 64) {
                     nibble_count = 1;
                     fprintf(FP, "\n");
                     for (m=0; m < indent; m++) fputc(' ', FP);
                     fprintf(FP, "     ");
                  }
                  fprintf(FP, "%c", hexValue[data]);
               }
               if ((num_nibbles & 0x1) == 1) {
                  if (nibble_count++ == 64) {
                     nibble_count = 1;
                     fprintf(FP, "\n");
                     for (m=0; m < indent; m++) fputc(' ', FP);
                     fprintf(FP, "     ");
                  }
                  fprintf(FP, "0");
               }
            }
         }
         fprintf(FP, ">}\n");

         for (m=0; m < indent; m++) fputc(' ', FP);
         fprintf(FP, "   %s\n", gPsCmd[PS_IMAGEMASK]);

         for (m=0; m < indent; m++) fputc(' ', FP);
         fprintf(FP, "   %s\n", gPsCmd[PS_GRESTORE]);

         if (row!=v_blocks-1 || col!=h_blocks-1) {
            fprintf(FP, "\n");
         }
      }
   }
   for (m=0; m < indent; m++) fputc(' ', FP);
   fprintf(FP, "%s\n", gPsCmd[PS_GRESTORE]);
}

void DumpXBmObj(FP, ObjPtr)
   FILE *FP;
   struct ObjRec *ObjPtr;
{  /* called when printing in the PostScript format */
   int ltx, lty, rbx, rby, w, h, trans_pat, fill, image_w, image_h;
   int color_index, flip, orig_x=ObjPtr->x, orig_y=ObjPtr->y;
   Pixmap bitmap=None;
   XImage *image=NULL;
   struct XBmRec *xbm_ptr=ObjPtr->detail.xbm;
   struct MtrxRec mtrx;

   if (xbm_ptr->real_type == XBM_EPS) {
      DumpEPSObj(FP, ObjPtr);
      if (preDumpSetup) hasReadHexString = TRUE;
      return;
   }

   if (ObjPtr->ctm == NULL) {
      ltx = ObjPtr->obbox.ltx;
      lty = ObjPtr->obbox.lty;
      rbx = ObjPtr->obbox.rbx;
      rby = ObjPtr->obbox.rby;
   } else {
      ltx = ObjPtr->orig_obbox.ltx;
      lty = ObjPtr->orig_obbox.lty;
      rbx = ObjPtr->orig_obbox.rbx;
      rby = ObjPtr->orig_obbox.rby;
   }

   trans_pat = ObjPtr->trans_pat;
   bitmap = xbm_ptr->bitmap;
   fill = xbm_ptr->fill;
   flip = xbm_ptr->flip;
   image_w = xbm_ptr->image_w;
   image_h = xbm_ptr->image_h;

   w = rbx - ltx;
   h = rby - lty;

   if (!(PRTGIF && !cmdLineOpenDisplay)) {
      if ((image=xbm_ptr->image) == NULL) {
         if ((image=xbm_ptr->image=XGetImage(mainDisplay, bitmap, 0, 0,
               image_w, image_h, 1, ZPixmap)) == NULL) {
            MsgBox(TgLoadString(STID_XGETIMAGE_MAY_RUN_OUT_VMEM), TOOL_NAME,
                  INFO_MB);
            return;
         }
      }
   }
   color_index = ObjPtr->color;

   if (ObjPtr->ctm == NULL) {
      memset(&mtrx, 0, sizeof(struct MtrxRec));
      mtrx.image_w = (float)image_w; mtrx.image_h = (float)image_h;
      mtrx.w = (float)w; mtrx.h = (float)h;
      mtrx.rotate = ROTATE0; mtrx.flip = flip;

      CalcTransform(&mtrx);

      orig_x = (mtrx.transformed_w >= 0.0) ? ltx : ltx+w;
      orig_y = (mtrx.transformed_h >= 0.0) ? lty : lty+h;
   }
   fprintf(FP, "%% XBM\n");
   if (ObjPtr->ctm != NULL) {
      float m[6];

      fprintf(FP, "%s\n", gPsCmd[PS_GSAVE]);
      m[CTM_SX] = ((float)ObjPtr->ctm->m[CTM_SX])/((float)1000.0);
      m[CTM_SY] = ((float)ObjPtr->ctm->m[CTM_SY])/((float)1000.0);
      m[CTM_SIN] = ((float)ObjPtr->ctm->m[CTM_SIN])/((float)1000.0);
      m[CTM_MSIN] = ((float)ObjPtr->ctm->m[CTM_MSIN])/((float)1000.0);
      fprintf(FP, "   %1d %1d %s\n", ObjPtr->x, ObjPtr->y,
            gPsCmd[PS_TRANSLATE]);
      fprintf(FP, "   [%.3f %.3f %.3f %.3f %1d %1d] %s\n",
            m[CTM_SX], m[CTM_SIN], m[CTM_MSIN], m[CTM_SY],
            ObjPtr->ctm->t[CTM_TX], ObjPtr->ctm->t[CTM_TY], gPsCmd[PS_CONCAT]);
      fprintf(FP, "   %1d %s %1d %s %s\n",
            ObjPtr->x, gPsCmd[PS_NEG], ObjPtr->y, gPsCmd[PS_NEG],
            gPsCmd[PS_TRANSLATE]);
   }
   DumpRGBColorLine(FP, color_index, 0, TRUE);

   switch (fill) {
   case NONEPAT: break;
   case SOLIDPAT:
      DumpRectPath(FP, ltx, lty, rbx, rby, 0, FALSE);
      fprintf(FP, "%s\n", gPsCmd[PS_FILL]);
      break;
   case BACKPAT:
      if (!trans_pat) {
         DumpRectPath(FP, ltx, lty, rbx, rby, 0, FALSE);
         fprintf(FP, "1 %s %s\n\n", gPsCmd[PS_SETGRAY], gPsCmd[PS_FILL]);
         DumpRGBColorLine(FP, color_index, 0, TRUE);
      }
      break;
   default:
      /* patterned */
      fprintf(FP, "%s\n", gPsCmd[PS_GSAVE]);
      if (colorDump || !useGray) {
         if (preDumpSetup) PSUseColorPattern();
         if (!trans_pat) {
            DumpRectPath(FP, ltx, lty, rbx, rby, 3, FALSE);
            fprintf(FP, "1 %s %s\n\n", gPsCmd[PS_SETGRAY], gPsCmd[PS_FILL]);
            DumpRGBColorLine(FP, color_index, 3, TRUE);
         }
         DumpRectPath(FP, ltx, lty, rbx, rby, 3, FALSE);
         fprintf(FP, "%s %s\n", gPsCmd[PS_EOCLIP], gPsCmd[PS_NEWPATH]);
         DumpPatFill(FP, fill, ObjPtr->bbox, 3, TRUE);
      } else {
         GrayCheck(fill);
         fprintf(FP, "   %s %s\n", GrayStr(fill), gPsCmd[PS_SETGRAY]);
         DumpRectPath(FP, ltx, lty, rbx, rby, 3, FALSE);
         fprintf(FP, "%s\n", gPsCmd[PS_FILL]);
      }
      fprintf(FP, "%s\n", gPsCmd[PS_GRESTORE]);
      break;
   }
   DumpBitmap(FP, image, ObjPtr->detail.xbm->data, image_w, image_h,
         (ObjPtr->ctm!=0), orig_x, orig_y, 0, &mtrx, 0);

   if (ObjPtr->ctm != NULL) fprintf(FP, "%s\n", gPsCmd[PS_GRESTORE]);
   fprintf(FP, "\n");
}

int NeedsToCacheXBmObj(ObjPtr)
   struct ObjRec *ObjPtr;
{
   register struct XBmRec *xbm_ptr=ObjPtr->detail.xbm;
   int w, h;

   w = ObjPtr->obbox.rbx - ObjPtr->obbox.ltx;
   h = ObjPtr->obbox.rby - ObjPtr->obbox.lty;

   return (ObjPtr->ctm != NULL || zoomScale != 0 ||
         xbm_ptr->image_w != w || xbm_ptr->image_h != h || xbm_ptr->flip != 0
   );
}

static
void DrawHiddenXBm(win, ctm, vs, x, y, w, h, s)
   Window win;
   struct XfrmMtrxRec *ctm;
   XPoint *vs;
   int x, y, w, h;
   char *s;
{
   int str_w, len, sx, sy;
   XGCValues values;

   values.foreground = GetDrawingFgPixel(INVALID, INVALID);
   values.function = GXcopy;;
   values.fill_style = FillSolid;
#ifdef NO_THIN_LINE
   values.line_width = 1;
#else
   values.line_width = 0;
#endif
   values.font = rulerFontPtr->fid;
   values.line_style = LineSolid;

   XChangeGC(mainDisplay, drawGC,
         GCForeground | GCFunction | GCFillStyle | GCLineWidth | GCFont |
         GCLineStyle, &values);
   if (ctm != NULL) {
      XDrawLines(mainDisplay, win, drawGC, vs, 5, CoordModeOrigin);
   } else {
      XDrawRectangle(mainDisplay, win, drawGC, x, y, w, h);
   }

   len = strlen(s);
   str_w = rulerFontWidth*len;
   if (str_w < w && rulerFontHeight < h) {
      sx = x + ((w-str_w)>>1);
      sy = y + ((h-rulerFontHeight)>>1);
      XDrawString(mainDisplay, win, drawGC, sx, sy+rulerFontAsc, s, len);
   }
   XSetFont(mainDisplay, drawGC, canvasFontPtr->fid);
}

void DrawXBmObj(win, XOff, YOff, ObjPtr)
   Window win;
   int XOff, YOff;
   struct ObjRec *ObjPtr;
{
   int ltx, lty, rbx, rby, w, h, scr_w, scr_h, fill, trans_pat;
   int real_x_off, real_y_off;
   char s[80];
   struct XBmRec *xbm_ptr=ObjPtr->detail.xbm;
   XGCValues values;

   if (userDisableRedraw) return;

   if (ObjPtr->prev != NULL && ObjPtr->prev->type == OBJ_XBM &&
         ObjPtr->prev->detail.xbm->real_type == XBM_XBM &&
         ObjPtr->prev->detail.xbm->fill != NONEPAT &&
         Inside(ObjPtr->obbox, ObjPtr->prev->obbox)) {
      return;
   }
   trans_pat = ObjPtr->trans_pat;
   fill = xbm_ptr->fill;

   w = ObjPtr->obbox.rbx - ObjPtr->obbox.ltx;
   h = ObjPtr->obbox.rby - ObjPtr->obbox.lty;

   real_x_off = (zoomedIn ? XOff : (XOff>>zoomScale)<<zoomScale);
   real_y_off = (zoomedIn ? YOff : (YOff>>zoomScale)<<zoomScale);
   ltx = ZOOMED_SIZE(ObjPtr->obbox.ltx - real_x_off);
   lty = ZOOMED_SIZE(ObjPtr->obbox.lty - real_y_off);
   rbx = ZOOMED_SIZE(ObjPtr->obbox.rbx - real_x_off);
   rby = ZOOMED_SIZE(ObjPtr->obbox.rby - real_y_off);
   scr_w = rbx - ltx;
   scr_h = rby - lty;

   if (xbm_ptr->real_type==XBM_XBM && fill != NONEPAT) {
      values.foreground = GetDrawingBgPixel(fill, colorPixels[ObjPtr->color]);
      values.function = GXcopy;
      values.fill_style = (trans_pat ? FillStippled : FillOpaqueStippled);
      values.stipple = patPixmap[fill];
      XChangeGC(mainDisplay, drawGC,
            GCForeground | GCFunction | GCFillStyle | GCStipple, &values);
      if (ObjPtr->ctm != NULL) {
         XFillPolygon(mainDisplay, win, drawGC,  ObjPtr->rotated_obbox, 5,
               Convex, CoordModeOrigin);
      } else {
         XFillRectangle(mainDisplay, win, drawGC, ltx, lty, scr_w, scr_h);
      }
   }
   if (!mapShown) {
      if (xbm_ptr->real_type == XBM_EPS) {
         DrawHiddenXBm(win, ObjPtr->ctm, ObjPtr->rotated_obbox,
               ltx, lty, scr_w, scr_h, xbm_ptr->filename);
      } else {
         sprintf(s, "(%1dx%1d)", xbm_ptr->image_w, xbm_ptr->image_h);
         DrawHiddenXBm(win, ObjPtr->ctm, ObjPtr->rotated_obbox,
               ltx, lty, scr_w, scr_h, s);
      }
      return;
   }
   if (NeedsToCacheXBmObj(ObjPtr) && xbm_ptr->cached_bitmap == None) {
      MakeCachedBitmap(ObjPtr);
   }
   if (xbm_ptr->real_type==XBM_XBM || (xbm_ptr->real_type==XBM_EPS &&
         xbm_ptr->bitmap!=None)) {
      values.foreground = colorPixels[ObjPtr->color];
      values.function = GXcopy;
      values.fill_style = FillStippled;
      values.ts_x_origin = ltx;
      values.ts_y_origin = lty;

      if (ObjPtr->ctm==NULL && zoomScale==0 &&
            xbm_ptr->flip==0 && xbm_ptr->image_w==w && xbm_ptr->image_h==h) {
         values.stipple = xbm_ptr->bitmap;
      } else {
         if (xbm_ptr->cached_bitmap == None) return;
         values.stipple = xbm_ptr->cached_bitmap;
      }

      XChangeGC(mainDisplay, drawGC,
            GCForeground | GCFunction | GCFillStyle | GCStipple |
            GCTileStipXOrigin | GCTileStipYOrigin, &values);
      XFillRectangle(mainDisplay, win, drawGC, ltx, lty, scr_w, scr_h);
      XSetTSOrigin(mainDisplay, drawGC, 0, 0);
   } else if (xbm_ptr->real_type==XBM_EPS && xbm_ptr->bitmap==None) {
      DrawHiddenXBm(win, ObjPtr->ctm, ObjPtr->rotated_obbox,
               ltx, lty, rbx-ltx, rby-lty, xbm_ptr->filename);
   }
}

struct ObjRec *CreateXBmObj(ImageW, ImageH, W, H, bitmap, image)
   int ImageW, ImageH, W, H;
   Pixmap bitmap;
   XImage *image;
{
   struct XBmRec *xbm_ptr;
   struct ObjRec *obj_ptr;

   xbm_ptr = (struct XBmRec *)malloc(sizeof(struct XBmRec));
   if (xbm_ptr == NULL) FailAllocMessage();
   memset(xbm_ptr, 0, sizeof(struct XBmRec));

   xbm_ptr->image = image;
   xbm_ptr->image_w = ImageW;
   xbm_ptr->image_h = ImageH;
   xbm_ptr->bitmap = bitmap;
   xbm_ptr->data = NULL;

   xbm_ptr->eps_w = xbm_ptr->eps_h = 0;

   xbm_ptr->fill = objFill;
   xbm_ptr->flip = xbm_ptr->cached_flip = 0;
   xbm_ptr->cached_zoom = 0;
   xbm_ptr->cached_bitmap = None;
   xbm_ptr->cached_w = xbm_ptr->cached_h = 0;

   xbm_ptr->real_type = XBM_XBM;
   xbm_ptr->filename = NULL;
   xbm_ptr->epsflines = NULL;
   xbm_ptr->num_epsf_lines = 0;

   obj_ptr = (struct ObjRec *)malloc(sizeof(struct ObjRec));
   if (obj_ptr == NULL) FailAllocMessage();
   memset(obj_ptr, 0, sizeof(struct ObjRec));

   obj_ptr->bbox.ltx = obj_ptr->obbox.ltx = obj_ptr->x = drawOrigX;
   obj_ptr->bbox.lty = obj_ptr->obbox.lty = obj_ptr->y = drawOrigY;
   obj_ptr->bbox.rbx = obj_ptr->obbox.rbx = W + drawOrigX;
   obj_ptr->bbox.rby = obj_ptr->obbox.rby = H + drawOrigY;
   obj_ptr->type = OBJ_XBM;
   obj_ptr->color = colorIndex;
   if (mainDisplay != NULL) {
      UtilStrCpyN(obj_ptr->color_str, sizeof(obj_ptr->color_str),
            colorMenuItems[colorIndex]);
   }
   obj_ptr->id = objId++;
   obj_ptr->dirty = FALSE;
   obj_ptr->rotation = 0;
   obj_ptr->locked = FALSE;
   obj_ptr->detail.xbm = xbm_ptr;
   obj_ptr->fattr = obj_ptr->lattr = NULL;
   obj_ptr->ctm = NULL;
   obj_ptr->invisible = FALSE;
   obj_ptr->trans_pat = transPat;

   if (importXBmRV) InvertXBmObject(obj_ptr);
   return (obj_ptr);
}

static
void CopyXBmProperties(DestXbmObj, SrcXbmObj)
   struct ObjRec *DestXbmObj, *SrcXbmObj;
{
   struct XBmRec *dest_xbm_ptr=DestXbmObj->detail.xbm;
   struct XBmRec *src_xbm_ptr=SrcXbmObj->detail.xbm;

   dest_xbm_ptr->fill = src_xbm_ptr->fill;
   DestXbmObj->color = SrcXbmObj->color;
   memcpy(DestXbmObj->color_str, SrcXbmObj->color_str,
         sizeof(DestXbmObj->color_str));
   DestXbmObj->trans_pat = SrcXbmObj->trans_pat;
}

static
int FinishBreakUpXBitmap(obj_ptr, cols_and_rows, cols, rows)
   struct ObjRec *obj_ptr;
   int cols_and_rows, cols, rows;
{
   struct XBmRec *xbm_ptr=obj_ptr->detail.xbm;
   int y, image_w=xbm_ptr->image_w, image_h=xbm_ptr->image_h;
   int chunk_w=0, chunk_h=0, total_chunks=0;
   int orig_x=obj_ptr->x, orig_y=obj_ptr->y;

   if (cols_and_rows) {
      chunk_w = (int)(image_w / cols);
      chunk_h = (int)(image_h / rows);
   } else {
      chunk_w = cols;
      chunk_h = rows;
   }
   for (y=0; y < image_h; y += chunk_h) {
      int h=min(image_h-y,chunk_h), x;

      for (x=0; x < image_w; x += chunk_w) {
         int w=min(image_w-x,chunk_w);
         struct ObjRec *new_obj_ptr;
         Pixmap dest_bitmap=None;
         XImage *dest_image=NULL;

         if (w <= 0 || h <= 0 ||
               !ExtractBitmap(xbm_ptr->bitmap, xbm_ptr->image, x, y, w, h,
               &dest_bitmap, &dest_image)) {
            continue;
         }
         total_chunks++;
         new_obj_ptr = CreateXBmObj(w, h, w, h, dest_bitmap, dest_image);
         CopyXBmProperties(new_obj_ptr, obj_ptr);
         AdjObjBBox(new_obj_ptr);
         MoveObj(new_obj_ptr, orig_x+x, orig_y+y);
         AddObj(NULL, topObj, new_obj_ptr);
      }
   }
   if (total_chunks > 0) {
      int i;

      RemoveAllSel();
      UnlinkObj(obj_ptr);
      FreeObj(obj_ptr);

      for (i=0, obj_ptr=topObj; obj_ptr != NULL && i < total_chunks; i++,
            obj_ptr=obj_ptr->next) {
         AddObjIntoSel(obj_ptr, botSel, NULL, &topSel, &botSel);
      }
   }
   return (total_chunks > 0);
}

void BreakUpXBitmap(obj_ptr, cols_and_rows, cols, rows)
   struct ObjRec *obj_ptr;
   int cols_and_rows, cols, rows;
{
   struct XBmRec *xbm_ptr=obj_ptr->detail.xbm;

   if (xbm_ptr->real_type == XBM_EPS) {
      MsgBox(TgLoadString(STID_CANNOT_BREAKUP_EPS), TOOL_NAME, INFO_MB);
      return;
   }
   HighLightReverse();
   PrepareToReplaceAnObj(obj_ptr);
   if (FinishBreakUpXBitmap(obj_ptr, cols_and_rows, cols, rows)) {
      UpdSelBBox();
      RecordCmd(CMD_ONE_TO_MANY, NULL, topSel, botSel, numObjSelected);
      SetFileModified(TRUE);
      justDupped = FALSE;
   } else {
      AbortPrepareCmd(CMD_REPLACE);
   }
   HighLightForward();
}

void DumpXBmData(FP, bitmap, image, W, H)
   FILE *FP;
   Pixmap bitmap;
   XImage **image;
   int W, H;
{
   register int nibble_count, bit_count, data, i, j;

   if (*image == NULL) {
      if ((*image=XGetImage(mainDisplay,bitmap,0,0,W,H,1,ZPixmap)) == NULL) {
         MsgBox(TgLoadString(STID_XGETIMAGE_MAY_RUN_OUT_VMEM), TOOL_NAME,
               INFO_MB);
         return;
      }
   }

   nibble_count = 0;

   for (i=0; i < H; i++) {
      bit_count = 0;
      data = 0;

      for (j=0; j < W; j++) {
         data = (XGetPixel(*image, j, i) == 1) ? (data<<1) | 1 : (data<<1);

         if (++bit_count == 4) {
            if (nibble_count++ == 64) {
               nibble_count = 1;
               if (fprintf(FP, "\n     ") == EOF) writeFileFailed = TRUE;
            }
            if (fprintf(FP, "%c", hexValue[data]) == EOF) {
               writeFileFailed = TRUE;
            }
            bit_count = 0;
            data = 0;
         }
      }
      if ((W % 4) != 0) {
         data <<= (4 - (W % 4));
         if (nibble_count++ == 64) {
            nibble_count = 1;
            if (fprintf(FP, "\n     ") == EOF) writeFileFailed = TRUE;
         }
         if (fprintf(FP, "%c", hexValue[data]) == EOF) writeFileFailed = TRUE;
      }
   }
}

void SaveXBmObj(FP, ObjPtr)
   FILE *FP;
   struct ObjRec *ObjPtr;
{
   register int i;
   int ltx, lty, rbx, rby, image_w, image_h;
   int no_bitmap, compressed=FALSE;
   struct XBmRec *xbm_ptr=ObjPtr->detail.xbm;

   no_bitmap = (xbm_ptr->real_type==XBM_EPS &&
         !(xbm_ptr->save_epsf && xbm_ptr->bitmap != None));

   ltx = ObjPtr->obbox.ltx; lty = ObjPtr->obbox.lty;
   rbx = ObjPtr->obbox.rbx; rby = ObjPtr->obbox.rby;
   if (no_bitmap) {
      image_w = xbm_ptr->eps_w;
      image_h = xbm_ptr->eps_h;
   } else {
      image_w = xbm_ptr->image_w;
      image_h = xbm_ptr->image_h;
   }
   if (fprintf(FP, "xbm('%s','',", colorMenuItems[ObjPtr->color]) == EOF) {
      writeFileFailed = TRUE;
   }
   if (fprintf(FP,
         "%1d,%1d,%1d,%1d,%1d,%1d,%1d,%1d,%1d,0,%1d,%1d,%1d,%1d,%1d,%1d,",
         ltx, lty, rbx, rby, xbm_ptr->fill, ObjPtr->id, ObjPtr->rotation,
         image_w, image_h, xbm_ptr->flip, xbm_ptr->real_type,
         xbm_ptr->llx, xbm_ptr->lly, xbm_ptr->urx, xbm_ptr->ury) == EOF) {
      writeFileFailed = TRUE;
   }
   if (fprintf(FP,
         "%1d,%1d,%1d,%1d,%1d,%1d,%1d,\n    ",
         no_bitmap, ObjPtr->locked, xbm_ptr->save_epsf, compressed,
         ObjPtr->ctm!=NULL, ObjPtr->invisible, ObjPtr->trans_pat) == EOF) {
      writeFileFailed = TRUE;
   }
   switch (xbm_ptr->real_type) {
   case XBM_XBM:
      if (fprintf(FP, "\"\",\"\",") == EOF) writeFileFailed = TRUE;
      break;
   case XBM_EPS:
      if (fprintf(FP, "\"%s\",\"%s\",", xbm_ptr->write_date,
            xbm_ptr->filename) == EOF) {
         writeFileFailed = TRUE;
      }
      break;
   }
   if (xbm_ptr->save_epsf) {
      if (fprintf(FP, "%1d,[", xbm_ptr->num_epsf_lines) == EOF) {
         writeFileFailed = TRUE;
      }
      for (i = 0; i < xbm_ptr->num_epsf_lines; i++) {
         if (fprintf(FP, "\n    \"%s\"%s,", xbm_ptr->epsflines[i],
               (i==xbm_ptr->num_epsf_lines-1 ? "]" : "")) == EOF) {
            writeFileFailed = TRUE;
         }
      }
   }
   if (!no_bitmap) {
      if (fprintf(FP, "\n    \"") == EOF) writeFileFailed = TRUE;
      DumpXBmData(FP, xbm_ptr->bitmap, &(xbm_ptr->image), image_w, image_h);
      if (fprintf(FP, "\",") == EOF) writeFileFailed = TRUE;
   }
   if (ObjPtr->ctm != NULL && fprintf(FP,
         "[\n    %1d,%1d,%1d,%1d,%1d,%1d,%g,%g,%g,%g,%1d,%1d],",
         ObjPtr->x, ObjPtr->y,
         ObjPtr->orig_obbox.ltx, ObjPtr->orig_obbox.lty,
         ObjPtr->orig_obbox.rbx, ObjPtr->orig_obbox.rby,
         ObjPtr->ctm->m[CTM_SX], ObjPtr->ctm->m[CTM_SIN],
         ObjPtr->ctm->m[CTM_MSIN], ObjPtr->ctm->m[CTM_SY],
         ObjPtr->ctm->t[CTM_TX], ObjPtr->ctm->t[CTM_TY]) == EOF) {
      writeFileFailed = TRUE;
   }
   if (serializingFile) SaveCreatorID(FP, ObjPtr, "    ");
   SaveAttrs(FP, ObjPtr->lattr);
   if (fprintf(FP, ")") == EOF) writeFileFailed = TRUE;
}

static
int ReadTransformAndAdjustForXBm(FP, ObjPtr, xbm_ptr, transformed, rotate, flip)
   FILE *FP;
   struct ObjRec **ObjPtr;
   struct XBmRec *xbm_ptr;
   int transformed, rotate, flip;
{
   if (fileVersion >= 33 && transformed) {
      char inbuf[MAXSTRING];
      struct XfrmMtrxRec *ctm=NULL;
      int real_x=0, real_y=0;
      struct BBRec orig_obbox;

      (void)fgets(inbuf, MAXSTRING, FP);
      scanLineNum++;
      InitScan(inbuf, "\t\n, ");

      ctm = (struct XfrmMtrxRec *)malloc(sizeof(struct XfrmMtrxRec));
      if (ctm == NULL) FailAllocMessage();
      if (GETINT("xbitmap", real_x,           "real_x") == INVALID ||
          GETINT("xbitmap", real_y,           "real_y") == INVALID ||
          GETINT("xbitmap", orig_obbox.ltx,   "orig_obbox.ltx") == INVALID ||
          GETINT("xbitmap", orig_obbox.lty,   "orig_obbox.lty") == INVALID ||
          GETINT("xbitmap", orig_obbox.rbx,   "orig_obbox.rbx") == INVALID ||
          GETINT("xbitmap", orig_obbox.rby,   "orig_obbox.rby") == INVALID ||
          GETDBL("xbitmap", ctm->m[CTM_SX],   "CTM_SX") == INVALID ||
          GETDBL("xbitmap", ctm->m[CTM_SIN],  "CTM_SIN") == INVALID ||
          GETDBL("xbitmap", ctm->m[CTM_MSIN], "CTM_MSIN") == INVALID ||
          GETDBL("xbitmap", ctm->m[CTM_SY],   "CTM_SY") == INVALID ||
          GETINT("xbitmap", ctm->t[CTM_TX],   "CTM_TX") == INVALID ||
          GETINT("xbitmap", ctm->t[CTM_TY],   "CTM_TY") == INVALID) {
         return FALSE;
      }
      (*ObjPtr)->ctm = ctm;
      if (ctm != NULL) {
         memcpy(&(*ObjPtr)->orig_obbox, &orig_obbox, sizeof(struct BBRec));
         (*ObjPtr)->x = real_x;
         (*ObjPtr)->y = real_y;
         GetTransformedOBBoxOffsetVs(*ObjPtr, (*ObjPtr)->rotated_obbox);
      }
   }
   if (fileVersion < 33 && (rotate != ROTATE0 || flip != NO_FLIP)) {
      int ltx, lty, rbx, rby;
      double dz=(double)0, d1=(double)1000.0, dm1=((double)-1000.0);

      if (rotate == ROTATE90 || rotate == ROTATE270) {
         int h=(*ObjPtr)->obbox.rbx-(*ObjPtr)->obbox.ltx;
         int w=(*ObjPtr)->obbox.rby-(*ObjPtr)->obbox.lty;

         (*ObjPtr)->obbox.rby = (*ObjPtr)->obbox.lty + h;
         (*ObjPtr)->obbox.rbx = (*ObjPtr)->obbox.ltx + w;
      }
      ltx = ((*ObjPtr)->obbox.ltx); lty = ((*ObjPtr)->obbox.lty);
      rbx = ((*ObjPtr)->obbox.rbx); rby = ((*ObjPtr)->obbox.rby);
      SetRotatePivotByObject(*ObjPtr);

      if ((*ObjPtr)->ctm == NULL) {
         int image_w=0, image_h=0;

         if (xbm_ptr->bitmap == None) {
            image_w = xbm_ptr->eps_w;
            image_h = xbm_ptr->eps_h;
         } else {
            image_w = xbm_ptr->image_w;
            image_h = xbm_ptr->image_h;
         }
         if (image_w != rbx-ltx || image_h != rby-lty) {
            float fval=(float)0;
            double dx_scale=(double)0, dy_scale=(double)0;
            struct XfrmMtrxRec ctm;

            memset(&ctm, 0, sizeof(struct XfrmMtrxRec));
            fval = ((float)(rbx-ltx))/((float)image_w)*((float)1000.0);
            dx_scale = (double)fval;
            fval = ((float)(rby-lty))/((float)image_h)*((float)1000.0);
            dy_scale = (double)fval;
            (*ObjPtr)->obbox.rbx = ltx+image_w;
            (*ObjPtr)->obbox.rby = lty+image_h;
            ShearObj(*ObjPtr, CORNER_RB, dz, dz, dx_scale, dy_scale,
                  NULL, NULL);
         }
      }
      if (flip & HORI_EVEN) {
         ShearObj(*ObjPtr, CORNER_LEFT, dz, dz, dm1, d1,
               &ltx, &lty);
      }
      if (flip & VERT_EVEN) {
         ShearObj(*ObjPtr, CORNER_TOP, dz, dz, d1, dm1,
               &ltx, &lty);
      }
      if (rotate == ROTATE0) {
         if (flip & (HORI_ODD | VERT_ODD)) {
            RotateObj(*ObjPtr, CORNER_LT, CLOCKWISE90, &ltx, &lty);
            if (flip & HORI_ODD) {
               ShearObj(*ObjPtr, CORNER_LEFT, dz, dz, dm1, d1,
                     &ltx, &lty);
            }
            if (flip & VERT_ODD) {
               ShearObj(*ObjPtr, CORNER_TOP, dz, dz, d1, dm1,
                     &ltx, &lty);
            }
            RotateObj(*ObjPtr, CORNER_LT, COUNTER90, &ltx, &lty);
         }
      } else {
         switch (rotate) {
         case ROTATE90:
            RotateObj(*ObjPtr, CORNER_LT, CLOCKWISE90, &ltx, &lty);
            if (flip & HORI_ODD) {
               ShearObj(*ObjPtr, CORNER_LEFT, dz, dz, dm1, d1,
                     &ltx, &lty);
            }
            if (flip & VERT_ODD) {
               ShearObj(*ObjPtr, CORNER_TOP, dz, dz, d1, dm1,
                     &ltx, &lty);
            }
            break;
         case ROTATE180:
            RotateObj(*ObjPtr, CORNER_LT, CLOCKWISE90, &ltx, &lty);
            if (flip & HORI_ODD) {
               ShearObj(*ObjPtr, CORNER_LEFT, dz, dz, dm1, d1,
                     &ltx, &lty);
            }
            if (flip & VERT_ODD) {
               ShearObj(*ObjPtr, CORNER_TOP, dz, dz, d1, dm1,
                     &ltx, &lty);
            }
            RotateObj(*ObjPtr, CORNER_LT, CLOCKWISE90, &ltx, &lty);
            break;
         case ROTATE270:
            RotateObj(*ObjPtr, CORNER_LT, CLOCKWISE90, &ltx, &lty);
            if (flip & HORI_ODD) {
               ShearObj(*ObjPtr, CORNER_LEFT, dz, dz, dm1, d1,
                     &ltx, &lty);
            }
            if (flip & VERT_ODD) {
               ShearObj(*ObjPtr, CORNER_TOP, dz, dz, d1, dm1,
                     &ltx, &lty);
            }
            RotateObj(*ObjPtr, CORNER_LT, CLOCKWISE90, &ltx, &lty);
            RotateObj(*ObjPtr, CORNER_LT, CLOCKWISE90, &ltx, &lty);
            break;
         }
      }
      xbm_ptr->flip = NO_FLIP;
   }
   return TRUE;
}

void ReadXBmObj(FP, Inbuf, ObjPtr)
   FILE *FP;
   char *Inbuf;
   struct ObjRec **ObjPtr;
{
   struct XBmRec *xbm_ptr=NULL;
   char color_str[40], bg_color_str[40], *s, inbuf[MAXSTRING], *c_ptr;
   int ltx, lty, rbx, rby, i, j, k, data=0, color_index;
   int nibble_count, bit_count, num_nibbles, fill, trans_pat=FALSE;
   int rotation=0, new_alloc, id=0, image_w=0, image_h=0;
   int rotate=ROTATE0, flip=NO_FLIP, real_type=XBM_XBM, len;
   int no_bitmap=FALSE, llx=0, lly=0, urx=0, ury=0;
   int locked=FALSE, save_epsf=FALSE, compressed=FALSE;
   int num_epsf_lines=0, transform_read=FALSE;
   int transformed=FALSE, invisible=FALSE;
   Pixmap bitmap;
   char **epsflines=NULL;
   char *xbm_data, write_date[32], *filename;
   XImage *image;

   *ObjPtr = NULL;

   s = FindChar((int)'(', Inbuf);
   s = ParseStr(s, (int)',', color_str, sizeof(color_str));
   if (fileVersion >= 37) {
      s = ParseStr(s, (int)',', bg_color_str, sizeof(bg_color_str));
   }
   InitScan(s, "\t\n, ");

   if (fileVersion <= 8) {
      sprintf(inbuf, TgLoadCachedString(CSTID_INVALID_FILEVER_FOR_XBM),
            fileVersion);
      if (PRTGIF) {
         fprintf(stderr, "%s\n", inbuf);
      } else {
         Msg(inbuf);
      }
      return;
   } else if (fileVersion <= 13) {
      if (GETINT("xbitmap", ltx,      "ltx") == INVALID ||
          GETINT("xbitmap", lty,      "lty") == INVALID ||
          GETINT("xbitmap", rbx,      "rbx") == INVALID ||
          GETINT("xbitmap", rby,      "rby") == INVALID ||
          GETINT("xbitmap", fill,     "fill") == INVALID ||
          GETINT("xbitmap", id,       "id") == INVALID) {
         return;
      }
      if (id >= objId) objId = id+1;
   } else if (fileVersion <= 22) {
      if (GETINT("xbitmap", ltx,      "ltx") == INVALID ||
          GETINT("xbitmap", lty,      "lty") == INVALID ||
          GETINT("xbitmap", rbx,      "rbx") == INVALID ||
          GETINT("xbitmap", rby,      "rby") == INVALID ||
          GETINT("xbitmap", fill,     "fill") == INVALID ||
          GETINT("xbitmap", id,       "id") == INVALID ||
          GETINT("xbitmap", rotation, "rotation") == INVALID) {
         return;
      }
      if (id >= objId) objId = id+1;
   } else if (fileVersion <= 23) {
      if (GETINT("xbitmap", ltx,      "ltx") == INVALID ||
          GETINT("xbitmap", lty,      "lty") == INVALID ||
          GETINT("xbitmap", rbx,      "rbx") == INVALID ||
          GETINT("xbitmap", rby,      "rby") == INVALID ||
          GETINT("xbitmap", fill,     "fill") == INVALID ||
          GETINT("xbitmap", id,       "id") == INVALID ||
          GETINT("xbitmap", rotation, "rotation") == INVALID ||
          GETINT("xbitmap", image_w,  "image_w") == INVALID ||
          GETINT("xbitmap", image_h,  "image_h") == INVALID ||
          GETINT("xbitmap", rotate,   "rotate") == INVALID ||
          GETINT("xbitmap", flip,     "flip") == INVALID) {
         return;
      }
      if (id >= objId) objId = id+1;
   } else if (fileVersion <= 25) {
      if (GETINT("xbitmap", ltx,       "ltx") == INVALID ||
          GETINT("xbitmap", lty,       "lty") == INVALID ||
          GETINT("xbitmap", rbx,       "rbx") == INVALID ||
          GETINT("xbitmap", rby,       "rby") == INVALID ||
          GETINT("xbitmap", fill,      "fill") == INVALID ||
          GETINT("xbitmap", id,        "id") == INVALID ||
          GETINT("xbitmap", rotation,  "rotation") == INVALID ||
          GETINT("xbitmap", image_w,   "image_w") == INVALID ||
          GETINT("xbitmap", image_h,   "image_h") == INVALID ||
          GETINT("xbitmap", rotate,    "rotate") == INVALID ||
          GETINT("xbitmap", flip,      "flip") == INVALID ||
          GETINT("xbitmap", real_type, "real_type") == INVALID ||
          GETINT("xbitmap", llx,       "llx") == INVALID ||
          GETINT("xbitmap", lly,       "lly") == INVALID ||
          GETINT("xbitmap", urx,       "urx") == INVALID ||
          GETINT("xbitmap", ury,       "ury") == INVALID ||
          GETINT("xbitmap", no_bitmap, "no_bitmap") == INVALID) {
         return;
      }
      if (id >= objId) objId = id+1;
   } else if (fileVersion <= 28) {
      if (GETINT("xbitmap", ltx,       "ltx") == INVALID ||
          GETINT("xbitmap", lty,       "lty") == INVALID ||
          GETINT("xbitmap", rbx,       "rbx") == INVALID ||
          GETINT("xbitmap", rby,       "rby") == INVALID ||
          GETINT("xbitmap", fill,      "fill") == INVALID ||
          GETINT("xbitmap", id,        "id") == INVALID ||
          GETINT("xbitmap", rotation,  "rotation") == INVALID ||
          GETINT("xbitmap", image_w,   "image_w") == INVALID ||
          GETINT("xbitmap", image_h,   "image_h") == INVALID ||
          GETINT("xbitmap", rotate,    "rotate") == INVALID ||
          GETINT("xbitmap", flip,      "flip") == INVALID ||
          GETINT("xbitmap", real_type, "real_type") == INVALID ||
          GETINT("xbitmap", llx,       "llx") == INVALID ||
          GETINT("xbitmap", lly,       "lly") == INVALID ||
          GETINT("xbitmap", urx,       "urx") == INVALID ||
          GETINT("xbitmap", ury,       "ury") == INVALID ||
          GETINT("xbitmap", no_bitmap, "no_bitmap") == INVALID ||
          GETINT("xbitmap", locked,    "locked") == INVALID) {
         return;
      }
      if (id >= objId) objId = id+1;
   } else if (fileVersion <= 32) {
      if (GETINT("xbitmap", ltx,       "ltx") == INVALID ||
          GETINT("xbitmap", lty,       "lty") == INVALID ||
          GETINT("xbitmap", rbx,       "rbx") == INVALID ||
          GETINT("xbitmap", rby,       "rby") == INVALID ||
          GETINT("xbitmap", fill,      "fill") == INVALID ||
          GETINT("xbitmap", id,        "id") == INVALID ||
          GETINT("xbitmap", rotation,  "rotation") == INVALID ||
          GETINT("xbitmap", image_w,   "image_w") == INVALID ||
          GETINT("xbitmap", image_h,   "image_h") == INVALID ||
          GETINT("xbitmap", rotate,    "rotate") == INVALID ||
          GETINT("xbitmap", flip,      "flip") == INVALID ||
          GETINT("xbitmap", real_type, "real_type") == INVALID ||
          GETINT("xbitmap", llx,       "llx") == INVALID ||
          GETINT("xbitmap", lly,       "lly") == INVALID ||
          GETINT("xbitmap", urx,       "urx") == INVALID ||
          GETINT("xbitmap", ury,       "ury") == INVALID ||
          GETINT("xbitmap", no_bitmap, "no_bitmap") == INVALID ||
          GETINT("xbitmap", locked,    "locked") == INVALID ||
          GETINT("xbitmap", save_epsf, "save_epsf") == INVALID) {
         return;
      }
      if (id >= objId) objId = id+1;
   } else if (fileVersion <= 34) {
      if (GETINT("xbitmap", ltx,         "ltx") == INVALID ||
          GETINT("xbitmap", lty,         "lty") == INVALID ||
          GETINT("xbitmap", rbx,         "rbx") == INVALID ||
          GETINT("xbitmap", rby,         "rby") == INVALID ||
          GETINT("xbitmap", fill,        "fill") == INVALID ||
          GETINT("xbitmap", id,          "id") == INVALID ||
          GETINT("xbitmap", rotation,    "rotation") == INVALID ||
          GETINT("xbitmap", image_w,     "image_w") == INVALID ||
          GETINT("xbitmap", image_h,     "image_h") == INVALID ||
          GETINT("xbitmap", rotate,      "rotate") == INVALID ||
          GETINT("xbitmap", flip,        "flip") == INVALID ||
          GETINT("xbitmap", real_type,   "real_type") == INVALID ||
          GETINT("xbitmap", llx,         "llx") == INVALID ||
          GETINT("xbitmap", lly,         "lly") == INVALID ||
          GETINT("xbitmap", urx,         "urx") == INVALID ||
          GETINT("xbitmap", ury,         "ury") == INVALID ||
          GETINT("xbitmap", no_bitmap,   "no_bitmap") == INVALID ||
          GETINT("xbitmap", locked,      "locked") == INVALID ||
          GETINT("xbitmap", save_epsf,   "save_epsf") == INVALID ||
          GETINT("xbitmap", compressed,  "compressed") == INVALID ||
          GETINT("xbitmap", transformed, "transformed") == INVALID ||
          GETINT("xbitmap", invisible,   "invisible") == INVALID) {
         return;
      }
      if (id >= objId) objId = id+1;
   } else {
      if (GETINT("xbitmap", ltx,         "ltx") == INVALID ||
          GETINT("xbitmap", lty,         "lty") == INVALID ||
          GETINT("xbitmap", rbx,         "rbx") == INVALID ||
          GETINT("xbitmap", rby,         "rby") == INVALID ||
          GETINT("xbitmap", fill,        "fill") == INVALID ||
          GETINT("xbitmap", id,          "id") == INVALID ||
          GETINT("xbitmap", rotation,    "rotation") == INVALID ||
          GETINT("xbitmap", image_w,     "image_w") == INVALID ||
          GETINT("xbitmap", image_h,     "image_h") == INVALID ||
          GETINT("xbitmap", rotate,      "rotate") == INVALID ||
          GETINT("xbitmap", flip,        "flip") == INVALID ||
          GETINT("xbitmap", real_type,   "real_type") == INVALID ||
          GETINT("xbitmap", llx,         "llx") == INVALID ||
          GETINT("xbitmap", lly,         "lly") == INVALID ||
          GETINT("xbitmap", urx,         "urx") == INVALID ||
          GETINT("xbitmap", ury,         "ury") == INVALID ||
          GETINT("xbitmap", no_bitmap,   "no_bitmap") == INVALID ||
          GETINT("xbitmap", locked,      "locked") == INVALID ||
          GETINT("xbitmap", save_epsf,   "save_epsf") == INVALID ||
          GETINT("xbitmap", compressed,  "compressed") == INVALID ||
          GETINT("xbitmap", transformed, "transformed") == INVALID ||
          GETINT("xbitmap", invisible,   "invisible") == INVALID ||
          GETINT("xbitmap", trans_pat,   "trans_pat") == INVALID) {
         return;
      }
      if (id >= objId) objId = id+1;
   }
   if (fileVersion <= 22) {
      image_w = rbx-ltx;
      image_h = rby-lty;
      rotate = ROTATE0;
      flip = 0;
   }
   if (fileVersion >= 24) {
      char *tmp_str, *s, *s1;

      (void)fgets(inbuf, MAXSTRING, FP);
      scanLineNum++;

      tmp_str = FindChar((int)'"', inbuf);
      s = s1 = ReadString(tmp_str);
      s1++;
      *(--s) = '\0';
      len = strlen(tmp_str);
      if (len > 24) {
         (void)sprintf(gszMsgBox,
               TgLoadCachedString(CSTID_FIELD_TOO_LONG_IN_OBJ_TRUNC),
               scanFileName, scanLineNum, "write_date", "xbm");
         if (PRTGIF) {
            fprintf(stderr, "%s\n", gszMsgBox);
         } else {
            Msg(gszMsgBox);
         }
         tmp_str[23] = '\0';
      }
      strcpy(write_date, tmp_str);

      tmp_str = FindChar((int)'"', s1);
      s = ReadString(tmp_str);
      *(--s) = '\0';
      len = strlen(tmp_str);
      filename = (char*)malloc((len+1)*sizeof(char));
      if (filename == NULL) FailAllocMessage();
      strcpy(filename, tmp_str);
      if (save_epsf) {
         if (sscanf(&s[2], "%d", &num_epsf_lines) != 1) {
            sprintf(gszMsgBox,
                  TgLoadCachedString(CSTID_FILE_LINE_BAD_FLD_IN_OBJ_STR),
                  scanFileName, scanLineNum, "num_epsf_lines", "xbm", &s[2]);
            if (PRTGIF) {
               fprintf(stderr, "%s\n", gszMsgBox);
            } else {
               Msg(gszMsgBox);
            }
            return;
         }
         epsflines = (char**)malloc(num_epsf_lines*sizeof(char*));
         if (epsflines == NULL) FailAllocMessage();
         for (i=0; i < num_epsf_lines; i++) {
            if (fgets(inbuf, MAXSTRING, FP) == NULL) {
               sprintf(gszMsgBox,
                     TgLoadString(STID_UNEXPECTED_EOF_IN_ABORT_READ),
                     scanLineNum, scanFileName, "ReadXBmObj()");
               if (PRTGIF) {
                  fprintf(stderr, "%s\n", gszMsgBox);
               } else {
                  MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
               }
               return;
            }
            scanLineNum++;
            len = strlen(inbuf);
            if ((c_ptr=strchr(inbuf, '"')) == NULL) {
               sprintf(gszMsgBox,
                     TgLoadCachedString(CSTID_MALFORMED_LINE_NUM_IN_FILE),
                     scanLineNum, scanFileName);
               if (PRTGIF) {
                  fprintf(stderr, "%s\n", gszMsgBox);
               } else {
                  Msg(gszMsgBox);
               }
               return;
            }
            c_ptr++;
            len -= (c_ptr-inbuf);
            if (c_ptr[len-1] != '\n') {
               int cur_size=2*MAXSTRING-1, done=FALSE;
               char *line=(char*)malloc((cur_size+1)*sizeof(char));

               if (line == NULL) FailAllocMessage();
               strcpy(line, c_ptr);
               c_ptr = &(line[len]);

               while (!done && fgets(inbuf, MAXSTRING, FP) != NULL) {
                  len = strlen(inbuf);
                  if (inbuf[len-1] == '\r' || inbuf[len-1] == '\n') {
                     done = TRUE;
                     inbuf[len-1] = '\0';
                     strcpy(c_ptr, inbuf);
                  } else {
                     int n=c_ptr-line;

                     cur_size += MAXSTRING-1;
                     line = (char*)realloc(line, cur_size+1);
                     c_ptr = line + n;
                     strcpy(c_ptr, inbuf);
                     c_ptr += MAXSTRING-1;
                  }
               }
               if (!done) {
                  sprintf(gszMsgBox,
                        TgLoadString(STID_UNEXPECTED_EOF_IN_ABORT_READ),
                        scanLineNum, scanFileName, "ReadXBmObj()");
                  if (PRTGIF) {
                     fprintf(stderr, "%s\n", gszMsgBox);
                  } else {
                     MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
                  }
                  return;
               }
               epsflines[i] = line;
               len = strlen(line);
            } else {
               c_ptr[--len] = '\0';
               epsflines[i] = (char*)malloc((len+1)*sizeof(char));
               if (epsflines[i] == NULL) FailAllocMessage();
               strncpy(epsflines[i], c_ptr, len);
               epsflines[i][len] = '\0';
            }
            for (c_ptr=(&(epsflines[i])[len]); c_ptr != epsflines[i] &&
                  *c_ptr != '"'; c_ptr--) ;
            if (*c_ptr != '"') {
               sprintf(gszMsgBox,
                     TgLoadCachedString(CSTID_MALFORMED_LINE_NUM_IN_FILE),
                     scanLineNum, scanFileName);
               if (PRTGIF) {
                  fprintf(stderr, "%s\n", gszMsgBox);
               } else {
                  MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
               }
               return;
            }
            *c_ptr = '\0';
         }
      }
   } else {
      *write_date = '\0';
      filename = NULL;
   }
   fill = UpgradePenFill(fill);

   *ObjPtr = (struct ObjRec *)malloc(sizeof(struct ObjRec));
   if (*ObjPtr == NULL) FailAllocMessage();
   memset(*ObjPtr, 0, sizeof(struct ObjRec));
   xbm_ptr = (struct XBmRec *)malloc(sizeof(struct XBmRec));
   if (xbm_ptr == NULL) FailAllocMessage();
   memset(xbm_ptr, 0, sizeof(struct XBmRec));

   color_index = QuickFindColorIndex(*ObjPtr, color_str, &new_alloc, TRUE);

   (*ObjPtr)->color = color_index;
   if (mainDisplay != NULL) {
      UtilStrCpyN((*ObjPtr)->color_str, sizeof((*ObjPtr)->color_str),
            colorMenuItems[color_index]);
   }
   (*ObjPtr)->dirty = FALSE;
   (*ObjPtr)->id = id;
   (*ObjPtr)->rotation = rotation;
   (*ObjPtr)->locked = locked;
   (*ObjPtr)->type = OBJ_XBM;
   (*ObjPtr)->obbox.ltx = (*ObjPtr)->bbox.ltx = (*ObjPtr)->x = ltx;
   (*ObjPtr)->obbox.lty = (*ObjPtr)->bbox.lty = (*ObjPtr)->y = lty;
   (*ObjPtr)->obbox.rbx = (*ObjPtr)->bbox.rbx = rbx;
   (*ObjPtr)->obbox.rby = (*ObjPtr)->bbox.rby = rby;
   (*ObjPtr)->detail.xbm = xbm_ptr;
   (*ObjPtr)->ctm = NULL;
   (*ObjPtr)->invisible = invisible;
   (*ObjPtr)->trans_pat = trans_pat;

   xbm_ptr->bitmap = None;
   xbm_ptr->image = NULL;
   xbm_ptr->cached_bitmap = None;
   xbm_ptr->cached_zoom = 0;
   xbm_ptr->data = NULL;
   xbm_ptr->fill = fill;
   xbm_ptr->flip = flip;
   xbm_ptr->cached_flip = 0;
   xbm_ptr->cached_w = xbm_ptr->cached_h = 0;
   xbm_ptr->image_w = image_w;
   xbm_ptr->image_h = image_h;
   xbm_ptr->llx = llx; xbm_ptr->lly = lly;
   xbm_ptr->urx = urx; xbm_ptr->ury = ury;

   xbm_ptr->real_type = real_type;
   strcpy(xbm_ptr->write_date, write_date);
   xbm_ptr->filename = filename;
   xbm_ptr->epsflines = epsflines;
   xbm_ptr->num_epsf_lines = num_epsf_lines;
   xbm_ptr->save_epsf = save_epsf;

   if (no_bitmap && xbm_ptr->real_type==XBM_EPS) {
      float file_llx=0.0, file_lly=0.0, file_urx=0.0, file_ury=0.0;

      xbm_ptr->eps_w = xbm_ptr->image_w;
      xbm_ptr->eps_h = xbm_ptr->image_h;
      xbm_ptr->image_w = 0;
      xbm_ptr->image_h = 0;

      if (!(PRTGIF && !cmdLineOpenDisplay)) {
         transform_read = TRUE;
         if (!ReadTransformAndAdjustForXBm(FP, ObjPtr, xbm_ptr, transformed,
               rotate, flip)) {
            return;
         }
         if (MyReadEPSFile(xbm_ptr->filename, &image_w,  &image_h,
               &(xbm_ptr->bitmap), &(xbm_ptr->image),
               (save_epsf ? &(xbm_ptr->num_epsf_lines) : NULL),
               (save_epsf ? &(xbm_ptr->epsflines) : NULL),
               &(xbm_ptr->epsf_level), &file_llx, &file_lly, &file_urx,
               &file_ury, xbm_ptr->write_date) != BitmapSuccess) {
            return;
         } else if (xbm_ptr->bitmap == None) {
            xbm_ptr->eps_w = (file_urx >= file_llx) ?
                  (int)(file_urx-file_llx) : (int)(file_llx-file_urx);
            xbm_ptr->eps_h = (file_ury >= file_lly) ?
                  (int)(file_ury-file_lly) : (int)(file_lly-file_ury);
         } else {
            xbm_ptr->image_w = image_w;
            xbm_ptr->image_h = image_h;
            /* same as above! */
            xbm_ptr->eps_w = (file_urx >= file_llx) ?
                  (int)(file_urx-file_llx) : (int)(file_llx-file_urx);
            xbm_ptr->eps_h = (file_ury >= file_lly) ?
                  (int)(file_ury-file_lly) : (int)(file_lly-file_ury);
         }
         xbm_ptr->llx = (int)(file_llx*1000.0);
         xbm_ptr->lly = (int)(file_lly*1000.0);
         xbm_ptr->urx = (int)(file_urx*1000.0);
         xbm_ptr->ury = (int)(file_ury*1000.0);
      }
   } else {
      if (xbm_ptr->real_type == XBM_EPS) {
         float file_llx=((float)xbm_ptr->llx)/1000.0;
         float file_lly=((float)xbm_ptr->lly)/1000.0;
         float file_urx=((float)xbm_ptr->urx)/1000.0;
         float file_ury=((float)xbm_ptr->ury)/1000.0;

         /* same as above! */
         xbm_ptr->eps_w = (file_urx >= file_llx) ?
               (int)(file_urx-file_llx) : (int)(file_llx-file_urx);
         xbm_ptr->eps_h = (file_ury >= file_lly) ?
               (int)(file_ury-file_lly) : (int)(file_lly-file_ury);
      } else {
         xbm_ptr->eps_w = xbm_ptr->eps_h = 0;
      }
      num_nibbles = ((image_w % 4) == 0) ? (int)(image_w>>2) :
            (int)(image_w>>2)+1;

      (void)fgets(inbuf, MAXSTRING, FP);
      scanLineNum++;
      c_ptr = &inbuf[5];
      nibble_count = 0;

      if (PRTGIF && !cmdLineOpenDisplay) {
         xbm_data = (char*)malloc((image_h*num_nibbles)*sizeof(char));
         if (xbm_data == NULL) FailAllocMessage();
         for (i = 0; i < image_h; i++) {
            for (j = 0; j < num_nibbles; j++) {
               if (nibble_count++ == 64) {
                  (void)fgets(inbuf, MAXSTRING, FP);
                  scanLineNum++;
                  c_ptr = &inbuf[5];
                  nibble_count = 1;
               }
               xbm_data[i*num_nibbles+j] = *c_ptr++;
            }
         }
         xbm_ptr->data = xbm_data;
      } else {
         bitmap = XCreatePixmap(mainDisplay, dummyBitmap, image_w, image_h, 1);
         XFillRectangle(mainDisplay, bitmap, xbmGC, 0, 0, image_w, image_h);
         image = XGetImage(mainDisplay,bitmap,0,0,image_w,image_h,1,ZPixmap);

         for (i = 0; i < image_h; i++) {
            bit_count = 0;
            for (j = 0; j < num_nibbles; j++) {
               if (nibble_count++ == 64) {
                  (void)fgets(inbuf, MAXSTRING, FP);
                  scanLineNum++;
                  c_ptr = &inbuf[5];
                  nibble_count = 1;
               }
               if (*c_ptr >= '0' && *c_ptr <= '9') {
                  data = (int)(*c_ptr++) - (int)('0');
               } else if (*c_ptr >= 'a' && *c_ptr <= 'f') {
                  data = (int)(*c_ptr++) - (int)('a') + 10;
               }
               for (k = 0; k < 4; k++) {
                  if (bit_count++ == image_w) break;

                  if (data & (1<<(3-k))) {
                     XPutPixel(image, j*4+k, i, 1);
                  }
               }
            }
         }
         XPutImage(mainDisplay,bitmap,xbmGC,image,0,0,0,0,image_w,image_h);
         xbm_ptr->bitmap = bitmap;
         xbm_ptr->image = image;
      }
   }
   if (!transform_read && !ReadTransformAndAdjustForXBm(FP, ObjPtr, xbm_ptr,
         transformed, rotate, flip)) {
      return;
   }
   if (!transform_read && (!(PRTGIF && !cmdLineOpenDisplay)) &&
         (*ObjPtr)->ctm == NULL) {
      struct BBRec orig_obbox;

      SetBBRec(&orig_obbox, ltx, lty, rbx, rby);
      SetEPSObjCTM(*ObjPtr, &orig_obbox);
   }
}

void SetXBmPropMask(ObjPtr, plMask, plSkip, pProp)
   struct ObjRec *ObjPtr;
   long *plMask, *plSkip;
   struct PropertiesRec *pProp;
{
   struct XBmRec *xbm_ptr=ObjPtr->detail.xbm;

   SetCTMPropertyMask(ObjPtr->ctm, plMask, plSkip, pProp);

   if (xbm_ptr->real_type != XBM_EPS) {
      SetIntPropertyMask(PROP_MASK_COLOR, ObjPtr->color,
            colorMenuItems[ObjPtr->color], plMask, plSkip, pProp);
   }
}

void FreeXBmObj(ObjPtr)
   struct ObjRec *ObjPtr;
{
   register struct XBmRec *xbm_ptr=ObjPtr->detail.xbm;
   register int i;

   if (xbm_ptr->bitmap != None) XFreePixmap(mainDisplay, xbm_ptr->bitmap);
   if (xbm_ptr->image != NULL) XDestroyImage(xbm_ptr->image);
   if (xbm_ptr->cached_bitmap != None) {
      XFreePixmap(mainDisplay, xbm_ptr->cached_bitmap);
   }
   xbm_ptr->bitmap = None;
   xbm_ptr->image = NULL;
   xbm_ptr->cached_bitmap = None;
   xbm_ptr->cached_zoom = 0;
   if (xbm_ptr->data != NULL) free(xbm_ptr->data);
   if (xbm_ptr->filename != NULL) free(xbm_ptr->filename);
   if (xbm_ptr->real_type == XBM_EPS) {
      for (i = 0; i < xbm_ptr->num_epsf_lines; i++) {
         if (xbm_ptr->epsflines[i] != NULL) {
            free(xbm_ptr->epsflines[i]);
         }
      }
      if (xbm_ptr->epsflines != NULL) free(xbm_ptr->epsflines);
   }
   free(xbm_ptr);
   free(ObjPtr);
}
