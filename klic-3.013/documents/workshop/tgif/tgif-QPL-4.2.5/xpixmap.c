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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/xpixmap.c,v 1.89 2011/05/16 16:22:00 william Exp $
 */

#define _INCLUDE_FROM_XPIXMAP_C_

#include "tgifdefs.h"

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
#include "file.e"
#include "font.e"
#include "grid.e"
#include "hash.e"
#include "http.e"
#include "imgproc.e"
#include "import.e"
#include "mainloop.e"
#include "mainmenu.e"
#include "mark.e"
#include "move.e"
#include "msg.e"
#include "names.e"
#include "obj.e"
#include "page.e"
#include "pattern.e"
#include "pngtrans.e"
#include "ps.e"
#include "raster.e"
#include "rect.e"
#include "select.e"
#include "setup.e"
#include "stretch.e"
#include "strtbl.e"
#include "util.e"
#include "xbitmap.e"
#include "xpixmap.e"
#include "z_intrf.e"

struct BucketRec {
   int pixel, index, color_index;
   char s[40];
};

#define XPM_BUCKETS 67
#define XPM_BUCKET_INC 10

#define TYPE_PPM3 3
#define TYPE_PPM5 5
#define TYPE_PPM6 6

GC xpmGC=NULL;

int newColormapUsed=FALSE;
int allocColorFailed=FALSE;
int myReadTransparentPixmap=FALSE;

int xpmIsArray=FALSE;
long xpmArraySeek=(-1L);

int xpmHasTooManyColorsForPrinting=FALSE;

int hasReadHexString=FALSE;

PngHeaderInfo gPngHeaderInfo;

double rotatedSine[4]={ 0.0, 1.0, 0.0, -1.0 };
double rotatedCosine[4]={ 1.0, 0.0, -1.0, 0.0 };

static Pixmap dummyPixmap=None;

/* do not translate -- program constants */
static char hexValue[]="0123456789abcdef";

static int askForXPmSpec=FALSE;
static int guessXPmBgColor=FALSE;
static int littleEndianPpm6=FALSE;

static struct BucketRec **xpmBucket=NULL;
static int *xpmBucketSize=NULL;
static int *xpmBucketMaxSize=NULL;

static int shownXPmErrorMessage=FALSE;

static TgHash gColorsHashForPrinting;
static int gnColorsHashForPrintingValid=FALSE;

void ResetXPmErrorMessage()
{
   shownXPmErrorMessage = FALSE;
}

void InitXPm()
{
   register int i;
   XGCValues values;
   char *c_ptr;

   dummyPixmap = XCreatePixmap(mainDisplay, mainWindow, 1, 1, mainDepth);

   values.foreground = myFgPixel;
   values.background = myBgPixel;
   values.function = GXcopy;
   values.fill_style = FillSolid;
   xpmGC = XCreateGC(mainDisplay, dummyPixmap,
         GCForeground | GCBackground | GCFunction | GCFillStyle, &values);

   if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"AskForXPmSpec")) != NULL) {
      if (UtilStrICmp(c_ptr, "true") == 0) {
         askForXPmSpec = TRUE;
      } else {
         askForXPmSpec = FALSE;
      }
   }
   guessXPmBgColor = FALSE;
   if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"GuessXPmBgColor"))!=NULL) {
      if (UtilStrICmp(c_ptr, "true") == 0) {
         guessXPmBgColor = TRUE;
      }
   }
   littleEndianPpm6 = FALSE;
   if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"LittleEndianPpm6"))!=NULL) {
      if (UtilStrICmp(c_ptr, "true") == 0) {
         littleEndianPpm6 = TRUE;
      }
   }
   newColormapUsed = FALSE;

   xpmBucket =
         (struct BucketRec **)malloc(XPM_BUCKETS*sizeof(struct BucketRec *));
   xpmBucketSize = (int*)malloc((XPM_BUCKETS+1)*sizeof(int));
   xpmBucketMaxSize = (int*)malloc(XPM_BUCKETS*sizeof(int));
   if (xpmBucket == NULL || xpmBucketSize == NULL || xpmBucketMaxSize == NULL) {
      FailAllocMessage();
   }
   for (i=0; i < XPM_BUCKETS; i++) {
      xpmBucket[i] =
            (struct BucketRec *)malloc(XPM_BUCKET_INC*sizeof(struct BucketRec));
      if (xpmBucket[i] == NULL) FailAllocMessage();
      xpmBucketSize[i] = 0;
      xpmBucketMaxSize[i] = XPM_BUCKET_INC;
   }
   xpmBucketSize[XPM_BUCKETS] = INVALID;

   memset(&gPngHeaderInfo, 0, sizeof(PngHeaderInfo));
}

void CleanUpXPm()
{
   register int i;

   if (xpmGC != NULL) XFreeGC(mainDisplay, xpmGC);
   XFreePixmap(mainDisplay, dummyPixmap);

   askForXPmSpec = FALSE;
   for (i = 0; i < XPM_BUCKETS; i++) free(xpmBucket[i]);
   free(xpmBucket);
   free(xpmBucketSize);
   free(xpmBucketMaxSize);
   xpmBucket = NULL;
   xpmBucketSize = xpmBucketMaxSize = NULL;
}

int IsLinkedJpegObj(obj_ptr)
   struct ObjRec *obj_ptr;
{
   struct XPmRec *xpm_ptr=obj_ptr->detail.xpm;
   int real_type=xpm_ptr->real_type, linked_jpeg=xpm_ptr->linked_jpeg;
   char *jpeg_filename=xpm_ptr->filename;

   /* if the object is a linked JPEG file, will not perform imageproc */
   return (real_type == XPM_JPEG && linked_jpeg && jpeg_filename != NULL);
}

int IsPpmTrueObj(real_type, ppm_data_compress, ppm_data)
   int real_type, ppm_data_compress;
   char *ppm_data;
{
   return (real_type == PPM_TRUE &&
         (ppm_data_compress == PPM_JPEG_COMPRESS ||
         ppm_data_compress == PPM_DATA_DEFLATED) && ppm_data != NULL);
}

int ObjHasIndexedTransPixel(obj_ptr, pn_index)
   struct ObjRec *obj_ptr;
   int *pn_index;
{
   struct XPmRec *xpm_ptr=obj_ptr->detail.xpm;
   int i=0, ncolors=xpm_ptr->ncolors, *pixels=xpm_ptr->pixels;

   switch (xpm_ptr->real_type) {
   case XPM_XPM:
      for (i=0; i < ncolors; i++) {
         if (pixels[i] == (-1)) {
            if (pn_index != NULL) *pn_index = i;
            return TRUE;
         }
      }
      break;
   case XPM_JPEG: break;
   case PPM_TRUE: break;
   }
   return FALSE;
}

int ObjHasTrueColorTransPixel(obj_ptr, puch_trans_color_r, puch_trans_color_g,
      puch_trans_color_b)
   struct ObjRec *obj_ptr;
   unsigned char *puch_trans_color_r, *puch_trans_color_g, *puch_trans_color_b;
{
   struct XPmRec *xpm_ptr=obj_ptr->detail.xpm;

   switch (xpm_ptr->real_type) {
   case XPM_XPM: break;
   case XPM_JPEG: break;
   case PPM_TRUE:
      if (((PRTGIF && !cmdLineOpenDisplay) || fullTrueColorMode) && xpm_ptr->has_transparent_color) {
         if (puch_trans_color_r != NULL) *puch_trans_color_r = xpm_ptr->transparent_color[0];
         if (puch_trans_color_g != NULL) *puch_trans_color_g = xpm_ptr->transparent_color[1];
         if (puch_trans_color_b != NULL) *puch_trans_color_b = xpm_ptr->transparent_color[2];
         return TRUE;
      }
      break;
   }
   return FALSE;
}

int ObjHasTransPixel(obj_ptr)
   struct ObjRec *obj_ptr;
{
   struct XPmRec *xpm_ptr=obj_ptr->detail.xpm;

   switch (xpm_ptr->real_type) {
   case XPM_XPM: return ObjHasIndexedTransPixel(obj_ptr, NULL);
   case XPM_JPEG: break;
   case PPM_TRUE: return ObjHasTrueColorTransPixel(obj_ptr, NULL, NULL, NULL);
   }
   return FALSE;
}

#define xpmpixelhash(X) (((X)==(-1)) ? (XPM_BUCKETS-1) : ((X)%XPM_BUCKETS))

static
int xpmcharhash(chars_per_pixel, color_char)
   int chars_per_pixel;
   char *color_char;
{
   register int i, val=0;

   for (i = 0; i < chars_per_pixel; i++) val = (val<<1)+(int)(color_char[i]);
   return (xpmpixelhash(val));
}

int BuildXPmBuckets(ncolors, pixels, dump_index_to_color_index, chars_per_pixel,
      color_char, pxtii)
   int ncolors, *pixels, *dump_index_to_color_index, chars_per_pixel;
   char *color_char;
   XpmTransIndexInfo *pxtii;
{
   register int *ptr=NULL, i;
   int bucket=0;

   if (xpmBucketSize == NULL) {
      xpmBucket =
            (struct BucketRec **)malloc(XPM_BUCKETS*sizeof(struct BucketRec *));
      xpmBucketSize = (int*)malloc((XPM_BUCKETS+1)*sizeof(int));
      xpmBucketMaxSize = (int*)malloc(XPM_BUCKETS*sizeof(int));
      if (xpmBucket==NULL || xpmBucketSize==NULL || xpmBucketMaxSize==NULL) {
         FailAllocMessage();
      }
      for (i=0; i < XPM_BUCKETS; i++) {
         xpmBucket[i] = (struct BucketRec *)malloc(
               XPM_BUCKET_INC*sizeof(struct BucketRec));
         if (xpmBucket[i] == NULL) FailAllocMessage();
         xpmBucketSize[i] = 0;
         xpmBucketMaxSize[i] = XPM_BUCKET_INC;
      }
      xpmBucketSize[XPM_BUCKETS] = INVALID;
   }

   for (ptr = xpmBucketSize; *ptr != INVALID; ptr++) *ptr = 0;

   if (chars_per_pixel == INVALID) {
      /* build the hash table according to the pixels */
      for (i=0; i < ncolors; i++) {
         if (pxtii != NULL) {
            if (pxtii->has_transparent_pixel && pixels[i] == INVALID) {
#ifdef _TGIF_DBG /* debug, do not translate */
               TgAssert(pxtii->found_transparent_pixel == FALSE ||
                     pxtii->transparent_pixel_index == i,
                     "more than one transparent pixel in BuildXPmBuckets()",
                     NULL);
#endif /* _TGIF_DBG */
               pxtii->found_transparent_pixel = TRUE;
               pxtii->transparent_pixel_index = i;
               if (pxtii->dump_index_to_color_index != NULL) {
                  pxtii->dump_index_to_color_index[i] = INVALID;
               }
            } else if (pxtii->dump_index_to_color_index != NULL) {
               int pixel=pixels[i], found_index=INVALID;

               if (HashLookUpInt(&gColorsHashForPrinting,
                     (char*)(&pixel), sizeof(int), &found_index)) {
                  pxtii->dump_index_to_color_index[i] = found_index;
               } else {
                  pxtii->dump_index_to_color_index[i] = INVALID;
#ifdef _TGIF_DBG /* debug, do not translate */
                  TgAssert(FALSE,
                        "invalid found_index in BuildXPmBuckets()", NULL);
#endif /* _TGIF_DBG */
               }
            }
         }
         bucket = xpmpixelhash(pixels[i]);
         if (xpmBucketSize[bucket] == xpmBucketMaxSize[bucket]) {
            xpmBucket[bucket] = (struct BucketRec *)realloc(xpmBucket[bucket],
                  (xpmBucketMaxSize[bucket]+XPM_BUCKET_INC) *
                  sizeof(struct BucketRec));
            xpmBucketMaxSize[bucket] += XPM_BUCKET_INC;
         }
         xpmBucket[bucket][xpmBucketSize[bucket]].index = i;
         xpmBucket[bucket][xpmBucketSize[bucket]].pixel = pixels[i];
         if (dump_index_to_color_index == NULL) {
            xpmBucket[bucket][xpmBucketSize[bucket]].color_index = BAD;
         } else {
            xpmBucket[bucket][xpmBucketSize[bucket]].color_index =
                  dump_index_to_color_index[i];
         }
         (xpmBucketSize[bucket])++;
      }
   } else {
      /* build the hash table according to the color_char */
      if (chars_per_pixel >= 9) {
         sprintf(gszMsgBox, TgLoadString(STID_INVALID_CH_PER_PIX_IN_FUNC),
               chars_per_pixel, "BuildXPmBuckets()");
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
         return FALSE;
      }
      for (i=0; i < ncolors; i++) {
         bucket = xpmcharhash(chars_per_pixel, &color_char[i*chars_per_pixel]);
         if (xpmBucketSize[bucket] == xpmBucketMaxSize[bucket]) {
            xpmBucket[bucket] = (struct BucketRec *)realloc(xpmBucket[bucket],
                  (xpmBucketMaxSize[bucket]+XPM_BUCKET_INC) *
                  sizeof(struct BucketRec));
            xpmBucketMaxSize[bucket] += XPM_BUCKET_INC;
         }
         xpmBucket[bucket][xpmBucketSize[bucket]].index = i;
         strncpy(xpmBucket[bucket][xpmBucketSize[bucket]].s,
               &color_char[i*chars_per_pixel], chars_per_pixel);
         (xpmBucketSize[bucket])++;
      }
   }
   return TRUE;
}

int XPmLookUp(pixel, chars_per_pixel, color_char, pn_color_index_return)
   int pixel, chars_per_pixel, *pn_color_index_return;
   char *color_char;
{
   register int i;
   register struct BucketRec *ptr;
   int size, bucket;

   if (chars_per_pixel == INVALID) {
      /* hash according to the pixels */
      bucket = xpmpixelhash(pixel);
      size = xpmBucketSize[bucket];
      for (i = 0, ptr = xpmBucket[bucket]; i < size; i++, ptr++) {
         if (ptr->pixel == pixel) {
            if (pn_color_index_return != NULL) {
               *pn_color_index_return = ptr->color_index;
            }
            return (ptr->index);
         }
      }
   } else {
      /* hash according to the color_char */
      bucket = xpmcharhash(chars_per_pixel, color_char);
   
      size = xpmBucketSize[bucket];
      for (i=0, ptr=xpmBucket[bucket]; i < size; i++, ptr++) {
         if (strncmp(color_char, ptr->s, chars_per_pixel) == 0) {
            if (pn_color_index_return != NULL) {
               *pn_color_index_return = ptr->color_index;
            }
            return (ptr->index);
         }
      }
   }
   return INVALID;
}

void MakeCachedPixmap(ObjPtr)
   struct ObjRec *ObjPtr;
{
   register int c, r;
   int w, h, flip, target_percent;
   int num_cols, num_rows, image_w, image_h, watch_cursor;
   int start_col, start_row, do_msg;
   struct XPmRec *xpm_ptr=ObjPtr->detail.xpm;
   struct MtrxRec mtrx;
   Pixmap dest_pixmap=None, dest_bitmap=None;
   XImage *src_image=NULL, *src_bitmap_image=NULL;
   XImage *dest_image=NULL, *dest_bitmap_image=NULL;

   w = ObjPtr->obbox.rbx - ObjPtr->obbox.ltx;
   h = ObjPtr->obbox.rby - ObjPtr->obbox.lty;
   num_cols = (zoomedIn) ? (w<<zoomScale) : (w>>zoomScale);
   num_rows = (zoomedIn) ? (h<<zoomScale) : (h>>zoomScale);

   if (ObjPtr->ctm==NULL && xpm_ptr->cached_pixmap!=None &&
         xpm_ptr->cached_color!=(-1) && xpm_ptr->cached_color==ObjPtr->color &&
         xpm_ptr->cached_zoomed==zoomedIn && xpm_ptr->cached_zoom==zoomScale &&
         xpm_ptr->cached_w==num_cols && xpm_ptr->cached_h==num_rows &&
         xpm_ptr->cached_flip==xpm_ptr->flip)
      return;

   if ((w>>zoomScale)==0 || (h>>zoomScale)==0) {
      if (xpm_ptr->cached_pixmap != None) {
         XFreePixmap(mainDisplay, xpm_ptr->cached_pixmap);
      }
      if (xpm_ptr->cached_bitmap != None) {
         XFreePixmap(mainDisplay, xpm_ptr->cached_bitmap);
      }
      xpm_ptr->cached_pixmap = None;
      xpm_ptr->cached_bitmap = None;
      xpm_ptr->cached_color = (-1);
      return;
   }
   watch_cursor = watchCursorOnMainWindow;
   if (!watch_cursor && !RedrawDuringScrolling()) {
      SetWatchCursor(drawWindow);
      SetWatchCursor(mainWindow);
   } else {
      CheckInterrupt(TRUE);
   }
   src_image = xpm_ptr->image;
   src_bitmap_image = xpm_ptr->bitmap_image;
   flip = xpm_ptr->flip;
   image_w = xpm_ptr->image_w;
   image_h = xpm_ptr->image_h;
   if (xpm_ptr->cached_pixmap != None) {
      XFreePixmap(mainDisplay, xpm_ptr->cached_pixmap);
   }
   xpm_ptr->cached_pixmap = None;
   if (xpm_ptr->cached_bitmap != None) {
      XFreePixmap(mainDisplay, xpm_ptr->cached_bitmap);
   }
   xpm_ptr->cached_bitmap = None;
   xpm_ptr->cached_color = (-1);
   if (xpm_ptr->clip_mask != None) {
      XFreePixmap(mainDisplay, xpm_ptr->clip_mask);
   }
   xpm_ptr->clip_mask = None;

   if (src_image == NULL) {
      src_image = xpm_ptr->image = XGetImage(mainDisplay, xpm_ptr->pixmap,
            0, 0, image_w, image_h, AllPlanes, ZPixmap);
   }
   if (src_bitmap_image == NULL) {
      src_bitmap_image = xpm_ptr->bitmap_image = XGetImage(mainDisplay,
            xpm_ptr->bitmap, 0, 0, image_w, image_h, 1, ZPixmap);
   }
   do_msg = (((num_rows*num_cols)>=0x4000) && !RedrawDuringScrolling());
   if (do_msg) {
      SaveStatusStrings();
      SetStringStatus(TgLoadCachedString(CSTID_CACHING_PIXMAP));
      XSync(mainDisplay, False);
   }
   dest_pixmap = XCreatePixmap(mainDisplay, dummyPixmap, num_cols, num_rows,
         mainDepth);
   dest_bitmap = XCreatePixmap(mainDisplay, dummyBitmap, num_cols, num_rows,
         1);
   XFillRectangle(mainDisplay,dest_pixmap,xpmGC,0,0,num_cols,num_rows);
   XSetForeground(mainDisplay,xbmGC,1);
   XFillRectangle(mainDisplay,dest_bitmap,xbmGC,0,0,num_cols,num_rows);
   XSetForeground(mainDisplay,xbmGC,0);
   dest_image = XGetImage(mainDisplay, dest_pixmap, 0, 0, num_cols, num_rows,
         AllPlanes, ZPixmap);
   dest_bitmap_image = XGetImage(mainDisplay, dest_bitmap, 0, 0,
         num_cols, num_rows, 1, ZPixmap);

   if (ObjPtr->ctm == NULL) {
      mtrx.image_w = (float)image_w; mtrx.image_h = (float)image_h;
      mtrx.w = (float)num_cols; mtrx.h = (float)num_rows;
      mtrx.rotate = ROTATE0; mtrx.flip = flip;

      CalcTransform(&mtrx);

      start_col = (mtrx.transformed_w >= 0.0) ? 0 : (-num_cols)+1;
      start_row = (mtrx.transformed_h >= 0.0) ? 0 : (-num_rows)+1;

      target_percent = 5;
      for (r=0; r < num_rows; r++) {
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
         for (c=0; c < num_cols; c++) {
            double x, y;

            x = part_x+((double)(c+start_col)+0.5)*(mtrx.rev_m[0][0]);
            y = part_y+((double)(c+start_col)+0.5)*(mtrx.rev_m[0][1]);
            if (x >= ((double)0) && x < ((double)image_w) &&
                  y >= ((double)0) && y < ((double)image_h)) {
               int new_x=(int)x, new_y=(int)y;

               if (XGetPixel(src_bitmap_image,new_x,new_y) != 0) {
                  XPutPixel(dest_image, c, r, XGetPixel(src_image,new_x,new_y));
               } else {
                  XPutPixel(dest_bitmap_image, c, r, 0);
                  XPutPixel(dest_image, c, r, colorPixels[ObjPtr->color]);
               }
            }
         }
      }
   } else {
      Pixmap clip_mask;
      XImage *clip_mask_image;
      int bg_pixel=GetDrawingBgPixel(INVALID, INVALID);
      int abs_offset_x=ObjPtr->obbox.ltx-ObjPtr->x;
      int abs_offset_y=ObjPtr->obbox.lty-ObjPtr->y;

      clip_mask = XCreatePixmap(mainDisplay,dummyBitmap,num_cols,num_rows,1);
      XSetForeground(mainDisplay, xbmGC, 1);
      XFillRectangle(mainDisplay, clip_mask, xbmGC, 0, 0, num_cols, num_rows);
      XSetForeground(mainDisplay, xbmGC, 0);
      clip_mask_image = XGetImage(mainDisplay, clip_mask, 0, 0,
            num_cols, num_rows, 1, ZPixmap);

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
            new_dx += (double)(ObjPtr->x-ObjPtr->orig_obbox.ltx);
            new_dy += (double)(ObjPtr->y-ObjPtr->orig_obbox.lty);
            if (new_dx >= ((double)0) && new_dx < ((double)image_w) &&
                  new_dy >= ((double)0) && new_dy < ((double)image_h)) {
               int new_x=(int)new_dx, new_y=(int)new_dy;

               if (new_x < 0) new_x = 0;
               if (new_x >= image_w) new_x = image_w-1;
               if (new_y < 0) new_y = 0;
               if (new_y >= image_h) new_y = image_h-1;
               if (XGetPixel(src_bitmap_image,new_x,new_y) != 0) {
                  XPutPixel(dest_image, c, r, XGetPixel(src_image,new_x,new_y));
               } else {
                  XPutPixel(clip_mask_image, c, r, 0);
                  XPutPixel(dest_image, c, r, bg_pixel);
               }
            } else {
               XPutPixel(clip_mask_image, c, r, 0);
            }
         }
      }
      XPutImage(mainDisplay, clip_mask, xbmGC, clip_mask_image, 0, 0, 0, 0,
            num_cols, num_rows);
      xpm_ptr->clip_mask = clip_mask;
      XDestroyImage(clip_mask_image);
      memcpy(&xpm_ptr->cached_ctm, ObjPtr->ctm, sizeof(struct XfrmMtrxRec));
   }
   if (do_msg) {
      SetStringStatus(TgLoadCachedString(CSTID_FINISHED_CACHEING_PIXMAP));
      XSync(mainDisplay, False);
   }
   XPutImage(mainDisplay, dest_pixmap, xpmGC, dest_image, 0, 0, 0, 0,
         num_cols, num_rows);
   XPutImage(mainDisplay, dest_bitmap, xbmGC, dest_bitmap_image, 0, 0, 0, 0,
         num_cols, num_rows);
   if (do_msg) RestoreStatusStrings();

   xpm_ptr->cached_pixmap = dest_pixmap;
   xpm_ptr->cached_bitmap = dest_bitmap;
   xpm_ptr->cached_zoomed = zoomedIn;
   xpm_ptr->cached_zoom = zoomScale;
   xpm_ptr->cached_flip = xpm_ptr->flip;
   xpm_ptr->cached_w = num_cols;
   xpm_ptr->cached_h = num_rows;
   xpm_ptr->cached_color = ObjPtr->color;

   if (dest_image != NULL) XDestroyImage(dest_image);
   if (dest_bitmap_image != NULL) XDestroyImage(dest_bitmap_image);

   if (!watch_cursor && !RedrawDuringScrolling()) {
      SetDefaultCursor(mainWindow);
      ShowCursor();
   } else {
      CheckInterrupt(TRUE);
   }
}

int ExtractPixmap(orig_pixmap, orig_image, orig_bitmap, orig_bitmap_image,
      x, y, w, h, pixmap, image, bitmap, bitmap_image)
   Pixmap orig_pixmap, orig_bitmap, *pixmap, *bitmap;
   XImage *orig_image, *orig_bitmap_image, **image, **bitmap_image;
   int x, y, w, h;
{
   register int j, i;
   XImage *src_image=NULL, *src_bitmap_image=NULL;

   SetWatchCursor(drawWindow);
   SetWatchCursor(mainWindow);

   *pixmap = XCreatePixmap(mainDisplay, dummyPixmap, w, h, mainDepth);
   *bitmap = XCreatePixmap(mainDisplay, dummyBitmap, w, h, 1);
   *image = (*pixmap==None ? NULL : XGetImage(mainDisplay, *pixmap, 0, 0, w, h,
         AllPlanes, ZPixmap));
   *bitmap_image = (*bitmap==None ? NULL : XGetImage(mainDisplay, *bitmap, 0, 0,
         w, h, 1, ZPixmap));
   if (orig_image != NULL && x == 0 && y == 0) {
      src_image = orig_image;
   } else {
      src_image = XGetImage(mainDisplay, orig_pixmap, x, y, w, h, AllPlanes,
            ZPixmap);
   }
   if (orig_bitmap_image != NULL && x == 0 && y == 0) {
      src_bitmap_image = orig_bitmap_image;
   } else {
      src_bitmap_image = XGetImage(mainDisplay, orig_bitmap, x, y, w, h, 1,
            ZPixmap);
   }
   if (*pixmap == None || *bitmap == None || *image==NULL ||
         *bitmap_image==NULL || src_image==NULL || src_bitmap_image==NULL) {
      if (*pixmap == None) {
         FailAllocPixmapMessage(w, h);
      } else if (*bitmap == None) {
         FailAllocBitmapMessage(w, h);
      } else {
         MsgBox(TgLoadString(STID_XGETIMAGE_MAY_RUN_OUT_VMEM), TOOL_NAME,
               INFO_MB);
      }
      if (*pixmap != None) XFreePixmap(mainDisplay, *pixmap);
      if (*bitmap != None) XFreePixmap(mainDisplay, *bitmap);
      if (*image != NULL) XDestroyImage(*image);
      if (*bitmap_image != NULL) XDestroyImage(*bitmap_image);
      if (orig_image != NULL) XDestroyImage(src_image);
      if (orig_bitmap_image != NULL) XDestroyImage(src_bitmap_image);
      if (!(orig_image != NULL && x==0 && y==0) && src_image != NULL) {
         XDestroyImage(src_image);
      }
      if (!(orig_bitmap_image != NULL && x==0 && y==0) &&
            src_bitmap_image != NULL) {
         XDestroyImage(src_bitmap_image);
      }
      *pixmap = *bitmap = None;
      *image = *bitmap_image = NULL;
      SetDefaultCursor(mainWindow);
      SetDefaultCursor(drawWindow);
      return FALSE;
   }
   for (i = 0; i < h; i++) {
      for (j = 0; j < w; j++) {
         XPutPixel(*image, j, i, XGetPixel(src_image, j, i));
         XPutPixel(*bitmap_image, j, i, XGetPixel(src_bitmap_image, j, i));
      }
   }
   XPutImage(mainDisplay, *pixmap, xpmGC, *image, 0, 0, 0, 0, w, h);
   XPutImage(mainDisplay, *bitmap, xbmGC, *bitmap_image, 0, 0, 0, 0, w, h);
   SetDefaultCursor(mainWindow);
   SetDefaultCursor(drawWindow);

   if (!(orig_image != NULL && x == 0 && y == 0)) XDestroyImage(src_image);
   if (!(orig_bitmap_image != NULL && x == 0 && y == 0)) {
      XDestroyImage(src_bitmap_image);
   }
   return TRUE;
}

