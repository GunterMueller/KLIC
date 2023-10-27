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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/mark.c,v 1.25 2011/05/16 16:21:58 william Exp $
 */

#define _INCLUDE_FROM_MARK_C_

#include "tgifdefs.h"
#include "cmdids.h"

#include "choice.e"
#include "cmd.e"
#include "dialog.e"
#include "drawing.e"
#include "dup.e"
#include "exec.e"
#include "file.e"
#include "mark.e"
#include "msg.e"
#include "obj.e"
#include "oval.e"
#include "pattern.e"
#include "page.e"
#include "poly.e"
#include "polygon.e"
#include "raster.e"
#include "rect.e"
#include "setup.e"
#include "select.e"
#include "spline.e"
#include "stretch.e"
#include "strtbl.e"
#include "util.e"

int somethingHighLighted=FALSE;

static int tickMarkSize=8;

/* --------------------- Init & Clean Up --------------------- */

void CleanUpMark()
{
}

int InitMark()
{
   if (!(PRTGIF && !cmdLineOpenDisplay)) {
      char *c_ptr=NULL;

      tickMarkSize = 8;
      if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"TickMarkSize")) != NULL) {
         SetTickMarkSizeValue(c_ptr);
      }
      autoRotatePivot = FALSE;
      if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"AutoRotatePivot")) !=
            NULL && UtilStrICmp(c_ptr, "true") == 0) {
         autoRotatePivot = TRUE;
         choicePixmap[ROTATEMODE] = rotateModePixmap[!autoRotatePivot];
      }
   }
   return TRUE;
}

/* --------------------- HighLight Functions --------------------- */

static
void MarkStructuredSpline(obj_ptr, ssn, ssvs, ssmooth, curved, locked)
   struct ObjRec *obj_ptr;
   int ssn, curved, locked;
   char *ssmooth;
   IntPoint *ssvs;
{
   int i=0;

#ifdef _TGIF_DBG /* debug, do not translate */
   TgAssert(curved==LT_STRUCT_SPLINE,
         "curved != LT_STRUCT_SPLINE in MarkStructuredSpline()", NULL);
#endif /* _TGIF_DBG */

   if (obj_ptr->ctm == NULL) {
      if (ssmooth != NULL) {
         for (i = 0; i < ssn; i++) {
            if (ssmooth[i]) {
               MARKHO(drawWindow, locked ? revGrayGC : revDefaultGC,
                     OFFSET_X(ssvs[i].x), OFFSET_Y(ssvs[i].y));
            } else {
               MARKHR(drawWindow, locked ? revGrayGC : revDefaultGC,
                     OFFSET_X(ssvs[i].x), OFFSET_Y(ssvs[i].y));
            }
         }
      } else {
         for (i = 0; i < ssn; i++) {
            MARKHR(drawWindow, locked ? revGrayGC : revDefaultGC,
                  OFFSET_X(ssvs[i].x), OFFSET_Y(ssvs[i].y));
         }
      }
   } else {
      for (i=0; i < ssn; i++) {
         int x, y;

         TransformPointThroughCTM(ssvs[i].x-obj_ptr->x, ssvs[i].y-obj_ptr->y,
               obj_ptr->ctm, &x, &y);
         if (ssmooth != NULL && ssmooth[i]) {
            MARKHO(drawWindow, locked ? revGrayGC : revDefaultGC,
                  OFFSET_X(x+obj_ptr->x), OFFSET_Y(y+obj_ptr->y));
         } else {
            MARKHR(drawWindow, locked ? revGrayGC : revDefaultGC,
                  OFFSET_X(x+obj_ptr->x), OFFSET_Y(y+obj_ptr->y));
         }
      }
   }
}

static
void MarkPoly(ObjPtr, NumPts, V, Smooth, Curved, Locked)
   struct ObjRec *ObjPtr;
   int NumPts, Curved, Locked;
   char *Smooth;
   IntPoint *V;
{
   register int i;

   if (ObjPtr->ctm == NULL) {
      if (Curved != LT_INTSPLINE && Smooth != NULL) {
         for (i = 0; i < NumPts; i++) {
            if (Smooth[i]) {
               if (Curved == LT_STRUCT_SPLINE) {
                  MARKHO(drawWindow, Locked ? revGrayGC : revDefaultGC,
                        OFFSET_X(V[i].x), OFFSET_Y(V[i].y));
               } else {
                  MARKO(drawWindow, Locked ? revGrayGC : revDefaultGC,
                        OFFSET_X(V[i].x), OFFSET_Y(V[i].y));
               }
            } else {
               if (Curved == LT_STRUCT_SPLINE) {
                  MARKHR(drawWindow, Locked ? revGrayGC : revDefaultGC,
                        OFFSET_X(V[i].x), OFFSET_Y(V[i].y));
               } else {
                  MARK(drawWindow, Locked ? revGrayGC : revDefaultGC,
                        OFFSET_X(V[i].x), OFFSET_Y(V[i].y));
               }
            }
         }
      } else {
         for (i = 0; i < NumPts; i++) {
            MARK(drawWindow, Locked ? revGrayGC : revDefaultGC,
                  OFFSET_X(V[i].x), OFFSET_Y(V[i].y));
         }
      }
   } else {
      for (i=0; i < NumPts; i++) {
         int x, y;

         TransformPointThroughCTM(V[i].x-ObjPtr->x, V[i].y-ObjPtr->y,
               ObjPtr->ctm, &x, &y);
         if (Curved != LT_INTSPLINE && Smooth != NULL && Smooth[i]) {
            if (Curved == LT_STRUCT_SPLINE) {
               MARKHO(drawWindow, Locked ? revGrayGC : revDefaultGC,
                     OFFSET_X(x+ObjPtr->x), OFFSET_Y(y+ObjPtr->y));
            } else {
               MARKO(drawWindow, Locked ? revGrayGC : revDefaultGC,
                     OFFSET_X(x+ObjPtr->x), OFFSET_Y(y+ObjPtr->y));
            }
         } else {
            if (Curved == LT_STRUCT_SPLINE) {
               MARKHR(drawWindow, Locked ? revGrayGC : revDefaultGC,
                     OFFSET_X(x+ObjPtr->x), OFFSET_Y(y+ObjPtr->y));
            } else {
               MARK(drawWindow, Locked ? revGrayGC : revDefaultGC,
                     OFFSET_X(x+ObjPtr->x), OFFSET_Y(y+ObjPtr->y));
            }
         }
      }
   }
}

