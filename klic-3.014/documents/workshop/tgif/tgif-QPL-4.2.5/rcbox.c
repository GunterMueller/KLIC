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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/rcbox.c,v 1.12 2011/06/09 16:11:42 cvsps Exp $
 */

#define _INCLUDE_FROM_RCBOX_C_

#include "tgifdefs.h"

#include "attr.e"
#include "auxtext.e"
#include "box.e"
#include "cmd.e"
#include "color.e"
#include "choice.e"
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
#include "pattern.e"
#include "poly.e"
#include "ps.e"
#include "raster.e"
#include "rect.e"
#include "rcbox.e"
#include "ruler.e"
#include "select.e"
#include "setup.e"
#include "spline.e"
#include "strtbl.e"
#include "util.e"
#include "xpixmap.e"

int rcBoxDrawn=FALSE;

static XSegment rcbSegs[4];
static XArc rcbArcs[4];
static int rcbArcsInitialized=FALSE;

void SetRCBoxVertex(x1, y1, x2, y2, r)
   int x1, y1, x2, y2, r;
{
   register int inc_x, inc_y, d=2*r;

   inc_x = (x2 > x1);
   inc_y = (y2 > y1);
   rcbSegs[0].x1 = (inc_x) ? (x1+r) : (x1-r); rcbSegs[0].y1 = y1;
   rcbSegs[0].x2 = (inc_x) ? (x2-r) : (x2+r); rcbSegs[0].y2 = y1;
   rcbSegs[1].x1 = x2; rcbSegs[1].y1 = (inc_y) ? (y1+r) : (y1-r);
   rcbSegs[1].x2 = x2; rcbSegs[1].y2 = (inc_y) ? (y2-r) : (y2+r);
   rcbSegs[2].x1 = (inc_x) ? (x1+r) : (x1-r); rcbSegs[2].y1 = y2;
   rcbSegs[2].x2 = (inc_x) ? (x2-r) : (x2+r); rcbSegs[2].y2 = y2;
   rcbSegs[3].x1 = x1; rcbSegs[3].y1 = (inc_y) ? (y1+r) : (y1-r);
   rcbSegs[3].x2 = x1; rcbSegs[3].y2 = (inc_y) ? (y2-r) : (y2+r);

   if (!rcbArcsInitialized) {
      rcbArcsInitialized = TRUE;

      rcbArcs[0].angle1 = 90*64;   rcbArcs[0].angle2 = 90*64;
      rcbArcs[1].angle1 = 0;       rcbArcs[1].angle2 = 90*64;
      rcbArcs[2].angle1 = -90*64;  rcbArcs[2].angle2 = 90*64;
      rcbArcs[3].angle1 = -180*64; rcbArcs[3].angle2 = 90*64;
   }
   rcbArcs[0].width=rcbArcs[1].width=rcbArcs[2].width=rcbArcs[3].width=d;
   rcbArcs[0].height=rcbArcs[1].height=rcbArcs[2].height=rcbArcs[3].height=d;

   if (inc_x) {
      if (inc_y) {
         rcbArcs[0].x=x1; rcbArcs[0].y=y1;
         rcbArcs[1].x=x2-d; rcbArcs[1].y=y1;
         rcbArcs[2].x=x2-d; rcbArcs[2].y=y2-d;
         rcbArcs[3].x=x1; rcbArcs[3].y=y2-d;
      } else {
         rcbArcs[0].x=x1; rcbArcs[0].y=y2;
         rcbArcs[1].x=x2-d; rcbArcs[1].y=y2;
         rcbArcs[2].x=x2-d; rcbArcs[2].y=y1-d;
         rcbArcs[3].x=x1; rcbArcs[3].y=y1-d;
      }
   } else {
      if (inc_y) {
         rcbArcs[0].x=x2; rcbArcs[0].y=y1;
         rcbArcs[1].x=x1-d; rcbArcs[1].y=y1;
         rcbArcs[2].x=x1-d; rcbArcs[2].y=y2-d;
         rcbArcs[3].x=x2; rcbArcs[3].y=y2-d;
      } else {
         rcbArcs[0].x=x2; rcbArcs[0].y=y2;
         rcbArcs[1].x=x1-d; rcbArcs[1].y=y2;
         rcbArcs[2].x=x1-d; rcbArcs[2].y=y1-d;
         rcbArcs[3].x=x2; rcbArcs[3].y=y1-d;
      }
   }
}

void MyRCBox(window, gc, x1, y1, x2, y2, r)
   Window window;
   GC gc;
   int x1, y1, x2, y2, r;
{
   if (abs(x1-x2) < 2*r || abs(y1-y2) < 2*r) {
      MyBox(window, gc, x1, y1, x2, y2);
   } else {
      XDrawSegments(mainDisplay, window, gc, rcbSegs, 4);
      XDrawArcs(mainDisplay, window, gc, rcbArcs, 4);
   }
}
 
static
void MyFillRCBox(window, gc, x1, y1, x2, y2, r)
   Window window;
   GC gc;
   int x1, y1, x2, y2, r;
{
   if (abs(x1-x2) < 2*r || abs(y1-y2) < 2*r) {
      XFillRectangle(mainDisplay, window, gc, x1, y1, x2-x1, y2-y1);
   } else {
      XFillRectangle(mainDisplay, window, gc, x1+r, y1, x2-x1-2*r, y2-y1);
      XFillRectangle(mainDisplay, window, gc, x1, y1+r, x2-x1, y2-y1-2*r);
      XFillArcs(mainDisplay, window, gc, rcbArcs, 4);
   }
}

