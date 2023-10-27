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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/choice.c,v 1.15 2011/05/16 16:21:56 william Exp $
 */

#define _INCLUDE_FROM_CHOICE_C_

#include "tgifdefs.h"
#include "expfdefs.h"

#include "align.e"
#include "arc.e"
#include "auxtext.e"
#include "box.e"
#include "choice.e"
#include "color.e"
#include "cursor.e"
#include "dialog.e"
#include "drawing.e"
#include "edit.e"
#include "file.e"
#include "font.e"
#include "grid.e"
#include "inmethod.e"
#include "mainloop.e"
#include "mainmenu.e"
#include "mark.e"
#include "menu.e"
#include "menuinfo.e"
#include "miniline.e"
#include "msg.e"
#include "navigate.e"
#include "oval.e"
#include "page.e"
#include "pattern.e"
#include "poly.e"
#include "polygon.e"
#include "raster.e"
#include "rcbox.e"
#include "rect.e"
#include "select.e"
#include "setup.e"
#include "shape.e"
#include "special.e"
#include "stretch.e"
#include "strtbl.e"
#include "text.e"
#include "util.e"
#include "wb.e"
#include "xbitmap.e"
#include "xprtfltr.e"

#define FILE_ROW 0
#define ZOOM_ROW 0
#define RADIUS_ROW 0
#define PAGE_ROW 0
#define HORI_ALIGN_ROW 0
#define FONT_ROW 0
#define VSPACE_ROW 0
#define SHAPE_ROW 0
#define DASH_ROW 0
#define LINE_TYPE_ROW 0
#define FILL_ROW 0
#define COLOR_ROW 0

#define PRINT_ROW 1
#define MOVE_MODE_ROW 1
#define ROTATE_ROW 1
#define PAGELAYOUT_ROW 1
#define VERT_ALIGN_ROW 1
#define TEXT_SIZE_ROW 1
#define JUST_ROW 1
#define STRETCHTEXT_ROW 1
#define LINE_STYLE_ROW 1
#define LINE_WIDTH_ROW 1
#define PEN_ROW 1
#define TRANSPAT_ROW 1

#define FILE_COL 0
#define ZOOM_COL 1
#define RADIUS_COL 2
#define PAGE_COL 3
#define HORI_ALIGN_COL 4
#define FONT_COL 5
#define VSPACE_COL 6
#define SHAPE_COL 7
#define DASH_COL 8
#define LINE_TYPE_COL 9
#define FILL_COL 10
#define COLOR_COL 11

#define PRINT_COL 0
#define MOVE_MODE_COL 1
#define ROTATE_COL 2
#define PAGELAYOUT_COL 3
#define VERT_ALIGN_COL 4
#define TEXT_SIZE_COL 5
#define JUST_COL 6
#define STRETCHTEXT_COL 7
#define LINE_STYLE_COL 8
#define LINE_WIDTH_COL 9
#define PEN_COL 10
#define TRANSPAT_COL 11

#define CHOICE_SHIFT 4

#define CHOICE_COLOR ((COLOR_COL<<CHOICE_SHIFT)|COLOR_ROW)
#define CHOICE_FILE ((FILE_COL<<CHOICE_SHIFT)|FILE_ROW)
#define CHOICE_PRINT ((PRINT_COL<<CHOICE_SHIFT)|PRINT_ROW)
#define CHOICE_HORI_ALIGN ((HORI_ALIGN_COL<<CHOICE_SHIFT)|HORI_ALIGN_ROW)
#define CHOICE_VERT_ALIGN ((VERT_ALIGN_COL<<CHOICE_SHIFT)|VERT_ALIGN_ROW)
#define CHOICE_ROTATE ((ROTATE_COL<<CHOICE_SHIFT)|ROTATE_ROW)
#define CHOICE_MOVE_MODE ((MOVE_MODE_COL<<CHOICE_SHIFT)|MOVE_MODE_ROW)
#define CHOICE_RADIUS ((RADIUS_COL<<CHOICE_SHIFT)|RADIUS_ROW)
#define CHOICE_ZOOM ((ZOOM_COL<<CHOICE_SHIFT)|ZOOM_ROW)
#define CHOICE_FONT ((FONT_COL<<CHOICE_SHIFT)|FONT_ROW)
#define CHOICE_JUST ((JUST_COL<<CHOICE_SHIFT)|JUST_ROW)
#define CHOICE_TEXT_SIZE ((TEXT_SIZE_COL<<CHOICE_SHIFT)|TEXT_SIZE_ROW)
#define CHOICE_VSPACE ((VSPACE_COL<<CHOICE_SHIFT)|VSPACE_ROW)
#define CHOICE_SHAPE ((SHAPE_COL<<CHOICE_SHIFT)|SHAPE_ROW)
#define CHOICE_STRETCHTEXT ((STRETCHTEXT_COL<<CHOICE_SHIFT)|STRETCHTEXT_ROW)
#define CHOICE_LINE_WIDTH ((LINE_WIDTH_COL<<CHOICE_SHIFT)|LINE_WIDTH_ROW)
#define CHOICE_LINE_STYLE ((LINE_STYLE_COL<<CHOICE_SHIFT)|LINE_STYLE_ROW)
#define CHOICE_LINE_TYPE ((LINE_TYPE_COL<<CHOICE_SHIFT)|LINE_TYPE_ROW)
#define CHOICE_DASH ((DASH_COL<<CHOICE_SHIFT)|DASH_ROW)
#define CHOICE_FILL ((FILL_COL<<CHOICE_SHIFT)|FILL_ROW)
#define CHOICE_PEN ((PEN_COL<<CHOICE_SHIFT)|PEN_ROW)
#define CHOICE_TRANSPAT ((TRANSPAT_COL<<CHOICE_SHIFT)|TRANSPAT_ROW)
#define CHOICE_PAGE ((PAGE_COL<<CHOICE_SHIFT)|PAGE_ROW)
#define CHOICE_PAGELAYOUT ((PAGELAYOUT_COL<<CHOICE_SHIFT)|PAGELAYOUT_ROW)

int	curChoice=NOTHING;
int	cycleThroughChoice=FALSE;

int	setCurChoiceDontUpdateUI=FALSE;

static int	curRaisedChoice=INVALID;
static int	curRaisedMode=INVALID;

static GC	choiceGC=(GC)0;

static Pixmap	abcBitmap=None, rotatedAbcBitmap=None;
static XImage	*abcImage=NULL;
static int	abcRotation=0, abcRotatedBitmapSize=0;
static struct BBRec rotatedAbcBBox;

#include "xbm/abc.xbm"

void InitChoice()
{
   XGCValues values;

   values.foreground = xorOne;
   values.background = xorZero;
   values.fill_style = FillSolid;
   values.font = defaultFontPtr->fid;
   choiceGC = XCreateGC(mainDisplay, choiceWindow,
         GCForeground | GCBackground | GCFillStyle | GCFont, &values);

   memset(&rotatedAbcBBox, 0, sizeof(struct BBRec));
   abcBitmap = XCreateBitmapFromData(mainDisplay, choiceWindow,
         (char *)abc_bits, abc_width, abc_height);
   if (abcBitmap == None) FailAllocPixmapMessage(abc_width, abc_height);

   abcImage = XGetImage(mainDisplay, abcBitmap, 0, 0, abc_width, abc_height, 1,
         ZPixmap);
   if (abcImage == NULL) FailAllocMessage();
}

void CleanUpChoices()
{
   XFreeGC(mainDisplay, choiceGC);
   choiceGC = (GC)0;

   XFreePixmap(mainDisplay, abcBitmap);
   XDestroyImage(abcImage);
   abcImage = NULL;
   if (rotatedAbcBitmap != None) XFreePixmap(mainDisplay, rotatedAbcBitmap);
   abcBitmap = rotatedAbcBitmap = None;
}

static
void ShowStipple(dpy, win, gc, bitmap, col, row, w, h)
   Display *dpy;
   Window win;
   GC gc;
   Pixmap bitmap;
   int col, row, w, h;
{
   if (threeDLook) {
      int x=1+windowPadding+col*(choiceImageW+windowPadding);
      int y=1+windowPadding+row*(choiceImageH+windowPadding);
      XGCValues values;

      values.stipple = bitmap;
      values.ts_x_origin = x;
      values.ts_y_origin = y;
      XChangeGC(dpy, gc,
            GCStipple | GCTileStipXOrigin | GCTileStipYOrigin, &values);
      XFillRectangle(dpy, win, gc, x, y, w, h);
      XSetTSOrigin(dpy, gc, 0, 0);
   } else {
      int x=col*choiceImageW, y=row*choiceImageH;

      XSetStipple(dpy, gc, bitmap);
      XFillRectangle(dpy, win, gc, x, y, w, h);
   }
}

static
void HighLightThreeDButton(choice, highlight)
   int choice, highlight;
{
   struct BBRec bbox;
   int col=0, row=0;

   if (!threeDLook) return;

   col = (choice>>CHOICE_SHIFT);
   row = choice-(col<<CHOICE_SHIFT);
   bbox.ltx = 1+col*(choiceImageW+windowPadding);
   bbox.lty = 1+row*(choiceImageH+windowPadding);
   bbox.rbx = bbox.ltx+choiceImageW+(windowPadding<<1);
   bbox.rby = bbox.lty+choiceImageH+(windowPadding<<1);
   if (highlight) {
      TgDrawThreeDButton(mainDisplay, choiceWindow, textMenuGC, &bbox,
            TGBS_RAISED, 2, FALSE);
   } else {
      TgClearThreeDButton(mainDisplay, choiceWindow, textMenuGC, &bbox, 2);
   }
}

