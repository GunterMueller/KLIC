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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/tdgtlist.c,v 1.8 2011/05/16 16:22:00 william Exp $
 */

#define _INCLUDE_FROM_TDGTLIST_C_

#include "tgifdefs.h"
#include "patchlvl.h"

#include "auxtext.e"
#include "choose.e"
#include "color.e"
#include "cursor.e"
#include "dialog.e"
#include "drawing.e"
#include "font.e"
#include "mainloop.e"
#include "menu.e"
#include "msg.e"
#include "pattern.e"
#include "raster.e"
#include "rect.e"
#include "scroll.e"
#include "setup.e"
#include "strtbl.e"
#include "tdgtlist.e"
#include "tidget.e"
#include "util.e"

static void RedrawTdgtList ARGS_DECL((TidgetInfo *pti));
static int TdgtListEventHandler ARGS_DECL((TidgetInfo *pti, XEvent *input,
                TidgetInfo *handling_pti));
static int IsTdgtListEvent ARGS_DECL((TidgetInfo *pti, XEvent *input,
                TidgetInfo **ppti_handler_tidget_return));
static void DestroyTdgtList ARGS_DECL((TidgetInfo *pti));
static void MapTdgtList ARGS_DECL((TidgetInfo *pti));
static void TdgtListMoveResize ARGS_DECL((TidgetInfo *pti, int x, int y,
                int w, int h));
static int TdgtListSendCmd ARGS_DECL((TidgetInfo *pti, int cmd_type,
                int cmd_arg, void *pv_cmd_userdata));

static int gnListFontHeight=0;
static int gnListFontWidth=0;
static int gnListFontAsc=0;
static int gnListFontDes=0;
static XFontStruct *gnListFontPtr=NULL;

/* --------------------- Utility Functions --------------------- */

static
void FreeListItemInfo(pListItemInfo)
   ListItemInfo *pListItemInfo;
{
   UtilFree(pListItemInfo->buf);
   free(pListItemInfo);
}

/* --------------------- RedrawTdgtList() --------------------- */

static
void RedrawTdgtListItem(pTdgtList, index, pListItemInfo)
   TdgtList *pTdgtList;
   int index;
   ListItemInfo *pListItemInfo;
{
   ListInfo *pListInfo=(&pTdgtList->list_info);
   int len=0, top=0, end=0, checkbox_cols=0, text_left=0, box_offset=0, box_w=0;
   int selected=(index==pListInfo->marked_index);
   int length=ListLength(&pListInfo->list);
   XGCValues values;

   if (!pTdgtList->can_select) selected = FALSE;
   top = gnListFontAsc+1;
   if (pListInfo->first_index+pListInfo->num_visible_lines > length) {
      end = length;
   } else {
      end = pListInfo->first_index + pListInfo->num_visible_lines;
   }
   if (pListInfo->p_check_array != NULL &&
         pListInfo->p_check_array->num_cols > 0) {
      checkbox_cols = pListInfo->p_check_array->num_cols;
      text_left = checkbox_cols*ROW_HEIGHT;
      box_offset = 1;
      box_w = (ROW_HEIGHT-((box_offset+1)<<1));
   }
   if (pTdgtList->multicolor) {
      if (selected) {
      } else {
         char *buf=NULL;
         XFontStruct *font_ptr=NULL;
         int font_asc=0;
         TidgetDrawMsgStringFunc *pf_draw_string_func=NULL;

         len = strlen(pListItemInfo->nick_name)+strlen(pListItemInfo->buf)+8;
         buf = (char*)malloc(len*sizeof(char));
         if (buf == NULL) FailAllocMessage();
         if (*pListItemInfo->nick_name == '\0') {
            sprintf(buf, "%s", pListItemInfo->buf);
         } else {
            sprintf(buf, "%s : %s", pListItemInfo->nick_name,
                  pListItemInfo->buf);
         }
         TidgetGetFontInfoGivenStyle(pListItemInfo->font_style, &font_ptr,
               NULL, NULL, &font_asc, NULL);

         values.foreground = myBgPixel;
         values.background = myFgPixel;
         values.function = GXcopy;
         values.fill_style = FillSolid;
         XChangeGC(mainDisplay, gTidgetManager.gc,
               GCForeground | GCBackground | GCFunction | GCFillStyle, &values);
         XFillRectangle(mainDisplay, pTdgtList->dsp_win,
               revNameGC, 0,
               (index-pListInfo->first_index)*ROW_HEIGHT,
               ITEM_LEN*defaultFontWidth, ROW_HEIGHT);

         switch (pListItemInfo->font_style) {
         case STYLE_NR:
            if (msgFontPtr != NULL) {
               XSetFont(mainDisplay, gTidgetManager.gc, msgFontPtr->fid);
            }
            pf_draw_string_func = DrawMsgString;
            break;
         case STYLE_BR:
            if (boldMsgFontPtr != NULL) {
               XSetFont(mainDisplay, gTidgetManager.gc, boldMsgFontPtr->fid);
            }
            pf_draw_string_func = DrawBoldMsgString;
            break;
         case STYLE_NI:
            if (italicMsgFontPtr != NULL) {
               XSetFont(mainDisplay, gTidgetManager.gc, italicMsgFontPtr->fid);
            }
            pf_draw_string_func = DrawItalicMsgString;
            break;
         case STYLE_BI:
            if (boldItalicMsgFontPtr != NULL) {
               XSetFont(mainDisplay, gTidgetManager.gc,
                     boldItalicMsgFontPtr->fid);
            }
            pf_draw_string_func = DrawBoldItalicMsgString;
            break;
         }
         values.foreground = ((pListItemInfo->color_index == INVALID) ?
               myFgPixel : colorPixels[pListItemInfo->color_index]);
         values.background = myBgPixel;
         values.function = GXcopy;
         values.fill_style = FillSolid;
         XChangeGC(mainDisplay, gTidgetManager.gc,
               GCForeground |GCBackground |  GCFunction | GCFillStyle, &values);

         (pf_draw_string_func)(mainDisplay, pTdgtList->dsp_win,
               gTidgetManager.gc, text_left,
               (index-pListInfo->first_index)*ROW_HEIGHT+top, buf, strlen(buf));

         TidgetManagerResetGC();

         free(buf);
      }
   } else {
      if (selected) {
         values.foreground = myFgPixel;
         values.background = myBgPixel;
      } else {
         values.foreground = myBgPixel;
         values.background = myFgPixel;
      }
      values.function = GXcopy;
      values.fill_style = FillSolid;
      XChangeGC(mainDisplay, gTidgetManager.gc,
            GCForeground | GCBackground | GCFunction | GCFillStyle, &values);
      XFillRectangle(mainDisplay, pTdgtList->dsp_win, gTidgetManager.gc, 0,
            (index-pListInfo->first_index)*ROW_HEIGHT,
            ITEM_LEN*defaultFontWidth, ROW_HEIGHT);
      len = strlen(pListInfo->entries[index]);
      DrawMsgString(mainDisplay, pTdgtList->dsp_win, gTidgetManager.gc,
            text_left, (index-pListInfo->first_index)*ROW_HEIGHT+top,
            pListInfo->entries[index], len);
      TidgetManagerResetGC();
   }
   if (checkbox_cols > 0) {
      int col=0, cur_x=0;

      if (selected) {
         values.foreground = myFgPixel;
         values.background = myBgPixel;
      } else {
         values.foreground = myBgPixel;
         values.background = myFgPixel;
      }
      values.function = GXcopy;
      values.fill_style = FillSolid;
      XChangeGC(mainDisplay, gTidgetManager.gc,
            GCForeground | GCBackground | GCFunction | GCFillStyle, &values);
      for (col=0; col < pListInfo->p_check_array->num_cols; col++) {
         DrawCheckbox(mainDisplay, pTdgtList->dsp_win, gTidgetManager.gc,
               cur_x+box_offset,
               (index-pListInfo->first_index)*ROW_HEIGHT+top-box_w,
               box_w, box_w, pListInfo->p_check_array->value[col][index]);
         cur_x += ROW_HEIGHT;
      }
      TidgetManagerResetGC();
   }
}

