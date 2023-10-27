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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/dup.c,v 1.13 2011/05/16 16:21:57 william Exp $
 */

#define _INCLUDE_FROM_DUP_C_

#include "tgifdefs.h"

#include "attr.e"
#include "auxtext.e"
#include "choice.e"
#include "cmd.e"
#include "drawing.e"
#include "dup.e"
#include "file.e"
#include "grid.e"
#include "mark.e"
#include "move.e"
#include "msg.e"
#include "obj.e"
#include "page.e"
#include "raster.e"
#include "select.e"
#include "setup.e"
#include "text.e"
#include "util.e"
#include "xbitmap.e"
#include "xpixmap.e"

int	justDupped = FALSE;
int	useRecentDupDistance = TRUE;
int	dupDx = INVALID, dupDy = INVALID;

void CopyObjId(FromObjPtr, ToObjPtr)
   struct ObjRec *FromObjPtr, *ToObjPtr;
{
   register struct ObjRec *from_obj_ptr, *to_obj_ptr;
   register struct AttrRec *from_attr_ptr, *to_attr_ptr;

   ToObjPtr->id = FromObjPtr->id;
   if (FromObjPtr->creator_full_id != NULL) {
      UtilFree(ToObjPtr->creator_full_id);
      ToObjPtr->creator_full_id = UtilStrDup(FromObjPtr->creator_full_id);
      if (ToObjPtr->creator_full_id == NULL) FailAllocMessage();
   }
   switch (FromObjPtr->type) {
   case OBJ_SYM:
   case OBJ_GROUP:
   case OBJ_ICON:
   case OBJ_PIN:
      from_obj_ptr = FromObjPtr->detail.r->first;
      to_obj_ptr = ToObjPtr->detail.r->first;
      for ( ; from_obj_ptr!=NULL; from_obj_ptr=from_obj_ptr->next,
            to_obj_ptr=to_obj_ptr->next) {
         CopyObjId(from_obj_ptr, to_obj_ptr);
      }
      break;
   }
   from_attr_ptr = FromObjPtr->fattr;
   to_attr_ptr = ToObjPtr->fattr;
   for ( ; from_attr_ptr!=NULL; from_attr_ptr=from_attr_ptr->next,
         to_attr_ptr=to_attr_ptr->next) {
      CopyObjId(from_attr_ptr->obj, to_attr_ptr->obj);
   }
}

void CopyObjLocks(FromObjPtr, ToObjPtr)
   struct ObjRec *FromObjPtr, *ToObjPtr;
{
   register struct ObjRec *from_obj_ptr, *to_obj_ptr;
   register struct AttrRec *from_attr_ptr, *to_attr_ptr;

   ToObjPtr->locked = FromObjPtr->locked;
   switch (FromObjPtr->type) {
   case OBJ_SYM:
   case OBJ_GROUP:
   case OBJ_ICON:
   case OBJ_PIN:
      from_obj_ptr = FromObjPtr->detail.r->first;
      to_obj_ptr = ToObjPtr->detail.r->first;
      for ( ; from_obj_ptr!=NULL; from_obj_ptr=from_obj_ptr->next,
            to_obj_ptr=to_obj_ptr->next) {
         CopyObjLocks(from_obj_ptr, to_obj_ptr);
      }
      break;
   }
   from_attr_ptr = FromObjPtr->fattr;
   to_attr_ptr = ToObjPtr->fattr;
   for ( ; from_attr_ptr!=NULL; from_attr_ptr=from_attr_ptr->next,
         to_attr_ptr=to_attr_ptr->next) {
      CopyObjLocks(from_attr_ptr->obj, to_attr_ptr->obj);
   }
}

void UnlockAnObj(ObjPtr)
   struct ObjRec *ObjPtr;
{
   register struct ObjRec *obj_ptr;
   register struct AttrRec *attr_ptr;

   ObjPtr->locked = FALSE;
   switch (ObjPtr->type) {
   case OBJ_SYM:
   case OBJ_GROUP:
   case OBJ_ICON:
   case OBJ_PIN:
      for (obj_ptr=ObjPtr->detail.r->first; obj_ptr!=NULL;
            obj_ptr=obj_ptr->next) {
         UnlockAnObj(obj_ptr);
      }
   }
   for (attr_ptr=ObjPtr->fattr; attr_ptr!=NULL; attr_ptr=attr_ptr->next) {
      attr_ptr->obj->locked = FALSE;
   }
}

void DupObjXfrmMtrx(FromObjPtr, ToObjPtr)
   register struct ObjRec *FromObjPtr, *ToObjPtr;
{
   ToObjPtr->ctm = NULL;
   if (FromObjPtr->ctm == NULL) return;
   ToObjPtr->ctm = (struct XfrmMtrxRec *)malloc(sizeof(struct XfrmMtrxRec));
   if (ToObjPtr->ctm == NULL) FailAllocMessage();
   memcpy(ToObjPtr->ctm, FromObjPtr->ctm, sizeof(struct XfrmMtrxRec));
   memcpy(&ToObjPtr->orig_obbox,&FromObjPtr->orig_obbox,sizeof(struct BBRec));
}

void DupObjBasics(FromObjPtr, ToObjPtr)
   register struct ObjRec *FromObjPtr, *ToObjPtr;
{
   ToObjPtr->next = ToObjPtr->prev = NULL;
   ToObjPtr->x = FromObjPtr->x;
   ToObjPtr->y = FromObjPtr->y;
   ToObjPtr->color = FromObjPtr->color;
   ToObjPtr->bg_color = FromObjPtr->bg_color;
   memcpy(ToObjPtr->color_str, FromObjPtr->color_str,
         sizeof(ToObjPtr->color_str));
   memcpy(ToObjPtr->bg_color_str, FromObjPtr->bg_color_str,
         sizeof(ToObjPtr->bg_color_str));
   ToObjPtr->id = objId++;
   ToObjPtr->dirty = FALSE;
   ToObjPtr->rotation = FromObjPtr->rotation;
   ToObjPtr->type = FromObjPtr->type;
   ToObjPtr->bbox.ltx = FromObjPtr->bbox.ltx;
   ToObjPtr->bbox.lty = FromObjPtr->bbox.lty;
   ToObjPtr->bbox.rbx = FromObjPtr->bbox.rbx;
   ToObjPtr->bbox.rby = FromObjPtr->bbox.rby;
   ToObjPtr->obbox.ltx = FromObjPtr->obbox.ltx;
   ToObjPtr->obbox.lty = FromObjPtr->obbox.lty;
   ToObjPtr->obbox.rbx = FromObjPtr->obbox.rbx;
   ToObjPtr->obbox.rby = FromObjPtr->obbox.rby;
   ToObjPtr->locked = FALSE;
   ToObjPtr->invisible = FromObjPtr->invisible;
   ToObjPtr->trans_pat = FromObjPtr->trans_pat;

   memcpy(&ToObjPtr->orig_obbox, &FromObjPtr->orig_obbox,
         sizeof(struct BBRec));
   memcpy(ToObjPtr->rotated_obbox, FromObjPtr->rotated_obbox,
         5*sizeof(XPoint));
   DupObjXfrmMtrx(FromObjPtr, ToObjPtr);
}