static
void UpdateAbcBitmap()
{
   XImage *image;
   int r, ltx, lty, rbx, rby, half_sz;
   struct XfrmMtrxRec ctm;
   double radian, sin_val, cos_val;

   if (textRotation == 0) return;
   if (textRotation == abcRotation && rotatedAbcBitmap != None) return;
   if (rotatedAbcBitmap == None) {
      double dval=(double)sqrt(
            (double)(abc_width*abc_width+abc_height*abc_height));

      abcRotatedBitmapSize = ((round(dval)+2)<<1);
      if ((rotatedAbcBitmap=XCreatePixmap(mainDisplay, dummyBitmap,
            abcRotatedBitmapSize, abcRotatedBitmapSize, 1)) == None) {
         FailAllocPixmapMessage(abcRotatedBitmapSize, abcRotatedBitmapSize);
         return;
      }
   }
   XFillRectangle(mainDisplay, rotatedAbcBitmap, xbmGC, 0, 0,
         abcRotatedBitmapSize, abcRotatedBitmapSize);
   image = XGetImage(mainDisplay, rotatedAbcBitmap, 0, 0, abcRotatedBitmapSize,
         abcRotatedBitmapSize, 1, ZPixmap);
   if (image == NULL) {
      FailAllocMessage();
      return;
   }
   radian = (((double)textRotation)*M_PI/180.0/64.0);
   sin_val = sin(radian);
   cos_val = cos(radian);

   memset(&ctm, 0, sizeof(struct XfrmMtrxRec));
   ctm.m[CTM_SX] = ctm.m[CTM_SY] = ((double)1000)*cos_val;
   ctm.m[CTM_SIN] = ((double)1000)*sin_val;
   ctm.m[CTM_MSIN] = (-ctm.m[CTM_SIN]);
   ctm.t[CTM_TX] = ctm.t[CTM_TY] = 0;
   ltx = lty = abcRotatedBitmapSize+1;
   rbx = rby = (-1);
   half_sz = (abcRotatedBitmapSize>>1);
   for (r=0; r < abcRotatedBitmapSize; r++) {
      int c, y=r-half_sz;
      double dy=((double)y)+0.5;

      for (c=0; c < abcRotatedBitmapSize; c++) {
         int x=c-half_sz;
         double dx=((double)x)+0.5;
         double new_dx=(double)0, new_dy=(double)0;

         ReverseTransformDoublePointThroughCTM(dx, dy, &ctm, &new_dx, &new_dy);
         if (new_dx >= ((double)0) && new_dx < ((double)abc_width) &&
               new_dy >= ((double)0) && new_dy < ((double)abc_height)) {
            int new_x=(int)new_dx, new_y=(int)new_dy;

            if (new_x < 0) new_x = 0;
            if (new_x >= abc_width) new_x = abc_width-1;
            if (new_y < 0) new_y = 0;
            if (new_y >= abc_height) new_y = abc_height-1;
            if (XGetPixel(abcImage, new_x, new_y) == 1) {
               XPutPixel(image, c, r, 1);
               if (c < ltx) ltx = c; if (r < lty) lty = r;
               if (c > rbx) rbx = c; if (r > rby) rby = r;
            }
         }
      }
   }
   XPutImage(mainDisplay, rotatedAbcBitmap, xbmGC, image, 0, 0, 0, 0,
         abcRotatedBitmapSize, abcRotatedBitmapSize);
   XDestroyImage(image);
   rotatedAbcBBox.ltx = ltx; rotatedAbcBBox.lty = lty;
   rotatedAbcBBox.rbx = rbx; rotatedAbcBBox.rby = rby;
}

void ShowWhereToPrint()
{
   if (colorDump) {
      XSetForeground(mainDisplay, rasterGC, colorPixels[colorIndex]);
   }
   ShowStipple(mainDisplay, choiceWindow, rasterGC,
         whereToPrintPixmap[whereToPrint], PRINT_COL,
         PRINT_ROW, choiceImageW, choiceImageH);
   if (colorDump) XSetForeground(mainDisplay, rasterGC, myFgPixel);
}

void ShowColor(PropagateWhereToPrint)
   int PropagateWhereToPrint;
{
   XGCValues values;

   if (colorDisplay) {
      int x=COLOR_COL*choiceImageW, y=COLOR_ROW*choiceImageH;

      if (threeDLook) {
         x = 1+windowPadding+COLOR_COL*(choiceImageW+windowPadding);
         y = 1+windowPadding+COLOR_ROW*(choiceImageH+windowPadding);
      }
      values.foreground = colorPixels[colorIndex];
      values.function = GXcopy;
      values.fill_style = FillOpaqueStippled;
      values.stipple = patPixmap[1];
      XChangeGC(mainDisplay, patGC,
            GCForeground | GCFunction | GCFillStyle | GCStipple, &values);

      XFillRectangle(mainDisplay, choiceWindow, patGC, x, y,
            choiceImageW, choiceImageH);
   }
   if (PropagateWhereToPrint) ShowWhereToPrint();
}

void ShowHoriAlign()
{
   ShowStipple(mainDisplay, choiceWindow, rasterGC, alignHoriPixmap[horiAlign],
         HORI_ALIGN_COL, HORI_ALIGN_ROW, choiceImageW, choiceImageH);
}

void ShowVertAlign()
{
   ShowStipple(mainDisplay, choiceWindow, rasterGC, alignVertPixmap[vertAlign],
         VERT_ALIGN_COL, VERT_ALIGN_ROW, choiceImageW, choiceImageH);
}

void ShowJust()
{
   ShowStipple(mainDisplay, choiceWindow, rasterGC, justPixmap[textJust],
         JUST_COL, JUST_ROW, choiceImageW, choiceImageH);
}

void ShowCurFont()
{
   static int snPrevShowDoubleByteFont=INVALID;

   char *choice_char=GetShowFontChar(curFont);
   int x, y, w, h, choice_len=(choice_char==NULL ? 0 : strlen(choice_char));
   XRectangle recs[1];
   XGCValues values;
   int image_x=FONT_COL*choiceImageW, image_y=FONT_ROW*choiceImageH;
   int saved_sz_unit=curSzUnit;

   if (threeDLook) {
      image_x = 1+windowPadding+FONT_COL*(choiceImageW+windowPadding);
      image_y = 1+windowPadding+FONT_ROW*(choiceImageH+windowPadding);
   }
   recs[0].x = image_x;
   recs[0].y = image_y;
   recs[0].width = choiceImageW;
   recs[0].height = choiceImageH;

   if (curChoice == DRAWTEXT && textCursorShown && editTextSize != 0) {
      curSzUnit = GetCurSzUnit();
      SetCanvasFont();
   }
   if (choice_char != NULL && canvasFontDoubleByte &&
         canvasFontDoubleByteModBytes) {
      choice_char[0] |= 0x80;
      choice_char[1] |= 0x80;
   }
   if (choice_char == NULL) {
      /* do not translate -- the string is used to measure things */
      w = XTextWidth(canvasFontPtr, "W", 1);
   } else {
      w = MyTextWidth(canvasFontPtr, choice_char, choice_len);
   }
   h = canvasFontHeight;

   XClearArea(mainDisplay, choiceWindow, image_x-windowPadding,
         image_y-windowPadding, choiceImageW+(windowPadding<<1),
         choiceImageH+(windowPadding<<1), FALSE);

   XSetFont(mainDisplay, choiceGC, canvasFontPtr->fid);
   XSetClipRectangles(mainDisplay, choiceGC, 0, 0, recs, 1, YXBanded);

   values.foreground = myFgPixel;
   values.background = myBgPixel;
   XChangeGC(mainDisplay, choiceGC, GCForeground | GCBackground, &values);

   x = (w > choiceImageW) ?
         image_x - (w-choiceImageW)/2 :
         image_x + (choiceImageW-w)/2;
   y = (h > choiceImageH) ?
         image_y + (canvasFontAsc-(h-choiceImageH)/2) :
         image_y + (canvasFontAsc+(choiceImageH-h)/2);
   if (canvasFontDoubleByte && canvasFontDoubleByteVertical) {
      Pixmap bitmap=None;
      TextExtentsInfo stTextExtents;

      memset(&stTextExtents, 0, sizeof(TextExtentsInfo));
      /* choice_char cannot be NULL */
      stTextExtents.buf = choice_char;
      stTextExtents.len = choice_len;
      bitmap = PaintVertChar(&stTextExtents, ROTATE270);
      if (bitmap == None) {
         return;
      }
      x = image_x + ((choiceImageW-stTextExtents.bbox_h)>>1);
      y = image_y + ((choiceImageH-stTextExtents.bbox_w)>>1);

      values.clip_mask = bitmap;
      values.clip_x_origin = x;
      values.clip_y_origin = y;
      XChangeGC(mainDisplay, choiceGC,
            GCClipMask | GCClipXOrigin | GCClipYOrigin, &values);

      XFillRectangle(mainDisplay, choiceWindow, choiceGC,
            (x>image_x ? x : image_x), (y>image_y ? y: image_y),
            (x>image_x ? stTextExtents.bbox_w : choiceImageW),
            (y>image_y ? stTextExtents.bbox_h : choiceImageH));

      values.clip_mask = None;
      values.clip_x_origin = 0;
      values.clip_y_origin = 0;
      XChangeGC(mainDisplay, choiceGC,
            GCClipMask | GCClipXOrigin | GCClipYOrigin, &values);

      y += stTextExtents.bbox_w;
      w = stTextExtents.bbox_h;
   } else {
      if (choice_char == NULL) {
         /* do not translate -- the string is used to measure things */
         XDrawString(mainDisplay, choiceWindow, choiceGC, x, y, "W", 1);
      } else {
         MyDrawString(mainDisplay, choiceWindow, choiceGC, mainDepth, x, y,
               choice_char, choice_len);
      }
   }
   if (curUnderlineOn) {
      XDrawLine(mainDisplay, choiceWindow, choiceGC, x, y+curUnderlineYOffset,
            x+w, y+curUnderlineYOffset);
   }
   if (curOverlineOn) {
      XDrawLine(mainDisplay, choiceWindow, choiceGC,
            x, y-canvasFontAsc-curOverlineYOffset,
            x+w, y-canvasFontAsc-curOverlineYOffset);
   }
   values.foreground = xorOne;
   values.background = xorZero;
   XChangeGC(mainDisplay, choiceGC, GCForeground | GCBackground, &values);

   recs[0].x = 0;
   recs[0].y = 0;
   recs[0].width = choiceWindowW;
   recs[0].height = choiceWindowH;
   XSetClipRectangles(mainDisplay, choiceGC, 0, 0, recs, 1, YXBanded);

   /*
    * The following code was added so that when you move the text cursor
    *         from a double-byte font to a single-byte font, we can close
    *         the double-byte input method.
    */
   if (gnInputMethod != TGIM_NONE) {
      if (snPrevShowDoubleByteFont != INVALID &&
            snPrevShowDoubleByteFont && !canvasFontDoubleByte &&
            tgIMDeactiveOnCreateText(mainDisplay, drawWindow)) {
         if (tgIMHandleCreateText(mainDisplay, drawWindow)) {
         }
      }
      snPrevShowDoubleByteFont = canvasFontDoubleByte;
   }
   if (curChoice == DRAWTEXT && textCursorShown && editTextSize != 0) {
      curSzUnit = saved_sz_unit;
      SetCanvasFont();
   }
}

