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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/tgisdl.c,v 1.15 2011/05/16 16:22:00 william Exp $
 */

#define _INCLUDE_FROM_TGISDL_C_

#include "tgifdefs.h"
#include "tidget.h"

#include "auxtext.e"
#include "dialog.e"
#include "file.e"
#include "font.e"
#include "mainloop.e"
#include "msg.e"
#include "names.e"
#include "raster.e"
#include "setup.e"
#include "strtbl.e"
#include "tdgtbase.e"
#include "tdgtbrow.e"
#include "tdgtbtn.e"
#include "tdgtdraw.e"
#include "text.e"
#include "tidget.e"
#include "tgisdl.e"
#include "util.e"

InsertSymbolInfo gInsertSymbolInfo;

typedef struct tagInsertSymbolDlgInfo {
   Window parent_win; /* should be the rootWindow */

   TdgtDraw *sym_pixmap_ctl; /* user pick HS values here */
   TdgtBtnRow *btn_row_ctl;

   int screen_w, screen_h;
   int base_win_w, base_win_h;

   int sym_x, sym_y;

   int btn_row_x, btn_row_y, btn_row_w, btn_row_h;

   char *title_str, *formatted_title_str;
   int def_btn_id;

   int sym_pixmap_x, sym_pixmap_y, v_pixmap_x, v_pixmap_y;

   unsigned char r, g, b;
   int h, s, v; /* 0 <= h,s,v <= 255 */
   int pixel;
   char color_str[40];

   TrueColorInfo tci;
} InsertSymbolDlgInfo;

/* --------------------- Utility Functions --------------------- */

#define TDGTARG_INSSYM_DPY_CLEAR 1 /* pv_cmd_userdata is NULL */

static
void ClearAllControls(pTdgtBase)
   TdgtBase *pTdgtBase;
{
   InsertSymbolDlgInfo *pisdi=(InsertSymbolDlgInfo*)(pTdgtBase->pti->userdata);

   TidgetSendCmd(pisdi->sym_pixmap_ctl->pti, TDGTCMD_DRAW_USER,
         TDGTARG_INSSYM_DPY_CLEAR, pTdgtBase);
}

static
char InsertSymbolGetSelectedChar()
{
   int row=gInsertSymbolInfo.selected_row, col=gInsertSymbolInfo.selected_col;
   int index=0;

   if (!gInsertSymbolInfo.something_selected) return '\0';

   if (row > 6) {
      index = 0xa0 + ((row-7)<<4) + col;
   } else {
      index = 0x20 + (row<<4) + col;
   }
   return (char)(((unsigned int)index) & 0x0ff);
}

static
int InsertSymbolInAppl(pTdgtBase)
   TdgtBase *pTdgtBase;
{
   char buf[3];

   buf[0] = InsertSymbolGetSelectedChar();
   buf[1] = buf[2] = '\0';
   HandleInputFromBuffer(buf);

   return TRUE;
}

static
void InsertSymbolToggleSelected(pTdgtDraw)
   TdgtDraw *pTdgtDraw;
{
   int x=windowPadding+TDGTBASE_DEF_H_PAD-1;
   int y=windowPadding+TDGTBASE_DEF_V_PAD-1;

   x += (gInsertSymbolInfo.a_symbol_w+1)*gInsertSymbolInfo.selected_col;
   y += (gInsertSymbolInfo.a_symbol_h+1)*gInsertSymbolInfo.selected_row;

   XFillRectangle(mainDisplay, pTdgtDraw->pti->tci.win, revDefaultGC,
         x, y, gInsertSymbolInfo.a_symbol_w, gInsertSymbolInfo.a_symbol_h);
}

static
int InsertSymbolSelectedValid(row, col)
   int row, col;
{
   if (row >= TGIS_ROWS || col >= TGIS_COLS || row == 6) return FALSE;

   if (row == 0 && col == 0) return FALSE;
   if (row == 5 && col == 15) return FALSE;
   if (row == 12 && col == 0) return FALSE;
   if (row == 12 && col == 15) return FALSE;

   return TRUE;
}

/* --------------------- Focus Stuff --------------------- */

static
void InsertSymbolDlgRemoveFocus(pTdgtBase)
   TdgtBase *pTdgtBase;
{
   InsertSymbolDlgInfo *pisdi=(InsertSymbolDlgInfo*)(pTdgtBase->pti->userdata);
   Window win=TidgetGetFocusWindow();

   if (win == None) return;
   if (win == pisdi->sym_pixmap_ctl->pti->tci.win) {
      TidgetDrawFocusRect(pisdi->sym_pixmap_ctl->pti,
            &pisdi->sym_pixmap_ctl->client_area);
   }
}

/* --------------------- HSRedrawCallback --------------------- */

static
void SetupControls(pTdgtBase)
   TdgtBase *pTdgtBase;
{
}