void DupPolyObj(PolyPtr, ObjPtr)
   struct PolyRec *PolyPtr;
   struct ObjRec *ObjPtr;
{
   register int i, num_pts;
   register struct PolyRec *poly_ptr;
   register XPoint *sv;
   register IntPoint *pv;

   poly_ptr = (struct PolyRec *)malloc(sizeof(struct PolyRec));
   if (poly_ptr == NULL) FailAllocMessage();
   memset(poly_ptr, 0, sizeof(struct PolyRec));
   num_pts = poly_ptr->n = PolyPtr->n;
   pv = (IntPoint*)malloc((num_pts+1)*sizeof(IntPoint));
   if (pv == NULL) FailAllocMessage();
   if (PolyPtr->smooth != NULL) {
      poly_ptr->smooth = (char*)malloc((num_pts+1)*sizeof(char));
      if (poly_ptr->smooth == NULL) FailAllocMessage();
   } else {
      poly_ptr->smooth = NULL;
   }
   for (i=0; i < num_pts; i++) {
      pv[i].x = PolyPtr->vlist[i].x;
      pv[i].y = PolyPtr->vlist[i].y;
      if (PolyPtr->smooth != NULL && poly_ptr->smooth != NULL) {
         poly_ptr->smooth[i] = PolyPtr->smooth[i];
      }
   }
   poly_ptr->vlist = pv;
   poly_ptr->style = PolyPtr->style;
   poly_ptr->width = PolyPtr->width;
   poly_ptr->aw = PolyPtr->aw;
   poly_ptr->ah = PolyPtr->ah;
   strcpy(poly_ptr->width_spec, PolyPtr->width_spec);
   strcpy(poly_ptr->aw_spec, PolyPtr->aw_spec);
   strcpy(poly_ptr->ah_spec, PolyPtr->ah_spec);
   poly_ptr->pen = PolyPtr->pen;
   poly_ptr->fill = PolyPtr->fill;
   poly_ptr->curved = PolyPtr->curved;
   if (PolyPtr->sn != 0 && PolyPtr->svlist != NULL) {
      poly_ptr->sn = num_pts = PolyPtr->sn;
      sv = (XPoint*)malloc((num_pts+1)*sizeof(XPoint));
      if (sv == NULL) FailAllocMessage();
      for (i=0; i < num_pts; i++) {
         sv[i].x = PolyPtr->svlist[i].x;
         sv[i].y = PolyPtr->svlist[i].y;
      }
      poly_ptr->svlist = sv;
   } else {
      poly_ptr->sn = 0;
      poly_ptr->svlist = NULL;
   }
   if (PolyPtr->ssn != 0 && PolyPtr->ssvlist != NULL) {
      poly_ptr->ssn = num_pts = PolyPtr->ssn;
      pv = (IntPoint*)malloc((num_pts+1)*sizeof(IntPoint));
      if (pv == NULL) FailAllocMessage();
      for (i=0; i < num_pts; i++) {
         pv[i].x = PolyPtr->ssvlist[i].x;
         pv[i].y = PolyPtr->ssvlist[i].y;
      }
      poly_ptr->ssvlist = pv;
   } else {
      poly_ptr->ssn = 0;
      poly_ptr->ssvlist = NULL;
   }
   if (PolyPtr->ssmooth != NULL && PolyPtr->ssvlist != NULL) {
      num_pts = PolyPtr->ssn;
      poly_ptr->ssmooth = (char*)malloc((num_pts+1)*sizeof(char));
      if (poly_ptr->ssmooth == NULL) FailAllocMessage();
      memcpy(poly_ptr->ssmooth, PolyPtr->ssmooth, num_pts*sizeof(char));
      poly_ptr->ssmooth[num_pts] = FALSE;
   } else {
      poly_ptr->ssmooth = NULL;
   }
   if (PolyPtr->curved == LT_INTSPLINE && PolyPtr->intn != 0 &&
         PolyPtr->intvlist != NULL) {
      poly_ptr->intn = num_pts = PolyPtr->intn;
      pv = (IntPoint*)malloc((num_pts+1)*sizeof(IntPoint));
      if (pv == NULL) FailAllocMessage();
      for (i=0; i < num_pts; i++) {
         pv[i].x = PolyPtr->intvlist[i].x;
         pv[i].y = PolyPtr->intvlist[i].y;
      }
      poly_ptr->intvlist = pv;
   } else {
      poly_ptr->intn = 0;
      poly_ptr->intvlist = NULL;
   }
   poly_ptr->dash = PolyPtr->dash;
   if (ObjPtr->ctm != NULL && PolyPtr->rotated_n != 0 &&
         PolyPtr->rotated_vlist != NULL) {
      poly_ptr->rotated_n = num_pts = PolyPtr->rotated_n;
      sv = (XPoint*)malloc((num_pts+1)*sizeof(XPoint));
      if (sv == NULL) FailAllocMessage();
      for (i=0; i < num_pts; i++) {
         sv[i].x = PolyPtr->rotated_vlist[i].x;
         sv[i].y = PolyPtr->rotated_vlist[i].y;
      }
      poly_ptr->rotated_vlist = sv;
   } else {
      poly_ptr->rotated_n = 0;
      poly_ptr->rotated_vlist = NULL;
   }
/*
 * Need to check for pins.
 * Need to handle the case where connection objects need to be dupped!
 */
   poly_ptr->start_conn = poly_ptr->end_conn = NULL;

   ObjPtr->detail.p = poly_ptr;
}

void DupPolygonObj(PolygonPtr, ObjPtr)
   struct PolygonRec *PolygonPtr;
   struct ObjRec *ObjPtr;
{
   register int i, num_pts;
   register struct PolygonRec *polygon_ptr;
   XPoint *sv;
   IntPoint *pv;

