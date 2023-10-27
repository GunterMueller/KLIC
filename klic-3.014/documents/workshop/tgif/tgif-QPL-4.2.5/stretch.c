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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/stretch.c,v 1.63 2011/05/16 16:21:59 william Exp $
 */

#define _INCLUDE_FROM_STRETCH_C_

#include "tgifdefs.h"
#include "cmdids.h"

#include "align.e"
#include "arc.e"
#include "auxtext.e"
#include "choice.e"
#include "cmd.e"
#include "color.e"
#include "cursor.e"
#include "dialog.e"
#include "drawing.e"
#include "dup.e"
#include "exec.e"
#include "file.e"
#include "font.e"
#include "grid.e"
#include "mainloop.e"
#include "mainmenu.e"
#include "mark.e"
#include "menu.e"
#include "menuinfo.e"
#include "miniline.e"
#include "move.e"
#include "msg.e"
#include "navigate.e"
#include "obj.e"
#include "page.e"
#include "poly.e"
#include "raster.e"
#include "rect.e"
#include "ruler.e"
#include "scroll.e"
#include "select.e"
#include "setup.e"
#include "spline.e"
#include "stretch.e"
#include "strtbl.e"
#include "text.e"
#include "util.e"
#include "xbitmap.e"
#include "xpixmap.e"

int stretchableText=FALSE;
int compoundObjWithTextStretchableForPSE=TRUE;
int rotationIncrement=(45<<6); /* degrees*64 */

int autoRotatePivot=FALSE;
int rotatePivotAbsXYValid=FALSE;
int rotatePivotAbsX=0;
int rotatePivotAbsY=0;

static int stretchingEverything=FALSE;

static
int PtIn4Corners(XOff, YOff, BBox, Corner)
   int XOff, YOff, * Corner;
   struct BBRec BBox;
{
   if (PtInMark(XOff, YOff, OFFSET_X(BBox.ltx), OFFSET_Y(BBox.lty))) {
      *Corner = 1;
      return TRUE;
   }
   if (PtInMark(XOff, YOff, OFFSET_X(BBox.ltx), OFFSET_Y(BBox.rby))) {
      *Corner = 7;
      return TRUE;
   }
   if (PtInMark(XOff, YOff, OFFSET_X(BBox.rbx), OFFSET_Y(BBox.lty))) {
      *Corner = 3;
      return TRUE;
   }
   if (PtInMark(XOff, YOff, OFFSET_X(BBox.rbx), OFFSET_Y(BBox.rby))) {
      *Corner = 5;
      return TRUE;
   }
   return FALSE;
}

static
int PtIn8Places(XOff, YOff, BBox, Corner)
   int XOff, YOff, *Corner;
   struct BBRec BBox;
{
   register int xmid, ymid;

   if (PtIn4Corners(XOff, YOff, BBox, Corner)) return TRUE;

   xmid = ((BBox.ltx+BBox.rbx)>>1);
   if (PtInMark(XOff, YOff, OFFSET_X(xmid), OFFSET_Y(BBox.lty))) {
      *Corner = 2;
      return TRUE;
   }
   if (PtInMark(XOff, YOff, OFFSET_X(xmid), OFFSET_Y(BBox.rby))) {
      *Corner = 6;
      return TRUE;
   }
   ymid = ((BBox.lty+BBox.rby)>>1);
   if (PtInMark(XOff, YOff, OFFSET_X(BBox.ltx), OFFSET_Y(ymid))) {
      *Corner = 8;
      return TRUE;
   }
   if (PtInMark(XOff, YOff, OFFSET_X(BBox.rbx), OFFSET_Y(ymid))) {
      *Corner = 4;
      return TRUE;
   }
   return FALSE;
}

int PtInPolyMark(ObjPtr, XOff, YOff, NumPts, V, Index)
   struct ObjRec *ObjPtr;
   int XOff, YOff, NumPts, * Index;
   IntPoint *V;
{
   int i;

   if (ObjPtr->ctm == NULL) {
      for (i = 0; i < NumPts; i++) {
         if (PtInMark(XOff, YOff, OFFSET_X(V[i].x), OFFSET_Y(V[i].y))) {
            *Index = i;
            return TRUE;
         }
      }
   } else {
      for (i = 0; i < NumPts; i++) {
         int x, y;

         TransformPointThroughCTM(V[i].x-ObjPtr->x, V[i].y-ObjPtr->y,
               ObjPtr->ctm, &x, &y);
         if (PtInMark(XOff, YOff, OFFSET_X(x+ObjPtr->x),
               OFFSET_Y(y+ObjPtr->y))) {
            *Index = i;
            return TRUE;
         }
      }
   }
   return FALSE;
}

int RetractedArrowAttr(obj_ptr)
   struct ObjRec *obj_ptr;
{
   register struct AttrRec *attr_ptr;

   if (obj_ptr->type != OBJ_POLY) return FALSE;

   if (obj_ptr->detail.p->n <= 2) return FALSE;
   for (attr_ptr=obj_ptr->lattr; attr_ptr!=NULL; attr_ptr=attr_ptr->prev) {
      if (*attr_ptr->attr_name.s=='\0' && strcmp(attr_ptr->attr_value.s,
            "retracted_arrows")==0) {
         return TRUE;
      }
   }
   return FALSE;
}

int AutoRetractedArrowAttr(obj_ptr, check_v_count)
   /* if check_v_count == TRUE:  return FALSE if poly_ptr->n != 3 */
   /* if check_v_count == FALSE: skip the poly_ptr->n check */
   struct ObjRec *obj_ptr;
   int check_v_count;
{
   register struct AttrRec *attr_ptr;

   if (obj_ptr->type == OBJ_POLY) {
      struct PolyRec *poly_ptr=obj_ptr->detail.p;

      if (poly_ptr->style == LS_PLAIN || poly_ptr->style == LS_DOUBLE ||
            (check_v_count && poly_ptr->n != 3)) {
         return FALSE;
      }
   } else {
      return FALSE;
   }
   for (attr_ptr=obj_ptr->lattr; attr_ptr!=NULL; attr_ptr=attr_ptr->prev) {
      if (*attr_ptr->attr_name.s=='\0' &&
            strcmp(attr_ptr->attr_value.s, "auto_retracted_arrows")==0) {
         return TRUE;
      }
   }
   return FALSE;
}

int AutoCenterAttr(obj_ptr)
   struct ObjRec *obj_ptr;
{
   register struct AttrRec *attr_ptr;

   for (attr_ptr=obj_ptr->lattr; attr_ptr!=NULL; attr_ptr=attr_ptr->prev) {
      if (*attr_ptr->attr_name.s=='\0' && strcmp(attr_ptr->attr_value.s,
            "auto_center_attr")==0) {
         return TRUE;
      }
   }
   return FALSE;
}

void CenterObjInOBBox(TextObjPtr, OBBox, BBoxReturn)
   struct ObjRec *TextObjPtr;
   struct BBRec OBBox, *BBoxReturn;
{
   int text_w, text_h, bbox_w, bbox_h, dx, dy;

   if (BBoxReturn != NULL) {
      BBoxReturn->ltx = TextObjPtr->bbox.ltx;
      BBoxReturn->lty = TextObjPtr->bbox.lty;
      BBoxReturn->rbx = TextObjPtr->bbox.rbx;
      BBoxReturn->rby = TextObjPtr->bbox.rby;
   }
   text_w = TextObjPtr->obbox.rbx-TextObjPtr->obbox.ltx;
   text_h = TextObjPtr->obbox.rby-TextObjPtr->obbox.lty;
   bbox_w = OBBox.rbx - OBBox.ltx;
   bbox_h = OBBox.rby - OBBox.lty;
   if (text_w > bbox_w) {
      dx = OBBox.ltx-((text_w-bbox_w)>>1)-TextObjPtr->obbox.ltx;
   } else {
      dx = OBBox.ltx+((bbox_w-text_w)>>1)-TextObjPtr->obbox.ltx;
   }
   if (text_h > bbox_h) {
      dy = OBBox.lty-((text_h-bbox_h)>>1)-TextObjPtr->obbox.lty;
   } else {
      dy = OBBox.lty+((bbox_h-text_h)>>1)-TextObjPtr->obbox.lty;
   }
   MoveObj(TextObjPtr, dx, dy);
   if (BBoxReturn != NULL) {
      if (TextObjPtr->bbox.ltx < BBoxReturn->ltx) {
         BBoxReturn->ltx = TextObjPtr->bbox.ltx;
      }
      if (TextObjPtr->bbox.lty < BBoxReturn->lty) {
         BBoxReturn->lty = TextObjPtr->bbox.lty;
      }
      if (TextObjPtr->bbox.rbx > BBoxReturn->rbx) {
         BBoxReturn->rbx = TextObjPtr->bbox.rbx;
      }
      if (TextObjPtr->bbox.rby > BBoxReturn->rby) {
         BBoxReturn->rby = TextObjPtr->bbox.rby;
      }
   }
}

struct SelRec *PtInSelMark(XOff, YOff, Corner)
   int XOff, YOff, *Corner;
   /* XOff and YOff are screen offsets */
   /* 1 2 3 */
   /* 8   4 */
   /* 7 6 5 */
{
   register struct SelRec *sel_ptr;
   register struct ObjRec *obj_ptr;

   for (sel_ptr=topSel; sel_ptr != NULL; sel_ptr=sel_ptr->next) {
      obj_ptr = sel_ptr->obj;

      switch (obj_ptr->type) {
      case OBJ_POLY:
         if (obj_ptr->detail.p->curved == LT_STRUCT_SPLINE) {
            if (PtInPolyMark(obj_ptr, XOff, YOff, obj_ptr->detail.p->ssn,
                  obj_ptr->detail.p->ssvlist, Corner)) {
               return sel_ptr;
            }
         } else {
            if (PtInPolyMark(obj_ptr, XOff, YOff, obj_ptr->detail.p->n,
                  obj_ptr->detail.p->vlist, Corner)) {
               return sel_ptr;
            }
         }
         break;
      case OBJ_POLYGON:
         if (obj_ptr->detail.g->curved == LT_STRUCT_SPLINE) {
            if (PtInPolyMark(obj_ptr, XOff, YOff, obj_ptr->detail.g->ssn-1,
                  obj_ptr->detail.g->ssvlist, Corner)) {
               return sel_ptr;
            }
         } else {
            if (PtInPolyMark(obj_ptr, XOff, YOff, obj_ptr->detail.g->n-1,
                  obj_ptr->detail.g->vlist, Corner)) {
               return sel_ptr;
            }
         }
         break;
      case OBJ_BOX:
      case OBJ_GROUP:
      case OBJ_ICON:
      case OBJ_SYM:
      case OBJ_PIN:
      case OBJ_OVAL:
      case OBJ_ARC:
      case OBJ_RCBOX:
      case OBJ_XBM:
      case OBJ_XPM:
         if (PtIn8Places(XOff, YOff, obj_ptr->obbox, Corner)) {
            return sel_ptr;
         }
         break;
      case OBJ_TEXT:
         if (curChoice == ROTATEMODE) {
            if (PtIn8Places(XOff, YOff, obj_ptr->obbox, Corner)) {
               return sel_ptr;
            }
         } else if (curChoice == NOTHING) {
            if (stretchableText &&
                  PtIn8Places(XOff, YOff, obj_ptr->obbox, Corner)) {
               return sel_ptr;
            }
         }
         break;
      }
   }
   return NULL;
}

int HasOnReshape(obj_ptr, pp_name_attr)
   struct ObjRec *obj_ptr;
   struct AttrRec **pp_name_attr;
   /* returns the name attribute pointer */
{
   if (pp_name_attr != NULL) *pp_name_attr = NULL;
   if ((obj_ptr->type == OBJ_POLY || obj_ptr->type == OBJ_POLYGON) &&
         obj_ptr->ctm == NULL) {
      if (FindAttrWithName(obj_ptr, "on_reshape=", NULL) != NULL) {
         struct AttrRec *attr=FindAttrWithName(obj_ptr, "name=", NULL);

         if (attr != NULL && *attr->attr_value.s != '\0') {
            if (pp_name_attr != NULL) *pp_name_attr = attr;
            return TRUE;
         }
      }
   }
   return FALSE;
}

static
int SkipOnResize(obj_ptr)
   struct ObjRec *obj_ptr;
{
   if (obj_ptr->type != OBJ_GROUP &&
         obj_ptr->type != OBJ_ICON &&
         obj_ptr->type != OBJ_SYM &&
         obj_ptr->type != OBJ_PIN) {
      if (obj_ptr->ctm != NULL) {
         return TRUE;
      }
   } else {
      struct ObjRec *sub_obj=obj_ptr->detail.r->last;

      for ( ; sub_obj != NULL; sub_obj=sub_obj->prev) {
         if (sub_obj->type != OBJ_GROUP &&
               sub_obj->type != OBJ_ICON &&
               sub_obj->type != OBJ_SYM &&
               sub_obj->type != OBJ_PIN &&
               sub_obj->ctm != NULL) {
            return TRUE;
         }
      }
   }
   return FALSE;
}

static
int HasOnResize(obj_ptr, pp_name_attr)
   struct ObjRec *obj_ptr;
   struct AttrRec **pp_name_attr;
   /* returns the name attribute pointer */
{
   if (pp_name_attr != NULL) *pp_name_attr = NULL;
   if (!SkipOnResize(obj_ptr)) {
      if (FindAttrWithName(obj_ptr, "on_resize=", NULL) != NULL) {
         struct AttrRec *attr=FindAttrWithName(obj_ptr, "name=", NULL);

         if (attr != NULL && *attr->attr_value.s != '\0') {
            if (pp_name_attr != NULL) *pp_name_attr = attr;
            return TRUE;
         }
      }
   }
   return FALSE;
}

static
char **NeedToProcessOnResize(pn_num_to_resize)
   int *pn_num_to_resize;
{
   int num_to_resize=0;
   char **ppsz_names=NULL;
   struct SelRec *sel_ptr=NULL;

   *pn_num_to_resize = 0;
   for (sel_ptr=botSel; sel_ptr != NULL; sel_ptr=sel_ptr->prev) {
      struct AttrRec *name_attr=NULL;

      if (HasOnResize(sel_ptr->obj, &name_attr) && name_attr != NULL) {
         num_to_resize++;
         if (ppsz_names == NULL) {
            ppsz_names = (char**)malloc(sizeof(char*));
         } else {
            ppsz_names = (char**)realloc(ppsz_names,
                  num_to_resize*sizeof(char*));
         }
         if (ppsz_names == NULL) FailAllocMessage();
         ppsz_names[num_to_resize-1] = (char*)(long)(sel_ptr->obj->id);
      }
   }
   if (num_to_resize == 0) return NULL;

   *pn_num_to_resize = num_to_resize;
   return ppsz_names;
}

static
struct ObjRec *FindObjWithOID(oid)
   int oid;
{
   struct ObjRec *obj_ptr=NULL;

   for (obj_ptr=botObj; obj_ptr != NULL; obj_ptr=obj_ptr->prev) {
      if (obj_ptr->id == oid) {
         return obj_ptr;
      }
   }
   return NULL;
}

static
void DoOnResize(ppsz_names, num_to_resize)
   char **ppsz_names;
   int num_to_resize;
{
   int i=0;

   RemoveAllSel();
   for (i=0; i < num_to_resize; i++) {
      struct AttrRec *exec_attr=NULL;
      struct ObjRec *obj_ptr=FindObjWithOID((int)(long)(ppsz_names[i]));

      if (obj_ptr != NULL && !SkipOnResize(obj_ptr)) {
         exec_attr = FindAttrWithName(obj_ptr, "on_resize=", NULL);
      }
      if (exec_attr != NULL) {
         DoExecLoop(obj_ptr, exec_attr);
      } else {
         sprintf(gszMsgBox,
               TgLoadCachedString(CSTID_CANT_FIND_OBJ_NAME_ON_RESIZE),
               ppsz_names[i]);
         Msg(gszMsgBox);
         *ppsz_names[i] = '\0';
      }
   }
   for (i=0; i < num_to_resize; i++) {
      struct ObjRec *obj_ptr=FindObjWithOID((int)(long)(ppsz_names[i]));

      if (obj_ptr != NULL) {
         if (!AlreadySelected(obj_ptr)) {
            AddNewSelObj(obj_ptr);
         }
      }
   }
   free(ppsz_names);

   UpdSelBBox();
}

#define STRETCH_DRAW  (FALSE)
#define STRETCH_ERASE (TRUE)
#define STRETCH_CLICK (FALSE)
#define STRETCH_DRAG  (TRUE)

#define STRETCH_STARTSHOW 0
#define STRETCH_DOSHOW    1
#define STRETCH_ENDSHOW   2

int GetVlistIndexFromStretchStructuredSplineInfo(psssi, ss_index)
   StretchStructuredSplineInfo *psssi;
   int ss_index;
{
   if (psssi->hinge) {
      return psssi->orig_hinge_index;
   } if (psssi->earlier_smooth_selected) {
      return psssi->orig_hinge_index-1;
   } else {
      return psssi->orig_hinge_index+1;
   }
}

static
void DoStretchPolyMeasureCursor(start, num_pts, vs, index, dx, dy, erase, drag,
      obj_type, grid_x, grid_y)
   int start, num_pts, index, dx, dy, erase, drag, obj_type, grid_x, grid_y;
   IntPoint *vs;
{
   static IntPoint prev_pt={0,0}, mid_pt={0,0}, next_pt={0,0};
   char buf[80], x_buf[80], y_buf[80], a_buf[80];
   int x=vs[index].x+dx, y=vs[index].y+dy, angle2=0;

#ifdef _TGIF_DBG /* debug, do not translate */
   TgAssert((obj_type == OBJ_POLY || index != num_pts-1),
         "index == num_pts-1 for a polygon in DoStretchPolyMeasureCursor()",
         NULL);
#endif /* _TGIF_DBG */
   if (index == 0 || index == num_pts-1) {
      if (obj_type == OBJ_POLY) {
         if (num_pts > 2) {
            if (index == 0) {
               prev_pt.x = x;       prev_pt.y = y;
               mid_pt.x = vs[1].x;  mid_pt.y = vs[1].y;
               next_pt.x = vs[2].x; next_pt.y = vs[2].y;
            } else {
               prev_pt.x = vs[num_pts-3].x; prev_pt.y = vs[num_pts-3].y;
               mid_pt.x = vs[num_pts-2].x;  mid_pt.y = vs[num_pts-2].y;
               next_pt.x = x;               next_pt.y = y;
            }
         } else {
            if (index == 0) {
               prev_pt.x = x;           prev_pt.y = y;
               mid_pt.x = vs[1].x;      mid_pt.y = vs[1].y;
               next_pt.x = vs[1].x+100; next_pt.y = vs[1].y;
            } else {
               prev_pt.x = x;           prev_pt.y = y;
               mid_pt.x = vs[0].x;      mid_pt.y = vs[0].y;
               next_pt.x = vs[0].x+100; next_pt.y = vs[0].y;
            }
         }
      } else {
         prev_pt.x = vs[1].x;         prev_pt.y = vs[1].y;
         mid_pt.x = x;                mid_pt.y = y;
         next_pt.x = vs[num_pts-2].x; next_pt.y = vs[num_pts-2].y;
      }
   } else {
      prev_pt.x = vs[index-1].x; prev_pt.y = vs[index-1].y;
      mid_pt.x = x;              mid_pt.y = y;
      next_pt.x = vs[index+1].x; next_pt.y = vs[index+1].y;
   }
   if (num_pts == 2) {
      if (index == 0 && x == mid_pt.x && y == mid_pt.y) {
         strcpy(a_buf, "0");
      } else {
         PointsToArc(mid_pt.x, mid_pt.y, x, y, mid_pt.x+100, mid_pt.y,
               ARC_CCW, FALSE, NULL, NULL, NULL, NULL, NULL, &angle2);
         if (angle2 > 180*64) angle2=(360*64)-angle2;
         FormatAngle(angle2, a_buf);
      }
   } else {
      if ((index == 0 || index == num_pts-1) &&
            (x == mid_pt.x && y == mid_pt.y)) {
         strcpy(a_buf, "0");
      } else if ((index != 0 && index != num_pts-1) &&
            ((x == prev_pt.x && y == prev_pt.y) ||
            (x == next_pt.x && y == next_pt.y))) {
         strcpy(a_buf, "180");
      } else {
         PointsToArc(mid_pt.x, mid_pt.y, prev_pt.x, prev_pt.y, next_pt.x,
               next_pt.y, ARC_CCW, FALSE, NULL, NULL, NULL, NULL, NULL,
               &angle2);
         if (angle2 > 180*64) angle2=(360*64)-angle2;
         FormatAngle(angle2, a_buf);
      }
   }
   PixelToMeasurementUnit(x_buf, x);
   PixelToMeasurementUnit(y_buf, y);
   if (curChoice == FREEHAND) {
      sprintf(buf, "x=%s\ny=%s", x_buf, y_buf);
   } else {
      sprintf(buf, "x=%s\ny=%s\nangle=%s", x_buf, y_buf, a_buf);
   }
   switch (start) {
   case STRETCH_STARTSHOW:
      StartShowMeasureCursor(grid_x, grid_y, buf, TRUE);
      break;
   case STRETCH_DOSHOW: ShowMeasureCursor(grid_x, grid_y, buf, TRUE); break;
   case STRETCH_ENDSHOW: EndShowMeasureCursor(grid_x, grid_y, buf, TRUE); break;
   }
}

static
void DoStretchStructSplineMeasureCursor(start, start_v, dx, dy, grid_x, grid_y)
   int start, dx, dy, grid_x, grid_y;
   IntPoint *start_v;
{
   int x=0, y=0;
   char buf[MAXSTRING], x_buf[80], y_buf[80], dx_buf[80], dy_buf[80];

   x = start_v->x + dx;
   y = start_v->y + dy;
   PixelToMeasurementUnit(x_buf, x);
   PixelToMeasurementUnit(y_buf, y);
   PixelToMeasurementUnit(dx_buf, dx);
   PixelToMeasurementUnit(dy_buf, dy);
   sprintf(buf, "x=%s\ny=%s\ndx=%s\ndy=%s", x_buf, y_buf, dx_buf, dy_buf);

   switch (start) {
   case STRETCH_STARTSHOW:
      StartShowMeasureCursor(grid_x, grid_y, buf, TRUE);
      break;
   case STRETCH_DOSHOW: ShowMeasureCursor(grid_x, grid_y, buf, TRUE); break;
   case STRETCH_ENDSHOW: EndShowMeasureCursor(grid_x, grid_y, buf, TRUE); break;
   }
}

void SetIPTInfoForStretchPoly(index, n, vs, psssi)
   int index, n; /* 0 <= index < ssn */
   IntPoint *vs;
   StretchStructuredSplineInfo *psssi;
{
   int i=0, j=0, k=0, num_hinge_vs=(n+2)/3;

#ifdef _TGIF_DBG /* debug, do not translate */
   TgAssert((n+2)%3 == 0, "invalid n in SetIPTInfoForStretchPoly()", NULL);
#endif /* _TGIF_DBG */

   memset(psssi, 0, sizeof(StretchStructuredSplineInfo));
   for (i=0, j=0, k=0; i < num_hinge_vs; i++, j+=3, k++) {
      int set_the_rest_for_this=FALSE;

      if (i == 0) {
         if (k == index) {
            psssi->hinge = TRUE;
            set_the_rest_for_this = TRUE;
         }
         if (vs[0].x != vs[1].x || vs[0].y != vs[1].y) {
            k++;
            if (k == index) {
               psssi->hinge = FALSE;
               psssi->earlier_smooth_selected = FALSE;
               set_the_rest_for_this = TRUE;
            }
         }
         if (set_the_rest_for_this) {
            psssi->orig_hinge_index = j;
            psssi->prev_valid = FALSE;
            psssi->next_valid = TRUE;
            psssi->ipt.earlier_smooth_pt.x = vs[0].x;
            psssi->ipt.earlier_smooth_pt.y = vs[0].y;
            psssi->ipt.hinge_pt.x = vs[0].x;
            psssi->ipt.hinge_pt.y = vs[0].y;
            psssi->ipt.later_smooth_pt.x = vs[1].x;
            psssi->ipt.later_smooth_pt.y = vs[1].y;
            if (vs[0].x != vs[1].x || vs[0].y != vs[1].y) {
               psssi->ipt.later_valid = TRUE;
            }
            psssi->ipt_next.earlier_smooth_pt.x = vs[2].x;
            psssi->ipt_next.earlier_smooth_pt.y = vs[2].y;
            psssi->ipt_next.hinge_pt.x = vs[3].x;
            psssi->ipt_next.hinge_pt.y = vs[3].y;
            if (num_hinge_vs > 2) {
               psssi->ipt_next.later_smooth_pt.x = vs[4].x;
               psssi->ipt_next.later_smooth_pt.y = vs[4].y;
            } else {
               /* this may not be necessary */
               psssi->ipt_next.later_smooth_pt.x = vs[3].x;
               psssi->ipt_next.later_smooth_pt.y = vs[3].y;
            }
            if (vs[2].x != vs[3].x || vs[2].y != vs[3].y) {
               psssi->ipt_next.earlier_valid = TRUE;
            }
            if (vs[4].x != vs[3].x || vs[4].y != vs[3].y) {
               psssi->ipt_next.later_valid = TRUE;
            }
         }
      } else if (i == num_hinge_vs-1) {
         if (vs[n-1].x != vs[n-2].x || vs[n-1].y != vs[n-2].y) {
            if (k == index) {
               psssi->hinge = FALSE;
               psssi->earlier_smooth_selected = TRUE;
               set_the_rest_for_this = TRUE;
            }
            k++;
         }
         if (k == index) {
            psssi->hinge = TRUE;
            set_the_rest_for_this = TRUE;
         }
         if (set_the_rest_for_this) {
            psssi->orig_hinge_index = j;
            psssi->prev_valid = TRUE;
            psssi->next_valid = FALSE;
            psssi->ipt.later_smooth_pt.x = vs[n-1].x;
            psssi->ipt.later_smooth_pt.y = vs[n-1].y;
            psssi->ipt.hinge_pt.x = vs[n-1].x;
            psssi->ipt.hinge_pt.y = vs[n-1].y;
            psssi->ipt.earlier_smooth_pt.x = vs[n-2].x;
            psssi->ipt.earlier_smooth_pt.y = vs[n-2].y;
            if (vs[n-1].x != vs[n-2].x || vs[n-1].y != vs[n-2].y) {
               psssi->ipt.earlier_valid = TRUE;
            }
            psssi->ipt_prev.later_smooth_pt.x = vs[n-3].x;
            psssi->ipt_prev.later_smooth_pt.y = vs[n-3].y;
            psssi->ipt_prev.hinge_pt.x = vs[n-4].x;
            psssi->ipt_prev.hinge_pt.y = vs[n-4].y;
            if (num_hinge_vs > 2) {
               psssi->ipt_prev.earlier_smooth_pt.x = vs[n-5].x;
               psssi->ipt_prev.earlier_smooth_pt.y = vs[n-5].y;
            } else {
               /* this may not be necessary */
               psssi->ipt_prev.earlier_smooth_pt.x = vs[n-4].x;
               psssi->ipt_prev.earlier_smooth_pt.y = vs[n-4].y;
            }
            if (vs[n-5].x != vs[n-4].x || vs[n-5].y != vs[n-4].y) {
               psssi->ipt_prev.earlier_valid = TRUE;
            }
            if (vs[n-3].x != vs[n-4].x || vs[n-3].y != vs[n-4].y) {
               psssi->ipt_prev.later_valid = TRUE;
            }
         }
      } else {
         if (vs[j-1].x != vs[j].x || vs[j-1].y != vs[j].y) {
            if (k == index) {
               psssi->hinge = FALSE;
               psssi->earlier_smooth_selected = TRUE;
               set_the_rest_for_this = TRUE;
            }
            k++;
         }
         if (k == index) {
            psssi->hinge = TRUE;
            set_the_rest_for_this = TRUE;
         }
         if (vs[j+1].x != vs[j].x || vs[j+1].y != vs[j].y) {
            k++;
            if (k == index) {
               psssi->hinge = FALSE;
               psssi->earlier_smooth_selected = FALSE;
               set_the_rest_for_this = TRUE;
            }
         }
         if (set_the_rest_for_this) {
            psssi->orig_hinge_index = j;
            psssi->prev_valid = TRUE;
            psssi->next_valid = TRUE;
            if (i > 1) {
               psssi->ipt_prev.earlier_smooth_pt.x = vs[j-4].x;
               psssi->ipt_prev.earlier_smooth_pt.y = vs[j-4].y;
               if (vs[j-4].x != vs[j-3].x || vs[j-4].y != vs[j-3].y) {
                  psssi->ipt_prev.earlier_valid = TRUE;
               }
            } else {
               /* this may not be necessary */
               psssi->ipt_prev.earlier_smooth_pt.x = vs[j-3].x;
               psssi->ipt_prev.earlier_smooth_pt.y = vs[j-3].y;
            }
            psssi->ipt_prev.hinge_pt.x = vs[j-3].x;
            psssi->ipt_prev.hinge_pt.y = vs[j-3].y;
            psssi->ipt_prev.later_smooth_pt.x = vs[j-2].x;
            psssi->ipt_prev.later_smooth_pt.y = vs[j-2].y;
            if (vs[j-2].x != vs[j-3].x || vs[j-2].y != vs[j-3].y) {
               psssi->ipt_prev.later_valid = TRUE;
            }
   
            psssi->ipt.earlier_smooth_pt.x = vs[j-1].x;
            psssi->ipt.earlier_smooth_pt.y = vs[j-1].y;
            psssi->ipt.hinge_pt.x = vs[j].x;
            psssi->ipt.hinge_pt.y = vs[j].y;
            psssi->ipt.later_smooth_pt.x = vs[j+1].x;
            psssi->ipt.later_smooth_pt.y = vs[j+1].y;
            if (vs[j-1].x != vs[j].x || vs[j-1].y != vs[j].y) {
               psssi->ipt.earlier_valid = TRUE;
            }
            if (vs[j+1].x != vs[j].x || vs[j+1].y != vs[j].y) {
               psssi->ipt.later_valid = TRUE;
            }
   
            psssi->ipt_next.earlier_smooth_pt.x = vs[j+2].x;
            psssi->ipt_next.earlier_smooth_pt.y = vs[j+2].y;
            psssi->ipt_next.hinge_pt.x = vs[j+3].x;
            psssi->ipt_next.hinge_pt.y = vs[j+3].y;
            if (i < num_hinge_vs-2) {
               psssi->ipt_next.later_smooth_pt.x = vs[j+4].x;
               psssi->ipt_next.later_smooth_pt.y = vs[j+4].y;
               if (vs[j+4].x != vs[j+3].x || vs[j+4].y != vs[j+3].y) {
                  psssi->ipt_next.later_valid = TRUE;
               }
            } else {
               /* this may not be necessary */
               psssi->ipt_next.later_smooth_pt.x = vs[j+3].x;
               psssi->ipt_next.later_smooth_pt.y = vs[j+3].y;
            }
            if (vs[j+2].x != vs[j+3].x || vs[j+2].y != vs[j+3].y) {
               psssi->ipt_next.earlier_valid = TRUE;
            }
         }
      }
   }
}

void SetIPTInfoForStretchPolygon(index, n, vs, psssi)
   int index, n; /* 0 <= index < ssn */
   IntPoint *vs;
   StretchStructuredSplineInfo *psssi;
{
   int i=0, j=0, k=0, num_hinge_vs=(n+2)/3;

#ifdef _TGIF_DBG /* debug, do not translate */
   TgAssert((n+2)%3 == 0, "invalid n in SetIPTInfoForStretchPolygon()", NULL);
#endif /* _TGIF_DBG */

   memset(psssi, 0, sizeof(StretchStructuredSplineInfo));
   psssi->prev_valid = psssi->next_valid = TRUE;
   for (i=0, j=0, k=0; i < num_hinge_vs; i++, j+=3, k++) {
      int set_the_rest_for_this=FALSE;

      if (i == 0) {
         if (k == index) {
            psssi->hinge = TRUE;
            set_the_rest_for_this = TRUE;
         }
         if (vs[0].x != vs[1].x || vs[0].y != vs[1].y) {
            k++;
            if (k == index) {
               psssi->hinge = FALSE;
               psssi->earlier_smooth_selected = FALSE;
               set_the_rest_for_this = TRUE;
            }
         }
         if (set_the_rest_for_this) {
            psssi->orig_hinge_index = j;
            psssi->ipt_prev.earlier_smooth_pt.x = vs[n-5].x;
            psssi->ipt_prev.earlier_smooth_pt.y = vs[n-5].y;
            psssi->ipt_prev.hinge_pt.x = vs[n-4].x;
            psssi->ipt_prev.hinge_pt.y = vs[n-4].y;
            psssi->ipt_prev.later_smooth_pt.x = vs[n-3].x;
            psssi->ipt_prev.later_smooth_pt.y = vs[n-3].y;
            if (vs[n-3].x != vs[n-4].x || vs[n-3].y != vs[n-4].y) {
               psssi->ipt_prev.later_valid = TRUE;
            }
            if (vs[n-5].x != vs[n-4].x || vs[n-5].y != vs[n-4].y) {
               psssi->ipt_prev.earlier_valid = TRUE;
            }

            psssi->ipt.earlier_smooth_pt.x = vs[n-2].x;
            psssi->ipt.earlier_smooth_pt.y = vs[n-2].y;
            psssi->ipt.hinge_pt.x = vs[0].x;
            psssi->ipt.hinge_pt.y = vs[0].y;
            psssi->ipt.later_smooth_pt.x = vs[1].x;
            psssi->ipt.later_smooth_pt.y = vs[1].y;
            if (vs[1].x != vs[0].x || vs[1].y != vs[0].y) {
               psssi->ipt.later_valid = TRUE;
            }
            if (vs[n-2].x != vs[0].x || vs[n-2].y != vs[0].y) {
               psssi->ipt.earlier_valid = TRUE;
            }

            psssi->ipt_next.earlier_smooth_pt.x = vs[2].x;
            psssi->ipt_next.earlier_smooth_pt.y = vs[2].y;
            psssi->ipt_next.hinge_pt.x = vs[3].x;
            psssi->ipt_next.hinge_pt.y = vs[3].y;
            psssi->ipt_next.later_smooth_pt.x = vs[4].x;
            psssi->ipt_next.later_smooth_pt.y = vs[4].y;
            if (vs[4].x != vs[3].x || vs[4].y != vs[3].y) {
               psssi->ipt_next.later_valid = TRUE;
            }
            if (vs[2].x != vs[3].x || vs[2].y != vs[3].y) {
               psssi->ipt_next.earlier_valid = TRUE;
            }
         }
      } else if (i == num_hinge_vs-1) {
         if (vs[n-1].x != vs[n-2].x || vs[n-1].y != vs[n-2].y) {
            if (k == index) {
               psssi->hinge = FALSE;
               psssi->earlier_smooth_selected = TRUE;
               set_the_rest_for_this = TRUE;
            }
            k++;
         }
         if (k == index) {
            psssi->hinge = TRUE;
            set_the_rest_for_this = TRUE;
         }
         if (set_the_rest_for_this) {
            psssi->orig_hinge_index = j;
            psssi->ipt_prev.earlier_smooth_pt.x = vs[n-5].x;
            psssi->ipt_prev.earlier_smooth_pt.y = vs[n-5].y;
            psssi->ipt_prev.hinge_pt.x = vs[n-4].x;
            psssi->ipt_prev.hinge_pt.y = vs[n-4].y;
            psssi->ipt_prev.later_smooth_pt.x = vs[n-3].x;
            psssi->ipt_prev.later_smooth_pt.y = vs[n-3].y;
            if (vs[n-3].x != vs[n-4].x || vs[n-3].y != vs[n-4].y) {
               psssi->ipt_prev.later_valid = TRUE;
            }
            if (vs[n-5].x != vs[n-4].x || vs[n-5].y != vs[n-4].y) {
               psssi->ipt_prev.earlier_valid = TRUE;
            }

            psssi->ipt.earlier_smooth_pt.x = vs[n-2].x;
            psssi->ipt.earlier_smooth_pt.y = vs[n-2].y;
            psssi->ipt.hinge_pt.x = vs[n-1].x;
            psssi->ipt.hinge_pt.y = vs[n-1].y;
            psssi->ipt.later_smooth_pt.x = vs[1].x;
            psssi->ipt.later_smooth_pt.y = vs[1].y;
            if (vs[1].x != vs[n-1].x || vs[1].y != vs[n-1].y) {
               psssi->ipt.later_valid = TRUE;
            }
            if (vs[n-2].x != vs[n-1].x || vs[n-2].y != vs[n-1].y) {
               psssi->ipt.earlier_valid = TRUE;
            }

            psssi->ipt_next.earlier_smooth_pt.x = vs[2].x;
            psssi->ipt_next.earlier_smooth_pt.y = vs[2].y;
            psssi->ipt_next.hinge_pt.x = vs[3].x;
            psssi->ipt_next.hinge_pt.y = vs[3].y;
            psssi->ipt_next.later_smooth_pt.x = vs[4].x;
            psssi->ipt_next.later_smooth_pt.y = vs[4].y;
            if (vs[4].x != vs[3].x || vs[4].y != vs[3].y) {
               psssi->ipt_next.later_valid = TRUE;
            }
            if (vs[2].x != vs[3].x || vs[2].y != vs[3].y) {
               psssi->ipt_next.earlier_valid = TRUE;
            }
         }
      } else {
         if (vs[j-1].x != vs[j].x || vs[j-1].y != vs[j].y) {
            if (k == index) {
               psssi->hinge = FALSE;
               psssi->earlier_smooth_selected = TRUE;
               set_the_rest_for_this = TRUE;
            }
            k++;
         }
         if (k == index) {
            psssi->hinge = TRUE;
            set_the_rest_for_this = TRUE;
         }
         if (vs[j+1].x != vs[j].x || vs[j+1].y != vs[j].y) {
            k++;
            if (k == index) {
               psssi->hinge = FALSE;
               psssi->earlier_smooth_selected = FALSE;
               set_the_rest_for_this = TRUE;
            }
         }
         if (set_the_rest_for_this) {
            psssi->orig_hinge_index = j;
            if (i > 1) {
               psssi->ipt_prev.earlier_smooth_pt.x = vs[j-4].x;
               psssi->ipt_prev.earlier_smooth_pt.y = vs[j-4].y;
               if (vs[j-4].x != vs[j-3].x || vs[j-4].y != vs[j-3].y) {
                  psssi->ipt_prev.earlier_valid = TRUE;
               }
            } else {
               psssi->ipt_prev.earlier_smooth_pt.x = vs[n-2].x;
               psssi->ipt_prev.earlier_smooth_pt.y = vs[n-2].y;
               if (vs[n-2].x != vs[j-3].x || vs[n-2].y != vs[j-3].y) {
                  psssi->ipt_prev.earlier_valid = TRUE;
               }
            }
            psssi->ipt_prev.hinge_pt.x = vs[j-3].x;
            psssi->ipt_prev.hinge_pt.y = vs[j-3].y;
            psssi->ipt_prev.later_smooth_pt.x = vs[j-2].x;
            psssi->ipt_prev.later_smooth_pt.y = vs[j-2].y;
            if (vs[j-2].x != vs[j-3].x || vs[j-2].y != vs[j-3].y) {
               psssi->ipt_prev.later_valid = TRUE;
            }
   
            psssi->ipt.earlier_smooth_pt.x = vs[j-1].x;
            psssi->ipt.earlier_smooth_pt.y = vs[j-1].y;
            psssi->ipt.hinge_pt.x = vs[j].x;
            psssi->ipt.hinge_pt.y = vs[j].y;
            psssi->ipt.later_smooth_pt.x = vs[j+1].x;
            psssi->ipt.later_smooth_pt.y = vs[j+1].y;
            if (vs[j-1].x != vs[j].x || vs[j-1].y != vs[j].y) {
               psssi->ipt.earlier_valid = TRUE;
            }
            if (vs[j+1].x != vs[j].x || vs[j+1].y != vs[j].y) {
               psssi->ipt.later_valid = TRUE;
            }
   
            psssi->ipt_next.earlier_smooth_pt.x = vs[j+2].x;
            psssi->ipt_next.earlier_smooth_pt.y = vs[j+2].y;
            psssi->ipt_next.hinge_pt.x = vs[j+3].x;
            psssi->ipt_next.hinge_pt.y = vs[j+3].y;
            if (i < num_hinge_vs-2) {
               psssi->ipt_next.later_smooth_pt.x = vs[j+4].x;
               psssi->ipt_next.later_smooth_pt.y = vs[j+4].y;
               if (vs[j+4].x != vs[j+3].x || vs[j+4].y != vs[j+3].y) {
                  psssi->ipt_next.earlier_valid = TRUE;
               }
   	 } else {
               psssi->ipt_next.later_smooth_pt.x = vs[1].x;
               psssi->ipt_next.later_smooth_pt.y = vs[1].y;
               if (vs[1].x != vs[j+3].x || vs[1].y != vs[j+3].y) {
                  psssi->ipt_next.earlier_valid = TRUE;
               }
            }
            if (vs[j+2].x != vs[j+3].x || vs[j+2].y != vs[j+3].y) {
               psssi->ipt_next.earlier_valid = TRUE;
            }
         }
      }
   }
}