static
void RedrawInsertSymbolPixmapControlValues(pTdgtBase)
   TdgtBase *pTdgtBase;
{
   InsertSymbolDlgInfo *pisdi=(InsertSymbolDlgInfo*)(pTdgtBase->pti->userdata);

   RedrawTidget(pisdi->sym_pixmap_ctl->pti);
}

#define SKIP_NONE      0x0
#define SKIP_HS_PIXMAP 0x1
#define SKIP_V_PIXMAP  0x2

static
void RedrawControls(pTdgtBase, skip)
   TdgtBase *pTdgtBase;
   int skip;
{
   if ((skip & SKIP_HS_PIXMAP) != SKIP_HS_PIXMAP) {
      RedrawInsertSymbolPixmapControlValues(pTdgtBase);
   }
}

static
int HSRedrawCallback(pTdgtDraw)
   TdgtDraw *pTdgtDraw;
{
   TdgtBase *pTdgtBase=NULL;

   if (pTdgtDraw == NULL) return FALSE;
   pTdgtBase = (TdgtBase*)(pTdgtDraw->pti->tci.parent_tidgetinfo->tidget);

   XCopyArea(mainDisplay, gInsertSymbolInfo.sym_pixmap, pTdgtDraw->pti->tci.win,
         gTidgetManager.gc, 0, 0, pTdgtDraw->client_area.w,
         pTdgtDraw->client_area.h, pTdgtDraw->client_area.x,
         pTdgtDraw->client_area.y);
   if (gInsertSymbolInfo.something_selected) {
      InsertSymbolToggleSelected(pTdgtDraw);
   }
   if (TidgetGetFocusWindow() == pTdgtDraw->pti->tci.win) {
      TidgetDrawFocusRect(pTdgtDraw->pti, &pTdgtDraw->client_area);
   }
   TidgetManagerResetGC();

   return TRUE;
}

/* --------------------- HSEvHandlerCallback() --------------------- */

static
int GetRowCol(x, y, pn_row, pn_col)
   int x, y, *pn_row, *pn_col;
{
   int row=0, col=0;

   x -= (windowPadding+TDGTBASE_DEF_H_PAD-1);
   y -= (windowPadding+TDGTBASE_DEF_V_PAD-1);

   if (x < 0 || y < 0) return FALSE;

   row = (int)(y / (gInsertSymbolInfo.a_symbol_h+1));
   col = (int)(x / (gInsertSymbolInfo.a_symbol_w+1));

   if (!InsertSymbolSelectedValid(row, col)) {
      return FALSE;
   }
   *pn_row = row;
   *pn_col = col;

   return TRUE;
}

static
int ClickInInsertSymbolPixmap(pTdgtDraw, button_ev)
   TdgtDraw *pTdgtDraw;
   XButtonEvent *button_ev;
{
   TdgtBase *pTdgtBase=
         (TdgtBase*)(pTdgtDraw->pti->tci.parent_tidgetinfo->tidget);
   InsertSymbolDlgInfo *pisdi=(InsertSymbolDlgInfo*)(pTdgtBase->pti->userdata);
   int x=button_ev->x, y=button_ev->y;
   int on_a_symbol=FALSE, row=0, col=0, changed=FALSE;
   int double_clicked=FALSE;
   Time click_time=button_ev->time;

   if (TidgetGetFocusWindow() == pTdgtDraw->pti->tci.win) {
      /* no need to change focus */
   } else {
      /* change focus */
      TidgetSendCmd(pTdgtBase->pti, TDGTCMD_REMOVE_FOCUS, 0, NULL);
      TidgetSetFocusWindow(pTdgtDraw->pti->tci.win);
   }
   on_a_symbol = GetRowCol(x, y, &row, &col);
   if (gInsertSymbolInfo.something_selected) {
      if (on_a_symbol) {
         if (row == gInsertSymbolInfo.selected_row &&
               col == gInsertSymbolInfo.selected_col) {
            /* clicked on the same symbol */
            double_clicked = ((click_time-gInsertSymbolInfo.last_click_time) <
                  doubleClickInterval);
         } else {
            InsertSymbolToggleSelected(pisdi->sym_pixmap_ctl);
            gInsertSymbolInfo.selected_row = row;
            gInsertSymbolInfo.selected_col = col;
            changed = TRUE;
         }
      } else {
         gInsertSymbolInfo.something_selected = FALSE;
         changed = TRUE;
      }
   } else {
      if (on_a_symbol) {
         gInsertSymbolInfo.something_selected = TRUE;
         gInsertSymbolInfo.selected_row = row;
         gInsertSymbolInfo.selected_col = col;
         changed = TRUE;
      }
   }
   if (changed) {
      SetupControls(pTdgtBase);
      RedrawControls(pTdgtBase, SKIP_NONE);
   } else if (double_clicked && gInsertSymbolInfo.something_selected) {
      InsertSymbolInAppl(pTdgtBase);
   }
   gInsertSymbolInfo.last_click_time = click_time;

   return FALSE;
}

