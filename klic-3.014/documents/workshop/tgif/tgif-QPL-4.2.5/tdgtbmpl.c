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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/tdgtbmpl.c,v 1.7 2011/05/16 16:21:59 william Exp $
 */

#define _INCLUDE_FROM_TDGTBMPL_C_

#include "tgifdefs.h"
#include "patchlvl.h"

#include "auxtext.e"
#include "choose.e"
#include "color.e"
#include "cursor.e"
#include "dialog.e"
#include "font.e"
#include "mainloop.e"
#include "menu.e"
#include "msg.e"
#include "names.e"
#include "pattern.e"
#include "raster.e"
#include "rect.e"
#include "scroll.e"
#include "setup.e"
#include "strtbl.e"
#include "tdgtbmpl.e"
#include "text.e"
#include "tidget.e"
#include "util.e"

/*
 * A BmpList control looks like the following:
 *
 *         +----------------------------------------------+
 *         | +-------------------------------------+----+ |
 *         | |                                     |    | |
 *         | |               dsp_win               |scr | |
 *         | |                                     |_win| |
 *         | |                                     |    | |
 *         | +-------------------------------------+----+ |
 *         +----------------------------------------------+
 *
 * The outside is windowPadding+[vh]_pad.
 * The inside windows have no decorations.  The left one is the
 *         dsp_win and the right one is scr_win.
 * The width of scr_win is 18 in 3D mode.  This includes a 2-pixel
 *         decoration all around.
 * The width of dsp_win is gap+(bmp_w+gap)*num_cols and the height of dsp_win
 *         is gap+(bmp_h+gap)*num_visible_lines.
 */

static void RedrawTdgtBmpList ARGS_DECL((TidgetInfo *pti));
static int TdgtBmpListEventHandler ARGS_DECL((TidgetInfo *pti, XEvent *input,
		TidgetInfo *handling_pti));
static int IsTdgtBmpListEvent ARGS_DECL((TidgetInfo *pti, XEvent *input,
		TidgetInfo **ppti_handler_tidget_return));
static void DestroyTdgtBmpList ARGS_DECL((TidgetInfo *pti));
static void MapTdgtBmpList ARGS_DECL((TidgetInfo *pti));
static void TdgtBmpListMoveResize ARGS_DECL((TidgetInfo *pti, int x, int y,
		int w, int h));
static int TdgtBmpListSendCmd ARGS_DECL((TidgetInfo *pti, int cmd_type,
		int cmd_arg, void *pv_cmd_userdata));

/* --------------------- Utility Functions --------------------- */

static
void FreeBmpListItemInfo(pblii)
   BmpListItemInfo *pblii;
{
   if (pblii->pf_free_callback != NULL) {
      /* the callback function must also free pblii */
      (pblii->pf_free_callback)(pblii);
   } else {
      if (pblii->pixmap != None) {
         XFreePixmap(mainDisplay, pblii->pixmap);
      }
      free(pblii);
   }
}

static
int TdgtBmpListGetNumRows(pTdgtBmpList)
   TdgtBmpList *pTdgtBmpList;
{
   int num_bitmaps=ListLength(&pTdgtBmpList->list);
   int num_cols=pTdgtBmpList->num_cols;

   if ((num_bitmaps % num_cols) == 0) {
      return (int)(num_bitmaps / num_cols);
   } else {
      return 1+(int)(num_bitmaps / num_cols);
   }
}

/* --------------------- RedrawTdgtBmpList() --------------------- */

static
void RedrawTdgtBmpListItem(pTdgtBmpList, index, x, y, pblii)
   TdgtBmpList *pTdgtBmpList;
   int index, x, y;
   BmpListItemInfo *pblii;
{
   XGCValues values;

   if (pblii->depth == 1) {
      values.foreground = myFgPixel;
      values.background = myBgPixel;
      values.function = GXcopy;
      values.fill_style = FillStippled;
      values.ts_x_origin = x;
      values.ts_y_origin = y;
      values.stipple = pblii->pixmap;
      XChangeGC(mainDisplay, gTidgetManager.gc, GCForeground | GCBackground |
            GCFunction | GCFillStyle | GCTileStipXOrigin | GCTileStipYOrigin |
            GCStipple, &values);

      XFillRectangle(mainDisplay, pTdgtBmpList->dsp_win, gTidgetManager.gc,
            x, y, pblii->width, pblii->height);

      if (index == pTdgtBmpList->marked_index) {
         values.foreground = myFgPixel^myBgPixel;
         values.fill_style = FillSolid;
         values.function = GXxor;
         XChangeGC(mainDisplay, gTidgetManager.gc,
               GCForeground | GCFillStyle | GCFunction,
               &values);
         XFillRectangle(mainDisplay, pTdgtBmpList->dsp_win, gTidgetManager.gc,
               x, y, pblii->width, pblii->height);
      }
      TidgetManagerResetGC();
   } else {
#ifdef _TGIF_DBG /* debug, do not translate */
      sprintf(gszMsgBox, "Bitmap depth of %1d not supported in %s().\n",
            pblii->depth, "RedrawTdgtBmpListItem");
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
#endif /* _TGIF_DBG */
   }
}

static
void RedrawTdgtBmpListScrollWindow(pTdgtBmpList)
   TdgtBmpList *pTdgtBmpList;
{
   double frac=(double)0, start_frac=(double)0;
   int block_h=0, block_start=0, scr_area_h=pTdgtBmpList->scr_win_info.h;
   int num_rows=TdgtBmpListGetNumRows(pTdgtBmpList);
   int first_visible_row=(pTdgtBmpList->first_index/pTdgtBmpList->num_cols);

   start_frac = (num_rows > 0) ?  (double)(((double)first_visible_row) /
         ((double)num_rows)) : (((double)0));
   /* starting pixel */
   block_start = (int)(scr_area_h * start_frac);

   if (num_rows > pTdgtBmpList->num_visible_lines) {
      frac = (double)(((double)pTdgtBmpList->num_visible_lines) /
            ((double)num_rows));
   } else {
      frac = (double)1;
   }
   if (pTdgtBmpList->first_index+pTdgtBmpList->num_visible_lines >= num_rows) {
      block_h = scr_area_h - block_start;
   } else {
      block_h = (int)(scr_area_h * frac);
   }
   TgDrawScrollBar(mainDisplay, pTdgtBmpList->scr_win, VERT_SCROLLBAR, 0, 0,
         scrollBarW, scr_area_h, start_frac, pTdgtBmpList->num_visible_lines,
         num_rows);
}