static
void DumpRCBoxPSPath(FP, ltx, lty, rbx, rby, r, blank1, blank2)
   FILE *FP;
   int ltx, lty, rbx, rby, r;
   char *blank1, *blank2;
{
   if (abs(ltx-rbx) < 2*r || abs(lty-rby) < 2*r) {
      fprintf(FP, "%s%s\n%s%1d %1d %s ", blank1,
            gPsCmd[PS_NEWPATH], blank2, rbx, lty, gPsCmd[PS_MOVETO]);
      fprintf(FP, "%1d %1d %s ", rbx, rby, gPsCmd[PS_LINETO]);
      fprintf(FP, "%1d %1d %s ", ltx, rby, gPsCmd[PS_LINETO]);
      fprintf(FP, "%1d %1d %s\n", ltx, lty, gPsCmd[PS_LINETO]);
   } else {
      fprintf(FP, "%s%s\n%s%1d %1d %s\n", blank1,
            gPsCmd[PS_NEWPATH], blank2, rbx-r, lty, gPsCmd[PS_MOVETO]);
      fprintf(FP, "%s%1d %1d %1d %1d %1d %s\n", blank2,
            rbx, lty, rbx, rby, r, gPsCmd[PS_ARCTO4]);
      fprintf(FP, "%s%1d %1d %s\n", blank2, rbx, rby-r, gPsCmd[PS_LINETO]);
      fprintf(FP, "%s%1d %1d %1d %1d %1d %s\n", blank2,
            rbx, rby, ltx, rby, r, gPsCmd[PS_ARCTO4]);
      fprintf(FP, "%s%1d %1d %s\n", blank2, ltx+r, rby, gPsCmd[PS_LINETO]);
      fprintf(FP, "%s%1d %1d %1d %1d %1d %s\n", blank2,
            ltx, rby, ltx, lty, r, gPsCmd[PS_ARCTO4]);
      fprintf(FP, "%s%1d %1d %s\n", blank2, ltx, lty+r, gPsCmd[PS_LINETO]);
      fprintf(FP, "%s%1d %1d %1d %1d %1d %s\n", blank2,
            ltx, lty, rbx, lty, r, gPsCmd[PS_ARCTO4]);
   }
}

static
void DumpRCBoxPath(FP, ObjPtr, ltx, lty, rbx, rby, r, width, pen, dash,
      trans_pat)
   FILE *FP;
   struct ObjRec *ObjPtr;
   int ltx, lty, rbx, rby, r, width, pen, dash, trans_pat;
{
   register int i;
   int w_is_int=TRUE;
   char *width_spec=ObjPtr->detail.rcb->width_spec;
   double dw=GetWidthInDouble(width, width_spec, &w_is_int);

   fprintf(FP, "   %s\n", gPsCmd[PS_GSAVE]);
   if (!colorDump && useGray && pen > BACKPAT) {
      GrayCheck(pen);
      fprintf(FP, "      %s %s\n", GrayStr(pen), gPsCmd[PS_SETGRAY]);
   }
   DumpRCBoxPSPath(FP, ltx, lty, rbx, rby, r, "      ", "         ");
   fprintf(FP, "      %s\n", gPsCmd[PS_CLOSEPATH]);

   if (ObjPtr->ctm != NULL) {
      fprintf(FP, "      %s\n", &(gPsCmd[PS_TGIFSETMATRIX])[1]);
   }
   if (dash != 0) {
      fprintf(FP, "      [");
      for (i=0; i < dashListLength[dash]-1; i++) {
         fprintf(FP, "%1d ", (int)(dashList[dash][i]));
      }
      fprintf(FP, "%1d] 0 %s\n",
            (int)(dashList[dash][dashListLength[dash]-1]), gPsCmd[PS_SETDASH]);
   }
   if (w_is_int) {
      if (width != 1) {
         fprintf(FP, "      %1d %s\n", width, gPsCmd[PS_SETLINEWIDTH]);
      }
   } else {
      fprintf(FP, "      %.3f %s\n", dw, gPsCmd[PS_SETLINEWIDTH]);
   }
   switch (pen) {
   case SOLIDPAT: fprintf(FP, "      %s\n", gPsCmd[PS_STROKE]); break;
   case BACKPAT:
      fprintf(FP, "      1 %s %s 0 %s\n",
            gPsCmd[PS_SETGRAY], gPsCmd[PS_STROKE], gPsCmd[PS_SETGRAY]);
      break;
   default:
      if (colorDump || !useGray) {
         if (preDumpSetup) PSUseColorPattern();
         fprintf(FP, "      %s\n", gPsCmd[PS_FLATTENPATH]);
         DumpPatFill(FP, pen, ObjPtr->bbox, 6, TRUE);
      } else {
         fprintf(FP, "      %s\n", gPsCmd[PS_STROKE]);
      }
      break;
   }
   fprintf(FP, "   %s\n", gPsCmd[PS_GRESTORE]);
}
 
void DumpRCBoxObj(FP, ObjPtr)
   FILE *FP;
   struct ObjRec *ObjPtr;
{
   register int ltx, lty, rbx, rby;
   int trans_pat, fill, width, pen, dash, color_index, r;

   if (ObjPtr->ctm == NULL) {
      ltx = ObjPtr->obbox.ltx; lty = ObjPtr->obbox.lty;
      rbx = ObjPtr->obbox.rbx; rby = ObjPtr->obbox.rby;
   } else {
      ltx = ObjPtr->orig_obbox.ltx; lty = ObjPtr->orig_obbox.lty;
      rbx = ObjPtr->orig_obbox.rbx; rby = ObjPtr->orig_obbox.rby;
   }

   trans_pat = ObjPtr->trans_pat;
   fill = ObjPtr->detail.rcb->fill;
   pen = ObjPtr->detail.rcb->pen;
   width = ObjPtr->detail.rcb->width;
   dash = ObjPtr->detail.rcb->dash;
   r = ObjPtr->detail.rcb->radius;

