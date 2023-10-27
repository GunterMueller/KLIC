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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/tdgtsedt.c,v 1.11 2011/05/16 16:22:00 william Exp $
 */

#define _INCLUDE_FROM_TDGTSEDT_C_

#include "tgifdefs.h"
#include "patchlvl.h"
#include "tidget.h"

#include "auxtext.e"
#include "choose.e"
#include "color.e"
#include "cursor.e"
#include "dialog.e"
#include "font.e"
#include "inmethod.e"
#include "menu.e"
#include "msg.e"
#include "raster.e"
#include "rect.e"
#include "setup.e"
#include "tdgtbase.e"
#include "tdgtsedt.e"
#include "text.e"
#include "tidget.e"
#include "util.e"

static void RedrawTdgtSmplEdit ARGS_DECL((TidgetInfo *pti));
static int TdgtSmplEditEventHandler ARGS_DECL((TidgetInfo *pti, XEvent *input,
                TidgetInfo *handling_pti));
static int IsTdgtSmplEditEvent ARGS_DECL((TidgetInfo *pti, XEvent *input,
                TidgetInfo **ppti_handler_tidget_return));
static void DestroyTdgtSmplEdit ARGS_DECL((TidgetInfo *pti));
static void MapTdgtSmplEdit ARGS_DECL((TidgetInfo *pti));
static void TdgtSmplEditMoveResize ARGS_DECL((TidgetInfo *pti, int x, int y,
                int w, int h));
static int TdgtSmplEditSendCmd ARGS_DECL((TidgetInfo *pti, int cmd_type,
                int cmd_arg, void *pv_cmd_userdata));

/* --------------------- Utility Functions --------------------- */

/* --------------------- RedrawTdgtSmplEdit() --------------------- */

#define RIGHT_GAP 2

static
char *TdgtSmplEditBinarySearch(caption, len, max_w, font_ptr,
      pf_text_width_func)
   char *caption;
   int len, max_w;
   XFontStruct *font_ptr;
   TidgetMsgTextWidthFunc *pf_text_width_func;
{
   /*
    * When &caption[too_long-1] is too long and &caption[too_long] is not
    *         long enough, return &caption[too_long].
    */
   int prev_too_long=INVALID, too_long=0, too_short=len, found=FALSE;
   
   while (!found) {
      int w=0;

      if (too_short < too_long) {
#ifdef _TGIF_DBG /* debug, do not translate */
         sprintf(gszMsgBox, "Programing error in TdgtSmplEditBinarySearch()!\n");
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
#endif /* _TGIF_DBG */
      }
      if (too_short-too_long <= 4) {
         w = (pf_text_width_func)(font_ptr, &caption[too_long], len-too_long);
         if (w > max_w) {
            prev_too_long = too_long;
            too_long++;
         } else {
            return (&caption[too_long]);
         }
      } else {
         int mid=((too_long+too_short)>>1);

         w = (pf_text_width_func)(font_ptr, &caption[mid], len-mid);
         if (w > max_w) {
            too_long = mid;
         } else {
            too_short = mid;
         }
      }
   }
   return caption;
}