   polygon_ptr = (struct PolygonRec *)malloc(sizeof(struct PolygonRec));
   if (polygon_ptr == NULL) FailAllocMessage();
   memset(polygon_ptr, 0, sizeof(struct PolygonRec));
   num_pts = polygon_ptr->n = PolygonPtr->n;
   pv = (IntPoint*)malloc((num_pts+1)*sizeof(IntPoint));
   if (pv == NULL) FailAllocMessage();
   if (PolygonPtr->smooth != NULL) {
      polygon_ptr->smooth = (char*)malloc((num_pts+1)*sizeof(char));
      if (polygon_ptr->smooth == NULL) FailAllocMessage();
   } else {
      polygon_ptr->smooth = NULL;
   }
   for (i=0; i < num_pts; i++) {
      pv[i].x = PolygonPtr->vlist[i].x;
      pv[i].y = PolygonPtr->vlist[i].y;
      if (PolygonPtr->smooth != NULL && polygon_ptr->smooth != NULL) {
         polygon_ptr->smooth[i] = PolygonPtr->smooth[i];
      }
   }
   polygon_ptr->vlist = pv;
   polygon_ptr->fill = PolygonPtr->fill;
   polygon_ptr->width = PolygonPtr->width;
   strcpy(polygon_ptr->width_spec, PolygonPtr->width_spec);
   polygon_ptr->pen = PolygonPtr->pen;
   polygon_ptr->curved = PolygonPtr->curved;
   if (PolygonPtr->sn != 0 && PolygonPtr->svlist != NULL) {
      polygon_ptr->sn = num_pts = PolygonPtr->sn;
      sv = (XPoint*)malloc((num_pts+1)*sizeof(XPoint));
      if (sv == NULL) FailAllocMessage();
      for (i=0; i < num_pts; i++) {
         sv[i].x = PolygonPtr->svlist[i].x;
         sv[i].y = PolygonPtr->svlist[i].y;
      }
      polygon_ptr->svlist = sv;
   } else {
      polygon_ptr->sn = 0;
      polygon_ptr->svlist = NULL;
   }
   if (PolygonPtr->ssn != 0 && PolygonPtr->ssvlist != NULL) {
      polygon_ptr->ssn = num_pts = PolygonPtr->ssn;
      pv = (IntPoint*)malloc((num_pts+1)*sizeof(IntPoint));
      if (pv == NULL) FailAllocMessage();
      for (i=0; i < num_pts; i++) {
         pv[i].x = PolygonPtr->ssvlist[i].x;
         pv[i].y = PolygonPtr->ssvlist[i].y;
      }
      polygon_ptr->ssvlist = pv;
   } else {
      polygon_ptr->ssn = 0;
      polygon_ptr->ssvlist = NULL;
   }
   if (PolygonPtr->ssmooth != NULL && PolygonPtr->ssvlist != NULL) {
      num_pts = PolygonPtr->ssn;
      polygon_ptr->ssmooth = (char*)malloc((num_pts+1)*sizeof(char));
      if (polygon_ptr->ssmooth == NULL) FailAllocMessage();
      memcpy(polygon_ptr->ssmooth, PolygonPtr->ssmooth, num_pts*sizeof(char));
      polygon_ptr->ssmooth[num_pts] = FALSE;
   } else {
      polygon_ptr->ssmooth = NULL;
   }
   if (PolygonPtr->curved == LT_INTSPLINE && PolygonPtr->intn != 0 &&
         PolygonPtr->intvlist != NULL) {
      polygon_ptr->intn = num_pts = PolygonPtr->intn;
      pv = (IntPoint*)malloc((num_pts+1)*sizeof(IntPoint));
      if (pv == NULL) FailAllocMessage();
      for (i=0; i < num_pts; i++) {
         pv[i].x = PolygonPtr->intvlist[i].x;
         pv[i].y = PolygonPtr->intvlist[i].y;
      }
      polygon_ptr->intvlist = pv;
   } else {
      polygon_ptr->intn = 0;
      polygon_ptr->intvlist = NULL;
   }
   polygon_ptr->dash = PolygonPtr->dash;
   if (ObjPtr->ctm != NULL && PolygonPtr->rotated_n != 0 &&
         PolygonPtr->rotated_vlist != NULL) {
      polygon_ptr->rotated_n = num_pts = PolygonPtr->rotated_n;
      sv = (XPoint*)malloc((num_pts+1)*sizeof(XPoint));
      if (sv == NULL) FailAllocMessage();
      for (i=0; i < num_pts; i++) {
         sv[i].x = PolygonPtr->rotated_vlist[i].x;
         sv[i].y = PolygonPtr->rotated_vlist[i].y;
      }
      polygon_ptr->rotated_vlist = sv;
   } else {
      polygon_ptr->rotated_n = 0;
      polygon_ptr->rotated_vlist = NULL;
   }

   ObjPtr->detail.g = polygon_ptr;
}

void DupOvalObj(OvalPtr, ObjPtr)
   struct OvalRec *OvalPtr;
   struct ObjRec *ObjPtr;
{
   register struct OvalRec *oval_ptr;

   oval_ptr = (struct OvalRec *)malloc(sizeof(struct OvalRec));
   if (oval_ptr == NULL) FailAllocMessage();
   memset(oval_ptr, 0, sizeof(struct OvalRec));
   oval_ptr->fill = OvalPtr->fill;
   oval_ptr->width = OvalPtr->width;
   strcpy(oval_ptr->width_spec, OvalPtr->width_spec);
   oval_ptr->pen = OvalPtr->pen;
   oval_ptr->dash = OvalPtr->dash;
   if (ObjPtr->ctm != NULL && OvalPtr->rotated_n != 0 &&
         OvalPtr->rotated_vlist != NULL) {
      int i, num_pts;
      XPoint *v;

      oval_ptr->rotated_n = num_pts = OvalPtr->rotated_n;
      v = (XPoint*)malloc((num_pts+1)*sizeof(XPoint));
      if (v == NULL) FailAllocMessage();
      for (i=0; i < num_pts; i++) {
         v[i].x = OvalPtr->rotated_vlist[i].x;
         v[i].y = OvalPtr->rotated_vlist[i].y;
      }
      oval_ptr->rotated_vlist = v;
   } else {
      oval_ptr->rotated_n = 0;
      oval_ptr->rotated_vlist = NULL;
   }

   ObjPtr->detail.o = oval_ptr;
}

void DupBoxObj(BoxPtr, ObjPtr)
   struct BoxRec *BoxPtr;
   struct ObjRec *ObjPtr;
{
   register struct BoxRec *box_ptr;

   box_ptr = (struct BoxRec *)malloc(sizeof(struct BoxRec));
   if (box_ptr == NULL) FailAllocMessage();
   memset(box_ptr, 0, sizeof(struct BoxRec));
   box_ptr->fill = BoxPtr->fill;
   box_ptr->width = BoxPtr->width;
   strcpy(box_ptr->width_spec, BoxPtr->width_spec);
   box_ptr->pen = BoxPtr->pen;
   box_ptr->dash = BoxPtr->dash;

   ObjPtr->detail.b = box_ptr;
}

void DupRCBoxObj(RCBoxPtr, ObjPtr)
   struct RCBoxRec *RCBoxPtr;
   struct ObjRec *ObjPtr;
{
   register struct RCBoxRec *rcbox_ptr;

