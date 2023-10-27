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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/oval.c,v 1.11 2011/06/09 16:11:42 cvsps Exp $
 */

#define _INCLUDE_FROM_OVAL_C_

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
#include "grid.e"
#include "mainloop.e"
#include "mark.e"
#include "msg.e"
#include "obj.e"
#include "oval.e"
#include "pattern.e"
#include "poly.e"
#include "ps.e"
#include "raster.e"
#include "rect.e"
#include "ruler.e"
#include "select.e"
#include "setup.e"
#include "spline.e"
#include "strtbl.e"
#include "util.e"
#include "xpixmap.e"

int ovalDrawn=FALSE;

static
void GetPSEllipseStr(xc, yc, a, b, s)
   int xc, yc, a, b;
   char *s;
{
   if (preDumpSetup) PSUseEllipse();
#ifdef INVERT_CTM_BUG
   if (preDumpSetup) PSUseMinRadius();
   sprintf(s, "%s %1d %1d %1d %s %1d %s TGEL",
         gPsCmd[PS_NEWPATH], xc, yc, a, "tgif_min_radius",
         b, "tgif_min_radius");
#else
   sprintf(s, "%s %1d %1d %1d %1d TGEL",
         gPsCmd[PS_NEWPATH], xc, yc, a, b);
#endif
}

void MyOval(window, gc, bbox)
   Window window;
   GC gc;
   struct BBRec bbox;
{
   int ltx, lty, w, h;

   if (bbox.ltx > bbox.rbx) {
      ltx = bbox.rbx; w = bbox.ltx - ltx;
   } else {
      ltx = bbox.ltx; w = bbox.rbx - ltx;
   }
   if (bbox.lty > bbox.rby) {
      lty = bbox.rby; h = bbox.lty - lty;
   } else {
      lty = bbox.lty; h = bbox.rby - lty;
   }
   XDrawArc(mainDisplay, window, gc, ltx, lty, w, h, 0, (360<<6));
}

static
void DumpOvalPath(FP, ObjPtr, Xc, Yc, A, B, Width, Pen, Dash, TransPat)
   FILE *FP;
   struct ObjRec *ObjPtr;
   int Xc, Yc, A, B, Width, Pen, Dash, TransPat;
{
   register int i;
   int w_is_int=TRUE;
   char *width_spec=ObjPtr->detail.o->width_spec;
   double dw=GetWidthInDouble(Width, width_spec, &w_is_int);
   char s[MAXSTRING];

   fprintf(FP, "   %s\n", gPsCmd[PS_GSAVE]);
   if (!colorDump && useGray && Pen > BACKPAT) {
      GrayCheck(Pen);
      fprintf(FP, "      %s %s\n", GrayStr(Pen), gPsCmd[PS_SETGRAY]);
   }
   GetPSEllipseStr(Xc, Yc, A, B, s);
   fprintf(FP, "      %s\n", s);

   if (ObjPtr->ctm != NULL) {
      fprintf(FP, "      %s\n", &(gPsCmd[PS_TGIFSETMATRIX])[1]);
   }
   if (w_is_int) {
      if (Width != 1) {
         fprintf(FP, "      %1d %s\n", Width, gPsCmd[PS_SETLINEWIDTH]);
      }
   } else {
      fprintf(FP, "      %.3f %s\n", dw, gPsCmd[PS_SETLINEWIDTH]);
   }
   if (Dash != 0) {
      fprintf(FP, "      [");
      for (i = 0; i < dashListLength[Dash]-1; i++) {
         fprintf(FP, "%1d ", (int)(dashList[Dash][i]));
      }
      fprintf(FP, "%1d] 0 %s\n",
            (int)(dashList[Dash][dashListLength[Dash]-1]), gPsCmd[PS_SETDASH]);
   }
   switch (Pen) {
   case SOLIDPAT: fprintf(FP, "      %s\n", gPsCmd[PS_STROKE]); break;
   case BACKPAT:
      if (!TransPat) {
         fprintf(FP, "      1 %s %s 0 %s\n",
               gPsCmd[PS_SETGRAY], gPsCmd[PS_STROKE], gPsCmd[PS_SETGRAY]);
      }
      break;
   default:
      if (colorDump || !useGray) {
         if (preDumpSetup) PSUseColorPattern();
         fprintf(FP, "      %s\n", gPsCmd[PS_FLATTENPATH]);
         DumpPatFill(FP, Pen, ObjPtr->bbox, 6, TRUE);
      } else {
         fprintf(FP, "      %s\n", gPsCmd[PS_STROKE]);
      }
      break;
   }
   fprintf(FP, "   %s\n", gPsCmd[PS_GRESTORE]);
}
 
void DumpOvalObj(FP, ObjPtr)
   FILE *FP;
   struct ObjRec *ObjPtr;
{
   int ltx, lty, rbx, rby, xc, yc, a, b;
   int trans_pat, fill, width, pen, dash, color_index;
   char s[MAXSTRING];
   struct OvalRec *oval_ptr=ObjPtr->detail.o;

   if (ObjPtr->ctm == NULL) {
      ltx = ObjPtr->obbox.ltx; lty = ObjPtr->obbox.lty;
      rbx = ObjPtr->obbox.rbx; rby = ObjPtr->obbox.rby;
   } else {
      ltx = ObjPtr->orig_obbox.ltx; lty = ObjPtr->orig_obbox.lty;
      rbx = ObjPtr->orig_obbox.rbx; rby = ObjPtr->orig_obbox.rby;
   }
   a = (rbx - ltx) / 2; xc = ltx + a;
   b = (rby - lty) / 2; yc = lty + b;

   trans_pat = ObjPtr->trans_pat;
   fill = oval_ptr->fill;
   width = oval_ptr->width;
   pen = oval_ptr->pen;
   dash = oval_ptr->dash;