static
void RedrawTdgtBmpListDspWindow(pTdgtBmpList)
   TdgtBmpList *pTdgtBmpList;
{
   int i=0, x=0, y=0, gap=pTdgtBmpList->gap;
   CVListElem *pElem=NULL, *pNextElem=NULL;

   XClearWindow(mainDisplay, pTdgtBmpList->dsp_win);

   for (i=0, pElem=ListFirst(&pTdgtBmpList->list); pElem != NULL;
         pElem=ListNext(&pTdgtBmpList->list, pElem), i++) {
      if (i == pTdgtBmpList->first_index) {
         break;
      }
   }
   if (pElem == NULL) return;

   x = y = gap;
   for ( ; pElem != NULL; pElem=pNextElem, i++) {
      BmpListItemInfo *pblii=(BmpListItemInfo*)(pElem->obj);
      BmpListItemInfo *pblii_next=NULL;

      RedrawTdgtBmpListItem(pTdgtBmpList, i, x, y, pblii);

      x += pTdgtBmpList->one_bmp_w + gap;
      pNextElem = ListNext(&pTdgtBmpList->list, pElem);
      if (pNextElem != NULL) {
         pblii_next = (BmpListItemInfo*)(pNextElem->obj);
         if (x+pTdgtBmpList->one_bmp_w > pTdgtBmpList->dsp_win_info.w) {
            x = gap;
            y += pTdgtBmpList->one_bmp_h+gap;
            if (y >= pTdgtBmpList->dsp_win_info.h) {
               break;
            }
         }
      }
   }
}

static
void RedrawTdgtBmpListBaseWindow(pTdgtBmpList)
   TdgtBmpList *pTdgtBmpList;
{
   if (threeDLook) {
      struct BBRec bbox;

      SetBBRec(&bbox, 0, 0, pTdgtBmpList->pti->tci.win_info.w,
            pTdgtBmpList->pti->tci.win_info.h);
      TgDrawThreeDButton(mainDisplay, pTdgtBmpList->pti->tci.win,
            gTidgetManager.gc, &bbox, TGBS_LOWRED, 2, FALSE);
      TidgetManagerResetGC();
   }
}

static
void RedrawTdgtBmpList(pti)
   TidgetInfo *pti;
{
   TdgtBmpList *pTdgtBmpList=(TdgtBmpList*)(pti->tidget);
   XEvent ev;

   RedrawTdgtBmpListScrollWindow(pTdgtBmpList);
   RedrawTdgtBmpListDspWindow(pTdgtBmpList);
   RedrawTdgtBmpListBaseWindow(pTdgtBmpList);

   while (XCheckWindowEvent(mainDisplay, pTdgtBmpList->pti->tci.win,
         ExposureMask, &ev)) ;
}

/* --------------------- TdgtBmpListEventHandler() --------------------- */

static
int TdgtBmpListScrollItemCallback(pv_userdata)
   void *pv_userdata;
   /* returns TRUE to cancel scrolling */
{
   TdgtBmpList *pTdgtBmpList=((TdgtBmpList*)pv_userdata);
   int num_visible_lines=pTdgtBmpList->num_visible_lines;
   int num_cols=pTdgtBmpList->num_cols;
   int length=ListLength(&pTdgtBmpList->list);

   if (pTdgtBmpList->scr_dir == SCRL_UP) {
      if (pTdgtBmpList->first_index == 0) return FALSE;
      pTdgtBmpList->first_index -= num_cols;
   } else {
      if (length <= num_visible_lines ||
            pTdgtBmpList->first_index+num_visible_lines == length) {
         return FALSE;
      }
      pTdgtBmpList->first_index += num_cols;
   }
   RedrawTidget(pTdgtBmpList->pti);
   XSync(mainDisplay, False);

   return FALSE;
}

static
int TdgtBmpListScrollPageCallback(pv_userdata)
   void *pv_userdata;
   /* returns TRUE to cancel scrolling */
{
   TdgtBmpList *pTdgtBmpList=((TdgtBmpList*)pv_userdata);
   int num_visible_lines=pTdgtBmpList->num_visible_lines;
   int length=ListLength(&pTdgtBmpList->list);

   if (pTdgtBmpList->scr_dir == SCRL_UP) {
      if (pTdgtBmpList->first_index == 0) return FALSE;
      pTdgtBmpList->first_index -= num_visible_lines;
      if (pTdgtBmpList->first_index < 0) pTdgtBmpList->first_index = 0;
   } else {
      if (length <= num_visible_lines ||
            pTdgtBmpList->first_index+num_visible_lines == length) {
         return FALSE;
      }
      pTdgtBmpList->first_index += num_visible_lines;
      if (pTdgtBmpList->first_index+num_visible_lines >= length) {
         pTdgtBmpList->first_index = length-num_visible_lines;
      }
   }
   RedrawTidget(pTdgtBmpList->pti);
   XSync(mainDisplay, False);

   return FALSE;
}

static
int DoTdgtBmpListBtnScroll(pTdgtBmpList, scroll_page, scr_dir, pbbox)
   TdgtBmpList *pTdgtBmpList;
   int scroll_page, scr_dir;
   struct BBRec *pbbox;
   /* returns TRUE if the done scrolling */
{
   int num_visible_lines=pTdgtBmpList->num_visible_lines;
   int num_rows=TdgtBmpListGetNumRows(pTdgtBmpList);
   int num_cols=pTdgtBmpList->num_cols;
   ScrollBtnCallbackInfo sbci;

   pTdgtBmpList->scr_dir = scr_dir;
   memset(&sbci, 0, sizeof(ScrollBtnCallbackInfo));

   if (scroll_page) {
      sbci.ms = 200;
      sbci.pv_userdata = pTdgtBmpList;
      sbci.pf_scroll_btn_callback = TdgtBmpListScrollPageCallback;
      if (TgPressButtonLoop(mainDisplay, pTdgtBmpList->scr_win, NULL,
            &sbci)) {
         if (scr_dir == SCRL_UP) {
            if (pTdgtBmpList->first_index == 0) return TRUE;
            pTdgtBmpList->first_index -= num_visible_lines;
            if (pTdgtBmpList->first_index < 0) pTdgtBmpList->first_index = 0;
         } else {
            if (num_rows <= num_visible_lines ||
                  (pTdgtBmpList->first_index/num_cols)+num_visible_lines ==
                  num_rows) {
               return TRUE;
            }
            pTdgtBmpList->first_index += num_visible_lines;
            if ((pTdgtBmpList->first_index/num_cols)+num_visible_lines >=
                  num_rows) {
               pTdgtBmpList->first_index =
                     (num_rows-num_visible_lines)*num_cols;
            }
         }
      }
   } else {
      sbci.ms = 50;
      sbci.pv_userdata = pTdgtBmpList;
      sbci.pf_scroll_btn_callback = TdgtBmpListScrollItemCallback;
      if (TgPressButtonLoop(mainDisplay, pTdgtBmpList->scr_win, pbbox, &sbci)) {
         if (scr_dir == SCRL_UP) {
            if (pTdgtBmpList->first_index == 0) return TRUE;
            pTdgtBmpList->first_index -= num_cols;
         } else {
            if (num_rows <= num_visible_lines ||
                  (pTdgtBmpList->first_index/num_cols)+num_visible_lines ==
                  num_rows) {
               return TRUE;
            }
            pTdgtBmpList->first_index += num_cols;
         }
      }
   }
   return FALSE;
}

