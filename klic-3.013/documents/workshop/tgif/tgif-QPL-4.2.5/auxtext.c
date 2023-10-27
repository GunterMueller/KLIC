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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/auxtext.c,v 1.17 2011/05/16 16:21:56 william Exp $
 */

#define _INCLUDE_FROM_AUXTEXT_C_

#include "tgifdefs.h"

#include "auxtext.e"
#include "choice.e"
#include "color.e"
#include "cursor.e"
#include "cutpaste.e"
#include "dialog.e"
#include "drawing.e"
#include "file.e"
#include "font.e"
#include "miniline.e"
#include "msg.e"
#include "obj.e"
#include "pattern.e"
#include "prtgif.e"
#include "ps.e"
#include "raster.e"
#include "rect.e"
#include "setup.e"
#include "select.e"
#include "strtbl.e"
#include "text.e"
#include "util.e"
#include "xbitmap.e"
#include "xpixmap.e"

GC	rotateGC=NULL;
Pixmap	textBackingBitmap=None, textBackingBgBitmap=None;
Pixmap	textBackingPixmap=None;
int	textBackingBitmapW=INVALID;
int	textBackingBitmapH=INVALID;
int	doubleQuoteDoubleQuote=FALSE;
int	groupedTextEditable=FALSE;

struct SelRec *outerSel=NULL, *innerSel=NULL;

void GetStrSizeInfo(pDynStr, w, lbearing, rextra)
   struct DynStrRec *pDynStr;
   int *w, *lbearing, *rextra;
{
   XCharStruct xcs;

   MyTextExtents(canvasFontPtr, pDynStr->s, pDynStr->sz-1, &xcs);
   if (w != NULL) *w = xcs.width;
   if (lbearing != NULL) *lbearing = (xcs.lbearing >= 0 ? 0 : xcs.lbearing);
   if (rextra != NULL) *rextra = xcs.rbearing-xcs.width;
}

int GetNumberOfMiniLines(minilines)
   MiniLinesInfo *minilines;
{
   int count=0;
   MiniLineInfo *pMiniLine=minilines->first;

   for ( ; pMiniLine != NULL; pMiniLine=pMiniLine->next, count++) {
   }
   return count;
}

int IsHex(ch, pn_val)
   char ch;
   int *pn_val;
{
   if (ch >= '0' && ch <= '9') {
      *pn_val = ((int)(ch - '0'));
      return TRUE;
   }
   if (ch >= 'a' && ch <= 'f') {
      *pn_val = 10+((int)(ch - 'a'));
      return TRUE;
   }
   if (ch >= 'A' && ch <= 'F') {
      *pn_val = 10+((int)(ch - 'F'));
      return TRUE;
   }
   return FALSE;
}

int InputOctalString(pszBuf, pnBufSize)
   char *pszBuf;
   int *pnBufSize;
{
   int i=0, nBufSizeIn=(*pnBufSize), nError=FALSE;
   int nPrevIsOctal=FALSE, nByte1OfOctal=TRUE;
   char *psz=NULL, szSpec[MAXSTRING];

   *pnBufSize = 0;
   *pszBuf = '\0';
   *szSpec = '\0';
   if (canvasFontDoubleByte) {
      sprintf(gszMsgBox, TgLoadString(STID_INPUT_OCTAL_STR), "\\244\\244");
   } else {
      sprintf(gszMsgBox, TgLoadString(STID_INPUT_STR));
   }
   Dialog(gszMsgBox, TgLoadCachedString(CSTID_DLG_ACCEPT_CANCEL), szSpec);
   UtilTrimBlanks(szSpec);
   if (*szSpec == '\0') return FALSE;

   if (szSpec[0] == '0' && szSpec[1] == 'x') {
      for (i=0, psz=(&szSpec[2]); !nError && i < nBufSizeIn-1 && *psz != '\0';
            i++, psz=(&psz[2])) {
         int hi_nibble=0, lo_nibble=0;

         if (IsHex(psz[0], &hi_nibble) && IsHex(psz[1], &lo_nibble)) {
            int hval=(((hi_nibble&0x0ff)<<4)+(lo_nibble&0x0ff));

            pszBuf[i] = hval;

            if (canvasFontDoubleByte && canvasFontDoubleByteModBytes) {
               if ((pszBuf[i] & 0x80) == 0) {
                  pszBuf[i] |= 0x80;
               }
            }
         } else {
            nError = TRUE;
         }
      }
   } else {
      for (i=0, psz=szSpec; !nError && i < nBufSizeIn-1 && *psz != '\0';
            i++, psz++) {
         int nThisIsOctal=FALSE;

         if (*psz == '\\') {
            if (psz[1] == '\0') {
               pszBuf[i] = '\\';
            } else if (psz[1] == '\\') {
               pszBuf[i] = '\\';
               psz++;
            } else if (psz[1] == 'r') {
               pszBuf[i] = '\r';
               psz++;
            } else if (psz[1] == 'n') {
               pszBuf[i] = '\n';
               psz++;
            } else if (psz[1] >= '0' && psz[1] <= '7') {
               char *psz1=NULL, saved_ch='\0';
               int oval=0;

               *psz = '0';
               for (psz1=(&psz[2]); *psz1 >= '0' && *psz1 <= '7'; psz1++) ;
               saved_ch = (*psz1);
               *psz1 = '\0';
               if (sscanf(psz, "%o", &oval) != 1) {
                  nError = TRUE;
               } else {
                  pszBuf[i] = (char)(oval & 0xff);

                  nThisIsOctal = TRUE;
                  if (canvasFontDoubleByte && canvasFontDoubleByteModBytes) {
                     if (nPrevIsOctal) {
                        nByte1OfOctal = !nByte1OfOctal;
                     } else {
                        nByte1OfOctal = TRUE;
                     }
                     if ((pszBuf[i] & 0x80) == 0) {
                        pszBuf[i] |= 0x80;
                     }
                  }
               }
               *psz1 = saved_ch;
               *psz = '\\';
               if (!nError) {
                  psz = psz1;
                  psz--;
               }
            } else {
               i--;
               psz++;
            }
         } else {
            pszBuf[i] = *psz;
         }
         nPrevIsOctal = nThisIsOctal;
      }
   }
   if (canvasFontDoubleByte && ((i & 0x1) != 0)) {
      nError = TRUE;
   }
   if (nError) {
      *pszBuf = '\0';
      sprintf(gszMsgBox, TgLoadCachedString(CSTID_MALFORMED_INPUT_STR), szSpec);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return FALSE;
   }
   pszBuf[i] = '\0';
   *pnBufSize = i;
   return TRUE;
}

int CharIsCntrlINS(key_ev, key_sym)
   XKeyEvent *key_ev;
   KeySym key_sym;
{
   if (key_ev != NULL && ((key_ev->state & ControlMask) != 0) &&
         ((key_ev->state & ShiftMask) == 0) &&
         (key_sym == XK_Insert || key_sym == XK_KP_Insert)) {
      return TRUE;
   }
   return FALSE;
}

int CharIsShiftINS(key_ev, key_sym)
   XKeyEvent *key_ev;
   KeySym key_sym;
{
   if (key_ev != NULL && ((key_ev->state & ControlMask) == 0) &&
         ((key_ev->state & ShiftMask) != 0) &&
         (key_sym == XK_Insert || key_sym == XK_KP_Insert)) {
      return TRUE;
   }
   return FALSE;
}

int CharIsCntrlShiftINS(key_ev, key_sym)
   XKeyEvent *key_ev;
   KeySym key_sym;
{
   if (key_ev != NULL && ((key_ev->state & ControlMask) != 0) &&
         ((key_ev->state & ShiftMask) != 0) &&
         (key_sym == XK_Insert || key_sym == XK_KP_Insert)) {
      return TRUE;
   }
   return FALSE;
}

/*
 * int has_ch=XLookupString(key_ev, buf, sizeof(buf), &key_sym, &c_stat);
 *
 * TranslateKeys(buf, &key_sym);
 *
 * if (CharIsESC(key_ev, buf, key_sym, &has_ch)) {
 * } else if (CharIsCRorLF(key_ev, buf, key_sym, &has_ch)) {
 * } else if (CharIsBSorDEL(key_ev, buf, key_sym, &has_ch, FALSE)) {
 * } else if (CharIsTAB(key_ev, buf, key_sym, &has_ch)) {
 * }
 */
int CharIsESC(key_ev, psz_buf, key_sym, pn_has_ch)
   XKeyEvent *key_ev;
   char *psz_buf;
   KeySym key_sym;
   int *pn_has_ch;
{
   if (key_sym == XK_Escape) return TRUE;
   if (key_ev != NULL && (key_ev->state & ControlMask) &&
         key_sym == XK_bracketleft) {
      return TRUE;
   }
   if (pn_has_ch != NULL) {
      return ((*pn_has_ch) && *psz_buf == '\033');
   }
   return (*psz_buf == '\033');
}

int CharIsCRorLF(key_ev, psz_buf, key_sym, pn_has_ch)
   XKeyEvent *key_ev;
   char *psz_buf;
   KeySym key_sym;
   int *pn_has_ch;
{
   if (key_sym==XK_Return || key_sym==XK_KP_Enter || key_sym==XK_Linefeed) {
      return TRUE;
   }
   if (key_ev != NULL && (key_ev->state & ControlMask) &&
         (key_sym == XK_m || key_sym == XK_j)) {
      return TRUE;
   }
   if (pn_has_ch != NULL) {
      return ((*pn_has_ch) && (*psz_buf == '\r' || *psz_buf == '\n'));
   }
   return (*psz_buf == '\r' || *psz_buf == '\n');
}

int CharIsDEL(key_ev, psz_buf, key_sym, pn_has_ch)
   XKeyEvent *key_ev;
   char *psz_buf;
   KeySym key_sym;
   int *pn_has_ch;
{
   if (key_sym==XK_Delete || key_sym==XK_KP_Delete) {
      return TRUE;
   }
   if (pn_has_ch != NULL) {
      return ((*pn_has_ch) && (*psz_buf == '\177'));
   }
   return (*psz_buf == '\177');
}

int CharIsBS(key_ev, psz_buf, key_sym, pn_has_ch, left_is_BS)
   XKeyEvent *key_ev;
   char *psz_buf;
   KeySym key_sym;
   int *pn_has_ch, left_is_BS;
{
   if ((left_is_BS && (key_sym==XK_Left || key_sym==XK_KP_Left)) ||
         key_sym==XK_BackSpace) {
      return TRUE;
   }
   if (key_ev != NULL && (key_ev->state & ControlMask) && (key_sym == XK_h)) {
      return TRUE;
   }
   if (pn_has_ch != NULL) {
      return ((*pn_has_ch) && (*psz_buf == '\b'));
   }
   return (*psz_buf == '\b');
}

int CharIsBSorDEL(key_ev, psz_buf, key_sym, pn_has_ch, left_is_BS)
   XKeyEvent *key_ev;
   char *psz_buf;
   KeySym key_sym;
   int *pn_has_ch, left_is_BS;
{
   if ((left_is_BS && (key_sym==XK_Left || key_sym==XK_KP_Left)) ||
         key_sym==XK_BackSpace || key_sym==XK_Delete || key_sym==XK_KP_Delete) {
      return TRUE;
   }
   if (key_ev != NULL && (key_ev->state & ControlMask) && (key_sym == XK_h)) {
      return TRUE;
   }
   if (pn_has_ch != NULL) {
      return ((*pn_has_ch) && (*psz_buf == '\b' || *psz_buf == '\177'));
   }
   return (*psz_buf == '\b' || *psz_buf == '\177');
}

int CharIsTAB(key_ev, psz_buf, key_sym, pn_has_ch)
   XKeyEvent *key_ev;
   char *psz_buf;
   KeySym key_sym;
   int *pn_has_ch;
{
   if (key_sym==XK_Tab || key_sym==XK_KP_Tab) {
      return TRUE;
   }
   if (key_ev != NULL && (key_ev->state & ControlMask) && (key_sym == XK_i)) {
      return TRUE;
   }
   if (pn_has_ch != NULL) {
      return ((*pn_has_ch) && (*psz_buf == '\t'));
   }
   return (*psz_buf == '\t');
}

