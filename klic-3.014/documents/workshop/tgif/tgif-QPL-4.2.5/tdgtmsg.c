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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/tdgtmsg.c,v 1.9 2011/05/16 16:22:00 william Exp $
 */

#define _INCLUDE_FROM_TDGTMSG_C_

#include "tgifdefs.h"
#include "patchlvl.h"
#include "tidget.h"

#include "choose.e"
#include "cursor.e"
#include "font.e"
#include "menu.e"
#include "msg.e"
#include "raster.e"
#include "rect.e"
#include "setup.e"
#include "tdgtmsg.e"
#include "text.e"
#include "tidget.e"
#include "util.e"

static void RedrawTdgtMsg ARGS_DECL((TidgetInfo *pti));
static int TdgtMsgEventHandler ARGS_DECL((TidgetInfo *pti, XEvent *input,
                TidgetInfo *handling_pti));
static int IsTdgtMsgEvent ARGS_DECL((TidgetInfo *pti, XEvent *input,
                TidgetInfo **ppti_handler_tidget_return));
static void DestroyTdgtMsg ARGS_DECL((TidgetInfo *pti));
static void MapTdgtMsg ARGS_DECL((TidgetInfo *pti));
static void TdgtMsgMoveResize ARGS_DECL((TidgetInfo *pti, int x, int y,
                int w, int h));
static int TdgtMsgSendCmd ARGS_DECL((TidgetInfo *pti, int cmd_type,
                int cmd_arg, void *pv_cmd_userdata));

/* --------------------- RedrawTdgtMsg() --------------------- */

static
void TdgtMsgDrawCaption(pTdgtMsg)
   TdgtMsg *pTdgtMsg;
{
   int x=windowPadding+pTdgtMsg->pti->tci.h_pad;
   int y=windowPadding+pTdgtMsg->pti->tci.v_pad;
   int just=pTdgtMsg->just;
   int win_w=pTdgtMsg->pti->tci.win_info.w;
   char *psz=NULL, *psz_cur=TdgtMsgGetText(pTdgtMsg);

   if (boldMsgFontSet != NULL || boldMsgFontPtr != NULL) {
       if (boldMsgFontPtr != NULL) {
          XSetFont(mainDisplay, gTidgetManager.gc, boldMsgFontPtr->fid);
       }
       y += boldMsgFontAsc;
   } else if (msgFontSet != NULL || msgFontPtr != NULL) {
       XSetFont(mainDisplay, gTidgetManager.gc, msgFontPtr->fid);
       y += msgFontAsc;
   } else {
       y += defaultFontAsc;
   }
   switch (just) {
   case JUST_L: break;
   case JUST_C: x = (win_w>>1); break;
   case JUST_R: x = win_w-windowPadding-pTdgtMsg->pti->tci.h_pad; break;
   }
   do {
      int line_w=0;

      psz = strchr(psz_cur, '\n');
      if (psz != NULL) *psz = '\0';

      switch (just) {
      case JUST_L:
         DrawBoldMsgString(mainDisplay, pTdgtMsg->pti->tci.win,
               gTidgetManager.gc, x, y, psz_cur, strlen(psz_cur));
         break;
      case JUST_C:
         line_w = BoldMsgTextWidth(boldMsgFontPtr, psz_cur, strlen(psz_cur));
         DrawBoldMsgString(mainDisplay, pTdgtMsg->pti->tci.win,
               gTidgetManager.gc, x-(line_w>>1), y, psz_cur, strlen(psz_cur));
         break;
      case JUST_R:
         line_w = BoldMsgTextWidth(boldMsgFontPtr, psz_cur, strlen(psz_cur));
         DrawBoldMsgString(mainDisplay, pTdgtMsg->pti->tci.win,
               gTidgetManager.gc, x-line_w, y, psz_cur, strlen(psz_cur));
         break;
      }
      if (boldMsgFontSet != NULL || boldMsgFontPtr != NULL) {
          y += boldMsgFontHeight;
      } else if (msgFontSet != NULL || msgFontPtr != NULL) {
          y += msgFontHeight;
      } else {
          y += defaultFontHeight;
      }
      if (psz != NULL) {
         *psz++ = '\n';
         psz_cur = psz;
      } else {
         psz_cur = NULL;
      }
   } while (psz != NULL);
   TidgetManagerResetGC();
}