static
void Mark4Corners(BBox, Locked, Thin)
   struct BBRec BBox;
   int Locked, Thin;
{
   if (Locked) {
      MARK(drawWindow, revGrayGC, OFFSET_X(BBox.ltx), OFFSET_Y(BBox.lty));
      if (!Thin) {
         MARK(drawWindow, revGrayGC, OFFSET_X(BBox.ltx), OFFSET_Y(BBox.rby));
         MARK(drawWindow, revGrayGC, OFFSET_X(BBox.rbx), OFFSET_Y(BBox.lty));
      }
      MARK(drawWindow, revGrayGC, OFFSET_X(BBox.rbx), OFFSET_Y(BBox.rby));
   } else {
      MARK(drawWindow, revDefaultGC, OFFSET_X(BBox.ltx), OFFSET_Y(BBox.lty));
      if (!Thin) {
         MARK(drawWindow, revDefaultGC, OFFSET_X(BBox.ltx), OFFSET_Y(BBox.rby));
         MARK(drawWindow, revDefaultGC, OFFSET_X(BBox.rbx), OFFSET_Y(BBox.lty));
      }
      MARK(drawWindow, revDefaultGC, OFFSET_X(BBox.rbx), OFFSET_Y(BBox.rby));
   }
}

static
void Mark8Places(BBox, Locked)
   struct BBRec BBox;
   int Locked;
{
   register int xmid=0, ymid=0;

   if (ZOOMED_SIZE(BBox.rbx - BBox.ltx) == 0) {
      if (ZOOMED_SIZE(BBox.rby - BBox.lty) == 0) {
         MARK(drawWindow, Locked ? revGrayGC : revDefaultGC,
               OFFSET_X(BBox.ltx), OFFSET_Y(BBox.lty));
      } else {
         if (ZOOMED_SIZE(BBox.rby - BBox.lty) >= 10) {
            ymid = ((BBox.lty+BBox.rby) >> 1);
            MARK(drawWindow, Locked ? revGrayGC : revDefaultGC,
                  OFFSET_X(BBox.ltx), OFFSET_Y(ymid));
         }
         Mark4Corners(BBox, Locked, TRUE);
      }
   } else if (ZOOMED_SIZE(BBox.rby - BBox.lty) == 0) {
      if (ZOOMED_SIZE(BBox.rbx - BBox.ltx) >= 10) {
         xmid = (BBox.ltx+BBox.rbx) / 2;
         MARK(drawWindow, Locked ? revGrayGC : revDefaultGC,
               OFFSET_X(xmid), OFFSET_Y(BBox.lty));
      }
      Mark4Corners(BBox, Locked, TRUE);
   } else {
      if (ZOOMED_SIZE(BBox.rbx - BBox.ltx) >= 10) {
         xmid = (BBox.ltx+BBox.rbx) / 2;
         if (Locked) {
            MARK(drawWindow, revGrayGC, OFFSET_X(xmid), OFFSET_Y(BBox.lty));
            MARK(drawWindow, revGrayGC, OFFSET_X(xmid), OFFSET_Y(BBox.rby));
         } else {
            MARK(drawWindow, revDefaultGC, OFFSET_X(xmid), OFFSET_Y(BBox.lty));
            MARK(drawWindow, revDefaultGC, OFFSET_X(xmid), OFFSET_Y(BBox.rby));
         }
      }
      if (ZOOMED_SIZE(BBox.rby - BBox.lty) >= 10) {
         ymid = ((BBox.lty+BBox.rby) >> 1);
         if (Locked) {
            MARK(drawWindow, revGrayGC, OFFSET_X(BBox.ltx), OFFSET_Y(ymid));
            MARK(drawWindow, revGrayGC, OFFSET_X(BBox.rbx), OFFSET_Y(ymid));
         } else {
            MARK(drawWindow, revDefaultGC, OFFSET_X(BBox.ltx), OFFSET_Y(ymid));
            MARK(drawWindow, revDefaultGC, OFFSET_X(BBox.rbx), OFFSET_Y(ymid));
         }
      }
      Mark4Corners(BBox, Locked, FALSE);
   }
}

void HighLightAnObj(ObjPtr)
   register struct ObjRec *ObjPtr;
{
   if (execCurDepth > 0) return;
   switch (ObjPtr->type) {
   case OBJ_POLY:
      if (ObjPtr->detail.p->curved == LT_STRUCT_SPLINE) {
         MarkStructuredSpline(ObjPtr, ObjPtr->detail.p->ssn,
               ObjPtr->detail.p->ssvlist, ObjPtr->detail.p->ssmooth,
               ObjPtr->detail.p->curved, ObjPtr->locked);
      } else {
         MarkPoly(ObjPtr, ObjPtr->detail.p->n, ObjPtr->detail.p->vlist,
               ObjPtr->detail.p->smooth, ObjPtr->detail.p->curved,
               ObjPtr->locked);
      }
      break;
   case OBJ_POLYGON:
      if (ObjPtr->detail.g->curved == LT_STRUCT_SPLINE) {
         MarkStructuredSpline(ObjPtr, ObjPtr->detail.g->ssn-1,
               ObjPtr->detail.g->ssvlist, ObjPtr->detail.g->ssmooth,
               ObjPtr->detail.g->curved, ObjPtr->locked);
      } else {
         MarkPoly(ObjPtr, ObjPtr->detail.g->n-1, ObjPtr->detail.g->vlist,
               ObjPtr->detail.g->smooth, ObjPtr->detail.g->curved,
               ObjPtr->locked);
      }
      break;

   case OBJ_BOX:
   case OBJ_OVAL:
   case OBJ_TEXT:
   case OBJ_ARC:
   case OBJ_RCBOX:
   case OBJ_GROUP:
   case OBJ_XBM:
   case OBJ_XPM:
   case OBJ_SYM:
   case OBJ_ICON:
   case OBJ_PIN:
      Mark8Places(ObjPtr->obbox, ObjPtr->locked);
      break;
   }
}