static
void RedrawTdgtListScrollWindow(pTdgtList)
   TdgtList *pTdgtList;
{
   ListInfo *pListInfo=(&pTdgtList->list_info);
   double frac=(double)0, start_frac=(double)0;
   int block_h=0, block_start=0;
   int length=ListLength(&pListInfo->list);

   start_frac = (length > 0) ?  (double)((double)(pListInfo->first_index) /
         (double)length) : ((double)0.0);
   /* starting pixel */
   block_start = (int)(pTdgtList->scr_area_h * start_frac);

   if (length > pListInfo->num_visible_lines) {
      frac = (double)((double)pListInfo->num_visible_lines / (double)(length));
   } else {
      frac = 1.0;
   }
   if (pListInfo->first_index+pListInfo->num_visible_lines >= length) {
      block_h = pTdgtList->scr_area_h - block_start;
   } else {
      block_h = (int)(pTdgtList->scr_area_h * frac);
   }
   TgDrawScrollBar(mainDisplay, pTdgtList->scr_win, VERT_SCROLLBAR,
         0, 0, scrollBarW, pTdgtList->scr_area_h, start_frac,
         pListInfo->num_visible_lines, length);
}

static
void RedrawTdgtListDspWindow(pTdgtList)
   TdgtList *pTdgtList;
{
   ListInfo *pListInfo=(&pTdgtList->list_info);
   int length=ListLength(&pListInfo->list);
   int i=0, end=0;
   XGCValues values;

   if (pListInfo->first_index+pListInfo->num_visible_lines > length) {
      end = length;
   } else {
      end = pListInfo->first_index + pListInfo->num_visible_lines;
   }
   values.foreground = myBgPixel;
   values.background = myFgPixel;
   values.function = GXcopy;
   values.fill_style = FillSolid;
   XChangeGC(mainDisplay, gTidgetManager.gc,
         GCForeground | GCBackground | GCFunction | GCFillStyle, &values);
   XFillRectangle(mainDisplay, pTdgtList->dsp_win, gTidgetManager.gc, 0, 0,
         pTdgtList->dsp_win_info.w, pTdgtList->dsp_win_info.h);
   TidgetManagerResetGC();

   if (pTdgtList->multicolor) {
      CVListElem *pElem=NULL;

      for (i=0, pElem=ListFirst(&pListInfo->list);
            i < pListInfo->first_index && pElem != NULL;
            i++, pElem=ListNext(&pListInfo->list, pElem)) {
      }
      for (i=pListInfo->first_index; i < end && pElem != NULL;
            i++, pElem=ListNext(&pListInfo->list, pElem)) {
         ListItemInfo *pListItemInfo=(ListItemInfo*)(pElem->obj);

         RedrawTdgtListItem(pTdgtList, i, pListItemInfo);
      }
   } else {
      for (i=pListInfo->first_index; i < end; i++) {
         RedrawTdgtListItem(pTdgtList, i, NULL);
      }
   }
}