int CharIsHome(key_ev, psz_buf, key_sym, pn_has_ch)
   XKeyEvent *key_ev;
   char *psz_buf;
   KeySym key_sym;
   int *pn_has_ch;
{
   return (key_sym==XK_Home || key_sym==XK_KP_Home);
}

int CharIsEnd(key_ev, psz_buf, key_sym, pn_has_ch)
   XKeyEvent *key_ev;
   char *psz_buf;
   KeySym key_sym;
   int *pn_has_ch;
{
   return (key_sym==XK_End || key_sym==XK_KP_End);
}

int CharIsCntrlSpace(key_ev, psz_buf, key_sym, pn_has_ch)
   XKeyEvent *key_ev;
   char *psz_buf;
   KeySym key_sym;
   int *pn_has_ch;
{
   int state=key_ev->state;

   if (((state & ControlMask) == ControlMask) && ((state & METAMASK) == 0)) {
      if (key_sym == XK_space || (psz_buf[0] == ' ' && psz_buf[1] == '\0')) {
         return TRUE;
      } else if (key_sym == XK_backslash || (psz_buf[0] == '\\' &&
            psz_buf[1] == '\0')) {
         return TRUE;
      }
   }
   return FALSE;
}

void TranslateKeys(s, key_sym)
   char *s;
   KeySym *key_sym;
{
   switch (*key_sym) {
   case XK_KP_Space:     *key_sym=XK_space;    s[0]=' ';  s[1]='\0'; break;
   case XK_KP_Tab:       *key_sym=XK_Tab;      s[0]='\t'; s[1]='\0'; break;
   case XK_KP_Enter:     *key_sym=XK_Return;   s[0]='\r'; s[1]='\0'; break;
   case XK_KP_Equal:     *key_sym=XK_equal;    s[0]='=';  s[1]='\0'; break;
   case XK_KP_Multiply:  *key_sym=XK_multiply; s[0]='*';  s[1]='\0'; break;
   case XK_KP_Add:       *key_sym=XK_plus;     s[0]='+';  s[1]='\0'; break;
   case XK_KP_Separator: *key_sym=XK_comma;    s[0]=',';  s[1]='\0'; break;
   case XK_KP_Subtract:  *key_sym=XK_minus;    s[0]='-';  s[1]='\0'; break;
   case XK_KP_Decimal:   *key_sym=XK_period;   s[0]='.';  s[1]='\0'; break;
   case XK_KP_Divide:    *key_sym=XK_slash;    s[0]='/';  s[1]='\0'; break;

   case XK_KP_0: *key_sym=XK_0; s[0]='0'; s[1]='\0'; break;
   case XK_KP_1: *key_sym=XK_1; s[0]='1'; s[1]='\0'; break;
   case XK_KP_2: *key_sym=XK_2; s[0]='2'; s[1]='\0'; break;
   case XK_KP_3: *key_sym=XK_3; s[0]='3'; s[1]='\0'; break;
   case XK_KP_4: *key_sym=XK_4; s[0]='4'; s[1]='\0'; break;
   case XK_KP_5: *key_sym=XK_5; s[0]='5'; s[1]='\0'; break;
   case XK_KP_6: *key_sym=XK_6; s[0]='6'; s[1]='\0'; break;
   case XK_KP_7: *key_sym=XK_7; s[0]='7'; s[1]='\0'; break;
   case XK_KP_8: *key_sym=XK_8; s[0]='8'; s[1]='\0'; break;
   case XK_KP_9: *key_sym=XK_9; s[0]='9'; s[1]='\0'; break;
   }
}

static
void GetTransformedTextBBoxAbsVs(ObjPtr, LBearing, RightExtra, Vs)
   struct ObjRec *ObjPtr;
   int LBearing, RightExtra;
   IntPoint *Vs; /* array of 5 points */
{
   int x, y;
   struct BBRec bbox;

   bbox.ltx = ObjPtr->orig_obbox.ltx - ObjPtr->x + LBearing;
   bbox.lty = ObjPtr->orig_obbox.lty - ObjPtr->y;
   bbox.rbx = ObjPtr->orig_obbox.rbx - ObjPtr->x + RightExtra;
   bbox.rby = ObjPtr->orig_obbox.rby - ObjPtr->y;
   TransformOffsetBBoxThroughCTM(&bbox, ObjPtr->ctm, Vs);
   x = Vs[0].x + ObjPtr->x; y = Vs[0].y + ObjPtr->y;
   Vs[0].x = Vs[4].x = x; Vs[0].y = Vs[4].y = y;
   x = Vs[1].x + ObjPtr->x; y = Vs[1].y + ObjPtr->y;
   Vs[1].x = x; Vs[1].y = y;
   x = Vs[2].x + ObjPtr->x; y = Vs[2].y + ObjPtr->y;
   Vs[2].x = x; Vs[2].y = y;
   x = Vs[3].x + ObjPtr->x; y = Vs[3].y + ObjPtr->y;
   Vs[3].x = x; Vs[3].y = y;
}

void SetTextOBBox(ObjPtr, Just, W, H, LBearing, RightExtra, Rotate)
   struct ObjRec *ObjPtr;
   int Just, W, H, LBearing, RightExtra, Rotate;
{  /* In this procedure, it is assumed that the x and y field */
   /*       of the text object has the correct information.*/
   /* The rotation is clockwise */
   register int mw2, pw2;

   if (ObjPtr->ctm == NULL) {
      switch (Just) {
      case JUST_L:
         switch (Rotate) {
         case ROTATE0:
            ObjPtr->obbox.ltx = ObjPtr->x; ObjPtr->obbox.rbx = ObjPtr->x+W;
            ObjPtr->obbox.lty = ObjPtr->y; ObjPtr->obbox.rby = ObjPtr->y+H;
            break;
         case ROTATE90:
            ObjPtr->obbox.ltx = ObjPtr->x-H; ObjPtr->obbox.rbx = ObjPtr->x;
            ObjPtr->obbox.lty = ObjPtr->y; ObjPtr->obbox.rby = ObjPtr->y+W;
            break;
         case ROTATE180:
            ObjPtr->obbox.ltx = ObjPtr->x-W; ObjPtr->obbox.rbx = ObjPtr->x;
            ObjPtr->obbox.lty = ObjPtr->y-H; ObjPtr->obbox.rby = ObjPtr->y;
            break;
         case ROTATE270:
            ObjPtr->obbox.ltx = ObjPtr->x; ObjPtr->obbox.rbx = ObjPtr->x+H;
            ObjPtr->obbox.lty = ObjPtr->y-W; ObjPtr->obbox.rby = ObjPtr->y;
            break;
         }
         break;
      case JUST_C:
         mw2 = W/2;
         pw2 = W-W/2;
         switch (Rotate) {
         case ROTATE0:
            ObjPtr->obbox.ltx = ObjPtr->x-mw2;
            ObjPtr->obbox.rbx = ObjPtr->x+pw2;
            ObjPtr->obbox.lty = ObjPtr->y; ObjPtr->obbox.rby = ObjPtr->y+H;
            break;
         case ROTATE90:
            ObjPtr->obbox.ltx = ObjPtr->x-H;
            ObjPtr->obbox.rbx = ObjPtr->x;
            ObjPtr->obbox.lty = ObjPtr->y-mw2;
            ObjPtr->obbox.rby = ObjPtr->y+pw2;
            break;
         case ROTATE180:
            ObjPtr->obbox.ltx = ObjPtr->x-pw2;
            ObjPtr->obbox.rbx = ObjPtr->x+mw2;
            ObjPtr->obbox.lty = ObjPtr->y-H;
            ObjPtr->obbox.rby = ObjPtr->y;
            break;
         case ROTATE270:
            ObjPtr->obbox.ltx = ObjPtr->x;
            ObjPtr->obbox.rbx = ObjPtr->x+H;
            ObjPtr->obbox.lty = ObjPtr->y-pw2;
            ObjPtr->obbox.rby = ObjPtr->y+mw2;
            break;
         }
         break;
      case JUST_R:
         switch (Rotate) {
         case ROTATE0:
            ObjPtr->obbox.ltx = ObjPtr->x-W; ObjPtr->obbox.rbx = ObjPtr->x;
            ObjPtr->obbox.lty = ObjPtr->y; ObjPtr->obbox.rby = ObjPtr->y+H;
            break;
         case ROTATE90:
            ObjPtr->obbox.ltx = ObjPtr->x-H; ObjPtr->obbox.rbx = ObjPtr->x;
            ObjPtr->obbox.lty = ObjPtr->y-W; ObjPtr->obbox.rby = ObjPtr->y;
            break;
         case ROTATE180:
            ObjPtr->obbox.ltx = ObjPtr->x; ObjPtr->obbox.rbx = ObjPtr->x+W;
            ObjPtr->obbox.lty = ObjPtr->y-H; ObjPtr->obbox.rby = ObjPtr->y;
            break;
         case ROTATE270:
            ObjPtr->obbox.ltx = ObjPtr->x; ObjPtr->obbox.rbx = ObjPtr->x+H;
            ObjPtr->obbox.lty = ObjPtr->y; ObjPtr->obbox.rby = ObjPtr->y+W;
            break;
         }
         break;
      }
   } else {
      IntPoint abs_obj_obbox_vs[5];

      GetTransformedOBBoxAbsVs(ObjPtr, abs_obj_obbox_vs);
      ObjPtr->obbox.ltx = min(min(abs_obj_obbox_vs[0].x,abs_obj_obbox_vs[1].x),
            min(abs_obj_obbox_vs[2].x,abs_obj_obbox_vs[3].x));
      ObjPtr->obbox.rbx = max(max(abs_obj_obbox_vs[0].x,abs_obj_obbox_vs[1].x),
            max(abs_obj_obbox_vs[2].x,abs_obj_obbox_vs[3].x));
      ObjPtr->obbox.lty = min(min(abs_obj_obbox_vs[0].y,abs_obj_obbox_vs[1].y),
            min(abs_obj_obbox_vs[2].y,abs_obj_obbox_vs[3].y));
      ObjPtr->obbox.rby = max(max(abs_obj_obbox_vs[0].y,abs_obj_obbox_vs[1].y),
            max(abs_obj_obbox_vs[2].y,abs_obj_obbox_vs[3].y));
   }
}

void SetTextBBox(ObjPtr, Just, W, H, LBearing, RightExtra, Rotate)
   struct ObjRec *ObjPtr;
   int Just, W, H, LBearing, RightExtra, Rotate;
   /* In this procedure, it is assumed that the x and y field */
   /*       of the text object has the correct information.*/
   /* The rotation is clockwise */
{
   SetTextOBBox(ObjPtr, Just, W, H, LBearing, RightExtra, Rotate);

   if (ObjPtr->ctm == NULL) {
      switch (Rotate) {
      case ROTATE0:
         ObjPtr->bbox.ltx = ObjPtr->obbox.ltx + LBearing;
         ObjPtr->bbox.rbx = ObjPtr->obbox.rbx + RightExtra;
         ObjPtr->bbox.lty = ObjPtr->obbox.lty;
         ObjPtr->bbox.rby = ObjPtr->obbox.rby;
         break;
      case ROTATE90:
         ObjPtr->bbox.ltx = ObjPtr->obbox.ltx;
         ObjPtr->bbox.rbx = ObjPtr->obbox.rbx;
         ObjPtr->bbox.lty = ObjPtr->obbox.lty + LBearing;
         ObjPtr->bbox.rby = ObjPtr->obbox.rby + RightExtra;
         break;
      case ROTATE180:
         ObjPtr->bbox.ltx = ObjPtr->obbox.ltx - RightExtra;
         ObjPtr->bbox.rbx = ObjPtr->obbox.rbx - LBearing;
         ObjPtr->bbox.lty = ObjPtr->obbox.lty;
         ObjPtr->bbox.rby = ObjPtr->obbox.rby;
         break;
      case ROTATE270:
         ObjPtr->bbox.ltx = ObjPtr->obbox.ltx;
         ObjPtr->bbox.rbx = ObjPtr->obbox.rbx;
         ObjPtr->bbox.lty = ObjPtr->obbox.lty - RightExtra;
         ObjPtr->bbox.rby = ObjPtr->obbox.rby - LBearing;
         break;
      }
   } else {
      IntPoint abs_obj_bbox_vs[5];

      GetTransformedTextBBoxAbsVs(ObjPtr, LBearing, RightExtra,
            abs_obj_bbox_vs);
      ObjPtr->bbox.ltx = min(min(abs_obj_bbox_vs[0].x,abs_obj_bbox_vs[1].x),
            min(abs_obj_bbox_vs[2].x,abs_obj_bbox_vs[3].x));
      ObjPtr->bbox.rbx = max(max(abs_obj_bbox_vs[0].x,abs_obj_bbox_vs[1].x),
            max(abs_obj_bbox_vs[2].x,abs_obj_bbox_vs[3].x));
      ObjPtr->bbox.lty = min(min(abs_obj_bbox_vs[0].y,abs_obj_bbox_vs[1].y),
            min(abs_obj_bbox_vs[2].y,abs_obj_bbox_vs[3].y));
      ObjPtr->bbox.rby = max(max(abs_obj_bbox_vs[0].y,abs_obj_bbox_vs[1].y),
            max(abs_obj_bbox_vs[2].y,abs_obj_bbox_vs[3].y));
   }
   ObjPtr->bbox.ltx--;
   ObjPtr->bbox.rbx++;
   ObjPtr->bbox.lty--;
   ObjPtr->bbox.rby++;
}

