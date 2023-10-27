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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/tdgtdraw.c,v 1.7 2011/05/16 16:22:00 william Exp $
 */

#define _INCLUDE_FROM_TDGTDRAW_C_

#include "tgifdefs.h"
#include "patchlvl.h"
#include "tidget.h"

#include "button.e"
#include "color.e"
#include "cursor.e"
#include "font.e"
#include "menu.e"
#include "msg.e"
#include "raster.e"
#include "rect.e"
#include "setup.e"
#include "tdgtdraw.e"
#include "tidget.e"
#include "util.e"

static void RedrawTdgtDraw ARGS_DECL((TidgetInfo *pti));
static int TdgtDrawEventHandler ARGS_DECL((TidgetInfo *pti, XEvent *input,
                TidgetInfo *handling_pti));
static int IsTdgtDrawEvent ARGS_DECL((TidgetInfo *pti, XEvent *input,
                TidgetInfo **ppti_handler_tidget_return));
static void DestroyTdgtDraw ARGS_DECL((TidgetInfo *pti));
static void MapTdgtDraw ARGS_DECL((TidgetInfo *pti));
static void TdgtDrawMoveResize ARGS_DECL((TidgetInfo *pti, int x, int y,
                int w, int h));
static int TdgtDrawSendCmd ARGS_DECL((TidgetInfo *pti, int cmd_type,
                int cmd_arg, void *pv_cmd_userdata));

/* --------------------- RedrawTdgtDraw() --------------------- */

static
void RedrawTdgtDraw(pti)
   TidgetInfo *pti;
{
   TdgtDraw *pTdgtDraw=(TdgtDraw*)(pti->tidget);

   XClearWindow(mainDisplay, pTdgtDraw->pti->tci.win);

   if (threeDLook) {
      struct BBRec bbox;

      SetBBRec(&bbox, 0, 0, pTdgtDraw->pti->tci.win_info.w,
            pTdgtDraw->pti->tci.win_info.h);
      switch (pTdgtDraw->pti->tci.state) {
      case TGBS_NORMAL: break;
      default:
         TgDrawThreeDButton(mainDisplay, pTdgtDraw->pti->tci.win,
               gTidgetManager.gc, &bbox, pTdgtDraw->pti->tci.state, 2, FALSE);
         TidgetManagerResetGC();
         break;
      }
   } else {
      /*
       * XDrawRectangle(mainDisplay, pTdgtDraw->pti->tci.win, gTidgetManager.gc,
       *       0, 0, pTdgtDraw->pti->tci.win_info.w,
       *       pTdgtDraw->pti->tci.win_info.h);
       */
   }
   if (pTdgtDraw->pf_redraw_callback != NULL) {
      if ((pTdgtDraw->pf_redraw_callback)(pTdgtDraw)) {
         /* do nothing */
      }
   }
}

/* --------------------- TdgtDrawEventHandler() --------------------- */

static
int TdgtDrawEventHandler(pti, input, handling_pti)
   TidgetInfo *pti, *handling_pti;
   XEvent *input;
{
   TdgtDraw *pTdgtDraw=(TdgtDraw*)(pti->tidget);

   if (pti != handling_pti) return FALSE;

   if (input->type == Expose) {
      XEvent ev;

      RedrawTdgtDraw(pTdgtDraw->pti);
      while (XCheckWindowEvent(mainDisplay, pTdgtDraw->pti->tci.win,
            ExposureMask, &ev)) ;
   } else if (pTdgtDraw->pf_ev_handler_callback != NULL) {
      return (pTdgtDraw->pf_ev_handler_callback)(pTdgtDraw, input);
   }
   return FALSE;
}

/* --------------------- IsTdgtDrawEvent() --------------------- */

static
int IsTdgtDrawEvent(pti, input, ppti_handler_tidget_return)
   TidgetInfo *pti, **ppti_handler_tidget_return;
   XEvent *input;
{
   TdgtDraw *pTdgtDraw=(TdgtDraw*)(pti->tidget);

   if (input->xany.window == pTdgtDraw->pti->tci.win) {
      *ppti_handler_tidget_return = pti;
      return TRUE;
   }
   return FALSE;
}

/* --------------------- DestroyTdgtDraw() --------------------- */

