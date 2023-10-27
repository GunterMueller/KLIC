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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/scroll.c,v 1.8 2011/05/16 16:21:59 william Exp $
 */

#define _INCLUDE_FROM_SCROLL_C_

#include "tgifdefs.h"

#include "choice.e"
#include "cursor.e"
#include "dialog.e"
#include "drawing.e"
#include "dup.e"
#include "exec.e"
#include "grid.e"
#include "mainloop.e"
#include "menu.e"
#include "miniline.e"
#include "msg.e"
#include "obj.e"
#include "page.e"
#include "raster.e"
#include "rect.e"
#include "ruler.e"
#include "scroll.e"
#include "setup.e"
#include "strtbl.e"
#include "text.e"
#include "util.e"

#include "xbm/scrl_up.xbm"

#define FAKE_CM 80

int autoPan=TRUE;

int scrollingCanvas=INVALID;
int smoothScrollingCanvas=JUMP_SCROLLING;

int resetOriginOnAdvancePage=FALSE;

static int scrollAreaH=0, scrollAreaW=0;

int TgAnyButtonDown(dpy, win)
   Display *dpy;
   Window win;
{
   Window root_win=None, child_win=None;
   unsigned int status;
   int parent_root_x, parent_root_y, x=0, y=0;

   XQueryPointer(dpy, win, &root_win, &child_win,
         &parent_root_x, &parent_root_y, &x, &y, &status);
   return ((status & BUTTONSMASK) != 0);
}

int TgPressButtonLoop(dpy, win, pbbox, psbci)
   Display *dpy;
   Window win;
   struct BBRec *pbbox;
   ScrollBtnCallbackInfo *psbci;
   /* returns TRUE if need to scroll one more time */
{
   time_t tv_usec=(time_t)0;
   int done=FALSE, need_to_scroll_once=TRUE, initial_timeout=TRUE;

   if (pbbox != NULL) {
      TgDrawThreeDButton(dpy, win, textMenuGC, pbbox, TGBS_LOWRED, 2, FALSE);
   }
   if (!TgAnyButtonDown(dpy, win)) {
      if (pbbox != NULL) {
         TgDrawThreeDButton(dpy, win, textMenuGC, pbbox, TGBS_RAISED, 2, FALSE);
      }
      return need_to_scroll_once;
   }
   tv_usec = (psbci->ms*1000);

   XGrabPointer(dpy, win, False,
         ButtonReleaseMask, GrabModeAsync,
         GrabModeAsync, None, handCursor, CurrentTime);

   do {
      struct timeval timeout;
      fd_set fdset;
      int select_width=XConnectionNumber(dpy)+1, status=0;

      timeout.tv_sec = 0;
      timeout.tv_usec = (initial_timeout ? 600000 : tv_usec);
      FD_ZERO(&fdset);
      FD_SET(select_width-1, &fdset);
#ifdef __hpux
      status = select(select_width, (int*)&fdset, NULL, NULL, &timeout);
#else /* !__hpux */
      status = select(select_width, &fdset, NULL, NULL, &timeout);
#endif /* __hpux */
      initial_timeout = FALSE;

      if (status < 0) {
         if (errno == EINTR) {
            /* interrupted by a system call, do it again */
            if (TgAnyButtonDown(dpy, win)) {
               need_to_scroll_once = FALSE;
               if ((psbci->pf_scroll_btn_callback)(psbci->pv_userdata)) {
                  XUngrabPointer(dpy, CurrentTime);
                  return TRUE;
               }
            }
         } else {
            XUngrabPointer(dpy, CurrentTime);
            sprintf(gszMsgBox, TgLoadString(STID_FUNC_SELECT_SYS_CALL_FAILED),
                  "TgPressButtonLoop()");
            MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
            break;
         }
      } else if (status == 0 && TgAnyButtonDown(dpy, win)) {
         need_to_scroll_once = FALSE;
         if ((psbci->pf_scroll_btn_callback)(psbci->pv_userdata)) {
            XUngrabPointer(dpy, CurrentTime);
            return TRUE;
         }
      } else {
         XUngrabPointer(dpy, CurrentTime);
         XFlush(dpy);
         XSync(dpy, False);
         if (pbbox != NULL) {
            TgDrawThreeDButton(dpy, win, textMenuGC, pbbox, TGBS_RAISED, 2,
                  FALSE);
         }
         done = TRUE;
         return need_to_scroll_once;
      }
   } while (!done);

   return need_to_scroll_once;
}

int TgGetScrollHit(x, y, orientation, scroll_area_w, scroll_area_h,
      start_frac, length, total, pn_btn_offset)
   int x, y, scroll_area_w, scroll_area_h, length, total, *pn_btn_offset;
   double start_frac;
{
   int block_start=0, block_size=0, min_block_size=1+(windowPadding<<1);
   int block_area=((orientation==VERT_SCROLLBAR)?scroll_area_h:scroll_area_w);
   double frac=(double)0;

   if (pn_btn_offset != NULL) *pn_btn_offset = 0;
   if (orientation == VERT_SCROLLBAR) {
      y -= scrollBarW;
   } else {
      x -= scrollBarW;
   }
   block_area -= (scrollBarW<<1);
   if (total == 0) {
      frac = (double)1.0;
   } else {
      frac = (double)((double)length / (double)total);
   }
   if (frac > 1.0) frac = 1.0;
   block_start = ((int)(block_area * start_frac));
   if (start_frac + frac >= 1.0) {
      block_size = block_area - block_start;
   } else {
      block_size = (int)(block_area * frac);
   }
   if (block_size < min_block_size) block_size = min_block_size;
   if (orientation == VERT_SCROLLBAR) {
      if (block_start > scroll_area_h-min_block_size) {
         block_start = scroll_area_h-min_block_size;
      }
   } else {
      if (block_start > scroll_area_w-min_block_size) {
         block_start = scroll_area_w-min_block_size;
      }
   }
   if (orientation == VERT_SCROLLBAR) {
      if (y < block_start) {
         return (-1);
      } else if (y < block_start+block_size) {
         if (pn_btn_offset != NULL) *pn_btn_offset = block_start-y;
         return 0;
      }
   } else {
      if (x < block_start) {
         return (-1);
      } else if (x < block_start+block_size) {
         if (pn_btn_offset != NULL) *pn_btn_offset = block_start-x;
         return 0;
      }
   }
   return 1;
}