static
int HSEvHandlerCallback(pTdgtDraw, pXEv)
   TdgtDraw *pTdgtDraw;
   XEvent *pXEv;
{
   if (pXEv->xany.window == pTdgtDraw->pti->tci.win) {
      if (pXEv->type == ButtonPress) {
         XButtonEvent *button_ev=(&(pXEv->xbutton));

         return ClickInInsertSymbolPixmap(pTdgtDraw, button_ev);
      }
   }
   return FALSE;
}

/* --------------------- HSSendCmdCallback() --------------------- */

static
int HSSendCmdCallback(pTdgtDraw, cmd_type, cmd_arg, pv_cmd_userdata)
   TdgtDraw *pTdgtDraw;
   int cmd_type, cmd_arg;
   void *pv_cmd_userdata;
{
   TdgtBase *pTdgtBase=NULL;
   InsertSymbolDlgInfo *pisdi=NULL;

   switch (cmd_type) {
   case TDGTCMD_DRAW_USER:
      switch (cmd_arg) {
      case TDGTARG_INSSYM_DPY_CLEAR:
         pTdgtBase = (TdgtBase*)pv_cmd_userdata;
         pisdi = (InsertSymbolDlgInfo*)(pTdgtBase->pti->userdata);
         /* clear it */
         gInsertSymbolInfo.something_selected = FALSE;
         RedrawTidget(pisdi->sym_pixmap_ctl->pti);
         break;
      }
      break;
   }
   return FALSE;
}

/* --------------- KeyPress Event Handlers --------------- */

static
int HandleCRorLF(pTdgtBase)
   TdgtBase *pTdgtBase;
{
   if (gInsertSymbolInfo.something_selected) {
      InsertSymbolInAppl(pTdgtBase);
   }
   return FALSE;
}

static
int HandleTab(pTdgtBase)
   TdgtBase *pTdgtBase;
{
   InsertSymbolDlgInfo *pisdi=(InsertSymbolDlgInfo*)(pTdgtBase->pti->userdata);
   Window win=TidgetGetFocusWindow();

   if (win == None) {
      TidgetSetFocusWindow(pisdi->sym_pixmap_ctl->pti->tci.win);
      return FALSE;
   }
   return FALSE;
}

static
int HandleBackSpace(pTdgtBase)
   TdgtBase *pTdgtBase;
{
   return FALSE;
}

static
int HandleCursorKey(pTdgtBase, key_sym)
   TdgtBase *pTdgtBase;
   KeySym key_sym;
{
   InsertSymbolDlgInfo *pisdi=(InsertSymbolDlgInfo*)(pTdgtBase->pti->userdata);
   Window win=TidgetGetFocusWindow();
   int dir=(-1);

   if (win == None) return FALSE;

   switch (key_sym) {
   case XK_Up: dir = SCROLL_UP; break;
   case XK_KP_Up: dir = SCROLL_UP; break;
   case XK_Down: dir = SCROLL_DOWN; break;
   case XK_KP_Down: dir = SCROLL_DOWN; break;
   case XK_Left: dir = SCROLL_LEFT; break;
   case XK_KP_Left: dir = SCROLL_LEFT; break;
   case XK_Right: dir = SCROLL_RIGHT; break;
   case XK_KP_Right: dir = SCROLL_RIGHT; break;
   default: return FALSE;
   }
   if (gInsertSymbolInfo.something_selected) {
      InsertSymbolToggleSelected(pisdi->sym_pixmap_ctl);

      switch (dir) {
      case SCROLL_UP:
         gInsertSymbolInfo.selected_row--;
         if (gInsertSymbolInfo.selected_row < 0) {
            gInsertSymbolInfo.selected_row = TGIS_ROWS-1;
         }
         break;
      case SCROLL_DOWN:
         gInsertSymbolInfo.selected_row++;
         if (gInsertSymbolInfo.selected_row >= TGIS_ROWS) {
            gInsertSymbolInfo.selected_row = 0;
         }
         break;
      case SCROLL_LEFT:
         gInsertSymbolInfo.selected_col--;
         if (gInsertSymbolInfo.selected_col < 0) {
            gInsertSymbolInfo.selected_col = TGIS_COLS-1;
         }
         break;
      case SCROLL_RIGHT:
         gInsertSymbolInfo.selected_col++;
         if (gInsertSymbolInfo.selected_col >= TGIS_COLS) {
            gInsertSymbolInfo.selected_col = 0;
         }
         break;
      }
   } else {
      switch (dir) {
      case SCROLL_UP:
         gInsertSymbolInfo.selected_row = TGIS_ROWS-1;
         gInsertSymbolInfo.selected_col = TGIS_COLS-1;
         break;
      case SCROLL_DOWN:
         gInsertSymbolInfo.selected_row = 0;
         gInsertSymbolInfo.selected_col = 0;
         break;
      case SCROLL_LEFT:
         gInsertSymbolInfo.selected_row = TGIS_ROWS-1;
         gInsertSymbolInfo.selected_col = TGIS_COLS-1;
         break;
      case SCROLL_RIGHT:
         gInsertSymbolInfo.selected_row = 0;
         gInsertSymbolInfo.selected_col = 0;
         break;
      }
      gInsertSymbolInfo.something_selected = TRUE;
   }
   while (!InsertSymbolSelectedValid(gInsertSymbolInfo.selected_row,
         gInsertSymbolInfo.selected_col)) {
      switch (dir) {
      case SCROLL_UP:
         gInsertSymbolInfo.selected_row--;
         if (gInsertSymbolInfo.selected_row < 0) {
            gInsertSymbolInfo.selected_row = TGIS_ROWS-1;
         }
         break;
      case SCROLL_DOWN:
         gInsertSymbolInfo.selected_row++;
         if (gInsertSymbolInfo.selected_row >= TGIS_ROWS) {
            gInsertSymbolInfo.selected_row = 0;
         }
         break;
      case SCROLL_LEFT:
         gInsertSymbolInfo.selected_col--;
         if (gInsertSymbolInfo.selected_col < 0) {
            gInsertSymbolInfo.selected_col = TGIS_COLS-1;
         }
         break;
      case SCROLL_RIGHT:
         gInsertSymbolInfo.selected_col++;
         if (gInsertSymbolInfo.selected_col >= TGIS_COLS) {
            gInsertSymbolInfo.selected_col = 0;
         }
         break;
      }
   }
   SetupControls(pTdgtBase);
   RedrawControls(pTdgtBase, SKIP_NONE);

   return FALSE;
}

