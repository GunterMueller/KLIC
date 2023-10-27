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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/chat.c,v 1.8 2011/05/16 16:21:56 william Exp $
 */

#define _INCLUDE_FROM_CHAT_C_

#include "tgifdefs.h"
#include "patchlvl.h"
#include "tidget.h"

#include "button.e"
#include "chat.e"
#include "cmd.e"
#include "color.e"
#include "choose.e"
#include "dialog.e"
#include "file.e"
#include "font.e"
#include "http.e"
#include "ini.e"
#include "inmethod.e"
#include "msg.e"
#include "menu.e"
#include "rect.e"
#include "setup.e"
#include "strtbl.e"
#include "tdgtbtn.e"
#include "tdgtsedt.e"
#include "tdgtlist.e"
#include "tidget.e"
#include "util.e"
#include "xpixmap.e"

#include "xbm/a1.xbm"
#include "xbm/bold1.xbm"
#include "xbm/italic1.xbm"

#define CHAT_BTN_AUTO   0
#define CHAT_BTN_BOLD   1
#define CHAT_BTN_ITALIC 2
#define CHAT_BTN_SEND   3
#define CHAT_BTN_COLOR  4

#define MAX_CHAT_BTNS 5

#define ID_CHAT_BTN_AUTO   (100+CHAT_BTN_AUTO)
#define ID_CHAT_BTN_BOLD   (100+CHAT_BTN_BOLD)
#define ID_CHAT_BTN_ITALIC (100+CHAT_BTN_ITALIC)
#define ID_CHAT_BTN_SEND   (100+CHAT_BTN_SEND)
#define ID_CHAT_BTN_COLOR  (100+CHAT_BTN_COLOR)
#define ID_CHAT_LIST       105
#define ID_CHAT_EDIT       106

typedef struct tagChatInfo {
   SimpleWinInfo base_win_info, list_win_info, edit_win_info;
   SimpleWinInfo btn_win_info[MAX_CHAT_BTNS];

   int just_clicked;

   char ini_fname[MAXPATHLENGTH];

   int font_height; /* font height used for all controls */
   int font_width;
   int gap; /* gaps between controls */
   int btn_width; /* width of square buttons */
   int num_lines; /* number of visible lines in the list control */

   TdgtList *list_ctl;
   TdgtBtn *btn_ctl[MAX_CHAT_BTNS];
   TdgtSmplEdit *edit_ctl;
} ChatInfo;

ChatInfo gstChatInfo;

char *cmdLineChatNickName=NULL;

static Pixmap a1Pixmap=None;
static Pixmap bold1Pixmap=None;
static Pixmap italic1Pixmap=None;

static char *gpszChatNickName=NULL;

void RedrawChatWindow()
{
   if (PRTGIF || noChatWindow || chatWindow == None) return;

   /* draw a frame around the window */
   if (threeDLook) {
      if (dialogboxUse3DBorder) {
         struct BBRec bbox;

         SetBBRec(&bbox, 0, 0, gstChatInfo.base_win_info.w,
               gstChatInfo.base_win_info.h);
         TgDrawThreeDButton(mainDisplay, chatWindow, textMenuGC, &bbox,
               TGBS_RAISED, 2, FALSE);
      }
   } else {
      XDrawRectangle(mainDisplay, chatWindow, nameGC, 0, 0,
            gstChatInfo.base_win_info.w-1, gstChatInfo.base_win_info.h-1);
   }
}

static
void MoveResizeChatSubWindows()
{
   if (gstChatInfo.list_ctl != NULL) {
      TidgetMoveResize(gstChatInfo.list_ctl->pti, gstChatInfo.list_win_info.x,
            gstChatInfo.list_win_info.y, gstChatInfo.list_win_info.w,
            gstChatInfo.list_win_info.h);
   }
   if (gstChatInfo.btn_ctl[CHAT_BTN_SEND] != NULL) {
      TidgetMoveResize((gstChatInfo.btn_ctl[CHAT_BTN_SEND])->pti,
            gstChatInfo.btn_win_info[CHAT_BTN_SEND].x,
            gstChatInfo.btn_win_info[CHAT_BTN_SEND].y,
            gstChatInfo.btn_win_info[CHAT_BTN_SEND].w,
            gstChatInfo.btn_win_info[CHAT_BTN_SEND].h);
   }
   if (gstChatInfo.edit_ctl != NULL) {
      TidgetMoveResize(gstChatInfo.edit_ctl->pti, gstChatInfo.edit_win_info.x,
            gstChatInfo.edit_win_info.y, gstChatInfo.edit_win_info.w,
            gstChatInfo.edit_win_info.h);
   }
}