void TgDrawScrollBar(dpy, win, orientation, x_off, y_off, scroll_area_w,
      scroll_area_h, start_frac, length, total)
   Display *dpy;
   Window win;
   int x_off, y_off, scroll_area_w, scroll_area_h, length, total;
   double start_frac;
{
   int block_start=0, block_size=0, min_block_size=1+(windowPadding<<1);
   int block_area=((orientation==VERT_SCROLLBAR)?scroll_area_h:scroll_area_w);
   double frac=(double)0;
   XGCValues values;

   if (threeDLook) {
      struct BBRec bbox;

      if (orientation == VERT_SCROLLBAR) {
         values.foreground = myFgPixel;
         values.background = myLtGryPixel;
         values.fill_style = FillOpaqueStippled;
         values.stipple = scrlBitmap[SCRL_UP];
         values.ts_x_origin = x_off+(windowPadding<<1);
         values.ts_y_origin = y_off+1+(windowPadding<<1);
         XChangeGC(mainDisplay, textMenuGC,
               GCForeground | GCBackground | GCFillStyle | GCStipple |
               GCTileStipXOrigin | GCTileStipYOrigin, &values);
         XFillRectangle(dpy, win, textMenuGC, values.ts_x_origin,
               values.ts_y_origin, scrl_up_width, scrl_up_height);
         values.stipple = scrlBitmap[SCRL_DN];
         values.ts_y_origin =
               y_off+scroll_area_h-scrollBarW+1+(windowPadding<<1);
         XChangeGC(mainDisplay, textMenuGC,
               GCStipple | GCTileStipYOrigin, &values);
         XFillRectangle(dpy, win, textMenuGC, values.ts_x_origin,
               values.ts_y_origin, scrl_up_width, scrl_up_height);
         values.fill_style = FillSolid;
         values.ts_x_origin = 0;
         values.ts_y_origin = 0;
         XChangeGC(mainDisplay, textMenuGC,
               GCFillStyle | GCTileStipXOrigin | GCTileStipYOrigin, &values);
         SetBBRec(&bbox, x_off, y_off, x_off+scrollBarW, y_off+scrollBarW);
         TgDrawThreeDButton(dpy, win, textMenuGC, &bbox, TGBS_RAISED, 2, FALSE);
         SetBBRec(&bbox, x_off, y_off+scroll_area_h-scrollBarW,
               x_off+scrollBarW, y_off+scroll_area_h);
         TgDrawThreeDButton(dpy, win, textMenuGC, &bbox, TGBS_RAISED, 2, FALSE);
      } else {
         values.foreground = myFgPixel;
         values.background = myLtGryPixel;
         values.fill_style = FillOpaqueStippled;
         values.stipple = scrlBitmap[SCRL_LF];
         values.ts_x_origin = x_off+(windowPadding<<1);
         values.ts_y_origin = y_off+(windowPadding<<1);
         XChangeGC(mainDisplay, textMenuGC,
               GCForeground | GCBackground | GCFillStyle | GCStipple |
               GCTileStipXOrigin | GCTileStipYOrigin, &values);
         XFillRectangle(dpy, win, textMenuGC, values.ts_x_origin,
               values.ts_y_origin, scrl_up_width, scrl_up_height);
         values.stipple = scrlBitmap[SCRL_RT];
         values.ts_x_origin = x_off+scroll_area_w-scrollBarW+(windowPadding<<1);
         XChangeGC(mainDisplay, textMenuGC,
               GCStipple | GCTileStipXOrigin, &values);
         XFillRectangle(dpy, win, textMenuGC, values.ts_x_origin,
               values.ts_y_origin, scrl_up_width, scrl_up_height);
         values.fill_style = FillSolid;
         values.ts_x_origin = 0;
         values.ts_y_origin = 0;
         XChangeGC(mainDisplay, textMenuGC,
               GCFillStyle | GCTileStipXOrigin | GCTileStipYOrigin, &values);
         SetBBRec(&bbox, x_off, y_off, x_off+scrollBarW, y_off+scrollBarW);
         TgDrawThreeDButton(dpy, win, textMenuGC, &bbox, TGBS_RAISED, 2, FALSE);
         SetBBRec(&bbox, x_off+scroll_area_w-scrollBarW, y_off,
               x_off+scroll_area_w, y_off+scrollBarW);
         TgDrawThreeDButton(dpy, win, textMenuGC, &bbox, TGBS_RAISED, 2, FALSE);
      }
      block_area -= (scrollBarW<<1);
      if (total == 0) {
         frac = (double)1.0;
      } else {
         frac = (double)((double)length / (double)total);
      }
      if (frac > 1.0) frac = 1.0;
      block_start = scrollBarW + ((int)(block_area * start_frac));
      if (start_frac + frac >= 1.0) {
         block_size = block_area - block_start + scrollBarW;
      } else {
         block_size = (int)(block_area * frac);
      }
      if (block_size < min_block_size) block_size = min_block_size;
      if (orientation == VERT_SCROLLBAR) {
         if (block_start > scroll_area_h-scrollBarW-min_block_size) {
            block_start = scroll_area_h-scrollBarW-min_block_size;
         }
      } else {
         if (block_start > scroll_area_w-scrollBarW-min_block_size) {
            block_start = scroll_area_w-scrollBarW-min_block_size;
         }
      }
      values.foreground = myLtGryPixel;
      values.background = myBgPixel;
      values.fill_style = FillOpaqueStippled;
      values.stipple = patPixmap[SCROLLPAT];
      XChangeGC(dpy, patGC,
            GCForeground | GCBackground | GCFillStyle | GCStipple, &values);
      if (orientation == VERT_SCROLLBAR) {
         XFillRectangle(dpy, win, patGC, x_off, y_off+scrollBarW, scroll_area_w,
               scroll_area_h-(scrollBarW<<1));
      } else {
         XFillRectangle(dpy, win, patGC, x_off+scrollBarW, y_off,
               scroll_area_w-(scrollBarW<<1), scroll_area_h);
      }
      values.foreground = myLtGryPixel;
      values.background = myLtGryPixel;
      values.function = GXcopy;
      values.fill_style = FillSolid;
      XChangeGC(dpy, patGC,
            GCForeground | GCBackground | GCFunction | GCFillStyle, &values);
      if (orientation == VERT_SCROLLBAR) {
         XFillRectangle(dpy, win, patGC, x_off, y_off+block_start,
               scroll_area_w, block_size);
         SetBBRec(&bbox, x_off, y_off+block_start, x_off+scroll_area_w,
               y_off+block_start+block_size);
      } else {
         XFillRectangle(dpy, win, patGC, x_off+block_start, y_off, block_size,
               scroll_area_h);
         SetBBRec(&bbox, x_off+block_start, y_off, x_off+block_start+block_size,
               y_off+scroll_area_h);
      }
      TgDrawThreeDButton(dpy, win, textMenuGC, &bbox, TGBS_RAISED, 2, FALSE);
   } else {
      if (total == 0) {
         frac = (double)1.0;
      } else {
         frac = (double)((double)length / (double)total);
      }
      if (frac > 1.0) frac = 1.0;
      block_start = (int)(block_area * start_frac);
      if (start_frac + frac >= 1.0) {
         block_size = block_area - block_start;
      } else {
         block_size = (int)(block_area * frac);
      }
      if (block_size <= 0) block_size = 1;

      values.foreground = myBgPixel;
      values.background = myFgPixel;
      values.function = GXcopy;
      values.fill_style = FillSolid;
      XChangeGC(dpy, patGC,
            GCForeground | GCBackground | GCFunction | GCFillStyle, &values);
      XFillRectangle(dpy, win, patGC, x_off, y_off, scroll_area_w,
            scroll_area_h);

      values.foreground = myFgPixel;
      values.background = myBgPixel;
      values.fill_style = FillOpaqueStippled;
      values.stipple = patPixmap[SCROLLPAT];
      XChangeGC(dpy, patGC,
            GCForeground | GCBackground | GCFillStyle | GCStipple, &values);
      if (orientation == VERT_SCROLLBAR) {
         XFillRectangle(dpy, win, patGC, x_off, y_off+block_start,
               scroll_area_w, block_size);
      } else {
         XFillRectangle(dpy, win, patGC, x_off+block_start, y_off, block_size,
               scroll_area_h);
      }
   }
}

void RedrawVScrollWindow()
{
   double start_frac=(double)0;
   int total=max(paperHeight,(drawOrigY+drawWinH));
   XEvent ev;

   XSync(mainDisplay, False);
   while (XCheckWindowEvent(mainDisplay, vSBarWindow, ExposureMask, &ev)) ;

   if (total == 0) {
      start_frac = (double)1.0;
   } else {
      start_frac = (double)((double)drawOrigY / (double)total);
   }
   if (start_frac > 1.0) start_frac = 1.0;
   TgDrawScrollBar(mainDisplay, vSBarWindow, VERT_SCROLLBAR,
         0, 0, scrollBarW, scrollAreaH, start_frac, drawWinH, total);
}

void RedrawHScrollWindow()
{
   double start_frac=(double)0;
   int total=max(paperWidth,(drawOrigX+drawWinW));
   XEvent ev;

   XSync(mainDisplay, False);
   while (XCheckWindowEvent(mainDisplay, hSBarWindow, ExposureMask, &ev)) ;

   if (total == 0) {
      start_frac = (double)1.0;
   } else {
      start_frac = (double)((double)drawOrigX / (double)total);
   }
   if (start_frac > 1.0) start_frac = 1.0;
   TgDrawScrollBar(mainDisplay, hSBarWindow, HORI_SCROLLBAR,
         0, 0, scrollAreaW, scrollBarW, start_frac, drawWinW, total);
}

