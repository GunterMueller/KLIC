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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/tidget.c,v 1.8 2011/05/16 16:22:00 william Exp $
 */

#define _INCLUDE_FROM_TIDGET_C_

#include "tgifdefs.h"
#include "patchlvl.h"
#include "tidget.h"

#include "font.e"
#include "msg.e"
#include "raster.e"
#include "setup.e"
#include "tdgtbase.e"
#include "tdgtbmpl.e"
#include "tdgtbrow.e"
#include "tdgtbtn.e"
#include "tdgtdraw.e"
#include "tdgtlist.e"
#include "tdgtmsg.e"
#include "tdgtsedt.e"
#include "text.e"
#include "tidget.e"
#include "util.e"

TidgetManager gTidgetManager;

/* --------------------- TidgetWindowNotify --------------------- */

void TidgetWindowNotify(win, ctl_id, nf_type, nf_arg, nf_arg2)
   Window win;
   int ctl_id, nf_type, nf_arg, nf_arg2;
{
   XClientMessageEvent client_ev;

   memset(&client_ev, 0, sizeof(XClientMessageEvent));
   client_ev.type = ClientMessage;
   client_ev.window = win;
   client_ev.message_type = gTidgetManager.notify_atom;
   client_ev.format = 32;
   client_ev.data.l[0] = (long)(TDGT_NOTIFY & 0x0ffffffff);
   client_ev.data.l[1] = (long)(ctl_id & 0x0ffffffff);
   client_ev.data.l[2] = (long)(nf_type & 0x0ffffffff);
   client_ev.data.l[3] = (long)(nf_arg & 0x0ffffffff);
   client_ev.data.l[4] = (long)(nf_arg2 & 0x0ffffffff);
   XSendEvent(mainDisplay, win, False, NoEventMask, (XEvent*)&client_ev);
}

/* --------------------- TidgetNotify --------------------- */

void TidgetNotify(pti, ctl_id, nf_type, nf_arg, nf_arg2)
   TidgetInfo *pti; /* referring to a base tidget */
   int ctl_id, nf_type, nf_arg, nf_arg2;
{
   TidgetWindowNotify(pti->tci.win, ctl_id, nf_type, nf_arg, nf_arg2);
}

/* --------------------- TidgetControlNotify --------------------- */

static
void DoTidgetControlNotify(pti, ctl_id, nf_type, nf_arg, nf_arg2)
   TidgetInfo *pti; /* referring to tidget, e.g., a button */
   int ctl_id, nf_type, nf_arg, nf_arg2;
{
   if (pti->tci.parent_tidgetinfo == NULL) {
      TidgetWindowNotify(pti->tci.parent_win, ctl_id, nf_type, nf_arg, nf_arg2);
   } else if (pti->tci.parent_tidgetinfo->type == TIDGET_TYPE_BASE) {
      TidgetWindowNotify(pti->tci.parent_win, ctl_id, nf_type, nf_arg, nf_arg2);
   } else {
      DoTidgetControlNotify(pti->tci.parent_tidgetinfo, ctl_id, nf_type, nf_arg,
            nf_arg2);
   }
}

void TidgetControlNotify(pti, nf_type, nf_arg, nf_arg2)
   TidgetInfo *pti; /* referring to tidget, e.g., a button */
   int nf_type, nf_arg, nf_arg2;
{
   DoTidgetControlNotify(pti, pti->tci.ctl_id, nf_type, nf_arg, nf_arg2);
}

/* --------------------- IsTdgtWindowNotifyEvent --------------------- */

int IsTdgtWindowNotifyEvent(win, pXEv, pTdgtNtfy)
   Window win;
   XEvent *pXEv;
   TdgtNtfy *pTdgtNtfy;
{
   if (pXEv->xany.window == win && pXEv->type == ClientMessage) {
      if (pXEv->xclient.message_type == gTidgetManager.notify_atom &&
            pXEv->xclient.format == 32 &&
            pXEv->xclient.data.l[0] == TDGT_NOTIFY) {
         if (pTdgtNtfy != NULL) {
            memset(pTdgtNtfy, 0, sizeof(TdgtNtfy));

            pTdgtNtfy->ctl_id = (int)(pXEv->xclient.data.l[1]);
            pTdgtNtfy->nf_type = (int)(pXEv->xclient.data.l[2]);
            pTdgtNtfy->nf_arg = (int)(pXEv->xclient.data.l[3]);
            pTdgtNtfy->nf_arg2 = (int)(pXEv->xclient.data.l[4]);
         }
         return TRUE;
      }
   }
   return FALSE;
}

