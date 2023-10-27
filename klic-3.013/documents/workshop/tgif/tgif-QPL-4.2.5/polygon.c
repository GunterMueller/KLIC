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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/polygon.c,v 1.41 2011/06/09 15:55:15 cvsps Exp $
 */

#define _INCLUDE_FROM_POLYGON_C_

#include "tgifdefs.h"

#include "arc.e"
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
#include "grid.e"
#include "file.e"
#include "mainloop.e"
#include "mark.e"
#include "msg.e"
#include "obj.e"
#include "pattern.e"
#include "poly.e"
#include "polygon.e"
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

int polygonDrawn=FALSE;

XPoint *MakePolygonVertex(XOff, YOff, NumVs, Vs)
   int XOff, YOff, NumVs;
   register IntPoint *Vs;
{
   register XPoint *v;
   register int i;
   int real_x_off, real_y_off;

   real_x_off = (zoomedIn ? XOff : (XOff>>zoomScale)<<zoomScale);
   real_y_off = (zoomedIn ? YOff : (YOff>>zoomScale)<<zoomScale);

   v = (XPoint*)malloc((NumVs+1)*sizeof(XPoint));
   if (v == NULL) FailAllocMessage();
   for (i = 0; i < NumVs; i++) {
      v[i].x = (short)ZOOMED_SIZE(Vs[i].x-real_x_off);
      v[i].y = (short)ZOOMED_SIZE(Vs[i].y-real_y_off);
   }
   return v;
}

void DumpPoints(FP, NumPts, V, Indent)
   FILE *FP;
   int NumPts, Indent;
   IntPoint *V;
{
   register int i, j;

   for (i=1; i < NumPts; i++) {
      for (j = 0; j < Indent; j++) fprintf(FP, " ");
      fprintf(FP, "%1d %1d %s\n", V[i].x, V[i].y, gPsCmd[PS_LINETO]);
   }
}

static
void DumpJustPolygonPath(FP, Vs, NumPts, Smooth, Curved)
   FILE *FP;
   IntPoint *Vs;
   char *Smooth;
   int NumPts, Curved;
{
   fprintf(FP, "%s\n", gPsCmd[PS_NEWPATH]);
   if (Curved == LT_INTSPLINE) {
      DumpCurvedPolygonPoints(FP, Curved, NumPts, Vs, 3);
   } else {
      DumpMultiCurvedPolygonPoints(FP, Smooth, Curved, NumPts, Vs, 3);
   }
   fprintf(FP, "%s\n", gPsCmd[PS_CLOSEPATH]);
}

static
void DumpPolygonPath(FP, ObjPtr, Vs, NumPts, Width, Pen, Fill, Dash, TransPat)
   FILE *FP;
   struct ObjRec *ObjPtr;
   IntPoint *Vs;
   int NumPts, Width, Pen, Fill, Dash, TransPat;
{
   register int i;
   int w_is_int=TRUE;
   char *width_spec=ObjPtr->detail.g->width_spec;
   double dw=GetWidthInDouble(Width, width_spec, &w_is_int);

   if (Fill != (-1) && Pen == (-1)) {
      /* dumping the fill */
      switch (Fill) {
      case SOLIDPAT: fprintf(FP, "   %s\n", gPsCmd[PS_EOFILL]); break;
      case BACKPAT:
         if (!TransPat) {
            fprintf(FP, "   1 %s %s\n", gPsCmd[PS_SETGRAY], gPsCmd[PS_EOFILL]);
         }
         break;
      default:
         if (colorDump || !useGray) {
            if (preDumpSetup) PSUseColorPattern();
            fprintf(FP, "   %s %s\n", gPsCmd[PS_EOCLIP], gPsCmd[PS_NEWPATH]);
            for (i=0; i < 3; i++) fprintf(FP, " ");
            DumpPatFill(FP, Fill, ObjPtr->bbox, 0, TRUE);
         } else {
            GrayCheck(Fill);
            for (i=0; i < 3; i++) fprintf(FP, " ");
            fprintf(FP, "%s %s\n", GrayStr(Fill), gPsCmd[PS_SETGRAY]);
            fprintf(FP, "   %s\n", gPsCmd[PS_EOFILL]);
         }
         break;
      }
   } else if (Fill == (-1) && Pen != (-1)) {
      /* dumping the pen */
      if (ObjPtr->ctm != NULL) {
         fprintf(FP, "   %s\n", &(gPsCmd[PS_TGIFSETMATRIX])[1]);
      }
      if (w_is_int) {
         if (Width != 1) {
            fprintf(FP, "   %1d %s\n", Width, gPsCmd[PS_SETLINEWIDTH]);
         }
      } else {
         fprintf(FP, "   %.3f %s\n", dw, gPsCmd[PS_SETLINEWIDTH]);
      }
      if (Dash != 0) {
         fprintf(FP, "   [");
         for (i = 0; i < dashListLength[Dash]-1; i++) {
            fprintf(FP, "%1d ", (int)(dashList[Dash][i]));
         }
         fprintf(FP, "%1d] 0 %s\n",
               (int)(dashList[Dash][dashListLength[Dash]-1]),
               gPsCmd[PS_SETDASH]);
      }
      switch (Pen) {
      case SOLIDPAT: fprintf(FP, "   %s\n", gPsCmd[PS_STROKE]); break;
      case BACKPAT:
         if (!TransPat) {
            fprintf(FP, "   1 %s %s 0 %s\n", gPsCmd[PS_SETGRAY],
                  gPsCmd[PS_STROKE], gPsCmd[PS_SETGRAY]);
         }
         break;
      default:
         if (colorDump || !useGray) {
            if (preDumpSetup) PSUseColorPattern();
            fprintf(FP, "   %s\n", gPsCmd[PS_FLATTENPATH]);
            for (i=0; i < 3; i++) fprintf(FP, " ");
            DumpPatFill(FP, Pen, ObjPtr->bbox, 0, TRUE);
         } else {
            GrayCheck(Pen);
            fprintf(FP, "   %s %s\n", GrayStr(Pen), gPsCmd[PS_SETGRAY]);
            fprintf(FP, "   %s\n", gPsCmd[PS_STROKE]);
         }
         break;
      }
   }
}

void DumpPolygonObj(FP, ObjPtr)
   FILE *FP;
   struct ObjRec *ObjPtr;
{
   IntPoint *v=NULL, *intv=NULL;
   int num_pts=0, trans_pat, fill, width, pen, curved, dash, color_index, intn;
   char *smooth=NULL;

   trans_pat = ObjPtr->trans_pat;
   fill = ObjPtr->detail.g->fill;
   width = ObjPtr->detail.g->width;
   pen = ObjPtr->detail.g->pen;
   curved = ObjPtr->detail.g->curved;
   dash = ObjPtr->detail.g->dash;
   intv = ObjPtr->detail.g->intvlist;
   intn = ObjPtr->detail.g->intn;
   if (curved == LT_STRUCT_SPLINE) {
      v = ObjPtr->detail.g->ssvlist;
      num_pts = ObjPtr->detail.g->ssn;
      smooth = ObjPtr->detail.g->ssmooth;
   } else {
      v = ObjPtr->detail.g->vlist;
      num_pts = ObjPtr->detail.g->n;
      smooth = ObjPtr->detail.g->smooth;
   }

   if ((fill == NONEPAT || (trans_pat && fill == BACKPAT)) &&
         (pen == NONEPAT || (trans_pat && pen == BACKPAT))) {
      return;
   }
   fprintf(FP, "%% POLYGON/CLOSED-SPLINE\n");
   color_index = ObjPtr->color;
   DumpRGBColorLine(FP, color_index, 0, TRUE);

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
   if (curved != LT_INTSPLINE) {
      DumpJustPolygonPath(FP, v, num_pts, smooth, curved);
   } else {
      DumpJustPolygonPath(FP, intv, intn, smooth, curved);
   }
   fprintf(FP, "%s\n", gPsCmd[PS_GSAVE]);

   if (fill != NONEPAT) {
      if (curved != LT_INTSPLINE) {
         if ((colorDump || !useGray) && fill > BACKPAT && !trans_pat) {
            DumpPolygonPath(FP, ObjPtr, v, num_pts, width, (-1), BACKPAT, 0,
                  trans_pat);
            fprintf(FP, "%s\n", gPsCmd[PS_GRESTORE]);
            fprintf(FP, "%s\n", gPsCmd[PS_GSAVE]);
         }
         DumpPolygonPath(FP, ObjPtr, v, num_pts, width, (-1), fill, 0,
                  trans_pat);
      } else {
         if ((colorDump || !useGray) && fill > BACKPAT && !trans_pat) {
            DumpPolygonPath(FP, ObjPtr, intv, intn, width, (-1), BACKPAT, 0,
                  trans_pat);
            fprintf(FP, "%s\n", gPsCmd[PS_GRESTORE]);
            fprintf(FP, "%s\n", gPsCmd[PS_GSAVE]);
         }
         DumpPolygonPath(FP, ObjPtr, intv, intn, width, (-1), fill, 0,
                  trans_pat);
      }
   }
   if (pen != NONEPAT) {
      fprintf(FP, "%s\n", gPsCmd[PS_GRESTORE]);
      fprintf(FP, "%s\n", gPsCmd[PS_GSAVE]);
      if (curved != LT_INTSPLINE) {
         if ((colorDump || !useGray) && pen > BACKPAT && !trans_pat) {
            DumpPolygonPath(FP, ObjPtr, v, num_pts, width, BACKPAT, (-1), 0,
                  trans_pat);
            fprintf(FP, "%s\n", gPsCmd[PS_GRESTORE]);
            fprintf(FP, "%s\n", gPsCmd[PS_GSAVE]);
         }
         DumpPolygonPath(FP, ObjPtr, v, num_pts, width, pen, (-1), dash,
                  trans_pat);
      } else {
         if ((colorDump || !useGray) && pen > BACKPAT && !trans_pat) {
            DumpPolygonPath(FP, ObjPtr, intv, intn, width, BACKPAT, (-1), 0,
                  trans_pat);
            fprintf(FP, "%s\n", gPsCmd[PS_GRESTORE]);
            fprintf(FP, "%s\n", gPsCmd[PS_GSAVE]);
         }
         DumpPolygonPath(FP, ObjPtr, intv, intn, width, pen, (-1), dash,
                  trans_pat);
      }
   }
   fprintf(FP, "%s\n", gPsCmd[PS_GRESTORE]);
   if (ObjPtr->ctm != NULL) fprintf(FP, "%s\n", gPsCmd[PS_GRESTORE]);
   fprintf(FP, "\n");
}

int NeedsToCachePolygonObj(ObjPtr)
   struct ObjRec *ObjPtr;
{
   return (ObjPtr->ctm != NULL);
}

static
void MakeCachedPolygon(ObjPtr)
   struct ObjRec *ObjPtr;
{
   register int i;
   struct PolygonRec *polygon_ptr=ObjPtr->detail.g;
   IntPoint *v=polygon_ptr->vlist, *pv=NULL, *pv1=NULL;
   int num_pts=polygon_ptr->n, num_pts1=0;

   if (ObjPtr->ctm == NULL) return;

   if (polygon_ptr->rotated_vlist != NULL) free(polygon_ptr->rotated_vlist);
   polygon_ptr->rotated_n = 0;
   polygon_ptr->rotated_vlist = NULL;

   if (polygon_ptr->curved == LT_STRUCT_SPLINE) {
      v = polygon_ptr->ssvlist;
      num_pts = polygon_ptr->ssn;
   }
   pv = (IntPoint*)malloc((num_pts+1)*sizeof(IntPoint));
   if (pv == NULL) {
      FailAllocMessage();
      return;
   }
   for (i=0; i < num_pts; i++) {
      int x, y;

      TransformPointThroughCTM(v[i].x-ObjPtr->x, v[i].y-ObjPtr->y,
            ObjPtr->ctm, &x, &y);
      pv[i].x = x+ObjPtr->x;
      pv[i].y = y+ObjPtr->y;
   }
   switch (polygon_ptr->curved) {
   case LT_STRAIGHT:
   case LT_SPLINE:
      polygon_ptr->rotated_vlist = MakeMultiSplinePolygonVertex(
            polygon_ptr->curved, &(polygon_ptr->rotated_n),
            polygon_ptr->smooth, drawOrigX, drawOrigY, num_pts, pv);
      break;
   case LT_STRUCT_SPLINE:
      polygon_ptr->rotated_vlist = MakeMultiSplinePolygonVertex(
            polygon_ptr->curved, &(polygon_ptr->rotated_n),
            polygon_ptr->ssmooth, drawOrigX, drawOrigY, num_pts, pv);
      break;
   case LT_INTSPLINE:
      polygon_ptr->rotated_vlist = MakeIntSplinePolygonVertex(
            &(polygon_ptr->rotated_n), &(num_pts1), &(pv1),
            drawOrigX, drawOrigY, num_pts, pv);
      free(pv1);
      break;
   }
   free(pv);
}

void DrawPolygonObj(Win, XOff, YOff, ObjPtr)
   Window Win;
   int XOff, YOff;
   struct ObjRec *ObjPtr;
{
   struct PolygonRec *polygon_ptr=ObjPtr->detail.g;
   XPoint *v;
   int trans_pat, fill, width, pen, dash, pixel, num_pts;
   XGCValues values;

   trans_pat = ObjPtr->trans_pat;
   fill = polygon_ptr->fill;
   width = polygon_ptr->width;
   pen = polygon_ptr->pen;
   dash = polygon_ptr->dash;
   pixel = colorPixels[ObjPtr->color];

   if (NeedsToCachePolygonObj(ObjPtr) && polygon_ptr->rotated_vlist==NULL) {
      MakeCachedPolygon(ObjPtr);
   }
   if (userDisableRedraw) return;

   if ((fill == NONEPAT || (trans_pat && fill == BACKPAT)) &&
         (pen == NONEPAT || (trans_pat && pen == BACKPAT))) {
      return;
   }
   v = polygon_ptr->svlist;
   num_pts = polygon_ptr->sn;