void MoveResizeChatWindow(x, y, w, h)
   int x, y, w, h;
{
   XMoveResizeWindow(mainDisplay, chatWindow, x, y, w, h);
   MoveResizeChatSubWindows();
}

static
int GetChatFontStyle(pnIsBold, pnIsItalic)
   int *pnIsBold, *pnIsItalic;
{
   int bold_state=TdgtBtnGetState(gstChatInfo.btn_ctl[CHAT_BTN_BOLD]);
   int italic_state=TdgtBtnGetState(gstChatInfo.btn_ctl[CHAT_BTN_ITALIC]);
   int is_bold=(bold_state != TGBS_NORMAL);
   int is_italic=(italic_state != TGBS_NORMAL);
   int font_style=INVALID;

   if (is_bold) {
      if (is_italic) {
         font_style = STYLE_BI;
      } else {
         font_style = STYLE_BR;
      }
   } else {
      if (is_italic) {
         font_style = STYLE_NI;
      } else {
         font_style = STYLE_NR;
      }
   }
   if (pnIsBold != NULL) *pnIsBold = is_bold;
   if (pnIsItalic != NULL) *pnIsItalic = is_italic;

   return font_style;
}

static
int InitChatNickName()
{
   char *c_ptr=NULL;

   if (cmdLineChatNickName != NULL) {
      gpszChatNickName = UtilStrDup(cmdLineChatNickName);
   } else {
      if ((c_ptr=XGetDefault(mainDisplay, TOOL_NAME, "ChatNickName")) != NULL) {
         gpszChatNickName = UtilStrDup(c_ptr);
      }
   }
   if (gpszChatNickName == NULL) {
      c_ptr = getenv("USER");
      if (c_ptr != NULL) {
         gpszChatNickName = UtilStrDup(c_ptr);
      } else {
         gpszChatNickName = UtilStrDup(
               TgLoadCachedString(CSTID_PARANED_UNKNOWN));
      }
   }
   if (gpszChatNickName == NULL) FailAllocMessage();

   return TRUE;
}

static
void GetChatNickName(buf, buf_sz)
   char *buf;
   int buf_sz;
{
   UtilStrCpyN(buf, buf_sz, gpszChatNickName);
}

static
int AddChatLineToListCtl(nick_name, color_index, font_style, buf)
   char *nick_name, *buf;
   int color_index, font_style;
{
   ListItemInfo *pListItemInfo=(ListItemInfo*)malloc(sizeof(ListItemInfo));

   if (pListItemInfo == NULL) FailAllocMessage();
   memset(pListItemInfo, 0, sizeof(ListItemInfo));

   UtilStrCpyN(pListItemInfo->nick_name, sizeof(pListItemInfo->nick_name),
         nick_name);
   pListItemInfo->color_index = color_index;
   pListItemInfo->font_style = font_style;
   pListItemInfo->buf = UtilStrDup(buf);
   if (pListItemInfo->buf == NULL) FailAllocMessage();

   return TdgtListInsertListItemInfo(gstChatInfo.list_ctl, pListItemInfo);
}

static
void ChatSendClicked()
{
   char *buf=TdgtSmplEditGetText(gstChatInfo.edit_ctl);

   if (buf != NULL && *buf != '\0') {
      int saved_auto_scroll=TdgtListGetAutoScrollOnInsert(gstChatInfo.list_ctl);
      int rc=0;
      struct SubCmdRec subcmd;
      char *encoded_text=Base64Encode(buf);

      memset(&subcmd, 0, sizeof(struct SubCmdRec));

      TdgtSmplEditGetTextFormatInfo(gstChatInfo.edit_ctl,
            &subcmd.detail.chat.tfi);

      subcmd.detail.chat.type = CHAT_STATE_NORMAL;
      GetChatNickName(subcmd.detail.chat.nick_name,
            sizeof(subcmd.detail.chat.nick_name));
      UtilStrCpyN(subcmd.detail.chat.encoding,
            sizeof(subcmd.detail.chat.encoding), "base64");
      TdgtListSetAutoScrollOnInsert(gstChatInfo.list_ctl, TRUE);
      rc = AddChatLineToListCtl(subcmd.detail.chat.nick_name,
            subcmd.detail.chat.tfi.color_index,
            subcmd.detail.chat.tfi.font_style, buf);
      TdgtListSetAutoScrollOnInsert(gstChatInfo.list_ctl, saved_auto_scroll);
      if (rc) {
         subcmd.detail.chat.buf = UtilStrDup(encoded_text);
         if (subcmd.detail.chat.buf == NULL) FailAllocMessage();
         RecordWBChatALine(&subcmd);
         UtilFree(subcmd.detail.chat.buf);
      }
      TdgtSmplEditSetText(gstChatInfo.edit_ctl, "");
      UtilFree(encoded_text);
   }
}