void SetTextOrigBBoxes(ObjPtr, Just, W, H, LBearing, RightExtra, Rotate)
   struct ObjRec *ObjPtr;
   int Just, W, H, LBearing, RightExtra, Rotate;
   /* In this procedure, it is assumed that the x and y field */
   /*       of the text object has the correct information.*/
   /* The rotation is clockwise */
{
   struct BBRec obbox, bbox;
   struct XfrmMtrxRec *ctm;

   if (ObjPtr->ctm == NULL) return;
   memcpy(&obbox, &ObjPtr->obbox, sizeof(struct BBRec));
   memcpy(&bbox, &ObjPtr->bbox, sizeof(struct BBRec));
   ctm = ObjPtr->ctm;
   ObjPtr->ctm = NULL;

   SetTextBBox(ObjPtr, Just, W, H, LBearing, RightExtra, Rotate);

   memcpy(&ObjPtr->orig_obbox, &ObjPtr->obbox, sizeof(struct BBRec));
   memcpy(&ObjPtr->detail.t->orig_bbox, &ObjPtr->bbox, sizeof(struct BBRec));
   ObjPtr->ctm = ctm;
}

int UpdTextBBox(ObjPtr)
   struct ObjRec *ObjPtr;
   /*
    * This function should be called every time any part of a text object is
    *       changed.
    * Therefore, it should never be called if PRTGIF.
    * Returns FALSE if the text object is changed.
    */
{
   struct TextRec *text_ptr=ObjPtr->detail.t;
   int read_only=FALSE;
#ifdef _TGIF_DBG /* debug, do not translate */
   int saved_baseline_y=text_ptr->baseline_y;

#endif /* _TGIF_DBG */

   if (mainDisplay == NULL) return FALSE;

#ifdef _TGIF_DBG /* debug, do not translate */
   if (PRTGIF && !cmdLineOpenDisplay) {
      fprintf(stderr, "UpdTextBBox() called in PRTGIF!\n");
   }
#endif /* _TGIF_DBG */
   if (text_ptr->read_only) {
      return TRUE;
   }
   PushCurFont();

   ObjFontInfoToCurFontInfo(text_ptr);
   allowFontFaceSubstitution = FALSE;
   read_only = (!RecalcTextMetrics(text_ptr, ObjPtr->x, text_ptr->baseline_y));
   allowFontFaceSubstitution = TRUE;

#ifdef _TGIF_DBG /* debug, do not translate */
   if (text_ptr->baseline_y != saved_baseline_y) {
      TgAssert(FALSE,
            "Text_ptr->baseline_y changed in UpdTextBBox()", "Restored");
      text_ptr->baseline_y = saved_baseline_y;
   }
#endif /* _TGIF_DBG */

   if (read_only) {
      PopCurFont();
      return FALSE;
   }
   ObjPtr->y = text_ptr->baseline_y - text_ptr->minilines.first->asc;

   SetTextOrigBBoxes(ObjPtr, textJust, text_ptr->w, text_ptr->h,
         text_ptr->min_lbearing, text_ptr->max_rextra, ROTATE0);
   SetTextBBox(ObjPtr, textJust, text_ptr->w, text_ptr->h,
         text_ptr->min_lbearing, text_ptr->max_rextra, ROTATE0);

   PopCurFont();
   return (!read_only);
}

int PixelOnOff(image, col_start, row_start, scale)
   XImage *image;
   int col_start, row_start, scale;
{
   register int m, n;
   int on_count=0, off_count=0;

   if (scale == 2) {
      for (m = 0; m < 2; m++) {
         for (n = 0; n < 2; n++) {
            switch (XGetPixel(image,col_start+n,row_start+m)) {
            case 0: if (++off_count > 2) return 0; break;
            case 1: if (++on_count >= 2) return 1; break;
            }
         }
      }
   } else if (scale > 2) {
      int half_scale=scale>>1;

      for (m = 0; m < 2; m++) {
         for (n = 0; n < 2; n++) {
            switch (PixelOnOff(image, col_start+n*half_scale,
                  row_start+m*half_scale, half_scale)) {
            case 0: if (++off_count > 2) return 0; break;
            case 1: if (++on_count >= 2) return 1; break;
            }
         }
      }
   } else {
      return 1;
   }
   return 0;
}

static
int PaintBackingPixmap(text_ptr, bbox_w, bbox_h, min_lbearing, max_rextra,
      baseline_y)
   struct TextRec *text_ptr;
   int bbox_w, bbox_h, min_lbearing, max_rextra, baseline_y;
   /*
    * bbox_w includes left bearing and right extra.
    */
{
   MiniLinesInfo *minilines=(&text_ptr->minilines);
   int x=0, w=minilines->w, saved_pen=penPat, saved_trans_pat=transPat;

   if (bbox_w > textBackingBitmapW || bbox_h > textBackingBitmapH) {
      int tmp_w, tmp_h;

      if (textBackingBitmap != None) {
         XFreePixmap(mainDisplay, textBackingBitmap);
      }
      if (textBackingBgBitmap != None) {
         XFreePixmap(mainDisplay, textBackingBgBitmap);
      }
      if (textBackingPixmap != None) {
         XFreePixmap(mainDisplay, textBackingPixmap);
      }
      tmp_w = max(bbox_w,textBackingBitmapW);
      tmp_h = max(bbox_h,textBackingBitmapH);
      textBackingBitmap = XCreatePixmap(mainDisplay, mainWindow,
            tmp_w, tmp_h, 1);
      textBackingBgBitmap = XCreatePixmap(mainDisplay, mainWindow,
            tmp_w, tmp_h, 1);
      textBackingPixmap = XCreatePixmap(mainDisplay, mainWindow,
            tmp_w, tmp_h, mainDepth);
      if (textBackingBitmap == None || textBackingBgBitmap == None ||
            textBackingPixmap == None) {
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
         return FailAllocPixmapMessage(tmp_w, tmp_h);
      }
      textBackingBitmapW = tmp_w;
      textBackingBitmapH = tmp_h;
   }
   switch (textJust) {
   case JUST_L: x = (-min_lbearing); break;
   case JUST_C:
      x = ((bbox_w+min_lbearing-max_rextra-w)>>1)-min_lbearing+(w>>1);
      break;
   case JUST_R: x = bbox_w-max_rextra; break;
   }
   XFillRectangle(mainDisplay, textBackingPixmap, xpmGC, 0, 0, bbox_w, bbox_h);
   XSetForeground(mainDisplay, rotateGC, 0);
   XFillRectangle(mainDisplay, textBackingBitmap, rotateGC, 0, 0, bbox_w,
         bbox_h);
   XFillRectangle(mainDisplay, textBackingBgBitmap, rotateGC, 0, 0, bbox_w,
         bbox_h);
   XSetForeground(mainDisplay, rotateGC, 1);

   ResetDirtyBBoxInfo();
   transPat = FALSE;
   penPat = SOLIDPAT;
   PaintMiniLines(mainDisplay, textBackingBgBitmap, rotateGC, 1,
         x, baseline_y, minilines, FALSE, TRUE);

   penPat = text_ptr->pen;
   PaintMiniLines(mainDisplay, textBackingBitmap, rotateGC, 1,
         x, baseline_y, minilines, FALSE, TRUE);
   PaintMiniLines(mainDisplay, textBackingPixmap, xpmGC, mainDepth,
         x, baseline_y, minilines, FALSE, FALSE);

   penPat = saved_pen;
   transPat = saved_trans_pat;

   return TRUE;
}

#ifdef NOT_DEFINED
#ifdef _TGIF_DBG /* debug, do not translate */
static
void DebugCachedTextBitmapImage(image, before, w, h, min_lbearing, max_rextra)
   XImage *image;
   int before, w, h, min_lbearing, max_rextra;
{
   int i=0;

   if (before) fprintf(stderr, "\n");
   fprintf(stderr, "           w = %1d\n", w);
   fprintf(stderr, "           h = %1d\n", h);
   if (before) {
      fprintf(stderr, "min_lbearing = %1d\n", min_lbearing);
      fprintf(stderr, "  max_rextra = %1d\n", max_rextra);
   }
   for (i=0; i < h; i++) {
      int j=0;

      for (j=0; j < w; j++) {
         int data=XGetPixel(image,j,i);

         fprintf(stderr, "%c%s", (data==1 ? 'X' : ' '), (j+1 == w ? "\n" : ""));
      }
   }
}
#endif /* _TGIF_DBG */
#endif /* NOT_DEFINED */

void MakeCachedTextBitmap(ObjPtr)
   struct ObjRec *ObjPtr;
{
   int r=0, c=0, bbox_w=0, bbox_h=0, num_cols=0, num_rows=0;
   int watch_cursor=FALSE, do_msg=FALSE, min_lbearing=0, max_rextra=0;
   int flat=FALSE, skinny=FALSE, start_row=0, start_col=0, ok=FALSE;
   struct MtrxRec mtrx;
   Pixmap dest_bitmap=None, dest_bg_bitmap=None, dest_pixmap=None;
   XImage *src_image=NULL, *src_bitmap_image=NULL, *src_bitmap_bg_image=NULL;
   XImage *dest_image=NULL, *dest_bitmap_image=NULL, *dest_bitmap_bg_image=NULL;
   struct TextRec *text_ptr=ObjPtr->detail.t;

   if (ObjPtr->ctm == NULL && text_ptr->cached_bitmap != None &&
         text_ptr->cached_zoomed == zoomedIn &&
         text_ptr->cached_zoom == zoomScale) {
      return;
   }
   if (ObjPtr->obbox.ltx==ObjPtr->obbox.rbx ||
         ObjPtr->obbox.lty==ObjPtr->obbox.rby) {
      return;
   }
   InvalidateTextCache(text_ptr);

   if (PRTGIF) {
      RecalcTextMetrics(text_ptr, ObjPtr->x, text_ptr->baseline_y);
   }
   if (curChoice == DRAWTEXT && textCursorShown && ObjPtr == curTextObj) {
      if (RestoreEditTextSize(ObjPtr, TRUE)) {
         UpdTextBBox(ObjPtr);
      }
   }
   if (text_ptr->read_only) {
      min_lbearing = text_ptr->min_lbearing;
      max_rextra = text_ptr->max_rextra;
      bbox_w = text_ptr->orig_w;
      bbox_h = text_ptr->orig_h;
   } else {
      min_lbearing = text_ptr->min_lbearing;
      max_rextra = text_ptr->max_rextra;
      bbox_w = text_ptr->w - min_lbearing + max_rextra;
      bbox_h = text_ptr->h;
   }
   ok = PaintBackingPixmap(text_ptr, bbox_w, bbox_h, min_lbearing,
         max_rextra, text_ptr->minilines.first->asc);
   if (curChoice == DRAWTEXT && textCursorShown && ObjPtr == curTextObj) {
      if (RestoreEditTextSize(ObjPtr, FALSE)) {
         UpdTextBBox(ObjPtr);
      }
   }
   if (!ok) {
      return;
   }
   watch_cursor = watchCursorOnMainWindow;
   if (!watch_cursor && !RedrawDuringScrolling()) {
      SetWatchCursor(drawWindow);
      SetWatchCursor(mainWindow);
   } else {
      CheckInterrupt(TRUE);
   }
   src_image = XGetImage(mainDisplay, textBackingPixmap, 0, 0, bbox_w, bbox_h,
         AllPlanes, ZPixmap);
   src_bitmap_image = XGetImage(mainDisplay, textBackingBitmap, 0, 0,
         bbox_w, bbox_h, 1, ZPixmap);
   src_bitmap_bg_image = XGetImage(mainDisplay, textBackingBgBitmap, 0, 0,
         bbox_w, bbox_h, 1, ZPixmap);

