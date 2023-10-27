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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/move.c,v 1.34 2011/05/16 16:21:58 william Exp $
 */

#define _INCLUDE_FROM_MOVE_C_

#include "tgifdefs.h"

#include "arc.e"
#include "attr.e"
#include "choice.e"
#include "cmd.e"
#include "cursor.e"
#include "dialog.e"
#include "drawing.e"
#include "dup.e"
#include "exec.e"
#include "grid.e"
#include "mainloop.e"
#include "mark.e"
#include "move.e"
#include "msg.e"
#include "names.e"
#include "obj.e"
#include "oval.e"
#include "poly.e"
#include "raster.e"
#include "rcbox.e"
#include "rect.e"
#include "ruler.e"
#include "select.e"
#include "setup.e"
#include "spline.e"
#include "stretch.e"
#include "strtbl.e"

int oneMotionTimeout=200;
int minMoveInterval=0;

static
void MovePoly(ObjPtr, Dx, Dy)
   register struct ObjRec *ObjPtr;
   register int Dx, Dy;
{
   register int i;

   for (i=0; i < ObjPtr->detail.p->n; i++) {
      ObjPtr->detail.p->vlist[i].x += Dx;
      ObjPtr->detail.p->vlist[i].y += Dy;
   }
}

static
void MovePolygon(ObjPtr, Dx, Dy)
   register struct ObjRec *ObjPtr;
   register int Dx, Dy;
{
   register int i;

   for (i=0; i < ObjPtr->detail.g->n; i++) {
      ObjPtr->detail.g->vlist[i].x += Dx;
      ObjPtr->detail.g->vlist[i].y += Dy;
   }
}

static
void MoveArc(ObjPtr, Dx, Dy)
   struct ObjRec *ObjPtr;
   register int Dx, Dy;
{
   register struct ArcRec *arc_ptr=ObjPtr->detail.a;

   arc_ptr->xc += Dx;  arc_ptr->yc += Dy;
   arc_ptr->x1 += Dx;  arc_ptr->y1 += Dy;
   arc_ptr->x2 += Dx;  arc_ptr->y2 += Dy;
   arc_ptr->ltx += Dx; arc_ptr->lty += Dy;
}

void MoveObj(ObjPtr, Dx, Dy)
   struct ObjRec *ObjPtr;
   int Dx, Dy;
{
   struct ObjRec *ptr=NULL;
   int saved_undoing_or_redoing=undoingOrRedoing;

   ObjPtr->x += Dx;
   ObjPtr->y += Dy;
   ObjPtr->bbox.ltx += Dx;
   ObjPtr->bbox.lty += Dy;
   ObjPtr->bbox.rbx += Dx;
   ObjPtr->bbox.rby += Dy;
   ObjPtr->obbox.ltx += Dx;
   ObjPtr->obbox.lty += Dy;
   ObjPtr->obbox.rbx += Dx;
   ObjPtr->obbox.rby += Dy;
   MoveRotatedObjCache(ObjPtr, Dx, Dy);
   switch (ObjPtr->type) {
   case OBJ_POLY:
      MoveAttrs(ObjPtr->fattr, Dx, Dy);
      MovePoly(ObjPtr, Dx, Dy); 
      /* fake the undoingOrRedoing so that no */
      /*        actual auto-adjusting is done */
      undoingOrRedoing = TRUE;
      AdjObjSplineVs(ObjPtr);
      undoingOrRedoing = saved_undoing_or_redoing;
      break;
   case OBJ_BOX:
      MoveAttrs(ObjPtr->fattr, Dx, Dy);
      break;
   case OBJ_OVAL:
      MoveAttrs(ObjPtr->fattr, Dx, Dy);
      break;
   case OBJ_TEXT:
      ObjPtr->detail.t->baseline_y += Dy;
      break;
   case OBJ_POLYGON:
      MoveAttrs(ObjPtr->fattr, Dx, Dy);
      MovePolygon(ObjPtr, Dx, Dy);
      AdjObjSplineVs(ObjPtr);
      break;
   case OBJ_ARC:
      MoveAttrs(ObjPtr->fattr, Dx, Dy);
      MoveArc(ObjPtr, Dx, Dy);
      AdjObjSplineVs(ObjPtr);
      break;
   case OBJ_RCBOX:
      MoveAttrs(ObjPtr->fattr, Dx, Dy);
      break;
   case OBJ_XBM:
      MoveAttrs(ObjPtr->fattr, Dx, Dy);
      break;
   case OBJ_XPM:
      MoveAttrs(ObjPtr->fattr, Dx, Dy);
      break;
   case OBJ_GROUP:
   case OBJ_ICON:
   case OBJ_SYM:
   case OBJ_PIN:
      MoveAttrs(ObjPtr->fattr, Dx, Dy);
      for (ptr=ObjPtr->detail.r->first; ptr != NULL; ptr=ptr->next) {
         MoveObj(ptr, Dx, Dy);
      }
      break;
   }
}

void MoveAllSelObjects(Dx, Dy)
   register int Dx, Dy;
{
   register struct SelRec *sel_ptr;

   for (sel_ptr=topSel; sel_ptr != NULL; sel_ptr=sel_ptr->next) {
      if (!sel_ptr->obj->locked) {
         MoveObj(sel_ptr->obj, Dx, Dy);
      }
   }
   if (numObjLocked != 0) Msg(TgLoadCachedString(CSTID_LOCKED_OBJS_NOT_MOVED));
}

static
void MarkObjectsForMove()
{
   register struct ObjRec *obj_ptr;
   register struct SelRec *sel_ptr;

   for (obj_ptr=botObj; obj_ptr != NULL; obj_ptr=obj_ptr->prev) {
      obj_ptr->marked = FALSE;
   }
   for (sel_ptr=botSel; sel_ptr != NULL; sel_ptr=sel_ptr->prev) {
      sel_ptr->obj->marked = TRUE;
   }
}

static
int EndPtInObjList(XOff, YOff, FirstObjPtr)
   int XOff, YOff;
   struct ObjRec *FirstObjPtr;
   /* XOff and YOff are screen offsets */
{
   register struct ObjRec *obj_ptr;
   register struct AttrRec *attr_ptr;
   int found=FALSE, saved_fill=0, saved_trans_pat=FALSE;

   for (obj_ptr=FirstObjPtr; obj_ptr != NULL; obj_ptr=obj_ptr->next) {
      for (attr_ptr=obj_ptr->fattr; attr_ptr!=NULL; attr_ptr=attr_ptr->next) {
         if (attr_ptr->shown &&
               XOff >= OFFSET_X(attr_ptr->obj->bbox.ltx)-3 &&
               YOff >= OFFSET_Y(attr_ptr->obj->bbox.lty)-3 &&
               XOff <= OFFSET_X(attr_ptr->obj->bbox.rbx)+3 &&
               YOff <= OFFSET_Y(attr_ptr->obj->bbox.rby)+3) {
            return TRUE;
         }
      }
      if (XOff >= OFFSET_X(obj_ptr->bbox.ltx)-3 &&
            YOff >= OFFSET_Y(obj_ptr->bbox.lty)-3 &&
            XOff <= OFFSET_X(obj_ptr->bbox.rbx)+3 &&
            YOff <= OFFSET_Y(obj_ptr->bbox.rby)+3) {
         switch (obj_ptr->type) {
         case OBJ_TEXT: if (FindGoodText(XOff,YOff,obj_ptr)) return TRUE; break;
         case OBJ_XBM: if (FindGoodXBm(XOff,YOff,obj_ptr)) return TRUE; break;
         case OBJ_XPM: if (FindGoodXPm(XOff,YOff,obj_ptr)) return TRUE; break;
         case OBJ_BOX:
            saved_trans_pat = obj_ptr->trans_pat;
            saved_fill = obj_ptr->detail.b->fill;
            obj_ptr->detail.b->fill = SOLIDPAT;
            found = FindGoodBox(XOff,YOff,obj_ptr);
            obj_ptr->detail.b->fill = saved_fill;
            obj_ptr->trans_pat = saved_trans_pat;
            if (found) return TRUE;
            break;
         case OBJ_OVAL:
            saved_trans_pat = obj_ptr->trans_pat;
            saved_fill = obj_ptr->detail.o->fill;
            obj_ptr->detail.o->fill = SOLIDPAT;
            found = FindGoodOval(XOff,YOff,obj_ptr);
            obj_ptr->detail.o->fill = saved_fill;
            obj_ptr->trans_pat = saved_trans_pat;
            if (found) return TRUE;
            break;
         case OBJ_POLY: if (FindGoodPoly(XOff,YOff,obj_ptr)) return TRUE; break;
         case OBJ_POLYGON:
            saved_trans_pat = obj_ptr->trans_pat;
            saved_fill = obj_ptr->detail.g->fill;
            obj_ptr->detail.g->fill = SOLIDPAT;
            found = FindGoodPolygon(XOff,YOff,obj_ptr);
            obj_ptr->detail.g->fill = saved_fill;
            obj_ptr->trans_pat = saved_trans_pat;
            if (found) return TRUE;
            break;
         case OBJ_ARC: if (FindGoodArc(XOff,YOff,obj_ptr)) return TRUE; break;
         case OBJ_RCBOX:
            saved_trans_pat = obj_ptr->trans_pat;
            saved_fill = obj_ptr->detail.rcb->fill;
            obj_ptr->detail.rcb->fill = SOLIDPAT;
            found = FindGoodRCBox(XOff,YOff,obj_ptr);
            obj_ptr->detail.rcb->fill = saved_fill;
            obj_ptr->trans_pat = saved_trans_pat;
            if (found) return TRUE;
            break;

         case OBJ_GROUP:
         case OBJ_ICON:
         case OBJ_SYM:
            if (EndPtInObjList(XOff,YOff,obj_ptr->detail.r->first)) {
               return TRUE;
            }
            break;
         case OBJ_PIN:
            if (EndPtInObjList(XOff,YOff,GetPinObj(obj_ptr)->detail.r->first)) {
               return TRUE;
            }
         }
      }
   }
   return FALSE;
}

int EndPtInSelected(XOff, YOff)
   int XOff, YOff;
   /* XOff and YOff are screen offsets */
{
   register struct SelRec *sel_ptr;
   register struct ObjRec *obj_ptr;
   register struct AttrRec *attr_ptr;
   int found, saved_fill=0, saved_trans_pat=FALSE;

   for (sel_ptr=topSel; sel_ptr != NULL; sel_ptr=sel_ptr->next) {
      obj_ptr = sel_ptr->obj;
      if (obj_ptr->locked) continue;

      for (attr_ptr=obj_ptr->fattr; attr_ptr!=NULL; attr_ptr=attr_ptr->next) {
         if (attr_ptr->shown &&
               XOff >= OFFSET_X(attr_ptr->obj->bbox.ltx)-3 &&
               YOff >= OFFSET_Y(attr_ptr->obj->bbox.lty)-3 &&
               XOff <= OFFSET_X(attr_ptr->obj->bbox.rbx)+3 &&
               YOff <= OFFSET_Y(attr_ptr->obj->bbox.rby)+3) {
            return TRUE;
         }
      }
      if (XOff >= OFFSET_X(obj_ptr->bbox.ltx)-3 &&
            YOff >= OFFSET_Y(obj_ptr->bbox.lty)-3 &&
            XOff <= OFFSET_X(obj_ptr->bbox.rbx)+3 &&
            YOff <= OFFSET_Y(obj_ptr->bbox.rby)+3) {
         switch (obj_ptr->type) {
         case OBJ_TEXT: if (FindGoodText(XOff,YOff,obj_ptr)) return TRUE; break;
         case OBJ_XBM: if (FindGoodXBm(XOff,YOff,obj_ptr)) return TRUE; break;
         case OBJ_XPM: if (FindGoodXPm(XOff,YOff,obj_ptr)) return TRUE; break;
         case OBJ_BOX:
            saved_trans_pat = obj_ptr->trans_pat;
            saved_fill = obj_ptr->detail.b->fill;
            obj_ptr->detail.b->fill = SOLIDPAT;
            found = FindGoodBox(XOff,YOff,obj_ptr);
            obj_ptr->detail.b->fill = saved_fill;
            obj_ptr->trans_pat = saved_trans_pat;
            if (found) return TRUE;
            break;
         case OBJ_OVAL:
            saved_trans_pat = obj_ptr->trans_pat;
            saved_fill = obj_ptr->detail.o->fill;
            obj_ptr->detail.o->fill = SOLIDPAT;
            found = FindGoodOval(XOff,YOff,obj_ptr);
            obj_ptr->detail.o->fill = saved_fill;
            obj_ptr->trans_pat = saved_trans_pat;
            if (found) return TRUE;
            break;
         case OBJ_POLY: if (FindGoodPoly(XOff,YOff,obj_ptr)) return TRUE; break;
         case OBJ_POLYGON:
            saved_trans_pat = obj_ptr->trans_pat;
            saved_fill = obj_ptr->detail.g->fill;
            obj_ptr->detail.g->fill = SOLIDPAT;
            found = FindGoodPolygon(XOff,YOff,obj_ptr);
            obj_ptr->detail.g->fill = saved_fill;
            obj_ptr->trans_pat = saved_trans_pat;
            if (found) return TRUE;
            break;
         case OBJ_ARC: if (FindGoodArc(XOff,YOff,obj_ptr)) return TRUE; break;
         case OBJ_RCBOX:
            saved_trans_pat = obj_ptr->trans_pat;
            saved_fill = obj_ptr->detail.rcb->fill;
            obj_ptr->detail.rcb->fill = SOLIDPAT;
            found = FindGoodRCBox(XOff,YOff,obj_ptr);
            obj_ptr->detail.rcb->fill = saved_fill;
            obj_ptr->trans_pat = saved_trans_pat;
            if (found) return TRUE;
            break;

         case OBJ_GROUP:
         case OBJ_ICON:
         case OBJ_SYM:
            if (EndPtInObjList(XOff,YOff,obj_ptr->detail.r->first)) {
               return TRUE;
            }
            break;
         case OBJ_PIN:
            if (EndPtInObjList(XOff,YOff,GetPinObj(obj_ptr)->detail.r->first)) {
               return TRUE;
            }
            break;
         }
      }
   }
   return FALSE;
}

