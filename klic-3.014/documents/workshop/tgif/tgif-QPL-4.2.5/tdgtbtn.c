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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/tdgtbtn.c,v 1.8 2011/05/16 16:22:00 william Exp $
 */

#define _INCLUDE_FROM_TDGTBTN_C_

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
#include "tdgtbtn.e"
#include "tidget.e"
#include "util.e"

static void RedrawTdgtBtn ARGS_DECL((TidgetInfo *pti));
static int TdgtBtnEventHandler ARGS_DECL((TidgetInfo *pti, XEvent *input,
                TidgetInfo *handling_pti));
static int IsTdgtBtnEvent ARGS_DECL((TidgetInfo *pti, XEvent *input,
                TidgetInfo **ppti_handler_tidget_return));
static void DestroyTdgtBtn ARGS_DECL((TidgetInfo *pti));
static void MapTdgtBtn ARGS_DECL((TidgetInfo *pti));
static void TdgtBtnMoveResize ARGS_DECL((TidgetInfo *pti, int x, int y,
                int w, int h));
static int TdgtBtnSendCmd ARGS_DECL((TidgetInfo *pti, int cmd_type,
                int cmd_arg, void *pv_cmd_userdata));

static int gnBtnFontHeight=0;
static int gnBtnFontWidth=0;
static int gnBtnFontAsc=0;
static int gnBtnFontDes=0;
static XFontStruct *gnBtnFontPtr=NULL;

/* --------------------- RedrawTdgtBtn() --------------------- */

void CalcTdgtBtnDim(text, min_len, h_pad, v_pad, pn_width, pn_height)
   char *text;
   int min_len, h_pad, v_pad, *pn_width, *pn_height;
{
   int btn_w=ButtonWidth(text, min_len, NULL)+(h_pad<<1), btn_h=0;

   if (boldMsgFontSet != NULL || boldMsgFontPtr != NULL) {
      btn_h = boldMsgFontHeight;
   } else if (msgFontSet != NULL || msgFontPtr != NULL) {
      btn_h = msgFontHeight;
   } else {
      btn_h = defaultFontHeight;
   }
   btn_h += (v_pad<<1)+(windowPadding<<1);

   if (pn_width != NULL) *pn_width = btn_w;
   if (pn_height != NULL) *pn_height = btn_h;
}

/* --------------------- RedrawTdgtBtn() --------------------- */

static
void DrawTdgtTextBtn(pTdgtBtn, pBBRec, str, len, down, pButtonFontInfo)
   TdgtBtn *pTdgtBtn;
   struct BBRec *pBBRec;
   char *str;
   int len, down;
   ButtonFontInfo *pButtonFontInfo;
{
   if (down) {
      DisplayButtonInBBox(pTdgtBtn->pti->tci.win, str, len, pBBRec,
            BUTTON_INVERT, FALSE, 0, pButtonFontInfo);
      if (threeDLook) {
         TgDrawThreeDButton(mainDisplay, pTdgtBtn->pti->tci.win,
               gTidgetManager.gc, pBBRec, TGBS_LOWRED, 2, TRUE);
      }
   } else {
      DisplayButtonInBBox(pTdgtBtn->pti->tci.win, str, len, pBBRec,
            BUTTON_NORMAL, FALSE, 0, pButtonFontInfo);
      if (threeDLook) {
         TgDrawThreeDButton(mainDisplay, pTdgtBtn->pti->tci.win,
               gTidgetManager.gc, pBBRec, TGBS_RAISED, 2, TRUE);
      }
   }
   TidgetManagerResetGC();
}