struct ObjRec *CutXPixmap(ObjPtrToCut, pAbsX, pAbsY, pAbsW, pAbsH)
   struct ObjRec *ObjPtrToCut; /* if NULL, use topSel->obj */
   int *pAbsX, *pAbsY, *pAbsW, *pAbsH;
   /*
    * if (ObjPtrToCut != NULL) {
    *    return new_obj_ptr if cut was done successfully
    *    return ObjPtrToCut if there is no need to cut anything
    *    return NULL if something is wrong and object needs to be deleted
    * }
    * Note: if ObjPtrToCut is not NULL, then ObjPtrToCut is topSel->obj and
    *         it's called from CropImage() with topSel->obj->ctm != NULL
    *         with fullTrueColorMode == TRUE && HasZlibSupport() == TRUE
    */
{
   register int j, i;
   int w, h, chars_per_pixel, ncolors=0, *pixels=NULL, len;
   int ltx, lty, rbx, rby, new_w, new_h;
   int src_x=0, src_y=0, src_w=0, src_h=0, image_w=0, image_h=0;
   char *color_char, **color_str;
   float h_scale=1.0, v_scale=1.0, mag=1.0;
   Pixmap dest_pixmap=None, dest_bitmap=None;
   XImage *dest_image=NULL, *dest_bitmap_image=NULL;
   struct ObjRec *obj_ptr=topSel->obj, *new_obj_ptr=NULL;
   struct XPmRec *xpm_ptr=obj_ptr->detail.xpm, *new_xpm_ptr=NULL;

   src_x = 0;
   src_y = 0;
   if (ObjPtrToCut != NULL && pAbsX != NULL && pAbsY != NULL && pAbsW != NULL &&
         pAbsH != NULL) {
      obj_ptr = ObjPtrToCut;
      xpm_ptr = obj_ptr->detail.xpm;
      image_w = xpm_ptr->image_w;
      image_h = xpm_ptr->image_h;
      src_x = *pAbsX;
      src_y = *pAbsY;
      src_w = *pAbsW;
      src_h = *pAbsH;
      if (src_x==0 && src_y==0 && src_w==image_w && src_h==image_h &&
            mag==1.0) {
         return obj_ptr;
      }
      if (src_w==0 || src_h==0) {
         MsgBox(TgLoadString(STID_XPM_CANT_HAVE_0_W_OR_H), TOOL_NAME, INFO_MB);
         return NULL;
      }
   } else {
      src_w = image_w = xpm_ptr->image_w;
      src_h = image_h = xpm_ptr->image_h;
      mag = 1.0;

      h_scale = ((float)((float)(obj_ptr->obbox.rbx-obj_ptr->obbox.ltx)) /
            ((float)image_w));
      v_scale = ((float)((float)(obj_ptr->obbox.rby-obj_ptr->obbox.lty)) /
            ((float)image_h));

      if (pAbsX == NULL && pAbsY == NULL && pAbsW == NULL && pAbsH == NULL) {
         char mag_spec[MAXSTRING];

         sprintf(gszMsgBox, TgLoadString(STID_ENTER_GEOM_SPEC_ORIG_SIZE),
               image_w, image_h);
         *mag_spec = '\0';
         Dialog(gszMsgBox, TgLoadCachedString(CSTID_DLG_ACCEPT_CANCEL),
               mag_spec);
         UtilTrimBlanks(mag_spec);
         if (*mag_spec == '\0') return obj_ptr;

         ParseCutSpec(mag_spec, image_w, image_h, &mag, &src_x, &src_y, &src_w,
               &src_h);
      } else {
         src_x = *pAbsX;
         src_y = *pAbsY;
         src_w = *pAbsW;
         src_h = *pAbsH;
      }
      if (src_x==0 && src_y==0 && src_w==image_w && src_h==image_h &&
            mag==1.0) {
         return obj_ptr;
      }
      if (src_w==0 || src_h==0) {
         MsgBox(TgLoadString(STID_XPM_CANT_HAVE_0_W_OR_H), TOOL_NAME, INFO_MB);
         return NULL;
      }
      PrepareToRecord(CMD_REPLACE, topSel, botSel, numObjSelected);
   }
   if (!ExtractPixmap(xpm_ptr->pixmap, xpm_ptr->image, xpm_ptr->bitmap,
         xpm_ptr->bitmap_image, src_x, src_y, src_w, src_h,
         &dest_pixmap, &dest_image, &dest_bitmap, &dest_bitmap_image)) {
      if (ObjPtrToCut == NULL || pAbsX == NULL || pAbsY == NULL ||
            pAbsW == NULL || pAbsH == NULL) {
         AbortPrepareCmd(CMD_REPLACE);
      }
      return NULL;
   }
   if ((xpm_ptr->real_type == XPM_JPEG || xpm_ptr->real_type == PPM_TRUE) &&
         fullTrueColorMode && HasZlibSupport()) {
      char tmp_fname[MAXPATHLENGTH];

      if (MkTempFile(tmp_fname, sizeof(tmp_fname), tmpDir, TOOL_NAME) == NULL) {
         if (ObjPtrToCut == NULL || pAbsX == NULL || pAbsY == NULL ||
               pAbsW == NULL || pAbsH == NULL) {
            AbortPrepareCmd(CMD_REPLACE);
         }
         return NULL;
      }
      if (DumpXImageToPpmFile(dest_image, src_w, src_h, tmp_fname, TRUE)) {
         char deflated_fname[MAXPATHLENGTH];
         unsigned int data_size=0;
         char *ppm_data=NULL;

         snprintf(deflated_fname, sizeof(deflated_fname), "%s.ppm.z",
               tmp_fname);
         ppm_data = ReadFileIntoBuf(deflated_fname, &data_size);
         unlink(deflated_fname);
         unlink(tmp_fname);
         if (ppm_data) {
            new_obj_ptr = CreatePpmTrueObjFromImage(dest_image, src_w, src_h,
                  ppm_data, data_size);
         }
      }
      XFreePixmap(mainDisplay, dest_pixmap);
      XFreePixmap(mainDisplay, dest_bitmap);
      XDestroyImage(dest_image);
      XDestroyImage(dest_bitmap_image);
      dest_pixmap = dest_bitmap = None;
      dest_image = dest_bitmap_image = NULL;

      if (new_obj_ptr == NULL) {
         if (ObjPtrToCut == NULL || pAbsX == NULL || pAbsY == NULL ||
               pAbsW == NULL || pAbsH == NULL) {
            AbortPrepareCmd(CMD_REPLACE);
         }
         return NULL;
      }
      MoveObj(new_obj_ptr, obj_ptr->x, obj_ptr->y);
   }
   sprintf(gszMsgBox, TgLoadCachedString(CSTID_NEW_XPM_SIZE_IS_W_X_H),
         src_w, src_h);
   Msg(gszMsgBox);
   if (ObjPtrToCut != NULL && pAbsX != NULL && pAbsY != NULL && pAbsW != NULL &&
         pAbsH != NULL) {
      return new_obj_ptr;
   } else {
      UnlinkObj(obj_ptr);

      ltx = selLtX; lty = selLtY; rbx = selRbX; rby = selRbY;
      HighLightReverse();

      w = new_w = (int)(((float)src_w) * mag);
      h = new_h = (int)(((float)src_h) * mag);
   }
   if (new_obj_ptr == NULL) {
      new_obj_ptr = (struct ObjRec *)malloc(sizeof(struct ObjRec));
      if (new_obj_ptr == NULL) FailAllocMessage();
      memset(new_obj_ptr, 0, sizeof(struct ObjRec));
      DupObjBasics(obj_ptr, new_obj_ptr);

      new_xpm_ptr = (struct XPmRec *)malloc(sizeof(struct XPmRec));
      if (new_xpm_ptr == NULL) FailAllocMessage();
      memset(new_xpm_ptr, 0, sizeof(struct XPmRec));
      new_obj_ptr->detail.xpm = new_xpm_ptr;

      new_xpm_ptr->image_w = src_w;
      new_xpm_ptr->image_h = src_h;
      new_xpm_ptr->pixmap = dest_pixmap;
      new_xpm_ptr->image = dest_image;
      new_xpm_ptr->bitmap = dest_bitmap;
      new_xpm_ptr->bitmap_image = dest_bitmap_image;
      new_xpm_ptr->data = NULL;
      new_xpm_ptr->fill = xpm_ptr->fill;
      new_xpm_ptr->flip = xpm_ptr->flip;
      new_xpm_ptr->cached_zoom = 0;
      new_xpm_ptr->cached_pixmap = None;
      new_xpm_ptr->cached_bitmap = None;
      new_xpm_ptr->cached_flip = 0;
      new_xpm_ptr->cached_w = 0;
      new_xpm_ptr->cached_h = 0;
      new_xpm_ptr->cached_color = (-1);

      chars_per_pixel = new_xpm_ptr->chars_per_pixel = xpm_ptr->chars_per_pixel;
      new_xpm_ptr->first_pixel_is_bg = xpm_ptr->first_pixel_is_bg;

      ncolors = new_xpm_ptr->ncolors = xpm_ptr->ncolors;
      color_char = new_xpm_ptr->color_char =
            (char*)malloc((ncolors*chars_per_pixel)*sizeof(char));
      if (color_char == NULL) FailAllocMessage();
      color_str = new_xpm_ptr->color_str =
            (char**)malloc(ncolors*sizeof(char*));
      if (color_str == NULL) FailAllocMessage();
      pixels = new_xpm_ptr->pixels = (int*)malloc(ncolors*sizeof(int));
      if (pixels == NULL) FailAllocMessage();
      for (i = 0; i < ncolors; i++) {
         pixels[i] = xpm_ptr->pixels[i];

         for (j = 0; j < chars_per_pixel; j++) {
            color_char[i*chars_per_pixel+j] =
                  xpm_ptr->color_char[i*chars_per_pixel+j];
         }
         len = strlen(xpm_ptr->color_str[i]);
         color_str[i] = (char*)malloc((len+1)*sizeof(char));
         if (color_str[i] == NULL) FailAllocMessage();
         strcpy(color_str[i], xpm_ptr->color_str[i]);
      }
      new_w = round(h_scale * ((float)w));
      new_h = round(v_scale * ((float)h));

      new_obj_ptr->obbox.ltx = obj_ptr->obbox.ltx;
      new_obj_ptr->obbox.lty = obj_ptr->obbox.lty;
      new_obj_ptr->obbox.rbx = new_obj_ptr->bbox.rbx = obj_ptr->obbox.ltx+new_w;
      new_obj_ptr->obbox.rby = new_obj_ptr->bbox.rby = obj_ptr->obbox.lty+new_h;

      AdjObjBBox(new_obj_ptr);
   }
   topSel->obj = botSel->obj = new_obj_ptr;
   AddObj(NULL, topObj, new_obj_ptr);
   if (pAbsX != NULL && pAbsY != NULL) {
      MoveObj(new_obj_ptr, *pAbsX, *pAbsY);
   }
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

   return new_obj_ptr;
}

static
void SaveXPmColors(FP, def_color_index, xpm_ptr, ncolors, chars_per_pixel,
      color_char, color_str, pixels)
   FILE *FP;
   int def_color_index, ncolors, chars_per_pixel, *pixels;
   struct XPmRec *xpm_ptr;
   char *color_char, **color_str;
{
   register int i, j;
   int cur_pixel, found_index;

   if (!colorDisplay && xpm_ptr->red != NULL) {
      for (i=0; i < ncolors; i++) {
         if (fprintf(FP, "   \"") == EOF) writeFileFailed = TRUE;
         for (j=0; j < chars_per_pixel; j++) {
            if (fprintf(FP, "%c", color_char[i*chars_per_pixel+j]) == EOF) {
               writeFileFailed = TRUE;
            }
         }
         if (i == ncolors-1) {
            if (fprintf(FP, "\", \"%s\", %1d, %1d, %1d],[\n", color_str[i],
                  (int)(xpm_ptr->red[i]), (int)(xpm_ptr->green[i]),
                  (int)(xpm_ptr->blue[i])) == EOF) {
               writeFileFailed = TRUE;
            }
         } else {
            if (fprintf(FP, "\", \"%s\", %1d, %1d, %1d,\n", color_str[i],
                  (int)(xpm_ptr->red[i]), (int)(xpm_ptr->green[i]),
                  (int)(xpm_ptr->blue[i])) == EOF) {
               writeFileFailed = TRUE;
            }
         }
      }
   } else {
      for (i=0; i < ncolors; i++) {
         found_index = def_color_index;
         cur_pixel = pixels[i];
         if (cur_pixel != (-1)) {
            for (j=0; j < maxColors; j++) {
               if (colorPixels[j] == cur_pixel) {
                  found_index = j;
                  break;
               }
            }
         }
         if (fprintf(FP, "   \"") == EOF) writeFileFailed = TRUE;
         for (j = 0; j < chars_per_pixel; j++) {
            if (fprintf(FP, "%c", color_char[i*chars_per_pixel+j]) == EOF) {
               writeFileFailed = TRUE;
            }
         }
         if (i == ncolors-1) {
            if (fprintf(FP, "\", \"%s\", %1d, %1d, %1d],[\n", color_str[i],
                  (int)(10000*((int)tgifColors[found_index].red)/maxRGB),
                  (int)(10000*((int)tgifColors[found_index].green)/maxRGB),
                  (int)(10000*((int)tgifColors[found_index].blue)/maxRGB)) ==
                  EOF) {
               writeFileFailed = TRUE;
            }
         } else {
            if (fprintf(FP, "\", \"%s\", %1d, %1d, %1d,\n", color_str[i],
                  (int)(10000*((int)tgifColors[found_index].red)/maxRGB),
                  (int)(10000*((int)tgifColors[found_index].green)/maxRGB),
                  (int)(10000*((int)tgifColors[found_index].blue)/maxRGB)) ==
                  EOF) {
               writeFileFailed = TRUE;
            }
         }
      }
   }
}

static
void PrTgifDumpOldXPmObj(FP, ObjPtr, pmtrx)
   FILE *FP;
   struct ObjRec *ObjPtr;
   struct MtrxRec *pmtrx;
{
   int row=0, i=0, index=0, col=0, j=0, num_nibbles=0, nibble_count=0;
   int ltx=0, lty=0, rbx=0, rby=0, w=0, h=0, image_w=0, image_h=0;
   int ncolors=0, *pixels=NULL, flip=0, orig_x=0, orig_y=0;
   int chars_per_pixel=0, x=0, y=0;
   int h_blocks=0, v_blocks=0, block_w=0, block_h=0, bit_count=0, data=0;
   char *xpm_data=NULL, bg_char[2];
   Pixmap pixmap, bitmap=(Pixmap)0;
   struct XPmRec *xpm_ptr=NULL;

   bg_char[0] = bg_char[1] = '\0';

   ltx = ObjPtr->obbox.ltx;
   lty = ObjPtr->obbox.lty;
   rbx = ObjPtr->obbox.rbx;
   rby = ObjPtr->obbox.rby;

   xpm_ptr = ObjPtr->detail.xpm;

   pixmap = xpm_ptr->pixmap;
   bitmap = xpm_ptr->bitmap;
   pixels = xpm_ptr->pixels;
   ncolors = xpm_ptr->ncolors;
   flip = xpm_ptr->flip;
   image_w = xpm_ptr->image_w;
   image_h = xpm_ptr->image_h;

   w = rbx - ltx;
   h = rby - lty;

   orig_x = (pmtrx->transformed_w >= 0.0) ? ltx : ltx+w;
   orig_y = (pmtrx->transformed_h >= 0.0) ? lty : lty+h;

   xpm_data = xpm_ptr->data;
   chars_per_pixel = xpm_ptr->chars_per_pixel;

   fprintf(stderr, "%s\n", TgLoadString(STID_WARN_XPM_ALL_NON_BG_BE_BLACK));
   switch (chars_per_pixel) {
   case 1:
      bg_char[0] = xpm_ptr->color_char[0];
      bg_char[1] = '\0';
      if (*bg_char != '`' && *bg_char != ' ') {
         bg_char[0] = bg_char[1] = '\0';
      }
      break;
   case 2:
      bg_char[0] = xpm_ptr->color_char[0];
      bg_char[1] = xpm_ptr->color_char[1];
      if (!(bg_char[0] == '`' && bg_char[1] == '`') &&
            !(bg_char[0] == ' ' && bg_char[1] == ' ')) {
         bg_char[0] = '\0';
      }
      break;
   }

   h_blocks = ((image_w&0xff) == 0) ? (image_w>>8) : ((image_w>>8)+1);
   v_blocks = ((image_h&0xff) == 0) ? (image_h>>8) : ((image_h>>8)+1);

   fprintf(FP, "%s\n", gPsCmd[PS_GSAVE]);
   fprintf(FP, "   %1d %1d %s %.3f %.3f %s %1d %s\n\n",
         orig_x, orig_y, gPsCmd[PS_TRANSLATE],
         pmtrx->dump_h_scale, pmtrx->dump_v_scale, gPsCmd[PS_SCALE],
         pmtrx->degree, gPsCmd[PS_ROTATE]);

   for (row=0; row < v_blocks; row++) {
      y = row<<8;
      block_h = (row == v_blocks-1) ? image_h-y : 0x100;

      for (col=0; col < h_blocks; col++) {
         x = col<<8;
         block_w = (col == h_blocks-1) ? image_w-x : 0x100;

         num_nibbles = ((block_w%4) == 0) ? (int)(block_w>>2) :
               (int)(block_w>>2)+1;

         fprintf(FP, "   %s\n", gPsCmd[PS_GSAVE]);
         fprintf(FP, "   %1d %1d %s\n", x, y, gPsCmd[PS_TRANSLATE]);
         fprintf(FP, "   %1d %1d true [1 0 0 1 0 0]\n   {<", block_w,
               block_h);

         nibble_count = 0;
         for (i=0; i < block_h; i++) {
            bit_count = 0;
            data = 0;
            for (j=0; j < block_w; j++) {
               switch (chars_per_pixel) {
               case 1:
                  data = (xpm_data[(i+y)*image_w+j+x] != *bg_char) ?
                        (data<<1) | 1 : (data<<1);
                  break;
               case 2:
                  index = ((i+y)*image_w+j+x)*chars_per_pixel;
                  data = (xpm_data[index] == bg_char[0] &&
                        xpm_data[index+1] == bg_char[1]) ?
                        (data<<1) : (data<<1) | 1;
                  break;
               }
               if (++bit_count == 4) {
                  if (nibble_count++ == 64) {
                     nibble_count = 1;
                     fprintf(FP, "\n     ");
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
                  fprintf(FP, "\n     ");
               }
               fprintf(FP, "%c", hexValue[data]);
            }
            if ((num_nibbles & 0x1) == 1) {
               if (nibble_count++ == 64) {
                  nibble_count = 1;
                  fprintf(FP, "\n     ");
               }
               fprintf(FP, "0");
            }
         }
         fprintf(FP, ">}\n");
         fprintf(FP, "   %s\n", gPsCmd[PS_IMAGEMASK]);
         fprintf(FP, "   %s\n", gPsCmd[PS_GRESTORE]);
         if (row!=v_blocks-1 || col!=h_blocks-1) fprintf(FP, "\n");
      }
   }
   fprintf(FP, "%s\n", gPsCmd[PS_GRESTORE]);
   fprintf(FP, "\n");
}

static
int MalformedPpmMessage(tmp_fname)
   char *tmp_fname;
{
   sprintf(gszMsgBox, TgLoadString(STID_CANT_IMPORT_GIVEN_PPM_MALFORM),
         tmp_fname);
   if (PRTGIF) {
      fprintf(stderr, "%s\n", gszMsgBox);
   } else {
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
   }
   return FALSE;
}

static
int ReadPpmHeaderStr(ppm_fp, tmp_fname, buf, sz_buf)
   FILE *ppm_fp;
   char *tmp_fname, *buf;
   int sz_buf;
{
   int i=0, write_index=0;

   for (i=0; i < sz_buf-1; i++) {
      char ch='\0';

      if (fread(&ch, sizeof(char), 1, ppm_fp) != 1) {
         /* failed */
         break;
      }
      if (i == 0 && ch == '#') {
         /* skip a comment line */
         char *line_buf=UtilGetALine(ppm_fp);

         if (line_buf != NULL) {
            UtilFree(line_buf);
            i = (-1);
         } else {
            break;
         }
      } else if (ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r') {
         if (write_index != 0) {
            buf[write_index++] = '\0';
            return TRUE;
         }
      } else {
         buf[write_index++] = ch;
      }
   }
   return MalformedPpmMessage(tmp_fname);
}

static
int ReadPpmHeader(ppm_fp, tmp_fname, pn_format, pn_w, pn_h, pn_max_val)
   FILE *ppm_fp;
   char *tmp_fname;
   int *pn_format, *pn_w, *pn_h, *pn_max_val;
{
   char buf[MAXSTRING];

   if (!ReadPpmHeaderStr(ppm_fp, tmp_fname, buf, sizeof(buf))) {
      return FALSE;
   }
   if (strcmp(buf, "P3") == 0) {
      if (pn_format != NULL) *pn_format = TYPE_PPM3;
   } else if (strcmp(buf, "P6") == 0) {
      if (pn_format != NULL) *pn_format = TYPE_PPM6;
   } else if (strcmp(buf, "P5") == 0) {
      if (pn_format != NULL) *pn_format = TYPE_PPM5;
   } else {
      return MalformedPpmMessage(tmp_fname);
   }
   if (!ReadPpmHeaderStr(ppm_fp, tmp_fname, buf, sizeof(buf))) {
      return FALSE;
   }
   if (sscanf(buf, "%d", pn_w) != 1) {
      return MalformedPpmMessage(tmp_fname);
   }
   if (!ReadPpmHeaderStr(ppm_fp, tmp_fname, buf, sizeof(buf))) {
      return FALSE;
   }
   if (sscanf(buf, "%d", pn_h) != 1) {
      return MalformedPpmMessage(tmp_fname);
   }
   if (!ReadPpmHeaderStr(ppm_fp, tmp_fname, buf, sizeof(buf))) {
      return FALSE;
   }
   if (sscanf(buf, "%d", pn_max_val) != 1) {
      return MalformedPpmMessage(tmp_fname);
   }
   return TRUE;
}

static
void DumpPpm6File(FP, image_w, image_h, tmp_fname)
   FILE *FP;
   int image_w, image_h;
   char *tmp_fname;
{
   FILE *ppm_fp=fopen(tmp_fname, "r");
   int format=0, w=0, h=0, max_val=0, row=0, col=0;

   if (ppm_fp == NULL) {
      FailToOpenMessage(tmp_fname, "r", NULL);
      return;
   }
   if (!ReadPpmHeader(ppm_fp, tmp_fname, &format, &w, &h, &max_val)) {
      fclose(ppm_fp);
      return;
   }
   if (w != image_w || h != image_h) {
      sprintf(gszMsgBox, TgLoadString(STID_PPM6_DIM_NOT_MATCH), w, h, tmp_fname,
            image_w, image_h);
      if (PRTGIF) {
         fprintf(stderr, "%s\n", gszMsgBox);
      } else {
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      }
      fclose(ppm_fp);
      return;
   }
   if (max_val != 255 && max_val != 65535) {
      sprintf(gszMsgBox, TgLoadString(STID_UNSUP_PPM6_MAX_VAL), max_val,
            tmp_fname);
      if (PRTGIF) {
         fprintf(stderr, "%s\n", gszMsgBox);
      } else {
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      }
      fclose(ppm_fp);
      return;
   }
   for (row=0; row < image_h; row++) {
      for (col=0; col < image_w; col++) {
         unsigned char buf[4];

         if (format == TYPE_PPM6) {
            if (max_val == 65535) {
               unsigned char r_buf[2], g_buf[2], b_buf[2];

               if (fread(r_buf, sizeof(char), 2, ppm_fp) != 2 ||
                     fread(g_buf, sizeof(char), 2, ppm_fp) != 2 ||
                     fread(b_buf, sizeof(char), 2, ppm_fp) != 2) {
                  MalformedPpmMessage(tmp_fname);
                  fclose(ppm_fp);
                  return;
               }
               if (littleEndianPpm6) {
                  buf[0] = r_buf[1];
                  buf[1] = g_buf[1];
                  buf[2] = b_buf[1];
               } else {
                  buf[0] = r_buf[0];
                  buf[1] = g_buf[0];
                  buf[2] = b_buf[0];
               }
            } else {
               if (fread(buf, sizeof(char), 3, ppm_fp) != 3) {
                  MalformedPpmMessage(tmp_fname);
                  fclose(ppm_fp);
                  return;
               }
            }
         } else if (format == TYPE_PPM5) {
            if (max_val == 65535) {
               unsigned char g_buf[2];

               if (fread(g_buf, sizeof(char), 2, ppm_fp) != 2) {
                  MalformedPpmMessage(tmp_fname);
                  fclose(ppm_fp);
                  return;
               }
               if (littleEndianPpm6) {
                  buf[0] = buf[1] = buf[2] = g_buf[1];
               } else {
                  buf[0] = buf[1] = buf[2] = g_buf[0];
               }
            } else {
               if (fread(buf, sizeof(char), 1, ppm_fp) != 1) {
                  MalformedPpmMessage(tmp_fname);
                  fclose(ppm_fp);
                  return;
               }
               buf[1] = buf[2] = buf[0];
            }
         } else {
            int red=0, green=0, blue=0;

            if (fscanf(FP, "%d %d %d", &red, &green, &blue) != 3) {
               MalformedPpmMessage(tmp_fname);
               fclose(ppm_fp);
               return;
            }
	    buf[0] = (unsigned char)(red & 0x0ff);
            buf[1] = (unsigned char)(green & 0x0ff);
            buf[2] = (unsigned char)(blue & 0x0ff);
         }
         buf[3] = '\0';
         if (!colorDump) {
            double dr=(double)0, dg=(double)0, db=(double)0, dgray=(double)0;
            int igray=0;

            dr = (double)(unsigned int)(buf[0]);
            dg = (double)(unsigned int)(buf[1]);
            db = (double)(unsigned int)(buf[2]);
            dgray = (0.299*dr)+(0.587*dg)+(0.114*db);
            igray = round(dgray);
            if (igray > 255) igray = 255;
            if (igray < 0) igray = 0;
            buf[0] = buf[1] = buf[2] = (unsigned char)(igray & 0x0ff);
         }
         fprintf(FP, "%02x%02x%02x", (unsigned int)(buf[0]),
               (unsigned int)(buf[1]), (unsigned int)(buf[2]));
         if (col%12 == 11) fprintf(FP, "\n   ");
      }
      if (col%12 != 11) fprintf(FP, "\n");
      if (row != image_h-1) fprintf(FP, "   ");
   }
   fclose(ppm_fp);
}

static
int ObjHasXpmObj(ObjPtr)
   struct ObjRec *ObjPtr;
{
   switch (ObjPtr->type) {
   case OBJ_XPM:
      if (!colorLayers || ObjInVisibleLayer(ObjPtr)) {
         return TRUE;
      }
      return FALSE;

   case OBJ_SYM:
   case OBJ_ICON:
   case OBJ_GROUP:
      if (!colorLayers || ObjInVisibleLayer(ObjPtr)) {
         struct ObjRec *obj_ptr=ObjPtr->detail.r->last;

         for ( ; obj_ptr != NULL; obj_ptr=obj_ptr->prev) {
            obj_ptr->tmp_parent = ObjPtr;
            if (ObjHasXpmObj(obj_ptr)) return TRUE;
         }
         return TRUE;
      }
      return FALSE;
   case OBJ_PIN:
      if (!colorLayers || ObjInVisibleLayer(ObjPtr)) {
         struct ObjRec *obj_ptr=GetPinObj(ObjPtr);

         for ( ; obj_ptr != NULL; obj_ptr=obj_ptr->prev) {
            obj_ptr->tmp_parent = ObjPtr;
            if (ObjHasXpmObj(obj_ptr)) return TRUE;
         }
         return TRUE;
      }
      return FALSE;

   default: break;
   }
   return FALSE;
}

static
int PageHasXpmObj(page_ptr)
   struct PageRec *page_ptr;
{
   struct ObjRec *obj_ptr=NULL;

