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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/tgtwb5.c,v 1.11 2011/05/16 16:22:00 william Exp $
 */

#ifndef _NO_TGTWB5

#define _INCLUDE_FROM_TGTWB5_C_

#include "tgifdefs.h"
#include "big5.h"
#include "tidget.h"

#include "auxtext.e"
#include "dialog.e"
#include "font.e"
#include "msg.e"
#include "setup.e"
#include "strtbl.e"
#include "tgtwb5.e"
#include "tgtwb5dl.e"
#include "tgtwb5xl.e"
#include "tidget.e"
#include "util.e"

#include "xbm/tgtwb5.xbm"

/* --------------------- Init & Clean Up --------------------- */

void Tgtwb5_CleanUp(dpy, win)
   Display *dpy;
   Window win;
{
   if (gZyfhInfo.initialized) {
      if (gZyfhInfo.dialogbox_tidgetinfo != NULL) {
         DestroyTdgtb5DialogBox(dpy, gZyfhInfo.dialogbox_tidgetinfo);
      }
   }
   UtilFree(gZyfhInfo.b5_font_name);
   if (gZyfhInfo.xfs != NULL) {
      XFreeFont(mainDisplay, gZyfhInfo.xfs);
   }
   if (gZyfhInfo.zyfh_bitmap != None) {
      XFreePixmap(mainDisplay, gZyfhInfo.zyfh_bitmap);
   }
   if (gZyfhInfo.symbol_bitmap != None) {
      XFreePixmap(mainDisplay, gZyfhInfo.symbol_bitmap);
   }
   memset(&gZyfhInfo, 0, sizeof(ZyfhInfo));

   CleanUpTdgtb5Dlg();
}

static char szDefB5FontName[] = 
      "-taipei-fixed-medium-r-normal--16-150-75-75-c-160-big5-0";

int Tgtwb5_Init(dpy, win, arg)
   Display *dpy;
   Window win;
   char *arg;
{
   memset(&gZyfhInfo, 0, sizeof(ZyfhInfo));
   if (arg != NULL) {
      gZyfhInfo.b5_font_name = UtilStrDup(arg);
      if (gZyfhInfo.b5_font_name == NULL) FailAllocMessage();
   } else {
      gZyfhInfo.b5_font_name = UtilStrDup(szDefB5FontName);
      if (gZyfhInfo.b5_font_name == NULL) FailAllocMessage();
   }
   if (strstr(gZyfhInfo.b5_font_name, "-gb2312") != NULL) {
      gZyfhInfo.b5_font_real_encoding = GB_ENCODING;
      UtilStrCpyN(gZyfhInfo.im_name, sizeof(gZyfhInfo.im_name), "tgchgb");
   } else if (strstr(gZyfhInfo.b5_font_name, "-big5") != NULL) {
      gZyfhInfo.b5_font_real_encoding = BIG5_ENCODING;
      UtilStrCpyN(gZyfhInfo.im_name, sizeof(gZyfhInfo.im_name), "tgtwb5");
   } else {
      gZyfhInfo.b5_font_real_encoding = INVALID;
      sprintf(gszMsgBox, TgLoadString(STID_BAD_ENCODING_TGTWB5_FONT),
            gZyfhInfo.b5_font_name, TOOL_NAME, "DoubleByteInputMethod");
      fprintf(stderr, "%s\n", gszMsgBox);

      Tgtwb5_CleanUp(dpy, win);

      return FALSE;
   }
   gZyfhInfo.xfs = LoadAFontByName(gZyfhInfo.b5_font_name, TRUE, TRUE);
   if (gZyfhInfo.xfs == NULL) {
      sprintf(gszMsgBox, TgLoadString(STID_CANNOT_LOAD_TGTWB5_FONT),
            gZyfhInfo.b5_font_name, TOOL_NAME, "DoubleByteInputMethod",
            gZyfhInfo.im_name);
      fprintf(stderr, "%s\n", gszMsgBox);

      Tgtwb5_CleanUp(dpy, win);

      return FALSE;
   }
   gZyfhInfo.b5_font_mod_bytes = ((gZyfhInfo.xfs->min_byte1 & 0x80) == 0);
   gZyfhInfo.b5_font_w = gZyfhInfo.xfs->max_bounds.rbearing;
   gZyfhInfo.b5_font_asc = gZyfhInfo.xfs->max_bounds.ascent;
   gZyfhInfo.b5_font_des = gZyfhInfo.xfs->max_bounds.descent;
   gZyfhInfo.b5_font_h = gZyfhInfo.b5_font_asc + gZyfhInfo.b5_font_des;

   gZyfhInfo.zyfh_num_cols = 13;
   gZyfhInfo.zyfh_num_rows = 4;
   gZyfhInfo.zyfh_max_symbols = gZyfhInfo.zyfh_num_cols*gZyfhInfo.zyfh_num_rows;
   gZyfhInfo.zyfh_bitmap_w = tgtwb5_width;
   gZyfhInfo.zyfh_bitmap_h = tgtwb5_height;
   gZyfhInfo.zyfh_image_wh =
         round(((double)tgtwb5_width)/((double)gZyfhInfo.zyfh_num_cols));
   gZyfhInfo.zyfh_bitmap = XCreateBitmapFromData(mainDisplay, mainWindow,
           (char*)tgtwb5_bits, tgtwb5_width, tgtwb5_height);

   gZyfhInfo.symbol_bitmap = XCreatePixmap(mainDisplay, rootWindow,
         gZyfhInfo.zyfh_image_wh, gZyfhInfo.zyfh_image_wh, 1);

   if (!InitTdgtb5Dlg()) {
      Tgtwb5_CleanUp(dpy, win);

      return FALSE;
   }
   return TRUE;
}

/* --------------------- Tgtwb5_SendKey --------------------- */

static XComposeStatus c_stat;

int Tgtwb5_SendKey(dpy, win, key_ev, buf)
   Display *dpy;
   Window win;
   XKeyEvent *key_ev;
   char *buf;
   /* return FALSE means key_ev will be handled without modifications */
{
   int has_ch=0;
   char s[80];
   KeySym key_sym=(KeySym)0;

   has_ch = XLookupString(key_ev, s, sizeof(s), &key_sym, &c_stat);
   TranslateKeys(s, &key_sym);

   return FALSE;
}

/* --------------------- Tgtwb5_HandleCntrlSpace --------------------- */

int Tgtwb5_HandleCntrlSpace(dpy, win)
   Display *dpy;
   Window win;
{
   if (!gZyfhInfo.initialized) {
      gZyfhInfo.initialized = TRUE;
      /* create window */
      gZyfhInfo.dialogbox_tidgetinfo = CreateTdgtb5DialogBox(dpy, rootWindow);
      if (gZyfhInfo.dialogbox_tidgetinfo == NULL) {
         /* print a message */
      }
   }
   if (gZyfhInfo.dialogbox_tidgetinfo == NULL) {
      return FALSE;
   }
   if (!gZyfhInfo.mapped) {
      ShowTdgtb5DialogBox();
      Tdgtb5DlgLoop(gZyfhInfo.dialogbox_tidgetinfo);
   } else {
      HideTdgtb5DialogBox();
   }
   return FALSE;
}

/* --------------------- Tgtwb5_HandleCreateText --------------------- */

int Tgtwb5_HandleCreateText(dpy, win)
   Display *dpy;
   Window win;
{
   if (gZyfhInfo.initialized && gZyfhInfo.mapped) {
      HideTdgtb5DialogBox();
   }
   return TRUE;
}

#endif /* ~_NO_TGTWB5 */

