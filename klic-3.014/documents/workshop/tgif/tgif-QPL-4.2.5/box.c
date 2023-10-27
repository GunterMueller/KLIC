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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/box.c,v 1.13 2011/06/09 16:11:41 cvsps Exp $
 */

#define _INCLUDE_FROM_BOX_C_

#include "tgifdefs.h"

#include "attr.e"
#include "auxtext.e"
#include "box.e"
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
#include "pattern.e"
#include "poly.e"
#include "ps.e"
#include "raster.e"
#include "rect.e"
#include "ruler.e"
#include "select.e"
#include "setup.e"
#include "strtbl.e"
#include "util.e"
#include "xpixmap.e"

int boxDrawn=FALSE;

static XPoint bv[5];

void MyBox(window, gc, x1, y1, x2, y2)
   /* Hollow box, solid outline with width=1 */
   Window window;
   GC gc;
   int x1, y1, x2, y2;
{
   bv[0].x = (short)x1; bv[0].y = (short)y1;
   bv[1].x = (short)x1; bv[1].y = (short)y2;
   bv[2].x = (short)x2; bv[2].y = (short)y2;
   bv[3].x = (short)x2; bv[3].y = (short)y1;
   bv[4].x = (short)x1; bv[4].y = (short)y1;
   XDrawLines(mainDisplay, window, gc, bv, 5, CoordModeOrigin);
}

static
void DumpBoxPath(FP, ObjPtr, LtX, LtY, RbX, RbY, Width, Pen, Dash, TransPat)
   FILE *FP;
   struct ObjRec *ObjPtr;
   int LtX, LtY, RbX, RbY, Width, Pen, Dash, TransPat;
{
   register int i;
   int w_is_int=TRUE;
   char *width_spec=ObjPtr->detail.b->width_spec;
   double dw=GetWidthInDouble(Width, width_spec, &w_is_int);

   fprintf(FP, "   %s\n", gPsCmd[PS_GSAVE]);
   if (!colorDump && useGray && Pen > BACKPAT) {
      GrayCheck(Pen);
      fprintf(FP, "      %s %s\n", GrayStr(Pen), gPsCmd[PS_SETGRAY]);
   }
   DumpRectPath(FP, LtX, LtY, RbX, RbY, 6, TRUE);

   if (ObjPtr->ctm != NULL) {
      fprintf(FP, "      %s\n", &(gPsCmd[PS_TGIFSETMATRIX])[1]);
   }
   if (w_is_int) {
      if (Width != 1) {
         fprintf(FP, "      %1d %s\n", Width, gPsCmd[PS_SETLINEWIDTH]);
      }
   } else {
      fprintf(FP, "      %.3f %s\n", dw, gPsCmd[PS_SETLINEWIDTH]);
   }
   if (Dash != 0) {
      fprintf(FP, "      [");
      for (i = 0; i < dashListLength[Dash]-1; i++) {
         fprintf(FP, "%1d ", (int)(dashList[Dash][i]));
      }
      fprintf(FP, "%1d] 0 %s\n",
            (int)(dashList[Dash][dashListLength[Dash]-1]), gPsCmd[PS_SETDASH]);
   }

   switch (Pen) {
   case SOLIDPAT: fprintf(FP, "      %s\n", gPsCmd[PS_STROKE]); break;
   case BACKPAT:
      if (!TransPat) {
         fprintf(FP, "      1 %s %s 0 %s\n",
               gPsCmd[PS_SETGRAY], gPsCmd[PS_STROKE], gPsCmd[PS_SETGRAY]);
      }
      break;
   default:
      if (colorDump || !useGray) {
         if (preDumpSetup) PSUseColorPattern();
         fprintf(FP, "      %s\n", gPsCmd[PS_FLATTENPATH]);
         DumpPatFill(FP, Pen, ObjPtr->bbox, 6, TRUE);
      } else {
         fprintf(FP, "      %s\n", gPsCmd[PS_STROKE]);
      }
      break;
   }
   fprintf(FP, "   %s\n", gPsCmd[PS_GRESTORE]);
}
 
void DumpBoxObj(FP, ObjPtr)
   FILE *FP;
   struct ObjRec *ObjPtr;
{
   int ltx, lty, rbx, rby, fill, width, pen, dash, color_index, trans_pat;