void ChatEventHandler(input, handling_pti)
   XEvent *input;
   TidgetInfo *handling_pti;
{
   int keypress_event=(input->type==KeyPress), event_handled=FALSE;

   if (PRTGIF || noChatWindow || chatWindow == None) return;

   /*
    * If the event is a KeyPress event, either a special key is pressed
    *         or a regular key is pressed.  Special keys should be handled
    *         by the list control and regular keys should be handled by
    *         the edit control.
    */
   if (input->xany.window == chatWindow) {
      TdgtNtfy tdgt_notify;

      memset(&tdgt_notify, 0, sizeof(TdgtNtfy));
      if (input->type == Expose) {
         XEvent ev;

         RedrawChatWindow();
         while (XCheckWindowEvent(mainDisplay, chatWindow, ExposureMask, &ev)) ;
      } else if (input->type == EnterNotify) {
         SetMouseStatus("", "", "");
         if (input->xcrossing.mode == NotifyNormal &&
               (input->xcrossing.detail == NotifyAncestor ||
               input->xcrossing.detail == NotifyNonlinear ||
               input->xcrossing.detail == NotifyNonlinearVirtual)) {
            TdgtSmplEditSetFocus(gstChatInfo.edit_ctl, TRUE);
            if (gnInputMethod != TGIM_NONE) {
               tgIMFocusIn(mainDisplay, gstChatInfo.edit_ctl->pti->tci.win);
            }
         }
      } else if (input->type == LeaveNotify) {
         if (input->xcrossing.mode == NotifyNormal &&
               (input->xcrossing.detail == NotifyAncestor ||
               input->xcrossing.detail == NotifyNonlinear ||
               input->xcrossing.detail == NotifyNonlinearVirtual)) {
            TdgtSmplEditSetFocus(gstChatInfo.edit_ctl, FALSE);
            if (gnInputMethod != TGIM_NONE) {
               tgIMFocusOut(mainDisplay, gstChatInfo.edit_ctl->pti->tci.win);
            }
         }
      } else if (IsTdgtWindowNotifyEvent(chatWindow, input, &tdgt_notify)) {
         switch (tdgt_notify.nf_type) {
         case TDGTNF_EDIT_ENTERED:
            ChatSendClicked();
            break;
         case TDGTNF_BTN_CLICKED:
            switch (tdgt_notify.ctl_id) {
            case ID_CHAT_BTN_SEND: ChatSendClicked(); break;

            case ID_CHAT_BTN_BOLD:
            case ID_CHAT_BTN_ITALIC:
               {
                  int is_bold=FALSE, is_italic=FALSE;

                  /* bold or italic button changed state */
                  int font_style=GetChatFontStyle(&is_bold, &is_italic);

                  TdgtSmplEditSetFontStyle(gstChatInfo.edit_ctl, font_style);
                  tgWriteProfileString("User", "Bold",
                        (is_bold ? "1" : "0"), gstChatInfo.ini_fname);
                  tgWriteProfileString("User", "Italic",
                        (is_italic ? "1" : "0"), gstChatInfo.ini_fname);
                  tgWriteProfileString(NULL, NULL, NULL, gstChatInfo.ini_fname);
               }
               break;
            case ID_CHAT_BTN_AUTO:
               {
                  /* auto scroll changed state */
                  int auto_state=TdgtBtnGetState(
                        gstChatInfo.btn_ctl[CHAT_BTN_AUTO]);
                  int auto_scroll=(auto_state != TGBS_NORMAL);

                  TdgtListSetAutoScrollOnInsert(gstChatInfo.list_ctl,
                        auto_scroll);
               }
               break;
            }
            break;
         case TDGTNF_MULTI_BTN_CLICKED:
            if (tdgt_notify.ctl_id == ID_CHAT_BTN_COLOR) {
               int color_index=INVALID;

               switch (tdgt_notify.nf_arg) { /* Button[1-3] */
               case Button1:
                  color_index = (int)(long)TdgtBtnGetText(
                        gstChatInfo.btn_ctl[CHAT_BTN_COLOR]);
                  if (++color_index >= maxColors) color_index = 0;
                  break;
               case Button2:
                  color_index = tdgt_notify.nf_arg2;
                  break;
               case Button3:
                  color_index = (int)(long)TdgtBtnGetText(
                        gstChatInfo.btn_ctl[CHAT_BTN_COLOR]);
                  if (--color_index < 0) color_index = maxColors-1;
                  break;
               }
               TdgtBtnSetText(gstChatInfo.btn_ctl[CHAT_BTN_COLOR],
                     (char*)(long)color_index, NULL);
               TdgtSmplEditSetColorIndex(gstChatInfo.edit_ctl, color_index);
               tgWriteProfileString("User", "Color",
                     colorMenuItems[color_index], gstChatInfo.ini_fname);
               tgWriteProfileString(NULL, NULL, NULL, gstChatInfo.ini_fname);
            }
            break;
         }
         event_handled = TRUE;
      }
   } else if (gstChatInfo.list_ctl != NULL &&
         TidgetEventHandler(gstChatInfo.list_ctl->pti, input, handling_pti)) {
      /* this event has been handled by the list */
      event_handled = TRUE;
   } else if (gstChatInfo.edit_ctl != NULL &&
         TidgetEventHandler(gstChatInfo.edit_ctl->pti, input, handling_pti)) {
      event_handled = TRUE;
   } else {
      int i=0;

      for (i=0; i < MAX_CHAT_BTNS; i++) {
         if (gstChatInfo.btn_ctl[i] != NULL &&
               TidgetEventHandler((gstChatInfo.btn_ctl[i])->pti, input,
               handling_pti)) {
            event_handled = TRUE;
         }
      }
   }
   if (keypress_event && !event_handled &&
         TidgetEventHandler(gstChatInfo.edit_ctl->pti, input, handling_pti)) {
   }
}

