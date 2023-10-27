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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/choose.c,v 1.11 2011/05/16 16:21:56 william Exp $
 */

#define _INCLUDE_FROM_CHOOSE_C_

#include "tgifdefs.h"

#include "auxtext.e"
#include "button.e"
#include "choose.e"
#include "cutpaste.e"
#include "cursor.e"
#include "dialog.e"
#include "drawing.e"
#include "file.e"
#include "font.e"
#include "mainloop.e"
#include "mainmenu.e"
#include "menu.e"
#include "msg.e"
#include "names.e"
#include "raster.e"
#include "rect.e"
#include "remote.e"
#include "ruler.e"
#include "scroll.e"
#include "setup.e"
#include "strtbl.e"
#include "util.e"

GC nameGC=(GC)0, revNameGC=(GC)0;

typedef struct NamesRec {
   Window base_win, dsp_base_win, dsp_win, scroll_win;
   int base_win_w, base_win_h;
   int dsp_base_win_w, dsp_base_win_h;
   int dsp_area_w, dsp_area_h, scroll_area_h;

   char **entries;
   int num_entries;
   int first_index;
   int marked_index;
   int num_btns;
   struct BBRec btn_bbox[MAXBUTTONS], path_bbox;
   char btn_str[MAXBUTTONS][40];
   int btn_id[MAXBUTTONS];

   char inbuf[512];
   int buf_index, just_clicked, scroll_dir;

   DspList *dsp_ptr;
   char *title_str, *formatted_title_str;

   struct CheckArrayRec *p_check_array;

   int title_w, title_h, title_num_lines;
   int a_btn_w, graph_w, btn_start, title_start, graph_start;
   int btn_w, btn_gap, btn_selected, exposed, screen_w, screen_h;
   int edit_style, loop_once, dont_free_entries, leading;

   int change_to_root, pop_from_root, faking_dot_dot;
   int tabbed_from_root, just_tabbed_from_root;
   int def_btn_id, double_click_btn_id;

   GetEntriesFunc *pf_before_loop;
   AfterLoopFunc *pf_after_loop;
   CheckUpdateFunc *pf_check_update; /* called when a checkbox changes state */

   /* used only when edit_style== NAMES_SELECT_FILE */
   char dir_name[MAXPATHLENGTH+1], saved_dir_name[MAXPATHLENGTH+1];
   char name[MAXPATHLENGTH+1], saved_name[MAXPATHLENGTH+1];
   int name_index, remote_file;

   void *userdata;
} *NamesRecPtr;

static struct NamesRec namesInfo;

static XComposeStatus c_stat;

int GetWorkingDirectory(buf, buf_sz)
   char *buf;
   int buf_sz;
{
   char *psz_result=NULL;

#ifdef VMS
   psz_result = getcwd(buf, buf_sz, 0);
#else /* ~VMS */
#ifdef ibm
   psz_result = getwd(buf, buf_sz);
#else /* ~ibm */
#ifdef NeXT
   psz_result = getwd(buf, buf_sz);
#else /* ~NeXT */
#ifdef luna88k
   psz_result = getwd(buf);
#else /* ~luna88k */
#ifdef sequent
   psz_result = getwd(buf);
#else /* ~sequent */
   psz_result = getcwd(buf, buf_sz);
#endif /* sequent */
#endif /* luna88k */
#endif /* NeXT */
#endif /* ibm */
#endif /* VMS */
   if (psz_result == NULL) {
      UtilStrCpyN(buf, buf_sz, ".");
      return FALSE;
   }
   return TRUE;
}

int SetWorkingDirectory(dir)
   char *dir;
{
   return (chdir(dir) == 0);
}

static
void CalcBaseWinWidth(pni)
   struct NamesRec *pni;
{
   int max_w=max(pni->graph_w,pni->btn_w);

   pni->base_win_w = max_w + (defaultFontWidth<<2);
   pni->graph_start = ((pni->base_win_w-pni->graph_w)>>1);
   if (pni->title_num_lines <= 1) {
      pni->title_start = ((pni->base_win_w-pni->title_w)>>1);
   } else {
      pni->title_start = pni->graph_start;
   }
   pni->btn_start = ((pni->base_win_w-pni->btn_w)>>1);

   pni->path_bbox.ltx = pni->graph_start;
   pni->path_bbox.lty = (ROW_HEIGHT<<1)+pni->title_h;
   /* the path_bbox needs 2 pixels around it */
   pni->path_bbox.lty -= 2;
   pni->path_bbox.rbx = pni->path_bbox.ltx + ITEM_LEN*defaultFontWidth+6;
   pni->path_bbox.rby = pni->path_bbox.lty +
         ((msgFontSet==NULL && msgFontPtr==NULL) ? defaultFontHeight :
         msgFontHeight)+1+4;
   if (boldMsgFontSet == NULL && boldMsgFontPtr == NULL) {
      pni->base_win_h = (7+ITEM_DSPED)*ROW_HEIGHT + pni->title_h;
   } else {
      pni->base_win_h = (6+ITEM_DSPED)*ROW_HEIGHT + boldMsgFontHeight + 1 +
            pni->title_h;
   }
}

void CleanUpCheckArray(pCheckArray)
   /* Does not free pCheckArray */
   struct CheckArrayRec *pCheckArray;
{
   int i, num_cols=pCheckArray->num_cols;

   if (pCheckArray->col_name != NULL) {
      /* there's an extra one column here! */
      for (i=0; i <= num_cols; i++) {
         if (pCheckArray->col_name[i] != NULL) {
            UtilFree(pCheckArray->col_name[i]);
         }
      }
      free(pCheckArray->col_name);
   }
   if (pCheckArray->value != NULL) {
      for (i=0; i < num_cols; i++) {
         if (pCheckArray->value[i] != NULL) {
            free(pCheckArray->value[i]);
         }
      }
      free(pCheckArray->value);
   }
   memset(pCheckArray, 0, sizeof(struct CheckArrayRec));
}

int CopyCheckArray(pCheckArrayTo, pCheckArrayFrom)
   struct CheckArrayRec *pCheckArrayTo, *pCheckArrayFrom;
{
   int i;
   int num_cols=pCheckArrayFrom->num_cols, num_rows=pCheckArrayFrom->num_rows;

   memset(pCheckArrayTo, 0, sizeof(struct CheckArrayRec));
   pCheckArrayTo->num_cols = num_cols;
   pCheckArrayTo->num_rows = num_rows;
   if (pCheckArrayFrom->col_name != NULL) {
      /* there's an extra one column here! */
      pCheckArrayTo->col_name = (char**)malloc((num_cols+1)*sizeof(char*));
      if (pCheckArrayTo->col_name == NULL) FailAllocMessage();
      memset(pCheckArrayTo->col_name, 0, (num_cols+1)*sizeof(char*));

      for (i=0; i <= num_cols; i++) {
         if (pCheckArrayFrom->col_name[i] != NULL) {
            pCheckArrayTo->col_name[i] =
                  UtilStrDup(pCheckArrayFrom->col_name[i]);
            if (pCheckArrayTo->col_name[i] == NULL) FailAllocMessage();
         }
      }
   }
   if (pCheckArrayFrom->value != NULL) {
      pCheckArrayTo->value = (int**)malloc(num_cols*sizeof(int*));
      if (pCheckArrayTo->value == NULL) FailAllocMessage();
      memset(pCheckArrayTo->value, 0, num_cols*sizeof(int*));

      for (i=0; i < num_cols; i++) {
         if (pCheckArrayFrom->value[i] != NULL) {
            pCheckArrayTo->value[i] = (int*)malloc(num_rows*sizeof(int));
            if (pCheckArrayTo->value[i] == NULL) FailAllocMessage();
            memcpy(pCheckArrayTo->value[i], pCheckArrayFrom->value[i],
                  num_rows*sizeof(int));
         }
      }
   }
   return TRUE;
}

void NamesSetTitle(title_str)
   char *title_str;
{
   struct NamesRec *pni=(&namesInfo);

   if (pni->title_str != NULL) free(pni->title_str);
   if (pni->formatted_title_str != NULL) free(pni->formatted_title_str);
   pni->formatted_title_str = NULL;
   pni->title_str = UtilStrDup(title_str);
   if (pni->title_str == NULL) FailAllocMessage();

   if (boldMsgFontSet == NULL && boldMsgFontPtr == NULL) {
      CalcFormatStringInBox(pni->title_str, defaultFontPtr, defaultFontHeight,
            pni->graph_w, &pni->title_num_lines, &pni->title_w, &pni->title_h,
            &pni->formatted_title_str);
   } else {
      CalcFormatStringInBox(pni->title_str, boldMsgFontPtr, boldMsgFontHeight,
            pni->graph_w, &pni->title_num_lines, &pni->title_w, &pni->title_h,
            &pni->formatted_title_str);
   }
   CalcBaseWinWidth(pni);
}

void ResetNamesInfo()
{
   struct NamesRec *pni=(&namesInfo);

   if (pni->title_str != NULL) {
      free(pni->title_str);
      pni->title_str = NULL;
   }
   if (pni->formatted_title_str != NULL) {
      free(pni->formatted_title_str);
      pni->formatted_title_str = NULL;
   }
   if (!pni->dont_free_entries) {
      if (pni->dsp_ptr != NULL) {
         free(pni->dsp_ptr);
         pni->dsp_ptr = NULL;
      }
      if (pni->entries != NULL) {
         free(*(pni->entries));
         free(pni->entries);
         pni->entries = NULL;
      }
   }

   memset(&namesInfo, 0, sizeof(struct NamesRec));

   pni->num_entries = INVALID;
   pni->first_index = INVALID;
   pni->marked_index = INVALID;
   /* do not translate -- the string is used to measure things */
   pni->a_btn_w = ButtonWidth("OK", 8, NULL);
   pni->btn_gap = defaultFontWidth;
   pni->screen_w = DisplayWidth(mainDisplay, mainScreen);
   pni->screen_h = DisplayHeight(mainDisplay, mainScreen);
   pni->dsp_area_w = ITEM_LEN * defaultFontWidth;
   pni->dsp_area_h = ITEM_DSPED * ROW_HEIGHT;
   pni->scroll_area_h = pni->dsp_area_h;
   pni->dsp_base_win_w = pni->dsp_area_w + scrollBarW + (windowPadding<<1) +
         (brdrW<<2);
   pni->dsp_base_win_h = pni->dsp_area_h + (windowPadding<<1) + (brdrW<<1);
   pni->graph_w = pni->dsp_area_w + scrollBarW + (brdrW<<2);
   NamesSetTitle("");
   if (boldMsgFontSet == NULL && boldMsgFontPtr == NULL) {
      pni->base_win_h = (8+ITEM_DSPED)*ROW_HEIGHT;
   } else {
      pni->base_win_h = (6+ITEM_DSPED)*ROW_HEIGHT + 2*(boldMsgFontHeight+1);
   }
   pni->def_btn_id = BUTTON_OK;
   pni->double_click_btn_id = BUTTON_OK;
   pni->tabbed_from_root = TRUE;
}

