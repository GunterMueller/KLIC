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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/button.c,v 1.6 2011/05/16 16:21:56 william Exp $
 */

#define _INCLUDE_FROM_BUTTON_C_

#include "tgifdefs.h"

#include "auxtext.e"
#include "box.e"
#include "button.e"
#include "cursor.e"
#include "file.e"
#include "font.e"
#include "mainloop.e"
#include "mainmenu.e"
#include "menu.e"
#include "msg.e"
#include "raster.e"
#include "rect.e"
#include "setup.e"

#define BTN_DEFAULT_FONT         0
#define BTN_MSG_FONT             1
#define BTN_BOLD_MSG_FONT        2
#define BTN_ITALIC_MSG_FONT      3
#define BTN_BOLD_ITALIC_MSG_FONT 4

static
int GetButtonFontStyle(pButtonFontInfo)
   ButtonFontInfo *pButtonFontInfo;
{
   int font_style=INVALID;

   if (pButtonFontInfo == NULL) {
      if (boldMsgFontSet == NULL && boldMsgFontPtr == NULL) {
         font_style = BTN_DEFAULT_FONT;
      } else {
         font_style = BTN_BOLD_MSG_FONT;
      }
   } else {
      switch (pButtonFontInfo->font_style) {
      case STYLE_NR:
         font_style = BTN_MSG_FONT;
         break;
      case STYLE_BR:
         if (boldMsgFontSet == NULL && boldMsgFontPtr == NULL) {
            font_style = BTN_MSG_FONT;
         } else {
            font_style = BTN_BOLD_MSG_FONT;
         }
         break;
      case STYLE_NI:
         if (italicMsgFontSet == NULL && italicMsgFontPtr == NULL) {
            font_style = BTN_MSG_FONT;
         } else {
            font_style = BTN_ITALIC_MSG_FONT;
         }
         break;
      case STYLE_BI:
         if (boldItalicMsgFontSet == NULL && boldItalicMsgFontPtr == NULL) {
            font_style = BTN_MSG_FONT;
         } else {
            font_style = BTN_BOLD_ITALIC_MSG_FONT;
         }
         break;
      default: break;
      }
   }
   return font_style;
}

int ButtonWidth(Str, MinLen, pButtonFontInfo)
   char *Str;
   int MinLen;
   ButtonFontInfo *pButtonFontInfo;
{
   int len=strlen(Str);

   if (pButtonFontInfo != NULL) {
#ifdef _TGIF_DBG /* debug, do not translate */
      fprintf(stderr, "ButtonWidth() is called with pButtonFontInfo != NULL\n");
#endif /* _TGIF_DBG */
   }
   if (boldMsgFontSet == NULL && boldMsgFontPtr == NULL) {
      return (defaultFontWidth * max(len+2, MinLen));
   } else if (len > MinLen-2) {
      char buf[40];
      int len1=0, len2=0;

      /* do not translate -- the string is used to measure things */
      sprintf(buf, "  %s  ", Str);
      len1 = 4+BoldMsgTextWidth(boldMsgFontPtr, buf, len+4);
      len2 = 4+BoldMsgTextWidth(boldMsgFontPtr, "  CANCEL  ", 10);

      return max(len1,len2);
   } else {
      /* do not translate -- the string is used to measure things */
      return 4+BoldMsgTextWidth(boldMsgFontPtr, "  CANCEL  ", 10);
   }
}

#define BTN_Y_MARGIN 2