static
void SetVsAndVs2ForHinge(psssi, dx, dy, pn_num_vs, vs, pn_num_vs2, vs2)
   StretchStructuredSplineInfo *psssi;
   int dx, dy, *pn_num_vs, *pn_num_vs2;
   IntPoint *vs, *vs2;
{
   int num_vs=0, num_vs2=0;

   if (!psssi->prev_valid) {
      /* first poly point */
      num_vs = 0;
      vs2[0].x = psssi->ipt.hinge_pt.x + dx;
      vs2[0].y = psssi->ipt.hinge_pt.y + dy;
      if (psssi->ipt.later_valid) {
         vs2[1].x = psssi->ipt.later_smooth_pt.x + dx;
         vs2[1].y = psssi->ipt.later_smooth_pt.y + dy;
         if (psssi->ipt_next.earlier_valid) {
            num_vs2 = 4;
            vs2[2].x = psssi->ipt_next.earlier_smooth_pt.x;
            vs2[2].y = psssi->ipt_next.earlier_smooth_pt.y;
         } else {
            num_vs2 = 3;
         }
      } else {
         if (psssi->ipt_next.earlier_valid) {
            num_vs2 = 3;
            vs2[1].x = psssi->ipt_next.earlier_smooth_pt.x;
            vs2[1].y = psssi->ipt_next.earlier_smooth_pt.y;
         } else {
            num_vs2 = 2;
         }
      }
      vs2[num_vs2-1].x = psssi->ipt_next.hinge_pt.x;
      vs2[num_vs2-1].y = psssi->ipt_next.hinge_pt.y;
   } else if (!psssi->next_valid) {
      /* last poly point */
      num_vs2 = 0;
      vs[0].x = psssi->ipt.hinge_pt.x + dx;
      vs[0].y = psssi->ipt.hinge_pt.y + dy;
      if (psssi->ipt.earlier_valid) {
         vs[1].x = psssi->ipt.earlier_smooth_pt.x + dx;
         vs[1].y = psssi->ipt.earlier_smooth_pt.y + dy;
         if (psssi->ipt_prev.later_valid) {
            num_vs = 4;
            vs[2].x = psssi->ipt_prev.later_smooth_pt.x;
            vs[2].y = psssi->ipt_prev.later_smooth_pt.y;
         } else {
            num_vs = 3;
         }
      } else {
         if (psssi->ipt_prev.later_valid) {
            num_vs = 3;
            vs[1].x = psssi->ipt_prev.later_smooth_pt.x;
            vs[1].y = psssi->ipt_prev.later_smooth_pt.y;
         } else {
            num_vs = 2;
         }
      }
      vs[num_vs-1].x = psssi->ipt_prev.hinge_pt.x;
      vs[num_vs-1].y = psssi->ipt_prev.hinge_pt.y;
   } else {
      /* set the vs */
      vs[0].x = psssi->ipt.hinge_pt.x + dx;
      vs[0].y = psssi->ipt.hinge_pt.y + dy;
      if (psssi->ipt.earlier_valid) {
         vs[1].x = psssi->ipt.earlier_smooth_pt.x + dx;
         vs[1].y = psssi->ipt.earlier_smooth_pt.y + dy;
         if (psssi->ipt_prev.later_valid) {
            num_vs = 4;
            vs[2].x = psssi->ipt_prev.later_smooth_pt.x;
            vs[2].y = psssi->ipt_prev.later_smooth_pt.y;
         } else {
            num_vs = 3;
         }
      } else {
         if (psssi->ipt_prev.later_valid) {
            num_vs = 3;
            vs[1].x = psssi->ipt_prev.later_smooth_pt.x;
            vs[1].y = psssi->ipt_prev.later_smooth_pt.y;
         } else {
            num_vs = 2;
         }
      }
      vs[num_vs-1].x = psssi->ipt_prev.hinge_pt.x;
      vs[num_vs-1].y = psssi->ipt_prev.hinge_pt.y;

      /* set the vs2 */
      vs2[0].x = psssi->ipt.hinge_pt.x + dx;
      vs2[0].y = psssi->ipt.hinge_pt.y + dy;
      if (psssi->ipt.later_valid) {
         vs2[1].x = psssi->ipt.later_smooth_pt.x + dx;
         vs2[1].y = psssi->ipt.later_smooth_pt.y + dy;
         if (psssi->ipt_next.earlier_valid) {
            num_vs2 = 4;
            vs2[2].x = psssi->ipt_next.earlier_smooth_pt.x;
            vs2[2].y = psssi->ipt_next.earlier_smooth_pt.y;
         } else {
            num_vs2 = 3;
         }
      } else {
         if (psssi->ipt_next.earlier_valid) {
            num_vs2 = 3;
            vs2[1].x = psssi->ipt_next.earlier_smooth_pt.x;
            vs2[1].y = psssi->ipt_next.earlier_smooth_pt.y;
         } else {
            num_vs2 = 2;
         }
      }
      vs2[num_vs2-1].x = psssi->ipt_next.hinge_pt.x;
      vs2[num_vs2-1].y = psssi->ipt_next.hinge_pt.y;
   }
   *pn_num_vs = num_vs;
   *pn_num_vs2 = num_vs2;
}

static
void SetVsAndVs2ForSmooth(psssi, dx, dy, pn_num_vs, vs, pn_num_vs2, vs2)
   StretchStructuredSplineInfo *psssi;
   int dx, dy, *pn_num_vs, *pn_num_vs2;
   IntPoint *vs, *vs2;
{
   int num_vs=0, num_vs2=0;

   if (!psssi->prev_valid) {
      /* first poly point */
      num_vs = 0;
      vs2[0].x = psssi->ipt.hinge_pt.x;
      vs2[0].y = psssi->ipt.hinge_pt.y;
#ifdef _TGIF_DBG /* debug, do not translate */
      TgAssert(psssi->ipt.later_valid,
            "psssi->ipt.later_valid is FALSE in SetVsAndVs2ForSmooth()", NULL);
#endif /* _TGIF_DBG */
      vs2[1].x = psssi->ipt.later_smooth_pt.x + dx;
      vs2[1].y = psssi->ipt.later_smooth_pt.y + dy;
      if (psssi->ipt_next.earlier_valid) {
         num_vs2 = 4;
         vs2[2].x = psssi->ipt_next.earlier_smooth_pt.x;
         vs2[2].y = psssi->ipt_next.earlier_smooth_pt.y;
      } else {
         num_vs2 = 3;
      }
      vs2[num_vs2-1].x = psssi->ipt_next.hinge_pt.x;
      vs2[num_vs2-1].y = psssi->ipt_next.hinge_pt.y;
   } else if (!psssi->next_valid) {
      /* last poly point */
      num_vs2 = 0;
      vs[0].x = psssi->ipt.hinge_pt.x;
      vs[0].y = psssi->ipt.hinge_pt.y;
#ifdef _TGIF_DBG /* debug, do not translate */
      TgAssert(psssi->ipt.earlier_valid,
            "psssi->ipt.earlier_valid is FALSE in SetVsAndVs2ForSmooth()",
            NULL);
#endif /* _TGIF_DBG */
      vs[1].x = psssi->ipt.earlier_smooth_pt.x + dx;
      vs[1].y = psssi->ipt.earlier_smooth_pt.y + dy;
      if (psssi->ipt_prev.later_valid) {
         num_vs = 4;
         vs[2].x = psssi->ipt_prev.later_smooth_pt.x;
         vs[2].y = psssi->ipt_prev.later_smooth_pt.y;
      } else {
         num_vs = 3;
      }
      vs[num_vs-1].x = psssi->ipt_prev.hinge_pt.x;
      vs[num_vs-1].y = psssi->ipt_prev.hinge_pt.y;
   } else {
      if (psssi->earlier_smooth_selected) {
#ifdef _TGIF_DBG /* debug, do not translate */
         TgAssert(psssi->ipt.earlier_valid,
               "psssi->ipt.earlier_valid is FALSE with psssi->earlier_smooth_selected in SetVsAndVs2ForSmooth()",
               NULL);
#endif /* _TGIF_DBG */
         /* set the vs */
         vs[0].x = psssi->ipt.hinge_pt.x;
         vs[0].y = psssi->ipt.hinge_pt.y;
         vs[1].x = psssi->ipt.earlier_smooth_pt.x + dx;
         vs[1].y = psssi->ipt.earlier_smooth_pt.y + dy;
         if (psssi->ipt_prev.later_valid) {
            num_vs = 4;
            vs[2].x = psssi->ipt_prev.later_smooth_pt.x;
            vs[2].y = psssi->ipt_prev.later_smooth_pt.y;
         } else {
            num_vs = 3;
         }
         vs[num_vs-1].x = psssi->ipt_prev.hinge_pt.x;
         vs[num_vs-1].y = psssi->ipt_prev.hinge_pt.y;

         /* set the vs2 */
         vs2[0].x = psssi->ipt.hinge_pt.x;
         vs2[0].y = psssi->ipt.hinge_pt.y;
         if (psssi->ipt.later_valid) {
            vs2[1].x = ((psssi->ipt.hinge_pt.x) << 1) -
                  psssi->ipt.earlier_smooth_pt.x - dx;
            vs2[1].y = ((psssi->ipt.hinge_pt.y) << 1) -
                  psssi->ipt.earlier_smooth_pt.y - dy;
            if (psssi->ipt_next.earlier_valid) {
               num_vs2 = 4;
               vs2[2].x = psssi->ipt_next.earlier_smooth_pt.x;
               vs2[2].y = psssi->ipt_next.earlier_smooth_pt.y;
            } else {
               num_vs2 = 3;
            }
         } else {
            if (psssi->ipt_next.earlier_valid) {
               num_vs2 = 3;
               vs2[1].x = psssi->ipt_next.earlier_smooth_pt.x;
               vs2[1].y = psssi->ipt_next.earlier_smooth_pt.y;
            } else {
               num_vs2 = 2;
            }
         }
         vs2[num_vs2-1].x = psssi->ipt_next.hinge_pt.x;
         vs2[num_vs2-1].y = psssi->ipt_next.hinge_pt.y;
      } else {
         /* set the vs */
         vs[0].x = psssi->ipt.hinge_pt.x;
         vs[0].y = psssi->ipt.hinge_pt.y;
         if (psssi->ipt.earlier_valid) {
            vs[1].x = ((psssi->ipt.hinge_pt.x) << 1) -
                  psssi->ipt.later_smooth_pt.x - dx;
            vs[1].y = ((psssi->ipt.hinge_pt.y) << 1) -
                  psssi->ipt.later_smooth_pt.y - dy;
            if (psssi->ipt_prev.later_valid) {
               num_vs = 4;
               vs[2].x = psssi->ipt_prev.later_smooth_pt.x;
               vs[2].y = psssi->ipt_prev.later_smooth_pt.y;
            } else {
               num_vs = 3;
            }
         } else {
            if (psssi->ipt_prev.later_valid) {
               num_vs = 3;
               vs[1].x = psssi->ipt_prev.later_smooth_pt.x;
               vs[1].y = psssi->ipt_prev.later_smooth_pt.y;
            } else {
               num_vs = 2;
            }
         }
         vs[num_vs-1].x = psssi->ipt_prev.hinge_pt.x;
         vs[num_vs-1].y = psssi->ipt_prev.hinge_pt.y;

#ifdef _TGIF_DBG /* debug, do not translate */
         TgAssert(psssi->ipt.later_valid,
               "psssi->ipt.later_valid is FALSE with !psssi->earlier_smooth_selected in SetVsAndVs2ForSmooth()",
               NULL);
#endif /* _TGIF_DBG */
         /* set the vs2 */
         vs2[0].x = psssi->ipt.hinge_pt.x;
         vs2[0].y = psssi->ipt.hinge_pt.y;
         vs2[1].x = psssi->ipt.later_smooth_pt.x + dx;
         vs2[1].y = psssi->ipt.later_smooth_pt.y + dy;
         if (psssi->ipt_next.earlier_valid) {
            num_vs2 = 4;
            vs2[2].x = psssi->ipt_next.earlier_smooth_pt.x;
            vs2[2].y = psssi->ipt_next.earlier_smooth_pt.y;
         } else {
            num_vs2 = 3;
         }
         vs2[num_vs2-1].x = psssi->ipt_next.hinge_pt.x;
         vs2[num_vs2-1].y = psssi->ipt_next.hinge_pt.y;
      }
   }
   *pn_num_vs = num_vs;
   *pn_num_vs2 = num_vs2;
}

void SetVsAndVs2ForStretchStructSpline(psssi, dx, dy, pn_num_vs, vs, pn_num_vs2,
      vs2)
   StretchStructuredSplineInfo *psssi;
   int dx, dy, *pn_num_vs, *pn_num_vs2;
   IntPoint *vs, *vs2;
{
   if (psssi->hinge) {
      SetVsAndVs2ForHinge(psssi, dx, dy, pn_num_vs, vs, pn_num_vs2, vs2);
   } else {
      SetVsAndVs2ForSmooth(psssi, dx, dy, pn_num_vs, vs, pn_num_vs2, vs2);
   }
}

void FixUpSmoothAndSmooth2ForStretchStructSpline(num_vs, smooth, num_vs2,
      smooth2)
   int num_vs, num_vs2;
   char *smooth, *smooth2;
{
   int i=0;

   smooth[0] = FALSE;
   smooth[num_vs-1] = FALSE;
   for (i=1; i < num_vs-1; i++) smooth[i] = TRUE;

   smooth2[0] = FALSE;
   smooth2[num_vs2-1] = FALSE;
   for (i=1; i < num_vs2-1; i++) smooth2[i] = TRUE;
}

void EraseHighLightForHinge(psssi, dx, dy, draw_dashed_line, draw_vertices)
   StretchStructuredSplineInfo *psssi;
   int dx, dy, draw_dashed_line, draw_vertices;
{
   int x=0, y=0;
   XPoint vs[2];
   XGCValues values;

   if (draw_dashed_line) {
      XSetDashes(mainDisplay, revDefaultGC, 0, dashList[8], dashListLength[8]);
   }
   if (!psssi->prev_valid) {
      /* first poly point */
      x = psssi->ipt.hinge_pt.x + dx;
      y = psssi->ipt.hinge_pt.y + dy;
      if (draw_vertices) {
         MARKHR(drawWindow, revDefaultGC, OFFSET_X(x), OFFSET_Y(y));
      }
      vs[0].x = OFFSET_X(x);
      vs[0].y = OFFSET_Y(y);
      if (psssi->ipt.later_valid) {
         x = psssi->ipt.later_smooth_pt.x + dx;
         y = psssi->ipt.later_smooth_pt.y + dy;
         if (draw_vertices) {
            MARKHO(drawWindow, revDefaultGC, OFFSET_X(x), OFFSET_Y(y));
         }
         vs[1].x = OFFSET_X(x);
         vs[1].y = OFFSET_Y(y);
         if (draw_dashed_line) {
            values.line_style = LineOnOffDash;
            XChangeGC(mainDisplay, revDefaultGC, GCLineStyle, &values);
            MyDashedLine(drawWindow, revDefaultGC, vs, 2);
            values.line_style = LineSolid;
            XChangeGC(mainDisplay, revDefaultGC, GCLineStyle, &values);
         }
      }
   } else if (!psssi->next_valid) {
      /* last poly point */
      x = psssi->ipt.hinge_pt.x + dx;
      y = psssi->ipt.hinge_pt.y + dy;
      if (draw_vertices) {
         MARKHR(drawWindow, revDefaultGC, OFFSET_X(x), OFFSET_Y(y));
      }
      vs[0].x = OFFSET_X(x);
      vs[0].y = OFFSET_Y(y);
      if (psssi->ipt.earlier_valid) {
         x = psssi->ipt.earlier_smooth_pt.x + dx;
         y = psssi->ipt.earlier_smooth_pt.y + dy;
         if (draw_vertices) {
            MARKHO(drawWindow, revDefaultGC, OFFSET_X(x), OFFSET_Y(y));
         }
         vs[1].x = OFFSET_X(x);
         vs[1].y = OFFSET_Y(y);
         if (draw_dashed_line) {
            values.line_style = LineOnOffDash;
            XChangeGC(mainDisplay, revDefaultGC, GCLineStyle, &values);
            MyDashedLine(drawWindow, revDefaultGC, vs, 2);
            values.line_style = LineSolid;
            XChangeGC(mainDisplay, revDefaultGC, GCLineStyle, &values);
         }
      }
   } else {
      x = psssi->ipt.hinge_pt.x + dx;
      y = psssi->ipt.hinge_pt.y + dy;
      if (draw_vertices) {
         MARKHR(drawWindow, revDefaultGC, OFFSET_X(x), OFFSET_Y(y));
      }
      vs[0].x = OFFSET_X(x);
      vs[0].y = OFFSET_Y(y);
      if (psssi->ipt.later_valid) {
         x = psssi->ipt.later_smooth_pt.x + dx;
         y = psssi->ipt.later_smooth_pt.y + dy;
         if (draw_vertices) {
            MARKHO(drawWindow, revDefaultGC, OFFSET_X(x), OFFSET_Y(y));
         }
         vs[1].x = OFFSET_X(x);
         vs[1].y = OFFSET_Y(y);
         if (draw_dashed_line) {
            values.line_style = LineOnOffDash;
            XChangeGC(mainDisplay, revDefaultGC, GCLineStyle, &values);
            MyDashedLine(drawWindow, revDefaultGC, vs, 2);
            values.line_style = LineSolid;
            XChangeGC(mainDisplay, revDefaultGC, GCLineStyle, &values);
         }
      }
      if (psssi->ipt.earlier_valid) {
         x = psssi->ipt.earlier_smooth_pt.x + dx;
         y = psssi->ipt.earlier_smooth_pt.y + dy;
         if (draw_vertices) {
            MARKHO(drawWindow, revDefaultGC, OFFSET_X(x), OFFSET_Y(y));
         }
         vs[1].x = OFFSET_X(x);
         vs[1].y = OFFSET_Y(y);
         if (draw_dashed_line) {
            values.line_style = LineOnOffDash;
            XChangeGC(mainDisplay, revDefaultGC, GCLineStyle, &values);
            MyDashedLine(drawWindow, revDefaultGC, vs, 2);
            values.line_style = LineSolid;
            XChangeGC(mainDisplay, revDefaultGC, GCLineStyle, &values);
         }
      }
   }
}

void EraseHighLightForSmooth(psssi, dx, dy, draw_dashed_line, draw_vertices)
   StretchStructuredSplineInfo *psssi;
   int dx, dy, draw_dashed_line, draw_vertices;
{
   int x=0, y=0;
   XPoint vs[2];
   XGCValues values;

   if (draw_dashed_line) {
      XSetDashes(mainDisplay, revDefaultGC, 0, dashList[8], dashListLength[8]);
   }
   if (!psssi->prev_valid) {
      /* first poly point */
      x = psssi->ipt.hinge_pt.x;
      y = psssi->ipt.hinge_pt.y;
      if (draw_vertices) {
         MARKHR(drawWindow, revDefaultGC, OFFSET_X(x), OFFSET_Y(y));
      }
      vs[0].x = OFFSET_X(x);
      vs[0].y = OFFSET_Y(y);
      if (psssi->ipt.later_valid) {
         x = psssi->ipt.later_smooth_pt.x + dx;
         y = psssi->ipt.later_smooth_pt.y + dy;
         if (draw_vertices) {
            MARKHO(drawWindow, revDefaultGC, OFFSET_X(x), OFFSET_Y(y));
         }
         vs[1].x = OFFSET_X(x);
         vs[1].y = OFFSET_Y(y);
         if (draw_dashed_line) {
            values.line_style = LineOnOffDash;
            XChangeGC(mainDisplay, revDefaultGC, GCLineStyle, &values);
            MyDashedLine(drawWindow, revDefaultGC, vs, 2);
            values.line_style = LineSolid;
            XChangeGC(mainDisplay, revDefaultGC, GCLineStyle, &values);
         }
      }
   } else if (!psssi->next_valid) {
      /* last poly point */
      x = psssi->ipt.hinge_pt.x;
      y = psssi->ipt.hinge_pt.y;
      if (draw_vertices) {
         MARKHR(drawWindow, revDefaultGC, OFFSET_X(x), OFFSET_Y(y));
      }
      vs[0].x = OFFSET_X(x);
      vs[0].y = OFFSET_Y(y);
      if (psssi->ipt.earlier_valid) {
         x = psssi->ipt.earlier_smooth_pt.x + dx;
         y = psssi->ipt.earlier_smooth_pt.y + dy;
         if (draw_vertices) {
            MARKHO(drawWindow, revDefaultGC, OFFSET_X(x), OFFSET_Y(y));
         }
         vs[1].x = OFFSET_X(x);
         vs[1].y = OFFSET_Y(y);
         if (draw_dashed_line) {
            values.line_style = LineOnOffDash;
            XChangeGC(mainDisplay, revDefaultGC, GCLineStyle, &values);
            MyDashedLine(drawWindow, revDefaultGC, vs, 2);
            values.line_style = LineSolid;
            XChangeGC(mainDisplay, revDefaultGC, GCLineStyle, &values);
         }
      }
   } else {
      if (psssi->earlier_smooth_selected) {
         x = psssi->ipt.hinge_pt.x;
         y = psssi->ipt.hinge_pt.y;
         if (draw_vertices) {
            MARKHR(drawWindow, revDefaultGC, OFFSET_X(x), OFFSET_Y(y));
         }
         vs[0].x = OFFSET_X(x);
         vs[0].y = OFFSET_Y(y);
         if (psssi->ipt.later_valid) {
            x = ((psssi->ipt.hinge_pt.x) << 1) -
                  psssi->ipt.earlier_smooth_pt.x - dx;
            y = ((psssi->ipt.hinge_pt.y) << 1) -
                  psssi->ipt.earlier_smooth_pt.y - dy;
            if (draw_vertices) {
               MARKHO(drawWindow, revDefaultGC, OFFSET_X(x), OFFSET_Y(y));
            }
            vs[1].x = OFFSET_X(x);
            vs[1].y = OFFSET_Y(y);
            if (draw_dashed_line) {
               values.line_style = LineOnOffDash;
               XChangeGC(mainDisplay, revDefaultGC, GCLineStyle, &values);
               MyDashedLine(drawWindow, revDefaultGC, vs, 2);
               values.line_style = LineSolid;
               XChangeGC(mainDisplay, revDefaultGC, GCLineStyle, &values);
            }
         }
         if (psssi->ipt.earlier_valid) {
            x = psssi->ipt.earlier_smooth_pt.x + dx;
            y = psssi->ipt.earlier_smooth_pt.y + dy;
            if (draw_vertices) {
               MARKHO(drawWindow, revDefaultGC, OFFSET_X(x), OFFSET_Y(y));
            }
            vs[1].x = OFFSET_X(x);
            vs[1].y = OFFSET_Y(y);
            if (draw_dashed_line) {
               values.line_style = LineOnOffDash;
               XChangeGC(mainDisplay, revDefaultGC, GCLineStyle, &values);
               MyDashedLine(drawWindow, revDefaultGC, vs, 2);
               values.line_style = LineSolid;
               XChangeGC(mainDisplay, revDefaultGC, GCLineStyle, &values);
            }
         }
      } else {
         x = psssi->ipt.hinge_pt.x;
         y = psssi->ipt.hinge_pt.y;
         if (draw_vertices) {
            MARKHR(drawWindow, revDefaultGC, OFFSET_X(x), OFFSET_Y(y));
         }
         vs[0].x = OFFSET_X(x);
         vs[0].y = OFFSET_Y(y);
         if (psssi->ipt.later_valid) {
            x = psssi->ipt.later_smooth_pt.x + dx;
            y = psssi->ipt.later_smooth_pt.y + dy;
            if (draw_vertices) {
               MARKHO(drawWindow, revDefaultGC, OFFSET_X(x), OFFSET_Y(y));
            }
            vs[1].x = OFFSET_X(x);
            vs[1].y = OFFSET_Y(y);
            if (draw_dashed_line) {
               values.line_style = LineOnOffDash;
               XChangeGC(mainDisplay, revDefaultGC, GCLineStyle, &values);
               MyDashedLine(drawWindow, revDefaultGC, vs, 2);
               values.line_style = LineSolid;
               XChangeGC(mainDisplay, revDefaultGC, GCLineStyle, &values);
            }
         }
         if (psssi->ipt.earlier_valid) {
            x = ((psssi->ipt.hinge_pt.x) << 1) -
                  psssi->ipt.later_smooth_pt.x - dx;
            y = ((psssi->ipt.hinge_pt.y) << 1) -
                  psssi->ipt.later_smooth_pt.y - dy;
            if (draw_vertices) {
               MARKHO(drawWindow, revDefaultGC, OFFSET_X(x), OFFSET_Y(y));
            }
            vs[1].x = OFFSET_X(x);
            vs[1].y = OFFSET_Y(y);
            if (draw_dashed_line) {
               values.line_style = LineOnOffDash;
               XChangeGC(mainDisplay, revDefaultGC, GCLineStyle, &values);
               MyDashedLine(drawWindow, revDefaultGC, vs, 2);
               values.line_style = LineSolid;
               XChangeGC(mainDisplay, revDefaultGC, GCLineStyle, &values);
            }
         }
      }
   }
}

void EraseHighLightForStretchStructSpline(psssi, dx, dy, draw_dashed_line,
      draw_vertices)
   StretchStructuredSplineInfo *psssi;
   int dx, dy, draw_dashed_line, draw_vertices;
{
   if (psssi->hinge) {
      EraseHighLightForHinge(psssi, dx, dy, draw_dashed_line, draw_vertices);
   } else {
      EraseHighLightForSmooth(psssi, dx, dy, draw_dashed_line, draw_vertices);
   }
}

XPoint *DupVs(pn_return, vs, n)
   int *pn_return, n;
   XPoint *vs;
{
   XPoint *vs_return=(XPoint*)malloc(n*sizeof(XPoint));

   if (vs_return == NULL) FailAllocMessage();
   memcpy(vs_return, vs, n*sizeof(XPoint));
   *pn_return = n;

   return vs_return;
}

void MoveATransformedPoint(obj_ptr, vs, abs_dx, abs_dy)
   struct ObjRec *obj_ptr;
   IntPoint *vs;
   int abs_dx, abs_dy;
{
   int x=0, y=0;

   TransformPointThroughCTM(vs->x-obj_ptr->x, vs->y-obj_ptr->y,
         obj_ptr->ctm, &x, &y);
   x += obj_ptr->x + abs_dx;
   y += obj_ptr->y + abs_dy;
   ReverseTransformPointThroughCTM(x-obj_ptr->x, y-obj_ptr->y,
         obj_ptr->ctm, &x, &y);
   vs->x = x + obj_ptr->x;
   vs->y = y + obj_ptr->y;
}

void UpdateObjForStretchStructSpline(obj_ptr, n, vs, abs_dx, abs_dy, psssi)
   struct ObjRec *obj_ptr;
   int n, abs_dx, abs_dy;
   IntPoint *vs;
   StretchStructuredSplineInfo *psssi;
{
   int index=psssi->orig_hinge_index;

   if (obj_ptr->ctm == NULL) {
      if (psssi->hinge) {
         if (!psssi->prev_valid) {
            /* first poly point */
            vs[0].x += abs_dx;
            vs[0].y += abs_dy;
            vs[1].x += abs_dx;
            vs[1].y += abs_dy;
         } else if (!psssi->next_valid) {
            /* last poly point */
            vs[n-1].x += abs_dx;
            vs[n-1].y += abs_dy;
            vs[n-2].x += abs_dx;
            vs[n-2].y += abs_dy;
         } else {
            if (obj_ptr->type == OBJ_POLYGON && index == 0) {
               vs[0].x += abs_dx;
               vs[0].y += abs_dy;
               vs[1].x += abs_dx;
               vs[1].y += abs_dy;
               /* since it's a polygon */
               vs[n-1].x += abs_dx;
               vs[n-1].y += abs_dy;
               vs[n-2].x += abs_dx;
               vs[n-2].y += abs_dy;
            } else if (obj_ptr->type == OBJ_POLYGON && index == n-1) {
               vs[n-1].x += abs_dx;
               vs[n-1].y += abs_dy;
               vs[n-2].x += abs_dx;
               vs[n-2].y += abs_dy;
               /* since it's a polygon */
               vs[0].x += abs_dx;
               vs[0].y += abs_dy;
               vs[1].x += abs_dx;
               vs[1].y += abs_dy;
            } else {
               vs[index-1].x += abs_dx;
               vs[index-1].y += abs_dy;
               vs[index].x += abs_dx;
               vs[index].y += abs_dy;
               vs[index+1].x += abs_dx;
               vs[index+1].y += abs_dy;
            }
         }
      } else {
         if (!psssi->prev_valid) {
            /* first poly point */
            vs[1].x += abs_dx;
            vs[1].y += abs_dy;
            if (obj_ptr->type == OBJ_POLYGON && index == 0) {
               vs[n-2].x = ((vs[0].x) << 1) - vs[1].x;
               vs[n-2].y = ((vs[0].y) << 1) - vs[1].y;
            }
         } else if (!psssi->next_valid) {
            /* last poly point */
            vs[n-2].x += abs_dx;
            vs[n-2].y += abs_dy;
            if (obj_ptr->type == OBJ_POLYGON && index == n-1) {
               vs[1].x = ((vs[n-1].x) << 1) - vs[n-2].x;
               vs[1].y = ((vs[n-1].y) << 1) - vs[n-2].y;
            }
         } else {
            if (psssi->earlier_smooth_selected) {
               if (obj_ptr->type == OBJ_POLYGON &&
                     (index == 0 || index == n-1)) {
                  vs[n-2].x += abs_dx;
                  vs[n-2].y += abs_dy;
                  /* since it's a polygon */
                  vs[1].x = ((vs[n-1].x) << 1) - vs[n-2].x;
                  vs[1].y = ((vs[n-1].y) << 1) - vs[n-2].y;
               } else {
                  vs[index-1].x += abs_dx;
                  vs[index-1].y += abs_dy;
                  vs[index+1].x = ((vs[index].x) << 1) - vs[index-1].x;
                  vs[index+1].y = ((vs[index].y) << 1) - vs[index-1].y;
               }
            } else {
               if (obj_ptr->type == OBJ_POLYGON &&
                     (index == 0 || index == n-1)) {
                  vs[1].x += abs_dx;
                  vs[1].y += abs_dy;
                  /* since it's a polygon */
                  vs[n-2].x = ((vs[0].x) << 1) - vs[1].x;
                  vs[n-2].y = ((vs[0].y) << 1) - vs[1].y;
               } else {
                  vs[index+1].x += abs_dx;
                  vs[index+1].y += abs_dy;
                  vs[index-1].x = ((vs[index].x) << 1) - vs[index+1].x;
                  vs[index-1].y = ((vs[index].y) << 1) - vs[index+1].y;
               }
            }
         }
      }
   } else {
      if (psssi->hinge) {
         if (!psssi->prev_valid) {
            /* first poly point */
            MoveATransformedPoint(obj_ptr, &vs[0], abs_dx, abs_dy);
            MoveATransformedPoint(obj_ptr, &vs[1], abs_dx, abs_dy);
            if (obj_ptr->type == OBJ_POLYGON && index == 0) {
               MoveATransformedPoint(obj_ptr, &vs[n-1], abs_dx, abs_dy);
               MoveATransformedPoint(obj_ptr, &vs[n-2], abs_dx, abs_dy);
            }
         } else if (!psssi->next_valid) {
            /* last poly point */
            MoveATransformedPoint(obj_ptr, &vs[n-1], abs_dx, abs_dy);
            MoveATransformedPoint(obj_ptr, &vs[n-2], abs_dx, abs_dy);
            if (obj_ptr->type == OBJ_POLYGON &&
                  index == n-1) {
               MoveATransformedPoint(obj_ptr, &vs[0], abs_dx, abs_dy);
               MoveATransformedPoint(obj_ptr, &vs[1], abs_dx, abs_dy);
            }
         } else {
            if (obj_ptr->type == OBJ_POLYGON && index == 0) {
               MoveATransformedPoint(obj_ptr, &vs[0], abs_dx, abs_dy);
               MoveATransformedPoint(obj_ptr, &vs[1], abs_dx, abs_dy);
               /* since it's a polygon */
               MoveATransformedPoint(obj_ptr, &vs[n-1], abs_dx, abs_dy);
               MoveATransformedPoint(obj_ptr, &vs[n-2], abs_dx, abs_dy);
            } else if (obj_ptr->type == OBJ_POLYGON && index == n-1) {
               MoveATransformedPoint(obj_ptr, &vs[n-1], abs_dx, abs_dy);
               MoveATransformedPoint(obj_ptr, &vs[n-2], abs_dx, abs_dy);
               /* since it's a polygon */
               MoveATransformedPoint(obj_ptr, &vs[0], abs_dx, abs_dy);
               MoveATransformedPoint(obj_ptr, &vs[1], abs_dx, abs_dy);
            } else {
               MoveATransformedPoint(obj_ptr, &vs[index-1], abs_dx, abs_dy);
               MoveATransformedPoint(obj_ptr, &vs[index], abs_dx, abs_dy);
               MoveATransformedPoint(obj_ptr, &vs[index+1], abs_dx, abs_dy);
            }
         }
      } else {
         if (!psssi->prev_valid) {
            /* first poly point */
            MoveATransformedPoint(obj_ptr, &vs[1], abs_dx, abs_dy);
            if (obj_ptr->type == OBJ_POLYGON && index == 0) {
               vs[n-2].x = ((vs[0].x) << 1) - vs[1].x;
               vs[n-2].y = ((vs[0].y) << 1) - vs[1].y;
            }
         } else if (!psssi->next_valid) {
            /* last poly point */
            MoveATransformedPoint(obj_ptr, &vs[n-2], abs_dx, abs_dy);
            if (obj_ptr->type == OBJ_POLYGON && index == n-1) {
               vs[1].x = ((vs[n-1].x) << 1) - vs[n-2].x;
               vs[1].y = ((vs[n-1].y) << 1) - vs[n-2].y;
            }
         } else {
            if (psssi->earlier_smooth_selected) {
               if (obj_ptr->type == OBJ_POLYGON &&
                     (index == 0 || index == n-1)) {
                  MoveATransformedPoint(obj_ptr, &vs[n-2], abs_dx, abs_dy);
                  /* since it's a polygon */
                  vs[1].x = ((vs[n-1].x) << 1) - vs[n-2].x;
                  vs[1].y = ((vs[n-1].y) << 1) - vs[n-2].y;
               } else {
                  MoveATransformedPoint(obj_ptr, &vs[index-1], abs_dx, abs_dy);
                  vs[index+1].x = ((vs[index].x) << 1) - vs[index-1].x;
                  vs[index+1].y = ((vs[index].y) << 1) - vs[index-1].y;
               }
            } else {
               if (obj_ptr->type == OBJ_POLYGON &&
                     (index == 0 || index == n-1)) {
                  MoveATransformedPoint(obj_ptr, &vs[1], abs_dx, abs_dy);
                  /* since it's a polygon */
                  vs[n-2].x = ((vs[0].x) << 1) - vs[1].x;
                  vs[n-2].y = ((vs[0].y) << 1) - vs[1].y;
               } else {
                  MoveATransformedPoint(obj_ptr, &vs[index+1], abs_dx, abs_dy);
                  vs[index-1].x = ((vs[index].x) << 1) - vs[index+1].x;
                  vs[index-1].y = ((vs[index].y) << 1) - vs[index+1].y;
               }
            }
         }
      }
   }
}

static
void StretchStructSpline(XGridOff, YGridOff, ObjPtr, Index)
   int XGridOff, YGridOff, Index;
   struct ObjRec *ObjPtr;
{
   struct PolyRec *poly_ptr=NULL;
   struct PolygonRec *polygon_ptr=NULL;
   struct AttrRec *name_attr=NULL, *on_reshape_attr=NULL;
   int i, x, y, dx, dy, stretching=TRUE;
   int ltx=0, lty=0, rbx=0, rby=0;
   int grid_x=XGridOff, grid_y=YGridOff, sn=0, sn2=0, saved_sn=0, saved_sn2=0;
   int auto_center_attr=AutoCenterAttr(ObjPtr);
   int has_on_reshape=HasOnReshape(ObjPtr, &name_attr);
   int num_vs=0, num_vs2=0, ruler_x=0, ruler_y=0, n=0;
   char smooth[5], smooth2[5];
   XEvent input, ev;
   XPoint *sv=NULL, *sv2=NULL, *saved_sv=NULL, *saved_sv2=NULL;
   IntPoint vs[5], vs2[5], start_v, *vlist=NULL, *pvs=NULL;
   StretchStructuredSplineInfo sssi;

   if (ObjPtr->locked) {
      Msg(TgLoadString(STID_LOCKED_OBJS_CANT_BE_STRETCHED));
      return;
   }
   memset(&sssi, 0, sizeof(StretchStructuredSplineInfo));
   memset(vs, 0, 5*sizeof(IntPoint));
   memset(vs2, 0, 5*sizeof(IntPoint));

   switch (ObjPtr->type) {
   case OBJ_POLY:
      poly_ptr = ObjPtr->detail.p;
      start_v.x = poly_ptr->ssvlist[Index].x;
      start_v.y = poly_ptr->ssvlist[Index].y;
      n = poly_ptr->n;
      pvs = poly_ptr->vlist;
      break;
   case OBJ_POLYGON:
      polygon_ptr = ObjPtr->detail.g;
      start_v.x = polygon_ptr->ssvlist[Index].x;
      start_v.y = polygon_ptr->ssvlist[Index].y;
      n = polygon_ptr->n;
      pvs = polygon_ptr->vlist;
      break;
   }
   vlist = (IntPoint*)malloc((n+1)*sizeof(IntPoint));
   if (vlist == NULL) FailAllocMessage();
   memset(vlist, 0, (n+1)*sizeof(IntPoint));