   rcbox_ptr = (struct RCBoxRec *)malloc(sizeof(struct RCBoxRec));
   if (rcbox_ptr == NULL) FailAllocMessage();
   memset(rcbox_ptr, 0, sizeof(struct RCBoxRec));
   rcbox_ptr->fill = RCBoxPtr->fill;
   rcbox_ptr->width = RCBoxPtr->width;
   strcpy(rcbox_ptr->width_spec, RCBoxPtr->width_spec);
   rcbox_ptr->pen = RCBoxPtr->pen;
   rcbox_ptr->dash = RCBoxPtr->dash;
   rcbox_ptr->radius = RCBoxPtr->radius;
   if (ObjPtr->ctm != NULL && RCBoxPtr->rotated_n != 0 &&
         RCBoxPtr->rotated_vlist != NULL) {
      int i, num_pts;
      XPoint *v;

      rcbox_ptr->rotated_n = num_pts = RCBoxPtr->rotated_n;
      v = (XPoint*)malloc((num_pts+1)*sizeof(XPoint));
      if (v == NULL) FailAllocMessage();
      for (i=0; i < num_pts; i++) {
         v[i].x = RCBoxPtr->rotated_vlist[i].x;
         v[i].y = RCBoxPtr->rotated_vlist[i].y;
      }
      rcbox_ptr->rotated_vlist = v;
   } else {
      rcbox_ptr->rotated_n = 0;
      rcbox_ptr->rotated_vlist = NULL;
   }

   ObjPtr->detail.rcb = rcbox_ptr;
}

void DupArcObj(ArcPtr, ObjPtr)
   struct ArcRec *ArcPtr;
   struct ObjRec *ObjPtr;
{
   register struct ArcRec *arc_ptr;

   arc_ptr = (struct ArcRec *)malloc(sizeof(struct ArcRec));
   if (arc_ptr == NULL) FailAllocMessage();
   memset(arc_ptr, 0, sizeof(struct ArcRec));
   arc_ptr->fill = ArcPtr->fill;
   arc_ptr->width = ArcPtr->width;
   arc_ptr->aw = ArcPtr->aw;
   arc_ptr->ah = ArcPtr->ah;
   strcpy(arc_ptr->width_spec, ArcPtr->width_spec);
   strcpy(arc_ptr->aw_spec, ArcPtr->aw_spec);
   strcpy(arc_ptr->ah_spec, ArcPtr->ah_spec);
   arc_ptr->pen = ArcPtr->pen;
   arc_ptr->dash = ArcPtr->dash;
   arc_ptr->style = ArcPtr->style;

   arc_ptr->xc = ArcPtr->xc;             arc_ptr->yc = ArcPtr->yc;
   arc_ptr->x1 = ArcPtr->x1;             arc_ptr->y1 = ArcPtr->y1;
   arc_ptr->x2 = ArcPtr->x2;             arc_ptr->y2 = ArcPtr->y2;
   arc_ptr->dir = ArcPtr->dir;
   arc_ptr->ltx = ArcPtr->ltx;           arc_ptr->lty = ArcPtr->lty;
   arc_ptr->w = ArcPtr->w;               arc_ptr->h = ArcPtr->h;
   arc_ptr->angle1 = ArcPtr->angle1;     arc_ptr->angle2 = ArcPtr->angle2;
   arc_ptr->a_angle1 = ArcPtr->a_angle1; arc_ptr->a_angle2 = ArcPtr->a_angle2;
   if (ObjPtr->ctm != NULL && ArcPtr->rotated_n != 0 &&
         ArcPtr->rotated_vlist != NULL) {
      int i, num_pts;
      XPoint *v;

      arc_ptr->rotated_n = num_pts = ArcPtr->rotated_n;
      v = (XPoint*)malloc((num_pts+2)*sizeof(XPoint));
      if (v == NULL) FailAllocMessage();
      for (i=0; i < num_pts+2; i++) {
         v[i].x = ArcPtr->rotated_vlist[i].x;
         v[i].y = ArcPtr->rotated_vlist[i].y;
      }
      arc_ptr->rotated_vlist = v;
   } else {
      arc_ptr->rotated_n = 0;
      arc_ptr->rotated_vlist = NULL;
   }

   ObjPtr->detail.a = arc_ptr;
}

void DupXBmObj(XBmPtr, ObjPtr)
   struct XBmRec *XBmPtr;
   struct ObjRec *ObjPtr;
{
   register struct XBmRec *xbm_ptr;
   register int i;
   Pixmap bitmap, cached_bitmap;
   int w, h, image_w, image_h;

   xbm_ptr = (struct XBmRec *)malloc(sizeof(struct XBmRec));
   if (xbm_ptr == NULL) FailAllocMessage();
   memset(xbm_ptr, 0, sizeof(struct XBmRec));
   ObjPtr->detail.xbm = xbm_ptr;

   xbm_ptr->image = NULL;
   xbm_ptr->data = NULL;
   xbm_ptr->fill = XBmPtr->fill;
   xbm_ptr->image_w = image_w = XBmPtr->image_w;
   xbm_ptr->image_h = image_h = XBmPtr->image_h;
   xbm_ptr->flip = XBmPtr->flip;
   xbm_ptr->cached_zoom = XBmPtr->cached_zoom;
   xbm_ptr->cached_zoomed = XBmPtr->cached_zoomed;
   xbm_ptr->cached_flip = XBmPtr->cached_flip;
   xbm_ptr->cached_w = XBmPtr->cached_w;
   xbm_ptr->cached_h = XBmPtr->cached_h;

   xbm_ptr->eps_w = XBmPtr->eps_w;
   xbm_ptr->eps_h = XBmPtr->eps_h;

   xbm_ptr->bitmap = None;
   xbm_ptr->cached_bitmap = None;

   *xbm_ptr->write_date = '\0';
   xbm_ptr->save_epsf = XBmPtr->save_epsf;
   xbm_ptr->real_type = XBmPtr->real_type;