   if ((fill == NONEPAT || (trans_pat && fill == BACKPAT)) &&
         (pen == NONEPAT || (trans_pat && pen == BACKPAT))) {
      return;
   }
   fprintf(FP, "%% RCBOX\n");
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
   color_index = ObjPtr->color;
   DumpRGBColorLine(FP, color_index, 0, TRUE);

   switch (fill) {
   case NONEPAT: break;
   case SOLIDPAT:
      DumpRCBoxPSPath(FP, ltx, lty, rbx, rby, r, "", "   ");
      fprintf(FP, "%s %s\n", gPsCmd[PS_CLOSEPATH], gPsCmd[PS_FILL]);
      break;
   case BACKPAT:
      if (!trans_pat) {
         DumpRCBoxPSPath(FP, ltx, lty, rbx, rby, r, "", "   ");
         fprintf(FP, "%s 1 %s %s\n",
               gPsCmd[PS_CLOSEPATH], gPsCmd[PS_SETGRAY], gPsCmd[PS_FILL]);
         DumpRGBColorLine(FP, color_index, 3, TRUE);
      }
      break;
   default:
      fprintf(FP, "%s\n", gPsCmd[PS_GSAVE]);
      if (colorDump || !useGray) {
         if (!trans_pat) {
            DumpRCBoxPSPath(FP, ltx, lty, rbx, rby, r, "   ", "      ");
            fprintf(FP, "   %s 1 %s %s\n", gPsCmd[PS_CLOSEPATH],
                  gPsCmd[PS_SETGRAY], gPsCmd[PS_FILL]);
            DumpRGBColorLine(FP, color_index, 3, TRUE);
         }
         DumpRCBoxPSPath(FP, ltx, lty, rbx, rby, r, "   ", "      ");
         if (preDumpSetup) PSUseColorPattern();
         fprintf(FP, "   %s %s %s\n", gPsCmd[PS_CLOSEPATH],
               gPsCmd[PS_EOCLIP], gPsCmd[PS_NEWPATH]);
         DumpPatFill(FP, fill, ObjPtr->bbox, 3, TRUE);
      } else {
         GrayCheck(fill);
         fprintf(FP, "   %s %s\n", GrayStr(fill), gPsCmd[PS_SETGRAY]);
         DumpRCBoxPSPath(FP, ltx, lty, rbx, rby, r, "   ", "      ");
         fprintf(FP, "   %s %s\n", gPsCmd[PS_CLOSEPATH], gPsCmd[PS_FILL]);
      }
      fprintf(FP, "%s\n", gPsCmd[PS_GRESTORE]);
      break;
   }
   if (pen == NONEPAT || (pen == BACKPAT && trans_pat)) {
      if (ObjPtr->ctm != NULL) fprintf(FP, "%s\n", gPsCmd[PS_GRESTORE]);
      fprintf(FP, "\n");
      return;
   }
   fprintf(FP, "%s\n", gPsCmd[PS_GSAVE]);

   if ((colorDump || !useGray) && pen > BACKPAT && !trans_pat) {
      DumpRCBoxPath(FP, ObjPtr, ltx, lty, rbx, rby, r, width, BACKPAT, 0,
            trans_pat);
      DumpRGBColorLine(FP, color_index, 3, TRUE);
   }
   DumpRCBoxPath(FP, ObjPtr, ltx, lty, rbx, rby, r, width, pen, dash,
         trans_pat);

   fprintf(FP, "%s\n", gPsCmd[PS_GRESTORE]);
   if (ObjPtr->ctm != NULL) fprintf(FP, "%s\n", gPsCmd[PS_GRESTORE]);
   fprintf(FP, "\n");
}

int NeedsToCacheRCBoxObj(ObjPtr)
   struct ObjRec *ObjPtr;
{
   return (ObjPtr->ctm != NULL);
}

static
void MakeCachedRCBox(ObjPtr)
   struct ObjRec *ObjPtr;
{
   struct RCBoxRec *rcbox_ptr=ObjPtr->detail.rcb;
   XPoint *sv=NULL;
   IntPoint *pv;
   char *smooth=NULL;
   struct BBRec obbox;
   int i, w, h, sn;
   int r=rcbox_ptr->radius, num_vs;

   if (ObjPtr->ctm == NULL) return;

   obbox.ltx = ObjPtr->orig_obbox.ltx - ObjPtr->x;
   obbox.lty = ObjPtr->orig_obbox.lty - ObjPtr->y;
   obbox.rbx = ObjPtr->orig_obbox.rbx - ObjPtr->x;
   obbox.rby = ObjPtr->orig_obbox.rby - ObjPtr->y;
   w = obbox.rbx - obbox.ltx;
   h = obbox.rby - obbox.lty;
   num_vs = (w < (r<<1) || h < (r<<1)) ? 5 : 13;