static
void RedrawTdgtBtn(pti)
   TidgetInfo *pti;
{
   TdgtBtn *pTdgtBtn=(TdgtBtn*)(pti->tidget);
   struct BBRec bbox;
   char *str=NULL;
   int len=0, pixel=INVALID, btn_down=(pTdgtBtn->pti->tci.state != TGBS_NORMAL);
   ButtonFontInfo btn_font_info;
   XGCValues values;
   Pixmap bitmap=None;

   SetBBRec(&bbox, 0, 0, pTdgtBtn->pti->tci.win_info.w,
         pTdgtBtn->pti->tci.win_info.h);

   switch (pTdgtBtn->btn_type) {
   case TGMUTYPE_TEXT:
      str = ((pTdgtBtn->str == NULL) ? "" : pTdgtBtn->str);
      len = strlen(str);
      memset(&btn_font_info, 0, sizeof(ButtonFontInfo));
      btn_font_info.font_style = pTdgtBtn->font_style;
      DrawTdgtTextBtn(pTdgtBtn, &bbox, str, len, FALSE, &btn_font_info);
      break;
   case TGMUTYPE_COLOR:
      if (colorDisplay) {
         pixel = (btn_down ? myFgPixel : colorPixels[(int)(long)(pTdgtBtn->str)]);
      } else {
         pixel = myFgPixel;
      }
      values.foreground = pixel;
      values.function = GXcopy;
      values.fill_style = FillOpaqueStippled;
      values.stipple = patPixmap[1];
      XChangeGC(mainDisplay, gTidgetManager.gc,
            GCForeground | GCFunction | GCFillStyle | GCStipple, &values);

      XFillRectangle(mainDisplay, pTdgtBtn->pti->tci.win, gTidgetManager.gc,
            windowPadding, windowPadding, pTdgtBtn->pti->tci.content_w,
            pTdgtBtn->pti->tci.content_h);
      if (threeDLook) {
         TgDrawThreeDButton(mainDisplay, pTdgtBtn->pti->tci.win,
               gTidgetManager.gc, &bbox, (btn_down ? TGBS_LOWRED : TGBS_RAISED),
               2, TRUE);
      }
      TidgetManagerResetGC();
      break;
   case TGMUTYPE_BITMAP:
      bitmap = (*(Pixmap*)pTdgtBtn->str);
      if (threeDLook) {
         values.foreground = myFgPixel;
         values.background = (threeDLook ? myLtGryPixel : myBgPixel);
         values.function = GXcopy;
         values.fill_style = FillStippled;
         values.stipple = bitmap;
         values.ts_x_origin = windowPadding;
         values.ts_y_origin = windowPadding;
         XChangeGC(mainDisplay, gTidgetManager.gc,
               GCForeground | GCBackground | GCFunction | GCFillStyle |
               GCStipple | GCTileStipXOrigin | GCTileStipYOrigin, &values);
         XFillRectangle(mainDisplay, pTdgtBtn->pti->tci.win, gTidgetManager.gc,
               values.ts_x_origin, values.ts_y_origin,
               pTdgtBtn->pti->tci.content_w, pTdgtBtn->pti->tci.content_h);
         XSetTSOrigin(mainDisplay, gTidgetManager.gc, 0, 0);
         switch (pTdgtBtn->btn_style) {
         case TDGTBTN_CLICK:
            break;
         case TDGTBTN_STICKY:
            TidgetManagerResetGC();
            TgDrawThreeDButton(mainDisplay, pTdgtBtn->pti->tci.win,
                  gTidgetManager.gc, &bbox,
                  (btn_down ? TGBS_LOWRED : TGBS_RAISED), 2, TRUE);
            break;
         }
         TidgetManagerResetGC();
      } else {
         if (btn_down) {
            values.foreground = myFgPixel;
            values.fill_style = FillSolid;
            XChangeGC(mainDisplay, gTidgetManager.gc,
                  GCForeground | GCFillStyle, &values);

            XFillRectangle(mainDisplay, pTdgtBtn->pti->tci.win,
                  gTidgetManager.gc, windowPadding, windowPadding,
                  pTdgtBtn->pti->tci.content_w, pTdgtBtn->pti->tci.content_h);

            values.foreground = myFgPixel;
            values.fill_style = FillStippled;
            values.stipple = bitmap;
            XChangeGC(mainDisplay, gTidgetManager.gc,
                  GCForeground | GCFillStyle | GCStipple, &values);
            XFillRectangle(mainDisplay, pTdgtBtn->pti->tci.win,
                  gTidgetManager.gc, windowPadding, windowPadding,
                  pTdgtBtn->pti->tci.content_w, pTdgtBtn->pti->tci.content_h);
         } else {
            values.foreground = myFgPixel;
            values.fill_style = FillStippled;
            values.stipple = bitmap;
            XChangeGC(mainDisplay, gTidgetManager.gc,
                  GCForeground | GCFillStyle | GCStipple, &values);

            XFillRectangle(mainDisplay, pTdgtBtn->pti->tci.win,
                  gTidgetManager.gc, windowPadding, windowPadding,
                  pTdgtBtn->pti->tci.content_w, pTdgtBtn->pti->tci.content_h);
         }
         TidgetManagerResetGC();
      }
      break;
   }
}

