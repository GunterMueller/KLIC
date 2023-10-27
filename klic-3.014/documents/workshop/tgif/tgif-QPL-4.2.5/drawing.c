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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/drawing.c,v 1.69 2011/06/09 16:11:41 cvsps Exp $
 */

#define _INCLUDE_FROM_DRAWING_C_

#include "tgifdefs.h"
#include "cmdids.h"

#include "align.e"
#include "animate.e"
#include "arc.e"
#include "attr.e"
#include "auxtext.e"
#include "box.e"
#include "choice.e"
#include "cmd.e"
#include "color.e"
#include "cutpaste.e"
#include "cursor.e"
#include "dialog.e"
#include "drawing.e"
#include "dup.e"
#include "edit.e"
#include "eps.e"
#include "exec.e"
#include "file.e"
#include "font.e"
#include "grid.e"
#include "group.e"
#include "help.e"
#include "imgproc.e"
#include "import.e"
#include "inmethod.e"
#include "mark.e"
#include "mainloop.e"
#include "mainmenu.e"
#include "menu.e"
#include "menuinfo.e"
#include "miniline.e"
#include "msg.e"
#include "navigate.e"
#include "names.e"
#include "obj.e"
#include "oval.e"
#include "page.e"
#include "pattern.e"
#include "pin.e"
#include "poly.e"
#include "polygon.e"
#include "raster.e"
#include "rcbox.e"
#include "rect.e"
#include "remote.e"
#include "ruler.e"
#include "scroll.e"
#include "select.e"
#include "setup.e"
#include "shape.e"
#include "shortcut.e"
#include "special.e"
#include "stk.e"
#include "stream.e"
#include "stretch.e"
#include "strtbl.e"
#include "tangram2.e"
#include "tcp.e"
#include "text.e"
#include "tidget.e"
#include "util.e"
#include "wb.e"
#include "xbitmap.e"
#include "xpixmap.e"

#define O_VIS 4
#define O_INVIS 4
#define O_GRID (O_VIS+O_INVIS)

#define DEF_CHECK_INTERVAL 1

int		disableRedraw=FALSE;
int		intrCheckInterval=DEF_CHECK_INTERVAL;
int		pasteInDrawTextMode=FALSE;
int		pasteFromFileInDrawTextMode=FALSE;
int		pasteCompoundTextInDrawTextMode=FALSE;
char		pasteFromFileName[MAXPATHLENGTH+1];
int		copyInDrawTextMode=FALSE;
int		copyDoubleByteStringInDrawTextMode=FALSE;
int		numRedrawBBox=INVALID;
int		numClipRecs=0;
int		clipOrdering=Unsorted;
XRectangle	clipRecs[4];
int		checkBBox=TRUE;

int		btn1Warp=FALSE;

int		userDisableRedraw=FALSE;
int		executingCommands=FALSE;
int		escPressedWhileExecutingCommands=FALSE;
int		gnDisableShortcuts=FALSE;
int		enableMouseWheel=TRUE;
int		btn2PopupMainMenu=FALSE;

static Pixmap	execAnimatePixmap=None;
static int	execAnimatePixmapW=0, execAnimatePixmapH=0;
static int	execAnimatePixmapDataW=0, execAnimatePixmapDataH=0;

static Pixmap	execAnimateRulerPixmap=None;
static int	execAnimateRulerPixmapW=0, execAnimateRulerPixmapH=0;

static struct BBRec	smallArea[2];

static int skipCrossHair=FALSE;

void SetXorDrawGC(xor_pixel)
   int xor_pixel;
{
   XGCValues values;

   values.foreground = xor_pixel;
   values.function = GXxor;
   values.fill_style = FillSolid;
#ifdef NO_THIN_LINE
   values.line_width = 1;
#else
   values.line_width = 0;
#endif
   values.line_style = LineSolid;

   XChangeGC(mainDisplay, drawGC,
         GCForeground | GCFunction | GCFillStyle | GCLineWidth | GCLineStyle,
         &values);
}

void SetDefaultDrawWinClipRecs()
{
   SetRecVals(clipRecs[0], 0, 0, ZOOMED_SIZE(drawWinW), ZOOMED_SIZE(drawWinH));
   numClipRecs = 1;
   clipOrdering = YXBanded;
   XSetClipRectangles(mainDisplay, drawGC, 0, 0, clipRecs, numClipRecs,
         clipOrdering);
}

void SetDefaultIconWinClipRecs()
{
   SetRecVals(clipRecs[0], 0, 0, iconWindowW, iconWindowH);
   numClipRecs = 1;
   clipOrdering = YXBanded;
   XSetClipRectangles(mainDisplay, drawGC, 0, 0, clipRecs, numClipRecs,
         clipOrdering);
}

static
void DrawHorizOutline(Win, Y, X1, X2, XStart, XEnd)
   Window Win;
   int Y, X1, X2, XStart, XEnd;
   /* XStart and XEnd are the real place, X1 and X2 are on outline grid */
{
   register int i;

   if (XStart-X1 < O_VIS) {
      XDrawLine(mainDisplay, Win, defaultGC, XStart, Y, X1+O_VIS-1, Y);
   }
   for (i=X1+O_GRID; i < X2-O_GRID; i+= O_GRID) {
      XDrawLine(mainDisplay, Win, defaultGC, i, Y, i+O_VIS-1, Y);
   }
   if (X2-XEnd < O_VIS) {
      XDrawLine(mainDisplay, Win, defaultGC, X2-O_GRID, Y, XEnd, Y);
   } else {
      XDrawLine(mainDisplay, Win, defaultGC, X2-O_GRID, Y, X2-O_INVIS-1, Y);
   }
}

static
void DrawVertOutline(Win, X, Y1, Y2, YStart, YEnd)
   Window Win;
   int X, Y1, Y2, YStart, YEnd;
   /* YStart and YEnd are the real place, Y1 and Y2 are on outline grid */
{
   register int i;

   if (YStart-Y1 < O_VIS) {
      XDrawLine(mainDisplay, Win, defaultGC, X, YStart, X, Y1+O_VIS-1);
   }
   for (i=Y1+O_GRID; i < Y2-O_GRID; i+= O_GRID) {
      XDrawLine(mainDisplay, Win, defaultGC, X, i, X, i+O_VIS-1);
   }
   if (Y2-YEnd < O_VIS) {
      XDrawLine(mainDisplay, Win, defaultGC, X, Y2-O_GRID, X, YEnd);
   } else {
      XDrawLine(mainDisplay, Win, defaultGC, X, Y2-O_GRID, X, Y2-O_INVIS-1);
   }
}

static
void DrawSymOutline(Win, XOff, YOff, ObjPtr)
   Window Win;
   int XOff, YOff;
   struct ObjRec *ObjPtr;
{
   int ltx, lty, rbx, rby, x_start, x_end, y_start, y_end;

   ltx = ZOOMED_SIZE(ObjPtr->obbox.ltx - XOff - QUARTER_INCH) + 1;
   lty = ZOOMED_SIZE(ObjPtr->obbox.lty - YOff - QUARTER_INCH) + 1;
   rbx = ZOOMED_SIZE(ObjPtr->obbox.rbx - XOff + QUARTER_INCH) - 1;
   rby = ZOOMED_SIZE(ObjPtr->obbox.rby - YOff + QUARTER_INCH) - 1;

   x_start = (ltx % O_GRID == 0) ? ltx : (int)(ltx / O_GRID) * O_GRID;
   x_end = (rbx % O_GRID == 0) ? rbx : ((int)(rbx / O_GRID) + 1) * O_GRID;
   DrawHorizOutline(Win, lty, x_start, x_end, ltx, rbx);
   DrawHorizOutline(Win, rby, x_start, x_end, ltx, rbx);
   y_start = (lty % O_GRID == 0) ? lty : (int)(lty / O_GRID) * O_GRID;
   y_end = (rby % O_GRID == 0) ? rby : ((int)(rby / O_GRID) + 1) * O_GRID;
   DrawVertOutline(Win, ltx, y_start, y_end, lty, rby);
   DrawVertOutline(Win, rbx, y_start, y_end, lty, rby);
}

static
int NeedToDraw(ObjBBox)
   struct BBRec ObjBBox;
{
   switch (numRedrawBBox) {
   case 0: return (BBoxIntersect(ObjBBox, drawWinBBox));
   case 1: return (BBoxIntersect(ObjBBox, drawWinBBox) &&
         BBoxIntersect(ObjBBox, smallArea[0]));
   case 2: return (BBoxIntersect(ObjBBox, drawWinBBox) &&
         (BBoxIntersect(ObjBBox, smallArea[0]) ||
         BBoxIntersect(ObjBBox, smallArea[1])));
   default:
      fprintf(stderr, "%s\n", TgLoadString(STID_WARN_INVALID_NUMREDRAWBBOX));
      break;
   }
   return TRUE;
}

#include "xbm/intr.xbm"
#include "xbm/trek.xbm"

static int intrShown=FALSE;
static int checkCount=0;
static int savedCheckInterval=(-1);
static int intrIndex=(-1);

static long intrTick=0L;

static
void RedrawInterrupt()
{
   GC gc;
   XGCValues values;
   int x=0, y=0, bg_pixel=(threeDLook ? myLtGryPixel : myBgPixel);
#ifdef _NO_GETTIMEOFDAY
   struct timeb now;
#else /* ~_NO_GETTIMEOFDAY */
   struct timeval now;
   struct timezone zone;
#endif /* _NO_GETTIMEOFDAY */
   long cur_tick;

   if (!intrShown) return;

#ifdef _NO_GETTIMEOFDAY
   ftime(&now);
   cur_tick = ((long)(((long)now.millitm) / 200)) +
         ((long)(((long)now.time) * 5));
#else /* ~_NO_GETTIMEOFDAY */
   gettimeofday(&now, &zone);
   cur_tick = ((long)(now.tv_usec / 200000)) + ((long)(now.tv_sec * 5));
#endif /* _NO_GETTIMEOFDAY */
   if (intrIndex != (-1) && intrTick == cur_tick) return;

   intrTick = cur_tick;
   if (++intrIndex == MAXINTRS) intrIndex = 0;

   x = ((rulerW-intr_width)>>1);
   y = ((rulerW-intr_height)>>1);
   values.foreground = myFgPixel;
   values.background = bg_pixel;
   values.function = GXcopy;
   values.fill_style = FillSolid;
   gc = XCreateGC(mainDisplay, dummyWindow1,
         GCForeground | GCBackground | GCFunction | GCFillStyle, &values);
   if (gc != NULL) {
      if (threeDLook) {
         XSetForeground(mainDisplay, gc, bg_pixel);
         XFillRectangle(mainDisplay, dummyWindow1, gc, 0, 0, rulerW, rulerW);
      }
      values.foreground = myFgPixel;
      values.fill_style = FillOpaqueStippled;
      values.stipple = intrPixmap[intrIndex];
      values.ts_x_origin = x;
      values.ts_y_origin = y;
      XChangeGC(mainDisplay, gc, GCForeground | GCFillStyle | GCStipple |
            GCTileStipXOrigin | GCTileStipYOrigin, &values);
      XFillRectangle(mainDisplay, dummyWindow1, gc, x, y, intr_width,
            intr_height);
      XFreeGC(mainDisplay, gc);
   }
   XSync(mainDisplay, False);
}

static
void ShowHyperSpace()
{
   GC gc;
   XGCValues values;
   int x=0, y=0, bg_pixel=(threeDLook ? myLtGryPixel : myBgPixel);

   x = ((rulerW-1-trek_width)>>1);
   y = ((rulerW-1-trek_height)>>1);
   values.foreground = myFgPixel;
   values.background = bg_pixel;
   values.function = GXcopy;
   values.fill_style = FillSolid;
   gc = XCreateGC(mainDisplay, dummyWindow1,
         GCForeground | GCBackground | GCFunction | GCFillStyle, &values);
   if (gc != NULL) {
      if (threeDLook) {
         XSetForeground(mainDisplay, gc, bg_pixel);
         XFillRectangle(mainDisplay, dummyWindow1, gc, 0, 0, rulerW, rulerW);
      }
      values.foreground = myFgPixel;
      values.fill_style = FillOpaqueStippled;
      values.stipple = trekPixmap;
      values.ts_x_origin = x;
      values.ts_y_origin = y;
      XChangeGC(mainDisplay, gc, GCForeground | GCFillStyle | GCStipple |
            GCTileStipXOrigin | GCTileStipYOrigin, &values);
      XFillRectangle(mainDisplay, dummyWindow1, gc, x, y, trek_width,
            trek_height);
      XFreeGC(mainDisplay, gc);
   }
}

static int interruptLevel=0;

void ShowInterrupt(CheckInterval)
   int CheckInterval;
{
   if (PRTGIF || interruptLevel++ > 0) return;

   if (CheckInterval > 0) {
      savedCheckInterval = intrCheckInterval;
      intrCheckInterval = CheckInterval;
   }
   if (intrCheckInterval <= 0) return;

   intrShown = TRUE;
   intrIndex = (-1);
   RedrawInterrupt();
}

int HideInterrupt()
{
   if (PRTGIF || --interruptLevel > 0) return interruptLevel;
   interruptLevel = 0;
   if (execAnimatePixmap == None) {
      XEvent ev;

      while (XCheckWindowEvent(mainDisplay,dummyWindow1,ButtonPressMask,&ev)) ;
   }
   XClearWindow(mainDisplay, dummyWindow1);
   intrShown = FALSE;
   checkCount = 0;
   if (savedCheckInterval > 0) {
      intrCheckInterval = savedCheckInterval;
      savedCheckInterval = (-1);
   }
   if (inHyperSpace) ShowHyperSpace();
   XSync(mainDisplay, False);
   return 0;
}

static
void HighLightDummyWindow1(highlight)
   int highlight;
{
   if (threeDLook) {
      struct BBRec bbox;

      SetBBRec(&bbox, 0, 0, rulerW-1, rulerW-1);
      if (highlight) {
         TgDrawThreeDButton(mainDisplay, dummyWindow1, textMenuGC, &bbox,
               TGBS_RAISED, 1, FALSE);
      } else {
         TgClearThreeDButton(mainDisplay, dummyWindow1, textMenuGC, &bbox, 1);
      }
   }
}

void RedrawDummyWindow1()
{
   XEvent ev;

   if (mainDisplay == NULL) return;

   while (XCheckWindowEvent(mainDisplay, dummyWindow1, ExposureMask, &ev)) ;
   while (XCheckWindowEvent(mainDisplay, dummyWindow1, ButtonPressMask, &ev)) ;
   if (intrShown) {
      RedrawInterrupt();
   } else if (inHyperSpace) {
      ShowHyperSpace();
   } else {
      HideInterrupt();
   }
   if (intr_bits != NULL && trek_bits != NULL) { } /* do nothing but reference the variable */
}

#include "xbm/run.xbm"

static
void ShowRunning(dpy, win, win_w, win_h)
   Display *dpy;
   Window win;
   int win_w, win_h;
{
   GC gc;
   XGCValues values;
   int x=0, y=0, bg_pixel=(threeDLook ? myLtGryPixel : myBgPixel);

   x = ((rulerW-run_width)>>1);
   y = ((rulerW-run_height)>>1);
   values.foreground = myFgPixel;
   values.background = bg_pixel;
   values.function = GXcopy;
   values.fill_style = FillSolid;
   gc = XCreateGC(dpy, win,
         GCForeground | GCBackground | GCFunction | GCFillStyle, &values);
   if (gc != NULL) {
      if (threeDLook) {
         XSetForeground(dpy, gc, bg_pixel);
         XFillRectangle(dpy, win, gc, 0, 0, win_w, win_h);
      }
      values.foreground = myFgPixel;
      values.fill_style = FillOpaqueStippled;
      values.stipple = runBitmap;
      values.ts_x_origin = x;
      values.ts_y_origin = y;
      XChangeGC(dpy, gc, GCForeground | GCFillStyle | GCStipple |
            GCTileStipXOrigin | GCTileStipYOrigin, &values);
      XFillRectangle(dpy, win, gc, 0, 0, run_width, run_height);
      XFreeGC(dpy, gc);
   }
   if (run_bits != NULL) { } /* do nothing but reference the variable */
}

void RedrawDummyWindow2()
{
   XEvent ev;

   if (mainDisplay == NULL) return;

   XClearWindow(mainDisplay, dummyWindow2);
   while (XCheckWindowEvent(mainDisplay, dummyWindow2, ExposureMask, &ev)) ;
   if (gnDisableShortcuts) {
      ShowRunning(mainDisplay, dummyWindow2, scrollBarW, scrollBarW);
   }
   if (threeDLook) {
      struct BBRec bbox;

      SetBBRec(&bbox, 0, 0, scrollBarW, scrollBarW);
      TgDrawThreeDButton(mainDisplay, dummyWindow2, textMenuGC, &bbox,
            gnDisableShortcuts ? TGBS_RAISED : TGBS_LOWRED, 2, TRUE);
   }
}

void DummiesEventHandler(input)
   XEvent *input;
{
   if (input->xany.window == dummyWindow1) {
      if (input->type == Expose) {
         RedrawDummyWindow1();
      } else if (input->type == EnterNotify) {
         if (intrShown) {
            SetMouseStatus(TgLoadCachedString(CSTID_INTERRUPT),
                  TgLoadCachedString(CSTID_INTERRUPT),
                  TgLoadCachedString(CSTID_INTERRUPT));
         } else if (inHyperSpace) {
            SetMouseStatus(TgLoadCachedString(CSTID_LEAVE_HYPERSPACE),
                  TgLoadCachedString(CSTID_PARANED_NONE),
                  TgLoadCachedString(CSTID_PARANED_NONE));
         } else {
            SetMouseStatus(TgLoadCachedString(CSTID_ENTER_HYPERSPACE),
                  TgLoadCachedString(CSTID_PARANED_NONE),
                  TgLoadCachedString(CSTID_PARANED_NONE));
         }
         if (inHyperSpace) {
            HighLightDummyWindow1(TRUE);
         }
      } else if (input->type == LeaveNotify) {
         SetMouseStatus("", "", "");
         if (inHyperSpace) {
            HighLightDummyWindow1(FALSE);
         }
      } else if (input->type == ButtonPress) {
         if (!intrShown && execAnimatePixmap == None) {
            ToggleHyperSpace(FALSE);
            if (inHyperSpace) {
               HighLightDummyWindow1(TRUE);
            }
         } else if (intrShown) {
            HideInterrupt();
         }
      }
   } else if (input->xany.window == dummyWindow2) {
      if (input->type == Expose) {
         RedrawDummyWindow2();
      } else if (input->type == EnterNotify) {
         SetMouseStatusToAllNone();
      }
   }
}

static XComposeStatus c_stat;

int KeyPressEventIsEscape(key_ev)
   XKeyEvent *key_ev;
{
   KeySym key_sym=(KeySym)0;
   char buf[80];
   int has_ch=XLookupString(key_ev, buf, sizeof(buf), &key_sym, &c_stat);

   TranslateKeys(buf, &key_sym);
   if (CharIsESC(key_ev, buf, key_sym, &has_ch)) {
      return TRUE;
   }
   return FALSE;
}

static
Bool CheckESC(p_display, p_ev, psz_arg)
   Display *p_display;
   XEvent *p_ev;
   char *psz_arg;
{
   if (p_ev->type == KeyPress) {
      if (KeyPressEventIsEscape(&p_ev->xkey)) {
         return True;
      }
   }
   return False;
}

int ESCPressed()
{
   XEvent ev;

   if (PRTGIF) return FALSE;
   if (XCheckIfEvent(mainDisplay, &ev, CheckESC, NULL)) {
      if (executingCommands) {
         escPressedWhileExecutingCommands = TRUE;
      }
      return TRUE;
   }
   if (executingCommands) {
      return escPressedWhileExecutingCommands;
   }
   return FALSE;
}

int CheckInterrupt(check_esc)
   int check_esc;
{
   if (PRTGIF) return FALSE;
   if (execAnimatePixmap == None && intrCheckInterval <= 0) return FALSE;
   if (check_esc && ESCPressed()) return TRUE;
   if (++checkCount >= intrCheckInterval) {
      XEvent ev;

      RedrawInterrupt();
      checkCount = 0;
      if (check_esc && XCheckWindowEvent(mainDisplay, dummyWindow1,
            ButtonPressMask, &ev)) {
         while (XCheckWindowEvent(mainDisplay, dummyWindow1, ButtonPressMask,
               &ev)) ;
         return TRUE;
      }
   }
   return FALSE;
}

void DrawClippedPixmap(pixmap, win, gc, pixmap_w, pixmap_h, ltx, lty)
   Pixmap pixmap;
   Window win;
   GC gc;
   int pixmap_w, pixmap_h, ltx, lty;
{
   if (numClipRecs <= 0) {
      XCopyArea(mainDisplay, pixmap, win, gc, 0, 0, pixmap_w, pixmap_h,
            ltx, lty);
   } else {
      int i;
      struct BBRec pixmap_bbox;

      pixmap_bbox.ltx = ltx;
      pixmap_bbox.lty = lty;
      pixmap_bbox.rbx = ltx+pixmap_w;
      pixmap_bbox.rby = lty+pixmap_h;
      for (i=0; i < numClipRecs; i++) {
         struct BBRec bbox;

         bbox.ltx = (int)clipRecs[i].x;
         bbox.lty = (int)clipRecs[i].y;
         bbox.rbx = bbox.ltx + ((int)clipRecs[i].width);
         bbox.rby = bbox.lty + ((int)clipRecs[i].height);
         if (BBoxIntersect(pixmap_bbox, bbox)) {
            int x, y, w, h;

            bbox.ltx = max(bbox.ltx, pixmap_bbox.ltx);
            bbox.lty = max(bbox.lty, pixmap_bbox.lty);
            bbox.rbx = min(bbox.rbx, pixmap_bbox.rbx);
            bbox.rby = min(bbox.rby, pixmap_bbox.rby);
            x = bbox.ltx - pixmap_bbox.ltx;
            y = bbox.lty - pixmap_bbox.lty;
            w = bbox.rbx - bbox.ltx;
            h = bbox.rby - bbox.lty;
            XCopyArea(mainDisplay, pixmap, win, gc, x, y, w, h,
                  ltx+x, lty+y);
         }
      }
   }
}

void FillClippedRectangle(win, gc, ltx, lty, orig_w, orig_h)
   Window win;
   GC gc;
   int ltx, lty, orig_w, orig_h;
{
   if (numClipRecs <= 0) {
      XFillRectangle(mainDisplay, win, gc, ltx, lty, orig_w, orig_h);
   } else {
      int i;
      struct BBRec obj_bbox;

      obj_bbox.ltx = ltx;
      obj_bbox.lty = lty;
      obj_bbox.rbx = ltx+orig_w;
      obj_bbox.rby = lty+orig_h;
      for (i=0; i < numClipRecs; i++) {
         struct BBRec bbox;

         bbox.ltx = (int)clipRecs[i].x;
         bbox.lty = (int)clipRecs[i].y;
         bbox.rbx = bbox.ltx + ((int)clipRecs[i].width);
         bbox.rby = bbox.lty + ((int)clipRecs[i].height);
         if (BBoxIntersect(obj_bbox, bbox)) {
            int x, y, w, h;

            bbox.ltx = max(bbox.ltx, obj_bbox.ltx);
            bbox.lty = max(bbox.lty, obj_bbox.lty);
            bbox.rbx = min(bbox.rbx, obj_bbox.rbx);
            bbox.rby = min(bbox.rby, obj_bbox.rby);
            x = bbox.ltx - obj_bbox.ltx;
            y = bbox.lty - obj_bbox.lty;
            w = bbox.rbx - bbox.ltx;
            h = bbox.rby - bbox.lty;
            XFillRectangle(mainDisplay, win, gc, ltx+x, lty+y, w, h);
         }
      }
   }
}

