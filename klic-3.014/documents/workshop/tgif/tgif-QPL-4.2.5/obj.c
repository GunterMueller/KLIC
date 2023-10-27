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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/obj.c,v 1.16 2011/05/16 16:21:58 william Exp $
 */

#define _INCLUDE_FROM_OBJ_C_

#include "tgifdefs.h"

#include "arc.e"
#include "attr.e"
#include "auxtext.e"
#include "box.e"
#include "cmd.e"
#include "cursor.e"
#include "group.e"
#include "msg.e"
#include "obj.e"
#include "oval.e"
#include "page.e"
#include "pin.e"
#include "poly.e"
#include "polygon.e"
#include "rcbox.e"
#include "rect.e"
#include "setup.e"
#include "spline.e"
#include "stretch.e"
#include "text.e"
#include "xbitmap.e"
#include "xpixmap.e"

struct ObjRec *topObj=NULL, *botObj=NULL;

void AddObj(PrevPtr, NextPtr, ObjPtr)
   struct ObjRec *PrevPtr, *NextPtr, *ObjPtr;
   /* add ObjPtr between PrevPtr and NextPtr */
{
   ObjPtr->prev = PrevPtr;
   ObjPtr->next = NextPtr;

   if (PrevPtr == NULL) {
      curPage->top = topObj = ObjPtr;
   } else {
      PrevPtr->next = ObjPtr;
   }
   if (NextPtr == NULL) {
      curPage->bot = botObj = ObjPtr;
   } else {
      NextPtr->prev = ObjPtr;
   }
}

void UnlinkObj(ObjPtr)
   struct ObjRec *ObjPtr;
{
   if (topObj == ObjPtr) {
      curPage->top = topObj = ObjPtr->next;
   } else {
      ObjPtr->prev->next = ObjPtr->next;
   }
   if (botObj == ObjPtr) {
      curPage->bot = botObj = ObjPtr->prev;
   } else {
      ObjPtr->next->prev = ObjPtr->prev;
   }
}

void ReplaceObj(old_obj, new_obj)
   struct ObjRec *old_obj, *new_obj;
{
   if (topObj == old_obj) {
      curPage->top = topObj = new_obj;
      new_obj->prev = NULL;
   } else {
      old_obj->prev->next = new_obj;
      new_obj->prev = old_obj->prev;
   }
   if (botObj == old_obj) {
      curPage->bot = botObj = new_obj;
      new_obj->next = NULL;
   } else {
      old_obj->next->prev = new_obj;
      new_obj->next = old_obj->next;
   }
}

void FreeObj(ObjPtr)
   register struct ObjRec *ObjPtr;
{
   if (ObjPtr == NULL) return;

   if (ObjPtr->ctm != NULL) {
      free(ObjPtr->ctm);
      ObjPtr->ctm = NULL;
   }
   if (ObjPtr->creator_full_id != NULL) {
      free(ObjPtr->creator_full_id);
      ObjPtr->creator_full_id = NULL;
   }
   switch (ObjPtr->type) {
   case OBJ_POLY: DelAllAttrs(ObjPtr->fattr); FreePolyObj(ObjPtr); break;
   case OBJ_BOX: DelAllAttrs(ObjPtr->fattr); FreeBoxObj(ObjPtr); break;
   case OBJ_OVAL: DelAllAttrs(ObjPtr->fattr); FreeOvalObj(ObjPtr); break;
   case OBJ_TEXT: FreeTextObj(ObjPtr); break;
   case OBJ_POLYGON: DelAllAttrs(ObjPtr->fattr); FreePolygonObj(ObjPtr); break;
   case OBJ_ARC: DelAllAttrs(ObjPtr->fattr); FreeArcObj(ObjPtr); break;
   case OBJ_RCBOX: DelAllAttrs(ObjPtr->fattr); FreeRCBoxObj(ObjPtr); break;
   case OBJ_XBM: DelAllAttrs(ObjPtr->fattr); FreeXBmObj(ObjPtr); break;
   case OBJ_XPM: DelAllAttrs(ObjPtr->fattr); FreeXPmObj(ObjPtr); break;
   case OBJ_GROUP:
   case OBJ_ICON:
   case OBJ_SYM:
   case OBJ_PIN: DelAllAttrs(ObjPtr->fattr); FreeGroupObj(ObjPtr); break;
   }
}

void DelObj(ObjPtr)
   struct ObjRec *ObjPtr;
{
   if (ObjPtr->type == OBJ_POLY) {
      if (ObjPtr->detail.p->start_conn != NULL) {
         DelConnFromPoly(ObjPtr->detail.p->start_conn);
      }
      if (ObjPtr->detail.p->end_conn != NULL) {
         DelConnFromPoly(ObjPtr->detail.p->end_conn);
      }
   } else if (ObjPtr->type == OBJ_PIN) {
      while (ObjPtr->detail.r->pin_connected > 0) {
         DelConnFromPin(ObjPtr->detail.p->end_conn);
      }
   }
   if (topObj == ObjPtr) {
      curPage->top = topObj = ObjPtr->next;
   } else {
      ObjPtr->prev->next = ObjPtr->next;
   }
   if (botObj == ObjPtr) {
      curPage->bot = botObj = ObjPtr->prev;
   } else {
      ObjPtr->next->prev = ObjPtr->prev;
   }
   FreeObj(ObjPtr);
}

