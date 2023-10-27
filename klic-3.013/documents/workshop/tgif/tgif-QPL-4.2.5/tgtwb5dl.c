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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/tgtwb5dl.c,v 1.15 2011/05/22 22:55:02 william Exp $
 */

#ifndef _NO_TGTWB5

#define _INCLUDE_FROM_TGTWB5DL_C_

#include "tgifdefs.h"
#include "patchlvl.h"
#include "tidget.h"

#include "auxtext.e"
#include "cursor.e"
#include "dialog.e"
#include "file.e"
#include "font.e"
#include "mainloop.e"
#include "msg.e"
#include "raster.e"
#include "rect.e"
#include "scroll.e"
#include "setup.e"
#include "strtbl.e"
#include "tdgtbase.e"
#include "tdgtbmpl.e"
#include "tdgtbrow.e"
#include "tdgtbtn.e"
#include "tdgtdraw.e"
#include "tdgtsedt.e"
#include "tdgtlist.e"
#include "tdgtmsg.e"
#include "text.e"
#include "tidget.e"
#include "tgtwb5dl.e"
#include "tgtwb5xl.e"
#include "util.e"
#include "xbitmap.e"

ZyfhInfo gZyfhInfo;

typedef struct tagZyfhDlgInfo {
   Window parent_win; /* should be the rootWindow */

   TdgtMsg *msg_ctl;
   TdgtDraw *zyfh_ctl; /* the zhu-yin-fu-hao keypad */
   TdgtDraw *zyfh_dpy_ctl; /* displays what has been entered */
   TdgtSmplEdit *edit_ctl;
   TdgtBmpList *bmp_list_ctl;
   TdgtBtnRow *btn_row_ctl;

   int screen_w, screen_h;
   int base_win_w, base_win_h;
   int msg_x, msg_y, msg_w, msg_h;
   int zyfh_dpy_x, zyfh_dpy_y, zyfh_dpy_w, zyfh_dpy_h;
   int zyfh_x, zyfh_y, zyfh_w, zyfh_h;
   int edit_x, edit_y, edit_w, edit_h;
   int bmp_list_x, bmp_list_y, bmp_list_w, bmp_list_h;
   int btn_row_x, btn_row_y, btn_row_w, btn_row_h;

   char **entries;
   int num_entries;
   int first_index;
   int marked_index;
   DspList *dsp_ptr;

   char *title_str, *formatted_title_str;
   int def_btn_id, double_click_btn_id;

   Time last_click_time;
   int last_selected;
   int just_fetched;
   int english_spelling; /* TRUE means pinyin, FALSE means zhu-yin-fu-hao */
   int zyfh_spell_index;
   int zyfh_spell_buf[MAXZYFH_CHARS+1]; /* each char is one of PH_* */
} ZyfhDlgInfo;

/* --------------------- Utility Functions --------------------- */

typedef struct tagZyfhDpyAppendInfo {
   TdgtBase *base_ctl;
   int row, col; /* position in zyfh_ctl */
} ZyfhDpyAppendInfo;

#define TDGTARG_ZYFH_DPY_CLEAR 1 /* pv_cmd_userdata is NULL */
#define TDGTARG_ZYFH_DPY_APPEND 2 /* pv_cmd_userdata is (ZyfhDpyAppendInfo*) */

static
void ClearAllControls(pTdgtBase)
   TdgtBase *pTdgtBase;
{
   ZyfhDlgInfo *pzdi=(ZyfhDlgInfo*)(pTdgtBase->pti->userdata);

   pzdi->last_selected = INVALID;
   pzdi->just_fetched = FALSE;
   pzdi->zyfh_spell_index = 0;
   memset(pzdi->zyfh_spell_buf, 0, sizeof(pzdi->zyfh_spell_buf));

   TidgetSendCmd(pzdi->zyfh_dpy_ctl->pti, TDGTCMD_DRAW_USER,
         TDGTARG_ZYFH_DPY_CLEAR, pTdgtBase);
   TdgtSmplEditSetText(pzdi->edit_ctl, "");
   TidgetSendCmd(pzdi->bmp_list_ctl->pti, TDGTCMD_LIST_RESETCONTENT, 0, NULL);
}

static
int SendSelectedCharToClient(pTdgtBase)
   TdgtBase *pTdgtBase;
{
   ZyfhDlgInfo *pzdi=(ZyfhDlgInfo*)(pTdgtBase->pti->userdata);
   int selected_index=INVALID;
   BmpListItemInfo *pblii=NULL;

   TidgetSendCmd(pzdi->bmp_list_ctl->pti, TDGTCMD_LIST_GETCURSEL, 0,
         &selected_index);
   if (TidgetSendCmd(pzdi->bmp_list_ctl->pti, TDGTCMD_LIST_GETITEM,
         selected_index, &pblii)) {
      if (pblii != NULL) {
         XKeyEvent key_ev;
         int has_ch=2;
         char buf[3];
         XChar2b *pch16=(XChar2b*)(pblii->pv_userdata);

         memset(&key_ev, 0, sizeof(XKeyEvent));
         buf[0] = pch16->byte1;
         buf[1] = pch16->byte2;
         buf[2] = '\0';
         HandleTextFromBuffer(&key_ev, buf, (KeySym)0, has_ch);

         return TRUE;
      }
   }
   return FALSE;
}

static
int SendCRToClient(pTdgtBase)
   TdgtBase *pTdgtBase;
{
   XKeyEvent key_ev;
   int has_ch=1;
   char buf[2];

   memset(&key_ev, 0, sizeof(XKeyEvent));
   buf[0] = '\n';
   buf[1] = '\0';
   HandleTextFromBuffer(&key_ev, buf, (KeySym)0, has_ch);

   return TRUE;
}

static
int InvalidSequence(pTdgtBase)
   TdgtBase *pTdgtBase;
{
   ZyfhDlgInfo *pzdi=(ZyfhDlgInfo*)(pTdgtBase->pti->userdata);

   if (pzdi->zyfh_spell_index == 4 &&
         (pzdi->zyfh_spell_buf[0] == PH_L || pzdi->zyfh_spell_buf[0] == PH_N) &&
         pzdi->zyfh_spell_buf[1] == PH_Yu && pzdi->zyfh_spell_buf[3] < PH_Er &&
         pzdi->zyfh_spell_buf[2] != PH_Eh) {
      return TRUE;
   }
   return FALSE;
}

static
int Fill2ByteCharFromHexStringBuf(buf, pch16)
   char *buf;
   XChar2b *pch16;
{
   int hi_nibble=0, lo_nibble=0, hval=0;
   
   if (IsHex(buf[0], &hi_nibble) && IsHex(buf[1], &lo_nibble)) {
      hval = (((hi_nibble&0x0ff)<<4)+(lo_nibble&0x0ff));
      pch16->byte1 = hval;
      if (IsHex(buf[2], &hi_nibble) && IsHex(buf[3], &lo_nibble)) {
         hval = (((hi_nibble&0x0ff)<<4)+(lo_nibble&0x0ff));
         pch16->byte2 = hval;
      } else {
         return FALSE;
      }
   } else {
      return FALSE;
   }
   return TRUE;
}