   if (ObjPtr->ctm == NULL) {
      ltx = ObjPtr->obbox.ltx;
      lty = ObjPtr->obbox.lty;
      rbx = ObjPtr->obbox.rbx;
      rby = ObjPtr->obbox.rby;
   } else {
      ltx = ObjPtr->orig_obbox.ltx;
      lty = ObjPtr->orig_obbox.lty;
      rbx = ObjPtr->orig_obbox.rbx;
      rby = ObjPtr->orig_obbox.rby;
   }
   trans_pat = ObjPtr->trans_pat;
   fill = ObjPtr->detail.b->fill;
   pen = ObjPtr->detail.b->pen;
   width = ObjPtr->detail.b->width;
   dash = ObjPtr->detail.b->dash;

   if ((fill == NONEPAT || (trans_pat && fill == BACKPAT)) &&
         (pen == NONEPAT || (trans_pat && pen == BACKPAT))) {
      return;
   }
   fprintf(FP, "%% BOX\n");
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
         /* solid black object */
         DumpRectPath(FP, ltx, lty, rbx, rby, 0, FALSE);
         fprintf(FP, "%s\n", gPsCmd[PS_FILL]);
         break;
      case BACKPAT:
         if (!trans_pat) {
            /* solid white object */
            DumpRectPath(FP, ltx, lty, rbx, rby, 0, FALSE);
            fprintf(FP, "1 %s %s\n", gPsCmd[PS_SETGRAY], gPsCmd[PS_FILL]);
            DumpRGBColorLine(FP, color_index, 3, TRUE);
         }
         break;
      default:
         /* patterned */
         fprintf(FP, "%s\n", gPsCmd[PS_GSAVE]);
         if (colorDump || !useGray) {
            if (preDumpSetup) PSUseColorPattern();
            if (!trans_pat) {
               DumpRectPath(FP, ltx, lty, rbx, rby, 3, FALSE);
               fprintf(FP, "1 %s %s\n", gPsCmd[PS_SETGRAY], gPsCmd[PS_FILL]);
               DumpRGBColorLine(FP, color_index, 3, TRUE);
            }
            DumpRectPath(FP, ltx, lty, rbx, rby, 3, FALSE);
            fprintf(FP, "%s %s\n", gPsCmd[PS_EOCLIP], gPsCmd[PS_NEWPATH]);
            DumpPatFill(FP, fill, ObjPtr->bbox, 3, TRUE);
         } else {
            GrayCheck(fill);
            fprintf(FP, "   %s %s\n", GrayStr(fill), gPsCmd[PS_SETGRAY]);
            DumpRectPath(FP, ltx, lty, rbx, rby, 3, FALSE);
            fprintf(FP, "%s\n", gPsCmd[PS_FILL]);
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
   fprintf(FP, "   10 %s\n", gPsCmd[PS_SETMITERLIMIT]);

   if ((colorDump || !useGray) && pen > BACKPAT) {
      DumpBoxPath(FP, ObjPtr, ltx, lty, rbx, rby, width, BACKPAT, 0, trans_pat);
      DumpRGBColorLine(FP, color_index, 3, TRUE);
   }
   DumpBoxPath(FP, ObjPtr, ltx, lty, rbx, rby, width, pen, dash, trans_pat);

   fprintf(FP, "%s\n", gPsCmd[PS_GRESTORE]);
   if (ObjPtr->ctm != NULL) fprintf(FP, "%s\n", gPsCmd[PS_GRESTORE]);
   fprintf(FP, "\n");
}

void DrawBoxObj(win, XOff, YOff, ObjPtr)
   Window win;
   int XOff, YOff;
   struct ObjRec *ObjPtr;
{
   struct BoxRec *box_ptr=ObjPtr->detail.b;
   int fill, pen, pixel, ltx, lty, rbx, rby, width, dash, trans_pat;
   int real_x_off, real_y_off;
   XGCValues values;

   trans_pat = ObjPtr->trans_pat;
   pen = box_ptr->pen;
   fill = box_ptr->fill;
   width = box_ptr->width;
   dash = box_ptr->dash;
   pixel = colorPixels[ObjPtr->color];