static
void RedrawTdgtMsg(pti)
   TidgetInfo *pti;
{
   TdgtMsg *pTdgtMsg=(TdgtMsg*)(pti->tidget);

   XClearWindow(mainDisplay, pTdgtMsg->pti->tci.win);

   if (threeDLook) {
      struct BBRec bbox;
   
      SetBBRec(&bbox, 0, 0, pTdgtMsg->pti->tci.win_info.w,
            pTdgtMsg->pti->tci.win_info.h);
      switch (pTdgtMsg->pti->tci.state) {
      case TGBS_NORMAL: break;
      default:
         TgDrawThreeDButton(mainDisplay, pTdgtMsg->pti->tci.win,
               gTidgetManager.gc, &bbox, pTdgtMsg->pti->tci.state, 2, TRUE);
         TidgetManagerResetGC();
         break;
      }
   } else {
      /*
       * XDrawRectangle(mainDisplay, pTdgtMsg->pti->tci.win, gTidgetManager.gc,
       *       0, 0, pTdgtMsg->pti->tci.win_info.w,
       *       pTdgtMsg->pti->tci.win_info.h);
       */
   }
   TdgtMsgDrawCaption(pTdgtMsg);
}

/* --------------------- TdgtMsgEventHandler() --------------------- */

static
int TdgtMsgEventHandler(pti, input, handling_pti)
   TidgetInfo *pti, *handling_pti;
   XEvent *input;
{
   TdgtMsg *pTdgtMsg=(TdgtMsg*)(pti->tidget);

   if (pti != handling_pti) return FALSE;

   if (input->type == Expose) {
      XEvent ev;

      RedrawTdgtMsg(pTdgtMsg->pti);
      while (XCheckWindowEvent(mainDisplay, pTdgtMsg->pti->tci.win,
            ExposureMask, &ev)) ;
   }
   return FALSE;
}

/* --------------------- IsTdgtMsgEvent() --------------------- */

static
int IsTdgtMsgEvent(pti, input, ppti_handler_tidget_return)
   TidgetInfo *pti, **ppti_handler_tidget_return;
   XEvent *input;
{
   TdgtMsg *pTdgtMsg=(TdgtMsg*)(pti->tidget);

   if (input->xany.window == pTdgtMsg->pti->tci.win) {
      *ppti_handler_tidget_return = pti;
      return TRUE;
   }
   return FALSE;
}

/* --------------------- DestroyTdgtMsg() --------------------- */

static
void DestroyTdgtMsg(pti)
   TidgetInfo *pti;
{
   TdgtMsg *pTdgtMsg=(TdgtMsg*)(pti->tidget);

   TdgtMsgReset(pTdgtMsg);

   free(pTdgtMsg);
}

/* --------------------- MapTdgtMsg() --------------------- */

static
void MapTdgtMsg(pti)
   TidgetInfo *pti;
{
   TdgtMsg *pTdgtMsg=(TdgtMsg*)(pti->tidget);

#ifdef MAPBEFORESELECT
   XMapWindow(mainDisplay, pTdgtMsg->pti->tci.win);
   XSelectInput(mainDisplay, pTdgtMsg->pti->tci.win,
         ButtonReleaseMask | ButtonPressMask | KeyPressMask | ExposureMask |
         EnterWindowMask | LeaveWindowMask);
#else
   XSelectInput(mainDisplay, pTdgtMsg->pti->tci.win,
         ButtonReleaseMask | ButtonPressMask | KeyPressMask | ExposureMask |
         EnterWindowMask | LeaveWindowMask);
   XMapWindow(mainDisplay, pTdgtMsg->pti->tci.win);
#endif
}

/* --------------------- TdgtMsgMoveResize() --------------------- */

static
void TdgtMsgMoveResize(pti, x, y, w, h)
   TidgetInfo *pti;
   int x, y, w, h;
{
   TdgtMsg *pTdgtMsg=(TdgtMsg*)(pti->tidget);

   /* there should be no need to resize a button */
   pTdgtMsg->pti->tci.win_info.x = x;
   pTdgtMsg->pti->tci.win_info.y = y;
   pTdgtMsg->pti->tci.win_info.w = w;
   pTdgtMsg->pti->tci.win_info.h = h;
   XMoveResizeWindow(mainDisplay, pTdgtMsg->pti->tci.win, x, y, w, h);
}

/* --------------------- TdgtMsgSendCmd() --------------------- */

static
int TdgtMsgSendCmd(pti, cmd_type, cmd_arg, pv_cmd_userdata)
   TidgetInfo *pti;
   int cmd_type, cmd_arg;
   void *pv_cmd_userdata;
{
   TdgtMsg *pTdgtMsg=(TdgtMsg*)(pti->tidget);

   if (pTdgtMsg != NULL) {
   }
   return FALSE;
}

/* --------------------- TdgtMsgReset() --------------------- */

void TdgtMsgReset(pTdgtMsg)
   TdgtMsg *pTdgtMsg;
{
   FreeDynStrBuf(&pTdgtMsg->pti->tci.dyn_str);
}

/* --------------------- CreateTdgtMsg() --------------------- */