int ObjInVisibleLayer(ObjPtr)
   struct ObjRec *ObjPtr;
{
   struct ObjRec *obj_ptr;
   struct AttrRec *attr_ptr;

   if (!colorLayers) return TRUE;

   switch (ObjPtr->type) {
   case OBJ_POLY:
   case OBJ_BOX:
   case OBJ_OVAL:
   case OBJ_POLYGON:
   case OBJ_ARC:
   case OBJ_RCBOX:
   case OBJ_XBM:
      if (colorLayerOn[ObjPtr->color]) {
         return TRUE;
      }
      break;

   case OBJ_TEXT:
      if (ObjPtr->detail.t->fill != NONEPAT && colorLayerOn[ObjPtr->color]) {
         return TRUE;
      } else {
         return MiniLinesInVisibleLayer(&ObjPtr->detail.t->minilines);
      }
      break;

   case OBJ_XPM: return TRUE;

   case OBJ_GROUP:
   case OBJ_ICON:
   case OBJ_SYM:
      for (obj_ptr=ObjPtr->detail.r->last; obj_ptr != NULL;
            obj_ptr=obj_ptr->prev) {
         obj_ptr->tmp_parent = ObjPtr;
         if (ObjInVisibleLayer(obj_ptr)) {
            return TRUE;
         }
      }
      break;

   case OBJ_PIN:
      obj_ptr = GetPinObj(ObjPtr);
      obj_ptr->tmp_parent = ObjPtr;
      if (ObjInVisibleLayer(obj_ptr)) {
         return TRUE;
      }
      break;
   }
   for (attr_ptr=ObjPtr->fattr; attr_ptr != NULL; attr_ptr = attr_ptr->next) {
      if (attr_ptr->shown && ObjInVisibleLayer(attr_ptr->obj)) {
         return TRUE;
      }
   }
   return FALSE;
}

int DrawObj(Win, ObjPtr)
   Window Win;
   register struct ObjRec *ObjPtr;
   /* returns TRUE if all objects are drawn */
   /* returns FALSE if interrupted by the user */
{
   if (disableRedraw || (placingTopObj && ObjPtr==topObj)) {
      return TRUE;
   }
   switch (ObjPtr->type) {
   case OBJ_POLY:
      if (!colorLayers ||
            ObjPtr->tmp_parent!=NULL || ObjInVisibleLayer(ObjPtr)) {
         DrawPolyObj(Win, drawOrigX, drawOrigY, ObjPtr); 
         DrawAttrs(Win, drawOrigX, drawOrigY, ObjPtr->fattr);
      }
      break;
   case OBJ_BOX:
      if (!colorLayers ||
            ObjPtr->tmp_parent!=NULL || ObjInVisibleLayer(ObjPtr)) {
         DrawBoxObj(Win, drawOrigX, drawOrigY, ObjPtr);
         DrawAttrs(Win, drawOrigX, drawOrigY, ObjPtr->fattr);
      }
      break;
   case OBJ_OVAL:
      if (!colorLayers ||
            ObjPtr->tmp_parent!=NULL || ObjInVisibleLayer(ObjPtr)) {
         DrawOvalObj(Win, drawOrigX, drawOrigY, ObjPtr);
         DrawAttrs(Win, drawOrigX, drawOrigY, ObjPtr->fattr);
      }
      break;
   case OBJ_TEXT:
      if (!colorLayers ||
            ObjPtr->tmp_parent!=NULL || ObjInVisibleLayer(ObjPtr)) {
         DrawTextObj(Win, drawOrigX, drawOrigY, ObjPtr);
      }
      break;
   case OBJ_POLYGON:
      if (!colorLayers ||
            ObjPtr->tmp_parent!=NULL || ObjInVisibleLayer(ObjPtr)) {
         DrawPolygonObj(Win, drawOrigX, drawOrigY, ObjPtr);
         DrawAttrs(Win, drawOrigX, drawOrigY, ObjPtr->fattr);
      }
      break;
   case OBJ_ARC:
      if (!colorLayers ||
            ObjPtr->tmp_parent!=NULL || ObjInVisibleLayer(ObjPtr)) {
         DrawArcObj(Win, drawOrigX, drawOrigY, ObjPtr);
         DrawAttrs(Win, drawOrigX, drawOrigY, ObjPtr->fattr);
      }
      break;
   case OBJ_RCBOX:
      if (!colorLayers ||
            ObjPtr->tmp_parent!=NULL || ObjInVisibleLayer(ObjPtr)) {
         DrawRCBoxObj(Win, drawOrigX, drawOrigY, ObjPtr);
         DrawAttrs(Win, drawOrigX, drawOrigY, ObjPtr->fattr);
      }
      break;
   case OBJ_XBM:
      if (!colorLayers ||
            ObjPtr->tmp_parent!=NULL || ObjInVisibleLayer(ObjPtr)) {
         DrawXBmObj(Win, drawOrigX, drawOrigY, ObjPtr);
         DrawAttrs(Win, drawOrigX, drawOrigY, ObjPtr->fattr);
      }
      break;
   case OBJ_XPM:
      if (!colorLayers ||
            ObjPtr->tmp_parent!=NULL || ObjInVisibleLayer(ObjPtr)) {
         DrawXPmObj(Win, drawOrigX, drawOrigY, ObjPtr);
         DrawAttrs(Win, drawOrigX, drawOrigY, ObjPtr->fattr);
      }
      break;

   case OBJ_SYM:
   case OBJ_ICON:
   case OBJ_GROUP:
      if (!colorLayers ||
            ObjPtr->tmp_parent!=NULL || ObjInVisibleLayer(ObjPtr)) {
         struct ObjRec *obj_ptr=ObjPtr->detail.r->last;

         for ( ; obj_ptr != NULL; obj_ptr = obj_ptr->prev) {
            if (!checkBBox || NeedToDraw(obj_ptr->bbox)) {
               obj_ptr->tmp_parent = ObjPtr;
               if (!DrawObj(Win, obj_ptr)) return FALSE;
               if (execAnimatePixmap == None && CheckInterrupt(TRUE)) {
                  SetStringStatus(TgLoadString(STID_USER_INTR_ABORT_REPAINT));
                  Msg(TgLoadString(STID_USER_INTR_ABORT_REPAINT));
                  return FALSE;
               }
            }
         }
         if (ObjPtr->type == OBJ_ICON && ObjPtr->dirty) {
            struct AttrRec *attr_ptr=ObjPtr->fattr;

            for ( ; attr_ptr != NULL; attr_ptr = attr_ptr->next) {
               UpdTextBBox(attr_ptr->obj);
            }
            AdjObjBBox(ObjPtr);
            UpdSelBBox();
            ObjPtr->dirty = FALSE;
         }
         DrawAttrs(Win, drawOrigX, drawOrigY, ObjPtr->fattr);
         if (ObjPtr->type == OBJ_SYM) {
            DrawSymOutline(Win, drawOrigX, drawOrigY, ObjPtr);
         }
      }
      break;

   case OBJ_PIN:
      if (!colorLayers ||
            ObjPtr->tmp_parent!=NULL || ObjInVisibleLayer(ObjPtr)) {
         struct ObjRec *obj_ptr=GetPinObj(ObjPtr);

         if (!checkBBox || NeedToDraw(obj_ptr->bbox)) {
            obj_ptr->tmp_parent = ObjPtr;
            if (!DrawObj(Win, obj_ptr)) return FALSE;
            if (execAnimatePixmap == None && CheckInterrupt(TRUE)) {
               SetStringStatus(TgLoadString(STID_USER_INTR_ABORT_REPAINT));
               Msg(TgLoadString(STID_USER_INTR_ABORT_REPAINT));
               return FALSE;
            }
         }
         DrawAttrs(Win, drawOrigX, drawOrigY, ObjPtr->fattr);
      }
      break;
   }
   return TRUE;
}

void DrawPaperBoundary(Win)
   Window Win;
{
   register int x_end, y_end;

   if (mainDisplay == NULL || inSlideShow) return;

   if (drawOrigX+drawWinW > paperWidth) {
      x_end = OFFSET_X(paperWidth);
      if (drawOrigY+drawWinH > paperHeight) {
         y_end = OFFSET_Y(paperHeight);
         XDrawLine(mainDisplay, Win, defaultGC, x_end, 0, x_end, y_end);
         XDrawLine(mainDisplay, Win, defaultGC, 0, y_end, x_end, y_end);
      } else {
         XDrawLine(mainDisplay, Win, defaultGC, x_end, 0, x_end,
               ZOOMED_SIZE(drawWinH));
      }
   } else if (drawOrigY+drawWinH > paperHeight) {
      y_end = OFFSET_Y(paperHeight);
      XDrawLine(mainDisplay, Win, defaultGC, 0, y_end,
            ZOOMED_SIZE(drawWinW), y_end);
   }
}

void RedrawAnArea(BotObj, LtX, LtY, RbX, RbY)
   struct ObjRec *BotObj;
   int LtX, LtY, RbX, RbY;
   /* LtX, LtY, RbX, RbY are absolute coordinates */
{
   register struct ObjRec *obj_ptr;
   int x=OFFSET_X(LtX), y=OFFSET_Y(LtY), redraw_cross_hair=FALSE;
   int w=ZOOMED_SIZE(RbX-LtX)+1, h=ZOOMED_SIZE(RbY-LtY)+1;

   if (mainDisplay == NULL || disableRedraw) return;

   if (!userDisableRedraw) {
      smallArea[0].ltx = LtX; smallArea[0].lty = LtY;
      smallArea[0].rbx = RbX; smallArea[0].rby = RbY;
      if (!BBoxIntersect(smallArea[0], drawWinBBox)) {
         return;
      }
      SetRecVals(clipRecs[0], OFFSET_X(LtX), OFFSET_Y(LtY),
            ZOOMED_SIZE(RbX-LtX)+1, ZOOMED_SIZE(RbY-LtY)+1);
      numClipRecs = 1;
      clipOrdering = YXBanded;
      XSetClipRectangles(mainDisplay, drawGC, 0, 0, clipRecs, numClipRecs,
            clipOrdering);

      if (execAnimatePixmap != None) {
         XGCValues values;
         int real_w=(x+w >= execAnimatePixmapW ? execAnimatePixmapW-x : w);
         int real_h=(y+h >= execAnimatePixmapH ? execAnimatePixmapH-y : h);

         if (!skipCrossHair && showCrossHair) {
            int cx, cy;

            GetCrossHairPosition(&cx, &cy, NULL);
            if (cx >= x && cx < x+real_w && cy >= y && cy < y+real_h) {
               RedrawCrossHair();
               redraw_cross_hair = TRUE;
            }
         }
         values.foreground = GetDrawingBgPixel(INVALID, INVALID);
         values.function = GXcopy;
         values.fill_style = FillSolid;
         XChangeGC(mainDisplay, drawGC,
               GCForeground | GCFunction | GCFillStyle, &values);
         XFillRectangle(mainDisplay, execAnimatePixmap, drawGC,
               x, y, real_w, real_h);
      } else {
         if (!skipCrossHair && showCrossHair) {
            int cx, cy;

            GetCrossHairPosition(&cx, &cy, NULL);
            if (cx >= x && cx < x+w && cy >= y && cy < y+h) {
               RedrawCrossHair();
               redraw_cross_hair = TRUE;
            }
         }
         XClearArea(mainDisplay, drawWindow, x, y, w, h, FALSE);
      }

      if ((paperWidth >= LtX && paperWidth <= RbX) ||
            (paperHeight >= LtY && paperHeight <= RbY)) {
         DrawPaperBoundary(execAnimatePixmap==None ? drawWindow :
               execAnimatePixmap);
      }
      if (execAnimatePixmap != None) {
         DrawGridLines(execAnimatePixmap, x, y, w, h);
         DrawPageLines(execAnimatePixmap, x, y, w, h);
      } else {
         DrawGridLines(drawWindow, x, y, w, h);
         DrawPageLines(drawWindow, x, y, w, h);
      }
      ShowInterrupt(DEF_CHECK_INTERVAL);
   }
   numRedrawBBox = 1;
   smallArea[0].ltx = LtX; smallArea[0].lty = LtY;
   smallArea[0].rbx = RbX; smallArea[0].rby = RbY;
   for (obj_ptr = BotObj; obj_ptr != NULL; obj_ptr = obj_ptr->prev) {
      obj_ptr->tmp_parent = NULL;
      if (BBoxIntersect(obj_ptr->bbox, drawWinBBox) &&
            BBoxIntersect(obj_ptr->bbox, smallArea[0])) {
         if (!DrawObj(execAnimatePixmap==None ? drawWindow : execAnimatePixmap,
               obj_ptr)) {
            break;
         }
         if (execAnimatePixmap == None && CheckInterrupt(TRUE)) {
            SetStringStatus(TgLoadString(STID_USER_INTR_ABORT_REPAINT));
            Msg(TgLoadString(STID_USER_INTR_ABORT_REPAINT));
            break;
         }
      }
   }
   if (!userDisableRedraw) {
      HideInterrupt();
      SetDefaultDrawWinClipRecs();
      if (execAnimatePixmap != None && execAnimateRedraw &&
            x < execAnimatePixmapW && y < execAnimatePixmapH) {
         int real_w=(x+w >= execAnimatePixmapW ? execAnimatePixmapW-x : w);
         int real_h=(y+h >= execAnimatePixmapH ? execAnimatePixmapH-y : h);

         XSetFunction(mainDisplay, drawGC, GXcopy);
         XCopyArea(mainDisplay, execAnimatePixmap, drawWindow, drawGC,
               x, y, real_w, real_h, x, y);
      }
      if (redraw_cross_hair) RedrawCrossHair();
   }
}

void RedrawAreas(BotObj, LtX1, LtY1, RbX1, RbY1, LtX2, LtY2, RbX2, RbY2)
   struct ObjRec *BotObj;
   int LtX1, LtY1, RbX1, RbY1, LtX2, LtY2, RbX2, RbY2;
   /* note:  these coordinates are absolute */
{
   register struct ObjRec *obj_ptr;
   struct BBRec bbox1, bbox2;
   int rec1_slot, redraw_cross_hair=FALSE;
   int x1=OFFSET_X(LtX1), y1=OFFSET_Y(LtY1);
   int w1=ZOOMED_SIZE(RbX1-LtX1)+1, h1=ZOOMED_SIZE(RbY1-LtY1)+1;
   int x2=OFFSET_X(LtX2), y2=OFFSET_Y(LtY2);
   int w2=ZOOMED_SIZE(RbX2-LtX2)+1, h2=ZOOMED_SIZE(RbY2-LtY2)+1;

   if (mainDisplay == NULL || disableRedraw) return;

   bbox1.ltx = LtX1; bbox1.lty = LtY1;
   bbox1.rbx = RbX1; bbox1.rby = RbY1;
   bbox2.ltx = LtX2; bbox2.lty = LtY2;
   bbox2.rbx = RbX2; bbox2.rby = RbY2;

   if (Inside(bbox1, bbox2)) {
      RedrawAnArea(BotObj, LtX2, LtY2, RbX2, RbY2);
      return;
   } else if (Inside(bbox2, bbox1)) {
      RedrawAnArea(BotObj, LtX1, LtY1, RbX1, RbY1);
      return;
   }
   if (!BBoxIntersect(bbox1, drawWinBBox) &&
         !BBoxIntersect(bbox2, drawWinBBox)) {
      return;
   }

   if (!userDisableRedraw) {
      if (execAnimatePixmap != None) {
         XGCValues values;
         int real_w=(x1+w1 >= execAnimatePixmapW ? execAnimatePixmapW-x1 : w1);
         int real_h=(y1+h1 >= execAnimatePixmapH ? execAnimatePixmapH-y1 : h1);

         if (!skipCrossHair && showCrossHair) {
            int cx, cy;

            GetCrossHairPosition(&cx, &cy, NULL);
            if (cx >= x1 && cx < x1+real_w && cy >= y1 && cy < y1+real_h) {
               RedrawCrossHair();
               redraw_cross_hair = TRUE;
            }
         }
         values.foreground = GetDrawingBgPixel(INVALID, INVALID);
         values.function = GXcopy;
         values.fill_style = FillSolid;
         XChangeGC(mainDisplay, drawGC,
               GCForeground | GCFunction | GCFillStyle, &values);
         XFillRectangle(mainDisplay, execAnimatePixmap, drawGC,
               x1, y1, real_w, real_h);
      } else {
         if (!skipCrossHair && showCrossHair) {
            int cx, cy;

            GetCrossHairPosition(&cx, &cy, NULL);
            if (cx >= x1 && cx < x1+w1 && cy >= y1 && cy < y1+h1) {
               RedrawCrossHair();
               redraw_cross_hair = TRUE;
            }
         }
         XClearArea(mainDisplay, drawWindow, x1, y1, w1, h1, FALSE);
      }

      if (BBoxIntersect(bbox1, bbox2)) {
         int union_ltx, union_lty, union_rbx, union_rby;

         union_ltx = min(LtX1,LtX2); union_lty = min(LtY1,LtY2);
         union_rbx = max(RbX1,RbX2); union_rby = max(RbY1,RbY2);
         skipCrossHair = TRUE;
         RedrawAnArea(BotObj, union_ltx, union_lty, union_rbx, union_rby);
         skipCrossHair = FALSE;
         if (redraw_cross_hair) RedrawCrossHair();
         return;
      }
      if (LtY1 == LtY2) {
         rec1_slot = (LtX1 <= LtX2) ? 0 : 1;
         SetRecVals(clipRecs[rec1_slot], OFFSET_X(LtX1), OFFSET_Y(LtY1),
               ZOOMED_SIZE(RbX1-LtX1)+1, ZOOMED_SIZE(RbY1-LtY1)+1);
         SetRecVals(clipRecs[!rec1_slot], OFFSET_X(LtX2), OFFSET_Y(LtY2),
               ZOOMED_SIZE(RbX2-LtX2)+1, ZOOMED_SIZE(RbY2-LtY2)+1);
         numClipRecs = 2;
      } else {
         if (LtY1 < LtY2) {
            if (RbY1 <= LtY2) {  /* y-extents do not intersect */
               SetRecVals(clipRecs[0], OFFSET_X(LtX1), OFFSET_Y(LtY1),
                     ZOOMED_SIZE(RbX1-LtX1)+1, ZOOMED_SIZE(RbY1-LtY1)+1);
               SetRecVals(clipRecs[1], OFFSET_X(LtX2), OFFSET_Y(LtY2),
                     ZOOMED_SIZE(RbX2-LtX2)+1, ZOOMED_SIZE(RbY2-LtY2)+1);
               numClipRecs = 2;
            } else if (RbY1 >= RbY2) {
               /* box 2's y-extents is inside box 1's y-extents */
               /*
                * Updated on 6/7/2009.  The old code below using rec1_slot
                *       did not make sense.
                *
                * rec1_slot = (LtX1 < LtX2) ? 0 : 1;  
                */
               SetRecVals(clipRecs[0], OFFSET_X(LtX1), OFFSET_Y(LtY1),
                     ZOOMED_SIZE(RbX1-LtX1)+1, ZOOMED_SIZE(RbY1-LtY1)+1);
               SetRecVals(clipRecs[1], OFFSET_X(LtX2), OFFSET_Y(LtY2),
                     ZOOMED_SIZE(RbX2-LtX2)+1, ZOOMED_SIZE(RbY2-LtY2)+1);
               numClipRecs = 2;
            } else {  
               SetRecVals(clipRecs[0], OFFSET_X(LtX1), OFFSET_Y(LtY1),
                     ZOOMED_SIZE(RbX1-LtX1)+1, ZOOMED_SIZE(LtY2-LtY1)+1);
               if (LtX1 < LtX2) {
                  SetRecVals(clipRecs[1], OFFSET_X(LtX1), OFFSET_Y(LtY2),
                        ZOOMED_SIZE(RbX1-LtX1)+1, ZOOMED_SIZE(RbY1-LtY2)+1);
                  SetRecVals(clipRecs[2], OFFSET_X(LtX2), OFFSET_Y(LtY2),
                        ZOOMED_SIZE(RbX2-LtX2)+1, ZOOMED_SIZE(RbY1-LtY2)+1);
               } else {
                  SetRecVals(clipRecs[1], OFFSET_X(LtX2), OFFSET_Y(LtY2),
                        ZOOMED_SIZE(RbX2-LtX2)+1, ZOOMED_SIZE(RbY1-LtY2)+1);
                  SetRecVals(clipRecs[2], OFFSET_X(LtX1), OFFSET_Y(LtY2),
                        ZOOMED_SIZE(RbX1-LtX1)+1, ZOOMED_SIZE(RbY1-LtY2)+1);
               }
               SetRecVals(clipRecs[3], OFFSET_X(LtX2), OFFSET_Y(RbY1),
                     ZOOMED_SIZE(RbX2-LtX2)+1, ZOOMED_SIZE(RbY2-RbY1)+1);
               numClipRecs = 4;
            }
         } else {
            if (RbY2 <= LtY1) {  /* y-extents do not intersect */
               SetRecVals(clipRecs[0], OFFSET_X(LtX2), OFFSET_Y(LtY2),
                     ZOOMED_SIZE(RbX2-LtX2)+1, ZOOMED_SIZE(RbY2-LtY2)+1);
               SetRecVals(clipRecs[1], OFFSET_X(LtX1), OFFSET_Y(LtY1),
                     ZOOMED_SIZE(RbX1-LtX1)+1, ZOOMED_SIZE(RbY1-LtY1)+1);
               numClipRecs = 2;
            } else if (RbY2 >= RbY1) {
               /* box 1's y-extents is inside box 2's y-extents */
               rec1_slot = (LtX1 < LtX2) ? 0 : 1;  
               SetRecVals(clipRecs[rec1_slot], OFFSET_X(LtX1), OFFSET_Y(LtY1),
                     ZOOMED_SIZE(RbX1-LtX1)+1, ZOOMED_SIZE(RbY1-LtY1)+1);
               SetRecVals(clipRecs[!rec1_slot], OFFSET_X(LtX2), OFFSET_Y(LtY2),
                     ZOOMED_SIZE(RbX2-LtX2)+1, ZOOMED_SIZE(RbY2-LtY2)+1);
               numClipRecs = 2;
            } else {  
               SetRecVals(clipRecs[0], OFFSET_X(LtX2), OFFSET_Y(LtY2),
                     ZOOMED_SIZE(RbX2-LtX2)+1, ZOOMED_SIZE(LtY1-LtY2)+1);
               if (LtX1 < LtX2) {
                  SetRecVals(clipRecs[1], OFFSET_X(LtX1), OFFSET_Y(LtY1),
                        ZOOMED_SIZE(RbX1-LtX1)+1, ZOOMED_SIZE(RbY2-LtY1)+1);
                  SetRecVals(clipRecs[2], OFFSET_X(LtX2), OFFSET_Y(LtY1),
                        ZOOMED_SIZE(RbX2-LtX2)+1, ZOOMED_SIZE(RbY2-LtY1)+1);
               } else {
                  SetRecVals(clipRecs[1], OFFSET_X(LtX2), OFFSET_Y(LtY1),
                        ZOOMED_SIZE(RbX2-LtX2)+1, ZOOMED_SIZE(RbY2-LtY1)+1);
                  SetRecVals(clipRecs[2], OFFSET_X(LtX1), OFFSET_Y(LtY1),
                        ZOOMED_SIZE(RbX1-LtX1)+1, ZOOMED_SIZE(RbY2-LtY1)+1);
               }
               SetRecVals(clipRecs[3], OFFSET_X(LtX1), OFFSET_Y(RbY2),
                     ZOOMED_SIZE(RbX1-LtX1)+1, ZOOMED_SIZE(RbY1-RbY2)+1);
               numClipRecs = 4;
            }
         }
      }
      clipOrdering = YXSorted;
      XSetClipRectangles(mainDisplay, drawGC, 0, 0, clipRecs, numClipRecs,
            clipOrdering);

      if ((paperWidth >= LtX1 && paperWidth <= RbX1) ||
            (paperHeight >= LtY1 && paperHeight <= RbY1) ||
            (paperWidth >= LtX2 && paperWidth <= RbX2) ||
            (paperHeight >= LtY2 && paperHeight <= RbY2)) {
         DrawPaperBoundary(execAnimatePixmap==None ? drawWindow :
               execAnimatePixmap);
      }
      if (execAnimatePixmap != None) {
         DrawGridLines(execAnimatePixmap, x1, y1, w1, h1);
         DrawPageLines(execAnimatePixmap, x1, y1, w1, h1);
      } else {
         DrawGridLines(drawWindow, x1, y1, w1, h1);
         DrawPageLines(drawWindow, x1, y1, w1, h1);
      }
      ShowInterrupt(DEF_CHECK_INTERVAL);
   }
   numRedrawBBox = 2;
   smallArea[0].ltx = LtX1; smallArea[0].lty = LtY1;
   smallArea[0].rbx = RbX1; smallArea[0].rby = RbY1;
   smallArea[1].ltx = LtX2; smallArea[1].lty = LtY2;
   smallArea[1].rbx = RbX2; smallArea[1].rby = RbY2;
   for (obj_ptr = BotObj; obj_ptr != NULL; obj_ptr = obj_ptr->prev) {
      obj_ptr->tmp_parent = NULL;
      if (BBoxIntersect(obj_ptr->bbox, drawWinBBox) &&
            (BBoxIntersect(obj_ptr->bbox, bbox1) ||
            BBoxIntersect(obj_ptr->bbox, bbox2))) {
         if (!DrawObj(execAnimatePixmap==None ? drawWindow : execAnimatePixmap,
               obj_ptr)) {
            break;
         }
         if (execAnimatePixmap == None && CheckInterrupt(TRUE)) {
            SetStringStatus(TgLoadString(STID_USER_INTR_ABORT_REPAINT));
            Msg(TgLoadString(STID_USER_INTR_ABORT_REPAINT));
            break;
         }
      }
   }
   if (!userDisableRedraw) {
      HideInterrupt();
      SetDefaultDrawWinClipRecs();
      if (execAnimatePixmap != None && execAnimateRedraw) {
         XSetFunction(mainDisplay, drawGC, GXcopy);
         if (x1 < execAnimatePixmapW && y1 < execAnimatePixmapH) {
            int real_w=(x1+w1>=execAnimatePixmapW ? execAnimatePixmapW-x1 : w1);
            int real_h=(y1+h1>=execAnimatePixmapH ? execAnimatePixmapH-y1 : h1);

            XCopyArea(mainDisplay, execAnimatePixmap, drawWindow, drawGC,
                  x1, y1, real_w, real_h, x1, y1);
         }
         if (x2 < execAnimatePixmapW && y2 < execAnimatePixmapH) {
            int real_w=(x2+w2>=execAnimatePixmapW ? execAnimatePixmapW-x2 : w2);
            int real_h=(y2+h2>=execAnimatePixmapH ? execAnimatePixmapH-y2 : h2);

            XCopyArea(mainDisplay, execAnimatePixmap, drawWindow, drawGC,
                  x2, y2, real_w, real_h, x2, y2);
         }
      }
      if (redraw_cross_hair) RedrawCrossHair();
   }
}