   if (rcbox_ptr->rotated_vlist != NULL) free(rcbox_ptr->rotated_vlist);
   rcbox_ptr->rotated_n = 0;
   rcbox_ptr->rotated_vlist = (XPoint*)malloc((num_vs+1)*sizeof(XPoint));
   pv = (IntPoint*)malloc((num_vs+1)*sizeof(IntPoint));
   if (rcbox_ptr->rotated_vlist == NULL || pv == NULL) FailAllocMessage();
   if (num_vs == 13) {
      smooth = (char*)malloc((num_vs+1)*sizeof(char));
      if (smooth == NULL) FailAllocMessage();
   }
   if (num_vs == 5) {
      pv[0].x = obbox.ltx; pv[0].y = obbox.lty;
      pv[1].x = obbox.rbx; pv[1].y = obbox.lty;
      pv[2].x = obbox.rbx; pv[2].y = obbox.rby;
      pv[3].x = obbox.ltx; pv[3].y = obbox.rby;
      pv[4].x = pv[0].x; pv[4].y = pv[0].y;
   } else {
      pv[0].x  = obbox.ltx;   pv[0].y  = obbox.lty;
      pv[1].x  = obbox.ltx+r; pv[1].y  = obbox.lty;
      pv[2].x  = obbox.rbx-r; pv[2].y  = obbox.lty;
      pv[3].x  = obbox.rbx;   pv[3].y  = obbox.lty;
      pv[4].x  = obbox.rbx;   pv[4].y  = obbox.lty+r;
      pv[5].x  = obbox.rbx;   pv[5].y  = obbox.rby-r;
      pv[6].x  = obbox.rbx;   pv[6].y  = obbox.rby;
      pv[7].x  = obbox.rbx-r; pv[7].y  = obbox.rby;
      pv[8].x  = obbox.ltx+r; pv[8].y  = obbox.rby;
      pv[9].x  = obbox.ltx;   pv[9].y  = obbox.rby;
      pv[10].x = obbox.ltx;   pv[10].y = obbox.rby-r;
      pv[11].x = obbox.ltx;   pv[11].y = obbox.lty+r;
      pv[12].x = pv[0].x; pv[12].y = pv[0].y;
      for (i=0; i < num_vs; i++) smooth[i] = FALSE;
      smooth[0] = smooth[3] = smooth[6] = smooth[9] = smooth[12] = TRUE;
   }
   for (i=0; i < num_vs; i++) {
      int x, y;

      TransformPointThroughCTM(pv[i].x, pv[i].y, ObjPtr->ctm, &x, &y);
      pv[i].x = x + ObjPtr->x;
      pv[i].y = y + ObjPtr->y;
      rcbox_ptr->rotated_vlist[i].x = (short)OFFSET_X(pv[i].x);
      rcbox_ptr->rotated_vlist[i].y = (short)OFFSET_Y(pv[i].y);
   }
   if (num_vs == 13) {
      sv = MakeMultiSplinePolygonVertex(LT_SPLINE, &sn, smooth, drawOrigX,
            drawOrigY, num_vs, pv);
      if (sv == NULL) FailAllocMessage();
      free(rcbox_ptr->rotated_vlist);
      rcbox_ptr->rotated_n = sn;
      rcbox_ptr->rotated_vlist = sv;
   } else {
      rcbox_ptr->rotated_n = num_vs;
   }
   free(pv);
   if (smooth != NULL) free(smooth);
}

void DrawRCBoxObj(win, XOff, YOff, ObjPtr)
   Window win;
   int XOff, YOff;
   struct ObjRec *ObjPtr;
{
   struct RCBoxRec *rcbox_ptr=ObjPtr->detail.rcb;
   int trans_pat, fill, pen, pixel, ltx, lty, rbx, rby, width, dash;
   int real_x_off, real_y_off, radius;
   XGCValues values;

   trans_pat = ObjPtr->trans_pat;
   pen = rcbox_ptr->pen;
   fill = rcbox_ptr->fill;
   width = rcbox_ptr->width;
   dash = rcbox_ptr->dash;
   radius = ZOOMED_SIZE(rcbox_ptr->radius);
   pixel = colorPixels[ObjPtr->color];

   if (NeedsToCacheRCBoxObj(ObjPtr) && rcbox_ptr->rotated_vlist==NULL) {
      MakeCachedRCBox(ObjPtr);
   }
   if (userDisableRedraw) return;

   if ((fill == NONEPAT || (trans_pat && fill == BACKPAT)) &&
         (pen == NONEPAT || (trans_pat && pen == BACKPAT))) {
      return;
   }
   real_x_off = (zoomedIn ? XOff : (XOff>>zoomScale)<<zoomScale);
   real_y_off = (zoomedIn ? YOff : (YOff>>zoomScale)<<zoomScale);
   ltx = ZOOMED_SIZE(ObjPtr->obbox.ltx - real_x_off);
   lty = ZOOMED_SIZE(ObjPtr->obbox.lty - real_y_off);
   rbx = ZOOMED_SIZE(ObjPtr->obbox.rbx - real_x_off);
   rby = ZOOMED_SIZE(ObjPtr->obbox.rby - real_y_off);

   SetRCBoxVertex(ltx, lty, rbx, rby, radius);

   if (fill != NONEPAT) {
      values.foreground = GetDrawingBgPixel(fill, pixel);
      values.function = GXcopy;
      values.fill_style = (trans_pat ? FillStippled : FillOpaqueStippled);
      values.stipple = patPixmap[fill];
      XChangeGC(mainDisplay, drawGC,
            GCForeground | GCFunction | GCFillStyle | GCStipple, &values);
      if (ObjPtr->ctm != NULL) {
         XFillPolygon(mainDisplay, win, drawGC, rcbox_ptr->rotated_vlist,
               rcbox_ptr->rotated_n, Convex, CoordModeOrigin);
      } else {
         MyFillRCBox(win, drawGC, ltx, lty, rbx, rby, radius);
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
         XDrawLines(mainDisplay, win, drawGC, rcbox_ptr->rotated_vlist,
               rcbox_ptr->rotated_n, CoordModeOrigin);
      } else {
         MyRCBox(win, drawGC, ltx, lty, rbx, rby, radius);
      }
   }
}