static
void FreeBitmapItemCallback(pblii)
   BmpListItemInfo *pblii;
{
   if (pblii == NULL) return;

   if (pblii->pixmap != None) XFreePixmap(mainDisplay, pblii->pixmap);
   if (pblii->pv_userdata != NULL) {
      /* XChar2b */
      free(pblii->pv_userdata);
   }
   free(pblii);
}

#ifdef NOT_DEFINED /* debug, do not translate */
static
void DebugCharBitmap(bitmap, w, h)
   Pixmap bitmap;
   int w, h;
{
   static int n=0;
   int r=0, c=0;
   XImage *image=XGetImage(mainDisplay, bitmap, 0, 0, w,
         h, 1, ZPixmap);

   fprintf(stdout, "Character %1d:\n", ++n);
   for (r=0; r < h; r++) {
      fprintf(stdout, "    ");
      for (c=0; c < w; c++) {
         if (XGetPixel(image, (int)c, (int)r) == 1) {
            fprintf(stdout, "%1d", 1);
         } else {
            fprintf(stdout, "%1d", 0);
         }
      }
      fprintf(stdout, "\n");
   }
   XDestroyImage(image);
}

void DrawAndDebugChar()
{
   XChar2b str16;
   XGCValues values;
   GC tmp_gc;
   Pixmap bitmap=XCreatePixmap(mainDisplay, mainWindow, gZyfhInfo.b5_font_w,
         gZyfhInfo.b5_font_h, 1);

   values.foreground = 1;
   values.background = 0;
   values.fill_style = FillSolid;
   values.function = GXcopy;
   values.font = gZyfhInfo.xfs->fid;
   tmp_gc = XCreateGC(mainDisplay, bitmap,
         GCForeground | GCBackground | GCFillStyle | GCFunction | GCFont,
         &values);

   XSetForeground(mainDisplay, tmp_gc, 0);
   XFillRectangle(mainDisplay, bitmap, tmp_gc, 0, 0, gZyfhInfo.b5_font_w,
         gZyfhInfo.b5_font_h);
   XSetForeground(mainDisplay, tmp_gc, 1);

   switch (gZyfhInfo.b5_font_real_encoding) {
   case GB_ENCODING:
      str16.byte1 = 0xb9 & 0x7f;
      str16.byte2 = 0xfa & 0x7f;
      break;
   case BIG5_ENCODING:
      str16.byte1 = 0xa4;
      str16.byte2 = 0xad;
      break;
   }
   XDrawString16(mainDisplay, bitmap, tmp_gc, 0, gZyfhInfo.b5_font_asc,
         &str16, 1);

   DebugCharBitmap(bitmap, gZyfhInfo.b5_font_w, gZyfhInfo.b5_font_h);

   XFreeGC(mainDisplay, tmp_gc);
   XFreePixmap(mainDisplay, bitmap);
}
#endif /* NOT_DEFINED */

static
int DoLoadChars(pTdgtBase, buf, str16, num_char2bs)
   TdgtBase *pTdgtBase;
   char *buf;
   XChar2b *str16;
   int num_char2bs;
{
   ZyfhDlgInfo *pzdi=(ZyfhDlgInfo*)(pTdgtBase->pti->userdata);
   int i=0, redraw_disabled=FALSE;

   TidgetSendCmd(pzdi->bmp_list_ctl->pti, TDGTCMD_LIST_RESETCONTENT, 0, NULL);

   redraw_disabled = TidgetDisableRedraw(pzdi->bmp_list_ctl->pti, TRUE);
   for (i=0; i < num_char2bs; i++) {
      XGCValues values;
      Pixmap bitmap=None;
      BmpListItemInfo *pblii=NULL;
      XChar2b *pch16=NULL;
      XChar2b str16_buf;

      bitmap = XCreatePixmap(mainDisplay, mainWindow, gZyfhInfo.b5_font_w,
            gZyfhInfo.b5_font_h, 1);
      if (bitmap == None) {
         FailAllocPixmapMessage(gZyfhInfo.b5_font_w, gZyfhInfo.b5_font_h);
      }
      XSetForeground(mainDisplay, rotateGC, 0);
      XFillRectangle(mainDisplay, bitmap, rotateGC, 0, 0, gZyfhInfo.b5_font_w,
            gZyfhInfo.b5_font_h);

      values.foreground = 1;
      values.font = gZyfhInfo.xfs->fid;
      XChangeGC(mainDisplay, rotateGC, GCForeground | GCFont, &values);

      if (gZyfhInfo.b5_font_mod_bytes) {
         str16_buf.byte1 = str16[i].byte1 & 0x7f;
         str16_buf.byte2 = str16[i].byte2 & 0x7f;
      } else {
         str16_buf.byte1 = str16[i].byte1;
         str16_buf.byte2 = str16[i].byte2;
      }
      XDrawString16(mainDisplay, bitmap, rotateGC, 0, gZyfhInfo.b5_font_asc,
            &str16_buf, 1);

#ifdef NOT_DEFINED
      DebugCharBitmap(bitmap, gZyfhInfo.b5_font_w, gZyfhInfo.b5_font_h);
#endif /* NOT_DEFINED */

      pch16 = (XChar2b*)malloc(sizeof(XChar2b));
      if (pch16 == NULL) FailAllocMessage();
      memset(pch16, 0, sizeof(XChar2b));
      memcpy(pch16, &str16[i], sizeof(XChar2b));

      pblii = (BmpListItemInfo*)malloc(sizeof(BmpListItemInfo));
      if (pblii == NULL) FailAllocMessage();
      memset(pblii, 0, sizeof(BmpListItemInfo));

      pblii->pixmap = bitmap;
      pblii->depth = 1;
      pblii->width = gZyfhInfo.b5_font_w;
      pblii->height = gZyfhInfo.b5_font_h;
      pblii->pv_userdata = pch16;
      pblii->pf_free_callback = FreeBitmapItemCallback;

      if (!TdgtBmpListAddItem(pzdi->bmp_list_ctl, pblii)) {
      }
   }
   TidgetDisableRedraw(pzdi->bmp_list_ctl->pti, redraw_disabled);

   return TRUE;
}

#define MAX_PINYIN_ET_BIG5_STR 628
#define MAX_BIG5_STR (MAX_PINYIN_ET_BIG5_STR>>2)