void RedrawScrollBars()
{
   if (vSBarWindow != None) RedrawVScrollWindow();
   if (hSBarWindow != None) RedrawHScrollWindow();
}

void GetMaxScrollOrigin(pn_orig_x, pn_orig_y)
   int *pn_orig_x, *pn_orig_y;
{
   if (pn_orig_y != NULL) {
      if (paperHeight <= drawWinH) {
         *pn_orig_y = 0;
      } else {
         switch (gridSystem) {
         case ENGLISH_GRID:
            if ((paperHeight-drawWinH) % ABS_SIZE(HALF_INCH) == 0) {
               *pn_orig_y = paperHeight-drawWinH;
            } else {
               *pn_orig_y = max(0, ((int)((paperHeight-drawWinH)/
                     ABS_SIZE(HALF_INCH)) + 1) * ABS_SIZE(HALF_INCH));
            }
            break;
         case METRIC_GRID:
            if (zoomedIn && zoomScale > 1) {
               if ((paperHeight-drawWinH) % ABS_SIZE(FAKE_CM) == 0) {
                  *pn_orig_y = paperHeight-drawWinH;
               } else {
                  *pn_orig_y = max(0, ((int)((paperHeight-drawWinH)/
                        ABS_SIZE(FAKE_CM)) + 1) * ABS_SIZE(FAKE_CM));
               }
            } else {
               if ((paperHeight-drawWinH) % ABS_SIZE(ONE_CM) == 0) {
                  *pn_orig_y = paperHeight-drawWinH;
               } else {
                  *pn_orig_y = max(0, ((int)((paperHeight-drawWinH)/
                        ABS_SIZE(ONE_CM)) + 1) * ABS_SIZE(ONE_CM));
               }
            }
            break;
         }
      }
      *pn_orig_y = ABS_SIZE(ZOOMED_SIZE(*pn_orig_y));
   }
   if (pn_orig_x != NULL) {
      if (paperWidth <= drawWinW) {
          *pn_orig_x = 0;
      } else {
         switch (gridSystem) {
         case ENGLISH_GRID:
            if ((paperWidth-drawWinW) % ABS_SIZE(HALF_INCH) == 0) {
               *pn_orig_x = paperWidth-drawWinW;
            } else {
               *pn_orig_x = max(0, ((int)((paperWidth-drawWinW)/
                     ABS_SIZE(HALF_INCH)) + 1) * ABS_SIZE(HALF_INCH));
            }
            break;
         case METRIC_GRID:
            if (zoomedIn && zoomScale > 1) {
               if ((paperWidth-drawWinW) % ABS_SIZE(FAKE_CM) == 0) {
                  *pn_orig_x = paperWidth-drawWinW;
               } else {
                  *pn_orig_x = max(0, ((int)((paperWidth-drawWinW)/
                        ABS_SIZE(FAKE_CM)) + 1) * ABS_SIZE(FAKE_CM));
               }
            } else {
               if ((paperWidth-drawWinW) % ABS_SIZE(ONE_CM) == 0) {
                  *pn_orig_x = paperWidth-drawWinW;
               } else {
                  *pn_orig_x = max(0, ((int)((paperWidth-drawWinW)/
                        ABS_SIZE(ONE_CM)) + 1) * ABS_SIZE(ONE_CM));
               }
            }
            break;
         }
      }
      *pn_orig_x = ABS_SIZE(ZOOMED_SIZE(*pn_orig_x));
   }
}

void ScrollTo(XOff, YOff)
   int XOff, YOff;
{
   int h_adjust=0, v_adjust=0;
   static int first_auto_pan_msg=TRUE;

   if (!autoPan || (XOff >= 0 && ABS_SIZE(XOff) < drawWinW &&
         YOff >= 0 && ABS_SIZE(YOff) < drawWinH)) {
      return;
   }
   if (XOff < 0) {
      if (ABS_SIZE(-XOff) > drawOrigX) {
         h_adjust = (-ZOOMED_SIZE(drawOrigX));
      } else {
         switch (gridSystem) {
         case ENGLISH_GRID:
            h_adjust = ((-XOff) % HALF_INCH == 0) ? (XOff) :
                  (((int)((-XOff)/HALF_INCH))+1) * (-HALF_INCH);
            break;
         case METRIC_GRID:
            if (zoomedIn && zoomScale > 1) {
               h_adjust = ((-XOff) % FAKE_CM == 0) ? (XOff) :
                     (((int)((-XOff)/FAKE_CM))+1) * (-FAKE_CM);
            } else {
               h_adjust = ((-XOff) % ONE_CM == 0) ? (XOff) :
                     (((int)((-XOff)/ONE_CM))+1) * (-ONE_CM);
            }
            break;
         }
      }
   } else if (ABS_SIZE(XOff) >= drawWinW) {
      switch (gridSystem) {
      case ENGLISH_GRID:
         h_adjust = (((int)((XOff-ZOOMED_SIZE(drawWinW))/HALF_INCH))+1) *
               HALF_INCH;
         break;
      case METRIC_GRID:
         if (zoomedIn && zoomScale > 1) {
            h_adjust = (((int)((XOff-ZOOMED_SIZE(drawWinW))/FAKE_CM))+1) *
                  FAKE_CM;
         } else {
            h_adjust = (((int)((XOff-ZOOMED_SIZE(drawWinW))/ONE_CM))+1) *
                  ONE_CM;
         }
         break;
      }
   }
   if (YOff < 0) {
      if (ABS_SIZE(-YOff) > drawOrigY) {
         v_adjust = (-ZOOMED_SIZE(drawOrigY));
      } else {
         switch (gridSystem) {
         case ENGLISH_GRID:
            v_adjust = ((-YOff) % HALF_INCH == 0) ? (YOff) :
                  (((int)((-YOff)/HALF_INCH))+1) * (-HALF_INCH);
            break;
         case METRIC_GRID:
            if (zoomedIn && zoomScale > 1) {
               v_adjust = ((-YOff) % FAKE_CM == 0) ? (YOff) :
                     (((int)((-YOff)/FAKE_CM))+1) * (-FAKE_CM);
            } else {
               v_adjust = ((-YOff) % ONE_CM == 0) ? (YOff) :
                     (((int)((-YOff)/ONE_CM))+1) * (-ONE_CM);
            }
            break;
         }
      }
   } else if (ABS_SIZE(YOff+(textCursorH>>1)) >= drawWinH) {
      switch (gridSystem) {
      case ENGLISH_GRID:
         v_adjust = (((int)((YOff+(textCursorH>>1) -
               ZOOMED_SIZE(drawWinH))/HALF_INCH))+1) * HALF_INCH;
         break;
      case METRIC_GRID:
         if (zoomedIn && zoomScale > 1) {
            v_adjust = (((int)((YOff+(textCursorH>>1) -
                  ZOOMED_SIZE(drawWinH))/FAKE_CM))+1) * FAKE_CM;
         } else {
            v_adjust = (((int)((YOff+(textCursorH>>1) -
                  ZOOMED_SIZE(drawWinH))/ONE_CM))+1) * ONE_CM;
         }
         break;
      }
   }
   if (h_adjust == 0 && v_adjust == 0) return;

   if (first_auto_pan_msg) {
      first_auto_pan_msg = FALSE;
      sprintf(gszMsgBox, TgLoadString(STID_TURN_OFF_AUTO_PAN_SET_XDEF),
            TOOL_NAME, "AutoPanInEditText");
      Msg(gszMsgBox);
   }
   drawOrigX += ABS_SIZE(h_adjust);
   drawOrigY += ABS_SIZE(v_adjust);
   RedrawScrollBars();
   UpdDrawWinBBox();
   AdjSplineVs();
   AdjustCurText((-h_adjust), (-v_adjust));
   RedrawRulers();
   ClearAndRedrawDrawWindow();
}