   if (fill != 0) {
      values.foreground = GetDrawingBgPixel(fill, pixel);
      values.function = GXcopy;
      values.fill_style = (trans_pat ? FillStippled : FillOpaqueStippled);
      values.stipple = patPixmap[fill];
      XChangeGC(mainDisplay, drawGC,
            GCForeground | GCFunction | GCFillStyle | GCStipple, &values);
      if (ObjPtr->ctm == NULL) {
         XFillPolygon(mainDisplay, Win, drawGC, v, num_pts, Complex,
               CoordModeOrigin);
      } else {
         XFillPolygon(mainDisplay, Win, drawGC, polygon_ptr->rotated_vlist,
               polygon_ptr->rotated_n, Complex, CoordModeOrigin);
      }
   }

   if (pen == NONEPAT) return;

   values.foreground = GetDrawingBgPixel(pen, pixel);
   values.function = GXcopy;
   values.fill_style = (trans_pat ? FillStippled : FillOpaqueStippled);
   values.stipple = patPixmap[pen];
   values.line_width = ZOOMED_SIZE(width);
#ifdef NO_THIN_LINE
   if (values.line_width < 1) values.line_width = 1;
#endif
   values.join_style = JoinBevel;
   if (dash != 0) {
      XSetDashes(mainDisplay, drawGC, 0, dashList[dash],
            dashListLength[dash]);
      values.line_style = LineOnOffDash;
   } else {
      values.line_style = LineSolid;
   }
   XChangeGC(mainDisplay, drawGC,
         GCForeground | GCFunction | GCFillStyle | GCStipple |
         GCLineWidth | GCLineStyle | GCJoinStyle, &values);

   if (ObjPtr->ctm == NULL) {
      XDrawLines(mainDisplay, Win, drawGC, v, num_pts, CoordModeOrigin);
   } else {
      XDrawLines(mainDisplay, Win, drawGC, polygon_ptr->rotated_vlist,
            polygon_ptr->rotated_n, CoordModeOrigin);
   }
   values.join_style = JoinMiter;
   XChangeGC(mainDisplay, drawGC, GCJoinStyle, &values);
}

#define CREATE_RELATIVE (FALSE)
#define CREATE_ABSOLUTE (TRUE)

void CreatePolygonObj(NumPts, CreateAbsolute)
   int NumPts;
   int CreateAbsolute;
{
   register int i;
   struct PtRec *pt_ptr=NULL, *next_pt=NULL;
   struct PolygonRec *polygon_ptr=NULL;
   struct ObjRec *obj_ptr=NULL;
   IntPoint *v=NULL;
   int width, w, ltx, lty, rbx, rby, num_ss_pts=0;
   char *smooth=NULL;
   CVListElem *elem=NULL;
   IntPointTriplet *pipt=NULL;

   polygon_ptr = (struct PolygonRec *)malloc(sizeof(struct PolygonRec));
   if (polygon_ptr == NULL) FailAllocMessage();
   memset(polygon_ptr, 0, sizeof(struct PolygonRec));
   if (curSpline == LT_STRUCT_SPLINE) {
      num_ss_pts = ((NumPts-1)*3)+1;
      polygon_ptr->n = num_ss_pts;
      v = (IntPoint*)malloc((num_ss_pts+1)*sizeof(IntPoint));
      if (v == NULL) FailAllocMessage();
      smooth = (char*)malloc((num_ss_pts+1)*sizeof(char));
      if (smooth == NULL) FailAllocMessage();
      elem = ListLast(&gStructSplineList);
      pipt = (IntPointTriplet*)(elem->obj);
      ltx = rbx = pipt->hinge_pt.x;
      lty = rby = pipt->hinge_pt.y;

      for (i=NumPts-1; i >= 0; i--, elem=ListPrev(&gStructSplineList, elem)) {
         pipt = (IntPointTriplet*)(elem->obj);
         if (i == 0) {
            v[0].x = CreateAbsolute ? pipt->hinge_pt.x :
                  ABS_X(pipt->hinge_pt.x);
            v[0].y = CreateAbsolute ? pipt->hinge_pt.y :
                  ABS_Y(pipt->hinge_pt.y);
            if (pipt->later_valid) {
               v[1].x = CreateAbsolute ? pipt->later_smooth_pt.x :
                     ABS_X(pipt->later_smooth_pt.x);
               v[1].y = CreateAbsolute ? pipt->later_smooth_pt.y :
                     ABS_Y(pipt->later_smooth_pt.y);
               smooth[1] = TRUE;
            } else {
               v[1].x = v[0].x;
               v[1].y = v[0].y;
               smooth[1] = FALSE;
            }
            smooth[0] = FALSE;
         } else if (i == NumPts-1) {
            v[num_ss_pts-1].x = CreateAbsolute ? pipt->hinge_pt.x :
                  ABS_X(pipt->hinge_pt.x);
            v[num_ss_pts-1].y = CreateAbsolute ? pipt->hinge_pt.y :
                  ABS_Y(pipt->hinge_pt.y);
            if (pipt->earlier_valid) {
               v[num_ss_pts-2].x = CreateAbsolute ? pipt->earlier_smooth_pt.x :
                     ABS_X(pipt->earlier_smooth_pt.x);
               v[num_ss_pts-2].y = CreateAbsolute ? pipt->earlier_smooth_pt.y :
                     ABS_Y(pipt->earlier_smooth_pt.y);
               smooth[num_ss_pts-2] = TRUE;
            } else {
               v[num_ss_pts-2].x = v[num_ss_pts-1].x;
               v[num_ss_pts-2].y = v[num_ss_pts-1].y;
               smooth[num_ss_pts-2] = FALSE;
            }
            smooth[num_ss_pts-1] = FALSE;
         } else {
            int index=3*i;

            v[index-1].x = CreateAbsolute ? pipt->earlier_smooth_pt.x :
                  ABS_X(pipt->earlier_smooth_pt.x);
            v[index-1].y = CreateAbsolute ? pipt->earlier_smooth_pt.y :
                  ABS_Y(pipt->earlier_smooth_pt.y);
            v[index].x = CreateAbsolute ? pipt->hinge_pt.x :
                  ABS_X(pipt->hinge_pt.x);
            v[index].y = CreateAbsolute ? pipt->hinge_pt.y :
                  ABS_Y(pipt->hinge_pt.y);
            v[index+1].x = CreateAbsolute ? pipt->later_smooth_pt.x :
                  ABS_X(pipt->later_smooth_pt.x);
            v[index+1].y = CreateAbsolute ? pipt->later_smooth_pt.y :
                  ABS_Y(pipt->later_smooth_pt.y);
            smooth[index-1] = pipt->earlier_valid;
            smooth[index] = FALSE;
            smooth[index+1] = pipt->later_valid;
         }
         free(pipt);
      }
      for (i=0; i < num_ss_pts; i++) {
         if (v[i].x < ltx) ltx = v[i].x;
         if (v[i].y < lty) lty = v[i].y;
         if (v[i].x > rbx) rbx = v[i].x;
         if (v[i].y > rby) rby = v[i].y;
      }
      numPtsInPoly = 0;
      ListUnlinkAll(&gStructSplineList);
   } else {
      polygon_ptr->n = NumPts;
      v = (IntPoint*)malloc((NumPts+1)*sizeof(IntPoint));
      if (v == NULL) FailAllocMessage();
      if (curSpline != LT_INTSPLINE) {
         smooth = (char*)malloc((NumPts+1)*sizeof(char));
         if (smooth == NULL) FailAllocMessage();
      }
      pt_ptr = lastPtPtr;
      ltx = rbx = pt_ptr->x;
      lty = rby = pt_ptr->y;
      for (i=NumPts-1; i >= 0; i--, lastPtPtr = next_pt) {
         next_pt = lastPtPtr->next;
         v[i].x = CreateAbsolute ? lastPtPtr->x : ABS_X(lastPtPtr->x);
         v[i].y = CreateAbsolute ? lastPtPtr->y : ABS_Y(lastPtPtr->y);
         if (curSpline != LT_INTSPLINE) {
            if (lastPtPtr->x < ltx) ltx = lastPtPtr->x;
            if (lastPtPtr->y < lty) lty = lastPtPtr->y;
            if (lastPtPtr->x > rbx) rbx = lastPtPtr->x;
            if (lastPtPtr->y > rby) rby = lastPtPtr->y;
            smooth[i] = (curSpline != LT_STRAIGHT);
         }
         free(lastPtPtr);
      }
      numPtsInPoly = 0;
      lastPtPtr = NULL;
   }
   polygon_ptr->vlist = v;
   polygon_ptr->smooth = smooth;
   polygon_ptr->svlist = NULL;
   polygon_ptr->intvlist = NULL;
   polygon_ptr->fill = objFill;
   polygon_ptr->width = width = curWidthOfLine[lineWidth];
   UtilStrCpyN(polygon_ptr->width_spec, sizeof(polygon_ptr->width_spec),
         curWidthOfLineSpec[lineWidth]);
   polygon_ptr->pen = penPat;
   polygon_ptr->curved = curSpline;
   polygon_ptr->dash = curDash;
   /*
    * polygon_ptr->tighter = (curSpline == LT_STRUCT_SPLINE) ?
    *       tighterStructSplines : FALSE;
    */
   polygon_ptr->rotated_n = 0;
   polygon_ptr->rotated_vlist = NULL;

   obj_ptr = (struct ObjRec *)malloc(sizeof(struct ObjRec));
   if (obj_ptr == NULL) FailAllocMessage();
   memset(obj_ptr, 0, sizeof(struct ObjRec));
   obj_ptr->detail.g = polygon_ptr;

   obj_ptr->color = colorIndex;
   if (mainDisplay != NULL) {
      UtilStrCpyN(obj_ptr->color_str, sizeof(obj_ptr->color_str),
            colorMenuItems[colorIndex]);
   }
   obj_ptr->type = OBJ_POLYGON;
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
   obj_ptr->id = objId++;
   obj_ptr->dirty = FALSE;
   obj_ptr->rotation = 0;
   obj_ptr->locked = FALSE;
   obj_ptr->fattr = obj_ptr->lattr = NULL;
   obj_ptr->ctm = NULL;
   obj_ptr->invisible = FALSE;
   obj_ptr->trans_pat = transPat;

   AdjObjSplineVs(obj_ptr);
   if (curSpline != LT_INTSPLINE) {
      UpdPolyBBox(obj_ptr, polygon_ptr->n, polygon_ptr->vlist);
   } else {
      UpdPolyBBox(obj_ptr, polygon_ptr->intn, polygon_ptr->intvlist);
   }
   AdjObjBBox(obj_ptr);
   AddObj(NULL, topObj, obj_ptr);
}

void ResetCreatePolygon()
{
   numPtsInPoly = 0;
   lastPtPtr = NULL;
}

void AddPtToCreatePolygon(AbsX, AbsY)
   int AbsX, AbsY;
{
   struct PtRec *pt_ptr=(struct PtRec *)malloc(sizeof(struct PtRec));

   if (pt_ptr == NULL) FailAllocMessage();
   memset(pt_ptr, 0, sizeof(struct PtRec));
   pt_ptr->next = lastPtPtr;
   numPtsInPoly++;
   lastPtPtr = pt_ptr;
   pt_ptr->x = AbsX;
   pt_ptr->y = AbsY;
}

#define POLYGON_DRAW  (FALSE)
#define POLYGON_ERASE (TRUE)
#define POLYGON_CLICK (FALSE)
#define POLYGON_DRAG  (TRUE)

#define POLYGON_STARTSHOW 0
#define POLYGON_DOSHOW    1
#define POLYGON_ENDSHOW   2

typedef struct tagPolygonMeasureCursorInfo {
   IntPoint first_pt;
   IntPoint prev_prev_pt;
   IntPoint prev_pt;
} PolygonMeasureCursorInfo;

static
void DoPolygonMeasureCursor(ppmci, start, num_pts, x, y, dx, dy, erase, drag,
      closed)
   PolygonMeasureCursorInfo *ppmci;
   int start, num_pts, x, y, dx, dy, erase, drag, closed;
{
   char buf[80], w_buf[80], h_buf[80], x_buf[80], y_buf[80], a_buf[80];
   int angle2=0;

   if (erase == POLYGON_DRAW && drag == POLYGON_CLICK) {
      if (num_pts == 1) {
         ppmci->first_pt.x = ppmci->prev_pt.x = ppmci->prev_prev_pt.x = x;
         ppmci->first_pt.y = ppmci->prev_pt.y = ppmci->prev_prev_pt.y = y;
      } else {
         ppmci->prev_prev_pt.x = ppmci->prev_pt.x;
         ppmci->prev_prev_pt.y = ppmci->prev_pt.y;
         ppmci->prev_pt.x = x;
         ppmci->prev_pt.y = y;
      }
   }
   if (x == ppmci->prev_pt.x && y == ppmci->prev_pt.y) {
      strcpy(a_buf, "0");
   } else if (num_pts == 1) {
      PointsToArc(ppmci->prev_pt.x, ppmci->prev_pt.y, ppmci->prev_pt.x+100,
            ppmci->prev_pt.y, x, y, ARC_CCW, FALSE, NULL, NULL, NULL, NULL,
            NULL, &angle2);
      if (angle2 > 180*64) angle2=(360*64)-angle2;
      FormatAngle(angle2, a_buf);
   } else if (closed) {
      if (x == ppmci->first_pt.x && y == ppmci->first_pt.y) {
         strcpy(a_buf, "0");
      } else {
         PointsToArc(x, y, ppmci->first_pt.x, ppmci->first_pt.y,
               ppmci->prev_pt.x, ppmci->prev_pt.y, ARC_CCW, FALSE, NULL, NULL,
               NULL, NULL, NULL, &angle2);
         if (angle2 > 180*64) angle2=(360*64)-angle2;
         FormatAngle(angle2, a_buf);
      }
   } else {
      PointsToArc(ppmci->prev_pt.x, ppmci->prev_pt.y, ppmci->prev_prev_pt.x,
            ppmci->prev_prev_pt.y, x, y, ARC_CCW, FALSE, NULL, NULL, NULL, NULL,
            NULL, &angle2);
      if (angle2 > 180*64) angle2=(360*64)-angle2;
      FormatAngle(angle2, a_buf);
   }
   PixelToMeasurementUnit(w_buf, dx);
   PixelToMeasurementUnit(h_buf, dy);
   PixelToMeasurementUnit(x_buf, x);
   PixelToMeasurementUnit(y_buf, y);
   sprintf(buf, "w=%s\nh=%s\nx=%s\ny=%s\nangle=%s", w_buf, h_buf, x_buf, y_buf,
         a_buf);

   x = OFFSET_X(x);
   y = OFFSET_Y(y);
   switch (start) {
   case POLYGON_STARTSHOW: StartShowMeasureCursor(x, y, buf, TRUE); break;
   case POLYGON_DOSHOW: ShowMeasureCursor(x, y, buf, TRUE); break;
   case POLYGON_ENDSHOW: EndShowMeasureCursor(x, y, buf, TRUE); break;
   }
}