void CreateRCBoxObj(X1, Y1, X2, Y2, CreateAbsolute)
   int X1, Y1, X2, Y2, CreateAbsolute;
{
   struct RCBoxRec *rcbox_ptr;
   struct ObjRec *obj_ptr;
   int width, w, ltx, lty, rbx, rby;

   rcbox_ptr = (struct RCBoxRec *)malloc(sizeof(struct RCBoxRec));
   if (rcbox_ptr == NULL) FailAllocMessage();
   memset(rcbox_ptr, 0, sizeof(struct RCBoxRec));
   rcbox_ptr->fill = objFill;
   rcbox_ptr->width = width = curWidthOfLine[lineWidth];
   UtilStrCpyN(rcbox_ptr->width_spec, sizeof(rcbox_ptr->width_spec),
         curWidthOfLineSpec[lineWidth]);
   rcbox_ptr->pen = penPat;
   rcbox_ptr->dash = curDash;
   rcbox_ptr->radius = rcbRadius;

   rcbox_ptr->rotated_n = 0;
   rcbox_ptr->rotated_vlist = NULL;

   obj_ptr = (struct ObjRec *)malloc(sizeof(struct ObjRec));
   if (obj_ptr == NULL) FailAllocMessage();
   memset(obj_ptr, 0, sizeof(struct ObjRec));

   if (X1 < X2) {
      if (Y1 < Y2) {
         ltx = X1; lty = Y1; rbx = X2; rby = Y2;
      } else {
         ltx = X1; lty = Y2; rbx = X2; rby = Y1;
      }
   } else {
      if (Y1 < Y2) {
         ltx = X2; lty = Y1; rbx = X1; rby = Y2;
      } else {
         ltx = X2; lty = Y2; rbx = X1; rby = Y1;
      }
   }
   if (CreateAbsolute) {
      obj_ptr->bbox.ltx = obj_ptr->obbox.ltx = obj_ptr->x = ltx;
      obj_ptr->bbox.lty = obj_ptr->obbox.lty = obj_ptr->y = lty;
      obj_ptr->bbox.rbx = obj_ptr->obbox.rbx = rbx;
      obj_ptr->bbox.rby = obj_ptr->obbox.rby = rby;
   } else {
      obj_ptr->bbox.ltx = obj_ptr->obbox.ltx = obj_ptr->x = ABS_X(ltx);
      obj_ptr->bbox.lty = obj_ptr->obbox.lty = obj_ptr->y = ABS_Y(lty);
      obj_ptr->bbox.rbx = obj_ptr->obbox.rbx = ABS_X(rbx);
      obj_ptr->bbox.rby = obj_ptr->obbox.rby = ABS_Y(rby);
   }
   w = HALF_W(width);
   obj_ptr->bbox.ltx -= w;
   obj_ptr->bbox.lty -= w;
   obj_ptr->bbox.rbx += w;
   obj_ptr->bbox.rby += w;
   obj_ptr->type = OBJ_RCBOX;
   obj_ptr->color = colorIndex;
   if (mainDisplay != NULL) {
      UtilStrCpyN(obj_ptr->color_str, sizeof(obj_ptr->color_str),
            colorMenuItems[colorIndex]);
   }
   obj_ptr->id = objId++;
   obj_ptr->dirty = FALSE;
   obj_ptr->rotation = 0;
   obj_ptr->locked = FALSE;
   obj_ptr->detail.rcb = rcbox_ptr;
   obj_ptr->fattr = obj_ptr->lattr = NULL;
   obj_ptr->ctm = NULL;
   obj_ptr->invisible = FALSE;
   obj_ptr->trans_pat = transPat;

   AddObj(NULL, topObj, obj_ptr);
}