void ScrollUp(button_ev)
   XButtonEvent *button_ev;
{
   int adjustment=0;

   if (drawOrigY != 0) {
      switch (gridSystem) {
      case ENGLISH_GRID:
         if (button_ev!=NULL &&
               (button_ev->state & (ShiftMask|ControlMask))) {
            adjustment = ((int)(ZOOMED_SIZE(drawWinH)/HALF_INCH))*HALF_INCH;
            if (drawOrigY-ABS_SIZE(adjustment) < 0) {
               adjustment = ZOOMED_SIZE(drawOrigY);
            }
         } else {
            adjustment = HALF_INCH;
         }
         break;
      case METRIC_GRID:
         if (zoomedIn && zoomScale > 1) {
            if (button_ev!=NULL &&
                  (button_ev->state & (ShiftMask|ControlMask))) {
               adjustment = ((int)(ZOOMED_SIZE(drawWinH)/FAKE_CM))*FAKE_CM;
               if (drawOrigY-ABS_SIZE(adjustment) < 0)
                  adjustment = ZOOMED_SIZE(drawOrigY);
            } else {
               adjustment = FAKE_CM;
            }
         } else {
            if (button_ev!=NULL &&
                  (button_ev->state & (ShiftMask|ControlMask))) {
               adjustment = ((int)(ZOOMED_SIZE(drawWinH)/ONE_CM))*ONE_CM;
               if (drawOrigY-ABS_SIZE(adjustment) < 0)
                  adjustment = ZOOMED_SIZE(drawOrigY);
            } else {
               adjustment = ONE_CM;
            }
         }
         break;
      }
      if (drawOrigY >= ABS_SIZE(adjustment)) {
         drawOrigY -= ABS_SIZE(adjustment);
      } else {
         adjustment = ZOOMED_SIZE(drawOrigY);
         drawOrigY -= ABS_SIZE(adjustment);
         if (drawOrigY < 0) drawOrigY = 0;
      }
      RedrawVScrollWindow();
      UpdDrawWinBBox();
      AdjSplineVs();
      AdjustCurText(0, adjustment);
      RedrawRulers();
      ClearAndRedrawDrawWindow();
   }
}

void ForceScrollDown(ScrollAFullWindow)
   int ScrollAFullWindow;
{
   int adjustment=0;

   switch (gridSystem) {
   case ENGLISH_GRID:
      if (ScrollAFullWindow) {
         adjustment = ((int)(ZOOMED_SIZE(drawWinH)/HALF_INCH))*HALF_INCH;
      } else {
         adjustment = HALF_INCH;
      }
      break;
   case METRIC_GRID:
      if (zoomedIn && zoomScale > 1) {
         if (ScrollAFullWindow) {
            adjustment = ((int)(ZOOMED_SIZE(drawWinH)/FAKE_CM))*FAKE_CM;
         } else {
            adjustment = FAKE_CM;
         }
      } else {
         if (ScrollAFullWindow) {
            adjustment = ((int)(ZOOMED_SIZE(drawWinH)/ONE_CM))*ONE_CM;
         } else {
            adjustment = ONE_CM;
         }
      }
      break;
   }
   drawOrigY += ABS_SIZE(adjustment);
   RedrawVScrollWindow();
   UpdDrawWinBBox();
   AdjSplineVs();
   AdjustCurText(0, -adjustment);
   RedrawRulers();
   ClearAndRedrawDrawWindow();
}

void ScrollDown(button_ev)
   XButtonEvent *button_ev;
{
   if (paperHeight <= drawWinH) return;

   if (drawOrigY+drawWinH < paperHeight) {
      ForceScrollDown(button_ev != NULL &&
            (button_ev->state & (ShiftMask|ControlMask)));
   }
}

typedef struct tagSBarInfo {
   int scroll_page, scroll_dir;
} SBarInfo;

static
int ScrollBtnCallback(pv_userdata)
   void *pv_userdata;
   /* returns TRUE to cancel scrolling */
{
   SBarInfo *psbi=((SBarInfo*)(pv_userdata));
   XButtonEvent btn_ev;

   btn_ev.state = (psbi->scroll_page ? ShiftMask : 0);
   switch (psbi->scroll_dir) {
   case SCRL_UP: ScrollUp(&btn_ev); break;
   case SCRL_DN: ScrollDown(&btn_ev); break;
   case SCRL_LF: ScrollLeft(&btn_ev); break;
   case SCRL_RT: ScrollRight(&btn_ev); break;
   }
   return FALSE;
}

static
int DoSBarBtnScroll(dpy, win, scroll_page, scroll_dir, pbbox)
   Display *dpy;
   Window win;
   int scroll_page, scroll_dir;
   struct BBRec *pbbox;
   /* returns TRUE if the done scrolling */
{
   ScrollBtnCallbackInfo sbci;
   SBarInfo sbi;

   memset(&sbi, 0, sizeof(SBarInfo));
   sbi.scroll_page = scroll_page;
   sbi.scroll_dir = scroll_dir;

   memset(&sbci, 0, sizeof(ScrollBtnCallbackInfo));
   sbci.ms = (scroll_page ? 300 : 200);
   sbci.pv_userdata = ((void*)(&sbi));
   sbci.pf_scroll_btn_callback = ScrollBtnCallback;
   if (TgPressButtonLoop(dpy, win, pbbox, &sbci)) {
      XButtonEvent btn_ev;

      btn_ev.state = (scroll_page ? ShiftMask : 0);
      switch (scroll_dir) {
      case SCRL_UP: ScrollUp(&btn_ev); break;
      case SCRL_DN: ScrollDown(&btn_ev); break;
      case SCRL_LF: ScrollLeft(&btn_ev); break;
      case SCRL_RT: ScrollRight(&btn_ev); break;
      }
   }
   return FALSE;
}

static
void DragToInVSBar(lty, block_start, block_h)
   int lty, block_start, block_h;
{
   double start_frac=(double)0.0;
   int saved_y=drawOrigY, adjustment=0, scroll_all_the_way=FALSE;

   if (threeDLook) {
      start_frac = (double)(((double)(lty-scrollBarW)) /
            ((double)(scrollAreaH-(scrollBarW<<1))));
      if (lty+block_h >= scrollAreaH-scrollBarW) {
         scroll_all_the_way = TRUE;
      }
   } else {
      start_frac = (double)(((double)block_start)/((double)scrollAreaH));
      if (block_start+block_h >= scrollAreaH) {
         scroll_all_the_way = TRUE;
      }
   }
   if (start_frac < 0.0) start_frac = 0.0;
   if (start_frac > 1.0) start_frac = 1.0;
   if (scroll_all_the_way) {
      GetMaxScrollOrigin(NULL, &drawOrigY);
   } else {
      double dv=(double)0;

      switch (gridSystem) {
      case ENGLISH_GRID:
         dv = ((double)paperHeight) * start_frac;
         break;
      case METRIC_GRID:
         if (zoomedIn && zoomScale > 1) {
            dv = ((double)paperHeight) * start_frac;
         } else {
            dv = ((double)paperHeight) * start_frac;
         }
         break;
      }
      drawOrigY = round(dv);
      drawOrigY = ABS_SIZE(ZOOMED_SIZE(drawOrigY));
   }
   adjustment = saved_y - drawOrigY;
   if (adjustment != 0) {
      RedrawVScrollWindow();
      UpdDrawWinBBox();
      AdjSplineVs();
      AdjustCurText(0, ZOOMED_SIZE(adjustment));
      RedrawRulers();
      ClearAndRedrawDrawWindow();
   } else {
      RedrawVScrollWindow();
   }
}

