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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/tdgtbase.c,v 1.8 2011/05/16 16:21:59 william Exp $
 */

#define _INCLUDE_FROM_TDGTBASE_C_

#include "tgifdefs.h"
#include "patchlvl.h"
#include "tidget.h"

#include "cursor.e"
#include "menu.e"
#include "msg.e"
#include "raster.e"
#include "rect.e"
#include "setup.e"
#include "tdgtbase.e"
#include "text.e"
#include "tidget.e"
#include "util.e"

static void RedrawTdgtBase ARGS_DECL((TidgetInfo *pti));
static int TdgtBaseEventHandler ARGS_DECL((TidgetInfo *pti, XEvent *input,
                TidgetInfo *handling_pti));
static int IsTdgtBaseEvent ARGS_DECL((TidgetInfo *pti, XEvent *input,
                TidgetInfo **ppti_handler_tidget_return));
static void DestroyTdgtBase ARGS_DECL((TidgetInfo *pti));
static void MapTdgtBase ARGS_DECL((TidgetInfo *pti));
static void TdgtBaseMoveResize ARGS_DECL((TidgetInfo *pti, int x, int y,
                int w, int h));
static int TdgtBaseSendCmd ARGS_DECL((TidgetInfo *pti, int cmd_type,
                int cmd_arg, void *pv_cmd_userdata));

/* --------------------- RedrawTdgtBase() --------------------- */

static
void RedrawTdgtBase(pti)
   TidgetInfo *pti;
{
   TdgtBase *pTdgtBase=(TdgtBase*)(pti->tidget);
   CVListElem *pElem=NULL;

   XClearWindow(mainDisplay, pTdgtBase->pti->tci.win);

   if (threeDLook) {
      struct BBRec bbox;
   
      SetBBRec(&bbox, 0, 0, pTdgtBase->pti->tci.win_info.w,
            pTdgtBase->pti->tci.win_info.h);
      switch (pTdgtBase->pti->tci.state) {
      case TGBS_NORMAL: break;
      default:
         TgDrawThreeDButton(mainDisplay, pTdgtBase->pti->tci.win,
               gTidgetManager.gc, &bbox, pTdgtBase->pti->tci.state, 2, FALSE);
         break;
      }
   } else {
      XDrawRectangle(mainDisplay, pTdgtBase->pti->tci.win, gTidgetManager.gc, 0,
            0, pTdgtBase->pti->tci.win_info.w, pTdgtBase->pti->tci.win_info.h);
   }
   for (pElem=ListFirst(&pTdgtBase->pti->tci.clist); pElem != NULL;
         pElem=ListNext(&pTdgtBase->pti->tci.clist, pElem)) {
      TidgetInfo *pti=(TidgetInfo*)(pElem->obj);

      if (pti != NULL) {
         RedrawTidget(pti);
      }
   }
   TidgetManagerResetGC();
}

/* --------------------- TdgtBaseEventHandler() --------------------- */

static
int TdgtBaseEventHandler(pti, input, handling_pti)
   TidgetInfo *pti, *handling_pti;
   XEvent *input;
{
   TdgtBase *pTdgtBase=(TdgtBase*)(pti->tidget);
   CVListElem *pElem=NULL;

   if (pti == handling_pti) {
      if (input->type == KeyPress &&
            pTdgtBase->pf_key_press_ev_handler_callback != NULL) {
         return (pTdgtBase->pf_key_press_ev_handler_callback)(pTdgtBase, input);
      }
      if (input->xany.window == pTdgtBase->pti->tci.win) {
         if (IsWM_DELETE_WINDOW(input)) {
            if (pTdgtBase->pf_wm_delete_ev_handler_callback != NULL) {
               (pTdgtBase->pf_wm_delete_ev_handler_callback)(pTdgtBase);
            }
         } else if (input->type == Expose) {
            XEvent ev;

            RedrawTdgtBase(pTdgtBase->pti);
            while (XCheckWindowEvent(mainDisplay, pTdgtBase->pti->tci.win,
                  ExposureMask, &ev)) ;
         } else if (input->type == EnterNotify) {
            /* may be we need to do something here */
         } else if (input->type == LeaveNotify) {
            /* may be we need to do something here */
         }
         return FALSE;
      }
   }
   for (pElem=ListFirst(&pTdgtBase->pti->tci.clist); pElem != NULL;
         pElem=ListNext(&pTdgtBase->pti->tci.clist, pElem)) {
      TidgetInfo *pti=(TidgetInfo*)(pElem->obj);

      if (pti != NULL) {
         if (TdgtBaseEventHandler(pti, input, handling_pti)) {
            return TRUE;
         }
      }
   }
   return FALSE;
}