static
void TdgtSmplEditDrawCaption(pTdgtSmplEdit)
   TdgtSmplEdit *pTdgtSmplEdit;
{
   int x=pTdgtSmplEdit->client_area.x, y=pTdgtSmplEdit->client_area.y, w=0;
   int font_asc=0, client_area_w=pTdgtSmplEdit->client_area.w;
   char *caption=TdgtSmplEditGetText(pTdgtSmplEdit), *psz_start=NULL;
   XFontStruct *font_ptr=NULL;
   TidgetDrawMsgStringFunc *pf_draw_string_func=NULL;
   TidgetMsgTextWidthFunc *pf_text_width_func=NULL;

   if (caption == NULL) return;

   TidgetGetFontInfoGivenStyle(pTdgtSmplEdit->font_style, &font_ptr, NULL, NULL,
         &font_asc, NULL);
   y += font_asc;
   switch (pTdgtSmplEdit->font_style) {
   case STYLE_NR:
      if (msgFontPtr != NULL) {
         XSetFont(mainDisplay, gTidgetManager.gc, msgFontPtr->fid);
      }
      pf_draw_string_func = DrawMsgString;
      pf_text_width_func = MsgTextWidth;
      break;
   case STYLE_BR:
      if (boldMsgFontPtr != NULL) {
         XSetFont(mainDisplay, gTidgetManager.gc, boldMsgFontPtr->fid);
      }
      pf_draw_string_func = DrawBoldMsgString;
      pf_text_width_func = BoldMsgTextWidth;
      break;
   case STYLE_NI:
      if (italicMsgFontPtr != NULL) {
         XSetFont(mainDisplay, gTidgetManager.gc, italicMsgFontPtr->fid);
      }
      pf_draw_string_func = DrawItalicMsgString;
      pf_text_width_func = ItalicMsgTextWidth;
      break;
   case STYLE_BI:
      if (boldItalicMsgFontPtr != NULL) {
         XSetFont(mainDisplay, gTidgetManager.gc, boldItalicMsgFontPtr->fid);
      }
      pf_draw_string_func = DrawBoldItalicMsgString;
      pf_text_width_func = BoldItalicMsgTextWidth;
      break;
   }
   w = (pf_text_width_func)(font_ptr, caption, strlen(caption));
   if (w > client_area_w-RIGHT_GAP) {
      psz_start = TdgtSmplEditBinarySearch(caption, strlen(caption),
            client_area_w-RIGHT_GAP, font_ptr, pf_text_width_func);
   } else {
      psz_start = caption;
   }
   if (pTdgtSmplEdit->color_index == INVALID) {
      XSetForeground(mainDisplay, gTidgetManager.gc, myFgPixel);
   } else {
      XSetForeground(mainDisplay, gTidgetManager.gc,
            colorPixels[pTdgtSmplEdit->color_index]);
   }
   (pf_draw_string_func)(mainDisplay, pTdgtSmplEdit->pti->tci.win,
         gTidgetManager.gc, x, y, psz_start, strlen(psz_start));

   if (TidgetIsFocus(pTdgtSmplEdit->pti)) {
      x = (pf_text_width_func)(font_ptr, psz_start, strlen(psz_start)) +
            pTdgtSmplEdit->client_area.x;
      XSetForeground(mainDisplay, gTidgetManager.gc, myFgPixel);
      XDrawLine(mainDisplay, pTdgtSmplEdit->pti->tci.win, gTidgetManager.gc,
            x+1, pTdgtSmplEdit->client_area.y+1, x+1,
            pTdgtSmplEdit->client_area.y+pTdgtSmplEdit->client_area.h-1);
      TidgetDrawFocusRect(pTdgtSmplEdit->pti, &pTdgtSmplEdit->client_area);
   }
   TidgetManagerResetGC();
}

static
void RedrawTdgtSmplEdit(pti)
   TidgetInfo *pti;
{
   TdgtSmplEdit *pTdgtSmplEdit=(TdgtSmplEdit*)(pti->tidget);

   XClearWindow(mainDisplay, pTdgtSmplEdit->pti->tci.win);

   if (threeDLook) {
      struct BBRec bbox;
   
      SetBBRec(&bbox, 0, 0, pTdgtSmplEdit->pti->tci.win_info.w,
            pTdgtSmplEdit->pti->tci.win_info.h);
      switch (pTdgtSmplEdit->pti->tci.state) {
      case TGBS_NORMAL: break;
      default:
         TgDrawThreeDButton(mainDisplay, pTdgtSmplEdit->pti->tci.win,
               gTidgetManager.gc, &bbox, pTdgtSmplEdit->pti->tci.state,
               2, FALSE);
         TidgetManagerResetGC();
         break;
      }
   } else {
      /*
       * XDrawRectangle(mainDisplay, pTdgtSmplEdit->pti->tci.win,
       *         gTidgetManager.gc, 0, 0, pTdgtSmplEdit->pti->tci.win_info.w,
       *         pTdgtSmplEdit->pti->tci.win_info.h);
       */
   }
   TdgtSmplEditDrawCaption(pTdgtSmplEdit);
}

/* --------------------- TdgtSmplEditEventHandler() --------------------- */

static XComposeStatus c_stat;

