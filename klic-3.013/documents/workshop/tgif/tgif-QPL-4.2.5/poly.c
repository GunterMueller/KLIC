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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/poly.c,v 1.69 2011/05/16 16:21:59 william Exp $
 */

#define _INCLUDE_FROM_POLY_C_

#include "tgifdefs.h"

#include "arc.e"
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
#include "list.e"
#include "mainloop.e"
#include "mark.e"
#include "msg.e"
#include "obj.e"
#include "page.e"
#include "pattern.e"
#include "pin.e"
#include "poly.e"
#include "polygon.e"
#include "ps.e"
#include "raster.e"
#include "rect.e"
#include "ruler.e"
#include "select.e"
#include "setup.e"
#include "special.e"
#include "spline.e"
#include "stretch.e"
#include "strtbl.e"
#include "util.e"
#include "xpixmap.e"

#define RETREAT (0.8)

WiringInfo gstWiringInfo;

int polyDrawn=FALSE;
int queryPolyNameOnConnect=TRUE;

/* short widthOfLine[] = { 0, 3,  6,  0 }; */
/* short arrowHeadH[]  = { 3, 5,  10, 3 }; */
/* short arrowHeadW[]  = { 8, 12, 22, 8 }; */

short origWidthOfLine[] = { 1, 2,  3,  4,  5,  6,  7 };
short origArrowHeadH[]  = { 3, 4,  5,  6,  7,  8,  9 };
short origArrowHeadW[]  = { 8, 10, 12, 14, 18, 20, 22 };

short *curWidthOfLine=NULL;
short *curArrowHeadH=NULL;
short *curArrowHeadW=NULL;

char **curWidthOfLineSpec=NULL;
char **curArrowHeadHSpec=NULL;
char **curArrowHeadWSpec=NULL;

int numPtsInPoly=0;
CVList gStructSplineList;

struct PtRec *lastPtPtr=NULL;
struct ObjRec *drawPolyHighlightedNode=NULL;

IntPoint *GetPolyOrPolygonAbsVs(pn_return, pn_allocated, ObjPtr)
   int *pn_return, *pn_allocated;
   struct ObjRec *ObjPtr;
{
   int i=0, n=0;
   struct PolyRec *poly_ptr=NULL;
   struct PolygonRec *polygon_ptr=NULL;
   IntPoint *vs=NULL, *return_vs=NULL;

   switch (ObjPtr->type) {
   case OBJ_POLY:
      poly_ptr = ObjPtr->detail.p;
      if (poly_ptr->curved == LT_STRUCT_SPLINE) {
         n = poly_ptr->ssn;
         vs = poly_ptr->ssvlist;
      } else {
         n = poly_ptr->n;
         vs = poly_ptr->vlist;
      }
      break;
   case OBJ_POLYGON:
      polygon_ptr = ObjPtr->detail.g;
      if (polygon_ptr->curved == LT_STRUCT_SPLINE) {
         n = polygon_ptr->ssn;
         vs = polygon_ptr->ssvlist;
      } else {
         n = polygon_ptr->n;
         vs = polygon_ptr->vlist;
      }
      break;
   }
   *pn_return = n;
   if (ObjPtr->ctm == NULL) {
      *pn_allocated = FALSE;
      return vs;
   }
   *pn_allocated = TRUE;

   return_vs = (IntPoint*)malloc(n*sizeof(IntPoint));
   if (return_vs == NULL) FailAllocMessage();
   memset(return_vs, 0, n*sizeof(IntPoint));

   for (i=0; i < n; i++) {
      int x_off=0, y_off=0;

      TransformPointThroughCTM(vs[i].x-ObjPtr->x, vs[i].y-ObjPtr->y,
            ObjPtr->ctm, &x_off, &y_off);
      return_vs[i].x = ObjPtr->x + x_off;
      return_vs[i].y = ObjPtr->y + y_off;
   }
   return return_vs;
}

XPoint *MakePolyVertex(XOff, YOff, NumVs, Vs)
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

void CalcPolyBBox(ObjPtr)
   struct ObjRec *ObjPtr;
{
   register int x, y;
   struct PolyRec *poly_ptr=ObjPtr->detail.p;
   int style=poly_ptr->style, width=poly_ptr->width;
   int aw=poly_ptr->aw, ah=poly_ptr->ah;
   int ltx=ObjPtr->obbox.ltx, lty=ObjPtr->obbox.lty;
   int rbx=ObjPtr->obbox.rbx, rby=ObjPtr->obbox.rby;
   int dx=0, dy=0, tmp_x=0, tmp_y=0, num_pts=0;
   double len, sin, cos, w, h;
   IntPoint *v=NULL;
   int retracted_arrow=(RetractedArrowAttr(ObjPtr) ||
         AutoRetractedArrowAttr(ObjPtr, TRUE));

   switch (poly_ptr->curved) {
   case LT_STRAIGHT:
   case LT_SPLINE:
      num_pts = poly_ptr->n;
      v = poly_ptr->vlist;
      break;
   case LT_STRUCT_SPLINE:
      num_pts = poly_ptr->ssn;
      v = poly_ptr->ssvlist;
      break;
   case LT_INTSPLINE:
      num_pts = poly_ptr->intn;
      v = poly_ptr->intvlist;
      break;
   }
   dx = v[1].x - v[0].x;
   dy = v[1].y - v[0].y;
   if ((style & LS_LEFT) && (dx != 0 || dy != 0)) {
      len = (double)sqrt((double)(((double)dx)*((double)dx) +
            ((double)dy)*((double)dy)));
      sin = ((double)dy) / len;
      cos = ((double)dx) / len;

      w = (double)aw; h = (double)max(ah,(width>>1));

      x = round(v[0].x + w*cos - h*sin);
      y = round(v[0].y + w*sin + h*cos);
      if (ObjPtr->ctm != NULL) {
         TransformPointThroughCTM(x-ObjPtr->x, y-ObjPtr->y, ObjPtr->ctm,
               &tmp_x, &tmp_y);
         x = tmp_x+ObjPtr->x;
         y = tmp_y+ObjPtr->y;
      }
      if (x < ltx) ltx = x; if (y < lty) lty = y;
      if (x > rbx) rbx = x; if (y > rby) rby = y;

      x = round(v[0].x + w*cos + h*sin);
      y = round(v[0].y + w*sin - h*cos);
      if (ObjPtr->ctm != NULL) {
         TransformPointThroughCTM(x-ObjPtr->x, y-ObjPtr->y, ObjPtr->ctm,
               &tmp_x, &tmp_y);
         x = tmp_x+ObjPtr->x;
         y = tmp_y+ObjPtr->y;
      }
      if (x < ltx) ltx = x; if (y < lty) lty = y;
      if (x > rbx) rbx = x; if (y > rby) rby = y;
   }
   dx = v[num_pts-1].x - v[num_pts-2].x;
   dy = v[num_pts-1].y - v[num_pts-2].y;
   if ((style & LS_RIGHT) && (dx != 0 || dy != 0)) {
      len = (double)sqrt((double)(((double)dx)*((double)dx) +
            ((double)dy)*((double)dy)));
      sin = ((double)dy) / len;
      cos = ((double)dx) / len;

      w = (double)aw; h = (double)max(ah,(width>>1));

      x = round(v[num_pts-1].x - w*cos + h*sin);
      y = round(v[num_pts-1].y - w*sin - h*cos);
      if (ObjPtr->ctm != NULL) {
         TransformPointThroughCTM(x-ObjPtr->x, y-ObjPtr->y, ObjPtr->ctm,
               &tmp_x, &tmp_y);
         x = tmp_x+ObjPtr->x;
         y = tmp_y+ObjPtr->y;
      }
      if (x < ltx) ltx = x; if (y < lty) lty = y;
      if (x > rbx) rbx = x; if (y > rby) rby = y;

      x = round(v[num_pts-1].x - w*cos - h*sin);
      y = round(v[num_pts-1].y - w*sin + h*cos);
      if (ObjPtr->ctm != NULL) {
         TransformPointThroughCTM(x-ObjPtr->x, y-ObjPtr->y, ObjPtr->ctm,
               &tmp_x, &tmp_y);
         x = tmp_x+ObjPtr->x;
         y = tmp_y+ObjPtr->y;
      }
      if (x < ltx) ltx = x; if (y < lty) lty = y;
      if (x > rbx) rbx = x; if (y > rby) rby = y;
   }
   if (retracted_arrow) {
      int i;

      for (i=1; i < num_pts; i++) {
         x = v[i].x; y = v[i].y;
         if (x-ah < ltx) ltx = x-ah; if (y-ah < lty) lty = y-ah;
         if (x+ah > rbx) rbx = x+ah; if (y+ah > rby) rby = y+ah;
      }
   }
   ObjPtr->bbox.ltx = min(ltx, ObjPtr->obbox.ltx-(width>>1));
   ObjPtr->bbox.lty = min(lty, ObjPtr->obbox.lty-(width>>1));
   ObjPtr->bbox.rbx = max(rbx, ObjPtr->obbox.rbx+(width>>1));
   ObjPtr->bbox.rby = max(rby, ObjPtr->obbox.rby+(width>>1));
}

void UpdPolyBBox(ObjPtr, NumPts, V)
   struct ObjRec *ObjPtr;
   int NumPts;
   IntPoint *V;
{
   register int i;
   int ltx, lty, rbx, rby;

   ltx = rbx = V[0].x;
   lty = rby = V[0].y;

   for (i = 1; i < NumPts; i++) {
      if (V[i].x < ltx) ltx = V[i].x; if (V[i].y < lty) lty = V[i].y;
      if (V[i].x > rbx) rbx = V[i].x; if (V[i].y > rby) rby = V[i].y;
   }
   if (ObjPtr->ctm == NULL) {
      ObjPtr->x = ltx;
      ObjPtr->y = lty;
      ObjPtr->obbox.ltx = ltx;
      ObjPtr->obbox.lty = lty;
      ObjPtr->obbox.rbx = rbx;
      ObjPtr->obbox.rby = rby;
   } else {
      IntPoint abs_obj_obbox_vs[5];

      ObjPtr->orig_obbox.ltx = ltx;
      ObjPtr->orig_obbox.lty = lty;
      ObjPtr->orig_obbox.rbx = rbx;
      ObjPtr->orig_obbox.rby = rby;
      GetTransformedOBBoxAbsVs(ObjPtr, abs_obj_obbox_vs);
      ObjPtr->obbox.ltx = min(min(abs_obj_obbox_vs[0].x,abs_obj_obbox_vs[1].x),
            min(abs_obj_obbox_vs[2].x,abs_obj_obbox_vs[3].x));
      ObjPtr->obbox.rbx = max(max(abs_obj_obbox_vs[0].x,abs_obj_obbox_vs[1].x),
            max(abs_obj_obbox_vs[2].x,abs_obj_obbox_vs[3].x));
      ObjPtr->obbox.lty = min(min(abs_obj_obbox_vs[0].y,abs_obj_obbox_vs[1].y),
            min(abs_obj_obbox_vs[2].y,abs_obj_obbox_vs[3].y));
      ObjPtr->obbox.rby = max(max(abs_obj_obbox_vs[0].y,abs_obj_obbox_vs[1].y),
            max(abs_obj_obbox_vs[2].y,abs_obj_obbox_vs[3].y));
   }
   AdjObjBBox(ObjPtr);
}

void UpdNonIntSplinePolyBBox(obj_ptr, num_pts, v)
   struct ObjRec *obj_ptr;
   int num_pts;
   IntPoint *v;
{
   switch (obj_ptr->type) {
   case OBJ_POLY:
      switch (obj_ptr->detail.p->curved) {
      case LT_STRAIGHT:
      case LT_SPLINE:
      case LT_STRUCT_SPLINE:
         UpdPolyBBox(obj_ptr, num_pts, v);
         break;
      case LT_INTSPLINE:
         UpdPolyBBox(obj_ptr, obj_ptr->detail.p->intn,
               obj_ptr->detail.p->intvlist);
         break;
      }
      break;
   case OBJ_POLYGON:
      switch (obj_ptr->detail.g->curved) {
      case LT_STRAIGHT:
      case LT_SPLINE:
      case LT_STRUCT_SPLINE:
         UpdPolyBBox(obj_ptr, num_pts, v);
         break;
      case LT_INTSPLINE:
         UpdPolyBBox(obj_ptr, obj_ptr->detail.g->intn,
               obj_ptr->detail.g->intvlist);
         break;
      }
      break;
   }
}

void UpdPolyOrPolygonBBox(obj_ptr)
   struct ObjRec *obj_ptr;
{
   switch (obj_ptr->type) {
   case OBJ_POLY:
      switch (obj_ptr->detail.p->curved) {
      case LT_STRAIGHT:
      case LT_SPLINE:
      case LT_STRUCT_SPLINE:
         UpdPolyBBox(obj_ptr, obj_ptr->detail.p->n, obj_ptr->detail.p->vlist);
         break;
      case LT_INTSPLINE:
         UpdPolyBBox(obj_ptr, obj_ptr->detail.p->intn,
               obj_ptr->detail.p->intvlist);
         break;
      }
      break;
   case OBJ_POLYGON:
      switch (obj_ptr->detail.g->curved) {
      case LT_STRAIGHT:
      case LT_SPLINE:
      case LT_STRUCT_SPLINE:
         UpdPolyBBox(obj_ptr, obj_ptr->detail.g->n, obj_ptr->detail.g->vlist);
         break;
      case LT_INTSPLINE:
         UpdPolyBBox(obj_ptr, obj_ptr->detail.g->intn,
               obj_ptr->detail.g->intvlist);
         break;
      }
      break;
   }
}

#define CREATE_RELATIVE (FALSE)
#define CREATE_ABSOLUTE (TRUE)

void CreatePolyObj(NumPts, CreateAbsolute)
   int NumPts;
   int CreateAbsolute;
{
   struct PtRec *pt_ptr=NULL, *next_pt=NULL;
   struct PolyRec *poly_ptr=NULL;
   struct ObjRec *obj_ptr=NULL;
   register int i;
   IntPoint *v=NULL;
   int ltx, lty, rbx, rby, num_ss_pts=0;
   char *smooth=NULL;
   CVListElem *elem=NULL;
   IntPointTriplet *pipt=NULL;

   poly_ptr = (struct PolyRec *)malloc(sizeof(struct PolyRec));
   if (poly_ptr == NULL) FailAllocMessage();
   memset(poly_ptr, 0, sizeof(struct PolyRec));
   if (curSpline == LT_STRUCT_SPLINE) {
      num_ss_pts = (NumPts*3)-2;
      poly_ptr->n = num_ss_pts;
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
      poly_ptr->n = NumPts;
      v = (IntPoint*)malloc((NumPts+1)*sizeof(IntPoint));
      if (v == NULL) FailAllocMessage();
      if (curSpline != LT_INTSPLINE) {
         smooth = (char*)malloc((NumPts+1)*sizeof(char));
         if (smooth == NULL) FailAllocMessage();
      }
      pt_ptr = lastPtPtr;
      ltx = rbx = pt_ptr->x;
      lty = rby = pt_ptr->y;

      for (i=NumPts-1; i >= 0; i--, lastPtPtr=next_pt) {
         next_pt = lastPtPtr->next;
         v[i].x = CreateAbsolute ? lastPtPtr->x : ABS_X(lastPtPtr->x);
         v[i].y = CreateAbsolute ? lastPtPtr->y : ABS_Y(lastPtPtr->y);
         if (curSpline != LT_INTSPLINE) {
            if (lastPtPtr->x < ltx) ltx = lastPtPtr->x;
            if (lastPtPtr->y < lty) lty = lastPtPtr->y;
            if (lastPtPtr->x > rbx) rbx = lastPtPtr->x;
            if (lastPtPtr->y > rby) rby = lastPtPtr->y;
            if (curSpline == LT_STRAIGHT) {
               smooth[i] = FALSE;
            } else {
               smooth[i] = (i != 0 && i != NumPts-1);
            }
         }
         free(lastPtPtr);
      }
      numPtsInPoly = 0;
      lastPtPtr = NULL;
   }
   poly_ptr->vlist = v;
   poly_ptr->smooth = smooth;
   poly_ptr->svlist = poly_ptr->asvlist = NULL;
   poly_ptr->intvlist = NULL;
   poly_ptr->style = lineStyle;
   poly_ptr->width = curWidthOfLine[lineWidth];
   poly_ptr->aw = curArrowHeadW[lineWidth];
   poly_ptr->ah = curArrowHeadH[lineWidth];
   UtilStrCpyN(poly_ptr->width_spec, sizeof(poly_ptr->width_spec),
         curWidthOfLineSpec[lineWidth]);
   UtilStrCpyN(poly_ptr->aw_spec, sizeof(poly_ptr->aw_spec),
         curArrowHeadWSpec[lineWidth]);
   UtilStrCpyN(poly_ptr->ah_spec, sizeof(poly_ptr->ah_spec),
         curArrowHeadHSpec[lineWidth]);
   poly_ptr->pen = penPat;
   poly_ptr->curved = curSpline;
   poly_ptr->fill = objFill;
   poly_ptr->dash = curDash;
   /*
    * poly_ptr->tighter = (curSpline == LT_STRUCT_SPLINE) ? 
    *       tighterStructSplines : FALSE;
    */
   poly_ptr->rotated_n = 0;
   poly_ptr->rotated_vlist = NULL;
   poly_ptr->rotated_asn = 0;
   poly_ptr->rotated_asvlist = NULL;

   obj_ptr = (struct ObjRec *)malloc(sizeof(struct ObjRec));
   if (obj_ptr == NULL) FailAllocMessage();
   memset(obj_ptr, 0, sizeof(struct ObjRec));
   obj_ptr->detail.p = poly_ptr;

   obj_ptr->color = colorIndex;
   if (mainDisplay != NULL) {
      UtilStrCpyN(obj_ptr->color_str, sizeof(obj_ptr->color_str),
            colorMenuItems[colorIndex]);
   }
   obj_ptr->type = OBJ_POLY;
   if (CreateAbsolute) {
      obj_ptr->obbox.ltx = obj_ptr->x = ltx;
      obj_ptr->obbox.lty = obj_ptr->y = lty;
      obj_ptr->obbox.rbx = rbx;
      obj_ptr->obbox.rby = rby;
   } else {
      obj_ptr->obbox.ltx = obj_ptr->x = ABS_X(ltx);
      obj_ptr->obbox.lty = obj_ptr->y = ABS_Y(lty);
      obj_ptr->obbox.rbx = ABS_X(rbx);
      obj_ptr->obbox.rby = ABS_Y(rby);
   }
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
      UpdPolyBBox(obj_ptr, poly_ptr->n, poly_ptr->vlist);
   } else {
      UpdPolyBBox(obj_ptr, poly_ptr->intn, poly_ptr->intvlist);
   }
   AdjObjBBox(obj_ptr);
   AddObj(NULL, topObj, obj_ptr);
}

void ResetCreatePoly()
{
   numPtsInPoly = 0;
   lastPtPtr = NULL;
   CVListInit(&gStructSplineList);
}

void AddPtToCreatePoly(AbsX, AbsY)
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

#ifdef HP_LINE_BUG
static
void MyDrawLines(dpy, win, gc, sv, sn)
   Display *dpy;
   Window win;
   GC gc;
   XPoint *sv;
   int sn;
{
   if (sn == 2) {
      XPoint hp_sv[3];

      hp_sv[0].x = sv[0].x; hp_sv[0].y = sv[0].y;
      hp_sv[1].x = sv[0].x; hp_sv[1].y = sv[0].y;
      hp_sv[2].x = sv[1].x; hp_sv[2].y = sv[1].y;
      XDrawLines(dpy, win, gc, hp_sv, 3, CoordModeOrigin);
   } else {
      XDrawLines(dpy, win, gc, sv, sn, CoordModeOrigin);
   }
}
#else /* ~HP_LINE_BUG */
#define MyDrawLines(dpy,win,gc,sv,sn) \
        XDrawLines((dpy),(win),(gc),(sv),(sn),CoordModeOrigin)
#endif /* HP_LINE_BUG */

static
void SetXorDrawGCForPoly(color_index, line_width_index)
   int color_index, line_width_index;
{
   XGCValues values;

   values.fill_style = FillSolid;
#ifdef NO_THIN_LINE
   values.line_width = 1;
#else /* ~NO_THIN_LINE */
   values.line_width = 0;
#endif /* NO_THIN_LINE */
   if (curChoice == FREEHAND) {
      values.foreground = colorPixels[color_index];
      values.function = GXcopy;
      values.line_width = curWidthOfLine[line_width_index];
#ifdef NO_THIN_LINE
      if (curWidthOfLine[lineWidth] < 1) values.line_width = 1;
#endif /* NO_THIN_LINE */
   } else {
      values.foreground = xorColorPixels[color_index];
      values.function = GXxor;
   }
   values.line_style = LineSolid;
   values.join_style = JoinBevel;

   XChangeGC(mainDisplay, drawGC,
         GCForeground | GCFunction | GCFillStyle | GCLineWidth | GCLineStyle |
         GCJoinStyle, &values);
}

#define POLY_DRAW  (FALSE)
#define POLY_ERASE (TRUE)
#define POLY_CLICK (FALSE)
#define POLY_DRAG  (TRUE)

#define POLY_STARTSHOW 0
#define POLY_DOSHOW    1
#define POLY_ENDSHOW   2

typedef struct tagPolyMeasureCursorInfo {
   IntPoint first_pt;
   IntPoint prev_prev_pt;
   IntPoint prev_pt;
} PolyMeasureCursorInfo;