   for (obj_ptr=page_ptr->bot; obj_ptr != NULL; obj_ptr=obj_ptr->prev) {
      obj_ptr->tmp_parent = NULL;
      if (ObjHasXpmObj(obj_ptr)) return TRUE;
   }
   return FALSE;
}

static
int FileHasXpmObj()
{
   struct PageRec *page_ptr=NULL;

   for (page_ptr=firstPage; page_ptr != NULL; page_ptr=page_ptr->next) {
      if (PageHasXpmObj(page_ptr)) return TRUE;
   }
   return FALSE;
}

static
void HashAllColorPixels()
{
   int i=0;

   for (i=0; i < maxColors; i++) {
      int pixel=colorPixels[i];

      HashStoreInt(&gColorsHashForPrinting, (char*)(&pixel), sizeof(int), i);
   }
}

void CleanUpCachedColorsForPrinting()
{
   if (gnColorsHashForPrintingValid) {
      CleanUpHash(&gColorsHashForPrinting);
      gnColorsHashForPrintingValid = FALSE;
   }
}

void CacheColorsForPrinting()
{
   XColor *saved_tgif_colors=tgifColors;

   if (PRTGIF && !cmdLineOpenDisplay) return;
   if (!FileHasXpmObj()) return;

   gnColorsHashForPrintingValid = TRUE;
   InitHash(&gColorsHashForPrinting, TG_HASH_SIZE_LARGE);

   if (printUsingRequestedColor) tgifColors = tgifRequestedColors;

   HashAllColorPixels();

   if (printUsingRequestedColor) tgifColors = saved_tgif_colors;
}

typedef struct tagImagePixelInfo {
   int image_w, image_h;
   int **image_pixels;
   /*
    * transparent is not used for now because the only to create a pixmap
    *       object with too many colors is to import a XPM object
    * we assume that people just use JPEG and GIF import
    */
   unsigned char **transparent;
} ImagePixelInfo;

static
void FreeImagePixels(pipi)
   ImagePixelInfo *pipi;
{
   int row=0, image_h=pipi->image_h;
   int **image_pixels=NULL;
   unsigned char **transparent=NULL;

   if (pipi == NULL) return;

   image_pixels = pipi->image_pixels;
   transparent = pipi->transparent;

   if (image_pixels != NULL) {
      for (row=0; row < image_h; row++) {
         if (image_pixels[row] != NULL) free(image_pixels[row]);
      }
      free(image_pixels);
   }
   if (transparent != NULL) {
      for (row=0; row < image_h; row++) {
         if (transparent[row] != NULL) free(transparent[row]);
      }
      free(transparent);
   }
   memset(pipi, 0, sizeof(ImagePixelInfo));
}

static
void SetAColorByteInfoByXpmRGB(pcb, xpm_ptr, pixel_index)
   ColorBytes *pcb;
   struct XPmRec *xpm_ptr;
   int pixel_index;
{
   double red=0, green=0, blue=0;
   int r=0, g=0, b=0;

   red = ((double)255) * ((double)(xpm_ptr->red[pixel_index])) /
         ((double)10000.0);
   green = ((double)255) * ((double)(xpm_ptr->green[pixel_index])) /
         ((double)10000.0);
   blue = ((double)255) * ((double)(xpm_ptr->blue[pixel_index])) /
         ((double)10000.0);

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
void SetAColorByteByXpmRGB(pcb, xpm_ptr, index, table_size)
   ColorBytes *pcb;
   struct XPmRec *xpm_ptr;
   int index, table_size;
{
#ifdef _TGIF_DBG /* debug, do not translate */
   TgAssert(index >= 0 && index < table_size,
         "index out of range in SetAColorByteByXpmRGB()", NULL);
#endif /* _TGIF_DBG */
   if (pcb[index].valid) return;

   SetAColorByteInfoByXpmRGB(&pcb[index], xpm_ptr, index);
}

static
void SetAColorByteByMaxRGB(pcb, xpm_color_index, global_color_index, table_size)
   ColorBytes *pcb;
   int xpm_color_index, global_color_index, table_size;
{
#ifdef _TGIF_DBG /* debug, do not translate */
   TgAssert(xpm_color_index >= 0 && xpm_color_index < table_size,
         "xpm_color_index out of range in SetAColorByteByXpmRGB()", NULL);
#endif /* _TGIF_DBG */
   if (pcb[xpm_color_index].valid) return;

   SetAColorByteInfo(&pcb[xpm_color_index], &tgifColors[global_color_index]);
}

int ConvertPpmTrueToPpm6(ppm_data, ppm_data_size, ppm_data_compress,
      tmp_ppm6_fname, tmp_ppm6_sz)
   char *ppm_data, *tmp_ppm6_fname;
   int ppm_data_size, ppm_data_compress, tmp_ppm6_sz;
{
   *tmp_ppm6_fname = '\0';
   if (ppm_data_compress == PPM_JPEG_COMPRESS) {
      char jpeg_fname[MAXPATHLENGTH+1];

      if (!WriteBufIntoTmpFile(ppm_data, ppm_data_size, jpeg_fname,
            sizeof(jpeg_fname))) {
         return FALSE;
      }
      if (!ConvertJpegToPpm6(jpeg_fname, tmp_ppm6_fname, tmp_ppm6_sz)) {
         unlink(jpeg_fname);
         return FALSE;
      }
      unlink(jpeg_fname);
   } else if (ppm_data_compress == PPM_DATA_DEFLATED) {
      FILE *fp=NULL;
      int rc=0;

      if (MkTempFile(tmp_ppm6_fname, tmp_ppm6_sz, tmpDir, TOOL_NAME) == NULL) {
         return FALSE;
      }
      if ((fp=fopen(tmp_ppm6_fname, "w")) == NULL) {
         FailToOpenMessage(tmp_ppm6_fname, "w", NULL);
         return FALSE;
      }
      if (!DoInflate(ppm_data, ppm_data_size, fp, FALSE, &rc)) {
         fclose(fp);
         unlink(tmp_ppm6_fname);
         ZlibError(rc, FALSE);
         return FALSE;
      }
      fclose(fp);
   }
   return TRUE;
}
   
static
int GetImagePixels(ObjPtr, xpm_ptr, pipi, pxtii, pcb)
   struct ObjRec *ObjPtr;
   struct XPmRec *xpm_ptr;
   ImagePixelInfo *pipi;
   XpmTransIndexInfo *pxtii;
   ColorBytes *pcb;
   /*
    * can only get here if the object has too many colors and colorDump
    * also, this object canont have transparent pixels
    */
{
   int row=0, image_w=xpm_ptr->image_w, image_h=xpm_ptr->image_h;
   int chars_per_pixel=0, pixel_index=0;
   int ncolors=0;
   char *xpm_data=NULL, *color_char=NULL;
   Pixmap pixmap=(Pixmap)0, bitmap=(Pixmap)0;
   XImage *image=NULL, *bitmap_image=NULL;
 
   memset(pipi, 0, sizeof(ImagePixelInfo));
   if (image_h == 0) return FALSE;

   pixmap = xpm_ptr->pixmap;
   bitmap = xpm_ptr->bitmap;

   if ((!PRTGIF || cmdLineOpenDisplay) &&
         (image=xpm_ptr->image) == NULL) {
      if ((image=xpm_ptr->image=XGetImage(mainDisplay, pixmap, 0, 0,
            image_w, image_h, AllPlanes, ZPixmap)) == NULL) {
         Msg(TgLoadString(STID_XGETIMAGE_MAY_RUN_OUT_VMEM));
         return FALSE;
      }
   }
   if ((!PRTGIF || cmdLineOpenDisplay) &&
         (bitmap_image=xpm_ptr->bitmap_image) == NULL) {
      if ((bitmap_image=xpm_ptr->bitmap_image=XGetImage(mainDisplay, bitmap,
            0, 0, image_w, image_h, 1, ZPixmap)) == NULL) {
         Msg(TgLoadString(STID_XGETIMAGE_MAY_RUN_OUT_VMEM));
         return FALSE;
      }
   }
   pipi->image_pixels = (int**)malloc(image_h*sizeof(int*));
   pipi->transparent =
         (unsigned char **)malloc(image_h*sizeof(unsigned char *));
   if (pipi->image_pixels == NULL || pipi->transparent == NULL) {
      FailAllocMessage();
   }
   memset(pipi->image_pixels, 0, image_h*sizeof(int*));
   memset(pipi->transparent, 0, image_h*sizeof(unsigned char *));
   for (row=0; row < image_h; row++) {
      pipi->image_pixels[row] = (int*)malloc(image_w*sizeof(int));
      pipi->transparent[row] =
            (unsigned char *)malloc(image_w*sizeof(unsigned char));
      if (pipi->image_pixels[row] == NULL || pipi->transparent[row] == NULL) {
         FailAllocMessage();
      }
      memset(pipi->image_pixels[row], 0, image_w*sizeof(int));
      memset(pipi->transparent[row], 0, image_w*sizeof(unsigned char));
   }
   ncolors = xpm_ptr->ncolors;

   xpm_data = xpm_ptr->data;
   chars_per_pixel = xpm_ptr->chars_per_pixel;
   color_char = xpm_ptr->color_char;

   for (row=0; row < image_h; row++) {
      int col=0;

      for (col=0; col < image_w; col++) {
         int transparent_pixel=INVALID;
#ifdef NOT_DEFINED
         float gray=(float)0;
         int value=0;
#endif /* NOT_DEFINED */

         if (PRTGIF && !cmdLineOpenDisplay) {
            /* this works! */
            pixel_index = XPmLookUp(INVALID, chars_per_pixel,
                  &(xpm_data[(row*image_w+col)*chars_per_pixel]), NULL);
            SetAColorByteByXpmRGB(pcb, xpm_ptr, pixel_index, ncolors+3);

            if (!pxtii->has_transparent_pixel) {
               if (!colorDump) {
                  /* should never get here */
#ifdef NOT_DEFINED
                  gray = 0.299*((float)xpm_ptr->red[pixel_index]/10000.0) +
                        0.587*((float)xpm_ptr->green[pixel_index]/10000.0) +
                        0.114*((float)xpm_ptr->blue[pixel_index]/10000.0);
                  value = gray*256;
                  pixel_index = ((value>255) ? 255 : ((value<0) ? 0 : value));
#endif /* NOT_DEFINED */
               } else {
                  if (pxtii->has_transparent_pixel &&
                          pxtii->found_transparent_pixel &&
                          pxtii->transparent_pixel_index == pixel_index) {
                     pipi->transparent[row][col] = TRUE;
                  }
               }
            } else {
               /* should never get here */
               if (pxtii->has_transparent_pixel &&
                       pxtii->found_transparent_pixel &&
                       pxtii->transparent_pixel_index == pixel_index) {
                  pipi->transparent[row][col] = TRUE;
               }
            }
         } else {
            if (colorDump || pxtii->has_transparent_pixel) {
               if (!colorDisplay && xpm_ptr->red != NULL) {
                  pixel_index = XPmLookUp(INVALID, chars_per_pixel,
                        &(xpm_data[(row*image_w+col)*chars_per_pixel]), NULL);
                  SetAColorByteByXpmRGB(pcb, xpm_ptr, pixel_index, ncolors+3);
               } else if (XGetPixel(bitmap_image,col,row) != 0) {
                  /* this works! */
                  int pixel=XGetPixel(image,col,row), global_color_index=(-1);

                  pixel_index = XPmLookUp(pixel, INVALID, NULL, NULL);
                  if (HashLookUpInt(&gColorsHashForPrinting,
                        (char*)(&pixel), sizeof(int), &global_color_index)) {
                     SetAColorByteByMaxRGB(pcb, pixel_index, global_color_index,
                           ncolors+3);
                  } else {
                  }
               } else {
                  /* transparent pixel */
                  pixel_index = XPmLookUp((-1), INVALID, NULL, NULL);
                  transparent_pixel = pixel_index;
               }
               if (pxtii->has_transparent_pixel &&
                       pxtii->found_transparent_pixel &&
                       pxtii->transparent_pixel_index == pixel_index) {
                  pipi->transparent[row][col] = TRUE;
               }
            } else {
               /* should never get here */
#ifdef NOT_DEFINED
               if (!colorDisplay && xpm_ptr->red != NULL) {
                  pixel_index = XPmLookUp(INVALID, chars_per_pixel,
                        &(xpm_data[(row*image_w+col)*chars_per_pixel]), NULL);
                  gray = 0.299*((float)xpm_ptr->red[pixel_index]/10000.0) +
                        0.587*((float)xpm_ptr->green[pixel_index]/10000.0) +
                        0.114*((float)xpm_ptr->blue[pixel_index]/10000.0);
               } else {
                  int cur_pixel=0, found_index=0;

                  if (XGetPixel(bitmap_image, col, row) != 0) {
                     cur_pixel = XGetPixel(image, col, row);
                  } else {
                     found_index = ObjPtr->color;
                     cur_pixel = (-1);
                  }
                  if (cur_pixel != (-1)) {
                     int i=0;

                     for (i=0; i < maxColors; i++) {
                        if (colorPixels[i] == cur_pixel) {
                           found_index = i;
                           break;
                        }
                     }
                  }
                  gray = 0.299*((float)tgifColors[found_index].red/maxRGB) +
                        0.587*((float)tgifColors[found_index].green/maxRGB) +
                        0.114*((float)tgifColors[found_index].blue/maxRGB);
                  SetAColorByteByMaxRGB(pcb, pixel_index, ncolors+3);
               }
               value = gray*256;
               pixel_index = ((value>255) ? 255 : ((value<0) ? 0 : value));
#endif /* NOT_DEFINED */
            }
         }
         pipi->image_pixels[row][col] = pixel_index;
      }
   }
   return TRUE;
}

static
int PrepareColorsForConvertObjToPpm6(xpm_ptr, pxtii)
   struct XPmRec *xpm_ptr;
   XpmTransIndexInfo *pxtii;
{
   int index=0, ncolors=xpm_ptr->ncolors;
   int chars_per_pixel=xpm_ptr->chars_per_pixel;
   char *color_char=xpm_ptr->color_char;
   int *pixels=xpm_ptr->pixels;

   if (colorDump) {
      if ((PRTGIF && !cmdLineOpenDisplay) ||
            (!colorDisplay && xpm_ptr->red != NULL)) {
         if (!BuildXPmBuckets(ncolors, NULL, NULL, chars_per_pixel,
               color_char, pxtii)) {
            return FALSE;
         }
         for (index=0; index < ncolors; index++) {
            if (xpm_ptr->red[index] == (-1)) {
               if (pxtii->has_transparent_pixel) {
                  if (pxtii->found_transparent_pixel) {
#ifdef _TGIF_DBG /* debug, do not translate */
                     TgAssert(FALSE,
                           "Found transparent pixel again in PrepareColorsForConvertObjToPpm6()",
                           NULL);
#endif /* _TGIF_DBG */
                  } else { 
                     pxtii->found_transparent_pixel = TRUE;
                     pxtii->transparent_pixel_index = index;
                  }
               }
            }
         }
      } else {
         /* interactive printing */
         pxtii->dump_index_to_color_index =
               (int*)malloc((maxColors+3)*sizeof(int));
         if (pxtii->dump_index_to_color_index == NULL) FailAllocMessage();
         for (index=0; index < maxColors+3; index++) {
            pxtii->dump_index_to_color_index[index] = INVALID;
         }
         if (!BuildXPmBuckets(ncolors, pixels,
               pxtii->dump_index_to_color_index, INVALID, NULL, pxtii)) {
            return FALSE;
         }
      }
   } else if ((PRTGIF && !cmdLineOpenDisplay) ||
         (!colorDisplay && xpm_ptr->red != NULL)) {
      if (!BuildXPmBuckets(ncolors, NULL, NULL, chars_per_pixel, color_char,
            pxtii)) {
         return FALSE;
      }
   } else if (pxtii->has_transparent_pixel) {
      /*
       * should never get here since such object is not suppose to have
       *         transparent pixels!
       */
      pxtii->dump_index_to_color_index =
            (int*)malloc((maxColors+3)*sizeof(int));
      if (pxtii->dump_index_to_color_index == NULL) FailAllocMessage();
      for (index=0; index < maxColors+3; index++) {
         pxtii->dump_index_to_color_index[index] = INVALID;
      }
      if (!BuildXPmBuckets(ncolors, pixels, pxtii->dump_index_to_color_index,
            INVALID, NULL, pxtii)) {
         return FALSE;
      }
   }
   return TRUE;
}

static
int ConvertObjToPpm6(ObjPtr, xpm_ptr, psz_ppm6_path, ppm6_path_sz, pxtii)
   struct ObjRec *ObjPtr;
   struct XPmRec *xpm_ptr;
   char *psz_ppm6_path;
   int ppm6_path_sz;
   XpmTransIndexInfo *pxtii;
{
   FILE *ppm_fp=NULL;
   int row=0, image_w=xpm_ptr->image_w, image_h=xpm_ptr->image_h, pixel_index=0;
   int ncolors=xpm_ptr->ncolors;
   XColor *saved_tgif_colors=tgifColors;
   ColorBytes *pcb=(ColorBytes*)malloc((ncolors+3)*sizeof(ColorBytes));
   ImagePixelInfo ipi;

   if (!PrepareColorsForConvertObjToPpm6(xpm_ptr, pxtii)) return FALSE;

   /*
    * pcb maps an object color index to an RGB value
    */
   if (pcb == NULL) FailAllocMessage();
   memset(pcb, 0, (ncolors+3)*sizeof(ColorBytes));
   if (printUsingRequestedColor) tgifColors = tgifRequestedColors;

   if (MkTempFile(psz_ppm6_path, ppm6_path_sz, tmpDir, TOOL_NAME) == NULL) {
      free(pcb);
      if (printUsingRequestedColor) tgifColors = saved_tgif_colors;
      return FALSE;
   }
   if ((ppm_fp=fopen(psz_ppm6_path, "w")) == NULL) {
      free(pcb);
      if (printUsingRequestedColor) tgifColors = saved_tgif_colors;
      return FailToOpenMessage(psz_ppm6_path, "w", NULL);
   }
   memset(&ipi, 0, sizeof(ImagePixelInfo));

   if (!GetImagePixels(ObjPtr, xpm_ptr, &ipi, pxtii, pcb)) {
      free(pcb);
      if (printUsingRequestedColor) tgifColors = saved_tgif_colors;
      return FALSE;
   }
   writeFileFailed = FALSE;

   fprintf(ppm_fp, "P6\n%1d %1d\n255\n", image_w, image_h);
   for (row=0; row < image_h; row++) {
      int col=0;

      for (col=0; col < image_w; col++) {
         pixel_index = ipi.image_pixels[row][col];

         if (fprintf(ppm_fp, "%c%c%c", pcb[pixel_index].r, pcb[pixel_index].g,
               pcb[pixel_index].b) == EOF) {
            writeFileFailed = TRUE;
         }
      }
   }
   fclose(ppm_fp);
   FreeImagePixels(&ipi);

   if (writeFileFailed) {
      FailToWriteFileMessage(psz_ppm6_path);
   }
   if (printUsingRequestedColor) tgifColors = saved_tgif_colors;
   free(pcb);

   return TRUE;
}

void DumpXPmObj(FP, ObjPtr)
   FILE *FP;
   struct ObjRec *ObjPtr;
{
   int row=0, i=0, index=0, col=0, too_many_colors=FALSE;
   int ltx=0, lty=0, rbx=0, rby=0, w=0, h=0, image_w=0, image_h=0;
   int ncolors=0, *pixels=NULL, flip=0, orig_x=0, orig_y=0;
   int cur_pixel=0, chars_per_pixel=0, x=0, y=0, found_index=0, pixel_index=0;
   int has_transparent_pixel=FALSE, is_linked_jpeg=IsLinkedJpegObj(ObjPtr);
   unsigned int trans_color_pixel_r=0, trans_color_pixel_g=0, trans_color_pixel_b=0;
   int is_ppm_true=FALSE;
   char *xpm_data=NULL, *color_char=NULL;
   Pixmap pixmap=(Pixmap)0, bitmap=(Pixmap)0;
   XImage *image=NULL, *bitmap_image=NULL;
   struct XPmRec *xpm_ptr=NULL;
   struct MtrxRec mtrx;
   XpmTransIndexInfo xtii;

   memset(&mtrx, 0, sizeof(struct MtrxRec));
   memset(&xtii, 0, sizeof(XpmTransIndexInfo));

   ltx = ObjPtr->obbox.ltx;
   lty = ObjPtr->obbox.lty;
   rbx = ObjPtr->obbox.rbx;
   rby = ObjPtr->obbox.rby;

   xpm_ptr = ObjPtr->detail.xpm;

   pixmap = xpm_ptr->pixmap;
   bitmap = xpm_ptr->bitmap;
   pixels = xpm_ptr->pixels;
   ncolors = xpm_ptr->ncolors;
   flip = xpm_ptr->flip;
   image_w = xpm_ptr->image_w;
   image_h = xpm_ptr->image_h;

   w = rbx - ltx;
   h = rby - lty;

   is_ppm_true = IsPpmTrueObj(xpm_ptr->real_type, xpm_ptr->ppm_data_compress,
         xpm_ptr->ppm_data);

   if ((!PRTGIF || cmdLineOpenDisplay) &&
         (image=xpm_ptr->image) == NULL) {
      if ((image=xpm_ptr->image=XGetImage(mainDisplay, pixmap, 0, 0,
            image_w, image_h, AllPlanes, ZPixmap)) == NULL) {
         Msg(TgLoadString(STID_XGETIMAGE_MAY_RUN_OUT_VMEM));
         return;
      }
   }
   if ((!PRTGIF || cmdLineOpenDisplay) &&
         (bitmap_image=xpm_ptr->bitmap_image) == NULL) {
      if ((bitmap_image=xpm_ptr->bitmap_image=XGetImage(mainDisplay, bitmap,
            0, 0, image_w, image_h, 1, ZPixmap)) == NULL) {
         Msg(TgLoadString(STID_XGETIMAGE_MAY_RUN_OUT_VMEM));
         return;
      }
   }
   mtrx.image_w = (float)image_w; mtrx.image_h = (float)image_h;
   mtrx.w = (float)w; mtrx.h = (float)h;
   mtrx.rotate = ROTATE0; mtrx.flip = flip;

   CalcTransform(&mtrx);

   orig_x = (mtrx.transformed_w >= 0.0) ? ltx : ltx+w;
   orig_y = (mtrx.transformed_h >= 0.0) ? lty : lty+h;

   fprintf(FP, "%% XPM\n");

   xpm_data = xpm_ptr->data;
   chars_per_pixel = xpm_ptr->chars_per_pixel;
   color_char = xpm_ptr->color_char;

   if (PRTGIF && !cmdLineOpenDisplay && fileVersion < 25) {
      /* can't print color with PRTGIF, treat it like a bitmap */
      PrTgifDumpOldXPmObj(FP, ObjPtr, &mtrx);
      return;
   }
   if ((PRTGIF && !cmdLineOpenDisplay) ||
         (!colorDisplay && xpm_ptr->red != NULL)) {
      if (xpm_ptr->real_type == PPM_TRUE && xpm_ptr->has_transparent_color) {
         has_transparent_pixel = TRUE;
         trans_color_pixel_r = (unsigned int)xpm_ptr->transparent_color[0];
         trans_color_pixel_g = (unsigned int)xpm_ptr->transparent_color[1];
         trans_color_pixel_b = (unsigned int)xpm_ptr->transparent_color[2];
      } else {
         int *red_ptr=xpm_ptr->red;

         if (ncolors > 0xff) {
            too_many_colors = TRUE;
         }
         for (index = 0; index < ncolors; index++) {
            if (red_ptr[index] == (-1)) {
               has_transparent_pixel = TRUE;
               break;
            }
         }
      }
   } else {
      if (fullTrueColorMode && xpm_ptr->real_type == PPM_TRUE &&
            xpm_ptr->has_transparent_color) {
         has_transparent_pixel = TRUE;
         trans_color_pixel_r = (unsigned int)xpm_ptr->transparent_color[0];
         trans_color_pixel_g = (unsigned int)xpm_ptr->transparent_color[1];
         trans_color_pixel_b = (unsigned int)xpm_ptr->transparent_color[2];
      } else {
         if (ncolors > 0xff) {
            too_many_colors = TRUE;
         }
         for (index = 0; index < ncolors; index++) {
            if (pixels[index] == (-1)) {
               has_transparent_pixel = TRUE;
               break;
            }
         }
      }
   }
   if (has_transparent_pixel) {
      xtii.has_transparent_pixel = TRUE;
      if (((PRTGIF && !cmdLineOpenDisplay) || fullTrueColorMode) &&
            xpm_ptr->real_type == PPM_TRUE && xpm_ptr->has_transparent_color) {
      } else if (too_many_colors && colorDump) {
         sprintf(gszMsgBox, TgLoadString(STID_TRANS_XPM_TOO_MANY_COLORS_SKP),
               ncolors);
         if (PRTGIF) {
            fprintf(stderr, "%s\n", gszMsgBox);
         } else {
            Msg(gszMsgBox);
         }
         xpmHasTooManyColorsForPrinting = TRUE;
         return;
      }
   }
   x = (mtrx.transformed_w >= 0.0) ? ltx : ltx+w;
   y = (mtrx.transformed_h >= 0.0) ? lty : lty+h;

   fprintf(FP, "%s\n", gPsCmd[PS_GSAVE]);
   if (is_linked_jpeg) {
      if (!ConvertJpegToPpm6(xpm_ptr->filename, xpm_ptr->tmp_ppm6_fname,
            sizeof(xpm_ptr->tmp_ppm6_fname))) {
         return;
      }
   } else if (is_ppm_true) {
      if (!ConvertPpmTrueToPpm6(xpm_ptr->ppm_data, xpm_ptr->ppm_data_size,
            xpm_ptr->ppm_data_compress, xpm_ptr->tmp_ppm6_fname,
            sizeof(xpm_ptr->tmp_ppm6_fname))) {
         return;
      }
   } else if (too_many_colors && colorDump) {
      if (!ConvertObjToPpm6(ObjPtr, xpm_ptr, xpm_ptr->tmp_ppm6_fname,
            sizeof(xpm_ptr->tmp_ppm6_fname), &xtii)) {
         return;
      }
   } else if (colorDump) {
      fprintf(FP, "   3 %1d %s %s tgifsetpixels\n", ncolors,
            gPsCmd[PS_MUL], gPsCmd[PS_ARRAY]);

      if ((PRTGIF && !cmdLineOpenDisplay) ||
            (!colorDisplay && xpm_ptr->red != NULL)) {
         if (!BuildXPmBuckets(ncolors, NULL, NULL, chars_per_pixel,
               color_char, &xtii)) {
            return;
         }
         for (index = 0; index < ncolors; index++) {
            if (xpm_ptr->red[index] == (-1)) {
               fprintf(FP, "%s%3d [ -1.00 -1.00 -1.00 ] tgifsetpix",
                     ((index & 0x1) ? "" : "   "), index*3);
            } else {
               fprintf(FP, "%s%3d [ %.3f %.3f %.3f ] tgifsetpix",
                     ((index & 0x1) ? "" : "   "), index*3,
                     ((float)xpm_ptr->red[index])/((float)10000.0),
                     ((float)xpm_ptr->green[index])/((float)10000.0),
                     ((float)xpm_ptr->blue[index])/((float)10000.0));
            }
            fprintf(FP, "%s", ((index & 0x1) ? "\n" : " "));
         }
      } else {
         if (!BuildXPmBuckets(ncolors, pixels, NULL, INVALID, NULL, &xtii)) {
            return;
         }
         for (index = 0; index < ncolors; index++) {
            cur_pixel = pixels[index];
            if (cur_pixel != (-1)) {
               found_index = ObjPtr->color;
               for (i = 0; i < maxColors; i++) {
                  if (colorPixels[i] == cur_pixel) {
                     found_index = i;
                     break;
                  }
               }
               fprintf(FP, "%s%3d [ %.3f %.3f %.3f ] tgifsetpix",
                     ((index & 0x1) ? "" : "   "), index*3,
                     ((float)tgifColors[found_index].red/maxRGB),
                     ((float)tgifColors[found_index].green/maxRGB),
                     ((float)tgifColors[found_index].blue/maxRGB));
            } else {
               fprintf(FP, "%s%3d [ -1.00 -1.00 -1.00 ] tgifsetpix",
                     ((index & 0x1) ? "" : "   "), index*3);
            }
            fprintf(FP, "%s", ((index & 0x1) ? "\n" : " "));
         }
      }
      if (ncolors & 0x1) fprintf(FP, "\n");
   } else if ((PRTGIF && !cmdLineOpenDisplay) ||
         (!colorDisplay && xpm_ptr->red != NULL)) {
      if (!BuildXPmBuckets(ncolors, NULL, NULL, chars_per_pixel, color_char,
            &xtii)) {
         return;
      }
      if (has_transparent_pixel) {
         fprintf(FP, "   3 %1d %s %s tgifsetpixels\n", ncolors,
               gPsCmd[PS_MUL], gPsCmd[PS_ARRAY]);

         for (index = 0; index < ncolors; index++) {
            if (xpm_ptr->red[index] == (-1)) {
               fprintf(FP, "%s%3d [ -1.00 -1.00 -1.00 ] tgifsetpix",
                     ((index & 0x1) ? "" : "   "), index*3);
            } else {
               float gray;

               gray = 0.299*((float)xpm_ptr->red[index]/10000.0) +
                     0.587*((float)xpm_ptr->green[index]/10000.0) +
                     0.114*((float)xpm_ptr->blue[index]/10000.0);
               fprintf(FP, "%s%3d [ %.3f %.3f %.3f ] tgifsetpix",
                     ((index & 0x1) ? "" : "   "), index*3,
                     gray, gray, gray);
            }
            fprintf(FP, "%s", ((index & 0x1) ? "\n" : " "));
         }
      }
   } else if (has_transparent_pixel) {
      if (!BuildXPmBuckets(ncolors, pixels, NULL, INVALID, NULL, &xtii)) {
         return;
      }
      fprintf(FP, "   3 %1d %s %s tgifsetpixels\n", ncolors,
            gPsCmd[PS_MUL], gPsCmd[PS_ARRAY]);

      for (index = 0; index < ncolors; index++) {
         cur_pixel = pixels[index];
         if (cur_pixel != (-1)) {
            float gray;

            found_index = ObjPtr->color;
            for (i = 0; i < maxColors; i++) {
               if (colorPixels[i] == cur_pixel) {
                  found_index = i;
                  break;
               }
            }
            gray = 0.299*((float)tgifColors[found_index].red)/((float)maxRGB) +
                  0.587*((float)tgifColors[found_index].green)/((float)maxRGB) +
                  0.114*((float)tgifColors[found_index].blue)/((float)maxRGB);
            fprintf(FP, "%s%3d [ %.3f %.3f %.3f ] tgifsetpix",
                  ((index & 0x1) ? "" : "   "), index*3,
                  gray, gray, gray);
         } else {
            fprintf(FP, "%s%3d [ -1.00 -1.00 -1.00 ] tgifsetpix",
                  ((index & 0x1) ? "" : "   "), index*3);
         }
         fprintf(FP, "%s", ((index & 0x1) ? "\n" : " "));
      }
   } else {
      /* grayscale */
   }
   if (((PRTGIF && !cmdLineOpenDisplay) || fullTrueColorMode) &&
         xpm_ptr->real_type == PPM_TRUE && xpm_ptr->has_transparent_color) {
      fprintf(FP, "   %1d %1d %1d tgifsetppmtruetranspix\n",
            trans_color_pixel_r, trans_color_pixel_g, trans_color_pixel_b);
   }
   if (ObjPtr->ctm == NULL) {
      fprintf(FP, "   %1d %1d %s %.3f %.3f %s %1d %s\n", x, y,
            gPsCmd[PS_TRANSLATE], mtrx.dump_h_scale, mtrx.dump_v_scale,
            gPsCmd[PS_SCALE], mtrx.degree, gPsCmd[PS_ROTATE]);
   } else {
      float m[6];

      m[CTM_SX] = ((float)ObjPtr->ctm->m[CTM_SX])/((float)1000.0);
      m[CTM_SY] = ((float)ObjPtr->ctm->m[CTM_SY])/((float)1000.0);
      m[CTM_SIN] = ((float)ObjPtr->ctm->m[CTM_SIN])/((float)1000.0);
      m[CTM_MSIN] = ((float)ObjPtr->ctm->m[CTM_MSIN])/((float)1000.0);
      fprintf(FP, "   %1d %1d %s\n", ObjPtr->x, ObjPtr->y,
            gPsCmd[PS_TRANSLATE]);
      fprintf(FP, "   [%.3f %.3f %.3f %.3f %1d %1d] %s\n",
            m[CTM_SX], m[CTM_SIN], m[CTM_MSIN], m[CTM_SY],
            ObjPtr->ctm->t[CTM_TX], ObjPtr->ctm->t[CTM_TY], gPsCmd[PS_CONCAT]);
   }
   fprintf(FP, "   %1d %1d 8\n   [1 0 0 1 0 0]\n", image_w, image_h);

   if (preDumpSetup) {
      PSUseColorImage();
      hasReadHexString = TRUE;
   }
   if (is_linked_jpeg || is_ppm_true || (too_many_colors && colorDump)) {
      if (((PRTGIF && !cmdLineOpenDisplay) || fullTrueColorMode) &&
            xpm_ptr->real_type == PPM_TRUE && xpm_ptr->has_transparent_color) {
         fprintf(FP, "   { }\n");
         fprintf(FP, "   false 3 tgiftransppmtruecolorimage\n   ");
      } else {
         fprintf(FP, "   {currentfile\n");
         fprintf(FP, "    tgifbwpicstr readhexstring pop}\n");
         fprintf(FP, "   false 3 colorimage\n   ");
      }
   } else if (colorDump) {
      fprintf(FP, "   {currentfile\n");
      fprintf(FP, "    tgifbwpicstr readhexstring pop 0 get tgifcolorspot}\n");
      if (has_transparent_pixel) {
         fprintf(FP, "   false 3 tgiftranscolorimage\n   ");
      } else {
         fprintf(FP, "   false 3 colorimage\n   ");
      }
   } else {
      /* grayscale */
      if (has_transparent_pixel) {
         fprintf(FP, "   {currentfile\n");
         fprintf(FP, "    %s}\n",
               "tgifbwpicstr readhexstring pop 0 get tgifcolorspot");
         fprintf(FP, "   false 3 tgiftranscolorimage\n   ");
      } else {
         fprintf(FP, "   {currentfile\n");
         fprintf(FP, "    tgifbwpicstr readhexstring pop}\n");
         fprintf(FP, "   %s\n   ", gPsCmd[PS_IMAGE]);
      }
   }
   /*
    * Transparent_pixel is dealt with a little bit differently here.
    * When generating colors for a particular pixel, if the pixel is
    *       suppose to be transparent, the color components are all
    *       (-1).  The postscript code will take care of it and not
    *       put down any ink.
    * Whether we are doing colorDump or not doesn't matter here.
    */
   if (is_linked_jpeg || is_ppm_true || (too_many_colors && colorDump)) {
      DumpPpm6File(FP, image_w, image_h, xpm_ptr->tmp_ppm6_fname);
      unlink(xpm_ptr->tmp_ppm6_fname);
      *xpm_ptr->tmp_ppm6_fname = '\0';
      if (xtii.dump_index_to_color_index != NULL) {
         free(xtii.dump_index_to_color_index);
      }
   } else if (PRTGIF && !cmdLineOpenDisplay) {
      for (row=0; row < image_h; row++) {
         for (col=0; col < image_w; col++) {
            float gray;
            int value;

            pixel_index = XPmLookUp(INVALID, chars_per_pixel,
                  &(xpm_data[(row*image_w+col)*chars_per_pixel]), NULL);

            if (!has_transparent_pixel) {
               if (!colorDump) {
                  gray = 0.299*((float)xpm_ptr->red[pixel_index]/10000.0) +
                        0.587*((float)xpm_ptr->green[pixel_index]/10000.0) +
                        0.114*((float)xpm_ptr->blue[pixel_index]/10000.0);
                  value = gray*256;
                  pixel_index = ((value>255) ? 255 : ((value<0) ? 0 : value));
               }
            }
            fprintf(FP, "%c", hexValue[(pixel_index>>4)&0x0f]);
            fprintf(FP, "%c", hexValue[pixel_index&0x0f]);
            if (col%36 == 35) fprintf(FP, "\n   ");
         }
         if (col%36 != 0) fprintf(FP, "\n");
         if (row != image_h-1) fprintf(FP, "   ");
      }
   } else {
      for (row=0; row < image_h; row++) {
         for (col=0; col < image_w; col++) {
            if (colorDump || has_transparent_pixel) {
               if (!colorDisplay && xpm_ptr->red != NULL) {
                  pixel_index = XPmLookUp(INVALID, chars_per_pixel,
                        &(xpm_data[(row*image_w+col)*chars_per_pixel]), NULL);
               } else if (XGetPixel(bitmap_image,col,row) != 0) {
                  pixel_index = XPmLookUp(XGetPixel(image,col,row), INVALID,
                        NULL, NULL);
               } else {
                  pixel_index = XPmLookUp((-1), INVALID, NULL, NULL);
               }
            } else {
               float gray;
               int value;

               if (!colorDisplay && xpm_ptr->red != NULL) {
                  pixel_index = XPmLookUp(INVALID, chars_per_pixel,
                        &(xpm_data[(row*image_w+col)*chars_per_pixel]), NULL);
                  gray = 0.299*((float)xpm_ptr->red[pixel_index]/10000.0) +
                        0.587*((float)xpm_ptr->green[pixel_index]/10000.0) +
                        0.114*((float)xpm_ptr->blue[pixel_index]/10000.0);
               } else {
                  if (XGetPixel(bitmap_image, col, row) != 0) {
                     cur_pixel = XGetPixel(image, col, row);
                  } else {
                     found_index = ObjPtr->color;
                     cur_pixel = (-1);
                  }
                  if (cur_pixel != (-1)) {
                     for (i = 0; i < maxColors; i++) {
                        if (colorPixels[i] == cur_pixel) {
                           found_index = i;
                           break;
                        }
                     }
                  }
                  gray = 0.299*((float)tgifColors[found_index].red/maxRGB) +
                        0.587*((float)tgifColors[found_index].green/maxRGB) +
                        0.114*((float)tgifColors[found_index].blue/maxRGB);
               }
               value = gray*256;
               pixel_index = ((value>255) ? 255 : ((value<0) ? 0 : value));
            }
            fprintf(FP, "%c", hexValue[(pixel_index>>4)&0x0f]);
            fprintf(FP, "%c", hexValue[pixel_index&0x0f]);
            if (col%36 == 35) {
               fprintf(FP, "\n%s",
                     (col==image_w-1 && row==image_h-1) ? "" : "   ");
            }
         }
         if (col%36 != 0) fprintf(FP, "\n");
         if (row != image_h-1 && (col%36) != 0) fprintf(FP, "   ");
      }
   }
   fprintf(FP, "%s\n", gPsCmd[PS_GRESTORE]);
   fprintf(FP, "\n");
}

int NeedsToCacheXPmObj(ObjPtr)
   struct ObjRec *ObjPtr;
{
   register struct XPmRec *xpm_ptr=ObjPtr->detail.xpm;
   int w, h;

   w = ObjPtr->obbox.rbx - ObjPtr->obbox.ltx;
   h = ObjPtr->obbox.rby - ObjPtr->obbox.lty;

   return (ObjPtr->ctm != NULL ||
         zoomScale != 0 || xpm_ptr->image_w != w || xpm_ptr->image_h != h ||
         xpm_ptr->flip != 0);
}

static
void DrawHiddenXPm(win, ctm, vs, x, y, w, h, s)
   Window win;
   struct XfrmMtrxRec *ctm;
   XPoint *vs;
   int x, y, w, h;
   char *s;
{
   int str_w, len, sx, sy;
   XGCValues values;

   values.foreground = GetDrawingBgPixel(INVALID, INVALID);
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
      XFillPolygon(mainDisplay, win, drawGC, vs, 5, Convex, CoordModeOrigin);
   } else {
      XFillRectangle(mainDisplay, win, drawGC, x, y, w, h);
   }
   XSetForeground(mainDisplay, drawGC, GetDrawingFgPixel(INVALID, INVALID));
   if (ctm != NULL) {
      XDrawLines(mainDisplay, win, drawGC, vs, 5, CoordModeOrigin);
   } else {
      XDrawRectangle(mainDisplay, win, drawGC, x, y, w, h);
   }
   XSetForeground(mainDisplay, drawGC, colorPixels[colorIndex]);

   len = strlen(s);
   str_w = rulerFontWidth*len;
   if (str_w < w && rulerFontHeight < h) {
      sx = x + ((w-str_w)>>1);
      sy = y + ((h-rulerFontHeight)>>1);
      XDrawString(mainDisplay, win, drawGC, sx, sy+rulerFontAsc, s, len);
   }
   XSetFont(mainDisplay, drawGC, canvasFontPtr->fid);
}

void RecolorXPmObj(obj_ptr, color_index)
   struct ObjRec *obj_ptr;
   int color_index;
{
   register int c, r;
   struct XPmRec *xpm_ptr=obj_ptr->detail.xpm;
   int image_w=xpm_ptr->image_w, image_h=xpm_ptr->image_h;
   Pixmap pixmap=None;
   XImage *image=NULL;

   xpm_ptr->cached_color = (-1);
   if (xpm_ptr->bitmap_image == NULL) {
      xpm_ptr->bitmap_image = XGetImage(mainDisplay, xpm_ptr->bitmap,
            0, 0, image_w, image_h, 1, ZPixmap);
      if (xpm_ptr->bitmap_image == NULL) return;
   }
   if (xpm_ptr->image == NULL) {
      xpm_ptr->image = XGetImage(mainDisplay, xpm_ptr->pixmap,
            0, 0, image_w, image_h, AllPlanes, ZPixmap);
      if (xpm_ptr->image == NULL) return;
   }
   pixmap = XCreatePixmap(mainDisplay, dummyPixmap, image_w, image_h,
         mainDepth);
   if (pixmap == None) return;
   XFillRectangle(mainDisplay, pixmap, xpmGC, 0, 0, image_w, image_h);
   image = XGetImage(mainDisplay, pixmap, 0, 0, image_w, image_h,
         AllPlanes, ZPixmap);
   if (image == NULL) {
      XFreePixmap(mainDisplay, pixmap);
      return;
   }
   for (r=0; r < image_h; r++) {
      for (c=0; c < image_w; c++) {
         if (XGetPixel(xpm_ptr->bitmap_image, c, r) != 0) {
            XPutPixel(image, c, r, XGetPixel(xpm_ptr->image, c, r));
         } else {
            XPutPixel(image, c, r, colorPixels[color_index]);
         }
      }
   }
   XPutImage(mainDisplay, pixmap, xpmGC, image, 0, 0, 0, 0, image_w, image_h);
   XFreePixmap(mainDisplay, xpm_ptr->pixmap);
   XDestroyImage(xpm_ptr->image);
   xpm_ptr->pixmap = pixmap;
   xpm_ptr->image = image;
   if (obj_ptr->ctm != NULL) {
      MakeCachedPixmap(obj_ptr);
   }
}

void DrawXPmObj(win, XOff, YOff, ObjPtr)
   Window win;
   int XOff, YOff;
   struct ObjRec *ObjPtr;
{
   int ltx, lty, rbx, rby, w, h, scr_w, scr_h, real_x_off, real_y_off;
   char s[80];
   struct XPmRec *xpm_ptr=ObjPtr->detail.xpm;
   XGCValues values;

