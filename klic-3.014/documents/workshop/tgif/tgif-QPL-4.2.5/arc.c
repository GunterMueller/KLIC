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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/arc.c,v 1.9 2011/05/16 16:21:56 william Exp $
 */

#define _INCLUDE_FROM_ARC_C_

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
#include "mainloop.e"
#include "mark.e"
#include "msg.e"
#include "obj.e"
#include "page.e"
#include "pattern.e"
#include "poly.e"
#include "ps.e"
#include "raster.e"
#include "rect.e"
#include "ruler.e"
#include "select.e"
#include "setup.e"
#include "special.e"
#include "spline.e"
#include "strtbl.e"
#include "util.e"
#include "xpixmap.e"

#define EXPAND_BBOX(bbox,x,y) \
   if ((x)<(bbox)->ltx) (bbox)->ltx=(x); if ((y)<(bbox)->lty) (bbox)->lty=(y); \
   if ((x)>(bbox)->rbx) (bbox)->rbx=(x); if ((y)>(bbox)->rby) (bbox)->rby=(y)

int	arcDrawn=FALSE;

/*
 * 0 degree is horizontal in the direction of the X axis.
 * Positive angles measures counter-clockwise from 0 degree.
 * Negative angles measures clockwise from 0 degree.
 * Angle1 means the start angle.
 * Angle2 means the amount between angle1 and the real angle2.
 */

static
int ArcDirection(xc, yc, x1, y1, x2, y2)
   int xc, yc, x1, y1, x2, y2;
{
   register int dx, dy;
   register double theta1, theta2;

   dx = x1-xc; dy = y1-yc;
   theta1 = (dx==0) ? ((dy>=0) ? M_PI/2.0 : -M_PI/2.0) :
         atan2 ((double)(dy), (double)(dx));
   theta2 = (x2==xc) ? ((y2>=yc) ? M_PI/2.0 : -M_PI/2.0) :
         atan2 ((double)(y2-yc), (double)(x2-xc));
   if (theta1 < 0) theta1 += 2*M_PI;
   if (theta2 < 0) theta2 += 2*M_PI;

   if (theta2 > theta1) {
      if (theta2-theta1 >= 2*M_PI-theta2+theta1) {
         return ARC_CCW;
      } else {
         return ARC_CW;
      }
   } else if (theta1 > theta2) {
      if (theta1-theta2 >= 2*M_PI-theta1+theta2) {
         return ARC_CW;
      } else {
         return ARC_CCW;
      }
   } else {
      return ARC_CCW;
   }
}

void PointsToArc(xc, yc, x1, y1, x2, y2, dir, int_degree, ltx, lty, w, h,
      angle1, angle2)
   int xc, yc, x1, y1, x2, y2, dir, int_degree;
   int *ltx, *lty, *w, *h, *angle1, *angle2;
   /* Only good if the points are part of a circle, not an oval */
{
   register int dx, dy, radius;
   double tmp_theta;

   dx = x1-xc; dy = y1-yc;
   radius = (int)sqrt((double)(((double)dx)*((double)dx) +
         ((double)dy)*((double)dy)));
   if (ltx != NULL) *ltx = xc-radius;
   if (lty != NULL) *lty = yc-radius;
   if (w != NULL) *w = (radius<<1);
   if (h != NULL) *h = (radius<<1);
   if (int_degree) {
      int theta1, theta2, d_theta;

      tmp_theta = (dx==0) ? ((dy>=0) ? M_PI/2.0 : -M_PI/2.0) :
            atan2 ((double)(dy),(double)(dx));
      theta1 = (int)(tmp_theta/M_PI*(-180.0));
      tmp_theta = (x2==xc) ? ((y2>=yc) ? M_PI/2.0 : -M_PI/2.0) :
            atan2 ((double)(y2-yc),(double)(x2-xc));
      theta2 = (int)(tmp_theta/M_PI*(-180.0));
      /* NOTE:  *angle1 must be between -180 degrees and +180 degrees */
      if (angle1 != NULL) *angle1 = theta1*64;
      d_theta = theta2-theta1;
      switch (dir) {
      case ARC_CCW: if (d_theta < 0) d_theta = 360 + d_theta; break;
      case ARC_CW:  if (d_theta > 0) d_theta = d_theta - 360; break;
      }
      if (d_theta == 0) d_theta = 360;
      if (angle2 != NULL) *angle2 = (d_theta<<6);
   } else {
      double theta1, theta2, d_theta;

      tmp_theta = (dx==0) ? ((dy>=0) ? M_PI/2.0 : -M_PI/2.0) :
            atan2 ((double)(dy),(double)(dx));
      theta1 = tmp_theta/M_PI*(-180.0);
      tmp_theta = (x2==xc) ? ((y2>=yc) ? M_PI/2.0 : -M_PI/2.0) :
            atan2 ((double)(y2-yc),(double)(x2-xc));
      theta2 = tmp_theta/M_PI*(-180.0);
      /* NOTE:  *angle1 must be between -180 degrees and +180 degrees */
      if (angle1 != NULL) *angle1 = (int)(theta1*64.0);
      d_theta = theta2-theta1;
      switch (dir) {
      case ARC_CCW: if (d_theta < 0) d_theta = 360.0 + d_theta; break;
      case ARC_CW:  if (d_theta > 0) d_theta = d_theta - 360.0; break;
      }
      if (fabs(d_theta) < INT_TOL) d_theta = (double)360.0;
      if (angle2 != NULL) *angle2 = (int)(d_theta*64.0);
   }
}

void ArcRealX2Y2(ArcPtr, RealX2, RealY2)
   register struct ArcRec *ArcPtr;
   int *RealX2, *RealY2;
{
   register double angle_in_radian;
   int w=ArcPtr->w, h=ArcPtr->h;

   angle_in_radian = (ArcPtr->angle1+ArcPtr->angle2)*M_PI/180/64;
   *RealX2 = ArcPtr->xc + round((w/2)*cos(angle_in_radian));
   *RealY2 = ArcPtr->yc - round((h/2)*sin(angle_in_radian));
}

static
double GetRadianInArc(arc_ptr, x, y)
   struct ArcRec *arc_ptr;
   int x, y; /* x and y are abs */
{
   return ((double)atan2((double)((arc_ptr->yc-y)*arc_ptr->w),
         (double)((x-arc_ptr->xc)*arc_ptr->h)));
}

#define RETREAT (0.8)

void GetArcArrowInfo(obj_ptr, ptip_vs1, ptail_vs1, vs1, pa_angle1,
      ptip_vs2, ptail_vs2, vs2, pa_angle2)
   struct ObjRec *obj_ptr;
   IntPoint *ptip_vs1, *ptail_vs1, *vs1, *ptip_vs2, *ptail_vs2, *vs2;
   int *pa_angle1, *pa_angle2;
   /*
    * if want to get a_angle1 and a_angle2, this function must be
    * called in the following fashion (cannot omit the second call):
    *
    *     a_angle1 = arc_ptr->angle1;
    *     a_angle2 = arc_ptr->angle2;
    *     GetArcArrowInfo(*, *, *, *, &a_angle1, *, *, *, &a_angle2);
    *     arc_ptr->a_angle1 = a_angle1;
    *     arc_ptr->a_angle2 = a_angle2;
    */
{
   struct ArcRec *arc_ptr=obj_ptr->detail.a;
   int style=arc_ptr->style, a_angle1, a_angle2, angle90, angle360;
   int x1, y1, x2, y2, int_dx, int_dy, x, y, angle1, angle2, dir, w, h, aw, ah;
   double dx, dy, angle_in_radian, theta1, theta2;
   IntPoint tip_vs1, tail_vs1, tip_vs2, tail_vs2;

   if (style == LS_PLAIN) return;

   angle90 = (90<<6);
   angle360 = (360<<6);
   dir = arc_ptr->dir;
   x1 = arc_ptr->x1; y1 = arc_ptr->y1;
   angle1 = a_angle1 = arc_ptr->angle1; angle2 = a_angle2 = arc_ptr->angle2;
   w = arc_ptr->w; h = arc_ptr->h;
   aw = arc_ptr->aw; ah = arc_ptr->ah;

   /* the arrow should appear at angle1 */
   theta1 = 0.0;
   tip_vs1.x = x1;
   tip_vs1.y = y1;
   if ((style & LS_LEFT) && obj_ptr->ctm != NULL) {
      TransformPointThroughCTM(x1-obj_ptr->x, y1-obj_ptr->y,
            obj_ptr->ctm, &x, &y);
      tip_vs1.x = x+obj_ptr->x;
      tip_vs1.y = y+obj_ptr->y;
   }
   switch (dir) {
   case ARC_CCW: theta1=(double)(angle1-(angle90)); break;
   case ARC_CW: theta1=(double)(angle1+(angle90)); break;
   }
   dx = -((double)w)*cos(theta1*M_PI/180.0/64.0);
   dy = ((double)h)*sin(theta1*M_PI/180.0/64.0);
   int_dx = round(dx);
   int_dy = round(dy);
   tail_vs1.x = x1+int_dx;
   tail_vs1.y = y1+int_dy;
   if ((style & LS_LEFT) && obj_ptr->ctm != NULL) {
      TransformPointThroughCTM(x1+int_dx-obj_ptr->x, y1+int_dy-obj_ptr->y,
            obj_ptr->ctm, &x, &y);
      tail_vs1.x = x+obj_ptr->x;
      tail_vs1.y = y+obj_ptr->y;
   }
   if (int_dx==0 && int_dy==0) {
      if ((style & LS_LEFT) && vs1 != NULL) {
         if (obj_ptr->ctm == NULL) {
            vs1[0].x = vs1[1].x = vs1[2].x = vs1[3].x = x1;
            vs1[0].y = vs1[1].y = vs1[2].y = vs1[3].y = y1;
         } else {
            TransformPointThroughCTM(x1-obj_ptr->x, y1-obj_ptr->y,
                  obj_ptr->ctm, &x, &y);
            vs1[0].x = vs1[1].x = vs1[2].x = vs1[3].x = x+obj_ptr->x;
            vs1[0].y = vs1[1].y = vs1[2].y = vs1[3].y = y+obj_ptr->y;
         }
      }
   } else {
      double sin_val, cos_val, len;

      dx = (double)(tail_vs1.x - tip_vs1.x);
      dy = (double)(tail_vs1.y - tip_vs1.y);
      len = (double)sqrt(dx*dx+dy*dy);
      sin_val = dy/len;
      cos_val = dx/len;
      if ((style & LS_LEFT) && vs1 != NULL) {
         vs1[0].x = tip_vs1.x;
         vs1[0].y = tip_vs1.y;
         vs1[1].x = round(tip_vs1.x + aw*cos_val - ah*sin_val);
         vs1[1].y = round(tip_vs1.y + aw*sin_val + ah*cos_val);
         vs1[2].x = round(tip_vs1.x + aw*cos_val + ah*sin_val);
         vs1[2].y = round(tip_vs1.y + aw*sin_val - ah*cos_val);
         vs1[3].x = vs1[0].x;
         vs1[3].y = vs1[0].y;
      }
      if ((style & LS_LEFT) && (pa_angle1 != NULL || pa_angle2 != NULL)) {
         x = round(tip_vs1.x + aw*cos_val*RETREAT);
         y = round(tip_vs1.y + aw*sin_val*RETREAT);
         if (obj_ptr->ctm == NULL) {
            angle_in_radian = GetRadianInArc(arc_ptr, x, y);
         } else {
            int tmp_x, tmp_y;

            ReverseTransformPointThroughCTM(x-obj_ptr->x, y-obj_ptr->y,
                  obj_ptr->ctm, &tmp_x, &tmp_y);
            x = tmp_x+obj_ptr->x;
            y = tmp_y+obj_ptr->y;
            angle_in_radian = GetRadianInArc(arc_ptr, x, y);
         }
         angle_in_radian = angle_in_radian*180.0/M_PI*64.0;
         a_angle1 = round(angle_in_radian);
      }
   }
   theta2 = 0.0;
   ArcRealX2Y2(arc_ptr, &x2, &y2);
   tip_vs2.x = x2;
   tip_vs2.y = y2;
   if ((style & LS_RIGHT) && obj_ptr->ctm != NULL) {
      TransformPointThroughCTM(x2-obj_ptr->x, y2-obj_ptr->y,
            obj_ptr->ctm, &x, &y);
      tip_vs2.x = x+obj_ptr->x;
      tip_vs2.y = y+obj_ptr->y;
   }
   switch (dir) {
   case ARC_CCW: theta2=(double)((angle1+angle2)+(angle90)); break;
   case ARC_CW: theta2=(double)((angle1+angle2)-(angle90)); break;
   }
   dx = -((double)w)*cos(theta2*M_PI/180.0/64.0);
   dy = ((double)h)*sin(theta2*M_PI/180.0/64.0);
   int_dx = round(dx);
   int_dy = round(dy);
   tail_vs2.x = x2+int_dx;
   tail_vs2.y = y2+int_dy;
   if ((style & LS_RIGHT) && obj_ptr->ctm != NULL) {
      TransformPointThroughCTM(x2+int_dx-obj_ptr->x, y2+int_dy-obj_ptr->y,
            obj_ptr->ctm, &x, &y);
      tail_vs2.x = x+obj_ptr->x;
      tail_vs2.y = y+obj_ptr->y;
   }
   if (int_dx==0 && int_dy==0) {
      if ((style & LS_RIGHT) && vs2 != NULL) {
         if (obj_ptr->ctm == NULL) {
            vs2[0].x = vs2[1].x = vs2[2].x = vs2[3].x = x2;
            vs2[0].y = vs2[1].y = vs2[2].y = vs2[3].y = y2;
         } else {
            TransformPointThroughCTM(x2-obj_ptr->x, y2-obj_ptr->y,
                  obj_ptr->ctm, &x, &y);
            vs2[0].x = vs2[1].x = vs2[2].x = vs2[3].x = x+obj_ptr->x;
            vs2[0].y = vs2[1].y = vs2[2].y = vs2[3].y = y+obj_ptr->y;
         }
      }
   } else {
      double sin_val, cos_val, len;

      dx = tail_vs2.x - tip_vs2.x;
      dy = tail_vs2.y - tip_vs2.y;
      len = (double)sqrt(dx*dx+dy*dy);
      sin_val = dy/len;
      cos_val = dx/len;
      if ((style & LS_RIGHT) && vs2 != NULL) {
         vs2[0].x = tip_vs2.x;
         vs2[0].y = tip_vs2.y;
         vs2[1].x = round(tip_vs2.x + aw*cos_val - ah*sin_val);
         vs2[1].y = round(tip_vs2.y + aw*sin_val + ah*cos_val);
         vs2[2].x = round(tip_vs2.x + aw*cos_val + ah*sin_val);
         vs2[2].y = round(tip_vs2.y + aw*sin_val - ah*cos_val);
         vs2[3].x = vs2[0].x;
         vs2[3].y = vs2[0].y;
      }
      if (pa_angle1 != NULL || pa_angle2 != NULL) {
         int delta64;

         if (style & LS_RIGHT) {
            x = round(tip_vs2.x + aw*cos_val*RETREAT);
            y = round(tip_vs2.y + aw*sin_val*RETREAT);
            if (obj_ptr->ctm == NULL) {
               angle_in_radian = GetRadianInArc(arc_ptr, x, y);
            } else {
               int tmp_x, tmp_y;

               ReverseTransformPointThroughCTM(x-obj_ptr->x, y-obj_ptr->y,
                     obj_ptr->ctm, &tmp_x, &tmp_y);
               x = tmp_x+obj_ptr->x;
               y = tmp_y+obj_ptr->y;
               angle_in_radian = GetRadianInArc(arc_ptr, x, y);
            }
            angle_in_radian = angle_in_radian*180.0/M_PI*64.0;
         } else {
            angle_in_radian = angle1+angle2;
            while (angle_in_radian > angle360) angle_in_radian -= angle360;
            while (angle_in_radian < (-angle360)) angle_in_radian += angle360;
         }
         delta64 = round(angle_in_radian)-(a_angle1);
         switch(dir) {
         case ARC_CCW: if (delta64 < 0) delta64 += (angle360); break;
         case ARC_CW: if (delta64 > 0) delta64 -= (angle360); break;
         }
         while (delta64 > angle360) delta64 -= angle360;
         while (delta64 < (-angle360)) delta64 += angle360;
         if (delta64 == 0 && angle2 != 0) delta64 = (angle360);
         a_angle2 = delta64;
      }
   }
   if (pa_angle1 != NULL) *pa_angle1 = a_angle1;
   if (pa_angle2 != NULL) *pa_angle2 = a_angle2;
   if (ptip_vs1 != NULL) memcpy(ptip_vs1, &tip_vs1, sizeof(IntPoint));
   if (ptail_vs1 != NULL) memcpy(ptail_vs1, &tail_vs1, sizeof(IntPoint));
   if (ptip_vs2 != NULL) memcpy(ptip_vs2, &tip_vs2, sizeof(IntPoint));
   if (ptail_vs2 != NULL) memcpy(ptail_vs2, &tail_vs2, sizeof(IntPoint));
}