static
void TdgtMsgCalcGeom(caption, pn_w, pn_h)
   char *caption;
   int *pn_w, *pn_h;
{
   int w=0, h=0;
   char *psz=NULL, *psz_cur=NULL, *dup_caption=UtilStrDup(caption);

   if (dup_caption == NULL) FailAllocMessage();

   psz_cur = dup_caption;
   do {
      int line_w=0;

      psz = strchr(psz_cur, '\n');
      if (psz != NULL) *psz = '\0';
      line_w = BoldMsgTextWidth(boldMsgFontPtr, psz_cur, strlen(psz_cur));
      if (line_w > w) w = line_w;
      if (boldMsgFontSet != NULL || boldMsgFontPtr != NULL) {
          h += boldMsgFontHeight;
      } else if (msgFontSet != NULL || msgFontPtr != NULL) {
          h += msgFontHeight;
      } else {
          h += defaultFontHeight;
      }
      if (psz != NULL) {
         *psz++ = '\n';
         psz_cur = psz;
      } else {
         psz_cur = NULL;
      }
   } while (psz != NULL);

   UtilFree(dup_caption);

   if (pn_w != NULL) *pn_w = w;
   if (pn_h != NULL) *pn_h = h;
}

TdgtMsg *CreateTdgtMsg(parent_win, parent_tidgetinfo, ctl_id, x, y, h_pad,
      v_pad, state, caption, just)
   Window parent_win;
   TidgetInfo *parent_tidgetinfo;
   int ctl_id, x, y, h_pad, v_pad, state, just;
   char *caption;
{
   int bg_pixel=(threeDLook ? myLtGryPixel : myBgPixel), w=0, h=0;
   TdgtMsg *pTdgtMsg=NULL;

   if (caption != NULL) {
      int msg_w=0, msg_h=0;

      TdgtMsgCalcGeom(caption, &msg_w, &msg_h);
      w = msg_w + (windowPadding<<1) + (h_pad<<1);
      h = msg_h + (windowPadding<<1) + (v_pad<<1);
   }
   pTdgtMsg = (TdgtMsg*)malloc(sizeof(TdgtMsg));
   if (pTdgtMsg == NULL) FailAllocMessage();
   memset(pTdgtMsg, 0, sizeof(TdgtMsg));

   pTdgtMsg->pti = NewTidgetInfo(parent_tidgetinfo, TIDGET_TYPE_MSG, pTdgtMsg,
         ctl_id, NULL);
   if ((pTdgtMsg->pti->tci.win=XCreateSimpleWindow(mainDisplay, parent_win,
         x, y, w, h, brdrW, myBorderPixel, bg_pixel)) == 0) {
      FailToCreateWindowMessage("CreateTdgtMsg()", NULL, TRUE);
      return NULL;
   }
   XSelectInput(mainDisplay, pTdgtMsg->pti->tci.win,
         ButtonReleaseMask | ButtonPressMask | KeyPressMask | ExposureMask |
         EnterWindowMask | LeaveWindowMask);
   SetTidgetInfoBasic(pTdgtMsg->pti, TIDGET_TYPE_MSG, pTdgtMsg, parent_win,
         x, y, w, h, h_pad, v_pad, state, caption);
   TidgetSetCallbacks(pTdgtMsg->pti,
         RedrawTdgtMsg, TdgtMsgEventHandler, IsTdgtMsgEvent, DestroyTdgtMsg,
         MapTdgtMsg, TdgtMsgMoveResize, TdgtMsgSendCmd);

   pTdgtMsg->just = just;

   return pTdgtMsg;
}

/* --------------------- TdgtMsgSetText() --------------------- */

int TdgtMsgSetText(pTdgtMsg, str)
   TdgtMsg *pTdgtMsg;
   char *str;
{
   RedrawTdgtMsg(pTdgtMsg->pti);
   if (str != NULL) {
      FreeDynStrBuf(&(pTdgtMsg->pti->tci.dyn_str));
      DynStrSet(&(pTdgtMsg->pti->tci.dyn_str), str);
   }
   return TRUE;
}

/* --------------------- TdgtMsgGetText() --------------------- */

char *TdgtMsgGetText(pTdgtMsg)
   TdgtMsg *pTdgtMsg;
{
   return pTdgtMsg->pti->tci.dyn_str.s;
}

/* --------------------- TdgtMsgSetState() --------------------- */

int TdgtMsgSetState(pTdgtMsg, new_state)
   TdgtMsg *pTdgtMsg;
   int new_state;
{
   int need_to_redraw=(pTdgtMsg->pti->tci.state != new_state);

   pTdgtMsg->pti->tci.state = new_state;
   if (need_to_redraw) {
      RedrawTdgtMsg(pTdgtMsg->pti);
   }
   return TRUE;
}

/* --------------------- TdgtMsgGetState() --------------------- */

int TdgtMsgGetState(pTdgtMsg)
   TdgtMsg *pTdgtMsg;
{
   return pTdgtMsg->pti->tci.state;
}

/* --------------------- Init & Clean Up --------------------- */

int InitTdgtMsg()
{
   return TRUE;
}

void CleanUpTdgtMsg()
{
}