static
int HandleChar(pTdgtBase, key_sym, buf, has_ch)
   TdgtBase *pTdgtBase;
   KeySym key_sym;
   char *buf;
   int has_ch;
{
   return FALSE;
}

/* --------------- TdgtInsertSymbolDlgKeyPressEvHandlerCallback --------------- */

static XComposeStatus c_stat;

static
int TdgtInsertSymbolDlgKeyPressEvHandlerCallback(pTdgtBase, pXEv)
   TdgtBase *pTdgtBase;
   XEvent *pXEv;
{
   InsertSymbolDlgInfo *pisdi=(InsertSymbolDlgInfo*)(pTdgtBase->pti->userdata);
   int has_ch=0;
   char buf[80];
   KeySym key_sym=(KeySym)0;
   XKeyEvent *key_ev=NULL;

   if (pXEv->type != KeyPress) return FALSE;
   if (pisdi == NULL) return FALSE;

   key_ev = (&(pXEv->xkey));
   has_ch = XLookupString(key_ev, buf, sizeof(buf), &key_sym, &c_stat);
   TranslateKeys(buf, &key_sym);

   if (CharIsESC(key_ev, buf, key_sym, &has_ch)) {
      HideTdgtInsertSymbolDialogBox();
   } else if (CharIsCRorLF(key_ev, buf, key_sym, &has_ch)) {
      return HandleCRorLF(pTdgtBase);
   } else if (CharIsTAB(key_ev, buf, key_sym, &has_ch)) {
      return HandleTab(pTdgtBase);
   } else if (CharIsCntrlSpace(key_ev, buf, key_sym, &has_ch)) {
      HideTdgtInsertSymbolDialogBox();
   } else if (CharIsBS(key_ev, buf, key_sym, &has_ch, FALSE)) {
      return HandleBackSpace(pTdgtBase);
   } else if (key_sym==XK_Up || key_sym==XK_KP_Up || key_sym==XK_Down ||
         key_sym==XK_KP_Down || key_sym==XK_Left || key_sym==XK_KP_Left ||
         key_sym==XK_Right || key_sym==XK_KP_Right ||
         (key_sym>'\040' && key_sym<='\177' &&
         (key_ev->state & ControlMask) == ControlMask)) {
      return HandleCursorKey(pTdgtBase, key_sym);
   } else if (key_sym>='\040' && key_sym<='\177') {
      return HandleChar(pTdgtBase, key_sym, buf, has_ch);
   }
   return FALSE;
}

/* --------------------- ShowTdgtInsertSymbolDialogBox --------------------- */

void ShowTdgtInsertSymbolDialogBox()
{
   TidgetInfo *pti=gInsertSymbolInfo.dialogbox_tidgetinfo;
   InsertSymbolDlgInfo *pisdi=(InsertSymbolDlgInfo*)(pti->userdata);

   TidgetManagerSetWantAllKeyPressEvents(pti, TRUE);
   gInsertSymbolInfo.mapped = TRUE;
   MapTidget(pti);

   if (pisdi != NULL) {
      ClearAllControls((TdgtBase*)(pti->tidget));
      SetupControls((TdgtBase*)(pti->tidget));
   }
}