static
int TdgtSmplEditEventHandler(pti, input, handling_pti)
   TidgetInfo *pti, *handling_pti;
   XEvent *input;
{
   TdgtSmplEdit *pTdgtSmplEdit=(TdgtSmplEdit*)(pti->tidget);

   if (pti != handling_pti) return FALSE;

   if (input->type == Expose) {
      XEvent ev;

      RedrawTdgtSmplEdit(pTdgtSmplEdit->pti);
      while (XCheckWindowEvent(mainDisplay, pTdgtSmplEdit->pti->tci.win,
            ExposureMask, &ev)) ;
   } else if (input->type == KeyPress) {
      int has_ch=0;
      char buf[80];
      KeySym key_sym=(KeySym)0;
      XKeyEvent *key_ev=(&(input->xkey));

      if (gnInputMethod != TGIM_NONE &&
            /*
             * Cannot call: InputMethodTypeMatched(canvasFontDoubleByte) &&
             */
            tgIMExpectLookupString(mainDisplay, pTdgtSmplEdit->pti->tci.win)) {
         if (tgIMHandleLookupString(mainDisplay, pTdgtSmplEdit->pti->tci.win,
               key_ev, buf, sizeof(buf), &key_sym, &has_ch)) {
            if (has_ch > 0) buf[has_ch] = '\0';
            has_ch = HandleDoubleByteUTF8Chars(buf, has_ch);
         }
         TranslateKeys(buf, &key_sym);
      } else {
         has_ch = XLookupString(key_ev, buf, sizeof(buf), &key_sym, &c_stat);
         if (has_ch > 0) buf[has_ch] = '\0';
         if (gnInputMethod != TGIM_NONE) {
            char s1[80];

            *s1 = '\0';
            if (tgIMTranslateKeyEvent(mainDisplay, mainWindow, key_ev, s1)) {
               strcpy(buf, s1);
               /* added by Ambrose Li <acli@mingpaoxpress.com> */
               key_sym = (KeySym)0;
            }
            if (tgIMExpectClientMessage(mainDisplay, mainWindow)) {
               /* input characters only come from ClientMessage */
               return FALSE;
            }
         } else {
            TranslateKeys(buf, &key_sym);
         }
      }
      if (CharIsCRorLF(key_ev, buf, key_sym, &has_ch)) {
         TidgetControlNotify(pTdgtSmplEdit->pti, TDGTNF_EDIT_ENTERED, 0, 0);
         return TRUE;
      } else if (CharIsBS(key_ev, buf, key_sym, &has_ch, FALSE)) {
         TdgtSmplEditDelLastChar(pTdgtSmplEdit);
         return TRUE;
      } else if (key_sym>='\040' && key_sym<='\377') {
         TdgtSmplEditAppendStr(pTdgtSmplEdit, buf, 1);
         return TRUE;
      }
   } else if (input->type == ButtonPress) {
      if (pTdgtSmplEdit->cursor_shown) {
         /* already has focus, don't do anythingn */
      } else {
         TdgtBase *pTdgtBase=
               (TdgtBase*)(pTdgtSmplEdit->pti->tci.parent_tidgetinfo->tidget);

         if (TidgetSendCmd(pTdgtBase->pti, TDGTCMD_CAN_HANDLE_FOCUS_CHANGE,
               0, NULL)) {
            TidgetSendCmd(pTdgtBase->pti, TDGTCMD_REMOVE_FOCUS, 0, NULL);
         }
         TdgtSmplEditSetFocus(pTdgtSmplEdit, TRUE);
      }
   }
   return FALSE;
}

/* --------------------- IsTdgtSmplEditEvent() --------------------- */

static
int IsTdgtSmplEditEvent(pti, input, ppti_handler_tidget_return)
   TidgetInfo *pti, **ppti_handler_tidget_return;
   XEvent *input;
{
   TdgtSmplEdit *pTdgtSmplEdit=(TdgtSmplEdit*)(pti->tidget);

   if (input->xany.window == pTdgtSmplEdit->pti->tci.win) {
      *ppti_handler_tidget_return = pti;
      return TRUE;
   }
   return FALSE;
}

/* --------------------- DestroyTdgtSmplEdit() --------------------- */

static
void DestroyTdgtSmplEdit(pti)
   TidgetInfo *pti;
{
   TdgtSmplEdit *pTdgtSmplEdit=(TdgtSmplEdit*)(pti->tidget);

   TdgtSmplEditReset(pTdgtSmplEdit);

   free(pTdgtSmplEdit);
}

/* --------------------- MapTdgtSmplEdit() --------------------- */