   if (ObjPtr->ctm == NULL) {
      for (i=0; i < n; i++) {
         vlist[i].x = pvs[i].x;
         vlist[i].y = pvs[i].y;
      }
   } else {
      for (i=0; i < n; i++) {
         TransformPointThroughCTM(pvs[i].x-ObjPtr->x, pvs[i].y-ObjPtr->y,
               ObjPtr->ctm, &x, &y);
         vlist[i].x = x+ObjPtr->x;
         vlist[i].y = y+ObjPtr->y;
      }
   }
   if (poly_ptr != NULL) {
      SetIPTInfoForStretchPoly(Index, n, vlist, &sssi);
   } else if (polygon_ptr != NULL) {
      SetIPTInfoForStretchPolygon(Index, n, vlist, &sssi);
   }
   SetVsAndVs2ForStretchStructSpline(&sssi, 0, 0, &num_vs, vs, &num_vs2, vs2);
   FixUpSmoothAndSmooth2ForStretchStructSpline(num_vs, smooth, num_vs2,
         smooth2);
   if (sssi.prev_valid) {
      sv = MakeMultiSplinePolyVertex(LT_STRUCT_SPLINE, &sn, smooth,
            drawOrigX, drawOrigY, num_vs, vs);
      saved_sv = DupVs(&saved_sn, sv, sn);
   }
   if (sssi.next_valid) {
      sv2 = MakeMultiSplinePolyVertex(LT_STRUCT_SPLINE, &sn2, smooth2,
            drawOrigX, drawOrigY, num_vs2, vs2);
      saved_sv2 = DupVs(&saved_sn2, sv2, sn2);
   }
   ltx = ObjPtr->bbox.ltx - handleSize;
   lty = ObjPtr->bbox.lty - handleSize;
   rbx = ObjPtr->bbox.rbx + handleSize;
   rby = ObjPtr->bbox.rby + handleSize;

   XFlush(mainDisplay);
   XSync(mainDisplay, False);

   if (XCheckMaskEvent(mainDisplay, ExposureMask, &ev) ||
         XCheckMaskEvent(mainDisplay, VisibilityChangeMask, &ev)) {
      ExposeEventHandler(&ev, TRUE);
   }
   EraseHighLightForStretchStructSpline(&sssi, 0, 0, TRUE, FALSE);
   DoStretchStructSplineMeasureCursor(STRETCH_STARTSHOW, &start_v, 0, 0, grid_x,
         grid_y);
   if (!debugNoPointerGrab) {
      XGrabPointer(mainDisplay, drawWindow, False,
            PointerMotionMask | ButtonReleaseMask,
            GrabModeAsync, GrabModeAsync, None, handCursor, CurrentTime);
   }
   dx = dy = 0;
   while (stretching) {
      XNextEvent(mainDisplay, &input);

      if (input.type == Expose || input.type == VisibilityNotify) {
         ExposeEventHandler(&input, TRUE);
      } else if (input.type == ButtonRelease) {
         XUngrabPointer(mainDisplay, CurrentTime);
         XSync(mainDisplay, False);
         stretching = FALSE;
      } else if (input.type == MotionNotify || input.type == KeyPress ||
            input.type == KeyRelease) {
         DoStretchStructSplineMeasureCursor(STRETCH_DOSHOW, &start_v,
               ABS_SIZE(grid_x-XGridOff), ABS_SIZE(grid_y-YGridOff),
               grid_x, grid_y);
         if (input.type == KeyPress || input.type == KeyRelease ) {
            x = grid_x;
            y = grid_y;
         } else {
            x = input.xmotion.x;
            y = input.xmotion.y;
         }
         if (shiftForDiagMouseMove && DiagEventCheck(&input)) {
            if (input.type == KeyRelease) {
               x = input.xkey.x;
               y = input.xkey.y;
            } else {
               DiagGridXY(XGridOff, YGridOff, &x, &y);
            }
         }
         GridXY(x, y, &grid_x, &grid_y);

         /* erase */
         if (sv != NULL) {
            XDrawLines(mainDisplay, drawWindow, revDefaultGC, sv, sn,
                  CoordModeOrigin);
            free(sv);
            sv = NULL;
         }
         if (sv2 != NULL) {
            XDrawLines(mainDisplay, drawWindow, revDefaultGC, sv2, sn2,
                  CoordModeOrigin);
            free(sv2);
            sv2 = NULL;
         }
         EraseHighLightForStretchStructSpline(&sssi, ABS_SIZE(dx), ABS_SIZE(dy),
               TRUE, TRUE);
         dx = grid_x - XGridOff;
         dy = grid_y - YGridOff;
         ruler_x = OFFSET_X(start_v.x) + dx;
         ruler_y = OFFSET_Y(start_v.y) + dy;
         MarkRulers(ruler_x, ruler_y);

         /* draw */
         SetVsAndVs2ForStretchStructSpline(&sssi, ABS_SIZE(dx), ABS_SIZE(dy),
               &num_vs, vs, &num_vs2, vs2);
         FixUpSmoothAndSmooth2ForStretchStructSpline(num_vs, smooth, num_vs2,
               smooth2);
         if (sssi.prev_valid) {
            sv = MakeMultiSplinePolyVertex(LT_STRUCT_SPLINE, &sn, smooth,
                  drawOrigX, drawOrigY, num_vs, vs);
            XDrawLines(mainDisplay, drawWindow, revDefaultGC, sv, sn,
                  CoordModeOrigin);
         }
         if (sssi.next_valid) {
            sv2 = MakeMultiSplinePolyVertex(LT_STRUCT_SPLINE, &sn2, smooth2,
                  drawOrigX, drawOrigY, num_vs2, vs2);
            XDrawLines(mainDisplay, drawWindow, revDefaultGC, sv2, sn2,
                  CoordModeOrigin);
         }
         EraseHighLightForStretchStructSpline(&sssi, ABS_SIZE(dx), ABS_SIZE(dy),
               TRUE, TRUE);
         DoStretchStructSplineMeasureCursor(STRETCH_DOSHOW, &start_v,
               ABS_SIZE(grid_x-XGridOff), ABS_SIZE(grid_y-YGridOff),
               grid_x, grid_y);
         while (XCheckMaskEvent(mainDisplay, PointerMotionMask, &ev)) ;
      }
   }
   DoStretchStructSplineMeasureCursor(STRETCH_ENDSHOW, &start_v,
         ABS_SIZE(grid_x-XGridOff), ABS_SIZE(grid_y-YGridOff), grid_x, grid_y);
   /* erase */
   if (sv != NULL) {
      XDrawLines(mainDisplay, drawWindow, revDefaultGC, sv, sn,
            CoordModeOrigin);
      free(sv);
      sv = NULL;
   }
   if (sv2 != NULL) {
      XDrawLines(mainDisplay, drawWindow, revDefaultGC, sv2, sn2,
            CoordModeOrigin);
      free(sv2);
      sv2 = NULL;
   }
   if (saved_sv != NULL) {
      XDrawLines(mainDisplay, drawWindow, revDefaultGC, saved_sv, saved_sn,
            CoordModeOrigin);
      free(saved_sv);
      saved_sv = NULL;
   }
   if (saved_sv2 != NULL) {
      XDrawLines(mainDisplay, drawWindow, revDefaultGC, saved_sv2, saved_sn2,
            CoordModeOrigin);
      free(saved_sv2);
      saved_sv2 = NULL;
   }
   EraseHighLightForStretchStructSpline(&sssi, ABS_SIZE(dx), ABS_SIZE(dy), TRUE,
         TRUE);
   EraseHighLightForHinge(&sssi, 0, 0, FALSE, TRUE);

   if (dx != 0 || dy != 0) {
      if (has_on_reshape && name_attr != NULL) {
         on_reshape_attr = FindAttrWithName(ObjPtr, "on_reshape=", NULL);
      }
      HighLightReverse();

      if (on_reshape_attr != NULL) {
         StartCompositeCmd();
      }
      PrepareToReplaceAnObj(ObjPtr);

      if (poly_ptr != NULL) {
         UpdateObjForStretchStructSpline(ObjPtr, poly_ptr->n, poly_ptr->vlist,
               ABS_SIZE(dx), ABS_SIZE(dy), &sssi);
         AdjObjSplineVs(ObjPtr);
         UpdPolyBBox(ObjPtr, poly_ptr->ssn, poly_ptr->ssvlist);
      } else if (polygon_ptr != NULL) {
         UpdateObjForStretchStructSpline(ObjPtr, polygon_ptr->n,
               polygon_ptr->vlist, ABS_SIZE(dx), ABS_SIZE(dy), &sssi);
         AdjObjSplineVs(ObjPtr);
         UpdPolyBBox(ObjPtr, polygon_ptr->ssn, polygon_ptr->ssvlist);
      }
      if (auto_center_attr) {
         struct AttrRec *attr_ptr=ObjPtr->fattr;
         int modified=FALSE;

         for ( ; attr_ptr != NULL; attr_ptr=attr_ptr->next) {
            if (attr_ptr->shown) {
               struct BBRec bbox;

               CenterObjInOBBox(attr_ptr->obj, ObjPtr->obbox, &bbox);
               if (bbox.ltx < ltx) ltx = bbox.ltx;
               if (bbox.lty < lty) lty = bbox.lty;
               if (bbox.rbx > rbx) rbx = bbox.rbx;
               if (bbox.rby > rby) rby = bbox.rby;
               modified = TRUE;
            }
         }
         if (modified) AdjObjBBox(ObjPtr);
      }
      RecordReplaceAnObj(ObjPtr);
      if (on_reshape_attr != NULL) {
         DoExec(on_reshape_attr, ObjPtr);
      }
      if (on_reshape_attr != NULL) {
         EndCompositeCmd();
      }
      UpdSelBBox();
      RedrawAreas(botObj, ltx-GRID_ABS_SIZE(1), lty-GRID_ABS_SIZE(1),
            rbx+GRID_ABS_SIZE(1), rby+GRID_ABS_SIZE(1),
            ObjPtr->bbox.ltx-GRID_ABS_SIZE(1),
            ObjPtr->bbox.lty-GRID_ABS_SIZE(1),
            ObjPtr->bbox.rbx+GRID_ABS_SIZE(1),
            ObjPtr->bbox.rby+GRID_ABS_SIZE(1));
      SetFileModified(TRUE);
      justDupped = FALSE;

      HighLightForward();
   }
   if (sv != NULL) free(sv);
   if (sv2 != NULL) free(sv2);
   if (vlist != NULL) free(vlist);
}

static XPoint v[5];

static
void StretchPoly(XGridOff, YGridOff, ObjPtr, NumPts, V, Index)
   int XGridOff, YGridOff, NumPts, Index;
   IntPoint *V;
   struct ObjRec *ObjPtr;
{
   struct AttrRec *name_attr=NULL, *on_reshape_attr=NULL;
   int i, x, y, dx, dy, stretching=TRUE;
   int ltx, lty, rbx, rby, curved=LT_STRAIGHT;
   int grid_x=XGridOff, grid_y=YGridOff, sn, intn;
   int auto_center_attr=AutoCenterAttr(ObjPtr);
   int has_on_reshape=HasOnReshape(ObjPtr, &name_attr);
   char *smooth=NULL;
   XEvent input, ev;
   XPoint *sv=NULL;
   IntPoint *pv=NULL, *cntrlv=NULL;

   if (ObjPtr->locked) {
      Msg(TgLoadString(STID_LOCKED_OBJS_CANT_BE_STRETCHED));
      return;
   }
   pv = (IntPoint*)malloc((NumPts+1)*sizeof(IntPoint));
   if (pv == NULL) FailAllocMessage();
   memset(pv, 0, (NumPts+1)*sizeof(IntPoint));

   switch (ObjPtr->type) {
   case OBJ_POLY:
      curved = ObjPtr->detail.p->curved;
      if (curved != LT_INTSPLINE && ObjPtr->detail.p->smooth != NULL) {
         smooth = (char*)malloc((NumPts+1)*sizeof(char));
         if (smooth == NULL) FailAllocMessage();
      }
      if (ObjPtr->ctm == NULL) {
         for (i = 0; i < NumPts; i++) {
            pv[i].x = V[i].x;
            pv[i].y = V[i].y;
            if (smooth != NULL) smooth[i] = ObjPtr->detail.p->smooth[i];
         }
      } else {
         for (i = 0; i < NumPts; i++) {
            int x, y;

            TransformPointThroughCTM(V[i].x-ObjPtr->x, V[i].y-ObjPtr->y,
                  ObjPtr->ctm, &x, &y);
            pv[i].x = x+ObjPtr->x;
            pv[i].y = y+ObjPtr->y;
            if (smooth != NULL) smooth[i] = ObjPtr->detail.p->smooth[i];
         }
      }
      switch (curved) {
      case LT_STRAIGHT:
      case LT_SPLINE:
         sv = MakeMultiSplinePolyVertex(curved, &sn, smooth,
               drawOrigX, drawOrigY, NumPts, pv);
         break;
      case LT_INTSPLINE:
         sv = MakeIntSplinePolyVertex(&sn, &intn, &cntrlv, drawOrigX,
               drawOrigY, NumPts, pv);
         break;
      }
      break;
   case OBJ_POLYGON:
      curved = ObjPtr->detail.g->curved;
      if (curved != LT_INTSPLINE && ObjPtr->detail.g->smooth != NULL) {
         smooth = (char*)malloc((NumPts+1)*sizeof(char));
         if (smooth == NULL) FailAllocMessage();
      }
      if (ObjPtr->ctm == NULL) {
         for (i = 0; i < NumPts; i++) {
            pv[i].x = V[i].x;
            pv[i].y = V[i].y;
            if (smooth != NULL) smooth[i] = ObjPtr->detail.g->smooth[i];
         }
      } else {
         for (i = 0; i < NumPts; i++) {
            int x, y;

            TransformPointThroughCTM(V[i].x-ObjPtr->x, V[i].y-ObjPtr->y,
                  ObjPtr->ctm, &x, &y);
            pv[i].x = x+ObjPtr->x;
            pv[i].y = y+ObjPtr->y;
            if (smooth != NULL) smooth[i] = ObjPtr->detail.g->smooth[i];
         }
      }
      switch (curved) {
      case LT_STRAIGHT:
      case LT_SPLINE:
         sv = MakeMultiSplinePolygonVertex(curved, &sn, smooth,
               drawOrigX, drawOrigY, NumPts, pv);
         break;
      case LT_INTSPLINE:
         sv = MakeIntSplinePolygonVertex(&sn, &intn, &cntrlv,
               drawOrigX, drawOrigY, NumPts, pv);
         break;
      }
      break;
   }
   ltx = ObjPtr->bbox.ltx;
   lty = ObjPtr->bbox.lty;
   rbx = ObjPtr->bbox.rbx;
   rby = ObjPtr->bbox.rby;

   XFlush(mainDisplay);
   XSync(mainDisplay, False);

   if (XCheckMaskEvent(mainDisplay, ExposureMask, &ev) ||
         XCheckMaskEvent(mainDisplay, VisibilityChangeMask, &ev)) {
      ExposeEventHandler(&ev, TRUE);
   }
   DoStretchPolyMeasureCursor(STRETCH_STARTSHOW, NumPts, pv, Index, 0, 0,
         STRETCH_DRAW, STRETCH_CLICK, ObjPtr->type, grid_x, grid_y);
   if (!debugNoPointerGrab) {
      XGrabPointer(mainDisplay, drawWindow, False,
            PointerMotionMask | ButtonReleaseMask,
            GrabModeAsync, GrabModeAsync, None, handCursor, CurrentTime);
   }
   dx = dy = 0;
   while (stretching) {
      XNextEvent(mainDisplay, &input);

      if (input.type == Expose || input.type == VisibilityNotify) {
         ExposeEventHandler(&input, TRUE);
      } else if (input.type == ButtonRelease) {
         XUngrabPointer(mainDisplay, CurrentTime);
         XSync(mainDisplay, False);
         stretching = FALSE;
      } else if (input.type == MotionNotify || input.type == KeyPress ||
            input.type == KeyRelease) {
         DoStretchPolyMeasureCursor(STRETCH_DOSHOW, NumPts, pv, Index,
               ABS_SIZE(grid_x-XGridOff), ABS_SIZE(grid_y-YGridOff),
               STRETCH_ERASE, STRETCH_DRAG, ObjPtr->type, grid_x, grid_y);
         if (input.type == KeyPress || input.type == KeyRelease ) {
            x = grid_x;
            y = grid_y;
         } else {
            x = input.xmotion.x;
            y = input.xmotion.y;
         }
         if (shiftForDiagMouseMove && DiagEventCheck(&input)) {
            if (input.type == KeyRelease) {
               x = input.xkey.x;
               y = input.xkey.y;
            } else {
               DiagGridXY(XGridOff, YGridOff, &x, &y);
            }
         }
         GridXY(x, y, &grid_x, &grid_y);

         XDrawLines(mainDisplay, drawWindow, revDefaultGC, sv, sn,
               CoordModeOrigin);
         dx = grid_x - XGridOff;
         dy = grid_y - YGridOff;
         v[1].x = OFFSET_X(V[Index].x) + dx;
         v[1].y = OFFSET_Y(V[Index].y) + dy;
         MarkRulers(v[1].x, v[1].y);

         if (sv != NULL) {
            free(sv);
            sv = NULL;
         }
         if (ObjPtr->type==OBJ_POLYGON && (Index==0 || Index==NumPts-1)) {
            if (ObjPtr->ctm == NULL) {
               pv[0].x = pv[NumPts-1].x = V[0].x + ABS_SIZE(dx);
               pv[0].y = pv[NumPts-1].y = V[0].y + ABS_SIZE(dy);
            } else {
               int x, y;

               TransformPointThroughCTM(V[0].x-ObjPtr->x, V[0].y-ObjPtr->y,
                     ObjPtr->ctm, &x, &y);
               pv[0].x = pv[NumPts-1].x = x + ObjPtr->x + ABS_SIZE(dx);
               pv[0].y = pv[NumPts-1].y = y + ObjPtr->y + ABS_SIZE(dy);
            }
         } else {
            if (ObjPtr->ctm == NULL) {
               pv[Index].x = V[Index].x + ABS_SIZE(dx);
               pv[Index].y = V[Index].y + ABS_SIZE(dy);
            } else {
               int x, y;

               TransformPointThroughCTM(V[Index].x-ObjPtr->x,
                     V[Index].y-ObjPtr->y, ObjPtr->ctm, &x, &y);
               pv[Index].x = x + ObjPtr->x + ABS_SIZE(dx);
               pv[Index].y = y + ObjPtr->y + ABS_SIZE(dy);
            }
         }
         switch (ObjPtr->type) {
         case OBJ_POLY:
            switch (curved) {
            case LT_STRAIGHT:
            case LT_SPLINE:
               sv = MakeMultiSplinePolyVertex(curved, &sn, smooth,
                     drawOrigX, drawOrigY, NumPts, pv);
               break;
            case LT_INTSPLINE:
               free(cntrlv);
               sv = MakeIntSplinePolyVertex(&sn, &intn, &cntrlv,
                     drawOrigX, drawOrigY, NumPts, pv);
               break;
            }
            break;
         case OBJ_POLYGON:
            switch (curved) {
            case LT_STRAIGHT:
            case LT_SPLINE:
               sv = MakeMultiSplinePolygonVertex(curved, &sn, smooth,
                     drawOrigX, drawOrigY, NumPts, pv);
               break;
            case LT_INTSPLINE:
               free(cntrlv);
               sv = MakeIntSplinePolygonVertex(&sn, &intn, &cntrlv,
                     drawOrigX, drawOrigY, NumPts, pv);
               break;
            }
            break;
         }
         XDrawLines(mainDisplay, drawWindow, revDefaultGC, sv, sn,
               CoordModeOrigin);
         DoStretchPolyMeasureCursor(STRETCH_DOSHOW, NumPts, pv, Index,
               ABS_SIZE(grid_x-XGridOff), ABS_SIZE(grid_y-YGridOff),
               STRETCH_DRAW, STRETCH_DRAG, ObjPtr->type, grid_x, grid_y);
         while (XCheckMaskEvent(mainDisplay, PointerMotionMask, &ev)) ;
      }
   }
   DoStretchPolyMeasureCursor(STRETCH_ENDSHOW, NumPts, pv, Index,
         ABS_SIZE(grid_x-XGridOff), ABS_SIZE(grid_y-YGridOff),
         STRETCH_ERASE, STRETCH_DRAG, ObjPtr->type, grid_x, grid_y);
   if (dx != 0 || dy != 0) {
      if (has_on_reshape && name_attr != NULL) {
         on_reshape_attr = FindAttrWithName(ObjPtr, "on_reshape=", NULL);
      }
      XDrawLines(mainDisplay, drawWindow, revDefaultGC, sv, sn,
            CoordModeOrigin);
      HighLightReverse();

      if (on_reshape_attr != NULL) {
         StartCompositeCmd();
      }
      PrepareToReplaceAnObj(ObjPtr);

      dx = ABS_SIZE(dx);
      dy = ABS_SIZE(dy);
      switch (ObjPtr->type) {
      case OBJ_POLY:
         if (ObjPtr->ctm == NULL) {
            V[Index].x += dx; V[Index].y += dy;
         } else {
            int x, y, new_x, new_y;

            TransformPointThroughCTM(V[Index].x-ObjPtr->x,
                  V[Index].y-ObjPtr->y, ObjPtr->ctm, &x, &y);
            x += ObjPtr->x + dx;
            y += ObjPtr->y + dy;
            ReverseTransformPointThroughCTM(x-ObjPtr->x, y-ObjPtr->y,
                  ObjPtr->ctm, &new_x, &new_y);
            V[Index].x = new_x + ObjPtr->x;
            V[Index].y = new_y + ObjPtr->y;
         }
         AdjObjSplineVs(ObjPtr);
         if (ObjPtr->detail.p->curved != LT_INTSPLINE) {
            UpdPolyBBox(ObjPtr, NumPts, V);
         } else {
            UpdPolyBBox(ObjPtr, ObjPtr->detail.p->intn,
                  ObjPtr->detail.p->intvlist);
         }
         break;
      case OBJ_POLYGON:
         if (ObjPtr->ctm == NULL) {
            V[Index].x += dx; V[Index].y += dy;
         } else {
            int x, y, new_x, new_y;

            TransformPointThroughCTM(V[Index].x-ObjPtr->x,
                  V[Index].y-ObjPtr->y, ObjPtr->ctm, &x, &y);
            x += ObjPtr->x + dx;
            y += ObjPtr->y + dy;
            ReverseTransformPointThroughCTM(x-ObjPtr->x, y-ObjPtr->y,
                  ObjPtr->ctm, &new_x, &new_y);
            V[Index].x = new_x + ObjPtr->x;
            V[Index].y = new_y + ObjPtr->y;
         }
         if (Index == 0) {
            V[NumPts-1].x = V[Index].x; V[NumPts-1].y = V[Index].y;
         } else if (Index == NumPts-1) {
            V[0].x = V[Index].x; V[0].y = V[Index].y;
         }
         AdjObjSplineVs(ObjPtr);
         if (ObjPtr->detail.g->curved != LT_INTSPLINE) {
            UpdPolyBBox(ObjPtr, NumPts, V);
         } else {
            UpdPolyBBox(ObjPtr, ObjPtr->detail.g->intn,
                  ObjPtr->detail.g->intvlist);
         }
         break;
      }
      if (auto_center_attr) {
         struct AttrRec *attr_ptr=ObjPtr->fattr;
         int modified=FALSE;

         for ( ; attr_ptr != NULL; attr_ptr=attr_ptr->next) {
            if (attr_ptr->shown) {
               struct BBRec bbox;

               CenterObjInOBBox(attr_ptr->obj, ObjPtr->obbox, &bbox);
               if (bbox.ltx < ltx) ltx = bbox.ltx;
               if (bbox.lty < lty) lty = bbox.lty;
               if (bbox.rbx > rbx) rbx = bbox.rbx;
               if (bbox.rby > rby) rby = bbox.rby;
               modified = TRUE;
            }
         }
         if (modified) AdjObjBBox(ObjPtr);
      }
      RecordReplaceAnObj(ObjPtr);
      if (on_reshape_attr != NULL) {
         DoExec(on_reshape_attr, ObjPtr);
      }
      if (on_reshape_attr != NULL) {
         EndCompositeCmd();
      }
      UpdSelBBox();
      RedrawAreas(botObj, ltx-GRID_ABS_SIZE(1), lty-GRID_ABS_SIZE(1),
            rbx+GRID_ABS_SIZE(1), rby+GRID_ABS_SIZE(1),
            ObjPtr->bbox.ltx-GRID_ABS_SIZE(1),
            ObjPtr->bbox.lty-GRID_ABS_SIZE(1),
            ObjPtr->bbox.rbx+GRID_ABS_SIZE(1),
            ObjPtr->bbox.rby+GRID_ABS_SIZE(1));
      SetFileModified(TRUE);
      justDupped = FALSE;

      HighLightForward();
   }
   free(pv);
   if (sv != NULL) free(sv);
   if (smooth != NULL) free(smooth);
   if (curved == LT_INTSPLINE && cntrlv != NULL) free(cntrlv);
}

static double	multX=(double)0.0, multY=(double)0.0;
static int	pivotX=0, pivotY=0, changeX=0, changeY=0, moveX=0, moveY=0;
static int	absPivotX=0, absPivotY=0;

static
void StretchedXY(X, Y, NewX, NewY)
   int X, Y, * NewX, * NewY; /* screen offsets */
{
   register int dx, dy;

   dx = round((double)((double)(X - pivotX) * multX));
   dy = round((double)((double)(Y - pivotY) * multY));
   *NewX = pivotX + dx;
   *NewY = pivotY + dy;
}

static
void StretchedAbsXY(X, Y, NewX, NewY)
   int X, Y, * NewX, * NewY; /* screen offsets */
{
   register int dx, dy;

   dx = round((double)((double)(X - absPivotX) * multX));
   dy = round((double)((double)(Y - absPivotY) * multY));
   *NewX = absPivotX + dx;
   *NewY = absPivotY + dy;
}

static
void ShearedXY(Corner, x, y, dx_shear, dy_shear, dx_scale, dy_scale,
      new_x, new_y)
   int Corner, x, y, *new_x, *new_y;
   double dx_shear, dy_shear, dx_scale, dy_scale;
{
   double val, dx, dy;

   if (Corner != CORNER_NONE && Corner != CORNER_RIGHT &&
         Corner != CORNER_LEFT) {
      if (y == pivotY) {
         *new_x = x;
         *new_y = y;
      } else {
         dy = ((double)(y-pivotY))*dy_scale/1000.0;
         val = tan(dx_shear/1000.0)*dy;
         *new_x = round(val + x);
         *new_y = round(dy + pivotY);
      }
   }
   if (Corner != CORNER_NONE && Corner != CORNER_TOP &&
         Corner != CORNER_BOTTOM) {
      if (x == pivotX) {
         *new_x = x;
         *new_y = y;
      } else {
         dx = ((double)(x-pivotX))*dx_scale/1000.0;
         val = tan(dy_shear/1000.0)*dx;
         *new_x = round(dx + pivotX);
         *new_y = round(val + y);
      }
   }
}

static
void ShearedAbsXY(Corner, abs_x, abs_y, dx_shear, dy_shear, dx_scale, dy_scale,
      new_x, new_y)
   int Corner, abs_x, abs_y, *new_x, *new_y;
   double dx_shear, dy_shear, dx_scale, dy_scale;
{
   double val, dx, dy;

   if (Corner != CORNER_NONE && Corner != CORNER_RIGHT &&
         Corner != CORNER_LEFT) {
      if (abs_y == absPivotY) {
         *new_x = abs_x;
         *new_y = abs_y;
      } else {
         dy = ((double)(abs_y-absPivotY))*dy_scale/1000.0;
         val = tan(dx_shear/1000.0)*dy;
         *new_x = round(val + abs_x);
         *new_y = round(dy + absPivotY);
      }
   }
   if (Corner != CORNER_NONE && Corner != CORNER_TOP &&
         Corner != CORNER_BOTTOM) {
      if (abs_x == absPivotX) {
         *new_x = abs_x;
         *new_y = abs_y;
      } else {
         dx = ((double)(abs_x-absPivotX))*dx_scale/1000.0;
         val = tan(dy_shear/1000.0)*dx;
         *new_x = round(dx + absPivotX);
         *new_y = round(val + abs_y);
      }
   }
}

static
void SetPivot(Corner, pOBBox)
   int Corner;
   struct BBRec *pOBBox;
   /* pivotX, pivotY, moveX, moveY will be set to screen offsets */
{
   switch (Corner) {
   case CORNER_NONE: /* same as CORNER_CC */
      pivotX = moveX = ((pOBBox->ltx+pOBBox->rbx)>>1);
      pivotY = moveY = ((pOBBox->lty+pOBBox->rby)>>1);
      changeX = TRUE; changeY = TRUE;
      break;
   case CORNER_LT:
      pivotX = pOBBox->rbx; pivotY = pOBBox->rby;
      moveX = pOBBox->ltx; moveY = pOBBox->lty;
      changeX = changeY = TRUE;
      break;
   case CORNER_TOP:
      pivotX = moveX = ((pOBBox->ltx+pOBBox->rbx)>>1); pivotY = pOBBox->rby;
      moveY = pOBBox->lty;
      changeX = FALSE; changeY = TRUE;
      break;
   case CORNER_RT:
      pivotX = pOBBox->ltx; pivotY = pOBBox->rby;
      moveX = pOBBox->rbx; moveY = pOBBox->lty;
      changeX = changeY = TRUE;
      break;
   case CORNER_RIGHT:
      pivotX = pOBBox->ltx; pivotY = moveY = ((pOBBox->lty+pOBBox->rby)>>1);
      moveX = pOBBox->rbx;
      changeX = TRUE; changeY = FALSE;
      break;
   case CORNER_RB:
      pivotX = pOBBox->ltx; pivotY = pOBBox->lty;
      moveX = pOBBox->rbx; moveY = pOBBox->rby;
      changeX = changeY = TRUE;
      break;
   case CORNER_BOTTOM:
      pivotX = moveX = ((pOBBox->ltx+pOBBox->rbx)>>1); pivotY = pOBBox->lty;
      moveY = pOBBox->rby;
      changeX = FALSE; changeY = TRUE;
      break;
   case CORNER_LB:
      pivotX = pOBBox->rbx; pivotY = pOBBox->lty;
      moveX = pOBBox->ltx; moveY = pOBBox->rby;
      changeX = changeY = TRUE;
      break;
   case CORNER_LEFT:
      pivotX = pOBBox->rbx; pivotY = moveY = ((pOBBox->lty+pOBBox->rby)>>1);
      moveX = pOBBox->ltx;
      changeX = TRUE; changeY = FALSE;
      break;
   }
   multX = 1.0;
   multY = 1.0;
   absPivotX = pivotX;
   absPivotY = pivotY;
   pivotX = OFFSET_X(absPivotX);
   pivotY = OFFSET_Y(absPivotY);
   moveX = OFFSET_X(moveX);
   moveY = OFFSET_Y(moveY);
}

void ShearObj(ObjPtr, Corner, dxShear, dyShear, dxScale, dyScale,
      RealLtX, RealLtY)
   struct ObjRec *ObjPtr;
   int Corner;
   double dxShear, dyShear, dxScale, dyScale; /* scaled by 1000 */
   int *RealLtX, *RealLtY;
{
   IntPoint abs_obj_obbox_vs[5];
   int x, y, new_ltx, new_lty, new_rbx, new_rby;
   double tan_val;
   struct XfrmMtrxRec ctm, new_ctm;
   struct ObjRec *obj_ptr;
   struct AttrRec *attr_ptr;
   int auto_center_attr=AutoCenterAttr(ObjPtr);

   switch (ObjPtr->type) {
   case OBJ_GROUP:
   case OBJ_ICON:
   case OBJ_SYM:
   case OBJ_PIN:
      for (obj_ptr=ObjPtr->detail.r->first; obj_ptr != NULL;
            obj_ptr=obj_ptr->next) {
         ShearObj(obj_ptr, Corner, dxShear, dyShear, dxScale, dyScale,
               RealLtX, RealLtY);
      }
      break;

   default:
      if (ObjPtr->ctm == NULL) {
         memcpy(&ObjPtr->orig_obbox, &ObjPtr->obbox, sizeof(struct BBRec));
         if (ObjPtr->type == OBJ_TEXT) {
            memcpy(&ObjPtr->detail.t->orig_bbox, &ObjPtr->bbox,
                  sizeof(struct BBRec));
         }
         ObjPtr->ctm = (struct XfrmMtrxRec *)malloc(sizeof(struct XfrmMtrxRec));
         if (ObjPtr->ctm == NULL) FailAllocMessage();
         ObjPtr->ctm->m[CTM_SX] = ObjPtr->ctm->m[CTM_SY] = (double)1000;
         ObjPtr->ctm->m[CTM_SIN] = ObjPtr->ctm->m[CTM_MSIN] = (double)0;
         ObjPtr->ctm->t[CTM_TX] = ObjPtr->ctm->t[CTM_TY] = 0;
      }
      ShearedAbsXY(Corner, ObjPtr->x+ObjPtr->ctm->t[CTM_TX],
            ObjPtr->y+ObjPtr->ctm->t[CTM_TY], dxShear, dyShear,
            dxScale, dyScale, &x, &y);
      switch (Corner) {
      case CORNER_TOP:
      case CORNER_BOTTOM:
         tan_val = tan(dxShear/1000.0);
         ctm.m[CTM_SX] = (double)1000;
         ctm.m[CTM_SY] = dyScale;
         ctm.m[CTM_SIN] = (double)0;
         ctm.m[CTM_MSIN] = dyScale*tan_val;
         break;
      case CORNER_RIGHT:
      case CORNER_LEFT:
         tan_val = tan(dyShear/1000.0);
         ctm.m[CTM_SX] = dxScale;
         ctm.m[CTM_SY] = (double)1000;
         ctm.m[CTM_SIN] = dxScale*tan_val;
         ctm.m[CTM_MSIN] = (double)0;
         break;
      default:
         ctm.m[CTM_SX] = dxScale;
         ctm.m[CTM_SY] = dyScale;
         ctm.m[CTM_SIN] = (double)0;
         ctm.m[CTM_MSIN] = (double)0;
         break;
      }
      ctm.t[CTM_TX] = 0;
      ctm.t[CTM_TY] = 0;
      ConcatCTM(ObjPtr->ctm, &ctm, &new_ctm);
      new_ctm.t[CTM_TX] = x-ObjPtr->x;
      new_ctm.t[CTM_TY] = y-ObjPtr->y;
      memcpy(ObjPtr->ctm, &new_ctm, sizeof(struct XfrmMtrxRec));

      GetTransformedOBBoxAbsVs(ObjPtr, abs_obj_obbox_vs);

      new_ltx = min(min(abs_obj_obbox_vs[0].x,abs_obj_obbox_vs[1].x),
            min(abs_obj_obbox_vs[2].x,abs_obj_obbox_vs[3].x));
      new_rbx = max(max(abs_obj_obbox_vs[0].x,abs_obj_obbox_vs[1].x),
            max(abs_obj_obbox_vs[2].x,abs_obj_obbox_vs[3].x));
      new_lty = min(min(abs_obj_obbox_vs[0].y,abs_obj_obbox_vs[1].y),
            min(abs_obj_obbox_vs[2].y,abs_obj_obbox_vs[3].y));
      new_rby = max(max(abs_obj_obbox_vs[0].y,abs_obj_obbox_vs[1].y),
            max(abs_obj_obbox_vs[2].y,abs_obj_obbox_vs[3].y));

      ObjPtr->obbox.ltx = new_ltx; ObjPtr->obbox.lty = new_lty;
      ObjPtr->obbox.rbx = new_rbx; ObjPtr->obbox.rby = new_rby;
      if (RealLtX != NULL && RealLtY != NULL) {
         MoveObj(ObjPtr, (*RealLtX)-new_ltx, (*RealLtY)-new_lty);
      }
      if (ObjPtr->ctm->m[CTM_SX] >= 999.0 &&
            ObjPtr->ctm->m[CTM_SX] <= 1001.0 &&
            ObjPtr->ctm->m[CTM_SY] >= 999.0 &&
            ObjPtr->ctm->m[CTM_SY] <= 1001.0 &&
            ObjPtr->ctm->m[CTM_SIN] >= (-1.0) &&
            ObjPtr->ctm->m[CTM_SIN] <= 1.0 &&
            ObjPtr->ctm->m[CTM_MSIN] >= (-1.0) &&
            ObjPtr->ctm->m[CTM_MSIN] <= 1.0) {
         int dx=ObjPtr->ctm->t[CTM_TX], dy=ObjPtr->ctm->t[CTM_TY];

         free(ObjPtr->ctm);
         ObjPtr->ctm = NULL;

         memcpy(&ObjPtr->obbox, &ObjPtr->orig_obbox, sizeof(struct BBRec));
         if (ObjPtr->type == OBJ_TEXT) {
            memcpy(&ObjPtr->bbox, &ObjPtr->detail.t->orig_bbox,
                  sizeof(struct BBRec));
         }
         MoveObj(ObjPtr, dx, dy);
      }
      break;
   }
   AdjObjOBBox(ObjPtr);
   if (auto_center_attr) {
      for (attr_ptr=ObjPtr->fattr; attr_ptr != NULL; attr_ptr=attr_ptr->next) {
         ShearObj(attr_ptr->obj, Corner, dxShear, dyShear, dxScale, dyScale,
               NULL, NULL);
         if (attr_ptr->shown) {
            CenterObjInOBBox(attr_ptr->obj, ObjPtr->obbox, NULL);
         }
      }
   } else {
      for (attr_ptr=ObjPtr->fattr; attr_ptr != NULL; attr_ptr=attr_ptr->next) {
         ShearObj(attr_ptr->obj, Corner, dxShear, dyShear, dxScale, dyScale,
               NULL, NULL);
      }
   }
   AdjObjSplineVs(ObjPtr);
   AdjObjCache(ObjPtr);
   AdjObjBBox(ObjPtr);
}