   if ((fill == NONEPAT || (trans_pat && fill == BACKPAT)) &&
         (pen == NONEPAT || (trans_pat && pen == BACKPAT))) {
      return;
   }
   real_x_off = (zoomedIn ? XOff : (XOff>>zoomScale)<<zoomScale);
   real_y_off = (zoomedIn ? YOff : (YOff>>zoomScale)<<zoomScale);
   ltx = ZOOMED_SIZE(ObjPtr->obbox.ltx - real_x_off);
   lty = ZOOMED_SIZE(ObjPtr->obbox.lty - real_y_off);
   rbx = ZOOMED_SIZE(ObjPtr->obbox.rbx - real_x_off);
   rby = ZOOMED_SIZE(ObjPtr->obbox.rby - real_y_off);

   if (fill != NONEPAT) {
      values.foreground = GetDrawingBgPixel(fill, pixel);
      values.function = GXcopy;
      values.fill_style = (trans_pat ? FillStippled : FillOpaqueStippled);
      values.stipple = patPixmap[fill];
      XChangeGC(mainDisplay, drawGC,
            GCForeground | GCFunction | GCFillStyle | GCStipple, &values);
      if (ObjPtr->ctm != NULL) {
         XFillPolygon(mainDisplay, win, drawGC, ObjPtr->rotated_obbox, 5,
               Convex, CoordModeOrigin);
      } else {
         XFillRectangle(mainDisplay, win, drawGC, ltx, lty, rbx-ltx, rby-lty);
      }
   }