static
void EraseStructSplineLinesForCont(OrigX, OrigY, grid_x, grid_y, sv, sn, sv2,
      sn2)
   int OrigX, OrigY, grid_x, grid_y, sn, sn2;
   XPoint *sv, *sv2;
{
   if (sv == NULL && sv2 == NULL) {
      XDrawLine(mainDisplay, drawWindow, drawGC, OrigX, OrigY, grid_x, grid_y);
   } else  {
      if (sv != NULL) {
         XDrawLines(mainDisplay, drawWindow, drawGC, sv, sn, CoordModeOrigin);
      }
      if (sv2 != NULL) {
         XDrawLines(mainDisplay, drawWindow, drawGC, sv2, sn2, CoordModeOrigin);
      }
   }
}

static
void ContinueForStructSplinePolygonControlPoints(OrigX, OrigY, LastX, LastY,
      psv, psn, psv2, psn2, pipt_first, pipt_prev, pipt, pn_abort)
   int OrigX, OrigY, LastX, LastY, *psn, *psn2, *pn_abort;
   XPoint **psv, **psv2;
   IntPointTriplet *pipt_first, *pipt_prev, *pipt;
{
   int i=0, done=FALSE, grid_x=0, grid_y=0, end_x=0, end_y=0, n=2;
   int sn=(*psn), orig_sn=(*psn), sn2=(*psn2), orig_sn2=(*psn2);
   int num_pts=1, first_time=TRUE;
   IntPoint v[4];
   XPoint *sv=NULL, *orig_sv=(*psv), *sv2=NULL, *orig_sv2=(*psv2), dash_vs[2];
   PolygonMeasureCursorInfo pmci;
   XGCValues values;

   *pn_abort = FALSE;

   if (orig_sv != NULL) {
      sv = (XPoint*)malloc(orig_sn*sizeof(XPoint));
      if (sv == NULL) FailAllocMessage();
      memset(sv, 0, orig_sn*sizeof(XPoint));
      for (i=0; i < orig_sn; i++) {
         sv[i].x = orig_sv[i].x;
         sv[i].y = orig_sv[i].y;
      }
      sn = orig_sn;
   }
   if (orig_sv2 != NULL) {
      sv2 = (XPoint*)malloc(orig_sn2*sizeof(XPoint));
      if (sv2 == NULL) FailAllocMessage();
      memset(sv2, 0, orig_sn2*sizeof(XPoint));
      for (i=0; i < orig_sn2; i++) {
         sv2[i].x = orig_sv2[i].x;
         sv2[i].y = orig_sv2[i].y;
      }
      sn2 = orig_sn2;
   }
   memset(pipt, 0, sizeof(IntPointTriplet));
   pipt->earlier_valid = pipt->later_valid = FALSE;
   pipt->hinge_pt.x = pipt->earlier_smooth_pt.x = pipt->later_smooth_pt.x =
         LastX;
   pipt->hinge_pt.y = pipt->earlier_smooth_pt.y = pipt->later_smooth_pt.y =
         LastY;
   pipt->ratio = (double)1;

   XSetDashes(mainDisplay, revDefaultGC, 0, dashList[8], dashListLength[8]);

   grid_x = end_x = dash_vs[0].x = dash_vs[1].x = LastX;
   grid_y = end_y = dash_vs[0].y = dash_vs[1].y = LastY;
   v[0].x = ABS_X(OrigX);
   v[0].y = ABS_Y(OrigY);
   v[1].x = ABS_X(LastX);
   v[1].y = ABS_Y(LastY);
   n = 2;
   MARKHR(drawWindow, revDefaultGC, LastX, LastY);

   /* the previous curve has already been drawn */
   /* draw the measure cursor */
   DoPolygonMeasureCursor(&pmci, POLYGON_DOSHOW, num_pts, ABS_X(grid_x),
         ABS_Y(grid_y), 0, 0, POLYGON_ERASE, POLYGON_DRAG);
   while (!done) {
      XEvent input, ev;

      XNextEvent(mainDisplay, &input);

      if (input.type == Expose || input.type == VisibilityNotify) {
         ExposeEventHandler(&input, TRUE);
         SetXorDrawGC(colorIndex);
      } else if (input.type == MotionNotify) {
         end_x = input.xmotion.x;
         end_y = input.xmotion.y;

         /* erase */
         if (first_time || grid_x != LastX || grid_y != LastY) {
            EraseStructSplineLinesForCont(OrigX, OrigY, grid_x, grid_y, sv, sn,
                  sv2, sn2);
            if (!pipt_first->earlier_valid && sv != NULL && sv2 == NULL) {
               XDrawLine(mainDisplay, drawWindow, drawGC, grid_x, grid_y,
                     pipt_first->earlier_smooth_pt.x,
                     pipt_first->earlier_smooth_pt.y);
            }
            first_time = FALSE;
         } else {
            if (orig_sv != NULL || orig_sv2 != NULL) {
               if (orig_sv != NULL) {
                  XDrawLines(mainDisplay, drawWindow, drawGC, orig_sv, orig_sn,
                        CoordModeOrigin);
               }
               if (orig_sv2 != NULL) {
                  XDrawLines(mainDisplay, drawWindow, drawGC, orig_sv2,
                        orig_sn2, CoordModeOrigin);
               }
            } else {
               XDrawLine(mainDisplay, drawWindow, drawGC, OrigX, OrigY, LastX,
                     LastY);
            }
         }
         DoPolygonMeasureCursor(&pmci, POLYGON_DOSHOW, num_pts, ABS_X(grid_x),
               ABS_Y(grid_y), ABS_SIZE(abs(grid_x-LastX)),
               ABS_SIZE(abs(grid_y-LastY)), POLYGON_ERASE, POLYGON_DRAG);
         MARKHO(drawWindow, revDefaultGC, dash_vs[0].x, dash_vs[0].y);
         MARKHO(drawWindow, revDefaultGC, dash_vs[1].x, dash_vs[1].y);
         values.line_style = LineOnOffDash;
         XChangeGC(mainDisplay, revDefaultGC, GCLineStyle, &values);
         MyDashedLine(drawWindow, revDefaultGC, dash_vs, 2);
         values.line_style = LineSolid;
         XChangeGC(mainDisplay, revDefaultGC, GCLineStyle, &values);

         /* draw */
         GridXY(end_x, end_y, &grid_x, &grid_y);
         MarkRulers(grid_x, grid_y);
         if (grid_x != LastX || grid_y != LastY) {
            if (sv != NULL) {
               free(sv);
               sv = NULL;
            }
            if (sv2 != NULL) {
               free(sv2);
               sv2 = NULL;
            }
            v[0].x = ABS_X(OrigX);
            v[0].y = ABS_Y(OrigY);
            if (pipt_prev->later_valid) {
               v[1].x = ABS_X(pipt_prev->later_smooth_pt.x);
               v[1].y = ABS_Y(pipt_prev->later_smooth_pt.y);
               n = 4;
            } else {
               n = 3;
            }
            v[n-2].x = ABS_X((LastX<<1)-grid_x);
            v[n-2].y = ABS_Y((LastY<<1)-grid_y);
            v[n-1].x = ABS_X(LastX);
            v[n-1].y = ABS_Y(LastY);
            sv = MakeSplinePolyVertex(0, curSpline, &sn, drawOrigX, drawOrigY,
                  n, v);
            XDrawLines(mainDisplay, drawWindow, drawGC, sv, sn,
                  CoordModeOrigin);

            v[0].x = ABS_X(pipt_first->hinge_pt.x);
            v[0].y = ABS_Y(pipt_first->hinge_pt.y);
            if (pipt_first->earlier_valid) {
               v[1].x = ABS_X(pipt_first->earlier_smooth_pt.x);
               v[1].y = ABS_Y(pipt_first->earlier_smooth_pt.y);
               n = 4;
            } else {
               n = 3;
            }
            v[n-2].x = ABS_X(grid_x);
            v[n-2].y = ABS_Y(grid_y);
            v[n-1].x = ABS_X(LastX);
            v[n-1].y = ABS_Y(LastY);
            sv2 = MakeSplinePolyVertex(0, curSpline, &sn2, drawOrigX, drawOrigY,
                  n, v);
            XDrawLines(mainDisplay, drawWindow, drawGC, sv2, sn2,
                  CoordModeOrigin);
         } else {
            if (orig_sv != NULL || orig_sv2 != NULL) {
               if (orig_sv != NULL) {
                  XDrawLines(mainDisplay, drawWindow, drawGC, orig_sv, orig_sn,
                        CoordModeOrigin);
               }
               if (orig_sv2 != NULL) {
                  XDrawLines(mainDisplay, drawWindow, drawGC, orig_sv2,
                        orig_sn2, CoordModeOrigin);
               }
            } else {
               XDrawLine(mainDisplay, drawWindow, drawGC, OrigX, OrigY, LastX,
                     LastY);
            }
         }
         while (XCheckMaskEvent(mainDisplay, PointerMotionMask, &ev)) ;

         dash_vs[0].x = grid_x;
         dash_vs[0].y = grid_y;
         dash_vs[1].x = (LastX<<1)-grid_x;
         dash_vs[1].y = (LastY<<1)-grid_y;
         pipt->later_smooth_pt.x = dash_vs[0].x;
         pipt->later_smooth_pt.y = dash_vs[0].y;
         pipt->earlier_smooth_pt.x = dash_vs[1].x;
         pipt->earlier_smooth_pt.y = dash_vs[1].y;
         if (grid_x == LastX && grid_y == LastY) {
            pipt->earlier_valid = pipt->later_valid = FALSE;
         } else {
            pipt->earlier_valid = pipt->later_valid = TRUE;
         }
         MARKHO(drawWindow, revDefaultGC, dash_vs[0].x, dash_vs[0].y);
         MARKHO(drawWindow, revDefaultGC, dash_vs[1].x, dash_vs[1].y);
         values.line_style = LineOnOffDash;
         XChangeGC(mainDisplay, revDefaultGC, GCLineStyle, &values);
         MyDashedLine(drawWindow, revDefaultGC, dash_vs, 2);
         values.line_style = LineSolid;
         XChangeGC(mainDisplay, revDefaultGC, GCLineStyle, &values);
         DoPolygonMeasureCursor(&pmci, POLYGON_DOSHOW, num_pts, ABS_X(grid_x),
               ABS_Y(grid_y), ABS_SIZE(abs(grid_x-LastX)),
               ABS_SIZE(abs(grid_y-LastY)), POLYGON_DRAW, POLYGON_DRAG);
      } else if (input.type == KeyPress) {
         if (KeyPressEventIsEscape(&input.xkey)) {
            /* erase */
            DoPolygonMeasureCursor(&pmci, POLYGON_ENDSHOW, num_pts,
                  ABS_X(grid_x), ABS_Y(grid_y), ABS_SIZE(abs(grid_x-LastX)),
                  ABS_SIZE(abs(grid_y-LastY)), POLYGON_ERASE, POLYGON_DRAG);
            MARKHO(drawWindow, revDefaultGC, dash_vs[0].x, dash_vs[0].y);
            MARKHO(drawWindow, revDefaultGC, dash_vs[1].x, dash_vs[1].y);
            values.line_style = LineOnOffDash;
            XChangeGC(mainDisplay, revDefaultGC, GCLineStyle, &values);
            MyDashedLine(drawWindow, revDefaultGC, dash_vs, 2);
            values.line_style = LineSolid;
            XChangeGC(mainDisplay, revDefaultGC, GCLineStyle, &values);
            Msg("");
            *pn_abort = TRUE;
            done = TRUE;
         }
      } else if (input.type == ButtonRelease) {
         /* erase */
         DoPolygonMeasureCursor(&pmci, POLYGON_ENDSHOW, num_pts, ABS_X(grid_x),
               ABS_Y(grid_y), ABS_SIZE(abs(grid_x-LastX)),
               ABS_SIZE(abs(grid_y-LastY)), POLYGON_ERASE, POLYGON_DRAG);
         MARKHO(drawWindow, revDefaultGC, dash_vs[0].x, dash_vs[0].y);
         MARKHO(drawWindow, revDefaultGC, dash_vs[1].x, dash_vs[1].y);
         values.line_style = LineOnOffDash;
         XChangeGC(mainDisplay, revDefaultGC, GCLineStyle, &values);
         MyDashedLine(drawWindow, revDefaultGC, dash_vs, 2);
         values.line_style = LineSolid;
         XChangeGC(mainDisplay, revDefaultGC, GCLineStyle, &values);

         GridXY(input.xbutton.x, input.xbutton.y, &grid_x, &grid_y);
         pipt->later_smooth_pt.x = grid_x;
         pipt->later_smooth_pt.y = grid_y;
         pipt->earlier_smooth_pt.x = (LastX<<1)-grid_x;
         pipt->earlier_smooth_pt.y = (LastY<<1)-grid_y;
         if (grid_x == LastX && grid_y == LastY) {
            pipt->earlier_valid = pipt->later_valid = FALSE;
            if (sv2 != NULL) {
               free(sv2);
               sv2 = NULL;

               v[0].x = ABS_X(pipt_first->hinge_pt.x);
               v[0].y = ABS_Y(pipt_first->hinge_pt.y);
               if (pipt_first->earlier_valid) {
                  v[1].x = ABS_X(pipt_first->earlier_smooth_pt.x);
                  v[1].y = ABS_Y(pipt_first->earlier_smooth_pt.y);
                  n = 3;
               } else {
                  n = 2;
               }
               v[n-1].x = ABS_X(LastX);
               v[n-1].y = ABS_Y(LastY);
               if (n > 2) {
                  sv2 = MakeSplinePolyVertex(0, curSpline, &sn2, drawOrigX,
                        drawOrigY, n, v);
               } else {
                  XDrawLine(mainDisplay, drawWindow, drawGC,
                        pipt_first->hinge_pt.x, pipt_first->hinge_pt.y,
                        LastX, LastY);
               }
            }
         } else {
            pipt->earlier_valid = pipt->later_valid = TRUE;
         }
         done = TRUE;
      }
   }
   MARKHR(drawWindow, revDefaultGC, LastX, LastY);

   *psv = sv;
   *psn = sn;
   *psv2 = sv2;
   *psn2 = sn2;

   if (orig_sv != NULL) free(orig_sv);
   if (orig_sv2 != NULL) free(orig_sv2);
}