void DelAllObj()
{
   register struct ObjRec *ptr, *next_obj;

   if (topObj == NULL) return;

   for (ptr=topObj; ptr != NULL; ptr=next_obj) {
      next_obj = ptr->next;
      FreeObj(ptr);
   }
   if (curPage != NULL) curPage->top = curPage->bot = NULL;
   topObj = botObj = NULL;
}

void AdjObjOBBox(ObjPtr)
   struct ObjRec *ObjPtr;
{
   struct ObjRec *obj_ptr;
   int oltx, olty, orbx, orby;

   switch (ObjPtr->type) {
   case OBJ_ARC:
      CalcArcOBBox(ObjPtr);
      break;
   case OBJ_TEXT:
      SetTextOBBox(ObjPtr, ObjPtr->detail.t->minilines.just,
            ObjPtr->obbox.rbx-ObjPtr->obbox.ltx,
            ObjPtr->obbox.rby-ObjPtr->obbox.lty,
            ObjPtr->detail.t->min_lbearing,
            ObjPtr->detail.t->max_rextra, ROTATE0);
      break;
   case OBJ_GROUP:
   case OBJ_ICON:
   case OBJ_SYM:
      obj_ptr = ObjPtr->detail.r->last;
      oltx = obj_ptr->obbox.ltx; olty = obj_ptr->obbox.lty;
      orbx = obj_ptr->obbox.rbx; orby = obj_ptr->obbox.rby;
      for (obj_ptr = obj_ptr->prev; obj_ptr != NULL; obj_ptr = obj_ptr->prev) {
         if (obj_ptr->obbox.ltx < oltx) oltx = obj_ptr->obbox.ltx;
         if (obj_ptr->obbox.lty < olty) olty = obj_ptr->obbox.lty;
         if (obj_ptr->obbox.rbx > orbx) orbx = obj_ptr->obbox.rbx;
         if (obj_ptr->obbox.rby > orby) orby = obj_ptr->obbox.rby;
      }
      ObjPtr->obbox.ltx = oltx; ObjPtr->obbox.lty = olty;
      ObjPtr->obbox.rbx = orbx; ObjPtr->obbox.rby = orby;
      break;
   case OBJ_PIN:
      obj_ptr = GetPinObj(ObjPtr);
      oltx = obj_ptr->obbox.ltx; olty = obj_ptr->obbox.lty;
      orbx = obj_ptr->obbox.rbx; orby = obj_ptr->obbox.rby;
      ObjPtr->obbox.ltx = oltx; ObjPtr->obbox.lty = olty;
      ObjPtr->obbox.rbx = orbx; ObjPtr->obbox.rby = orby;
      break;
   }
}

void AdjObjBBox(ObjPtr)
   struct ObjRec *ObjPtr;
{
   register int w=0;
   register struct ObjRec *obj_ptr;
   register struct AttrRec *attr_ptr;
   int ltx, lty, rbx, rby;

   AdjObjOBBox(ObjPtr);

   switch (ObjPtr->type) {
   case OBJ_POLY:
      CalcPolyBBox(ObjPtr);
      break;
   case OBJ_ARC:
      CalcArcBBox(ObjPtr, ObjPtr->obbox, &(ObjPtr->bbox));
      break;
   case OBJ_POLYGON: w = HALF_W(ObjPtr->detail.g->width); break;
   case OBJ_BOX: w = HALF_W(ObjPtr->detail.b->width); break;
   case OBJ_OVAL: w = HALF_W(ObjPtr->detail.o->width); break;
   case OBJ_RCBOX: w = HALF_W(ObjPtr->detail.rcb->width); break;
   case OBJ_XBM: break;
   case OBJ_XPM: break;

   case OBJ_GROUP:
   case OBJ_ICON:
   case OBJ_SYM:
      obj_ptr = ObjPtr->detail.r->last;
      ltx = obj_ptr->bbox.ltx; lty = obj_ptr->bbox.lty;
      rbx = obj_ptr->bbox.rbx; rby = obj_ptr->bbox.rby;
      for (obj_ptr = obj_ptr->prev; obj_ptr != NULL; obj_ptr = obj_ptr->prev) {
         if (obj_ptr->bbox.ltx < ltx) ltx = obj_ptr->bbox.ltx;
         if (obj_ptr->bbox.lty < lty) lty = obj_ptr->bbox.lty;
         if (obj_ptr->bbox.rbx > rbx) rbx = obj_ptr->bbox.rbx;
         if (obj_ptr->bbox.rby > rby) rby = obj_ptr->bbox.rby;
      }
      ObjPtr->bbox.ltx = ltx; ObjPtr->bbox.lty = lty;
      ObjPtr->bbox.rbx = rbx; ObjPtr->bbox.rby = rby;
      break;
   case OBJ_PIN:
      obj_ptr = GetPinObj(ObjPtr);
      ltx = obj_ptr->bbox.ltx; lty = obj_ptr->bbox.lty;
      rbx = obj_ptr->bbox.rbx; rby = obj_ptr->bbox.rby;
      ObjPtr->bbox.ltx = ltx; ObjPtr->bbox.lty = lty;
      ObjPtr->bbox.rbx = rbx; ObjPtr->bbox.rby = rby;
      break;
   }
   switch (ObjPtr->type) {
   case OBJ_POLY:
   case OBJ_ARC:
      break;
   case OBJ_POLYGON:
   case OBJ_BOX:
   case OBJ_OVAL:
   case OBJ_RCBOX:
   case OBJ_XBM:
   case OBJ_XPM:
      ObjPtr->bbox.ltx = ObjPtr->obbox.ltx - w;
      ObjPtr->bbox.lty = ObjPtr->obbox.lty - w;
      ObjPtr->bbox.rbx = ObjPtr->obbox.rbx + w;
      ObjPtr->bbox.rby = ObjPtr->obbox.rby + w;
      break;
   case OBJ_TEXT:
      SetTextBBox(ObjPtr, ObjPtr->detail.t->minilines.just,
            ObjPtr->obbox.rbx-ObjPtr->obbox.ltx,
            ObjPtr->obbox.rby-ObjPtr->obbox.lty,
            ObjPtr->detail.t->min_lbearing,
            ObjPtr->detail.t->max_rextra, ROTATE0);
      break;
   }