static
void DoDragInVSBar(btn_y, btn_offset)
   int btn_y, btn_offset;
{
   double frac=(double)0.0, start_frac=(double)0.0;
   int block_h=0, block_start=0, continuous_drag=FALSE;
   int done=FALSE, lty=0, min_block_size=1+(windowPadding<<1);
   int total=max(paperHeight,(drawOrigY+drawWinH));
   XEvent ev;

   scrollingCanvas = SCRL_UP;
   if (smoothScrollingCanvas == SMOOTH_SCROLLING) {
      BeginExecAnimate();
      if (!TgAnyButtonDown(mainDisplay, vSBarWindow)) {
         EndExecAnimate();
         ClearAndRedrawDrawWindow();
         return;
      }
      continuous_drag = execAnimating;
   }
   block_start = btn_y+btn_offset;
   frac = (double)((double)drawWinH / (double)(paperHeight));
   if (threeDLook) {
      start_frac = (double)(((double)(block_start-scrollBarW)) /
            ((double)(scrollAreaH-(scrollBarW<<1))));
      block_h = (frac >= 1.0) ? (scrollAreaH-(scrollBarW<<1)) :
            (int)(((double)(scrollAreaH-(scrollBarW<<1))) * frac);
      if (block_h < min_block_size) block_h = min_block_size;
      lty = (start_frac + frac >= 1.0) ? scrollAreaH-scrollBarW-block_h :
            block_start;
      start_frac = (double)(((double)(lty-scrollBarW)) /
            ((double)(scrollAreaH-(scrollBarW<<1))));
   } else {
      start_frac = (double)(((double)block_start)/((double)scrollAreaH));
      block_h = (frac >= 1.0) ? scrollAreaH :
            (int)(((double)scrollAreaH) * frac);
      if (block_h < 1) block_h = 1;
      lty = (start_frac + frac >= 1.0) ? scrollAreaH-block_h : block_start;
      start_frac = (double)(((double)lty)/((double)scrollAreaH));
   }
   TgDrawScrollBar(mainDisplay, vSBarWindow, VERT_SCROLLBAR, 0, 0, scrollBarW,
         scrollAreaH, start_frac, drawWinH, total);
   XGrabPointer(mainDisplay, vSBarWindow, False,
         PointerMotionMask | ButtonReleaseMask, GrabModeAsync,
         GrabModeAsync, None, handCursor, CurrentTime);

   while (!done) {
      XNextEvent(mainDisplay, &ev);

      if (ev.type == Expose || ev.type == VisibilityNotify) {
         ExposeEventHandler(&ev, TRUE);
      } else if (ev.type == ButtonRelease) {
         XUngrabPointer(mainDisplay, CurrentTime);
         done = TRUE;
      } else if (ev.type == MotionNotify) {
         int new_y=0;

         block_start = ev.xmotion.y;

         if (threeDLook) {
            new_y = ev.xmotion.y+btn_offset;
            if (new_y < scrollBarW) {
               new_y = scrollBarW;
            } else if (new_y+block_h >= scrollAreaH-scrollBarW) {
               new_y = scrollAreaH-block_h-scrollBarW;
            }
            if (new_y != lty) {
               lty = new_y;
               start_frac = (double)(((double)(lty-scrollBarW)) /
                     ((double)(scrollAreaH-(scrollBarW<<1))));
               TgDrawScrollBar(mainDisplay, vSBarWindow, VERT_SCROLLBAR, 0, 0,
                     scrollBarW, scrollAreaH, start_frac, drawWinH, total);
            }
         } else {
            if (block_start <= 0) {
               new_y = 0;
            } else if (block_start+block_h >= scrollAreaH) {
               new_y = scrollAreaH-block_h;
            } else {
               new_y = block_start;
            }
            if (new_y != lty) {
               lty = new_y;
               start_frac = (double)(((double)lty)/((double)scrollAreaH));
               TgDrawScrollBar(mainDisplay, vSBarWindow, VERT_SCROLLBAR, 0, 0,
                     scrollBarW, scrollAreaH, start_frac, drawWinH, total);
            }
         }
         if (continuous_drag) {
            int scroll_all_the_way=FALSE;

            if (threeDLook) {
               start_frac = (double)(((double)(lty-scrollBarW)) /
                     ((double)(scrollAreaH-(scrollBarW<<1))));
               if (lty+block_h >= scrollAreaH-scrollBarW) {
                  scroll_all_the_way = TRUE;
               }
            } else {
               start_frac = (double)(((double)lty)/((double)scrollAreaH));
               if (block_start+block_h >= scrollAreaH) {
                  scroll_all_the_way = TRUE;
               }
            }
            RedrawAreaFromCache(start_frac, scroll_all_the_way);
         } else if (smoothScrollingCanvas == JUMP_SCROLLING) {
            DragToInVSBar(lty, block_start, block_h);
         }
         while (XCheckMaskEvent(mainDisplay, PointerMotionMask, &ev)) ;
      }
   }
   if (smoothScrollingCanvas == SMOOTH_SCROLLING) {
      EndExecAnimate();
      RedrawCurText();
   }
   scrollingCanvas = INVALID;

   DragToInVSBar(lty, block_start, block_h);
}

static
void VSBarHandler(button_ev)
   XButtonEvent *button_ev;
{
   int do_drag=FALSE, btn_offset=0;
   int total=max(paperHeight,(drawOrigY+drawWinH));

   if (!threeDLook && button_ev->button == Button3 &&
         button_ev->type == ButtonPress) {
      DoSBarBtnScroll(mainDisplay, vSBarWindow,
            ((button_ev->state & (ShiftMask|ControlMask)) != 0), SCRL_UP, NULL);
   } else if (!threeDLook && button_ev->button == Button1 &&
         button_ev->type == ButtonPress) {
      DoSBarBtnScroll(mainDisplay, vSBarWindow,
            ((button_ev->state & (ShiftMask|ControlMask)) != 0), SCRL_DN, NULL);
   } else if (button_ev->button == Button1 && button_ev->type == ButtonPress) {
      if (button_ev->y < scrollBarW ||
            button_ev->y >= scrollAreaH-scrollBarW) {
         int which=0;
         struct BBRec bbox;

         if (button_ev->y < scrollBarW) {
            which = SCRL_UP;
            SetBBRec(&bbox, 0, 0, scrollBarW, scrollBarW);
         } else {
            which = SCRL_DN;
            SetBBRec(&bbox, 0, scrollAreaH-scrollBarW, scrollBarW,
                  scrollAreaH);
         }
         DoSBarBtnScroll(mainDisplay, vSBarWindow, FALSE, which, &bbox);
      } else {
         double start_frac=(double)0.0;
         int hit=0;

         if (total == 0) {
            start_frac = (double)1.0;
         } else {
            start_frac = (double)((double)drawOrigY / (double)total);
         }
         if (start_frac > 1.0) start_frac = 1.0;
         hit = TgGetScrollHit(button_ev->x, button_ev->y, VERT_SCROLLBAR,
               scrollBarW, scrollAreaH, start_frac, drawWinH, total,
               &btn_offset);
         if (hit == 0) {
            do_drag = TRUE;
         } else {
            DoSBarBtnScroll(mainDisplay, vSBarWindow, TRUE,
                  (hit < 0 ? SCRL_UP : SCRL_DN), NULL);
         }
      }
   } else if (!threeDLook && button_ev->button == Button2 &&
         button_ev->type == ButtonPress) {
      do_drag = TRUE;
   }
   if (do_drag) {
      DoDragInVSBar(button_ev->y, btn_offset);
   }
}