static
void RedrawTdgtListBaseWindow(pTdgtList)
   TdgtList *pTdgtList;
{
   if (threeDLook) {
      struct BBRec bbox;

      SetBBRec(&bbox, 0, 0, pTdgtList->pti->tci.win_info.w,
            pTdgtList->pti->tci.win_info.h);
      TgDrawThreeDButton(mainDisplay, pTdgtList->pti->tci.win,
            gTidgetManager.gc, &bbox, TGBS_LOWRED, 2, FALSE);
      TidgetManagerResetGC();
   }
}

static
void RedrawTdgtList(pti)
   TidgetInfo *pti;
{
   TdgtList *pTdgtList=(TdgtList*)(pti->tidget);

   XEvent ev;

   RedrawTdgtListScrollWindow(pTdgtList);
   RedrawTdgtListDspWindow(pTdgtList);
   RedrawTdgtListBaseWindow(pTdgtList);

   while (XCheckWindowEvent(mainDisplay, pTdgtList->pti->tci.win, ExposureMask,
         &ev)) ;
}

/* --------------------- TdgtListEventHandler() --------------------- */

static
int TdgtListScrollItemCallback(pv_userdata)
   void *pv_userdata;
   /* returns TRUE to cancel scrolling */
{
   TdgtList *pTdgtList=((TdgtList*)pv_userdata);
   ListInfo *pListInfo=(&pTdgtList->list_info);
   int num_visible_lines=pListInfo->num_visible_lines;
   int length=ListLength(&pListInfo->list);

   if (pTdgtList->scr_dir == SCRL_UP) {
      if (pListInfo->first_index == 0) return FALSE;
      pListInfo->first_index--;
   } else {
      if (length <= num_visible_lines ||
            pListInfo->first_index+num_visible_lines == length) {
         return FALSE;
      }
      pListInfo->first_index++;
   }
   RedrawTdgtList(pTdgtList->pti);
   XSync(mainDisplay, False);

   return FALSE;
}

static
int TdgtListScrollPageCallback(pv_userdata)
   void *pv_userdata;
   /* returns TRUE to cancel scrolling */
{
   TdgtList *pTdgtList=((TdgtList*)pv_userdata);
   ListInfo *pListInfo=(&pTdgtList->list_info);
   int num_visible_lines=pListInfo->num_visible_lines;
   int length=ListLength(&pListInfo->list);

   if (pTdgtList->scr_dir == SCRL_UP) {
      if (pListInfo->first_index == 0) return FALSE;
      pListInfo->first_index -= num_visible_lines;
      if (pListInfo->first_index < 0) pListInfo->first_index = 0;
   } else {
      if (length <= num_visible_lines ||
            pListInfo->first_index+num_visible_lines == length) {
         return FALSE;
      }
      pListInfo->first_index += num_visible_lines;
      if (pListInfo->first_index+num_visible_lines >= length) {
         pListInfo->first_index = length-num_visible_lines;
      }
   }
   RedrawTdgtList(pTdgtList->pti);
   XSync(mainDisplay, False);

   return FALSE;
}

static
int DoTdgtListBtnScroll(pTdgtList, scroll_page, scr_dir, pbbox)
   TdgtList *pTdgtList;
   int scroll_page, scr_dir;
   struct BBRec *pbbox;
   /* returns TRUE if done scrolling */
{
   ListInfo *pListInfo=(&pTdgtList->list_info);
   int num_visible_lines=pListInfo->num_visible_lines;
   int length=ListLength(&pListInfo->list);
   ScrollBtnCallbackInfo sbci;

   pTdgtList->scr_dir = scr_dir;
   memset(&sbci, 0, sizeof(ScrollBtnCallbackInfo));

   if (scroll_page) {
      sbci.ms = 200;
      sbci.pv_userdata = pTdgtList;
      sbci.pf_scroll_btn_callback = TdgtListScrollPageCallback;
      if (TgPressButtonLoop(mainDisplay, pTdgtList->scr_win, NULL,
            &sbci)) {
         if (scr_dir == SCRL_UP) {
            if (pListInfo->first_index == 0) return TRUE;
            pListInfo->first_index -= num_visible_lines;
            if (pListInfo->first_index < 0) pListInfo->first_index = 0;
         } else {
            if (length <= num_visible_lines ||
                  pListInfo->first_index+num_visible_lines == length) {
               return TRUE;
            }
            pListInfo->first_index += num_visible_lines;
            if (pListInfo->first_index+num_visible_lines >= length) {
               pListInfo->first_index = length-num_visible_lines;
            }
         }
      }
   } else {
      sbci.ms = 50;
      sbci.pv_userdata = pTdgtList;
      sbci.pf_scroll_btn_callback = TdgtListScrollItemCallback;
      if (TgPressButtonLoop(mainDisplay, pTdgtList->scr_win, pbbox, &sbci)) {
         if (scr_dir == SCRL_UP) {
            if (pListInfo->first_index == 0) return TRUE;
            pListInfo->first_index--;
         } else {
            if (length <= num_visible_lines ||
                  pListInfo->first_index+num_visible_lines == length) {
               return TRUE;
            }
            pListInfo->first_index++;
         }
      }
   }
   return FALSE;
}