/* --------------------- TdgtBtnEventHandler() --------------------- */

static
int ButtonPressInTdgtBtn(pTdgtBtn, button_ev, pn_changing,
      pn_selected_btn_index)
   TdgtBtn *pTdgtBtn;
   XButtonEvent *button_ev;
   int *pn_changing, *pn_selected_btn_index;
{
   struct BBRec bbox;
   char *str=NULL;
   int len=0, looping=TRUE, selected=TRUE;
   ButtonFontInfo btn_font_info;
   XEvent ev;

   memset(&btn_font_info, 0, sizeof(ButtonFontInfo));
   btn_font_info.font_style = pTdgtBtn->font_style;

   SetBBRec(&bbox, 0, 0, pTdgtBtn->pti->tci.win_info.w,
         pTdgtBtn->pti->tci.win_info.h);

   if (!debugNoPointerGrab) {
      XGrabPointer(mainDisplay, pTdgtBtn->pti->tci.win, False,
            PointerMotionMask | ButtonReleaseMask, GrabModeAsync,
            GrabModeAsync, None, handCursor, CurrentTime);
   }
   switch (pTdgtBtn->btn_type) {
   case TGMUTYPE_TEXT:
      str = ((pTdgtBtn->str == NULL) ? "" : pTdgtBtn->str);
      len = strlen(str);
      DrawTdgtTextBtn(pTdgtBtn, &bbox, str, len, TRUE, &btn_font_info);
      break;
   case TGMUTYPE_COLOR:
      pTdgtBtn->pti->tci.state = TGBS_LOWRED;
      RedrawTdgtBtn(pTdgtBtn->pti);
      pTdgtBtn->pti->tci.state = TGBS_NORMAL;
      break;
   case TGMUTYPE_BITMAP:
      break;
   }
   while (looping) {
      XNextEvent(mainDisplay, &ev);

      if (ev.type == ButtonRelease) {
         XUngrabPointer(mainDisplay, CurrentTime);
         if (debugNoPointerGrab) XSync(mainDisplay, False);
         looping = FALSE;
      } else if (ev.type == MotionNotify) {
         if (PointInBBox(ev.xmotion.x, ev.xmotion.y, bbox)) {
            if (!selected) {
               selected = TRUE;
               switch (pTdgtBtn->btn_type) {
               case TGMUTYPE_TEXT:
                  DrawTdgtTextBtn(pTdgtBtn, &bbox, str, len, TRUE,
                        &btn_font_info);
                  break;
               case TGMUTYPE_COLOR:
                  pTdgtBtn->pti->tci.state = TGBS_LOWRED;
                  RedrawTdgtBtn(pTdgtBtn->pti);
                  pTdgtBtn->pti->tci.state = TGBS_NORMAL;
                  break;
               case TGMUTYPE_BITMAP: break;
               }
            }
         } else {
            if (selected) {
               selected = FALSE;
               switch (pTdgtBtn->btn_type) {
               case TGMUTYPE_TEXT:
                  DrawTdgtTextBtn(pTdgtBtn, &bbox, str, len, FALSE,
                        &btn_font_info);
                  break;
               case TGMUTYPE_COLOR: RedrawTdgtBtn(pTdgtBtn->pti); break;
               case TGMUTYPE_BITMAP: break;
               }
            }
         }
      }
   }
   if (selected) {
      switch (pTdgtBtn->btn_type) {
      case TGMUTYPE_TEXT:
         DrawTdgtTextBtn(pTdgtBtn, &bbox, str, len, FALSE, &btn_font_info);
         break;
      case TGMUTYPE_COLOR: RedrawTdgtBtn(pTdgtBtn->pti); break;
      case TGMUTYPE_BITMAP: break;
      }
   }
   return selected;
}