static
void GetBetterBBox(ObjPtr, LtX, LtY, RbX, RbY, AlreadyFound)
   struct ObjRec *ObjPtr;
   int *LtX, *LtY, *RbX, *RbY, *AlreadyFound;
{
   int found, style=INVALID, w=0, ltx, lty, rbx, rby;
   struct ObjRec *obj_ptr;
   struct AttrRec *attr_ptr;

   if (colorLayers &&
            ObjPtr->tmp_parent==NULL && !ObjInVisibleLayer(ObjPtr)) {
      return;
   }
   if (*AlreadyFound && ObjPtr->bbox.ltx >= *LtX && ObjPtr->bbox.lty >= *LtY &&
         ObjPtr->bbox.rbx <= *RbX && ObjPtr->bbox.rby <= *RbY) {
      return;
   }
   switch (ObjPtr->type) {
   case OBJ_POLY:
   case OBJ_ARC:
      switch (ObjPtr->type) {
      case OBJ_POLY:
         w = ObjPtr->detail.p->width;
         style = ObjPtr->detail.p->style;
         break;
      case OBJ_ARC:
         w = ObjPtr->detail.a->width;
         style = ObjPtr->detail.a->style;
         break;
      }
      if (style==LS_PLAIN && (w & 0x1)) {
         w = (w-1)>>1;
         ltx = ObjPtr->obbox.ltx-w; lty = ObjPtr->obbox.lty-w;
         rbx = ObjPtr->obbox.rbx+w; rby = ObjPtr->obbox.rby+w;
      } else {
         ltx = ObjPtr->bbox.ltx; lty = ObjPtr->bbox.lty;
         rbx = ObjPtr->bbox.rbx; rby = ObjPtr->bbox.rby;
      }
      break;
   case OBJ_BOX:
   case OBJ_OVAL:
   case OBJ_POLYGON:
   case OBJ_RCBOX:
      switch (ObjPtr->type) {
      case OBJ_BOX: w = ObjPtr->detail.b->width; break;
      case OBJ_OVAL: w = ObjPtr->detail.o->width; break;
      case OBJ_POLYGON: w = ObjPtr->detail.g->width; break;
      case OBJ_RCBOX: w = ObjPtr->detail.rcb->width; break;
      }
      if (w & 0x1) {
         w = (w-1)>>1;
         ltx = ObjPtr->obbox.ltx-w; lty = ObjPtr->obbox.lty-w;
         rbx = ObjPtr->obbox.rbx+w; rby = ObjPtr->obbox.rby+w;
      } else {
         ltx = ObjPtr->bbox.ltx; lty = ObjPtr->bbox.lty;
         rbx = ObjPtr->bbox.rbx; rby = ObjPtr->bbox.rby;
      }
      break;

   case OBJ_TEXT:
      ltx = ObjPtr->bbox.ltx; lty = ObjPtr->bbox.lty;
      rbx = ObjPtr->bbox.rbx; rby = ObjPtr->bbox.rby;
      break;

   case OBJ_XBM:
   case OBJ_XPM:
      ltx = ObjPtr->bbox.ltx;   lty = ObjPtr->bbox.lty;
      rbx = ObjPtr->bbox.rbx-1; rby = ObjPtr->bbox.rby-1;
      break;

   case OBJ_GROUP:
   case OBJ_ICON:
   case OBJ_SYM:
      found = FALSE;
      obj_ptr = ObjPtr->detail.r->last;
      for ( ; obj_ptr != NULL; obj_ptr = obj_ptr->prev) {
         obj_ptr->tmp_parent = ObjPtr;
         GetBetterBBox(obj_ptr, &ltx, &lty, &rbx, &rby, &found);
      }
      if (!found) {
         return;
      }
      break;

   case OBJ_PIN:
      found = FALSE;
      obj_ptr = GetPinObj(ObjPtr);
      obj_ptr->tmp_parent = ObjPtr;
      GetBetterBBox(obj_ptr, &ltx, &lty, &rbx, &rby, &found);
      if (!found) {
         return;
      }
      break;
   }
   for (attr_ptr=ObjPtr->lattr; attr_ptr!=NULL; attr_ptr=attr_ptr->prev) {
      if (attr_ptr->shown) {
         if (attr_ptr->obj->bbox.ltx < ltx) ltx = attr_ptr->obj->bbox.ltx;
         if (attr_ptr->obj->bbox.lty < lty) lty = attr_ptr->obj->bbox.lty;
         if (attr_ptr->obj->bbox.rbx > rbx) rbx = attr_ptr->obj->bbox.rbx;
         if (attr_ptr->obj->bbox.rby > rby) rby = attr_ptr->obj->bbox.rby;
      }
   }
   if (ObjPtr->type == OBJ_SYM) {
      if (ObjPtr->obbox.ltx-QUARTER_INCH < ltx) {
         ltx = ObjPtr->obbox.ltx - QUARTER_INCH;
      }
      if (ObjPtr->obbox.lty-QUARTER_INCH < lty) {
         lty = ObjPtr->obbox.lty - QUARTER_INCH;
      }
      if (ObjPtr->obbox.rbx+QUARTER_INCH > rbx) {
         rbx = ObjPtr->obbox.rbx + QUARTER_INCH;
      }
      if (ObjPtr->obbox.rby+QUARTER_INCH > rby) {
         rby = ObjPtr->obbox.rby + QUARTER_INCH;
      }
   }
   if (*AlreadyFound) {
      if (ltx < *LtX) *LtX = ltx; if (lty < *LtY) *LtY = lty;
      if (rbx > *RbX) *RbX = rbx; if (rby > *RbY) *RbY = rby;
   } else {
      *LtX = ltx; *LtY = lty; *RbX = rbx; *RbY = rby;
   }
   *AlreadyFound = TRUE;
}

int RedrawDuringScrolling()
{
   return (scrollingCanvas != INVALID && (execAnimatePixmap != None ||
         smoothScrollingCanvas == JUMP_SCROLLING));
}

static
void RedrawVertSliceFromCache(start_frac, scroll_all_the_way)
   double start_frac;
   int scroll_all_the_way;
{
   int y=0;

   if (start_frac < 0.0) start_frac = 0.0;
   if (start_frac > 1.0) start_frac = 1.0;
   if (scroll_all_the_way) {
      GetMaxScrollOrigin(NULL, &y);
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
      y = round(dv);
   }
   XCopyArea(mainDisplay, execAnimatePixmap, drawWindow, drawGC,
         0, ZOOMED_SIZE(y), initDrawWinW, initDrawWinH, 0, 0);
   XCopyArea(mainDisplay, execAnimateRulerPixmap, vRuleWindow, defaultGC,
         0, ZOOMED_SIZE(y), rulerW-windowPadding, initDrawWinH, 0, 0);
}

static
void RedrawHoriSliceFromCache(start_frac, scroll_all_the_way)
   double start_frac;
   int scroll_all_the_way;
{
   int x=0;

   if (start_frac < 0.0) start_frac = 0.0;
   if (start_frac > 1.0) start_frac = 1.0;
   if (scroll_all_the_way) {
      GetMaxScrollOrigin(&x, NULL);
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
      x = round(dv);
   }
   XCopyArea(mainDisplay, execAnimatePixmap, drawWindow, drawGC,
         ZOOMED_SIZE(x), 0, initDrawWinW, initDrawWinH, 0, 0);
   XCopyArea(mainDisplay, execAnimateRulerPixmap, hRuleWindow, defaultGC,
         ZOOMED_SIZE(x), 0, initDrawWinW, rulerW-windowPadding, 0, 0);
}

void RedrawAreaFromCache(start_frac, scroll_all_the_way)
   double start_frac;
   int scroll_all_the_way;
{
   if (mainDisplay == NULL || disableRedraw) return;

   if (RedrawDuringScrolling()) {
      switch (scrollingCanvas) {
      case SCRL_UP:
      case SCRL_DN:
         RedrawVertSliceFromCache(start_frac, scroll_all_the_way);
         break;
      case SCRL_LF:
      case SCRL_RT:
         RedrawHoriSliceFromCache(start_frac, scroll_all_the_way);
         break;
      }
   }
}

void RedrawDrawWindow(BotObj)
   struct ObjRec *BotObj;
{
   register struct ObjRec *obj_ptr;

   if (mainDisplay == NULL || disableRedraw) return;

   if (!skipCrossHair) {
      RedrawCrossHair();
   }
   if (execAnimating) {
      int already_animating=(execAnimatePixmap!=None), x=0, y=0, w=0, h=0;

      if (execAnimatePixmap != None) {
         XFreePixmap(mainDisplay, execAnimatePixmap);
      }
      if (execAnimateRulerPixmap != None) {
         XFreePixmap(mainDisplay, execAnimateRulerPixmap);
      }
      execAnimatePixmap = execAnimateRulerPixmap = None;
      switch (scrollingCanvas) {
      case SCRL_UP:
      case SCRL_DN:
         execAnimatePixmapW = ZOOMED_SIZE(drawWinW);
         GetMaxScrollOrigin(NULL, &y);
         execAnimatePixmapDataH = ZOOMED_SIZE(y+drawWinH);
         h = ZOOMED_SIZE(paperHeight);
         if ((h % initDrawWinH) == 0) {
            execAnimatePixmapH = (((int)(h / initDrawWinH))+1)*initDrawWinH;
         } else {
            execAnimatePixmapH = (((int)(h / initDrawWinH))+2)*initDrawWinH;
         }
         execAnimateRulerPixmapW = rulerW-windowPadding;
         execAnimateRulerPixmapH = execAnimatePixmapH;
         break;
      case SCRL_LF:
      case SCRL_RT:
         w = ZOOMED_SIZE(paperWidth);
         if ((w % initDrawWinW) == 0) {
            execAnimatePixmapW = (((int)(w / initDrawWinW))+1)*initDrawWinW;
         } else {
            execAnimatePixmapW = (((int)(w / initDrawWinW))+2)*initDrawWinW;
         }
         GetMaxScrollOrigin(&x, NULL);
         execAnimatePixmapDataW = ZOOMED_SIZE(x+drawWinW);
         execAnimatePixmapH = ZOOMED_SIZE(drawWinH);

         execAnimateRulerPixmapW = execAnimatePixmapW;
         execAnimateRulerPixmapH = rulerW-windowPadding;
         break;
      default:
         execAnimatePixmapW = ZOOMED_SIZE(drawWinW);
         execAnimatePixmapH = ZOOMED_SIZE(drawWinH);
         break;
      }
      execAnimatePixmap = XCreatePixmap(mainDisplay, mainWindow,
            execAnimatePixmapW, execAnimatePixmapH, mainDepth);
      if (execAnimatePixmap == None) {
         if (already_animating) {
            sprintf(gszMsgBox, TgLoadString(STID_CANNOT_ALLOC_XPM_NO_ANIM),
                  execAnimatePixmapW, execAnimatePixmapH);
            MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
         }
      } else {
         XGCValues values;

         values.foreground = GetDrawingBgPixel(INVALID, INVALID);
         values.function = GXcopy;
         values.fill_style = FillSolid;
         XChangeGC(mainDisplay, drawGC,
               GCForeground | GCFunction | GCFillStyle, &values);
         if (scrollingCanvas != INVALID) {
            int saved_win_w=drawWinW, saved_win_h=drawWinH, abort=FALSE;

            execAnimateRulerPixmap = XCreatePixmap(mainDisplay, mainWindow,
                  execAnimateRulerPixmapW, execAnimateRulerPixmapH, mainDepth);
            if (execAnimateRulerPixmap == None) {
               abort = TRUE;
            }
            drawWinW = ABS_SIZE(execAnimatePixmapW);
            drawWinH = ABS_SIZE(execAnimatePixmapH);

            SetDefaultDrawWinClipRecs();

            XFillRectangle(mainDisplay, execAnimatePixmap, drawGC,
                  0, 0, execAnimatePixmapW, execAnimatePixmapH);
            if (execAnimateRulerPixmap != None) {
               XFillRectangle(mainDisplay, execAnimateRulerPixmap, drawGC,
                     0, 0, execAnimateRulerPixmapW, execAnimateRulerPixmapH);
            }
            drawWinW = saved_win_w;
            drawWinH = saved_win_h;
            SetDefaultDrawWinClipRecs();

            if (abort) {
               XFreePixmap(mainDisplay, execAnimatePixmap);
               execAnimatePixmap = None;
            }
         } else {
            XFillRectangle(mainDisplay, execAnimatePixmap, drawGC,
                  0, 0, execAnimatePixmapW, execAnimatePixmapH);
         }
      }
   }
   if (execAnimatePixmap != None && scrollingCanvas != INVALID) {
      int saved_orig_x=drawOrigX, saved_orig_y=drawOrigY, x=0, y=0;
      int saved_win_w=drawWinW, saved_win_h=drawWinH, scroll_vertical=FALSE;
      int watch_cursor=watchCursorOnMainWindow;

      SaveStatusStrings();
      SetStringStatus(TgLoadCachedString(CSTID_CACHING_IMAGE));
      if (!watch_cursor) {
         SetWatchCursor(drawWindow);
         SetWatchCursor(mainWindow);
      }
      switch (scrollingCanvas) {
      case SCRL_UP:
      case SCRL_DN:
         x = 0;
         y = ZOOMED_SIZE(drawOrigY);
         drawOrigY = 0;
         drawWinH = ABS_SIZE(execAnimatePixmapH);
         scroll_vertical = TRUE;
         break;
      case SCRL_LF:
      case SCRL_RT:
         x = ZOOMED_SIZE(drawOrigX);
         y = 0;
         drawOrigX = 0;
         drawWinW = ABS_SIZE(execAnimatePixmapW);
         scroll_vertical = FALSE;
         break;
      }
      UpdDrawWinBBox();
      SetDefaultDrawWinClipRecs();

      DrawPaperBoundary(execAnimatePixmap);
      RedrawGridLines(execAnimatePixmap);
      RedrawPageLines(execAnimatePixmap);

      if (scroll_vertical) {
         RedrawVRuler(mainDisplay, execAnimateRulerPixmap);
      } else {
         RedrawHRuler(mainDisplay, execAnimateRulerPixmap);
      }
      AdjSplineVs();

      numRedrawBBox = 0;
      ShowInterrupt(DEF_CHECK_INTERVAL);
      for (obj_ptr = BotObj; obj_ptr != NULL; obj_ptr = obj_ptr->prev) {
         obj_ptr->tmp_parent = NULL;
         if (BBoxIntersect(obj_ptr->bbox, drawWinBBox)) {
            if (!TgAnyButtonDown(mainDisplay, drawWindow)) {
               /* canceled */
               Msg(TgLoadString(STID_SCROLLING_CANCELED));
               break;
            }
            if (!DrawObj(execAnimatePixmap, obj_ptr) && CheckInterrupt(TRUE)) {
               break;
            }
         }
      }
      HideInterrupt();

      XSetFunction(mainDisplay, drawGC, GXcopy);
      XCopyArea(mainDisplay, execAnimatePixmap, drawWindow, drawGC,
            x, y, initDrawWinW, initDrawWinH, 0, 0);

      drawOrigX = saved_orig_x;
      drawOrigY = saved_orig_y;
      drawWinW = saved_win_w;
      drawWinH = saved_win_h;
      UpdDrawWinBBox();
      SetDefaultDrawWinClipRecs();

      if (!watch_cursor) {
         SetDefaultCursor(mainWindow);
         ShowCursor(); 
      }
      RestoreStatusStrings();
   } else {
      DrawPaperBoundary(execAnimatePixmap==None ? drawWindow :
            execAnimatePixmap);
      RedrawGridLines(execAnimatePixmap==None ? drawWindow : execAnimatePixmap);
      RedrawPageLines(execAnimatePixmap==None ? drawWindow : execAnimatePixmap);

      numRedrawBBox = 0;
      ShowInterrupt(DEF_CHECK_INTERVAL);
      for (obj_ptr = BotObj; obj_ptr != NULL; obj_ptr = obj_ptr->prev) {
         obj_ptr->tmp_parent = NULL;
         if (BBoxIntersect(obj_ptr->bbox, drawWinBBox)) {
            if (!DrawObj(execAnimatePixmap==None ? drawWindow :
                  execAnimatePixmap, obj_ptr)) {
               break;
            }
            if (execAnimatePixmap == None && CheckInterrupt(TRUE)) {
               SetStringStatus(TgLoadString(STID_USER_INTR_ABORT_REPAINT));
               Msg(TgLoadString(STID_USER_INTR_ABORT_REPAINT));
               break;
            }
         }
      }
      HideInterrupt();
      if (execAnimatePixmap != None) {
         XSetFunction(mainDisplay, drawGC, GXcopy);
         XCopyArea(mainDisplay, execAnimatePixmap, drawWindow, drawGC,
               0, 0, execAnimatePixmapW, execAnimatePixmapH, 0, 0);
      }
      if (!skipCrossHair) {
         RedrawCrossHair();
      }
   }
}

Pixmap DrawAllOnPixmap(LtX, LtY, W, H, nRedraw)
   int *LtX, *LtY, *W, *H, nRedraw;
{
   register struct ObjRec *obj_ptr;
   int ltx, lty, rbx, rby, w, h, saved_zoom_scale, saved_zoomed_in, found=FALSE;
   int saved_draw_orig_x, saved_draw_orig_y, saved_draw_win_w, saved_draw_win_h;
   Pixmap pixmap;
   XGCValues values;

   ltx = lty = rbx = rby = 0;
   for (obj_ptr = botObj; obj_ptr != NULL; obj_ptr = obj_ptr->prev) {
      obj_ptr->tmp_parent = NULL;
      GetBetterBBox(obj_ptr, &ltx, &lty, &rbx, &rby, &found);
   }
   if (!found) {
      *LtX = *LtY = *W = *H = 0;
      sprintf(gszMsgBox, "No objects to print!");
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return None;
   }
   *W = w = rbx - ltx + 1;
   *H = h = rby - lty + 1;
   *LtX = ltx;
   *LtY = lty;

   saved_draw_orig_x = drawOrigX; saved_draw_orig_y = drawOrigY;
   saved_draw_win_w = drawWinW; saved_draw_win_h = drawWinH;
   saved_zoom_scale = zoomScale;
   saved_zoomed_in = zoomedIn;

   drawOrigX = ltx; drawOrigY = lty;
   drawWinW = w; drawWinH = h;
   zoomScale = 0;
   zoomedIn = FALSE;

   SetDefaultDrawWinClipRecs();

   pixmap = XCreatePixmap(mainDisplay, mainWindow, w, h, mainDepth);

   if (pixmap == None) {
      FailAllocPixmapMessage(w, h);
      sprintf(gszMsgBox, TgLoadString(STID_CANNOT_ALLOC_PIXMAP_OF_SIZE), w, h);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return None;
   }

   values.foreground = GetDrawingBgPixel(INVALID, INVALID);
   values.function = GXcopy;
   values.fill_style = FillSolid;
   XChangeGC(mainDisplay, drawGC,
         GCForeground | GCFunction | GCFillStyle, &values);
   XFillRectangle(mainDisplay, pixmap, drawGC, 0, 0, w, h);

   AdjCaches();
   AdjSplineVs();

   checkBBox = FALSE;
   ShowInterrupt(DEF_CHECK_INTERVAL);
   for (obj_ptr=botObj; obj_ptr != NULL; obj_ptr=obj_ptr->prev) {
      obj_ptr->tmp_parent = NULL;
      if (!DrawObj(pixmap, obj_ptr)) {
         XFreePixmap(mainDisplay, pixmap);
         pixmap = None;
         break;
      }
      if (execAnimatePixmap == None && CheckInterrupt(TRUE)) {
         SetStringStatus(TgLoadString(STID_USER_INTR_ABORT_REPAINT));
         Msg(TgLoadString(STID_USER_INTR_ABORT_REPAINT));
         XFreePixmap(mainDisplay, pixmap);
         pixmap = None;
         break;
      }
   }
   HideInterrupt();
   checkBBox = TRUE;

   drawOrigX = saved_draw_orig_x; drawOrigY = saved_draw_orig_y;
   drawWinW = saved_draw_win_w; drawWinH = saved_draw_win_h;
   zoomedIn = saved_zoomed_in;
   zoomScale = saved_zoom_scale;

   AdjSplineVs();
   AdjCaches();
   SetDefaultDrawWinClipRecs();

   skipCrossHair = TRUE;
   if (nRedraw) RedrawDrawWindow(botObj);
   skipCrossHair = FALSE;

   if (gpExportClipBBox != NULL) {
      int y, intersect_w=0, intersect_h=0;
      int src_offset_x=0, src_offset_y=0, dest_offset_x=0, dest_offset_y=0;
      int dest_w=gpExportClipBBox->rbx-gpExportClipBBox->ltx;
      int dest_h=gpExportClipBBox->rby-gpExportClipBBox->lty;
      Pixmap dest_pixmap=XCreatePixmap(mainDisplay, mainWindow, dest_w,
            dest_h, mainDepth);
      XImage *src_image=NULL, *dest_image=NULL;
      struct BBRec src_bbox, intersect_bbox;

      if (dest_pixmap == None) FailAllocBitmapMessage(dest_w, dest_h);
      XSetForeground(mainDisplay, drawGC, GetDrawingBgPixel(INVALID, INVALID));
      XFillRectangle(mainDisplay, dest_pixmap, drawGC, 0, 0, dest_w, dest_h);

      SetBBRec(&src_bbox, (*LtX), (*LtY), (*LtX)+(*W), (*LtY)+(*H));
      if (!IntersectRect(src_bbox, *gpExportClipBBox, &intersect_bbox)) {
         XFreePixmap(mainDisplay, pixmap);
         return dest_pixmap;
      }
      dest_image = XGetImage(mainDisplay, dest_pixmap, 0, 0, dest_w, dest_h,
            AllPlanes, ZPixmap);
      if (dest_image == NULL) FailAllocMessage();

      src_image = XGetImage(mainDisplay, pixmap, 0, 0, (*W), (*H), AllPlanes,
            ZPixmap);
      if (src_image == NULL) FailAllocMessage();

      intersect_w = intersect_bbox.rbx-intersect_bbox.ltx;
      intersect_h = intersect_bbox.rby-intersect_bbox.lty;
      src_offset_x = intersect_bbox.ltx-src_bbox.ltx;
      src_offset_y = intersect_bbox.lty-src_bbox.lty;
      dest_offset_x = intersect_bbox.ltx-gpExportClipBBox->ltx;
      dest_offset_y = intersect_bbox.lty-gpExportClipBBox->lty;

      for (y=0; y < intersect_h; y++) {
         int x;

         for (x=0; x < intersect_w; x++) {
            XPutPixel(dest_image, x+dest_offset_x, y+dest_offset_y,
                  XGetPixel(src_image, x+src_offset_x, y+src_offset_y));
         }
      }
      XPutImage(mainDisplay, dest_pixmap, xpmGC, dest_image, 0, 0, 0, 0,
            dest_w, dest_h);
      XDestroyImage(dest_image);
      XDestroyImage(src_image);
      XFreePixmap(mainDisplay, pixmap);
      *LtX = gpExportClipBBox->ltx;
      *LtY = gpExportClipBBox->lty;
      *W = gpExportClipBBox->rbx-(*LtX);
      *H = gpExportClipBBox->rby-(*LtY);
      return dest_pixmap;
   }
   return pixmap;
}

