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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/select.c,v 1.26 2011/05/16 16:21:59 william Exp $
 */

#define _INCLUDE_FROM_SELECT_C_

#include "tgifdefs.h"

#include "attr.e"
#include "auxtext.e"
#include "button.e"
#include "choice.e"
#include "cmd.e"
#include "color.e"
#include "cursor.e"
#include "dialog.e"
#include "drawing.e"
#include "dup.e"
#include "edit.e"
#include "exec.e"
#include "file.e"
#include "font.e"
#include "grid.e"
#include "group.e"
#include "http.e"
#include "mainloop.e"
#include "mark.e"
#include "menu.e"
#include "miniline.e"
#include "move.e"
#include "msg.e"
#include "names.e"
#include "navigate.e"
#include "obj.e"
#include "page.e"
#include "pin.e"
#include "poly.e"
#include "raster.e"
#include "rect.e"
#include "remote.e"
#include "ruler.e"
#include "scroll.e"
#include "select.e"
#include "setup.e"
#include "special.e"
#include "stk.e"
#include "stretch.e"
#include "strtbl.e"
#include "util.e"

#define FORWARD 0
#define REVERSE 1

int	selLtX=0, selLtY=0, selRbX=0, selRbY=0;
int	selObjLtX=0, selObjLtY=0, selObjRbX=0, selObjRbY=0;
int	selNoLockLtX=0, selNoLockLtY=0, selNoLockRbX=0, selNoLockRbY=0;
int	selNoLockObjLtX=0, selNoLockObjLtY=0;
int	selNoLockObjRbX=0, selNoLockObjRbY=0;
int	numObjSelected=0;
int	numObjLocked=0;
struct SelRec	*topSel=NULL, *botSel=NULL;
struct VSelRec	*topVSel=NULL, *botVSel=NULL;

int GetObjCurved(ObjPtr)
   struct ObjRec *ObjPtr;
{
   switch (ObjPtr->type) {
   case OBJ_POLY: return ObjPtr->detail.p->curved;
   case OBJ_POLYGON: return ObjPtr->detail.g->curved;
   }
#ifdef _TGIF_DBG /* debug, do not translate */
   TgAssert(FALSE, "Unexpected object type in GetObjCurved()", NULL);
#endif /* _TGIF_DBG */
   return 0;
}

struct SelRec *SelectThisObject(ObjPtr)
   struct ObjRec *ObjPtr;
{
   struct SelRec *sel_ptr=(struct SelRec *)malloc(sizeof(struct SelRec));

   if (sel_ptr == NULL) FailAllocMessage();
   memset(sel_ptr, 0, sizeof(struct SelRec));
   sel_ptr->obj = ObjPtr;
   return sel_ptr;
}

int CountSelectedVertices()
{
   struct VSelRec *vsel_ptr;
   int count = 0;

   for (vsel_ptr=topVSel; vsel_ptr != NULL; vsel_ptr=vsel_ptr->next) {
      int curved=GetObjCurved(vsel_ptr->obj);

      if (curved == LT_STRUCT_SPLINE) {
         /* can select at most one vertex for LT_STRUCT_SPLINE */
         count++;
      } else {
         int n=vsel_ptr->n;

         count += n;
         if (vsel_ptr->obj->type == OBJ_POLYGON) {
            int i=0;

            for (i=0; i < n; i++) {
               if (vsel_ptr->v_index[i] == 0) {
                  count--;
                  break;
               }
            }
         }
      }
   }
   return count;
}

void CalcBBox(X1, Y1, X2, Y2, LtX, LtY, RbX, RbY)
   int X1, Y1, X2, Y2, * LtX, * LtY, * RbX, * RbY;
{
   if (X1 < X2) {
      if (Y1 < Y2) {
         *LtX = X1; *LtY = Y1; *RbX = X2; *RbY = Y2;
      } else {
         *LtX = X1; *LtY = Y2; *RbX = X2; *RbY = Y1;
      }
   } else {
      if (Y1 < Y2) {
         *LtX = X2; *LtY = Y1; *RbX = X1; *RbY = Y2;
      } else {
         *LtX = X2; *LtY = Y2; *RbX = X1; *RbY = Y1;
      }
   }
}

void CalcVertexBBox(LtX, LtY, RbX, RbY)
   int *LtX, *LtY, *RbX, *RbY;
{
   register int i, *x_ptr, *y_ptr;
   struct VSelRec *vsel_ptr;

   *LtX = selRbX; *LtY = selRbY; *RbX = selLtX; *RbY = selLtY;

   for (vsel_ptr=topVSel; vsel_ptr!=NULL; vsel_ptr=vsel_ptr->next) {
      for (i=0, x_ptr=vsel_ptr->x, y_ptr=vsel_ptr->y; i < vsel_ptr->n;
            i++, x_ptr++, y_ptr++) {
         if (*x_ptr < *LtX) *LtX = *x_ptr;
         if (*y_ptr < *LtY) *LtY = *y_ptr;
         if (*x_ptr > *RbX) *RbX = *x_ptr;
         if (*y_ptr > *RbY) *RbY = *y_ptr;
      }
   }
}

void UnSelNonVertexObjs(highlight, no_locked_obj_only)
   int highlight, no_locked_obj_only;
{
   register struct ObjRec *obj_ptr;
   register struct SelRec *sel_ptr, *prev_sel;

   for (sel_ptr=botSel; sel_ptr != NULL; sel_ptr=prev_sel) {
      prev_sel = sel_ptr->prev;
      obj_ptr = sel_ptr->obj;

      if ((obj_ptr->type==OBJ_POLY || obj_ptr->type==OBJ_POLYGON) &&
            (!no_locked_obj_only || !obj_ptr->locked)) {
         continue;
      }
      if (highlight) HighLightAnObj(obj_ptr);

      if (sel_ptr->prev == NULL) {
         topSel = sel_ptr->next;
      } else {
         sel_ptr->prev->next = sel_ptr->next;
      }
      if (sel_ptr->next == NULL) {
         botSel = sel_ptr->prev;
      } else {
         sel_ptr->next->prev = sel_ptr->prev;
      }
      free(sel_ptr);
   }
}

void FreeTopSel()
{
   struct SelRec *sel_ptr=topSel;
 
   topSel = topSel->next;
   if (topSel != NULL) {
      topSel->prev = NULL;
   } else {
      botSel = NULL;
   }
   free(sel_ptr);
   numObjSelected--;
   UpdSelBBox();
}

void JustFreeSel(pTopSel, pBotSel)
   struct SelRec *pTopSel, *pBotSel;
{
   struct SelRec *pNextSel=NULL;

   for ( ; pTopSel != NULL; pTopSel=pNextSel) {
      pNextSel = pTopSel->next;
      free(pTopSel);
   }
}

void UnlinkSel(pSel, ppTopSel, ppBotSel)
   struct SelRec *pSel, **ppTopSel, **ppBotSel;
{
   if ((*ppTopSel) == pSel) {
      (*ppTopSel) = pSel->next;
   } else {
      pSel->prev->next = pSel->next;
   }
   if ((*ppBotSel) == pSel) {
      (*ppBotSel) = pSel->prev;
   } else {
      pSel->next->prev = pSel->prev;
   }
}

struct SelRec *FindObjInSel(pObj, pTopSel, pBotSel)
   struct ObjRec *pObj;
   struct SelRec *pTopSel, *pBotSel;
{
   for ( ; pTopSel != NULL; pTopSel=pTopSel->next) {
      if (pTopSel->obj == pObj) {
         return pTopSel;
      }
   }
   return NULL;
}

int PrependObjToSel(pObj, ppTopSel, ppBotSel)
   struct ObjRec *pObj;
   struct SelRec **ppTopSel, **ppBotSel;
{
   int nReturn=TRUE;
   struct SelRec *pSel=(struct SelRec *)malloc(sizeof(struct SelRec));

   if (pSel == NULL) {
      FailAllocMessage();
      nReturn = FALSE;
   }
   memset(pSel, 0, sizeof(struct SelRec));
   pSel->obj = pObj;
   pSel->next = (*ppTopSel);
   pSel->prev = NULL;
   if ((*ppTopSel) == NULL) {
      (*ppBotSel) = pSel;
   } else {
      (*ppTopSel)->prev = pSel;
   }
   (*ppTopSel) = pSel;
   return nReturn;
}

struct SelRec *AddObjIntoSel(pObj, pPrevSel, pNextSel, ppTopSel, ppBotSel)
   struct ObjRec *pObj;
   struct SelRec *pPrevSel, *pNextSel, **ppTopSel, **ppBotSel;
{
   struct SelRec *pSel=(struct SelRec *)malloc(sizeof(struct SelRec));

   if (pSel == NULL) {
      FailAllocMessage();
      return NULL;
   }
   memset(pSel, 0, sizeof(struct SelRec));
   pSel->obj = pObj;
   pSel->next = pNextSel;
   pSel->prev = pPrevSel;
   if (pPrevSel == NULL) {
      (*ppTopSel) = pSel;
   } else {
      pPrevSel->next = pSel;
   }
   if (pNextSel == NULL) {
      (*ppBotSel) = pSel;
   } else {
      pNextSel->prev = pSel;
   }
   return pSel;
}

void JustRemoveAllVSel()
{
   register struct VSelRec *next_vsel;

   while (topVSel != NULL) {
      next_vsel = topVSel->next;
      free(topVSel->v_index);
      free(topVSel->x);
      free(topVSel->y);
      free(topVSel);
      topVSel = next_vsel;
   }
   botVSel = NULL;
}

void RemoveAllSel()
{
   register struct SelRec *next_sel;
   register struct VSelRec *next_vsel;

   while (topSel != NULL) {
      next_sel = topSel->next;
      free(topSel);
      topSel = next_sel;
   }
   botSel = NULL;

   while (topVSel != NULL) {
      next_vsel = topVSel->next;
      free(topVSel->v_index);
      free(topVSel->x);
      free(topVSel->y);
      free(topVSel);
      topVSel = next_vsel;
   }
   botVSel = NULL;
   numObjSelected = 0;
}

struct AttrRec *FindObjAttrWithName(ObjPtr, AttrName)
   struct ObjRec *ObjPtr;
   char *AttrName;
   /* AttrName here must not contain '.' */
{
   register struct AttrRec *attr_ptr;
   struct AttrRec *found_attr=NULL;
   int count=1, compare_name=(strchr(AttrName,'=') != NULL);

   if (ObjPtr == NULL) return NULL;

   for (attr_ptr=ObjPtr->fattr; attr_ptr!=NULL; attr_ptr=attr_ptr->next) {
      if (compare_name) {
         if (strcmp(attr_ptr->attr_name.s, AttrName) == 0) {
            found_attr = attr_ptr;
            break;
         }
      } else {
         if (strcmp(attr_ptr->attr_value.s, AttrName) == 0) {
            found_attr = attr_ptr;
            break;
         }
      }
   }
   if (attr_ptr == NULL) return NULL;

   if (GetTextObjFirstStrSeg(found_attr->obj)->color == colorIndex) {
      return found_attr;
   }
   for (attr_ptr=found_attr->next; attr_ptr!=NULL; attr_ptr=attr_ptr->next) {
      if (compare_name) {
         if (strcmp(attr_ptr->attr_name.s, AttrName) == 0) {
            if (GetTextObjFirstStrSeg(attr_ptr->obj)->color == colorIndex) {
               break;
            } else if (GetTextObjFirstStrSeg(attr_ptr->obj)->color ==
                  GetTextObjFirstStrSeg(found_attr->obj)->color) {
               /* same color as found_attr's color, so don't increase count */
            } else {
               count++;
            }
         }
      } else {
         if (strcmp(attr_ptr->attr_value.s, AttrName) == 0) {
            if (GetTextObjFirstStrSeg(attr_ptr->obj)->color == colorIndex) {
               break;
            } else if (GetTextObjFirstStrSeg(attr_ptr->obj)->color ==
                  GetTextObjFirstStrSeg(found_attr->obj)->color) {
               /* same color as found_attr's color, so don't increase count */
            } else {
               count++;
            }
         }
      }
   }
   if (attr_ptr != NULL) {
      found_attr = attr_ptr;
   } else if (count != 1) {
      sprintf(gszMsgBox, TgLoadCachedString(CSTID_CANT_FIND_ATTR_WITH_COLOR),
            AttrName, colorMenuItems[colorIndex]);
      Msg(gszMsgBox);
      return NULL;
   }
   return found_attr;
}