void CalcArcOBBox(ObjPtr)
   struct ObjRec *ObjPtr;
{
   struct ArcRec *arc_ptr=ObjPtr->detail.a;

   if (ObjPtr->ctm == NULL) {
      int theta1, theta2, real_x2, real_y2;
      int dir=arc_ptr->dir, ltx=arc_ptr->ltx, lty=arc_ptr->lty;
      int w=arc_ptr->w, h=arc_ptr->h, pass_theta1=FALSE, coverage=0, angle;
      struct BBRec obbox;

      memcpy(&obbox, &ObjPtr->obbox, sizeof(struct BBRec));

      ObjPtr->x = arc_ptr->xc;
      ObjPtr->y = arc_ptr->yc;

      theta1 = (arc_ptr->angle1)/64;
      theta2 = theta1 + (arc_ptr->angle2)/64;

      ArcRealX2Y2(arc_ptr, &real_x2, &real_y2);

      if (arc_ptr->fill == NONEPAT) {
         /* don't counter the center of the arc */
         obbox.ltx = min(arc_ptr->x1,real_x2);
         obbox.lty = min(arc_ptr->y1,real_y2);
         obbox.rbx = max(arc_ptr->x1,real_x2);
         obbox.rby = max(arc_ptr->y1,real_y2);
      } else {
         obbox.ltx = min(arc_ptr->xc,min(arc_ptr->x1,real_x2));
         obbox.lty = min(arc_ptr->yc,min(arc_ptr->y1,real_y2));
         obbox.rbx = max(arc_ptr->xc,max(arc_ptr->x1,real_x2));
         obbox.rby = max(arc_ptr->yc,max(arc_ptr->y1,real_y2));
      }

      if (theta2 < -180) theta2 += 360;
      if (theta2 > 180) theta2 -= 360;

      if (theta1 < 0) theta1 += 360;
      if (theta2 < 0) theta2 += 360;

      if (theta1 == theta2) {
         coverage = 0xf;
      } else if (dir == ARC_CCW) {
         angle = 0;
         while (angle < theta2 || !pass_theta1) {
            if (angle >= theta1 && !pass_theta1) {
               pass_theta1 = TRUE;
               if (theta2 > theta1 && angle >= theta2) break;
               if (theta2 < theta1) angle -= 360;
            }
            if (pass_theta1) coverage |= 1 << (((angle+360)/90) % 4);
            angle = (angle == 360) ? 0 : (angle+90);
         }
      } else {
         angle = 360;
         while (angle > theta2 || !pass_theta1) {
            if (angle <= theta1 && !pass_theta1) {
               pass_theta1 = TRUE;
               if (theta2 < theta1 && angle <= theta2) break;
               if (theta2 > theta1) angle += 360;
            }
            if (pass_theta1) coverage |= 1 << ((angle/90) % 4);
            angle = (angle == 0) ? 360 : (angle-90);
         }
      }
      if (coverage & 0x1) { EXPAND_BBOX(&obbox,(int)(ltx+w),(int)(lty+h/2)); }
      if (coverage & 0x2) { EXPAND_BBOX(&obbox,(int)(ltx+w/2),lty); }
      if (coverage & 0x4) { EXPAND_BBOX(&obbox,ltx,(int)(lty+h/2)); }
      if (coverage & 0x8) { EXPAND_BBOX(&obbox,(int)(ltx+w/2),(int)(lty+h)); }
      memcpy(&ObjPtr->obbox, &obbox, sizeof(struct BBRec));
   } else {
      IntPoint abs_obj_obbox_vs[5];

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
}

void CalcArcBBox(obj_ptr, obbox, bbox)
   struct ObjRec *obj_ptr;
   struct BBRec obbox, *bbox;
   /* given good obbox, figure out bbox */
{
   struct ArcRec *arc_ptr=obj_ptr->detail.a;
   int i, half_w=(arc_ptr->width>>1);
   int ltx=obbox.ltx-half_w, lty=obbox.lty-half_w;
   int rbx=obbox.rbx+half_w, rby=obbox.rby+half_w;
   IntPoint vs1[4], vs2[4];

   GetArcArrowInfo(obj_ptr, NULL, NULL, vs1, NULL, NULL, NULL, vs2, NULL);
   if (arc_ptr->style & LS_LEFT) {
      for (i=0; i < 4; i++) {
         if (vs1[i].x < ltx) ltx = vs1[i].x;
         if (vs1[i].y < lty) lty = vs1[i].y;
         if (vs1[i].x > rbx) rbx = vs1[i].x;
         if (vs1[i].y > rby) rby = vs1[i].y;
      }
   }
   if (arc_ptr->style & LS_RIGHT) {
      for (i=0; i < 4; i++) {
         if (vs2[i].x < ltx) ltx = vs2[i].x;
         if (vs2[i].y < lty) lty = vs2[i].y;
         if (vs2[i].x > rbx) rbx = vs2[i].x;
         if (vs2[i].y > rby) rby = vs2[i].y;
      }
   }
   bbox->ltx = min(ltx, obbox.ltx-(arc_ptr->width>>1));
   bbox->lty = min(lty, obbox.lty-(arc_ptr->width>>1));
   bbox->rbx = max(rbx, obbox.rbx+(arc_ptr->width>>1));
   bbox->rby = max(rby, obbox.rby+(arc_ptr->width>>1));
}

static
void DumpArcPSPath(FP, xc, yc, xr, yr, dir, a1, a2, outline, blank1, blank2)
   FILE *FP;
   int xc, yc, xr, yr, dir, a1, a2, outline;
   char *blank1, *blank2;
{
#ifdef INVERT_CTM_BUG
   if (preDumpSetup) PSUseMinRadius();
#endif /* INVERT_CTM_BUG */

   if (preDumpSetup) PSUseArc();
   fprintf(FP, "%s%s\n", blank1, gPsCmd[PS_NEWPATH]);
   if (outline) fprintf(FP, "%s%1d %1d %s\n", blank2, xc, yc,
         gPsCmd[PS_MOVETO]);
#ifdef INVERT_CTM_BUG
   switch (dir) {
   case ARC_CCW:
      fprintf(FP, "%s%1d %1d %1d %s %1d %s %1d %1d TGAR\n", blank2,
            xc, yc, xr, "tgif_min_radius", yr, "tgif_min_radius", a1, a2);
      break;
   case ARC_CW:
      fprintf(FP, "%s%1d %1d %1d %s %1d %s %1d %1d TGAN\n", blank2,
            xc, yc, xr, "tgif_min_radius", yr, "tgif_min_radius", a1, a2);
      break;
   }
#else
   switch (dir) {
   case ARC_CCW:
      fprintf(FP, "%s%1d %1d %1d %1d %1d %1d TGAR\n", blank2,
            xc, yc, xr, yr, a1, a2);
      break;
   case ARC_CW:
      fprintf(FP, "%s%1d %1d %1d %1d %1d %1d TGAN\n", blank2,
            xc, yc, xr, yr, a1, a2);
      break;
   }
#endif
   if (outline) {
      fprintf(FP, "%s%1d %1d %s\n", blank2, xc, yc, gPsCmd[PS_LINETO]);
   }
}

static
void DumpArcArrows(FP, obj_ptr)
   FILE *FP;
   struct ObjRec *obj_ptr;
{
   struct ArcRec *arc_ptr=obj_ptr->detail.a;
   int style=arc_ptr->style, aw=arc_ptr->aw, ah=arc_ptr->ah, pen=arc_ptr->pen;
   int trans_pat=obj_ptr->trans_pat, color_index=obj_ptr->color;
   IntPoint tip_vs1, tail_vs1, tip_vs2, tail_vs2;
   char *aw_spec=arc_ptr->aw_spec, *ah_spec=arc_ptr->ah_spec;