   if (pen != NONEPAT) {
      values.foreground = GetDrawingBgPixel(pen, pixel);
      values.function = GXcopy;
      values.fill_style = (trans_pat ? FillStippled : FillOpaqueStippled);
      values.stipple = patPixmap[pen];
      values.line_width = ZOOMED_SIZE(width);
#ifdef NO_THIN_LINE
      if (values.line_width < 1) values.line_width = 1;
#endif
      values.join_style = JoinMiter;
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
      if (ObjPtr->ctm != NULL) {
         XDrawLines(mainDisplay, win, drawGC, ObjPtr->rotated_obbox, 5,
               CoordModeOrigin);
      } else {
         XDrawRectangle(mainDisplay, win, drawGC, ltx, lty, rbx-ltx, rby-lty);
      }
   }
}

int CreateBoxObj(X1, Y1, X2, Y2, CreateAbsolute)
   int X1, Y1, X2, Y2, CreateAbsolute;
{
   struct BoxRec *box_ptr;
   struct ObjRec *obj_ptr;
   int width=0, w, ltx, lty, rbx, rby;

   if (X1 == X2 && Y1 == Y2) return FALSE;
   box_ptr = (struct BoxRec *)malloc(sizeof(struct BoxRec));
   if (box_ptr == NULL) FailAllocMessage();
   memset(box_ptr, 0, sizeof(struct BoxRec));
   if (mainDisplay != NULL) {
      box_ptr->fill = objFill;
      box_ptr->width = width = curWidthOfLine[lineWidth];
      UtilStrCpyN(box_ptr->width_spec, sizeof(box_ptr->width_spec),
            curWidthOfLineSpec[lineWidth]);
      box_ptr->pen = penPat;
      box_ptr->dash = curDash;
   }
   obj_ptr = (struct ObjRec *)malloc(sizeof(struct ObjRec));
   if (obj_ptr == NULL) FailAllocMessage();
   memset(obj_ptr, 0, sizeof(struct ObjRec));

   if (X1 < X2) {
      if (Y1 < Y2) {
         ltx = X1; lty = Y1; rbx = X2; rby = Y2;
      } else {
         ltx = X1; lty = Y2; rbx = X2; rby = Y1;
      }
   } else {
      if (Y1 < Y2) {
         ltx = X2; lty = Y1; rbx = X1; rby = Y2;
      } else {
         ltx = X2; lty = Y2; rbx = X1; rby = Y1;
      }
   }
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
   obj_ptr->type = OBJ_BOX;
   obj_ptr->color = colorIndex;
   if (mainDisplay != NULL) {
      UtilStrCpyN(obj_ptr->color_str, sizeof(obj_ptr->color_str),
            colorMenuItems[colorIndex]);
   }
   obj_ptr->id = objId++;
   obj_ptr->dirty = FALSE;
   obj_ptr->rotation = 0;
   obj_ptr->locked = FALSE;
   obj_ptr->detail.b = box_ptr;
   obj_ptr->fattr = obj_ptr->lattr = NULL;
   obj_ptr->ctm = NULL;
   obj_ptr->invisible = FALSE;
   obj_ptr->trans_pat = transPat;

   AddObj(NULL, topObj, obj_ptr);

   return TRUE;
}

static
void ContinueBox(OrigX, OrigY)
   int  OrigX, OrigY;
{
   int end_x, end_y, grid_x, grid_y, saved_x, saved_y;
   int done=FALSE, abort=FALSE;
   char buf[80], w_buf[80], h_buf[80], x_buf[80], y_buf[80];
   XEvent input, ev;
   XMotionEvent *motion_ev;

   SetXorDrawGC(xorColorPixels[colorIndex]);

   saved_x = grid_x = OrigX;
   saved_y = grid_y = OrigY; 
   PixelToMeasurementUnit(w_buf, 0);
   PixelToMeasurementUnit(h_buf, 0);
   PixelToMeasurementUnit(x_buf, ABS_X(grid_x));
   PixelToMeasurementUnit(y_buf, ABS_Y(grid_y));
   sprintf(buf, "w=%s\nh=%s\nx=%s\ny=%s", w_buf, h_buf, x_buf, y_buf);
   StartShowMeasureCursor(grid_x, grid_y, buf, TRUE);
   BeginIntervalRulers(grid_x, grid_y, grid_x, grid_y);
   if (!debugNoPointerGrab) {
      XGrabPointer(mainDisplay, drawWindow, FALSE,
            PointerMotionMask | ButtonReleaseMask,
            GrabModeAsync, GrabModeAsync, None, handCursor, CurrentTime);
   }
   while (!done) {
      XNextEvent(mainDisplay, &input);

      if (input.type == Expose || input.type == VisibilityNotify) {
         ExposeEventHandler(&input, TRUE);
         SetXorDrawGC(xorColorPixels[colorIndex]);
      } else if (input.type == ButtonRelease) {
         XUngrabPointer(mainDisplay, CurrentTime);
         MyBox(drawWindow, drawGC, OrigX, OrigY, saved_x, saved_y);
         EndIntervalRulers(grid_x, grid_y);
         PixelToMeasurementUnit(w_buf, ABS_SIZE(abs(saved_x-OrigX)));
         PixelToMeasurementUnit(h_buf, ABS_SIZE(abs(saved_y-OrigY)));
         PixelToMeasurementUnit(x_buf, ABS_X(saved_x));
         PixelToMeasurementUnit(y_buf, ABS_Y(saved_y));
         sprintf(buf, "w=%s\nh=%s\nx=%s\ny=%s", w_buf, h_buf, x_buf, y_buf);
         EndShowMeasureCursor(saved_x, saved_y, buf, TRUE);
         done = TRUE;
      } else if (input.type == MotionNotify) {
         motion_ev = &(input.xmotion);
         end_x = motion_ev->x;
         end_y = motion_ev->y;
         GridXY(end_x, end_y, &grid_x, &grid_y);
         if (motion_ev->state & (ShiftMask | ControlMask)) {
            int w, h, pos_w=TRUE, pos_h=TRUE;

            w = grid_x - OrigX;
            h = grid_y - OrigY;
            if (w < 0) {
               w = (-w);
               pos_w = FALSE;
            }
            if (h < 0) {
               h = (-h);
               pos_h = FALSE;
            }
            if (w > h) {
               grid_x = (pos_w ? (OrigX+h) : (OrigX-h));
            } else {
               grid_y = (pos_h ? (OrigY+w) : (OrigY-w));
            }
         }
         if (grid_x != saved_x || grid_y != saved_y) {
            PixelToMeasurementUnit(w_buf, ABS_SIZE(abs(saved_x-OrigX)));
            PixelToMeasurementUnit(h_buf, ABS_SIZE(abs(saved_y-OrigY)));
            PixelToMeasurementUnit(x_buf, ABS_X(saved_x));
            PixelToMeasurementUnit(y_buf, ABS_Y(saved_y));
            sprintf(buf, "w=%s\nh=%s\nx=%s\ny=%s", w_buf, h_buf, x_buf, y_buf);
            ShowMeasureCursor(saved_x, saved_y, buf, TRUE);
            MyBox(drawWindow, drawGC, OrigX, OrigY, saved_x, saved_y);
            saved_x = grid_x;
            saved_y = grid_y;
            MyBox(drawWindow, drawGC, OrigX, OrigY, saved_x, saved_y);
            PixelToMeasurementUnit(w_buf, ABS_SIZE(abs(saved_x-OrigX)));
            PixelToMeasurementUnit(h_buf, ABS_SIZE(abs(saved_y-OrigY)));
            PixelToMeasurementUnit(x_buf, ABS_X(saved_x));
            PixelToMeasurementUnit(y_buf, ABS_Y(saved_y));
            sprintf(buf, "w=%s\nh=%s\nx=%s\ny=%s", w_buf, h_buf, x_buf, y_buf);
            ShowMeasureCursor(saved_x, saved_y, buf, TRUE);
         }
         DrawIntervalRulers(OrigX, OrigY, grid_x, grid_y, NULL);
         while (XCheckMaskEvent(mainDisplay, PointerMotionMask, &ev)) ;
      } else if (input.type == KeyPress) {
         if (KeyPressEventIsEscape(&input.xkey)) {
            XUngrabPointer(mainDisplay, CurrentTime);
            MyBox(drawWindow, drawGC, OrigX, OrigY, saved_x, saved_y);
            EndIntervalRulers(grid_x, grid_y);
            PixelToMeasurementUnit(w_buf, ABS_SIZE(abs(saved_x-OrigX)));
            PixelToMeasurementUnit(h_buf, ABS_SIZE(abs(saved_y-OrigY)));
            PixelToMeasurementUnit(x_buf, ABS_X(saved_x));
            PixelToMeasurementUnit(y_buf, ABS_Y(saved_y));
            sprintf(buf, "w=%s\nh=%s\nx=%s\ny=%s", w_buf, h_buf, x_buf, y_buf);
            EndShowMeasureCursor(saved_x, saved_y, buf, TRUE);
            abort = TRUE;
            done = TRUE;
         }
      }
   }
   if (!abort && OrigX != grid_x && OrigY != grid_y) {
      CreateBoxObj(OrigX, OrigY, grid_x, grid_y, FALSE);
      RecordNewObjCmd();
      DrawBoxObj(drawWindow, drawOrigX, drawOrigY, topObj);
      boxDrawn = TRUE;
      SetFileModified(TRUE);
   }
   XSync(mainDisplay, False);
}

void DrawBox(input)
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
      ContinueBox(grid_x, grid_y);
   }
}