void ClearAndRedrawDrawWindow()
{
   if (mainDisplay == NULL || disableRedraw) return;

   XClearWindow(mainDisplay, drawWindow);
   if (execAnimatePixmap != None) {
      XGCValues values;

      values.foreground = GetDrawingBgPixel(INVALID, INVALID);
      values.function = GXcopy;
      values.fill_style = FillSolid;
      XChangeGC(mainDisplay, drawGC,
            GCForeground | GCFunction | GCFillStyle, &values);
      XFillRectangle(mainDisplay, execAnimatePixmap, drawGC,
            0, 0, execAnimatePixmapW, execAnimatePixmapH);
   }
   somethingHighLighted = FALSE;
   skipCrossHair = TRUE;
   RedrawDrawWindow(botObj);
   skipCrossHair = FALSE;
   ResetDirtyBBoxInfo();
   RedrawCurText();
   if (!execAnimating) HighLightForward();
   RedrawCrossHair();
}

void ClearAndRedrawDrawWindowNoCurT()
   /* use to be ClearAndRedrawDrawWindowDontDrawCurText() */
{
   if (mainDisplay == NULL || disableRedraw) return;

   XClearWindow(mainDisplay, drawWindow);
   if (execAnimatePixmap != None) {
      XGCValues values;

      values.foreground = GetDrawingBgPixel(INVALID, INVALID);
      values.function = GXcopy;
      values.fill_style = FillSolid;
      XChangeGC(mainDisplay, drawGC,
            GCForeground | GCFunction | GCFillStyle, &values);
      XFillRectangle(mainDisplay, execAnimatePixmap, drawGC,
            0, 0, execAnimatePixmapW, execAnimatePixmapH);
   }
   somethingHighLighted = FALSE;
   skipCrossHair = TRUE;
   RedrawDrawWindow(botObj);
   skipCrossHair = FALSE;
   HighLightForward();
   RedrawCrossHair();
}

int BeginExecAnimate()
{
   execAnimating = TRUE;
   execAnimateRedraw = TRUE;
   RedrawDrawWindow(botObj);

   if (execAnimatePixmap == None) {
      sprintf(gszMsgBox, TgLoadString(STID_CANNOT_ALLOC_PIXMAP_OF_SIZE),
            execAnimatePixmapW, execAnimatePixmapH);
      if (scrollingCanvas == INVALID) {
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      } else {
         SetStringStatus(gszMsgBox);
      }
      execAnimatePixmapW = execAnimatePixmapH = 0;
      execAnimating = execAnimateRedraw = FALSE;
      return FALSE;
   }
   return TRUE;
}

void EndExecAnimate()
{
   if (execAnimatePixmap != None) {
      XFreePixmap(mainDisplay, execAnimatePixmap);
   }
   if (execAnimateRulerPixmap != None) {
      XFreePixmap(mainDisplay, execAnimateRulerPixmap);
   }
   execAnimatePixmap = execAnimateRulerPixmap = None;
   execAnimatePixmapW = execAnimatePixmapH = 0;
   execAnimatePixmapDataW = execAnimatePixmapDataH = 0;
   execAnimating = execAnimateRedraw = FALSE;
}

void CleanUpDrawingWindow()
{
   FreeEditAttrInfo(gpEditAttrInEditorAttrInfo);
   gpEditAttrInEditorAttrInfo = NULL;

   if (execAnimatePixmap != None) {
      Msg(TgLoadString(STID_FORCING_END_ANIMATE));
      EndExecAnimate();
   }
   SetCurChoice(NOTHING);
   if (topSel != NULL) {
      if (!deserializingFile) HighLightReverse();
      RemoveAllSel();
   }
   if (tgifObj != NULL && tgifObj->fattr != NULL) {
      DelAllAttrs(tgifObj->fattr);
      tgifObj->fattr = tgifObj->lattr = NULL;
   }
   DelAllPages();
   ResetRotatePivotValidInfo();
}

static
long GetElapseTime(long start_sec, long start_msec, long end_sec, long end_msec)
{
   long diff_sec=end_sec-start_sec;
   long diff_msec=end_msec-start_msec;

   if (diff_msec < 0) {
      diff_sec--;
      diff_msec += 1000;
   }
   return ((diff_sec*1000)+diff_msec);
}

static
void BenchMark()
{
   static int count=0;
   int need_to_check_auto_exec=FALSE, i=0;

   while (XPending(mainDisplay)) {
      i++;
      TryProcessAnXEvent(&need_to_check_auto_exec);
   }
   /* debug, do not translate */
   fprintf(stderr, "%1d BenchMark Ready (%1d events processed)!\n", count, i);
   if (count++ < 3) {
      SendCommandToSelf(CMDID_BENCHMARK, INVALID);
   } else {
      long start_sec=0L, start_msec=0L, end_sec=0L, end_msec=0L;
      long elapsed_msec=0L;
      int x=0, y=0, z=0, w=64, x_end=(min(drawWinW,drawWinH)-64), total=1000000;
      XRectangle rectangles[1000];

      XSetForeground(mainDisplay, defaultGC, colorPixels[y%maxColors]);
      UtilGetMilliSecTime(&start_sec, &start_msec);
      for (i=0; i < total; i++) {
         XDrawRectangle(mainDisplay, drawWindow, defaultGC, x+y, x, w, w);
         if (x++ >= x_end) {
            x=0;
            if (y++ >= x_end) {
               y=0;
               XSetForeground(mainDisplay, defaultGC,
                     colorPixels[(++z)%maxColors]);
            }
         }
      }
      XSetForeground(mainDisplay, defaultGC, myFgPixel);
      UtilGetMilliSecTime(&end_sec, &end_msec);
      elapsed_msec = GetElapseTime(start_sec, start_msec, end_sec, end_msec);
      if (elapsed_msec > 0) {
         double avg=((double)total)*((double)1000)/((double)elapsed_msec);

         /* debug, do not translate */
         fprintf(stderr, "Took %ld ms to draw %1d rectangles (%.2f %s).\n",
               elapsed_msec, total, avg, "rectangles per second");
      }
      x = y = z = 0;
      XSetForeground(mainDisplay, defaultGC, colorPixels[y%maxColors]);
      UtilGetMilliSecTime(&start_sec, &start_msec);
      for (i=0; i < 1000; i++) {
         rectangles[i].width = rectangles[i].height = w;
      }
      w = 0;
      for (i=0; i < total; i++) {
         if (w < 1000) {
            rectangles[w].x = x+y;
            rectangles[w].y = x;
            w++;
         } else {
            XDrawRectangles(mainDisplay, drawWindow, defaultGC, rectangles,
                  1000);
            w = 0;
         }
         if (x++ >= x_end) {
            x=0;
            if (y++ >= x_end) {
               y=0;
               XSetForeground(mainDisplay, defaultGC,
                     colorPixels[(++z)%maxColors]);
            }
         }
      }
      XSetForeground(mainDisplay, defaultGC, myFgPixel);
      UtilGetMilliSecTime(&end_sec, &end_msec);
      elapsed_msec = GetElapseTime(start_sec, start_msec, end_sec, end_msec);
      if (elapsed_msec > 0) {
         double avg=((double)total)*((double)1000)/((double)elapsed_msec);

         /* debug, do not translate */
         fprintf(stderr, "Took %ld ms to draw %1d rectangles (%.2f %s).\n",
               elapsed_msec, total, avg, "rectangles per second");
      }
   }
}

static
void ToggleShowChat()
{
   noChatWindow = !noChatWindow;
   if (noChatWindow) {
      XUnmapWindow(mainDisplay, chatWindow);
   } else {
      XMapWindow(mainDisplay, chatWindow);
   }
   Reconfigure(TRUE);
}