   if (ObjPtr->ctm == NULL) {
      if (text_ptr->read_only) {
         num_cols = ObjPtr->bbox.rbx - ObjPtr->bbox.ltx - 2;
         num_rows = ObjPtr->bbox.rby - ObjPtr->bbox.lty - 2;
      } else {
         num_cols = bbox_w; num_rows = bbox_h;
      }
      if (zoomedIn || zoomScale == 0) {
         num_cols <<= zoomScale;
         num_rows <<= zoomScale;
      } else {
         num_cols >>= zoomScale;
         num_rows >>= zoomScale;
      }
   } else {
      num_cols = ZOOMED_SIZE(ObjPtr->bbox.rbx - ObjPtr->bbox.ltx - 2);
      num_rows = ZOOMED_SIZE(ObjPtr->bbox.rby - ObjPtr->bbox.lty - 2);
   }
   if (num_cols == 0) {
      skinny = TRUE;
      num_cols = 1;
   }
   if (num_rows == 0) {
      flat = TRUE;
      num_rows = 1;
   }
   do_msg = (((num_rows*num_cols)>=0x4000) && !RedrawDuringScrolling());
   if (do_msg) {
      SaveStatusStrings();
      SetStringStatus(TgLoadCachedString(CSTID_CACHING_TEXT_BMP));
      XSync(mainDisplay, False);
   }
   dest_bitmap = XCreatePixmap(mainDisplay,mainWindow,num_cols,num_rows,1);
   dest_bg_bitmap = XCreatePixmap(mainDisplay,mainWindow,num_cols,num_rows,1);
   XSetForeground(mainDisplay, rotateGC, 0);
   XFillRectangle(mainDisplay,dest_bitmap,rotateGC,0,0,num_cols,num_rows);
   XFillRectangle(mainDisplay,dest_bg_bitmap,rotateGC,0,0,num_cols,num_rows);
   dest_pixmap = XCreatePixmap(mainDisplay, mainWindow, num_cols, num_rows,
         mainDepth);
   XFillRectangle(mainDisplay, dest_pixmap, xpmGC, 0, 0, num_cols, num_rows);

   dest_image = XGetImage(mainDisplay, dest_pixmap, 0, 0, num_cols,
         num_rows, AllPlanes, ZPixmap);
   dest_bitmap_image = XGetImage(mainDisplay, dest_bitmap, 0, 0, num_cols,
         num_rows, 1, ZPixmap);
   dest_bitmap_bg_image = XGetImage(mainDisplay, dest_bg_bitmap, 0, 0, num_cols,
         num_rows, 1, ZPixmap);

   if (!flat && !skinny) {
#ifdef NOT_DEFINED
#ifdef _TGIF_DBG
      DebugCachedTextBitmapImage(src_bitmap_image, TRUE, bbox_w, bbox_h,
            min_lbearing, max_rextra);
#endif /* _TGIF_DBG */
#endif /* NOT_DEFINED */
      if (ObjPtr->ctm == NULL) {
         mtrx.image_w = (float)bbox_w;
         mtrx.image_h = (float)bbox_h;
         mtrx.w = (float)num_cols;
         mtrx.h = (float)num_rows;
         mtrx.rotate = ROTATE0;
         mtrx.flip = NO_FLIP;

         CalcTransform(&mtrx);

         start_col = (mtrx.transformed_w >= 0.0) ? 0 : (-num_cols)+1;
         start_row = (mtrx.transformed_h >= 0.0) ? 0 : (-num_rows)+1;

         for (r=0; r < num_rows; r++) {
            double part_x, part_y;

            if (do_msg && ((r & 0xf) == 0)) {
               int percent=(r*10000/num_rows)/100;

               sprintf(gszMsgBox, TgLoadCachedString(CSTID_PROGRESS_PERCENT),
                     percent);
               SetStringStatus(gszMsgBox);
               XSync(mainDisplay, False);
            }
            part_x = ((double)(r+start_row)+0.5)*(mtrx.rev_m[1][0]);
            part_y = ((double)(r+start_row)+0.5)*(mtrx.rev_m[1][1]);
            for (c=0; c < num_cols; c++) {
               double x, y;

               x = part_x+((double)(c+start_col)+0.5)*(mtrx.rev_m[0][0]);
               y = part_y+((double)(c+start_col)+0.5)*(mtrx.rev_m[0][1]);
               if (x >= ((double)0) && x < ((double)bbox_w) &&
                     y >= ((double)0) && y < ((double)bbox_h)) {
                  int new_x=(int)x, new_y=(int)y;

                  if (new_x < 0) new_x = 0;
                  if (new_x >= bbox_w) new_x = bbox_w-1;
                  if (new_y < 0) new_y = 0;
                  if (new_y >= bbox_h) new_y = bbox_h-1;
                  if (XGetPixel(src_bitmap_image, new_x, new_y) == 1) {
                     int dest_pixel=XGetPixel(src_image, new_x, new_y);

                     XPutPixel(dest_bitmap_image, c, r, 1);
                     XPutPixel(dest_image, c, r, dest_pixel);
                  }
                  if (XGetPixel(src_bitmap_bg_image, new_x, new_y) == 1) {
                     XPutPixel(dest_bitmap_bg_image, c, r, 1);
                  }
               }
            }
         }
      } else {
         int abs_offset_x=ObjPtr->bbox.ltx+1-ObjPtr->x;
         int abs_offset_y=ObjPtr->bbox.lty+1-ObjPtr->y;
         int abs_adj_x=ObjPtr->x-ObjPtr->orig_obbox.ltx;
         int abs_adj_y=ObjPtr->y-ObjPtr->orig_obbox.lty;

         for (r=0; r < num_rows; r++) {
            int y=abs_offset_y+ABS_SIZE(r);
            double dy=((double)y)+0.5;

            if (do_msg && ((r & 0xf) == 0)) {
               int percent=(r*10000/num_rows)/100;

               sprintf(gszMsgBox, TgLoadCachedString(CSTID_PROGRESS_PERCENT),
                     percent);
               SetStringStatus(gszMsgBox);
               XSync(mainDisplay, False);
            }
            for (c=0; c < num_cols; c++) {
               int x=abs_offset_x+ABS_SIZE(c);
               double dx=((double)x)+0.5;
               double new_dx=((double)0), new_dy=((double)0);
               
               ReverseTransformDoublePointThroughCTM(dx, dy, ObjPtr->ctm,
                     &new_dx, &new_dy);
               new_dx += (double)abs_adj_x;
               new_dy += (double)abs_adj_y;
               if (new_dx >= ((double)0) && new_dx < ((double)bbox_w) &&
                     new_dy >= ((double)0) && new_dy < ((double)bbox_h)) {
                  int new_x=(int)new_dx, new_y=(int)new_dy;

                  if (new_x < 0) new_x = 0;
                  if (new_x >= bbox_w) new_x = bbox_w-1;
                  if (new_y < 0) new_y = 0;
                  if (new_y >= bbox_h) new_y = bbox_h-1;
                  if (XGetPixel(src_bitmap_image, new_x, new_y) == 1) {
                     int dest_pixel=XGetPixel(src_image, new_x, new_y);

                     XPutPixel(dest_bitmap_image, c, r, 1);
                     XPutPixel(dest_image, c, r, dest_pixel);
                  }
                  if (XGetPixel(src_bitmap_bg_image, new_x, new_y) == 1) {
                     XPutPixel(dest_bitmap_bg_image, c, r, 1);
                  }
               }
            }
         }
         memcpy(&text_ptr->cached_ctm,ObjPtr->ctm,sizeof(struct XfrmMtrxRec));
      }
#ifdef NOT_DEFINED
#ifdef _TGIF_DBG
      DebugCachedTextBitmapImage(dest_bitmap_image, FALSE, num_cols, num_rows,
            0, 0);
#endif /* _TGIF_DBG */
#endif /* NOT_DEFINED */
   }
   if (do_msg) {
      SetStringStatus(TgLoadCachedString(CSTID_FINISHED_CACHEING_TEXT_BMP));
      XSync(mainDisplay, False);
   }
   XPutImage(mainDisplay, dest_bitmap, rotateGC, dest_bitmap_image, 0, 0, 0, 0,
         num_cols, num_rows);
   XPutImage(mainDisplay, dest_bg_bitmap, rotateGC, dest_bitmap_bg_image, 0, 0,
         0, 0, num_cols, num_rows);
   XPutImage(mainDisplay, dest_pixmap, xpmGC, dest_image, 0, 0, 0, 0,
         num_cols, num_rows);
   if (do_msg) RestoreStatusStrings();

   text_ptr->cached_bitmap = dest_bitmap;
   text_ptr->cached_bg_bitmap = dest_bg_bitmap;
   text_ptr->cached_pixmap = dest_pixmap;
   text_ptr->cached_zoomed = zoomedIn;
   text_ptr->cached_zoom = zoomScale;
   XDestroyImage(src_image);
   XDestroyImage(src_bitmap_image);
   XDestroyImage(src_bitmap_bg_image);
   XDestroyImage(dest_image);
   XDestroyImage(dest_bitmap_image);
   XDestroyImage(dest_bitmap_bg_image);

   if (!watch_cursor && !RedrawDuringScrolling()) {
      SetDefaultCursor(mainWindow);
      ShowCursor();
   } else {
      CheckInterrupt(TRUE);
   }
}

void CleanOuterInnerSel()
{
   register struct SelRec *sel_ptr, *next_sel;

   if (outerSel != NULL) {
      for (sel_ptr=outerSel; sel_ptr != NULL; sel_ptr=next_sel) {
         next_sel = sel_ptr->next;
         free(sel_ptr);
      }
      outerSel = innerSel = NULL;
   }
}

void UpdateOuterInnerSel(obj_ptr)
   struct ObjRec *obj_ptr;
   /* outerSel is at the top of the chain and innerSel is at the bottom */
{
   AddObjIntoSel(obj_ptr, NULL, outerSel, &outerSel, &innerSel);
}

/*
 * Need to check for pins.
 */