static
void EraseStructSplineLinesForContAndUpdateSvs(OrigX, OrigY, grid_x, grid_y,
      pev, num_pts, psv, sn, psv2, sn2, pipt_prev, pipt_first)
   int OrigX, OrigY, grid_x, grid_y, sn, sn2;
   XEvent *pev;
   XPoint **psv, **psv2;
   IntPointTriplet *pipt_prev, *pipt_first;
{
   XPoint *sv=(*psv), *sv2=(*psv2);

   /* erase */
   if (sv == NULL && sv2 != NULL && !pipt_prev->later_valid) {
      /* erase the straight line */
      XDrawLine(mainDisplay, drawWindow, drawGC,
            pipt_prev->later_smooth_pt.x, pipt_prev->later_smooth_pt.y,
            grid_x, grid_y);
   }
   EraseStructSplineLinesForCont(OrigX, OrigY, grid_x, grid_y, sv, sn, sv2,
         sn2);
   if ((pev->type == MotionNotify && num_pts > 1) ||
         (pev->type == ButtonPress && num_pts > 2)) {
      if (pipt_first->earlier_valid) {
         /* don't need to do anything here */
      } else {
         XDrawLine(mainDisplay, drawWindow, drawGC, pipt_first->hinge_pt.x,
               pipt_first->hinge_pt.y, grid_x, grid_y);
      }
   }
   if (sv != NULL) free(sv);
   if (sv2 != NULL) free(sv2);
   *psv = *psv2 = NULL;
}

static
int CountStructuredSplinePolygonVs()
{
   int i=0, len=ListLength(&gStructSplineList), num_vs=0;
   CVListElem *elem=NULL;

   for (i=0, elem=ListFirst(&gStructSplineList); elem != NULL;
         i++, elem=ListNext(&gStructSplineList, elem)) {
      IntPointTriplet *ipt=(IntPointTriplet*)(elem->obj);

      if (i == 0) {
         if (ipt->later_valid) {
            num_vs++;
         }
      } else if (i == len-1) {
         if (ipt->earlier_valid) {
            num_vs++;
         }
      } else {
         if (ipt->later_valid) {
            num_vs++;
         }
         if (ipt->earlier_valid) {
            num_vs++;
         }
      }
      num_vs++;
   }
   return num_vs;
}

#ifdef _TGIF_DBG
void sync()
{
   XSync(mainDisplay,0);
}
#endif /* _TGIF_DBG */

static XComposeStatus c_stat;