static
int DoShortCut(key_ev, name, key_sym, state, args)
   XKeyEvent *key_ev;
   char *name, *args;
   KeySym key_sym;
   unsigned int state;
   /* returns INVALID if the key event can be caught by other windows */
{
   if (gnDisableShortcuts) {
      sprintf(gszMsgBox, TgLoadString(STID_TOOL_NOT_FINISH_WITH_EXEC),
            TOOL_NAME);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return BAD;
   }
   if ((state & ControlMask) && (!(state & METAMASK))) {
      int rc=BAD;

      switch (key_sym&0xff) {
      case 'a': /*^a*/ SelAllObj(TRUE, FALSE); break;
      case 'b': /*^b*/ BackProc(); break;
      case 'c': /*^c*/ CopyToCutBuffer(); break;
      case 'd': /*^d*/ DupSelObj(); break;
      case 'e': /*^e*/ PushCurChoice(); break;
      case 'f': /*^f*/ FrontProc(); break;
      case 'g': /*^g*/ GroupSelObj(TRUE, TRUE, TRUE); break;
      case 'h': /*^h*/ return(INVALID);
      case 'i': /*^i*/ Instantiate(); break;
      case 'j': /*^j*/ return(INVALID);
      case 'k': /*^k*/ PopIcon(); break;
      case 'l': /*^l*/ AlignSelObjs(); break;
      case 'm': /*^m*/ return(INVALID);
      case 'n': /*^n*/ NewProc(); break;
      case 'o': /*^o*/
         OpenProc((args==NULL || *args=='\0') ? NULL : args);
         break;
      case 'p': /*^p*/ Dump(""); break;
      case 'q': /*^q*/ if ((rc=QuitProc())==INVALID) return BAD; return rc;
      case 'r': /*^r*/ ClearAndRedrawDrawWindow(); break;
      case 's': /*^s*/ SaveFile(); break;
      case 't': /*^t*/ AlignSelToGrid(); break;
      case 'u': /*^u*/ UngroupSelObj(TRUE, TRUE); break;
      case 'v': /*^v*/ PasteFromCutBuffer(); break;
      case 'w': /*^w*/ SetCurChoice(DRAWTEXT); break;
      case 'x': /*^x*/ CutToCutBuffer(); break;
      case 'y': /*^y*/ RedoCmd(); break;
      case 'z': /*^z*/ UndoCmd(); break;
      case ',': /*^,*/ ScrollLeft(NULL); break;
      case '.': /*^.*/ ScrollRight(NULL); break;
      case '-': /*^-*/ PrintWithCommand(""); break;
      case '1': /*^1*/ ScreenCapture(); break;
      case '2': /*^2*/ FullScreenCapture(); break;

      case ' ':
      case '\\':
         if (curChoice == DRAWTEXT && textCursorShown &&
               (canvasFontDoubleByte || gstCopyUTF8Info.double_byte_valid)) {
            if (textHighlight) {
               curTextModified = TRUE;
               DelSelText();
            }
            if (!canvasFontDoubleByte && gstCopyUTF8Info.double_byte_valid) {
               SwitchToDoubleByteFont(&gstCopyUTF8Info.double_byte_seg);
            }
            if (tgIMActiveOnCntrlSpace(mainDisplay, drawWindow)) {
               if (tgIMHandleCntrlSpace(mainDisplay, drawWindow)) {
                  if (gnInputMethod != TGIM_NONE && gnOverTheSpot) {
                     if (!tgIMTellCursorPosition(mainDisplay, drawWindow,
                           textCurX, textCurBaselineY+curStrBlock->seg->des)) {
                     }
                  }
               }
            } else {
               return INVALID;
            }
         }
         break;
      default: return INVALID;
      }
   } else if ((state & METAMASK) && (!(state & ControlMask))) {
      switch (key_sym&0xff) {
      case 'a': /*#a*/ AddAttrs(); break;
      case 'b': /*#b*/ return ProbeProc();
      case 'c': /*#c*/ RotateCounter(); break;
      case 'd': /*#d*/ DecGrid(); break;
      case 'e': /*#e*/ AnimateSel(); break;
      case 'f': /*#f*/ FlashSelColor(); break;
      case 'g': /*#f*/ ToggleGridShown(); break;
      case 'h': /*#h*/ FlipHorizontal(); break;
      case 'i': /*#i*/ IncGrid(); break;
      case 'j': /*#j*/ HideAllAttrNames(); break;
      case 'k': /*#k*/ SetCurChoice(NOTHING); break;
      case 'l': /*#l*/ DistrSelObjs(); break;
      case 'm': /*#m*/ MoveAttr(); break;
      case 'n': /*#n*/ ShowAllAttrNames(); break;
      case 'o': /*#o*/ ZoomOut(); break;
      case 'p': /*#p*/ ImportFile(); break;
      case 'q': /*#q*/ SetCurChoice(DRAWPOLY); break;
      case 'r': /*#r*/ SetCurChoice(DRAWBOX); break;
      case 's': /*#s*/ return SolveProc();
      case 't': /*#t*/ DetachAttrs(); break;
      case 'u': /*#u*/ return AnimateProc(); break;
      case 'v': /*#v*/ FlipVertical(); break;
      case 'w': /*#w*/ RotateClockWise(); break;
      case 'x': /*#x*/ return EscapeProc();
      case 'y': /*#y*/ return SimulateProc();
      case 'z': /*#z*/ ZoomIn(); break;
      case '9': /*#9*/ MakePreciseArc(); break;
      case '0': /*#0*/ UpdateSelObjs(); break;
      case ',': /*#,*/ ScrollUp(NULL); break;
      case '.': /*#.*/ ScrollDown(NULL); break;
      case '-': /*#-*/ ShowAllAttrs(); break;
      case '{': /*#{*/ AlignObjsTop(); break;
      case '+': /*#+*/ AlignObjsMiddle(); break;
      case '}': /*#}*/ AlignObjsBottom(); break;
      case '[': /*#[*/ AlignObjsLeft(); break;
      case '=': /*#=*/ AlignObjsCenter(); break;
      case ']': /*#]*/ AlignObjsRight(); break;
      case '"': /*#"*/ MakeRegularPolygon(); break;
      case '%': /*#%*/ SetPrintReduction(); break;
      case ':': /*#:*/ DefaultZoom(); break;
      case '`': /*#`*/ ZoomWayOut(); break;
      case '~': /*#~*/ SaveNewFile(TRUE, NULL); break;
      case ';': /*#;*/ CutMaps(); break;
      case '_': /*#_*/ AbutHorizontal(); break;
      case '|': /*#|*/ AbutVertical(); break;
      case '#': /*##*/
         BreakUpText((args==NULL || *args=='\0') ? NULL : args);
         break;
      case '^': /*#^*/ ScrollToOrigin(); break;
      case '@': /*#@*/ ToggleMoveMode(); break;
      case '$': /*#$*/ SetCurChoice(VERTEXMODE); break;
      case '&': /*#&*/ AlignSelToPage(); break;
      case '*': /*#**/ ChangeDomain(); break;
      case '(': /*#(*/
         ImportEPSFile(FALSE, (args==NULL || *args=='\0') ? NULL : args);
         break;
      case ')': /*#)*/ ScaleAllSelObj(); break;
      case '<': /*#<*/ LockSelObj(); break;
      case '>': /*#>*/ UnlockSelObj(); break;
      default: return INVALID;
      }
   } else if ((state & METAMASK) && (state & ControlMask)) {
      switch (key_sym&0xff) {
      case 'a': /*^#a*/ AddPoint(); break;
      case 'b': /*^#b*/ ChangeFontStyle(STYLE_BR); break;
      case 'c': /*^#c*/ ChangeFontJust(JUST_C); break;
      case 'd': /*^#d*/ DeletePoint(); break;
      case 'e': /*^#e*/ SetCurChoice(DRAWRCBOX); break;
      case 'f': /*^#f*/ InvertXBitmaps(); break;
      case 'g': /*^#g*/ ToggleSnapOn(); break;
      case 'h': /*^#h*/ HideAllAttrs(); break;
      case 'i': /*^#i*/ MakeIconic(NULL, TRUE); break;
      case 'j': /*^#j*/ UnMakeIconic(); break;
      case 'k': /*^#k*/ ToggleColorPostScript(); break;
      case 'l': /*^#l*/ ChangeFontJust(JUST_L); break;
      case 'm': /*^#m*/ MakeSymbolic(); break;
      case 'n': /*^#n*/ UnMakeSymbolic(); break;
      case 'o': /*^#o*/ ChangeFontStyle(STYLE_NR); break;
      case 'p': /*^#p*/ ChangeFontStyle(STYLE_BI); break;
      case 'q': /*^#q*/ SetCurChoice(DRAWPOLYGON); break;
      case 'r': /*^#r*/ ChangeFontJust(JUST_R); break;
      case 's': /*^#s*/
         SaveNewFile(FALSE, (args==NULL || *args=='\0') ? NULL : args);
         break;
      case 't': /*^#t*/ ChangeFontStyle(STYLE_NI); break;
      case 'u': /*^#u*/ UpdateSymbols(); break;
      case 'v': /*^#v*/ SetCurChoice(DRAWCORNEROVAL); break;
      case 'w': /*^#w*/ ToggleAllSelLineType(); break;
      case 'x': /*^#x*/ ToggleWhereToPrint(); break;
      case 'y': /*^#y*/ PushIcon(); break;
      case 'Y': /*^#Y*/ PasteCompoundText(); break;
      case 'z': /*^#z*/ SetCurChoice(DRAWARC); break;
      case '.': /*^#.*/ ImportXBitmapFile(); break;
      case ',': /*^#,*/ ImportXPixmapFile(); break;
      case '-': /*^#-*/ ToggleGridSystem(); break;
      case '=': /*^#=*/ FindAgain(); break;
      case '5': /*^#5*/ InsertRightSubscript(); break;
      case '6': /*^#6*/ InsertRightSuperscript(); break;
      case '7': /*^#7*/ ToggleEqAttrShown(); break;
      case '8': /*^#8*/
         if (inSlideShow) {
            LeaveSlideShow();
         } else {
            EnterSlideShow();
         }
         break;
      case '9': /*^#9*/ FindNoCase(); break;
      case '0': /*^#0*/ FindCaseSensitive(); break;
      default: return INVALID;
      }
   } else if (name != NULL && key_sym == '\0' && state == 0) {
      XButtonEvent button_ev;

      button_ev.state = ShiftMask;

      /* do not translate -- program constants */
      if (strcmp(name, "Delete()") == 0) {
         if (curChoice == DRAWTEXT && textCursorShown) {
            DelSelText();
         } else {
            DelAllSelObj();
         }
      } else if (strcmp(name, "DrawCornerOval()") == 0) {
         SetCurChoice(DRAWCORNEROVAL);
      } else if (strcmp(name, "DrawCenterOval()") == 0) {
         SetCurChoice(DRAWCENTEROVAL);
      } else if (strcmp(name, "DrawEdgeOval()") == 0) {
         SetCurChoice(DRAWEDGECIRCLE);
      } else if (strcmp(name, "DrawEdgeArc()") == 0) {
         SetCurChoice(DRAWEDGEARC);
      } else if (strcmp(name, "ScrollPageUp()") == 0) {
         ScrollUp(&button_ev);
      } else if (strcmp(name, "ScrollPageDown()") == 0) {
         ScrollDown(&button_ev);
      } else if (strcmp(name, "ScrollPageRight()") == 0) {
         ScrollRight(&button_ev);
      } else if (strcmp(name, "ScrollPageLeft()") == 0) {
         ScrollLeft(&button_ev);
      } else if (strcmp(name, "FlushUndoBuffer()") == 0) {
         FlushUndoBuffer();
      } else if (strcmp(name, "PrintMsgBuffer()") == 0) {
         PrintMsgBuffer();
      } else if (strcmp(name, "SaveOrigin()") == 0) {
         SaveOrigin();
      } else if (strcmp(name, "RestoreImageWH()") == 0) {
         RestoreImageWH();
      } else if (strcmp(name, "UpdateEPS()") == 0) {
         UpdateEPS();
      } else if (strcmp(name, "ToggleMapShown()") == 0) {
         ToggleMapShown();
      } else if (strcmp(name, "ToggleUseGrayScale()") == 0) {
         ToggleUseGray();
      } else if (strcmp(name, "FreeHandMode()") == 0) {
         SetCurChoice(FREEHAND);
      } else if (strcmp(name, "SaveSymInLibrary()") == 0) {
         SaveSymInLibrary();
      } else if (strcmp(name, "CenterAnEndPoint()") == 0 ||
            strcmp(name, "CenterAVertex()") == 0) {
         CenterAnEndPoint();
      } else if (strcmp(name, "NextPage()") == 0) {
         NextPage();
      } else if (strcmp(name, "PrevPage()") == 0) {
         PrevPage();
      } else if (strcmp(name, "NamePages()") == 0) {
         NamePages();
      } else if (strcmp(name, "GotoPage()") == 0) {
         GotoPage((args==NULL || *args=='\0') ? NULL : args);
      } else if (strcmp(name, "AddPageBefore()") == 0) {
         AddPageBefore();
      } else if (strcmp(name, "AddPageAfter()") == 0) {
         AddPageAfter();
      } else if (strcmp(name, "DeleteCurPage()") == 0) {
         DeleteCurPage();
      } else if (strcmp(name, "TogglePageLineShown()") == 0) {
         TogglePageLineShown();
      } else if (strcmp(name, "SpecifyDrawingSize()") == 0) {
         SpecifyDrawingSize();
      } else if (strcmp(name, "PrintOnePage()") == 0) {
         PrintOnePage();
      } else if (strcmp(name, "ToggleNamedAttrShown()") == 0) {
         if (args != NULL) {
            ToggleNamedAttrShown(args);
         }
      } else if (strcmp(name, "AttachFileAttrs()") == 0) {
         AddFileAttrs();
      } else if (strcmp(name, "DetachFileAttrs()") == 0) {
         DetachFileAttrs();
      } else if (strcmp(name, "EditFileAttrs()") == 0) {
         EditFileAttrs();
      } else if (strcmp(name, "PrintSelectedObjs()") == 0) {
         PrintSelectedObjs();
      } else if (strcmp(name, "InputPolyPts()") == 0) {
         InputPolyPts();
      } else if (strcmp(name, "InputPolygonPts()") == 0) {
         InputPolygonPts();
      } else if (strcmp(name, "EditAttrs()") == 0) {
         EditAttrs();
      } else if (strcmp(name, "ConvertIntSpline()") == 0) {
         ConvertIntSpline();
      } else if (strcmp(name, "PasteFromFile()") == 0) {
         PasteFromFile();
      } else if (strcmp(name, "ToggleShowMeasurement()") == 0) {
         ToggleShowMeasurement();
      } else if (strcmp(name, "SetMeasureUnit()") == 0) {
         SetMeasureUnit();
      } else if (strcmp(name, "Cut()") == 0) {
         CutToCutBuffer();
      } else if (strcmp(name, "ToggleSmoothHinge()") == 0) {
         ToggleSmoothHinge();
      } else if (strcmp(name, "ToggleShowMenubar()") == 0) {
         ToggleShowMenubar();
      } else if (strcmp(name, "ToggleShowStatus()") == 0) {
         ToggleShowStatus();
      } else if (strcmp(name, "BrowseXBitmap()") == 0) {
         BrowseXBitmap();
      } else if (strcmp(name, "BrowseXPixmap()") == 0) {
         BrowseXPixmap();
      } else if (strcmp(name, "SpecifyPaperSize()") == 0) {
         SpecifyPaperSize();
      } else if (strcmp(name, "ToggleOneMotionSelMove()") == 0) {
         ToggleOneMotionSelectMove();
      } else if (strcmp(name, "GoBack()") == 0) {
         NavigateBack();
      } else if (strcmp(name, "GoForward()") == 0) {
         NavigateForward();
      } else if (strcmp(name, "RefreshCurrent()") == 0) {
         NavigateRefresh();
      } else if (strcmp(name, "HotList()") == 0) {
         NavigateHotList();
      } else if (strcmp(name, "AddCurrentToHotList()") == 0) {
         NavigateAddToHotList();
      } else if (strcmp(name, "SessionHistory()") == 0) {
         NavigateSessionHistory();
      } else if (strcmp(name, "ToggleHyperSpace()") == 0) {
         ToggleHyperSpace (FALSE);
      } else if (strcmp(name, "EmbedEPSFile()") == 0) {
         ImportEPSFile(TRUE, NULL);
      } else if (strcmp(name, "SetSelLineWidth()") == 0) {
         SetSelLineWidth(NULL);
      } else if (strcmp(name, "AddColor()") == 0) {
         AddColor();
      } else if (strcmp(name, "ImportAttrs()") == 0) {
         ImportAttrs();
      } else if (strcmp(name, "ExportAttrs()") == 0) {
         ExportAttrs();
      } else if (strcmp(name, "MergeWithTable()") == 0) {
         MergeWithTable();
      } else if (strcmp(name, "ExportToTable()") == 0) {
         ExportToTable();
      } else if (strcmp(name, "DeletePages()") == 0) {
         DeletePages();
      } else if (strcmp(name, "PrintOneFilePerPage()") == 0) {
         PrintOneFilePerPage();
      } else if (strcmp(name, "ImportGIFFile()") == 0) {
         ImportGIFFile();
      } else if (strcmp(name, "ImportPNGFile()") == 0) {
         ImportPNGFile();
      } else if (strcmp(name, "ImportJPEGFile()") == 0) {
         ImportJPEGFile(TRUE, NULL);
      } else if (strcmp(name, "ImportPBMFile()") == 0) {
         ImportPBMFile();
      } else if (strcmp(name, "ImportPGMFile()") == 0) {
         ImportPGMFile();
      } else if (strcmp(name, "ImportPPMFile()") == 0) {
         ImportPPMFile();
      } else if (strcmp(name, "SetExportPixelTrim()") == 0) {
         SetExportPixelTrim(FALSE);
      } else if (strcmp(name, "ToggleColorLayers()") == 0) {
         ToggleColorLayers();
      } else if (strcmp(name, "ToggleStretchableText()") == 0) {
         ToggleStretchableText();
      } else if (strcmp(name, "BreakUpBit/Pixmap()") == 0) {
         BreakUpMaps();
      } else if (strcmp(name, "LayoutOnArc()") == 0) {
         LayoutOnArc();
      } else if (strcmp(name, "PreciseRotate()") == 0) {
         PreciseRotate();
      } else if (strcmp(name, "JoinPoly()") == 0) {
         JoinPoly();
      } else if (strcmp(name, "CutPoly()") == 0) {
         CutPoly();
      } else if (strcmp(name, "GetBoundingBox()") == 0) {
         GetBoundingBox();
      } else if (strcmp(name, "SetTemplate()") == 0) {
         SetTemplate();
      } else if (strcmp(name, "MakeGray()") == 0) {
         MakeGray();
      } else if (strcmp(name, "InvertColor()") == 0) {
         InvertColor();
      } else if (strcmp(name, "InterpolateColor()") == 0) {
         InterpolateColor();
      } else if (strcmp(name, "BrightenDarken()") == 0) {
         BrightenDarken();
      } else if (strcmp(name, "ChangeSaturation()") == 0) {
         ChangeSaturation();
      } else if (strcmp(name, "ChangeHue()") == 0) {
         ChangeHue();
      } else if (strcmp(name, "ContrastEnhance()") == 0) {
         ContrastEnhance();
      } else if (strcmp(name, "ColorBalance()") == 0) {
         ColorBalance();
      } else if (strcmp(name, "Gamma()") == 0) {
         Gamma(NULL);
      } else if (strcmp(name, "EdgeDetect()") == 0) {
         EdgeDetect();
      } else if (strcmp(name, "Emboss()") == 0) {
         Emboss();
      } else if (strcmp(name, "ReduceColors()") == 0) {
         ReduceColors();
      } else if (strcmp(name, "ReduceToPixmapColors()") == 0) {
         ReduceToPixmapColors();
      } else if (strcmp(name, "SetDefaultColorLevels()") == 0) {
         SetDefaultColorLevels();
      } else if (strcmp(name, "ReduceToDefaultColors()") == 0) {
         ReduceToDefaultColors();
      } else if (strcmp(name, "DefaultErrorDiffuse()") == 0) {
         DefaultErrorDiffuse();
      } else if (strcmp(name, "Spread()") == 0) {
         Spread();
      } else if (strcmp(name, "Sharpen()") == 0) {
         Sharpen();
      } else if (strcmp(name, "Blur3()") == 0) {
         Blur3();
      } else if (strcmp(name, "Blur5()") == 0) {
         Blur5();
      } else if (strcmp(name, "Blur7()") == 0) {
         Blur7();
      } else if (strcmp(name, "RunBggen()") == 0) {
         RunBggen();
      } else if (strcmp(name, "CircularBggen()") == 0) {
         CircularBggen();
      } else if (strcmp(name, "SimpleRectBggen()") == 0) {
         SimpleRectBggen();
      } else if (strcmp(name, "RegenerateImage()") == 0) {
         RegenerateImage();
      } else if (strcmp(name, "CropImage()") == 0) {
         CropImage();
      } else if (strcmp(name, "GetColor()") == 0) {
         GetColor();
      } else if (strcmp(name, "ReplaceColor()") == 0) {
         ReplaceColor();
      } else if (strcmp(name, "FloodFill()") == 0) {
         FloodFill();
      } else if (strcmp(name, "CreateContour()") == 0) {
         CreateContour();
      } else if (strcmp(name, "Subtract()") == 0) {
         Subtract();
      } else if (strcmp(name, "AlphaCombine()") == 0) {
         AlphaCombine();
      } else if (strcmp(name, "XorColors()") == 0) {
         XorColors();
      } else if (strcmp(name, "ImportOtherFile()") == 0) {
         ImportOtherFile();
      } else if (strcmp(name, "ImportOtherFileType()") == 0) {
         if (args != NULL) {
            ImportOtherFileType(args);
         }
      } else if (strcmp(name, "BrowseOther()") == 0) {
         BrowseOther();
      } else if (strcmp(name, "BrowseOtherType()") == 0) {
         if (args != NULL) {
            BrowseOtherType(args);
         }
      } else if (strcmp(name, "ToggleShowCrossHair()") == 0) {
         ToggleShowCrossHair();
      } else if (strcmp(name, "SetShapeShadow()") == 0) {
         SetShapeShadow();
      } else if (strcmp(name, "NoTransform()") == 0) {
         NoTransform();
      } else if (strcmp(name, "About()") == 0) {
         About();
      } else if (strcmp(name, "Copyright()") == 0) {
         Copyright();
      } else if (strcmp(name, "SetSelFontSize()") == 0) {
         SetSelFontSize(NULL);
      } else if (strcmp(name, "ZoomInAtCursor()") == 0) {
         if (key_ev != NULL) {
            int abs_x=ABS_X(key_ev->x);
            int abs_y=ABS_Y(key_ev->y);

            ZoomInAtCursor(abs_x, abs_y);
         }
      } else if (strcmp(name, "CenterAtCursor()") == 0) {
         if (key_ev != NULL) {
            int abs_x=ABS_X(key_ev->x);
            int abs_y=ABS_Y(key_ev->y);

            CenterAtCursor(abs_x, abs_y);
         }
      } else if (strcmp(name, "SetEditTextSize()") == 0) {
         SetEditTextSize();
      } else if (strcmp(name, "SetTextRotation()") == 0) {
         SetTextRotation(NULL);
      } else if (strcmp(name, "SetRotationIncrement()") == 0) {
         SetRotationIncrement(NULL);
      } else if (strcmp(name, "CurrentVersionInfo()") == 0 ||
            strcmp(name, "LatestReleaseInfo()") == 0) {
         LatestReleaseInfo();
      } else if (strcmp(name, "VectorWarp()") == 0) {
         VectorWarp();
      } else if (strcmp(name, "ConnectPins()") == 0) {
         ConnectPins();
      } else if (strcmp(name, "PasteCompoundText()") == 0) {
         PasteCompoundText();
      } else if (strcmp(name, "CopyProperties()") == 0) {
         CopyProperties(TRUE);
      } else if (strcmp(name, "SaveProperties()") == 0) {
         SaveProperties();
      } else if (strcmp(name, "PasteProperties()") == 0) {
         PasteProperties(TRUE);
      } else if (strcmp(name, "RestoreProperties()") == 0) {
         RestoreProperties();
      } else if (strcmp(name, "RotateShearMode()") == 0) {
         SetCurChoice(ROTATEMODE);
      } else if (strcmp(name, "ChangeAllSelFill()") == 0) {
         if (args != NULL) {
            ChangeAllSelFill(atoi(args), TRUE);
         }
      } else if (strcmp(name, "ChangeAllSelPen()") == 0) {
         if (args != NULL) {
            ChangeAllSelPen(atoi(args), TRUE);
         }
      } else if (strcmp(name, "ChangeAllSelLineWidth()") == 0) {
         if (args != NULL) {
            ChangeAllSelLineWidth(atoi(args), TRUE);
         }
      } else if (strcmp(name, "ChangeAllSelLineStyle()") == 0) {
         if (args != NULL) {
            ChangeAllSelLineStyle(atoi(args), TRUE);
         }
      } else if (strcmp(name, "ChangeAllSelLineType()") == 0) {
         if (args != NULL) {
            ChangeAllSelLineType(atoi(args), TRUE);
         }
      } else if (strcmp(name, "ChangeAllSelLineDash()") == 0) {
         if (args != NULL) {
            ChangeAllSelLineDash(atoi(args), TRUE);
         }
      } else if (strcmp(name, "ChangeAllSelFont()") == 0) {
         if (args != NULL) {
            ChangeFont(atoi(args), FALSE);
         }
      } else if (strcmp(name, "ChangeAllSelFontSize()") == 0) {
         if (args != NULL) {
            ChangeFontSize(atoi(args));
         }
      } else if (strcmp(name, "ChangeAllSelFontStyle()") == 0) {
         if (args != NULL) {
            ChangeFontStyle(atoi(args));
         }
      } else if (strcmp(name, "ChangeAllSelFontJust()") == 0) {
         if (args != NULL) {
            ChangeFontJust(atoi(args)-MAXFONTSTYLES-1);
         }
      } else if (strcmp(name, "ChangeAllSelColor()") == 0) {
         if (args != NULL) {
            ChangeAllSelColor(atoi(args), TRUE);
         }
      } else if (strcmp(name, "LanscapeMode()") == 0) {
         ChangePageStyle(LANDSCAPE);
      } else if (strcmp(name, "PortraitMode()") == 0) {
         ChangePageStyle(PORTRAIT);
      } else if (strcmp(name, "SetWhereToPrint()") == 0) {
         if (args != NULL) {
            SetWhereToPrint(atoi(args));
         }
      } else if (strcmp(name, "SetHoriAlign()") == 0) {
         if (args != NULL) {
            HoriAlignSubMenu(atoi(args));
         }
      } else if (strcmp(name, "SetVertAlign()") == 0) {
         if (args != NULL) {
            VertAlignSubMenu(atoi(args));
         }
      } else if (strcmp(name, "SetMoveMode()") == 0) {
         if (args != NULL) {
            MoveModeSubMenu(atoi(args));
         }
      } else if (strcmp(name, "SetStretchTextMode()") == 0) {
         if (args != NULL) {
            StretchableTextModeSubMenu(atoi(args));
         }
      } else if (strcmp(name, "CreateShape()") == 0) {
         if (args != NULL) {
            ShapeSubMenu(atoi(args));
         }
      } else if (strcmp(name, "SetPageLayoutMode()") == 0) {
         if (args != NULL) {
            PageLayoutSubMenu(atoi(args));
         }
      } else if (strcmp(name, "SetTransPatMode()") == 0) {
         if (args != NULL) {
            ChangeAllSelTransPat(atoi(args), TRUE);
         }
      } else if (strcmp(name, "ToggleShowMode()") == 0) {
         ToggleShowMode();
      } else if (strcmp(name, "SetSlideShowBorderColor()") == 0) {
         SetSlideShowBorderColor();
      } else if (strcmp(name, "SetSlideShowWindowOffsets()") == 0) {
         SetSlideShowWindowOffsets();
      } else if (strcmp(name, "ExportXPixmapDeckToGIF()") == 0) {
         ExportXPixmapDeckToGIF();
      } else if (strcmp(name, "ImportGIFToXPixmapDeck()") == 0) {
         ImportGIFToXPixmapDeck();
      } else if (strcmp(name, "InsertThinSpace()") == 0) {
         InsertThinSpace();
      } else if (strcmp(name, "InsertVerticalOffset()") == 0) {
         InsertVerticalOffset();
      } else if (strcmp(name, "RemoveVerticalOffset()") == 0) {
         RemoveVerticalOffset();
      } else if (strcmp(name, "InsertLeftSuperscript()") == 0) {
         InsertLeftSuperscript();
      } else if (strcmp(name, "InsertLeftSubscript()") == 0) {
         InsertLeftSubscript();
      } else if (strcmp(name, "InsertCenterSuperscript()") == 0) {
         InsertCenterSuperscript();
      } else if (strcmp(name, "InsertCenterSubscript()") == 0) {
         InsertCenterSubscript();
      } else if (strcmp(name, "SetScriptFraction()") == 0) {
         SetScriptFraction();
      } else if (strcmp(name, "FakeUserAgent()") == 0) {
         if (args != NULL) {
            FakeUserAgent(args);
         }
      } else if (strcmp(name, "FakeReferer()") == 0) {
         if (args != NULL) {
            FakeReferer(args);
         }
      } else if (strcmp(name, "ToggleKeepAlive()") == 0) {
         ToggleKeepAlive();
      } else if (strcmp(name, "SizeToWidest()") == 0) {
         SizeToWidest();
      } else if (strcmp(name, "SizeToNarrowest()") == 0) {
         SizeToNarrowest();
      } else if (strcmp(name, "SizeToTallest()") == 0) {
         SizeToTallest();
      } else if (strcmp(name, "SizeToShortest()") == 0) {
         SizeToShortest();
      } else if (strcmp(name, "SizeToGivenWidthHeight()") == 0) {
         SizeToGivenWidthHeight();
      } else if (strcmp(name, "SizeToGivenWidth()") == 0) {
         SizeToGivenWidth();
      } else if (strcmp(name, "SizeToGivenHeight()") == 0) {
         SizeToGivenHeight();
      } else if (strcmp(name, "ExecCmdsFromFile()") == 0) {
         if (args != NULL) {
            ExecCmdsFromFile(args);
         }
      } else if (strcmp(name, "StartExecCmdsFromFile()") == 0) {
         StartExecCmdsFromFile();
      } else if (strcmp(name, "CopyPlainTextAsObject()") == 0) {
         CopyPlainTextAsObject();
      } else if (strcmp(name, "SetTextFillPatternColor()") == 0) {
         SetTextFillPatternColor();
      } else if (strcmp(name, "AlignDirect()") == 0) {
         if (args != NULL) {
            AlignDirect(atoi(args));
         }
      } else if (strcmp(name, "DistributeDirect()") == 0) {
         if (args != NULL) {
            DistributeDirect(atoi(args));
         }
      } else if (strcmp(name, "ToggleVisibleGridInSlideShow()") == 0) {
         ToggleVisibleGridInSlideShow();
      } else if (strcmp(name, "ChangeScrollMode()") == 0) {
         if (args != NULL) {
            ChangeScrollMode(atoi(args));
         }
      } else if (strcmp(name, "SetAltEditTextBgColor()") == 0) {
         SetAltEditTextBgColor();
      } else if (strcmp(name, "SetAltEditTextHighlightColor()") == 0) {
         SetAltEditTextHighlightColor();
      } else if (strcmp(name, "ToggleAltEditTextBgColor()") == 0) {
         ToggleAltEditTextBgColor();
      } else if (strcmp(name, "ChangeAllSelFontUnderline()") == 0) {
         if (args != NULL) {
            ChangeFontUnderline(atoi(args)-MAXFONTSTYLES-MAXJUSTS-2);
         }
      } else if (strcmp(name, "ChangeAllSelFontOverline()") == 0) {
         if (args != NULL) {
            ChangeFontOverline(atoi(args)-MAXFONTSTYLES-MAXJUSTS-5);
         }
      } else if (strcmp(name, "EditPageFileNames()") == 0) {
         EditPageFileNames();
      } else if (strcmp(name, "ExportHalfToneBitmap()") == 0) {
         ExportHalfToneBitmap();
      } else if (strcmp(name, "ExportThresholdBitmap()") == 0) {
         ExportThresholdBitmap();
      } else if (strcmp(name, "SetExportBitmapThreshold()") == 0) {
         if (args != NULL) {
            SetExportBitmapThreshold(NULL);
         }
      } else if (strcmp(name, "PreciseScaleEverything()") == 0) {
         PreciseScaleEverything();
      } else if (strcmp(name, "SetPaperColor()") == 0) {
         SetPaperColor();
      } else if (strcmp(name, "DelayedFullScreenCapture()") == 0) {
         DelayedFullScreenCapture();
      } else if (strcmp(name, "ToggleHideDuringCapture()") == 0) {
         ToggleHideDuringCapture();
      } else if (strcmp(name, "EditDomainPaths()") == 0) {
         EditDomainPaths();
      } else if (strcmp(name, "SelectDefaultDomain()") == 0) {
         SelectDefaultDomain();
      } else if (strcmp(name, "AddADomain()") == 0) {
         AddADomain();
      } else if (strcmp(name, "DeleteADomain()") == 0) {
         DeleteADomain();
      } else if (strcmp(name, "ReloadDomainInfoFromX()") == 0) {
         ReloadDomainInfoFromX();
      } else if (strcmp(name, "EditIndexedAttrInEditor()") == 0) {
         if (args != NULL) {
            EditIndexedAttrInEditor(atoi(args));
         }
      } else if (strcmp(name, "EditIndexedAttrGroupInEditor()") == 0) {
         if (args != NULL) {
            EditIndexedAttrGroupInEditor(atoi(args));
         }
      } else if (strcmp(name, "GetProperty()") == 0) {
         if (args != NULL) {
            GetProperty(atoi(args));
         }
      } else if (strcmp(name, "PeekDimension()") == 0) {
         if (args != NULL) {
            PeekDimension(atoi(args));
         }
      } else if (strcmp(name, "SetHtmlExportTemplate()") == 0) {
         SetHtmlExportTemplate();
      } else if (strcmp(name, "PrintPages()") == 0) {
         PrintPages();
      } else if (strcmp(name, "GoHyperSpaceInSlideShow()") == 0) {
         GoHyperSpaceInSlideShow();
      } else if (strcmp(name, "FreehandModeInSlideShow()") == 0) {
         FreehandModeInSlideShow();
      } else if (strcmp(name, "OpenARecentlyUsedFile()") == 0) {
         if (args != NULL) {
            OpenARecentlyUsedFile(atoi(args));
         }
      } else if (strcmp(name, "MoveEditTextBox()") == 0) {
         MoveEditTextBox();
      } else if (strcmp(name, "ReplaceGraphic()") == 0) {
         ReplaceGraphic();
      } else if (strcmp(name, "ToggleShowMeasurementInTooltip()") == 0) {
         ToggleShowMeasurementInTooltip();
      } else if (strcmp(name, "ToggleAutoEPSPreviewBitmap()") == 0) {
         ToggleAutoEPSPreviewBitmap();
      } else if (strcmp(name, "CreateThumbnails()") == 0) {
         CreateThumbnails();
      } else if (strcmp(name, "ConnectTwoPortsByAWire()") == 0) {
         ConnectTwoPortsByAWire();
      } else if (strcmp(name, "RenameSignalNameForAPort()") == 0) {
         RenameSignalNameForAPort();
      } else if (strcmp(name, "ClearSignalNameForAPort()") == 0) {
         ClearSignalNameForAPort();
      } else if (strcmp(name, "ToggleShowWireSignalName()") == 0) {
         ToggleShowWireSignalName();
      } else if (strcmp(name, "ToggleShowChoicebar()") == 0) {
         ToggleShowChoicebar();
      } else if (strcmp(name, "MergePortsWithAnObject()") == 0) {
         MergePortsWithAnObject();
      } else if (strcmp(name, "RenumberObjectIds()") == 0) {
         RenumberObjectIds();
      } else if (strcmp(name, "RepeatConnectTwoPortsByAWire()") == 0) {
         RepeatConnectTwoPortsByAWire();
      } else if (strcmp(name, "ConnectPortsToBroadcastWire()") == 0) {
         ConnectPortsToBroadcastWire();
      } else if (strcmp(name, "ImportMultipageTextFile()") == 0) {
         ImportMultipageTextFile();
      } else if (strcmp(name, "SetMarginsForImportMultipageTextFile()") == 0) {
         SetMarginsForImportMultipageTextFile();
      } else if (strcmp(name,
            "ToggleWordWrapDuringImportMultipageTextFile()") == 0) {
         ToggleWordWrapDuringImportMultipageTextFile();
      } else if (strcmp(name, "HandleDataInMBuff()") == 0) {
         HandleDataInMBuff();
      } else if (strcmp(name, "BenchMark()") == 0) {
         BenchMark();
      } else if (strcmp(name, "ConvertToBezier()") == 0) {
         ConvertToBezier();
      } else if (strcmp(name, "SetBezierConvertNumSegs()") == 0) {
         SetBezierConvertNumSegs((args==NULL || *args=='\0') ? NULL : args);
      } else if (strcmp(name, "AddSquareTickMarks()") == 0) {
         AddTickMarks(CMDID_ADDSQUARETICKMARKS);
      } else if (strcmp(name, "AddTriangleTickMarks()") == 0) {
         AddTickMarks(CMDID_ADDTRIANGLETICKMARKS);
      } else if (strcmp(name, "AddCircleTickMarks()") == 0) {
         AddTickMarks(CMDID_ADDCIRCLETICKMARKS);
      } else if (strcmp(name, "AddXTickMarks()") == 0) {
         AddTickMarks(CMDID_ADDXTICKMARKS);
      } else if (strcmp(name, "AddDiamondTickMarks()") == 0) {
         AddTickMarks(CMDID_ADDDIAMONDTICKMARKS);
      } else if (strcmp(name, "AddBowtieTickMarks()") == 0) {
         AddTickMarks(CMDID_ADDBOWTIETICKMARKS);
      } else if (strcmp(name, "AddInvTriangleTickMarks()") == 0) {
         AddTickMarks(CMDID_ADDINVTRIANGLETICKMARKS);
      } else if (strcmp(name, "AddPlusTickMarks()") == 0) {
         AddTickMarks(CMDID_ADDPLUSTICKMARKS);
      } else if (strcmp(name, "AddHourGlassTickMarks()") == 0) {
         AddTickMarks(CMDID_ADDHOURGLASSTICKMARKS);
      } else if (strcmp(name, "SetTickMarkSize()") == 0) {
         SetTickMarkSize((args==NULL || *args=='\0') ? NULL : args);
      } else if (strcmp(name, "ToggleShowChat()") == 0) {
         ToggleShowChat();
      } else if (strcmp(name, "SavePagesAs()") == 0) {
         SavePagesAs();
      } else if (strcmp(name, "AddPageBeforeCopyAll()") == 0) {
         AddPageBeforeCopyAll();
      } else if (strcmp(name, "AddPageAfterCopyAll()") == 0) {
         AddPageAfterCopyAll();
      } else if (strcmp(name, "InsertHexOctalChar()") == 0) {
         InsertHexOctalChar();
      } else if (strcmp(name, "ResetInputMethod()") == 0) {
         ResetInputMethod();
      } else if (strcmp(name, "LinkExtJPEGFile()") == 0) {
         ImportJPEGFile(FALSE, NULL);
      } else if (strcmp(name, "NextSlide()") == 0) {
         NextSlide();
      } else if (strcmp(name, "PrevSlide()") == 0) {
         PrevSlide();
      } else if (strcmp(name, "SetObjectShadowColor()") == 0) {
         SetObjectShadowColor();
      } else if (strcmp(name, "SetObjectShadowOffsets()") == 0) {
         SetObjectShadowOffsets();
      } else if (strcmp(name, "AddObjectShadow()") == 0) {
         AddObjectShadow();
      } else if (strcmp(name, "RemoveObjectShadow()") == 0) {
         RemoveObjectShadow();
      } else if (strcmp(name, "CopyDoubleByteString()") == 0) {
         CopyDoubleByteString();
      } else if (strcmp(name, "PasteDoubleByteString()") == 0) {
         PasteDoubleByteString();
      } else if (strcmp(name, "ReduceToMobileWebSafeColors()") == 0) {
         ReduceToMobileWebSafeColors();
      } else if (strcmp(name, "CreatePixmapFromSelected()") == 0) {
         CreatePixmapFromSelected();
      } else if (strcmp(name, "ToggleAutoRotatePivot()") == 0) {
         ToggleAutoRotatePivot();
      } else if (strcmp(name, "SpecifyRotatePivot()") == 0) {
         SpecifyRotatePivot();
      } else if (strcmp(name, "ResetRotatePivot()") == 0) {
         ResetRotatePivot();
      } else if (strcmp(name, "NextPolyRotationPivot()") == 0) {
         NextPolyRotationPivot();
      } else if (strcmp(name, "MoveRotationPivotToArcCenter()") == 0) {
         MoveRotationPivotToArcCenter();
      } else if (strcmp(name, "MoveRotatePivotCenter()") == 0) {
         MoveRotatePivot(CORNER_NONE);
      } else if (strcmp(name, "MoveRotatePivotLeftTop()") == 0) {
         MoveRotatePivot(CORNER_LT);
      } else if (strcmp(name, "MoveRotatePivotRightTop()") == 0) {
         MoveRotatePivot(CORNER_RT);
      } else if (strcmp(name, "MoveRotatePivotLeftBottom()") == 0) {
         MoveRotatePivot(CORNER_LB);
      } else if (strcmp(name, "MoveRotatePivotRightBottom()") == 0) {
         MoveRotatePivot(CORNER_RB);
      } else if (strcmp(name, "MoveRotatePivotLeft()") == 0) {
         MoveRotatePivot(CORNER_LEFT);
      } else if (strcmp(name, "MoveRotatePivotRight()") == 0) {
         MoveRotatePivot(CORNER_RIGHT);
      } else if (strcmp(name, "MoveRotatePivotTop()") == 0) {
         MoveRotatePivot(CORNER_TOP);
      } else if (strcmp(name, "MoveRotatePivotBottom()") == 0) {
         MoveRotatePivot(CORNER_BOTTOM);
      } else if (strcmp(name, "ChooseRotatePivot()") == 0) {
         if (args != NULL) {
            AutoRotatePivotSubMenu(atoi(args));
         }
      } else if (strcmp(name, "ChooseColor()") == 0) {
         ChooseColor();
      } else if (strcmp(name, "ExtendSegment()") == 0) {
         ExtendSegment();
      } else if (strcmp(name, "InsertSymbol()") == 0) {
         InsertSymbol();
      } else if (strcmp(name, "ToggleRightMargin()") == 0) {
         ToggleRightMargin();
      } else if (strcmp(name, "SpecifyRightMargin()") == 0) {
         SpecifyRightMargin();
      } else if (strcmp(name, "ToggleFloodReplaceColorThreshold()") == 0) {
         ToggleFloodReplaceColorThreshold();
      } else if (strcmp(name, "SetFloodReplaceColorThreshold()") == 0) {
         SetFloodReplaceColorThreshold();
      } else if (strcmp(name, "RemoveTransparentPixel()") == 0) {
         RemoveTransparentPixel();
      } else if (strcmp(name, "ReplaceColorWithTrans()") == 0) {
         ReplaceColorWithTrans();
#ifdef NOT_DEFINED
      } else if (strcmp(name, "ToggleTighterStructuredSplines()") == 0) {
         ToggleTighterStructuredSplines();
#endif /* NOT_DEFINED */
      } else if (strcmp(name, "MakeBoxObjFromBoundingBox()") == 0) {
         MakeBoxObjFromBoundingBox();
      } else if (strcmp(name, "MakeRCBoxObjFromBoundingBox()") == 0) {
         MakeRCBoxObjFromBoundingBox();
      } else if (strcmp(name, "MakeOvalObjFromBoundingBox()") == 0) {
         MakeOvalObjFromBoundingBox();
      }
   }
   return BAD;
}