static
void HighLightVertices(Dir)
   int Dir;
{
   register struct VSelRec *vsel_ptr=NULL;
   register struct ObjRec *obj_ptr;
   register int i;
   struct SelRec *sel_ptr=NULL;

   if (mainDisplay == NULL || execCurDepth > 0) return;

   switch (Dir) {
   case FORWARD: vsel_ptr=botVSel; sel_ptr=botSel; break;
   case REVERSE: vsel_ptr=topVSel; sel_ptr=topSel; break;
   }
   while (sel_ptr != NULL) {
      obj_ptr = sel_ptr->obj;

      switch (obj_ptr->type) {
      case OBJ_POLY:
         if (obj_ptr->detail.p->curved == LT_STRUCT_SPLINE) {
            MarkStructuredSpline(obj_ptr, obj_ptr->detail.p->ssn,
                  obj_ptr->detail.p->ssvlist, obj_ptr->detail.p->ssmooth,
                  obj_ptr->detail.p->curved, obj_ptr->locked);
         } else {
            MarkPoly(obj_ptr, obj_ptr->detail.p->n, obj_ptr->detail.p->vlist,
                  obj_ptr->detail.p->smooth, obj_ptr->detail.p->curved,
                  obj_ptr->locked);
         }
         break;
      case OBJ_POLYGON:
         if (obj_ptr->detail.g->curved == LT_STRUCT_SPLINE) {
            MarkStructuredSpline(obj_ptr, obj_ptr->detail.g->ssn-1,
                  obj_ptr->detail.g->ssvlist, obj_ptr->detail.g->ssmooth,
                  obj_ptr->detail.g->curved, obj_ptr->locked);
         } else {
            MarkPoly(obj_ptr, obj_ptr->detail.g->n-1, obj_ptr->detail.g->vlist,
                  obj_ptr->detail.g->smooth, obj_ptr->detail.g->curved,
                  obj_ptr->locked);
         }
         break;
      }
      switch (Dir) {
      case FORWARD: sel_ptr = sel_ptr->prev; break;
      case REVERSE: sel_ptr = sel_ptr->next; break;
      }
   }
   while (vsel_ptr != NULL) {
      char *smooth=NULL;
      int curved=(-1);

      switch (vsel_ptr->obj->type) {
      case OBJ_POLY:
         curved = vsel_ptr->obj->detail.p->curved;
         if (curved == LT_STRUCT_SPLINE) {
            smooth = vsel_ptr->obj->detail.p->ssmooth;
         } else {
            smooth = vsel_ptr->obj->detail.p->smooth;
         }
         break;
      case OBJ_POLYGON:
         curved = vsel_ptr->obj->detail.g->curved;
         if (curved == LT_STRUCT_SPLINE) {
            smooth = vsel_ptr->obj->detail.g->ssmooth;
         } else {
            smooth = vsel_ptr->obj->detail.g->smooth;
         }
         break;
      }
      for (i = 0; i < vsel_ptr->n; i++) {
         if (!(vsel_ptr->obj->type==OBJ_POLYGON &&
               vsel_ptr->obj->detail.g->n-1==vsel_ptr->v_index[i])) {
            if (curved != LT_INTSPLINE && curved != (-1) && smooth != NULL) {
               if (smooth[vsel_ptr->v_index[i]]) {
                  if (curved == LT_STRUCT_SPLINE) {
                     MARKHO(drawWindow, revDefaultGC, OFFSET_X(vsel_ptr->x[i]),
                           OFFSET_Y(vsel_ptr->y[i]));
                  } else {
                     MARKO(drawWindow, revDefaultGC, OFFSET_X(vsel_ptr->x[i]),
                           OFFSET_Y(vsel_ptr->y[i]));
                  }
               } else {
                  if (curved == LT_STRUCT_SPLINE) {
                     MARKHR(drawWindow, revDefaultGC, OFFSET_X(vsel_ptr->x[i]),
                           OFFSET_Y(vsel_ptr->y[i]));
                  } else {
                     MARK(drawWindow, revDefaultGC, OFFSET_X(vsel_ptr->x[i]),
                           OFFSET_Y(vsel_ptr->y[i]));
                  }
               }
            } else {
               MARK(drawWindow, revDefaultGC, OFFSET_X(vsel_ptr->x[i]),
                     OFFSET_Y(vsel_ptr->y[i]));
            }
            MARKV(drawWindow, revDefaultGC, OFFSET_X(vsel_ptr->x[i]),
                  OFFSET_Y(vsel_ptr->y[i]));
         }
      }
      switch (Dir) {
      case FORWARD: vsel_ptr = vsel_ptr->prev; break;
      case REVERSE: vsel_ptr = vsel_ptr->next; break;
      }
   }
}