static
void DoDragInTdgtBmpList(pTdgtBmpList, button_ev, btn_y, btn_offset)
   TdgtBmpList *pTdgtBmpList;
   XButtonEvent *button_ev;
   int btn_y, btn_offset;
{
   int num_rows=TdgtBmpListGetNumRows(pTdgtBmpList);
   int num_visible_lines=pTdgtBmpList->num_visible_lines;
   double frac=(double)0, start_frac=(double)0;
   int block_h=0, block_start=0, done=FALSE;
   int scr_area_h=pTdgtBmpList->scr_win_info.h;

   if (num_rows <= num_visible_lines) return;

   frac = (double)((double)num_visible_lines / (double)(num_rows));
   block_h = (int)(scr_area_h * frac);

   if (num_rows > pTdgtBmpList->num_visible_lines) {
      frac = (double)((double)pTdgtBmpList->num_visible_lines /
            (double)(num_rows));
      block_h = (int)(((double)(scr_area_h-(scrollBarW<<1))) *
            frac);
   } else {
      frac = 1.0;
      block_h = scr_area_h-(scrollBarW<<1);
   }
   if (threeDLook) {
      block_start = button_ev->y+btn_offset;;
      start_frac = (double)((double)(block_start-scrollBarW) /
            (double)(scr_area_h-(scrollBarW<<1)));
      if (block_start+block_h >= scr_area_h-scrollBarW) {
         pTdgtBmpList->first_index = (num_rows - num_visible_lines) *
               pTdgtBmpList->num_cols;
      } else {
         pTdgtBmpList->first_index = (int)(num_rows * start_frac) *
               pTdgtBmpList->num_cols;
      }
   } else {
      block_start = button_ev->y;
      start_frac = (double)((double)(block_start) / (double)(scr_area_h));
      if (block_start+block_h >= scr_area_h) {
         pTdgtBmpList->first_index = (num_rows - num_visible_lines) *
               pTdgtBmpList->num_cols;
      } else {
         pTdgtBmpList->first_index = (int)(num_rows * start_frac) *
               pTdgtBmpList->num_cols;
      }
   }
   RedrawTidget(pTdgtBmpList->pti);

   XGrabPointer(mainDisplay, pTdgtBmpList->scr_win, False,
         PointerMotionMask | ButtonReleaseMask, GrabModeAsync,
         GrabModeAsync, None, handCursor, CurrentTime);

   while (!done) {
      XEvent ev;

      XNextEvent(mainDisplay, &ev);

      if (ev.type == Expose || ev.type == VisibilityNotify) {
         ExposeEventHandler(&ev, TRUE);
      } else if (ev.type == ButtonRelease) {
         XUngrabPointer(mainDisplay, CurrentTime);
         if (debugNoPointerGrab) XSync(mainDisplay, False);
         done = TRUE;
      } else if (ev.type == MotionNotify) {
         int new_name_first=0, y=ev.xmotion.y;

         if (threeDLook) {
            y += btn_offset;
            start_frac = (double)(((double)(y-scrollBarW)) /
                 ((double)(scr_area_h-(scrollBarW<<1))));

            if (y <= scrollBarW) {
               new_name_first = 0;
            } else if (y+block_h >= scr_area_h-scrollBarW) {
               new_name_first = (num_rows - num_visible_lines) *
                     pTdgtBmpList->num_cols;
            } else {
               new_name_first = (int)(num_rows * start_frac) *
                     pTdgtBmpList->num_cols;
            }
         } else {
            start_frac = (double)(((double)y) / ((double)scr_area_h));

            if (y <= 0) {
               new_name_first = 0;
            } else if (y+block_h >= scr_area_h) {
               new_name_first = (num_rows - num_visible_lines) *
                     pTdgtBmpList->num_cols;
            } else {
               new_name_first = (int)(num_rows * start_frac) *
                     pTdgtBmpList->num_cols;
            }
         }
         if (pTdgtBmpList->first_index != new_name_first) {
            pTdgtBmpList->first_index = new_name_first;
            RedrawTidget(pTdgtBmpList->pti);
         }
         while (XCheckMaskEvent(mainDisplay, PointerMotionMask, &ev)) ;
      }
   }
}