   if ((fill == NONEPAT || (trans_pat && fill == BACKPAT)) &&
         (pen == NONEPAT || (trans_pat && pen == BACKPAT))) {
      return;
   }
   fprintf(FP, "%% OVAL\n");
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
      fprintf(FP, "   %1d %s %1d %s %s\n", ObjPtr->x, gPsCmd[PS_NEG],
            ObjPtr->y, gPsCmd[PS_NEG], gPsCmd[PS_TRANSLATE]);
   }
   color_index = ObjPtr->color;
   DumpRGBColorLine(FP, color_index, 0, TRUE);

   GetPSEllipseStr(xc, yc, a, b, s);
   switch (fill) {
   case NONEPAT: break;
   case SOLIDPAT:
      /* solid black oval */
      fprintf(FP, "%s %s\n", s, gPsCmd[PS_FILL]);
      break;
   case BACKPAT:
      /* solid white oval */
      if (!trans_pat) {
         fprintf(FP, "%s\n", s);
         fprintf(FP, "%s 1 %s %s\n",
               gPsCmd[PS_CLOSEPATH], gPsCmd[PS_SETGRAY], gPsCmd[PS_FILL]);
         DumpRGBColorLine(FP, color_index, 3, TRUE);
      }
      break;
   default:
      /* patterned */
      fprintf(FP, "%s\n", gPsCmd[PS_GSAVE]);
      if (colorDump || !useGray) {
         if (preDumpSetup) PSUseColorPattern();
         if (!trans_pat) {
            fprintf(FP, "   %s\n", s);
            fprintf(FP, "   %s 1 %s %s\n",
                  gPsCmd[PS_CLOSEPATH], gPsCmd[PS_SETGRAY], gPsCmd[PS_FILL]);
            DumpRGBColorLine(FP, color_index, 3, TRUE);
         }
         fprintf(FP, "   %s\n", s);
         fprintf(FP, "   %s %s %s\n",
               gPsCmd[PS_CLOSEPATH], gPsCmd[PS_EOCLIP], gPsCmd[PS_NEWPATH]);
         DumpPatFill(FP, fill, ObjPtr->bbox, 3, TRUE);
      } else {
         GrayCheck(fill);
         fprintf(FP, "   %s %s\n", GrayStr(fill), gPsCmd[PS_SETGRAY]);
         fprintf(FP, "   %s %s\n", s, gPsCmd[PS_FILL]);
      }
      fprintf(FP, "%s\n", gPsCmd[PS_GRESTORE]);
      break;
   }
   if (pen == NONEPAT) {
      if (ObjPtr->ctm != NULL) fprintf(FP, "%s\n", gPsCmd[PS_GRESTORE]);
      fprintf(FP, "\n");
      return;
   }

   fprintf(FP, "%s\n", gPsCmd[PS_GSAVE]);

   if ((colorDump || !useGray) && pen > BACKPAT) {
      if (!trans_pat) {
         DumpOvalPath(FP, ObjPtr, xc, yc, a, b, width, BACKPAT, 0, trans_pat);
         DumpRGBColorLine(FP, color_index, 3, TRUE);
      }
   }
   DumpOvalPath(FP, ObjPtr, xc, yc, a, b, width, pen, dash, trans_pat);

   fprintf(FP, "%s\n", gPsCmd[PS_GRESTORE]);
   if (ObjPtr->ctm != NULL) fprintf(FP, "%s\n", gPsCmd[PS_GRESTORE]);
   fprintf(FP, "\n");
}

int NeedsToCacheOvalObj(ObjPtr)
   struct ObjRec *ObjPtr;
{
   return (ObjPtr->ctm != NULL);
}

static double ovalXMag1=(double)0.0, ovalYMag1=(double)0.0;
static double ovalXMag2=(double)0.0, ovalYMag2=(double)0.0;

static
void MakeCachedOval(ObjPtr)
   struct ObjRec *ObjPtr;
{
   register int i;
   int sn, cntrln, saved_zoomedin, saved_zoomscale, saved_x, saved_y;
   struct OvalRec *oval_ptr=ObjPtr->detail.o;
   XPoint *sv=NULL;
   DoublePoint *tmp_sv=NULL, *pv=NULL, *cntrlv=NULL;
   struct BBRec obbox;
   double w, h, cx, cy;

   if (ovalXMag1 == (double)0.0) {
      ovalXMag1 = (double)cos((double)(28.6*M_PI/180.0));
      ovalYMag1 = (double)sin((double)(28.6*M_PI/180.0));
      ovalXMag2 = (double)cos((double)(61.4*M_PI/180.0));
      ovalYMag2 = (double)sin((double)(61.4*M_PI/180.0));
   }
   if (ObjPtr->ctm == NULL) return;

   if (oval_ptr->rotated_vlist != NULL) free(oval_ptr->rotated_vlist);
   oval_ptr->rotated_n = 0;
   oval_ptr->rotated_vlist = NULL;
   tmp_sv = (DoublePoint*)malloc((13+1)*sizeof(DoublePoint));
   pv = (DoublePoint*)malloc((13+1)*sizeof(DoublePoint));
   if (tmp_sv == NULL || pv == NULL) FailAllocMessage();

   obbox.ltx = ObjPtr->orig_obbox.ltx - ObjPtr->x;
   obbox.lty = ObjPtr->orig_obbox.lty - ObjPtr->y;
   obbox.rbx = ObjPtr->orig_obbox.rbx - ObjPtr->x;
   obbox.rby = ObjPtr->orig_obbox.rby - ObjPtr->y;
   cx = (((double)(obbox.ltx+obbox.rbx))/2.0);
   cy = (((double)(obbox.lty+obbox.rby))/2.0);
   w = (((double)(obbox.rbx-obbox.ltx))/2.0);
   h = (((double)(obbox.rby-obbox.lty))/2.0);