int IsChatWindowEvent(input, ppti_handler_tidget_return)
   XEvent *input;
   TidgetInfo **ppti_handler_tidget_return;
{
   int i=0;

   *ppti_handler_tidget_return = NULL;

   if (chatWindow == None) return FALSE;

   if (input->xany.window == chatWindow) return TRUE;
   if (gstChatInfo.list_ctl != NULL && IsTidgetEvent(gstChatInfo.list_ctl->pti,
         input, ppti_handler_tidget_return)) {
      return TRUE;
   }
   for (i=0; i < MAX_CHAT_BTNS; i++) {
      if (gstChatInfo.btn_ctl[i] != NULL &&
            IsTidgetEvent((gstChatInfo.btn_ctl[i])->pti, input,
            ppti_handler_tidget_return)) {
         return TRUE;
      }
   }
   if (gstChatInfo.edit_ctl != NULL && IsTidgetEvent(gstChatInfo.edit_ctl->pti,
         input, ppti_handler_tidget_return)) {
      return TRUE;
   }
   return FALSE;
}

#define BTN_Y_MARGIN 2

int GetInitialChatWindowHeight()
{
   int x=0, y=0;

   gstChatInfo.num_lines=6;
   if (msgFontSet == NULL && msgFontPtr == NULL) {
      gstChatInfo.font_height = defaultFontHeight;
      gstChatInfo.font_width = defaultFontWidth;
   } else {
      gstChatInfo.font_height = msgFontHeight;
      gstChatInfo.font_width = msgFontWidth;
   }
   gstChatInfo.btn_width = a1_width+(windowPadding<<1);

   /* gap */
   y = gstChatInfo.gap = 4;

   /* list control */
   gstChatInfo.list_win_info.x = gstChatInfo.gap;
   gstChatInfo.list_win_info.y = y;
   gstChatInfo.list_win_info.w = (-1);
   gstChatInfo.list_win_info.h =
         (gstChatInfo.num_lines*ROW_HEIGHT)+2+(windowPadding<<1);
   y += gstChatInfo.list_win_info.h;

   /* gap */
   y += gstChatInfo.gap;

   /* other controls */
   x = gstChatInfo.list_win_info.x;
   gstChatInfo.btn_win_info[CHAT_BTN_AUTO].x = x;
   gstChatInfo.btn_win_info[CHAT_BTN_AUTO].y = y;
   gstChatInfo.btn_win_info[CHAT_BTN_AUTO].w = gstChatInfo.btn_width;
   gstChatInfo.btn_win_info[CHAT_BTN_AUTO].h = gstChatInfo.btn_width;
   x += gstChatInfo.btn_width + (gstChatInfo.btn_width>>1);
   gstChatInfo.btn_win_info[CHAT_BTN_COLOR].x = x;
   gstChatInfo.btn_win_info[CHAT_BTN_COLOR].y = y;
   gstChatInfo.btn_win_info[CHAT_BTN_COLOR].w = gstChatInfo.btn_width;
   gstChatInfo.btn_win_info[CHAT_BTN_COLOR].h = gstChatInfo.btn_width;
   x += gstChatInfo.btn_width + 2;
   gstChatInfo.btn_win_info[CHAT_BTN_BOLD].x = x;
   gstChatInfo.btn_win_info[CHAT_BTN_BOLD].y = y;
   gstChatInfo.btn_win_info[CHAT_BTN_BOLD].w = gstChatInfo.btn_width;
   gstChatInfo.btn_win_info[CHAT_BTN_BOLD].h = gstChatInfo.btn_width;
   x += gstChatInfo.btn_width + 2;
   gstChatInfo.btn_win_info[CHAT_BTN_ITALIC].x = x;
   gstChatInfo.btn_win_info[CHAT_BTN_ITALIC].y = y;
   gstChatInfo.btn_win_info[CHAT_BTN_ITALIC].w = gstChatInfo.btn_width;
   gstChatInfo.btn_win_info[CHAT_BTN_ITALIC].h = gstChatInfo.btn_width;
   x += gstChatInfo.btn_width + (gstChatInfo.btn_width>>1);
   gstChatInfo.edit_win_info.x = x;
   gstChatInfo.edit_win_info.y = y;
   gstChatInfo.edit_win_info.w = (-1);
   gstChatInfo.edit_win_info.h = gstChatInfo.btn_width;
   x += gstChatInfo.btn_width + (gstChatInfo.btn_width>>1);
   gstChatInfo.btn_win_info[CHAT_BTN_SEND].x = (-1);
   gstChatInfo.btn_win_info[CHAT_BTN_SEND].y = y;
   gstChatInfo.btn_win_info[CHAT_BTN_SEND].w =
         ButtonWidth("Send", 4, NULL);
   gstChatInfo.btn_win_info[CHAT_BTN_SEND].h =
         (gstChatInfo.font_height+(BTN_Y_MARGIN<<1)+(windowPadding<<1));
   y += gstChatInfo.btn_width;

   /* gap */
   y += gstChatInfo.gap;

   return y;
}