void ShowTextVSpace()
{
   int len, x, y;
   char s[80];
   XGCValues values;
   int image_x=VSPACE_COL*choiceImageW, image_y=VSPACE_ROW*choiceImageH;

   if (threeDLook) {
      image_x = 1+windowPadding+VSPACE_COL*(choiceImageW+windowPadding);
      image_y = 1+windowPadding+VSPACE_ROW*(choiceImageH+windowPadding);
   }
   ShowStipple(mainDisplay, choiceWindow, rasterGC, vspacePixmap,
         VSPACE_COL, VSPACE_ROW, choiceImageW, choiceImageH);

   sprintf(s, "%1d", textVSpace);
   len = strlen(s);
   if (threeDLook) {
      x = image_x+(choiceImageW>>1)-1;
      y = image_y+((choiceImageH-rulerFontAsc)>>1)+rulerFontAsc;
   } else {
      x = (int)((VSPACE_COL+0.5)*choiceImageW-2);
      y = VSPACE_ROW*choiceImageH+((choiceImageH-rulerFontAsc)>>1)+rulerFontAsc;
   }
   values.foreground = myFgPixel;
   values.background = myBgPixel;
   values.font = rulerFontPtr->fid;
   XChangeGC(mainDisplay, choiceGC, GCForeground | GCBackground | GCFont,
         &values);

   XDrawString(mainDisplay, choiceWindow, choiceGC, x, y, s, len);

   values.foreground = xorOne;
   values.background = xorZero;
   XChangeGC(mainDisplay, choiceGC, GCForeground | GCBackground, &values);
}

void ShowZoom()
{
   int len, x, y, w, x_w, one_w;
   char s[80];
   XGCValues values;
   int image_x=ZOOM_COL*choiceImageW, image_y=ZOOM_ROW*choiceImageH;

   if (threeDLook) {
      image_x = 1+windowPadding+ZOOM_COL*(choiceImageW+windowPadding);
      image_y = 1+windowPadding+ZOOM_ROW*(choiceImageH+windowPadding);
   }
   XClearArea(mainDisplay, choiceWindow, image_x-windowPadding,
         image_y-windowPadding, choiceImageW+(windowPadding<<1),
         choiceImageH+(windowPadding<<1), FALSE);

   sprintf(s, "x%1d", 1<<zoomScale);
   len = strlen(s);

   w = XTextWidth(rulerFontPtr, s, len)+1;
   x_w = XTextWidth(rulerFontPtr, "x", 1)+1;
   one_w = XTextWidth(rulerFontPtr, "1", 1);
   x = image_x+((choiceImageW-w)>>1);
   y = image_y+((choiceImageH-rulerFontAsc)>>1)+rulerFontAsc;
   values.foreground = myFgPixel;
   values.background = myBgPixel;
   values.font = rulerFontPtr->fid;
   XChangeGC(mainDisplay, choiceGC, GCForeground | GCBackground | GCFont,
         &values);

   /* do not translate -- program constants */
   XDrawString(mainDisplay, choiceWindow, choiceGC, x, y, "x", 1);

   x += x_w;
   sprintf(s, "%1d", 1<<zoomScale);
   len--;
   if (zoomedIn || zoomScale==0) {
      XDrawString(mainDisplay, choiceWindow, choiceGC, x, y, s, len);
   } else {
      y = ZOOM_ROW*choiceImageH + (choiceImageH>>1) + windowPadding + 1;
      /* do not translate -- program constants */
      XDrawString(mainDisplay, choiceWindow, choiceGC, x+((w-x_w-one_w)>>1),
            y-2, "1", 1);
      XDrawLine(mainDisplay, choiceWindow, choiceGC, x, y, x+(w-x_w-1), y);
      XDrawString(mainDisplay, choiceWindow, choiceGC, x, y+rulerFontAsc,
            s, len);
   }
   values.foreground = xorOne;
   values.background = xorZero;
   XChangeGC(mainDisplay, choiceGC, GCForeground | GCBackground, &values);
}

void ShowTextSize()
{
   int len, x, y, w;
   char s[80];
   XGCValues values;
   int image_x=TEXT_SIZE_COL*choiceImageW, image_y=TEXT_SIZE_ROW*choiceImageH;
   int sz_unit=GetCurSzUnit();

   if (threeDLook) {
      image_x = 1+windowPadding+TEXT_SIZE_COL*(choiceImageW+windowPadding);
      image_y = 1+windowPadding+TEXT_SIZE_ROW*(choiceImageH+windowPadding);
   }
   XClearArea(mainDisplay, choiceWindow, image_x-windowPadding,
         image_y-windowPadding, choiceImageW+(windowPadding<<1),
         choiceImageH+(windowPadding<<1), FALSE);

   if (showFontSizeInPoints) {
      sprintf(s, "%1dpt", SzUnitToPointSize(sz_unit));
   } else {
      sprintf(s, "%1d", SzUnitToFontSize(sz_unit));
   }
   len = strlen(s);
   w = XTextWidth(defaultFontPtr, s, len);
   x = image_x + ((choiceImageW-w)>>1);
   y = image_y + ((choiceImageH-defaultFontAsc)>>1) + defaultFontAsc;

   values.foreground = myFgPixel;
   values.background = myBgPixel;
   values.font = defaultFontPtr->fid;
   XChangeGC(mainDisplay, choiceGC, GCForeground | GCBackground | GCFont,
         &values);

   XDrawString(mainDisplay, choiceWindow, choiceGC, x, y, s, len);

   values.foreground = xorOne;
   values.background = xorZero;
   XChangeGC(mainDisplay, choiceGC, GCForeground | GCBackground, &values);
}

void ShowRotate()
{
   int ltx, lty, w, h, rbx, rby, x, y;
   XGCValues values;
   Pixmap bitmap=None;
   int image_x=ROTATE_COL*choiceImageW, image_y=ROTATE_ROW*choiceImageH;

   if (threeDLook) {
      image_x = 1+windowPadding+ROTATE_COL*(choiceImageW+windowPadding);
      image_y = 1+windowPadding+ROTATE_ROW*(choiceImageH+windowPadding);
   }
   UpdateAbcBitmap();
   XClearArea(mainDisplay, choiceWindow, image_x-windowPadding,
         image_y-windowPadding, choiceImageW+(windowPadding<<1),
         choiceImageH+(windowPadding<<1), FALSE);

   if (textRotation == 0) {
      bitmap = abcBitmap;
      x = 0;
      y = 0;
      w = abc_width;
      h = abc_height;
   } else {
      bitmap = rotatedAbcBitmap;
      x = rotatedAbcBBox.ltx;
      y = rotatedAbcBBox.lty;
      w = rotatedAbcBBox.rbx-x;
      h = rotatedAbcBBox.rby-y;
   }
   ltx = ((choiceImageW-w)>>1);
   lty = ((choiceImageH-h)>>1);
   rbx = ltx + w;
   rby = lty + h;
   if (ltx < 0) {
      x = (-ltx);
      w -= x;
      ltx = 0;
   }
   if (lty < 0) {
      y = (-lty);
      h -= y;
      lty = 0;
   }
   if (rbx > choiceImageW) {
      w -= (rbx-choiceImageW);
      rbx = choiceImageW;
   }
   if (rby > choiceImageH) {
      h -= (rbx-choiceImageH);
      rby = choiceImageH;
   }
   values.foreground = myFgPixel;
   values.background = myBgPixel;
   values.function = GXcopy;
   values.fill_style = FillStippled;
   values.ts_x_origin = image_x+ltx-x;
   values.ts_y_origin = image_y+lty-y;
   values.stipple = bitmap;
   XChangeGC(mainDisplay, patGC,
         GCForeground | GCBackground | GCFunction | GCFillStyle | GCStipple |
         GCTileStipXOrigin | GCTileStipYOrigin, &values);
   XFillRectangle(mainDisplay, choiceWindow, patGC, image_x+ltx, image_y+lty,
         rbx-ltx, rby-lty);
   XSetTSOrigin(mainDisplay, patGC, 0, 0);
}

void ShowLineWidth()
{
   char s[40];
   int x, y, len, w;
   XGCValues values;
   int image_x=LINE_WIDTH_COL*choiceImageW, image_y=LINE_WIDTH_ROW*choiceImageH;

   if (threeDLook) {
      image_x = 1+windowPadding+LINE_WIDTH_COL*(choiceImageW+windowPadding);
      image_y = 1+windowPadding+LINE_WIDTH_ROW*(choiceImageH+windowPadding);
   }
   ShowStipple(mainDisplay, choiceWindow, rasterGC,
         shortLineWidthPixmap[lineWidth], LINE_WIDTH_COL, LINE_WIDTH_ROW,
         choiceImageW, choiceImageH);

   UtilStrCpyN(s, sizeof(s), curWidthOfLineSpec[lineWidth]);
   len = strlen(s);
   w = rulerFontWidth * len;
   x = image_x+((choiceImageW-w)>>1);
   y = image_y+((choiceImageH-rulerFontAsc)>>1) + rulerFontAsc;
   values.foreground = (threeDLook ? myLtGryPixel : myBgPixel);
   values.background = (threeDLook ? myLtGryPixel : myBgPixel);
   values.font = rulerFontPtr->fid;
   XChangeGC(mainDisplay, choiceGC, GCForeground | GCBackground | GCFont,
         &values);
   XFillRectangle(mainDisplay, choiceWindow, choiceGC,
         x-2, y-rulerFontAsc-2, w+4, rulerFontAsc+4);
   XSetForeground(mainDisplay, choiceGC, myFgPixel);
   XDrawString(mainDisplay, choiceWindow, choiceGC, x, y, s, len);
   values.foreground = xorOne;
   values.background = xorZero;
   XChangeGC(mainDisplay, choiceGC, GCForeground | GCBackground, &values);
}

void ShowLineStyle()
{
   ShowStipple(mainDisplay, choiceWindow, rasterGC,
         shortLineStylePixmap[lineStyle], LINE_STYLE_COL, LINE_STYLE_ROW,
         choiceImageW, choiceImageH);
}

void ShowLineType()
{
   ShowStipple(mainDisplay, choiceWindow, rasterGC,
         shortLineTypePixmap[curSpline], LINE_TYPE_COL, LINE_TYPE_ROW,
         choiceImageW, choiceImageH);
}

void ShowDash()
{
   ShowStipple(mainDisplay, choiceWindow, rasterGC, shortDashPixmap[curDash],
         DASH_COL, DASH_ROW, choiceImageW, choiceImageH);
}