static
struct ObjRec *FindAVertex(XOff, YOff, VIndex, AbsX, AbsY)
   int XOff, YOff, *VIndex, *AbsX, *AbsY;
   /* XOff and YOff are screen offsets */
{
   struct SelRec *sel_ptr;

   for (sel_ptr=topSel; sel_ptr != NULL; sel_ptr=sel_ptr->next) {
      struct ObjRec *obj_ptr=sel_ptr->obj;
      struct PolyRec *poly_ptr=NULL;
      struct PolygonRec *polygon_ptr=NULL;
      int n=0;
      IntPoint *vs=NULL;

      if (obj_ptr->type != OBJ_POLY && obj_ptr->type != OBJ_POLYGON) continue;
      if (!(XOff >= OFFSET_X(obj_ptr->bbox.ltx)-3 &&
            YOff >= OFFSET_Y(obj_ptr->bbox.lty)-3 &&
            XOff <= OFFSET_X(obj_ptr->bbox.rbx)+3 &&
            YOff <= OFFSET_Y(obj_ptr->bbox.rby)+3)) {
         continue;
      }

      switch (obj_ptr->type) {
      case OBJ_POLY:
         poly_ptr = obj_ptr->detail.p;
         if (poly_ptr->curved == LT_STRUCT_SPLINE) {
            n = poly_ptr->ssn;
            vs = poly_ptr->ssvlist;
         } else {
            n = poly_ptr->n;
            vs = poly_ptr->vlist;
         }
         if (PtInPolyMark(obj_ptr, XOff, YOff, n, vs, VIndex)) {
            if (obj_ptr->ctm == NULL) {
               *AbsX = vs[*VIndex].x;
               *AbsY = vs[*VIndex].y;
            } else {
               int x, y;

               TransformPointThroughCTM( vs[*VIndex].x-obj_ptr->x,
                     vs[*VIndex].y-obj_ptr->y, obj_ptr->ctm, &x, &y);
               *AbsX = x+obj_ptr->x;
               *AbsY = y+obj_ptr->y;
            }
            return obj_ptr;
         }
         break;
      case OBJ_POLYGON:
         polygon_ptr = obj_ptr->detail.g;
         if (polygon_ptr->curved == LT_STRUCT_SPLINE) {
            n = polygon_ptr->ssn;
            vs = polygon_ptr->ssvlist;
         } else {
            n = polygon_ptr->n;
            vs = polygon_ptr->vlist;
         }
         if (PtInPolyMark(obj_ptr, XOff, YOff, n, vs, VIndex)) {
            if (obj_ptr->ctm == NULL) {
               *AbsX = vs[*VIndex].x;
               *AbsY = vs[*VIndex].y;
            } else {
               int x, y;

               TransformPointThroughCTM(vs[*VIndex].x-obj_ptr->x,
                     vs[*VIndex].y-obj_ptr->y, obj_ptr->ctm, &x, &y);
               *AbsX = x+obj_ptr->x;
               *AbsY = y+obj_ptr->y;
            }
            return obj_ptr;
         }
         break;
      }
   }
   return NULL;
}

static
int CandidatePortOwner(obj_ptr)
   struct ObjRec *obj_ptr;
{
   if (obj_ptr->type == OBJ_SYM || obj_ptr->type == OBJ_ICON) {
      return TRUE;
   } else if (obj_ptr->type == OBJ_GROUP) {
      struct AttrRec *attr_ptr=FindAttrWithName(obj_ptr, "type=", NULL);

      if (attr_ptr != NULL && strcmp(attr_ptr->attr_value.s,
            "tgBroadcastWire") == 0) {
         return TRUE;
      }
   }
   return FALSE;
}

struct ObjRec *FindAnObj(XOff, YOff, OwnerObj, ConnectObj, ReturnedObjName)
   int XOff, YOff;
   struct ObjRec **OwnerObj, **ConnectObj;
   char *ReturnedObjName;
   /* XOff and YOff are screen offsets */
{
   register struct ObjRec *obj_ptr;
   register struct AttrRec *attr_ptr;
   struct ObjRec *sub_obj, *returned_obj=NULL, *actual_obj=NULL;
   struct ObjRec *owner_obj=NULL;
   int found_attr=FALSE;

   if (OwnerObj != NULL) *OwnerObj = NULL;
   if (ConnectObj != NULL) *ConnectObj = NULL;
   if (ReturnedObjName != NULL) *ReturnedObjName = '\0';

   for (obj_ptr=topObj; returned_obj==NULL && obj_ptr!=NULL;
         obj_ptr=obj_ptr->next) {
      obj_ptr->tmp_child = NULL;
      obj_ptr->tmp_parent = NULL;
      if (colorLayers && !ObjInVisibleLayer(obj_ptr)) {
         continue;
      }
      actual_obj = obj_ptr;
      for (attr_ptr=obj_ptr->fattr; returned_obj==NULL && attr_ptr!=NULL;
            attr_ptr=attr_ptr->next) {
         if (attr_ptr->shown &&
               XOff >= OFFSET_X(attr_ptr->obj->bbox.ltx)-3 &&
               YOff >= OFFSET_Y(attr_ptr->obj->bbox.lty)-3 &&
               XOff <= OFFSET_X(attr_ptr->obj->bbox.rbx)+3 &&
               YOff <= OFFSET_Y(attr_ptr->obj->bbox.rby)+3) {
            owner_obj = obj_ptr;
            returned_obj = attr_ptr->obj;
            found_attr = TRUE;
            break;
         }
      }
      if (returned_obj != NULL) break;

      if (XOff >= OFFSET_X(obj_ptr->bbox.ltx)-3 &&
            YOff >= OFFSET_Y(obj_ptr->bbox.lty)-3 &&
            XOff <= OFFSET_X(obj_ptr->bbox.rbx)+3 &&
            YOff <= OFFSET_Y(obj_ptr->bbox.rby)+3) {
         struct ObjRec *next_level_child=NULL, *visible_obj=NULL;

         switch (obj_ptr->type) {
         case OBJ_TEXT:
            if (FindGoodText(XOff,YOff,obj_ptr)) returned_obj = obj_ptr;
            break;
         case OBJ_XBM:
            if (FindGoodXBm(XOff,YOff,obj_ptr)) returned_obj = obj_ptr;
            break;
         case OBJ_XPM:
            if (FindGoodXPm(XOff,YOff,obj_ptr)) returned_obj = obj_ptr;
            break;
         case OBJ_BOX:
            if (FindGoodBox(XOff,YOff,obj_ptr)) returned_obj = obj_ptr;
            break;
         case OBJ_OVAL:
            if (FindGoodOval(XOff,YOff,obj_ptr)) returned_obj = obj_ptr;
            break;
         case OBJ_POLY:
            if (FindGoodPoly(XOff,YOff,obj_ptr)) returned_obj = obj_ptr;
            break;
         case OBJ_POLYGON:
            if (FindGoodPolygon(XOff,YOff,obj_ptr)) returned_obj = obj_ptr;
            break;
         case OBJ_ARC:
            if (FindGoodArc(XOff,YOff,obj_ptr)) returned_obj = obj_ptr;
            break;
         case OBJ_RCBOX:
            if (FindGoodRCBox(XOff,YOff,obj_ptr)) returned_obj = obj_ptr;
            break;

         case OBJ_GROUP:
         case OBJ_ICON:
         case OBJ_SYM:
            if (colorLayers) {
               struct ObjRec *tmp_obj;

               for (tmp_obj=obj_ptr->detail.r->first; tmp_obj != NULL;
                     tmp_obj=tmp_obj->next) {
                  tmp_obj->tmp_parent = obj_ptr;
               }
            }
            if (FindGoodObj(XOff, YOff, obj_ptr->detail.r->first, &sub_obj,
                  &next_level_child)) {
               obj_ptr->tmp_child = next_level_child;
               if (sub_obj == NULL) {
                  owner_obj = NULL;
                  returned_obj = obj_ptr;
               } else {
                  owner_obj = obj_ptr;
                  returned_obj = sub_obj;
               }
            }
            break;
         case OBJ_PIN:
            visible_obj = GetPinObj(obj_ptr);
            if (colorLayers) {
               struct ObjRec *tmp_obj;

               for (tmp_obj=visible_obj->detail.r->first; tmp_obj != NULL;
                     tmp_obj=tmp_obj->next) {
                  tmp_obj->tmp_parent = visible_obj;
               }
            }
            obj_ptr->tmp_child = visible_obj;
            if (FindGoodObj(XOff, YOff, GetPinObj(obj_ptr), &sub_obj,
                  &next_level_child)) {
               visible_obj->tmp_child = next_level_child;
               if (sub_obj == NULL) {
                  owner_obj = NULL;
                  returned_obj = obj_ptr;
               } else {
                  owner_obj = obj_ptr;
                  returned_obj = sub_obj;
               }
            }
            break;
         }
      }
   }
   if (ReturnedObjName != NULL) {
      char *c_ptr;

      *ReturnedObjName = '\0';
      c_ptr = ReturnedObjName;
      if (returned_obj != NULL) {
         struct ObjRec *prev_obj=NULL;
         int see_sym_or_icon=FALSE;

         for (obj_ptr=actual_obj; obj_ptr != NULL; obj_ptr=obj_ptr->tmp_child) {
            /* do not translate -- program constants */
            if (connectingPortsFromInternalCommand) {
               /* this is pretty much left for compatibility only */
               if ((attr_ptr=FindObjAttrWithName(obj_ptr, "name=")) != NULL) {
                  *c_ptr++ = '!';
                  strcpy(c_ptr, attr_ptr->attr_value.s);
                  c_ptr = &c_ptr[strlen(c_ptr)];
               } else {
                  if (prev_obj != NULL) prev_obj->tmp_child = NULL;
                  break;
               }
            } else {
               int just_seen_sym_or_icon=FALSE;

               if (!see_sym_or_icon) {
                  if (CandidatePortOwner(obj_ptr)) {
                     see_sym_or_icon = TRUE;
                     just_seen_sym_or_icon = TRUE;
                  }
               }
               if (see_sym_or_icon) {
                  if ((attr_ptr=FindObjAttrWithName(obj_ptr, "name=")) ==
                        NULL) {
                     /* broken name is invalid */
                     c_ptr = ReturnedObjName;
                     if (prev_obj != NULL) prev_obj->tmp_child = NULL;
                     break;
                  } else if (BlankStr(attr_ptr->attr_value.s)) {
                     /* do not translate -- program constants */
                     *c_ptr++ = '!';
                     strcpy(c_ptr, "(unknown)");
                     c_ptr = &c_ptr[strlen(c_ptr)];
                  } else {
                     *c_ptr++ = '!';
                     strcpy(c_ptr, attr_ptr->attr_value.s);
                     c_ptr = &c_ptr[strlen(c_ptr)];
                  }
                  if (!just_seen_sym_or_icon && CandidatePortOwner(obj_ptr)) {
                     obj_ptr->tmp_child = NULL;
                     prev_obj = obj_ptr;
                     break;
                  }
               }
            }
            prev_obj = obj_ptr;
         }
         if (ConnectObj != NULL) *ConnectObj = prev_obj;
      }
      if (c_ptr == ReturnedObjName) {
         *ReturnedObjName = '\0';
      } else {
         *c_ptr = '\0';
      }
   }
   if (OwnerObj != NULL) *OwnerObj = owner_obj;
   return returned_obj;
}

static
int VertexAlreadySelected(ObjPtr, VIndex, VSelPtr)
   register struct ObjRec *ObjPtr;
   int VIndex;
   register struct VSelRec **VSelPtr;
{
   register int i;

   for (*VSelPtr=topVSel; *VSelPtr != NULL; *VSelPtr=(*VSelPtr)->next) {
      if ((*VSelPtr)->obj == ObjPtr) {
         for (i = 0; i < (*VSelPtr)->n; i++) {
            if ((*VSelPtr)->v_index[i] == VIndex) {
               return TRUE;
            }
         }
         return FALSE;
      }
   }
   return FALSE;
}

struct SelRec *AlreadySelected(ObjPtr)
   register struct ObjRec *ObjPtr;
{
   register struct SelRec *sel_ptr;

   for (sel_ptr=topSel; sel_ptr != NULL; sel_ptr=sel_ptr->next) {
      if (sel_ptr->obj == ObjPtr) {
         return sel_ptr;
      }
   }
   return NULL;
}

void AddSel(PrevPtr, NextPtr, SelPtr)
   struct SelRec *PrevPtr, *NextPtr, *SelPtr;
   /* add SelPtr between PrevPtr and NextPtr */
{
   SelPtr->prev = PrevPtr;
   SelPtr->next = NextPtr;

   if (PrevPtr == NULL) {
      topSel = SelPtr;
   } else {
      PrevPtr->next = SelPtr;
   }
   if (NextPtr == NULL) {
      botSel = SelPtr;
   } else {
      NextPtr->prev = SelPtr;
   }
   numObjSelected++;
}

void AddNewSelObj(ObjPtr)
   register struct ObjRec *ObjPtr;
{
   register struct ObjRec *obj_ptr=topObj;
   register struct SelRec *sel_ptr=topSel, *new_sel_ptr;

   new_sel_ptr = (struct SelRec *)malloc(sizeof(struct SelRec));
   if (new_sel_ptr == NULL) FailAllocMessage();
   new_sel_ptr->obj = ObjPtr;

   for ( ; sel_ptr != NULL && obj_ptr != ObjPtr; obj_ptr=obj_ptr->next) {
      if (obj_ptr == sel_ptr->obj) {
         sel_ptr = sel_ptr->next;
      }
   }
   if (sel_ptr == NULL) {
      /* the object is below the last selected object */
      if (botSel == NULL) {
         topSel = new_sel_ptr;
      } else {
         botSel->next = new_sel_ptr;
      }
      new_sel_ptr->prev = botSel;
      new_sel_ptr->next = NULL;
      botSel = new_sel_ptr;
   } else {
      /* the object is between sel_ptr and sel_ptr->prev */
      if (sel_ptr->prev == NULL) {
         topSel = new_sel_ptr;
      } else {
         sel_ptr->prev->next = new_sel_ptr;
      }
      new_sel_ptr->next = sel_ptr;
      new_sel_ptr->prev = sel_ptr->prev;
      sel_ptr->prev = new_sel_ptr;
   }
   numObjSelected++;
}