/* --------------------- IsTdgtBaseEvent() --------------------- */

static
int IsTdgtBaseEvent(pti, input, ppti_handler_tidget_return)
   TidgetInfo *pti, **ppti_handler_tidget_return;
   XEvent *input;
{
   TdgtBase *pTdgtBase=(TdgtBase*)(pti->tidget);

   if (input->type == KeyPress &&
         pTdgtBase->pf_key_press_ev_handler_callback != NULL) {
      CVListElem *pElem=NULL;

      if (input->xany.window == pTdgtBase->pti->tci.win) {
         *ppti_handler_tidget_return = pti;
         return TRUE;
      }
      for (pElem=ListFirst(&pTdgtBase->pti->tci.clist); pElem != NULL;
            pElem=ListNext(&pTdgtBase->pti->tci.clist, pElem)) {
         TidgetInfo *pti=(TidgetInfo*)(pElem->obj);

         if (pti != NULL) {
            TidgetInfo *dummy_pti=NULL;

            if (IsTidgetEvent(pti, input, &dummy_pti)) {
               *ppti_handler_tidget_return = pti;
               return TRUE;
            }
         }
      }
   }
   return FALSE;
}

/* --------------------- DestroyTdgtBase() --------------------- */

static
void DestroyTdgtBase(pti)
   TidgetInfo *pti;
{
   TdgtBase *pTdgtBase=(TdgtBase*)(pti->tidget);

   TdgtBaseReset(pTdgtBase);

   free(pTdgtBase);
}

/* --------------------- MapTdgtBase() --------------------- */

static
void MapTdgtBase(pti)
   TidgetInfo *pti;
{
   TdgtBase *pTdgtBase=(TdgtBase*)(pti->tidget);
   CVListElem *pElem=NULL;

#ifdef MAPBEFORESELECT
   XMapWindow(mainDisplay, pTdgtBase->pti->tci.win);
   XSelectInput(mainDisplay, pTdgtBase->win, StructureNotifyMask |
         ButtonReleaseMask | ButtonPressMask | KeyPressMask | ExposureMask |
         EnterWindowMask | LeaveWindowMask);
#else
   XSelectInput(mainDisplay, pTdgtBase->pti->tci.win, StructureNotifyMask |
         ButtonReleaseMask | ButtonPressMask | KeyPressMask | ExposureMask |
         EnterWindowMask | LeaveWindowMask);
   XMapWindow(mainDisplay, pTdgtBase->pti->tci.win);
#endif
   for (pElem=ListFirst(&pTdgtBase->pti->tci.clist); pElem != NULL;
         pElem=ListNext(&pTdgtBase->pti->tci.clist, pElem)) {
      TidgetInfo *pti=(TidgetInfo*)(pElem->obj);

      if (pti != NULL) {
         MapTidget(pti);
      }
   }
}

/* --------------------- TdgtBaseMoveResize() --------------------- */

static
void TdgtBaseMoveResize(pti, x, y, w, h)
   TidgetInfo *pti;
   int x, y, w, h;
{
   TdgtBase *pTdgtBase=(TdgtBase*)(pti->tidget);

   /* there should be no need to resize a button */
   pTdgtBase->pti->tci.win_info.x = x;
   pTdgtBase->pti->tci.win_info.y = y;
   pTdgtBase->pti->tci.win_info.w = w;
   pTdgtBase->pti->tci.win_info.h = h;
   XMoveResizeWindow(mainDisplay, pTdgtBase->pti->tci.win, x, y, w, h);
}