static
void ContinuePolygon(OrigX, OrigY)
   int  OrigX, OrigY;
   /* OrigX and OrigY are screen coordinates (scaled and translated). */
{
   register int i;
   XEvent input, ev;
   XButtonEvent *button_ev;
   XMotionEvent *motion_ev;
   KeySym key_sym;
   int start_polygon_x=OrigX, start_polygon_y=OrigY;
   int end_x, end_y, grid_x, grid_y, done=FALSE;
   int saved_x, saved_y, closed=FALSE, abort=FALSE;
   int num_pts=1, n=2, sn=0, sn2=0, max_n=40, intn=0, tmp_n=0;
   int ltx=OrigX, lty=OrigY, rbx=OrigX, rby=OrigY, nothing_is_drawn=TRUE;
   XPoint *sv=NULL, *sv2=NULL;
   IntPoint *pv=NULL, *cntrlv=NULL, tmp_vs[4];
   IntPointTriplet ipt_prev, ipt, first_ipt;
   PolygonMeasureCursorInfo pmci;

   memset(&ipt, 0, sizeof(IntPointTriplet));
   memset(&ipt_prev, 0, sizeof(IntPointTriplet));
   memset(&first_ipt, 0, sizeof(IntPointTriplet));
   memset(&pmci, 0, sizeof(PolygonMeasureCursorInfo));
   SetXorDrawGC(xorColorPixels[colorIndex]);

   grid_x = end_x = saved_x = OrigX;
   grid_y = end_y = saved_y = OrigY;
   if (curSpline != LT_STRAIGHT && curSpline != LT_STRUCT_SPLINE) {
      pv = (IntPoint*)malloc((max_n+1)*sizeof(IntPoint));
      if (pv == NULL) FailAllocMessage();
      pv[0].x = pv[1].x = pv[2].x = ABS_X(OrigX);
      pv[0].y = pv[1].y = pv[2].y = ABS_Y(OrigY);
      switch (curSpline) {
      case LT_SPLINE:
         sv = MakeSplinePolygonVertex(0, curSpline, &sn, drawOrigX, drawOrigY,
               n+1, pv);
         break;
      case LT_INTSPLINE:
         sv = MakeIntSplinePolygonVertex(&sn, &intn, &cntrlv,
               drawOrigX, drawOrigY, n+1, pv);
         for (i=0; i < sn; i++) {
            if (sv[i].x < ltx) ltx = sv[i].x;
            if (sv[i].y < lty) lty = sv[i].y;
            if (sv[i].x > rbx) rbx = sv[i].x;
            if (sv[i].y > rby) rby = sv[i].y;
         }
         break;
      }
   } else if (curSpline == LT_STRUCT_SPLINE) {
      ipt_prev.earlier_valid = ipt_prev.later_valid = FALSE;
      ipt_prev.earlier_smooth_pt.x = ipt_prev.hinge_pt.x =
            ipt_prev.later_smooth_pt.x = OrigX;
      ipt_prev.earlier_smooth_pt.y = ipt_prev.hinge_pt.y =
            ipt_prev.later_smooth_pt.y = OrigY;
      memcpy(&first_ipt, &ipt_prev, sizeof(IntPointTriplet));
      pv = (IntPoint*)malloc((max_n+1)*sizeof(IntPoint));
      if (pv == NULL) FailAllocMessage();
      pv[0].x = pv[1].x = pv[2].x = pv[3].x = ABS_X(OrigX);
      pv[0].y = pv[1].y = pv[2].y = pv[3].y = ABS_Y(OrigY);
      sv = MakeSplinePolyVertex(0, curSpline, &sn, drawOrigX, drawOrigY, n+1,
            pv);
   }
   SaveStatusStrings();
   if (curSpline == LT_STRUCT_SPLINE) {
      SetMouseStatus(TgLoadCachedString(CSTID_ADD_A_STRUCT_VERTEX),
         TgLoadCachedString(CSTID_CLOSE_POLYGON_SPLINE),
         TgLoadCachedString(CSTID_CLOSE_POLYGON_SPLINE));
   } else {
      SetMouseStatus(TgLoadCachedString(CSTID_ADD_A_VERTEX),
         TgLoadCachedString(CSTID_CLOSE_POLYGON_SPLINE),
         TgLoadCachedString(CSTID_CLOSE_POLYGON_SPLINE));
   }
   DoPolygonMeasureCursor(&pmci, POLYGON_STARTSHOW, num_pts, ABS_X(grid_x),
         ABS_Y(grid_y), 0, 0, POLYGON_DRAW, POLYGON_CLICK, closed);
   if (!debugNoPointerGrab) {
      XGrabPointer(mainDisplay, drawWindow, FALSE,
            PointerMotionMask | ButtonPressMask | ButtonReleaseMask,
            GrabModeAsync, GrabModeAsync, None, handCursor, CurrentTime);
   }
   if (curSpline == LT_STRUCT_SPLINE) {
      DoPolygonMeasureCursor(&pmci, POLYGON_DOSHOW, num_pts, ABS_X(grid_x),
            ABS_Y(grid_y), 0, 0, POLYGON_ERASE, POLYGON_DRAG, closed);
      ContinueForStructSplinePolygonControlPoints(OrigX, OrigY,
            grid_x, grid_y, &sv, &sn, &sv2, &sn2, &first_ipt,
            &ipt_prev, &ipt, &abort);
      memcpy(&first_ipt, &ipt, sizeof(IntPointTriplet));
      memcpy(&ipt_prev, &ipt, sizeof(IntPointTriplet));
      if (first_ipt.later_valid) {
         grid_x = OFFSET_X(first_ipt.later_smooth_pt.x);
         grid_y = OFFSET_Y(first_ipt.later_smooth_pt.y);
      }
      SetFirstPoint(OrigX, OrigY, &first_ipt);
      DoPolygonMeasureCursor(&pmci, POLYGON_STARTSHOW, num_pts, ABS_X(grid_x),
            ABS_Y(grid_y), ABS_SIZE(abs(grid_x-OrigX)),
            ABS_SIZE(abs(grid_y-OrigY)), POLYGON_DRAW,
            POLYGON_CLICK, closed);
   } else {
      SetFirstPoint(OrigX, OrigY, NULL);
   }
   while (!done && !abort) {
      XNextEvent(mainDisplay, &input);

      if (input.type == Expose || input.type == VisibilityNotify) {
         ExposeEventHandler(&input, TRUE);
         SetXorDrawGC(xorColorPixels[colorIndex]);
      } else if ((!(shiftForDiagMouseMove && DiagEventCheck(&input))) &&
            (input.type == KeyPress || input.type == KeyRelease)) {
         char s[80];
         int has_ch=FALSE;

         has_ch = XLookupString(&(input.xkey), s, sizeof(s), &key_sym, &c_stat);
         if (num_pts > 1 && !(curSpline != LT_STRAIGHT &&
               curSpline != LT_STRUCT_SPLINE) &&
               (key_sym == XK_Control_L || key_sym == XK_Control_R)) {
            XDrawLine(mainDisplay, drawWindow, drawGC, saved_x, saved_y,
                  grid_x, grid_y);
            /* erase */
            DoPolygonMeasureCursor(&pmci, POLYGON_DOSHOW, num_pts,
                  ABS_X(grid_x), ABS_Y(grid_y), ABS_SIZE(abs(grid_x-OrigX)),
                  ABS_SIZE(abs(grid_y-OrigY)), POLYGON_ERASE, POLYGON_DRAG,
                  closed);
            if (ControlKeyPressedEvent(&input)) {
               closed = TRUE;
            } else {
               closed = FALSE;
            }
            /* draw */
            DoPolygonMeasureCursor(&pmci, POLYGON_DOSHOW, num_pts,
                  ABS_X(grid_x), ABS_Y(grid_y), ABS_SIZE(abs(grid_x-OrigX)),
                  ABS_SIZE(abs(grid_y-OrigY)), POLYGON_DRAW, POLYGON_DRAG,
                  closed);
         } else if (CharIsESC(&(input.xkey), s, key_sym, &has_ch)) {
            /* erase */
            DoPolygonMeasureCursor(&pmci, POLYGON_ENDSHOW, num_pts,
                  ABS_X(grid_x), ABS_Y(grid_y), ABS_SIZE(abs(grid_x-OrigX)),
                  ABS_SIZE(abs(grid_y-OrigY)), POLYGON_ERASE, POLYGON_CLICK,
                  closed);
            abort = TRUE;
            done = TRUE;
         }
      } else if ((input.type==MotionNotify && input.xany.window==drawWindow) ||
            (shiftForDiagMouseMove && DiagEventCheck(&input) &&
            (input.type == KeyPress || input.type == KeyRelease))) {
         unsigned int event_state=0;

         /* erase */
         DoPolygonMeasureCursor(&pmci, POLYGON_DOSHOW, num_pts, ABS_X(grid_x),
               ABS_Y(grid_y), ABS_SIZE(abs(grid_x-OrigX)),
               ABS_SIZE(abs(grid_y-OrigY)), POLYGON_ERASE, POLYGON_DRAG,
               closed);
         if (curSpline != LT_STRAIGHT && curSpline != LT_STRUCT_SPLINE) {
            XDrawLines(mainDisplay, drawWindow, drawGC, sv, sn,
                  CoordModeOrigin);
         } else if (curSpline == LT_STRUCT_SPLINE) {
            if (nothing_is_drawn) {
               /* then there is nothing to erase */
               nothing_is_drawn = FALSE;
            } else {
               EraseStructSplineLinesForContAndUpdateSvs(OrigX, OrigY, grid_x,
                     grid_y, &input, num_pts, &sv, sn, &sv2, sn2, &ipt_prev,
                     &first_ipt);
            }
         } else {
            XDrawLine(mainDisplay, drawWindow, drawGC, OrigX, OrigY, grid_x,
                  grid_y);
         }
         if (num_pts > 1 && closed) {
            XDrawLine(mainDisplay, drawWindow, drawGC, saved_x, saved_y,
                  grid_x, grid_y);
         }
         if (input.type == KeyPress || input.type == KeyRelease) {
            end_x = grid_x;
            end_y = grid_y;
            event_state = input.xkey.state;
         } else {
            motion_ev = &(input.xmotion);
            end_x = motion_ev->x;
            end_y = motion_ev->y;
            event_state = motion_ev->state;
         }
         if (shiftForDiagMouseMove && DiagEventCheck(&input)) {
            if (input.type == KeyRelease) {
               end_x = input.xkey.x;
               end_y = input.xkey.y;
            } else {
               DiagGridXY(OrigX, OrigY, &end_x, &end_y);
            }
         }
         /* draw */
         GridXY(end_x, end_y, &grid_x, &grid_y);
         MarkRulers(grid_x, grid_y);
         if (curSpline != LT_STRAIGHT && curSpline != LT_STRUCT_SPLINE) {
            if (sv != NULL) {
               free(sv);
               sv = NULL;
            }
            pv[n-1].x = ABS_X(grid_x);
            pv[n-1].y = ABS_Y(grid_y);
            switch (curSpline) {
            case LT_SPLINE:
               sv = MakeSplinePolygonVertex(0, curSpline, &sn, drawOrigX,
                     drawOrigY, n+1, pv);
               break;
            case LT_INTSPLINE:
               if (cntrlv != NULL) free(cntrlv);
               sv = MakeIntSplinePolygonVertex(&sn, &intn, &cntrlv,
                     drawOrigX, drawOrigY, n+1, pv);
               for (i=0; i < sn; i++) {
                  if (sv[i].x < ltx) ltx = sv[i].x;
                  if (sv[i].y < lty) lty = sv[i].y;
                  if (sv[i].x > rbx) rbx = sv[i].x;
                  if (sv[i].y > rby) rby = sv[i].y;
               }
               break;
            }
            XDrawLines(mainDisplay, drawWindow, drawGC, sv, sn,
                  CoordModeOrigin);
         } else if (curSpline == LT_STRUCT_SPLINE) {
            if (sv != NULL) {
               free(sv);
               sv = NULL;
            }
            if (sv2 != NULL) {
               free(sv2);
               sv2 = NULL;
            }
            if (ipt_prev.later_valid) {
               pv[0].x = ABS_X(OrigX);
               pv[0].y = ABS_Y(OrigY);
               pv[1].x = ABS_X(ipt_prev.later_smooth_pt.x);
               pv[1].y = ABS_Y(ipt_prev.later_smooth_pt.y);
               pv[2].x = pv[3].x = ABS_X(grid_x);
               pv[2].y = pv[3].y = ABS_Y(grid_y);
               n = 3;
               sv = MakeSplinePolyVertex(0, curSpline, &sn, drawOrigX,
                     drawOrigY, n, pv);
               XDrawLines(mainDisplay, drawWindow, drawGC, sv, sn,
                     CoordModeOrigin);
               if (first_ipt.earlier_valid) {
                  tmp_n = 3;
                  tmp_vs[0].x = ABS_X(grid_x);
                  tmp_vs[0].y = ABS_Y(grid_y);
                  tmp_vs[1].x = ABS_X(first_ipt.earlier_smooth_pt.x);
                  tmp_vs[1].y = ABS_Y(first_ipt.earlier_smooth_pt.y);
                  tmp_vs[2].x = ABS_X(first_ipt.hinge_pt.x);
                  tmp_vs[2].y = ABS_Y(first_ipt.hinge_pt.y);
                  sv2 = MakeSplinePolyVertex(0, curSpline, &sn2,
                        drawOrigX, drawOrigY, tmp_n, tmp_vs);
                  XDrawLines(mainDisplay, drawWindow, drawGC, sv2, sn2,
                        CoordModeOrigin);
               } else {
                  XDrawLine(mainDisplay, drawWindow, drawGC,
                        first_ipt.hinge_pt.x, first_ipt.hinge_pt.y,
                        grid_x, grid_y);
               }
            } else {
               XDrawLine(mainDisplay, drawWindow, drawGC, OrigX, OrigY,
                     grid_x, grid_y);
               if (num_pts > 1) {
                  if (first_ipt.earlier_valid) {
                     tmp_n = 3;
                     tmp_vs[0].x = ABS_X(grid_x);
                     tmp_vs[0].y = ABS_Y(grid_y);
                     tmp_vs[1].x = ABS_X(first_ipt.earlier_smooth_pt.x);
                     tmp_vs[1].y = ABS_Y(first_ipt.earlier_smooth_pt.y);
                     tmp_vs[2].x = ABS_X(first_ipt.hinge_pt.x);
                     tmp_vs[2].y = ABS_Y(first_ipt.hinge_pt.y);
                     sv2 = MakeSplinePolyVertex(0, curSpline, &sn2,
                           drawOrigX, drawOrigY, tmp_n, tmp_vs);
                     XDrawLines(mainDisplay, drawWindow, drawGC, sv2, sn2,
                           CoordModeOrigin);
                  } else {
                     XDrawLine(mainDisplay, drawWindow, drawGC,
                           first_ipt.hinge_pt.x, first_ipt.hinge_pt.y, grid_x,
                           grid_y);
                  }
               }
            }
         } else {
            if (num_pts > 1) {
               if (event_state & ControlMask) {
                  XDrawLine(mainDisplay, drawWindow, drawGC, saved_x, saved_y,
                        grid_x, grid_y);
                  closed = TRUE;
               } else {
                  closed = FALSE;
               }
            }
            XDrawLine(mainDisplay, drawWindow, drawGC, OrigX, OrigY, grid_x,
                  grid_y);
         }
         DoPolygonMeasureCursor(&pmci, POLYGON_DOSHOW, num_pts, ABS_X(grid_x),
               ABS_Y(grid_y), ABS_SIZE(abs(grid_x-OrigX)),
               ABS_SIZE(abs(grid_y-OrigY)), POLYGON_DRAW, POLYGON_DRAG, closed);
         while (XCheckMaskEvent(mainDisplay, PointerMotionMask, &ev)) ;
      } else if (input.type == ButtonPress) {
         /* erase */
         DoPolygonMeasureCursor(&pmci, POLYGON_ENDSHOW, num_pts, ABS_X(grid_x),
               ABS_Y(grid_y), ABS_SIZE(abs(grid_x-OrigX)),
               ABS_SIZE(abs(grid_y-OrigY)), POLYGON_ERASE, POLYGON_DRAG,
               closed);
         button_ev = &(input.xbutton);

         end_x = button_ev->x;
         end_y = button_ev->y;

         if (shiftForDiagMouseMove && DiagEventCheck(&input)) {
            DiagGridXY(OrigX, OrigY, &end_x, &end_y);
         }
         GridXY(end_x, end_y, &grid_x, &grid_y);

         if (grid_x == OrigX && grid_y == OrigY) {
            if (curSpline == LT_STRUCT_SPLINE) {
               if (num_pts == 1) {
                  abort = TRUE;
               } else {
                  /* need to overwrite the previous point */

                  /* erase */

                  /*
                   * since we've clicked at the same point, need to pretend
                   *       that num_pts is one more
                   */
                  EraseStructSplineLinesForContAndUpdateSvs(OrigX, OrigY,
                        grid_x, grid_y, &input, num_pts+1, &sv, sn, &sv2, sn2,
                        &ipt_prev, &first_ipt);
                  if (ipt_prev.later_valid) {
                     /* erase whole curve */
                     DrawAllStructSplinePointsForCont(num_pts);
                     ipt_prev.earlier_valid = ipt_prev.later_valid = FALSE;
                     ipt_prev.earlier_smooth_pt.x = ipt_prev.later_smooth_pt.x =
                           ipt_prev.hinge_pt.x;
                     ipt_prev.earlier_smooth_pt.y = ipt_prev.later_smooth_pt.y =
                           ipt_prev.hinge_pt.y;
                     UpdateLastPointForCont(&ipt_prev);
                     /* draw whole curve */
                     DrawAllStructSplinePointsForCont(num_pts);
                  }
                  UpdatePrevToLastPointForCont(&ipt_prev);
                  ipt.earlier_valid = ipt.later_valid = FALSE;
                  ipt.earlier_smooth_pt.x = ipt.later_smooth_pt.x =
                        ipt.hinge_pt.x = grid_x;
                  ipt.earlier_smooth_pt.y = ipt.later_smooth_pt.y =
                        ipt.hinge_pt.y = grid_y;
                  OrigX = ipt_prev.hinge_pt.x;
                  OrigY = ipt_prev.hinge_pt.y;
                  if (ipt_prev.later_valid) {
                     pv[0].x = ABS_X(OrigX);
                     pv[0].y = ABS_Y(OrigY);
                     pv[1].x = ABS_X(ipt_prev.later_smooth_pt.x);
                     pv[1].y = ABS_Y(ipt_prev.later_smooth_pt.y);
                     pv[2].x = pv[3].x = ABS_X(grid_x);
                     pv[2].y = pv[3].y = ABS_Y(grid_y);
                     n = 3;
                     sv = MakeSplinePolyVertex(0, curSpline, &sn, drawOrigX,
                           drawOrigY, n, pv);
                  }
                  if (first_ipt.earlier_valid) {
                     tmp_n = 3;
                     tmp_vs[0].x = ABS_X(grid_x);
                     tmp_vs[0].y = ABS_Y(grid_y);
                     tmp_vs[1].x = ABS_X(first_ipt.earlier_smooth_pt.x);
                     tmp_vs[1].y = ABS_Y(first_ipt.earlier_smooth_pt.y);
                     tmp_vs[2].x = ABS_X(first_ipt.hinge_pt.x);
                     tmp_vs[2].y = ABS_Y(first_ipt.hinge_pt.y);
                     sv2 = MakeSplinePolyVertex(0, curSpline, &sn2,
                           drawOrigX, drawOrigY, tmp_n, tmp_vs);
                     XDrawLines(mainDisplay, drawWindow, drawGC, sv2, sn2,
                           CoordModeOrigin);
                  }
               }
            }
         } else {
            num_pts++;
            ipt.earlier_valid = ipt.later_valid = FALSE;
            ipt.earlier_smooth_pt.x = ipt.later_smooth_pt.x = ipt.hinge_pt.x =
                  grid_x;
            ipt.earlier_smooth_pt.y = ipt.later_smooth_pt.y = ipt.hinge_pt.y =
                  grid_y;
            ipt.ratio = (double)1;
            AddPointForCont(grid_x, grid_y, &ipt);
            if (curSpline != LT_STRAIGHT && curSpline != LT_STRUCT_SPLINE) {
               if (n >= max_n-3) {
                  max_n += 40;
                  pv = (IntPoint*)realloc(pv, sizeof(IntPoint)*max_n+1);
                  if (pv == NULL) FailAllocMessage();
               }
               /* erase */
               XDrawLines(mainDisplay, drawWindow, drawGC, sv, sn,
                     CoordModeOrigin);
               if (sv != NULL) {
                  free(sv);
                  sv = NULL;
               }
               pv[n].x = ABS_X(grid_x);
               pv[n].y = ABS_Y(grid_y);
               n++;
               pv[n].x = pv[0].x;
               pv[n].y = pv[0].y;
               switch (curSpline) {
               case LT_SPLINE:
                  sv = MakeSplinePolygonVertex(0, curSpline, &sn, drawOrigX,
                        drawOrigY, n+1, pv);
                  break;
               case LT_INTSPLINE:
                  if (cntrlv != NULL) free(cntrlv);
                  sv = MakeIntSplinePolygonVertex(&sn, &intn, &cntrlv,
                        drawOrigX, drawOrigY, n+1, pv);
                  for (i=0; i < sn; i++) {
                     if (sv[i].x < ltx) ltx = sv[i].x;
                     if (sv[i].y < lty) lty = sv[i].y;
                     if (sv[i].x > rbx) rbx = sv[i].x;
                     if (sv[i].y > rby) rby = sv[i].y;
                  }
                  break;
               }
               /* draw */
               XDrawLines(mainDisplay, drawWindow, drawGC, sv, sn,
                     CoordModeOrigin);
            } else if (curSpline == LT_STRUCT_SPLINE) {
               /* erase */
               EraseStructSplineLinesForContAndUpdateSvs(OrigX, OrigY, grid_x,
                     grid_y, &input, num_pts, &sv, sn, &sv2, sn2, &ipt_prev,
                     &first_ipt);
               /* draw */
               if (ipt_prev.later_valid) {
                  pv[0].x = ABS_X(OrigX);
                  pv[0].y = ABS_Y(OrigY);
                  pv[1].x = ABS_X(ipt_prev.later_smooth_pt.x);
                  pv[1].y = ABS_Y(ipt_prev.later_smooth_pt.y);
                  pv[2].x = pv[3].x = ABS_X(grid_x);
                  pv[2].y = pv[3].y = ABS_Y(grid_y);
                  n = 3;
                  sv = MakeSplinePolyVertex(0, curSpline, &sn, drawOrigX,
                        drawOrigY, n, pv);
                  XDrawLines(mainDisplay, drawWindow, drawGC, sv, sn,
                        CoordModeOrigin);
                  if (first_ipt.earlier_valid) {
                     tmp_n = 3;
                     tmp_vs[0].x = ABS_X(grid_x);
                     tmp_vs[0].y = ABS_Y(grid_y);
                     tmp_vs[1].x = ABS_X(first_ipt.earlier_smooth_pt.x);
                     tmp_vs[1].y = ABS_Y(first_ipt.earlier_smooth_pt.y);
                     tmp_vs[2].x = ABS_X(first_ipt.hinge_pt.x);
                     tmp_vs[2].y = ABS_Y(first_ipt.hinge_pt.y);
                     sv2 = MakeSplinePolyVertex(0, curSpline, &sn2,
                           drawOrigX, drawOrigY, tmp_n, tmp_vs);
                     XDrawLines(mainDisplay, drawWindow, drawGC, sv2, sn2,
                           CoordModeOrigin);
                  } else {
                     XDrawLine(mainDisplay, drawWindow, drawGC,
                           first_ipt.hinge_pt.x, first_ipt.hinge_pt.y,
                           grid_x, grid_y);
                  }
               } else {
                  XDrawLine(mainDisplay, drawWindow, drawGC, OrigX, OrigY,
                        grid_x, grid_y);
                  if (sv != NULL) {
                     free(sv);
                     sv = NULL;
                  }
               }
            }
         }
         if (abort) {
            continue;
         }
         if (num_pts == 2 && closed && button_ev->button == Button1 &&
               curSpline == LT_STRAIGHT) {
            XDrawLine(mainDisplay, drawWindow, drawGC, OrigX, OrigY, grid_x,
                  grid_y);
         }
         if (grid_x == start_polygon_x && grid_y == start_polygon_y) {
            if (curSpline != LT_STRAIGHT && curSpline != LT_STRUCT_SPLINE) {
               /* erase the whole thing */
               XDrawLines(mainDisplay, drawWindow, drawGC, sv, sn,
                     CoordModeOrigin);
            } else if (curSpline == LT_STRUCT_SPLINE) {
               CVListElem *elem=ListFirst(&gStructSplineList);
               IntPointTriplet *pipt=(IntPointTriplet*)(elem->obj);

               /* erase the last part */
               if (nothing_is_drawn) {
                  /* then there is nothing to erase */
                  nothing_is_drawn = FALSE;
               } else {
                  EraseStructSplineLinesForContAndUpdateSvs(OrigX, OrigY,
                        grid_x, grid_y, &input, num_pts, &sv, sn, &sv2, sn2,
                        &ipt_prev, &first_ipt);
               }
               /* make the last point identical to the first point */
               UpdateLastPointForCont(pipt);
            } else {
               /* erase the last part */
               XDrawLine(mainDisplay, drawWindow, drawGC, OrigX, OrigY,
                     grid_x, grid_y);
               if (num_pts > 1 && closed) {
                  XDrawLine(mainDisplay, drawWindow, drawGC, saved_x,
                        saved_y, grid_x, grid_y);
               }
               if (num_pts < 4) {
                  XDrawLine(mainDisplay, drawWindow, drawGC, saved_x,
                        saved_y, OrigX, OrigY);
               }
            }
            done = TRUE;
         } else {
            switch(button_ev->button) {
            case Button1:
               if (curSpline == LT_STRUCT_SPLINE) {
                  /*
                   * if the first point is curved, both the curve "before"
                   *       and "after have been drawn at this point
                   * if the first point is *not* curved, only the curve "before"
                   *       has been drawn, the line "after" is open (not drawn)
                   */
                  ContinueForStructSplinePolygonControlPoints(OrigX, OrigY,
                        grid_x, grid_y, &sv, &sn, &sv2, &sn2, &first_ipt,
                        &ipt_prev, &ipt, &abort);
                  UpdateLastPointForCont(&ipt);
                  /* at this point, the curve has been drawn */
                  if (sv != NULL) {
                     free(sv);
                     sv = NULL;
                     if (sv2 == NULL && !first_ipt.earlier_valid) {
                        /* erase it */
                        XDrawLine(mainDisplay, drawWindow, drawGC,
                              first_ipt.hinge_pt.x, first_ipt.hinge_pt.y,
                              ipt.hinge_pt.x, ipt.hinge_pt.y);
                     }
                  }
                  if (sv2 != NULL) {
                     /* erase it */
                     XDrawLines(mainDisplay, drawWindow, drawGC, sv2, sn2,
                           CoordModeOrigin);
                     free(sv2);
                     sv2 = NULL;
                     if (ipt.later_valid) {
                        sn = 2;
                        sv = (XPoint*)malloc((sn+1)*sizeof(XPoint));
                        if (sv == NULL) FailAllocMessage();
                        sv[0].x = ABS_X(ipt.later_smooth_pt.x);
                        sv[0].y = ABS_Y(ipt.later_smooth_pt.y);
                        sv[1].x = ABS_X(ipt.hinge_pt.x);
                        sv[1].y = ABS_Y(ipt.hinge_pt.y);
                        XDrawLines(mainDisplay, drawWindow, drawGC, sv, sn,
                              CoordModeOrigin);
                        if (first_ipt.earlier_valid) {
                           tmp_n = 3;
                           tmp_vs[0].x = ABS_X(ipt.later_smooth_pt.x);
                           tmp_vs[0].y = ABS_Y(ipt.later_smooth_pt.y);
                           tmp_vs[1].x = ABS_X(first_ipt.earlier_smooth_pt.x);
                           tmp_vs[1].y = ABS_Y(first_ipt.earlier_smooth_pt.y);
                           tmp_vs[2].x = ABS_X(first_ipt.hinge_pt.x);
                           tmp_vs[2].y = ABS_Y(first_ipt.hinge_pt.y);
                           sv2 = MakeSplinePolyVertex(0, curSpline, &sn2,
                                 drawOrigX, drawOrigY, tmp_n, tmp_vs);
                           XDrawLines(mainDisplay, drawWindow, drawGC, sv2, sn2,
                                 CoordModeOrigin);
                        } else {
                           XDrawLine(mainDisplay, drawWindow, drawGC,
                                 first_ipt.hinge_pt.x, first_ipt.hinge_pt.y,
                                 ipt.later_smooth_pt.x, ipt.later_smooth_pt.y);
                        }
                     } else {
                        nothing_is_drawn = TRUE;
                     }
                  } else {
                     nothing_is_drawn = TRUE;
                  }
                  if (!abort) {
                     CVListElem *elem=ListLast(&gStructSplineList);

                     if (elem != NULL) {
                        IntPointTriplet *pipt=(IntPointTriplet*)(elem->obj);

                        memcpy(pipt, &ipt, sizeof(IntPointTriplet));
                     }
                     memcpy(&ipt_prev, &ipt, sizeof(IntPointTriplet));
                     OrigX = ipt.hinge_pt.x;
                     OrigY = ipt.hinge_pt.y;
                     if (ipt.later_valid) {
                        grid_x = ipt.later_smooth_pt.x;
                        grid_y = ipt.later_smooth_pt.y;
                     } else {
                        grid_x = ipt.hinge_pt.x;
                        grid_y = ipt.hinge_pt.y;
                     }
                     DoPolygonMeasureCursor(&pmci, POLYGON_STARTSHOW, num_pts,
                           ABS_X(grid_x), ABS_Y(grid_y),
                           ABS_SIZE(abs(grid_x-OrigX)),
                           ABS_SIZE(abs(grid_y-OrigY)), POLYGON_DRAW,
                           POLYGON_CLICK, closed);
                  }
               } else {
                  OrigX = grid_x; OrigY = grid_y;
                  /* draw */
                  DoPolygonMeasureCursor(&pmci, POLYGON_STARTSHOW, num_pts,
                        ABS_X(grid_x), ABS_Y(grid_y), 0, 0, POLYGON_DRAW,
                        POLYGON_CLICK, closed);
               }
               break;
            case Button2:
            case Button3:
               if (curSpline != LT_STRAIGHT && curSpline != LT_STRUCT_SPLINE) {
                  XDrawLines(mainDisplay, drawWindow, drawGC, sv, sn,
                        CoordModeOrigin);
                  if (num_pts == 1) {
                     done = TRUE;
                     break;
                  }
               } else if (curSpline == LT_STRUCT_SPLINE) {
                  /*
                   * if the first point is curved, both the curve "before"
                   *       and "after have been drawn at this point
                   * if the first point is *not* curved, only the curve "before"
                   *       has been drawn, the line "after" is open (not drawn)
                   */
                  memset(&ipt, 0, sizeof(IntPointTriplet));
                  ipt.earlier_valid = ipt.later_valid = FALSE;
                  ipt.hinge_pt.x = ipt.earlier_smooth_pt.x =
                        ipt.later_smooth_pt.x = grid_x;
                  ipt.hinge_pt.y = ipt.earlier_smooth_pt.y =
                        ipt.later_smooth_pt.y = grid_y;
                  ipt.ratio = (double)1;
                  ContinueForStructSplinePolygonControlPoints(OrigX, OrigY,
                        grid_x, grid_y, &sv, &sn, &sv2, &sn2, &first_ipt,
                        &ipt_prev, &ipt, &abort);
                  UpdateLastPointForCont(&ipt);
                  if (num_pts == 1 && !first_ipt.later_valid &&
                        !ipt.earlier_valid) {
                     done = TRUE;
                     break;
                  }
               } else {
                  XDrawLine(mainDisplay, drawWindow, drawGC, OrigX, OrigY,
                        grid_x, grid_y);
                  if (num_pts > 1 && closed) {
                     XDrawLine(mainDisplay, drawWindow, drawGC, saved_x,
                           saved_y, grid_x, grid_y);
                  }
                  if (num_pts < 3) {
                     XDrawLine(mainDisplay, drawWindow, drawGC, saved_x,
                           saved_y, OrigX, OrigY);
                  }
                  if (num_pts == 1) {
                     done = TRUE;
                     break;
                  }
               }
               num_pts++;
               AddPointForCont(start_polygon_x, start_polygon_y, &first_ipt);
               done = TRUE;
               break;
            }
         }
      }
   }
   XUngrabPointer(mainDisplay, CurrentTime);
   RestoreStatusStrings();
   SetMouseStatus(NULL, NULL, NULL);
   Msg("");

   if (pv != NULL) free(pv);
   if (sv != NULL) free(sv);
   if (cntrlv != NULL) free(cntrlv);

   if (!abort) {
      if (curSpline == LT_STRUCT_SPLINE) {
         int num_actual_vs=CountStructuredSplinePolygonVs();

         if (num_actual_vs <= 3) {
            abort = TRUE;
         }
      } else if (num_pts <= 3) {
         abort = TRUE;
      }
   }
   if (!abort) {
      CreatePolygonObj(num_pts, CREATE_RELATIVE);
      RecordNewObjCmd();
      RedrawAnArea(botObj, topObj->bbox.ltx-GRID_ABS_SIZE(1),
            topObj->bbox.lty-GRID_ABS_SIZE(1),
            topObj->bbox.rbx+GRID_ABS_SIZE(1),
            topObj->bbox.rby+GRID_ABS_SIZE(1));
      polygonDrawn = TRUE;
      SetFileModified(TRUE);
   } else {
      if (curSpline != LT_INTSPLINE) {
         ltx = rbx = grid_x;
         lty = rby = grid_y;
      }
      FreePointsForCont(&ltx, &lty, &rbx, &rby);
      RedrawAnArea(botObj, ABS_X(ltx)-GRID_ABS_SIZE(1),
            ABS_Y(lty)-GRID_ABS_SIZE(1), ABS_X(rbx)+GRID_ABS_SIZE(1),
            ABS_Y(rby)+GRID_ABS_SIZE(1));
      numPtsInPoly = 0;
      lastPtPtr = NULL;
      polygonDrawn = FALSE;
   }
}