   GetArcArrowInfo(obj_ptr, &tip_vs1, &tail_vs1, NULL, NULL,
         &tip_vs2, &tail_vs2, NULL, NULL);
   if (obj_ptr->ctm == NULL) {
      if (style & LS_LEFT) {
         DumpArrow(FP, &tail_vs1, &tip_vs1, aw, ah, aw_spec, ah_spec,
               pen, trans_pat, color_index);
      }
      if (style & LS_RIGHT) {
         DumpArrow(FP, &tail_vs2, &tip_vs2, aw, ah, aw_spec, ah_spec,
               pen, trans_pat, color_index);
      }
   } else {
      if (style & LS_LEFT) {
         DumpArrow(FP, &tail_vs1, &tip_vs1, aw, ah, aw_spec, ah_spec,
               pen, trans_pat, color_index);
      }
      if (style & LS_RIGHT) {
         DumpArrow(FP, &tail_vs2, &tip_vs2, aw, ah, aw_spec, ah_spec,
               pen, trans_pat, color_index);
      }
   }
}

static
void DumpArcPath(FP, ObjPtr, xc, yc, xr, yr, dir, angle1, angle2,
      width, pen, dash, trans_pat)
   FILE *FP;
   struct ObjRec *ObjPtr;
   int xc, yc, xr, yr, dir, angle1, angle2, width, pen, dash, trans_pat;
{
   register int i;
   int w_is_int=TRUE;
   char *width_spec=ObjPtr->detail.a->width_spec;
   double dw=GetWidthInDouble(width, width_spec, &w_is_int);

   fprintf(FP, "   %s\n", gPsCmd[PS_GSAVE]);
   if (!colorDump && useGray && pen > BACKPAT) {
      GrayCheck(pen);
      fprintf(FP, "      %s %s\n", GrayStr(pen), gPsCmd[PS_SETGRAY]);
   }

   DumpArcPSPath(FP,xc,yc,xr,yr,dir,angle1,angle2,FALSE,"      ","         ");

   if (ObjPtr->ctm != NULL) {
      fprintf(FP, "      %s\n", &(gPsCmd[PS_TGIFSETMATRIX])[1]);
   }
   if (w_is_int) {
      if (width != 1) {
         fprintf(FP, "      %1d %s\n", width, gPsCmd[PS_SETLINEWIDTH]);
      }
   } else {
      fprintf(FP, "      %.3f %s\n", dw, gPsCmd[PS_SETLINEWIDTH]);
   }
   if (dash != 0) {
      fprintf(FP, "      [");
      for (i = 0; i < dashListLength[dash]-1; i++) {
         fprintf(FP, "%1d ", (int)(dashList[dash][i]));
      }
      fprintf(FP, "%1d] 0 %s\n",
            (int)(dashList[dash][dashListLength[dash]-1]), gPsCmd[PS_SETDASH]);
   }
   switch (pen) {
   case SOLIDPAT: fprintf(FP, "      %s\n", gPsCmd[PS_STROKE]); break;
   case BACKPAT:
      if (!trans_pat) {
         fprintf(FP, "      1 %s %s 0 %s\n",
               gPsCmd[PS_SETGRAY], gPsCmd[PS_STROKE], gPsCmd[PS_SETGRAY]);
      }
      break;
   default:
      if (colorDump || !useGray) {
         if (preDumpSetup) PSUseColorPattern();
         fprintf(FP, "      %s\n", gPsCmd[PS_FLATTENPATH]);
         DumpPatFill(FP, pen, ObjPtr->bbox, 6, TRUE);
      } else {
         fprintf(FP, "      %s\n", gPsCmd[PS_STROKE]);
      }
   }
   fprintf(FP, "   %s\n", gPsCmd[PS_GRESTORE]);
}

void DumpArcObj(FP, ObjPtr)
   FILE *FP;
   struct ObjRec *ObjPtr;
{
   register struct ArcRec *arc_ptr=ObjPtr->detail.a;
   int fill, trans_pat, width, pen, dash, color_index;
   int xc, yc, xr, yr, dir, angle1, angle2, style, a_angle1, a_angle2;
   int x1, y1, aw, ah;

   trans_pat = ObjPtr->trans_pat;
   fill = arc_ptr->fill;
   width = arc_ptr->width;
   aw = arc_ptr->aw;
   ah = arc_ptr->ah;
   pen = arc_ptr->pen;
   dash = arc_ptr->dash;
   style = arc_ptr->style;
   xc = arc_ptr->xc; yc = arc_ptr->yc;
   x1 = arc_ptr->x1; y1 = arc_ptr->y1;
   xr = (int)(arc_ptr->w/2); yr = (int)(arc_ptr->h/2);
   dir = arc_ptr->dir;
   angle1 = -round(((double)arc_ptr->angle1)/64.0);
   angle2 = -round(((double)arc_ptr->angle2)/64.0) + angle1;
   a_angle1 = -round(((double)arc_ptr->a_angle1)/64.0);
   a_angle2 = -round(((double)arc_ptr->a_angle2)/64.0) + a_angle1;

   if ((fill == NONEPAT || (trans_pat && fill == BACKPAT)) &&
         (pen == NONEPAT || (trans_pat && pen == BACKPAT))) {
      return;
   }
   fprintf(FP, "%% ARC\n");
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
      DumpArcPSPath(FP,xc,yc,xr,yr,dir,angle1,angle2,TRUE,"","   ");
      fprintf(FP, "   %s\n", gPsCmd[PS_FILL]);
      break;
   case BACKPAT:
      if (!trans_pat) {
         DumpArcPSPath(FP,xc,yc,xr,yr,dir,angle1,angle2,TRUE,"","   ");
         fprintf(FP, "   %s 1 %s %s\n", gPsCmd[PS_CLOSEPATH],
               gPsCmd[PS_SETGRAY], gPsCmd[PS_FILL]);
         DumpRGBColorLine(FP, color_index, 3, TRUE);
      }
      break;
   default:
      fprintf(FP, "%s\n", gPsCmd[PS_GSAVE]);
      if (colorDump || !useGray) {
         if (preDumpSetup) PSUseColorPattern();
         if (!trans_pat) {
            DumpArcPSPath(FP, xc, yc, xr, yr, dir, angle1, angle2, TRUE,
                  "   ","      ");
            fprintf(FP, "   %s 1 %s %s\n", gPsCmd[PS_CLOSEPATH],
                  gPsCmd[PS_SETGRAY], gPsCmd[PS_FILL]);
            DumpRGBColorLine(FP, color_index, 3, TRUE);
         }
         DumpArcPSPath(FP,xc,yc,xr,yr,dir,angle1,angle2,TRUE,"   ","      ");
         fprintf(FP, "   %s %s %s\n", gPsCmd[PS_CLOSEPATH],
               gPsCmd[PS_CLIP], gPsCmd[PS_NEWPATH]);
         DumpPatFill(FP, fill, ObjPtr->bbox, 3, TRUE);
      } else {
         GrayCheck(fill);
         fprintf(FP, "   %s %s\n", GrayStr(fill), gPsCmd[PS_SETGRAY]);
         DumpArcPSPath(FP,xc,yc,xr,yr,dir,angle1,angle2,TRUE,"   ","      ");
         fprintf(FP, "   %s\n", gPsCmd[PS_FILL]);
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

   if ((colorDump || !useGray) && pen > BACKPAT && !trans_pat) {
      if (style == LS_PLAIN) {
         DumpArcPath(FP, ObjPtr, xc, yc, xr, yr, dir, angle1, angle2,
               width, BACKPAT, 0, trans_pat);
      } else {
         DumpArcPath(FP, ObjPtr, xc, yc, xr, yr, dir, a_angle1, a_angle2,
               width, BACKPAT, 0, trans_pat);
      }
      DumpRGBColorLine(FP, color_index, 3, TRUE);
   }
   if (style == LS_PLAIN) {
      DumpArcPath(FP, ObjPtr, xc, yc, xr, yr, dir, angle1, angle2,
            width, pen, dash, trans_pat);
   } else {
      DumpArcPath(FP,ObjPtr,xc,yc,xr,yr,dir,a_angle1,a_angle2,
            width, pen, dash, trans_pat);
   }
   fprintf(FP, "%s\n", gPsCmd[PS_GRESTORE]);

   if (style != LS_PLAIN && !(pen == BACKPAT && trans_pat)) {
      DumpArcArrows(FP,ObjPtr);
   }
   if (ObjPtr->ctm != NULL) fprintf(FP, "%s\n", gPsCmd[PS_GRESTORE]);
   fprintf(FP, "\n");
}

int NeedsToCacheArcObj(ObjPtr)
   struct ObjRec *ObjPtr;
{
   return (ObjPtr->ctm != NULL);
}

static int arcXYMagInitialized=FALSE;
static double arcXMag[6], arcYMag[6];

static
void MakeArcRotatedVs(obj_ptr, oval_vs, angle1, angle2,
      rotated_n, rotated_vlist)
   struct ObjRec *obj_ptr;
   IntPoint *oval_vs;
   int angle1, angle2, *rotated_n;
   XPoint **rotated_vlist;
{
   struct ArcRec *arc_ptr=obj_ptr->detail.a;
   XPoint *sv=NULL;
   IntPoint *pv=NULL, *cntrlv=NULL;
   int i=0, sn=0, cntrln=0, v_index=0, cur_index=0, angle=0;
   int num_vs=0, angle90=(90<<6), angle360=(360<<6), seen_angle1=FALSE;
   int cx=arc_ptr->xc, cy=arc_ptr->yc;
   double hw=(double)(arc_ptr->w>>1), hh=(double)(arc_ptr->h>>1);
   int start_angle=angle90-angle360, end_angle=angle360;
   IntPoint first_pt;

   memset(&first_pt, 0, sizeof(IntPoint));

   if (angle1 > angle2) {
      angle = angle2;
      angle2 = angle1;
      angle1 = angle;
   }
   while (start_angle > angle1) start_angle -= angle360;
   while (end_angle < angle2) end_angle += angle360;
   for (angle=start_angle; angle <= end_angle; angle+=angle90) {
      /* the quadrant being considered is angle-angle90 to angle */
      double angle_in_quadrant; /* 0 < angle_in_quadrant <= 90.0 degrees */

      if (seen_angle1) {
         if (angle >= angle2) {
            angle_in_quadrant = ((double)(angle2-(angle-angle90)))/64.0;
            if (angle_in_quadrant < 20.0) {
               num_vs++;
            } else if (angle_in_quadrant < 35.0) {
               num_vs += 2;
            } else if (angle_in_quadrant < 50.0) {
               num_vs += 3;
            } else if (angle_in_quadrant < 65.0) {
               num_vs += 4;
            } else if (angle_in_quadrant < 80.0) {
               num_vs += 5;
            } else {
               num_vs += 6;
            }
            break;
         } else {
            num_vs += 6;
         }
      } else {
         if (angle >= angle1) {
            angle_in_quadrant = ((double)(angle1-(angle-angle90)))/64.0;
            if (angle_in_quadrant < 10.0) {
               num_vs += 7;
            } else if (angle_in_quadrant < 25.0) {
               num_vs += 6;
            } else if (angle_in_quadrant < 40.0) {
               num_vs += 5;
            } else if (angle_in_quadrant < 55.0) {
               num_vs += 4;
            } else if (angle_in_quadrant < 70.0) {
               num_vs += 3;
            } else if (angle_in_quadrant < 80.0) {
               num_vs += 2;
            } else {
               num_vs++;
            }
            if (angle >= angle2) {
               angle_in_quadrant = ((double)(angle2-(angle-angle90)))/64.0;
               if (angle_in_quadrant < 20.0) {
                  num_vs -= 5;
               } else if (angle_in_quadrant < 35.0) {
                  num_vs -= 4;
               } else if (angle_in_quadrant < 50.0) {
                  num_vs -= 3;
               } else if (angle_in_quadrant < 65.0) {
                  num_vs -= 2;
               } else if (angle_in_quadrant < 80.0) {
                  num_vs--;
               }
               break;
            }
            seen_angle1 = TRUE;
         }
      }
   }
   *rotated_n = 0;
   *rotated_vlist = (XPoint*)malloc((num_vs+5)*sizeof(XPoint));
   pv = (IntPoint*)malloc((num_vs+5)*sizeof(IntPoint));
   if (*rotated_vlist == NULL || pv == NULL) FailAllocMessage();
   memset(*rotated_vlist, 0, (num_vs+5)*sizeof(XPoint));
   memset(pv, 0, (num_vs+5)*sizeof(IntPoint));

   cur_index = 0;
   seen_angle1 = FALSE;
   for (angle=start_angle, v_index=0; angle <= end_angle;
         angle+=angle90, v_index+=6) {
      /* the quadrant being considered is angle-angle90 to angle */
      double angle_in_quadrant; /* 0 < angle_in_quadrant <= 90.0 degrees */
      int count=(-1);
      double sin_val, cos_val;

      if (v_index >= 24) v_index = 0;
      if (seen_angle1) {
         if (angle >= angle2) {
            angle_in_quadrant = ((double)(angle2-(angle-angle90)))/64.0;
            if (angle_in_quadrant < 20.0) {
               count = 1;
            } else if (angle_in_quadrant < 35.0) {
               count = 2;
            } else if (angle_in_quadrant < 50.0) {
               count = 3;
            } else if (angle_in_quadrant < 65.0) {
               count = 4;
            } else if (angle_in_quadrant < 80.0) {
               count = 5;
            } else {
               count = 6;
            }
            for (i=1; i < count; i++) {
               pv[cur_index].x = oval_vs[v_index+i].x;
               pv[cur_index].y = oval_vs[v_index+i].y;
               cur_index++;
            }
            sin_val = cos((double)(angle2*M_PI/180.0/64.0));
            cos_val = sin((double)(angle2*M_PI/180.0/64.0));
            pv[cur_index].x = cx + round(hw*sin_val);
            pv[cur_index].y = cy - round(hh*cos_val);
            cur_index++;
            break;
         } else {
            for (i=1; i < 7; i++) {
               pv[cur_index].x = oval_vs[v_index+i].x;
               pv[cur_index].y = oval_vs[v_index+i].y;
               cur_index++;
            }
         }
      } else {
         if (angle >= angle1) {
            angle_in_quadrant = ((double)(angle1-(angle-angle90)))/64.0;
            if (angle_in_quadrant < 10.0) {
               count = 7;
            } else if (angle_in_quadrant < 25.0) {
               count = 6;
            } else if (angle_in_quadrant < 40.0) {
               count = 5;
            } else if (angle_in_quadrant < 55.0) {
               count = 4;
            } else if (angle_in_quadrant < 70.0) {
               count = 3;
            } else if (angle_in_quadrant < 80.0) {
               count = 2;
            } else {
               count = 1;
            }
            sin_val = cos((double)(angle1*M_PI/180.0/64.0));
            cos_val = sin((double)(angle1*M_PI/180.0/64.0));
            pv[0].x = first_pt.x = cx + round(hw*sin_val);
            pv[0].y = first_pt.y = cy - round(hh*cos_val);
            for (i=1; i < count; i++) {
               pv[i].x = oval_vs[(v_index+7-(count-i)) % 24].x;
               pv[i].y = oval_vs[(v_index+7-(count-i)) % 24].y;
            }
            cur_index += count;
            if (angle >= angle2) {
               angle_in_quadrant = ((double)(angle2-(angle-angle90)))/64.0;
               if (angle_in_quadrant < 20.0) {
                  cur_index -= 6;
               } else if (angle_in_quadrant < 35.0) {
                  cur_index -= 5;
               } else if (angle_in_quadrant < 50.0) {
                  cur_index -= 4;
               } else if (angle_in_quadrant < 65.0) {
                  cur_index -= 3;
               } else if (angle_in_quadrant < 80.0) {
                  cur_index -= 2;
               } else {
                  cur_index--;
               }
               sin_val = cos((double)(angle2*M_PI/180.0/64.0));
               cos_val = sin((double)(angle2*M_PI/180.0/64.0));
               pv[cur_index].x = cx + round(hw*sin_val);
               pv[cur_index].y = cy - round(hh*cos_val);
               cur_index++;
               break;
            }
            seen_angle1 = TRUE;
         }
      }
   }
   if (num_vs != cur_index) {
      fprintf(stderr, "num_vs (%1d) != cur_index (%1d)\n", num_vs, cur_index);
   }
   pv[num_vs].x = cx;
   pv[num_vs].y = cy;
   for (i=0; i < num_vs+1; i++) {
      int x, y;

      if (i == 0) {
         TransformPointThroughCTM(first_pt.x-obj_ptr->x, first_pt.y-obj_ptr->y,
               obj_ptr->ctm, &x, &y);
         first_pt.x = x + obj_ptr->x;
         first_pt.y = y + obj_ptr->y;
      }
      TransformPointThroughCTM(pv[i].x-obj_ptr->x, pv[i].y-obj_ptr->y,
            obj_ptr->ctm, &x, &y);
      pv[i].x = x + obj_ptr->x;
      pv[i].y = y + obj_ptr->y;
      (*rotated_vlist)[i].x = (short)OFFSET_X(pv[i].x);
      (*rotated_vlist)[i].y = (short)OFFSET_Y(pv[i].y);
   }
   if (num_vs == 1) {
      /* well, the real way to do this is to use a mid-point */
      sn = 2;
      sv = (XPoint*)malloc((num_vs+1)*sizeof(XPoint));
      if (sv == NULL) FailAllocMessage();
      memset(sv, 0, (num_vs+1)*sizeof(XPoint));
      sv[0].x = (short)(OFFSET_X(first_pt.x));
      sv[0].y = (short)(OFFSET_Y(first_pt.y));
      sv[1].x = (short)(OFFSET_X(pv[0].x));
      sv[1].y = (short)(OFFSET_Y(pv[0].y));
   } else {
      sv = MakeIntSplinePolyVertex(&sn, &cntrln, &cntrlv,
            drawOrigX, drawOrigY, num_vs, pv);
   }
   if (sv == NULL) {
      FailAllocMessage();
   } else {
      XPoint *new_sv=(XPoint*)malloc((sn+3)*sizeof(XPoint));

      if (new_sv == NULL) FailAllocMessage();
      memset(new_sv, 0, (sn+3)*sizeof(XPoint));
      for (i=0; i < sn; i++) {
         new_sv[i].x = sv[i].x;
         new_sv[i].y = sv[i].y;
      }
      new_sv[sn].x = (*rotated_vlist)[num_vs].x;
      new_sv[sn].y = (*rotated_vlist)[num_vs].y;
      new_sv[sn+1].x = new_sv[0].x;
      new_sv[sn+1].y = new_sv[0].y;
      free(sv);
      sv = new_sv;
   }
   free(*rotated_vlist);
   *rotated_n = sn;
   *rotated_vlist = sv;

   free(pv);
   if (cntrlv != NULL) free(cntrlv);
}

static
void MakeCachedArc(ObjPtr)
   struct ObjRec *ObjPtr;
{
   struct ArcRec *arc_ptr=ObjPtr->detail.a;
   IntPoint tmp_vs[25];
   struct BBRec obbox;
   int i, cx, cy, a_angle1, a_angle2;
   double hw, hh; /* half w and half h */