/* --------------------- HideTdgtInsertSymbolDialogBox --------------------- */

void HideTdgtInsertSymbolDialogBox()
{
   TidgetManagerSetWantAllKeyPressEvents(NULL, FALSE);
   TidgetSetHasFocus(FALSE);
   gInsertSymbolInfo.mapped = FALSE;
   XUnmapWindow(mainDisplay, gInsertSymbolInfo.dialogbox_tidgetinfo->tci.win);

   if (warpToWinCenter) {
      XWarpPointer(mainDisplay, None, drawWindow, 0, 0, 0, 0,
            (int)(ZOOMED_SIZE(drawWinW)>>1), (int)(ZOOMED_SIZE(drawWinH)>>1));
   }
}

/* --------------------- CreateTdgtInsertSymbolDialogBox --------------------- */

static
int TdgtInsertSymbolDlgSetEntries(pti, dsp_ptr, entries, num_entries, marked_index)
   TidgetInfo *pti;
   DspList *dsp_ptr;
   char **entries;
   int num_entries, marked_index;
{
   return TRUE;
}

static
int TdgtInsertSymbolDlgSetMsg(pti, msg)
   TidgetInfo *pti;
   char *msg;
{
   return TRUE;
}

static
void InitTdgtInsertSymbolDlgInfo(pti)
   TidgetInfo *pti;
{
   InsertSymbolDlgInfo *pisdi=NULL;

   if (pti == NULL) return;

   pisdi = (InsertSymbolDlgInfo*)(pti->userdata);
   if (pisdi == NULL) return;

   pisdi->screen_w = DisplayWidth(mainDisplay, mainScreen);
   pisdi->screen_h = DisplayHeight(mainDisplay, mainScreen);
   TdgtInsertSymbolDlgSetMsg(pti, "");

   pisdi->def_btn_id = ID_INSSYM_BTN_OK;
}

typedef struct tagCreateBtnInfo {
   char *text;
   int ctl_id;
} CreateBtnInfo;

static CreateBtnInfo cbi[] = {
   { "Insert Symbol",     ID_INSSYM_BTN_ADD    },
   { "Insert and Close",  ID_INSSYM_BTN_OK     },
   { "Close",             ID_INSSYM_BTN_CANCEL },
   { NULL, 0 }
};

static
int CreateInsertSymbolButtons(pTdgtBase)
   TdgtBase *pTdgtBase;
{
   InsertSymbolDlgInfo *pisdi=(InsertSymbolDlgInfo*)(pTdgtBase->pti->userdata);
   TdgtBtnRow *pTdgtBtnRow=pisdi->btn_row_ctl;
   int i=0;

   for (i=0; cbi[i].text != NULL; i++) {
      TdgtBtn *pTdgtBtn=NULL;
      int btn_w=0, btn_h=0;

      CalcTdgtBtnDim(cbi[i].text, 8, TDGTBTN_DEF_H_PAD, TDGTBTN_DEF_V_PAD,
            &btn_w, &btn_h);
      pTdgtBtn = CreateTdgtBtn(pTdgtBtnRow->pti->tci.win,
            pTdgtBtnRow->pti, cbi[i].ctl_id, 0, 0, btn_w, btn_h,
            TDGTBTN_DEF_H_PAD, TDGTBTN_DEF_V_PAD, TGMUTYPE_TEXT, TDGTBTN_CLICK,
            TGBS_NORMAL, STYLE_NR, cbi[i].text, NULL);
      if (pTdgtBtn == NULL) {
         DestroyTidget(&pTdgtBase->pti);
         return FALSE;
      }
      if (!TdgtBtnRowAddBtn(pTdgtBtnRow, pTdgtBtn)) {
         DestroyTidget(&pTdgtBase->pti);
         return FALSE;
      }
   }
   return TRUE;
}

static
void AdjustDialogBoxWidth(dpy, pTdgtBase)
   Display *dpy;
   TdgtBase *pTdgtBase;
{
   InsertSymbolDlgInfo *pisdi=(InsertSymbolDlgInfo*)(pTdgtBase->pti->userdata);
   int max_w=0, bottom=0, h_pad=pTdgtBase->pti->tci.h_pad, cur_x=0, cur_y=0;
   int v_pad=pTdgtBase->pti->tci.v_pad, end_y=0;
   SimpleWinInfo *pswi=NULL;

   /* hue edit */
   pswi = (&(pisdi->sym_pixmap_ctl->pti->tci.win_info));
   max_w = pswi->w;

   bottom = pisdi->btn_row_ctl->pti->tci.win_info.y +
         pisdi->btn_row_ctl->pti->tci.win_info.h;

   cur_x = windowPadding + h_pad;
   cur_y = windowPadding + v_pad;

   /* sym_pixmap */
   pswi = (&(pisdi->sym_pixmap_ctl->pti->tci.win_info));
   end_y = pswi->y+pswi->h;

   /* button row */
   pswi = (&(pisdi->btn_row_ctl->pti->tci.win_info));
   pswi->w = max_w;
   TidgetMoveResize(pisdi->btn_row_ctl->pti, pswi->x, pswi->y, pswi->w,
         pswi->h);

   pswi = (&(pTdgtBase->pti->tci.win_info));
   pswi->w = max_w+(windowPadding<<1)+(h_pad<<1);
   pswi->h = bottom+windowPadding+v_pad;
   TidgetMoveResize(pTdgtBase->pti, pswi->x, pswi->y, pswi->w, pswi->h);
}