void SetChatWindowGeom()
{
   int x=0, y=0;

   gstChatInfo.base_win_info.x = (-1);
   gstChatInfo.base_win_info.y = (-1);
   gstChatInfo.base_win_info.w = chatWindowW;
   gstChatInfo.base_win_info.h = chatWindowH;

   /* gap */
   y = gstChatInfo.gap;

   /* list control */
   gstChatInfo.list_win_info.x = gstChatInfo.gap;
   gstChatInfo.list_win_info.y = y;
   gstChatInfo.list_win_info.w =
         gstChatInfo.base_win_info.w - (gstChatInfo.gap<<1);
   gstChatInfo.list_win_info.h =
         (gstChatInfo.num_lines*ROW_HEIGHT)+2+(windowPadding<<1);

   y += gstChatInfo.list_win_info.h;

   /* gap */
   y += gstChatInfo.gap;

   /* other controls */
   x = gstChatInfo.list_win_info.x;
   gstChatInfo.btn_win_info[CHAT_BTN_AUTO].x = x;
   gstChatInfo.btn_win_info[CHAT_BTN_AUTO].y = y;
   gstChatInfo.btn_win_info[CHAT_BTN_AUTO].w = gstChatInfo.btn_width;
   gstChatInfo.btn_win_info[CHAT_BTN_AUTO].h = gstChatInfo.btn_width;
   x += gstChatInfo.btn_width + (gstChatInfo.btn_width>>1);
   gstChatInfo.btn_win_info[CHAT_BTN_COLOR].x = x;
   gstChatInfo.btn_win_info[CHAT_BTN_COLOR].y = y;
   gstChatInfo.btn_win_info[CHAT_BTN_COLOR].w = gstChatInfo.btn_width;
   gstChatInfo.btn_win_info[CHAT_BTN_COLOR].h = gstChatInfo.btn_width;
   x += gstChatInfo.btn_width + 2;
   gstChatInfo.btn_win_info[CHAT_BTN_BOLD].x = x;
   gstChatInfo.btn_win_info[CHAT_BTN_BOLD].y = y;
   gstChatInfo.btn_win_info[CHAT_BTN_BOLD].w = gstChatInfo.btn_width;
   gstChatInfo.btn_win_info[CHAT_BTN_BOLD].h = gstChatInfo.btn_width;
   x += gstChatInfo.btn_width + 2;
   gstChatInfo.btn_win_info[CHAT_BTN_ITALIC].x = x;
   gstChatInfo.btn_win_info[CHAT_BTN_ITALIC].y = y;
   gstChatInfo.btn_win_info[CHAT_BTN_ITALIC].w = gstChatInfo.btn_width;
   gstChatInfo.btn_win_info[CHAT_BTN_ITALIC].h = gstChatInfo.btn_width;
   x += gstChatInfo.btn_width + (gstChatInfo.btn_width>>1);
   gstChatInfo.edit_win_info.x = x;
   gstChatInfo.edit_win_info.y = y;
   gstChatInfo.edit_win_info.w = (-1);
   gstChatInfo.edit_win_info.h = gstChatInfo.btn_width;
   x += gstChatInfo.btn_width + (gstChatInfo.btn_width>>1);
   gstChatInfo.btn_win_info[CHAT_BTN_SEND].x = (-1);
   gstChatInfo.btn_win_info[CHAT_BTN_SEND].y = y;
   gstChatInfo.btn_win_info[CHAT_BTN_SEND].w =
         ButtonWidth("Send", 4, NULL);
   gstChatInfo.btn_win_info[CHAT_BTN_SEND].h = gstChatInfo.btn_width;
   y += gstChatInfo.btn_width;

   gstChatInfo.btn_win_info[CHAT_BTN_SEND].x =
         gstChatInfo.base_win_info.w - gstChatInfo.gap -
         (windowPadding<<1) - gstChatInfo.btn_win_info[CHAT_BTN_SEND].w;
   gstChatInfo.edit_win_info.w = gstChatInfo.btn_win_info[CHAT_BTN_SEND].x -
         (gstChatInfo.btn_width>>1) - gstChatInfo.edit_win_info.x;

   /* gap */
   y += gstChatInfo.gap;
}