void ScrollLeft(button_ev)
   XButtonEvent *button_ev;
{
   int adjustment=0;

   if (drawOrigX != 0) {
      switch (gridSystem) {
      case ENGLISH_GRID:
         if (button_ev!=NULL &&
               (button_ev->state & (ShiftMask|ControlMask))) {
            adjustment = ((int)(ZOOMED_SIZE(drawWinW)/HALF_INCH))*HALF_INCH;
            if (drawOrigX-ABS_SIZE(adjustment) < 0) {
               adjustment = ZOOMED_SIZE(drawOrigX);
            }
         } else {
            adjustment = HALF_INCH;
         }
         break;
      case METRIC_GRID:
         if (zoomedIn && zoomScale > 1) {
            if (button_ev!=NULL &&
                  (button_ev->state & (ShiftMask|ControlMask))) {
               adjustment = ((int)(ZOOMED_SIZE(drawWinW)/FAKE_CM))*FAKE_CM;
               if (drawOrigX-ABS_SIZE(adjustment) < 0) {
                  adjustment = ZOOMED_SIZE(drawOrigX);
               }
            } else {
               adjustment = FAKE_CM;
            }
         } else {
            if (button_ev!=NULL &&
                  (button_ev->state & (ShiftMask|ControlMask))) {
               adjustment = ((int)(ZOOMED_SIZE(drawWinW)/ONE_CM))*ONE_CM;
               if (drawOrigX-ABS_SIZE(adjustment) < 0) {
                  adjustment = ZOOMED_SIZE(drawOrigX);
               }
            } else {
               adjustment = ONE_CM;
            }
         }
         break;
      }
      if (drawOrigX >= ABS_SIZE(adjustment)) {
         drawOrigX -= ABS_SIZE(adjustment);
      } else {
         adjustment = ZOOMED_SIZE(drawOrigX);
         drawOrigX -= ABS_SIZE(adjustment);
         if (drawOrigX < 0) drawOrigX = 0;
      }
      RedrawHScrollWindow();
      UpdDrawWinBBox();
      AdjSplineVs();
      AdjustCurText(adjustment, 0);
      RedrawHRulerWindow();
      ClearAndRedrawDrawWindow();
   }
}

void ScrollRight(button_ev)
   XButtonEvent *button_ev;
{
   int adjustment=0;

   if (paperWidth <= drawWinW) return;

   if (drawOrigX+drawWinW < paperWidth) {
      switch (gridSystem) {
      case ENGLISH_GRID:
         if (button_ev!=NULL && (button_ev->state & (ShiftMask|ControlMask))) {
            adjustment = ((int)(ZOOMED_SIZE(drawWinW)/HALF_INCH))*HALF_INCH;
         } else {
            adjustment = HALF_INCH;
         }
         break;
      case METRIC_GRID:
         if (zoomedIn && zoomScale > 1) {
            if (button_ev!=NULL &&
                  (button_ev->state & (ShiftMask|ControlMask))) {
               adjustment = ((int)(ZOOMED_SIZE(drawWinW)/FAKE_CM))*FAKE_CM;
            } else {
               adjustment = FAKE_CM;
            }
         } else {
            if (button_ev!=NULL &&
                  (button_ev->state & (ShiftMask|ControlMask))) {
               adjustment = ((int)(ZOOMED_SIZE(drawWinW)/ONE_CM))*ONE_CM;
            } else {
               adjustment = ONE_CM;
            }
         }
         break;
      }
      drawOrigX += ABS_SIZE(adjustment);
      RedrawHScrollWindow();
      UpdDrawWinBBox();
      AdjSplineVs();
      AdjustCurText(-adjustment, 0);
      RedrawHRulerWindow();
      ClearAndRedrawDrawWindow();
   }
}

static
void DragToInHSBar(ltx, block_start, block_w)
   int ltx, block_start, block_w;
{
   double start_frac=(double)0.0;
   int saved_x=drawOrigX, adjustment=0, scroll_all_the_way=FALSE;

   if (threeDLook) {
      start_frac = (double)((double)(ltx-scrollBarW) /
            (double)(scrollAreaW-(scrollBarW<<1)));
      if (ltx+block_w >= scrollAreaW-scrollBarW) {
         scroll_all_the_way = TRUE;
      }
   } else {
      start_frac = (double)((double)(block_start)/(double)(scrollAreaW));
      if (block_start+block_w >= scrollAreaW) {
         scroll_all_the_way = TRUE;
      }
   }
   if (start_frac < 0.0) start_frac = 0.0;
   if (start_frac > 1.0) start_frac = 1.0;
   if (scroll_all_the_way) {
      GetMaxScrollOrigin(&drawOrigX, NULL);
   } else {
      double dv=(double)0;

      switch (gridSystem) {
      case ENGLISH_GRID:
         dv = ((double)paperWidth) * start_frac;
         break;
      case METRIC_GRID:
         if (zoomedIn && zoomScale > 1) {
            dv = ((double)paperWidth) * start_frac;
         } else {
            dv = ((double)paperWidth) * start_frac;
         }
         break;
      }
      drawOrigX = round(dv);
      drawOrigX = ABS_SIZE(ZOOMED_SIZE(drawOrigX));
   }
   adjustment = saved_x - drawOrigX;
   if (adjustment != 0) {
      RedrawHScrollWindow();
      UpdDrawWinBBox();
      AdjSplineVs();
      AdjustCurText(ZOOMED_SIZE(adjustment), 0);
      RedrawHRulerWindow();
      ClearAndRedrawDrawWindow();
   } else {
      RedrawHScrollWindow();
   }
}

static
void DoDragInHSBar(btn_x, btn_offset)
   int btn_x, btn_offset;
{
   double frac=(double)0.0, start_frac=(double)0.0;
   int block_w=0, block_start=0, continuous_drag=FALSE;
   int done=FALSE, ltx=0, min_block_size=1+(windowPadding<<1);
   int total=max(paperWidth,(drawOrigX+drawWinW));
   XEvent ev;

   scrollingCanvas = SCRL_LF;
   if (smoothScrollingCanvas == SMOOTH_SCROLLING) {
      BeginExecAnimate();
      if (!TgAnyButtonDown(mainDisplay, vSBarWindow)) {
         EndExecAnimate();
         ClearAndRedrawDrawWindow();
         return;
      }
      continuous_drag = execAnimating;
   }
   block_start = btn_x+btn_offset;
   frac = (double)(((double)drawWinW) / ((double)paperWidth));
   if (threeDLook) {
      start_frac = (double)(((double)(block_start-scrollBarW)) /
            ((double)(scrollAreaW-(scrollBarW<<1))));
      block_w = (frac >= 1.0) ? (scrollAreaW-(scrollBarW<<1)) :
            (int)(((double)(scrollAreaW-(scrollBarW<<1))) * frac);
      if (block_w < min_block_size) block_w = min_block_size;
      ltx = (start_frac + frac >= 1.0) ? scrollAreaW-scrollBarW-block_w :
            block_start;
      start_frac = (double)(((double)(ltx-scrollBarW)) /
            ((double)(scrollAreaW-(scrollBarW<<1))));
   } else {
      start_frac = (double)(((double)block_start)/((double)scrollAreaW));
      block_w = (frac >= 1.0) ? scrollAreaW :
            (int)(((double)scrollAreaW) * frac);
      if (block_w <= 0) block_w = 1;
      ltx = (start_frac + frac >= 1.0) ? scrollAreaW-block_w : block_start;
      start_frac = (double)(((double)ltx)/((double)scrollAreaW));
   }
   TgDrawScrollBar(mainDisplay, hSBarWindow, HORI_SCROLLBAR,
         0, 0, scrollAreaW, scrollBarW, start_frac, drawWinW, total);
   XGrabPointer(mainDisplay, hSBarWindow, False,
         PointerMotionMask | ButtonReleaseMask, GrabModeAsync,
         GrabModeAsync, None, handCursor, CurrentTime);

   while (!done) {
      XNextEvent(mainDisplay, &ev);

      if (ev.type == Expose || ev.type == VisibilityNotify) {
         ExposeEventHandler(&ev, TRUE);
      } else if (ev.type == ButtonRelease) {
         XUngrabPointer(mainDisplay, CurrentTime);
         done = TRUE;
      } else if (ev.type == MotionNotify) {
         int new_x=0;

         block_start = ev.xmotion.x;

         if (threeDLook) {
            new_x = ev.xmotion.x+btn_offset;
            if (new_x < scrollBarW) {
               new_x = scrollBarW;
            } else if (new_x+block_w >= scrollAreaW-scrollBarW) {
               new_x = scrollAreaW-block_w-scrollBarW;
            }
            if (new_x != ltx) {
               ltx = new_x;
               start_frac = (double)(((double)(ltx-scrollBarW)) /
                     ((double)(scrollAreaW-(scrollBarW<<1))));
               TgDrawScrollBar(mainDisplay, hSBarWindow, HORI_SCROLLBAR, 0, 0,
                     scrollAreaW, scrollBarW, start_frac, drawWinW, total);
            }
         } else {
            if (block_start <= 0) {
               new_x = 0;
            } else if (block_start+block_w >= scrollAreaW) {
               new_x = scrollAreaW-block_w;
            } else {
               new_x = block_start;
            }
            if (new_x != ltx) {
               ltx = new_x;
               start_frac = (double)(((double)ltx)/((double)scrollAreaW));
               TgDrawScrollBar(mainDisplay, hSBarWindow, HORI_SCROLLBAR, 0, 0,
                     scrollAreaW, scrollBarW, start_frac, drawWinW, total);
            }
         }
         if (continuous_drag) {
            int scroll_all_the_way=FALSE;

            if (threeDLook) {
               start_frac = (double)(((double)(ltx-scrollBarW)) /
                     ((double)(scrollAreaW-(scrollBarW<<1))));
               if (ltx+block_w >= scrollAreaW-scrollBarW) {
                  scroll_all_the_way = TRUE;
               }
            } else {
               start_frac = (double)(((double)ltx)/((double)scrollAreaW));
               if (block_start+block_w >= scrollAreaW) {
                  scroll_all_the_way = TRUE;
               }
            }
            RedrawAreaFromCache(start_frac, scroll_all_the_way);
         } else if (smoothScrollingCanvas == JUMP_SCROLLING) {
            DragToInHSBar(ltx, block_start, block_w);
         }
         while (XCheckMaskEvent(mainDisplay, PointerMotionMask, &ev)) ;
      }
   }
   if (smoothScrollingCanvas == SMOOTH_SCROLLING) {
      EndExecAnimate();
      RedrawCurText();
   }
   scrollingCanvas = INVALID;

   DragToInHSBar(ltx, block_start, block_w);
}