static
void HighLight(Dir)
   int Dir;
{
   register struct SelRec *sel_ptr=NULL;
   register struct ObjRec *obj_ptr;

   if (mainDisplay == NULL || execCurDepth > 0) return;

   switch (Dir) {
   case FORWARD: sel_ptr = botSel; break;
   case REVERSE: sel_ptr = topSel; break;
   }
   while (sel_ptr != NULL) {
      obj_ptr = sel_ptr->obj;
      switch (obj_ptr->type) {
      case OBJ_POLY:
         if (obj_ptr->detail.p->curved == LT_STRUCT_SPLINE) {
            MarkStructuredSpline(obj_ptr, obj_ptr->detail.p->ssn,
                  obj_ptr->detail.p->ssvlist, obj_ptr->detail.p->ssmooth,
                  obj_ptr->detail.p->curved, obj_ptr->locked);
         } else {
            MarkPoly(obj_ptr, obj_ptr->detail.p->n, obj_ptr->detail.p->vlist,
                  obj_ptr->detail.p->smooth, obj_ptr->detail.p->curved,
                  obj_ptr->locked);
         }
         break;
      case OBJ_POLYGON:
         if (obj_ptr->detail.g->curved == LT_STRUCT_SPLINE) {
            MarkStructuredSpline(obj_ptr, obj_ptr->detail.g->ssn-1,
                  obj_ptr->detail.g->ssvlist, obj_ptr->detail.g->ssmooth,
                  obj_ptr->detail.g->curved, obj_ptr->locked);
         } else {
            MarkPoly(obj_ptr, obj_ptr->detail.g->n-1, obj_ptr->detail.g->vlist,
                  obj_ptr->detail.g->smooth, obj_ptr->detail.g->curved,
                  obj_ptr->locked);
         }
         break;

      case OBJ_BOX:
      case OBJ_OVAL:
      case OBJ_TEXT:
      case OBJ_GROUP:
      case OBJ_ARC:
      case OBJ_RCBOX:
      case OBJ_XBM:
      case OBJ_XPM:
      case OBJ_SYM:
      case OBJ_ICON:
      case OBJ_PIN:
         Mark8Places(obj_ptr->obbox, obj_ptr->locked);
         break;
      }
      switch (Dir) {
      case FORWARD: sel_ptr = sel_ptr->prev; break;
      case REVERSE: sel_ptr = sel_ptr->next; break;
      }
   }
}

void HighLightForward()
{
   if (curChoice == VERTEXMODE) {
      HighLightVertices(FORWARD);
   } else {
      HighLight(FORWARD);
      if (!autoRotatePivot && curChoice == ROTATEMODE) {
         HighLightRotatePivot(FORWARD);
      }
   }
   somethingHighLighted = TRUE;
}
 
void HighLightReverse()
{
   if (curChoice == VERTEXMODE) {
      HighLightVertices(REVERSE);
   } else {
      if (!autoRotatePivot && curChoice == ROTATEMODE) {
         HighLightRotatePivot(REVERSE);
      }
      HighLight(REVERSE);
   }
   somethingHighLighted = FALSE;
}

/* --------------------- AddTickMarks() --------------------- */

struct tagAddTickMarksInfo {
   int n;
   IntPoint *vs;
   char *smooth;
} gATMI;

static
int CreatePolygonTickMarks(num_pts)
   int num_pts;
{
   gATMI.vs = (IntPoint*)malloc((num_pts+1)*sizeof(IntPoint));
   if (gATMI.vs == NULL) return FailAllocMessage();
   memset(gATMI.vs, 0, (num_pts+1)*sizeof(IntPoint));

   gATMI.smooth = (char*)malloc(num_pts*sizeof(char));
   if (gATMI.smooth == NULL) {
      free(gATMI.vs);
      gATMI.vs = NULL;
      return FailAllocMessage();
   }
   memset(gATMI.smooth, 0, num_pts*sizeof(char));

   gATMI.n = num_pts;
   return TRUE;
}

static
int StartCreateTickMarks(nCmdId)
   int nCmdId;
{
   switch (nCmdId) {
   case CMDID_ADDSQUARETICKMARKS: return CreatePolygonTickMarks(5);
   case CMDID_ADDTRIANGLETICKMARKS: return CreatePolygonTickMarks(4);
   case CMDID_ADDCIRCLETICKMARKS: return TRUE;
   case CMDID_ADDXTICKMARKS: return CreatePolygonTickMarks(9);
   case CMDID_ADDDIAMONDTICKMARKS: return CreatePolygonTickMarks(5);
   case CMDID_ADDBOWTIETICKMARKS: return CreatePolygonTickMarks(5);
   case CMDID_ADDINVTRIANGLETICKMARKS: return CreatePolygonTickMarks(4);
   case CMDID_ADDPLUSTICKMARKS: return CreatePolygonTickMarks(9);
   case CMDID_ADDHOURGLASSTICKMARKS: return CreatePolygonTickMarks(5);
   default: break;
   }
   return FALSE;
}

static
void EndCreateTickMarks()
{
   if (gATMI.vs != NULL) free(gATMI.vs);
   if (gATMI.smooth != NULL) free(gATMI.smooth);
   gATMI.vs = NULL;
   gATMI.smooth = NULL;
   gATMI.n = 0;
}

static
void CreatePolygonalTickMark()
{
   int i=0, saved_cur_spline=curSpline;

   curSpline = LT_STRAIGHT;
   ResetCreatePolygon();
   for (i=0; i < gATMI.n; i++) {
      AddPtToCreatePolygon(gATMI.vs[i].x, gATMI.vs[i].y);
   }
   CreatePolygonObj(gATMI.n, TRUE);
   curSpline = saved_cur_spline;
}