static
struct ObjRec *SubFindTextObj(ObjPtr, XOff, YOff, obj_to_be_found)
   struct ObjRec *ObjPtr, *obj_to_be_found;
   int XOff, YOff;
{
   register struct ObjRec *obj_ptr;
   register struct AttrRec *attr_ptr;
   struct ObjRec *found_text_obj;

   for (obj_ptr=ObjPtr->detail.r->first; obj_ptr!=NULL; obj_ptr=obj_ptr->next) {
      if (ObjPtr->type == OBJ_PIN && obj_ptr != GetPinObj(ObjPtr)) {
         continue;
      }
      obj_ptr->tmp_parent = ObjPtr;
      if (obj_to_be_found == NULL) {
         if (obj_ptr->type == OBJ_TEXT &&
               XOff >= OFFSET_X(obj_ptr->bbox.ltx)-3 &&
               YOff >= OFFSET_Y(obj_ptr->bbox.lty)-3 &&
               XOff <= OFFSET_X(obj_ptr->bbox.rbx)+3 &&
               YOff <= OFFSET_Y(obj_ptr->bbox.rby)+3) {
            return obj_ptr;
         } else {
            attr_ptr = obj_ptr->fattr;
            for (; attr_ptr != NULL;  attr_ptr=attr_ptr->next) {
               if (XOff>=OFFSET_X(attr_ptr->obj->bbox.ltx)-3 &&
                     YOff>=OFFSET_Y(attr_ptr->obj->bbox.lty)-3 &&
                     XOff<=OFFSET_X(attr_ptr->obj->bbox.rbx)+3 &&
                     YOff<=OFFSET_Y(attr_ptr->obj->bbox.rby)+3 &&
                     attr_ptr->shown == TRUE) {
                  return attr_ptr->obj;
               }
            }
            switch (obj_ptr->type) {
            case OBJ_GROUP:
            case OBJ_ICON:
            case OBJ_SYM:
            case OBJ_PIN: break;
            default: continue;
            }
            if (XOff >= OFFSET_X(obj_ptr->bbox.ltx)-3 &&
                  YOff >= OFFSET_Y(obj_ptr->bbox.lty)-3 &&
                  XOff <= OFFSET_X(obj_ptr->bbox.rbx)+3 &&
                  YOff <= OFFSET_Y(obj_ptr->bbox.rby)+3) {
               if ((found_text_obj=SubFindTextObj(obj_ptr, XOff, YOff,
                     obj_to_be_found)) != NULL) {
                  UpdateOuterInnerSel(obj_ptr);
                  return found_text_obj;
               }
            }
         }
      } else {
         if (obj_ptr == obj_to_be_found) {
            return obj_ptr;
         } else {
            attr_ptr = obj_ptr->fattr;
            for (; attr_ptr != NULL;  attr_ptr=attr_ptr->next) {
               if (attr_ptr->obj == obj_to_be_found) {
                  return attr_ptr->obj;
               }
            }
            switch (obj_ptr->type) {
            case OBJ_GROUP:
            case OBJ_ICON:
            case OBJ_SYM:
            case OBJ_PIN: break;
            default: continue;
            }
            if ((found_text_obj=SubFindTextObj(obj_ptr, XOff, YOff,
                  obj_to_be_found)) != NULL) {
               UpdateOuterInnerSel(obj_ptr);
               return found_text_obj;
            }
         }
      }
   }
   return NULL;
}

struct ObjRec *FindTextObj(XOff, YOff, obj_to_be_found)
   int XOff, YOff;
   struct ObjRec *obj_to_be_found;
   /* XOff and YOff are screen offsets */
{
   register struct ObjRec *obj_ptr;
   register struct AttrRec *attr_ptr;
   struct ObjRec *found_text_obj;

   CleanOuterInnerSel();

   for (obj_ptr=topObj; obj_ptr != NULL; obj_ptr=obj_ptr->next) {
      obj_ptr->tmp_parent = NULL;
      if (colorLayers && !ObjInVisibleLayer(obj_ptr)) {
         continue;
      }
      if (obj_to_be_found == NULL) {
         if (obj_ptr->type == OBJ_TEXT &&
               XOff >= OFFSET_X(obj_ptr->bbox.ltx)-3 &&
               YOff >= OFFSET_Y(obj_ptr->bbox.lty)-3 &&
               XOff <= OFFSET_X(obj_ptr->bbox.rbx)+3 &&
               YOff <= OFFSET_Y(obj_ptr->bbox.rby)+3) {
            return obj_ptr;
         } else {
            attr_ptr = obj_ptr->fattr;
            for (; attr_ptr != NULL;  attr_ptr=attr_ptr->next) {
               if (XOff>=OFFSET_X(attr_ptr->obj->bbox.ltx)-3 &&
                     YOff>=OFFSET_Y(attr_ptr->obj->bbox.lty)-3 &&
                     XOff<=OFFSET_X(attr_ptr->obj->bbox.rbx)+3 &&
                     YOff<=OFFSET_Y(attr_ptr->obj->bbox.rby)+3 &&
                     attr_ptr->shown == TRUE) {
                  return attr_ptr->obj;
               }
            }
            if (!groupedTextEditable) continue;

            switch (obj_ptr->type) {
            case OBJ_GROUP:
            case OBJ_ICON:
            case OBJ_SYM:
            case OBJ_PIN: break;
            default: continue;
            }
            if (XOff >= OFFSET_X(obj_ptr->bbox.ltx)-3 &&
                  YOff >= OFFSET_Y(obj_ptr->bbox.lty)-3 &&
                  XOff <= OFFSET_X(obj_ptr->bbox.rbx)+3 &&
                  YOff <= OFFSET_Y(obj_ptr->bbox.rby)+3) {
               if (colorLayers && !ObjInVisibleLayer(obj_ptr)) {
               } else if ((found_text_obj=SubFindTextObj(obj_ptr, XOff, YOff,
                     obj_to_be_found)) != NULL) {
                  UpdateOuterInnerSel(obj_ptr);
                  return found_text_obj;
               }
            }
         }
      } else {
         if (obj_ptr == obj_to_be_found) {
            return obj_ptr;
         } else {
            attr_ptr = obj_ptr->fattr;
            for (; attr_ptr != NULL;  attr_ptr=attr_ptr->next) {
               if (attr_ptr->obj == obj_to_be_found) {
                  return attr_ptr->obj;
               }
            }
            if (!groupedTextEditable) continue;

            switch (obj_ptr->type) {
            case OBJ_GROUP:
            case OBJ_ICON:
            case OBJ_SYM:
            case OBJ_PIN: break;
            default: continue;
            }
            if (colorLayers && !ObjInVisibleLayer(obj_ptr)) {
            } else if ((found_text_obj=SubFindTextObj(obj_ptr, XOff, YOff,
                  obj_to_be_found)) != NULL) {
               UpdateOuterInnerSel(obj_ptr);
               return found_text_obj;
            }
         }
      }
   }
   return NULL;
}

void UnlinkCurTextFromInnerSel()
{
   register struct GroupRec *group_obj=innerSel->obj->detail.r;

   if (group_obj->first == group_obj->last) {
      if (outerSel != innerSel || innerSel->obj->fattr != NULL) {
#ifdef _TGIF_DBG /* debug, do not translate */
         sprintf(gszMsgBox, "%s %s.  %s!  %s %s.",
               "Error!   The text object within a composite object which you",
               "have just erased caused all ancestors to be deleted",
               "You may see ghost images now",
               "Please undo and try ungroup the ancestor object first",
               "before erasing the text");
         if (PRTGIF) {
            fprintf(stderr, "%s\n", gszMsgBox);
         } else {
            MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
         }
#endif /* _TGIF_DBG */
      }
      DelObj(outerSel->obj);
      CleanOuterInnerSel();
      curTextObj = NULL;
      return;
   } else if (curTextObj == group_obj->first) {
      curTextObj->next->prev = NULL;
      group_obj->first = curTextObj->next;
   } else if (curTextObj == group_obj->last) {
      curTextObj->prev->next = NULL;
      group_obj->last = curTextObj->prev;
   } else {
      curTextObj->prev->next = curTextObj->next;
      curTextObj->next->prev = curTextObj->prev;
   }
   curTextObj->prev = curTextObj->next = NULL;
}

void AdjAncestorsBBox()
{
   register struct SelRec *sel_ptr;

   for (sel_ptr=innerSel; sel_ptr != NULL; sel_ptr=sel_ptr->prev) {
      AdjObjBBox(sel_ptr->obj);
   }
}

static
void DumpTextPath(FP, ObjPtr)
   FILE *FP;
   struct ObjRec *ObjPtr;
{
   struct TextRec *text_ptr=ObjPtr->detail.t;
   MiniLinesInfo *minilines=(&text_ptr->minilines);
   int x=ObjPtr->x, y=ObjPtr->y, trans_pat=ObjPtr->trans_pat;

   fprintf(FP, "   %s\n", gPsCmd[PS_GSAVE]);
   fprintf(FP, "      1 %s\n", gPsCmd[PS_SETLINEWIDTH]);
   if (ObjPtr->ctm != NULL) {
      float m[6];

      m[CTM_SX] = ((float)ObjPtr->ctm->m[CTM_SX])/((float)1000.0);
      m[CTM_SY] = ((float)ObjPtr->ctm->m[CTM_SY])/((float)1000.0);
      m[CTM_SIN] = ((float)ObjPtr->ctm->m[CTM_SIN])/((float)1000.0);
      m[CTM_MSIN] = ((float)ObjPtr->ctm->m[CTM_MSIN])/((float)1000.0);
      fprintf(FP, "      %1d %1d %s\n", ObjPtr->x, ObjPtr->y,
            gPsCmd[PS_TRANSLATE]);
      fprintf(FP, "      [%.3f %.3f %.3f %.3f %1d %1d] %s\n",
            m[CTM_SX], m[CTM_SIN], m[CTM_MSIN], m[CTM_SY],
            ObjPtr->ctm->t[CTM_TX], ObjPtr->ctm->t[CTM_TY], gPsCmd[PS_CONCAT]);
      x = y = 0;
   }
   switch (penPat) {
   case SOLIDPAT: break;
   case BACKPAT:
      if (!trans_pat) {
         fprintf(FP, "      1 %s\n", gPsCmd[PS_SETGRAY]);
      }
      break;
   default:
      if (!colorDump && useGray) {
         GrayCheck(penPat);
         fprintf(FP, "      %s %s\n", GrayStr(penPat), gPsCmd[PS_SETGRAY]);
      }
      break;
   }
   fprintf(FP, "      %1d %1d %s\n", x, y+minilines->first->asc,
         gPsCmd[PS_MOVETO]);
   if (preDumpSetup) PSUseMiniLines();
   DumpMiniLines(minilines, x, y+minilines->first->asc, FP, &ObjPtr->obbox,
         TRUE, 6);

#ifdef _TGIF_DBG
   CleanUpIndentStrings();
#endif /* _TGIF_DBG */

   fprintf(FP, "   %s\n", gPsCmd[PS_GRESTORE]);
}

static
void DumpSimpleTextBBoxPath(FP, just, x, lty, rby, indent)
   FILE *FP;
   int just, x, lty, rby, indent;
{
   int i=0;

   if (psUseShortHand) {
      for (i=0; i < indent; i++) fprintf(FP, " ");
      fprintf(FP, "%s %1d %1d %s ",
            gPsCmd[PS_NEWPATH],
            x, lty, gPsCmd[PS_MOVETO]);
      switch (just) {
      case JUST_L: break;
      case JUST_C:
         fprintf(FP, "tgifstrw 2 %s %s 0 %s ",
               gPsCmd[PS_DIV], gPsCmd[PS_NEG], gPsCmd[PS_RMOVETO]);
         break;
      case JUST_R:
         fprintf(FP, "tgifstrw %s 0 %s ", gPsCmd[PS_NEG], gPsCmd[PS_RMOVETO]);
         break;
      }
      fprintf(FP, "tgifstrw 0 %s 0 %1d %s tgifstrw neg 0 %s %s ",
            gPsCmd[PS_RLINETO], rby-lty, gPsCmd[PS_RLINETO],
            gPsCmd[PS_RLINETO], gPsCmd[PS_CLOSEPATH]);
   } else {
      for (i=0; i < indent; i++) fprintf(FP, " ");
      fprintf(FP, "%s\n", gPsCmd[PS_NEWPATH]);
      for (i=0; i < indent; i++) fprintf(FP, " ");

      fprintf(FP, "   %1d %1d %s ", x, lty, gPsCmd[PS_MOVETO]);
      switch (just) {
      case JUST_L: break;
      case JUST_C:
         fprintf(FP, "tgifstrw 2 %s %s 0 %s\n",
               gPsCmd[PS_DIV], gPsCmd[PS_NEG], gPsCmd[PS_RMOVETO]);
         for (i=0; i < indent+3; i++) fprintf(FP, " ");
         break;
      case JUST_R:
         fprintf(FP, "tgifstrw %s 0 %s\n", gPsCmd[PS_NEG], gPsCmd[PS_RMOVETO]);
         for (i=0; i < indent+3; i++) fprintf(FP, " ");
         break;
      }
      fprintf(FP, "tgifstrw 0 %s ", gPsCmd[PS_RLINETO]);
      fprintf(FP, "0 %1d %s ", rby-lty, gPsCmd[PS_RLINETO]);
      fprintf(FP, "tgifstrw neg 0 %s\n", gPsCmd[PS_RLINETO]);
      for (i=0; i < indent; i++) fprintf(FP, " ");
      fprintf(FP, "%s ", gPsCmd[PS_CLOSEPATH]);
   }
}