   tmp_sv[0].x  = (double)obbox.rbx;          tmp_sv[0].y  = (double)cy;
   tmp_sv[1].x  = ((double)cx)+(w*ovalXMag1); tmp_sv[1].y  = ((double)cy)-(h*ovalYMag1);
   tmp_sv[2].x  = ((double)cx)+(w*ovalXMag2); tmp_sv[2].y  = ((double)cy)-(h*ovalYMag2);
   tmp_sv[3].x  = (double)cx;                 tmp_sv[3].y  = (double)obbox.lty;
   tmp_sv[4].x  = ((double)cx)-(w*ovalXMag2); tmp_sv[4].y  = ((double)cy)-(h*ovalYMag2);
   tmp_sv[5].x  = ((double)cx)-(w*ovalXMag1); tmp_sv[5].y  = ((double)cy)-(h*ovalYMag1);
   tmp_sv[6].x  = (double)obbox.ltx;          tmp_sv[6].y  = (double)cy;
   tmp_sv[7].x  = ((double)cx)-(w*ovalXMag1); tmp_sv[7].y  = ((double)cy)+(h*ovalYMag1);
   tmp_sv[8].x  = ((double)cx)-(w*ovalXMag2); tmp_sv[8].y  = ((double)cy)+(h*ovalYMag2);
   tmp_sv[9].x  = (double)cx;                 tmp_sv[9].y  = (double)obbox.rby;
   tmp_sv[10].x = ((double)cx)+(w*ovalXMag2); tmp_sv[10].y = ((double)cy)+(h*ovalYMag2);
   tmp_sv[11].x = ((double)cx)+(w*ovalXMag1); tmp_sv[11].y = ((double)cy)+(h*ovalYMag1);
   tmp_sv[12].x = tmp_sv[0].x;                tmp_sv[12].y = tmp_sv[0].y;
   for (i=0; i < 13; i++) {
      double x, y;

      TransformDoublePointThroughCTM(tmp_sv[i].x, tmp_sv[i].y, ObjPtr->ctm,
            &x, &y);
      tmp_sv[i].x = pv[i].x = OFFSET_DOUBLE_X(x + (double)ObjPtr->x);
      tmp_sv[i].y = pv[i].y = OFFSET_DOUBLE_Y(y + (double)ObjPtr->y);
   }
   saved_zoomedin = zoomedIn;
   saved_zoomscale = zoomScale;
   saved_x = drawOrigX;
   saved_y = drawOrigY;
   zoomedIn = FALSE;
   zoomScale = 0;
   drawOrigX = drawOrigY = 0;
   sv = MakeDoubleIntSplinePolygonVertex(&sn, &cntrln, &cntrlv,
         drawOrigX, drawOrigY, 13, pv);
   zoomedIn = saved_zoomedin;
   zoomScale = saved_zoomscale;
   drawOrigX = saved_x;
   drawOrigY = saved_y;
   free(pv);
   if (sv != NULL) {
      free(tmp_sv);
      oval_ptr->rotated_n = sn;
      oval_ptr->rotated_vlist = sv;
   } else {
      oval_ptr->rotated_n = 13;
   }

   if (cntrlv != NULL) free(cntrlv);
}
 
void DrawOvalObj(window, XOff, YOff, ObjPtr)
   Window window;
   int XOff, YOff;
   struct ObjRec *ObjPtr;
{
   int trans_pat, fill, width, pen, dash, pixel, real_x_off, real_y_off;
   struct BBRec bbox;
   XGCValues values;
   struct OvalRec *oval_ptr=ObjPtr->detail.o;

   real_x_off = (zoomedIn ? XOff : (XOff>>zoomScale)<<zoomScale);
   real_y_off = (zoomedIn ? YOff : (YOff>>zoomScale)<<zoomScale);
   bbox.ltx = ZOOMED_SIZE(ObjPtr->obbox.ltx - real_x_off);
   bbox.lty = ZOOMED_SIZE(ObjPtr->obbox.lty - real_y_off);
   bbox.rbx = ZOOMED_SIZE(ObjPtr->obbox.rbx - real_x_off);
   bbox.rby = ZOOMED_SIZE(ObjPtr->obbox.rby - real_y_off);

   trans_pat = ObjPtr->trans_pat;
   fill = oval_ptr->fill;
   width = oval_ptr->width;
   pen = oval_ptr->pen;
   dash = oval_ptr->dash;
   pixel = colorPixels[ObjPtr->color];

   if (NeedsToCacheOvalObj(ObjPtr) && oval_ptr->rotated_vlist==NULL) {
      MakeCachedOval(ObjPtr);
   }
   if (userDisableRedraw) return;

   if (fill != NONEPAT) {
      values.foreground = GetDrawingBgPixel(fill, pixel);
      values.function = GXcopy;
      values.fill_style = (trans_pat ? FillStippled : FillOpaqueStippled);
      values.stipple = patPixmap[fill];
      XChangeGC(mainDisplay, drawGC,
            GCForeground | GCFunction | GCFillStyle | GCStipple, &values);
      if (ObjPtr->ctm != NULL) {
         XFillPolygon(mainDisplay, window, drawGC,
               oval_ptr->rotated_vlist, oval_ptr->rotated_n,
               Convex, CoordModeOrigin);
      } else {
         XFillArc(mainDisplay, window, drawGC, bbox.ltx, bbox.lty,
               bbox.rbx-bbox.ltx, bbox.rby-bbox.lty, 0, (360<<6));
      }
   }
   if (pen != NONEPAT) {
      values.foreground = GetDrawingBgPixel(pen, pixel);
      values.function = GXcopy;
      values.fill_style = (trans_pat ? FillStippled : FillOpaqueStippled);
      values.stipple = patPixmap[pen];
      values.line_width = ZOOMED_SIZE(width);
#ifdef NO_THIN_LINE
      if (values.line_width < 1) values.line_width = 1;
#else
#ifdef THIN_OVAL_AND_ARC
      if (values.line_width <= 1) values.line_width = 0;
#endif
#endif
      if (dash != 0) {
         XSetDashes(mainDisplay, drawGC, 0, dashList[dash],
               dashListLength[dash]);
         values.line_style = LineOnOffDash;
      } else {
         values.line_style = LineSolid;
      }
      XChangeGC(mainDisplay, drawGC,
            GCForeground | GCFunction | GCFillStyle | GCStipple | GCLineWidth |
            GCLineStyle, &values);
      if (ObjPtr->ctm != NULL) {
         XDrawLines(mainDisplay, window, drawGC,
               oval_ptr->rotated_vlist, oval_ptr->rotated_n,
               CoordModeOrigin);
      } else {
         XDrawArc(mainDisplay, window, drawGC, bbox.ltx, bbox.lty,
               bbox.rbx-bbox.ltx, bbox.rby-bbox.lty, 0, (360<<6));
      }
   }
}