static
void StretchSimpleText(ObjPtr, Corner)
   struct ObjRec *ObjPtr;
   int Corner;
{
   /* !stretchableText */
   if (ObjPtr->ctm == NULL) {
      int new_x, new_y, h=ABS_SIZE(ObjPtr->obbox.rby-ObjPtr->obbox.lty);

      StretchedAbsXY(ObjPtr->x, ObjPtr->y, &new_x, &new_y);
      MoveObj(ObjPtr, new_x-ObjPtr->x, new_y-ObjPtr->y);
      ObjPtr->x = new_x;
      ObjPtr->y = new_y;
      if (multX < 0) {
         ObjPtr->detail.t->minilines.just =
               MAXJUSTS-1-ObjPtr->detail.t->minilines.just;
      }
      if (multY < 0) {
         MoveObj(ObjPtr, 0, -h);
      }
   } else {
      int abs_x, abs_y, new_x, new_y;

      abs_x = ObjPtr->x+ObjPtr->ctm->t[CTM_TX];
      abs_y = ObjPtr->y+ObjPtr->ctm->t[CTM_TY];
      StretchedAbsXY(abs_x, abs_y, &new_x, &new_y);
      if (multX < 0.0 || multY < 0.0) {
         int new_ltx, new_lty, new_rbx, new_rby;
         IntPoint abs_obj_obbox_vs[5];
         struct XfrmMtrxRec ctm, new_ctm;

         ctm.m[CTM_SX] = ctm.m[CTM_SY] = (double)1000;
         ctm.m[CTM_SIN] = ctm.m[CTM_MSIN] = (double)0;
         ctm.t[CTM_TX] = ctm.t[CTM_TY] = 0;
         ctm.m[CTM_SX] = (multX < 0.0) ? (double)(-1000) : (double)1000;
         ctm.m[CTM_SY] = (multY < 0.0) ? (double)(-1000) : (double)1000;
         ConcatCTM(ObjPtr->ctm, &ctm, &new_ctm);
         new_ctm.t[CTM_TX] = new_x-ObjPtr->x;
         new_ctm.t[CTM_TY] = new_y-ObjPtr->y;
         memcpy(ObjPtr->ctm, &new_ctm, sizeof(struct XfrmMtrxRec));

         GetTransformedOBBoxAbsVs(ObjPtr, abs_obj_obbox_vs);

         new_ltx = min(min(abs_obj_obbox_vs[0].x,abs_obj_obbox_vs[1].x),
               min(abs_obj_obbox_vs[2].x,abs_obj_obbox_vs[3].x));
         new_rbx = max(max(abs_obj_obbox_vs[0].x,abs_obj_obbox_vs[1].x),
               max(abs_obj_obbox_vs[2].x,abs_obj_obbox_vs[3].x));
         new_lty = min(min(abs_obj_obbox_vs[0].y,abs_obj_obbox_vs[1].y),
               min(abs_obj_obbox_vs[2].y,abs_obj_obbox_vs[3].y));
         new_rby = max(max(abs_obj_obbox_vs[0].y,abs_obj_obbox_vs[1].y),
               max(abs_obj_obbox_vs[2].y,abs_obj_obbox_vs[3].y));

         ObjPtr->obbox.ltx = new_ltx; ObjPtr->obbox.lty = new_lty;
         ObjPtr->obbox.rbx = new_rbx; ObjPtr->obbox.rby = new_rby;
         abs_x = ObjPtr->x+ObjPtr->ctm->t[CTM_TX];
         abs_y = ObjPtr->y+ObjPtr->ctm->t[CTM_TY];
      }
      MoveObj(ObjPtr, new_x-abs_x, new_y-abs_y);
   }
   UpdTextBBox(ObjPtr);
   AdjObjSplineVs(ObjPtr);
   AdjObjBBox(ObjPtr);
}

static
int CompoundObjHasTextSubObj(ObjPtr)
   struct ObjRec *ObjPtr;
{
   struct ObjRec *sub_obj=ObjPtr->detail.r->last;

   for ( ; sub_obj != NULL; sub_obj=sub_obj->prev) {
      if (sub_obj->type == OBJ_GROUP ||
            sub_obj->type == OBJ_ICON ||
            sub_obj->type == OBJ_SYM ||
            sub_obj->type == OBJ_PIN) {
         if (CompoundObjHasTextSubObj(sub_obj)) {
            return TRUE;
         }
      } else {
         return (sub_obj->type == OBJ_TEXT);
      }
   }
   return FALSE;
}

static
void JustMoveSimpleCompoundObj(ObjPtr, Corner)
   struct ObjRec *ObjPtr;
   int Corner;
{
   /* !stretchableText && !compoundObjWithTextStretchableForPSE */
   if (ObjPtr->ctm == NULL) {
      int new_x, new_y, h=ABS_SIZE(ObjPtr->obbox.rby-ObjPtr->obbox.lty);

      StretchedAbsXY(ObjPtr->x, ObjPtr->y, &new_x, &new_y);
      MoveObj(ObjPtr, new_x-ObjPtr->x, new_y-ObjPtr->y);
      ObjPtr->x = new_x;
      ObjPtr->y = new_y;
      if (multY < 0) {
         MoveObj(ObjPtr, 0, -h);
      }
   } else {
      int abs_x, abs_y, new_x, new_y;

      abs_x = ObjPtr->x+ObjPtr->ctm->t[CTM_TX];
      abs_y = ObjPtr->y+ObjPtr->ctm->t[CTM_TY];
      StretchedAbsXY(abs_x, abs_y, &new_x, &new_y);
      if (multX < 0.0 || multY < 0.0) {
         int new_ltx, new_lty, new_rbx, new_rby;
         IntPoint abs_obj_obbox_vs[5];
         struct XfrmMtrxRec ctm, new_ctm;

         ctm.m[CTM_SX] = ctm.m[CTM_SY] = (double)1000;
         ctm.m[CTM_SIN] = ctm.m[CTM_MSIN] = (double)0;
         ctm.t[CTM_TX] = ctm.t[CTM_TY] = 0;
         ctm.m[CTM_SX] = (multX < 0.0) ? (double)(-1000) : (double)1000;
         ctm.m[CTM_SY] = (multY < 0.0) ? (double)(-1000) : (double)1000;
         ConcatCTM(ObjPtr->ctm, &ctm, &new_ctm);
         new_ctm.t[CTM_TX] = new_x-ObjPtr->x;
         new_ctm.t[CTM_TY] = new_y-ObjPtr->y;
         memcpy(ObjPtr->ctm, &new_ctm, sizeof(struct XfrmMtrxRec));

         GetTransformedOBBoxAbsVs(ObjPtr, abs_obj_obbox_vs);

         new_ltx = min(min(abs_obj_obbox_vs[0].x,abs_obj_obbox_vs[1].x),
               min(abs_obj_obbox_vs[2].x,abs_obj_obbox_vs[3].x));
         new_rbx = max(max(abs_obj_obbox_vs[0].x,abs_obj_obbox_vs[1].x),
               max(abs_obj_obbox_vs[2].x,abs_obj_obbox_vs[3].x));
         new_lty = min(min(abs_obj_obbox_vs[0].y,abs_obj_obbox_vs[1].y),
               min(abs_obj_obbox_vs[2].y,abs_obj_obbox_vs[3].y));
         new_rby = max(max(abs_obj_obbox_vs[0].y,abs_obj_obbox_vs[1].y),
               max(abs_obj_obbox_vs[2].y,abs_obj_obbox_vs[3].y));

         ObjPtr->obbox.ltx = new_ltx; ObjPtr->obbox.lty = new_lty;
         ObjPtr->obbox.rbx = new_rbx; ObjPtr->obbox.rby = new_rby;
         abs_x = ObjPtr->x+ObjPtr->ctm->t[CTM_TX];
         abs_y = ObjPtr->y+ObjPtr->ctm->t[CTM_TY];
      }
      MoveObj(ObjPtr, new_x-abs_x, new_y-abs_y);
   }
   AdjObjSplineVs(ObjPtr);
   AdjObjBBox(ObjPtr);
}

static
void StretchAttr(ObjPtr, Corner, dxScale, dyScale, AutoCenterAttr)
   struct ObjRec *ObjPtr;
   int Corner, AutoCenterAttr;
   double dxScale, dyScale;
{
   struct AttrRec *attr_ptr=ObjPtr->fattr;

   if (attr_ptr == NULL) return;

   if (stretchingEverything) {
      struct BBRec final_obbox;
      double dz=(double)0;

      if (AutoCenterAttr) {
         for (attr_ptr=ObjPtr->fattr; attr_ptr != NULL;
               attr_ptr=attr_ptr->next) {
            struct ObjRec *obj_ptr=attr_ptr->obj;
            int ltx=0, lty=0, rbx=0, rby=0;

            StretchedAbsXY(obj_ptr->obbox.ltx, obj_ptr->obbox.lty, &ltx, &lty);
            StretchedAbsXY(obj_ptr->obbox.rbx, obj_ptr->obbox.rby, &rbx, &rby);
            CalcBBox(ltx, lty, rbx, rby, &final_obbox.ltx, &final_obbox.lty,
                  &final_obbox.rbx, &final_obbox.rby);

            ShearObj(attr_ptr->obj, Corner, dz, dz, dxScale, dyScale,
                     &final_obbox.ltx, &final_obbox.lty);
            if (attr_ptr->shown) {
               CenterObjInOBBox(attr_ptr->obj, ObjPtr->obbox, NULL);
            }
         }
      } else {
         for (attr_ptr=ObjPtr->fattr; attr_ptr != NULL;
               attr_ptr=attr_ptr->next) {
            struct ObjRec *obj_ptr=attr_ptr->obj;
            int ltx=0, lty=0, rbx=0, rby=0;

            StretchedAbsXY(obj_ptr->obbox.ltx, obj_ptr->obbox.lty, &ltx, &lty);
            StretchedAbsXY(obj_ptr->obbox.rbx, obj_ptr->obbox.rby, &rbx, &rby);
            CalcBBox(ltx, lty, rbx, rby, &final_obbox.ltx, &final_obbox.lty,
                  &final_obbox.rbx, &final_obbox.rby);

            ShearObj(obj_ptr, Corner, dz, dz, dxScale, dyScale,
                     &final_obbox.ltx, &final_obbox.lty);
         }
      }
   } else {
      if (AutoCenterAttr) {
         for (attr_ptr=ObjPtr->fattr; attr_ptr != NULL;
               attr_ptr=attr_ptr->next) {
            if (attr_ptr->shown) {
               CenterObjInOBBox(attr_ptr->obj, ObjPtr->obbox, NULL);
            } else {
               StretchSimpleText(attr_ptr->obj, Corner);
            }
         }
      } else {
         for (attr_ptr=ObjPtr->fattr; attr_ptr != NULL;
               attr_ptr=attr_ptr->next) {
            StretchSimpleText(attr_ptr->obj, Corner);
         }
      }
   }
}

static
void StretchSimpleArc(ObjPtr)
   struct ObjRec *ObjPtr;
{
   struct ArcRec *arc_ptr=ObjPtr->detail.a;
   int x, y;

   StretchedAbsXY(arc_ptr->xc, arc_ptr->yc, &x, &y);
   arc_ptr->xc = ObjPtr->x = x;
   arc_ptr->yc = ObjPtr->y = y;
   StretchedAbsXY(arc_ptr->x1, arc_ptr->y1, &x, &y);
   arc_ptr->x1 = x;
   arc_ptr->y1 = y;
   StretchedAbsXY(arc_ptr->x2, arc_ptr->y2, &x, &y);
   arc_ptr->x2 = x;
   arc_ptr->y2 = y;
   StretchedAbsXY(arc_ptr->ltx, arc_ptr->lty, &x, &y);
   arc_ptr->ltx = arc_ptr->xc-abs(x-arc_ptr->xc);
   arc_ptr->lty = arc_ptr->yc-abs(y-arc_ptr->yc);
   arc_ptr->w = (arc_ptr->xc-arc_ptr->ltx)<<1;
   arc_ptr->h = (arc_ptr->yc-arc_ptr->lty)<<1;

   if (multX < 0) {
      arc_ptr->dir = !(arc_ptr->dir);
      arc_ptr->angle2 = -(arc_ptr->angle2);
      if (arc_ptr->angle1 > 0) {
         arc_ptr->angle1 = (180*64) - arc_ptr->angle1;
      } else {
         arc_ptr->angle1 = (-180*64) - arc_ptr->angle1;
      }
   }
   if (multY < 0) {
      arc_ptr->dir = !(arc_ptr->dir);
      arc_ptr->angle1 = -(arc_ptr->angle1);
      arc_ptr->angle2 = -(arc_ptr->angle2);
   }
   AdjObjSplineVs(ObjPtr);
   AdjObjBBox(ObjPtr);
}

static
void StretchSimplePoly(ObjPtr)
   struct ObjRec *ObjPtr;
{
   int i, ltx=0, lty=0, rbx=0, rby=0;
   struct PolyRec *poly_ptr= ObjPtr->detail.p;
   IntPoint *vs=poly_ptr->vlist;

   for (i = 0; i < poly_ptr->n; i++) {
      int x, y;

      StretchedAbsXY(vs[i].x, vs[i].y, &x, &y);
      vs[i].x = x;
      vs[i].y = y;
      if (i == 0) {
         ltx = rbx = x;
         lty = rby = y;
      } else {
         if (x < ltx) ltx = x; if (y < lty) lty = y;
         if (x > rbx) rbx = x; if (y > rby) rby = y;
      }
   }
   ObjPtr->obbox.ltx = ObjPtr->x = ltx;
   ObjPtr->obbox.lty = ObjPtr->y = lty;
   ObjPtr->obbox.rbx = rbx;
   ObjPtr->obbox.rby = rby;
   AdjObjSplineVs(ObjPtr);
   if (poly_ptr->curved == LT_INTSPLINE) {
      UpdPolyBBox(ObjPtr, poly_ptr->intn, poly_ptr->intvlist);
   }
}

static
void StretchSimplePolygon(ObjPtr)
   struct ObjRec *ObjPtr;
{
   int i, ltx=0, lty=0, rbx=0, rby=0;
   struct PolygonRec *polygon_ptr= ObjPtr->detail.g;
   IntPoint *vs=polygon_ptr->vlist;

   for (i = 0; i < polygon_ptr->n; i++) {
      int x, y;

      StretchedAbsXY(vs[i].x, vs[i].y, &x, &y);
      vs[i].x = x;
      vs[i].y = y;
      if (i == 0) {
         ltx = rbx = x;
         lty = rby = y;
      } else {
         if (x < ltx) ltx = x; if (y < lty) lty = y;
         if (x > rbx) rbx = x; if (y > rby) rby = y;
      }
   }
   ObjPtr->obbox.ltx = ObjPtr->x = ltx;
   ObjPtr->obbox.lty = ObjPtr->y = lty;
   ObjPtr->obbox.rbx = rbx;
   ObjPtr->obbox.rby = rby;
   AdjObjSplineVs(ObjPtr);
   if (polygon_ptr->curved == LT_INTSPLINE) {
      UpdPolyBBox(ObjPtr, polygon_ptr->intn, polygon_ptr->intvlist);
   }
}

static
void StretchSimpleObj(ObjPtr, Corner, dxScale, dyScale, FinalOBBox,
      auto_center_attr)
   struct ObjRec *ObjPtr;
   int Corner, auto_center_attr;
   double dxScale, dyScale;
   struct BBRec *FinalOBBox;
{
   ObjPtr->obbox.ltx = ObjPtr->x = FinalOBBox->ltx;
   ObjPtr->obbox.lty = ObjPtr->y = FinalOBBox->lty;
   ObjPtr->obbox.rbx = FinalOBBox->rbx;
   ObjPtr->obbox.rby = FinalOBBox->rby;

   switch (ObjPtr->type) {
   case OBJ_ARC: StretchSimpleArc(ObjPtr); break;
   case OBJ_POLY: StretchSimplePoly(ObjPtr); break;
   case OBJ_POLYGON: StretchSimplePolygon(ObjPtr); break;
   case OBJ_RCBOX: AdjObjSplineVs(ObjPtr); break;
   case OBJ_BOX: AdjObjSplineVs(ObjPtr); break;
   case OBJ_OVAL: AdjObjSplineVs(ObjPtr); break;
   }
   AdjObjOBBox(ObjPtr);
   StretchAttr(ObjPtr, Corner, dxScale, dyScale, auto_center_attr);
   AdjObjBBox(ObjPtr);
}

static
void ScaleLineWidth(ObjPtr, dScale)
   struct ObjRec *ObjPtr;
   double dScale;
{
   double dscale=(double)(dScale/1000.0);

   if (!stretchingEverything) return;

   switch (ObjPtr->type) {
   case OBJ_POLY:
      ScaleWidthAndSpec(dscale, &ObjPtr->detail.p->width,
            ObjPtr->detail.p->width_spec);
      ScaleWidthAndSpec(dscale, &ObjPtr->detail.p->aw,
            ObjPtr->detail.p->aw_spec);
      ScaleWidthAndSpec(dscale, &ObjPtr->detail.p->ah,
            ObjPtr->detail.p->ah_spec);
      break;
   case OBJ_POLYGON:
      ScaleWidthAndSpec(dscale, &ObjPtr->detail.g->width,
            ObjPtr->detail.g->width_spec);
      break;
   case OBJ_BOX:
      ScaleWidthAndSpec(dscale, &ObjPtr->detail.b->width,
            ObjPtr->detail.b->width_spec);
      break;
   case OBJ_OVAL:
      ScaleWidthAndSpec(dscale, &ObjPtr->detail.o->width,
            ObjPtr->detail.o->width_spec);
      break;
   case OBJ_ARC:
      ScaleWidthAndSpec(dscale, &ObjPtr->detail.a->width,
            ObjPtr->detail.a->width_spec);
      ScaleWidthAndSpec(dscale, &ObjPtr->detail.a->aw,
            ObjPtr->detail.a->aw_spec);
      ScaleWidthAndSpec(dscale, &ObjPtr->detail.a->ah,
            ObjPtr->detail.a->ah_spec);
      break;
   case OBJ_RCBOX:
      ScaleWidthAndSpec(dscale, &ObjPtr->detail.rcb->width,
            ObjPtr->detail.rcb->width_spec);
      break;
   }
}

static
void StretchObj(ObjPtr, Corner, dxScale, dyScale, ForceToUseCTM)
   struct ObjRec *ObjPtr;
   int Corner, ForceToUseCTM;
   double dxScale, dyScale;
{
   int ltx, lty, rbx, rby;
   int auto_center_attr=AutoCenterAttr(ObjPtr);
   struct BBRec final_obbox;
   struct ObjRec *obj_ptr=NULL;
   struct AttrRec *saved_fattr=NULL, *saved_lattr=NULL;
   double dz=(double)0;

   StretchedAbsXY(ObjPtr->obbox.ltx, ObjPtr->obbox.lty, &ltx, &lty);
   StretchedAbsXY(ObjPtr->obbox.rbx, ObjPtr->obbox.rby, &rbx, &rby);
   CalcBBox(ltx, lty, rbx, rby, &final_obbox.ltx, &final_obbox.lty,
         &final_obbox.rbx, &final_obbox.rby);

   if (ForceToUseCTM && ObjPtr->ctm == NULL &&
         ObjPtr->type != OBJ_GROUP && ObjPtr->type != OBJ_ICON &&
         ObjPtr->type != OBJ_SYM && ObjPtr->type != OBJ_PIN) {
      memcpy(&ObjPtr->orig_obbox, &ObjPtr->obbox, sizeof(struct BBRec));
      if (ObjPtr->type == OBJ_TEXT) {
         memcpy(&ObjPtr->detail.t->orig_bbox, &ObjPtr->bbox,
               sizeof(struct BBRec));
      }
      ObjPtr->ctm = (struct XfrmMtrxRec *)malloc(sizeof(struct XfrmMtrxRec));
      if (ObjPtr->ctm == NULL) FailAllocMessage();
      ObjPtr->ctm->m[CTM_SX] = ObjPtr->ctm->m[CTM_SY] = (double)1000;
      ObjPtr->ctm->m[CTM_SIN] = ObjPtr->ctm->m[CTM_MSIN] = (double)0;
      ObjPtr->ctm->t[CTM_TX] = ObjPtr->ctm->t[CTM_TY] = 0;
   }
   switch (ObjPtr->type) {
   case OBJ_POLY:
   case OBJ_POLYGON:
   case OBJ_BOX:
   case OBJ_OVAL:
   case OBJ_ARC:
   case OBJ_RCBOX:
      ScaleLineWidth(ObjPtr, dxScale);
      if (ObjPtr->ctm == NULL) {
         StretchSimpleObj(ObjPtr, Corner, dxScale, dyScale, &final_obbox,
               auto_center_attr);
      } else {
         ShearObj(ObjPtr, Corner, dz, dz, dxScale, dyScale,
               &final_obbox.ltx, &final_obbox.lty);
      }
      break;
   case OBJ_TEXT:
      if (stretchableText) {
         ShearObj(ObjPtr, Corner, dz, dz, dxScale, dyScale,
               &final_obbox.ltx, &final_obbox.lty);
      } else {
         StretchSimpleText(ObjPtr, Corner);
      }
      break;
   case OBJ_GROUP:
   case OBJ_ICON:
   case OBJ_SYM:
   case OBJ_PIN:
      if (!stretchableText && !compoundObjWithTextStretchableForPSE &&
            CompoundObjHasTextSubObj(ObjPtr)) {
         JustMoveSimpleCompoundObj(ObjPtr, Corner);
      } else {
         for (obj_ptr=ObjPtr->detail.r->first; obj_ptr != NULL;
               obj_ptr=obj_ptr->next) {
            StretchObj(obj_ptr, Corner, dxScale, dyScale, ForceToUseCTM);
         }
         AdjObjOBBox(ObjPtr);
         StretchAttr(ObjPtr, Corner, dxScale, dyScale, auto_center_attr);
         AdjObjSplineVs(ObjPtr);
         AdjObjBBox(ObjPtr);
      }
      break;
   case OBJ_XBM:
   case OBJ_XPM:
      saved_fattr = ObjPtr->fattr;
      saved_lattr = ObjPtr->lattr;
      ObjPtr->fattr = ObjPtr->lattr = NULL;
      ShearObj(ObjPtr, Corner, dz, dz, dxScale, dyScale,
            &final_obbox.ltx, &final_obbox.lty);
      ObjPtr->fattr = saved_fattr;
      ObjPtr->lattr = saved_lattr;
      StretchAttr(ObjPtr, Corner, dxScale, dyScale, auto_center_attr);
      AdjObjBBox(ObjPtr);
      break;
   }
}

static
void StretchAllSelObjects(Corner, dxScale, dyScale)
   int Corner;
   double dxScale, dyScale;
{
   struct SelRec *sel_ptr;

   for (sel_ptr = topSel; sel_ptr != NULL; sel_ptr = sel_ptr->next) {
      if (!sel_ptr->obj->locked) {
         StretchObj(sel_ptr->obj, Corner, dxScale, dyScale, FALSE);
      }
   }
   if (numObjLocked != 0) {
      Msg(TgLoadString(STID_LOCKED_OBJS_ARE_NOT_STRETCHED));
   }
}

static
void MarkObjectsForStretch()
{
   register struct ObjRec *obj_ptr;
   register struct SelRec *sel_ptr;

   for (obj_ptr = botObj; obj_ptr != NULL; obj_ptr = obj_ptr->prev) {
      obj_ptr->marked = FALSE;
   }
   for (sel_ptr = botSel; sel_ptr != NULL; sel_ptr = sel_ptr->prev) {
      sel_ptr->obj->marked = TRUE;
   }
}

static
int ConstrainedStretchAllSel(Corner, ltx, lty, rbx, rby)
   int Corner;
   int *ltx, *lty, *rbx, *rby;
{
   register struct ObjRec *obj_ptr;
   int something_stretched=FALSE, num_pts;
   int x_off, y_off, move_first, move_last, x, y;
   IntPoint *v;

   for (obj_ptr=botObj; obj_ptr != NULL; obj_ptr=obj_ptr->prev) {
      if (!obj_ptr->marked && obj_ptr->type==OBJ_POLY && !obj_ptr->locked) {
         num_pts = obj_ptr->detail.p->n;
         v = obj_ptr->detail.p->vlist;

         if (obj_ptr->ctm == NULL) {
            x_off = OFFSET_X(v[0].x); y_off = OFFSET_Y(v[0].y);
            move_first = EndPtInSelected(x_off, y_off);
            x_off = OFFSET_X(v[num_pts-1].x); y_off = OFFSET_Y(v[num_pts-1].y);
            move_last = EndPtInSelected(x_off, y_off);
         } else {
            int tmp_x, tmp_y;

            TransformPointThroughCTM(v[0].x-obj_ptr->x, v[0].y-obj_ptr->y,
                  obj_ptr->ctm, &tmp_x, &tmp_y);
            tmp_x += obj_ptr->x;
            tmp_y += obj_ptr->y;
            x_off = OFFSET_X(tmp_x); y_off = OFFSET_Y(tmp_y);
            move_first = EndPtInSelected(x_off, y_off);
            TransformPointThroughCTM(v[num_pts-1].x-obj_ptr->x,
                  v[num_pts-1].y-obj_ptr->y, obj_ptr->ctm, &tmp_x, &tmp_y);
            tmp_x += obj_ptr->x;
            tmp_y += obj_ptr->y;
            x_off = OFFSET_X(tmp_x); y_off = OFFSET_Y(tmp_y);
            move_last = EndPtInSelected(x_off, y_off);
         }

         if (move_first || move_last) {
            int index=INVALID, seg_dx, seg_dy, dx, dy, cur_seg_dx, cur_seg_dy;

            PrepareToReplaceAnObj(obj_ptr);

            if (obj_ptr->ctm != NULL) {
               /* Remove the transformations! */
               int i;

               for (i=0; i < num_pts; i++) {
                  int tmp_x, tmp_y;

                  TransformPointThroughCTM(v[i].x-obj_ptr->x, v[i].y-obj_ptr->y,
                        obj_ptr->ctm, &tmp_x, &tmp_y);
                  v[i].x = tmp_x+obj_ptr->x;
                  v[i].y = tmp_y+obj_ptr->y;
               }
               free(obj_ptr->ctm);
               obj_ptr->ctm = NULL;
               UpdPolyBBox(obj_ptr, num_pts, v);
            }
            if (something_stretched) {
               if (obj_ptr->bbox.ltx < *ltx) *ltx = obj_ptr->bbox.ltx;
               if (obj_ptr->bbox.lty < *lty) *lty = obj_ptr->bbox.lty;
               if (obj_ptr->bbox.rbx > *rbx) *rbx = obj_ptr->bbox.rbx;
               if (obj_ptr->bbox.rby > *rby) *rby = obj_ptr->bbox.rby;
            } else {
               *ltx = obj_ptr->bbox.ltx; *lty = obj_ptr->bbox.lty;
               *rbx = obj_ptr->bbox.rbx; *rby = obj_ptr->bbox.rby;
            }
            something_stretched = TRUE;
            if (move_first && move_last && num_pts==3) {
               StretchedAbsXY(v[0].x, v[0].y, &x, &y);
               dx = x-v[0].x; dy = y-v[0].y;
               index = 1;
               cur_seg_dx = v[index-1].x - v[index].x;
               cur_seg_dy = v[index-1].y - v[index].y;
               seg_dx = v[index].x - v[index+1].x;
               seg_dy = v[index].y - v[index+1].y;

               if (cur_seg_dy==0 && seg_dx==0 &&
                     (seg_dy!=0 || (seg_dy==0 && dx==0))) {
                  v[index].y += dy;
               } else if (cur_seg_dx==0 && seg_dy==0 &&
                     (seg_dx!=0 || (seg_dx==0 && dy==0))) {
                  v[index].x += dx;
               }
            } else {
               if (move_first && num_pts>2) {
                  StretchedAbsXY(v[0].x, v[0].y, &x, &y);
                  dx = x-v[0].x; dy = y-v[0].y;
                  index = 1;
                  cur_seg_dx = v[index-1].x - v[index].x;
                  cur_seg_dy = v[index-1].y - v[index].y;
                  seg_dx = v[index].x - v[index+1].x;
                  seg_dy = v[index].y - v[index+1].y;

                  if (cur_seg_dy==0 && cur_seg_dx!=0 &&
                        (seg_dy!=0 || (seg_dy==0 && dx==0))) {
                     v[index].y += dy;
                  } else if (cur_seg_dx==0 && cur_seg_dy!=0 &&
                        (seg_dx!=0 || (seg_dx==0 && dy==0))) {
                     v[index].x += dx;
                  }
               }
               if (move_last && num_pts>2) {
                  StretchedAbsXY(v[num_pts-1].x, v[num_pts-1].y, &x, &y);
                  dx = x-v[num_pts-1].x; dy = y-v[num_pts-1].y;
                  index = num_pts-2;
                  cur_seg_dx = v[index+1].x - v[index].x;
                  cur_seg_dy = v[index+1].y - v[index].y;
                  seg_dx = v[index].x - v[index-1].x;
                  seg_dy = v[index].y - v[index-1].y;

                  if (cur_seg_dy==0 && cur_seg_dx!=0 &&
                        (seg_dy!=0 || (seg_dy==0 && dx==0))) {
                     v[index].y += dy;
                  } else if (cur_seg_dx==0 && cur_seg_dy!=0 &&
                        (seg_dx!=0 || (seg_dx==0 && dy==0))) {
                     v[index].x += dx;
                  }
               }
            }
            if (move_first) {
               StretchedAbsXY(v[0].x, v[0].y, &x, &y);
               v[0].x = x; v[0].y = y;
            }
            if (move_last) {
               StretchedAbsXY(v[num_pts-1].x, v[num_pts-1].y, &x, &y);
               v[num_pts-1].x = x; v[num_pts-1].y = y;
            }
            AdjObjSplineVs(obj_ptr);
            switch (obj_ptr->type) {
            case OBJ_POLY:
               if (obj_ptr->detail.p->curved != LT_INTSPLINE) {
                  UpdPolyBBox(obj_ptr, num_pts, v);
               } else {
                  UpdPolyBBox(obj_ptr, obj_ptr->detail.p->intn,
                        obj_ptr->detail.p->intvlist);
               }
               break;
            case OBJ_POLYGON:
               if (obj_ptr->detail.g->curved != LT_INTSPLINE) {
                  UpdPolyBBox(obj_ptr, num_pts, v);
               } else {
                  UpdPolyBBox(obj_ptr, obj_ptr->detail.g->intn,
                        obj_ptr->detail.g->intvlist);
               }
               break;
            }
            AdjObjBBox(obj_ptr);
            if (AutoCenterAttr(obj_ptr)) {
               struct AttrRec *attr_ptr=obj_ptr->fattr;
               int modified=FALSE;

               for ( ; attr_ptr != NULL; attr_ptr = attr_ptr->next) {
                  if (attr_ptr->shown) {
                     struct BBRec bbox;

                     CenterObjInOBBox(attr_ptr->obj, obj_ptr->obbox, &bbox);
                     if (bbox.ltx < *ltx) *ltx = bbox.ltx;
                     if (bbox.lty < *lty) *lty = bbox.lty;
                     if (bbox.rbx > *rbx) *rbx = bbox.rbx;
                     if (bbox.rby > *rby) *rby = bbox.rby;
                     modified = TRUE;
                  }
               }
               if (modified) AdjObjBBox(obj_ptr);
            }
            if (obj_ptr->bbox.ltx < *ltx) *ltx = obj_ptr->bbox.ltx;
            if (obj_ptr->bbox.lty < *lty) *lty = obj_ptr->bbox.lty;
            if (obj_ptr->bbox.rbx > *rbx) *rbx = obj_ptr->bbox.rbx;
            if (obj_ptr->bbox.rby > *rby) *rby = obj_ptr->bbox.rby;
            RecordReplaceAnObj(obj_ptr);
         }
      }
   }
   return something_stretched;
}

static
void StretchAllSel(Corner, dxScale, dyScale)
   int Corner;
   double dxScale, dyScale; /* dxScale and dyScale are scaled by 1000 */
{
   int ltx, lty, rbx, rby, saved_ltx, saved_lty, saved_rbx, saved_rby;
   int poly_stretched;

   saved_ltx = selLtX; saved_lty = selLtY;
   saved_rbx = selRbX; saved_rby = selRbY;

   if (moveMode==CONST_MOVE) {
      MarkObjectsForStretch();

      StartCompositeCmd();
      PrepareToRecord(CMD_STRETCH, topSel, botSel, numObjSelected);
      RecordCmd(CMD_STRETCH, NULL, topSel, botSel, numObjSelected);

      poly_stretched = ConstrainedStretchAllSel(Corner, &ltx, &lty, &rbx, &rby);
      StretchAllSelObjects(Corner, dxScale, dyScale);
      UpdSelBBox();
      if (poly_stretched) {
         ltx = min(ltx,min(selLtX,saved_ltx));
         lty = min(lty,min(selLtY,saved_lty));
         rbx = max(rbx,max(selRbX,saved_rbx));
         rby = max(rby,max(selRbY,saved_rby));
         RedrawAnArea(botObj, ltx-GRID_ABS_SIZE(1), lty-GRID_ABS_SIZE(1),
               rbx+GRID_ABS_SIZE(1), rby+GRID_ABS_SIZE(1));
      } else {
         RedrawAreas(botObj, saved_ltx-GRID_ABS_SIZE(1),
               saved_lty-GRID_ABS_SIZE(1),
               saved_rbx+GRID_ABS_SIZE(1), saved_rby+GRID_ABS_SIZE(1),
               selLtX-GRID_ABS_SIZE(1), selLtY-GRID_ABS_SIZE(1),
               selRbX+GRID_ABS_SIZE(1), selRbY+GRID_ABS_SIZE(1));
      }
      EndCompositeCmd();
   } else {
      PrepareToRecord(CMD_REPLACE, topSel, botSel, numObjSelected);
      StretchAllSelObjects(Corner, dxScale, dyScale);
      RecordCmd(CMD_REPLACE, NULL, topSel, botSel, numObjSelected);
      UpdSelBBox();
      RedrawAreas(botObj, saved_ltx-GRID_ABS_SIZE(1),
            saved_lty-GRID_ABS_SIZE(1),
            saved_rbx+GRID_ABS_SIZE(1), saved_rby+GRID_ABS_SIZE(1),
            selLtX-GRID_ABS_SIZE(1), selLtY-GRID_ABS_SIZE(1),
            selRbX+GRID_ABS_SIZE(1), selRbY+GRID_ABS_SIZE(1));
   }
}

static
int DiagonalDistance(dx, dy)
   double dx, dy;
{
   double dval=sqrt(dx*dx+dy*dy);

   return round(dval);
}

static
double DiagonalAngle(dx, dy)
   int dx, dy;
{
   double theta=((dx==0) ? ((dy>=0) ? M_PI/2.0 : -M_PI/2.0) :
         atan2((double)(dy), (double)(dx)));

   return theta*((double)180.0)/M_PI;
}

static
void GetMeasurement(ObjPtr, buf, buf1)
   struct ObjRec *ObjPtr;
   char *buf, *buf1;
{
   int ltx, lty, rbx, rby, real_ltx, real_lty, real_rbx, real_rby, dx, dy;
   char x_buf[80], y_buf[80], d_buf[80];
   double diag_angle;

   StretchedAbsXY(ObjPtr->obbox.ltx, ObjPtr->obbox.lty, &ltx, &lty);
   StretchedAbsXY(ObjPtr->obbox.rbx, ObjPtr->obbox.rby, &rbx, &rby);
   CalcBBox(ltx, lty, rbx, rby, &real_ltx, &real_lty, &real_rbx, &real_rby);
   PixelToMeasurementUnit(x_buf, abs(real_rbx-real_ltx));
   PixelToMeasurementUnit(y_buf, abs(real_rby-real_lty));
   sprintf(buf, "w=%s\nh=%s", x_buf, y_buf);
   dx = real_rbx-real_ltx;
   dy = real_rby-real_lty;
   PixelToMeasurementUnit(d_buf, abs(DiagonalDistance((double)dx, (double)dy)));
   diag_angle = DiagonalAngle(dx, dy);
   sprintf(buf1, "Diagonal: length=%s angle=%.2f or %.2f", d_buf,
         (float)diag_angle, (float)(((double)360.0)-diag_angle));
}

static
void PointsToShearScale(Corner, x_pivot, y_pivot, x_move, y_move,
      x_current, y_current, dx_shear, dy_shear, dx_scale, dy_scale)
   int Corner, x_pivot, y_pivot, x_move, y_move, x_current, y_current;
   double *dx_shear, *dy_shear, *dx_scale, *dy_scale;
   /* the returned shear value is 1000*arctan() */
   /* the returned scale value is 1000*scaling */
{
   int dx=x_current-x_move, dy=y_current-y_move;

   switch (Corner) {
   case CORNER_TOP:
   case CORNER_BOTTOM:
      if (dx_scale != NULL) *dx_scale = (double)1000;
      if (dy_scale != NULL) {
         *dy_scale = (dy == 0 ? (double)1000 :
               (double)(((double)(y_current-y_pivot)) /
               ((double)(y_move-y_pivot))*1000.0));
      }
      if (dx_shear != NULL) {
         *dx_shear = (dx == 0 ? (double)0 :
               (double)(atan2((double)dx,(double)y_current-y_pivot)*1000.0));
      }
      if (dy_shear != NULL) *dy_shear = (double)0;
      break;
   case CORNER_RIGHT:
   case CORNER_LEFT:
      if (dx_scale != NULL) {
         *dx_scale = (dx == 0 ? (double)1000 :
               (double)(((double)(x_current-x_pivot)) /
               ((double)(x_move-x_pivot))*1000.0));
      }
      if (dy_scale != NULL) *dy_scale = (double)1000;
      if (dx_shear != NULL) *dx_shear = (double)0;
      if (dy_shear != NULL) {
         *dy_shear = (dy == 0 ? (double)0 :
               (double)(atan2((double)dy,(double)x_current-x_pivot)*1000.0));
      }
      break;
   default: /* scaling only, no shearing */
      if (dx_scale != NULL) {
         *dx_scale = (dx == 0 ? (double)1000 :
               (double)(((double)(x_current-x_pivot)) /
               ((double)(x_move-x_pivot))*1000.0));
      }
      if (dy_scale != NULL) {
         *dy_scale = (dy == 0 ? (double)1000 :
               (double)(((double)(y_current-y_pivot)) /
               ((double)(y_move-y_pivot))*1000.0));
      }
      if (dx_shear != NULL) *dx_shear = (double)0;
      if (dy_shear != NULL) *dy_shear = (double)0;
      break;
   }
}

static
void StretchBox(XGridOff, YGridOff, ObjPtr, Corner)
   int XGridOff, YGridOff, Corner;
   struct ObjRec *ObjPtr;
{
   int x, y, stretching=TRUE;
   int ltx, lty, rbx, rby, sel_ltx, sel_lty, sel_rbx, sel_rby;
   int stretched_ltx, stretched_lty, stretched_rbx, stretched_rby;
   int stretched_sel_ltx, stretched_sel_lty, stretched_sel_rbx;
   int stretched_sel_rby;
   int ruler_ltx, ruler_lty, ruler_rbx, ruler_rby;
   int sel_obj_ltx, sel_obj_lty, sel_obj_rbx, sel_obj_rby;
   int grid_x=XGridOff, grid_y=YGridOff, proportional=FALSE;
   char buf[80], buf1[80];
   double obj_w, obj_h;
   XEvent input, ev;

   if (numObjSelected == numObjLocked || ObjPtr->locked) {
      MsgBox(TgLoadString(STID_LOCKED_OBJS_CANT_BE_STRETCHED), TOOL_NAME,
            INFO_MB);
      return;
   }
   XFlush(mainDisplay);
   XSync(mainDisplay, False);

   if (XCheckMaskEvent(mainDisplay, ExposureMask, &ev) ||
         XCheckMaskEvent(mainDisplay, VisibilityChangeMask, &ev)) {
      ExposeEventHandler(&ev, TRUE);
   }
   SetPivot(Corner, &ObjPtr->obbox);

   stretched_sel_ltx = sel_ltx = OFFSET_X(selNoLockLtX);
   stretched_sel_lty = sel_lty = OFFSET_Y(selNoLockLtY);
   stretched_sel_rbx = sel_rbx = OFFSET_X(selNoLockRbX);
   stretched_sel_rby = sel_rby = OFFSET_Y(selNoLockRbY);
   SelBox(drawWindow, revDefaultGC, stretched_sel_ltx-2, stretched_sel_lty-2,
         stretched_sel_rbx+2, stretched_sel_rby+2);