static
void DumpTextFill(FP, ObjPtr)
   FILE *FP;
   struct ObjRec *ObjPtr;
{
   struct TextRec *text_ptr=ObjPtr->detail.t;
   int fill=text_ptr->fill, trans_pat=ObjPtr->trans_pat, ltx, lty, rbx, rby;
   int color_index=ObjPtr->color;
   int simple_text=SimpleTextObj(text_ptr);

   if (simple_text) {
      fprintf(FP, "   /tgifstrw 0 def\n");
      DumpSimpleTextPath(FP, ObjPtr);
   }
   DumpRGBColorLine(FP, color_index, 0, TRUE);

   if (ObjPtr->ctm == NULL) {
      ltx = ObjPtr->bbox.ltx;
      lty = ObjPtr->bbox.lty;
      rbx = ObjPtr->bbox.rbx-1;
      rby = ObjPtr->bbox.rby-1;
   } else {
      ltx = ObjPtr->detail.t->orig_bbox.ltx;
      lty = ObjPtr->detail.t->orig_bbox.lty;
      rbx = ObjPtr->detail.t->orig_bbox.rbx-1;
      rby = ObjPtr->detail.t->orig_bbox.rby-1;
   }
   if (ObjPtr->ctm != NULL) {
      float m[6];

      fprintf(FP, "%s\n", gPsCmd[PS_GSAVE]);
      m[CTM_SX] = ((float)ObjPtr->ctm->m[CTM_SX])/((float)1000.0);
      m[CTM_SY] = ((float)ObjPtr->ctm->m[CTM_SY])/((float)1000.0);
      m[CTM_SIN] = ((float)ObjPtr->ctm->m[CTM_SIN])/((float)1000.0);
      m[CTM_MSIN] = ((float)ObjPtr->ctm->m[CTM_MSIN])/((float)1000.0);
      fprintf(FP, "   %1d %1d %s\n", ObjPtr->x, ObjPtr->y,
            gPsCmd[PS_TRANSLATE]);
      fprintf(FP, "   [%.3f %.3f %.3f %.3f %1d %1d] %s\n",
            m[CTM_SX], m[CTM_SIN], m[CTM_MSIN], m[CTM_SY],
            ObjPtr->ctm->t[CTM_TX], ObjPtr->ctm->t[CTM_TY], gPsCmd[PS_CONCAT]);
      fprintf(FP, "   %1d %s %1d %s %s\n",
            ObjPtr->x, gPsCmd[PS_NEG], ObjPtr->y, gPsCmd[PS_NEG],
            gPsCmd[PS_TRANSLATE]);
   }
   switch (fill) {
   case NONEPAT: break;
   case SOLIDPAT:
      if (simple_text) {
         DumpSimpleTextBBoxPath(FP, text_ptr->minilines.just,
               ObjPtr->x, lty, rby, 0);
      } else {
         DumpRectPath(FP, ltx, lty, rbx, rby, 0, FALSE);
      }
      fprintf(FP, "%s\n", gPsCmd[PS_FILL]);
      break;
   case BACKPAT:
      if (!trans_pat) {
         if (simple_text) {
            DumpSimpleTextBBoxPath(FP, text_ptr->minilines.just,
                  ObjPtr->x, lty, rby, 0);
         } else {
            DumpRectPath(FP, ltx, lty, rbx, rby, 0, FALSE);
         }
         fprintf(FP, "1 %s %s\n", gPsCmd[PS_SETGRAY], gPsCmd[PS_FILL]);
         DumpRGBColorLine(FP, color_index, 0, TRUE);
      }
      break;
   default:
      /* patterned */
      fprintf(FP, "%s\n", gPsCmd[PS_GSAVE]);
      if (colorDump || !useGray) {
         if (!trans_pat) {
            if (simple_text) {
               DumpSimpleTextBBoxPath(FP, text_ptr->minilines.just,
                     ObjPtr->x, lty, rby, 3);
            } else {
               DumpRectPath(FP, ltx, lty, rbx, rby, 3, FALSE);
            }
            fprintf(FP, "1 %s %s\n", gPsCmd[PS_SETGRAY], gPsCmd[PS_FILL]);
            DumpRGBColorLine(FP, color_index, 3, TRUE);
         }
      } else {
         GrayCheck(fill);
         fprintf(FP, "      %s %s\n", GrayStr(fill), gPsCmd[PS_SETGRAY]);
      }
      if (colorDump || !useGray) {
         if (preDumpSetup) PSUseColorPattern();
         if (simple_text) {
            DumpSimpleTextBBoxPath(FP, text_ptr->minilines.just,
                  ObjPtr->x, lty, rby, 3);
         } else {
            DumpRectPath(FP, ltx, lty, rbx, rby, 3, FALSE);
         }
         fprintf(FP, "%s %s\n", gPsCmd[PS_EOCLIP], gPsCmd[PS_NEWPATH]);
         DumpPatFill(FP, fill, ObjPtr->bbox, 3, TRUE);
      } else {
         if (simple_text) {
            DumpSimpleTextBBoxPath(FP, text_ptr->minilines.just,
                  ObjPtr->x, lty, rby, 3);
         } else {
            DumpRectPath(FP, ltx, lty, rbx, rby, 3, FALSE);
         }
         fprintf(FP, "%s\n", gPsCmd[PS_FILL]);
      }
      fprintf(FP, "%s\n", gPsCmd[PS_GRESTORE]);
      break;
   }
   if (ObjPtr->ctm != NULL) fprintf(FP, "%s\n", gPsCmd[PS_GRESTORE]);
}

void DumpTextObj(FP, ObjPtr)
   FILE *FP;
   register struct ObjRec *ObjPtr;
{
   struct TextRec *text_ptr=ObjPtr->detail.t;
   MiniLinesInfo *minilines=(&text_ptr->minilines);
   int pen=text_ptr->pen, fill=text_ptr->fill, trans_pat=ObjPtr->trans_pat;
   int underline_y_offset=text_ptr->underline_y_offset;
   int overline_y_offset=text_ptr->overline_y_offset;

   if ((fill == NONEPAT || (trans_pat && fill == BACKPAT)) &&
         (pen == NONEPAT || (trans_pat && pen == BACKPAT))) {
      return;
   }
   if (ObjPtr->ctm != NULL && (ObjPtr->obbox.ltx == ObjPtr->obbox.rbx ||
         ObjPtr->obbox.lty == ObjPtr->obbox.rby)) {
      /* zero width or height text */
      return;
   }
   fprintf(FP, "%% TEXT\n");
   fprintf(FP, "%s\n", gPsCmd[PS_NEWPATH]);

   if (!PRTGIF) SaveCurFont();

   ObjFontInfoToCurFontInfo(text_ptr);

   penPat = pen;
   transPat = trans_pat;
   objFill = fill;
   curUnderlineYOffset = underline_y_offset;
   curOverlineYOffset = overline_y_offset;

   if (PRTGIF || text_ptr->read_only) {
      canvasFontAsc = minilines->first->asc;
      canvasFontDes = minilines->first->des;
      textCursorH = canvasFontAsc + canvasFontDes;
   }
   /* fill the bounding rectangle */
   DumpTextFill(FP, ObjPtr);

   if (penPat == NONEPAT) {
      fprintf(FP, "\n");
      if (!PRTGIF) RestoreCurFont();
      return;
   }
   if ((colorDump || !useGray) && penPat>BACKPAT && curFont==FONT_COU) {
      if (PRTGIF) {
         fprintf(stderr, "%s\n",
               TgLoadString(STID_WARN_PRINT_COURIER_IN_COLOR));
         fprintf(stderr, "%s\n",
               TgLoadString(STID_MAY_CAUSE_ERROR_IN_PRINTOUT));
      } else {
         TwoLineMsg(TgLoadString(STID_WARN_PRINT_COURIER_IN_COLOR),
               TgLoadString(STID_MAY_CAUSE_ERROR_IN_PRINTOUT));
      }
   }
   if ((colorDump || !useGray) && penPat > BACKPAT && !trans_pat) {
      int tmp_pen = penPat;

      penPat = BACKPAT;
      DumpTextPath(FP, ObjPtr);
      penPat = tmp_pen;
   }
   if (!(penPat == BACKPAT && trans_pat)) {
      DumpTextPath(FP, ObjPtr);
   }
   fprintf(FP, "\n");
   if (!PRTGIF) RestoreCurFont();
}

static
int NeedToCacheStrSeg(pStrSeg)
   StrSegInfo *pStrSeg;
{
   return (pStrSeg->double_byte && pStrSeg->double_byte_vertical);
}

static
int NeedToCacheStrBlock(pStrBlock)
   StrBlockInfo *pStrBlock;
{
   switch (pStrBlock->type) {
   case SB_SIMPLE: return NeedToCacheStrSeg(pStrBlock->seg);
   case SB_CHAR_SPACE: return FALSE;

   case SB_SUPSUB_LEFT:
   case SB_SUPSUB_CENTER:
   case SB_SUPSUB_RIGHT:
      if (pStrBlock->sup != NULL && NeedToCacheMiniLines(pStrBlock->sup)) {
         return TRUE;
      }
      if (pStrBlock->sub != NULL && NeedToCacheMiniLines(pStrBlock->sub)) {
         return TRUE;
      }
      if (pStrBlock->type == SB_SUPSUB_CENTER) {
         return NeedToCacheStrSeg(pStrBlock->seg);
      }
      break;
   }
   return FALSE;
}

static
int NeedToCacheMiniLine(pMiniLine)
   MiniLineInfo *pMiniLine;
{
   StrBlockInfo *pStrBlock=NULL;

   for (pStrBlock=pMiniLine->first_block; pStrBlock != NULL;
         pStrBlock=pStrBlock->next) {
      if (NeedToCacheStrBlock(pStrBlock)) {
         return TRUE;
      }
   }
   return FALSE;
}

int NeedToCacheMiniLines(minilines)
   MiniLinesInfo *minilines;
{
   MiniLineInfo *pMiniLine=NULL;

   for (pMiniLine=minilines->first; pMiniLine != NULL;
         pMiniLine=pMiniLine->next) {
      if (NeedToCacheMiniLine(pMiniLine)) {
         return TRUE;
      }
   }
   return FALSE;
}

int NeedsToCacheTextObj(ObjPtr)
   struct ObjRec *ObjPtr;
{
   struct TextRec *text_ptr=ObjPtr->detail.t;

   if (ObjPtr->ctm != NULL || zoomScale != 0 || text_ptr->read_only) {
      return TRUE;
   }
   return NeedToCacheMiniLines(&text_ptr->minilines);
}

void SaveString(FP, S)
   FILE *FP;
   register char *S;
{
   for ( ; *S != '\0'; S++) {
      if (*S == '\\') {
         if (fprintf(FP, "%s", "\\\\") == EOF) writeFileFailed = TRUE;
      } else if (*S == '"') {
         if (doubleQuoteDoubleQuote) {
            if (fprintf(FP, "%s", "\"\"") == EOF) writeFileFailed = TRUE;
         } else {
            if (fprintf(FP, "%s", "\\\"") == EOF) writeFileFailed = TRUE;
         }
      } else if ((*S) & 0x80) {
         if (fprintf(FP, "\\%o", (*S)&0xff) == EOF) writeFileFailed = TRUE;
      } else {
         if (fputc(*S, FP) == EOF) writeFileFailed = TRUE;
      }
   }
}

void SaveDoubleByteString(FP, S)
   FILE *FP;
   register char *S;
{
   for ( ; *S != '\0'; S++) {
      if ((*S)&0x80) {
         if (fprintf(FP, "\\%03o\\%03o", S[0]&0xff, S[1]&0xff) == EOF) {
            writeFileFailed = TRUE;
         }
         S++;
         if (*S == '\0') break;
      } else if (*S == '\\') {
         if (fprintf(FP, "%s", "\\\\") == EOF) writeFileFailed = TRUE;
      } else if (*S == '"') {
         if (doubleQuoteDoubleQuote) {
            if (fprintf(FP, "%s", "\"\"") == EOF) writeFileFailed = TRUE;
         } else {
            if (fprintf(FP, "%s", "\\\"") == EOF) writeFileFailed = TRUE;
         }
      } else {
         if (fputc(*S, FP) == EOF) writeFileFailed = TRUE;
      }
   }
}