   if (ObjPtr->ctm != NULL) {
      memcpy(&xbm_ptr->cached_ctm, &XBmPtr->cached_ctm,
            sizeof(struct XfrmMtrxRec));
   }
   if (XBmPtr->real_type==XBM_XBM || (XBmPtr->real_type==XBM_EPS &&
         XBmPtr->bitmap!=None)) {
      bitmap = XCreatePixmap(mainDisplay, mainWindow, image_w, image_h, 1);
      if (bitmap == None) {
         FailAllocBitmapMessage(image_w, image_h);
         return;
      }
      XCopyArea(mainDisplay, XBmPtr->bitmap, bitmap, xbmGC, 0, 0,
            image_w, image_h, 0, 0);
      xbm_ptr->bitmap = bitmap;
      if (XBmPtr->cached_bitmap != None) {
         w = ZOOMED_SIZE(ObjPtr->obbox.rbx - ObjPtr->obbox.ltx);
         h = ZOOMED_SIZE(ObjPtr->obbox.rby - ObjPtr->obbox.lty);

         cached_bitmap = XCreatePixmap(mainDisplay, mainWindow, w, h, 1);
         if (cached_bitmap == None) {
            FailAllocBitmapMessage(w, h);
            return;
         }
         XCopyArea(mainDisplay, XBmPtr->cached_bitmap, cached_bitmap, xbmGC,
               0, 0, w, h, 0, 0);
         xbm_ptr->cached_bitmap = cached_bitmap;
      }
   }
   if (XBmPtr->real_type == XBM_EPS) {
      int len=strlen(XBmPtr->filename);
      int num_epsf_lines=XBmPtr->num_epsf_lines;

      xbm_ptr->llx = XBmPtr->llx; xbm_ptr->lly = XBmPtr->lly;
      xbm_ptr->urx = XBmPtr->urx; xbm_ptr->ury = XBmPtr->ury;
      xbm_ptr->filename = (char*)malloc((len+1)*sizeof(char));
      if (xbm_ptr->filename == NULL) FailAllocMessage();
      strcpy(xbm_ptr->filename, XBmPtr->filename);
      strcpy(xbm_ptr->write_date, XBmPtr->write_date);
      if (num_epsf_lines > 0) {
         xbm_ptr->num_epsf_lines = num_epsf_lines;
         xbm_ptr->epsflines = (char**)malloc(num_epsf_lines*sizeof(char*));
         if (xbm_ptr->epsflines == NULL) FailAllocMessage();
         for (i=0; i < num_epsf_lines; i++) {
            len = strlen(XBmPtr->epsflines[i]);
            xbm_ptr->epsflines[i] = (char*)malloc((len+1)*sizeof(char));
            if (xbm_ptr->epsflines[i] == NULL) FailAllocMessage();
            strcpy(xbm_ptr->epsflines[i], XBmPtr->epsflines[i]);
         }
      }
   }
}

void DupXPmObj(XPmPtr, ObjPtr)
   struct XPmRec *XPmPtr;
   struct ObjRec *ObjPtr;
{
   register struct XPmRec *xpm_ptr=NULL;
   Pixmap pixmap=None, bitmap=None, cached_pixmap=None, cached_bitmap=None;
   int w=0, h=0, ncolors=0, image_w=0, image_h=0;

   xpm_ptr = (struct XPmRec *)malloc(sizeof(struct XPmRec));
   if (xpm_ptr == NULL) FailAllocMessage();
   memset(xpm_ptr, 0, sizeof(struct XPmRec));
   ObjPtr->detail.xpm = xpm_ptr;

   xpm_ptr->image = NULL;
   xpm_ptr->bitmap_image = NULL;
   xpm_ptr->data = NULL;
   xpm_ptr->fill = XPmPtr->fill;
   xpm_ptr->image_w = image_w = XPmPtr->image_w;
   xpm_ptr->image_h = image_h = XPmPtr->image_h;
   xpm_ptr->flip = XPmPtr->flip;
   xpm_ptr->cached_zoom = XPmPtr->cached_zoom;
   xpm_ptr->cached_zoomed = XPmPtr->cached_zoomed;
   xpm_ptr->cached_flip = XPmPtr->cached_flip;
   xpm_ptr->cached_w = XPmPtr->cached_w;
   xpm_ptr->cached_h = XPmPtr->cached_h;
   xpm_ptr->cached_color = (-1);

   xpm_ptr->pixmap = None;
   xpm_ptr->bitmap = None;
   xpm_ptr->cached_pixmap = None;
   xpm_ptr->cached_bitmap = None;
   xpm_ptr->clip_mask = None;

   xpm_ptr->real_type = XPmPtr->real_type;
   xpm_ptr->linked_jpeg = FALSE;
   xpm_ptr->filename = NULL;

   if (xpm_ptr->real_type == XPM_JPEG) {
      xpm_ptr->linked_jpeg = XPmPtr->linked_jpeg;
      if (XPmPtr->filename != NULL) {
         xpm_ptr->filename = UtilStrDup(XPmPtr->filename);
         if (xpm_ptr->filename == NULL) FailAllocMessage();
      }
   } else if (xpm_ptr->real_type == PPM_TRUE) {
      xpm_ptr->ppm_data_compress = XPmPtr->ppm_data_compress;
      if (xpm_ptr->ppm_data_compress == PPM_JPEG_COMPRESS ||
            xpm_ptr->ppm_data_compress == PPM_DATA_DEFLATED) {
         xpm_ptr->ppm_mask_size = XPmPtr->ppm_mask_size;
         if (xpm_ptr->ppm_mask_size > 0) {
            xpm_ptr->ppm_mask_data = (char*)malloc(xpm_ptr->ppm_mask_size);
            if (xpm_ptr->ppm_mask_data == NULL) FailAllocMessage();
            memcpy(xpm_ptr->ppm_mask_data, XPmPtr->ppm_mask_data,
                  xpm_ptr->ppm_mask_size);
         }
         xpm_ptr->ppm_data_size = XPmPtr->ppm_data_size;
         xpm_ptr->ppm_data = (char*)malloc(xpm_ptr->ppm_data_size);
         if (xpm_ptr->ppm_data == NULL) FailAllocMessage();
         memcpy(xpm_ptr->ppm_data, XPmPtr->ppm_data, xpm_ptr->ppm_data_size);
         xpm_ptr->has_transparent_color = XPmPtr->has_transparent_color;
         memcpy(xpm_ptr->transparent_color, XPmPtr->transparent_color,
               sizeof(xpm_ptr->transparent_color));
      }
   }
   if (ObjPtr->ctm != NULL) {
      memcpy(&xpm_ptr->cached_ctm, &XPmPtr->cached_ctm,
            sizeof(struct XfrmMtrxRec));
   }
   pixmap = XCreatePixmap(mainDisplay,mainWindow,image_w,image_h,mainDepth);
   if (pixmap == None) {
      FailAllocPixmapMessage(image_w, image_h);
      return;
   }
   XCopyArea(mainDisplay,XPmPtr->pixmap,pixmap,xpmGC,0,0,image_w,image_h,0,0);
   xpm_ptr->pixmap = pixmap;

   bitmap = XCreatePixmap(mainDisplay,mainWindow,image_w,image_h,1);
   if (bitmap == None) {
      FailAllocBitmapMessage(image_w, image_h);
      return;
   }
   XCopyArea(mainDisplay,XPmPtr->bitmap,bitmap,xbmGC,0,0,image_w,image_h,0,0);
   xpm_ptr->bitmap = bitmap;

   w = ZOOMED_SIZE(ObjPtr->obbox.rbx - ObjPtr->obbox.ltx);
   h = ZOOMED_SIZE(ObjPtr->obbox.rby - ObjPtr->obbox.lty);