static
void TdgtBmpListScrollHandler(pTdgtBmpList, button_ev)
   TdgtBmpList *pTdgtBmpList;
   XButtonEvent *button_ev;
{
   int do_drag=FALSE, btn_offset=0, scr_area_h=pTdgtBmpList->scr_win_info.h;

   if (!threeDLook && button_ev->button == Button3 &&
         button_ev->type == ButtonPress) {
      if (DoTdgtBmpListBtnScroll(pTdgtBmpList,
            ((button_ev->state & (ShiftMask|ControlMask)) != 0),
            SCRL_UP, NULL)) {
         return;
      }
   } else if (!threeDLook && button_ev->button == Button1 &&
         button_ev->type == ButtonPress) {
      if (DoTdgtBmpListBtnScroll(pTdgtBmpList,
            ((button_ev->state & (ShiftMask|ControlMask)) != 0),
            SCRL_DN, NULL)) {
         return;
      }
   } else if (button_ev->button == Button1 && button_ev->type == ButtonPress) {
      if (button_ev->y < scrollBarW ||
            button_ev->y >= scr_area_h-scrollBarW) {
         int which=0;
         struct BBRec bbox;

         if (button_ev->y < scrollBarW) {
            which = SCRL_UP;
            SetBBRec(&bbox, 0, 0, scrollBarW, scrollBarW);
         } else {
            which = SCRL_DN;
            SetBBRec(&bbox, 0, scr_area_h-scrollBarW, scrollBarW, scr_area_h);
         }
         if (DoTdgtBmpListBtnScroll(pTdgtBmpList, FALSE, which, &bbox)) {
            return;
         }
      } else {
         double start_frac=(double)0;
         int hit=0;
         int num_rows=TdgtBmpListGetNumRows(pTdgtBmpList);
         int first_visible_row=
               (pTdgtBmpList->first_index/pTdgtBmpList->num_cols);

         start_frac = (num_rows > 0) ?
               ((double)(((double)first_visible_row) / (double)num_rows)) :
               ((double)0);
         hit = TgGetScrollHit(button_ev->x, button_ev->y, VERT_SCROLLBAR,
               scrollBarW, scr_area_h, start_frac,
               pTdgtBmpList->num_visible_lines, num_rows, &btn_offset);
         if (hit == 0) {
            do_drag = TRUE;
         } else {
            if (DoTdgtBmpListBtnScroll(pTdgtBmpList, TRUE,
                  (hit < 0 ? SCRL_UP : SCRL_DN), NULL)) {
               return;
            }
         }
      }
   } else if (!threeDLook && button_ev->button == Button2 &&
         button_ev->type == ButtonPress) {
      do_drag = TRUE;
   }
   if (do_drag) {
      DoDragInTdgtBmpList(pTdgtBmpList, button_ev, button_ev->y, btn_offset);
   } else {
      RedrawTidget(pTdgtBmpList->pti);
   }
}

static
int TdgtBmpListControlChar(pTdgtBmpList, key_ev, key_sym)
   TdgtBmpList *pTdgtBmpList;
   XKeyEvent *key_ev;
   KeySym key_sym;
{
   int length=ListLength(&pTdgtBmpList->list);
   int num_visible_lines=pTdgtBmpList->num_visible_lines;
   int num_cols=pTdgtBmpList->num_cols;
   int i=0;

   if (key_ev->state & ControlMask) {
      switch (key_sym) {
      case XK_Left: return (BAD);
      case XK_KP_Left: return (BAD);
      case XK_Up: key_sym = ((unsigned long)'b')&0xff; break;
      case XK_KP_Up: key_sym = ((unsigned long)'b')&0xff; break;
      case XK_Right: return (BAD);
      case XK_KP_Right: return (BAD);
      case XK_Down: key_sym = ((unsigned long)'f')&0xff; break;
      case XK_KP_Down: key_sym = ((unsigned long)'f')&0xff; break;
      case XK_Page_Up: key_sym = ((unsigned long)'b')&0xff; break;
      case XK_KP_Page_Up: key_sym = ((unsigned long)'b')&0xff; break;
      case XK_Page_Down: key_sym = ((unsigned long)'f')&0xff; break;
      case XK_KP_Page_Down: key_sym = ((unsigned long)'f')&0xff; break;
      }
   } else {
      switch (key_sym) {
      case XK_Left: return BAD;
      case XK_KP_Left: return BAD;
      case XK_Up: key_sym = ((unsigned long)'k')&0xff; break;
      case XK_KP_Up: key_sym = ((unsigned long)'k')&0xff; break;
      case XK_Right: return BAD;
      case XK_KP_Right: return BAD;
      case XK_Down: key_sym = ((unsigned long)'j')&0xff; break;
      case XK_KP_Down: key_sym = ((unsigned long)'j')&0xff; break;
      case XK_Page_Up: key_sym = ((unsigned long)'b')&0xff; break;
      case XK_KP_Page_Up: key_sym = ((unsigned long)'b')&0xff; break;
      case XK_Page_Down: key_sym = ((unsigned long)'f')&0xff; break;
      case XK_KP_Page_Down: key_sym = ((unsigned long)'f')&0xff; break;
      }
   }
   switch (key_sym&0xff) {
   case 'w': /* erase */
   case 'y':
      return INVALID;
   case 'n': /* down one */
   case 'j':
      if (pTdgtBmpList->can_select) {
         /*
          * i = (pni->marked_index < length-1) ?
          *       pni->marked_index+1 : length-1;
          */
      } else {
         if (length > num_visible_lines) {
            if (pTdgtBmpList->first_index+num_visible_lines < length) {
               pTdgtBmpList->first_index += num_cols;
            }
         }
      }
      break;
   case 'p': /* up one */
   case 'k':
      if (pTdgtBmpList->can_select) {
         /*
          * i = (pni->marked_index>0) ? pni->marked_index-1 : 0;
          */
      } else {
         if (length > num_visible_lines) {
            if (pTdgtBmpList->first_index > 0) {
               pTdgtBmpList->first_index -= num_cols;
            }
         }
      }
      break;
   case 'd': /* down one page */
   case 'f':
      if (pTdgtBmpList->can_select) {
         /*
          * if (pni->marked_index==INVALID) {
          *    i = (length <= num_visible_lines) ?  length-1 :
          *          num_visible_lines;
          * } else if (pni->marked_index < length-num_visible_lines) {
          *    i = pni->marked_index+num_visible_lines;
          * } else {
          *    i = length-1;
          * }
          */
      } else {
         if (length > num_visible_lines) {
            if (pTdgtBmpList->first_index+num_visible_lines < length) {
               pTdgtBmpList->first_index += num_visible_lines;
               if (pTdgtBmpList->first_index + num_visible_lines > length) {
                  pTdgtBmpList->first_index = length - num_visible_lines;
               }
            }
         }
      }
      break;
   case 'u': /* up one page */
   case 'b':
      if (pTdgtBmpList->can_select) {
         /*
          * i = (pni->marked_index > (num_visible_lines-1)) ?
          *       pni->marked_index-num_visible_lines : 0;
          */
      } else {
         pTdgtBmpList->first_index -= num_visible_lines;
         if (pTdgtBmpList->first_index < 0) pTdgtBmpList->first_index = 0;
      }
      break;
   default: return BAD;
   }
   return i;
}

static
void SpecialKeyInTdgtBmpList(pTdgtBmpList, key_ev, key_sym, pn_changing,
      pn_selected_btn_index)
   TdgtBmpList *pTdgtBmpList;
   XKeyEvent *key_ev;
   KeySym key_sym;
   int *pn_changing, *pn_selected_btn_index;
{
   int saved_first=pTdgtBmpList->first_index;
   int saved_marked=pTdgtBmpList->marked_index;
   int rc=TdgtBmpListControlChar(pTdgtBmpList, key_ev, key_sym);

   if (rc == BAD || rc == INVALID) return;

   if (saved_first != pTdgtBmpList->first_index ||
         saved_marked != pTdgtBmpList->marked_index) {
      RedrawTidget(pTdgtBmpList->pti);
   }
}