void ShowFile()
{
   switch (pageStyle) {
   case PORTRAIT:
      ShowStipple(mainDisplay, choiceWindow, rasterGC, filePixmap,
            FILE_COL, FILE_ROW, choiceImageW, choiceImageH);
      break;
   case LANDSCAPE:
      ShowStipple(mainDisplay, choiceWindow, rasterGC, landscapePixmap,
            FILE_COL, FILE_ROW, choiceImageW, choiceImageH);
      break;
   }
}

void ShowRCBRadius()
{
   int len, x, y;
   char s[80];
   XGCValues values;
   int image_x=RADIUS_COL*choiceImageW, image_y=RADIUS_ROW*choiceImageH;

   if (threeDLook) {
      image_x = 1+windowPadding+RADIUS_COL*(choiceImageW+windowPadding);
      image_y = 1+windowPadding+RADIUS_ROW*(choiceImageH+windowPadding);
   }
   ShowStipple(mainDisplay, choiceWindow, rasterGC, rcbRadiusPixmap,
         RADIUS_COL, RADIUS_ROW, choiceImageW, choiceImageH);

   sprintf(s, "%1d", rcbRadius);
   len = strlen(s);
   if (threeDLook) {
      x = image_x+(choiceImageW>>1)-1;
      y = image_y+((choiceImageH-rulerFontAsc)>>1)+rulerFontAsc;
   } else {
      x = (int)((RADIUS_COL+0.5)*choiceImageW-2);
      y = (((RADIUS_ROW+1)*choiceImageH-rulerFontAsc)>>1)+rulerFontAsc;
   }
   values.foreground = myFgPixel;
   values.background = myBgPixel;
   values.font = rulerFontPtr->fid;
   XChangeGC(mainDisplay, choiceGC, GCForeground | GCBackground | GCFont,
         &values);

   XDrawString(mainDisplay, choiceWindow, choiceGC, x, y, s, len);

   values.foreground = xorOne;
   values.background = xorZero;
   XChangeGC(mainDisplay, choiceGC, GCForeground | GCBackground, &values);
}

void ShowMoveMode()
{
   ShowStipple(mainDisplay, choiceWindow, rasterGC, moveModePixmap[moveMode],
         MOVE_MODE_COL, MOVE_MODE_ROW, choiceImageW, choiceImageH);
}

void ShowShape()
{
   ShowStipple(mainDisplay, choiceWindow, rasterGC, shapePixmap[SHAPE_STAR],
         SHAPE_COL, SHAPE_ROW, choiceImageW, choiceImageH);
}

void ShowStretchableTextMode()
{
   ShowStipple(mainDisplay, choiceWindow, rasterGC,
         stretchableModePixmap[stretchableText], STRETCHTEXT_COL,
         STRETCHTEXT_ROW, choiceImageW, choiceImageH);
}

void ShowFill()
{
   XGCValues values;
   int image_x=FILL_COL*choiceImageW, image_y=FILL_ROW*choiceImageH;

   if (threeDLook) {
      image_x = 1+windowPadding+FILL_COL*(choiceImageW+windowPadding);
      image_y = 1+windowPadding+FILL_ROW*(choiceImageH+windowPadding);
   }
   values.foreground = myFgPixel;
   values.background = (threeDLook ? myLtGryPixel : myBgPixel);
   values.function = GXcopy;
   values.fill_style = FillOpaqueStippled;
   values.stipple = patPixmap[objFill];
   values.ts_x_origin = image_x;
   values.ts_y_origin = image_y;
   XChangeGC(mainDisplay, patGC,
         GCForeground | GCBackground | GCFunction | GCFillStyle | GCStipple |
         GCTileStipXOrigin | GCTileStipYOrigin, &values);

   XFillRectangle(mainDisplay, choiceWindow, patGC, image_x, image_y,
         choiceImageW, choiceImageH);
   XSetTSOrigin(mainDisplay, patGC, 0, 0);
}

void ShowTransPatMode()
{
   ShowStipple(mainDisplay, choiceWindow, rasterGC, transPatPixmap[transPat],
         TRANSPAT_COL, TRANSPAT_ROW, choiceImageW, choiceImageH);
}

void ShowPen()
{
   XGCValues values;
   int image_x=PEN_COL*choiceImageW, image_y=PEN_ROW*choiceImageH;

   if (threeDLook) {
      image_x = 1+windowPadding+PEN_COL*(choiceImageW+windowPadding);
      image_y = 1+windowPadding+PEN_ROW*(choiceImageH+windowPadding);
   }
   values.foreground = myFgPixel;
   values.background = (threeDLook ? myLtGryPixel : myBgPixel);
   values.function = GXcopy;
   values.fill_style = FillOpaqueStippled;
   values.stipple = patPixmap[penPat];
   values.ts_x_origin = image_x;
   values.ts_y_origin = image_y;
   XChangeGC(mainDisplay, patGC,
         GCForeground | GCBackground | GCFunction | GCFillStyle | GCStipple |
         GCTileStipXOrigin | GCTileStipYOrigin, &values);

   XFillRectangle(mainDisplay, choiceWindow, patGC, image_x, image_y,
         choiceImageW, choiceImageH);
   XSetTSOrigin(mainDisplay, patGC, 0, 0);
   if (penPat != NONEPAT) {
      XClearArea(mainDisplay, choiceWindow,
            image_x+(choiceImageW>>2), image_y+(choiceImageH>>2),
            (choiceImageW>>1), (choiceImageH>>1), FALSE);
   }
}

void ShowPage()
{
   int len, x, y, w;
   char s[80];
   XGCValues values;
   int image_x=PAGE_COL*choiceImageW, image_y=PAGE_ROW*choiceImageH;

   if (threeDLook) {
      image_x = 1+windowPadding+PAGE_COL*(choiceImageW+windowPadding);
      image_y = 1+windowPadding+PAGE_ROW*(choiceImageH+windowPadding);
   }
   XClearArea(mainDisplay, choiceWindow, image_x-windowPadding,
         image_y-windowPadding, choiceImageW+(windowPadding<<1),
         choiceImageH+(windowPadding<<1), FALSE);

   switch (pageLayoutMode) {
   case PAGE_STACK: sprintf(s, "%1d/%1d", curPageNum, lastPageNum); break;
   case PAGE_TILE: sprintf(s, "%1dx%1d", paperCol, paperRow); break;
   }
   len = strlen(s);
   w = XTextWidth(rulerFontPtr, s, len);
   x = image_x+((choiceImageW-w)>>1);
   y = image_y+((choiceImageH-rulerFontAsc)>>1) + rulerFontAsc;

   values.foreground = myFgPixel;
   values.background = myBgPixel;
   values.font = rulerFontPtr->fid;
   XChangeGC(mainDisplay, choiceGC, GCForeground | GCBackground | GCFont,
         &values);

   XDrawString(mainDisplay, choiceWindow, choiceGC, x, y, s, len);

   values.foreground = xorOne;
   values.background = xorZero;
   XChangeGC(mainDisplay, choiceGC, GCForeground | GCBackground,
         &values);

   RedrawPageWindow();
}

void ShowPageLayout()
{
   int image_x=PAGELAYOUT_COL*choiceImageW, image_y=PAGELAYOUT_ROW*choiceImageH;

   if (threeDLook) {
      image_x = 1+windowPadding+PAGELAYOUT_COL*(choiceImageW+windowPadding);
      image_y = 1+windowPadding+PAGELAYOUT_ROW*(choiceImageH+windowPadding);
   }
   XClearArea(mainDisplay, choiceWindow, image_x-windowPadding,
         image_y-windowPadding, choiceImageW+(windowPadding<<1),
         choiceImageH+(windowPadding<<1), FALSE);
   ShowStipple(mainDisplay, choiceWindow, rasterGC,
         pageLayoutPixmap[pageLayoutMode], PAGELAYOUT_COL, PAGELAYOUT_ROW,
         choiceImageW, choiceImageH);
}

static
int NeedSelectTopObject(Choice, VertexTypeObj)
   int Choice, VertexTypeObj;
{
   if (VertexTypeObj) {
      return (Choice==NOTHING || Choice==VERTEXMODE || Choice==ROTATEMODE);
   } else {
      return (Choice==NOTHING || Choice==ROTATEMODE);
   }
}

void ShowCurChoiceMouseStatus(choice, state, cur_text_under_mouse)
   int choice, cur_text_under_mouse;
   unsigned int state; /* 0 or ControlMask */
{
   if (choice == INVALID) {
      SetMouseStatus(TgLoadCachedString(CSTID_PARANED_NONE),
            TgLoadCachedString(CSTID_MAIN_MENU),
            TgLoadCachedString(CSTID_PARANED_NONE));
   } else if (choice == NOTHING && inHyperSpace) {
      SetCurChoiceMouseStatusStrings(choice, TRUE, NULL, cur_text_under_mouse,
            state);
   } else {
      SetCurChoiceMouseStatusStrings(choice, FALSE, NULL, cur_text_under_mouse,
            state);
   }
}