/* --------------------- IsTdgtNotifyEvent --------------------- */

int IsTdgtNotifyEvent(pti, pXEv, pTdgtNtfy)
   TidgetInfo *pti; /* referring to a base tidget */
   XEvent *pXEv;
   TdgtNtfy *pTdgtNtfy;
{
   return IsTdgtWindowNotifyEvent(pti->tci.win, pXEv, pTdgtNtfy);
}

/* --------------------- TidgetDrawFocusRect --------------------- */

void TidgetDrawFocusRect(pti, pswi)
   TidgetInfo *pti;
   SimpleWinInfo *pswi;
{
   Window win=pti->tci.win;
   XGCValues values;
   int x=pswi->x, y=pswi->y, w=pswi->w, h=pswi->h;

   XSetDashes(mainDisplay, revDefaultGC, 0, dashList[8], dashListLength[8]);
   values.line_style = LineOnOffDash;
   XChangeGC(mainDisplay, revDefaultGC, GCLineStyle, &values);

   XDrawRectangle(mainDisplay, win, revDefaultGC, x-1, y-1, w+2, h+2);

   values.line_style = LineSolid;
   XChangeGC(mainDisplay, revDefaultGC, GCLineStyle, &values);
}

/* --------------------- RedrawTidget --------------------- */

void RedrawTidget(pti)
   TidgetInfo *pti;
{
   if (pti->tci.pf_redraw_callback != NULL) {
      if (!pti->tci.redraw_disabled) {
         (pti->tci.pf_redraw_callback)(pti);
         TidgetSetDirty(pti, FALSE);
      }
   }
}

/* --------------------- TidgetEventHandler --------------------- */

int TidgetEventHandler(pti, input, pti_handler_tidget)
   /*
    * Returns TRUE if the event has been handled.
    */
   TidgetInfo *pti, *pti_handler_tidget;
   XEvent *input;
{
   if (pti == pti_handler_tidget) {
      if (pti->tci.pf_ev_handler_callback != NULL) {
         (pti->tci.pf_ev_handler_callback)(pti, input, pti_handler_tidget);
      }
      return TRUE;
   }
   /* iterate through tidget-based child windows */
   if (pti->tci.can_have_children) {
      CVListElem *pElem=NULL;

      for (pElem=ListFirst(&pti->tci.clist); pElem != NULL;
            pElem=ListNext(&pti->tci.clist, pElem)) {
         TidgetInfo *child_pti=(TidgetInfo*)(pElem->obj);

         if (TidgetEventHandler(child_pti, input, pti_handler_tidget)) {
            return TRUE;
         }
      }
   }
   return FALSE;
}

/* --------------------- TestAndHandleEventForTidget --------------------- */

#ifdef NOT_DEFINED
int TestAndHandleEventForTidget(pti, input)
   TidgetInfo *pti;
   XEvent *input;
{
   if (input->xany.window == pti->tci.win) {
      if (pti->tci.pf_ev_handler_callback != NULL) {
         (pti->tci.pf_ev_handler_callback)(pti, input);
      }
      return TRUE;
   }
   /*
    * If a tidget has non-tidget child windows, then it should
    *         have an is_event callback function.
    */
   if (pti->tci.pf_is_event_callback != NULL &&
         (pti->tci.pf_is_event_callback)(pti, input)) {
      /* gets here if the tidget has other non-tidget child windows */
      if (pti->tci.pf_ev_handler_callback != NULL) {
         (pti->tci.pf_ev_handler_callback)(pti, input);
      }
      return TRUE;
   }
   /* iterate through tidget-based child windows */
   if (pti->tci.can_have_children) {
      CVListElem *pElem=NULL;

      for (pElem=ListFirst(&pti->tci.clist); pElem != NULL;
            pElem=ListNext(&pti->tci.clist, pElem)) {
         TidgetInfo *child_pti=(TidgetInfo*)(pElem->obj);

         if (TestAndHandleEventForTidget(child_pti, input)) {
            return TRUE;
         }
      }
   }
   return FALSE;
}
#endif /* NOT_DEFINED */

/* --------------------- IsTidgetEvent --------------------- */