void DrawPolygon(input)
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
      ContinuePolygon(grid_x, grid_y);
   } 
}

void SelectAndHighLightNewObjects(PrevTopObj)
   struct ObjRec *PrevTopObj;
{
   register struct ObjRec *obj_ptr;
   register struct SelRec *sel_ptr;

   if (topSel != NULL) {
      HighLightReverse();
      RemoveAllSel();
   }
   for (obj_ptr=topObj; obj_ptr != PrevTopObj; obj_ptr=obj_ptr->next) {
      sel_ptr = (struct SelRec *)malloc(sizeof(struct SelRec));
      if (sel_ptr == NULL) FailAllocMessage();
      sel_ptr->obj = obj_ptr;
      sel_ptr->prev = sel_ptr->next = NULL;
      AddSel(botSel, NULL, sel_ptr);
   }
   UpdSelBBox();
   HighLightForward();
}

void InputPolygonPts()
{
   char inbuf[MAXSTRING+1];
   int more_polygon=FALSE, num_polygons=0;
   int started_composite=FALSE;
   struct ObjRec *saved_top_obj=topObj;

   MakeQuiescent();
   XSync(mainDisplay, False);
   do {
      int len, ok=TRUE, num_pts=0;
      int first_x=0, first_y=0, eof=TRUE;
      struct PtRec *pt_ptr;

      more_polygon = FALSE;
      numPtsInPoly = 0;
      lastPtPtr = NULL;
      printf("%s\n", TgLoadString(STID_INPUT_PAIRS_OF_POINTS_POLYGON));
      printf("> ");
      fflush(stdout);
      while (ok && fgets(inbuf, MAXSTRING, stdin) != NULL) {
         if (strcmp(inbuf, ";\n") == 0) {
            eof = FALSE;
            more_polygon = TRUE;
            break;
         }
         if (strcmp(inbuf, ".\n") == 0) {
            eof = FALSE;
            break;
         }
         len = strlen(inbuf);
         if (len > 0) {
            char *c_ptr=strtok(inbuf," ,\t\n"), *c_ptr1=NULL;
   
            if (c_ptr != NULL) c_ptr1 = strtok(NULL," ,\t\n");
            if (c_ptr1 != NULL) while (strchr(" ,\t\n", *c_ptr1)) c_ptr1++;
            while (c_ptr != NULL && c_ptr1 != NULL) {
               num_pts++;
               pt_ptr = (struct PtRec *)malloc(sizeof(struct PtRec));
               if (pt_ptr == NULL) FailAllocMessage();
               pt_ptr->next = lastPtPtr;
               if (sscanf(c_ptr, "%d", &pt_ptr->x) != 1 ||
                     sscanf(c_ptr1, "%d", &pt_ptr->y) != 1) {
                  ok = FALSE;
                  MsgBox(TgLoadString(STID_READ_INT_ERR_FOR_POLYGON_PTS),
                        TOOL_NAME, INFO_MB);
                  XSync(mainDisplay, False);
                  break;
               }
               if (lastPtPtr == NULL) {
                  first_x = pt_ptr->x;
                  first_y = pt_ptr->y;
               }
               lastPtPtr = pt_ptr;
               c_ptr = strtok(NULL," ,\t\n");
               if (c_ptr != NULL) c_ptr1 = strtok(NULL," ,\t\n");
               if (c_ptr1 != NULL) while (strchr(" ,\t\n", *c_ptr1)) c_ptr1++;
            }
            if (c_ptr != NULL) {
               ok = FALSE;
               MsgBox(TgLoadString(STID_READ_INT_ERR_FOR_POLYGON_PTS),
                     TOOL_NAME, INFO_MB);
               XSync(mainDisplay, False);
            }
         }
         printf("> ");
         fflush(stdout);
      }
      printf("\n");
      if (eof) rewind(stdin);
      if (ok && num_pts > 2) {
         num_polygons++;
         if (lastPtPtr->x != first_x || lastPtPtr->y != first_y) {
            num_pts++;
            pt_ptr = (struct PtRec *)malloc(sizeof(struct PtRec));
            if (pt_ptr == NULL) FailAllocMessage();
            pt_ptr->next = lastPtPtr;
            pt_ptr->x = first_x;
            pt_ptr->y = first_y;
            lastPtPtr = pt_ptr;
         }
         CreatePolygonObj(num_pts, CREATE_ABSOLUTE);
         if (more_polygon || num_polygons > 1) {
            if (num_polygons <= 1) {
               StartCompositeCmd();
               started_composite = TRUE;
            }
            RecordNewObjCmd();
            numRedrawBBox = 0;
            topObj->tmp_parent = NULL;
            DrawObj(drawWindow, topObj);
         } else {
            RecordNewObjCmd();
            RedrawAnArea(botObj, topObj->bbox.ltx-GRID_ABS_SIZE(1),
                  topObj->bbox.lty-GRID_ABS_SIZE(1),
                  topObj->bbox.rbx+GRID_ABS_SIZE(1),
                  topObj->bbox.rby+GRID_ABS_SIZE(1));
            SelectTopObj();
            SetFileModified(TRUE);
            justDupped = FALSE;
         }
      }
      if (ok && num_pts <= 2) {
         MsgBox(TgLoadString(STID_TOO_FEW_POINTERS_ENTERED), TOOL_NAME,
               INFO_MB);
         XSync(mainDisplay, False);
      }
      for ( ; lastPtPtr != NULL; lastPtPtr=pt_ptr) {
         pt_ptr = lastPtPtr->next;
         free(pt_ptr);
      }
   } while (more_polygon);
   if (num_polygons > 1 || started_composite) {
      SelectAndHighLightNewObjects(saved_top_obj);
      GroupSelObj(TRUE, TRUE, TRUE);
      EndCompositeCmd();

      SetFileModified(TRUE);
      justDupped = FALSE;
   }
}