static
void HSBarHandler(button_ev)
   XButtonEvent *button_ev;
{
   int do_drag=FALSE, btn_offset=0;
   int total=max(paperWidth,(drawOrigX+drawWinW));

   if (!threeDLook && button_ev->button == Button3 &&
         button_ev->type == ButtonPress) {
      DoSBarBtnScroll(mainDisplay, hSBarWindow,
            ((button_ev->state & (ShiftMask|ControlMask)) != 0), SCRL_LF, NULL);
   } else if (!threeDLook && button_ev->button == Button1 &&
         button_ev->type == ButtonPress) {
      DoSBarBtnScroll(mainDisplay, hSBarWindow,
            ((button_ev->state & (ShiftMask|ControlMask)) != 0), SCRL_RT, NULL);
   } else if (button_ev->button == Button1 && button_ev->type == ButtonPress) {
      if (button_ev->x < scrollBarW ||
            button_ev->x >= scrollAreaW-scrollBarW) {
         int which=0;
         struct BBRec bbox;

         if (button_ev->x < scrollBarW) {
            which = SCRL_LF;
            SetBBRec(&bbox, 0, 0, scrollBarW, scrollBarW);
         } else {
            which = SCRL_RT;
            SetBBRec(&bbox, scrollAreaW-scrollBarW, 0, scrollAreaW,
                  scrollBarW);
         }
         DoSBarBtnScroll(mainDisplay, hSBarWindow, FALSE, which, &bbox);
      } else {
         double start_frac=(double)0.0;
         int hit=0;

         if (total == 0) {
            start_frac = (double)1.0;
         } else {
            start_frac = (double)((double)drawOrigX / (double)total);
         }
         if (start_frac > 1.0) start_frac = 1.0;
         hit = TgGetScrollHit(button_ev->x, button_ev->y, HORI_SCROLLBAR,
               scrollAreaW, scrollBarW, start_frac, drawWinW, total,
               &btn_offset);
         if (hit == 0) {
            do_drag = TRUE;
         } else {
            DoSBarBtnScroll(mainDisplay, hSBarWindow, TRUE,
                  (hit < 0 ? SCRL_LF : SCRL_RT), NULL);
         }
      }
   } else if (!threeDLook && button_ev->button == Button2 &&
         button_ev->type == ButtonPress) {
      do_drag = TRUE;
   }
   if (do_drag) {
      DoDragInHSBar(button_ev->x, btn_offset);
   }
}

void ScrollEventHandler(input)
   XEvent *input;
{
   if (input->xany.window == vSBarWindow) {
      if (input->type == Expose) {
         XSync(mainDisplay, False);
         RedrawVScrollWindow();
         return;
      } else if (input->type == MotionNotify) {
         if (threeDLook) {
            if (input->xmotion.y < scrollBarW) {
               SetMouseStatus(TgLoadCachedString(CSTID_SCROLL_UP), "", "");
            } else if (input->xmotion.y >= scrollAreaH-scrollBarW) {
               SetMouseStatus(TgLoadCachedString(CSTID_SCROLL_DOWN), "", "");
            } else {
               int total=max(paperHeight,(drawOrigY+drawWinH)), hit=FALSE;
               double start_frac=(double)0.0;

               if (total == 0) {
                  start_frac = (double)1.0;
               } else {
                  start_frac = (double)((double)drawOrigY / (double)total);
               }
               if (start_frac > 1.0) start_frac = 1.0;
               hit = TgGetScrollHit(input->xmotion.x, input->xmotion.y,
                     VERT_SCROLLBAR, scrollBarW, scrollAreaH, start_frac,
                     drawWinH, total, NULL);
               if (hit < 0) {
                  SetMouseStatus(TgLoadCachedString(CSTID_SCROLL_PAGE_UP),
                        "", "");
               } else if (hit > 0) {
                  SetMouseStatus(TgLoadCachedString(CSTID_SCROLL_PAGE_DOWN),
                        "", "");
               } else {
                  SetMouseStatus(TgLoadCachedString(CSTID_SCROLL_VERTICALLY),
                        "", "");
               }
            }
         }
         return;
      } else if (input->type == EnterNotify) {
         if (threeDLook) {
            SetMouseStatus("", "", "");
         } else {
            SetMouseStatus(TgLoadCachedString(CSTID_SCROLL_DOWN),
                  TgLoadCachedString(CSTID_SCROLL_VERTICALLY),
                  TgLoadCachedString(CSTID_SCROLL_UP));
         }
         return;
      }
      Msg(""); 
      VSBarHandler(&(input->xbutton));
      return;
   } else if (input->xany.window == hSBarWindow) {
      if (input->type == Expose) {
         XSync(mainDisplay, False);
         RedrawHScrollWindow();
         return;
      } else if (input->type == MotionNotify) {
         if (threeDLook) {
            if (input->xmotion.x < scrollBarW) {
               SetMouseStatus(TgLoadCachedString(CSTID_SCROLL_LEFT), "", "");
            } else if (input->xmotion.x >= scrollAreaW-scrollBarW) {
               SetMouseStatus(TgLoadCachedString(CSTID_SCROLL_RIGHT), "", "");
            } else {
               int total=max(paperWidth,(drawOrigX+drawWinW)), hit=FALSE;
               double start_frac=(double)0.0;

               if (total == 0) {
                  start_frac = (double)1.0;
               } else {
                  start_frac=(double)((double)drawOrigX / (double)total);
               }
               if (start_frac > 1.0) start_frac = 1.0;
               hit = TgGetScrollHit(input->xmotion.x, input->xmotion.y,
                     HORI_SCROLLBAR, scrollAreaW, scrollBarW, start_frac,
                     drawWinW, total, NULL);
               if (hit < 0) {
                  SetMouseStatus(TgLoadCachedString(CSTID_SCROLL_PAGE_LEFT),
                        "", "");
               } else if (hit > 0) {
                  SetMouseStatus(TgLoadCachedString(CSTID_SCROLL_PAGE_RIGHT),
                        "", "");
               } else {
                  SetMouseStatus(TgLoadCachedString(CSTID_SCROLL_HORIZONTALLY),
                        "", "");
               }
            }
         }
         return;
      } else if (input->type == EnterNotify) {
         if (threeDLook) {
            SetMouseStatus("", "", "");
         } else {
            SetMouseStatus(TgLoadCachedString(CSTID_SCROLL_RIGHT),
                  TgLoadCachedString(CSTID_SCROLL_HORIZONTALLY),
                  TgLoadCachedString(CSTID_SCROLL_LEFT));
         }
         return;
      }
      Msg(""); 
      HSBarHandler(&(input->xbutton));
      return;
   }
}