static
int ConstrainedMoveAllSel(Dx, Dy, ltx, lty, rbx, rby)
   register int Dx, Dy;
   int *ltx, *lty, *rbx, *rby;
{
   struct ObjRec *obj_ptr=NULL;
   int something_stretched=FALSE;

   for (obj_ptr=botObj; obj_ptr != NULL; obj_ptr=obj_ptr->prev) {
      if (!obj_ptr->marked && obj_ptr->type==OBJ_POLY && !obj_ptr->locked) {
         struct PolyRec *poly_ptr=obj_ptr->detail.p;
         IntPoint *v=obj_ptr->detail.p->vlist;
         int num_pts=obj_ptr->detail.p->n;
         int x_off=0, y_off=0, move_first=FALSE, move_last=FALSE;
         int move_2nd=FALSE, move_2nd_to_last=FALSE;
         int possibly_move_3rd=FALSE, possibly_move_3rd_to_last=FALSE;

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
         if (move_first && poly_ptr->curved == LT_STRUCT_SPLINE &&
               v[0].x == v[1].x && v[0].y == v[1].y) {
            move_2nd = TRUE;
            if (num_pts > 4 &&
                  v[2].x == v[3].x && v[2].y == v[3].y &&
                  v[4].x == v[3].x && v[4].y == v[3].y) {
               possibly_move_3rd = TRUE;
            }
         }
         if (move_last && poly_ptr->curved == LT_STRUCT_SPLINE &&
               v[num_pts-1].x == v[num_pts-2].x &&
               v[num_pts-1].y == v[num_pts-2].y) {
            move_2nd_to_last = TRUE;
            if (num_pts > 4 &&
                  v[num_pts-3].x == v[num_pts-4].x &&
                  v[num_pts-3].y == v[num_pts-4].y &&
                  v[num_pts-5].x == v[num_pts-4].x &&
                  v[num_pts-5].y == v[num_pts-4].y) {
               possibly_move_3rd_to_last = TRUE;
            }
         }
         if (move_first || move_last) {
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
            if (move_first && move_last) {
               MoveObj(obj_ptr, Dx, Dy);
            } else {
               int index=INVALID, seg_dx=0, seg_dy=0;
               int cur_seg_dx=0, cur_seg_dy=0;

               if (move_first) {
                  if (num_pts > 2) {
                     if (possibly_move_3rd) {
                        /* only for LT_STRUCT_SPLINE */
                        index = 3;
                        cur_seg_dx = v[index-3].x - v[index].x;
                        cur_seg_dy = v[index-3].y - v[index].y;
                        seg_dx = v[index].x - v[index+3].x;
                        seg_dy = v[index].y - v[index+3].y;
                     } else {
                        index = 1;
                        cur_seg_dx = v[index-1].x - v[index].x;
                        cur_seg_dy = v[index-1].y - v[index].y;
                        seg_dx = v[index].x - v[index+1].x;
                        seg_dy = v[index].y - v[index+1].y;
                     }
                  }
                  v[0].x += Dx; v[0].y += Dy;
                  if (move_2nd) {
                     v[1].x += Dx; v[1].y += Dy;
                  }
               } else {
                  if (num_pts > 2) {
                     if (possibly_move_3rd_to_last) {
                        /* only for LT_STRUCT_SPLINE */
                        index = num_pts-4;
                        cur_seg_dx = v[index+3].x - v[index].x;
                        cur_seg_dy = v[index+3].y - v[index].y;
                        seg_dx = v[index].x - v[index-3].x;
                        seg_dy = v[index].y - v[index-3].y;
                     } else {
                        index = num_pts-2;
                        cur_seg_dx = v[index+1].x - v[index].x;
                        cur_seg_dy = v[index+1].y - v[index].y;
                        seg_dx = v[index].x - v[index-1].x;
                        seg_dy = v[index].y - v[index-1].y;
                     }
                  }
                  v[num_pts-1].x += Dx; v[num_pts-1].y += Dy;
                  if (move_2nd_to_last) {
                     v[num_pts-2].x += Dx; v[num_pts-2].y += Dy;
                  }
               }
               if (poly_ptr->curved == LT_STRUCT_SPLINE) {
                  if (possibly_move_3rd_to_last) {
                     if (cur_seg_dy==0 && cur_seg_dx!=0 &&
                           (seg_dy!=0 || (seg_dy==0 && Dx==0))) {
                        v[index-1].y += Dy;
                        v[index].y += Dy;
                        v[index+1].y += Dy;
                     } else if (cur_seg_dx==0 && cur_seg_dy!=0 &&
                           (seg_dx!=0 || (seg_dx==0 && Dy==0))) {
                        v[index-1].x += Dx;
                        v[index].x += Dx;
                        v[index+1].x += Dx;
                     }
                  }
               } else {
                  if (num_pts>2 && cur_seg_dy==0 && cur_seg_dx!=0 &&
                        (seg_dy!=0 || (seg_dy==0 && Dx==0))) {
                     v[index].y += Dy;
                  } else if (num_pts>2 && cur_seg_dx==0 && cur_seg_dy!=0 &&
                        (seg_dx!=0 || (seg_dx==0 && Dy==0))) {
                     v[index].x += Dx;
                  }
               }
            }
            AdjObjSplineVs(obj_ptr);
            if (obj_ptr->detail.p->curved != LT_INTSPLINE) {
               UpdPolyBBox(obj_ptr, num_pts, v);
            } else {
               UpdPolyBBox(obj_ptr, obj_ptr->detail.p->intn,
                     obj_ptr->detail.p->intvlist);
            }
            if (AutoCenterAttr(obj_ptr)) {
               struct AttrRec *attr_ptr=obj_ptr->fattr;
               int modified=FALSE;

               for ( ; attr_ptr != NULL; attr_ptr=attr_ptr->next) {
                  if (attr_ptr->shown) {
                     struct BBRec bbox;

                     CenterObjInOBBox(attr_ptr->obj, obj_ptr->obbox,
                           &bbox);
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
   MoveAllSelObjects(Dx, Dy);
   return something_stretched;
}

void MoveAllSel(Dx, Dy)
   register int Dx, Dy;
{
   int ltx=0, lty=0, rbx=0, rby=0;
   struct SubCmdRec *sub_cmd=NULL;

   sub_cmd = (struct SubCmdRec *)malloc(sizeof(struct SubCmdRec));
   if (sub_cmd == NULL) FailAllocMessage();
   memset(sub_cmd, 0, sizeof(struct SubCmdRec));
   sub_cmd->detail.move.dx = Dx;
   sub_cmd->detail.move.dy = Dy;

   if (moveMode==CONST_MOVE && !justDupped) {
      MarkObjectsForMove();

      StartCompositeCmd();
      PrepareToRecord(CMD_MOVE, topSel, botSel, numObjSelected);
      RecordCmd(CMD_MOVE, sub_cmd, NULL, NULL, 0);
      if (ConstrainedMoveAllSel(Dx, Dy, &ltx, &lty, &rbx, &rby)) {
         ltx = min(ltx,min(selLtX,selLtX+Dx));
         lty = min(lty,min(selLtY,selLtY+Dy));
         rbx = max(rbx,max(selRbX,selRbX+Dx));
         rby = max(rby,max(selRbY,selRbY+Dy));
         RedrawAnArea(botObj, ltx-GRID_ABS_SIZE(1), lty-GRID_ABS_SIZE(1),
               rbx+GRID_ABS_SIZE(1), rby+GRID_ABS_SIZE(1));
      } else {
         RedrawAreas(botObj, selLtX-GRID_ABS_SIZE(1),
               selLtY-GRID_ABS_SIZE(1),
               selRbX+GRID_ABS_SIZE(1), selRbY+GRID_ABS_SIZE(1),
               selLtX-GRID_ABS_SIZE(1)+Dx, selLtY-GRID_ABS_SIZE(1)+Dy,
               selRbX+GRID_ABS_SIZE(1)+Dx, selRbY+GRID_ABS_SIZE(1)+Dy);
      }
      EndCompositeCmd();
   } else {
      MoveAllSelObjects(Dx, Dy);
      PrepareToRecord(CMD_MOVE, topSel, botSel, numObjSelected);
      RecordCmd(CMD_MOVE, sub_cmd, NULL, NULL, 0);
      RedrawAreas(botObj, selLtX-GRID_ABS_SIZE(1),
            selLtY-GRID_ABS_SIZE(1),
            selRbX+GRID_ABS_SIZE(1), selRbY+GRID_ABS_SIZE(1),
            selLtX-GRID_ABS_SIZE(1)+Dx, selLtY-GRID_ABS_SIZE(1)+Dy,
            selRbX+GRID_ABS_SIZE(1)+Dx, selRbY+GRID_ABS_SIZE(1)+Dy);
   }
   free(sub_cmd);
}

void MoveAnObj(ObjPtr, TopOwner, Dx, Dy)
   struct ObjRec *ObjPtr, *TopOwner;
   int Dx, Dy;
   /* This function is meant to be called from within an internal command */
{
   if (execCurDepth <= 0) {
#ifdef _TGIF_DBG /* debug, do not translate */
      TgAssert(FALSE,
            "MoveAnObj() called not from an internal command!", NULL);
      return;
#endif /* _TGIF_DBG */
   }
   if (ObjPtr == TopOwner) {
      struct SelRec *saved_top_sel=topSel, *saved_bot_sel=botSel;
      int ltx=ObjPtr->bbox.ltx, lty=ObjPtr->bbox.lty;
      int rbx=ObjPtr->bbox.rbx, rby=ObjPtr->bbox.rby;
      struct SubCmdRec *sub_cmd=NULL;

      topSel = botSel = (struct SelRec *)malloc(sizeof(struct SelRec));
      if (topSel == NULL) FailAllocMessage();
      memset(topSel, 0, sizeof(struct SelRec));
      topSel->next = topSel->prev = NULL;
      topSel->obj = ObjPtr;
      UpdSelBBox();

      sub_cmd = (struct SubCmdRec *)malloc(sizeof(struct SubCmdRec));
      if (sub_cmd == NULL) FailAllocMessage();
      memset(sub_cmd, 0, sizeof(struct SubCmdRec));
      sub_cmd->detail.move.dx = Dx;
      sub_cmd->detail.move.dy = Dy;

      MoveAllSelObjects(Dx, Dy);
      PrepareToRecord(CMD_MOVE, topSel, botSel, numObjSelected);
      RecordCmd(CMD_MOVE, sub_cmd, NULL, NULL, 0);
      RedrawAreas(botObj, ltx-GRID_ABS_SIZE(1), lty-GRID_ABS_SIZE(1),
            rbx+GRID_ABS_SIZE(1), rby+GRID_ABS_SIZE(1),
            ltx-GRID_ABS_SIZE(1)+Dx, lty-GRID_ABS_SIZE(1)+Dy,
            rbx+GRID_ABS_SIZE(1)+Dx, rby+GRID_ABS_SIZE(1)+Dy);

      free(sub_cmd);

      free(topSel);
      topSel = saved_top_sel;
      botSel = saved_bot_sel;
      UpdSelBBox();
   } else {
      int ltx=TopOwner->bbox.ltx, lty=TopOwner->bbox.lty;
      int rbx=TopOwner->bbox.rbx, rby=TopOwner->bbox.rby;

      PrepareToReplaceAnObj(TopOwner);
      MoveAttrs(ObjPtr->fattr, Dx, Dy);
      MoveObj(ObjPtr, Dx, Dy);
      if (ObjPtr->bbox.ltx < ltx) ltx = ObjPtr->bbox.ltx;
      if (ObjPtr->bbox.lty < lty) lty = ObjPtr->bbox.lty;
      if (ObjPtr->bbox.rbx > rbx) rbx = ObjPtr->bbox.rbx;
      if (ObjPtr->bbox.rby > rby) rby = ObjPtr->bbox.rby;
      while (ObjPtr != TopOwner) {
         ObjPtr = ObjPtr->tmp_parent;
         AdjObjBBox(ObjPtr);
         if (ObjPtr->bbox.ltx < ltx) ltx = ObjPtr->bbox.ltx;
         if (ObjPtr->bbox.lty < lty) lty = ObjPtr->bbox.lty;
         if (ObjPtr->bbox.rbx > rbx) rbx = ObjPtr->bbox.rbx;
         if (ObjPtr->bbox.rby > rby) rby = ObjPtr->bbox.rby;
      }
      RecordReplaceAnObj(TopOwner);
      RedrawAnArea(botObj, ltx-GRID_ABS_SIZE(1), lty-GRID_ABS_SIZE(1),
            rbx+GRID_ABS_SIZE(1), rby+GRID_ABS_SIZE(1));
      UpdSelBBox();
   }
}

typedef struct tagRubberRec {
   struct BBRec obbox;
   XPoint *sv, *pv;
   int sn;
   int radius; /* for rcbox */

   XPoint *v; /* for oval, poly or polygon */
   int num_pts, curved, intn; /* for oval, poly or polygon */
   IntPoint *cntrlv, *polyv; /* for poly or polygon */
   char *smooth; /* for poly or polygon */

   int ltx, lty, w, h, angle1, angle2, fill; /* for arc */
   int saved_xc, saved_yc, saved_x1, saved_y1; /* for arc */
   int saved_x2, saved_y2, saved_ltx, saved_lty; /* for arc */
   int xc, yc, x1, y1, x2, y2; /* for arc */
} RubberInfo;

static
void FreeRubberInfo(pInnerObj, pRubberInfo)
   struct ObjRec *pInnerObj;
   RubberInfo *pRubberInfo;
{
   switch (pInnerObj->type) {
   case OBJ_ARC:
      if (pInnerObj->ctm != NULL) {
         free(pRubberInfo->sv);
         free(pRubberInfo->pv);
      }
      break;

   case OBJ_RCBOX:
   case OBJ_OVAL:
      if (pInnerObj->ctm != NULL) {
         free(pRubberInfo->sv);
         free(pRubberInfo->pv);
      }
      break;

   case OBJ_POLY:
   case OBJ_POLYGON:
      if (pRubberInfo->sv != NULL) free(pRubberInfo->sv);
      if (pRubberInfo->polyv != NULL) free(pRubberInfo->polyv);
      if (pRubberInfo->smooth != NULL) free(pRubberInfo->smooth);
      if (pRubberInfo->curved == LT_INTSPLINE &&
            pRubberInfo->cntrlv != NULL) {
         free(pRubberInfo->cntrlv);
      }
      break;
   }
}

static
void SetRubberInfo(pInnerObj, pRubberInfo)
   struct ObjRec *pInnerObj;
   RubberInfo *pRubberInfo;
{
   int i=0, n=0;
   IntPoint *vs=NULL;
   char *smooth=NULL;
   struct PolyRec *poly_ptr=NULL;
   struct PolygonRec *polygon_ptr=NULL;

   switch (pInnerObj->type) {
   case OBJ_BOX:
   case OBJ_XBM:
   case OBJ_XPM:
   case OBJ_TEXT:
      if (pInnerObj->ctm == NULL) {
         pRubberInfo->obbox.ltx = OFFSET_X(pInnerObj->obbox.ltx);
         pRubberInfo->obbox.lty = OFFSET_Y(pInnerObj->obbox.lty);
         pRubberInfo->obbox.rbx = OFFSET_X(pInnerObj->obbox.rbx);
         pRubberInfo->obbox.rby = OFFSET_Y(pInnerObj->obbox.rby);
      } else {
         /* no need to do anything here, everything is computed */
      }
      break;
   case OBJ_RCBOX:
      if (pInnerObj->ctm == NULL) {
         pRubberInfo->obbox.ltx = OFFSET_X(pInnerObj->obbox.ltx);
         pRubberInfo->obbox.lty = OFFSET_Y(pInnerObj->obbox.lty);
         pRubberInfo->obbox.rbx = OFFSET_X(pInnerObj->obbox.rbx);
         pRubberInfo->obbox.rby = OFFSET_Y(pInnerObj->obbox.rby);
         pRubberInfo->radius = pInnerObj->detail.rcb->radius;
      } else {
         pRubberInfo->sn = pInnerObj->detail.rcb->rotated_n;
         pRubberInfo->sv = (XPoint*)malloc(pRubberInfo->sn*sizeof(XPoint));
         pRubberInfo->pv = (XPoint*)malloc(pRubberInfo->sn*sizeof(XPoint));
         if (pRubberInfo->sv == NULL || pRubberInfo->pv == NULL) {
            FailAllocMessage();
         }
         for (i=0; i < pRubberInfo->sn; i++) {
            pRubberInfo->pv[i].x = pInnerObj->detail.rcb->rotated_vlist[i].x;
            pRubberInfo->pv[i].y = pInnerObj->detail.rcb->rotated_vlist[i].y;
         }
      }
      break;
   case OBJ_ARC:
      if (pInnerObj->ctm == NULL) {
         struct ObjRec *arc_obj_ptr=DupObj(pInnerObj);
         struct ArcRec *arc_ptr=arc_obj_ptr->detail.a;

         pRubberInfo->fill = arc_ptr->fill;
         pRubberInfo->ltx = OFFSET_X(arc_ptr->ltx);
         pRubberInfo->lty = OFFSET_Y(arc_ptr->lty);
         pRubberInfo->w = OFFSET_X(arc_ptr->ltx+arc_ptr->w)-pRubberInfo->ltx;
         pRubberInfo->h = OFFSET_Y(arc_ptr->lty+arc_ptr->h)-pRubberInfo->lty;
         pRubberInfo->angle1 = arc_ptr->angle1;
         pRubberInfo->angle2 = arc_ptr->angle2;
         pRubberInfo->xc = OFFSET_X(arc_ptr->xc);
         pRubberInfo->yc = OFFSET_Y(arc_ptr->yc);
         pRubberInfo->x1 = OFFSET_X(arc_ptr->x1);
         pRubberInfo->y1 = OFFSET_Y(arc_ptr->y1);
         ArcRealX2Y2(arc_ptr, &pRubberInfo->x2, &pRubberInfo->y2);
         pRubberInfo->x2 = OFFSET_X(pRubberInfo->x2);
         pRubberInfo->y2 = OFFSET_Y(pRubberInfo->y2);
         pRubberInfo->saved_xc = pRubberInfo->xc;
         pRubberInfo->saved_yc = pRubberInfo->yc;
         pRubberInfo->saved_x1 = pRubberInfo->x1;
         pRubberInfo->saved_y1 = pRubberInfo->y1;
         pRubberInfo->saved_x2 = pRubberInfo->x2;
         pRubberInfo->saved_y2 = pRubberInfo->y2;
         pRubberInfo->saved_ltx = pRubberInfo->ltx;
         pRubberInfo->saved_lty = pRubberInfo->lty;

         FreeArcObj(arc_obj_ptr);
      } else {
         pRubberInfo->sn = pInnerObj->detail.a->rotated_n;
         pRubberInfo->sv = (XPoint*)malloc((pRubberInfo->sn+2)*sizeof(XPoint));
         pRubberInfo->pv = (XPoint*)malloc((pRubberInfo->sn+2)*sizeof(XPoint));
         if (pRubberInfo->sv == NULL || pRubberInfo->pv == NULL) {
            FailAllocMessage();
         }
         for (i=0; i < pRubberInfo->sn+2; i++) {
            pRubberInfo->pv[i].x = pInnerObj->detail.a->rotated_vlist[i].x;
            pRubberInfo->pv[i].y = pInnerObj->detail.a->rotated_vlist[i].y;
         }
      }
      break;
   case OBJ_OVAL:
      if (pInnerObj->ctm == NULL) {
         pRubberInfo->num_pts = 13;
         pRubberInfo->obbox.ltx = OFFSET_X(pInnerObj->obbox.ltx);
         pRubberInfo->obbox.lty = OFFSET_Y(pInnerObj->obbox.lty);
         pRubberInfo->obbox.rbx = OFFSET_X(pInnerObj->obbox.rbx);
         pRubberInfo->obbox.rby = OFFSET_Y(pInnerObj->obbox.rby);
      } else {
         pRubberInfo->sn = pInnerObj->detail.o->rotated_n;
         pRubberInfo->sv = (XPoint*)malloc(pRubberInfo->sn*sizeof(XPoint));
         pRubberInfo->pv = (XPoint*)malloc(pRubberInfo->sn*sizeof(XPoint));
         if (pRubberInfo->sv == NULL || pRubberInfo->pv == NULL) {
            FailAllocMessage();
         }
         for (i=0; i < pRubberInfo->sn; i++) {
            pRubberInfo->pv[i].x = pInnerObj->detail.o->rotated_vlist[i].x;
            pRubberInfo->pv[i].y = pInnerObj->detail.o->rotated_vlist[i].y;
         }
      }
      break;
   case OBJ_POLY:
      poly_ptr = pInnerObj->detail.p;
      pRubberInfo->curved = poly_ptr->curved;
      if (pRubberInfo->curved == LT_STRUCT_SPLINE) {
         n = poly_ptr->ssn;
         vs = poly_ptr->ssvlist;
         smooth = poly_ptr->ssmooth;
      } else {
         n = poly_ptr->n;
         vs = poly_ptr->vlist;
         smooth = poly_ptr->smooth;
      }
      pRubberInfo->num_pts = n;
      pRubberInfo->polyv =
            (IntPoint*)malloc((pRubberInfo->num_pts+1)*sizeof(IntPoint));
      if (pRubberInfo->polyv == NULL) FailAllocMessage();
      if (pRubberInfo->curved != LT_INTSPLINE && smooth != NULL) {
         pRubberInfo->smooth = (char*)malloc((n+1)*sizeof(char));
         if (pRubberInfo->smooth == NULL) FailAllocMessage();
      }
      if (pInnerObj->ctm == NULL) {
         for (i=0; i < pRubberInfo->num_pts; i++) {
            pRubberInfo->polyv[i].x = vs[i].x;
            pRubberInfo->polyv[i].y = vs[i].y;
            if (pRubberInfo->smooth != NULL) {
               pRubberInfo->smooth[i] = smooth[i];
            }
         }
      } else {
         for (i=0; i < pRubberInfo->num_pts; i++) {
            int x=0, y=0;

            TransformPointThroughCTM(vs[i].x-pInnerObj->x, vs[i].y-pInnerObj->y,
                  pInnerObj->ctm, &x, &y);
            pRubberInfo->polyv[i].x = x+pInnerObj->x;
            pRubberInfo->polyv[i].y = y+pInnerObj->y;
            if (pRubberInfo->smooth != NULL) {
               pRubberInfo->smooth[i] = smooth[i];
            }
         }
      }
      if (pRubberInfo->curved != LT_INTSPLINE) {
         pRubberInfo->sv = MakeMultiSplinePolyVertex(pRubberInfo->curved,
               &pRubberInfo->sn, pRubberInfo->smooth, drawOrigX, drawOrigY,
               pRubberInfo->num_pts, pRubberInfo->polyv);
      } else {
         pRubberInfo->sv = MakeIntSplinePolyVertex(&pRubberInfo->sn,
               &pRubberInfo->intn, &pRubberInfo->cntrlv, drawOrigX,
               drawOrigY, pRubberInfo->num_pts, pRubberInfo->polyv);
      }
      break;
   case OBJ_POLYGON:
      polygon_ptr = pInnerObj->detail.g;
      pRubberInfo->curved = polygon_ptr->curved;
      if (pRubberInfo->curved == LT_STRUCT_SPLINE) {
         n = polygon_ptr->ssn;
         vs = polygon_ptr->ssvlist;
         smooth = polygon_ptr->ssmooth;
      } else {
         n = polygon_ptr->n;
         vs = polygon_ptr->vlist;
         smooth = polygon_ptr->smooth;
      }
      pRubberInfo->num_pts = n;
      pRubberInfo->polyv =
            (IntPoint*)malloc((pRubberInfo->num_pts+1)*sizeof(IntPoint));
      if (pRubberInfo->polyv == NULL) FailAllocMessage();
      if (pRubberInfo->curved != LT_INTSPLINE && smooth != NULL) {
         pRubberInfo->smooth =
               (char*)malloc((pRubberInfo->num_pts+1)*sizeof(char));
         if (pRubberInfo->smooth == NULL) FailAllocMessage();
      }
      if (pInnerObj->ctm == NULL) {
         for (i=0; i < pRubberInfo->num_pts; i++) {
            pRubberInfo->polyv[i].x = vs[i].x;
            pRubberInfo->polyv[i].y = vs[i].y;
            if (pRubberInfo->smooth != NULL) {
               pRubberInfo->smooth[i] = smooth[i];
            }
         }
      } else {
         for (i=0; i < pRubberInfo->num_pts; i++) {
            int x=0, y=0;

            TransformPointThroughCTM(vs[i].x-pInnerObj->x, vs[i].y-pInnerObj->y,
                  pInnerObj->ctm, &x, &y);
            pRubberInfo->polyv[i].x = x+pInnerObj->x;
            pRubberInfo->polyv[i].y = y+pInnerObj->y;
            if (pRubberInfo->smooth != NULL) {
               pRubberInfo->smooth[i] = smooth[i];
            }
         }
      }
      if (pRubberInfo->curved != LT_INTSPLINE) {
         pRubberInfo->sv = MakeMultiSplinePolygonVertex(pRubberInfo->curved,
               &pRubberInfo->sn, pRubberInfo->smooth, drawOrigX, drawOrigY,
               pRubberInfo->num_pts, pRubberInfo->polyv);
      } else {
         pRubberInfo->sv = MakeIntSplinePolygonVertex(&pRubberInfo->sn,
               &pRubberInfo->intn, &pRubberInfo->cntrlv, drawOrigX,
               drawOrigY, pRubberInfo->num_pts, pRubberInfo->polyv);
      }
      break;
   case OBJ_GROUP:
   case OBJ_ICON:
   case OBJ_SYM:
   case OBJ_PIN:
      pRubberInfo->obbox.ltx = OFFSET_X(pInnerObj->obbox.ltx);
      pRubberInfo->obbox.lty = OFFSET_Y(pInnerObj->obbox.lty);
      pRubberInfo->obbox.rbx = OFFSET_X(pInnerObj->obbox.rbx);
      pRubberInfo->obbox.rby = OFFSET_Y(pInnerObj->obbox.rby);
      break;
   }
}

static
void DrawInnerRubberObj(pInnerObj, pRubberInfo, dx, dy)
   struct ObjRec *pInnerObj;
   RubberInfo *pRubberInfo;
   int dx, dy;
{
   int i=0;

   switch (pInnerObj->type) {
   case OBJ_BOX:
   case OBJ_XBM:
   case OBJ_XPM:
   case OBJ_TEXT:
      if (pInnerObj->ctm == NULL) {
         SelBox(drawWindow, revDefaultGC,
               pRubberInfo->obbox.ltx+dx, pRubberInfo->obbox.lty+dy,
               pRubberInfo->obbox.rbx+dx, pRubberInfo->obbox.rby+dy);
      } else {
         XPoint obj_obbox_vs[5];

         for (i=0; i < 5; i++) {
            obj_obbox_vs[i].x = pInnerObj->rotated_obbox[i].x+dx;
            obj_obbox_vs[i].y = pInnerObj->rotated_obbox[i].y+dy;
         }
         XDrawLines(mainDisplay, drawWindow, revDefaultGC,
               obj_obbox_vs, 5, CoordModeOrigin);
      }
      break;
   case OBJ_RCBOX:
      if (pInnerObj->ctm == NULL) {
         SetRCBoxVertex(
               pRubberInfo->obbox.ltx+dx, pRubberInfo->obbox.lty+dy,
               pRubberInfo->obbox.rbx+dx, pRubberInfo->obbox.rby+dy,
               pRubberInfo->radius);
         MyRCBox(drawWindow, revDefaultGC,
               pRubberInfo->obbox.ltx, pRubberInfo->obbox.lty,
               pRubberInfo->obbox.rbx, pRubberInfo->obbox.rby,
               pRubberInfo->radius);
      } else {
         for (i=0; i < pRubberInfo->sn; i++) {
            pRubberInfo->sv[i].x = pRubberInfo->pv[i].x + dx;
            pRubberInfo->sv[i].y = pRubberInfo->pv[i].y + dy;
         }
         XDrawLines(mainDisplay, drawWindow, revDefaultGC,
               pRubberInfo->sv, pRubberInfo->sn, CoordModeOrigin);
      }
      break;
   case OBJ_ARC:
      if (pInnerObj->ctm == NULL) {
         if (!(pRubberInfo->fill == NONEPAT ||
               (pInnerObj->trans_pat && pRubberInfo->fill == BACKPAT))) {
            XDrawLine(mainDisplay, drawWindow, revDefaultGC,
                  pRubberInfo->xc, pRubberInfo->yc,
                  pRubberInfo->x1, pRubberInfo->y1);
            XDrawLine(mainDisplay, drawWindow, revDefaultGC,
                  pRubberInfo->xc, pRubberInfo->yc,
                  pRubberInfo->x2, pRubberInfo->y2);
         }
         XDrawArc(mainDisplay, drawWindow, revDefaultGC,
               pRubberInfo->ltx, pRubberInfo->lty,
               pRubberInfo->w, pRubberInfo->h,
               pRubberInfo->angle1, pRubberInfo->angle2);
      } else {
         for (i=0; i < pRubberInfo->sn+2; i++) {
            pRubberInfo->sv[i].x = pRubberInfo->pv[i].x + dx;
            pRubberInfo->sv[i].y = pRubberInfo->pv[i].y + dy;
         }
         if (!(pRubberInfo->fill == NONEPAT ||
               (pInnerObj->trans_pat && pRubberInfo->fill == BACKPAT))) {
            XDrawLines(mainDisplay, drawWindow, revDefaultGC,
                  pRubberInfo->sv, pRubberInfo->sn+2, CoordModeOrigin);
         } else {
            XDrawLines(mainDisplay, drawWindow, revDefaultGC,
                  pRubberInfo->sv, pRubberInfo->sn, CoordModeOrigin);
         }
      }
      break;
   case OBJ_OVAL:
      if (pInnerObj->ctm == NULL) {
         struct BBRec o_bbox;

         o_bbox.ltx = pRubberInfo->obbox.ltx+dx;
         o_bbox.lty = pRubberInfo->obbox.lty+dy;
         o_bbox.rbx = pRubberInfo->obbox.rbx+dx;
         o_bbox.rby = pRubberInfo->obbox.rby+dy;
         MyOval(drawWindow, revDefaultGC, o_bbox);
      } else {
         for (i=0; i < pRubberInfo->sn; i++) {
            pRubberInfo->sv[i].x = pRubberInfo->pv[i].x + dx;
            pRubberInfo->sv[i].y = pRubberInfo->pv[i].y + dy;
         }
         XDrawLines(mainDisplay, drawWindow, revDefaultGC,
               pRubberInfo->sv, pRubberInfo->sn, CoordModeOrigin);
      }
      break;
   case OBJ_POLY:
   case OBJ_POLYGON:
      XDrawLines(mainDisplay, drawWindow, revDefaultGC,
            pRubberInfo->sv, pRubberInfo->sn, CoordModeOrigin);
      break;
   case OBJ_GROUP:
   case OBJ_ICON:
   case OBJ_SYM:
   case OBJ_PIN:
      SelBox(drawWindow, revDefaultGC,
            pRubberInfo->obbox.ltx+dx, pRubberInfo->obbox.lty+dy,
            pRubberInfo->obbox.rbx+dx, pRubberInfo->obbox.rby+dy);
      break;
   }
}

static
void UpdateInnerRubberObj(pInnerObj, pRubberInfo, dx, dy)
   struct ObjRec *pInnerObj;
   RubberInfo *pRubberInfo;
   int dx, dy;
{
   int i=0, n=0;
   IntPoint *vs=NULL;
   struct PolyRec *poly_ptr=NULL;
   struct PolygonRec *polygon_ptr=NULL;

   switch (pInnerObj->type) {
   case OBJ_ARC:
      if (pInnerObj->ctm == NULL) {
         pRubberInfo->xc = pRubberInfo->saved_xc+dx;
         pRubberInfo->yc = pRubberInfo->saved_yc+dy;
         pRubberInfo->x1 = pRubberInfo->saved_x1+dx;
         pRubberInfo->y1 = pRubberInfo->saved_y1+dy;
         pRubberInfo->x2 = pRubberInfo->saved_x2+dx;
         pRubberInfo->y2 = pRubberInfo->saved_y2+dy;
         pRubberInfo->ltx = pRubberInfo->saved_ltx+dx;
         pRubberInfo->lty = pRubberInfo->saved_lty+dy;
      }
      break;
   case OBJ_POLY:
   case OBJ_POLYGON:
      if (pRubberInfo->sv != NULL) {
         free(pRubberInfo->sv);
         pRubberInfo->sv = NULL;
      }
      switch (pInnerObj->type) {
      case OBJ_POLY:
         poly_ptr = pInnerObj->detail.p;
         if (poly_ptr->curved == LT_STRUCT_SPLINE) {
            n = poly_ptr->ssn;
            vs = poly_ptr->ssvlist;
         } else {
            n = poly_ptr->n;
            vs = poly_ptr->vlist;
         }
         if (pInnerObj->ctm == NULL) {
            for (i=0; i < pRubberInfo->num_pts; i++) {
               pRubberInfo->polyv[i].x = vs[i].x+ABS_SIZE(dx);
               pRubberInfo->polyv[i].y = vs[i].y+ABS_SIZE(dy);
            }
         } else {
            for (i=0; i < pRubberInfo->num_pts; i++) {
               int x=0, y=0;

               TransformPointThroughCTM(vs[i].x-pInnerObj->x,
                     vs[i].y-pInnerObj->y, pInnerObj->ctm, &x, &y);
               pRubberInfo->polyv[i].x = x+pInnerObj->x+ABS_SIZE(dx);
               pRubberInfo->polyv[i].y = y+pInnerObj->y+ABS_SIZE(dy);
            }
         }
         if (pRubberInfo->curved != LT_INTSPLINE) {
            pRubberInfo->sv = MakeMultiSplinePolyVertex(pRubberInfo->curved,
                  &pRubberInfo->sn, pRubberInfo->smooth, drawOrigX,
                  drawOrigY, pRubberInfo->num_pts, pRubberInfo->polyv);
         } else {
            free(pRubberInfo->cntrlv);
            pRubberInfo->sv = MakeIntSplinePolyVertex(&pRubberInfo->sn,
                  &pRubberInfo->intn, &pRubberInfo->cntrlv, drawOrigX,
                  drawOrigY, pRubberInfo->num_pts, pRubberInfo->polyv);
         }
         break;
      case OBJ_POLYGON:
         polygon_ptr = pInnerObj->detail.g;
         if (polygon_ptr->curved == LT_STRUCT_SPLINE) {
            n = polygon_ptr->ssn;
            vs = polygon_ptr->ssvlist;
         } else {
            n = polygon_ptr->n;
            vs = polygon_ptr->vlist;
         }
         if (pInnerObj->ctm == NULL) {
            for (i=0; i < pRubberInfo->num_pts; i++) {
               pRubberInfo->polyv[i].x = vs[i].x+ABS_SIZE(dx);
               pRubberInfo->polyv[i].y = vs[i].y+ABS_SIZE(dy);
            }
         } else {
            for (i=0; i < pRubberInfo->num_pts; i++) {
               int x=0, y=0;

               TransformPointThroughCTM(vs[i].x-pInnerObj->x,
                     vs[i].y-pInnerObj->y, pInnerObj->ctm, &x, &y);
               pRubberInfo->polyv[i].x = x+pInnerObj->x+ABS_SIZE(dx);
               pRubberInfo->polyv[i].y = y+pInnerObj->y+ABS_SIZE(dy);
            }
         }
         if (pRubberInfo->curved != LT_INTSPLINE) {
            pRubberInfo->sv = MakeMultiSplinePolygonVertex(pRubberInfo->curved,
                  &pRubberInfo->sn, pRubberInfo->smooth, drawOrigX, drawOrigY,
                  pRubberInfo->num_pts, pRubberInfo->polyv);
         } else {
            free(pRubberInfo->cntrlv);
            pRubberInfo->sv = MakeIntSplinePolygonVertex(&pRubberInfo->sn,
                  &pRubberInfo->intn, &pRubberInfo->cntrlv, drawOrigX,
                  drawOrigY, pRubberInfo->num_pts, pRubberInfo->polyv);
         }
         break;
      }
      break;
   }
}

static
void MoveSubObjEventCheck(ev, pn_move_sub_obj)
   XEvent *ev;
   int *pn_move_sub_obj;
{
   if (ev->type == MotionNotify) {
      *pn_move_sub_obj = (ev->xmotion.state & ControlMask) &&
            (!(ev->xmotion.state & ShiftMask));
   } else if (ev->type == KeyPress || ev->type == KeyRelease) {
      char s[80];
      KeySym key_sym;

      XLookupString(&(ev->xkey), s, sizeof(s), &key_sym, NULL);
      if (key_sym == XK_Control_L || key_sym == XK_Control_R) {
         *pn_move_sub_obj = (ev->type == KeyPress);
      }
   }
}

void MoveAnAttr(attr_ptr, attr_owner_obj, dx, dy)
   struct AttrRec *attr_ptr;
   struct ObjRec *attr_owner_obj;
   int dx, dy;
{
   struct ObjRec *text_obj_ptr=attr_ptr->obj;
   int ltx, lty, rbx, rby;

   if (attr_owner_obj == NULL) {
      attr_owner_obj = GetTopOwner(attr_ptr->owner);
   }
   ltx = attr_owner_obj->bbox.ltx;
   lty = attr_owner_obj->bbox.lty;
   rbx = attr_owner_obj->bbox.rbx;
   rby = attr_owner_obj->bbox.rby;
   PrepareToReplaceAnObj(attr_owner_obj);
   MoveObj(text_obj_ptr, dx, dy);
   RecursivelyAdjObjBBox(attr_ptr->owner, attr_ptr->owner, attr_owner_obj);
   RecordReplaceAnObj(attr_owner_obj);
   RedrawAreas(botObj, ltx-GRID_ABS_SIZE(1), lty-GRID_ABS_SIZE(1),
         rbx+GRID_ABS_SIZE(1), rby+GRID_ABS_SIZE(1),
         attr_owner_obj->bbox.ltx-GRID_ABS_SIZE(1),
         attr_owner_obj->bbox.lty-GRID_ABS_SIZE(1),
         attr_owner_obj->bbox.rbx+GRID_ABS_SIZE(1),
         attr_owner_obj->bbox.rby+GRID_ABS_SIZE(1));
   SetFileModified(TRUE);
}

void MoveSel(OrigX, OrigY, ObjPtr, down_button_ev)
   int OrigX, OrigY;
   struct ObjRec *ObjPtr;
   XButtonEvent *down_button_ev;
{
   int sel_ltx=0, sel_lty=0, sel_rbx=0, sel_rby=0;
   int ruler_ltx=0, ruler_lty=0, ruler_rbx=0, ruler_rby=0;
   int moving=TRUE, dx=0, dy=0, grid_x=OrigX, grid_y=OrigY;
   int can_move_sub_obj=FALSE, move_sub_obj=FALSE, no_no_lock_sel=FALSE;
   char buf[80], x_buf[80], y_buf[80];
   XEvent ev;
   Time down_click_time=(Time)0;
   RubberInfo rubber_info;

   if (down_button_ev != NULL) {
      down_click_time = down_button_ev->time;
   }
   if (numObjSelected == numObjLocked || ObjPtr->locked) {
      no_no_lock_sel = TRUE;
   }
   XFlush(mainDisplay);
   XSync(mainDisplay, False);

   if (XCheckMaskEvent(mainDisplay, ExposureMask, &ev) ||
         XCheckMaskEvent(mainDisplay, VisibilityChangeMask, &ev)) {
      ExposeEventHandler(&ev, TRUE);
   }
   if (ObjPtr->type == OBJ_TEXT && ObjPtr->detail.t->attr != NULL) {
      can_move_sub_obj = TRUE;
      SetStringStatus(TgLoadString(STID_HINT_CTL_MOVE_ATTR_ONLY));
   }
   if (!move_sub_obj && !no_no_lock_sel) {
      sel_ltx = OFFSET_X(selNoLockLtX)-1; sel_lty = OFFSET_Y(selNoLockLtY)-1;
      sel_rbx = OFFSET_X(selNoLockRbX)+1; sel_rby = OFFSET_Y(selNoLockRbY)+1;
   }
   ruler_ltx = OFFSET_X(selNoLockObjLtX); ruler_lty = OFFSET_Y(selNoLockObjLtY);
   ruler_rbx = OFFSET_X(selNoLockObjRbX); ruler_rby = OFFSET_Y(selNoLockObjRbY);

   if (!move_sub_obj && !no_no_lock_sel) {
      SelBox(drawWindow, revDefaultGC, sel_ltx, sel_lty, sel_rbx, sel_rby);
   }
   PixelToMeasurementUnit(x_buf, 0);
   PixelToMeasurementUnit(y_buf, 0);
   if (VerboseMeasureTooltip()) {
      sprintf(buf, "dx=%s dy=%s", x_buf, y_buf);
   } else {
      sprintf(buf, "dx=%s\ndy=%s", x_buf, y_buf);
   }
   StartShowMeasureCursor(OrigX, OrigY, buf, TRUE);
   BeginIntervalRulers(ruler_ltx, ruler_lty, ruler_rbx, ruler_rby);

   memset(&rubber_info, 0, sizeof(RubberInfo));
   SetRubberInfo(ObjPtr, &rubber_info);
   DrawInnerRubberObj(ObjPtr, &rubber_info, 0, 0);

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
         Time release_time=input.xbutton.time;

         XUngrabPointer(mainDisplay, CurrentTime);
         XSync(mainDisplay, False);
         moving = FALSE;

         EndIntervalRulers(grid_x, grid_y);
         PixelToMeasurementUnit(x_buf, ABS_SIZE(dx));
         PixelToMeasurementUnit(y_buf, ABS_SIZE(dy));
         if (VerboseMeasureTooltip()) {
            sprintf(buf, "dx=%s dy=%s", x_buf, y_buf);
         } else {
            sprintf(buf, "dx=%s\ndy=%s", x_buf, y_buf);
         }
         EndShowMeasureCursor(grid_x, grid_y, buf, TRUE);
         if (!move_sub_obj && !no_no_lock_sel) {
            SelBox(drawWindow, revDefaultGC, sel_ltx+dx, sel_lty+dy, sel_rbx+dx,
                  sel_rby+dy);
         }
         DrawInnerRubberObj(ObjPtr, &rubber_info, dx, dy);

         dx = grid_x - OrigX;
         dy = grid_y - OrigY;

         if (oneMotionSelectMove && down_button_ev != NULL &&
               (release_time-down_click_time) < oneMotionTimeout) {
            dx = dy = 0;
         } else if (!oneMotionSelectMove && down_button_ev != NULL &&
               (release_time-down_click_time) < minMoveInterval) {
            dx = dy = 0;
         }
      } else if (input.type == MotionNotify || input.type == KeyPress ||
            input.type == KeyRelease) {
         int x=0, y=0, saved_move_sub_obj=move_sub_obj;

         PixelToMeasurementUnit(x_buf, ABS_SIZE(dx));
         PixelToMeasurementUnit(y_buf, ABS_SIZE(dy));
         if (VerboseMeasureTooltip()) {
            sprintf(buf, "dx=%s dy=%s", x_buf, y_buf);
         } else {
            sprintf(buf, "dx=%s\ndy=%s", x_buf, y_buf);
         }
         ShowMeasureCursor(grid_x, grid_y, buf, TRUE);
         if (input.type == KeyPress || input.type == KeyRelease) {
            x = grid_x;
            y = grid_y;
         } else {
            x = input.xmotion.x;
            y = input.xmotion.y;
         }
         if (shiftForDiagMouseMove && DiagEventCheck(&input)) {
            if (useRecentDupDistance && justDupped &&
                  useRecentForDiagMouseMove) {
               DiagGridXY(OrigX-ZOOMED_SIZE(dupDx), OrigY-ZOOMED_SIZE(dupDy),
                     &x, &y);
            } else {
               DiagGridXY(OrigX, OrigY, &x, &y);
            }
         } else if (can_move_sub_obj) {
            MoveSubObjEventCheck(&input, &move_sub_obj);
         }
         GridXY(x, y, &grid_x, &grid_y);

         /* erase */
         DrawInnerRubberObj(ObjPtr, &rubber_info, dx, dy);
         if (!saved_move_sub_obj && !no_no_lock_sel) {
            SelBox(drawWindow, revDefaultGC, sel_ltx+dx, sel_lty+dy, sel_rbx+dx,
                  sel_rby+dy);
         }
         dx = grid_x - OrigX;
         dy = grid_y - OrigY;

         if ((dx != 0 || dy != 0) &&
               (numObjSelected == numObjLocked || ObjPtr->locked)) {
            XUngrabPointer(mainDisplay, CurrentTime);
            XSync(mainDisplay, False);

            EndIntervalRulers(grid_x, grid_y);
            PixelToMeasurementUnit(x_buf, ABS_SIZE(dx));
            PixelToMeasurementUnit(y_buf, ABS_SIZE(dy));
            if (VerboseMeasureTooltip()) {
               sprintf(buf, "dx=%s dy=%s", x_buf, y_buf);
            } else {
               sprintf(buf, "dx=%s\ndy=%s", x_buf, y_buf);
            }
            ShowMeasureCursor(grid_x, grid_y, buf, TRUE);
            EndShowMeasureCursor(grid_x, grid_y, buf, TRUE);
            if (!move_sub_obj && !no_no_lock_sel) {
               SelBox(drawWindow, revDefaultGC, sel_ltx+dx, sel_lty+dy,
                     sel_rbx+dx, sel_rby+dy);
            }
            MsgBox(TgLoadString(STID_LOCKED_OBJS_CANT_BE_MOVED), TOOL_NAME,
                  INFO_MB);
            return;
         }
         PixelToMeasurementUnit(x_buf, ABS_SIZE(dx));
         PixelToMeasurementUnit(y_buf, ABS_SIZE(dy));
         if (VerboseMeasureTooltip()) {
            sprintf(buf, "dx=%s dy=%s", x_buf, y_buf);
         } else {
            sprintf(buf, "dx=%s\ndy=%s", x_buf, y_buf);
         }
         DrawIntervalRulers(ruler_ltx+dx, ruler_lty+dy, ruler_rbx+dx,
               ruler_rby+dy, buf);
         if (!move_sub_obj && !no_no_lock_sel) {
            SelBox(drawWindow, revDefaultGC, sel_ltx+dx, sel_lty+dy, sel_rbx+dx,
                  sel_rby+dy);
         }
         ShowMeasureCursor(grid_x, grid_y, buf, TRUE);

         UpdateInnerRubberObj(ObjPtr, &rubber_info, dx, dy);
         DrawInnerRubberObj(ObjPtr, &rubber_info, dx, dy);

         while (XCheckMaskEvent(mainDisplay, PointerMotionMask, &ev)) ;
      }
   }
   FreeRubberInfo(ObjPtr, &rubber_info);

   if (dx != 0 || dy != 0) {
      if (numObjSelected == numObjLocked || ObjPtr->locked) {
         MsgBox(TgLoadString(STID_LOCKED_OBJS_CANT_BE_MOVED), TOOL_NAME,
               INFO_MB);
         return;
      }
      HighLightReverse();
      dx = ABS_SIZE(dx);
      dy = ABS_SIZE(dy);
      if (numObjSelected == numObjLocked) {
         HighLightForward();
         return;
      }
      if (can_move_sub_obj && move_sub_obj) {
         MoveAnAttr(ObjPtr->detail.t->attr, NULL, dx, dy);
      } else {
         MoveAllSel(dx, dy);
      }
      HighLightForward();
      UpdSelBBox();
      if (justDupped) {
         dupDx += dx;
         dupDy += dy;
      }
      SetFileModified(TRUE);
   }
   if (can_move_sub_obj) {
      ShowCurChoiceMouseStatus(curChoice, 0, FALSE);
   }
}

static
int IsSmoothForStructuredSpline(vlist_index, n, vlist)
   int vlist_index, n;
   IntPoint *vlist;
{
   int i=0, j=0, num_hinge_vs=(n+2)/3;

#ifdef _TGIF_DBG /* debug, do not translate */
   TgAssert((n+2)%3 == 0,
         "invalid n in IsSmoothForStructuredSpline()", NULL);
#endif /* _TGIF_DBG */
   for (i=0, j=0; i < num_hinge_vs; i++, j+=3) {
      if (vlist_index > j+1) continue;
      if (i == 0) {
         if (vlist_index == j) {
            return FALSE;
         }
         return (vlist[0].x != vlist[1].x || vlist[0].y != vlist[1].y);
      } else if (i == num_hinge_vs-1) {
         if (vlist_index == j) {
            return FALSE;
         }
         return (vlist[n-1].x != vlist[n-2].x || vlist[n-1].y != vlist[n-2].y);
      } else {
         if (vlist_index == j) {
            return FALSE;
         }
         return (vlist[j-1].x != vlist[j].x || vlist[j-1].y != vlist[j].y);
      }
   }
   return FALSE;
}

void FinishMoveVertexForStretchStructSpline(vsel_ptr, abs_dx, abs_dy, psssi)
   struct VSelRec *vsel_ptr;
   int abs_dx, abs_dy;
   StretchStructuredSplineInfo *psssi;
{
   struct ObjRec *obj_ptr=NULL;
   struct PolyRec *poly_ptr=NULL;
   struct PolygonRec *polygon_ptr=NULL;
   int was_smooth_point=(!psssi->hinge), vlist_index=0;

   obj_ptr = vsel_ptr->obj;
   switch (obj_ptr->type) {
   case OBJ_POLY:
      poly_ptr = obj_ptr->detail.p;
      break;
   case OBJ_POLYGON:
      polygon_ptr = obj_ptr->detail.g;
      break;
   }
   if (was_smooth_point) {
      vlist_index = GetVlistIndexFromStretchStructuredSplineInfo(psssi,
            vsel_ptr->v_index[0]);
   }
   if (poly_ptr != NULL) {
      UpdateObjForStretchStructSpline(obj_ptr, poly_ptr->n,
            poly_ptr->vlist, abs_dx, abs_dy, psssi);
      if (was_smooth_point && !IsSmoothForStructuredSpline(vlist_index,
            poly_ptr->n, poly_ptr->vlist)) {
         if (!psssi->prev_valid) {
            /* first poly point */
            vsel_ptr->v_index[0] -= 1;
         } else if (!psssi->next_valid) {
            /* last poly point */
            /* merge with next point, so don't need to do anything */
         } else {
            if (!psssi->earlier_smooth_selected) {
               vsel_ptr->v_index[0] -= 2;
            } else {
               /* merge with next point, so don't need to do anything */
            }
         }
         SetIPTInfoForStretchPoly(vsel_ptr->v_index[0], poly_ptr->n,
               poly_ptr->vlist, psssi);
      }
   } else if (polygon_ptr != NULL) {
      UpdateObjForStretchStructSpline(obj_ptr, polygon_ptr->n,
            polygon_ptr->vlist, abs_dx, abs_dy, psssi);
      if (was_smooth_point && !IsSmoothForStructuredSpline(vlist_index,
            polygon_ptr->n, polygon_ptr->vlist)) {
         if (psssi->orig_hinge_index == 0 ||
               psssi->orig_hinge_index == polygon_ptr->n-1) {
            vsel_ptr->v_index[0] = 0;
         } else if (!psssi->earlier_smooth_selected) {
            if (vsel_ptr->v_index[0] == 1) {
               vsel_ptr->v_index[0] -= 1;
            } else {
               vsel_ptr->v_index[0] -= 2;
            }
         }
         SetIPTInfoForStretchPolygon(vsel_ptr->v_index[0], polygon_ptr->n,
               polygon_ptr->vlist, psssi);
      }
   }
}

void MoveAllSelVs(abs_dx, abs_dy)
   int abs_dx, abs_dy;
{
   int i;
   IntPoint *v=NULL;
   struct ObjRec *obj_ptr=NULL;
   struct VSelRec *vsel_ptr=NULL;
   int n=0, ltx=selLtX, lty=selLtY, rbx=selRbX, rby=selRbY;

   StartCompositeCmd();
   for (vsel_ptr=botVSel; vsel_ptr != NULL; vsel_ptr=vsel_ptr->prev) {
      int auto_retracted_arrow=FALSE, curved=(-1);
      struct PolyRec *poly_ptr=NULL;
      struct PolygonRec *polygon_ptr=NULL;
      StretchStructuredSplineInfo *psssi=NULL;

      obj_ptr = vsel_ptr->obj;
      switch (obj_ptr->type) {
      case OBJ_POLY:
         poly_ptr = obj_ptr->detail.p;
         curved = poly_ptr->curved;
         if (curved == LT_STRUCT_SPLINE) {
            v = poly_ptr->ssvlist;
            n = poly_ptr->ssn;
            psssi = (StretchStructuredSplineInfo*)(obj_ptr->userdata);
            if (psssi == NULL) {
               psssi = (StretchStructuredSplineInfo*)malloc(
                     sizeof(StretchStructuredSplineInfo));
               if (psssi == NULL) FailAllocMessage();
               memset(psssi, 0, sizeof(StretchStructuredSplineInfo));
               obj_ptr->userdata = vsel_ptr->obj->userdata = psssi;
               SetIPTInfoForStretchPoly(vsel_ptr->v_index[0], poly_ptr->n,
                     poly_ptr->vlist, psssi);
            }
         } else {
            v = poly_ptr->vlist;
            n = poly_ptr->n;
         }
         auto_retracted_arrow = AutoRetractedArrowAttr(obj_ptr, TRUE);
         break;
      case OBJ_POLYGON:
         polygon_ptr = obj_ptr->detail.g;
         curved = polygon_ptr->curved;
         if (curved == LT_STRUCT_SPLINE) {
            v = polygon_ptr->ssvlist;
            n = polygon_ptr->ssn;
            psssi = (StretchStructuredSplineInfo*)(obj_ptr->userdata);
            if (psssi == NULL) {
               psssi = (StretchStructuredSplineInfo*)malloc(
                     sizeof(StretchStructuredSplineInfo));
               if (psssi == NULL) FailAllocMessage();
               memset(psssi, 0, sizeof(StretchStructuredSplineInfo));
               obj_ptr->userdata = vsel_ptr->obj->userdata = psssi;
               SetIPTInfoForStretchPolygon(vsel_ptr->v_index[0], polygon_ptr->n,
                     polygon_ptr->vlist, psssi);
            }
         } else {
            v = polygon_ptr->vlist;
            n = polygon_ptr->n;
         }
         break;
      }
      PrepareToReplaceAnObj(obj_ptr);
      if (obj_ptr->ctm == NULL) {
         for (i=0; i < vsel_ptr->n; i++) {
            vsel_ptr->x[i] += abs_dx;
            vsel_ptr->y[i] += abs_dy;
            v[vsel_ptr->v_index[i]].x += abs_dx;
            v[vsel_ptr->v_index[i]].y += abs_dy;
         }
      } else {
         for (i=0; i < vsel_ptr->n; i++) {
            int x=0, y=0, x2=0, y2=0;

            /*
             * vsel_ptr->x[i] += abs_dx;
             * vsel_ptr->y[i] += abs_dy;
             */
            ReverseTransformPointThroughCTM(vsel_ptr->x[i]+abs_dx-obj_ptr->x,
                  vsel_ptr->y[i]+abs_dy-obj_ptr->y, obj_ptr->ctm, &x, &y);
            v[vsel_ptr->v_index[i]].x = x + obj_ptr->x;
            v[vsel_ptr->v_index[i]].y = y + obj_ptr->y;
            TransformPointThroughCTM(x, y, obj_ptr->ctm, &x2, &y2);
            vsel_ptr->x[i] = x2 + obj_ptr->x;
            vsel_ptr->y[i] = y2 + obj_ptr->y;
         }
      }
      if (curved == LT_STRUCT_SPLINE) {
#ifdef _TGIF_DBG /* debug, do not translate */
         TgAssert(obj_ptr->userdata != NULL,
               "obj_ptr is NULL in MoveAllSelVs()", NULL);
#endif /* _TGIF_DBG */
         FinishMoveVertexForStretchStructSpline(vsel_ptr, abs_dx, abs_dy,
               psssi);
         free(psssi);
         obj_ptr->userdata = NULL;
      }
      AdjObjSplineVs(obj_ptr);
      if (auto_retracted_arrow) {
         for (i=0; i < vsel_ptr->n; i++) {
            if (vsel_ptr->v_index[i] == 1) {
               vsel_ptr->x[i] = v[1].x;
               vsel_ptr->y[i] = v[1].y;
            }
         }
      }
      switch (obj_ptr->type) {
      case OBJ_POLY:
         if (obj_ptr->detail.p->curved != LT_INTSPLINE) {
            UpdPolyBBox(obj_ptr, n, v);
         } else {
            UpdPolyBBox(obj_ptr, obj_ptr->detail.p->intn,
                  obj_ptr->detail.p->intvlist);
         }
         break;
      case OBJ_POLYGON:
         if (obj_ptr->detail.g->curved != LT_INTSPLINE) {
            UpdPolyBBox(obj_ptr, n, v);
         } else {
            UpdPolyBBox(obj_ptr, obj_ptr->detail.g->intn,
                  obj_ptr->detail.g->intvlist);
         }
         break;
      }
      RecordReplaceAnObj(obj_ptr);
   }
   EndCompositeCmd();
   UpdSelBBox();
   RedrawAreas(botObj, ltx-GRID_ABS_SIZE(1), lty-GRID_ABS_SIZE(1),
         rbx+GRID_ABS_SIZE(1), rby+GRID_ABS_SIZE(1),
         selLtX-GRID_ABS_SIZE(1), selLtY-GRID_ABS_SIZE(1),
         selRbX+GRID_ABS_SIZE(1), selRbY+GRID_ABS_SIZE(1));
}

static struct ObjRec *tmpTopObj=NULL, *tmpBotObj=NULL;

#define FORWARD 0
#define REVERSE 1

static
void EndMoveVsForStructuredSpline(obj_ptr, abs_dx, abs_dy)
   struct ObjRec *obj_ptr;
   int abs_dx, abs_dy;
{
   StretchStructuredSplineInfo *psssi=
         (StretchStructuredSplineInfo*)(obj_ptr->userdata);

   if (psssi != NULL) {
      if (psssi->sv != NULL) {
         XDrawLines(mainDisplay, drawWindow, revDefaultGC,
               psssi->sv, psssi->sn, CoordModeOrigin);
         free(psssi->sv);
         psssi->sv = NULL;
      }
      if (psssi->sv2 != NULL) {
         XDrawLines(mainDisplay, drawWindow, revDefaultGC,
               psssi->sv2, psssi->sn2, CoordModeOrigin);
         free(psssi->sv2);
         psssi->sv2 = NULL;
      }
      EraseHighLightForStretchStructSpline(psssi, abs_dx, abs_dy, FALSE, TRUE);
   }
}

static
void EndMoveVs(abs_dx, abs_dy)
   int abs_dx, abs_dy;
{
   int i=0;
   struct ObjRec *obj_ptr=NULL, *next_obj=NULL;
   struct PolyRec *poly_ptr=NULL;
   struct PolygonRec *polygon_ptr=NULL;

   for (obj_ptr=tmpTopObj; obj_ptr != NULL; obj_ptr = next_obj) {
      next_obj = obj_ptr->next;
      switch (obj_ptr->type) {
      case OBJ_POLY:
         poly_ptr = obj_ptr->detail.p;
         if (poly_ptr->vlist != NULL) {
            free(poly_ptr->vlist);
            poly_ptr->vlist = NULL;
         }
         if (poly_ptr->svlist != NULL) {
            free(poly_ptr->svlist);
            poly_ptr->svlist = NULL;
         }
         if (poly_ptr->curved == LT_INTSPLINE &&
               poly_ptr->intvlist != NULL) {
            free(poly_ptr->intvlist);
            poly_ptr->intvlist = NULL;
         } else if (poly_ptr->curved == LT_STRUCT_SPLINE) {
            EndMoveVsForStructuredSpline(obj_ptr, 0, 0);
            for (i=0; i < poly_ptr->ssn; i++) {
               if (poly_ptr->ssmooth[i]) {
                  MARKHO(drawWindow, revDefaultGC,
                        OFFSET_X(poly_ptr->ssvlist[i].x),
                        OFFSET_Y(poly_ptr->ssvlist[i].y));
               } else {
                  MARKHR(drawWindow, revDefaultGC,
                        OFFSET_X(poly_ptr->ssvlist[i].x),
                        OFFSET_Y(poly_ptr->ssvlist[i].y));
               }
            }
         }
         free(poly_ptr);
         break;
      case OBJ_POLYGON:
         polygon_ptr = obj_ptr->detail.g;
         if (polygon_ptr->vlist != NULL) {
            free(polygon_ptr->vlist);
            polygon_ptr->vlist = NULL;
         }
         if (polygon_ptr->svlist != NULL) {
            free(polygon_ptr->svlist);
            polygon_ptr->svlist = NULL;
         }
         if (polygon_ptr->curved == LT_INTSPLINE &&
               polygon_ptr->intvlist != NULL) {
            free(polygon_ptr->intvlist);
            polygon_ptr->intvlist = NULL;
         } else if (polygon_ptr->curved == LT_STRUCT_SPLINE) {
            EndMoveVsForStructuredSpline(obj_ptr, 0, 0);
            for (i=0; i < polygon_ptr->ssn; i++) {
               if (polygon_ptr->ssmooth[i]) {
                  MARKHO(drawWindow, revDefaultGC,
                        OFFSET_X(polygon_ptr->ssvlist[i].x),
                        OFFSET_Y(polygon_ptr->ssvlist[i].y));
               } else {
                  MARKHR(drawWindow, revDefaultGC,
                        OFFSET_X(polygon_ptr->ssvlist[i].x),
                        OFFSET_Y(polygon_ptr->ssvlist[i].y));
               }
            }
         }
         free(polygon_ptr);
         break;
      }
      free(obj_ptr);
   }
}

static
void PrepareToMoveVs()
{
   struct VSelRec *vsel_ptr=NULL;

   tmpTopObj = tmpBotObj = NULL;
   for (vsel_ptr=botVSel; vsel_ptr != NULL; vsel_ptr=vsel_ptr->prev) {
      struct ObjRec *obj_ptr=NULL;
      struct PolyRec *poly_ptr=NULL, *poly_copy=NULL;
      struct PolygonRec *polygon_ptr=NULL, *polygon_copy=NULL;
      XPoint *sv=NULL;
      IntPoint *polyv=NULL, *polyssv=NULL, *cntrlv=NULL;
      int i=0, num_pts=0, sn=0, curved=(-1), intn=0, ssn=0;
      char *smooth=NULL, *ssmooth=NULL;
      StretchStructuredSplineInfo *psssi=NULL;

      obj_ptr = (struct ObjRec *)malloc(sizeof(struct ObjRec));
      if (obj_ptr == NULL) FailAllocMessage();
      memset(obj_ptr, 0, sizeof(struct ObjRec));
      obj_ptr->prev = NULL;
      obj_ptr->next = tmpTopObj;
      obj_ptr->ctm = NULL;
      if (tmpTopObj == NULL) {
         tmpBotObj = obj_ptr;
      } else {
         tmpTopObj->prev = obj_ptr;
      }
      tmpTopObj = obj_ptr;
      obj_ptr->type = vsel_ptr->obj->type;

      switch (vsel_ptr->obj->type) {
      case OBJ_POLY:
         poly_copy = (struct PolyRec *)malloc(sizeof(struct PolyRec));
         if (poly_copy == NULL) FailAllocMessage();
         memset(poly_copy, 0, sizeof(struct PolyRec));
         obj_ptr->detail.p = poly_copy;

         poly_ptr = vsel_ptr->obj->detail.p;
         curved = poly_copy->curved = poly_ptr->curved;
         num_pts = poly_copy->n = poly_ptr->n;
         if (curved == LT_STRUCT_SPLINE) {
            ssn = poly_copy->ssn = poly_ptr->ssn;
         }
         polyv = (IntPoint*)malloc((num_pts+1)*sizeof(IntPoint));
         if (polyv == NULL) FailAllocMessage();
         if (curved == LT_STRUCT_SPLINE) {
            polyssv = (IntPoint*)malloc((ssn+2)*sizeof(IntPoint));
            if (polyssv == NULL) FailAllocMessage();
         }
         if (curved == LT_STRUCT_SPLINE || ((curved == LT_STRAIGHT ||
               curved == LT_SPLINE) && poly_ptr->smooth != NULL)) {
            if (poly_ptr->smooth != NULL) {
               smooth = (char*)malloc((num_pts+1)*sizeof(char));
               if (smooth == NULL) FailAllocMessage();
            }
            if (curved == LT_STRUCT_SPLINE) {
               ssmooth = (char*)malloc((ssn+2)*sizeof(char));
               if (ssmooth == NULL) FailAllocMessage();
            }
         }
         if (vsel_ptr->obj->ctm == NULL) {
            for (i=0; i < num_pts; i++) {
               polyv[i].x = poly_ptr->vlist[i].x;
               polyv[i].y = poly_ptr->vlist[i].y;
               if (smooth != NULL) smooth[i] = poly_ptr->smooth[i];
            }
            if (curved == LT_STRUCT_SPLINE) {
               for (i=0; i < ssn; i++) {
                  polyssv[i].x = poly_ptr->ssvlist[i].x;
                  polyssv[i].y = poly_ptr->ssvlist[i].y;
                  if (ssmooth != NULL) ssmooth[i] = poly_ptr->ssmooth[i];
               }
            }
         } else {
            int x, y;

            for (i=0; i < num_pts; i++) {
               TransformPointThroughCTM(
                     poly_ptr->vlist[i].x-vsel_ptr->obj->x,
                     poly_ptr->vlist[i].y-vsel_ptr->obj->y,
                     vsel_ptr->obj->ctm, &x, &y);
               polyv[i].x = x+vsel_ptr->obj->x;
               polyv[i].y = y+vsel_ptr->obj->y;
               if (smooth != NULL) smooth[i] = poly_ptr->smooth[i];
            }
            if (curved == LT_STRUCT_SPLINE) {
               for (i=0; i < ssn; i++) {
                  TransformPointThroughCTM(
                        poly_ptr->ssvlist[i].x-vsel_ptr->obj->x,
                        poly_ptr->ssvlist[i].y-vsel_ptr->obj->y,
                        vsel_ptr->obj->ctm, &x, &y);
                  polyssv[i].x = x+vsel_ptr->obj->x;
                  polyssv[i].y = y+vsel_ptr->obj->y;
                  if (ssmooth != NULL) ssmooth[i] = poly_ptr->ssmooth[i];
               }
            }
         }
         if (curved != LT_INTSPLINE) {
            if (curved == LT_STRUCT_SPLINE) {
               sv = MakeMultiSplinePolyVertex(curved, &sn, ssmooth,
                     drawOrigX, drawOrigY, ssn, polyssv);
               poly_copy->ssvlist = polyssv;
               poly_copy->ssmooth = ssmooth;
               poly_copy->ssn = ssn;
            } else {
               sv = MakeMultiSplinePolyVertex(curved, &sn, smooth,
                     drawOrigX, drawOrigY, num_pts, polyv);
            }
         } else {
            sv = MakeIntSplinePolyVertex(&sn, &intn, &cntrlv,
                  drawOrigX, drawOrigY, num_pts, polyv);
         }
         poly_copy->vlist = polyv;
         poly_copy->smooth = smooth;
         poly_copy->svlist = sv;
         poly_copy->sn = sn;

         if (curved == LT_STRUCT_SPLINE) {
            for (i=0; i < ssn; i++) {
               if (ssmooth[i]) {
                  MARKHO(drawWindow, revDefaultGC, OFFSET_X(polyssv[i].x),
                        OFFSET_Y(polyssv[i].y));
               } else {
                  MARKHR(drawWindow, revDefaultGC, OFFSET_X(polyssv[i].x),
                        OFFSET_Y(polyssv[i].y));
               }
            }
         }
         break;
      case OBJ_POLYGON:
         polygon_copy = (struct PolygonRec *)malloc(sizeof(struct PolygonRec));
         if (polygon_copy == NULL) FailAllocMessage();
         memset(polygon_copy, 0, sizeof(struct PolygonRec));
         obj_ptr->detail.g = polygon_copy;

         polygon_ptr = vsel_ptr->obj->detail.g;
         curved = polygon_copy->curved = polygon_ptr->curved;
         num_pts = polygon_copy->n = polygon_ptr->n;
         if (curved == LT_STRUCT_SPLINE) {
            ssn = polygon_copy->ssn = polygon_ptr->ssn;
         }
         polyv = (IntPoint*)malloc((num_pts+1)*sizeof(IntPoint));
         if (polyv == NULL) FailAllocMessage();
         if (curved == LT_STRUCT_SPLINE) {
            polyssv = (IntPoint*)malloc((ssn+2)*sizeof(IntPoint));
            if (polyssv == NULL) FailAllocMessage();
         }
         if (curved == LT_STRUCT_SPLINE || ((curved == LT_STRAIGHT ||
               curved == LT_SPLINE) && polygon_ptr->smooth != NULL)) {
            if (polygon_ptr->smooth != NULL) {
               smooth = (char*)malloc((num_pts+1)*sizeof(char));
               if (smooth == NULL) FailAllocMessage();
            }
            if (curved == LT_STRUCT_SPLINE) {
               ssmooth = (char*)malloc((ssn+1)*sizeof(char));
               if (ssmooth == NULL) FailAllocMessage();
            }
         }
         if (vsel_ptr->obj->ctm == NULL) {
            for (i=0; i < num_pts; i++) {
               polyv[i].x = polygon_ptr->vlist[i].x;
               polyv[i].y = polygon_ptr->vlist[i].y;
               if (smooth != NULL) smooth[i] = polygon_ptr->smooth[i];
            }
            if (curved == LT_STRUCT_SPLINE) {
               for (i=0; i < ssn; i++) {
                  polyssv[i].x = polygon_ptr->ssvlist[i].x;
                  polyssv[i].y = polygon_ptr->ssvlist[i].y;
                  if (ssmooth != NULL) ssmooth[i] = polygon_ptr->ssmooth[i];
               }
            }
         } else {
            int x, y;

            for (i=0; i < num_pts; i++) {
               TransformPointThroughCTM(
                     polygon_ptr->vlist[i].x-vsel_ptr->obj->x,
                     polygon_ptr->vlist[i].y-vsel_ptr->obj->y,
                     vsel_ptr->obj->ctm, &x, &y);
               polyv[i].x = x+vsel_ptr->obj->x;
               polyv[i].y = y+vsel_ptr->obj->y;
               if (smooth != NULL) smooth[i] = polygon_ptr->smooth[i];
            }
            if (curved == LT_STRUCT_SPLINE) {
               for (i=0; i < num_pts; i++) {
                  TransformPointThroughCTM(
                        polygon_ptr->ssvlist[i].x-vsel_ptr->obj->x,
                        polygon_ptr->ssvlist[i].y-vsel_ptr->obj->y,
                        vsel_ptr->obj->ctm, &x, &y);
                  polyssv[i].x = x+vsel_ptr->obj->x;
                  polyssv[i].y = y+vsel_ptr->obj->y;
                  if (ssmooth != NULL) ssmooth[i] = polygon_ptr->ssmooth[i];
               }
            }
         }
         if (curved != LT_INTSPLINE) {
            if (curved == LT_STRUCT_SPLINE) {
               sv = MakeMultiSplinePolygonVertex(curved, &sn, ssmooth,
                     drawOrigX, drawOrigY, ssn, polyssv);
               polygon_copy->ssvlist = polyssv;
               polygon_copy->ssmooth = ssmooth;
               polygon_copy->ssn = ssn;
            } else {
               sv = MakeMultiSplinePolygonVertex(curved, &sn, smooth,
                     drawOrigX, drawOrigY, num_pts,polyv);
            }
         } else {
            sv = MakeIntSplinePolygonVertex(&sn, &intn, &cntrlv,
                  drawOrigX, drawOrigY, num_pts,polyv);
         }
         polygon_copy->vlist = polyv;
         polygon_copy->smooth = smooth;
         polygon_copy->svlist = sv;
         polygon_copy->sn = sn;

         if (curved == LT_STRUCT_SPLINE) {
            for (i=0; i < ssn; i++) {
               if (ssmooth[i]) {
                  MARKHO(drawWindow, revDefaultGC, OFFSET_X(polyssv[i].x),
                        OFFSET_Y(polyssv[i].y));
               } else {
                  MARKHR(drawWindow, revDefaultGC, OFFSET_X(polyssv[i].x),
                        OFFSET_Y(polyssv[i].y));
               }
            }
         }
         break;
      }
      if (curved == LT_STRUCT_SPLINE) {
         psssi = (StretchStructuredSplineInfo*)malloc(
               sizeof(StretchStructuredSplineInfo));
         if (psssi == NULL) FailAllocMessage();
         memset(psssi, 0, sizeof(StretchStructuredSplineInfo));
#ifdef _TGIF_DBG /* debug, do not translate */
         TgAssert(obj_ptr->userdata == NULL,
               "obj_ptr is not NULL in PrepareToMoveVs()", NULL);
#endif /* _TGIF_DBG */
         obj_ptr->userdata = vsel_ptr->obj->userdata = psssi;
         if (poly_ptr != NULL) {
            SetIPTInfoForStretchPoly(vsel_ptr->v_index[0], num_pts,
                  polyv, psssi);
         } else if (polygon_ptr != NULL) {
            SetIPTInfoForStretchPolygon(vsel_ptr->v_index[0], num_pts,
                  polyv, psssi);
         }
         SetVsAndVs2ForStretchStructSpline(psssi, 0, 0, &psssi->num_vs,
               psssi->vs, &psssi->num_vs2, psssi->vs2);
         FixUpSmoothAndSmooth2ForStretchStructSpline(psssi->num_vs,
               psssi->smooth, psssi->num_vs2, psssi->smooth2);
         if (psssi->prev_valid) {
            psssi->sv = MakeMultiSplinePolyVertex(LT_STRUCT_SPLINE,
                  &psssi->sn, psssi->smooth, drawOrigX, drawOrigY,
                  psssi->num_vs, psssi->vs);
            psssi->saved_sv = DupVs(&psssi->saved_sn, psssi->sv, psssi->sn);
         }
         if (psssi->next_valid) {
            psssi->sv2 = MakeMultiSplinePolyVertex(LT_STRUCT_SPLINE,
                  &psssi->sn2, psssi->smooth2, drawOrigX, drawOrigY,
                  psssi->num_vs2, psssi->vs2);
            psssi->saved_sv2 = DupVs(&psssi->saved_sn2, psssi->sv2, psssi->sn2);
         }
         EraseHighLightForStretchStructSpline(psssi, 0, 0, FALSE, TRUE);
      }
   }
}

static
void MarkVsForStructuredSplines(Dir, abs_dx, abs_dy)
   int Dir, abs_dx, abs_dy;
{
   struct ObjRec *obj_ptr=NULL;

   switch (Dir) {
   case FORWARD: obj_ptr = tmpBotObj; break;
   case REVERSE: obj_ptr = tmpTopObj; break;
   }
   while (obj_ptr != NULL) {
      StretchStructuredSplineInfo *psssi=
            (StretchStructuredSplineInfo*)(obj_ptr->userdata);

      if (psssi != NULL) {
         switch (Dir) {
         case REVERSE:
            /* erase */
            if (psssi->sv != NULL) {
               XDrawLines(mainDisplay, drawWindow, revDefaultGC,
                     psssi->sv, psssi->sn, CoordModeOrigin);
               free(psssi->sv);
               psssi->sv = NULL;
            }
            if (psssi->sv2 != NULL) {
               XDrawLines(mainDisplay, drawWindow, revDefaultGC,
                     psssi->sv2, psssi->sn2, CoordModeOrigin);
               free(psssi->sv2);
               psssi->sv2 = NULL;
            }
            EraseHighLightForStretchStructSpline(psssi, abs_dx, abs_dy, TRUE,
                  TRUE);
            break;
         case FORWARD:
            /* draw */
            SetVsAndVs2ForStretchStructSpline(psssi, abs_dx, abs_dy,
                  &psssi->num_vs, psssi->vs, &psssi->num_vs2, psssi->vs2);
            FixUpSmoothAndSmooth2ForStretchStructSpline(psssi->num_vs,
                  psssi->smooth, psssi->num_vs2,
                  psssi->smooth2);
            if (psssi->prev_valid) {
               psssi->sv = MakeMultiSplinePolyVertex(LT_STRUCT_SPLINE,
                     &psssi->sn, psssi->smooth, drawOrigX, drawOrigY,
                     psssi->num_vs, psssi->vs);
               XDrawLines(mainDisplay, drawWindow, revDefaultGC,
                     psssi->sv, psssi->sn, CoordModeOrigin);
            }
            if (psssi->next_valid) {
               psssi->sv2 = MakeMultiSplinePolyVertex(LT_STRUCT_SPLINE,
                     &psssi->sn2, psssi->smooth2, drawOrigX, drawOrigY,
                     psssi->num_vs2, psssi->vs2);
               XDrawLines(mainDisplay, drawWindow, revDefaultGC,
                     psssi->sv2, psssi->sn2, CoordModeOrigin);
            }
            EraseHighLightForStretchStructSpline(psssi, abs_dx, abs_dy, TRUE,
                  TRUE);
            break;
         }
         if (psssi->hinge) {
            MARKHR(drawWindow, revDefaultGC,
                  OFFSET_X(psssi->ipt.hinge_pt.x+abs_dx),
                  OFFSET_Y(psssi->ipt.hinge_pt.y+abs_dy));
         } else if (psssi->earlier_smooth_selected) {
            MARKHO(drawWindow, revDefaultGC,
                  OFFSET_X(psssi->ipt.earlier_smooth_pt.x+abs_dx),
                  OFFSET_Y(psssi->ipt.earlier_smooth_pt.y+abs_dy));
         } else {
            MARKHO(drawWindow, revDefaultGC,
                  OFFSET_X(psssi->ipt.later_smooth_pt.x+abs_dx),
                  OFFSET_Y(psssi->ipt.later_smooth_pt.y+abs_dy));
         }
      }
      switch (Dir) {
      case FORWARD: obj_ptr = obj_ptr->prev; break;
      case REVERSE: obj_ptr = obj_ptr->next; break;
      }
   }
}

static
void HighLightVs(Dir, abs_dx, abs_dy)
   int Dir, abs_dx, abs_dy;
{
   int i=0, n=0, curved=(-1);
   struct ObjRec *obj_ptr=NULL;
   IntPoint *v=NULL;
   char *smooth=NULL;

   switch (Dir) {
   case FORWARD: obj_ptr = tmpBotObj; break;
   case REVERSE: obj_ptr = tmpTopObj; break;
   }
   while (obj_ptr != NULL) {
      switch (obj_ptr->type) {
      case OBJ_POLY:
         curved = obj_ptr->detail.p->curved;
         if (curved == LT_STRUCT_SPLINE) {
            n = obj_ptr->detail.p->ssn;
            v = obj_ptr->detail.p->ssvlist;
            smooth = obj_ptr->detail.p->ssmooth;
         } else {
            n = obj_ptr->detail.p->n;
            v = obj_ptr->detail.p->vlist;
            smooth = obj_ptr->detail.p->smooth;
         }
         if (curved != LT_INTSPLINE && smooth != NULL) {
            if (curved == LT_STRUCT_SPLINE) {
               /* nothing to do here */
            } else {
               for (i=0; i < n; i++) {
                  if (smooth[i]) {
                     MARKO(drawWindow, revDefaultGC, OFFSET_X(v[i].x),
                           OFFSET_Y(v[i].y));
                  } else {
                     MARK(drawWindow, revDefaultGC, OFFSET_X(v[i].x),
                           OFFSET_Y(v[i].y));
                  }
               }
            }
         } else {
            for (i=0; i < n; i++) {
               MARK(drawWindow, revDefaultGC, OFFSET_X(v[i].x),
                     OFFSET_Y(v[i].y));
            }
         }
         if (curved == LT_STRUCT_SPLINE) {
            /* nothing to do here */
         } else {
            XDrawLines(mainDisplay, drawWindow, revDefaultGC,
                  obj_ptr->detail.p->svlist, obj_ptr->detail.p->sn,
                  CoordModeOrigin);
         }
         break;
      case OBJ_POLYGON:
         curved = obj_ptr->detail.g->curved;
         if (curved == LT_STRUCT_SPLINE) {
            n = obj_ptr->detail.g->ssn;
            v = obj_ptr->detail.g->ssvlist;
            smooth = obj_ptr->detail.g->ssmooth;
         } else {
            n = obj_ptr->detail.g->n;
            v = obj_ptr->detail.g->vlist;
            smooth = obj_ptr->detail.g->smooth;
         }
         if (obj_ptr->detail.g->curved != LT_INTSPLINE && smooth != NULL) {
            if (curved == LT_STRUCT_SPLINE) {
               /* nothing to do */
            } else {
               for (i=0; i < obj_ptr->detail.g->n-1; i++) {
                  if (smooth[i]) {
                     MARKO(drawWindow, revDefaultGC, OFFSET_X(v[i].x),
                           OFFSET_Y(v[i].y));
                  } else {
                     MARK(drawWindow, revDefaultGC, OFFSET_X(v[i].x),
                           OFFSET_Y(v[i].y));
                  }
               }
            }
         } else {
            for (i=0; i < obj_ptr->detail.g->n-1; i++) {
               MARK(drawWindow, revDefaultGC, OFFSET_X(v[i].x),
                     OFFSET_Y(v[i].y));
            }
         }
         if (curved == LT_STRUCT_SPLINE) {
            /* nothing to do here */
         } else {
            XDrawLines(mainDisplay, drawWindow, revDefaultGC,
                  obj_ptr->detail.g->svlist, obj_ptr->detail.g->sn,
                  CoordModeOrigin);
         }
         break;
      }
      switch (Dir) {
      case FORWARD: obj_ptr = obj_ptr->prev; break;
      case REVERSE: obj_ptr = obj_ptr->next; break;
      }
   }
}

static
void MarkVs(Dir, Dx, Dy)
   int Dir, Dx, Dy; /* Dx and Dy are screen offsets */
{
   register int i, x, y;
   register struct VSelRec *vsel_ptr=NULL;

   switch (Dir) {
   case FORWARD: vsel_ptr = botVSel; break;
   case REVERSE: vsel_ptr = topVSel; break;
   }
   while (vsel_ptr != NULL) {
      char *smooth=NULL;
      int curved=(-1);

      switch (vsel_ptr->obj->type) {
      case OBJ_POLY:
         smooth = vsel_ptr->obj->detail.p->smooth;
         curved = vsel_ptr->obj->detail.p->curved;
         break;
      case OBJ_POLYGON:
         smooth = vsel_ptr->obj->detail.g->smooth;
         curved = vsel_ptr->obj->detail.g->curved;
         break;
      }
      for (i=0; i < vsel_ptr->n; i++) {
         if (!(vsel_ptr->obj->type==OBJ_POLYGON &&
               vsel_ptr->obj->detail.g->n-1==vsel_ptr->v_index[i])) {
            x = OFFSET_X(vsel_ptr->x[i])+Dx;
            y = OFFSET_Y(vsel_ptr->y[i])+Dy;
            if (curved == LT_STRUCT_SPLINE || ((curved == LT_STRAIGHT ||
                  curved == LT_SPLINE) && smooth != NULL)) {
               if (curved == LT_STRUCT_SPLINE) {
                  /* nothing to do here */
               } else {
                  if (smooth[vsel_ptr->v_index[i]]) {
                     if (curved == LT_STRUCT_SPLINE) {
                        MARKHO(drawWindow, revDefaultGC, x, y);
                     } else {
                        MARKO(drawWindow, revDefaultGC, x, y);
                     }
                  } else {
                     if (curved == LT_STRUCT_SPLINE) {
                        MARKHR(drawWindow, revDefaultGC, x, y);
                     } else {
                        MARK(drawWindow, revDefaultGC, x, y);
                     }
                  }
               }
            } else {
               MARK(drawWindow, revDefaultGC, x, y);
            }
            MARKV(drawWindow, revDefaultGC, x, y);
         }
      }
      switch (Dir) {
      case FORWARD: vsel_ptr = vsel_ptr->prev; break;
      case REVERSE: vsel_ptr = vsel_ptr->next; break;
      }
   }
}

static
void GetSelectedVsBBox(pBBox)
   struct BBRec *pBBox;
{
   int found=FALSE, ltx=0, lty=0, rbx=0, rby=0;
   struct VSelRec *vsel_ptr=NULL;

   vsel_ptr = botVSel;
   while (vsel_ptr != NULL) {
      int i=0;

      for (i=0; i < vsel_ptr->n; i++) {
         if (!(vsel_ptr->obj->type==OBJ_POLYGON &&
               vsel_ptr->obj->detail.g->n-1==vsel_ptr->v_index[i])) {
            int x=OFFSET_X(vsel_ptr->x[i]);
            int y=OFFSET_Y(vsel_ptr->y[i]);

            if (found) {
               if (x < ltx) ltx = x;
               if (x > rbx) rbx = x;
               if (y < lty) lty = y;
               if (y > rby) rby = y;
            } else {
               found = TRUE;
               ltx = rbx = x;
               lty = rby = y;
            }
         }
      }
      vsel_ptr = vsel_ptr->prev;
   }
   if (found) {
      pBBox->ltx = ltx; pBBox->lty = lty; pBBox->rbx = rbx; pBBox->rby = rby;
   }
}

void MoveSelVs(OrigX, OrigY)
   int OrigX, OrigY;
{
   int x=0, y=0, i=0;
   struct ObjRec *obj_ptr=NULL;
   struct VSelRec *vsel_ptr=NULL;
   struct PolyRec *poly_ptr=NULL;
   struct PolygonRec *polygon_ptr=NULL;
   IntPoint *pv=NULL;
   int moving=TRUE, dx=0, dy=0, num_pts=0, curved=FALSE;
   int grid_x=OrigX, grid_y=OrigY, abs_dx_from_orig=0, abs_dy_from_orig=0;
   int saved_grid_x=OrigX, saved_grid_y=OrigY;
   struct BBRec ruler_bbox;
   char buf[80], x_buf[80], y_buf[80];
   XEvent ev;

   XFlush(mainDisplay);
   XSync(mainDisplay, False);

   if (XCheckMaskEvent(mainDisplay, ExposureMask, &ev) ||
         XCheckMaskEvent(mainDisplay, VisibilityChangeMask, &ev)) {
      ExposeEventHandler(&ev, TRUE);
   }
   HighLightReverse();

   PrepareToMoveVs();
   MarkVsForStructuredSplines(FORWARD, 0, 0);
   HighLightVs(FORWARD, 0, 0);
   MarkVs(FORWARD, 0, 0);

   memset(&ruler_bbox, 0, sizeof(struct BBRec));
   GetSelectedVsBBox(&ruler_bbox);

   PixelToMeasurementUnit(x_buf, 0);
   PixelToMeasurementUnit(y_buf, 0);
   sprintf(buf, "dx=%s\ndy=%s", x_buf, y_buf);
   if (VerboseMeasureTooltip()) {
      sprintf(buf, "dx=%s dy=%s", x_buf, y_buf);
   } else {
      sprintf(buf, "dx=%s\ndy=%s", x_buf, y_buf);
   }
   StartShowMeasureCursor(grid_x, grid_y, buf, TRUE);
   BeginIntervalRulers(ruler_bbox.ltx, ruler_bbox.lty, ruler_bbox.rbx,
         ruler_bbox.rby);

   if (!debugNoPointerGrab) {
      XGrabPointer(mainDisplay, drawWindow, FALSE,
            PointerMotionMask | ButtonReleaseMask,
            GrabModeAsync, GrabModeAsync, None, moveCursor, CurrentTime);
   }
   while (moving) {
      XEvent input;

      XNextEvent(mainDisplay, &input);

      if (input.type == Expose || input.type == VisibilityNotify) {
         ExposeEventHandler(&input, TRUE);
      } else if (input.type == ButtonRelease) {
         XUngrabPointer(mainDisplay, CurrentTime);
         XSync(mainDisplay, False);

         EndIntervalRulers(grid_x, grid_y);
         PixelToMeasurementUnit(x_buf, ABS_SIZE(grid_x-OrigX));
         PixelToMeasurementUnit(y_buf, ABS_SIZE(grid_y-OrigY));
         if (VerboseMeasureTooltip()) {
            sprintf(buf, "dx=%s dy=%s", x_buf, y_buf);
         } else {
            sprintf(buf, "dx=%s\ndy=%s", x_buf, y_buf);
         }
         EndShowMeasureCursor(grid_x, grid_y, buf, TRUE);
         MarkRulers(grid_x, grid_y);
         moving = FALSE;

         dx = grid_x - OrigX;
         dy = grid_y - OrigY;

         MarkVs(REVERSE, dx, dy);
         MarkVsForStructuredSplines(REVERSE, ABS_SIZE(dx), ABS_SIZE(dy));
         HighLightVs(REVERSE, ABS_SIZE(dx), ABS_SIZE(dy));
         EndMoveVs(ABS_SIZE(dx), ABS_SIZE(dy));

         if (dx != 0 || dy != 0) {
            MoveAllSelVs(ABS_SIZE(dx), ABS_SIZE(dy));
            HighLightForward();
            SetFileModified(TRUE);
         } else {
            HighLightForward();
         }
      } else if (input.type == MotionNotify || input.type == KeyPress ||
            input.type == KeyRelease) {
         PixelToMeasurementUnit(x_buf, ABS_SIZE(grid_x-OrigX));
         PixelToMeasurementUnit(y_buf, ABS_SIZE(grid_y-OrigY));
         if (VerboseMeasureTooltip()) {
            sprintf(buf, "dx=%s dy=%s", x_buf, y_buf);
         } else {
            sprintf(buf, "dx=%s\ndy=%s", x_buf, y_buf);
         }
         ShowMeasureCursor(grid_x, grid_y, buf, TRUE);
         /* erase */
         MarkVs(REVERSE, grid_x-OrigX, grid_y-OrigY);
         MarkVsForStructuredSplines(REVERSE, ABS_SIZE(grid_x-OrigX),
               ABS_SIZE(grid_y-OrigY));
         HighLightVs(REVERSE, ABS_SIZE(grid_x-OrigX), ABS_SIZE(grid_x-OrigX));

         if (input.type == KeyPress || input.type == KeyRelease) {
            x = saved_grid_x;
            y = saved_grid_y;
         } else {
            x = input.xmotion.x;
            y = input.xmotion.y;
         }
         if (shiftForDiagMouseMove && DiagEventCheck(&input)) {
            if (useRecentDupDistance && justDupped &&
                  useRecentForDiagMouseMove) {
               DiagGridXY(OrigX-ZOOMED_SIZE(dupDx), OrigY-ZOOMED_SIZE(dupDy),
                     &x, &y);
            } else {
               DiagGridXY(OrigX, OrigY, &x, &y);
            }
         }
         GridXY(x, y, &grid_x, &grid_y);

         dx = grid_x - saved_grid_x;
         dy = grid_y - saved_grid_y;

         saved_grid_x = grid_x;
         saved_grid_y = grid_y;

         abs_dx_from_orig = ABS_SIZE(grid_x-OrigX);
         abs_dy_from_orig = ABS_SIZE(grid_y-OrigY);

         for (vsel_ptr=botVSel, obj_ptr=tmpBotObj; vsel_ptr != NULL;
               vsel_ptr=vsel_ptr->prev, obj_ptr=obj_ptr->prev) {
            switch (obj_ptr->type) {
            case OBJ_POLY:
               poly_ptr = obj_ptr->detail.p;
               curved = poly_ptr->curved;
               num_pts = poly_ptr->n;
               pv = poly_ptr->vlist;
               for (i=0; i < vsel_ptr->n; i++) {
                  pv[vsel_ptr->v_index[i]].x += ABS_SIZE(dx);
                  pv[vsel_ptr->v_index[i]].y += ABS_SIZE(dy);
               }
               if (poly_ptr->svlist != NULL) {
                  free(poly_ptr->svlist);
                  poly_ptr->svlist = NULL;
               }
               switch (curved) {
               case LT_STRAIGHT:
               case LT_SPLINE:
                  poly_ptr->svlist = MakeMultiSplinePolyVertex(
                        curved, &(poly_ptr->sn), poly_ptr->smooth,
                        drawOrigX, drawOrigY, num_pts, pv);
                  break;
               case LT_STRUCT_SPLINE:
                  /* nothing to do here */
                  break;
               case LT_INTSPLINE:
                  if (poly_ptr->intvlist != NULL) {
                     free(poly_ptr->intvlist);
                     poly_ptr->intvlist = NULL;
                  }
                  poly_ptr->svlist = MakeIntSplinePolyVertex(
                        &(poly_ptr->sn), &(poly_ptr->intn),
                        &(poly_ptr->intvlist), drawOrigX, drawOrigY,
                        num_pts, pv);
                  break;
               }
               break;
            case OBJ_POLYGON:
               polygon_ptr = obj_ptr->detail.g;
               curved = polygon_ptr->curved;
               num_pts = polygon_ptr->n;
               pv = polygon_ptr->vlist;
               for (i=0; i < vsel_ptr->n; i++) {
                  pv[vsel_ptr->v_index[i]].x += ABS_SIZE(dx);
                  pv[vsel_ptr->v_index[i]].y += ABS_SIZE(dy);
               }
               if (polygon_ptr->svlist != NULL) {
                  free(polygon_ptr->svlist);
                  polygon_ptr->svlist = NULL;
               }
               switch (curved) {
               case LT_STRAIGHT:
               case LT_SPLINE:
                  polygon_ptr->svlist =
                        MakeMultiSplinePolygonVertex(curved, 
                        &(polygon_ptr->sn), polygon_ptr->smooth,
                        drawOrigX, drawOrigY, num_pts, pv);
                  break;
               case LT_STRUCT_SPLINE:
                  /* nothing to do here */
                  break;
               case LT_INTSPLINE:
                  if (polygon_ptr->intvlist != NULL) {
                     free(polygon_ptr->intvlist);
                     polygon_ptr->intvlist = NULL;
                  }
                  polygon_ptr->svlist =
                        MakeIntSplinePolygonVertex(&(polygon_ptr->sn),
                        &(polygon_ptr->intn), &(polygon_ptr->intvlist),
                        drawOrigX, drawOrigY, num_pts, pv);
                  break;
               }
               break;
            }
         }
         /* draw */
         MarkVsForStructuredSplines(FORWARD, abs_dx_from_orig,
               abs_dy_from_orig);
         HighLightVs(FORWARD, abs_dx_from_orig, abs_dy_from_orig);
         MarkVs(FORWARD, grid_x-OrigX, grid_y-OrigY);
         PixelToMeasurementUnit(x_buf, abs_dx_from_orig);
         PixelToMeasurementUnit(y_buf, abs_dy_from_orig);
         if (VerboseMeasureTooltip()) {
            sprintf(buf, "dx=%s dy=%s", x_buf, y_buf);
         } else {
            sprintf(buf, "dx=%s\ndy=%s", x_buf, y_buf);
         }
         DrawIntervalRulers(ruler_bbox.ltx+abs_dx_from_orig,
               ruler_bbox.lty+abs_dy_from_orig, ruler_bbox.rbx+abs_dx_from_orig,
               ruler_bbox.rby+abs_dy_from_orig, buf);
         ShowMeasureCursor(grid_x, grid_y, buf, TRUE);
         while (XCheckMaskEvent(mainDisplay, PointerMotionMask, &ev)) ;
      }
   }
}