static
void DoDragInTdgtList(pTdgtList, button_ev, btn_y, btn_offset)
   TdgtList *pTdgtList;
   XButtonEvent *button_ev;
   int btn_y, btn_offset;
{
   ListInfo *pListInfo=(&pTdgtList->list_info);
   int length=ListLength(&pListInfo->list);
   int num_visible_lines=pListInfo->num_visible_lines;
   double frac=(double)0, start_frac=(double)0;
   int block_h=0, block_start=0, done=FALSE;

   if (length <= num_visible_lines) return;

   frac = (double)((double)num_visible_lines / (double)(length));
   block_h = (int)(pTdgtList->scr_area_h * frac);

   if (length > pListInfo->num_visible_lines) {
      frac = (double)((double)pListInfo->num_visible_lines / (double)(length));
      block_h = (int)(((double)(pTdgtList->scr_area_h-(scrollBarW<<1))) * frac);
   } else {
      frac = 1.0;
      block_h = pTdgtList->scr_area_h-(scrollBarW<<1);
   }
   if (threeDLook) {
      block_start = button_ev->y+btn_offset;;
      start_frac = (double)((double)(block_start-scrollBarW) /
            (double)(pTdgtList->scr_area_h-(scrollBarW<<1)));
      if (block_start+block_h >= pTdgtList->scr_area_h-scrollBarW) {
         pListInfo->first_index = length - num_visible_lines;
      } else {
         pListInfo->first_index = (int)(length * start_frac);
      }
   } else {
      block_start = button_ev->y;
      start_frac = (double)((double)(block_start) /
            (double)(pTdgtList->scr_area_h));
      if (block_start+block_h >= pTdgtList->scr_area_h) {
         pListInfo->first_index = length - num_visible_lines;
      } else {
         pListInfo->first_index = (int)(length * start_frac);
      }
   }
   RedrawTdgtList(pTdgtList->pti);

   XGrabPointer(mainDisplay, pTdgtList->scr_win, False,
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
                 ((double)(pTdgtList->scr_area_h-(scrollBarW<<1))));

            if (y <= scrollBarW) {
               new_name_first = 0;
            } else if (y+block_h >= pTdgtList->scr_area_h-scrollBarW) {
               new_name_first = length - num_visible_lines;
            } else {
               new_name_first = (int)(length * start_frac);
            }
         } else {
            start_frac = (double)(((double)y) /
                 ((double)pTdgtList->scr_area_h));

            if (y <= 0) {
               new_name_first = 0;
            } else if (y+block_h >= pTdgtList->scr_area_h) {
               new_name_first = length - num_visible_lines;
            } else {
               new_name_first = (int)(length * start_frac);
            }
         }
         if (pListInfo->first_index != new_name_first) {
            pListInfo->first_index = new_name_first;
            RedrawTdgtList(pTdgtList->pti);
         }
         while (XCheckMaskEvent(mainDisplay, PointerMotionMask, &ev)) ;
      }
   }
}