void SetCurChoice(NewChoice)
   int NewChoice;
{
   struct ObjRec *selected_single_text_obj=NULL;
   int old_choice=curChoice, sel_top_obj=FALSE;

   if (curChoice == NewChoice) return;
   if (gstWBInfo.do_whiteboard && (serializingFile || deserializingFile)) {
      return;
   }
   if (inSlideShow) {
      if (NewChoice == NOTHING || NewChoice == DRAWTEXT ||
            NewChoice == VERTEXMODE || NewChoice == ROTATEMODE) {
         /* these modes are not allowed in slideshow */
         return;
      }
   }
   switch (curChoice) {
   case NOTHING:
      if (topSel != NULL) {
         if (NewChoice == VERTEXMODE) {
            UnSelNonVertexObjs(TRUE, TRUE); /* with highlight */
            UpdSelBBox();
         } else if (NewChoice != ROTATEMODE) {
            if (NewChoice == DRAWTEXT && topSel != NULL && topSel == botSel &&
                  topSel->obj->type == OBJ_TEXT) {
               selected_single_text_obj = topSel->obj;
            }
            HighLightReverse();
            RemoveAllSel();
         } else {
            HighLightReverse();
         }
      }
      break;
   case DRAWTEXT:
      CreateTextObj(TRUE, TRUE);
      if (NeedSelectTopObject(NewChoice, FALSE) && textDrawn) {
         HighLightJustDrawnText();
         sel_top_obj = TRUE;
      }
      textDrawn = FALSE;
      textCursorShown = FALSE;
      break;
   case DRAWBOX:
      if (NeedSelectTopObject(NewChoice, FALSE) && boxDrawn) {
         SelectTopObj();
         sel_top_obj = TRUE;
      }
      boxDrawn = FALSE;
      break;
   case DRAWCORNEROVAL:
   case DRAWCENTEROVAL:
   case DRAWEDGECIRCLE:
      if (NeedSelectTopObject(NewChoice, FALSE) && ovalDrawn) {
         SelectTopObj();
         sel_top_obj = TRUE;
      }
      ovalDrawn = FALSE;
      break;
   case DRAWPOLY:
      if (NeedSelectTopObject(NewChoice, TRUE) && polyDrawn) {
         SelectTopObj();
         sel_top_obj = TRUE;
      }
      polyDrawn = FALSE;
      break;
   case DRAWPOLYGON:
      if (NeedSelectTopObject(NewChoice, TRUE) && polygonDrawn) {
         SelectTopObj();
         sel_top_obj = TRUE;
      }
      polygonDrawn = FALSE;
      break;
   case DRAWARC:
   case DRAWEDGEARC:
      if (NeedSelectTopObject(NewChoice, FALSE) && arcDrawn) {
         SelectTopObj();
         sel_top_obj = TRUE;
      }
      arcDrawn = FALSE;
      break;
   case DRAWRCBOX:
      if (NeedSelectTopObject(NewChoice, FALSE) && rcBoxDrawn) {
         SelectTopObj();
         sel_top_obj = TRUE;
      }
      rcBoxDrawn = FALSE;
      break;
   case FREEHAND:
      if (NeedSelectTopObject(NewChoice, TRUE) && polyDrawn) {
         SelectTopObj();
         sel_top_obj = TRUE;
      }
      polyDrawn = FALSE;
      break;
   case VERTEXMODE:
      if (NewChoice == NOTHING) {
         HighLightReverse();
         JustRemoveAllVSel();
         HighLightForward();
      } else if (NewChoice == ROTATEMODE) {
         HighLightReverse();
         JustRemoveAllVSel();
      } else {
         HighLightReverse();
         RemoveAllSel();
      }
      break;
   case ROTATEMODE:
      if (topSel != NULL) {
         if (NewChoice == VERTEXMODE) {
            UnSelNonVertexObjs(TRUE, TRUE); /* with highlight */
            UpdSelBBox();
            HighLightReverse();
         } else if (NewChoice != NOTHING) {
            if (NewChoice == DRAWTEXT && topSel != NULL && topSel == botSel &&
                  topSel->obj->type == OBJ_TEXT) {
               selected_single_text_obj = topSel->obj;
            }
            HighLightReverse();
            RemoveAllSel();
         } else {
            HighLightReverse();
         }
      }
      break;
   }

   curChoice = NewChoice;

   switch (curChoice) {
   case DRAWTEXT:
   case DRAWBOX:
   case DRAWCORNEROVAL:
   case DRAWCENTEROVAL:
   case DRAWEDGECIRCLE:
   case DRAWPOLY:
   case DRAWPOLYGON:
   case DRAWARC:
   case DRAWEDGEARC:
   case DRAWRCBOX:
   case FREEHAND:
      if (colorLayers && !colorLayerOn[colorIndex]) {
         sprintf(gszMsgBox, TgLoadString(STID_INVISIBLE_COLOR_SELECTED_YNC),
               colorIndex, colorMenuItems[colorIndex]);
         if (MsgBox(gszMsgBox, TOOL_NAME, YNC_MB) == MB_ID_YES) {
            colorLayerOn[colorIndex] = TRUE;
            RedrawColorWindow();
            ClearAndRedrawDrawWindow();
         }
      }
      break;
   case NOTHING:
      if (old_choice != VERTEXMODE && !sel_top_obj) {
         HighLightForward();
      }
      break;
   case VERTEXMODE:
      if (old_choice == ROTATEMODE) {
         HighLightForward();
      }
      break;
   case ROTATEMODE:
      if (sel_top_obj) {
         curChoice = old_choice;
         HighLightReverse();
         curChoice = ROTATEMODE;
         HighLightForward();
      } else if (old_choice == NOTHING) {
         HighLightForward();
      } else if (old_choice == VERTEXMODE) {
         HighLightForward();
      }
      break;
   }
   if (!setCurChoiceDontUpdateUI) {
      ShowCursor();

      RedrawModeWindow();
      UpdatePinnedMenu(MENU_MODE);

      ShowCurChoiceMouseStatus(curChoice, 0, FALSE);
   }
   textCursorShown = FALSE;

   if (inHyperSpace && curChoice != NOTHING) {
      ToggleHyperSpace(FALSE);
   } else if (!inHyperSpace && selected_single_text_obj != NULL) {
      CleanOuterInnerSel();
      CleanOuterInnerSelForFind();
      HighLightText(selected_single_text_obj, NULL, INVALID, NULL, INVALID);
   }
}

static int prevChoice=NOTHING;

void PushCurChoice()
   /* Kouichi Matsuda's modification */
{
   if (curChoice == NOTHING) {
      SetCurChoice(prevChoice);
   } else {
      prevChoice = curChoice;
      SetCurChoice(NOTHING);
   }
}

static
void TextVSpaceLoop(button_ev)
   XButtonEvent *button_ev;
{
   int saved_text_vspace=textVSpace, res=8, done=FALSE, need_to_restore=FALSE;
   int orig_x=button_ev->x, orig_y=button_ev->y, saved_change=0;
   XEvent ev;

   if (!debugNoPointerGrab) {
      XGrabPointer(mainDisplay, choiceWindow, False,
            PointerMotionMask | ButtonReleaseMask, GrabModeAsync,
            GrabModeAsync, None, handCursor, CurrentTime);
   }
   while (!done) {
      XNextEvent(mainDisplay, &ev);

      if (ev.type == Expose || ev.type == VisibilityNotify) {
         ExposeEventHandler(&ev, TRUE);
      } else if (ev.type == ButtonRelease) {
         XUngrabPointer(mainDisplay, CurrentTime);
         done = TRUE;
      } else if (ev.type == MotionNotify) {
         int dx=ev.xmotion.x-orig_x, dy=ev.xmotion.y-orig_y;
         int change=((abs(dx)>abs(dy)) ? (int)(dx/res) : (int)(dy/res));

         if (change != saved_change) {
            textVSpace = saved_text_vspace+change;
            ShowTextVSpace();
            saved_change = change;
         }
         while (XCheckMaskEvent(mainDisplay, PointerMotionMask, &ev)) ;
      }
   }
   ShowTextVSpace();
   if ((topSel == NULL || stickyMenuSelection) &&
         (textCursorH+textVSpace <= 0)) {
      need_to_restore = TRUE;
   }
   ChangeVSpace(textVSpace);
   if (need_to_restore) {
      textVSpace = saved_text_vspace;
      ShowTextVSpace();
   }
}

static
void RCBRadiusLoop(button_ev)
   XButtonEvent *button_ev;
{
   int saved_rcb_radius = rcbRadius, res=8, done=FALSE;
   int orig_x=button_ev->x, orig_y=button_ev->y, saved_change=0;
   XEvent ev;

   if (!debugNoPointerGrab) {
      XGrabPointer(mainDisplay, choiceWindow, False,
            PointerMotionMask | ButtonReleaseMask, GrabModeAsync,
            GrabModeAsync, None, handCursor, CurrentTime);
   }
   while (!done) {
      XNextEvent(mainDisplay, &ev);

      if (ev.type == Expose || ev.type == VisibilityNotify) {
         ExposeEventHandler(&ev, TRUE);
      } else if (ev.type == ButtonRelease) {
         XUngrabPointer(mainDisplay, CurrentTime);
         done = TRUE;
      } else if (ev.type == MotionNotify) {
         int dx=ev.xmotion.x-orig_x, dy=ev.xmotion.y-orig_y;
         int change=((abs(dx)>abs(dy)) ? (int)(dx/res) : (int)(dy/res));

         if (change != saved_change) {
            rcbRadius = max(MIN_RCB_RADIUS, saved_rcb_radius+change);
            ShowRCBRadius();
            saved_change = change;
         }
         while (XCheckMaskEvent(mainDisplay, PointerMotionMask, &ev)) ;
      }
   }
   if (topSel != NULL) ChangeAllSelRCBRadius(rcbRadius);
}

static
void ZoomLoop(button_ev)
   XButtonEvent *button_ev;
{
   int saved_zoomed_in=zoomedIn, saved_zoom_scale=zoomScale, res=8;
   int new_zoomed_in, new_zoom_scale, done=FALSE, saved_change=0;
   int initial_zoom, zoom, max_zoom, win_w, win_h;
   int orig_x=button_ev->x, orig_y=button_ev->y;
   XEvent ev;

   initial_zoom = (zoomedIn) ? (MAX_ZOOMED_IN-zoomScale) :
         (MAX_ZOOMED_IN+zoomScale);

   win_w = drawWinW;
   win_h = drawWinH;
   new_zoomed_in = zoomedIn;
   new_zoom_scale = zoomScale;
   while ((win_w>>1) >= paperWidth && (win_h>>1) >= paperHeight) {
      if (new_zoomed_in) {
         new_zoom_scale++;
      } else if (new_zoom_scale == 0) {
         new_zoomed_in = TRUE;
         new_zoom_scale++;
      } else {
         new_zoom_scale--;
      }
      win_w >>= 1;
      win_h >>= 1;
   }
   while (win_w < paperWidth || win_h < paperHeight) {
      if (!new_zoomed_in) {
         new_zoom_scale++;
      } else if (new_zoom_scale == 1) {
         new_zoomed_in = FALSE;
         new_zoom_scale--;
      } else {
         new_zoom_scale--;
      }
      win_w <<= 1;
      win_h <<= 1;
   }
   max_zoom = (new_zoomed_in) ? (MAX_ZOOMED_IN-new_zoom_scale) :
         (MAX_ZOOMED_IN+new_zoom_scale);

   if (!debugNoPointerGrab) {
      XGrabPointer(mainDisplay, choiceWindow, False,
            PointerMotionMask | ButtonReleaseMask, GrabModeAsync,
            GrabModeAsync, None, handCursor, CurrentTime);
   }
   while (!done) {
      XNextEvent(mainDisplay, &ev);

      if (ev.type == Expose || ev.type == VisibilityNotify) {
         ExposeEventHandler(&ev, TRUE);
      } else if (ev.type == ButtonRelease) {
         XUngrabPointer(mainDisplay, CurrentTime);
         done = TRUE;
      } else if (ev.type == MotionNotify) {
         int dx=ev.xmotion.x-orig_x, dy=ev.xmotion.y-orig_y;
         int change=((abs(dx)>abs(dy)) ? (int)(dx/res) : (int)(dy/res));

         if (change != saved_change) {
            zoom = initial_zoom+change;
            if (zoom < 0) zoom = 0;
            if (zoom > max_zoom) zoom = max_zoom;
            zoomedIn = (zoom < MAX_ZOOMED_IN);
            zoomScale = (zoomedIn) ? (MAX_ZOOMED_IN-zoom) :
                  (zoom-MAX_ZOOMED_IN);
            ShowZoom();
            saved_change = change;
         }
         while (XCheckMaskEvent(mainDisplay, PointerMotionMask, &ev)) ;
      }
   }
   if (saved_zoomed_in==zoomedIn && saved_zoom_scale==zoomScale) return;
   new_zoomed_in = zoomedIn;
   new_zoom_scale = zoomScale;
   zoomedIn = saved_zoomed_in;
   zoomScale = saved_zoom_scale;
   PreciseZoom(new_zoomed_in, new_zoom_scale, FALSE);
}