void MapChatSubWindows()
{
   int i=0;

   if (gstChatInfo.list_ctl != NULL) MapTidget(gstChatInfo.list_ctl->pti);
   for (i=0; i < MAX_CHAT_BTNS; i++) {
      if (gstChatInfo.btn_ctl[i] != NULL) {
         MapTidget((gstChatInfo.btn_ctl[i])->pti);
      }
   }
   if (gstChatInfo.edit_ctl != NULL) MapTidget(gstChatInfo.edit_ctl->pti);
}

int CreateChatSubWindows()
{
   MouseOverStatusInfo mosi;
   char *buf=NULL;
   int new_alloc=FALSE, ival=0, font_style=STYLE_NR;
   int is_bold=FALSE, is_italic=FALSE, color_index=colorIndex;

   if ((buf=tgGetProfileString("User", "Color", gstChatInfo.ini_fname)) !=
         NULL) {
      color_index = QuickFindColorIndex(NULL, buf, &new_alloc, TRUE);
      tgFreeProfileString(buf);
   }
   if ((buf=tgGetProfileString("User", "Bold", gstChatInfo.ini_fname)) !=
         NULL) {
      if (sscanf(buf, "%d", &ival) == 1) {
         is_bold = ival;
      } else {
         tgWriteProfileString("User", "Bold", "0", gstChatInfo.ini_fname);
         tgWriteProfileString(NULL, NULL, NULL, gstChatInfo.ini_fname);
      }
      tgFreeProfileString(buf);
   }
   if ((buf=tgGetProfileString("User", "Italic", gstChatInfo.ini_fname)) !=
         NULL) {
      if (sscanf(buf, "%d", &ival) == 1) {
         is_italic = ival;
      } else {
         tgWriteProfileString("User", "Italic", "0", gstChatInfo.ini_fname);
         tgWriteProfileString(NULL, NULL, NULL, gstChatInfo.ini_fname);
      }
      tgFreeProfileString(buf);
   }
   if (is_bold) {
      if (is_italic) {
         font_style = STYLE_BI;
      } else {
         font_style = STYLE_BR;
      }
   } else {
      if (is_italic) {
         font_style = STYLE_NI;
      } else {
         font_style = STYLE_NR;
      }
   }
   memset(&mosi, 0, sizeof(MouseOverStatusInfo));
   mosi.one_line_status = TRUE;

   a1Pixmap = XCreateBitmapFromData(mainDisplay, mainWindow,
         (char*)a1_bits, a1_width, a1_height);
   bold1Pixmap = XCreateBitmapFromData(mainDisplay, mainWindow,
         (char*)bold1_bits, bold1_width, bold1_height);
   italic1Pixmap = XCreateBitmapFromData(mainDisplay, mainWindow,
         (char*)italic1_bits, italic1_width, italic1_height);

   gstChatInfo.list_ctl = CreateTdgtList(chatWindow, NULL, ID_CHAT_LIST,
         gstChatInfo.list_win_info.x, gstChatInfo.list_win_info.y,
         gstChatInfo.list_win_info.w, TDGTLIST_DEF_H_PAD, TDGTLIST_DEF_V_PAD,
         gstChatInfo.num_lines, FALSE, TRUE, TRUE);
   if (gstChatInfo.list_ctl == NULL) return FALSE;

   UtilStrCpyN(mosi.one_line_str, sizeof(mosi.one_line_str),
         TgLoadString(STID_TOGGLE_AUTO_SCR_IN_CHAT));
   gstChatInfo.btn_ctl[CHAT_BTN_AUTO] = CreateTdgtBtn(chatWindow, NULL,
         ID_CHAT_BTN_AUTO, gstChatInfo.btn_win_info[CHAT_BTN_AUTO].x,
         gstChatInfo.btn_win_info[CHAT_BTN_AUTO].y,
         gstChatInfo.btn_win_info[CHAT_BTN_AUTO].w,
         gstChatInfo.btn_win_info[CHAT_BTN_AUTO].h,
         0, 0, TGMUTYPE_BITMAP, TDGTBTN_STICKY, TGBS_LOWRED, STYLE_NR,
         (char*)(&a1Pixmap), &mosi);

   UtilStrCpyN(mosi.one_line_str, sizeof(mosi.one_line_str),
         TgLoadString(STID_TOGGLE_BOLD_IN_CHAT));
   gstChatInfo.btn_ctl[CHAT_BTN_BOLD] = CreateTdgtBtn(chatWindow, NULL,
         ID_CHAT_BTN_BOLD, gstChatInfo.btn_win_info[CHAT_BTN_BOLD].x,
         gstChatInfo.btn_win_info[CHAT_BTN_BOLD].y,
         gstChatInfo.btn_win_info[CHAT_BTN_BOLD].w,
         gstChatInfo.btn_win_info[CHAT_BTN_BOLD].h,
         0, 0, TGMUTYPE_BITMAP, TDGTBTN_STICKY,
         (is_bold ? TGBS_LOWRED : TGBS_NORMAL), STYLE_BR,
         (char*)(&bold1Pixmap), &mosi);

   UtilStrCpyN(mosi.one_line_str, sizeof(mosi.one_line_str),
         TgLoadString(STID_TOGGLE_ITALIC_IN_CHAT));
   gstChatInfo.btn_ctl[CHAT_BTN_ITALIC] = CreateTdgtBtn(chatWindow, NULL,
         ID_CHAT_BTN_ITALIC, gstChatInfo.btn_win_info[CHAT_BTN_ITALIC].x,
         gstChatInfo.btn_win_info[CHAT_BTN_ITALIC].y,
         gstChatInfo.btn_win_info[CHAT_BTN_ITALIC].w,
         gstChatInfo.btn_win_info[CHAT_BTN_ITALIC].h,
         0, 0, TGMUTYPE_BITMAP, TDGTBTN_STICKY,
         (is_italic ? TGBS_LOWRED : TGBS_NORMAL), STYLE_NI,
         (char*)(&italic1Pixmap), &mosi);

   UtilStrCpyN(mosi.one_line_str, sizeof(mosi.one_line_str),
         TgLoadString(STID_SEND_CHAT_TEXT));
   gstChatInfo.btn_ctl[CHAT_BTN_SEND] = CreateTdgtBtn(chatWindow, NULL,
         ID_CHAT_BTN_SEND, gstChatInfo.btn_win_info[CHAT_BTN_SEND].x,
         gstChatInfo.btn_win_info[CHAT_BTN_SEND].y,
         gstChatInfo.btn_win_info[CHAT_BTN_SEND].w,
         gstChatInfo.btn_win_info[CHAT_BTN_SEND].h,
         0, 0, TGMUTYPE_TEXT, TDGTBTN_CLICK, TGBS_NORMAL, STYLE_NR, "Send",
         &mosi);

   mosi.one_line_status = FALSE;
   UtilStrCpyN(mosi.btn_str[0], sizeof(mosi.btn_str[0]), colorMouseStatus[0].l);
   UtilStrCpyN(mosi.btn_str[1], sizeof(mosi.btn_str[1]), colorMouseStatus[0].m);
   UtilStrCpyN(mosi.btn_str[2], sizeof(mosi.btn_str[2]), colorMouseStatus[0].r);
   gstChatInfo.btn_ctl[CHAT_BTN_COLOR] = CreateTdgtBtn(chatWindow, NULL,
         ID_CHAT_BTN_COLOR, gstChatInfo.btn_win_info[CHAT_BTN_COLOR].x,
         gstChatInfo.btn_win_info[CHAT_BTN_COLOR].y,
         gstChatInfo.btn_win_info[CHAT_BTN_COLOR].w,
         gstChatInfo.btn_win_info[CHAT_BTN_COLOR].h,
         0, 0, TGMUTYPE_COLOR, TDGTBTN_CLICK, TGBS_NORMAL, INVALID,
         (char*)(long)color_index, &mosi);

   gstChatInfo.edit_ctl = CreateTdgtSmplEdit(chatWindow, NULL, ID_CHAT_EDIT,
         gstChatInfo.edit_win_info.x, gstChatInfo.edit_win_info.y,
         gstChatInfo.edit_win_info.w, TDGTSEDT_DEF_H_PAD, TDGTSEDT_DEF_V_PAD,
         "", font_style, color_index);
   if (gstChatInfo.edit_ctl == NULL) return FALSE;

   return TRUE;
}