static
void DestroyTdgtDraw(pti)
   TidgetInfo *pti;
{
   TdgtDraw *pTdgtDraw=(TdgtDraw*)(pti->tidget);

   TdgtDrawReset(pTdgtDraw);

   free(pTdgtDraw);
}

/* --------------------- MapTdgtDraw() --------------------- */

static
void MapTdgtDraw(pti)
   TidgetInfo *pti;
{
   TdgtDraw *pTdgtDraw=(TdgtDraw*)(pti->tidget);

#ifdef MAPBEFORESELECT
   XMapWindow(mainDisplay, pTdgtDraw->pti->tci.win);
   XSelectInput(mainDisplay, pTdgtDraw->pti->tci.win,
         ButtonReleaseMask | ButtonPressMask | KeyPressMask | ExposureMask |
         PointerMotionMask | EnterWindowMask | LeaveWindowMask);
#else
   XSelectInput(mainDisplay, pTdgtDraw->pti->tci.win,
         ButtonReleaseMask | ButtonPressMask | KeyPressMask | ExposureMask |
         PointerMotionMask | EnterWindowMask | LeaveWindowMask);
   XMapWindow(mainDisplay, pTdgtDraw->pti->tci.win);
#endif
}

/* --------------------- TdgtDrawMoveResize() --------------------- */

static
void TdgtDrawMoveResize(pti, x, y, w, h)
   TidgetInfo *pti;
   int x, y, w, h;
{
   TdgtDraw *pTdgtDraw=(TdgtDraw*)(pti->tidget);

   /* there should be no need to resize a button */
   pTdgtDraw->pti->tci.win_info.x = x;
   pTdgtDraw->pti->tci.win_info.y = y;
   pTdgtDraw->pti->tci.win_info.w = w;
   pTdgtDraw->pti->tci.win_info.h = h;
   XMoveResizeWindow(mainDisplay, pTdgtDraw->pti->tci.win, x, y, w, h);
}

/* --------------------- TdgtDrawSendCmd() --------------------- */

static
int TdgtDrawSendCmd(pti, cmd_type, cmd_arg, pv_cmd_userdata)
   TidgetInfo *pti;
   int cmd_type, cmd_arg;
   void *pv_cmd_userdata;
{
   TdgtDraw *pTdgtDraw=(TdgtDraw*)(pti->tidget);

   if (pTdgtDraw->pf_sendcmd_callback != NULL) {
      return ((pTdgtDraw->pf_sendcmd_callback)(pTdgtDraw, cmd_type, cmd_arg,
            pv_cmd_userdata));
   }
   return FALSE;
}

/* --------------------- TdgtDrawReset() --------------------- */

void TdgtDrawReset(pTdgtDraw)
   TdgtDraw *pTdgtDraw;
{
   if (pTdgtDraw->pf_reset_callback != NULL) {
      if ((pTdgtDraw->pf_reset_callback)(pTdgtDraw)) {
         /* do nothing */
      }
   }
}

/* --------------------- CreateTdgtDraw() --------------------- */

TdgtDraw *CreateTdgtDraw(parent_win, parent_tidgetinfo, ctl_id, x, y, client_w,
      client_h, h_pad, v_pad, state, pv_userdata)
   Window parent_win;
   TidgetInfo *parent_tidgetinfo;
   int ctl_id, x, y, client_w, client_h, h_pad, v_pad, state;
   void *pv_userdata;
{
   TdgtDraw *pTdgtDraw=NULL;
   int w=client_w+(windowPadding<<1)+(h_pad<<1);
   int h=client_h+(windowPadding<<1)+(v_pad<<1);

   pTdgtDraw = (TdgtDraw*)malloc(sizeof(TdgtDraw));
   if (pTdgtDraw == NULL) FailAllocMessage();
   memset(pTdgtDraw, 0, sizeof(TdgtDraw));

   pTdgtDraw->pti = NewTidgetInfo(parent_tidgetinfo, TIDGET_TYPE_DRAW,
         pTdgtDraw, ctl_id, NULL);
   if ((pTdgtDraw->pti->tci.win=XCreateSimpleWindow(mainDisplay, parent_win,
         x, y, w, h, brdrW, myBorderPixel, myBgPixel)) == 0) {
      FailToCreateWindowMessage("CreateTdgtDraw()", NULL, TRUE);
   }
   XSelectInput(mainDisplay, pTdgtDraw->pti->tci.win,
         ButtonReleaseMask | ButtonPressMask | KeyPressMask | ExposureMask |
         PointerMotionMask | EnterWindowMask | LeaveWindowMask);
   SetTidgetInfoBasic(pTdgtDraw->pti, TIDGET_TYPE_DRAW, pTdgtDraw, parent_win,
         x, y, w, h, h_pad, v_pad, state, NULL);
   TidgetSetCallbacks(pTdgtDraw->pti,
         RedrawTdgtDraw, TdgtDrawEventHandler, IsTdgtDrawEvent, DestroyTdgtDraw,
         MapTdgtDraw, TdgtDrawMoveResize, TdgtDrawSendCmd);

   pTdgtDraw->client_area.x = windowPadding + h_pad;
   pTdgtDraw->client_area.y = windowPadding + v_pad;
   pTdgtDraw->client_area.w = w-(windowPadding<<1)-(h_pad<<1);
   pTdgtDraw->client_area.h = h-(windowPadding<<1)-(v_pad<<1);

   pTdgtDraw->pv_userdata = pv_userdata;

   return pTdgtDraw;
}