int IsTidgetEvent(pti, input, ppti_handler_tidget_return)
   TidgetInfo *pti, **ppti_handler_tidget_return;
   XEvent *input;
{
   if (input->xany.window == pti->tci.win) {
      *ppti_handler_tidget_return = pti;
      return TRUE;
   }
   /*
    * If a tidget has non-tidget child windows, then it should
    *         have an is_event callback function.
    */
   if (pti->tci.pf_is_event_callback != NULL &&
         (pti->tci.pf_is_event_callback)(pti, input,
         ppti_handler_tidget_return)) {
      return TRUE;
   }
   /* iterate through tidget-based child windows */
   if (pti->tci.can_have_children) {
      CVListElem *pElem=NULL;

      for (pElem=ListFirst(&pti->tci.clist); pElem != NULL;
            pElem=ListNext(&pti->tci.clist, pElem)) {
         TidgetInfo *child_pti=(TidgetInfo*)(pElem->obj);

         if (IsTidgetEvent(child_pti, input, ppti_handler_tidget_return)) {
            return TRUE;
         }
      }
   }
   return FALSE;
}

/* --------------------- DestroyTidget --------------------- */

void DestroyTidget(ppti)
   TidgetInfo **ppti;
{
   TidgetInfo *pti=(*ppti);

   if (pti->tci.parent_tidgetinfo == NULL) {
      CVListElem *pElem=ListFind(&gTidgetManager.toplevel_tidgets, pti, NULL);

      if (pElem == NULL) {
#ifdef _TGIF_DBG /* debug, do not translate */
         fprintf(stderr, "Cannot find a TidgetInfo in DestroyTidget().\n");
#endif /* _TGIF_DBG */
      } else {
         ListUnlink(&gTidgetManager.toplevel_tidgets, pElem);
         free(pElem);
      }
   }
   if (pti->tci.pf_destroy_callback != NULL) {
      (pti->tci.pf_destroy_callback)(pti);
   }
   free(pti);
   *ppti = NULL;
}

/* --------------------- SetTidgetInfoBasic --------------------- */

void SetTidgetInfoBasic(pti, tidget_type, tidget, parent_win, x, y, w, h, h_pad,
      v_pad, state, caption)
   TidgetInfo *pti;
   int tidget_type, x, y, w, h, h_pad, v_pad, state;
   void *tidget;
   Window parent_win;
   char *caption;
{
   pti->type = tidget_type;
   pti->tidget = tidget;

   pti->tci.parent_win = parent_win;
   pti->tci.win_info.x = x;
   pti->tci.win_info.y = y;
   pti->tci.win_info.w = w;
   pti->tci.win_info.h = h;
   pti->tci.h_pad = h_pad;
   pti->tci.v_pad = v_pad;
   pti->tci.content_w = w-(windowPadding<<1)-(h_pad<<1);
   pti->tci.content_h = h-(windowPadding<<1)-(v_pad<<1);
   pti->tci.state = state;

   if (caption != NULL) {
      DynStrSet(&(pti->tci.dyn_str), caption);
   }
}

/* --------------------- TidgetCanHaveChildren --------------------- */

void TidgetCanHaveChildren(pti, can_have_children)
   TidgetInfo *pti;
   int can_have_children;
{
   pti->tci.can_have_children = can_have_children;
}

/* --------------------- TidgetSetCallbacks --------------------- */

void TidgetSetCallbacks(pti, pf_redraw_callback, pf_ev_handler_callback,
      pf_is_event_callback, pf_destroy_callback, pf_map_callback,
      pf_moveresize_callback, pf_sendcmd_callback)
   TidgetInfo *pti;
   TidgetRedrawCallbackFunc *pf_redraw_callback;
   TidgetEvHandlerCallbackFunc *pf_ev_handler_callback;
   TidgetIsEventCallbackFunc *pf_is_event_callback;
   TidgetDestroyCallbackFunc *pf_destroy_callback;
   TidgetMapCallbackFunc *pf_map_callback;
   TidgetMoveResizeCallbackFunc *pf_moveresize_callback;
   TidgetSendCmdCallbackFunc *pf_sendcmd_callback;
{
   pti->tci.pf_redraw_callback = pf_redraw_callback;
   pti->tci.pf_ev_handler_callback = pf_ev_handler_callback;
   pti->tci.pf_is_event_callback = pf_is_event_callback;
   pti->tci.pf_destroy_callback = pf_destroy_callback;
   pti->tci.pf_map_callback = pf_map_callback;
   pti->tci.pf_moveresize_callback = pf_moveresize_callback;
   pti->tci.pf_sendcmd_callback = pf_sendcmd_callback;
}

/* --------------------- NewTidgetInfo --------------------- */