static
void CalcSquareVs(v)
   IntPoint v;
{
   int half_w=(tickMarkSize>>1);

   gATMI.vs[0].x = v.x-half_w; gATMI.vs[0].y = v.y-half_w;
   gATMI.vs[1].x = v.x+half_w; gATMI.vs[1].y = v.y-half_w;
   gATMI.vs[2].x = v.x+half_w; gATMI.vs[2].y = v.y+half_w;
   gATMI.vs[3].x = v.x-half_w; gATMI.vs[3].y = v.y+half_w;
   gATMI.vs[4].x = v.x-half_w; gATMI.vs[4].y = v.y-half_w;

   CreatePolygonalTickMark();
}

static
void CalcTriangleVs(v)
   IntPoint v;
{
   int i=0, half_w=(tickMarkSize>>1);
   double d_offset=((double)tickMarkSize)*((double)3)/((double)16);
   int offset=round(d_offset);

   gATMI.vs[0].x = v.x;        gATMI.vs[0].y = v.y-half_w;
   gATMI.vs[1].x = v.x+half_w; gATMI.vs[1].y = v.y+half_w;
   gATMI.vs[2].x = v.x-half_w; gATMI.vs[2].y = v.y+half_w;
   gATMI.vs[3].x = v.x;        gATMI.vs[3].y = v.y-half_w;
   for (i=0; i < 4; i++) gATMI.vs[i].y -= offset;

   CreatePolygonalTickMark();
}

static
void CalcCircleVs(v)
   IntPoint v;
{
   int half_w=(tickMarkSize>>1);
   struct BBRec obbox;

   obbox.ltx = v.x-half_w; obbox.lty = v.y-half_w;
   obbox.rbx = v.x+half_w; obbox.rby = v.y+half_w;

   CreateOvalObj(&obbox, TRUE);
}

static
void CalcXVs(v)
   IntPoint v;
{
   int half_w=(tickMarkSize>>1);

   gATMI.vs[0].x = v.x;        gATMI.vs[0].y = v.y;       
   gATMI.vs[1].x = v.x-half_w; gATMI.vs[1].y = v.y-half_w;
   gATMI.vs[2].x = v.x;        gATMI.vs[2].y = v.y;       
   gATMI.vs[3].x = v.x+half_w; gATMI.vs[3].y = v.y-half_w;
   gATMI.vs[4].x = v.x;        gATMI.vs[4].y = v.y;       
   gATMI.vs[5].x = v.x+half_w; gATMI.vs[5].y = v.y+half_w;
   gATMI.vs[6].x = v.x;        gATMI.vs[6].y = v.y;       
   gATMI.vs[7].x = v.x-half_w; gATMI.vs[7].y = v.y+half_w;
   gATMI.vs[8].x = v.x;        gATMI.vs[8].y = v.y;       

   CreatePolygonalTickMark();
}

static
void CalcDiamondVs(v)
   IntPoint v;
{
   int half_w=(tickMarkSize>>1);

   gATMI.vs[0].x = v.x-half_w; gATMI.vs[0].y = v.y;
   gATMI.vs[1].x = v.x;        gATMI.vs[1].y = v.y-half_w;
   gATMI.vs[2].x = v.x+half_w; gATMI.vs[2].y = v.y;
   gATMI.vs[3].x = v.x;        gATMI.vs[3].y = v.y+half_w;
   gATMI.vs[4].x = v.x-half_w; gATMI.vs[4].y = v.y;

   CreatePolygonalTickMark();
}

static
void CalcBowtieVs(v)
   IntPoint v;
{
   int half_w=(tickMarkSize>>1);

   gATMI.vs[0].x = v.x-half_w; gATMI.vs[0].y = v.y-half_w;
   gATMI.vs[1].x = v.x-half_w; gATMI.vs[1].y = v.y+half_w;
   gATMI.vs[2].x = v.x+half_w; gATMI.vs[2].y = v.y-half_w;
   gATMI.vs[3].x = v.x+half_w; gATMI.vs[3].y = v.y+half_w;
   gATMI.vs[4].x = v.x-half_w; gATMI.vs[4].y = v.y-half_w;

   CreatePolygonalTickMark();
}

static
void CalcInvTriangleVs(v)
   IntPoint v;
{
   int i=0, half_w=(tickMarkSize>>1);
   double d_offset=((double)tickMarkSize)*((double)3)/((double)16);
   int offset=round(d_offset);

   gATMI.vs[0].x = v.x;        gATMI.vs[0].y = v.y+half_w;
   gATMI.vs[1].x = v.x+half_w; gATMI.vs[1].y = v.y-half_w;
   gATMI.vs[2].x = v.x-half_w; gATMI.vs[2].y = v.y-half_w;
   gATMI.vs[3].x = v.x;        gATMI.vs[3].y = v.y+half_w;
   for (i=0; i < 4; i++) gATMI.vs[i].y += offset;

   CreatePolygonalTickMark();
}

static
void CalcPlusVs(v)
   IntPoint v;
{
   int half_w=(tickMarkSize>>1);

   gATMI.vs[0].x = v.x;        gATMI.vs[0].y = v.y;       
   gATMI.vs[1].x = v.x-half_w; gATMI.vs[1].y = v.y;
   gATMI.vs[2].x = v.x;        gATMI.vs[2].y = v.y;       
   gATMI.vs[3].x = v.x;        gATMI.vs[3].y = v.y-half_w;
   gATMI.vs[4].x = v.x;        gATMI.vs[4].y = v.y;       
   gATMI.vs[5].x = v.x+half_w; gATMI.vs[5].y = v.y;
   gATMI.vs[6].x = v.x;        gATMI.vs[6].y = v.y;       
   gATMI.vs[7].x = v.x;        gATMI.vs[7].y = v.y+half_w;
   gATMI.vs[8].x = v.x;        gATMI.vs[8].y = v.y;       

   CreatePolygonalTickMark();
}