void CreateOvalObj(BBox, CreateAbsolute)
   struct BBRec *BBox;
   int CreateAbsolute;
{
   struct OvalRec *oval_ptr;
   struct ObjRec *obj_ptr;
   int width, w;

   oval_ptr = (struct OvalRec *)malloc(sizeof(struct OvalRec));
   if (oval_ptr == NULL) FailAllocMessage();
   memset(oval_ptr, 0, sizeof(struct OvalRec));
   oval_ptr->fill = objFill;
   oval_ptr->width = width = curWidthOfLine[lineWidth];
   UtilStrCpyN(oval_ptr->width_spec, sizeof(oval_ptr->width_spec),
         curWidthOfLineSpec[lineWidth]);
   oval_ptr->pen = penPat;
   oval_ptr->dash = curDash;
   oval_ptr->rotated_n = 0;
   oval_ptr->rotated_vlist = NULL;

   obj_ptr = (struct ObjRec *)malloc(sizeof(struct ObjRec));
   if (obj_ptr == NULL) FailAllocMessage();
   memset(obj_ptr, 0, sizeof(struct ObjRec));

   if (CreateAbsolute) {
      obj_ptr->bbox.ltx = obj_ptr->obbox.ltx = obj_ptr->x = BBox->ltx;
      obj_ptr->bbox.lty = obj_ptr->obbox.lty = obj_ptr->y = BBox->lty;
      obj_ptr->bbox.rbx = obj_ptr->obbox.rbx = BBox->rbx;
      obj_ptr->bbox.rby = obj_ptr->obbox.rby = BBox->rby;
   } else {
      obj_ptr->bbox.ltx = obj_ptr->obbox.ltx = obj_ptr->x = ABS_X(BBox->ltx);
      obj_ptr->bbox.lty = obj_ptr->obbox.lty = obj_ptr->y = ABS_Y(BBox->lty);
      obj_ptr->bbox.rbx = obj_ptr->obbox.rbx = ABS_X(BBox->rbx);
      obj_ptr->bbox.rby = obj_ptr->obbox.rby = ABS_Y(BBox->rby);
   }
   w = HALF_W(width);
   obj_ptr->bbox.ltx -= w;
   obj_ptr->bbox.lty -= w;
   obj_ptr->bbox.rbx += w;
   obj_ptr->bbox.rby += w;
   obj_ptr->type = OBJ_OVAL;
   obj_ptr->color = colorIndex;
   if (mainDisplay != NULL) {
      UtilStrCpyN(obj_ptr->color_str, sizeof(obj_ptr->color_str),
            colorMenuItems[colorIndex]);
   }
   obj_ptr->id = objId++;
   obj_ptr->dirty = FALSE;
   obj_ptr->rotation = 0;
   obj_ptr->locked = FALSE;
   obj_ptr->detail.o = oval_ptr;
   obj_ptr->fattr = obj_ptr->lattr = NULL;
   obj_ptr->ctm = NULL;
   obj_ptr->invisible = FALSE;
   obj_ptr->trans_pat = transPat;

   AddObj(NULL, topObj, obj_ptr);
}

static
void AdjContinueOvalBBox(pBBoxIn, pBBoxOut)
   struct BBRec *pBBoxIn, *pBBoxOut;
{
   int w, h, cx, cy;
   double dx, dy, r;

   switch (curChoice) {
   case DRAWCORNEROVAL:
      memcpy(pBBoxOut, pBBoxIn, sizeof(struct BBRec));
      break;
   case DRAWCENTEROVAL:
      w = pBBoxIn->rbx - pBBoxIn->ltx;
      h = pBBoxIn->rby - pBBoxIn->lty;
      pBBoxOut->ltx = pBBoxIn->rbx - (w<<1);
      pBBoxOut->lty = pBBoxIn->rby - (h<<1);
      pBBoxOut->rbx = pBBoxIn->rbx;
      pBBoxOut->rby = pBBoxIn->rby;
      break;
   case DRAWEDGECIRCLE:
      cx = ((pBBoxIn->rbx + pBBoxIn->ltx) >> 1);
      cy = ((pBBoxIn->rby + pBBoxIn->lty) >> 1);
      dx = (double)(cx - pBBoxIn->ltx);
      dy = (double)(cy - pBBoxIn->lty);
      r = (double)sqrt(dx*dx+dy*dy);
      w = round(r);
      pBBoxOut->ltx = cx-w;
      pBBoxOut->lty = cy-w;
      pBBoxOut->rbx = cx+w;
      pBBoxOut->rby = cy+w;
      break;
   }
}

static
void MyContinueOval(d, gc, pBBox)
   Drawable d;
   GC gc;
   struct BBRec *pBBox;
{
   struct BBRec bbox;

   AdjContinueOvalBBox(pBBox, &bbox);
   MyOval(d, gc, bbox);
}