TidgetInfo *NewTidgetInfo(parent_tidgetinfo, type, tidget, ctl_id, userdata)
   TidgetInfo *parent_tidgetinfo;
   int type, ctl_id;
   void *tidget;
   void *userdata;
{
   TidgetInfo *pti=(TidgetInfo*)malloc(sizeof(TidgetInfo));

   if (pti == NULL) FailAllocMessage();
   memset(pti, 0, sizeof(TidgetInfo));

   pti->type = type;
   pti->tidget = tidget;
   pti->userdata = userdata;

   pti->tci.ctl_id = ctl_id;
   pti->tci.parent_tidgetinfo = parent_tidgetinfo;
   CVListInit(&pti->tci.clist);

   if (parent_tidgetinfo == NULL) {
      ListAppend(&gTidgetManager.toplevel_tidgets, pti);
   } else if (parent_tidgetinfo->type == TIDGET_TYPE_BASE &&
         parent_tidgetinfo->tidget != NULL) {
      if (!TdgtBaseAddChild((TdgtBase*)(parent_tidgetinfo->tidget), pti)) {
         /* this should not happen */
      }
   }
   return pti;
}

/* --------------------- MapTidget --------------------- */

void MapTidget(pti)
   TidgetInfo *pti;
{
   if (pti->tci.pf_map_callback != NULL) {
      (pti->tci.pf_map_callback)(pti);
   }
}

/* --------------------- TidgetMoveResize --------------------- */

void TidgetMoveResize(pti, x, y, w, h)
   TidgetInfo *pti;
   int x, y, w, h;
{
   if (pti->tci.pf_moveresize_callback != NULL) {
      (pti->tci.pf_moveresize_callback)(pti, x, y, w, h);
   }
}

/* --------------------- ResetTidgetCommon --------------------- */

void ResetTidgetCommon(ptci)
   TidgetCommonInfo *ptci;
{
   TidgetInfo *saved_parent_tidgetinfo=NULL;

   if (ptci == NULL) return;

   saved_parent_tidgetinfo = ptci->parent_tidgetinfo;
   XDestroyWindow(mainDisplay, ptci->win);

   FreeDynStrBuf(&(ptci->dyn_str));

   memset(ptci, 0, sizeof(TidgetCommonInfo));

   ptci->parent_tidgetinfo = saved_parent_tidgetinfo;
   CVListInit(&ptci->clist);
}

/* --------------------- TidgetSendCmd --------------------- */

int TidgetSendCmd(pti, cmd_type, cmd_arg, pv_cmd_userdata)
   TidgetInfo *pti;
   int cmd_type, cmd_arg;
   void *pv_cmd_userdata;
{
   if (pti->tci.pf_sendcmd_callback != NULL) {
      return (pti->tci.pf_sendcmd_callback)(pti, cmd_type, cmd_arg,
            pv_cmd_userdata);
   }
   return FALSE;
}

/* --------------------- TidgetEnableRedraw --------------------- */

int TidgetDisableRedraw(pti, redraw_disabled)
   TidgetInfo *pti;
   int redraw_disabled;
{
   int prev_redraw_disabled=(pti->tci.redraw_disabled);

   pti->tci.redraw_disabled = redraw_disabled;

   if (!redraw_disabled && prev_redraw_disabled && pti->tci.dirty) {
      RedrawTidget(pti);
   }
   return prev_redraw_disabled;
}

/* --------------------- TidgetSetDirty --------------------- */

int TidgetSetDirty(pti, dirty)
   TidgetInfo *pti;
   int dirty;
{
   int prev_dirty=(pti->tci.dirty);

   pti->tci.dirty = dirty;

   return prev_dirty;
}

/* --------------------- TidgetHasFocus --------------------- */

static int gnTidgetHasFocus=FALSE;

int TidgetHasFocus()
{
   return gnTidgetHasFocus;
}

/* --------------------- TidgetSetHasFocus --------------------- */

int TidgetSetHasFocus(has_focus)
   int has_focus;
{
   int prev_has_focus=gnTidgetHasFocus;

   gnTidgetHasFocus = has_focus;

   return prev_has_focus;
}

/* --------------------- TidgetGetFocusWindow --------------------- */

static Window gFocusWindow=None;

Window TidgetGetFocusWindow()
{
   return gFocusWindow;
}

/* --------------------- TidgetSetFocusWindow --------------------- */