static XComposeStatus c_stat;

static
int KeyPressInTdgtBmpList(pTdgtBmpList, key_ev, pn_changing,
      pn_selected_btn_index)
   TdgtBmpList *pTdgtBmpList;
   XKeyEvent *key_ev;
   int *pn_changing, *pn_selected_btn_index;
{
   int length=ListLength(&pTdgtBmpList->list);
   char buf[80];
   KeySym key_sym;
   int has_ch=FALSE;

   has_ch = XLookupString(key_ev, buf, sizeof(buf), &key_sym, &c_stat);
   TranslateKeys(buf, &key_sym);

   if ((key_ev->state & ControlMask) && key_sym == XK_j) {
      SpecialKeyInTdgtBmpList(pTdgtBmpList, key_ev, key_sym, pn_changing,
            pn_selected_btn_index);
   } else if (CharIsCRorLF(key_ev, buf, key_sym, &has_ch)) {
      if (pTdgtBmpList->marked_index != INVALID) {
         TidgetControlNotify(pTdgtBmpList->pti, TDGTNF_LIST_DBL_CLICKED,
               pTdgtBmpList->marked_index, 0);
         return TRUE;
      }
   } else if (CharIsESC(key_ev, buf, key_sym, &has_ch)) {
      if (pTdgtBmpList->can_select) {
         /*
          * *pn_changing = FALSE;
          * *pn_selected_btn_index = GetBtnIndexFromBtnId(BUTTON_CANCEL);
          */
      }
   } else if (CharIsBSorDEL(key_ev, buf, key_sym, &has_ch, FALSE)) {
      if (pTdgtBmpList->can_select) {
         /*
          * if (pni->edit_style == NAMES_SELECT_FILE &&
          *       (pni->faking_dot_dot || pni->pop_from_root)) {
          *    *pn_changing = FALSE;
          *    *pn_selected_btn_index = INVALID;
          * } else {
          *    BackSpaceInNames(pn_changing, pn_selected_btn_index);
          * }
          */
      }
   } else if (CharIsTAB(key_ev, buf, key_sym, &has_ch)) {
      if (pTdgtBmpList->can_select) {
         /* should tab out of focus?! */
      }
   } else if (length != 0 && ((key_sym>'\040' && key_sym<='\177' &&
         (key_ev->state & ControlMask)) || key_sym==XK_Up ||
         key_sym==XK_Down || key_sym==XK_KP_Up || key_sym==XK_KP_Down ||
         key_sym==XK_Page_Up || key_sym==XK_KP_Page_Up ||
         key_sym==XK_Page_Down || key_sym==XK_KP_Page_Down)) {
      SpecialKeyInTdgtBmpList(pTdgtBmpList, key_ev, key_sym, pn_changing,
            pn_selected_btn_index);
   } else if (key_sym>='\040' && key_sym<='\177' && length != 0) {
      if (pTdgtBmpList->can_select) {
         /*
          * CharInNames(buf, pn_changing, pn_selected_btn_index);
          */
      }
      return FALSE;
   }
   return FALSE;
}

static
int TdgtBmpListDspHandler(pTdgtBmpList, button_ev)
   TdgtBmpList *pTdgtBmpList;
   XButtonEvent *button_ev;
{
   int gap=pTdgtBmpList->gap;
   int btn_x=button_ev->x, btn_y=button_ev->y;
   int first_index=pTdgtBmpList->first_index;
   int num_cols=pTdgtBmpList->num_cols;
   int row_index=(btn_y-gap)/(pTdgtBmpList->one_bmp_h+gap);
   int col_index=(btn_x-gap)/(pTdgtBmpList->one_bmp_w+gap);
   int index=first_index+(row_index*num_cols)+col_index;
   int prev_marked_index=pTdgtBmpList->marked_index;
   Time click_time=button_ev->time;

   if (index >= ListLength(&pTdgtBmpList->list)) {
      pTdgtBmpList->marked_index = INVALID;
   } else {
      pTdgtBmpList->marked_index = index;
   }
   if (prev_marked_index != pTdgtBmpList->marked_index) {
      RedrawTdgtBmpListDspWindow(pTdgtBmpList);
      /* send notifications */
   }
   TidgetControlNotify(pTdgtBmpList->pti, TDGTNF_LIST_CLICKED,
         pTdgtBmpList->marked_index, button_ev->button);

   /* now check for double-click */
   if (pTdgtBmpList->marked_index != INVALID &&
         pTdgtBmpList->last_selected_index == pTdgtBmpList->marked_index &&
         (click_time-pTdgtBmpList->last_clicked_time) < doubleClickInterval) {
      /* send notifications */
      TidgetControlNotify(pTdgtBmpList->pti, TDGTNF_LIST_DBL_CLICKED,
            pTdgtBmpList->marked_index, 0);
   }
   memcpy(&pTdgtBmpList->last_clicked_time, &click_time, sizeof(Time));
   pTdgtBmpList->last_selected_index = pTdgtBmpList->marked_index;

   return TRUE;
}

static
int ButtonPressInTdgtBmpList(pTdgtBmpList, button_ev)
   TdgtBmpList *pTdgtBmpList;
   XButtonEvent *button_ev;
{
   if (button_ev->window == pTdgtBmpList->scr_win) {
      TdgtBmpListScrollHandler(pTdgtBmpList, button_ev);
      return TRUE;
   } else if (button_ev->window == pTdgtBmpList->dsp_win) {
      if (pTdgtBmpList->can_select != 0) {
         if ((button_ev->button == Button1 &&
               (pTdgtBmpList->can_select & Button1Mask) == Button1Mask) ||
             (button_ev->button == Button2 &&
               (pTdgtBmpList->can_select & Button2Mask) == Button2Mask) ||
             (button_ev->button == Button3 &&
               (pTdgtBmpList->can_select & Button3Mask) == Button3Mask)) {
            return TdgtBmpListDspHandler(pTdgtBmpList, button_ev);
         }
      }
   }
   return FALSE;
}