static
void ContinueRCBox(OrigX, OrigY)
   int OrigX, OrigY;
{
   int end_x, end_y, grid_x, grid_y, saved_x, saved_y;
   int done=FALSE, abort=FALSE;
   int radius=ZOOMED_SIZE(rcbRadius);
   char buf[80], w_buf[80], h_buf[80], x_buf[80], y_buf[80];
   XEvent input, ev;
   XMotionEvent *motion_ev;

   SetXorDrawGC(xorColorPixels[colorIndex]);

   saved_x = grid_x = OrigX;
   saved_y = grid_y = OrigY; 
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
   SetRCBoxVertex(OrigX, OrigY, saved_x, saved_y, radius);
   while (!done) {
      XNextEvent(mainDisplay, &input);

      if (input.type == Expose || input.type == VisibilityNotify) {
         ExposeEventHandler(&input, TRUE);
         SetXorDrawGC(xorColorPixels[colorIndex]);
      } else if (input.type == ButtonRelease) {
         XUngrabPointer(mainDisplay, CurrentTime);
         MyRCBox(drawWindow,drawGC,OrigX,OrigY,saved_x,saved_y,radius);
         EndIntervalRulers(grid_x, grid_y);
         PixelToMeasurementUnit(w_buf, ABS_SIZE(abs(saved_x-OrigX)));
         PixelToMeasurementUnit(h_buf, ABS_SIZE(abs(saved_y-OrigY)));
         PixelToMeasurementUnit(x_buf, ABS_X(saved_x));
         PixelToMeasurementUnit(y_buf, ABS_Y(saved_y));
         sprintf(buf, "w=%s\nh=%s\nx=%s\ny=%s", w_buf, h_buf, x_buf, y_buf);
         EndShowMeasureCursor(saved_x, saved_y, buf, TRUE);
         done = TRUE;
      } else if (input.type == MotionNotify) {
         motion_ev = &(input.xmotion);
         end_x = motion_ev->x;
         end_y = motion_ev->y;
         GridXY(end_x, end_y, &grid_x, &grid_y);
         if (motion_ev->state & (ShiftMask | ControlMask)) {
            int w, h, pos_w=TRUE, pos_h=TRUE;

            w = grid_x - OrigX;
            h = grid_y - OrigY;
            if (w < 0) {
               w = (-w);
               pos_w = FALSE;
            }
            if (h < 0) {
               h = (-h);
               pos_h = FALSE;
            }
            if (w > h) {
               grid_x = (pos_w ? (OrigX+h) : (OrigX-h));
            } else {
               grid_y = (pos_h ? (OrigY+w) : (OrigY-w));
            }
         }
         if (grid_x != saved_x || grid_y != saved_y) {
            PixelToMeasurementUnit(w_buf, ABS_SIZE(abs(saved_x-OrigX)));
            PixelToMeasurementUnit(h_buf, ABS_SIZE(abs(saved_y-OrigY)));
            PixelToMeasurementUnit(x_buf, ABS_X(saved_x));
            PixelToMeasurementUnit(y_buf, ABS_Y(saved_y));
            sprintf(buf, "w=%s\nh=%s\nx=%s\ny=%s", w_buf, h_buf, x_buf, y_buf);
            ShowMeasureCursor(saved_x, saved_y, buf, TRUE);
            MyRCBox(drawWindow,drawGC,OrigX,OrigY,saved_x,saved_y,radius);
            saved_x = grid_x;
            saved_y = grid_y;
            SetRCBoxVertex(OrigX, OrigY, saved_x, saved_y, radius);
            MyRCBox(drawWindow,drawGC,OrigX,OrigY,saved_x,saved_y,radius);
            PixelToMeasurementUnit(w_buf, ABS_SIZE(abs(saved_x-OrigX)));
            PixelToMeasurementUnit(h_buf, ABS_SIZE(abs(saved_y-OrigY)));
            PixelToMeasurementUnit(x_buf, ABS_X(saved_x));
            PixelToMeasurementUnit(y_buf, ABS_Y(saved_y));
            sprintf(buf, "w=%s\nh=%s\nx=%s\ny=%s", w_buf, h_buf, x_buf, y_buf);
            ShowMeasureCursor(saved_x, saved_y, buf, TRUE);
         }
         DrawIntervalRulers(OrigX, OrigY, grid_x, grid_y, NULL);
         while (XCheckMaskEvent(mainDisplay, PointerMotionMask, &ev)) ;
      } else if (input.type == KeyPress) {
         if (KeyPressEventIsEscape(&input.xkey)) {
            XUngrabPointer(mainDisplay, CurrentTime);
            MyRCBox(drawWindow,drawGC,OrigX,OrigY,saved_x,saved_y,radius);
            EndIntervalRulers(grid_x, grid_y);
            PixelToMeasurementUnit(w_buf, ABS_SIZE(abs(saved_x-OrigX)));
            PixelToMeasurementUnit(h_buf, ABS_SIZE(abs(saved_y-OrigY)));
            PixelToMeasurementUnit(x_buf, ABS_X(saved_x));
            PixelToMeasurementUnit(y_buf, ABS_Y(saved_y));
            sprintf(buf, "w=%s\nh=%s\nx=%s\ny=%s", w_buf, h_buf, x_buf, y_buf);
            EndShowMeasureCursor(saved_x, saved_y, buf, TRUE);
            abort = TRUE;
            done = TRUE;
         }
      }
   }
   if (!abort && OrigX != grid_x && OrigY != grid_y) {
      CreateRCBoxObj(OrigX, OrigY, grid_x, grid_y, FALSE);
      RecordNewObjCmd();
      DrawRCBoxObj(drawWindow, drawOrigX, drawOrigY, topObj);
      rcBoxDrawn = TRUE;
      SetFileModified(TRUE);
   }
   XSync(mainDisplay, False);
}

void DrawRCBox(input)
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
      ContinueRCBox(grid_x, grid_y);
   }
}

void MakeRCBoxObjFromBoundingBox()
{
   if (topSel == NULL) {
      MsgBox(TgLoadCachedString(CSTID_NO_OBJ_SELECTED), TOOL_NAME, INFO_MB);
      return;
   }
   if (curChoice == VERTEXMODE) SetCurChoice(NOTHING);

   HighLightReverse();
   CreateRCBoxObj(selObjLtX, selObjLtY, selObjRbX, selObjRbY, TRUE);
   SelectTopObj();
   RecordNewObjCmd();
   RedrawAnArea(botObj, selLtX-GRID_ABS_SIZE(1), selLtY-GRID_ABS_SIZE(1),
         selRbX+GRID_ABS_SIZE(1), selRbY+GRID_ABS_SIZE(1));
   HighLightForward();
   justDupped = FALSE;
   SetFileModified(TRUE);
}

void SaveRCBoxObj(FP, ObjPtr)
   FILE *FP;
   struct ObjRec *ObjPtr;
{
   struct RCBoxRec *rcbox_ptr=ObjPtr->detail.rcb;