void InitNamesInfo()
{
   XGCValues values;

   memset(&namesInfo, 0, sizeof(struct NamesRec));

   values.foreground = myFgPixel;
   values.background = (threeDLook ? myLtGryPixel : myBgPixel);
   values.fill_style = FillSolid;
   if (msgFontPtr == NULL) {
      values.font = defaultFontPtr->fid;
   } else {
      values.font = msgFontPtr->fid;
   }
   nameGC = XCreateGC(mainDisplay, rootWindow,
         GCForeground | GCBackground | GCFillStyle | GCFont, &values);

   values.foreground = myBgPixel;
   values.background = myFgPixel;
   revNameGC = XCreateGC(mainDisplay, rootWindow,
         GCForeground | GCBackground | GCFillStyle | GCFont, &values);
}

void CleanUpNamesInfo()
{
   struct NamesRec *pni=(&namesInfo);

   ResetNamesInfo();
   if (pni->title_str != NULL) free(pni->title_str);
   pni->title_str = NULL;
   if (pni->formatted_title_str != NULL) free(pni->formatted_title_str);
   pni->formatted_title_str = NULL;
   XFreeGC(mainDisplay, nameGC);
   XFreeGC(mainDisplay, revNameGC);
   nameGC = revNameGC = (GC)0;
}

void NamesAddButton(btn_str, btn_id)
   char *btn_str;
   int btn_id;
{
   struct NamesRec *pni=(&namesInfo);

   pni->btn_id[pni->num_btns] = btn_id;
   strcpy(pni->btn_str[pni->num_btns], btn_str);
   pni->btn_w += ButtonWidth(btn_str, 8, NULL);
   if (pni->num_btns != 0) {
      pni->btn_w += pni->btn_gap;
   }
   pni->num_btns++;
   if (pni->num_btns > MAXBUTTONS) {
#ifdef _TGIF_DBG /* debug, do not translate */
      TgAssert(FALSE,
            "Too many buttons detected in NamesAddButton().", NULL);
#endif /* _TGIF_DBG */
   }
   CalcBaseWinWidth(pni);
}

static
void RedrawNameScrollWin()
{
   struct NamesRec *pni=(&namesInfo);
   double frac, start_frac;
   int block_h, block_start;

   start_frac = (pni->num_entries > 0) ?
         (double)((double)(pni->first_index) /
         (double)(pni->num_entries)) : ((double)0.0);
   /* starting pixel */
   block_start = (int)(pni->scroll_area_h * start_frac);

   if (pni->num_entries > ITEM_DSPED) {
      frac = (double)((double)ITEM_DSPED / (double)(pni->num_entries));
   } else {
      frac = 1.0;
   }
   if (pni->first_index+ITEM_DSPED >= pni->num_entries) {
      block_h = pni->scroll_area_h - block_start;
   } else {
      block_h = (int)(pni->scroll_area_h * frac);
   }
   TgDrawScrollBar(mainDisplay, pni->scroll_win, VERT_SCROLLBAR,
         0, 0, scrollBarW, pni->scroll_area_h, start_frac, ITEM_DSPED,
         pni->num_entries);
}

#include "xbm/check.xbm"

void DrawCheckbox(dpy, win, gc, x, y, w, h, checked)
   Display *dpy;
   Window win;
   GC gc;
   int x, y, w, h, checked;
{
   if (threeDLook) {
      struct BBRec bbox;

      SetBBRec(&bbox, x, y, x+w+1, y+h+1);
      TgDrawThreeDButton(dpy, win, textMenuGC, &bbox, TGBS_LOWRED, 2, FALSE);
   } else {
      XDrawRectangle(dpy, win, gc, x, y, w, h);
   }
   if (checked) {
      if (threeDLook) {
         XGCValues values;

         values.fill_style = FillStippled;
         values.stipple = checkBitmap;
         values.ts_x_origin = x+2;
         values.ts_y_origin = y+2;
         XChangeGC(dpy, gc,
               GCFillStyle | GCStipple | GCTileStipXOrigin | GCTileStipYOrigin,
               &values);
         XFillRectangle(dpy, win, gc, values.ts_x_origin,
               values.ts_y_origin, check_width, check_height);
         values.fill_style = FillSolid;
         values.ts_x_origin = 0;
         values.ts_y_origin = 0;
         XChangeGC(dpy, gc,
               GCFillStyle | GCTileStipXOrigin | GCTileStipYOrigin, &values);
      } else {
         XDrawLine(dpy, win, gc, x, y, x+w, y+h);
         XDrawLine(dpy, win, gc, x, y+h, x+w, y);
      }
   }
   if (check_bits == NULL) { }
}

static
void RedrawItem(pni, index)
   struct NamesRec *pni;
   int index;
{
   int len=0, top=0, end=0, checkbox_cols=0, text_left=0, box_offset=0, box_w=0;
   int selected=(index==pni->marked_index);

   if ((msgFontSet==NULL && msgFontPtr==NULL)) {
      top = defaultFontAsc+1;
   } else {
      top = msgFontAsc+1;
   }
   if (pni->first_index+ITEM_DSPED > pni->num_entries) {
      end = pni->num_entries;
   } else {
      end = pni->first_index + ITEM_DSPED;
   }
   if (pni->p_check_array != NULL && pni->p_check_array->num_cols > 0) {
      checkbox_cols = pni->p_check_array->num_cols;
      text_left = checkbox_cols*ROW_HEIGHT;
      box_offset = 1;
      box_w = (ROW_HEIGHT-((box_offset+1)<<1));
   }
   len = strlen(pni->entries[index]);
   XFillRectangle(mainDisplay, pni->dsp_win,
         (selected ? nameGC : revNameGC), 0,
         (index-pni->first_index)*ROW_HEIGHT, ITEM_LEN*defaultFontWidth,
         ROW_HEIGHT);
   DrawMsgString(mainDisplay, pni->dsp_win,
         (selected ? revNameGC : nameGC), text_left,
         (index-pni->first_index)*ROW_HEIGHT+top, pni->entries[index], len);
   if (checkbox_cols > 0) {
      int col=0, cur_x=0;

      for (col=0; col < pni->p_check_array->num_cols; col++) {
         DrawCheckbox(mainDisplay, pni->dsp_win,
               (selected ? revNameGC : nameGC), cur_x+box_offset,
               (index-pni->first_index)*ROW_HEIGHT+top-box_w,
               box_w, box_w, pni->p_check_array->value[col][index]);
         cur_x += ROW_HEIGHT;
      }
   }
}

static
void RedrawDspWindow()
{
   struct NamesRec *pni=(&namesInfo);
   int i, end=0;

   if (pni->first_index+ITEM_DSPED > pni->num_entries) {
      end = pni->num_entries;
   } else {
      end = pni->first_index + ITEM_DSPED;
   }
   XFillRectangle(mainDisplay, pni->dsp_win, revNameGC, 0, 0,
         ITEM_LEN*defaultFontWidth, ITEM_DSPED*ROW_HEIGHT);

   for (i=pni->first_index; i < end; i++) {
      RedrawItem(pni, i);
   }
}

static
void RedrawDspBaseWindow()
{
   if (threeDLook) {
      struct NamesRec *pni=(&namesInfo);
      struct BBRec bbox;

      SetBBRec(&bbox, 0, 0, pni->dsp_base_win_w, pni->dsp_base_win_h);
      TgDrawThreeDButton(mainDisplay, pni->dsp_base_win, textMenuGC, &bbox,
            TGBS_LOWRED, 2, FALSE);
   }
}

static
void RedrawNamePath()
{
   struct NamesRec *pni=(&namesInfo);
   int len=strlen(pni->inbuf), cursor_x=0, cursor_y, box_w, box_h, x, y;
   char *c_ptr;
   struct BBRec bbox;

   if (msgFontPtr == NULL) {
      XSetFont(mainDisplay, nameGC, defaultFontPtr->fid);
   } else {
      XSetFont(mainDisplay, nameGC, msgFontPtr->fid);
   }
   x = pni->graph_start;
   /* the path_bbox has 2 pixels around it */
   y = pni->path_bbox.lty+2;

   box_w = ITEM_LEN*defaultFontWidth+6+(windowPadding<<1);
   cursor_y = y;
   if ((msgFontSet==NULL && msgFontPtr==NULL)) {
      y += defaultFontAsc;
   } else {
      y += msgFontAsc;
   }
   box_h = (ROW_HEIGHT+3);
   if (threeDLook) {
      XSetForeground(mainDisplay, textMenuGC, myBgPixel);
      XFillRectangle(mainDisplay, pni->base_win, textMenuGC, x, cursor_y-2,
            box_w+(windowPadding<<1), box_h+(windowPadding<<1));
      SetBBRec(&bbox, x, cursor_y-2, x+box_w+(windowPadding<<1),
            cursor_y-2+box_h+(windowPadding<<1));
      TgDrawThreeDButton(mainDisplay, pni->base_win, textMenuGC, &bbox,
            TGBS_LOWRED, 2, FALSE);
   } else {
      XClearArea(mainDisplay, pni->base_win, x, cursor_y-2, box_w+1, box_h,
            False);
      XDrawRectangle(mainDisplay, pni->base_win, nameGC, x, cursor_y-2,
            box_w+1, box_h);
   }
   if ((msgFontSet==NULL && msgFontPtr==NULL)) {
      if (len > ITEM_LEN) {
         c_ptr = &(pni->inbuf[len-ITEM_LEN]);
         len = ITEM_LEN;
      } else {
         c_ptr = pni->inbuf;
      }
      cursor_x = x+2+len*defaultFontWidth;
   } else {
      int index=0;

      cursor_x = MsgTextWidth(msgFontPtr, pni->inbuf, len);
      while (cursor_x > box_w-4) {
         index++;
         cursor_x = MsgTextWidth(msgFontPtr, &pni->inbuf[index], len-index);
      }
      c_ptr = &pni->inbuf[index];
      len -= index;
      cursor_x += x+2+1;
   }
   DrawMsgString(mainDisplay, pni->base_win, nameGC, x+2+windowPadding,
         y+1+windowPadding, c_ptr, len);
   XDrawLine(mainDisplay, pni->base_win, nameGC, cursor_x+windowPadding,
         cursor_y+1+windowPadding, cursor_x+windowPadding,
         cursor_y+box_h-5+windowPadding);
}