   attr_ptr = ObjPtr->fattr;

   ltx = ObjPtr->bbox.ltx;
   lty = ObjPtr->bbox.lty;
   rbx = ObjPtr->bbox.rbx;
   rby = ObjPtr->bbox.rby;
   for ( ; attr_ptr != NULL; attr_ptr = attr_ptr->next) {
      if (attr_ptr->shown) {
         if (attr_ptr->obj->bbox.ltx < ltx) ltx = attr_ptr->obj->bbox.ltx;
         if (attr_ptr->obj->bbox.lty < lty) lty = attr_ptr->obj->bbox.lty;
         if (attr_ptr->obj->bbox.rbx > rbx) rbx = attr_ptr->obj->bbox.rbx;
         if (attr_ptr->obj->bbox.rby > rby) rby = attr_ptr->obj->bbox.rby;
      }
   }
   ObjPtr->bbox.ltx = ltx;
   ObjPtr->bbox.lty = lty;
   ObjPtr->bbox.rbx = rbx;
   ObjPtr->bbox.rby = rby;

   if (ObjPtr->type == OBJ_SYM) {
      if (ObjPtr->obbox.ltx-QUARTER_INCH < ObjPtr->bbox.ltx) {
         ObjPtr->bbox.ltx = ObjPtr->obbox.ltx - QUARTER_INCH;
      }
      if (ObjPtr->obbox.lty-QUARTER_INCH < ObjPtr->bbox.lty) {
         ObjPtr->bbox.lty = ObjPtr->obbox.lty - QUARTER_INCH;
      }
      if (ObjPtr->obbox.rbx+QUARTER_INCH > ObjPtr->bbox.rbx) {
         ObjPtr->bbox.rbx = ObjPtr->obbox.rbx + QUARTER_INCH;
      }
      if (ObjPtr->obbox.rby+QUARTER_INCH > ObjPtr->bbox.rby) {
         ObjPtr->bbox.rby = ObjPtr->obbox.rby + QUARTER_INCH;
      }
   }
}

static
void RecursivelyAdjAnObjBBox(target_obj, owner_obj)
   struct ObjRec *target_obj, *owner_obj;
{
   if (target_obj == owner_obj) {
      AdjObjBBox(target_obj);
   } else {
      struct ObjRec *obj_ptr=NULL;

      switch (owner_obj->type) {
      case OBJ_GROUP:
      case OBJ_ICON:
      case OBJ_SYM:
      case OBJ_PIN:
         for (obj_ptr=owner_obj->detail.r->first; obj_ptr!=NULL;
               obj_ptr=obj_ptr->next) {
            RecursivelyAdjAnObjBBox(target_obj, obj_ptr);
         }
         AdjObjBBox(owner_obj);
         break;
      default: break;
      }
   }
}

void RecursivelyAdjObjBBox(sub_obj, owner_obj, top_owner)
   struct ObjRec *sub_obj, *owner_obj, *top_owner;
{
   if (owner_obj == NULL) {
      RecursivelyAdjAnObjBBox(sub_obj, sub_obj);
   } else {
      RecursivelyAdjAnObjBBox(sub_obj, top_owner);
   }
}

static
void InvalidateObjCache(ObjPtr)
   struct ObjRec *ObjPtr;
{
   int w=0, h=0, num_rows=0, num_cols=0;
   struct TextRec *text_ptr=NULL;
   struct XBmRec *xbm_ptr=NULL;
   struct XPmRec *xpm_ptr=NULL;
   struct ObjRec *obj_ptr=NULL;