static
void TdgtInsertSymbolDlgWmDeleteCallback(pTdgtBase)
   TdgtBase *pTdgtBase;
{
   InsertSymbolDlgInfo *pisdi=(InsertSymbolDlgInfo*)(pTdgtBase->pti->userdata);

   if (pisdi == NULL) return;

   HideTdgtInsertSymbolDialogBox();
}

static
int TdgtInsertSymbolDlgFocusCallback(pTdgtBase, cmd_type, cmd_arg,
      pv_cmd_userdata)
   TdgtBase *pTdgtBase;
   int cmd_type, cmd_arg;
   void *pv_cmd_userdata;
{
   InsertSymbolDlgInfo *pisdi=(InsertSymbolDlgInfo*)(pTdgtBase->pti->userdata);

   if (pisdi == NULL) return FALSE;

   switch (cmd_type) {
   case TDGTCMD_CAN_HANDLE_FOCUS_CHANGE: return TRUE;
   case TDGTCMD_REMOVE_FOCUS: InsertSymbolDlgRemoveFocus(pTdgtBase); return TRUE;
   }
   return FALSE;
}

static
void AdjustDialogBoxSize(dpy, pTdgtBase)
   Display *dpy;
   TdgtBase *pTdgtBase;
{
   InsertSymbolDlgInfo *pisdi=(InsertSymbolDlgInfo*)(pTdgtBase->pti->userdata);
   int base_win_x=0, base_win_y=0, base_win_w=0, base_win_h=0;
   XWMHints wmhints;
   XSizeHints sizehints;
   
   if (pTdgtBase == NULL) return;

   AdjustDialogBoxWidth(dpy, pTdgtBase);

   base_win_w = pTdgtBase->pti->tci.win_info.w;
   base_win_h = pTdgtBase->pti->tci.win_info.h;

   memset(&wmhints, 0, sizeof(XWMHints));
   wmhints.flags = InputHint | StateHint;
   wmhints.input = True;
   wmhints.initial_state = NormalState;
   XSetWMHints(dpy, pTdgtBase->pti->tci.win, &wmhints);
   
   base_win_x = (base_win_w > pisdi->screen_w) ? 0 :
         ((pisdi->screen_w-base_win_w)>>1);
   base_win_y = (base_win_h > pisdi->screen_h) ? 0 :
         ((pisdi->screen_h-base_win_h)/3);

   XMoveResizeWindow(dpy, pTdgtBase->pti->tci.win, base_win_x,
         base_win_y, base_win_w, base_win_h);

   sizehints.flags = PPosition | PSize | USPosition | PMinSize | PMaxSize;
   sizehints.x = base_win_x;
   sizehints.y = base_win_y;
   sizehints.width = sizehints.min_width = sizehints.max_width = base_win_w;
   sizehints.height = sizehints.min_height = sizehints.max_height = base_win_h;
#ifdef NOTR4MODE
   XSetNormalHints(dpy, pTdgtBase->pti->tci.win, &sizehints);
#else
   XSetWMNormalHints(dpy, pTdgtBase->pti->tci.win, &sizehints);
#endif

   XSetTransientForHint(dpy, pTdgtBase->pti->tci.win, mainWindow);
}

static
int SetupInsertSymbolPixmap(pisdi)
   InsertSymbolDlgInfo *pisdi;
{
   int row=0, y=0;
   char ch=(char)0x20;
   XGCValues values;
   Pixmap pixmap=gInsertSymbolInfo.sym_pixmap;
   GC gc=gInsertSymbolInfo.gc;
   int padding=gInsertSymbolInfo.padding;
   int asc=gInsertSymbolInfo.font_asc;

   if (pixmap == None) return FALSE;

   values.foreground = myBgPixel;
   XChangeGC(mainDisplay, gc, GCForeground, &values);

   XFillRectangle(mainDisplay, pixmap, gc, 0, 0, gInsertSymbolInfo.pixmap_w,
         gInsertSymbolInfo.pixmap_h);

   values.foreground = myFgPixel;
   XChangeGC(mainDisplay, gc, GCForeground, &values);

   for (row=0; row < TGIS_ROWS; row++) {
      int col=0, x=0;

      if (row == 6) {
         ch = (char)0xa0;
      } else {
         for (col=0; col < TGIS_COLS; col++) {
            XDrawRectangle(mainDisplay, pixmap, gc, x, y,
                  gInsertSymbolInfo.a_symbol_w+1,
                  gInsertSymbolInfo.a_symbol_h+1);
            XDrawString(mainDisplay, pixmap, gc, x+padding, y+padding+asc,
                  &ch, 1);
            x += gInsertSymbolInfo.a_symbol_w+1;
            ch++;
         }
      }
      y += gInsertSymbolInfo.a_symbol_h+1;
   }
   return TRUE;
}