   if (userDisableRedraw) return;

   if (ObjPtr->prev != NULL && ObjPtr->prev->type == OBJ_XPM &&
         Inside(ObjPtr->obbox, ObjPtr->prev->obbox) &&
         ObjPtr->prev->detail.xpm->bitmap == None) {
      return;
   }
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

   if (!mapShown) {
      sprintf(s, "(%1dx%1d)", xpm_ptr->image_w, xpm_ptr->image_h);
      DrawHiddenXPm(win, ObjPtr->ctm, ObjPtr->rotated_obbox,
            ltx, lty, scr_w, scr_h, s);
      return;
   }
   if (NeedsToCacheXPmObj(ObjPtr) &&
         (ObjPtr->ctm == NULL ||
         (ObjPtr->ctm != NULL && (xpm_ptr->cached_pixmap == None ||
         xpm_ptr->clip_mask == None)))) {
      MakeCachedPixmap(ObjPtr);
   }
   XSetFunction(mainDisplay, drawGC, GXcopy);
   if (ObjPtr->ctm==NULL && zoomScale==0 &&
         xpm_ptr->flip==0 && xpm_ptr->image_w==w && xpm_ptr->image_h==h) {
      if (xpm_ptr->bitmap == None) {
         XCopyArea(mainDisplay, xpm_ptr->pixmap, win, drawGC, 0, 0, w, h,
               ltx, lty);
      } else {
         values.function = GXcopy;
         values.clip_x_origin = ltx;
         values.clip_y_origin = lty;
         values.clip_mask = xpm_ptr->bitmap;
         XChangeGC(mainDisplay, drawGC, GCFunction |
               GCClipXOrigin | GCClipYOrigin | GCClipMask, &values);

         DrawClippedPixmap(xpm_ptr->pixmap, win, drawGC, scr_w, scr_h,
               ltx, lty);

         values.clip_x_origin = 0;
         values.clip_y_origin = 0;
         values.clip_mask = None;
         XChangeGC(mainDisplay, drawGC,
               GCClipXOrigin | GCClipYOrigin | GCClipMask, &values);
         if (numClipRecs > 0) {
            XSetClipRectangles(mainDisplay, drawGC, 0, 0, clipRecs,
               numClipRecs, clipOrdering);
         }
      }
   } else {
      if (xpm_ptr->cached_pixmap == None) return;
      if (ObjPtr->ctm == NULL) {
         if (xpm_ptr->cached_bitmap == None) {
            XCopyArea(mainDisplay, xpm_ptr->cached_pixmap, win, drawGC, 0, 0,
                  scr_w, scr_h, ltx, lty);
         } else {
            values.function = GXcopy;
            values.clip_x_origin = ltx;
            values.clip_y_origin = lty;
            values.clip_mask = xpm_ptr->cached_bitmap;
            XChangeGC(mainDisplay, drawGC, GCFunction |
                  GCClipXOrigin | GCClipYOrigin | GCClipMask, &values);

            DrawClippedPixmap(xpm_ptr->cached_pixmap, win, drawGC, scr_w, scr_h,
                  ltx, lty);

            values.clip_x_origin = 0;
            values.clip_y_origin = 0;
            values.clip_mask = None;
            XChangeGC(mainDisplay, drawGC,
                  GCClipXOrigin | GCClipYOrigin | GCClipMask, &values);
            if (numClipRecs > 0) {
               XSetClipRectangles(mainDisplay, drawGC, 0, 0, clipRecs,
                  numClipRecs, clipOrdering);
            }
         }
      } else if (xpm_ptr->clip_mask == None) {
         XCopyArea(mainDisplay, xpm_ptr->cached_pixmap, win, drawGC, 0, 0,
               scr_w, scr_h, ltx, lty);
      } else {
         values.function = GXcopy;
         values.clip_x_origin = ltx;
         values.clip_y_origin = lty;
         values.clip_mask = xpm_ptr->clip_mask;
         XChangeGC(mainDisplay, drawGC, GCFunction |
               GCClipXOrigin | GCClipYOrigin | GCClipMask, &values);

         DrawClippedPixmap(xpm_ptr->cached_pixmap, win, drawGC, scr_w, scr_h,
               ltx, lty);

         values.clip_x_origin = 0;
         values.clip_y_origin = 0;
         values.clip_mask = None;
         XChangeGC(mainDisplay, drawGC,
               GCClipXOrigin | GCClipYOrigin | GCClipMask, &values);
         if (numClipRecs > 0) {
            XSetClipRectangles(mainDisplay, drawGC, 0, 0, clipRecs,
               numClipRecs, clipOrdering);
         }
      }
   }
}

int PrTgifFindColorIndex(ObjPtr, s)
   struct ObjRec *ObjPtr;
   char *s;
{
   int i;

   for (i=0; i < maxColors; i++) {
      int valid=FALSE;
      char *s1=s, *s2=colorMenuItems[i];

      if (*s1 == *s2) {
         valid = TRUE;
      } else if (*s1 >= 'a' && *s1 <= 'z') {
         if (*s1+'A'-'a' == *s2) {
            valid = TRUE;
         }
      } else if (*s >= 'A' && *s <= 'Z') {
         if (*s1+'a'-'A' == *s2) {
            valid = TRUE;
         }
      }
      if (valid && (UtilStrICmp(++s1,++s2) == 0)) {
         if (ObjPtr != NULL) {
            UtilStrCpyN(ObjPtr->color_str, sizeof(ObjPtr->color_str),
                  colorMenuItems[i]);
         }
         return i;
      }
   }
   return INVALID;
}

int QuickFindColorIndex(ObjPtr, s, new_alloc, use_default)
   struct ObjRec *ObjPtr;
   char *s;
   int *new_alloc, use_default;
{
   register int i;
   XColor exact_def, screen_def;

   if (new_alloc != NULL) *new_alloc = FALSE;

   if (PRTGIF && cmdLineColor && (prTgifFoundColorInfo ||
         !cmdLineOpenDisplay)) {
      int color_index=PrTgifFindColorIndex(ObjPtr, s);

      if (color_index != INVALID || !cmdLineOpenDisplay) {
         return color_index;
      }
   }
   if (ObjPtr != NULL) {
      UtilStrCpyN(ObjPtr->color_str, sizeof(ObjPtr->color_str), s);
   }
   if (colorMenuItems == NULL) {
      return INVALID;
   }
   if (*s == '#') {
      if (!TgifParseColor(s, &exact_def)) {
         if (use_default) {
            sprintf(gszMsgBox,
                  TgLoadCachedString(CSTID_CANNOT_ALLOC_COLOR_USE_ALT),
                  s, colorMenuItems[colorIndex]);
            if (!PRTGIF) Msg(gszMsgBox);
            allocColorFailed = TRUE;
            if (ObjPtr != NULL) {
               UtilStrCpyN(ObjPtr->color_str, sizeof(ObjPtr->color_str),
                     colorMenuItems[colorIndex]);
            }
            return colorIndex;
         }
         return INVALID;
      }
   } else {
      for (i=0; i < maxColors; i++) {
         int valid=FALSE;
         char *s1=s, *s2=colorMenuItems[i];

         if (*s1 == *s2) {
            valid = TRUE;
         } else if (*s1 >= 'a' && *s1 <= 'z') {
            if (*s1+'A'-'a' == *s2) {
               valid = TRUE;
            }
         } else if (*s >= 'A' && *s <= 'Z') {
            if (*s1+'a'-'A' == *s2) {
               valid = TRUE;
            }
         }
         if (valid && (UtilStrICmp(++s1,++s2) == 0)) {
            if (ObjPtr != NULL) {
               UtilStrCpyN(ObjPtr->color_str, sizeof(ObjPtr->color_str),
                     colorMenuItems[i]);
            }
            return i;
         }
      }

      if (!TgifParseColor(s, &exact_def)) {
         if (use_default) {
            sprintf(gszMsgBox,
                  TgLoadCachedString(CSTID_CANNOT_ALLOC_COLOR_USE_ALT),
                  s, colorMenuItems[colorIndex]);
            if (!PRTGIF) Msg(gszMsgBox);
            allocColorFailed = TRUE;
            if (ObjPtr != NULL) {
               UtilStrCpyN(ObjPtr->color_str, sizeof(ObjPtr->color_str),
                     colorMenuItems[colorIndex]);
            }
            return colorIndex;
         }
         return INVALID;
      }
   }
   for (i=0; i < maxColors; i++) {
      if (tgifRequestedColors[i].red == exact_def.red &&
            tgifRequestedColors[i].green == exact_def.green &&
            tgifRequestedColors[i].blue == exact_def.blue) {
         if (ObjPtr != NULL) {
            UtilStrCpyN(ObjPtr->color_str, sizeof(ObjPtr->color_str),
                  colorMenuItems[i]);
         }
         return i;
      }
   }

   memcpy(&screen_def, &exact_def, sizeof(XColor));

   if (!XAllocColor(mainDisplay, mainColormap, &screen_def)) {
      Colormap colormap;

      if (newColormapUsed) {
         if (use_default) {
            sprintf(gszMsgBox,
                  TgLoadCachedString(CSTID_CANNOT_ALLOC_COLOR_USE_ALT),
                  s, colorMenuItems[colorIndex]);
            if (!PRTGIF) Msg(gszMsgBox);
            allocColorFailed = TRUE;
            if (ObjPtr != NULL) {
               UtilStrCpyN(ObjPtr->color_str, sizeof(ObjPtr->color_str),
                     colorMenuItems[colorIndex]);
            }
            return colorIndex;
         }
         return INVALID;
      }
      colormap = XCopyColormapAndFree(mainDisplay, mainColormap);
      mainColormap = colormap;
      newColormapUsed = TRUE;
      XSetWindowColormap(mainDisplay, mainWindow, mainColormap);
      if (!XAllocColor(mainDisplay, mainColormap, &screen_def)) {
         if (use_default) {
            sprintf(gszMsgBox,
                  TgLoadCachedString(CSTID_CANNOT_ALLOC_COLOR_USE_ALT),
                  s, colorMenuItems[colorIndex]);
            if (!PRTGIF) Msg(gszMsgBox);
            allocColorFailed = TRUE;
            if (ObjPtr != NULL) {
               UtilStrCpyN(ObjPtr->color_str, sizeof(ObjPtr->color_str),
                     colorMenuItems[colorIndex]);
            }
            return colorIndex;
         }
         return INVALID;
      }
   }
   colorPixels = (int*)realloc(colorPixels, (maxColors+1)*sizeof(int));
   xorColorPixels = (int*)realloc(xorColorPixels,
         (maxColors+1)*sizeof(int));
   colorLayerOn = (int*)realloc(colorLayerOn, (maxColors+1)*sizeof(int));
   colorMenuItems = (char**)realloc(colorMenuItems,
         (maxColors+1)*sizeof(char*));
   colorMenuItems[maxColors] = (char*)malloc((strlen(s)+1)*sizeof(char));
   if (colorMenuItems[maxColors] == NULL) FailAllocMessage();
   tgifColors = (XColor*)realloc(tgifColors,
         (maxColors+1)*sizeof(XColor));
   tgifRequestedColors = (XColor*)realloc(tgifRequestedColors,
         (maxColors+1)*sizeof(XColor));

   colorPixels[maxColors] = screen_def.pixel;
   xorColorPixels[maxColors] =
         screen_def.pixel ^ GetDrawingBgPixel(INVALID, INVALID);
   colorLayerOn[maxColors] = TRUE;
   strcpy(colorMenuItems[maxColors], s);
   tgifRequestedColors[maxColors].red = exact_def.red;
   tgifRequestedColors[maxColors].green = exact_def.green;
   tgifRequestedColors[maxColors].blue = exact_def.blue;
   tgifColors[maxColors].red = screen_def.red;
   tgifColors[maxColors].green = screen_def.green;
   tgifColors[maxColors].blue = screen_def.blue;
   maxColors++;
   *new_alloc = TRUE;

   DestroyPinnedMenu(MENU_COLOR);
   if (!PRTGIF && colorLayers) {
      needToRedrawColorWindow = TRUE;
   }
   return (maxColors-1);
}

struct ObjRec *CreateXPmObj(ImageW, ImageH, W, H, pixmap, image, bitmap,
      bitmap_image, ncolors, chars_per_pixel, first_pixel_is_bg, color_char,
      color_str, pixels, xpm_data)
   int ImageW, ImageH, W, H;
   Pixmap pixmap, bitmap;
   XImage *image, *bitmap_image;
   int ncolors, chars_per_pixel, first_pixel_is_bg;
   char *color_char;
   char **color_str;
   int *pixels;
   char *xpm_data;
{
   struct XPmRec *xpm_ptr;
   struct ObjRec *obj_ptr;

   xpm_ptr = (struct XPmRec *)malloc(sizeof(struct XPmRec));
   if (xpm_ptr == NULL) FailAllocMessage();
   memset(xpm_ptr, 0, sizeof(struct XPmRec));

   xpm_ptr->pixmap = pixmap;
   xpm_ptr->image = image;
   xpm_ptr->bitmap = bitmap;
   xpm_ptr->bitmap_image = bitmap_image;
   xpm_ptr->image_w = ImageW;
   xpm_ptr->image_h = ImageH;
   xpm_ptr->data = xpm_data;

   xpm_ptr->fill = objFill;
   xpm_ptr->flip = xpm_ptr->cached_flip = 0;
   xpm_ptr->cached_zoom = 0;
   xpm_ptr->cached_pixmap = None;
   xpm_ptr->cached_bitmap = None;
   xpm_ptr->cached_w = xpm_ptr->cached_h = 0;
   xpm_ptr->cached_color = (-1);

   xpm_ptr->ncolors = ncolors;
   xpm_ptr->chars_per_pixel = chars_per_pixel;
   xpm_ptr->first_pixel_is_bg = first_pixel_is_bg;
   xpm_ptr->color_char = color_char;
   xpm_ptr->color_str = color_str;
   xpm_ptr->pixels = pixels;
   if (colorDisplay || ncolors == (-1)) {
      xpm_ptr->red = xpm_ptr->green = xpm_ptr->blue = NULL;
   } else {
      int i;

      xpm_ptr->red = (int*)malloc(ncolors*sizeof(int));
      xpm_ptr->green = (int*)malloc(ncolors*sizeof(int));
      xpm_ptr->blue = (int*)malloc(ncolors*sizeof(int));
      if (xpm_ptr->red==NULL || xpm_ptr->green==NULL || xpm_ptr->blue==NULL) {
         FailAllocMessage();
      }
      for (i=0; i < ncolors; i++) {
         int new_alloc;
         int index=QuickFindColorIndex(NULL, color_str[i], &new_alloc, TRUE);

         xpm_ptr->red[i] =
               (int)(10000*((int)tgifRequestedColors[index].red)/maxRGB);
         xpm_ptr->green[i] =
               (int)(10000*((int)tgifRequestedColors[index].green)/maxRGB);
         xpm_ptr->blue[i] =
               (int)(10000*((int)tgifRequestedColors[index].blue)/maxRGB);
      }
   }
   xpm_ptr->clip_mask = None;

   obj_ptr = (struct ObjRec *)malloc(sizeof(struct ObjRec));
   if (obj_ptr == NULL) FailAllocMessage();
   memset(obj_ptr, 0, sizeof(struct ObjRec));

   obj_ptr->bbox.ltx = obj_ptr->obbox.ltx = obj_ptr->x = drawOrigX;
   obj_ptr->bbox.lty = obj_ptr->obbox.lty = obj_ptr->y = drawOrigY;
   obj_ptr->bbox.rbx = obj_ptr->obbox.rbx = W + drawOrigX;
   obj_ptr->bbox.rby = obj_ptr->obbox.rby = H + drawOrigY;
   obj_ptr->type = OBJ_XPM;
   obj_ptr->color = colorIndex;
   if (mainDisplay != NULL) {
      UtilStrCpyN(obj_ptr->color_str, sizeof(obj_ptr->color_str),
            colorMenuItems[colorIndex]);
   }
   obj_ptr->id = objId++;
   obj_ptr->dirty = FALSE;
   obj_ptr->rotation = 0;
   obj_ptr->locked = FALSE;
   obj_ptr->detail.xpm = xpm_ptr;
   obj_ptr->fattr = obj_ptr->lattr = NULL;
   obj_ptr->ctm = NULL;
   obj_ptr->invisible = FALSE;

   return obj_ptr;
}
 
static
void CopyXPmProperties(DestXpmObj, SrcXpmObj)
   struct ObjRec *DestXpmObj, *SrcXpmObj;
{
   struct XPmRec *dest_xpm_ptr=DestXpmObj->detail.xpm;
   struct XPmRec *src_xpm_ptr=SrcXpmObj->detail.xpm;