static
int TdgtBmpListEventHandler(pti, input, handling_pti)
   TidgetInfo *pti, *handling_pti;
   XEvent *input;
{
   TdgtBmpList *pTdgtBmpList=(TdgtBmpList*)(pti->tidget);

   if (pti != handling_pti) return FALSE;

   if (input->type == Expose) {
      RedrawTidget(pTdgtBmpList->pti);
   } else if (input->type == EnterNotify) {
      if (pTdgtBmpList->mosi.one_line_status) {
         SetStringStatus(pTdgtBmpList->mosi.one_line_str);
      } else {
         MouseOverStatusInfo *pmosi=(&pTdgtBmpList->mosi);

         SetMouseStatus(pmosi->btn_str[0], pmosi->btn_str[1],
               pmosi->btn_str[2]);
      }
   } else if (input->type == LeaveNotify) {
      SetMouseStatus("", "", "");
   } else if (input->type == KeyPress) {
      if (pTdgtBmpList->can_select) {
         /*
          * KeyPressInTdgtBmpList(pTdgtBmpList, &changing,
          *         &selected_btn_index);
          */
      } else {
         return KeyPressInTdgtBmpList(pTdgtBmpList, &input->xkey, NULL,
               NULL);
      }
   } else if (input->type == ButtonPress) {
      return ButtonPressInTdgtBmpList(pTdgtBmpList, &input->xbutton);
   }
   return FALSE;
}

/* --------------------- IsTdgtBmpListEvent() --------------------- */

static
int IsTdgtBmpListEvent(pti, input, ppti_handler_tidget_return)
   TidgetInfo *pti, **ppti_handler_tidget_return;
   XEvent *input;
{
   TdgtBmpList *pTdgtBmpList=(TdgtBmpList*)(pti->tidget);

   if (input->xany.window == pTdgtBmpList->dsp_win ||
         input->xany.window == pTdgtBmpList->scr_win) {
      *ppti_handler_tidget_return = pti;
      return TRUE;
   }
   return FALSE;
}

/* --------------------- DestroyTdgtBmpList() --------------------- */

static
void DestroyTdgtBmpList(pti)
   TidgetInfo *pti;
{
   TdgtBmpList *pTdgtBmpList=(TdgtBmpList*)(pti->tidget);

   TdgtBmpListReset(pTdgtBmpList);

   free(pTdgtBmpList);
}

/* --------------------- MapTdgtBmpList() --------------------- */

static
void MapTdgtBmpList(pti)
   TidgetInfo *pti;
{
   TdgtBmpList *pTdgtBmpList=(TdgtBmpList*)(pti->tidget);

#ifdef MAPBEFORESELECT
   XMapWindow(mainDisplay, pTdgtBmpList->pti->tci.win);
   XSelectInput(mainDisplay, pTdgtBmpList->pti->tci.win, ExposureMask);

   XMapWindow(mainDisplay, pTdgtBmpList->dsp_win);
   XSelectInput(mainDisplay, pTdgtBmpList->dsp_win, KeyPressMask |
         ButtonPressMask | KeyPressMask | ExposureMask |
         EnterWindowMask | LeaveWindowMask);

   XMapWindow(mainDisplay, pTdgtBmpList->scr_win);
   XSelectInput(mainDisplay, pTdgtBmpList->scr_win, KeyPressMask |
         ButtonPressMask | ExposureMask);
#else
   XSelectInput(mainDisplay, pTdgtBmpList->pti->tci.win, ExposureMask);
   XMapWindow(mainDisplay, pTdgtBmpList->pti->tci.win);

   XSelectInput(mainDisplay, pTdgtBmpList->dsp_win, KeyPressMask |
         ButtonPressMask | KeyPressMask | ExposureMask |
         EnterWindowMask | LeaveWindowMask);
   XMapWindow(mainDisplay, pTdgtBmpList->dsp_win);

   XSelectInput(mainDisplay, pTdgtBmpList->scr_win, KeyPressMask |
         ButtonPressMask | ExposureMask);
   XMapWindow(mainDisplay, pTdgtBmpList->scr_win);
#endif
}

/* --------------------- TdgtBmpListMoveResize() --------------------- */

static
void TdgtBmpListMoveResize(pti, x, y, w, h)
   TidgetInfo *pti;
   int x, y, w, h;
{
   TdgtBmpList *pTdgtBmpList=(TdgtBmpList*)(pti->tidget);

   pTdgtBmpList->pti->tci.win_info.x = x;
   pTdgtBmpList->pti->tci.win_info.y = y;
   pTdgtBmpList->pti->tci.win_info.w = w;
   pTdgtBmpList->pti->tci.win_info.h = h;
   XMoveResizeWindow(mainDisplay, pTdgtBmpList->pti->tci.win, x, y, w, h);

   pTdgtBmpList->dsp_win_info.x = windowPadding;
   pTdgtBmpList->dsp_win_info.y = windowPadding;
   pTdgtBmpList->dsp_win_info.w = w-(windowPadding<<1)-scrollBarW;
   pTdgtBmpList->dsp_win_info.h = h-(windowPadding<<1);
   XMoveResizeWindow(mainDisplay, pTdgtBmpList->dsp_win,
         pTdgtBmpList->dsp_win_info.x, pTdgtBmpList->dsp_win_info.y,
         pTdgtBmpList->dsp_win_info.w, pTdgtBmpList->dsp_win_info.h);

   pTdgtBmpList->scr_win_info.x = w-windowPadding-scrollBarW;
   pTdgtBmpList->scr_win_info.y = windowPadding;
   pTdgtBmpList->scr_win_info.w = scrollBarW;
   pTdgtBmpList->scr_win_info.h = h-(windowPadding<<1);
   XMoveResizeWindow(mainDisplay, pTdgtBmpList->scr_win,
         pTdgtBmpList->scr_win_info.x, pTdgtBmpList->scr_win_info.y,
         pTdgtBmpList->scr_win_info.w, pTdgtBmpList->scr_win_info.h);
}

/* --------------------- TdgtBmpListSendCmd() --------------------- */