TidgetInfo *CreateTdgtInsertSymbolDialogBox(dpy, parent_win)
   Display *dpy;
   Window parent_win; /* should be the rootWindow */
{
   int cur_x=0, cur_y=0, v_gap=20, h_pad=0, v_pad=0;
   int vh_padding=0, h_gap=14, end_y=0, rgbhsv_width=0;
   TdgtBase *pTdgtBase=NULL;
   InsertSymbolDlgInfo *pisdi=NULL;
   char caption[MAXSTRING];

   TidgetSetFocusWindow(None);
   if (!colorDisplay || mainVisual->class != TrueColor) {
      sprintf(gszMsgBox, TgLoadString(STID_COLOR_WHEEL_NOT_AVAIL));
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      free(pisdi);
      return NULL;
   }
   pisdi = (InsertSymbolDlgInfo*)malloc(sizeof(InsertSymbolDlgInfo));
   if (pisdi == NULL) FailAllocMessage();
   memset(pisdi, 0, sizeof(InsertSymbolDlgInfo));

   memset(&pisdi->tci, 0, sizeof(TrueColorInfo));
   if (!SetupInsertSymbolPixmap(pisdi)) {
      free(pisdi);
      return NULL;
   }
   if (boldMsgFontSet != NULL || boldMsgFontPtr != NULL) {
       rgbhsv_width = BoldMsgTextWidth(boldMsgFontPtr, "9999", 4);
   } else if (msgFontSet != NULL || msgFontPtr != NULL) {
       rgbhsv_width = MsgTextWidth(msgFontPtr, "9999", 4);
   } else {
       rgbhsv_width = defaultFontWidth * 4;
   }
   h_pad = TDGTBASE_DEF_H_PAD;
   v_pad = TDGTBASE_DEF_V_PAD;
   if (msgFontSet != NULL || msgFontPtr != NULL) {
      vh_padding = (msgFontWidth<<1);
   } else {
      vh_padding = (defaultFontWidth<<1);
   }
   cur_x = vh_padding+windowPadding;
   cur_y = vh_padding+windowPadding;

   sprintf(caption, TgLoadString(STID_TOOL_COLOR_WHEEL), TOOL_NAME);
   pTdgtBase = CreateTdgtBase(parent_win, NULL, ID_INSSYM_DIALOG, 0, 0, 100,
         100, vh_padding, vh_padding, (dialogboxUse3DBorder ? TGBS_RAISED :
         TGBS_NORMAL), caption);
   if (pTdgtBase == NULL) {
      free(pisdi);
      return NULL;
   }
   pTdgtBase->pti->userdata = pisdi;
   InitTdgtInsertSymbolDlgInfo(pTdgtBase->pti);

   /* sym_pixmap */
   pisdi->sym_pixmap_x = cur_x;
   pisdi->sym_pixmap_y = cur_y;
   pisdi->sym_pixmap_ctl = CreateTdgtDraw(pTdgtBase->pti->tci.win,
         pTdgtBase->pti, ID_INSSYM_SYM_AREA, cur_x, cur_y,
         gInsertSymbolInfo.pixmap_w, gInsertSymbolInfo.pixmap_h,
         TDGTDRAW_DEF_H_PAD, TDGTDRAW_DEF_V_PAD, TGBS_LOWRED, NULL);
   if (pisdi->sym_pixmap_ctl == NULL) {
      DestroyTidget(&pTdgtBase->pti);
      return NULL;
   }
   TdgtDrawSetRedrawCallback(pisdi->sym_pixmap_ctl, HSRedrawCallback);
   TdgtDrawSetEvHandlerCallback(pisdi->sym_pixmap_ctl, HSEvHandlerCallback);
   TdgtDrawSetSendCmdCallback(pisdi->sym_pixmap_ctl, HSSendCmdCallback);

   cur_x += pisdi->sym_pixmap_ctl->pti->tci.win_info.w + h_gap;
   end_y = cur_y + pisdi->sym_pixmap_ctl->pti->tci.win_info.h - windowPadding;

   /* buttons */

   cur_x = vh_padding+windowPadding;
   cur_y = end_y + v_gap;
   pisdi->btn_row_ctl = CreateTdgtBtnRow(pTdgtBase->pti->tci.win,
         pTdgtBase->pti, INVALID, cur_x, cur_y, TDGTBROW_DEF_H_PAD,
         TDGTBROW_DEF_V_PAD, TGBS_NORMAL, defaultFontWidth, JUST_C);
   if (pisdi->btn_row_ctl == NULL) {
      DestroyTidget(&pTdgtBase->pti);
      return NULL;
   }
   if (!CreateInsertSymbolButtons(pTdgtBase)) {
      DestroyTidget(&pTdgtBase->pti);
      return NULL;
   }
   TdgtInsertSymbolDlgSetEntries(pTdgtBase->pti, NULL, NULL, 0, INVALID);

   /*
    * Specify that the base tidget will handle all the KeyPress events
    *         in all its children tidgets.
    */
   TdgtBaseWillHandleKeyPressEvents(pTdgtBase,
         TdgtInsertSymbolDlgKeyPressEvHandlerCallback);
   /*
    * Specify that the base tidget will handle all the KeyPress events
    *         in all its children tidgets.
    */
   TdgtBaseSetWmDeleteCallback(pTdgtBase, TdgtInsertSymbolDlgWmDeleteCallback);

   /*
    * Specify a callback function to answer some focus related questions.
    */
   TdgtBaseSetFocusCallback(pTdgtBase, TdgtInsertSymbolDlgFocusCallback);

   AdjustDialogBoxSize(dpy, pTdgtBase);
   TidgetSetFocusWindow(pisdi->sym_pixmap_ctl->pti->tci.win);

   return pTdgtBase->pti;
}