int ShortHand(input)
   XEvent *input;
   /*
    * returns BAD if the character is a <CONTROL> or a <META> character
    *       this will cause the event to be swollowed
    * returns INVALID if the character is a normal character, this means
    *       that the event it not processed and will be handled by a
    *       routine later
    * otherwise, returns the value of sub-functions, such as QuitProc()
    */
{
   register int i;
   char buf[80], *name=NULL, args[MAXSTRING+1];
   int valid_shortcut=FALSE, have_ch;
   KeySym key_sym=(KeySym)0;
   XKeyEvent *key_ev;

   key_ev = (&(input->xkey));
   have_ch = XLookupString(key_ev, buf, sizeof(buf), &key_sym, &c_stat);
   TranslateKeys(buf, &key_sym);

   *args = '\0';

   if (key_ev->window == drawWindow && CharIsCntrlINS(key_ev, key_sym)) {
      CopyToCutBuffer();
      return BAD;
   } else if (key_ev->window == drawWindow && CharIsShiftINS(key_ev, key_sym)) {
      PasteFromCutBuffer();
      return BAD;
   } else if (key_sym >= '\040' && (key_ev->state & (ControlMask | METAMASK))) {
      valid_shortcut = TRUE;
   } else if (key_sym >= XK_F1 && key_sym <= XK_F12) {
      char code='\0';
      unsigned int state=0;

      valid_shortcut = FetchFuncKeyShortCut((int)(key_sym), &code, &state,
            &name, args, sizeof(args));
      if (valid_shortcut) {
         key_sym = code;
         key_ev->state = state;
      }
   } else if (((key_sym>'\040' && key_sym<='\177') ||
         (key_sym>0xa0 && key_sym<=0xff)) &&
         !(key_ev->state & (ControlMask | METAMASK)) &&
         curChoice != DRAWTEXT && !TidgetHasFocus()) {
      char code='\0';
      unsigned int state=0;

      for (i = 0; i < numExtraWins; i++) {
         if (key_ev->window == extraWinInfo[i].window &&
               extraWinInfo[i].window != None) {
            break;
         }
      }
      if (i == numExtraWins) {
         valid_shortcut = FetchShortCut((int)(key_sym&0xff),
               &code, &state, &name, args, sizeof(args));
         if (valid_shortcut) {
            key_sym = code;
            key_ev->state = state;
         }
      }
   }
   if (valid_shortcut) {
      int rc=0;
      int try_x_lookup_keysym=FALSE;

      Msg("");
#ifndef _NO_XLOOKUPKEYSYM
      try_x_lookup_keysym = TRUE;
#endif /* ~_NO_XLOOKUPKEYSYM */
      rc = DoShortCut(key_ev, name, key_sym, key_ev->state, args);
      if (rc == INVALID && try_x_lookup_keysym) {
         key_sym = XLookupKeysym(key_ev, 0);

         return DoShortCut(key_ev, name, key_sym, key_ev->state, args);
      }
      return rc;
   }
   return INVALID;
}

int ExecuteCmdById(nCmdId, nIndex)
   int nCmdId, nIndex;
{
   char *name=NULL, args[MAXSTRING+1], code='\0';
   unsigned int state;
   KeySym key_sym=(KeySym)0;
   XKeyEvent key_ev;

   *args = '\0';
   if (FetchCmdById(nCmdId, &code, &state, &name, args)) {
      if (*args == '\0') sprintf(args, "%d", nIndex);
      key_sym = code;
      key_ev.state = state;
      Msg("");
      return DoShortCut(&key_ev, name, key_sym, key_ev.state, args);
   } else if (cmdLineTgrm2 && ValidTangram2CmdId(nCmdId)) {
      return DoTangram2Cmd(nCmdId, NULL);
   }
   return INVALID;
}

int CallShortCut(name, argc, argv, code, state)
   char *name, *argv[], *code;
   int argc;
   unsigned int state;
{
   /* do not translate -- program constants */
   if (UtilStrICmp(name, "ZoomInAtCursor") == 0 ||
         UtilStrICmp(name, "CenterAtCursor") == 0) {
      return FALSE;
   }
   DoShortCut(NULL, name, (KeySym)(*code), state, (argc<=1 ? NULL : argv[1]));
   return TRUE;
}

int SomethingDirty()
{
   register struct ObjRec *obj_ptr=topObj;

   for ( ; obj_ptr != NULL; obj_ptr = obj_ptr->next) {
      if (obj_ptr->dirty) {
         return TRUE;
      }
   }
   return FALSE;
}

static char gszEditorCmd[MAXSTRING];

static char gszDefEditorCmd[]="xterm -title '%s' -e vi '%s'";

static
void InitEditor()
{
   static int stInitialized=FALSE;

   if (!stInitialized) {
      int count=0;
      char *psz=NULL;

      strcpy(gszEditorCmd, gszDefEditorCmd);
      if ((psz=XGetDefault(mainDisplay, TOOL_NAME, "Editor")) != NULL) {
         UtilStrCpyN(gszEditorCmd, sizeof(gszEditorCmd), psz);
      }
      UtilTrimBlanks(gszEditorCmd);
      for(psz=strstr(gszEditorCmd,"%s"); psz != NULL; psz=strstr(++psz,"%s")) {
         count++;
      }
      if (count != 2) {
         sprintf(gszMsgBox, TgLoadString(STID_INVALID_XDEF_USE_ALT_STR),
               TOOL_NAME, "Editor", gszEditorCmd, gszDefEditorCmd);
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
         strcpy(gszEditorCmd, gszDefEditorCmd);
      }
      stInitialized = TRUE;
   }
}

typedef struct tagTmpFileRec {
   char tmp_fname[MAXPATHLENGTH];
   struct stat stat_buf;
} TmpFileInfo;

static
int WriteAttrToTmp(attr_ptr, ptfi)
   struct AttrRec *attr_ptr;
   TmpFileInfo *ptfi;
{
   FILE *fp=NULL;

   if (MkTempFile(ptfi->tmp_fname, sizeof(ptfi->tmp_fname), tmpDir,
         TOOL_NAME) == NULL) {
      return FALSE;
   }
   if ((fp=fopen(ptfi->tmp_fname, "w")) == NULL) {
      sprintf(gszMsgBox, TgLoadString(STID_CANNOT_OPEN_FILE_FOR_WRITING),
            ptfi->tmp_fname);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return FALSE;
   }
   writeFileFailed = FALSE;
   if (*attr_ptr->attr_name.s == '\0') {
      DumpMiniLinesInAscii(fp, &attr_ptr->obj->detail.t->minilines, NULL);
   } else {
      int need_to_free_tmp_buf=FALSE;
      MiniLineInfo *pMiniLine=attr_ptr->obj->detail.t->minilines.first;
      char *tmp_buf=NULL, *psz=NULL;

      tmp_buf = ConvertAttrNameFirstMiniLineToString(attr_ptr,
            &need_to_free_tmp_buf);
      psz = strchr(tmp_buf, '=');
      fprintf(fp, "%s\n", &psz[1]);

      for (pMiniLine=pMiniLine->next; pMiniLine != NULL;
            pMiniLine=pMiniLine->next) {
         DumpMiniLineInAscii(fp, pMiniLine, NULL);
         if (fprintf(fp, "\n") == EOF) writeFileFailed = TRUE;
      }
      if (need_to_free_tmp_buf) UtilFree(tmp_buf);
   }
   fclose(fp);

   if (writeFileFailed) {
      FailToWriteFileMessage(ptfi->tmp_fname);
      unlink(ptfi->tmp_fname);
      return FALSE;
   }
   if (tmpFileMode != 0 && chmod(ptfi->tmp_fname, tmpFileMode)) {
      sprintf(gszMsgBox, TgLoadString(STID_CANNOT_CHMOD), ptfi->tmp_fname);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      unlink(ptfi->tmp_fname);
      return FALSE;
   }
   if (stat(ptfi->tmp_fname, &ptfi->stat_buf) != 0) {
      sprintf(gszMsgBox, TgLoadString(STID_STAT_FAIL_EDIT_ATTR_VAL_SAME),
            ptfi->tmp_fname);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      unlink(ptfi->tmp_fname);
      return FALSE;
   }
   return TRUE;
}

static
int WriteNamedAttrsToTmp(obj_ptr, num_restricted, ppsz_restricted, ptfi,
      pn_num_exported)
   struct ObjRec *obj_ptr;
   int num_restricted, *pn_num_exported;
   char **ppsz_restricted;
   TmpFileInfo *ptfi;
{
   int i=0, num_found=0;
   FILE *fp=NULL;

   if (MkTempFile(ptfi->tmp_fname, sizeof(ptfi->tmp_fname), tmpDir,
         TOOL_NAME) == NULL) {
      return FALSE;
   }
   if ((fp=fopen(ptfi->tmp_fname, "w")) == NULL) {
      sprintf(gszMsgBox, TgLoadString(STID_CANNOT_OPEN_FILE_FOR_WRITING),
            ptfi->tmp_fname);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return FALSE;
   }
   sprintf(gszMsgBox, TgLoadCachedString(CSTID_WRITING_ATTR_TO_NAMED_FILE),
         ptfi->tmp_fname);
   Msg(gszMsgBox);
   writeFileFailed = FALSE;

   for (i=0; i < num_restricted; i++) {
      char *attr_name=ppsz_restricted[i];
      struct AttrRec *attr_ptr=FindObjAttrWithName(obj_ptr, attr_name);

      if (attr_ptr == NULL) {
         sprintf(gszMsgBox, TgLoadCachedString(CSTID_CANT_FIND_NAMED_ATTR),
               attr_name);
         Msg(gszMsgBox);
      } else {
         if (*attr_ptr->attr_name.s == '\0') {
            DumpMiniLinesInAscii(fp, &attr_ptr->obj->detail.t->minilines, NULL);
         } else {
            int need_to_free_tmp_buf=FALSE;
            MiniLineInfo *pMiniLine=attr_ptr->obj->detail.t->minilines.first;
            char *tmp_buf=NULL;

            num_found++;
            tmp_buf = ConvertAttrNameFirstMiniLineToString(attr_ptr,
                  &need_to_free_tmp_buf);
            fprintf(fp, "%s\n", tmp_buf);

            for (pMiniLine=pMiniLine->next; pMiniLine != NULL;
                  pMiniLine=pMiniLine->next) {
               DumpMiniLineInAscii(fp, pMiniLine, NULL);
               if (fprintf(fp, "\n") == EOF) writeFileFailed = TRUE;
            }
            if (need_to_free_tmp_buf) UtilFree(tmp_buf);

            if (fprintf(fp, "%s\n", gszAttrSeparator) == EOF) {
               writeFileFailed = TRUE;
            }
         }
      }
   }
   fclose(fp);

   if (writeFileFailed) {
      FailToWriteFileMessage(ptfi->tmp_fname);
      unlink(ptfi->tmp_fname);
      return FALSE;
   }
   if (tmpFileMode != 0 && chmod(ptfi->tmp_fname, tmpFileMode)) {
      sprintf(gszMsgBox, TgLoadString(STID_CANNOT_CHMOD), ptfi->tmp_fname);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      unlink(ptfi->tmp_fname);
      return FALSE;
   }
   if (stat(ptfi->tmp_fname, &ptfi->stat_buf) != 0) {
      sprintf(gszMsgBox, TgLoadString(STID_STAT_FAIL_EDIT_ATTR_VAL_SAME),
            ptfi->tmp_fname);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      unlink(ptfi->tmp_fname);
      return FALSE;
   }
   sprintf(gszMsgBox, TgLoadString(STID_ATTR_EXPORTED_TO_NAMED_FILE),
         ptfi->tmp_fname);
   Msg(gszMsgBox);

   if (pn_num_exported != NULL) *pn_num_exported = num_found;

   return TRUE;
}

static
int TmpFileChanged(ptfi)
   TmpFileInfo *ptfi;
{
   struct stat stat_buf;

   memset(&stat_buf, 0, sizeof(struct stat));
   if (stat(ptfi->tmp_fname, &stat_buf) != 0) {
      sprintf(gszMsgBox, TgLoadString(STID_STAT_FAIL_EDIT_ATTR_VAL_SAME),
            ptfi->tmp_fname);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return FALSE;
   }
   if (stat_buf.st_mtime == ptfi->stat_buf.st_mtime &&
         stat_buf.st_size == ptfi->stat_buf.st_size) {
      return FALSE;
   }
   return TRUE;
}

static
int ReadAttrFromTmp(attr_ptr, ptfi)
   struct AttrRec *attr_ptr;
   TmpFileInfo *ptfi;
{
   struct ObjRec *attr_owner_obj=attr_ptr->owner;
   FILE *fp=NULL;

   if ((fp=fopen(ptfi->tmp_fname, "r")) == NULL) {
      sprintf(gszMsgBox, TgLoadString(STID_CANNOT_OPEN_FILE_FOR_READING),
            ptfi->tmp_fname);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return FALSE;
   }
   PrepareToReplaceAnObj(attr_owner_obj);

   JustReadFileIntoAttr(fp, attr_ptr, attr_owner_obj);

   fclose(fp);

   return TRUE;
}

static
int ReadNamedAttrsFromTmp(obj_ptr, num_restricted, ppsz_restricted, ptfi)
   struct ObjRec *obj_ptr;
   int num_restricted;
   char **ppsz_restricted;
   TmpFileInfo *ptfi;
{
   FILE *fp=NULL;
   int rc=0;

   if ((fp=fopen(ptfi->tmp_fname, "r")) == NULL) {
      sprintf(gszMsgBox, TgLoadString(STID_CANNOT_OPEN_FILE_FOR_READING),
            ptfi->tmp_fname);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return FALSE;
   }
   rc = ImportNamedAttrs(fp, obj_ptr, num_restricted, ppsz_restricted,
         ptfi->tmp_fname);
   fclose(fp);

   return rc;
}

static
int AbortLaunch(pVoid)
   void *pVoid;
{
   if (MsgBox(TgLoadString(STID_CNTRL_C_ABORT_LAUNCH), TOOL_NAME, YNC_MB) ==
         MB_ID_YES) {
      return TRUE;
   }
   return FALSE;
}

void EditIndexedAttrInEditor(index)
   int index;
{
   int i=0, num_attrs=0, total_attrs=0;
   int restricted=FALSE, num_restricted=0, actual_index=0, found=FALSE;
   char **attr_strings=NULL, cmd[MAXSTRING<<1], title[MAXSTRING];
   char **ppsz_restricted=NULL;
   struct AttrRec *attr_ptr=NULL, *restricted_attr=NULL;
   FILE *pfp=NULL;
   TmpFileInfo tfi;

   InitEditor();

   if (gpEditAttrInEditorAttrInfo == NULL || topSel == NULL ||
         topSel != botSel) {
      return;
   }
   for (attr_ptr=topSel->obj->fattr; attr_ptr != NULL;
         attr_ptr=attr_ptr->next, total_attrs++) {
   }
   attr_ptr = topSel->obj->fattr;

   num_attrs = gpEditAttrInEditorAttrInfo->num_attrs;
   attr_strings = gpEditAttrInEditorAttrInfo->attr_strings;

   if (total_attrs <= 0 || num_attrs <= 0 || attr_strings == NULL) return;

   restricted = HasEditAttrsInContextMenu(topSel->obj, &restricted_attr);
   if (restricted) {
      GetRestrictedAttrNames(restricted_attr->obj, &ppsz_restricted,
            &num_restricted);
      if (ppsz_restricted == NULL || num_restricted <= 0) {
         return;
      }
   }
   for (i=0; i < total_attrs; i++, attr_ptr=attr_ptr->next) {
      if (restricted) {
         if (!IsRestrictedAttr(attr_ptr->attr_name.s, ppsz_restricted,
               num_restricted)) {
            continue;
         }
      }
      if (actual_index == index) {
         found = TRUE;
         break;
      }
      actual_index++;
   }
   FreeRestrictedAttrNames(ppsz_restricted, num_restricted);

   if (!found) return;

   memset(&tfi, 0, sizeof(TmpFileInfo));
   if (!WriteAttrToTmp(attr_ptr, &tfi)) return;

   SaveStatusStrings();
   if (*attr_ptr->attr_name.s == '\0') {
      sprintf(title, TgLoadString(STID_EDIT_UNNAME_ATTR_DOTS));
      sprintf(cmd, gszEditorCmd, title, tfi.tmp_fname);
      sprintf(gszMsgBox, TgLoadString(STID_EDIT_UNNAME_ATTR_WITH_CMD), cmd);
   } else {
      sprintf(title, TgLoadString(STID_EDIT_VAL_OF_ATTR_DOTS),
            attr_ptr->attr_name.s);
      sprintf(cmd, gszEditorCmd, title, tfi.tmp_fname);
      sprintf(gszMsgBox, TgLoadString(STID_EDIT_VAL_OF_ATTR_WITH_CMD),
            attr_ptr->attr_name.s, cmd);
   }
   if (!FindProgramInPath(cmd, NULL, FALSE)) {
      RestoreStatusStrings();
      unlink(tfi.tmp_fname);
      return;
   }
   ShowInterrupt(DEF_CHECK_INTERVAL);
   SetStringStatus(gszMsgBox);

   EndMeasureTooltip(FALSE);
   if ((pfp=(FILE*)popen(cmd, "r")) == NULL) {
      sprintf(gszMsgBox, TgLoadString(STID_FAIL_TO_EXECUTE_CMD), cmd);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
   } else {
      int quit=FALSE, got_eof=FALSE, fd=fileno(pfp);

      Msg(TgLoadCachedString(CSTID_CNTRL_C_TO_INTR_AND_ABEND));
      SetSocketBlockingState(&fd, FALSE);

      /*
       * Note: Calling WaitForEvent() with the second argument being
       *       TRUE can lose data in the pipe.  Can do this here
       *       because the data in the pipe is ignored.
       */
      while (WaitForEvent(pfp, TRUE, FALSE, &quit, EXPOSE_AND_ESC_X_EV_ONLY,
            AbortLaunch, NULL)) {
         if (quit) {
            break;
         } else if (PipeReachedEOF(pfp)) {
            got_eof = TRUE;
            break;
         }
      }
      if (quit && !got_eof) {
         sprintf(gszMsgBox,
               TgLoadString(STID_CMD_ABORT_LAUNCH_CLOSE_TOOL), cmd, TOOL_NAME);
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      } else {
         pclose(pfp);
      }
   }
   RestoreStatusStrings();
   HideInterrupt();

   if (TmpFileChanged(&tfi)) {
      HighLightReverse();
      if (ReadAttrFromTmp(attr_ptr, &tfi)) {
         SetFileModified(TRUE);
      }
      HighLightForward();
   }
   unlink(tfi.tmp_fname);
}