static
int TdgtBtnEventHandler(pti, input, handling_pti)
   TidgetInfo *pti, *handling_pti;
   XEvent *input;
{
   TdgtBtn *pTdgtBtn=(TdgtBtn*)(pti->tidget);
   int nf_arg=0, nf_arg2=INVALID;

   if (pti != handling_pti) return FALSE;

   if (input->type == Expose) {
      XEvent ev;

      RedrawTdgtBtn(pTdgtBtn->pti);
      while (XCheckWindowEvent(mainDisplay, pTdgtBtn->pti->tci.win,
            ExposureMask, &ev)) ;
   } else if (input->type == ButtonPress) {
      switch (pTdgtBtn->btn_style) {
      case TDGTBTN_CLICK:
         switch (pTdgtBtn->btn_type) {
         case TGMUTYPE_TEXT:
            if (ButtonPressInTdgtBtn(pTdgtBtn, &input->xbutton, NULL, NULL)) {
               /* clicked */
               TidgetControlNotify(pTdgtBtn->pti, TDGTNF_BTN_CLICKED, 0, 0);
               return TRUE;
            }
            break;
         case TGMUTYPE_COLOR:
            switch (input->xbutton.button) {
            case Button1: nf_arg = (int)Button1; break;
            case Button2:
               {
                  int x=0, y=0, root_x=0, root_y=0, color_index=0;
                  Window root_win, child_win;
                  unsigned int status=0;

                  XQueryPointer(mainDisplay, rootWindow, &root_win, &child_win,
                        &root_x, &root_y, &x, &y, &status);
                  gstMenuDontSendCommandInfo.dont_send_command = TRUE;
                  gstMenuDontSendCommandInfo.selected_index = INVALID;
                  ColorMenu(x, y, FALSE);
                  color_index = gstMenuDontSendCommandInfo.selected_index;
                  if (color_index >= 0 && color_index < maxColors) {
                     nf_arg = Button2;
                     nf_arg2 = color_index;
                  }
                  memset(&gstMenuDontSendCommandInfo, 0,
                        sizeof(MenuDontSendCommandInfo));
               }
               break;
            case Button3: nf_arg = Button3; break;
            default: break;
            }
            if (nf_arg != 0) {
               TidgetControlNotify(pTdgtBtn->pti, TDGTNF_MULTI_BTN_CLICKED,
                     nf_arg, nf_arg2);
            }
            return TRUE;
         case TGMUTYPE_BITMAP: break;
         }
         break;
      case TDGTBTN_STICKY:
         /* press it */
         if (pTdgtBtn->pti->tci.state == TGBS_NORMAL) {
            pTdgtBtn->pti->tci.state = TGBS_LOWRED;
         } else {
            pTdgtBtn->pti->tci.state = TGBS_NORMAL;
         }
         TidgetControlNotify(pTdgtBtn->pti, TDGTNF_BTN_CLICKED, 0, 0);
         RedrawTdgtBtn(pTdgtBtn->pti);
         return TRUE;
      }
   } else if (input->type == EnterNotify) {
      if (pTdgtBtn->mosi.one_line_status) {
         SetStringStatus(pTdgtBtn->mosi.one_line_str);
      } else {
         MouseOverStatusInfo *pmosi=(&pTdgtBtn->mosi);

         SetMouseStatus(pmosi->btn_str[0], pmosi->btn_str[1],
               pmosi->btn_str[2]);
      }
   } else if (input->type == LeaveNotify) {
      SetMouseStatus("", "", "");
   }
   return FALSE;
}

/* --------------------- IsTdgtBtnEvent() --------------------- */

static
int IsTdgtBtnEvent(pti, input, ppti_handler_tidget_return)
   TidgetInfo *pti, **ppti_handler_tidget_return;
   XEvent *input;
{
   TdgtBtn *pTdgtBtn=(TdgtBtn*)(pti->tidget);

   if (input->xany.window == pTdgtBtn->pti->tci.win) return TRUE;

   return FALSE;
}

/* --------------------- DestroyTdgtBtn() --------------------- */