void FormatFloat(pfval, buf)
   float *pfval;
   char *buf;
{
   char int_buf[80], frac_buf[4];
   float fval=((*pfval)*1000.0);
   int ival=round(fval), len;

   *buf = '\0';
   if (ival == 0) {
      strcpy(buf, "0"); 
      return;
   }
   sprintf(int_buf, "%04d", ival);
   len = strlen(int_buf);
   if (strcmp(&int_buf[len-3], "000") == 0) {
      *frac_buf = '\0';
   } else if (strcmp(&int_buf[len-2], "00") == 0) {
      int_buf[len-2] = '\0';
      strcpy(frac_buf, &int_buf[len-3]);
   } else if (strcmp(&int_buf[len-1], "0") == 0) {
      int_buf[len-1] = '\0';
      strcpy(frac_buf, &int_buf[len-3]);
   } else {
      strcpy(frac_buf, &int_buf[len-3]);
   }
   int_buf[len-3] = '\0';
   if (*frac_buf == '\0') {
      strcpy(buf, int_buf);
   } else {
      sprintf(buf, "%s.%s", int_buf, frac_buf);
   }
}

void FormatAngle(angle, buf)
   int angle; /* degrees*64 */
   char *buf;
{
   float fval=(((float)angle)/64.0);

   FormatFloat(&fval, buf);
}

static
void ShowRotateMouseStatus()
{
   char left_buf[MAXSTRING], right_buf[MAXSTRING], tmp_buf[MAXSTRING];

   if (rotationIncrement == 0) {
      SetMouseStatus(TgLoadCachedString(CSTID_PARANED_NONE),
            TgLoadCachedString(CSTID_PARANED_NONE),
            TgLoadCachedString(CSTID_PARANED_NONE));
      return;
   }
   FormatAngle(rotationIncrement, tmp_buf);
   sprintf(left_buf, TgLoadCachedString(CSTID_ROTATE_DEGREES_CW), tmp_buf);
   sprintf(right_buf, TgLoadCachedString(CSTID_ROTATE_DEGREES_CCW), tmp_buf);
   SetMouseStatus(left_buf, TgLoadCachedString(CSTID_PARANED_NONE), right_buf);
}

static
int ValidChoice(choice)
   int choice;
{
   switch (choice) {
   case CHOICE_COLOR:
   case CHOICE_HORI_ALIGN:
   case CHOICE_VERT_ALIGN:
   case CHOICE_PAGE:
   case CHOICE_PAGELAYOUT:
   case CHOICE_JUST:
   case CHOICE_FONT:
   case CHOICE_VSPACE:
   case CHOICE_TEXT_SIZE:
   case CHOICE_ROTATE:
   case CHOICE_LINE_WIDTH:
   case CHOICE_LINE_STYLE:
   case CHOICE_LINE_TYPE:
   case CHOICE_DASH:
   case CHOICE_RADIUS:
   case CHOICE_ZOOM:
   case CHOICE_MOVE_MODE:
   case CHOICE_SHAPE:
   case CHOICE_STRETCHTEXT:
   case CHOICE_TRANSPAT:
   case CHOICE_PRINT:
   case CHOICE_FILE:
   case CHOICE_FILL:
   case CHOICE_PEN:
      return TRUE;
   }
   return FALSE;
}