   ruler_ltx = sel_obj_ltx = OFFSET_X(selNoLockObjLtX);
   ruler_lty = sel_obj_lty = OFFSET_Y(selNoLockObjLtY);
   ruler_rbx = sel_obj_rbx = OFFSET_X(selNoLockObjRbX);
   ruler_rby = sel_obj_rby = OFFSET_Y(selNoLockObjRbY);

   stretched_ltx = ltx = OFFSET_X(ObjPtr->obbox.ltx);
   stretched_lty = lty = OFFSET_Y(ObjPtr->obbox.lty);
   stretched_rbx = rbx = OFFSET_X(ObjPtr->obbox.rbx);
   stretched_rby = rby = OFFSET_Y(ObjPtr->obbox.rby);
   SelBox(drawWindow, revDefaultGC, stretched_ltx, stretched_lty,
         stretched_rbx, stretched_rby);

   if (ltx == rbx || lty == rby) {
      Msg(TgLoadString(ltx == rbx ? STID_CANT_STRETCH_OBJ_HAS_0_WIDTH :
            STID_CANT_STRETCH_OBJ_HAS_0_HEIGHT));
      SelBox(drawWindow, revDefaultGC, stretched_ltx, stretched_lty,
            stretched_rbx, stretched_rby);
      SelBox(drawWindow, revDefaultGC, stretched_sel_ltx-2,
            stretched_sel_lty-2, stretched_sel_rbx+2, stretched_sel_rby+2);
      return;
   }
   SaveStatusStrings();

   obj_w = (double)(moveX - pivotX);
   obj_h = (double)(moveY - pivotY);

   GetMeasurement(ObjPtr, buf, buf1);
   StartShowMeasureCursor(grid_x, grid_y, buf, TRUE);
   if (showMeasurement) SetStringStatus(buf1);

   BeginIntervalRulers(ruler_ltx, ruler_lty, ruler_rbx, ruler_rby);
   if (!debugNoPointerGrab) {
      XGrabPointer(mainDisplay, drawWindow, False,
            PointerMotionMask | ButtonReleaseMask,
            GrabModeAsync, GrabModeAsync, None, handCursor, CurrentTime);
   }
   while (stretching) {
      XNextEvent(mainDisplay, &input);

      if (input.type == Expose || input.type == VisibilityNotify) {
         ExposeEventHandler(&input, TRUE);
      } else if (input.type == ButtonRelease) {
         proportional = (input.xbutton.state & (ShiftMask|ControlMask));
         XUngrabPointer(mainDisplay, CurrentTime);
         XSync(mainDisplay, False);
         stretching = FALSE;
      } else if (input.type == MotionNotify || input.type == KeyPress ||
            input.type == KeyRelease) {
         proportional = (input.xmotion.state & (ShiftMask|ControlMask));

         GetMeasurement(ObjPtr, buf, buf1);
         EndShowMeasureCursor(grid_x, grid_y, buf, TRUE);
         if (showMeasurement) SetStringStatus(buf1);

         if (input.type == KeyPress || input.type == KeyRelease) {
            x = grid_x;
            y = grid_y;
         } else {
            x = input.xmotion.x;
            y = input.xmotion.y;
         }
         GridXY(x, y, &grid_x, &grid_y);

         SelBox(drawWindow, revDefaultGC, stretched_ltx, stretched_lty,
               stretched_rbx, stretched_rby);
         SelBox(drawWindow, revDefaultGC, stretched_sel_ltx-2,
               stretched_sel_lty-2, stretched_sel_rbx+2, stretched_sel_rby+2);

         if (proportional) {
            int new_w, new_h;
            double w_ratio, h_ratio;

            new_w = moveX + grid_x - XGridOff - pivotX;
            new_h = moveY + grid_y - YGridOff - pivotY;
            w_ratio = (moveX!=pivotX) ? fabs(((double)new_w)/obj_w) : 0.0;
            h_ratio = (moveY!=pivotY) ? fabs(((double)new_h)/obj_h) : 0.0;
            if (changeX && changeY) {
               if (w_ratio >= h_ratio) {
                  multX = (moveX!=pivotX) ? ((double)new_w)/obj_w : 1.0;
                  multY = fabs(multX) * ((new_h*obj_h>=0) ? 1.0 : -1.0);
               } else {
                  multX = fabs(multY) * ((new_w*obj_w>=0) ? 1.0 : -1.0);
                  multY = (moveY!=pivotY) ? ((double)new_h)/obj_h : 1.0;
               }
            } else if (changeX) {
               multX = (moveX!=pivotX) ? ((double)new_w)/obj_w : 1.0;
               multY = fabs(multX);
            } else if (changeY) {
               multX = fabs(multY);
               multY = (moveY!=pivotY) ? ((double)new_h)/obj_h : 1.0;
            }
         } else {
            if (changeX) {
               multX = (moveX!=pivotX) ?
                     (double)(moveX+grid_x-XGridOff-pivotX)/obj_w : 1.0;
            } else {
               multX = (double)1.0;
            }
            if (changeY) {
               multY = (moveY!=pivotY) ?
                     (double)(moveY+grid_y-YGridOff-pivotY)/obj_h : 1.0;
            } else {
               multY = (double)1.0;
            }
         }
         StretchedXY(sel_ltx, sel_lty, &stretched_sel_ltx, &stretched_sel_lty);
         StretchedXY(sel_rbx, sel_rby, &stretched_sel_rbx, &stretched_sel_rby);
         StretchedXY(ltx, lty, &stretched_ltx, &stretched_lty);
         StretchedXY(rbx, rby, &stretched_rbx, &stretched_rby);
         StretchedXY(sel_obj_ltx, sel_obj_lty, &ruler_ltx, &ruler_lty);
         StretchedXY(sel_obj_rbx, sel_obj_rby, &ruler_rbx, &ruler_rby);

         DrawIntervalRulers(ruler_ltx, ruler_lty, ruler_rbx, ruler_rby, NULL);
         SelBox(drawWindow, revDefaultGC, stretched_sel_ltx-2,
               stretched_sel_lty-2, stretched_sel_rbx+2, stretched_sel_rby+2);
         SelBox(drawWindow, revDefaultGC, stretched_ltx, stretched_lty,
               stretched_rbx, stretched_rby);
         GetMeasurement(ObjPtr, buf, buf1);
         EndShowMeasureCursor(grid_x, grid_y, buf, TRUE);
         if (showMeasurement) SetStringStatus(buf1);
         while (XCheckMaskEvent(mainDisplay, PointerMotionMask, &ev)) ;
      }
   }
   EndIntervalRulers(grid_x, grid_y);
   GetMeasurement(ObjPtr, buf, buf1);
   EndShowMeasureCursor(grid_x, grid_y, buf, TRUE);
   if (showMeasurement) SetStringStatus(buf1);

   RestoreStatusStrings();

   SelBox(drawWindow, revDefaultGC, stretched_ltx, stretched_lty,
         stretched_rbx, stretched_rby);
   SelBox(drawWindow, revDefaultGC, stretched_sel_ltx-2,
         stretched_sel_lty-2, stretched_sel_rbx+2, stretched_sel_rby+2);
   if (multX != (double)1.0 || multY != (double)1.0) {
      int num_to_resize=0;
      double dx_scale=(double)1000, dy_scale=(double)1000;
      char **ppsz_names_to_resize=NULL;

      PointsToShearScale(Corner, pivotX, pivotY, moveX, moveY,
            moveX+grid_x-XGridOff, moveY+grid_y-YGridOff,
            NULL, NULL, &dx_scale, &dy_scale);
      if (proportional) {
         int abs_x_scale=round(dx_scale), abs_y_scale=round(dy_scale);

         if (abs_x_scale > abs_y_scale) {
            dy_scale = dx_scale;
         } else if (abs_x_scale < abs_y_scale) {
            dx_scale = dy_scale;
         }
      }
      HighLightReverse();
      ppsz_names_to_resize = NeedToProcessOnResize(&num_to_resize);
      if (ppsz_names_to_resize == NULL) {
         StretchAllSel(Corner, dx_scale, dy_scale);
      } else {
         StartCompositeCmd();
         StretchAllSel(Corner, dx_scale, dy_scale);
         DoOnResize(ppsz_names_to_resize, num_to_resize);
         EndCompositeCmd();
      }
      HighLightForward();
      SetFileModified(TRUE);
      justDupped = FALSE;
   }
}

void StretchSel(XGridOff, YGridOff, ObjPtr, Corner)
   int XGridOff, YGridOff, Corner;
   struct ObjRec *ObjPtr;
{
   switch (ObjPtr->type) {
   case OBJ_BOX:
   case OBJ_OVAL:
   case OBJ_GROUP:
   case OBJ_ICON:
   case OBJ_SYM:
   case OBJ_PIN:
   case OBJ_ARC:
   case OBJ_RCBOX:
   case OBJ_XBM:
   case OBJ_XPM:
      StretchBox(XGridOff, YGridOff, ObjPtr, Corner);
      break;
   case OBJ_POLY:
      if (ObjPtr->detail.p->curved == LT_STRUCT_SPLINE) {
         StretchStructSpline(XGridOff, YGridOff, ObjPtr, Corner);
      } else {
         StretchPoly(XGridOff, YGridOff, ObjPtr, ObjPtr->detail.p->n,
               ObjPtr->detail.p->vlist, Corner);
      }
      break;
   case OBJ_POLYGON:
      if (ObjPtr->detail.g->curved == LT_STRUCT_SPLINE) {
         StretchStructSpline(XGridOff, YGridOff, ObjPtr, Corner);
      } else {
         StretchPoly(XGridOff, YGridOff, ObjPtr, ObjPtr->detail.g->n,
               ObjPtr->detail.g->vlist, Corner);
      }
      break;
   case OBJ_TEXT:
      if (stretchableText) {
         StretchBox(XGridOff, YGridOff, ObjPtr, Corner);
      }
      break;
   }
}

void ScaleAnEPSObj(ObjPtr, ScalingFactor)
   struct ObjRec *ObjPtr;
   float *ScalingFactor;
{
   struct BBRec *obbox=(&(ObjPtr->obbox));

   multX = multY = (double)(*ScalingFactor);
   changeX = changeY = (fabs(multX-1.0) > 1.0e-6);
   if (!changeX && !changeY) return;;

   absPivotX = obbox->ltx;
   absPivotY = obbox->lty;
   moveX = obbox->rbx;
   moveY = obbox->rby;
   StretchObj(ObjPtr, CORNER_RB, (double)(multX*1000.0), (double)(multY*1000.0),
         FALSE);
}

static
void ScaleAllSelObjects(Corner, dxScale, dyScale)
   int Corner;
   double dxScale, dyScale;
   /*
    * Force to use CTM!
    */
{
   struct SelRec *sel_ptr=NULL;

   for (sel_ptr=topSel; sel_ptr != NULL; sel_ptr=sel_ptr->next) {
      sel_ptr->obj->tmp_parent = NULL;
   }
   for (sel_ptr=topSel; sel_ptr != NULL; sel_ptr=sel_ptr->next) {
      if (stretchingEverything || !sel_ptr->obj->locked) {
         StretchObj(sel_ptr->obj, Corner, dxScale, dyScale, TRUE);
      }
   }
   if (!stretchingEverything && numObjLocked != 0) {
      Msg(TgLoadString(STID_LOCKED_OBJS_CANT_BE_SCALED));
   }
}

static
void ScaleAllSel(Corner, dxScale, dyScale, redraw)
   int Corner, redraw;
   double dxScale, dyScale;
{
   int ltx, lty, rbx, rby, saved_ltx, saved_lty, saved_rbx, saved_rby;
   int poly_stretched;

   saved_ltx = selLtX; saved_lty = selLtY;
   saved_rbx = selRbX; saved_rby = selRbY;

   if (moveMode == CONST_MOVE) {
      MarkObjectsForStretch();

      StartCompositeCmd();
      PrepareToRecord(CMD_STRETCH, topSel, botSel, numObjSelected);
      RecordCmd(CMD_STRETCH, NULL, topSel, botSel, numObjSelected);

      poly_stretched = ConstrainedStretchAllSel(Corner, &ltx, &lty, &rbx, &rby);
      ScaleAllSelObjects(Corner, dxScale, dyScale);
      UpdSelBBox();
      if (redraw) {
         if (poly_stretched) {
            ltx = min(ltx,min(selLtX,saved_ltx));
            lty = min(lty,min(selLtY,saved_lty));
            rbx = max(rbx,max(selRbX,saved_rbx));
            rby = max(rby,max(selRbY,saved_rby));
            RedrawAnArea(botObj, ltx-GRID_ABS_SIZE(1), lty-GRID_ABS_SIZE(1),
                  rbx+GRID_ABS_SIZE(1), rby+GRID_ABS_SIZE(1));
         } else {
            RedrawAreas(botObj, saved_ltx-GRID_ABS_SIZE(1),
                  saved_lty-GRID_ABS_SIZE(1),
                  saved_rbx+GRID_ABS_SIZE(1), saved_rby+GRID_ABS_SIZE(1),
                  selLtX-GRID_ABS_SIZE(1), selLtY-GRID_ABS_SIZE(1),
                  selRbX+GRID_ABS_SIZE(1), selRbY+GRID_ABS_SIZE(1));
         }
      }
      EndCompositeCmd();
   } else {
      PrepareToRecord(CMD_REPLACE, topSel, botSel, numObjSelected);
      ScaleAllSelObjects(Corner, dxScale, dyScale);
      RecordCmd(CMD_REPLACE, NULL, topSel, botSel, numObjSelected);
      UpdSelBBox();
      if (redraw) {
         RedrawAreas(botObj, saved_ltx-GRID_ABS_SIZE(1),
               saved_lty-GRID_ABS_SIZE(1),
               saved_rbx+GRID_ABS_SIZE(1), saved_rby+GRID_ABS_SIZE(1),
               selLtX-GRID_ABS_SIZE(1), selLtY-GRID_ABS_SIZE(1),
               selRbX+GRID_ABS_SIZE(1), selRbY+GRID_ABS_SIZE(1));
      }
   }
}

static
char *FindColon(s)
   register char *s;
{
   while (*s!=':' && *s!='x' && *s!='X' && *s!=' ' && *s!='\0') s++;
   return ((*s==':' || *s=='x' || *s=='X' || *s==' ') ? (s) : (char *)NULL);
}

void ScaleAllSelObj()
{
   char spec[MAXSTRING], *y_spec=NULL, **ppsz_names_to_resize=NULL;
   int corner=INVALID, saved_h_align=horiAlign, saved_v_align=vertAlign;
   int num_to_resize=0;
   struct BBRec obbox;

   if (topSel == NULL) {
      MsgBox(TgLoadCachedString(CSTID_NO_OBJ_SELECTED), TOOL_NAME, INFO_MB);
      return;
   }
   if (numObjSelected == numObjLocked) {
      MsgBox(TgLoadString(STID_LOCKED_OBJS_CANT_BE_SCALED), TOOL_NAME, INFO_MB);
      return;
   }
   *spec = '\0';
   Dialog(TgLoadString(STID_ENTER_SCALING_FACTORS_XY), NULL, spec);
   UtilTrimBlanks(spec);
   if (*spec == '\0') return;

   horiAlign = ALIGN_L;
   vertAlign = ALIGN_T;
   corner = CORNER_RB;

   obbox.ltx = selNoLockObjLtX; obbox.lty = selNoLockObjLtY;
   obbox.rbx = selNoLockObjRbX; obbox.rby = selNoLockObjRbY;
   SetPivot(corner, &obbox);
   horiAlign = saved_h_align;
   vertAlign = saved_v_align;
   if ((y_spec=FindColon(spec)) == NULL) {
      if (sscanf(spec, "%lf", &multX) != 1 || multX <= 0.0) {
         sprintf(gszMsgBox, TgLoadString(STID_INVALID_SPEC), spec);
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
         return;
      }
      multY = multX;
   } else {
      *y_spec++ = '\0';
      if (sscanf(spec, "%lf", &multX) != 1 ||
            sscanf(y_spec, "%lf", &multY) != 1 ||
            multX <= 0.0 || multY <= 0.0) {
         sprintf(gszMsgBox, TgLoadString(STID_INVALID_SPEC), spec);
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
         return;
      }
   }
   changeX = (fabs(multX-1.0) > 1.0e-6);
   changeY = (fabs(multY-1.0) > 1.0e-6);
   if (!changeX && !changeY) return;

   horiAlign = ALIGN_L;
   vertAlign = ALIGN_T;
   HighLightReverse();
   ppsz_names_to_resize = NeedToProcessOnResize(&num_to_resize);
   if (ppsz_names_to_resize == NULL) {
      ScaleAllSel(corner, (double)(multX*1000.0), (double)(multY*1000.0), TRUE);
   } else {
      StartCompositeCmd();
      ScaleAllSel(corner, (double)(multX*1000.0), (double)(multY*1000.0), TRUE);
      DoOnResize(ppsz_names_to_resize, num_to_resize);
      EndCompositeCmd();
   }
   HighLightForward();
   SetFileModified(TRUE);
   justDupped = FALSE;
   horiAlign = saved_h_align;
   vertAlign = saved_v_align;
}

void PreciseScaleEverything()
{
   int corner=CORNER_RB, saved_h_align=horiAlign, saved_v_align=vertAlign;
   int saved_cur_page_num=curPageNum, prev_page_num=curPageNum;
   int saved_stretchable_text=stretchableText;
   char spec[MAXSTRING], buf[MAXSTRING];
   struct BBRec obbox;

   *spec = '\0';
   Dialog(TgLoadString(STID_ENTER_A_SCALING_FACTOR), NULL, spec);
   UtilTrimBlanks(spec);
   if (*spec == '\0') return;

   obbox.ltx = obbox.lty = 0;
   obbox.rbx = onePageWidth; obbox.rby = onePageHeight;
   SetPivot(corner, &obbox);

   if (FindColon(spec) != NULL) {
      MsgBox(TgLoadString(STID_ONLY_INPUT_ONE_NUMERIC_VAL), TOOL_NAME, INFO_MB);
      return;
   } else if (sscanf(spec, "%lf", &multX) != 1 || multX <= 0.0) {
      sprintf(gszMsgBox, TgLoadString(STID_INVALID_SPEC), spec);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return;
   } else if (fabs(multX-((float)1.0)) < INT_TOL) {
      return;
   }
   if (round(multX) >= 10) {
      sprintf(gszMsgBox, TgLoadString(STID_SCALING_EVERYTHING_LARGE_SURE),
            multX);
      if (MsgBox(gszMsgBox, TOOL_NAME, YNC_MB) != MB_ID_YES) {
         return;
      }
   }
   multY = multX;

   changeX = (fabs(multX-1.0) > 1.0e-6);
   changeY = (fabs(multY-1.0) > 1.0e-6);
   if (!changeX && !changeY) return;

   MakeQuiescent();

   if (firstCmd != NULL) {
      if (!OkToFlushUndoBuffer(
            TgLoadString(STID_PRECISE_SCALE_EV_CAUSE_FLUSH))) {
         SetCurChoice(curChoiceBeforeMakeQuiescent);
         return;
      }
   }
   corner = CORNER_RB;
   horiAlign = ALIGN_L;
   vertAlign = ALIGN_T;
   stretchableText = saved_stretchable_text;

   printMag /= multX;
   if (UpdPageStyle(pageStyle)) {
      UpdDrawWinBBox();
      AdjSplineVs();
   }
   FormatFloat(&printMag, buf);
   if (printMag <= 100.0) {
      sprintf(gszMsgBox, TgLoadString(STID_NEW_REDUCTION_IS_PERCENT), buf);
   } else {
      sprintf(gszMsgBox, TgLoadString(STID_NEW_ENLARGEMENT_IS_PERCENT), buf);
   }
   Msg(gszMsgBox);
   SaveStatusStrings();

   StartCompositeCmd();
   for (curPageNum=1; curPageNum <= lastPageNum; curPageNum++) {
      int num_to_resize=0;
      char **ppsz_names_to_resize=NULL;

      /* No need for the CMD_GOTO_PAGE stuff since CleanUpCmds will be called */
      /* PrepareToRecord(CMD_GOTO_PAGE, NULL, NULL, prev_page_num); */
      GotoPageNum(curPageNum);
      ShowPage();
      XSync(mainDisplay, False);
      /* RecordCmd(CMD_GOTO_PAGE, NULL, NULL, NULL, curPageNum); */
      prev_page_num = curPageNum;
      sprintf(gszMsgBox, TgLoadCachedString(CSTID_SCALING_EVERYTHING_PAGE),
            curPageNum, lastPageNum);
      SetStringStatus(gszMsgBox);

      SelAllObj(FALSE, FALSE);

      ppsz_names_to_resize = NeedToProcessOnResize(&num_to_resize);
      stretchingEverything = TRUE;
      ScaleAllSel(corner, (double)(multX*1000.0), (double)(multY*1000.0), TRUE);
      stretchingEverything = FALSE;
      if (ppsz_names_to_resize != NULL) {
         DoOnResize(ppsz_names_to_resize, num_to_resize);
      }
      RemoveAllSel();
   }
   /* PrepareToRecord(CMD_GOTO_PAGE, NULL, NULL, prev_page_num); */
   GotoPageNum(saved_cur_page_num);
   ShowPage();
   /* RecordCmd(CMD_GOTO_PAGE, NULL, NULL, NULL, saved_cur_page_num); */

   EndCompositeCmd();
   CleanUpCmds();

   RestoreStatusStrings();
   SetFileModified(TRUE);
   justDupped = FALSE;

   stretchableText = saved_stretchable_text;
   horiAlign = saved_h_align;
   vertAlign = saved_v_align;

   RedrawScrollBars();
   RedrawRulers();
   RedrawTitleWindow();
   ClearAndRedrawDrawWindow();
   SetCurChoice(curChoiceBeforeMakeQuiescent);
}

void ScaleObjLikeScaleEverything(obj_ptr, scale, redraw)
   struct ObjRec *obj_ptr;
   double scale;
   int redraw;
{
   int corner=CORNER_RB, saved_h_align=horiAlign, saved_v_align=vertAlign;
   int saved_stretchable_text=stretchableText, saved_move_mode=moveMode;
   float saved_print_mag=printMag;
   struct BBRec obbox;

   obbox.ltx = obbox.lty = 0;
   obbox.rbx = onePageWidth; obbox.rby = onePageHeight;
   SetPivot(corner, &obbox);

   if (fabs(scale-((float)1.0)) < INT_TOL) {
      return;
   }
   multX = multY = ((double)1)/scale;

   changeX = (fabs(multX-1.0) > 1.0e-6);
   changeY = (fabs(multY-1.0) > 1.0e-6);
   if (!changeX && !changeY) return;

   corner = CORNER_RB;
   horiAlign = ALIGN_L;
   vertAlign = ALIGN_T;
   stretchableText = TRUE;
   moveMode = UNCONST_MOVE;

   printMag /= multX;
   if (UpdPageStyle(pageStyle)) {
      UpdDrawWinBBox();
      AdjSplineVs();
   }
   if (multX < 100.0) {
      sprintf(gszMsgBox, TgLoadString(STID_REDUCE_BY_FACTOR), multX);
   } else {
      sprintf(gszMsgBox, TgLoadString(STID_ENLARGE_BY_FACTOR), multX);
   }
   SetStringStatus(gszMsgBox);
   SaveStatusStrings();

   AddObj(NULL, topObj, obj_ptr);
   topSel = botSel = SelectThisObject(obj_ptr);
   UpdSelBBox();

   StartCompositeCmd();

   stretchingEverything = TRUE;
   ScaleAllSel(corner, (double)(multX*1000.0), (double)(multY*1000.0), redraw);
   stretchingEverything = FALSE;
   RemoveAllSel();

   EndCompositeCmd();
   CleanUpCmds();

   UnlinkObj(topObj);

   RestoreStatusStrings();
   SetFileModified(TRUE);
   justDupped = FALSE;

   moveMode = saved_move_mode;
   stretchableText = saved_stretchable_text;
   horiAlign = saved_h_align;
   vertAlign = saved_v_align;
   printMag = saved_print_mag;

   if (UpdPageStyle(pageStyle)) {
      UpdDrawWinBBox();
      AdjSplineVs();
   }
}

void SizeAllSelObj(AbsW, AbsH)
   int AbsW, AbsH;
{
   int saved_h_align=horiAlign, saved_v_align=vertAlign, num_to_resize=0;
   char **ppsz_names_to_resize=NULL;
   struct BBRec obbox;

   if (topSel == NULL) {
      MsgBox(TgLoadCachedString(CSTID_NO_OBJ_SELECTED), TOOL_NAME, INFO_MB);
      return;
   }
   if (AbsW == selObjRbX-selObjLtX && AbsH == selObjRbY-selObjLtY) return;

   obbox.ltx = selObjLtX; obbox.lty = selObjLtY;
   obbox.rbx = selObjRbX; obbox.rby = selObjRbY;
   SetPivot(CORNER_RB, &obbox);

   horiAlign = ALIGN_L;
   vertAlign = ALIGN_T;
   multX = (selObjRbX==selObjLtX ? ((double)1.0) :
         ((double)AbsW) / ((double)selObjRbX-selObjLtX));
   multY = (selObjRbY==selObjLtY ? ((double)1.0) :
         ((double)AbsH) / ((double)selObjRbY-selObjLtY));
   changeX = (fabs(multX-1.0) > 1.0e-6);
   changeY = (fabs(multY-1.0) > 1.0e-6);
   ppsz_names_to_resize = NeedToProcessOnResize(&num_to_resize);
   if (ppsz_names_to_resize == NULL) {
      ScaleAllSel(CORNER_RB, (double)(multX*1000.0), (double)(multY*1000.0),
            TRUE);
   } else {
      StartCompositeCmd();
      ScaleAllSel(CORNER_RB, (double)(multX*1000.0), (double)(multY*1000.0),
            TRUE);
      DoOnResize(ppsz_names_to_resize, num_to_resize);
      EndCompositeCmd();
   }
   horiAlign = saved_h_align;
   vertAlign = saved_v_align;

   UpdSelBBox();
   SetFileModified(TRUE);
   justDupped = FALSE;
}

void SizeAnObj(ObjPtr, TopOwner, AbsW, AbsH)
   struct ObjRec *ObjPtr, *TopOwner;
   int AbsW, AbsH;
   /* This function is meant to be called from within an internal command */
{
   int obj_w=ObjPtr->obbox.rbx-ObjPtr->obbox.ltx;
   int obj_h=ObjPtr->obbox.rby-ObjPtr->obbox.lty;

   if (execCurDepth <= 0) {
#ifdef _TGIF_DBG /* debug, do not translate */
      TgAssert(FALSE,
            "SizeAnObj() called not from an internal command!", NULL);
      return;
#endif /* _TGIF_DBG */
   }
   if (obj_w == AbsW && obj_h == AbsH) {
      return;
   }
   if (ObjPtr == TopOwner) {
      struct SelRec *saved_top_sel=topSel, *saved_bot_sel=botSel;

      topSel = botSel = NULL;
      AddObjIntoSel(ObjPtr, NULL, topSel, &topSel, &botSel);
      UpdSelBBox();

      SizeAllSelObj(AbsW, AbsH);

      RemoveAllSel();
      topSel = saved_top_sel;
      botSel = saved_bot_sel;
      UpdSelBBox();
   } else {
      int saved_h_align=horiAlign, saved_v_align=vertAlign;
      int ltx=TopOwner->bbox.ltx, lty=TopOwner->bbox.lty;
      int rbx=TopOwner->bbox.rbx, rby=TopOwner->bbox.rby;
      struct SelRec *saved_top_sel=topSel, *saved_bot_sel=botSel;

      SetPivot(CORNER_RB, &ObjPtr->obbox);

      multX = (obj_w == 0) ? ((double)1.0) : ((double)AbsW) / ((double)obj_w);
      multY = (obj_h == 0) ? ((double)1.0) : ((double)AbsH) / ((double)obj_h);
      changeX = (fabs(multX-1.0) > 1.0e-6);
      changeY = (fabs(multY-1.0) > 1.0e-6);
      if (!changeX && !changeY) {
         return;
      }
      horiAlign = ALIGN_L;
      vertAlign = ALIGN_T;

      topSel = botSel = NULL;
      UpdSelBBox();

      PrepareToReplaceAnObj(TopOwner);

      StretchObj(ObjPtr, CORNER_RB, (double)(multX*1000.0),
            (double)(multY*1000.0), FALSE);
      while (ObjPtr != TopOwner) {
         ObjPtr = ObjPtr->tmp_parent;
         AdjObjBBox(ObjPtr);
      }
      RecordReplaceAnObj(TopOwner);
      RedrawAreas(botObj, ltx-GRID_ABS_SIZE(1), lty-GRID_ABS_SIZE(1),
            rbx+GRID_ABS_SIZE(1), rby+GRID_ABS_SIZE(1),
            TopOwner->bbox.ltx-GRID_ABS_SIZE(1),
            TopOwner->bbox.lty-GRID_ABS_SIZE(1),
            TopOwner->bbox.rbx+GRID_ABS_SIZE(1),
            TopOwner->bbox.rby+GRID_ABS_SIZE(1));

      RemoveAllSel();
      topSel = saved_top_sel;
      botSel = saved_bot_sel;
      UpdSelBBox();

      SetFileModified(TRUE);
      justDupped = FALSE;
      horiAlign = saved_h_align;
      vertAlign = saved_v_align;
   }
}

static
void DoSizeAllSelToGivenWidthHeight(abs_w, abs_h, do_width, do_height)
   int abs_h, do_width, do_height;
{
   struct SelRec *saved_top_sel=topSel, *saved_bot_sel=botSel, *sel_ptr=NULL;
   int saved_h_align=horiAlign, saved_v_align=vertAlign, num_to_resize=0;
   char **ppsz_names_to_resize=NULL;

   if (topSel == NULL || (!do_width && !do_height)) {
      return;
   }
   horiAlign = ALIGN_L;
   vertAlign = ALIGN_T;

   ppsz_names_to_resize = NeedToProcessOnResize(&num_to_resize);

   HighLightReverse();
   StartCompositeCmd();
   for (sel_ptr=saved_top_sel; sel_ptr != NULL; sel_ptr=sel_ptr->next) {
      struct ObjRec *obj_ptr=sel_ptr->obj;
      int w=(obj_ptr->obbox.rbx-obj_ptr->obbox.ltx);
      int h=(obj_ptr->obbox.rby-obj_ptr->obbox.lty);

      topSel = botSel = SelectThisObject(obj_ptr);
      UpdSelBBox();
      if (do_width && do_height) {
         SetPivot(CORNER_RB, &obj_ptr->obbox);
         multX = (w == 0 ? ((double)1.0) : ((double)abs_w) / ((double)w));
         multY = (h == 0 ? ((double)1.0) : ((double)abs_h) / ((double)h));
         changeX = (fabs(multX-1.0) > 1.0e-6);
         changeY = (fabs(multY-1.0) > 1.0e-6);
         ScaleAllSel(CORNER_RB, (double)(multX*1000.0), (double)(multY*1000.0),
               TRUE);
      } else if (do_width) {
         SetPivot(CORNER_RIGHT, &obj_ptr->obbox);
         multX = (w == 0 ? ((double)1.0) : ((double)abs_w) / ((double)w));
         changeX = (fabs(multX-1.0) > 1.0e-6);
         ScaleAllSel(CORNER_RIGHT, (double)(multX*1000.0), (double)1000, TRUE);
      } else {
         SetPivot(CORNER_BOTTOM, &obj_ptr->obbox);
         multY = (h == 0 ? ((double)1.0) : ((double)abs_h) / ((double)h));
         changeY = (fabs(multY-1.0) > 1.0e-6);
         ScaleAllSel(CORNER_BOTTOM, (double)1000, (double)(multY*1000.0), TRUE);
      }
      free(topSel);
   }
   if (ppsz_names_to_resize != NULL) {
      DoOnResize(ppsz_names_to_resize, num_to_resize);
   }
   EndCompositeCmd();

   horiAlign = saved_h_align;
   vertAlign = saved_v_align;

   topSel = saved_top_sel;
   botSel = saved_bot_sel;
   UpdSelBBox();

   HighLightForward();
   SetFileModified(TRUE);
   justDupped = FALSE;
}

void SizeAllSelToGivenWidthHeight(AbsW, AbsH)
   int AbsW, AbsH;
{
   DoSizeAllSelToGivenWidthHeight(AbsW, AbsH, TRUE, TRUE);
}

void SizeAllSelToGivenWidth(AbsW)
   int AbsW;
{
   DoSizeAllSelToGivenWidthHeight(AbsW, INVALID, TRUE, FALSE);
}

void SizeAllSelToGivenHeight(AbsH)
   int AbsH;
{
   DoSizeAllSelToGivenWidthHeight(INVALID, AbsH, FALSE, TRUE);
}

void FlipObjHorizontal(ObjPtr)
   struct ObjRec *ObjPtr;
{
   int two_x_pivot=selNoLockObjLtX+selNoLockObjRbX;
   int new_obj_ltx=two_x_pivot-ObjPtr->obbox.rbx;
   int new_obj_rbx=two_x_pivot-ObjPtr->obbox.ltx;
   int new_obj_lty=ObjPtr->obbox.lty;

   if (ObjPtr->ctm == NULL && ObjPtr->type != OBJ_XBM &&
         ObjPtr->type != OBJ_XPM) {
      register IntPoint *v;
      register int i;
      int num_pts;
      struct ObjRec *obj_ptr;
      struct AttrRec *attr_ptr;
      struct ArcRec *arc_ptr;

      switch (ObjPtr->type) {
      case OBJ_TEXT:
         if (ObjPtr->detail.t->minilines.just != JUST_C) {
            ObjPtr->detail.t->minilines.just =
                  MAXJUSTS-1-ObjPtr->detail.t->minilines.just;
            if (ObjPtr->detail.t->cached_bitmap != None) {
               XFreePixmap(mainDisplay, ObjPtr->detail.t->cached_bitmap);
            }
            ObjPtr->detail.t->cached_bitmap = None;

            if (zoomScale != 0) {
               ObjPtr->detail.t->cached_zoom = 0;
            }
         }
         MoveObj(ObjPtr, two_x_pivot-((ObjPtr->x)<<1), 0);
         UpdTextBBox(ObjPtr);
         break;

      default:
         switch (ObjPtr->type) {
         case OBJ_XBM:
            ObjPtr->detail.xbm->flip ^= HORI_EVEN;
            if (ObjPtr->detail.xbm->cached_bitmap != None) {
               XFreePixmap(mainDisplay, ObjPtr->detail.xbm->cached_bitmap);
            }
            ObjPtr->detail.xbm->cached_bitmap = None;

            if (zoomScale != 0) {
               ObjPtr->detail.xbm->cached_zoom = 0;
            }
            break;
         case OBJ_XPM:
            ObjPtr->detail.xpm->flip ^= HORI_EVEN;
            if (ObjPtr->detail.xpm->cached_pixmap != None) {
               XFreePixmap(mainDisplay, ObjPtr->detail.xpm->cached_pixmap);
            }
            ObjPtr->detail.xpm->cached_pixmap = None;
            if (ObjPtr->detail.xpm->cached_bitmap != None) {
               XFreePixmap(mainDisplay, ObjPtr->detail.xpm->cached_bitmap);
            }
            ObjPtr->detail.xpm->cached_bitmap = None;
            ObjPtr->detail.xpm->cached_color = (-1);

            if (zoomScale != 0) {
               ObjPtr->detail.xpm->cached_zoom = 0;
            }
            break;
         case OBJ_ICON:
         case OBJ_PIN:
            ObjPtr->detail.r->flip ^= HORI_EVEN;
            break;
         }
         ObjPtr->obbox.ltx = ObjPtr->x = new_obj_ltx;
         ObjPtr->obbox.rbx = new_obj_rbx;
         break;
      }

      switch (ObjPtr->type) {
      case OBJ_POLY:
         num_pts = ObjPtr->detail.p->n;
         v = ObjPtr->detail.p->vlist;
         for (i = 0; i < num_pts; i++, v++) (*v).x = two_x_pivot - (*v).x;
         AdjObjSplineVs(ObjPtr);
         attr_ptr = ObjPtr->fattr;
         for ( ; attr_ptr != NULL; attr_ptr = attr_ptr->next) {
            FlipObjHorizontal(attr_ptr->obj);
         }
         break;
      case OBJ_POLYGON:
         num_pts = ObjPtr->detail.g->n;
         v = ObjPtr->detail.g->vlist;
         for (i = 0; i < num_pts; i++, v++) (*v).x = two_x_pivot - (*v).x;
         AdjObjSplineVs(ObjPtr);
         attr_ptr = ObjPtr->fattr;
         for ( ; attr_ptr != NULL; attr_ptr = attr_ptr->next) {
            FlipObjHorizontal(attr_ptr->obj);
         }
         break;
      case OBJ_BOX:
      case OBJ_OVAL:
      case OBJ_RCBOX:
      case OBJ_XBM:
      case OBJ_XPM:
         attr_ptr = ObjPtr->fattr;
         for ( ; attr_ptr != NULL; attr_ptr = attr_ptr->next) {
            FlipObjHorizontal(attr_ptr->obj);
         }
         AdjObjSplineVs(ObjPtr);
         break;
      case OBJ_TEXT:
         AdjObjSplineVs(ObjPtr);
         break;
      case OBJ_ARC:
         arc_ptr = ObjPtr->detail.a;
         arc_ptr->xc = two_x_pivot - arc_ptr->xc;
         arc_ptr->x1 = two_x_pivot - arc_ptr->x1;
         arc_ptr->x2 = two_x_pivot - arc_ptr->x2;
         arc_ptr->dir = !(arc_ptr->dir);
         arc_ptr->ltx = two_x_pivot - arc_ptr->ltx - arc_ptr->w;
         if (arc_ptr->angle1 > 0) {
            arc_ptr->angle1 = (180*64) - arc_ptr->angle1;
         } else {
            arc_ptr->angle1 = (-180)*64 - arc_ptr->angle1;
         }
         arc_ptr->angle2 = -(arc_ptr->angle2);
         AdjObjBBox(ObjPtr);
         attr_ptr = ObjPtr->fattr;
         for ( ; attr_ptr != NULL; attr_ptr = attr_ptr->next) {
            FlipObjHorizontal(attr_ptr->obj);
         }
         AdjObjSplineVs(ObjPtr);
         break;
      case OBJ_GROUP:
      case OBJ_ICON:
      case OBJ_SYM:
      case OBJ_PIN:
         obj_ptr = ObjPtr->detail.r->first;
         for ( ; obj_ptr != NULL; obj_ptr = obj_ptr->next) {
            FlipObjHorizontal(obj_ptr);
         }
         attr_ptr = ObjPtr->fattr;
         for ( ; attr_ptr != NULL; attr_ptr = attr_ptr->next) {
            FlipObjHorizontal(attr_ptr->obj);
         }
         AdjObjSplineVs(ObjPtr);
         break;
      }
      AdjObjBBox(ObjPtr);
   } else {
      double dz=(double)0, d1=(double)1000, dm1=(double)-1000;

      ShearObj(ObjPtr, CORNER_LEFT, dz, dz, dm1, d1, NULL, NULL);
      MoveObj(ObjPtr, new_obj_ltx-ObjPtr->obbox.ltx,
            new_obj_lty-ObjPtr->obbox.lty);
   }
   SetFileModified(TRUE);
}