Window TidgetSetFocusWindow(win)
   Window win;
{
   Window prev_win=gFocusWindow;

   gFocusWindow = win;

   return prev_win;
}

/* --------------------- TidgetIsFocus --------------------- */

int TidgetIsFocus(pti)
   TidgetInfo *pti;
{
   if (gFocusWindow == None) return FALSE;

   return (pti->tci.win == gFocusWindow);
}

/* --------------------- TidgetGetFontInfo --------------------- */

void TidgetGetFontInfo(pnFontIndex, pnSzUnit)
   int *pnFontIndex, *pnSzUnit;
{
   if (pnFontIndex != NULL) *pnFontIndex = FONT_HEL;
   if (pnSzUnit != NULL) *pnSzUnit = FontSizeToSzUnit(14);
}

/* --------------------- TidgetGetFontInfoGivenStyle --------------------- */

void TidgetGetFontInfoGivenStyle(font_style, pp_font_ptr, pn_font_width,
      pn_font_height, pn_font_asc, pn_font_des)
   int font_style, *pn_font_width, *pn_font_height, *pn_font_asc, *pn_font_des;
   XFontStruct **pp_font_ptr;
{
   int font_width=defaultFontWidth;
   int font_height=defaultFontHeight;
   int font_asc=defaultFontAsc;
   int font_des=defaultFontDes;
   XFontStruct *font_ptr=defaultFontPtr;

   if (msgFontSet != NULL || msgFontPtr != NULL) {
      font_width = msgFontWidth;
      font_height = msgFontHeight;
      font_asc = msgFontAsc;
      font_des = msgFontDes;
      font_ptr = msgFontPtr;
   }
   switch (font_style) {
   case STYLE_NR:
      if (msgFontSet != NULL || msgFontPtr != NULL) {
         font_width = msgFontWidth;
         font_height = msgFontHeight;
         font_asc = msgFontAsc;
         font_des = msgFontDes;
         font_ptr = msgFontPtr;
      }
      break;
   case STYLE_BR:
      if (boldMsgFontSet != NULL || boldMsgFontPtr != NULL) {
         font_width = boldMsgFontWidth;
         font_height = boldMsgFontHeight;
         font_asc = boldMsgFontAsc;
         font_des = boldMsgFontDes;
         font_ptr = boldMsgFontPtr;
      }
      break;
   case STYLE_NI:
      if (italicMsgFontSet != NULL || italicMsgFontPtr != NULL) {
         font_width = italicMsgFontWidth;
         font_height = italicMsgFontHeight;
         font_asc = italicMsgFontAsc;
         font_des = italicMsgFontDes;
         font_ptr = italicMsgFontPtr;
      }
      break;
   case STYLE_BI:
      if (boldItalicMsgFontSet != NULL || boldItalicMsgFontPtr != NULL) {
         font_width = boldItalicMsgFontWidth;
         font_height = boldItalicMsgFontHeight;
         font_asc = boldItalicMsgFontAsc;
         font_des = boldItalicMsgFontDes;
         font_ptr = boldItalicMsgFontPtr;
      }
      break;
   }
   if (pp_font_ptr != NULL) *pp_font_ptr = font_ptr;
   if (pn_font_width != NULL) *pn_font_width = font_width;
   if (pn_font_height != NULL) *pn_font_height = font_height;
   if (pn_font_asc != NULL) *pn_font_asc = font_asc;
   if (pn_font_des != NULL) *pn_font_des = font_des;
}

/* --------------------- TidgetManagerResetGC --------------------- */

void TidgetManagerResetGC()
{
   if (gTidgetManager.gc != NULL) {
      XGCValues values;

      values.background = myBgPixel;
      values.foreground = myFgPixel;
      values.font = defaultFontPtr->fid;
      values.fill_style = FillSolid;
      values.function = GXcopy;
      values.ts_x_origin = values.ts_y_origin = 0;
      XChangeGC(mainDisplay, gTidgetManager.gc,
            GCBackground | GCForeground | GCFont | GCFillStyle | GCFunction |
            GCTileStipXOrigin | GCTileStipYOrigin, &values);
   }
}

/* --------------------- TidgetManagerHandleEvent --------------------- */