int ChoiceEventHandler(input)
   XEvent *input;
{
   XEvent ev;
   int xindex, yindex, x=0, y=0, choice;

   if (input->type == Expose) {
      XSync(mainDisplay, False);
      while (XCheckWindowEvent(mainDisplay, choiceWindow, ExposureMask, &ev)) ;
      RedrawChoiceWindow();
   } else if (input->type == EnterNotify || input->type == LeaveNotify) {
      SetMouseStatus("", "", "");
      if (curRaisedChoice != INVALID) {
         HighLightThreeDButton(curRaisedChoice, FALSE);
         curRaisedChoice = INVALID;
      }
   } else if (input->type == MotionNotify) {
      int nIndex=0;
      char szLeft[MAXSTRING], szRight[MAXSTRING];

      if (threeDLook) {
         xindex = (int)((input->xmotion.x-1) / (choiceImageW+windowPadding));
         yindex = (int)((input->xmotion.y-1) / (choiceImageH+windowPadding));
         choice = (xindex<<CHOICE_SHIFT)|yindex;
         if (choice != curRaisedChoice) {
            if (curRaisedChoice != INVALID) {
               HighLightThreeDButton(curRaisedChoice, FALSE);
               curRaisedChoice = INVALID;
            }
            if (ValidChoice(choice)) {
               HighLightThreeDButton(choice, TRUE);
               curRaisedChoice = choice;
            }
         }
      } else {
         xindex = (int)(input->xmotion.x / choiceImageW);
         yindex = (int)(input->xmotion.y / choiceImageH);
         choice = (xindex<<CHOICE_SHIFT)|yindex;
      }
      switch (choice) {
      case CHOICE_COLOR: QuickSetMouseStatus(colorMouseStatus, 0); break;
      case CHOICE_HORI_ALIGN: QuickSetMouseStatus(hAlignMouseStatus, 0); break;
      case CHOICE_VERT_ALIGN: QuickSetMouseStatus(vAlignMouseStatus, 0); break;
      case CHOICE_PAGE:
         QuickSetMouseStatus(pageMouseStatus, pageLayoutMode);
         break;
      case CHOICE_PAGELAYOUT:
         QuickSetMouseStatus(pageLayoutMouseStatus, pageLayoutMode);
         break;
      case CHOICE_JUST: QuickSetMouseStatus(justMouseStatus, textJust); break;
      case CHOICE_FONT: QuickSetMouseStatus(fontMouseStatus, 0); break;
      case CHOICE_VSPACE: QuickSetMouseStatus(vspaceMouseStatus, 0); break;
      case CHOICE_TEXT_SIZE: QuickSetMouseStatus(textSizeMouseStatus, 0); break;
      case CHOICE_ROTATE: ShowRotateMouseStatus(); break;
      case CHOICE_LINE_WIDTH:
         QuickSetMouseStatus(lineWidthMouseStatus, 0);
         break;
      case CHOICE_LINE_STYLE:
         QuickSetMouseStatus(lineStyleMouseStatus, 0);
         break;
      case CHOICE_LINE_TYPE: QuickSetMouseStatus(lineTypeMouseStatus, 0); break;
      case CHOICE_DASH: QuickSetMouseStatus(lineDashMouseStatus, 0); break;
      case CHOICE_RADIUS: QuickSetMouseStatus(rcbRadiusMouseStatus, 0); break;
      case CHOICE_ZOOM: QuickSetMouseStatus(zoomMouseStatus, 0); break;
      case CHOICE_MOVE_MODE:
         QuickSetMouseStatus(moveModeMouseStatus, moveMode);
         break;
      case CHOICE_SHAPE: QuickSetMouseStatus(shapeMouseStatus, 0); break;
      case CHOICE_STRETCHTEXT:
         QuickSetMouseStatus(stretchableTextModeMouseStatus, stretchableText);
         break;
      case CHOICE_TRANSPAT:
         QuickSetMouseStatus(transPatMouseStatus, transPat);
         break;
      case CHOICE_PRINT:
         nIndex = GetExportIndex(whereToPrint, EXPORT_NEXT);
         if (nIndex == PRINTER) {
            sprintf(szLeft, TgLoadCachedString(CSTID_PRINT_MODE),
                  GetExportName(whereToPrint, EXPORT_NEXT));
         } else {
            sprintf(szLeft, TgLoadCachedString(CSTID_EXPORT_MODE),
                  GetExportName(whereToPrint, EXPORT_NEXT));
         }
         nIndex = GetExportIndex(whereToPrint, EXPORT_PREV);
         if (nIndex == PRINTER) {
            sprintf(szRight, TgLoadCachedString(CSTID_PRINT_MODE),
                  GetExportName(whereToPrint, EXPORT_PREV));
         } else {
            sprintf(szRight, TgLoadCachedString(CSTID_EXPORT_MODE),
                  GetExportName(whereToPrint, EXPORT_PREV));
         }
         SetMouseStatus(szLeft, TgLoadCachedString(CSTID_PRINT_EXPORT_MENU),
               szRight);
         break;
      case CHOICE_FILE: QuickSetMouseStatus(fileMouseStatus, pageStyle); break;
      case CHOICE_FILL: QuickSetMouseStatus(fillMouseStatus, 0); break;
      case CHOICE_PEN: QuickSetMouseStatus(penMouseStatus, 0); break;
      }
   } else if (input->type == ButtonPress) {
      XButtonEvent *button_ev=(&(input->xbutton));
      int delta=0, root_x, root_y;
      Window root_win, child_win;
      unsigned int status;

      Msg("");
      switch (button_ev->button) {
      case Button1: delta = 1; break;
      case Button2:
         XQueryPointer(mainDisplay, rootWindow, &root_win, &child_win,
               &root_x, &root_y, &x, &y, &status);
         break;
      case Button3: delta = -1; break;
      default: return INVALID;
      }
      if (threeDLook) {
         xindex = (int)((input->xmotion.x-1) / (choiceImageW+windowPadding));
         yindex = (int)((input->xmotion.y-1) / (choiceImageH+windowPadding));
         choice = (xindex<<CHOICE_SHIFT)|yindex;
         if (choice != curRaisedChoice) {
            if (curRaisedChoice != INVALID) {
               HighLightThreeDButton(curRaisedChoice, FALSE);
               curRaisedChoice = INVALID;
            }
            if (ValidChoice(choice)) {
               HighLightThreeDButton(choice, TRUE);
               curRaisedChoice = choice;
            }
         }
      } else {
         xindex = (int)(input->xmotion.x / choiceImageW);
         yindex = (int)(input->xmotion.y / choiceImageH);
         choice = (xindex<<CHOICE_SHIFT)|yindex;
      }
      if (delta != 0) {
         cycleThroughChoice = TRUE;
      }
      switch (choice) {
      case CHOICE_COLOR:
         if (colorDisplay) {
            if (delta == 0) {
               ColorMenu(x, y, FALSE);
            } else {
               if (curChoice != NOTHING &&
                     !(curChoice == DRAWTEXT && textCursorShown)) {
                  SetCurChoice(NOTHING);
               }
               colorIndex = (colorIndex+maxColors+delta) % maxColors;
               if (colorLayers && !colorLayerOn[colorIndex]) {
                  sprintf(gszMsgBox,
                        TgLoadString(STID_COLOR_IS_CURRENTLY_INVISIBLE),
                        colorIndex, colorMenuItems[colorIndex]);
                  Msg(gszMsgBox);
               }
               ShowColor(TRUE);
               UpdatePinnedMenu(MENU_COLOR);
               if (topSel != NULL) {
                  ChangeAllSelColor(colorIndex, TRUE);
               } else if (curChoice == DRAWTEXT && textCursorShown) {
                  ChangeAllSelColor(colorIndex, FALSE);
               } else {
                  sprintf(gszMsgBox, TgLoadString(STID_COLOR_SET_TO),
                        colorMenuItems[colorIndex]);
                  Msg(gszMsgBox);
               }
               QuickSetMouseStatus(colorMouseStatus, 0);
            }
         }
         break;
      case CHOICE_HORI_ALIGN:
         if (delta == 0) {
            HoriAlignMenu(x, y, FALSE);
         } else {
            HoriAlignSubMenu((horiAlign+MAXALIGNS+delta) % MAXALIGNS);
            QuickSetMouseStatus(hAlignMouseStatus, 0);
         }
         break;
      case CHOICE_VERT_ALIGN:
         if (delta == 0) {
            VertAlignMenu(x, y, FALSE);
         } else {
            VertAlignSubMenu((vertAlign+MAXALIGNS+delta) % MAXALIGNS);
            QuickSetMouseStatus(vAlignMouseStatus, 0);
         }
         break;
      case CHOICE_PAGE:
         if (delta == 0) {
            PageMenu(x, y, FALSE);
         } else if (pageLayoutMode == PAGE_STACK) {
            if (delta == 1) {
               NextPage();
            } else {
               PrevPage();
            }
            QuickSetMouseStatus(pageMouseStatus, pageLayoutMode);
         } else {
            SpecifyDrawingSize();
            QuickSetMouseStatus(pageMouseStatus, pageLayoutMode);
         }
         break;
      case CHOICE_PAGELAYOUT:
         if (delta == 0) {
            PageLayoutMenu(x, y, FALSE);
         } else {
            PageLayoutSubMenu(!pageLayoutMode);
            QuickSetMouseStatus(pageLayoutMouseStatus, pageLayoutMode);
         }
         break;
      case CHOICE_JUST:
         if (delta == 0) {
            StyleMenu(x, y, FALSE);
         } else {
            if (curChoice == DRAWTEXT && !textCursorShown) {
               if (TieLooseEnds()) {
                  SetCurChoice(NOTHING);
                  HighLightReverse();
               } else {
                  SetCurChoice(NOTHING);
               }
               RemoveAllSel();
            }
            textJust = (textJust+MAXJUSTS+delta) % MAXJUSTS;
            ShowJust();
            UpdatePinnedMenu(MENU_STYLE);
            if (topSel != NULL) {
               ChangeFontJust(textJust);
            } else if (curChoice == DRAWTEXT && textCursorShown) {
               ChangeFontJust(textJust);
            }
            QuickSetMouseStatus(justMouseStatus, textJust);
         }
         break;
      case CHOICE_FONT:
         if (delta == 0) {
            FontMenu(x, y, FALSE);
         } else {
            if (curChoice == DRAWTEXT && !textCursorShown) {
               if (TieLooseEnds()) {
                  SetCurChoice(NOTHING);
                  HighLightReverse();
               } else {
                  SetCurChoice(NOTHING);
               }
               RemoveAllSel();
            }
            ChangeFont((curFont+numFonts+delta)%numFonts, TRUE);
            QuickSetMouseStatus(fontMouseStatus, 0);
         }
         break;
      case CHOICE_VSPACE:
         if (delta == 0) {
            TextVSpaceLoop(button_ev);
         } else {
            if (curChoice == DRAWTEXT && !textCursorShown) {
               if (TieLooseEnds()) {
                  SetCurChoice(NOTHING);
                  HighLightReverse();
               } else {
                  SetCurChoice(NOTHING);
               }
               RemoveAllSel();
            }
            if (topSel == NULL && delta < 0 &&
                  textCursorH+textVSpace+delta <= 0) {
               Msg(TgLoadString(STID_VSPACE_TOO_SMALL_NOT_CHANGED));
            } else {
               textVSpace += delta;
               ShowTextVSpace();
               if (topSel != NULL) {
                  ChangeVSpace(textVSpace);
               } else if (curChoice == DRAWTEXT && textCursorShown) {
                  ChangeVSpace(textVSpace);
               }
            }
            QuickSetMouseStatus(vspaceMouseStatus, 0);
         }
         break;
      case CHOICE_TEXT_SIZE:
         if (delta == 0) {
            SizeMenu(x, y, FALSE);
         } else {
            int cur_index=INVALID, saved_index=INVALID;

            if (curChoice == DRAWTEXT && !textCursorShown) {
               if (TieLooseEnds()) {
                  SetCurChoice(NOTHING);
                  HighLightReverse();
               } else {
                  SetCurChoice(NOTHING);
               }
               RemoveAllSel();
            }
            cur_index = saved_index = GetSizeMenuIndex();
            if (cur_index == INVALID) {
               cur_index = 0;
            } else {
               cur_index = (cur_index+numFontSizes+delta) % numFontSizes;
            }
            if (curChoice == DRAWTEXT && textCursorShown && editTextSize != 0) {
               if (RestoreEditTextSize(curTextObj, TRUE)) {
                  UpdTextBBox(curTextObj);
               }
            }
            curSzUnit = fontSzUnits[cur_index];
            allowFontFaceSubstitution = FALSE;
            SetCanvasFont();
            allowFontFaceSubstitution = TRUE;
            if (canvasFontSize == INVALID) {
               sprintf(gszMsgBox,
                     TgLoadString(STID_FONT_AND_SIZE_NOT_AVAILABLE),
                     fontMenuStr[curFont], SzUnitToFontSize(curSzUnit));
               MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
               cur_index = saved_index;
               curSzUnit = fontSzUnits[cur_index];
               SetCanvasFont();
            }
            PutCurSzUnit(curSzUnit);
            if (curChoice == DRAWTEXT && textCursorShown && editTextSize != 0) {
               if (RestoreEditTextSize(curTextObj, FALSE)) {
                  UpdTextBBox(curTextObj);
               }
            }
            ShowCurFont();
            ShowTextSize();

            UpdatePinnedMenu(MENU_SIZE);
            if (topSel != NULL) {
               ChangeFontSize(cur_index);
            } else if (curChoice == DRAWTEXT && textCursorShown) {
               ChangeFontSize(cur_index);
            }
            QuickSetMouseStatus(textSizeMouseStatus, 0);
         }
         break;
      case CHOICE_ROTATE:
         if (delta != 0) {
            if (curChoice == DRAWTEXT) {
               if (TieLooseEnds()) {
                  SetCurChoice(NOTHING);
                  HighLightReverse();
               } else {
                  SetCurChoice(NOTHING);
               }
               RemoveAllSel();
            }
            textRotation += delta*rotationIncrement;
            while (textRotation < 0) textRotation += (360<<6);
            while (textRotation >= (360<<6)) textRotation -= (360<<6);
            ShowRotate();
            if (topSel != NULL) {
               if (delta == 1) {
                  RotateClockWise();
               } else {
                  RotateCounter();
               }
            }
         }
         ShowRotateMouseStatus();
         break;
      case CHOICE_LINE_WIDTH:
         if (delta == 0) {
            LineWidthMenu(x, y, FALSE);
         } else {
            lineWidth = (lineWidth+maxLineWidths+delta) % maxLineWidths;
            ShowLineWidth();
            UpdatePinnedMenu(MENU_LINEWIDTH);
            if (topSel != NULL) {
               ChangeAllSelLineWidth(lineWidth, TRUE);
            } else {
               sprintf(gszMsgBox, TgLoadString(STID_LINE_WIDTH_SET_TO),
                     curWidthOfLineSpec[lineWidth]);
               Msg(gszMsgBox);
            }
            QuickSetMouseStatus(lineWidthMouseStatus, 0);
         }
         break;
      case CHOICE_LINE_STYLE:
         if (delta == 0) {
            LineStyleMenu(x, y, FALSE);
         } else {
            lineStyle = (lineStyle+MAXLINESTYLES+delta) % MAXLINESTYLES;
            ShowLineStyle();
            UpdatePinnedMenu(MENU_LINESTYLE);
            if (topSel != NULL) ChangeAllSelLineStyle(lineStyle, TRUE);
            QuickSetMouseStatus(lineStyleMouseStatus, 0);
         }
         break;
      case CHOICE_LINE_TYPE:
         if (delta == 0) {
            LineTypeMenu(x, y, FALSE);
         } else {
            curSpline = (curSpline+MAXLINETYPES+delta) % MAXLINETYPES;
            ShowLineType();
            UpdatePinnedMenu(MENU_LINETYPE);
            if (topSel != NULL) {
               ChangeAllSelLineType(curSpline, TRUE);
            } else {
               *gszMsgBox = '\0';
               switch (curSpline) {
               case LT_STRAIGHT:
                  Msg(TgLoadString(STID_LINE_TYPE_IS_STRAIGHT));
                  break;
               case LT_SPLINE:
                  Msg(TgLoadString(STID_LINE_TYPE_IS_SPLINE));
                  break;
               case LT_INTSPLINE:
                  Msg(TgLoadString(STID_LINE_TYPE_IS_INTSPLINE));
                  break;
               case LT_STRUCT_SPLINE:
                  Msg(TgLoadString(STID_LINE_TYPE_IS_STRUCT_SPLINE));
                  break;
               }
            }
            QuickSetMouseStatus(lineTypeMouseStatus, 0);
         }
         break;
      case CHOICE_DASH:
         if (delta == 0) {
            LineDashMenu(x, y, FALSE);
         } else {
            curDash = (curDash+MAXDASHES+delta) % MAXDASHES;
            ShowDash();
            UpdatePinnedMenu(MENU_LINEDASH);
            if (topSel != NULL) ChangeAllSelLineDash(curDash, TRUE);
            QuickSetMouseStatus(lineDashMouseStatus, 0);
         }
         break;
      case CHOICE_RADIUS:
         if (delta == 0) {
            RCBRadiusLoop(button_ev);
         } else {
            if (rcbRadius+delta >= MIN_RCB_RADIUS) {
               rcbRadius += delta;
               ShowRCBRadius();
               if (topSel != NULL) ChangeAllSelRCBRadius(rcbRadius);
            }
            QuickSetMouseStatus(rcbRadiusMouseStatus, 0);
         }
         break;
      case CHOICE_ZOOM:
         switch (delta) {
         case -1: ZoomOut(); QuickSetMouseStatus(zoomMouseStatus, 0); break;
         case 0: ZoomLoop(button_ev); break;
         case 1: ZoomIn(); break;
         }
         break;
      case CHOICE_MOVE_MODE:
         if (delta == 0) {
            MoveModeMenu(x, y, FALSE);
         } else {
            ToggleMoveMode();
            QuickSetMouseStatus(moveModeMouseStatus, moveMode);
         }
         break;
      case CHOICE_SHAPE:
         if (delta == 0) ShapeMenu(x, y, FALSE);
         break;
      case CHOICE_STRETCHTEXT:
         if (delta == 0) {
            StretchableTextModeMenu(x, y, FALSE);
         } else {
            ToggleStretchableText();
            QuickSetMouseStatus(stretchableTextModeMouseStatus,
                  stretchableText);
         }
         break;
      case CHOICE_TRANSPAT:
         if (delta == 0) {
            TransPatModeMenu(x, y, FALSE);
         } else {
            transPat = !transPat;
            ShowTransPatMode();
            UpdatePinnedMenu(MENU_TRANSPAT);
            if (topSel != NULL) ChangeAllSelTransPat(transPat, TRUE);
            QuickSetMouseStatus(transPatMouseStatus, transPat);
         }
         break;
      case CHOICE_PRINT:
         if (delta == 0) {
            ExportFormatMenu(x, y, FALSE);
         } else {
            whereToPrint = (whereToPrint+maxWhereToPrint+delta) %
                  maxWhereToPrint-1;
            ToggleWhereToPrint();
         }
         break;
      case CHOICE_FILE:
         if (delta == 0) {
            PageStyleMenu(x, y, FALSE);
         } else {
            switch (pageStyle) {
            case PORTRAIT: ChangePageStyle(LANDSCAPE); break;
            case LANDSCAPE: ChangePageStyle(PORTRAIT); break;
            }
            QuickSetMouseStatus(fileMouseStatus, pageStyle);
         }
         break;
      case CHOICE_FILL:
         if (delta == 0) {
            FillMenu(x, y, FALSE);
         } else {
            if (curChoice == DRAWTEXT && !textCursorShown) {
               if (TieLooseEnds()) {
                  SetCurChoice(NOTHING);
                  HighLightReverse();
               } else {
                  SetCurChoice(NOTHING);
               }
               RemoveAllSel();
            }
            objFill = (objFill+MAXPATTERNS+delta) % MAXPATTERNS;
            ShowFill();
            UpdatePinnedMenu(MENU_FILL);
            if (topSel != NULL) {
               ChangeAllSelFill(objFill, TRUE);
            } else if (curChoice == DRAWTEXT && textCursorShown) {
               ChangeAllSelFill(objFill, FALSE);
            }
            QuickSetMouseStatus(fillMouseStatus, 0);
         }
         break;
      case CHOICE_PEN:
         if (delta == 0) {
            PenMenu(x, y, FALSE);
         } else {
            if (curChoice == DRAWTEXT && !textCursorShown) {
               if (TieLooseEnds()) {
                  SetCurChoice(NOTHING);
                  HighLightReverse();
               } else {
                  SetCurChoice(NOTHING);
               }
               RemoveAllSel();
            }
            penPat = (penPat+MAXPATTERNS+delta) % MAXPATTERNS;
            ShowPen();
            UpdatePinnedMenu(MENU_PEN);
            if (topSel != NULL) {
               ChangeAllSelPen(penPat, TRUE);
            } else if (curChoice == DRAWTEXT && textCursorShown) {
               ChangeAllSelPen(penPat, FALSE);
            }
            QuickSetMouseStatus(penMouseStatus, 0);
         }
         break;
      }
      cycleThroughChoice = FALSE;
   }
   return INVALID;
}