   dest_xpm_ptr->fill = src_xpm_ptr->fill;
   DestXpmObj->color = SrcXpmObj->color;
   memcpy(DestXpmObj->color_str, SrcXpmObj->color_str,
         sizeof(DestXpmObj->color_str));
}

static
int FinishBreakUpXPixmap(obj_ptr, cols_and_rows, cols, rows)
   struct ObjRec *obj_ptr;
   int cols_and_rows, cols, rows;
{
   struct XPmRec *xpm_ptr=obj_ptr->detail.xpm;
   int y, image_w=xpm_ptr->image_w, image_h=xpm_ptr->image_h;
   int chunk_w=0, chunk_h=0, total_chunks=0;
   int orig_x=obj_ptr->x, orig_y=obj_ptr->y;
   int ncolors, chars_per_pixel, first_pixel_is_bg;
   char tmp_fname[MAXPATHLENGTH];

   if (MkTempFile(tmp_fname, sizeof(tmp_fname), tmpDir, TOOL_NAME) == NULL) {
      return FALSE;
   }
   ncolors = xpm_ptr->ncolors;
   chars_per_pixel = xpm_ptr->chars_per_pixel;
   first_pixel_is_bg = xpm_ptr->first_pixel_is_bg;

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
         int w=min(image_w-x,chunk_w), i, *pixels=NULL;
         char *color_char=NULL, **color_str=NULL, *xpm_data=NULL;
         struct ObjRec *new_obj_ptr=NULL;
         Pixmap dest_pixmap=None, dest_bitmap=None;
         XImage *dest_image=NULL, *dest_bitmap_image=NULL;

         if (w <= 0 || h <= 0 ||
               !ExtractPixmap(xpm_ptr->pixmap, xpm_ptr->image, xpm_ptr->bitmap,
               xpm_ptr->bitmap_image, x, y, w, h, &dest_pixmap, &dest_image,
               &dest_bitmap, &dest_bitmap_image)) {
            continue;
         }
         if ((xpm_ptr->real_type == XPM_JPEG ||
               xpm_ptr->real_type == PPM_TRUE) &&
               fullTrueColorMode && HasZlibSupport()) {
            if (DumpXImageToPpmFile(dest_image, w, h, tmp_fname, TRUE)) {
               char deflated_fname[MAXPATHLENGTH];
               unsigned int data_size=0;
               char *ppm_data=NULL;

               snprintf(deflated_fname, sizeof(deflated_fname), "%s.ppm.z",
                     tmp_fname);
               ppm_data = ReadFileIntoBuf(deflated_fname, &data_size);
               unlink(deflated_fname);
               unlink(tmp_fname);
               if (ppm_data) {
                  new_obj_ptr = CreatePpmTrueObjFromImage(dest_image, w, h,
                        ppm_data, data_size);
               }
            }
            if (dest_pixmap != None) XFreePixmap(mainDisplay, dest_pixmap);
            if (dest_bitmap != None) XFreePixmap(mainDisplay, dest_bitmap);
            if (dest_image != NULL) XDestroyImage(dest_image);
            if (dest_bitmap_image != NULL) XDestroyImage(dest_bitmap_image);
            dest_pixmap = dest_bitmap = None;
            dest_image = dest_bitmap_image = NULL;
            if (new_obj_ptr != NULL) {
               total_chunks++;
            }
         } else {
            pixels = (int*)malloc(ncolors*sizeof(int));
            if (pixels == NULL) FailAllocMessage();
            memcpy(pixels, xpm_ptr->pixels, (size_t)(ncolors*sizeof(int)));

            color_char = (char*)malloc(ncolors*chars_per_pixel*sizeof(char));
            if (color_char == NULL) FailAllocMessage();
            memcpy(color_char, xpm_ptr->color_char,
                  (size_t)(ncolors*chars_per_pixel*sizeof(char)));

            color_str = (char**)malloc(ncolors*sizeof(char*));
            if (color_str == NULL) FailAllocMessage();
            for (i=0; i < ncolors; i++) {
               if ((color_str[i]=UtilStrDup(xpm_ptr->color_str[i])) == NULL) {
                  FailAllocMessage();
               }
            }
            total_chunks++;
            new_obj_ptr = CreateXPmObj(w, h, w, h, dest_pixmap, dest_image,
                  dest_bitmap, dest_bitmap_image, ncolors, chars_per_pixel,
                  first_pixel_is_bg, color_char, color_str, pixels, xpm_data);
            CopyXPmProperties(new_obj_ptr, obj_ptr);
         }
         if (new_obj_ptr != NULL) {
            AdjObjBBox(new_obj_ptr);
            MoveObj(new_obj_ptr, orig_x+x, orig_y+y);
            AddObj(NULL, topObj, new_obj_ptr);
         }
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

void BreakUpXPixmap(obj_ptr, cols_and_rows, cols, rows)
   struct ObjRec *obj_ptr;
   int cols_and_rows, cols, rows;
{
   HighLightReverse();
   PrepareToReplaceAnObj(obj_ptr);
   if (FinishBreakUpXPixmap(obj_ptr, cols_and_rows, cols, rows)) {
      UpdSelBBox();
      RecordCmd(CMD_ONE_TO_MANY, NULL, topSel, botSel, numObjSelected);
      SetFileModified(TRUE);
      justDupped = FALSE;
   } else {
      AbortPrepareCmd(CMD_REPLACE);
   }
   HighLightForward();
}

static
void SaveXPmPixels(FP, xpm_ptr, pixmap, image, bitmap, bitmap_image, w, h,
      ncolors, chars_per_pixel, color_char, pixels)
   FILE *FP;
   struct XPmRec *xpm_ptr;
   Pixmap pixmap, bitmap;
   XImage **image, **bitmap_image;
   int w, h, ncolors, chars_per_pixel, *pixels;
   char *color_char;
{
   register int j, data, index, i, k;
   char *xpm_data=NULL;

   if (*image == NULL) {
      *image = XGetImage(mainDisplay, pixmap, 0, 0, w, h, AllPlanes, ZPixmap);
      if (*image == NULL) {
         sprintf(gszMsgBox, "%s\n\n%s",
               TgLoadString(STID_XGETIMAGE_MAY_RUN_OUT_VMEM),
               TgLoadString(STID_SAVED_FILE_MAY_BE_CORRUPTED));
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
         return;
      }
   }
   if (*bitmap_image == NULL) {
      *bitmap_image = XGetImage(mainDisplay, bitmap, 0, 0, w, h, 1, ZPixmap);
      if (*bitmap_image == NULL) {
         sprintf(gszMsgBox, "%s\n\n%s",
               TgLoadString(STID_XGETIMAGE_MAY_RUN_OUT_VMEM),
               TgLoadString(STID_SAVED_FILE_MAY_BE_CORRUPTED));
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
         return;
      }
   }
   xpm_data = xpm_ptr->data;
   for (i=0; i < h; i++) {
      if (fprintf(FP, "   \"") == EOF) writeFileFailed = TRUE;
      for (j=0; j < w; j++) {
         if (!colorDisplay && xpm_data != NULL && xpm_ptr->red != NULL) {
            for (k=0; k < chars_per_pixel; k++) {
               if (fprintf(FP, "%c", xpm_data[j*chars_per_pixel+k]) == EOF) {
                  writeFileFailed = TRUE;
               }
            }
         } else {
            if (XGetPixel(*bitmap_image, j, i) != 0) {
               data = XGetPixel(*image,j,i);
            } else {
               data = (-1);
            }
            index = XPmLookUp(data, INVALID, NULL, NULL);
            if (index == INVALID) {
               sprintf(gszMsgBox,
                     TgLoadString(STID_UNRECOG_GIVEN_PIXEL_VAL_PRINT),
                     j, i, data, (long)data);
               Msg(gszMsgBox);
               return;
            }
            for (k=0; k < chars_per_pixel; k++) {
               if (fprintf(FP, "%c", color_char[index*chars_per_pixel+k]) ==
                     EOF) {
                  writeFileFailed = TRUE;
               }
            }
         }
      }
      if (xpm_data != NULL) xpm_data += w*chars_per_pixel;
      if (i == h-1) {
         if (fprintf(FP, "\"],") == EOF) writeFileFailed = TRUE;
      } else {
         if (fprintf(FP, "\",\n") == EOF) writeFileFailed = TRUE;
      }
   }
}

static
int WritePpmTrueData(FP, xpm_ptr)
   FILE *FP;
   struct XPmRec *xpm_ptr;
{
   char *psz=xpm_ptr->ppm_data;
   int total_bytes_to_write=xpm_ptr->ppm_data_size;

   while (total_bytes_to_write > 0) {
      int bytes_to_read=min(48, total_bytes_to_write), bytes_to_write=0;
      char out_buf[65];

      bytes_to_write = (bytes_to_read / 3);
      if ((bytes_to_read % 3) == 0) {
         bytes_to_write = bytes_to_write << 2;
      } else {
         bytes_to_write = (bytes_to_write+1) << 2;
      }
      *out_buf = '\0';
      DoBase64Encode(psz, bytes_to_read, out_buf);
      if (fprintf(FP, "   \"") == EOF) writeFileFailed = TRUE;
      if (fwrite(out_buf, 1, bytes_to_write, FP) != bytes_to_write) {
         writeFileFailed = TRUE;
      }
      psz += bytes_to_read;
      total_bytes_to_write -= bytes_to_read;
      if (total_bytes_to_write > 0) {
         if (fprintf(FP, "\",\n") == EOF) writeFileFailed = TRUE;
      } else {
         if (fprintf(FP, "\"],[\n") == EOF) writeFileFailed = TRUE;
      }
   }
   if (xpm_ptr->ppm_mask_data != NULL) {
      TgAssert(FALSE, "xpm_ptr->ppm_mask_data != NULL in WritePpmTrueData()",
            NULL);
   }
   if (fprintf(FP, "   ],") == EOF) writeFileFailed = TRUE;

   return TRUE;
}

static
char *ReadPpmTrueData(fp, ppm_data_size)
   FILE *fp;
   int ppm_data_size;
{
   char line[64+20], *write_ptr=NULL;
   char *buf_return=(char*)malloc(ppm_data_size+1);

   if (buf_return == NULL) FailAllocMessage();
   write_ptr = buf_return;
   while (fgets(line, sizeof(line), fp) != NULL) {
      int len=strlen(line), bytes_converted=0;
      char *s=NULL, buf[64+20], out_buf[49];

      scanLineNum++;
      if (len > 0 && line[len-1] != '\n') {
         sprintf(gszMsgBox, TgLoadString(STID_BAD_LINE_READING_GIVEN_OBJ),
               "ppm_true");
         if (PRTGIF) {
            fprintf(stderr, "%s\n", gszMsgBox);
         } else {
            Msg(gszMsgBox);
         }
         UtilFree(buf_return);
         return NULL;
      }
      s = FindChar((int)'"', line);
      s = ParseStr(s, (int)'"', buf, sizeof(buf));
      len = strlen(buf);
      if (len > 64 || (len & 0x3) != 0) {
         sprintf(gszMsgBox, TgLoadString(STID_BAD_LINE_READING_GIVEN_OBJ),
               "ppm_true");
         if (PRTGIF) {
            fprintf(stderr, "%s\n", gszMsgBox);
         } else {
            Msg(gszMsgBox);
         }
         UtilFree(buf_return);
         return NULL;
      }
      /* bytes_converted = (len >> 2) * 3; */
      bytes_converted = DoBase64Decode(buf, len, out_buf);
      ppm_data_size -= bytes_converted;
      if (ppm_data_size < 0) {
         sprintf(gszMsgBox, TgLoadString(STID_INSUF_DATA_READING_GIVEN_OBJ),
               "ppm_true");
         if (PRTGIF) {
            fprintf(stderr, "%s\n", gszMsgBox);
         } else {
            Msg(gszMsgBox);
         }
         UtilFree(buf_return);
         return NULL;
      }
      memcpy(write_ptr, out_buf, bytes_converted);
      write_ptr += bytes_converted;
      if (ppm_data_size == 0) break;
   }
   if (ppm_data_size != 0) {
      sprintf(gszMsgBox, TgLoadString(STID_BAD_LINE_READING_GIVEN_OBJ),
            "ppm_true");
      if (PRTGIF) {
         fprintf(stderr, "%s\n", gszMsgBox);
      } else {
         Msg(gszMsgBox);
      }
      UtilFree(buf_return);
      return NULL;
   }
   return buf_return;
}

static
char *ReadPpmTrueMask(fp, ppm_mask_size)
   FILE *fp;
   int ppm_mask_size;
{
   char line[64+20];

   while (fgets(line, sizeof(line), fp) != NULL) {
      /* this feature has not been implemented yet */
      break;
   }
   return NULL;
}

void SaveXPmObj(FP, ObjPtr)
   FILE *FP;
   struct ObjRec *ObjPtr;
{
   int ltx, lty, rbx, rby, fill, ncolors, *pixels=NULL;
   int chars_per_pixel, first_pixel_is_bg, image_w, image_h;
   int compressed=FALSE, real_type=0, linked_jpeg=FALSE, jpeg=FALSE;
   int ppm_z=FALSE;
   struct XPmRec *xpm_ptr=ObjPtr->detail.xpm;
   char *color_char=NULL, **color_str=NULL, *jpeg_filename=NULL;

   ltx = ObjPtr->obbox.ltx; lty = ObjPtr->obbox.lty;
   rbx = ObjPtr->obbox.rbx; rby = ObjPtr->obbox.rby;
   image_w = xpm_ptr->image_w;
   image_h = xpm_ptr->image_h;
   fill = xpm_ptr->fill;
   ncolors = xpm_ptr->ncolors;
   chars_per_pixel = xpm_ptr->chars_per_pixel;
   first_pixel_is_bg = xpm_ptr->first_pixel_is_bg;
   color_char = xpm_ptr->color_char;
   color_str = xpm_ptr->color_str;
   pixels = xpm_ptr->pixels;

   real_type = xpm_ptr->real_type;
   linked_jpeg = xpm_ptr->linked_jpeg;
   jpeg_filename = xpm_ptr->filename;

   /* only doing linked JPEG */
   jpeg = (real_type == XPM_JPEG && linked_jpeg && jpeg_filename != NULL);
   ppm_z = (real_type == PPM_TRUE);

   if (!colorDisplay && xpm_ptr->red == NULL && !shownXPmErrorMessage) {
      MsgBox(TgLoadString(STID_CANT_SAVE_XPM_ON_BW_DPY), TOOL_NAME, INFO_MB);
      shownXPmErrorMessage = TRUE;
      return;
   }
   if (jpeg) {
      if (fprintf(FP, "jpeg('%s','',", colorMenuItems[ObjPtr->color]) == EOF) {
         writeFileFailed = TRUE;
      }
   } else if (ppm_z) {
      if (fprintf(FP, "ppm_true('%s',", colorMenuItems[ObjPtr->color]) == EOF) {
         writeFileFailed = TRUE;
      }
      if (xpm_ptr->has_transparent_color) {
         if (fprintf(FP, "'#%02x%02x%02x',1,",
               xpm_ptr->transparent_color[0], xpm_ptr->transparent_color[1],
               xpm_ptr->transparent_color[2]) == EOF) {
            writeFileFailed = TRUE;
         }
      } else {
         if (fprintf(FP, "'#ffffff',0,") == EOF) {
            writeFileFailed = TRUE;
         }
      }
   } else {
      if (fprintf(FP, "xpm('%s','',", colorMenuItems[ObjPtr->color]) == EOF) {
         writeFileFailed = TRUE;
      }
   }
   if (ppm_z) {
      if (fprintf(FP,
            "%1d,%1d,%1d,%1d,%1d,%1d,%1d,%1d,",
            ltx, lty, rbx, rby, ObjPtr->id, ObjPtr->rotation, image_w,
            image_h) == EOF) {
         writeFileFailed = TRUE;
      }
   } else {
      if (fprintf(FP,
            "%1d,%1d,%1d,%1d,%1d,%1d,%1d,%1d,%1d,%1d,%1d,%1d,",
            ltx, lty, rbx, rby, fill, ncolors, chars_per_pixel,
            first_pixel_is_bg, ObjPtr->id, ObjPtr->rotation, image_w,
            image_h) == EOF) {
         writeFileFailed = TRUE;
      }
   }
   if (jpeg) {
      if (fprintf(FP,
            "0,%1d,%1d,%1d,%1d,%1d,%1d,%1d,\n    \"%s\",",
            xpm_ptr->flip, ObjPtr->locked, compressed,
            ObjPtr->ctm!=NULL, ObjPtr->invisible, real_type, linked_jpeg,
            jpeg_filename) == EOF) {
         writeFileFailed = TRUE;
      }
   } else if (ppm_z) {
      if (fprintf(FP,
            "0,%1d,%1d,%1d,%1d,%1d,%1d,%1d,",
            xpm_ptr->flip, ObjPtr->locked, ObjPtr->ctm!=NULL, ObjPtr->invisible,
            xpm_ptr->ppm_data_compress, xpm_ptr->ppm_data_size,
            xpm_ptr->ppm_mask_size) == EOF) {
         writeFileFailed = TRUE;
      }
   } else {
      if (fprintf(FP,
            "0,%1d,%1d,%1d,%1d,%1d,",
            xpm_ptr->flip, ObjPtr->locked, compressed,
            ObjPtr->ctm!=NULL, ObjPtr->invisible) == EOF) {
         writeFileFailed = TRUE;
      }
   }
   if (ObjPtr->ctm != NULL && fprintf(FP,
         "[\n   %1d,%1d,%1d,%1d,%1d,%1d,%g,%g,%g,%g,%1d,%1d],",
         ObjPtr->x, ObjPtr->y,
         ObjPtr->orig_obbox.ltx, ObjPtr->orig_obbox.lty,
         ObjPtr->orig_obbox.rbx, ObjPtr->orig_obbox.rby,
         ObjPtr->ctm->m[CTM_SX], ObjPtr->ctm->m[CTM_SIN],
         ObjPtr->ctm->m[CTM_MSIN], ObjPtr->ctm->m[CTM_SY],
         ObjPtr->ctm->t[CTM_TX], ObjPtr->ctm->t[CTM_TY]) == EOF) {
      writeFileFailed = TRUE;
   }
   if (jpeg) {
      /* nothing to write here */
   } else if (ppm_z) {
      if (fprintf(FP, "[\n") == EOF) writeFileFailed = TRUE;
      if (!WritePpmTrueData(FP, xpm_ptr)) {
         return;
      }
   } else {
      if (fprintf(FP, "[\n") == EOF) writeFileFailed = TRUE;
      if (!colorDisplay && xpm_ptr->red != NULL) {
         if (!BuildXPmBuckets(ncolors, NULL, NULL, chars_per_pixel,
               color_char, NULL)) {
            return;
         }
      } else {
         if (!BuildXPmBuckets(ncolors, pixels, NULL, INVALID, NULL, NULL)) {
            return;
         }
      }
      SaveXPmColors(FP, ObjPtr->color, xpm_ptr, ncolors, chars_per_pixel,
            color_char, color_str, pixels);
      SaveXPmPixels(FP, xpm_ptr, xpm_ptr->pixmap, &(xpm_ptr->image),
            xpm_ptr->bitmap, &(xpm_ptr->bitmap_image),
            image_w, image_h, ncolors, chars_per_pixel, color_char, pixels);
   }
   if (serializingFile) SaveCreatorID(FP, ObjPtr, "    ");
   SaveAttrs(FP, ObjPtr->lattr);
   if (fprintf(FP, ")") == EOF) writeFileFailed = TRUE;
}

static
void FreeAuxData(fp, ncolors, color_char, color_str,
      p_pixels, pixels_used, p_pixmap, p_image, p_bitmap, p_bitmap_image)
   FILE *fp;
   int ncolors;
   char *color_char, **color_str;
   int **p_pixels, *pixels_used;
   Pixmap *p_pixmap, *p_bitmap;
   XImage **p_image, **p_bitmap_image;
{
   register int i;

   for (i = 0; i < ncolors; i++) free(color_str[i]);
   free(color_char);
   free(color_str);
   if (p_pixels != NULL && *p_pixels != NULL) free(*p_pixels);
   if (pixels_used != NULL) free(pixels_used);
   if (p_pixmap != NULL && *p_pixmap != None) {
      XFreePixmap(mainDisplay, *p_pixmap);
   }
   if (p_bitmap != NULL && *p_bitmap != None){
       XFreePixmap(mainDisplay, *p_bitmap);
   }
   if (p_image != NULL && *p_image != NULL) XDestroyImage(*p_image);
   if (p_bitmap_image != NULL && *p_bitmap_image != NULL) {
      XDestroyImage(*p_bitmap_image);
   }
   fclose(fp);
}

int MyReadPixmapFile(file_name, image_w_return, image_h_return, w_return,
      h_return, pixmap_return, image_return, bitmap_return, bitmap_image_return,
      ncolors_return, chars_per_pixel_return, first_pixel_is_bg_return,
      color_char, color_str, pixels, xpm_data)
   char *file_name;
   int *image_w_return, *image_h_return, *w_return, *h_return;
   int *ncolors_return;
   int *chars_per_pixel_return, *first_pixel_is_bg_return;
   Pixmap *pixmap_return, *bitmap_return;
   XImage **image_return, **bitmap_image_return;
   char **color_char, ***color_str;
   int **pixels;
   char **xpm_data;
{
   register int j, k;
   register char *c_ptr;
   FILE *fp;
   char inbuf[MAXSTRING], *line, s[MAXSTRING];
   char mag_spec[MAXSTRING], *xpm_data_ptr=NULL;
   int i, len, format, found=FALSE, index, saved_max_colors, xpm_version=1;
   int x, y, w, h, chars_per_pixel, new_alloc, image_w=0, image_h=0;
   int *pixels_used=NULL, bg_pixel;
   float mag=(float)0;

   myReadTransparentPixmap = FALSE;

   if (xpm_data != NULL) *xpm_data = NULL;
   gnNumNewColorsInPixmapFile = 0;

   if ((fp=fopen(file_name, "r")) == NULL) return BitmapOpenFailed;

   saved_max_colors = maxColors;
   while (fgets(inbuf, MAXSTRING, fp) != NULL) {
      if (strncmp(inbuf, "#define ", 8) == 0) {
         xpm_version = 1;
         found = TRUE;
         break;
      } else if (strncmp(inbuf, "/* XPM */", 9) == 0) {
         xpm_version = 3;
         found = TRUE;
         break;
      }
   }
   if (!found) return BitmapFileInvalid;

   if (xpm_version == 1) {
      c_ptr = FindChar((int)' ', inbuf);
      c_ptr = ParseStr(c_ptr, (int)' ', s, sizeof(s));
      len = strlen(s);
      if (len <= 7 || strcmp(&s[len-7], "_format") != 0) {
         return BitmapFileInvalid;
      }
      sscanf(c_ptr, "%d", &format);
      if (format != 1) {
         sprintf(gszMsgBox, TgLoadString(STID_CANT_IMPORT_GIVEN_FORMAT_XPM),
               format);
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
         return BitmapFileInvalid;
      }
      if (fgets(inbuf, MAXSTRING, fp)==NULL) return BitmapFileInvalid;
      c_ptr = FindChar((int)' ', inbuf);
      c_ptr = ParseStr(c_ptr, (int)' ', s, sizeof(s));
      len = strlen(s);
      if (len <= 6 || strcmp(&s[len-6], "_width") != 0) {
         return BitmapFileInvalid;
      }
      sscanf(c_ptr, "%d", &image_w);
      if (image_w_return != NULL) *image_w_return = image_w;

      if (fgets(inbuf, MAXSTRING, fp)==NULL) return BitmapFileInvalid;
      c_ptr = FindChar((int)' ', inbuf);
      c_ptr = ParseStr(c_ptr, (int)' ', s, sizeof(s));
      len = strlen(s);
      if (len <= 7 || strcmp(&s[len-7], "_height") != 0) {
         return BitmapFileInvalid;
      }
      sscanf(c_ptr, "%d", &image_h);
      if (image_h_return != NULL) *image_h_return = image_h;

      if (fgets(inbuf, MAXSTRING, fp)==NULL) return BitmapFileInvalid;
      c_ptr = FindChar((int)' ', inbuf);
      c_ptr = ParseStr(c_ptr, (int)' ', s, sizeof(s));
      len = strlen(s);
      if (len <= 8 || strcmp(&s[len-8], "_ncolors") != 0) {
         return BitmapFileInvalid;
      }
      sscanf(c_ptr, "%d", ncolors_return);

      if (fgets(inbuf, MAXSTRING, fp)==NULL) return BitmapFileInvalid;
      c_ptr = FindChar((int)' ', inbuf);
      c_ptr = ParseStr(c_ptr, (int)' ', s, sizeof(s));
      len = strlen(s);
      if (len <= 16 || strcmp(&s[len-16], "_chars_per_pixel") != 0) {
         return BitmapFileInvalid;
      }
      sscanf(c_ptr, "%d", &chars_per_pixel);
      if (chars_per_pixel_return != NULL) {
         *chars_per_pixel_return = chars_per_pixel;
      }
      if (chars_per_pixel > 2) {
         sprintf(gszMsgBox, TgLoadString(STID_CANT_IMPORT_GIVEN_CPP_XPM),
               chars_per_pixel);
         Msg(gszMsgBox);
         return BitmapFileInvalid;
      }

      if (fgets(inbuf, MAXSTRING, fp)==NULL) return BitmapFileInvalid;
      len = strlen(inbuf);
      if (len <= 27 || strncmp(inbuf, "static char *", 13) != 0 ||
            strncmp(&inbuf[len-14], "_colors[] = {\n", 14) != 0) {
         return BitmapFileInvalid;
      }
   } else {
      /* xpm_version is 3 */
      found = FALSE;
      while (fgets(inbuf, MAXSTRING, fp) != NULL) {
         if (*inbuf == '"') {
            found = TRUE;
            break;
         }
      }
      if (!found) return BitmapFileInvalid;
      c_ptr = &inbuf[1];
      if (sscanf(c_ptr, "%d %d %d %d", &image_w, &image_h,
            ncolors_return, &chars_per_pixel) != 4) {
         return BitmapFileInvalid;
      }
      if (image_w_return != NULL) *image_w_return = image_w;
      if (image_h_return != NULL) *image_h_return = image_h;
      if (chars_per_pixel_return != NULL) {
         *chars_per_pixel_return = chars_per_pixel;
      }
   }

   *color_char =
         (char*)malloc((*ncolors_return)*(chars_per_pixel)*sizeof(char)+1);
   *color_str = (char**)malloc((*ncolors_return)*sizeof(char*));
   if (color_char == NULL || color_str == NULL) FailAllocMessage();
   if (pixels != NULL) {
      *pixels = (int*)malloc((*ncolors_return)*sizeof(int));
      pixels_used = (int*)malloc((*ncolors_return)*sizeof(int));
      if (*pixels == NULL || pixels_used == NULL) FailAllocMessage();
      memset(pixels_used, 0, (*ncolors_return)*sizeof(int));
   }
   if (pixmap_return != NULL) *pixmap_return = None;
   if (bitmap_return != NULL) *bitmap_return = None;
   if (image_return != NULL) *image_return = NULL;
   if (bitmap_image_return != NULL) *bitmap_image_return = NULL;
   if (first_pixel_is_bg_return != NULL) *first_pixel_is_bg_return = FALSE;

   bg_pixel = GetDrawingBgPixel(INVALID, INVALID);
   if (xpm_version == 1) {
      for (i = 0; i < *ncolors_return; i++) {
         if (fgets(inbuf, MAXSTRING, fp) == NULL) {
            FreeAuxData(fp, i, *color_char, *color_str, pixels, pixels_used,
                  pixmap_return, image_return, bitmap_return,
                  bitmap_image_return);
            return BitmapFileInvalid;
         }
         c_ptr = FindChar((int)'"', inbuf);
         for (j = 0; j < chars_per_pixel; j++) {
            (*color_char)[i*(chars_per_pixel)+j] = c_ptr[j];
         }
         if (guessXPmBgColor && i == 0 &&
               ((chars_per_pixel == 1 && (*c_ptr=='`' || *c_ptr==' ')) ||
               (chars_per_pixel == 2 && ((c_ptr[0]=='`' && c_ptr[1]=='`') ||
               (c_ptr[0]==' ' && c_ptr[1]==' '))))) {
            strcpy(s, (myFileBgColorStr==NULL ? myBgColorStr :
                  myFileBgColorStr));
            if (pixels != NULL) (*pixels)[0] = bg_pixel;
            if (first_pixel_is_bg_return != NULL) {
               *first_pixel_is_bg_return = TRUE;
            }
         } else {
            c_ptr = FindChar((int)'"', c_ptr);
            c_ptr = FindChar((int)'"', c_ptr);
            ParseStr(c_ptr, (int)'"', s, sizeof(s));
            if (pixels != NULL) {
               if (UtilStrICmp(s, "None") == 0) {
                  (*pixels)[i] = (-1);
                  myReadTransparentPixmap = TRUE;
               } else if ((index=QuickFindColorIndex(NULL, s, &new_alloc,
                     TRUE)) == INVALID) {
                  sprintf(gszMsgBox,
                        TgLoadCachedString(CSTID_CANNOT_ALLOC_COLOR_USE_ALT),
                        s, colorMenuItems[colorIndex]);
                  Msg(gszMsgBox);
                  strcpy(s, colorMenuItems[colorIndex]);
                  (*pixels)[i] = colorPixels[colorIndex];
                  allocColorFailed = TRUE;
               } else {
                  (*pixels)[i] = colorPixels[index];
               }
            } else {
               if (UtilStrICmp(s, "None") == 0) {
                  myReadTransparentPixmap = TRUE;
               }
            }
         }
         len = strlen(s);
         (*color_str)[i] = (char*)malloc((len+1)*sizeof(char));
         if ((*color_str)[i] == NULL) FailAllocMessage();
         strcpy((*color_str)[i], s);
      }

      if (fgets(inbuf, MAXSTRING, fp) == NULL ||
            inbuf[0] != '}' ||
            fgets(inbuf, MAXSTRING, fp) == NULL) {
         FreeAuxData(fp, *ncolors_return, *color_char, *color_str, pixels,
               pixels_used, pixmap_return, image_return, bitmap_return,
               bitmap_image_return);
         return BitmapFileInvalid;
      }
      len = strlen(inbuf);
      if (len <= 27 || strncmp(inbuf, "static char *", 13) != 0 ||
            strncmp(&inbuf[len-14], "_pixels[] = {\n", 14) != 0) {
         return BitmapFileInvalid;
      }
   } else {
      /* xpm_version is 3 */
      for (i=0; i < *ncolors_return; i++) {
         if (i == 0) {
            found = FALSE;
            while (fgets(inbuf, MAXSTRING, fp) != NULL) {
               if (*inbuf == '"') {
                  found = TRUE;
                  break;
               }
            }
            if (!found) {
               FreeAuxData(fp, i, *color_char, *color_str, pixels, pixels_used,
                     pixmap_return, image_return, bitmap_return,
                     bitmap_image_return);
               return BitmapFileInvalid;
            }
         } else if (fgets(inbuf, MAXSTRING, fp) == NULL) {
            FreeAuxData(fp, i, *color_char, *color_str, pixels, pixels_used,
                  pixmap_return, image_return, bitmap_return,
                  bitmap_image_return);
            return BitmapFileInvalid;
         }
         c_ptr = FindChar((int)'"', inbuf);
         for (j = 0; j < chars_per_pixel; j++) {
            (*color_char)[i*(chars_per_pixel)+j] = c_ptr[j];
         }
         if (guessXPmBgColor && i == 0 &&
               ((chars_per_pixel == 1 && (*c_ptr=='`' || *c_ptr==' ')) ||
               (chars_per_pixel == 2 && ((c_ptr[0]=='`' && c_ptr[1]=='`') ||
               (c_ptr[0]==' ' && c_ptr[1]==' '))))) {
            strcpy(s, (myFileBgColorStr==NULL ? myBgColorStr :
                  myFileBgColorStr));
            if (pixels != NULL) (*pixels)[0] = bg_pixel;
            if (first_pixel_is_bg_return != NULL) {
               *first_pixel_is_bg_return = TRUE;
            }
         } else {
            char *ptr;

            c_ptr += chars_per_pixel;
            while (*c_ptr != '\0') {
               while (*c_ptr == ' ' || *c_ptr == '\t') c_ptr++;
               if ((*c_ptr == 'c' || *c_ptr == 'm') &&
                     (c_ptr[1]==' ' || c_ptr[1]=='\t')) {
                  break;
               }
               while (*c_ptr!=' ' && *c_ptr!='\t' && *c_ptr!='\0') c_ptr++;
               while (*c_ptr == ' ' || *c_ptr == '\t') c_ptr++;
               while (*c_ptr!=' ' && *c_ptr!='\t' && *c_ptr!='\0') c_ptr++;
            }
            if (*c_ptr++ == '\0') {
               FreeAuxData(fp, i, *color_char, *color_str, pixels, pixels_used,
                     pixmap_return, image_return, bitmap_return,
                     bitmap_image_return);
               return BitmapFileInvalid;
            }
            while (*c_ptr == ' ' || *c_ptr == '\t') c_ptr++;
            for (ptr=c_ptr; *ptr !=  '"' && *ptr != '\0'; ptr++) {
               if (*ptr == ' ' || *ptr == '\t') {
                  char tmp_ch=ptr[1];

                  if (tmp_ch == 'm' || tmp_ch == 's') {
                     tmp_ch = ptr[2];
                     if (tmp_ch == ' ' || tmp_ch == '\t') {
                        break;
                     }
                  } else if (tmp_ch == 'g') {
                     tmp_ch = ptr[2];
                     if (tmp_ch == ' ' || tmp_ch == '\t') {
                        break;
                     } else if (tmp_ch == '4') {
                        tmp_ch = ptr[4];
                        if (tmp_ch == ' ' || tmp_ch == '\t') {
                           break;
                        }
                     }
                  }
               }
            }
            if (*ptr == '\0') {
               FreeAuxData(fp, i, *color_char, *color_str, pixels, pixels_used,
                     pixmap_return, image_return, bitmap_return,
                     bitmap_image_return);
               return BitmapFileInvalid;
            }
            while (ptr >= c_ptr) {
               if (*ptr == ' ' || *ptr == '\t' || *ptr == '"') {
                  ptr--;
               } else {
                  break;
               }
            }
            if (ptr < c_ptr) {
               FreeAuxData(fp, i, *color_char, *color_str, pixels, pixels_used,
                     pixmap_return, image_return, bitmap_return,
                     bitmap_image_return);
               return BitmapFileInvalid;
            }
            *(++ptr) = '\0';
            strcpy(s, c_ptr);
            if (pixels != NULL) {
               if (UtilStrICmp(s, "None") == 0) {
                  (*pixels)[i] = (-1);
                  myReadTransparentPixmap = TRUE;
               } else if ((index=QuickFindColorIndex(NULL, s, &new_alloc,
                     TRUE)) == INVALID) {
                  sprintf(gszMsgBox,
                        TgLoadCachedString(CSTID_CANNOT_ALLOC_COLOR_USE_ALT),
                        s, colorMenuItems[colorIndex]);
                  Msg(gszMsgBox);
                  strcpy(s, colorMenuItems[colorIndex]);
                  (*pixels)[i] = colorPixels[colorIndex];
                  allocColorFailed = TRUE;
               } else {
                  (*pixels)[i] = colorPixels[index];
               }
            } else {
               if (UtilStrICmp(s, "None") == 0) {
                  myReadTransparentPixmap = TRUE;
               }
            }
         }
         len = strlen(s);
         (*color_str)[i] = (char*)malloc((len+1)*sizeof(char));
         if ((*color_str)[i] == NULL) FailAllocMessage();
         strcpy((*color_str)[i], s);
      }
   }
   if (chars_per_pixel_return == NULL && first_pixel_is_bg_return == NULL &&
         pixels == NULL &&  xpm_data == NULL) {
      /* just getting the colors, so no need to read data */
      if (pixels_used != NULL) free(pixels_used);
      return BitmapSuccess;
   }

   x = 0;
   y = 0;
   w = image_w;
   h = image_h;
   if (pixmap_return != NULL && bitmap_return != NULL &&
         image_return != NULL && bitmap_image_return != NULL) {
      mag = 1.0;
      if (askForXPmSpec) {
         sprintf(gszMsgBox, TgLoadString(STID_ENTER_GEOM_SPEC_ORIG_SIZE),
               image_w, image_h);
         *mag_spec = '\0';
         Dialog(gszMsgBox, TgLoadCachedString(CSTID_DLG_ACCEPT_CONTINUE),
               mag_spec);
         if (*mag_spec != '\0') {
            ParseCutSpec(mag_spec, image_w, image_h, &mag, &x, &y, &w, &h);
         }
      }
      *pixmap_return = XCreatePixmap(mainDisplay, dummyPixmap, w, h, mainDepth);
      *bitmap_return = XCreatePixmap(mainDisplay, dummyBitmap, w, h, 1);
      if (*pixmap_return == None || *bitmap_return == None) {
         FreeAuxData(fp, *ncolors_return, *color_char, *color_str, pixels,
               pixels_used, pixmap_return, image_return, bitmap_return,
               bitmap_image_return);
         return BitmapNoMemory;
      }
      XFillRectangle(mainDisplay, *pixmap_return, xpmGC, 0, 0, w, h);
      XSetForeground(mainDisplay, xbmGC, 1);
      XFillRectangle(mainDisplay, *bitmap_return, xbmGC, 0, 0, w, h);
      XSetForeground(mainDisplay, xbmGC, 0);
      *image_return = XGetImage(mainDisplay, *pixmap_return, 0, 0, w, h,
            AllPlanes, ZPixmap);
      *bitmap_image_return = XGetImage(mainDisplay, *bitmap_return, 0, 0, w, h,
            1, ZPixmap);
      if (*image_return == NULL || *bitmap_image_return == NULL) {
         MsgBox(TgLoadString(STID_XGETIMAGE_MAY_RUN_OUT_VMEM), TOOL_NAME,
               INFO_MB);
         FreeAuxData(fp, *ncolors_return, *color_char, *color_str, pixels,
               pixels_used, pixmap_return, image_return, bitmap_return,
               bitmap_image_return);
         return BitmapNoMemory;
      }
   }
   if (!BuildXPmBuckets(*ncolors_return, NULL, NULL, chars_per_pixel,
         *color_char, NULL)) {
      FreeAuxData(fp, *ncolors_return, *color_char, *color_str, pixels,
            pixels_used, pixmap_return, image_return, bitmap_return,
            bitmap_image_return);
      return BitmapFileInvalid;
   }
   if (!colorDisplay) {
      if (xpm_data != NULL) {
         xpm_data_ptr = *xpm_data = (char*)malloc(
               image_w*image_h*chars_per_pixel*sizeof(char)+1);
         if (*xpm_data == NULL) FailAllocMessage();
      }
   }
   line = (char*)malloc((image_w*chars_per_pixel+20)*sizeof(char));
   if (line == NULL) FailAllocMessage();
   for (i=0; i < y+h; i++) {
      if (xpm_version == 3 && i == 0) {
         found = FALSE;
         while (fgets(line, image_w*chars_per_pixel+20, fp) != NULL) {
            if (*line == '"') {
               found = TRUE;
               break;
            }
         }
         if (!found) {
            FreeAuxData(fp, *ncolors_return, *color_char, *color_str, pixels,
                  pixels_used, pixmap_return, image_return, bitmap_return,
                  bitmap_image_return);
            return BitmapFileInvalid;
         }
      } else if (fgets(line, image_w*chars_per_pixel+20, fp)==NULL) {
         FreeAuxData(fp, *ncolors_return, *color_char, *color_str, pixels,
               pixels_used, pixmap_return, image_return, bitmap_return,
               bitmap_image_return);
         free(line);
         return BitmapFileInvalid;
      }
      if (i >= y) {
         c_ptr = FindChar((int)'"', line);
         if (xpm_data_ptr != NULL) {
            strncpy(xpm_data_ptr, c_ptr, image_w*chars_per_pixel);
            xpm_data_ptr += image_w*chars_per_pixel;
         }
         if (pixels != NULL) {
            for (j=0; j < x+w; j++, c_ptr+=chars_per_pixel) {
               if (j >= x) {
                  k = XPmLookUp(INVALID, chars_per_pixel, c_ptr, NULL);
                  if (k == INVALID) {
                     FreeAuxData(fp, *ncolors_return, *color_char, *color_str,
                           pixels, pixels_used, pixmap_return, image_return,
                           bitmap_return, bitmap_image_return);
                     free(line);
                     return BitmapFileInvalid;
                  }
                  if (pixels_used != NULL) pixels_used[k] = TRUE;
                  if ((*pixels)[k] == (-1)) {
                     if (bitmap_image_return != NULL) {
                        XPutPixel(*bitmap_image_return, j-x, i-y, 0);
                     }
                     if (image_return != NULL) {
                        XPutPixel(*image_return, j-x, i-y,
                              colorPixels[colorIndex]);
                     }
                  } else {
                     if (image_return != NULL) {
                        XPutPixel(*image_return, j-x, i-y, (*pixels)[k]);
                     }
                  }
               }
            }
         }
      }
   }
   if (xpm_data_ptr != NULL) *xpm_data_ptr = '\0';
   free(line);
   fclose(fp);
   if (pixmap_return != NULL && image_return != NULL) {
      XPutImage(mainDisplay,*pixmap_return,xpmGC,*image_return,0,0,0,0,w,h);
   }
   if (bitmap_return != NULL && bitmap_image_return != NULL) {
      XPutImage(mainDisplay, *bitmap_return, xbmGC, *bitmap_image_return,
            0, 0, 0, 0, w, h);
   }
   if (image_w_return != NULL) *image_w_return = w;
   if (image_h_return != NULL) *image_h_return = h;
   if (w_return != NULL) *w_return = (int)(((float)w) * mag);
   if (h_return != NULL) *h_return = (int)(((float)h) * mag);
   if (pixels != NULL) {
      int new_ncolors=0;

      for (i=0; i < *ncolors_return; i++) {
         if (pixels_used[i]) {
            new_ncolors++;
         }
      }
      if (new_ncolors < *ncolors_return) {
         /* some pixels are not used */
         char *new_color_char=(char*)malloc(
               new_ncolors*chars_per_pixel*sizeof(char));
         char **new_color_str=(char**)malloc(new_ncolors*sizeof(char *));
         int *new_pixels=(int*)malloc(new_ncolors*sizeof(int));

         if (new_color_char == NULL || new_color_str == NULL ||
               new_pixels == NULL) {
            FailAllocMessage();
         } else {
            int new_index=0, ok=TRUE;

            for (i=0; i < *ncolors_return; i++) {
               if (pixels_used[i]) {
                  int j;

                  strncpy(&new_color_char[new_index*chars_per_pixel],
                        &(*color_char)[i*chars_per_pixel],
                        chars_per_pixel*sizeof(char));
                  new_color_str[new_index] = UtilStrDup((*color_str)[i]);
                  if (new_color_str[new_index] == NULL) {
                     for (j=0; j < i; j++) {
                        free(new_color_str[j]);
                     }
                     free(new_color_char);
                     free(new_color_str);
                     free(new_pixels);
                     new_color_char = NULL;
                     new_color_str = NULL;
                     new_pixels = NULL;
                     ok = FALSE;
                     break;
                  }
                  new_pixels[new_index] = (*pixels)[i];
                  new_index++;
               }
            }
            if (ok) {
               free(*color_char);
               *color_char = new_color_char;
               for (i=0; i < *ncolors_return; i++) {
                  free((*color_str)[i]);
               }
               free(*color_str);
               *color_str = new_color_str;
               free(*pixels);
               *pixels = new_pixels;
               if (first_pixel_is_bg_return != NULL &&
                     *first_pixel_is_bg_return && pixels_used[0]) {
                  *first_pixel_is_bg_return = FALSE;
               }
               new_color_char = NULL;
               new_color_str = NULL;
               new_pixels = NULL;
               *ncolors_return = new_ncolors;
            }
         }
         if (new_color_char != NULL) free(new_color_char);
         if (new_color_str != NULL) free(new_color_str);
         if (new_pixels != NULL) free(new_pixels);
      }
   }
   if (pixels_used != NULL) free(pixels_used);
   gnNumNewColorsInPixmapFile = maxColors - saved_max_colors;
   if (gnNumNewColorsInPixmapFile != 0) {
      sprintf(gszMsgBox, TgLoadCachedString(CSTID_GIVEN_ADDTNL_COLOR_ALLOCATED),
            gnNumNewColorsInPixmapFile);
      Msg(gszMsgBox);
   }
   return BitmapSuccess;
}

void ReadXPmObj(FP, Inbuf, ObjPtr)
   FILE *FP;
   char *Inbuf;
   struct ObjRec **ObjPtr;
{
   struct XPmRec *xpm_ptr;
   char color_s[40], trans_color_s[40], *s, inbuf[MAXSTRING], *c_ptr, *line;
   int ltx, lty, rbx, rby, i, j, k, image_w, image_h;
   int ncolors, * pixels, len, index, fill, color_index;
   int *red=NULL, *green=NULL, *blue=NULL;
   int unrecognized_color = FALSE, rotation, chars_per_pixel;
   int first_pixel_is_bg, first_pixel_maybe_bg, new_alloc;
   int id=0, rotate=ROTATE0, flip=NO_FLIP, locked=FALSE;
   int compressed=FALSE, real_x=0, real_y=0;
   int transformed=FALSE, invisible=FALSE, bg_pixel;
   char *xpm_data=NULL, *color_char, **color_str;
   Pixmap pixmap=None, bitmap=None;
   XImage *image=NULL, *bitmap_image=NULL;
   struct XfrmMtrxRec *ctm=NULL;
   struct BBRec orig_obbox;

   *ObjPtr = NULL;

   s = FindChar((int)'(', Inbuf);
   s = ParseStr(s, (int)',', color_s, sizeof(color_s));
   if (fileVersion >= 37) {
      s = ParseStr(s, (int)',', trans_color_s, sizeof(trans_color_s));
   }
   InitScan(s, "\t\n, ");

   rotation = 0;
   chars_per_pixel = 1;
   first_pixel_maybe_bg = TRUE;
   first_pixel_is_bg = TRUE;
   if (fileVersion <= 9) {
      sprintf(gszMsgBox, TgLoadCachedString(CSTID_INVALID_FILEVER_FOR_XPM),
            fileVersion);
      if (PRTGIF) {
         fprintf(stderr, "%s\n", gszMsgBox);
      } else {
         Msg(gszMsgBox);
      }
      return;
   } else if (fileVersion <= 13) {
      if (GETINT("xpixmap", ltx,      "ltx") == INVALID ||
          GETINT("xpixmap", lty,      "lty") == INVALID ||
          GETINT("xpixmap", rbx,      "rbx") == INVALID ||
          GETINT("xpixmap", rby,      "rby") == INVALID ||
          GETINT("xpixmap", fill,     "fill") == INVALID ||
          GETINT("xpixmap", ncolors,  "ncolors") == INVALID ||
          GETINT("xpixmap", id,       "id") == INVALID) {
         return;
      }
      if (id >= objId) objId = id+1;
   } else if (fileVersion <= 14) {
      if (GETINT("xpixmap", ltx,      "ltx") == INVALID ||
          GETINT("xpixmap", lty,      "lty") == INVALID ||
          GETINT("xpixmap", rbx,      "rbx") == INVALID ||
          GETINT("xpixmap", rby,      "rby") == INVALID ||
          GETINT("xpixmap", fill,     "fill") == INVALID ||
          GETINT("xpixmap", ncolors,  "ncolors") == INVALID ||
          GETINT("xpixmap", id,       "id") == INVALID ||
          GETINT("xpixmap", rotation, "rotation") == INVALID) {
         return;
      }
      if (id >= objId) objId = id+1;
   } else if (fileVersion <= 22) {
      if (GETINT("xpixmap", ltx,              "ltx") == INVALID ||
          GETINT("xpixmap", lty,              "lty") == INVALID ||
          GETINT("xpixmap", rbx,              "rbx") == INVALID ||
          GETINT("xpixmap", rby,              "rby") == INVALID ||
          GETINT("xpixmap", fill,             "fill") == INVALID ||
          GETINT("xpixmap", ncolors,          "ncolors") == INVALID ||
          GETINT("xpixmap", chars_per_pixel,  "chars_per_pixel") == INVALID ||
          GETINT("xpixmap", first_pixel_is_bg,"first_pixel_is_bg") == INVALID ||
          GETINT("xpixmap", id,               "id") == INVALID ||
          GETINT("xpixmap", rotation,         "rotation") == INVALID) {
         return;
      }
      if (id >= objId) objId = id+1;
      first_pixel_maybe_bg = FALSE;
   } else if (fileVersion <= 25) {
      if (GETINT("xpixmap", ltx,              "ltx") == INVALID ||
          GETINT("xpixmap", lty,              "lty") == INVALID ||
          GETINT("xpixmap", rbx,              "rbx") == INVALID ||
          GETINT("xpixmap", rby,              "rby") == INVALID ||
          GETINT("xpixmap", fill,             "fill") == INVALID ||
          GETINT("xpixmap", ncolors,          "ncolors") == INVALID ||
          GETINT("xpixmap", chars_per_pixel,  "chars_per_pixel") == INVALID ||
          GETINT("xpixmap", first_pixel_is_bg,"first_pixel_is_bg") == INVALID ||
          GETINT("xpixmap", id,               "id") == INVALID ||
          GETINT("xpixmap", rotation,         "rotation") == INVALID ||
          GETINT("xpixmap", image_w,          "image_w") == INVALID ||
          GETINT("xpixmap", image_h,          "image_h") == INVALID ||
          GETINT("xpixmap", rotate,           "rotate") == INVALID ||
          GETINT("xpixmap", flip,             "flip") == INVALID) {
         return;
      }
      if (id >= objId) objId = id+1;
      first_pixel_maybe_bg = FALSE;
   } else if (fileVersion <= 32) {
      if (GETINT("xpixmap", ltx,              "ltx") == INVALID ||
          GETINT("xpixmap", lty,              "lty") == INVALID ||
          GETINT("xpixmap", rbx,              "rbx") == INVALID ||
          GETINT("xpixmap", rby,              "rby") == INVALID ||
          GETINT("xpixmap", fill,             "fill") == INVALID ||
          GETINT("xpixmap", ncolors,          "ncolors") == INVALID ||
          GETINT("xpixmap", chars_per_pixel,  "chars_per_pixel") == INVALID ||
          GETINT("xpixmap", first_pixel_is_bg,"first_pixel_is_bg") == INVALID ||
          GETINT("xpixmap", id,               "id") == INVALID ||
          GETINT("xpixmap", rotation,         "rotation") == INVALID ||
          GETINT("xpixmap", image_w,          "image_w") == INVALID ||
          GETINT("xpixmap", image_h,          "image_h") == INVALID ||
          GETINT("xpixmap", rotate,           "rotate") == INVALID ||
          GETINT("xpixmap", flip,             "flip") == INVALID ||
          GETINT("xpixmap", locked,           "locked") == INVALID) {
         return;
      }
      if (id >= objId) objId = id+1;
      first_pixel_maybe_bg = FALSE;
   } else {
      if (GETINT("xpixmap", ltx,              "ltx") == INVALID ||
          GETINT("xpixmap", lty,              "lty") == INVALID ||
          GETINT("xpixmap", rbx,              "rbx") == INVALID ||
          GETINT("xpixmap", rby,              "rby") == INVALID ||
          GETINT("xpixmap", fill,             "fill") == INVALID ||
          GETINT("xpixmap", ncolors,          "ncolors") == INVALID ||
          GETINT("xpixmap", chars_per_pixel,  "chars_per_pixel") == INVALID ||
          GETINT("xpixmap", first_pixel_is_bg,"first_pixel_is_bg") == INVALID ||
          GETINT("xpixmap", id,               "id") == INVALID ||
          GETINT("xpixmap", rotation,         "rotation") == INVALID ||
          GETINT("xpixmap", image_w,          "image_w") == INVALID ||
          GETINT("xpixmap", image_h,          "image_h") == INVALID ||
          GETINT("xpixmap", rotate,           "rotate") == INVALID ||
          GETINT("xpixmap", flip,             "flip") == INVALID ||
          GETINT("xpixmap", locked,           "locked") == INVALID ||
          GETINT("xpixmap", compressed,       "compressed") == INVALID ||
          GETINT("xpixmap", transformed,      "transformed") == INVALID ||
          GETINT("xpixmap", invisible,        "invisible") == INVALID) {
         return;
      }
      if (id >= objId) objId = id+1;
      first_pixel_maybe_bg = FALSE;
   }
   if (fileVersion <= 22) {
      image_w = rbx-ltx;
      image_h = rby-lty;
      rotate = ROTATE0;
      flip = 0;
   }
   if (fileVersion >= 33 && transformed) {
      (void)fgets(inbuf, MAXSTRING, FP);
      scanLineNum++;
      InitScan(inbuf, "\t\n, ");

      ctm = (struct XfrmMtrxRec *)malloc(sizeof(struct XfrmMtrxRec));
      if (ctm == NULL) FailAllocMessage();
      if (GETINT("xpixmap", real_x,           "real_x") == INVALID ||
          GETINT("xpixmap", real_y,           "real_y") == INVALID ||
          GETINT("xpixmap", orig_obbox.ltx,   "orig_obbox.ltx") == INVALID ||
          GETINT("xpixmap", orig_obbox.lty,   "orig_obbox.lty") == INVALID ||
          GETINT("xpixmap", orig_obbox.rbx,   "orig_obbox.rbx") == INVALID ||
          GETINT("xpixmap", orig_obbox.rby,   "orig_obbox.rby") == INVALID ||
          GETDBL("xpixmap", ctm->m[CTM_SX],   "CTM_SX") == INVALID ||
          GETDBL("xpixmap", ctm->m[CTM_SIN],  "CTM_SIN") == INVALID ||
          GETDBL("xpixmap", ctm->m[CTM_MSIN], "CTM_MSIN") == INVALID ||
          GETDBL("xpixmap", ctm->m[CTM_SY],   "CTM_SY") == INVALID ||
          GETINT("xpixmap", ctm->t[CTM_TX],   "CTM_TX") == INVALID ||
          GETINT("xpixmap", ctm->t[CTM_TY],   "CTM_TY") == INVALID) {
         return;
      }
   }
   fill = UpgradePenFill(fill);

   *ObjPtr = (struct ObjRec *)malloc(sizeof(struct ObjRec));
   if (*ObjPtr == NULL) FailAllocMessage();
   memset(*ObjPtr, 0, sizeof(struct ObjRec));
   xpm_ptr = (struct XPmRec *)malloc(sizeof(struct XPmRec));
   if (xpm_ptr == NULL) FailAllocMessage();
   memset(xpm_ptr, 0, sizeof(struct XPmRec));

   color_index = QuickFindColorIndex(*ObjPtr, color_s, &new_alloc, TRUE);

   (*ObjPtr)->color = color_index;
   if (mainDisplay != NULL) {
      UtilStrCpyN((*ObjPtr)->color_str, sizeof((*ObjPtr)->color_str),
            colorMenuItems[color_index]);
   }
   (*ObjPtr)->dirty = FALSE;
   (*ObjPtr)->id = id;
   (*ObjPtr)->rotation = rotation;
   (*ObjPtr)->locked = locked;
   (*ObjPtr)->type = OBJ_XPM;
   (*ObjPtr)->obbox.ltx = (*ObjPtr)->bbox.ltx = (*ObjPtr)->x = ltx;
   (*ObjPtr)->obbox.lty = (*ObjPtr)->bbox.lty = (*ObjPtr)->y = lty;
   (*ObjPtr)->obbox.rbx = (*ObjPtr)->bbox.rbx = rbx;
   (*ObjPtr)->obbox.rby = (*ObjPtr)->bbox.rby = rby;
   (*ObjPtr)->detail.xpm = xpm_ptr;
   (*ObjPtr)->ctm = ctm;
   (*ObjPtr)->invisible = invisible;

   if (ctm != NULL) {
      memcpy(&(*ObjPtr)->orig_obbox, &orig_obbox, sizeof(struct BBRec));
      (*ObjPtr)->x = real_x;
      (*ObjPtr)->y = real_y;
      GetTransformedOBBoxOffsetVs(*ObjPtr, (*ObjPtr)->rotated_obbox);
   }
   color_char = (char*)malloc(ncolors*chars_per_pixel*sizeof(char));
   color_str = (char**)malloc(ncolors*sizeof(char*));
   pixels = (int*)malloc(ncolors*sizeof(int));
   if (color_char == NULL || color_str == NULL || pixels == NULL) {
      FailAllocMessage();
   }
   if (fileVersion >= 25 && ((PRTGIF && !cmdLineOpenDisplay) ||
         !colorDisplay)) {
      red = (int*)malloc(ncolors*sizeof(int));
      green = (int*)malloc(ncolors*sizeof(int));
      blue = (int*)malloc(ncolors*sizeof(int));
      if (red == NULL || green == NULL || blue == NULL) FailAllocMessage();
   }
   xpm_ptr->pixmap = None;
   xpm_ptr->image = NULL;
   xpm_ptr->bitmap = None;
   xpm_ptr->bitmap_image = NULL;
   xpm_ptr->cached_pixmap = None;
   xpm_ptr->cached_bitmap = None;
   xpm_ptr->cached_zoom = 0;
   xpm_ptr->data = NULL;
   xpm_ptr->fill = fill;
   xpm_ptr->flip = flip;
   xpm_ptr->cached_flip = 0;
   xpm_ptr->cached_w = xpm_ptr->cached_h = 0;
   xpm_ptr->cached_color = (-1);
   xpm_ptr->image_w = image_w;
   xpm_ptr->image_h = image_h;

   xpm_ptr->ncolors = ncolors;
   xpm_ptr->chars_per_pixel = chars_per_pixel;
   xpm_ptr->first_pixel_is_bg = first_pixel_is_bg;
   xpm_ptr->color_char = color_char;
   xpm_ptr->color_str = color_str;
   xpm_ptr->pixels = pixels;
   xpm_ptr->red = red;
   xpm_ptr->green = green;
   xpm_ptr->blue = blue;

   bg_pixel = GetDrawingBgPixel(INVALID, INVALID);
   for (i=0; i < ncolors; i++) {
      (void)fgets(inbuf, MAXSTRING, FP);
      scanLineNum++;
      c_ptr = FindChar((int)'"', inbuf);
      for (j = 0; j < chars_per_pixel; j++) {
         color_char[i*chars_per_pixel+j] = c_ptr[j];
      }
      c_ptr = FindChar((int)'"', c_ptr);
      c_ptr = FindChar((int)'"', c_ptr);
      c_ptr = ParseStr(c_ptr, (int)'"', color_s, sizeof(color_s));
      if (!PRTGIF || cmdLineOpenDisplay) {
         if (i == 0 && (first_pixel_is_bg || (first_pixel_maybe_bg &&
               (color_char[i]=='`' || color_char[i]==' ')))) {
            strcpy(color_s, (myFileBgColorStr==NULL ? myBgColorStr :
                  myFileBgColorStr));
            pixels[i] = bg_pixel;
            xpm_ptr->first_pixel_is_bg = first_pixel_is_bg = TRUE;
         } else if (UtilStrICmp(color_s, "None") == 0) {
            pixels[i] = (-1);
         } else if ((index = QuickFindColorIndex(NULL, color_s, &new_alloc,
               TRUE)) == INVALID) {
            sprintf(gszMsgBox,
                  TgLoadCachedString(CSTID_CANNOT_ALLOC_COLOR_USE_ALT),
                  color_s, colorMenuItems[colorIndex]);
            Msg(gszMsgBox);
            allocColorFailed = TRUE;
            strcpy(color_s, colorMenuItems[colorIndex]);
            pixels[i] = colorPixels[colorIndex];
         } else {
            pixels[i] = colorPixels[index];
         }

         len = strlen(color_s);
         color_str[i] = (char*)malloc((len+1)*sizeof(char));
         if (color_str[i] == NULL) FailAllocMessage();
         strcpy(color_str[i], color_s);
      }
      if (fileVersion >= 25 && ((PRTGIF && !cmdLineOpenDisplay) ||
            !colorDisplay)) {
         /* has RGB information for PRTGIF */
         InitScan(c_ptr, "\t\n, ");

         if (GETINT("xpixmap", red[i],   "red") == INVALID ||
             GETINT("xpixmap", green[i], "green") == INVALID ||
             GETINT("xpixmap", blue[i],  "blue") == INVALID) {
            return;
         }
         if (UtilStrICmp(color_s, "None") == 0) {
            red[i] = green[i] = blue[i] = (-1);
         } else if (PRTGIF && cmdLineColor && (prTgifFoundColorInfo ||
               !cmdLineOpenDisplay)) {
            int found_index=PrTgifFindColorIndex(NULL, color_s);

            if (found_index != INVALID) {
               red[i] =
                     (int)(10000*((int)tgifColors[found_index].red)/maxRGB);
               green[i] =
                     (int)(10000*((int)tgifColors[found_index].green)/maxRGB);
               blue[i] =
                     (int)(10000*((int)tgifColors[found_index].blue)/maxRGB);
            }
         }
      }
   }
   xpm_ptr->clip_mask = None;

   if (PRTGIF && !cmdLineOpenDisplay) {
      xpm_data = (char*)malloc(image_w*image_h*chars_per_pixel*sizeof(char));
      if (xpm_data == NULL) FailAllocMessage();
      xpm_ptr->data = xpm_data;

      line = (char*)malloc((image_w*chars_per_pixel+20)*sizeof(char));
      if (line == NULL) FailAllocMessage();
      for (i=0; i < image_h; i++, xpm_data += image_w*chars_per_pixel) {
         (void)fgets(line, image_w*chars_per_pixel+20, FP);
         scanLineNum++;
         c_ptr = &line[4];
         strncpy(xpm_data, c_ptr, image_w*chars_per_pixel);
      }
      free(line);
   } else {
      if (fileVersion >= 25 && !colorDisplay) {
         xpm_data = (char*)malloc(image_w*image_h*chars_per_pixel*sizeof(char));
         if (xpm_data == NULL) FailAllocMessage();
         xpm_ptr->data = xpm_data;
      }
      pixmap = XCreatePixmap(mainDisplay, dummyPixmap, image_w, image_h,
            mainDepth);
      bitmap = XCreatePixmap(mainDisplay, dummyPixmap, image_w, image_h, 1);
      XFillRectangle(mainDisplay, pixmap, xpmGC, 0, 0, image_w, image_h);
      XSetForeground(mainDisplay, xbmGC, 1);
      XFillRectangle(mainDisplay, bitmap, xbmGC, 0, 0, image_w, image_h);
      XSetForeground(mainDisplay, xbmGC, 0);
      image = XGetImage(mainDisplay, pixmap, 0, 0, image_w, image_h, AllPlanes,
            ZPixmap);
      bitmap_image = XGetImage(mainDisplay, bitmap, 0, 0, image_w, image_h, 1,
            ZPixmap);
      if (image == NULL || bitmap_image == NULL) {
         MsgBox(TgLoadString(STID_XGETIMAGE_MAY_RUN_OUT_VMEM), TOOL_NAME,
               INFO_MB);
         XFreePixmap(mainDisplay, pixmap);
         XFreePixmap(mainDisplay, bitmap);
         if (image != NULL) XDestroyImage(image);
         if (bitmap_image != NULL) XDestroyImage(bitmap_image);
         return;
      }
      if (!BuildXPmBuckets(ncolors, NULL, NULL, chars_per_pixel, color_char,
            NULL)) {
         XFreePixmap(mainDisplay, pixmap);
         XFreePixmap(mainDisplay, bitmap);
         if (image != NULL) XDestroyImage(image);
         if (bitmap_image != NULL) XDestroyImage(bitmap_image);
         return;
      }
      line = (char*)malloc((image_w*chars_per_pixel+20)*sizeof(char));
      if (line == NULL) FailAllocMessage();
      for (i=0; i < image_h; i++) {
         (void)fgets(line, image_w*chars_per_pixel+20, FP);
         scanLineNum++;
         c_ptr = &line[4];
         if (xpm_data != NULL) {
            strncpy(xpm_data, c_ptr, image_w*chars_per_pixel);
         }
         for (j = 0; j < image_w; j++, c_ptr+=chars_per_pixel) {
            k = XPmLookUp(INVALID, chars_per_pixel, c_ptr, NULL);
            if (k == INVALID) {
               XPutPixel(image, j, i, pixels[0]);
               unrecognized_color = TRUE;
            } else if (pixels[k] == (-1)) {
               XPutPixel(bitmap_image, j, i, 0);
               XPutPixel(image, j, i, colorPixels[color_index]);
            } else {
               XPutPixel(image, j, i, pixels[k]);
            }
         }
         if (xpm_data != NULL) xpm_data += image_w*chars_per_pixel;
      }
      free(line);
      XPutImage(mainDisplay,pixmap,xpmGC,image,0,0,0,0,image_w,image_h);
      XPutImage(mainDisplay,bitmap,xbmGC,bitmap_image,0,0,0,0,image_w,image_h);
      xpm_ptr->pixmap = pixmap;
      xpm_ptr->image = image;
      xpm_ptr->bitmap = bitmap;
      xpm_ptr->bitmap_image = bitmap_image;

      if (unrecognized_color) {
         sprintf(gszMsgBox,
               TgLoadCachedString(CSTID_COLOR_SUBS_FOR_UNRECOG_IN_XPM),
               color_str[0]);
         Msg(gszMsgBox);
      }
   }
   if (fileVersion < 33 && (rotate != ROTATE0 || flip != NO_FLIP)) {
      double dz=(double)0.0, d1=(double)1000.0, dm1=((double)-1000.0);

      if (rotate == ROTATE90 || rotate == ROTATE270) {
         int h=(*ObjPtr)->obbox.rbx-(*ObjPtr)->obbox.ltx;
         int w=(*ObjPtr)->obbox.rby-(*ObjPtr)->obbox.lty;

         (*ObjPtr)->obbox.rby = (*ObjPtr)->obbox.lty + h;
         (*ObjPtr)->obbox.rbx = (*ObjPtr)->obbox.ltx + w;
      }
      ltx = ((*ObjPtr)->obbox.ltx);
      lty = ((*ObjPtr)->obbox.lty);
      SetRotatePivotByObject(*ObjPtr);

      if (flip & HORI_EVEN) {
         ShearObj(*ObjPtr, CORNER_LEFT, dz, dz, dm1, d1, &ltx, &lty);
      }
      if (flip & VERT_EVEN) {
         ShearObj(*ObjPtr, CORNER_TOP, dz, dz, d1, dm1, &ltx, &lty);
      }
      if (rotate == ROTATE0) {
         if (flip & (HORI_ODD | VERT_ODD)) {
            RotateObj(*ObjPtr, CORNER_LT, CLOCKWISE90, &ltx, &lty);
            if (flip & HORI_ODD) {
               ShearObj(*ObjPtr, CORNER_LEFT, dz, dz, dm1, d1, &ltx, &lty);
            }
            if (flip & VERT_ODD) {
               ShearObj(*ObjPtr, CORNER_TOP, dz, dz, d1, dm1, &ltx, &lty);
            }
            RotateObj(*ObjPtr, CORNER_LT, COUNTER90, &ltx, &lty);
         }
      } else {
         switch (rotate) {
         case ROTATE90:
            RotateObj(*ObjPtr, CORNER_LT, CLOCKWISE90, &ltx, &lty);
            if (flip & HORI_ODD) {
               ShearObj(*ObjPtr, CORNER_LEFT, dz, dz, dm1, d1, &ltx, &lty);
            }
            if (flip & VERT_ODD) {
               ShearObj(*ObjPtr, CORNER_TOP, dz, dz, d1, dm1, &ltx, &lty);
            }
            break;
         case ROTATE180:
            RotateObj(*ObjPtr, CORNER_LT, CLOCKWISE90, &ltx, &lty);
            if (flip & HORI_ODD) {
               ShearObj(*ObjPtr, CORNER_LEFT, dz, dz, dm1, d1, &ltx, &lty);
            }
            if (flip & VERT_ODD) {
               ShearObj(*ObjPtr, CORNER_TOP, dz, dz, d1, dm1, &ltx, &lty);
            }
            RotateObj(*ObjPtr, CORNER_LT, CLOCKWISE90, &ltx, &lty);
            break;
         case ROTATE270:
            RotateObj(*ObjPtr, CORNER_LT, CLOCKWISE90, &ltx, &lty);
            if (flip & HORI_ODD) {
               ShearObj(*ObjPtr, CORNER_LEFT, dz, dz, dm1, d1, &ltx, &lty);
            }
            if (flip & VERT_ODD) {
               ShearObj(*ObjPtr, CORNER_TOP, dz, dz, d1, dm1, &ltx, &lty);
            }
            RotateObj(*ObjPtr, CORNER_LT, CLOCKWISE90, &ltx, &lty);
            RotateObj(*ObjPtr, CORNER_LT, CLOCKWISE90, &ltx, &lty);
            break;
         }
      }
      xpm_ptr->flip = NO_FLIP;
   }
}

static
struct ObjRec *MyReadJpegFileSkipData(jpeg_filename, expected_image_w,
      expected_image_h)
   char *jpeg_filename;
   int expected_image_w, expected_image_h;
{
   FILE *ppm_fp=NULL;
   char tmp_fname[MAXPATHLENGTH+1];
   int rc=0, format=0, image_w=0, image_h=0, max_val=0;
   struct ObjRec *obj_ptr=NULL;

   SetWatchCursor(drawWindow);
   SetWatchCursor(mainWindow);
   SaveStatusStrings();
   rc = ConvertJpegToPpm6(jpeg_filename, tmp_fname, sizeof(tmp_fname));
   RestoreStatusStrings();
   SetDefaultCursor(mainWindow);
   ShowCursor();
   if (!rc) {
      return NULL;
   }
   ppm_fp = fopen(tmp_fname, "r");
   if (ppm_fp == NULL) {
      FailToOpenMessage(tmp_fname, "r", NULL);
      return NULL;
   }
   if (!ReadPpmHeader(ppm_fp, tmp_fname, &format, &image_w, &image_h,
         &max_val)) {
      fclose(ppm_fp);
      return NULL;
   }
   fclose(ppm_fp);

   if (expected_image_w != (-1) && expected_image_h != (-1) &&
         (image_w != expected_image_w || image_h != expected_image_h)) {
      sprintf(gszMsgBox, TgLoadString(STID_JPEG_DIM_NOT_MATCH_DELETED), image_w,
            image_h, jpeg_filename, expected_image_w, expected_image_h);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return NULL;
   }
   obj_ptr = CreateXPmObj(image_w, image_h, image_w, image_h, None, NULL, None,
         NULL, (-1), 1, FALSE, NULL, NULL, NULL, NULL);

   unlink(tmp_fname);

   return obj_ptr;
}

static
struct ObjRec *MyReadJpegFile(jpeg_filename, expected_image_w, expected_image_h)
   char *jpeg_filename;
   int expected_image_w, expected_image_h;
{
   char tmp_fname[MAXPATHLENGTH+1];
   int rc=0, ncolors=0, chars_per_pixel=0, *pixels=NULL;
   int first_pixel_is_bg=FALSE, image_w=0, image_h=0, w=0, h=0;
   Pixmap pixmap=None, bitmap=None;
   XImage *image=NULL, *bitmap_image=NULL;
   char *color_char, **color_str=NULL, *xpm_data=NULL;
   struct ObjRec *obj_ptr=NULL;

   if (fullTrueColorMode && HasZlibSupport()) {
      struct XPmRec *xpm_ptr=NULL;
      char ppm6_fname[MAXPATHLENGTH+1];

      rc = ConvertJpegToPpm6(jpeg_filename, ppm6_fname, sizeof(ppm6_fname));
      if (!rc) return NULL;

      ResetPngHeaderInfo(&gPngHeaderInfo);
      obj_ptr = CreatePpmTrueObjFromFile(ppm6_fname);
      unlink(ppm6_fname);

      if (obj_ptr == NULL) return NULL;

      xpm_ptr = obj_ptr->detail.xpm;
      xpm_ptr->real_type = PPM_TRUE;
      xpm_ptr->ppm_data_compress = PPM_JPEG_COMPRESS;
      xpm_ptr->ppm_data = ReadFileIntoBuf( jpeg_filename,
            &xpm_ptr->ppm_data_size);
      xpm_ptr->ppm_mask_data = NULL;
      xpm_ptr->ppm_mask_size = 0;

      if (expected_image_w != (-1) && expected_image_h != (-1) &&
            (xpm_ptr->image_w != expected_image_w ||
            xpm_ptr->image_h != expected_image_h)) {
         sprintf(gszMsgBox, TgLoadString(STID_JPEG_DIM_NOT_MATCH_DELETED),
               xpm_ptr->image_w, xpm_ptr->image_h, jpeg_filename,
               expected_image_w, expected_image_h);
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
         return NULL;
      }
      return obj_ptr;
   }
   SetWatchCursor(drawWindow);
   SetWatchCursor(mainWindow);
   SaveStatusStrings();
   *tmp_fname = '\0';
   rc = ConvertJpegToXpm(jpeg_filename, tmp_fname, sizeof(tmp_fname));
   RestoreStatusStrings();
   SetDefaultCursor(mainWindow);
   ShowCursor();
   if (!rc) {
      return NULL;
   }
   SetWatchCursor(drawWindow);
   SetWatchCursor(mainWindow);
   rc = MyReadPixmapFile(tmp_fname,
         &image_w, &image_h, &w, &h, &pixmap, &image, &bitmap, &bitmap_image,
         &ncolors, &chars_per_pixel, &first_pixel_is_bg, &color_char,
         &color_str, &pixels, &xpm_data);
   SetDefaultCursor(mainWindow);
   ShowCursor();

   if (rc != BitmapSuccess) {
      sprintf(gszMsgBox, TgLoadString(STID_CANNOT_IMPORT_XPM_FILE), tmp_fname);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return NULL;
   }
   if (expected_image_w != (-1) && expected_image_h != (-1) &&
         (image_w != expected_image_w || image_h != expected_image_h)) {
      sprintf(gszMsgBox, TgLoadString(STID_JPEG_DIM_NOT_MATCH_DELETED), image_w,
            image_h, jpeg_filename, expected_image_w, expected_image_h);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return NULL;
   }
   obj_ptr = CreateXPmObj(image_w, image_h, w, h, pixmap, image, bitmap,
         bitmap_image, ncolors, chars_per_pixel, first_pixel_is_bg, color_char,
         color_str, pixels, xpm_data);

   unlink(tmp_fname);

   return obj_ptr;
}

void ReadJpegObj(FP, Inbuf, ObjPtr)
   FILE *FP;
   char *Inbuf;
   struct ObjRec **ObjPtr;
{
   struct XPmRec *xpm_ptr=NULL;
   char color_s[40], trans_color_s[40], *s=NULL, inbuf[MAXSTRING];
   char *jpeg_filename=NULL;
   int ltx, lty, rbx, rby, image_w=0, image_h=0;
   int ncolors, fill, color_index;
   int rotation, chars_per_pixel;
   int first_pixel_is_bg, first_pixel_maybe_bg, new_alloc;
   int id=0, rotate=ROTATE0, flip=NO_FLIP, locked=FALSE;
   int compressed=FALSE, real_x=0, real_y=0, real_type=0, linked_jpeg=FALSE;
   int transformed=FALSE, invisible=FALSE;
   struct XfrmMtrxRec *ctm=NULL;
   struct BBRec orig_obbox;
   struct ObjRec *new_obj_ptr=NULL;

   *ObjPtr = NULL;

   s = FindChar((int)'(', Inbuf);
   s = ParseStr(s, (int)',', color_s, sizeof(color_s));
   if (fileVersion >= 37) {
      s = ParseStr(s, (int)',', trans_color_s, sizeof(trans_color_s));
   }
   InitScan(s, "\t\n, ");

   rotation = 0;
   chars_per_pixel = 1;
   first_pixel_maybe_bg = TRUE;
   first_pixel_is_bg = TRUE;
   if (fileVersion < 37) {
      sprintf(gszMsgBox,
            TgLoadCachedString(CSTID_INVALID_FILEVER_FOR_LINK_JPEG),
            fileVersion);
      if (PRTGIF) {
         fprintf(stderr, "%s\n", gszMsgBox);
      } else {
         Msg(gszMsgBox);
      }
      return;
   } else {
      if (GETINT("jpeg", ltx,              "ltx") == INVALID ||
          GETINT("jpeg", lty,              "lty") == INVALID ||
          GETINT("jpeg", rbx,              "rbx") == INVALID ||
          GETINT("jpeg", rby,              "rby") == INVALID ||
          GETINT("jpeg", fill,             "fill") == INVALID ||
          GETINT("jpeg", ncolors,          "ncolors") == INVALID ||
          GETINT("jpeg", chars_per_pixel,  "chars_per_pixel") == INVALID ||
          GETINT("jpeg", first_pixel_is_bg,"first_pixel_is_bg") == INVALID ||
          GETINT("jpeg", id,               "id") == INVALID ||
          GETINT("jpeg", rotation,         "rotation") == INVALID ||
          GETINT("jpeg", image_w,          "image_w") == INVALID ||
          GETINT("jpeg", image_h,          "image_h") == INVALID ||
          GETINT("jpeg", rotate,           "rotate") == INVALID ||
          GETINT("jpeg", flip,             "flip") == INVALID ||
          GETINT("jpeg", locked,           "locked") == INVALID ||
          GETINT("jpeg", compressed,       "compressed") == INVALID ||
          GETINT("jpeg", transformed,      "transformed") == INVALID ||
          GETINT("jpeg", invisible,        "invisible") == INVALID ||
          GETINT("jpeg", real_type,        "real_type") == INVALID ||
          GETINT("jpeg", linked_jpeg,      "linked_jpeg") == INVALID) {
         return;
      }
      if (id >= objId) objId = id+1;
      first_pixel_maybe_bg = FALSE;
   }
   if (real_type == XPM_JPEG && linked_jpeg) {
      char *tmp_str=NULL;

      (void)fgets(inbuf, MAXSTRING, FP);
      scanLineNum++;

      tmp_str = FindChar((int)'"', inbuf);
      s = ReadString(tmp_str);
      *(--s) = '\0';
      jpeg_filename = UtilStrDup(tmp_str);
      if (jpeg_filename == NULL) FailAllocMessage();
   } else {
      strcpy(gszMsgBox, TgLoadString(STID_INVALID_PARAM_LINKED_JPEG));
      if (PRTGIF) {
         fprintf(stderr, "%s\n", gszMsgBox);
      } else {
         Msg(gszMsgBox);
      }
      return;
   }
   if (transformed) {
      (void)fgets(inbuf, MAXSTRING, FP);
      scanLineNum++;
      InitScan(inbuf, "\t\n, ");

      ctm = (struct XfrmMtrxRec *)malloc(sizeof(struct XfrmMtrxRec));
      if (ctm == NULL) FailAllocMessage();
      if (GETINT("xpixmap", real_x,           "real_x") == INVALID ||
          GETINT("xpixmap", real_y,           "real_y") == INVALID ||
          GETINT("xpixmap", orig_obbox.ltx,   "orig_obbox.ltx") == INVALID ||
          GETINT("xpixmap", orig_obbox.lty,   "orig_obbox.lty") == INVALID ||
          GETINT("xpixmap", orig_obbox.rbx,   "orig_obbox.rbx") == INVALID ||
          GETINT("xpixmap", orig_obbox.rby,   "orig_obbox.rby") == INVALID ||
          GETDBL("xpixmap", ctm->m[CTM_SX],   "CTM_SX") == INVALID ||
          GETDBL("xpixmap", ctm->m[CTM_SIN],  "CTM_SIN") == INVALID ||
          GETDBL("xpixmap", ctm->m[CTM_MSIN], "CTM_MSIN") == INVALID ||
          GETDBL("xpixmap", ctm->m[CTM_SY],   "CTM_SY") == INVALID ||
          GETINT("xpixmap", ctm->t[CTM_TX],   "CTM_TX") == INVALID ||
          GETINT("xpixmap", ctm->t[CTM_TY],   "CTM_TY") == INVALID) {
         return;
      }
   }
   fill = UpgradePenFill(fill);

   if (!UtilPathExists(jpeg_filename)) {
      sprintf(gszMsgBox, TgLoadString(STID_INVALID_PATH_WHILE_READ_JPEG),
            jpeg_filename);
      if (PRTGIF) {
         fprintf(stderr, "%s\n", gszMsgBox);
      } else {
         Msg(gszMsgBox);
      }
      if (ctm != NULL) free(ctm);
      UtilFree(jpeg_filename);
      return;
   }
   if (PRTGIF && !cmdLineOpenDisplay) {
      new_obj_ptr = MyReadJpegFileSkipData(jpeg_filename, image_w, image_h);
   } else {
      new_obj_ptr = MyReadJpegFile(jpeg_filename, image_w, image_h);
   }
   if (new_obj_ptr == NULL) return;

   new_obj_ptr->id = id;
   MoveObj(new_obj_ptr, ltx, lty);
   if (ctm != NULL) {
      new_obj_ptr->obbox.ltx = new_obj_ptr->bbox.ltx = new_obj_ptr->x = ltx;
      new_obj_ptr->obbox.lty = new_obj_ptr->bbox.lty = new_obj_ptr->y = lty;
      new_obj_ptr->obbox.rbx = new_obj_ptr->bbox.rbx = rbx;
      new_obj_ptr->obbox.rby = new_obj_ptr->bbox.rby = rby;
      new_obj_ptr->ctm = ctm;

      memcpy(&new_obj_ptr->orig_obbox, &orig_obbox, sizeof(struct BBRec));
      new_obj_ptr->x = real_x;
      new_obj_ptr->y = real_y;
      GetTransformedOBBoxOffsetVs(new_obj_ptr, new_obj_ptr->rotated_obbox);
   }
   xpm_ptr = new_obj_ptr->detail.xpm;

   xpm_ptr->real_type = real_type;
   xpm_ptr->linked_jpeg = linked_jpeg;
   xpm_ptr->filename = jpeg_filename;

   color_index = QuickFindColorIndex(new_obj_ptr, color_s, &new_alloc, TRUE);
   new_obj_ptr->color = color_index;
   UtilStrCpyN(new_obj_ptr->color_str, sizeof(new_obj_ptr->color_str), color_s);

   *ObjPtr = new_obj_ptr;
}

static
struct ObjRec *MyReadPpmTrueFileSkipData(ppm_fname, expected_image_w,
      expected_image_h)
   char *ppm_fname;
   int expected_image_w, expected_image_h;
{
   FILE *ppm_fp=NULL;
   int format=0, image_w=0, image_h=0, max_val=0;
   struct ObjRec *obj_ptr=NULL;
   struct XPmRec *xpm_ptr=NULL;

   ppm_fp = fopen(ppm_fname, "r");
   if (ppm_fp == NULL) {
      FailToOpenMessage(ppm_fname, "r", NULL);
      return NULL;
   }
   if (!ReadPpmHeader(ppm_fp, ppm_fname, &format, &image_w, &image_h,
         &max_val)) {
      fclose(ppm_fp);
      return NULL;
   }
   fclose(ppm_fp);
   
   if (expected_image_w != (-1) && expected_image_h != (-1) &&
         (image_w != expected_image_w || image_h != expected_image_h)) {
      sprintf(gszMsgBox, TgLoadString(STID_PPM6_DIM_NOT_MATCH), image_w,
            image_h, ppm_fname, expected_image_w, expected_image_h);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return NULL;
   }
   obj_ptr = CreateXPmObj(image_w, image_h, image_w, image_h, None, NULL, None,
         NULL, (-1), 1, FALSE, NULL, NULL, NULL, NULL);
   if (obj_ptr == NULL) return NULL;

   xpm_ptr = obj_ptr->detail.xpm;
   UtilStrCpyN(xpm_ptr->tmp_ppm6_fname, sizeof(xpm_ptr->tmp_ppm6_fname),
         ppm_fname);
   xpm_ptr->real_type = PPM_TRUE;

   return obj_ptr;
}

static
struct ObjRec *MyReadPpmFile(ppm_fname, expected_image_w, expected_image_h)
   char *ppm_fname;
   int expected_image_w, expected_image_h;
{
   char tmp_fname[MAXPATHLENGTH+1];
   int rc=0, ncolors=0, chars_per_pixel=0, *pixels=NULL;
   int first_pixel_is_bg=FALSE, image_w=0, image_h=0, w=0, h=0;
   Pixmap pixmap=None, bitmap=None;
   XImage *image=NULL, *bitmap_image=NULL;
   char *color_char, **color_str=NULL, *xpm_data=NULL;
   struct ObjRec *obj_ptr=NULL;

   SetWatchCursor(drawWindow);
   SetWatchCursor(mainWindow);
   SaveStatusStrings();
   rc = ConvertPpmToXpm(ppm_fname, tmp_fname, sizeof(tmp_fname));
   RestoreStatusStrings();
   SetDefaultCursor(mainWindow);
   ShowCursor();
   if (!rc) {
      return NULL;
   }
   SetWatchCursor(drawWindow);
   SetWatchCursor(mainWindow);
   rc = MyReadPixmapFile(tmp_fname,
         &image_w, &image_h, &w, &h, &pixmap, &image, &bitmap, &bitmap_image,
         &ncolors, &chars_per_pixel, &first_pixel_is_bg, &color_char,
         &color_str, &pixels, &xpm_data);
   SetDefaultCursor(mainWindow);
   ShowCursor();

   if (rc != BitmapSuccess) {
      sprintf(gszMsgBox, TgLoadString(STID_CANNOT_IMPORT_XPM_FILE), tmp_fname);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return NULL;
   }
   if (expected_image_w != (-1) && expected_image_h != (-1) &&
         (image_w != expected_image_w || image_h != expected_image_h)) {
      sprintf(gszMsgBox, TgLoadString(STID_PPM6_DIM_NOT_MATCH), image_w,
            image_h, ppm_fname, expected_image_w, expected_image_h);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return NULL;
   }
   obj_ptr = CreateXPmObj(image_w, image_h, w, h, pixmap, image, bitmap,
         bitmap_image, ncolors, chars_per_pixel, first_pixel_is_bg, color_char,
         color_str, pixels, xpm_data);
   
   unlink(tmp_fname);
   
   return obj_ptr;
}

int SetupTrueColorInfo(ptci)
   TrueColorInfo *ptci;
{
   unsigned int r_mask=0, g_mask=0, b_mask=0;
   unsigned int r_maxval=0, g_maxval=0, b_maxval=0;
   unsigned int r_bits=0, g_bits=0, b_bits=0;
   int r_shift=(-1), g_shift=(-1), b_shift=(-1);
   unsigned int shifts=0;
   
   ptci->r_mask = r_mask = mainVisual->red_mask;
   ptci->g_mask = g_mask = mainVisual->green_mask;
   ptci->b_mask = b_mask = mainVisual->blue_mask;
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
#ifdef _TGIF_DBG /* debug, do not translate */
      TgAssert(FALSE, "unexpected error in SetupTrueColorInfo()", NULL);
#endif /* _TGIF_DBG */
      return FALSE;
   }
   return TRUE;
}

static
struct ObjRec *MyReadPpmTrueFile2(ppm_fname, expected_image_w, expected_image_h)
   char *ppm_fname;
   int expected_image_w, expected_image_h;
{
   FILE *ppm_fp=NULL;
   int row=0, format=0, image_w=0, image_h=0, max_val=0;
   struct ObjRec *obj_ptr=NULL;
   struct XPmRec *xpm_ptr=NULL;
   Pixmap pixmap=None, bitmap=None;
   XImage *image=NULL, *bitmap_image=NULL;
   TrueColorInfo tci;
   double dmaxval=(double)0;
   int can_have_trans_pixel=FALSE, has_trans_pixel=FALSE;
   unsigned int trans_pixel_r=0, trans_pixel_g=0, trans_pixel_b=0;

   memset(&tci, 0, sizeof(TrueColorInfo));
   if (!SetupTrueColorInfo(&tci)) return NULL;

   ppm_fp = fopen(ppm_fname, "r");
   if (ppm_fp == NULL) {
      FailToOpenMessage(ppm_fname, "r", NULL);
      return NULL;
   }
   if (!ReadPpmHeader(ppm_fp, ppm_fname, &format, &image_w, &image_h,
         &max_val)) {
      fclose(ppm_fp);
      return NULL;
   }
   if (max_val != 255 && max_val != 65535) {
      sprintf(gszMsgBox, TgLoadString(STID_UNSUP_PPM6_MAX_VAL), max_val,
            ppm_fname);
      if (PRTGIF) {
         fprintf(stderr, "%s\n", gszMsgBox);
      } else {
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      }
      fclose(ppm_fp);
      return NULL;
   }
   if (expected_image_w != (-1) && expected_image_h != (-1) &&
         (image_w != expected_image_w || image_h != expected_image_h)) {
      sprintf(gszMsgBox, TgLoadString(STID_PPM6_DIM_NOT_MATCH), image_w,
            image_h, ppm_fname, expected_image_w, expected_image_h);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      fclose(ppm_fp);
      return NULL;
   }
   dmaxval = (double)255;

   pixmap = XCreatePixmap(mainDisplay, mainWindow, image_w, image_h, mainDepth);
   bitmap = XCreatePixmap(mainDisplay, dummyBitmap, image_w, image_h, 1);
   if (pixmap == None || bitmap == None) {
      if (pixmap != None) XFreePixmap(mainDisplay, pixmap);
      if (bitmap != None) XFreePixmap(mainDisplay, bitmap);
      sprintf(gszMsgBox, TgLoadString(STID_CANNOT_ALLOC_PIXMAP_OF_SIZE),
            image_w, image_h);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      fclose(ppm_fp);
      return NULL;
   }
   XFillRectangle(mainDisplay, pixmap, xpmGC, 0, 0, image_w, image_h);

   XSetForeground(mainDisplay, xbmGC, 1);
   XFillRectangle(mainDisplay, bitmap, xbmGC, 0, 0, image_w, image_h);
   XSetForeground(mainDisplay, xbmGC, 0);
   bitmap_image =  XGetImage(mainDisplay, bitmap, 0, 0, image_w, image_h, 1,
            ZPixmap);
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
   if (gPngHeaderInfo.valid && gPngHeaderInfo.trans_color_pixel_found) {
      can_have_trans_pixel = TRUE;
      trans_pixel_r = (unsigned int)gPngHeaderInfo.trans_color_pixel_red;
      trans_pixel_g = (unsigned int)gPngHeaderInfo.trans_color_pixel_green;
      trans_pixel_b = (unsigned int)gPngHeaderInfo.trans_color_pixel_blue;
      if (max_val == 65535) {
         trans_pixel_r = trans_pixel_r | ((trans_pixel_r<<8)&0x0ff00);
         trans_pixel_g = trans_pixel_g | ((trans_pixel_g<<8)&0x0ff00);
         trans_pixel_b = trans_pixel_b | ((trans_pixel_b<<8)&0x0ff00);
      }
   }
   for (row=0; row < image_h; row++) {
      int col=0;

      for (col=0; col < image_w; col++) {
         unsigned char buf[3];
         unsigned int r=0, g=0, b=0;
         double dr=(double)0, dg=(double)0, db=(double)0;
         int bytes_read=0;
         uint32_t pixel=0;

         if (format == TYPE_PPM6) {
            if (max_val == 65535) {
               unsigned char r_buf[2], g_buf[2], b_buf[2];

               if (fread(r_buf, sizeof(char), 2, ppm_fp) != 2 ||
                     fread(g_buf, sizeof(char), 2, ppm_fp) != 2 ||
                     fread(b_buf, sizeof(char), 2, ppm_fp) != 2) {
                  XFreePixmap(mainDisplay, pixmap);
                  XFreePixmap(mainDisplay, bitmap);
                  sprintf(gszMsgBox,
                        TgLoadString(STID_CANNOT_ALLOC_PIXMAP_OF_SIZE),
                        image_w, image_h);
                  MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
                  fclose(ppm_fp);
                  return NULL;
               }
               if (littleEndianPpm6) {
                  buf[0] = r_buf[1];
                  buf[1] = g_buf[1];
                  buf[2] = b_buf[1];
               } else {
                  buf[0] = r_buf[0];
                  buf[1] = g_buf[0];
                  buf[2] = b_buf[0];
               }
            } else {
               if ((bytes_read=fread(buf, sizeof(char), 3, ppm_fp)) != 3) {
                  XFreePixmap(mainDisplay, pixmap);
                  XFreePixmap(mainDisplay, bitmap);
                  sprintf(gszMsgBox,
                        TgLoadString(STID_CANNOT_ALLOC_PIXMAP_OF_SIZE),
                        image_w, image_h);
                  MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
                  fclose(ppm_fp);
                  return NULL;
               }
            }
         } else if (format == TYPE_PPM5) {
            if (max_val == 65535) {
               unsigned char g_buf[2];

               if (fread(g_buf, sizeof(char), 2, ppm_fp) != 2) {
                  XFreePixmap(mainDisplay, pixmap);
                  XFreePixmap(mainDisplay, bitmap);
                  sprintf(gszMsgBox,
                        TgLoadString(STID_CANNOT_ALLOC_PIXMAP_OF_SIZE),
                        image_w, image_h);
                  MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
                  fclose(ppm_fp);
                  return NULL;
               }
               if (littleEndianPpm6) {
                  buf[0] = buf[1] = buf[2] = g_buf[1];
               } else {
                  buf[0] = buf[1] = buf[2] = g_buf[0];
               }
            } else {
               if ((bytes_read=fread(buf, sizeof(char), 1, ppm_fp)) != 1) {
                  XFreePixmap(mainDisplay, pixmap);
                  XFreePixmap(mainDisplay, bitmap);
                  sprintf(gszMsgBox,
                        TgLoadString(STID_CANNOT_ALLOC_PIXMAP_OF_SIZE),
                        image_w, image_h);
                  MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
                  fclose(ppm_fp);
                  return NULL;
               }
               buf[1] = buf[2] = buf[0];
            }
         } else {
            int red=0, green=0, blue=0;

            if (fscanf(ppm_fp, "%d %d %d", &red, &green, &blue) != 3) {
               XFreePixmap(mainDisplay, pixmap);
               XFreePixmap(mainDisplay, bitmap);
               sprintf(gszMsgBox,
                     TgLoadString(STID_CANNOT_ALLOC_PIXMAP_OF_SIZE),
                     image_w, image_h);
               MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
               fclose(ppm_fp);
               return NULL;
            }
            buf[0] = (unsigned char)(red & 0x0ff);
            buf[1] = (unsigned char)(green & 0x0ff);
            buf[2] = (unsigned char)(blue & 0x0ff);
         }
         r = (unsigned int)buf[0];
         g = (unsigned int)buf[1];
         b = (unsigned int)buf[2];
         dr = ((double)r) / dmaxval * tci.dr_maxval;
         dg = ((double)g) / dmaxval * tci.dg_maxval;
         db = ((double)b) / dmaxval * tci.db_maxval;
         r = round(dr);
         g = round(dg);
         b = round(db);
         pixel = ((r << tci.r_shift) & mainVisual->red_mask) |
                 ((g << tci.g_shift) & mainVisual->green_mask) |
                 ((b << tci.b_shift) & mainVisual->blue_mask) ;
         XPutPixel(image, col, row, pixel);
         if (can_have_trans_pixel && trans_pixel_r == r &&
               trans_pixel_g == g && trans_pixel_b == b) {
            XPutPixel(bitmap_image, col, row, 0);
            has_trans_pixel = TRUE;
         }
      }
   }
   fclose(ppm_fp);

   XPutImage(mainDisplay, pixmap, xpmGC, image, 0, 0, 0, 0, image_w, image_h);
   if (has_trans_pixel) {
      XPutImage(mainDisplay, bitmap, xbmGC, bitmap_image, 0, 0, 0, 0,
            image_w, image_h);
   } else {
      /*
       * It seems to be a waste not to do the following.  But it's done
       *       this way so that other code (such as in "dup.c") do not
       *       have to be changed.
       *
       * XDestroyImage(bitmap_image);
       * XFreePixmap(mainDisplay, bitmap);
       * bitmap_image = NULL;
       * bitmap = None;
       */
   }
   obj_ptr = CreateXPmObj(image_w, image_h, image_w, image_h, pixmap, image,
         bitmap, bitmap_image, 0, 0, FALSE, NULL, NULL, NULL, NULL);
   xpm_ptr = obj_ptr->detail.xpm;
   xpm_ptr->real_type = PPM_TRUE;
   if (obj_ptr == NULL) return NULL;

   return obj_ptr;
}

static
struct ObjRec *MyReadPpmTrueFile(ppm_fname, expected_image_w, expected_image_h)
   char *ppm_fname;
   int expected_image_w, expected_image_h;
{
   struct ObjRec *obj_ptr=NULL;
   struct XPmRec *xpm_ptr=NULL;

   if (PRTGIF && !cmdLineOpenDisplay) {
      obj_ptr = MyReadPpmTrueFileSkipData(ppm_fname, expected_image_w,
            expected_image_h);
   } else {
      if (fullTrueColorMode && HasZlibSupport()) {
         obj_ptr = MyReadPpmTrueFile2(ppm_fname, expected_image_w,
               expected_image_h);
         if (obj_ptr != NULL) {
            MoveObj(obj_ptr, -drawOrigX, -drawOrigY);

            return obj_ptr;
         } else {
            return NULL;
         }
      }
      obj_ptr = MyReadPpmFile(ppm_fname, expected_image_w, expected_image_h);
      if (obj_ptr == NULL) {
         return NULL;
      }
      xpm_ptr = obj_ptr->detail.xpm;
      xpm_ptr->real_type = XPM_XPM;
   }
   MoveObj(obj_ptr, -drawOrigX, -drawOrigY);

   return obj_ptr;
}

struct ObjRec *CreatePpmTrueObjFromFile(ppm_fname)
   char *ppm_fname;
{
   return MyReadPpmTrueFile(ppm_fname, (-1), (-1));
}

void ReadPpmTrueObj(FP, Inbuf, ObjPtr)
   FILE *FP;
   char *Inbuf;
   struct ObjRec **ObjPtr;
{
   struct XPmRec *xpm_ptr=NULL;
   char *s=NULL, inbuf[MAXSTRING], *ppm_data=NULL, *ppm_mask_data=NULL;
   char color_s[40], trans_color_s[40], tmp_fname[MAXPATHLENGTH];
   int ltx=0, lty=0, rbx=0, rby=0, image_w=0, image_h=0, color_index=(-1);
   int rotation=0, new_alloc=FALSE, id=0, rotate=ROTATE0, flip=NO_FLIP;
   int locked=FALSE, has_transp_color=FALSE, real_x=0, real_y=0;
   int transformed=FALSE, invisible=FALSE;
   int ppm_data_compress=FALSE, ppm_data_size=0, ppm_mask_size=0;
   struct XfrmMtrxRec *ctm=NULL;
   struct BBRec orig_obbox;
   struct ObjRec *new_obj_ptr=NULL;
   unsigned char trans_color_pixel_r='\0', trans_color_pixel_g='\0', trans_color_pixel_b='\0';