static
void ContinueOval(OrigX, OrigY)
   int OrigX, OrigY;
{
   int end_x, end_y, grid_x, grid_y, done=FALSE, abort=FALSE;
   char buf[80], w_buf[80], h_buf[80], x_buf[80], y_buf[80];
   struct BBRec bbox;
   XEvent input, ev;
   XMotionEvent *motion_ev=NULL;

   bbox.ltx = bbox.rbx = OrigX;
   bbox.lty = bbox.rby = OrigY;

   SetXorDrawGC(xorColorPixels[colorIndex]);

   grid_x = end_x = OrigX;
   grid_y = end_y = OrigY; 
   PixelToMeasurementUnit(w_buf, 0);
   PixelToMeasurementUnit(h_buf, 0);
   PixelToMeasurementUnit(x_buf, ABS_X(grid_x));
   PixelToMeasurementUnit(y_buf, ABS_Y(grid_y));
   sprintf(buf, "w=%s\nh=%s\nx=%s\ny=%s", w_buf, h_buf, x_buf, y_buf);
   StartShowMeasureCursor(grid_x, grid_y, buf, TRUE);
   BeginIntervalRulers(grid_x, grid_y, grid_x, grid_y);
   if (!debugNoPointerGrab) {
      XGrabPointer(mainDisplay, drawWindow, FALSE,
            PointerMotionMask | ButtonReleaseMask,
            GrabModeAsync, GrabModeAsync, None, handCursor, CurrentTime);
   }
   while (!done) {
      XNextEvent(mainDisplay, &input);

      if (input.type == Expose || input.type == VisibilityNotify) {
         ExposeEventHandler(&input, TRUE);
         SetXorDrawGC(xorColorPixels[colorIndex]);
      } else if (input.type == ButtonRelease) {
         XUngrabPointer(mainDisplay, CurrentTime);
         MyContinueOval(drawWindow, drawGC, &bbox);
         EndIntervalRulers(grid_x, grid_y);
         PixelToMeasurementUnit(w_buf, ABS_SIZE(abs(bbox.rbx-OrigX)));
         PixelToMeasurementUnit(h_buf, ABS_SIZE(abs(bbox.rby-OrigY)));
         PixelToMeasurementUnit(x_buf, ABS_X(bbox.rbx));
         PixelToMeasurementUnit(y_buf, ABS_Y(bbox.rby));
         sprintf(buf, "w=%s\nh=%s\nx=%s\ny=%s", w_buf, h_buf, x_buf, y_buf);
         EndShowMeasureCursor(bbox.rbx, bbox.rby, buf, TRUE);
         done = TRUE;
      } else if (input.type == MotionNotify || input.type == KeyPress ||
            input.type == KeyRelease) {
         if (input.type == KeyPress) {
            if (KeyPressEventIsEscape(&input.xkey)) {
               XUngrabPointer(mainDisplay, CurrentTime);
               MyContinueOval(drawWindow, drawGC, &bbox);
               EndIntervalRulers(grid_x, grid_y);
               PixelToMeasurementUnit(w_buf, ABS_SIZE(abs(bbox.rbx-OrigX)));
               PixelToMeasurementUnit(h_buf, ABS_SIZE(abs(bbox.rby-OrigY)));
               PixelToMeasurementUnit(x_buf, ABS_X(bbox.rbx));
               PixelToMeasurementUnit(y_buf, ABS_Y(bbox.rby));
               sprintf(buf, "w=%s\nh=%s\nx=%s\ny=%s",
                     w_buf, h_buf, x_buf, y_buf);
               EndShowMeasureCursor(bbox.rbx, bbox.rby, buf, TRUE);
               abort = TRUE;
               done = TRUE;
            }
         }
         if (done) {
            break;
         }
         motion_ev = (&(input.xmotion));
         if (input.type == KeyPress || input.type == KeyRelease) {
            end_x = grid_x;
            end_y = grid_y;
         } else {
            end_x = motion_ev->x;
            end_y = motion_ev->y;
         }
         if (curChoice == DRAWEDGECIRCLE &&
               shiftForDiagMouseMove && DiagEventCheck(&input)) {
            if (input.type == KeyRelease) {
               end_x = input.xkey.x;
               end_y = input.xkey.y;
            } else {
               DiagGridXY(OrigX, OrigY, &end_x, &end_y);
            }
         }
         GridXY(end_x, end_y, &grid_x, &grid_y);
	 if (input.type == MotionNotify && curChoice != DRAWEDGECIRCLE &&
               (motion_ev->state & (ShiftMask | ControlMask)) != 0) {
            int w, h, pos_w=TRUE, pos_h=TRUE;

            w = grid_x - bbox.ltx;
            h = grid_y - bbox.lty;
            if (w < 0) {
               w = (-w);
               pos_w = FALSE;
            }
            if (h < 0) {
               h = (-h);
               pos_h = FALSE;
            }
            if (w > h) {
               grid_x = (pos_w ? (bbox.ltx+h) : (bbox.ltx-h));
            } else {
               grid_y = (pos_h ? (bbox.lty+w) : (bbox.lty-w));
            }
         }
         if (grid_x != bbox.rbx || grid_y != bbox.rby) {
            PixelToMeasurementUnit(w_buf, ABS_SIZE(abs(bbox.rbx-OrigX)));
            PixelToMeasurementUnit(h_buf, ABS_SIZE(abs(bbox.rby-OrigY)));
            PixelToMeasurementUnit(x_buf, ABS_X(bbox.rbx));
            PixelToMeasurementUnit(y_buf, ABS_Y(bbox.rby));
            sprintf(buf, "w=%s\nh=%s\nx=%s\ny=%s", w_buf, h_buf, x_buf, y_buf);
            ShowMeasureCursor(bbox.rbx, bbox.rby, buf, TRUE);
            MyContinueOval(drawWindow, drawGC, &bbox);
            bbox.rbx = grid_x;
            bbox.rby = grid_y;
            MyContinueOval(drawWindow, drawGC, &bbox);
            PixelToMeasurementUnit(w_buf, ABS_SIZE(abs(bbox.rbx-OrigX)));
            PixelToMeasurementUnit(h_buf, ABS_SIZE(abs(bbox.rby-OrigY)));
            PixelToMeasurementUnit(x_buf, ABS_X(bbox.rbx));
            PixelToMeasurementUnit(y_buf, ABS_Y(bbox.rby));
            sprintf(buf, "w=%s\nh=%s\nx=%s\ny=%s", w_buf, h_buf, x_buf, y_buf);
            ShowMeasureCursor(bbox.rbx, bbox.rby, buf, TRUE);
         }
         DrawIntervalRulers(OrigX, OrigY, grid_x, grid_y, NULL);
         while (XCheckMaskEvent(mainDisplay, PointerMotionMask, &ev)) ;
      }
   }
   if (!abort &&
         ((curChoice!=DRAWEDGECIRCLE && OrigX!=grid_x && OrigY!=grid_y) ||
         (curChoice==DRAWEDGECIRCLE && !(OrigX==grid_x && OrigY==grid_y)))) {
      struct BBRec final_bbox;

      AdjContinueOvalBBox(&bbox, &final_bbox);
      if (final_bbox.ltx > final_bbox.rbx) {
         /* swap final_bbox.ltx & final_bbox.rbx */
         end_x = final_bbox.ltx;
         final_bbox.ltx = final_bbox.rbx;
         final_bbox.rbx = end_x;
      }
      if (final_bbox.lty > final_bbox.rby) {
         /* swap final_bbox.lty & final_bbox.rby */
         end_y = final_bbox.lty;
         final_bbox.lty = final_bbox.rby;
         final_bbox.rby = end_y;
      }
      CreateOvalObj(&final_bbox, FALSE);
      RecordNewObjCmd();
      DrawOvalObj(drawWindow, drawOrigX, drawOrigY, topObj);
      ovalDrawn = TRUE;
      SetFileModified(TRUE);
   }
   XSync(mainDisplay, False);
}