static
void HighLightModeThreeDButton(mode, highlight)
   int mode, highlight;
{
   struct BBRec bbox;

   if (!threeDLook) return;

   bbox.ltx = (windowPadding>>1);
   bbox.lty = (windowPadding>>1)+mode*(choiceImageH+(windowPadding<<1));
   bbox.rbx = bbox.ltx+choiceImageW+(windowPadding<<1);
   bbox.rby = bbox.lty+choiceImageH+(windowPadding<<1);
   if (highlight) {
      TgDrawThreeDButton(mainDisplay, modeWindow, textMenuGC, &bbox,
            TGBS_RAISED, 2, FALSE);
   } else if (mode == curChoice) {
      TgDrawThreeDButton(mainDisplay, modeWindow, textMenuGC, &bbox,
            TGBS_LOWRED, 2, FALSE);
   } else {
      TgClearThreeDButton(mainDisplay, modeWindow, textMenuGC, &bbox, 2);
   }
}

void RedrawModeWindow()
{
   int i, x=(windowPadding>>1), y=(windowPadding>>1);
   XGCValues values;

   XClearWindow(mainDisplay, modeWindow);

   for (i=0; i < MAXCHOICES; i++) {
      Pixmap bitmap=((i==DRAWPOLY && connectingPortsByWire) ? wireBitmap :
            choicePixmap[i]);

      if (threeDLook) {
         values.stipple = bitmap;
         values.ts_x_origin = x+windowPadding;
         values.ts_y_origin = y+windowPadding;
         XChangeGC(mainDisplay, rasterGC,
               GCStipple | GCTileStipXOrigin | GCTileStipYOrigin, &values);
         XFillRectangle(mainDisplay, modeWindow, rasterGC, values.ts_x_origin,
               values.ts_y_origin, choiceImageW, choiceImageH);
         XSetTSOrigin(mainDisplay, rasterGC, 0, 0);
         if (i == curChoice) {
            struct BBRec bbox;

            SetBBRec(&bbox, x, y, x+choiceImageW+(windowPadding<<1),
                  y+choiceImageH+(windowPadding<<1));
            TgDrawThreeDButton(mainDisplay, modeWindow, textMenuGC, &bbox,
                  TGBS_LOWRED, 2, FALSE);
         }
      } else {
         if (i == curChoice) {
            XSetForeground(mainDisplay, textMenuGC, myFgPixel);
            XFillRectangle(mainDisplay, modeWindow, textMenuGC,
                  x+windowPadding, y+windowPadding, choiceImageW, choiceImageH);
            XSetStipple(mainDisplay, rvPixmapMenuGC, bitmap);
            XFillRectangle(mainDisplay, modeWindow, rvPixmapMenuGC,
                  x+windowPadding, y+windowPadding, choiceImageW, choiceImageH);
         } else {
            XSetStipple(mainDisplay, rasterGC, bitmap);
            XFillRectangle(mainDisplay, modeWindow, rasterGC, x+windowPadding,
                  y+windowPadding, choiceImageW, choiceImageH);
         }
      }
      y += choiceImageH+(windowPadding<<1);
   }
   if (threeDLook) {
      struct BBRec bbox;

      SetBBRec(&bbox, 0, 0, modeWindowW, modeWindowH);
      TgDrawThreeDButton(mainDisplay, modeWindow, textMenuGC, &bbox,
            TGBS_RAISED, 1, FALSE);
   }
}

int ModeEventHandler(input)
   XEvent *input;
{
   XEvent ev;
   int mode=INVALID;

   if (noModeWindow) return INVALID;

   if (input->type == Expose) {
      XSync(mainDisplay, False);
      while (XCheckWindowEvent(mainDisplay, modeWindow, ExposureMask, &ev)) ;
      RedrawModeWindow();
   } else if (input->type == EnterNotify || input->type == LeaveNotify) {
      if (input->type == EnterNotify) {
         SetStringStatus("");
      } else {
         SetMouseStatus("", "", "");
      }
      if (curRaisedMode != INVALID) {
         HighLightModeThreeDButton(curRaisedMode, FALSE);
         curRaisedMode = INVALID;
      }
   } else if (input->type == MotionNotify) {
      mode = (int)((input->xmotion.y-(windowPadding>>1)) /
            (choiceImageH+(windowPadding<<1)));
      if (threeDLook) {
         if (mode != curRaisedMode) {
            if (curRaisedMode != INVALID) {
               HighLightModeThreeDButton(curRaisedMode, FALSE);
               curRaisedMode = INVALID;
            }
            if (mode >= 0 && mode < MAXCHOICES) {
               HighLightModeThreeDButton(mode, TRUE);
               curRaisedMode = mode;
            }
         }
      }
      if (mode >=0 && mode < MAXCHOICES) {
         TgMenuItemInfo *menu_iteminfos=modeMenuInfo.items;

         if (mode == ROTATEMODE) {
            QuickSetMouseStatus(autoRotatePivotMouseStatus, 0);
         } else {
            /* the raw status_str is English */
            SetStringStatus(_(menu_iteminfos[mode].status_str));
         }
      } else {
         SetStringStatus("");
      }
   } else if (input->type == ButtonPress) {
      XButtonEvent *button_ev=(&(input->xbutton));

      mode = (int)((input->xbutton.y-(windowPadding>>1)) /
            (choiceImageH+(windowPadding<<1)));
      if (mode >=0 && mode < MAXCHOICES) {
         if (button_ev->button == Button2 && mode == ROTATEMODE) {
            int x=0, y=0, root_x=0, root_y=0;
            Window root_win=None, child_win=None;
            unsigned int status=0;

            XQueryPointer(mainDisplay, rootWindow, &root_win, &child_win,
                  &root_x, &root_y, &x, &y, &status);
            AutoRotatePivotMenu(x, y, FALSE);
         } else {
            TgMenuItemInfo *menu_iteminfos=modeMenuInfo.items;

            SetCurChoice(mode);
            /* the raw status_str is English */
            SetStringStatus(_(menu_iteminfos[mode].status_str));
         }
      } else if (button_ev->button == Button2 && mode >= MAXCHOICES) {
         return MainMenu();
      }
   }
   return INVALID;
}