void DisplayButtonInBBox(Win, Str, Len, BBox, Normal, HighLight, Width,
      pButtonFontInfo)
   Window Win;
   char *Str;
   int Len, HighLight, Width;
   struct BBRec *BBox;
   int Normal;
   ButtonFontInfo *pButtonFontInfo;
   /* Display a button in Win at location ((*BBox).ltx,(*BBox).lty), Str is */
   /*    centered in the button.  The width of the button is given in BBox. */
   /* Len must be strlen(Str) */
   /* If HighLight is TRUE, Width is used to draw an outline around the box. */
{
   int button_w=0, button_h=0, left=0, top=0, text_w=0;
   int bg_pixel=(threeDLook ? myLtGryPixel : myBgPixel);
   int btn_font_style=INVALID;

   button_w = BBox->rbx - BBox->ltx;
   button_h = BBox->rby - BBox->lty;

   btn_font_style = GetButtonFontStyle(pButtonFontInfo);
   switch (btn_font_style) {
   case BTN_DEFAULT_FONT:
      text_w = defaultFontWidth * Len;
      top = ((button_h - defaultFontHeight)>>1);
      break;
   case BTN_MSG_FONT:
      text_w = BoldMsgTextWidth(msgFontPtr, Str, Len);
      top = ((button_h - msgFontHeight)>>1);
      break;
   case BTN_BOLD_MSG_FONT:
      text_w = BoldMsgTextWidth(boldMsgFontPtr, Str, Len);
      top = ((button_h - boldMsgFontHeight)>>1);
      break;
   case BTN_ITALIC_MSG_FONT:
      text_w = BoldMsgTextWidth(italicMsgFontPtr, Str, Len);
      top = ((button_h - italicMsgFontHeight)>>1);
      break;
   case BTN_BOLD_ITALIC_MSG_FONT:
      text_w = BoldMsgTextWidth(boldItalicMsgFontPtr, Str, Len);
      top = ((button_h - boldItalicMsgFontHeight)>>1);
      break;
   }
   left = ((button_w - text_w)>>1);

   XSetForeground(mainDisplay, defaultGC, (Normal ? bg_pixel : myFgPixel));
   XFillRectangle(mainDisplay, Win, defaultGC, BBox->ltx, BBox->lty,
         button_w, button_h);
   XSetForeground(mainDisplay, defaultGC, myFgPixel);
   if (threeDLook) {
      struct BBRec bbox;

      SetBBRec(&bbox, BBox->ltx, BBox->lty-windowPadding, BBox->ltx+button_w,
            BBox->lty+button_h+windowPadding);
      TgDrawThreeDButton(mainDisplay, Win, textMenuGC, &bbox,
            (Normal ? TGBS_RAISED : TGBS_LOWRED), 2, Normal);
   } else {
      XDrawRectangle(mainDisplay, Win, defaultGC, BBox->ltx, BBox->lty,
            button_w, button_h);
   }
   if (!Normal) {
      XSetForeground(mainDisplay, defaultGC, myBgPixel);
   }
   if (pButtonFontInfo == NULL) {
      if (boldMsgFontSet == NULL && boldMsgFontPtr == NULL) {
         DrawBoldMsgString(mainDisplay, Win, defaultGC, BBox->ltx+left,
               BBox->lty+defaultFontAsc+BTN_Y_MARGIN, Str, Len);
      } else {
         if (boldMsgFontPtr != NULL) {
            XSetFont(mainDisplay, defaultGC, boldMsgFontPtr->fid);
         }
         DrawBoldMsgString(mainDisplay, Win, defaultGC, BBox->ltx+left,
               BBox->lty+boldMsgFontAsc+BTN_Y_MARGIN, Str, Len);
         XSetFont(mainDisplay, defaultGC, defaultFontPtr->fid);
      }
   } else {
      switch (btn_font_style) {
      case BTN_DEFAULT_FONT:
         if (defaultFontPtr != NULL) {
            XSetFont(mainDisplay, defaultGC, defaultFontPtr->fid);
         }
         DrawMsgString(mainDisplay, Win, defaultGC, BBox->ltx+left,
               BBox->lty+defaultFontAsc+BTN_Y_MARGIN, Str, Len);
         break;
      case BTN_MSG_FONT:
         if (msgFontPtr != NULL) {
            XSetFont(mainDisplay, defaultGC, msgFontPtr->fid);
         }
         DrawMsgString(mainDisplay, Win, defaultGC, BBox->ltx+left,
               BBox->lty+msgFontAsc+BTN_Y_MARGIN, Str, Len);
         break;
      case BTN_BOLD_MSG_FONT:
         if (boldMsgFontPtr != NULL) {
            XSetFont(mainDisplay, defaultGC, boldMsgFontPtr->fid);
         }
         DrawBoldMsgString(mainDisplay, Win, defaultGC, BBox->ltx+left,
               BBox->lty+boldMsgFontAsc+BTN_Y_MARGIN, Str, Len);
         break;
      case BTN_ITALIC_MSG_FONT:
         if (italicMsgFontPtr != NULL) {
            XSetFont(mainDisplay, defaultGC, italicMsgFontPtr->fid);
         }
         DrawItalicMsgString(mainDisplay, Win, defaultGC, BBox->ltx+left,
               BBox->lty+italicMsgFontAsc+BTN_Y_MARGIN, Str, Len);
         break;
      case BTN_BOLD_ITALIC_MSG_FONT:
         if (boldItalicMsgFontPtr != NULL) {
            XSetFont(mainDisplay, defaultGC, boldItalicMsgFontPtr->fid);
         }
         DrawBoldItalicMsgString(mainDisplay, Win, defaultGC, BBox->ltx+left,
               BBox->lty+boldItalicMsgFontAsc+BTN_Y_MARGIN, Str, Len);
         break;
      }
      XSetFont(mainDisplay, defaultGC, defaultFontPtr->fid);
   }
   if (!Normal) {
      XSetForeground(mainDisplay, defaultGC, myFgPixel);
   }
   if (HighLight) {
      if (threeDLook) {
         XSetForeground(mainDisplay, defaultGC, myBorderPixel);
         XDrawRectangle(mainDisplay, Win, defaultGC, BBox->ltx-1,
               BBox->lty-windowPadding-1, button_w+1,
               button_h+(windowPadding<<1)+1);
         XSetForeground(mainDisplay, defaultGC, myFgPixel);
      } else {
         XDrawRectangle(mainDisplay, Win, defaultGC, BBox->ltx-Width,
               BBox->lty-Width, button_w+(Width<<1), button_h+(Width<<1));
      }
   }
}