void FlipIconHorizontal(ObjPtr)
   struct ObjRec *ObjPtr;
{
   register int two_x_pivot;
   int new_obj_ltx, new_obj_rbx;
   struct ObjRec *obj_ptr;
   struct AttrRec *attr_ptr;

   two_x_pivot = selNoLockObjLtX + selNoLockObjRbX;
   new_obj_ltx = two_x_pivot - ObjPtr->obbox.rbx;
   new_obj_rbx = two_x_pivot - ObjPtr->obbox.ltx;

   ObjPtr->detail.r->flip ^= HORI_EVEN;

   ObjPtr->obbox.ltx = ObjPtr->x = new_obj_ltx;
   ObjPtr->obbox.rbx = new_obj_rbx;

   obj_ptr = ObjPtr->detail.r->first;
   for ( ; obj_ptr != NULL; obj_ptr = obj_ptr->next) {
      FlipObjHorizontal(obj_ptr);
   }
   attr_ptr = ObjPtr->fattr;
   for ( ; attr_ptr != NULL; attr_ptr = attr_ptr->next) {
      FlipObjHorizontal(attr_ptr->obj);
   }
   AdjObjBBox(ObjPtr);
}

void FlipObjVertical(ObjPtr)
   struct ObjRec *ObjPtr;
{
   int two_x_pivot=selNoLockObjLtY+selNoLockObjRbY;
   int new_obj_lty=two_x_pivot-ObjPtr->obbox.rby;
   int new_obj_rby=two_x_pivot-ObjPtr->obbox.lty;
   int new_obj_ltx=ObjPtr->obbox.ltx;

   if (ObjPtr->ctm == NULL && ObjPtr->type != OBJ_XBM &&
         ObjPtr->type != OBJ_XPM) {
      register IntPoint *v;
      register int i;
      int num_pts;
      struct ObjRec *obj_ptr;
      struct AttrRec *attr_ptr;
      struct ArcRec *arc_ptr;

      switch (ObjPtr->type) {
      case OBJ_TEXT:
         MoveObj(ObjPtr, 0, new_obj_lty-ObjPtr->y);
         UpdTextBBox(ObjPtr);
         break;

      default:
         switch (ObjPtr->type) {
         case OBJ_XBM:
            ObjPtr->detail.xbm->flip ^= VERT_EVEN;
            if (ObjPtr->detail.xbm->cached_bitmap != None) {
               XFreePixmap(mainDisplay, ObjPtr->detail.xbm->cached_bitmap);
            }
            ObjPtr->detail.xbm->cached_bitmap = None;

            if (zoomScale != 0) {
               ObjPtr->detail.xbm->cached_zoom = 0;
            }
            break;
         case OBJ_XPM:
            ObjPtr->detail.xpm->flip ^= VERT_EVEN;
            if (ObjPtr->detail.xpm->cached_pixmap != None) {
               XFreePixmap(mainDisplay, ObjPtr->detail.xpm->cached_pixmap);
            }
            ObjPtr->detail.xpm->cached_pixmap = None;
            if (ObjPtr->detail.xpm->cached_bitmap != None) {
               XFreePixmap(mainDisplay, ObjPtr->detail.xpm->cached_bitmap);
            }
            ObjPtr->detail.xpm->cached_bitmap = None;
            ObjPtr->detail.xpm->cached_color = (-1);

            if (zoomScale != 0) {
               ObjPtr->detail.xpm->cached_zoom = 0;
            }
            break;
         case OBJ_ICON:
         case OBJ_PIN:
            ObjPtr->detail.r->flip ^= VERT_EVEN;
            break;
         }
         ObjPtr->obbox.lty = ObjPtr->y = new_obj_lty;
         ObjPtr->obbox.rby = new_obj_rby;
         break;
      }

      switch (ObjPtr->type) {
      case OBJ_POLY:
         num_pts = ObjPtr->detail.p->n;
         v = ObjPtr->detail.p->vlist;
         for (i = 0; i < num_pts; i++, v++) (*v).y = two_x_pivot - (*v).y;
         AdjObjSplineVs(ObjPtr);
         attr_ptr = ObjPtr->fattr;
         for ( ; attr_ptr != NULL; attr_ptr = attr_ptr->next) {
            FlipObjVertical(attr_ptr->obj);
         }
         break;
      case OBJ_POLYGON:
         num_pts = ObjPtr->detail.g->n;
         v = ObjPtr->detail.g->vlist;
         for (i = 0; i < num_pts; i++, v++) (*v).y = two_x_pivot - (*v).y;
         AdjObjSplineVs(ObjPtr);
         attr_ptr = ObjPtr->fattr;
         for ( ; attr_ptr != NULL; attr_ptr = attr_ptr->next) {
            FlipObjVertical(attr_ptr->obj);
         }
         break;
      case OBJ_BOX:
      case OBJ_OVAL:
      case OBJ_RCBOX:
      case OBJ_XBM:
      case OBJ_XPM:
         attr_ptr = ObjPtr->fattr;
         for ( ; attr_ptr != NULL; attr_ptr = attr_ptr->next) {
            FlipObjVertical(attr_ptr->obj);
         }
         AdjObjSplineVs(ObjPtr);
         break;
      case OBJ_TEXT:
         AdjObjSplineVs(ObjPtr);
         break;
      case OBJ_ARC:
         arc_ptr = ObjPtr->detail.a;
         arc_ptr->yc = two_x_pivot - arc_ptr->yc;
         arc_ptr->y1 = two_x_pivot - arc_ptr->y1;
         arc_ptr->y2 = two_x_pivot - arc_ptr->y2;
         arc_ptr->dir = !(arc_ptr->dir);
         arc_ptr->lty = two_x_pivot - arc_ptr->lty - arc_ptr->h;
         arc_ptr->angle1 = -(arc_ptr->angle1);
         arc_ptr->angle2 = -(arc_ptr->angle2);
         AdjObjBBox(ObjPtr);
         attr_ptr = ObjPtr->fattr;
         for ( ; attr_ptr != NULL; attr_ptr = attr_ptr->next) {
            FlipObjVertical(attr_ptr->obj);
         }
         AdjObjSplineVs(ObjPtr);
         break;
      case OBJ_GROUP:
      case OBJ_ICON:
      case OBJ_SYM:
      case OBJ_PIN:
         obj_ptr = ObjPtr->detail.r->first;
         for ( ; obj_ptr != NULL; obj_ptr = obj_ptr->next) {
            FlipObjVertical(obj_ptr);
         }
         attr_ptr = ObjPtr->fattr;
         for ( ; attr_ptr != NULL; attr_ptr = attr_ptr->next) {
            FlipObjVertical(attr_ptr->obj);
         }
         AdjObjSplineVs(ObjPtr);
         break;
      }
      AdjObjBBox(ObjPtr);
   } else {
      double dz=(double)0, d1=(double)1000, dm1=(double)-1000;

      ShearObj(ObjPtr, CORNER_TOP, dz, dz, d1, dm1, NULL, NULL);
      MoveObj(ObjPtr, new_obj_ltx-ObjPtr->obbox.ltx,
            new_obj_lty-ObjPtr->obbox.lty);
   }
   SetFileModified(TRUE);
}

void FlipIconVertical(ObjPtr)
   struct ObjRec *ObjPtr;
{
   register int two_x_pivot;
   int new_obj_lty, new_obj_rby;
   struct ObjRec *obj_ptr;
   struct AttrRec *attr_ptr;

   two_x_pivot = selNoLockObjLtY + selNoLockObjRbY;
   new_obj_lty = two_x_pivot - ObjPtr->obbox.rby;
   new_obj_rby = two_x_pivot - ObjPtr->obbox.lty;

   ObjPtr->detail.r->flip ^= VERT_EVEN;

   ObjPtr->obbox.lty = ObjPtr->y = new_obj_lty;
   ObjPtr->obbox.rby = new_obj_rby;

   obj_ptr = ObjPtr->detail.r->first;
   for ( ; obj_ptr != NULL; obj_ptr = obj_ptr->next) {
      FlipObjVertical(obj_ptr);
   }
   attr_ptr = ObjPtr->fattr;
   for ( ; attr_ptr != NULL; attr_ptr = attr_ptr->next) {
      FlipObjVertical(attr_ptr->obj);
   }
   AdjObjBBox(ObjPtr);
}

/* --------------------- Rotation --------------------- */

static int	rotatePivotX=0;
static int	rotatePivotY=0;

static
int OkToTransform(obj_ptr, stid)
   struct ObjRec *obj_ptr;
   int stid;
{
   char obj_name[MAXSTRING];
   struct AttrRec *name_attr=NULL;

   if (obj_ptr->locked) return FALSE;

   *obj_name = '\0';
   if (HasOnResize(obj_ptr, &name_attr) && name_attr != NULL) {
      UtilStrCpyN(obj_name, sizeof(obj_name), name_attr->attr_value.s);
      sprintf(gszMsgBox, TgLoadString(stid), obj_name);
      if (MsgBox(gszMsgBox, TOOL_NAME, YN_MB) != MB_ID_YES) {
         return FALSE;
      }
   }
   return TRUE;
}

void SetRotatePivot()
{
   if (!autoRotatePivot && rotatePivotAbsXYValid) {
      rotatePivotX = rotatePivotAbsX;
      rotatePivotY = rotatePivotAbsY;
   } else {
      rotatePivotX = ((selNoLockObjLtX + selNoLockObjRbX)>>1);
      rotatePivotY = ((selNoLockObjLtY + selNoLockObjRbY)>>1);
   }
}

void SetRotatePivotByObject(ObjPtr)
   struct ObjRec *ObjPtr;
{
   rotatePivotX = ((ObjPtr->obbox.ltx + ObjPtr->obbox.rbx)>>1);
   rotatePivotY = ((ObjPtr->obbox.lty + ObjPtr->obbox.rby)>>1);
}

static
void RotatePtClockWise(X, Y, NewX, NewY)
   int X, Y, *NewX, *NewY;
{
   *NewX = rotatePivotX + rotatePivotY - Y;
   *NewY = rotatePivotY - rotatePivotX + X;
}

static
void RotatedXY(X, Y, AngleDelta, NewX, NewY)
   int X, Y, AngleDelta, *NewX, *NewY; /* AngleDelta is degree*64 */
{
   register double radian, sin_val, cos_val;
   int dx=X-pivotX, dy=Y-pivotY;

   if (dx == 0 && dy == 0) {
      *NewX = pivotX;
      *NewY = pivotY;
   } else {
      radian = (((double)AngleDelta)*M_PI/180.0/64.0);
      sin_val = sin(radian);
      cos_val = cos(radian);
      *NewX = pivotX + round(dx*cos_val - dy*sin_val);
      *NewY = pivotY + round(dx*sin_val + dy*cos_val);
   }
}

static
void RotatedAbsXY(X, Y, AngleDelta, NewX, NewY)
   int X, Y, AngleDelta, *NewX, *NewY; /* AngleDelta is degree*64 */
{
   register double radian, sin_val, cos_val;
   int dx=X-absPivotX, dy=Y-absPivotY;

   if (dx == 0 && dy == 0) {
      *NewX = absPivotX;
      *NewY = absPivotY;
   } else {
      radian = (((double)AngleDelta)*M_PI/180.0/64.0);
      sin_val = sin(radian);
      cos_val = cos(radian);
      *NewX = absPivotX + round(dx*cos_val - dy*sin_val);
      *NewY = absPivotY + round(dx*sin_val + dy*cos_val);
   }
}

void RotateObj(ObjPtr, Corner, AngleDelta, RealLtX, RealLtY)
   struct ObjRec *ObjPtr;
   int Corner, AngleDelta; /* AngleDelta is degree*64 */
   int *RealLtX, *RealLtY;
{
   IntPoint abs_obj_obbox_vs[5];
   int x, y, new_ltx, new_lty, new_rbx, new_rby;
   double radian=(((double)AngleDelta)*M_PI/180.0/64.0);
   double sin_val=sin(radian), cos_val=cos(radian);
   struct XfrmMtrxRec ctm, new_ctm;
   struct ObjRec *obj_ptr;
   struct AttrRec *attr_ptr;

   switch (ObjPtr->type) {
   case OBJ_GROUP:
   case OBJ_ICON:
   case OBJ_SYM:
   case OBJ_PIN:
      for (obj_ptr=ObjPtr->detail.r->first; obj_ptr != NULL;
            obj_ptr=obj_ptr->next) {
         RotateObj(obj_ptr, Corner, AngleDelta, RealLtX, RealLtY);
      }
      break;

   default:
      if (ObjPtr->ctm == NULL) {
         memcpy(&ObjPtr->orig_obbox, &ObjPtr->obbox, sizeof(struct BBRec));
         if (ObjPtr->type == OBJ_TEXT) {
            memcpy(&ObjPtr->detail.t->orig_bbox, &ObjPtr->bbox,
                  sizeof(struct BBRec));
         }
         ObjPtr->ctm = (struct XfrmMtrxRec *)malloc(sizeof(struct XfrmMtrxRec));
         if (ObjPtr->ctm == NULL) FailAllocMessage();
         ObjPtr->ctm->m[CTM_SX] = ObjPtr->ctm->m[CTM_SY] = (double)1000;
         ObjPtr->ctm->m[CTM_SIN] = ObjPtr->ctm->m[CTM_MSIN] = (double)0;
         ObjPtr->ctm->t[CTM_TX] = ObjPtr->ctm->t[CTM_TY] = 0;
      }
      RotatedAbsXY(ObjPtr->x+ObjPtr->ctm->t[CTM_TX],
            ObjPtr->y+ObjPtr->ctm->t[CTM_TY], AngleDelta, &x, &y);
      ctm.m[CTM_SX] = ctm.m[CTM_SY] = ((double)1000.0)*cos_val;
      ctm.m[CTM_SIN] = ((double)1000.0)*sin_val;
      ctm.m[CTM_MSIN] = (-ctm.m[CTM_SIN]);
      ctm.t[CTM_TX] = 0;
      ctm.t[CTM_TY] = 0;
      ConcatCTM(ObjPtr->ctm, &ctm, &new_ctm);
      new_ctm.t[CTM_TX] = x-ObjPtr->x;
      new_ctm.t[CTM_TY] = y-ObjPtr->y;
      memcpy(ObjPtr->ctm, &new_ctm, sizeof(struct XfrmMtrxRec));

      GetTransformedOBBoxAbsVs(ObjPtr, abs_obj_obbox_vs);

      new_ltx = min(min(abs_obj_obbox_vs[0].x,abs_obj_obbox_vs[1].x),
            min(abs_obj_obbox_vs[2].x,abs_obj_obbox_vs[3].x));
      new_rbx = max(max(abs_obj_obbox_vs[0].x,abs_obj_obbox_vs[1].x),
            max(abs_obj_obbox_vs[2].x,abs_obj_obbox_vs[3].x));
      new_lty = min(min(abs_obj_obbox_vs[0].y,abs_obj_obbox_vs[1].y),
            min(abs_obj_obbox_vs[2].y,abs_obj_obbox_vs[3].y));
      new_rby = max(max(abs_obj_obbox_vs[0].y,abs_obj_obbox_vs[1].y),
            max(abs_obj_obbox_vs[2].y,abs_obj_obbox_vs[3].y));

      ObjPtr->obbox.ltx = new_ltx; ObjPtr->obbox.lty = new_lty;
      ObjPtr->obbox.rbx = new_rbx; ObjPtr->obbox.rby = new_rby;
      if (RealLtX != NULL && RealLtY != NULL) {
         int dx=(*RealLtX)-new_ltx, dy=(*RealLtY)-new_lty;

         ObjPtr->x += dx; ObjPtr->y += dy;
         ObjPtr->bbox.ltx += dx; ObjPtr->bbox.lty += dy;
         ObjPtr->bbox.rbx += dx; ObjPtr->bbox.rby += dy;
         ObjPtr->obbox.ltx += dx; ObjPtr->obbox.lty += dy;
         ObjPtr->obbox.rbx += dx; ObjPtr->obbox.rby += dy;
         MoveRotatedObjCache(ObjPtr, dx, dy);
      }
      if (ObjPtr->ctm->m[CTM_SX] >= 999.0 &&
            ObjPtr->ctm->m[CTM_SX] <= 1001.0 &&
            ObjPtr->ctm->m[CTM_SY] >= 999.0 &&
            ObjPtr->ctm->m[CTM_SY] <= 1001.0 &&
            ObjPtr->ctm->m[CTM_SIN] >= (-1.0) &&
            ObjPtr->ctm->m[CTM_SIN] <= 1.0 &&
            ObjPtr->ctm->m[CTM_MSIN] >= (-1.0) &&
            ObjPtr->ctm->m[CTM_MSIN] <= 1.0) {
         int dx=ObjPtr->ctm->t[CTM_TX], dy=ObjPtr->ctm->t[CTM_TY];
         struct AttrRec *saved_fattr=NULL, *saved_lattr=NULL;

         free(ObjPtr->ctm);
         ObjPtr->ctm = NULL;

         memcpy(&ObjPtr->obbox, &ObjPtr->orig_obbox, sizeof(struct BBRec));
         if (ObjPtr->type == OBJ_TEXT) {
            memcpy(&ObjPtr->bbox, &ObjPtr->detail.t->orig_bbox,
                  sizeof(struct BBRec));
         }
         saved_fattr = ObjPtr->fattr;
         saved_lattr = ObjPtr->lattr;
         ObjPtr->fattr = ObjPtr->lattr = NULL;
         MoveObj(ObjPtr, dx, dy);
         ObjPtr->fattr = saved_fattr;
         ObjPtr->lattr = saved_lattr;
      }
      break;
   }
   for (attr_ptr=ObjPtr->fattr; attr_ptr != NULL; attr_ptr=attr_ptr->next) {
      RotateObj(attr_ptr->obj, Corner, AngleDelta, RealLtX, RealLtY);
   }
   AdjObjCache(ObjPtr);
   AdjObjBBox(ObjPtr);
}

void RotateObjForLayout(ObjPtr, AngleInRadian, Corner)
   struct ObjRec *ObjPtr;
   double AngleInRadian;
   int Corner;
{
   double angle=AngleInRadian*64.0*180.0/M_PI;

   SetPivot(Corner, &ObjPtr->obbox);
   RotateObj(ObjPtr, Corner, round(angle), NULL, NULL);
}

void RotateObjClockWise(ObjPtr)
   struct ObjRec *ObjPtr;
{
   double angle_in_radian=((double)(rotationIncrement))*M_PI/180.0/64.0;
   double sin_val=sin(angle_in_radian);
   double cos_val=cos(angle_in_radian);
   int orig_x=((ObjPtr->obbox.ltx+ObjPtr->obbox.rbx)>>1);
   int orig_y=ObjPtr->obbox.lty;
   int x=0, y=0, dx=orig_x-rotatePivotX, dy=orig_y-rotatePivotY;

   if (dx != 0 || dy != 0) {
      x = (int)round(dx*cos_val - dy*sin_val);
      y = (int)round(dx*sin_val + dy*cos_val);
   }
   x += rotatePivotX;
   y += rotatePivotY;
   /* RotateObjForLayout() rotates about center-top */
   RotateObjForLayout(ObjPtr, angle_in_radian, CORNER_BOTTOM);
   MoveObj(ObjPtr, x-orig_x, y-orig_y);
   SetFileModified(TRUE);
}

void RotateIconClockWise(ObjPtr)
   struct ObjRec *ObjPtr;
{
   int ltx, lty, rbx, rby;
   struct ObjRec *obj_ptr;
   struct AttrRec *attr_ptr;

   SetRotatePivot();
   RotatePtClockWise(ObjPtr->obbox.ltx, ObjPtr->obbox.rby, &ltx, &lty);
   RotatePtClockWise(ObjPtr->obbox.rbx, ObjPtr->obbox.lty, &rbx, &rby);
   ObjPtr->obbox.ltx = ObjPtr->x = ltx;
   ObjPtr->obbox.lty = ObjPtr->y = lty;
   ObjPtr->obbox.rbx = rbx;
   ObjPtr->obbox.rby = rby;

   obj_ptr = ObjPtr->detail.r->first;
   for ( ; obj_ptr != NULL; obj_ptr = obj_ptr->next) {
      RotateObjClockWise(obj_ptr);
   }
   attr_ptr = ObjPtr->fattr;
   for ( ; attr_ptr != NULL; attr_ptr = attr_ptr->next) {
      RotateObjClockWise(attr_ptr->obj);
   }
   AdjObjBBox(ObjPtr);
}

static
void RotatePtCounter(X, Y, NewX, NewY)
   int X, Y, *NewX, *NewY;
{
   *NewX = rotatePivotX - rotatePivotY + Y;
   *NewY = rotatePivotY + rotatePivotX - X;
}

void RotateObjCounter(ObjPtr)
   struct ObjRec *ObjPtr;
{
   double angle_in_radian=((double)(-rotationIncrement))*M_PI/180.0/64.0;
   double sin_val=sin(angle_in_radian);
   double cos_val=cos(angle_in_radian);
   int orig_x=((ObjPtr->obbox.ltx+ObjPtr->obbox.rbx)>>1);
   int orig_y=ObjPtr->obbox.lty;
   int x=0, y=0, dx=orig_x-rotatePivotX, dy=orig_y-rotatePivotY;

   if (dx != 0 || dy != 0) {
      x = (int)round(dx*cos_val - dy*sin_val);
      y = (int)round(dx*sin_val + dy*cos_val);
   }
   x += rotatePivotX;
   y += rotatePivotY;
   /* RotateObjForLayout() rotates about center-top */
   RotateObjForLayout(ObjPtr, angle_in_radian, CORNER_BOTTOM);
   MoveObj(ObjPtr, x-orig_x, y-orig_y);
   SetFileModified(TRUE);
}

void RotateIconCounter(ObjPtr)
   struct ObjRec *ObjPtr;
{
   int ltx, lty, rbx, rby;
   struct ObjRec *obj_ptr;
   struct AttrRec *attr_ptr;

   SetRotatePivot();
/* ObjPtr->detail.r->rotate = (ObjPtr->detail.r->rotate+4-1) % 4; */
   RotatePtCounter(ObjPtr->obbox.rbx, ObjPtr->obbox.lty, &ltx, &lty);
   RotatePtCounter(ObjPtr->obbox.ltx, ObjPtr->obbox.rby, &rbx, &rby);
   ObjPtr->obbox.ltx = ObjPtr->x = ltx;
   ObjPtr->obbox.lty = ObjPtr->y = lty;
   ObjPtr->obbox.rbx = rbx;
   ObjPtr->obbox.rby = rby;

   obj_ptr = ObjPtr->detail.r->first;
   for ( ; obj_ptr != NULL; obj_ptr = obj_ptr->next) {
      RotateObjCounter(obj_ptr);
   }
   attr_ptr = ObjPtr->fattr;
   for ( ; attr_ptr != NULL; attr_ptr = attr_ptr->next) {
      RotateObjCounter(attr_ptr->obj);
   }
   AdjObjBBox(ObjPtr);
}

static
void FlipAllSelHorizontal(saved_ltx, saved_lty, saved_rbx, saved_rby)
   int saved_ltx, saved_lty, saved_rbx, saved_rby;
{
   struct SelRec *sel_ptr;

   PrepareToRecord(CMD_REPLACE, topSel, botSel, numObjSelected);
   JustRemoveAllVSel();
   for (sel_ptr = topSel; sel_ptr != NULL; sel_ptr = sel_ptr->next) {
      if (!sel_ptr->obj->locked) {
         FlipObjHorizontal(sel_ptr->obj);
      }
   }
   UpdSelBBox();
   RecordCmd(CMD_REPLACE, NULL, topSel, botSel, numObjSelected);
   RedrawAreas(botObj, saved_ltx-GRID_ABS_SIZE(1), saved_lty-GRID_ABS_SIZE(1),
         saved_rbx+GRID_ABS_SIZE(1), saved_rby+GRID_ABS_SIZE(1),
         selLtX-GRID_ABS_SIZE(1), selLtY-GRID_ABS_SIZE(1),
         selRbX+GRID_ABS_SIZE(1), selRbY+GRID_ABS_SIZE(1));
}

void FlipHorizontal()
{
   int saved_ltx, saved_lty, saved_rbx, saved_rby, num_to_resize=0;
   struct BBRec sel_obbox;
   char **ppsz_names_to_resize=NULL;

   if (topSel == NULL) {
      MsgBox(TgLoadCachedString(CSTID_NO_OBJ_SELECTED), TOOL_NAME, INFO_MB);
      return;
   }
   if (numObjSelected == numObjLocked) {
      Msg(TgLoadString(STID_LOCKED_OBJS_CANT_BE_FLIPPED));
      return;
   }
   sel_obbox.ltx = selNoLockObjLtX; sel_obbox.lty = selNoLockObjLtY;
   sel_obbox.rbx = selNoLockObjRbX; sel_obbox.rby = selNoLockObjRbY;
   SetPivot(CORNER_LEFT, &sel_obbox);

   saved_ltx = selLtX; saved_lty = selLtY;
   saved_rbx = selRbX; saved_rby = selRbY;
   HighLightReverse();
   ppsz_names_to_resize = NeedToProcessOnResize(&num_to_resize);
   if (ppsz_names_to_resize == NULL) {
      FlipAllSelHorizontal(saved_ltx, saved_lty, saved_rbx, saved_rby);
   } else {
      StartCompositeCmd();
      FlipAllSelHorizontal(saved_ltx, saved_lty, saved_rbx, saved_rby);
      DoOnResize(ppsz_names_to_resize, num_to_resize);
      EndCompositeCmd();
   }
   HighLightForward();
   justDupped = FALSE;
   if (numObjLocked != 0) {
      Msg(TgLoadString(STID_LOCKED_OBJS_ARE_NOT_FLIPPED));
   } else {
      Msg(TgLoadString(STID_FLIPPED_HORIZONTALLY));
   }
}

static
void FlipAllSelVertical(saved_ltx, saved_lty, saved_rbx, saved_rby)
   int saved_ltx, saved_lty, saved_rbx, saved_rby;
{
   struct SelRec *sel_ptr;

   PrepareToRecord(CMD_REPLACE, topSel, botSel, numObjSelected);
   JustRemoveAllVSel();
   for (sel_ptr = topSel; sel_ptr != NULL; sel_ptr = sel_ptr->next) {
      if (!sel_ptr->obj->locked) {
         FlipObjVertical(sel_ptr->obj);
      }
   }
   UpdSelBBox();
   RecordCmd(CMD_REPLACE, NULL, topSel, botSel, numObjSelected);
   RedrawAreas(botObj, saved_ltx-GRID_ABS_SIZE(1), saved_lty-GRID_ABS_SIZE(1),
         saved_rbx+GRID_ABS_SIZE(1), saved_rby+GRID_ABS_SIZE(1),
         selLtX-GRID_ABS_SIZE(1), selLtY-GRID_ABS_SIZE(1),
         selRbX+GRID_ABS_SIZE(1), selRbY+GRID_ABS_SIZE(1));
}

void FlipVertical()
{
   int saved_ltx, saved_lty, saved_rbx, saved_rby, num_to_resize=0;
   struct BBRec sel_obbox;
   char **ppsz_names_to_resize=NULL;

   if (topSel == NULL) {
      MsgBox(TgLoadCachedString(CSTID_NO_OBJ_SELECTED), TOOL_NAME, INFO_MB);
      return;
   }
   if (numObjSelected == numObjLocked) {
      Msg(TgLoadString(STID_LOCKED_OBJS_CANT_BE_FLIPPED));
      return;
   }
   sel_obbox.ltx = selNoLockObjLtX; sel_obbox.lty = selNoLockObjLtY;
   sel_obbox.rbx = selNoLockObjRbX; sel_obbox.rby = selNoLockObjRbY;
   SetPivot(CORNER_TOP, &sel_obbox);

   saved_ltx = selLtX; saved_lty = selLtY;
   saved_rbx = selRbX; saved_rby = selRbY;
   HighLightReverse();
   ppsz_names_to_resize = NeedToProcessOnResize(&num_to_resize);
   if (ppsz_names_to_resize == NULL) {
      FlipAllSelVertical(saved_ltx, saved_lty, saved_rbx, saved_rby);
   } else {
      StartCompositeCmd();
      FlipAllSelVertical(saved_ltx, saved_lty, saved_rbx, saved_rby);
      DoOnResize(ppsz_names_to_resize, num_to_resize);
      EndCompositeCmd();
   }
   HighLightForward();
   justDupped = FALSE;
   if (numObjLocked != 0) {
      Msg(TgLoadString(STID_LOCKED_OBJS_ARE_NOT_FLIPPED));
   } else {
      Msg(TgLoadString(STID_FLIPPED_VERTICALLY));
   }
}

/* --------------------- Rotate --------------------- */

void RotateClockWise()
{
   register struct SelRec *sel_ptr;
   int saved_ltx, saved_lty, saved_rbx, saved_rby;
   int text_obj_created, text_cursor_shown;

   if (topSel == NULL) {
      text_cursor_shown = textCursorShown;
      text_obj_created = TieLooseEnds();
      textRotation += rotationIncrement;
      while (textRotation < 0) textRotation += (360<<6);
      while (textRotation >= (360<<6)) textRotation -= (360<<6);
      ShowRotate();
      if (!text_obj_created && curChoice == DRAWTEXT && text_cursor_shown) {
         NewCurText();
         RedrawCurText();
      } else {
         textCursorShown = FALSE;
      }
      return;
   }
   if (numObjSelected == numObjLocked) {
      MsgBox(TgLoadString(STID_LOCKED_OBJS_CANT_BE_ROTATED), TOOL_NAME,
            INFO_MB);
      return;
   }
   saved_ltx = selLtX; saved_lty = selLtY;
   saved_rbx = selRbX; saved_rby = selRbY;
   HighLightReverse();
   PrepareToRecord(CMD_REPLACE, topSel, botSel, numObjSelected);
   JustRemoveAllVSel();
   SetRotatePivot();
   for (sel_ptr=topSel; sel_ptr != NULL; sel_ptr=sel_ptr->next) {
      if (OkToTransform(sel_ptr->obj, STID_DISABLE_ON_RESIZE_ROTATE)) {
         RotateObjClockWise(sel_ptr->obj);
      }
   }
   UpdSelBBox();
   RecordCmd(CMD_REPLACE, NULL, topSel, botSel, numObjSelected);
   RedrawAreas(botObj, saved_ltx-GRID_ABS_SIZE(1), saved_lty-GRID_ABS_SIZE(1),
         saved_rbx+GRID_ABS_SIZE(1), saved_rby+GRID_ABS_SIZE(1),
         selLtX-GRID_ABS_SIZE(1), selLtY-GRID_ABS_SIZE(1),
         selRbX+GRID_ABS_SIZE(1), selRbY+GRID_ABS_SIZE(1));
   HighLightForward();
   justDupped = FALSE;
   if (numObjLocked != 0) {
      Msg(TgLoadString(STID_LOCKED_OBJS_ARE_NOT_ROTATED));
   } else {
      Msg(TgLoadString(STID_ROTATED_CLOCKWISE));
   }
}

void RotateCounter()
{
   register struct SelRec *sel_ptr;
   int saved_ltx, saved_lty, saved_rbx, saved_rby;
   int text_obj_created, text_cursor_shown;

   if (topSel == NULL) {
      text_cursor_shown = textCursorShown;
      text_obj_created = TieLooseEnds();
      textRotation -= rotationIncrement;
      while (textRotation < 0) textRotation += (360<<6);
      while (textRotation >= (360<<6)) textRotation -= (360<<6);
      ShowRotate();
      if (!text_obj_created && curChoice == DRAWTEXT && text_cursor_shown) {
         NewCurText();
         RedrawCurText();
      } else {
         textCursorShown = FALSE;
      }
      return;
   }
   if (numObjSelected == numObjLocked) {
      MsgBox(TgLoadString(STID_LOCKED_OBJS_CANT_BE_ROTATED), TOOL_NAME,
            INFO_MB);
      return;
   }
   saved_ltx = selLtX; saved_lty = selLtY;
   saved_rbx = selRbX; saved_rby = selRbY;
   HighLightReverse();
   PrepareToRecord(CMD_REPLACE, topSel, botSel, numObjSelected);
   JustRemoveAllVSel();
   SetRotatePivot();
   for (sel_ptr = topSel; sel_ptr != NULL; sel_ptr = sel_ptr->next) {
      if (OkToTransform(sel_ptr->obj, STID_DISABLE_ON_RESIZE_ROTATE)) {
         RotateObjCounter(sel_ptr->obj);
      }
   }
   UpdSelBBox();
   RecordCmd(CMD_REPLACE, NULL, topSel, botSel, numObjSelected);
   RedrawAreas(botObj, saved_ltx-GRID_ABS_SIZE(1), saved_lty-GRID_ABS_SIZE(1),
         saved_rbx+GRID_ABS_SIZE(1), saved_rby+GRID_ABS_SIZE(1),
         selLtX-GRID_ABS_SIZE(1), selLtY-GRID_ABS_SIZE(1),
         selRbX+GRID_ABS_SIZE(1), selRbY+GRID_ABS_SIZE(1));
   HighLightForward();
   justDupped = FALSE;
   if (numObjLocked != 0) {
      Msg(TgLoadString(STID_LOCKED_OBJS_ARE_NOT_ROTATED));
   } else {
      Msg(TgLoadString(STID_ROTATED_COUNTER_CLOCKWISE));
   }
}

void SetTextRotation(pszBuf)
   char *pszBuf;
{
   char spec[80], buf[80];
   float fval;
   int ival;

   *spec = '\0';
   if (pszBuf != NULL) {
      strcpy(spec, pszBuf);
   } else {
      FormatAngle(textRotation, buf);
      sprintf(gszMsgBox, TgLoadString(STID_ENTER_TEXT_ROT_IN_DEGREE_CUR), buf);
      if (Dialog(gszMsgBox, NULL, spec) == INVALID) return;
   }
   UtilTrimBlanks(spec);
   if (*spec == '\0') return;

   if (sscanf(spec, "%f", &fval) != 1) {
      sprintf(gszMsgBox, TgLoadString(STID_CANT_PARSE_ENTER_ONE_NUM_VAL),
            spec);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return;
   }
   fval *= (float)64.0;
   ival = round(fval);
   if (ival < 0 || ival >= (360<<6)) {
      sprintf(gszMsgBox, TgLoadString(STID_INVALID_VAL_ENTERED_RNG_INC),
            spec, 0, 360);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return;
   }
   textRotation = ival;
   ShowRotate();
   FormatAngle(textRotation, buf);
   sprintf(gszMsgBox, TgLoadString(STID_TEXT_ROTATION_SET_TO_GIVEN), buf);
   Msg(gszMsgBox);
}

void SetRotationIncrement(pszBuf)
   char *pszBuf;
{
   char spec[80], buf[80];
   float fval;
   int ival;

   *spec = '\0';
   if (pszBuf != NULL) {
      strcpy(spec, pszBuf);
   } else {
      FormatAngle(rotationIncrement, buf);
      sprintf(gszMsgBox, TgLoadString(STID_ENTER_ROT_INC_IN_DEGREE_CUR), buf);
      if (Dialog(gszMsgBox, NULL, spec) == INVALID) return;
   }
   UtilTrimBlanks(spec);
   if (*spec == '\0') return;

   if (sscanf(spec, "%f", &fval) != 1) {
      sprintf(gszMsgBox, TgLoadString(STID_CANT_PARSE_ENTER_ONE_NUM_VAL),
            spec);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return;
   }
   fval *= (float)64.0;
   ival = round(fval);
   if (ival <= 0 || ival >= (360<<6)) {
      sprintf(gszMsgBox, TgLoadString(STID_INVALID_VAL_ENTERED_RNG_EXC),
            spec, 0, 360);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return;
   }
   rotationIncrement = ival;
   FormatAngle(rotationIncrement, buf);
   sprintf(gszMsgBox, TgLoadString(STID_TEXT_ROT_INC_SET_TO_GIVEN), buf);
   Msg(gszMsgBox);
}

static
int IdentCTM(ctm1, ctm2)
   struct XfrmMtrxRec *ctm1, *ctm2;
{
   return ((fabs(ctm1->m[CTM_SX]-ctm2->m[CTM_SX]) < EQ_TOL) &&
         (fabs(ctm1->m[CTM_SY]-ctm2->m[CTM_SY]) < EQ_TOL) &&
         (fabs(ctm1->m[CTM_SIN]-ctm2->m[CTM_SIN]) < EQ_TOL) &&
         (fabs(ctm1->m[CTM_MSIN]-ctm2->m[CTM_MSIN]) < EQ_TOL));
}

static
int SetObjCTM(ObjPtr, nTransformed, ctm)
   struct ObjRec *ObjPtr;
   int nTransformed;
   struct XfrmMtrxRec *ctm;
{
   int nReturn=FALSE, cx=0, cy=0;

   switch (ObjPtr->type) {
   case OBJ_GROUP:
   case OBJ_ICON:
   case OBJ_SYM:
   case OBJ_PIN:
      break;

   default:
      cx = ((ObjPtr->obbox.ltx+ObjPtr->obbox.rbx)>>1);
      cy = ((ObjPtr->obbox.lty+ObjPtr->obbox.rby)>>1);
      if (nTransformed) {
         struct XfrmMtrxRec *saved_ctm=NULL;

         if (ObjPtr->ctm == NULL) {
            SetCTM(ObjPtr, ctm);
            nReturn = TRUE;
         } else {
            if (!IdentCTM(ObjPtr->ctm, ctm)) {
               if (ObjPtr->type == OBJ_TEXT) {
                  saved_ctm = ObjPtr->ctm;
                  ObjPtr->ctm = NULL;
                  if (!UpdTextBBox(ObjPtr)) {
                     /* read-only text */
                     ObjPtr->ctm = saved_ctm;
                  } else {
                     free(saved_ctm);
                     SetCTM(ObjPtr, ctm);
                     UpdTextBBox(ObjPtr);
                     nReturn = TRUE;
                  }
               } else {
                  free(ObjPtr->ctm);
                  ObjPtr->ctm = NULL;
                  SetCTM(ObjPtr, ctm);
                  nReturn = TRUE;
               }
            }
         }
      } else {
         if (ObjPtr->ctm != NULL) {
            free(ObjPtr->ctm);
            ObjPtr->ctm = NULL;
            nReturn = TRUE;
         }
      }
      if (nReturn) {
         int new_cx=0, new_cy=0;

         new_cx = ((ObjPtr->obbox.ltx+ObjPtr->obbox.rbx)>>1);
         new_cy = ((ObjPtr->obbox.lty+ObjPtr->obbox.rby)>>1);
         MoveObj(ObjPtr, cx-new_cx, cy-new_cy);
         AdjObjCache(ObjPtr);
         AdjObjSplineVs(ObjPtr);
         AdjObjBBox(ObjPtr);
      }
      break;
   }
   return nReturn;
}

void SetSelCTM(nTransformed, ctm)
   int nTransformed;
   struct XfrmMtrxRec *ctm;
{
   struct SelRec *sel_ptr=NULL;
   int ltx=selLtX, lty=selLtY, rbx=selRbX, rby=selRbY, changed=FALSE;