static
void RedrawNameBaseWindow()
{
   struct NamesRec *pni=(&namesInfo);
   int i, left, base_line;

   /* draw a frame around the window */
   if (threeDLook) {
      if (dialogboxUse3DBorder) {
         struct BBRec bbox;

         SetBBRec(&bbox, 0, 0, pni->base_win_w, pni->base_win_h);
         TgDrawThreeDButton(mainDisplay, pni->base_win, textMenuGC, &bbox,
               TGBS_RAISED, 2, FALSE);
      }
   } else {
      XDrawRectangle(mainDisplay, pni->base_win, nameGC, 0, 0,
            pni->base_win_w-1, pni->base_win_h-1);
   }
   if (boldMsgFontSet == NULL && boldMsgFontPtr == NULL) {
      base_line = defaultFontAsc+ROW_HEIGHT;
      XSetFont(mainDisplay, nameGC, defaultFontPtr->fid);
   } else {
      base_line = boldMsgFontAsc+ROW_HEIGHT;
      if (boldMsgFontPtr != NULL) {
         XSetFont(mainDisplay, nameGC, boldMsgFontPtr->fid);
      }
   }
   /* draw the title */
   if (pni->tabbed_from_root) {
      if (pni->title_num_lines <= 1) {
         DrawBoldMsgString(mainDisplay, pni->base_win, nameGC, pni->title_start,
               base_line, pni->title_str, strlen(pni->title_str));
      } else {
         char *psz_line=pni->formatted_title_str;
         char *psz=strchr(pni->formatted_title_str, '\n');

         do {
            if (psz != NULL) *psz = '\0';
            DrawBoldMsgString(mainDisplay, pni->base_win, nameGC,
                  pni->title_start, base_line, psz_line, strlen(psz_line));
            base_line += ((boldMsgFontSet==NULL && boldMsgFontPtr==NULL) ?
                  defaultFontHeight : boldMsgFontHeight);
            if (psz != NULL) {
               *psz++ = '\n';
               psz_line = psz;
               psz = strchr(psz_line, '\n');
            } else {
               break;
            }
         } while (psz_line != NULL);
      }
   } else {
      int tmp_w, len, tmp_start;

      strcpy(gszMsgBox, TgLoadString(STID_TAB_FOR_FILE_COMPLETION));
      len = strlen(gszMsgBox);
      if (boldMsgFontSet == NULL && boldMsgFontPtr == NULL) {
         tmp_w = defaultFontWidth * len;
      } else {
         tmp_w = BoldMsgTextWidth(boldMsgFontPtr, gszMsgBox, len);
      }
      tmp_start = ((pni->base_win_w-tmp_w)>>1);
      DrawBoldMsgString(mainDisplay, pni->base_win, nameGC, tmp_start,
            base_line, gszMsgBox, len);
      if (pni->title_str != NULL && *pni->title_str != '\0') {
         SetStringStatus(pni->title_str);
      }
   }
   if (boldMsgFontSet != NULL || boldMsgFontPtr != NULL) {
      if (msgFontPtr == NULL) {
         XSetFont(mainDisplay, nameGC, defaultFontPtr->fid);
      } else {
         XSetFont(mainDisplay, nameGC, msgFontPtr->fid);
      }
   }
   RedrawNamePath();

   /* draw the buttons */
   left = pni->btn_start;
   for (i=0; i < pni->num_btns; i++) {
      int len=strlen(pni->btn_str[i]);
      int button_w=ButtonWidth(pni->btn_str[i], 8, NULL), button_h=0;

      if (boldMsgFontSet == NULL && boldMsgFontPtr == NULL) {
         button_h = defaultFontHeight + 4;
      } else {
         button_h = boldMsgFontHeight + 4;
      }
      pni->btn_bbox[i].ltx = left;
      pni->btn_bbox[i].lty = (5+ITEM_DSPED)*ROW_HEIGHT + pni->title_h;
      pni->btn_bbox[i].lty -= 1;
      pni->btn_bbox[i].rbx = pni->btn_bbox[i].ltx+button_w;
      pni->btn_bbox[i].rby = pni->btn_bbox[i].lty+button_h;
      DisplayButtonInBBox(pni->base_win, pni->btn_str[i],
            len, &(pni->btn_bbox[i]), BUTTON_NORMAL, FALSE, 0, NULL);
      left = pni->btn_bbox[i].rbx + 1 + defaultFontWidth;
   }
}

static
void RedrawNamesWindow()
{
   RedrawNameBaseWindow();
   RedrawNameScrollWin();
   RedrawDspWindow();
   RedrawDspBaseWindow();
}

static
int ExposeOrMapNames(input)
   XEvent *input;
{
   struct NamesRec *pni=(&namesInfo);
   XEvent ev;

   if ((input->type==MapNotify && input->xany.window==pni->base_win) ||
         (input->type==Expose && (input->xany.window==pni->base_win ||
         input->xany.window==pni->scroll_win ||
         input->xany.window==pni->dsp_win)) || (!pni->exposed &&
         (XCheckWindowEvent(mainDisplay, pni->base_win, ExposureMask,
         &ev) || XCheckWindowEvent(mainDisplay, pni->scroll_win,
         ExposureMask, &ev) || XCheckWindowEvent(mainDisplay, pni->dsp_win,
         ExposureMask, &ev) || XCheckWindowEvent(mainDisplay,
         pni->base_win, StructureNotifyMask, &ev)))) {
      while (XCheckWindowEvent(mainDisplay, pni->base_win,
            ExposureMask, &ev)) ;
      while (XCheckWindowEvent(mainDisplay, pni->scroll_win,
            ExposureMask, &ev)) ;
      while (XCheckWindowEvent(mainDisplay, pni->dsp_win,
            ExposureMask, &ev)) ;
      while (XCheckWindowEvent(mainDisplay, pni->base_win,
            StructureNotifyMask, &ev)) ;

      RedrawNamesWindow();

      pni->exposed = TRUE;
      XSync(mainDisplay, False);

      if ((input->type==MapNotify &&
            input->xany.window==pni->base_win) ||
            (input->type==Expose &&
            (input->xany.window==pni->base_win ||
            input->xany.window==pni->scroll_win ||
            input->xany.window==pni->dsp_win))) {
         return TRUE;
      }
   }
   return FALSE;
}

static
int CreateNamesWindows(win_name)
   char *win_name;
{
   struct NamesRec *pni=(&namesInfo);
   int y, base_win_x, base_win_y;
   int bg_pixel=(threeDLook ? myLtGryPixel : myBgPixel);
   XWMHints wmhints;
   XSizeHints sizehints;
   XSetWindowAttributes win_attrs;

   base_win_x = (pni->base_win_w > pni->screen_w) ? 0 :
         ((pni->screen_w-pni->base_win_w)>>1);
   base_win_y = (pni->base_win_h > pni->screen_h) ? 0 :
         ((pni->screen_h-pni->base_win_h)/3);

   if ((pni->base_win=XCreateSimpleWindow(mainDisplay, rootWindow,
         base_win_x, base_win_y, pni->base_win_w, pni->base_win_h,
         brdrW, myBorderPixel, bg_pixel)) == 0) {
      return FailToCreateWindowMessage("CreateNamesWindows()", NULL, FALSE);
   }
   XDefineCursor(mainDisplay, pni->base_win, defaultCursor);

   y = (ROW_HEIGHT<<2)+pni->title_h;
   if ((pni->dsp_base_win=XCreateSimpleWindow(mainDisplay, pni->base_win,
         pni->graph_start, y, pni->dsp_base_win_w, pni->dsp_base_win_h,
         0, myBorderPixel, bg_pixel)) == 0) {
      XDestroyWindow(mainDisplay, pni->base_win);
      return FailToCreateWindowMessage("CreateNamesWindows()", NULL, FALSE);
   }
   if ((pni->dsp_win=XCreateSimpleWindow(mainDisplay, pni->dsp_base_win,
         windowPadding, windowPadding, pni->dsp_area_w, pni->dsp_area_h,
         brdrW, myBorderPixel, bg_pixel)) == 0) {
      XDestroyWindow(mainDisplay, pni->base_win);
      return FailToCreateWindowMessage("CreateNamesWindows()", NULL, FALSE);
   }
   if ((pni->scroll_win=XCreateSimpleWindow(mainDisplay,
         pni->dsp_base_win, windowPadding+pni->dsp_area_w+(brdrW<<1),
         windowPadding, scrollBarW, pni->dsp_area_h,
         brdrW, myBorderPixel, bg_pixel)) == 0) {
      XDestroyWindow(mainDisplay, pni->base_win);
      return FailToCreateWindowMessage("CreateNamesWindows()", NULL, FALSE);
   }
   win_attrs.save_under = True;
   win_attrs.colormap = mainColormap;
   XChangeWindowAttributes(mainDisplay, pni->base_win,
         CWSaveUnder | CWColormap, &win_attrs);

   wmhints.flags = InputHint | StateHint;
   wmhints.input = True;
   wmhints.initial_state = NormalState;
   XSetWMHints(mainDisplay, pni->base_win, &wmhints);

   sizehints.flags = PPosition | PSize | USPosition | PMinSize | PMaxSize;
   sizehints.x = base_win_x;
   sizehints.y = base_win_y;
   sizehints.width = sizehints.min_width = sizehints.max_width =
         pni->base_win_w;
   sizehints.height = sizehints.min_height = sizehints.max_height =
         pni->base_win_h;
#ifdef NOTR4MODE
   XSetNormalHints(mainDisplay, pni->base_win, &sizehints);
#else
   XSetWMNormalHints(mainDisplay, pni->base_win, &sizehints);
#endif
   RegisterWM_DELETE_WINDOW(pni->base_win);
   XStoreName(mainDisplay, pni->base_win, win_name);

   XSetTransientForHint(mainDisplay, pni->base_win, mainWindow);

#ifdef MAPBEFORESELECT
   XMapWindow(mainDisplay, pni->base_win);
   XSelectInput(mainDisplay, pni->base_win,
         KeyPressMask | ButtonPressMask | ExposureMask | StructureNotifyMask);
   XMapWindow(mainDisplay, pni->dsp_base_win);
   XSelectInput(mainDisplay, pni->dsp_base_win, ExposureMask);
   XMapWindow(mainDisplay, pni->dsp_win);
   XSelectInput(mainDisplay, pni->dsp_win,
         KeyPressMask | ButtonPressMask | ExposureMask);
   XMapWindow(mainDisplay, pni->scroll_win);
   XSelectInput(mainDisplay, pni->scroll_win,
         KeyPressMask | ButtonPressMask | ExposureMask);
#else
   XSelectInput(mainDisplay, pni->base_win,
         KeyPressMask | ButtonPressMask | ExposureMask | StructureNotifyMask);
   XMapWindow(mainDisplay, pni->base_win);
   XSelectInput(mainDisplay, pni->dsp_base_win, ExposureMask);
   XMapWindow(mainDisplay, pni->dsp_base_win);
   XSelectInput(mainDisplay, pni->dsp_win,
         KeyPressMask | ButtonPressMask | ExposureMask);
   XMapWindow(mainDisplay, pni->dsp_win);
   XSelectInput(mainDisplay, pni->scroll_win,
         KeyPressMask | ButtonPressMask | ExposureMask);
   XMapWindow(mainDisplay, pni->scroll_win);
#endif

   if (warpToWinCenter) {
      XWarpPointer(mainDisplay, None, pni->base_win, 0, 0, 0, 0,
            (pni->base_win_w>>1), (pni->base_win_h>>1));
   }
   XSync(mainDisplay, False);

   Msg("");

   return TRUE;
}