static
void AddToLastPoint(XOff, YOff)
   int XOff, YOff;
{
   struct PtRec *pt_ptr=(struct PtRec *)malloc(sizeof(struct PtRec));

   if (pt_ptr == NULL) FailAllocMessage();
   pt_ptr->next = lastPtPtr;
   numPtsInPoly++;
   lastPtPtr = pt_ptr;
   pt_ptr->x = XOff;
   pt_ptr->y = YOff;
}

void GetBoundingBox()
{
   struct SelRec *sel_ptr, *tmp_top_sel=NULL, *tmp_bot_sel=NULL;
   int num_created=0, saved_cur_spline=curSpline;

   if (topSel == NULL) {
      MsgBox(TgLoadCachedString(CSTID_NO_OBJ_SELECTED), TOOL_NAME, INFO_MB);
      return;
   }
   if (curChoice == VERTEXMODE) SetCurChoice(NOTHING);
   curSpline = LT_STRAIGHT;

   tmp_top_sel = tmp_bot_sel = NULL;

   HighLightReverse();
   StartCompositeCmd();
   for (sel_ptr=botSel; sel_ptr != NULL; sel_ptr=sel_ptr->prev) {
      struct ObjRec *obj_ptr=sel_ptr->obj;
      struct SelRec *new_sel_ptr;

      if (obj_ptr->ctm == NULL) {
         struct BBRec *p_obbox=(&obj_ptr->obbox);

         AddToLastPoint(p_obbox->ltx, p_obbox->lty);
         AddToLastPoint(p_obbox->ltx, p_obbox->rby);
         AddToLastPoint(p_obbox->rbx, p_obbox->rby);
         AddToLastPoint(p_obbox->rbx, p_obbox->lty);
         AddToLastPoint(p_obbox->ltx, p_obbox->lty);
         CreatePolygonObj(5, CREATE_ABSOLUTE);
      } else {
         int i;

         numPtsInPoly = 0;
         lastPtPtr = NULL;
         for (i=0; i < 5; i++) {
            AddToLastPoint(obj_ptr->rotated_obbox[i].x,
                  obj_ptr->rotated_obbox[i].y);
         }
         CreatePolygonObj(5, CREATE_RELATIVE);
      }

      new_sel_ptr = (struct SelRec *)malloc(sizeof(struct SelRec));
      if (new_sel_ptr == NULL) FailAllocMessage();
      new_sel_ptr->obj = topObj;
      new_sel_ptr->next = NULL;
      new_sel_ptr->prev = NULL;
      PrepareToRecord(CMD_NEW, NULL, NULL, 0);
      RecordCmd(CMD_NEW, NULL, new_sel_ptr, new_sel_ptr, 1);

      new_sel_ptr->next = tmp_top_sel;
      if (tmp_top_sel == NULL) {
         tmp_bot_sel = new_sel_ptr;
      } else {
         tmp_top_sel->prev = new_sel_ptr;
      }
      tmp_top_sel = new_sel_ptr;
      num_created++;
   }
   EndCompositeCmd();
   curSpline = saved_cur_spline;

   RemoveAllSel();
   topSel = tmp_top_sel;
   botSel = tmp_bot_sel;
   UpdSelBBox();
   RedrawAnArea(botObj, selLtX-GRID_ABS_SIZE(1), selLtY-GRID_ABS_SIZE(1),
         selRbX+GRID_ABS_SIZE(1), selRbY+GRID_ABS_SIZE(1));
   HighLightForward();
   justDupped = FALSE;
   SetFileModified(TRUE);
}

void SavePolygonObj(FP, ObjPtr)
   FILE *FP;
   struct ObjRec *ObjPtr;
{
   register int i, n;
   int count;
   struct PolygonRec *polygon_ptr=ObjPtr->detail.g;

   n = polygon_ptr->n;
   if (fprintf(FP, "polygon('%s','',%1d,[\n\t",
         colorMenuItems[ObjPtr->color], polygon_ptr->n) == EOF) {
      writeFileFailed = TRUE;
   }
   for (i=0, count = 0; i < n-1; i++) {
      if (fprintf(FP, "%1d,%1d,", polygon_ptr->vlist[i].x,
            polygon_ptr->vlist[i].y) == EOF) {
         writeFileFailed = TRUE;
      }
      if (++count == 8) {
         count = 0;
         if (fprintf(FP, "\n\t") == EOF) writeFileFailed = TRUE;
      }
   }
   if (fprintf(FP, "%1d,%1d],",
         polygon_ptr->vlist[n-1].x, polygon_ptr->vlist[n-1].y) == EOF)
      writeFileFailed = TRUE;

