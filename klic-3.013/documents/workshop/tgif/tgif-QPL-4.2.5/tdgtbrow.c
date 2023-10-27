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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/tdgtbrow.c,v 1.7 2011/05/16 16:22:00 william Exp $
 */

#define _INCLUDE_FROM_TDGTBROW_C_

#include "tgifdefs.h"
#include "patchlvl.h"
#include "tidget.h"

#include "menu.e"
#include "msg.e"
#include "raster.e"
#include "rect.e"
#include "setup.e"
#include "tdgtbrow.e"
#include "tdgtbtn.e"
#include "tidget.e"
#include "util.e"

static void RedrawTdgtBtnRow ARGS_DECL((TidgetInfo *pti));
static int TdgtBtnRowEventHandler ARGS_DECL((TidgetInfo *pti, XEvent *input,
                TidgetInfo *handling_pti));
static int IsTdgtBtnRowEvent ARGS_DECL((TidgetInfo *pti, XEvent *input,
                TidgetInfo **ppti_handler_tidget_return));
static void DestroyTdgtBtnRow ARGS_DECL((TidgetInfo *pti));
static void MapTdgtBtnRow ARGS_DECL((TidgetInfo *pti));
static void TdgtBtnRowMoveResize ARGS_DECL((TidgetInfo *pti, int x, int y,
                int w, int h));
static int TdgtBtnRowSendCmd ARGS_DECL((TidgetInfo *pti, int cmd_type,
                int cmd_arg, void *pv_cmd_userdata));

/* --------------------- Utility Functions --------------------- */

static
void GetContentWH(pTdgtBtnRow, pn_content_w, pn_content_h)
   TdgtBtnRow *pTdgtBtnRow;
   int *pn_content_w, *pn_content_h;
{
   int w=0, h=0;

   if (ListLength(&pTdgtBtnRow->pti->tci.clist) > 0) {
      CVListElem *pElem=NULL;

      for (pElem=ListFirst(&pTdgtBtnRow->pti->tci.clist); pElem != NULL;
            pElem=ListNext(&pTdgtBtnRow->pti->tci.clist, pElem)) {
         TidgetInfo *pti=(TidgetInfo*)(pElem->obj);

         w += pti->tci.win_info.w + pTdgtBtnRow->h_gap;
         if (pti->tci.win_info.h > h) {
            h = pti->tci.win_info.h;
         }
      }
      w -= pTdgtBtnRow->h_gap;
   }
   if (pn_content_w != NULL) *pn_content_w = w;
   if (pn_content_h != NULL) *pn_content_h = h;
}

/* --------------------- RedrawTdgtBtnRow() --------------------- */

static
void RedrawTdgtBtnRow(pti)
   TidgetInfo *pti;
{
   TdgtBtnRow *pTdgtBtnRow=(TdgtBtnRow*)(pti->tidget);
   CVListElem *pElem=NULL;

   XClearWindow(mainDisplay, pTdgtBtnRow->pti->tci.win);

   if (threeDLook) {
      struct BBRec bbox;

      SetBBRec(&bbox, 0, 0, pTdgtBtnRow->pti->tci.win_info.w,
            pTdgtBtnRow->pti->tci.win_info.h);
      switch (pTdgtBtnRow->pti->tci.state) {
      case TGBS_NORMAL: break;
      default:
         TgDrawThreeDButton(mainDisplay, pTdgtBtnRow->pti->tci.win,
               gTidgetManager.gc, &bbox, pTdgtBtnRow->pti->tci.state, 2, TRUE);
         TidgetManagerResetGC();
         break;
      }
   } else {
      /*
       * XDrawRectangle(mainDisplay, pTdgtBtnRow->pti->tci.win,
       *       gTidgetManager.gc, 0, 0, pTdgtBtnRow->pti->tci.win_info.w,
       *       pTdgtBtnRow->pti->tci.win_info.h);
       */
   }
   for (pElem=ListFirst(&pTdgtBtnRow->pti->tci.clist); pElem != NULL;
         pElem=ListNext(&pTdgtBtnRow->pti->tci.clist, pElem)) {
      TidgetInfo *pti=(TidgetInfo*)(pElem->obj);

      if (pti != NULL && pti->tidget != NULL) {
         RedrawTidget(pti);
      }
   }
}