/* --------------------- ChatInsertChatLine --------------------- */

void ChatAppendChatLine(ptfi, nick_name, encoding, buf)
   TextFormatInfo *ptfi;
   char *nick_name, *encoding, *buf;
{
   char *clear_text=NULL;
   ListItemInfo *pListItemInfo=(ListItemInfo*)malloc(sizeof(ListItemInfo));

   if (pListItemInfo == NULL) FailAllocMessage();
   memset(pListItemInfo, 0, sizeof(ListItemInfo));

   if (*encoding == '\0') {
      clear_text = buf;
   } else if (strcmp(encoding, "base64") == 0) {
      clear_text = Base64Decode(buf);
   } else {
      /* reject */
   }
   UtilStrCpyN(pListItemInfo->nick_name, sizeof(pListItemInfo->nick_name),
         nick_name);
   pListItemInfo->color_index = ptfi->color_index;
   pListItemInfo->font_style = ptfi->font_style;
   pListItemInfo->buf = UtilStrDup(clear_text);
   if (pListItemInfo->buf == NULL) FailAllocMessage();

   TdgtListInsertListItemInfo(gstChatInfo.list_ctl, pListItemInfo);

   UtilFree(clear_text);
}

/* --------------------- ChatAddUser --------------------- */

void ChatAddUser(buf)
   char *buf;
{
}