static
void MapTdgtSmplEdit(pti)
   TidgetInfo *pti;
{
   TdgtSmplEdit *pTdgtSmplEdit=(TdgtSmplEdit*)(pti->tidget);

#ifdef MAPBEFORESELECT
   XMapWindow(mainDisplay, pTdgtSmplEdit->pti->tci.win);
   XSelectInput(mainDisplay, pTdgtSmplEdit->pti->tci.win,
         ButtonReleaseMask | ButtonPressMask | KeyPressMask | ExposureMask |
         EnterWindowMask | LeaveWindowMask);
#else
   XSelectInput(mainDisplay, pTdgtSmplEdit->pti->tci.win,
         ButtonReleaseMask | ButtonPressMask | KeyPressMask | ExposureMask |
         EnterWindowMask | LeaveWindowMask);
   XMapWindow(mainDisplay, pTdgtSmplEdit->pti->tci.win);
#endif
}

/* --------------------- TdgtSmplEditMoveResize() --------------------- */

static
void TdgtSmplEditMoveResize(pti, x, y, w, h)
   TidgetInfo *pti;
   int x, y, w, h;
{
   TdgtSmplEdit *pTdgtSmplEdit=(TdgtSmplEdit*)(pti->tidget);
   int h_pad=pti->tci.h_pad, v_pad=pti->tci.v_pad;

   /* there should be no need to resize a button */
   pTdgtSmplEdit->pti->tci.win_info.x = x;
   pTdgtSmplEdit->pti->tci.win_info.y = y;
   pTdgtSmplEdit->pti->tci.win_info.w = w;
   pTdgtSmplEdit->pti->tci.win_info.h = h;
   XMoveResizeWindow(mainDisplay, pTdgtSmplEdit->pti->tci.win, x, y, w, h);

   pTdgtSmplEdit->client_area.x = windowPadding + h_pad;
   pTdgtSmplEdit->client_area.y = windowPadding + v_pad;
   pTdgtSmplEdit->client_area.w = w-(windowPadding<<1)-(h_pad<<1);
   pTdgtSmplEdit->client_area.h = h-(windowPadding<<1)-(v_pad<<1);
}

/* --------------------- TdgtSmplEditSendCmd() --------------------- */

static
int TdgtSmplEditSendCmd(pti, cmd_type, cmd_arg, pv_cmd_userdata)
   TidgetInfo *pti;
   int cmd_type, cmd_arg;
   void *pv_cmd_userdata;
{
   TdgtSmplEdit *pTdgtSmplEdit=(TdgtSmplEdit*)(pti->tidget);

   if (pTdgtSmplEdit != NULL) {
   }
   return FALSE;
}

/* --------------------- TdgtSmplEditReset() --------------------- */

void TdgtSmplEditReset(pTdgtSmplEdit)
   TdgtSmplEdit *pTdgtSmplEdit;
{
   FreeDynStrBuf(&(pTdgtSmplEdit->pti->tci.dyn_str));
}

/* --------------------- CreateTdgtSmplEdit() --------------------- */

static
void TdgtSmplEditCalcHeight(font_style, pn_content_h)
   int font_style, *pn_content_h;
{
   int font_height=0;

   TidgetGetFontInfoGivenStyle(font_style, NULL, NULL, &font_height, NULL,
         NULL);

   if (pn_content_h != NULL) *pn_content_h = font_height;
}