static
void CalcHourGlassVs(v)
   IntPoint v;
{
   int half_w=(tickMarkSize>>1);

   gATMI.vs[0].x = v.x-half_w; gATMI.vs[0].y = v.y-half_w;
   gATMI.vs[1].x = v.x+half_w; gATMI.vs[1].y = v.y-half_w;
   gATMI.vs[2].x = v.x-half_w; gATMI.vs[2].y = v.y+half_w;
   gATMI.vs[3].x = v.x+half_w; gATMI.vs[3].y = v.y+half_w;
   gATMI.vs[4].x = v.x-half_w; gATMI.vs[4].y = v.y-half_w;

   CreatePolygonalTickMark();
}

static
void CreateTickMarks(n, vs, nCmdId)
   int n, nCmdId;
   IntPoint *vs;
{
   int i=0;

   for (i=0; i < n; i++) {
      switch (nCmdId) {
      case CMDID_ADDSQUARETICKMARKS: CalcSquareVs(vs[i]); break;
      case CMDID_ADDTRIANGLETICKMARKS: CalcTriangleVs(vs[i]); break;
      case CMDID_ADDCIRCLETICKMARKS: CalcCircleVs(vs[i]); break;
      case CMDID_ADDXTICKMARKS: CalcXVs(vs[i]); break;
      case CMDID_ADDDIAMONDTICKMARKS: CalcDiamondVs(vs[i]); break;
      case CMDID_ADDBOWTIETICKMARKS: CalcBowtieVs(vs[i]); break;
      case CMDID_ADDINVTRIANGLETICKMARKS: CalcInvTriangleVs(vs[i]); break;
      case CMDID_ADDPLUSTICKMARKS: CalcPlusVs(vs[i]); break;
      case CMDID_ADDHOURGLASSTICKMARKS: CalcHourGlassVs(vs[i]); break;
      default: break;
      }
   }
}

static
void SetRealVertex(vs_return, i, dvs)
   IntPoint *vs_return;
   int i;
   DoublePoint dvs[3];
{
   DoublePoint m1, m2, m;

   m1.x = (dvs[0].x+dvs[1].x) / ((double)2);
   m1.y = (dvs[0].y+dvs[1].y) / ((double)2);
   m2.x = (dvs[1].x+dvs[2].x) / ((double)2);
   m2.y = (dvs[1].y+dvs[2].y) / ((double)2);
   m.x = (m1.x+m2.x) / ((double)2);
   m.y = (m1.y+m2.y) / ((double)2);

   vs_return[i].x = round(m.x);
   vs_return[i].y = round(m.y);
}

static
IntPoint *GetSplinePolyTickMarkVs(pn_return, n, vs, smooth)
   int *pn_return, n;
   IntPoint *vs;
   char *smooth;
{
   int i=0, first_hinge_index=0, write_index=0;
   IntPoint *vs_return=(IntPoint*)malloc(((n<<1)-1)*sizeof(IntPoint));

   if (vs_return == NULL) FailAllocMessage();
   memset(vs_return, 0, n*sizeof(IntPoint));

   i = write_index = 0;
   while (i < n) {
      if (smooth[i]) {
         int j=0, last_hinge_index=i, num_vs=0;
         DoublePoint dvs[3];

         for (last_hinge_index=i+1; smooth[last_hinge_index];
               last_hinge_index++) {
         }
         num_vs = last_hinge_index-first_hinge_index+1;
         if (num_vs > 3) {
            DoublePoint *mid_pt_of_mid_segs=NULL;

            mid_pt_of_mid_segs =
                  (DoublePoint*)malloc((num_vs-3)*sizeof(DoublePoint));
            if (mid_pt_of_mid_segs == NULL) FailAllocMessage();
            memset(mid_pt_of_mid_segs, 0, (num_vs-3)*sizeof(DoublePoint));
   
            for (j=first_hinge_index+1; j < last_hinge_index-1; j++) {
               /*
                * These are the midpoints of imaginary line segments
                * the spline actually runs through.
                */
               mid_pt_of_mid_segs[j-first_hinge_index-1].x =
                     ((double)(vs[j].x + vs[j+1].x)) / ((double)2);
               mid_pt_of_mid_segs[j-first_hinge_index-1].y =
                     ((double)(vs[j].y + vs[j+1].y)) / ((double)2);
            }
            dvs[0].x = (double)(vs[first_hinge_index].x);
            dvs[0].y = (double)(vs[first_hinge_index].y);
            dvs[1].x = (double)(vs[first_hinge_index+1].x);
            dvs[1].y = (double)(vs[first_hinge_index+1].y);
            dvs[2].x = mid_pt_of_mid_segs[0].x;
            dvs[2].y = mid_pt_of_mid_segs[0].y;
            SetRealVertex(vs_return, write_index++, dvs);
            vs_return[write_index].x = round(dvs[2].x);
            vs_return[write_index].y = round(dvs[2].y);
            write_index++;
   
            for (j=first_hinge_index+2; j < last_hinge_index-1; j++) {
               dvs[0].x = mid_pt_of_mid_segs[j-first_hinge_index-2].x;
               dvs[0].y = mid_pt_of_mid_segs[j-first_hinge_index-2].y;
               dvs[1].x = (double)(vs[j].x);
               dvs[1].y = (double)(vs[j].y);
               dvs[2].x = mid_pt_of_mid_segs[j-first_hinge_index-1].x;
               dvs[2].y = mid_pt_of_mid_segs[j-first_hinge_index-1].y;
               SetRealVertex(vs_return, write_index++, dvs);
               vs_return[write_index].x = round(dvs[2].x);
               vs_return[write_index].y = round(dvs[2].y);
               write_index++;
            }
            dvs[0].x = mid_pt_of_mid_segs[j-2].x;
            dvs[0].y = mid_pt_of_mid_segs[j-2].y;
            dvs[1].x = (double)(vs[last_hinge_index-1].x);
            dvs[1].y = (double)(vs[last_hinge_index-1].y);
            dvs[2].x = (double)(vs[last_hinge_index].x);
            dvs[2].y = (double)(vs[last_hinge_index].y);
            SetRealVertex(vs_return, write_index++, dvs);
   
            free(mid_pt_of_mid_segs);
         } else {
            dvs[0].x = (double)(vs[first_hinge_index].x);
            dvs[0].y = (double)(vs[first_hinge_index].y);
            dvs[1].x = (double)(vs[first_hinge_index+1].x);
            dvs[1].y = (double)(vs[first_hinge_index+1].y);
            dvs[2].x = (double)(vs[first_hinge_index+2].x);
            dvs[2].y = (double)(vs[first_hinge_index+2].y);
            SetRealVertex(vs_return, write_index++, dvs);
         }
         i = last_hinge_index;
      } else {
         vs_return[write_index].x = vs[i].x;
         vs_return[write_index].y = vs[i].y;
         first_hinge_index = i;
         i++;
         write_index++;
      }
   }
   *pn_return = write_index;

   return vs_return;
}