void ScrollToSpecifiedOrigin(cur_page_num, orig_x, orig_y, zoom_scale,
      zoomed_in)
   int cur_page_num, orig_x, orig_y, zoom_scale, zoomed_in;
{
   int adj_caches=(zoomScale != zoom_scale || zoomedIn != zoomed_in);

   if (cur_page_num <= 0 || cur_page_num > lastPageNum) return;
   if (drawOrigX == orig_x && drawOrigY == orig_y &&
         cur_page_num == curPageNum && !adj_caches) {
      return;
   }
   drawOrigX = orig_x;
   drawOrigY = orig_y;
   zoomScale = zoom_scale;
   zoomedIn = zoomed_in;
   UpdDrawWinWH();

   UpdDrawWinBBox();
   AdjSplineVs();
   if (adj_caches) { AdjCaches(); ShowZoom(); }

   if (cur_page_num != curPageNum) {
      GotoPageNum(cur_page_num);
      ShowPage();
   }
   ClearAndRedrawDrawWindow();
   RedrawRulers();
   RedrawScrollBars();
   justDupped = FALSE;
}

static int xOrigin=0, yOrigin=0, zoomScaleOrigin=0, zoomedInOrigin=FALSE;

int ScrollToOrigin()
   /*
    * return TRUE of scrolled or zoomed
    */
{
   int adj_caches=(zoomScale!=zoomScaleOrigin || zoomedIn!=zoomedInOrigin);

   if (drawOrigX==xOrigin && drawOrigY==yOrigin && !adj_caches) {
      return FALSE;
   }
   TieLooseEnds();
   SetCurChoice(NOTHING);

   drawOrigX = xOrigin;
   drawOrigY = yOrigin;
   zoomScale = zoomScaleOrigin;
   zoomedIn = zoomedInOrigin;
   UpdDrawWinWH();

   UpdDrawWinBBox();
   AdjSplineVs();
   if (adj_caches) { AdjCaches(); ShowZoom(); }
   ClearAndRedrawDrawWindow();
   RedrawRulers();
   RedrawScrollBars();
   justDupped = FALSE;

   return TRUE;
}

void SaveOrigin()
{
   xOrigin = drawOrigX;
   yOrigin = drawOrigY;
   zoomScaleOrigin = zoomScale;
   zoomedInOrigin = zoomedIn;
}

void SetOrigin(x, y, zoom_scale, zoomed_in)
   int x, y, zoom_scale, zoomed_in;
{
   xOrigin = x;
   yOrigin = y;
   zoomScaleOrigin = zoom_scale;
   zoomedInOrigin = zoomed_in;
}

void GetOrigin(pn_x, pn_y, pn_zoom_scale, pn_zoomed_in)
   int *pn_x, *pn_y, *pn_zoom_scale, *pn_zoomed_in;
{
   *pn_x = xOrigin;
   *pn_y = yOrigin;
   *pn_zoom_scale = zoomScaleOrigin;
   *pn_zoomed_in = zoomedInOrigin;
}

void CleanUpScrolls()
{
   if (scrl_up_bits == NULL) { }
}

void UpdScrollWinWH()
{
   scrollAreaH = vSBarH;
   scrollAreaW = hSBarW;
   RedrawScrollBars();
}

void InitScroll()
{
   char *c_ptr=NULL;

   smoothScrollingCanvas = JUMP_SCROLLING;
   if ((c_ptr=XGetDefault(mainDisplay, TOOL_NAME, "SmoothScrollingCanvas")) !=
         NULL) {
      if (UtilStrICmp(c_ptr, "smooth") == 0) {
         smoothScrollingCanvas = SMOOTH_SCROLLING;
      } else if (UtilStrICmp(c_ptr, "jump") == 0) {
         smoothScrollingCanvas = JUMP_SCROLLING;
      } else if (UtilStrICmp(c_ptr, "off") == 0) {
         smoothScrollingCanvas = NO_UPDATE_SCROLLING;
      } else {
         fprintf(stderr, TgLoadString(STID_INVALID_XDEF_USE_ALT_STR),
               TOOL_NAME, "SmoothScrollingCanvas", c_ptr, "jump");
         fprintf(stderr, "\n");
      }
   }
   resetOriginOnAdvancePage = FALSE;
   if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"ResetOriginOnAdvancePage")) !=
         NULL && UtilStrICmp(c_ptr, "true") == 0) {
      resetOriginOnAdvancePage = TRUE;
   }
   UpdScrollWinWH();
}

void ChangeScrollMode(ModeIndex)
   int ModeIndex;
{
   smoothScrollingCanvas = ModeIndex;

   switch (smoothScrollingCanvas) {
   case SMOOTH_SCROLLING:
      sprintf(gszMsgBox, TgLoadString(STID_WILL_UPD_WIN_SMOOTH_SCROLL));
      break;
   case JUMP_SCROLLING:
      sprintf(gszMsgBox, TgLoadString(STID_WILL_UPD_WIN_JUMP_SCROLL));
      break;
   case NO_UPDATE_SCROLLING:
      sprintf(gszMsgBox, TgLoadString(STID_WILL_NOT_UPD_WIN_SCROLL));
      break;
   }
   Msg(gszMsgBox);
}

int RefreshScrollModeMenu(menu)
   TgMenu *menu;
{
   int i, rc=TRUE, num_items=menu->num_items;
   TgMenuItem *menuitems=menu->menuitems;

   for (i=0; i < num_items; i++) {
      TgMenuItem *menu_item=(&menuitems[i]);
      TgMenuItem stMenuItem;

      memset(&stMenuItem, 0, sizeof(TgMenuItem));
      stMenuItem.state = TGBS_NORMAL;
      stMenuItem.checked = (i == smoothScrollingCanvas);
      if (!TgSetMenuItemInfo(menu_item, TGMU_MASK_STATE|TGMU_MASK_RADIO,
            &stMenuItem)) {
         rc = FALSE;
      }
   }
   return rc;
}

TgMenu *CreateScrollModeMenu(parent_menu, x, y, menu_info, status_str_xlated)
   TgMenu *parent_menu;
   int x, y;
   TgMenuInfo *menu_info;
   int status_str_xlated; /* ignored, always 0 */
{
   TgMenu *menu=TgCreateMenuFromMenuInfo(parent_menu, x, y, menu_info, FALSE);

   if (menu != NULL) {
      menu->track_menubar = TRUE;
      if (!RefreshScrollModeMenu(menu)) {
         return TgDestroyMenu(menu, TRUE);
      }
      menu->refresh_proc = ((RefreshMenuFunc*)RefreshScrollModeMenu);
   }
   return menu;
}