   if (XPmPtr->cached_pixmap != None) {
      cached_pixmap = XCreatePixmap(mainDisplay, mainWindow, w, h, mainDepth);
      if (cached_pixmap == None) {
         FailAllocPixmapMessage(w, h);
         return;
      }
      XCopyArea(mainDisplay, XPmPtr->cached_pixmap, cached_pixmap, xpmGC, 0, 0,
            w, h, 0, 0);
      xpm_ptr->cached_pixmap = cached_pixmap;
   }
   if (XPmPtr->cached_bitmap != None) {
      cached_bitmap = XCreatePixmap(mainDisplay, mainWindow, w, h, 1);
      if (cached_bitmap == None) {
         FailAllocBitmapMessage(w, h);
         return;
      }
      XCopyArea(mainDisplay, XPmPtr->cached_bitmap, cached_bitmap, xbmGC, 0, 0,
            w, h, 0, 0);
      xpm_ptr->cached_bitmap = cached_bitmap;
   }
   ncolors = xpm_ptr->ncolors = XPmPtr->ncolors;

   if (xpm_ptr->real_type != PPM_TRUE) {
      int i=0, *pixels=NULL, chars_per_pixel=0;
      char *color_char=NULL, **color_str=NULL;

      chars_per_pixel = xpm_ptr->chars_per_pixel = XPmPtr->chars_per_pixel;
      xpm_ptr->first_pixel_is_bg = XPmPtr->first_pixel_is_bg;

      color_char = xpm_ptr->color_char =
            (char*)malloc(ncolors*chars_per_pixel*sizeof(char));
      color_str = xpm_ptr->color_str = (char**)malloc(ncolors*sizeof(char*));
      pixels = xpm_ptr->pixels = (int*)malloc(ncolors*sizeof(int));
      if (color_char==NULL || color_str==NULL || pixels==NULL) {
         FailAllocMessage();
      }
      for (i=0; i < ncolors; i++) {
         int j=0, len=0;

         for (j=0; j < chars_per_pixel; j++) {
            color_char[i*chars_per_pixel+j] =
                  XPmPtr->color_char[i*chars_per_pixel+j];
         }
         pixels[i] = XPmPtr->pixels[i];

         len = strlen(XPmPtr->color_str[i]);
         color_str[i] = (char*)malloc((len+1)*sizeof(char));
         if (color_str[i] == NULL) FailAllocMessage();
         strcpy(color_str[i], XPmPtr->color_str[i]);
      }
   }
   xpm_ptr->red = xpm_ptr->green = xpm_ptr->blue = NULL;
   if (ObjPtr->ctm != NULL && XPmPtr->clip_mask != None) {
      w = ZOOMED_SIZE(ObjPtr->obbox.rbx - ObjPtr->obbox.ltx);
      h = ZOOMED_SIZE(ObjPtr->obbox.rby - ObjPtr->obbox.lty);

      pixmap = XCreatePixmap(mainDisplay, mainWindow, w, h, 1);
      if (pixmap == None) {
         FailAllocBitmapMessage(w, h);
         return;
      }
      XCopyArea(mainDisplay, XPmPtr->clip_mask, pixmap, xbmGC, 0, 0,
            w, h, 0, 0);
      xpm_ptr->clip_mask = pixmap;
   }
}

void DupStrSeg(pToStrBlock, pFromStrSeg)
   StrBlockInfo *pToStrBlock;
   StrSegInfo *pFromStrSeg;
   /* pToStrBlock->type is either SB_SIMPLE or SB_SUPSUB_CENTER */
   /* pToStrBlock->seg is the newly created string segment */
{
   StrSegInfo *str_seg=(StrSegInfo*)malloc(sizeof(StrSegInfo));

   if (str_seg == NULL) FailAllocMessage();
   memcpy(str_seg, pFromStrSeg, sizeof(StrSegInfo));
   if (PRTGIF && pFromStrSeg->font_name != NULL &&
         *pFromStrSeg->font_name != '\0') {
      /*
       * this should never happen anyway (because DupStrSeg() is never called
       *       in PRTGIF)
       */
      str_seg->font_name = UtilStrDup(pFromStrSeg->font_name);
      if (str_seg->font_name == NULL) FailAllocMessage();
   }
   str_seg->dyn_str.s = NULL;
   str_seg->dyn_str.sz = 0;
   DynStrCpy(&str_seg->dyn_str, &pFromStrSeg->dyn_str);
   str_seg->owner = pToStrBlock;
   pToStrBlock->seg = str_seg;
}

void DupStrBlock(pStrBlock, pMiniLineOwner, ppFirstStrBlock, ppLastStrBlock)
   StrBlockInfo *pStrBlock, **ppFirstStrBlock, **ppLastStrBlock;
   MiniLineInfo *pMiniLineOwner;
{
   StrBlockInfo *str_block=(StrBlockInfo*)malloc(sizeof(StrBlockInfo));

   if (str_block == NULL) FailAllocMessage();
   memcpy(str_block, pStrBlock, sizeof(StrBlockInfo));
   str_block->seg = NULL;
   str_block->sup = str_block->sub = NULL;
   str_block->owner_mini_line = pMiniLineOwner;
   str_block->next = NULL;
   str_block->prev = (*ppLastStrBlock);
   if ((*ppLastStrBlock) == NULL) {
      (*ppFirstStrBlock) = str_block;
   } else {
      (*ppLastStrBlock)->next = str_block;
   }
   (*ppLastStrBlock) = str_block;
   switch (pStrBlock->type) {
   case SB_SIMPLE:
      DupStrSeg(str_block, pStrBlock->seg);
      break;
   case SB_CHAR_SPACE: break;

   case SB_SUPSUB_LEFT:
   case SB_SUPSUB_CENTER:
   case SB_SUPSUB_RIGHT:
      if (pStrBlock->sup != NULL) {
         DupMiniLines(pStrBlock->sup, str_block, &str_block->sup);
      }
      if (pStrBlock->sub != NULL) {
         DupMiniLines(pStrBlock->sub, str_block, &str_block->sub);
      }
      if (pStrBlock->type == SB_SUPSUB_CENTER) {
         DupStrSeg(str_block, pStrBlock->seg);
      }
      break;
   }
}

void DupMiniLine(pMiniLine, pOwnerMiniLines, pOwnerBlock, ppFirstMiniLine,
      ppLastMiniLine)
   MiniLineInfo *pMiniLine, **ppFirstMiniLine, **ppLastMiniLine;
   MiniLinesInfo *pOwnerMiniLines;
   StrBlockInfo *pOwnerBlock;
{
   MiniLineInfo *new_mini_line=(MiniLineInfo*)malloc(sizeof(MiniLineInfo));
   StrBlockInfo *pStrBlock=NULL;

   if (new_mini_line == NULL) FailAllocMessage();
   memcpy(new_mini_line, pMiniLine, sizeof(MiniLineInfo));
   new_mini_line->first_block = new_mini_line->last_block = NULL;
   new_mini_line->owner_minilines = pOwnerMiniLines;
   new_mini_line->next = NULL;
   new_mini_line->prev = (*ppLastMiniLine);
   if ((*ppLastMiniLine) == NULL) {
      (*ppFirstMiniLine) = new_mini_line;
   } else {
      (*ppLastMiniLine)->next = new_mini_line;
   }
   (*ppLastMiniLine) = new_mini_line;
   for (pStrBlock=pMiniLine->first_block; pStrBlock != NULL;
         pStrBlock=pStrBlock->next) {
      DupStrBlock(pStrBlock, new_mini_line, &new_mini_line->first_block,
            &new_mini_line->last_block);
   }
}