void DrawOval(input)
   XEvent *input;
{
   XButtonEvent *button_ev;
   int mouse_x, mouse_y, grid_x, grid_y;

   if (input->type != ButtonPress) return;

   button_ev = &(input->xbutton);
   if (button_ev->button == Button1) {
      mouse_x = button_ev->x;
      mouse_y = button_ev->y;
      GridXY(mouse_x, mouse_y, &grid_x, &grid_y);
      ContinueOval(grid_x, grid_y);
   }
}

void MakeOvalObjFromBoundingBox()
{
   struct BBRec obbox;

   if (topSel == NULL) {
      MsgBox(TgLoadCachedString(CSTID_NO_OBJ_SELECTED), TOOL_NAME, INFO_MB);
      return;
   }
   if (curChoice == VERTEXMODE) SetCurChoice(NOTHING);

   HighLightReverse();
   obbox.ltx = selObjLtX;
   obbox.lty = selObjLtY;
   obbox.rbx = selObjRbX;
   obbox.rby = selObjRbY;
   CreateOvalObj(&obbox, TRUE);
   SelectTopObj();
   RecordNewObjCmd();
   RedrawAnArea(botObj, selLtX-GRID_ABS_SIZE(1), selLtY-GRID_ABS_SIZE(1),
         selRbX+GRID_ABS_SIZE(1), selRbY+GRID_ABS_SIZE(1));
   HighLightForward();
   justDupped = FALSE;
   SetFileModified(TRUE);
}

void SaveOvalObj(FP, ObjPtr)
   FILE *FP;
   struct ObjRec *ObjPtr;
{
   struct OvalRec *oval_ptr=ObjPtr->detail.o;

