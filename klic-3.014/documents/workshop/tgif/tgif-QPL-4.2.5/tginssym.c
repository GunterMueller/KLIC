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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/tginssym.c,v 1.5 2011/06/02 15:53:08 william Exp $
 */

#define _INCLUDE_FROM_TGINSSYM__C_

#include "tgifdefs.h"
#include "tidget.h"

#include "auxtext.e"
#include "dialog.e"
#include "font.e"
#include "msg.e"
#include "setup.e"
#include "strtbl.e"
#include "tginssym.e"
#include "tgisdl.e"
#include "tidget.e"
#include "util.e"

/* --------------------- Init & Clean Up --------------------- */

void TgInsertSymbol_CleanUp(dpy, win)
   Display *dpy;
   Window win;
{
   if (gInsertSymbolInfo.initialized) {
      if (gInsertSymbolInfo.dialogbox_tidgetinfo != NULL) {
         DestroyTdgtInsertSymbolDialogBox(dpy,
               gInsertSymbolInfo.dialogbox_tidgetinfo);
      }
   }
   if (gInsertSymbolInfo.sym_pixmap != None) {
      XFreePixmap(mainDisplay, gInsertSymbolInfo.sym_pixmap);
   }
   if (gInsertSymbolInfo.xfs != NULL) {
      XFreeFont(mainDisplay, gInsertSymbolInfo.xfs);
   }
   if (gInsertSymbolInfo.gc != NULL) {
      XFreeGC(mainDisplay, gInsertSymbolInfo.gc);
   }
   memset(&gInsertSymbolInfo, 0, sizeof(InsertSymbolInfo));

   CleanUpTdgtInsertSymbolDlg();
}

int TgInsertSymbol_Init(dpy, win, arg)
   Display *dpy;
   Window win;
   char *arg;
{
   XGCValues values;

   memset(&gInsertSymbolInfo, 0, sizeof(InsertSymbolInfo));

   gInsertSymbolInfo.xfs = LoadASymbolFont(14);
   if (gInsertSymbolInfo.xfs == NULL) {
      TgInsertSymbol_CleanUp(dpy, win);
      return FALSE;
   }
   gInsertSymbolInfo.font_width = gInsertSymbolInfo.xfs->max_bounds.width;
   gInsertSymbolInfo.font_asc = gInsertSymbolInfo.xfs->max_bounds.ascent;
   gInsertSymbolInfo.font_des = gInsertSymbolInfo.xfs->max_bounds.descent;
   gInsertSymbolInfo.font_height = gInsertSymbolInfo.font_asc +
         gInsertSymbolInfo.font_des;
   gInsertSymbolInfo.padding = 2;

   values.foreground = myFgPixel;
   values.background = myBgPixel;
   values.fill_style = FillSolid;
   values.font = gInsertSymbolInfo.xfs->fid;

   gInsertSymbolInfo.gc = XCreateGC(mainDisplay, mainWindow,
         GCForeground | GCBackground | GCFillStyle | GCFont, &values);

   if (gInsertSymbolInfo.gc == NULL) {
      TgInsertSymbol_CleanUp(dpy, win);
      return FALSE;
   }
   gInsertSymbolInfo.a_symbol_w = gInsertSymbolInfo.font_width +
         (gInsertSymbolInfo.padding<<1);
   gInsertSymbolInfo.a_symbol_h = gInsertSymbolInfo.font_height +
         (gInsertSymbolInfo.padding<<1);
   gInsertSymbolInfo.pixmap_w = TGIS_COLS*(gInsertSymbolInfo.a_symbol_w+1) + 1;
   gInsertSymbolInfo.pixmap_h = TGIS_ROWS*(gInsertSymbolInfo.a_symbol_h+1) + 1;

   gInsertSymbolInfo.sym_pixmap = XCreatePixmap(mainDisplay, rootWindow,
         gInsertSymbolInfo.pixmap_w, gInsertSymbolInfo.pixmap_h, mainDepth);
   if (gInsertSymbolInfo.sym_pixmap == None) {
      FailAllocPixmapMessage(gInsertSymbolInfo.pixmap_w,
            gInsertSymbolInfo.pixmap_h);
      TgInsertSymbol_CleanUp(dpy, win);
      return FALSE;
   }
   if (!InitTdgtInsertSymbolDlg()) {
      TgInsertSymbol_CleanUp(dpy, win);

      return FALSE;
   }
   return TRUE;
}

void CleanUpInsertSymbol()
{
   TgInsertSymbol_CleanUp(mainDisplay, drawWindow);
}

int InitInsertSymbol()
{
   return TgInsertSymbol_Init(mainDisplay, drawWindow, NULL);
}

/* --------------------- TgInsertSymbol_SendKey --------------------- */

static XComposeStatus c_stat;

int TgInsertSymbol_SendKey(dpy, win, key_ev, buf)
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

/* --------------------- TgInsertSymbol_HandleCntrlSpace --------------------- */

int TgInsertSymbol_HandleCntrlSpace(dpy, win)
   Display *dpy;
   Window win;
{
   if (!gInsertSymbolInfo.initialized) {
      gInsertSymbolInfo.initialized = TRUE;
      /* create window */
      gInsertSymbolInfo.dialogbox_tidgetinfo =
            CreateTdgtInsertSymbolDialogBox(dpy, rootWindow);
      if (gInsertSymbolInfo.dialogbox_tidgetinfo == NULL) {
         /* print a message */
      }
   }
   if (gInsertSymbolInfo.dialogbox_tidgetinfo == NULL) {
      return FALSE;
   }
   if (!gInsertSymbolInfo.mapped) {
      ShowTdgtInsertSymbolDialogBox();
      TdgtInsertSymbolDlgLoop(gInsertSymbolInfo.dialogbox_tidgetinfo);
   } else {
      HideTdgtInsertSymbolDialogBox();
   }
   return FALSE;
}

/* --------------------- TgInsertSymbol_HandleCreateText --------------------- */

int TgInsertSymbol_HandleCreateText(dpy, win)
   Display *dpy;
   Window win;
{
   if (gInsertSymbolInfo.initialized && gInsertSymbolInfo.mapped) {
      HideTdgtInsertSymbolDialogBox();
   }
   return TRUE;
}