static
void DestroyTdgtBtn(pti)
   TidgetInfo *pti;
{
   TdgtBtn *pTdgtBtn=(TdgtBtn*)(pti->tidget);

   TdgtBtnReset(pTdgtBtn);

   free(pTdgtBtn);
}

/* --------------------- MapTdgtBtn() --------------------- */

static
void MapTdgtBtn(pti)
   TidgetInfo *pti;
{
   TdgtBtn *pTdgtBtn=(TdgtBtn*)(pti->tidget);

#ifdef MAPBEFORESELECT
   XMapWindow(mainDisplay, pTdgtBtn->pti->tci.win);
   XSelectInput(mainDisplay, pTdgtBtn->pti->tci.win,
         ButtonReleaseMask | ButtonPressMask | KeyPressMask | ExposureMask |
         EnterWindowMask | LeaveWindowMask);
#else
   XSelectInput(mainDisplay, pTdgtBtn->pti->tci.win,
         ButtonReleaseMask | ButtonPressMask | KeyPressMask | ExposureMask |
         EnterWindowMask | LeaveWindowMask);
   XMapWindow(mainDisplay, pTdgtBtn->pti->tci.win);
#endif
}

/* --------------------- TdgtBtnMoveResize() --------------------- */

static
void TdgtBtnMoveResize(pti, x, y, w, h)
   TidgetInfo *pti;
   int x, y, w, h;
{
   TdgtBtn *pTdgtBtn=(TdgtBtn*)(pti->tidget);

   /* there should be no need to resize a button */
   pTdgtBtn->pti->tci.win_info.x = x;
   pTdgtBtn->pti->tci.win_info.y = y;
   pTdgtBtn->pti->tci.win_info.w = w;
   pTdgtBtn->pti->tci.win_info.h = h;
   XMoveResizeWindow(mainDisplay, pTdgtBtn->pti->tci.win, x, y, w, h);
}

/* --------------------- TdgtBtnSendCmd() --------------------- */

static
int TdgtBtnSendCmd(pti, cmd_type, cmd_arg, pv_cmd_userdata)
   TidgetInfo *pti;
   int cmd_type, cmd_arg;
   void *pv_cmd_userdata;
{
   TdgtBtn *pTdgtBtn=(TdgtBtn*)(pti->tidget);

   if (pTdgtBtn != NULL) {
   }
   return FALSE;
}

/* --------------------- TdgtBtnReset() --------------------- */

void TdgtBtnReset(pTdgtBtn)
   TdgtBtn *pTdgtBtn;
{
   switch (pTdgtBtn->btn_type) {
   case TGMUTYPE_TEXT: UtilFree(pTdgtBtn->str); break;
   case TGMUTYPE_COLOR: break;
   case TGMUTYPE_BITMAP: break;
   }
   pTdgtBtn->str = NULL;
}

/* --------------------- CreateTdgtBtn() --------------------- */

TdgtBtn *CreateTdgtBtn(parent_win, parent_tidgetinfo, ctl_id, x, y, w, h, h_pad,
      v_pad, btn_type, btn_style, state, font_style, str, pmosi)
   Window parent_win;
   TidgetInfo *parent_tidgetinfo;
   int ctl_id, x, y, w, h, h_pad, v_pad, state, font_style;
   char *str;
   MouseOverStatusInfo *pmosi;
{
   int bg_pixel=(threeDLook ? myLtGryPixel : myBgPixel);
   TdgtBtn *pTdgtBtn=NULL;

   pTdgtBtn = (TdgtBtn*)malloc(sizeof(TdgtBtn));
   if (pTdgtBtn == NULL) FailAllocMessage();
   memset(pTdgtBtn, 0, sizeof(TdgtBtn));

   pTdgtBtn->pti = NewTidgetInfo(parent_tidgetinfo, TIDGET_TYPE_BTN, pTdgtBtn,
         ctl_id, NULL);
   if ((pTdgtBtn->pti->tci.win=XCreateSimpleWindow(mainDisplay, parent_win,
         x, y, w, h, brdrW, myBorderPixel, bg_pixel)) == 0) {
      FailToCreateWindowMessage("CreateTdgtBtn()", NULL, TRUE);
   }
   XSelectInput(mainDisplay, pTdgtBtn->pti->tci.win,
         ButtonReleaseMask | ButtonPressMask | KeyPressMask | ExposureMask |
         EnterWindowMask | LeaveWindowMask);
   SetTidgetInfoBasic(pTdgtBtn->pti, TIDGET_TYPE_BTN, pTdgtBtn, parent_win,
         x, y, w, h, h_pad, v_pad, state, NULL);
   TidgetSetCallbacks(pTdgtBtn->pti,
         RedrawTdgtBtn, TdgtBtnEventHandler, IsTdgtBtnEvent, DestroyTdgtBtn,
         MapTdgtBtn, TdgtBtnMoveResize, TdgtBtnSendCmd);

   switch (btn_type) {
   case TGMUTYPE_TEXT:
      if (str != NULL) {
         pTdgtBtn->str = UtilStrDup(str);
         if (pTdgtBtn->str == NULL) FailAllocMessage();
      }
      break;
   case TGMUTYPE_COLOR: pTdgtBtn->str = str; break;
   case TGMUTYPE_BITMAP: pTdgtBtn->str = str; break;
   }
   if (pmosi != NULL) {
      memcpy(&pTdgtBtn->mosi, pmosi, sizeof(MouseOverStatusInfo));
   }
   pTdgtBtn->btn_type = btn_type;
   pTdgtBtn->btn_style = btn_style;
   pTdgtBtn->font_style = font_style;

   return pTdgtBtn;
}