   w = ObjPtr->obbox.rbx - ObjPtr->obbox.ltx;
   h = ObjPtr->obbox.rby - ObjPtr->obbox.lty;
   switch (ObjPtr->type) {
   case OBJ_POLY:
      if (NeedsToCachePolyObj(ObjPtr)) {
         struct PolyRec *poly_ptr=ObjPtr->detail.p;

         if (poly_ptr->rotated_vlist != NULL) free(poly_ptr->rotated_vlist);
         if (poly_ptr->rotated_asvlist != NULL) {
            free(poly_ptr->rotated_asvlist);
         }
         poly_ptr->rotated_vlist = poly_ptr->rotated_asvlist = NULL;
         poly_ptr->rotated_n = poly_ptr->rotated_asn = 0;
      }
      break;
   case OBJ_BOX: break;

   case OBJ_OVAL:
      /* atan2(61/112) = 0.498721 rad = 28.5746 deg */
      if (NeedsToCacheOvalObj(ObjPtr)) {
         struct OvalRec *oval_ptr=ObjPtr->detail.o;

         if (oval_ptr->rotated_vlist != NULL) free(oval_ptr->rotated_vlist);
         oval_ptr->rotated_vlist = NULL;
         oval_ptr->rotated_n = 0;
      }
      break;
   case OBJ_POLYGON:
      if (NeedsToCachePolygonObj(ObjPtr)) {
         struct PolygonRec *polygon_ptr=ObjPtr->detail.g;

         if (polygon_ptr->rotated_vlist != NULL) {
            free(polygon_ptr->rotated_vlist);
         }
         polygon_ptr->rotated_vlist = NULL;
         polygon_ptr->rotated_n = 0;
      }
      break;
   case OBJ_ARC:
      if (NeedsToCacheArcObj(ObjPtr)) {
         struct ArcRec *arc_ptr=ObjPtr->detail.a;

         if (arc_ptr->rotated_vlist != NULL) free(arc_ptr->rotated_vlist);
         if (arc_ptr->rotated_asvlist != NULL) free(arc_ptr->rotated_asvlist);
         arc_ptr->rotated_vlist = arc_ptr->rotated_asvlist = NULL;
         arc_ptr->rotated_n = arc_ptr->rotated_asn = 0;
      }
      break;
   case OBJ_RCBOX:
      if (NeedsToCacheRCBoxObj(ObjPtr)) {
         struct RCBoxRec *rcbox_ptr=ObjPtr->detail.rcb;

         if (rcbox_ptr->rotated_vlist != NULL) free(rcbox_ptr->rotated_vlist);
         rcbox_ptr->rotated_vlist = NULL;
         rcbox_ptr->rotated_n = 0;
      }
      break;
   case OBJ_TEXT:
      text_ptr = ObjPtr->detail.t;
      if (NeedsToCacheTextObj(ObjPtr)) {
         if (text_ptr->cached_bitmap != None) {
            XFreePixmap(mainDisplay, text_ptr->cached_bitmap);
         }
         if (text_ptr->cached_pixmap != None) {
            XFreePixmap(mainDisplay, text_ptr->cached_pixmap);
         }
         if (text_ptr->cached_bg_bitmap != None) {
            XFreePixmap(mainDisplay, text_ptr->cached_bg_bitmap);
         }
         text_ptr->cached_bitmap = None;
         text_ptr->cached_pixmap = None;
         text_ptr->cached_bg_bitmap = None;
         if (zoomScale != 0) {
            text_ptr->cached_zoom = 0;
         }
      }
      break;
   case OBJ_XBM:
      xbm_ptr = ObjPtr->detail.xbm;
      num_cols = (zoomedIn) ? (w<<zoomScale) : (w>>zoomScale);
      num_rows = (zoomedIn) ? (h<<zoomScale) : (h>>zoomScale);
      if (NeedsToCacheXBmObj(ObjPtr)) {
         if (xbm_ptr->cached_bitmap != None) {
            XFreePixmap(mainDisplay, xbm_ptr->cached_bitmap);
         }
         xbm_ptr->cached_bitmap = None;
         if (zoomScale != 0) {
            xbm_ptr->cached_zoom = 0;
         }
      }
      break;
   case OBJ_XPM:
      xpm_ptr = ObjPtr->detail.xpm;
      num_cols = (zoomedIn) ? (w<<zoomScale) : (w>>zoomScale);
      num_rows = (zoomedIn) ? (h<<zoomScale) : (h>>zoomScale);
      if (NeedsToCacheXPmObj(ObjPtr)) {
         if (xpm_ptr->cached_pixmap != None) {
            XFreePixmap(mainDisplay, xpm_ptr->cached_pixmap);
         }
         xpm_ptr->cached_pixmap = None;
         if (xpm_ptr->cached_bitmap != None) {
            XFreePixmap(mainDisplay, xpm_ptr->cached_bitmap);
         }
         xpm_ptr->cached_bitmap = None;
         if (xpm_ptr->clip_mask != None) {
            XFreePixmap(mainDisplay, xpm_ptr->clip_mask);
         }
         xpm_ptr->clip_mask = None;
         if (zoomScale != 0) {
            xpm_ptr->cached_zoom = 0;
         }
         xpm_ptr->cached_color = (-1);
      }
      break;

   case OBJ_GROUP:
   case OBJ_ICON:
   case OBJ_SYM:
   case OBJ_PIN:
      for (obj_ptr=ObjPtr->detail.r->first; obj_ptr != NULL;
            obj_ptr=obj_ptr->next) {
         InvalidateObjCache(obj_ptr);
      }
      break;
   }
}