void EditIndexedAttrGroupInEditor(index)
   int index;
{
   int num_attrs_in_attr_group=0, total_attrs_in_obj=0;
   int num_restricted=0, num_exported=0, done=FALSE;
   char **attr_name_array=NULL, cmd[MAXSTRING<<1], title[MAXSTRING];
   char **ppsz_restricted=NULL;
   struct AttrRec *attr_ptr=NULL;
   FILE *pfp=NULL;
   TmpFileInfo tfi;

   InitEditor();

   if (gpEditAttrInEditorAttrInfo == NULL || topSel == NULL ||
         topSel != botSel || index >= maxAttrGroups) {
      return;
   }
   for (attr_ptr=topSel->obj->fattr; attr_ptr != NULL;
         attr_ptr=attr_ptr->next, total_attrs_in_obj++) {
   }
   attr_ptr = topSel->obj->fattr;

   num_attrs_in_attr_group = gAttrGroupInfo[index]->num_attrs;
   attr_name_array = gAttrGroupInfo[index]->attr_name;

   if (total_attrs_in_obj <= 0 || num_attrs_in_attr_group <= 0 ||
         attr_name_array == NULL) {
      return;
   }
   /*
    * For this function, we do not need to call HasEditAttrsInContextMenu()
    *     because the "edit_attrs_in_context_menu=" attribute is only for the
    *     "Edit Attribute In Editor" submenu and *not* for the
    *     "Edit Attribute Group In Editor" submenu.
    * We just pretend that the "edit_attrs_in_context_menu=" attribute is
    *     there so we can reuse the code.
    */
   GetAttrGroupAttrNames(topSel->obj, index, &ppsz_restricted, &num_restricted);
   if (ppsz_restricted == NULL || num_restricted <= 0) {
      return;
   }
   memset(&tfi, 0, sizeof(TmpFileInfo));
   if (!WriteNamedAttrsToTmp(topSel->obj, num_restricted, ppsz_restricted,
         &tfi, &num_exported)) {
      FreeRestrictedAttrNames(ppsz_restricted, num_restricted);
      return;
   }
   if (num_restricted != num_exported) {
      sprintf(gszMsgBox, TgLoadString(STID_WARN_CANNOT_FIND_SOME_ATTRS),
               gAttrGroupInfo[index]->displayed_names);
      if (MsgBox(gszMsgBox, TOOL_NAME, YNC_MB) != MB_ID_YES) {
         FreeRestrictedAttrNames(ppsz_restricted, num_restricted);
         unlink(tfi.tmp_fname);
         return;
      }
   }
   SaveStatusStrings();
   if (*attr_ptr->attr_name.s == '\0') {
      sprintf(title, TgLoadString(STID_EDIT_UNNAME_ATTR_DOTS));
      sprintf(cmd, gszEditorCmd, title, tfi.tmp_fname);
      sprintf(gszMsgBox, TgLoadString(STID_EDIT_UNNAME_ATTR_WITH_CMD), cmd);
   } else {
      sprintf(title, TgLoadString(STID_EDIT_VAL_OF_ATTR_DOTS),
            attr_ptr->attr_name.s);
      sprintf(cmd, gszEditorCmd, title, tfi.tmp_fname);
      sprintf(gszMsgBox, TgLoadString(STID_EDIT_VAL_OF_ATTR_WITH_CMD),
            attr_ptr->attr_name.s, cmd);
   }
   if (!FindProgramInPath(cmd, NULL, FALSE)) {
      RestoreStatusStrings();
      unlink(tfi.tmp_fname);
      return;
   }
   while (!done) {
      ShowInterrupt(DEF_CHECK_INTERVAL);
      SetStringStatus(gszMsgBox);

      EndMeasureTooltip(FALSE);
      if ((pfp=(FILE*)popen(cmd, "r")) == NULL) {
         sprintf(gszMsgBox, TgLoadString(STID_FAIL_TO_EXECUTE_CMD), cmd);
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      } else {
         int quit=FALSE, got_eof=FALSE, fd=fileno(pfp);

         Msg(TgLoadCachedString(CSTID_CNTRL_C_TO_INTR_AND_ABEND));
         SetSocketBlockingState(&fd, FALSE);

         /*
          * Note: Calling WaitForEvent() with the second argument being
          *       TRUE can lose data in the pipe.  Can do this here
          *       because the data in the pipe is ignored.
          */
         while (WaitForEvent(pfp, TRUE, FALSE, &quit, EXPOSE_AND_ESC_X_EV_ONLY,
               AbortLaunch, NULL)) {
            if (quit) {
               break;
            } else if (PipeReachedEOF(pfp)) {
               got_eof = TRUE;
               break;
            }
         }
         if (quit && !got_eof) {
            sprintf(gszMsgBox,
                  TgLoadString(STID_CMD_ABORT_LAUNCH_CLOSE_TOOL), cmd,
                  TOOL_NAME);
            MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
         } else {
            pclose(pfp);
         }
      }
      RestoreStatusStrings();
      HideInterrupt();

      if (TmpFileChanged(&tfi)) {
         HighLightReverse();
         if (ReadNamedAttrsFromTmp(topSel->obj, num_restricted, ppsz_restricted,
               &tfi)) {
            done = TRUE;
         } else {
            /* get the file modified */
            if (stat(tfi.tmp_fname, &tfi.stat_buf) != 0) {
               sprintf(gszMsgBox,
                     TgLoadString(STID_STAT_FAIL_EDIT_ATTR_VAL_SAME),
                     tfi.tmp_fname);
               MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
               unlink(tfi.tmp_fname);
               done = TRUE;
            }
         }
         HighLightForward();
      } else {
         done = TRUE;
      }
   }
   unlink(tfi.tmp_fname);
}

TgMenu *CreateEditAttrInEditorMenu(parent_menu, X, Y, menu_info,
      status_str_xlated)
   TgMenu *parent_menu;
   int X, Y;
   TgMenuInfo *menu_info;
   int status_str_xlated; /* ignored, always 0 */
{
   TgMenu *menu=NULL;

   if (topSel == NULL || topSel != botSel) return NULL;

   FreeEditAttrInfo(gpEditAttrInEditorAttrInfo);
   gpEditAttrInEditorAttrInfo = NULL;

   gpEditAttrInEditorAttrInfo = CreateEditAttrInfo(topSel->obj);

   if (gpEditAttrInEditorAttrInfo == NULL) return NULL;

   menu = CreateAttrMenu(parent_menu, X, Y,
         gpEditAttrInEditorAttrInfo->num_attrs,
         gpEditAttrInEditorAttrInfo->attr_strings,
         gpEditAttrInEditorAttrInfo->status_strings,
         gpEditAttrInEditorAttrInfo->fore_colors);
   if (menu != NULL) {
      int i=0, num_items=menu->num_items;
      TgMenuItem *menuitems=menu->menuitems;

      for (i=0; i < num_items; i++) {
         menuitems[i].cmdid = CMDID_EDITATTRINEDITOR;
      }
   }
   return menu;
}

TgMenu *CreateEditAttrGroupInEditorMenu(parent_menu, X, Y, menu_info,
      status_str_xlated)
   TgMenu *parent_menu;
   int X, Y;
   TgMenuInfo *menu_info;
   int status_str_xlated; /* ignored, always 0 */
{
   TgMenu *menu=NULL;

   if (topSel == NULL || topSel != botSel) return NULL;

   FreeEditAttrInfo(gpEditAttrInEditorAttrInfo);
   gpEditAttrInEditorAttrInfo = NULL;

   gpEditAttrInEditorAttrInfo = CreateEditAttrGroupInfo(topSel->obj);

   if (gpEditAttrInEditorAttrInfo == NULL) return NULL;

   menu = CreateAttrMenu(parent_menu, X, Y,
         gpEditAttrInEditorAttrInfo->num_attrs,
         gpEditAttrInEditorAttrInfo->attr_strings,
         gpEditAttrInEditorAttrInfo->status_strings,
         gpEditAttrInEditorAttrInfo->fore_colors);
   if (menu != NULL) {
      int i=0, num_items=menu->num_items;
      TgMenuItem *menuitems=menu->menuitems;

      for (i=0; i < num_items; i++) {
         menuitems[i].cmdid = CMDID_EDITATTRGROUPINEDITOR;
      }
   }
   return menu;
}

void GetProperty(index)
   int index;
{
   DoGetProperty(index);
}

TgMenu *CreateGetPropertyMenu(parent_menu, X, Y, menu_info,
      status_str_xlated)
   TgMenu *parent_menu;
   int X, Y;
   TgMenuInfo *menu_info;
   int status_str_xlated; /* ignored, always 0 */
{
   TgMenu *menu=NULL;

   if (topSel == NULL || topSel != botSel) return NULL;

   FreeEditAttrInfo(gpEditAttrInEditorAttrInfo);
   gpEditAttrInEditorAttrInfo = NULL;

   gpEditAttrInEditorAttrInfo = CreateGetPropertyInfo();

   if (gpEditAttrInEditorAttrInfo == NULL) return NULL;

   menu = CreateAttrMenu(parent_menu, X, Y,
         gpEditAttrInEditorAttrInfo->num_attrs,
         gpEditAttrInEditorAttrInfo->attr_strings,
         gpEditAttrInEditorAttrInfo->status_strings,
         gpEditAttrInEditorAttrInfo->fore_colors);
   if (menu != NULL) {
      int i=0, num_items=menu->num_items;
      TgMenuItem *menuitems=menu->menuitems;

      for (i=0; i < num_items; i++) {
         menuitems[i].cmdid = CMDID_GETPROPERTY;
      }
   }
   return menu;
}

/* --------------------- PeekDimension stuff --------------------- */

static
void SetPeekStrings(attr_strings, status_strings, i, menu_text, status_text)
   char **attr_strings, **status_strings;
   int i;
   char *menu_text, *status_text;
{
   attr_strings[i] = UtilStrDup(menu_text);
   status_strings[i] = UtilStrDup(status_text);
   if (attr_strings[i] == NULL || status_strings[i] == NULL) FailAllocMessage();
}

static
int CanGetArea(obj_ptr)
   struct ObjRec *obj_ptr;
{
   int type=obj_ptr->type;

   if (type == OBJ_GROUP || type == OBJ_SYM || type == OBJ_ICON ||
         type == OBJ_PIN) {
      if (obj_ptr->detail.r->first == obj_ptr->detail.r->last) {
         return CanGetArea(obj_ptr->detail.r->first);
      }
      return FALSE;
   }
   return (type == OBJ_BOX || type == OBJ_POLYGON);
}

static
double GetPolygonArea(polygon_ptr)
   struct PolygonRec *polygon_ptr;
{
   int i=0, n=0, curved=polygon_ptr->curved;
   int num_smooth_points=0, num_hinge_points=0;
   double area=(double)0;
   IntPoint *vs=NULL;
   char *smooth=NULL;

   if (curved == LT_STRUCT_SPLINE) {
      n = polygon_ptr->ssn;
      vs = polygon_ptr->ssvlist;
      smooth = polygon_ptr->ssmooth;
   } else {
      n = polygon_ptr->n;
      vs = polygon_ptr->vlist;
      smooth = polygon_ptr->smooth;
   }
   switch (curved) {
   case LT_STRAIGHT:
   case LT_SPLINE:
   case LT_STRUCT_SPLINE:
      for (i=1; i < n; i++) {
         if (smooth[i]) {
            num_smooth_points++;
         } else {
            num_hinge_points++;
         }
      }
      if (num_smooth_points == 0) {
         for (i=0; i < n-1; i++) {
            area += (double)(((double)vs[i].x) * ((double)vs[i+1].y) -
                  ((double)vs[i+1].x) * ((double)vs[i].y));
         }
         area = area / ((double)2);
      } else {
         /* don't know how to calculate with smooth points */
      }
      break;
   case LT_INTSPLINE:
      /* don't know how to calculate with smooth points */
      break;
   }
   for (i=0; i < n-1; i++) {
      area += (double)(((double)vs[i].x) * ((double)vs[i+1].y) -
            ((double)vs[i+1].x) * ((double)vs[i].y));
   }
   area = area / ((double)2);

   return area;
}

static
double GetArea(obj_ptr)
   struct ObjRec *obj_ptr;
{
   int type=obj_ptr->type, w=0, h=0;
   double area=(double)0, scale_x=(double)0, scale_y=(double)0;

   if (type == OBJ_GROUP || type == OBJ_SYM || type == OBJ_ICON ||
         type == OBJ_PIN) {
      if (obj_ptr->detail.r->first == obj_ptr->detail.r->last) {
         return GetArea(obj_ptr->detail.r->first);
      }
   }
   switch (type) {
   case OBJ_BOX:
      if (obj_ptr->ctm == NULL) {
         w = obj_ptr->obbox.rbx-obj_ptr->obbox.ltx;
         h = obj_ptr->obbox.rby-obj_ptr->obbox.lty;
         area = (double)(((double)w)*((double)h));
      } else {
         w = obj_ptr->orig_obbox.rbx-obj_ptr->orig_obbox.ltx;
         h = obj_ptr->orig_obbox.rby-obj_ptr->orig_obbox.lty;
         area = (double)(((double)w)*((double)h));
         scale_x = (double) (((double)fabs(obj_ptr->ctm->m[CTM_SX])) /
               ((double)1000));
         scale_y = (double) (((double)fabs(obj_ptr->ctm->m[CTM_SY])) /
               ((double)1000));
         area = area * scale_x * scale_y;
      }
      break;
   case OBJ_POLYGON:
      area = GetPolygonArea(obj_ptr->detail.g);
      if (obj_ptr->ctm != NULL) {
         scale_x = (double) (((double)fabs(obj_ptr->ctm->m[CTM_SX])) /
               ((double)1000));
         scale_y = (double) (((double)fabs(obj_ptr->ctm->m[CTM_SY])) /
               ((double)1000));
         area = area * scale_x * scale_y;
      }
      break;
   }
   return area;
}


static
EditAttrInfo *CreatePeekDimensionInfo()
{
   EditAttrInfo *pEditAttrInfo=NULL;
   char **attr_strings=NULL, **status_strings=NULL;
   char menu_text[MAXSTRING], status_text[MAXSTRING];
   char buf[MAXSTRING], buf1[MAXSTRING], *psz=NULL, *psz1=NULL;
   int num_attrs=10, can_get_area=FALSE;
   struct ObjRec *obj_ptr=NULL;

   if (topSel == NULL) return NULL;

   obj_ptr = topSel->obj;

   can_get_area = CanGetArea(obj_ptr);
   if (can_get_area) {
      num_attrs += 2;
   }
   pEditAttrInfo = (EditAttrInfo*)malloc(sizeof(EditAttrInfo));
   if (pEditAttrInfo == NULL) FailAllocMessage();
   memset(pEditAttrInfo, 0, sizeof(EditAttrInfo));

   attr_strings = (char**)malloc(num_attrs*sizeof(char*));
   status_strings = (char**)malloc(num_attrs*sizeof(char*));
   if (attr_strings == NULL || status_strings == NULL) FailAllocMessage();
   memset(attr_strings, 0, num_attrs*sizeof(char*));
   memset(status_strings, 0, num_attrs*sizeof(char*));

   PixelToMeasurementUnit(buf, obj_ptr->obbox.rbx-obj_ptr->obbox.ltx);
   PixelToMeasurementUnit(buf1, obj_ptr->bbox.rbx-obj_ptr->bbox.ltx);
   psz = ((*buf) == '+' ? &buf[1] : buf);
   psz1 = ((*buf1) == '+' ? &buf1[1] : buf1);
   /* do not translate -- program constants */
   sprintf(menu_text, "width = %s (outer width = %s)", psz, psz1);
   strcpy(status_text, TgLoadString(STID_PEEK_DIM_WIDTH));
   SetPeekStrings(attr_strings, status_strings, 0, menu_text, status_text);

   PixelToMeasurementUnit(buf, obj_ptr->obbox.rby-obj_ptr->obbox.lty);
   PixelToMeasurementUnit(buf1, obj_ptr->bbox.rby-obj_ptr->bbox.lty);
   psz = ((*buf) == '+' ? &buf[1] : buf);
   psz1 = ((*buf1) == '+' ? &buf1[1] : buf1);
   /* do not translate -- program constants */
   sprintf(menu_text, "height = %s (outer height = %s)", psz, psz1);
   strcpy(status_text, TgLoadString(STID_PEEK_DIM_HEIGHT));
   SetPeekStrings(attr_strings, status_strings, 1, menu_text, status_text);

   attr_strings[2] = TGMUITEM_SEPARATOR;

   PixelToMeasurementUnit(buf, obj_ptr->obbox.ltx);
   PixelToMeasurementUnit(buf1, obj_ptr->bbox.ltx);
   psz = ((*buf) == '+' ? &buf[1] : buf);
   psz1 = ((*buf1) == '+' ? &buf1[1] : buf1);
   /* do not translate -- program constants */
   sprintf(menu_text, "left = %s (outer left = %s)", psz, psz1);
   strcpy(status_text, TgLoadString(STID_PEEK_DIM_LEFT));
   SetPeekStrings(attr_strings, status_strings, 3, menu_text, status_text);

   PixelToMeasurementUnit(buf, obj_ptr->obbox.lty);
   PixelToMeasurementUnit(buf1, obj_ptr->bbox.lty);
   psz = ((*buf) == '+' ? &buf[1] : buf);
   psz1 = ((*buf1) == '+' ? &buf1[1] : buf1);
   /* do not translate -- program constants */
   sprintf(menu_text, "top = %s (outer top = %s)", psz, psz1);
   strcpy(status_text, TgLoadString(STID_PEEK_DIM_TOP));
   SetPeekStrings(attr_strings, status_strings, 4, menu_text, status_text);

   PixelToMeasurementUnit(buf, obj_ptr->obbox.rbx);
   PixelToMeasurementUnit(buf1, obj_ptr->bbox.rbx);
   psz = ((*buf) == '+' ? &buf[1] : buf);
   psz1 = ((*buf1) == '+' ? &buf1[1] : buf1);
   /* do not translate -- program constants */
   sprintf(menu_text, "right = %s (outer right = %s)", psz, psz1);
   strcpy(status_text, TgLoadString(STID_PEEK_DIM_RIGHT));
   SetPeekStrings(attr_strings, status_strings, 5, menu_text, status_text);

   PixelToMeasurementUnit(buf, obj_ptr->obbox.rby);
   PixelToMeasurementUnit(buf1, obj_ptr->bbox.rby);
   psz = ((*buf) == '+' ? &buf[1] : buf);
   psz1 = ((*buf1) == '+' ? &buf1[1] : buf1);
   /* do not translate -- program constants */
   sprintf(menu_text, "bottom = %s (outer bottom = %s)", psz, psz1);
   strcpy(status_text, TgLoadString(STID_PEEK_DIM_BOTTOM));
   SetPeekStrings(attr_strings, status_strings, 6, menu_text, status_text);

   attr_strings[7] = TGMUITEM_SEPARATOR;

   PixelToMeasurementUnit(buf, ((obj_ptr->obbox.rbx+obj_ptr->obbox.ltx)>>1));
   psz = ((*buf) == '+' ? &buf[1] : buf);
   /* do not translate -- program constants */
   sprintf(menu_text, "cx = %s", psz);
   strcpy(status_text, TgLoadString(STID_PEEK_DIM_CX));
   SetPeekStrings(attr_strings, status_strings, 8, menu_text, status_text);

   PixelToMeasurementUnit(buf, ((obj_ptr->obbox.rby+obj_ptr->obbox.lty)>>1));
   psz = ((*buf) == '+' ? &buf[1] : buf);
   /* do not translate -- program constants */
   sprintf(menu_text, "cy = %s", psz);
   strcpy(status_text, TgLoadString(STID_PEEK_DIM_CY));
   SetPeekStrings(attr_strings, status_strings, 9, menu_text, status_text);

   if (can_get_area) {
      double area=GetArea(obj_ptr);

      attr_strings[10] = TGMUITEM_SEPARATOR;

      SquarePixelToMeasurementUnit(buf, round(area));
      psz = ((*buf) == '+' ? &buf[1] : buf);
      /* do not translate -- program constants */
      sprintf(menu_text, "area = %s", psz);
      strcpy(status_text, TgLoadString(STID_PEEK_AREA));
      SetPeekStrings(attr_strings, status_strings, 11, menu_text, status_text);
   }
   pEditAttrInfo->num_attrs = num_attrs;
   pEditAttrInfo->fore_colors = NULL;
   pEditAttrInfo->attr_indices = NULL;
   pEditAttrInfo->attr_names = NULL;
   pEditAttrInfo->attr_values = NULL;
   pEditAttrInfo->attr_strings = attr_strings;
   pEditAttrInfo->status_strings = status_strings;

   return pEditAttrInfo;
}

void PeekDimension(index)
   int index;
{
   /* There's really nothing to do! */
}

TgMenu *CreatePeekDimensionMenu(parent_menu, X, Y, menu_info,
      status_str_xlated)
   TgMenu *parent_menu;
   int X, Y;
   TgMenuInfo *menu_info;
   int status_str_xlated; /* ignored, always 0 */
{
   TgMenu *menu=NULL;

   if (topSel == NULL || topSel != botSel) return NULL;

   FreeEditAttrInfo(gpEditAttrInEditorAttrInfo);
   gpEditAttrInEditorAttrInfo = NULL;

   gpEditAttrInEditorAttrInfo = CreatePeekDimensionInfo();

   if (gpEditAttrInEditorAttrInfo == NULL) return NULL;

   menu = CreateAttrMenu(parent_menu, X, Y,
         gpEditAttrInEditorAttrInfo->num_attrs,
         gpEditAttrInEditorAttrInfo->attr_strings,
         gpEditAttrInEditorAttrInfo->status_strings,
         gpEditAttrInEditorAttrInfo->fore_colors);
   if (menu != NULL) {
      int i=0, num_items=menu->num_items;
      TgMenuItem *menuitems=menu->menuitems;

      for (i=0; i < num_items; i++) {
         menuitems[i].cmdid = CMDID_GETPROPERTY;
      }
   }
   return menu;
}