/* --------------------- TdgtDrawSetUserData() --------------------- */

int TdgtDrawSetUserData(pTdgtDraw, pv_userdata)
   TdgtDraw *pTdgtDraw;
   void *pv_userdata;
{
   pTdgtDraw->pv_userdata = pv_userdata;
   RedrawTdgtDraw(pTdgtDraw->pti);

   return TRUE;
}

/* --------------------- TdgtDrawGetUserData() --------------------- */

char *TdgtDrawGetUserData(pTdgtDraw)
   TdgtDraw *pTdgtDraw;
{
   return (char*)(pTdgtDraw->pv_userdata);
}

/* --------------------- TdgtDrawSetState() --------------------- */

int TdgtDrawSetState(pTdgtDraw, new_state)
   TdgtDraw *pTdgtDraw;
   int new_state;
{
   int need_to_redraw=(pTdgtDraw->pti->tci.state != new_state);

   pTdgtDraw->pti->tci.state = new_state;
   if (need_to_redraw) {
      RedrawTdgtDraw(pTdgtDraw->pti);
   }
   return TRUE;
}

/* --------------------- TdgtDrawGetState() --------------------- */

int TdgtDrawGetState(pTdgtDraw)
   TdgtDraw *pTdgtDraw;
{
   return pTdgtDraw->pti->tci.state;
}

/* --------------------- TdgtDrawSetRedrawCallback() --------------------- */

void TdgtDrawSetRedrawCallback(pTdgtDraw, pf_redraw_callback)
   TdgtDraw *pTdgtDraw;
   TdgtDrawRedrawCallbackFunc *pf_redraw_callback;
{
   pTdgtDraw->pf_redraw_callback = pf_redraw_callback;
}

/* --------------------- TdgtDrawSetEvHandlerCallback() --------------------- */

void TdgtDrawSetEvHandlerCallback(pTdgtDraw, pf_ev_handler_callback)
   TdgtDraw *pTdgtDraw;
   TdgtDrawEvHandlerCallbackFunc *pf_ev_handler_callback;
{
   pTdgtDraw->pf_ev_handler_callback = pf_ev_handler_callback;
}

/* --------------------- TdgtDrawSetResetCallback() --------------------- */

void TdgtDrawSetResetCallback(pTdgtDraw, pf_reset_callback)
   TdgtDraw *pTdgtDraw;
   TdgtDrawResetCallbackFunc *pf_reset_callback;
{
   pTdgtDraw->pf_reset_callback = pf_reset_callback;
}

/* --------------------- TdgtDrawSetSendCmdCallback() --------------------- */

void TdgtDrawSetSendCmdCallback(pTdgtDraw, pf_sendcmd_callback)
   TdgtDraw *pTdgtDraw;
   TdgtDrawSendCmdCallbackFunc *pf_sendcmd_callback;
{
   pTdgtDraw->pf_sendcmd_callback = pf_sendcmd_callback;
}

/* --------------------- Init & Clean Up --------------------- */

int InitTdgtDraw()
{
   return TRUE;
}

void CleanUpTdgtDraw()
{
}