   if (!arcXYMagInitialized) {
      int j;

      for (i=0, j=0; i < 90; i+=15, j++) {
         arcXMag[j] = cos((double)(((double)i)*M_PI/180.0));
         arcYMag[j] = sin((double)(((double)i)*M_PI/180.0));
      }
      arcXYMagInitialized = TRUE;
   }
   if (ObjPtr->ctm == NULL) return;

   a_angle1 = arc_ptr->angle1;
   a_angle2 = arc_ptr->angle2;
   if (arc_ptr->style != LS_PLAIN) {
      GetArcArrowInfo(ObjPtr, NULL, NULL, NULL, &a_angle1,
            NULL, NULL, NULL, &a_angle2);
   }
   arc_ptr->a_angle1 = a_angle1;
   arc_ptr->a_angle2 = a_angle2;
   cx = arc_ptr->xc;
   cy = arc_ptr->yc;
   obbox.ltx = cx-(arc_ptr->w>>1); obbox.lty = cy-(arc_ptr->h>>1);
   obbox.rbx = cx+(arc_ptr->w>>1); obbox.rby = cy+(arc_ptr->h>>1);
   hw = (double)(arc_ptr->w>>1);
   hh = (double)(arc_ptr->h>>1);

   for (i=0; i < 24; i++) {
      double dx, dy;

      switch (i) {
      case 0: tmp_vs[0].x=obbox.rbx; tmp_vs[0].y=cy; break;
      case 6: tmp_vs[6].x=cx; tmp_vs[6].y=obbox.lty; break;
      case 12: tmp_vs[12].x=obbox.ltx; tmp_vs[12].y=cy; break;
      case 18: tmp_vs[18].x=cx; tmp_vs[18].y=obbox.rby; break;
      default:
         if (i < 6) {
            dx = (hw*arcXMag[i % 6]); dy = (hh*arcYMag[i % 6]);
            tmp_vs[i].x=cx+round(dx); tmp_vs[i].y=cy-round(dy);
         } else if (i < 12) {
            dx = (hw*arcXMag[(24-i) % 6]); dy = (hh*arcYMag[(24-i) % 6]);
            tmp_vs[i].x=cx-round(dx); tmp_vs[i].y=cy-round(dy);
         } else if (i < 18) {
            dx = (hw*arcXMag[i % 6]); dy = (hh*arcYMag[i % 6]);
            tmp_vs[i].x=cx-round(dx); tmp_vs[i].y=cy+round(dy);
         } else {
            dx = (hw*arcXMag[(24-i) % 6]); dy = (hh*arcYMag[(24-i) % 6]);
            tmp_vs[i].x=cx+round(dx); tmp_vs[i].y=cy+round(dy);
         }
         break;
      }
   }
   tmp_vs[24].x=tmp_vs[0].x; tmp_vs[24].y=tmp_vs[0].y;

   if (arc_ptr->rotated_vlist != NULL) free(arc_ptr->rotated_vlist);
   if (arc_ptr->rotated_asvlist != NULL) free(arc_ptr->rotated_asvlist);
   arc_ptr->rotated_vlist = arc_ptr->rotated_asvlist = NULL;
   arc_ptr->rotated_n = arc_ptr->rotated_asn = 0;

   MakeArcRotatedVs(ObjPtr, tmp_vs, arc_ptr->angle1,
         arc_ptr->angle1+arc_ptr->angle2, &arc_ptr->rotated_n,
         &arc_ptr->rotated_vlist);
   if (arc_ptr->style != LS_PLAIN) {
      MakeArcRotatedVs(ObjPtr, tmp_vs, arc_ptr->a_angle1,
            arc_ptr->a_angle1+arc_ptr->a_angle2, &arc_ptr->rotated_asn,
            &arc_ptr->rotated_asvlist);
   }
}

void DrawArcObj(window, XOff, YOff, ObjPtr)
   Window window;
   int XOff, YOff;
   struct ObjRec *ObjPtr;
{
   struct ArcRec *arc_ptr=ObjPtr->detail.a;
   int i, ltx, lty, w, h, angle1, angle2;
   int fill, trans_pat, width, pen, dash, pixel, real_x_off, real_y_off;
   XPoint tmp_v[4];
   IntPoint vs1[4], vs2[4];
   XGCValues values;

   if (NeedsToCacheArcObj(ObjPtr) && arc_ptr->rotated_vlist==NULL) {
      MakeCachedArc(ObjPtr);
   }
   if (userDisableRedraw) return;

   real_x_off = (zoomedIn ? XOff : (XOff>>zoomScale)<<zoomScale);
   real_y_off = (zoomedIn ? YOff : (YOff>>zoomScale)<<zoomScale);

   ltx = ZOOMED_SIZE(arc_ptr->ltx-real_x_off);
   lty = ZOOMED_SIZE(arc_ptr->lty-real_y_off);
   w = ZOOMED_SIZE(arc_ptr->ltx+arc_ptr->w-real_x_off)-ltx;
   h = ZOOMED_SIZE(arc_ptr->lty+arc_ptr->h-real_y_off)-lty;
   angle1 = arc_ptr->angle1;
   angle2 = arc_ptr->angle2;

   trans_pat = ObjPtr->trans_pat;
   fill = arc_ptr->fill;
   width = arc_ptr->width;
   pen = arc_ptr->pen;
   dash = arc_ptr->dash;
   pixel = colorPixels[ObjPtr->color];