static
IntPoint *GetHingedPolygonTickMarkVs(pn_return, n, vs, smooth,
      first_hinge_index)
   int *pn_return, n, first_hinge_index;
   IntPoint *vs;
   char *smooth;
{
   int i=0, j=0;
   IntPoint *tmp_vs=NULL;
   char *tmp_smooth=NULL;
   IntPoint *vs_return=NULL;

   if (first_hinge_index == 0) {
      return GetSplinePolyTickMarkVs(pn_return, n-1, vs, smooth);
   }
   tmp_vs = (IntPoint*)malloc(n*sizeof(IntPoint));
   if (tmp_vs == NULL) FailAllocMessage();
   memset(tmp_vs, 0, n*sizeof(IntPoint));

   tmp_smooth = (char*)malloc(n*sizeof(char));
   if (tmp_smooth == NULL) FailAllocMessage();
   memset(tmp_smooth, 0, n*sizeof(char));

   i = first_hinge_index;
   for (j=0; i < n-1; j++, i++) {
      tmp_vs[j].x = vs[i].x;
      tmp_vs[j].y = vs[i].y;
      tmp_smooth[j] = smooth[i];
   }
   for (i=0; j < n; j++, i++) {
      tmp_vs[j].x = vs[i].x;
      tmp_vs[j].y = vs[i].y;
      tmp_smooth[j] = smooth[i];
   }
   vs_return =  GetSplinePolyTickMarkVs(pn_return, n-1, tmp_vs,
         tmp_smooth);
   free(tmp_vs);

   return vs_return;
}

static
IntPoint *GetSplinePolygonTickMarkVs(pn_return, n, vs, smooth)
   int *pn_return, n;
   IntPoint *vs;
   char *smooth;
{
   int i=0, write_index=0;
   IntPoint *vs_return=NULL;
   DoublePoint *mid_pt_of_mid_segs=NULL, dvs[3];

   for (i=0; i < n; i++) {
      if (!smooth[i]) {
         return GetHingedPolygonTickMarkVs(pn_return, n, vs, smooth, i);
      }
   }
   /* every control point is smooth */
   vs_return = (IntPoint*)malloc(((n<<1)-1)*sizeof(IntPoint));
   if (vs_return == NULL) FailAllocMessage();
   memset(vs_return, 0, ((n<<1)-1)*sizeof(IntPoint));

   write_index = 0;

   mid_pt_of_mid_segs =
         (DoublePoint*)malloc((n-1)*sizeof(DoublePoint));
   if (mid_pt_of_mid_segs == NULL) FailAllocMessage();
   memset(mid_pt_of_mid_segs, 0, (n-1)*sizeof(DoublePoint));

   for (i=0; i < n-1; i++) {
      /*
       * These are the midpoints of imaginary line segments
       * the spline actually runs through.
       */
      mid_pt_of_mid_segs[i].x = ((double)(vs[i].x + vs[i+1].x)) / ((double)2);
      mid_pt_of_mid_segs[i].y = ((double)(vs[i].y + vs[i+1].y)) / ((double)2);
   }
   for (i=0; i < n-1; i++) {
      if (i == 0) {
         dvs[0].x = mid_pt_of_mid_segs[n-2].x;
         dvs[0].y = mid_pt_of_mid_segs[n-2].y;
      } else {
         dvs[0].x = mid_pt_of_mid_segs[i-1].x;
         dvs[0].y = mid_pt_of_mid_segs[i-1].y;
      }
      dvs[1].x = (double)(vs[i].x);
      dvs[1].y = (double)(vs[i].y);
      dvs[2].x = mid_pt_of_mid_segs[i].x;
      dvs[2].y = mid_pt_of_mid_segs[i].y;

      SetRealVertex(vs_return, write_index++, dvs);
      vs_return[write_index].x = round(dvs[2].x);
      vs_return[write_index].y = round(dvs[2].y);
      write_index++;
   }
   free(mid_pt_of_mid_segs);

   *pn_return = write_index;

   return vs_return;
}