static
int HandleSelectNext(pTdgtBmpList, cmd_arg)
   TdgtBmpList *pTdgtBmpList;
   int cmd_arg; /* one of TDGTARG_MOVE_* */
{
   int prev_marked_index=pTdgtBmpList->marked_index;
   int num_items=ListLength(&pTdgtBmpList->list);
   int num_cols=pTdgtBmpList->num_cols;
   int col=(prev_marked_index%num_cols);
   int last_row_first_item_index=0, this_row_first_item_index=0;
   int this_row_is_last_row=TRUE;

   if (num_items == 0) return TRUE;

   if (num_items%num_cols == 0) {
      last_row_first_item_index = num_cols * (round(((double)num_items) /
            ((double)num_cols))-1);
   } else {
      last_row_first_item_index = num_cols * ((int)(num_items / num_cols));
   }
   if (prev_marked_index%num_cols == 0) {
      this_row_first_item_index = num_cols *
            round(((double)prev_marked_index) / ((double)num_cols));
   } else {
      this_row_first_item_index = num_cols * ((int)(prev_marked_index /
            num_cols));
   }
   this_row_is_last_row = (this_row_first_item_index+num_cols >= num_items);
   switch (cmd_arg) {
   case TDGTARG_MOVE_UP:
      if (prev_marked_index == INVALID) {
         pTdgtBmpList->marked_index = num_items-1;
      } else if (num_items <= num_cols) {
         /* do nothing */
      } else {
         pTdgtBmpList->marked_index -= num_cols;
         if (pTdgtBmpList->marked_index < 0) {
            pTdgtBmpList->marked_index = last_row_first_item_index+col;
         }
      }
      break;
   case TDGTARG_MOVE_DOWN:
      if (prev_marked_index == INVALID) {
         pTdgtBmpList->marked_index = 0;
      } else if (num_items <= num_cols) {
         /* do nothing */
      } else {
         pTdgtBmpList->marked_index += num_cols;
         if (pTdgtBmpList->marked_index >= num_items) {
            pTdgtBmpList->marked_index = col;
         }
      }
      break;
   case TDGTARG_MOVE_RIGHT:
      if (prev_marked_index == INVALID) {
         pTdgtBmpList->marked_index = 0;
      } else if (this_row_is_last_row) {
         if (prev_marked_index == num_items-1) {
            pTdgtBmpList->marked_index = this_row_first_item_index;
         } else {
            pTdgtBmpList->marked_index++;
         }
      } else if (col == num_cols-1) {
         pTdgtBmpList->marked_index = this_row_first_item_index;
      } else {
         pTdgtBmpList->marked_index++;
      }
      break;
   case TDGTARG_MOVE_LEFT:
      if (prev_marked_index == INVALID) {
         pTdgtBmpList->marked_index = 0;
      } else if (this_row_is_last_row) {
         if (col == 0) {
            pTdgtBmpList->marked_index = num_items-1;
         } else {
            pTdgtBmpList->marked_index--;
         }
      } else if (col == 0) {
         pTdgtBmpList->marked_index = this_row_first_item_index+num_cols-1;
      } else {
         pTdgtBmpList->marked_index--;
      }
      break;
   }
   RedrawTdgtBmpListDspWindow(pTdgtBmpList);

   return TRUE;
}

static
int HandleGetCurSel(pTdgtBmpList, pn_selected_index)
   TdgtBmpList *pTdgtBmpList;
   int *pn_selected_index;
{
   if (pn_selected_index != NULL) {
      *pn_selected_index = pTdgtBmpList->marked_index;
   }
   return TRUE;
}

static
int HandleGetItem(pTdgtBmpList, index, ppblii)
   TdgtBmpList *pTdgtBmpList;
   int index;
   BmpListItemInfo **ppblii;
{
   int i=0;
   CVListElem *pElem=NULL;
   
   if (index == INVALID) return FALSE;

   for (i=0, pElem=ListFirst(&pTdgtBmpList->list); pElem != NULL;
         pElem=ListNext(&pTdgtBmpList->list, pElem), i++) {
      if (i == index) {
         BmpListItemInfo *pblii=(BmpListItemInfo*)(pElem->obj);

         if (ppblii != NULL) {
            *ppblii = pblii;
         }
         return TRUE;
      } else if (i > index) {
         break;
      }
   }
   return FALSE;
}

int TdgtBmpListSendCmd(pti, cmd_type, cmd_arg, pv_cmd_userdata)
   TidgetInfo *pti;
   int cmd_type, cmd_arg;
   void *pv_cmd_userdata;
{
   TdgtBmpList *pTdgtBmpList=(TdgtBmpList*)(pti->tidget);

   switch (cmd_type) {
   case TDGTCMD_LIST_SEL_NEXT:
      return HandleSelectNext(pTdgtBmpList, cmd_arg);
   case TDGTCMD_LIST_RESETCONTENT:
      TdgtBmpListCleanUpEntries(pTdgtBmpList);
      RedrawTidget(pTdgtBmpList->pti);
      return TRUE;
   case TDGTCMD_LIST_GETCURSEL:
      return HandleGetCurSel(pTdgtBmpList, (int*)pv_cmd_userdata);
   case TDGTCMD_LIST_GETITEM:
      return HandleGetItem(pTdgtBmpList, cmd_arg,
            (BmpListItemInfo**)pv_cmd_userdata);
   }
   return FALSE;
}

/* --------------------- TdgtBmpListReset() --------------------- */

void TdgtBmpListReset(pTdgtBmpList)
   TdgtBmpList *pTdgtBmpList;
{
   TdgtBmpListCleanUpEntries(pTdgtBmpList);

   FreeDynStrBuf(&pTdgtBmpList->pti->tci.dyn_str);
}

/* --------------------- CreateTdgtBmpList() --------------------- */

static
void TdgtBmpListCalcGeom(bmp_w, bmp_h, num_cols, num_visible_lines, gap,
      pn_content_w, pn_content_h)
   int bmp_w, bmp_h, num_cols, num_visible_lines, gap;
   int *pn_content_w, *pn_content_h;
   /*
    * The content area includes the bitmap display area and the scrollbar.
    */
{
   int w=gap+((bmp_w+gap)*num_cols), h=gap+((bmp_h+gap)*num_visible_lines);

   /*
    * Display area has *no* windowPadding around it (scrollBarW includes
    *         (windowPadding<<1) already).
    */
   w += scrollBarW;

   if (pn_content_w != NULL) *pn_content_w = w;
   if (pn_content_h != NULL) *pn_content_h = h;
}