/* --------------------- Init & Clean Up --------------------- */

int InitChat()
{
   memset(&gstChatInfo, 0, sizeof(ChatInfo));

   sprintf(gstChatInfo.ini_fname, "%s%cchat.ini", tgifDir, DIR_SEP);
   return InitChatNickName();
}

void CleanUpChat()
{
   int i=0;

   UtilFree(gpszChatNickName);
   gpszChatNickName = NULL;

   if (a1Pixmap != None) XFreePixmap(mainDisplay, a1Pixmap);
   if (bold1Pixmap != None) XFreePixmap(mainDisplay, bold1Pixmap);
   if (italic1Pixmap != None) XFreePixmap(mainDisplay, italic1Pixmap);
   a1Pixmap = bold1Pixmap = italic1Pixmap = None;

   if (gstChatInfo.list_ctl != NULL) {
      TidgetInfo *pti=((gstChatInfo.list_ctl)->pti);

      DestroyTidget(&pti);
   }
   for (i=0; i < MAX_CHAT_BTNS; i++) {
      if (gstChatInfo.btn_ctl[i] != NULL) {
         TidgetInfo *pti=((gstChatInfo.btn_ctl[i])->pti);

         DestroyTidget(&pti);
      }
   }
   if (gstChatInfo.edit_ctl != NULL) {
      TidgetInfo *pti=((gstChatInfo.edit_ctl)->pti);

      DestroyTidget(&pti);
   }
}