static
void NamesUpdateIndices()
{
   struct NamesRec *pni=(&namesInfo);

   pni->first_index = 0;
   if (pni->marked_index == INVALID) {
      if (pni->edit_style != NAMES_SELECT_FILE) {
         *pni->inbuf = '\0';
         pni->buf_index = 0;
      }
   } else {
      if (pni->marked_index >= ITEM_DSPED) {
         if (pni->marked_index < pni->num_entries-ITEM_DSPED) {
            pni->first_index = pni->marked_index;
         } else {
            pni->first_index = pni->num_entries-ITEM_DSPED;
         }
      }
      if (pni->edit_style != NAMES_SELECT_FILE) {
         strcpy(pni->inbuf, &(pni->entries[pni->marked_index])[pni->leading]);
         pni->buf_index = strlen(pni->inbuf);;
      }
   }
}

static
int IsFirstEqChar(s, index)
   char *s;
   int index;
{
   s = (&s[index]);
   if (*s != '=') return FALSE;
   for (--index, --s; index >= 0; index--, s--) {
      if (*s == '=') {
         return FALSE;
      }
   }
   return TRUE;
}

static
int SetMarkedIndex()
{
   struct NamesRec *pni=(&namesInfo);
   int i;

   if (*pni->name == '\0') return FALSE;
   for (i=0; i < pni->num_entries; i++) {
      if (strncmp(&(pni->entries[i])[pni->leading],
            pni->name, pni->name_index) == 0) {
         break;
      }
   }
   if (i < pni->num_entries) {
      if (i < pni->first_index) {
         pni->first_index = i;
      } else if (i >= pni->first_index+ITEM_DSPED) {
         if (i < pni->num_entries-ITEM_DSPED) {
            pni->first_index = i;
         } else {
            pni->first_index = pni->num_entries-ITEM_DSPED;
         }
      }
      pni->marked_index = i;
      return TRUE;
   } else {
      return FALSE;
   }
}

static
void BackSpaceInNames(pn_changing, pn_selected_btn_index)
   int *pn_changing, *pn_selected_btn_index;
{
   struct NamesRec *pni=(&namesInfo);
   int i;

   switch (pni->edit_style) {
   case NAMES_SIMPLE_SELECT_NAME: return;
   case NAMES_COMPLEX_SELECT_NAME:
      if (pni->marked_index == INVALID || pni->buf_index <= 0) return;
      pni->inbuf[--pni->buf_index] = '\0';
      if (pni->buf_index != 0) {
         int i;

         for (i=0; i < pni->num_entries; i++) {
            if (strncmp(&(pni->entries[i])[pni->leading],
                  pni->inbuf, pni->buf_index) == 0) {
               break;
            }
         }
         if (i < pni->num_entries) {
            if (i < pni->first_index) {
               pni->first_index = i;
            } else if (i >= pni->first_index+ITEM_DSPED) {
               if (i < pni->num_entries-ITEM_DSPED) {
                  pni->first_index = i;
               } else {
                  pni->first_index = pni->num_entries-ITEM_DSPED;
               }
            }
            pni->marked_index = i;
         }
      } else {
         pni->first_index = 0;
         pni->marked_index = INVALID;
      }
      break;
   case NAMES_SELECT_FILE:
      if (pni->name_index > 0) {
         pni->name[--pni->name_index] = '\0';
         sprintf(pni->inbuf, "%s%c%s", pni->dir_name, DIR_SEP, pni->name);
         pni->buf_index = strlen(pni->inbuf);
         if (pni->tabbed_from_root && pni->name_index != 0) {
            for (i=0; i < pni->num_entries; i++) {
               if (strncmp(&(pni->entries[i])[pni->leading],
                     pni->name, pni->name_index) == 0) {
                  pni->marked_index = i;
                  break;
               }
            }
         } else {
            pni->marked_index = INVALID;
         }
      } else if (pni->change_to_root && *pni->dir_name == '\0' &&
            *pni->saved_dir_name != '\0') {
         pni->pop_from_root = TRUE;
         strcpy(pni->dir_name, pni->saved_dir_name);
         *pni->saved_dir_name = '\0';
         pni->tabbed_from_root = TRUE;
         pni->change_to_root = FALSE;
         sprintf(pni->inbuf, "%s%c%s", pni->dir_name, DIR_SEP, pni->name);
         pni->buf_index = strlen(pni->inbuf);
         *pn_changing = FALSE;
         *pn_selected_btn_index = INVALID;
      } else {
         /* fake as if "../" is selected */
         sprintf(pni->name, "..%c", DIR_SEP);
         pni->name_index = strlen(pni->name);
         for (i=0; i < pni->num_entries; i++) {
            if (strncmp(&(pni->entries[i])[pni->leading],
                  pni->name, pni->name_index) == 0) {
               pni->marked_index = i;
               pni->faking_dot_dot = TRUE;
               *pn_changing = FALSE;
               *pn_selected_btn_index = i;
               break;
            }
         }
      }
      break;
   case NAMES_EDIT_ATTR:
      if (pni->marked_index == INVALID || pni->buf_index <= 0) return;
      if (pni->dsp_ptr == NULL ||
            !pni->dsp_ptr[pni->marked_index].directory ||
            !IsFirstEqChar(pni->inbuf, pni->buf_index-1)) {
         pni->inbuf[--pni->buf_index] = '\0';
         strcpy(&(pni->entries[pni->marked_index])[pni->leading],
               pni->inbuf);
      }
      break;
   case NAMES_EDIT_NAME:
      if (pni->marked_index == INVALID || pni->buf_index <= 0) return;
      pni->inbuf[--pni->buf_index] = '\0';
      strcpy(&(pni->entries[pni->marked_index])[pni->leading],
            pni->inbuf);
      break;
   default: break;
   }
   if (pni->exposed) {
      RedrawNamePath();
      RedrawDspWindow();
   }
}

static
void ParseFileName(FullName, DirName, Name)
   char *FullName, *DirName, *Name;
{
   int i, len;

   len = strlen(FullName);
   strcpy(DirName, FullName);
   for (i=len-1; i>=0; i--) {
      if (DirName[i] == DIR_SEP) {
         strcpy(Name, &(DirName[i+1]));
         DirName[i] = '\0';
         return;
      }
   }
   *DirName = *Name = '\0';
}

static
void TabInNames()
{
   struct NamesRec *pni=(&namesInfo);

   if (pni->edit_style == NAMES_SELECT_FILE) {
      pni->tabbed_from_root = TRUE;
      pni->just_tabbed_from_root = TRUE;
      if (!FileIsRemote(pni->name)) {
         /* saved_name is used for just_tabbed_from_root */
         sprintf(pni->inbuf, "%s%c%s", pni->dir_name, DIR_SEP, pni->name);
         pni->buf_index = strlen(pni->inbuf);
         strcpy(pni->saved_name, pni->name);
         ParseFileName(pni->inbuf, pni->dir_name, pni->name);
      } else {
         pni->remote_file = TRUE;
      }
   }
}

static
void CharInNames(buf, pn_changing, pn_selected_btn_index)
   char *buf;
   int *pn_changing, *pn_selected_btn_index;
{
   struct NamesRec *pni=(&namesInfo);
   int i=INVALID;

   switch (pni->edit_style) {
   case NAMES_SIMPLE_SELECT_NAME:
      if (buf[0] == ' ' && buf[1] == '\0' &&
            pni->p_check_array != NULL && pni->p_check_array->num_cols == 1) {
         i = pni->marked_index;
         pni->p_check_array->value[0][i] = !(pni->p_check_array->value[0][i]);
         RedrawItem(pni, i);
      }
      return;

   case NAMES_COMPLEX_SELECT_NAME:
      if (buf[0] == '$') {
         i = pni->num_entries-1;
         strcpy(pni->inbuf, &(pni->entries[i])[pni->leading]);
      } else if (buf[0] == '^') {
         i = 0;
         strcpy(pni->inbuf, &(pni->entries[i])[pni->leading]);
      } else {
         pni->inbuf[pni->buf_index++] = buf[0];
         pni->inbuf[pni->buf_index] = '\0';
         for (i=0; i < pni->num_entries; i++) {
            if (strncmp(&(pni->entries[i])[pni->leading],
                  pni->inbuf, pni->buf_index) == 0) {
               break;
            }
         }
      }
      if (i < pni->num_entries) {
         if (i < pni->first_index) {
            pni->first_index = i;
         } else if (i >= pni->first_index+ITEM_DSPED) {
            if (i < pni->num_entries-ITEM_DSPED) {
               pni->first_index = i;
            } else {
               pni->first_index = pni->num_entries-ITEM_DSPED;
            }
         }
         pni->marked_index = i;
      } else {
         pni->inbuf[--pni->buf_index] = '\0';
      }
      break;
   case NAMES_SELECT_FILE:
      if (!pni->tabbed_from_root) {
         pni->name[pni->name_index++] = buf[0];
         pni->name[pni->name_index] = '\0';
         sprintf(pni->inbuf, "%s%c%s", pni->dir_name, DIR_SEP, pni->name);
         pni->buf_index = strlen(pni->inbuf);
      } else if (buf[0] == '$') {
         i = pni->num_entries-1;
         strcpy(pni->name, &(pni->entries[i])[pni->leading]);
         pni->name_index = strlen(pni->name);
      } else if (buf[0] == '^') {
         i = 0;
         strcpy(pni->name, &(pni->entries[i])[pni->leading]);
         pni->name_index = strlen(pni->name);
#ifdef apollo
      } else if (pni->name_index == 0 && buf[0] == DIR_SEP &&
            *pni->dir_name == '\0') {
         strcpy(pni->dir_name, DIR_SEP_STR);
         pni->name[pni->name_index++] = buf[0];
         i = pni->num_entries;
         strcpy(pni->inbuf, "//");
#endif /* apollo */
      } else if (pni->name_index == 0 && buf[0] == DIR_SEP) {
         if (*pni->saved_dir_name == '\0') {
            strcpy(pni->saved_dir_name, pni->dir_name);
            pni->num_entries = 0;
         }
         *pn_changing = FALSE;
         pni->change_to_root = TRUE;
         pni->tabbed_from_root = FALSE;
         *pn_selected_btn_index = INVALID;
         *pni->dir_name = *pni->name = '\0';
         pni->name_index = 0;
         sprintf(pni->inbuf, "%s%c%s", pni->dir_name, DIR_SEP, pni->name);
         pni->buf_index = strlen(pni->inbuf);
         break;
      } else {
         pni->name[pni->name_index++] = buf[0];
         pni->name[pni->name_index] = '\0';
         for (i=0; i < pni->num_entries; i++) {
            if (strncmp(&(pni->entries[i])[pni->leading],
                  pni->name, pni->name_index) == 0) {
               break;
            }
         }
      }
      if (pni->num_entries > 0) {
         if (i < pni->num_entries) {
            if (i < pni->first_index) {
               pni->first_index = i;
            } else if (i >= pni->first_index+ITEM_DSPED) {
               if (i < pni->num_entries-ITEM_DSPED) {
                  pni->first_index = i;
               } else {
                  pni->first_index = pni->num_entries-ITEM_DSPED;
               }
            }
            pni->marked_index = i;
            sprintf(pni->inbuf, "%s%c%s", pni->dir_name, DIR_SEP, pni->name);
            pni->buf_index = strlen(pni->inbuf);
            if (buf[0] == DIR_SEP) {
               *pn_changing = FALSE;
               *pn_selected_btn_index = INVALID;
            }
         } else {
            pni->name[--pni->name_index] = '\0';
         }
      }
      break;
   case NAMES_EDIT_ATTR:
      if (pni->marked_index == INVALID) return;
      pni->inbuf[pni->buf_index++] = buf[0];
      pni->inbuf[pni->buf_index] = '\0';
      strcpy(&(pni->entries[pni->marked_index])[pni->leading],
            pni->inbuf);
      break;
   case NAMES_EDIT_NAME:
      if (pni->marked_index == INVALID) return;
      pni->inbuf[pni->buf_index++] = buf[0];
      pni->inbuf[pni->buf_index] = '\0';
      strcpy(&(pni->entries[pni->marked_index])[pni->leading],
            pni->inbuf);
      break;
   default: break;
   }
   if (pni->exposed) {
      RedrawNamePath();
      RedrawDspWindow();
   }
}