static
void TdgtListScrollHandler(pTdgtList, button_ev)
   TdgtList *pTdgtList;
   XButtonEvent *button_ev;
{
   ListInfo *pListInfo=(&pTdgtList->list_info);
   int length=ListLength(&pListInfo->list);
   int do_drag=FALSE, btn_offset=0;

   if (!threeDLook && button_ev->button == Button3 &&
         button_ev->type == ButtonPress) {
      if (DoTdgtListBtnScroll(pTdgtList,
            ((button_ev->state & (ShiftMask|ControlMask)) != 0),
            SCRL_UP, NULL)) {
         return;
      }
   } else if (!threeDLook && button_ev->button == Button1 &&
         button_ev->type == ButtonPress) {
      if (DoTdgtListBtnScroll(pTdgtList,
            ((button_ev->state & (ShiftMask|ControlMask)) != 0),
            SCRL_DN, NULL)) {
         return;
      }
   } else if (button_ev->button == Button1 && button_ev->type == ButtonPress) {
      if (button_ev->y < scrollBarW ||
            button_ev->y >= pTdgtList->scr_area_h-scrollBarW) {
         int which=INVALID;
         struct BBRec bbox;

         if (button_ev->y < scrollBarW) {
            which = SCRL_UP;
            SetBBRec(&bbox, 0, 0, scrollBarW, scrollBarW);
         } else {
            which = SCRL_DN;
            SetBBRec(&bbox, 0, pTdgtList->scr_area_h-scrollBarW, scrollBarW,
                  pTdgtList->scr_area_h);
         }
         if (DoTdgtListBtnScroll(pTdgtList, FALSE, which, &bbox)) {
            return;
         }
      } else {
         double start_frac=(double)0;
         int hit=0;

         start_frac = (length > 0) ?
               (double)((double)(pListInfo->first_index) / (double)length) :
               ((double)0.0);
         hit = TgGetScrollHit(button_ev->x, button_ev->y, VERT_SCROLLBAR,
               scrollBarW, pTdgtList->scr_area_h, start_frac,
               pListInfo->num_visible_lines, length, &btn_offset);
         if (hit == 0) {
            do_drag = TRUE;
         } else {
            if (DoTdgtListBtnScroll(pTdgtList, TRUE,
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
      DoDragInTdgtList(pTdgtList, button_ev, button_ev->y, btn_offset);
   } else {
      RedrawTdgtList(pTdgtList->pti);
   }
}

static
int TdgtListDspHandler(pTdgtList, button_ev)
   TdgtList *pTdgtList;
   XButtonEvent *button_ev;
{
   if (enableMouseWheel &&
         (button_ev->button == Button4 || button_ev->button == Button5)) {
      int which=INVALID;
      struct BBRec bbox;

      if (button_ev->button == Button4) {
         which = SCRL_UP;
         SetBBRec(&bbox, 0, 0, scrollBarW, scrollBarW);
      } else if (button_ev->button == Button5) {
         which = SCRL_DN;
         SetBBRec(&bbox, 0, pTdgtList->scr_area_h-scrollBarW, scrollBarW,
               pTdgtList->scr_area_h);
      }
      if (which != INVALID) {
         DoTdgtListBtnScroll(pTdgtList, FALSE, which, &bbox);
         RedrawTdgtList(pTdgtList->pti);
      }
      return INVALID;
   } else if (button_ev->button != Button1) {
      return INVALID;
   }
   return INVALID;
}

static
int TdgtListControlChar(pTdgtList, key_ev, key_sym)
   TdgtList *pTdgtList;
   XKeyEvent *key_ev;
   KeySym key_sym;
{
   ListInfo *pListInfo=(&pTdgtList->list_info);
   int length=ListLength(&pListInfo->list);
   int num_visible_lines=pListInfo->num_visible_lines;
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
      if (pTdgtList->can_select) {
         /*
          * i = (pni->marked_index < length-1) ?
          *       pni->marked_index+1 : length-1;
          */
      } else {
         if (length > num_visible_lines) {
            if (pListInfo->first_index+num_visible_lines < length) {
               pListInfo->first_index++;
            }
         }
      }
      break;
   case 'p': /* up one */
   case 'k':
      if (pTdgtList->can_select) {
         /*
          * i = (pni->marked_index>0) ? pni->marked_index-1 : 0;
          */
      } else {
         if (length > num_visible_lines) {
            if (pListInfo->first_index > 0) {
               pListInfo->first_index--;
            }
         }
      }
      break;
   case 'd': /* down one page */
   case 'f':
      if (pTdgtList->can_select) {
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
            if (pListInfo->first_index+num_visible_lines < length) {
               pListInfo->first_index += num_visible_lines;
               if (pListInfo->first_index + num_visible_lines > length) {
                  pListInfo->first_index = length - num_visible_lines;
               }
            }
         }
      }
      break;
   case 'u': /* up one page */
   case 'b':
      if (pTdgtList->can_select) {
         /*
          * i = (pni->marked_index > (num_visible_lines-1)) ?
          *       pni->marked_index-num_visible_lines : 0;
          */
      } else {
         pListInfo->first_index -= num_visible_lines;
         if (pListInfo->first_index < 0) pListInfo->first_index = 0;
      }
      break;
   default: return BAD;
   }
   return i;
}

static
void SpecialKeyInTdgtList(pTdgtList, key_ev, key_sym, pn_changing,
      pn_selected_btn_index)
   TdgtList *pTdgtList;
   XKeyEvent *key_ev;
   KeySym key_sym;
   int *pn_changing, *pn_selected_btn_index;
{
   ListInfo *pListInfo=(&pTdgtList->list_info);
   int saved_first=pListInfo->first_index;
   int saved_marked=pListInfo->marked_index;
   int rc=TdgtListControlChar(pTdgtList, key_ev, key_sym);

   if (rc == BAD || rc == INVALID) return;

   if (saved_first != pListInfo->first_index ||
         saved_marked != pListInfo->marked_index) {
      RedrawTdgtList(pTdgtList->pti);
   }
}

static XComposeStatus c_stat;

static
int KeyPressInTdgtList(pTdgtList, key_ev, pn_changing, pn_selected_btn_index)
   TdgtList *pTdgtList;
   XKeyEvent *key_ev;
   int *pn_changing, *pn_selected_btn_index;
{
   ListInfo *pListInfo=(&pTdgtList->list_info);
   int length=ListLength(&pListInfo->list);
   char buf[80];
   KeySym key_sym;
   int has_ch=FALSE;

   has_ch = XLookupString(key_ev, buf, sizeof(buf), &key_sym, &c_stat);
   TranslateKeys(buf, &key_sym);

   if ((key_ev->state & ControlMask) && key_sym == XK_j) {
      SpecialKeyInTdgtList(pTdgtList, key_ev, key_sym, pn_changing,
            pn_selected_btn_index);
   } else if (CharIsCRorLF(key_ev, buf, key_sym, &has_ch)) {
      if (pTdgtList->can_select) {
         /*
          * if (pni->def_btn_id != INVALID) {
          *    *pn_changing = FALSE;
          *    *pn_selected_btn_index = GetBtnIndexFromBtnId(pni->def_btn_id);
          * }
          */
      } else {
         return FALSE;
      }
   } else if (CharIsESC(key_ev, buf, key_sym, &has_ch)) {
      if (pTdgtList->can_select) {
         /*
          * *pn_changing = FALSE;
          * *pn_selected_btn_index = GetBtnIndexFromBtnId(BUTTON_CANCEL);
          */
      }
   } else if (CharIsBSorDEL(key_ev, buf, key_sym, &has_ch, FALSE)) {
      if (pTdgtList->can_select) {
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
      if (pTdgtList->can_select) {
         /* should tab out of focus?! */
      }
   } else if (length != 0 && ((key_sym>'\040' && key_sym<='\177' &&
         (key_ev->state & ControlMask)) || key_sym==XK_Up ||
         key_sym==XK_Down || key_sym==XK_KP_Up || key_sym==XK_KP_Down ||
         key_sym==XK_Page_Up || key_sym==XK_KP_Page_Up ||
         key_sym==XK_Page_Down || key_sym==XK_KP_Page_Down)) {
      SpecialKeyInTdgtList(pTdgtList, key_ev, key_sym, pn_changing,
            pn_selected_btn_index);
   } else if (key_sym>='\040' && key_sym<='\177' && length != 0) {
      if (pTdgtList->can_select) {
         /*
          * CharInNames(buf, pn_changing, pn_selected_btn_index);
          */
      }
      return FALSE;
   }
   return TRUE;
}

static
void ButtonPressInTdgtList(pTdgtList, button_ev, pn_changing,
      pn_selected_btn_index)
   TdgtList *pTdgtList;
   XButtonEvent *button_ev;
   int *pn_changing, *pn_selected_btn_index;
{
   if (button_ev->window == pTdgtList->scr_win) {
      TdgtListScrollHandler(pTdgtList, button_ev);
   } else if (button_ev->window == pTdgtList->dsp_win) {
      int double_clicked=(TdgtListDspHandler(pTdgtList, button_ev)!=INVALID);

      if (double_clicked) {
         /* do something here */
      }
      if (pTdgtList->can_select) {
         /* do something here */
      }
   }
}

static
int TdgtListEventHandler(pti, input, handling_pti)
   TidgetInfo *pti, *handling_pti;
   XEvent *input;
{
   TdgtList *pTdgtList=(TdgtList*)(pti->tidget);

   if (pti != handling_pti) return FALSE;

   if (input->type == Expose) {
      RedrawTdgtList(pTdgtList->pti);
   } else if (input->type == KeyPress) {
      if (pTdgtList->can_select) {
         /* KeyPressInTdgtList(pTdgtList, &changing, &selected_btn_index); */
      } else {
         return KeyPressInTdgtList(pTdgtList, &input->xkey, NULL, NULL);
      }
   } else if (input->type == ButtonPress) {
      if (pTdgtList->can_select) {
         /* ButtonPressInTdgtList(pTdgtList, &changing, &selected_btn_index); */
      } else {
         ButtonPressInTdgtList(pTdgtList, &input->xbutton, NULL, NULL);
      }
   }
   return TRUE;
}

/* --------------------- IsTdgtListEvent() --------------------- */

static
int IsTdgtListEvent(pti, input, ppti_handler_tidget_return)
   TidgetInfo *pti, **ppti_handler_tidget_return;
   XEvent *input;
{
   TdgtList *pTdgtList=(TdgtList*)(pti->tidget);

   if (input->xany.window == pTdgtList->pti->tci.win ||
         input->xany.window == pTdgtList->dsp_win ||
         input->xany.window == pTdgtList->scr_win) {
      *ppti_handler_tidget_return = pti;
      return TRUE;
   }
   return FALSE;
}

/* --------------------- DestroyTdgtList() --------------------- */

static
void DestroyTdgtList(pti)
   TidgetInfo *pti;
{
   TdgtList *pTdgtList=(TdgtList*)(pti->tidget);

   TdgtListReset(pTdgtList);

   free(pTdgtList);
}

/* --------------------- MapTdgtList() --------------------- */

static
void MapTdgtList(pti)
   TidgetInfo *pti;
{
   TdgtList *pTdgtList=(TdgtList*)(pti->tidget);

#ifdef MAPBEFORESELECT
   XMapWindow(mainDisplay, pTdgtList->pti->tci.win);
   XSelectInput(mainDisplay, pTdgtList->pti->tci.win, ExposureMask);

   XMapWindow(mainDisplay, pTdgtList->dsp_win);
   XSelectInput(mainDisplay, pTdgtList->dsp_win, KeyPressMask |
         ButtonPressMask | ExposureMask);

   XMapWindow(mainDisplay, pTdgtList->scr_win);
   XSelectInput(mainDisplay, pTdgtList->scr_win, KeyPressMask |
         ButtonPressMask | ExposureMask);
#else
   XSelectInput(mainDisplay, pTdgtList->pti->tci.win, ExposureMask);
   XMapWindow(mainDisplay, pTdgtList->pti->tci.win);

   XSelectInput(mainDisplay, pTdgtList->dsp_win, KeyPressMask |
         ButtonPressMask | ExposureMask);
   XMapWindow(mainDisplay, pTdgtList->dsp_win);

   XSelectInput(mainDisplay, pTdgtList->scr_win, KeyPressMask |
         ButtonPressMask | ExposureMask);
   XMapWindow(mainDisplay, pTdgtList->scr_win);
#endif
}

/* --------------------- TdgtListMoveResize() --------------------- */

static
void TdgtListMoveResize(pti, x, y, w, h)
   TidgetInfo *pti;
   int x, y, w, h;
{
   TdgtList *pTdgtList=(TdgtList*)(pti->tidget);

   pTdgtList->pti->tci.win_info.x = x;
   pTdgtList->pti->tci.win_info.y = y;
   pTdgtList->pti->tci.win_info.w = w;
   pTdgtList->pti->tci.win_info.h = h;

   pTdgtList->pti->tci.content_w =
         w-(windowPadding<<1)-pTdgtList->pti->tci.h_pad;
   pTdgtList->pti->tci.content_h =
         h-(windowPadding<<1)-pTdgtList->pti->tci.v_pad;
   XMoveResizeWindow(mainDisplay, pTdgtList->pti->tci.win, x, y, w, h);

   pTdgtList->dsp_win_info.x = windowPadding;
   pTdgtList->dsp_win_info.y = windowPadding;
   pTdgtList->dsp_win_info.w = w-(windowPadding<<1)-scrollBarW;
   pTdgtList->dsp_win_info.h = h-(windowPadding<<1);
   XMoveResizeWindow(mainDisplay, pTdgtList->dsp_win, pTdgtList->dsp_win_info.x,
         pTdgtList->dsp_win_info.y, pTdgtList->dsp_win_info.w,
         pTdgtList->dsp_win_info.h);

   pTdgtList->scr_win_info.x = w-windowPadding-scrollBarW;
   pTdgtList->scr_win_info.y = windowPadding;
   pTdgtList->scr_win_info.w = scrollBarW;
   pTdgtList->scr_win_info.h = h-(windowPadding<<1);
   XMoveResizeWindow(mainDisplay, pTdgtList->scr_win, pTdgtList->scr_win_info.x,
         pTdgtList->scr_win_info.y, pTdgtList->scr_win_info.w,
         pTdgtList->scr_win_info.h);
}

/* --------------------- TdgtListSendCmd() --------------------- */

static
int TdgtListSendCmd(pti, cmd_type, cmd_arg, pv_cmd_userdata)
   TidgetInfo *pti;
   int cmd_type, cmd_arg;
   void *pv_cmd_userdata;
{
   TdgtList *pTdgtList=(TdgtList*)(pti->tidget);

   if (pTdgtList != NULL) {
   }
   return FALSE;
}

/* --------------------- TdgtListReset() --------------------- */

void TdgtListReset(pTdgtList)
   TdgtList *pTdgtList;
{
   ListInfo *pListInfo=(&pTdgtList->list_info);
   CVListElem *pElem=NULL;

   TdgtListCleanUpEntries(pTdgtList);

   for (pElem=ListFirst(&pListInfo->list); pElem != NULL;
         pElem=ListNext(&pListInfo->list, pElem)) {
      ListItemInfo *pListItemInfo=(ListItemInfo*)(pElem->obj);

      FreeListItemInfo(pListItemInfo);
   }
   ListUnlinkAll(&pListInfo->list);
}

/* --------------------- CreateTdgtList() --------------------- */

static
void TdgtListCalcHeight(num_visible_lines, pn_content_h)
   int num_visible_lines, *pn_content_h;
{
   int h=(num_visible_lines*ROW_HEIGHT);

   if (pn_content_h != NULL) *pn_content_h = h;
}

TdgtList *CreateTdgtList(parent_win, parent_tidgetinfo, ctl_id, x, y, w, h_pad,
      v_pad, num_visible_lines, can_select, multicolor, auto_scroll_on_insert)
   Window parent_win;
   TidgetInfo *parent_tidgetinfo;
   int ctl_id, x, y, w, h_pad, v_pad, num_visible_lines, auto_scroll_on_insert;
{
   int bg_pixel=(threeDLook ? myLtGryPixel : myBgPixel), h=0, content_h=0;
   TdgtList *pTdgtList=NULL;

   TdgtListCalcHeight(num_visible_lines, &content_h);

   h = content_h + (windowPadding<<1) + (v_pad<<1);

   pTdgtList = (TdgtList*)malloc(sizeof(TdgtList));
   if (pTdgtList == NULL) FailAllocMessage();
   memset(pTdgtList, 0, sizeof(TdgtList));

   pTdgtList->pti = NewTidgetInfo(parent_tidgetinfo, TIDGET_TYPE_LIST,
         pTdgtList, ctl_id, NULL);
   if ((pTdgtList->pti->tci.win=XCreateSimpleWindow(mainDisplay, parent_win,
         x, y, w, h, brdrW, myBorderPixel, bg_pixel)) == 0) {
      FailToCreateWindowMessage("CreateTdgtList()", NULL, TRUE);
   }
   XSelectInput(mainDisplay, pTdgtList->pti->tci.win, ExposureMask);
   SetTidgetInfoBasic(pTdgtList->pti, TIDGET_TYPE_LIST, pTdgtList, parent_win,
         x, y, w, h, h_pad, v_pad, TGBS_LOWRED, NULL);
   TidgetSetCallbacks(pTdgtList->pti,
         RedrawTdgtList, TdgtListEventHandler, IsTdgtListEvent, DestroyTdgtList,
         MapTdgtList, TdgtListMoveResize, TdgtListSendCmd);

   CVListInit(&pTdgtList->list_info.list);

   /* Note: scrollBarW accounts for windowPadding */
   if ((pTdgtList->dsp_win=XCreateSimpleWindow(mainDisplay,
         pTdgtList->pti->tci.win, windowPadding, windowPadding,
         w-(windowPadding<<1)-scrollBarW, h-(windowPadding<<1), brdrW,
         myBorderPixel, bg_pixel)) == 0) {
      FailToCreateWindowMessage("CreateTdgtList()", NULL, TRUE);
   }
   XSelectInput(mainDisplay, pTdgtList->dsp_win, KeyPressMask |
         ButtonPressMask | ExposureMask);
   pTdgtList->dsp_win_info.x = windowPadding;
   pTdgtList->dsp_win_info.y = windowPadding;
   pTdgtList->dsp_win_info.w = w-(windowPadding<<1)-scrollBarW;
   pTdgtList->dsp_win_info.h = h-(windowPadding<<1);

   if ((pTdgtList->scr_win=XCreateSimpleWindow(mainDisplay,
         pTdgtList->pti->tci.win, w-windowPadding-scrollBarW, windowPadding,
         scrollBarW, h-(windowPadding<<1), brdrW, myBorderPixel, bg_pixel)) ==
         0) {
      FailToCreateWindowMessage("CreateTdgtList()", NULL, TRUE);
   }
   XSelectInput(mainDisplay, pTdgtList->scr_win, KeyPressMask |
         ButtonPressMask | ExposureMask);
   pTdgtList->scr_win_info.x = w-windowPadding-scrollBarW;
   pTdgtList->scr_win_info.y = windowPadding;
   pTdgtList->scr_win_info.w = scrollBarW;
   pTdgtList->scr_win_info.h = h-(windowPadding<<1);

   pTdgtList->list_info.num_visible_lines = num_visible_lines;
   pTdgtList->can_select = can_select;
   pTdgtList->multicolor = (colorDisplay ? multicolor : FALSE);
   pTdgtList->auto_scroll_on_insert = auto_scroll_on_insert;

   pTdgtList->scr_area_h = num_visible_lines*ROW_HEIGHT;

   return pTdgtList;
}

/* --------------------- TdgtListCleanUpEntries() --------------------- */

int TdgtListCleanUpEntries(pTdgtList)
   TdgtList *pTdgtList;
{
   ListInfo *pListInfo=(&pTdgtList->list_info);
   int length=ListLength(&pListInfo->list);

   if (pListInfo->entries != NULL) {
      int i=0;

      for (i=0; i < length; i++) {
         UtilFree(pListInfo->entries[i]);
      }
      free(pListInfo->entries);
      pListInfo->entries = NULL;
   }
   return TRUE;
}

/* --------------------- TdgtListUpdateEntries() --------------------- */

int TdgtListUpdateEntries(pTdgtList)
   TdgtList *pTdgtList;
{
   ListInfo *pListInfo=(&pTdgtList->list_info);
   CVListElem *pElem=NULL;
   int length=ListLength(&pListInfo->list), index=0;

   if (pListInfo->entries != NULL) {
      TdgtListCleanUpEntries(pTdgtList);
   }
   pListInfo->entries = (char**)malloc(length*sizeof(char*));
   if (pListInfo->entries == NULL) FailAllocMessage();
   memset(pListInfo->entries, 0, length*sizeof(char*));

   for (pElem=ListFirst(&pListInfo->list); pElem != NULL;
         pElem=ListNext(&pListInfo->list, pElem), index++) {
      ListItemInfo *pListItemInfo=(ListItemInfo*)(pElem->obj);

      pListInfo->entries[index] = UtilStrDup(pListItemInfo->buf);
      if (pListInfo->entries[index] == NULL) FailAllocMessage();
   }
   return TRUE;
}

/* --------------------- TdgtListScrollToBottom() --------------------- */

int TdgtListScrollToBottom(pTdgtList)
   TdgtList *pTdgtList;
{
   ListInfo *pListInfo=(&pTdgtList->list_info);
   int length=ListLength(&pListInfo->list);

   if (pListInfo->num_visible_lines < length) {
      pListInfo->first_index = length - pListInfo->num_visible_lines;
   } else {
      pListInfo->first_index = 0;
   }
   return TRUE;
}

/* --------------------- TdgtListInsertString() --------------------- */

int TdgtListInsertString(pTdgtList, buf)
   TdgtList *pTdgtList;
   char *buf;
{
   ListInfo *pListInfo=(&pTdgtList->list_info);
   ListItemInfo *pListItemInfo=(ListItemInfo*)malloc(sizeof(ListItemInfo));

   if (pListItemInfo == NULL) FailAllocMessage();
   memset(pListItemInfo, 0, sizeof(ListItemInfo));

   *pListItemInfo->nick_name = '\0';
   pListItemInfo->color_index = colorIndex;
   pListItemInfo->font_style = STYLE_NR;
   pListItemInfo->buf = UtilStrDup(buf);
   if (pListItemInfo->buf == NULL) FailAllocMessage();

   TdgtListCleanUpEntries(pTdgtList);
   ListAppend(&pListInfo->list, pListItemInfo);

   if (pTdgtList->auto_scroll_on_insert) {
      TdgtListScrollToBottom(pTdgtList);
   }
   TdgtListUpdateEntries(pTdgtList);
   RedrawTdgtList(pTdgtList->pti);

   return TRUE;
}

int TdgtListInsertListItemInfo(pTdgtList, pListItemInfo)
   TdgtList *pTdgtList;
   ListItemInfo *pListItemInfo;
{
   ListInfo *pListInfo=(&pTdgtList->list_info);

   TdgtListCleanUpEntries(pTdgtList);
   ListAppend(&pListInfo->list, pListItemInfo);

   if (pTdgtList->auto_scroll_on_insert) {
      TdgtListScrollToBottom(pTdgtList);
   }
   TdgtListUpdateEntries(pTdgtList);
   RedrawTdgtList(pTdgtList->pti);

   return TRUE;
}

int TdgtListSetAutoScrollOnInsert(pTdgtList, auto_scroll_on_insert)
   TdgtList *pTdgtList;
   int auto_scroll_on_insert;
{
   pTdgtList->auto_scroll_on_insert = auto_scroll_on_insert;
   return TRUE;
}

int TdgtListGetAutoScrollOnInsert(pTdgtList)
   TdgtList *pTdgtList;
{
   return pTdgtList->auto_scroll_on_insert;
}

/* --------------------- Init & Clean Up --------------------- */

int InitTdgtList()
{
   if (msgFontSet == NULL && msgFontPtr == NULL) {
      gnListFontHeight = defaultFontHeight;
      gnListFontWidth = defaultFontWidth;
      gnListFontAsc = defaultFontAsc;
      gnListFontDes = defaultFontDes;
      gnListFontPtr = defaultFontPtr;
   } else {
      gnListFontHeight = msgFontHeight;
      gnListFontWidth = msgFontWidth;
      gnListFontAsc = msgFontAsc;
      gnListFontDes = msgFontDes;
      gnListFontPtr = msgFontPtr;
   }
   return TRUE;
}

void CleanUpTdgtList()
{
   gnListFontHeight = gnListFontWidth = gnListFontAsc = gnListFontDes = 0;
   gnListFontPtr = NULL;
}

