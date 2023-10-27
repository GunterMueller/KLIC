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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/imgproc.c,v 1.82 2011/05/25 16:54:27 cvsps Exp $
 */

#define _INCLUDE_FROM_IMGPROC_C_

#include "tgifdefs.h"
#include "cmdids.h"

#include "auxtext.e"
#include "choice.e"
#include "cmd.e"
#include "color.e"
#include "cursor.e"
#include "dialog.e"
#include "drawing.e"
#include "dup.e"
#include "file.e"
#include "grid.e"
#include "hash.e"
#include "imgproc.e"
#include "list.e"
#include "mainloop.e"
#include "mainmenu.e"
#include "mark.e"
#include "menu.e"
#include "menuinfo.e"
#include "move.e"
#include "msg.e"
#include "names.e"
#include "navigate.e"
#include "obj.e"
#include "page.e"
#include "pngtrans.e"
#include "poly.e"
#include "polygon.e"
#include "raster.e"
#include "rect.e"
#include "remote.e"
#include "ruler.e"
#include "select.e"
#include "setup.e"
#include "special.e"
#include "strtbl.e"
#include "util.e"
#include "xbitmap.e"
#include "xpixmap.e"
#include "z_intrf.e"

#define HISTOGRAMCOUNT(i) (gpHistogram[(i)].pixel)
#define HISTOGRAMRED(i) (gpHistogram[(i)].red)
#define HISTOGRAMGREEN(i) (gpHistogram[(i)].green)
#define HISTOGRAMBLUE(i) (gpHistogram[(i)].blue)

#define MAXIMAGEPROCS 34

int numImageProc=MAXIMAGEPROCS;
int gnInImageProc=FALSE;
int gnConvolving=FALSE;
int gnNumNewColorsInPixmapFile=0;

char gszImageProcXPmFile[MAXPATHLENGTH+1];

float gfVectorWarpSoftness=2.0;

int threshFillReplaceEnabled=FALSE;
int fillReplaceRedThresh=15;
int fillReplaceGreenThresh=15;
int fillReplaceBlueThresh=15;

static NLFN *gpImageMapColorFunc=NULL;
static NLFN *gpConvolveFunc=NULL;
static int gpConvolveCmdID=(-1);
static int gnCombining=FALSE;

static int gnCombineW=0, gnCombineH=0;

static char bggenToXpmCmd[MAXSTRING+1];
static char bggenToPpm6Cmd[MAXSTRING+1];
static char ppmquantCmd[MAXSTRING+1];
static char ppmFSquantCmd[MAXSTRING+1];

static XColor gDefErrorDiffuseLevel;

static struct ObjRec *gpFgObj=NULL, *gpBgObj=NULL, *gpAlphaObj=NULL;
static struct BBRec gTotalCombineBBox;
static struct BBRec gFgCombineBBox, gBgCombineBBox, gAlphaCombineBBox;
static XImage *gpFgImage=NULL, *gpBgImage=NULL, *gpAlphaImage=NULL;
static XImage *gpFgBitmapImage=NULL, *gpBgBitmapImage=NULL,
      *gpAlphaBitmapImage=NULL;

typedef struct tagConvExtraInfo {
   FILE *fp; /* non-NULL only if (fullTrueColorMode && HasZlibSupport()) */
   XColor my_bg_xcolor;
   int alpha_combine;
   /* if alpha_combine is TRUE, these are for the foreground object */
   int image_w, image_h;
   XImage *image, *bitmap_image;
   XColor **xcolors;
   /* if alpha_combine is not TRUE, everything below are not used */
   int final_w, final_h;
   int alpha_image_w, alpha_image_h, bg_image_w, bg_image_h;
   XImage *alpha_image, *alpha_bitmap_image, *bg_image, *bg_bitmap_image;
   XColor **alpha_xcolors, **bg_xcolors;
} ConvExtraInfo;

static ConvExtraInfo gConvExtraInfo;
static TrueColorInfo gTrueColorInfo;

typedef struct tagThreshFillReplaceInfo {
   int use_thresholding;
   /* for true color */
   int min_r, max_r, min_g, max_g, min_b, max_b;
   TrueColorInfo tci;
   /* for non-true color */
   TgHash hash_table;
   int *within_threshold;
} ThreshFillReplaceInfo;

static ThreshFillReplaceInfo gThreshFillReplaceInfo;

static
int ParseThreshFillReplaceSpec(spec)
   char *spec;
{
   char *psz=NULL, sz_spec_copy[MAXSTRING];
   int red=0, green=0, blue=0;

   *sz_spec_copy = '\0';
   UtilStrCpyN(sz_spec_copy, sizeof(sz_spec_copy), spec);
   UtilTrimBlanks(sz_spec_copy);

   if ((psz=strtok(sz_spec_copy, " ,\t\n\r")) == NULL ||
         sscanf(psz, "%d", &red) != 1) {
      sprintf(gszMsgBox, TgLoadString(STID_ERR_PARSE_THRESH_COLORS), spec);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return FALSE;
   }
   if (red < 0) red = 0;
   if (red > 255) red = 255;

   psz = strtok(NULL, " ,\t\n\r");
   if (psz == NULL) {
      fillReplaceRedThresh = fillReplaceGreenThresh = fillReplaceBlueThresh =
            red;
      return TRUE;
   }
   if (sscanf(psz, "%d", &green) != 1) {
      sprintf(gszMsgBox, TgLoadString(STID_ERR_PARSE_THRESH_COLORS), spec);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return FALSE;
   }
   if (green < 0) green = 0;
   if (green > 255) green = 255;

   psz = strtok(NULL, " ,\t\n\r");
   if (psz == NULL || sscanf(psz, "%d", &blue) != 1) {
      sprintf(gszMsgBox, TgLoadString(STID_ERR_PARSE_THRESH_COLORS), spec);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return FALSE;
   }
   if (blue < 0) blue = 0;
   if (blue > 255) blue = 255;

   psz = strtok(NULL, " ,\t\n\r");
   if (psz != NULL) {
      sprintf(gszMsgBox, TgLoadString(STID_ERR_PARSE_THRESH_COLORS), spec);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return FALSE;
   }
   fillReplaceRedThresh = red;
   fillReplaceGreenThresh = green;
   fillReplaceBlueThresh = blue;

   return TRUE;
}

static
void ResetThreshFillReplaceInfo()
{
   if (threshFillReplaceEnabled) {
      gThreshFillReplaceInfo.use_thresholding = TRUE;
      if (fullTrueColorMode) {
         SetupTrueColorInfo(&gThreshFillReplaceInfo.tci);
      }
   } else {
      gThreshFillReplaceInfo.use_thresholding = FALSE;
   }
}

static
int TrueColorPixelWithinRange(pixel, ptfri)
   int pixel;
   ThreshFillReplaceInfo *ptfri;
{
   uint32_t pix=(uint32_t)pixel;
   unsigned int r=0, g=0, b=0;
   double dr=(double)0, dg=(double)0, db=(double)0;
   TrueColorInfo *ptci=(&ptfri->tci);
   XColor xcolor;

   r = (pix & ptci->r_mask) >> ptci->r_shift;
   g = (pix & ptci->g_mask) >> ptci->g_shift;
   b = (pix & ptci->b_mask) >> ptci->b_shift;
   dr = ((double)r) / ptci->dr_maxval_div255;
   dg = ((double)g) / ptci->dg_maxval_div255;
   db = ((double)b) / ptci->db_maxval_div255;
   xcolor.red = round(dr);
   xcolor.green = round(dg);
   xcolor.blue = round(db);
   if (xcolor.red > 255) xcolor.red = 255;
   if (xcolor.green > 255) xcolor.green = 255;
   if (xcolor.blue > 255) xcolor.blue = 255;

   return (xcolor.red >= ptfri->min_r && xcolor.red <= ptfri->max_r &&
         xcolor.green >= ptfri->min_g && xcolor.green <= ptfri->max_g &&
         xcolor.blue >= ptfri->min_b && xcolor.blue <= ptfri->max_b);
}

static
int OtherPixelWithinRange(pixel, ptfri)
   int pixel;
   ThreshFillReplaceInfo *ptfri;
{
   double dr=(double)0, dg=(double)0, db=(double)0;
   XColor xcolor;
   int global_color_index=(-1);

   if (!HashLookUpInt(&ptfri->hash_table, (char*)(&pixel), sizeof(int),
         &global_color_index)) {
      return FALSE;
   }
   if (global_color_index == (-1)) {
      return FALSE;
   }
   memcpy(&xcolor, &tgifColors[global_color_index], sizeof(XColor));

   dr = ((double)xcolor.red) / ((double)maxRGB) * ((double)255);
   dg = ((double)xcolor.green) / ((double)maxRGB) * ((double)255);
   db = ((double)xcolor.blue) / ((double)maxRGB) * ((double)255);
   xcolor.red = round(dr);
   xcolor.green = round(dg);
   xcolor.blue = round(db);
   if (xcolor.red > 255) xcolor.red = 255;
   if (xcolor.green > 255) xcolor.green = 255;
   if (xcolor.blue > 255) xcolor.blue = 255;

   return (xcolor.red >= ptfri->min_r && xcolor.red <= ptfri->max_r &&
         xcolor.green >= ptfri->min_g && xcolor.green <= ptfri->max_g &&
         xcolor.blue >= ptfri->min_b && xcolor.blue <= ptfri->max_b);
}

static
int GetPixelRGB(ptfri, pixel, pxc_pixel)
   ThreshFillReplaceInfo *ptfri;
   int pixel;
   XColor *pxc_pixel;
{
   if (fullTrueColorMode) {
      uint32_t pix=(uint32_t)pixel;
      unsigned int r=0, g=0, b=0;
      double dr=(double)0, dg=(double)0, db=(double)0;
      TrueColorInfo *ptci=(&ptfri->tci);
      XColor xcolor;

      r = (pix & ptci->r_mask) >> ptci->r_shift;
      g = (pix & ptci->g_mask) >> ptci->g_shift;
      b = (pix & ptci->b_mask) >> ptci->b_shift;
      dr = ((double)r) / ptci->dr_maxval_div255;
      dg = ((double)g) / ptci->dg_maxval_div255;
      db = ((double)b) / ptci->db_maxval_div255;
      pxc_pixel->red = round(dr);
      pxc_pixel->green = round(dg);
      pxc_pixel->blue = round(db);
      if (pxc_pixel->red > 255) pxc_pixel->red = 255;
      if (pxc_pixel->green > 255) pxc_pixel->green = 255;
      if (pxc_pixel->blue > 255) xcolor.blue = 255;
   } else {
      int i=0, global_color_index=(-1);

      for (i=0; i < maxColors; i++) {
         if (pixel == colorPixels[i]) {
            global_color_index = i;
            break;
         }
      }
      if (global_color_index != (-1)) {
         double dr=(double)0, dg=(double)0, db=(double)0;

         memcpy(pxc_pixel, &tgifColors[global_color_index], sizeof(XColor));
         dr = ((double)pxc_pixel->red) / ((double)maxRGB) * ((double)255);
         dg = ((double)pxc_pixel->green) / ((double)maxRGB) * ((double)255);
         db = ((double)pxc_pixel->blue) / ((double)maxRGB) * ((double)255);
         pxc_pixel->red = round(dr);
         pxc_pixel->green = round(dg);
         pxc_pixel->blue = round(db);
         if (pxc_pixel->red > 255) pxc_pixel->red = 255;
         if (pxc_pixel->green > 255) pxc_pixel->green = 255;
         if (pxc_pixel->blue > 255) pxc_pixel->blue = 255;
      } else {
#ifdef _TGIF_DBG /* debug, do not translate */
         fprintf(stderr,
               "Canot find pixel indel for pixel=0x%08x in GetPixelRGB().\n",
               pixel);
#endif /* _TGIF_DBG */
         return FALSE;
      }
   }
   return TRUE;
}

static
int SetupThreshFillReplaceInfo(ptfri, pixel, pxc_pixel)
   ThreshFillReplaceInfo *ptfri;
   int pixel;
   XColor *pxc_pixel;
{
   if (fullTrueColorMode) {
      if (!GetPixelRGB(ptfri, pixel, pxc_pixel)) return FALSE;

      ptfri->min_r = (int)(pxc_pixel->red) - fillReplaceRedThresh;
      ptfri->max_r = (int)(pxc_pixel->red) + fillReplaceRedThresh;
      if (ptfri->min_r < 0) ptfri->min_r = 0;
      if (ptfri->max_r > 255) ptfri->max_r = 255;

      ptfri->min_g = (int)(pxc_pixel->green) - fillReplaceGreenThresh;
      ptfri->max_g = (int)(pxc_pixel->green) + fillReplaceGreenThresh;
      if (ptfri->min_g < 0) ptfri->min_g = 0;
      if (ptfri->max_g > 255) ptfri->max_g = 255;

      ptfri->min_b = (int)(pxc_pixel->blue) - fillReplaceBlueThresh;
      ptfri->max_b = (int)(pxc_pixel->blue) + fillReplaceBlueThresh;
      if (ptfri->min_b < 0) ptfri->min_b = 0;
      if (ptfri->max_b > 255) ptfri->max_b = 255;
   } else {
      int i=0;
      TgHash *pth=(&ptfri->hash_table);
      int global_color_index=(-1);

      InitHash(pth, TG_HASH_SIZE_LARGE);
      for (i=0; i < maxColors; i++) {
         HashStoreInt(pth, (char*)(&colorPixels[i]), sizeof(int), i);
      }
      if (HashLookUpInt(pth, (char*)(&pixel), sizeof(int),
            &global_color_index)) {
         double dr=(double)0, dg=(double)0, db=(double)0;
         XColor xcolor;

         ptfri->within_threshold = (int*)malloc(maxColors*sizeof(int));
         if (ptfri->within_threshold == NULL) FailAllocMessage();
         memset(ptfri->within_threshold, 0, maxColors*sizeof(int));

         memcpy(&xcolor, &tgifColors[global_color_index], sizeof(XColor));
         dr = ((double)xcolor.red) / ((double)maxRGB) * ((double)255);
         dg = ((double)xcolor.green) / ((double)maxRGB) * ((double)255);
         db = ((double)xcolor.blue) / ((double)maxRGB) * ((double)255);
         xcolor.red = round(dr);
         xcolor.green = round(dg);
         xcolor.blue = round(db);
         if (xcolor.red > 255) xcolor.red = 255;
         if (xcolor.green > 255) xcolor.green = 255;
         if (xcolor.blue > 255) xcolor.blue = 255;

         ptfri->min_r = (int)(xcolor.red) - fillReplaceRedThresh;
         ptfri->max_r = (int)(xcolor.red) + fillReplaceRedThresh;
         if (ptfri->min_r < 0) ptfri->min_r = 0;
         if (ptfri->max_r > 255) ptfri->max_r = 255;

         ptfri->min_g = (int)(xcolor.green) - fillReplaceGreenThresh;
         ptfri->max_g = (int)(xcolor.green) + fillReplaceGreenThresh;
         if (ptfri->min_g < 0) ptfri->min_g = 0;
         if (ptfri->max_g > 255) ptfri->max_g = 255;

         ptfri->min_b = (int)(xcolor.blue) - fillReplaceBlueThresh;
         ptfri->max_b = (int)(xcolor.blue) + fillReplaceBlueThresh;
         if (ptfri->min_b < 0) ptfri->min_b = 0;
         if (ptfri->max_b > 255) ptfri->max_b = 255;

         for (i=0; i < maxColors; i++) {
            ptfri->within_threshold[i] =
                  OtherPixelWithinRange(colorPixels[i], ptfri);
         }
      } else {
#ifdef _TGIF_DBG /* debug, do not translate */
         fprintf(stderr,
               "HashLookup() failed in SetupThreshFillReplaceInfo() for pixel=0x%08x.\n",
               pixel);
#endif /* _TGIF_DBG */
         return FALSE;
      }
   }
   return TRUE;
}

static
int DoPpm6(xpm_ptr)
   struct XPmRec *xpm_ptr;
{
   if (gnCombining || xpm_ptr == NULL) {
      return (fullTrueColorMode && HasZlibSupport());
   } else if (xpm_ptr != NULL && (xpm_ptr->real_type == PPM_TRUE ||
         xpm_ptr->real_type == XPM_JPEG) && xpm_ptr->ppm_mask_data == NULL &&
         fullTrueColorMode && HasZlibSupport()) {
      return TRUE;
   }
   return FALSE;
}

static
int FreePreprocessPixels(h, xcolors)
   int h;
   XColor **xcolors;
{
   int i=0;

   for (i=0; i < h; i++) {
      if (xcolors[i] != NULL) {
         free(xcolors[i]);
      } else {
         break;
      }
   }
   free(xcolors);
   gConvExtraInfo.xcolors = NULL;

   return FALSE;
}

static
void ClearConvExtraInfo()
{
   if (gConvExtraInfo.xcolors != NULL) {
      FreePreprocessPixels(gConvExtraInfo.image_h, gConvExtraInfo.xcolors);
   }
   if (gConvExtraInfo.alpha_xcolors != NULL) {
      FreePreprocessPixels(gConvExtraInfo.alpha_image_h,
            gConvExtraInfo.alpha_xcolors);
   }
   if (gConvExtraInfo.bg_xcolors != NULL) {
      FreePreprocessPixels(gConvExtraInfo.bg_image_h,
            gConvExtraInfo.bg_xcolors);
   }
   memset(&gConvExtraInfo, 0, sizeof(ConvExtraInfo));
}

static
XColor **PreprocessOneImagePixels(w, h, image, bitmap_image)
   int w, h;
   XImage *image, *bitmap_image;
{
   int i=0;
   XColor **xcolors=NULL;
   ProgressInfo pi;

   xcolors = (XColor**)malloc(h*sizeof(XColor*));
   if (xcolors == NULL) {
      FailAllocMessage();
      return NULL;
   }
   memset(xcolors, 0, h*sizeof(XColor*));

   for (i=0; i < h; i++) {
      xcolors[i] = (XColor*)malloc(w*sizeof(XColor));
      if (xcolors[i] == NULL) {
         FailAllocMessage();
         FreePreprocessPixels(h, xcolors);
         return NULL;
      }
      memset(xcolors[i], 0, w*sizeof(XColor));
   }
   BeginProgress(&pi, h);
   for (i=0; i < h; i++) {
      int j=0;

      UpdateProgress(&pi, i);
      for (j=0; j < w; j++) {
         if (bitmap_image != NULL && XGetPixel(bitmap_image,j,i) == 0) {
            TgAssert(FALSE, "transparent pixel not supported", NULL);
            FreePreprocessPixels(h, xcolors);
            return NULL;
         } else {
            unsigned int pixel=(unsigned int)XGetPixel(image,j,i);
            uint32_t pix=(uint32_t)pixel;
            unsigned int r=0, g=0, b=0;
            double dr=(double)0, dg=(double)0, db=(double)0;

            r = (pix & gTrueColorInfo.r_mask) >> gTrueColorInfo.r_shift;
            g = (pix & gTrueColorInfo.g_mask) >> gTrueColorInfo.g_shift;
            b = (pix & gTrueColorInfo.b_mask) >> gTrueColorInfo.b_shift;
            dr = ((double)r) / gTrueColorInfo.dr_maxval_div255;
            dg = ((double)g) / gTrueColorInfo.dg_maxval_div255;
            db = ((double)b) / gTrueColorInfo.db_maxval_div255;
            xcolors[i][j].red = round(dr);
            xcolors[i][j].green = round(dg);
            xcolors[i][j].blue = round(db);
            if (xcolors[i][j].red > 255) xcolors[i][j].red = 255;
            if (xcolors[i][j].green > 255) xcolors[i][j].green = 255;
            if (xcolors[i][j].blue > 255) xcolors[i][j].blue = 255;
            xcolors[i][j].pixel = pixel;
         }
      }
   }
   return xcolors;
}

static
int PreprocessPixels()
{
   if (gConvExtraInfo.alpha_combine) {
      memset(&gConvExtraInfo.my_bg_xcolor, 0, sizeof(XColor));
      unsigned int r=(unsigned int)myBgColor.red;
      unsigned int g=(unsigned int)myBgColor.green;
      unsigned int b=(unsigned int)myBgColor.blue;
      double dr=((double)r)/((double)maxRGB)*((double)255);
      double dg=((double)g)/((double)maxRGB)*((double)255);
      double db=((double)b)/((double)maxRGB)*((double)255);

      gConvExtraInfo.my_bg_xcolor.red = round(dr);
      gConvExtraInfo.my_bg_xcolor.green = round(dg);
      gConvExtraInfo.my_bg_xcolor.blue = round(db);
      gConvExtraInfo.my_bg_xcolor.pixel = myBgPixel;

      /* foreground object */
      gConvExtraInfo.xcolors = PreprocessOneImagePixels(
            gConvExtraInfo.image_w, gConvExtraInfo.image_h,
            gConvExtraInfo.image, gConvExtraInfo.bitmap_image);
      if (gConvExtraInfo.xcolors == NULL) {
         return FALSE;
      }
      /* background object */
      gConvExtraInfo.bg_xcolors = PreprocessOneImagePixels(
            gConvExtraInfo.bg_image_w, gConvExtraInfo.bg_image_h,
            gConvExtraInfo.bg_image, gConvExtraInfo.bg_bitmap_image);
      if (gConvExtraInfo.bg_xcolors == NULL) {
         return FALSE;
      }
      if (gpConvolveCmdID == CMDID_ALPHACOMBINE) {
         gConvExtraInfo.alpha_xcolors = PreprocessOneImagePixels(
               gConvExtraInfo.alpha_image_w, gConvExtraInfo.alpha_image_h,
               gConvExtraInfo.alpha_image, gConvExtraInfo.alpha_bitmap_image);
         if (gConvExtraInfo.alpha_xcolors == NULL) {
            return FALSE;
         }
      }
   } else {
      gConvExtraInfo.xcolors = PreprocessOneImagePixels(gConvExtraInfo.image_w,
            gConvExtraInfo.image_h, gConvExtraInfo.image,
            gConvExtraInfo.bitmap_image);
      if (gConvExtraInfo.xcolors == NULL) {
         return FALSE;
      }
      if (gpConvolveCmdID == CMDID_VECTORWARP) {
         gConvExtraInfo.bg_xcolors = PreprocessOneImagePixels(
               gConvExtraInfo.image_w, gConvExtraInfo.image_h,
               gConvExtraInfo.image, gConvExtraInfo.bitmap_image);
         if (gConvExtraInfo.bg_xcolors == NULL) {
            return FALSE;
         }
      }
   }
   return TRUE;
}

static
int SetConvExtraInfo(fp, image_w, image_h, image, bitmap_image)
   FILE *fp;
   int image_w, image_h;
   XImage *image, *bitmap_image;
{
   memset(&gConvExtraInfo, 0, sizeof(ConvExtraInfo));
   gConvExtraInfo.fp = fp;
   gConvExtraInfo.image_w = gConvExtraInfo.bg_image_w = image_w;
   gConvExtraInfo.image_h = gConvExtraInfo.bg_image_h = image_h;
   gConvExtraInfo.image = image;
   gConvExtraInfo.bitmap_image = bitmap_image;

   switch (gpConvolveCmdID) {
   case CMDID_EDGEDETECT:
   case CMDID_EMBOSS:
   case CMDID_SPREAD:
   case CMDID_SHARPEN:
   case CMDID_BLUR3:
   case CMDID_VECTORWARP:
      if (!PreprocessPixels()) {
         return FALSE;
      }
      break;
   case CMDID_SUBTRACT:
   case CMDID_XORCOLORS:
      gConvExtraInfo.alpha_combine = TRUE;

      gConvExtraInfo.final_w = gnCombineW;
      gConvExtraInfo.final_h = gnCombineH;

      gConvExtraInfo.image_w = gFgCombineBBox.rbx-gFgCombineBBox.ltx;
      gConvExtraInfo.image_h = gFgCombineBBox.rby-gFgCombineBBox.lty;
      gConvExtraInfo.image = gpFgImage;
      gConvExtraInfo.bitmap_image = gpFgBitmapImage;

      if (!InitTrueColorInfo(gConvExtraInfo.image, &gTrueColorInfo,
            gConvExtraInfo.image_w)) {
         sprintf(gszMsgBox, TgLoadString(STID_CANNOT_GET_IMAGE_OF_SIZE),
               gConvExtraInfo.image_w, gConvExtraInfo.image_h);
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
         return FALSE;
      }
      gConvExtraInfo.bg_image_w = gBgCombineBBox.rbx-gBgCombineBBox.ltx;
      gConvExtraInfo.bg_image_h = gBgCombineBBox.rby-gBgCombineBBox.lty;
      gConvExtraInfo.bg_image = gpBgImage;
      gConvExtraInfo.bg_bitmap_image = gpBgBitmapImage;
      if (!PreprocessPixels()) {
         return FALSE;
      }
      break;
   case CMDID_ALPHACOMBINE:
      gConvExtraInfo.alpha_combine = TRUE;

      gConvExtraInfo.final_w = gnCombineW;
      gConvExtraInfo.final_h = gnCombineH;

      gConvExtraInfo.image_w = gFgCombineBBox.rbx-gFgCombineBBox.ltx;
      gConvExtraInfo.image_h = gFgCombineBBox.rby-gFgCombineBBox.lty;
      gConvExtraInfo.image = gpFgImage;
      gConvExtraInfo.bitmap_image = gpFgBitmapImage;

      if (!InitTrueColorInfo(gConvExtraInfo.image, &gTrueColorInfo,
            gConvExtraInfo.image_w)) {
         sprintf(gszMsgBox, TgLoadString(STID_CANNOT_GET_IMAGE_OF_SIZE),
               gConvExtraInfo.image_w, gConvExtraInfo.image_h);
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
         return FALSE;
      }
      gConvExtraInfo.alpha_image_w =
            gAlphaCombineBBox.rbx-gAlphaCombineBBox.ltx;
      gConvExtraInfo.alpha_image_h =
            gAlphaCombineBBox.rby-gAlphaCombineBBox.lty;
      gConvExtraInfo.alpha_image = gpAlphaImage;
      gConvExtraInfo.alpha_bitmap_image = gpAlphaBitmapImage;

      gConvExtraInfo.bg_image_w = gBgCombineBBox.rbx-gBgCombineBBox.ltx;
      gConvExtraInfo.bg_image_h = gBgCombineBBox.rby-gBgCombineBBox.lty;
      gConvExtraInfo.bg_image = gpBgImage;
      gConvExtraInfo.bg_bitmap_image = gpBgBitmapImage;
      if (!PreprocessPixels()) {
         return FALSE;
      }
      break;
   default: break;
   }
   return TRUE;
}

static
char *GetImageProcName(nCmdId)
   int nCmdId;
{
   TgMenuItemInfo *item_info=NULL;

   for (item_info=imageProcMenuInfo.items; item_info->menu_str != NULL;
         item_info++) {
      if (item_info->menu_str != TGMUITEM_SEPARATOR &&
            item_info->cmdid == nCmdId) {
         /* must use gettext() here */
         return _(item_info->menu_str);
      } else if (item_info->shortcut_str == TGMUITEM_SUBMENU) {
         TgMenuInfo *submenu_info=item_info->submenu_info;
         TgMenuItemInfo *submenu_item_info=NULL;

         for (submenu_item_info=submenu_info->items;
               submenu_item_info->menu_str != NULL;
               submenu_item_info++) {
            if (submenu_item_info->menu_str != TGMUITEM_SEPARATOR &&
                  submenu_item_info->cmdid == nCmdId) {
               /* must use gettext() here */
               return _(submenu_item_info->menu_str);
            }
         }
      }
   }
   return TgLoadCachedString(CSTID_PARANED_UNKNOWN);
}

static
int TrueColorTransPixelCheck(obj_ptr, cmdid)
   struct ObjRec *obj_ptr;
   int cmdid;
{
   if (ObjHasTrueColorTransPixel(obj_ptr, NULL, NULL, NULL)) {
      sprintf(gszMsgBox, TgLoadString(STID_BAD_CMD_FOR_TRANS_PPMTRUE),
            GetImageProcName(cmdid));
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return TRUE;
   }
   return FALSE;
}

static
int CurColorIsTranscolor(trans_color_r, trans_color_g, trans_color_b)
   unsigned char trans_color_r, trans_color_g, trans_color_b;
{
   TrueColorInfo tci;
   unsigned int r=0, g=0, b=0;
   double dr=(double)0, dg=(double)0, db=(double)0;
   uint32_t pixel=0;
   double dmaxval=(double)255;

   if (!SetupTrueColorInfo(&tci)) return FALSE;

   r = (unsigned int)trans_color_r;
   g = (unsigned int)trans_color_g;
   b = (unsigned int)trans_color_b;
   dr = ((double)r) / dmaxval * tci.dr_maxval;
   dg = ((double)g) / dmaxval * tci.dg_maxval;
   db = ((double)b) / dmaxval * tci.db_maxval;
   r = round(dr);
   g = round(dg);
   b = round(db);
   pixel = ((r << tci.r_shift) & mainVisual->red_mask) |
           ((g << tci.g_shift) & mainVisual->green_mask) |
           ((b << tci.b_shift) & mainVisual->blue_mask) ;

   return (((int)pixel) == colorPixels[colorIndex]);
}

static
int CheckSelectionForImageProc(cmdid)
   int cmdid;
{
   char szBuf[MAXSTRING+1];

   UtilStrCpyN(szBuf, sizeof(szBuf), GetImageProcName(cmdid));
   if (topSel == NULL || topSel != botSel || topSel->obj->type != OBJ_XPM) {
      sprintf(gszMsgBox, TgLoadString(STID_SINGLE_XPM_IMGPROC), szBuf);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return FALSE;
   }
   if (topSel->obj->fattr != NULL) {
      sprintf(gszMsgBox, TgLoadString(STID_XPM_HAS_ATTR_IMGPROC), szBuf);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return FALSE;
   }
   return TRUE;
}

#define LONG_AXIS_IS_RED 0
#define LONG_AXIS_IS_GREEN 1
#define LONG_AXIS_IS_BLUE 2

typedef struct CubeRec { /* not really a ``cube'' */
   int min_index, max_index, level, long_axis;
   unsigned long num_points;
   unsigned short red_length, green_length, blue_length;
} *CubePointer;

typedef struct HGBucketRec {
   int index;
   struct HGBucketRec *next;
} *HGBucketPoiner;

static struct HGBucketRec *gaHGBucket[256];

typedef struct tagTmpBucketInfo {
   int pixel;
   int index;
   struct tagTmpBucketInfo *next;
} TmpBucketInfo;

static XColor *gpHistogram=NULL;
static int *gpnSortedIndex=NULL;
static int **gnOrigImageIndex=NULL, **gnFinalImageIndex=NULL;
static int gnImageW=(-1), gnImageH=(-1);
static int *gpnPixelToIndexMap=NULL, gnPixelToIndexMapSize=0;
static int gnHistogramEntries=0, gnHistogramSize=0;
static int gnQuantizingLevels=222, gnUserSpecifiedLevels=(-1);
static struct CubeRec *gpCube=NULL;
static int gnCubeEntries=0;
static int gnTransparentIndex=(-1);

static
int GetXPmImages(xpm_ptr, p_image, p_bitmap_image)
   struct XPmRec *xpm_ptr;
   XImage **p_image, **p_bitmap_image;
{
   Pixmap pixmap=xpm_ptr->pixmap, bitmap=xpm_ptr->bitmap;
   int image_w=xpm_ptr->image_w, image_h=xpm_ptr->image_h;

   *p_image = XGetImage(mainDisplay, pixmap, 0, 0, image_w, image_h, AllPlanes,
         ZPixmap);
   if (bitmap != None) {
      *p_bitmap_image = XGetImage(mainDisplay, bitmap, 0, 0, image_w, image_h,
         1, ZPixmap);
   } else {
      *p_bitmap_image = NULL;
   }
   if ((*p_image) == NULL || (bitmap != None && (*p_bitmap_image) == NULL)) {
      sprintf(gszMsgBox, TgLoadString(STID_CANNOT_GET_IMAGE_OF_SIZE),
            image_w, image_h);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return FALSE;
   }
   return TRUE;
}

static
void CleanUpTmpBuckets()
{
   if (gpnPixelToIndexMap != NULL) {
      int i=0;
      TmpBucketInfo **buckets=(TmpBucketInfo**)gpnPixelToIndexMap;

      for (i=0; i < gnPixelToIndexMapSize; i++) {
         TmpBucketInfo *ptbi=NULL, *ptbi_next=NULL;

         for (ptbi=buckets[i]; ptbi != NULL; ptbi=ptbi_next) {
            ptbi_next = ptbi->next;
            free(ptbi);
         }
      }
      free(buckets);
      gpnPixelToIndexMap = NULL;
   }
   gnPixelToIndexMapSize = 0;
}

static
void CleanUpIndexOfPixel()
{
   if (mainVisual->class == TrueColor) {
      CleanUpTmpBuckets();
   } else {
      if (gpnPixelToIndexMap != NULL) free(gpnPixelToIndexMap);
      gpnPixelToIndexMap = NULL;
      gnPixelToIndexMapSize = 0;
   }
}

void CleanUpConvolution()
{
   register int i;

   CleanUpIndexOfPixel();

   if (gpHistogram != NULL) {
      free(gpHistogram);
      gpHistogram = NULL;
   }
   if (gpnSortedIndex != NULL) {
      free(gpnSortedIndex);
      gpnSortedIndex = NULL;
   }
   gnHistogramEntries = gnHistogramSize = 0;

   if (gpCube != NULL) {
      free(gpCube);
      gpCube = NULL;
   }
   gnCubeEntries = 0;
   if (gnOrigImageIndex != NULL) {
      for (i=0; i < gnImageH; i++) {
         if (gnOrigImageIndex[i] != NULL) {
            free(gnOrigImageIndex[i]);
         } else {
            break;
         }
      }
      free(gnOrigImageIndex);
      gnOrigImageIndex = NULL;
   }
   if (gnFinalImageIndex != NULL) {
      for (i=0; i < gnImageH; i++) {
         if (gnFinalImageIndex[i] != NULL) {
            free(gnFinalImageIndex[i]);
         } else {
            break;
         }
      }
      free(gnFinalImageIndex);
      gnFinalImageIndex = NULL;
   }
   gnImageW = gnImageH = (-1);

   for (i=0; i < 256; i++) {
      if (gaHGBucket[i] != NULL) {
         struct HGBucketRec *bucket_ptr=gaHGBucket[i], *next_bucket;

         for ( ; bucket_ptr != NULL; bucket_ptr=next_bucket) {
            next_bucket = bucket_ptr->next;
            free(bucket_ptr);
         }
         gaHGBucket[i] = NULL;
      }
   }
}

static
int GetOrAllocHistogramIndex(pcolor)
   XColor *pcolor;
{
   int hashvalue=0;

   if (pcolor == NULL) {
      if (gnTransparentIndex != (-1)) return gnTransparentIndex;
   } else {
      struct HGBucketRec *bucket_ptr;

      hashvalue = (int)((pcolor->red ^ pcolor->green ^ pcolor->blue) & 0xff);
      if (gaHGBucket[hashvalue] != NULL) {
         struct HGBucketRec *bucket_ptr=gaHGBucket[hashvalue];

         for ( ; bucket_ptr != NULL; bucket_ptr=bucket_ptr->next) {
            int i=bucket_ptr->index;

            if (gpHistogram[i].red == pcolor->red &&
                  gpHistogram[i].green == pcolor->green &&
                  gpHistogram[i].blue == pcolor->blue) {
               HISTOGRAMCOUNT(i)++;
               return i;
            }
         }
      }
      bucket_ptr = (struct HGBucketRec *)malloc(sizeof(struct HGBucketRec));
      if (bucket_ptr == NULL) {
         FailAllocMessage();
         return (-1);
      }
      bucket_ptr->index = gnHistogramEntries;
      bucket_ptr->next = gaHGBucket[hashvalue];
      gaHGBucket[hashvalue] = bucket_ptr;
   }
   if (gnHistogramEntries >= gnHistogramSize) {
      gnHistogramSize += 256;
      if ((gpHistogram=(XColor*)realloc(gpHistogram,
            gnHistogramSize*sizeof(XColor))) == NULL) {
         FailAllocMessage();
         return (-1);
      }
   }
   memset(&gpHistogram[gnHistogramEntries], 0, sizeof(XColor));
   HISTOGRAMCOUNT(gnHistogramEntries) = 1;
   if (pcolor == NULL) {
      gpHistogram[gnHistogramEntries].red = 0;
      gpHistogram[gnHistogramEntries].green = 0;
      gpHistogram[gnHistogramEntries].blue = 0;
   } else {
      gpHistogram[gnHistogramEntries].red = pcolor->red;
      gpHistogram[gnHistogramEntries].green = pcolor->green;
      gpHistogram[gnHistogramEntries].blue = pcolor->blue;
   }
   return (gnHistogramEntries++);
}

static
int PixelToIndexHash(pixel)
   int pixel;
{
   return (((pixel)==(-1)) ? (gnPixelToIndexMapSize-1) :
         (pixel % gnPixelToIndexMapSize));
}

static
int GetIndexOfPixel(pixel)
   int pixel;
{
   if (mainVisual->class == TrueColor) {
      TmpBucketInfo **buckets=(TmpBucketInfo**)gpnPixelToIndexMap, *ptbi=NULL;
      int bucket=PixelToIndexHash(pixel);

      for (ptbi=buckets[bucket]; ptbi != NULL; ptbi=ptbi->next) {
         if (ptbi->pixel == pixel) {
            return ptbi->index;
         }
      }
      return INVALID;
   } else {
      return gpnPixelToIndexMap[pixel];
   }
}

static
void UpdatePixelToIndexMapping(buckets, pixel, index)
   TmpBucketInfo **buckets;
   int pixel, index;
{
   int bucket=PixelToIndexHash(pixel);
   TmpBucketInfo *ptbi=NULL;

   for (ptbi=buckets[bucket]; ptbi != NULL; ptbi=ptbi->next) {
      if (ptbi->pixel == pixel) {
         return;
      }
   }
   ptbi = (TmpBucketInfo *)malloc(sizeof(TmpBucketInfo));
   if (ptbi == NULL) FailAllocMessage();
   memset(ptbi, 0, sizeof(TmpBucketInfo));
   ptbi->next = buckets[bucket];
   buckets[bucket] = ptbi;
   ptbi->pixel = pixel;
   ptbi->index = index;
}

static
int AllocTmpBuckets(populate_with_color_pixels)
   int populate_with_color_pixels;
{
   TmpBucketInfo **buckets=(TmpBucketInfo **)gpnPixelToIndexMap;

   gnPixelToIndexMapSize = 257;
   buckets = (TmpBucketInfo**)malloc(gnPixelToIndexMapSize *
         sizeof(TmpBucketInfo*));
   if (buckets == NULL) {
      FailAllocMessage();
      return FALSE;
   }
   memset(buckets, 0, gnPixelToIndexMapSize*sizeof(TmpBucketInfo*));

   gpnPixelToIndexMap = (int*)buckets;
   if (populate_with_color_pixels) {
      int i=0;

      for (i=0; i < maxColors; i++) {
         UpdatePixelToIndexMapping(buckets, colorPixels[i], i);
      }
   }
   return TRUE;
}

static
int CreatePixelToIndexMapping()
{
   register int i=0;

   if (mainVisual->class == TrueColor) {
      if (!AllocTmpBuckets(TRUE)) {
         CleanUpConvolution();
         return FALSE;
      }
   } else {
      int max_pixel=(-1);

      for (i=0; i < maxColors; i++) {
         if (colorPixels[i] > max_pixel) {
            max_pixel = colorPixels[i];
         }
      }
      if (max_pixel == (-1)) return FALSE;

      gpnPixelToIndexMap = (int*)malloc((max_pixel+1)*sizeof(int));
      if (gpnPixelToIndexMap == NULL) {
         FailAllocMessage();
         CleanUpConvolution();
         return FALSE;
      }
      memset(gpnPixelToIndexMap, (-1), (max_pixel+1)*sizeof(int));
      for (i=0; i < maxColors; i++) {
         gpnPixelToIndexMap[colorPixels[i]] = i;
      }
   }
   return TRUE;
}

static
int CreateObjPixelToIndexMapping(xpm_ptr)
   struct XPmRec *xpm_ptr;
{
   register int i=0;

   gnTransparentIndex = (-1);

   if (mainVisual->class == TrueColor) {
      int start_index=0;
      XColor *xcolors=NULL;

      xcolors = (XColor*)malloc(xpm_ptr->ncolors*sizeof(XColor));
      if (xcolors == NULL) FailAllocMessage();
      memset(xcolors, 0, xpm_ptr->ncolors*sizeof(XColor));
      if (!AllocTmpBuckets(TRUE)) {
         free(xcolors);
         CleanUpConvolution();
         return FALSE;
      }
      start_index = (xpm_ptr->first_pixel_is_bg ? 1 : 0);
      for (i=start_index; i < xpm_ptr->ncolors; i++) {
         int pixel=xpm_ptr->pixels[i];

         /* do not translate -- program constants */
         if (UtilStrICmp(xpm_ptr->color_str[i], "None") == 0) {
            if (gnTransparentIndex == (-1)) {
               gnTransparentIndex = GetOrAllocHistogramIndex(NULL);
            }
         } else {
            ((ImageMapColorFunc*)gpImageMapColorFunc)(GetIndexOfPixel(pixel),
                  &xcolors[i]);
         }
      }
      CleanUpTmpBuckets();
      if (!AllocTmpBuckets(FALSE)) {
         free(xcolors);
         CleanUpConvolution();
         return FALSE;
      }
      for (i=start_index; i < xpm_ptr->ncolors; i++) {
         UpdatePixelToIndexMapping((TmpBucketInfo**)gpnPixelToIndexMap,
               xpm_ptr->pixels[i], GetOrAllocHistogramIndex(&xcolors[i]));
      }
      free(xcolors);
   } else {
      int max_pixel=(-1), start_index, *pixel_to_index_map=NULL;

      for (i=0; i < maxColors; i++) {
         if (colorPixels[i] > max_pixel) {
            max_pixel = colorPixels[i];
         }
      }
      if (max_pixel == (-1)) return FALSE;

      gpnPixelToIndexMap = (int*)malloc((max_pixel+1)*sizeof(int));
      pixel_to_index_map = (int*)malloc((max_pixel+1)*sizeof(int));
      if (gpnPixelToIndexMap == NULL || pixel_to_index_map == NULL) {
         if (gpnPixelToIndexMap != NULL) free(gpnPixelToIndexMap);
         if (pixel_to_index_map != NULL) free(pixel_to_index_map);
         gpnPixelToIndexMap = NULL;
         FailAllocMessage();
         CleanUpConvolution();
         return FALSE;
      }
      memset(gpnPixelToIndexMap, (-1), (max_pixel+1)*sizeof(int));
      memset(pixel_to_index_map, (-1), (max_pixel+1)*sizeof(int));
      for (i=0; i < maxColors; i++) {
         pixel_to_index_map[colorPixels[i]] = i;
      }
      start_index = (xpm_ptr->first_pixel_is_bg ? 1 : 0);
      for (i=start_index; i < xpm_ptr->ncolors; i++) {
         XColor xcolor;
         int pixel=xpm_ptr->pixels[i];

         memset(&xcolor, 0, sizeof(XColor));
         /* do not translate -- program constants */
         if (UtilStrICmp(xpm_ptr->color_str[i], "None") == 0) {
            if (gnTransparentIndex == (-1)) {
               gnTransparentIndex = GetOrAllocHistogramIndex(NULL);
            }
         } else {
            ((ImageMapColorFunc*)gpImageMapColorFunc)(pixel_to_index_map[pixel],
                  &xcolor);
            gpnPixelToIndexMap[pixel] = GetOrAllocHistogramIndex(&xcolor);
         }
      }
      free(pixel_to_index_map);
   }
   return TRUE;
}

static
int DumpConvolution(fp)
   FILE *fp;
{
   register int j, i;
   int chars_per_pixel=(gnHistogramEntries > 20 ? 2 : 1);
   char c0[27], c1[11];
   ProgressInfo pi;

   /* do not translate -- program constants */
   strcpy(c0, "abcdefghijklmnopqrstuvwxyz");
   strcpy(c1, "0123456789");
   if (fprintf(fp, "#define conv_format 1\n") == EOF ||
         fprintf(fp, "#define conv_width %1d\n", gnImageW) == EOF ||
         fprintf(fp, "#define conv_height %1d\n", gnImageH) == EOF ||
         fprintf(fp, "#define conv_ncolors %1d\n", gnHistogramEntries) == EOF ||
         fprintf(fp, "#define conv_chars_per_pixel %1d\n",
         chars_per_pixel) == EOF ||
         fprintf(fp, "static char *conv_colors[] = {\n") == EOF) {
      writeFileFailed = TRUE;
   }
   for (j=0; j < gnHistogramEntries; j++) {
      int red=(int)gpHistogram[j].red;
      int green=(int)gpHistogram[j].green;
      int blue=(int)gpHistogram[j].blue;

      /* do not translate -- program constants */
      if (gnTransparentIndex == j) {
         if (chars_per_pixel == 1) {
            if (fprintf(fp, "   \"%c\", \"None\"", c0[j]) == EOF) {
               writeFileFailed = TRUE;
            }
         } else {
            if (fprintf(fp, "   \"%c%c\", \"None\"",
                  c0[(int)(j/10)], c1[j % 10]) == EOF) {
               writeFileFailed = TRUE;
            }
         }
      } else {
         if (chars_per_pixel == 1) {
            if (fprintf(fp, "   \"%c\", \"#%04x%04x%04x\"",
                  c0[j], red&0x0ffff, green&0x0ffff, blue&0x0ffff) == EOF) {
               writeFileFailed = TRUE;
            }
         } else {
            if (fprintf(fp, "   \"%c%c\", \"#%04x%04x%04x\"",
                  c0[(int)(j/10)], c1[j % 10],
                  red&0x0ffff, green&0x0ffff, blue&0x0ffff) == EOF) {
               writeFileFailed = TRUE;
            }
         }
      }
      if (j == gnHistogramEntries-1) {
         fprintf(fp, "\n};\n");
      } else {
         fprintf(fp, ",\n");
      }
   }
   if (fprintf(fp, "static char *conv_pixels[] = {\n") == EOF) {
      writeFileFailed = TRUE;
   }
   BeginProgress(&pi, gnImageH);
   for (i=0; i < gnImageH; i++) {
      UpdateProgress(&pi, i);
      fprintf(fp, "\"");
      for (j=0; j < gnImageW; j++) {
         int index=gnFinalImageIndex[i][j];

         if (chars_per_pixel == 1) {
            if (fprintf(fp, "%c", c0[index]) == EOF) {
               writeFileFailed = TRUE;
            }
         } else {
            if (fprintf(fp, "%c%c",
                  c0[(int)(index/10)], c1[index % 10]) == EOF) {
               writeFileFailed = TRUE;
            }
         }
      }
      if (i == gnImageH-1) {
         if (fprintf(fp, "\"\n};\n") == EOF) writeFileFailed = TRUE;
      } else {
         if (fprintf(fp, "\",\n") == EOF) writeFileFailed = TRUE;
      }
   }
   return TRUE;
}

static int gnDebugQuantization=FALSE;

static
int DumpQuantizedConvolution(fp)
   FILE *fp;
{
   register int j, i;
   int chars_per_pixel=(gnCubeEntries > 20 ? 2 : 1);
   char c0[27], c1[11];
   ProgressInfo pi;

   /* do not translate -- program constants */
   strcpy(c0, "abcdefghijklmnopqrstuvwxyz");
   strcpy(c1, "0123456789");
   if (fprintf(fp, "#define conv_format 1\n") == EOF ||
         fprintf(fp, "#define conv_width %1d\n", gnImageW) == EOF ||
         fprintf(fp, "#define conv_height %1d\n", gnImageH) == EOF ||
         fprintf(fp, "#define conv_ncolors %1d\n", gnCubeEntries) == EOF ||
         fprintf(fp, "#define conv_chars_per_pixel %1d\n",
         chars_per_pixel) == EOF ||
         fprintf(fp, "static char *conv_colors[] = {\n") == EOF) {
      writeFileFailed = TRUE;
   }
   if (gnDebugQuantization) { /* debug, do not translate */
      fprintf(stderr, "Dumping colors...\n");
   }
   for (j=0; j < gnCubeEntries; j++) {
      int min_index=gpCube[j].min_index;
      int max_index=gpCube[j].max_index;
      int idx=gpnSortedIndex[min_index];
      double num_points=(double)HISTOGRAMCOUNT(idx);
      double red=((double)HISTOGRAMRED(idx))*num_points;
      double green=((double)HISTOGRAMGREEN(idx))*num_points;
      double blue=((double)HISTOGRAMBLUE(idx))*num_points;
      long lred, lgreen, lblue;

      for (i=min_index+1; i <= max_index; i++) {
         double n;

         idx = gpnSortedIndex[i];
         n = (double)HISTOGRAMCOUNT(idx);
         num_points += n;
         red += ((long)HISTOGRAMRED(idx))*n;
         green += ((long)HISTOGRAMGREEN(idx))*n;
         blue += ((long)HISTOGRAMBLUE(idx))*n;
      }
      red /= num_points; green /= num_points; blue /= num_points;
      lred = (long)red; lgreen = (long)green; lblue = (long)blue;
      if (gnDebugQuantization) {
         fprintf(stderr, "\t#%02x%02x%02x %6ld\n",
               (int)((lred>>8) & 0xff), (int)((lgreen>>8) & 0xff),
               (int)((lblue>>8) & 0xff), (long)num_points);
      }
      if (chars_per_pixel == 1) {
         if (fprintf(fp, "   \"%c\", \"#%04x%04x%04x\"",
               c0[j], (int)(lred&0x0ffff), (int)(lgreen&0x0ffff),
               (int)(lblue&0x0ffff)) == EOF) {
            writeFileFailed = TRUE;
         }
      } else {
         if (fprintf(fp, "   \"%c%c\", \"#%04x%04x%04x\"",
               c0[(int)(j/10)], c1[j % 10], (int)(lred&0x0ffff),
               (int)(lgreen&0x0ffff), (int)(lblue&0x0ffff)) == EOF) {
            writeFileFailed = TRUE;
         }
      }
      if (j == gnCubeEntries-1) {
         fprintf(fp, "\n};\n");
      } else {
         fprintf(fp, ",\n");
      }
      /*
       * use the gpHistogram[*].pixel as the reverse color index
       */
      for (i=min_index; i <= max_index; i++) {
         HISTOGRAMCOUNT(gpnSortedIndex[i]) = (long)j;
      }
   }
   if (fprintf(fp, "static char *conv_pixels[] = {\n") == EOF) {
      writeFileFailed = TRUE;
   }
   BeginProgress(&pi, gnImageH);
   for (i=0; i < gnImageH; i++) {
      UpdateProgress(&pi, i);
      fprintf(fp, "\"");
      for (j=0; j < gnImageW; j++) {
         int orig_index=gnFinalImageIndex[i][j];
         int index=HISTOGRAMCOUNT(orig_index);

         if (chars_per_pixel == 1) {
            if (fprintf(fp, "%c", c0[index]) == EOF) {
               writeFileFailed = TRUE;
            }
         } else {
            if (fprintf(fp, "%c%c",
                  c0[(int)(index/10)], c1[index % 10]) == EOF) {
               writeFileFailed = TRUE;
            }
         }
      }
      if (i == gnImageH-1) {
         if (fprintf(fp, "\"\n};\n") == EOF) writeFileFailed = TRUE;
      } else {
         if (fprintf(fp, "\",\n") == EOF) writeFileFailed = TRUE;
      }
   }
   return TRUE;
}

static
int AlreadySorted(nMinIndex, nMaxIndex, nLongAxis)
   int nMinIndex, nMaxIndex, nLongAxis;
{
   register int i;

   switch (nLongAxis) {
   case LONG_AXIS_IS_RED:
      for (i=nMinIndex; i < nMaxIndex; i++) {
         if (HISTOGRAMRED(gpnSortedIndex[i]) <
               HISTOGRAMRED(gpnSortedIndex[i+1])) {
            return FALSE;
         }
      }
      break;
   case LONG_AXIS_IS_GREEN:
      for (i=nMinIndex; i < nMaxIndex; i++) {
         if (HISTOGRAMGREEN(gpnSortedIndex[i]) <
               HISTOGRAMGREEN(gpnSortedIndex[i+1])) {
            return FALSE;
         }
      }
      break;
   case LONG_AXIS_IS_BLUE:
      for (i=nMinIndex; i < nMaxIndex; i++) {
         if (HISTOGRAMBLUE(gpnSortedIndex[i]) <
               HISTOGRAMBLUE(gpnSortedIndex[i+1])) {
            return FALSE;
         }
      }
      break;
   }
   return TRUE;
}

static
void DebugSortACube(nMinIndex, nMaxIndex, nLevel, nLongAxis)
   int nMinIndex, nMaxIndex, nLevel, nLongAxis;
{
   register int i;
   int sorted=TRUE;

   /* debug, do not translate */
   fprintf(stderr, "Level %1d done (long axis is '%s'):\n", nLevel,
         (nLongAxis==LONG_AXIS_IS_RED ? "red" :
         (nLongAxis==LONG_AXIS_IS_GREEN ? "green" : "blue")));
   for (i=nMinIndex; i <= nMaxIndex; i++) {
      fprintf(stderr, "\t%6ld: %6d %6d %6d\n",
            (long)HISTOGRAMCOUNT(gpnSortedIndex[i]),
            (int)HISTOGRAMRED(gpnSortedIndex[i]),
            (int)HISTOGRAMGREEN(gpnSortedIndex[i]),
            (int)HISTOGRAMBLUE(gpnSortedIndex[i]));
      switch (nLongAxis) {
      case LONG_AXIS_IS_RED:
         if (sorted && i != nMinIndex && HISTOGRAMRED(gpnSortedIndex[i-1]) <
               HISTOGRAMRED(gpnSortedIndex[i])) {
            sorted = FALSE;
         }
         break;
      case LONG_AXIS_IS_GREEN:
         if (sorted && i != nMinIndex && HISTOGRAMGREEN(gpnSortedIndex[i-1]) <
               HISTOGRAMGREEN(gpnSortedIndex[i])) {
            sorted = FALSE;
         }
         break;
      case LONG_AXIS_IS_BLUE:
         if (sorted && i != nMinIndex && HISTOGRAMBLUE(gpnSortedIndex[i-1]) <
               HISTOGRAMBLUE(gpnSortedIndex[i])) {
            sorted = FALSE;
         }
         break;
      }
   }
   if (!sorted) fprintf(stderr, "Not sorted!\n");
}

/*
static
void DisplaySortACube(nMinIndex, nMaxIndex)
   int nMinIndex, nMaxIndex;
{
   register int i;
   int sorted=TRUE;

   for (i=nMinIndex; i <= nMaxIndex; i++) {
      fprintf(stderr, "\t%6ld %6d %6d %6d %6d %3d\n",
            (long)HISTOGRAMCOUNT(gpnSortedIndex[i]),
            (int)HISTOGRAMRED(gpnSortedIndex[i]),
            (int)HISTOGRAMGREEN(gpnSortedIndex[i]),
            (int)HISTOGRAMBLUE(gpnSortedIndex[i]),
            gpnSortedIndex[i], i);
   }
   if (!sorted) fprintf(stderr, "Not sorted!\n");
}
 */

static int dbg_sort=FALSE;

static
void QuickSortACube(nMinIndex, nMaxIndex, nLevel, nLongAxis)
   int nMinIndex, nMaxIndex, nLevel, nLongAxis;
{
   register int i, j;
   int pivot_index, tmp, something_swapped;
   unsigned long pivot_value=0;

   if (nMinIndex > nMaxIndex) return;
   if (AlreadySorted(nMinIndex, nMaxIndex, nLongAxis)) return;

   pivot_index = nMaxIndex;
   switch (nLongAxis) {
   case LONG_AXIS_IS_RED:
      pivot_value = HISTOGRAMRED(gpnSortedIndex[pivot_index]);
      break;
   case LONG_AXIS_IS_GREEN:
      pivot_value = HISTOGRAMGREEN(gpnSortedIndex[pivot_index]);
      break;
   case LONG_AXIS_IS_BLUE:
      pivot_value = HISTOGRAMBLUE(gpnSortedIndex[pivot_index]);
      break;
   }
   i = nMinIndex;
   j = nMaxIndex-1;
   something_swapped = FALSE;
   do {
      switch (nLongAxis) {
      case LONG_AXIS_IS_RED:
         while (HISTOGRAMRED(gpnSortedIndex[i]) > pivot_value) i++;
         while (j > i && HISTOGRAMRED(gpnSortedIndex[j]) < pivot_value) j--;
         break;
      case LONG_AXIS_IS_GREEN:
         while (HISTOGRAMGREEN(gpnSortedIndex[i]) > pivot_value) i++;
         while (j > i && HISTOGRAMGREEN(gpnSortedIndex[j]) < pivot_value) j--;
         break;
      case LONG_AXIS_IS_BLUE:
         while (HISTOGRAMBLUE(gpnSortedIndex[i]) > pivot_value) i++;
         while (j > i && HISTOGRAMBLUE(gpnSortedIndex[j]) < pivot_value) j--;
         break;
      }
      if (j > i) {
         tmp = gpnSortedIndex[j];
         gpnSortedIndex[j] = gpnSortedIndex[i];
         gpnSortedIndex[i] = tmp;
         if (something_swapped == FALSE) {
            something_swapped = TRUE;
         }
         if (j == i+1) break;
         i++; j--;
      } else {
         break;
      }
   } while (TRUE);
   if (i == nMaxIndex) {
      /* pivot_value is the smallest */
      if (something_swapped) {
#ifdef _TGIF_DBG /* debug, do not translate */
         fprintf(stderr, "Huh? nMinIndex=%1d, nMaxIndex=%1d, nLevel=%1d\n",
               nMinIndex, nMaxIndex, nLevel);
#endif /* _TGIF_DBG */
      } else {
         QuickSortACube(nMinIndex, j, nLevel+1, nLongAxis);
      }
   } else if (j > i) {
      tmp = gpnSortedIndex[nMaxIndex];
      gpnSortedIndex[nMaxIndex] = gpnSortedIndex[j];
      gpnSortedIndex[j] = tmp;
      QuickSortACube(nMinIndex, j-1, nLevel+1, nLongAxis);
      QuickSortACube(j+1, nMaxIndex, nLevel+1, nLongAxis);
   } else {
      tmp = gpnSortedIndex[nMaxIndex];
      gpnSortedIndex[nMaxIndex] = gpnSortedIndex[i];
      gpnSortedIndex[i] = tmp;
      QuickSortACube(nMinIndex, i-1, nLevel+1, nLongAxis);
      QuickSortACube(i+1, nMaxIndex, nLevel+1, nLongAxis);
   }
   if (dbg_sort) {
      DebugSortACube(nMinIndex, nMaxIndex, nLevel, nLongAxis);
   }
}

static
void SweepACube(cube_index)
   int cube_index;
{
   register int i;
   int min_index=gpCube[cube_index].min_index;
   int max_index=gpCube[cube_index].max_index;
   unsigned short min_r, max_r, min_g, max_g, min_b, max_b;

   min_r = max_r = gpHistogram[gpnSortedIndex[min_index]].red;
   min_g = max_g = gpHistogram[gpnSortedIndex[min_index]].green;
   min_b = max_b = gpHistogram[gpnSortedIndex[min_index]].blue;
   gpCube[cube_index].num_points = HISTOGRAMCOUNT(gpnSortedIndex[min_index]);
   for (i=min_index+1; i <= max_index; i++) {
      unsigned short red=gpHistogram[gpnSortedIndex[i]].red;
      unsigned short green=gpHistogram[gpnSortedIndex[i]].green;
      unsigned short blue=gpHistogram[gpnSortedIndex[i]].blue;

      gpCube[cube_index].num_points += HISTOGRAMCOUNT(gpnSortedIndex[i]);
      if (red < min_r) min_r = red;
      if (red > max_r) max_r = red;
      if (green < min_g) min_g = green;
      if (green > max_g) max_g = green;
      if (blue < min_b) min_b = blue;
      if (blue > max_b) max_b = blue;
   }
   gpCube[cube_index].red_length = max_r-min_r;
   gpCube[cube_index].green_length = max_g-min_g;
   gpCube[cube_index].blue_length = max_b-min_b;
   if (gpCube[cube_index].red_length >= gpCube[cube_index].green_length) {
      if (gpCube[cube_index].red_length >= gpCube[cube_index].blue_length) {
         gpCube[cube_index].long_axis = LONG_AXIS_IS_RED;
      } else {
         gpCube[cube_index].long_axis = LONG_AXIS_IS_BLUE;
      }
   } else {
      if (gpCube[cube_index].green_length >= gpCube[cube_index].blue_length) {
         gpCube[cube_index].long_axis = LONG_AXIS_IS_GREEN;
      } else {
         gpCube[cube_index].long_axis = LONG_AXIS_IS_BLUE;
      }
   }
}

static
int SplitACube(cube_index, pul_before_count, pul_after_count)
   int cube_index;
   unsigned long *pul_before_count, *pul_after_count;
   /*
    * cube to be split into (min_index,return_index)
    * and (return_index+1,max_index)
    */
{
   register int i;
   int min_index=gpCube[cube_index].min_index;
   int max_index=gpCube[cube_index].max_index;
   unsigned long count;
   unsigned long half_num_points;

   if (max_index == min_index+1) {
      *pul_before_count = HISTOGRAMCOUNT(gpnSortedIndex[min_index]);
      *pul_after_count = HISTOGRAMCOUNT(gpnSortedIndex[max_index]);
      return min_index;
   }
   count = 0;
   half_num_points = (gpCube[cube_index].num_points>>1);
   for (i=min_index; i <= max_index; i++) {
      unsigned long inc=HISTOGRAMCOUNT(gpnSortedIndex[i]);

      if (count+inc >= half_num_points) {
         if (i == min_index) {
            *pul_before_count = inc;
            *pul_after_count = gpCube[cube_index].num_points-inc;
            return i;
         } else if (i == max_index) {
            *pul_before_count = count;
            *pul_after_count = gpCube[cube_index].num_points-count;
            return i-1;
         } else if (count+inc == half_num_points) {
            *pul_before_count = count+inc;
            *pul_after_count = gpCube[cube_index].num_points-count-inc;
            return i;
         } else if (half_num_points-count >= count+inc-half_num_points) {
            if (i+1 == max_index) {
               *pul_before_count = count;
               *pul_after_count = gpCube[cube_index].num_points-count;
               return i;
            } else {
               *pul_before_count = count+inc;
               *pul_after_count = gpCube[cube_index].num_points-count-inc;
               return i+1;
            }
         } else {
            *pul_before_count = count;
            *pul_after_count = gpCube[cube_index].num_points-count;
            return i;
         }
      }
      count += inc;
   }
   count = HISTOGRAMCOUNT(gpnSortedIndex[max_index-1]);
   *pul_before_count = gpCube[cube_index].num_points-count;
   *pul_after_count = count;
   return max_index-1;
}

static
int Quantize()
   /* median-cut quantization */
{
   int smallest_level=0, max_level=0, cube_index;

   gpCube = (struct CubeRec *)malloc(gnQuantizingLevels*sizeof(struct CubeRec));
   if (gpCube == NULL) {
      FailAllocMessage();
      return FALSE;
   }
   memset(gpCube, 0, gnQuantizingLevels*sizeof(struct CubeRec));
   gnCubeEntries = 1;
   gpCube[0].min_index = 0;
   gpCube[0].max_index = gnHistogramEntries-1;
   gpCube[0].level = 0;
   cube_index = 0;
   SweepACube(0);
   if (gnDebugQuantization) {
      int i;

      /* debug, do not translate */
      fprintf(stderr, "Original histogram in Quantize():\n");
      for (i=0; i < gnHistogramEntries; i++) {
         unsigned long count=(int)HISTOGRAMCOUNT(gpnSortedIndex[i]);
         int red=(int)((HISTOGRAMRED(gpnSortedIndex[i])>>8) & 0xff);
         int green=(int)((HISTOGRAMGREEN(gpnSortedIndex[i])>>8) & 0xff);
         int blue=(int)((HISTOGRAMBLUE(gpnSortedIndex[i])>>8) & 0xff);

         fprintf(stderr, "\t#%02x%02x%02x %6ld\n",
               red&0x0ff, green&0x0ff, blue&0x0ff, count);
      }
   }
   while (gnCubeEntries < gnQuantizingLevels) {
      unsigned long before_count, after_count;
      int split_index, new_level;

      while (smallest_level <= max_level) {
         int saved_cube_index=cube_index, found=FALSE;

         for ( ; cube_index < gnCubeEntries; cube_index++) {
            if (gpCube[cube_index].min_index != gpCube[cube_index].max_index &&
                  gpCube[cube_index].level == smallest_level) {
               found = TRUE;
               break;
            }
         }
         if (found) break;
         for (cube_index=0; cube_index < saved_cube_index; cube_index++) {
            if (gpCube[cube_index].min_index != gpCube[cube_index].max_index &&
                  gpCube[cube_index].level == smallest_level) {
               found = TRUE;
               break;
            }
         }
         if (found) break;
         smallest_level++;
      }
      if (smallest_level > max_level) break;

      /*
       * determine which is the longest axis
       */
      QuickSortACube(gpCube[cube_index].min_index, gpCube[cube_index].max_index,
            0, gpCube[cube_index].long_axis);
      /*
       * cube to be split into (min_index,split_index)
       * and (split_index+1,max_index)
       */
      split_index = SplitACube(cube_index, &before_count, &after_count);
      new_level = gpCube[cube_index].level+1;
      if (gnDebugQuantization) {
         /* debug, do not translate */
         fprintf(stderr,
               "Level %2d (%2d): [%3d,%3d] -> %6ld/[%3d,%3d] %6ld/[%3d,%3d]\n",
               gpCube[cube_index].level, cube_index,
               gpCube[cube_index].min_index, gpCube[cube_index].max_index,
               before_count, gpCube[cube_index].min_index, split_index,
               after_count, split_index+1, gpCube[cube_index].max_index);
      }
      gpCube[gnCubeEntries].min_index = split_index+1;
      gpCube[gnCubeEntries].max_index = gpCube[cube_index].max_index;
      gpCube[gnCubeEntries].level = new_level;
      gpCube[gnCubeEntries].num_points = after_count;
      SweepACube(gnCubeEntries);
      gnCubeEntries++;

      gpCube[cube_index].min_index = gpCube[cube_index].min_index;
      gpCube[cube_index].max_index = split_index;
      gpCube[cube_index].level = new_level;
      gpCube[cube_index].num_points = before_count;
      SweepACube(cube_index);

      if (max_level < new_level) max_level = new_level;

      cube_index++;
   }
   if (gnDebugQuantization) {
      for (cube_index=0; cube_index < gnCubeEntries; cube_index++) {
         int i;

         /* debug, do not translate */
         fprintf(stderr, "cube %3d: (%3d) [%3d,%3d] %6ld\n",
               cube_index, gpCube[cube_index].level,
               gpCube[cube_index].min_index, gpCube[cube_index].max_index,
               (long)gpCube[cube_index].num_points);
         for (i=gpCube[cube_index].min_index; i <= gpCube[cube_index].max_index;
               i++) {
            unsigned long count=(int)HISTOGRAMCOUNT(gpnSortedIndex[i]);
            int red=(int)((HISTOGRAMRED(gpnSortedIndex[i])>>8) & 0xff);
            int green=(int)((HISTOGRAMGREEN(gpnSortedIndex[i])>>8) & 0xff);
            int blue=(int)((HISTOGRAMBLUE(gpnSortedIndex[i])>>8) & 0xff);

            fprintf(stderr, "\t#%02x%02x%02x %6ld\n",
                  red&0x0ff, green&0x0ff, blue&0x0ff, count);
         }
      }
   }
   return TRUE;
}

int DoConvolution(fp, image, bitmap_image, w, h, xpm_ptr)
   FILE *fp;
   XImage *image, *bitmap_image;
   int w, h;
   struct XPmRec *xpm_ptr;
{
   register int j, i;
   int interrupted=FALSE, rc;
   ProgressInfo pi;

   if (gpConvolveFunc == NULL) {
      return FALSE;
   }
   memset(gaHGBucket, 0, sizeof(gaHGBucket));
   gnHistogramEntries = 0;

   if (DoPpm6(xpm_ptr)) {
      writeFileFailed = FALSE;
      if (fprintf(fp, "P6\n%1d %1d\n255\n", w, h) == EOF) {
         writeFileFailed = TRUE;
      }
      if (gpConvolveCmdID == CMDID_VECTORWARP) {
         /* already initialized in ComputeVectorWarpData(), except for fp */
         gConvExtraInfo.fp = fp;
      } else {
         if (!SetConvExtraInfo(fp, w, h, image, bitmap_image)) {
            CleanUpConvolution();
            return FALSE;
         }
      }
      ShowInterrupt(1);
      BeginProgress(&pi, h);
      for (i=0; i < h; i++) {
         UpdateProgress(&pi, i);
   
         if (ESCPressed() || CheckInterrupt(TRUE)) {
            Msg(TgLoadString(STID_USER_INTR));
            interrupted = TRUE;
            break;
         }
         for (j=0; j < w; j++) {
            ((ConvolveFunc*)gpConvolveFunc)(j, i);
         }
      }
      HideInterrupt();
      ClearConvExtraInfo();
      if (interrupted) {
         CleanUpConvolution();
         return FALSE;
      }
      CleanUpConvolution();

      return (writeFileFailed == FALSE);
   }
   SetStringStatus(TgLoadCachedString(CSTID_BUILDING_HISTOGRAM_DOTS));
   XSync(mainDisplay, False);
   if (!CreatePixelToIndexMapping()) {
      return FALSE;
   }
   gnHistogramSize = 256;
   gpHistogram = (XColor*)malloc(gnHistogramSize*sizeof(XColor));
   if (gpHistogram == NULL) {
      FailAllocMessage();
      CleanUpConvolution();
      return FALSE;
   }
   gnImageW = w;
   gnImageH = h;
   gnOrigImageIndex = (int**)malloc(h*sizeof(int*));
   if (gnOrigImageIndex == NULL) {
      FailAllocMessage();
      CleanUpConvolution();
      return FALSE;
   }
   memset(gnOrigImageIndex, 0, h*sizeof(int*));
   for (i=0; i < h; i++) {
      gnOrigImageIndex[i] = (int*)malloc(w*sizeof(int));
      if (gnOrigImageIndex[i] == NULL) {
         FailAllocMessage();
         CleanUpConvolution();
         return FALSE;
      }
   }
   if (image != NULL) {
      BeginProgress(&pi, h);
      for (i=0; i < h; i++) {
         UpdateProgress(&pi, i);
         for (j=0; j < w; j++) {
            /* int pixel=XGetPixel(image,j,i); */
            /* int index=pnPixelToIndexMap[pixel]; */

            gnOrigImageIndex[i][j] = GetIndexOfPixel(XGetPixel(image,j,i));
         }
      }
   }
   gnFinalImageIndex = (int**)malloc(h*sizeof(int*));
   if (gnFinalImageIndex == NULL) {
      FailAllocMessage();
      CleanUpConvolution();
      return FALSE;
   }
   memset(gnFinalImageIndex, 0, h*sizeof(int*));
   for (i=0; i < h; i++) {
      gnFinalImageIndex[i] = (int*)malloc(w*sizeof(int));
      if (gnFinalImageIndex[i] == NULL) {
         FailAllocMessage();
         CleanUpConvolution();
         return FALSE;
      }
   }
   ShowInterrupt(1);
   BeginProgress(&pi, h);
   for (i=0; i < h; i++) {
      UpdateProgress(&pi, i);
      if (ESCPressed() || CheckInterrupt(TRUE)) {
         Msg(TgLoadString(STID_USER_INTR));
         interrupted = TRUE;
         break;
      }
      for (j=0; j < w; j++) {
         gnFinalImageIndex[i][j] = ((ConvolveFunc*)gpConvolveFunc)(j, i);
      }
   }
   HideInterrupt();
   if (interrupted) {
      CleanUpConvolution();
      return FALSE;
   }
   gpnSortedIndex = (int*)malloc(gnHistogramEntries*sizeof(int));
   if (gpnSortedIndex == NULL) {
      FailAllocMessage();
      CleanUpConvolution();
      return FALSE;
   }
   for (i=0; i < gnHistogramEntries; i++) gpnSortedIndex[i] = i;
   if (gnUserSpecifiedLevels != (-1) ||
         gnHistogramEntries > gnQuantizingLevels) {
      int saved_levels=gnQuantizingLevels;

      if (gnUserSpecifiedLevels != (-1)) {
         gnQuantizingLevels = gnUserSpecifiedLevels;
      }
      sprintf(gszMsgBox, TgLoadCachedString(CSTID_QUANTIZING_COLORS_DOTS),
            gnHistogramEntries, gnQuantizingLevels);
      Msg(gszMsgBox);
      SetStringStatus(gszMsgBox);
      XSync(mainDisplay, False);
      if (Quantize()) {
         rc = DumpQuantizedConvolution(fp);
         if (gnUserSpecifiedLevels != (-1)) {
            gnQuantizingLevels = saved_levels;
         }
         CleanUpConvolution();
         return rc;
      }
      CleanUpConvolution();
      gnQuantizingLevels = saved_levels;;
      return FALSE;
   }
   rc = DumpConvolution(fp);
   CleanUpConvolution();
   return rc;
}

static
int DoColorMapping(fp, image, bitmap_image, w, h, xpm_ptr)
   FILE *fp;
   XImage *image, *bitmap_image;
   int w, h;
   struct XPmRec *xpm_ptr;
{
   register int j, i;
   int interrupted=FALSE, rc;
   ProgressInfo pi;

   if (gpImageMapColorFunc == NULL) {
      return FALSE;
   }
   SetStringStatus(TgLoadCachedString(CSTID_REMAPPING_COLORS_DOTS));
   XSync(mainDisplay, False);

   memset(gaHGBucket, 0, sizeof(gaHGBucket));
   gnHistogramEntries = 0;

   if (DoPpm6(xpm_ptr)) {
      writeFileFailed = FALSE;
      if (fprintf(fp, "P6\n%1d %1d\n255\n", w, h) == EOF) {
         writeFileFailed = TRUE;
      }
      ShowInterrupt(1);
      BeginProgress(&pi, h);
      for (i=0; i < h; i++) {
         UpdateProgress(&pi, i);
   
         if (ESCPressed() || CheckInterrupt(TRUE)) {
            Msg(TgLoadString(STID_USER_INTR));
            interrupted = TRUE;
            break;
         }
         for (j=0; j < w; j++) {
            if (bitmap_image != NULL && XGetPixel(bitmap_image,j,i) == 0) {
               TgAssert(FALSE, "transparent pixel not supported", NULL);
            } else {
               ((ImageMapColorFunc*)gpImageMapColorFunc)(XGetPixel(image,j,i),
                     (XColor*)fp);
            }
         }
      }
      HideInterrupt();
      if (interrupted) {
         CleanUpConvolution();
         return FALSE;
      }
      CleanUpConvolution();

      return (writeFileFailed == FALSE);
   }
   gnHistogramSize = 256;
   gpHistogram = (XColor*)malloc(gnHistogramSize*sizeof(XColor));
   if (gpHistogram == NULL) {
      FailAllocMessage();
      CleanUpConvolution();
      return FALSE;
   }
   if (!CreateObjPixelToIndexMapping(xpm_ptr)) {
      CleanUpConvolution();
      return FALSE;
   }
   gnImageW = w;
   gnImageH = h;
   gnFinalImageIndex = (int**)malloc(h*sizeof(int*));
   if (gnFinalImageIndex == NULL) {
      FailAllocMessage();
      CleanUpConvolution();
      return FALSE;
   }
   memset(gnFinalImageIndex, 0, h*sizeof(int*));
   for (i=0; i < h; i++) {
      gnFinalImageIndex[i] = (int*)malloc(w*sizeof(int));
      if (gnFinalImageIndex[i] == NULL) {
         FailAllocMessage();
         CleanUpConvolution();
         return FALSE;
      }
   }
   ShowInterrupt(1);
   BeginProgress(&pi, h);
   for (i=0; i < h; i++) {
      UpdateProgress(&pi, i);

      if (ESCPressed() || CheckInterrupt(TRUE)) {
         Msg(TgLoadString(STID_USER_INTR));
         interrupted = TRUE;
         break;
      }
      for (j=0; j < w; j++) {
         /* int pixel=XGetPixel(image,j,i); */
         /* int index=pnPixelToIndexMap[pixel]; */

         if (bitmap_image != NULL && XGetPixel(bitmap_image,j,i) == 0) {
            gnFinalImageIndex[i][j] = gnTransparentIndex;
         } else {
            gnFinalImageIndex[i][j] = GetIndexOfPixel(XGetPixel(image,j,i));
         }
      }
   }
   HideInterrupt();
   if (interrupted) {
      CleanUpConvolution();
      return FALSE;
   }
   rc = DumpConvolution(fp);
   CleanUpConvolution();
   return rc;
}

/* ----------------------- ProcessImage ----------------------- */

FILE *GetImageProcOutputFileName(pszPath, path_buf_sz, pnShortName, ppszRest)
   char *pszPath, **ppszRest;
   int path_buf_sz, *pnShortName;
{
   FILE *fp=NULL;

   if (MkTempFile(pszPath, path_buf_sz, tmpDir, TOOL_NAME) == NULL) {
      return NULL;
   }
   if ((*pnShortName=IsPrefix(bootDir, pszPath, ppszRest))) {
      *ppszRest = (&(*ppszRest)[1]);
   }
   if ((fp=fopen(pszPath, "w")) == NULL) {
      if (*pnShortName) {
         sprintf(gszMsgBox, TgLoadString(STID_CANNOT_OPEN_FILE_FOR_WRITING),
               *ppszRest);
      } else {
         sprintf(gszMsgBox, TgLoadString(STID_CANNOT_OPEN_FILE_FOR_WRITING),
               pszPath);
      }
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return NULL;
   }
   return fp;
}

static
int CleanUpProcessImage(fp, image, bitmap_image)
   FILE *fp;
   XImage *image, *bitmap_image;
{
   if (fp != NULL) fclose(fp);
   if (image != NULL) XDestroyImage(image);
   if (bitmap_image != NULL) XDestroyImage(bitmap_image);
   return FALSE;
}

static
int ProcessImage()
{
   int short_name=FALSE, ok=TRUE;
   char path[MAXPATHLENGTH+1], *rest=NULL;
   Pixmap pixmap=None, bitmap=None;
   XImage *image=NULL, *bitmap_image=NULL;
   FILE *fp=NULL;
   struct ObjRec *obj_ptr=NULL;
   int image_w=0, image_h=0;
   struct XPmRec *xpm_ptr=NULL;

   if ((fp=GetImageProcOutputFileName(path, sizeof(path), &short_name,
         &rest)) == NULL) {
      return FALSE;
   }
   if (gnCombining) {
      obj_ptr = NULL;
   } else {
      obj_ptr = topSel->obj;
   }
   if (obj_ptr == NULL) {
      pixmap = None;
      bitmap = None;
      image_w = gnCombineW;
      image_h = gnCombineH;
      image = bitmap_image = NULL;
   } else if (obj_ptr->type == OBJ_XPM) {
      xpm_ptr = obj_ptr->detail.xpm;

      pixmap = xpm_ptr->pixmap;
      bitmap = xpm_ptr->bitmap;
      image_w = xpm_ptr->image_w;
      image_h = xpm_ptr->image_h;

      image = XGetImage(mainDisplay, pixmap, 0, 0, image_w, image_h, AllPlanes,
            ZPixmap);
      if (bitmap != None) {
         bitmap_image = XGetImage(mainDisplay, bitmap, 0, 0, image_w, image_h,
            1, ZPixmap);
      }
      if (image == NULL || (bitmap != None && bitmap_image == NULL)) {
         sprintf(gszMsgBox, TgLoadString(STID_CANNOT_GET_IMAGE_OF_SIZE),
               image_w, image_h);
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
         return CleanUpProcessImage(fp, image, bitmap_image);
      }
      if (DoPpm6(xpm_ptr)) {
         if (!InitTrueColorInfo(image, &gTrueColorInfo, image_w)) {
            sprintf(gszMsgBox, TgLoadString(STID_CANNOT_GET_IMAGE_OF_SIZE),
                  image_w, image_h);
            MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
            return CleanUpProcessImage(fp, image, bitmap_image);
         }
         if (gpConvolveCmdID == CMDID_REDUCECOLORS) {
            int floyd=FALSE;
            char tmp_fname[MAXPATHLENGTH];

            switch (MsgBox(TgLoadString(STID_Q_FS_ERROR_DIFFUSE), TOOL_NAME,
                  YNC_MB)) {
            case MB_ID_YES: floyd = TRUE; break;
            case MB_ID_NO: floyd = FALSE; break;
            case MB_ID_CANCEL:
               return CleanUpProcessImage(fp, image, bitmap_image);
            }
            if (MkTempFile(tmp_fname, sizeof(tmp_fname), tmpDir, TOOL_NAME) ==
                  NULL) {
               return CleanUpProcessImage(fp, image, bitmap_image);
            }
            if (DumpXImageToPpmFile(image, image_w, image_h, tmp_fname,
                  FALSE)) {
               FILE *pfp=NULL;
               char psz_cmd[MAXPATHLENGTH<<1];
               int bytes_read=0;

               if (floyd) {
                  snprintf(psz_cmd, sizeof(psz_cmd), ppmFSquantCmd,
                        gnUserSpecifiedLevels, tmp_fname);
               } else {
                  snprintf(psz_cmd, sizeof(psz_cmd), ppmquantCmd,
                        gnUserSpecifiedLevels, tmp_fname);
               }
               if ((pfp=(FILE*)popen(psz_cmd,"r")) == NULL) {
                  sprintf(gszMsgBox, TgLoadString(STID_FAIL_TO_EXECUTE_CMD),
                        psz_cmd);
                  MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
                  unlink(tmp_fname);
                  return CleanUpProcessImage(fp, image, bitmap_image);
               }
               writeFileFailed = FALSE;
               while ((bytes_read=fread(gszMsgBox, sizeof(char),
                     sizeof(gszMsgBox), pfp)) > 0) {
                  if ((int)fwrite(gszMsgBox, sizeof(char), bytes_read,
                        fp) <= 0) {
                     writeFileFailed = TRUE;
                     break;
                  }
               }
               pclose(pfp);
               if (writeFileFailed) {
                  FailToWriteFileMessage(short_name ? rest : tmp_fname);
                  unlink(tmp_fname);
                  return CleanUpProcessImage(fp, image, bitmap_image);
               }
               unlink(tmp_fname);
               CleanUpProcessImage(fp, image, bitmap_image);
               strcpy(gszImageProcXPmFile, path);
               if (gnConvolving) {
                  CleanUpConvolution();
               }
               return TRUE;
            }
         }
      }
   } else {
      return CleanUpProcessImage(fp, image, bitmap_image);
   }
   SaveStatusStrings();
   if (gnConvolving) {
      ok = DoConvolution(fp, image, bitmap_image, image_w, image_h, xpm_ptr);
   } else {
      ok = DoColorMapping(fp, image, bitmap_image, image_w, image_h, xpm_ptr);
   }
   RestoreStatusStrings();
   CleanUpProcessImage(fp, image, bitmap_image);
   if (!ok) return FALSE;
   strcpy(gszImageProcXPmFile, path);
   if (gnConvolving) {
      CleanUpConvolution();
   }
   return TRUE;
}

static
int DoImageProc(pvImageMapColorFunc)
   NLFN *pvImageMapColorFunc;
{
   int saved_colordump=colorDump, saved_left=leftExportPixelTrim;
   int saved_top=topExportPixelTrim, saved_right=rightExportPixelTrim;
   int saved_bottom=bottomExportPixelTrim, saved_where_to_print=whereToPrint;
   int saved_ltx, saved_lty, saved_cur_file_defined=curFileDefined;
   int ltx, lty, rbx, rby, saved_w, saved_h, saved_x=0, saved_y=0;
   int w, h, image_w, image_h, ncolors, first_pixel_is_bg, ctm_saved=FALSE;
   int rc, chars_per_pixel, *pixels=NULL, retry_count=0;
   struct XfrmMtrxRec saved_ctm;
   struct BBRec saved_orig_obbox;
   XPoint saved_rotated_obbox[5];
   char *color_char=NULL, **color_str=NULL, *xpm_data=NULL;
   Pixmap pixmap=None, bitmap=None;
   XImage *image=NULL, *bitmap_image=NULL;
   struct ObjRec *obj_ptr;
   struct AttrRec *saved_fattr=NULL, *saved_lattr=NULL;
   struct XPmRec *xpm_ptr=NULL;

   if (gnCombining) {
      saved_ltx = selObjLtX;
      saved_lty = selObjLtY;
      saved_w = selObjRbX - saved_ltx;
      saved_h = selObjRbY - saved_lty;
      ltx = selLtX;
      lty = selLtY;
      rbx = selRbX;
      rby = selRbY;
   } else {
      if (topSel->obj->ctm != NULL) {
         ctm_saved = TRUE;
         saved_x = topSel->obj->x;
         saved_y = topSel->obj->y;
         memcpy(&saved_ctm, topSel->obj->ctm,
               sizeof(struct XfrmMtrxRec));

         memcpy(&saved_orig_obbox, &topSel->obj->orig_obbox,
               sizeof(struct BBRec));
         memcpy(saved_rotated_obbox, topSel->obj->rotated_obbox,
               5*sizeof(XPoint));
      }
      saved_ltx = topSel->obj->obbox.ltx;
      saved_lty = topSel->obj->obbox.lty;
      saved_w = topSel->obj->obbox.rbx - saved_ltx;
      saved_h = topSel->obj->obbox.rby - saved_lty;
      ltx = topSel->obj->bbox.ltx;
      lty = topSel->obj->bbox.lty;
      rbx = topSel->obj->bbox.rbx;
      rby = topSel->obj->bbox.rby;
   }
   leftExportPixelTrim = topExportPixelTrim = rightExportPixelTrim =
         bottomExportPixelTrim = 0;
   *gszImageProcXPmFile = '\0';

   curFileDefined = TRUE;
   whereToPrint = XBM_FILE;
   colorDump = TRUE;
   gnInImageProc = TRUE;
   gpImageMapColorFunc = (NLFN*)pvImageMapColorFunc;

   SetWatchCursor(drawWindow);
   SetWatchCursor(mainWindow);
   ProcessImage();
   SetDefaultCursor(mainWindow);
   ShowCursor();

   gpImageMapColorFunc = NULL;
   colorDump = saved_colordump;
   whereToPrint = saved_where_to_print;
   curFileDefined = saved_cur_file_defined;
   leftExportPixelTrim = saved_left;
   topExportPixelTrim = saved_top;
   rightExportPixelTrim = saved_right;
   bottomExportPixelTrim = saved_bottom;

   if (*gszImageProcXPmFile == '\0') {
      gnInImageProc = FALSE;
      return FALSE;
   }
   if (gnCombining) {
      struct SelRec *sel_ptr;

      HighLightReverse();
      PrepareToRecord(CMD_REPLACE, topSel, botSel, numObjSelected);

      obj_ptr = NULL;
      for (sel_ptr=botSel; sel_ptr != NULL; sel_ptr=sel_ptr->prev) {
         UnlinkObj(sel_ptr->obj);
         FreeObj(sel_ptr->obj);
      }
      RemoveAllSel();
   } else {
      HighLightReverse();
      PrepareToRecord(CMD_REPLACE, topSel, botSel, numObjSelected);

      obj_ptr = topSel->obj;
      xpm_ptr = obj_ptr->detail.xpm;
      saved_fattr = obj_ptr->fattr;
      saved_lattr = obj_ptr->lattr;
      obj_ptr->fattr = obj_ptr->lattr = NULL;
      RemoveAllSel();
   }
   gnInImageProc = FALSE;
   if (DoPpm6(xpm_ptr)) {
      char deflated_fname[MAXPATHLENGTH+1];

      if (obj_ptr != NULL) UnlinkObj(obj_ptr);
      if (obj_ptr != NULL) FreeObj(obj_ptr);

      ResetPngHeaderInfo(&gPngHeaderInfo);
      obj_ptr = CreatePpmTrueObjFromFile(gszImageProcXPmFile);
      if (obj_ptr != NULL &&
            MkTempFile(deflated_fname, sizeof(deflated_fname), tmpDir,
            TOOL_NAME) != NULL &&
            DeflateFile(gszImageProcXPmFile, deflated_fname)) {
         /* good */
      } else {
         FreeObj(obj_ptr);

         sprintf(gszMsgBox, TgLoadString(STID_CANNOT_IMPORT_GIVEN_PPM),
               gszImageProcXPmFile);
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
         unlink(gszImageProcXPmFile);
         *gszImageProcXPmFile = '\0';
         AbortPrepareCmd(CMD_REPLACE);

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
   } else {
      do {
         int new_colormap_used=newColormapUsed;

         if (obj_ptr != NULL) UnlinkObj(obj_ptr);
         if (obj_ptr != NULL) FreeObj(obj_ptr);

         gnInImageProc = TRUE;
         if (FlushColormap()) {
            Msg(TgLoadString(STID_COLORMAP_FLUSHED));
         }
         gnInImageProc = FALSE;

         allocColorFailed = FALSE;
         SetWatchCursor(drawWindow);
         SetWatchCursor(mainWindow);
         rc = MyReadPixmapFile(gszImageProcXPmFile, &image_w, &image_h, &w, &h,
               &pixmap, &image, &bitmap, &bitmap_image, &ncolors,
               &chars_per_pixel, &first_pixel_is_bg, &color_char, &color_str,
               &pixels, &xpm_data);
         SetDefaultCursor(mainWindow);
         ShowCursor();

         if (rc != BitmapSuccess) {
            sprintf(gszMsgBox, TgLoadString(STID_CANNOT_IMPORT_XPM_FILE),
                  gszImageProcXPmFile);
            MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
            unlink(gszImageProcXPmFile);
            *gszImageProcXPmFile = '\0';
            if (!PRTGIF && colorLayers && needToRedrawColorWindow) {
               RedrawColorWindow();
            }
            AbortPrepareCmd(CMD_REPLACE);

            return FALSE;
         }
         obj_ptr = CreateXPmObj(image_w, image_h, w, h, pixmap, image, bitmap,
               bitmap_image, ncolors, chars_per_pixel, FALSE, color_char,
               color_str, pixels, xpm_data);
         if (!new_colormap_used && newColormapUsed && allocColorFailed) {
            if (retry_count > 1) {
               break;
            }
            if (MsgBox(TgLoadString(STID_MAY_USED_UP_COLORS_RETRY), TOOL_NAME,
                  YNC_MB) != MB_ID_YES) {
               break;
            }
            retry_count++;
            if (saved_fattr != NULL) {
               obj_ptr->fattr = obj_ptr->lattr = NULL;
            }
         } else {
            break;
         }
      } while (retry_count > 0);
   }
   unlink(gszImageProcXPmFile);
   *gszImageProcXPmFile = '\0';

   obj_ptr->obbox.rbx = obj_ptr->obbox.ltx+saved_w;
   obj_ptr->obbox.rby = obj_ptr->obbox.lty+saved_h;
   AdjObjBBox(obj_ptr);
   AddObj(NULL, topObj, obj_ptr);
   MoveObj(obj_ptr, saved_ltx-obj_ptr->obbox.ltx,
         saved_lty-obj_ptr->obbox.lty);
   if (ctm_saved) {
      obj_ptr->x = saved_x;
      obj_ptr->y = saved_y;
      obj_ptr->ctm =
            (struct XfrmMtrxRec*)malloc(sizeof(struct XfrmMtrxRec));
      if (obj_ptr->ctm == NULL) FailAllocMessage();
      memcpy(obj_ptr->ctm, &saved_ctm, sizeof(struct XfrmMtrxRec));

      memcpy(&obj_ptr->orig_obbox, &saved_orig_obbox,
            sizeof(struct BBRec));
      memcpy(obj_ptr->rotated_obbox, saved_rotated_obbox,
            5*sizeof(XPoint));
   }
   if (saved_fattr != NULL) {
      obj_ptr->fattr = saved_fattr;
      obj_ptr->lattr = saved_lattr;
   }
   RedrawAreas(botObj, ltx-GRID_ABS_SIZE(1), lty-GRID_ABS_SIZE(1),
         rbx+GRID_ABS_SIZE(1), rby+GRID_ABS_SIZE(1),
         obj_ptr->bbox.ltx-GRID_ABS_SIZE(1),
         obj_ptr->bbox.lty-GRID_ABS_SIZE(1),
         obj_ptr->bbox.rbx+GRID_ABS_SIZE(1),
         obj_ptr->bbox.rby+GRID_ABS_SIZE(1));

   if (saved_fattr != NULL && topObj->fattr == NULL) {
      topObj->fattr = saved_fattr;
      topObj->lattr = saved_lattr;
   }
   if (!PRTGIF && colorLayers && needToRedrawColorWindow) {
      RedrawColorWindow();
   }
   if (gnCombining) {
      SelectTopObj();

      recordCmdUsesNewColormap = TRUE;
      RecordCmd(CMD_MANY_TO_ONE, NULL, topSel, botSel, 1);
      recordCmdUsesNewColormap = FALSE;
   } else {
      SelectTopObj();

      recordCmdUsesNewColormap = TRUE;
      RecordCmd(CMD_REPLACE, NULL, topSel, botSel, numObjSelected);
      recordCmdUsesNewColormap = FALSE;
   }
   SetFileModified(TRUE);
   justDupped = FALSE;
   return TRUE;
}

/* ----------------------- MakeGray ----------------------- */

static
void ChangeToGray(nColorIndex, pColor)
   int nColorIndex;
   XColor *pColor;
{
   struct XPmRec *xpm_ptr=topObj->detail.xpm;

   if (DoPpm6(xpm_ptr)) {
      int pixel=nColorIndex;
      FILE *fp=(FILE*)pColor;
      uint32_t pix=(uint32_t)(unsigned int)pixel;
      unsigned int r=0, g=0, b=0, igray=0;
      double dr=(double)0, dg=(double)0, db=(double)0;
      double dgray=(double)0;
      unsigned char buf[3];

      r = (pix & gTrueColorInfo.r_mask) >> gTrueColorInfo.r_shift;
      g = (pix & gTrueColorInfo.g_mask) >> gTrueColorInfo.g_shift;
      b = (pix & gTrueColorInfo.b_mask) >> gTrueColorInfo.b_shift;
      dr = ((double)r) / gTrueColorInfo.dr_maxval;
      dg = ((double)g) / gTrueColorInfo.dg_maxval;
      db = ((double)b) / gTrueColorInfo.db_maxval;

      dgray = ((double)(0.299*dr + 0.587*dg + 0.114*db))*((double)256.0);
      igray = (dgray < ((double)0)) ? 0 : ((unsigned int)dgray);
      if (igray > 255) igray = 255;
      buf[0] = buf[1] = buf[2] = (unsigned char)(igray&0xff);
      if ((int)fwrite(buf, sizeof(char), 3, fp) <= 0) writeFileFailed = TRUE;
   } else {
      int red=(int)tgifColors[nColorIndex].red;
      int green=(int)tgifColors[nColorIndex].green;
      int blue=(int)tgifColors[nColorIndex].blue;
      float gray=0.299*((float)red)+0.587*((float)green)+0.114*((float)blue);
      int val=(int)gray;
      int real_gray=((val>0x0ffff) ? 0x0ffff : ((val<0) ? 0 : val));

      pColor->red = pColor->green = pColor->blue = real_gray;
   }
}

void MakeGray()
{
   if (!CheckSelectionForImageProc(CMDID_MAKEGRAY)) return;
   if (TrueColorTransPixelCheck(topSel->obj, CMDID_MAKEGRAY)) return;

   DoImageProc((NLFN*)ChangeToGray);
}

/* ----------------------- InvertColor ----------------------- */

static
void ChangeToInvertColor(nColorIndex, pColor)
   int nColorIndex;
   XColor *pColor;
{
   struct XPmRec *xpm_ptr=topObj->detail.xpm;

   if (DoPpm6(xpm_ptr)) {
      int pixel=nColorIndex;
      FILE *fp=(FILE*)pColor;
      uint32_t pix=(uint32_t)(unsigned int)pixel;
      unsigned int r=0, g=0, b=0;
      double dr=(double)0, dg=(double)0, db=(double)0;
      unsigned char buf[3];

      r = (pix & gTrueColorInfo.r_mask) >> gTrueColorInfo.r_shift;
      g = (pix & gTrueColorInfo.g_mask) >> gTrueColorInfo.g_shift;
      b = (pix & gTrueColorInfo.b_mask) >> gTrueColorInfo.b_shift;
      dr = ((double)r) / gTrueColorInfo.dr_maxval;
      dg = ((double)g) / gTrueColorInfo.dg_maxval;
      db = ((double)b) / gTrueColorInfo.db_maxval;

      dr = (((double)1.0) - dr) * ((double)256);
      dg = (((double)1.0) - dg) * ((double)256);
      db = (((double)1.0) - db) * ((double)256);

      r = (dr < ((double)0)) ? 0 : ((unsigned int)dr);
      g = (dg < ((double)0)) ? 0 : ((unsigned int)dg);
      b = (db < ((double)0)) ? 0 : ((unsigned int)db);
      if (r > 255) r = 255;
      if (g > 255) g = 255;
      if (b > 255) b = 255;
      buf[0] = (unsigned char)r;
      buf[1] = (unsigned char)g;
      buf[2] = (unsigned char)b;
      if ((int)fwrite(buf, sizeof(char), 3, fp) <= 0) writeFileFailed = TRUE;
   } else {
      int red=(int)tgifColors[nColorIndex].red;
      int green=(int)tgifColors[nColorIndex].green;
      int blue=(int)tgifColors[nColorIndex].blue;

      pColor->red = 0x0ffff-((unsigned int)red);
      pColor->green = 0x0ffff-((unsigned int)green);
      pColor->blue = 0x0ffff-((unsigned int)blue);
   }
}

void InvertColor()
{
   if (!CheckSelectionForImageProc(CMDID_INVERTCOLOR)) return;
   if (TrueColorTransPixelCheck(topSel->obj, CMDID_INVERTCOLOR)) return;

   DoImageProc((NLFN*)ChangeToInvertColor);
}

/* ----------------------- InterpolateColor ----------------------- */

static XColor gInterpFromColor, gInterpToColor;

static
void ChangeToInterpolateColor(nColorIndex, pColor)
   int nColorIndex;
   XColor *pColor;
{
   struct XPmRec *xpm_ptr=topObj->detail.xpm;

   if (DoPpm6(xpm_ptr)) {
      int pixel=nColorIndex;
      FILE *fp=(FILE*)pColor;
      uint32_t pix=(uint32_t)(unsigned int)pixel;
      unsigned int r=0, g=0, b=0;
      double dr=(double)0, dg=(double)0, db=(double)0;
      unsigned char buf[3];
      double dgray=(double)0;

      r = (pix & gTrueColorInfo.r_mask) >> gTrueColorInfo.r_shift;
      g = (pix & gTrueColorInfo.g_mask) >> gTrueColorInfo.g_shift;
      b = (pix & gTrueColorInfo.b_mask) >> gTrueColorInfo.b_shift;
      dr = ((double)r) / gTrueColorInfo.dr_maxval;
      dg = ((double)g) / gTrueColorInfo.dg_maxval;
      db = ((double)b) / gTrueColorInfo.db_maxval;

      dgray = (0.299*dr) + (0.587*dg) + (0.114*db);

      dr = (((double)gInterpFromColor.red) +
            dgray * (((double)gInterpToColor.red) -
            ((double)gInterpFromColor.red))) / ((double)256);
      dg = (((double)gInterpFromColor.green) +
            dgray * (((double)gInterpToColor.green) -
            ((double)gInterpFromColor.green))) / ((double)256);
      db = (((double)gInterpFromColor.blue) +
            dgray * (((double)gInterpToColor.blue) -
            ((double)gInterpFromColor.blue))) / ((double)256);

      r = (dr < ((double)0)) ? 0 : ((unsigned int)dr);
      g = (dg < ((double)0)) ? 0 : ((unsigned int)dg);
      b = (db < ((double)0)) ? 0 : ((unsigned int)db);
      if (r > 255) r = 255;
      if (g > 255) g = 255;
      if (b > 255) b = 255;
      buf[0] = (unsigned char)r;
      buf[1] = (unsigned char)g;
      buf[2] = (unsigned char)b;
      if ((int)fwrite(buf, sizeof(char), 3, fp) <= 0) writeFileFailed = TRUE;
   } else {
      int red=(int)tgifColors[nColorIndex].red;
      int green=(int)tgifColors[nColorIndex].green;
      int blue=(int)tgifColors[nColorIndex].blue;
      float gray=(0.299*((float)red)+0.587*((float)green)+0.114*((float)blue)) /
            (float)(0x0000ffff), tmp_fval;
      int val, real_red, real_green, real_blue;

      tmp_fval = ((float)gInterpFromColor.red) +
            gray*(((float)gInterpToColor.red)-((float)gInterpFromColor.red));
      val = round(tmp_fval);
      real_red = ((val>0x0ffff) ? 0x0ffff : ((val<0) ? 0 : val));

      tmp_fval = ((float)gInterpFromColor.green) +
            gray*(((float)gInterpToColor.green)-((float)gInterpFromColor.green));
      val = round(tmp_fval);
      real_green = ((val>0x0ffff) ? 0x0ffff : ((val<0) ? 0 : val));

      tmp_fval = ((float)gInterpFromColor.blue) +
            gray*(((float)gInterpToColor.blue)-((float)gInterpFromColor.blue));
      val = round(tmp_fval);
      real_blue = ((val>0x0ffff) ? 0x0ffff : ((val<0) ? 0 : val));

      pColor->red = (unsigned int)real_red;
      pColor->green = (unsigned int)real_green;
      pColor->blue = (unsigned int)real_blue;
   }
}

void InterpolateColor()
{
   char *c_ptr, szFrom[MAXSTRING+1], szTo[MAXSTRING+1];
   char szSpec[MAXSTRING+1], szSpecCopy[MAXSTRING+1];

   if (!CheckSelectionForImageProc(CMDID_INTERPOLATECOLOR)) return;
   if (TrueColorTransPixelCheck(topSel->obj, CMDID_INTERPOLATECOLOR)) return;

   *szSpec = '\0';
   Dialog(TgLoadString(STID_ENTER_PAIR_COLORS_INTERPOLATE),
         TgLoadCachedString(CSTID_DLG_ACCEPT_CANCEL), szSpec);
   UtilTrimBlanks(szSpec);
   if (*szSpec == '\0') return;

   strcpy(szSpecCopy, szSpec);
   *szFrom = *szTo = '\0';
   if ((c_ptr=strtok(szSpec, " ,-\t\n\r")) != NULL) {
      strcpy(szFrom, c_ptr);
      if ((c_ptr=strtok(NULL, " ,-\t\n\r")) != NULL) {
         strcpy(szTo, c_ptr);
      }
   }
   if (*szFrom == '\0' || *szTo == '\0') {
      sprintf(gszMsgBox, TgLoadString(STID_INVALID_SPEC), szSpecCopy);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return;
   }
   if (!TgifParseColor(szFrom, &gInterpFromColor)) {
      sprintf(gszMsgBox, TgLoadString(STID_GIVEN_IS_NOT_A_VALID_COLOR), szFrom);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return;
   } else if (!TgifParseColor(szTo, &gInterpToColor)) {
      sprintf(gszMsgBox, TgLoadString(STID_GIVEN_IS_NOT_A_VALID_COLOR), szTo);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return;
   }
   DoImageProc((NLFN*)ChangeToInterpolateColor);
}

/* ----------------------- Brighten/Darken ----------------------- */

static double gdBrighten=(double)0;
static int gnBrighten=0;

static
void ChangeToBrightenDarken(nColorIndex, pColor)
   int nColorIndex;
   XColor *pColor;
{
   struct XPmRec *xpm_ptr=topObj->detail.xpm;

   if (DoPpm6(xpm_ptr)) {
      int pixel=nColorIndex;
      FILE *fp=(FILE*)pColor;
      uint32_t pix=(uint32_t)(unsigned int)pixel;
      unsigned int r=0, g=0, b=0;
      double dr=(double)0, dg=(double)0, db=(double)0;
      unsigned char buf[3];

      r = (pix & gTrueColorInfo.r_mask) >> gTrueColorInfo.r_shift;
      g = (pix & gTrueColorInfo.g_mask) >> gTrueColorInfo.g_shift;
      b = (pix & gTrueColorInfo.b_mask) >> gTrueColorInfo.b_shift;
      dr = ((double)r) / gTrueColorInfo.dr_maxval;
      dg = ((double)g) / gTrueColorInfo.dg_maxval;
      db = ((double)b) / gTrueColorInfo.db_maxval;

      dr = (dr + gdBrighten) * ((double)256);
      dg = (dg + gdBrighten) * ((double)256);
      db = (db + gdBrighten) * ((double)256);

      r = (dr < ((double)0)) ? 0 : ((unsigned int)dr);
      g = (dg < ((double)0)) ? 0 : ((unsigned int)dg);
      b = (db < ((double)0)) ? 0 : ((unsigned int)db);
      if (r > 255) r = 255;
      if (g > 255) g = 255;
      if (b > 255) b = 255;
      buf[0] = (unsigned char)r;
      buf[1] = (unsigned char)g;
      buf[2] = (unsigned char)b;
      if ((int)fwrite(buf, sizeof(char), 3, fp) <= 0) writeFileFailed = TRUE;
   } else {
      int red=((int)tgifColors[nColorIndex].red)+gnBrighten;
      int green=((int)tgifColors[nColorIndex].green)+gnBrighten;
      int blue=((int)tgifColors[nColorIndex].blue)+gnBrighten;
      int real_red, real_green, real_blue;

      real_red = ((red>0x0ffff) ? 0x0ffff : ((red<0) ? 0 : red));
      real_green = ((green>0x0ffff) ? 0x0ffff : ((green<0) ? 0 : green));
      real_blue = ((blue>0x0ffff) ? 0x0ffff : ((blue<0) ? 0 : blue));

      pColor->red = (unsigned int)real_red;
      pColor->green = (unsigned int)real_green;
      pColor->blue = (unsigned int)real_blue;
   }
}

void BrightenDarken()
{
   char *c_ptr, szValue[MAXSTRING+1];
   char szSpec[MAXSTRING+1], szSpecCopy[MAXSTRING+1];
   float fVal;

   if (!CheckSelectionForImageProc(CMDID_BRIGHTENDARKEN)) return;
   if (TrueColorTransPixelCheck(topSel->obj, CMDID_BRIGHTENDARKEN)) return;

   *szSpec = '\0';
   Dialog(TgLoadString(STID_ENTER_VAL_MINUS_PLUS_ONE_BW),
         TgLoadCachedString(CSTID_DLG_ACCEPT_CANCEL), szSpec);
   UtilTrimBlanks(szSpec);
   if (*szSpec == '\0') return;

   strcpy(szSpecCopy, szSpec);
   if ((c_ptr=strtok(szSpec, " ,\t\n\r")) == NULL) return;
   strcpy(szValue, c_ptr);
   if (sscanf(szValue, "%f", &fVal) != 1) {
      sprintf(gszMsgBox, TgLoadString(STID_FAIL_TO_PARSE_FOR_A_VAL),
            szSpecCopy);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return;
   }
   gdBrighten = ((double)fVal);
   gnBrighten = (int)round(gdBrighten*((double)0x0000ffff));

   DoImageProc((NLFN*)ChangeToBrightenDarken);
}

/* ----------------------- ChangeSaturation ----------------------- */

/*
 * 0                  65535          spread
 * |--------------------r-----| s = -------- * 65535
 * |---g----------------------|        v
 * |-----------b--------------|
 * |<---------v-------->|              x
 *     |<----spread---->|       h = -------- * 60
 *     |<--x-->|                     spread
 *
 * 0     60   120   180   240   300   360
 * |--r--|--g--|--g--|--b--|--b--|--r--|
 */

static double gfSaturation=(double)0.0;

void RGBtoHSV(r, g, b, h, s, v)
   int r, g, b, *h, *v;
   double *s;
   /* 0 <= r,g,b <= 0x0ffff */
   /* 0 <= *h < 360 */
   /* 0 <= *s,*v <= 0x0ffff */
{
   int max_val=max(r,max(g,b)), min_val=min(r,min(g,b));
   double spread=(double)(max_val-min_val);

   *v = max_val;
   if (max_val == 0) {
      *s = (double)0.0;
   } else {
      *s = (double)((spread*((double)0x0ffff))/((double)max_val));
   }
   if (*s < (double)0.0) *s = (double)0.0;
   if (*s > INT_TOL) {
      int hue=0;

      if (r == max_val) {
         hue = (int)(((double)(g-b))/spread*((double)60.0));
         if (hue < -60) hue = -60;
         if (hue < 0) {
            hue += 360;
         } else if (hue > 60) {
            hue = 60;
         }
      } else if (g == max_val) {
         hue = (int)(((double)120.0) + (((double)(b-r))/spread*((double)60.0)));
         if (hue < 60) hue = 60;
         if (hue > 180) hue = 180;
      } else if (b == max_val) {
         hue = (int)(((double)240.0) + (((double)(r-g))/spread*((double)60.0)));
         if (hue < 180) hue = 180;
         if (hue > 300) hue = 300;
      }
      *h = hue;
   } else {
      *h = 0;
   }
}

#define R_IS_MAX 0
#define G_IS_MAX 1
#define B_IS_MAX 2

void HSVtoRGB(h, s, v, r, g, b)
   double s;
   int h, v, *r, *g, *b;
   /* 0 <= *r,*g,*b <= 0x0ffff */
   /* 0 <= h < 360 */
   /* 0 <= s,v <= 0x0ffff */
{
   if (s <= INT_TOL) {
      *r = *g = *b = v;
   } else {
      double frac, spread;
      int which, min_val, mid_val;

      if (h >= 300) {
         frac = (((double)(360-h))/((double)60.0));
         which = R_IS_MAX;
      } else if (h >= 240) {
         frac = (((double)(h-240))/((double)60.0));
         which = B_IS_MAX;
      } else if (h >= 180) {
         frac = (((double)(240-h))/((double)60.0));
         which = B_IS_MAX;
      } else if (h >= 120) {
         frac = (((double)(h-120))/((double)60.0));
         which = G_IS_MAX;
      } else if (h >= 60) {
         frac = (((double)(120-h))/((double)60.0));
         which = G_IS_MAX;
      } else {
         frac = (((double)h)/((double)60.0));
         which = R_IS_MAX;
      }
      spread = (((double)v)*s/((double)0x0ffff));
      min_val = (int)(v-spread);
      mid_val = min_val+((int)(frac*spread));

      switch (which) {
      case R_IS_MAX:
         *r = v;
         if (h >= 300) {
            /* g < b */  *g = min_val; *b = mid_val;
         } else {
            /* g >= b */ *g = mid_val; *b = min_val;
         }
         break;
      case G_IS_MAX:
         *g = v;
         if (h >= 120) {
            /* b >= r */ *b = mid_val; *r = min_val;
         } else {
            /* b < r */  *b = min_val; *r = mid_val;
         }
         break;
      case B_IS_MAX:
         *b = v;
         if (h >= 240) {
            /* r >= g */ *r = mid_val; *g = min_val;
         } else {
            /* r < g */  *r = min_val; *g = mid_val;
         }
         break;
      }
   }
}

static
void ChangeToChangeSaturation(nColorIndex, pColor)
   int nColorIndex;
   XColor *pColor;
{
   struct XPmRec *xpm_ptr=topObj->detail.xpm;

   if (DoPpm6(xpm_ptr)) {
      int pixel=nColorIndex;
      FILE *fp=(FILE*)pColor;
      uint32_t pix=(uint32_t)(unsigned int)pixel;
      unsigned int r=0, g=0, b=0;
      double dr=(double)0, dg=(double)0, db=(double)0;
      unsigned char buf[3];
      int h=0, v=0;
      double s=(double)0;

      r = (pix & gTrueColorInfo.r_mask) >> gTrueColorInfo.r_shift;
      g = (pix & gTrueColorInfo.g_mask) >> gTrueColorInfo.g_shift;
      b = (pix & gTrueColorInfo.b_mask) >> gTrueColorInfo.b_shift;
      dr = ((double)r) / gTrueColorInfo.dr_maxval;
      dg = ((double)g) / gTrueColorInfo.dg_maxval;
      db = ((double)b) / gTrueColorInfo.db_maxval;

      dr *= ((double)0x10000);
      dg *= ((double)0x10000);
      db *= ((double)0x10000);

      r = (dr < ((double)0)) ? 0 : ((unsigned int)dr);
      g = (dg < ((double)0)) ? 0 : ((unsigned int)dg);
      b = (db < ((double)0)) ? 0 : ((unsigned int)db);
      if (r > 0x0ffff) r = 0x0ffff;
      if (g > 0x0ffff) g = 0x0ffff;
      if (b > 0x0ffff) b = 0x0ffff;

      RGBtoHSV(r, g, b, &h, &s, &v);
      s = s * (((double)1.0)+gfSaturation);
      if (s > (double)0x0ffff) s = (double)0x0ffff;
      if (s < (double)0.0) s = (double)0.0;
      HSVtoRGB(h, s, v, (int*)(&r), (int*)(&g), (int*)(&b));


      dr = ((double)r) / ((double)256);
      dg = ((double)g) / ((double)256);
      db = ((double)b) / ((double)256);

      r = (dr < ((double)0)) ? 0 : ((unsigned int)dr);
      g = (dg < ((double)0)) ? 0 : ((unsigned int)dg);
      b = (db < ((double)0)) ? 0 : ((unsigned int)db);
      if (r > 255) r = 255;
      if (g > 255) g = 255;
      if (b > 255) b = 255;
      buf[0] = (unsigned char)r;
      buf[1] = (unsigned char)g;
      buf[2] = (unsigned char)b;
      if ((int)fwrite(buf, sizeof(char), 3, fp) <= 0) writeFileFailed = TRUE;
   } else {
      int red=((int)tgifColors[nColorIndex].red);
      int green=((int)tgifColors[nColorIndex].green);
      int blue=((int)tgifColors[nColorIndex].blue);
      int h, v, real_red, real_green, real_blue;
      double s=(double)0;

      RGBtoHSV(red, green, blue, &h, &s, &v);
      s = s * (((double)1.0)+gfSaturation);
      if (s > (double)0x0ffff) s = (double)0x0ffff;
      if (s < (double)0.0) s = (double)0.0;
      HSVtoRGB(h, s, v, &red, &green, &blue);

      real_red = ((red>0x0ffff) ? 0x0ffff : ((red<0) ? 0 : red));
      real_green = ((green>0x0ffff) ? 0x0ffff : ((green<0) ? 0 : green));
      real_blue = ((blue>0x0ffff) ? 0x0ffff : ((blue<0) ? 0 : blue));

      pColor->red = (unsigned int)real_red;
      pColor->green = (unsigned int)real_green;
      pColor->blue = (unsigned int)real_blue;
   }
}

void ChangeSaturation()
{
   char *c_ptr, szValue[MAXSTRING+1];
   char szSpec[MAXSTRING+1], szSpecCopy[MAXSTRING+1];
   float fVal;

   if (!CheckSelectionForImageProc(CMDID_CHANGESATURATION)) return;
   if (TrueColorTransPixelCheck(topSel->obj, CMDID_CHANGESATURATION)) return;

   *szSpec = '\0';
   Dialog(TgLoadString(STID_ENTER_VAL_MINUS_PLUS_ONE_SAT),
         TgLoadCachedString(CSTID_DLG_ACCEPT_CANCEL), szSpec);
   UtilTrimBlanks(szSpec);
   if (*szSpec == '\0') return;

   strcpy(szSpecCopy, szSpec);
   if ((c_ptr=strtok(szSpec, " ,\t\n\r")) == NULL) return;
   strcpy(szValue, c_ptr);
   if (sscanf(szValue, "%f", &fVal) != 1) {
      sprintf(gszMsgBox, TgLoadString(STID_FAIL_TO_PARSE_FOR_A_VAL),
            szSpecCopy);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return;
   }
   if (fVal > (float)1.0) fVal = (float)1.0;
   if (fVal < (float)(-1.0)) fVal = (float)(-1.0);
   gfSaturation = fVal;

   DoImageProc((NLFN*)ChangeToChangeSaturation);
}

/* ----------------------- ChangeHue ----------------------- */

static int gnFromHue=0, gnToHue=0;
static float gfFromAngle=(float)0.0, gfToAngle=(float)0.0;
static float gfFromStart=(float)0.0, gfToStart=(float)0.0;
static float gfFromEnd=(float)0.0, gfToEnd=(float)0.0;

static
int HueInFromRange(fHue)
   float fHue;
{
   if (gfFromStart >= gfFromEnd) {
      if (gfFromEnd <= fHue && fHue <= gfFromStart) {
         return TRUE;
      }
   } else {
      if (gfFromStart <= fHue && fHue <= gfFromEnd) {
         return TRUE;
      }
   }
   return FALSE;
}

static
void ChangeToChangeHue(nColorIndex, pColor)
   int nColorIndex;
   XColor *pColor;
{
   struct XPmRec *xpm_ptr=topObj->detail.xpm;

   if (DoPpm6(xpm_ptr)) {
      int pixel=nColorIndex;
      FILE *fp=(FILE*)pColor;
      uint32_t pix=(uint32_t)(unsigned int)pixel;
      unsigned int r=0, g=0, b=0;
      double dr=(double)0, dg=(double)0, db=(double)0;
      unsigned char buf[3];
      int h=0, v=0;
      double s=(double)0;

      r = (pix & gTrueColorInfo.r_mask) >> gTrueColorInfo.r_shift;
      g = (pix & gTrueColorInfo.g_mask) >> gTrueColorInfo.g_shift;
      b = (pix & gTrueColorInfo.b_mask) >> gTrueColorInfo.b_shift;
      dr = ((double)r) / gTrueColorInfo.dr_maxval;
      dg = ((double)g) / gTrueColorInfo.dg_maxval;
      db = ((double)b) / gTrueColorInfo.db_maxval;

      dr *= ((double)0x10000);
      dg *= ((double)0x10000);
      db *= ((double)0x10000);

      r = (dr < ((double)0)) ? 0 : ((unsigned int)dr);
      g = (dg < ((double)0)) ? 0 : ((unsigned int)dg);
      b = (db < ((double)0)) ? 0 : ((unsigned int)db);
      if (r > 0x0ffff) r = 0x0ffff;
      if (g > 0x0ffff) g = 0x0ffff;
      if (b > 0x0ffff) b = 0x0ffff;

      RGBtoHSV(r, g, b, &h, &s, &v);
      if (s > INT_TOL && h > 300) h -= 360;
      if (s > INT_TOL && HueInFromRange((double)h)) {
         double fFraction=(double)0.0;

         if (fabs(gfFromAngle) > INT_TOL) {
            fFraction = (((double)h)-gfFromStart)/(gfFromAngle*((double)2.0));
         }
         h = (int)(fFraction*gfToAngle*((double)2.0) + gfToStart);
         while (h >= 360) h -= 360;
         while (h < 0) h += 360;
         HSVtoRGB(h, s, v, (int*)(&r), (int*)(&g), (int*)(&b));
      }
      dr = ((double)r) / ((double)256);
      dg = ((double)g) / ((double)256);
      db = ((double)b) / ((double)256);

      r = (dr < ((double)0)) ? 0 : ((unsigned int)dr);
      g = (dg < ((double)0)) ? 0 : ((unsigned int)dg);
      b = (db < ((double)0)) ? 0 : ((unsigned int)db);
      if (r > 255) r = 255;
      if (g > 255) g = 255;
      if (b > 255) b = 255;
      buf[0] = (unsigned char)r;
      buf[1] = (unsigned char)g;
      buf[2] = (unsigned char)b;
      if ((int)fwrite(buf, sizeof(char), 3, fp) <= 0) writeFileFailed = TRUE;
   } else {
      int red=((int)tgifColors[nColorIndex].red);
      int green=((int)tgifColors[nColorIndex].green);
      int blue=((int)tgifColors[nColorIndex].blue);
      int h, v, real_red, real_green, real_blue;
      double s=(double)0;

      RGBtoHSV(red, green, blue, &h, &s, &v);
      if (s > INT_TOL && h > 300) h -= 360;
      if (s > INT_TOL && HueInFromRange((double)h)) {
         double fFraction=(double)0.0;

         if (fabs(gfFromAngle) > INT_TOL) {
            fFraction = (((double)h)-gfFromStart)/(gfFromAngle*((double)2.0));
         }
         h = (int)(fFraction*gfToAngle*((double)2.0) + gfToStart);
         while (h >= 360) h -= 360;
         while (h < 0) h += 360;
         HSVtoRGB(h, s, v, &red, &green, &blue);

         real_red = ((red>0x0ffff) ? 0x0ffff : ((red<0) ? 0 : red));
         real_green = ((green>0x0ffff) ? 0x0ffff : ((green<0) ? 0 : green));
         real_blue = ((blue>0x0ffff) ? 0x0ffff : ((blue<0) ? 0 : blue));

         pColor->red = (unsigned int)real_red;
         pColor->green = (unsigned int)real_green;
         pColor->blue = (unsigned int)real_blue;
      } else {
         pColor->red = (unsigned int)red;
         pColor->green = (unsigned int)green;
         pColor->blue = (unsigned int)blue;
      }
   }
}

void ChangeHue()
{
   char *szFrom=NULL, *szFromAngle=NULL, *szTo=NULL, *szToAngle=NULL;
   char szSpec[MAXSTRING+1], szSpecCopy[MAXSTRING+1], szValue[MAXSTRING+1];
   double sVal=(double)0, dVal=(double)0;
   int vVal;
   XColor xcolor;

   if (!CheckSelectionForImageProc(CMDID_CHANGEHUE)) return;
   if (TrueColorTransPixelCheck(topSel->obj, CMDID_CHANGEHUE)) return;

   *szSpec = '\0';
   Dialog(TgLoadString(STID_ENTER_VAL_FOR_CHANGE_HUE), NULL, szSpec);
   UtilTrimBlanks(szSpec);
   if (*szSpec == '\0') return;

   strcpy(szSpecCopy, szSpec);
   if ((szFrom=strtok(szSpec, " ,\t\n\r")) == NULL ||
         (szFromAngle=strtok(NULL, " ,\t\n\r")) == NULL ||
         (szTo=strtok(NULL, " ,\t\n\r")) == NULL ||
         (szToAngle=strtok(NULL, " ,\t\n\r")) == NULL) {
      sprintf(gszMsgBox, TgLoadString(STID_FAIL_TO_PARSE_FOR_4_VAL),
            szSpecCopy);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return;
   }
   if (!TgifParseColor(szFrom, &xcolor)) {
      sprintf(gszMsgBox, TgLoadString(STID_GIVEN_IS_NOT_A_VALID_COLOR), szFrom);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return;
   }
   RGBtoHSV(((int)xcolor.red), ((int)xcolor.green), ((int)xcolor.blue),
         &gnFromHue, &sVal, &vVal);
   if (!TgifParseColor(szTo, &xcolor)) {
      sprintf(gszMsgBox, TgLoadString(STID_GIVEN_IS_NOT_A_VALID_COLOR), szTo);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return;
   }
   RGBtoHSV(((int)xcolor.red), ((int)xcolor.green), ((int)xcolor.blue),
         &gnToHue, &sVal, &vVal);

   strcpy(szValue, szFromAngle);
   if (sscanf(szValue, "%lf", &dVal) != 1) {
      sprintf(gszMsgBox, TgLoadString(STID_FAIL_TO_PARSE_FOR_A_VAL),
            szFromAngle);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return;
   }
   if (dVal > (double)180.0) dVal = (double)180.0;
   if (dVal < (double)(-180.0)) dVal = (double)(-180.0);
   gfFromAngle = (float)dVal;
   gfFromStart = ((double)gnFromHue)-gfFromAngle;
   gfFromEnd = ((double)gnFromHue)+gfFromAngle;

   strcpy(szValue, szToAngle);
   if (sscanf(szValue, "%lf", &dVal) != 1) {
      sprintf(gszMsgBox, TgLoadString(STID_FAIL_TO_PARSE_FOR_A_VAL), szToAngle);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return;
   }
   if (dVal > (double)180.0) dVal = (double)180.0;
   if (dVal < (double)(-180.0)) dVal = (double)(-180.0);
   gfToAngle = (float)dVal;
   gfToStart = ((double)gnToHue)-gfToAngle;
   gfToEnd = ((double)gnToHue)+gfToAngle;

   DoImageProc((NLFN*)ChangeToChangeHue);
}

/* ----------------------- ContrastEnhance ----------------------- */

static float gfContrastFactor=1.0;

static
void ChangeToContrastEnhance(nColorIndex, pColor)
   int nColorIndex;
   XColor *pColor;
{
   struct XPmRec *xpm_ptr=topObj->detail.xpm;

   if (DoPpm6(xpm_ptr)) {
      int pixel=nColorIndex;
      FILE *fp=(FILE*)pColor;
      uint32_t pix=(uint32_t)(unsigned int)pixel;
      unsigned int r=0, g=0, b=0;
      double dr=(double)0, dg=(double)0, db=(double)0;
      unsigned char buf[3];

      r = (pix & gTrueColorInfo.r_mask) >> gTrueColorInfo.r_shift;
      g = (pix & gTrueColorInfo.g_mask) >> gTrueColorInfo.g_shift;
      b = (pix & gTrueColorInfo.b_mask) >> gTrueColorInfo.b_shift;
      dr = ((double)r) / gTrueColorInfo.dr_maxval;
      dg = ((double)g) / gTrueColorInfo.dg_maxval;
      db = ((double)b) / gTrueColorInfo.db_maxval;

      dr = ((dr-0.5) * ((double)gfContrastFactor) + 0.5) * 256.0;
      dg = ((dg-0.5) * ((double)gfContrastFactor) + 0.5) * 256.0;
      db = ((db-0.5) * ((double)gfContrastFactor) + 0.5) * 256.0;

      r = (dr < ((double)0)) ? 0 : ((unsigned int)dr);
      g = (dg < ((double)0)) ? 0 : ((unsigned int)dg);
      b = (db < ((double)0)) ? 0 : ((unsigned int)db);
      if (r > 255) r = 255;
      if (g > 255) g = 255;
      if (b > 255) b = 255;
      buf[0] = (unsigned char)r;
      buf[1] = (unsigned char)g;
      buf[2] = (unsigned char)b;
      if ((int)fwrite(buf, sizeof(char), 3, fp) <= 0) writeFileFailed = TRUE;
   } else {
      int red=(int)tgifColors[nColorIndex].red;
      int green=(int)tgifColors[nColorIndex].green;
      int blue=(int)tgifColors[nColorIndex].blue;
      int ival, real_red, real_green, real_blue;
      float fval;

      fval = ((float)(red-0x8000))*gfContrastFactor + ((float)0x8000);
      ival = round(fval);
      real_red = ((ival>0x0ffff) ? 0x0ffff : ((ival<0) ? 0 : ival));
      fval = ((float)(green-0x8000))*gfContrastFactor + ((float)0x8000);
      ival = round(fval);
      real_green = ((ival>0x0ffff) ? 0x0ffff : ((ival<0) ? 0 : ival));
      fval = ((float)(blue-0x8000))*gfContrastFactor + ((float)0x8000);
      ival = round(fval);
      real_blue = ((ival>0x0ffff) ? 0x0ffff : ((ival<0) ? 0 : ival));

      pColor->red = (unsigned int)real_red;
      pColor->green = (unsigned int)real_green;
      pColor->blue = (unsigned int)real_blue;
   }
}

void ContrastEnhance()
{
   char *c_ptr, szValue[MAXSTRING+1];
   char szSpec[MAXSTRING+1], szSpecCopy[MAXSTRING+1];
   float fVal;

   if (!CheckSelectionForImageProc(CMDID_CONTRASTENHANCE)) return;
   if (TrueColorTransPixelCheck(topSel->obj, CMDID_CONTRASTENHANCE)) return;

   *szSpec = '\0';
   Dialog(TgLoadString(STID_ENTER_VAL_FOR_CONTRAST_ENH),
         TgLoadCachedString(CSTID_DLG_ACCEPT_CANCEL), szSpec);
   UtilTrimBlanks(szSpec);
   if (*szSpec == '\0') return;

   strcpy(szSpecCopy, szSpec);
   if ((c_ptr=strtok(szSpec, " ,\t\n\r")) == NULL) return;
   strcpy(szValue, c_ptr);
   if (strcmp(szValue, "1.0") == 0 || strcmp(szValue, "1") == 0 ||
         strcmp(szValue, "1.") == 0) {
      return;
   } else if (sscanf(szValue, "%f", &fVal) != 1) {
      sprintf(gszMsgBox, TgLoadString(STID_FAIL_TO_PARSE_FOR_A_VAL),
            szSpecCopy);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return;
   } else if (fVal < (float)0.0) {
      sprintf(gszMsgBox, TgLoadString(STID_GIVEN_NEG_VAL_NOT_ALLOWED),
            szSpecCopy);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return;
   }
   gfContrastFactor = fVal;

   DoImageProc((NLFN*)ChangeToContrastEnhance);
}

/* ----------------------- ColorBalance ----------------------- */

static float gfRedBalanceFactor=1.0;
static float gfGreenBalanceFactor=1.0;
static float gfBlueBalanceFactor=1.0;

static
void ChangeToColorBalance(nColorIndex, pColor)
   int nColorIndex;
   XColor *pColor;
{
   struct XPmRec *xpm_ptr=topObj->detail.xpm;

   if (DoPpm6(xpm_ptr)) {
      int pixel=nColorIndex;
      FILE *fp=(FILE*)pColor;
      uint32_t pix=(uint32_t)(unsigned int)pixel;
      unsigned int r=0, g=0, b=0;
      double dr=(double)0, dg=(double)0, db=(double)0;
      unsigned char buf[3];

      r = (pix & gTrueColorInfo.r_mask) >> gTrueColorInfo.r_shift;
      g = (pix & gTrueColorInfo.g_mask) >> gTrueColorInfo.g_shift;
      b = (pix & gTrueColorInfo.b_mask) >> gTrueColorInfo.b_shift;
      dr = ((double)r) / gTrueColorInfo.dr_maxval;
      dg = ((double)g) / gTrueColorInfo.dg_maxval;
      db = ((double)b) / gTrueColorInfo.db_maxval;

      dr = dr * gfRedBalanceFactor * ((double)256);
      dg = dg * gfGreenBalanceFactor * ((double)256);
      db = db * gfBlueBalanceFactor * ((double)256);

      r = (dr < ((double)0)) ? 0 : ((unsigned int)dr);
      g = (dg < ((double)0)) ? 0 : ((unsigned int)dg);
      b = (db < ((double)0)) ? 0 : ((unsigned int)db);
      if (r > 255) r = 255;
      if (g > 255) g = 255;
      if (b > 255) b = 255;
      buf[0] = (unsigned char)r;
      buf[1] = (unsigned char)g;
      buf[2] = (unsigned char)b;
      if ((int)fwrite(buf, sizeof(char), 3, fp) <= 0) writeFileFailed = TRUE;
   } else {
      int red=(int)tgifColors[nColorIndex].red;
      int green=(int)tgifColors[nColorIndex].green;
      int blue=(int)tgifColors[nColorIndex].blue;
      int ival, real_red, real_green, real_blue;
      float fval;

      fval = ((float)red) * gfRedBalanceFactor;
      ival = round(fval);
      real_red = ((ival>0x0ffff) ? 0x0ffff : ((ival<0) ? 0 : ival));
      fval = ((float)green) * gfGreenBalanceFactor;
      ival = round(fval);
      real_green = ((ival>0x0ffff) ? 0x0ffff : ((ival<0) ? 0 : ival));
      fval = ((float)blue) * gfBlueBalanceFactor;
      ival = round(fval);
      real_blue = ((ival>0x0ffff) ? 0x0ffff : ((ival<0) ? 0 : ival));

      pColor->red = (unsigned int)real_red;
      pColor->green = (unsigned int)real_green;
      pColor->blue = (unsigned int)real_blue;
   }
}

void ColorBalance()
{
   char *c_ptr, szRed[MAXSTRING+1], szGreen[MAXSTRING+1], szBlue[MAXSTRING+1];
   char szSpec[MAXSTRING+1], szSpecCopy[MAXSTRING+1];

   if (!CheckSelectionForImageProc(CMDID_COLORBALANCE)) return;
   if (TrueColorTransPixelCheck(topSel->obj, CMDID_COLORBALANCE)) return;

   *szSpec = '\0';
   Dialog(TgLoadString(STID_ENTER_VAL_FOR_COLOR_BAL),
         TgLoadCachedString(CSTID_DLG_ACCEPT_CANCEL), szSpec);
   UtilTrimBlanks(szSpec);
   if (*szSpec == '\0') return;

   strcpy(szSpecCopy, szSpec);
   *szRed = *szGreen = *szBlue = '\0';
   if ((c_ptr=strtok(szSpec, " ,\t\n\r")) != NULL) {
      strcpy(szRed, c_ptr);
      if ((c_ptr=strtok(NULL, " ,\t\n\r")) != NULL) {
         strcpy(szGreen, c_ptr);
         if ((c_ptr=strtok(NULL, " ,\t\n\r")) != NULL) {
            strcpy(szBlue, c_ptr);
         }
      }
   }
   if (*szRed == '\0' || *szGreen == '\0' || *szBlue == '\0' ||
         sscanf(szRed, "%f", &gfRedBalanceFactor) != 1 ||
         sscanf(szGreen, "%f", &gfGreenBalanceFactor) != 1 ||
         sscanf(szBlue, "%f", &gfBlueBalanceFactor) != 1) {
      sprintf(gszMsgBox, TgLoadString(STID_FAIL_TO_PARSE_FOR_3_VAL),
            szSpecCopy);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return;
   } else if (gfRedBalanceFactor < (float)0.0 ||
         gfGreenBalanceFactor < (float)0.0 ||
         gfBlueBalanceFactor < (float)0.0) {
      sprintf(gszMsgBox, TgLoadString(STID_GIVEN_NEG_VAL_NOT_ALLOWED),
            szSpecCopy);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return;
   }

   DoImageProc((NLFN*)ChangeToColorBalance);
}

/* ----------------------- Gamma ----------------------- */

static float gfOneOverGamma=1.0;

static
void ChangeToGamma(nColorIndex, pColor)
   int nColorIndex;
   XColor *pColor;
{
   struct XPmRec *xpm_ptr=topObj->detail.xpm;

   if (DoPpm6(xpm_ptr)) {
      int pixel=nColorIndex; 
      FILE *fp=(FILE*)pColor;
      uint32_t pix=(uint32_t)(unsigned int)pixel;
      unsigned int r=0, g=0, b=0;
      double dr=(double)0, dg=(double)0, db=(double)0;
      unsigned char buf[3];

      r = (pix & gTrueColorInfo.r_mask) >> gTrueColorInfo.r_shift;
      g = (pix & gTrueColorInfo.g_mask) >> gTrueColorInfo.g_shift;
      b = (pix & gTrueColorInfo.b_mask) >> gTrueColorInfo.b_shift;
      dr = ((double)r) / gTrueColorInfo.dr_maxval;
      dg = ((double)g) / gTrueColorInfo.dg_maxval; 
      db = ((double)b) / gTrueColorInfo.db_maxval;

      dr = pow(dr, (double)gfOneOverGamma) * ((double)256);
      dg = pow(dg, (double)gfOneOverGamma) * ((double)256);
      db = pow(db, (double)gfOneOverGamma) * ((double)256);

      r = (dr < ((double)0)) ? 0 : ((unsigned int)dr);
      g = (dg < ((double)0)) ? 0 : ((unsigned int)dg);
      b = (db < ((double)0)) ? 0 : ((unsigned int)db);
      if (r > 255) r = 255;
      if (g > 255) g = 255;
      if (b > 255) b = 255;
      buf[0] = (unsigned char)r;
      buf[1] = (unsigned char)g;
      buf[2] = (unsigned char)b;
      if ((int)fwrite(buf, sizeof(char), 3, fp) <= 0) writeFileFailed = TRUE;
   } else {
      double red=((double)tgifColors[nColorIndex].red)/((double)0x0ffff);
      double green=((double)tgifColors[nColorIndex].green)/((double)0x0ffff);
      double blue=((double)tgifColors[nColorIndex].blue)/((double)0x0ffff);
      int ival, real_red, real_green, real_blue;
      double dval;

      dval = pow(red, (double)gfOneOverGamma) * ((double)0x010000);
      ival = round(dval);
      real_red = ((ival>0x0ffff) ? 0x0ffff : ((ival<0) ? 0 : ival));
      dval = pow(green, (double)gfOneOverGamma) * ((double)0x010000);
      ival = round(dval);
      real_green = ((ival>0x0ffff) ? 0x0ffff : ((ival<0) ? 0 : ival));
      dval = pow(blue, (double)gfOneOverGamma) * ((double)0x010000);
      ival = round(dval);
      real_blue = ((ival>0x0ffff) ? 0x0ffff : ((ival<0) ? 0 : ival));

      pColor->red = (unsigned int)real_red;
      pColor->green = (unsigned int)real_green;
      pColor->blue = (unsigned int)real_blue;
   }
}

void Gamma(buf)
   char *buf;
{
   char *c_ptr, szValue[MAXSTRING+1];
   char szSpec[MAXSTRING+1], szSpecCopy[MAXSTRING+1];
   float gamma=0.0;

   if (!CheckSelectionForImageProc(CMDID_GAMMA)) return;
   if (TrueColorTransPixelCheck(topSel->obj, CMDID_GAMMA)) return;

   if (buf != NULL) {
      strcpy(szSpec, buf);
   } else {
      *szSpec = '\0';
      Dialog(TgLoadString(STID_ENTER_VAL_FOR_GAMMA),
            TgLoadCachedString(CSTID_DLG_ACCEPT_CANCEL), szSpec);
   }
   UtilTrimBlanks(szSpec);
   if (*szSpec == '\0') return;

   strcpy(szSpecCopy, szSpec);
   *szValue = '\0';
   if ((c_ptr=strtok(szSpec, " ,\t\n\r")) != NULL) {
      strcpy(szValue, c_ptr);
   }
   if (*szValue == '\0' || sscanf(szValue, "%f", &gamma) != 1) {
      sprintf(gszMsgBox, TgLoadString(STID_FAIL_TO_PARSE_FOR_A_VAL),
            szSpecCopy);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return;
   } else if (gamma < (float)INT_TOL) {
      sprintf(gszMsgBox, TgLoadString(STID_GIVEN_NEG_VAL_NOT_ALLOWED),
            szSpecCopy);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return;
   }
   gfOneOverGamma = (float)1.0 / gamma;

   DoImageProc((NLFN*)ChangeToGamma);
}

/* ----------------------- Convolution Required ----------------------- */

/* ----------------------- EdgeDetect ----------------------- */

#ifdef NOT_DEFINED
static
int tx=50, ty=16;
#endif /* NOT_DEFINED */

static
int ConvolveToEdgeDetect(x, y)
   int x, y;
   /*
    *               [ -1  0 +1 ]                [ -1 -2 -1 ]
    * horizontally: [ -2  0 +2 ]    vertically: [  0  0  0 ]
    *               [ -2  0 +1 ]                [ +1 +2 +1 ]
    */
{
   if (gConvExtraInfo.fp != NULL) {
      XColor **xcolors=gConvExtraInfo.xcolors;
      FILE *fp=gConvExtraInfo.fp;
      int w=gConvExtraInfo.image_w, h=gConvExtraInfo.image_h;
      unsigned char buf[3];

      if (x == 0 || x == w-1 || y == 0 || y == h-1) {
         buf[0] = (unsigned char)xcolors[y][x].red;
         buf[1] = (unsigned char)xcolors[y][x].green;
         buf[2] = (unsigned char)xcolors[y][x].blue;
      } else {
         unsigned int r=0, g=0, b=0;
         double rx=0, gx=0, bx=0, ry=0, gy=0, by=0;
         double ddr, ddg, ddb;

         /*
          * Thinning is not implemented!
          */
         rx = (double)(
               ((int)xcolors[y][x+1].red) +
               ((int)xcolors[y][x+1].red) +
               ((int)xcolors[y-1][x+1].red) +
               ((int)xcolors[y+1][x+1].red) -
               ((int)xcolors[y][x-1].red) -
               ((int)xcolors[y][x-1].red) -
               ((int)xcolors[y-1][x-1].red) -
               ((int)xcolors[y+1][x-1].red));
         gx = (double)(
               ((int)xcolors[y][x+1].green) +
               ((int)xcolors[y][x+1].green) +
               ((int)xcolors[y-1][x+1].green) +
               ((int)xcolors[y+1][x+1].green) -
               ((int)xcolors[y][x-1].green) -
               ((int)xcolors[y][x-1].green) -
               ((int)xcolors[y-1][x-1].green) -
               ((int)xcolors[y+1][x-1].green));
         bx = (double)(
               ((int)xcolors[y][x+1].blue) +
               ((int)xcolors[y][x+1].blue) +
               ((int)xcolors[y-1][x+1].blue) +
               ((int)xcolors[y+1][x+1].blue) -
               ((int)xcolors[y][x-1].blue) -
               ((int)xcolors[y][x-1].blue) -
               ((int)xcolors[y-1][x-1].blue) -
               ((int)xcolors[y+1][x-1].blue));

#ifdef NOT_DEFINED
if (x == tx && y == ty) {
   fprintf(stderr, "x = %1d, y = %1d\n", x, y);
   fprintf(stderr, "rx = %g, gx = %g, bx = %g\n", rx, gx, bx);
   fflush(stderr);
   fprintf(stderr, "\n");
}
#endif /* NOT_DEFINED */

         ry = (double)(
               ((int)xcolors[y+1][x].red) +
               ((int)xcolors[y+1][x].red) +
               ((int)xcolors[y+1][x-1].red) +
               ((int)xcolors[y+1][x+1].red) -
               ((int)xcolors[y-1][x].red) -
               ((int)xcolors[y-1][x].red) -
               ((int)xcolors[y-1][x-1].red) -
               ((int)xcolors[y-1][x+1].red));
         gy = (double)(
               ((int)xcolors[y+1][x].green) +
               ((int)xcolors[y+1][x].green) +
               ((int)xcolors[y+1][x-1].green) +
               ((int)xcolors[y+1][x+1].green) -
               ((int)xcolors[y-1][x].green) -
               ((int)xcolors[y-1][x].green) -
               ((int)xcolors[y-1][x-1].green) -
               ((int)xcolors[y-1][x+1].green));
         by = (double)(
               ((int)xcolors[y+1][x].blue) +
               ((int)xcolors[y+1][x].blue) +
               ((int)xcolors[y+1][x-1].blue) +
               ((int)xcolors[y+1][x+1].blue) -
               ((int)xcolors[y-1][x].blue) -
               ((int)xcolors[y-1][x].blue) -
               ((int)xcolors[y-1][x-1].blue) -
               ((int)xcolors[y-1][x+1].blue));

         if (rx == 0 && ry == 0 && gx == 0 && gy == 0 && bx == 0 && by == 0) {
            buf[0] = (unsigned char)255;
            buf[1] = (unsigned char)255;
            buf[2] = (unsigned char)255;
         } else {
            ddr = sqrt(rx*rx + ry*ry);
            ddg = sqrt(gx*gx + gy*gy);
            ddb = sqrt(bx*bx + by*by);
            r = (unsigned int)ddr;
            g = (unsigned int)ddg;
            b = (unsigned int)ddb;
            if (r > 255) r = 255;
            if (g > 255) g = 255;
            if (b > 255) b = 255;
            r = 255 - r;
            g = 255 - g;
            b = 255 - b;

            buf[0] = (unsigned char)r;
            buf[1] = (unsigned char)g;
            buf[2] = (unsigned char)b;
         }
      }
      if ((int)fwrite(buf, sizeof(char), 3, fp) <= 0) writeFileFailed = TRUE;

      return TRUE;
   } else {
      XColor xcolor;
      long red, green, blue;

      if (x == 0 || x == gnImageW-1 || y == 0 || y == gnImageH-1) {
         return GetOrAllocHistogramIndex(&tgifColors[gnOrigImageIndex[y][x]]);
      }
      memset(&xcolor, 0, sizeof(XColor));
      red = (((((long)tgifColors[gnOrigImageIndex[y][x]].red)<<2) -
            ((long)tgifColors[gnOrigImageIndex[y][x-1]].red) -
            ((long)tgifColors[gnOrigImageIndex[y][x+1]].red) -
            ((long)tgifColors[gnOrigImageIndex[y-1][x]].red) -
            ((long)tgifColors[gnOrigImageIndex[y+1][x]].red))>>2);
      green = (((((long)tgifColors[gnOrigImageIndex[y][x]].green)<<2) -
            ((long)tgifColors[gnOrigImageIndex[y][x-1]].green) -
            ((long)tgifColors[gnOrigImageIndex[y][x+1]].green) -
            ((long)tgifColors[gnOrigImageIndex[y-1][x]].green) -
            ((long)tgifColors[gnOrigImageIndex[y+1][x]].green))>>2);
      blue = (((((long)tgifColors[gnOrigImageIndex[y][x]].blue)<<2) -
            ((long)tgifColors[gnOrigImageIndex[y][x-1]].blue) -
            ((long)tgifColors[gnOrigImageIndex[y][x+1]].blue) -
            ((long)tgifColors[gnOrigImageIndex[y-1][x]].blue) -
            ((long)tgifColors[gnOrigImageIndex[y+1][x]].blue))>>2);
      xcolor.red = (red > 0L ?
            (red > 0x0000ffff ? 0x0ffff : (unsigned int)(red&0xffff)) : 0);
      xcolor.green = (green > 0L ?
            (green > 0x0000ffff ? 0x0ffff : (unsigned int)(green&0xffff)) : 0);
      xcolor.blue = (blue > 0L ?
            (blue > 0x0000ffff ? 0x0ffff : (unsigned int)(blue&0xffff)) : 0);
      return GetOrAllocHistogramIndex(&xcolor);
   }
}

void EdgeDetect()
{
   if (!CheckSelectionForImageProc(CMDID_EDGEDETECT)) {
      return;
   }
   if (topSel->obj->detail.xpm->image_w < 2 ||
         topSel->obj->detail.xpm->image_h < 2) {
      MsgBox(TgLoadString(STID_SEL_TOO_THIN_FLAT_FOR_EDGE), TOOL_NAME, INFO_MB);
      return;
   }
   gpConvolveFunc = (NLFN*)ConvolveToEdgeDetect;
   gpConvolveCmdID = CMDID_EDGEDETECT;
   gnConvolving = TRUE;
   DoImageProc(NULL);
   gnConvolving = FALSE;
   gpConvolveFunc = NULL;
   gpConvolveCmdID = (-1);
}

/* ----------------------- Emboss ----------------------- */

static
int ConvolveToEmboss(x, y)
   int x, y;
{
   if (gConvExtraInfo.fp != NULL) {
      XColor **xcolors=gConvExtraInfo.xcolors;
      FILE *fp=gConvExtraInfo.fp;
      int w=gConvExtraInfo.image_w, h=gConvExtraInfo.image_h;
      unsigned char buf[3];

      if (x == 0 || x == w-1 || y == 0 || y == h-1) {
         double dgray=(double)0;
         int gray=0;

         dgray = 0.299*((double)xcolors[y][x].red) +
               0.587*((double)xcolors[y][x].green) +
               0.144*((double)xcolors[y][x].blue);
         gray = (int)round(dgray);
         if (gray < 0) gray = 0;
         if (gray > 255) gray = 255;
         buf[0] = buf[1] = buf[2] = (unsigned char)(unsigned int)gray;
      } else {
         double d_lt_gray=(double)0, d_rb_gray=(double)0, dgray=(double)0;
         int gray=0;

         d_lt_gray = 0.299*((double)xcolors[y-1][x-1].red) +
               0.587*((double)xcolors[y-1][x-1].green) +
               0.144*((double)xcolors[y-1][x-1].blue);
         d_rb_gray = 0.299*((double)xcolors[y+1][x+1].red) +
               0.587*((double)xcolors[y+1][x+1].green) +
               0.144*((double)xcolors[y+1][x+1].blue);
         dgray = 128.0 + ((d_rb_gray - d_lt_gray) / 2.0);
         gray = (int)round(dgray);
         if (gray < 0) gray = 0;
         if (gray > 255) gray = 255;
         buf[0] = buf[1] = buf[2] = (unsigned char)(unsigned int)gray;
      }
      if ((int)fwrite(buf, sizeof(char), 3, fp) <= 0) writeFileFailed = TRUE;

      return TRUE;
   } else {
      XColor xcolor;

      memset(&xcolor, 0, sizeof(XColor));
      if (x == 0 || x == gnImageW-1 || y == 0 || y == gnImageH-1) {
         float gray=0.299*((float)tgifColors[gnOrigImageIndex[y][x]].red) +
               0.587*((float)tgifColors[gnOrigImageIndex[y][x]].green) +
               0.144*((float)tgifColors[gnOrigImageIndex[y][x]].blue);
   
         xcolor.red = xcolor.green = xcolor.blue = (unsigned short)gray;
         return GetOrAllocHistogramIndex(&xcolor);
      } else {
         float lt_gray, rb_gray;
         int val;

         lt_gray=0.299*((float)tgifColors[gnOrigImageIndex[y-1][x-1]].red) +
               0.587*((float)tgifColors[gnOrigImageIndex[y-1][x-1]].green) +
               0.144*((float)tgifColors[gnOrigImageIndex[y-1][x-1]].blue);
         rb_gray=0.299*((float)tgifColors[gnOrigImageIndex[y+1][x+1]].red) +
               0.587*((float)tgifColors[gnOrigImageIndex[y+1][x+1]].green) +
               0.144*((float)tgifColors[gnOrigImageIndex[y+1][x+1]].blue);
         val = 0x7fff +
               (int)((rb_gray - lt_gray) / 2.0);
         val = ((val>0x0ffff) ? 0x0ffff : ((val<0) ? 0 : val));
         xcolor.red = xcolor.green = xcolor.blue = (unsigned short)val;
         return GetOrAllocHistogramIndex(&xcolor);
      }
   }
}

void Emboss()
{
   if (!CheckSelectionForImageProc(CMDID_EMBOSS)) return;

   if (topSel->obj->detail.xpm->image_w < 2 ||
         topSel->obj->detail.xpm->image_h < 2) {
      MsgBox(TgLoadString(STID_SEL_TOO_THIN_FLAT_FOR_EMBOSS), TOOL_NAME,
            INFO_MB);
      return;
   }
   gpConvolveFunc = (NLFN*)ConvolveToEmboss;
   gpConvolveCmdID = CMDID_EMBOSS;
   gnConvolving = TRUE;
   DoImageProc(NULL);
   gnConvolving = FALSE;
   gpConvolveFunc = NULL;
   gpConvolveCmdID = (-1);
}

/* ----------------------- ReduceColors ----------------------- */

static
int ConvolveToReduceColors(x, y)
   int x, y;
{
   return GetOrAllocHistogramIndex(&tgifColors[gnOrigImageIndex[y][x]]);
}

void ReduceColors()
{
   char *c_ptr, szSpec[MAXSTRING+1], szSpecCopy[MAXSTRING+1];
   int colors_to_reduce_to=0;
   struct XPmRec *xpm_ptr=NULL;

   if (!CheckSelectionForImageProc(CMDID_REDUCECOLORS)) {
      return;
   }
   sprintf(gszMsgBox, TgLoadString(STID_ENTER_NUM_COLORS_TO_REDUCE_TO),
         topSel->obj->detail.xpm->ncolors);
   *szSpec = '\0';
   Dialog(gszMsgBox, TgLoadCachedString(CSTID_DLG_ACCEPT_CANCEL), szSpec);
   UtilTrimBlanks(szSpec);
   if (*szSpec == '\0') return;

   xpm_ptr = topSel->obj->detail.xpm;
   strcpy(szSpecCopy, szSpec);
   if ((c_ptr=strtok(szSpec, " ,\t\n\r")) == NULL) return;
   colors_to_reduce_to = atoi(c_ptr);
   if (DoPpm6(xpm_ptr)) {
      if (colors_to_reduce_to < 2 ||
            colors_to_reduce_to > 256) {
         sprintf(gszMsgBox, TgLoadString(STID_NUM_COLORS_BETWEEN_2_N_GIVEN),
               szSpecCopy, 256);
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
         return;
      }
   } else {
      if (colors_to_reduce_to < 2 ||
            colors_to_reduce_to > topSel->obj->detail.xpm->ncolors) {
         sprintf(gszMsgBox, TgLoadString(STID_NUM_COLORS_BETWEEN_2_N_GIVEN),
               szSpecCopy, topSel->obj->detail.xpm->ncolors);
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
         return;
      }
   }
   gnUserSpecifiedLevels = colors_to_reduce_to;
   gpConvolveFunc = (NLFN*)ConvolveToReduceColors;
   gpConvolveCmdID = CMDID_REDUCECOLORS;
   gnConvolving = TRUE;
   DoImageProc(NULL);
   gnConvolving = FALSE;
   gpConvolveFunc = NULL;
   gpConvolveCmdID = (-1);
   gnUserSpecifiedLevels = (-1);
}

/* ----------------------- ReduceToPixmapColors ----------------------- */

static struct XPmRec gXPmTarget;
static XColor *gTargetColors=NULL;
static int *gTargetColorValid=NULL;
static int *gnObjectColorsToTargetColorMapping=NULL;
static int *gnTgifColorsToObjectColorMapping=NULL;

static int gnFloyd=FALSE;
static int **gnImageTargetColor=NULL;
static XImage *gpTargetImage=NULL, *gpTargetBitmapImage=NULL;

static
void FreeTargetColors(ncolors)
   int ncolors;
{
   if (gTargetColors != NULL) {
      free(gTargetColors);
      gTargetColors = NULL;
   }
   if (gTargetColorValid != NULL) {
      free(gTargetColorValid);
      gTargetColorValid = NULL;
   }
}

static
int AllocTargetColors(ncolors)
   int ncolors;
{
   int i=0;

   gTargetColors = (XColor*)malloc(ncolors*sizeof(XColor));
   if (gTargetColors == NULL) FailAllocMessage();
   memset(gTargetColors, 0, ncolors*sizeof(XColor));

   gTargetColorValid = (int*)malloc(ncolors*sizeof(int));
   if (gTargetColorValid == NULL) FailAllocMessage();
   for (i=0; i < ncolors; i++) {
      gTargetColorValid[i] = TRUE;
   }
   return TRUE;
}

static
int GetClosestColorIndex(red_bits, green_bits, blue_bits, red, green, blue,
      target_ncolors)
   int red_bits, green_bits, blue_bits, red, green, blue, target_ncolors;
{
   int min_index=0;
   unsigned long dr=(double)0, dg=(double)0, db=(double)0, min_dist=(double)0;

   if (gXPmTarget.color_str != NULL ||
         gXPmTarget.userdata == (void*)CMDID_REDUCETOMOBILEWEBCOLORS) {
      /* ReduceToPixmapColors() or ReduceToMobileWebSafeColors() */
      int j=0;

      for (j=0; j < target_ncolors; j++) {
         if (gTargetColorValid[j]) {
            dr = (unsigned long)abs(red-(int)(gTargetColors[j].red));
            dg = (unsigned long)abs(green-(int)(gTargetColors[j].green));
            db = (unsigned long)abs(blue-(int)(gTargetColors[j].blue));
            dr >>= 4; dg >>= 4; db >>= 4;
            break;
         }
      }
      min_index = j;
      min_dist = dr*dr + dg*dg + db*db;
      for (j++; j < target_ncolors; j++) {
         if (gTargetColorValid[j]) {
            unsigned long new_dist;

            dr = (unsigned long)abs(red-(int)(gTargetColors[j].red));
            dg = (unsigned long)abs(green-(int)(gTargetColors[j].green));
            db = (unsigned long)abs(blue-(int)(gTargetColors[j].blue));
            dr >>= 4; dg >>= 4; db >>= 4;
            new_dist = dr*dr + dg*dg + db*db;

            if (new_dist < min_dist) {
               min_index = j;
               min_dist = new_dist;
            }
         }
      }
   } else if (gXPmTarget.userdata == (void*)CMDID_REDUCETOMOBILEWEBCOLORS) {
      /* ReduceToDefaultColors() or DefaultErrorDiffuse() */
      int k, j, i;
      int red_max=(1<<red_bits);
      int green_max=(1<<green_bits);
      int blue_max=(1<<blue_bits);
      int red_index=(red >> (16-red_bits));
      int green_index=(green >> (16-green_bits));
      int blue_index=(blue >> (16-blue_bits));

      min_index = 0;
      dr = (unsigned long)abs(red-(int)(gTargetColors[min_index].red));
      dg = (unsigned long)abs(green-(int)(gTargetColors[min_index].green));
      db = (unsigned long)abs(blue-(int)(gTargetColors[min_index].blue));
      dr >>= 4; dg >>= 4; db >>= 4;

      min_dist = dr*dr + dg*dg + db*db;
      for (i=red_index-1; i <= red_index+1; i++) {
         if (i >= 0 && i < red_max) {
            for (j=green_index-1; j <= green_index+1; j++) {
               if (j >= 0 && j < green_max) {
                  for (k=blue_index-1; k <= blue_index+1; k++) {
                     if (k >= 0 && k < blue_max) {
                        if (i!=red_index || j!=green_index || k!=blue_index) {
                           int index=(((i << green_bits) + j) << blue_bits) + k;
                           unsigned long new_dist;

                           dr = (unsigned long)abs(red -
                                 (int)(gTargetColors[index].red));
                           dg = (unsigned long)abs(green -
                                 (int)(gTargetColors[index].green));
                           db = (unsigned long)abs(blue -
                                 (int)(gTargetColors[index].blue));
                           dr >>= 4; dg >>= 4; db >>= 4;
                           new_dist = dr*dr + dg*dg + db*db;
                           if (new_dist < min_dist) {
                              min_index = index;
                              min_dist = new_dist;
                           }
                        }
                     }
                  }
               }
            }
         }
      }
   } else {
      /* ReduceToDefaultColors() or DefaultErrorDiffuse() */
      int k, j, i;
      int red_max=(1<<red_bits);
      int green_max=(1<<green_bits);
      int blue_max=(1<<blue_bits);
      int red_index=(red >> (16-red_bits));
      int green_index=(green >> (16-green_bits));
      int blue_index=(blue >> (16-blue_bits));

      min_index = (((red_index << green_bits) + green_index) << blue_bits) +
            blue_index;
      dr = (unsigned long)abs(red-(int)(gTargetColors[min_index].red));
      dg = (unsigned long)abs(green-(int)(gTargetColors[min_index].green));
      db = (unsigned long)abs(blue-(int)(gTargetColors[min_index].blue));
      dr >>= 4; dg >>= 4; db >>= 4;

      min_dist = dr*dr + dg*dg + db*db;
      for (i=red_index-1; i <= red_index+1; i++) {
         if (i >= 0 && i < red_max) {
            for (j=green_index-1; j <= green_index+1; j++) {
               if (j >= 0 && j < green_max) {
                  for (k=blue_index-1; k <= blue_index+1; k++) {
                     if (k >= 0 && k < blue_max) {
                        if (i!=red_index || j!=green_index || k!=blue_index) {
                           int index=(((i << green_bits) + j) << blue_bits) + k;
                           unsigned long new_dist;

                           dr = (unsigned long)abs(red -
                                 (int)(gTargetColors[index].red));
                           dg = (unsigned long)abs(green -
                                 (int)(gTargetColors[index].green));
                           db = (unsigned long)abs(blue -
                                 (int)(gTargetColors[index].blue));
                           dr >>= 4; dg >>= 4; db >>= 4;
                           new_dist = dr*dr + dg*dg + db*db;
                           if (new_dist < min_dist) {
                              min_index = index;
                              min_dist = new_dist;
                           }
                        }
                     }
                  }
               }
            }
         }
      }
   }
   return min_index;
}
static
int ConvolveToErrorDiffuse(x, y)
   int x, y;
{
   int targetcolor_index=gnImageTargetColor[y][x];

   if (gConvExtraInfo.fp != NULL) {
      FILE *fp=gConvExtraInfo.fp;
      double dr=(double)0, dg=(double)0, db=(double)0;
      unsigned char buf[3];
      unsigned int r=(unsigned int)(gTargetColors[targetcolor_index].red);
      unsigned int g=(unsigned int)(gTargetColors[targetcolor_index].green);
      unsigned int b=(unsigned int)(gTargetColors[targetcolor_index].blue);

      dr = ((double)r) / maxRGB * ((double)255);
      dg = ((double)g) / maxRGB * ((double)255);
      db = ((double)b) / maxRGB * ((double)255);
      r = (dr < ((double)0)) ? 0 : ((unsigned int)dr);
      g = (dg < ((double)0)) ? 0 : ((unsigned int)dg);
      b = (db < ((double)0)) ? 0 : ((unsigned int)db);
      if (r > 255) r = 255;
      if (g > 255) g = 255;
      if (b > 255) b = 255;
      buf[0] = (unsigned char)r;
      buf[1] = (unsigned char)g;
      buf[2] = (unsigned char)b;
      if ((int)fwrite(buf, sizeof(char), 3, fp) <= 0) writeFileFailed = TRUE;

      return TRUE;
   } else {
      return GetOrAllocHistogramIndex(&gTargetColors[targetcolor_index]);
   }
}

static
int ConvolveToReduceToPixmapColors(x, y)
   int x, y;
{
   if (gConvExtraInfo.fp != NULL) {
      int index=0, pixel=XGetPixel(gConvExtraInfo.image,x,y);
      FILE *fp=gConvExtraInfo.fp;
      uint32_t pix=(uint32_t)(unsigned int)pixel;
      unsigned int r=0, g=0, b=0;
      double dr=(double)0, dg=(double)0, db=(double)0;
      unsigned char buf[3];

      r = (pix & gTrueColorInfo.r_mask) >> gTrueColorInfo.r_shift;
      g = (pix & gTrueColorInfo.g_mask) >> gTrueColorInfo.g_shift;
      b = (pix & gTrueColorInfo.b_mask) >> gTrueColorInfo.b_shift;
      dr = ((double)r) / gTrueColorInfo.dr_maxval * ((double)maxRGB);
      dg = ((double)g) / gTrueColorInfo.dg_maxval * ((double)maxRGB);
      db = ((double)b) / gTrueColorInfo.db_maxval * ((double)maxRGB);

      r = (dr < ((double)0)) ? 0 : ((unsigned int)dr);
      g = (dg < ((double)0)) ? 0 : ((unsigned int)dg);
      b = (db < ((double)0)) ? 0 : ((unsigned int)db);
      if (r > maxRGB) r = maxRGB;
      if (g > maxRGB) g = maxRGB;
      if (b > maxRGB) b = maxRGB;

      index = GetClosestColorIndex(gDefErrorDiffuseLevel.red,
            gDefErrorDiffuseLevel.green, gDefErrorDiffuseLevel.blue,
            (int)r, (int)g, (int)b, gnUserSpecifiedLevels);

      r = (unsigned int)(gTargetColors[index].red);
      g = (unsigned int)(gTargetColors[index].green);
      b = (unsigned int)(gTargetColors[index].blue);

      dr = ((double)r) / maxRGB * ((double)255);
      dg = ((double)g) / maxRGB * ((double)255);
      db = ((double)b) / maxRGB * ((double)255);
      r = (dr < ((double)0)) ? 0 : ((unsigned int)dr);
      g = (dg < ((double)0)) ? 0 : ((unsigned int)dg);
      b = (db < ((double)0)) ? 0 : ((unsigned int)db);
      if (r > 255) r = 255;
      if (g > 255) g = 255;
      if (b > 255) b = 255;
      buf[0] = (unsigned char)r;
      buf[1] = (unsigned char)g;
      buf[2] = (unsigned char)b;
      if ((int)fwrite(buf, sizeof(char), 3, fp) <= 0) writeFileFailed = TRUE;

      return TRUE;
   } else {
      int tgifcolor_index=gnOrigImageIndex[y][x]; /* index into tgifColors */
      int objcolor_index=gnTgifColorsToObjectColorMapping[tgifcolor_index];
      int targetcolor_index=gnObjectColorsToTargetColorMapping[objcolor_index];

      return GetOrAllocHistogramIndex(&gTargetColors[targetcolor_index]);
   }
}

static
int GetIntensity(index, max_levels)
   int index, max_levels; /* 0 < max_levels <= 255 */
   /*
    * May be it should be like the following:
    *
    * int ival=(int)(((double)index)/((double)max_levels)*((double)maxRGB));
    * 
    * return (ival < 0 ? 0 : (ival > maxRGB ? maxRGB : ival));
    */
{
   int ival=(int)(((double)index)/((double)max_levels)*((double)255));

   return ((ival < 0 ? 0 : (ival > 0xff ? 0xff : ival))<<8);
}

#define FS_SCALE 0x400
#define HALF_FS_SCALE 0x200

static
int DoPrepareForErrorDiffuse(xpm_ptr)
   struct XPmRec *xpm_ptr;
{
   int j, i, target_ncolors=gXPmTarget.ncolors, fs_forward=TRUE;
   int image_w=xpm_ptr->image_w, image_h=xpm_ptr->image_h;
   int red_bits=0, green_bits=0, blue_bits=0;
   long *this_r_err, *this_g_err, *this_b_err, *tmp_err;
   long *next_r_err, *next_g_err, *next_b_err;
   ProgressInfo pi;

   if (!CreatePixelToIndexMapping()) return FALSE;
   if (gXPmTarget.color_str != NULL) {
      for (i=0; i < target_ncolors; i++) {
         if (!TgifParseColor(gXPmTarget.color_str[i], &gTargetColors[i])) {
            gTargetColorValid[i] = FALSE;
         }
      }
   } else {
      red_bits = gDefErrorDiffuseLevel.red;
      green_bits = gDefErrorDiffuseLevel.green;
      blue_bits = gDefErrorDiffuseLevel.blue;
   }
   if (!GetXPmImages(xpm_ptr, &gpTargetImage, &gpTargetBitmapImage)) {
      return FALSE;
   }
   gnImageTargetColor = (int**)malloc(image_h*sizeof(int*));
   if (gnImageTargetColor == NULL) return FailAllocMessage();
   for (i=0; i < image_h; i++) {
      if ((gnImageTargetColor[i]=(int*)malloc(image_w*sizeof(int))) == NULL) {
         for (j=0; j < i; j++) free(gnImageTargetColor[j]);
         free(gnImageTargetColor);
         gnImageTargetColor = NULL;
         return FailAllocMessage();
      }
   }
   this_r_err = (long*)malloc((image_w+2)*sizeof(long));
   this_g_err = (long*)malloc((image_w+2)*sizeof(long));
   this_b_err = (long*)malloc((image_w+2)*sizeof(long));
   next_r_err = (long*)malloc((image_w+2)*sizeof(long));
   next_g_err = (long*)malloc((image_w+2)*sizeof(long));
   next_b_err = (long*)malloc((image_w+2)*sizeof(long));
   if (this_r_err==NULL || this_g_err==NULL || this_b_err==NULL ||
         next_r_err==NULL || next_g_err==NULL || next_b_err==NULL) {
      if (this_r_err != NULL) free(this_r_err);
      if (this_g_err != NULL) free(this_g_err);
      if (this_b_err != NULL) free(this_b_err);
      if (next_r_err != NULL) free(next_r_err);
      if (next_g_err != NULL) free(next_g_err);
      if (next_b_err != NULL) free(next_b_err);
      return FALSE;
   }
   srand(0);
   for (j=0; j < image_w+2; j++) {
      this_r_err[j] = (rand() % FS_SCALE - HALF_FS_SCALE) << 4;
      this_g_err[j] = (rand() % FS_SCALE - HALF_FS_SCALE) << 4;
      this_b_err[j] = (rand() % FS_SCALE - HALF_FS_SCALE) << 4;
   }

   BeginProgress(&pi, image_h);
   for (i=0; i < image_h; i++) {
      int col, limitcol;

      UpdateProgress(&pi, i);
      /*--------------------------------------------------------*/
      /* the error diffusion code is adapted from 'ppmquant.c', */
      /*       which is part of the netpbm package.             */
      /*                                                        */
      /*       Copyright (C) 1989, 1991 by Jef Poskanzer        */
      /*--------------------------------------------------------*/
      col = (fs_forward ? 0 : image_w-1);
      limitcol = (fs_forward ? image_w : (-1));
      for (j=0; j < image_w+2; j++) {
         next_r_err[j] = next_g_err[j] = next_b_err[j] = 0L;
      }
      do {
         int red, green, blue, err, min_index=0;
         int pixel=XGetPixel(gpTargetImage, col, i);

         if (DoPpm6(xpm_ptr)) {
            uint32_t pix=(uint32_t)(unsigned int)pixel;
            unsigned int r=0, g=0, b=0;
            double dr=(double)0, dg=(double)0, db=(double)0;

            r = (pix & gTrueColorInfo.r_mask) >> gTrueColorInfo.r_shift;
            g = (pix & gTrueColorInfo.g_mask) >> gTrueColorInfo.g_shift;
            b = (pix & gTrueColorInfo.b_mask) >> gTrueColorInfo.b_shift;
            dr = ((double)r) / gTrueColorInfo.dr_maxval * ((double)65535);
            dg = ((double)g) / gTrueColorInfo.dg_maxval * ((double)65535);
            db = ((double)b) / gTrueColorInfo.db_maxval * ((double)65535);

            r = (dr < ((double)0)) ? 0 : ((unsigned int)dr);
            g = (dg < ((double)0)) ? 0 : ((unsigned int)dg);
            b = (db < ((double)0)) ? 0 : ((unsigned int)db);
            if (r > 65535) r = 65535;
            if (g > 65535) g = 65535;
            if (b > 65535) b = 65535;

            red = (int)(r) + this_r_err[col+1];
            green = (int)(g) + this_g_err[col+1];
            blue = (int)(b) + this_b_err[col+1];
            if (red < 0) red = 0; if (red > 65535) red = 65535;
            if (green < 0) green = 0; if (green > 65535) green = 65535;
            if (blue < 0) blue = 0; if (blue > 65535) blue = 65535;

            min_index = GetClosestColorIndex(gDefErrorDiffuseLevel.red,
                  gDefErrorDiffuseLevel.green, gDefErrorDiffuseLevel.blue,
                  (int)red, (int)green, (int)blue, target_ncolors);
         } else {
            int index=GetIndexOfPixel(pixel);

            red = (int)(tgifColors[index].red) + this_r_err[col+1];
            green = (int)(tgifColors[index].green) + this_g_err[col+1];
            blue = (int)(tgifColors[index].blue) + this_b_err[col+1];
            if (red < 0) red = 0; if (red > 65535) red = 65535;
            if (green < 0) green = 0; if (green > 65535) green = 65535;
            if (blue < 0) blue = 0; if (blue > 65535) blue = 65535;

            min_index = GetClosestColorIndex(red_bits, green_bits, blue_bits,
                  red, green, blue, target_ncolors);
         }
         gnImageTargetColor[i][col] = min_index;
         if (fs_forward) {
            err = (red - gTargetColors[min_index].red); /* * FS_SCALE; */
            this_r_err[col+2] += (err*7) >> 4;
            next_r_err[col  ] += (err*3) >> 4;
            next_r_err[col+1] += (err*5) >> 4;
            next_r_err[col+2] += (err  ) >> 4;
            err = (green - gTargetColors[min_index].green); /* * FS_SCALE; */
            this_g_err[col+2] += (err*7) >> 4;
            next_g_err[col  ] += (err*3) >> 4;
            next_g_err[col+1] += (err*5) >> 4;
            next_g_err[col+2] += (err  ) >> 4;
            err = (blue - gTargetColors[min_index].blue); /* * FS_SCALE; */
            this_b_err[col+2] += (err*7) >> 4;
            next_b_err[col  ] += (err*3) >> 4;
            next_b_err[col+1] += (err*5) >> 4;
            next_b_err[col+2] += (err  ) >> 4;

            col++;
         } else {
            err = (red - gTargetColors[min_index].red); /* * FS_SCALE; */
            this_r_err[col  ] += (err*7) >> 4;
            next_r_err[col+2] += (err*3) >> 4;
            next_r_err[col+1] += (err*5) >> 4;
            next_r_err[col  ] += (err  ) >> 4;
            err = (green - gTargetColors[min_index].green); /* * FS_SCALE; */
            this_g_err[col  ] += (err*7) >> 4;
            next_g_err[col+2] += (err*3) >> 4;
            next_g_err[col+1] += (err*5) >> 4;
            next_g_err[col  ] += (err  ) >> 4;
            err = (blue - gTargetColors[min_index].blue); /* * FS_SCALE; */
            this_b_err[col  ] += (err*7) >> 4;
            next_b_err[col+2] += (err*3) >> 4;
            next_b_err[col+1] += (err*5) >> 4;
            next_b_err[col  ] += (err  ) >> 4;

            col--;
         }
      } while (col != limitcol);
      tmp_err = this_r_err; this_r_err = next_r_err; next_r_err = tmp_err;
      tmp_err = this_g_err; this_g_err = next_g_err; next_g_err = tmp_err;
      tmp_err = this_b_err; this_b_err = next_b_err; next_b_err = tmp_err;
      fs_forward = !fs_forward;
   }
   free(this_r_err); free(this_g_err); free(this_b_err);
   free(next_r_err); free(next_g_err); free(next_b_err);

   return TRUE;
}

static
int PrepareForErrorDiffuse(xpm_ptr)
   struct XPmRec *xpm_ptr;
{
   int rc;

   SaveStatusStrings();
   SetWatchCursor(drawWindow);
   SetWatchCursor(mainWindow);
   rc = DoPrepareForErrorDiffuse(xpm_ptr);
   SetDefaultCursor(mainWindow);
   ShowCursor();
   RestoreStatusStrings();
   return rc;
}

static
int MapTargetColors(xpm_ptr)
   struct XPmRec *xpm_ptr;
{
   int i, target_ncolors=gXPmTarget.ncolors;
   int red_bits=0, green_bits=0, blue_bits=0;

   if (gXPmTarget.color_str != NULL) {
      for (i=0; i < target_ncolors; i++) {
         if (!TgifParseColor(gXPmTarget.color_str[i], &gTargetColors[i])) {
            gTargetColorValid[i] = FALSE;
         }
      }
   } else {
      red_bits = gDefErrorDiffuseLevel.red;
      green_bits = gDefErrorDiffuseLevel.green;
      blue_bits = gDefErrorDiffuseLevel.blue;
   }
   gnTgifColorsToObjectColorMapping = (int*)malloc(maxColors*sizeof(int));
   if (gnTgifColorsToObjectColorMapping == NULL) {
      FailAllocMessage();
      return FALSE;
   }
   memset(gnTgifColorsToObjectColorMapping, (-1), maxColors*sizeof(int));

   for (i=0; i < maxColors; i++) {
      int cur_pixel=colorPixels[i], ncolors=xpm_ptr->ncolors;
      int found_index, *pixels=xpm_ptr->pixels;
      int red, green, blue, min_index;

      for (found_index=0; found_index < ncolors; found_index++) {
         if (pixels[found_index] == cur_pixel) {
            break;
         }
      }
      if (found_index >= ncolors) {
         continue;
      }
      gnTgifColorsToObjectColorMapping[i] = found_index;

      red = (int)(tgifColors[i].red);
      green = (int)(tgifColors[i].green);
      blue = (int)(tgifColors[i].blue);

      min_index = GetClosestColorIndex(red_bits, green_bits, blue_bits,
            red, green, blue, target_ncolors);

      gnObjectColorsToTargetColorMapping[found_index] = min_index;
   }
   return TRUE;
}

void ReduceToPixmapColors()
{
   int i, ok=TRUE, rc, short_name=FALSE;
   char xpm_fname[MAXPATHLENGTH+1], *rest=NULL, tmp_fname[MAXPATHLENGTH+1];
   XEvent ev;
   int ncolors=0, chars_per_pixel=0, first_pixel_is_bg=0, remote_file=FALSE;
   char *color_char=NULL, **color_str=NULL, *xpm_data=NULL;
   struct XPmRec *xpm_ptr=NULL;

   if (!CheckSelectionForImageProc(CMDID_REDUCETOPIXMAPCOLORS)) {
      return;
   }
   importingFile = TRUE;
   *xpm_fname = *tmp_fname = '\0';
   gnObjectColorsToTargetColorMapping = NULL;
   if (importFromLibrary) {
      char name[MAXSTRING+1], path[MAXSTRING+1];

      if (SelectFromLibrary(TgLoadString(STID_SEL_XPM_FILE_FOR_RED_COLORS),
            XPM_FILE_EXT, name, path) == INVALID) {
         ok = FALSE;
      } else {
         sprintf(xpm_fname, "%s%c%s", path, DIR_SEP, name);
      }
   } else if (SelectFileNameToImport(
         TgLoadString(STID_SEL_XPM_FILE_FOR_RED_COLORS), XPM_FILE_EXT,
         xpm_fname) == INVALID) {
      ok = FALSE;
   } else if (FileIsRemote(xpm_fname)) {
      int is_html=FALSE;

      if (!DownloadRemoteFile(xpm_fname, NULL, NULL, &is_html, tmp_fname, NULL,
           0) || *tmp_fname == '\0') {
         ok = FALSE;
      } else {
         remote_file = TRUE;
      }
   }
   importingFile = FALSE;
   if (!ok) {
      if (remote_file) unlink(tmp_fname);
      return;
   }
   XSync(mainDisplay, False);
   if (XCheckMaskEvent(mainDisplay, ExposureMask, &ev)) {
      ExposeEventHandler(&ev, TRUE);
   }
   xpm_ptr = topSel->obj->detail.xpm;
   if (DoPpm6(xpm_ptr)) {
      gnObjectColorsToTargetColorMapping = NULL;
   } else {
      gnObjectColorsToTargetColorMapping =
            (int*)malloc(xpm_ptr->ncolors*sizeof(int));
      if (gnObjectColorsToTargetColorMapping == NULL) {
         FailAllocMessage();
         if (remote_file) unlink(tmp_fname);
         return;
      }
      memset(gnObjectColorsToTargetColorMapping, 0,
            xpm_ptr->ncolors*sizeof(int));
   }
   SetWatchCursor(drawWindow);
   SetWatchCursor(mainWindow);
   rc = MyReadPixmapFile(remote_file ? tmp_fname : xpm_fname, NULL, NULL, NULL,
         NULL, NULL, NULL, NULL, NULL, &ncolors, &chars_per_pixel,
         &first_pixel_is_bg, &color_char, &color_str, NULL, &xpm_data);
   SetDefaultCursor(mainWindow);
   ShowCursor();

   if (remote_file) {
      short_name = FALSE;
   } else {
      if ((short_name=IsPrefix(bootDir, xpm_fname, &rest))) ++rest;
   }
   if (rc != BitmapSuccess) {
      sprintf(gszMsgBox, TgLoadString(STID_INVALID_GIVEN_XPM_FILE),
            (short_name ? rest : xpm_fname));
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      if (gnObjectColorsToTargetColorMapping != NULL) {
         free(gnObjectColorsToTargetColorMapping);
         gnObjectColorsToTargetColorMapping = NULL;
      }
      if (remote_file) unlink(tmp_fname);
      return;
   }
   gXPmTarget.ncolors = ncolors;
   gXPmTarget.chars_per_pixel = chars_per_pixel;
   gXPmTarget.first_pixel_is_bg = first_pixel_is_bg;
   gXPmTarget.color_char = color_char;
   gXPmTarget.color_str = color_str;
   gXPmTarget.data = xpm_data;
   gXPmTarget.userdata = (void*)CMDID_REDUCETOPIXMAPCOLORS;

   if (ncolors <= 0) {
      sprintf(gszMsgBox, TgLoadString(STID_INVALID_GIVEN_XPM_FILE),
            (short_name ? rest : xpm_fname));
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
   } else {
      int image_h=xpm_ptr->image_h;

      rc = MsgBox(TgLoadString(STID_Q_FS_ERROR_DIFFUSE), TOOL_NAME, YNC_MB);
      switch (rc) {
      case MB_ID_YES: gnFloyd = TRUE; break;
      case MB_ID_NO: gnFloyd = FALSE; break;
      case MB_ID_CANCEL:
         if (gnObjectColorsToTargetColorMapping != NULL) {
            free(gnObjectColorsToTargetColorMapping);
            gnObjectColorsToTargetColorMapping = NULL;
         }
         if (remote_file) unlink(tmp_fname);
         return;
      }
      AllocTargetColors(ncolors);
      if (gnFloyd) {
         if (DoPpm6(xpm_ptr)) {
            if (!InitTrueColorInfo(xpm_ptr->image, &gTrueColorInfo,
                  xpm_ptr->image_w)) {
               sprintf(gszMsgBox, TgLoadString(STID_CANNOT_GET_IMAGE_OF_SIZE),
                     xpm_ptr->image_w, xpm_ptr->image_h);
               MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
               if (gnObjectColorsToTargetColorMapping != NULL) {
                  free(gnObjectColorsToTargetColorMapping);
                  gnObjectColorsToTargetColorMapping = NULL;
               }
               if (remote_file) unlink(tmp_fname);
               return;
            }
         }
         rc = PrepareForErrorDiffuse(xpm_ptr);
         CleanUpIndexOfPixel();
         if (gpTargetImage != NULL) XDestroyImage(gpTargetImage);
         if (gpTargetBitmapImage != NULL) XDestroyImage(gpTargetBitmapImage);
         gpTargetImage = gpTargetBitmapImage = NULL;
      } else {
         if (DoPpm6(xpm_ptr)) {
            if (gXPmTarget.color_str != NULL) {
               for (i=0; i < ncolors; i++) {
                  if (!TgifParseColor(gXPmTarget.color_str[i],
                        &gTargetColors[i])) {
                     gTargetColorValid[i] = FALSE;
                  }
               }
            }
            rc = TRUE;
         } else {
            rc = MapTargetColors(xpm_ptr);
         }
      }
      if (rc) {
         gnUserSpecifiedLevels = ncolors;
         if (gnFloyd) {
            gpConvolveFunc = (NLFN*)ConvolveToErrorDiffuse;
            gpConvolveCmdID = CMDID_REDUCETOPIXMAPCOLORS;
         } else {
            gpConvolveFunc = (NLFN*)ConvolveToReduceToPixmapColors;
            gpConvolveCmdID = CMDID_REDUCETOPIXMAPCOLORS;
         }
         gnConvolving = TRUE;
         DoImageProc(NULL);
         gnConvolving = FALSE;
         gpConvolveFunc = NULL;
         gpConvolveCmdID = (-1);
         gnUserSpecifiedLevels = (-1);
      }
      FreeTargetColors();
      if (gnFloyd) {
         if (gnImageTargetColor != NULL) {
            for (i=0; i < image_h; i++) free(gnImageTargetColor[i]);
            free(gnImageTargetColor);
            gnImageTargetColor = NULL;
         }
      }
      gnFloyd = FALSE;
   }
   if (gXPmTarget.color_char != NULL) free(gXPmTarget.color_char);
   if (gXPmTarget.color_str != NULL) {
      for (i=0; i < gXPmTarget.ncolors; i++) free(gXPmTarget.color_str[i]);
      free(gXPmTarget.color_str);
   }
   if (gXPmTarget.data != NULL) free(gXPmTarget.data);
   memset(&gXPmTarget, 0, sizeof(struct XPmRec));

   if (gnObjectColorsToTargetColorMapping != NULL) {
      free(gnObjectColorsToTargetColorMapping);
      gnObjectColorsToTargetColorMapping = NULL;
   }
   if (gnTgifColorsToObjectColorMapping != NULL) {
      free(gnTgifColorsToObjectColorMapping);
      gnTgifColorsToObjectColorMapping = NULL;
   }
   if (remote_file) unlink(tmp_fname);
}

/* ---------------------- ReduceToMobileWebSafeColors ---------------------- */

static
int GetMobileWebSafeIntensity(index)
   int index;
{
   switch (index) {
   case 0: return 0x00000;
   case 1: return 0x03300;
   case 2: return 0x06600;
   case 3: return 0x09900;
   case 4: return 0x0cc00;
   case 5: return 0x0ff00;
   }
   TgAssert(FALSE, "invalid parameter passed to GetMobileWebSafeIntensity()",
         NULL);
   return (-1);
}

void ReduceToMobileWebSafeColors()
{
   int i, red=0, rc=FALSE, ncolors=0, image_h=0;
   struct XPmRec *xpm_ptr=NULL;

   if (!CheckSelectionForImageProc(CMDID_REDUCETOMOBILEWEBCOLORS)) {
      return;
   }
   xpm_ptr = topSel->obj->detail.xpm;
   image_h = xpm_ptr->image_h;

   if (DoPpm6(xpm_ptr)) {
      gnObjectColorsToTargetColorMapping = NULL;
   } else {
      gnObjectColorsToTargetColorMapping =
            (int*)malloc(xpm_ptr->ncolors*sizeof(int));
      if (gnObjectColorsToTargetColorMapping == NULL) {
         FailAllocMessage();
         return;
      }
      memset(gnObjectColorsToTargetColorMapping, 0,
            xpm_ptr->ncolors*sizeof(int));
   }
   ncolors = 6*6*6;

   memset(&gXPmTarget, 0, sizeof(struct XPmRec));
   gXPmTarget.ncolors = ncolors;
   gXPmTarget.userdata = (void*)CMDID_REDUCETOMOBILEWEBCOLORS;

   rc = MsgBox(TgLoadString(STID_Q_FS_ERROR_DIFFUSE), TOOL_NAME, YNC_MB);
   switch (rc) {
   case MB_ID_YES: gnFloyd = TRUE; break;
   case MB_ID_NO: gnFloyd = FALSE; break;
   case MB_ID_CANCEL:
      if (gnObjectColorsToTargetColorMapping != NULL) {
         free(gnObjectColorsToTargetColorMapping);
         gnObjectColorsToTargetColorMapping = NULL;
      }
      return;
   }
   AllocTargetColors(ncolors);
   i = 0;
   for (red=0; red < 6; red++) {
      int red_intensity=GetMobileWebSafeIntensity(red), green;

      for (green=0; green < 6; green++) {
         int green_intensity=GetMobileWebSafeIntensity(green), blue;

         for (blue=0; blue < 6; blue++) {
            int blue_intensity=GetMobileWebSafeIntensity(blue);

            gTargetColors[i].red = red_intensity;
            gTargetColors[i].green = green_intensity;
            gTargetColors[i].blue = blue_intensity;
            i++;
         }
      }
   }
   if (gnFloyd) {
      if (DoPpm6(xpm_ptr)) {
         if (!InitTrueColorInfo(xpm_ptr->image, &gTrueColorInfo,
               xpm_ptr->image_w)) {
            sprintf(gszMsgBox, TgLoadString(STID_CANNOT_GET_IMAGE_OF_SIZE),
                  xpm_ptr->image_w, xpm_ptr->image_h);
            MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
            if (gnObjectColorsToTargetColorMapping != NULL) {
               free(gnObjectColorsToTargetColorMapping);
               gnObjectColorsToTargetColorMapping = NULL;
            }
            return;
         }
      }
      rc = PrepareForErrorDiffuse(xpm_ptr);
      CleanUpIndexOfPixel();
      if (gpTargetImage != NULL) XDestroyImage(gpTargetImage);
      if (gpTargetBitmapImage != NULL) XDestroyImage(gpTargetBitmapImage);
      gpTargetImage = gpTargetBitmapImage = NULL;
   } else {
      if (DoPpm6(xpm_ptr)) {
         rc = TRUE;
      } else {
         rc = MapTargetColors(xpm_ptr);
      }
   }
   if (rc) {
      gnUserSpecifiedLevels = ncolors;
      if (gnFloyd) {
         gpConvolveFunc = (NLFN*)ConvolveToErrorDiffuse;
         gpConvolveCmdID = CMDID_REDUCETOMOBILEWEBCOLORS;
      } else {
         gpConvolveFunc = (NLFN*)ConvolveToReduceToPixmapColors;
         gpConvolveCmdID = CMDID_REDUCETOMOBILEWEBCOLORS;
      }
      gnConvolving = TRUE;
      DoImageProc(NULL);
      gnConvolving = FALSE;
      gpConvolveFunc = NULL;
      gpConvolveCmdID = (-1);
      gnUserSpecifiedLevels = (-1);
   }
   FreeTargetColors();
   if (gnFloyd) {
      if (gnImageTargetColor != NULL) {
         for (i=0; i < image_h; i++) free(gnImageTargetColor[i]);
         free(gnImageTargetColor);
         gnImageTargetColor = NULL;
      }
   }
   memset(&gXPmTarget, 0, sizeof(struct XPmRec));

   if (gnObjectColorsToTargetColorMapping != NULL) {
      free(gnObjectColorsToTargetColorMapping);
      gnObjectColorsToTargetColorMapping = NULL;
   }
   if (gnTgifColorsToObjectColorMapping != NULL) {
      free(gnTgifColorsToObjectColorMapping);
      gnTgifColorsToObjectColorMapping = NULL;
   }
}

/* ----------------------- SetDefaultColorLevels ----------------------- */

#define PDCL_OK 0
#define PDCL_TOO_LARGE 1
#define PDCL_TOO_SMALL 2
#define PDCL_BAD_FORMAT 3

static
int ParseDefaultColorLevels(buf, p_xcolor)
   char *buf;
   XColor *p_xcolor;
{
   char *red_ptr, *green_ptr, *blue_ptr;

   if ((red_ptr=strtok(buf, " ,:\t\n\r")) != NULL &&
         (green_ptr=strtok(NULL, " ,:\t\n\r")) != NULL &&
         (blue_ptr=strtok(NULL, " ,:\t\n\r")) != NULL) {
      int red, green, blue;

      red = atoi(red_ptr);
      green = atoi(green_ptr);
      blue = atoi(blue_ptr);
      if (red+green+blue > 8) {
         return PDCL_TOO_LARGE;
      } else if (red <= 0 || green <= 0 || blue <= 0) {
         return PDCL_TOO_SMALL;
      } else {
         p_xcolor->red = red;
         p_xcolor->green = green;
         p_xcolor->blue = blue;
         return PDCL_OK;
      }
   }
   return PDCL_BAD_FORMAT;
}

void SetDefaultColorLevels()
{
   char szSpec[MAXSTRING+1], szSpecCopy[MAXSTRING+1];
   XColor xcolor;

   sprintf(gszMsgBox, TgLoadString(STID_ENTER_NUM_BITS_IN_RGB),
         (int)gDefErrorDiffuseLevel.red, (int)gDefErrorDiffuseLevel.green,
         (int)gDefErrorDiffuseLevel.blue);
   *szSpec = '\0';
   Dialog(gszMsgBox, TgLoadCachedString(CSTID_DLG_ACCEPT_CANCEL), szSpec);
   UtilTrimBlanks(szSpec);
   if (*szSpec == '\0') return;

   strcpy(szSpecCopy, szSpec);
   switch (ParseDefaultColorLevels(szSpecCopy, &xcolor)) {
   case PDCL_OK:
      gDefErrorDiffuseLevel.red = xcolor.red;
      gDefErrorDiffuseLevel.green = xcolor.green;
      gDefErrorDiffuseLevel.blue = xcolor.blue;
      Msg(TgLoadString(STID_RGB_LEVELS_CHANGED_TO));
      break;
   case PDCL_TOO_LARGE:
      sprintf(gszMsgBox, TgLoadString(STID_BAD_VAL_SUM_RGB_LEVEL), szSpec);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      break;
   case PDCL_TOO_SMALL:
      sprintf(gszMsgBox, TgLoadString(STID_BAD_VAL_GT_0_RGB_LEVEL), szSpec);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      break;
   case PDCL_BAD_FORMAT:
      sprintf(gszMsgBox, TgLoadString(STID_FAIL_TO_PARSE_FOR_3_VAL), szSpec);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      break;
   }
}

/* ----------------------- ReduceToDefaultColors ----------------------- */

void ReduceToDefaultColors()
{
   int i, red=0, rc=FALSE, ncolors=0, image_h=0;
   int red_bits, green_bits, blue_bits, red_levels, green_levels, blue_levels;
   struct XPmRec *xpm_ptr=NULL;

   if (!CheckSelectionForImageProc(CMDID_DEFAULTERRORDIFFUSE)) {
      return;
   }
   xpm_ptr = topSel->obj->detail.xpm;
   image_h = xpm_ptr->image_h;

   if (DoPpm6(xpm_ptr)) {
      gnObjectColorsToTargetColorMapping = NULL;
   } else {
      gnObjectColorsToTargetColorMapping =
            (int*)malloc(xpm_ptr->ncolors*sizeof(int));
      if (gnObjectColorsToTargetColorMapping == NULL) {
         FailAllocMessage();
         return;
      }
      memset(gnObjectColorsToTargetColorMapping, 0,
            xpm_ptr->ncolors*sizeof(int));
   }
   red_bits = gDefErrorDiffuseLevel.red;
   green_bits = gDefErrorDiffuseLevel.green;
   blue_bits = gDefErrorDiffuseLevel.blue;
   sprintf(gszMsgBox, TgLoadString(STID_RGB_LEVELS_ARE),
         red_bits, green_bits, blue_bits);
   Msg(gszMsgBox);
   red_levels = (1 << red_bits);
   green_levels = (1 << green_bits);
   blue_levels = (1 << blue_bits);
   ncolors = (1 << (red_bits+green_bits+blue_bits));
   if (ncolors > 0x100) ncolors = 0x100;

   memset(&gXPmTarget, 0, sizeof(struct XPmRec));
   gXPmTarget.ncolors = ncolors;
   gXPmTarget.userdata = (void*)CMDID_DEFAULTERRORDIFFUSE;

   AllocTargetColors(ncolors);
   i = 0;
   for (red=0; red < red_levels; red++) {
      int red_intensity=GetIntensity(red, red_levels-1), green;

      for (green=0; green < green_levels; green++) {
         int green_intensity=GetIntensity(green, green_levels-1), blue;

         for (blue=0; blue < blue_levels; blue++) {
            int blue_intensity=GetIntensity(blue, blue_levels-1);

            gTargetColors[i].red = red_intensity;
            gTargetColors[i].green = green_intensity;
            gTargetColors[i].blue = blue_intensity;
            i++;
         }
      }
   }
   if (DoPpm6(xpm_ptr)) {
      rc = TRUE;
   } else {
      rc = MapTargetColors(xpm_ptr);
   }
   if (rc) {
      gnUserSpecifiedLevels = ncolors;
      gpConvolveFunc = (NLFN*)ConvolveToReduceToPixmapColors;
      gpConvolveCmdID = CMDID_DEFAULTERRORDIFFUSE;
      gnConvolving = TRUE;
      DoImageProc(NULL);
      gnConvolving = FALSE;
      gpConvolveFunc = NULL;
      gpConvolveCmdID = (-1);
      gnUserSpecifiedLevels = (-1);
   }
   FreeTargetColors();
   memset(&gXPmTarget, 0, sizeof(struct XPmRec));

   if (gnObjectColorsToTargetColorMapping != NULL) {
      free(gnObjectColorsToTargetColorMapping);
      gnObjectColorsToTargetColorMapping = NULL;
   }
   if (gnTgifColorsToObjectColorMapping != NULL) {
      free(gnTgifColorsToObjectColorMapping);
      gnTgifColorsToObjectColorMapping = NULL;
   }
}

/* ----------------------- DefaultErrorDiffuse ----------------------- */

void DefaultErrorDiffuse()
{
   int i, red, rc, ncolors, image_h;
   int red_bits, green_bits, blue_bits, red_levels, green_levels, blue_levels;
   struct XPmRec *xpm_ptr=NULL;

   if (!CheckSelectionForImageProc(CMDID_DEFAULTERRORDIFFUSE)) {
      return;
   }
   xpm_ptr = topSel->obj->detail.xpm;
   image_h = xpm_ptr->image_h;

   red_bits = gDefErrorDiffuseLevel.red;
   green_bits = gDefErrorDiffuseLevel.green;
   blue_bits = gDefErrorDiffuseLevel.blue;
   sprintf(gszMsgBox, TgLoadString(STID_RGB_LEVELS_ARE),
         red_bits, green_bits, blue_bits);
   Msg(gszMsgBox);
   red_levels = (1 << red_bits);
   green_levels = (1 << green_bits);
   blue_levels = (1 << blue_bits);
   ncolors = (1 << (red_bits+green_bits+blue_bits));
   if (ncolors > 0x100) ncolors = 0x100;

   memset(&gXPmTarget, 0, sizeof(struct XPmRec));
   gXPmTarget.ncolors = ncolors;
   gXPmTarget.userdata = (void*)CMDID_DEFAULTERRORDIFFUSE;

   gnFloyd = TRUE;

   AllocTargetColors(ncolors);
   i = 0;
   for (red=0; red < red_levels; red++) {
      int red_intensity=GetIntensity(red, red_levels-1), green;

      for (green=0; green < green_levels; green++) {
         int green_intensity=GetIntensity(green, green_levels-1), blue;

         for (blue=0; blue < blue_levels; blue++) {
            int blue_intensity=GetIntensity(blue, blue_levels-1);

            gTargetColors[i].red = red_intensity;
            gTargetColors[i].green = green_intensity;
            gTargetColors[i].blue = blue_intensity;
            i++;
         }
      }
   }
   if (DoPpm6(xpm_ptr)) {
      if (!InitTrueColorInfo(xpm_ptr->image, &gTrueColorInfo,
            xpm_ptr->image_w)) {
         sprintf(gszMsgBox, TgLoadString(STID_CANNOT_GET_IMAGE_OF_SIZE),
               xpm_ptr->image_w, xpm_ptr->image_h);
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
         return;
      }
   }
   rc = PrepareForErrorDiffuse(xpm_ptr);

   CleanUpIndexOfPixel();
   if (gpTargetImage != NULL) XDestroyImage(gpTargetImage);
   if (gpTargetBitmapImage != NULL) XDestroyImage(gpTargetBitmapImage);
   gpTargetImage = gpTargetBitmapImage = NULL;

   if (rc) {
      gnUserSpecifiedLevels = ncolors;
      gpConvolveFunc = (NLFN*)ConvolveToErrorDiffuse;
      gpConvolveCmdID = CMDID_DEFAULTERRORDIFFUSE;
      gnConvolving = TRUE;
      DoImageProc(NULL);
      gnConvolving = FALSE;
      gpConvolveFunc = NULL;
      gpConvolveCmdID = (-1);
      gnUserSpecifiedLevels = (-1);
   }
   FreeTargetColors();
   if (gnImageTargetColor != NULL) {
      for (i=0; i < image_h; i++) free(gnImageTargetColor[i]);
      free(gnImageTargetColor);
      gnImageTargetColor = NULL;
   }
   gnFloyd = FALSE;
   memset(&gXPmTarget, 0, sizeof(struct XPmRec));
}

/* ----------------------- Spread ----------------------- */

static int gnAmountToSpread=0;
static int **gnSpreadImageIndex=NULL;

static
int ConvolveToSpread(x, y)
   int x, y;
{
   if (gConvExtraInfo.fp != NULL) {
      XColor **xcolors=gConvExtraInfo.xcolors;
      FILE *fp=gConvExtraInfo.fp;
      int w=gConvExtraInfo.image_w, h=gConvExtraInfo.image_h;
      unsigned char buf[3];

      if (x == 0 && y == 0) {
         int r=0;

         srand(0);
         for (r=0; r < h; r++) {
            int c=0;

            for (c=0; c < w; c++) {
               int dx=(rand() % gnAmountToSpread) - (gnAmountToSpread>>1);
               int dy=(rand() % gnAmountToSpread) - (gnAmountToSpread>>1);
               int tmp_y=r+dy, tmp_x=c+dx;

               if (tmp_x >= 0 && tmp_x < w && tmp_y >= 0 && tmp_y < h) {
                  XColor tmp_xcolor;

                  memcpy(&tmp_xcolor, &xcolors[r][c], sizeof(XColor));
                  memcpy(&xcolors[r][c], &xcolors[tmp_y][tmp_x],
                        sizeof(XColor));
                  memcpy(&xcolors[tmp_y][tmp_x], &tmp_xcolor, sizeof(XColor));
               }
            }
         }
      }
      buf[0] = (unsigned char)xcolors[y][x].red;
      buf[1] = (unsigned char)xcolors[y][x].green;
      buf[2] = (unsigned char)xcolors[y][x].blue;
      if ((int)fwrite(buf, sizeof(char), 3, fp) <= 0) writeFileFailed = TRUE;

      return TRUE;
   } else {
      return GetOrAllocHistogramIndex(&tgifColors[gnSpreadImageIndex[y][x]]);
   }
}

static
void CleanUpSpreadData()
{
   int i, image_h=topSel->obj->detail.xpm->image_h;

   if (gnSpreadImageIndex != NULL) {
      for (i=0; i < image_h; i++) free(gnSpreadImageIndex[i]);
      free(gnSpreadImageIndex);
      gnSpreadImageIndex = NULL;
   }
}

static
int ComputeSpreadData()
{
   struct XPmRec *xpm_ptr=topSel->obj->detail.xpm;
   int i, image_w=xpm_ptr->image_w, image_h=xpm_ptr->image_h;
   ProgressInfo pi;
   Pixmap pixmap=xpm_ptr->pixmap;
   XImage *image=XGetImage(mainDisplay, pixmap, 0, 0, image_w, image_h,
         AllPlanes, ZPixmap);

   if (image == NULL) {
      FailAllocMessage();
      return FALSE;
   }
   if (!CreatePixelToIndexMapping()) {
      XDestroyImage(image);
      return FALSE;
   }
   gnSpreadImageIndex = (int**)malloc(image_h*sizeof(int*));
   if (gnSpreadImageIndex == NULL) {
      FailAllocMessage();
      XDestroyImage(image);
      CleanUpIndexOfPixel();
      return FALSE;
   }
   gnAmountToSpread++;
   BeginProgress(&pi, image_h);
   for (i=0; i < image_h; i++) {
      int j;

      UpdateProgress(&pi, i);
      gnSpreadImageIndex[i] = (int*)malloc(image_w*sizeof(int));
      if (gnSpreadImageIndex[i] == NULL) {
         FailAllocMessage();
         for (j=0; j < i; j++) free(gnSpreadImageIndex[j]);
         free(gnSpreadImageIndex);
         gnSpreadImageIndex = NULL;
         XDestroyImage(image);
         CleanUpIndexOfPixel();
         return FALSE;
      }
      for (j=0; j < image_w; j++) {
         gnSpreadImageIndex[i][j] = GetIndexOfPixel(XGetPixel(image,j,i));
      }
   }
   BeginProgress(&pi, image_h);
   srand(0);
   for (i=0; i < image_h; i++) {
      int j;

      UpdateProgress(&pi, i);
      for (j=0; j < image_w; j++) {
         int dx=(rand() % gnAmountToSpread) - (gnAmountToSpread>>1);
         int dy=(rand() % gnAmountToSpread) - (gnAmountToSpread>>1);
         int y=i+dy, x=j+dx, tmp_index;

         if (x >= 0 && x < image_w && y >= 0 && y < image_h) {
            tmp_index = gnSpreadImageIndex[i][j];
            gnSpreadImageIndex[i][j] = gnSpreadImageIndex[y][x];
            gnSpreadImageIndex[y][x] = tmp_index;
         }
      }
   }
   XDestroyImage(image);
   CleanUpIndexOfPixel();

   return TRUE;
}

void Spread()
{
   char *c_ptr=NULL, szSpec[MAXSTRING+1], szSpecCopy[MAXSTRING+1];

   if (!CheckSelectionForImageProc(CMDID_SPREAD)) {
      return;
   }
   *szSpec = '\0';
   Dialog(TgLoadString(STID_ENTER_INT_AMT_TO_SPREAD),
         TgLoadCachedString(CSTID_DLG_ACCEPT_CANCEL), szSpec);
   UtilTrimBlanks(szSpec);
   if (*szSpec == '\0') return;

   strcpy(szSpecCopy, szSpec);
   if ((c_ptr=strtok(szSpec, " ,\t\n\r")) == NULL) return;
   gnAmountToSpread = atoi(c_ptr);
   if (gnAmountToSpread <= 0) {
      sprintf(gszMsgBox, TgLoadString(STID_INVALID_SPEC), szSpecCopy);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return;
   }
   if (!DoPpm6(topSel->obj->detail.xpm)) {
      if (!ComputeSpreadData()) {
         return;
      }
   }
   gpConvolveFunc = (NLFN*)ConvolveToSpread;
   gpConvolveCmdID = CMDID_SPREAD;
   gnConvolving = TRUE;
   DoImageProc(NULL);
   gnConvolving = FALSE;
   gpConvolveFunc = NULL;
   gpConvolveCmdID = (-1);

   CleanUpSpreadData();
}

/* ----------------------- Sharpen ----------------------- */

static
int ConvolveToSharpen(x, y)
   int x, y;
   /*
    * [  0 -1  0 ]
    * [ -1  5 -1 ]
    * [  0 -1  0 ]
    */
{
   if (gConvExtraInfo.fp != NULL) {
      XColor **xcolors=gConvExtraInfo.xcolors;
      FILE *fp=gConvExtraInfo.fp;
      int w=gConvExtraInfo.image_w, h=gConvExtraInfo.image_h;
      unsigned char buf[3];

      if (x == 0 || x == w-1 || y == 0 || y == h-1) {
         buf[0] = (unsigned char)xcolors[y][x].red;
         buf[1] = (unsigned char)xcolors[y][x].green;
         buf[2] = (unsigned char)xcolors[y][x].blue;
      } else {
         int r=0, g=0, b=0;

         r = (int)(
               (((int)xcolors[y][x].red)<<3) -
               ((int)xcolors[y][x-1].red) -
               ((int)xcolors[y][x+1].red) -
               ((int)xcolors[y-1][x].red) -
               ((int)xcolors[y+1][x].red));
         g = (int)(
               (((int)xcolors[y][x].green)<<3) -
               ((int)xcolors[y][x-1].green) -
               ((int)xcolors[y][x+1].green) -
               ((int)xcolors[y-1][x].green) -
               ((int)xcolors[y+1][x].green));
         b = (int)(
               (((int)xcolors[y][x].blue)<<3) -
               ((int)xcolors[y][x-1].blue) -
               ((int)xcolors[y][x+1].blue) -
               ((int)xcolors[y-1][x].blue) -
               ((int)xcolors[y+1][x].blue));
         r >>= 2;
         g >>= 2;
         b >>= 2;
         if (r < 0) r = 0;
         if (g < 0) g = 0;
         if (b < 0) b = 0;
         if (r > 255) r = 255;
         if (g > 255) g = 255;
         if (b > 255) b = 255;

         buf[0] = (unsigned char)(unsigned int)r;
         buf[1] = (unsigned char)(unsigned int)g;
         buf[2] = (unsigned char)(unsigned int)b;
      }
      if ((int)fwrite(buf, sizeof(char), 3, fp) <= 0) writeFileFailed = TRUE;

      return TRUE;
   } else {
      register long red, green, blue;
      XColor xcolor, *color_ptr;

      if (x == 0 || x == gnImageW-1 || y == 0 || y == gnImageH-1) {
         return GetOrAllocHistogramIndex(&tgifColors[gnOrigImageIndex[y][x]]);
      }
      color_ptr = (&tgifColors[gnOrigImageIndex[y][x]]);
      red = ((long)color_ptr->red) << 3;
      green = ((long)color_ptr->green) << 3;
      blue = ((long)color_ptr->blue) << 3;
      color_ptr = (&tgifColors[gnOrigImageIndex[y][x-1]]);
      red -= ((long)color_ptr->red);
      green -= ((long)color_ptr->green);
      blue -= ((long)color_ptr->blue);
      color_ptr = (&tgifColors[gnOrigImageIndex[y][x+1]]);
      red -= ((long)color_ptr->red);
      green -= ((long)color_ptr->green);
      blue -= ((long)color_ptr->blue);
      color_ptr = (&tgifColors[gnOrigImageIndex[y-1][x]]);
      red -= ((long)color_ptr->red);
      green -= ((long)color_ptr->green);
      blue -= ((long)color_ptr->blue);
      color_ptr = (&tgifColors[gnOrigImageIndex[y+1][x]]);
      red = (red - ((long)color_ptr->red)) >> 2;
      green = (green - ((long)color_ptr->green)) >> 2;
      blue = (blue - ((long)color_ptr->blue)) >> 2;

      memset(&xcolor, 0, sizeof(XColor));
      xcolor.red = (red > 0L ?
            (red > 0x0000ffff ? 0x0ffff : (unsigned int)(red&0xffff)) : 0);
      xcolor.green = (green > 0L ?
            (green > 0x0000ffff ? 0x0ffff : (unsigned int)(green&0xffff)) : 0);
      xcolor.blue = (blue > 0L ?
            (blue > 0x0000ffff ? 0x0ffff : (unsigned int)(blue&0xffff)) : 0);
      return GetOrAllocHistogramIndex(&xcolor);
   }
}

void Sharpen()
{
   if (!CheckSelectionForImageProc(CMDID_SHARPEN)) return;
   if (topSel->obj->detail.xpm->image_w < 2 ||
         topSel->obj->detail.xpm->image_h < 2) {
      MsgBox(TgLoadString(STID_SEL_TOO_THIN_FLAT_FOR_SHARPEN), TOOL_NAME,
            INFO_MB);
      return;
   }
   gpConvolveFunc = (NLFN*)ConvolveToSharpen;
   gpConvolveCmdID = CMDID_SHARPEN;
   gnConvolving = TRUE;
   DoImageProc(NULL);
   gnConvolving = FALSE;
   gpConvolveFunc = NULL;
   gpConvolveCmdID = (-1);
}

/* ----------------------- Blur3, Blur5, Blur7 ----------------------- */

static int gnBlurSize=1;

static
int ConvolveToBlur(x, y)
   int x, y;
   /*
    *                 [ 1/25 1/25 1/25 ]
    * [ 1/9 1/9 1/9 ] [ 1/25 1/25 1/25 ]
    * [ 1/9 1/9 1/9 ] [ 1/25 1/25 1/25 ] ...
    * [ 1/9 1/9 1/9 ] [ 1/25 1/25 1/25 ]
    *                 [ 1/25 1/25 1/25 ]
    */
{
   if (gConvExtraInfo.fp != NULL) {
      XColor **xcolors=gConvExtraInfo.xcolors;
      FILE *fp=gConvExtraInfo.fp;
      int w=gConvExtraInfo.image_w, h=gConvExtraInfo.image_h;
      unsigned char buf[3];

      unsigned int r=0, g=0, b=0;
      double dr=(double)0, dg=(double)0, db=(double)0;
      int x_index=0, num_cells=0;

      for (x_index=x-gnBlurSize; x_index <= x+gnBlurSize; x_index++) {
         int y_index=0;

         if (x_index >= 0 && x_index < w) {
            for (y_index=y-gnBlurSize; y_index <= y+gnBlurSize; y_index++) {
               if (y_index >= 0 && y_index < h) {
                  r += ((int)xcolors[y_index][x_index].red);
                  g += ((int)xcolors[y_index][x_index].green);
                  b += ((int)xcolors[y_index][x_index].blue);
                  num_cells++;
               }
            }
         }
      }
      dr = ((double)r) / ((double)num_cells);
      dg = ((double)g) / ((double)num_cells);
      db = ((double)b) / ((double)num_cells);
      r = (unsigned int)round(dr);
      g = (unsigned int)round(dg);
      b = (unsigned int)round(db);
      if (r > 255) r = 255;
      if (g > 255) g = 255;
      if (b > 255) b = 255;

      buf[0] = (unsigned char)r;
      buf[1] = (unsigned char)g;
      buf[2] = (unsigned char)b;

      if ((int)fwrite(buf, sizeof(char), 3, fp) <= 0) writeFileFailed = TRUE;

      return TRUE;
   } else {
      register long red=0L, green=0L, blue=0L;
      XColor xcolor;
      int x_index, array_size=((gnBlurSize<<1)+1);

      if (x < gnBlurSize || x >= gnImageW-gnBlurSize ||
            y < gnBlurSize || y >= gnImageH-gnBlurSize) {
         return GetOrAllocHistogramIndex(&tgifColors[gnOrigImageIndex[y][x]]);
      }
      for (x_index=x-gnBlurSize; x_index <= x+gnBlurSize; x_index++) {
         int y_index;

         for (y_index=y-gnBlurSize; y_index <= y+gnBlurSize; y_index++) {
            register XColor *color_ptr;

            color_ptr = (&tgifColors[gnOrigImageIndex[y_index][x_index]]);
            red += ((long)color_ptr->red);
            green += ((long)color_ptr->green);
            blue += ((long)color_ptr->blue);
         }
      }
      array_size *= array_size;
      red /= array_size;
      green /= array_size;
      blue /= array_size;

      memset(&xcolor, 0, sizeof(XColor));
      xcolor.red = (red > 0L ?
            (red > 0x0000ffff ? 0x0ffff : (unsigned int)(red&0xffff)) : 0);
      xcolor.green = (green > 0L ?
            (green > 0x0000ffff ? 0x0ffff : (unsigned int)(green&0xffff)) : 0);
      xcolor.blue = (blue > 0L ?
            (blue > 0x0000ffff ? 0x0ffff : (unsigned int)(blue&0xffff)) : 0);
      return GetOrAllocHistogramIndex(&xcolor);
   }
}

static
void Blur(nSize)
   int nSize;
{
   int cmdid=CMDID_BLUR3;

   switch (nSize) {
   case 3: cmdid=CMDID_BLUR3; break;
   case 5: cmdid=CMDID_BLUR5; break;
   case 7: cmdid=CMDID_BLUR7; break;
   }
   gnBlurSize = ((nSize-1)>>1);
   if (!CheckSelectionForImageProc(cmdid)) return;
   if (topSel->obj->detail.xpm->image_w <= gnBlurSize ||
         topSel->obj->detail.xpm->image_h <= gnBlurSize) {
      sprintf(gszMsgBox, TgLoadString(STID_SEL_TOO_THIN_FLAT_FOR_BLUR),
            nSize, nSize);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return;
   }
   gpConvolveFunc = (NLFN*)ConvolveToBlur;
   gpConvolveCmdID = CMDID_BLUR3;
   gnConvolving = TRUE;
   DoImageProc(NULL);
   gnConvolving = FALSE;
   gpConvolveFunc = NULL;
   gpConvolveCmdID = (-1);
}

void Blur3() { Blur(3); }
void Blur5() { Blur(5); }
void Blur7() { Blur(7); }

/* ----------------------- AlphaCombine ----------------------- */

static
int PointInRect(x, y, p_bbox)
   int  x, y;
   struct BBRec *p_bbox;
{
   return (x>=p_bbox->ltx && y>=p_bbox->lty && x<p_bbox->rbx && y<p_bbox->rby);
}

#define INSIDE_NONE  0x0
#define INSIDE_FG    0x1
#define INSIDE_BG    0x2
#define INSIDE_ALPHA 0x4

static
int ConvolveToAlphaCombine(x, y)
   int x, y;
{
   XColor fg_xcolor, bg_xcolor, alpha_xcolor, xcolor;
   int inside_flag=INSIDE_NONE, pixel;
   double alpha=(double)0, beta=(double)0;
   long red=0L, green=0L, blue=0L;

   if (PointInRect(x, y, &gAlphaCombineBBox)) inside_flag |= INSIDE_ALPHA;
   if (PointInRect(x, y, &gFgCombineBBox)) inside_flag |= INSIDE_FG;
   if (PointInRect(x, y, &gBgCombineBBox)) inside_flag |= INSIDE_BG;

   if (gConvExtraInfo.fp != NULL) {
      FILE *fp=gConvExtraInfo.fp;
      XColor **xcolors=NULL;
      int image_x=0, image_y=0;
      unsigned int r=0, g=0, b=0;
      unsigned char buf[3];

      switch (inside_flag) {
      case 0x0:
      case 0x4: /* INSIDE_ALPHA */
         buf[0] = (unsigned char)gConvExtraInfo.my_bg_xcolor.red;
         buf[1] = (unsigned char)gConvExtraInfo.my_bg_xcolor.green;
         buf[2] = (unsigned char)gConvExtraInfo.my_bg_xcolor.blue;
         break;
      case 0x1: /* INSIDE_FG */
      case 0x3: /* INSIDE_BG | INSIDE_FG */
      case 0x5: /* INSIDE_ALPHA | INSIDE_FG */
         xcolors = gConvExtraInfo.xcolors;
         image_x = x-gFgCombineBBox.ltx;
         image_y = y-gFgCombineBBox.lty;
         buf[0] = (unsigned char)xcolors[image_y][image_x].red;
         buf[1] = (unsigned char)xcolors[image_y][image_x].green;
         buf[2] = (unsigned char)xcolors[image_y][image_x].blue;
         break;
      case 0x2: /* INSIDE_BG */
      case 0x6: /* INSIDE_ALPHA | INSIDE_BG */
         xcolors = gConvExtraInfo.bg_xcolors;
         image_x = x-gBgCombineBBox.ltx;
         image_y = y-gBgCombineBBox.lty;
         buf[0] = (unsigned char)xcolors[image_y][image_x].red;
         buf[1] = (unsigned char)xcolors[image_y][image_x].green;
         buf[2] = (unsigned char)xcolors[image_y][image_x].blue;
         break;
      case 0x7: /* INSIDE_ALPHA | INSIDE_BG | INSIDE_FG */
         xcolors = gConvExtraInfo.xcolors;
         image_x = x-gFgCombineBBox.ltx;
         image_y = y-gFgCombineBBox.lty;
         memcpy(&fg_xcolor, &xcolors[image_y][image_x], sizeof(XColor));
         xcolors = gConvExtraInfo.bg_xcolors;
         image_x = x-gBgCombineBBox.ltx;
         image_y = y-gBgCombineBBox.lty;
         memcpy(&bg_xcolor, &xcolors[image_y][image_x], sizeof(XColor));
         xcolors = gConvExtraInfo.alpha_xcolors;
         image_x = x-gAlphaCombineBBox.ltx;
         image_y = y-gAlphaCombineBBox.lty;
         memcpy(&alpha_xcolor, &xcolors[image_y][image_x], sizeof(XColor));
         alpha = (0.299*((double)alpha_xcolor.red) +
               0.587*((double)alpha_xcolor.green) +
               0.114*((double)alpha_xcolor.blue)) / 255.0;
         beta = ((double)1) - alpha;
         r = ((double)(fg_xcolor.red)) * alpha +
               ((double)(bg_xcolor.red)) * beta;
         g = ((double)(fg_xcolor.green)) * alpha +
               ((double)(bg_xcolor.green)) * beta;
         b = ((double)(fg_xcolor.blue)) * alpha +
               ((double)(bg_xcolor.blue)) * beta;
         if (r < 0) r = 0;
         if (g < 0) g = 0;
         if (b < 0) b = 0;
         if (r > 255) r = 255;
         if (g > 255) g = 255;
         if (b > 255) b = 255;
         buf[0] = (unsigned char)r;
         buf[1] = (unsigned char)g;
         buf[2] = (unsigned char)b;
         break;
      }
      if ((int)fwrite(buf, sizeof(char), 3, fp) <= 0) writeFileFailed = TRUE;

      return TRUE;
   } else {
      switch (inside_flag) {
      case 0x0:
      case 0x4: /* INSIDE_ALPHA */
         if (myFileBgColorStr == NULL) {
            return GetOrAllocHistogramIndex(&myBgColor);
         } else {
            return GetOrAllocHistogramIndex(&myFileBgColor);
         }
      case 0x1:
      case 0x3: /* INSIDE_BG | INSIDE_FG */
      case 0x5: /* INSIDE_ALPHA | INSIDE_FG */
         /* INSIDE_FG */
         pixel = XGetPixel(gpFgImage, x-gFgCombineBBox.ltx,
               y-gFgCombineBBox.lty);
         return GetOrAllocHistogramIndex(&tgifColors[GetIndexOfPixel(pixel)]);
      case 0x2: /* INSIDE_BG */
      case 0x6: /* INSIDE_ALPHA | INSIDE_BG */
         pixel = XGetPixel(gpBgImage, x-gBgCombineBBox.ltx,
               y-gBgCombineBBox.lty);
         return GetOrAllocHistogramIndex(&tgifColors[GetIndexOfPixel(pixel)]);
      case 0x7:
         /* INSIDE_ALPHA | INSIDE_BG | INSIDE_FG */
         pixel = XGetPixel(gpFgImage, x-gFgCombineBBox.ltx,
               y-gFgCombineBBox.lty);
         memcpy(&fg_xcolor, &tgifColors[GetIndexOfPixel(pixel)],
               sizeof(XColor));
         pixel = XGetPixel(gpBgImage, x-gBgCombineBBox.ltx,
               y-gBgCombineBBox.lty);
         memcpy(&bg_xcolor, &tgifColors[GetIndexOfPixel(pixel)],
               sizeof(XColor));
         pixel = XGetPixel(gpAlphaImage, x-gAlphaCombineBBox.ltx,
               y-gAlphaCombineBBox.lty);
         memcpy(&alpha_xcolor, &tgifColors[GetIndexOfPixel(pixel)],
               sizeof(XColor));
         alpha = ((double)(0.299*((double)alpha_xcolor.red) +
               0.587*((double)alpha_xcolor.green) +
               0.114*((double)alpha_xcolor.blue))) / 65536.0;
         beta = 1.0 - alpha;
         red = (long)(((double)fg_xcolor.red)*alpha +
               ((double)bg_xcolor.red)*beta);
         green = (long)(((double)fg_xcolor.green)*alpha +
               ((double)bg_xcolor.green)*beta);
         blue = (long)(((double)fg_xcolor.blue)*alpha +
               ((double)bg_xcolor.blue)*beta);
         break;
      }
      memset(&xcolor, 0, sizeof(XColor));
      xcolor.red = (red > 0L ?
            (red > 0x0000ffff ? 0x0ffff : (unsigned int)(red&0xffff)) : 0);
      xcolor.green = (green > 0L ?
            (green > 0x0000ffff ? 0x0ffff : (unsigned int)(green&0xffff)) : 0);
      xcolor.blue = (blue > 0L ?
            (blue > 0x0000ffff ? 0x0ffff : (unsigned int)(blue&0xffff)) : 0);
      return GetOrAllocHistogramIndex(&xcolor);
   }
}

static
void OffsetRect(p_bbox, dx, dy)
   struct BBRec *p_bbox;
   int dx, dy;
{
   p_bbox->ltx += dx; p_bbox->lty += dy;
   p_bbox->rbx += dx; p_bbox->rby += dy;
}

static
void CleanUpAlphaCombine()
{
   if (gpFgImage != NULL) XDestroyImage(gpFgImage);
   if (gpBgImage != NULL) XDestroyImage(gpBgImage);
   if (gpAlphaImage != NULL) XDestroyImage(gpAlphaImage);
   if (gpFgBitmapImage != NULL) XDestroyImage(gpFgBitmapImage);
   if (gpBgBitmapImage != NULL) XDestroyImage(gpBgBitmapImage);
   if (gpAlphaBitmapImage != NULL) XDestroyImage(gpAlphaBitmapImage);
   gpFgImage = gpBgImage = gpAlphaImage =
         gpFgBitmapImage = gpBgBitmapImage = gpAlphaBitmapImage = NULL;
   gpFgObj = gpBgObj = gpAlphaObj = NULL;
}

static
int PrepareForAlphaCombine()
{
   memcpy(&gFgCombineBBox, &gpFgObj->obbox, sizeof(struct BBRec));
   memcpy(&gBgCombineBBox, &gpBgObj->obbox, sizeof(struct BBRec));
   if (gpAlphaObj != NULL) {
      memcpy(&gAlphaCombineBBox, &gpAlphaObj->obbox, sizeof(struct BBRec));
   }
   UnionRect(&gFgCombineBBox, &gBgCombineBBox, &gTotalCombineBBox);
   if (gpAlphaObj != NULL) {
      UnionRect(&gAlphaCombineBBox, &gTotalCombineBBox, &gTotalCombineBBox);
   }
   gnCombineW = gTotalCombineBBox.rbx - gTotalCombineBBox.ltx;
   gnCombineH = gTotalCombineBBox.rby - gTotalCombineBBox.lty;
   OffsetRect(&gFgCombineBBox, -gTotalCombineBBox.ltx, -gTotalCombineBBox.lty);
   OffsetRect(&gBgCombineBBox, -gTotalCombineBBox.ltx, -gTotalCombineBBox.lty);
   if (gpAlphaObj != NULL) {
      OffsetRect(&gAlphaCombineBBox,
            -gTotalCombineBBox.ltx, -gTotalCombineBBox.lty);
   }
   gpAlphaImage = gpAlphaBitmapImage = NULL;
   if (!GetXPmImages(gpFgObj->detail.xpm, &gpFgImage, &gpFgBitmapImage) ||
         !GetXPmImages(gpBgObj->detail.xpm, &gpBgImage, &gpBgBitmapImage) ||
         (gpAlphaObj != NULL && !GetXPmImages(gpAlphaObj->detail.xpm,
         &gpAlphaImage, &gpAlphaBitmapImage))) {
      return FALSE;
   }
   return TRUE;
}

void AlphaCombine()
{
   char szBuf[MAXSTRING+1];
   int num_objs=0;

   strcpy(szBuf, GetImageProcName(CMDID_ALPHACOMBINE));
   gpFgObj = gpBgObj = gpAlphaObj = NULL;
   if (curChoice == NOTHING && numObjSelected == 3) {
      struct SelRec *sel_ptr;
      struct ObjRec *obj_ptr;
      int ok=TRUE;

      for (obj_ptr=topObj; ok && obj_ptr != NULL; obj_ptr=obj_ptr->next) {
         for (sel_ptr=topSel; sel_ptr != NULL; sel_ptr=sel_ptr->next) {
            if (obj_ptr == sel_ptr->obj) {
               if (obj_ptr->type == OBJ_XPM) {
                  struct XPmRec *xpm_ptr=obj_ptr->detail.xpm;
                  int image_w=obj_ptr->obbox.rbx-obj_ptr->obbox.ltx;
                  int image_h=obj_ptr->obbox.rby-obj_ptr->obbox.lty;

                  if (obj_ptr->ctm != NULL || xpm_ptr->image_w != image_w ||
                        xpm_ptr->image_h != image_h) {
                     char szBuf1[MAXSTRING+1];

                     strcpy(szBuf1,
                           GetImageProcName(CMDID_REGENERATEIMAGE));
                     sprintf(gszMsgBox,
                           TgLoadString(STID_IMAGE_PROC_CANT_USE_XFORMED),
                           szBuf, szBuf1);
                     MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
                     return;
                  }
                  switch (num_objs++) {
                  case 0: gpFgObj = obj_ptr; break;
                  case 1: gpAlphaObj = obj_ptr; break;
                  case 2: gpBgObj = obj_ptr; break;
                  }
               } else {
                  ok = FALSE;
                  break;
               }
            }
         }
      }
      if (!ok) num_objs = 0;
   }
   if (num_objs != 3) {
      sprintf(gszMsgBox, TgLoadString(STID_SEL_3_XPM_FOR_IMAGEPROC_CMD), szBuf);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return;
   }
   if (PrepareForAlphaCombine()) {
      gnCombining = TRUE;
      gpConvolveFunc = (NLFN*)ConvolveToAlphaCombine;
      gpConvolveCmdID = CMDID_ALPHACOMBINE;
      gnConvolving = TRUE;
      DoImageProc(NULL);
      gnConvolving = FALSE;
      gpConvolveFunc = NULL;
      gpConvolveCmdID = (-1);
      gnCombining = FALSE;
   }
   CleanUpAlphaCombine();
}

/* ----------------------- Subtract ----------------------- */

static
int ConvolveToSubtract(x, y)
   int x, y;
{
   XColor fg_xcolor, bg_xcolor, xcolor;
   int inside_flag=INSIDE_NONE, pixel;
   long red=0L, green=0L, blue=0L;

   if (PointInRect(x, y, &gFgCombineBBox)) inside_flag |= INSIDE_FG;
   if (PointInRect(x, y, &gBgCombineBBox)) inside_flag |= INSIDE_BG;

   if (gConvExtraInfo.fp != NULL) {
      FILE *fp=gConvExtraInfo.fp;
      XColor **xcolors=NULL;
      int r=0, g=0, b=0, image_x=0, image_y=0;
      unsigned char buf[3];

      switch (inside_flag) {
      case 0x0:
         buf[0] = (unsigned char)gConvExtraInfo.my_bg_xcolor.red;
         buf[1] = (unsigned char)gConvExtraInfo.my_bg_xcolor.green;
         buf[2] = (unsigned char)gConvExtraInfo.my_bg_xcolor.blue;
         break;
      case 0x1:
         /* INSIDE_FG */
         xcolors = gConvExtraInfo.xcolors;
         image_x = x-gFgCombineBBox.ltx;
         image_y = y-gFgCombineBBox.lty;
         buf[0] = (unsigned char)xcolors[image_y][image_x].red;
         buf[1] = (unsigned char)xcolors[image_y][image_x].green;
         buf[2] = (unsigned char)xcolors[image_y][image_x].blue;
         break;
      case 0x2:
         /* INSIDE_BG */
         xcolors = gConvExtraInfo.bg_xcolors;
         image_x = x-gBgCombineBBox.ltx;
         image_y = y-gBgCombineBBox.lty;
         buf[0] = (unsigned char)xcolors[image_y][image_x].red;
         buf[1] = (unsigned char)xcolors[image_y][image_x].green;
         buf[2] = (unsigned char)xcolors[image_y][image_x].blue;
         break;
      case 0x3:
         /* INSIDE_BG | INSIDE_FG */
         xcolors = gConvExtraInfo.xcolors;
         image_x = x-gFgCombineBBox.ltx;
         image_y = y-gFgCombineBBox.lty;
         memcpy(&fg_xcolor, &xcolors[image_y][image_x], sizeof(XColor));
         xcolors = gConvExtraInfo.bg_xcolors;
         image_x = x-gBgCombineBBox.ltx;
         image_y = y-gBgCombineBBox.lty;
         memcpy(&bg_xcolor, &xcolors[image_y][image_x], sizeof(XColor));
         r = ((int)fg_xcolor.red) - ((int)bg_xcolor.red);
         g = ((int)fg_xcolor.green) - ((int)bg_xcolor.green);
         b = ((int)fg_xcolor.blue) - ((int)bg_xcolor.blue);
         if (r < 0) r = 0;
         if (g < 0) g = 0;
         if (b < 0) b = 0;
         if (r > 255) r = 255;
         if (g > 255) g = 255;
         if (b > 255) b = 255;
         buf[0] = (unsigned char)r;
         buf[1] = (unsigned char)g;
         buf[2] = (unsigned char)b;
         break;
      }
      if ((int)fwrite(buf, sizeof(char), 3, fp) <= 0) writeFileFailed = TRUE;

      return TRUE;
   } else {
      switch (inside_flag) {
      case 0x0:
         if (myFileBgColorStr == NULL) {
            return GetOrAllocHistogramIndex(&myBgColor);
         } else {
            return GetOrAllocHistogramIndex(&myFileBgColor);
         }
      case 0x1:
         /* INSIDE_FG */
         pixel = XGetPixel(gpFgImage, x-gFgCombineBBox.ltx,
               y-gFgCombineBBox.lty);
         memcpy(&fg_xcolor, &tgifColors[GetIndexOfPixel(pixel)],
               sizeof(XColor));
         red = (long)fg_xcolor.red;
         green = (long)fg_xcolor.green;
         blue = (long)fg_xcolor.blue;
         break;
      case 0x2:
         /* INSIDE_BG */
         pixel = XGetPixel(gpBgImage, x-gBgCombineBBox.ltx,
               y-gBgCombineBBox.lty);
         memcpy(&bg_xcolor, &tgifColors[GetIndexOfPixel(pixel)],
               sizeof(XColor));
         red = (long)bg_xcolor.red;
         green = (long)bg_xcolor.green;
         blue = (long)bg_xcolor.blue;
         break;
      case 0x3:
         /* INSIDE_BG | INSIDE_FG */
         pixel = XGetPixel(gpFgImage, x-gFgCombineBBox.ltx,
               y-gFgCombineBBox.lty);
         memcpy(&fg_xcolor, &tgifColors[GetIndexOfPixel(pixel)],
               sizeof(XColor));
         pixel = XGetPixel(gpBgImage, x-gBgCombineBBox.ltx,
               y-gBgCombineBBox.lty);
         memcpy(&bg_xcolor, &tgifColors[GetIndexOfPixel(pixel)],
               sizeof(XColor));
         red = ((long)fg_xcolor.red) - ((long)bg_xcolor.red);
         green = ((long)fg_xcolor.green) - ((long)bg_xcolor.green);
         blue = ((long)fg_xcolor.blue) - ((long)bg_xcolor.blue);
         break;
      }
      memset(&xcolor, 0, sizeof(XColor));
      xcolor.red = (red > 0L ?
            (red > 0x0000ffff ? 0x0ffff : (unsigned int)(red&0xffff)) : 0);
      xcolor.green = (green > 0L ?
            (green > 0x0000ffff ? 0x0ffff : (unsigned int)(green&0xffff)) : 0);
      xcolor.blue = (blue > 0L ?
            (blue > 0x0000ffff ? 0x0ffff : (unsigned int)(blue&0xffff)) : 0);
      return GetOrAllocHistogramIndex(&xcolor);
   }
}

void Subtract()
{
   char szBuf[MAXSTRING+1];
   int num_objs=0;

   strcpy(szBuf, GetImageProcName(CMDID_SUBTRACT));
   gpFgObj = gpBgObj = gpAlphaObj = NULL;
   if (curChoice == NOTHING && numObjSelected == 2) {
      struct SelRec *sel_ptr;
      struct ObjRec *obj_ptr;
      int ok=TRUE;

      for (obj_ptr=topObj; ok && obj_ptr != NULL; obj_ptr=obj_ptr->next) {
         for (sel_ptr=topSel; sel_ptr != NULL; sel_ptr=sel_ptr->next) {
            if (obj_ptr == sel_ptr->obj) {
               if (obj_ptr->type == OBJ_XPM) {
                  struct XPmRec *xpm_ptr=obj_ptr->detail.xpm;
                  int image_w=obj_ptr->obbox.rbx-obj_ptr->obbox.ltx;
                  int image_h=obj_ptr->obbox.rby-obj_ptr->obbox.lty;

                  if (obj_ptr->ctm != NULL || xpm_ptr->image_w != image_w ||
                        xpm_ptr->image_h != image_h) {
                     char szBuf1[MAXSTRING+1];

                     strcpy(szBuf1,
                           GetImageProcName(CMDID_REGENERATEIMAGE));
                     sprintf(gszMsgBox,
                           TgLoadString(STID_IMAGE_PROC_CANT_USE_XFORMED),
                           szBuf, szBuf1);
                     MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
                     return;
                  }
                  switch (num_objs++) {
                  case 0: gpFgObj = obj_ptr; break;
                  case 1: gpBgObj = obj_ptr; break;
                  }
               } else {
                  ok = FALSE;
                  break;
               }
            }
         }
      }
      if (!ok) num_objs = 0;
   }
   if (num_objs != 2) {
      sprintf(gszMsgBox, TgLoadString(STID_SEL_2_XPM_FOR_IMAGEPROC_CMD), szBuf);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return;
   }
   if (PrepareForAlphaCombine()) {
      gnCombining = TRUE;
      gpConvolveFunc = (NLFN*)ConvolveToSubtract;
      gpConvolveCmdID = CMDID_SUBTRACT;
      gnConvolving = TRUE;
      DoImageProc(NULL);
      gnConvolving = FALSE;
      gpConvolveFunc = NULL;
      gpConvolveCmdID = (-1);
      gnCombining = FALSE;
   }
   CleanUpAlphaCombine();
}

/* ----------------------- XorColors ----------------------- */

static
int ConvolveToXorColors(x, y)
   int x, y;
{
   XColor fg_xcolor, bg_xcolor, xcolor;
   int inside_flag=INSIDE_NONE, pixel=0;

   memset(&xcolor, 0, sizeof(XColor));

   if (PointInRect(x, y, &gFgCombineBBox)) inside_flag |= INSIDE_FG;
   if (PointInRect(x, y, &gBgCombineBBox)) inside_flag |= INSIDE_BG;

   if (gConvExtraInfo.fp != NULL) {
      FILE *fp=gConvExtraInfo.fp;
      XColor **xcolors=NULL;
      int image_x=0, image_y=0;
      unsigned int r=0, g=0, b=0;
      unsigned char buf[3];

      switch (inside_flag) {
      case 0x0:
         buf[0] = (unsigned char)gConvExtraInfo.my_bg_xcolor.red;
         buf[1] = (unsigned char)gConvExtraInfo.my_bg_xcolor.green;
         buf[2] = (unsigned char)gConvExtraInfo.my_bg_xcolor.blue;
         break;
      case 0x1:
         /* INSIDE_FG */
         xcolors = gConvExtraInfo.xcolors;
         image_x = x-gFgCombineBBox.ltx;
         image_y = y-gFgCombineBBox.lty;
         buf[0] = (unsigned char)xcolors[image_y][image_x].red;
         buf[1] = (unsigned char)xcolors[image_y][image_x].green;
         buf[2] = (unsigned char)xcolors[image_y][image_x].blue;
         break;
      case 0x2:
         /* INSIDE_BG */
         xcolors = gConvExtraInfo.bg_xcolors;
         image_x = x-gBgCombineBBox.ltx;
         image_y = y-gBgCombineBBox.lty;
         buf[0] = (unsigned char)xcolors[image_y][image_x].red;
         buf[1] = (unsigned char)xcolors[image_y][image_x].green;
         buf[2] = (unsigned char)xcolors[image_y][image_x].blue;
         break;
      case 0x3:
         /* INSIDE_BG | INSIDE_FG */
         xcolors = gConvExtraInfo.xcolors;
         image_x = x-gFgCombineBBox.ltx;
         image_y = y-gFgCombineBBox.lty;
         memcpy(&fg_xcolor, &xcolors[image_y][image_x], sizeof(XColor));
         xcolors = gConvExtraInfo.bg_xcolors;
         image_x = x-gBgCombineBBox.ltx;
         image_y = y-gBgCombineBBox.lty;
         memcpy(&bg_xcolor, &xcolors[image_y][image_x], sizeof(XColor));
         r = (unsigned int)((fg_xcolor.red) ^ (bg_xcolor.red));
         g = (unsigned int)((fg_xcolor.green) ^ (bg_xcolor.green));
         b = (unsigned int)((fg_xcolor.blue) ^ (bg_xcolor.blue));
         if (r < 0) r = 0;
         if (g < 0) g = 0;
         if (b < 0) b = 0;
         if (r > 255) r = 255;
         if (g > 255) g = 255;
         if (b > 255) b = 255;
         buf[0] = (unsigned char)r;
         buf[1] = (unsigned char)g;
         buf[2] = (unsigned char)b;
         break;
      }
      if ((int)fwrite(buf, sizeof(char), 3, fp) <= 0) writeFileFailed = TRUE;

      return TRUE;
   } else {
      switch (inside_flag) {
      case 0x0:
         if (myFileBgColorStr == NULL) {
            return GetOrAllocHistogramIndex(&myBgColor);
         } else {
            return GetOrAllocHistogramIndex(&myFileBgColor);
         }
      case 0x1:
         /* INSIDE_FG */
         pixel = XGetPixel(gpFgImage, x-gFgCombineBBox.ltx,
               y-gFgCombineBBox.lty);
         memcpy(&fg_xcolor, &tgifColors[GetIndexOfPixel(pixel)],
               sizeof(XColor));
         xcolor.red = fg_xcolor.red;
         xcolor.green = fg_xcolor.green;
         xcolor.blue = fg_xcolor.blue;
         break;
      case 0x2:
         /* INSIDE_BG */
         pixel = XGetPixel(gpBgImage, x-gBgCombineBBox.ltx,
               y-gBgCombineBBox.lty);
         memcpy(&bg_xcolor, &tgifColors[GetIndexOfPixel(pixel)],
               sizeof(XColor));
         xcolor.red = bg_xcolor.red;
         xcolor.green = bg_xcolor.green;
         xcolor.blue = bg_xcolor.blue;
         break;
      case 0x3:
         /* INSIDE_BG | INSIDE_FG */
         pixel = XGetPixel(gpFgImage, x-gFgCombineBBox.ltx,
               y-gFgCombineBBox.lty);
         memcpy(&fg_xcolor, &tgifColors[GetIndexOfPixel(pixel)],
               sizeof(XColor));
         pixel = XGetPixel(gpBgImage, x-gBgCombineBBox.ltx,
               y-gBgCombineBBox.lty);
         memcpy(&bg_xcolor, &tgifColors[GetIndexOfPixel(pixel)],
               sizeof(XColor));
         xcolor.red = (fg_xcolor.red) ^ (bg_xcolor.red);
         xcolor.green = (fg_xcolor.green) ^ (bg_xcolor.green);
         xcolor.blue = (fg_xcolor.blue) ^ (bg_xcolor.blue);
         break;
      }
      return GetOrAllocHistogramIndex(&xcolor);
   }
}

void XorColors()
{
   char szBuf[MAXSTRING+1];
   int num_objs=0;

   strcpy(szBuf, GetImageProcName(CMDID_XORCOLORS));
   gpFgObj = gpBgObj = gpAlphaObj = NULL;
   if (curChoice == NOTHING && numObjSelected == 2) {
      struct SelRec *sel_ptr;
      struct ObjRec *obj_ptr;
      int ok=TRUE;

      for (obj_ptr=topObj; ok && obj_ptr != NULL; obj_ptr=obj_ptr->next) {
         for (sel_ptr=topSel; sel_ptr != NULL; sel_ptr=sel_ptr->next) {
            if (obj_ptr == sel_ptr->obj) {
               if (obj_ptr->type == OBJ_XPM) {
                  struct XPmRec *xpm_ptr=obj_ptr->detail.xpm;
                  int image_w=obj_ptr->obbox.rbx-obj_ptr->obbox.ltx;
                  int image_h=obj_ptr->obbox.rby-obj_ptr->obbox.lty;

                  if (obj_ptr->ctm != NULL || xpm_ptr->image_w != image_w ||
                        xpm_ptr->image_h != image_h) {
                     char szBuf1[MAXSTRING+1];

                     strcpy(szBuf1,
                           GetImageProcName(CMDID_REGENERATEIMAGE));
                     sprintf(gszMsgBox,
                           TgLoadString(STID_IMAGE_PROC_CANT_USE_XFORMED),
                           szBuf, szBuf1);
                     MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
                     return;
                  }
                  switch (num_objs++) {
                  case 0: gpFgObj = obj_ptr; break;
                  case 1: gpBgObj = obj_ptr; break;
                  }
               } else {
                  ok = FALSE;
                  break;
               }
            }
         }
      }
      if (!ok) num_objs = 0;
   }
   if (num_objs != 2) {
      sprintf(gszMsgBox, TgLoadString(STID_SEL_2_XPM_FOR_IMAGEPROC_CMD), szBuf);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return;
   }
   if (PrepareForAlphaCombine()) {
      gnCombining = TRUE;
      gpConvolveFunc = (NLFN*)ConvolveToXorColors;
      gpConvolveCmdID = CMDID_XORCOLORS;
      gnConvolving = TRUE;
      DoImageProc(NULL);
      gnConvolving = FALSE;
      gpConvolveFunc = NULL;
      gpConvolveCmdID = (-1);
      gnCombining = FALSE;
   }
   CleanUpAlphaCombine();
}

/* ======================= Image Warping ======================= */

/* ----------------------- VectorWarp ----------------------- */

typedef struct RevWarpXformRec {
   /*
    * x = X + x_coef ( X + w - f(x)W ) ( Y + h - g(y)H )
    * y = Y + y_coef ( X + w - f(x)W ) ( Y + h - g(y)H )
    *
    *                  [ 00 | 10 ]
    * where f(x)g(y) = [ ---+--- ]
    *                  [ 01 | 11 ]
    *
    * X and Y are in the warped domain and x and y are in the original domain
    *
    * x_coef = -dw / ( ( w + dw - f(x)W ) ( h + dh - g(y)H ) )
    * y_coef = -dh / ( ( w + dw - f(x)W ) ( h + dh - g(y)H ) )
    *
    * Nevertheless, this structure is not used!
    */
   double x_coef, y_coef;
} RevWarpXform;

static IntPoint gaTrapMapPt[5];
static TrapMapSeg gaTrapMapSeg[4];
static int gaTrapMapLeaf[4];

static TrapMapSpec gLeftTrapMapSpec[] = {
   /*
    * In-order specification of the trapezoidal map for Cx < Tx.
    */
   { TRAP_NODE_PT, TRAP_PT_C },
   { TRAP_NODE_SEG, TRAP_SEG_L },
   { TRAP_NODE_LEAF, TRAP_LEAF_LT },
   { TRAP_NODE_LEAF, TRAP_LEAF_LB },
   { TRAP_NODE_PT, TRAP_PT_T },
   { TRAP_NODE_SEG, TRAP_SEG_T },
   { TRAP_NODE_LEAF, TRAP_LEAF_LT },
   { TRAP_NODE_SEG, TRAP_SEG_R },
   { TRAP_NODE_LEAF, TRAP_LEAF_RT },
   { TRAP_NODE_SEG, TRAP_SEG_B },
   { TRAP_NODE_LEAF, TRAP_LEAF_RB },
   { TRAP_NODE_LEAF, TRAP_LEAF_LB },
   { TRAP_NODE_SEG, TRAP_SEG_R },
   { TRAP_NODE_LEAF, TRAP_LEAF_RT },
   { TRAP_NODE_LEAF, TRAP_LEAF_RB },
   { -1, -1 }
};

static TrapMapSpec gRightTrapMapSpec[] = {
   /*
    * In-order specification of the trapezoidal map for Cx > Tx.
    */
   { TRAP_NODE_PT, TRAP_PT_T },
   { TRAP_NODE_SEG, TRAP_SEG_L },
   { TRAP_NODE_LEAF, TRAP_LEAF_LT },
   { TRAP_NODE_LEAF, TRAP_LEAF_LB },
   { TRAP_NODE_PT, TRAP_PT_C },
   { TRAP_NODE_SEG, TRAP_SEG_T },
   { TRAP_NODE_LEAF, TRAP_LEAF_RT },
   { TRAP_NODE_SEG, TRAP_SEG_L },
   { TRAP_NODE_LEAF, TRAP_LEAF_LT },
   { TRAP_NODE_SEG, TRAP_SEG_B },
   { TRAP_NODE_LEAF, TRAP_LEAF_LB },
   { TRAP_NODE_LEAF, TRAP_LEAF_RB },
   { TRAP_NODE_SEG, TRAP_SEG_R },
   { TRAP_NODE_LEAF, TRAP_LEAF_RT },
   { TRAP_NODE_LEAF, TRAP_LEAF_RB },
   { -1, -1 }
};

static TrapMapSpec gCenterTrapMapSpec[] = {
   /*
    * In-order specification of the trapezoidal map for Cx == Tx.
    */
   { TRAP_NODE_PT, TRAP_PT_T },
   { TRAP_NODE_SEG, TRAP_SEG_L },
   { TRAP_NODE_LEAF, TRAP_LEAF_LT },
   { TRAP_NODE_LEAF, TRAP_LEAF_LB },
   { TRAP_NODE_SEG, TRAP_SEG_R },
   { TRAP_NODE_LEAF, TRAP_LEAF_RT },
   { TRAP_NODE_LEAF, TRAP_LEAF_RB },
   { -1, -1 }
};

static TrapMap *gpTrapMap=NULL;
static TrapMapSpec *gpTrapMapSpec=NULL;
static int **gpSegYIntersects=NULL;
static int **gpExtraYIntersects=NULL;

static
void CleanTrapMapNode(pTrapMapNode)
   TrapMap *pTrapMapNode;
{
   switch (pTrapMapNode->type) {
   case TRAP_NODE_LEAF: break;
   case TRAP_NODE_PT:
      if (pTrapMapNode->detail.pt.left != NULL) {
         CleanTrapMapNode(pTrapMapNode->detail.pt.left);
      }
      if (pTrapMapNode->detail.pt.right != NULL) {
         CleanTrapMapNode(pTrapMapNode->detail.pt.right);
      }
      break;
   case TRAP_NODE_SEG:
      if (pTrapMapNode->detail.seg.above != NULL) {
         CleanTrapMapNode(pTrapMapNode->detail.seg.above);
      }
      if (pTrapMapNode->detail.seg.below != NULL) {
         CleanTrapMapNode(pTrapMapNode->detail.seg.below);
      }
      break;
   }
   free(pTrapMapNode);
}

static
void CleanTrapMap()
{
   if (gpTrapMap != NULL) {
      CleanTrapMapNode(gpTrapMap);
   }
   gpTrapMap = NULL;
   if (gpSegYIntersects != NULL) {
      int i;

      for (i=0; i < 4; i++) {
         if (gpSegYIntersects[i] != NULL) {
            free(gpSegYIntersects[i]);
         }
      }
      free(gpSegYIntersects);
      gpSegYIntersects = NULL;
   }
   if (gpExtraYIntersects != NULL) {
      int i;

      for (i=0; i < 4; i++) {
         if (gpExtraYIntersects[i] != NULL) {
            free(gpExtraYIntersects[i]);
         }
      }
      free(gpExtraYIntersects);
      gpExtraYIntersects = NULL;
   }
}

static
int SetTrapMapLeaf(pTrapMapSpecRoot, pnIndex, pTrapMapNode, nWhich)
   TrapMapSpec *pTrapMapSpecRoot;
   int *pnIndex;
   TrapMap *pTrapMapNode;
   int nWhich;
{
   pTrapMapNode->type = TRAP_NODE_LEAF;
   pTrapMapNode->detail.leaf.which = nWhich;
   pTrapMapNode->detail.leaf.data = (&gaTrapMapLeaf[nWhich]);
   return TRUE;
}

static
int SetTrapMapPt(pTrapMapSpecRoot, pnIndex, pTrapMapNode, nWhich)
   TrapMapSpec *pTrapMapSpecRoot;
   int *pnIndex;
   TrapMap *pTrapMapNode;
   int nWhich;
{
   pTrapMapNode->type = TRAP_NODE_PT;
   pTrapMapNode->detail.pt.which = nWhich;
   pTrapMapNode->detail.pt.data = (&gaTrapMapPt[nWhich]);
   pTrapMapNode->detail.pt.left = (TrapMap*)malloc(sizeof(TrapMap));
   pTrapMapNode->detail.pt.right = (TrapMap*)malloc(sizeof(TrapMap));
   if (pTrapMapNode->detail.pt.left == NULL ||
         pTrapMapNode->detail.pt.right == NULL) {
      FailAllocMessage();
   }
   memset(pTrapMapNode->detail.pt.left, 0, sizeof(TrapMap));
   memset(pTrapMapNode->detail.pt.right, 0, sizeof(TrapMap));
   (*pnIndex)++;
   if (!BuildTrapMapNode(pTrapMapSpecRoot, pnIndex,
         pTrapMapNode->detail.pt.left)) {
      return FALSE;
   }
   (*pnIndex)++;
   if (!BuildTrapMapNode(pTrapMapSpecRoot, pnIndex,
         pTrapMapNode->detail.pt.right)) {
      return FALSE;
   }
   return TRUE;
}

static
int SetTrapMapSeg(pTrapMapSpecRoot, pnIndex, pTrapMapNode, nWhich)
   TrapMapSpec *pTrapMapSpecRoot;
   int *pnIndex;
   TrapMap *pTrapMapNode;
   int nWhich;
{
   pTrapMapNode->type = TRAP_NODE_SEG;
   pTrapMapNode->detail.seg.which = nWhich;
   pTrapMapNode->detail.seg.data = (&gaTrapMapSeg[nWhich]);
   pTrapMapNode->detail.seg.above = (TrapMap*)malloc(sizeof(TrapMap));
   pTrapMapNode->detail.seg.below = (TrapMap*)malloc(sizeof(TrapMap));
   if (pTrapMapNode->detail.seg.above == NULL ||
         pTrapMapNode->detail.seg.below == NULL) {
      FailAllocMessage();
   }
   memset(pTrapMapNode->detail.seg.above, 0, sizeof(TrapMap));
   memset(pTrapMapNode->detail.seg.below, 0, sizeof(TrapMap));
   (*pnIndex)++;
   if (!BuildTrapMapNode(pTrapMapSpecRoot, pnIndex,
         pTrapMapNode->detail.seg.above)) {
      return FALSE;
   }
   (*pnIndex)++;
   if (!BuildTrapMapNode(pTrapMapSpecRoot, pnIndex,
         pTrapMapNode->detail.seg.below)) {
      return FALSE;
   }
   return TRUE;
}

int BuildTrapMapNode(pTrapMapSpecRoot, pnIndex, pTrapMapNode)
   TrapMapSpec *pTrapMapSpecRoot;
   int *pnIndex;
   TrapMap *pTrapMapNode;
{
   TrapMapSpec *pTrapMapSpec=(&pTrapMapSpecRoot[*pnIndex]);

   switch (pTrapMapSpec->type) {
   case TRAP_NODE_LEAF:
      return SetTrapMapLeaf(pTrapMapSpecRoot, pnIndex, pTrapMapNode,
            pTrapMapSpec->which);
   case TRAP_NODE_PT:
      return SetTrapMapPt(pTrapMapSpecRoot, pnIndex, pTrapMapNode,
            pTrapMapSpec->which);
   case TRAP_NODE_SEG:
      return SetTrapMapSeg(pTrapMapSpecRoot, pnIndex, pTrapMapNode,
            pTrapMapSpec->which);
   default: break;
   }
   return TRUE;
}

static
int BuildTrapMap(pTrapMapSpec)
   TrapMapSpec *pTrapMapSpec;
{
   int nIndex=0;

   gpTrapMap = (TrapMap*)malloc(sizeof(TrapMap));
   if (gpTrapMap == NULL) FailAllocMessage();
   memset(gpTrapMap, 0, sizeof(TrapMap));
   if (BuildTrapMapNode(pTrapMapSpec, &nIndex, gpTrapMap)) {
      return TRUE;
   }
   CleanTrapMap();
   return FALSE;
}

/*
 * L, T, R, B, C are points.  Left subtree are to the left of the point.
 * sL, sT, sR, sB are line segments.  Left subtree are above the line segment.
 * lt, rt, lb, rb are the quardrants (leaves of the tree).
 *
 * If Cx == Tx && Cy == Ly: Do not warp.
 *
 * If Cx < Tx:
 *
 *      +-------T-------+                C
 *      |               |               / \
 *      |     sT        |              /   \
 *      |               |             /     \
 *      |    C   sR     |            sL      T
 *      L sL            R           /  \    / \
 *      |               |          lt  lb  /   \
 *      |     sB        |                 /     \
 *      |               |                sT     sR
 *      |               |               /  \   /  \
 *      +-------B-------+              lt  sR rt  rb
 *                                        /  \
 *                                       rt   sB
 *                                           /  \
 *                                          rb  lb 
 * If Cx > Tx:
 *
 *      +-------T-------+                T
 *      |               |               / \
 *      |        sT     |              /   \
 *      |               |             /     \
 *      |     sL   C    |            sL      C
 *      L            sR R           /  \    / \
 *      |               |          lt  lb  /   \
 *      |        sB     |                 /     \
 *      |               |                sT     sR
 *      |               |               /  \   /  \
 *      +-------B-------+              rt  sL rt  rb
 *                                        /  \
 *                                       lt   sB
 *                                           /  \
 *                                          lb  rb
 * If Cx == Tx:
 *
 *      +-------T-------+                T
 *      |               |               / \
 *      |       sT      |              /   \
 *      |               |             /     \
 *      |  sL   C   sR  |            sL      sR
 *      L               R           /  \    /  \
 *      |               |          lt  lb  rt  rb
 *      |       sB      |
 *      |               |
 *      |               |
 *      +-------B-------+
 */

static int **gnVectorWarpImageSrcIndex=NULL;
static int **gnVectorWarpImageDestIndex=NULL;

static
int ConvolveToVectorWarp(x, y)
   int x, y;
{
   if (gConvExtraInfo.fp != NULL) {
      XColor **xcolors=gConvExtraInfo.xcolors;
      FILE *fp=gConvExtraInfo.fp;
      unsigned char buf[3];

      buf[0] = (unsigned char)xcolors[y][x].red;
      buf[1] = (unsigned char)xcolors[y][x].green;
      buf[2] = (unsigned char)xcolors[y][x].blue;

      if ((int)fwrite(buf, sizeof(char), 3, fp) <= 0) writeFileFailed = TRUE;

      return TRUE;
   } else {
      return GetOrAllocHistogramIndex(
            &tgifColors[gnVectorWarpImageDestIndex[y][x]]);
   }
}

static
void CleanUpVectorWarpData()
{
   int i, image_h=topSel->obj->detail.xpm->image_h;

   if (gnVectorWarpImageSrcIndex != NULL) {
      for (i=0; i < image_h; i++) {
         if (gnVectorWarpImageSrcIndex[i] != NULL) {
            free(gnVectorWarpImageSrcIndex[i]);
         }
      }
      free(gnVectorWarpImageSrcIndex);
      gnVectorWarpImageSrcIndex = NULL;
   }
   if (gnVectorWarpImageDestIndex != NULL) {
      for (i=0; i < image_h; i++) {
         if (gnVectorWarpImageDestIndex[i] != NULL) {
            free(gnVectorWarpImageDestIndex[i]);
         }
      }
      free(gnVectorWarpImageDestIndex);
      gnVectorWarpImageDestIndex = NULL;
   }
}

static
void SetTrapMapSegValue(nSegIndex, nPtIndex, w)
   int nSegIndex, nPtIndex, w;
{
   int i;
   double dx=(double)0.0, dy, m, b;

   dy = (double)(gaTrapMapPt[TRAP_PT_C].y - gaTrapMapPt[nPtIndex].y);
   if (gaTrapMapPt[TRAP_PT_C].x == gaTrapMapPt[nPtIndex].x) {
      dx = (double)0.0;
      m = (double)0.0;
   } else {
      dx = (double)(gaTrapMapPt[TRAP_PT_C].x - gaTrapMapPt[nPtIndex].x);
      m = ((double)dy) / ((double)dx);
   }
   b = ((double)(gaTrapMapPt[TRAP_PT_C].y)) -
         m * ((double)(gaTrapMapPt[TRAP_PT_C].x));
   gaTrapMapSeg[nSegIndex].m = m;
   gaTrapMapSeg[nSegIndex].b = b;
   for (i=0; i < w; i++) {
      double y=m*((double)(i))+b;

      gpSegYIntersects[nSegIndex][i] = round(y);
   }
}

static
void SetTrapMapExtraValue(nLeafIndex, end_x, end_y, w)
   int nLeafIndex, end_x, end_y, w;
{
   int i;
   double dx=(double)0.0, dy, m, b;

   dy = (double)(gaTrapMapPt[TRAP_PT_C].y - end_y);
   if (gaTrapMapPt[TRAP_PT_C].x == end_x) {
      dx = (double)0.0;
      m = (double)0.0;
   } else {
      dx = (double)(gaTrapMapPt[TRAP_PT_C].x - end_x);
      m = ((double)dy) / ((double)dx);
   }
   b = ((double)(gaTrapMapPt[TRAP_PT_C].y)) -
         m * ((double)(gaTrapMapPt[TRAP_PT_C].x));
   for (i=0; i < w; i++) {
      double y=m*((double)(i))+b;

      gpExtraYIntersects[nLeafIndex][i] = round(y);
   }
}

static
int CheckVectorWarpCoords(pFromPt, pToPt, dx, dy, image_w, image_h, pBBox)
   IntPoint *pFromPt, *pToPt;
   int dx, dy, image_w, image_h;
   struct BBRec *pBBox;
{
   IntPoint to_pt;
   double ddx, ddy, d;
   int i, diam, ltx, lty, rbx, rby, w, h;
   char szBuf[MAXSTRING+1];

   strcpy(szBuf, GetImageProcName(CMDID_VECTORWARP));
   if (pFromPt->x < topSel->obj->obbox.ltx ||
         pFromPt->y < topSel->obj->obbox.lty ||
         pFromPt->x >= topSel->obj->obbox.rbx ||
         pFromPt->y >= topSel->obj->obbox.rby) {
      MsgBox(TgLoadString(STID_PICK_PT_WITHIN_IMAGE_BOUND), TOOL_NAME, INFO_MB);
      return FALSE;
   } else if (image_w <= 4 || image_h <= 4) {
      sprintf(gszMsgBox, TgLoadString(STID_IMG_TOO_SMALL_IMAGEPROC_CMD), szBuf);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return FALSE;
   }
   to_pt.x = pFromPt->x+dx;
   to_pt.y = pFromPt->y+dy;
   if (to_pt.x < topSel->obj->obbox.ltx ||
         to_pt.y < topSel->obj->obbox.lty ||
         to_pt.x >= topSel->obj->obbox.rbx ||
         to_pt.y >= topSel->obj->obbox.rby) {
      if (dx == 0) {
         if (dy > 0) {
            to_pt.y = topSel->obj->obbox.rby-1;
         } else {
            to_pt.y = topSel->obj->obbox.lty;
         }
      } else {
         double m, b, x_intersect, y_intersect;
         int intersect=CORNER_TOP;

         m = ((double)dy) / ((double)dx);
         b = ((double)(pFromPt->y)) - m * ((double)(pFromPt->x));
         if (dx < 0) {
            y_intersect = m * ((double)(topSel->obj->obbox.ltx)) + b;
            if (y_intersect < topSel->obj->obbox.lty) {
               intersect = CORNER_TOP;
            } else if (y_intersect > topSel->obj->obbox.rby-1) {
               intersect = CORNER_BOTTOM;
            } else {
               intersect = CORNER_LEFT;
            }
         } else {
            y_intersect = m * ((double)(topSel->obj->obbox.rbx-1)) + b;
            if (y_intersect < topSel->obj->obbox.lty) {
               intersect = CORNER_TOP;
            } else if (y_intersect > topSel->obj->obbox.rby-1) {
               intersect = CORNER_BOTTOM;
            } else {
               intersect = CORNER_RIGHT;
            }
         }
         switch (intersect) {
         case CORNER_TOP:
            /* intersects the obbox at the top */
            to_pt.y = topSel->obj->obbox.lty;
            x_intersect = (((double)(to_pt.y)) - b) / m;
            to_pt.x = round(x_intersect);
            if (to_pt.x < topSel->obj->obbox.ltx) {
               to_pt.x = topSel->obj->obbox.ltx;
            } else if (to_pt.x >= topSel->obj->obbox.rbx) {
               to_pt.x = topSel->obj->obbox.rbx-1;
            }
            break;
         case CORNER_BOTTOM:
            /* intersects the obbox at the bottom */
            to_pt.y = topSel->obj->obbox.rby;
            x_intersect = (((double)(to_pt.y)) - b) / m;
            to_pt.x = round(x_intersect);
            if (to_pt.x < topSel->obj->obbox.ltx) {
               to_pt.x = topSel->obj->obbox.ltx;
            } else if (to_pt.x >= topSel->obj->obbox.rbx) {
               to_pt.x = topSel->obj->obbox.rbx-1;
            }
            break;
         case CORNER_LEFT:
            /* intersects the obbox at the left */
            to_pt.x = topSel->obj->obbox.lty;
            y_intersect = (m * ((double)(to_pt.x))) + b;
            to_pt.y = round(y_intersect);
            if (to_pt.y < topSel->obj->obbox.lty) {
               to_pt.y = topSel->obj->obbox.lty;
            } else if (to_pt.y >= topSel->obj->obbox.rby) {
               to_pt.y = topSel->obj->obbox.rby-1;
            }
            break;
         case CORNER_RIGHT:
            /* intersects the obbox at the right */
            to_pt.x = topSel->obj->obbox.rby;
            y_intersect = (m * ((double)(to_pt.x))) + b;
            to_pt.y = round(y_intersect);
            if (to_pt.y < topSel->obj->obbox.lty) {
               to_pt.y = topSel->obj->obbox.lty;
            } else if (to_pt.y >= topSel->obj->obbox.rby) {
               to_pt.y = topSel->obj->obbox.rby-1;
            }
            break;
         }
      }
   }
   ddx = (double)(to_pt.x - pFromPt->x);
   ddy = (double)(to_pt.y - pFromPt->y);
   d = ((double)gfVectorWarpSoftness)*(double)sqrt((double)(ddx*ddx+ddy*ddy));
   diam = round(d);
   ltx = pFromPt->x-diam - topSel->obj->obbox.ltx;
   lty = pFromPt->y-diam - topSel->obj->obbox.lty;
   rbx = pFromPt->x+diam - topSel->obj->obbox.ltx;
   rby = pFromPt->y+diam - topSel->obj->obbox.lty;
   if (ltx < 0) ltx = 0;
   if (lty < 0) lty = 0;
   if (rbx >= image_w) rbx = image_w;
   if (rby >= image_h) rby = image_h;
   pBBox->ltx = ltx; pBBox->lty = lty;
   pBBox->rbx = rbx; pBBox->rby = rby;
   pFromPt->x -= topSel->obj->obbox.ltx;
   pFromPt->y -= topSel->obj->obbox.lty;
   pToPt->x = to_pt.x - topSel->obj->obbox.ltx;
   pToPt->y = to_pt.y - topSel->obj->obbox.lty;
   if (pToPt->x == pFromPt->x) {
      if (pToPt->y == pFromPt->y) {
         Msg(TgLoadString(STID_NO_WARPING));
         return FALSE;
      }
      gpTrapMapSpec = gCenterTrapMapSpec;
   } else if (pToPt->x < pFromPt->x) {
      gpTrapMapSpec = gLeftTrapMapSpec;
   } else {
      gpTrapMapSpec = gRightTrapMapSpec;
   }
   w = pBBox->rbx-pBBox->ltx;
   h = pBBox->rby-pBBox->lty;

   gpSegYIntersects = (int**)malloc(4*sizeof(int*));
   gpExtraYIntersects = (int**)malloc(4*sizeof(int*));
   if (gpSegYIntersects == NULL || gpExtraYIntersects == NULL) {
      FailAllocMessage();
      if (gpSegYIntersects != NULL) free(gpSegYIntersects);
      if (gpExtraYIntersects != NULL) free(gpExtraYIntersects);
      return FALSE;
   }
   for (i=0; i < 4; i++) {
      gpSegYIntersects[i] = (int*)malloc(w*sizeof(int));
      gpExtraYIntersects[i] = (int*)malloc(w*sizeof(int));
      if (gpSegYIntersects[i] == NULL || gpExtraYIntersects[i] == NULL) {
         FailAllocMessage();
         return FALSE;
      }
      memset(gpSegYIntersects[i], 0, w*sizeof(int));
      memset(gpExtraYIntersects[i], 0, w*sizeof(int));
   }

   /* sets L, T, R, B, and C points */
   gaTrapMapPt[TRAP_PT_L].x = pBBox->ltx - pBBox->ltx;
   gaTrapMapPt[TRAP_PT_L].y = pFromPt->y - pBBox->lty;
   gaTrapMapPt[TRAP_PT_T].x = pFromPt->x - pBBox->ltx;
   gaTrapMapPt[TRAP_PT_T].y = pBBox->lty - pBBox->lty;
   gaTrapMapPt[TRAP_PT_R].x = pBBox->rbx - pBBox->ltx;
   gaTrapMapPt[TRAP_PT_R].y = pFromPt->y - pBBox->lty;
   gaTrapMapPt[TRAP_PT_B].x = pFromPt->x - pBBox->ltx;
   gaTrapMapPt[TRAP_PT_B].y = pBBox->rby - pBBox->lty;
   gaTrapMapPt[TRAP_PT_C].x = pToPt->x - pBBox->ltx;
   gaTrapMapPt[TRAP_PT_C].y = pToPt->y - pBBox->lty;

   /* sets sL, sT, sR, and sB line segments */
   SetTrapMapSegValue(TRAP_SEG_L, TRAP_PT_L, w);
   SetTrapMapSegValue(TRAP_SEG_T, TRAP_PT_T, w);
   SetTrapMapSegValue(TRAP_SEG_R, TRAP_PT_R, w);
   SetTrapMapSegValue(TRAP_SEG_B, TRAP_PT_B, w);

   /* sets sL, sT, sR, and sB line segments */
   SetTrapMapExtraValue(TRAP_LEAF_LT, 0, 0, w);
   SetTrapMapExtraValue(TRAP_LEAF_RT, w, 0, w);
   SetTrapMapExtraValue(TRAP_LEAF_LB, 0, h, w);
   SetTrapMapExtraValue(TRAP_LEAF_RB, w, h, w);

   /* sets lt, rt, lb, rb quardrants */
   gaTrapMapLeaf[TRAP_LEAF_LT] = TRAP_LEAF_LT;
   gaTrapMapLeaf[TRAP_LEAF_RT] = TRAP_LEAF_RT;
   gaTrapMapLeaf[TRAP_LEAF_LB] = TRAP_LEAF_LB;
   gaTrapMapLeaf[TRAP_LEAF_RB] = TRAP_LEAF_RB;

   return TRUE;
}

static
int GetQuadrant(pTrapMap, x, y)
   TrapMap *pTrapMap;
   int x, y;
   /* returns one of the TRAP_LEAF_*'s */
{
   switch (pTrapMap->type) {
   case TRAP_NODE_LEAF:
      return pTrapMap->detail.leaf.which;
   case TRAP_NODE_PT:
      if (x < pTrapMap->detail.pt.data->x) {
         return GetQuadrant(pTrapMap->detail.pt.left, x, y);
      }
      return GetQuadrant(pTrapMap->detail.pt.right, x, y);
   case TRAP_NODE_SEG:
      if (y < gpSegYIntersects[pTrapMap->detail.seg.which][x]) {
         return GetQuadrant(pTrapMap->detail.seg.above, x, y);
      }
      return GetQuadrant(pTrapMap->detail.seg.below, x, y);
   }
   return (-1);
}

/*
 *     w                 w      dw
 *  +-----+           +-----+------+
 *  |  o  |  warp     |      \
 * h|(x,y)| ======>  h|   o   \
 *  |     |           | (X,Y)  \      X is also known as x_hat
 *  +-----+           +__       \     Y is also known as y_hat
 *                    |  \__     \
 *                  dh|     \__   \
 *                    +        \___\
 *
 * w -> x_off, w+dw -> d_new_w, W -> d_bbox_w, x_hat -> new_x
 * h -> y_off, h+dh -> d_new_h, H -> d_bbox_h, y_hat -> new_y
 */

static
int ComputeVectorWarpData(pFromPt, dx, dy)
   IntPoint *pFromPt;
   int dx, dy;
{
   struct XPmRec *xpm_ptr=topSel->obj->detail.xpm;
   int i, image_w=xpm_ptr->image_w, image_h=xpm_ptr->image_h;
   int bbox_w, bbox_h, x_off, y_off, ppm6=FALSE;
   double d_bbox_w, d_bbox_h, d_x_off, d_y_off, d_new_w, d_new_h, ddw, ddh;
   Pixmap pixmap=xpm_ptr->pixmap;
   XImage *image=NULL;
   struct BBRec bbox; /* only points within bbox need to be warped */
   IntPoint to_pt;
   XColor **xcolors=NULL, **bg_xcolors=NULL;
   ProgressInfo pi;

   if (!CheckVectorWarpCoords(pFromPt, &to_pt, dx, dy, image_w, image_h,
         &bbox)) {
      return FALSE;
   }
   ppm6 = DoPpm6(xpm_ptr);
   /*
    * w -> x_off, w+dw -> new_w, W -> bbox_w, x_hat -> new_x
    * h -> y_off, h+dh -> new_h, H -> bbox_h, y_hat -> new_y
    */
   bbox_w = bbox.rbx - bbox.ltx; d_bbox_w = (double)bbox_w;
   bbox_h = bbox.rby - bbox.lty; d_bbox_h = (double)bbox_h;
   x_off = pFromPt->x - bbox.ltx; d_x_off = (double)x_off;
   y_off = pFromPt->y - bbox.lty; d_y_off = (double)y_off;
   d_new_w = (double)(to_pt.x - bbox.ltx);
   d_new_h = (double)(to_pt.y - bbox.lty);
   ddw = d_new_w - ((double)x_off);
   ddh = d_new_h - ((double)y_off);

   if (!BuildTrapMap(gpTrapMapSpec)) {
      return FALSE;
   }
   image = XGetImage(mainDisplay, pixmap, 0, 0, image_w, image_h, AllPlanes,
         ZPixmap);
   if (image == NULL) {
      FailAllocMessage();
      return FALSE;
   }
   if (ppm6) {
      if (!InitTrueColorInfo(image, &gTrueColorInfo, image_w)) {
         sprintf(gszMsgBox, TgLoadString(STID_CANNOT_GET_IMAGE_OF_SIZE),
               image_w, image_h);
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
         XDestroyImage(image);
         return FALSE;
      }
      if (!SetConvExtraInfo(NULL, image_w, image_h, image, NULL)) {
         XDestroyImage(image);
         return FALSE;
      }
      xcolors = gConvExtraInfo.xcolors;
      bg_xcolors = gConvExtraInfo.bg_xcolors;
   } else {
      if (!CreatePixelToIndexMapping()) {
         XDestroyImage(image);
         return FALSE;
      }
      gnVectorWarpImageSrcIndex = (int**)malloc(image_h*sizeof(int*));
      gnVectorWarpImageDestIndex = (int**)malloc(image_h*sizeof(int*));
      if (gnVectorWarpImageSrcIndex == NULL ||
            gnVectorWarpImageDestIndex == NULL) {
         if (gnVectorWarpImageSrcIndex != NULL) free(gnVectorWarpImageSrcIndex);
         if (gnVectorWarpImageDestIndex != NULL) {
            free(gnVectorWarpImageDestIndex);
         }
         FailAllocMessage();
         XDestroyImage(image);
         CleanUpIndexOfPixel();

         return FALSE;
      }
      memset(gnVectorWarpImageSrcIndex, 0, image_h*sizeof(int*));
      memset(gnVectorWarpImageDestIndex, 0, image_h*sizeof(int*));
      BeginProgress(&pi, image_h);
      for (i=0; i < image_h; i++) {
         int j;

         UpdateProgress(&pi, i);
         gnVectorWarpImageSrcIndex[i] = (int*)malloc(image_w*sizeof(int));
         gnVectorWarpImageDestIndex[i] = (int*)malloc(image_w*sizeof(int));
         if (gnVectorWarpImageSrcIndex[i] == NULL ||
               gnVectorWarpImageDestIndex[i] == NULL) {
            FailAllocMessage();
            for (j=0; j < i; j++) {
               if (gnVectorWarpImageSrcIndex[j] != NULL) {
                  free(gnVectorWarpImageSrcIndex[j]);
               }
            }
            free(gnVectorWarpImageSrcIndex);
            gnVectorWarpImageSrcIndex = NULL;
            for (j=0; j < i; j++) {
               if (gnVectorWarpImageDestIndex[j] != NULL) {
                  free(gnVectorWarpImageDestIndex[j]);
               }
            }
            free(gnVectorWarpImageDestIndex);
            gnVectorWarpImageDestIndex = NULL;
            XDestroyImage(image);
            CleanUpIndexOfPixel();

            return FALSE;
         }
         for (j=0; j < image_w; j++) {
            gnVectorWarpImageSrcIndex[i][j] = gnVectorWarpImageDestIndex[i][j] =
                  GetIndexOfPixel(XGetPixel(image,j,i));
         }
      }
   }
   BeginProgress(&pi, image_h);
   srand(0);
   for (i=0; i < image_h; i++) {
      int j;

      UpdateProgress(&pi, i);
      if (i < bbox.lty || i >= bbox.rby) continue;
      for (j=0; j < image_w; j++) {
         int quadrant, new_x, new_y;

         if (j < bbox.ltx || j >= bbox.rbx) continue;

         new_x = j - bbox.ltx;
         new_y = i - bbox.lty;
         quadrant = GetQuadrant(gpTrapMap, new_x, new_y);
         if (quadrant != -1) {
            double d_new_x=(double)new_x, d_new_y=(double)new_y;
            double ddx=(double)0.0, ddy=(double)0.0, frac;
            int x, y, above;

            above = (new_y < gpExtraYIntersects[quadrant][new_x]);

            switch (quadrant) {
            case TRAP_LEAF_LT:
               if (above) {
                  ddx = d_new_x - ddw*d_new_y/d_new_h;
                  ddy = d_y_off*d_new_y/d_new_h;
               } else {
                  ddx = d_x_off*d_new_x/d_new_w;
                  ddy = d_new_y - ddh*d_new_x/d_new_w;
               }
               break;
            case TRAP_LEAF_RT:
               if (above) {
                  ddx = d_new_x - ddw*d_new_y/d_new_h;
                  ddy = d_y_off*d_new_y/d_new_h;
               } else {
                  frac = (d_new_x-d_bbox_w)/(d_new_w-d_bbox_w);
                  ddx = d_bbox_w - (d_bbox_w-d_x_off)*frac;
                  ddy = d_new_y - ddh*frac;
               }
               break;
            case TRAP_LEAF_LB:
               if (above) {
                  ddx = d_x_off*d_new_x/d_new_w;
                  ddy = d_new_y - ddh*d_new_x/d_new_w;
               } else {
                  frac = (d_new_y-d_bbox_h)/(d_new_h-d_bbox_h);
                  ddx = d_new_x - ddw*frac;
                  ddy = d_bbox_h - (d_bbox_h-d_y_off)*frac;
               }
               break;
            case TRAP_LEAF_RB:
               if (above) {
                  frac = (d_new_x-d_bbox_w)/(d_new_w-d_bbox_w);
                  ddx = d_bbox_w - (d_bbox_w-d_x_off)*frac;
                  ddy = d_new_y - ddh*frac;
               } else {
                  frac = (d_new_y-d_bbox_h)/(d_new_h-d_bbox_h);
                  ddx = d_new_x - ddw*frac;
                  ddy = d_bbox_h - (d_bbox_h-d_y_off)*frac;
               }
               break;
            }
            x = round(ddx);
            y = round(ddy);
            x += bbox.ltx;
            y += bbox.lty;
            if (x < bbox.ltx) x = bbox.ltx;
            if (x >= bbox.rbx) x = bbox.rbx-1;
            if (y < bbox.lty) y = bbox.lty;
            if (y >= bbox.rby) y = bbox.rby-1;
            if (ppm6) {
               xcolors[i][j] = bg_xcolors[y][x];
            } else {
               gnVectorWarpImageDestIndex[i][j] =
                     gnVectorWarpImageSrcIndex[y][x];
            }
         }
      }
   }
   XDestroyImage(image);
   CleanUpIndexOfPixel();

   return TRUE;
}

static
int SpecifyLineSeg(pnFromAbsX, pnFromAbsY, pnToAbsX, pnToAbsY)
   int *pnFromAbsX, *pnFromAbsY, *pnToAbsX, *pnToAbsY;
{
   int started=FALSE, done=FALSE, orig_x=0, orig_y=0, grid_x=0, grid_y=0;
   int root_x, root_y, saved_snap_on=snapOn;
   char buf[80];
   unsigned int status;
   Window root_win, child_win;

   snapOn = FALSE;
   *buf = '\0';
   SetMouseStatus(TgLoadString(STID_START_LINE_SEG_DOTS),
         TgLoadCachedString(CSTID_FINISH), TgLoadCachedString(CSTID_FINISH));
   XGrabPointer(mainDisplay, drawWindow, FALSE,
         PointerMotionMask | ButtonPressMask | ButtonReleaseMask,
         GrabModeAsync, GrabModeAsync, None, handCursor, CurrentTime);
   XQueryPointer(mainDisplay, drawWindow, &root_win, &child_win,
         &root_x, &root_y, &orig_x, &orig_y, &status);
   GridXY(orig_x, orig_y, &grid_x, &grid_y);
   MarkRulers(grid_x, grid_y);
   orig_x = grid_x;
   orig_y = grid_y;
   while (!done) {
      XEvent input;

      XNextEvent(mainDisplay, &input);
      switch (input.type) {
      case Expose: ExposeEventHandler(&input, TRUE); break;
      case VisibilityNotify: ExposeEventHandler(&input, TRUE); break;
      case ButtonPress:
         if (input.xbutton.button == Button1) {
            MarkRulers(grid_x, grid_y);
            SetMouseStatus(TgLoadString(STID_ENDT_LINE_SEG_DOTS), "", "");
            GridXY(input.xbutton.x, input.xbutton.y, &grid_x, &grid_y);
            orig_x = grid_x;
            orig_y = grid_y;
            XDrawLine(mainDisplay, drawWindow, revDefaultGC, orig_x, orig_y,
                  grid_x, grid_y);
            MarkRulers(grid_x, grid_y);
            started = TRUE;
         } else {
            XUngrabPointer(mainDisplay, CurrentTime);
            XSync(mainDisplay, False);
            done = TRUE;
            started = FALSE;
         }
         break;
      case MotionNotify:
         MarkRulers(grid_x, grid_y);
         if (started) {
            XDrawLine(mainDisplay, drawWindow, revDefaultGC, orig_x, orig_y,
                  grid_x, grid_y);
         }
         GridXY(input.xmotion.x, input.xmotion.y, &grid_x, &grid_y);
         if (started) {
            XDrawLine(mainDisplay, drawWindow, revDefaultGC, orig_x, orig_y,
                  grid_x, grid_y);
         }
         MarkRulers(grid_x, grid_y);
         break;
      case ButtonRelease:
         XUngrabPointer(mainDisplay, CurrentTime);
         XSync(mainDisplay, False);
         done = TRUE;
         MarkRulers(grid_x, grid_y);
         XDrawLine(mainDisplay, drawWindow, revDefaultGC, orig_x, orig_y,
               grid_x, grid_y);
      }
   }
   snapOn = saved_snap_on;
   if (started && !(orig_x == grid_x && orig_y == grid_y)) {
      *pnFromAbsX = ABS_X(orig_x);
      *pnFromAbsY = ABS_Y(orig_y);
      *pnToAbsX = ABS_X(grid_x);
      *pnToAbsY = ABS_Y(grid_y);
      return TRUE;
   }
   return FALSE;
}

void VectorWarp()
{
   int ok=TRUE;
   IntPoint from_pt, to_pt;

   if (!CheckSelectionForImageProc(CMDID_VECTORWARP)) {
      return;
   }
   memset(&from_pt, 0, sizeof(IntPoint));
   memset(&to_pt, 0, sizeof(IntPoint));
   SaveStatusStrings();
   while (ok) {
      if (somethingHighLighted) HighLightReverse();
      ok = SpecifyLineSeg(&from_pt.x, &from_pt.y, &to_pt.x, &to_pt.y);
      if (!somethingHighLighted) HighLightForward();

      if (!ok) break;

      gpConvolveCmdID = CMDID_VECTORWARP;
      if (!ComputeVectorWarpData(&from_pt, to_pt.x-from_pt.x,
            to_pt.y-from_pt.y)) {
         CleanTrapMap();
         gpConvolveCmdID = (-1);
         break;
      }
      CleanTrapMap();

      gpConvolveFunc = (NLFN*)ConvolveToVectorWarp;
      gpConvolveCmdID = CMDID_VECTORWARP;
      gnConvolving = TRUE;
      DoImageProc(NULL);
      gnConvolving = FALSE;
      gpConvolveFunc = NULL;
      gpConvolveCmdID = (-1);

      CleanUpVectorWarpData();
   }
   RestoreStatusStrings();
}

/* ======================= Non-Image Processing ======================= */

/* ----------------------- RunBggen ----------------------- */

static
int GetBggenImageSize(p_image_w, p_image_h)
   int *p_image_w, *p_image_h;
{
   char spec[MAXSTRING+1], *dup_spec=NULL, *part1=NULL, *part2=NULL;
   int ok=TRUE;

   *spec = '\0';
   if (Dialog(TgLoadString(STID_ENTER_IMAGE_SIZE_IN_PIX_RC),
         TgLoadCachedString(CSTID_DLG_ACCEPT_CANCEL), spec)==INVALID) {
      return FALSE;
   }
   UtilTrimBlanks(spec);
   if (*spec == '\0') return FALSE;
   if ((dup_spec=UtilStrDup(spec)) == NULL) {
      FailAllocMessage();
      return FALSE;
   }
   if ((part1=strtok(dup_spec, " ,xX[]")) != NULL &&
         (part2=strtok(NULL, " ,xX[]")) != NULL) {
      *p_image_w = atoi(part1);
      *p_image_h = atoi(part2);
      if ((*p_image_w) <= 0 || (*p_image_h) <= 0) {
         ok = FALSE;
      }
   } else {
      ok = FALSE;
   }
   if (!ok) {
      sprintf(gszMsgBox, TgLoadString(STID_INVALID_SPEC), dup_spec);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
   }
   free(dup_spec);
   return ok;
}

static
int BggenGenerateXpm(image_w, image_h, sz_spec, sz_path, path_buf_sz)
   int image_w, image_h, path_buf_sz;
   char *sz_spec, *sz_path;
{
   char *psz_cmd, sz_geom[MAXSTRING+1];
   FILE *pFile, *pPipe;
   int bytes_read;

   if (MkTempFile(sz_path, path_buf_sz, tmpDir, TOOL_NAME) == NULL) {
      return FALSE;
   }
   sprintf(sz_geom, "%1dx%1d", image_w, image_h);
   if (fullTrueColorMode && HasZlibSupport()) {
      sprintf(gszMsgBox, bggenToPpm6Cmd, sz_spec, sz_geom);
   } else {
      sprintf(gszMsgBox, bggenToXpmCmd, sz_spec, sz_geom);
   }
   if ((psz_cmd=UtilStrDup(gszMsgBox)) == NULL) {
      return FailAllocMessage();
   }
   if (!FindProgramInPath(psz_cmd, NULL, FALSE)) {
      free(psz_cmd);
      return FALSE;
   }
   if ((pFile=fopen(sz_path,"w")) == NULL) {
      sprintf(gszMsgBox, TgLoadString(STID_CANNOT_OPEN_FILE_FOR_WRITING),
            sz_path);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      free(psz_cmd);
      return FALSE;
   }
   Msg("Executing:");
   sprintf(gszMsgBox, "    %s", psz_cmd);
   Msg(gszMsgBox);
   sprintf(gszMsgBox, "Executing '%s'...", psz_cmd);
   SetStringStatus(gszMsgBox);
   XSync(mainDisplay, False);
   if ((pPipe=(FILE*)popen(psz_cmd,"r")) == NULL) {
      sprintf(gszMsgBox, TgLoadString(STID_FAIL_TO_EXECUTE_CMD), psz_cmd);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      free(psz_cmd);
      fclose(pFile);
      unlink(sz_path);
      return FALSE;
   }
   writeFileFailed = FALSE;
   while ((bytes_read=fread(gszMsgBox, sizeof(char), sizeof(gszMsgBox),
         pPipe)) > 0) {
      if ((int)fwrite(gszMsgBox, sizeof(char), bytes_read, pFile) <= 0) {
         writeFileFailed = TRUE;
         break;
      }
   }
   pclose(pPipe);
   SetStringStatus(TgLoadCachedString(CSTID_DOTS_DONE));
   free(psz_cmd);
   fclose(pFile);
   if (writeFileFailed) {
      FailToWriteFileMessage(sz_path);
      unlink(sz_path);
      return FALSE;
   }
   return TRUE;
}

void RunBggen()
{
   int image_w=0, image_h=0, w, h, short_name, rc, use_obj_pos=FALSE;
   int ltx=0, lty=0;
   int ncolors=0, chars_per_pixel=0, first_pixel_is_bg=0, *pixels=NULL;
   char szSpec[MAXSTRING+1], szPath[MAXPATHLENGTH+1], *rest;
   char *color_char=NULL, **color_str=NULL, *xpm_data=NULL;
   struct ObjRec *obj_ptr;
   Pixmap pixmap=None, bitmap=None;
   XImage *image=NULL, *bitmap_image=NULL;

   if (curChoice != NOTHING || topSel == NULL) {
      MakeQuiescent();
      if (!GetBggenImageSize(&image_w, &image_h)) {
         return;
      }
   } else if (!CheckSelectionForImageProc(CMDID_RUNBGGEN)) {
      return;
   } else {
      obj_ptr = topSel->obj;
      ltx = obj_ptr->obbox.ltx;
      lty = obj_ptr->obbox.lty;
      image_w = obj_ptr->obbox.rbx - ltx;
      image_h = obj_ptr->obbox.rby - lty;
      use_obj_pos = TRUE;
      HighLightReverse();
   }
   *szSpec = '\0';
   Dialog(TgLoadString(STID_ENTER_CMD_OP_FOR_BGGEN),
         TgLoadCachedString(CSTID_DLG_ACCEPT_CANCEL), szSpec);
   UtilTrimBlanks(szSpec);
   if (*szSpec == '\0') {
      if (use_obj_pos) HighLightForward();
      return;
   }
   SaveStatusStrings();
   if (!BggenGenerateXpm(image_w, image_h, szSpec, szPath, sizeof(szPath))) {
      RestoreStatusStrings();
      if (use_obj_pos) HighLightForward();
      return;
   }
   RestoreStatusStrings();

   if (fullTrueColorMode && HasZlibSupport()) {
      char deflated_fname[MAXPATHLENGTH+1];
      struct XPmRec *xpm_ptr=NULL;

      ResetPngHeaderInfo(&gPngHeaderInfo);
      obj_ptr = CreatePpmTrueObjFromFile(szPath);
      if (obj_ptr != NULL &&
            MkTempFile(deflated_fname, sizeof(deflated_fname), tmpDir,
            TOOL_NAME) != NULL &&
            DeflateFile(szPath, deflated_fname)) {
         /* good */
      } else {
         FreeObj(obj_ptr);

         sprintf(gszMsgBox, TgLoadString(STID_CANNOT_IMPORT_GIVEN_PPM), szPath);
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
         unlink(szPath);
         return;
      }
      xpm_ptr = obj_ptr->detail.xpm;
      xpm_ptr->real_type = PPM_TRUE;
      xpm_ptr->ppm_data_compress = PPM_DATA_DEFLATED;
      xpm_ptr->ppm_data = ReadFileIntoBuf(deflated_fname,
            &xpm_ptr->ppm_data_size);
      xpm_ptr->ppm_mask_data = NULL;
      xpm_ptr->ppm_mask_size = 0;
      unlink(deflated_fname);
   } else {
      SetWatchCursor(drawWindow);
      SetWatchCursor(mainWindow);
      rc = MyReadPixmapFile(szPath, &image_w, &image_h, &w, &h, &pixmap,
            &image, &bitmap, &bitmap_image, &ncolors, &chars_per_pixel,
            &first_pixel_is_bg, &color_char, &color_str, &pixels, &xpm_data);
      SetDefaultCursor(mainWindow);
      ShowCursor();

      if ((short_name=IsPrefix(bootDir, szPath, &rest))) ++rest;
      if (rc != BitmapSuccess) {
         if (use_obj_pos) HighLightForward();
         sprintf(gszMsgBox, TgLoadString(STID_CANNOT_IMPORT_XPM_FILE),
               (short_name ? rest : szPath));
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
         unlink(szPath);
         return;
      }
      obj_ptr = CreateXPmObj(image_w, image_h, w, h, pixmap, image, bitmap,
            bitmap_image, ncolors, chars_per_pixel, first_pixel_is_bg,
            color_char, color_str, pixels, xpm_data);
   }
   unlink(szPath);
   AddObj(NULL, topObj, obj_ptr);
   if (use_obj_pos) {
      RemoveAllSel();
      MoveObj(obj_ptr, ltx-obj_ptr->obbox.ltx, lty-obj_ptr->obbox.lty);
      numRedrawBBox = 0;
      obj_ptr->tmp_parent = NULL;
      DrawObj(drawWindow, obj_ptr);
   } else {
      PlaceTopObj(obj_ptr, NULL, NULL);
   }
   SelectTopObj();
   RecordNewObjCmd();
   SetFileModified(TRUE);
   justDupped = FALSE;
   if (!PRTGIF && colorLayers && needToRedrawColorWindow) {
      RedrawColorWindow();
   }
   sprintf(gszMsgBox, TgLoadString(STID_NEW_XPM_WH_GENERATED),
         image_w, image_h);
   Msg(gszMsgBox);
}

/* ----------------------- CircularBggen ----------------------- */

static
int CircularBggenGenerateXpm(image_w, image_h, ncolors, sz_path, path_buf_sz)
   int image_w, image_h, ncolors, path_buf_sz;
   char *sz_path;
{
   FILE *pFile=NULL;
   int i=0, cx=0, cy=0;
   float fval=(float)0, finc=65535.0/((float)(ncolors-1));
   double max_dist=(double)0;
   ProgressInfo pi;

   if (MkTempFile(sz_path, path_buf_sz, tmpDir, TOOL_NAME) == NULL) {
      return FALSE;
   }
   gpHistogram = (XColor*)malloc(ncolors*sizeof(XColor));
   if (gpHistogram == NULL) return FailAllocMessage();
   memset(gpHistogram, 0, ncolors*sizeof(XColor));

   for (i=0, fval=0.0; i < ncolors; i++, fval+=finc) {
      int ival;

      ival = round(fval);
      HISTOGRAMRED(i) = HISTOGRAMGREEN(i) = HISTOGRAMBLUE(i) =
            (unsigned int)ival;
   }
   i--;
   HISTOGRAMRED(i) = HISTOGRAMGREEN(i) = HISTOGRAMBLUE(i) = 65535;

   cx = (image_w>>1);
   cy = (image_h>>1);
   max_dist=(double)sqrt((double)(cx*cx+cy*cy));

   if (fullTrueColorMode && HasZlibSupport()) {
      if ((pFile=fopen(sz_path,"w")) == NULL) {
         sprintf(gszMsgBox, TgLoadString(STID_CANNOT_OPEN_FILE_FOR_WRITING),
               sz_path);
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
         free(gpHistogram);
         gpHistogram = NULL;
         return FALSE;
      }
      writeFileFailed = FALSE;
      if (fprintf(pFile, "P6\n%1d %1d\n255\n", image_w, image_h) == EOF) {
         writeFileFailed = TRUE;
      }
   } else {
      gnFinalImageIndex = (int**)malloc(image_h*sizeof(int*));
      if (gnFinalImageIndex == NULL) {
         free(gpHistogram);
         gpHistogram = NULL;
         return FailAllocMessage();
      }
      memset(gnFinalImageIndex, 0, image_h*sizeof(int*));
   }
   BeginProgress(&pi, image_h);
   for (i=0; i < image_h; i++) {
      int j, dy2=(i-cy)*(i-cy);

      UpdateProgress(&pi, i);
      if (fullTrueColorMode && HasZlibSupport()) {
         /* don't allocate */
      } else {
         if ((gnFinalImageIndex[i]=(int*)malloc(image_w*sizeof(int))) == NULL) {
            for (j=0; j < i; j++) free(gnFinalImageIndex[j]);
            free(gnFinalImageIndex);
            free(gpHistogram);
            gnFinalImageIndex = NULL;
            gpHistogram = NULL;
            return FailAllocMessage();
         }
      }
      for (j=0; j < image_w; j++) {
         int dx2=(j-cx)*(j-cx);
         double dist=(double)sqrt((double)(dx2+dy2));
         double dgray=((double)ncolors)*dist/max_dist+0.5;
         int index=round(dgray);

         if (index < 0) index = 0;
         if (index >= ncolors) index = ncolors-1;
         if (fullTrueColorMode && HasZlibSupport()) {
            unsigned int gray=0;
            unsigned char buf[3];

            gray = (unsigned int)HISTOGRAMRED(ncolors-1-index);
            gray >>= 8;
            buf[0] = buf[1] = buf[2] = (unsigned char)gray;
            if ((int)fwrite(buf, sizeof(char), 3, pFile) <= 0) {
               writeFileFailed = TRUE;
            }
         } else {
            gnFinalImageIndex[i][j] = ncolors-1-index;
         }
      }
   }
   gnTransparentIndex = (-1);
   if (fullTrueColorMode && HasZlibSupport()) {
      fclose(pFile);
   } else {
      if ((pFile=fopen(sz_path,"w")) == NULL) {
         sprintf(gszMsgBox, TgLoadString(STID_CANNOT_OPEN_FILE_FOR_WRITING),
               sz_path);
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
         for (i=0; i < image_h; i++) free(gnFinalImageIndex[i]);
         free(gnFinalImageIndex);
         free(gpHistogram);
         gnFinalImageIndex = NULL;
         gpHistogram = NULL;
         return FALSE;
      }
      gnImageW = image_w;
      gnImageH = image_h;
      gnHistogramEntries = ncolors;
      writeFileFailed = FALSE;
      DumpConvolution(pFile);
      fclose(pFile);
      for (i=0; i < image_h; i++) free(gnFinalImageIndex[i]);
      free(gnFinalImageIndex);
   }
   free(gpHistogram);
   gnFinalImageIndex = NULL;
   gpHistogram = NULL;
   if (writeFileFailed) {
      FailToWriteFileMessage(sz_path);
      unlink(sz_path);
      return FALSE;
   }
   return TRUE;
}

void CircularBggen()
{
   int image_w=0, image_h=0, w, h, short_name, rc, use_obj_pos=FALSE;
   int ltx=0, lty=0;
   int ncolors=0, chars_per_pixel=0, first_pixel_is_bg=0, *pixels=NULL;
   char szSpec[MAXSTRING+1], szPath[MAXPATHLENGTH+1], *rest;
   char *color_char=NULL, **color_str=NULL, *xpm_data=NULL;
   struct ObjRec *obj_ptr;
   Pixmap pixmap=None, bitmap=None;
   XImage *image=NULL, *bitmap_image=NULL;

   if (curChoice != NOTHING || topSel == NULL) {
      MakeQuiescent();
      if (!GetBggenImageSize(&image_w, &image_h)) {
         return;
      }
   } else if (!CheckSelectionForImageProc(CMDID_CIRCULARBGGEN)) {
      return;
   } else {
      obj_ptr = topSel->obj;
      ltx = obj_ptr->obbox.ltx;
      lty = obj_ptr->obbox.lty;
      image_w = obj_ptr->obbox.rbx - ltx;
      image_h = obj_ptr->obbox.rby - lty;
      use_obj_pos = TRUE;
      HighLightReverse();
   }
   *szSpec = '\0';
   Dialog(TgLoadString(STID_ENTER_GRAY_LEVELS_222),
         TgLoadCachedString(CSTID_DLG_ACCEPT_CANCEL), szSpec);
   UtilTrimBlanks(szSpec);
   if (*szSpec == '\0') {
      if (use_obj_pos) HighLightForward();
      return;
   }
   ncolors = atoi(szSpec);
   if (ncolors < 2 || ncolors > 222) {
      sprintf(gszMsgBox, TgLoadString(STID_INVALID_GIVEN_VALUE_ENTERED),
            szSpec);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      if (use_obj_pos) HighLightForward();
      return;
   }
   SaveStatusStrings();
   if (!CircularBggenGenerateXpm(image_w, image_h, ncolors, szPath,
         sizeof(szPath))) {
      RestoreStatusStrings();
      if (use_obj_pos) HighLightForward();
      return;
   }
   RestoreStatusStrings();

   if (fullTrueColorMode && HasZlibSupport()) {
      char deflated_fname[MAXPATHLENGTH+1];
      struct XPmRec *xpm_ptr=NULL;

      ResetPngHeaderInfo(&gPngHeaderInfo);
      obj_ptr = CreatePpmTrueObjFromFile(szPath);
      if (obj_ptr != NULL &&
            MkTempFile(deflated_fname, sizeof(deflated_fname), tmpDir,
            TOOL_NAME) != NULL &&
            DeflateFile(szPath, deflated_fname)) {
         /* good */
      } else {
         FreeObj(obj_ptr);

         sprintf(gszMsgBox, TgLoadString(STID_CANNOT_IMPORT_GIVEN_PPM), szPath);
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
         unlink(szPath);
         return;
      }
      xpm_ptr = obj_ptr->detail.xpm;
      xpm_ptr->real_type = PPM_TRUE;
      xpm_ptr->ppm_data_compress = PPM_DATA_DEFLATED;
      xpm_ptr->ppm_data = ReadFileIntoBuf(deflated_fname,
            &xpm_ptr->ppm_data_size);
      xpm_ptr->ppm_mask_data = NULL;
      xpm_ptr->ppm_mask_size = 0;
      unlink(deflated_fname);
   } else {
      SetWatchCursor(drawWindow);
      SetWatchCursor(mainWindow);
      rc = MyReadPixmapFile(szPath, &image_w, &image_h, &w, &h, &pixmap,
            &image, &bitmap, &bitmap_image, &ncolors, &chars_per_pixel,
            &first_pixel_is_bg, &color_char, &color_str, &pixels, &xpm_data);
      SetDefaultCursor(mainWindow);
      ShowCursor();
   
      if ((short_name=IsPrefix(bootDir, szPath, &rest))) ++rest;
      if (rc != BitmapSuccess) {
         if (use_obj_pos) HighLightForward();
         sprintf(gszMsgBox, TgLoadString(STID_CANNOT_IMPORT_XPM_FILE),
               (short_name ? rest : szPath));
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
         unlink(szPath);
         return;
      }
      obj_ptr = CreateXPmObj(image_w, image_h, w, h, pixmap, image, bitmap,
            bitmap_image, ncolors, chars_per_pixel, first_pixel_is_bg,
            color_char, color_str, pixels, xpm_data);
   }
   unlink(szPath);
   AddObj(NULL, topObj, obj_ptr);
   if (use_obj_pos) {
      RemoveAllSel();
      MoveObj(obj_ptr, ltx-obj_ptr->obbox.ltx, lty-obj_ptr->obbox.lty);
      numRedrawBBox = 0;
      obj_ptr->tmp_parent = NULL;
      DrawObj(drawWindow, obj_ptr);
   } else {
      PlaceTopObj(obj_ptr, NULL, NULL);
   }
   SelectTopObj();
   RecordNewObjCmd();
   SetFileModified(TRUE);
   justDupped = FALSE;
   if (!PRTGIF && colorLayers && needToRedrawColorWindow) {
      RedrawColorWindow();
   }
   sprintf(gszMsgBox, TgLoadString(STID_NEW_XPM_WH_GENERATED),
         image_w, image_h);
   Msg(gszMsgBox);
}

/* ----------------------- SimpleRectBggen ----------------------- */

static
int SimpleRectBggenGenerateXpm(image_w, image_h, pxcolor, sz_path, path_buf_sz)
   int image_w, image_h, path_buf_sz;
   XColor *pxcolor;
   char *sz_path;
{
   FILE *pFile=NULL;
   int i=0;
   ProgressInfo pi;

   if (MkTempFile(sz_path, path_buf_sz, tmpDir, TOOL_NAME) == NULL) {
      return FALSE;
   }
   gpHistogram = (XColor*)malloc(sizeof(XColor));
   if (gpHistogram == NULL) return FailAllocMessage();
   memset(gpHistogram, 0, sizeof(XColor));

   HISTOGRAMRED(0) = pxcolor->red;
   HISTOGRAMGREEN(0) = pxcolor->green;
   HISTOGRAMBLUE(0) = pxcolor->blue;

   if (fullTrueColorMode && HasZlibSupport()) {
      if ((pFile=fopen(sz_path,"w")) == NULL) {
         sprintf(gszMsgBox, TgLoadString(STID_CANNOT_OPEN_FILE_FOR_WRITING),
               sz_path);
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
         free(gpHistogram);
         gpHistogram = NULL;
         return FALSE;
      }
      writeFileFailed = FALSE;
      if (fprintf(pFile, "P6\n%1d %1d\n255\n", image_w, image_h) == EOF) {
         writeFileFailed = TRUE;
      }
   } else {
      gnFinalImageIndex = (int**)malloc(image_h*sizeof(int*));
      if (gnFinalImageIndex == NULL) {
         free(gpHistogram);
         gpHistogram = NULL;
         return FailAllocMessage();
      }
      memset(gnFinalImageIndex, 0, image_h*sizeof(int*));
   }
   BeginProgress(&pi, image_h);
   for (i=0; i < image_h; i++) {
      int j;

      UpdateProgress(&pi, i);
      if (fullTrueColorMode && HasZlibSupport()) {
         unsigned char buf[3];
         unsigned int r=0, g=0, b=0;

         r = (unsigned int)(pxcolor->red);
         g = (unsigned int)(pxcolor->green);
         b = (unsigned int)(pxcolor->blue);
         buf[0] = (unsigned char)((r>>8)&0x0ff);
         buf[1] = (unsigned char)((g>>8)&0x0ff);
         buf[2] = (unsigned char)((b>>8)&0x0ff);
         for (j=0; j < image_w; j++) {
            if ((int)fwrite(buf, sizeof(char), 3, pFile) <= 0) {
               writeFileFailed = TRUE;
            }
         }
      } else {
         if ((gnFinalImageIndex[i]=(int*)malloc(image_w*sizeof(int))) == NULL) {
            for (j=0; j < i; j++) free(gnFinalImageIndex[j]);
            free(gnFinalImageIndex);
            free(gpHistogram);
            gnFinalImageIndex = NULL;
            gpHistogram = NULL;
            return FailAllocMessage();
         }
         memset(gnFinalImageIndex[i], 0, image_w*sizeof(int));
      }
   }
   gnTransparentIndex = (-1);
   if (fullTrueColorMode && HasZlibSupport()) {
      fclose(pFile);
   } else {
      if ((pFile=fopen(sz_path,"w")) == NULL) {
         sprintf(gszMsgBox, TgLoadString(STID_CANNOT_OPEN_FILE_FOR_WRITING),
               sz_path);
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
         for (i=0; i < image_h; i++) free(gnFinalImageIndex[i]);
         free(gnFinalImageIndex);
         free(gpHistogram);
         gnFinalImageIndex = NULL;
         gpHistogram = NULL;
         return FALSE;
      }
      gnImageW = image_w;
      gnImageH = image_h;
      gnHistogramEntries = 1;
      writeFileFailed = FALSE;
      DumpConvolution(pFile);
      fclose(pFile);
      for (i=0; i < image_h; i++) free(gnFinalImageIndex[i]);
      free(gnFinalImageIndex);
   }
   free(gpHistogram);
   gnFinalImageIndex = NULL;
   gpHistogram = NULL;
   if (writeFileFailed) {
      FailToWriteFileMessage(sz_path);
      unlink(sz_path);
      return FALSE;
   }
   return TRUE;
}

void SimpleRectBggen()
{
   int image_w=0, image_h=0, w, h, short_name, rc, use_obj_pos=FALSE;
   int ltx=0, lty=0;
   int ncolors=0, chars_per_pixel=0, first_pixel_is_bg=0, *pixels=NULL;
   char szSpec[MAXSTRING+1], szPath[MAXPATHLENGTH+1], *rest;
   char *color_char=NULL, **color_str=NULL, *xpm_data=NULL;
   struct ObjRec *obj_ptr;
   XColor xcolor;
   Pixmap pixmap=None, bitmap=None;
   XImage *image=NULL, *bitmap_image=NULL;

   if (curChoice != NOTHING || topSel == NULL) {
      MakeQuiescent();
      if (!GetBggenImageSize(&image_w, &image_h)) {
         return;
      }
   } else if (!CheckSelectionForImageProc(CMDID_CIRCULARBGGEN)) {
      return;
   } else {
      obj_ptr = topSel->obj;
      ltx = obj_ptr->obbox.ltx;
      lty = obj_ptr->obbox.lty;
      image_w = obj_ptr->obbox.rbx - ltx;
      image_h = obj_ptr->obbox.rby - lty;
      use_obj_pos = TRUE;
      HighLightReverse();
   }
   *szSpec = '\0';
   Dialog(TgLoadString(STID_ENTER_COLOR_FOR_RECT_BGGEN),
         TgLoadCachedString(CSTID_DLG_ACCEPT_CANCEL), szSpec);
   UtilTrimBlanks(szSpec);
   if (*szSpec == '\0') {
      if (use_obj_pos) HighLightForward();
      return;
   }
   if (!TgifParseColor(szSpec, &xcolor)) {
      sprintf(gszMsgBox, TgLoadString(STID_CANNOT_PARSE_NAMED_COLOR), szSpec);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      if (use_obj_pos) HighLightForward();
      return;
   }
   SaveStatusStrings();
   if (!SimpleRectBggenGenerateXpm(image_w, image_h, &xcolor, szPath,
         sizeof(szPath))) {
      RestoreStatusStrings();
      if (use_obj_pos) HighLightForward();
      return;
   }
   RestoreStatusStrings();

   if (fullTrueColorMode && HasZlibSupport()) {
      char deflated_fname[MAXPATHLENGTH+1];
      struct XPmRec *xpm_ptr=NULL;

      ResetPngHeaderInfo(&gPngHeaderInfo);
      obj_ptr = CreatePpmTrueObjFromFile(szPath);
      if (obj_ptr != NULL &&
            MkTempFile(deflated_fname, sizeof(deflated_fname), tmpDir,
            TOOL_NAME) != NULL &&
            DeflateFile(szPath, deflated_fname)) {
         /* good */
      } else {
         FreeObj(obj_ptr);

         sprintf(gszMsgBox, TgLoadString(STID_CANNOT_IMPORT_GIVEN_PPM), szPath);
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
         unlink(szPath);
         return;
      }
      xpm_ptr = obj_ptr->detail.xpm;
      xpm_ptr->real_type = PPM_TRUE;
      xpm_ptr->ppm_data_compress = PPM_DATA_DEFLATED;
      xpm_ptr->ppm_data = ReadFileIntoBuf(deflated_fname,
            &xpm_ptr->ppm_data_size);
      xpm_ptr->ppm_mask_data = NULL;
      xpm_ptr->ppm_mask_size = 0;
      unlink(deflated_fname);
   } else {
      SetWatchCursor(drawWindow);
      SetWatchCursor(mainWindow);
      rc = MyReadPixmapFile(szPath, &image_w, &image_h, &w, &h, &pixmap,
            &image, &bitmap, &bitmap_image, &ncolors, &chars_per_pixel,
            &first_pixel_is_bg, &color_char, &color_str, &pixels, &xpm_data);
      SetDefaultCursor(mainWindow);
      ShowCursor();

      if ((short_name=IsPrefix(bootDir, szPath, &rest))) ++rest;
      if (rc != BitmapSuccess) {
         if (use_obj_pos) HighLightForward();
         sprintf(gszMsgBox, TgLoadString(STID_CANNOT_IMPORT_XPM_FILE),
               (short_name ? rest : szPath));
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
         unlink(szPath);
         return;
      }
      obj_ptr = CreateXPmObj(image_w, image_h, w, h, pixmap, image, bitmap,
            bitmap_image, ncolors, chars_per_pixel, first_pixel_is_bg,
            color_char, color_str, pixels, xpm_data);
   }
   unlink(szPath);
   AddObj(NULL, topObj, obj_ptr);
   if (use_obj_pos) {
      RemoveAllSel();
      MoveObj(obj_ptr, ltx-obj_ptr->obbox.ltx, lty-obj_ptr->obbox.lty);
      numRedrawBBox = 0;
      obj_ptr->tmp_parent = NULL;
      DrawObj(drawWindow, obj_ptr);
   } else {
      PlaceTopObj(obj_ptr, NULL, NULL);
   }
   SelectTopObj();
   RecordNewObjCmd();
   SetFileModified(TRUE);
   justDupped = FALSE;
   if (!PRTGIF && colorLayers && needToRedrawColorWindow) {
      RedrawColorWindow();
   }
   sprintf(gszMsgBox, TgLoadString(STID_NEW_XPM_WH_GENERATED),
         image_w, image_h);
   Msg(gszMsgBox);
}

/* ----------------------- RegenerateImage ----------------------- */

static
int RegenerateImageFile(pszPath, xpm_ptr)
   char *pszPath;
   struct XPmRec *xpm_ptr;
{
   int saved_colordump=colorDump, saved_left=leftExportPixelTrim;
   int saved_top=topExportPixelTrim, saved_right=rightExportPixelTrim;
   int saved_bottom=bottomExportPixelTrim, saved_where_to_print=whereToPrint;
   int dump_ppm_preferred=FALSE;

   leftExportPixelTrim = topExportPixelTrim = rightExportPixelTrim =
         bottomExportPixelTrim = 0;
   *gszImageProcXPmFile = '\0';
   gnConvolving = FALSE;

   colorDump = TRUE;
   if (xpm_ptr == NULL) {
      /* called from CreatePixmapFromSelected() */
      if (fullTrueColorMode && HasZlibSupport()) {
         whereToPrint = PPM_FILE;
         dump_ppm_preferred = TRUE;
      } else {
         whereToPrint = XBM_FILE;
      }
   } else {
      /* called from RegenerateImage() or CropImage() */
      if (DoPpm6(xpm_ptr)) {
         whereToPrint = PPM_FILE;
         dump_ppm_preferred = TRUE;
      } else if (xpm_ptr->real_type == XPM_XPM &&
            fullTrueColorMode && HasZlibSupport()) {
         whereToPrint = PPM_FILE;
         dump_ppm_preferred = TRUE;
      } else {
         whereToPrint = XBM_FILE;
      }
   }
   gnInImageProc = TRUE;
   gpImageMapColorFunc = NULL;

   SetWatchCursor(drawWindow);
   SetWatchCursor(mainWindow);
   DumpXBitmapFile(gnInImageProc, dump_ppm_preferred, FALSE);
   SetDefaultCursor(mainWindow);
   ShowCursor();

   gnInImageProc = FALSE;
   whereToPrint = saved_where_to_print;
   colorDump = saved_colordump;
   leftExportPixelTrim = saved_left;
   topExportPixelTrim = saved_top;
   rightExportPixelTrim = saved_right;
   bottomExportPixelTrim = saved_bottom;
   if (*gszImageProcXPmFile == '\0') return FALSE;
   strcpy(pszPath, gszImageProcXPmFile);
   return TRUE;
}

void RegenerateImage()
{
   int image_w=0, image_h=0, w, h, short_name, rc;
   int ltx=0, lty=0, ppm6=FALSE;
   int ncolors=0, chars_per_pixel=0, first_pixel_is_bg=0, *pixels=NULL;
   char *color_char=NULL, **color_str=NULL, *xpm_data=NULL, *rest;
   char szPath[MAXPATHLENGTH+1];
   struct ObjRec *obj_ptr=NULL, *saved_top_obj=NULL, *saved_bot_obj=NULL;
   struct SelRec *top_sel_ptr=NULL, *bot_sel_ptr=NULL;
   Pixmap pixmap=None, bitmap=None;
   XImage *image=NULL, *bitmap_image=NULL;
   struct XPmRec *xpm_ptr=NULL;

   if (!CheckSelectionForImageProc(CMDID_REGENERATEIMAGE)) {
      return;
   }
   obj_ptr = topSel->obj;
   xpm_ptr = obj_ptr->detail.xpm;
   if (obj_ptr->ctm == NULL) {
      if (MsgBox(TgLoadString(STID_XPM_NOT_XFORMED_REGEN_ANYWAY), TOOL_NAME,
            YNC_MB) != MB_ID_YES) {
         return;
      }
   }
   if (DoPpm6(xpm_ptr)) {
      ppm6 = TRUE;
   } else if (xpm_ptr->real_type == XPM_XPM &&
         fullTrueColorMode && HasZlibSupport()) {
      ppm6 = TRUE;
   }
   ltx = obj_ptr->obbox.ltx;
   lty = obj_ptr->obbox.lty;

   HighLightReverse();
   PrepareToReplaceAnObj(obj_ptr);
   PushPageInfo();
   saved_top_obj = topObj;
   saved_bot_obj = botObj;

   JustDupSelObj(&top_sel_ptr, &bot_sel_ptr);
   curPage->top = topObj = top_sel_ptr->obj;
   curPage->bot = botObj = bot_sel_ptr->obj;
   CopyObjId(topSel->obj, topObj);
   CopyObjLocks(topSel->obj, topObj);

   rc = RegenerateImageFile(szPath, topObj->detail.xpm);

   DelAllObj();
   free(top_sel_ptr);
   PopPageInfo();
   curPage->top = topObj = saved_top_obj;
   curPage->bot = botObj = saved_bot_obj;
   RedrawAnArea(botObj, selLtX-GRID_ABS_SIZE(1), selLtY-GRID_ABS_SIZE(1),
         selRbX+GRID_ABS_SIZE(1), selRbY+GRID_ABS_SIZE(1));
   if (!rc) {
      HighLightForward();
      AbortPrepareCmd(CMD_REPLACE);
      return;
   }
   UnlinkObj(obj_ptr);
   FreeObj(obj_ptr);
   RemoveAllSel();

   if (ppm6) {
      char deflated_fname[MAXPATHLENGTH+1];
      struct XPmRec *xpm_ptr=NULL;

      ResetPngHeaderInfo(&gPngHeaderInfo);
      obj_ptr = CreatePpmTrueObjFromFile(szPath);
      if (obj_ptr != NULL &&
            MkTempFile(deflated_fname, sizeof(deflated_fname), tmpDir,
            TOOL_NAME) != NULL &&
            DeflateFile(szPath, deflated_fname)) {
         /* good */
      } else {
         FreeObj(obj_ptr);

         sprintf(gszMsgBox, TgLoadString(STID_CANNOT_IMPORT_GIVEN_PPM), szPath);
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
         unlink(szPath);
         AbortPrepareCmd(CMD_REPLACE);

         return;
      }
      xpm_ptr = obj_ptr->detail.xpm;
      xpm_ptr->real_type = PPM_TRUE;
      xpm_ptr->ppm_data_compress = PPM_DATA_DEFLATED;
      xpm_ptr->ppm_data = ReadFileIntoBuf(deflated_fname,
            &xpm_ptr->ppm_data_size);
      xpm_ptr->ppm_mask_data = NULL;
      xpm_ptr->ppm_mask_size = 0;
      unlink(deflated_fname);
   } else {
      SetWatchCursor(drawWindow);
      SetWatchCursor(mainWindow);
      rc = MyReadPixmapFile(szPath, &image_w, &image_h, &w, &h, &pixmap,
            &image, &bitmap, &bitmap_image, &ncolors, &chars_per_pixel,
            &first_pixel_is_bg, &color_char, &color_str, &pixels, &xpm_data);
      SetDefaultCursor(mainWindow);
      ShowCursor();

      if ((short_name=IsPrefix(bootDir, szPath, &rest))) ++rest;
      if (rc != BitmapSuccess) {
         AbortPrepareCmd(CMD_REPLACE);
         sprintf(gszMsgBox, TgLoadString(STID_CANNOT_IMPORT_XPM_FILE),
               (short_name ? rest : szPath));
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
         unlink(szPath);
         return;
      }
      obj_ptr = CreateXPmObj(image_w, image_h, w, h, pixmap, image, bitmap,
            bitmap_image, ncolors, chars_per_pixel, first_pixel_is_bg,
            color_char, color_str, pixels, xpm_data);
      if (obj_ptr == NULL) return;
   }
   unlink(szPath);
   AddObj(NULL, topObj, obj_ptr);
   MoveObj(obj_ptr, ltx-obj_ptr->obbox.ltx, lty-obj_ptr->obbox.lty);
   numRedrawBBox = 0;
   obj_ptr->tmp_parent = NULL;
   DrawObj(drawWindow, obj_ptr);
   SelectTopObj();
   RecordReplaceAnObj(topObj);
   SetFileModified(TRUE);
   justDupped = FALSE;
   if (!PRTGIF && colorLayers && needToRedrawColorWindow) {
      RedrawColorWindow();
   }
   sprintf(gszMsgBox, TgLoadString(STID_NEW_XPM_WH_GENERATED),
         image_w, image_h);
   Msg(gszMsgBox);
}

/* ----------------------- CreatePixmapFromSelected ----------------------- */

void CreatePixmapFromSelected()
{
   int image_w=0, image_h=0, w=0, h=0, short_name=FALSE, rc=0;
   int ltx=0, lty=0, ppm6=FALSE;
   int ncolors=0, chars_per_pixel=0, first_pixel_is_bg=0, *pixels=NULL;
   char *color_char=NULL, **color_str=NULL, *xpm_data=NULL, *rest;
   char szPath[MAXPATHLENGTH+1];
   struct ObjRec *obj_ptr=NULL, *saved_top_obj=NULL, *saved_bot_obj=NULL;
   struct SelRec *top_sel_ptr=NULL, *bot_sel_ptr=NULL, *sel_ptr=NULL;
   struct SelRec *next_sel=NULL;
   Pixmap pixmap=None, bitmap=None;
   XImage *image=NULL, *bitmap_image=NULL;

   if (topSel == NULL) {
      MsgBox(TgLoadCachedString(CSTID_NO_OBJ_SELECTED), TOOL_NAME, INFO_MB);
      return;
   }
   if (topSel == botSel && topSel->obj->type == OBJ_XPM) {
      RegenerateImage();
      return;
   }
   if (fullTrueColorMode && HasZlibSupport()) {
      ppm6 = TRUE;
   }
   ltx = selLtX;
   lty = selLtY;

   HighLightReverse();
   PushPageInfo();
   saved_top_obj = topObj;
   saved_bot_obj = botObj;

   JustDupSelObj(&top_sel_ptr, &bot_sel_ptr);
   curPage->top = topObj = top_sel_ptr->obj;
   curPage->bot = botObj = bot_sel_ptr->obj;
   for (sel_ptr=topSel, obj_ptr=topObj; obj_ptr!=NULL;
         sel_ptr=sel_ptr->next, obj_ptr=obj_ptr->next) {
      CopyObjId(sel_ptr->obj, obj_ptr);
      CopyObjLocks(sel_ptr->obj, obj_ptr);
   }
   rc = RegenerateImageFile(szPath, NULL);

   DelAllObj();
   for (sel_ptr=top_sel_ptr; sel_ptr != NULL; sel_ptr=next_sel) {
      next_sel = sel_ptr->next;
      free(sel_ptr);
   }
   PopPageInfo();
   curPage->top = topObj = saved_top_obj;
   curPage->bot = botObj = saved_bot_obj;
   RedrawAnArea(botObj, selLtX-GRID_ABS_SIZE(1), selLtY-GRID_ABS_SIZE(1),
         selRbX+GRID_ABS_SIZE(1), selRbY+GRID_ABS_SIZE(1));
   if (!rc) {
      HighLightForward();
      return;
   }
   RemoveAllSel();

   if (ppm6) {
      char deflated_fname[MAXPATHLENGTH+1];
      struct XPmRec *xpm_ptr=NULL;

      ResetPngHeaderInfo(&gPngHeaderInfo);
      obj_ptr = CreatePpmTrueObjFromFile(szPath);
      if (obj_ptr != NULL &&
            MkTempFile(deflated_fname, sizeof(deflated_fname), tmpDir,
            TOOL_NAME) != NULL &&
            DeflateFile(szPath, deflated_fname)) {
         /* good */
      } else {
         FreeObj(obj_ptr);

         sprintf(gszMsgBox, TgLoadString(STID_CANNOT_IMPORT_GIVEN_PPM), szPath);
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
         unlink(szPath);
         AbortPrepareCmd(CMD_REPLACE);

         return;
      }
      xpm_ptr = obj_ptr->detail.xpm;
      xpm_ptr->real_type = PPM_TRUE;
      xpm_ptr->ppm_data_compress = PPM_DATA_DEFLATED;
      xpm_ptr->ppm_data = ReadFileIntoBuf(deflated_fname,
            &xpm_ptr->ppm_data_size);
      xpm_ptr->ppm_mask_data = NULL;
      xpm_ptr->ppm_mask_size = 0;
      unlink(deflated_fname);
   } else {
      SetWatchCursor(drawWindow);
      SetWatchCursor(mainWindow);
      rc = MyReadPixmapFile(szPath, &image_w, &image_h, &w, &h, &pixmap,
            &image, &bitmap, &bitmap_image, &ncolors, &chars_per_pixel,
            &first_pixel_is_bg, &color_char, &color_str, &pixels, &xpm_data);
      SetDefaultCursor(mainWindow);
      ShowCursor();

      if ((short_name=IsPrefix(bootDir, szPath, &rest))) ++rest;
      if (rc != BitmapSuccess) {
         AbortPrepareCmd(CMD_REPLACE);
         sprintf(gszMsgBox, TgLoadString(STID_CANNOT_IMPORT_XPM_FILE),
               (short_name ? rest : szPath));
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
         unlink(szPath);
         return;
      }
      obj_ptr = CreateXPmObj(image_w, image_h, w, h, pixmap, image, bitmap,
            bitmap_image, ncolors, chars_per_pixel, first_pixel_is_bg,
            color_char, color_str, pixels, xpm_data);
      if (obj_ptr == NULL) return;
   }
   unlink(szPath);
   AddObj(NULL, topObj, obj_ptr);
   MoveObj(obj_ptr, ltx-obj_ptr->obbox.ltx, lty-obj_ptr->obbox.lty);
   numRedrawBBox = 0;
   obj_ptr->tmp_parent = NULL;
   DrawObj(drawWindow, obj_ptr);
   SelectTopObj();
   RecordNewObjCmd();
   SetFileModified(TRUE);
   justDupped = FALSE;
   if (!PRTGIF && colorLayers && needToRedrawColorWindow) {
      RedrawColorWindow();
   }
   sprintf(gszMsgBox, TgLoadString(STID_NEW_XPM_WH_GENERATED),
         image_w, image_h);
   Msg(gszMsgBox);
}

/* ----------------------- CropImage ----------------------- */

static
void ContinueCrop(nObjAbsLtX, nObjAbsLtY, nStartXOff, nStartYOff,
      pnEndXOff, pnEndYOff)
   int nObjAbsLtX, nObjAbsLtY, nStartXOff, nStartYOff, *pnEndXOff, *pnEndYOff;
{
   int end_x, end_y, cropping=TRUE;
   char buf[80], w_buf[80], h_buf[80], x_buf[80], y_buf[80];

   end_x = nStartXOff;
   end_y = nStartYOff;
   SelBox(drawWindow, revDefaultGC, nStartXOff, nStartYOff, end_x, end_y);
   PixelToMeasurementUnit(w_buf, 0);
   PixelToMeasurementUnit(h_buf, 0);
   PixelToMeasurementUnit(x_buf, ABS_X(end_x)-nObjAbsLtX);
   PixelToMeasurementUnit(y_buf, ABS_Y(end_y)-nObjAbsLtY);
   sprintf(buf, "w=%s\nh=%s\nx=%s\ny=%s", w_buf, h_buf, x_buf, y_buf);
   StartShowMeasureCursor(end_x, end_y, buf, TRUE);
   XGrabPointer(mainDisplay, drawWindow, False,
         PointerMotionMask | ButtonReleaseMask,
         GrabModeAsync, GrabModeAsync, None, handCursor, CurrentTime);
   
   while (cropping) {
      XEvent input;

      XNextEvent(mainDisplay, &input);

      if (input.type == Expose || input.type == VisibilityNotify) {
         ExposeEventHandler(&input, TRUE);
      } else if (input.type == ButtonRelease) {
         XUngrabPointer(mainDisplay, CurrentTime);
         PixelToMeasurementUnit(w_buf, ABS_SIZE(abs(end_x-nStartXOff)));
         PixelToMeasurementUnit(h_buf, ABS_SIZE(abs(end_y-nStartYOff)));
         PixelToMeasurementUnit(x_buf, ABS_X(end_x)-nObjAbsLtX);
         PixelToMeasurementUnit(y_buf, ABS_Y(end_y)-nObjAbsLtY);
         sprintf(buf, "w=%s\nh=%s\nx=%s\ny=%s", w_buf, h_buf, x_buf, y_buf);
         EndShowMeasureCursor(end_x, end_y, buf, TRUE);
         SelBox(drawWindow, revDefaultGC, nStartXOff, nStartYOff, end_x, end_y);
         cropping = FALSE;
      } else if (input.type == MotionNotify) {
         int new_end_x, new_end_y;
         XMotionEvent *motion_ev;
         XEvent ev;

         motion_ev = &(input.xmotion);
         new_end_x = motion_ev->x;
         new_end_y = motion_ev->y;

         PixelToMeasurementUnit(w_buf, ABS_SIZE(abs(end_x-nStartXOff)));
         PixelToMeasurementUnit(h_buf, ABS_SIZE(abs(end_y-nStartYOff)));
         PixelToMeasurementUnit(x_buf, ABS_X(end_x)-nObjAbsLtX);
         PixelToMeasurementUnit(y_buf, ABS_Y(end_y)-nObjAbsLtY);
         sprintf(buf, "w=%s\nh=%s\nx=%s\ny=%s", w_buf, h_buf, x_buf, y_buf);
         ShowMeasureCursor(end_x, end_y, buf, TRUE);
         SelBox(drawWindow, revDefaultGC, nStartXOff, nStartYOff, end_x, end_y);
         end_x = new_end_x; end_y = new_end_y;
         SelBox(drawWindow, revDefaultGC, nStartXOff, nStartYOff, end_x, end_y);
         PixelToMeasurementUnit(w_buf, ABS_SIZE(abs(end_x-nStartXOff)));
         PixelToMeasurementUnit(h_buf, ABS_SIZE(abs(end_y-nStartYOff)));
         PixelToMeasurementUnit(x_buf, ABS_X(end_x)-nObjAbsLtX);
         PixelToMeasurementUnit(y_buf, ABS_Y(end_y)-nObjAbsLtY);
         sprintf(buf, "w=%s\nh=%s\nx=%s\ny=%s", w_buf, h_buf, x_buf, y_buf);
         ShowMeasureCursor(end_x, end_y, buf, TRUE);

         MarkRulers(end_x, end_y);
         while (XCheckMaskEvent(mainDisplay, PointerMotionMask, &ev)) ;
      }
   }
   *pnEndXOff = end_x;
   *pnEndYOff = end_y;
}

static
int GetCropArea(obj_abs_ltx, obj_abs_lty, pbbox)
   int obj_abs_ltx, obj_abs_lty;
   struct BBRec *pbbox;
{
   unsigned int button;
   int mouse_x=0, mouse_y=0, rc=TRUE;

   SaveStatusStrings();
   Msg(TgLoadString(STID_DRAG_A_RECT_TO_CROP));
   SetMouseStatus(TgLoadString(STID_START_CROP_IMAGE),
         TgLoadString(STID_ABORT_CROP_IMAGE),
         TgLoadString(STID_ABORT_CROP_IMAGE));
   button = DrawWindowLoop(&mouse_x, &mouse_y, cornerCursor, FALSE);
   if (button == Button1) {
      int end_x=mouse_x, end_y=mouse_y;

      ContinueCrop(obj_abs_ltx, obj_abs_lty, mouse_x, mouse_y, &end_x, &end_y);
      if (pbbox != NULL) {
         CalcBBox(mouse_x, mouse_y, end_x, end_y,
               &pbbox->ltx, &pbbox->lty, &pbbox->rbx, &pbbox->rby);
      }
   } else {
      rc = FALSE;
   }
   Msg("");
   RestoreStatusStrings();
   return rc;
}

void CropImage()
{
   int image_w=0, image_h=0, w, h, short_name, rc, ppm6=FALSE;
   int ltx=0, lty=0, crop_x=0, crop_y=0, crop_w=0, crop_h=0;
   int ncolors=0, chars_per_pixel=0, first_pixel_is_bg=0, *pixels=NULL;
   int saved_ltx=selLtX, saved_lty=selLtY, saved_rbx=selRbX, saved_rby=selRbY;
   char *color_char=NULL, **color_str=NULL, *xpm_data=NULL, *rest;
   char szPath[MAXPATHLENGTH+1];
   struct ObjRec *obj_ptr=NULL, *saved_top_obj=NULL, *saved_bot_obj=NULL;
   struct SelRec *top_sel_ptr=NULL, *bot_sel_ptr=NULL;
   Pixmap pixmap=None, bitmap=None;
   XImage *image=NULL, *bitmap_image=NULL;
   struct AttrRec *saved_fattr=NULL, *saved_lattr=NULL;
   struct BBRec crop_bbox;

   if (!CheckSelectionForImageProc(CMDID_CROPIMAGE)) {
      return;
   }
   obj_ptr = topSel->obj;
   if (DoPpm6(obj_ptr->detail.xpm)) {
      ppm6 = TRUE;
   }
   ltx = obj_ptr->obbox.ltx;
   lty = obj_ptr->obbox.lty;

   HighLightReverse();
   XSync(mainDisplay, False);
   if (!GetCropArea(ltx, lty, &crop_bbox)) {
      return;
   }
   if (somethingHighLighted) {
      HighLightReverse();
   }
   XSync(mainDisplay, False);
   crop_bbox.ltx = ABS_X(crop_bbox.ltx); crop_bbox.lty = ABS_Y(crop_bbox.lty);
   crop_bbox.rbx = ABS_X(crop_bbox.rbx); crop_bbox.rby = ABS_Y(crop_bbox.rby);
   if (!BBoxIntersect(crop_bbox, obj_ptr->obbox)) {
      HighLightForward();
      MsgBox(TgLoadString(STID_SEL_AREA_NOT_INTERSECT_IMAGE), TOOL_NAME,
            INFO_MB);
      return;
   } else {
      crop_bbox.ltx = max(crop_bbox.ltx, obj_ptr->obbox.ltx);
      crop_bbox.lty = max(crop_bbox.lty, obj_ptr->obbox.lty);
      crop_bbox.rbx = min(crop_bbox.rbx, obj_ptr->obbox.rbx);
      crop_bbox.rby = min(crop_bbox.rby, obj_ptr->obbox.rby);
      crop_x = crop_bbox.ltx-obj_ptr->obbox.ltx;
      crop_y = crop_bbox.lty-obj_ptr->obbox.lty;
      crop_w = crop_bbox.rbx-crop_bbox.ltx;
      crop_h = crop_bbox.rby-crop_bbox.lty;
      if (crop_x == 0 && crop_y == 0 &&
            crop_w == (obj_ptr->obbox.rbx-obj_ptr->obbox.ltx) &&
            crop_h == (obj_ptr->obbox.rby-obj_ptr->obbox.lty)) {
         HighLightForward();
         return;
      }
      if (crop_w == 0 || crop_h == 0) {
         MsgBox(TgLoadString(STID_XPM_CANT_HAVE_0_W_OR_H), TOOL_NAME, INFO_MB);
         HighLightForward();
         return;
      }
   }
   if (obj_ptr->ctm == NULL) {
      HighLightForward();
      CutXPixmap(NULL, &crop_x, &crop_y, &crop_w, &crop_h);
      return;
   }
   PrepareToReplaceAnObj(obj_ptr);
   PushPageInfo();
   saved_top_obj = topObj;
   saved_bot_obj = botObj;

   JustDupSelObj(&top_sel_ptr, &bot_sel_ptr);
   curPage->top = topObj = top_sel_ptr->obj;
   curPage->bot = botObj = bot_sel_ptr->obj;
   CopyObjId(topSel->obj, topObj);
   CopyObjLocks(topSel->obj, topObj);

   rc = RegenerateImageFile(szPath, topObj->detail.xpm);

   DelAllObj();
   free(top_sel_ptr);
   PopPageInfo();
   curPage->top = topObj = saved_top_obj;
   curPage->bot = botObj = saved_bot_obj;
   RedrawAnArea(botObj, saved_ltx-GRID_ABS_SIZE(1),
         saved_lty-GRID_ABS_SIZE(1), saved_rbx+GRID_ABS_SIZE(1),
         saved_rby+GRID_ABS_SIZE(1));
   if (!rc) {
      HighLightForward();
      AbortPrepareCmd(CMD_REPLACE);
      return;
   }
   if (ppm6) {
      char deflated_fname[MAXPATHLENGTH+1];
      struct XPmRec *xpm_ptr=NULL;
      struct ObjRec *tmp_obj_ptr=NULL;

      ResetPngHeaderInfo(&gPngHeaderInfo);
      tmp_obj_ptr = CreatePpmTrueObjFromFile(szPath);
      if (tmp_obj_ptr != NULL &&
            MkTempFile(deflated_fname, sizeof(deflated_fname), tmpDir,
            TOOL_NAME) != NULL &&
            DeflateFile(szPath, deflated_fname)) {
         /* good */
      } else {
         FreeObj(tmp_obj_ptr);

         sprintf(gszMsgBox, TgLoadString(STID_CANNOT_IMPORT_GIVEN_PPM), szPath);
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
         unlink(szPath);
         AbortPrepareCmd(CMD_REPLACE);

         return;
      }
      xpm_ptr = tmp_obj_ptr->detail.xpm;
      xpm_ptr->real_type = PPM_TRUE;
      xpm_ptr->ppm_data_compress = PPM_DATA_DEFLATED;
      xpm_ptr->ppm_data = ReadFileIntoBuf(deflated_fname,
            &xpm_ptr->ppm_data_size);
      xpm_ptr->ppm_mask_data = NULL;
      xpm_ptr->ppm_mask_size = 0;
      unlink(deflated_fname);

      if (tmp_obj_ptr == NULL) {
         return;
      } else {
         struct ObjRec *new_obj_ptr=CutXPixmap(tmp_obj_ptr, &crop_x, &crop_y,
               &crop_w, &crop_h);

         if (new_obj_ptr == NULL || new_obj_ptr == tmp_obj_ptr) {
            FreeObj(tmp_obj_ptr);
            HighLightForward();
            AbortPrepareCmd(CMD_REPLACE);
            return;
         }
         FreeObj(tmp_obj_ptr);

         UnlinkObj(obj_ptr);
         FreeObj(obj_ptr);
         RemoveAllSel();
         obj_ptr = new_obj_ptr;
      }
      /* drop through */
   } else {
      SetWatchCursor(drawWindow);
      SetWatchCursor(mainWindow);
      rc = MyReadPixmapFile(szPath, &image_w, &image_h, &w, &h, &pixmap,
            &image, &bitmap, &bitmap_image, &ncolors, &chars_per_pixel,
            &first_pixel_is_bg, &color_char, &color_str, &pixels, &xpm_data);
      SetDefaultCursor(mainWindow);
      ShowCursor();
      if (rc == BitmapSuccess) {
         Pixmap dest_pixmap=None, dest_bitmap=None;
         XImage *dest_image=NULL, *dest_bitmap_image=NULL;
         int ok;

         ok = ExtractPixmap(pixmap, image, bitmap,
               bitmap_image, crop_x, crop_y, crop_w, crop_h,
               &dest_pixmap, &dest_image, &dest_bitmap, &dest_bitmap_image);
         if (ok) {
            XFreePixmap(mainDisplay, pixmap);
            XFreePixmap(mainDisplay, bitmap);
            XDestroyImage(image);
            XDestroyImage(bitmap_image);
            pixmap = dest_pixmap;
            bitmap = dest_bitmap;
            image = dest_image;
            bitmap_image = dest_bitmap_image;
         } else {
            rc = BitmapFileInvalid;
         }
      }
      if (rc == BitmapSuccess) {
         saved_fattr = obj_ptr->fattr;
         saved_lattr = obj_ptr->lattr;
         obj_ptr->fattr = obj_ptr->lattr = NULL;
         UnlinkObj(obj_ptr);
         FreeObj(obj_ptr);
         RemoveAllSel();
      }
      if ((short_name=IsPrefix(bootDir, szPath, &rest))) ++rest;
      if (rc != BitmapSuccess) {
         AbortPrepareCmd(CMD_REPLACE);
         sprintf(gszMsgBox, TgLoadString(STID_CANNOT_IMPORT_XPM_FILE),
               (short_name ? rest : szPath));
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
         unlink(szPath);
         return;
      }
      obj_ptr = CreateXPmObj(crop_w, crop_h, crop_w, crop_h, pixmap, image,
            bitmap, bitmap_image, ncolors, chars_per_pixel, first_pixel_is_bg,
            color_char, color_str, pixels, xpm_data);
   }
   unlink(szPath);
   AddObj(NULL, topObj, obj_ptr);
   MoveObj(obj_ptr, ltx-obj_ptr->obbox.ltx+crop_x,
         lty-obj_ptr->obbox.lty+crop_y);
   if (saved_fattr != NULL) {
      obj_ptr->fattr = saved_fattr;
      obj_ptr->lattr = saved_lattr;
   }
   RecordReplaceAnObj(topObj);
   RedrawAnArea(botObj, saved_ltx-GRID_ABS_SIZE(1), saved_lty-GRID_ABS_SIZE(1),
            saved_rbx+GRID_ABS_SIZE(1), saved_rby+GRID_ABS_SIZE(1));
   SelectTopObj();
   SetFileModified(TRUE);
   justDupped = FALSE;
   if (!PRTGIF && colorLayers && needToRedrawColorWindow) {
      RedrawColorWindow();
   }
   sprintf(gszMsgBox, TgLoadString(STID_NEW_XPM_WH_GENERATED),
         image_w, image_h);
   Msg(gszMsgBox);
}

/* ----------------------- GetColor ----------------------- */

static
void SetCurrentColor(xpm_ptr, image, bitmap_image, image_x, image_y)
   struct XPmRec *xpm_ptr;
   XImage *image, *bitmap_image;
   int image_x, image_y;
{
   int pixel=(-1);

   if (bitmap_image == NULL) {
      pixel = XGetPixel(image, image_x, image_y);
   } else {
      if (XGetPixel(bitmap_image, image_x, image_y) == 0) {
         /* transparent */
      } else {
         pixel = XGetPixel(image, image_x, image_y);
      }
   }
   if (pixel == (-1)) {
   } else if (DoPpm6(xpm_ptr)) {
      uint32_t pix=(uint32_t)(unsigned int)pixel;
      unsigned int r=0, g=0, b=0;
      double dr=(double)0, dg=(double)0, db=(double)0;
      char color_s[40];
      int color_index=0, new_alloc=FALSE;

      r = (pix & gTrueColorInfo.r_mask) >> gTrueColorInfo.r_shift;
      g = (pix & gTrueColorInfo.g_mask) >> gTrueColorInfo.g_shift;
      b = (pix & gTrueColorInfo.b_mask) >> gTrueColorInfo.b_shift;
      dr = ((double)r) / gTrueColorInfo.dr_maxval * ((double)256);
      dg = ((double)g) / gTrueColorInfo.dg_maxval * ((double)256);
      db = ((double)b) / gTrueColorInfo.db_maxval * ((double)256);
      r = (dr < ((double)0)) ? 0 : ((unsigned int)dr);
      g = (dg < ((double)0)) ? 0 : ((unsigned int)dg);
      b = (db < ((double)0)) ? 0 : ((unsigned int)db);
      if (r > 255) r = 255;
      if (g > 255) g = 255;
      if (b > 255) b = 255;
      sprintf(color_s, "#%02x%02x%02x", r, g, b);
      color_index = QuickFindColorIndex(NULL, color_s, &new_alloc, FALSE);
      if (color_index != INVALID) {
         ChangeAllSelColor(color_index, FALSE);
      }
   } else {
      int i;

      for (i=0; i < maxColors; i++) {
         if (colorPixels[i] == pixel) {
            struct SelRec *saved_top_sel=topSel, *saved_bot_sel=botSel;

            topSel = botSel = NULL;
            ChangeAllSelColor(i, FALSE);
            topSel = saved_top_sel;
            botSel = saved_bot_sel;
            break;
         }
      }
   }
}

static
void DoGetColor(obj_ptr)
   struct ObjRec *obj_ptr;
{
   int image_w, image_h, done=FALSE;
   struct XPmRec *xpm_ptr=obj_ptr->detail.xpm;
   XImage *image, *bitmap_image;

   image_w = xpm_ptr->image_w;
   image_h = xpm_ptr->image_h;
   image = xpm_ptr->image;
   bitmap_image = xpm_ptr->bitmap_image;
   if (image == NULL) {
      image = xpm_ptr->image = XGetImage(mainDisplay, xpm_ptr->pixmap, 0, 0,
            image_w, image_h, AllPlanes, ZPixmap);
      if (image == NULL) FailAllocMessage();
   }
   if (xpm_ptr->bitmap != None && bitmap_image == NULL) {
      bitmap_image = xpm_ptr->bitmap_image = XGetImage(mainDisplay,
            xpm_ptr->bitmap, 0, 0, image_w, image_h, AllPlanes, ZPixmap);
      if (bitmap_image == NULL) FailAllocMessage();
   }
   if (DoPpm6(xpm_ptr)) {
      if (!InitTrueColorInfo(image, &gTrueColorInfo, image_w)) {
         sprintf(gszMsgBox, TgLoadString(STID_CANNOT_GET_IMAGE_OF_SIZE),
               image_w, image_h);
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
         return;
      }
   }
   SaveStatusStrings();
   Msg(TgLoadString(STID_SEL_A_COLOR_TO_USE_AS_CUR));
   SetMouseStatus(TgLoadString(STID_SELECT_A_COLOR),
         TgLoadCachedString(CSTID_FINISH), TgLoadCachedString(CSTID_FINISH));
   while (!done) {
      int mouse_x=0, mouse_y=0;
      unsigned int button=PickAPoint(&mouse_x, &mouse_y, dripCursor);

      if (button == Button1) {
         int abs_x=ABS_X(mouse_x), abs_y=ABS_Y(mouse_y), found=FALSE;
         int image_x=0, image_y=0;

         if (obj_ptr->ctm == NULL) {
            if (abs_x >= obj_ptr->obbox.ltx && abs_y >= obj_ptr->obbox.lty &&
                  abs_x < obj_ptr->obbox.rbx && abs_y < obj_ptr->obbox.rby) {
               image_x = abs_x-obj_ptr->obbox.ltx;
               image_y = abs_y-obj_ptr->obbox.lty;
               found = TRUE;
            } else {
               SetStringStatus(TgLoadString(STID_SEL_PT_NOT_ON_IMAGE));
            }
         } else {
            struct XPmRec *xpm_ptr=obj_ptr->detail.xpm;
            double tmp_dx=(double)0, tmp_dy=(double)0;

            ReverseTransformDoublePointThroughCTM(
                  ((double)(abs_x-obj_ptr->x)+0.5),
                  ((double)(abs_y-obj_ptr->y)+0.5),
                  obj_ptr->ctm, &tmp_dx, &tmp_dy);
            tmp_dx += (double)(obj_ptr->x-obj_ptr->orig_obbox.ltx);
            tmp_dy += (double)(obj_ptr->y-obj_ptr->orig_obbox.lty);
            if (tmp_dx >= ((double)0) && tmp_dx < ((double)xpm_ptr->image_w) &&
                  tmp_dy >= ((double)0) &&
                  tmp_dy < ((double)xpm_ptr->image_h)) {
               int tmp_x=(int)tmp_dx, tmp_y=(int)tmp_dy;

               if (tmp_x < 0) tmp_x = 0;
               if (tmp_x >= xpm_ptr->image_w) tmp_x = xpm_ptr->image_w-1;
               if (tmp_y < 0) tmp_y = 0;
               if (tmp_y >= xpm_ptr->image_h) tmp_y = xpm_ptr->image_h-1;
               image_x = tmp_x;
               image_y = tmp_y;
               found = TRUE;
            } else {
               SetStringStatus(TgLoadString(STID_SEL_PT_NOT_ON_IMAGE));
            }
         }
         if (found) {
            SetCurrentColor(xpm_ptr, image, bitmap_image, image_x, image_y);
         }
      } else {
         done = TRUE;
      }
   }
   RestoreStatusStrings();
}

void GetColor()
{
   int i, pixel;
   char szBuf[MAXSTRING+1];
   struct ObjRec *obj_ptr;

   strcpy(szBuf, GetImageProcName(CMDID_GETCOLOR));
   if (!(curChoice == NOTHING || curChoice == ROTATEMODE) || topSel == NULL ||
         topSel != botSel) {
      sprintf(gszMsgBox, TgLoadString(STID_ONE_PRIM_FOR_IMAGEPROC_CMD), szBuf);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return;
   }
   obj_ptr = topSel->obj;
   switch (obj_ptr->type) {
   case OBJ_GROUP:
   case OBJ_ICON:
   case OBJ_SYM:
   case OBJ_PIN:
      sprintf(gszMsgBox, TgLoadString(STID_ONE_PRIM_FOR_IMAGEPROC_CMD), szBuf);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      break;
   case OBJ_XPM:
      HighLightReverse();
      XSync(mainDisplay, False);
      DoGetColor(obj_ptr);
      if (!somethingHighLighted) HighLightForward();
      break;
   default:
      pixel = colorPixels[obj_ptr->color];
      for (i=0; i < maxColors; i++) {
         if (colorPixels[i] == pixel) {
            struct SelRec *saved_top_sel=topSel, *saved_bot_sel=botSel;

            topSel = botSel = NULL;
            ChangeAllSelColor(i, FALSE);
            topSel = saved_top_sel;
            botSel = saved_bot_sel;
            break;
         }
      }
      break;
   }
}

/* ----------------------- ReplaceColor ----------------------- */

typedef struct tagReplaceColorInfo {
   int cmdid;
   int has_pixel_for_trans;
   int pixel_for_trans;
   int index_for_trans;
   struct XPmRec *xpm_ptr;
   TrueColorInfo tci;
} ReplaceColorInfo;

static ReplaceColorInfo gReplaceColorInfo;

static
unsigned int FillReplacePickAPoint(OrigX, OrigY, EndX, EndY, AllowDrag, cursor)
   int *OrigX, *OrigY, *EndX, *EndY, AllowDrag;
   /* if AllowDrag is TRUE, EndX and EndY must not be NULL */
   Cursor cursor;
{
   unsigned int button=(unsigned int)(-1);
   int dragging=FALSE;
   XEvent input;

   if (!debugNoPointerGrab) {
      XGrabPointer(mainDisplay, drawWindow, False,
            PointerMotionMask | ButtonPressMask | ButtonReleaseMask,
            GrabModeAsync, GrabModeAsync, None, cursor, CurrentTime);
   }
   for (;;) {
      XNextEvent(mainDisplay, &input);

      if (TgWindowIsPinnedMenu(input.xany.window, MENU_COLOR)) {
         SaveStatusStrings();
         TgHandlePinnedMenuEvent(input.xany.window, MENU_COLOR, &input);
         RestoreStatusStrings();
      } else if (TgIsCmdEvent(&input)) {
         TgHandleCmdEvent(&input);
      } else if (input.type == Expose || input.type == VisibilityNotify) {
         ExposeEventHandler(&input, TRUE);
      } else if (input.type == ButtonPress) {
         button = input.xbutton.button;
         *OrigX = input.xbutton.x;
         *OrigY = input.xbutton.y;
         if (AllowDrag && button == Button3) {
            *EndX = *OrigX;
            *EndY = *OrigY;
            SelBox(drawWindow, revDefaultGC, *OrigX, *OrigY, *EndX, *EndY);
            dragging = TRUE;
            SaveStatusStrings();
            SetStringStatus(TgLoadString(STID_DARG_TO_FILL_AN_AREA));
         } else {
            XUngrabPointer(mainDisplay, CurrentTime);
            XSync(mainDisplay, False);
            return button;
         }
      } else if (AllowDrag && dragging && input.type == ButtonRelease) {
         SelBox(drawWindow, revDefaultGC, *OrigX, *OrigY, *EndX, *EndY);
         XUngrabPointer(mainDisplay, CurrentTime);
         XSync(mainDisplay, False);
         *EndX = input.xbutton.x;
         *EndY = input.xbutton.y;
         RestoreStatusStrings();
         XSync(mainDisplay, False);
         return button;
      } else if (AllowDrag && dragging && input.type == MotionNotify) {
         SelBox(drawWindow, revDefaultGC, *OrigX, *OrigY, *EndX, *EndY);
         *EndX = input.xmotion.x;
         *EndY = input.xmotion.y;
         SelBox(drawWindow, revDefaultGC, *OrigX, *OrigY, *EndX, *EndY);
      } else if (input.type == KeyPress) {
         if (KeyPressEventIsEscape(&input.xkey)) {
            if (AllowDrag && dragging) {
               SelBox(drawWindow, revDefaultGC, *OrigX, *OrigY, *EndX, *EndY);
               RestoreStatusStrings();
            }
            XUngrabPointer(mainDisplay, CurrentTime);
            XSync(mainDisplay, False);
            return (unsigned int)(-1);
         }
      }
   }
}

static
void DoReplaceAColor(obj_ptr, image, bitmap_image, image_x, image_y,
      image_w, image_h)
   struct ObjRec *obj_ptr;
   XImage *image, *bitmap_image;
   int image_x, image_y, image_w, image_h;
{
   struct XPmRec *xpm_ptr=obj_ptr->detail.xpm;
   int pixel=(-1);
   XColor pixel_color;

   memset(&pixel_color, 0, sizeof(XColor));
   if (bitmap_image == NULL) {
      pixel = XGetPixel(image, image_x, image_y);
   } else {
      if (XGetPixel(bitmap_image, image_x, image_y) == 0) {
         /* transparent */
      } else {
         pixel = XGetPixel(image, image_x, image_y);
      }
   }
   if (pixel != (-1) && gThreshFillReplaceInfo.use_thresholding) {
      if (!SetupThreshFillReplaceInfo(&gThreshFillReplaceInfo, pixel, &pixel_color)) {
         return;
      }
   }
   SetWatchCursor(drawWindow);
   SetWatchCursor(mainWindow);
   if (pixel == (-1)) {
   } else {
      int i, r, *pixels=xpm_ptr->pixels, ncolors=xpm_ptr->ncolors;
      char **color_str=xpm_ptr->color_str;

      if (gThreshFillReplaceInfo.use_thresholding) {
         if (fullTrueColorMode) {
            for (r=0; r < image_h; r++) {
               int c;

               for (c=0; c < image_w; c++) {
                  int pix=XGetPixel(image, c, r);

                  if (pix != (-1) && TrueColorPixelWithinRange(pix,
                        &gThreshFillReplaceInfo)) {
                     if (gReplaceColorInfo.cmdid == CMDID_REPLACECOLORWITHTRANS) {
                        XPutPixel(image, c, r, gReplaceColorInfo.pixel_for_trans);
                        XPutPixel(bitmap_image, c, r, 0);
                     } else {
                        XPutPixel(image, c, r, colorPixels[colorIndex]);
                     }
                  }
               }
            }
         } else {
            for (r=0; r < image_h; r++) {
               int c;

               for (c=0; c < image_w; c++) {
                  int pix=XGetPixel(image, c, r), global_color_index=(-1);

                  if (pix != (-1) && HashLookUpInt(
                        &gThreshFillReplaceInfo.hash_table, (char*)(&pix),
                        sizeof(int), &global_color_index)) {
                     if (global_color_index != (-1) &&
                           gThreshFillReplaceInfo.within_threshold[global_color_index]) {
                        if (gReplaceColorInfo.cmdid == CMDID_REPLACECOLORWITHTRANS) {
                           XPutPixel(image, c, r, colorPixels[gReplaceColorInfo.index_for_trans]);
                           XPutPixel(bitmap_image, c, r, 0);
                        } else {
                           XPutPixel(image, c, r, colorPixels[colorIndex]);
                        }
                     }
                  }
               }
            }
            if (gThreshFillReplaceInfo.within_threshold != NULL) {
               free(gThreshFillReplaceInfo.within_threshold);
               gThreshFillReplaceInfo.within_threshold = NULL;
            }
         }
      } else {
         if (fullTrueColorMode) {
            for (r=0; r < image_h; r++) {
               int c;

               for (c=0; c < image_w; c++) {
                  if (XGetPixel(image, c, r) == pixel) {
                     if (gReplaceColorInfo.cmdid == CMDID_REPLACECOLORWITHTRANS) {
                        XPutPixel(image, c, r, gReplaceColorInfo.pixel_for_trans);
                        XPutPixel(bitmap_image, c, r, 0);
                     } else {
                        XPutPixel(image, c, r, colorPixels[colorIndex]);
                     }
                  }
               }
            }
         } else {
            for (r=0; r < image_h; r++) {
               int c;

               for (c=0; c < image_w; c++) {
                  if (XGetPixel(image, c, r) == pixel) {
                     if (gReplaceColorInfo.cmdid == CMDID_REPLACECOLORWITHTRANS) {
                        XPutPixel(image, c, r, colorPixels[gReplaceColorInfo.index_for_trans]);
                        XPutPixel(bitmap_image, c, r, 0);
                     } else {
                        XPutPixel(image, c, r, colorPixels[colorIndex]);
                     }
                  }
               }
            }
         }
         for (i=0; i < ncolors; i++) {
            if (pixels[i] == pixel) {
               pixels[i] = colorPixels[colorIndex];
               if (color_str[i] != NULL) free(color_str[i]);
               color_str[i] = UtilStrDup(colorMenuItems[colorIndex]);
               if (color_str[i] == NULL) FailAllocMessage();
            }
         }
      }
      if (xpm_ptr->data != NULL) {
#ifdef _TGIF_DBG /* debug, do not translate */
         fprintf(stderr,
               "In ReplaceAColor(), unexpected xpm_ptr->data != NULL.\n");
#endif /* _TGIF_DBG */
      }
   }
   SetDefaultCursor(mainWindow);
   ShowCursor();

   XPutImage(mainDisplay, xpm_ptr->pixmap, xpmGC, image, 0, 0, 0, 0,
      image_w, image_h);
   if (bitmap_image != NULL) {
      XPutImage(mainDisplay, xpm_ptr->bitmap, xbmGC, bitmap_image, 0, 0, 0, 0,
         image_w, image_h);
   }
   if (xpm_ptr->cached_pixmap != None) {
      XFreePixmap(mainDisplay, xpm_ptr->cached_pixmap);
      xpm_ptr->cached_pixmap = None;
   }
   if (xpm_ptr->cached_bitmap != None) {
      XFreePixmap(mainDisplay, xpm_ptr->cached_bitmap);
      xpm_ptr->cached_bitmap = None;
   }
   if ((xpm_ptr->real_type == XPM_JPEG ||
         xpm_ptr->real_type == PPM_TRUE) &&
         fullTrueColorMode && HasZlibSupport()) {
      unsigned int ppm_data_size=0;
      char tmp_fname[MAXPATHLENGTH], ext[MAXPATHLENGTH];
      char *ppm_data=NULL;

      *tmp_fname = *ext = '\0';
      if (MkTempFile(tmp_fname, sizeof(tmp_fname), tmpDir,
            TOOL_NAME) == NULL) {
         /* print error message? */
         return;
      }
      if (!DumpXImageToFile(xpm_ptr->image, xpm_ptr->image_w,
            xpm_ptr->image_h, tmp_fname, ext)) {
         /* print error message? */
         return;
      }
      if (strcmp(ext, ".ppm.z") == 0) {
         char deflated_fname[MAXPATHLENGTH];

         snprintf(deflated_fname, sizeof(deflated_fname), "%s%s",
               tmp_fname, ext);
         ppm_data = ReadFileIntoBuf(deflated_fname, &ppm_data_size);
         unlink(deflated_fname);
      }
      if (ppm_data != NULL) {
         if (xpm_ptr->ppm_data != NULL) free(xpm_ptr->ppm_data);
         if (xpm_ptr->ppm_mask_data != NULL) {
            free(xpm_ptr->ppm_mask_data);
         }
         xpm_ptr->real_type = PPM_TRUE;
         xpm_ptr->ppm_data = ppm_data;
         xpm_ptr->ppm_data_size = ppm_data_size;
         xpm_ptr->ppm_mask_data = NULL;
         xpm_ptr->ppm_mask_size = 0;
         xpm_ptr->ppm_data_compress = PPM_DATA_DEFLATED;
      }
   }
   AdjObjCache(obj_ptr);
   RedrawAnArea(botObj, selLtX-GRID_ABS_SIZE(1), selLtY-GRID_ABS_SIZE(1),
         selRbX+GRID_ABS_SIZE(1), selRbY+GRID_ABS_SIZE(1));
   SetFileModified(TRUE);
   justDupped = FALSE;
}

static
int ContinueReplaceColor(obj_ptr)
   struct ObjRec *obj_ptr;
{
   int done=FALSE, image_w, image_h, changed=FALSE;
   struct XPmRec *xpm_ptr=obj_ptr->detail.xpm;
   XImage *image, *bitmap_image;

   xpm_ptr = obj_ptr->detail.xpm;
   image_w = xpm_ptr->image_w;
   image_h = xpm_ptr->image_h;
   image = xpm_ptr->image;
   bitmap_image = xpm_ptr->bitmap_image;
   if (image == NULL) {
      image = xpm_ptr->image = XGetImage(mainDisplay, xpm_ptr->pixmap, 0, 0,
            image_w, image_h, AllPlanes, ZPixmap);
      if (image == NULL) FailAllocMessage();
   }
   if (xpm_ptr->bitmap != None && bitmap_image == NULL) {
      bitmap_image = xpm_ptr->bitmap_image = XGetImage(mainDisplay,
            xpm_ptr->bitmap, 0, 0, image_w, image_h, AllPlanes, ZPixmap);
      if (bitmap_image == NULL) FailAllocMessage();
   }
   SaveStatusStrings();
   Msg(TgLoadString(STID_SEL_A_COLOR_TO_BE_REPLACED));
   SetMouseStatus(TgLoadString(STID_SEL_A_COLOR_TO_REPLACE),
         TgLoadCachedString(CSTID_FINISH), TgLoadCachedString(CSTID_FINISH));
   while (!done) {
      int mouse_x=0, mouse_y=0;
      unsigned int button=FillReplacePickAPoint(&mouse_x, &mouse_y, NULL, NULL,
            FALSE, floodCursor);

      if (button == Button1) {
         int abs_x=ABS_X(mouse_x), abs_y=ABS_Y(mouse_y), found=FALSE;
         int image_x=0, image_y=0;

         if (obj_ptr->ctm == NULL) {
            if (abs_x >= obj_ptr->obbox.ltx && abs_y >= obj_ptr->obbox.lty &&
                  abs_x < obj_ptr->obbox.rbx && abs_y < obj_ptr->obbox.rby) {
               image_x = abs_x-obj_ptr->obbox.ltx;
               image_y = abs_y-obj_ptr->obbox.lty;
               if (image_x >= 0 && image_y >= 0 &&
                     image_x < image_w && image_y < image_h) {
                  found = TRUE;
               }
            }
         } else {
            double image_dx=(double)0, image_dy=(double)0;

            ReverseTransformDoublePointThroughCTM(
                  ((double)(abs_x-obj_ptr->x)+0.5),
                  ((double)(abs_y-obj_ptr->y)+0.5),
                  obj_ptr->ctm, &image_dx, &image_dy);
            image_dx += (double)(obj_ptr->x-obj_ptr->orig_obbox.ltx);
            image_dy += (double)(obj_ptr->y-obj_ptr->orig_obbox.lty);
            if (image_dx >= ((double)0) && image_dy >= ((double)0) &&
                  image_dx < ((double)image_w) &&
                  image_dy < ((double)image_h)) {
               image_x = (int)image_dx;
               image_y = (int)image_dy;
               if (image_x < 0) image_x = 0;
               if (image_x >= image_w) image_x = image_w-1;
               if (image_y < 0) image_y = 0;
               if (image_y >= image_h) image_y = image_h-1;
               found = TRUE;
            }
         }
         if (found) {
            changed = TRUE;
            if (somethingHighLighted) HighLightReverse();
            DoReplaceAColor(obj_ptr, image, bitmap_image, image_x, image_y,
                  image_w, image_h);
            if (!somethingHighLighted) HighLightForward();
         } else {
            SetStringStatus(TgLoadString(STID_SEL_PT_NOT_ON_IMAGE));
         }
      } else {
         done = TRUE;
      }
   }
   RestoreStatusStrings();
   return changed;
}

void ReplaceColor()
{
   struct ObjRec *obj_ptr=NULL;
   unsigned char trans_color_r='\0', trans_color_g='\0', trans_color_b='\0';

   if (!CheckSelectionForImageProc(CMDID_REPLACECOLOR)) {
      return;
   }
   obj_ptr = topSel->obj;
   if (ObjHasTrueColorTransPixel(obj_ptr, &trans_color_r, &trans_color_g,
         &trans_color_b)) {
      if (CurColorIsTranscolor(trans_color_r, trans_color_g, trans_color_b)) {
         sprintf(gszMsgBox, TgLoadString(STID_CUR_COLOR_IS_TRANS_PIXEL),
               colorMenuItems[colorIndex],
               GetImageProcName(CMDID_REPLACECOLOR));
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
         return;
      }
   }
   memset(&gReplaceColorInfo, 0, sizeof(ReplaceColorInfo));
   gReplaceColorInfo.cmdid = CMDID_REPLACECOLOR;

   if (somethingHighLighted) HighLightReverse();
   XSync(mainDisplay, False);
   PrepareToReplaceAnObj(obj_ptr);

   ResetThreshFillReplaceInfo();
   if (!ContinueReplaceColor(obj_ptr)) {
      AbortPrepareCmd(CMD_REPLACE);
   } else {
      RecordReplaceAnObj(obj_ptr);
   }
   if (!somethingHighLighted) HighLightForward();
}

/* ----------------------- ReplaceColorWithTrans ----------------------- */

static
int SetupReplaceColorInfo(obj_ptr, prci)
   struct ObjRec *obj_ptr;
   ReplaceColorInfo *prci;
{
   struct XPmRec *xpm_ptr=obj_ptr->detail.xpm;
   int i=0, ncolors=xpm_ptr->ncolors, index=(-1);
   int image_w=xpm_ptr->image_w, image_h=xpm_ptr->image_h;
   unsigned char trans_color_r='\0', trans_color_g='\0', trans_color_b='\0';
   XImage *image=NULL;
   TrueColorInfo *ptci=(&prci->tci);

   prci->xpm_ptr = xpm_ptr;

   switch (xpm_ptr->real_type) {
   case XPM_XPM:
      if (!ObjHasIndexedTransPixel(obj_ptr, &index)) {
         int chars_per_pixel=xpm_ptr->chars_per_pixel;

         xpm_ptr->pixels = (int*)realloc(xpm_ptr->pixels, sizeof(int)*(ncolors+1));
         if (xpm_ptr->pixels == NULL) FailAllocMessage();
         xpm_ptr->pixels[ncolors] = (-1);

         xpm_ptr->color_str = (char**)realloc(xpm_ptr->color_str, sizeof(char*)*(ncolors+1));
         if (xpm_ptr->color_str == NULL) FailAllocMessage();
         xpm_ptr->color_str[ncolors] = UtilStrDup("None");
         if (xpm_ptr->color_str[ncolors] == NULL) FailAllocMessage();

         if (chars_per_pixel == 1 && ncolors >= 20) {
            /* needs to go from 1 chars_per_pixel to 2 chars_per_pixel */
            char *color_char=(char*)malloc(((ncolors+1)<<1)*sizeof(char));

            if (color_char == NULL) FailAllocMessage();
            for (i=0; i < ncolors+1; i++) {
               if (i == 0 && xpm_ptr->color_char[0] == '`') {
                  color_char[i<<1] = color_char[(i<<1)+1] = '`';
               } else {
                  color_char[i<<1] = (char)(((int)('a'))+(int)(i/10));
                  color_char[(i<<1)+1] = (char)(((int)('0'))+(int)(i%10));
               }
            }
            free(xpm_ptr->color_char);
            xpm_ptr->color_char = color_char;
            xpm_ptr->chars_per_pixel = 2;
         } else {
            char *color_char=NULL;

            xpm_ptr->color_char = color_char =
                  (char*)realloc(xpm_ptr->color_char,
                  sizeof(char)*chars_per_pixel*(ncolors+1));
            if (color_char == NULL) FailAllocMessage();
            if (chars_per_pixel == 1) {
               for (i=0; i < ncolors+1; i++) {
                  if (i == 0 && xpm_ptr->color_char[0] == '`') {
                     color_char[i] = '`';
                  } else {
                     color_char[i] = (char)(((int)('a'))+i-1);
                  }
               }
            } else {
               for (i=0; i < ncolors+1; i++) {
                  if (i == 0 && xpm_ptr->color_char[0] == '`' &&
                        xpm_ptr->color_char[1] == '`') {
                     color_char[i<<1] = color_char[(i<<1)+1] = '`';
                  } else {
                     color_char[i<<1] = (char)(((int)('a'))+(int)(i/10));
                     color_char[(i<<1)+1] = (char)(((int)('0'))+(int)(i%10));
                  }
               }
            }
         }
         xpm_ptr->ncolors++;

         index = ncolors;
      }
      prci->index_for_trans = index;
      prci->has_pixel_for_trans = TRUE;
      break;
   case XPM_JPEG: break;
   case PPM_TRUE:
      image = XGetImage(mainDisplay, xpm_ptr->pixmap, 0, 0, image_w, image_h, AllPlanes, ZPixmap);

      if (image == NULL || !InitTrueColorInfo(image, &prci->tci, image_w)) {
         sprintf(gszMsgBox, TgLoadString(STID_CANNOT_GET_IMAGE_OF_SIZE), image_w, image_h);
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
         XDestroyImage(image);
         return FALSE;
      }
      if (ObjHasTrueColorTransPixel(obj_ptr, &trans_color_r, &trans_color_g, &trans_color_b)) {
         unsigned int r=(unsigned int)trans_color_r;
         unsigned int g=(unsigned int)trans_color_g;
         unsigned int b=(unsigned int)trans_color_b;

         prci->pixel_for_trans = ((r << ptci->r_shift) & mainVisual->red_mask) |
                 ((g << ptci->g_shift) & mainVisual->green_mask) |
                 ((b << ptci->b_shift) & mainVisual->blue_mask) ;
      } else {
         unsigned char has_r[256], has_g[256], has_b[256];
         unsigned char can_have_r[256], can_have_g[256], can_have_b[256];
         int row=0, found=FALSE;
         XImage *image=XGetImage(mainDisplay, xpm_ptr->pixmap, 0, 0, image_w, image_h, AllPlanes, ZPixmap);
         unsigned int r_maxval=((ptci->r_mask)>>ptci->r_shift);
         unsigned int g_maxval=((ptci->g_mask)>>ptci->g_shift);
         unsigned int b_maxval=((ptci->b_mask)>>ptci->b_shift);

         memset(has_r, 0, sizeof(256*sizeof(unsigned char)));
         memset(has_g, 0, sizeof(256*sizeof(unsigned char)));
         memset(has_b, 0, sizeof(256*sizeof(unsigned char)));
         memset(can_have_r, 0, sizeof(256*sizeof(unsigned char)));
         memset(can_have_g, 0, sizeof(256*sizeof(unsigned char)));
         memset(can_have_b, 0, sizeof(256*sizeof(unsigned char)));
         for (i=0; i < r_maxval; i++) {
            double dval=((double)i)/ptci->dr_maxval_div255;
            unsigned int uval=round(dval);

            if (uval > 255) uval = 255;
            can_have_r[uval] = 1;
         }
         for (i=0; i < g_maxval; i++) {
            double dval=((double)i)/ptci->dg_maxval_div255;
            unsigned int uval=round(dval);

            if (uval > 255) uval = 255;
            can_have_g[uval] = 1;
         }
         for (i=0; i < b_maxval; i++) {
            double dval=((double)i)/ptci->db_maxval_div255;
            unsigned int uval=round(dval);

            if (uval > 255) uval = 255;
            can_have_b[uval] = 1;
         }
         for (row=0; row < image_h; row++) {
            int col=0;

            for (col=0; col < image_w; col++) {
               int pixel=XGetPixel(image, col, row);
               uint32_t pix=(uint32_t)pixel;
               unsigned int r=0, g=0, b=0;
               double dr=(double)0, dg=(double)0, db=(double)0;

               r = (pix & ptci->r_mask) >> ptci->r_shift;
               g = (pix & ptci->g_mask) >> ptci->g_shift;
               b = (pix & ptci->b_mask) >> ptci->b_shift;
               dr = ((double)r) / ptci->dr_maxval_div255;
               dg = ((double)g) / ptci->dg_maxval_div255;
               db = ((double)b) / ptci->db_maxval_div255;
               r = round(dr);
               g = round(dg);
               b = round(db);
               if (r > 255) r = 255;
               if (g > 255) g = 255;
               if (b > 255) b = 255;
               if (r == 255 && g == 255) has_b[b] = 1;
               if (r == 255 && b == 255) has_g[g] = 1;
               if (g == 255 && b == 255) has_r[r] = 1;
            }
         }
         XDestroyImage(image);

         for (i=255; i >= 0 && !found; i--) {
            if (can_have_r[i] && !has_r[i]) {
               prci->pixel_for_trans = ((i << ptci->r_shift) & mainVisual->red_mask) |
                    ((255 << ptci->g_shift) & mainVisual->green_mask) |
                    ((255 << ptci->b_shift) & mainVisual->blue_mask) ;
               found = TRUE;
            }
         }
         for (i=255; i >= 0 && !found; i--) {
            if (can_have_g[i] && !has_g[i]) {
               prci->pixel_for_trans = ((i << ptci->g_shift) & mainVisual->green_mask);
               found = TRUE;
            }
         }
         for (i=255; i >= 0 && !found; i--) {
            if (can_have_b[i] && !has_b[i]) {
               prci->pixel_for_trans = ((i << ptci->b_shift) & mainVisual->blue_mask);
               found = TRUE;
            }
         }
         if (!found) {
            snprintf(gszMsgBox, sizeof(gszMsgBox), TgLoadString(STID_CANNOT_FIND_GOOD_TRANSPIX));
            MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
            return FALSE;
         }
      }
      prci->has_pixel_for_trans = TRUE;
      break;
   }
   if (!prci->has_pixel_for_trans) {
      return FALSE;
   }
#ifdef _TGIF_DBG /* debug, do not translate */
#ifdef NOT_DEFINED
   if (prci->xpm_ptr->real_type == XPM_XPM) {
      fprintf(stderr, "index_for_trans = %1d\n", prci->index_for_trans);
   } else {
      fprintf(stderr, "pixel_for_trans = %08x\n", prci->pixel_for_trans);
   }
#endif /* NOT_DEFINED */
#endif /* _TGIF_DBG */
   return TRUE;
}

void ReplaceColorWithTrans()
{
   struct ObjRec *obj_ptr=NULL;

   if (!CheckSelectionForImageProc(CMDID_REPLACECOLORWITHTRANS)) {
      return;
   }
   obj_ptr = topSel->obj;

   memset(&gReplaceColorInfo, 0, sizeof(ReplaceColorInfo));
   gReplaceColorInfo.cmdid = CMDID_REPLACECOLORWITHTRANS;
   if (!SetupReplaceColorInfo(obj_ptr, &gReplaceColorInfo)) {
      return;
   }
   if (somethingHighLighted) HighLightReverse();
   XSync(mainDisplay, False);
   PrepareToReplaceAnObj(obj_ptr);

   ResetThreshFillReplaceInfo();
   if (!ContinueReplaceColor(obj_ptr)) {
      AbortPrepareCmd(CMD_REPLACE);
   } else {
      RecordReplaceAnObj(obj_ptr);
   }
   if (!somethingHighLighted) HighLightForward();
}

/* ----------------------- FloodFill ----------------------- */

#define DIR_NONE 0
#define DIR_UP 1
#define DIR_RIGHT 2
#define DIR_DOWN 3
#define DIR_LEFT 4

/*
 *      1
 *      ^
 *      |
 * 4 <- 0 -> 2
 *      |
 *      v
 *      3
 */

static int gnPixelToFill=(-1);

typedef struct tagCell {
   int row;
   int col;
   int dir; /* direction where it came from */
} Cell;

#define CELL_NOTVISITED(map,c,r,image_w) (map[(c)+(r)*(image_w)]==0)

static
int SelectThisPixelForFloodFill(c, r, pixel, image)
   int c, r, pixel;
   XImage *image;
{
   int pix=XGetPixel(image, c, r);

   if (gThreshFillReplaceInfo.use_thresholding) {
      if (fullTrueColorMode) {
         if (pix != (-1) && TrueColorPixelWithinRange(pix,
               &gThreshFillReplaceInfo)) {
            /* skip */
         } else {
            return FALSE;
         }
      } else {
         int global_color_index=(-1);

         if (pix != (-1) && HashLookUpInt(
               &gThreshFillReplaceInfo.hash_table, (char*)(&pix),
               sizeof(int), &global_color_index)) {
            if (global_color_index != (-1) &&
                  gThreshFillReplaceInfo.within_threshold[global_color_index]) {
               /* skip */
            } else {
               return FALSE;
            }
         } else {
            return FALSE;
         }
      }
   } else {
      if (pix != pixel) {
         return FALSE;
      }
   }
   return TRUE;
}

static
void FloodFillAddToQueue(plist, c, r, dir, pbbox)
   CVList *plist;
   int c, r, dir;
   struct BBRec *pbbox;
{
   Cell *cell=(Cell*)malloc(sizeof(Cell));

   if (cell == NULL) FailAllocMessage();
   memset(cell, 0, sizeof(Cell));
   cell->row = r;
   cell->col = c;
   cell->dir = dir;

   ListAppend(plist, cell);
   if (c < pbbox->ltx) pbbox->ltx = c;
   if (c > pbbox->rbx) pbbox->rbx = c;
   if (r < pbbox->lty) pbbox->lty = r;
   if (r > pbbox->rby) pbbox->rby = r;
}

static
void FloodFillBFS(image, pixel, image_w, image_h, plist, map, pbbox)
   XImage *image;
   int pixel, image_w, image_h;
   CVList *plist;
   unsigned char *map;
   struct BBRec *pbbox;
{
   while (!ListEmpty(plist)) {
      CVListElem *elem=ListFirst(plist);
      Cell *cell=(Cell*)(elem->obj);
      int c=cell->col, r=cell->row; /* this pixel must be already selected */
      int dir=cell->dir;
      int next_r=0, next_c=0, next_dir=0;

      ListUnlink(plist, elem);
      free(elem);
      free(cell);

      if (dir != DIR_DOWN && r-1 >= 0) {
         next_c = c;
         next_r = r-1;
         next_dir = DIR_UP;
         if (CELL_NOTVISITED(map,next_c,next_r,image_w)) {
             if (SelectThisPixelForFloodFill(next_c, next_r, pixel, image)) {
                map[next_c+(next_r*image_w)] = 1;
                FloodFillAddToQueue(plist, next_c, next_r, next_dir, pbbox);
             } else {
                map[next_c+(next_r*image_w)] = (-1);
             }
         }
      }
      if (dir != DIR_LEFT && c+1 < image_w) {
         next_c = c+1;
         next_r = r;
         next_dir = DIR_RIGHT;
         if (CELL_NOTVISITED(map,next_c,next_r,image_w)) {
             if (SelectThisPixelForFloodFill(next_c, next_r, pixel, image)) {
                map[next_c+(next_r*image_w)] = 1;
                FloodFillAddToQueue(plist, next_c, next_r, next_dir, pbbox);
             } else {
                map[next_c+(next_r*image_w)] = (-1);
             }
         }
      }
      if (dir != DIR_UP && r+1 < image_h) {
         next_c = c;
         next_r = r+1;
         next_dir = DIR_DOWN;
         if (CELL_NOTVISITED(map,next_c,next_r,image_w)) {
             if (SelectThisPixelForFloodFill(next_c, next_r, pixel, image)) {
                map[next_c+(next_r*image_w)] = 1;
                FloodFillAddToQueue(plist, next_c, next_r, next_dir, pbbox);
             } else {
                map[next_c+(next_r*image_w)] = (-1);
             }
         }
      }
      if (dir != DIR_RIGHT && c-1 >= 0) {
         next_c = c-1;
         next_r = r;
         next_dir = DIR_LEFT;
         if (CELL_NOTVISITED(map,next_c,next_r,image_w)) {
             if (SelectThisPixelForFloodFill(next_c, next_r, pixel, image)) {
                map[next_c+(next_r*image_w)] = 1;
                FloodFillAddToQueue(plist, next_c, next_r, next_dir, pbbox);
             } else {
                map[next_c+(next_r*image_w)] = (-1);
             }
         }
      }
   }
}

static
void CalcFloodFill(c, r, pixel, image, image_w, image_h, dir, map, pbbox)
   int c, r, pixel, image_w, image_h, dir;
   XImage *image;
   unsigned char *map;
   struct BBRec *pbbox;
{
   CVList list;

   CVListInit(&list);

   FloodFillAddToQueue(&list, c, r, dir, pbbox);
   FloodFillBFS(image, pixel, image_w, image_h, &list, map, pbbox);
}

static
void DoFloodFill(image, image_w, map, pbbox)
   XImage *image;
   int image_w;
   unsigned char *map;
   struct BBRec *pbbox;
{
   int r=0;

   for (r=pbbox->lty; r <= pbbox->rby; r++) {
      int c=0;

      for (c=pbbox->ltx; c <= pbbox->rbx; c++) {
         if (map[c+r*image_w] == 1) {
            XPutPixel(image, c, r, gnPixelToFill);
         }
      }
   }
}

static
void StartFloodFill(obj_ptr, image, bitmap_image, image_x, image_y,
      image_w, image_h, do_flood_fill)
   struct ObjRec *obj_ptr;
   XImage *image, *bitmap_image;
   int image_x, image_y, image_w, image_h, do_flood_fill;
{
   struct XPmRec *xpm_ptr=obj_ptr->detail.xpm;
   int pixel=(-1);
   XColor pixel_color;

   memset(&pixel_color, 0, sizeof(XColor));
   if (bitmap_image == NULL) {
      pixel = XGetPixel(image, image_x, image_y);
   } else {
      if (XGetPixel(bitmap_image, image_x, image_y) == 0) {
         /* transparent */
      } else {
         pixel = XGetPixel(image, image_x, image_y);
      }
   }
   if (do_flood_fill && pixel != (-1) && gThreshFillReplaceInfo.use_thresholding) {
      if (!SetupThreshFillReplaceInfo(&gThreshFillReplaceInfo, pixel, &pixel_color)) {
         return;
      }
   }
   gnPixelToFill = colorPixels[colorIndex];
   if (pixel == (-1)) {
   } else if (gnPixelToFill != pixel) {
      int i, *pixels=xpm_ptr->pixels, ncolors=xpm_ptr->ncolors, found=FALSE;

      if (do_flood_fill) {
         struct BBRec bbox;
         unsigned char *map=(unsigned char *)malloc(image_h*image_w*sizeof(unsigned char));

         if (map == NULL) FailAllocMessage();
         /* 0 means not visited, 1 means update pixel, otherwise, don't update */
         memset(map, 0, image_h*image_w*sizeof(unsigned char));
         map[image_x+image_y*image_w] = 1;
         bbox.ltx = bbox.rbx = image_x;
         bbox.lty = bbox.rby = image_y;
         CalcFloodFill(image_x, image_y, pixel, image, image_w, image_h, 0, map, &bbox);
         DoFloodFill(image, image_w, map, &bbox);
         free(map);

         if (gThreshFillReplaceInfo.within_threshold != NULL) {
            free(gThreshFillReplaceInfo.within_threshold);
            gThreshFillReplaceInfo.within_threshold = NULL;
         }
      } else {
         XPutPixel(image, image_x, image_y, gnPixelToFill);
      }
      for (i=0; i < ncolors; i++) {
         if (pixels[i] == gnPixelToFill) {
            found = TRUE;
         }
      }
      if (!found && xpm_ptr->real_type == XPM_XPM) {
         int chars_per_pixel=xpm_ptr->chars_per_pixel;

         xpm_ptr->pixels = (int*)realloc(xpm_ptr->pixels,
               sizeof(int)*(ncolors+1));
         if (xpm_ptr->pixels == NULL) FailAllocMessage();
         xpm_ptr->pixels[ncolors] = colorPixels[colorIndex];

         xpm_ptr->color_str = (char**)realloc(xpm_ptr->color_str,
               sizeof(char*)*(ncolors+1));
         if (xpm_ptr->color_str == NULL) FailAllocMessage();
         xpm_ptr->color_str[ncolors] = UtilStrDup(colorMenuItems[colorIndex]);
         if (xpm_ptr->color_str[ncolors] == NULL) FailAllocMessage();

         if (chars_per_pixel == 1 && ncolors >= 20) {
            /* needs to go from 1 chars_per_pixel to 2 chars_per_pixel */
            char *color_char=(char*)malloc(((ncolors+1)<<1)*sizeof(char));

            if (color_char == NULL) FailAllocMessage();
            for (i=0; i < ncolors+1; i++) {
               if (i == 0 && xpm_ptr->color_char[0] == '`') {
                  color_char[i<<1] = color_char[(i<<1)+1] = '`';
               } else {
                  color_char[i<<1] = (char)(((int)('a'))+(int)(i/10));
                  color_char[(i<<1)+1] = (char)(((int)('0'))+(int)(i%10));
               }
            }
            free(xpm_ptr->color_char);
            xpm_ptr->color_char = color_char;
            xpm_ptr->chars_per_pixel = 2;
         } else {
            char *color_char;

            xpm_ptr->color_char = color_char =
                  (char*)realloc(xpm_ptr->color_char,
                  sizeof(char)*chars_per_pixel*(ncolors+1));
            if (color_char == NULL) FailAllocMessage();
            if (chars_per_pixel == 1) {
               for (i=0; i < ncolors+1; i++) {
                  if (i == 0 && xpm_ptr->color_char[0] == '`') {
                     color_char[i] = '`';
                  } else {
                     color_char[i] = (char)(((int)('a'))+i-1);
                  }
               }
            } else {
               for (i=0; i < ncolors+1; i++) {
                  if (i == 0 && xpm_ptr->color_char[0] == '`' &&
                        xpm_ptr->color_char[1] == '`') {
                     color_char[i<<1] = color_char[(i<<1)+1] = '`';
                  } else {
                     color_char[i<<1] = (char)(((int)('a'))+(int)(i/10));
                     color_char[(i<<1)+1] = (char)(((int)('0'))+(int)(i%10));
                  }
               }
            }
         }
         xpm_ptr->ncolors++;
      }
      if (xpm_ptr->data != NULL) {
#ifdef _TGIF_DBG /* debug, do not translate */
         fprintf(stderr,
               "In FloodFill(), unexpected xpm_ptr->data != NULL.\n");
#endif /* _TGIF_DBG */
      }
   }
}

static
int TryFloodFill(obj_ptr, image_w, image_h, image, bitmap_image, button,
      mouse_x, mouse_y)
   struct ObjRec *obj_ptr;
   int image_w, image_h, mouse_x, mouse_y;
   XImage *image, *bitmap_image;
   unsigned int button;
{
   int changed=FALSE, abs_x=ABS_X(mouse_x), abs_y=ABS_Y(mouse_y), found=FALSE;
   int image_x=0, image_y=0;

   if (obj_ptr->ctm == NULL) {
      if (abs_x >= obj_ptr->obbox.ltx && abs_y >= obj_ptr->obbox.lty &&
            abs_x < obj_ptr->obbox.rbx && abs_y < obj_ptr->obbox.rby) {
         image_x = abs_x-obj_ptr->obbox.ltx;
         image_y = abs_y-obj_ptr->obbox.lty;
         if (image_x >= 0 && image_y >= 0 &&
               image_x < image_w && image_y < image_h) {
            found = TRUE;
         }
      }
   } else {
      double image_dx=(double)0, image_dy=(double)0;

      ReverseTransformDoublePointThroughCTM(
            ((double)(abs_x-obj_ptr->x)+0.5),
            ((double)(abs_y-obj_ptr->y)+0.5),
            obj_ptr->ctm, &image_dx, &image_dy);
      image_dx += (double)(obj_ptr->x-obj_ptr->orig_obbox.ltx);
      image_dy += (double)(obj_ptr->y-obj_ptr->orig_obbox.lty);
      if (image_dx >= ((double)0) && image_dy >= ((double)0) &&
            image_dx < ((double)image_w) &&
            image_dy < ((double)image_h)) {
         image_x = (int)image_dx;
         image_y = (int)image_dy;
         if (image_x < 0) image_x = 0;
         if (image_x >= image_w) image_x = image_w-1;
         if (image_y < 0) image_y = 0;
         if (image_y >= image_h) image_y = image_h-1;
         found = TRUE;
      }
   }
   if (found) {
      changed = TRUE;
      if (somethingHighLighted) HighLightReverse();
      StartFloodFill(obj_ptr, image, bitmap_image, image_x, image_y,
            image_w, image_h, button==Button1);
      if (!somethingHighLighted) HighLightForward();
   }
   return changed;
}

static
void GetUntransformedPoint(obj_ptr, abs_x, abs_y, v)
   struct ObjRec *obj_ptr;
   int abs_x, abs_y;
   IntPoint *v;
{
   double image_dx=(double)0, image_dy=(double)0;

   ReverseTransformDoublePointThroughCTM(
         ((double)(abs_x-obj_ptr->x)+0.5),
         ((double)(abs_y-obj_ptr->y)+0.5),
         obj_ptr->ctm, &image_dx, &image_dy);
   image_dx += (double)(obj_ptr->x-obj_ptr->orig_obbox.ltx);
   image_dy += (double)(obj_ptr->y-obj_ptr->orig_obbox.lty);
   v->x = (int)image_dx;
   v->y = (int)image_dy;
}

static
int ContinueFloodFill(obj_ptr)
   struct ObjRec *obj_ptr;
{
   int done=FALSE, image_w, image_h, changed=FALSE;
   struct XPmRec *xpm_ptr=obj_ptr->detail.xpm;
   XImage *image=NULL, *bitmap_image=NULL;

   xpm_ptr = obj_ptr->detail.xpm;
   image_w = xpm_ptr->image_w;
   image_h = xpm_ptr->image_h;
   image = xpm_ptr->image;
   bitmap_image = xpm_ptr->bitmap_image;
   if (image == NULL) {
      image = xpm_ptr->image = XGetImage(mainDisplay, xpm_ptr->pixmap, 0, 0,
            image_w, image_h, AllPlanes, ZPixmap);
      if (image == NULL) FailAllocMessage();
   }
   if (xpm_ptr->bitmap != None && bitmap_image == NULL) {
      bitmap_image = xpm_ptr->bitmap_image = XGetImage(mainDisplay,
            xpm_ptr->bitmap, 0, 0, image_w, image_h, AllPlanes, ZPixmap);
      if (bitmap_image == NULL) FailAllocMessage();
   }
   SaveStatusStrings();
   sprintf(gszMsgBox, "    %s", TgLoadString(STID_ESC_TO_FINISH));
   TwoLineMsg(TgLoadString(STID_BTN1_FLOODFILL_BTN3_SET_COLOR), gszMsgBox);
   SetMouseStatus(TgLoadString(STID_FLOOD_FILL),
         TgLoadCachedString(CSTID_FINISH), TgLoadString(STID_SET_A_PIXEL));
   while (!done) {
      int mouse_x=0, mouse_y=0, mouse_end_x=0, mouse_end_y=0;
      unsigned int button=FillReplacePickAPoint(&mouse_x, &mouse_y,
            &mouse_end_x, &mouse_end_y, TRUE, floodCursor);

      SetWatchCursor(drawWindow);
      SetWatchCursor(mainWindow);
      if (button == Button1) {
         changed = TryFloodFill(obj_ptr, image_w, image_h, image, bitmap_image,
               button, mouse_x, mouse_y);
      } else if (button == Button3) {
         if (mouse_x == mouse_end_x && mouse_y == mouse_end_y) {
            changed = TryFloodFill(obj_ptr, image_w, image_h, image,
                  bitmap_image, button, mouse_x, mouse_y);
         } else if (obj_ptr->ctm == NULL) {
            int r=0;
            struct BBRec bbox;

            SetBBRec(&bbox, mouse_x, mouse_y, mouse_end_x, mouse_end_y);
            for (r=bbox.lty; r < bbox.rby; r++) {
               int c=0;

               for (c=bbox.ltx; c < bbox.rbx; c++) {
                  if (TryFloodFill(obj_ptr, image_w, image_h, image,
                        bitmap_image, button, c, r)) {
                     changed = TRUE;
                  }
               }
            }
         } else {
            int r=0, abs_x=ABS_X(mouse_x), abs_y=ABS_Y(mouse_y);
            int abs_end_x=ABS_X(mouse_end_x), abs_end_y=ABS_Y(mouse_end_y);
            int ltx=0, lty=0, rbx=0, rby=0;
            IntPoint ivs[6];

            GetUntransformedPoint(obj_ptr, abs_x, abs_y, &ivs[0]);
            GetUntransformedPoint(obj_ptr, abs_x, abs_end_y, &ivs[1]);
            GetUntransformedPoint(obj_ptr, abs_end_x, abs_end_y, &ivs[2]);
            GetUntransformedPoint(obj_ptr, abs_end_x, abs_y, &ivs[3]);
            memcpy(&ivs[4], &ivs[0], sizeof(IntPoint));
            ltx = min(min(ivs[0].x, ivs[1].x),min(ivs[2].x, ivs[3].x));
            lty = min(min(ivs[0].y, ivs[1].y),min(ivs[2].y, ivs[3].y));
            rbx = max(max(ivs[0].x, ivs[1].x),max(ivs[2].x, ivs[3].x));
            rby = max(max(ivs[0].y, ivs[1].y),max(ivs[2].y, ivs[3].y));

            for (r=lty; r < rby; r++) {
               int c=0;

               if (r < 0 || r >= image_h) continue;
               for (c=ltx; c < rbx; c++) {
                  if (c < 0 || c >= image_w) continue;

                  if (PointInIntPolygon(c, r, 5, ivs)) {
                     StartFloodFill(obj_ptr, image, bitmap_image, c, r,
                           c, r, FALSE);
                     changed = TRUE;
                  }
               }
            }
         }
      } else {
         done = TRUE;
      }
      if (!done) {
         if (changed) {
            XPutImage(mainDisplay, xpm_ptr->pixmap, xpmGC, image, 0, 0, 0, 0,
               image_w, image_h);
            if (bitmap_image != NULL) {
               XPutImage(mainDisplay, xpm_ptr->bitmap, xbmGC, bitmap_image,
                  0, 0, 0, 0, image_w, image_h);
            }
            if (xpm_ptr->cached_pixmap != None) {
               XFreePixmap(mainDisplay, xpm_ptr->cached_pixmap);
               xpm_ptr->cached_pixmap = None;
            }
            if (xpm_ptr->cached_bitmap != None) {
               XFreePixmap(mainDisplay, xpm_ptr->cached_bitmap);
               xpm_ptr->cached_bitmap = None;
            }
            if ((xpm_ptr->real_type == XPM_JPEG ||
                  xpm_ptr->real_type == PPM_TRUE) &&
                  fullTrueColorMode && HasZlibSupport()) {
               unsigned int ppm_data_size=0;
               char tmp_fname[MAXPATHLENGTH], ext[MAXPATHLENGTH];
               char *ppm_data=NULL;

               *tmp_fname = *ext = '\0';
               if (MkTempFile(tmp_fname, sizeof(tmp_fname), tmpDir,
                     TOOL_NAME) == NULL) {
                  /* print error message? */
                  return FALSE;
               }
               if (!DumpXImageToFile(xpm_ptr->image, xpm_ptr->image_w,
                     xpm_ptr->image_h, tmp_fname, ext)) {
                  /* print error message? */
                  return FALSE;
               }
               if (strcmp(ext, ".ppm.z") == 0) {
                  char deflated_fname[MAXPATHLENGTH];

                  snprintf(deflated_fname, sizeof(deflated_fname), "%s%s",
                        tmp_fname, ext);
                  ppm_data = ReadFileIntoBuf(deflated_fname, &ppm_data_size);
                  unlink(deflated_fname);
               }
               if (ppm_data != NULL) {
                  if (xpm_ptr->ppm_data != NULL) free(xpm_ptr->ppm_data);
                  if (xpm_ptr->ppm_mask_data != NULL) {
                     free(xpm_ptr->ppm_mask_data);
                  }
                  xpm_ptr->real_type = PPM_TRUE;
                  xpm_ptr->ppm_data = ppm_data;
                  xpm_ptr->ppm_data_size = ppm_data_size;
                  xpm_ptr->ppm_mask_data = NULL;
                  xpm_ptr->ppm_mask_size = 0;
                  xpm_ptr->ppm_data_compress = PPM_DATA_DEFLATED;
               }
            }
            AdjObjCache(obj_ptr);
            if (somethingHighLighted) HighLightReverse();
            RedrawAnArea(botObj, selLtX-GRID_ABS_SIZE(1),
                  selLtY-GRID_ABS_SIZE(1), selRbX+GRID_ABS_SIZE(1),
                  selRbY+GRID_ABS_SIZE(1));
            if (!somethingHighLighted) HighLightForward();
            SetFileModified(TRUE);
            justDupped = FALSE;
         } else {
            SetStringStatus(TgLoadString(STID_SEL_PT_NOT_ON_IMAGE));
         }
      }
      SetDefaultCursor(mainWindow);
      ShowCursor();
   }
   RestoreStatusStrings();
   return changed;
}

void FloodFill()
{
   struct ObjRec *obj_ptr=NULL;
   unsigned char trans_color_r='\0', trans_color_g='\0', trans_color_b='\0';

   if (!CheckSelectionForImageProc(CMDID_FLOODFILL)) {
      return;
   }
   obj_ptr = topSel->obj;
   if (ObjHasTrueColorTransPixel(obj_ptr, &trans_color_r, &trans_color_g,
         &trans_color_b)) {
      if (CurColorIsTranscolor(trans_color_r, trans_color_g, trans_color_b)) {
         sprintf(gszMsgBox, TgLoadString(STID_CUR_COLOR_IS_TRANS_PIXEL),
               colorMenuItems[colorIndex],
               GetImageProcName(CMDID_FLOODFILL));
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
         return;
      }
   }
   if (somethingHighLighted) HighLightReverse();
   XSync(mainDisplay, False);
   PrepareToReplaceAnObj(obj_ptr);

   ResetThreshFillReplaceInfo();
   if (ContinueFloodFill(obj_ptr)) {
      RecordReplaceAnObj(obj_ptr);
   } else {
      AbortPrepareCmd(CMD_REPLACE);
   }
   if (!somethingHighLighted) HighLightForward();
}

/* ----------------------- ToggleFloodReplaceColorThreshold ----------------------- */

void ToggleFloodReplaceColorThreshold()
{
   threshFillReplaceEnabled = !threshFillReplaceEnabled;

   if (threshFillReplaceEnabled) {
      sprintf(gszMsgBox, TgLoadString(STID_FLOOD_REPLACE_ENABLED),
            fillReplaceRedThresh, fillReplaceGreenThresh,
            fillReplaceBlueThresh);
      Msg(gszMsgBox);
   } else {
      sprintf(gszMsgBox, TgLoadString(STID_FLOOD_REPLACE_DISABLED));
      Msg(gszMsgBox);
   }
}

/* ----------------------- SetFloodReplaceColorThreshold ----------------------- */

void SetFloodReplaceColorThreshold()
{
   char spec[MAXSTRING];

   *spec = '\0';
   sprintf(gszMsgBox, TgLoadString(STID_SET_FLOOD_REPLACE_THRESH),
         fillReplaceRedThresh, fillReplaceGreenThresh,
         fillReplaceBlueThresh);
   if (Dialog(gszMsgBox, NULL, spec) == INVALID) return;
   UtilTrimBlanks(spec);
   if (*spec == '\0') return;

   if (ParseThreshFillReplaceSpec(spec)) {
      sprintf(gszMsgBox, TgLoadString(STID_FLOOD_REPLACE_THRESH_SET_TO),
            fillReplaceRedThresh, fillReplaceGreenThresh,
            fillReplaceBlueThresh);
      Msg(gszMsgBox);
   }
}

/* ----------------------- CreateContour ----------------------- */

/*------------------------------------------------*/
/* the contour code is adapted from Tim Feldman's */
/*        contour code in Graphics Gem III        */
/*------------------------------------------------*/

typedef struct CDirInfo {
   /* 5 6 7 */
   /* 4   0 */
   /* 3 2 1 */
   short dir;
   struct CDirInfo *next;
} CDirInfoPtr;

static struct CDirInfo *topOfChain=NULL, *botOfChain=NULL;

static int gnContourX=0, gnContourY=0;
static int gnContourW=0, gnContourH=0;
static unsigned short gnContourRed=0, gnContourGreen=0, gnContourBlue=0;
static int **gnaContourPixels=NULL;
static XImage *gContourImage=NULL, *gContourBitmapImage=NULL;
static struct ObjRec *gpContourObj=NULL;

static
void CleanUpContour()
{
   if (gnaContourPixels != NULL) {
      int i;

      for (i=0; i < gnContourH; i++) {
         if (gnaContourPixels[i] != NULL) {
            free(gnaContourPixels[i]);
         } else {
            break;
         }
      }
      free(gnaContourPixels);
   }
   gnaContourPixels = NULL;

   for ( ; topOfChain != NULL; topOfChain=botOfChain) {
      botOfChain = topOfChain->next;
      free(topOfChain);
   }
   topOfChain = botOfChain = NULL;
}

static
int OnContour(x, y)
   int x, y;
{
   if (x >= 0 && x < gnContourW && y >= 0 && y < gnContourH) {
      int index;

      if (gnaContourPixels[y][x] == BAD) {
         gnaContourPixels[y][x] = XGetPixel(gContourImage, x, y);
      }
      index = GetIndexOfPixel(gnaContourPixels[y][x]);
      if (tgifColors[index].red == gnContourRed &&
            tgifColors[index].green == gnContourGreen &&
            tgifColors[index].blue == gnContourBlue) {
         return TRUE;
      }
   }
   return FALSE;
}

static
int ProbeContour(x, y, dir, pn_new_x, pn_new_y)
   int x, y, dir, *pn_new_x, *pn_new_y;
{
   switch (dir) {
   case 0: x++;      break;
   case 1: x++; y++; break;
   case 2:      y++; break;
   case 3: x--; y++; break;
   case 4: x--;      break;
   case 5: x--; y--; break;
   case 6:      y--; break;
   case 7: x++; y--; break;
   }
   *pn_new_x = x;
   *pn_new_y = y;
   return OnContour(x, y);
}

static
int ContourNeighbor(x, y, last_dir, pn_new_x, pn_new_y)
   int x, y, last_dir, *pn_new_x, *pn_new_y;
   /*
    * if last vector was 0, start looking at 1
    * if last vector was 1, start looking at 3
    * if last vector was 2, start looking at 3
    * if last vector was 3, start looking at 5
    * if last vector was 4, start looking at 5
    * if last vector was 5, start looking at 7
    * if last vector was 6, start looking at 7
    * if last vector was 7, start looking at 1
    */
{
   int i;

   if (last_dir & 0x1) {
      last_dir += 2;
   } else {
      last_dir++;
   }
   if (last_dir > 7) last_dir -= 8;
   for (i=0; i < 8; i++) {
      if (ProbeContour(x, y, last_dir, pn_new_x, pn_new_y)) {
         return last_dir;
      } else {
         if (--last_dir < 0) last_dir += 8;
      }
   }
#ifdef _TGIF_DBG /* debug, do not translate */
   fprintf(stderr, "Should not have come here ContourNeighbor()!\n");
#endif /* _TGIF_DBG */
   return (-1);
}

static
int CreatePolyFromContour(num_pts)
   int num_pts;
{
   int x=gnContourX, y=gnContourY, generate=(num_pts > 2);
   struct CDirInfo *pcdi=topOfChain;
   struct XfrmMtrxRec *ctm=NULL;

   if (generate) {
      ResetCreatePolygon();
      ctm = gpContourObj->ctm;
   } else {
      num_pts = 0;
   }
   while (pcdi != NULL) {
      int dir=pcdi->dir, n=1;
      struct CDirInfo *pcdi1=pcdi->next;

      for ( ; pcdi1 != NULL; pcdi1=pcdi1->next) {
         if (pcdi1->dir != dir) {
            break;
         } else {
            n++;
         }
      }
      if (generate) {
         if (ctm == NULL) {
            AddPtToCreatePolygon(gpContourObj->x+x, gpContourObj->y+y);
         } else {
            int tmp_x, tmp_y;

            TransformPointThroughCTM(x, y, ctm, &tmp_x, &tmp_y);
            AddPtToCreatePolygon(gpContourObj->x+tmp_x, gpContourObj->y+tmp_y);
         }
      } else {
         num_pts++;
      }
      switch (dir) {
      case 0: x += n;         break;
      case 1: x += n; y += n; break;
      case 2:         y += n; break;
      case 3: x -= n; y += n; break;
      case 4: x -= n;         break;
      case 5: x -= n; y -= n; break;
      case 6:         y -= n; break;
      case 7: x += n; y -= n; break;
      }
      pcdi = pcdi1;
   }
   if (generate) {
      if (ctm == NULL) {
         AddPtToCreatePolygon(gpContourObj->x+x, gpContourObj->y+y);
      } else {
         int tmp_x, tmp_y;

         TransformPointThroughCTM(x, y, ctm, &tmp_x, &tmp_y);
         AddPtToCreatePolygon(gpContourObj->x+tmp_x, gpContourObj->y+tmp_y);
      }
      CreatePolygonObj(num_pts, TRUE);
   } else {
      num_pts++;
   }
   if (generate) {
      return TRUE;
   }
   return num_pts;
}

static
int DoCreateContour()
{
   int x, y, dir, new_x, new_y, last_dir, num_pts=0;

   while (OnContour(gnContourX, gnContourY)) {
      gnContourX--;
   }
   gnContourX++;
   topOfChain = NULL;

   x = new_x = gnContourX;
   y = new_y = gnContourY;
   dir = 0;
   for (;;) {
      if (ProbeContour(x, y, dir, &new_x, &new_y)) {
         break;
      } else if (++dir >= 8) {
         MsgBox(TgLoadString(STID_NO_CONTOUR_CAN_BE_GEN_HERE), TOOL_NAME,
               INFO_MB);
         return FALSE;
      }
   }
   last_dir = 1;
   for (;;) {
      struct CDirInfo *pcdi;

      dir = ContourNeighbor(x, y, last_dir, &new_x, &new_y);
      pcdi = (struct CDirInfo *)malloc(sizeof(struct CDirInfo));
      if (pcdi == NULL) FailAllocMessage();
      memset(pcdi, 0, sizeof(struct CDirInfo));
      pcdi->dir = dir;
      pcdi->next = NULL;
      if (botOfChain == NULL) {
         topOfChain = pcdi;
      } else {
         botOfChain->next = pcdi;
      }
      botOfChain = pcdi;
      if (new_x == gnContourX && new_y == gnContourY) {
         break;
      }
      x = new_x;
      y = new_y;
      last_dir = dir;
   }
   num_pts = CreatePolyFromContour(0);
   if (num_pts > 2) {
      return CreatePolyFromContour(num_pts);
   }
   MsgBox(TgLoadString(STID_NO_CONTOUR_CAN_BE_GEN_HERE), TOOL_NAME, INFO_MB);
   return FALSE;
}

static
int StartCreateContour(obj_ptr, image, bitmap_image, image_x, image_y,
      image_w, image_h)
   struct ObjRec *obj_ptr;
   XImage *image, *bitmap_image;
   int image_x, image_y, image_w, image_h;
{
   int i, pixel=(-1), created=FALSE;

   gnContourX = image_x;
   gnContourY = image_y;
   gnContourW = image_w;
   gnContourH = image_h;
   gContourImage = image;
   gContourBitmapImage = bitmap_image;
   gpContourObj = obj_ptr;

   gnaContourPixels = (int**)malloc(image_h*sizeof(int*));
   if (gnaContourPixels == NULL) {
      return FailAllocMessage();
   }
   memset(gnaContourPixels, 0, image_h*sizeof(int*));
   for (i=0; i < image_h; i++) {
      int j;

      gnaContourPixels[i] = (int*)malloc(image_w*sizeof(int));
      if (gnaContourPixels[i] == NULL) {
         FailAllocMessage();
         CleanUpContour();
         return FALSE;
      }
      for (j=0; j < image_w; j++) gnaContourPixels[i][j] = BAD;
   }
   if (!CreatePixelToIndexMapping()) {
      CleanUpContour();
      return FALSE;
   }
   if (bitmap_image == NULL) {
      pixel = XGetPixel(image, image_x, image_y);
   } else {
      if (XGetPixel(bitmap_image, image_x, image_y) == 0) {
         /* transparent */
      } else {
         pixel = XGetPixel(image, image_x, image_y);
      }
   }
   SetWatchCursor(drawWindow);
   SetWatchCursor(mainWindow);
   gnPixelToFill = colorPixels[colorIndex];
   if (pixel == (-1)) {
   } else {
      int index=GetIndexOfPixel(pixel);

      gnContourRed = tgifColors[index].red;
      gnContourGreen = tgifColors[index].green;
      gnContourBlue = tgifColors[index].blue;
      created = DoCreateContour();
   }
   SetDefaultCursor(mainWindow);
   ShowCursor();

   CleanUpIndexOfPixel();
   CleanUpContour();

   return created;
}

static
int ContinueCreateContour(obj_ptr)
   struct ObjRec *obj_ptr;
{
   int image_w, image_h, changed=FALSE, mouse_x=0, mouse_y=0;
   unsigned int button;
   struct XPmRec *xpm_ptr=obj_ptr->detail.xpm;
   XImage *image, *bitmap_image;

   xpm_ptr = obj_ptr->detail.xpm;
   image_w = xpm_ptr->image_w;
   image_h = xpm_ptr->image_h;
   image = xpm_ptr->image;
   bitmap_image = xpm_ptr->bitmap_image;
   if (image == NULL) {
      image = xpm_ptr->image = XGetImage(mainDisplay, xpm_ptr->pixmap, 0, 0,
            image_w, image_h, AllPlanes, ZPixmap);
      if (image == NULL) FailAllocMessage();
   }
   if (xpm_ptr->bitmap != None && bitmap_image == NULL) {
      bitmap_image = xpm_ptr->bitmap_image = XGetImage(mainDisplay,
            xpm_ptr->bitmap, 0, 0, image_w, image_h, AllPlanes, ZPixmap);
      if (bitmap_image == NULL) FailAllocMessage();
   }
   SaveStatusStrings();
   Msg(TgLoadString(STID_SEL_A_COLOR_TO_BE_TRACED));
   SetMouseStatus(TgLoadString(STID_START_CONTOUR),
         TgLoadCachedString(CSTID_FINISH), TgLoadCachedString(CSTID_FINISH));

   button = FillReplacePickAPoint(&mouse_x, &mouse_y, NULL, NULL, FALSE,
         handCursor);

   if (button == Button1) {
      int abs_x=ABS_X(mouse_x), abs_y=ABS_Y(mouse_y), found=FALSE;
      int image_x=0, image_y=0;

      if (obj_ptr->ctm == NULL) {
         if (abs_x >= obj_ptr->obbox.ltx && abs_y >= obj_ptr->obbox.lty &&
               abs_x < obj_ptr->obbox.rbx && abs_y < obj_ptr->obbox.rby) {
            image_x = abs_x-obj_ptr->obbox.ltx;
            image_y = abs_y-obj_ptr->obbox.lty;
            if (image_x >= 0 && image_y >= 0 &&
                  image_x < image_w && image_y < image_h) {
               found = TRUE;
            }
         }
      } else {
         double image_dx=(double)0, image_dy=(double)0;

         ReverseTransformDoublePointThroughCTM(
               ((double)(abs_x-obj_ptr->x)+0.5),
               ((double)(abs_y-obj_ptr->y)+0.5),
               obj_ptr->ctm, &image_dx, &image_dy);
         image_x += (double)(obj_ptr->x-obj_ptr->orig_obbox.ltx);
         image_y += (double)(obj_ptr->y-obj_ptr->orig_obbox.lty);
         if (image_dx >= ((double)0) && image_dy >= ((double)0) &&
               image_dx < ((double)image_w) && image_dy < ((double)image_h)) {
            image_x = (int)image_dx;
            image_y = (int)image_dy;
            if (image_x < 0) image_x = 0;
            if (image_x >= image_w) image_x = image_w-1;
            if (image_y < 0) image_y = 0;
            if (image_y >= image_h) image_y = image_h-1;
            found = TRUE;
         }
      }
      if (found) {
         if (somethingHighLighted) HighLightReverse();
         changed = StartCreateContour(obj_ptr, image, bitmap_image, image_x,
               image_y, image_w, image_h);
         if (!somethingHighLighted) HighLightForward();
      } else {
         SetStringStatus(TgLoadString(STID_SEL_PT_NOT_ON_IMAGE));
      }
   }
   RestoreStatusStrings();
   return changed;
}

void CreateContour()
{
   struct ObjRec *obj_ptr;

   if (!CheckSelectionForImageProc(CMDID_CREATECONTOUR)) {
      return;
   }
   obj_ptr = topSel->obj;

   if (somethingHighLighted) HighLightReverse();
   XSync(mainDisplay, False);
   if (ContinueCreateContour(obj_ptr)) {
      if (somethingHighLighted) HighLightReverse();
      RemoveAllSel();
      numRedrawBBox = 0;
      obj_ptr->tmp_parent = NULL;
      DrawObj(drawWindow, topObj);
      SelectTopObj();
      RecordNewObjCmd();
      SetFileModified(TRUE);
      justDupped = FALSE;
   } else {
      if (!somethingHighLighted) HighLightForward();
   }
}

/* ----------------------- Init and Clean Up ----------------------- */

void CleanUpImageProc()
{
   CleanUpConvolution();
}

/* do not translate -- program constants */
static char gszDefBggen[]="bggen %s -g %s | ppmquant 64 | ppmtoxpm";
static char gszDefPpm6Bggen[]="bggen %s -g %s";
static char gszDefPpmquant[]="pnmquant %d %s";
static char gszDefPpmFSquant[]="pnmquant -fs %d %s";

void InitImageProc()
{
   char *c_ptr=NULL, spec[MAXSTRING];

   memset(&gConvExtraInfo, 0, sizeof(ConvExtraInfo));
   memset(&gThreshFillReplaceInfo, 0, sizeof(gThreshFillReplaceInfo));

   gnQuantizingLevels = 222;
   if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"QuantizingLevels")) != NULL) {
      gnQuantizingLevels = atoi(c_ptr);
      if (gnQuantizingLevels < 2 || gnQuantizingLevels > 256) {
         fprintf(stderr, TgLoadString(STID_INVALID_XDEF_RNG_USE_ALT_VAL),
               TOOL_NAME, "QuantizingLevels", c_ptr, 2, 256, 256-maxColors);
         gnQuantizingLevels = 256-maxColors;
      }
   }
   strcpy(bggenToXpmCmd, gszDefBggen);
   if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"BggenToXpm")) != NULL) {
      int count=0;

      UtilStrCpyN(bggenToXpmCmd, sizeof(bggenToXpmCmd), c_ptr);
      for (c_ptr=strstr(bggenToXpmCmd,"%s"); c_ptr!=NULL;
            c_ptr=strstr(++c_ptr,"%s")) {
         count++;
      }
      if (count != 2) {
         sprintf(gszMsgBox, TgLoadString(STID_INVALID_XDEF_USE_ALT_STR),
               TOOL_NAME, "BggenToXpm", bggenToXpmCmd, gszDefBggen);
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
         strcpy(bggenToXpmCmd, gszDefBggen);
      }
   }
   strcpy(bggenToPpm6Cmd, gszDefPpm6Bggen);
   if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"BggenToPpm6")) != NULL) {
      int count=0;

      UtilStrCpyN(bggenToPpm6Cmd, sizeof(bggenToPpm6Cmd), c_ptr);
      for (c_ptr=strstr(bggenToPpm6Cmd,"%s"); c_ptr!=NULL;
            c_ptr=strstr(++c_ptr,"%s")) {
         count++;
      }
      if (count != 2) {
         sprintf(gszMsgBox, TgLoadString(STID_INVALID_XDEF_USE_ALT_STR),
               TOOL_NAME, "BggenToPpm6", bggenToPpm6Cmd, gszDefPpm6Bggen);
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
         strcpy(bggenToPpm6Cmd, gszDefPpm6Bggen);
      }
   }
   strcpy(ppmquantCmd, gszDefPpmquant);
   if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"PpmQuantize")) != NULL) {
      int count=0;

      UtilStrCpyN(ppmquantCmd, sizeof(ppmquantCmd), c_ptr);
      for (c_ptr=strstr(ppmquantCmd,"%s"); c_ptr!=NULL;
            c_ptr=strstr(++c_ptr,"%s")) {
         count++;
      }
      if (count != 1) {
         sprintf(gszMsgBox, TgLoadString(STID_INVALID_XDEF_USE_ALT_STR),
               TOOL_NAME, "PpmQuantize", ppmquantCmd, gszDefPpmquant);
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
         strcpy(ppmquantCmd, gszDefPpmquant);
      }
      for (c_ptr=strstr(ppmquantCmd,"%d"); c_ptr!=NULL;
            c_ptr=strstr(++c_ptr,"%d")) {
         count++;
      }
      if (count != 1) {
         sprintf(gszMsgBox, TgLoadString(STID_INVALID_XDEF_USE_ALT_STR),
               TOOL_NAME, "PpmQuantize", ppmquantCmd, gszDefPpmquant);
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
         strcpy(ppmquantCmd, gszDefPpmquant);
      }
   }
   strcpy(ppmFSquantCmd, gszDefPpmFSquant);
   if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"PpmFSQuantize")) != NULL) {
      int count=0;

      UtilStrCpyN(ppmFSquantCmd, sizeof(ppmFSquantCmd), c_ptr);
      for (c_ptr=strstr(ppmFSquantCmd,"%s"); c_ptr!=NULL;
            c_ptr=strstr(++c_ptr,"%s")) {
         count++;
      }
      if (count != 1) {
         sprintf(gszMsgBox, TgLoadString(STID_INVALID_XDEF_USE_ALT_STR),
               TOOL_NAME, "PpmQuantize", ppmFSquantCmd, gszDefPpmFSquant);
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
         strcpy(ppmFSquantCmd, gszDefPpmFSquant);
      }
      for (c_ptr=strstr(ppmFSquantCmd,"%d"); c_ptr!=NULL;
            c_ptr=strstr(++c_ptr,"%d")) {
         count++;
      }
      if (count != 1) {
         sprintf(gszMsgBox, TgLoadString(STID_INVALID_XDEF_USE_ALT_STR),
               TOOL_NAME, "PpmQuantize", ppmFSquantCmd, gszDefPpmFSquant);
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
         strcpy(ppmFSquantCmd, gszDefPpmFSquant);
      }
   }
   gDefErrorDiffuseLevel.red = gDefErrorDiffuseLevel.green =
         gDefErrorDiffuseLevel.blue = 2;
   if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"DefaultErrorDiffuseLevels")) !=
         NULL) {
      XColor xcolor;
      char *dup_buf=UtilStrDup(c_ptr);

      if (dup_buf == NULL) FailAllocMessage();
      switch (ParseDefaultColorLevels(dup_buf, &xcolor)) {
      case PDCL_OK:
         gDefErrorDiffuseLevel.red = xcolor.red;
         gDefErrorDiffuseLevel.green = xcolor.green;
         gDefErrorDiffuseLevel.blue = xcolor.blue;
         break;
      case PDCL_TOO_LARGE:
         fprintf(stderr, TgLoadString(STID_VAL_TOO_LARGE_IN_XDEF_USE_ALT),
               TOOL_NAME, "DefaultErrorDiffuseLevels", c_ptr, "2 2 2");
         fprintf(stderr, "\n");
         break;
      case PDCL_TOO_SMALL:
         fprintf(stderr, TgLoadString(STID_VAL_TOO_SMALL_IN_XDEF_USE_ALT),
               TOOL_NAME, "DefaultErrorDiffuseLevels", c_ptr, "2 2 2");
         break;
      case PDCL_BAD_FORMAT:
         fprintf(stderr, TgLoadString(STID_INVALID_XDEF_USE_ALT_STR),
               TOOL_NAME, "DefaultErrorDiffuseLevels", c_ptr, "2 2 2");
         break;
      }
      free(dup_buf);
   }
   memset(gaHGBucket, 0, sizeof(gaHGBucket));
   memset(&gTrueColorInfo, 0, sizeof(TrueColorInfo));

   threshFillReplaceEnabled = FALSE;
   if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,
         "EnableThresholdFloodReplaceColor")) != NULL &&
         UtilStrICmp(c_ptr, "true") == 0) {
      threshFillReplaceEnabled = TRUE;
   }
   *spec = '\0';
   if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,
         "FloodReplaceColorThreshold")) != NULL) {
      UtilStrCpyN(spec, sizeof(spec), c_ptr);
      UtilTrimBlanks(spec);
      if (!ParseThreshFillReplaceSpec(spec)) {
         fprintf(stderr, TgLoadString(STID_INVALID_XDEF_USE_ALT_STR),
               TOOL_NAME, "FloodReplaceColorThreshold", spec,
               "15,15,15");
         fprintf(stderr, "\n");
      }
   }
}

/* ----------------------- Menu Functions ----------------------- */

int RefreshImageProcMenu(menu)
   TgMenu *menu;
{
   int ok=TRUE;

   /* Enable Threshold-based Flood and Replace Colors */
   ok &= TgSetMenuItemCheckById(menu, CMDID_TOGGLEFLOODREPLACECOLORTHRESH,
         threshFillReplaceEnabled);

   /* Set Threshold for Flood and Replace Colors */
   ok &= TgEnableMenuItemById(menu, CMDID_SETFLOODREPLACECOLORTHRESH,
         threshFillReplaceEnabled);

   return ok;
}

TgMenu *CreateImageProcMenu(parent_menu, x, y, menu_info, status_str_xlated)
   TgMenu *parent_menu;
   int x, y;
   TgMenuInfo *menu_info;
   int status_str_xlated; /* ignored, always 0 */
{
   TgMenu *menu=TgCreateMenuFromMenuInfo(parent_menu, x, y, menu_info, FALSE);

   if (menu != NULL) {
      if (!RefreshImageProcMenu(menu)) {
         return TgDestroyMenu(menu, TRUE);
      }
      menu->refresh_proc = ((RefreshMenuFunc*)RefreshImageProcMenu);
   }
   return menu;
}

int ImageProcMenu(X, Y, TrackMenubar)
   int  X, Y, TrackMenubar;
{
   int rc=INVALID;
   TgMenu *menu=(imageProcMenuInfo.create_proc)(NULL, X, Y, &imageProcMenuInfo,
         FALSE);

   activeMenu = MENU_IMAGEPROC;
   if (menu != NULL) {
      menu->track_menubar = TrackMenubar;

      rc = TgMenuLoop(menu);
      TgDestroyMenu(menu, TRUE);
   }
   return rc;
}