static
int AddObjTickMarks(ObjPtr, nCmdId)
   struct ObjRec *ObjPtr;
   int nCmdId;
{
   int n=0, vs_allocated=FALSE, num_cntrl_points=0, cntrl_vs_allocated=FALSE;
   IntPoint *vs=NULL, *cntrl_vs=NULL;
   char *smooth=NULL;
   struct PolyRec *poly_ptr=NULL;
   struct PolygonRec *polygon_ptr=NULL;

   cntrl_vs = GetPolyOrPolygonAbsVs(&num_cntrl_points, &cntrl_vs_allocated,
         ObjPtr);
   switch (ObjPtr->type) {
   case OBJ_POLY:
      poly_ptr = ObjPtr->detail.p;
      smooth = poly_ptr->smooth;
      switch (poly_ptr->curved) {
      case LT_STRAIGHT:
         n = num_cntrl_points;
         vs = cntrl_vs;
         break;
      case LT_SPLINE:
         vs = GetSplinePolyTickMarkVs(&n, num_cntrl_points, cntrl_vs, smooth);
         vs_allocated = TRUE;
         break;
      case LT_INTSPLINE:
         n = num_cntrl_points;
         vs = cntrl_vs;
         break;
      case LT_STRUCT_SPLINE:
         vs = GetStructuredSplinePolyTickMarkVs(&n, ObjPtr, poly_ptr, NULL);
         vs_allocated = TRUE;
         break;
      }
      break;
   case OBJ_POLYGON:
      polygon_ptr = ObjPtr->detail.g;
      smooth = polygon_ptr->smooth;
      switch (polygon_ptr->curved) {
      case LT_STRAIGHT:
         n = num_cntrl_points-1;
         vs = cntrl_vs;
         break;
      case LT_SPLINE:
         vs = GetSplinePolygonTickMarkVs(&n, num_cntrl_points, cntrl_vs,
               smooth);
         vs_allocated = TRUE;
         break;
      case LT_INTSPLINE:
         n = num_cntrl_points-1;
         vs = cntrl_vs;
         break;
      case LT_STRUCT_SPLINE:
         vs = GetStructuredSplinePolyTickMarkVs(&n, ObjPtr, NULL, polygon_ptr);
         vs_allocated = TRUE;
         break;
      }
      break;
   }
   CreateTickMarks(n, vs, nCmdId);

   if (vs_allocated) free(vs);
   if (cntrl_vs_allocated) free(cntrl_vs);

   return TRUE;
}

void AddTickMarks(nCmdId)
   int nCmdId;
{
   struct SelRec *sel_ptr=NULL;
   struct ObjRec *saved_top_obj=topObj, *saved_bot_obj=botObj;
   int changed=FALSE;

   if (topSel == NULL) {
      MsgBox(TgLoadString(STID_NO_POLY_ETC_SELECTED), TOOL_NAME, INFO_MB);
      return;
   }
   StartCreateTickMarks(nCmdId);
   HighLightReverse();
   curPage->top = curPage->bot = topObj = botObj = NULL;
   for (sel_ptr=botSel; sel_ptr != NULL; sel_ptr=sel_ptr->prev) {
      struct ObjRec *obj_ptr=sel_ptr->obj;

      if (obj_ptr->type == OBJ_POLY || obj_ptr->type == OBJ_POLYGON) {
         AddObjTickMarks(sel_ptr->obj, nCmdId);
      }
   }
   EndCreateTickMarks();

   if (topObj != NULL) {
      struct ObjRec *obj_ptr=NULL, *new_bot_obj=botObj, *prev_obj=NULL;

      changed = TRUE;
      RemoveAllSel();
      curPage->top = topObj = saved_top_obj;
      curPage->bot = botObj = saved_bot_obj;
      StartCompositeCmd();
      for (obj_ptr=new_bot_obj; obj_ptr != NULL; obj_ptr=prev_obj) {
         prev_obj = obj_ptr->prev;
         AddObj(NULL, topObj, obj_ptr);
         RecordNewObjCmd();
      }
      EndCompositeCmd();

      for (obj_ptr=saved_top_obj->prev; obj_ptr != NULL; obj_ptr=prev_obj) {
         prev_obj = obj_ptr->prev;
         AddObjIntoSel(obj_ptr, NULL, topSel, &topSel, &botSel);
      }
   } else {
      curPage->top = topObj = saved_top_obj;
      curPage->bot = botObj = saved_bot_obj;
   }
   if (changed) {
      SetFileModified(TRUE);
      UpdSelBBox();
      RedrawAnArea(botObj, selLtX-GRID_ABS_SIZE(1), selLtY-GRID_ABS_SIZE(1),
            selRbX+GRID_ABS_SIZE(1), selRbY+GRID_ABS_SIZE(1));
      justDupped = FALSE;
   } else {
      HighLightForward();
      MsgBox(TgLoadString(STID_NO_POLY_ETC_SELECTED), TOOL_NAME, INFO_MB);
      return;
   }
   HighLightForward();
}

int SetTickMarkSizeValue(spec)
   char *spec;
{
   int ival=0;

   if (sscanf(spec, "%d", &ival) != 1) {
      sprintf(gszMsgBox, TgLoadCachedString(CSTID_MALFORMED_INPUT_STR), spec);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return FALSE;
   } else if (ival < 2) {
      sprintf(gszMsgBox, TgLoadString(STID_ENT_VAL_RANGE_ENTER_GE_INT),
            spec, 3);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return FALSE;
   }
   tickMarkSize = ival;

   return TRUE;
}

void SetTickMarkSize(psz_num_segs)
   char *psz_num_segs;
{
   char spec[MAXSTRING+1];

   *spec = '\0';
   if (psz_num_segs != NULL && strcmp(psz_num_segs, "-1") != 0) {
      UtilStrCpyN(spec, sizeof(spec), psz_num_segs);
   } else {
      sprintf(gszMsgBox, TgLoadString(STID_ENTER_TICK_MARK_SIZE_CUR_IS),
            tickMarkSize);
      if (Dialog(gszMsgBox, NULL, spec) == INVALID) return;
   }
   UtilTrimBlanks(spec);
   if (*spec == '\0') return;

   if (SetTickMarkSizeValue(spec)) {
      sprintf(gszMsgBox, TgLoadString(STID_TICK_MARK_SIZE_SET_TO_INT),
            tickMarkSize);
      Msg(gszMsgBox);
   }
}