void AdjObjCache(ObjPtr)
   struct ObjRec *ObjPtr;
{
   register struct ObjRec *obj_ptr;
   register struct AttrRec *attr_ptr;

   GetTransformedOBBoxOffsetVs(ObjPtr, ObjPtr->rotated_obbox);
   switch (ObjPtr->type) {
   case OBJ_GROUP:
   case OBJ_ICON:
   case OBJ_SYM:
   case OBJ_PIN:
      for (obj_ptr=ObjPtr->detail.r->first; obj_ptr != NULL;
            obj_ptr=obj_ptr->next) {
         AdjObjCache(obj_ptr);
      }
      break;
   default:
      InvalidateObjCache(ObjPtr);
      break;
   }
   for (attr_ptr=ObjPtr->fattr; attr_ptr!=NULL; attr_ptr=attr_ptr->next) {
      if (attr_ptr->shown) {
         AdjObjCache(attr_ptr->obj);
      }
   }
}

void AdjCaches()
{
   register struct ObjRec *obj_ptr, *ptr;
   register struct AttrRec *attr_ptr;

   if (topObj == NULL) return;

   for (obj_ptr = topObj; obj_ptr != NULL; obj_ptr=obj_ptr->next) {
      switch (obj_ptr->type) {
      case OBJ_TEXT:
      case OBJ_XBM:
      case OBJ_XPM:
         AdjObjCache(obj_ptr);
         break;
      case OBJ_GROUP:
      case OBJ_ICON:
      case OBJ_SYM:
      case OBJ_PIN:
         for (ptr=obj_ptr->detail.r->first; ptr != NULL; ptr=ptr->next) {
            AdjObjCache(ptr);
         }
         break;
      }
      for (attr_ptr=obj_ptr->fattr; attr_ptr!=NULL; attr_ptr=attr_ptr->next) {
         if (attr_ptr->shown) {
            AdjObjCache(attr_ptr->obj);
         }
      }
   }
}

static int dontAutoRetractArrow=FALSE;

#define RETREAT (0.8)

static
void AdjArcCache(ObjPtr)
   struct ObjRec *ObjPtr;
{
   struct ArcRec *arc_ptr=ObjPtr->detail.a;
   int a_angle1, a_angle2;

   if (arc_ptr->style == LS_PLAIN) return;

   a_angle1 = arc_ptr->angle1;
   a_angle2 = arc_ptr->angle2;
   GetArcArrowInfo(ObjPtr, NULL, NULL, NULL, &a_angle1,
         NULL, NULL, NULL, &a_angle2);
   arc_ptr->a_angle1 = a_angle1;
   arc_ptr->a_angle2 = a_angle2;
}

static
void AdjPolyCache(ObjPtr)
   struct ObjRec *ObjPtr;
{
   struct PolyRec *poly_ptr=ObjPtr->detail.p;

   if (poly_ptr->rotated_vlist != NULL) {
      free(poly_ptr->rotated_vlist);
   }
   if (poly_ptr->rotated_asvlist != NULL) {
      free(poly_ptr->rotated_asvlist);
   }
   poly_ptr->rotated_vlist = poly_ptr->rotated_asvlist = NULL;
   poly_ptr->rotated_n = poly_ptr->rotated_asn = 0;