static
int LoadChars(pTdgtBase, buf)
   TdgtBase *pTdgtBase;
   char *buf;
{
   ZyfhDlgInfo *pzdi=(ZyfhDlgInfo*)(pTdgtBase->pti->userdata);
   int len=0, str16_index=0, ok=TRUE;
   char *psz=NULL, **ppsz=NULL, *psz_found=NULL;
   XChar2b str16[MAX_BIG5_STR+1];

   if (!pzdi->english_spelling && InvalidSequence(pTdgtBase)) {
      sprintf(gszMsgBox, TgLoadString(STID_INVALID_PINYIN_SEQ), buf);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return FALSE;
   }
   switch (gZyfhInfo.b5_font_real_encoding) {
   case GB_ENCODING:
      for (ppsz=pinyin_gb2312; *ppsz!=NULL; ppsz++) {
         if (strcmp(*ppsz, buf) == 0) {
            psz_found = (*(++ppsz));
            break;
         }
         ppsz++;
      }
      break;
   case BIG5_ENCODING:
      for (ppsz=pinyin_et_big5; *ppsz!=NULL; ppsz++) {
         if (strcmp(*ppsz, buf) == 0) {
            psz_found = (*(++ppsz));
            break;
         }
         ppsz++;
      }
      break;
   default: return FALSE;
   }
   if (psz_found == NULL) {
      sprintf(gszMsgBox, TgLoadString(STID_INVALID_PINYIN_SEQ), buf);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return FALSE;
   }
   len = strlen(psz_found);
#ifdef _TGIF_DBG /* debug, do not translate */
   if (len > MAX_PINYIN_ET_BIG5_STR) {
      sprintf(gszMsgBox, "Fetched hexstring in LoadChars() is too long: %1d (limit is %1d).\n",
            len, MAX_PINYIN_ET_BIG5_STR);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return FALSE;
   }
#endif /* _TGIF_DBG */
   for (psz=psz_found; ok && *psz != '\0'; psz=(&psz[4])) {
      if (Fill2ByteCharFromHexStringBuf(psz, &str16[str16_index])) {
         str16_index++;
      } else {
         ok = FALSE;
      }
   }
   memset(&str16[str16_index], 0, sizeof(XChar2b));

#ifdef _TGIF_DBG /* debug, do not translate */
   if (!ok) {
      switch (gZyfhInfo.b5_font_real_encoding) {
      case GB_ENCODING:
         sprintf(gszMsgBox, "The pinyin_gb2312[] table seems to be corrupted.\n");
         break;
      case BIG5_ENCODING:
         sprintf(gszMsgBox, "The pinyin_et_big5[] table seems to be corrupted.\n");
         break;
      }
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return FALSE;
   }
#endif /* _TGIF_DBG */

   return DoLoadChars(pTdgtBase, buf, str16, str16_index);
}

static
int FetchChars(pTdgtBase)
   TdgtBase *pTdgtBase;
{
   ZyfhDlgInfo *pzdi=(ZyfhDlgInfo*)(pTdgtBase->pti->userdata);
   char *buf=UtilStrDup(TdgtSmplEditGetText(pzdi->edit_ctl));

   if (buf != NULL && *buf != '\0') {
      char saved_ch='\0';
      int ok=FALSE;

      if (buf[0] >= 'a' && buf[0] <= 'z') {
         saved_ch = buf[0];
         buf[0] = buf[0]-'a'+'A';
      }
      ok = LoadChars(pTdgtBase, buf);
      if (saved_ch != '\0') buf[0] = saved_ch;
   }
   pzdi->just_fetched = TRUE;
   pzdi->last_selected = INVALID;

   UtilFree(buf);

   return FALSE;
}

static
void ReverseZhuYenFuHao(index, pn_symbol_ltx, pn_symbol_lty)
   int index, *pn_symbol_ltx, *pn_symbol_lty;
   /*
    * Given an index obtained from pzdi->zyfh_spell_buf[x], calculate
    *         the ltx and lty of a bitmap from the zhfh_ctl.
    */
{
   if (pn_symbol_lty != NULL) {
      *pn_symbol_lty = (index % (gZyfhInfo.zyfh_num_rows)) *
            (gZyfhInfo.zyfh_image_wh);
   }
   if (pn_symbol_ltx != NULL) {
      *pn_symbol_ltx = ((int)(index / gZyfhInfo.zyfh_num_rows)) *
            (gZyfhInfo.zyfh_image_wh);
   }
}

static
int ComposePinYin(pTdgtBase, buf, buf_sz)
   TdgtBase *pTdgtBase;
   char *buf;
   int buf_sz;
{
   ZyfhDlgInfo *pzdi=(ZyfhDlgInfo*)(pTdgtBase->pti->userdata);
   int i=0;

   if (buf_sz < MAXZYFH_CHARS+1) return FALSE;

   *buf = '\0';
   for (i=0; i < pzdi->zyfh_spell_index; i++) {
      switch (i) {
      case 0:
         if (pzdi->zyfh_spell_index >= 2 && pzdi->zyfh_spell_buf[0] == PH_Yu &&
               pzdi->zyfh_spell_buf[1] == PH_Eng) {
            strcat (buf, "Yo");
         } else {
            strcpy(buf, firstLargeTransTbl[pzdi->zyfh_spell_buf[0]]);
            if (pzdi->zyfh_spell_index == 2 &&
                  pzdi->zyfh_spell_buf[1] < PH_Er) {
               strcat(buf, singleExtention[pzdi->zyfh_spell_buf[0]]);
            }
         }
         break;
      case 1:
         if (pzdi->zyfh_spell_index >= 3 && pzdi->zyfh_spell_buf[1] == PH_W &&
               pzdi->zyfh_spell_buf[2] == PH_Eng) {
            strcat(buf, "o");
         } else if (pzdi->zyfh_spell_index >= 3 &&
               pzdi->zyfh_spell_buf[1] == PH_Yu &&
               pzdi->zyfh_spell_buf[2] == PH_Eng) {
            strcat(buf, "io");
         } else if (pzdi->zyfh_spell_index >= 2 &&
               pzdi->zyfh_spell_buf[0] == PH_Yu &&
               pzdi->zyfh_spell_buf[1] == PH_Eng) {
            strcat(buf, "ng");
         } else if (pzdi->zyfh_spell_index >= 2 &&
               pzdi->zyfh_spell_buf[0] == PH_Y &&
               pzdi->zyfh_spell_buf[1] == PH_En) {
            strcat(buf, "in");
         } else if (pzdi->zyfh_spell_index >= 2 &&
               pzdi->zyfh_spell_buf[0] == PH_Y &&
               pzdi->zyfh_spell_buf[1] == PH_Eng) {
            strcat(buf, "ing");
         } else {
            strcat(buf, secondLargeTransTbl[pzdi->zyfh_spell_buf[i]]);
         }
         break;
      case 2:
         if (pzdi->zyfh_spell_index == 3 && pzdi->zyfh_spell_buf[2] < PH_Er &&
               pzdi->zyfh_spell_buf[1] == PH_Yu &&
               (pzdi->zyfh_spell_buf[0] == PH_L ||
               pzdi->zyfh_spell_buf[0] == PH_N)) {
            strcat(buf, ":");
         }
         strcat(buf, thirdLargeTransTbl[pzdi->zyfh_spell_buf[i]]);
         break;
      case 3:
         if (pzdi->zyfh_spell_index == 4 && pzdi->zyfh_spell_buf[3] < PH_Er &&
               pzdi->zyfh_spell_buf[2] == PH_Eh &&
               pzdi->zyfh_spell_buf[1] == PH_Yu &&
               (pzdi->zyfh_spell_buf[0] == PH_L ||
               pzdi->zyfh_spell_buf[0] == PH_N)) {
            strcat(buf, ":");
         }
         strcat(buf, fourthLargeTransTbl[pzdi->zyfh_spell_buf[i]]);
         break;
      }
   }
   return TRUE;
}