/* --------------------- DestroyTdgtInsertSymbolDialogBox --------------------- */

void DestroyTdgtInsertSymbolDialogBox(dpy, pti)
   Display *dpy;
   TidgetInfo *pti;
{
   InsertSymbolDlgInfo *pisdi=NULL;

   if (pti == NULL) return;

   pisdi = (InsertSymbolDlgInfo*)(pti->userdata);
   if (pisdi != NULL) {
      free(pisdi);
   }
   DestroyTidget(&pti);
}

/* --------------------- TdgtInsertSymbolDlgLoop --------------------- */

static
int HandleTdgtNotifyEvent(pTdgtBase, pTdgtNtfy)
   TdgtBase *pTdgtBase;
   TdgtNtfy *pTdgtNtfy;
{
   switch (pTdgtNtfy->ctl_id) {
   case ID_INSSYM_BTN_ADD: InsertSymbolInAppl(pTdgtBase); break;
   case ID_INSSYM_BTN_OK:
      if (InsertSymbolInAppl(pTdgtBase)) {
         HideTdgtInsertSymbolDialogBox();
      }
      break;
   case ID_INSSYM_BTN_CANCEL: HideTdgtInsertSymbolDialogBox(); break;
   }
   return FALSE;
}

int TdgtInsertSymbolDlgLoop(pti)
   TidgetInfo *pti;
{
   TdgtBase *pTdgtBase=(TdgtBase*)(pti->tidget);
   int which_btn_clicked=INVALID, first_time_config=TRUE;

   if (pTdgtBase == NULL) return which_btn_clicked;

   while (gInsertSymbolInfo.mapped) {
      XEvent xev;
      TdgtNtfy tdgt_notify;
      TidgetInfo *handling_pti=NULL;

      XNextEvent(mainDisplay, &xev);

      if (xev.type == MotionNotify) {
         /* do nothing */
      } else if (xev.type == ConfigureNotify &&
            xev.xany.window == pti->tci.win) {
         if (first_time_config) {
            first_time_config = FALSE;
            if (warpToWinCenter) {
               /* does not work because the window has been moved */
               XWarpPointer(mainDisplay, None, pti->tci.win, 0, 0, 0, 0,
                     (pti->tci.win_info.w>>1), (pti->tci.win_info.h>>1));
            }
         }
      } else if (xev.type == Expose) {
         ExposeEventHandler(&xev, FALSE);
      } else if (xev.type == ConfigureNotify &&
            xev.xany.window==mainWindow) {
         Reconfigure(FALSE);
      } else if (xev.type == KeyPress) {
         TidgetManagerHandleAllKeyPressEvent(&xev);
      } else if (IsTdgtNotifyEvent(pti, &xev, &tdgt_notify)) {
         HandleTdgtNotifyEvent(pTdgtBase, &tdgt_notify);
      } else if (IsTidgetEvent(pti, &xev, &handling_pti)) {
         TidgetEventHandler(pti, &xev, handling_pti);
      } else {
         /* anything else? */
      }
   }
   return which_btn_clicked;
}

/* --------------------- Init & Clean Up --------------------- */

int InitTdgtInsertSymbolDlg()
{
   return TRUE;
}

void CleanUpTdgtInsertSymbolDlg()
{
}