/* --------------------- TdgtBtnRowEventHandler() --------------------- */

static
int TdgtBtnRowEventHandler(pti, input, handling_pti)
   TidgetInfo *pti, *handling_pti;
   XEvent *input;
{
   TdgtBtnRow *pTdgtBtnRow=(TdgtBtnRow*)(pti->tidget);
   CVListElem *pElem=NULL;

   if (pti == handling_pti) {
      if (input->type == Expose && input->xany.window ==
            pTdgtBtnRow->pti->tci.win) {
         XEvent ev;

         RedrawTdgtBtnRow(pTdgtBtnRow->pti);
         while (XCheckWindowEvent(mainDisplay, pTdgtBtnRow->pti->tci.win,
               ExposureMask, &ev)) ;
      }
   }
   for (pElem=ListFirst(&pTdgtBtnRow->pti->tci.clist); pElem != NULL;
         pElem=ListNext(&pTdgtBtnRow->pti->tci.clist, pElem)) {
      TidgetInfo *child_pti=(TidgetInfo*)(pElem->obj);

      if (TidgetEventHandler(child_pti, input, handling_pti)) {
         return TRUE;
      }
   }
   return FALSE;
}

/* --------------------- IsTdgtBtnRowEvent() --------------------- */

static
int IsTdgtBtnRowEvent(pti, input, ppti_handler_tidget_return)
   TidgetInfo *pti, **ppti_handler_tidget_return;
   XEvent *input;
{
   TdgtBtnRow *pTdgtBtnRow=(TdgtBtnRow*)(pti->tidget);
   CVListElem *pElem=NULL;

   if (input->xany.window == pTdgtBtnRow->pti->tci.win) return TRUE;
 
   for (pElem=ListFirst(&pTdgtBtnRow->pti->tci.clist); pElem != NULL;
         pElem=ListNext(&pTdgtBtnRow->pti->tci.clist, pElem)) {
      TidgetInfo *child_pti=(TidgetInfo*)(pElem->obj);

      if (IsTidgetEvent(child_pti, input, ppti_handler_tidget_return)) {
         return TRUE;
      }
   }
   return FALSE;
}

/* --------------------- DestroyTdgtBtnRow() --------------------- */

static
void DestroyTdgtBtnRow(pti)
   TidgetInfo *pti;
{
   TdgtBtnRow *pTdgtBtnRow=(TdgtBtnRow*)(pti->tidget);

   TdgtBtnRowReset(pTdgtBtnRow);

   free(pTdgtBtnRow);
}

/* --------------------- MapTdgtBtnRow() --------------------- */

static
void MapTdgtBtnRow(pti)
   TidgetInfo *pti;
{
   TdgtBtnRow *pTdgtBtnRow=(TdgtBtnRow*)(pti->tidget);

   CVListElem *pElem=NULL;

#ifdef MAPBEFORESELECT
   XMapWindow(mainDisplay, pTdgtBtnRow->pti->tci.win);
   XSelectInput(mainDisplay, pTdgtBtnRow->pti->tci.win,
         ButtonReleaseMask | ButtonPressMask | KeyPressMask | ExposureMask |
         EnterWindowMask | LeaveWindowMask);
#else
   XSelectInput(mainDisplay, pTdgtBtnRow->pti->tci.win,
         ButtonReleaseMask | ButtonPressMask | KeyPressMask | ExposureMask |
         EnterWindowMask | LeaveWindowMask);
   XMapWindow(mainDisplay, pTdgtBtnRow->pti->tci.win);
#endif
   for (pElem=ListFirst(&pTdgtBtnRow->pti->tci.clist); pElem != NULL;
         pElem=ListNext(&pTdgtBtnRow->pti->tci.clist, pElem)) {
      TidgetInfo *pti=(TidgetInfo*)(pElem->obj);

      if (pti != NULL) {
         MapTidget(pti);
      }
   }
}

/* --------------------- TdgtBtnRowMoveResize() --------------------- */