   switch (poly_ptr->curved) {
   case LT_STRAIGHT:
   case LT_SPLINE:
      if (poly_ptr->svlist != NULL) {
         free(poly_ptr->svlist);
      }
      poly_ptr->svlist = MakeMultiSplinePolyVertex(
            poly_ptr->curved, &(poly_ptr->sn), poly_ptr->smooth,
            drawOrigX, drawOrigY,
            poly_ptr->n, poly_ptr->vlist);
      break;
   case LT_STRUCT_SPLINE:
      if (poly_ptr->svlist != NULL) free(poly_ptr->svlist);
      if (poly_ptr->ssvlist != NULL) free(poly_ptr->ssvlist);
      if (poly_ptr->ssmooth != NULL) free(poly_ptr->ssmooth);
      if (poly_ptr->smooth != NULL) free(poly_ptr->smooth);
      poly_ptr->smooth = NULL;
      poly_ptr->ssvlist = MakeStructuredSplinePolyVertex(&(poly_ptr->ssn),
            &(poly_ptr->ssmooth), poly_ptr->n, poly_ptr->vlist);
      poly_ptr->svlist = MakeMultiSplinePolyVertex(
            poly_ptr->curved, &(poly_ptr->sn), poly_ptr->ssmooth,
            drawOrigX, drawOrigY, poly_ptr->ssn, poly_ptr->ssvlist);
      break;
   case LT_INTSPLINE:
      if (poly_ptr->svlist != NULL) {
         free(poly_ptr->svlist);
      }
      if (poly_ptr->intvlist != NULL) {
         free(poly_ptr->intvlist);
      }
      if (!undoingOrRedoing && !dontAutoRetractArrow) {
         if (AutoRetractedArrowAttr(ObjPtr, FALSE)) {
            int new_x=0, new_y=0;
            IntPoint *v=NULL;

            if (poly_ptr->n != 3) {
               int x0, y0, x2, y2;

               v = poly_ptr->vlist;
               x0 = v[0].x; y0 = v[0].y;
               x2 = v[poly_ptr->n-1].x; y2 = v[poly_ptr->n-1].y;
               if (poly_ptr->smooth != NULL) free(poly_ptr->smooth);
               poly_ptr->smooth = (char*)malloc(4*sizeof(char));
               if (poly_ptr->smooth == NULL) FailAllocMessage();
               poly_ptr->smooth[0] = poly_ptr->smooth[1] =
                     poly_ptr->smooth[2] = FALSE;
               if (poly_ptr->vlist != NULL) free(poly_ptr->vlist);
               poly_ptr->vlist = (IntPoint*)malloc(4*sizeof(IntPoint));
               if (poly_ptr->vlist == NULL) FailAllocMessage();
               poly_ptr->vlist[0].x = x0; poly_ptr->vlist[0].y = y0;
               poly_ptr->vlist[2].x = x2; poly_ptr->vlist[2].y = y2;
               poly_ptr->n = 3;
            }
            v = poly_ptr->vlist;
            CalcAutoRetractedArrowAttrBend(poly_ptr->style,
                  v[0].x, v[0].y, v[2].x, v[2].y, &new_x, &new_y);
            v[1].x = new_x;
            v[1].y = new_y;
         }
      }
      poly_ptr->svlist = MakeIntSplinePolyVertex(
            &(poly_ptr->sn), &(poly_ptr->intn),
            &(poly_ptr->intvlist), drawOrigX, drawOrigY,
            poly_ptr->n, poly_ptr->vlist);
      break;
   }
   if (poly_ptr->style != LS_PLAIN) {
      IntPoint *v=poly_ptr->vlist, *v_copy;
      int i, n=poly_ptr->n, dx, dy, aw=poly_ptr->aw, retracted_arrow;
      double len, sin_val, cos_val;

      retracted_arrow = (RetractedArrowAttr(ObjPtr) ||
            AutoRetractedArrowAttr(ObjPtr, TRUE));
      if (poly_ptr->asvlist != NULL) {
         free(poly_ptr->asvlist);
      }
      switch (poly_ptr->curved) {
      case LT_STRAIGHT:
      case LT_SPLINE:
         v = poly_ptr->vlist;
         n = poly_ptr->n;
         break;
      case LT_STRUCT_SPLINE:
         v = poly_ptr->ssvlist;
         n = poly_ptr->ssn;
         break;
      case LT_INTSPLINE:
         v = poly_ptr->intvlist;
         n = poly_ptr->intn;
         break;
      }
      v_copy = (IntPoint *)malloc((n+1)*sizeof(IntPoint));
      if (v_copy == NULL) FailAllocMessage();
      for (i = 0; i < n; i++) {
         v_copy[i].x = v[i].x;
         v_copy[i].y = v[i].y;
      }
      if (aw == 0) aw = 1;

      dx = v_copy[1].x - v_copy[0].x;
      dy = v_copy[1].y - v_copy[0].y;
      if (!retracted_arrow && (poly_ptr->style & LS_LEFT) &&
            (dx != 0 || dy != 0)) {
         len = (double)sqrt((double)(((double)dx)*((double)dx) +
               ((double)dy)*((double)dy)));
         sin_val = ((double)dy)/len;
         cos_val = ((double)dx)/len;

         v_copy[0].x = round(v_copy[0].x+RETREAT*aw*cos_val);
         v_copy[0].y = round(v_copy[0].y+RETREAT*aw*sin_val);
      }
      dx = v_copy[n-1].x - v_copy[n-2].x;
      dy = v_copy[n-1].y - v_copy[n-2].y;
      if (!retracted_arrow && (poly_ptr->style & LS_RIGHT) &&
            (dx != 0 || dy != 0)) {
         len = (double)sqrt((double)(((double)dx)*((double)dx) +
               ((double)dy)*((double)dy)));
         sin_val = ((double)dy)/len;
         cos_val = ((double)dx)/len;

         v_copy[n-1].x = round(v_copy[n-1].x-RETREAT*aw*cos_val);
         v_copy[n-1].y = round(v_copy[n-1].y-RETREAT*aw*sin_val);
      }
      switch (poly_ptr->curved) {
      case LT_STRAIGHT:
      case LT_SPLINE:
         poly_ptr->asvlist = MakeMultiSplinePolyVertex(
               poly_ptr->curved, &(poly_ptr->asn), poly_ptr->smooth,
               drawOrigX, drawOrigY, n, v_copy);
         break;
      case LT_STRUCT_SPLINE:
         poly_ptr->asvlist = MakeMultiSplinePolyVertex(
               poly_ptr->curved, &(poly_ptr->asn), poly_ptr->ssmooth,
               drawOrigX, drawOrigY, n, v_copy);
         break;
      case LT_INTSPLINE:
         poly_ptr->asvlist = MakeSplinePolyVertex(0, poly_ptr->curved,
               &(poly_ptr->asn), drawOrigX, drawOrigY, n, v_copy);
         break;
      }
      free(v_copy);
   }
}