static
int GetNameEntryNum(RowOffset)
   int RowOffset;
{
   struct NamesRec *pni=(&namesInfo);
   int index;

   index = pni->first_index + RowOffset;
   if (index >= pni->num_entries) return INVALID;
   return index;
}

static
int ScrollItemCallback(pv_userdata)
   void *pv_userdata;
   /* returns TRUE to cancel scrolling */
{
   struct NamesRec *pni=((struct NamesRec *)pv_userdata);

   if (pni->scroll_dir == SCRL_UP) {
      if (pni->first_index == 0) return FALSE;
      pni->first_index--;
   } else {
      if (pni->num_entries <= ITEM_DSPED ||
            pni->first_index+ITEM_DSPED == pni->num_entries) {
         return FALSE;
      }
      pni->first_index++;
   }
   RedrawNameScrollWin();
   RedrawDspWindow();
   XSync(mainDisplay, False);

   return FALSE;
}

static
int ScrollPageCallback(pv_userdata)
   void *pv_userdata;
   /* returns TRUE to cancel scrolling */
{
   struct NamesRec *pni=((struct NamesRec *)pv_userdata);

   if (pni->scroll_dir == SCRL_UP) {
      if (pni->first_index == 0) return FALSE;
      pni->first_index -= ITEM_DSPED;
      if (pni->first_index < 0) pni->first_index = 0;
   } else {
      if (pni->num_entries <= ITEM_DSPED ||
            pni->first_index+ITEM_DSPED == pni->num_entries) {
         return FALSE;
      }
      pni->first_index += ITEM_DSPED;
      if (pni->first_index+ITEM_DSPED >= pni->num_entries) {
         pni->first_index = pni->num_entries-ITEM_DSPED;
      }
   }
   RedrawNameScrollWin();
   RedrawDspWindow();
   XSync(mainDisplay, False);

   return FALSE;
}

static
int DoNameBtnScroll(scroll_page, scroll_dir, pbbox, pni)
   int scroll_page, scroll_dir;
   struct BBRec *pbbox;
   struct NamesRec *pni;
   /* returns TRUE if done scrolling */
{
   ScrollBtnCallbackInfo sbci;

   pni->scroll_dir = scroll_dir;
   memset(&sbci, 0, sizeof(ScrollBtnCallbackInfo));

   if (scroll_page) {
      sbci.ms = 200;
      sbci.pv_userdata = pni;
      sbci.pf_scroll_btn_callback = ScrollPageCallback;
      if (TgPressButtonLoop(mainDisplay, pni->scroll_win, NULL,
            &sbci)) {
         if (scroll_dir == SCRL_UP) {
            if (pni->first_index == 0) return TRUE;
            pni->first_index -= ITEM_DSPED;
            if (pni->first_index < 0) pni->first_index = 0;
         } else {
            if (pni->num_entries <= ITEM_DSPED ||
                  pni->first_index+ITEM_DSPED == pni->num_entries) {
               return TRUE;
            }
            pni->first_index += ITEM_DSPED;
            if (pni->first_index+ITEM_DSPED >= pni->num_entries) {
               pni->first_index = pni->num_entries-ITEM_DSPED;
            }
         }
      }
   } else {
      sbci.ms = 50;
      sbci.pv_userdata = pni;
      sbci.pf_scroll_btn_callback = ScrollItemCallback;
      if (TgPressButtonLoop(mainDisplay, pni->scroll_win, pbbox, &sbci)) {
         if (scroll_dir == SCRL_UP) {
            if (pni->first_index == 0) return TRUE;
            pni->first_index--;
         } else {
            if (pni->num_entries <= ITEM_DSPED ||
                  pni->first_index+ITEM_DSPED == pni->num_entries) {
               return TRUE;
            }
            pni->first_index++;
         }
      }
   }
   return FALSE;
}

static
void NameScrollHandler(button_ev)
   XButtonEvent *button_ev;
{
   struct NamesRec *pni=(&namesInfo);
   double frac, start_frac;
   int block_h, block_start, do_drag=FALSE, btn_offset=0;

   if (!threeDLook && button_ev->button == Button3 &&
         button_ev->type == ButtonPress) {
      if (DoNameBtnScroll(((button_ev->state & (ShiftMask|ControlMask)) != 0),
            SCRL_UP, NULL, pni)) {
         return;
      }
   } else if (!threeDLook && button_ev->button == Button1 &&
         button_ev->type == ButtonPress) {
      if (DoNameBtnScroll(((button_ev->state & (ShiftMask|ControlMask)) != 0),
            SCRL_DN, NULL, pni)) {
         return;
      }
   } else if (button_ev->button == Button1 && button_ev->type == ButtonPress) {
      if (button_ev->y < scrollBarW ||
            button_ev->y >= pni->scroll_area_h-scrollBarW) {
         int which=INVALID;
         struct BBRec bbox;

         if (button_ev->y < scrollBarW) {
            which = SCRL_UP;
            SetBBRec(&bbox, 0, 0, scrollBarW, scrollBarW);
         } else {
            which = SCRL_DN;
            SetBBRec(&bbox, 0, pni->scroll_area_h-scrollBarW, scrollBarW,
                  pni->scroll_area_h);
         }
         if (DoNameBtnScroll(FALSE, which, &bbox, pni)) {
            return;
         }
      } else {
         int hit=0;

         start_frac = (pni->num_entries > 0) ?
               (double)((double)(pni->first_index) /
               (double)(pni->num_entries)) : ((double)0.0);
         hit = TgGetScrollHit(button_ev->x, button_ev->y, VERT_SCROLLBAR,
               scrollBarW, pni->scroll_area_h, start_frac, ITEM_DSPED,
               pni->num_entries, &btn_offset);
         if (hit == 0) {
            do_drag = TRUE;
         } else {
            if (DoNameBtnScroll(TRUE, (hit < 0 ? SCRL_UP : SCRL_DN), NULL,
                  pni)) {
               return;
            }
         }
      }
   } else if (!threeDLook && button_ev->button == Button2 &&
         button_ev->type == ButtonPress) {
      do_drag = TRUE;
   }
   if (do_drag) {
      int done=FALSE;
      XEvent ev;

      if (pni->num_entries <= ITEM_DSPED) return;

      frac = (double)((double)ITEM_DSPED / (double)(pni->num_entries));
      block_h = (int)(pni->scroll_area_h * frac);
      if (threeDLook) {
         block_start = button_ev->y+btn_offset;;
         start_frac = (double)((double)(block_start-scrollBarW) /
               (double)(pni->scroll_area_h-(scrollBarW<<1)));
         if (block_start+block_h >= pni->scroll_area_h-scrollBarW) {
            pni->first_index = pni->num_entries - ITEM_DSPED;
         } else {
            pni->first_index = (int)(pni->num_entries * start_frac);
         }
      } else {
         block_start = button_ev->y;
         start_frac = (double)((double)(block_start) /
               (double)(pni->scroll_area_h));
         if (block_start+block_h >= pni->scroll_area_h) {
            pni->first_index = pni->num_entries - ITEM_DSPED;
         } else {
            pni->first_index = (int)(pni->num_entries * start_frac);
         }
      }
      RedrawNameScrollWin();
      RedrawDspWindow();

      XGrabPointer(mainDisplay, pni->scroll_win, False,
            PointerMotionMask | ButtonReleaseMask, GrabModeAsync,
            GrabModeAsync, None, handCursor, CurrentTime);

      while (!done) {
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
                    ((double)(pni->scroll_area_h-(scrollBarW<<1))));

               if (y <= scrollBarW) {
                  new_name_first = 0;
               } else if (y+block_h >= pni->scroll_area_h-scrollBarW) {
                  new_name_first = pni->num_entries - ITEM_DSPED;
               } else {
                  new_name_first = (int)(pni->num_entries * start_frac);
               }
            } else {
               start_frac = (double)(((double)y) /
                    ((double)pni->scroll_area_h));

               if (y <= 0) {
                  new_name_first = 0;
               } else if (y+block_h >= pni->scroll_area_h) {
                  new_name_first = pni->num_entries - ITEM_DSPED;
               } else {
                  new_name_first = (int)(pni->num_entries * start_frac);
               }
            }
            if (pni->first_index != new_name_first) {
               pni->first_index = new_name_first;
               RedrawNameScrollWin();
               RedrawDspWindow();
            }
            while (XCheckMaskEvent(mainDisplay, PointerMotionMask, &ev)) ;
         }
      }
      return;
   }
   RedrawNameScrollWin();
   RedrawDspWindow();
}