   if (fprintf(FP,
         "%1d,%1d,%1d,%1d,%1d,%1d,%1d,%1d,%1d,%1d,'%s',%1d,",
         polygon_ptr->fill, polygon_ptr->width, polygon_ptr->pen,
         polygon_ptr->curved, ObjPtr->id, polygon_ptr->dash, ObjPtr->rotation,
         ObjPtr->locked, ObjPtr->ctm!=NULL, ObjPtr->invisible,
         polygon_ptr->width_spec, ObjPtr->trans_pat) == EOF) {
      writeFileFailed = TRUE;
   }
   if (fprintf(FP, "\n    \"") == EOF) writeFileFailed = TRUE;
   SaveSmoothHinge(FP, polygon_ptr->curved, polygon_ptr->n,
         polygon_ptr->smooth);
   if (fprintf(FP, "\",") == EOF) writeFileFailed = TRUE;
   if (ObjPtr->ctm != NULL && fprintf(FP,
         "[\n\t%1d,%1d,%1d,%1d,%1d,%1d,%g,%g,%g,%g,%1d,%1d],",
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

void ReadPolygonObj(FP, Inbuf, ObjPtr)
   FILE *FP;
   char *Inbuf;
   struct ObjRec **ObjPtr;
{
   register int i;
   struct PolygonRec *polygon_ptr;
   IntPoint *v;
   char color_str[40], bg_color_str[40], *s, inbuf[MAXSTRING];
   int num_pts, ltx=0, lty=0, rbx=0, rby=0, x, y, id=0;
   int trans_pat=FALSE, fill, width, pen, w, new_alloc, locked=FALSE;
   int curved, dash, initialized, rotation, count;
   int real_x=0, real_y=0, transformed=FALSE, invisible=FALSE;
   char *smooth=NULL, width_spec[40];
   struct XfrmMtrxRec *ctm=NULL;
   struct BBRec orig_obbox;

   *ObjPtr = NULL;

   s = FindChar((int)'(', Inbuf);
   s = ParseStr(s, (int)',', color_str, sizeof(color_str));
   if (fileVersion >= 37) {
      s = ParseStr(s, (int)',', bg_color_str, sizeof(bg_color_str));
   }
   InitScan(s, "\t\n, []");

   if (GETINT("polygon", num_pts, "number of points") == INVALID) {
      return;
   }
   if (num_pts <= 0) {
      (void)sprintf(gszMsgBox, TgLoadString(STID_INVALID_NUM_PTS_IN_POLYGON),
            scanFileName, scanLineNum);
      if (PRTGIF) {
         fprintf(stderr, "%s\n", gszMsgBox);
      } else {
         Msg(gszMsgBox);
      }
      return;
   }

   *ObjPtr = (struct ObjRec *)malloc(sizeof(struct ObjRec));
   if (*ObjPtr == NULL) FailAllocMessage();
   memset(*ObjPtr, 0, sizeof(struct ObjRec));
   polygon_ptr = (struct PolygonRec *)malloc(sizeof(struct PolygonRec));
   if (polygon_ptr == NULL) FailAllocMessage();
   memset(polygon_ptr, 0, sizeof(struct PolygonRec));

   if (num_pts < 3) {
      v = (IntPoint*)malloc(5*sizeof(IntPoint));
      if (v == NULL) FailAllocMessage();
      smooth = (char*)malloc(5*sizeof(char));
      if (smooth == NULL) FailAllocMessage();
   } else {
      v = (IntPoint*)malloc((num_pts+1)*sizeof(IntPoint));
      if (v == NULL) FailAllocMessage();
      smooth = (char*)malloc((num_pts+1)*sizeof(char));
      if (smooth == NULL) FailAllocMessage();
   }

   initialized = FALSE;

   *width_spec = '\0';
   if (fileVersion <= 13) {
      for (i=0; i < num_pts; i++) {
         if (GETINT("polygon", x, "x") == INVALID ||
             GETINT("polygon", y, "y") == INVALID) {
            free(*ObjPtr);
            free(polygon_ptr);
            free(v);
            *ObjPtr = NULL;
            return;
         }
         v[i].x = x; v[i].y = y;
         if (!initialized) {
            initialized = TRUE;
            ltx = rbx = x; lty = rby = y;
         } else {
            if (x < ltx) ltx = x; if (y < lty) lty = y;
            if (x > rbx) rbx = x; if (y > rby) rby = y;
         }
      }
   } else {
      (void)fgets(inbuf, MAXSTRING, FP);
      scanLineNum++;
      s = inbuf;
      InitScan(s, "\t\n, []");
      for (i=0, count = 0; i < num_pts; i++) {
         if (GETINT("polygon", x, "x") == INVALID ||
             GETINT("polygon", y, "y") == INVALID) {
            free(*ObjPtr);
            free(polygon_ptr);
            free(v);
            *ObjPtr = NULL;
            return;
         }
         v[i].x = x; v[i].y = y;
         if (!initialized) {
            initialized = TRUE;
            ltx = rbx = x; lty = rby = y;
         } else {
            if (x < ltx) ltx = x; if (y < lty) lty = y;
            if (x > rbx) rbx = x; if (y > rby) rby = y;
         }
         if (++count == 8 && i != num_pts-1) {
            count = 0;
            (void)fgets(inbuf, MAXSTRING, FP);
            scanLineNum++;
            s = inbuf;
            InitScan(s, "\t\n, []");
         }
      }
   }

   switch (num_pts) {
   case 1:
      sprintf(gszMsgBox, TgLoadCachedString(CSTID_SNGL_PT_POLYGON_CONVERTED),
            v[0].x, v[0].y);
      if (PRTGIF) {
         fprintf(stderr, "%s\n", gszMsgBox);
      } else {
         Msg(gszMsgBox);
      }
      v[3].x = v[2].x = v[1].x = v[0].x;
      v[3].y = v[2].y = v[1].y = v[0].y;
      num_pts = 4;
      break;
   case 2:
      sprintf(gszMsgBox, TgLoadCachedString(CSTID_TWO_PT_POLYGON_CONVERTED),
            v[0].x, v[0].y, v[1].x, v[1].y);
      if (PRTGIF) {
         fprintf(stderr, "%s\n", gszMsgBox);
      } else {
         Msg(gszMsgBox);
      }
      v[3].x = v[2].x = v[0].x;
      v[3].y = v[2].y = v[0].y;
      num_pts = 4;
      break;
   case 3:
      sprintf(gszMsgBox, TgLoadCachedString(CSTID_TWO_PT_POLYGON_CONVERTED),
            v[0].x, v[0].y, v[1].x, v[1].y);
      if (PRTGIF) {
         fprintf(stderr, "%s\n", gszMsgBox);
      } else {
         Msg(gszMsgBox);
      }
      v[3].x = v[2].x = v[0].x;
      v[3].y = v[2].y = v[0].y;
      num_pts = 4;
      break;
   }

   polygon_ptr->n = num_pts;

   dash = 0;
   rotation = 0;
   if (fileVersion <= 3) {
      if (GETINT("polygon", fill,     "fill") == INVALID ||
          GETINT("polygon", width,    "width") == INVALID ||
          GETINT("polygon", pen,      "pen") == INVALID) {
         free(*ObjPtr);
         free(polygon_ptr);
         free(v);
         *ObjPtr = NULL;
         return;
      }
      if (width == LINE_CURVED) {
         width = 0;
         curved = TRUE;
      } else {
         curved = FALSE;
      }
      switch (width) {
      case 1: width = 3; break;
      case 2: width = 6; break;
      }
      id = objId++;
   } else if (fileVersion <= 5) {
      if (GETINT("polygon", fill,     "fill") == INVALID ||
          GETINT("polygon", width,    "width") == INVALID ||
          GETINT("polygon", pen,      "pen") == INVALID ||
          GETINT("polygon", curved,   "curved") == INVALID) {
         free(*ObjPtr);
         free(polygon_ptr);
         free(v);
         *ObjPtr = NULL;
         return;
      }
      switch (width) {
      case 1: width = 3; break;
      case 2: width = 6; break;
      }
      id = objId++;
   } else if (fileVersion <= 7) {
      if (GETINT("polygon", fill,     "fill") == INVALID ||
          GETINT("polygon", width,    "width") == INVALID ||
          GETINT("polygon", pen,      "pen") == INVALID ||
          GETINT("polygon", curved,   "curved") == INVALID) {
         free(*ObjPtr);
         free(polygon_ptr);
         free(v);
         *ObjPtr = NULL;
         return;
      }
      id = objId++;
   } else if (fileVersion <= 8) {
      if (GETINT("polygon", fill,     "fill") == INVALID ||
          GETINT("polygon", width,    "width") == INVALID ||
          GETINT("polygon", pen,      "pen") == INVALID ||
          GETINT("polygon", curved,   "curved") == INVALID ||
          GETINT("polygon", id,       "id") == INVALID) {
         free(*ObjPtr);
         free(polygon_ptr);
         free(v);
         *ObjPtr = NULL;
         return;
      }
      if (id >= objId) objId = id+1;
   } else if (fileVersion <= 13) {
      if (GETINT("polygon", fill,     "fill") == INVALID ||
          GETINT("polygon", width,    "width") == INVALID ||
          GETINT("polygon", pen,      "pen") == INVALID ||
          GETINT("polygon", curved,   "curved") == INVALID ||
          GETINT("polygon", id,       "id") == INVALID ||
          GETINT("polygon", dash,     "dash") == INVALID) {
         free(*ObjPtr);
         free(polygon_ptr);
         free(v);
         *ObjPtr = NULL;
         return;
      }
      if (id >= objId) objId = id+1;
   } else if (fileVersion <= 25) {
      if (GETINT("polygon", fill,     "fill") == INVALID ||
          GETINT("polygon", width,    "width") == INVALID ||
          GETINT("polygon", pen,      "pen") == INVALID ||
          GETINT("polygon", curved,   "curved") == INVALID ||
          GETINT("polygon", id,       "id") == INVALID ||
          GETINT("polygon", dash,     "dash") == INVALID ||
          GETINT("polygon", rotation, "rotation") == INVALID) {
         free(*ObjPtr);
         free(polygon_ptr);
         free(v);
         *ObjPtr = NULL;
         return;
      }
      if (id >= objId) objId = id+1;
   } else if (fileVersion <= 32) {
      if (GETINT("polygon", fill,     "fill") == INVALID ||
          GETINT("polygon", width,    "width") == INVALID ||
          GETINT("polygon", pen,      "pen") == INVALID ||
          GETINT("polygon", curved,   "curved") == INVALID ||
          GETINT("polygon", id,       "id") == INVALID ||
          GETINT("polygon", dash,     "dash") == INVALID ||
          GETINT("polygon", rotation, "rotation") == INVALID ||
          GETINT("polygon", locked,   "locked") == INVALID) {
         free(*ObjPtr);
         free(polygon_ptr);
         free(v);
         *ObjPtr = NULL;
         return;
      }
      if (id >= objId) objId = id+1;
   } else if (fileVersion <= 34) {
      if (GETINT("polygon", fill,        "fill") == INVALID ||
          GETINT("polygon", width,       "width") == INVALID ||
          GETINT("polygon", pen,         "pen") == INVALID ||
          GETINT("polygon", curved,      "curved") == INVALID ||
          GETINT("polygon", id,          "id") == INVALID ||
          GETINT("polygon", dash,        "dash") == INVALID ||
          GETINT("polygon", rotation,    "rotation") == INVALID ||
          GETINT("polygon", locked,      "locked") == INVALID ||
          GETINT("polygon", transformed, "transformed") == INVALID ||
          GETINT("polygon", invisible,   "invisible") == INVALID ||
          GETSTR("polygon", width_spec,  "width_spec") == INVALID) {
         free(*ObjPtr);
         free(polygon_ptr);
         free(v);
         *ObjPtr = NULL;
         return;
      }
      if (id >= objId) objId = id+1;
      UtilRemoveQuotes(width_spec);
   } else {
      if (GETINT("polygon", fill,        "fill") == INVALID ||
          GETINT("polygon", width,       "width") == INVALID ||
          GETINT("polygon", pen,         "pen") == INVALID ||
          GETINT("polygon", curved,      "curved") == INVALID ||
          GETINT("polygon", id,          "id") == INVALID ||
          GETINT("polygon", dash,        "dash") == INVALID ||
          GETINT("polygon", rotation,    "rotation") == INVALID ||
          GETINT("polygon", locked,      "locked") == INVALID ||
          GETINT("polygon", transformed, "transformed") == INVALID ||
          GETINT("polygon", invisible,   "invisible") == INVALID ||
          GETSTR("polygon", width_spec,  "width_spec") == INVALID ||
          GETINT("polygon", trans_pat,   "trans_pat") == INVALID) {
         free(*ObjPtr);
         free(polygon_ptr);
         free(v);
         *ObjPtr = NULL;
         return;
      }
      if (id >= objId) objId = id+1;
      UtilRemoveQuotes(width_spec);
   }

   if (fileVersion <= 16 && width <= 6) width = origWidthOfLine[width];

   if (fileVersion <= 25 && curved > 1) curved = 0;
   if (curved == LT_INTSPLINE && smooth != NULL) {
      free(smooth);
      smooth = NULL;
   }
   if (fileVersion <= 30) {
      switch (curved) {
      case LT_STRAIGHT:
         for (i=0; i < num_pts; i++) smooth[i] = FALSE;
         break;
      case LT_SPLINE:
         for (i=0; i < num_pts; i++) smooth[i] = TRUE;
         break;
      }
   } else if (!ReadSmoothHinge(FP, curved, num_pts, smooth)) {
      free(*ObjPtr);
      free(polygon_ptr);
      free(v);
      *ObjPtr = NULL;
      return;
   }
   if (fileVersion >= 33 && transformed) {
      (void)fgets(inbuf, MAXSTRING, FP);
      scanLineNum++;
      InitScan(inbuf, "\t\n, ");

      ctm = (struct XfrmMtrxRec *)malloc(sizeof(struct XfrmMtrxRec));
      if (ctm == NULL) FailAllocMessage();
      if (GETINT("polygon", real_x,           "real_x") == INVALID ||
          GETINT("polygon", real_y,           "real_y") == INVALID ||
          GETINT("polygon", orig_obbox.ltx,   "orig_obbox.ltx") == INVALID ||
          GETINT("polygon", orig_obbox.lty,   "orig_obbox.lty") == INVALID ||
          GETINT("polygon", orig_obbox.rbx,   "orig_obbox.rbx") == INVALID ||
          GETINT("polygon", orig_obbox.rby,   "orig_obbox.rby") == INVALID ||
          GETDBL("polygon", ctm->m[CTM_SX],   "CTM_SX") == INVALID ||
          GETDBL("polygon", ctm->m[CTM_SIN],  "CTM_SIN") == INVALID ||
          GETDBL("polygon", ctm->m[CTM_MSIN], "CTM_MSIN") == INVALID ||
          GETDBL("polygon", ctm->m[CTM_SY],   "CTM_SY") == INVALID ||
          GETINT("polygon", ctm->t[CTM_TX],   "CTM_TX") == INVALID ||
          GETINT("polygon", ctm->t[CTM_TY],   "CTM_TY") == INVALID) {
         free(*ObjPtr);
         free(polygon_ptr);
         free(v);
         *ObjPtr = NULL;
         free(ctm);
         return;
      }
   }
   if (fileVersion <= 32) {
      sprintf(width_spec, "%1d", width);
   }

   fill = UpgradePenFill(fill);
   pen = UpgradePenFill(pen);

   polygon_ptr->fill = fill;
   polygon_ptr->width = width;
   UtilStrCpyN(polygon_ptr->width_spec, sizeof(polygon_ptr->width_spec),
         width_spec);
   polygon_ptr->pen = pen;
   polygon_ptr->curved = curved;
   polygon_ptr->dash = dash;

   polygon_ptr->vlist = v;
   polygon_ptr->smooth = smooth;
   polygon_ptr->svlist = NULL;
   polygon_ptr->intvlist = NULL;

   polygon_ptr->rotated_n = 0;
   polygon_ptr->rotated_vlist = NULL;

   (*ObjPtr)->x = ltx;
   (*ObjPtr)->y = lty;
   (*ObjPtr)->color = QuickFindColorIndex(*ObjPtr, color_str, &new_alloc, TRUE);
   UtilStrCpyN((*ObjPtr)->color_str, sizeof((*ObjPtr)->color_str), color_str);
   (*ObjPtr)->dirty = FALSE;
   (*ObjPtr)->id = id;
   (*ObjPtr)->rotation = rotation;
   (*ObjPtr)->locked = locked;
   (*ObjPtr)->type = OBJ_POLYGON;
   (*ObjPtr)->obbox.ltx = ltx;
   (*ObjPtr)->obbox.lty = lty;
   (*ObjPtr)->obbox.rbx = rbx;
   (*ObjPtr)->obbox.rby = rby;
   w = HALF_W(width);
   (*ObjPtr)->bbox.ltx = ltx - w;
   (*ObjPtr)->bbox.lty = lty - w;
   (*ObjPtr)->bbox.rbx = rbx + w;
   (*ObjPtr)->bbox.rby = rby + w;
   (*ObjPtr)->detail.g = polygon_ptr;
   (*ObjPtr)->ctm = ctm;
   (*ObjPtr)->invisible = invisible;
   (*ObjPtr)->trans_pat = trans_pat;
   if (ctm != NULL) {
      memcpy(&(*ObjPtr)->orig_obbox, &orig_obbox, sizeof(struct BBRec));
      (*ObjPtr)->x = real_x;
      (*ObjPtr)->y = real_y;
      GetTransformedOBBoxOffsetVs(*ObjPtr, (*ObjPtr)->rotated_obbox);
   }
   AdjObjCache(*ObjPtr);
   AdjObjSplineVs(*ObjPtr);
   if (polygon_ptr->curved != LT_INTSPLINE) {
      UpdPolyBBox(*ObjPtr, polygon_ptr->n, polygon_ptr->vlist);
   } else {
      UpdPolyBBox(*ObjPtr, polygon_ptr->intn, polygon_ptr->intvlist);
   }
}

void SetPolygonPropMask(ObjPtr, plMask, plSkip, pProp)
   struct ObjRec *ObjPtr;
   long *plMask, *plSkip;
   struct PropertiesRec *pProp;
{
   struct PolygonRec *polygon_ptr=ObjPtr->detail.g;

   SetCTMPropertyMask(ObjPtr->ctm, plMask, plSkip, pProp);

   SetIntPropertyMask(PROP_MASK_COLOR, ObjPtr->color,
         colorMenuItems[ObjPtr->color], plMask, plSkip, pProp);
   SetIntPropertyMask(PROP_MASK_WIDTH, polygon_ptr->width,
         polygon_ptr->width_spec, plMask, plSkip, pProp);

   SetIntPropertyMask(PROP_MASK_TRANSPAT, ObjPtr->trans_pat, NULL,
         plMask, plSkip, pProp);
   SetIntPropertyMask(PROP_MASK_FILL, polygon_ptr->fill, NULL,
         plMask, plSkip, pProp);
   SetIntPropertyMask(PROP_MASK_PEN, polygon_ptr->pen, NULL,
         plMask, plSkip, pProp);
   SetIntPropertyMask(PROP_MASK_DASH, polygon_ptr->dash, NULL,
         plMask, plSkip, pProp);
   SetIntPropertyMask(PROP_MASK_CURVED, polygon_ptr->curved, NULL,
         plMask, plSkip, pProp);
}

void FreePolygonObj(ObjPtr)
   struct ObjRec *ObjPtr;
{
   if (ObjPtr->detail.g->ssvlist != NULL) free(ObjPtr->detail.g->ssvlist);
   if (ObjPtr->detail.g->intvlist != NULL) free(ObjPtr->detail.g->intvlist);
   if (ObjPtr->detail.g->svlist != NULL) free(ObjPtr->detail.g->svlist);
   if (ObjPtr->detail.g->rotated_vlist != NULL) {
      free(ObjPtr->detail.g->rotated_vlist);
   }
   free(ObjPtr->detail.g->vlist);
   if (ObjPtr->detail.g->smooth != NULL) free(ObjPtr->detail.g->smooth);
   if (ObjPtr->detail.g->ssmooth != NULL) free(ObjPtr->detail.g->ssmooth);
   free(ObjPtr->detail.g);
   free(ObjPtr);
}