static
void UpdateSpellingAndDpyControls(pTdgtBase)
   TdgtBase *pTdgtBase;
{
   ZyfhDlgInfo *pzdi=(ZyfhDlgInfo*)(pTdgtBase->pti->userdata);
   char buf[MAXZYFH_CHARS+1];
   int fetch_immediately=FALSE;

   if (ComposePinYin(pTdgtBase, buf, sizeof(buf))) {
      int len=strlen(buf);

      TdgtSmplEditSetText(pzdi->edit_ctl, buf);
      if (!pzdi->english_spelling && pzdi->zyfh_spell_index == 1 &&
            (strcmp(buf, "Punctuations") == 0 || strcmp(buf, "Symbols") == 0)) {
         fetch_immediately = TRUE;
      } else if (len > 0 && buf[len-1] >= '1' && buf[len-1] <= '5') {
         fetch_immediately = TRUE;
      }
   }
   RedrawTidget(pzdi->zyfh_dpy_ctl->pti);

   if (fetch_immediately) {
      FetchChars(pTdgtBase);
   }
}

static
int HandleBackSpace(pTdgtBase)
   TdgtBase *pTdgtBase;
{
   ZyfhDlgInfo *pzdi=(ZyfhDlgInfo*)(pTdgtBase->pti->userdata);

   if (pzdi->english_spelling) {
      if (TdgtSmplEditDelLastChar(pzdi->edit_ctl)) {
         pzdi->just_fetched = FALSE;
         pzdi->zyfh_spell_index--;
         pzdi->last_selected = INVALID;
         TidgetSendCmd(pzdi->bmp_list_ctl->pti, TDGTCMD_LIST_RESETCONTENT, 0,
               NULL);
      } else {
         /* no change */
      }
   } else {
      if (pzdi->zyfh_spell_index > 0) {
         pzdi->just_fetched = FALSE;
         pzdi->zyfh_spell_index--;
         pzdi->last_selected = INVALID;
         UpdateSpellingAndDpyControls(pTdgtBase);
         TidgetSendCmd(pzdi->bmp_list_ctl->pti, TDGTCMD_LIST_RESETCONTENT, 0,
               NULL);
      } else {
         /* no change */
      }
   }
   return FALSE;
}

/* --------------------- ZyfhRedrawCallback --------------------- */

static
int ZyfhRedrawCallback(pTdgtDraw)
   TdgtDraw *pTdgtDraw;
{
   XGCValues values;

   if (pTdgtDraw == NULL) return FALSE;

   values.fill_style = FillOpaqueStippled;
   values.ts_x_origin = pTdgtDraw->client_area.x;
   values.ts_y_origin = pTdgtDraw->client_area.y;
   values.stipple = gZyfhInfo.zyfh_bitmap;
   XChangeGC(mainDisplay, gTidgetManager.gc,
         GCFillStyle | GCTileStipXOrigin | GCTileStipYOrigin | GCStipple,
         &values);

   XFillRectangle(mainDisplay, pTdgtDraw->pti->tci.win, gTidgetManager.gc,
         pTdgtDraw->client_area.x, pTdgtDraw->client_area.y,
         pTdgtDraw->client_area.w, pTdgtDraw->client_area.h);

   TidgetManagerResetGC();

   return TRUE;
}

/* --------------------- ZyfhEvHandlerCallback() --------------------- */

static
int GetZhuYenFuHao(pTdgtDraw, button_x, button_y, pn_symbol_ltx, pn_symbol_lty)
   TdgtDraw *pTdgtDraw;
   int button_x, button_y, *pn_symbol_ltx, *pn_symbol_lty;
{
   struct BBRec bbox;
   int x=0, y=0, row=0, col=0;

   bbox.ltx = pTdgtDraw->client_area.x;
   bbox.lty = pTdgtDraw->client_area.y;
   bbox.rbx = pTdgtDraw->client_area.x + pTdgtDraw->client_area.w;
   bbox.rby = pTdgtDraw->client_area.y + pTdgtDraw->client_area.h;

   if (!PointInBBox(button_x, button_y, bbox)) return INVALID;

   x = button_x-pTdgtDraw->client_area.x;
   y = button_y-pTdgtDraw->client_area.y;
   row = (int)(y/gZyfhInfo.zyfh_image_wh);
   col = (int)(x/gZyfhInfo.zyfh_image_wh);

   if (pn_symbol_ltx != NULL) {
      *pn_symbol_ltx = col*gZyfhInfo.zyfh_image_wh + pTdgtDraw->client_area.x;
   }
   if (pn_symbol_lty != NULL) {
      *pn_symbol_lty = row*gZyfhInfo.zyfh_image_wh + pTdgtDraw->client_area.y;
   }
   return ((col*gZyfhInfo.zyfh_num_rows)+row);
}

static
int ClickInZyfh(pTdgtDraw, button_ev)
   TdgtDraw *pTdgtDraw;
   XButtonEvent *button_ev;
{
   TdgtBase *pTdgtBase=
         (TdgtBase*)(pTdgtDraw->pti->tci.parent_tidgetinfo->tidget);
   ZyfhDlgInfo *pzdi=(ZyfhDlgInfo*)(pTdgtBase->pti->userdata);
   int down_ltx=0, down_lty=0, down_index=0, inside=TRUE, highlighted=TRUE;
   char *psz=NULL;

   down_index = GetZhuYenFuHao(pTdgtDraw, button_ev->x, button_ev->y,
         &down_ltx, &down_lty);
   if (down_index == INVALID) return FALSE;

   switch (pzdi->zyfh_spell_index) {
   case 0: psz=firstLargeTransTbl[down_index]; break;
   case 1: psz=secondLargeTransTbl[down_index]; break;
   case 2: psz=thirdLargeTransTbl[down_index]; break;
   case 3: psz=fourthLargeTransTbl[down_index]; break;
   }
   if (*psz != '\0') {
      /* clicked on a symbol */
      XFillRectangle(mainDisplay, pTdgtDraw->pti->tci.win, revDefaultGC,
            down_ltx, down_lty, gZyfhInfo.zyfh_image_wh,
            gZyfhInfo.zyfh_image_wh);
      if (!debugNoPointerGrab) {
         XGrabPointer(mainDisplay, pTdgtDraw->pti->tci.win, FALSE,
               PointerMotionMask | ButtonReleaseMask, GrabModeAsync,
               GrabModeAsync, None, defaultCursor, CurrentTime);
      }
      while (TRUE) {
         XEvent xev;

         XNextEvent(mainDisplay, &xev);
         if (xev.type == Expose) {
            ExposeEventHandler(&xev, FALSE);
         } else if (xev.type == ButtonRelease) {
            int up_index=0, up_ltx=0, up_lty=0;

            XUngrabPointer(mainDisplay, CurrentTime);
            XSync(mainDisplay, False);
            up_index = GetZhuYenFuHao(pTdgtDraw, xev.xbutton.x, xev.xbutton.y,
                  &up_ltx, &up_lty);
            if (up_index == down_index) {
               if (highlighted) {
                  XFillRectangle(mainDisplay, pTdgtDraw->pti->tci.win,
                        revDefaultGC, down_ltx, down_lty,
                        gZyfhInfo.zyfh_image_wh, gZyfhInfo.zyfh_image_wh);
                  highlighted = FALSE;
               }
               pzdi->zyfh_spell_buf[pzdi->zyfh_spell_index++] = down_index;
               UpdateSpellingAndDpyControls(pTdgtBase);
            }
            break;
         } else if (xev.type == MotionNotify) {
            int mouse_index=0, mouse_ltx=0, mouse_lty=0;
            XEvent tmp_xev;

            while (XCheckMaskEvent(mainDisplay, PointerMotionMask, &tmp_xev)) ;

            mouse_index = GetZhuYenFuHao(pTdgtDraw, xev.xmotion.x,
                  xev.xmotion.y, &mouse_ltx, &mouse_lty);
            if (inside) {
               if (mouse_index != down_index) {
                  XFillRectangle(mainDisplay, pTdgtDraw->pti->tci.win,
                        revDefaultGC, down_ltx, down_lty,
                        gZyfhInfo.zyfh_image_wh, gZyfhInfo.zyfh_image_wh);
                  highlighted = !highlighted;
                  inside = FALSE;
               }
            } else {
               if (mouse_index == down_index) {
                  XFillRectangle(mainDisplay, pTdgtDraw->pti->tci.win,
                        revDefaultGC, down_ltx, down_lty,
                        gZyfhInfo.zyfh_image_wh, gZyfhInfo.zyfh_image_wh);
                  highlighted = !highlighted;
                  inside = TRUE;
               }
            }
         }
      }
   }
   return FALSE;
}