TdgtSmplEdit *CreateTdgtSmplEdit(parent_win, parent_tidgetinfo, ctl_id, x, y,
      content_w, h_pad, v_pad, caption, font_style, color_index)
   Window parent_win;
   TidgetInfo *parent_tidgetinfo;
   int ctl_id, x, y, content_w, h_pad, v_pad, font_style, color_index;
   char *caption;
{
   int w=0, h=0, content_h=0;
   TdgtSmplEdit *pTdgtSmplEdit=NULL;

   TdgtSmplEditCalcHeight(font_style, &content_h);
   w = content_w + (windowPadding<<1) + (h_pad<<1);
   h = content_h + (windowPadding<<1) + (v_pad<<1);

   pTdgtSmplEdit = (TdgtSmplEdit*)malloc(sizeof(TdgtSmplEdit));
   if (pTdgtSmplEdit == NULL) FailAllocMessage();
   memset(pTdgtSmplEdit, 0, sizeof(TdgtSmplEdit));

   pTdgtSmplEdit->pti = NewTidgetInfo(parent_tidgetinfo, TIDGET_TYPE_SEDT,
         pTdgtSmplEdit, ctl_id, NULL);
   if ((pTdgtSmplEdit->pti->tci.win=XCreateSimpleWindow(mainDisplay, parent_win,
         x, y, w, h, brdrW, myBorderPixel, myBgPixel)) == 0) {
      FailToCreateWindowMessage("CreateTdgtSmplEdit()", NULL, TRUE);
      return NULL;
   }
   XSelectInput(mainDisplay, pTdgtSmplEdit->pti->tci.win,
         ButtonReleaseMask | ButtonPressMask | KeyPressMask | ExposureMask |
         EnterWindowMask | LeaveWindowMask);
   SetTidgetInfoBasic(pTdgtSmplEdit->pti, TIDGET_TYPE_SEDT, pTdgtSmplEdit,
         parent_win, x, y, w, h, h_pad, v_pad, TGBS_LOWRED, caption);
   TidgetSetCallbacks(pTdgtSmplEdit->pti,
         RedrawTdgtSmplEdit, TdgtSmplEditEventHandler, IsTdgtSmplEditEvent,
         DestroyTdgtSmplEdit, MapTdgtSmplEdit, TdgtSmplEditMoveResize,
         TdgtSmplEditSendCmd);

   pTdgtSmplEdit->client_area.x = windowPadding + h_pad;
   pTdgtSmplEdit->client_area.y = windowPadding + v_pad;
   pTdgtSmplEdit->client_area.w = w-(windowPadding<<1)-(h_pad<<1);
   pTdgtSmplEdit->client_area.h = h-(windowPadding<<1)-(v_pad<<1);

   pTdgtSmplEdit->font_style = font_style;
   pTdgtSmplEdit->color_index = color_index;

   return pTdgtSmplEdit;
}

/* --------------------- TdgtSmplEditSetText() --------------------- */

int TdgtSmplEditSetText(pTdgtSmplEdit, str)
   TdgtSmplEdit *pTdgtSmplEdit;
   char *str;
{
   int len=strlen(str);

   if (len+1 == pTdgtSmplEdit->pti->tci.dyn_str.sz &&
         strcmp(str, pTdgtSmplEdit->pti->tci.dyn_str.s) == 0) {
      /* same string, no change */
      return FALSE;
   }
   FreeDynStrBuf(&(pTdgtSmplEdit->pti->tci.dyn_str));

   if (str != NULL) {
      DynStrSet(&(pTdgtSmplEdit->pti->tci.dyn_str), str);
   }
   RedrawTdgtSmplEdit(pTdgtSmplEdit->pti);

   return TRUE;
}

/* --------------------- TdgtSmplEditGetText() --------------------- */

char *TdgtSmplEditGetText(pTdgtSmplEdit)
   TdgtSmplEdit *pTdgtSmplEdit;
{
   return pTdgtSmplEdit->pti->tci.dyn_str.s;
}

/*--------------------- TdgtSmplEditGetTextFormatInfo() -------------------- */

void TdgtSmplEditGetTextFormatInfo(pTdgtSmplEdit, pTextFormatInfo)
   TdgtSmplEdit *pTdgtSmplEdit;
   TextFormatInfo *pTextFormatInfo;
{
   if (pTextFormatInfo != NULL) {
      pTextFormatInfo->font_style = pTdgtSmplEdit->font_style;
      pTextFormatInfo->color_index = pTdgtSmplEdit->color_index;
      if (pTextFormatInfo->color_index == INVALID) {
         *pTextFormatInfo->color_str = '\0';
      } else {
         UtilStrCpyN(pTextFormatInfo->color_str,
               sizeof(pTextFormatInfo->color_str),
               colorMenuItems[pTextFormatInfo->color_index]);
      }
   }
}

/* --------------------- TdgtSmplEditSetColorIndex() --------------------- */

int TdgtSmplEditSetColorIndex(pTdgtSmplEdit, color_index)
   TdgtSmplEdit *pTdgtSmplEdit;
   int color_index;
{
   int prev_color_index=pTdgtSmplEdit->color_index;

   if (prev_color_index != color_index) {
      pTdgtSmplEdit->color_index = color_index;
      RedrawTdgtSmplEdit(pTdgtSmplEdit->pti);
   }
   return prev_color_index;
}