static
void SaveStrSeg(FP, pStrSeg)
   FILE *FP;
   StrSegInfo *pStrSeg;
   /*
    * str_seg()
    */
{
   char font_str[MAXSTRING];
   int overunder=((pStrSeg->overline_on<<1)+pStrSeg->underline_on);

   GetPSFontStr(pStrSeg->font, pStrSeg->style, font_str);
         /* font_str starts with the '/' character */

   if (fprintf(FP, "str_seg('%s',", colorMenuItems[pStrSeg->color]) == EOF) {
      writeFileFailed = TRUE;
   }
   /*
    * When the new file format (version 38) is implemented, make sure to
    * save the DontReencode related information (dontreencode) in the file!
    */
   if (fprintf(FP,
         "'%s',%1d,%1d,%1d,%1d,%1d,%1d,%1d,%1d,%1d,%1d,%1d,%1d,",
         &font_str[1], pStrSeg->style, pStrSeg->sz_unit,
         pStrSeg->w, pStrSeg->asc, pStrSeg->des, pStrSeg->min_lbearing,
         pStrSeg->max_rextra, overunder,
         pStrSeg->double_byte, pStrSeg->double_byte_mod_bytes,
         pStrSeg->double_byte_vertical, pStrSeg->direction) == EOF) {
      writeFileFailed = TRUE;
   }
   if (fprintf(FP, "\n\t\"") == EOF) writeFileFailed = TRUE;
   if (pStrSeg->double_byte) {
      SaveDoubleByteString(FP, pStrSeg->dyn_str.s);
   } else {
      SaveString(FP, pStrSeg->dyn_str.s);
   }
   if (fprintf(FP, "\")") == EOF) writeFileFailed = TRUE;
}

static
void SaveAStrBlock(FP, pStrBlock)
   FILE *FP;
   StrBlockInfo *pStrBlock;
   /*
    * str_block()
    */
{
   int has_sup=FALSE, has_sub=FALSE;
   int something_saved=FALSE;

   switch (pStrBlock->type) {
   case SB_SUPSUB_LEFT:
   case SB_SUPSUB_CENTER:
   case SB_SUPSUB_RIGHT:
      if (pStrBlock->sup != NULL) has_sup = TRUE;
      if (pStrBlock->sub != NULL) has_sub = TRUE;
      break;
   }
   if (fprintf(FP, "str_block(%1d,%1d,%1d,%1d,%1d,%1d,%1d,%1d,%1d",
         pStrBlock->type, pStrBlock->w, pStrBlock->asc, pStrBlock->des,
         pStrBlock->min_lbearing, pStrBlock->max_rextra,
         pStrBlock->special_char_w, has_sup, has_sub) == EOF) {
      writeFileFailed = TRUE;
   }
   switch (pStrBlock->type) {
   case SB_SIMPLE:
      if (fprintf(FP, ",[\n") == EOF) writeFileFailed = TRUE;
      SaveStrSeg(FP, pStrBlock->seg);
      if (fprintf(FP, "]") == EOF) writeFileFailed = TRUE;
      break;
   case SB_CHAR_SPACE:
      break;

   case SB_SUPSUB_LEFT:
   case SB_SUPSUB_CENTER:
   case SB_SUPSUB_RIGHT:
      if (fprintf(FP, ",[\n") == EOF) writeFileFailed = TRUE;

      if (pStrBlock->sup != NULL) {
         SaveMiniLines(FP, pStrBlock->sup);
         something_saved = TRUE;
      }
      if (pStrBlock->sub != NULL) {
         if (something_saved) {
            if (fprintf(FP, ",\n") == EOF) writeFileFailed = TRUE;
         }
         SaveMiniLines(FP, pStrBlock->sub);
         something_saved = TRUE;
      }
      if (pStrBlock->type == SB_SUPSUB_CENTER) {
         if (something_saved) {
            if (fprintf(FP, ",\n") == EOF) writeFileFailed = TRUE;
         }
         /* pStrBlock->seg better not be NULL or it will crash */
         SaveStrSeg(FP, pStrBlock->seg);
         something_saved = TRUE;
      }
      if (something_saved) {
         if (fprintf(FP, "\n]") == EOF) writeFileFailed = TRUE;
      } else {
         if (fprintf(FP, "]") == EOF) writeFileFailed = TRUE;
      }
      break;
   }
   if (fprintf(FP, ")") == EOF) writeFileFailed = TRUE;
}

static
void SaveStrBlocks(FP, pFirstStrBlock)
   FILE *FP;
   StrBlockInfo *pFirstStrBlock;
   /*
    * ... [
    * str_block(),
    * str_block(),
    * ...
    * str_block()
    * ]
    */
{
   StrBlockInfo *pStrBlock;

   if (fprintf(FP, "[\n") == EOF) writeFileFailed = TRUE;

   for (pStrBlock=pFirstStrBlock; pStrBlock != NULL;
         pStrBlock=pStrBlock->next) {
      SaveAStrBlock(FP, pStrBlock);
      if (pStrBlock->next != NULL) {
         if (fprintf(FP, ",\n") == EOF) {
            writeFileFailed = TRUE;
         }
      }
   }
   if (pFirstStrBlock == NULL) {
      if (fprintf(FP, "]") == EOF) writeFileFailed = TRUE;
   } else {
      if (fprintf(FP, "\n]") == EOF) writeFileFailed = TRUE;
   }
}

static
void SaveAMiniLine(FP, pMiniLine)
   FILE *FP;
   MiniLineInfo *pMiniLine;
   /*
    * mini_line(w,asc,des,min_lbearing,max_rextra,v_gap)
    */
{
   if (pMiniLine == pMiniLine->owner_minilines->first &&
         pMiniLine->v_gap != 0) {
      TgAssert(FALSE, "First mini_line has non-zero v_gap in SaveAMiniLine()",
            "It's set to 0");
      pMiniLine->v_gap = 0;
   }
   if (fprintf(FP, "mini_line(%1d,%1d,%1d,%1d,%1d,%1d,",
         pMiniLine->w, pMiniLine->asc, pMiniLine->des, pMiniLine->min_lbearing,
         pMiniLine->max_rextra, pMiniLine->v_gap) == EOF) {
      writeFileFailed = TRUE;
   }
   SaveStrBlocks(FP, pMiniLine->first_block);

   if (fprintf(FP, ")") == EOF) writeFileFailed = TRUE;
}

void SaveMiniLines(FP, minilines)
   FILE *FP;
   MiniLinesInfo *minilines;
   /*
    * ... [
    * minilines(w,h,min_lbearing,max_rextra,just,v_space,baseline_offset,[
    * mini_line(),
    * mini_line(),
    * ...
    * mini_line()
    * ])
    * ]
    */
{
   MiniLineInfo *pMiniLine=NULL;

   if (fprintf(FP, "minilines(%1d,%1d,%1d,%1d,%1d,%1d,%1d,[\n",
         minilines->w, minilines->h, minilines->min_lbearing,
         minilines->max_rextra, minilines->just, minilines->v_space,
         minilines->baseline_offset) == EOF) {
      writeFileFailed = TRUE;
   }
   for (pMiniLine=minilines->first; pMiniLine != NULL;
         pMiniLine=pMiniLine->next) {
      SaveAMiniLine(FP, pMiniLine);
      if (pMiniLine->next != NULL) {
         if (fprintf(FP, ",\n") == EOF) writeFileFailed = TRUE;
      }
   }
   if (fprintf(FP, "\n])") == EOF) writeFileFailed = TRUE;
}

void CombineOverUnderYOffsets(underline_y_offset, overline_y_offset,
      pn_combined)
   int underline_y_offset, overline_y_offset, *pn_combined;
{
   underline_y_offset = (underline_y_offset & 0x0ffff);
   overline_y_offset = (overline_y_offset & 0x0ffff);
   *pn_combined = ((overline_y_offset << 16) | underline_y_offset);
}

void UncombineOverUnderYOffsets(combined, pn_underline_y_offset,
      pn_overline_y_offset)
   int combined, *pn_underline_y_offset, *pn_overline_y_offset;
{
   int underline_y_offset=(combined & 0x0ffff);
   int overline_y_offset=((combined >> 16) & 0x0ffff);

   if ((underline_y_offset & 0x08000) == 0x08000) {
      underline_y_offset = (0x010000 - abs(underline_y_offset));
      underline_y_offset = -underline_y_offset;
   }
   if ((overline_y_offset & 0x08000) == 0x08000) {
      overline_y_offset = (0x010000 - abs(overline_y_offset));
      overline_y_offset = -overline_y_offset;
   }
   *pn_underline_y_offset = underline_y_offset;
   *pn_overline_y_offset = overline_y_offset;
}

void SaveTextObj(FP, ObjPtr)
   FILE *FP;
   struct ObjRec *ObjPtr;
{
   struct TextRec *text_ptr=ObjPtr->detail.t;
   int compressed=FALSE, combined_y_offset=0;

   CombineOverUnderYOffsets(text_ptr->underline_y_offset,
         text_ptr->overline_y_offset, &combined_y_offset);
   if (fprintf(FP, "text('%s',", colorMenuItems[ObjPtr->color]) == EOF) {
      writeFileFailed = TRUE;
   }
   if (fprintf(FP,
         "%1d,%1d,%1d,%1d,%1d,%1d,%1d,%1d,%1d,%1d,",
         ObjPtr->x, ObjPtr->y, text_ptr->lines, text_ptr->minilines.just,
         text_ptr->pen, ObjPtr->obbox.rbx-ObjPtr->obbox.ltx,
         ObjPtr->obbox.rby-ObjPtr->obbox.lty, ObjPtr->id,
         text_ptr->minilines.first->first_block->asc,
         text_ptr->minilines.first->first_block->des) == EOF) {
      writeFileFailed = TRUE;
   }
   if (fprintf(FP,
         "%1d,%1d,%1d,%1d,%1d,%1d,%1d,%1d,%1d,\"\",",
         text_ptr->fill, text_ptr->minilines.v_space, ObjPtr->rotation,
         ObjPtr->locked, combined_y_offset, text_ptr->w,
         text_ptr->h, text_ptr->min_lbearing, text_ptr->max_rextra) == EOF) {
      writeFileFailed = TRUE;
   }
   if (fprintf(FP,
         "%1d,%1d,%1d,%1d,%1d,'',",
         compressed, ObjPtr->ctm!=NULL, ObjPtr->invisible,
         ObjPtr->trans_pat, text_ptr->baseline_y) == EOF) {
      writeFileFailed = TRUE;
   }
   if (ObjPtr->ctm != NULL && fprintf(FP,
         "[\n\t%1d,%1d,%1d,%1d,%1d,%1d,%g,%g,%g,%g,%1d,%1d,%1d,%1d,%1d,%1d],",
         ObjPtr->x, ObjPtr->y,
         ObjPtr->orig_obbox.ltx, ObjPtr->orig_obbox.lty,
         ObjPtr->orig_obbox.rbx, ObjPtr->orig_obbox.rby,
         ObjPtr->ctm->m[CTM_SX], ObjPtr->ctm->m[CTM_SIN],
         ObjPtr->ctm->m[CTM_MSIN], ObjPtr->ctm->m[CTM_SY],
         ObjPtr->ctm->t[CTM_TX], ObjPtr->ctm->t[CTM_TY],
         text_ptr->orig_bbox.ltx, text_ptr->orig_bbox.lty,
         text_ptr->orig_bbox.rbx, text_ptr->orig_bbox.rby) == EOF) {
      writeFileFailed = TRUE;
   }
   if (serializingFile) SaveCreatorID(FP, ObjPtr, "\t");
   if (fprintf(FP, "[\n") == EOF) writeFileFailed = TRUE;
   if (text_ptr->minilines.baseline_offset != 0) {
      TgAssert(FALSE,
            "Minilines of text object has bad baseline_offset in SaveTextObj()",
            "It's set to 0");
      text_ptr->minilines.baseline_offset = 0;
   }
   SaveMiniLines(FP, &text_ptr->minilines);
   if (fprintf(FP, "])") == EOF) writeFileFailed = TRUE;
}