static
int NameDspHandler(button_ev)
   XButtonEvent *button_ev;
{
   struct NamesRec *pni=(&namesInfo);
   static Time last_click_time;
   static int last_name_marked;
   Time click_time;
   int checkbox_cols=0, box_offset=0, clicked_col=INVALID, check_updated=FALSE;

   if (enableMouseWheel &&
         (button_ev->button == Button4 || button_ev->button == Button5)) {
      int which=INVALID;
      struct BBRec bbox;

      if (button_ev->button == Button4) {
         which = SCRL_UP;
         SetBBRec(&bbox, 0, 0, scrollBarW, scrollBarW);
      } else if (button_ev->button == Button5) {
         which = SCRL_DN;
         SetBBRec(&bbox, 0, pni->scroll_area_h-scrollBarW, scrollBarW,
               pni->scroll_area_h);
      }
      if (which != INVALID) {
         DoNameBtnScroll(FALSE, which, &bbox, pni);
         RedrawNameScrollWin();
         RedrawDspWindow();
      }
      return INVALID;
   } else if (button_ev->button != Button1) {
      return INVALID;
   }
   if (pni->p_check_array != NULL && pni->p_check_array->num_cols > 0) {
      checkbox_cols = pni->p_check_array->num_cols;
      box_offset = 1;
      if (button_ev->x < checkbox_cols*ROW_HEIGHT) {
         clicked_col = (int)(button_ev->x / ROW_HEIGHT);
      }
   }
   if (pni->marked_index != INVALID &&
         pni->marked_index >= pni->first_index &&
         pni->marked_index < pni->first_index+ITEM_DSPED) {
      int saved_index=pni->marked_index;

      pni->marked_index = INVALID;
      RedrawItem(pni, saved_index);
      pni->marked_index = saved_index;
   }
   pni->marked_index = GetNameEntryNum((int)(button_ev->y / ROW_HEIGHT));
   if (pni->marked_index != INVALID) {
      if (clicked_col != INVALID) {
         int i=pni->marked_index;

         pni->p_check_array->value[clicked_col][i] =
               !(pni->p_check_array->value[clicked_col][i]);
         if (pni->pf_check_update != NULL) {
            /*
             * check_udpated set to TRUE if the callback function calls
             *       SetStringStatus()
             */
            check_updated = (pni->pf_check_update)(&pni->dsp_ptr,
                  &pni->entries, &pni->num_entries,
                  &pni->marked_index, &pni->p_check_array, pni->inbuf,
                  clicked_col, i, pni->userdata);
         }
      }
      RedrawItem(pni, pni->marked_index);
   }
   click_time = button_ev->time;
   if (pni->just_clicked && pni->marked_index != INVALID &&
         last_name_marked == pni->marked_index &&
         (click_time-last_click_time) < doubleClickInterval) {
      return TRUE;
   }
   pni->just_clicked = TRUE;
   last_click_time = click_time;
   last_name_marked = pni->marked_index;

   if (!check_updated) {
      if (pni->dsp_ptr != NULL && pni->marked_index != INVALID) {
         SetStringStatus(pni->dsp_ptr[pni->marked_index].pathstr);
      } else {
         SetStringStatus("");
      }
   }
   return INVALID;
}

static
int ControlChar(key_ev, key_sym)
   XKeyEvent *key_ev;
   KeySym key_sym;
{
   register int i;
   struct NamesRec *pni=(&namesInfo);

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
      i = (pni->marked_index < pni->num_entries-1) ?
            pni->marked_index+1 : pni->num_entries-1;
      break;
   case 'p': /* up one */
   case 'k':
      i = (pni->marked_index>0) ? pni->marked_index-1 : 0;
      break;
   case 'd': /* down one page */
   case 'f':
      if (pni->marked_index==INVALID) {
         i = (pni->num_entries <= ITEM_DSPED) ?
               pni->num_entries-1 : ITEM_DSPED;
      } else if (pni->marked_index < pni->num_entries-ITEM_DSPED) {
         i = pni->marked_index+ITEM_DSPED;
      } else {
         i = pni->num_entries-1;
      }
      break;
   case 'u': /* up one page */
   case 'b':
      i = (pni->marked_index > (ITEM_DSPED-1)) ?
            pni->marked_index-ITEM_DSPED : 0;
      break;
   default: return BAD;
   }
   return i;
}

static
void BackUpOneWord(pn_changing, pn_selected_btn_index)
   int *pn_changing, *pn_selected_btn_index;
{
   struct NamesRec *pni=(&namesInfo);
   int new_dir=FALSE;

   if (pni->buf_index == 0) return;

   if (pni->inbuf[pni->buf_index-1] == DIR_SEP) {
      pni->inbuf[--pni->buf_index] = '\0';
      new_dir = TRUE;
   } else {
      while (pni->buf_index > 0) {
         if (pni->inbuf[(pni->buf_index)-1] == DIR_SEP) {
            pni->inbuf[pni->buf_index] = '\0';
            break;
         } else {
            pni->buf_index--;
         }
      }
      pni->inbuf[pni->buf_index] = '\0';
   }
   ParseFileName(pni->inbuf, pni->dir_name, pni->name);
   if (new_dir) {
      int i;

      sprintf(pni->name, "..%c", DIR_SEP);
      pni->name_index = strlen(pni->name);
      for (i=0; i < pni->num_entries; i++) {
         if (strncmp(&(pni->entries[i])[pni->leading],
               pni->name, pni->name_index) == 0) {
            pni->marked_index = i;
            pni->faking_dot_dot = TRUE;
            *pn_changing = FALSE;
            *pn_selected_btn_index = i;
            break;
         }
      }
   } else {
      pni->first_index = 0;
      pni->marked_index = INVALID;
   }
}

static
void SpecialKeyInNames(key_ev, key_sym, pn_changing, pn_selected_btn_index)
   XKeyEvent *key_ev;
   KeySym key_sym;
   int *pn_changing, *pn_selected_btn_index;
{
   struct NamesRec *pni=(&namesInfo);
   int i=ControlChar(key_ev, key_sym);

   if (i == BAD) return;

   if (i == INVALID) {
      if (pni->edit_style == NAMES_SELECT_FILE) {
         /* back-up one word */
         BackUpOneWord(pn_changing, pn_selected_btn_index);
         sprintf(pni->inbuf, "%s%c%s", pni->dir_name, DIR_SEP, pni->name);
         pni->buf_index = strlen(pni->inbuf);
      } else {
         *pni->inbuf = '\0';
         pni->buf_index = 0;
         pni->first_index = 0;
         pni->marked_index = INVALID;
      }
   } else if (i < pni->num_entries) {
      if (pni->edit_style == NAMES_SELECT_FILE) {
         strcpy(pni->name, &(pni->entries[i])[pni->leading]);
         pni->name_index = strlen(pni->name);
         sprintf(pni->inbuf, "%s%c%s", pni->dir_name, DIR_SEP, pni->name);
         pni->buf_index = strlen(pni->inbuf);
      } else {
         strcpy(pni->inbuf, &(pni->entries[i])[pni->leading]);
         pni->buf_index = strlen(pni->inbuf);
      }
      if (i < pni->first_index) {
         pni->first_index = i;
      } else if (i >= pni->first_index+ITEM_DSPED) {
         if (i < pni->num_entries-ITEM_DSPED) {
            pni->first_index = i;
         } else {
            pni->first_index = pni->num_entries-ITEM_DSPED;
         }
      }
      pni->marked_index = i;
   }
   if (pni->exposed) {
      RedrawNamePath();
      RedrawNameScrollWin();
      RedrawDspWindow();
      RedrawDspBaseWindow();
   }
   if (pni->dsp_ptr != NULL && pni->marked_index != INVALID) {
      SetStringStatus(pni->dsp_ptr[pni->marked_index].pathstr);
   } else {
      SetStringStatus("");
   }
}

static
int GetBtnIndexFromBtnId(btn_id)
   int btn_id;
{
   struct NamesRec *pni=(&namesInfo);
   int i;

   for (i=0; i < pni->num_btns; i++) {
      if (pni->btn_id[i] == btn_id) {
         return i;
      }
   }
#ifdef _TGIF_DBG /* debug, do not translate */
   sprintf(gszMsgBox, "Programing error!\n\nCannot find a button with id=%1d.",
         btn_id);
   MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
#endif /* _TGIF_DBG */
   return INVALID;
}

static
void KeyPressInNames(key_ev, pn_changing, pn_selected_btn_index)
   XKeyEvent *key_ev;
   int *pn_changing, *pn_selected_btn_index;
{
   struct NamesRec *pni=(&namesInfo);
   char buf[80];
   KeySym key_sym;
   int has_ch;

   has_ch = XLookupString(key_ev, buf, sizeof(buf), &key_sym, &c_stat);
   TranslateKeys(buf, &key_sym);

   if ((key_ev->state & ControlMask) && key_sym == XK_j) {
      SpecialKeyInNames(key_ev, key_sym, pn_changing, pn_selected_btn_index);
   } else if (CharIsCRorLF(key_ev, buf, key_sym, &has_ch)) {
      if (pni->edit_style == NAMES_SELECT_FILE) {
         if (!pni->tabbed_from_root) {
            TabInNames();
         }
         *pn_changing = FALSE;
         *pn_selected_btn_index = GetBtnIndexFromBtnId(pni->def_btn_id);
      } else {
         if (pni->def_btn_id != INVALID) {
            *pn_changing = FALSE;
            *pn_selected_btn_index = GetBtnIndexFromBtnId(pni->def_btn_id);
         }
      }
   } else if (CharIsESC(key_ev, buf, key_sym, &has_ch)) {
      *pn_changing = FALSE;
      *pn_selected_btn_index = GetBtnIndexFromBtnId(BUTTON_CANCEL);
   } else if (CharIsBSorDEL(key_ev, buf, key_sym, &has_ch, FALSE)) {
      if (pni->edit_style == NAMES_SELECT_FILE &&
            (pni->faking_dot_dot || pni->pop_from_root)) {
         *pn_changing = FALSE;
         *pn_selected_btn_index = INVALID;
      } else {
         BackSpaceInNames(pn_changing, pn_selected_btn_index);
      }
   } else if (CharIsTAB(key_ev, buf, key_sym, &has_ch)) {
      if (pni->edit_style == NAMES_SELECT_FILE && pni->change_to_root) {
         TabInNames();
         *pn_changing = FALSE;
         *pn_selected_btn_index = INVALID;
      }
   } else if ((!pni->tabbed_from_root || pni->num_entries != 0) &&
         ((key_sym>'\040' && key_sym<='\177' &&
         (key_ev->state & ControlMask)) || key_sym==XK_Up ||
         key_sym==XK_Down || key_sym==XK_KP_Up || key_sym==XK_KP_Down ||
         key_sym==XK_Page_Up || key_sym==XK_KP_Page_Up ||
         key_sym==XK_Page_Down || key_sym==XK_KP_Page_Down)) {
      SpecialKeyInNames(key_ev, key_sym, pn_changing, pn_selected_btn_index);
   } else if (key_sym>='\040' && key_sym<='\177' &&
         (!pni->tabbed_from_root || pni->num_entries != 0)) {
      CharInNames(buf, pn_changing, pn_selected_btn_index);
   }
}