int TidgetManagerHandleEvent(pXEv)
   /* return TRUE if the event is handled */
   XEvent *pXEv;
{
   CVListElem *pElem=NULL;

   for (pElem=ListFirst(&gTidgetManager.toplevel_tidgets); pElem != NULL;
         pElem=ListNext(&gTidgetManager.toplevel_tidgets, pElem)) {
      TidgetInfo *pti=(TidgetInfo*)(pElem->obj);

      if (pti != NULL) {
         TidgetInfo *handling_pti=NULL;

         if (IsTidgetEvent(pti, pXEv, &handling_pti)) {
            TidgetEventHandler(pti, pXEv, handling_pti);
            return TRUE;
         }
      }
   }
   return FALSE;
}

/* ------------------ TidgetManagerHandleAllKeyPressEvent ------------------ */

int TidgetManagerHandleAllKeyPressEvent(pXEv)
   /* return TRUE if the event is handled */
   XEvent *pXEv;
{
   TidgetInfo *pti=(gTidgetManager.key_press_base_tidgetinfo);
   TdgtBase *pTdgtBase=NULL;

   if (pti == NULL)  return FALSE;

   if (pti->type != TIDGET_TYPE_BASE) return FALSE;

   pTdgtBase = pti->tidget;

   if (pTdgtBase == NULL) return FALSE;

   if (pTdgtBase->pf_key_press_ev_handler_callback != NULL) {
      return (pTdgtBase->pf_key_press_ev_handler_callback)(pTdgtBase, pXEv);
   }
   return FALSE;
}

/* ------------------- TidgetManagerWantAllKeyPressEvents ------------------- */

int TidgetManagerWantAllKeyPressEvents()
{
   return gTidgetManager.want_all_key_press_events;
}

/* ----------------- TidgetManagerSetWantAllKeyPressEvents ----------------- */

int TidgetManagerSetWantAllKeyPressEvents(pti, want_all_key_press_events)
   TidgetInfo *pti;
   int want_all_key_press_events;
{
   int prev_want_all_key_press_events=gTidgetManager.want_all_key_press_events;

   gTidgetManager.want_all_key_press_events = want_all_key_press_events;
   gTidgetManager.key_press_base_tidgetinfo = pti;

   return prev_want_all_key_press_events;
}

/* --------------------- Init & Clean Up --------------------- */

int InitTidget()
{
   int ok=TRUE;
   XGCValues values;

   memset(&gTidgetManager, 0, sizeof(TidgetManager));
   CVListInit(&gTidgetManager.toplevel_tidgets);

   if (mainDisplay != NULL) {
      values.background = myBgPixel;
      values.foreground = myFgPixel;
      values.fill_style = FillSolid;
      values.function = GXcopy;
      gTidgetManager.gc = XCreateGC(mainDisplay, mainWindow,
            GCBackground | GCForeground | GCFillStyle | GCFunction, &values);

      gTidgetManager.notify_atom = XInternAtom(mainDisplay, "TDGT_NOTIFY",
            False);
   }
   if (!InitTdgtBase()) ok = FALSE;
   if (!InitTdgtList()) ok = FALSE;
   if (!InitTdgtBtn()) ok = FALSE;
   if (!InitTdgtDraw()) ok = FALSE;
   if (!InitTdgtMsg()) ok = FALSE;
   if (!InitTdgtBtnRow()) ok = FALSE;
   if (!InitTdgtBmpList()) ok = FALSE;
   if (!InitTdgtSmplEdit()) ok = FALSE;

   return ok;
}

void CleanUpTidget()
{
   CVListElem *pElem=NULL;

   if (ListLength(&gTidgetManager.toplevel_tidgets) > 0) {
#ifdef _TGIF_DBG /* debug, do not translate */
      fprintf(stderr,
            "gTidgetManager.toplevel_tidgets is not empty.  %1d left.\n",
            ListLength(&gTidgetManager.toplevel_tidgets));
#endif /* _TGIF_DBG */
      for (pElem=ListFirst(&gTidgetManager.toplevel_tidgets); pElem != NULL;
            pElem=ListNext(&gTidgetManager.toplevel_tidgets, pElem)) {
      }
      ListUnlinkAll(&gTidgetManager.toplevel_tidgets);
   }
   CleanUpTdgtBase();
   CleanUpTdgtList();
   CleanUpTdgtBtn();
   CleanUpTdgtDraw();
   CleanUpTdgtMsg();
   CleanUpTdgtBtnRow();
   CleanUpTdgtBmpList();
   CleanUpTdgtSmplEdit();

   if (gTidgetManager.gc != NULL) {
      XFreeGC(mainDisplay, gTidgetManager.gc);
      gTidgetManager.gc = NULL;
   }
   memset(&gTidgetManager, 0, sizeof(TidgetManager));
}