void ExpandCurSelBBoxes(obj_ptr)
   struct ObjRec *obj_ptr;
{
   if (obj_ptr->bbox.ltx < selLtX) selLtX = obj_ptr->bbox.ltx;
   if (obj_ptr->bbox.lty < selLtY) selLtY = obj_ptr->bbox.lty;
   if (obj_ptr->bbox.rbx < selRbX) selRbX = obj_ptr->bbox.rbx;
   if (obj_ptr->bbox.rby < selRbY) selRbY = obj_ptr->bbox.rby;
   if (obj_ptr->obbox.ltx < selObjLtX) {
      selObjLtX = obj_ptr->obbox.ltx;
   }
   if (obj_ptr->obbox.lty < selObjLtY) {
      selObjLtY = obj_ptr->obbox.lty;
   }
   if (obj_ptr->obbox.rbx < selObjRbX) {
      selObjRbX = obj_ptr->obbox.rbx;
   }
   if (obj_ptr->obbox.rby < selObjRbY) {
      selObjRbY = obj_ptr->obbox.rby;
   }
}

static
void SetNoLockBBox(ObjPtr)
   struct ObjRec *ObjPtr;
{
   selNoLockLtX = ObjPtr->bbox.ltx;
   selNoLockLtY = ObjPtr->bbox.lty;
   selNoLockRbX = ObjPtr->bbox.rbx;
   selNoLockRbY = ObjPtr->bbox.rby;
   selNoLockObjLtX = ObjPtr->obbox.ltx;
   selNoLockObjLtY = ObjPtr->obbox.lty;
   selNoLockObjRbX = ObjPtr->obbox.rbx;
   selNoLockObjRbY = ObjPtr->obbox.rby;
}

void UpdSelBBox()
   /* update selLtX, selLtY, selRbX, selRbY */
{
   register struct ObjRec *obj_ptr;
   register struct SelRec *sel_ptr;
   int seen_no_locked=FALSE;

   numObjSelected = 0;
   numObjLocked = 0;
   if ((sel_ptr = topSel) == NULL) return;

   numObjSelected++;
   obj_ptr = sel_ptr->obj;
   if (obj_ptr->locked) numObjLocked++;
   selLtX = obj_ptr->bbox.ltx;
   selLtY = obj_ptr->bbox.lty;
   selRbX = obj_ptr->bbox.rbx;
   selRbY = obj_ptr->bbox.rby;
   selObjLtX = obj_ptr->obbox.ltx;
   selObjLtY = obj_ptr->obbox.lty;
   selObjRbX = obj_ptr->obbox.rbx;
   selObjRbY = obj_ptr->obbox.rby;
   if (!obj_ptr->locked) {
      SetNoLockBBox(obj_ptr);
      seen_no_locked = TRUE;
   }
   for (sel_ptr=topSel->next; sel_ptr != NULL; sel_ptr=sel_ptr->next) {
      numObjSelected++;
      obj_ptr = sel_ptr->obj;
      if (obj_ptr->bbox.ltx < selLtX) selLtX = obj_ptr->bbox.ltx;
      if (obj_ptr->bbox.lty < selLtY) selLtY = obj_ptr->bbox.lty;
      if (obj_ptr->bbox.rbx > selRbX) selRbX = obj_ptr->bbox.rbx;
      if (obj_ptr->bbox.rby > selRbY) selRbY = obj_ptr->bbox.rby;
      if (obj_ptr->obbox.ltx < selObjLtX) selObjLtX = obj_ptr->obbox.ltx;
      if (obj_ptr->obbox.lty < selObjLtY) selObjLtY = obj_ptr->obbox.lty;
      if (obj_ptr->obbox.rbx > selObjRbX) selObjRbX = obj_ptr->obbox.rbx;
      if (obj_ptr->obbox.rby > selObjRbY) selObjRbY = obj_ptr->obbox.rby;
      if (obj_ptr->locked) {
         numObjLocked++;
      } else {
         if (seen_no_locked) {
            if (obj_ptr->bbox.ltx < selNoLockLtX) {
               selNoLockLtX = obj_ptr->bbox.ltx;
            }
            if (obj_ptr->bbox.lty < selNoLockLtY) {
               selNoLockLtY = obj_ptr->bbox.lty;
            }
            if (obj_ptr->bbox.rbx > selNoLockRbX) {
               selNoLockRbX = obj_ptr->bbox.rbx;
            }
            if (obj_ptr->bbox.rby > selNoLockRbY) {
               selNoLockRbY = obj_ptr->bbox.rby;
            }
            if (obj_ptr->obbox.ltx < selNoLockObjLtX) {
               selNoLockObjLtX = obj_ptr->obbox.ltx;
            }
            if (obj_ptr->obbox.lty < selNoLockObjLtY) {
               selNoLockObjLtY = obj_ptr->obbox.lty;
            }
            if (obj_ptr->obbox.rbx > selNoLockObjRbX) {
               selNoLockObjRbX = obj_ptr->obbox.rbx;
            }
            if (obj_ptr->obbox.rby > selNoLockObjRbY) {
               selNoLockObjRbY = obj_ptr->obbox.rby;
            }
         } else {
            SetNoLockBBox(obj_ptr);
            seen_no_locked = TRUE;
         }
      }
   }
}
 
static
struct VSelRec *SelectOneVertex(XOff, YOff)
   int XOff, YOff;
   /* XOff and YOff are screen offsets */
{
   register struct ObjRec *obj_ptr;
   int v_index, x, y;

   JustRemoveAllVSel();
   if ((obj_ptr=FindAVertex(XOff, YOff, &v_index, &x, &y)) == NULL) {
      return NULL;
   }
   topVSel = (struct VSelRec *)malloc(sizeof(struct VSelRec));
   if (topVSel == NULL) FailAllocMessage();
   memset(topVSel, 0, sizeof(struct VSelRec));
   topVSel->obj = obj_ptr;
   topVSel->max_v = 10;
   topVSel->v_index = (int*)malloc(10*sizeof(int));
   if (topVSel->v_index == NULL) FailAllocMessage();
   topVSel->x = (int*)malloc(10*sizeof(int));
   topVSel->y = (int*)malloc(10*sizeof(int));
   if (topVSel->x == NULL || topVSel->y == NULL) FailAllocMessage();
   topVSel->v_index[0] = v_index;
   topVSel->x[0] = x;
   topVSel->y[0] = y;
   if (obj_ptr->type==OBJ_POLYGON && v_index==0) {
      topVSel->n = 2;
      topVSel->v_index[1] = obj_ptr->detail.g->n-1;
      topVSel->x[1] = x;
      topVSel->y[1] = y;
   } else {
      topVSel->n = 1;
   }
   topVSel->next = NULL;
   topVSel->prev = NULL;
   botVSel = topVSel;
   UpdSelBBox();

   return topVSel;
}

static
struct SelRec *SelectOneObj(XOff, YOff, ppInnerObj)
   int XOff, YOff;
   struct ObjRec **ppInnerObj;
   /* XOff and YOff are screen offsets */
{
   register struct ObjRec *obj_ptr;
   struct ObjRec *owner_obj;

   RemoveAllSel();
   if ((obj_ptr=FindAnObj(XOff,YOff,&owner_obj,NULL,NULL)) == NULL) {
      return NULL;
   }
   if (ppInnerObj != NULL) {
      *ppInnerObj = (owner_obj==NULL ? NULL : obj_ptr);
   }
   if (owner_obj != NULL) obj_ptr = owner_obj;

   topSel = (struct SelRec *)malloc(sizeof(struct SelRec));
   if (topSel == NULL) FailAllocMessage();
   topSel->next = NULL;
   topSel->obj = obj_ptr;
   topSel->prev = NULL;
   botSel = topSel;
   UpdSelBBox();

   return topSel;
}

static
int FindVertices(X1, Y1, X2, Y2, TopVSel, BotVSel, pn_struct_spline_msg)
   int X1, Y1, X2, Y2, *pn_struct_spline_msg;
   struct VSelRec **TopVSel, **BotVSel;
   /* X1, Y1, X2, Y2 are absolute coordinates */
{
   struct SelRec *sel_ptr=NULL;
   struct BBRec bbox;

   *TopVSel = *BotVSel = NULL;

   bbox.ltx = X1; bbox.lty = Y1;
   bbox.rbx = X2; bbox.rby = Y2;
   for (sel_ptr=botSel; sel_ptr != NULL; sel_ptr=sel_ptr->prev) {
      int i=0, n=0, count=0, max_count=0, j=0, curved=(-1);
      IntPoint *v=NULL;
      struct ObjRec *obj_ptr=sel_ptr->obj;

      if (obj_ptr->type != OBJ_POLY && obj_ptr->type != OBJ_POLYGON) continue;
      if (!BBoxIntersect(bbox, obj_ptr->bbox)) continue;

      switch (obj_ptr->type) {
      case OBJ_POLY:
         curved = obj_ptr->detail.p->curved;
         if (curved == LT_STRUCT_SPLINE) {
            n = obj_ptr->detail.p->ssn;
            v = obj_ptr->detail.p->ssvlist;
         } else {
            n = obj_ptr->detail.p->n;
            v = obj_ptr->detail.p->vlist;
         }
         break;
      case OBJ_POLYGON:
         curved = obj_ptr->detail.g->curved;
         if (curved == LT_STRUCT_SPLINE) {
            n = obj_ptr->detail.g->ssn;
            v = obj_ptr->detail.g->ssvlist;
         } else {
            n = obj_ptr->detail.g->n;
            v = obj_ptr->detail.g->vlist;
         }
         break;
      }
      if (obj_ptr->ctm == NULL) {
         for (i = 0, count = 0; i < n; i++) {
            if (v[i].x >= X1 && v[i].x <= X2 && v[i].y >= Y1 && v[i].y <= Y2) {
               count++;
            }
         }
      } else {
         for (i = 0, count = 0; i < n; i++) {
            int x, y;

            TransformPointThroughCTM(v[i].x-obj_ptr->x, v[i].y-obj_ptr->y,
                  obj_ptr->ctm, &x, &y);
            if (x+obj_ptr->x >= X1 && x+obj_ptr->x <= X2 &&
                  y+obj_ptr->y >= Y1 && y+obj_ptr->y <= Y2) {
               count++;
            }
         }
      }
      if (count != 0) {
         struct VSelRec *vsel_ptr=NULL;

         if (curved == LT_STRUCT_SPLINE && count > 1) {
            if (!(*pn_struct_spline_msg)) {
               *pn_struct_spline_msg = TRUE;
               Msg(TgLoadString(STID_ONE_V_STRUCT_SPLINE_IN_V_MODE));
            }
            count = 1;
         }
         vsel_ptr = (struct VSelRec *)malloc(sizeof(struct VSelRec));
         if (vsel_ptr == NULL) FailAllocMessage();
         memset(vsel_ptr, 0, sizeof(struct VSelRec));
         vsel_ptr->obj = obj_ptr;
         vsel_ptr->next = *TopVSel;
         vsel_ptr->prev = NULL;
         if (*TopVSel == NULL) {
            *BotVSel = vsel_ptr;
         } else {
            (*TopVSel)->prev = vsel_ptr;
         }
         *TopVSel = vsel_ptr;
         vsel_ptr->n = count;
         max_count = ((count%10) == 0) ? 10*((int)(count/10)) :
               10*((int)(count/10)+1);
         vsel_ptr->max_v = max_count;
         vsel_ptr->v_index = (int*)malloc(max_count*sizeof(int));
         if (vsel_ptr->v_index == NULL) FailAllocMessage();
         vsel_ptr->x = (int*)malloc(max_count*sizeof(int));
         vsel_ptr->y = (int*)malloc(max_count*sizeof(int));
         if (vsel_ptr->x == NULL || vsel_ptr->y == NULL) FailAllocMessage();

         if (obj_ptr->ctm == NULL) {
            for (i = 0, j = 0; i < n; i++) {
               if (v[i].x >= X1 && v[i].x <= X2 &&
                     v[i].y >= Y1 && v[i].y <= Y2) {
                  vsel_ptr->v_index[j] = i;
                  vsel_ptr->x[j] = v[i].x;
                  vsel_ptr->y[j] = v[i].y;
                  j++;
               }
            }
         } else {
            for (i = 0, j = 0; i < n; i++) {
               int x, y;

               TransformPointThroughCTM(v[i].x-obj_ptr->x, v[i].y-obj_ptr->y,
                     obj_ptr->ctm, &x, &y);
               if (x+obj_ptr->x >= X1 && x+obj_ptr->x <= X2 &&
                     y+obj_ptr->y >= Y1 && y+obj_ptr->y <= Y2) {
                  vsel_ptr->v_index[j] = i;
                  vsel_ptr->x[j] = x+obj_ptr->x;
                  vsel_ptr->y[j] = y+obj_ptr->y;
                  j++;
               }
            }
         }
      }
   }
   return (*TopVSel != NULL);
}
 
static
struct SelRec *FindObjects(X1, Y1, X2, Y2, TopSel, BotSel)
   int X1, Y1, X2, Y2;
   struct SelRec **TopSel, **BotSel;
   /* X1, Y1, X2, Y2 are absolute coordinates */
{
   struct ObjRec *obj_ptr=NULL;

   *TopSel = *BotSel = NULL;

   for (obj_ptr=botObj; obj_ptr != NULL; obj_ptr=obj_ptr->prev) {
      obj_ptr->tmp_parent = NULL;
      if (colorLayers && !ObjInVisibleLayer(obj_ptr)) {
         continue;
      }
      if (X1 <= obj_ptr->bbox.ltx && X2 >= obj_ptr->bbox.rbx &&
            Y1 <= obj_ptr->bbox.lty && Y2 >= obj_ptr->bbox.rby) {
         AddObjIntoSel(obj_ptr, NULL, *TopSel, TopSel, BotSel);
      }
   }

   return (*TopSel);
}
 