static
void ButtonPressInPath(button_ev)
   XButtonEvent *button_ev;
{
   struct NamesRec *pni=(&namesInfo);

   if (button_ev->button == Button1) {
   } else if (button_ev->button == Button2) {
      int buf_len=0, from_selection=FALSE;
      char *cut_buffer=NULL;

      switch (pni->edit_style) {
      case NAMES_SIMPLE_SELECT_NAME: return;
      case NAMES_COMPLEX_SELECT_NAME: return;
      case NAMES_SELECT_FILE:
         if (pni->tabbed_from_root) return;
         break;
      case NAMES_EDIT_ATTR: break;
      case NAMES_EDIT_NAME: break;
      }
      cut_buffer = FetchSelectionOrCutBuffer(&buf_len, &from_selection);
      if (cut_buffer != NULL) {
         char *c_ptr=cut_buffer;
         int max_len;

         if (pni->edit_style == NAMES_SELECT_FILE) {
            max_len = sizeof(pni->inbuf)-strlen(pni->dir_name)-3;
            while (pni->name_index < max_len &&
                  *c_ptr >= '\040' && *c_ptr < '\177') {
               pni->name[pni->name_index++] = (*c_ptr++);
            }
            pni->name[pni->name_index] = '\0';
            sprintf(pni->inbuf, "%s%c%s", pni->dir_name, DIR_SEP, pni->name);
            pni->buf_index = strlen(pni->inbuf);
         } else {
            max_len = sizeof(pni->inbuf)-strlen(pni->inbuf)-3;
            while (pni->buf_index < max_len &&
                  *c_ptr >= '\040' && *c_ptr < '\177') {
               pni->inbuf[pni->buf_index++] = (*c_ptr++);
            }
            pni->inbuf[pni->buf_index] = '\0';
            strcpy(&(pni->entries[pni->marked_index])[pni->leading],
                  pni->inbuf);
         }
         if (pni->exposed) {
            RedrawNamePath();
            RedrawDspWindow();
         }
         FreeSelectionOrCutBuffer(cut_buffer, from_selection);
      }
   } else if (button_ev->button == Button3) {
   }
}

static
void ButtonPressInNames(button_ev, pn_changing, pn_selected_btn_index)
   XButtonEvent *button_ev;
   int *pn_changing, *pn_selected_btn_index;
{
   struct NamesRec *pni=(&namesInfo);

   if (button_ev->window == pni->base_win) {
      int i;

      if (PointInBBox(button_ev->x, button_ev->y, pni->path_bbox)) {
         ButtonPressInPath(button_ev);
      } else {
         for (i=0; i < pni->num_btns; i++) {
            if (PointInBBox(button_ev->x, button_ev->y,
                  pni->btn_bbox[i])) {
               struct BBRec *bbox=(&(pni->btn_bbox[i]));
               char *btn_str=pni->btn_str[i];
               int looping=TRUE, selected=TRUE, len=strlen(btn_str);
               XEvent ev;

               XGrabPointer(mainDisplay, pni->base_win, False,
                     PointerMotionMask | ButtonReleaseMask, GrabModeAsync,
                     GrabModeAsync, None, handCursor, CurrentTime);
               DisplayButtonInBBox(pni->base_win, btn_str, len,
                     bbox, BUTTON_INVERT, FALSE, 0, NULL);
               while (looping) {
                  XNextEvent(mainDisplay, &ev);

                  if (ev.type == ButtonRelease) {
                     XUngrabPointer(mainDisplay, CurrentTime);
                     if (debugNoPointerGrab) XSync(mainDisplay, False);
                     looping = FALSE;
                  } else if (ev.type == MotionNotify) {
                     if (PointInBBox(ev.xmotion.x, ev.xmotion.y,
                           pni->btn_bbox[i])) {
                        if (!selected) {
                           selected = TRUE;
                           DisplayButtonInBBox(pni->base_win, btn_str, len,
                                 bbox, BUTTON_INVERT, FALSE, 0, NULL);
                        }
                     } else {
                        if (selected) {
                           selected = FALSE;
                           DisplayButtonInBBox(pni->base_win, btn_str, len,
                                 bbox, BUTTON_NORMAL, FALSE, 0, NULL);
                        }
                     }
                  }
               }
               if (selected) {
                  *pn_changing = FALSE;
                  *pn_selected_btn_index = i;
                  DisplayButtonInBBox(pni->base_win, btn_str, len, bbox,
                        BUTTON_NORMAL, FALSE, 0, NULL);
               }
               break;
            }
         }
      }
   } else if (button_ev->window == pni->scroll_win) {
      NameScrollHandler(button_ev);
   } else if (button_ev->window == pni->dsp_win) {
      int double_clicked=(NameDspHandler(button_ev)!=INVALID);

      if (pni->marked_index != INVALID) {
         if (pni->edit_style == NAMES_SELECT_FILE) {
            strcpy(pni->name,
                  &(pni->entries[pni->marked_index])[pni->leading]);
            pni->name_index = strlen(pni->name);
            sprintf(pni->inbuf, "%s%c%s", pni->dir_name, DIR_SEP, pni->name);
            pni->buf_index = strlen(pni->inbuf);
         } else {
            strcpy(pni->inbuf,
                  &(pni->entries[pni->marked_index])[pni->leading]);
            pni->buf_index = strlen(pni->inbuf);
         }
         RedrawNamePath();
         if (double_clicked && pni->double_click_btn_id != INVALID) {
            *pn_changing = FALSE;
            *pn_selected_btn_index =
                  GetBtnIndexFromBtnId(pni->double_click_btn_id);
         }
      }
   }
}

void NamesSetDefaultBtnId(def_btn_id, double_click_btn_id)
   int def_btn_id, double_click_btn_id;
{
   struct NamesRec *pni=(&namesInfo);

   pni->def_btn_id = def_btn_id;
   pni->double_click_btn_id = double_click_btn_id;
}

void NamesSetStyle(edit_style, loop_once)
   int edit_style, loop_once;
{
   struct NamesRec *pni=(&namesInfo);

   pni->edit_style = edit_style;
   pni->loop_once = loop_once;
}

void NamesSetEntries(dsp_ptr, entries, num_entries, p_check_array,
      dont_free_entries, marked_index, leading)
   DspList *dsp_ptr;
   char **entries;
   int num_entries, dont_free_entries, marked_index, leading;
   struct CheckArrayRec *p_check_array;
{
   struct NamesRec *pni=(&namesInfo);

   pni->dsp_ptr = dsp_ptr;
   pni->entries = entries;
   pni->num_entries = num_entries;
   pni->p_check_array = p_check_array;
   pni->dont_free_entries = dont_free_entries;
   pni->marked_index = marked_index;
   pni->leading = leading;
}

void NamesSetCallback(pf_before_loop, pf_after_loop, pf_check_update)
   GetEntriesFunc *pf_before_loop;
   AfterLoopFunc *pf_after_loop;
   CheckUpdateFunc *pf_check_update;
{
   struct NamesRec *pni=(&namesInfo);

   if (pni->dsp_ptr != NULL || pni->entries != NULL) {
#ifdef _TGIF_DBG /* debug, do not translate */
      sprintf(gszMsgBox, "%s.",
            "dsp_ptr != NULL || entries != NULL in NamesSetCallback()");
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
#endif /* _TGIF_DBG */
      if (pni->dsp_ptr != NULL) free(pni->dsp_ptr);
      if (pni->entries != NULL) {
         free(*(pni->entries));
         free(pni->entries);
      }
   }
   pni->dsp_ptr = NULL;
   pni->entries = NULL;
   pni->num_entries = 0;
   pni->pf_before_loop = pf_before_loop;
   pni->pf_after_loop = pf_after_loop;
   pni->pf_check_update = pf_check_update;
}

void NamesSetDir(dir_name)
   char *dir_name;
{
   struct NamesRec *pni=(&namesInfo);

   if (pni->edit_style != NAMES_SELECT_FILE) {
#ifdef _TGIF_DBG /* debug, do not translate */
      sprintf(gszMsgBox, "%s!\n\nNamesSetDir() called with %s.",
            "Programing error", "pni->edit_style != NAMES_SELECT_FILE.");
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
#endif /* _TGIF_DBG */
   }
   UtilStrCpyN(pni->dir_name, sizeof(pni->dir_name), dir_name);
}

static
void BeforeLoopForSelectFile()
{
   struct NamesRec *pni=(&namesInfo);

   if (pni->faking_dot_dot) {
      *pni->name = '\0';
      pni->name_index = 0;
   } else if (pni->tabbed_from_root &&
         !pni->just_tabbed_from_root) {
      pni->name[0] = '\0';
      pni->name_index = 0;
   } else if (pni->tabbed_from_root && pni->just_tabbed_from_root) {
      pni->name_index = strlen(pni->name);
      if (!SetMarkedIndex()) {
         pni->name[0] = '\0';
         pni->name_index = 0;
      }
   }
   sprintf(pni->inbuf, "%s%c%s", pni->dir_name, DIR_SEP, pni->name);
   pni->buf_index = strlen(pni->inbuf);

   if (pni->exposed) {
      XClearWindow(mainDisplay, pni->base_win);
      RedrawNameBaseWindow();
   }
   pni->pop_from_root = FALSE;
}

static
int GetParentRealDir(dir, buf, buf_sz)
   char *dir, *buf;
   int buf_sz;
{
   int rc=FALSE;
   char saved_cur_dir[MAXPATHLENGTH];

   if (!GetWorkingDirectory(saved_cur_dir, sizeof(saved_cur_dir))) {
      *saved_cur_dir = '\0';
   }
   if (SetWorkingDirectory(dir) && SetWorkingDirectory("..")) {
      if (GetWorkingDirectory(buf, buf_sz)) {
         rc = TRUE;
      }
   }
   if (*saved_cur_dir != '\0') {
      SetWorkingDirectory(saved_cur_dir);
   }
   return rc;
}