   *ObjPtr = NULL;

   s = FindChar((int)'(', Inbuf);
   s = ParseStr(s, (int)',', color_s, sizeof(color_s));
   s = ParseStr(s, (int)',', trans_color_s, sizeof(trans_color_s));
   InitScan(s, "\t\n, ");

   rotation = 0;
   if (fileVersion < 37) {
      sprintf(gszMsgBox,
            TgLoadCachedString(CSTID_INVALID_FILEVER_FOR_LINK_JPEG),
            fileVersion);
      if (PRTGIF) {
         fprintf(stderr, "%s\n", gszMsgBox);
      } else {
         Msg(gszMsgBox);
      }
      return;
   } else {
      if (GETINT("ppm_true", has_transp_color, "has_transparent_color") == INVALID ||
          GETINT("ppm_true", ltx,              "ltx") == INVALID ||
          GETINT("ppm_true", lty,              "lty") == INVALID ||
          GETINT("ppm_true", rbx,              "rbx") == INVALID ||
          GETINT("ppm_true", rby,              "rby") == INVALID ||
          GETINT("ppm_true", id,               "id") == INVALID ||
          GETINT("ppm_true", rotation,         "rotation") == INVALID ||
          GETINT("ppm_true", image_w,          "image_w") == INVALID ||
          GETINT("ppm_true", image_h,          "image_h") == INVALID ||
          GETINT("ppm_true", rotate,           "rotate") == INVALID ||
          GETINT("ppm_true", flip,             "flip") == INVALID ||
          GETINT("ppm_true", locked,           "locked") == INVALID ||
          GETINT("ppm_true", transformed,      "transformed") == INVALID ||
          GETINT("ppm_true", invisible,        "invisible") == INVALID ||
          GETINT("ppm_true", ppm_data_compress,"ppm_data_compress") == INVALID ||
          GETINT("ppm_true", ppm_data_size,    "ppm_data_size") == INVALID ||
          GETINT("ppm_true", ppm_mask_size,    "ppm_mask_size") == INVALID) {
         return;
      }
      if (id >= objId) objId = id+1;
   }
   if (ppm_data_compress == PPM_JPEG_COMPRESS) {
      /* this is fine */
   } else if (ppm_data_compress == PPM_DATA_DEFLATED) {
      if (!HasZlibSupport()) {
         sprintf(gszMsgBox, TgLoadString(STID_CANNOT_INFLATE_GIVEN_OBJ),
               "ppm_true", TOOL_NAME);
         if (PRTGIF) {
            fprintf(stderr, "%s\n", gszMsgBox);
         } else {
            Msg(gszMsgBox);
         }
         return;
      }
   } else {
      /* including PPM_DATA_RAW -- not supported, must be an error */
      sprintf(gszMsgBox, TgLoadString(STID_INVALID_GIVEN_PARAM_GIVEN_OBJ),
            "ppm_data_compress", "ppm_true");
      if (PRTGIF) {
         fprintf(stderr, "%s\n", gszMsgBox);
      } else {
         Msg(gszMsgBox);
      }
      return;
   }
   if (transformed) {
      (void)fgets(inbuf, MAXSTRING, FP);
      scanLineNum++;
      InitScan(inbuf, "\t\n, ");

      ctm = (struct XfrmMtrxRec *)malloc(sizeof(struct XfrmMtrxRec));
      if (ctm == NULL) FailAllocMessage();
      if (GETINT("xpixmap", real_x,           "real_x") == INVALID ||
          GETINT("xpixmap", real_y,           "real_y") == INVALID ||
          GETINT("xpixmap", orig_obbox.ltx,   "orig_obbox.ltx") == INVALID ||
          GETINT("xpixmap", orig_obbox.lty,   "orig_obbox.lty") == INVALID ||
          GETINT("xpixmap", orig_obbox.rbx,   "orig_obbox.rbx") == INVALID ||
          GETINT("xpixmap", orig_obbox.rby,   "orig_obbox.rby") == INVALID ||
          GETDBL("xpixmap", ctm->m[CTM_SX],   "CTM_SX") == INVALID ||
          GETDBL("xpixmap", ctm->m[CTM_SIN],  "CTM_SIN") == INVALID ||
          GETDBL("xpixmap", ctm->m[CTM_MSIN], "CTM_MSIN") == INVALID ||
          GETDBL("xpixmap", ctm->m[CTM_SY],   "CTM_SY") == INVALID ||
          GETINT("xpixmap", ctm->t[CTM_TX],   "CTM_TX") == INVALID ||
          GETINT("xpixmap", ctm->t[CTM_TY],   "CTM_TY") == INVALID) {
         return;
      }
   }
   ppm_data = ReadPpmTrueData(FP, ppm_data_size);
   ppm_mask_data = ReadPpmTrueMask(FP, ppm_mask_size);
   if (ppm_data == NULL) return;