/* --------------------- TdgtBtnSetText() --------------------- */

int TdgtBtnSetText(pTdgtBtn, str, pmosi)
   TdgtBtn *pTdgtBtn;
   char *str;
   MouseOverStatusInfo *pmosi;
{
   switch (pTdgtBtn->btn_type) {
   case TGMUTYPE_TEXT:
      UtilFree(pTdgtBtn->str);
      if (str != NULL) {
         pTdgtBtn->str = UtilStrDup(str);
         if (pTdgtBtn->str == NULL) FailAllocMessage();
      }
      break;
   case TGMUTYPE_COLOR: pTdgtBtn->str = str; break;
   case TGMUTYPE_BITMAP: pTdgtBtn->str = str; break;
   }
   if (pmosi != NULL) {
      memcpy(&pTdgtBtn->mosi, pmosi, sizeof(MouseOverStatusInfo));
   }
   RedrawTdgtBtn(pTdgtBtn->pti);

   return TRUE;
}

/* --------------------- TdgtBtnGetText() --------------------- */

char *TdgtBtnGetText(pTdgtBtn)
   TdgtBtn *pTdgtBtn;
{
   return pTdgtBtn->str;
}

/* --------------------- TdgtBtnSetState() --------------------- */

int TdgtBtnSetState(pTdgtBtn, new_state)
   TdgtBtn *pTdgtBtn;
   int new_state;
{
   int need_to_redraw=(pTdgtBtn->pti->tci.state != new_state);

   pTdgtBtn->pti->tci.state = new_state;
   if (need_to_redraw) {
      RedrawTdgtBtn(pTdgtBtn->pti);
   }
   return TRUE;
}

/* --------------------- TdgtBtnGetState() --------------------- */

int TdgtBtnGetState(pTdgtBtn)
   TdgtBtn *pTdgtBtn;
{
   return pTdgtBtn->pti->tci.state;
}

/* --------------------- Init & Clean Up --------------------- */

int InitTdgtBtn()
{
   if (msgFontSet == NULL && msgFontPtr == NULL) {
      gnBtnFontHeight = defaultFontHeight;
      gnBtnFontWidth = defaultFontWidth;
      gnBtnFontAsc = defaultFontAsc;
      gnBtnFontDes = defaultFontDes;
      gnBtnFontPtr = defaultFontPtr;
   } else {
      gnBtnFontHeight = msgFontHeight;
      gnBtnFontWidth = msgFontWidth;
      gnBtnFontAsc = msgFontAsc;
      gnBtnFontDes = msgFontDes;
      gnBtnFontPtr = msgFontPtr;
   }
   return TRUE;
}

void CleanUpTdgtBtn()
{
   gnBtnFontHeight = gnBtnFontWidth = gnBtnFontAsc = gnBtnFontDes = 0;
   gnBtnFontPtr = NULL;
}