/* --------------------- TdgtBaseSendCmd() --------------------- */

static
int TdgtBaseSendCmd(pti, cmd_type, cmd_arg, pv_cmd_userdata)
   TidgetInfo *pti;
   int cmd_type, cmd_arg;
   void *pv_cmd_userdata;
{
   TdgtBase *pTdgtBase=(TdgtBase*)(pti->tidget);

   if (pTdgtBase != NULL) {
      switch (cmd_type) {
      case TDGTCMD_CAN_HANDLE_FOCUS_CHANGE:
         return (pTdgtBase->pf_focus_callback != NULL);
      case TDGTCMD_REMOVE_FOCUS:
         return (pTdgtBase->pf_focus_callback)(pTdgtBase, cmd_type, cmd_arg,
               pv_cmd_userdata);
      }
   }
   return FALSE;
}

/* --------------------- TdgtBaseReset() --------------------- */

void TdgtBaseReset(pTdgtBase)
   TdgtBase *pTdgtBase;
{
   CVListElem *pElem=NULL;

   for (pElem=ListFirst(&pTdgtBase->pti->tci.clist); pElem != NULL;
         pElem=ListNext(&pTdgtBase->pti->tci.clist, pElem)) {
      TidgetInfo *pti=(TidgetInfo*)(pElem->obj);

      if (pti != NULL) {
         DestroyTidget(&pti);
      }
   }
   ListUnlinkAll(&pTdgtBase->pti->tci.clist);

   ResetTidgetCommon(&pTdgtBase->pti->tci);
}

/* --------------------- CreateTdgtBase() --------------------- */

TdgtBase *CreateTdgtBase(parent_win, parent_tidgetinfo, ctl_id, x, y, w, h,
      h_pad, v_pad, state, caption)
   Window parent_win;
   TidgetInfo *parent_tidgetinfo;
   int ctl_id, x, y, w, h, h_pad, v_pad, state;
   char *caption;
{
   int bg_pixel=(threeDLook ? myLtGryPixel : myBgPixel);
   XWMHints wmhints;
   XSetWindowAttributes win_attrs;
   TdgtBase *pTdgtBase=NULL;

   pTdgtBase = (TdgtBase*)malloc(sizeof(TdgtBase));
   if (pTdgtBase == NULL) FailAllocMessage();
   memset(pTdgtBase, 0, sizeof(TdgtBase));

   pTdgtBase->pti = NewTidgetInfo(parent_tidgetinfo, TIDGET_TYPE_BASE,
         pTdgtBase, ctl_id, NULL);
   if ((pTdgtBase->pti->tci.win=XCreateSimpleWindow(mainDisplay, parent_win,
         x, y, w, h, brdrW, myBorderPixel, bg_pixel)) == 0) {
      FailToCreateWindowMessage("CreateTdgtBase()", NULL, TRUE);
   }
   XSelectInput(mainDisplay, pTdgtBase->pti->tci.win, StructureNotifyMask |
         ButtonReleaseMask | ButtonPressMask | KeyPressMask | ExposureMask |
         EnterWindowMask | LeaveWindowMask);
   SetTidgetInfoBasic(pTdgtBase->pti, TIDGET_TYPE_BASE, pTdgtBase, parent_win,
         x, y, w, h, h_pad, v_pad, state, caption);
   TidgetCanHaveChildren(pTdgtBase->pti, TRUE);
   TidgetSetCallbacks(pTdgtBase->pti,
         RedrawTdgtBase, TdgtBaseEventHandler, IsTdgtBaseEvent, DestroyTdgtBase,
         MapTdgtBase, TdgtBaseMoveResize, TdgtBaseSendCmd);

   XDefineCursor(mainDisplay, pTdgtBase->pti->tci.win, defaultCursor);

   memset(&win_attrs, 0, sizeof(XSetWindowAttributes));
   win_attrs.save_under = True;
   win_attrs.colormap = mainColormap;
   XChangeWindowAttributes(mainDisplay, pTdgtBase->pti->tci.win,
         CWSaveUnder | CWOverrideRedirect | CWColormap, &win_attrs);

   memset(&wmhints, 0, sizeof(XWMHints));
   wmhints.flags = InputHint;
   wmhints.input = True;
   XSetWMHints(mainDisplay, pTdgtBase->pti->tci.win, &wmhints);

   RegisterWM_DELETE_WINDOW(pTdgtBase->pti->tci.win);
   if (caption != NULL && *caption != '\0') {
      XStoreName(mainDisplay, pTdgtBase->pti->tci.win, caption);
   }
   return pTdgtBase;
}