void SelBox(window, gc, x1, y1, x2, y2)
   Window window;
   GC gc;
   int x1, y1, x2, y2;
{
   XPoint sv[5];

   if (x1 == x2 || y1 == y2) {
      XDrawLine(mainDisplay, window, gc, x1, y1, x2, y2);
   } else {
      sv[0].x = (short)x1; sv[0].y = (short)y1;
      sv[1].x = (short)x1; sv[1].y = (short)y2;
      sv[2].x = (short)x2; sv[2].y = (short)y2;
      sv[3].x = (short)x2; sv[3].y = (short)y1;
      sv[4].x = (short)x1; sv[4].y = (short)y1;
      XDrawLines(mainDisplay, window, gc, sv, 5, CoordModeOrigin);
   }
}

static
struct ObjRec *PtInObjList(XOff, YOff, FirstObjPtr)
   int XOff, YOff;
   struct ObjRec *FirstObjPtr;
   /* XOff and YOff are screen offsets */
{
   register struct ObjRec *obj_ptr, *obj_ptr1;
   register struct AttrRec *attr_ptr;

   for (obj_ptr=FirstObjPtr; obj_ptr != NULL; obj_ptr=obj_ptr->next) {
      for (attr_ptr=obj_ptr->fattr; attr_ptr!=NULL; attr_ptr=attr_ptr->next) {
         if (attr_ptr->shown &&
               XOff >= OFFSET_X(attr_ptr->obj->bbox.ltx)-3 &&
               YOff >= OFFSET_Y(attr_ptr->obj->bbox.lty)-3 &&
               XOff <= OFFSET_X(attr_ptr->obj->bbox.rbx)+3 &&
               YOff <= OFFSET_Y(attr_ptr->obj->bbox.rby)+3) {
            return attr_ptr->obj;
         }
      }
      if (XOff >= OFFSET_X(obj_ptr->bbox.ltx)-3 &&
            YOff >= OFFSET_Y(obj_ptr->bbox.lty)-3 &&
            XOff <= OFFSET_X(obj_ptr->bbox.rbx)+3 &&
            YOff <= OFFSET_Y(obj_ptr->bbox.rby)+3) {
         switch (obj_ptr->type) {
         case OBJ_TEXT:
            if (FindGoodText(XOff,YOff,obj_ptr)) return obj_ptr; break;
         case OBJ_XBM:
            if (FindGoodXBm(XOff,YOff,obj_ptr)) return obj_ptr; break;
         case OBJ_XPM:
            if (FindGoodXPm(XOff,YOff,obj_ptr)) return obj_ptr; break;
         case OBJ_BOX:
            if (FindGoodBox(XOff,YOff,obj_ptr)) return obj_ptr; break;
         case OBJ_OVAL:
            if (FindGoodOval(XOff,YOff,obj_ptr)) return obj_ptr; break;
         case OBJ_POLY:
            if (FindGoodPoly(XOff,YOff,obj_ptr)) return obj_ptr; break;
         case OBJ_POLYGON:
            if (FindGoodPolygon(XOff,YOff,obj_ptr)) return obj_ptr; break;
         case OBJ_ARC:
            if (FindGoodArc(XOff,YOff,obj_ptr)) return obj_ptr; break;
         case OBJ_RCBOX:
            if (FindGoodRCBox(XOff,YOff,obj_ptr)) return obj_ptr; break;

         case OBJ_GROUP:
         case OBJ_SYM:
         case OBJ_ICON:
            obj_ptr1 = PtInObjList(XOff, YOff, obj_ptr->detail.r->first);
            if (obj_ptr1 != NULL) return obj_ptr1;
            break;
         case OBJ_PIN:
            obj_ptr1 = PtInObjList(XOff, YOff,
                  GetPinObj(obj_ptr)->detail.r->first);
            if (obj_ptr1 != NULL) return obj_ptr1;
            break;
         }
      }
   }
   return NULL;
}

static
struct ObjRec *PtInSelected(XOff, YOff)
   int XOff, YOff;
   /* XOff and YOff are screen offsets */
{
   register struct SelRec *sel_ptr;
   register struct ObjRec *obj_ptr, *obj_ptr1;
   register struct AttrRec *attr_ptr;

   for (sel_ptr=topSel; sel_ptr != NULL; sel_ptr=sel_ptr->next) {
      obj_ptr = sel_ptr->obj;

      for (attr_ptr=obj_ptr->fattr; attr_ptr!=NULL; attr_ptr=attr_ptr->next) {
         if (attr_ptr->shown &&
               XOff >= OFFSET_X(attr_ptr->obj->bbox.ltx)-3 &&
               YOff >= OFFSET_Y(attr_ptr->obj->bbox.lty)-3 &&
               XOff <= OFFSET_X(attr_ptr->obj->bbox.rbx)+3 &&
               YOff <= OFFSET_Y(attr_ptr->obj->bbox.rby)+3) {
            return attr_ptr->obj;
         }
      }
      if (XOff >= OFFSET_X(obj_ptr->bbox.ltx)-3 &&
            YOff >= OFFSET_Y(obj_ptr->bbox.lty)-3 &&
            XOff <= OFFSET_X(obj_ptr->bbox.rbx)+3 &&
            YOff <= OFFSET_Y(obj_ptr->bbox.rby)+3) {
         switch (obj_ptr->type) {
         case OBJ_TEXT:
            if (FindGoodText(XOff,YOff,obj_ptr)) return obj_ptr; break;
         case OBJ_XBM:
            if (FindGoodXBm(XOff,YOff,obj_ptr)) return obj_ptr; break;
         case OBJ_XPM:
            if (FindGoodXPm(XOff,YOff,obj_ptr)) return obj_ptr; break;
         case OBJ_BOX:
            if (FindGoodBox(XOff,YOff,obj_ptr)) return obj_ptr; break;
         case OBJ_OVAL:
            if (FindGoodOval(XOff,YOff,obj_ptr)) return obj_ptr; break;
         case OBJ_POLY:
            if (FindGoodPoly(XOff,YOff,obj_ptr)) return obj_ptr; break;
         case OBJ_POLYGON:
            if (FindGoodPolygon(XOff,YOff,obj_ptr)) return obj_ptr; break;
         case OBJ_ARC:
            if (FindGoodArc(XOff,YOff,obj_ptr)) return obj_ptr; break;
         case OBJ_RCBOX:
            if (FindGoodRCBox(XOff,YOff,obj_ptr)) return obj_ptr; break;

         case OBJ_GROUP:
         case OBJ_ICON:
         case OBJ_SYM:
            obj_ptr1 = PtInObjList(XOff, YOff, obj_ptr->detail.r->first);
            if (obj_ptr1 != NULL) return obj_ptr1;
            break;
         case OBJ_PIN:
            obj_ptr1 = PtInObjList(XOff, YOff,
                  GetPinObj(obj_ptr)->detail.r->first);
            if (obj_ptr1 != NULL) return obj_ptr1;
            break;
         }
      }
   }
   return NULL;
}

static
void ToggleVertexSelection(ObjPtr, VIndex, AbsX, AbsY, pn_struct_spline_msg)
   struct ObjRec *ObjPtr;
   int VIndex, AbsX, AbsY, *pn_struct_spline_msg;
{
   int i, j, n=0, curved=(-1), last_polygon_vertex=FALSE;
   struct VSelRec *vsel_ptr=NULL;
   char *smooth=NULL;

   switch (ObjPtr->type) {
   case OBJ_POLY:
      curved = ObjPtr->detail.p->curved;
      if (curved == LT_STRUCT_SPLINE) {
         smooth = ObjPtr->detail.p->ssmooth;
      } else {
         smooth = ObjPtr->detail.p->smooth;
      }
      break;
   case OBJ_POLYGON:
      curved = ObjPtr->detail.g->curved;
      if (curved == LT_STRUCT_SPLINE) {
         smooth = ObjPtr->detail.g->ssmooth;
         if (VIndex == ObjPtr->detail.g->ssn-1) {
            last_polygon_vertex = TRUE;
         }
      } else {
         smooth = ObjPtr->detail.g->smooth;
         if (VIndex == ObjPtr->detail.g->n-1) {
            last_polygon_vertex = TRUE;
         }
      }
      break;
   }
   if (VertexAlreadySelected(ObjPtr, VIndex, &vsel_ptr)) {
      /* de-select a vertex */
      if (vsel_ptr->n == 1) {
         if (vsel_ptr->prev == NULL) {
            topVSel = vsel_ptr->next;
         } else {
            vsel_ptr->prev->next = vsel_ptr->next;
         }
         if (vsel_ptr->next == NULL) {
            botVSel = vsel_ptr->prev;
         } else {
            vsel_ptr->next->prev = vsel_ptr->prev;
         }
         free(vsel_ptr->v_index);
         free(vsel_ptr->x);
         free(vsel_ptr->y);
         free(vsel_ptr);
      } else {
         for (j = 0; j < vsel_ptr->n; j++) {
            if (vsel_ptr->v_index[j] == VIndex) {
               break;
            }
         }
         if (j > vsel_ptr->n) {
            sprintf(gszMsgBox, TgLoadString(STID_INCONSIS_VERTEX_SEL_IN_FUNC),
                  "ToggleVertexSelection()");
            TgAssert(FALSE, gszMsgBox, NULL);
         }
         for (i=j; i < vsel_ptr->n-1; i++) {
            vsel_ptr->v_index[i] = vsel_ptr->v_index[i+1];
            vsel_ptr->x[i] = vsel_ptr->x[i+1];
            vsel_ptr->y[i] = vsel_ptr->y[i+1];
         }
         vsel_ptr->n--;
      }
   } else {
      if (vsel_ptr == NULL) {
         /* a new object */
         vsel_ptr = (struct VSelRec *)malloc(sizeof(struct VSelRec));
         if (vsel_ptr == NULL) FailAllocMessage();
         memset(vsel_ptr, 0, sizeof(struct VSelRec));
         vsel_ptr->obj = ObjPtr;
         n = vsel_ptr->n = 1;
         vsel_ptr->max_v = 10;
         vsel_ptr->v_index = (int*)malloc(10*sizeof(int));
         if (vsel_ptr->v_index == NULL) FailAllocMessage();
         vsel_ptr->x = (int*)malloc(10*sizeof(int));
         vsel_ptr->y = (int*)malloc(10*sizeof(int));
         if (vsel_ptr->x == NULL || vsel_ptr->y == NULL) FailAllocMessage();

         vsel_ptr->prev = NULL;
         vsel_ptr->next = topVSel;
         if (topVSel == NULL) {
            botVSel = vsel_ptr;
         } else {
            topVSel->prev = vsel_ptr;
         }
         topVSel = vsel_ptr;
      } else {
         if (!last_polygon_vertex && curved == LT_STRUCT_SPLINE) {
            if (!(*pn_struct_spline_msg)) {
               *pn_struct_spline_msg = TRUE;
               Msg(TgLoadString(STID_ONE_V_STRUCT_SPLINE_IN_V_MODE));
            }
            return;
         } else {
            n = ++(vsel_ptr->n);
            if (n > vsel_ptr->max_v) {
               int max_v;

               vsel_ptr->max_v += 10;
               max_v = vsel_ptr->max_v;
               vsel_ptr->v_index = (int*)realloc(vsel_ptr->v_index,
                     sizeof(int)*max_v);
               vsel_ptr->x = (int*)realloc(vsel_ptr->x, sizeof(int)*max_v);
               vsel_ptr->y = (int*)realloc(vsel_ptr->y, sizeof(int)*max_v);
            }
         }
      }
      vsel_ptr->v_index[n-1] = VIndex;
      vsel_ptr->x[n-1] = AbsX;
      vsel_ptr->y[n-1] = AbsY;
   }
   if (!last_polygon_vertex) {
      if (curved != LT_INTSPLINE && curved != (-1) && smooth != NULL) {
         if (smooth[VIndex]) {
            if (curved == LT_STRUCT_SPLINE) {
               MARKHO(drawWindow, revDefaultGC, OFFSET_X(AbsX), OFFSET_Y(AbsY));
            } else {
               MARKO(drawWindow, revDefaultGC, OFFSET_X(AbsX), OFFSET_Y(AbsY));
            }
         } else {
            if (curved == LT_STRUCT_SPLINE) {
               MARKHR(drawWindow, revDefaultGC, OFFSET_X(AbsX), OFFSET_Y(AbsY));
            } else {
               MARK(drawWindow, revDefaultGC, OFFSET_X(AbsX), OFFSET_Y(AbsY));
            }
         }
      } else {
         MARK(drawWindow, revDefaultGC, OFFSET_X(AbsX), OFFSET_Y(AbsY));
      }
      MARKV(drawWindow, revDefaultGC, OFFSET_X(AbsX), OFFSET_Y(AbsY));
   }
}

static
void ToggleSelectedObjIfSelectedAlready(ObjPtr)
   register struct ObjRec *ObjPtr;
{
   register struct SelRec *sel_ptr;

   if ((sel_ptr=AlreadySelected(ObjPtr)) != NULL) {
      /* de-select an object */
      HighLightAnObj(ObjPtr);

      if (sel_ptr->prev == NULL) {
         topSel = sel_ptr->next;
      } else {
         sel_ptr->prev->next = sel_ptr->next;
      }
      if (sel_ptr->next == NULL) {
         botSel = sel_ptr->prev;
      } else {
         sel_ptr->next->prev = sel_ptr->prev;
      }
      free(sel_ptr);
      numObjSelected--;
   } else {
      /* add a newly selected object */
      AddNewSelObj(ObjPtr);
      HighLightAnObj(ObjPtr);
   }
}