   if (topSel == NULL) {
      MsgBox(TgLoadCachedString(CSTID_NO_OBJ_SELECTED), TOOL_NAME, INFO_MB);
      return;
   }
   if (numObjSelected == numObjLocked) {
      MsgBox(TgLoadString(STID_LOCKED_OBJS_CANT_BE_XFORMED), TOOL_NAME,
            INFO_MB);
      return;
   }
   HighLightReverse();
   StartCompositeCmd();
   for (sel_ptr=botSel; sel_ptr != NULL; sel_ptr=sel_ptr->prev) {
      if (OkToTransform(sel_ptr->obj, STID_DISABLE_ON_RESIZE_TRANSFORM)) {
         PrepareToReplaceAnObj(sel_ptr->obj);
         if (SetObjCTM(sel_ptr->obj, nTransformed, ctm)) {
            changed = TRUE;
            RecordReplaceAnObj(sel_ptr->obj);
         } else {
            AbortPrepareCmd(CMD_REPLACE);
         }
      }
   }
   EndCompositeCmd();
   if (changed) {
      SetFileModified(TRUE);
      UpdSelBBox();
      RedrawAreas(botObj, ltx-GRID_ABS_SIZE(1), lty-GRID_ABS_SIZE(1),
            rbx+GRID_ABS_SIZE(1), rby+GRID_ABS_SIZE(1),
            selLtX-GRID_ABS_SIZE(1), selLtY-GRID_ABS_SIZE(1),
            selRbX+GRID_ABS_SIZE(1), selRbY+GRID_ABS_SIZE(1));
      justDupped = FALSE;
      if (numObjLocked != 0) {
         Msg(TgLoadString(STID_LOCKED_OBJS_ARE_NOT_XFORMED));
      }
   }
   HighLightForward();
}

/* --------------------- Rotate Any Angle --------------------- */

static
void RotateBBoxByAnAngle(bbox, d_angle, vs)
   struct BBRec *bbox; /* the original bounding box */
   int d_angle; /* d_angle is degree*64 */
   XPoint *vs; /* array of 5 points */
{
   int x, y;

   RotatedXY(bbox->ltx, bbox->lty, d_angle, &x, &y);
   vs[0].x = vs[4].x = x; vs[0].y = vs[4].y = y;
   RotatedXY(bbox->rbx, bbox->lty, d_angle, &x, &y);
   vs[1].x = x; vs[1].y = y;
   RotatedXY(bbox->rbx, bbox->rby, d_angle, &x, &y);
   vs[2].x = x; vs[2].y = y;
   RotatedXY(bbox->ltx, bbox->rby, d_angle, &x, &y);
   vs[3].x = x; vs[3].y = y;
}

static
void RotateVsByAnAngle(InVs, NumPts, d_angle, OutVs)
   XPoint *InVs, *OutVs;
   int NumPts, d_angle;
{
   register int i;

   for (i=0; i < NumPts; i++) {
      int x, y;

      RotatedXY(InVs[i].x, InVs[i].y, d_angle, &x, &y);
      OutVs[i].x = x;
      OutVs[i].y = y;
   }
}

static
void RotateAllSelObjects(Corner, AngleDelta)
   int Corner, AngleDelta; /* AngleDelta is degree*64 */
{
   register struct SelRec *sel_ptr;

   for (sel_ptr = topSel; sel_ptr != NULL; sel_ptr = sel_ptr->next) {
      if (OkToTransform(sel_ptr->obj, STID_DISABLE_ON_RESIZE_ROTATE)) {
         RotateObj(sel_ptr->obj, Corner, AngleDelta, NULL, NULL);
      }
   }
   if (numObjLocked != 0) {
      Msg(TgLoadString(STID_LOCKED_OBJS_ARE_NOT_ROTATED));
   }
}

static
int ConstrainedRotateAllSel(Corner, AngleDelta, ltx, lty, rbx, rby)
   int Corner, AngleDelta, *ltx, *lty, *rbx, *rby;
{
   register struct ObjRec *obj_ptr;
   int something_stretched=FALSE, num_pts;
   int x_off, y_off, move_first, move_last, x, y;
   IntPoint *v;

   for (obj_ptr=botObj; obj_ptr != NULL; obj_ptr=obj_ptr->prev) {
      if (!obj_ptr->marked && obj_ptr->type==OBJ_POLY && !obj_ptr->locked) {
         num_pts = obj_ptr->detail.p->n;
         v = obj_ptr->detail.p->vlist;

         if (obj_ptr->ctm == NULL) {
            x_off = OFFSET_X(v[0].x); y_off = OFFSET_Y(v[0].y);
            move_first = EndPtInSelected(x_off, y_off);
            x_off = OFFSET_X(v[num_pts-1].x); y_off = OFFSET_Y(v[num_pts-1].y);
            move_last = EndPtInSelected(x_off, y_off);
         } else {
            int tmp_x, tmp_y;

            TransformPointThroughCTM(v[0].x-obj_ptr->x, v[0].y-obj_ptr->y,
                  obj_ptr->ctm, &tmp_x, &tmp_y);
            tmp_x += obj_ptr->x;
            tmp_y += obj_ptr->y;
            x_off = OFFSET_X(tmp_x); y_off = OFFSET_Y(tmp_y);
            move_first = EndPtInSelected(x_off, y_off);
            TransformPointThroughCTM(v[num_pts-1].x-obj_ptr->x,
                  v[num_pts-1].y-obj_ptr->y, obj_ptr->ctm, &tmp_x, &tmp_y);
            tmp_x += obj_ptr->x;
            tmp_y += obj_ptr->y;
            x_off = OFFSET_X(tmp_x); y_off = OFFSET_Y(tmp_y);
            move_last = EndPtInSelected(x_off, y_off);
         }
         if (move_first || move_last) {
            int index=INVALID, seg_dx, seg_dy, dx, dy, cur_seg_dx, cur_seg_dy;

            PrepareToReplaceAnObj(obj_ptr);

            if (obj_ptr->ctm != NULL) {
               /* Remove the transformations! */
               int i;

               for (i=0; i < num_pts; i++) {
                  int tmp_x, tmp_y;

                  TransformPointThroughCTM(v[i].x-obj_ptr->x, v[i].y-obj_ptr->y,
                        obj_ptr->ctm, &tmp_x, &tmp_y);
                  v[i].x = tmp_x+obj_ptr->x;
                  v[i].y = tmp_y+obj_ptr->y;
               }
               free(obj_ptr->ctm);
               obj_ptr->ctm = NULL;
               UpdPolyBBox(obj_ptr, num_pts, v);
            }
            if (something_stretched) {
               if (obj_ptr->bbox.ltx < *ltx) *ltx = obj_ptr->bbox.ltx;
               if (obj_ptr->bbox.lty < *lty) *lty = obj_ptr->bbox.lty;
               if (obj_ptr->bbox.rbx > *rbx) *rbx = obj_ptr->bbox.rbx;
               if (obj_ptr->bbox.rby > *rby) *rby = obj_ptr->bbox.rby;
            } else {
               *ltx = obj_ptr->bbox.ltx; *lty = obj_ptr->bbox.lty;
               *rbx = obj_ptr->bbox.rbx; *rby = obj_ptr->bbox.rby;
            }
            something_stretched = TRUE;
            if (move_first && move_last && num_pts==3) {
               RotatedAbsXY(v[0].x, v[0].y, AngleDelta, &x, &y);
               dx = x-v[0].x; dy = y-v[0].y;
               index = 1;
               cur_seg_dx = v[index-1].x - v[index].x;
               cur_seg_dy = v[index-1].y - v[index].y;
               seg_dx = v[index].x - v[index+1].x;
               seg_dy = v[index].y - v[index+1].y;

               if (cur_seg_dy==0 && seg_dx==0 &&
                     (seg_dy!=0 || (seg_dy==0 && dx==0))) {
                  v[index].y += dy;
               } else if (cur_seg_dx==0 && seg_dy==0 &&
                     (seg_dx!=0 || (seg_dx==0 && dy==0))) {
                  v[index].x += dx;
               }
            } else {
               if (move_first && num_pts>2) {
                  RotatedAbsXY(v[0].x, v[0].y, AngleDelta, &x, &y);
                  dx = x-v[0].x; dy = y-v[0].y;
                  index = 1;
                  cur_seg_dx = v[index-1].x - v[index].x;
                  cur_seg_dy = v[index-1].y - v[index].y;
                  seg_dx = v[index].x - v[index+1].x;
                  seg_dy = v[index].y - v[index+1].y;

                  if (cur_seg_dy==0 && cur_seg_dx!=0 &&
                        (seg_dy!=0 || (seg_dy==0 && dx==0))) {
                     v[index].y += dy;
                  } else if (cur_seg_dx==0 && cur_seg_dy!=0 &&
                        (seg_dx!=0 || (seg_dx==0 && dy==0))) {
                     v[index].x += dx;
                  }
               }
               if (move_last && num_pts>2) {
                  RotatedAbsXY(v[num_pts-1].x, v[num_pts-1].y, AngleDelta,
                        &x, &y);
                  dx = x-v[num_pts-1].x; dy = y-v[num_pts-1].y;
                  index = num_pts-2;
                  cur_seg_dx = v[index+1].x - v[index].x;
                  cur_seg_dy = v[index+1].y - v[index].y;
                  seg_dx = v[index].x - v[index-1].x;
                  seg_dy = v[index].y - v[index-1].y;

                  if (cur_seg_dy==0 && cur_seg_dx!=0 &&
                        (seg_dy!=0 || (seg_dy==0 && dx==0))) {
                     v[index].y += dy;
                  } else if (cur_seg_dx==0 && cur_seg_dy!=0 &&
                        (seg_dx!=0 || (seg_dx==0 && dy==0))) {
                     v[index].x += dx;
                  }
               }
            }
            if (move_first) {
               RotatedAbsXY(v[0].x, v[0].y, AngleDelta, &x, &y);
               v[0].x = x; v[0].y = y;
            }
            if (move_last) {
               RotatedAbsXY(v[num_pts-1].x, v[num_pts-1].y, AngleDelta,
                     &x, &y);
               v[num_pts-1].x = x; v[num_pts-1].y = y;
            }
            AdjObjSplineVs(obj_ptr);
            switch (obj_ptr->type) {
            case OBJ_POLY:
               if (obj_ptr->detail.p->curved != LT_INTSPLINE) {
                  UpdPolyBBox(obj_ptr, num_pts, v);
               } else {
                  UpdPolyBBox(obj_ptr, obj_ptr->detail.p->intn,
                        obj_ptr->detail.p->intvlist);
               }
               break;
            case OBJ_POLYGON:
               if (obj_ptr->detail.g->curved != LT_INTSPLINE) {
                  UpdPolyBBox(obj_ptr, num_pts, v);
               } else {
                  UpdPolyBBox(obj_ptr, obj_ptr->detail.g->intn,
                        obj_ptr->detail.g->intvlist);
               }
               break;
            }
            AdjObjBBox(obj_ptr);
            if (AutoCenterAttr(obj_ptr)) {
               struct AttrRec *attr_ptr=obj_ptr->fattr;
               int modified=FALSE;

               for ( ; attr_ptr != NULL; attr_ptr = attr_ptr->next) {
                  if (attr_ptr->shown) {
                     struct BBRec bbox;

                     CenterObjInOBBox(attr_ptr->obj, obj_ptr->obbox, &bbox);
                     if (bbox.ltx < *ltx) *ltx = bbox.ltx;
                     if (bbox.lty < *lty) *lty = bbox.lty;
                     if (bbox.rbx > *rbx) *rbx = bbox.rbx;
                     if (bbox.rby > *rby) *rby = bbox.rby;
                     modified = TRUE;
                  }
               }
               if (modified) AdjObjBBox(obj_ptr);
            }
            if (obj_ptr->bbox.ltx < *ltx) *ltx = obj_ptr->bbox.ltx;
            if (obj_ptr->bbox.lty < *lty) *lty = obj_ptr->bbox.lty;
            if (obj_ptr->bbox.rbx > *rbx) *rbx = obj_ptr->bbox.rbx;
            if (obj_ptr->bbox.rby > *rby) *rby = obj_ptr->bbox.rby;
            RecordReplaceAnObj(obj_ptr);
         }
      }
   }
   return something_stretched;
}

static
void RotateAllSel(Corner, AngleDelta)
   int Corner, AngleDelta; /* AngleDelta is degree*64 */
{
   int ltx=0, lty=0, rbx=0, rby=0, saved_ltx, saved_lty, saved_rbx, saved_rby;
   int poly_stretched;

   saved_ltx = selLtX; saved_lty = selLtY;
   saved_rbx = selRbX; saved_rby = selRbY;

   if (moveMode==CONST_MOVE) {
      MarkObjectsForStretch();

      StartCompositeCmd();
      PrepareToRecord(CMD_STRETCH, topSel, botSel, numObjSelected);
      RecordCmd(CMD_STRETCH, NULL, topSel, botSel, numObjSelected);

      poly_stretched = ConstrainedRotateAllSel(Corner, AngleDelta,
            &ltx, &lty, &rbx, &rby);
      RotateAllSelObjects(Corner, AngleDelta);
      UpdSelBBox();
      if (poly_stretched) {
         ltx = min(ltx,min(selLtX,saved_ltx));
         lty = min(lty,min(selLtY,saved_lty));
         rbx = max(rbx,max(selRbX,saved_rbx));
         rby = max(rby,max(selRbY,saved_rby));
         RedrawAnArea(botObj, ltx-GRID_ABS_SIZE(1), lty-GRID_ABS_SIZE(1),
               rbx+GRID_ABS_SIZE(1), rby+GRID_ABS_SIZE(1));
      } else {
         RedrawAreas(botObj, saved_ltx-GRID_ABS_SIZE(1),
               saved_lty-GRID_ABS_SIZE(1),
               saved_rbx+GRID_ABS_SIZE(1), saved_rby+GRID_ABS_SIZE(1),
               selLtX-GRID_ABS_SIZE(1), selLtY-GRID_ABS_SIZE(1),
               selRbX+GRID_ABS_SIZE(1), selRbY+GRID_ABS_SIZE(1));
      }
      EndCompositeCmd();
   } else {
      PrepareToRecord(CMD_REPLACE, topSel, botSel, numObjSelected);
      RotateAllSelObjects(Corner, AngleDelta);
      RecordCmd(CMD_REPLACE, NULL, topSel, botSel, numObjSelected);
      UpdSelBBox();
      RedrawAreas(botObj, saved_ltx-GRID_ABS_SIZE(1),
            saved_lty-GRID_ABS_SIZE(1),
            saved_rbx+GRID_ABS_SIZE(1), saved_rby+GRID_ABS_SIZE(1),
            selLtX-GRID_ABS_SIZE(1), selLtY-GRID_ABS_SIZE(1),
            selRbX+GRID_ABS_SIZE(1), selRbY+GRID_ABS_SIZE(1));
   }
}

static
void FormatAngleForRotate(buf, angle)
   char *buf;
   int angle;
{
   float fval=(float)(((float)angle)/((float)64.0));

   sprintf(buf, "degree=%.2f", fval);
}

static
void RotateSel(XGridOff, YGridOff, ObjPtr, Corner)
   int XGridOff, YGridOff, Corner;
   struct ObjRec *ObjPtr;
{
   register int i;
   XEvent ev;
   XPoint all_bbox_vs[5], obj_obbox_vs[5], *vs=NULL, *orig_vs=NULL;
   int grid_x=XGridOff, grid_y=YGridOff, dx, dy, d_angle=0;
   int saved_x=XGridOff, saved_y=YGridOff;
   int rotating=TRUE, deg360=(360<<6), n=0;
   char buf[80];
   struct BBRec orig_all_bbox, orig_obj_obbox;

   if (numObjSelected == numObjLocked || ObjPtr->locked) {
      MsgBox(TgLoadString(STID_LOCKED_OBJS_CANT_BE_ROTATED), TOOL_NAME,
            INFO_MB);
      return;
   }
   XFlush(mainDisplay);
   XSync(mainDisplay, False);
   if (XCheckMaskEvent(mainDisplay, ExposureMask, &ev) ||
         XCheckMaskEvent(mainDisplay, VisibilityChangeMask, &ev)) {
      ExposeEventHandler(&ev, TRUE);
   }
   if (ObjPtr->type==OBJ_POLY || ObjPtr->type==OBJ_POLYGON) {
      IntPoint *pv;
      int px, py;

      if (ObjPtr->ctm == NULL) {
         pv = (ObjPtr->type==OBJ_POLY ? &ObjPtr->detail.p->vlist[Corner] :
               &ObjPtr->detail.g->vlist[Corner]);
         px = pv->x;
         py = pv->y;
      } else {
         pv = (ObjPtr->type==OBJ_POLY ? &ObjPtr->detail.p->vlist[Corner] :
               &ObjPtr->detail.g->vlist[Corner]);
         TransformPointThroughCTM(pv->x-ObjPtr->x, pv->y-ObjPtr->y,
               ObjPtr->ctm, &px, &py);
         px += ObjPtr->x;
         py += ObjPtr->y;
      }
      absPivotX = (ObjPtr->obbox.ltx+ObjPtr->obbox.rbx)>>1;
      absPivotY = (ObjPtr->obbox.lty+ObjPtr->obbox.rby)>>1;
      moveX = OFFSET_X(px);
      moveY = OFFSET_Y(py);
      changeX = changeY = TRUE;
      multX = multY = 1.0;
      pivotX = OFFSET_X(absPivotX);
      pivotY = OFFSET_Y(absPivotY);
   } else {
      SetPivot(Corner, &ObjPtr->obbox);
   }
   if (!autoRotatePivot && rotatePivotAbsXYValid) {
      absPivotX = rotatePivotAbsX;
      absPivotY = rotatePivotAbsY;
      pivotX = OFFSET_X(absPivotX);
      pivotY = OFFSET_Y(absPivotY);
   }
   SetBBRec(&orig_all_bbox, OFFSET_X(selNoLockLtX)-2, OFFSET_Y(selNoLockLtY)-2,
         OFFSET_X(selNoLockRbX)+2, OFFSET_Y(selNoLockRbY)+2);
   SetRotateVs(all_bbox_vs, orig_all_bbox.ltx, orig_all_bbox.lty,
         orig_all_bbox.rbx, orig_all_bbox.rby);
   XDrawLines(mainDisplay, drawWindow, revDefaultGC, all_bbox_vs, 5,
         CoordModeOrigin);

   if (ObjPtr->type==OBJ_POLY || ObjPtr->type==OBJ_POLYGON) {
      if (ObjPtr->type == OBJ_POLY) {
         if (ObjPtr->ctm == NULL) {
            n = ObjPtr->detail.p->sn;
            orig_vs = (XPoint*)malloc(n*sizeof(XPoint));
            vs = (XPoint*)malloc(n*sizeof(XPoint));
            if (orig_vs == NULL || vs == NULL) FailAllocMessage();
            for (i=0; i < n; i++) {
               vs[i].x = orig_vs[i].x = ObjPtr->detail.p->svlist[i].x;
               vs[i].y = orig_vs[i].y = ObjPtr->detail.p->svlist[i].y;
            }
         } else {
            n = ObjPtr->detail.p->rotated_n;
            orig_vs = (XPoint*)malloc(n*sizeof(XPoint));
            vs = (XPoint*)malloc(n*sizeof(XPoint));
            if (orig_vs == NULL || vs == NULL) FailAllocMessage();
            for (i=0; i < n; i++) {
               vs[i].x = orig_vs[i].x = ObjPtr->detail.p->rotated_vlist[i].x;
               vs[i].y = orig_vs[i].y = ObjPtr->detail.p->rotated_vlist[i].y;
            }
         }
      } else {
         if (ObjPtr->ctm == NULL) {
            n = ObjPtr->detail.g->sn;
            orig_vs = (XPoint*)malloc(n*sizeof(XPoint));
            vs = (XPoint*)malloc(n*sizeof(XPoint));
            if (orig_vs == NULL || vs == NULL) FailAllocMessage();
            for (i=0; i < n; i++) {
               vs[i].x = orig_vs[i].x = ObjPtr->detail.g->svlist[i].x;
               vs[i].y = orig_vs[i].y = ObjPtr->detail.g->svlist[i].y;
            }
         } else {
            n = ObjPtr->detail.g->rotated_n;
            orig_vs = (XPoint*)malloc(n*sizeof(XPoint));
            vs = (XPoint*)malloc(n*sizeof(XPoint));
            if (orig_vs == NULL || vs == NULL) FailAllocMessage();
            for (i=0; i < n; i++) {
               vs[i].x = orig_vs[i].x = ObjPtr->detail.g->rotated_vlist[i].x;
               vs[i].y = orig_vs[i].y = ObjPtr->detail.g->rotated_vlist[i].y;
            }
         }
      }
      XDrawLines(mainDisplay, drawWindow, revDefaultGC, vs, n,
            CoordModeOrigin);
   } else {
      if (ObjPtr->ctm == NULL) {
         SetBBRec(&orig_obj_obbox, OFFSET_X(ObjPtr->obbox.ltx),
               OFFSET_Y(ObjPtr->obbox.lty), OFFSET_X(ObjPtr->obbox.rbx),
               OFFSET_Y(ObjPtr->obbox.rby));
         SetRotateVs(obj_obbox_vs, orig_obj_obbox.ltx, orig_obj_obbox.lty,
               orig_obj_obbox.rbx, orig_obj_obbox.rby);
         XDrawLines(mainDisplay, drawWindow, revDefaultGC, obj_obbox_vs, 5,
               CoordModeOrigin);
      } else {
         memcpy(obj_obbox_vs, ObjPtr->rotated_obbox, 5*sizeof(XPoint));
         XDrawLines(mainDisplay, drawWindow, revDefaultGC, obj_obbox_vs, 5,
               CoordModeOrigin);
      }
   }
   dx = OFFSET_X(ObjPtr->obbox.rbx) - OFFSET_X(ObjPtr->obbox.ltx);
   dy = OFFSET_Y(ObjPtr->obbox.rby) - OFFSET_Y(ObjPtr->obbox.lty);
   if (dx == 0 && dy == 0) {
      MsgBox(TgLoadString(STID_SEL_OBJ_TOO_SMALL_ROT_ANOTHER), TOOL_NAME,
            INFO_MB);
      return;
   }
   grid_x = moveX;
   grid_y = moveY;
   FormatAngleForRotate(buf, 0);
   StartShowMeasureCursor(grid_x, grid_y, buf, TRUE);
   if (!debugNoPointerGrab) {
      XGrabPointer(mainDisplay, drawWindow, False,
            PointerMotionMask | ButtonReleaseMask,
            GrabModeAsync, GrabModeAsync, None, rotatingCursor, CurrentTime);
   }
   while (rotating) {
      XEvent input;

      XNextEvent(mainDisplay, &input);

      if (input.type == Expose || input.type == VisibilityNotify) {
         ExposeEventHandler(&input, TRUE);
      } else if (input.type == ButtonRelease) {
         XUngrabPointer(mainDisplay, CurrentTime);
         XSync(mainDisplay, False);
         rotating = FALSE;
         FormatAngleForRotate(buf, d_angle);
         EndShowMeasureCursor(grid_x, grid_y, buf, TRUE);
         if (ObjPtr->type==OBJ_POLY || ObjPtr->type==OBJ_POLYGON) {
            XDrawLines(mainDisplay, drawWindow, revDefaultGC, vs, n,
                  CoordModeOrigin);
         } else {
            XDrawLines(mainDisplay, drawWindow, revDefaultGC, obj_obbox_vs, 5,
                  CoordModeOrigin);
         }
         XDrawLines(mainDisplay, drawWindow, revDefaultGC, all_bbox_vs, 5,
               CoordModeOrigin);
      } else if (input.type == MotionNotify || input.type == KeyPress ||
            input.type == KeyRelease) {
         int end_x=0, end_y=0;

         FormatAngleForRotate(buf, d_angle);
         ShowMeasureCursor(grid_x, grid_y, buf, TRUE);
         if (ObjPtr->type==OBJ_POLY || ObjPtr->type==OBJ_POLYGON) {
            XDrawLines(mainDisplay, drawWindow, revDefaultGC, vs, n,
                  CoordModeOrigin);
         } else {
            XDrawLines(mainDisplay, drawWindow, revDefaultGC, obj_obbox_vs, 5,
                  CoordModeOrigin);
         }
         XDrawLines(mainDisplay, drawWindow, revDefaultGC, all_bbox_vs, 5,
               CoordModeOrigin);

         if (input.type == KeyPress || input.type == KeyRelease) {
            end_x = grid_x;
            end_y = grid_y;
         } else {
            end_x = input.xmotion.x;
            end_y = input.xmotion.y;
         }
         GridXY(end_x, end_y, &grid_x, &grid_y);
         dx = grid_x - saved_x;
         dy = grid_y - saved_y;
         grid_x = moveX + dx;
         grid_y = moveY + dy;
         MarkRulers(grid_x, grid_y);
         PointsToArc(pivotX, pivotY, moveX, moveY, grid_x, grid_y,
               ARC_CW, FALSE, NULL, NULL, NULL, NULL, NULL, &d_angle);
         if (shiftForDiagMouseMove && DiagEventCheck(&input)) {
            int tmp_angle=(int)(((double)d_angle) / ((double)(45<<5)));

            if (tmp_angle & 0x1) {
               if (tmp_angle > 0) {
                  tmp_angle = ((tmp_angle+1)>>1);
               } else {
                  tmp_angle = ((tmp_angle-1)>>1);
               }
            } else {
               tmp_angle >>= 1;
            }
            d_angle = tmp_angle * (45<<6);
         }
         if (d_angle == deg360) d_angle = 0;
         d_angle = (-d_angle);
         while (d_angle >= (deg360)) d_angle -= (deg360);

         RotateBBoxByAnAngle(&orig_all_bbox, d_angle, all_bbox_vs);
         XDrawLines(mainDisplay, drawWindow, revDefaultGC, all_bbox_vs, 5,
               CoordModeOrigin);
         if (ObjPtr->type==OBJ_POLY || ObjPtr->type==OBJ_POLYGON) {
            RotateVsByAnAngle(orig_vs, n, d_angle, vs);
            XDrawLines(mainDisplay, drawWindow, revDefaultGC, vs, n,
                  CoordModeOrigin);
         } else {
            if (ObjPtr->ctm == NULL) {
               RotateBBoxByAnAngle(&orig_obj_obbox, d_angle, obj_obbox_vs);
            } else {
               RotateVsByAnAngle(ObjPtr->rotated_obbox, 5, d_angle,
                     obj_obbox_vs);
            }
            XDrawLines(mainDisplay, drawWindow, revDefaultGC, obj_obbox_vs, 5,
                 CoordModeOrigin);
         }
         FormatAngleForRotate(buf, d_angle);
         ShowMeasureCursor(grid_x, grid_y, buf, TRUE);
         while (XCheckMaskEvent(mainDisplay, PointerMotionMask, &ev)) ;
      }
   }
   if (d_angle != 0) {
      HighLightReverse();
      RotateAllSel(Corner, d_angle);
      HighLightForward();
      SetFileModified(TRUE);
      justDupped = FALSE;
   }
   if (vs != NULL) free(vs);
   if (orig_vs != NULL) free(orig_vs);
}

/* --------------------- Shearing --------------------- */

static
void ShearBBox(Corner, bbox, dx_shear, dy_shear, dx_scale, dy_scale, vs)
   int Corner;
   double dx_shear, dy_shear, dx_scale, dy_scale;
   struct BBRec *bbox; /* the original bounding box */
   XPoint *vs; /* array of 5 points */
{
   int x, y;

   switch (Corner) {
   case CORNER_TOP:
   case CORNER_BOTTOM:
      if (bbox->lty == pivotY) {
         vs[0].x = vs[4].x = bbox->ltx;
         vs[0].y = vs[4].y = bbox->lty;
         vs[1].x = bbox->rbx; vs[1].y = bbox->lty;
      } else {
         ShearedXY(Corner, bbox->ltx, bbox->lty, dx_shear, dy_shear,
               dx_scale, dy_scale, &x, &y);
         vs[0].x = vs[4].x = x; vs[0].y = vs[4].y = y;
         ShearedXY(Corner, bbox->rbx, bbox->lty, dx_shear, dy_shear,
               dx_scale, dy_scale, &x, &y);
         vs[1].x = x; vs[1].y = y;
      }
      if (bbox->rby == pivotY) {
         vs[2].x = bbox->rbx; vs[2].y = bbox->rby;
         vs[3].x = bbox->ltx; vs[3].y = bbox->rby;
      } else {
         ShearedXY(Corner, bbox->rbx, bbox->rby, dx_shear, dy_shear,
               dx_scale, dy_scale, &x, &y);
         vs[2].x = x; vs[2].y = y;
         ShearedXY(Corner, bbox->ltx, bbox->rby, dx_shear, dy_shear,
               dx_scale, dy_scale, &x, &y);
         vs[3].x = x; vs[3].y = y;
      }
      break;
   case CORNER_RIGHT:
   case CORNER_LEFT:
      if (bbox->ltx == pivotX) {
         vs[0].x = vs[4].x = bbox->ltx;
         vs[0].y = vs[4].y = bbox->lty;
         vs[3].x = bbox->ltx; vs[3].y = bbox->rby;
      } else {
         ShearedXY(Corner, bbox->ltx, bbox->lty, dx_shear, dy_shear,
               dx_scale, dy_scale, &x, &y);
         vs[0].x = vs[4].x = x; vs[0].y = vs[4].y = y;
         ShearedXY(Corner, bbox->ltx, bbox->rby, dx_shear, dy_shear,
               dx_scale, dy_scale, &x, &y);
         vs[3].x = x; vs[3].y = y;
      }
      if (bbox->rbx == pivotX) {
         vs[1].x = bbox->rbx; vs[1].y = bbox->lty;
         vs[2].x = bbox->rbx; vs[2].y = bbox->rby;
      } else {
         ShearedXY(Corner, bbox->rbx, bbox->lty, dx_shear, dy_shear,
               dx_scale, dy_scale, &x, &y);
         vs[1].x = x; vs[1].y = y;
         ShearedXY(Corner, bbox->rbx, bbox->rby, dx_shear, dy_shear,
               dx_scale, dy_scale, &x, &y);
         vs[2].x = x; vs[2].y = y;
      }
      break;
   }
}

static
void ShearVs(Corner, InVs, NumPts, dx_shear, dy_shear, dx_scale, dy_scale,
      OutVs)
   int Corner, NumPts;
   double dx_shear, dy_shear, dx_scale, dy_scale;
   XPoint *InVs, *OutVs; /* array of 5 points */
{
   register int i;
   int x, y;

   switch (Corner) {
   case CORNER_TOP:
   case CORNER_BOTTOM:
      for (i=0; i < NumPts; i++) {
         if (InVs[i].y == pivotY) {
            OutVs[i].x = InVs[i].x;
            OutVs[i].y = InVs[i].y;
         } else {
            ShearedXY(Corner, InVs[i].x, InVs[i].y, dx_shear, dy_shear,
                  dx_scale, dy_scale, &x, &y);
            OutVs[i].x = x;
            OutVs[i].y = y;
         }
      }
      break;
   case CORNER_RIGHT:
   case CORNER_LEFT:
      for (i=0; i < NumPts; i++) {
         if (InVs[i].x == pivotX) {
            OutVs[i].x = InVs[i].x;
            OutVs[i].y = InVs[i].y;
         } else {
            ShearedXY(Corner, InVs[i].x, InVs[i].y, dx_shear, dy_shear,
                  dx_scale, dy_scale, &x, &y);
            OutVs[i].x = x;
            OutVs[i].y = y;
         }
      }
      break;
   }
}

static
void ShearAllSelObjects(Corner, dxShear, dyShear, dxScale, dyScale)
   int Corner;
   double dxShear, dyShear, dxScale, dyScale; /* everything scaled by 1000 */
{
   register struct SelRec *sel_ptr;

   for (sel_ptr = topSel; sel_ptr != NULL; sel_ptr = sel_ptr->next) {
      if (OkToTransform(sel_ptr->obj, STID_DISABLE_ON_RESIZE_SHEAR)) {
         ShearObj(sel_ptr->obj, Corner, dxShear, dyShear, dxScale, dyScale,
               NULL, NULL);
      }
   }
   if (numObjLocked != 0) {
      Msg(TgLoadString(STID_LOCKED_OBJS_ARE_NOT_SHEARED));
   }
}

static
int ConstrainedShearAllSel(Corner, dxShear, dyShear, dxScale, dyScale,
      ltx, lty, rbx, rby)
   int Corner, *ltx, *lty, *rbx, *rby;
   double dxShear, dyShear, dxScale, dyScale;
{
   register struct ObjRec *obj_ptr;
   int something_stretched=FALSE, num_pts;
   int x_off, y_off, move_first, move_last, x, y;
   IntPoint *v;

   for (obj_ptr=botObj; obj_ptr != NULL; obj_ptr=obj_ptr->prev) {
      if (!obj_ptr->marked && obj_ptr->type==OBJ_POLY && !obj_ptr->locked) {
         num_pts = obj_ptr->detail.p->n;
         v = obj_ptr->detail.p->vlist;

         if (obj_ptr->ctm == NULL) {
            x_off = OFFSET_X(v[0].x); y_off = OFFSET_Y(v[0].y);
            move_first = EndPtInSelected(x_off, y_off);
            x_off = OFFSET_X(v[num_pts-1].x); y_off = OFFSET_Y(v[num_pts-1].y);
            move_last = EndPtInSelected(x_off, y_off);
         } else {
            int tmp_x, tmp_y;

            TransformPointThroughCTM(v[0].x-obj_ptr->x, v[0].y-obj_ptr->y,
                  obj_ptr->ctm, &tmp_x, &tmp_y);
            tmp_x += obj_ptr->x;
            tmp_y += obj_ptr->y;
            x_off = OFFSET_X(tmp_x); y_off = OFFSET_Y(tmp_y);
            move_first = EndPtInSelected(x_off, y_off);
            TransformPointThroughCTM(v[num_pts-1].x-obj_ptr->x,
                  v[num_pts-1].y-obj_ptr->y, obj_ptr->ctm, &tmp_x, &tmp_y);
            tmp_x += obj_ptr->x;
            tmp_y += obj_ptr->y;
            x_off = OFFSET_X(tmp_x); y_off = OFFSET_Y(tmp_y);
            move_last = EndPtInSelected(x_off, y_off);
         }

         if (move_first || move_last) {
            int index=INVALID, seg_dx, seg_dy, dx, dy, cur_seg_dx, cur_seg_dy;

            PrepareToReplaceAnObj(obj_ptr);

            if (obj_ptr->ctm != NULL) {
               /* Remove the transformations! */
               int i;

               for (i=0; i < num_pts; i++) {
                  int tmp_x, tmp_y;

                  TransformPointThroughCTM(v[i].x-obj_ptr->x, v[i].y-obj_ptr->y,
                        obj_ptr->ctm, &tmp_x, &tmp_y);
                  v[i].x = tmp_x+obj_ptr->x;
                  v[i].y = tmp_y+obj_ptr->y;
               }
               free(obj_ptr->ctm);
               obj_ptr->ctm = NULL;
               UpdPolyBBox(obj_ptr, num_pts, v);
            }
            if (something_stretched) {
               if (obj_ptr->bbox.ltx < *ltx) *ltx = obj_ptr->bbox.ltx;
               if (obj_ptr->bbox.lty < *lty) *lty = obj_ptr->bbox.lty;
               if (obj_ptr->bbox.rbx > *rbx) *rbx = obj_ptr->bbox.rbx;
               if (obj_ptr->bbox.rby > *rby) *rby = obj_ptr->bbox.rby;
            } else {
               *ltx = obj_ptr->bbox.ltx; *lty = obj_ptr->bbox.lty;
               *rbx = obj_ptr->bbox.rbx; *rby = obj_ptr->bbox.rby;
            }
            something_stretched = TRUE;
            if (move_first && move_last && num_pts==3) {
               ShearedAbsXY(Corner, v[0].x, v[0].y, dxShear, dyShear,
                     dxScale, dyScale, &x, &y);
               dx = x-v[0].x; dy = y-v[0].y;
               index = 1;
               cur_seg_dx = v[index-1].x - v[index].x;
               cur_seg_dy = v[index-1].y - v[index].y;
               seg_dx = v[index].x - v[index+1].x;
               seg_dy = v[index].y - v[index+1].y;

               if (cur_seg_dy==0 && seg_dx==0 &&
                     (seg_dy!=0 || (seg_dy==0 && dx==0))) {
                  v[index].y += dy;
               } else if (cur_seg_dx==0 && seg_dy==0 &&
                     (seg_dx!=0 || (seg_dx==0 && dy==0))) {
                  v[index].x += dx;
               }
            } else {
               if (move_first && num_pts>2) {
                  ShearedAbsXY(Corner, v[0].x, v[0].y, dxShear, dyShear,
                        dxScale, dyScale, &x, &y);
                  dx = x-v[0].x; dy = y-v[0].y;
                  index = 1;
                  cur_seg_dx = v[index-1].x - v[index].x;
                  cur_seg_dy = v[index-1].y - v[index].y;
                  seg_dx = v[index].x - v[index+1].x;
                  seg_dy = v[index].y - v[index+1].y;

                  if (cur_seg_dy==0 && cur_seg_dx!=0 &&
                        (seg_dy!=0 || (seg_dy==0 && dx==0))) {
                     v[index].y += dy;
                  } else if (cur_seg_dx==0 && cur_seg_dy!=0 &&
                        (seg_dx!=0 || (seg_dx==0 && dy==0))) {
                     v[index].x += dx;
                  }
               }
               if (move_last && num_pts>2) {
                  ShearedAbsXY(Corner, v[num_pts-1].x, v[num_pts-1].y,
                        dxShear, dyShear, dxScale, dyScale, &x, &y);
                  dx = x-v[num_pts-1].x; dy = y-v[num_pts-1].y;
                  index = num_pts-2;
                  cur_seg_dx = v[index+1].x - v[index].x;
                  cur_seg_dy = v[index+1].y - v[index].y;
                  seg_dx = v[index].x - v[index-1].x;
                  seg_dy = v[index].y - v[index-1].y;

                  if (cur_seg_dy==0 && cur_seg_dx!=0 &&
                        (seg_dy!=0 || (seg_dy==0 && dx==0))) {
                     v[index].y += dy;
                  } else if (cur_seg_dx==0 && cur_seg_dy!=0 &&
                        (seg_dx!=0 || (seg_dx==0 && dy==0))) {
                     v[index].x += dx;
                  }
               }
            }
            if (move_first) {
               ShearedAbsXY(Corner, v[0].x, v[0].y, dxShear, dyShear,
                     dxScale, dyScale, &x, &y);
               v[0].x = x; v[0].y = y;
            }
            if (move_last) {
               ShearedAbsXY(Corner, v[num_pts-1].x, v[num_pts-1].y,
                     dxShear, dyShear, dxScale, dyScale, &x, &y);
               v[num_pts-1].x = x; v[num_pts-1].y = y;
            }
            AdjObjSplineVs(obj_ptr);
            switch (obj_ptr->type) {
            case OBJ_POLY:
               if (obj_ptr->detail.p->curved != LT_INTSPLINE) {
                  UpdPolyBBox(obj_ptr, num_pts, v);
               } else {
                  UpdPolyBBox(obj_ptr, obj_ptr->detail.p->intn,
                        obj_ptr->detail.p->intvlist);
               }
               break;
            case OBJ_POLYGON:
               if (obj_ptr->detail.g->curved != LT_INTSPLINE) {
                  UpdPolyBBox(obj_ptr, num_pts, v);
               } else {
                  UpdPolyBBox(obj_ptr, obj_ptr->detail.g->intn,
                        obj_ptr->detail.g->intvlist);
               }
               break;
            }
            AdjObjBBox(obj_ptr);
            if (AutoCenterAttr(obj_ptr)) {
               struct AttrRec *attr_ptr=obj_ptr->fattr;
               int modified=FALSE;

               for ( ; attr_ptr != NULL; attr_ptr = attr_ptr->next) {
                  if (attr_ptr->shown) {
                     struct BBRec bbox;

                     CenterObjInOBBox(attr_ptr->obj, obj_ptr->obbox, &bbox);
                     if (bbox.ltx < *ltx) *ltx = bbox.ltx;
                     if (bbox.lty < *lty) *lty = bbox.lty;
                     if (bbox.rbx > *rbx) *rbx = bbox.rbx;
                     if (bbox.rby > *rby) *rby = bbox.rby;
                     modified = TRUE;
                  }
               }
               if (modified) AdjObjBBox(obj_ptr);
            }
            if (obj_ptr->bbox.ltx < *ltx) *ltx = obj_ptr->bbox.ltx;
            if (obj_ptr->bbox.lty < *lty) *lty = obj_ptr->bbox.lty;
            if (obj_ptr->bbox.rbx > *rbx) *rbx = obj_ptr->bbox.rbx;
            if (obj_ptr->bbox.rby > *rby) *rby = obj_ptr->bbox.rby;
            RecordReplaceAnObj(obj_ptr);
         }
      }
   }
   return something_stretched;
}