void MakeBoxObjFromBoundingBox()
{
   if (topSel == NULL) {
      MsgBox(TgLoadCachedString(CSTID_NO_OBJ_SELECTED), TOOL_NAME, INFO_MB);
      return;
   }
   if (curChoice == VERTEXMODE) SetCurChoice(NOTHING);

   HighLightReverse();
   CreateBoxObj(selObjLtX, selObjLtY, selObjRbX, selObjRbY, TRUE);
   SelectTopObj();
   RecordNewObjCmd();
   RedrawAnArea(botObj, selLtX-GRID_ABS_SIZE(1), selLtY-GRID_ABS_SIZE(1),
         selRbX+GRID_ABS_SIZE(1), selRbY+GRID_ABS_SIZE(1));
   HighLightForward();
   justDupped = FALSE;
   SetFileModified(TRUE);
}

void SaveBoxObj(FP, ObjPtr)
   FILE *FP;
   struct ObjRec *ObjPtr;
{
   struct BoxRec *box_ptr=ObjPtr->detail.b;

   if (fprintf(FP, "box('%s','',", colorMenuItems[ObjPtr->color]) == EOF) {
      writeFileFailed = TRUE;
   }
   if (fprintf(FP,
         "%1d,%1d,%1d,%1d,%1d,%1d,%1d,%1d,%1d,%1d,%1d,%1d,%1d,'%s',%1d,",
         ObjPtr->obbox.ltx, ObjPtr->obbox.lty, ObjPtr->obbox.rbx,
         ObjPtr->obbox.rby, box_ptr->fill, box_ptr->width, box_ptr->pen,
         ObjPtr->id, box_ptr->dash, ObjPtr->rotation, ObjPtr->locked,
         ObjPtr->ctm!=NULL, ObjPtr->invisible, box_ptr->width_spec,
         ObjPtr->trans_pat) == EOF) {
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

void ReadBoxObj(FP, Inbuf, ObjPtr)
   FILE *FP;
   char *Inbuf;
   struct ObjRec **ObjPtr;
{
   struct BoxRec *box_ptr;
   char color_str[40], bg_color_str[40], *s, width_spec[40];
   int ltx, lty, rbx, rby, fill, width, pen, dash, w, id=0, trans_pat=FALSE;
   int new_alloc, rotation, locked=FALSE, transformed=FALSE, invisible=FALSE;

   *ObjPtr = NULL;

   s = FindChar((int)'(', Inbuf);
   s = ParseStr(s, (int)',', color_str, sizeof(color_str));
   if (fileVersion >= 37) {
      s = ParseStr(s, (int)',', bg_color_str, sizeof(bg_color_str));
   }
   InitScan(s, "\t\n, ");

   dash = 0;
   rotation = 0;
   *width_spec = '\0';
   if (fileVersion <= 5) {
      if (GETINT("box", ltx,      "ltx") == INVALID ||
          GETINT("box", lty,      "lty") == INVALID ||
          GETINT("box", rbx,      "rbx") == INVALID ||
          GETINT("box", rby,      "rby") == INVALID ||
          GETINT("box", fill,     "fill") == INVALID ||
          GETINT("box", width,    "width") == INVALID ||
          GETINT("box", pen,      "pen") == INVALID)
      {
         return;
      }
      switch (width) {
      case 1: width = 3; break;
      case 2: width = 6; break;
      }
      id = objId++;
   } else if (fileVersion <= 7) {
      if (GETINT("box", ltx,      "ltx") == INVALID ||
          GETINT("box", lty,      "lty") == INVALID ||
          GETINT("box", rbx,      "rbx") == INVALID ||
          GETINT("box", rby,      "rby") == INVALID ||
          GETINT("box", fill,     "fill") == INVALID ||
          GETINT("box", width,    "width") == INVALID ||
          GETINT("box", pen,      "pen") == INVALID)
      {
         return;
      }
      id = objId++;
   } else if (fileVersion <= 8) {
      if (GETINT("box", ltx,      "ltx") == INVALID ||
          GETINT("box", lty,      "lty") == INVALID ||
          GETINT("box", rbx,      "rbx") == INVALID ||
          GETINT("box", rby,      "rby") == INVALID ||
          GETINT("box", fill,     "fill") == INVALID ||
          GETINT("box", width,    "width") == INVALID ||
          GETINT("box", pen,      "pen") == INVALID ||
          GETINT("box", id,       "id") == INVALID)
      {
         return;
      }
      if (id >= objId) objId = id+1;
   } else if (fileVersion <= 13) {
      if (GETINT("box", ltx,      "ltx") == INVALID ||
          GETINT("box", lty,      "lty") == INVALID ||
          GETINT("box", rbx,      "rbx") == INVALID ||
          GETINT("box", rby,      "rby") == INVALID ||
          GETINT("box", fill,     "fill") == INVALID ||
          GETINT("box", width,    "width") == INVALID ||
          GETINT("box", pen,      "pen") == INVALID ||
          GETINT("box", id,       "id") == INVALID ||
          GETINT("box", dash,     "dash") == INVALID)
      {
         return;
      }
      if (id >= objId) objId = id+1;
   } else if (fileVersion <= 25) {
      if (GETINT("box", ltx,      "ltx") == INVALID ||
          GETINT("box", lty,      "lty") == INVALID ||
          GETINT("box", rbx,      "rbx") == INVALID ||
          GETINT("box", rby,      "rby") == INVALID ||
          GETINT("box", fill,     "fill") == INVALID ||
          GETINT("box", width,    "width") == INVALID ||
          GETINT("box", pen,      "pen") == INVALID ||
          GETINT("box", id,       "id") == INVALID ||
          GETINT("box", dash,     "dash") == INVALID ||
          GETINT("box", rotation, "rotation") == INVALID)
      {
         return;
      }
      if (id >= objId) objId = id+1;
   } else if (fileVersion <= 32) {
      if (GETINT("box", ltx,      "ltx") == INVALID ||
          GETINT("box", lty,      "lty") == INVALID ||
          GETINT("box", rbx,      "rbx") == INVALID ||
          GETINT("box", rby,      "rby") == INVALID ||
          GETINT("box", fill,     "fill") == INVALID ||
          GETINT("box", width,    "width") == INVALID ||
          GETINT("box", pen,      "pen") == INVALID ||
          GETINT("box", id,       "id") == INVALID ||
          GETINT("box", dash,     "dash") == INVALID ||
          GETINT("box", rotation, "rotation") == INVALID ||
          GETINT("box", locked,   "locked") == INVALID)
      {
         return;
      }
      if (id >= objId) objId = id+1;
   } else if (fileVersion <= 34) {
      if (GETINT("box", ltx,         "ltx") == INVALID ||
          GETINT("box", lty,         "lty") == INVALID ||
          GETINT("box", rbx,         "rbx") == INVALID ||
          GETINT("box", rby,         "rby") == INVALID ||
          GETINT("box", fill,        "fill") == INVALID ||
          GETINT("box", width,       "width") == INVALID ||
          GETINT("box", pen,         "pen") == INVALID ||
          GETINT("box", id,          "id") == INVALID ||
          GETINT("box", dash,        "dash") == INVALID ||
          GETINT("box", rotation,    "rotation") == INVALID ||
          GETINT("box", locked,      "locked") == INVALID ||
          GETINT("box", transformed, "transformed") == INVALID ||
          GETINT("box", invisible,   "invisible") == INVALID ||
          GETSTR("box", width_spec,  "width_spec") == INVALID)
      {
         return;
      }
      if (id >= objId) objId = id+1;
      UtilRemoveQuotes(width_spec);
   } else {
      if (GETINT("box", ltx,         "ltx") == INVALID ||
          GETINT("box", lty,         "lty") == INVALID ||
          GETINT("box", rbx,         "rbx") == INVALID ||
          GETINT("box", rby,         "rby") == INVALID ||
          GETINT("box", fill,        "fill") == INVALID ||
          GETINT("box", width,       "width") == INVALID ||
          GETINT("box", pen,         "pen") == INVALID ||
          GETINT("box", id,          "id") == INVALID ||
          GETINT("box", dash,        "dash") == INVALID ||
          GETINT("box", rotation,    "rotation") == INVALID ||
          GETINT("box", locked,      "locked") == INVALID ||
          GETINT("box", transformed, "transformed") == INVALID ||
          GETINT("box", invisible,   "invisible") == INVALID ||
          GETSTR("box", width_spec,  "width_spec") == INVALID ||
          GETINT("box", trans_pat,   "trans_pat") == INVALID)
      {
         return;
      }
      if (id >= objId) objId = id+1;
      UtilRemoveQuotes(width_spec);
   }
   if (fileVersion <= 16 && width <= 6) width = origWidthOfLine[width];
   if (fileVersion <= 32) {
      sprintf(width_spec, "%1d", width);
   }
   fill = UpgradePenFill(fill);
   pen = UpgradePenFill(pen);

   *ObjPtr = (struct ObjRec *)malloc(sizeof(struct ObjRec));
   if (*ObjPtr == NULL) FailAllocMessage();
   memset(*ObjPtr, 0, sizeof(struct ObjRec));
   box_ptr = (struct BoxRec *)malloc(sizeof(struct BoxRec));
   if (box_ptr == NULL) FailAllocMessage();
   memset(box_ptr, 0, sizeof(struct BoxRec));

   if (ltx > rbx || lty > rby) {
      int tmp_ltx, tmp_lty, tmp_rbx, tmp_rby;

      if (!PRTGIF) {
         Msg(TgLoadCachedString(CSTID_BAD_BOX_BBOX_ADJUSTED));
      }
      CalcBBox(ltx, lty, rbx, rby, &tmp_ltx, &tmp_lty, &tmp_rbx, &tmp_rby);
      ltx = tmp_ltx; lty = tmp_lty; rbx = tmp_rbx; rby = tmp_rby;
   }

   (*ObjPtr)->trans_pat = trans_pat;
   box_ptr->fill = fill;
   box_ptr->width = width;
   UtilStrCpyN(box_ptr->width_spec, sizeof(box_ptr->width_spec), width_spec);
   box_ptr->pen = pen;
   box_ptr->dash = dash;
   (*ObjPtr)->x = ltx;
   (*ObjPtr)->y = lty;
   (*ObjPtr)->color = QuickFindColorIndex(*ObjPtr, color_str, &new_alloc, TRUE);
   UtilStrCpyN((*ObjPtr)->color_str, sizeof((*ObjPtr)->color_str), color_str);
   (*ObjPtr)->dirty = FALSE;
   (*ObjPtr)->id = id;
   (*ObjPtr)->rotation = rotation;
   (*ObjPtr)->locked = locked;
   (*ObjPtr)->type = OBJ_BOX;
   (*ObjPtr)->obbox.ltx = ltx;
   (*ObjPtr)->obbox.lty = lty;
   (*ObjPtr)->obbox.rbx = rbx;
   (*ObjPtr)->obbox.rby = rby;
   w = HALF_W(width);
   (*ObjPtr)->bbox.ltx = ltx - w;
   (*ObjPtr)->bbox.lty = lty - w;
   (*ObjPtr)->bbox.rbx = rbx + w;
   (*ObjPtr)->bbox.rby = rby + w;
   (*ObjPtr)->detail.b = box_ptr;
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
      if (GETINT("box", real_x,           "real_x") == INVALID ||
          GETINT("box", real_y,           "real_y") == INVALID ||
          GETINT("box", orig_obbox.ltx,   "orig_obbox.ltx") == INVALID ||
          GETINT("box", orig_obbox.lty,   "orig_obbox.lty") == INVALID ||
          GETINT("box", orig_obbox.rbx,   "orig_obbox.rbx") == INVALID ||
          GETINT("box", orig_obbox.rby,   "orig_obbox.rby") == INVALID ||
          GETDBL("box", ctm->m[CTM_SX],   "CTM_SX") == INVALID ||
          GETDBL("box", ctm->m[CTM_SIN],  "CTM_SIN") == INVALID ||
          GETDBL("box", ctm->m[CTM_MSIN], "CTM_MSIN") == INVALID ||
          GETDBL("box", ctm->m[CTM_SY],   "CTM_SY") == INVALID ||
          GETINT("box", ctm->t[CTM_TX],   "CTM_TX") == INVALID ||
          GETINT("box", ctm->t[CTM_TY],   "CTM_TY") == INVALID) {
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
}

void SetBoxPropMask(ObjPtr, plMask, plSkip, pProp)
   struct ObjRec *ObjPtr;
   long *plMask, *plSkip;
   struct PropertiesRec *pProp;
{
   struct BoxRec *box_ptr=ObjPtr->detail.b;

   SetCTMPropertyMask(ObjPtr->ctm, plMask, plSkip, pProp);

   SetIntPropertyMask(PROP_MASK_COLOR, ObjPtr->color,
         colorMenuItems[ObjPtr->color], plMask, plSkip, pProp);
   SetIntPropertyMask(PROP_MASK_WIDTH, box_ptr->width, box_ptr->width_spec,
         plMask, plSkip, pProp);

   SetIntPropertyMask(PROP_MASK_TRANSPAT, ObjPtr->trans_pat, NULL,
         plMask, plSkip, pProp);
   SetIntPropertyMask(PROP_MASK_FILL, box_ptr->fill, NULL,
         plMask, plSkip, pProp);
   SetIntPropertyMask(PROP_MASK_PEN, box_ptr->pen, NULL,
         plMask, plSkip, pProp);
   SetIntPropertyMask(PROP_MASK_DASH, box_ptr->dash, NULL,
         plMask, plSkip, pProp);
}

void FreeBoxObj(ObjPtr)
   struct ObjRec *ObjPtr;
{
   free(ObjPtr->detail.b);
   free(ObjPtr);
}