static
int BreakForSelectFileAfterLoop()
{
   struct NamesRec *pni=(&namesInfo);

   if (pni->marked_index == INVALID && !pni->change_to_root &&
         !pni->pop_from_root && !pni->just_tabbed_from_root) {
      return TRUE;
   } else if (FileIsRemote(pni->name)) {
      pni->remote_file = TRUE;
      return TRUE;
   }
   if (pni->inbuf[pni->buf_index-1] != DIR_SEP) {
      if (!pni->just_tabbed_from_root) {
         return TRUE;
      }
   } else {
      if (pni->name[0] == '.' && pni->name[1] == '.' &&
            pni->name[2] == DIR_SEP) {
         /* need to find out what's the real directory */
         char parent_dir[MAXPATHLENGTH];

         if (GetParentRealDir(pni->dir_name, parent_dir, sizeof(parent_dir))) {
            int len=0;

            len = strlen(parent_dir);
            if (len > 0 && parent_dir[len-1] == DIR_SEP) {
               parent_dir[--len] = '\0';
            }
            sprintf(pni->inbuf, "%s%c..", parent_dir, DIR_SEP);
            pni->buf_index = strlen(pni->inbuf);
            ParseFileName(pni->inbuf, pni->dir_name, pni->name);
            return FALSE;
         }
      }
      pni->inbuf[--(pni->buf_index)] = '\0';
      if (pni->name[0] == '.' && pni->name[1] == '.' &&
            pni->name[2] == DIR_SEP) {
         /* saved_name is used for faking_dot_dot */
         int i;

         for (i=strlen(pni->dir_name)-1;
               i >= 0 && pni->dir_name[i] != DIR_SEP; i--) {
         }
         if (i < 0) {
            strcpy(pni->saved_name, pni->dir_name);
            *pni->dir_name = '\0';
         } else {
            strcpy(pni->saved_name, &(pni->dir_name[i+1]));
            pni->dir_name[i] = '\0';
         }
      } else {
         strcpy(&pni->inbuf[pni->buf_index++], DIR_SEP_STR);
         ParseFileName(pni->inbuf, pni->dir_name, pni->name);
      }
   }
   return FALSE;
}

int Names(win_name, pn_selected_index, selected_str, str_sz, p_void)
   char *win_name, *selected_str;
   int *pn_selected_index, str_sz;
   void *p_void;
{
   struct NamesRec *pni=(&namesInfo);
   int i, looping=TRUE, selected_btn_index=INVALID, left=0;

   if (selected_str != NULL) *selected_str = '\0';
   if (pn_selected_index != NULL) *pn_selected_index = INVALID;

   if (!CreateNamesWindows(win_name)) return INVALID;

   EndMeasureTooltip(FALSE);

   pni->userdata = p_void;

   if (pni->edit_style == NAMES_SELECT_FILE) {
      pni->faking_dot_dot = FALSE;
      pni->change_to_root = FALSE;
      pni->just_tabbed_from_root = FALSE;
      *pni->saved_dir_name = '\0';
   }
   left = pni->btn_start;
   for (i=0; i < pni->num_btns; i++) {
      int button_w=ButtonWidth(pni->btn_str[i], 8, NULL), button_h=0;

      if (boldMsgFontSet == NULL && boldMsgFontPtr == NULL) {
         pni->btn_bbox[i].lty = (6+ITEM_DSPED)*ROW_HEIGHT;
         button_h = defaultFontHeight + 4;
      } else {
         pni->btn_bbox[i].lty = (5+ITEM_DSPED)*ROW_HEIGHT +
               (boldMsgFontHeight+1);
         button_h = boldMsgFontHeight + 4;
      }
      pni->btn_bbox[i].ltx = left;
      pni->btn_bbox[i].lty -= 2;
      pni->btn_bbox[i].rbx = pni->btn_bbox[i].ltx+button_w;
      pni->btn_bbox[i].rby = pni->btn_bbox[i].lty+button_h;
      left = pni->btn_bbox[i].rbx + 1 + defaultFontWidth;
   }
   SaveStatusStrings();
   while (looping) {
      int changing=TRUE;

      if (pni->edit_style == NAMES_SELECT_FILE) {
         BeforeLoopForSelectFile();
      }
      if (pni->pf_before_loop != NULL) {
         int rc;
         char saved_ch='\0';

         SetWatchCursor(drawWindow);
         SetWatchCursor(mainWindow);
         SetWatchCursor(pni->base_win);
         if (pni->edit_style == NAMES_SELECT_FILE && !pni->tabbed_from_root &&
               !pni->just_tabbed_from_root) {
            saved_ch = *pni->inbuf;
            *pni->inbuf = '\0';
         }
         rc = (pni->pf_before_loop)(&pni->dsp_ptr,
               &pni->entries, &pni->num_entries,
               &pni->marked_index, &pni->p_check_array, pni->inbuf, p_void);
         if (pni->edit_style == NAMES_SELECT_FILE && !pni->tabbed_from_root &&
               !pni->just_tabbed_from_root) {
            *pni->inbuf = saved_ch;
         }
         if (pni->edit_style == NAMES_SELECT_FILE &&
               pni->just_tabbed_from_root) {
            /* saved_name is used for just_tabbed_from_root */
            *pni->dir_name = '\0';
            strcpy(pni->name, pni->saved_name);
            pni->name_index = strlen(pni->name);
            sprintf(pni->inbuf, "%s%c%s", pni->dir_name, DIR_SEP, pni->name);
            pni->buf_index = strlen(pni->inbuf);
            *pni->saved_name = '\0';
            ParseFileName(pni->inbuf, pni->dir_name, pni->name);
            pni->name_index = strlen(pni->name);
            if (!SetMarkedIndex()) {
               pni->name[0] = '\0';
               pni->name_index = 0;
            }
         }
         if (pni->edit_style == NAMES_SELECT_FILE && pni->faking_dot_dot) {
            /* saved_name is used for faking_dot_dot */
            strcpy(pni->name, pni->saved_name);
            pni->name_index = strlen(pni->name);
            sprintf(pni->inbuf, "%s%c%s", pni->dir_name, DIR_SEP, pni->name);
            pni->buf_index = strlen(pni->inbuf);
            *pni->saved_name = '\0';
            if (!SetMarkedIndex()) {
               pni->name[0] = '\0';
               pni->name_index = 0;
            }
         }
         SetDefaultCursor(mainWindow);
         ShowCursor();
         SetDrawCursor(pni->base_win);
         if (!rc) break;
      }
      if (pni->edit_style == NAMES_SELECT_FILE) {
         pni->just_tabbed_from_root = FALSE;
         pni->faking_dot_dot = FALSE;
      }
      NamesUpdateIndices();

      if (pni->pf_before_loop != NULL && pni->exposed) {
         RedrawNamePath();
         RedrawNameScrollWin();
         RedrawDspWindow();
         RedrawDspBaseWindow();
      }
      if (pni->dsp_ptr != NULL && pni->marked_index != INVALID) {
         SetStringStatus(pni->dsp_ptr[pni->marked_index].pathstr);
      }
      XSync(mainDisplay, False);

      selected_btn_index = INVALID;
      while (changing) {
         XEvent input, ev;

         XNextEvent(mainDisplay, &input);

         if (ExposeOrMapNames(&input)) {
            continue;
         }
         if (input.type == Expose) {
            ExposeEventHandler(&input, FALSE);
         } else if (input.type==VisibilityNotify &&
               input.xany.window==mainWindow &&
               input.xvisibility.state==VisibilityUnobscured) {
            int i;

            while (XCheckWindowEvent(mainDisplay, mainWindow,
                  VisibilityChangeMask, &ev)) ;
            if (pinnedMainMenu) XMapRaised(mainDisplay, mainMenuWindow);
            for (i=0; i < numExtraWins; i++) {
               if (extraWinInfo[i].mapped && extraWinInfo[i].raise &&
                     extraWinInfo[i].window != None) {
                  XMapRaised(mainDisplay, extraWinInfo[i].window);
               }
            }
            XMapRaised(mainDisplay, pni->base_win);
         } else if (input.type == KeyPress) {
            KeyPressInNames(&input.xkey, &changing, &selected_btn_index);
         } else if (input.type == ButtonPress) {
            ButtonPressInNames(&input.xbutton, &changing, &selected_btn_index);
         } else if ((input.xany.window==pni->base_win ||
               input.xany.window==pni->dsp_base_win ||
               input.xany.window==pni->dsp_win ||
               input.xany.window==pni->scroll_win) &&
               IsWM_DELETE_WINDOW(&input)) {
            KeyPressInNames(&input.xkey, &changing, &selected_btn_index);
            changing = FALSE;
            selected_btn_index = INVALID;
         }
      }
      if (pni->exposed && selected_btn_index != INVALID) {
         DisplayButtonInBBox(pni->base_win, pni->btn_str[selected_btn_index],
               strlen(pni->btn_str[selected_btn_index]),
               &(pni->btn_bbox[selected_btn_index]), BUTTON_INVERT, FALSE, 0,
               NULL);
         XSync(mainDisplay, False);
      }
      if (pni->edit_style == NAMES_SELECT_FILE) {
         if (pni->marked_index != INVALID) {
            strcpy(pni->name,
                  &(pni->entries[pni->marked_index])[pni->leading]);
            pni->name_index = strlen(pni->name);
            sprintf(pni->inbuf, "%s%c%s", pni->dir_name, DIR_SEP, pni->name);
            pni->buf_index = strlen(pni->inbuf);
         }
      }
      if (pni->pf_after_loop != NULL) {
         int btn_id=INVALID;

         if (selected_btn_index != INVALID) {
            btn_id = pni->btn_id[selected_btn_index];
         }
         if (!(pni->pf_after_loop)(&pni->dsp_ptr,
               &pni->entries, &pni->num_entries,
               &pni->marked_index, &pni->p_check_array, pni->inbuf, btn_id,
               pni->marked_index, p_void)) {
            looping = FALSE;
         }
      }
      if (selected_btn_index != INVALID) {
         if (pni->btn_id[selected_btn_index] == BUTTON_CANCEL) {
            looping = FALSE;
         }
      }
      if (pni->edit_style == NAMES_SELECT_FILE) {
         if (BreakForSelectFileAfterLoop()) {
            looping = FALSE;
         }
      }
      if (pni->loop_once == NAMES_LOOP_ONCE) {
         looping = FALSE;
      }
      if (pni->exposed && selected_btn_index != INVALID) {
         DisplayButtonInBBox(pni->base_win, pni->btn_str[selected_btn_index],
               strlen(pni->btn_str[selected_btn_index]),
               &(pni->btn_bbox[selected_btn_index]), BUTTON_NORMAL, FALSE, 0,
               NULL);
         XSync(mainDisplay, False);
      }
   }
   RestoreStatusStrings();
   XDestroyWindow(mainDisplay, pni->base_win);
   if (warpToWinCenter) {
      XWarpPointer(mainDisplay, None, drawWindow, 0, 0, 0, 0,
            (int)(ZOOMED_SIZE(drawWinW)>>1), (int)(ZOOMED_SIZE(drawWinH)>>1));
   }
   if (selected_str != NULL) {
      if (pni->remote_file) {
         UtilStrCpyN(selected_str, str_sz, pni->name);
      } else {
         UtilStrCpyN(selected_str, str_sz, pni->inbuf);
      }
   }
   pni->userdata = NULL;

   if (pn_selected_index != NULL) *pn_selected_index = pni->marked_index;
   if (selected_btn_index != INVALID) {
      return pni->btn_id[selected_btn_index];
   }
   return INVALID;
}