   if (fprintf(FP, "oval('%s','',", colorMenuItems[ObjPtr->color]) == EOF) {
      writeFileFailed = TRUE;
   }
   if (fprintf(FP,
         "%1d,%1d,%1d,%1d,%1d,%1d,%1d,%1d,%1d,%1d,%1d,%1d,%1d,'%s',%1d,",
         ObjPtr->obbox.ltx, ObjPtr->obbox.lty, ObjPtr->obbox.rbx,
         ObjPtr->obbox.rby, oval_ptr->fill, oval_ptr->width,
         oval_ptr->pen, ObjPtr->id, oval_ptr->dash,
         ObjPtr->rotation, ObjPtr->locked, ObjPtr->ctm!=NULL,
         ObjPtr->invisible, oval_ptr->width_spec, ObjPtr->trans_pat) == EOF) {
      writeFileFailed = TRUE;
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

void ReadOvalObj(FP, Inbuf, ObjPtr)
   FILE *FP;
   char *Inbuf;
   struct ObjRec **ObjPtr;
{
   struct OvalRec *oval_ptr;
   char color_str[40], bg_color_str[40], *s, width_spec[40];
   int ltx, lty, rbx, rby, trans_pat=FALSE, fill, width, pen, dash, w, id=0;
   int new_alloc, rotation, locked=FALSE, transformed=FALSE, invisible=FALSE;

   *ObjPtr = NULL;

   s = FindChar((int)'(', Inbuf);
   s = ParseStr(s, (int)',', color_str, sizeof(color_str));
   if (fileVersion >= 37) {
      s = ParseStr(s, (int)',', bg_color_str, sizeof(bg_color_str));
   }
   InitScan(s, "\n\t, ");

   dash = 0;
   rotation = 0;
   *width_spec = '\0';
   if (fileVersion <= 5) {
      if (GETINT("oval", ltx,      "ltx") == INVALID ||
          GETINT("oval", lty,      "lty") == INVALID ||
          GETINT("oval", rbx,      "rbx") == INVALID ||
          GETINT("oval", rby,      "rby") == INVALID ||
          GETINT("oval", fill,     "fill") == INVALID ||
          GETINT("oval", width,    "width") == INVALID ||
          GETINT("oval", pen,      "pen") == INVALID) {
         return;
      }
      switch (width) {
      case 1: width = 3; break;
      case 2: width = 6; break;
      }
      id = objId++;
   } else if (fileVersion <= 7) {
      if (GETINT("oval", ltx,      "ltx") == INVALID ||
          GETINT("oval", lty,      "lty") == INVALID ||
          GETINT("oval", rbx,      "rbx") == INVALID ||
          GETINT("oval", rby,      "rby") == INVALID ||
          GETINT("oval", fill,     "fill") == INVALID ||
          GETINT("oval", width,    "width") == INVALID ||
          GETINT("oval", pen,      "pen") == INVALID) {
         return;
      }
      id = objId++;
   } else if (fileVersion <= 8) {
      if (GETINT("oval", ltx,      "ltx") == INVALID ||
          GETINT("oval", lty,      "lty") == INVALID ||
          GETINT("oval", rbx,      "rbx") == INVALID ||
          GETINT("oval", rby,      "rby") == INVALID ||
          GETINT("oval", fill,     "fill") == INVALID ||
          GETINT("oval", width,    "width") == INVALID ||
          GETINT("oval", pen,      "pen") == INVALID ||
          GETINT("oval", id,       "id") == INVALID) {
         return;
      }
      if (id >= objId) objId = id+1;
   } else if (fileVersion <= 13) {
      if (GETINT("oval", ltx,      "ltx") == INVALID ||
          GETINT("oval", lty,      "lty") == INVALID ||
          GETINT("oval", rbx,      "rbx") == INVALID ||
          GETINT("oval", rby,      "rby") == INVALID ||
          GETINT("oval", fill,     "fill") == INVALID ||
          GETINT("oval", width,    "width") == INVALID ||
          GETINT("oval", pen,      "pen") == INVALID ||
          GETINT("oval", id,       "id") == INVALID ||
          GETINT("oval", dash,     "dash") == INVALID) {
         return;
      }
      if (id >= objId) objId = id+1;
   } else if (fileVersion <= 25) {
      if (GETINT("oval", ltx,      "ltx") == INVALID ||
          GETINT("oval", lty,      "lty") == INVALID ||
          GETINT("oval", rbx,      "rbx") == INVALID ||
          GETINT("oval", rby,      "rby") == INVALID ||
          GETINT("oval", fill,     "fill") == INVALID ||
          GETINT("oval", width,    "width") == INVALID ||
          GETINT("oval", pen,      "pen") == INVALID ||
          GETINT("oval", id,       "id") == INVALID ||
          GETINT("oval", dash,     "dash") == INVALID ||
          GETINT("oval", rotation, "rotation") == INVALID) {
         return;
      }
      if (id >= objId) objId = id+1;
   } else if (fileVersion <= 32) {
      if (GETINT("oval", ltx,      "ltx") == INVALID ||
          GETINT("oval", lty,      "lty") == INVALID ||
          GETINT("oval", rbx,      "rbx") == INVALID ||
          GETINT("oval", rby,      "rby") == INVALID ||
          GETINT("oval", fill,     "fill") == INVALID ||
          GETINT("oval", width,    "width") == INVALID ||
          GETINT("oval", pen,      "pen") == INVALID ||
          GETINT("oval", id,       "id") == INVALID ||
          GETINT("oval", dash,     "dash") == INVALID ||
          GETINT("oval", rotation, "rotation") == INVALID ||
          GETINT("oval", locked,   "locked") == INVALID) {
         return;
      }
      if (id >= objId) objId = id+1;
   } else if (fileVersion <= 34) {
      if (GETINT("oval", ltx,         "ltx") == INVALID ||
          GETINT("oval", lty,         "lty") == INVALID ||
          GETINT("oval", rbx,         "rbx") == INVALID ||
          GETINT("oval", rby,         "rby") == INVALID ||
          GETINT("oval", fill,        "fill") == INVALID ||
          GETINT("oval", width,       "width") == INVALID ||
          GETINT("oval", pen,         "pen") == INVALID ||
          GETINT("oval", id,          "id") == INVALID ||
          GETINT("oval", dash,        "dash") == INVALID ||
          GETINT("oval", rotation,    "rotation") == INVALID ||
          GETINT("oval", locked,      "locked") == INVALID ||
          GETINT("oval", transformed, "transformed") == INVALID ||
          GETINT("oval", invisible,   "invisible") == INVALID ||
          GETSTR("oval", width_spec,  "width_spec") == INVALID) {
         return;
      }
      if (id >= objId) objId = id+1;
      UtilRemoveQuotes(width_spec);
   } else {
      if (GETINT("oval", ltx,         "ltx") == INVALID ||
          GETINT("oval", lty,         "lty") == INVALID ||
          GETINT("oval", rbx,         "rbx") == INVALID ||
          GETINT("oval", rby,         "rby") == INVALID ||
          GETINT("oval", fill,        "fill") == INVALID ||
          GETINT("oval", width,       "width") == INVALID ||
          GETINT("oval", pen,         "pen") == INVALID ||
          GETINT("oval", id,          "id") == INVALID ||
          GETINT("oval", dash,        "dash") == INVALID ||
          GETINT("oval", rotation,    "rotation") == INVALID ||
          GETINT("oval", locked,      "locked") == INVALID ||
          GETINT("oval", transformed, "transformed") == INVALID ||
          GETINT("oval", invisible,   "invisible") == INVALID ||
          GETSTR("oval", width_spec,  "width_spec") == INVALID ||
          GETINT("oval", trans_pat,   "trans_pat") == INVALID) {
         return;
      }
      if (id >= objId) objId = id+1;
      UtilRemoveQuotes(width_spec);
   }

   if (ltx > rbx || lty > rby) {
      int tmp_ltx, tmp_lty, tmp_rbx, tmp_rby;

      if (!PRTGIF) Msg(TgLoadCachedString(CSTID_BAD_OVAL_BBOX_ADJ));
      CalcBBox(ltx, lty, rbx, rby, &tmp_ltx, &tmp_lty, &tmp_rbx, &tmp_rby);
      ltx = tmp_ltx; lty = tmp_lty; rbx = tmp_rbx; rby = tmp_rby;
   }

   if (fileVersion <= 16 && width <= 6) width = origWidthOfLine[width];
   if (fileVersion <= 32) {
      sprintf(width_spec, "%1d", width);
   }
   fill = UpgradePenFill(fill);
   pen = UpgradePenFill(pen);

   *ObjPtr = (struct ObjRec *)malloc(sizeof(struct ObjRec));
   if (*ObjPtr == NULL) FailAllocMessage();
   memset(*ObjPtr, 0, sizeof(struct ObjRec));
   oval_ptr = (struct OvalRec *)malloc(sizeof(struct OvalRec));
   if (oval_ptr == NULL) FailAllocMessage();
   memset(oval_ptr, 0, sizeof(struct OvalRec));

   oval_ptr->fill = fill;
   oval_ptr->width = width;
   UtilStrCpyN(oval_ptr->width_spec, sizeof(oval_ptr->width_spec), width_spec);
   oval_ptr->pen = pen;
   oval_ptr->dash = dash;

   oval_ptr->rotated_n = 0;
   oval_ptr->rotated_vlist = NULL;

   (*ObjPtr)->x = ltx;
   (*ObjPtr)->y = lty;
   (*ObjPtr)->color = QuickFindColorIndex(*ObjPtr, color_str, &new_alloc, TRUE);
   UtilStrCpyN((*ObjPtr)->color_str, sizeof((*ObjPtr)->color_str), color_str);
   (*ObjPtr)->dirty = FALSE;
   (*ObjPtr)->id = id;
   (*ObjPtr)->rotation = rotation;
   (*ObjPtr)->locked = locked;
   (*ObjPtr)->type = OBJ_OVAL;
   (*ObjPtr)->obbox.ltx = ltx;
   (*ObjPtr)->obbox.lty = lty;
   (*ObjPtr)->obbox.rbx = rbx;
   (*ObjPtr)->obbox.rby = rby;
   w = HALF_W(width);
   (*ObjPtr)->bbox.ltx = ltx - w;
   (*ObjPtr)->bbox.lty = lty - w;
   (*ObjPtr)->bbox.rbx = rbx + w;
   (*ObjPtr)->bbox.rby = rby + w;
   (*ObjPtr)->detail.o = oval_ptr;
   (*ObjPtr)->ctm = NULL;
   (*ObjPtr)->invisible = invisible;
   (*ObjPtr)->trans_pat = trans_pat;

   if (fileVersion >= 33 && transformed) {
      int real_x=0, real_y=0;
      struct BBRec orig_obbox;
      char inbuf[MAXSTRING+1];
      struct XfrmMtrxRec *ctm;

      (void)fgets(inbuf, MAXSTRING, FP);
      scanLineNum++;
      InitScan(inbuf, "\t\n, ");

      ctm = (struct XfrmMtrxRec *)malloc(sizeof(struct XfrmMtrxRec));
      if (ctm == NULL) FailAllocMessage();
      if (GETINT("oval", real_x,           "real_x") == INVALID ||
          GETINT("oval", real_y,           "real_y") == INVALID ||
          GETINT("oval", orig_obbox.ltx,   "orig_obbox.ltx") == INVALID ||
          GETINT("oval", orig_obbox.lty,   "orig_obbox.lty") == INVALID ||
          GETINT("oval", orig_obbox.rbx,   "orig_obbox.rbx") == INVALID ||
          GETINT("oval", orig_obbox.rby,   "orig_obbox.rby") == INVALID ||
          GETDBL("oval", ctm->m[CTM_SX],   "CTM_SX") == INVALID ||
          GETDBL("oval", ctm->m[CTM_SIN],  "CTM_SIN") == INVALID ||
          GETDBL("oval", ctm->m[CTM_MSIN], "CTM_MSIN") == INVALID ||
          GETDBL("oval", ctm->m[CTM_SY],   "CTM_SY") == INVALID ||
          GETINT("oval", ctm->t[CTM_TX],   "CTM_TX") == INVALID ||
          GETINT("oval", ctm->t[CTM_TY],   "CTM_TY") == INVALID) {
         return;
      }
      (*ObjPtr)->ctm = ctm;
      if (ctm != NULL) {
         memcpy(&(*ObjPtr)->orig_obbox, &orig_obbox, sizeof(struct BBRec));
         (*ObjPtr)->x = real_x;
         (*ObjPtr)->y = real_y;
         GetTransformedOBBoxOffsetVs(*ObjPtr, (*ObjPtr)->rotated_obbox);
      }
   }
}

void SetOvalPropMask(ObjPtr, plMask, plSkip, pProp)
   struct ObjRec *ObjPtr;
   long *plMask, *plSkip;
   struct PropertiesRec *pProp;
{
   struct OvalRec *oval_ptr=ObjPtr->detail.o;

   SetCTMPropertyMask(ObjPtr->ctm, plMask, plSkip, pProp);

   SetIntPropertyMask(PROP_MASK_COLOR, ObjPtr->color,
         colorMenuItems[ObjPtr->color], plMask, plSkip, pProp);
   SetIntPropertyMask(PROP_MASK_WIDTH, oval_ptr->width, oval_ptr->width_spec,
         plMask, plSkip, pProp);

   SetIntPropertyMask(PROP_MASK_TRANSPAT, ObjPtr->trans_pat, NULL,
         plMask, plSkip, pProp);
   SetIntPropertyMask(PROP_MASK_FILL, oval_ptr->fill, NULL,
         plMask, plSkip, pProp);
   SetIntPropertyMask(PROP_MASK_PEN, oval_ptr->pen, NULL,
         plMask, plSkip, pProp);
   SetIntPropertyMask(PROP_MASK_DASH, oval_ptr->dash, NULL,
         plMask, plSkip, pProp);
}

void FreeOvalObj(ObjPtr)
   struct ObjRec *ObjPtr;
{
   if (ObjPtr->detail.o->rotated_vlist != NULL) {
      free(ObjPtr->detail.o->rotated_vlist);
   }
   free(ObjPtr->detail.o);
   free(ObjPtr);
}