static
void ContinueSel(XOff, YOff, ShiftKeyDown)
   int XOff, YOff, ShiftKeyDown;
   /* XOff and YOff are screen offsets, and they are not on grid */
{
   int i, end_x, end_y, v_index, new_end_x, new_end_y;
   int done=FALSE, ltx, lty, rbx, rby, dx, dy, x, y;
   int struct_spline_msg=FALSE;
   char buf[80], w_buf[80], h_buf[80], x_buf[80], y_buf[80];
   XEvent input, ev;
   XMotionEvent *motion_ev=NULL;
   struct SelRec *sel_ptr=NULL, *top_sel_ptr=NULL, *bot_sel_ptr=NULL;
   struct VSelRec *vsel_ptr=NULL, *top_vsel_ptr=NULL, *bot_vsel_ptr=NULL;
   struct ObjRec *obj_ptr=NULL, *owner_obj=NULL;

   end_x = XOff;
   end_y = YOff; 

   SelBox(drawWindow, revDefaultGC, XOff, YOff, end_x, end_y);
   PixelToMeasurementUnit(w_buf, 0);
   PixelToMeasurementUnit(h_buf, 0);
   PixelToMeasurementUnit(x_buf, ABS_X(end_x));
   PixelToMeasurementUnit(y_buf, ABS_Y(end_y));
   sprintf(buf, "w=%s\nh=%s\nx=%s\ny=%s", w_buf, h_buf, x_buf, y_buf);
   StartShowMeasureCursor(end_x, end_y, buf, TRUE);
   if (!debugNoPointerGrab) {
      XGrabPointer(mainDisplay, drawWindow, False,
            PointerMotionMask | ButtonReleaseMask,
            GrabModeAsync, GrabModeAsync, None, handCursor, CurrentTime);
   }
   while (!done) {
      XNextEvent(mainDisplay, &input);

      if (input.type == Expose || input.type == VisibilityNotify) {
         ExposeEventHandler(&input, TRUE);
      } else if (input.type == ButtonRelease) {
         XUngrabPointer(mainDisplay, CurrentTime);
         PixelToMeasurementUnit(w_buf, ABS_SIZE(abs(end_x-XOff)));
         PixelToMeasurementUnit(h_buf, ABS_SIZE(abs(end_y-YOff)));
         PixelToMeasurementUnit(x_buf, ABS_X(end_x));
         PixelToMeasurementUnit(y_buf, ABS_Y(end_y));
         sprintf(buf, "w=%s\nh=%s\nx=%s\ny=%s", w_buf, h_buf, x_buf, y_buf);
         EndShowMeasureCursor(end_x, end_y, buf, TRUE);
         SelBox(drawWindow, revDefaultGC, XOff, YOff, end_x, end_y);
         done = TRUE;
      } else if (input.type == MotionNotify) {
         motion_ev = &(input.xmotion);
         new_end_x = motion_ev->x;
         new_end_y = motion_ev->y;

         PixelToMeasurementUnit(w_buf, ABS_SIZE(abs(end_x-XOff)));
         PixelToMeasurementUnit(h_buf, ABS_SIZE(abs(end_y-YOff)));
         PixelToMeasurementUnit(x_buf, ABS_X(end_x));
         PixelToMeasurementUnit(y_buf, ABS_Y(end_y));
         sprintf(buf, "w=%s\nh=%s\nx=%s\ny=%s", w_buf, h_buf, x_buf, y_buf);
         ShowMeasureCursor(end_x, end_y, buf, TRUE);
         SelBox(drawWindow, revDefaultGC, XOff, YOff, end_x, end_y);
         end_x = new_end_x; end_y = new_end_y;
         SelBox(drawWindow, revDefaultGC, XOff, YOff, end_x, end_y);
         PixelToMeasurementUnit(w_buf, ABS_SIZE(abs(end_x-XOff)));
         PixelToMeasurementUnit(h_buf, ABS_SIZE(abs(end_y-YOff)));
         PixelToMeasurementUnit(x_buf, ABS_X(end_x));
         PixelToMeasurementUnit(y_buf, ABS_Y(end_y));
         sprintf(buf, "w=%s\nh=%s\nx=%s\ny=%s", w_buf, h_buf, x_buf, y_buf);
         ShowMeasureCursor(end_x, end_y, buf, TRUE);

         MarkRulers(end_x, end_y);
         while (XCheckMaskEvent(mainDisplay, PointerMotionMask, &ev)) ;
      }
   }

   dx = abs(XOff - end_x);
   dy = abs(YOff - end_y);
   if (curChoice == VERTEXMODE) {
      if (dx <= 2 && dy <= 2) {
         if (topSel == NULL) {
            if (SelectOneObj(XOff, YOff, NULL) != NULL) {
               if ((topSel->obj->type == OBJ_POLY ||
                     topSel->obj->type == OBJ_POLYGON) &&
                     !topSel->obj->locked) {
                  HighLightForward();
               } else {
                  RemoveAllSel();
               }
            }
         } else if (ShiftKeyDown) {
            obj_ptr = FindAVertex(XOff, YOff, &v_index, &x, &y);
            if (obj_ptr != NULL) {
               ToggleVertexSelection(obj_ptr, v_index, x, y, 
                     &struct_spline_msg);
               if (obj_ptr->type==OBJ_POLYGON && v_index==0) {
                  if (obj_ptr->detail.g->curved == LT_STRUCT_SPLINE) {
                     ToggleVertexSelection(obj_ptr,obj_ptr->detail.g->ssn-1,
                           x, y, &struct_spline_msg);
                  } else {
                     ToggleVertexSelection(obj_ptr,obj_ptr->detail.g->n-1,
                           x, y, &struct_spline_msg);
                  }
               }
               UpdSelBBox();
            }
         } else {
            HighLightReverse();
            SelectOneVertex(XOff, YOff);
            HighLightForward();
         }
      } else {
         CalcBBox(XOff, YOff, end_x, end_y, &ltx, &lty, &rbx, &rby);
         if (topSel == NULL) {
            if (FindObjects(ABS_X(ltx), ABS_Y(lty), ABS_X(rbx), ABS_Y(rby),
                  &top_sel_ptr, &bot_sel_ptr) != NULL) {
               topSel = top_sel_ptr;
               botSel = bot_sel_ptr;
               UnSelNonVertexObjs(FALSE, TRUE); /* do not highlight */
               UpdSelBBox();
               HighLightForward();
            }
         } else if (ShiftKeyDown) {
            if (FindVertices(ABS_X(ltx), ABS_Y(lty), ABS_X(rbx), ABS_Y(rby),
                  &top_vsel_ptr, &bot_vsel_ptr, &struct_spline_msg)) {
               struct VSelRec *next_vsel;

               for (vsel_ptr=top_vsel_ptr; vsel_ptr!=NULL; vsel_ptr=next_vsel) {
                  obj_ptr = vsel_ptr->obj;
                  for (i = 0; i < vsel_ptr->n; i++) {
                     ToggleVertexSelection(obj_ptr, vsel_ptr->v_index[i],
                           vsel_ptr->x[i], vsel_ptr->y[i], &struct_spline_msg);
                  }
                  next_vsel = vsel_ptr->next;
                  free(vsel_ptr->v_index);
                  free(vsel_ptr->x);
                  free(vsel_ptr->y);
                  free(vsel_ptr);
               }
               UpdSelBBox();
            }
         } else {
            HighLightReverse();
            JustRemoveAllVSel();
            if (FindVertices(ABS_X(ltx), ABS_Y(lty), ABS_X(rbx), ABS_Y(rby),
                  &top_vsel_ptr, &bot_vsel_ptr, &struct_spline_msg)) {
               topVSel = top_vsel_ptr;
               botVSel = bot_vsel_ptr;
               UpdSelBBox();
            }
            HighLightForward();
         }
      }
   } else {
      if (dx <= 2 && dy <= 2) {
         if (ShiftKeyDown) {
            obj_ptr = FindAnObj(XOff, YOff, &owner_obj, NULL, NULL);
            if (obj_ptr != NULL) {
               if (owner_obj != NULL) obj_ptr = owner_obj;

               ToggleSelectedObjIfSelectedAlready(obj_ptr);
               UpdSelBBox();
            }
         } else {
            if (topSel != NULL) HighLightReverse();

            if (SelectOneObj(XOff, YOff, NULL) != NULL) {
               HighLightForward();
            }
         }
      } else {
         CalcBBox(XOff, YOff, end_x, end_y, &ltx, &lty, &rbx, &rby);
         if (ShiftKeyDown) {
            if (FindObjects(ABS_X(ltx), ABS_Y(lty), ABS_X(rbx), ABS_Y(rby),
                  &top_sel_ptr, &bot_sel_ptr) != NULL) {
               struct SelRec *next_sel;

               for (sel_ptr=top_sel_ptr; sel_ptr!=NULL; sel_ptr=next_sel) {
                  next_sel = sel_ptr->next;
                  obj_ptr = sel_ptr->obj;
                  ToggleSelectedObjIfSelectedAlready(obj_ptr);
                  free(sel_ptr);
               }
               UpdSelBBox();
            }
         } else {
            if (topSel != NULL) HighLightReverse();
            RemoveAllSel();
            if (FindObjects(ABS_X(ltx), ABS_Y(lty), ABS_X(rbx), ABS_Y(rby),
                  &top_sel_ptr, &bot_sel_ptr) != NULL) {
               topSel = top_sel_ptr;
               botSel = bot_sel_ptr;
               UpdSelBBox();
               HighLightForward();
            }
         }
      }
   }
}

static XComposeStatus	c_stat;