static
int ZyfhEvHandlerCallback(pTdgtDraw, pXEv)
   TdgtDraw *pTdgtDraw;
   XEvent *pXEv;
{
   if (pXEv->xany.window == pTdgtDraw->pti->tci.win) {
      static char ppsz_buf[MAX_STATUS_BTNS+1][MAXSTRING+1];
      static int one_line_status=TRUE;

      if (pXEv->type == EnterNotify) {
         SaveStatusStringsIntoBuf(ppsz_buf, &one_line_status);
         SetMouseStatus(TgLoadString(STID_ADD_ZYFH),
               TgLoadString(STID_FETCH_CHAR), TgLoadString(STID_BACKOUT_ZYFH));
      } else if (pXEv->type == LeaveNotify) {
         RestoreStatusStringsFromBuf(ppsz_buf, one_line_status);
      } else if (pXEv->type == ButtonPress) {
         XButtonEvent *button_ev=(&(pXEv->xbutton));
         TdgtBase *pTdgtBase=
               (TdgtBase*)(pTdgtDraw->pti->tci.parent_tidgetinfo->tidget);
         ZyfhDlgInfo *pzdi=(ZyfhDlgInfo*)(pTdgtBase->pti->userdata);

         switch (button_ev->button) {
         case Button1:
            if (pzdi->english_spelling) {
               pzdi->english_spelling = FALSE;
               pzdi->zyfh_spell_index = 0;
            }
            if (pzdi->just_fetched) {
               ClearAllControls(pTdgtBase);
            }
            if (pzdi->zyfh_spell_index >= MAXZYFH_CHARS) {
               XBell(mainDisplay, 0);
            } else {
               return ClickInZyfh(pTdgtDraw, button_ev);
            }
            break;
         case Button2:
            if (pzdi->just_fetched) {
               return SendSelectedCharToClient(pTdgtBase);
            } else {
               return FetchChars(pTdgtBase);
            }
            break;
         case Button3:
            return HandleBackSpace(pTdgtBase);
         }
      }
   }
   return FALSE;
}

/* --------------------- ZyfhDpyRedrawCallback --------------------- */

static
int ZyfhDpyRedrawCallback(pTdgtDraw)
   TdgtDraw *pTdgtDraw;
   /* redraw the vertical display area on the left top area */
{
   TdgtBase *pTdgtBase=
         (TdgtBase*)(pTdgtDraw->pti->tci.parent_tidgetinfo->tidget);
   ZyfhDlgInfo *pzdi=(ZyfhDlgInfo*)(pTdgtBase->pti->userdata);

   if (pTdgtDraw == NULL) return FALSE;

   if (pzdi->english_spelling) {
      /* leave it blank if pinyin is used */
   } else {
      int i=0, x=pTdgtDraw->client_area.x, y=pTdgtDraw->client_area.y;

      if (pzdi->zyfh_spell_index == 0) return FALSE;

      for (i=0; i < pzdi->zyfh_spell_index; i++, y+=gZyfhInfo.zyfh_image_wh) {
         int src_x=0, src_y=0;
         XGCValues values;

         values.fill_style = FillSolid;
         values.function = GXcopy;
         XChangeGC(mainDisplay, gTidgetManager.gc, GCFillStyle | GCFunction,
               &values);

         ReverseZhuYenFuHao(pzdi->zyfh_spell_buf[i], &src_x, &src_y);
         XCopyArea(mainDisplay, gZyfhInfo.zyfh_bitmap, gZyfhInfo.symbol_bitmap,
               xbmGC, src_x, src_y,
               gZyfhInfo.zyfh_image_wh, gZyfhInfo.zyfh_image_wh, 0, 0);

         values.fill_style = FillStippled;
         values.ts_x_origin = x;
         values.ts_y_origin = y;
         values.stipple = gZyfhInfo.symbol_bitmap;
         XChangeGC (mainDisplay, gTidgetManager.gc, 
               GCFillStyle | GCTileStipXOrigin | GCTileStipYOrigin | GCStipple,
               &values);

         XFillRectangle(mainDisplay, pTdgtDraw->pti->tci.win,
               gTidgetManager.gc, x, y,
               gZyfhInfo.zyfh_image_wh, gZyfhInfo.zyfh_image_wh);

         TidgetManagerResetGC();
      }
   }
   return TRUE;
}

/* --------------------- ZyfhDpySendCmdCallback --------------------- */

static
int ZyfhDpySendCmdCallback(pTdgtDraw, cmd_type, cmd_arg, pv_cmd_userdata)
   TdgtDraw *pTdgtDraw;
   int cmd_type, cmd_arg;
   void *pv_cmd_userdata;
{
   TdgtBase *pTdgtBase=NULL;
   ZyfhDlgInfo *pzdi=NULL;

   switch (cmd_type) {
   case TDGTCMD_DRAW_USER:
      switch (cmd_arg) {
      case TDGTARG_ZYFH_DPY_CLEAR:
         pTdgtBase = (TdgtBase*)pv_cmd_userdata;
         pzdi = (ZyfhDlgInfo*)(pTdgtBase->pti->userdata);
         /* clear it */
         RedrawTidget(pzdi->zyfh_dpy_ctl->pti);
         break;
      case TDGTARG_ZYFH_DPY_APPEND:
         break;
      }
      break;
   }
   return FALSE;
}

/* ------------------- Tdgtb5DlgKeyPressEvHandlerCallback ------------------- */

static
int HandleCursorKey(pTdgtBase, key_sym)
   TdgtBase *pTdgtBase;
   KeySym key_sym;
{
   ZyfhDlgInfo *pzdi=(ZyfhDlgInfo*)(pTdgtBase->pti->userdata);
   int cmd_arg=0;

   switch (key_sym) {
   case XK_Up: cmd_arg=TDGTARG_MOVE_UP; break;
   case XK_KP_Up: cmd_arg=TDGTARG_MOVE_UP; break;
   case XK_Down: cmd_arg=TDGTARG_MOVE_DOWN; break;
   case XK_KP_Down: cmd_arg=TDGTARG_MOVE_DOWN; break;
   case XK_Left: cmd_arg=TDGTARG_MOVE_LEFT; break;
   case XK_KP_Left: cmd_arg=TDGTARG_MOVE_LEFT; break;
   case XK_Right: cmd_arg=TDGTARG_MOVE_RIGHT; break;
   case XK_KP_Right: cmd_arg=TDGTARG_MOVE_RIGHT; break;
   default:
      switch (key_sym & 0x0ff) {
      case 'n': cmd_arg=TDGTARG_MOVE_RIGHT; break;
      case 'N': cmd_arg=TDGTARG_MOVE_RIGHT; break;
      case 'f': cmd_arg=TDGTARG_MOVE_RIGHT; break;
      case 'F': cmd_arg=TDGTARG_MOVE_RIGHT; break;
      case 'p': cmd_arg=TDGTARG_MOVE_LEFT; break;
      case 'P': cmd_arg=TDGTARG_MOVE_LEFT; break;
      case 'b': cmd_arg=TDGTARG_MOVE_LEFT; break;
      case 'B': cmd_arg=TDGTARG_MOVE_LEFT; break;
      default: return FALSE;
      }
      break;
   }
   TidgetSendCmd(pzdi->bmp_list_ctl->pti, TDGTCMD_LIST_SEL_NEXT, cmd_arg, NULL);

   return FALSE;
}