   *tmp_fname = '\0';
   if (IsPpmTrueObj(PPM_TRUE, ppm_data_compress, ppm_data)) {
      if (!ConvertPpmTrueToPpm6(ppm_data, ppm_data_size, ppm_data_compress,
            tmp_fname, sizeof(tmp_fname))) {
         UtilFree(ppm_data);
         return;
      }
   } else {
#ifdef _TGIF_DBG /* debug, do not translate */
      TgAssert(FALSE, "unsupported format in ReadPpmTrueObj()", NULL);
#endif /* _TGIF_DBG */
   }
   if (has_transp_color) {
      if (!ParseTransColor(trans_color_s, &trans_color_pixel_r,
            &trans_color_pixel_g, &trans_color_pixel_b)) {
         strcpy(gszMsgBox, TgLoadString(STID_BAD_TRANS_COLOR_NO_TRANS));
         if (PRTGIF && !cmdLineOpenDisplay) {
            fprintf(stderr, "%s\n", gszMsgBox);
         } else {
            Msg(gszMsgBox);
         }
         has_transp_color = FALSE;
      }
   }
   ResetPngHeaderInfo(&gPngHeaderInfo);
   if (has_transp_color) {
      SetPngHeaderInfoForTransColor(&gPngHeaderInfo, trans_color_pixel_r,
            trans_color_pixel_g, trans_color_pixel_b);
   }
   new_obj_ptr = MyReadPpmTrueFile(tmp_fname, image_w, image_h);
   ResetPngHeaderInfo(&gPngHeaderInfo);
   if (new_obj_ptr == NULL) {
      unlink(tmp_fname);
      UtilFree(ppm_data);
      return;
   }
   unlink(tmp_fname);
   /* must set real_type set in MyReadPpmTrueFile() */

