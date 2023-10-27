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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/animate.c,v 1.6 2011/05/16 16:21:56 william Exp $
 */

#define _INCLUDE_FROM_ANIMATE_C_

#include "tgifdefs.h"

#include "animate.e"
#include "color.e"
#include "dialog.e"
#include "msg.e"
#include "poly.e"
#include "raster.e"
#include "select.e"
#include "setup.e"
#include "strtbl.e"

#define SEND_SPEED 8

#define TOKEN_R 8

static XPoint	savedToken[5];

void AnimateSend(PolyPtr, Speed, Pixel)
   struct PolyRec *PolyPtr;
   int Speed, Pixel;
{
   register int delta, i;
   register XPoint *token;
   int x, y, num_pts, j, x_dist, y_dist;
   IntPoint *v;
   struct BBRec bbox;
   double slope, delta_x, delta_y;
   XGCValues values;

   values.foreground = Pixel;
   values.function = GXxor;
   values.line_style = FillSolid;
#ifdef NO_THIN_LINE
   values.line_width = 1;
#else
   values.line_width = 0;
#endif
   XChangeGC(mainDisplay, drawGC,
         GCForeground | GCFunction | GCLineStyle | GCLineWidth, &values);

   bbox.ltx = 0; bbox.lty = 0; bbox.rbx = 2*TOKEN_R; bbox.rby = 2*TOKEN_R;

   num_pts = PolyPtr->n;
   v = PolyPtr->vlist;
   token = (XPoint *)malloc(5*sizeof(XPoint));
   if (token == NULL) FailAllocMessage();

   for (j = 0; j < num_pts-1; j++) {
      x = OFFSET_X(v[j].x);
      y = OFFSET_Y(v[j].y);
      token[0].x = (short)(x - TOKEN_R); token[0].y = (short)(y - TOKEN_R);
      token[1].x = (short)(x + TOKEN_R); token[1].y = (short)(y - TOKEN_R);
      token[2].x = (short)(x + TOKEN_R); token[2].y = (short)(y + TOKEN_R);
      token[3].x = (short)(x - TOKEN_R); token[3].y = (short)(y + TOKEN_R);
      token[4].x = (short)(x - TOKEN_R); token[4].y = (short)(y - TOKEN_R);
      XFillPolygon(mainDisplay, drawWindow, drawGC, token, 5, Convex,
            CoordModeOrigin);
      if (v[j].x == v[j+1].x) {
         /* moving vertical */
         if ((y_dist = ZOOMED_SIZE(v[j+1].y-v[j].y)) > 0) {
            /* moving down */
            for (delta = 0; delta < y_dist; delta += Speed) {
               XFillPolygon(mainDisplay, drawWindow, drawGC, token, 5, Convex,
                     CoordModeOrigin);
               for (i = 0; i < 5; i++) token[i].y += Speed;
               XFillPolygon(mainDisplay, drawWindow, drawGC, token, 5, Convex,
                     CoordModeOrigin);
            }
         } else {
            /* moving up */
            for (delta = y_dist; delta < 0; delta += Speed) {
               XFillPolygon(mainDisplay, drawWindow, drawGC, token, 5, Convex,
                     CoordModeOrigin);
               for (i = 0; i < 5; i++) token[i].y -= Speed;
               XFillPolygon(mainDisplay, drawWindow, drawGC, token, 5, Convex,
                     CoordModeOrigin);
            }
         }
      } else if (v[j].y == v[j+1].y) {
         /* moving horizontal */
         if ((x_dist = ZOOMED_SIZE(v[j+1].x-v[j].x)) > 0) {
            /* moving right */
            for (delta = 0; delta < x_dist; delta += Speed) {
               XFillPolygon(mainDisplay, drawWindow, drawGC, token, 5, Convex,
                     CoordModeOrigin);
               for (i = 0; i < 5; i++) token[i].x += Speed;
               XFillPolygon(mainDisplay, drawWindow, drawGC, token, 5, Convex,
                     CoordModeOrigin);
            }
         } else {
            /* moving left */
            for (delta = x_dist; delta < 0; delta += Speed) {
               XFillPolygon(mainDisplay, drawWindow, drawGC, token, 5, Convex,
                     CoordModeOrigin);
               for (i = 0; i < 5; i++) token[i].x -= Speed;
               XFillPolygon(mainDisplay, drawWindow, drawGC, token, 5, Convex,
                     CoordModeOrigin);
            }
         }
      } else {
         /* moving diagonally */
         x_dist = ZOOMED_SIZE(v[j+1].x-v[j].x);
         y_dist = ZOOMED_SIZE(v[j+1].y-v[j].y);
         for (i = 0; i < 5; i++) {
            savedToken[i].x = token[i].x;
            savedToken[i].y = token[i].y;
         }
         if (abs(x_dist) > abs(y_dist)) {
            /* moving in the x direction */
            slope = ((double)y_dist) / ((double)x_dist);
            if (x_dist > 0) {
               /* moving right */
               for (delta = 0; delta < x_dist; delta += Speed) {
                  XFillPolygon(mainDisplay, drawWindow, drawGC, token, 5,
                        Convex, CoordModeOrigin);
                  delta_y = slope * ((double)delta);
                  for (i = 0; i < 5; i++) {
                     token[i].x = savedToken[i].x + delta;
                     token[i].y = savedToken[i].y + delta_y;
                  }
                  XFillPolygon(mainDisplay, drawWindow, drawGC, token, 5,
                        Convex, CoordModeOrigin);
               }
            } else {
               /* moving left */
               for (delta = 0; delta > x_dist; delta -= Speed) {
                  XFillPolygon(mainDisplay, drawWindow, drawGC, token, 5,
                        Convex, CoordModeOrigin);
                  delta_y = slope * ((double)delta);
                  for (i = 0; i < 5; i++) {
                     token[i].x = savedToken[i].x + delta;
                     token[i].y = savedToken[i].y + delta_y;
                  }
                  XFillPolygon(mainDisplay, drawWindow, drawGC, token, 5,
                        Convex, CoordModeOrigin);
               }
            }
         } else {
            /* moving in the y direction */
            slope = ((double)x_dist) / ((double)y_dist);
            if (y_dist > 0) {
               /* moving down */
               for (delta = 0; delta < y_dist; delta += Speed) {
                  XFillPolygon(mainDisplay, drawWindow, drawGC, token, 5,
                        Convex, CoordModeOrigin);
                  delta_x = slope * ((double)delta);
                  for (i = 0; i < 5; i++) {
                     token[i].x = savedToken[i].x + delta_x;
                     token[i].y = savedToken[i].y + delta;
                  }
                  XFillPolygon(mainDisplay, drawWindow, drawGC, token, 5,
                        Convex, CoordModeOrigin);
               }
            } else {
               /* moving up */
               for (delta = 0; delta > y_dist; delta -= Speed) {
                  XFillPolygon(mainDisplay, drawWindow, drawGC, token, 5,
                        Convex, CoordModeOrigin);
                  delta_x = slope * ((double)delta);
                  for (i = 0; i < 5; i++) {
                     token[i].x = savedToken[i].x + delta_x;
                     token[i].y = savedToken[i].y + delta;
                  }
                  XFillPolygon(mainDisplay, drawWindow, drawGC, token, 5,
                        Convex, CoordModeOrigin);
               }
            }
         }
      }
      XFillPolygon(mainDisplay, drawWindow, drawGC, token, 5, Convex,
            CoordModeOrigin);
   }
   free(token);
}