void DupMiniLines(minilines, pOwnerBlock, ppMinilines)
   MiniLinesInfo *minilines, **ppMinilines;
   StrBlockInfo *pOwnerBlock;
{
   MiniLinesInfo *new_minilines=(MiniLinesInfo*)malloc(sizeof(MiniLinesInfo));
   MiniLineInfo *pMiniLine=NULL, *pFirstMiniLine=NULL, *pLastMiniLine=NULL;

   if (new_minilines == NULL) FailAllocMessage();
   memcpy(new_minilines, minilines, sizeof(MiniLinesInfo));
   new_minilines->first = new_minilines->last = NULL;
   new_minilines->owner_block = pOwnerBlock;
   for (pMiniLine=minilines->first; pMiniLine != NULL;
         pMiniLine=pMiniLine->next) {
      DupMiniLine(pMiniLine, new_minilines, pOwnerBlock, &pFirstMiniLine,
            &pLastMiniLine);
   }
   new_minilines->first = pFirstMiniLine;
   new_minilines->last = pLastMiniLine;

   if (ppMinilines != NULL) {
      *ppMinilines = new_minilines;
   }
}

void DupTextObj(TextPtr, FromObjPtr, ToObjPtr)
   struct TextRec *TextPtr;
   struct ObjRec *FromObjPtr, *ToObjPtr;
{
   struct TextRec *text_ptr=NULL;
   MiniLinesInfo *minilines=NULL;
   MiniLineInfo *pMiniLine=NULL;

   text_ptr = (struct TextRec *)malloc(sizeof(struct TextRec));
   if (text_ptr == NULL) FailAllocMessage();
   memcpy(text_ptr, TextPtr, sizeof(struct TextRec));

   text_ptr->attr = NULL;
   text_ptr->cached_bitmap = None;
   text_ptr->cached_pixmap = None;
   text_ptr->cached_bg_bitmap = None;

   ToObjPtr->detail.t = text_ptr;

   if (TextPtr->cached_bitmap != None) {
      int w=ZOOMED_SIZE(FromObjPtr->bbox.rbx - FromObjPtr->bbox.ltx - 2);
      int h=ZOOMED_SIZE(FromObjPtr->bbox.rby - FromObjPtr->bbox.lty - 2);

      if (w == 0) w = 1;
      if (h == 0) h = 1;
      text_ptr->cached_bitmap = XCreatePixmap(mainDisplay,mainWindow,w,h,1);
      if (text_ptr->cached_bitmap == None) {
         FailAllocBitmapMessage(w, h);
         return;
      }
      XCopyArea(mainDisplay, TextPtr->cached_bitmap, text_ptr->cached_bitmap,
            rotateGC, 0, 0, w, h, 0, 0);
   }
   if (TextPtr->cached_bg_bitmap != None) {
      int w=ZOOMED_SIZE(FromObjPtr->bbox.rbx - FromObjPtr->bbox.ltx - 2);
      int h=ZOOMED_SIZE(FromObjPtr->bbox.rby - FromObjPtr->bbox.lty - 2);

      if (w == 0) w = 1;
      if (h == 0) h = 1;
      text_ptr->cached_bg_bitmap = XCreatePixmap(mainDisplay,mainWindow,w,h,1);
      if (text_ptr->cached_bg_bitmap == None) {
         FailAllocBitmapMessage(w, h);
         return;
      }
      XCopyArea(mainDisplay, TextPtr->cached_bg_bitmap,
            text_ptr->cached_bg_bitmap, rotateGC, 0, 0, w, h, 0, 0);
   }
   if (TextPtr->cached_pixmap != None) {
      int w=ZOOMED_SIZE(FromObjPtr->bbox.rbx - FromObjPtr->bbox.ltx - 2);
      int h=ZOOMED_SIZE(FromObjPtr->bbox.rby - FromObjPtr->bbox.lty - 2);

      if (w == 0) w = 1;
      if (h == 0) h = 1;
      text_ptr->cached_pixmap = XCreatePixmap(mainDisplay, mainWindow, w, h,
            mainDepth);
      if (text_ptr->cached_pixmap == None) {
         FailAllocBitmapMessage(w, h);
         return;
      }
      XCopyArea(mainDisplay, TextPtr->cached_pixmap, text_ptr->cached_pixmap,
            xpmGC, 0, 0, w, h, 0, 0);
   }
   DupMiniLines(&FromObjPtr->detail.t->minilines, NULL, &minilines);
   memcpy(&text_ptr->minilines, minilines, sizeof(MiniLinesInfo));
   free(minilines);
   minilines = (&text_ptr->minilines);
   for (pMiniLine=minilines->first; pMiniLine != NULL;
         pMiniLine=pMiniLine->next) {
      pMiniLine->owner_minilines = minilines;
   }
}

void DupConnection(FromConnPtr, ToConnPtr)
   struct ConnRec *FromConnPtr, *ToConnPtr;
{
}

void DupGroupObj(GroupPtr, ObjPtr)
   struct GroupRec *GroupPtr;
   struct ObjRec *ObjPtr;
{
   register struct GroupRec *group_ptr;
   struct ObjRec *top_obj;
   struct ObjRec *from_obj_ptr, *to_obj_ptr;

   group_ptr = (struct GroupRec *)malloc(sizeof(struct GroupRec));
   if (group_ptr == NULL) FailAllocMessage();
   memset(group_ptr, 0, sizeof(struct GroupRec));
   top_obj = NULL;
   from_obj_ptr = GroupPtr->last;
   for ( ; from_obj_ptr != NULL; from_obj_ptr=from_obj_ptr->prev) {
      to_obj_ptr = DupObj(from_obj_ptr);
      to_obj_ptr->next = top_obj;
      if (top_obj == NULL) {
         group_ptr->last = to_obj_ptr;
      } else {
         top_obj->prev = to_obj_ptr;
      }
      top_obj = to_obj_ptr;
   }
   top_obj->prev = NULL;
   group_ptr->first = top_obj;

   ObjPtr->detail.r = group_ptr;
}

struct ObjRec *DupObj(ObjPtr)
   struct ObjRec *ObjPtr;
{
   struct ObjRec *obj_ptr;

   obj_ptr = (struct ObjRec *)malloc(sizeof(struct ObjRec));
   if (obj_ptr == NULL) FailAllocMessage();
   memset(obj_ptr, 0, sizeof(struct ObjRec));
   DupObjBasics(ObjPtr, obj_ptr);