static
void KeyPressInSelect(key_ev)
   XKeyEvent *key_ev;
{
   XButtonEvent button_ev;
   char s[80];
   KeySym key_sym;
   int delta, dx=0, dy=0, has_ch=FALSE;

   has_ch = XLookupString(key_ev, s, sizeof(s), &key_sym, &c_stat);
   TranslateKeys(s, &key_sym);
   if (inSlideShow) {
      if (CharIsESC(key_ev, s, key_sym, &has_ch)) {
         LeaveSlideShow();
         return;
      }
      switch (key_sym) {
      case XK_Left: PrevSlide(); return;
      case XK_KP_Left: PrevSlide(); return;
      case XK_Up: PrevSlide(); return;
      case XK_KP_Up: PrevSlide(); return;
      case XK_Right: NextSlide(); return;
      case XK_KP_Right: NextSlide(); return;
      case XK_Down: NextSlide(); return;
      case XK_KP_Down: NextSlide(); return;
      default: break;
      }
   }
   button_ev.state = ShiftMask;
   if (key_sym == XK_Page_Up || key_sym == XK_KP_Page_Up) {
      ScrollUp(&button_ev);
   } else if (key_sym == XK_Page_Down || key_sym == XK_KP_Page_Down) {
      ScrollDown(&button_ev);
   }
   if (topSel==NULL && topVSel==NULL) {
      if (key_ev->state & ControlMask) {
         switch (key_sym) {
         case XK_Left: ScrollLeft(&button_ev); break;
         case XK_KP_Left: ScrollLeft(&button_ev); break;
         case XK_Up: ScrollUp(&button_ev); break;
         case XK_KP_Up: ScrollUp(&button_ev); break;
         case XK_Right: ScrollRight(&button_ev); break;
         case XK_KP_Right: ScrollRight(&button_ev); break;
         case XK_Down: ScrollDown(&button_ev); break;
         case XK_KP_Down: ScrollDown(&button_ev); break;
         }
      } else {
         switch (key_sym) {
         case XK_Left: ScrollLeft(NULL); break;
         case XK_KP_Left: ScrollLeft(NULL); break;
         case XK_Up: ScrollUp(NULL); break;
         case XK_KP_Up: ScrollUp(NULL); break;
         case XK_Right: ScrollRight(NULL); break;
         case XK_KP_Right: ScrollRight(NULL); break;
         case XK_Down: ScrollDown(NULL); break;
         case XK_KP_Down: ScrollDown(NULL); break;
         }
      }
      return;
   }
   if (CharIsDEL(key_ev, s, key_sym, &has_ch)) {
      DelAllSelObj();
   }
   if (key_sym!=XK_Left && key_sym!=XK_Up && key_sym!=XK_Right &&
         key_sym!=XK_Down && key_sym!=XK_KP_Left && key_sym!=XK_KP_Up &&
         key_sym!=XK_KP_Right && key_sym!=XK_KP_Down) {
      return;
   }

   if (snapOn) {
      delta = (gridSystem==ENGLISH_GRID) ? GRID_ABS_SIZE(xyEnglishGrid) :
            GRID_ABS_SIZE(xyMetricGrid);
   } else {
      delta = GRID_ABS_SIZE(1);
   }
   HighLightReverse();
   switch (key_sym) {
   case XK_Left:     dx = -delta; dy = 0; break;
   case XK_KP_Left:  dx = -delta; dy = 0; break;
   case XK_Up:       dx = 0;      dy = -delta; break;
   case XK_KP_Up:    dx = 0;      dy = -delta; break;
   case XK_Right:    dx = delta;  dy = 0; break;
   case XK_KP_Right: dx = delta;  dy = 0; break;
   case XK_Down:     dx = 0;      dy = delta; break;
   case XK_KP_Down:  dx = 0;      dy = delta; break;
   }
   if (curChoice == VERTEXMODE) {
      MoveAllSelVs(dx, dy);
   } else if (numObjSelected == numObjLocked) {
      HighLightForward();
      return;
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

static
int PtInObjAboveSelected(XOff, YOff)
   int XOff, YOff;
   /*
    * returns TRUE if there is an object under the mouse that's above all the
    *       selected objects -- in this case, that object should be selected
    */
{
   struct ObjRec *obj_ptr=NULL, *owner_obj=NULL, *found_obj=NULL;

   found_obj = FindAnObj(XOff, YOff, &owner_obj, NULL, NULL);
   if (found_obj == NULL) {
      /* no object under the cursor */
      return FALSE;
   }
   if (owner_obj != NULL) found_obj = owner_obj;
   if (topSel == NULL) {
      return TRUE;
   }
   for (obj_ptr=topObj; obj_ptr != NULL && obj_ptr != topSel->obj;
         obj_ptr=obj_ptr->next) {
      if (obj_ptr == found_obj) {
         return TRUE;
      }
   }
   return FALSE;
}

void Select(input)
   XEvent *input;
{
   register int i;
   XButtonEvent *button_ev;
   struct SelRec *sel_ptr;
   struct VSelRec *vsel_ptr;
   struct ObjRec *obj_ptr;
   Time click_time;

   if (input->type == KeyPress) {
      KeyPressInSelect(&(input->xkey));
      return;
   } else if (input->type != ButtonPress) {
      return;
   }

   button_ev = &(input->xbutton);
   if (button_ev->button == Button1) {
      static Time sSelectLastClickTime=(Time)0;
      static int snSelectJustClicked=FALSE;

      int mouse_x, mouse_y, grid_x, grid_y, corner;

      mouse_x = button_ev->x;
      mouse_y = button_ev->y;
      GridXY(mouse_x, mouse_y, &grid_x, &grid_y);

      click_time = button_ev->time;
      if (curChoice==VERTEXMODE && topSel!=NULL && snSelectJustClicked &&
            (click_time-sSelectLastClickTime) < doubleClickInterval) {
         snSelectJustClicked = FALSE;
         HighLightReverse();
         RemoveAllSel();
         return;
      } else if (curChoice==NOTHING && topSel!=NULL && snSelectJustClicked &&
            (click_time-sSelectLastClickTime) < doubleClickInterval) {
         snSelectJustClicked = FALSE;
         Teleport(button_ev);
         return;
      }
      snSelectJustClicked = TRUE;
      sSelectLastClickTime = click_time;

      if (button_ev->state & (ShiftMask | ControlMask)) {
         ContinueSel(mouse_x, mouse_y, TRUE);
         justDupped = FALSE;
         return;
      } else if (curChoice == VERTEXMODE && topVSel != NULL) {
         int found=FALSE;

         for (vsel_ptr=topVSel; vsel_ptr!=NULL && !found;
               vsel_ptr=vsel_ptr->next) {
            for (i = 0; i < vsel_ptr->n; i++) {
               if (PtInMark(mouse_x, mouse_y, OFFSET_X(vsel_ptr->x[i]),
                     OFFSET_Y(vsel_ptr->y[i]))) {
                  found = TRUE;
                  break;
               }
            }
         }
         if (found) {
            MoveSelVs(grid_x, grid_y);
            return;
         }
      } else if (curChoice == NOTHING) {
         struct ObjRec *inner_obj=NULL;

         if (topSel != NULL) {
            if (oneMotionSelectMove &&
                  PtInSelMark(mouse_x, mouse_y, &corner) == NULL &&
                  PtInSelected(mouse_x, mouse_y) == NULL) {
               /* may be pointing in not already selected object */
               HighLightReverse();
               RemoveAllSel();
               if (SelectOneObj(mouse_x, mouse_y, &inner_obj) != NULL) {
                  HighLightForward();
                  MoveSel(grid_x, grid_y,
                        (inner_obj==NULL ? topSel->obj : inner_obj), button_ev);
                  return;
               }
            } else if ((sel_ptr=PtInSelMark(mouse_x,mouse_y,&corner)) != NULL) {
               StretchSel(grid_x, grid_y, sel_ptr->obj, corner);
               return;
            } else if (!PtInObjAboveSelected(mouse_x, mouse_y) &&
                  (obj_ptr=PtInSelected(mouse_x, mouse_y)) != NULL) {
               MoveSel(grid_x, grid_y, obj_ptr, button_ev);
               return;
            }
         } else if (oneMotionSelectMove &&
               SelectOneObj(mouse_x, mouse_y, &inner_obj) != NULL) {
            HighLightForward();
            MoveSel(grid_x, grid_y,
                  (inner_obj==NULL ? topSel->obj : inner_obj), button_ev);
            return;
         }
      } else if (curChoice == ROTATEMODE && topSel != NULL) {
         if ((sel_ptr=PtInSelMark(mouse_x, mouse_y, &corner)) != NULL) {
            RotateShearSel(grid_x, grid_y, sel_ptr->obj, corner);
            return;
         }
         if (!autoRotatePivot && PtInRotatePivot(mouse_x, mouse_y)) {
            ContinueMoveRotatePivot(grid_x, grid_y);
            return;
         }
      }
      ContinueSel(mouse_x, mouse_y, FALSE);
      justDupped = FALSE;
   }
}

struct AttrRec *FindFileAttrWithName(AttrName)
   char *AttrName;
{
   struct AttrRec *attr_ptr, *found_attr=NULL;
   int count=1, compare_name=(strchr(AttrName,'=') != NULL);

   if (tgifObj == NULL) return NULL;

   for (attr_ptr=tgifObj->fattr; attr_ptr != NULL; attr_ptr=attr_ptr->next) {
      if (compare_name) {
         if (strcmp(attr_ptr->attr_name.s, AttrName) == 0) {
            found_attr = attr_ptr;
            break;
         }
      } else {
         if (strcmp(attr_ptr->attr_value.s, AttrName) == 0) {
            found_attr = attr_ptr;
            break;
         }
      }
   }
   if (attr_ptr == NULL) return NULL;

   if (GetTextObjFirstStrSeg(found_attr->obj)->color == colorIndex) {
      return found_attr;
   }
   for (attr_ptr=found_attr->next; attr_ptr!=NULL; attr_ptr=attr_ptr->next) {
      if (compare_name) {
         if (strcmp(attr_ptr->attr_name.s, AttrName) == 0) {
            if (GetTextObjFirstStrSeg(attr_ptr->obj)->color == colorIndex) {
               break;
            } else {
               count++;
            }
         }
      } else {
         if (strcmp(attr_ptr->attr_value.s, AttrName) == 0) {
            if (GetTextObjFirstStrSeg(attr_ptr->obj)->color == colorIndex) {
               break;
            } else {
               count++;
            }
         }
      }
   }
   if (attr_ptr != NULL) {
      found_attr = attr_ptr;
   } else if (count != 1) {
      sprintf(gszMsgBox,
            TgLoadCachedString(CSTID_CANT_FIND_FILEATTR_WITH_COLOR),
            AttrName, colorMenuItems[colorIndex]);
      Msg(gszMsgBox);
      return NULL;
   }
   return found_attr;
}

struct AttrRec *FindAttrWithName(ObjPtr, AttrName, pp_top_owner)
   struct ObjRec *ObjPtr, **pp_top_owner;
   char *AttrName;
{
   struct ObjRec *obj_ptr=NULL;
   char *dot_ptr=NULL;

   if (AttrName[0] == '!' && AttrName[1] == '*') {
      if (topSel == NULL) {
         return NULL;
      }
      if (pp_top_owner != NULL) *pp_top_owner = topSel->obj;
      return FindObjAttrWithName(topSel->obj, &AttrName[2]);
   }
   if ((dot_ptr=strchr(AttrName, '.')) == NULL) {
      if (pp_top_owner != NULL) *pp_top_owner = ObjPtr;
      return FindObjAttrWithName(ObjPtr, AttrName);
   }
   *dot_ptr = '\0';
   if (strcmp(AttrName, "!") == 0) {
      *dot_ptr++ = '.';
      if (pp_top_owner != NULL) *pp_top_owner = tgifObj;
      return FindFileAttrWithName(dot_ptr);
   }
   if (AttrName[0] == '#' && AttrName[1] == '#') {
      int obj_page_num=0;
      char *bang_ptr=strchr(&AttrName[2], '!');

      if (bang_ptr == NULL) {
         *dot_ptr = '.';
         return NULL;
      }
      *bang_ptr = '\0';
      if (sscanf(&AttrName[2], "%d", &obj_page_num) != 1) {
         *bang_ptr = '!';
         *dot_ptr = '.';
         return NULL;
      }
      if (obj_page_num < 1 || obj_page_num > lastPageNum) {
         sprintf(gszMsgBox, TgLoadString(STID_INVALID_PAGE_NUM),
               &AttrName[2]),
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
         *bang_ptr = '!';
         *dot_ptr = '.';
         return NULL;
      }
      *bang_ptr = '!';
      if (curPageNum == obj_page_num) {
         if ((obj_ptr=FindObjWithName(botObj, ObjPtr, bang_ptr, FALSE,
               FALSE, NULL, pp_top_owner)) == NULL) {
            *dot_ptr = '.';
            return NULL;
         }
      } else {
         struct PageRec *page_ptr=NULL;
         int page_num=0;

         for (page_ptr=firstPage, page_num=1; page_ptr != NULL;
               page_ptr=page_ptr->next, page_num++) {
            if (page_num == obj_page_num) {
               struct PageRec *saved_cur_page_ptr=curPage;
               int saved_cur_page_num=curPageNum;

               curPageNum = page_num;
               curPage = page_ptr;
               topObj = curPage->top;
               botObj = curPage->bot;
               obj_ptr = FindObjWithName(botObj, NULL, bang_ptr,
                     FALSE, FALSE, NULL, NULL);
               curPageNum = saved_cur_page_num;
               curPage = saved_cur_page_ptr;
               topObj = curPage->top;
               botObj = curPage->bot;

               if (obj_ptr == NULL) {
                  *dot_ptr = '.';
                  return NULL;
               }
               break;
            }
         }
         if (page_ptr == NULL) {
            *dot_ptr++ = '.';
            return NULL;
         }
      }
   } else {
      if ((obj_ptr=FindObjWithName(botObj, ObjPtr, AttrName, FALSE,
            FALSE, NULL, pp_top_owner)) == NULL) {
         *dot_ptr++ = '.';
         return NULL;
      }
   }
   *dot_ptr++ = '.';
   return FindObjAttrWithName(obj_ptr, dot_ptr);
}

struct AttrRec *ValidAttrArg(c_ptr, obj_ptr, new_c_ptr)
   char *c_ptr, **new_c_ptr;
   struct ObjRec *obj_ptr;
{
   char name[MAXSTRING+1], *name_ptr;
   struct AttrRec *attr_ptr;

   name_ptr = name;
   if (c_ptr[0] == '$' && c_ptr[1] == '(') {
      for (c_ptr = &c_ptr[2]; *c_ptr != '\0'; c_ptr++) {
         switch (*c_ptr) {
         case '\\':
            c_ptr++;
            *name_ptr++ = *c_ptr;
            break;
         case ')':
            *name_ptr++ = '=';
            *name_ptr = '\0';
            *new_c_ptr = c_ptr;
            attr_ptr = FindAttrWithName(obj_ptr, name, NULL);
            if (attr_ptr == NULL) {
               char msg[MAXSTRING+1];

               sprintf(msg, TgLoadCachedString(CSTID_CANT_FIND_NAMED_ATTR),
                     name);
               Msg(msg);
            }
            return attr_ptr;
         default: *name_ptr++ = *c_ptr; break;
         }
      }
   }
   return NULL;
}

int DoTeleport(teleport_attr)
   struct AttrRec *teleport_attr;
{
   char file_name[MAXPATHLENGTH+1], msg[MAXSTRING+1], *page_spec=NULL;
   char *dest_ptr=NULL;
   int do_not_save=FALSE, rc=TRUE, just_goto_page=FALSE, referer_set=FALSE;

   while (!DirIsRemote(curDir) && fileModified && !IsFiletUnSavable()) {
      XBell(mainDisplay, 0);
      switch (MsgBox(TgLoadString(STID_FILE_MOD_SAVE_BEFORE_OPEN), TOOL_NAME,
            YNC_MB)) {
      case MB_ID_YES: SaveFile(); break;
      case MB_ID_NO: do_not_save = TRUE; SetFileModified(FALSE); break;
      case MB_ID_CANCEL: return FALSE;
      }
   }
   if (!DirIsRemote(curDir) && fileModified && IsFiletUnSavable()) {
      do_not_save = TRUE;
      SetFileModified(FALSE);
   }
   if (!FormNewFileName(curDir, teleport_attr->attr_value.s,
         (strcmp(teleport_attr->attr_name.s,TELEPORT_ATTR)==0 ? OBJ_FILE_EXT :
         NULL), file_name, &page_spec)) {
      sprintf(msg, TgLoadString(STID_INVALID_NAMED_TELEPORT_DEST),
            teleport_attr->attr_value.s);
      MsgBox(msg, TOOL_NAME, INFO_MB);
      if (do_not_save) SetFileModified(TRUE);
      rc = FALSE;
   }
   if (*teleport_attr->attr_value.s == '#') just_goto_page = TRUE;

   if (rc && page_spec != NULL && just_goto_page) {
      int new_page_num=(-1);

      if (!GetPageNumFromPageSpec(page_spec, &new_page_num)) {
         sprintf(msg, TgLoadString(STID_INVALID_NAMED_TELEPORT_DEST),
               teleport_attr->attr_value.s);
         MsgBox(msg, TOOL_NAME, INFO_MB);
         if (do_not_save) SetFileModified(TRUE);
         rc = FALSE;
      } else if (new_page_num != curPageNum) {
         BeforeNavigate();
         GotoPageNum(new_page_num);
         ShowPage();
         ClearAndRedrawDrawWindow();
         RedrawTitleWindow();
         RedrawRulers();
         RedrawScrollBars();
         CleanUpCmds();
         CommitNavigate();
         justDupped = FALSE;
      }
      if (page_spec != NULL) free(page_spec);
      return rc;
   }
   if (!rc) {
      if (page_spec != NULL) free(page_spec);
      return rc;
   }
   MakeQuiescent();

   if (curFileDefined) {
      referer_set = TRUE;
      if (*curSymDir == '\0') {
         sprintf(gszMsgBox, "%s%c%s", curDir, DIR_SEP, curFileName);
      } else {
         sprintf(gszMsgBox, "%s%c%s", curSymDir, DIR_SEP, curFileName);
      }
      HttpSetReferer(gszMsgBox);
   }
   dest_ptr = UtilStrDup(teleport_attr->attr_value.s);
   if (dest_ptr == NULL) FailAllocMessage();
   if (FileIsRemote(file_name)) {
      char *buf=NULL, *content_type=NULL, final_url[MAXPATHLENGTH+1];
      int buf_sz=0, is_html=FALSE;

      *final_url = '\0';
      SetWatchCursor(drawWindow);
      SetWatchCursor(mainWindow);
      SaveStatusStrings();
      rc = LoadRemoteFileInMem(file_name, &buf, &content_type, &buf_sz,
            &is_html, FALSE, final_url, sizeof(final_url));
      RestoreStatusStrings();
      SetDefaultCursor(mainWindow);
      ShowCursor();
      if (rc && buf != NULL) {
         if (*final_url != '\0') {
            UtilStrCpyN(file_name, sizeof(file_name), final_url);
         }
         LoadRemoteFileFromMem(file_name, buf, content_type, buf_sz, is_html);
      } else {
         if (do_not_save) SetFileModified(TRUE);
         rc = FALSE;
      }
      if (content_type != NULL) FreeRemoteBuf(content_type);
      if (buf != NULL) FreeRemoteBuf(buf);
   } else {
      int obj_file=FALSE, gzipped=FALSE;
      FILE *fp=fopen(file_name, "r");

      if (fp == NULL) {
         sprintf(gszMsgBox, TgLoadString(STID_Q_FILE_NOT_EXIST_CREATE),
               file_name);
         switch (MsgBox(gszMsgBox, TOOL_NAME, YNC_MB)) {
         case MB_ID_YES:
            MakeQuiescent();
            SaveNewFile(TRUE, file_name);
            break;
         case MB_ID_NO: break;
         case MB_ID_CANCEL:
            if (do_not_save) SetFileModified(TRUE);
            rc = FALSE;
            break;
         }
      } else {
         fclose(fp);
      }
      obj_file = FileNameHasExtension(file_name, OBJ_FILE_TYPE, &gzipped,
            NULL);
      if (rc && !LoadFile(file_name, obj_file, obj_file && gzipped)) {
         if (do_not_save) SetFileModified(TRUE);
         rc = FALSE;
      }
   }
   /* At this point teleport_attr is no longer valid */
   if (rc && page_spec != NULL && !just_goto_page) {
      int new_page_num=(-1);

      if (!GetPageNumFromPageSpec(page_spec, &new_page_num)) {
         sprintf(msg, TgLoadString(STID_INVALID_NAMED_TELEPORT_DEST), dest_ptr);
         MsgBox(msg, TOOL_NAME, INFO_MB);
         rc = FALSE;
      } else if (new_page_num != curPageNum) {
         /*
          * No need to call BeforeNavigate() here because either
          * LoadRemoteFileFromMem() is called (which calls LoadFile())
          * or LoadFile() is called directly.
          */
         GotoPageNum(new_page_num);
         ShowPage();
         ClearAndRedrawDrawWindow();
         RedrawTitleWindow();
         RedrawRulers();
         RedrawScrollBars();
         justDupped = FALSE;
      }
   }
   if (dest_ptr != NULL) free(dest_ptr);
   if (page_spec != NULL) free(page_spec);

   if (referer_set) {
      HttpClearReferer();
   }
   return rc;
}

#define DO_PAGE_BY_NUM 0
#define DO_PAGE_BY_NAME 1

int DoPageTeleport(teleport_attr, do_by_page_name)
   struct AttrRec *teleport_attr;
   int do_by_page_name;
{
   int i, rc=TRUE;
   char msg[MAXSTRING+1];

   if (do_by_page_name) {
      struct PageRec *page_ptr;

      for (i=1, page_ptr=firstPage; page_ptr!=NULL;
            page_ptr=page_ptr->next, i++) {
         if (page_ptr->name != NULL && strcmp(page_ptr->name,
               teleport_attr->attr_value.s) == 0) {
            if (curPageNum != i) SetCurPage(i);
            return TRUE;
         }
      }
      sprintf(msg, TgLoadString(STID_CANT_FIND_PAGE_NAMED_TO_TEL),
            teleport_attr->attr_value.s);
      MsgBox(msg, TOOL_NAME, INFO_MB);
      rc = FALSE;
   } else {
      i = atoi(teleport_attr->attr_value.s);
      if (i >= 1 && i <= lastPageNum) {
         if (curPageNum != i) SetCurPage(i);
      } else {
         sprintf(msg, TgLoadString(STID_CANT_FIND_PAGE_NUM_TO_TEL), i);
         MsgBox(msg, TOOL_NAME, INFO_MB);
         rc = FALSE;
      }
   }
   return rc;
}

static
void ResetDeckIndices()
{
   register struct ObjRec *obj_ptr;

   for (obj_ptr=botObj; obj_ptr != NULL; obj_ptr=obj_ptr->prev) {
      switch (obj_ptr->type) {
      case OBJ_GROUP:
      case OBJ_ICON:
      case OBJ_SYM:
      case OBJ_PIN:
         obj_ptr->detail.r->deck_index = (-1);
         break;
      default: break;
      }
   }
}

void DoExecLoop(obj_ptr, exec_attr)
   struct ObjRec *obj_ptr;
   struct AttrRec *exec_attr;
{
   ResetExec(TRUE);

   while (exec_attr != NULL) {
      int saved_intr_check_interval=intrCheckInterval;
      int one_line_status=FALSE, exec_rc=TRUE, teleport_aborted=FALSE;
      int saved_history_depth=historyDepth;
      char status_buf[MAX_STATUS_BTNS+1][MAXSTRING+1];

      MakeQuiescent();
      intrCheckInterval = 1;
      ShowInterrupt(1);

      ResetDeckIndices();
      SaveStatusStringsIntoBuf(status_buf, &one_line_status);
      if (cmdToExecAfterHyperJump != NULL) {
         free(cmdToExecAfterHyperJump);
         cmdToExecAfterHyperJump = NULL;
      }
      warpToAttr = NULL;
      execNavigateBack = FALSE;
      exec_rc = DoExec(exec_attr, obj_ptr);
      exec_attr = NULL;
      RemoveAllSel();
      EndExecAnimate();
      if (saved_history_depth != historyDepth) RestoreDefaultHistoryDepth();
      if (exec_rc == TRUE && warpToAttr != NULL) {
         teleport_aborted = !DoTeleport(warpToAttr);
      }
      RestoreStatusStringsFromBuf(status_buf, one_line_status);

      while (HideInterrupt() > 0) ;
      intrCheckInterval = saved_intr_check_interval;

      if (exec_rc==TRUE && warpToAttr!=NULL && !teleport_aborted) {
         if (cmdToExecAfterHyperJump == NULL) {
            if ((exec_attr=FindFileAttrWithName("auto_exec=")) == NULL) {
               ResetExec(FALSE);
               return;
            }
            obj_ptr = NULL;
            continue;
         } else {
            exec_attr = FindAttrWithName(NULL, cmdToExecAfterHyperJump,
                  &obj_ptr);
            if (exec_attr == NULL) {
               sprintf(gszMsgBox, TgLoadString(STID_CANT_FIND_NAMED_ATTR_EXEC),
                     cmdToExecAfterHyperJump, "hyperjump_then_exec");
               MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
               ResetExec(FALSE);
               return;
            }
            continue;
         }
      }
      if (exec_rc==TRUE && execNavigateBack) {
         NavigateBack();
      }
      ResetExec(FALSE);
      return;
   }
   TwoLineMsg(TgLoadCachedString(CSTID_CANT_FIND_INTERPRETABLE_ATTR),
         TgLoadCachedString(CSTID_NO_ACTION_TAKEN));
   ResetExec(FALSE);
}

void Teleport(button_ev)
   XButtonEvent *button_ev;
{
   struct AttrRec *teleport_attr, *launch_attr, *exec_attr;
   struct ObjRec *obj_ptr, *owner_obj;
   char buf[MAXSTRING+1];
   int len;

   if ((obj_ptr=FindAnObj(button_ev->x,button_ev->y,&owner_obj,NULL,NULL)) ==
         NULL) {
      if (inSlideShow) {
         NextSlide();
      }
      return;
   }
   if (owner_obj != NULL) obj_ptr = owner_obj;

   teleport_attr = FindAttrWithName(obj_ptr, TELEPORT_ATTR, NULL);
   if (teleport_attr != NULL) {
      if (DoTeleport(teleport_attr)) {
         if ((exec_attr=FindFileAttrWithName("auto_exec=")) != NULL) {
            DoExecLoop(NULL, exec_attr);
         }
      }
      return;
   }
   teleport_attr = FindAttrWithName(obj_ptr, "href=", NULL);
   if (teleport_attr != NULL && *teleport_attr->attr_value.s != '\0') {
      if (DoTeleport(teleport_attr)) {
         if ((exec_attr=FindFileAttrWithName("auto_exec=")) != NULL) {
            DoExecLoop(NULL, exec_attr);
         }
      }
      return;
   }
   strcpy(buf, TELEPORT_ATTR);
   len = strlen(buf);
   if (buf[len-1] == '=') {
      sprintf(&buf[len-1], "_page#=");
      teleport_attr = FindAttrWithName(obj_ptr, buf, NULL);
      if (teleport_attr != NULL && *teleport_attr->attr_value.s != '\0') {
         DoPageTeleport(teleport_attr, DO_PAGE_BY_NUM);
         return;
      }
      sprintf(&buf[len-1], "_page=");
      teleport_attr = FindAttrWithName(obj_ptr, buf, NULL);
      if (teleport_attr != NULL && *teleport_attr->attr_value.s != '\0') {
         DoPageTeleport(teleport_attr, DO_PAGE_BY_NAME);
         return;
      }
   }
   launch_attr = FindAttrWithName(obj_ptr, LAUNCH_ATTR, NULL);
   if (launch_attr != NULL) {
      DoLaunch(launch_attr, obj_ptr);
      return;
   }
   exec_attr = FindAttrWithName(obj_ptr, EXEC_ATTR, NULL);
   if (exec_attr == NULL && inSlideShow) {
      NextSlide();
      return;
   }
   DoExecLoop(obj_ptr, exec_attr);
}

void SelAllObj(high_light, ignore_slideshow)
   int high_light, ignore_slideshow;
{
   struct ObjRec *obj_ptr=NULL;

   TieLooseEnds();
   SetCurChoice(NOTHING);
   if (topSel != NULL) {
      HighLightReverse();
      RemoveAllSel();
   }
   if (inSlideShow && !ignore_slideshow) {
      SetCurChoice(curChoiceBeforeMakeQuiescent);
      return;
   }
   for (obj_ptr=botObj; obj_ptr != NULL; obj_ptr=obj_ptr->prev) {
      obj_ptr->tmp_parent = NULL;
      if (colorLayers && !ObjInVisibleLayer(obj_ptr)) {
         continue;
      }
      AddObjIntoSel(obj_ptr, NULL, topSel, &topSel, &botSel);
   }
   UpdSelBBox();
   if (high_light) HighLightForward();
   justDupped = FALSE;
}

static struct ObjRec	*tmpTopObj=NULL, *tmpBotObj=NULL;

static
void PushTmpObj(ObjPtr)
   struct ObjRec *ObjPtr;
{
   ObjPtr->next = tmpTopObj;
   ObjPtr->prev = NULL;

   if (tmpBotObj == NULL) {
      tmpBotObj = ObjPtr;
   } else {
      tmpTopObj->prev = ObjPtr;
   }
   tmpTopObj = ObjPtr;
}

static
void BreakSel()
   /* break off selected objects from the main stream objects           */
   /* when returns, tmpTopObj points to the top of the selected objects */
   /*    and tmpBotObj points to the bottom of the selected objects     */
{
   struct SelRec *sel_ptr=NULL;

   for (sel_ptr=botSel; sel_ptr != NULL; sel_ptr=sel_ptr->prev) {
      UnlinkObj(sel_ptr->obj);
      PushTmpObj(sel_ptr->obj);
   }
}

void JustMoveSelToTop()
{
   if (topSel == NULL) return;

   tmpTopObj = tmpBotObj = NULL;

   BreakSel();
   tmpBotObj->next = topObj;
   if (topObj == NULL) {
      curPage->bot = botObj = tmpBotObj;
   } else {
      topObj->prev = tmpBotObj;
   }
   curPage->top = topObj = tmpTopObj;
}

void MoveSelToTop()
{
   if (topSel == NULL) {
      MsgBox(TgLoadCachedString(CSTID_NO_OBJ_SELECTED), TOOL_NAME, INFO_MB);
      return;
   }
   PrepareToRecord(CMD_REPLACE, topSel, botSel, numObjSelected);
   JustMoveSelToTop();
   RecordCmd(CMD_REPLACE, NULL, topSel, botSel, numObjSelected);
}

void MoveSelToBot()
{
   if (topSel == NULL) {
      MsgBox(TgLoadCachedString(CSTID_NO_OBJ_SELECTED), TOOL_NAME, INFO_MB);
      return;
   }
   PrepareToRecord(CMD_REPLACE, topSel, botSel, numObjSelected);
   tmpTopObj = tmpBotObj = NULL;

   BreakSel();
   tmpTopObj->prev = botObj;
   if (topObj == NULL) {
      curPage->top = topObj = tmpTopObj;
   } else {
      botObj->next = tmpTopObj;
   }
   curPage->bot = botObj = tmpBotObj;
   RecordCmd(CMD_REPLACE, NULL, topSel, botSel, numObjSelected);
}

static
void DeleteObjectInsteadOfVertex(obj_ptr)
   struct ObjRec *obj_ptr;
{
   struct SelRec *sel_ptr=NULL, *saved_top_sel=NULL, *saved_bot_sel=NULL;

   for (sel_ptr=botSel; sel_ptr!=NULL; sel_ptr=sel_ptr->prev) {
      if (sel_ptr->obj == obj_ptr) {
         break;
      }
   }
   if (sel_ptr->prev == NULL) {
      topSel = sel_ptr->next;
   } else {
      sel_ptr->prev->next = sel_ptr->next;
   }
   if (sel_ptr->next == NULL) {
      botSel = sel_ptr->prev;
   } else {
      sel_ptr->next->prev = sel_ptr->prev;
   }
   saved_top_sel = topSel;
   saved_bot_sel = botSel;
   topSel = botSel = sel_ptr;
   sel_ptr->next = sel_ptr->prev = NULL;
   DelObj(obj_ptr);
   topSel = saved_top_sel;
   botSel = saved_bot_sel;

   free(sel_ptr);
   ChangeReplaceOneCmdToDeleteCmd();
}

void DelAllSelObj()
{
   struct ObjRec *obj_ptr=NULL;
   struct SelRec *sel_ptr=NULL;
   int j, i;
   struct VSelRec *vsel_ptr=NULL;
   IntPoint *vlist=NULL;
   struct PolyRec *poly_ptr=NULL;
   struct PolygonRec *polygon_ptr=NULL;
   int n=0;
   short *mark=NULL;

   if (topSel==NULL && topVSel==NULL) {
      MsgBox(TgLoadCachedString(CSTID_NO_OBJ_SELECTED), TOOL_NAME, INFO_MB);
      return;
   }
   HighLightReverse();
   if (curChoice == VERTEXMODE) {
      StartCompositeCmd();
      for (vsel_ptr=botVSel; vsel_ptr!=NULL; vsel_ptr=vsel_ptr->prev) {
         int delete_it=FALSE, extra_vertex=FALSE, curved=(-1), ssn=0;
         char *smooth=NULL, *ssmooth=NULL;
         IntPoint *ssvlist=NULL;

         obj_ptr = vsel_ptr->obj;

         switch (obj_ptr->type) {
         case OBJ_POLY:
            poly_ptr = obj_ptr->detail.p;
            curved = poly_ptr->curved;
            if (curved == LT_STRUCT_SPLINE) {
               ssvlist = poly_ptr->ssvlist;
               ssn = poly_ptr->ssn;
               ssmooth = poly_ptr->ssmooth;
            } else {
               vlist = poly_ptr->vlist;
               n = poly_ptr->n;
               smooth = poly_ptr->smooth;
            }
            if (vsel_ptr->n >= n-1) delete_it = TRUE;
            break;
         case OBJ_POLYGON:
            polygon_ptr = obj_ptr->detail.g;
            curved = polygon_ptr->curved;
            if (curved == LT_STRUCT_SPLINE) {
               ssvlist = polygon_ptr->ssvlist;
               ssn = polygon_ptr->ssn;
               ssmooth = polygon_ptr->ssmooth;
            } else {
               vlist = polygon_ptr->vlist;
               n = polygon_ptr->n;
               smooth = polygon_ptr->smooth;
            }
            for (j=0; j < vsel_ptr->n; j++) {
               if (vsel_ptr->v_index[j] == 0) {
                  extra_vertex = TRUE;
                  break;
               }
            }
            if ((!extra_vertex && n-vsel_ptr->n <= 3) ||
                  (extra_vertex && n-vsel_ptr->n <= 2)) {
               delete_it = TRUE;
            }
            break;
         }
         if (curved == LT_STRUCT_SPLINE) {
            PrepareToReplaceAnObj(obj_ptr);
            if (DeleteStructuredSplinePoint(vsel_ptr->v_index[0], poly_ptr,
                  polygon_ptr)) {
               AdjObjSplineVs(obj_ptr);
               if (poly_ptr != NULL) {
                  UpdPolyBBox(obj_ptr, poly_ptr->n, poly_ptr->vlist);
               } else if (polygon_ptr != NULL) {
                  UpdPolyBBox(obj_ptr, polygon_ptr->n,
                        polygon_ptr->vlist);
               }
               AdjObjBBox(obj_ptr);
               RecordReplaceAnObj(obj_ptr);
            } else {
               /* delete the object */
               DeleteObjectInsteadOfVertex(obj_ptr);
            }
         } else {
            PrepareToReplaceAnObj(obj_ptr);
            if (delete_it) {
               /* delete the object */
               DeleteObjectInsteadOfVertex(obj_ptr);
            } else {
               mark = (short *)malloc(n*sizeof(short));
               if (mark == NULL) FailAllocMessage();
               for (j=0; j < n; j++) mark[j] = FALSE;
               for (j=0; j < vsel_ptr->n; j++) {
                  mark[vsel_ptr->v_index[j]] = TRUE;
               }
               switch (obj_ptr->type) {
               case OBJ_POLY:
                  for (i=n-1; i >= 0; i--) {
                     if (mark[i]) {
                        for (j=i+1; j < n; j++) {
                           vlist[j-1] = vlist[j];
                           if (smooth != NULL) smooth[j-1] = smooth[j];
                        }
                        if (smooth != NULL) {
                           if (i == 0) {
                              smooth[0] = FALSE;
                           } else if (i == n-1) {
                              smooth[n-2] = FALSE;
                           }
                        }
                        n--;
                     }
                  }
                  poly_ptr->n -= vsel_ptr->n;
                  AdjObjSplineVs(obj_ptr);
                  if (poly_ptr->curved != LT_INTSPLINE) {
                     UpdPolyBBox(obj_ptr, poly_ptr->n, poly_ptr->vlist);
                  } else {
                     UpdPolyBBox(obj_ptr, poly_ptr->intn, poly_ptr->intvlist);
                  }
                  break;
               case OBJ_POLYGON:
                  for (i=n-2; i >= 0; i--) {
                     if (mark[i]) {
                        for (j=i+1; j < n; j++) {
                           vlist[j-1] = vlist[j];
                           if (smooth != NULL) smooth[j-1] = smooth[j];
                        }
                        n--;
                        if (smooth != NULL && i == 0) {
                           smooth[n-1] = smooth[0];
                        }
                     }
                  }
                  polygon_ptr->n -= vsel_ptr->n;
                  if (extra_vertex) vlist[polygon_ptr->n++] = vlist[0];
                  AdjObjSplineVs(obj_ptr);
                  if (polygon_ptr->curved != LT_INTSPLINE) {
                     UpdPolyBBox(obj_ptr, polygon_ptr->n, polygon_ptr->vlist);
                  } else {
                     UpdPolyBBox(obj_ptr, polygon_ptr->intn,
                           polygon_ptr->intvlist);
                  }
                  break;
               }
               free(mark);
               AdjObjBBox(obj_ptr);
               RecordReplaceAnObj(obj_ptr);
            }
         }
      }
      EndCompositeCmd();
      JustRemoveAllVSel();
   } else {
      struct SelRec *first_extra_poly_sel=NULL, *last_extra_poly_sel=NULL;
      struct BBRec extra_bbox;
      int extra_poly_count=0, delete_extra_poly=FALSE;

      delete_extra_poly = FindExtraPoly(topSel, botSel,
            &first_extra_poly_sel, &last_extra_poly_sel, &extra_poly_count,
            &extra_bbox);
      StartCompositeCmd();
      PrepareToRecord(CMD_DELETE, topSel, botSel, numObjSelected);
      for (sel_ptr=botSel; sel_ptr != NULL; sel_ptr=sel_ptr->prev) {
         UnlinkObj(sel_ptr->obj);
         FreeObj(sel_ptr->obj);
      }
      RemoveAllSel();
      RecordCmd(CMD_DELETE, NULL, NULL, NULL, 0);
      if (delete_extra_poly) {
         /*
          * Please note that FindExtraPoly() returns the poly in the
          *	correct stacking order!
          */
         PrepareToRecord(CMD_DELETE, first_extra_poly_sel, last_extra_poly_sel,
               extra_poly_count);
         for (sel_ptr=last_extra_poly_sel; sel_ptr != NULL;
               sel_ptr=sel_ptr->prev) {
            UnlinkObj(sel_ptr->obj);
            FreeObj(sel_ptr->obj);
         }
         JustFreeSel(first_extra_poly_sel, last_extra_poly_sel);
         RecordCmd(CMD_DELETE, NULL, NULL, NULL, 0);

         ExpandExtents(&extra_bbox, &selLtX, &selLtY, &selRbX, &selRbY);
      }
      EndCompositeCmd();
   }
   RedrawAnArea(botObj, selLtX-GRID_ABS_SIZE(1), selLtY-GRID_ABS_SIZE(1),
         selRbX+GRID_ABS_SIZE(1), selRbY+GRID_ABS_SIZE(1));
   HighLightForward();
   SetFileModified(TRUE);
   justDupped = FALSE;
}

void GroupSingleObj(ForcePromoteAttrs)
   int ForcePromoteAttrs;
{
   tmpTopObj = tmpBotObj = NULL;

   BreakSel();
   CreateGroupObj(tmpTopObj, tmpBotObj);
   RemoveAllSel();
   if (tmpTopObj == tmpBotObj && tmpTopObj->fattr != NULL &&
         (ForcePromoteAttrs ||
         !(tmpTopObj->type == OBJ_GROUP || tmpTopObj->type == OBJ_SYM ||
         tmpTopObj->type == OBJ_ICON || tmpTopObj->type == OBJ_PIN))) {
      struct AttrRec *attr_ptr;

      topObj->fattr = tmpTopObj->fattr;
      topObj->lattr = tmpTopObj->lattr;
      tmpTopObj->fattr = tmpTopObj->lattr = NULL;
      for (attr_ptr=topObj->fattr; attr_ptr != NULL; attr_ptr=attr_ptr->next) {
         attr_ptr->owner = topObj;
      }
   }
   topSel = botSel = (struct SelRec *)malloc(sizeof(struct SelRec));
   if (topSel == NULL) FailAllocMessage();
   topSel->obj = topObj;
   topSel->next = topSel->prev = NULL;
   numObjSelected = 1;
}

void GroupSelObj(highlight, record_cmd, redraw)
   int highlight, record_cmd, redraw;
{
   if (topSel == NULL) {
      MsgBox(TgLoadCachedString(CSTID_NO_OBJ_SELECTED), TOOL_NAME, INFO_MB);
      return;
   }
   if (curChoice==VERTEXMODE && topSel!=NULL) {
      MsgBox(TgLoadString(STID_CANT_GROUP_IN_VERTEX_MODE), TOOL_NAME, INFO_MB);
      return;
   }
   if (topSel == botSel && topSel->obj->type != OBJ_POLY &&
         topSel->obj->type != OBJ_POLYGON) {
      MsgBox(TgLoadString(STID_CANT_GROUP_SINGLE_OBJECT), TOOL_NAME, INFO_MB);
      return;
   }

   tmpTopObj = tmpBotObj = NULL;

   if (highlight) HighLightReverse();
   if (record_cmd) PrepareToRecord(CMD_REPLACE, topSel, botSel, numObjSelected);
   BreakSel();

   CreateGroupObj(tmpTopObj, tmpBotObj);

   RemoveAllSel();
   topSel = botSel = (struct SelRec *)malloc(sizeof(struct SelRec));
   if (topSel == NULL) FailAllocMessage();
   topSel->obj = topObj;
   topSel->next = topSel->prev = NULL;
   UpdSelBBox();

   if (record_cmd) RecordCmd(CMD_MANY_TO_ONE, NULL, topSel, botSel, 1);
   if (redraw) {
      RedrawAnArea(botObj, selLtX-GRID_ABS_SIZE(1), selLtY-GRID_ABS_SIZE(1),
            selRbX+GRID_ABS_SIZE(1), selRbY+GRID_ABS_SIZE(1));
   }
   if (highlight) HighLightForward();
   SetFileModified(TRUE);
   justDupped = FALSE;
}

void SelectTopObj()
{
   if (topObj == NULL) return;

   topSel = botSel = (struct SelRec *)malloc(sizeof(struct SelRec));
   if (topSel == NULL) FailAllocMessage();
   topSel->obj = topObj;
   topSel->next = topSel->prev = NULL;
   UpdSelBBox();

   HighLightForward();
   justDupped = FALSE;
}