/* --------------------- TdgtBaseSetText() --------------------- */

int TdgtBaseSetText(pTdgtBase, str)
   TdgtBase *pTdgtBase;
   char *str;
{
   RedrawTdgtBase(pTdgtBase->pti);
   if (str != NULL) {
      FreeDynStrBuf(&(pTdgtBase->pti->tci.dyn_str));
      DynStrSet(&(pTdgtBase->pti->tci.dyn_str), str);
   }
   return TRUE;
}

/* --------------------- TdgtBaseGetText() --------------------- */

char *TdgtBaseGetText(pTdgtBase)
   TdgtBase *pTdgtBase;
{
   return pTdgtBase->pti->tci.dyn_str.s;
}

/* --------------------- TdgtBaseSetState() --------------------- */

int TdgtBaseSetState(pTdgtBase, new_state)
   TdgtBase *pTdgtBase;
   int new_state;
{
   int need_to_redraw=(pTdgtBase->pti->tci.state != new_state);

   pTdgtBase->pti->tci.state = new_state;
   if (need_to_redraw) {
      RedrawTdgtBase(pTdgtBase->pti);
   }
   return TRUE;
}

/* --------------------- TdgtBaseGetState() --------------------- */

int TdgtBaseGetState(pTdgtBase)
   TdgtBase *pTdgtBase;
{
   return pTdgtBase->pti->tci.state;
}

/* --------------------- TdgtBaseAddChild() --------------------- */

int TdgtBaseAddChild(pTdgtBase, pChildTidgetInfo)
   TdgtBase *pTdgtBase;
   TidgetInfo *pChildTidgetInfo;
{
   return ListAppend(&pTdgtBase->pti->tci.clist, pChildTidgetInfo);
}

/* ------------------- TdgtBaseWillHandleKeyPressEvents() ------------------- */

void TdgtBaseWillHandleKeyPressEvents(pTdgtBase,
      pf_key_press_ev_handler_callback)
   TdgtBase *pTdgtBase;
   TdgtBaseKeyPressEvHandlerCallbackFunc *pf_key_press_ev_handler_callback;
{
   pTdgtBase->pf_key_press_ev_handler_callback =
         pf_key_press_ev_handler_callback;
}

/* ------------------- TdgtBaseSetWmDeleteCallback() ------------------- */

void TdgtBaseSetWmDeleteCallback(pTdgtBase,
      pf_wm_delete_ev_handler_callback)
   TdgtBase *pTdgtBase;
   TdgtBaseWmDeleteEvHandlerCallbackFunc *pf_wm_delete_ev_handler_callback;
{
   pTdgtBase->pf_wm_delete_ev_handler_callback =
         pf_wm_delete_ev_handler_callback;
}

/* ------------------- TdgtBaseSetFocusCallback() ------------------- */

void TdgtBaseSetFocusCallback(pTdgtBase, pf_focus_callback)
   TdgtBase *pTdgtBase;
   TdgtBaseFocusCallbackFunc *pf_focus_callback;
{
   pTdgtBase->pf_focus_callback = pf_focus_callback;
}

/* --------------------- Init & Clean Up --------------------- */

int InitTdgtBase()
{
   return TRUE;
}

void CleanUpTdgtBase()
{
}