static
void DoPolyMeasureCursor(ppmci, start, num_pts, x, y, dx, dy, erase, drag)
   PolyMeasureCursorInfo *ppmci;
   int start, num_pts, x, y, dx, dy, erase, drag;
{
   char buf[80], w_buf[80], h_buf[80], x_buf[80], y_buf[80], a_buf[80];
   int angle2=0;

   if (erase == POLY_DRAW && drag == POLY_CLICK) {
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
   if (curChoice == FREEHAND) {
      sprintf(buf, "x=%s\ny=%s", x_buf, y_buf);
   } else {
      sprintf(buf, "w=%s\nh=%s\nx=%s\ny=%s\nangle=%s", w_buf, h_buf, x_buf,
            y_buf, a_buf);
   }
   x = OFFSET_X(x);
   y = OFFSET_Y(y);
   switch (start) {
   case POLY_STARTSHOW: StartShowMeasureCursor(x, y, buf, TRUE); break;
   case POLY_DOSHOW: ShowMeasureCursor(x, y, buf, TRUE); break;
   case POLY_ENDSHOW: EndShowMeasureCursor(x, y, buf, TRUE); break;
   }
}

void FreePointsForCont(pn_ltx, pn_lty, pn_rbx, pn_rby)
   int *pn_ltx, *pn_lty, *pn_rbx, *pn_rby;
{
   int ltx=(*pn_ltx), lty=(*pn_lty), rbx=(*pn_rbx), rby=(*pn_rby);

   if (curSpline == LT_STRUCT_SPLINE) {
      CVListElem *elem=NULL;

      for (elem=ListFirst(&gStructSplineList); elem != NULL;
            elem=ListNext(&gStructSplineList, elem)) {
         IntPointTriplet *ipt=(IntPointTriplet*)(elem->obj);

         if (ipt != NULL) {
            if (ipt->hinge_pt.x < ltx) ltx = ipt->hinge_pt.x;
            if (ipt->hinge_pt.y < lty) lty = ipt->hinge_pt.y;
            if (ipt->hinge_pt.x > rbx) rbx = ipt->hinge_pt.x;
            if (ipt->hinge_pt.y > rby) rby = ipt->hinge_pt.y;
            if (ipt->earlier_valid) {
               if (ipt->earlier_smooth_pt.x < ltx) {
                  ltx = ipt->earlier_smooth_pt.x;
               }
               if (ipt->earlier_smooth_pt.y < lty) {
                  lty = ipt->earlier_smooth_pt.y;
               }
               if (ipt->earlier_smooth_pt.x > rbx) {
                  rbx = ipt->earlier_smooth_pt.x;
               }
               if (ipt->earlier_smooth_pt.y > rby) {
                  rby = ipt->hinge_pt.y;
               }
            }
            if (ipt->later_valid) {
               if (ipt->later_smooth_pt.x < ltx) ltx = ipt->later_smooth_pt.x;
               if (ipt->later_smooth_pt.y < lty) lty = ipt->later_smooth_pt.y;
               if (ipt->later_smooth_pt.x > rbx) rbx = ipt->later_smooth_pt.x;
               if (ipt->later_smooth_pt.y > rby) rby = ipt->later_smooth_pt.y;
            }
            free(ipt);
         }
      }
      ListUnlinkAll(&gStructSplineList);
   } else {
      struct PtRec *pt_ptr=NULL, *next_pt=NULL;

      for (pt_ptr=lastPtPtr; pt_ptr != NULL; pt_ptr=next_pt) {
         if (curSpline != LT_INTSPLINE) {
            if (pt_ptr->x < ltx) ltx = pt_ptr->x;
            if (pt_ptr->y < lty) lty = pt_ptr->y;
            if (pt_ptr->x > rbx) rbx = pt_ptr->x;
            if (pt_ptr->y > rby) rby = pt_ptr->y;
         }
         next_pt = pt_ptr->next;
         free(pt_ptr);
      }
   }
   *pn_ltx = ltx;
   *pn_lty = lty;
   *pn_rbx = rbx;
   *pn_rby = rby;
}

int SetFirstPoint(grid_x, grid_y, pipt_first)
   int grid_x, grid_y;
   IntPointTriplet *pipt_first;
{
   if (curSpline == LT_STRUCT_SPLINE) {
      IntPointTriplet *ipt=(IntPointTriplet*)malloc(sizeof(IntPointTriplet));

      if (ipt == NULL) return FailAllocMessage();
      memset(ipt, 0, sizeof(IntPointTriplet));
      if (pipt_first == NULL) {
         ipt->earlier_smooth_pt.x = ipt->hinge_pt.x = ipt->later_smooth_pt.x =
               grid_x; /* offset */
         ipt->earlier_smooth_pt.y = ipt->hinge_pt.y = ipt->later_smooth_pt.y =
               grid_y; /* offset */
         ipt->ratio = (double)1;
      } else {
         memcpy(ipt, pipt_first, sizeof(IntPointTriplet));
      }
      ListAppend(&gStructSplineList, ipt);
   } else {
      numPtsInPoly = 1;
      lastPtPtr = (struct PtRec *)malloc(sizeof(struct PtRec));
      if (lastPtPtr == NULL) return FailAllocMessage();
      lastPtPtr->x = grid_x; /* offset */
      lastPtPtr->y = grid_y; /* offset */
      lastPtPtr->next = NULL;
   }
   return TRUE;
}

int AddPointForCont(grid_x, grid_y, pipt)
   int grid_x, grid_y;
   IntPointTriplet *pipt;
{
   if (curSpline == LT_STRUCT_SPLINE) {
      IntPointTriplet *ipt=(IntPointTriplet*)malloc(sizeof(IntPointTriplet));

      if (ipt == NULL) return FailAllocMessage();
      memcpy(ipt, pipt, sizeof(IntPointTriplet));
      ListAppend(&gStructSplineList, ipt);
   } else {
      struct PtRec *pt_ptr=NULL;

      pt_ptr = (struct PtRec *)malloc(sizeof(struct PtRec));
      if (pt_ptr == NULL) FailAllocMessage();
      pt_ptr->next = lastPtPtr;
      lastPtPtr = pt_ptr;
      pt_ptr->x = grid_x;
      pt_ptr->y = grid_y;
   }
   return TRUE;
}

void UpdateLastPointForCont(pipt)
   IntPointTriplet *pipt;
{
   CVListElem *elem=ListLast(&gStructSplineList);
   IntPointTriplet *ipt=(IntPointTriplet*)(elem->obj);

   memcpy(ipt, pipt, sizeof(IntPointTriplet));
}

void DrawAllStructSplinePointsForCont(num_pts)
   int num_pts;
{
   int i=0;
   CVListElem *elem=ListFirst(&gStructSplineList);
   IntPointTriplet *pipt=(IntPointTriplet*)(elem->obj);

   for (i=0; i < num_pts-1; i++) {
      CVListElem *next_elem=ListNext(&gStructSplineList, elem);
      IntPointTriplet *pipt_next=(IntPointTriplet*)(next_elem->obj);
      int tmp_n=0;
      IntPoint tmp_vs[4];

      tmp_vs[0].x = ABS_X(pipt->hinge_pt.x);
      tmp_vs[0].y = ABS_Y(pipt->hinge_pt.y);
      if (pipt->later_valid) {
         if (pipt_next->earlier_valid) {
            tmp_n = 4;
            tmp_vs[1].x = ABS_X(pipt->later_smooth_pt.x);
            tmp_vs[1].y = ABS_Y(pipt->later_smooth_pt.y);
            tmp_vs[2].x = ABS_X(pipt_next->earlier_smooth_pt.x);
            tmp_vs[2].y = ABS_Y(pipt_next->earlier_smooth_pt.y);
         } else {
            tmp_n = 3;
            tmp_vs[1].x = ABS_X(pipt->later_smooth_pt.x);
            tmp_vs[1].y = ABS_Y(pipt->later_smooth_pt.y);
         }
      } else {
         if (pipt_next->earlier_valid) {
            tmp_n = 3;
            tmp_vs[1].x = ABS_X(pipt_next->earlier_smooth_pt.x);
            tmp_vs[1].y = ABS_Y(pipt_next->earlier_smooth_pt.y);
         } else {
            tmp_n = 2;
            XDrawLine(mainDisplay, drawWindow, drawGC, pipt->hinge_pt.x,
                  pipt->hinge_pt.y, pipt_next->hinge_pt.x,
                  pipt_next->hinge_pt.y);
         }
      }
      tmp_vs[tmp_n-1].x = ABS_X(pipt_next->hinge_pt.x);
      tmp_vs[tmp_n-1].y = ABS_Y(pipt_next->hinge_pt.y);

      if (tmp_n > 2) {
         int sn=0;
         XPoint *sv=MakeSplinePolyVertex(0, curSpline, &sn, drawOrigX,
               drawOrigY, tmp_n, tmp_vs);

         XDrawLines(mainDisplay, drawWindow, drawGC, sv, sn, CoordModeOrigin);
         free(sv);
      }
   }
}

void UpdatePrevToLastPointForCont(pipt_prev)
   IntPointTriplet *pipt_prev;
{
   CVListElem *elem=ListLast(&gStructSplineList);
   IntPointTriplet *pipt=(IntPointTriplet*)(elem->obj);

   elem = ListPrev(&gStructSplineList, elem);
   pipt = (IntPointTriplet*)(elem->obj);
   memcpy(pipt_prev, pipt, sizeof(IntPointTriplet));
}

static
void ContinueForStructSplinePolyControlPoints(OrigX, OrigY, LastX, LastY, psv,
      psn, pipt_prev, pipt, pn_abort)
   int OrigX, OrigY, LastX, LastY, *psn, *pn_abort;
   XPoint **psv;
   IntPointTriplet *pipt_prev, *pipt;
{
   int i=0, done=FALSE, grid_x=0, grid_y=0, end_x=0, end_y=0, n=2;
   int sn=(*psn), orig_sn=(*psn), num_pts=1, first_time=TRUE;
   IntPoint v[4];
   XPoint *sv=NULL, *orig_sv=(*psv), dash_vs[2];
   PolyMeasureCursorInfo pmci;
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
   DoPolyMeasureCursor(&pmci, POLY_DOSHOW, num_pts, ABS_X(grid_x),
         ABS_Y(grid_y), 0, 0, POLY_ERASE, POLY_DRAG);
   while (!done) {
      XEvent input, ev;

      XNextEvent(mainDisplay, &input);

      if (input.type == Expose || input.type == VisibilityNotify) {
         ExposeEventHandler(&input, TRUE);
         SetXorDrawGCForPoly(colorIndex, lineWidth);
      } else if (input.type == MotionNotify) {
         end_x = input.xmotion.x;
         end_y = input.xmotion.y;

         /* erase */
         if (first_time || grid_x != LastX || grid_y != LastY) {
            if (sv != NULL) {
               MyDrawLines(mainDisplay, drawWindow, drawGC, sv, sn);
            } else {
               XDrawLine(mainDisplay, drawWindow, drawGC, OrigX, OrigY, grid_x,
                     grid_y);
            }
            first_time = FALSE;
         } else {
            if (orig_sv != NULL) {
               MyDrawLines(mainDisplay, drawWindow, drawGC, orig_sv, orig_sn);
            } else {
               XDrawLine(mainDisplay, drawWindow, drawGC, OrigX, OrigY, LastX,
                     LastY);
            }
         }
         DoPolyMeasureCursor(&pmci, POLY_DOSHOW, num_pts, ABS_X(grid_x),
               ABS_Y(grid_y), ABS_SIZE(abs(grid_x-LastX)),
               ABS_SIZE(abs(grid_y-LastY)), POLY_ERASE, POLY_DRAG);
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
            MyDrawLines(mainDisplay, drawWindow, drawGC, sv, sn);
         } else {
            if (orig_sv != NULL) {
               MyDrawLines(mainDisplay, drawWindow, drawGC, orig_sv, orig_sn);
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
         DoPolyMeasureCursor(&pmci, POLY_DOSHOW, num_pts, ABS_X(grid_x),
               ABS_Y(grid_y), ABS_SIZE(abs(grid_x-LastX)),
               ABS_SIZE(abs(grid_y-LastY)), POLY_DRAW, POLY_DRAG);
      } else if (input.type == KeyPress) {
         if (KeyPressEventIsEscape(&input.xkey)) {
            /* erase */
            DoPolyMeasureCursor(&pmci, POLY_ENDSHOW, num_pts, ABS_X(grid_x),
                  ABS_Y(grid_y), ABS_SIZE(abs(grid_x-LastX)),
                  ABS_SIZE(abs(grid_y-LastY)), POLY_ERASE, POLY_DRAG);
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
         DoPolyMeasureCursor(&pmci, POLY_ENDSHOW, num_pts, ABS_X(grid_x),
               ABS_Y(grid_y), ABS_SIZE(abs(grid_x-LastX)),
               ABS_SIZE(abs(grid_y-LastY)), POLY_ERASE, POLY_DRAG);
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
         } else {
            pipt->earlier_valid = pipt->later_valid = TRUE;
         }
         done = TRUE;
      }
   }
   MARKHR(drawWindow, revDefaultGC, LastX, LastY);

   *psv = sv;
   *psn = sn;

   if (orig_sv != NULL) free(orig_sv);
}

static
void ContinuePoly(OrigX, OrigY)
   int OrigX, OrigY;
   /* OrigX and OrigY are screen coordinates (scaled and translated). */
   /* OrigX and OrigY are also on grid. */
{
   int i;
   XGCValues values;
   XEvent input, ev;
   XButtonEvent *button_ev;
   int abort=FALSE;
   int end_x, end_y, grid_x, grid_y, done=FALSE, num_pts=1;
   int last_x=OrigX, last_y=OrigY, n=2, sn=0, max_n=40, intn=0;
   int ltx=OrigX, lty=OrigY, rbx=OrigX, rby=OrigY;
   int one_line_status=FALSE, freehand_n=0, nothing_is_drawn=FALSE;
   char status_buf[MAX_STATUS_BTNS+1][MAXSTRING+1];
   IntPointTriplet ipt_prev, ipt, first_ipt;
   XPoint *sv=NULL, *freehand_vs=NULL;
   IntPoint *v=NULL, *cntrlv=NULL;
   PolyMeasureCursorInfo pmci;

   memset(&ipt, 0, sizeof(IntPointTriplet));
   memset(&ipt_prev, 0, sizeof(IntPointTriplet));
   memset(&first_ipt, 0, sizeof(IntPointTriplet));
   memset(&pmci, 0, sizeof(PolyMeasureCursorInfo));
   SetXorDrawGCForPoly(colorIndex, lineWidth);

   grid_x = end_x = OrigX;
   grid_y = end_y = OrigY;
   if (curChoice == FREEHAND) {
      freehand_vs = (XPoint*)malloc((max_n+1)*sizeof(XPoint));
      if (freehand_vs == NULL) FailAllocMessage();
      freehand_vs[0].x = freehand_vs[1].x = OrigX;
      freehand_vs[0].y = freehand_vs[1].y = OrigY;
      freehand_n = 1;
   } else if (curSpline != LT_STRAIGHT && curSpline != LT_STRUCT_SPLINE) {
      v = (IntPoint*)malloc((max_n+1)*sizeof(IntPoint));
      if (v == NULL) FailAllocMessage();
      v[0].x = v[1].x = v[2].x = ABS_X(OrigX);
      v[0].y = v[1].y = v[2].y = ABS_Y(OrigY);
      switch (curSpline) {
      case LT_SPLINE:
         sv = MakeSplinePolyVertex(0, curSpline, &sn, drawOrigX, drawOrigY, n,
               v);
         break;
      case LT_INTSPLINE:
         sv = MakeIntSplinePolyVertex(&sn, &intn, &cntrlv,
               drawOrigX, drawOrigY, n, v);
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
      v = (IntPoint*)malloc((max_n+1)*sizeof(IntPoint));
      if (v == NULL) FailAllocMessage();
      v[0].x = v[1].x = v[2].x = v[3].x = ABS_X(OrigX);
      v[0].y = v[1].y = v[2].y = v[3].y = ABS_Y(OrigY);
      sv = MakeSplinePolyVertex(0, curSpline, &sn, drawOrigX, drawOrigY, n+1,
            v);
   }
   SaveStatusStrings();
   if (curChoice == FREEHAND) {
      DoPolyMeasureCursor(&pmci, POLY_STARTSHOW, num_pts, ABS_X(grid_x),
            ABS_Y(grid_y), 0, 0, POLY_DRAW, POLY_CLICK);
      if (!debugNoPointerGrab) {
         XGrabPointer(mainDisplay, drawWindow, FALSE,
               PointerMotionMask | ButtonReleaseMask,
               GrabModeAsync, GrabModeAsync, None, handCursor, CurrentTime);
      }
      SetFirstPoint(OrigX, OrigY, NULL);
   } else {
      if (curSpline == LT_STRUCT_SPLINE) {
         SetMouseStatus(TgLoadCachedString(CSTID_ADD_A_STRUCT_VERTEX),
               TgLoadCachedString(CSTID_ADD_LAST_VERTEX),
               TgLoadCachedString(CSTID_ADD_LAST_VERTEX));
      } else {
         SetMouseStatus(TgLoadCachedString(CSTID_ADD_A_VERTEX),
               TgLoadCachedString(CSTID_ADD_LAST_VERTEX),
               TgLoadCachedString(CSTID_ADD_LAST_VERTEX));
      }
      DoPolyMeasureCursor(&pmci, POLY_STARTSHOW, num_pts, ABS_X(grid_x),
            ABS_Y(grid_y), 0, 0, POLY_DRAW, POLY_CLICK);
      if (!debugNoPointerGrab) {
         XGrabPointer(mainDisplay, drawWindow, FALSE,
               PointerMotionMask | ButtonPressMask | ButtonReleaseMask,
               GrabModeAsync, GrabModeAsync, None, handCursor, CurrentTime);
      }
      if (curSpline == LT_STRUCT_SPLINE) {
         DoPolyMeasureCursor(&pmci, POLY_DOSHOW, num_pts, ABS_X(grid_x),
               ABS_Y(grid_y), 0, 0, POLY_ERASE, POLY_DRAG);
         ContinueForStructSplinePolyControlPoints(OrigX, OrigY,
               grid_x, grid_y, &sv, &sn, &ipt_prev, &ipt, &abort);
         memcpy(&first_ipt, &ipt, sizeof(IntPointTriplet));
         memcpy(&ipt_prev, &ipt, sizeof(IntPointTriplet));
         if (first_ipt.later_valid) {
            grid_x = OFFSET_X(first_ipt.later_smooth_pt.x);
            grid_y = OFFSET_Y(first_ipt.later_smooth_pt.y);
         }
         SetFirstPoint(OrigX, OrigY, &first_ipt);
         DoPolyMeasureCursor(&pmci, POLY_STARTSHOW, num_pts, ABS_X(grid_x),
               ABS_Y(grid_y), ABS_SIZE(abs(grid_x-OrigX)),
               ABS_SIZE(abs(grid_y-OrigY)), POLY_DRAW, POLY_CLICK);
      } else {
         SetFirstPoint(OrigX, OrigY, NULL);
      }
   }
   if (gstWiringInfo.num_ports_to_connect > 0 || drawPolyToConnectPins > 0) {
      *gstWiringInfo.last_port_name = '\0';
      drawPolyHighlightedNode = NULL;
      SaveStatusStringsIntoBuf(status_buf, &one_line_status);
      values.line_width = 3;
      XChangeGC(mainDisplay, revGrayGC, GCLineWidth, &values);
   }
   while (!done) {
      XNextEvent(mainDisplay, &input);

      if (input.type == Expose || input.type == VisibilityNotify) {
         ExposeEventHandler(&input, TRUE);
         SetXorDrawGCForPoly(colorIndex, lineWidth);
      } else if ((input.type==MotionNotify && input.xany.window==drawWindow) ||
            input.type == KeyPress || input.type == KeyRelease) {
         if (input.type == KeyPress) {
            if (KeyPressEventIsEscape(&input.xkey)) {
               /* erase */
               DoPolyMeasureCursor(&pmci, POLY_ENDSHOW, num_pts, ABS_X(grid_x),
                     ABS_Y(grid_y), ABS_SIZE(abs(grid_x-OrigX)),
                     ABS_SIZE(abs(grid_y-OrigY)), POLY_ERASE, POLY_CLICK);
               Msg("");
               abort = TRUE;
               done = TRUE;
            }
         }
         if (done) {
            break;
         }
         /* erase */
         if (curChoice == FREEHAND) {
            if (freehand_n > 1) {
               MyDrawLines(mainDisplay, drawWindow, drawGC,
                     freehand_vs, freehand_n);
            }
         } else {
            if (curSpline != LT_STRAIGHT && curSpline != LT_STRUCT_SPLINE) {
               MyDrawLines(mainDisplay, drawWindow, drawGC, sv, sn);
            } else if (curSpline == LT_STRUCT_SPLINE) {
               if (nothing_is_drawn) {
                  /* then there is nothing to erase */
                  nothing_is_drawn = FALSE;
               } else {
                  if (sv == NULL) {
                     XDrawLine(mainDisplay, drawWindow, drawGC, OrigX, OrigY,
                           grid_x, grid_y);
                  } else {
                     MyDrawLines(mainDisplay, drawWindow, drawGC, sv, sn);
                  }
               }
            } else {
               XDrawLine(mainDisplay, drawWindow, drawGC, OrigX, OrigY, grid_x,
                     grid_y);
            }
         }
         DoPolyMeasureCursor(&pmci, POLY_DOSHOW, num_pts, ABS_X(grid_x),
               ABS_Y(grid_y), ABS_SIZE(abs(grid_x-OrigX)),
               ABS_SIZE(abs(grid_y-OrigY)), POLY_ERASE, POLY_DRAG);

         if (input.type == KeyPress || input.type == KeyRelease) {
            end_x = grid_x;
            end_y = grid_y;
         } else {
            end_x = input.xmotion.x;
            end_y = input.xmotion.y;
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
         if (curChoice == FREEHAND) {
            grid_x = end_x;
            grid_y = end_y;
            MarkRulers(grid_x, grid_y);
            if (curSpline != LT_STRAIGHT && curSpline != LT_STRUCT_SPLINE) {
               if (sv != NULL) {
                  free(sv);
                  sv = NULL;
               }
               v[n-1].x = v[n].x = ABS_X(grid_x);
               v[n-1].y = v[n].y = ABS_Y(grid_y);
               switch (curSpline) {
               case LT_SPLINE:
                  sv = MakeSplinePolyVertex(0, curSpline, &sn, drawOrigX,
                        drawOrigY, n, v);
                  break;
               case LT_INTSPLINE:
                  if (cntrlv != NULL) free(cntrlv);
                  sv = MakeIntSplinePolyVertex(&sn, &intn, &cntrlv,
                        drawOrigX, drawOrigY, n, v);
                  for (i=0; i < sn; i++) {
                     if (sv[i].x < ltx) ltx = sv[i].x;
                     if (sv[i].y < lty) lty = sv[i].y;
                     if (sv[i].x > rbx) rbx = sv[i].x;
                     if (sv[i].y > rby) rby = sv[i].y;
                  }
                  break;
               }
               MyDrawLines(mainDisplay, drawWindow, drawGC, sv, sn);
            } else {
               XDrawLine(mainDisplay, drawWindow, drawGC, OrigX, OrigY, grid_x,
                     grid_y);
               if (sv != NULL) {
                  free(sv);
                  sv = NULL;
               }
            }
            while (XCheckMaskEvent(mainDisplay, PointerMotionMask, &ev)) ;

            if (grid_x != last_x || grid_y != last_y) {
               num_pts++;
               AddPointForCont(grid_x, grid_y, &ipt);
               last_x = grid_x;
               last_y = grid_y;
               if (curSpline != LT_STRAIGHT && curSpline != LT_STRUCT_SPLINE) {
                  if (n >= max_n-2) {
                     max_n += 40;
                     v = (IntPoint*)realloc(v, sizeof(IntPoint)*max_n+1);
                     if (v == NULL) FailAllocMessage();
                  }
                  MyDrawLines(mainDisplay, drawWindow, drawGC, sv, sn);
                  if (sv != NULL) {
                     free(sv);
                     sv = NULL;
                  }
                  v[n].x = v[n+1].x = ABS_X(grid_x);
                  v[n].y = v[n+1].y = ABS_Y(grid_y);
                  n++;
                  switch (curSpline) {
                  case LT_SPLINE:
                     sv = MakeSplinePolyVertex(0, curSpline, &sn, drawOrigX,
                           drawOrigY, n, v);
                     break;
                  case LT_INTSPLINE:
                     if (cntrlv != NULL) free(cntrlv);
                     sv = MakeIntSplinePolyVertex(&sn, &intn, &cntrlv,
                           drawOrigX, drawOrigY, n, v);
                     for (i=0; i < sn; i++) {
                        if (sv[i].x < ltx) ltx = sv[i].x;
                        if (sv[i].y < lty) lty = sv[i].y;
                        if (sv[i].x > rbx) rbx = sv[i].x;
                        if (sv[i].y > rby) rby = sv[i].y;
                     }
                     break;
                  }
                  MyDrawLines(mainDisplay, drawWindow, drawGC, sv, sn);
               } else {
                  if (freehand_n >= max_n-2) {
                     max_n += 40;
                     freehand_vs = (XPoint*)realloc(freehand_vs,
                           sizeof(XPoint)*max_n+1);
                     if (freehand_vs == NULL) FailAllocMessage();
                  }
                  freehand_vs[freehand_n].x = grid_x;
                  freehand_vs[freehand_n].y = grid_y;
                  freehand_n++;
               }
            }
            OrigX = grid_x; OrigY = grid_y;
         } else {
            if (gstWiringInfo.num_ports_to_connect > 0) {
               int need_to_highlight=FALSE, something_changed=FALSE;
               struct ObjRec *owner_obj=NULL, *obj_ptr, *obj_under_cursor=NULL;
               char port_name[MAXSTRING];

               obj_ptr = FindAnObj(end_x, end_y, &owner_obj, &obj_under_cursor,
                     port_name);
               if (drawPolyHighlightedNode != NULL) {
                  if (obj_under_cursor != drawPolyHighlightedNode) {
                     /* un-highlight */
                     SelBox(drawWindow, revGrayGC,
                           OFFSET_X(drawPolyHighlightedNode->bbox.ltx)-2,
                           OFFSET_Y(drawPolyHighlightedNode->bbox.lty)-2,
                           OFFSET_X(drawPolyHighlightedNode->bbox.rbx)+2,
                           OFFSET_Y(drawPolyHighlightedNode->bbox.rby)+2);
                     if (obj_under_cursor != NULL &&
                           ObjIsAPort(obj_under_cursor)) {
                        drawPolyHighlightedNode = obj_under_cursor;
                        SetWiringNodeInfo(obj_under_cursor, owner_obj,
                              port_name, FALSE);
                     } else {
                        drawPolyHighlightedNode = NULL;
                        SetWiringNodeInfo(NULL, NULL, NULL, FALSE);
                     }
                     if (drawPolyHighlightedNode != NULL) {
                        need_to_highlight = TRUE;
                     }
                     something_changed = TRUE;
                  }
               } else {
                  if (obj_under_cursor != NULL) {
                     if (ObjIsAPort(obj_under_cursor)) {
                        drawPolyHighlightedNode = obj_under_cursor;
                        SetWiringNodeInfo(obj_under_cursor, owner_obj,
                              port_name, FALSE);
                     } else {
                        drawPolyHighlightedNode = NULL;
                        SetWiringNodeInfo(NULL, NULL, NULL, FALSE);
                     }
                     if (drawPolyHighlightedNode != NULL) {
                        need_to_highlight = TRUE;
                        something_changed = TRUE;
                     }
                  }
               }
               if (need_to_highlight) {
                  SelBox(drawWindow, revGrayGC,
                        OFFSET_X(drawPolyHighlightedNode->bbox.ltx)-2,
                        OFFSET_Y(drawPolyHighlightedNode->bbox.lty)-2,
                        OFFSET_X(drawPolyHighlightedNode->bbox.rbx)+2,
                        OFFSET_Y(drawPolyHighlightedNode->bbox.rby)+2);
               }
               if (something_changed) {
                  if (*gstWiringInfo.last_port_name != '\0') {
                     sprintf(gszMsgBox,
                           TgLoadCachedString(CSTID_END_A_WIRE_AT_NAMED_PORT),
                           gstWiringInfo.last_port_name);
                     SetStringStatus(gszMsgBox);
                  } else {
                     RestoreStatusStringsFromBuf(status_buf, one_line_status);
                     SetMouseStatus(TgLoadCachedString(CSTID_ADD_A_VERTEX),
                           TgLoadCachedString(CSTID_ADD_LAST_VERTEX),
                           TgLoadCachedString(CSTID_ADD_LAST_VERTEX));
                  }
               }
               if (drawPolyHighlightedNode != NULL) {
                  grid_x = OFFSET_X((drawPolyHighlightedNode->obbox.ltx +
                        drawPolyHighlightedNode->obbox.rbx)>>1);
                  grid_y = OFFSET_Y((drawPolyHighlightedNode->obbox.lty +
                        drawPolyHighlightedNode->obbox.rby)>>1);
               } else {
                  GridXY(end_x, end_y, &grid_x, &grid_y);
               }
#ifdef _NOT_DEFINED
            /*
             * drawPolyToConnectPins is only set to > 0 in "pin.c"
             * what's in "pin.c" is not used at this time
             */
            } else if (drawPolyToConnectPins > 0) {
               HandlePinHighlights(end_x, end_y);
               if (drawPolyHighlightedNode != NULL) {
                  grid_x = OFFSET_X((drawPolyHighlightedNode->obbox.ltx +
                        drawPolyHighlightedNode->obbox.rbx)>>1);
                  grid_y = OFFSET_Y((drawPolyHighlightedNode->obbox.lty +
                        drawPolyHighlightedNode->obbox.rby)>>1);
               } else {
                  GridXY(end_x, end_y, &grid_x, &grid_y);
               }
#endif /* _NOT_DEFINED */
            } else {
               GridXY(end_x, end_y, &grid_x, &grid_y);
            }
            MarkRulers(grid_x, grid_y);
            if (curSpline != LT_STRAIGHT && curSpline != LT_STRUCT_SPLINE) {
               if (sv != NULL) {
                  free(sv);
                  sv = NULL;
               }
               v[n-1].x = v[n].x = ABS_X(grid_x);
               v[n-1].y = v[n].y = ABS_Y(grid_y);
               switch (curSpline) {
               case LT_SPLINE:
                  sv = MakeSplinePolyVertex(0, curSpline, &sn, drawOrigX,
                        drawOrigY, n, v);
                  break;
               case LT_INTSPLINE:
                  free(cntrlv);
                  sv = MakeIntSplinePolyVertex(&sn, &intn, &cntrlv,
                        drawOrigX, drawOrigY, n, v);
                  for (i=0; i < sn; i++) {
                     if (sv[i].x < ltx) ltx = sv[i].x;
                     if (sv[i].y < lty) lty = sv[i].y;
                     if (sv[i].x > rbx) rbx = sv[i].x;
                     if (sv[i].y > rby) rby = sv[i].y;
                  }
                  break;
               }
               MyDrawLines(mainDisplay, drawWindow, drawGC, sv, sn);
            } else if (curSpline == LT_STRUCT_SPLINE) {
               if (sv != NULL) {
                  free(sv);
                  sv = NULL;
               }
               if (ipt_prev.later_valid) {
                  v[0].x = ABS_X(OrigX);
                  v[0].y = ABS_Y(OrigY);
                  v[1].x = ABS_X(ipt_prev.later_smooth_pt.x);
                  v[1].y = ABS_Y(ipt_prev.later_smooth_pt.y);
                  v[2].x = v[3].x = ABS_X(grid_x);
                  v[2].y = v[3].y = ABS_Y(grid_y);
                  n = 3;
                  sv = MakeSplinePolyVertex(0, curSpline, &sn, drawOrigX,
                        drawOrigY, n, v);
                  MyDrawLines(mainDisplay, drawWindow, drawGC, sv, sn);
               } else {
                  XDrawLine(mainDisplay, drawWindow, drawGC, OrigX, OrigY,
                        grid_x, grid_y);
                  if (sv != NULL) {
                     free(sv);
                     sv = NULL;
                  }
               }
            } else {
               XDrawLine(mainDisplay, drawWindow, drawGC, OrigX, OrigY,
                     grid_x, grid_y);
            }
         }
         DoPolyMeasureCursor(&pmci, POLY_DOSHOW, num_pts, ABS_X(grid_x),
               ABS_Y(grid_y), ABS_SIZE(abs(grid_x-OrigX)),
               ABS_SIZE(abs(grid_y-OrigY)), POLY_DRAW, POLY_DRAG);
         while (XCheckMaskEvent(mainDisplay, PointerMotionMask, &ev)) ;
      } else if (input.type == ButtonPress && curChoice != FREEHAND) {
         /* erase */
         DoPolyMeasureCursor(&pmci, POLY_ENDSHOW, num_pts, ABS_X(grid_x),
               ABS_Y(grid_y), ABS_SIZE(abs(grid_x-OrigX)),
               ABS_SIZE(abs(grid_y-OrigY)), POLY_ERASE, POLY_DRAG);

         button_ev = &(input.xbutton);

         end_x = button_ev->x;
         end_y = button_ev->y;
         if (shiftForDiagMouseMove && DiagEventCheck(&input)) {
            DiagGridXY(OrigX, OrigY, &end_x, &end_y);
         }
         if (drawPolyHighlightedNode != NULL) {
            grid_x = OFFSET_X((drawPolyHighlightedNode->obbox.ltx +
                  drawPolyHighlightedNode->obbox.rbx)>>1);
            grid_y = OFFSET_Y((drawPolyHighlightedNode->obbox.lty +
                  drawPolyHighlightedNode->obbox.rby)>>1);
         } else {
            GridXY(end_x, end_y, &grid_x, &grid_y);
         }
         if (grid_x == last_x && grid_y == last_y) {
            if (curSpline == LT_STRUCT_SPLINE) {
               if (num_pts == 1) {
                  abort = TRUE;
               } else {
                  /* need to overwrite the previous point */

                  /* erase */
                  if (sv != NULL) {
                     MyDrawLines(mainDisplay, drawWindow, drawGC, sv, sn);
                     free(sv);
                     sv = NULL;
                  } else {
                     XDrawLine(mainDisplay, drawWindow, drawGC, OrigX, OrigY,
                           grid_x, grid_y);
                  }
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
                     v[0].x = ABS_X(OrigX);
                     v[0].y = ABS_Y(OrigY);
                     v[1].x = ABS_X(ipt_prev.later_smooth_pt.x);
                     v[1].y = ABS_Y(ipt_prev.later_smooth_pt.y);
                     v[2].x = v[3].x = ABS_X(grid_x);
                     v[2].y = v[3].y = ABS_Y(grid_y);
                     n = 3;
                     sv = MakeSplinePolyVertex(0, curSpline, &sn, drawOrigX,
                           drawOrigY, n, v);
                  }
               }
            }
         } else {
            num_pts++;
            AddPointForCont(grid_x, grid_y, &ipt);
            last_x = grid_x;
            last_y = grid_y;
            if (curSpline != LT_STRAIGHT && curSpline != LT_STRUCT_SPLINE) {
               if (n >= max_n-2) {
                  max_n += 40;
                  v = (IntPoint*)realloc(v, sizeof(IntPoint)*max_n+1);
                  if (v == NULL) FailAllocMessage();
               }
               /* erase */
               MyDrawLines(mainDisplay, drawWindow, drawGC, sv, sn);
               if (sv != NULL) {
                  free(sv);
                  sv = NULL;
               }
               v[n].x = v[n+1].x = ABS_X(grid_x);
               v[n].y = v[n+1].y = ABS_Y(grid_y);
               n++;
               switch (curSpline) {
               case LT_SPLINE:
                  sv = MakeSplinePolyVertex(0, curSpline, &sn, drawOrigX,
                        drawOrigY, n, v);
                  break;
               case LT_INTSPLINE:
                  if (cntrlv != NULL) free(cntrlv);
                  sv = MakeIntSplinePolyVertex(&sn, &intn, &cntrlv,
                        drawOrigX, drawOrigY, n, v);
                  for (i=0; i < sn; i++) {
                     if (sv[i].x < ltx) ltx = sv[i].x;
                     if (sv[i].y < lty) lty = sv[i].y;
                     if (sv[i].x > rbx) rbx = sv[i].x;
                     if (sv[i].y > rby) rby = sv[i].y;
                  }
                  break;
               }
               /* draw */
               MyDrawLines(mainDisplay, drawWindow, drawGC, sv, sn);
            } else if (curSpline == LT_STRUCT_SPLINE) {
               /* erase */
               if (sv != NULL) {
                  MyDrawLines(mainDisplay, drawWindow, drawGC, sv, sn);
                  free(sv);
                  sv = NULL;
               } else {
                  XDrawLine(mainDisplay, drawWindow, drawGC, OrigX, OrigY,
                        grid_x, grid_y);
               }
               /* draw */
               if (ipt_prev.later_valid) {
                  v[0].x = ABS_X(OrigX);
                  v[0].y = ABS_Y(OrigY);
                  v[1].x = ABS_X(ipt_prev.later_smooth_pt.x);
                  v[1].y = ABS_Y(ipt_prev.later_smooth_pt.y);
                  v[2].x = v[3].x = ABS_X(grid_x);
                  v[2].y = v[3].y = ABS_Y(grid_y);
                  n = 3;
                  sv = MakeSplinePolyVertex(0, curSpline, &sn, drawOrigX,
                        drawOrigY, n, v);
                  MyDrawLines(mainDisplay, drawWindow, drawGC, sv, sn);
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

         switch (button_ev->button) {
         case Button1:
            if ((drawPolyToConnectPins > 0 &&
                  drawPolyHighlightedNode != NULL) ||
                  (gstWiringInfo.num_ports_to_connect > 0 &&
                  drawPolyHighlightedNode != NULL)) {
               gpEndPin = drawPolyHighlightedNode;
               done = TRUE;
            } else {
               if (curSpline == LT_STRUCT_SPLINE) {
                  ContinueForStructSplinePolyControlPoints(OrigX, OrigY, grid_x,
                        grid_y, &sv, &sn, &ipt_prev, &ipt, &abort);
                  UpdateLastPointForCont(&ipt);
                  /* at this point, the curve has been drawn */
                  if (sv != NULL) {
                     free(sv);
                     sv = NULL;
                  }
                  nothing_is_drawn = TRUE;
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
                     DoPolyMeasureCursor(&pmci, POLY_STARTSHOW, num_pts,
                           ABS_X(grid_x), ABS_Y(grid_y),
                           ABS_SIZE(abs(grid_x-OrigX)),
                           ABS_SIZE(abs(grid_y-OrigY)), POLY_DRAW, POLY_CLICK);
                  }
               } else {
                  OrigX = grid_x;
                  OrigY = grid_y;
                  /* draw */
                  DoPolyMeasureCursor(&pmci, POLY_STARTSHOW, num_pts,
                        ABS_X(grid_x), ABS_Y(grid_y), 0, 0, POLY_DRAW,
                        POLY_CLICK);
                  XDrawLine(mainDisplay, drawWindow, drawGC, OrigX, OrigY,
                        grid_x, grid_y);
               }
            }
            break;
         case Button2:
         case Button3:
#ifdef _NOT_DEFINED
            /*
             * drawPolyToConnectPins is only set to > 0 in "pin.c"
             * what's in "pin.c" is not used at this time
             */
            if (drawPolyToConnectPins > 0) {
               if (drawPolyHighlightedNode == NULL) {
                  abort = TRUE;
               } else {
                  gpEndPin = drawPolyHighlightedNode;
               }
            }
#endif /* _NOT_DEFINED */
            if (curSpline == LT_STRUCT_SPLINE) {
               memset(&ipt, 0, sizeof(IntPointTriplet));
               ipt.earlier_valid = ipt.later_valid = FALSE;
               ipt.hinge_pt.x = ipt.earlier_smooth_pt.x =
                     ipt.later_smooth_pt.x = grid_x;
               ipt.hinge_pt.y = ipt.earlier_smooth_pt.y =
                     ipt.later_smooth_pt.y = grid_y;
               ipt.ratio = (double)1;
               ContinueForStructSplinePolyControlPoints(OrigX, OrigY, grid_x,
                     grid_y, &sv, &sn, &ipt_prev, &ipt, &abort);
               UpdateLastPointForCont(&ipt);
            }
            done = TRUE;
            break;
         }
      } else if (input.type == ButtonRelease && curChoice == FREEHAND) {
         DoPolyMeasureCursor(&pmci, POLY_ENDSHOW, num_pts, ABS_X(grid_x),
               ABS_Y(grid_y), ABS_SIZE(abs(grid_x-OrigX)),
               ABS_SIZE(abs(grid_y-OrigY)), POLY_ERASE, POLY_CLICK);

         button_ev = &(input.xbutton);

         if (grid_x != last_x || grid_y != last_y) {
            num_pts++;
            AddPointForCont(grid_x, grid_y, &ipt);
            last_x = grid_x;
            last_y = grid_y;
         }
         done = TRUE;
      }
   }
   if (drawPolyHighlightedNode != NULL) {
      if (gstWiringInfo.num_ports_to_connect > 0) {
         SelBox(drawWindow, revGrayGC,
               OFFSET_X(drawPolyHighlightedNode->bbox.ltx)-2,
               OFFSET_Y(drawPolyHighlightedNode->bbox.lty)-2,
               OFFSET_X(drawPolyHighlightedNode->bbox.rbx)+2,
               OFFSET_Y(drawPolyHighlightedNode->bbox.rby)+2);
#ifdef _NOT_DEFINED
      /*
       * drawPolyToConnectPins is only set to > 0 in "pin.c"
       * what's in "pin.c" is not used at this time
       */
      } else if (drawPolyToConnectPins > 0) {
         HighLightAPin(FALSE);
#endif /* _NOT_DEFINED */
      }
      drawPolyHighlightedNode = NULL;
   } else if (!abort && gstWiringInfo.num_ports_to_connect > 0) {
      XBell(mainDisplay, 0);
      Msg("");
      Msg(TgLoadString(STID_TRY_AGAIN_AND_END_IN_A_PORT));
      abort = TRUE;
   }
   XUngrabPointer(mainDisplay, CurrentTime);
   if (gstWiringInfo.num_ports_to_connect > 0 || drawPolyToConnectPins > 0) {
      values.line_width = 1;
      XChangeGC(mainDisplay, revGrayGC, GCLineWidth, &values);
      RestoreStatusStringsFromBuf(status_buf, one_line_status);
   }
   RestoreStatusStrings();
   SetMouseStatus(NULL, NULL, NULL);
   if (!abort) Msg("");

   values.join_style = JoinMiter;
   XChangeGC(mainDisplay, drawGC, GCJoinStyle, &values);

   if (v != NULL) free(v);
   if (sv != NULL) free(sv);
   if (cntrlv != NULL) free(cntrlv);
   if (curChoice == FREEHAND && freehand_vs != NULL) {
      free(freehand_vs);
   }
   if (!abort && num_pts > 1) {
      CreatePolyObj(num_pts, CREATE_RELATIVE);
      RecordNewObjCmd();
      RedrawAnArea(botObj, topObj->bbox.ltx-GRID_ABS_SIZE(1),
            topObj->bbox.lty-GRID_ABS_SIZE(1),
            topObj->bbox.rbx+GRID_ABS_SIZE(1),
            topObj->bbox.rby+GRID_ABS_SIZE(1));
      polyDrawn = TRUE;
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
      polyDrawn = FALSE;
   }
   if (gstWiringInfo.num_ports_to_connect == 2 && !polyDrawn) {
      gstWiringInfo.num_ports_to_connect = (-1);
   }
#ifdef _NOT_DEFINED
   /*
    * drawPolyToConnectPins is only set to > 0 in "pin.c"
    * what's in "pin.c" is not used at this time
    */
   if (drawPolyToConnectPins == 2 && !polyDrawn) {
      drawPolyToConnectPins = (-1);
   }
#endif /* _NOT_DEFINED */
}

void DrawPoly(input)
   XEvent *input;
{
   int mouse_x, mouse_y, grid_x, grid_y;
   XButtonEvent *button_ev;

   if (input->type == KeyPress && curChoice == DRAWPOLY &&
         gstWiringInfo.num_ports_to_connect > 0) {
      if (KeyPressEventIsEscape(&input->xkey)) {
         HandlePressForPortInDrawWindow(TRUE);
      }
      return;
   }
   if (input->type != ButtonPress) return;

   button_ev = &(input->xbutton);
   if (button_ev->button == Button1) {
      int saved_cur_spline=curSpline;

      mouse_x = input->xbutton.x;
      mouse_y = input->xbutton.y;
      if (gstWiringInfo.num_ports_to_connect > 0 || drawPolyToConnectPins > 0 ||
            curChoice == FREEHAND) {
         if (drawPolyHighlightedNode != NULL) {
            grid_x = OFFSET_X((drawPolyHighlightedNode->obbox.ltx +
                  drawPolyHighlightedNode->obbox.rbx)>>1);
            grid_y = OFFSET_Y((drawPolyHighlightedNode->obbox.lty +
                  drawPolyHighlightedNode->obbox.rby)>>1);
            if (gstWiringInfo.num_ports_to_connect > 0 ||
                  curChoice == FREEHAND) {
               HandlePressForPortInDrawWindow(FALSE);
#ifdef _NOT_DEFINED
            /*
             * drawPolyToConnectPins is only set to > 0 in "pin.c"
             * what's in "pin.c" is not used at this time
             */
            } else if (drawPolyToConnectPins > 0) {
               HighLightAPin(TRUE);
#endif /* _NOT_DEFINED */
            }
            drawPolyHighlightedNode = NULL;
         } else if (gstWiringInfo.num_ports_to_connect > 0 ||
               drawPolyToConnectPins > 0) {
            XBell(mainDisplay, 0);
            SetStringStatus(TgLoadString(STID_TRY_AGAIN_AND_CLICK_IN_A_PORT));
            return;
         } else {
            grid_x = mouse_x;
            grid_y = mouse_y;
         }
      } else {
         GridXY(mouse_x, mouse_y, &grid_x, &grid_y);
      }
      if (curChoice == FREEHAND) curSpline = LT_STRAIGHT;

      ContinuePoly(grid_x, grid_y);

      if (curChoice == FREEHAND) curSpline = saved_cur_spline;
   } 
}

void SetWiringNodeInfo(port_obj, port_owner_obj, port_name, first)
   struct ObjRec *port_obj, *port_owner_obj;
   char *port_name;
   int first;
{
   if (first) {
      gstWiringInfo.first_port_obj = port_obj;
      gstWiringInfo.first_port_owner_obj = port_owner_obj;
      if (port_name == NULL) {
         *gstWiringInfo.first_port_name = '\0';
      } else {
         UtilStrCpyN(gstWiringInfo.first_port_name,
               sizeof(gstWiringInfo.first_port_name), port_name);
      }
   } else {
      gstWiringInfo.last_port_obj = port_obj;
      gstWiringInfo.last_port_owner_obj = port_owner_obj;
      if (port_name == NULL) {
         *gstWiringInfo.last_port_name = '\0';
      } else {
         UtilStrCpyN(gstWiringInfo.last_port_name,
               sizeof(gstWiringInfo.last_port_name), port_name);
      }
   }
}

void ResetWiringNodeInfo()
{
   memset(&gstWiringInfo, 0, sizeof(WiringInfo));
}

void InputPolyPts()
{
   char inbuf[MAXSTRING+1];
   int more_poly=FALSE, num_polys=0;
   int started_composite=FALSE;
   struct ObjRec *saved_top_obj=topObj;

   MakeQuiescent();
   XSync(mainDisplay, False);
   do {
      int len, ok=TRUE, num_pts=0, eof=TRUE;
      struct PtRec *pt_ptr;

      more_poly = FALSE;
      numPtsInPoly = 0;
      lastPtPtr = NULL;
      printf("%s\n", TgLoadString(STID_INPUT_PAIRS_OF_POINTS_POLY));
      printf("> ");
      fflush(stdout);
      while (ok && fgets(inbuf, MAXSTRING, stdin) != NULL) {
         if (strcmp(inbuf, ";\n") == 0) {
            more_poly = TRUE;
            eof = FALSE;
            break;
         }
         if (strcmp(inbuf, ".\n") == 0) {
            eof = FALSE;
            break;
         }
         len = strlen(inbuf);
         if (len > 0) {
            char *c_ptr=strtok(inbuf," ,\t\n"), *c_ptr1=NULL;
   
            if (c_ptr != NULL) {
               c_ptr1 = strtok(NULL," ,\t\n");
            }
            if (c_ptr1 != NULL) {
               while (strchr(" ,\t\n", *c_ptr1)) c_ptr1++;
            }
            while (c_ptr != NULL && c_ptr1 != NULL) {
               num_pts++;
               pt_ptr = (struct PtRec *)malloc(sizeof(struct PtRec));
               if (pt_ptr == NULL) FailAllocMessage();
               pt_ptr->next = lastPtPtr;
               if (sscanf(c_ptr, "%d", &pt_ptr->x) != 1 ||
                     sscanf(c_ptr1, "%d", &pt_ptr->y) != 1) {
                  ok = FALSE;
                  MsgBox(TgLoadString(STID_READ_INT_ERROR_FOR_POLY_PTS),
                        TOOL_NAME, INFO_MB);
                  XSync(mainDisplay, False);
                  break;
               }
               lastPtPtr = pt_ptr;
               c_ptr = strtok(NULL," ,\t\n");
               if (c_ptr != NULL) {
                  c_ptr1 = strtok(NULL," ,\t\n");
               }
               if (c_ptr1 != NULL) {
                  while (strchr(" ,\t\n", *c_ptr1)) c_ptr1++;
               }
            }
            if (c_ptr != NULL) {
               ok = FALSE;
               MsgBox(TgLoadString(STID_READ_INT_ERROR_FOR_POLY_PTS), TOOL_NAME,
                     INFO_MB);
               XSync(mainDisplay, False);
            }
         }
         printf("> ");
         fflush(stdout);
      }
      printf("\n");
      if (eof) rewind(stdin);
      if (ok && num_pts > 1) {
         num_polys++;
         CreatePolyObj(num_pts, CREATE_ABSOLUTE);
         if (more_poly || num_polys > 1) {
            if (num_polys <= 1) {
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
      if (ok && num_pts <= 1) {
         MsgBox(TgLoadString(STID_TOO_FEW_POINTERS_ENTERED), TOOL_NAME,
               INFO_MB);
         XSync(mainDisplay, False);
      }
      for ( ; lastPtPtr != NULL; lastPtPtr=pt_ptr) {
         pt_ptr = lastPtPtr->next;
         free(pt_ptr);
      }
   } while (more_poly);
   if (num_polys > 1 || started_composite) {
      SelectAndHighLightNewObjects(saved_top_obj);
      GroupSelObj(TRUE, TRUE, TRUE);
      EndCompositeCmd();

      SetFileModified(TRUE);
      justDupped = FALSE;
   }
}

/* --------------------- JoinPoly() --------------------- */

static
void FinishJoinPoly(obj_ptr1, obj_ptr2, poly_ptr1, vs, smooth, num_pts)
   struct ObjRec *obj_ptr1, *obj_ptr2;
   struct PolyRec *poly_ptr1;
   IntPoint *vs;
   char *smooth;
   int num_pts;
{
   struct SelRec *sel_ptr, *top_sel=NULL, *bot_sel=NULL;
   int x0=0, y0=0, index0=0;

   if (curChoice == VERTEXMODE) {
      if (topVSel->obj == obj_ptr1) {
         index0 = topVSel->v_index[0];
         x0 = topVSel->x[0];
         y0 = topVSel->y[0];
      } else {
         index0 = botVSel->v_index[0];
         x0 = botVSel->x[0];
         y0 = botVSel->y[0];
      }
   }
   RemoveAllSel();
   if (obj_ptr2->fattr != NULL) {
      int count=0;
      struct SelRec *next_sel;

      PrepareToReplaceAnObj(obj_ptr2);
      DetachAllObjAttrs(obj_ptr2, &top_sel, &bot_sel);
      /* obj_ptr2 is pointed to by bot_sel now */
      AdjObjBBox(obj_ptr2);
      for (sel_ptr=top_sel; sel_ptr != NULL; sel_ptr=sel_ptr->next) count++;
      RecordCmd(CMD_ONE_TO_MANY, NULL, top_sel, bot_sel, count);
      sel_ptr = bot_sel;
      bot_sel = bot_sel->prev;
      bot_sel->next = NULL;
      free(sel_ptr);
      if (curChoice == VERTEXMODE) {
         for (sel_ptr=top_sel; sel_ptr != NULL; sel_ptr=next_sel) {
            next_sel = sel_ptr->next;
            free(sel_ptr);
         }
         top_sel = bot_sel = NULL;
      }
   }
   sel_ptr = (struct SelRec *)malloc(sizeof(struct SelRec));
   if (sel_ptr == NULL) FailAllocMessage();
   sel_ptr->next = sel_ptr->prev = NULL;
   sel_ptr->obj = obj_ptr2;
   PrepareToRecord(CMD_DELETE, sel_ptr, sel_ptr, 1);
   UnlinkObj(obj_ptr2);
   FreeObj(obj_ptr2);
   RecordCmd(CMD_DELETE, NULL, NULL, NULL, 0);
   free(sel_ptr);

   PrepareToReplaceAnObj(obj_ptr1);

   if (poly_ptr1->vlist != NULL) free(poly_ptr1->vlist);
   if (poly_ptr1->svlist != NULL) free(poly_ptr1->svlist);
   if (poly_ptr1->asvlist != NULL) free(poly_ptr1->asvlist);
   if (poly_ptr1->smooth != NULL) free(poly_ptr1->smooth);
   if (poly_ptr1->intvlist != NULL) free(poly_ptr1->intvlist);
   if (poly_ptr1->rotated_vlist != NULL) free(poly_ptr1->rotated_vlist);
   if (poly_ptr1->rotated_asvlist != NULL) free(poly_ptr1->rotated_asvlist);
   poly_ptr1->smooth = smooth;
   poly_ptr1->vlist = poly_ptr1->intvlist = NULL;
   poly_ptr1->n = poly_ptr1->intn = 0;
   poly_ptr1->svlist = poly_ptr1->asvlist = poly_ptr1->rotated_vlist =
         poly_ptr1->rotated_asvlist = NULL;
   poly_ptr1->sn = poly_ptr1->asn = poly_ptr1->rotated_n =
         poly_ptr1->rotated_asn = 0;
   if (obj_ptr1->ctm != NULL) free(obj_ptr1->ctm);
   obj_ptr1->ctm = NULL;

   poly_ptr1->vlist = vs;
   poly_ptr1->n = num_pts;

   AdjObjSplineVs(obj_ptr1);
   if (poly_ptr1->curved != LT_INTSPLINE) {
      UpdPolyBBox(obj_ptr1, poly_ptr1->n, poly_ptr1->vlist);
   } else {
      UpdPolyBBox(obj_ptr1, poly_ptr1->intn, poly_ptr1->intvlist);
   }
   AdjObjBBox(obj_ptr1);
   RecordReplaceAnObj(obj_ptr1);

   topSel = (struct SelRec *)malloc(sizeof(struct SelRec));
   if (topSel == NULL) FailAllocMessage();
   topSel->obj = obj_ptr1;
   topSel->prev = NULL;
   topSel->next = top_sel;
   if (top_sel != NULL) {
      top_sel->prev = topSel;
      botSel = bot_sel;
   } else {
      botSel = topSel;
   }
   if (curChoice == VERTEXMODE) {
      topVSel = botVSel = (struct VSelRec *)malloc(sizeof(struct VSelRec));
      if (topVSel == NULL) FailAllocMessage();
      memset(topVSel, 0, sizeof(struct VSelRec));
      topVSel->obj = obj_ptr1;
      topVSel->max_v = 10;
      topVSel->v_index = (int*)malloc(10*sizeof(int));
      topVSel->x = (int*)malloc(10*sizeof(int));
      topVSel->y = (int*)malloc(10*sizeof(int));
      if (topVSel->v_index==NULL || topVSel->x==NULL || topVSel->y==NULL) {
         FailAllocMessage();
      }
      topVSel->v_index[0] = index0;
      topVSel->x[0] = x0;
      topVSel->y[0] = y0;
      topVSel->n = 1;
      topVSel->next = topVSel->prev = NULL;
   }
}

typedef struct tagJoinStructSplineInfo {
   StretchStructuredSplineInfo sssi1, sssi2;
} JoinStructSplineInfo;

static
void DoJoinPoly(obj_ptr1, obj_ptr2, poly_ptr1, poly_ptr2, min_index, coincide,
      pjssi)
   struct ObjRec *obj_ptr1, *obj_ptr2;
   struct PolyRec *poly_ptr1, *poly_ptr2;
   int min_index, coincide;
   JoinStructSplineInfo *pjssi;
   /*
    * min_index==0: join point 0   of poly_ptr1 and point 0   of poly_ptr2
    * min_index==1: join point 0   of poly_ptr1 and point n-1 of poly_ptr2
    * min_index==2: join point n-1 of poly_ptr1 and point 0   of poly_ptr2
    * min_index==3: join point n-1 of poly_ptr1 and point n-1 of poly_ptr2
    */
{
   int i, n, n1=poly_ptr1->n, n2=poly_ptr2->n, num_pts, ltx, lty, rbx, rby;
   int min_val=0, max_val=0, inc=0, curved=poly_ptr1->curved;
   IntPoint *vlist1=poly_ptr1->vlist, *vlist2=poly_ptr2->vlist;
   IntPoint *new_vs=NULL, tmp_p;
   char *new_smooth=NULL;

   if (pjssi != NULL) {
      if (coincide) {
         num_pts = n1+n2-1;
      } else {
         num_pts = n1+n2+2;
      }
   } else {
      num_pts = n1+n2;
      if (coincide) {
         num_pts = n1+n2-1;
      } else {
         num_pts = n1+n2;
      }
   }
   new_vs = (IntPoint*)malloc((num_pts+1)*sizeof(IntPoint));
   if (new_vs == NULL) FailAllocMessage();
   if (curved == LT_STRUCT_SPLINE) {
      IntPoint tmp_p1, tmp_p2;
      int fix_v1_of_vlist2=FALSE, fix_vn2_minus_2_of_vlist2=FALSE;

      switch (min_index) {
      case 0: min_val=n1-1; max_val=0; inc=(-1); break;
      case 1: min_val=n1-1; max_val=0; inc=(-1); break;
      case 2: min_val=0; max_val=n1; inc=1; break;
      case 3: min_val=0; max_val=n1; inc=1; break;
      }
      n = 0;
      i = min_val;
      while (inc > 0 ? i < max_val : i >= max_val) {
         if (obj_ptr1->ctm == NULL) {
            memcpy(&new_vs[n], &vlist1[i], sizeof(IntPoint));
         } else {
            TransformObjectV(obj_ptr1, &vlist1[i], &tmp_p);
            memcpy(&new_vs[n], &tmp_p, sizeof(IntPoint));
         }
         n++;
         i += inc;
      }
      if (coincide) {
         int has_smooth_1=FALSE, has_smooth_2=FALSE;

         switch (min_index) {
         case 0: /* join pt 0 of poly1 and pt 0 of poly2 */
            has_smooth_1 = pjssi->sssi1.ipt.later_valid;
            has_smooth_2 = pjssi->sssi2.ipt.later_valid;
            break;
         case 1: /* join pt 0 of poly1 and pt n-1 of poly2 */
            has_smooth_1 = pjssi->sssi1.ipt.later_valid;
            has_smooth_2 = pjssi->sssi2.ipt.earlier_valid;
            break;
         case 2: /* join pt n-1 of poly1 and pt 0 of poly2 */
            has_smooth_1 = pjssi->sssi1.ipt.earlier_valid;
            has_smooth_2 = pjssi->sssi2.ipt.later_valid;
            break;
         case 3: /* join pt n-1 of poly1 and pt n-1 of poly2 */
            has_smooth_1 = pjssi->sssi1.ipt.earlier_valid;
            has_smooth_2 = pjssi->sssi2.ipt.earlier_valid;
            break;
         }
         if (has_smooth_1) {
            /* first point has a corresponding smooth point */
            if (has_smooth_2) {
               /* last point has a corresponding smooth point */
               switch (min_index) {
               case 0: /* join pt 0 of poly1 and pt 0 of poly2 */
               case 2: /* join pt n-1 of poly1 and pt 0 of poly2 */
                  if (obj_ptr2->ctm == NULL) {
                     new_vs[n-2].x = (vlist2[0].x<<1) - vlist2[1].x;
                     new_vs[n-2].y = (vlist2[0].y<<1) - vlist2[1].y;
                  } else {
                     TransformObjectV(obj_ptr2, &vlist2[0], &tmp_p1);
                     TransformObjectV(obj_ptr2, &vlist2[1], &tmp_p2);
                     new_vs[n-2].x = (tmp_p1.x<<1) - tmp_p2.x;
                     new_vs[n-2].y = (tmp_p1.y<<1) - tmp_p2.y;
                  }
                  break;
               case 1: /* join pt 0 of poly1 and pt n-1 of poly2 */
               case 3: /* join pt n-1 of poly1 and pt n-1 of poly2 */
                  if (obj_ptr2->ctm == NULL) {
                     new_vs[n-2].x = (vlist2[n2-1].x<<1) - vlist2[n2-2].x;
                     new_vs[n-2].y = (vlist2[n2-1].y<<1) - vlist2[n2-2].y;
                  } else {
                     TransformObjectV(obj_ptr2, &vlist2[n2-1], &tmp_p1);
                     TransformObjectV(obj_ptr2, &vlist2[n2-2], &tmp_p2);
                     new_vs[n-2].x = (tmp_p1.x<<1) - tmp_p2.x;
                     new_vs[n-2].y = (tmp_p1.y<<1) - tmp_p2.y;
                  }
                  break;
               }
            } else {
               /* last point does not have a corresponding smooth point */
               new_vs[n-2].x = new_vs[n-1].x;
               new_vs[n-2].y = new_vs[n-1].y;
            }
         } else {
            /* first point does not have a corresponding smooth point */
            if (has_smooth_2) {
               /* last point has a corresponding smooth point */
               switch (min_index) {
               case 0: /* join pt 0 of poly1 and pt 0 of poly2 */
               case 2: /* join pt n-1 of poly1 and pt 0 of poly2 */
                  /*
                   * Cannot really do the following here because it will
                   *       mess up undo.  So, delay the fixing till the end.
                   *
                   * vlist2[1].x = vlist2[0].x;
                   * vlist2[1].y = vlist2[0].y;
                   */
                  fix_v1_of_vlist2 = TRUE;
                  break;
               case 1: /* join pt 0 of poly1 and pt n-1 of poly2 */
               case 3: /* join pt n-1 of poly1 and pt n-1 of poly2 */
                  /*
                   * Cannot really do the following here because it will
                   *       mess up undo.  So, delay the fixing till the end.
                   *
                   * vlist2[n2-2].x = vlist2[n2-1].x;
                   * vlist2[n2-2].y = vlist2[n2-1].y;
                   */
                  fix_vn2_minus_2_of_vlist2 = TRUE;
                  break;
               }
            } else {
               /* last point does not have a corresponding smooth point */
            }
         }
      } else {
         switch (min_index) {
         case 0: /* join point 0 of poly_ptr1 and point 0 of poly_ptr2 */
         case 1: /* join point 0 of poly_ptr1 and point n-1 of poly_ptr2 */
            if (pjssi->sssi1.ipt.later_valid) {
               /* first point has a corresponding smooth point */
               new_vs[n].x = (new_vs[n-1].x<<1) - new_vs[n-2].x;
               new_vs[n].y = (new_vs[n-1].y<<1) - new_vs[n-2].y;
            } else {
               /* first point does not have a corresponding smooth point */
               new_vs[n].x = new_vs[n-1].x;
               new_vs[n].y = new_vs[n-1].y;
            }
            n++;
            break;
         case 2: /* join point n-1 of poly_ptr1 and point 0 of poly_ptr2 */
         case 3: /* join point n-1 of poly_ptr1 and point n-1 of poly_ptr2 */
            if (pjssi->sssi1.ipt.earlier_valid) {
               /* first point has a corresponding smooth point */
               new_vs[n].x = (new_vs[n-1].x<<1) - new_vs[n-2].x;
               new_vs[n].y = (new_vs[n-1].y<<1) - new_vs[n-2].y;
            } else {
               /* first point does not have a corresponding smooth point */
               new_vs[n].x = new_vs[n-1].x;
               new_vs[n].y = new_vs[n-1].y;
            }
            n++;
            break;
         }
         switch (min_index) {
         case 0: /* join point 0 of poly_ptr1 and point 0 of poly_ptr2 */
         case 2: /* join point n-1 of poly_ptr1 and point 0 of poly_ptr2 */
            if (pjssi->sssi2.ipt.later_valid) {
               /* last point has a corresponding smooth point */
               if (obj_ptr2->ctm == NULL) {
                  new_vs[n].x = (vlist2[0].x<<1) - vlist2[1].x;
                  new_vs[n].y = (vlist2[0].y<<1) - vlist2[1].y;
               } else {
                  TransformObjectV(obj_ptr2, &vlist2[0], &tmp_p1);
                  TransformObjectV(obj_ptr2, &vlist2[1], &tmp_p2);
                  new_vs[n].x = (tmp_p1.x<<1) - tmp_p2.x;
                  new_vs[n].y = (tmp_p1.y<<1) - tmp_p2.y;
               }
            } else {
               /* last point does not have a corresponding smooth point */
               if (obj_ptr2->ctm == NULL) {
                  new_vs[n].x = vlist2[0].x;
                  new_vs[n].y = vlist2[0].y;
               } else {
                  TransformObjectV(obj_ptr2, &vlist2[0], &tmp_p);
                  new_vs[n].x = tmp_p.x;
                  new_vs[n].y = tmp_p.y;
               }
            }
            n++;
            break;
         case 1: /* join point 0 of poly_ptr1 and point n-1 of poly_ptr2 */
         case 3: /* join point n-1 of poly_ptr1 and point n-1 of poly_ptr2 */
            if (pjssi->sssi2.ipt.earlier_valid) {
               /* last point has a corresponding smooth point */
               if (obj_ptr2->ctm == NULL) {
                  new_vs[n].x = (vlist2[n2-1].x<<1) - vlist2[n2-2].x;
                  new_vs[n].y = (vlist2[n2-1].y<<1) - vlist2[n2-2].y;
               } else {
                  TransformObjectV(obj_ptr2, &vlist2[n2-1], &tmp_p1);
                  TransformObjectV(obj_ptr2, &vlist2[n2-2], &tmp_p2);
                  new_vs[n].x = (tmp_p1.x<<1) - tmp_p2.x;
                  new_vs[n].y = (tmp_p1.y<<1) - tmp_p2.y;
               }
            } else {
               /* last point does not have a corresponding smooth point */
               if (obj_ptr2->ctm == NULL) {
                  new_vs[n].x = vlist2[n2-1].x;
                  new_vs[n].y = vlist2[n2-1].y;
               } else {
                  TransformObjectV(obj_ptr2, &vlist2[n2-1], &tmp_p);
                  new_vs[n].x = tmp_p.x;
                  new_vs[n].y = tmp_p.y;
               }
            }
            n++;
            break;
         }
      }
      if (coincide) {
         switch (min_index) {
         case 0: min_val=1; max_val=n2; inc=1; break;
         case 1: min_val=n2-2; max_val=0; inc=(-1); break;
         case 2: min_val=1; max_val=n2; inc=1; break;
         case 3: min_val=n2-2; max_val=0; inc=(-1); break;
         }
      } else {
         switch (min_index) {
         case 0: min_val=0; max_val=n2; inc=1; break;
         case 1: min_val=n2-1; max_val=0; inc=(-1); break;
         case 2: min_val=0; max_val=n2; inc=1; break;
         case 3: min_val=n2-1; max_val=0; inc=(-1); break;
         }
      }
      i = min_val;
      while (inc > 0 ? i < max_val : i >= max_val) {
         if (obj_ptr2->ctm == NULL) {
            memcpy(&new_vs[n], &vlist2[i], sizeof(IntPoint));
         } else {
            TransformObjectV(obj_ptr2, &vlist2[i], &tmp_p);
            memcpy(&new_vs[n], &tmp_p, sizeof(IntPoint));
         }
         n++;
         i += inc;
      }
      if (fix_v1_of_vlist2 || fix_vn2_minus_2_of_vlist2) {
         new_vs[n1].x = new_vs[n1-1].x;
         new_vs[n1].y = new_vs[n1-1].y;
      }
   } else {
      if (curved != LT_INTSPLINE) {
         new_smooth = (char*)malloc((num_pts+1)*sizeof(char));
         if (new_smooth == NULL) FailAllocMessage();
      }
      switch (min_index) {
      case 0: min_val=n1-1; max_val=0; inc=(-1); break;
      case 1: min_val=n1-1; max_val=0; inc=(-1); break;
      case 2: min_val=0; max_val=n1; inc=1; break;
      case 3: min_val=0; max_val=n1; inc=1; break;
      }
      n = 0;
      i = min_val;
      while (inc > 0 ? i < max_val : i >= max_val) {
         if (obj_ptr1->ctm == NULL) {
            memcpy(&new_vs[n], &vlist1[i], sizeof(IntPoint));
         } else {
            TransformObjectV(obj_ptr1, &vlist1[i], &tmp_p);
            memcpy(&new_vs[n], &tmp_p, sizeof(IntPoint));
         }
         if (new_smooth != NULL) {
            new_smooth[n] = poly_ptr1->smooth[i];
         }
         n++;
         i += inc;
      }
      if (coincide) {
         switch (min_index) {
         case 0: min_val=1; max_val=n2; inc=1; break;
         case 1: min_val=n2-2; max_val=0; inc=(-1); break;
         case 2: min_val=1; max_val=n2; inc=1; break;
         case 3: min_val=n2-2; max_val=0; inc=(-1); break;
         }
      } else {
         switch (min_index) {
         case 0: min_val=0; max_val=n2; inc=1; break;
         case 1: min_val=n2-1; max_val=0; inc=(-1); break;
         case 2: min_val=0; max_val=n2; inc=1; break;
         case 3: min_val=n2-1; max_val=0; inc=(-1); break;
         }
      }
      i = min_val;
      while (inc > 0 ? i < max_val : i >= max_val) {
         if (obj_ptr2->ctm == NULL) {
            memcpy(&new_vs[n], &vlist2[i], sizeof(IntPoint));
         } else {
            TransformObjectV(obj_ptr2, &vlist2[i], &tmp_p);
            memcpy(&new_vs[n], &tmp_p, sizeof(IntPoint));
         }
         if (new_smooth != NULL) {
            new_smooth[n] = poly_ptr2->smooth[i];
         }
         n++;
         i += inc;
      }
   }
   ltx = selLtX; lty = selLtY; rbx = selRbX; rby = selRbY;
   HighLightReverse();
   StartCompositeCmd();
   FinishJoinPoly(obj_ptr1, obj_ptr2, poly_ptr1, new_vs, new_smooth, num_pts);
   EndCompositeCmd();
   UpdSelBBox();
   RedrawAreas(botObj, ltx-GRID_ABS_SIZE(1), lty-GRID_ABS_SIZE(1),
         rbx+GRID_ABS_SIZE(1), rby+GRID_ABS_SIZE(1),
         selLtX-GRID_ABS_SIZE(1), selLtY-GRID_ABS_SIZE(1),
         selRbX+GRID_ABS_SIZE(1), selRbY+GRID_ABS_SIZE(1));
   SetFileModified(TRUE);
   justDupped = FALSE;
   HighLightForward();
}

static
void FinishCloseOnePoly(obj_ptr, poly_ptr, vs, smooth, num_pts)
   struct ObjRec *obj_ptr;
   struct PolyRec *poly_ptr;
   IntPoint *vs;
   char *smooth;
   int num_pts;
{
   struct PolygonRec *polygon_ptr=(struct PolygonRec *)malloc(
         sizeof(struct PolygonRec));

   if (polygon_ptr == NULL) FailAllocMessage();

   memset(polygon_ptr, 0, sizeof(struct PolygonRec));
   polygon_ptr->n = num_pts;
   polygon_ptr->vlist = vs;
   polygon_ptr->smooth = smooth;
   polygon_ptr->sn = 0;
   polygon_ptr->svlist = NULL;
   polygon_ptr->intn = 0;
   polygon_ptr->intvlist = NULL;
   polygon_ptr->fill = poly_ptr->fill;
   polygon_ptr->width = poly_ptr->width;
   UtilStrCpyN(polygon_ptr->width_spec, sizeof(polygon_ptr->width_spec),
         poly_ptr->width_spec);
   polygon_ptr->pen = poly_ptr->pen;
   polygon_ptr->curved = poly_ptr->curved;
   polygon_ptr->dash = poly_ptr->dash;
   polygon_ptr->rotated_n = 0;
   polygon_ptr->rotated_vlist = NULL;

   PrepareToReplaceAnObj(obj_ptr);

   if (poly_ptr->vlist != NULL) free(poly_ptr->vlist);
   if (poly_ptr->svlist != NULL) free(poly_ptr->svlist);
   if (poly_ptr->asvlist != NULL) free(poly_ptr->asvlist);
   if (poly_ptr->smooth != NULL) free(poly_ptr->smooth);
   if (poly_ptr->intvlist != NULL) free(poly_ptr->intvlist);
   if (poly_ptr->rotated_vlist != NULL) free(poly_ptr->rotated_vlist);
   if (poly_ptr->rotated_asvlist != NULL) free(poly_ptr->rotated_asvlist);
   free(poly_ptr);

   obj_ptr->type = OBJ_POLYGON;
   obj_ptr->detail.g = polygon_ptr;

   AdjObjSplineVs(obj_ptr);
   if (polygon_ptr->curved != LT_INTSPLINE) {
      UpdPolyBBox(obj_ptr, polygon_ptr->n, polygon_ptr->vlist);
   } else {
      UpdPolyBBox(obj_ptr, polygon_ptr->intn, polygon_ptr->intvlist);
   }
   AdjObjBBox(obj_ptr);
   RecordReplaceAnObj(obj_ptr);
}

static
void CloseOnePoly(obj_ptr)
   struct ObjRec *obj_ptr;
{
   struct PolyRec *poly_ptr=obj_ptr->detail.p;
   int i, n=0, num_pts=0, ltx, lty, rbx, rby, coincide=FALSE;
   int curved=poly_ptr->curved;
   IntPoint *new_vs=NULL, *vs=NULL;
   char *new_smooth=NULL, *smooth=NULL;
   StretchStructuredSplineInfo sssi_first, sssi_last;

   memset(&sssi_first, 0, sizeof(StretchStructuredSplineInfo));
   memset(&sssi_last, 0, sizeof(StretchStructuredSplineInfo));

   if (curved == LT_STRUCT_SPLINE) {
      vs = poly_ptr->ssvlist;
      n = poly_ptr->ssn;
      smooth = poly_ptr->ssmooth;
      if (n < 3) {
         MsgBox(TgLoadString(STID_TOO_FEW_VER_TO_CLOSE_POLY), TOOL_NAME,
               INFO_MB);
         return;
      }
   } else {
      vs = poly_ptr->vlist;
      n = poly_ptr->n;
      smooth = poly_ptr->smooth;
      if (n <= 2) {
         MsgBox(TgLoadString(STID_TOO_FEW_VER_TO_CLOSE_POLY), TOOL_NAME,
               INFO_MB);
         return;
      }
   }
   if (vs[0].x == vs[n-1].x && vs[0].y == vs[n-1].y) {
      coincide = TRUE;
      if (curved == LT_STRUCT_SPLINE) {
         if (n <= 4) {
            MsgBox(TgLoadString(STID_TOO_FEW_VER_TO_CLOSE_POLY), TOOL_NAME,
                  INFO_MB);
            return;
         }
         SetIPTInfoForStretchPoly(0, poly_ptr->n, poly_ptr->vlist, &sssi_first);
         SetIPTInfoForStretchPoly(n-1, poly_ptr->n, poly_ptr->vlist,
               &sssi_last);
         num_pts = poly_ptr->n;
      } else {
         num_pts = n;
      }
   } else {
      if (curved == LT_STRUCT_SPLINE) {
         SetIPTInfoForStretchPoly(0, poly_ptr->n, poly_ptr->vlist, &sssi_first);
         SetIPTInfoForStretchPoly(n-1, poly_ptr->n, poly_ptr->vlist,
               &sssi_last);
         num_pts = poly_ptr->n+3;
      } else {
         num_pts = n+1;
      }
   }
   new_vs = (IntPoint*)malloc((num_pts+1)*sizeof(IntPoint));
   if (new_vs == NULL) FailAllocMessage();
   memset(new_vs, 0,  (num_pts+1)*sizeof(IntPoint));

   if (curved == LT_STRUCT_SPLINE) {
      for (i=0; i < poly_ptr->n; i++) {
         memcpy(&new_vs[i], &poly_ptr->vlist[i], sizeof(IntPoint));
      }
      if (coincide) {
         if (sssi_first.ipt.later_valid) {
            /* first point has a corresponding smooth point */
            if (sssi_last.ipt.earlier_valid) {
               /* last point has a corresponding smooth point */
               /*
                * in this case, move the last smooth point
                */
               new_vs[poly_ptr->n-2].x = (new_vs[0].x<<1) - new_vs[1].x;
               new_vs[poly_ptr->n-2].y = (new_vs[0].y<<1) - new_vs[1].y;
            } else {
               /* last point does not have a corresponding smooth point */
               /*
                * in this case, remove the first smooth point
                */
               new_vs[1].x = new_vs[0].x;
               new_vs[1].y = new_vs[0].y;
            }
         } else {
            /* first point does not have a corresponding smooth point */
            if (sssi_last.ipt.earlier_valid) {
               /* last point has a corresponding smooth point */
               /*
                * in this case, remove the last smooth point
                */
               new_vs[poly_ptr->n-2].x = new_vs[poly_ptr->n-1].x;
               new_vs[poly_ptr->n-2].y = new_vs[poly_ptr->n-1].y;
            } else {
               /* last point does not have a corresponding smooth point */
               /*
                * in this case, there is nothing to do
                */
            }
         }
      } else {
         new_vs[poly_ptr->n+2].x = new_vs[0].x;
         new_vs[poly_ptr->n+2].y = new_vs[0].y;

         if (sssi_first.ipt.later_valid) {
            /* first point has a corresponding smooth point */
            if (sssi_last.ipt.earlier_valid) {
               /* last point has a corresponding smooth point */
               /*
                * in this case, create 2 new smooth points
                */
               new_vs[poly_ptr->n+1].x = (new_vs[0].x<<1) - new_vs[1].x;
               new_vs[poly_ptr->n+1].y = (new_vs[0].y<<1) - new_vs[1].y;
               new_vs[poly_ptr->n].x = (new_vs[poly_ptr->n-1].x<<1) -
                     new_vs[poly_ptr->n-2].x;
               new_vs[poly_ptr->n].y = (new_vs[poly_ptr->n-1].y<<1) -
                     new_vs[poly_ptr->n-2].y;
            } else {
               /* last point does not have a corresponding smooth point */
               /*
                * in this case, create one new smooth point near the 1st point
                */
               new_vs[poly_ptr->n+1].x = (new_vs[0].x<<1) - new_vs[1].x;
               new_vs[poly_ptr->n+1].y = (new_vs[0].y<<1) - new_vs[1].y;
               new_vs[poly_ptr->n].x = new_vs[poly_ptr->n-1].x;
               new_vs[poly_ptr->n].y = new_vs[poly_ptr->n-1].y;
            }
         } else {
            /* first point does not have a corresponding smooth point */
            if (sssi_last.ipt.earlier_valid) {
               /* last point has a corresponding smooth point */
               /*
                * in this case, create one new smooth point near the last point
                */
               new_vs[poly_ptr->n+1].x = new_vs[0].x;
               new_vs[poly_ptr->n+1].y = new_vs[0].y;
               new_vs[poly_ptr->n].x = (new_vs[poly_ptr->n-1].x<<1) -
                     new_vs[poly_ptr->n-2].x;
               new_vs[poly_ptr->n].y = (new_vs[poly_ptr->n-1].y<<1) -
                     new_vs[poly_ptr->n-2].y;
            } else {
               /* last point does not have a corresponding smooth point */
               /*
                * in this case, create 2 hinge points
                */
               new_vs[poly_ptr->n+1].x = new_vs[0].x;
               new_vs[poly_ptr->n+1].y = new_vs[0].y;
               new_vs[poly_ptr->n].x = new_vs[poly_ptr->n-1].x;
               new_vs[poly_ptr->n].y = new_vs[poly_ptr->n-1].y;
            }
         }
      }
   } else {
      for (i=0; i < n; i++) memcpy(&new_vs[i], &vs[i], sizeof(IntPoint));
      memcpy(&new_vs[n], &vs[0], sizeof(IntPoint));
      if (poly_ptr->curved != LT_INTSPLINE) {
         new_smooth = (char*)malloc((num_pts+1)*sizeof(char));
         if (new_smooth == NULL) FailAllocMessage();
         for (i=0; i < n; i++) new_smooth[i] = smooth[i];
         new_smooth[0] = new_smooth[n] = FALSE;
      }
   }
   ltx = selLtX; lty = selLtY; rbx = selRbX; rby = selRbY;
   HighLightReverse();
   FinishCloseOnePoly(obj_ptr, poly_ptr, new_vs, new_smooth, num_pts);
   if (curChoice == VERTEXMODE) {
      if (topVSel == botVSel) {
         int x0=0, y0=0, first_index=(-1), last_index=(-1);

         for (i=0; i < topVSel->n; i++) {
            if (topVSel->v_index[i] == 0) {
               x0 = topVSel->x[i];
               y0 = topVSel->y[i];
               first_index = i;
            } else if (topVSel->v_index[i] ==
                  (coincide ? num_pts-1 : num_pts-2)) {
               if (!coincide) {
                  topVSel->v_index[i]++;
               }
               last_index = i;
            }
         }
         if (last_index != (-1) && first_index != (-1)) {
            topVSel->n = 2;
            topVSel->x[first_index] = topVSel->x[last_index] = x0;
            topVSel->y[first_index] = topVSel->y[last_index] = y0;
         } else {
            fprintf(stderr, "%s\n",
                  TgLoadString(STID_HUH_WHERE_ARE_THE_VERTICES));
         }
      } else {
         fprintf(stderr, "%s\n", TgLoadString(STID_HUH_TOPVSEL_NE_BOTVSEL));
      }
   }
   UpdSelBBox();
   RedrawAreas(botObj, ltx-GRID_ABS_SIZE(1), lty-GRID_ABS_SIZE(1),
         rbx+GRID_ABS_SIZE(1), rby+GRID_ABS_SIZE(1),
         selLtX-GRID_ABS_SIZE(1), selLtY-GRID_ABS_SIZE(1),
         selRbX+GRID_ABS_SIZE(1), selRbY+GRID_ABS_SIZE(1));
   SetFileModified(TRUE);
   justDupped = FALSE;
   HighLightForward();
}

static
int JoinPolyCompatCheck(poly_ptr1, poly_ptr2)
   struct PolyRec *poly_ptr1, *poly_ptr2;
{
   int rc=TRUE;

   switch (poly_ptr1->curved) {
   case LT_STRAIGHT:
   case LT_SPLINE:
      if (poly_ptr2->curved == LT_STRAIGHT ||
            poly_ptr2->curved == LT_SPLINE) {
         /* compatible */
      } else {
         rc = FALSE;
      }
      break;
   case LT_INTSPLINE:
      if (poly_ptr2->curved == LT_INTSPLINE) {
         /* compatible */
      } else {
         rc = FALSE;
      }
      break;
   case LT_STRUCT_SPLINE:
      if (poly_ptr2->curved == LT_STRUCT_SPLINE) {
         /* compatible */
      } else {
         rc = FALSE;
      }
      break;
   }
   if (!rc) {
      MsgBox(TgLoadString(STID_SEL_2_COMPAT_END_PTS_TO_JOIN), TOOL_NAME,
            INFO_MB);
      return FALSE;
   }
   return TRUE;
}

void JoinPoly()
{
   struct ObjRec *obj_ptr1=NULL, *obj_ptr2=NULL;
   struct PolyRec *poly_ptr1=NULL, *poly_ptr2=NULL;
   long dx=0L, dy=0L, min_lval=0L;
   int i=0, min_index=(-1);

   if (curChoice != VERTEXMODE && curChoice != NOTHING) {
      MsgBox(TgLoadString(STID_CMD_ONLY_AVAIL_IN_VERSEL_MODE), TOOL_NAME,
            INFO_MB);
      return;
   }
   if (curChoice == VERTEXMODE) {
      if (CountSelectedVertices() != 2) {
         MsgBox(TgLoadString(STID_SEL_2_ENDPOINT_VER_TO_JOIN), TOOL_NAME,
               INFO_MB);
         return;
      }
      obj_ptr1 = topVSel->obj;
      obj_ptr2 = botVSel->obj;
      if (obj_ptr1->type != OBJ_POLY || obj_ptr2->type != OBJ_POLY) {
         MsgBox(TgLoadString(STID_SEL_VER_FROM_POLY_OPEN_ONLY), TOOL_NAME,
               INFO_MB);
         return;
      }
      poly_ptr1 = obj_ptr1->detail.p;
      for (i=0; i < topVSel->n; i++) {
         int index=topVSel->v_index[i];

         if (index != 0 && index != poly_ptr1->n-1) {
            MsgBox(TgLoadString(STID_SEL_2_ENDPOINT_VER_TO_JOIN), TOOL_NAME,
                  INFO_MB);
            return;
         }
      }
      if (obj_ptr1 == obj_ptr2) {
         CloseOnePoly(obj_ptr1);
         return;
      }
      poly_ptr2 = obj_ptr2->detail.p;
      for (i=0; i < botVSel->n; i++) {
         int index=botVSel->v_index[i];

         if (index != 0 && index != poly_ptr2->n-1) {
            MsgBox(TgLoadString(STID_SEL_2_ENDPOINT_VER_TO_JOIN), TOOL_NAME,
                  INFO_MB);
            return;
         }
      }
      if (!JoinPolyCompatCheck(poly_ptr1, poly_ptr2)) {
         return;
      }
      if (topVSel->v_index[0] == 0) {
         if (botVSel->v_index[0] == 0) {
            min_index = 0;
         } else {
            min_index = 1;
         }
      } else {
         if (botVSel->v_index[0] == 0) {
            min_index = 2;
         } else {
            min_index = 3;
         }
      }
      dx = topVSel->x[0] - botVSel->x[0];
      dy = topVSel->y[0] - botVSel->y[0];
      min_lval = (dx*dx)+(dy*dy);
   } else {
      IntPoint p[4], tmp_p;
      long d[4];

      if (numObjSelected == 1 && topSel->obj->type == OBJ_POLY) {
         CloseOnePoly(topSel->obj);
         return;
      } else if (numObjSelected != 2 || topSel == NULL ||
            topSel->obj->type != OBJ_POLY || botSel->obj->type != OBJ_POLY) {
         MsgBox(TgLoadString(STID_SEL_2_POLYLINES_OPEN_SPLINES), TOOL_NAME,
               INFO_MB);
         return;
      }
      obj_ptr1 = topSel->obj;
      obj_ptr2 = botSel->obj;
      poly_ptr1 = obj_ptr1->detail.p;
      poly_ptr2 = obj_ptr2->detail.p;

      if (!JoinPolyCompatCheck(poly_ptr1, poly_ptr2)) {
         return;
      }
      memcpy(&p[0], &poly_ptr1->vlist[0], sizeof(IntPoint));
      memcpy(&p[1], &poly_ptr1->vlist[poly_ptr1->n-1], sizeof(IntPoint));
      memcpy(&p[2], &poly_ptr2->vlist[0], sizeof(IntPoint));
      memcpy(&p[3], &poly_ptr2->vlist[poly_ptr2->n-1], sizeof(IntPoint));
      if (obj_ptr1->ctm != NULL) {
         TransformObjectV(obj_ptr1, &p[0], &tmp_p);
         memcpy(&p[0], &tmp_p, sizeof(IntPoint));
         TransformObjectV(obj_ptr1, &p[1], &tmp_p);
         memcpy(&p[1], &tmp_p, sizeof(IntPoint));
      }
      if (obj_ptr2->ctm != NULL) {
         TransformObjectV(obj_ptr2, &p[2], &tmp_p);
         memcpy(&p[2], &tmp_p, sizeof(IntPoint));
         TransformObjectV(obj_ptr2, &p[3], &tmp_p);
         memcpy(&p[3], &tmp_p, sizeof(IntPoint));
      }
      dx=(long)(p[0].x-p[2].x); dy=(long)(p[0].y-p[2].y); d[0]=dx*dx+dy*dy;
      dx=(long)(p[0].x-p[3].x); dy=(long)(p[0].y-p[3].y); d[1]=dx*dx+dy*dy;
      dx=(long)(p[1].x-p[2].x); dy=(long)(p[1].y-p[2].y); d[2]=dx*dx+dy*dy;
      dx=(long)(p[1].x-p[3].x); dy=(long)(p[1].y-p[3].y); d[3]=dx*dx+dy*dy;
      min_index = 0;
      min_lval = d[0];
      for (i=1; i < 4; i++) {
         if (d[i] < min_lval) {
            min_index = i;
            min_lval = d[i];
         }
      }
   }
   if (poly_ptr1->curved == LT_STRUCT_SPLINE &&
         poly_ptr2->curved == LT_STRUCT_SPLINE) {
      JoinStructSplineInfo jssi;

      memset(&jssi, 0, sizeof(JoinStructSplineInfo));
      switch (min_index) {
      case 0:
         SetIPTInfoForStretchPoly(0, poly_ptr1->n, poly_ptr1->vlist,
               &jssi.sssi1);
         SetIPTInfoForStretchPoly(0, poly_ptr2->n, poly_ptr2->vlist,
               &jssi.sssi2);
         break;
      case 1:
         SetIPTInfoForStretchPoly(0, poly_ptr1->n, poly_ptr1->vlist,
               &jssi.sssi1);
         SetIPTInfoForStretchPoly(poly_ptr2->ssn-1, poly_ptr2->n,
               poly_ptr2->vlist, &jssi.sssi2);
         break;
      case 2:
         SetIPTInfoForStretchPoly(poly_ptr1->ssn-1, poly_ptr1->n,
               poly_ptr1->vlist, &jssi.sssi1);
         SetIPTInfoForStretchPoly(0, poly_ptr2->n, poly_ptr2->vlist,
               &jssi.sssi2);
         break;
      case 3:
         SetIPTInfoForStretchPoly(poly_ptr1->ssn-1, poly_ptr1->n,
               poly_ptr1->vlist, &jssi.sssi1);
         SetIPTInfoForStretchPoly(poly_ptr2->ssn-1, poly_ptr2->n,
               poly_ptr2->vlist, &jssi.sssi2);
         break;
      }
      DoJoinPoly(obj_ptr1, obj_ptr2, poly_ptr1, poly_ptr2, min_index,
            (min_lval==0L), &jssi);
   } else {
      DoJoinPoly(obj_ptr1, obj_ptr2, poly_ptr1, poly_ptr2, min_index,
            (min_lval==0L), NULL);
   }
}

/* --------------------- CutPoly() --------------------- */

static
void FinishCutPoly(obj_ptr1, poly_ptr1, vs1, smooth1, vs2, smooth2,
      num_pts1, num_pts2)
   struct ObjRec *obj_ptr1;
   struct PolyRec *poly_ptr1;
   IntPoint *vs1, *vs2;
   char *smooth1, *smooth2;
   int num_pts1, num_pts2;
{
   struct ObjRec *obj_ptr2;
   struct PolyRec *poly_ptr2;
   int x0, y0;

   x0 = topVSel->x[0];
   y0 = topVSel->y[0];

   RemoveAllSel();
   PrepareToReplaceAnObj(obj_ptr1);

   obj_ptr2 = DupObj(obj_ptr1);
   poly_ptr2 = obj_ptr2->detail.p;
   DelAllAttrs(obj_ptr2->fattr);
   obj_ptr2->fattr = obj_ptr2->lattr = NULL;

   if (poly_ptr1->vlist != NULL) free(poly_ptr1->vlist);
   if (poly_ptr1->svlist != NULL) free(poly_ptr1->svlist);
   if (poly_ptr1->asvlist != NULL) free(poly_ptr1->asvlist);
   if (poly_ptr1->smooth != NULL) free(poly_ptr1->smooth);
   if (poly_ptr1->intvlist != NULL) free(poly_ptr1->intvlist);
   if (poly_ptr1->rotated_vlist != NULL) free(poly_ptr1->rotated_vlist);
   if (poly_ptr1->rotated_asvlist != NULL) free(poly_ptr1->rotated_asvlist);
   poly_ptr1->smooth = smooth1;
   poly_ptr1->vlist = poly_ptr1->intvlist = NULL;
   poly_ptr1->n = poly_ptr1->intn = 0;
   poly_ptr1->svlist = poly_ptr1->asvlist = poly_ptr1->rotated_vlist =
         poly_ptr1->rotated_asvlist = NULL;
   poly_ptr1->sn = poly_ptr1->asn = poly_ptr1->rotated_n =
         poly_ptr1->rotated_asn = 0;
   if (obj_ptr1->ctm != NULL) free(obj_ptr1->ctm);
   obj_ptr1->ctm = NULL;

   poly_ptr1->vlist = vs1;
   poly_ptr1->n = num_pts1;

   if (poly_ptr2->vlist != NULL) free(poly_ptr2->vlist);
   if (poly_ptr2->svlist != NULL) free(poly_ptr2->svlist);
   if (poly_ptr2->asvlist != NULL) free(poly_ptr2->asvlist);
   if (poly_ptr2->smooth != NULL) free(poly_ptr2->smooth);
   if (poly_ptr2->intvlist != NULL) free(poly_ptr2->intvlist);
   if (poly_ptr2->rotated_vlist != NULL) free(poly_ptr2->rotated_vlist);
   if (poly_ptr2->rotated_asvlist != NULL) free(poly_ptr2->rotated_asvlist);
   poly_ptr2->smooth = smooth2;
   poly_ptr2->vlist = poly_ptr2->intvlist = NULL;
   poly_ptr2->n = poly_ptr2->intn = 0;
   poly_ptr2->svlist = poly_ptr2->asvlist = poly_ptr2->rotated_vlist =
         poly_ptr2->rotated_asvlist = NULL;
   poly_ptr2->sn = poly_ptr2->asn = poly_ptr2->rotated_n =
         poly_ptr2->rotated_asn = 0;
   if (obj_ptr2->ctm != NULL) free(obj_ptr2->ctm);
   obj_ptr2->ctm = NULL;

   poly_ptr2->vlist = vs2;
   poly_ptr2->n = num_pts2;

   AdjObjSplineVs(obj_ptr1);
   if (poly_ptr1->curved != LT_INTSPLINE) {
      UpdPolyBBox(obj_ptr1, poly_ptr1->n, poly_ptr1->vlist);
   } else {
      UpdPolyBBox(obj_ptr1, poly_ptr1->intn, poly_ptr1->intvlist);
   }
   AdjObjBBox(obj_ptr1);

   AdjObjSplineVs(obj_ptr2);
   if (poly_ptr2->curved != LT_INTSPLINE) {
      UpdPolyBBox(obj_ptr2, poly_ptr2->n, poly_ptr2->vlist);
   } else {
      UpdPolyBBox(obj_ptr2, poly_ptr2->intn, poly_ptr2->intvlist);
   }
   AdjObjBBox(obj_ptr2);
   AddObj(obj_ptr1->prev, obj_ptr1, obj_ptr2);

   topSel = (struct SelRec *)malloc(sizeof(struct SelRec));
   botSel = (struct SelRec *)malloc(sizeof(struct SelRec));
   if (topSel==NULL || botSel==NULL) FailAllocMessage();
   topSel->obj = obj_ptr2;
   botSel->obj = obj_ptr1;
   topSel->prev = botSel->next = NULL;
   topSel->next = botSel;
   botSel->prev = topSel;

   RecordCmd(CMD_ONE_TO_MANY, NULL, topSel, botSel, 2);

   topVSel = botVSel = (struct VSelRec *)malloc(sizeof(struct VSelRec));
   if (topVSel==NULL) FailAllocMessage();
   memset(topVSel, 0, sizeof(struct VSelRec));
   topVSel->obj = obj_ptr2;
   topVSel->max_v = 10;
   topVSel->v_index = (int*)malloc(10*sizeof(int));
   topVSel->x = (int*)malloc(10*sizeof(int));
   topVSel->y = (int*)malloc(10*sizeof(int));
   if (topVSel->v_index==NULL || topVSel->x==NULL || topVSel->y==NULL) {
      FailAllocMessage();
   }
   topVSel->v_index[0] = 0;
   topVSel->x[0] = x0;
   topVSel->y[0] = y0;
   topVSel->n = 1;
   topVSel->next = topVSel->prev = NULL;
}

static
void DoCutPoly(obj_ptr, index, poly_ptr)
   struct ObjRec *obj_ptr;
   int index;
   struct PolyRec *poly_ptr;
{
   int i=0, n=poly_ptr->n, n1=0, n2=0, ltx, lty, rbx, rby;
   int curved=poly_ptr->curved;
   IntPoint *vs1=NULL, *vs2=NULL;
   char *smooth1=NULL, *smooth2=NULL;

   if (curved == LT_STRUCT_SPLINE) {
      StretchStructuredSplineInfo sssi;

      memset(&sssi, 0, sizeof(StretchStructuredSplineInfo));
      if (index == poly_ptr->ssn-1) {
         return;
      }
      SetIPTInfoForStretchPoly(index, poly_ptr->n, poly_ptr->vlist, &sssi);
      if (!sssi.hinge) {
         MsgBox(TgLoadString(STID_CANNOT_CUT_AT_SMOOTH_PT), TOOL_NAME, INFO_MB);
         return;
      }
      n1 = sssi.orig_hinge_index + 1;
      n2 = n - sssi.orig_hinge_index;
      vs1 = (IntPoint*)malloc((n1+1)*sizeof(IntPoint));
      vs2 = (IntPoint*)malloc((n2+1)*sizeof(IntPoint));
      if (vs1==NULL || vs2==NULL) FailAllocMessage();

      for (i=0; i <= sssi.orig_hinge_index; i++) {
         if (obj_ptr->ctm == NULL) {
            memcpy(&vs1[i], &poly_ptr->vlist[i], sizeof(IntPoint));
         } else {
            IntPoint tmp_p;
   
            TransformObjectV(obj_ptr, &poly_ptr->vlist[i], &tmp_p);
            memcpy(&vs1[i], &tmp_p, sizeof(IntPoint));
         }
      }
      for (i=sssi.orig_hinge_index; i < n; i++) {
         if (obj_ptr->ctm == NULL) {
            memcpy(&vs2[i-sssi.orig_hinge_index], &poly_ptr->vlist[i],
                  sizeof(IntPoint));
         } else {
            IntPoint tmp_p;
   
            TransformObjectV(obj_ptr, &poly_ptr->vlist[i], &tmp_p);
            memcpy(&vs2[i-sssi.orig_hinge_index], &tmp_p, sizeof(IntPoint));
         }
      }
   } else {
      n1 = index+1;
      n2 = n-index;
      vs1 = (IntPoint*)malloc((n1+1)*sizeof(IntPoint));
      vs2 = (IntPoint*)malloc((n2+1)*sizeof(IntPoint));
      if (vs1==NULL || vs2==NULL) FailAllocMessage();
      if (poly_ptr->curved != LT_INTSPLINE) {
         smooth1 = (char*)malloc((n1+1)*sizeof(char));
         smooth2 = (char*)malloc((n2+1)*sizeof(char));
         if (smooth1==NULL || smooth2==NULL) FailAllocMessage();
      }
      for (i=0; i <= index; i++) {
         if (obj_ptr->ctm == NULL) {
            memcpy(&vs1[i], &poly_ptr->vlist[i], sizeof(IntPoint));
         } else {
            IntPoint tmp_p;
   
            TransformObjectV(obj_ptr, &poly_ptr->vlist[i], &tmp_p);
            memcpy(&vs1[i], &tmp_p, sizeof(IntPoint));
         }
         if (smooth1 != NULL) {
            smooth1[i] = poly_ptr->smooth[i];
         }
      }
      if (smooth1 != NULL) smooth1[0] = smooth1[index] = FALSE;
   
      for (i=index; i < n; i++) {
         if (obj_ptr->ctm == NULL) {
            memcpy(&vs2[i-index], &poly_ptr->vlist[i], sizeof(IntPoint));
         } else {
            IntPoint tmp_p;
   
            TransformObjectV(obj_ptr, &poly_ptr->vlist[i], &tmp_p);
            memcpy(&vs2[i-index], &tmp_p, sizeof(IntPoint));
         }
         if (smooth2 != NULL) {
            smooth2[i-index] = poly_ptr->smooth[i];
         }
      }
      if (smooth2 != NULL) smooth2[0] = smooth2[n-1-index] = FALSE;
   }
   ltx = selLtX; lty = selLtY; rbx = selRbX; rby = selRbY;
   HighLightReverse();
   FinishCutPoly(obj_ptr, poly_ptr, vs1, smooth1, vs2, smooth2, n1, n2);
   UpdSelBBox();
   RedrawAreas(botObj, ltx-GRID_ABS_SIZE(1), lty-GRID_ABS_SIZE(1),
         rbx+GRID_ABS_SIZE(1), rby+GRID_ABS_SIZE(1),
         selLtX-GRID_ABS_SIZE(1), selLtY-GRID_ABS_SIZE(1),
         selRbX+GRID_ABS_SIZE(1), selRbY+GRID_ABS_SIZE(1));
   SetFileModified(TRUE);
   justDupped = FALSE;
   HighLightForward();
}

static
void FinishCutPolygon(obj_ptr1, polygon_ptr1, vs, smooth, num_pts)
   struct ObjRec *obj_ptr1;
   struct PolygonRec *polygon_ptr1;
   IntPoint *vs;
   char *smooth;
   int num_pts;
{
   struct ObjRec *obj_ptr2;
   struct PolyRec *poly_ptr;
   struct PolygonRec *polygon_ptr2;
   int x0, y0;

   x0 = topVSel->x[0];
   y0 = topVSel->y[0];

   poly_ptr = (struct PolyRec *)malloc(sizeof(struct PolyRec));
   if (poly_ptr == NULL) FailAllocMessage();

   memset(poly_ptr, 0, sizeof(struct PolyRec));
   poly_ptr->n = num_pts;
   poly_ptr->vlist = vs;
   poly_ptr->smooth = smooth;
   poly_ptr->asn = 0;
   poly_ptr->asvlist = NULL;
   poly_ptr->sn = 0;
   poly_ptr->svlist = NULL;
   poly_ptr->intn = 0;
   poly_ptr->intvlist = NULL;
   poly_ptr->style = LS_PLAIN;
   poly_ptr->width = polygon_ptr1->width;
   UtilStrCpyN(poly_ptr->width_spec, sizeof(poly_ptr->width_spec),
         polygon_ptr1->width_spec);
   poly_ptr->pen = polygon_ptr1->pen;
   poly_ptr->curved = polygon_ptr1->curved;
   poly_ptr->fill = polygon_ptr1->fill;
   poly_ptr->dash = polygon_ptr1->dash;
   if (poly_ptr->width == curWidthOfLine[lineWidth]) {
      poly_ptr->aw = curArrowHeadW[lineWidth];
      poly_ptr->ah = curArrowHeadH[lineWidth];
      UtilStrCpyN(poly_ptr->aw_spec, sizeof(poly_ptr->aw_spec),
            curArrowHeadWSpec[lineWidth]);
      UtilStrCpyN(poly_ptr->ah_spec, sizeof(poly_ptr->ah_spec),
            curArrowHeadHSpec[lineWidth]);
   } else if (maxLineWidths > 0) {
      int i, width=poly_ptr->width;
      int min_diff=abs(curWidthOfLine[0]-width), min_index=0;

      for (i=1; min_diff > 0 && i < maxLineWidths; i++) {
         int diff=abs(curWidthOfLine[i]-width);

         if (diff < min_diff) {
            min_diff = diff;
            min_index = i;
         }
      }
      poly_ptr->aw = curArrowHeadW[min_index];
      poly_ptr->ah = curArrowHeadH[min_index];
      UtilStrCpyN(poly_ptr->aw_spec, sizeof(poly_ptr->aw_spec),
            curArrowHeadWSpec[min_index]);
      UtilStrCpyN(poly_ptr->ah_spec, sizeof(poly_ptr->ah_spec),
            curArrowHeadHSpec[min_index]);
   }
   poly_ptr->rotated_n = poly_ptr->rotated_asn = 0;
   poly_ptr->rotated_vlist = poly_ptr->rotated_asvlist = NULL;

   JustRemoveAllVSel();
   PrepareToReplaceAnObj(obj_ptr1);

   obj_ptr2 = DupObj(obj_ptr1);
   obj_ptr2->fattr = obj_ptr1->fattr;
   obj_ptr2->lattr = obj_ptr1->lattr;
   obj_ptr1->fattr = obj_ptr1->lattr = NULL;
   polygon_ptr2 = obj_ptr2->detail.g;

   if (polygon_ptr2->vlist != NULL) free(polygon_ptr2->vlist);
   if (polygon_ptr2->svlist != NULL) free(polygon_ptr2->svlist);
   if (polygon_ptr2->smooth != NULL) free(polygon_ptr2->smooth);
   if (polygon_ptr2->intvlist != NULL) free(polygon_ptr2->intvlist);
   if (polygon_ptr2->rotated_vlist != NULL) free(polygon_ptr2->rotated_vlist);
   free(polygon_ptr2);
   if (obj_ptr2->ctm != NULL) free(obj_ptr2->ctm);
   obj_ptr2->ctm = NULL;

   obj_ptr2->type = OBJ_POLY;
   obj_ptr2->detail.p = poly_ptr;

   AdjObjSplineVs(obj_ptr2);
   if (poly_ptr->curved != LT_INTSPLINE) {
      UpdPolyBBox(obj_ptr2, poly_ptr->n, poly_ptr->vlist);
   } else {
      UpdPolyBBox(obj_ptr2, poly_ptr->intn, poly_ptr->intvlist);
   }
   AdjObjBBox(obj_ptr2);

   AddObj(obj_ptr1->prev, obj_ptr1, obj_ptr2);
   UnlinkObj(obj_ptr1);
   FreeObj(obj_ptr1);

   topSel->obj = obj_ptr2;
   topSel->prev = topSel->next = NULL;
   botSel = topSel;

   RecordReplaceAnObj(obj_ptr2);

   topVSel = botVSel = (struct VSelRec *)malloc(sizeof(struct VSelRec));
   if (topVSel == NULL) FailAllocMessage();
   memset(topVSel, 0, sizeof(struct VSelRec));
   topVSel->obj = obj_ptr2;
   topVSel->max_v = 10;
   topVSel->v_index = (int*)malloc(10*sizeof(int));
   topVSel->x = (int*)malloc(10*sizeof(int));
   topVSel->y = (int*)malloc(10*sizeof(int));
   if (topVSel->v_index==NULL || topVSel->x==NULL || topVSel->y==NULL) {
      FailAllocMessage();
   }
   topVSel->v_index[0] = 0;
   topVSel->x[0] = x0;
   topVSel->y[0] = y0;
   topVSel->n = 1;
   topVSel->next = topVSel->prev = NULL;
}

static
void DoCutPolygon(obj_ptr, index, polygon_ptr)
   struct ObjRec *obj_ptr;
   int index;
   struct PolygonRec *polygon_ptr;
{
   int i, n=polygon_ptr->n, num_pts, ltx, lty, rbx, rby;
   int curved=polygon_ptr->curved;
   IntPoint *vs=NULL;
   char *smooth=NULL;

   if (curved == LT_STRUCT_SPLINE) {
      StretchStructuredSplineInfo sssi;

      memset(&sssi, 0, sizeof(StretchStructuredSplineInfo));
      SetIPTInfoForStretchPolygon(index, polygon_ptr->n, polygon_ptr->vlist,
            &sssi);
      if (!sssi.hinge) {
         MsgBox(TgLoadString(STID_CANNOT_CUT_AT_SMOOTH_PT), TOOL_NAME, INFO_MB);
         return;
      }
      num_pts = n;
      vs = (IntPoint*)malloc((num_pts+1)*sizeof(IntPoint));
      if (vs == NULL) FailAllocMessage();
      for (i=sssi.orig_hinge_index; i < num_pts; i++) {
         if (obj_ptr->ctm == NULL) {
            memcpy(&vs[i-sssi.orig_hinge_index], &polygon_ptr->vlist[i],
                  sizeof(IntPoint));
         } else {
            IntPoint tmp_p;
   
            TransformObjectV(obj_ptr, &polygon_ptr->vlist[i], &tmp_p);
            memcpy(&vs[i-sssi.orig_hinge_index], &tmp_p, sizeof(IntPoint));
         }
      }
      for (i=1; i <= sssi.orig_hinge_index; i++) {
         if (obj_ptr->ctm == NULL) {
            memcpy(&vs[i+num_pts-sssi.orig_hinge_index-1],
                  &polygon_ptr->vlist[i], sizeof(IntPoint));
         } else {
            IntPoint tmp_p;
   
            TransformObjectV(obj_ptr, &polygon_ptr->vlist[i], &tmp_p);
            memcpy(&vs[i+num_pts-sssi.orig_hinge_index-1], &tmp_p,
                  sizeof(IntPoint));
         }
      }
   } else {
      num_pts = n;
      vs = (IntPoint*)malloc((num_pts+1)*sizeof(IntPoint));
      if (vs == NULL) FailAllocMessage();
      if (polygon_ptr->curved != LT_INTSPLINE) {
         smooth = (char*)malloc((num_pts+1)*sizeof(char));
         if (smooth == NULL) FailAllocMessage();
      }
      for (i=index; i < num_pts; i++) {
         if (obj_ptr->ctm == NULL) {
            memcpy(&vs[i-index], &polygon_ptr->vlist[i], sizeof(IntPoint));
         } else {
            IntPoint tmp_p;
   
            TransformObjectV(obj_ptr, &polygon_ptr->vlist[i], &tmp_p);
            memcpy(&vs[i-index], &tmp_p, sizeof(IntPoint));
         }
         if (smooth != NULL) {
            smooth[i-index] = polygon_ptr->smooth[i];
         }
      }
      for (i=1; i <= index; i++) {
         if (obj_ptr->ctm == NULL) {
            memcpy(&vs[i+num_pts-index-1], &polygon_ptr->vlist[i],
                  sizeof(IntPoint));
         } else {
            IntPoint tmp_p;
   
            TransformObjectV(obj_ptr, &polygon_ptr->vlist[i], &tmp_p);
            memcpy(&vs[i+num_pts-index-1], &tmp_p, sizeof(IntPoint));
         }
         if (smooth != NULL) {
            smooth[i+num_pts-index-1] = polygon_ptr->smooth[i];
         }
      }
      if (smooth != NULL) smooth[0] = smooth[num_pts-1] = FALSE;
   }
   ltx = selLtX; lty = selLtY; rbx = selRbX; rby = selRbY;
   HighLightReverse();
   FinishCutPolygon(obj_ptr, polygon_ptr, vs, smooth, num_pts);
   UpdSelBBox();
   RedrawAreas(botObj, ltx-GRID_ABS_SIZE(1), lty-GRID_ABS_SIZE(1),
         rbx+GRID_ABS_SIZE(1), rby+GRID_ABS_SIZE(1),
         selLtX-GRID_ABS_SIZE(1), selLtY-GRID_ABS_SIZE(1),
         selRbX+GRID_ABS_SIZE(1), selRbY+GRID_ABS_SIZE(1));
   SetFileModified(TRUE);
   justDupped = FALSE;
   HighLightForward();
}

static
void CopyPolySegmentProperties(obj_ptr, poly_ptr, curved)
   struct ObjRec *obj_ptr;
   struct PolyRec *poly_ptr;
   int curved;
{
   struct PolyRec *new_poly_ptr=topObj->detail.p;

   new_poly_ptr->style = LS_PLAIN;
   new_poly_ptr->width = poly_ptr->width;
   new_poly_ptr->aw = poly_ptr->aw;
   new_poly_ptr->ah = poly_ptr->ah;
   strcpy(new_poly_ptr->width_spec, poly_ptr->width_spec);
   strcpy(new_poly_ptr->aw_spec, poly_ptr->aw_spec);
   strcpy(new_poly_ptr->ah_spec, poly_ptr->ah_spec);
   new_poly_ptr->pen = poly_ptr->pen;
   new_poly_ptr->fill = poly_ptr->fill;
   new_poly_ptr->curved = curved;
   new_poly_ptr->dash = poly_ptr->dash;

   topObj->color = obj_ptr->color;
   topObj->bg_color = obj_ptr->bg_color;
   memcpy(topObj->color_str, obj_ptr->color_str, sizeof(obj_ptr->color_str));
   memcpy(topObj->bg_color_str, obj_ptr->bg_color_str,
         sizeof(obj_ptr->bg_color_str));
   topObj->trans_pat = obj_ptr->trans_pat;

   AdjObjSplineVs(topObj);
   AdjObjBBox(topObj);
}

static
void CopyPolygonSegmentProperties(obj_ptr, polygon_ptr, curved)
   struct ObjRec *obj_ptr;
   struct PolygonRec *polygon_ptr;
   int curved;
{
   struct PolyRec *new_poly_ptr=topObj->detail.p;
   int index=0;

   new_poly_ptr->style = LS_PLAIN;
   new_poly_ptr->width = polygon_ptr->width;
   strcpy(new_poly_ptr->width_spec, polygon_ptr->width_spec);
   index = GetBestLineWidthIndex(new_poly_ptr->width, INVALID, INVALID);
   new_poly_ptr->aw = curArrowHeadW[index];
   new_poly_ptr->ah = curArrowHeadH[index];
   strcpy(new_poly_ptr->aw_spec, curArrowHeadWSpec[index]);
   strcpy(new_poly_ptr->ah_spec, curArrowHeadHSpec[index]);
   new_poly_ptr->pen = polygon_ptr->pen;
   new_poly_ptr->fill = polygon_ptr->fill;
   new_poly_ptr->curved = curved;
   new_poly_ptr->dash = polygon_ptr->dash;

   topObj->color = obj_ptr->color;
   topObj->bg_color = obj_ptr->bg_color;
   memcpy(topObj->color_str, obj_ptr->color_str, sizeof(obj_ptr->color_str));
   memcpy(topObj->bg_color_str, obj_ptr->bg_color_str,
         sizeof(obj_ptr->bg_color_str));
   topObj->trans_pat = obj_ptr->trans_pat;

   AdjObjSplineVs(topObj);
   AdjObjBBox(topObj);
}

static
int CreateSegmentsFromObject(obj_ptr, pp_top_obj, pp_bot_obj)
   struct ObjRec *obj_ptr, **pp_top_obj, **pp_bot_obj;
{
   int i=0, n=0, ssn=0, saved_line_style=0, saved_spline=0, curved=(-1);
   IntPoint *vlist=NULL, *ssvlist=NULL, tmp_p;
   struct PolyRec *poly_ptr=NULL;
   struct PolygonRec *polygon_ptr=NULL;
   struct ObjRec *saved_top_obj=topObj, *saved_bot_obj=botObj;
   char *ssmooth=NULL;

   switch (obj_ptr->type) {
   case OBJ_POLY:
      poly_ptr = obj_ptr->detail.p;
      curved = poly_ptr->curved;
      n = poly_ptr->n;
      if (n <= 2) return FALSE;
      vlist = poly_ptr->vlist;
      if (curved == LT_STRUCT_SPLINE) {
         ssn = poly_ptr->ssn;
         if (ssn <= 2) return FALSE;
         ssvlist = poly_ptr->ssvlist;
         ssmooth = poly_ptr->ssmooth;
      }
      break;
   case OBJ_POLYGON:
      polygon_ptr = obj_ptr->detail.g;
      curved = polygon_ptr->curved;
      n = polygon_ptr->n;
      if (n <= 2) return FALSE;
      vlist = polygon_ptr->vlist;
      if (curved == LT_STRUCT_SPLINE) {
         ssn = polygon_ptr->ssn;
         if (ssn <= 2) return FALSE;
         ssvlist = polygon_ptr->ssvlist;
         ssmooth = polygon_ptr->ssmooth;
      }
      break;
   }
   curPage->top = topObj = (*pp_top_obj);
   curPage->bot = botObj = (*pp_bot_obj);
   saved_line_style = lineStyle;
   saved_spline = curSpline;
   lineStyle = LS_PLAIN;
   if (curved == LT_STRUCT_SPLINE) {
      IntPoint tmp_vs[4];
      int num_vs=0;

      curSpline = LT_STRUCT_SPLINE;
      ResetCreatePoly();
      if (obj_ptr->ctm == NULL) {
         memcpy(&tmp_vs[0], &ssvlist[0], sizeof(IntPoint));
      } else {
         TransformObjectV(obj_ptr, &ssvlist[0], &tmp_p);
         memcpy(&tmp_vs[0], &tmp_p, sizeof(IntPoint));
      }
      num_vs = 1;
      for (i=1; i < ssn; i++) {
         if (obj_ptr->ctm == NULL) {
            memcpy(&tmp_vs[num_vs], &ssvlist[i], sizeof(IntPoint));
         } else {
            TransformObjectV(obj_ptr, &ssvlist[i], &tmp_p);
            memcpy(&tmp_vs[num_vs], &tmp_p, sizeof(IntPoint));
         }
         num_vs++;
         if (!ssmooth[i]) {
            IntPointTriplet ipt_first, ipt_last;

            memset(&ipt_first, 0, sizeof(IntPointTriplet));
            memset(&ipt_last, 0, sizeof(IntPointTriplet));

            memcpy(&ipt_first.hinge_pt, &tmp_vs[0], sizeof(IntPoint));
            ipt_first.earlier_valid = ipt_first.later_valid = FALSE;
            ipt_first.ratio = (double)1;

            memcpy(&ipt_last.hinge_pt, &tmp_vs[num_vs-1], sizeof(IntPoint));
            ipt_last.earlier_valid = ipt_last.later_valid = FALSE;
            ipt_last.ratio = (double)1;

            switch (num_vs) {
            case 2: break;
            case 3:
               memcpy(&ipt_first.later_smooth_pt, &tmp_vs[1], sizeof(IntPoint));
               ipt_first.later_valid = TRUE;
               break;
            case 4:
               memcpy(&ipt_first.later_smooth_pt, &tmp_vs[1], sizeof(IntPoint));
               ipt_first.later_valid = TRUE;
               memcpy(&ipt_last.earlier_smooth_pt, &tmp_vs[2],
                     sizeof(IntPoint));
               ipt_last.earlier_valid = TRUE;
               break;
            }
            AddPointForCont(0, 0, &ipt_first);
            AddPointForCont(0, 0, &ipt_last);
            CreatePolyObj(2, TRUE);
            switch (obj_ptr->type) {
            case OBJ_POLY:
               CopyPolySegmentProperties(obj_ptr, poly_ptr, LT_STRUCT_SPLINE);
               break;
            case OBJ_POLYGON:
               CopyPolygonSegmentProperties(obj_ptr, polygon_ptr,
                     LT_STRUCT_SPLINE);
               break;
            }
            ResetCreatePoly();
            memcpy(&tmp_vs[0], &tmp_vs[num_vs-1], sizeof(IntPoint));
            num_vs = 1;
         }
      }
   } else {
      IntPoint *tmp_vs=NULL;

      tmp_vs = (IntPoint*)malloc(n*sizeof(IntPoint));
      if (tmp_vs == NULL) FailAllocMessage();
      memset(tmp_vs, 0, n*sizeof(IntPoint));
      for (i=0; i < n; i++) {
         if (obj_ptr->ctm == NULL) {
            memcpy(&tmp_vs[i], &vlist[i], sizeof(IntPoint));
         } else {
            TransformObjectV(obj_ptr, &vlist[i], &tmp_p);
            memcpy(&tmp_vs[i], &tmp_p, sizeof(IntPoint));
         }
      }
      curSpline = LT_STRAIGHT;
      for (i=0; i < n-1; i++) {
         ResetCreatePoly();
         AddPtToCreatePoly(tmp_vs[i].x, tmp_vs[i].y);
         AddPtToCreatePoly(tmp_vs[i+1].x, tmp_vs[i+1].y);
         CreatePolyObj(2, TRUE);
         switch (obj_ptr->type) {
         case OBJ_POLY:
            CopyPolySegmentProperties(obj_ptr, poly_ptr, LT_STRAIGHT);
            break;
         case OBJ_POLYGON:
            CopyPolygonSegmentProperties(obj_ptr, polygon_ptr, LT_STRAIGHT);
            break;
         }
      }
      free(tmp_vs);
   }
   lineStyle = saved_line_style;
   curSpline = saved_spline;
   (*pp_top_obj) = topObj;
   (*pp_bot_obj) = botObj;
   curPage->top = topObj = saved_top_obj;
   curPage->bot = botObj = saved_bot_obj;

   return TRUE;
}

static
void CutPolyIntoSegments()
{
   struct SelRec *sel_ptr=NULL, *new_top_sel=NULL, *new_bot_sel=NULL;
   int something_changed=FALSE;

   UnSelNonVertexObjs(TRUE, FALSE); /* with highlight */
   UpdSelBBox();
   if (topSel == NULL) {
      MsgBox(TgLoadString(STID_NO_POLY_OBJ_SELECTED), TOOL_NAME, INFO_MB);
      return;
   }
   HighLightReverse();
   StartCompositeCmd();

   for (sel_ptr=botSel; sel_ptr != NULL; sel_ptr=sel_ptr->prev) {
      struct ObjRec *new_top_obj=NULL, *new_bot_obj=NULL;

      PrepareToReplaceAnObj(sel_ptr->obj);
      if (CreateSegmentsFromObject(sel_ptr->obj, &new_top_obj, &new_bot_obj)) {
         struct ObjRec *obj_ptr=NULL;
         struct SelRec *saved_top_sel=topSel, *saved_bot_sel=botSel;
         int count=0;

         something_changed = TRUE;
         UnlinkObj(sel_ptr->obj);
         FreeObj(sel_ptr->obj);

         /* add newly created line segments to the top of the object chain */
         new_bot_obj->next = topObj;
         if (topObj == NULL) {
            curPage->bot = botObj = new_bot_obj;
         } else {
            topObj->prev = new_bot_obj;
         }
         curPage->top = topObj = new_top_obj;

         topSel = botSel = NULL;
         for (obj_ptr=new_bot_obj; obj_ptr != NULL; obj_ptr=obj_ptr->prev,
               count++) {
            AddSel(NULL, topSel, SelectThisObject(obj_ptr));
         }
         UpdSelBBox();
         RecordCmd(CMD_ONE_TO_MANY, NULL, topSel, botSel, numObjSelected);

         botSel->next = new_top_sel;
         if (new_top_sel == NULL) {
            new_bot_sel = botSel;
         } else {
            new_top_sel->prev = botSel;
         }
         new_top_sel = topSel;

         topSel = saved_top_sel;
         botSel = saved_bot_sel;
      } else {
         AbortPrepareCmd(CMD_REPLACE);
      }
   }
   EndCompositeCmd();

   if (something_changed) {
      RemoveAllSel();
      topSel = new_top_sel;
      botSel = new_bot_sel;
      UpdSelBBox();

      RedrawAnArea(botObj, selLtX-GRID_ABS_SIZE(1), selLtY-GRID_ABS_SIZE(1),
            selRbX+GRID_ABS_SIZE(1), selRbY+GRID_ABS_SIZE(1));
      SetFileModified(TRUE);
      justDupped = FALSE;
   }
   HighLightForward();
}

void CutPoly()
{
   struct ObjRec *obj_ptr;
   struct PolyRec *poly_ptr=NULL;
   struct PolygonRec *polygon_ptr=NULL;
   int index;

   if (curChoice == NOTHING) {
      if (topSel == NULL) {
         MsgBox(TgLoadCachedString(CSTID_NO_OBJ_SELECTED), TOOL_NAME, INFO_MB);
         return;
      }
      if (MsgBox(TgLoadString(STID_Q_OK_TO_CUT_INTO_SEGMENTS), TOOL_NAME,
            YNC_MB) != MB_ID_YES) {
         return;
      }
      CutPolyIntoSegments();
      return;
   } else if (curChoice != VERTEXMODE) {
      MsgBox(TgLoadString(STID_CMD_ONLY_AVAIL_IN_VERTEX_MODE), TOOL_NAME,
            INFO_MB);
      return;
   } else if (CountSelectedVertices() != 1) {
      MsgBox(TgLoadString(STID_SEL_1_VERTEX_TO_CUT), TOOL_NAME, INFO_MB);
      return;
   }
   obj_ptr = topVSel->obj;
   index = topVSel->v_index[0];
   switch (obj_ptr->type) {
   case OBJ_POLY:
      poly_ptr = obj_ptr->detail.p;
      if (index==0 || index==poly_ptr->n-1) return;
      DoCutPoly(obj_ptr, index, poly_ptr);
      break;
   case OBJ_POLYGON:
      polygon_ptr = obj_ptr->detail.g;
      DoCutPolygon(obj_ptr, index, polygon_ptr);
      break;
   default:
      MsgBox(TgLoadString(STID_SEL_A_VERTEX_FROM_POLY_OR_GON), TOOL_NAME,
            INFO_MB);
      return;
   }
}

/* --------------------- GetWidthInDouble() --------------------- */

double GetWidthInDouble(nVal, pszSpec, pnIsInt)
   int nVal, *pnIsInt;
   char *pszSpec;
{
   float fval;

   if (pnIsInt != NULL) *pnIsInt = TRUE;
   if (pszSpec == NULL || *pszSpec == '\0') return (double)nVal;
   if (sscanf(pszSpec, "%f", &fval) != 1) return (double)nVal;
   if (pnIsInt != NULL) {
      if (fabs(((float)nVal)-fval) > INT_TOL) {
         *pnIsInt = FALSE;
      }
   }
   return ((double)fval);
}

/* --------------------- SetWidthAndSpec() --------------------- */

void SetWidthAndSpec(dWidth, pnWidth, pszWidthSpec)
   double dWidth;
   int *pnWidth;
   char *pszWidthSpec;
{
   float fval=(float)dWidth;

   FormatFloat(&fval, pszWidthSpec);
   *pnWidth = round(dWidth);
}

/* --------------------- ScaleWidthAndSpec() --------------------- */

void ScaleWidthAndSpec(dScale, pnWidth, pszWidthSpec)
   double dScale;
   int *pnWidth;
   char *pszWidthSpec; /* char[40] */
{
   double dw=GetWidthInDouble(*pnWidth, pszWidthSpec, NULL);

   dw = dw*dScale;
   SetWidthAndSpec(dw, pnWidth, pszWidthSpec);
}

/* --------------------- DumpArrow() --------------------- */

void DumpArrow(FP, TailV, HeadV, ArrowW, ArrowH, aw_spec, ah_spec, Pen,
      TransPat, ColorIndex)
   FILE *FP;
   IntPoint *TailV, *HeadV;
   int ArrowW, ArrowH, Pen, TransPat, ColorIndex;
   char *aw_spec, *ah_spec;
   /* HeadV is where the arrow tip is */
   /* Make sure this function is not called with (Pen==BACKPAT && TransPat) */
{
   int i, dx, dy;
   struct BBRec bbox;
   IntPoint v[2];
   double len, sin, cos, daw, dah;

   memset(&bbox, 0, sizeof(struct BBRec));
   dx = HeadV->x - TailV->x;
   dy = HeadV->y - TailV->y;

   if (dx == 0 && dy == 0) return;

   fprintf(FP, "%s\n", gPsCmd[PS_GSAVE]);
   fprintf(FP, "   %s\n", &(gPsCmd[PS_TGIFSETMATRIX])[1]);

   daw = GetWidthInDouble(ArrowW, aw_spec, NULL);
   dah = GetWidthInDouble(ArrowH, ah_spec, NULL);
   if (colorDump || !useGray) {
      len = (double)sqrt((double)(((double)dx)*((double)dx) +
            ((double)dy)*((double)dy)));
      sin = ((double)dy) / len;
      cos = ((double)dx) / len;

      v[0].x = round(HeadV->x - daw*cos + dah*sin);
      v[0].y = round(HeadV->y - daw*sin - dah*cos);
      v[1].x = round(HeadV->x - daw*cos - dah*sin);
      v[1].y = round(HeadV->y - daw*sin + dah*cos);

      bbox.ltx = bbox.rbx = HeadV->x;
      bbox.lty = bbox.rby = HeadV->y;

      for (i = 0; i < 2; i++) {
         if (v[i].x < bbox.ltx) bbox.ltx = v[i].x;
         if (v[i].y < bbox.lty) bbox.lty = v[i].y;
         if (v[i].x > bbox.rbx) bbox.rbx = v[i].x;
         if (v[i].y > bbox.rby) bbox.rby = v[i].y;
      }
      if (preDumpSetup) PSUseArrow();
      if (!TransPat) {
         fprintf(FP, "   %s\n", gPsCmd[PS_NEWPATH]);
         fprintf(FP, "      %1d %1d %.3f %.3f %1d %1d TGAT\n",
               HeadV->x, HeadV->y, daw, dah, dx, dy);
         fprintf(FP, "   1 %s %s %s\n", gPsCmd[PS_SETGRAY],
               gPsCmd[PS_CLOSEPATH], gPsCmd[PS_FILL]);
         DumpRGBColorLine(FP, ColorIndex, 3, TRUE);
      }
   } else {
      switch (Pen) {
      case SOLIDPAT: break;
      case BACKPAT: break;
      default:
         GrayCheck(Pen);
         fprintf(FP, "   %s %s\n", GrayStr(Pen), gPsCmd[PS_SETGRAY]);
         break;
      }
   }
   if (!((colorDump || !useGray) && Pen==BACKPAT)) {
      if (preDumpSetup) PSUseArrow();
      fprintf(FP, "   %s\n", gPsCmd[PS_NEWPATH]);
      fprintf(FP, "      %1d %1d %.3f %.3f %1d %1d TGAT\n",
            HeadV->x, HeadV->y, daw, dah, dx, dy);
   }
   if (colorDump || !useGray) {
      switch (Pen) {
      case SOLIDPAT:
         fprintf(FP, "   %s %s\n", gPsCmd[PS_CLOSEPATH], gPsCmd[PS_FILL]);
         break;
      case BACKPAT: break;
      default:
         if (preDumpSetup) PSUseColorPattern();
         fprintf(FP, "   %s %s %s\n", gPsCmd[PS_CLOSEPATH],
               gPsCmd[PS_EOCLIP], gPsCmd[PS_NEWPATH]);
         DumpPatFill(FP, Pen, bbox, 3, TRUE);
         break;
      }
   } else {
      switch (Pen) {
      case SOLIDPAT:
         fprintf(FP, "   %s %s\n", gPsCmd[PS_CLOSEPATH], gPsCmd[PS_FILL]);
         break;
      case BACKPAT:
         if (!TransPat) {
            fprintf(FP, "   %s 1 %s %s\n", gPsCmd[PS_CLOSEPATH],
                  gPsCmd[PS_SETGRAY], gPsCmd[PS_FILL]);
         }
         break;
      default:
         fprintf(FP, "   %s %s\n", gPsCmd[PS_CLOSEPATH], gPsCmd[PS_FILL]);
         break;
      }
   }
   fprintf(FP, "%s\n", gPsCmd[PS_GRESTORE]);
}

/* --------------------- DumpPolyObj() --------------------- */

static
int CalcArrowRetreatPoint(TailV, HeadV, ArrowW, aw_spec, ReturnV)
   IntPoint *TailV, *HeadV, *ReturnV;
   int ArrowW;
   char *aw_spec;
   /* HeadV is where the arrow tip is */
{
   int dx, dy;
   double len, sin, cos, daw;

   dx = HeadV->x - TailV->x;
   dy = HeadV->y - TailV->y;

   if (dx == 0 && dy == 0) return FALSE;

   len = (double)sqrt((double)(((double)dx)*((double)dx) +
         ((double)dy)*((double)dy)));
   sin = ((double)dy) / len;
   cos = ((double)dx) / len;

   daw = GetWidthInDouble(ArrowW, aw_spec, NULL);
   ReturnV->x = round(HeadV->x-RETREAT*daw*cos);
   ReturnV->y = round(HeadV->y-RETREAT*daw*sin);
   return TRUE;
}

static
void DumpPolyPath(FP, ObjPtr, Vs, NumPts, Smooth, Style, Width, ArrowW, ArrowH,
      width_spec, aw_spec, ah_spec, Pen, Fill, Curved, Dash, Indent, TransPat)
   FILE *FP;
   register struct ObjRec *ObjPtr;
   IntPoint *Vs;
   int NumPts, Style, Width, ArrowW, ArrowH;
   char *width_spec, *aw_spec, *ah_spec;
   int Pen, Fill, Curved, Dash, Indent, TransPat;
   char *Smooth;
{
   register int i, dx, dy;
   int retracted_arrow, color_index=ObjPtr->color, w_is_int=TRUE;
   double daw, dw;

   retracted_arrow = (RetractedArrowAttr(ObjPtr) ||
         AutoRetractedArrowAttr(ObjPtr, TRUE));

   daw = GetWidthInDouble(ArrowW, aw_spec, NULL);
   dw = GetWidthInDouble(Width, width_spec, &w_is_int);
   if (Fill == (-1) && Pen != (-1)) {
      /* dumping the pen */
      if (Dash != 0) {
         for (i=0; i < Indent+3; i++) fprintf(FP, " ");
         fprintf(FP, "[");
         for (i = 0; i < dashListLength[Dash]-1; i++) {
            fprintf(FP, "%1d ", (int)(dashList[Dash][i]));
         }
         fprintf(FP, "%1d] 0 %s\n",
               (int)(dashList[Dash][dashListLength[Dash]-1]),
               gPsCmd[PS_SETDASH]);
      }
   } else if (Fill != (-1) && Pen == (-1)) {
      /* dumping the fill */
      if (Fill > BACKPAT) {
         if (colorDump || !useGray) {
            for (i=0; i < Indent; i++) fprintf(FP, " ");
            fprintf(FP, "%s\n", gPsCmd[PS_GSAVE]);
            if (preDumpSetup) PSUseColorPattern();
            if (!TransPat) {
               DumpPolyPath(FP, ObjPtr, Vs, NumPts, Smooth, LS_PLAIN, Width,
                     ArrowW, ArrowH, width_spec, aw_spec, ah_spec,
                     (-1), BACKPAT, Curved, Dash, Indent, TransPat);
            }
         } else {
            for (i=0; i < Indent; i++) fprintf(FP, " ");
            fprintf(FP, "%s\n", gPsCmd[PS_GSAVE]);
            GrayCheck(Fill);
            for (i=0; i < Indent+3; i++) fprintf(FP, " ");
            fprintf(FP, "%s %s\n", GrayStr(Fill), gPsCmd[PS_SETGRAY]);
         }
      }
   }
#ifndef DONT_DRAW_ZERO_LENGTH_POLY_AS_DOT
   if (NumPts == 2 && Vs[0].x == Vs[1].x && Vs[0].y == Vs[1].y) {
      for (i=0; i < Indent+3; i++) fprintf(FP, " ");
      fprintf(FP, "1 %s\n", gPsCmd[PS_SETLINECAP]);
   }
#endif /* ~DONT_DRAW_ZERO_LENGTH_POLY_AS_DOT */
   for (i=0; i < Indent+3; i++) fprintf(FP, " ");
   fprintf(FP, "%s\n", gPsCmd[PS_NEWPATH]);
   for (i=0; i < Indent+3; i++) fprintf(FP, " ");
   fprintf(FP, "   %1d %1d %s\n", Vs[0].x, Vs[0].y, gPsCmd[PS_MOVETO]);
   if (Style & LS_LEFT) {
      dx = Vs[1].x - Vs[0].x;
      dy = Vs[1].y - Vs[0].y;
      if (dx != 0 || dy != 0) {
         if (!retracted_arrow) {
            if (ObjPtr->ctm == NULL) {
               for (i=0; i < Indent+6; i++) fprintf(FP, " ");
               fprintf(FP, "%1d %1d atan %s cos %.3f %s exch sin %.3f %s %s\n",
                     dy, dx, gPsCmd[PS_DUP], daw, gPsCmd[PS_MUL], daw,
                     gPsCmd[PS_MUL], gPsCmd[PS_RMOVETO]);
            } else {
               IntPoint ip0, ip1, ip2, ip3;

               TransformObjectV(ObjPtr, &Vs[1], &ip0);
               TransformObjectV(ObjPtr, &Vs[0], &ip1);
               if (CalcArrowRetreatPoint(&ip0, &ip1, ArrowW, aw_spec, &ip2)) {
                  ReversedTransformObjectV(ObjPtr, &ip2, &ip3);
                  for (i=0; i < Indent+6; i++) fprintf(FP, " ");
                  fprintf(FP, "%1d %1d %s\n", ip3.x, ip3.y, gPsCmd[PS_MOVETO]);
               }
            }
         }
      }
   }
   if (Style & LS_RIGHT) {
      if (Curved == LT_INTSPLINE) {
         DumpCurvedPolyPoints(FP, Curved, NumPts, Vs, Indent+6);
      } else {
         DumpMultiCurvedPolyPoints(FP, Smooth, Style, Curved, NumPts, Vs,
               Indent+6);
      }
      dx = Vs[NumPts-1].x - Vs[NumPts-2].x;
      dy = Vs[NumPts-1].y - Vs[NumPts-2].y;
      if (dx != 0 || dy != 0) {
         if (retracted_arrow) {
            for (i=0; i < Indent+6; i++) fprintf(FP, " ");
            fprintf(FP, "%1d %1d", Vs[NumPts-1].x, Vs[NumPts-1].y);
         } else {
            if (ObjPtr->ctm == NULL) {
               for (i=0; i < Indent+6; i++) fprintf(FP, " ");
               fprintf(FP, "%1d %1d atan %s cos %.3f %s %1d exch %s\n",
                     dy, dx, gPsCmd[PS_DUP], daw, gPsCmd[PS_MUL],
                     Vs[NumPts-1].x, gPsCmd[PS_SUB]);
               for (i=0; i < Indent+6; i++) fprintf(FP, " ");
               fprintf(FP, "exch sin %.3f %s %1d exch %s",
                     daw, gPsCmd[PS_MUL], Vs[NumPts-1].y, gPsCmd[PS_SUB]);
            } else {
               IntPoint ip0, ip1, ip2, ip3;

               TransformObjectV(ObjPtr, &Vs[NumPts-2], &ip0);
               TransformObjectV(ObjPtr, &Vs[NumPts-1], &ip1);
               if (CalcArrowRetreatPoint(&ip0, &ip1, ArrowW, aw_spec, &ip2)) {
                  ReversedTransformObjectV(ObjPtr, &ip2, &ip3);
                  for (i=0; i < Indent+6; i++) fprintf(FP, " ");
                  fprintf(FP, "%1d %1d", ip3.x, ip3.y);
               }
            }
         }
         switch (Curved) {
         case LT_STRAIGHT:
         case LT_SPLINE:
         case LT_STRUCT_SPLINE:
            if (NumPts <= 2 || (Smooth != NULL && !Smooth[NumPts-2])) {
               fprintf(FP, " %s", gPsCmd[PS_LINETO]);
            } else {
               fprintf(FP, " %s", gPsCmd[PS_CURVETO]);
            }
            break;
         case LT_INTSPLINE:
            if (NumPts <= 2) {
               fprintf(FP, " %s", gPsCmd[PS_LINETO]);
            } else {
               fprintf(FP, " %s", gPsCmd[PS_CURVETO]);
            }
            break;
         }
      }
      fprintf(FP, "\n");
   } else if (Curved == LT_INTSPLINE) {
      DumpCurvedPolyPoints(FP, Curved, NumPts, Vs, Indent+6);
      if (NumPts <= 2) {
         for (i=0; i < Indent+6; i++) fprintf(FP, " ");
         fprintf(FP, "%1d %1d %s\n",Vs[NumPts-1].x,Vs[NumPts-1].y,
               gPsCmd[PS_LINETO]);
      } else {
         for (i=0; i < Indent+6; i++) fprintf(FP, " ");
         fprintf(FP, "%1d %1d %s\n",Vs[NumPts-1].x,Vs[NumPts-1].y,
               gPsCmd[PS_CURVETO]);
      }
   } else {
      DumpMultiCurvedPolyPoints(FP, Smooth, Style, Curved, NumPts, Vs,
            Indent+6);
   }
   if (Fill == (-1) && Pen != (-1)) {
      /* dumping the pen */
      for (i=0; i < Indent+3; i++) fprintf(FP, " ");
      fprintf(FP, "%s\n", &(gPsCmd[PS_TGIFSETMATRIX])[1]);
      for (i=0; i < Indent+3; i++) fprintf(FP, " ");
      if (w_is_int) {
         fprintf(FP, "%1d %s\n", Width, gPsCmd[PS_SETLINEWIDTH]);
      } else {
         fprintf(FP, "%.3f %s\n", dw, gPsCmd[PS_SETLINEWIDTH]);
      }
      for (i=0; i < Indent+3; i++) fprintf(FP, " ");
      switch (Pen) {
      case SOLIDPAT: fprintf(FP, "%s\n", gPsCmd[PS_STROKE]); break;
      case BACKPAT:
         if (!TransPat) {
            fprintf(FP, "1 %s %s\n", gPsCmd[PS_SETGRAY], gPsCmd[PS_STROKE]);
         }
         break;
      default:
         if (colorDump || !useGray) {
            if (preDumpSetup) PSUseColorPattern();
            fprintf(FP, "%s\n", gPsCmd[PS_FLATTENPATH]);
            for (i=0; i < Indent+3; i++) fprintf(FP, " ");
            DumpPatFill(FP, Pen, ObjPtr->bbox, 0, TRUE);
         } else {
            GrayCheck(Pen);
            fprintf(FP, "%s %s\n", GrayStr(Pen), gPsCmd[PS_SETGRAY]);
            for (i=0; i < Indent+3; i++) fprintf(FP, " ");
            fprintf(FP, "%s\n", gPsCmd[PS_STROKE]);
         }
         break;
      }
      if (Dash != 0) {
         for (i=0; i < Indent+3; i++) fprintf(FP, " ");
         fprintf(FP, "[] 0 %s\n", gPsCmd[PS_SETDASH]);
      }
      if (Width != 1) {
         for (i=0; i < Indent+3; i++) fprintf(FP, " ");
         fprintf(FP, "1 %s\n", gPsCmd[PS_SETLINEWIDTH]);
      }
   } else if (Fill != (-1) && Pen == (-1)) {
      /* dumping the fill */
      for (i=0; i < Indent+3; i++) fprintf(FP, " ");
      switch (Fill) {
      case SOLIDPAT:
         fprintf(FP, "%s %s\n", gPsCmd[PS_CLOSEPATH], gPsCmd[PS_EOFILL]);
         break;
      case BACKPAT:
         if (!TransPat) {
            fprintf(FP, "%s 1 %s %s\n", gPsCmd[PS_CLOSEPATH],
                  gPsCmd[PS_SETGRAY], gPsCmd[PS_EOFILL]);
            for (i=0; i < Indent+3; i++) fprintf(FP, " ");
            DumpRGBColorLine(FP, color_index, 0, TRUE);
         }
         break;
      default:
         if (colorDump || !useGray) {
            if (preDumpSetup) PSUseColorPattern();
            fprintf(FP, "%s %s %s\n", gPsCmd[PS_CLOSEPATH],
                  gPsCmd[PS_EOCLIP], gPsCmd[PS_NEWPATH]);
            for (i=0; i < Indent+3; i++) fprintf(FP, " ");
            DumpPatFill(FP, Fill, ObjPtr->bbox, 0, TRUE);
         } else {
            fprintf(FP, "%s %s\n", gPsCmd[PS_CLOSEPATH], gPsCmd[PS_EOFILL]);
         }
         for (i=0; i < Indent; i++) fprintf(FP, " ");
         fprintf(FP, "%s\n", gPsCmd[PS_GRESTORE]);
         break;
      }
   }
}

void DumpPolyObj(FP, ObjPtr)
   FILE *FP;
   register struct ObjRec *ObjPtr;
{
   IntPoint *intv=NULL, *v=NULL;
   int num_pts, trans_pat, fill, pen, width, curved, dash, color_index;
   int style, aw, ah, rotation, intn, retracted_arrow;
   char *smooth=NULL, *width_spec=NULL, *aw_spec=NULL, *ah_spec=NULL;

   trans_pat = ObjPtr->trans_pat;
   fill = ObjPtr->detail.p->fill;
   width = ObjPtr->detail.p->width;
   aw = ObjPtr->detail.p->aw;
   ah = ObjPtr->detail.p->ah;
   width_spec = ObjPtr->detail.p->width_spec;
   aw_spec = ObjPtr->detail.p->aw_spec;
   ah_spec = ObjPtr->detail.p->ah_spec;
   pen = ObjPtr->detail.p->pen;
   style = ObjPtr->detail.p->style;
   curved = ObjPtr->detail.p->curved;
   dash = ObjPtr->detail.p->dash;
   rotation = ObjPtr->rotation;
   if (curved == LT_STRUCT_SPLINE) {
      v = ObjPtr->detail.p->ssvlist;
      num_pts = ObjPtr->detail.p->ssn;
      smooth = ObjPtr->detail.p->ssmooth;
   } else {
      v = ObjPtr->detail.p->vlist;
      num_pts = ObjPtr->detail.p->n;
      smooth = ObjPtr->detail.p->smooth;
   }
   intv = ObjPtr->detail.p->intvlist;
   intn = ObjPtr->detail.p->intn;

   if ((fill == NONEPAT || (trans_pat && fill == BACKPAT)) &&
         (pen == NONEPAT || (trans_pat && pen == BACKPAT))) {
      return;
   }
   fprintf(FP, "%% POLY/OPEN-SPLINE\n");
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
   if (fill != NONEPAT && num_pts > 2 && !(fill == BACKPAT && trans_pat)) {
      if (curved != LT_INTSPLINE) {
         DumpPolyPath(FP, ObjPtr, v, num_pts, smooth, LS_PLAIN, width, aw, ah,
               width_spec, aw_spec, ah_spec, (-1), fill, curved, dash,
               (fill > BACKPAT ? 0 : (-3)), trans_pat);
      } else {
         DumpPolyPath(FP, ObjPtr, intv, intn, smooth, LS_PLAIN, width, aw, ah,
               width_spec, aw_spec, ah_spec, (-1), fill, curved, dash,
               (fill > BACKPAT ? 0 : (-3)), trans_pat);
      }
   }
   if (pen == NONEPAT || (pen == BACKPAT && trans_pat)) {
      if (ObjPtr->ctm != NULL) fprintf(FP, "%s\n", gPsCmd[PS_GRESTORE]);
      fprintf(FP, "\n");
      return;
   }

   fprintf(FP, "%s\n", gPsCmd[PS_GSAVE]);

   if ((colorDump || !useGray) && pen > BACKPAT && !trans_pat) {
      fprintf(FP, "   %s\n", gPsCmd[PS_GSAVE]);
      if (curved != LT_INTSPLINE) {
         DumpPolyPath(FP, ObjPtr, v, num_pts, smooth, style, width, aw, ah,
               width_spec, aw_spec, ah_spec, BACKPAT, (-1), curved, 0, 3,
               trans_pat);
      } else {
         DumpPolyPath(FP, ObjPtr, intv, intn, smooth, style, width, aw, ah,
               width_spec, aw_spec, ah_spec, BACKPAT, (-1), curved, 0, 3,
               trans_pat);
      }
      fprintf(FP, "   %s\n", gPsCmd[PS_GRESTORE]);
   }
   if (curved != LT_INTSPLINE) {
      DumpPolyPath(FP, ObjPtr, v, num_pts, smooth, style, width, aw, ah,
            width_spec, aw_spec, ah_spec, pen, (-1), curved, dash, 0,
            trans_pat);
   } else {
      DumpPolyPath(FP, ObjPtr, intv, intn, smooth, style, width, aw, ah,
            width_spec, aw_spec, ah_spec, pen, (-1), curved, dash, 0,
            trans_pat);
   }
   fprintf(FP, "%s\n", gPsCmd[PS_GRESTORE]);

   retracted_arrow = (RetractedArrowAttr(ObjPtr) ||
         AutoRetractedArrowAttr(ObjPtr, TRUE));

   if (ObjPtr->ctm == NULL) {
      if (curved != LT_INTSPLINE) {
         switch (style) {
         case LS_PLAIN: break;
         case LS_LEFT:
            if (retracted_arrow) {
               DumpArrow(FP, &v[2], &v[1], aw, ah, aw_spec, ah_spec,
                     pen, trans_pat, color_index);
            } else {
               DumpArrow(FP, &v[1], &v[0], aw, ah, aw_spec, ah_spec,
                     pen, trans_pat, color_index);
            }
            break;
         case LS_RIGHT:
            if (retracted_arrow) {
               DumpArrow(FP, &v[num_pts-3], &v[num_pts-2], aw, ah,
                     aw_spec, ah_spec, pen, trans_pat, color_index);
            } else {
               DumpArrow(FP, &v[num_pts-2], &v[num_pts-1], aw, ah,
                     aw_spec, ah_spec, pen, trans_pat, color_index);
            }
            break;
         case LS_DOUBLE:
            if (retracted_arrow) {
               DumpArrow(FP, &v[2], &v[1], aw, ah, aw_spec, ah_spec,
                     pen, trans_pat, color_index);
               DumpArrow(FP, &v[num_pts-3], &v[num_pts-2], aw, ah,
                     aw_spec, ah_spec, pen, trans_pat, color_index);
            } else {
               DumpArrow(FP, &v[1], &v[0], aw, ah, aw_spec, ah_spec,
                     pen, trans_pat, color_index);
               DumpArrow(FP, &v[num_pts-2], &v[num_pts-1], aw, ah,
                     aw_spec, ah_spec, pen, trans_pat, color_index);
            }
            break;
         }
      } else {
         switch (style) {
         case LS_PLAIN: break;
         case LS_LEFT:
            if (retracted_arrow) {
               DumpArrow(FP, &intv[2], &v[1], aw, ah, aw_spec, ah_spec,
                     pen, trans_pat, color_index);
            } else {
               DumpArrow(FP, &intv[1], &intv[0], aw, ah, aw_spec, ah_spec,
                     pen, trans_pat, color_index);
            }
            break;
         case LS_RIGHT:
            if (retracted_arrow) {
               DumpArrow(FP, &intv[intn-3], &v[num_pts-2], aw, ah,
                     aw_spec, ah_spec, pen, trans_pat, color_index);
            } else {
               DumpArrow(FP, &intv[intn-2], &intv[intn-1], aw, ah,
                     aw_spec, ah_spec, pen, trans_pat, color_index);
            }
            break;
         case LS_DOUBLE:
            if (retracted_arrow) {
               DumpArrow(FP, &intv[2], &v[1], aw, ah, aw_spec, ah_spec,
                     pen, trans_pat, color_index);
               DumpArrow(FP, &intv[intn-3], &v[num_pts-2], aw, ah,
                     aw_spec, ah_spec, pen, trans_pat, color_index);
            } else {
               DumpArrow(FP, &intv[1], &intv[0], aw, ah, aw_spec, ah_spec,
                     pen, trans_pat, color_index);
               DumpArrow(FP, &intv[intn-2], &intv[intn-1], aw, ah,
                     aw_spec, ah_spec, pen, trans_pat, color_index);
            }
            break;
         }
      }
   } else {
      IntPoint ip0, ip1;

      if (curved != LT_INTSPLINE) {
         switch (style) {
         case LS_PLAIN: break;
         case LS_LEFT:
            if (retracted_arrow) {
               TransformObjectV(ObjPtr, &v[2], &ip0);
               TransformObjectV(ObjPtr, &v[1], &ip1);
               DumpArrow(FP, &ip0, &ip1, aw, ah, aw_spec, ah_spec,
                     pen, trans_pat, color_index);
            } else {
               TransformObjectV(ObjPtr, &v[1], &ip0);
               TransformObjectV(ObjPtr, &v[0], &ip1);
               DumpArrow(FP, &ip0, &ip1, aw, ah, aw_spec, ah_spec,
                     pen, trans_pat, color_index);
            }
            break;
         case LS_RIGHT:
            if (retracted_arrow) {
               TransformObjectV(ObjPtr, &v[num_pts-3], &ip0);
               TransformObjectV(ObjPtr, &v[num_pts-2], &ip1);
               DumpArrow(FP, &ip0, &ip1, aw, ah, aw_spec, ah_spec,
                     pen, trans_pat, color_index);
            } else {
               TransformObjectV(ObjPtr, &v[num_pts-2], &ip0);
               TransformObjectV(ObjPtr, &v[num_pts-1], &ip1);
               DumpArrow(FP, &ip0, &ip1, aw, ah, aw_spec, ah_spec,
                     pen, trans_pat, color_index);
            }
            break;
         case LS_DOUBLE:
            if (retracted_arrow) {
               TransformObjectV(ObjPtr, &v[2], &ip0);
               TransformObjectV(ObjPtr, &v[1], &ip1);
               DumpArrow(FP, &ip0, &ip1, aw, ah, aw_spec, ah_spec,
                     pen, trans_pat, color_index);
               TransformObjectV(ObjPtr, &v[num_pts-3], &ip0);
               TransformObjectV(ObjPtr, &v[num_pts-2], &ip1);
               DumpArrow(FP, &ip0, &ip1, aw, ah, aw_spec, ah_spec,
                     pen, trans_pat, color_index);
            } else {
               TransformObjectV(ObjPtr, &v[1], &ip0);
               TransformObjectV(ObjPtr, &v[0], &ip1);
               DumpArrow(FP, &ip0, &ip1, aw, ah, aw_spec, ah_spec,
                     pen, trans_pat, color_index);
               TransformObjectV(ObjPtr, &v[num_pts-2], &ip0);
               TransformObjectV(ObjPtr, &v[num_pts-1], &ip1);
               DumpArrow(FP, &ip0, &ip1,aw, ah, aw_spec, ah_spec,
                     pen, trans_pat, color_index);
            }
            break;
         }
      } else {
         switch (style) {
         case LS_PLAIN: break;
         case LS_LEFT:
            if (retracted_arrow) {
               TransformObjectV(ObjPtr, &intv[2], &ip0);
               TransformObjectV(ObjPtr, &v[1], &ip1);
               DumpArrow(FP, &ip0, &ip1, aw, ah, aw_spec, ah_spec,
                     pen, trans_pat, color_index);
            } else {
               TransformObjectV(ObjPtr, &intv[1], &ip0);
               TransformObjectV(ObjPtr, &intv[0], &ip1);
               DumpArrow(FP, &ip0, &ip1, aw, ah, aw_spec, ah_spec,
                     pen, trans_pat, color_index);
            }
            break;
         case LS_RIGHT:
            if (retracted_arrow) {
               TransformObjectV(ObjPtr, &intv[intn-3], &ip0);
               TransformObjectV(ObjPtr, &v[num_pts-2], &ip1);
               DumpArrow(FP, &ip0, &ip1, aw, ah, aw_spec, ah_spec,
                     pen, trans_pat, color_index);
            } else {
               TransformObjectV(ObjPtr, &intv[intn-2], &ip0);
               TransformObjectV(ObjPtr, &intv[intn-1], &ip1);
               DumpArrow(FP, &ip0, &ip1, aw, ah, aw_spec, ah_spec,
                     pen, trans_pat, color_index);
            }
            break;
         case LS_DOUBLE:
            if (retracted_arrow) {
               TransformObjectV(ObjPtr, &intv[2], &ip0);
               TransformObjectV(ObjPtr, &v[1], &ip1);
               DumpArrow(FP, &ip0, &ip1, aw, ah, aw_spec, ah_spec,
                     pen, trans_pat, color_index);
               TransformObjectV(ObjPtr, &intv[intn-3], &ip0);
               TransformObjectV(ObjPtr, &v[num_pts-2], &ip1);
               DumpArrow(FP, &ip0, &ip1, aw, ah, aw_spec, ah_spec,
                     pen, trans_pat, color_index);
            } else {
               TransformObjectV(ObjPtr, &intv[1], &ip0);
               TransformObjectV(ObjPtr, &intv[0], &ip1);
               DumpArrow(FP, &ip0, &ip1, aw, ah, aw_spec, ah_spec,
                     pen, trans_pat, color_index);
               TransformObjectV(ObjPtr, &intv[intn-2], &ip0);
               TransformObjectV(ObjPtr, &intv[intn-1], &ip1);
               DumpArrow(FP, &ip0, &ip1, aw, ah, aw_spec, ah_spec,
                     pen, trans_pat, color_index);
            }
            break;
         }
      }
   }
   if (ObjPtr->ctm != NULL) fprintf(FP, "%s\n", gPsCmd[PS_GRESTORE]);
   fprintf(FP, "\n");
}

/* --------------------- NeedsToCachePolyObj() --------------------- */

int NeedsToCachePolyObj(ObjPtr)
   struct ObjRec *ObjPtr;
{
   return (ObjPtr->ctm != NULL);
}

/* --------------------- DrawPolyObj() --------------------- */

static
void MakeCachedPoly(ObjPtr)
   struct ObjRec *ObjPtr;
{
   int i;
   struct PolyRec *poly_ptr=ObjPtr->detail.p;
   IntPoint *pv=poly_ptr->vlist, *intvs=NULL, *vs=NULL;
   int n=poly_ptr->n, intn=0;

   if (ObjPtr->ctm == NULL) return;
   if (poly_ptr->rotated_vlist != NULL) free(poly_ptr->rotated_vlist);
   if (poly_ptr->rotated_asvlist != NULL) free(poly_ptr->rotated_asvlist);
   poly_ptr->rotated_n = poly_ptr->rotated_asn = 0;
   poly_ptr->rotated_vlist = poly_ptr->rotated_asvlist = NULL;

   if (poly_ptr->curved == LT_STRUCT_SPLINE) {
      pv = poly_ptr->ssvlist;
      n = poly_ptr->ssn;
   }
   vs = (IntPoint*)malloc((n+1)*sizeof(IntPoint));
   if (vs == NULL) FailAllocMessage();
   for (i=0; i < n; i++) {
      int x, y;

      TransformPointThroughCTM(pv[i].x-ObjPtr->x, pv[i].y-ObjPtr->y,
            ObjPtr->ctm, &x, &y);
      vs[i].x = x+ObjPtr->x;
      vs[i].y = y+ObjPtr->y;
   }
   vs[n].x = vs[0].x;
   vs[n].y = vs[0].y;

   switch (poly_ptr->curved) {
   case LT_STRAIGHT:
   case LT_SPLINE:
      poly_ptr->rotated_vlist = MakeMultiSplinePolyVertex(
            poly_ptr->curved, &(poly_ptr->rotated_n), poly_ptr->smooth,
            drawOrigX, drawOrigY, n, vs);
      break;
   case LT_STRUCT_SPLINE:
      poly_ptr->rotated_vlist = MakeMultiSplinePolyVertex(
            poly_ptr->curved, &(poly_ptr->rotated_n), poly_ptr->ssmooth,
            drawOrigX, drawOrigY, n, vs);
      break;
   case LT_INTSPLINE:
      poly_ptr->rotated_vlist = MakeIntSplinePolyVertex(
            &(poly_ptr->rotated_n), &(intn), &(intvs),
            drawOrigX, drawOrigY, n, vs);
      break;
   }
   poly_ptr->rotated_vlist[poly_ptr->rotated_n].x =
         poly_ptr->rotated_vlist[0].x;
   poly_ptr->rotated_vlist[poly_ptr->rotated_n].y =
         poly_ptr->rotated_vlist[0].y;

   if (poly_ptr->style != LS_PLAIN && !RetractedArrowAttr(ObjPtr) &&
         !AutoRetractedArrowAttr(ObjPtr,TRUE)) {
      int aw=poly_ptr->aw, dx, dy;
      double len, sin, cos;

      if (aw == 0) aw = 1;

      switch (poly_ptr->curved) {
      case LT_STRAIGHT:
      case LT_SPLINE:
      case LT_STRUCT_SPLINE:
         dx = vs[1].x - vs[0].x;
         dy = vs[1].y - vs[0].y;
         if ((poly_ptr->style & LS_LEFT) && (dx != 0 || dy != 0)) {
            len = (double)sqrt((double)(((double)dx)*((double)dx) +
                  ((double)dy)*((double)dy)));
            sin = ((double)dy)/len;
            cos = ((double)dx)/len;
            vs[0].x = round(vs[0].x+RETREAT*aw*cos);
            vs[0].y = round(vs[0].y+RETREAT*aw*sin);
         }
         dx = vs[n-1].x - vs[n-2].x;
         dy = vs[n-1].y - vs[n-2].y;
         if ((poly_ptr->style & LS_RIGHT) && (dx != 0 || dy != 0)) {
            len = (double)sqrt((double)(((double)dx)*((double)dx) +
                  ((double)dy)*((double)dy)));
            sin = ((double)dy)/len;
            cos = ((double)dx)/len;
            vs[n-1].x = round(vs[n-1].x-(RETREAT*aw*cos));
            vs[n-1].y = round(vs[n-1].y-(RETREAT*aw*sin));
         }
         break;
      case LT_INTSPLINE:
         dx = intvs[1].x - intvs[0].x;
         dy = intvs[1].y - intvs[0].y;
         if ((poly_ptr->style & LS_LEFT) && (dx != 0 || dy != 0)) {
            len = (double)sqrt((double)(((double)dx)*((double)dx) +
                  ((double)dy)*((double)dy)));
            sin = ((double)dy)/len;
            cos = ((double)dx)/len;
            intvs[0].x = round(intvs[0].x+RETREAT*aw*cos);
            intvs[0].y = round(intvs[0].y+RETREAT*aw*sin);
         }
         dx = intvs[intn-1].x - intvs[intn-2].x;
         dy = intvs[intn-1].y - intvs[intn-2].y;
         if ((poly_ptr->style & LS_RIGHT) && (dx != 0 || dy != 0)) {
            len = (double)sqrt((double)(((double)dx)*((double)dx) +
                  ((double)dy)*((double)dy)));
            sin = ((double)dy)/len;
            cos = ((double)dx)/len;
            intvs[intn-1].x = round(intvs[intn-1].x-(RETREAT*aw*cos));
            intvs[intn-1].y = round(intvs[intn-1].y-(RETREAT*aw*sin));
         }
         break;
      }
   }
   if (poly_ptr->style != LS_PLAIN) {
      switch (poly_ptr->curved) {
      case LT_STRAIGHT:
      case LT_SPLINE:
         poly_ptr->rotated_asvlist = MakeMultiSplinePolyVertex(
               poly_ptr->curved, &(poly_ptr->rotated_asn), poly_ptr->smooth,
               drawOrigX, drawOrigY, n, vs);
         break;
      case LT_STRUCT_SPLINE:
         poly_ptr->rotated_asvlist = MakeMultiSplinePolyVertex(
               poly_ptr->curved, &(poly_ptr->rotated_asn), poly_ptr->ssmooth,
               drawOrigX, drawOrigY, n, vs);
         break;
      case LT_INTSPLINE:
         poly_ptr->rotated_asvlist = MakeSplinePolyVertex(
               0, poly_ptr->curved, &(poly_ptr->rotated_asn),
               drawOrigX, drawOrigY, intn, intvs);
         break;
      }
   }
   free(vs);
   if (intvs != NULL) free(intvs);
}

static
void SetArrowVsForDraw(obj_ptr, retracted_arrow, real_x_off, real_y_off,
      v0, v1, vnminus2, vnminus1)
   struct ObjRec *obj_ptr;
   int retracted_arrow, real_x_off, real_y_off;
   XPoint *v0, *v1, *vnminus2, *vnminus1;
{
   struct PolyRec *poly_ptr=obj_ptr->detail.p;
   int n=poly_ptr->n;

   if (obj_ptr->ctm == NULL) {
      if (poly_ptr->curved != LT_INTSPLINE) {
         IntPoint *vs=poly_ptr->vlist;
   
         if (poly_ptr->curved == LT_STRUCT_SPLINE) {
            n = poly_ptr->ssn;
            vs = poly_ptr->ssvlist;
         }
         if (retracted_arrow) {
            v0->x = ZOOMED_SIZE(vs[1].x-real_x_off);
            v0->y = ZOOMED_SIZE(vs[1].y-real_y_off);
            v1->x = ZOOMED_SIZE(vs[2].x-real_x_off);
            v1->y = ZOOMED_SIZE(vs[2].y-real_y_off);
            vnminus2->x = ZOOMED_SIZE(vs[n-3].x-real_x_off);
            vnminus2->y = ZOOMED_SIZE(vs[n-3].y-real_y_off);
            vnminus1->x = ZOOMED_SIZE(vs[n-2].x-real_x_off);
            vnminus1->y = ZOOMED_SIZE(vs[n-2].y-real_y_off);
         } else {
            v0->x = ZOOMED_SIZE(vs[0].x-real_x_off);
            v0->y = ZOOMED_SIZE(vs[0].y-real_y_off);
            v1->x = ZOOMED_SIZE(vs[1].x-real_x_off);
            v1->y = ZOOMED_SIZE(vs[1].y-real_y_off);
            vnminus2->x = ZOOMED_SIZE(vs[n-2].x-real_x_off);
            vnminus2->y = ZOOMED_SIZE(vs[n-2].y-real_y_off);
            vnminus1->x = ZOOMED_SIZE(vs[n-1].x-real_x_off);
            vnminus1->y = ZOOMED_SIZE(vs[n-1].y-real_y_off);
         }
      } else {
         int intn=poly_ptr->intn;
         IntPoint *intvlist=poly_ptr->intvlist;
   
         if (retracted_arrow) {
            int n=poly_ptr->n;
            IntPoint *vs=poly_ptr->vlist;
   
            v0->x = ZOOMED_SIZE(vs[1].x-real_x_off);
            v0->y = ZOOMED_SIZE(vs[1].y-real_y_off);
            v1->x = ZOOMED_SIZE(intvlist[2].x-real_x_off);
            v1->y = ZOOMED_SIZE(intvlist[2].y-real_y_off);
            vnminus2->x = ZOOMED_SIZE(intvlist[intn-3].x-real_x_off);
            vnminus2->y = ZOOMED_SIZE(intvlist[intn-3].y-real_y_off);
            vnminus1->x = ZOOMED_SIZE(vs[n-2].x-real_x_off);
            vnminus1->y = ZOOMED_SIZE(vs[n-2].y-real_y_off);
         } else {
            v0->x = ZOOMED_SIZE(intvlist[0].x-real_x_off);
            v0->y = ZOOMED_SIZE(intvlist[0].y-real_y_off);
            v1->x = ZOOMED_SIZE(intvlist[1].x-real_x_off);
            v1->y = ZOOMED_SIZE(intvlist[1].y-real_y_off);
            vnminus2->x = ZOOMED_SIZE(intvlist[intn-2].x-real_x_off);
            vnminus2->y = ZOOMED_SIZE(intvlist[intn-2].y-real_y_off);
            vnminus1->x = ZOOMED_SIZE(intvlist[intn-1].x-real_x_off);
            vnminus1->y = ZOOMED_SIZE(intvlist[intn-1].y-real_y_off);
         }
      }
   } else {
      int x, y;

      if (poly_ptr->curved != LT_INTSPLINE) {
         IntPoint *vs=poly_ptr->vlist;
   
         if (poly_ptr->curved == LT_STRUCT_SPLINE) {
            n = poly_ptr->ssn;
            vs = poly_ptr->ssvlist;
         }
         if (retracted_arrow) {
            TransformPointThroughCTM(vs[1].x-obj_ptr->x, vs[1].y-obj_ptr->y,
                  obj_ptr->ctm, &x, &y);
            v0->x = ZOOMED_SIZE(x+obj_ptr->x-real_x_off);
            v0->y = ZOOMED_SIZE(y+obj_ptr->y-real_y_off);
            TransformPointThroughCTM(vs[2].x-obj_ptr->x, vs[2].y-obj_ptr->y,
                  obj_ptr->ctm, &x, &y);
            v1->x = ZOOMED_SIZE(x+obj_ptr->x-real_x_off);
            v1->y = ZOOMED_SIZE(y+obj_ptr->y-real_y_off);
            TransformPointThroughCTM(vs[n-3].x-obj_ptr->x, vs[n-3].y-obj_ptr->y,
                  obj_ptr->ctm, &x, &y);
            vnminus2->x = ZOOMED_SIZE(x+obj_ptr->x-real_x_off);
            vnminus2->y = ZOOMED_SIZE(y+obj_ptr->y-real_y_off);
            TransformPointThroughCTM(vs[n-2].x-obj_ptr->x, vs[n-2].y-obj_ptr->y,
                  obj_ptr->ctm, &x, &y);
            vnminus1->x = ZOOMED_SIZE(x+obj_ptr->x-real_x_off);
            vnminus1->y = ZOOMED_SIZE(y+obj_ptr->y-real_y_off);
         } else {
            TransformPointThroughCTM(vs[0].x-obj_ptr->x, vs[0].y-obj_ptr->y,
                  obj_ptr->ctm, &x, &y);
            v0->x = ZOOMED_SIZE(x+obj_ptr->x-real_x_off);
            v0->y = ZOOMED_SIZE(y+obj_ptr->y-real_y_off);
            TransformPointThroughCTM(vs[1].x-obj_ptr->x, vs[1].y-obj_ptr->y,
                  obj_ptr->ctm, &x, &y);
            v1->x = ZOOMED_SIZE(x+obj_ptr->x-real_x_off);
            v1->y = ZOOMED_SIZE(y+obj_ptr->y-real_y_off);
            TransformPointThroughCTM(vs[n-2].x-obj_ptr->x, vs[n-2].y-obj_ptr->y,
                  obj_ptr->ctm, &x, &y);
            vnminus2->x = ZOOMED_SIZE(x+obj_ptr->x-real_x_off);
            vnminus2->y = ZOOMED_SIZE(y+obj_ptr->y-real_y_off);
            TransformPointThroughCTM(vs[n-1].x-obj_ptr->x, vs[n-1].y-obj_ptr->y,
                  obj_ptr->ctm, &x, &y);
            vnminus1->x = ZOOMED_SIZE(x+obj_ptr->x-real_x_off);
            vnminus1->y = ZOOMED_SIZE(y+obj_ptr->y-real_y_off);
         }
      } else {
         int intn=poly_ptr->intn;
         IntPoint *intvlist=poly_ptr->intvlist;
   
         if (retracted_arrow) {
            int n=poly_ptr->n;
            IntPoint *vs=poly_ptr->vlist;

            TransformPointThroughCTM(vs[1].x-obj_ptr->x, vs[1].y-obj_ptr->y,
                  obj_ptr->ctm, &x, &y);
            v0->x = ZOOMED_SIZE(x+obj_ptr->x-real_x_off);
            v0->y = ZOOMED_SIZE(y+obj_ptr->y-real_y_off);
            TransformPointThroughCTM(intvlist[2].x-obj_ptr->x,
                  intvlist[2].y-obj_ptr->y, obj_ptr->ctm, &x, &y);
            v1->x = ZOOMED_SIZE(x+obj_ptr->x-real_x_off);
            v1->y = ZOOMED_SIZE(y+obj_ptr->y-real_y_off);
            TransformPointThroughCTM(intvlist[intn-3].x-obj_ptr->x,
                  intvlist[intn-3].y-obj_ptr->y, obj_ptr->ctm, &x, &y);
            vnminus2->x = ZOOMED_SIZE(x+obj_ptr->x-real_x_off);
            vnminus2->y = ZOOMED_SIZE(y+obj_ptr->y-real_y_off);
            TransformPointThroughCTM(vs[n-2].x-obj_ptr->x, vs[n-2].y-obj_ptr->y,
                  obj_ptr->ctm, &x, &y);
            vnminus1->x = ZOOMED_SIZE(x+obj_ptr->x-real_x_off);
            vnminus1->y = ZOOMED_SIZE(y+obj_ptr->y-real_y_off);
         } else {
            TransformPointThroughCTM(intvlist[0].x-obj_ptr->x,
                  intvlist[0].y-obj_ptr->y, obj_ptr->ctm, &x, &y);
            v0->x = ZOOMED_SIZE(x+obj_ptr->x-real_x_off);
            v0->y = ZOOMED_SIZE(y+obj_ptr->y-real_y_off);
            TransformPointThroughCTM(intvlist[1].x-obj_ptr->x,
                  intvlist[1].y-obj_ptr->y, obj_ptr->ctm, &x, &y);
            v1->x = ZOOMED_SIZE(x+obj_ptr->x-real_x_off);
            v1->y = ZOOMED_SIZE(y+obj_ptr->y-real_y_off);
            TransformPointThroughCTM(intvlist[intn-2].x-obj_ptr->x,
                  intvlist[intn-2].y-obj_ptr->y, obj_ptr->ctm, &x, &y);
            vnminus2->x = ZOOMED_SIZE(x+obj_ptr->x-real_x_off);
            vnminus2->y = ZOOMED_SIZE(y+obj_ptr->y-real_y_off);
            TransformPointThroughCTM(intvlist[intn-1].x-obj_ptr->x,
                  intvlist[intn-1].y-obj_ptr->y, obj_ptr->ctm, &x, &y);
            vnminus1->x = ZOOMED_SIZE(x+obj_ptr->x-real_x_off);
            vnminus1->y = ZOOMED_SIZE(y+obj_ptr->y-real_y_off);
         }
      }
   }
}

void DrawPolyObj(Win, XOff, YOff, ObjPtr)
   Window Win;
   int XOff, YOff;
   struct ObjRec *ObjPtr;
{
   struct PolyRec *poly_ptr=ObjPtr->detail.p;
   XPoint *v, tmp_v[4];
   XPoint v0, v1, vnminus2, vnminus1;
   int trans_pat, pen, width, pixel, fill, n, dash, real_x_off, real_y_off;
   int style, aw, ah, num_pts, left_dx, left_dy, right_dx, right_dy;
   int retracted_arrow=FALSE;
   short tmps;
   double len, sin, cos;
   XGCValues values;

   trans_pat = ObjPtr->trans_pat;
   n = poly_ptr->n;
   fill = poly_ptr->fill;
   width = poly_ptr->width;
   aw = poly_ptr->aw;
   ah = poly_ptr->ah;
   pen = poly_ptr->pen;
   style = poly_ptr->style;
   dash = poly_ptr->dash;
   pixel = colorPixels[ObjPtr->color];

   if (NeedsToCachePolyObj(ObjPtr) && poly_ptr->rotated_vlist==NULL) {
      MakeCachedPoly(ObjPtr);
   }
   if (userDisableRedraw) return;

   if ((fill == NONEPAT || (trans_pat && fill == BACKPAT)) &&
         (pen == NONEPAT || (trans_pat && pen == BACKPAT))) {
      return;
   }
   real_x_off = (zoomedIn ? XOff : (XOff>>zoomScale)<<zoomScale);
   real_y_off = (zoomedIn ? YOff : (YOff>>zoomScale)<<zoomScale);

   v = poly_ptr->svlist;
   num_pts = poly_ptr->sn;

   if (v != NULL) {
      v[num_pts].x = v[0].x;
      v[num_pts].y = v[0].y;
   }
   if (fill != NONEPAT) {
      values.foreground = GetDrawingBgPixel(fill, pixel);
      values.function = GXcopy;
      values.fill_style = (trans_pat ? FillStippled : FillOpaqueStippled);
      values.stipple = patPixmap[fill];
      XChangeGC(mainDisplay, drawGC,
            GCForeground | GCFunction | GCFillStyle | GCStipple, &values);
      if (ObjPtr->ctm == NULL) {
         XFillPolygon(mainDisplay, Win, drawGC, v, num_pts+1, Complex,
               CoordModeOrigin);
      } else {
         XFillPolygon(mainDisplay, Win, drawGC, poly_ptr->rotated_vlist,
               poly_ptr->rotated_n+1, Complex, CoordModeOrigin);
      }
   }

   if (pen == NONEPAT || (pen == BACKPAT && trans_pat)) return;

   retracted_arrow = (RetractedArrowAttr(ObjPtr) ||
         AutoRetractedArrowAttr(ObjPtr, TRUE));

   SetArrowVsForDraw(ObjPtr, retracted_arrow, real_x_off, real_y_off,
         &v0, &v1, &vnminus2, &vnminus1);

   aw = ZOOMED_SIZE(aw); if (aw == 0) aw = 1;
   ah = ZOOMED_SIZE(ah); if (ah == 0) ah = 1;

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
         GCForeground | GCFunction | GCFillStyle | GCStipple | GCLineWidth |
         GCLineStyle | GCJoinStyle, &values);

   left_dx = v1.x - v0.x;
   left_dy = v1.y - v0.y;

   if ((style & LS_LEFT) && (left_dx != 0 || left_dy != 0)) {
      /* adjust the first point */
      len = (double)sqrt((double)(((double)left_dx)*((double)left_dx) +
            ((double)left_dy)*((double)left_dy)));
      sin = ((double)left_dy)/len;
      cos = ((double)left_dx)/len;

      tmp_v[0].x = tmp_v[3].x = v0.x;
      tmp_v[0].y = tmp_v[3].y = v0.y;
      tmps = round(v0.x + aw*cos - ah*sin); tmp_v[1].x = tmps;
      tmps = round(v0.y + aw*sin + ah*cos); tmp_v[1].y = tmps;
      tmps = round(v0.x + aw*cos + ah*sin); tmp_v[2].x = tmps;
      tmps = round(v0.y + aw*sin - ah*cos); tmp_v[2].y = tmps;

      XFillPolygon(mainDisplay, Win, drawGC, tmp_v, 4, Convex,
            CoordModeOrigin);
   }
   right_dx = vnminus1.x - vnminus2.x;
   right_dy = vnminus1.y - vnminus2.y;

   if ((style & LS_RIGHT) && (right_dx != 0 || right_dy != 0)) {
      /* adjust the last point */
      len = (double)sqrt((double)(((double)right_dx)*((double)right_dx) +
            ((double)right_dy)*((double)right_dy)));
      sin = ((double)right_dy)/len;
      cos = ((double)right_dx)/len;

      tmp_v[0].x = tmp_v[3].x = vnminus1.x;
      tmp_v[0].y = tmp_v[3].y = vnminus1.y;
      tmps = round(vnminus1.x - aw*cos + ah*sin); tmp_v[1].x = tmps;
      tmps = round(vnminus1.y - aw*sin - ah*cos); tmp_v[1].y = tmps;
      tmps = round(vnminus1.x - aw*cos - ah*sin); tmp_v[2].x = tmps;
      tmps = round(vnminus1.y - aw*sin + ah*cos); tmp_v[2].y = tmps;

      XFillPolygon(mainDisplay, Win, drawGC, tmp_v, 4, Convex,
            CoordModeOrigin);
   }
   if (style != LS_PLAIN) {
      if (ObjPtr->ctm == NULL) {
         MyDrawLines(mainDisplay, Win, drawGC, poly_ptr->asvlist,
               poly_ptr->asn);
      } else {
         MyDrawLines(mainDisplay, Win, drawGC, poly_ptr->rotated_asvlist,
               poly_ptr->rotated_asn);
      }
   } else {
      int draw_zero_length_poly=FALSE;

#ifndef DONT_DRAW_ZERO_LENGTH_POLY_AS_DOT
      if (ObjPtr->ctm == NULL) {
         if (num_pts == 2 && v[0].x == v[1].x && v[0].y == v[1].y) {
            draw_zero_length_poly = TRUE;
         }
      } else {
         if (poly_ptr->rotated_n == 2 &&
               poly_ptr->rotated_vlist[0].x == poly_ptr->rotated_vlist[1].x &&
               poly_ptr->rotated_vlist[0].y == poly_ptr->rotated_vlist[1].y) {
            draw_zero_length_poly = TRUE;
         }
      }
#endif /* ~DONT_DRAW_ZERO_LENGTH_POLY_AS_DOT */
      if (ObjPtr->ctm == NULL) {
         if (draw_zero_length_poly) {
            values.cap_style = CapRound;
            XChangeGC(mainDisplay, drawGC, GCCapStyle, &values);
         }
         MyDrawLines(mainDisplay, Win, drawGC, v, num_pts);
         if (draw_zero_length_poly) {
            values.cap_style = CapButt;
            XChangeGC(mainDisplay, drawGC, GCCapStyle, &values);
         }
      } else {
         if (draw_zero_length_poly) {
            values.cap_style = CapRound;
            XChangeGC(mainDisplay, drawGC, GCCapStyle, &values);
         }
         MyDrawLines(mainDisplay, Win, drawGC, poly_ptr->rotated_vlist,
               poly_ptr->rotated_n);
         if (draw_zero_length_poly) {
            values.cap_style = CapButt;
            XChangeGC(mainDisplay, drawGC, GCCapStyle, &values);
         }
      }
   }
   values.join_style = JoinMiter;
   XChangeGC(mainDisplay, drawGC, GCJoinStyle, &values);
}

/* --------------------- SaveSmoothHinge() --------------------- */

static char hexValue[]="0123456789abcdef";

void SaveSmoothHinge(FP, Curved, NumPts, Smooth)
   FILE *FP;
   int Curved, NumPts;
   char *Smooth;
{
   register int nibble_count=0, bit_count=0, data=0, j;

   if (Curved == LT_INTSPLINE || Smooth == NULL) return;

   for (j=0; j < NumPts; j++) {
      data = (Smooth[j] ? (data<<1) | 1 : (data<<1));

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
   if ((NumPts & 0x3) != 0) {
      data <<= (4 - (NumPts & 0x3));
      if (nibble_count++ == 64) {
         nibble_count = 1;
         if (fprintf(FP, "\n     ") == EOF) writeFileFailed = TRUE;
      }
      if (fprintf(FP, "%c", hexValue[data]) == EOF) writeFileFailed = TRUE;
   }
}

/* --------------------- SavePolyObj() --------------------- */

static
void SaveArrowSpec(FP, arrow_style, aw, ah, aindent, aw_spec, ah_spec,
      aindent_spec)
   FILE *FP;
   int arrow_style, aw, ah, aindent;
   char *aw_spec, *ah_spec, *aindent_spec;
{
   if (fprintf(FP, "%1d,%1d,%1d,%1d,'%s','%s','%s'",
         arrow_style, aw, ah, aindent, aw_spec, ah_spec, aindent_spec) == EOF) {
      writeFileFailed = TRUE;
   }
}

void SavePolyObj(FP, ObjPtr)
   FILE *FP;
   struct ObjRec *ObjPtr;
{
   register int i, n;
   int count, line_cap=LCAP_BUTT;
   struct PolyRec *poly_ptr=ObjPtr->detail.p;

   n = poly_ptr->n;
   if (fprintf(FP, "poly('%s','',%1d,[\n\t",
         colorMenuItems[ObjPtr->color], poly_ptr->n) == EOF) {
      writeFileFailed = TRUE;
   }
   for (i=0, count = 0; i < n-1; i++) {
      if (fprintf(FP, "%1d,%1d,", poly_ptr->vlist[i].x,
            poly_ptr->vlist[i].y) == EOF) {
         writeFileFailed = TRUE;
      }
      if (++count == 8) {
         count = 0;
         if (fprintf(FP, "\n\t") == EOF) writeFileFailed = TRUE;
      }
   }
   if (fprintf(FP, "%1d,%1d],", poly_ptr->vlist[n-1].x,
         poly_ptr->vlist[n-1].y) == EOF) {
      writeFileFailed = TRUE;
   }
   if (fprintf(FP,
         "%1d,%1d,%1d,%1d,%1d,%1d,%1d,%1d,%1d,%1d,%1d,'%s',%1d,%1d,",
         poly_ptr->style, poly_ptr->width, poly_ptr->pen, ObjPtr->id,
         poly_ptr->curved, poly_ptr->fill, poly_ptr->dash, ObjPtr->rotation,
         ObjPtr->locked, ObjPtr->ctm!=NULL, ObjPtr->invisible,
         poly_ptr->width_spec, ObjPtr->trans_pat, line_cap) == EOF) {
      writeFileFailed = TRUE;
   }
   if (fprintf(FP, "\n    \"") == EOF) writeFileFailed = TRUE;
   SaveSmoothHinge(FP, poly_ptr->curved, poly_ptr->n, poly_ptr->smooth);
   if (fprintf(FP, "\",") == EOF) writeFileFailed = TRUE;

   /* save an empty spline tension spec */
   if (fprintf(FP, "\"\",") == EOF) writeFileFailed = TRUE;
   /* save the left arrow spec */
   if (fprintf(FP, "[\n    ") == EOF) writeFileFailed = TRUE;
   SaveArrowSpec(FP, ASTY_COMPAT, poly_ptr->aw, poly_ptr->ah, 0,
         poly_ptr->aw_spec, poly_ptr->ah_spec, "0");
   if (fprintf(FP, "],[") == EOF) writeFileFailed = TRUE;
   /* save the right arrow spec */
   SaveArrowSpec(FP, ASTY_COMPAT, poly_ptr->aw, poly_ptr->ah, 0,
         poly_ptr->aw_spec, poly_ptr->ah_spec, "0");
   if (fprintf(FP, "],") == EOF) writeFileFailed = TRUE;

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

/* --------------------- ReadSmoothHinge() --------------------- */

int ReadSmoothHinge(FP, Curved, NumPts, Smooth)
   FILE *FP;
   int Curved, NumPts;
   char *Smooth;
{
   int num_nibbles=NumPts>>2, nibble_count=0, bit_count=0, j, k;
   char *c_ptr, inbuf[MAXSTRING+1];

   if ((NumPts & 0x3) != 0) num_nibbles++;
   (void)fgets(inbuf, MAXSTRING, FP);
   scanLineNum++;
   if (Curved == LT_INTSPLINE || Smooth == NULL) return TRUE;
   if ((c_ptr=strchr(inbuf, '"')) == NULL) {
      (void)sprintf(gszMsgBox, TgLoadString(STID_INVALID_SMOOTHHINGE_SPEC_POLY),
            scanFileName, scanLineNum);
      if (PRTGIF) {
         fprintf(stderr, "%s\n", gszMsgBox);
      } else {
         Msg(gszMsgBox);
      }
      return FALSE;
   }
   c_ptr++;
   for (j=0; *c_ptr != '"' && j < num_nibbles; j++) {
      int data=0;

      if (nibble_count++ == 64) {
         (void)fgets(inbuf, MAXSTRING, FP);
         scanLineNum++;
         for (c_ptr=inbuf; *c_ptr == ' '; c_ptr++) ;
         nibble_count = 1;
      }
      if (*c_ptr >= '0' && *c_ptr <= '9') {
         data = (int)(*c_ptr++) - (int)('0');
      } else if (*c_ptr >= 'a' && *c_ptr <= 'f') {
         data = (int)(*c_ptr++) - (int)('a') + 10;
      }
      for (k = 0; k < 4; k++) {
         if (bit_count++ == NumPts) break;

         Smooth[(j<<2)+k] = (data & (1<<(3-k)) ? TRUE : FALSE);
      }
   }
   /*
    * Should be reading spline tension here.  But this is not implemented, yet.
    */
   return TRUE;
}

/* --------------------- ReadPolyObj() --------------------- */

void ReadPolyObj(FP, Inbuf, ObjPtr)
   FILE *FP;
   char *Inbuf;
   struct ObjRec **ObjPtr;
{
   register int i;
   struct PolyRec *poly_ptr;
   IntPoint *v;
   char color_str[40], bg_color_str[40], *s, inbuf[MAXSTRING+1];
   char width_spec[40], aw_spec[40], ah_spec[40];
   int num_pts, ltx=0, lty=0, rbx=0, rby=0, x, y, id=0;
   int initialized, rotation, count, new_alloc, line_cap=LCAP_BUTT;
   int style, width=0, pen, curved, fill, dash, locked=FALSE, trans_pat=FALSE;
   int aw=origArrowHeadW[6], ah=origArrowHeadH[6], arrow_style=ASTY_COMPAT;
   char *smooth=NULL;
   int real_x=0, real_y=0, transformed=FALSE, invisible=FALSE;
   struct XfrmMtrxRec *ctm=NULL;
   struct BBRec orig_obbox;

   *ObjPtr = NULL;

   s = FindChar((int)'(', Inbuf);
   s = ParseStr(s, (int)',', color_str, sizeof(color_str));
   if (fileVersion >= 37) {
      s = ParseStr(s, (int)',', bg_color_str, sizeof(bg_color_str));
   }
   InitScan(s, "\t\n, []");

   if (GETINT("poly", num_pts, "number of points") == INVALID) {
      return;
   }
   if (num_pts <= 0) {
      (void)sprintf(gszMsgBox, TgLoadString(STID_INVALID_NUM_PTS_IN_POLY),
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
   poly_ptr = (struct PolyRec *)malloc(sizeof(struct PolyRec));
   if (poly_ptr == NULL) FailAllocMessage();
   memset(poly_ptr, 0, sizeof(struct PolyRec));

   if (num_pts == 1) {
      v = (IntPoint*)malloc(4*sizeof(IntPoint));
      if (v == NULL) FailAllocMessage();
      memset(v, 0, 4*sizeof(IntPoint));
      smooth = (char*)malloc(4*sizeof(char));
      if (smooth == NULL) FailAllocMessage();
      memset(smooth, 0, 4*sizeof(char));
   } else {
      v = (IntPoint*)malloc((num_pts+1)*sizeof(IntPoint));
      if (v == NULL) FailAllocMessage();
      memset(v, 0, (num_pts+1)*sizeof(IntPoint));
      smooth = (char*)malloc((num_pts+1)*sizeof(char));
      if (smooth == NULL) FailAllocMessage();
      memset(smooth, 0, (num_pts+1)*sizeof(char));
   }

   initialized = FALSE;

   *width_spec = *aw_spec = *ah_spec = '\0';
   if (fileVersion <= 13) {
      for (i=0; i < num_pts; i++) {
         if (GETINT("poly", x, "x") == INVALID ||
             GETINT("poly", y, "y") == INVALID) {
            free(*ObjPtr);
            free(poly_ptr);
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
         if (GETINT("poly", x, "x") == INVALID ||
             GETINT("poly", y, "y") == INVALID) {
            free(*ObjPtr);
            free(poly_ptr);
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

   if (num_pts == 1) {
      sprintf(gszMsgBox, TgLoadCachedString(CSTID_SINGLE_PT_POLY_CONVERTED),
            v[0].x, v[0].y);
      if (PRTGIF) {
         fprintf(stderr, "%s\n", gszMsgBox);
      } else {
         Msg(gszMsgBox);
      }
      v[1].x = v[0].x;
      v[1].y = v[0].y;
      num_pts = 2;
   }

   poly_ptr->n = num_pts;

   dash = 0;
   rotation = 0;
   if (fileVersion == INVALID) {
      if (GETINT("poly", style,    "style") == INVALID ||
          GETINT("poly", width,    "width") == INVALID ||
          GETINT("poly", pen,      "pen") == INVALID) {
         free(*ObjPtr);
         free(poly_ptr);
         free(v);
         *ObjPtr = NULL;
         return;
      }
      id = objId++;
      fill = NONEPAT;
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
   } else if (fileVersion <= 3) {
      if (GETINT("poly", style,    "style") == INVALID ||
          GETINT("poly", width,    "width") == INVALID ||
          GETINT("poly", pen,      "pen") == INVALID ||
          GETINT("poly", id,       "id") == INVALID) {
         free(*ObjPtr);
         free(poly_ptr);
         free(v);
         *ObjPtr = NULL;
         return;
      }
      if (id >= objId) objId = id+1;
      fill = NONEPAT;
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
   } else if (fileVersion <= 4) {
      if (GETINT("poly", style,    "style") == INVALID ||
          GETINT("poly", width,    "width") == INVALID ||
          GETINT("poly", pen,      "pen") == INVALID ||
          GETINT("poly", id,       "id") == INVALID ||
          GETINT("poly", curved,   "curved") == INVALID) {
         free(*ObjPtr);
         free(poly_ptr);
         free(v);
         *ObjPtr = NULL;
         return;
      }
      if (id >= objId) objId = id+1;
      fill = NONEPAT;
      switch (width) {
      case 1: width = 3; break;
      case 2: width = 6; break;
      }
   } else if (fileVersion <= 5) {
      if (GETINT("poly", style,    "style") == INVALID ||
          GETINT("poly", width,    "width") == INVALID ||
          GETINT("poly", pen,      "pen") == INVALID ||
          GETINT("poly", id,       "id") == INVALID ||
          GETINT("poly", curved,   "curved") == INVALID ||
          GETINT("poly", fill,     "fill") == INVALID) {
         free(*ObjPtr);
         free(poly_ptr);
         free(v);
         *ObjPtr = NULL;
         return;
      }
      if (id >= objId) objId = id+1;
      switch (width) {
      case 1: width = 3; break;
      case 2: width = 6; break;
      }
   } else if (fileVersion <= 8) {
      if (GETINT("poly", style,    "style") == INVALID ||
          GETINT("poly", width,    "width") == INVALID ||
          GETINT("poly", pen,      "pen") == INVALID ||
          GETINT("poly", id,       "id") == INVALID ||
          GETINT("poly", curved,   "curved") == INVALID ||
          GETINT("poly", fill,     "fill") == INVALID) {
         free(*ObjPtr);
         free(poly_ptr);
         free(v);
         *ObjPtr = NULL;
         return;
      }
      if (id >= objId) objId = id+1;
   } else if (fileVersion <= 13) {
      if (GETINT("poly", style,    "style") == INVALID ||
          GETINT("poly", width,    "width") == INVALID ||
          GETINT("poly", pen,      "pen") == INVALID ||
          GETINT("poly", id,       "id") == INVALID ||
          GETINT("poly", curved,   "curved") == INVALID ||
          GETINT("poly", fill,     "fill") == INVALID ||
          GETINT("poly", dash,     "dash") == INVALID) {
         free(*ObjPtr);
         free(poly_ptr);
         free(v);
         *ObjPtr = NULL;
         return;
      }
      if (id >= objId) objId = id+1;
   } else if (fileVersion <= 16) {
      if (GETINT("poly", style,    "style") == INVALID ||
          GETINT("poly", width,    "width") == INVALID ||
          GETINT("poly", pen,      "pen") == INVALID ||
          GETINT("poly", id,       "id") == INVALID ||
          GETINT("poly", curved,   "curved") == INVALID ||
          GETINT("poly", fill,     "fill") == INVALID ||
          GETINT("poly", dash,     "dash") == INVALID ||
          GETINT("poly", rotation, "rotation") == INVALID) {
         free(*ObjPtr);
         free(poly_ptr);
         free(v);
         *ObjPtr = NULL;
         return;
      }
      if (id >= objId) objId = id+1;
   } else if (fileVersion <= 25) {
      if (GETINT("poly", style,    "style") == INVALID ||
          GETINT("poly", width,    "width") == INVALID ||
          GETINT("poly", pen,      "pen") == INVALID ||
          GETINT("poly", id,       "id") == INVALID ||
          GETINT("poly", curved,   "curved") == INVALID ||
          GETINT("poly", fill,     "fill") == INVALID ||
          GETINT("poly", dash,     "dash") == INVALID ||
          GETINT("poly", rotation, "rotation") == INVALID ||
          GETINT("poly", aw,       "arrow head width") == INVALID ||
          GETINT("poly", ah,       "arrow head height") == INVALID) {
         free(*ObjPtr);
         free(poly_ptr);
         free(v);
         *ObjPtr = NULL;
         return;
      }
      if (id >= objId) objId = id+1;
   } else if (fileVersion <= 32) {
      if (GETINT("poly", style,    "style") == INVALID ||
          GETINT("poly", width,    "width") == INVALID ||
          GETINT("poly", pen,      "pen") == INVALID ||
          GETINT("poly", id,       "id") == INVALID ||
          GETINT("poly", curved,   "curved") == INVALID ||
          GETINT("poly", fill,     "fill") == INVALID ||
          GETINT("poly", dash,     "dash") == INVALID ||
          GETINT("poly", rotation, "rotation") == INVALID ||
          GETINT("poly", aw,       "arrow head width") == INVALID ||
          GETINT("poly", ah,       "arrow head height") == INVALID ||
          GETINT("poly", locked,   "locked") == INVALID) {
         free(*ObjPtr);
         free(poly_ptr);
         free(v);
         *ObjPtr = NULL;
         return;
      }
      if (id >= objId) objId = id+1;
   } else if (fileVersion <= 34) {
      if (GETINT("poly", style,       "style") == INVALID ||
          GETINT("poly", width,       "width") == INVALID ||
          GETINT("poly", pen,         "pen") == INVALID ||
          GETINT("poly", id,          "id") == INVALID ||
          GETINT("poly", curved,      "curved") == INVALID ||
          GETINT("poly", fill,        "fill") == INVALID ||
          GETINT("poly", dash,        "dash") == INVALID ||
          GETINT("poly", rotation,    "rotation") == INVALID ||
          GETINT("poly", aw,          "arrow head width") == INVALID ||
          GETINT("poly", ah,          "arrow head height") == INVALID ||
          GETINT("poly", locked,      "locked") == INVALID ||
          GETINT("poly", transformed, "transformed") == INVALID ||
          GETINT("poly", invisible,   "invisible") == INVALID ||
          GETSTR("poly", width_spec,  "width_spec") == INVALID ||
          GETSTR("poly", aw_spec,     "aw_spec") == INVALID ||
          GETSTR("poly", ah_spec,     "ah_spec") == INVALID) {
         free(*ObjPtr);
         free(poly_ptr);
         free(v);
         *ObjPtr = NULL;
         return;
      }
      if (id >= objId) objId = id+1;
      UtilRemoveQuotes(width_spec);
      UtilRemoveQuotes(aw_spec);
      UtilRemoveQuotes(ah_spec);
   } else if (fileVersion <= 36) {
      if (GETINT("poly", style,       "style") == INVALID ||
          GETINT("poly", width,       "width") == INVALID ||
          GETINT("poly", pen,         "pen") == INVALID ||
          GETINT("poly", id,          "id") == INVALID ||
          GETINT("poly", curved,      "curved") == INVALID ||
          GETINT("poly", fill,        "fill") == INVALID ||
          GETINT("poly", dash,        "dash") == INVALID ||
          GETINT("poly", rotation,    "rotation") == INVALID ||
          GETINT("poly", aw,          "arrow head width") == INVALID ||
          GETINT("poly", ah,          "arrow head height") == INVALID ||
          GETINT("poly", locked,      "locked") == INVALID ||
          GETINT("poly", transformed, "transformed") == INVALID ||
          GETINT("poly", invisible,   "invisible") == INVALID ||
          GETSTR("poly", width_spec,  "width_spec") == INVALID ||
          GETSTR("poly", aw_spec,     "aw_spec") == INVALID ||
          GETSTR("poly", ah_spec,     "ah_spec") == INVALID ||
          GETINT("poly", trans_pat,   "trans_pat") == INVALID) {
         free(*ObjPtr);
         free(poly_ptr);
         free(v);
         *ObjPtr = NULL;
         return;
      }
      if (id >= objId) objId = id+1;
      UtilRemoveQuotes(width_spec);
      UtilRemoveQuotes(aw_spec);
      UtilRemoveQuotes(ah_spec);
   } else {
      if (GETINT("poly", style,       "style") == INVALID ||
          GETINT("poly", width,       "width") == INVALID ||
          GETINT("poly", pen,         "pen") == INVALID ||
          GETINT("poly", id,          "id") == INVALID ||
          GETINT("poly", curved,      "curved") == INVALID ||
          GETINT("poly", fill,        "fill") == INVALID ||
          GETINT("poly", dash,        "dash") == INVALID ||
          GETINT("poly", rotation,    "rotation") == INVALID ||
          GETINT("poly", locked,      "locked") == INVALID ||
          GETINT("poly", transformed, "transformed") == INVALID ||
          GETINT("poly", invisible,   "invisible") == INVALID ||
          GETSTR("poly", width_spec,  "width_spec") == INVALID ||
          GETINT("poly", trans_pat,   "trans_pat") == INVALID ||
          GETINT("poly", line_cap,    "line_cap") == INVALID) {
         free(*ObjPtr);
         free(poly_ptr);
         free(v);
         *ObjPtr = NULL;
         return;
      }
      if (id >= objId) objId = id+1;
      UtilRemoveQuotes(width_spec);
      UtilRemoveQuotes(aw_spec);
      UtilRemoveQuotes(ah_spec);
      if (line_cap != LCAP_BUTT) {
         /* does not support other linecaps, yet */
         line_cap = LCAP_BUTT;
      }
   }

   if (fileVersion <= 16 && width <= 6) {
      aw = origArrowHeadW[width];
      ah = origArrowHeadH[width];
      width = origWidthOfLine[width];
   }
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
         smooth[0] = smooth[num_pts-1] = FALSE;
         for (i=1; i < num_pts-1; i++) smooth[i] = TRUE;
         break;
      }
   } else if (!ReadSmoothHinge(FP, curved, num_pts, smooth)) {
      free(*ObjPtr);
      free(poly_ptr);
      free(v);
      *ObjPtr = NULL;
      return;
   }
   if (smooth != NULL && (smooth[0] != FALSE || smooth[num_pts-1] != FALSE)) {
      smooth[0] = smooth[num_pts-1] = FALSE;
      SetFileModified(TRUE);

      sprintf(gszMsgBox, TgLoadCachedString(CSTID_BAD_SMOOTHHINGE_POLY_FIXED));
      if (PRTGIF) {
         fprintf(stderr, "%s\n", gszMsgBox);
      } else {
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      }
   }
   if (fileVersion >= 37) {
      int aindent=0;
      char aindent_spec[40];

      (void)fgets(inbuf, MAXSTRING, FP);
      scanLineNum++;
      InitScan(inbuf, "\t\n, []");

      /* for now, they both spec are identical */
      if (GETINT("poly", arrow_style,  "arrow_style") == INVALID ||
          GETINT("poly", aw,           "aw") == INVALID ||
          GETINT("poly", ah,           "ah") == INVALID ||
          GETINT("poly", aindent,      "aindent") == INVALID ||
          GETSTR("poly", aw_spec,      "aw_spec") == INVALID ||
          GETSTR("poly", ah_spec,      "ah_spec") == INVALID ||
          GETSTR("poly", aindent_spec, "aindent_spec") == INVALID ||
          GETINT("poly", arrow_style,  "arrow_style") == INVALID ||
          GETINT("poly", aw,           "aw") == INVALID ||
          GETINT("poly", ah,           "ah") == INVALID ||
          GETINT("poly", aindent,      "aindent") == INVALID ||
          GETSTR("poly", aw_spec,      "aw_spec") == INVALID ||
          GETSTR("poly", ah_spec,      "ah_spec") == INVALID ||
          GETSTR("poly", aindent_spec, "aindent_spec") == INVALID) {
         free(*ObjPtr);
         free(poly_ptr);
         free(v);
         *ObjPtr = NULL;
         return;
      }
      UtilRemoveQuotes(aw_spec);
      UtilRemoveQuotes(ah_spec);
      UtilRemoveQuotes(aindent_spec);
   }
   if (fileVersion >= 33 && transformed) {
      (void)fgets(inbuf, MAXSTRING, FP);
      scanLineNum++;
      InitScan(inbuf, "\t\n, ");

      ctm = (struct XfrmMtrxRec *)malloc(sizeof(struct XfrmMtrxRec));
      if (ctm == NULL) FailAllocMessage();
      if (GETINT("poly", real_x,           "real_x") == INVALID ||
          GETINT("poly", real_y,           "real_y") == INVALID ||
          GETINT("poly", orig_obbox.ltx,   "orig_obbox.ltx") == INVALID ||
          GETINT("poly", orig_obbox.lty,   "orig_obbox.lty") == INVALID ||
          GETINT("poly", orig_obbox.rbx,   "orig_obbox.rbx") == INVALID ||
          GETINT("poly", orig_obbox.rby,   "orig_obbox.rby") == INVALID ||
          GETDBL("poly", ctm->m[CTM_SX],   "CTM_SX") == INVALID ||
          GETDBL("poly", ctm->m[CTM_SIN],  "CTM_SIN") == INVALID ||
          GETDBL("poly", ctm->m[CTM_MSIN], "CTM_MSIN") == INVALID ||
          GETDBL("poly", ctm->m[CTM_SY],   "CTM_SY") == INVALID ||
          GETINT("poly", ctm->t[CTM_TX],   "CTM_TX") == INVALID ||
          GETINT("poly", ctm->t[CTM_TY],   "CTM_TY") == INVALID) {
         free(*ObjPtr);
         free(poly_ptr);
         free(v);
         *ObjPtr = NULL;
         free(ctm);
         return;
      }
   }
   if (fileVersion <= 32) {
      sprintf(width_spec, "%1d", width);
      sprintf(aw_spec, "%1d", aw);
      sprintf(ah_spec, "%1d", ah);
   }

   fill = UpgradePenFill(fill);
   pen = UpgradePenFill(pen);

   poly_ptr->style = style;
   poly_ptr->width = width;
   poly_ptr->aw = aw;
   poly_ptr->ah = ah;
   UtilStrCpyN(poly_ptr->width_spec, sizeof(poly_ptr->width_spec), width_spec);
   UtilStrCpyN(poly_ptr->aw_spec, sizeof(poly_ptr->aw_spec), aw_spec);
   UtilStrCpyN(poly_ptr->ah_spec, sizeof(poly_ptr->ah_spec), ah_spec);
   poly_ptr->pen = pen;
   poly_ptr->curved = curved;
   poly_ptr->fill = fill;
   poly_ptr->dash = dash;

   poly_ptr->vlist = v;
   poly_ptr->smooth = smooth;
   poly_ptr->svlist = poly_ptr->asvlist = NULL;
   poly_ptr->intvlist = NULL;

   poly_ptr->rotated_n = poly_ptr->rotated_asn = 0;
   poly_ptr->rotated_vlist = poly_ptr->rotated_asvlist = NULL;

   (*ObjPtr)->x = ltx;
   (*ObjPtr)->y = lty;
   (*ObjPtr)->color = QuickFindColorIndex(*ObjPtr, color_str, &new_alloc, TRUE);
   UtilStrCpyN((*ObjPtr)->color_str, sizeof((*ObjPtr)->color_str), color_str);
   (*ObjPtr)->dirty = FALSE;
   (*ObjPtr)->id = id;
   (*ObjPtr)->rotation = rotation;
   (*ObjPtr)->locked = locked;
   (*ObjPtr)->type = OBJ_POLY;
   (*ObjPtr)->obbox.ltx = ltx;
   (*ObjPtr)->obbox.lty = lty;
   (*ObjPtr)->obbox.rbx = rbx;
   (*ObjPtr)->obbox.rby = rby;
   (*ObjPtr)->detail.p = poly_ptr;
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
   if (poly_ptr->curved != LT_INTSPLINE) {
      UpdPolyBBox(*ObjPtr, poly_ptr->n, poly_ptr->vlist);
   } else {
      UpdPolyBBox(*ObjPtr, poly_ptr->intn, poly_ptr->intvlist);
   }
}

/* --------------------- SetPolyPropMask() --------------------- */

void SetPolyPropMask(ObjPtr, plMask, plSkip, pProp)
   struct ObjRec *ObjPtr;
   long *plMask, *plSkip;
   struct PropertiesRec *pProp;
{
   struct PolyRec *poly_ptr=ObjPtr->detail.p;

   SetCTMPropertyMask(ObjPtr->ctm, plMask, plSkip, pProp);

   SetIntPropertyMask(PROP_MASK_COLOR, ObjPtr->color,
         colorMenuItems[ObjPtr->color], plMask, plSkip, pProp);
   SetIntPropertyMask(PROP_MASK_WIDTH, poly_ptr->width, poly_ptr->width_spec,
         plMask, plSkip, pProp);
   SetIntPropertyMask(PROP_MASK_AW, poly_ptr->aw, poly_ptr->aw_spec,
         plMask, plSkip, pProp);
   SetIntPropertyMask(PROP_MASK_AH, poly_ptr->ah, poly_ptr->ah_spec,
         plMask, plSkip, pProp);

   SetIntPropertyMask(PROP_MASK_TRANSPAT, ObjPtr->trans_pat, NULL,
         plMask, plSkip, pProp);
   SetIntPropertyMask(PROP_MASK_FILL, poly_ptr->fill, NULL,
         plMask, plSkip, pProp);
   SetIntPropertyMask(PROP_MASK_PEN, poly_ptr->pen, NULL,
         plMask, plSkip, pProp);
   SetIntPropertyMask(PROP_MASK_DASH, poly_ptr->dash, NULL,
         plMask, plSkip, pProp);
   SetIntPropertyMask(PROP_MASK_CURVED, poly_ptr->curved, NULL,
         plMask, plSkip, pProp);
   SetIntPropertyMask(PROP_MASK_ARROW_STYLE, poly_ptr->style, NULL,
         plMask, plSkip, pProp);
}

/* --------------------- FreePolyObj() --------------------- */

void FreePolyObj(ObjPtr)
   struct ObjRec *ObjPtr;
{
   if (ObjPtr->detail.p->ssvlist != NULL) free(ObjPtr->detail.p->ssvlist);
   if (ObjPtr->detail.p->svlist != NULL) free(ObjPtr->detail.p->svlist);
   if (ObjPtr->detail.p->asvlist != NULL) free(ObjPtr->detail.p->asvlist);
   if (ObjPtr->detail.p->intvlist != NULL) free(ObjPtr->detail.p->intvlist);
   if (ObjPtr->detail.p->rotated_vlist != NULL) {
      free(ObjPtr->detail.p->rotated_vlist);
   }
   if (ObjPtr->detail.p->rotated_asvlist != NULL) {
      free(ObjPtr->detail.p->rotated_asvlist);
   }
   free(ObjPtr->detail.p->vlist);
   if (ObjPtr->detail.p->smooth != NULL) free(ObjPtr->detail.p->smooth);
   if (ObjPtr->detail.p->ssmooth != NULL) free(ObjPtr->detail.p->ssmooth);
   free(ObjPtr->detail.p);
   free(ObjPtr);
}

/* --------------------- InitPoly() --------------------- */

void InitPoly()
{
   ResetWiringNodeInfo();
   CVListInit(&gStructSplineList);
}