   if ((fill == NONEPAT || (trans_pat && fill == BACKPAT)) &&
         (pen == NONEPAT || (trans_pat && pen == BACKPAT))) {
      return;
   }
   if (fill != NONEPAT) {
      values.foreground = GetDrawingBgPixel(fill, pixel);
      values.function = GXcopy;
      values.fill_style = (trans_pat ? FillStippled : FillOpaqueStippled);
      values.stipple = patPixmap[fill];
      XChangeGC(mainDisplay, drawGC,
            GCForeground | GCFunction | GCFillStyle | GCStipple, &values);
      if (ObjPtr->ctm != NULL) {
         XFillPolygon(mainDisplay, window, drawGC, arc_ptr->rotated_vlist,
               arc_ptr->rotated_n+2, Complex, CoordModeOrigin);
      } else {
         XFillArc(mainDisplay, window, drawGC, ltx, lty, w, h, angle1, angle2);
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

   GetArcArrowInfo(ObjPtr, NULL, NULL, vs1, NULL, NULL, NULL, vs2, NULL);
   if (arc_ptr->style & LS_LEFT) {
      for (i=0; i < 3; i++) {
         tmp_v[i].x = (short)ZOOMED_SIZE(vs1[i].x-real_x_off);
         tmp_v[i].y = (short)ZOOMED_SIZE(vs1[i].y-real_y_off);
      }
      tmp_v[3].x = tmp_v[0].x;
      tmp_v[3].y = tmp_v[0].y;
      XFillPolygon(mainDisplay, window, drawGC, tmp_v, 4, Convex,
            CoordModeOrigin);
   }
   if (arc_ptr->style & LS_RIGHT) {
      for (i=0; i < 3; i++) {
         tmp_v[i].x = (short)ZOOMED_SIZE(vs2[i].x-real_x_off);
         tmp_v[i].y = (short)ZOOMED_SIZE(vs2[i].y-real_y_off);
      }
      tmp_v[3].x = tmp_v[0].x;
      tmp_v[3].y = tmp_v[0].y;
      XFillPolygon(mainDisplay, window, drawGC, tmp_v, 4, Convex,
            CoordModeOrigin);
   }
   if (ObjPtr->ctm != NULL) {
      if (arc_ptr->style == LS_PLAIN) {
         XDrawLines(mainDisplay, window, drawGC, arc_ptr->rotated_vlist,
               arc_ptr->rotated_n, CoordModeOrigin);
      } else {
         XDrawLines(mainDisplay, window, drawGC, arc_ptr->rotated_asvlist,
               arc_ptr->rotated_asn, CoordModeOrigin);
      }
   } else {
      if (arc_ptr->style == LS_PLAIN) {
         XDrawArc(mainDisplay, window, drawGC, ltx, lty, w, h, angle1, angle2);
      } else {
         XDrawArc(mainDisplay, window, drawGC, ltx, lty, w, h,
               arc_ptr->a_angle1, arc_ptr->a_angle2);
      }
   }
}

struct ObjRec *CreateArcObj(xc, yc, x1, y1, x2, y2, dir, ltx, lty, w, h,
      angle1, angle2, CreateAbsolute)
   int xc, yc, x1, y1, x2, y2, dir, ltx, lty, w, h, angle1, angle2;
   int CreateAbsolute;
{
   struct ArcRec *arc_ptr;
   struct ObjRec *obj_ptr;

   arc_ptr = (struct ArcRec *)malloc(sizeof(struct ArcRec));
   if (arc_ptr == NULL) FailAllocMessage();
   memset(arc_ptr, 0, sizeof(struct ArcRec));
   arc_ptr->fill = objFill;
   arc_ptr->width = curWidthOfLine[lineWidth];
   arc_ptr->aw = curArrowHeadW[lineWidth];
   arc_ptr->ah = curArrowHeadH[lineWidth];
   UtilStrCpyN(arc_ptr->width_spec, sizeof(arc_ptr->width_spec),
         curWidthOfLineSpec[lineWidth]);
   UtilStrCpyN(arc_ptr->aw_spec, sizeof(arc_ptr->aw_spec),
         curArrowHeadWSpec[lineWidth]);
   UtilStrCpyN(arc_ptr->ah_spec, sizeof(arc_ptr->ah_spec),
         curArrowHeadHSpec[lineWidth]);
   arc_ptr->pen = penPat;
   arc_ptr->dash = curDash;
   arc_ptr->style = lineStyle;

   if (CreateAbsolute) {
      arc_ptr->xc = xc;
      arc_ptr->yc = yc;
      arc_ptr->x1 = x1;
      arc_ptr->y1 = y1;
      arc_ptr->x2 = x2;
      arc_ptr->y2 = y2;
      arc_ptr->ltx = xc-(w>>1);
      arc_ptr->lty = yc-(h>>1);
   } else {
      arc_ptr->xc = ABS_X(xc);
      arc_ptr->yc = ABS_Y(yc);
      arc_ptr->x1 = ABS_X(x1);
      arc_ptr->y1 = ABS_Y(y1);
      arc_ptr->x2 = ABS_X(x2);
      arc_ptr->y2 = ABS_Y(y2);
      arc_ptr->ltx = ABS_X(xc-(w>>1));
      arc_ptr->lty = ABS_Y(yc-(h>>1));
   }
   arc_ptr->dir = dir;
   arc_ptr->w = (arc_ptr->xc-arc_ptr->ltx)<<1;
   arc_ptr->h = (arc_ptr->yc-arc_ptr->lty)<<1;
   arc_ptr->angle1 = arc_ptr->a_angle1 = angle1;
   arc_ptr->angle2 = arc_ptr->a_angle2 = angle2;
   arc_ptr->rotated_n = 0;
   arc_ptr->rotated_vlist = NULL;
   arc_ptr->rotated_asn = 0;
   arc_ptr->rotated_asvlist = NULL;

   obj_ptr = (struct ObjRec *)malloc(sizeof(struct ObjRec));
   if (obj_ptr == NULL) FailAllocMessage();
   memset(obj_ptr, 0, sizeof(struct ObjRec));
   obj_ptr->detail.a = arc_ptr;

   obj_ptr->type = OBJ_ARC;
   obj_ptr->color = colorIndex;
   if (mainDisplay != NULL) {
      UtilStrCpyN(obj_ptr->color_str, sizeof(obj_ptr->color_str),
            colorMenuItems[colorIndex]);
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
   AdjObjBBox(obj_ptr);
   AddObj(NULL, topObj, obj_ptr);

   return obj_ptr;
}

static
int Colinear(Ax, Ay, Dx, Dy, Cx, Cy)
   double Ax, Ay, Dx, Dy, Cx, Cy;
   /* returns TRUE if point D is between A and C and A, D, C are colinear */
{
   double len_ad=(double)0, len_dc=(double)0, len_ac=(double)0;
   double dx=(double)0, dy=(double)0;

   dx = Ax-Cx;
   dy = Ay-Cy;
   len_ac = (double)sqrt(dx*dx+dy*dy);

   dx = Ax-Dx;
   dy = Ay-Dy;
   len_ad = (double)sqrt(dx*dx+dy*dy);

   dx = Cx-Dx;
   dy = Cy-Dy;
   len_dc = (double)sqrt(dx*dx+dy*dy);

   return (fabs(len_ad+len_dc-len_ac) < INT_TOL);
}

static
int SegmentIntersects(Ax, Ay, Bx, By, Cx, Cy, Dx, Dy)
   int Ax, Ay, Bx, By;
   double Cx, Cy, Dx, Dy;
   /* returns TRUE if line segments AB and CD intersects */
{
   int AB_horizontal=(Ay==By), CD_horizontal=(fabs(Cy-Dy)<INT_TOL);
   int AB_vertical=(Ax==Bx), CD_vertical=(fabs(Cx-Dx)<INT_TOL);
   double y_intersect_AB=(double)0, slope_AB=(double)0;
   double y_intersect_CD=(double)0, slope_CD=(double)0;
   double Ix=(double)0, Iy=(double)0;

   if (AB_horizontal) {
      y_intersect_AB = (double)Ay;
      slope_AB = 0;
   } else if (AB_vertical) {
      /* y_intersect_AB = Inf; */
      /* slope_AB = Inf; */
   } else {
      slope_AB = (double)(((double)(Ay-By))/((double)(Ax-Bx)));
      y_intersect_AB = Ay - Ax * slope_AB;
   }
   if (CD_horizontal) {
      y_intersect_CD = Cy;
      slope_CD = 0;
   } else if (CD_vertical) {
      /* y_intersect_CD = Inf; */
      /* slope_CD = Inf; */
   } else {
      slope_CD = (double)((Cy-Dy)/(Cx-Dx));
      y_intersect_CD = Cy - Cx * slope_CD;
   }
   if (AB_horizontal) {
      if (CD_horizontal) {
         return FALSE;
      } else if (CD_vertical) {
         Ix = Cx;
         Iy = (double)Ay;
      } else {
         Ix = (((double)Ay) - y_intersect_CD) / slope_CD;
         Iy = (double)Ay;
      }
   } else if (AB_vertical) {
      if (CD_horizontal) {
         Ix = (double)Ax;
         Iy = Cy;
      } else if (CD_vertical) {
         return FALSE;
      } else {
         Ix = (double)Ax;
         Iy = slope_CD * ((double)Ax) + y_intersect_CD;
      }
   } else {
      if (CD_horizontal) {
         Ix = (Cy - y_intersect_AB) / slope_AB;
         Iy = Cy;
      } else if (CD_vertical) {
         Ix = Cx;
         Iy = slope_AB * Cx + y_intersect_AB;
      } else {
         Ix = (y_intersect_CD - y_intersect_AB) / (slope_AB - slope_CD);
         Iy = slope_AB * Ix + y_intersect_AB;
      }
   }
   return Colinear(Cx, Cy, Ix, Iy, Dx, Dy);
}

static
int GetCenter(Ax, Ay, Cx, Cy, Bx, By, pcx, pcy, pdir)
   int Ax, Ay, Cx, Cy, Bx, By, *pcx, *pcy, *pdir;
{
   /*
    * point A is (OrigX,OrigY) - start of arc
    * point B is (grid_x,grid_y) - this point is moving
    * point C is (first_x,first_y) - end of arc
    * point D is the center of the circle
    * point E is the midpoint of the AB segment
    * point F is the midpoint of the CB segment
    */
   int AB_horizontal=FALSE, BC_horizontal=FALSE;
   int AB_vertical=FALSE, BC_vertical=FALSE, saved_dir=(*pdir);
   double Dx=(double)0, Dy=(double)0;
   double Ex=(double)0, Ey=(double)0;
   double Fx=(double)0, Fy=(double)0;
   double y_intersect_DE=(double)0, slope_DE=(double)0;
   double y_intersect_DF=(double)0, slope_DF=(double)0;

   if ((Ax==Bx && Ay==By) || (Cx==Bx && Cy==By)) {
      return FALSE;
   }
   AB_horizontal = (Ay==By);
   AB_vertical = (Ax==Bx);

   Ex = (double)(((double)(Ax+Bx))/((double)2));
   Ey = (double)(((double)(Ay+By))/((double)2));

   if (AB_horizontal) {
      /* y_intersect_DE = Inf; */
      /* slope_DE = Inf; */
   } else if (AB_vertical) {
      y_intersect_DE = Ey;
      slope_DE = 0;
   } else {
      slope_DE = (double)(((double)(Bx-Ax))/((double)(Ay-By)));
      y_intersect_DE = Ey - Ex * slope_DE;
   }
   BC_horizontal = (Cy==By);
   BC_vertical = (Cx==Bx);

   Fx = (double)(((double)(Cx+Bx))/((double)2));
   Fy = (double)(((double)(Cy+By))/((double)2));

   if (BC_horizontal) {
      /* y_intersect_DF = Inf; */
      /* slope_DF = Inf; */
   } else if (BC_vertical) {
      y_intersect_DF = Fy;
      slope_DF = 0;
   } else {
      slope_DF = (double)(((double)(Bx-Cx))/((double)(Cy-By)));
      y_intersect_DF = Fy - Fx * slope_DF;
   }
   if (AB_horizontal) {
      if (BC_horizontal) {
         return FALSE;
      } else if (BC_vertical) {
         Dx = Ex;
         Dy = Fy;
      } else {
         Dx = Ex;
         Dy = slope_DF * Ex + y_intersect_DF;
      }
   } else if (AB_vertical) {
      if (BC_horizontal) {
         Dx = Fx;
         Dy = Ey;
      } else if (BC_vertical) {
         return FALSE;
      } else {
         Dx = (Ey - y_intersect_DF) / slope_DF;
         Dy = Ey;
      }
   } else {
      if (BC_horizontal) {
         Dx = Fx;
         Dy = slope_DE * Fx + y_intersect_DE;
      } else if (BC_vertical) {
         Dx = (Fy - y_intersect_DE) / slope_DE;
         Dy = Fy;
      } else {
         if (fabs(slope_DE - slope_DF) < INT_TOL) {
            return FALSE;
         } else {
            Dx = (y_intersect_DF - y_intersect_DE) / (slope_DE - slope_DF);
         }
         Dy = slope_DE * Dx + y_intersect_DE;
      }
   }
   *pcx = round(Dx);
   *pcy = round(Dy);
   if (Colinear(((double)Ax), ((double)Ay), Dx, Dy,
         ((double)Cx), ((double)Cy))) {
      if (saved_dir == INVALID) {
         *pdir = ArcDirection(*pcx, *pcy, Ax, Ay, Cx, Cy);
      } else {
         /* don't change dir */
      }
   } else {
      if (SegmentIntersects(Ax, Ay, Cx, Cy, ((double)Bx), ((double)By),
            Dx, Dy)) {
         *pdir = ArcDirection(*pcx, *pcy, Ax, Ay, Cx, Cy);
      } else {
         if (saved_dir == INVALID) {
            /* in this case, ArcDirection() gives the opposite direction */
            *pdir = !ArcDirection(*pcx, *pcy, Ax, Ay, Cx, Cy);
         } else {
            /* don't change dir */
         }
      }
   }
   return TRUE;
}

static
void HighLightInContinueArc(drawing_arc, OrigX, OrigY, saved_x, saved_y,
      ltx, lty, pw, ph, angle1, angle2, pdx, pdy, pradius, show_measure_cursor,
      end_show_measure_cursor, buf)
   int drawing_arc, OrigX, OrigY, saved_x, saved_y, ltx, lty;
   int *pw, *ph, angle1, angle2, *pdx, *pdy, *pradius;
   int show_measure_cursor, end_show_measure_cursor;
   char *buf;
{
   char r_buf[80], x_buf[80], y_buf[80];

   if (drawing_arc) {
      if (!show_measure_cursor) {
         XDrawArc(mainDisplay, drawWindow, drawGC, ltx, lty, *pw, *ph,
               angle1, angle2);
      }
      PixelToMeasurementUnit(r_buf, ABS_SIZE((*pw)>>1));
      sprintf(buf, "r=%s\ndegree=%1d", r_buf, abs(angle2>>6));
   } else {
      char *cl_or_r=(curChoice==DRAWEDGEARC ? "cl" : "r");

      if (!show_measure_cursor) {
         XDrawLine(mainDisplay, drawWindow, drawGC, OrigX, OrigY,
               saved_x, saved_y);
      }
      *pdx = saved_x - OrigX;
      *pdy = saved_y - OrigY;
      *pradius = (int)(sqrt((double)(((double)(*pdx))*((double)(*pdx)) +
            ((double)(*pdy))*((double)(*pdy)))));
      *pw = *ph = ((*pradius)<<1);
      PixelToMeasurementUnit(r_buf, ABS_SIZE(*pradius));
      PixelToMeasurementUnit(x_buf, ABS_X(saved_x));
      PixelToMeasurementUnit(y_buf, ABS_Y(saved_y));
      sprintf(buf, "%s=%s\nx=%s\ny=%s", cl_or_r, r_buf, x_buf, y_buf);
   }
   if (show_measure_cursor) {
      ShowMeasureCursor(saved_x, saved_y, buf, TRUE);
   }
   if (end_show_measure_cursor) {
      EndShowMeasureCursor(saved_x, saved_y, buf, TRUE);
   }
}

static
void ContinueArc(OrigX, OrigY)
   int OrigX, OrigY;
{
   int grid_x, grid_y, first_x=0, first_y=0, cx=0, cy=0;
   int end_x, end_y, saved_x, saved_y, dx, dy, radius;
   int done=FALSE, drawing_arc=FALSE;
   int dir=INVALID, ltx, lty, w, h, angle1, angle2=0;
   char buf[80], r_buf[80], x_buf[80], y_buf[80];
   char *cl_or_r=(curChoice==DRAWEDGEARC ? "cl" : "r");
   struct ObjRec *obj_ptr;
   XEvent input, ev;

   SetXorDrawGC(xorColorPixels[colorIndex]);

   grid_x = saved_x = OrigX;
   grid_y = saved_y = OrigY; 
   XDrawLine(mainDisplay, drawWindow, drawGC, OrigX, OrigY, saved_x, saved_y);

   PixelToMeasurementUnit(r_buf, 0);
   PixelToMeasurementUnit(x_buf, ABS_X(grid_x));
   PixelToMeasurementUnit(y_buf, ABS_Y(grid_y));
   sprintf(buf, "%s=%s\nx=%s\ny=%s", cl_or_r, r_buf, x_buf, y_buf);
   StartShowMeasureCursor(grid_x, grid_y, buf, TRUE);
   if (!debugNoPointerGrab) {
      XGrabPointer(mainDisplay, drawWindow, FALSE,
            PointerMotionMask | ButtonPressMask | ButtonReleaseMask,
            GrabModeAsync, GrabModeAsync, None, handCursor, CurrentTime);
   }
   if (curChoice == DRAWEDGEARC) {
      Msg(TgLoadCachedString(CSTID_SPECIFY_ARC_END));
      SetMouseStatus(TgLoadCachedString(CSTID_SET_ARC_END),
            TgLoadCachedString(CSTID_CANCEL), TgLoadCachedString(CSTID_CANCEL));
   } else {
      Msg(TgLoadCachedString(CSTID_SPECIFY_ARC_START));
      SetMouseStatus(TgLoadCachedString(CSTID_SET_ARC_START),
            TgLoadCachedString(CSTID_CANCEL), TgLoadCachedString(CSTID_CANCEL));
   }
   while (!done) {
      XNextEvent(mainDisplay, &input);

      if (input.type == Expose || input.type == VisibilityNotify) {
         ExposeEventHandler(&input, TRUE);
         SetXorDrawGC(xorColorPixels[colorIndex]);
      } else if (input.type == ButtonPress) {
         if (input.xbutton.button != Button1 || drawing_arc) {
            XUngrabPointer(mainDisplay, CurrentTime);
            HighLightInContinueArc(drawing_arc, OrigX, OrigY, saved_x,
                  saved_y, ltx, lty, &w, &h, angle1, angle2, &dx, &dy, &radius,
                  FALSE, TRUE, buf);
            done = TRUE;
            if (input.xbutton.button != Button1) {
               angle2 = 0;
            }
            Msg("");
            break;
         }
         /* drawing_arc is FALSE here */
         HighLightInContinueArc(drawing_arc, OrigX, OrigY, saved_x,
               saved_y, ltx, lty, &w, &h, angle1, angle2, &dx, &dy,
               &radius, TRUE, FALSE, buf);
         XDrawLine(mainDisplay, drawWindow, drawGC, OrigX, OrigY,
               saved_x, saved_y);
         first_x = saved_x;
         first_y = saved_y;
         drawing_arc = TRUE;
         if (OrigX == grid_x && OrigY == grid_y) {
            /* fake it as if the 1st point is ok but not the 2nd point */
            XUngrabPointer(mainDisplay, CurrentTime);
            grid_x = first_x;
            grid_y = first_y;
            done = TRUE;
         }
         if (curChoice == DRAWEDGEARC) {
            Msg(TgLoadCachedString(CSTID_SPECIFY_ARC_THIRD));
            SetMouseStatus(TgLoadCachedString(CSTID_SET_ARC_THIRD),
                  TgLoadCachedString(CSTID_CANCEL),
                  TgLoadCachedString(CSTID_CANCEL));
         } else {
            Msg(TgLoadCachedString(CSTID_SPECIFY_ARC_END));
            SetMouseStatus(TgLoadCachedString(CSTID_SET_ARC_END),
                  TgLoadCachedString(CSTID_CANCEL),
                  TgLoadCachedString(CSTID_CANCEL));
         }
         PixelToMeasurementUnit(r_buf, ABS_SIZE(radius));
         sprintf(buf, "r=%s\ndegree=0", r_buf);
         ShowMeasureCursor(saved_x, saved_y, buf, TRUE);
         if (done) {
            PixelToMeasurementUnit(r_buf, ABS_SIZE(radius));
            sprintf(buf, "r=%s\ndegree=0", r_buf);
            EndShowMeasureCursor(saved_x, saved_y, buf, TRUE);
         }
      } else if (input.type == MotionNotify || input.type == KeyPress ||
            input.type == KeyRelease) {
         if (input.type == KeyPress) {
            if (KeyPressEventIsEscape(&input.xkey)) {
               XUngrabPointer(mainDisplay, CurrentTime);
               HighLightInContinueArc(drawing_arc, OrigX, OrigY, saved_x,
                     saved_y, ltx, lty, &w, &h, angle1, angle2, &dx, &dy,
                     &radius, FALSE, TRUE, buf);
               done = TRUE;
               angle2 = 0;
               Msg("");
            }
         }
         if (done) {
            break;
         }
         HighLightInContinueArc(drawing_arc, OrigX, OrigY, saved_x, saved_y,
               ltx, lty, &w, &h, angle1, angle2, &dx, &dy, &radius,
               TRUE, FALSE, buf);
         if (input.type == KeyPress || input.type == KeyRelease) {
            end_x = saved_x;
            end_y = saved_y;
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
         GridXY(end_x, end_y, &grid_x, &grid_y);
         if (grid_x != saved_x || grid_y != saved_y) {
            if (drawing_arc) {
               if (curChoice == DRAWEDGEARC) {
                  /* finished with the endpoints of the arc */
                  if (dir != INVALID) {
                     XDrawArc(mainDisplay, drawWindow, drawGC, ltx, lty, w, h,
                           angle1, angle2);
                  }
                  saved_x = grid_x;
                  saved_y = grid_y;
                  if (!GetCenter(OrigX, OrigY, first_x, first_y, grid_x, grid_y,
                        &cx, &cy, &dir)) {
                     dir = INVALID;
                  } else {
                     ltx = cx; lty = cy; w = 0; h = 0; angle1 = angle2 = 0;
                     PointsToArc(cx, cy, OrigX, OrigY, first_x, first_y,
                           dir, TRUE, &ltx, &lty, &w, &h, &angle1, &angle2);
                     XDrawArc(mainDisplay, drawWindow, drawGC, ltx, lty, w, h,
                           angle1, angle2);
                  }
               } else {
                  /* finished with the center and the first point on the arc */
                  if (dir == INVALID) {
                     dir = ArcDirection(OrigX, OrigY, first_x, first_y,
                           grid_x, grid_y);
                     ltx = OrigX; lty = OrigY; w = 0; h = 0;
                     angle1 = angle2 = 0;
                     if (dir == ARC_CW) {
                        Msg(TgLoadCachedString(CSTID_SPECIFY_ARC_END_CW));
                     } else {
                        Msg(TgLoadCachedString(CSTID_SPECIFY_ARC_END_CCW));
                     }
                  }
                  XDrawArc(mainDisplay, drawWindow, drawGC, ltx, lty, w, h,
                        angle1, angle2);
                  saved_x = grid_x;
                  saved_y = grid_y;
                  PointsToArc(OrigX, OrigY, first_x, first_y, saved_x, saved_y,
                        dir, TRUE, &ltx, &lty, &w, &h, &angle1, &angle2);
                  XDrawArc(mainDisplay, drawWindow, drawGC, ltx, lty, w, h,
                        angle1, angle2);
               }
            } else {
               /* looking for the first point on the arc */
               XDrawLine(mainDisplay, drawWindow, drawGC, OrigX, OrigY,
                     saved_x, saved_y);
               saved_x = grid_x;
               saved_y = grid_y;
               XDrawLine(mainDisplay, drawWindow, drawGC, OrigX, OrigY,
                     saved_x, saved_y);
            }
         }
         HighLightInContinueArc(drawing_arc, OrigX, OrigY, saved_x, saved_y,
               ltx, lty, &w, &h, angle1, angle2, &dx, &dy, &radius,
               TRUE, FALSE, buf);
         MarkRulers(grid_x, grid_y);
         while (XCheckMaskEvent(mainDisplay, PointerMotionMask, &ev)) ;
      }
   }
   SetMouseStatus(NULL, NULL, NULL);
   if (angle2 == 0 || dir == INVALID) {
      Msg(TgLoadString(STID_NO_ARC_CREATED));
   } else {
      if (curChoice == DRAWEDGEARC) {
         obj_ptr = CreateArcObj(cx, cy, OrigX, OrigY, first_x, first_y,
               dir, ltx, lty, w, h, angle1, angle2, FALSE);
      } else {
         obj_ptr = CreateArcObj(OrigX, OrigY, first_x, first_y, saved_x,
               saved_y, dir, ltx, lty, w, h, angle1, angle2, FALSE);
      }
      RecordNewObjCmd();
      DrawArcObj(drawWindow, drawOrigX, drawOrigY, topObj);
      arcDrawn = TRUE;
      SetFileModified(TRUE);
   }
   XSync(mainDisplay, False);
}

void DrawArc(input)
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
      SaveStatusStrings();
      ContinueArc(grid_x, grid_y);
      RestoreStatusStrings();
   }
}

void SaveArcObj(FP, ObjPtr)
   FILE *FP;
   register struct ObjRec *ObjPtr;
{
   register struct ArcRec *arc_ptr=ObjPtr->detail.a;

   if (fprintf(FP, "arc('%s','',", colorMenuItems[ObjPtr->color]) == EOF) {
      writeFileFailed = TRUE;
   }
   if (fprintf(FP,
         "%1d,%1d,%1d,%1d,%1d,%1d,%1d,%1d,%1d,%1d,%1d,%1d,%1d,%1d,%1d,%1d,%1d,",
         arc_ptr->fill, arc_ptr->width, arc_ptr->pen, arc_ptr->dash,
         arc_ptr->ltx, arc_ptr->lty, arc_ptr->xc, arc_ptr->yc,
         arc_ptr->x1, arc_ptr->y1, arc_ptr->x2, arc_ptr->y2,
         arc_ptr->dir, arc_ptr->w, arc_ptr->h,
         arc_ptr->angle1, arc_ptr->angle2) == EOF) {
      writeFileFailed = TRUE;
   }
   if (fprintf(FP,
         "%1d,%1d,%1d,%1d,%1d,%1d,%1d,%1d,'%s','%s','%s',%1d,",
         ObjPtr->id, ObjPtr->rotation, arc_ptr->style, arc_ptr->aw,
         arc_ptr->ah, ObjPtr->locked, ObjPtr->ctm!=NULL,
         ObjPtr->invisible, arc_ptr->width_spec, arc_ptr->aw_spec,
         arc_ptr->ah_spec, ObjPtr->trans_pat) == EOF) {
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

void ReadArcObj(FP, Inbuf, ObjPtr)
   FILE *FP;
   char *Inbuf;
   struct ObjRec **ObjPtr;
{
   register struct ArcRec *arc_ptr;
   char color_str[40], bg_color_str[40], * s;
   char width_spec[40], aw_spec[40], ah_spec[40];
   int fill, trans_pat=FALSE, width=0, pen, dash, ltx, lty, w, h, id=0;
   int rotation, new_alloc, style, locked=FALSE;
   int aw=origArrowHeadW[0], ah=origArrowHeadH[0];
   int xc, yc, x1, y1, x2, y2, dir, angle1, angle2;
   int invisible=FALSE, transformed=FALSE;

   *ObjPtr = NULL;

   s = FindChar((int)'(', Inbuf);
   s = ParseStr(s, (int)',', color_str, sizeof(color_str));
   if (fileVersion >= 37) {
      s = ParseStr(s, (int)',', bg_color_str, sizeof(bg_color_str));
   }
   InitScan(s, ", \t\n");

   style = LS_PLAIN;
   rotation = 0;
   *width_spec = *aw_spec = *ah_spec = '\0';
   if (fileVersion <= 8) {
      *ObjPtr = NULL;
      sprintf(gszMsgBox, TgLoadString(STID_INVALID_ARC_VERSION), fileVersion);
      if (PRTGIF) {
         fprintf(stderr, "%s\n", gszMsgBox);
      } else {
         Msg(gszMsgBox);
      }
      return;
   } else if (fileVersion <= 13) {
      if (GETINT("arc", fill,   "fill") == INVALID ||
          GETINT("arc", width,  "width") == INVALID ||
          GETINT("arc", pen,    "pen") == INVALID ||
          GETINT("arc", dash,   "dash") == INVALID ||
          GETINT("arc", ltx,    "ltx") == INVALID ||
          GETINT("arc", lty,    "lty") == INVALID ||
          GETINT("arc", xc,     "xc") == INVALID ||
          GETINT("arc", yc,     "yc") == INVALID ||
          GETINT("arc", x1,     "x1") == INVALID ||
          GETINT("arc", y1,     "y1") == INVALID ||
          GETINT("arc", x2,     "x2") == INVALID ||
          GETINT("arc", y2,     "y2") == INVALID ||
          GETINT("arc", dir,    "direction") == INVALID ||
          GETINT("arc", w,      "width") == INVALID ||
          GETINT("arc", h,      "height") == INVALID ||
          GETINT("arc", angle1, "angle1") == INVALID ||
          GETINT("arc", angle2, "angle2") == INVALID ||
          GETINT("arc", id,     "id") == INVALID) {
         return;
      }
      if (id >= objId) objId = id+1;
   } else if (fileVersion <= 15) {
      if (GETINT("arc", fill,     "fill") == INVALID ||
          GETINT("arc", width,    "width") == INVALID ||
          GETINT("arc", pen,      "pen") == INVALID ||
          GETINT("arc", dash,     "dash") == INVALID ||
          GETINT("arc", ltx,      "ltx") == INVALID ||
          GETINT("arc", lty,      "lty") == INVALID ||
          GETINT("arc", xc,       "xc") == INVALID ||
          GETINT("arc", yc,       "yc") == INVALID ||
          GETINT("arc", x1,       "x1") == INVALID ||
          GETINT("arc", y1,       "y1") == INVALID ||
          GETINT("arc", x2,       "x2") == INVALID ||
          GETINT("arc", y2,       "y2") == INVALID ||
          GETINT("arc", dir,      "direction") == INVALID ||
          GETINT("arc", w,        "width") == INVALID ||
          GETINT("arc", h,        "height") == INVALID ||
          GETINT("arc", angle1,   "angle1") == INVALID ||
          GETINT("arc", angle2,   "angle2") == INVALID ||
          GETINT("arc", id,       "id") == INVALID ||
          GETINT("arc", rotation, "rotation") == INVALID) {
         return;
      }
      if (id >= objId) objId = id+1;
   } else if (fileVersion <= 16) {
      if (GETINT("arc", fill,     "fill") == INVALID ||
          GETINT("arc", width,    "width") == INVALID ||
          GETINT("arc", pen,      "pen") == INVALID ||
          GETINT("arc", dash,     "dash") == INVALID ||
          GETINT("arc", ltx,      "ltx") == INVALID ||
          GETINT("arc", lty,      "lty") == INVALID ||
          GETINT("arc", xc,       "xc") == INVALID ||
          GETINT("arc", yc,       "yc") == INVALID ||
          GETINT("arc", x1,       "x1") == INVALID ||
          GETINT("arc", y1,       "y1") == INVALID ||
          GETINT("arc", x2,       "x2") == INVALID ||
          GETINT("arc", y2,       "y2") == INVALID ||
          GETINT("arc", dir,      "direction") == INVALID ||
          GETINT("arc", w,        "width") == INVALID ||
          GETINT("arc", h,        "height") == INVALID ||
          GETINT("arc", angle1,   "angle1") == INVALID ||
          GETINT("arc", angle2,   "angle2") == INVALID ||
          GETINT("arc", id,       "id") == INVALID ||
          GETINT("arc", rotation, "rotation") == INVALID ||
          GETINT("arc", style,    "style") == INVALID) {
         return;
      }
      if (id >= objId) objId = id+1;
   } else if (fileVersion <= 25) {
      if (GETINT("arc", fill,     "fill") == INVALID ||
          GETINT("arc", width,    "width") == INVALID ||
          GETINT("arc", pen,      "pen") == INVALID ||
          GETINT("arc", dash,     "dash") == INVALID ||
          GETINT("arc", ltx,      "ltx") == INVALID ||
          GETINT("arc", lty,      "lty") == INVALID ||
          GETINT("arc", xc,       "xc") == INVALID ||
          GETINT("arc", yc,       "yc") == INVALID ||
          GETINT("arc", x1,       "x1") == INVALID ||
          GETINT("arc", y1,       "y1") == INVALID ||
          GETINT("arc", x2,       "x2") == INVALID ||
          GETINT("arc", y2,       "y2") == INVALID ||
          GETINT("arc", dir,      "direction") == INVALID ||
          GETINT("arc", w,        "width") == INVALID ||
          GETINT("arc", h,        "height") == INVALID ||
          GETINT("arc", angle1,   "angle1") == INVALID ||
          GETINT("arc", angle2,   "angle2") == INVALID ||
          GETINT("arc", id,       "id") == INVALID ||
          GETINT("arc", rotation, "rotation") == INVALID ||
          GETINT("arc", style,    "style") == INVALID ||
          GETINT("arc", aw,       "arrow head w") == INVALID ||
          GETINT("arc", ah,       "arrow head h") == INVALID) {
         return;
      }
      if (id >= objId) objId = id+1;
   } else if (fileVersion <= 32) {
      if (GETINT("arc", fill,     "fill") == INVALID ||
          GETINT("arc", width,    "width") == INVALID ||
          GETINT("arc", pen,      "pen") == INVALID ||
          GETINT("arc", dash,     "dash") == INVALID ||
          GETINT("arc", ltx,      "ltx") == INVALID ||
          GETINT("arc", lty,      "lty") == INVALID ||
          GETINT("arc", xc,       "xc") == INVALID ||
          GETINT("arc", yc,       "yc") == INVALID ||
          GETINT("arc", x1,       "x1") == INVALID ||
          GETINT("arc", y1,       "y1") == INVALID ||
          GETINT("arc", x2,       "x2") == INVALID ||
          GETINT("arc", y2,       "y2") == INVALID ||
          GETINT("arc", dir,      "direction") == INVALID ||
          GETINT("arc", w,        "width") == INVALID ||
          GETINT("arc", h,        "height") == INVALID ||
          GETINT("arc", angle1,   "angle1") == INVALID ||
          GETINT("arc", angle2,   "angle2") == INVALID ||
          GETINT("arc", id,       "id") == INVALID ||
          GETINT("arc", rotation, "rotation") == INVALID ||
          GETINT("arc", style,    "style") == INVALID ||
          GETINT("arc", aw,       "arrow head w") == INVALID ||
          GETINT("arc", ah,       "arrow head h") == INVALID ||
          GETINT("arc", locked,   "locked") == INVALID) {
         return;
      }
      if (id >= objId) objId = id+1;
   } else if (fileVersion <= 34) {
      if (GETINT("arc", fill,        "fill") == INVALID ||
          GETINT("arc", width,       "width") == INVALID ||
          GETINT("arc", pen,         "pen") == INVALID ||
          GETINT("arc", dash,        "dash") == INVALID ||
          GETINT("arc", ltx,         "ltx") == INVALID ||
          GETINT("arc", lty,         "lty") == INVALID ||
          GETINT("arc", xc,          "xc") == INVALID ||
          GETINT("arc", yc,          "yc") == INVALID ||
          GETINT("arc", x1,          "x1") == INVALID ||
          GETINT("arc", y1,          "y1") == INVALID ||
          GETINT("arc", x2,          "x2") == INVALID ||
          GETINT("arc", y2,          "y2") == INVALID ||
          GETINT("arc", dir,         "direction") == INVALID ||
          GETINT("arc", w,           "width") == INVALID ||
          GETINT("arc", h,           "height") == INVALID ||
          GETINT("arc", angle1,      "angle1") == INVALID ||
          GETINT("arc", angle2,      "angle2") == INVALID ||
          GETINT("arc", id,          "id") == INVALID ||
          GETINT("arc", rotation,    "rotation") == INVALID ||
          GETINT("arc", style,       "style") == INVALID ||
          GETINT("arc", aw,          "arrow head w") == INVALID ||
          GETINT("arc", ah,          "arrow head h") == INVALID ||
          GETINT("arc", locked,      "locked") == INVALID ||
          GETINT("arc", transformed, "transformed") == INVALID ||
          GETINT("arc", invisible,   "invisible") == INVALID ||
          GETSTR("arc", width_spec,  "width_spec") == INVALID ||
          GETSTR("arc", aw_spec,     "aw_spec") == INVALID ||
          GETSTR("arc", ah_spec,     "ah_spec") == INVALID) {
         return;
      }
      if (id >= objId) objId = id+1;
      UtilRemoveQuotes(width_spec);
      UtilRemoveQuotes(aw_spec);
      UtilRemoveQuotes(ah_spec);
   } else {
      if (GETINT("arc", fill,        "fill") == INVALID ||
          GETINT("arc", width,       "width") == INVALID ||
          GETINT("arc", pen,         "pen") == INVALID ||
          GETINT("arc", dash,        "dash") == INVALID ||
          GETINT("arc", ltx,         "ltx") == INVALID ||
          GETINT("arc", lty,         "lty") == INVALID ||
          GETINT("arc", xc,          "xc") == INVALID ||
          GETINT("arc", yc,          "yc") == INVALID ||
          GETINT("arc", x1,          "x1") == INVALID ||
          GETINT("arc", y1,          "y1") == INVALID ||
          GETINT("arc", x2,          "x2") == INVALID ||
          GETINT("arc", y2,          "y2") == INVALID ||
          GETINT("arc", dir,         "direction") == INVALID ||
          GETINT("arc", w,           "width") == INVALID ||
          GETINT("arc", h,           "height") == INVALID ||
          GETINT("arc", angle1,      "angle1") == INVALID ||
          GETINT("arc", angle2,      "angle2") == INVALID ||
          GETINT("arc", id,          "id") == INVALID ||
          GETINT("arc", rotation,    "rotation") == INVALID ||
          GETINT("arc", style,       "style") == INVALID ||
          GETINT("arc", aw,          "arrow head w") == INVALID ||
          GETINT("arc", ah,          "arrow head h") == INVALID ||
          GETINT("arc", locked,      "locked") == INVALID ||
          GETINT("arc", transformed, "transformed") == INVALID ||
          GETINT("arc", invisible,   "invisible") == INVALID ||
          GETSTR("arc", width_spec,  "width_spec") == INVALID ||
          GETSTR("arc", aw_spec,     "aw_spec") == INVALID ||
          GETSTR("arc", ah_spec,     "ah_spec") == INVALID ||
          GETINT("arc", trans_pat,   "trans_pat") == INVALID) {
         return;
      }
      if (id >= objId) objId = id+1;
      UtilRemoveQuotes(width_spec);
      UtilRemoveQuotes(aw_spec);
      UtilRemoveQuotes(ah_spec);
   }

   if (dir == ARC_CCW && angle2 < 0) {
      sprintf(gszMsgBox, TgLoadString(STID_WARN_INCONSIST_ARC_DIR));
      if (PRTGIF) {
         fprintf(stderr, "%s\n", gszMsgBox);
      } else {
         Msg(gszMsgBox);
      }
      SetFileModified(TRUE);
      dir = ARC_CW;
   } else if (dir == ARC_CW && angle2 > 0) {
      sprintf(gszMsgBox, TgLoadString(STID_WARN_INCONSIST_ARC_DIR));
      if (PRTGIF) {
         fprintf(stderr, "%s\n", gszMsgBox);
      } else {
         Msg(gszMsgBox);
      }
      SetFileModified(TRUE);
      dir = ARC_CCW;
   }

   if (fileVersion <= 16 && width <= 6) {
      aw = origArrowHeadW[width];
      ah = origArrowHeadH[width];
      width = origWidthOfLine[width];
   }
   if (fileVersion <= 32) {
      sprintf(width_spec, "%1d", width);
      sprintf(aw_spec, "%1d", aw);
      sprintf(ah_spec, "%1d", ah);
   }
   fill = UpgradePenFill(fill);
   pen = UpgradePenFill(pen);

   *ObjPtr = (struct ObjRec *)malloc(sizeof(struct ObjRec));
   if (*ObjPtr == NULL) FailAllocMessage();
   memset(*ObjPtr, 0, sizeof(struct ObjRec));
   arc_ptr = (struct ArcRec *)malloc(sizeof(struct ArcRec));
   if (arc_ptr == NULL) FailAllocMessage();
   memset(arc_ptr, 0, sizeof(struct ArcRec));

   (*ObjPtr)->trans_pat = trans_pat;
   arc_ptr->fill = fill;
   arc_ptr->width = width;
   arc_ptr->aw = aw;
   arc_ptr->ah = ah;
   UtilStrCpyN(arc_ptr->width_spec, sizeof(arc_ptr->width_spec), width_spec);
   UtilStrCpyN(arc_ptr->aw_spec, sizeof(arc_ptr->aw_spec), aw_spec);
   UtilStrCpyN(arc_ptr->ah_spec, sizeof(arc_ptr->ah_spec), ah_spec);
   arc_ptr->pen = pen;
   arc_ptr->dash = dash;
   arc_ptr->style = style;

   arc_ptr->xc = xc;         arc_ptr->yc = yc;
   arc_ptr->x1 = x1;         arc_ptr->y1 = y1;
   arc_ptr->x2 = x2;         arc_ptr->y2 = y2;
   arc_ptr->dir = dir;
   arc_ptr->ltx = ltx;       arc_ptr->lty = lty;
   arc_ptr->w = w;           arc_ptr->h = h;
   arc_ptr->angle1 = arc_ptr->a_angle1 = angle1;
   arc_ptr->angle2 = arc_ptr->a_angle2 = angle2;

   arc_ptr->rotated_n = 0;
   arc_ptr->rotated_vlist = NULL;
   arc_ptr->rotated_asn = 0;
   arc_ptr->rotated_asvlist = NULL;

   (*ObjPtr)->detail.a = arc_ptr;

   (*ObjPtr)->type = OBJ_ARC;
   (*ObjPtr)->color = QuickFindColorIndex(*ObjPtr, color_str, &new_alloc, TRUE);
   UtilStrCpyN((*ObjPtr)->color_str, sizeof((*ObjPtr)->color_str), color_str);
   (*ObjPtr)->dirty = FALSE;
   (*ObjPtr)->id = id;
   (*ObjPtr)->rotation = rotation;
   (*ObjPtr)->locked = locked;
   (*ObjPtr)->ctm = NULL;
   (*ObjPtr)->invisible = invisible;
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
      if (GETINT("arc", real_x,           "real_x") == INVALID ||
          GETINT("arc", real_y,           "real_y") == INVALID ||
          GETINT("arc", orig_obbox.ltx,   "orig_obbox.ltx") == INVALID ||
          GETINT("arc", orig_obbox.lty,   "orig_obbox.lty") == INVALID ||
          GETINT("arc", orig_obbox.rbx,   "orig_obbox.rbx") == INVALID ||
          GETINT("arc", orig_obbox.rby,   "orig_obbox.rby") == INVALID ||
          GETDBL("arc", ctm->m[CTM_SX],   "CTM_SX") == INVALID ||
          GETDBL("arc", ctm->m[CTM_SIN],  "CTM_SIN") == INVALID ||
          GETDBL("arc", ctm->m[CTM_MSIN], "CTM_MSIN") == INVALID ||
          GETDBL("arc", ctm->m[CTM_SY],   "CTM_SY") == INVALID ||
          GETINT("arc", ctm->t[CTM_TX],   "CTM_TX") == INVALID ||
          GETINT("arc", ctm->t[CTM_TY],   "CTM_TY") == INVALID) {
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
   AdjObjSplineVs(*ObjPtr);
   AdjObjBBox(*ObjPtr);
}

void SetArcPropMask(ObjPtr, plMask, plSkip, pProp)
   struct ObjRec *ObjPtr;
   long *plMask, *plSkip;
   struct PropertiesRec *pProp;
{
   struct ArcRec *arc_ptr=ObjPtr->detail.a;

   SetCTMPropertyMask(ObjPtr->ctm, plMask, plSkip, pProp);

   SetIntPropertyMask(PROP_MASK_COLOR, ObjPtr->color,
         colorMenuItems[ObjPtr->color], plMask, plSkip, pProp);
   SetIntPropertyMask(PROP_MASK_WIDTH, arc_ptr->width, arc_ptr->width_spec,
         plMask, plSkip, pProp);
   SetIntPropertyMask(PROP_MASK_AW, arc_ptr->aw, arc_ptr->aw_spec,
         plMask, plSkip, pProp);
   SetIntPropertyMask(PROP_MASK_AH, arc_ptr->ah, arc_ptr->ah_spec,
         plMask, plSkip, pProp);

   SetIntPropertyMask(PROP_MASK_TRANSPAT, ObjPtr->trans_pat, NULL,
         plMask, plSkip, pProp);
   SetIntPropertyMask(PROP_MASK_FILL, arc_ptr->fill, NULL,
         plMask, plSkip, pProp);
   SetIntPropertyMask(PROP_MASK_PEN, arc_ptr->pen, NULL,
         plMask, plSkip, pProp);
   SetIntPropertyMask(PROP_MASK_DASH, arc_ptr->dash, NULL,
         plMask, plSkip, pProp);
   SetIntPropertyMask(PROP_MASK_ARROW_STYLE, arc_ptr->style, NULL,
         plMask, plSkip, pProp);
}

void FreeArcObj(ObjPtr)
   struct ObjRec *ObjPtr;
{
   if (ObjPtr->detail.a->rotated_vlist != NULL) {
      free(ObjPtr->detail.a->rotated_vlist);
   }
   if (ObjPtr->detail.a->rotated_asvlist != NULL) {
      free(ObjPtr->detail.a->rotated_asvlist);
   }
   free(ObjPtr->detail.a);
   free(ObjPtr);
}

static
int ParseArcSpec(spec, seperator, radius, dir, theta1, theta2, error_str)
   char *spec, *error_str;
   int seperator, *radius, *dir, *theta1, *theta2;
{
   char *s, buf[MAXSTRING], tmp_buf[MAXSTRING];

   strcpy(tmp_buf, spec);

   s = ParseStr(tmp_buf, seperator, buf, sizeof(buf));
   if (*s != '\0') {
      UtilTrimBlanks(buf);
      if (!GetDimension(buf, TRUE, radius)) *s = '\0';
   }
   if (*s == '\0') {
      strcpy(error_str, TgLoadString(STID_INVALID_ARC_SPEC_RADIUS));
      return FALSE;
   }
   s = ParseStr(s, seperator, buf, sizeof(buf));
   UtilTrimBlanks(buf);
   switch (*buf) {
   case '+': *dir = ARC_CW; break;
   case '-': *dir = ARC_CCW; break;
   default: *s = '\0'; break;
   }
   if (*s == '\0') {
      strcpy(error_str, TgLoadString(STID_INVALID_ARC_SPEC_DIR));
      return FALSE;
   }
   s = ParseStr(s, seperator, buf, sizeof(buf));
   if (*s == '\0') {
      strcpy(error_str, TgLoadString(STID_INVALID_ARC_SPEC_THETA1));
      return FALSE;
   }
   UtilTrimBlanks(buf);
   UtilTrimBlanks(s);
   *theta1 = atoi(buf);
   *theta2 = atoi(s);

   return TRUE;
}

void MakePreciseArc()
{
   int r = 0, dir = 0, x1, y1, x2, y2, theta1, theta2, angle2=0;
   char spec[MAXSTRING], error_str[MAXSTRING];
   double angle_in_radian;
   struct ObjRec *obj_ptr;

   *spec = '\0';
   Dialog(TgLoadString(STID_ARC_SPEC), NULL, spec);
   UtilTrimBlanks(spec);
   if (*spec == '\0') return;

   TieLooseEnds();
   SetCurChoice(NOTHING);
   if (topSel!=NULL) { HighLightReverse(); RemoveAllSel(); }

   if (!ParseArcSpec(spec, (int)',', &r, &dir, &theta1, &theta2, error_str) &&
         !ParseArcSpec(spec, (int)' ', &r, &dir, &theta1, &theta2, error_str)) {
      sprintf(gszMsgBox, error_str, spec);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return;
   }
   if (r < 1) {
      sprintf(gszMsgBox, TgLoadString(STID_CANNOT_CREATE_ARC_WITH_RAD1),
            TOOL_NAME, r);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return;
   }
   switch (dir) {
   case ARC_CCW: theta1 += 90;        theta2 += 90;        break;
   case ARC_CW:  theta1 = -theta1+90; theta2 = -theta2+90; break;
   }

   angle_in_radian = theta1 * M_PI / 180;
   x1 = round(r*cos(angle_in_radian));
   y1 = -round(r*sin(angle_in_radian));
   angle_in_radian = theta2 * M_PI / 180;
   x2 = round(r*cos(angle_in_radian));
   y2 = -round(r*sin(angle_in_radian));

   while (theta1 < 0) theta1 += 360;
   while (theta2 > theta1) theta2 -= 360;
   while (theta2 < theta1) theta2 += 360;

   switch (dir) {
   case ARC_CCW:
      angle2 = theta2-theta1;
      if (angle2 == 0) angle2 = 360;
      break;
   case ARC_CW:
      angle2 = theta2-theta1-360;
      break;
   }
   obj_ptr = CreateArcObj(0, 0, x1, y1, x2, y2, dir, -r, -r, r*2, r*2,
         theta1*64, angle2*64, FALSE);

   PlaceTopObj(obj_ptr, NULL, NULL);
   SelectTopObj();
   RecordNewObjCmd();
   SetFileModified(TRUE);
   justDupped = FALSE;
}

void PreciseRotateAnArc()
{
}