TdgtBmpList *CreateTdgtBmpList(parent_win, parent_tidgetinfo, ctl_id, x, y,
      h_pad, v_pad, bmp_w, bmp_h, num_cols, num_visible_lines, gap, can_select,
      pmosi)
   Window parent_win;
   TidgetInfo *parent_tidgetinfo;
   int ctl_id, x, y, h_pad, v_pad, bmp_w, bmp_h, num_cols, num_visible_lines;
   int gap, can_select;
   MouseOverStatusInfo *pmosi;
{
   int bg_pixel=(threeDLook ? myLtGryPixel : myBgPixel), w=0, h=0;
   int content_w=0, content_h=0;
   TdgtBmpList *pTdgtBmpList=NULL;

   TdgtBmpListCalcGeom(bmp_w, bmp_h, num_cols, num_visible_lines, gap,
         &content_w, &content_h);
   w = content_w + (windowPadding<<1) + (h_pad<<1);
   h = content_h + (windowPadding<<1) + (v_pad<<1);

   pTdgtBmpList = (TdgtBmpList*)malloc(sizeof(TdgtBmpList));
   if (pTdgtBmpList == NULL) FailAllocMessage();
   memset(pTdgtBmpList, 0, sizeof(TdgtBmpList));

   pTdgtBmpList->pti = NewTidgetInfo(parent_tidgetinfo, TIDGET_TYPE_BMPL,
         pTdgtBmpList, ctl_id, NULL);
   if ((pTdgtBmpList->pti->tci.win=XCreateSimpleWindow(mainDisplay, parent_win,
         x, y, w, h, brdrW, myBorderPixel, bg_pixel)) == 0) {
      FailToCreateWindowMessage("CreateTdgtBmpList()", NULL, TRUE);
   }
   XSelectInput(mainDisplay, pTdgtBmpList->pti->tci.win, ExposureMask);
   SetTidgetInfoBasic(pTdgtBmpList->pti, TIDGET_TYPE_BMPL, pTdgtBmpList,
         parent_win, x, y, w, h, v_pad, h_pad, TGBS_NORMAL, "");
   TidgetSetCallbacks(pTdgtBmpList->pti,
         RedrawTdgtBmpList, TdgtBmpListEventHandler, IsTdgtBmpListEvent,
         DestroyTdgtBmpList, MapTdgtBmpList, TdgtBmpListMoveResize,
         TdgtBmpListSendCmd);

   CVListInit(&pTdgtBmpList->list);
   if (pmosi != NULL) {
      memcpy(&pTdgtBmpList->mosi, pmosi, sizeof(MouseOverStatusInfo));
   }
   /* Note: scrollBarW accounts for windowPadding */
   if ((pTdgtBmpList->dsp_win=XCreateSimpleWindow(mainDisplay,
         pTdgtBmpList->pti->tci.win, windowPadding, windowPadding,
         w-(windowPadding<<1)-scrollBarW, h-(windowPadding<<1), brdrW,
         myBorderPixel, myBgPixel)) == 0) {
      FailToCreateWindowMessage("CreateTdgtBmpList()", NULL, TRUE);
   }
   XSelectInput(mainDisplay, pTdgtBmpList->dsp_win, KeyPressMask |
         ButtonPressMask | KeyPressMask | ExposureMask |
         EnterWindowMask | LeaveWindowMask);
   pTdgtBmpList->dsp_win_info.x = windowPadding;
   pTdgtBmpList->dsp_win_info.y = windowPadding;
   pTdgtBmpList->dsp_win_info.w = w-(windowPadding<<1)-scrollBarW;
   pTdgtBmpList->dsp_win_info.h = h-(windowPadding<<1);

   if ((pTdgtBmpList->scr_win=XCreateSimpleWindow(mainDisplay,
         pTdgtBmpList->pti->tci.win, w-windowPadding-scrollBarW, windowPadding,
         scrollBarW, h-(windowPadding<<1), brdrW, myBorderPixel, bg_pixel)) ==
         0) {
      FailToCreateWindowMessage("CreateTdgtBmpList()", NULL, TRUE);
   }
   XSelectInput(mainDisplay, pTdgtBmpList->scr_win, KeyPressMask |
         ButtonPressMask | ExposureMask);
   pTdgtBmpList->scr_win_info.x = w-windowPadding-scrollBarW;
   pTdgtBmpList->scr_win_info.y = windowPadding;
   pTdgtBmpList->scr_win_info.w = scrollBarW;
   pTdgtBmpList->scr_win_info.h = h-(windowPadding<<1);

   pTdgtBmpList->can_select = can_select;

   pTdgtBmpList->one_bmp_w = bmp_w;
   pTdgtBmpList->one_bmp_h = bmp_h;
   pTdgtBmpList->gap = gap;

   pTdgtBmpList->first_index = 0;
   pTdgtBmpList->marked_index = INVALID;

   pTdgtBmpList->num_cols = num_cols;
   pTdgtBmpList->num_visible_lines = num_visible_lines;

   return pTdgtBmpList;
}

/* --------------------- TdgtBmpListCleanUpEntries() --------------------- */

int TdgtBmpListCleanUpEntries(pTdgtBmpList)
   TdgtBmpList *pTdgtBmpList;
{
   CVListElem *pElem=NULL;
   
   if (ListLength(&pTdgtBmpList->list) > 0) {
      TidgetSetDirty(pTdgtBmpList->pti, TRUE);
   }
   for (pElem=ListFirst(&pTdgtBmpList->list); pElem != NULL;
         pElem=ListNext(&pTdgtBmpList->list, pElem)) {
      BmpListItemInfo *pblii=(BmpListItemInfo*)(pElem->obj);

      FreeBmpListItemInfo(pblii);
   }
   ListUnlinkAll(&pTdgtBmpList->list);

   pTdgtBmpList->first_index = 0;
   pTdgtBmpList->marked_index = INVALID;

   return TRUE;
}

/* --------------------- TdgtBmpListScrollToBottom() --------------------- */

int TdgtBmpListScrollToBottom(pTdgtBmpList)
   TdgtBmpList *pTdgtBmpList;
{
   int length=ListLength(&pTdgtBmpList->list);

   if (pTdgtBmpList->num_visible_lines < length) {
      pTdgtBmpList->first_index = length - pTdgtBmpList->num_visible_lines;
   } else {
      pTdgtBmpList->first_index = 0;
   }
   return TRUE;
}

/* --------------------- TdgtBmpListAddItem() --------------------- */

int TdgtBmpListAddItem(pTdgtBmpList, pblii)
   TdgtBmpList *pTdgtBmpList;
   BmpListItemInfo *pblii;
{
   ListAppend(&pTdgtBmpList->list, pblii);

   TidgetSetDirty(pTdgtBmpList->pti, TRUE);

   RedrawTidget(pTdgtBmpList->pti);

   return TRUE;
}

/* --------------------- Init & Clean Up --------------------- */

int InitTdgtBmpList()
{
   return TRUE;
}

void CleanUpTdgtBmpList()
{
}