static
void TdgtBtnRowMoveResize(pti, x, y, w, h)
   TidgetInfo *pti;
   int x, y, w, h;
{
   TdgtBtnRow *pTdgtBtnRow=(TdgtBtnRow*)(pti->tidget);

   /* there should be no need to resize a button */
   pTdgtBtnRow->pti->tci.win_info.x = x;
   pTdgtBtnRow->pti->tci.win_info.y = y;
   pTdgtBtnRow->pti->tci.win_info.w = w;
   pTdgtBtnRow->pti->tci.win_info.h = h;
   XMoveResizeWindow(mainDisplay, pTdgtBtnRow->pti->tci.win, x, y, w, h);

   if (ListLength(&pTdgtBtnRow->pti->tci.clist) > 0) {
      int x=0;
      CVListElem *pElem=NULL;

      switch (pTdgtBtnRow->just) {
      case JUST_L: x = windowPadding+pti->tci.h_pad; break;
      case JUST_C: x = ((w-pTdgtBtnRow->content_w)>>1); break;
      case JUST_R:
         x = w-pTdgtBtnRow->content_w-windowPadding-pti->tci.h_pad;
         break;
      }
      for (pElem=ListFirst(&pTdgtBtnRow->pti->tci.clist); pElem != NULL;
            pElem=ListNext(&pTdgtBtnRow->pti->tci.clist, pElem)) {
         TidgetInfo *pti=(TidgetInfo*)(pElem->obj);

         TidgetMoveResize(pti, x, pti->tci.win_info.y, pti->tci.win_info.w,
               pti->tci.win_info.h);
         x += pti->tci.win_info.w + pTdgtBtnRow->h_gap;
      }
   }
}

/* --------------------- TdgtBtnRowSendCmd() --------------------- */

static
int TdgtBtnRowSendCmd(pti, cmd_type, cmd_arg, pv_cmd_userdata)
   TidgetInfo *pti;
   int cmd_type, cmd_arg;
   void *pv_cmd_userdata;
{
   TdgtBtnRow *pTdgtBtnRow=(TdgtBtnRow*)(pti->tidget);

   if (pTdgtBtnRow != NULL) {
   }
   return FALSE;
}

/* --------------------- TdgtBtnRowReset() --------------------- */

void TdgtBtnRowReset(pTdgtBtnRow)
   TdgtBtnRow *pTdgtBtnRow;
{
   CVListElem *pElem=NULL;

   for (pElem=ListFirst(&pTdgtBtnRow->pti->tci.clist); pElem != NULL;
         pElem=ListNext(&pTdgtBtnRow->pti->tci.clist, pElem)) {
      TidgetInfo *pti=(TidgetInfo*)(pElem->obj);

      DestroyTidget(&pti);
   }
   ListUnlinkAll(&pTdgtBtnRow->pti->tci.clist);
   CVListInit(&pTdgtBtnRow->pti->tci.clist);
}

/* --------------------- CreateTdgtBtnRow() --------------------- */

static
void TdgtBtnRowCalcGeom(pTdgtBtnRow, pn_content_w, pn_content_h)
   TdgtBtnRow *pTdgtBtnRow;
   int *pn_content_w, *pn_content_h;
{
   int btn_w=0, btn_h=0;

   if (pTdgtBtnRow != NULL) {
   }
   CalcTdgtBtnDim("Ok", 8, TDGTBTN_DEF_H_PAD, TDGTBTN_DEF_V_PAD, &btn_w,
         &btn_h);

   if (pn_content_w != NULL) *pn_content_w = btn_w;
   if (pn_content_h != NULL) *pn_content_h = btn_h;
}