void AnimateSel()
{
   if (topSel != botSel || topSel == NULL || topSel->obj->type != OBJ_POLY) {
      MsgBox(TgLoadString(STID_SELECT_ONLY_ONE_POLY), TOOL_NAME, INFO_MB);
      return;
   }
   AnimateSend(topSel->obj->detail.p, SEND_SPEED,
         xorColorPixels[topSel->obj->color]);
}

void AnimateFlashColor(ObjPtr, ColorIndex)
   struct ObjRec *ObjPtr;
   int ColorIndex;
{
   int saved_color_index = ObjPtr->color;

   ObjPtr->color = ColorIndex;
   DrawPolyObj(drawWindow, drawOrigX, drawOrigY, ObjPtr);
   ObjPtr->color = saved_color_index;
   DrawPolyObj(drawWindow, drawOrigX, drawOrigY, ObjPtr);
}

void FlashSelColor()
   /* This routine is kind of stupid.  It's left for compatibity reason. */
{
   register int i;

   if (topSel != botSel || topSel == NULL || topSel->obj->type != OBJ_POLY) {
      MsgBox(TgLoadString(STID_SELECT_ONLY_ONE_POLY), TOOL_NAME, INFO_MB);
      return;
   }
   for (i = 0; i < maxColors; i++) {
      if (strcmp("white", colorMenuItems[i]) == 0) {
         break;
      }
   }
   AnimateFlashColor(topSel->obj, i);
}