/* --------------------- TdgtSmplEditGetColorIndex() --------------------- */

int TdgtSmplEditGetColorIndex(pTdgtSmplEdit)
   TdgtSmplEdit *pTdgtSmplEdit;
{
   return pTdgtSmplEdit->color_index;
}

/* --------------------- TdgtSmplEditSetFontStyle() --------------------- */

int TdgtSmplEditSetFontStyle(pTdgtSmplEdit, font_style)
   TdgtSmplEdit *pTdgtSmplEdit;
   int font_style;
{
   int prev_font_style=pTdgtSmplEdit->font_style;

   if (prev_font_style != font_style) {
      pTdgtSmplEdit->font_style = font_style;
      RedrawTdgtSmplEdit(pTdgtSmplEdit->pti);
   }
   return prev_font_style;
}

/* --------------------- TdgtSmplEditGetFontStyle() --------------------- */

int TdgtSmplEditGetFontStyle(pTdgtSmplEdit)
   TdgtSmplEdit *pTdgtSmplEdit;
{
   return pTdgtSmplEdit->font_style;
}

/* --------------------- TdgtSmplEditAppendStr() --------------------- */

int TdgtSmplEditAppendStr(pTdgtSmplEdit, buf, len)
   TdgtSmplEdit *pTdgtSmplEdit;
   char *buf;
   int len;
   /* return TRUE if something changed */
{
   char *psz=NULL;

   if (len == 0 || *buf == '\0') return FALSE;

   /* Note: dyn_str.sz includes the '\0' */
   psz = (char*)malloc(pTdgtSmplEdit->pti->tci.dyn_str.sz+len);

   if (psz == NULL) FailAllocMessage();
   strcpy(psz, pTdgtSmplEdit->pti->tci.dyn_str.s);
   strncpy(&psz[pTdgtSmplEdit->pti->tci.dyn_str.sz-1], buf, len);
   psz[pTdgtSmplEdit->pti->tci.dyn_str.sz-1+len] = '\0';

   DynStrSet(&(pTdgtSmplEdit->pti->tci.dyn_str), psz);
   free(psz);

   RedrawTdgtSmplEdit(pTdgtSmplEdit->pti);

   return TRUE;
}

/* --------------------- TdgtSmplEditDelLastChar() --------------------- */

int TdgtSmplEditDelLastChar(pTdgtSmplEdit)
   TdgtSmplEdit *pTdgtSmplEdit;
   /* return TRUE if something changed */
{
   int len=pTdgtSmplEdit->pti->tci.dyn_str.sz-1;

   if (len <= 0) return FALSE;

   pTdgtSmplEdit->pti->tci.dyn_str.s[len-1] = '\0';
   pTdgtSmplEdit->pti->tci.dyn_str.sz = len;

   RedrawTdgtSmplEdit(pTdgtSmplEdit->pti);

   return TRUE;
}

/* --------------------- TdgtSmplEditSetFocus() --------------------- */

static
int TdgtSmplEditShowCursor(pTdgtSmplEdit, show)
   TdgtSmplEdit *pTdgtSmplEdit;
   int show;
   /* return TRUE if changed */
{
   if (pTdgtSmplEdit->cursor_shown != show) {
      pTdgtSmplEdit->cursor_shown = show;
      return TRUE;
   }
   return FALSE;
}

void TdgtSmplEditSetFocus(pTdgtSmplEdit, nSet)
   TdgtSmplEdit *pTdgtSmplEdit;
   int nSet;
{
   if (TdgtSmplEditShowCursor(pTdgtSmplEdit, nSet)) {
      if (nSet) {
         TidgetSetHasFocus(TRUE);
         TidgetSetFocusWindow(pTdgtSmplEdit->pti->tci.win);
      } else {
         TidgetSetHasFocus(FALSE);
         TidgetSetFocusWindow(None);
      }
      RedrawTdgtSmplEdit(pTdgtSmplEdit->pti);
   }
}

/* --------------------- Init & Clean Up --------------------- */

int InitTdgtSmplEdit()
{
   return TRUE;
}

void CleanUpTdgtSmplEdit()
{
}