static
void AdjPolygonCache(ObjPtr)
   struct ObjRec *ObjPtr;
{
   struct PolygonRec *polygon_ptr=ObjPtr->detail.g;

   if (polygon_ptr->rotated_vlist != NULL) {
      free(polygon_ptr->rotated_vlist);
   }
   polygon_ptr->rotated_vlist = NULL;
   polygon_ptr->rotated_n = 0;

   switch (polygon_ptr->curved) {
   case LT_STRAIGHT:
      if (polygon_ptr->svlist != NULL) {
         free(polygon_ptr->svlist);
      }
      polygon_ptr->svlist = MakeMultiSplinePolygonVertex(
            polygon_ptr->curved, &(polygon_ptr->sn), polygon_ptr->smooth,
            drawOrigX, drawOrigY,
            polygon_ptr->n, polygon_ptr->vlist);
      break;
   case LT_SPLINE:
      if (polygon_ptr->svlist != NULL) {
         free(polygon_ptr->svlist);
      }
      polygon_ptr->svlist = MakeMultiSplinePolygonVertex(
            polygon_ptr->curved, &(polygon_ptr->sn), polygon_ptr->smooth,
            drawOrigX, drawOrigY,
            polygon_ptr->n, polygon_ptr->vlist);
      break;
   case LT_STRUCT_SPLINE:
      if (polygon_ptr->svlist != NULL) free(polygon_ptr->svlist);
      if (polygon_ptr->ssvlist != NULL) free(polygon_ptr->ssvlist);
      if (polygon_ptr->ssmooth != NULL) free(polygon_ptr->ssmooth);
      if (polygon_ptr->smooth != NULL) free(polygon_ptr->smooth);
#ifdef _TGIF_DBG /* debug, do not translate */
      TgAssert(polygon_ptr->n >= 7, "NumPts < 7 in AdjPolygonCache()", NULL);
#endif /* _TGIF_DBG */
      polygon_ptr->smooth = NULL;
      polygon_ptr->ssvlist = MakeStructuredSplinePolygonVertex(
            &(polygon_ptr->ssn), &(polygon_ptr->ssmooth), polygon_ptr->n,
            polygon_ptr->vlist);
      polygon_ptr->svlist = MakeMultiSplinePolygonVertex(
            polygon_ptr->curved, &(polygon_ptr->sn), polygon_ptr->ssmooth,
            drawOrigX, drawOrigY,
            polygon_ptr->ssn, polygon_ptr->ssvlist);
      break;
   case LT_INTSPLINE:
      if (polygon_ptr->svlist != NULL) {
         free(polygon_ptr->svlist);
      }
      if (polygon_ptr->intvlist != NULL) {
         free(polygon_ptr->intvlist);
      }
      polygon_ptr->svlist = MakeIntSplinePolygonVertex(
            &(polygon_ptr->sn), &(polygon_ptr->intn),
            &(polygon_ptr->intvlist), drawOrigX, drawOrigY,
            polygon_ptr->n, polygon_ptr->vlist);
      break;
   }
}

void AdjObjSplineVs(ObjPtr)
   struct ObjRec *ObjPtr;
{
   struct ObjRec *obj_ptr;

   switch (ObjPtr->type) {
   case OBJ_BOX: break;
   case OBJ_XBM: break;
   case OBJ_XPM: break;
   case OBJ_OVAL: InvalidateObjCache(ObjPtr); break;
   case OBJ_TEXT: break;
   case OBJ_RCBOX: InvalidateObjCache(ObjPtr); break;
   case OBJ_ARC: InvalidateObjCache(ObjPtr); AdjArcCache(ObjPtr); break;
   case OBJ_POLY: AdjPolyCache(ObjPtr); break;
   case OBJ_POLYGON: AdjPolygonCache(ObjPtr); break;

   case OBJ_GROUP:
   case OBJ_ICON:
   case OBJ_SYM:
   case OBJ_PIN:
      for (obj_ptr=ObjPtr->detail.r->first; obj_ptr != NULL;
            obj_ptr=obj_ptr->next) {
         AdjObjSplineVs(obj_ptr);
      }
      break;
   }
   GetTransformedOBBoxOffsetVs(ObjPtr, ObjPtr->rotated_obbox);
}