   if (fprintf(FP, "rcbox('%s','',", colorMenuItems[ObjPtr->color]) == EOF) {
      writeFileFailed = TRUE;
   }
   if (fprintf(FP,
         "%1d,%1d,%1d,%1d,%1d,%1d,%1d,%1d,%1d,%1d,%1d,%1d,%1d,%1d,'%s',%1d,",
         ObjPtr->obbox.ltx, ObjPtr->obbox.lty, ObjPtr->obbox.rbx,
         ObjPtr->obbox.rby, rcbox_ptr->fill, rcbox_ptr->width, rcbox_ptr->pen,
         rcbox_ptr->dash, rcbox_ptr->radius, ObjPtr->id, ObjPtr->rotation,
         ObjPtr->locked, ObjPtr->ctm!=NULL, ObjPtr->invisible,
         rcbox_ptr->width_spec, ObjPtr->trans_pat) == EOF) {
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

void ReadRCBoxObj(FP, Inbuf, ObjPtr)
   FILE *FP;
   char *Inbuf;
   struct ObjRec **ObjPtr;
{
   struct RCBoxRec *rcbox_ptr;
   char color_str[40], bg_color_str[40], *s, msg[MAXSTRING], width_spec[40];
   int ltx, lty, rbx, rby, trans_pat=FALSE, fill, width, pen, dash, radius;
   int rotation, new_alloc, id=0, w, locked=FALSE;
   int transformed=FALSE, invisible=FALSE;

   *ObjPtr = NULL;

   s = FindChar((int)'(', Inbuf);
   s = ParseStr(s, (int)',', color_str, sizeof(color_str));
   if (fileVersion >= 37) {
      s = ParseStr(s, (int)',', bg_color_str, sizeof(bg_color_str));
   }
   InitScan(s, ", \t\n");

   rotation = 0;
   *width_spec = '\0';
   if (fileVersion <= 8) {
      sprintf(msg, TgLoadCachedString(CSTID_FILEVER_TOO_LARGE_FOR_RCBOX),
            fileVersion);
      if (PRTGIF) {
         fprintf(stderr, "%s\n", msg);
      } else {
         Msg(msg);
      }
      return;
   } else if (fileVersion <= 13) {
      if (GETINT("rcbox", ltx,      "ltx") == INVALID ||
          GETINT("rcbox", lty,      "lty") == INVALID ||
          GETINT("rcbox", rbx,      "rbx") == INVALID ||
          GETINT("rcbox", rby,      "rby") == INVALID ||
          GETINT("rcbox", fill,     "fill") == INVALID ||
          GETINT("rcbox", width,    "width") == INVALID ||
          GETINT("rcbox", pen,      "pen") == INVALID ||
          GETINT("rcbox", dash,     "dash") == INVALID ||
          GETINT("rcbox", radius,   "radius") == INVALID ||
          GETINT("rcbox", id,       "id") == INVALID) {
         return;
      }
      if (id >= objId) objId = id+1;
   } else if (fileVersion <= 25) {
      if (GETINT("rcbox", ltx,      "ltx") == INVALID ||
          GETINT("rcbox", lty,      "lty") == INVALID ||
          GETINT("rcbox", rbx,      "rbx") == INVALID ||
          GETINT("rcbox", rby,      "rby") == INVALID ||
          GETINT("rcbox", fill,     "fill") == INVALID ||
          GETINT("rcbox", width,    "width") == INVALID ||
          GETINT("rcbox", pen,      "pen") == INVALID ||
          GETINT("rcbox", dash,     "dash") == INVALID ||
          GETINT("rcbox", radius,   "radius") == INVALID ||
          GETINT("rcbox", id,       "id") == INVALID ||
          GETINT("rcbox", rotation, "rotation") == INVALID) {
         return;
      }
      if (id >= objId) objId = id+1;
   } else if (fileVersion <= 32) {
      if (GETINT("rcbox", ltx,      "ltx") == INVALID ||
          GETINT("rcbox", lty,      "lty") == INVALID ||
          GETINT("rcbox", rbx,      "rbx") == INVALID ||
          GETINT("rcbox", rby,      "rby") == INVALID ||
          GETINT("rcbox", fill,     "fill") == INVALID ||
          GETINT("rcbox", width,    "width") == INVALID ||
          GETINT("rcbox", pen,      "pen") == INVALID ||
          GETINT("rcbox", dash,     "dash") == INVALID ||
          GETINT("rcbox", radius,   "radius") == INVALID ||
          GETINT("rcbox", id,       "id") == INVALID ||
          GETINT("rcbox", rotation, "rotation") == INVALID ||
          GETINT("rcbox", locked,   "locked") == INVALID) {
         return;
      }
      if (id >= objId) objId = id+1;
   } else if (fileVersion <= 34) {
      if (GETINT("rcbox", ltx,         "ltx") == INVALID ||
          GETINT("rcbox", lty,         "lty") == INVALID ||
          GETINT("rcbox", rbx,         "rbx") == INVALID ||
          GETINT("rcbox", rby,         "rby") == INVALID ||
          GETINT("rcbox", fill,        "fill") == INVALID ||
          GETINT("rcbox", width,       "width") == INVALID ||
          GETINT("rcbox", pen,         "pen") == INVALID ||
          GETINT("rcbox", dash,        "dash") == INVALID ||
          GETINT("rcbox", radius,      "radius") == INVALID ||
          GETINT("rcbox", id,          "id") == INVALID ||
          GETINT("rcbox", rotation,    "rotation") == INVALID ||
          GETINT("rcbox", locked,      "locked") == INVALID ||
          GETINT("rcbox", transformed, "transformed") == INVALID ||
          GETINT("rcbox", invisible,   "invisible") == INVALID ||
          GETSTR("rcbox", width_spec,  "width_spec") == INVALID) {
         return;
      }
      if (id >= objId) objId = id+1;
      UtilRemoveQuotes(width_spec);
   } else {
      if (GETINT("rcbox", ltx,         "ltx") == INVALID ||
          GETINT("rcbox", lty,         "lty") == INVALID ||
          GETINT("rcbox", rbx,         "rbx") == INVALID ||
          GETINT("rcbox", rby,         "rby") == INVALID ||
          GETINT("rcbox", fill,        "fill") == INVALID ||
          GETINT("rcbox", width,       "width") == INVALID ||
          GETINT("rcbox", pen,         "pen") == INVALID ||
          GETINT("rcbox", dash,        "dash") == INVALID ||
          GETINT("rcbox", radius,      "radius") == INVALID ||
          GETINT("rcbox", id,          "id") == INVALID ||
          GETINT("rcbox", rotation,    "rotation") == INVALID ||
          GETINT("rcbox", locked,      "locked") == INVALID ||
          GETINT("rcbox", transformed, "transformed") == INVALID ||
          GETINT("rcbox", invisible,   "invisible") == INVALID ||
          GETSTR("rcbox", width_spec,  "width_spec") == INVALID ||
          GETINT("rcbox", trans_pat,   "trans_pat") == INVALID) {
         return;
      }
      if (id >= objId) objId = id+1;
      UtilRemoveQuotes(width_spec);
   }
   if (ltx > rbx || lty > rby) {
      int tmp_ltx, tmp_lty, tmp_rbx, tmp_rby;

      if (!PRTGIF) Msg(TgLoadCachedString(CSTID_BAD_RCBOX_BBOX_ADJUSTED));
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
   rcbox_ptr = (struct RCBoxRec *)malloc(sizeof(struct RCBoxRec));
   if (rcbox_ptr == NULL) FailAllocMessage();
   memset(rcbox_ptr, 0, sizeof(struct RCBoxRec));

   rcbox_ptr->fill = fill;
   rcbox_ptr->width = width;
   UtilStrCpyN(rcbox_ptr->width_spec, sizeof(rcbox_ptr->width_spec),
         width_spec);
   rcbox_ptr->pen = pen;
   rcbox_ptr->dash = dash;
   rcbox_ptr->radius = radius;

   rcbox_ptr->rotated_n = 0;
   rcbox_ptr->rotated_vlist = NULL;

   (*ObjPtr)->x = ltx;
   (*ObjPtr)->y = lty;
   (*ObjPtr)->color = QuickFindColorIndex(*ObjPtr, color_str, &new_alloc, TRUE);
   UtilStrCpyN((*ObjPtr)->color_str, sizeof((*ObjPtr)->color_str), color_str);
   (*ObjPtr)->dirty = FALSE;
   (*ObjPtr)->id = id;
   (*ObjPtr)->rotation = rotation;
   (*ObjPtr)->locked = locked;
   (*ObjPtr)->type = OBJ_RCBOX;
   (*ObjPtr)->obbox.ltx = ltx;
   (*ObjPtr)->obbox.lty = lty;
   (*ObjPtr)->obbox.rbx = rbx;
   (*ObjPtr)->obbox.rby = rby;
   w = HALF_W(width);
   (*ObjPtr)->bbox.ltx = ltx - w;
   (*ObjPtr)->bbox.lty = lty - w;
   (*ObjPtr)->bbox.rbx = rbx + w;
   (*ObjPtr)->bbox.rby = rby + w;
   (*ObjPtr)->detail.rcb = rcbox_ptr;
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
      if (GETINT("rcbox", real_x,           "real_x") == INVALID ||
          GETINT("rcbox", real_y,           "real_y") == INVALID ||
          GETINT("rcbox", orig_obbox.ltx,   "orig_obbox.ltx") == INVALID ||
          GETINT("rcbox", orig_obbox.lty,   "orig_obbox.lty") == INVALID ||
          GETINT("rcbox", orig_obbox.rbx,   "orig_obbox.rbx") == INVALID ||
          GETINT("rcbox", orig_obbox.rby,   "orig_obbox.rby") == INVALID ||
          GETDBL("rcbox", ctm->m[CTM_SX],   "CTM_SX") == INVALID ||
          GETDBL("rcbox", ctm->m[CTM_SIN],  "CTM_SIN") == INVALID ||
          GETDBL("rcbox", ctm->m[CTM_MSIN], "CTM_MSIN") == INVALID ||
          GETDBL("rcbox", ctm->m[CTM_SY],   "CTM_SY") == INVALID ||
          GETINT("rcbox", ctm->t[CTM_TX],   "CTM_TX") == INVALID ||
          GETINT("rcbox", ctm->t[CTM_TY],   "CTM_TY") == INVALID) {
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

void SetRCBoxPropMask(ObjPtr, plMask, plSkip, pProp)
   struct ObjRec *ObjPtr;
   long *plMask, *plSkip;
   struct PropertiesRec *pProp;
{
   struct RCBoxRec *rcbox_ptr=ObjPtr->detail.rcb;

   SetCTMPropertyMask(ObjPtr->ctm, plMask, plSkip, pProp);

   SetIntPropertyMask(PROP_MASK_COLOR, ObjPtr->color,
         colorMenuItems[ObjPtr->color], plMask, plSkip, pProp);
   SetIntPropertyMask(PROP_MASK_WIDTH, rcbox_ptr->width, rcbox_ptr->width_spec,
         plMask, plSkip, pProp);

   SetIntPropertyMask(PROP_MASK_TRANSPAT, ObjPtr->trans_pat, NULL,
         plMask, plSkip, pProp);
   SetIntPropertyMask(PROP_MASK_FILL, rcbox_ptr->fill, NULL,
         plMask, plSkip, pProp);
   SetIntPropertyMask(PROP_MASK_PEN, rcbox_ptr->pen, NULL,
         plMask, plSkip, pProp);
   SetIntPropertyMask(PROP_MASK_DASH, rcbox_ptr->dash, NULL,
         plMask, plSkip, pProp);
   SetIntPropertyMask(PROP_MASK_RCB_RADIUS, rcbox_ptr->radius, NULL,
         plMask, plSkip, pProp);
}

void FreeRCBoxObj(ObjPtr)
   struct ObjRec *ObjPtr;
{
   if (ObjPtr->detail.rcb->rotated_vlist != NULL) {
      free(ObjPtr->detail.rcb->rotated_vlist);
   }
   free(ObjPtr->detail.rcb);
   free(ObjPtr);
}