void SaveHexString(FP, buf)
   FILE *FP;
   char *buf;
{
   for ( ; *buf != '\0'; buf++) {
      int ival=0;

      ival = ((((unsigned int)(*buf)) >> 4) & 0x0f);
      if (ival >= 10) {
         fprintf(FP, "%c", (char)(((int)'a')+(ival-10)));
      } else {
         fprintf(FP, "%c", (char)(((int)'0')+ival));
      }
      ival = (((unsigned int)(*buf)) & 0x0f);
      if (ival >= 10) {
         fprintf(FP, "%c", (char)(((int)'a')+(ival-10)));
      } else {
         fprintf(FP, "%c", (char)(((int)'0')+ival));
      }
   }
}

char *ReadString(Str)
   char *Str;
{
   register char *s=NULL;

   if (Str == NULL) return NULL;

   for (s=Str; *s != '\0'; s++) {
      if (*s == '"') {
         if (s[1] == '"') {
            UtilStrCpy(s, s+1);
         } else {
            break;
         }
      } else if (*s == '\\') {
         if (s[1] >= '0' && s[1] <= '3') {
            if (s[2] >= '0' && s[2] <= '7' && s[3] >= '0' && s[3] <= '7') {
               *s = (char)(((s[1]-'0')<<6)+((s[2]-'0')<<3)+(s[3]-'0'));
               UtilStrCpy(s+1, s+4);
            } else {
               sprintf(gszMsgBox,
                     TgLoadString(STID_BAD_OCTAL_STRING_ENCOUNTERED),
                     s[1], s[2], s[3]);
               if (PRTGIF) {
                  fprintf(stderr, "%s\n", gszMsgBox);
               } else {
                  Msg(gszMsgBox);
               }
               UtilStrCpy(s, s+1);
            }
         } else {
            UtilStrCpy(s, s+1);
         }
      }
   }
   if (*s == '"') s++;
   return s;
}

StrSegInfo *CreateStrSegFromString(buf, pOwnerStrBlock)
   char *buf;
   StrBlockInfo *pOwnerStrBlock;
{
   StrSegInfo *pStrSeg=NewStrSeg();

   DynStrSet(&pStrSeg->dyn_str, buf);

   pStrSeg->color = colorIndex;
   pStrSeg->font = curFont;
   pStrSeg->style = curStyle;
   pStrSeg->sz_unit = GetCurSzUnit();
   if (mainDisplay != NULL) {
      strcpy(pStrSeg->color_str, colorMenuItems[colorIndex]);
   }
   pStrSeg->double_byte = canvasFontDoubleByte;
   pStrSeg->double_byte_mod_bytes = canvasFontDoubleByteModBytes;
   pStrSeg->double_byte_vertical = canvasFontDoubleByteVertical;
   pStrSeg->direction = canvasFontDirection;
   pStrSeg->dontreencode = canvasFontDontReencode;
   pStrSeg->underline_on = curUnderlineOn;
   pStrSeg->overline_on = curOverlineOn;

   pStrSeg->asc = canvasFontAsc;
   pStrSeg->des = canvasFontDes;
   if (mainDisplay != NULL) {
      pStrSeg->w = MyTextWidth(canvasFontPtr, pStrSeg->dyn_str.s,
            pStrSeg->dyn_str.sz-1);
   }
   pStrSeg->owner = pOwnerStrBlock;

   return pStrSeg;
}

StrBlockInfo *CreateStrBlockFromString(buf, pOwnerMiniLine)
   char *buf;
   MiniLineInfo *pOwnerMiniLine;
{
   StrBlockInfo *pStrBlock=NewStrBlock();

   pStrBlock->seg = CreateStrSegFromString(buf, pStrBlock);
   pStrBlock->type = SB_SIMPLE;
   pStrBlock->owner_mini_line = pOwnerMiniLine;

   return pStrBlock;
}

MiniLineInfo *CreateMiniLineFromString(buf, ppFirstMiniLine, ppLastMiniLine)
   char *buf;
   MiniLineInfo **ppFirstMiniLine, **ppLastMiniLine;
{
   MiniLineInfo *pMiniLine=NewMiniLine();

   if (canvasFontDoubleByte) {
      struct StrRec *pStr=NULL, *pStr1=NULL;

      pStr = SegmentDoubleByteString(buf);
      for (pStr1=pStr; pStr1 != NULL; pStr1=pStr1->next) {
         int str_seg_double_byte=TRUE;
         StrBlockInfo *pStrBlock=CreateStrBlockFromString(pStr1->dyn_str.s,
               pMiniLine);

         if (*pStr1->dyn_str.s != '\0') {
            str_seg_double_byte = ((*pStr1->dyn_str.s) & 0x80);
         }
         pStrBlock->prev = pMiniLine->last_block;
         if (pMiniLine->first_block == NULL) {
            pMiniLine->first_block = pStrBlock;
         } else {
            pMiniLine->last_block->next = pStrBlock;
         }
         pMiniLine->last_block = pStrBlock;

         if (str_seg_double_byte == FALSE) {
            pStrBlock->seg->font = defaultSingleByteFont;
            pStrBlock->seg->double_byte = FALSE;
            pStrBlock->seg->double_byte_mod_bytes = FALSE;
            pStrBlock->seg->double_byte_vertical = FALSE;
         }
      }
      FreeStrList(pStr);
   } else {
      StrBlockInfo *pStrBlock=CreateStrBlockFromString(buf, pMiniLine);

      pMiniLine->first_block = pMiniLine->last_block = pStrBlock;
   }
   if (ppFirstMiniLine != NULL && ppLastMiniLine != NULL) {
      pMiniLine->prev = (*ppLastMiniLine);
      pMiniLine->next = NULL;;
      if ((*ppLastMiniLine) == NULL) {
         (*ppFirstMiniLine) = pMiniLine;
      } else {
         (*ppLastMiniLine)->next = pMiniLine;
      }
      (*ppLastMiniLine) = pMiniLine;
   }
   return pMiniLine;
}

struct ObjRec *FormTextObjectFromFile(FP, AbsX, AbsBaselineY)
   FILE *FP;
   int AbsX, AbsBaselineY;
{
   MiniLineInfo *pFirstMiniLine=NULL, *pLastMiniLine=NULL;
   struct ObjRec *obj_ptr=NULL;
   struct TextRec *text_ptr=NULL;
   int num_lines=0;

   obj_ptr = (struct ObjRec *)malloc(sizeof(struct ObjRec));
   if (obj_ptr == NULL) FailAllocMessage();
   memset(obj_ptr, 0, sizeof(struct ObjRec));

   text_ptr = (struct TextRec *)malloc(sizeof(struct TextRec));
   if (text_ptr == NULL) FailAllocMessage();
   memset(text_ptr, 0, sizeof(struct TextRec));

   if (FP != NULL) {
      char *buf=NULL;

      while ((buf=UtilGetALine(FP)) != NULL) {
         MiniLineInfo *pMiniLine=CreateMiniLineFromString(buf,
               &pFirstMiniLine, &pLastMiniLine);

         if (pMiniLine != NULL) {
            num_lines++;
         }
         free(buf);
      }
   }
   if (num_lines == 0) {
      CreateMiniLineFromString("", &pFirstMiniLine, &pLastMiniLine);

      num_lines++;
   }
   text_ptr->lines = num_lines;
   text_ptr->minilines.first = pFirstMiniLine;
   text_ptr->minilines.last = pLastMiniLine;
   text_ptr->baseline_y = AbsBaselineY;

   CopyCurInfoIntoTextPtr(obj_ptr, text_ptr);

   obj_ptr->x = AbsX;
   obj_ptr->y = AbsBaselineY-text_ptr->minilines.first->asc;
   obj_ptr->type = OBJ_TEXT;
   obj_ptr->color = colorIndex;
   if (mainDisplay != NULL) {
      UtilStrCpyN(obj_ptr->color_str, sizeof(obj_ptr->color_str),
            colorMenuItems[colorIndex]);
   }
   obj_ptr->id = objId++;;
   obj_ptr->dirty = FALSE;
   obj_ptr->rotation = 0;
   obj_ptr->detail.t = text_ptr;
   obj_ptr->fattr = obj_ptr->lattr = NULL;
   obj_ptr->ctm = NULL;

   RecalcTextMetrics(text_ptr, AbsX, AbsBaselineY);

   SetTextBBox(obj_ptr, textJust, text_ptr->w, text_ptr->h,
         text_ptr->min_lbearing, text_ptr->max_rextra, ROTATE0);
   AdjObjBBox(obj_ptr);
   return obj_ptr;
}

static
int PaintLeftText(Str, Just, LtX, LtY)
   char *Str;
   int Just, LtX, LtY;
   /* LtX and LtY are UNSCALED screen offset */
{
   register int amount;

   if (zoomScale != 0) {
      LtX = ZOOMED_SIZE(LtX);
      LtY = ZOOMED_SIZE(LtY);
      amount = MyTextWidth(canvasFontPtr, Str, strlen(Str));
      BlurText(drawWindow, drawGC, LtX, LtY,
            (zoomedIn ? ((amount<<zoomScale)+1) : (amount>>zoomScale)+1),
            (zoomedIn ? (textCursorH<<zoomScale)+1 :
                        (textCursorH>>zoomScale)+1));
      return amount;
   }

   LtY += canvasFontAsc;
   amount = MyTextWidth(canvasFontPtr, Str, strlen(Str));
   MyDrawString(mainDisplay, drawWindow, drawGC, mainDepth, LtX, LtY,
            Str, strlen(Str));

   return amount; /* return the length of the painted string */
}

void RepaintFirstStr(ObjPtr, Str)
   struct ObjRec *ObjPtr;
   char *Str;
   /* Replace (graphically) the FIRST string of the text in ObjPtr by Str */
{
   struct TextRec *text_ptr=ObjPtr->detail.t;
   char *s=GetTextPtrFirstStrSeg(text_ptr)->dyn_str.s, *s1=Str;
   char tmp_str[MAXSTRING+1], *c_ptr;
   int len;
   struct BBRec bbox;
   XGCValues values;

   bbox.ltx = ObjPtr->obbox.ltx; bbox.lty = ObjPtr->obbox.lty;
   bbox.rbx = ObjPtr->obbox.rbx; bbox.rby = ObjPtr->obbox.rby;

   c_ptr = tmp_str;
   for ( ; *s != '\0' && *s1 != '\0' && *s1 == *s; *c_ptr++ = *s++, s1++) ;

   if (*s == *s1) return; /* no updates */
   text_ptr->attr->owner->dirty = TRUE;
   *c_ptr = '\0';

   PushCurFont();
   ObjFontInfoToCurFontInfo(text_ptr);
   SetCanvasFont();

   if (*s != '\0') {
      values.foreground = GetDrawingBgPixel(INVALID, INVALID);
      values.function = GXcopy;
      values.fill_style = FillSolid;
      XChangeGC(mainDisplay, drawGC,
            GCForeground | GCFunction | GCFillStyle, &values);

      len = MyTextWidth(canvasFontPtr, tmp_str, strlen(tmp_str));
      XFillRectangle(mainDisplay, drawWindow, drawGC, OFFSET_X(bbox.ltx+len),
            OFFSET_Y(bbox.lty),
            (zoomedIn ? ((bbox.rbx-bbox.ltx-len)<<zoomScale)+1 :
                        ((bbox.rbx-bbox.ltx-len)>>zoomScale)+1),
            (zoomedIn ? (textCursorH<<zoomScale)+1 :
                        (textCursorH>>zoomScale)+1));

      values.foreground = colorPixels[ObjPtr->color];
      XChangeGC(mainDisplay, drawGC, GCForeground, &values);
   } else {
      values.foreground = colorPixels[ObjPtr->color];
      values.function = GXcopy;
      values.fill_style = FillSolid;
      XChangeGC(mainDisplay, drawGC,
            GCForeground | GCFunction | GCFillStyle, &values);
   }

   ObjPtr->bbox.rbx = ObjPtr->obbox.rbx = bbox.ltx + PaintLeftText(Str,
         textJust, bbox.ltx-drawOrigX, bbox.lty-drawOrigY);

   PopCurFont();
}