void AdjSplineVs()
{
   register struct ObjRec *obj_ptr, *ptr;

   if (topObj == NULL) return;

   dontAutoRetractArrow = TRUE;
   for (obj_ptr = topObj; obj_ptr != NULL; obj_ptr = obj_ptr->next) {
      switch (obj_ptr->type) {
      case OBJ_POLY:
      case OBJ_BOX:
      case OBJ_OVAL:
      case OBJ_TEXT:
      case OBJ_POLYGON:
      case OBJ_ARC:
      case OBJ_RCBOX:
      case OBJ_XBM:
      case OBJ_XPM:
         AdjObjSplineVs(obj_ptr);
         break;
      case OBJ_GROUP:
      case OBJ_ICON:
      case OBJ_SYM:
      case OBJ_PIN:
         for (ptr=obj_ptr->detail.r->first; ptr != NULL; ptr=ptr->next) {
            AdjObjSplineVs(ptr);
         }
         GetTransformedOBBoxOffsetVs(obj_ptr, obj_ptr->rotated_obbox);
         break;
      }
   }
   dontAutoRetractArrow = FALSE;
}

void ClearObjCachesInAllPages()
{
   struct ObjRec *saved_top_obj=topObj, *saved_bot_obj=botObj;
   struct PageRec *saved_cur_page=curPage;
 
   for (curPage=firstPage; curPage != NULL; curPage=curPage->next) {
      topObj = curPage->top;
      botObj = curPage->bot;
 
      AdjCaches();
      AdjSplineVs();
      curPage->draw_orig_x = drawOrigX;
      curPage->draw_orig_y = drawOrigY;
      curPage->zoom_scale = zoomScale;
      curPage->zoomed_in = zoomedIn;
   }
   topObj = saved_top_obj;
   botObj = saved_bot_obj;
   curPage = saved_cur_page;
}

void MoveRotatedObjCache(ObjPtr, AbsDx, AbsDy)
   struct ObjRec *ObjPtr;
   int AbsDx, AbsDy;
{
   register int i;
   int dx_off, dy_off, n=0;
   XPoint *v=NULL;

   if (ObjPtr->ctm == NULL) return;

   dx_off = ZOOMED_SIZE(AbsDx);
   dy_off = ZOOMED_SIZE(AbsDy);
   for (i=0; i < 5; i++) {
      ObjPtr->rotated_obbox[i].x += dx_off;
      ObjPtr->rotated_obbox[i].y += dy_off;
   }
   ObjPtr->orig_obbox.ltx += AbsDx; ObjPtr->orig_obbox.lty += AbsDy;
   ObjPtr->orig_obbox.rbx += AbsDx; ObjPtr->orig_obbox.rby += AbsDy;
   switch (ObjPtr->type) {
   case OBJ_POLY:
   case OBJ_POLYGON:
   case OBJ_OVAL:
   case OBJ_ARC:
   case OBJ_RCBOX:
      switch (ObjPtr->type) {
      case OBJ_POLY:
         n = ObjPtr->detail.p->rotated_n;
         v = ObjPtr->detail.p->rotated_vlist;
         break;
      case OBJ_POLYGON:
         n = ObjPtr->detail.g->rotated_n;
         v = ObjPtr->detail.g->rotated_vlist;
         break;
      case OBJ_OVAL:
         n = ObjPtr->detail.o->rotated_n;
         v = ObjPtr->detail.o->rotated_vlist;
         break;
      case OBJ_ARC:
         n = ObjPtr->detail.a->rotated_n;
         v = ObjPtr->detail.a->rotated_vlist;
         break;
      case OBJ_RCBOX:
         n = ObjPtr->detail.rcb->rotated_n;
         v = ObjPtr->detail.rcb->rotated_vlist;
         break;
      }
      if (v != NULL) {
         for (i=0; i < n; i++) {
            v[i].x += dx_off;
            v[i].y += dy_off;
         }
      }
      if (ObjPtr->type == OBJ_POLY) {
         n = ObjPtr->detail.p->rotated_asn;
         v = ObjPtr->detail.p->rotated_asvlist;
         if (v != NULL) {
            for (i=0; i < n; i++) {
               v[i].x += dx_off;
               v[i].y += dy_off;
            }
         }
      }
      break;
   case OBJ_BOX:
   case OBJ_GROUP:
   case OBJ_ICON:
   case OBJ_SYM:
   case OBJ_PIN:
   case OBJ_XBM:
   case OBJ_XPM:
      break;
   case OBJ_TEXT:
      ObjPtr->detail.t->orig_bbox.ltx += AbsDx;
      ObjPtr->detail.t->orig_bbox.lty += AbsDy;
      ObjPtr->detail.t->orig_bbox.rbx += AbsDx;
      ObjPtr->detail.t->orig_bbox.rby += AbsDy;
      break;
   }
}

void AdjHotSpots()
{
}

void AdjObjHotSpot()
{
}

int IsTopLevelObject(ObjPtr)
   struct ObjRec *ObjPtr;
{
   struct ObjRec *obj_ptr=NULL;

   for (obj_ptr=topObj; obj_ptr != NULL; obj_ptr=obj_ptr->next) {
      if (obj_ptr == ObjPtr) {
         return TRUE;
      }
   }
   return FALSE;
}

struct ObjRec *GetTopOwner(ObjPtr)
   struct ObjRec *ObjPtr;
{
   struct ObjRec *top_owner=ObjPtr;

   while (top_owner->tmp_parent != NULL) {
      top_owner = top_owner->tmp_parent;
   }
   return top_owner;
}