static
int HandleChar(pTdgtBase, key_sym, buf, has_ch)
   TdgtBase *pTdgtBase;
   KeySym key_sym;
   char *buf;
   int has_ch;
{
   ZyfhDlgInfo *pzdi=(ZyfhDlgInfo*)(pTdgtBase->pti->userdata);

   if (!pzdi->english_spelling) {
      pzdi->english_spelling = TRUE;
      ClearAllControls((TdgtBase*)(gZyfhInfo.dialogbox_tidgetinfo->tidget));
   }
   if (pzdi->just_fetched) {
      ClearAllControls((TdgtBase*)(gZyfhInfo.dialogbox_tidgetinfo->tidget));
   }
   TdgtSmplEditAppendStr(pzdi->edit_ctl, buf, 1);

   return FALSE;
}

static XComposeStatus c_stat;

static
int Tdgtb5DlgKeyPressEvHandlerCallback(pTdgtBase, pXEv)
   TdgtBase *pTdgtBase;
   XEvent *pXEv;
{
   ZyfhDlgInfo *pzdi=(ZyfhDlgInfo*)(pTdgtBase->pti->userdata);
   int has_ch=0;
   char buf[80];
   KeySym key_sym=(KeySym)0;
   XKeyEvent *key_ev=NULL;

   if (pXEv->type != KeyPress) return FALSE;
   if (pzdi == NULL) return FALSE;

   key_ev = (&(pXEv->xkey));
   has_ch = XLookupString(key_ev, buf, sizeof(buf), &key_sym, &c_stat);
   TranslateKeys(buf, &key_sym);

   if (CharIsESC(key_ev, buf, key_sym, &has_ch)) {
      HideTdgtb5DialogBox();
   } else if (CharIsCRorLF(key_ev, buf, key_sym, &has_ch)) {
      if (pzdi->just_fetched) {
         return SendSelectedCharToClient(pTdgtBase);
      } else {
         return FetchChars(pTdgtBase);
      }
   } else if (CharIsCntrlSpace(key_ev, buf, key_sym, &has_ch)) {
      HideTdgtb5DialogBox();
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

/* --------------------- Tdgtb5DlgWmDeleteCallback --------------------- */

static
void Tdgtb5DlgWmDeleteCallback(pTdgtBase)
   TdgtBase *pTdgtBase;
{
   ZyfhDlgInfo *pzdi=(ZyfhDlgInfo*)(pTdgtBase->pti->userdata);

   if (pzdi == NULL) return;

   HideTdgtb5DialogBox();
}

/* --------------------- Tdgtb5DlgSetMsg --------------------- */

static
int Tdgtb5DlgSetMsg(pti, msg)
   TidgetInfo *pti;
   char *msg;
{
   return TRUE;
}

/* --------------------- Tdgtb5DlgSetEntries --------------------- */

static
int Tdgtb5DlgSetEntries(pti, dsp_ptr, entries, num_entries, marked_index)
   TidgetInfo *pti;
   DspList *dsp_ptr;
   char **entries;
   int num_entries, marked_index;
{
   return TRUE;
}

/* --------------------- InitTdgtb5DlgInfo --------------------- */

static
void InitTdgtb5DlgInfo(pti)
   TidgetInfo *pti;
{
   ZyfhDlgInfo *pzdi=NULL;

   if (pti == NULL) return;

   pzdi = (ZyfhDlgInfo*)(pti->userdata);
   if (pzdi == NULL) return;

   pzdi->num_entries = INVALID;
   pzdi->first_index = INVALID;
   pzdi->marked_index = INVALID;

   pzdi->screen_w = DisplayWidth(mainDisplay, mainScreen);
   pzdi->screen_h = DisplayHeight(mainDisplay, mainScreen);
   Tdgtb5DlgSetMsg(pti, "");

   pzdi->def_btn_id = BUTTON_OK;
   pzdi->double_click_btn_id = BUTTON_OK;
}

/* --------------------- ShowTdgtb5DialogBox --------------------- */

void ShowTdgtb5DialogBox()
{
   TidgetInfo *pti=gZyfhInfo.dialogbox_tidgetinfo;
   ZyfhDlgInfo *pzdi=(ZyfhDlgInfo*)(pti->userdata);

   TidgetManagerSetWantAllKeyPressEvents(pti, TRUE);
   gZyfhInfo.mapped = TRUE;
   MapTidget(pti);

   if (pzdi != NULL) {
      ClearAllControls((TdgtBase*)(pti->tidget));
      pzdi->english_spelling = FALSE;
   }
}

/* --------------------- HideTdgtb5DialogBox --------------------- */

void HideTdgtb5DialogBox()
{
   TidgetManagerSetWantAllKeyPressEvents(NULL, FALSE);
   gZyfhInfo.mapped = FALSE;
   XUnmapWindow(mainDisplay, gZyfhInfo.dialogbox_tidgetinfo->tci.win);

   if (warpToWinCenter) {
      XWarpPointer(mainDisplay, None, drawWindow, 0, 0, 0, 0,
            (int)(ZOOMED_SIZE(drawWinW)>>1), (int)(ZOOMED_SIZE(drawWinH)>>1));
   }
}

/* --------------------- CreateTdgtb5DialogBox --------------------- */

typedef struct tagCreateBtnInfo {
   char *text;
   int ctl_id;
   int cstid;
   int msg_id;
} CreateBtnInfo;

#define NUM_ZHFH_BUTTONS 4

static CreateBtnInfo cbi[NUM_ZHFH_BUTTONS]={
   { "Fetch", ID_ZYFH_BTN_FETCH, FALSE, STID_FETCH_CHARS_FROM_DB },
   { "Send",  ID_ZYFH_BTN_SEND,  FALSE, STID_SEND_SEL_CHAR_TO_DRAWING_AREA },
   { "<CR>",  ID_ZYFH_BTN_CR,    FALSE, STID_SEND_CR_TO_DRAWING_AREA },
   { "Close", ID_ZYFH_BTN_CLOSE, TRUE,  CSTID_CLOSE }
};

static
int CreateTgtwb5Buttons(pTdgtBase)
   TdgtBase *pTdgtBase;
{
   ZyfhDlgInfo *pzdi=(ZyfhDlgInfo*)(pTdgtBase->pti->userdata);
   TdgtBtnRow *pTdgtBtnRow=pzdi->btn_row_ctl;
   int i=0;

   for (i=0; i < NUM_ZHFH_BUTTONS; i++) {
      TdgtBtn *pTdgtBtn=NULL;
      MouseOverStatusInfo mosi;
      int btn_w=0, btn_h=0;

      memset(&mosi, 0, sizeof(MouseOverStatusInfo));
      mosi.one_line_status = TRUE;
      UtilStrCpyN(mosi.one_line_str, sizeof(mosi.one_line_str),
            cbi[i].cstid ? TgLoadCachedString(cbi[i].msg_id) :
            TgLoadString(cbi[i].msg_id));

      CalcTdgtBtnDim(cbi[i].text, 8, TDGTBTN_DEF_H_PAD, TDGTBTN_DEF_V_PAD,
            &btn_w, &btn_h);
      pTdgtBtn = CreateTdgtBtn(pTdgtBtnRow->pti->tci.win,
            pTdgtBtnRow->pti, cbi[i].ctl_id, 0, 0, btn_w, btn_h,
            TDGTBTN_DEF_H_PAD, TDGTBTN_DEF_V_PAD, TGMUTYPE_TEXT, TDGTBTN_CLICK,
            TGBS_NORMAL, STYLE_NR, cbi[i].text, &mosi);
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
   ZyfhDlgInfo *pzdi=(ZyfhDlgInfo*)(pTdgtBase->pti->userdata);
   int max_w=0, bottom=0, h_pad=pTdgtBase->pti->tci.h_pad;
   int v_pad=pTdgtBase->pti->tci.v_pad;
   SimpleWinInfo *pswi=NULL;

   if (pzdi->msg_ctl != NULL) {
      pswi = (&(pzdi->msg_ctl->pti->tci.win_info));
      if (pswi->w > max_w) max_w = pswi->w;
   }
   pswi = (&(pzdi->zyfh_ctl->pti->tci.win_info));
   if (pswi->x+pswi->w > max_w) max_w = pswi->x+pswi->w;

   pswi = (&(pzdi->bmp_list_ctl->pti->tci.win_info));
   if (pswi->w > max_w) max_w = pswi->w;

   pswi = (&(pzdi->btn_row_ctl->pti->tci.win_info));
   if (pswi->w > max_w) max_w = pswi->w;
   bottom = pswi->y+pswi->h;

   if (pzdi->msg_ctl != NULL) {
      pswi = (&(pzdi->msg_ctl->pti->tci.win_info));
      pswi->x = ((max_w-pswi->w)>>1) + windowPadding + h_pad;
      TidgetMoveResize(pzdi->msg_ctl->pti, pswi->x, pswi->y, pswi->w, pswi->h);
   }
   pswi = (&(pzdi->edit_ctl->pti->tci.win_info));
   pswi->w = max_w;
   TidgetMoveResize(pzdi->edit_ctl->pti, pswi->x, pswi->y, pswi->w, pswi->h);

   pswi = (&(pzdi->bmp_list_ctl->pti->tci.win_info));
   pswi->w = max_w;
   TidgetMoveResize(pzdi->bmp_list_ctl->pti, pswi->x, pswi->y, pswi->w,
         pswi->h);

   pswi = (&(pzdi->btn_row_ctl->pti->tci.win_info));
   pswi->w = max_w;
   TidgetMoveResize(pzdi->btn_row_ctl->pti, pswi->x, pswi->y, pswi->w, pswi->h);

   pswi = (&(pTdgtBase->pti->tci.win_info));
   pswi->w = max_w+(windowPadding<<1)+(h_pad<<1);
   pswi->h = bottom+windowPadding+v_pad;
   TidgetMoveResize(pTdgtBase->pti, pswi->x, pswi->y, pswi->w, pswi->h);
}

static
void AdjustDialogBoxSize(dpy, pTdgtBase)
   Display *dpy;
   TdgtBase *pTdgtBase;
{
   ZyfhDlgInfo *pzdi=(ZyfhDlgInfo*)(pTdgtBase->pti->userdata);
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
   
   base_win_x = (base_win_w > pzdi->screen_w) ? 0 :
         ((pzdi->screen_w-base_win_w)>>1);
   base_win_y = (base_win_h > pzdi->screen_h) ? 0 :
         ((pzdi->screen_h-base_win_h)/3);

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

TidgetInfo *CreateTdgtb5DialogBox(dpy, parent_win)
   Display *dpy;
   Window parent_win; /* should be the rootWindow */
{
   int orig_x=0, x=0, y=0, v_gap=20, num_bmp_cols=0, h_pad=0, v_pad=0;
   int max_w=((gZyfhInfo.b5_font_w<<4)+scrollBarW), vh_padding=0;
   int has_msg_ctl=FALSE;
   TdgtBase *pTdgtBase=NULL;
   ZyfhDlgInfo *pzdi=(ZyfhDlgInfo*)malloc(sizeof(ZyfhDlgInfo));
   char caption[MAXSTRING];
   MouseOverStatusInfo mosi;

   if (pzdi == NULL) FailAllocMessage();
   memset(pzdi, 0, sizeof(ZyfhDlgInfo));

   memset(&mosi, 0, sizeof(MouseOverStatusInfo));
   UtilStrCpyN(mosi.btn_str[0], sizeof(mosi.btn_str[0]),
         TgLoadString(STID_SELECT_A_CHAR));
   UtilStrCpyN(mosi.btn_str[1], sizeof(mosi.btn_str[1]),
         TgLoadString(STID_SEND_A_CHAR_TO_DRAWING_AREA));
   UtilStrCpyN(mosi.btn_str[2], sizeof(mosi.btn_str[2]),
         TgLoadCachedString(CSTID_PARANED_NONE));

   h_pad = TDGTBASE_DEF_H_PAD;
   v_pad = TDGTBASE_DEF_V_PAD;
   if (msgFontSet != NULL || msgFontPtr != NULL) {
      vh_padding = (msgFontWidth<<1);
   } else {
      vh_padding = (defaultFontWidth<<1);
   }
   h_pad = v_pad = vh_padding;

   orig_x = h_pad+windowPadding;
   x = h_pad+windowPadding;
   y = v_pad+windowPadding;

   switch (gZyfhInfo.b5_font_real_encoding) {
   case GB_ENCODING:
      sprintf(caption, TgLoadString(STID_TOOL_GB_CHINESE_KEYPAD), TOOL_NAME);
      break;
   case BIG5_ENCODING:
      sprintf(caption, TgLoadString(STID_TOOL_BIG5_CHINESE_KEYPAD), TOOL_NAME);
      break;
   default: return NULL;
   }
   pTdgtBase = CreateTdgtBase(parent_win, NULL, ID_ZYFH_DIALOG, 0, 0, 100, 100,
         h_pad, v_pad, (dialogboxUse3DBorder ? TGBS_RAISED : TGBS_NORMAL),
         caption);
   if (pTdgtBase == NULL) {
      free(pzdi);
      return NULL;
   }
   pTdgtBase->pti->userdata = pzdi;
   InitTdgtb5DlgInfo(pTdgtBase->pti);

   num_bmp_cols = (int)(((double)(pzdi->screen_w)) / ((double)3) /
         ((double)(gZyfhInfo.b5_font_w)));

   if (has_msg_ctl) {
      pzdi->msg_ctl = CreateTdgtMsg(pTdgtBase->pti->tci.win, pTdgtBase->pti,
            INVALID, x, y, TDGTMSG_DEF_H_PAD, TDGTMSG_DEF_V_PAD, TGBS_NORMAL,
            TgLoadString(STID_USE_ZYFH_KEYPAD_OR_PINYING), JUST_L);
      if (pzdi->msg_ctl == NULL) {
         DestroyTidget(&pTdgtBase->pti);
         return NULL;
      }
      y += pzdi->msg_ctl->pti->tci.win_info.h;
      y += v_gap;
   }
   pzdi->zyfh_dpy_ctl = CreateTdgtDraw(pTdgtBase->pti->tci.win, pTdgtBase->pti,
         ID_ZYFH_DPY, x, y, gZyfhInfo.zyfh_image_wh,
         ((gZyfhInfo.zyfh_image_wh)<<2), TDGTDRAW_DEF_H_PAD, TDGTDRAW_DEF_V_PAD,
         TGBS_LOWRED, NULL);
   if (pzdi->zyfh_dpy_ctl == NULL) {
      DestroyTidget(&pTdgtBase->pti);
      return NULL;
   }
   x += ((gZyfhInfo.zyfh_image_wh)<<2);
   pzdi->zyfh_ctl = CreateTdgtDraw(pTdgtBase->pti->tci.win, pTdgtBase->pti,
         ID_ZYFH_KEYPAD, x, y, gZyfhInfo.zyfh_bitmap_w, gZyfhInfo.zyfh_bitmap_h,
         TDGTDRAW_DEF_H_PAD, TDGTDRAW_DEF_V_PAD, TGBS_LOWRED, NULL);
   if (pzdi->zyfh_ctl == NULL) {
      DestroyTidget(&pTdgtBase->pti);
      return NULL;
   }
   TdgtDrawSetRedrawCallback(pzdi->zyfh_ctl, ZyfhRedrawCallback);
   TdgtDrawSetEvHandlerCallback(pzdi->zyfh_ctl, ZyfhEvHandlerCallback);

   TdgtDrawSetRedrawCallback(pzdi->zyfh_dpy_ctl, ZyfhDpyRedrawCallback);
   TdgtDrawSetSendCmdCallback(pzdi->zyfh_dpy_ctl, ZyfhDpySendCmdCallback);

   x = orig_x;
   y += pzdi->zyfh_ctl->pti->tci.win_info.h;
   y += v_gap;
   pzdi->edit_ctl = CreateTdgtSmplEdit(pTdgtBase->pti->tci.win, pTdgtBase->pti,
         ID_ZYFH_EDIT, x, y, max_w, TDGTSEDT_DEF_H_PAD, TDGTSEDT_DEF_V_PAD, "",
         STYLE_NR, INVALID);
   if (pzdi->edit_ctl == NULL) {
      DestroyTidget(&pTdgtBase->pti);
      return NULL;
   }
   y += pzdi->edit_ctl->pti->tci.win_info.h;
   y += v_gap;
   pzdi->bmp_list_ctl = CreateTdgtBmpList(pTdgtBase->pti->tci.win,
         pTdgtBase->pti, ID_ZYFH_BMPLIST, x, y, TDGTBMPL_DEF_H_PAD,
         TDGTBMPL_DEF_V_PAD, gZyfhInfo.b5_font_w, gZyfhInfo.b5_font_h,
         num_bmp_cols, 5, 0, (Button1Mask|Button2Mask), &mosi);
   if (pzdi->bmp_list_ctl == NULL) {
      DestroyTidget(&pTdgtBase->pti);
      return NULL;
   }
   y += pzdi->bmp_list_ctl->pti->tci.win_info.h;
   y += v_gap;
   pzdi->btn_row_ctl = CreateTdgtBtnRow(pTdgtBase->pti->tci.win, pTdgtBase->pti,
         INVALID, x, y, TDGTBROW_DEF_H_PAD, TDGTBROW_DEF_V_PAD, TGBS_NORMAL,
         defaultFontWidth, JUST_C);
   if (pzdi->btn_row_ctl == NULL) {
      DestroyTidget(&pTdgtBase->pti);
      return NULL;
   }
   if (!CreateTgtwb5Buttons(pTdgtBase)) {
      DestroyTidget(&pTdgtBase->pti);
      return NULL;
   }
   Tdgtb5DlgSetEntries(pTdgtBase->pti, NULL, NULL, 0, INVALID);

   /*
    * Specify that the base tidget will handle all the KeyPress events
    *         in all its children tidgets.
    */
   TdgtBaseWillHandleKeyPressEvents(pTdgtBase,
         Tdgtb5DlgKeyPressEvHandlerCallback);
   /*
    * Specify that the base tidget will handle all the KeyPress events
    *         in all its children tidgets.
    */
   TdgtBaseSetWmDeleteCallback(pTdgtBase, Tdgtb5DlgWmDeleteCallback);

   AdjustDialogBoxSize(dpy, pTdgtBase);

   return pTdgtBase->pti;
}

/* --------------------- DestroyTdgtb5DialogBox --------------------- */

void DestroyTdgtb5DialogBox(dpy, pti)
   Display *dpy;
   TidgetInfo *pti;
{
   ZyfhDlgInfo *pzdi=NULL;

   if (pti == NULL) return;

   pzdi = (ZyfhDlgInfo*)(pti->userdata);
   if (pzdi != NULL) {
      free(pzdi);
   }
   DestroyTidget(&pti);
}

/* --------------------- Tdgtb5DlgLoop --------------------- */

static
int HandleTdgtNotifyEvent(pTdgtBase, pTdgtNtfy)
   TdgtBase *pTdgtBase;
   TdgtNtfy *pTdgtNtfy;
{
   switch (pTdgtNtfy->ctl_id) {
   case ID_ZYFH_BMPLIST:
      switch (pTdgtNtfy->nf_type) {
      case TDGTNF_LIST_CLICKED:
         if (pTdgtNtfy->nf_arg2 == Button2) {
            return SendSelectedCharToClient(pTdgtBase);
         }
         break;
      case TDGTNF_LIST_DBL_CLICKED:
         return SendSelectedCharToClient(pTdgtBase);
      }
      break;
   case ID_ZYFH_BTN_FETCH: FetchChars(pTdgtBase); break;
   case ID_ZYFH_BTN_SEND: SendSelectedCharToClient(pTdgtBase); break;
   case ID_ZYFH_BTN_CR: SendCRToClient(pTdgtBase); break;
   case ID_ZYFH_BTN_CLOSE: HideTdgtb5DialogBox(); break;
   }
   return FALSE;
}

int Tdgtb5DlgLoop(pti)
   TidgetInfo *pti;
{
   TdgtBase *pTdgtBase=(TdgtBase*)(pti->tidget);
   int which_btn_clicked=INVALID, first_time_config=TRUE;

   if (pTdgtBase == NULL) return which_btn_clicked;

   while (gZyfhInfo.mapped) {
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
            SetStringStatus(TgLoadString(STID_CLICK_SEND_TO_SEND_CHAR));
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
      }
   }
   return which_btn_clicked;
}

/* --------------------- Init & Clean Up --------------------- */

int InitTdgtb5Dlg()
{
   return TRUE;
}

void CleanUpTdgtb5Dlg()
{
}

#endif /* ~_NO_TGTWB5 */