   switch (ObjPtr->type) {
   case OBJ_POLY:
      DupPolyObj(ObjPtr->detail.p, obj_ptr);
      DupAttrs(ObjPtr, obj_ptr);
      break;
   case OBJ_BOX:
      DupBoxObj(ObjPtr->detail.b, obj_ptr);
      DupAttrs(ObjPtr, obj_ptr);
      break;
   case OBJ_OVAL:
      DupOvalObj(ObjPtr->detail.o, obj_ptr);
      DupAttrs(ObjPtr, obj_ptr);
      break;
   case OBJ_TEXT: DupTextObj(ObjPtr->detail.t, ObjPtr, obj_ptr); break;
   case OBJ_POLYGON:
      DupPolygonObj(ObjPtr->detail.g, obj_ptr);
      DupAttrs(ObjPtr, obj_ptr);
      break;
   case OBJ_ARC:
      DupArcObj(ObjPtr->detail.a, obj_ptr);
      DupAttrs(ObjPtr, obj_ptr);
      break;
   case OBJ_RCBOX:
      DupRCBoxObj(ObjPtr->detail.rcb, obj_ptr);
      DupAttrs(ObjPtr, obj_ptr);
      break;
   case OBJ_XBM:
      DupXBmObj(ObjPtr->detail.xbm, obj_ptr);
      DupAttrs(ObjPtr, obj_ptr);
      break;
   case OBJ_XPM:
      DupXPmObj(ObjPtr->detail.xpm, obj_ptr);
      DupAttrs(ObjPtr, obj_ptr);
      break;
   case OBJ_SYM:
   case OBJ_GROUP:
   case OBJ_ICON:
   case OBJ_PIN:
      DupGroupObj(ObjPtr->detail.r, obj_ptr);
      DupAttrs(ObjPtr, obj_ptr);
      if (obj_ptr->type == OBJ_ICON || obj_ptr->type == OBJ_PIN) {
         strcpy(obj_ptr->detail.r->s, ObjPtr->detail.r->s);
         obj_ptr->detail.r->rotate = ObjPtr->detail.r->rotate;
         obj_ptr->detail.r->flip = ObjPtr->detail.r->flip;
         obj_ptr->detail.r->deck_index = (-1);
/*
 * Need to check for pins.
 * Need to handle the case where connection objects need to be dupped!
 */
         obj_ptr->detail.r->pin_connected = 0;
         obj_ptr->detail.r->first_conn = obj_ptr->detail.r->last_conn = NULL;
      }
      break;
   }
   return obj_ptr;
}

void DupSelObj()
{
   struct SelRec *sel_ptr, *sel_ptr1;
   struct ObjRec *obj_ptr, *top_obj, *bot_obj;
   int dx=0, dy=0;

   if (topSel==NULL || curChoice==VERTEXMODE) return;
/*
 * Need to check for pins.
 * Need to handle the case where connection objects need to be dupped!
 */
   top_obj = bot_obj = NULL;
   for (sel_ptr=botSel; sel_ptr != NULL; sel_ptr=sel_ptr->prev) {
      obj_ptr = DupObj(sel_ptr->obj);
      obj_ptr->next = top_obj;
      if (top_obj == NULL) {
         bot_obj = obj_ptr;
      } else {
         top_obj->prev = obj_ptr;
      }
      top_obj = obj_ptr;
   }
   top_obj->prev = NULL;

   HighLightReverse();

   sel_ptr = botSel;
   sel_ptr1 = sel_ptr->prev;
   for (obj_ptr=bot_obj; sel_ptr1 != NULL; obj_ptr=obj_ptr->prev) {
      sel_ptr->obj = obj_ptr;
      sel_ptr = sel_ptr1;
      sel_ptr1 = sel_ptr1->prev;
   }
   sel_ptr->obj = obj_ptr;

   bot_obj->next = topObj;
   topObj->prev = bot_obj;
   curPage->top = topObj = top_obj;

   if (justDupped && useRecentDupDistance) {
      dx = dupDx;
      dy = dupDy;
   } else {
      switch (gridSystem) {
      case ENGLISH_GRID:
         if (snapOn) {
            dupDx = dupDy = dx = dy = GRID_ABS_SIZE(xyEnglishGrid);
         } else {
            dupDx = dupDy = dx = dy = GRID_ABS_SIZE(DEFAULT_ENGLISH_GRID);
         }
         break;
      case METRIC_GRID:
         if (snapOn) {
            dupDx = dupDy = dx = dy = GRID_ABS_SIZE(xyMetricGrid);
         } else {
            dupDx = dupDy = dx = dy = GRID_ABS_SIZE(DEFAULT_METRIC_GRID);
         }
         break;
      }
      justDupped = TRUE;
   }

   MoveAllSelObjects(dx, dy);
   PrepareToRecord(CMD_NEW, NULL, NULL, 0);
   RecordCmd(CMD_NEW, NULL, topSel, botSel, numObjSelected);
   UpdSelBBox();
   RedrawAnArea(botObj, selLtX-(GRID_ABS_SIZE(1)), selLtY-(GRID_ABS_SIZE(1)),
         selRbX+(GRID_ABS_SIZE(1)), selRbY+(GRID_ABS_SIZE(1)));
   HighLightForward();
   SetFileModified(TRUE);
}

void DupTheseObjects(TopSel, BotSel, NewTopSel, NewBotSel)
   struct SelRec *TopSel, *BotSel, **NewTopSel, **NewBotSel;
{
   struct SelRec *sel_ptr, *new_sel_ptr;
   struct ObjRec *obj_ptr, *top_obj;

   *NewTopSel = *NewBotSel = NULL;
   if (TopSel == NULL) return;

   top_obj = NULL;
   for (sel_ptr=BotSel; sel_ptr != NULL; sel_ptr=sel_ptr->prev) {
      obj_ptr = DupObj(sel_ptr->obj);
      AdjObjSplineVs(obj_ptr);
      obj_ptr->next = top_obj;
      new_sel_ptr = (struct SelRec *)malloc(sizeof(struct SelRec));
      if (new_sel_ptr == NULL) FailAllocMessage();
      new_sel_ptr->next = *NewTopSel;
      new_sel_ptr->obj = obj_ptr;
      if (top_obj == NULL) {
         *NewBotSel = new_sel_ptr;
      } else {
         top_obj->prev = obj_ptr;
         (*NewTopSel)->prev = new_sel_ptr;
      }
      top_obj = obj_ptr;
      *NewTopSel = new_sel_ptr;
   }
   top_obj->prev = NULL;
   (*NewTopSel)->prev = NULL;
}

void JustDupSelObj(NewTopSel, NewBotSel)
   struct SelRec **NewTopSel, **NewBotSel;
{
   DupTheseObjects(topSel, botSel, NewTopSel, NewBotSel);
}