   color_index = QuickFindColorIndex(new_obj_ptr, color_s, &new_alloc, TRUE);
   new_obj_ptr->color = color_index;
   UtilStrCpyN(new_obj_ptr->color_str, sizeof(new_obj_ptr->color_str), color_s);
   new_obj_ptr->id = id;
   MoveObj(new_obj_ptr, ltx, lty);
   if (ctm != NULL) {
      new_obj_ptr->obbox.ltx = new_obj_ptr->bbox.ltx = new_obj_ptr->x = ltx;
      new_obj_ptr->obbox.lty = new_obj_ptr->bbox.lty = new_obj_ptr->y = lty;
      new_obj_ptr->obbox.rbx = new_obj_ptr->bbox.rbx = rbx;
      new_obj_ptr->obbox.rby = new_obj_ptr->bbox.rby = rby;
      new_obj_ptr->ctm = ctm;

      memcpy(&new_obj_ptr->orig_obbox, &orig_obbox, sizeof(struct BBRec));
      new_obj_ptr->x = real_x;
      new_obj_ptr->y = real_y;
      GetTransformedOBBoxOffsetVs(new_obj_ptr, new_obj_ptr->rotated_obbox);
   }
   xpm_ptr = new_obj_ptr->detail.xpm;

   if (xpm_ptr->real_type == PPM_TRUE) {
      xpm_ptr->ppm_data_compress = ppm_data_compress;
      xpm_ptr->ppm_data = ppm_data;
      xpm_ptr->ppm_mask_data = NULL;
      xpm_ptr->ppm_mask_size = 0;
      xpm_ptr->ppm_data_size = ppm_data_size;
      xpm_ptr->has_transparent_color = has_transp_color;
      if (xpm_ptr->has_transparent_color) {
         xpm_ptr->transparent_color[0] = trans_color_pixel_r;
         xpm_ptr->transparent_color[1] = trans_color_pixel_g;
         xpm_ptr->transparent_color[2] = trans_color_pixel_b;
      }
   } else {
      /* something didn't work, revert back to a regular XPM object */
      if (!PRTGIF || cmdLineOpenDisplay) {
         UtilStrCpyN(gszMsgBox, sizeof(gszMsgBox),
               TgLoadString(STID_PPM_TRUE_LOSSY_CONV_TO_XPM));
         Msg(gszMsgBox);
         SetFileModified(TRUE);
      }
      UtilFree(ppm_data);
   }
   *ObjPtr = new_obj_ptr;
}

void SetXPmPropMask(ObjPtr, plMask, plSkip, pProp)
   struct ObjRec *ObjPtr;
   long *plMask, *plSkip;
   struct PropertiesRec *pProp;
{
   SetCTMPropertyMask(ObjPtr->ctm, plMask, plSkip, pProp);
}

void FreeXPmObj(ObjPtr)
   struct ObjRec *ObjPtr;
{
   register int i, ncolors;
   struct XPmRec *xpm_ptr=ObjPtr->detail.xpm;

   if (xpm_ptr->pixmap != None) {
      if (xpm_ptr->pixels != NULL) free(xpm_ptr->pixels);
      if (xpm_ptr->red != NULL) free(xpm_ptr->red);
      if (xpm_ptr->green != NULL) free(xpm_ptr->green);
      if (xpm_ptr->blue != NULL) free(xpm_ptr->blue);
      if (xpm_ptr->color_char != NULL) free(xpm_ptr->color_char);
      if (xpm_ptr->color_str != NULL) {
         ncolors = xpm_ptr->ncolors;
         for (i = 0; i < ncolors; i++) free(xpm_ptr->color_str[i]);
         free(xpm_ptr->color_str);
      }
      XFreePixmap(mainDisplay, xpm_ptr->pixmap);
   }
   if (xpm_ptr->bitmap != None) XFreePixmap(mainDisplay, xpm_ptr->bitmap);
   if (xpm_ptr->image != NULL) XDestroyImage(xpm_ptr->image);
   if (xpm_ptr->bitmap_image != NULL) XDestroyImage(xpm_ptr->bitmap_image);
   if (xpm_ptr->cached_pixmap != None) {
      XFreePixmap(mainDisplay, xpm_ptr->cached_pixmap);
   }
   if (xpm_ptr->cached_bitmap != None) {
      XFreePixmap(mainDisplay, xpm_ptr->cached_bitmap);
   }
   if (xpm_ptr->clip_mask != None) {
      XFreePixmap(mainDisplay, xpm_ptr->clip_mask);
   }
   if (xpm_ptr->data != NULL) free(xpm_ptr->data);
   if (xpm_ptr->filename != NULL) free(xpm_ptr->filename);
   if (xpm_ptr->ppm_data != NULL) free(xpm_ptr->ppm_data);
   if (xpm_ptr->ppm_mask_data != NULL) free(xpm_ptr->ppm_mask_data);
   free(xpm_ptr);
   free(ObjPtr);
}