static
void ShearAllSel(Corner, dxShear, dyShear, dxScale, dyScale)
   int Corner;
   double dxShear, dyShear, dxScale, dyScale; /* everything scaled by 1000 */
{
   int ltx=0, lty=0, rbx=0, rby=0, saved_ltx, saved_lty, saved_rbx, saved_rby;
   int poly_stretched;

   saved_ltx = selLtX; saved_lty = selLtY;
   saved_rbx = selRbX; saved_rby = selRbY;

   if (moveMode==CONST_MOVE) {
      MarkObjectsForStretch();

      StartCompositeCmd();
      PrepareToRecord(CMD_STRETCH, topSel, botSel, numObjSelected);
      RecordCmd(CMD_STRETCH, NULL, topSel, botSel, numObjSelected);

      poly_stretched = ConstrainedShearAllSel(Corner, dxShear, dyShear, dxScale,
            dyScale, &ltx, &lty, &rbx, &rby);
      ShearAllSelObjects(Corner, dxShear, dyShear, dxScale, dyScale);
      UpdSelBBox();
      if (poly_stretched) {
         ltx = min(ltx,min(selLtX,saved_ltx));
         lty = min(lty,min(selLtY,saved_lty));
         rbx = max(rbx,max(selRbX,saved_rbx));
         rby = max(rby,max(selRbY,saved_rby));
         RedrawAnArea(botObj, ltx-GRID_ABS_SIZE(1), lty-GRID_ABS_SIZE(1),
               rbx+GRID_ABS_SIZE(1), rby+GRID_ABS_SIZE(1));
      } else {
         RedrawAreas(botObj, saved_ltx-GRID_ABS_SIZE(1),
               saved_lty-GRID_ABS_SIZE(1),
               saved_rbx+GRID_ABS_SIZE(1), saved_rby+GRID_ABS_SIZE(1),
               selLtX-GRID_ABS_SIZE(1), selLtY-GRID_ABS_SIZE(1),
               selRbX+GRID_ABS_SIZE(1), selRbY+GRID_ABS_SIZE(1));
      }
      EndCompositeCmd();
   } else {
      PrepareToRecord(CMD_REPLACE, topSel, botSel, numObjSelected);
      ShearAllSelObjects(Corner, dxShear, dyShear, dxScale, dyScale);
      RecordCmd(CMD_REPLACE, NULL, topSel, botSel, numObjSelected);
      UpdSelBBox();
      RedrawAreas(botObj, saved_ltx-GRID_ABS_SIZE(1),
            saved_lty-GRID_ABS_SIZE(1),
            saved_rbx+GRID_ABS_SIZE(1), saved_rby+GRID_ABS_SIZE(1),
            selLtX-GRID_ABS_SIZE(1), selLtY-GRID_ABS_SIZE(1),
            selRbX+GRID_ABS_SIZE(1), selRbY+GRID_ABS_SIZE(1));
   }
}

static
void ShearSel(XGridOff, YGridOff, ObjPtr, Corner)
   int XGridOff, YGridOff, Corner;
   struct ObjRec *ObjPtr;
{
   XEvent ev;
   XPoint all_bbox_vs[5], obj_obbox_vs[5];
   int grid_x=XGridOff, grid_y=YGridOff, dx, dy;
   int saved_x=XGridOff, saved_y=YGridOff;
   int shearing=TRUE, shear_hori=FALSE;
   double dx_scale=(double)1000, dy_scale=(double)1000;
   double dx_shear=(double)0, dy_shear=(double)0;
   char buf[80];
   struct BBRec orig_all_bbox, orig_obj_obbox;

   if (numObjSelected == numObjLocked || ObjPtr->locked) {
      MsgBox(TgLoadString(STID_LOCKED_OBJS_CANT_BE_ROTATED), TOOL_NAME,
            INFO_MB);
      return;
   }
   XFlush(mainDisplay);
   XSync(mainDisplay, False);
   if (XCheckMaskEvent(mainDisplay, ExposureMask, &ev) ||
         XCheckMaskEvent(mainDisplay, VisibilityChangeMask, &ev)) {
      ExposeEventHandler(&ev, TRUE);
   }
   SetPivot(Corner, &ObjPtr->obbox);

   SetBBRec(&orig_all_bbox, OFFSET_X(selNoLockLtX)-2, OFFSET_Y(selNoLockLtY)-2,
         OFFSET_X(selNoLockRbX)+2, OFFSET_Y(selNoLockRbY)+2);
   SetRotateVs(all_bbox_vs, orig_all_bbox.ltx, orig_all_bbox.lty,
         orig_all_bbox.rbx, orig_all_bbox.rby);
   XDrawLines(mainDisplay, drawWindow, revDefaultGC, all_bbox_vs, 5,
         CoordModeOrigin);

   if (ObjPtr->ctm == NULL) {
      SetBBRec(&orig_obj_obbox, OFFSET_X(ObjPtr->obbox.ltx),
            OFFSET_Y(ObjPtr->obbox.lty), OFFSET_X(ObjPtr->obbox.rbx),
            OFFSET_Y(ObjPtr->obbox.rby));
      SetRotateVs(obj_obbox_vs, orig_obj_obbox.ltx, orig_obj_obbox.lty,
            orig_obj_obbox.rbx, orig_obj_obbox.rby);
      XDrawLines(mainDisplay, drawWindow, revDefaultGC, obj_obbox_vs, 5,
            CoordModeOrigin);
   } else {
      memcpy(obj_obbox_vs, ObjPtr->rotated_obbox, 5*sizeof(XPoint));
      XDrawLines(mainDisplay, drawWindow, revDefaultGC, obj_obbox_vs, 5,
            CoordModeOrigin);
   }

   dx = OFFSET_X(ObjPtr->obbox.rbx) - OFFSET_X(ObjPtr->obbox.ltx);
   dy = OFFSET_Y(ObjPtr->obbox.rby) - OFFSET_Y(ObjPtr->obbox.lty);
   if (dx == 0 || dy == 0) {
      MsgBox(TgLoadString(STID_SEL_OBJ_TOO_SMALL_SHEAR_ANO), TOOL_NAME,
            INFO_MB);
      return;
   }
   if (Corner == CORNER_TOP || Corner == CORNER_BOTTOM) {
      shear_hori = TRUE;
      multX = 0.0;
      multY = (Corner == CORNER_BOTTOM ? 1.0 : (-1.0));
   } else {
      shear_hori = FALSE;
      multX = (Corner == CORNER_RIGHT ? 1.0 : (-1.0));
      multY = 0.0;
   }
   dx = dy = 0;
   grid_x = moveX;
   grid_y = moveY;
   sprintf(buf, "dx=0\ndy=0");
   StartShowMeasureCursor(grid_x, grid_y, buf, TRUE);
   if (!debugNoPointerGrab) {
      XGrabPointer(mainDisplay, drawWindow, False,
            PointerMotionMask | ButtonReleaseMask,
            GrabModeAsync, GrabModeAsync, None,
            ((Corner==CORNER_TOP || Corner==CORNER_BOTTOM) ? horiShearCursor :
            vertShearCursor), CurrentTime);
   }
   while (shearing) {
      XEvent input;

      XNextEvent(mainDisplay, &input);

      if (input.type == Expose || input.type == VisibilityNotify) {
         ExposeEventHandler(&input, TRUE);
      } else if (input.type == ButtonRelease) {
         XUngrabPointer(mainDisplay, CurrentTime);
         XSync(mainDisplay, False);
         shearing = FALSE;
         sprintf(buf, "dx=%1d\ndy=%1d", dx, dy);
         EndShowMeasureCursor(grid_x, grid_y, buf, TRUE);
         XDrawLines(mainDisplay, drawWindow, revDefaultGC, obj_obbox_vs, 5,
               CoordModeOrigin);
         XDrawLines(mainDisplay, drawWindow, revDefaultGC, all_bbox_vs, 5,
               CoordModeOrigin);
      } else if (input.type == MotionNotify) {
         sprintf(buf, "dx=%1d\ndy=%1d", dx, dy);
         ShowMeasureCursor(grid_x, grid_y, buf, TRUE);
         XDrawLines(mainDisplay, drawWindow, revDefaultGC, obj_obbox_vs, 5,
               CoordModeOrigin);
         XDrawLines(mainDisplay, drawWindow, revDefaultGC, all_bbox_vs, 5,
               CoordModeOrigin);

         GridXY(input.xmotion.x, input.xmotion.y, &grid_x, &grid_y);
         dx = grid_x - saved_x;
         dy = grid_y - saved_y;
         grid_x = moveX + dx;
         grid_y = moveY + dy;
         MarkRulers(grid_x, grid_y);
         PointsToShearScale(Corner, pivotX, pivotY, moveX, moveY,
               grid_x, grid_y, &dx_shear, &dy_shear, &dx_scale, &dy_scale);

         ShearBBox(Corner, &orig_all_bbox, dx_shear, dy_shear,
               dx_scale, dy_scale, all_bbox_vs);
         XDrawLines(mainDisplay, drawWindow, revDefaultGC, all_bbox_vs, 5,
               CoordModeOrigin);
         if (ObjPtr->ctm == NULL) {
            ShearBBox(Corner, &orig_obj_obbox, dx_shear, dy_shear, dx_scale,
                  dy_scale, obj_obbox_vs);
         } else {
            ShearVs(Corner, ObjPtr->rotated_obbox, 5, dx_shear, dy_shear,
                  dx_scale, dy_scale, obj_obbox_vs);
         }
         XDrawLines(mainDisplay, drawWindow, revDefaultGC, obj_obbox_vs, 5,
               CoordModeOrigin);
         sprintf(buf, "dx=%1d\ndy=%1d", dx, dy);
         ShowMeasureCursor(grid_x, grid_y, buf, TRUE);
         while (XCheckMaskEvent(mainDisplay, PointerMotionMask, &ev)) ;
      }
   }
   ShowCursor();
   if (dx != 0 || dy != 0) {
      PointsToShearScale(Corner, pivotX, pivotY, moveX, moveY,
            moveX+dx, moveY+dy, &dx_shear, &dy_shear, &dx_scale, &dy_scale);
      HighLightReverse();
      ShearAllSel(Corner, dx_shear, dy_shear, dx_scale, dy_scale);
      HighLightForward();
      SetFileModified(TRUE);
      justDupped = FALSE;
   }
}

void RotateShearSel(XGridOff, YGridOff, ObjPtr, Corner)
   int XGridOff, YGridOff, Corner;
   struct ObjRec *ObjPtr;
   /* 1 2 3 */
   /* 8   4 */
   /* 7 6 5 */
{
   if (ObjPtr->type==OBJ_POLY || ObjPtr->type==OBJ_POLYGON || (Corner & 0x1)) {
      RotateSel(XGridOff, YGridOff, ObjPtr, Corner);
   } else {
      ShearSel(XGridOff, YGridOff, ObjPtr, Corner);
   }
}

/* --------------------- Rotation Pivot --------------------- */

static
int RefreshFlipRotateMenu(menu)
   TgMenu *menu;
{
   int ok=TRUE, can_suggest_poly=FALSE, can_suggest_arc=FALSE;

   /* ResetRotationPivot */
   ok &= TgEnableMenuItemById(menu, CMDID_RESETROTATEPIVOT,
         !autoRotatePivot && topSel != NULL);
   /* SpecifyRotationPivot */
   ok &= TgEnableMenuItemById(menu, CMDID_SPECIFYROTATEPIVOT,
         curChoice == ROTATEMODE && topSel != NULL);

   /* NextPolyRotationPivot */
   /* MoveRotationPivotToArcCenter */
   if (!autoRotatePivot && curChoice == ROTATEMODE && topSel != NULL &&
         topSel == botSel) {
      struct ObjRec *obj_ptr=topSel->obj;

      if (obj_ptr->type == OBJ_POLY || obj_ptr->type == OBJ_POLYGON) {
         can_suggest_poly = TRUE;
      } else if (obj_ptr->type == OBJ_ARC) {
         can_suggest_arc = TRUE;
      }
   }
   ok &= TgEnableMenuItemById(menu, CMDID_NEXTPOLYROTATEPIVOT,
         can_suggest_poly);
   ok &= TgEnableMenuItemById(menu, CMDID_MOVEROTATEPIVOTARCCNTR,
         can_suggest_arc);

   return ok;
}

TgMenu *CreateFlipRotateMenu(parent_menu, x, y, menu_info, status_str_xlated)
   TgMenu *parent_menu;
   int x, y;
   TgMenuInfo *menu_info;
   int status_str_xlated; /* ignored, always 0 */
{
   TgMenu *menu=TgCreateMenuFromMenuInfo(parent_menu, x, y, menu_info, FALSE);

   if (menu != NULL) {
      menu->track_menubar = TRUE;
      if (!RefreshFlipRotateMenu(menu)) {
         return TgDestroyMenu(menu, TRUE);
      }
   }
   return menu;
}

static
void HighLightRotatePivotXY(abs_x, abs_y, Dir)
   int abs_x, abs_y, Dir;
{
   int scr_x=OFFSET_X(abs_x), scr_y=OFFSET_Y(abs_y);

   switch (Dir) {
   case FORWARD:
      XDrawLine(mainDisplay, drawWindow, revDefaultGC,
         scr_x-((handleSize<<1)+1), scr_y, scr_x+((handleSize<<1)+1), scr_y);
      XDrawLine(mainDisplay, drawWindow, revDefaultGC,
         scr_x, scr_y-((handleSize<<1)+1), scr_x, scr_y+((handleSize<<1)+1));
      XDrawArc(mainDisplay, drawWindow, revDefaultGC,
         scr_x-((handleSize<<1)+1), scr_y-((handleSize<<1)+1),
         ((handleSize<<2)+2), ((handleSize<<2)+2), 0, (360<<6));
      break;
   case REVERSE:
      XDrawArc(mainDisplay, drawWindow, revDefaultGC,
         scr_x-((handleSize<<1)+1), scr_y-((handleSize<<1)+1),
         ((handleSize<<2)+2), ((handleSize<<2)+2), 0, (360<<6));
      XDrawLine(mainDisplay, drawWindow, revDefaultGC,
         scr_x, scr_y-((handleSize<<1)+1), scr_x, scr_y+((handleSize<<1)+1));
      XDrawLine(mainDisplay, drawWindow, revDefaultGC,
         scr_x-((handleSize<<1)+1), scr_y, scr_x+((handleSize<<1)+1), scr_y);
      break;
   }
}

void HighLightRotatePivot(Dir)
   int Dir;
{
   int scr_x=0, scr_y=0;

   if (topSel == NULL || autoRotatePivot) return;

   if (!rotatePivotAbsXYValid) {
      rotatePivotAbsX = ((selObjLtX+selObjRbX)>>1);
      rotatePivotAbsY = ((selObjLtY+selObjRbY)>>1);
      rotatePivotAbsXYValid = TRUE;

      sprintf(gszMsgBox, TgLoadString(STID_NEW_ROTATE_PIVOT_IS),
            rotatePivotAbsX, rotatePivotAbsY);
      Msg(gszMsgBox);
   }
   scr_x = OFFSET_X(rotatePivotAbsX);
   scr_y = OFFSET_Y(rotatePivotAbsY);

   switch (Dir) {
   case FORWARD:
      XDrawLine(mainDisplay, drawWindow, revDefaultGC,
         scr_x-((handleSize<<1)+1), scr_y, scr_x+((handleSize<<1)+1), scr_y);
      XDrawLine(mainDisplay, drawWindow, revDefaultGC,
         scr_x, scr_y-((handleSize<<1)+1), scr_x, scr_y+((handleSize<<1)+1));
      XDrawArc(mainDisplay, drawWindow, revDefaultGC,
         scr_x-((handleSize<<1)+1), scr_y-((handleSize<<1)+1),
         ((handleSize<<2)+2), ((handleSize<<2)+2), 0, (360<<6));
      break;
   case REVERSE:
      XDrawArc(mainDisplay, drawWindow, revDefaultGC,
         scr_x-((handleSize<<1)+1), scr_y-((handleSize<<1)+1),
         ((handleSize<<2)+2), ((handleSize<<2)+2), 0, (360<<6));
      XDrawLine(mainDisplay, drawWindow, revDefaultGC,
         scr_x, scr_y-((handleSize<<1)+1), scr_x, scr_y+((handleSize<<1)+1));
      XDrawLine(mainDisplay, drawWindow, revDefaultGC,
         scr_x-((handleSize<<1)+1), scr_y, scr_x+((handleSize<<1)+1), scr_y);
      break;
   }
}

int PtInRotatePivot(mouse_x, mouse_y)
   int mouse_x, mouse_y;
{
   int scr_x=0, scr_y=0;
   struct BBRec bbox;

   if (!rotatePivotAbsXYValid) return FALSE;

   scr_x = OFFSET_X(rotatePivotAbsX);
   scr_y = OFFSET_Y(rotatePivotAbsY);

   bbox.ltx = scr_x-((handleSize<<1)+1);
   bbox.lty = scr_y-((handleSize<<1)+1);
   bbox.rbx = scr_x+((handleSize<<1)+1);
   bbox.rby = scr_y+((handleSize<<1)+1);

   return PointInBBox(mouse_x, mouse_y, bbox);
}

static
int RefreshMoveStdRotatePivotMenu(menu)
   TgMenu *menu;
{
   int ok=TRUE;

   /* MoveRotationPivotToCenter */
   ok &= TgEnableMenuItemById(menu, CMDID_MOVEROTATEPIVOTCENTER,
         !autoRotatePivot && curChoice == ROTATEMODE && topSel != NULL);
   /* MoveRotationPivotToLeftTopCorner */
   ok &= TgEnableMenuItemById(menu, CMDID_MOVEROTATEPIVOTLT,
         !autoRotatePivot && curChoice == ROTATEMODE && topSel != NULL);
   /* MoveRotationPivotToRightTopCorner */
   ok &= TgEnableMenuItemById(menu, CMDID_MOVEROTATEPIVOTRT,
         !autoRotatePivot && curChoice == ROTATEMODE && topSel != NULL);
   /* MoveRotationPivotToLeftBottomCorner */
   ok &= TgEnableMenuItemById(menu, CMDID_MOVEROTATEPIVOTLB,
         !autoRotatePivot && curChoice == ROTATEMODE && topSel != NULL);
   /* MoveRotationPivotToRightBottomCorner */
   ok &= TgEnableMenuItemById(menu, CMDID_MOVEROTATEPIVOTRB,
         !autoRotatePivot && curChoice == ROTATEMODE && topSel != NULL);
   /* MoveRotationPivotToLeftCorner */
   ok &= TgEnableMenuItemById(menu, CMDID_MOVEROTATEPIVOTLEFT,
         !autoRotatePivot && curChoice == ROTATEMODE && topSel != NULL);
   /* MoveRotationPivotToRightCorner */
   ok &= TgEnableMenuItemById(menu, CMDID_MOVEROTATEPIVOTRIGHT,
         !autoRotatePivot && curChoice == ROTATEMODE && topSel != NULL);
   /* MoveRotationPivotToTopCorner */
   ok &= TgEnableMenuItemById(menu, CMDID_MOVEROTATEPIVOTTOP,
         !autoRotatePivot && curChoice == ROTATEMODE && topSel != NULL);
   /* MoveRotationPivotToBottomCorner */
   ok &= TgEnableMenuItemById(menu, CMDID_MOVEROTATEPIVOTBOTTOM,
         !autoRotatePivot && curChoice == ROTATEMODE && topSel != NULL);

   return ok;
}

TgMenu *CreateMoveStdRotatePivotMenu(parent_menu, x, y, menu_info,
      status_str_xlated)
   TgMenu *parent_menu;
   int x, y;
   TgMenuInfo *menu_info;
   int status_str_xlated; /* ignored, always 0 */
{
   TgMenu *menu=TgCreateMenuFromMenuInfo(parent_menu, x, y, menu_info, FALSE);

   if (menu != NULL) {
      menu->track_menubar = TRUE;
      if (!RefreshMoveStdRotatePivotMenu(menu)) {
         return TgDestroyMenu(menu, TRUE);
      }
   }
   return menu;
}

void RefreshAutoRotatePivotMenu(menu)
   TgMenu *menu;
{
   int i, num_items=menu->num_items;
   TgMenuItem *menuitems=menu->menuitems;

   for (i=0; i < num_items; i++) {
      TgMenuItem *menu_item=(&menuitems[i]);
      TgMenuItem stMenuItem;

      memset(&stMenuItem, 0, sizeof(TgMenuItem));
      stMenuItem.state = TGBS_NORMAL;
      stMenuItem.checked = (i == (!autoRotatePivot));
      TgSetMenuItemInfo(menu_item, TGMU_MASK_STATE|TGMU_MASK_CHECK,
            &stMenuItem);
   }
}

TgMenu *CreateAutoRotatePivotMenu(parent_menu, x, y, menu_info,
      status_str_xlated)
   TgMenu *parent_menu;
   int x, y;
   TgMenuInfo *menu_info;
   int status_str_xlated; /* ignored, always 0 */
{
   TgMenu *menu=TgCreateMenuFromMenuInfo(parent_menu, x, y, menu_info, FALSE);

   if (menu != NULL) {
      TgMenuItem *menu_item=NULL;
      TgMenuItem stMenuItem;

      menu->track_menubar = TRUE;
      TgAdjustMenuGeometry(menu, choiceImageW, choiceImageH, MAX_ROTATE_PIVOT);
      menu_item = (&menu->menuitems[!autoRotatePivot]);

      memset(&stMenuItem, 0, sizeof(TgMenuItem));
      stMenuItem.checked = TRUE;
      if (!TgSetMenuItemInfo(menu_item, TGMU_MASK_CHECK, &stMenuItem)) {
         return TgDestroyMenu(menu, TRUE);
      }
      menu->refresh_proc = ((RefreshMenuFunc*)RefreshAutoRotatePivotMenu);
   }
   return menu;
}

int AutoRotatePivotMenu(X, Y, TrackMenubar)
   int X, Y, TrackMenubar;
{
   int rc=INVALID;
   TgMenu *menu=(autoRotatePivotMenuInfo.create_proc)(NULL, X, Y,
         &autoRotatePivotMenuInfo, INVALID);

   activeMenu = INVALID;
   if (menu != NULL) {
      menu->track_menubar = TrackMenubar;

      rc = TgMenuLoop(menu);
      TgDestroyMenu(menu, TRUE);
   }
   return rc;
}

void ResetRotatePivotValidInfo()
{
   rotatePivotAbsXYValid = FALSE;
   rotatePivotAbsX = 0;
   rotatePivotAbsY = 0;
}

int ReadRotatePivotInfo(buf)
   char *buf;
{
   int auto_pivot=TRUE, xy_valid=0, abs_x=0, abs_y=0;
   char *psz=NULL;

   if (importingFile) return TRUE;

   psz = FindChar((int)'(', buf);
   InitScan(psz, "\t\n, ");
   if (GETINT("rotate_pivot", auto_pivot, "auto_pivot") == INVALID ||
       GETINT("rotate_pivot", xy_valid, "xy_valid") == INVALID ||
       GETINT("rotate_pivot", abs_x, "x") == INVALID ||
       GETINT("rotate_pivot", abs_y, "y") == INVALID) {
      return FALSE;
   }
   autoRotatePivot = auto_pivot;
   rotatePivotAbsXYValid = xy_valid;
   rotatePivotAbsX = abs_x;
   rotatePivotAbsY = abs_y;
   if (!PRTGIF || cmdLineOpenDisplay) {
      choicePixmap[ROTATEMODE] = rotateModePixmap[!autoRotatePivot];
      RedrawModeWindow();
      UpdatePinnedMenu(MENU_MODE);
   }
   return TRUE;
}

void ContinueMoveRotatePivot(OrigX, OrigY)
   int OrigX, OrigY;
{
   int moving=TRUE, dx=0, dy=0, grid_x=OrigX, grid_y=OrigY;
   char buf[80], x_buf[80], y_buf[80];
   XEvent ev;

   XFlush(mainDisplay);
   XSync(mainDisplay, False);

   if (XCheckMaskEvent(mainDisplay, ExposureMask, &ev) ||
         XCheckMaskEvent(mainDisplay, VisibilityChangeMask, &ev)) {
      ExposeEventHandler(&ev, TRUE);
   }
   PixelToMeasurementUnit(x_buf, rotatePivotAbsX);
   PixelToMeasurementUnit(y_buf, rotatePivotAbsY);
   sprintf(buf, "x=%s\ny=%s", x_buf, y_buf);
   StartShowMeasureCursor(OrigX, OrigY, buf, TRUE);

   if (!debugNoPointerGrab) {
      XGrabPointer(mainDisplay, drawWindow, FALSE,
            PointerMotionMask | ButtonReleaseMask,
            GrabModeAsync, GrabModeAsync, None, moveCursor, CurrentTime);
   }
   dx = dy = 0;

   while (moving) {
      XEvent input;

      XNextEvent(mainDisplay, &input);

      if (input.type == Expose || input.type == VisibilityNotify) {
         ExposeEventHandler(&input, TRUE);
      } else if (input.type == ButtonRelease) {
         XUngrabPointer(mainDisplay, CurrentTime);
         XSync(mainDisplay, False);
         moving = FALSE;

         /* erase */
         PixelToMeasurementUnit(x_buf, rotatePivotAbsX+ABS_SIZE(dx));
         PixelToMeasurementUnit(y_buf, rotatePivotAbsY+ABS_SIZE(dy));
         sprintf(buf, "x=%s\ny=%s", x_buf, y_buf);
         ShowMeasureCursor(grid_x, grid_y, buf, TRUE);
         HighLightRotatePivotXY(rotatePivotAbsX+ABS_SIZE(dx),
               rotatePivotAbsY+ABS_SIZE(dy), REVERSE);

         dx = grid_x - OrigX;
         dy = grid_y - OrigY;
      } else if (input.type == MotionNotify) {
         int x=0, y=0;

         /* erase */
         PixelToMeasurementUnit(x_buf, rotatePivotAbsX+ABS_SIZE(dx));
         PixelToMeasurementUnit(y_buf, rotatePivotAbsY+ABS_SIZE(dy));
         sprintf(buf, "x=%s\ny=%s", x_buf, y_buf);
         ShowMeasureCursor(grid_x, grid_y, buf, TRUE);
         HighLightRotatePivotXY(rotatePivotAbsX+ABS_SIZE(dx),
               rotatePivotAbsY+ABS_SIZE(dy), REVERSE);

         x = input.xmotion.x;
         y = input.xmotion.y;
         GridXY(x, y, &grid_x, &grid_y);

         dx = grid_x - OrigX;
         dy = grid_y - OrigY;

         HighLightRotatePivotXY(rotatePivotAbsX+ABS_SIZE(dx),
               rotatePivotAbsY+ABS_SIZE(dy), FORWARD);
         PixelToMeasurementUnit(x_buf, rotatePivotAbsX+ABS_SIZE(dx));
         PixelToMeasurementUnit(y_buf, rotatePivotAbsY+ABS_SIZE(dy));
         sprintf(buf, "x=%s\ny=%s", x_buf, y_buf);
         ShowMeasureCursor(grid_x, grid_y, buf, TRUE);

         while (XCheckMaskEvent(mainDisplay, PointerMotionMask, &ev)) ;
      }
   }
   if (dx != 0 || dy != 0) {
      rotatePivotAbsX += ABS_SIZE(dx);
      rotatePivotAbsY += ABS_SIZE(dy);
      sprintf(gszMsgBox, TgLoadString(STID_NEW_ROTATE_PIVOT_IS),
            rotatePivotAbsX, rotatePivotAbsY);
      Msg(gszMsgBox);
   }
   HighLightRotatePivotXY(rotatePivotAbsX, rotatePivotAbsY, FORWARD);
}

void ToggleAutoRotatePivot()
{
   if (topSel != NULL && curChoice == ROTATEMODE) {
      if (somethingHighLighted) HighLightReverse();
   }
   autoRotatePivot = !autoRotatePivot;
   if (autoRotatePivot) {
      ResetRotatePivotValidInfo();
      Msg(TgLoadString(STID_SWITCHED_TO_AUTO_ROTATE_PIVOT));
   } else {
      Msg(TgLoadString(STID_SWITCHED_TO_USER_ROTATE_PIVOT));
   }
   choicePixmap[ROTATEMODE] = rotateModePixmap[!autoRotatePivot];
   RedrawModeWindow();
   UpdatePinnedMenu(MENU_MODE);
   if (topSel != NULL && curChoice == ROTATEMODE) {
      if (!somethingHighLighted) HighLightForward();
   }
}

void SpecifyRotatePivot()
{
   char spec[MAXSTRING];
   int abs_x=0, abs_y=0;

   if (autoRotatePivot) {
      UtilStrCpyN(gszMsgBox, sizeof(gszMsgBox),
            TgLoadString(STID_ENTER_ROT_PIVOT));
   } else if (rotatePivotAbsXYValid) {
      sprintf(gszMsgBox, TgLoadString(STID_ENTER_ROT_PIVOT_CUR_IS),
            rotatePivotAbsX, rotatePivotAbsY);
   } else {
      UtilStrCpyN(gszMsgBox, sizeof(gszMsgBox),
            TgLoadString(STID_ENTER_ROT_PIVOT));
   }
   *spec = '\0';
   Dialog(gszMsgBox, TgLoadCachedString(CSTID_DLG_ACCEPT_CANCEL), spec);
   UtilTrimBlanks(spec);
   if (*spec == '\0') return;

   if (ParseXYSpec(spec, &abs_x, &abs_y)) {
      if (autoRotatePivot) {
         ToggleAutoRotatePivot();
         rotatePivotAbsXYValid = TRUE;
      } else if (rotatePivotAbsXYValid) {
         /* nothing to do here */
      } else {
         rotatePivotAbsXYValid = TRUE;
      }
      rotatePivotAbsX = abs_x;
      rotatePivotAbsY = abs_y;
      sprintf(gszMsgBox, TgLoadString(STID_ROT_PIVOT_SET_TO),
            rotatePivotAbsX, rotatePivotAbsY);
      Msg(gszMsgBox);
   }
}

void ResetRotatePivot()
{
   if (topSel != NULL && curChoice == ROTATEMODE) {
      if (somethingHighLighted) HighLightReverse();
   }
   ResetRotatePivotValidInfo();
   if (topSel != NULL && curChoice == ROTATEMODE) {
      if (!somethingHighLighted) HighLightForward();
   }
}

static
IntPoint *GetPolyVertices(N, ObjPtr, NumPts, V, Curved, pn_need_to_free)
   int *N, NumPts, Curved, *pn_need_to_free;
   struct ObjRec *ObjPtr;
   IntPoint *V;
{
   if (ObjPtr->ctm == NULL) {
      *pn_need_to_free = FALSE;
      *N = NumPts;
      return V;
   } else {
      int i=0;
      IntPoint *v=(IntPoint*)malloc(NumPts*sizeof(IntPoint));

      if (v == NULL) FailAllocMessage();
      memset(v, 0, NumPts*sizeof(IntPoint));

      for (i=0; i < NumPts; i++) {
         int x=0, y=0;

         TransformPointThroughCTM(V[i].x-ObjPtr->x, V[i].y-ObjPtr->y,
               ObjPtr->ctm, &x, &y);
         v[i].x = x+ObjPtr->x;
         v[i].y = y+ObjPtr->y;
      }
      *pn_need_to_free = TRUE;
      *N = NumPts;
      return v;
   }
}

void NextPolyRotationPivot()
{
   int i=0, can_suggest=FALSE, num_pts=0, need_to_free=FALSE, min_index=0;
   struct ObjRec *obj_ptr=NULL;
   IntPoint *v=NULL;
   double min_dist=(double)0;

   if (!autoRotatePivot && curChoice == ROTATEMODE && topSel != NULL &&
         topSel == botSel) {
      obj_ptr = topSel->obj;
      if (obj_ptr->type == OBJ_POLY) {
         struct PolyRec *poly_ptr=obj_ptr->detail.p;

         v = GetPolyVertices(&num_pts, obj_ptr, poly_ptr->n, poly_ptr->vlist,
               poly_ptr->curved, &need_to_free);
         can_suggest = TRUE;
      } else if (obj_ptr->type == OBJ_POLYGON) {
         struct PolygonRec *polygon_ptr=obj_ptr->detail.g;

         v = GetPolyVertices(&num_pts, obj_ptr, polygon_ptr->n-1,
               polygon_ptr->vlist, polygon_ptr->curved, &need_to_free);
         can_suggest = TRUE;
      }
   }
   if (!can_suggest) {
      MsgBox(TgLoadString(STID_SELONLYONEPOLYOBJ), TOOL_NAME, INFO_MB);
      return;
   }
   HighLightReverse();
   if (rotatePivotAbsXYValid) {
      int dx=rotatePivotAbsX-v[0].x, dy=rotatePivotAbsY-v[0].y;
      int coincide_index=(-1);
      double ddx=(double)0, ddy=(double)0, dist=(double)0;

      min_index = 0;
      if (dx == 0 && dy == 0) {
         coincide_index = 0;
         min_dist = (double)0;
      } else {
         ddx = (double)dx;
         ddy = (double)dy;
         min_dist = (double)sqrt(ddx*ddx+ddy+ddy);
      }
      for (i=1; i < num_pts; i++) {
         dx = rotatePivotAbsX-v[i].x;
         dy = rotatePivotAbsY-v[i].y;
         if (dx == 0 && dy == 0) {
            if (coincide_index == (-1)) {
               coincide_index = i;
               min_index = i;
               min_dist = (double)0;
            } else if (coincide_index+1 == i) {
               coincide_index = i;
               min_index = i;
               min_dist = (double)0;
            } else {
               /* stick to the original coincide_index */
            }
         } else {
            ddx = (double)dx;
            ddy = (double)dy;
            dist = (double)sqrt(ddx*ddx+ddy+ddy);
            if (dist < min_dist) {
               min_index = i;
               min_dist = dist;
            }
         }
      }
      if (coincide_index != (-1)) {
         min_index = coincide_index+1;
      }
      if (min_index >= num_pts) {
         min_index = 0;
      }
   } else {
      min_index = 0;
   }
   rotatePivotAbsXYValid = TRUE;
   rotatePivotAbsX = v[min_index].x;
   rotatePivotAbsY = v[min_index].y;

   if (need_to_free) free(v);

   sprintf(gszMsgBox, TgLoadString(STID_NEW_ROTATE_PIVOT_IS),
         rotatePivotAbsX, rotatePivotAbsY);
   Msg(gszMsgBox);

   HighLightForward();
}

void MoveRotationPivotToArcCenter()
{
   int can_suggest=FALSE, x=0, y=0;
   struct ObjRec *obj_ptr=NULL;
   struct ArcRec *arc_ptr=NULL;

   if (!autoRotatePivot && curChoice == ROTATEMODE && topSel != NULL &&
         topSel == botSel) {
      obj_ptr = topSel->obj;
      if (obj_ptr->type == OBJ_ARC) {
         arc_ptr = obj_ptr->detail.a;
         can_suggest = TRUE;
      }
   }
   if (!can_suggest) {
      MsgBox(TgLoadString(STID_SELONLYONEARCOBJ), TOOL_NAME, INFO_MB);
      return;
   }
   HighLightReverse();
   if (obj_ptr->ctm == NULL) {
      x = arc_ptr->xc;
      y = arc_ptr->yc;
   } else {
      TransformPointThroughCTM(arc_ptr->xc-obj_ptr->x,
            arc_ptr->yc-obj_ptr->y, obj_ptr->ctm, &x, &y);
      x += obj_ptr->x;
      y += obj_ptr->y;
   }
   rotatePivotAbsXYValid = TRUE;
   rotatePivotAbsX = x;
   rotatePivotAbsY = y;

   sprintf(gszMsgBox, TgLoadString(STID_NEW_ROTATE_PIVOT_IS),
         rotatePivotAbsX, rotatePivotAbsY);
   Msg(gszMsgBox);

   HighLightForward();
}

void MoveRotatePivot(Corner)
   int Corner;
{
   if (topSel == NULL || curChoice != ROTATEMODE) return;

   HighLightReverse();
   switch (Corner) {
   case CORNER_NONE:
      rotatePivotAbsX = ((selObjLtX+selObjRbX)>>1);
      rotatePivotAbsY = ((selObjLtY+selObjRbY)>>1);
      break;
   case CORNER_LT:
      rotatePivotAbsX = selObjLtX;
      rotatePivotAbsY = selObjLtY;
      break;
   case CORNER_RT:
      rotatePivotAbsX = selObjRbX;
      rotatePivotAbsY = selObjLtY;
      break;
   case CORNER_LB:
      rotatePivotAbsX = selObjLtX;
      rotatePivotAbsY = selObjRbY;
      break;
   case CORNER_RB:
      rotatePivotAbsX = selObjRbX;
      rotatePivotAbsY = selObjRbY;
      break;
   case CORNER_LEFT:
      rotatePivotAbsX = selObjLtX;
      rotatePivotAbsY = ((selObjLtY+selObjRbY)>>1);
      break;
   case CORNER_RIGHT:
      rotatePivotAbsX = selObjRbX;
      rotatePivotAbsY = ((selObjLtY+selObjRbY)>>1);
      break;
   case CORNER_TOP:
      rotatePivotAbsX = ((selObjLtX+selObjRbX)>>1);
      rotatePivotAbsY = selObjLtY;
      break;
   case CORNER_BOTTOM:
      rotatePivotAbsX = ((selObjLtX+selObjRbX)>>1);
      rotatePivotAbsY = selObjRbY;
      break;
   }
   rotatePivotAbsXYValid = TRUE;

   sprintf(gszMsgBox, TgLoadString(STID_NEW_ROTATE_PIVOT_IS),
         rotatePivotAbsX, rotatePivotAbsY);
   Msg(gszMsgBox);

   HighLightForward();
}

void AutoRotatePivotSubMenu(index)
   int index;
{
   if (!autoRotatePivot == index) return;

   ToggleAutoRotatePivot();
}