TdgtBtnRow *CreateTdgtBtnRow(parent_win, parent_tidgetinfo, ctl_id, x, y, h_pad,
      v_pad, state, h_gap, just)
   Window parent_win;
   TidgetInfo *parent_tidgetinfo;
   int ctl_id, x, y, h_pad, v_pad, state, h_gap, just;
{
   int bg_pixel=(threeDLook ? myLtGryPixel : myBgPixel), w=0, h=0;
   int content_w=0, content_h=0;
   TdgtBtnRow *pTdgtBtnRow=NULL;

   TdgtBtnRowCalcGeom(NULL, &content_w, &content_h);
   w = content_w + (windowPadding<<1) + (h_pad<<1);
   h = content_h + (windowPadding<<1) + (v_pad<<1);

   pTdgtBtnRow = (TdgtBtnRow*)malloc(sizeof(TdgtBtnRow));
   if (pTdgtBtnRow == NULL) FailAllocMessage();
   memset(pTdgtBtnRow, 0, sizeof(TdgtBtnRow));

   pTdgtBtnRow->pti = NewTidgetInfo(parent_tidgetinfo, TIDGET_TYPE_BROW,
         pTdgtBtnRow, ctl_id, NULL);
   if ((pTdgtBtnRow->pti->tci.win=XCreateSimpleWindow(mainDisplay, parent_win,
         x, y, w, h, brdrW, myBorderPixel, bg_pixel)) == 0) {
      FailToCreateWindowMessage("CreateTdgtBtnRow()", NULL, TRUE);
   }
   XSelectInput(mainDisplay, pTdgtBtnRow->pti->tci.win,
         ButtonReleaseMask | ButtonPressMask | KeyPressMask | ExposureMask |
         EnterWindowMask | LeaveWindowMask);
   SetTidgetInfoBasic(pTdgtBtnRow->pti, TIDGET_TYPE_BROW, pTdgtBtnRow,
         parent_win, x, y, w, h, h_pad, v_pad, state, NULL);
   TidgetCanHaveChildren(pTdgtBtnRow->pti, TRUE);
   TidgetSetCallbacks(pTdgtBtnRow->pti,
         RedrawTdgtBtnRow, TdgtBtnRowEventHandler, IsTdgtBtnRowEvent,
         DestroyTdgtBtnRow, MapTdgtBtnRow, TdgtBtnRowMoveResize,
         TdgtBtnRowSendCmd);

   pTdgtBtnRow->client_area.x = windowPadding + h_pad;
   pTdgtBtnRow->client_area.y = windowPadding + v_pad;
   pTdgtBtnRow->client_area.w = w-(windowPadding<<1)-(h_pad<<1);
   pTdgtBtnRow->client_area.h = h-(windowPadding<<1)-(v_pad<<1);

   pTdgtBtnRow->content_w = content_w;
   pTdgtBtnRow->content_h = content_h;
   pTdgtBtnRow->h_gap = h_gap;
   pTdgtBtnRow->just = just;

   return pTdgtBtnRow;
}

/* --------------------- TdgtBtnRowAddBtn() --------------------- */

int TdgtBtnRowAddBtn(pTdgtBtnRow, pTdgtBtn)
   TdgtBtnRow *pTdgtBtnRow;
   TdgtBtn *pTdgtBtn;
{
   int x=0, content_w=0, content_h=0;
   int this_btn_w=pTdgtBtn->pti->tci.win_info.w;
   int this_btn_h=pTdgtBtn->pti->tci.win_info.h;

   GetContentWH(pTdgtBtnRow, &content_w, &content_h);
   if (this_btn_h > content_h) content_h = this_btn_h;
   if (content_w != 0) {
      x = content_w+pTdgtBtnRow->h_gap;
   }
   content_w = x+this_btn_w;

   pTdgtBtnRow->content_w = content_w;
   pTdgtBtnRow->content_h = content_h;

   /*
    * Set the client_area to be as small as possible -- just big enough
    *         to include all the buttons.
    */
   pTdgtBtnRow->client_area.w = content_w;
   pTdgtBtnRow->client_area.h = content_h;

   pTdgtBtnRow->pti->tci.win_info.w =
         content_w + (windowPadding<<1) + ((pTdgtBtnRow->pti->tci.h_pad)<<1);
   pTdgtBtnRow->pti->tci.win_info.h =
         content_h + (windowPadding<<1) + ((pTdgtBtnRow->pti->tci.v_pad)<<1);

   TdgtBtnRowMoveResize(pTdgtBtnRow->pti, pTdgtBtnRow->pti->tci.win_info.x,
         pTdgtBtnRow->pti->tci.win_info.y, pTdgtBtnRow->pti->tci.win_info.w,
         pTdgtBtnRow->pti->tci.win_info.h);

   TdgtBtnRowMoveResize(pTdgtBtn->pti,
         x+windowPadding+pTdgtBtnRow->pti->tci.h_pad,
         windowPadding+pTdgtBtnRow->pti->tci.v_pad, this_btn_w, this_btn_h);

   return ListAppend(&pTdgtBtnRow->pti->tci.clist, pTdgtBtn->pti);
}

/* --------------------- Init & Clean Up --------------------- */

int InitTdgtBtnRow()
{
   return TRUE;
}

void CleanUpTdgtBtnRow()
{
}