/* --------------------- RefreshContextMenu() --------------------- */

int RefreshContextMenu(menu)
   TgMenu *menu;
{
   int ok=TRUE;

   if (topSel == NULL || topSel != botSel) return FALSE;

   /* ImageProc submenu */
   ok &= TgEnableMenuItemById(menu, MENU_IMAGEPROC, CanPerformImageProc());

   /* Edit Attribute In Editor */
   ok &= TgEnableMenuItemBySubMenuInfoPtr(menu, &editAttrInEditorMenuInfo,
         topSel->obj->fattr != NULL);

   /* Edit Attribute Group In Editor */
   ok &= TgEnableMenuItemBySubMenuInfoPtr(menu, &editAttrGroupInEditorMenuInfo,
         topSel->obj->fattr != NULL && maxAttrGroups > 0);

   /* Get Property */
   ok &= TgEnableMenuItemBySubMenuInfoPtr(menu, &getPropertyMenuInfo,
         !(topSel->obj->type == OBJ_XPM || (topSel->obj->type == OBJ_XBM &&
         topSel->obj->detail.xbm->real_type==XBM_EPS)));

   /* Get Dimension */
   ok &= TgEnableMenuItemBySubMenuInfoPtr(menu, &peekDimensionMenuInfo,
         topSel != NULL);

   return ok;
}

TgMenu *CreateContextMenu(parent_menu, X, Y, menu_info, status_str_xlated)
   TgMenu *parent_menu;
   int X, Y;
   TgMenuInfo *menu_info;
   int status_str_xlated; /* ignored, always 0 */
{
   TgMenu *menu=TgCreateMenuFromMenuInfo(parent_menu, X, Y, menu_info, FALSE);

   if (menu != NULL) {
      if (!RefreshContextMenu(menu)) {
         return TgDestroyMenu(menu, TRUE);
      }
      menu->refresh_proc = ((RefreshMenuFunc*)RefreshContextMenu);
   }
   return menu;
}

int RefreshEditTextContextMenu(menu)
   TgMenu *menu;
{
   int ok=TRUE;

   /* Copy */
   ok &= TgEnableMenuItemById(menu, CMDID_COPY, textHighlight);
   /* CopyPlainTextAsObject */
   ok &= TgEnableMenuItemById(menu, CMDID_COPYPLAINTEXTASOBJECT,
         (curChoice == DRAWTEXT && textHighlight));
   /* Cut */
   ok &= TgEnableMenuItemById(menu, CMDID_CUT, textHighlight);
   /* Duplicate */
   ok &= TgEnableMenuItemById(menu, CMDID_DUPLICATE, textHighlight);
   /* Delete */
   ok &= TgEnableMenuItemById(menu, CMDID_DELETE, textHighlight);

   return ok;
}

TgMenu *CreateEditTextContextMenu(parent_menu, X, Y, menu_info,
      status_str_xlated)
   TgMenu *parent_menu;
   int X, Y;
   TgMenuInfo *menu_info;
   int status_str_xlated; /* ignored, always 0 */
{
   TgMenu *menu=TgCreateMenuFromMenuInfo(parent_menu, X, Y, menu_info, FALSE);

   if (menu != NULL) {
      if (!RefreshEditTextContextMenu(menu)) {
         return TgDestroyMenu(menu, TRUE);
      }
      menu->refresh_proc = ((RefreshMenuFunc*)RefreshContextMenu);
   }
   return menu;
}

int ContextMenu(X, Y, TrackMenubar)
   int X, Y, TrackMenubar;
{
   int rc=INVALID;
   TgMenu *menu=NULL;

   if (curChoice == DRAWTEXT && textCursorShown) {
      menu = (editTextContextMenuInfo.create_proc)(NULL, X, Y,
            &editTextContextMenuInfo, INVALID);
   } else {
      menu = (baseContextMenuInfo.create_proc)(NULL, X, Y, &baseContextMenuInfo,
            INVALID);
   }
   activeMenu = INVALID;
   if (menu != NULL) {
      menu->track_menubar = TrackMenubar;

      rc = TgMenuLoop(menu);
      TgDestroyMenu(menu, TRUE);
   }
   return rc;
}

void HandleMotionForPortInDrawWindow(mouse_x, mouse_y)
   int mouse_x, mouse_y;
{
   int need_to_highlight=FALSE, something_changed=FALSE;
   struct ObjRec *owner_obj=NULL, *obj_ptr, *obj_under_cursor=NULL;
   char port_name[MAXSTRING];

   obj_ptr = FindAnObj(mouse_x, mouse_y, &owner_obj, &obj_under_cursor,
         port_name);
   if (drawPolyHighlightedNode != NULL) {
      if (obj_under_cursor != drawPolyHighlightedNode) {
         /* un-highlight */
         SelBox(drawWindow, revGrayGC,
               OFFSET_X(drawPolyHighlightedNode->bbox.ltx)-2,
               OFFSET_Y(drawPolyHighlightedNode->bbox.lty)-2,
               OFFSET_X(drawPolyHighlightedNode->bbox.rbx)+2,
               OFFSET_Y(drawPolyHighlightedNode->bbox.rby)+2);
         /* do not translate -- program constants */
         if (obj_under_cursor != NULL && ObjIsAPort(obj_under_cursor)) {
            drawPolyHighlightedNode = obj_under_cursor;
            SetWiringNodeInfo(obj_under_cursor, owner_obj, port_name, TRUE);
         } else {
            drawPolyHighlightedNode = NULL;
            SetWiringNodeInfo(NULL, NULL, NULL, TRUE);
         }
         if (drawPolyHighlightedNode != NULL) {
            need_to_highlight = TRUE;
         }
         something_changed = TRUE;
      }
   } else if (obj_under_cursor != NULL) {
      if (ObjIsAPort(obj_under_cursor)) {
         drawPolyHighlightedNode = obj_under_cursor;
         SetWiringNodeInfo(obj_under_cursor, owner_obj, port_name, TRUE);
      } else {
         drawPolyHighlightedNode = NULL;
         SetWiringNodeInfo(NULL, NULL, NULL, TRUE);
      }
      if (drawPolyHighlightedNode != NULL) {
         need_to_highlight = TRUE;
         something_changed = TRUE;
      }
   }
   if (need_to_highlight) {
      SelBox(drawWindow, revGrayGC,
            OFFSET_X(drawPolyHighlightedNode->bbox.ltx)-2,
            OFFSET_Y(drawPolyHighlightedNode->bbox.lty)-2,
            OFFSET_X(drawPolyHighlightedNode->bbox.rbx)+2,
            OFFSET_Y(drawPolyHighlightedNode->bbox.rby)+2);
   }
   if (something_changed) {
      if (*gstWiringInfo.first_port_name != '\0') {
         char signal_name[MAXSTRING];
         struct AttrRec *first_attr_ptr=FindAttrWithName(
               gstWiringInfo.first_port_obj, "signal_name=", NULL);

         *signal_name = '\0';
         if (first_attr_ptr != NULL) {
            UtilStrCpyN(signal_name, sizeof(signal_name),
                  first_attr_ptr->attr_value.s);
         }
         if (gstWiringInfo.num_ports_to_connect == 99) {
            /* rename signal_name */
            SetHyperSpaceCursor(drawWindow);
            sprintf(gszMsgBox,
                  TgLoadCachedString(CSTID_SET_SIGNAME_FOR_NAMED_PORT),
                  signal_name, gstWiringInfo.first_port_name);
         } else if (gstWiringInfo.num_ports_to_connect == 999) {
            /* clear signal_name */
            SetHyperSpaceCursor(drawWindow);
            sprintf(gszMsgBox,
                  TgLoadCachedString(CSTID_CLEAR_SIGNAME_FOR_NAMED_PORT),
                  signal_name, gstWiringInfo.first_port_name);
         } else {
            sprintf(gszMsgBox,
                  TgLoadCachedString(CSTID_START_A_WIRE_FROM_NAMED_PORT),
                  gstWiringInfo.first_port_name);
         }
         SetStringStatus(gszMsgBox);
      } else {
         if (gstWiringInfo.num_ports_to_connect == 99 ||
               gstWiringInfo.num_ports_to_connect == 999) {
            SetHandCursor(drawWindow);
         }
         ShowCurChoiceMouseStatus(DRAWPOLY, 0, FALSE);
      }
   }
}

static int motionCursorIsMoveCursor=FALSE;

static
void HandleMotionInDrawWindow(input)
   XEvent *input;
{
   int mouse_x=0, mouse_y=0, grid_x=0, grid_y=0, cursor_is_move_cursor=FALSE;
   int saved_motion_cursor_is_move_cursor=motionCursorIsMoveCursor;
   unsigned int state=0;
   XEvent ev;

   while (XCheckWindowEvent(mainDisplay,drawWindow,PointerMotionMask,&ev)) ;

   state = (input->xmotion).state;
   mouse_x = (input->xmotion).x;
   mouse_y = (input->xmotion).y;
   GridXY(mouse_x, mouse_y, &grid_x, &grid_y);
   simpleMotionInDrawWin = TRUE;
   MarkRulers(grid_x, grid_y);
   simpleMotionInDrawWin = FALSE;
   if (curChoice == DRAWPOLY && gstWiringInfo.num_ports_to_connect > 0) {
      HandleMotionForPortInDrawWindow(mouse_x, mouse_y);
#ifdef _NOT_DEFINED
   /*
    * drawPolyToConnectPins is only set to > 0 in "pin.c"
    * what's in "pin.c" is not used at this time
    */
   } else if (curChoice == DRAWPOLY && drawPolyToConnectPins > 0) {
      HandlePinHighlights(mouse_x, mouse_y);
      if (drawPolyHighlightedNode != NULL) {
         gpStartPin = drawPolyHighlightedNode;
      }
#endif /* _NOT_DEFINED */
   } else if (!inHyperSpace && !btn1Warp &&
         !(inSlideShow && !goHyperSpaceInSlideShow)) {
      if (curChoice == DRAWTEXT && textCursorShown && MouseInCurText(input)) {
         if (MouseOnCurTextBoundary(input)) {
            SetCurChoiceMouseStatusStrings(curChoice, FALSE, NULL, TRUE, state);
            cursor_is_move_cursor = TRUE;
         } else {
            SetCurChoiceMouseStatusStrings(curChoice, FALSE, curTextObj, TRUE,
                  state);
         }
      } else {
         struct ObjRec *obj_ptr=NULL, *owner_obj=NULL;

         if ((obj_ptr=FindAnObj(mouse_x, mouse_y, &owner_obj, NULL, NULL)) !=
               NULL) {
            if (owner_obj != NULL) obj_ptr = owner_obj;
         }
         ShowCursor();
         SetCurChoiceMouseStatusStrings(curChoice, FALSE, obj_ptr, FALSE,
               state);
      }
   } else if (inHyperSpace || btn1Warp) {
      struct ObjRec *obj_ptr=NULL, *owner_obj=NULL;
      struct AttrRec *attr_ptr=NULL;

      if ((obj_ptr=FindAnObj(mouse_x, mouse_y, &owner_obj, NULL, NULL)) !=
            NULL) {
         if (owner_obj != NULL) obj_ptr = owner_obj;
         /* do not translate -- program constants */
         if ((attr_ptr=FindAttrWithName(obj_ptr, TELEPORT_ATTR, NULL)) !=
               NULL || (((attr_ptr=FindAttrWithName(obj_ptr, "href=",
               NULL)) != NULL) && *attr_ptr->attr_value.s != '\0')) {
            char fname[MAXPATHLENGTH+1];

            SetHyperSpaceCursor(drawWindow);
            if (FormNewFileName(curDir, attr_ptr->attr_value.s,
                  (strcmp(attr_ptr->attr_name.s,TELEPORT_ATTR)==0 ?
                  OBJ_FILE_EXT : NULL), fname, NULL)) {
               SetStringStatus(fname);
            }
         } else if ((allowLaunchInHyperSpace &&
               (attr_ptr=FindAttrWithName(obj_ptr, LAUNCH_ATTR, NULL)) !=
               NULL) || (attr_ptr=FindAttrWithName(obj_ptr, EXEC_ATTR,
               NULL)) != NULL) {
            SetHyperSpaceCursor(drawWindow);
            sprintf(gszMsgBox, "%s%s", attr_ptr->attr_name.s,
                  (*attr_ptr->attr_value.s=='\0' ? "..." :
                  attr_ptr->attr_value.s));
            SetStringStatus(gszMsgBox);
         } else {
            ShowCursor();
            ShowCurChoiceMouseStatus(INVALID, 0, FALSE);
         }
      } else {
         ShowCursor();
         ShowCurChoiceMouseStatus(INVALID, 0, FALSE);
      }
   }
   if (cursor_is_move_cursor != saved_motion_cursor_is_move_cursor) {
      if (cursor_is_move_cursor) {
         XDefineCursor(mainDisplay, drawWindow, moveCursor);
      } else {
         ShowCursor();
      }
      motionCursorIsMoveCursor = cursor_is_move_cursor;
   }
}

int HandlePressForPortInDrawWindow(cancel)
   int cancel;
{
   if (drawPolyHighlightedNode != NULL) {
      /* un-highlight */
      SelBox(drawWindow, revGrayGC,
            OFFSET_X(drawPolyHighlightedNode->bbox.ltx)-2,
            OFFSET_Y(drawPolyHighlightedNode->bbox.lty)-2,
            OFFSET_X(drawPolyHighlightedNode->bbox.rbx)+2,
            OFFSET_Y(drawPolyHighlightedNode->bbox.rby)+2);
      if (cancel) {
         drawPolyHighlightedNode = NULL;
         SetWiringNodeInfo(NULL, NULL, NULL, TRUE);
      }
   }
   if (cancel) {
      if (connectingPortsByWire) {
         MakeQuiescent();
         Msg(TgLoadString(STID_CONNECT_PORTS_CANCEL_BY_USER));
      }
      return FALSE;
   }
   return TRUE;
}

static
int HandlePressInDrawWindow(input, pn_status)
   XEvent *input;
   int *pn_status;
{
   XButtonEvent *button_ev;

   button_ev = &(input->xbutton);
   if (enableMouseWheel &&
         (button_ev->button == Button4 || button_ev->button == Button5)) {
      if ((button_ev->state & METAMASK) == METAMASK) {
         if (button_ev->button == Button4) {
            ScrollLeft(button_ev);
         } else if (button_ev->button == Button5) {
            ScrollRight(button_ev);
         }
      } else {
         if (button_ev->button == Button4) {
            ScrollUp(button_ev);
         } else if (button_ev->button == Button5) {
            ScrollDown(button_ev);
         }
      }
      *pn_status = INVALID;
      return TRUE;
   }
   if ((button_ev->state & ShiftMask) && (button_ev->state & ControlMask)) {
      int abs_x=ABS_X(button_ev->x);
      int abs_y=ABS_Y(button_ev->y);

      if (button_ev->button == Button1) {
         ZoomInAtCursor(abs_x, abs_y);
      } else if (button_ev->button == Button2) {
         CenterAtCursor(abs_x, abs_y);
      } else if (button_ev->button == Button3) {
         ZoomOut();
      }
      *pn_status = INVALID;
      return TRUE;
   } else if (button_ev->button == Button3 &&
         (button_ev->state & ShiftMask)) {
      SetCurChoice(NOTHING);
      *pn_status = INVALID;
      return TRUE;
   } else if (button_ev->button == Button2 && curChoice == DRAWTEXT &&
         textCursorShown) {
      if (((button_ev->state & ControlMask) == ControlMask &&
            MouseInCurText(input)) || MouseOnCurTextBoundary(input)) {
         MoveEditText(input);
         *pn_status = INVALID;
      } else if (btn2PopupMainMenu) {
         *pn_status = MainMenu();
      } else {
         *pn_status = INVALID;
      }
      return TRUE;
   } else if ((button_ev->button == Button2 && curChoice == NOTHING &&
         (button_ev->state & ShiftMask)) ||
         (inHyperSpace && button_ev->button == Button1)) {
      Teleport(button_ev);
      *pn_status = INVALID;
      return TRUE;
   } else if (button_ev->button == Button1 && curChoice == NOTHING &&
         ((button_ev->state & (ShiftMask | ControlMask)) == 0) &&
         btn1Warp) {
      Teleport(button_ev);
      *pn_status = INVALID;
      return TRUE;
   } else if (button_ev->button == Button1 && !inHyperSpace && !btn1Warp &&
         !(inSlideShow && !goHyperSpaceInSlideShow) && curChoice == DRAWTEXT &&
         textCursorShown && MouseOnCurTextBoundary(input)) {
      MoveEditText(input);
      *pn_status = INVALID;
      return TRUE;
   } else if (button_ev->button == Button2) {
      if (curChoice == DRAWPOLY && gstWiringInfo.num_ports_to_connect > 0) {
         return HandlePressForPortInDrawWindow(TRUE);
      } else if (btn2PopupMainMenu) {
         *pn_status = MainMenu();
      } else {
         *pn_status = INVALID;
      }
      return TRUE;
   } else if (button_ev->button == Button3) {
      /* context-sensitive menu? */
      if (curChoice == DRAWPOLY && gstWiringInfo.num_ports_to_connect > 0) {
         return HandlePressForPortInDrawWindow(TRUE);
      }
      if (btn3PopupModeMenu) {
         ModeMenu(button_ev->x_root, button_ev->y_root, FALSE);
      } else if (!inHyperSpace &&
            !(inSlideShow && !goHyperSpaceInSlideShow)) {
         if (curChoice == DRAWTEXT && textCursorShown &&
               MouseInCurText(input)) {
            ContextMenu(button_ev->x_root, button_ev->y_root, FALSE);
         } else {
            struct ObjRec *obj_ptr=NULL, *owner_obj=NULL;

            if ((obj_ptr=FindAnObj(button_ev->x, button_ev->y, &owner_obj,
                  NULL, NULL)) != NULL) {
               if (owner_obj != NULL) obj_ptr = owner_obj;
            }
            if (obj_ptr == NULL) {
               ModeMenu(button_ev->x_root, button_ev->y_root, FALSE);
            } else {
               int obj_may_not_exist=FALSE;

               if (curChoice == DRAWTEXT && textCursorShown &&
                     obj_ptr == curTextObj) {
                  obj_may_not_exist = TRUE;
               }
               if (!(topSel != NULL && topSel == botSel &&
                     topSel->obj == obj_ptr)) {
                  TieLooseEnds();
                  if (obj_may_not_exist && textDrawn) {
                     obj_may_not_exist = FALSE;
                  }
                  SetCurChoice(NOTHING);
                  if (topSel != NULL) {
                     HighLightReverse();
                     RemoveAllSel();
                  }
                  if (obj_may_not_exist) {
                     UpdSelBBox();
                  } else {
                     AddNewSelObj(obj_ptr);
                     UpdSelBBox();
                     justDupped = FALSE;
                     HighLightForward();
                  }
               }
               ContextMenu(button_ev->x_root, button_ev->y_root, FALSE);
            }
         }
      } else if (inSlideShow) {
         SlideShowModeMenu(button_ev->x_root, button_ev->y_root, FALSE);
      }
      *pn_status = INVALID;
      return TRUE;
   }
   return FALSE;
}

int DrawingEventHandler(input)
   XEvent *input;
{
   XEvent ev;

   if (input->type == Expose) {
      XSync(mainDisplay, False);
      while (XCheckWindowEvent(mainDisplay, drawWindow, ExposureMask, &ev)) ;

      if (topSel != NULL || curChoice == VERTEXMODE || SomethingDirty()) {
         ClearAndRedrawDrawWindow();
      } else {
         RedrawDrawWindow(botObj);
         ResetDirtyBBoxInfo();
         RedrawCurText();
      }
      return INVALID;
   } else if (input->type == ClientMessage) {
      if (curChoice == DRAWTEXT && canvasFontDoubleByte &&
            textCursorShown && tgIMExpectClientMessage(mainDisplay,
            drawWindow)) {
         if (tgIMHandleClientMessage(mainDisplay,
               drawWindow, (XClientMessageEvent*)input, NULL, NULL)) {
         }
      }
      return INVALID;
   } else if (input->type == EnterNotify) {
      if (input->xcrossing.mode == NotifyNormal) {
         RestoreDrawWinDrawTextInfo(FALSE);
         if (curChoice == DRAWTEXT && textCursorShown) {
            tgIMFocusIn(mainDisplay, drawWindow);
         }
      }
      ShowCurChoiceMouseStatus(curChoice, 0, FALSE);
      return INVALID;
   } else if (input->type == LeaveNotify) {
      EndMeasureTooltip(FALSE);

      if (input->xcrossing.mode == NotifyNormal) {
         SaveDrawWinDrawTextInfo(FALSE);
         if (curChoice == DRAWTEXT && textCursorShown) {
            tgIMFocusOut(mainDisplay, drawWindow);
         }
      }
      return INVALID;
   } else if (input->type == MotionNotify) {
      HandleMotionInDrawWindow(input);
      return INVALID;
   }

   if (input->type == ButtonPress) {
      int rc=INVALID;

      EndMeasureTooltip(FALSE);
      if (HandlePressInDrawWindow(input, &rc)) {
         return rc;
      }
      Msg("");
   }
   if (input->type == KeyPress && inSlideShow && !goHyperSpaceInSlideShow) {
      XKeyEvent *key_ev=(&(input->xkey));
      KeySym key_sym=(KeySym)0;
      char buf[80];
      int has_ch=XLookupString(key_ev, buf, sizeof(buf), &key_sym, &c_stat);

      TranslateKeys(buf, &key_sym);
      if (CharIsESC(key_ev, buf, key_sym, &has_ch)) {
         LeaveSlideShow();
         return INVALID;
      } else if (CharIsCRorLF(key_ev, buf, key_sym, &has_ch)) {
         /*
          * For now, this only goes to the next page.  In the future,
          *         this would single step.
          */
         NextSlide();
         return INVALID;
      } else if (key_sym == XK_Left || key_sym == XK_KP_Left ||
            key_sym == XK_Up || key_sym == XK_KP_Up ||
            key_sym == XK_Right || key_sym == XK_KP_Right ||
            key_sym == XK_Down || key_sym == XK_KP_Down) {
         switch (key_sym) {
         case XK_Left: PrevSlide(); break;
         case XK_KP_Left: PrevSlide(); break;
         case XK_Up: PrevSlide(); break;
         case XK_KP_Up: PrevSlide(); break;
         case XK_Right: NextSlide(); break;
         case XK_KP_Right: NextSlide(); break;
         case XK_Down: NextSlide(); break;
         case XK_KP_Down: NextSlide(); break;
         }
         return INVALID;
      } else if (key_sym == XK_Page_Up || key_sym == XK_KP_Page_Up ||
            key_sym == XK_Page_Down || key_sym == XK_KP_Page_Down) {
         switch (key_sym) {
         case XK_Page_Up: PrevSlide(); break;
         case XK_KP_Page_Up: PrevSlide(); break;
         case XK_Page_Down: NextSlide(); break;
         case XK_KP_Page_Down: NextSlide(); break;
         }
         return INVALID;
      }
   }
   switch(curChoice) {
   case NOTHING: Select(input); break;
   case DRAWTEXT: DrawText(input); break;
   case DRAWBOX: DrawBox(input); break;
   case DRAWCORNEROVAL: DrawOval(input); break;
   case DRAWCENTEROVAL: DrawOval(input); break;
   case DRAWEDGECIRCLE: DrawOval(input); break;
   case DRAWPOLY: DrawPoly(input); break;
   case DRAWPOLYGON: DrawPolygon(input); break;
   case DRAWARC: DrawArc(input); break;
   case DRAWEDGEARC: DrawArc(input); break;
   case DRAWRCBOX: DrawRCBox(input); break;
   case FREEHAND: DrawPoly(input); break;
   case VERTEXMODE: Select(input); break;
   case ROTATEMODE: Select(input); break;
   }
   return INVALID;
}

