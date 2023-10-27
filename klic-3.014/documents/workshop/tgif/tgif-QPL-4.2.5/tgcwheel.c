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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/tgcwheel.c,v 1.5 2011/06/02 15:53:08 william Exp $
 */

#define _INCLUDE_FROM_TGCWHEEL__C_

#include "tgifdefs.h"
#include "tidget.h"

#include "auxtext.e"
#include "dialog.e"
#include "font.e"
#include "msg.e"
#include "setup.e"
#include "strtbl.e"
#include "tgcwheel.e"
#include "tgcwdl.e"
#include "tidget.e"
#include "util.e"

/* --------------------- Init & Clean Up --------------------- */

void TgColorWheel_CleanUp(dpy, win)
   Display *dpy;
   Window win;
{
   if (gColorWheelInfo.initialized) {
      if (gColorWheelInfo.dialogbox_tidgetinfo != NULL) {
         DestroyTdgtColorWheelDialogBox(dpy,
               gColorWheelInfo.dialogbox_tidgetinfo);
      }
   }
   if (gColorWheelInfo.hs_pixmap != None) {
      XFreePixmap(mainDisplay, gColorWheelInfo.hs_pixmap);
   }
   if (gColorWheelInfo.v_pixmap != None) {
      XFreePixmap(mainDisplay, gColorWheelInfo.v_pixmap);
   }
   memset(&gColorWheelInfo, 0, sizeof(ColorWheelInfo));

   CleanUpTdgtColorWheelDlg();
}

int TgColorWheel_Init(dpy, win, arg)
   Display *dpy;
   Window win;
   char *arg;
{
   memset(&gColorWheelInfo, 0, sizeof(ColorWheelInfo));

   gColorWheelInfo.hs_pixmap = XCreatePixmap(mainDisplay, rootWindow, HS_WIDTH,
         HS_HEIGHT, mainDepth);
   if (arg != NULL) {
      UtilStrCpyN(gColorWheelInfo.color_str, sizeof(gColorWheelInfo.color_str),
            arg);
   }
   if (gColorWheelInfo.hs_pixmap == None) {
      FailAllocPixmapMessage(HS_WIDTH, HS_HEIGHT);
      TgColorWheel_CleanUp(dpy, win);
      return FALSE;
   }
   gColorWheelInfo.v_pixmap = XCreatePixmap(mainDisplay, rootWindow, V_WIDTH,
         V_HEIGHT, mainDepth);
   if (gColorWheelInfo.v_pixmap == None) {
      FailAllocPixmapMessage(V_WIDTH, V_HEIGHT);
      TgColorWheel_CleanUp(dpy, win);
      return FALSE;
   }
   if (!InitTdgtColorWheelDlg()) {
      TgColorWheel_CleanUp(dpy, win);

      return FALSE;
   }
   return TRUE;
}

void CleanUpColorWheel()
{
   TgColorWheel_CleanUp(mainDisplay, drawWindow);
}

int InitColorWheel()
{
   return TgColorWheel_Init(mainDisplay, drawWindow, NULL);
}

/* --------------------- TgColorWheel_SendKey --------------------- */

static XComposeStatus c_stat;

int TgColorWheel_SendKey(dpy, win, key_ev, buf)
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

/* --------------------- TgColorWheel_HandleCntrlSpace --------------------- */

int TgColorWheel_HandleCntrlSpace(dpy, win)
   Display *dpy;
   Window win;
{
   if (!gColorWheelInfo.initialized) {
      gColorWheelInfo.initialized = TRUE;
      /* create window */
      gColorWheelInfo.dialogbox_tidgetinfo =
            CreateTdgtColorWheelDialogBox(dpy, rootWindow);
      if (gColorWheelInfo.dialogbox_tidgetinfo == NULL) {
         /* print a message */
      }
   }
   if (gColorWheelInfo.dialogbox_tidgetinfo == NULL) {
      return FALSE;
   }
   if (!gColorWheelInfo.mapped) {
      ShowTdgtColorWheelDialogBox();
      TdgtColorWheelDlgLoop(gColorWheelInfo.dialogbox_tidgetinfo);
   } else {
      HideTdgtColorWheelDialogBox();
   }
   return FALSE;
}

/* --------------------- TgColorWheel_HandleCreateText --------------------- */

int TgColorWheel_HandleCreateText(dpy, win)
   Display *dpy;
   Window win;
{
   if (gColorWheelInfo.initialized && gColorWheelInfo.mapped) {
      HideTdgtColorWheelDialogBox();
   }
   return TRUE;
}

