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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/text.c,v 1.73 2011/06/14 02:32:19 william Exp $
 */

#define _INCLUDE_FROM_TEXT_C_

#include "tgifdefs.h"
#include "cmdids.h"

#include "attr.e"
#include "auxtext.e"
#include "choice.e"
#include "cmd.e"
#include "color.e"
#ifndef _NO_KINPUT
#include "convkinput.e"
#endif /* ~_NO_KINPUT */
#include "cutpaste.e"
#include "cursor.e"
#include "dialog.e"
#include "drawing.e"
#include "dup.e"
#include "edit.e"
#include "exec.e"
#include "file.e"
#include "font.e"
#include "grid.e"
#include "inmethod.e"
#include "mainloop.e"
#include "mainmenu.e"
#include "mark.e"
#include "menu.e"
#include "miniline.e"
#include "move.e"
#include "msg.e"
#include "names.e"
#ifndef _NO_NKF
#include "nkf.e"
#endif /* ~_NO_NKF */
#include "obj.e"
#include "pattern.e"
#include "poly.e"
#include "prtgif.e"
#include "raster.e"
#include "rect.e"
#include "ruler.e"
#include "scroll.e"
#include "select.e"
#include "setup.e"
#include "stretch.e"
#include "strtbl.e"
#include "text.e"
#include "tginssym.e"
#include "util.e"
#include "wb.e"
#include "xbitmap.e"
#include "xpixmap.e"

#define ADVANCE_LEFT (TRUE)
#define ADVANCE_RIGHT (FALSE)

#define POSITION_END (TRUE)
#define POSITION_CUR (FALSE)

int textDrawn=FALSE;
int curTextModified=FALSE;
int textVSpace=0;

int textJust=JUST_L;
int textCursorShown=FALSE;
int textCursorH=14; /* UNSCALED height of the text cursor */
int editingText=FALSE; /* editing existing text */
int curTextIsNew=FALSE;

int editTextSize=0;

int readingTextObject=FALSE;

float scriptFraction=((float)0.6);
char scriptFractionStr[80];

int deleteNextCharWithDelKey=TRUE;
int lengthLimit256InInsertChar=FALSE;

static struct ObjRec *justDrawnTextObj=NULL;

struct tagBeforeEditTextInfo {
   int x, baseline_y;
   int obbox_w, obbox_h;
   int first_mini_line_asc;
   struct BBRec bbox, obbox;
} beforeEditTextInfo;

static int tmpAdjAbsX=0, tmpAdjAbsY=0; /* absolute coordinates */
static int curTextMovedAbsX=0, curTextMovedAbsY=0; /* absolute coordinates */

/* --------------------- DynStr Routines --------------------- */

void FreeDynStrBuf(dyn_str)
   struct DynStrRec *dyn_str;
{
   if (dyn_str == NULL) return;
   UtilFree(dyn_str->s);
   memset(dyn_str, 0, sizeof(struct DynStrRec));
}

void DynStrCpy(dest_dyn_str, src_dyn_str)
   struct DynStrRec *dest_dyn_str, *src_dyn_str;
{
   char *psz_new=NULL;
   int new_sz=src_dyn_str->sz;

   if (new_sz == 0) {
#ifdef _TGIF_DBG /* debug, do not translate */
      TgAssert(FALSE, "src_dyn_str has 0 size in DynStrCpy()", "Fixed");
#endif /* _TGIF_DBG */
      new_sz++;
      psz_new = (char *)malloc(sizeof(char));
      if (psz_new == NULL) FailAllocMessage();
      *psz_new = '\0';
   } else {
      psz_new = (char *)malloc(new_sz*sizeof(char));
      if (psz_new == NULL) FailAllocMessage();
      strcpy(psz_new, src_dyn_str->s);
   }
   if (dest_dyn_str->s != NULL) free(dest_dyn_str->s);
   dest_dyn_str->s = psz_new;
   dest_dyn_str->sz = new_sz;
}

struct DynStrRec *NewDynStr()
{
   struct DynStrRec *new_dyn_str=NULL;

   new_dyn_str = (struct DynStrRec *)malloc(sizeof(struct DynStrRec));
   if (new_dyn_str == NULL) FailAllocMessage();
   new_dyn_str->s = NULL;
   new_dyn_str->sz = 0;
   return new_dyn_str;
}

struct DynStrRec *DynStrDup(dyn_str)
   struct DynStrRec *dyn_str;
{
   struct DynStrRec *new_dyn_str=NULL;

   new_dyn_str = (struct DynStrRec *)malloc(sizeof(struct DynStrRec));
   if (new_dyn_str == NULL) FailAllocMessage();
   new_dyn_str->s = NULL;
   DynStrCpy(new_dyn_str, dyn_str);
   return new_dyn_str;
}

void DynStrSet(dest_dyn_str, buf)
   struct DynStrRec *dest_dyn_str;
   char *buf;
{
   char *psz_new=NULL;
   int sz=strlen(buf)+1;

   psz_new = (char*)malloc(sz*sizeof(char));
   if (psz_new == NULL) FailAllocMessage();
   strcpy(psz_new, buf);
   if (dest_dyn_str->s != NULL) free(dest_dyn_str->s);
   dest_dyn_str->s = psz_new;
   dest_dyn_str->sz = sz;
}

/* --------------------- Str Routines --------------------- */

void FreeStr(str_ptr)
   struct StrRec *str_ptr;
{
   if (str_ptr->dyn_str.s != NULL) free(str_ptr->dyn_str.s);
   free(str_ptr);
}

void FreeStrList(str_ptr)
   struct StrRec *str_ptr;
{
   while (str_ptr != NULL) {
      struct StrRec *next_str_ptr=str_ptr->next;

      FreeStr(str_ptr);
      str_ptr = next_str_ptr;
   }
}

struct StrRec *NewStr()
{
   struct StrRec *new_str_ptr=NULL;

   new_str_ptr = (struct StrRec *)malloc(sizeof(struct StrRec));
   if (new_str_ptr == NULL) FailAllocMessage();
   memset(new_str_ptr, 0, sizeof(struct StrRec));
   DynStrSet(&new_str_ptr->dyn_str, "");
   return new_str_ptr;
}

/* --------------------- Text Routines --------------------- */

void SetTextHighlight()
{
   textHighlight = (endStrBlock != NULL && (curStrBlock != endStrBlock ||
         textCurIndex != textEndIndex));
}

typedef struct tagCursorPositionInCurText {
   struct ObjRec *cur_text_obj;
   StrBlockInfo *cur_str_block, *end_str_block;
   int text_cur_index, text_end_index;
} CursorPositionInCurTextInfo;

static CursorPositionInCurTextInfo gstCursorPositionInCurText;

void SaveCursorPositionInCurText()
{
   gstCursorPositionInCurText.cur_text_obj = curTextObj;
   gstCursorPositionInCurText.cur_str_block = curStrBlock;
   gstCursorPositionInCurText.end_str_block = endStrBlock;
   gstCursorPositionInCurText.text_cur_index = textCurIndex;
   gstCursorPositionInCurText.text_end_index = textEndIndex;
}

void RestoreCursorPositionInCurText()
{
   curTextObj = gstCursorPositionInCurText.cur_text_obj;
   curStrBlock = gstCursorPositionInCurText.cur_str_block;
   endStrBlock = gstCursorPositionInCurText.end_str_block;
   textCurIndex = gstCursorPositionInCurText.text_cur_index;
   textEndIndex = gstCursorPositionInCurText.text_end_index;
   SetTextHighlight();
}

void InvalidateTextCache(text_ptr)
   struct TextRec *text_ptr;
{
   if (text_ptr->cached_bitmap != None) {
      XFreePixmap(mainDisplay, text_ptr->cached_bitmap);
   }
   if (text_ptr->cached_pixmap != None) {
      XFreePixmap(mainDisplay, text_ptr->cached_pixmap);
   }
   if (text_ptr->cached_bg_bitmap != None) {
      XFreePixmap(mainDisplay, text_ptr->cached_bg_bitmap);
   }
   text_ptr->cached_zoom = 0;
   text_ptr->cached_bitmap = None;
   text_ptr->cached_pixmap = None;
   text_ptr->cached_bg_bitmap = None;
}

void TellTextCursorPosition(pnX, pnY)
   int *pnX, *pnY;
{
   *pnX = textCurX;
   *pnY = textCurY + textCursorH;
}

#define BLUR 32

void BlurText(Win, gc, XOff, YOff, W, H)
   Window Win;
   GC gc;
   int XOff, YOff, W, H;
   /* XOff and YOff are screen offsets (scaled and translated) */
{
   XPoint v[5];

   v[0].x = (short)XOff; v[0].y = (short)YOff;
   v[1].x = (short)XOff; v[1].y = (short)YOff+H+1;
   v[2].x = (short)XOff+W+1; v[2].y = (short)YOff+H+1;
   v[3].x = (short)XOff+W+1; v[3].y = (short)YOff;
   v[4].x = (short)XOff; v[4].y = (short)YOff;

   XFillPolygon(mainDisplay, Win, gc, v, 5, Convex, CoordModeOrigin);
}

void InitText()
{
   char *c_ptr=NULL;
   XGCValues values;

   textBackingBitmap = XCreatePixmap(mainDisplay,mainWindow,10,10,1);
   if (textBackingBitmap==None) {
      sprintf(gszMsgBox, TgLoadString(STID_CANT_ALLOC_BITMAP_OF_SIZE), 10, 10);
      Error("InitText()", gszMsgBox);
   }
   textBackingBgBitmap = XCreatePixmap(mainDisplay,mainWindow,10,10,1);
   if (textBackingBgBitmap==None) {
      sprintf(gszMsgBox, TgLoadString(STID_CANT_ALLOC_BITMAP_OF_SIZE), 10, 10);
      Error("InitText()", gszMsgBox);
   }
   textBackingPixmap = XCreatePixmap(mainDisplay,mainWindow,10,10,mainDepth);
   if (textBackingPixmap==None) {
      sprintf(gszMsgBox, TgLoadString(STID_CANNOT_ALLOC_PIXMAP_OF_SIZE), 10,
            10);
      Error("InitText()", gszMsgBox);
   }
   textBackingBitmapW = 10;
   textBackingBitmapH = 10;

   values.foreground = 1;
   values.background = 0;
   values.fill_style = FillSolid;
   values.function = GXcopy;
   rotateGC = XCreateGC(mainDisplay, textBackingBitmap,
         GCForeground | GCBackground | GCFillStyle | GCFunction,
         &values);
   if (rotateGC==NULL) {
      Error("InitText()", TgLoadString(STID_CANNOT_CREATE_GC));
   }
   scriptFraction = 0.6;
   strcpy(scriptFractionStr, "0.6");
   if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"ScriptFraction")) != NULL) {
      strcpy(scriptFractionStr, c_ptr);
      scriptFraction = (float)atof(c_ptr);
      if (scriptFraction < 0.2 || scriptFraction > 1.01) {
         fprintf(stderr, TgLoadString(STID_INVALID_XDEF_USE_ALT_STR),
               TOOL_NAME, "ScriptFraction", scriptFractionStr, "0.6");
         fprintf(stderr, "\n");
         scriptFraction = 0.6;
         strcpy(scriptFractionStr, "0.6");
      } else if (strcmp(scriptFractionStr, "0.6") == 0) {
         scriptFraction = 0.6;
         strcpy(scriptFractionStr, "0.6");
      }
   }
   deleteNextCharWithDelKey = TRUE;
   if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"DeleteNextCharWithDelKey")) !=
         NULL && UtilStrICmp("false", c_ptr) == 0) {
      deleteNextCharWithDelKey = FALSE;
   }
   lengthLimit256InInsertChar = FALSE;
   if ((c_ptr=XGetDefault(mainDisplay, TOOL_NAME,
         "LengthLimit256InInsertChar")) != NULL &&
         UtilStrICmp("true", c_ptr) == 0) {
      lengthLimit256InInsertChar = TRUE;
   }
}

void CleanUpText()
{
   if (textBackingBitmap != None) {
      XFreePixmap(mainDisplay, textBackingBitmap);
   }
   if (textBackingBgBitmap != None) {
      XFreePixmap(mainDisplay, textBackingBgBitmap);
   }
   if (textBackingPixmap != None) {
      XFreePixmap(mainDisplay, textBackingPixmap);
   }
   textBackingBitmap = None;
   textBackingBitmapW = textBackingBitmapH = 0;
   if (rotateGC != NULL) XFreeGC(mainDisplay, rotateGC);
   rotateGC = NULL;
}

static struct BBRec editTextAreaBBox; /* coordinates are screen offsets */

static
void SetEditTextArea(w, h, min_lbearing, max_rextra)
   int w, h, min_lbearing, max_rextra;
{
   memset(&editTextAreaBBox, 0, sizeof(struct BBRec));
   SetBBRec(&editTextAreaBBox,
         textOrigX, textOrigY, textOrigX+w, textOrigY+h);
   textW = w;
   textH = h;
   textAbsMinLBearing = min_lbearing;
   textAbsMaxRExtra = max_rextra;
}

static
void UpdateEditTextArea(w, h, min_lbearing, max_rextra)
   int w, h, min_lbearing, max_rextra;
{
   struct BBRec bbox;
   int text_area_enlarged=FALSE;

   SetBBRec(&bbox, textOrigX, textOrigY, textOrigX+w, textOrigY+h);
   UnionRect(&editTextAreaBBox, &bbox, &editTextAreaBBox);
   w = editTextAreaBBox.rbx-editTextAreaBBox.ltx;
   h = editTextAreaBBox.rby-editTextAreaBBox.lty;

   if (w > textW) {
      textW = w;
      text_area_enlarged = TRUE;
   }
   if (h > textH) {
      textH = h;
      text_area_enlarged = TRUE;
   }
   if (min_lbearing < textAbsMinLBearing) {
      textAbsMinLBearing = min_lbearing;
      text_area_enlarged = TRUE;
   }
   if (max_rextra > textAbsMaxRExtra) {
      textAbsMaxRExtra = max_rextra;
      text_area_enlarged = TRUE;
   }
   if (text_area_enlarged) {
      ForceDirtyBBoxToRedrawAll();
   }
}

static
void AdjTextVerticalShift(clean_bbox)
   int clean_bbox;
{
   struct TextRec *text_ptr=curTextObj->detail.t;
   int w=0, h=0, saved_first_miniline_asc=0, new_first_miniline_asc=0;

   saved_first_miniline_asc = text_ptr->minilines.first->asc;

   UpdTextBBox(curTextObj);

   new_first_miniline_asc = text_ptr->minilines.first->asc;
   if (new_first_miniline_asc > saved_first_miniline_asc) {
      int saved_orig_y=textOrigY;

      textAbsY = textAbsBaselineY - new_first_miniline_asc;
      textOrigY = OFFSET_Y(textAbsY);
      textCurY += (textOrigY - saved_orig_y);
   }
   w = text_ptr->minilines.w;
   h = (textOrigBaselineY - textOrigY) +
         (text_ptr->minilines.h - new_first_miniline_asc);

   SetTextCurXY();
   if (textHighlight) SetTextEndXY();

   UpdateEditTextArea(w, h, text_ptr->minilines.min_lbearing,
         text_ptr->minilines.max_rextra);
   UpdateCurTextBBoxes(clean_bbox);
}

void NewCurText()
{
   MiniLinesInfo *minilines=NULL;
   struct TextRec *text_ptr=NULL;

   if (textCursorH+textVSpace <= 0) {
      textVSpace = 0;
      ShowTextVSpace();
      MsgBox(TgLoadString(STID_TEXT_VSPACE_TOO_SMALL_RESET_0), TOOL_NAME,
            INFO_MB);
   }
   PushCurFont();

   text_ptr = (struct TextRec *)malloc(sizeof(struct TextRec));
   if (text_ptr == NULL) FailAllocMessage();
   memset(text_ptr, 0, sizeof(struct TextRec));

   minilines = (&text_ptr->minilines);

   CreateMiniLineFromString("", &minilines->first, &minilines->last);

   text_ptr->attr = NULL;

   minilines->just = textJust;
   minilines->v_space = textVSpace;
   minilines->first->asc = canvasFontAsc;
   minilines->first->des = canvasFontDes;

   text_ptr->pen = penPat;
   text_ptr->fill = objFill;
   text_ptr->cached_bitmap = None;
   text_ptr->cached_zoom = 0;
   text_ptr->lines = 1;

   curStrBlock = minilines->first->first_block;
   textCurIndex = 0;
   ResetOnCursorKey(FALSE);

   text_ptr->read_only = FALSE;
   text_ptr->orig_w = text_ptr->orig_h = 0;

   text_ptr->underline_y_offset = curUnderlineYOffset;
   text_ptr->overline_y_offset = curOverlineYOffset;
   text_ptr->min_lbearing = 0;
   text_ptr->max_rextra = 0;
   text_ptr->baseline_y = textAbsBaselineY;

   curTextObj = (struct ObjRec *)malloc(sizeof(struct ObjRec));
   if (curTextObj == NULL) FailAllocMessage();
   memset(curTextObj, 0, sizeof(struct ObjRec));
   curTextObj->x = textAbsX;
   curTextObj->y = textAbsY;
   curTextObj->type = OBJ_TEXT;
   curTextObj->id = objId++;
   curTextObj->dirty = FALSE;
   curTextObj->rotation = 0;
   curTextObj->locked = FALSE;
   /* the color specifies the fill pattern color */
   curTextObj->color = colorIndex;
   if (mainDisplay != NULL) {
      UtilStrCpyN(curTextObj->color_str, sizeof(curTextObj->color_str),
            colorMenuItems[colorIndex]);
   }
   curTextObj->detail.t = text_ptr;
   curTextObj->fattr = curTextObj->lattr = NULL;
   curTextObj->ctm = NULL;
   curTextObj->invisible = FALSE;
   curTextObj->trans_pat = transPat;
   AddObj(NULL, topObj, curTextObj);

   SetEditTextArea(0, textCursorH, 0, 0);

   textCursorShown = TRUE;
   SetTextHighlight();
   UpdatePinnedMenu(MENU_EDIT);

   curTextIsNew = TRUE;

   if (editTextSize != 0) {
      RestoreEditTextSize(curTextObj, FALSE);
   }
   AdjTextVerticalShift(TRUE);

   if (gnInputMethod != TGIM_NONE) {
      if (!tgIMHandleNewCurText(mainDisplay, drawWindow)) {
      }
   }
}

void FreeTextObj(ObjPtr)
   struct ObjRec *ObjPtr;
{
   if (ObjPtr->detail.t != NULL) {
      FreeMiniLines(&ObjPtr->detail.t->minilines, FALSE);
      if (ObjPtr->detail.t->cached_bitmap != None) {
         XFreePixmap(mainDisplay, ObjPtr->detail.t->cached_bitmap);
      }
      if (ObjPtr->detail.t->cached_pixmap != None) {
         XFreePixmap(mainDisplay, ObjPtr->detail.t->cached_pixmap);
      }
      if (ObjPtr->detail.t->cached_bg_bitmap != None) {
         XFreePixmap(mainDisplay, ObjPtr->detail.t->cached_bg_bitmap);
      }
      free(ObjPtr->detail.t);
   }
   free(ObjPtr);
}

static
void ShowTextRelatedInfo()
{
   ShowJust();
   ShowPen();
   ShowFill();
   ShowTransPatMode();
   ShowColor(FALSE);
   ShowTextVSpace();
   ShowCurFont();
   ShowTextSize();
}

void SetEditTextSizeValue(new_edit_text_size)
   int new_edit_text_size;
{
   editTextSize = new_edit_text_size;
   curRealSzUnit = INVALID;
}

void UpdateTextInfoChoices(forced)
   int forced;
{
   int saved_font=curFont, saved_style=curStyle, saved_sz_unit=curSzUnit;
   int saved_just=textJust, saved_color=colorIndex;
   int saved_underline_on=curUnderlineOn;
   int saved_overline_on=curOverlineOn;
   StrSegInfo *pStrSeg=curStrBlock->seg;
   MiniLinesInfo *minilines=curStrBlock->owner_mini_line->owner_minilines;

   if (!forced && pStrSeg->color == colorIndex &&
         /* pStrSeg->underline_on == curUnderlineOn && */
         minilines->just == textJust &&
         minilines->v_space == textVSpace &&
         pStrSeg->font == curFont &&
         pStrSeg->style == curStyle &&
         pStrSeg->sz_unit == curSzUnit &&
         (editTextSize == 0 || pStrSeg->real_sz_unit == curRealSzUnit) &&
         pStrSeg->underline_on == curUnderlineOn &&
         pStrSeg->overline_on == curOverlineOn) {
      return;
   }
   if (pStrSeg->color != colorIndex) {
      colorIndex = pStrSeg->color;
      ShowColor(FALSE);
   }
   if (minilines->just != textJust) {
      textJust = minilines->just;
      ShowJust();
   }
   if (minilines->v_space != textVSpace) {
      textVSpace = minilines->v_space;
      ShowTextVSpace();
   }
   if (editTextSize != 0) {
      saved_sz_unit = curRealSzUnit;

      if (forced || curRealSzUnit == INVALID ||
            pStrSeg->font != curFont || pStrSeg->style != curStyle ||
            pStrSeg->real_sz_unit != curRealSzUnit ||
            pStrSeg->sz_unit != curSzUnit ||
            pStrSeg->underline_on != curUnderlineOn ||
            pStrSeg->overline_on != curOverlineOn) {
         if (!TrySetCanvasFont(pStrSeg->font, pStrSeg->style, editTextSize,
               TRUE, NULL)) {
            SetEditTextSizeValue(0);
         } else {
            if (pStrSeg->font != curFont) curFont = pStrSeg->font;
            if (pStrSeg->style != curStyle) curStyle = pStrSeg->style;
            if (pStrSeg->real_sz_unit != curRealSzUnit) {
               curRealSzUnit = pStrSeg->real_sz_unit;
            }
            if (pStrSeg->underline_on != curUnderlineOn) {
               curUnderlineOn = pStrSeg->underline_on;
            }
            if (pStrSeg->overline_on != curOverlineOn) {
               curOverlineOn = pStrSeg->overline_on;
            }
            curSzUnit = FontSizeToSzUnit(editTextSize);
            SetCanvasFont();
            ShowTextSize();
            ShowCurFont();
            if (!textHighlight) CurFontMsg(FALSE, TRUE, pStrSeg);
         }
      }
   }
   if (editTextSize == 0) {
      if (forced || pStrSeg->font != curFont || pStrSeg->style != curStyle ||
            pStrSeg->sz_unit != curSzUnit ||
            pStrSeg->underline_on != curUnderlineOn ||
            pStrSeg->overline_on != curOverlineOn) {
         if (pStrSeg->font != curFont) curFont = pStrSeg->font;
         if (pStrSeg->style != curStyle) curStyle = pStrSeg->style;
         if (pStrSeg->sz_unit != curSzUnit) curSzUnit = pStrSeg->sz_unit;
         if (pStrSeg->underline_on != curUnderlineOn) {
            curUnderlineOn = pStrSeg->underline_on;
         }
         if (pStrSeg->overline_on != curOverlineOn) {
            curOverlineOn = pStrSeg->overline_on;
         }
         SetCanvasFont();
         ShowTextSize();
         ShowCurFont();
         if (!textHighlight) CurFontMsg(FALSE, TRUE, NULL);
      }
   }
   if (pStrSeg->underline_on != curUnderlineOn) {
      curUnderlineOn = pStrSeg->underline_on;
   }
   if (pStrSeg->overline_on != curOverlineOn) {
      curOverlineOn = pStrSeg->overline_on;
   }
   if (saved_style != curStyle || saved_just != textJust ||
         saved_underline_on != curUnderlineOn ||
         saved_overline_on != curOverlineOn) {
      UpdatePinnedMenu(MENU_STYLE);
   }
   if (saved_font != curFont) UpdatePinnedMenu(MENU_FONT);
   if (editTextSize != 0) {
      if (saved_sz_unit != curRealSzUnit) {
         UpdatePinnedMenu(MENU_SIZE);
      }
   } else {
      if (saved_sz_unit != curSzUnit) {
         UpdatePinnedMenu(MENU_SIZE);
      }
   }
   if (saved_color != colorIndex) UpdatePinnedMenu(MENU_COLOR);
}

static
void RestoreStrSegFromUsingEditTextSize(pStrSeg, pUserData)
   StrSegInfo *pStrSeg;
   void *pUserData; /* NULL */
{
   pStrSeg->sz_unit = pStrSeg->real_sz_unit;
}

static
void SaveStrSegToUseEditTextSize(pStrSeg, pUserData)
   StrSegInfo *pStrSeg;
   void *pUserData; /* NULL */
{  
   pStrSeg->real_sz_unit = pStrSeg->sz_unit;
   pStrSeg->sz_unit = FontSizeToSzUnit(editTextSize);
}

int RestoreEditTextSize(obj_ptr, restore)
   struct ObjRec *obj_ptr;
   int restore;
   /* returns TRUE if something is changed */
{
   if (editTextSize != 0) {
      MiniLinesInfo *minilines=(&obj_ptr->detail.t->minilines);

      if (restore) {
         DoFuncOnStrSegForMiniLines(minilines,
               RestoreStrSegFromUsingEditTextSize, NULL);
      } else {
         DoFuncOnStrSegForMiniLines(minilines, SaveStrSegToUseEditTextSize,
               NULL);
      }
      return TRUE;
   }
   return FALSE;
}

int ShouldRightMarginBeActive()
{
   if (curChoice != DRAWTEXT) return TRUE;
   if (!textCursorShown) return TRUE;

   if (curTextObj->ctm != NULL) return FALSE;
   if (textJust != JUST_L) return FALSE;
   if (curStrBlock->owner_mini_line->owner_minilines->owner_block != NULL) {
      return FALSE;
   }
   if (zoomScale != 0 || zoomedIn) return FALSE;
   if (editTextSize != 0) return FALSE;
   if (curStrBlock->next != NULL) return FALSE;
   if (curStrBlock->type != SB_SIMPLE) return FALSE;
   if (textCurIndex != curStrBlock->seg->dyn_str.sz-1) return FALSE;

   return TRUE;
}

void SetRightMarginActive()
{
   if (rightMarginEnabled == TRUE) {
      int saved_right_margin_active=rightMarginActive;

      rightMarginActive = ShouldRightMarginBeActive();
      if (rightMarginActive != saved_right_margin_active) {
         RedrawHRulerWindow();
      }
   } else {
      rightMarginActive = TRUE;
   }
}

static int gnDontRedrawDuringPaste=FALSE;
static int gnPastingLineNum=0;

static
void EndChangeCurText(clean_bbox)
   int clean_bbox;
{
   curTextModified = TRUE;
   if (!gnDontRedrawDuringPaste) {
      /*
       * If clean_bbox is TRUE, AdjTextVerticalShift() will adjust
       *       curStrBlock->bbox to be the new size.  After
       *       UpdateTextInfoChoices() is called, curStrBlock->clean_bbox
       *       will also be updated to the new size.
       */
      AdjTextVerticalShift(clean_bbox);

      RedrawCurText();
      UpdateTextInfoChoices(FALSE);
   }
   InvalidateTextCache(curTextObj->detail.t);
}

static
int NumLinesInCurText()
{
   MiniLinesInfo *minilines=(&curTextObj->detail.t->minilines);
   MiniLineInfo *pMiniLine=NULL;
   int count=0;

   for (pMiniLine=minilines->first; pMiniLine != NULL;
         pMiniLine=pMiniLine->next, count++) {
   }
   return count;
}

static
void CreateNoTextObj()
{
   struct TextRec *text_ptr=curTextObj->detail.t;
   struct AttrRec *attr_ptr=text_ptr->attr;
   int ltx=0, lty=0, rbx=0, rby=0, scr_ltx=0, scr_lty;

   if (attr_ptr != NULL) {
      /* the text being edited is an attribute */
      if (attr_ptr->nameshown) {
         UnlinkAttr(attr_ptr);
         FreeTextObj(curTextObj);
         FreeAttr(attr_ptr);
      } else {
         text_ptr->lines = 1;
         UpdateAttr(text_ptr, attr_ptr);
      }
      AdjObjBBox(attr_ptr->owner);
      if (outerSel != NULL) AdjAncestorsBBox();
      if (curTextModified) {
         if (outerSel != NULL) {
            RecordReplaceAnObj(outerSel->obj);
         } else {
            RecordReplaceAnObj(attr_ptr->owner);
         }
      } else {
         AbortPrepareCmd(CMD_REPLACE);
      }
   } else {
      if (outerSel != NULL) {
         UnlinkCurTextFromInnerSel();
         AdjAncestorsBBox();
      }
      if (!curTextIsNew) {
         if (outerSel != NULL) {
            RecordReplaceAnObj(outerSel->obj);
         } else {
            ChangeReplaceOneCmdToDeleteCmd();
         }
      } else {
         AbortPrepareCmd(CMD_REPLACE);
      }
      if (curTextObj != NULL) {
         if (outerSel != NULL) {
            /* curTextObj already broken off from the main */
            /*         stream of objects, so just free it. */
            FreeObj(curTextObj);
         } else {
            DelObj(curTextObj);
         }
      }
   }
   switch (textJust) {
   case JUST_L:
      scr_ltx = OFFSET_X(textAbsX-2);
      if (zoomedIn) {
         ltx = ABS_X(textOrigX-2)-2;
         rbx = ABS_X(textOrigX+textW+2)+2;
      } else {
         ltx = ABS_X(textOrigX-4);
         rbx = ABS_X(textOrigX+textW+4);
      }
      break;
   case JUST_C:
      scr_ltx = OFFSET_X(textAbsX)-textW/2-2;
      if (zoomedIn) {
         ltx = ABS_X(textOrigX-(textW>>1)-2)-2;
         rbx = ABS_X(textOrigX+(textW>>1)+2)+2;
      } else {
         ltx = ABS_X(textOrigX-(textW>>1)-4);
         rbx = ABS_X(textOrigX+(textW>>1)+4);
      }
      break;
   case JUST_R:
      scr_ltx = OFFSET_X(textAbsX)-textW-2;
      if (zoomedIn) {
         ltx = ABS_X(textOrigX-textW-2)-2;
         rbx = ABS_X(textOrigX+textW+2)+2;
      } else {
         ltx = ABS_X(textOrigX-textW-4);
         rbx = ABS_X(textOrigX+textW+4);
      }
      break;
   }
   scr_lty = OFFSET_Y(textAbsY)-2;
   if (zoomedIn) {
      lty = ABS_Y(textOrigY-2)-2;
      rby = ABS_Y(textOrigY+textH+2)+2;
   } else {
      lty = ABS_Y(textOrigY-4);
      rby = ABS_Y(textOrigY+textH+4);
   }
   ltx -= (curTextOutlineHalfW<<1);
   lty -= (curTextOutlineHalfW<<1);
   rbx += (curTextOutlineHalfW<<2);
   rby += (curTextOutlineHalfW<<2);
   if (editingText) {
      XClearArea(mainDisplay, drawWindow,
            scr_ltx-(curTextOutlineHalfW<<1), scr_lty-(curTextOutlineHalfW<<1),
            textW+5+(curTextOutlineHalfW<<2), textH+5+(curTextOutlineHalfW<<2),
            FALSE);
      RedrawAreas(botObj, beforeEditTextInfo.bbox.ltx-GRID_ABS_SIZE(2),
            beforeEditTextInfo.bbox.lty-GRID_ABS_SIZE(2),
            beforeEditTextInfo.bbox.rbx+GRID_ABS_SIZE(2),
            beforeEditTextInfo.bbox.rby+GRID_ABS_SIZE(2),
            ltx, lty, rbx, rby);
   } else {
      RedrawAnArea(botObj, ltx, lty, rbx, rby);
   }
   firstMiniLine = lastMiniLine = NULL;
   textCursorShown = FALSE;
   curTextObj = NULL;
   textCurIndex = 0;

   SetEditTextArea(0, textCursorH, 0, 0);

   if (editingText) {
      XClearArea(mainDisplay, drawWindow,
            scr_ltx-(curTextOutlineHalfW<<1), scr_lty-(curTextOutlineHalfW<<1),
            textW+5+(curTextOutlineHalfW<<2), textH+5+(curTextOutlineHalfW<<2),
            FALSE);
      RedrawAreas(botObj, beforeEditTextInfo.bbox.ltx-GRID_ABS_SIZE(2),
            beforeEditTextInfo.bbox.lty-GRID_ABS_SIZE(2),
            beforeEditTextInfo.bbox.rbx+GRID_ABS_SIZE(2),
            beforeEditTextInfo.bbox.rby+GRID_ABS_SIZE(2),
            ltx, lty, rbx, rby);
   } else {
      RedrawAnArea(botObj, ltx, lty, rbx, rby);
   }
   firstMiniLine = lastMiniLine = NULL;
   textCursorShown = FALSE;
   curTextObj = NULL;
   textCurIndex = 0;

   SetEditTextArea(0, textCursorH, 0, 0);

   PopCurFont();
   ShowTextRelatedInfo();
   if (editingText) {
      editingText = FALSE;
   }
   textDrawn = FALSE;
   justDrawnTextObj = NULL;
   textHighlight = FALSE;
   curTextIsNew = FALSE;

   curStrBlock = endStrBlock = NULL;
   textCurIndex = textEndIndex = INVALID;
   textHighlight = FALSE;
}

static
void RedrawAreasInCreateText(ltx1, lty1, rbx1, rby1, ltx2, lty2, rbx2, rby2)
   int ltx1, lty1, rbx1, rby1, ltx2, lty2, rbx2, rby2;
{
   struct BBRec bbox1, bbox2;

   SetBBRec(&bbox1, ltx1, lty1, rbx1, rby1);
   SetBBRec(&bbox2, ltx2, lty2, rbx2, rby2);
   if (curTextObj != NULL) {
      UnionRect(&bbox1, &curTextObj->bbox, &bbox1);
   }
   InflateBBox(&bbox2, -GRID_ABS_SIZE(2), -GRID_ABS_SIZE(2), &bbox2);
   if (!BBoxIntersect(bbox1, bbox2)) {
      RedrawAnArea(botObj, ltx2-GRID_ABS_SIZE(2), lty2-GRID_ABS_SIZE(2),
            rbx2+GRID_ABS_SIZE(2), rby2+GRID_ABS_SIZE(2));
   }
   RedrawAreas(botObj, bbox1.ltx-GRID_ABS_SIZE(2), bbox1.lty-GRID_ABS_SIZE(2),
         bbox1.rbx+GRID_ABS_SIZE(2), bbox1.rby+GRID_ABS_SIZE(2),
         ltx2-GRID_ABS_SIZE(2), lty2-GRID_ABS_SIZE(2), rbx2+GRID_ABS_SIZE(2),
         rby2+GRID_ABS_SIZE(2));
}

int CreateTextObj(nDeactivateIM, nRedraw)
   int nDeactivateIM, nRedraw;
   /* returns TRUE if something got created */
   /* returns FALSE otherwise */
{
   struct TextRec *text_ptr=NULL;
   struct AttrRec *attr_ptr=NULL;
   MiniLinesInfo *minilines=NULL;
   int num_lines=0, ltx=0, lty=0, rbx=0, rby=0, scr_ltx=0, scr_lty=0;
   int saved_font=curFont, saved_style=curStyle, saved_sz_unit=curSzUnit;
   int saved_just=textJust, saved_color=colorIndex;
   int saved_underline_on=curUnderlineOn;
   int saved_overline_on=curOverlineOn;

   if (nDeactivateIM) {
      if (canvasFontDoubleByte &&
            tgIMDeactiveOnCreateText(mainDisplay, drawWindow)) {
         if (tgIMHandleCreateText(mainDisplay, drawWindow)) {
         }
      }
   }
   if (!textCursorShown) return FALSE;

   if (gstWBInfo.do_whiteboard) {
      gstWBInfo.BlockRemoteCmdDepth--;
   }
   text_ptr = curTextObj->detail.t;
   minilines = (&text_ptr->minilines);
   num_lines = NumLinesInCurText();

   if (editTextSize != 0) {
      RestoreEditTextSize(curTextObj, TRUE);
      RecalcTextMetrics(text_ptr, curTextObj->x, text_ptr->baseline_y);
      UpdateEditTextArea(minilines->w, minilines->h, minilines->min_lbearing,
            minilines->max_rextra);
   }
   if (curTextModified) {
      int retry=TRUE, shrunk=FALSE;

      while (retry) {
         retry = FALSE;
         if (ShrinkMiniLines(&curTextObj->detail.t->minilines, &retry)) {
            shrunk = TRUE;
         }
      }
      if (shrunk) {
         curStrBlock = endStrBlock = NULL;
         textCurIndex = textEndIndex = INVALID;
         textHighlight = FALSE;
         UpdTextBBox(curTextObj);
      }
   }
   if (minilines->first == minilines->last && BlankMiniLine(minilines->first)) {
      /* no text entered or all text erased */
      CreateNoTextObj();
      UpdatePinnedMenu(MENU_EDIT);

      if (saved_style != curStyle || saved_just != textJust ||
            saved_underline_on != curUnderlineOn ||
            saved_overline_on != curOverlineOn) {
         UpdatePinnedMenu(MENU_STYLE);
      }
      if (saved_font != curFont) UpdatePinnedMenu(MENU_FONT);
      if (saved_sz_unit != curSzUnit) UpdatePinnedMenu(MENU_SIZE);
      if (saved_color != colorIndex) UpdatePinnedMenu(MENU_COLOR);

      if (gstWBInfo.do_whiteboard) {
         TryHandleWBInputData();
         SetCurChoice(DRAWTEXT);
      }
      curTextMovedAbsX = curTextMovedAbsY = 0;
      ClearCopyUTF8Info();

      return FALSE;
   }
   if (curTextModified) {
      InvalidateTextCache(text_ptr);
   }
   text_ptr->lines = num_lines;

   if (curTextObj->ctm == NULL) {
      curTextObj->x = textAbsX - tmpAdjAbsX - curTextMovedAbsX;
      text_ptr->baseline_y = textAbsBaselineY - tmpAdjAbsY - curTextMovedAbsY;
      curTextObj->y = text_ptr->baseline_y - minilines->first->asc;
   } else {
      int dx=0, dy=0;

      if (beforeEditTextInfo.first_mini_line_asc != minilines->first->asc) {
         int anchor_x=0, anchor_y=0, x=0, y=0;

         TransformPointThroughCTM(0, beforeEditTextInfo.first_mini_line_asc,
               curTextObj->ctm, &x, &y);
         anchor_x = curTextObj->x + x;
         anchor_y = curTextObj->y + y;
         TransformPointThroughCTM(0, minilines->first->asc,
               curTextObj->ctm, &x, &y);
         dx = anchor_x - (curTextObj->x + x);
         dy = anchor_y - (curTextObj->y + y);
      }
      curTextObj->x = textAbsX - tmpAdjAbsX - curTextMovedAbsX;
      curTextObj->y = textAbsY - tmpAdjAbsY - curTextMovedAbsY;

      curTextObj->x += dx;
      curTextObj->y += dy;

      curTextObj->x -= curTextObj->ctm->t[CTM_TX];
      curTextObj->y -= curTextObj->ctm->t[CTM_TY];
   }
   RecalcTextMetrics(text_ptr, curTextObj->x, text_ptr->baseline_y);

   textJust = minilines->just;

   SetTextOrigBBoxes(curTextObj, textJust, text_ptr->w, text_ptr->h,
         text_ptr->min_lbearing, text_ptr->max_rextra, ROTATE0);
   SetTextBBox(curTextObj, textJust, text_ptr->w, text_ptr->h,
         text_ptr->min_lbearing, text_ptr->max_rextra, ROTATE0);
   if (curTextObj->ctm != NULL) {
      GetTransformedOBBoxOffsetVs(curTextObj, curTextObj->rotated_obbox);
   }
   switch (textJust) {
   case JUST_L:
      scr_ltx = OFFSET_X(textAbsX-2);
      if (zoomedIn) {
         ltx = textAbsX-2-GRID_ABS_SIZE(2);
         rbx = textAbsX+textW+2+GRID_ABS_SIZE(2);
      } else {
         ltx = textAbsX-ABS_SIZE(2)-GRID_ABS_SIZE(2);
         rbx = textAbsX+ABS_SIZE(textW+2)+GRID_ABS_SIZE(2);
      }
      break;
   case JUST_C:
      scr_ltx = OFFSET_X(textAbsX)-textW/2-2;
      if (zoomedIn) {
         ltx = textAbsX-textW/2-2-GRID_ABS_SIZE(2);
         rbx = textAbsX+textW/2+2+GRID_ABS_SIZE(2);
      } else {
         ltx = textAbsX-ABS_SIZE(textW/2+2)-GRID_ABS_SIZE(2);
         rbx = textAbsX+ABS_SIZE(textW/2+2)+GRID_ABS_SIZE(2);
      }
      break;
   case JUST_R:
      scr_ltx = OFFSET_X(textAbsX)-textW-2;
      if (zoomedIn) {
         ltx = textAbsX-textW-2-GRID_ABS_SIZE(2);
         rbx = textAbsX+2+GRID_ABS_SIZE(2);
      } else {
         ltx = textAbsX-ABS_SIZE(textW+2)-GRID_ABS_SIZE(2);
         rbx = textAbsX+ABS_SIZE(2)+GRID_ABS_SIZE(2);
      }
      break;
   }
   scr_lty = OFFSET_Y(textAbsY);
   if (zoomedIn) {
      lty = textAbsY-2-GRID_ABS_SIZE(2);
      rby = textAbsY+textH+2+GRID_ABS_SIZE(2);
   } else {
      lty = textAbsY-ABS_SIZE(2)-GRID_ABS_SIZE(2);
      rby = textAbsY+ABS_SIZE(textH+2)+GRID_ABS_SIZE(2);
   }

   if ((attr_ptr=text_ptr->attr) != NULL) {
      UpdateAttr(text_ptr, attr_ptr);
      textDrawn = FALSE;
      justDrawnTextObj = NULL;
      if (curTextModified && AutoCenterAttr(attr_ptr->owner)) {
         struct BBRec bbox;

         CenterObjInOBBox(attr_ptr->obj, attr_ptr->owner->obbox, &bbox);
         if (bbox.ltx < ltx) ltx = bbox.ltx;
         if (bbox.lty < lty) lty = bbox.lty;
         if (bbox.rbx > rbx) rbx = bbox.rbx;
         if (bbox.rby > rby) rby = bbox.rby;
      }
      AdjObjBBox(attr_ptr->owner);
      if (outerSel != NULL) AdjAncestorsBBox();

      if (curTextModified) {
         if (outerSel != NULL) {
            RecordReplaceAnObj(outerSel->obj);
         } else {
            RecordReplaceAnObj(attr_ptr->owner);
         }
      } else {
         AbortPrepareCmd(CMD_REPLACE);
      }
   } else {
      if (outerSel != NULL) {
         textDrawn = FALSE;
         justDrawnTextObj = NULL;
         AdjAncestorsBBox();
      } else {
         textDrawn = TRUE;
         justDrawnTextObj = curTextObj;
      }
      if (curTextIsNew) {
         AbortPrepareCmd(CMD_REPLACE);
         RecordNewObjCmd();
      } else if (curTextModified) {
         if (outerSel != NULL) {
            RecordReplaceAnObj(outerSel->obj);
         } else {
            RecordReplaceAnObj(curTextObj);
         }
      } else {
         AbortPrepareCmd(CMD_REPLACE);
      }
   }
   textCursorShown = FALSE;
   textCurIndex = 0;

   GetCurTextBBoxes(&curTextOBBox, NULL);
   SetEditTextArea(curTextOBBox.rbx-curTextOBBox.ltx,
         curTextOBBox.rby-curTextOBBox.lty,
         text_ptr->minilines.min_lbearing, text_ptr->minilines.max_rextra);

   if (editingText) {
      int x=0;

      x = curTextBBox.ltx;
      XClearArea(mainDisplay, drawWindow,
            x-2-(curTextOutlineHalfW<<1),
            curTextBBox.lty-2-(curTextOutlineHalfW<<1),
            curTextBBox.rbx-x+5+(curTextOutlineHalfW<<2),
            curTextBBox.rby-curTextBBox.lty+5+(curTextOutlineHalfW<<2), FALSE);

      if (ABS_X(x) < ltx) ltx = ABS_X(x);
      if (ABS_Y(textOrigY) < lty) lty = ABS_Y(textOrigY);
      x += textW-textAbsMinLBearing+textAbsMaxRExtra+1;
      if (ABS_X(x) > rbx) rbx = ABS_X(x);
      if (ABS_Y(textOrigY+textH+1) > rby) rby = ABS_Y(textOrigY+textH+1);
      if (curTextObj->ctm != NULL) {
         int i;

         for (i=0; i < 4; i++) {
            int tmp_x=ABS_X(curTextObj->rotated_obbox[i].x);
            int tmp_y=ABS_Y(curTextObj->rotated_obbox[i].y);

            if (tmp_x < ltx) ltx = tmp_x;
            if (tmp_y < lty) lty = tmp_y;
            if (tmp_x > rbx) rbx = tmp_x;
            if (tmp_y > rby) rby = tmp_y;
         }
      }
      if (nRedraw) {
         ltx -= (curTextOutlineHalfW<<1);
         lty -= (curTextOutlineHalfW<<1);
         rbx += (curTextOutlineHalfW<<1);
         rby += (curTextOutlineHalfW<<1);
         RedrawAreasInCreateText(beforeEditTextInfo.bbox.ltx,
               beforeEditTextInfo.bbox.lty, beforeEditTextInfo.bbox.rbx,
               beforeEditTextInfo.bbox.rby, ltx, lty, rbx, rby);
      }
   } else {
      int x=0;

      switch (textJust) {
      case JUST_L: x = textOrigX+textAbsMinLBearing; break;
      case JUST_C: x = textOrigX-textW/2+textAbsMinLBearing; break;
      case JUST_R: x = textOrigX-textW+textAbsMinLBearing; break;
      }
      if (nRedraw) {
         ltx -= (curTextOutlineHalfW<<1);
         lty -= (curTextOutlineHalfW<<1);
         rbx += (curTextOutlineHalfW<<1);
         rby += (curTextOutlineHalfW<<1);
         RedrawAreasInCreateText(ltx, lty, rbx, rby, ABS_X(x-2),
               ABS_Y(textOrigY-2),
               ABS_X(x+textW-textAbsMinLBearing+textAbsMaxRExtra+3),
               ABS_Y(textOrigY+textH+3));
      }
   }
   textOrigX = textOrigY = textCurX = textCurY = ABS_SIZE(20);

   SetEditTextArea(0, textCursorH, 0, 0);

   textAbsX = textOrigX + drawOrigX;
   textAbsY = textOrigY + drawOrigY;
   curTextObj = NULL;

   PopCurFont();
   ShowTextRelatedInfo();
   if (editingText) {
      editingText = FALSE;
   } else if (textRotation != 0) {
      int saved_rotation_increment=rotationIncrement;
      int saved_ltx=topObj->bbox.ltx, saved_lty=topObj->bbox.lty;
      int saved_rbx=topObj->bbox.rbx, saved_rby=topObj->bbox.rby;

      SetRotatePivotByObject(topObj);
      rotationIncrement = textRotation;
      RotateObjClockWise(topObj);
      rotationIncrement = saved_rotation_increment;
      if (nRedraw) {
         RedrawAreasInCreateText(saved_ltx, saved_lty, saved_rbx, saved_rby,
               topObj->bbox.ltx, topObj->bbox.lty, topObj->bbox.rbx,
               topObj->bbox.rby);
      }
   }
   curTextIsNew = FALSE;
   SetTextHighlight();
   UpdatePinnedMenu(MENU_EDIT);

   curStrBlock = endStrBlock = NULL;
   textCurIndex = textEndIndex = INVALID;
   textHighlight = FALSE;

   if (saved_style != curStyle || saved_just != textJust ||
         saved_underline_on != curUnderlineOn ||
         saved_overline_on != curOverlineOn) {
      UpdatePinnedMenu(MENU_STYLE);
   }
   if (saved_font != curFont) UpdatePinnedMenu(MENU_FONT);
   if (saved_sz_unit != curSzUnit) UpdatePinnedMenu(MENU_SIZE);
   if (saved_color != colorIndex) UpdatePinnedMenu(MENU_COLOR);

   if (gstWBInfo.do_whiteboard) {
      int saved_text_drawn=textDrawn;

      textDrawn = FALSE;
      TryHandleWBInputData();
      if (justDrawnTextObj != NULL && !IsTopLevelObject(justDrawnTextObj)) {
         justDrawnTextObj = NULL;
         textDrawn = FALSE;
      } else {
         textDrawn = saved_text_drawn;
      }
      SetCurChoice(DRAWTEXT);
   }
   curTextMovedAbsX = curTextMovedAbsY = 0;
   ClearCopyUTF8Info();

   if (rightMarginEnabled == TRUE) {
      if (!rightMarginActive) {
         rightMarginActive = TRUE;
         RedrawHRulerWindow();
      }
   }
   return TRUE;
}

static
int CanAdvanceRight(pStrBlock, nIndex)
   StrBlockInfo *pStrBlock;
   int nIndex;
{
   if (nIndex != pStrBlock->seg->dyn_str.sz-1) {
      return TRUE;
   }
   if (pStrBlock->type == SB_SUPSUB_CENTER) {
      return FALSE;
   }
   for (pStrBlock=pStrBlock->next; pStrBlock != NULL;
         pStrBlock=pStrBlock->next) {
      switch (pStrBlock->type) {
      case SB_SIMPLE:
         if (*pStrBlock->seg->dyn_str.s != '\0') {
            return TRUE;
         }
         break;
      case SB_CHAR_SPACE: return TRUE;

      case SB_SUPSUB_LEFT: return FALSE;
      case SB_SUPSUB_CENTER: return FALSE;
      case SB_SUPSUB_RIGHT: return FALSE;
      }
   }
   return FALSE;
}

static
int CanAdvanceLeft(pStrBlock, nIndex)
   StrBlockInfo *pStrBlock;
   int nIndex;
{
   if (nIndex != 0) {
      return TRUE;
   }
   if (pStrBlock->type == SB_SUPSUB_CENTER) {
      return FALSE;
   }
   for (pStrBlock=pStrBlock->prev; pStrBlock != NULL;
         pStrBlock=pStrBlock->prev) {
      switch (pStrBlock->type) {
      case SB_SIMPLE:
         if (*pStrBlock->seg->dyn_str.s != '\0') {
            return TRUE;
         }
         break;
      case SB_CHAR_SPACE: return TRUE;

      case SB_SUPSUB_LEFT: return FALSE;
      case SB_SUPSUB_CENTER: return FALSE;
      case SB_SUPSUB_RIGHT: return FALSE;
      }
   }
   return FALSE;
}

int AtBeginningOfInheritedAttrValue(obj_ptr, pStrBlock, nIndex)
   struct ObjRec *obj_ptr;
   StrBlockInfo *pStrBlock;
   int nIndex;
{
   if (obj_ptr->type == OBJ_TEXT) {
      struct TextRec *text_ptr=obj_ptr->detail.t;
      struct AttrRec *attr_ptr=text_ptr->attr;

      if (attr_ptr != NULL && attr_ptr->inherited && attr_ptr->shown &&
            attr_ptr->nameshown) {
         if (pStrBlock != NULL && pStrBlock->prev == NULL &&
               pStrBlock->owner_mini_line == text_ptr->minilines.first) {
            if (nIndex <= attr_ptr->attr_name.sz-1) {
               return TRUE;
            }
         }
      }
   }
   return FALSE;
}

void HighLightJustDrawnText()
{
   if (justDrawnTextObj == NULL) return;

   AddNewSelObj(justDrawnTextObj);
   UpdSelBBox();
   HighLightAnObj(justDrawnTextObj);
   justDupped = FALSE;
}

void DelSelText()
{
   if (textHighlight) {
      DeleteHighlightedText();
      EndChangeCurText(FALSE);
   }
}

static
int PrepareEditExistingText(obj_ptr, abs_x, abs_y, x_off, y_off)
   struct ObjRec *obj_ptr;
   int abs_x, abs_y, *x_off, *y_off;
{
   struct TextRec *text_ptr=obj_ptr->detail.t;
   int orig_text_w=0, orig_text_h=0;

   if (text_ptr->read_only) {
      MsgBox(TgLoadString(STID_READ_ONLY_TEXT_CANT_BE_EDITED), TOOL_NAME,
            INFO_MB);
      return FALSE;
   }
   curTextObj = obj_ptr;
   memset(&beforeEditTextInfo, 0, sizeof(struct tagBeforeEditTextInfo));
   memcpy(&beforeEditTextInfo.bbox, &obj_ptr->bbox, sizeof(struct BBRec));
   memcpy(&beforeEditTextInfo.obbox, &obj_ptr->obbox, sizeof(struct BBRec));
   beforeEditTextInfo.x = obj_ptr->x;
   beforeEditTextInfo.baseline_y = text_ptr->baseline_y;
   beforeEditTextInfo.first_mini_line_asc = text_ptr->minilines.first->asc;
   beforeEditTextInfo.obbox_w = orig_text_w =
         obj_ptr->obbox.rbx - obj_ptr->obbox.ltx;
   beforeEditTextInfo.obbox_h = orig_text_h =
         obj_ptr->obbox.rby - obj_ptr->obbox.lty;

   PushCurFont();
   editingText = TRUE;

   ObjFontInfoToCurFontInfo(text_ptr);
   penPat = text_ptr->pen;
   objFill = text_ptr->fill;
   transPat = obj_ptr->trans_pat;

   SetEditTextArea(0, 0, 0, 0);

   colorIndex = obj_ptr->color;
   ShowTextRelatedInfo();
   ResetOnCursorKey(FALSE);

   textAbsX = obj_ptr->x;
   textAbsY = obj_ptr->y;
   textAbsBaselineY = text_ptr->baseline_y;
   if (obj_ptr->ctm != NULL) {
      textAbsX += obj_ptr->ctm->t[CTM_TX];
      textAbsY += obj_ptr->ctm->t[CTM_TY];
      textAbsBaselineY += obj_ptr->ctm->t[CTM_TY];
   }
   textOrigX = OFFSET_X(textAbsX);
   textOrigY = OFFSET_Y(textAbsY);
   textOrigBaselineY = textOrigY + text_ptr->baseline_y - obj_ptr->y;

   if (editTextSize != 0) {
      if (RestoreEditTextSize(obj_ptr, FALSE)) {
         UpdTextBBox(obj_ptr);
      }
   }
   if (obj_ptr->ctm == NULL) {
      SetEditTextArea(obj_ptr->obbox.rbx-obj_ptr->obbox.ltx,
            obj_ptr->obbox.rby-obj_ptr->obbox.lty,
            text_ptr->min_lbearing, text_ptr->max_rextra);
      switch (textJust) {
      case JUST_L: tmpAdjAbsX = ((orig_text_w-ABS_SIZE(textW))>>1); break;
      case JUST_C: tmpAdjAbsX = 0; break;
      case JUST_R: tmpAdjAbsX = ((ABS_SIZE(textW)-orig_text_w)>>1); break;
      }
      tmpAdjAbsY = ((orig_text_h-ABS_SIZE(textH))>>1);
   } else {
      int abs_w=0, abs_h=0;
      int orig_2_xc=(beforeEditTextInfo.obbox.ltx+beforeEditTextInfo.obbox.rbx);
      int orig_2_yc=(beforeEditTextInfo.obbox.lty+beforeEditTextInfo.obbox.rby);

      SetEditTextArea(obj_ptr->orig_obbox.rbx-obj_ptr->orig_obbox.ltx,
            obj_ptr->orig_obbox.rby-obj_ptr->orig_obbox.lty,
            text_ptr->min_lbearing, text_ptr->max_rextra);
      abs_w = ABS_SIZE(textW);
      abs_h = ABS_SIZE(textH);

      switch (textJust) {
      case JUST_L: tmpAdjAbsX = ((orig_2_xc-abs_w)>>1)-textAbsX; break;
      case JUST_C: tmpAdjAbsX = ((orig_2_xc)>>1) - textAbsX; break;
      case JUST_R: tmpAdjAbsX = ((orig_2_xc+abs_w)>>1)-textAbsX; break;
      }
      tmpAdjAbsY = ((orig_2_yc - abs_h)>>1) - textAbsY;
   }
   textAbsX += tmpAdjAbsX;
   textAbsY += tmpAdjAbsY;
   textAbsBaselineY += tmpAdjAbsY;
   textOrigX = OFFSET_X(textAbsX);
   textOrigY = OFFSET_Y(textAbsY);
   textOrigBaselineY = textOrigY + text_ptr->baseline_y - obj_ptr->y;

   if (obj_ptr->ctm == NULL) {
      abs_x += tmpAdjAbsX;
      abs_y += tmpAdjAbsY;
   } else {
      int x, y, dx=0;

      ReverseTransformPointThroughCTM(abs_x-obj_ptr->x, abs_y-obj_ptr->y,
            obj_ptr->ctm, &x, &y);
      abs_x = obj_ptr->x + x;
      abs_y = obj_ptr->y + y;

      switch (textJust) {
      case JUST_L: break;
      case JUST_C: dx = (textW>>1); break;
      case JUST_R: dx = textW; break;
      }
      abs_x += textAbsX - obj_ptr->orig_obbox.ltx - dx;
      abs_y += textAbsY - obj_ptr->orig_obbox.lty;
   }
   *x_off = OFFSET_X(abs_x);
   *y_off = OFFSET_Y(abs_y);

   if (editTextSize != 0) {
      if (RestoreEditTextSize(curTextObj, TRUE)) {
         UpdTextBBox(curTextObj);
      }
   }
   if (outerSel != NULL) {
      PrepareToReplaceAnObj(outerSel->obj);
   } else if (text_ptr->attr == NULL) {
      PrepareToReplaceAnObj(obj_ptr);
   } else {
      PrepareToReplaceAnObj(text_ptr->attr->owner);
   }
   if (editTextSize != 0) {
      if (RestoreEditTextSize(curTextObj, FALSE)) {
         UpdTextBBox(curTextObj);
      }
   }
   SetEditTextArea(textW, textH, text_ptr->minilines.min_lbearing,
         text_ptr->minilines.max_rextra);
   UpdateCurTextBBoxes(TRUE);

   if (gstWBInfo.do_whiteboard) {
      gstWBInfo.BlockRemoteCmdDepth++;
   }
   curTextMovedAbsX = curTextMovedAbsY = 0;

   return TRUE;
}

static
int BeginAdvance(drag, dir, ppStrBlock, pnCurIndex)
   int drag, *pnCurIndex, dir;
   StrBlockInfo **ppStrBlock;
{
   int position_end=FALSE;

   if (drag) {
      if (textHighlight) {
         *ppStrBlock = endStrBlock;
         *pnCurIndex = textEndIndex;
      } else {
         *ppStrBlock = curStrBlock;
         *pnCurIndex = textCurIndex;
      }
      position_end = TRUE;
   } else {
      if (textHighlight) {
         textHighlight = FALSE;
      }
      if (dir == ADVANCE_RIGHT && endStrBlock != NULL) {
         *ppStrBlock = endStrBlock;
         *pnCurIndex = textEndIndex;
         position_end = TRUE;
      } else {
         *ppStrBlock = curStrBlock;
         *pnCurIndex = textCurIndex;
      }
   }
   return position_end;
}

static
void EndAdvance(position_end, pStrBlock, nCurIndex)
   int position_end, nCurIndex;
   StrBlockInfo *pStrBlock;
{
   if (position_end) {
      endStrBlock = pStrBlock;
      textEndIndex = nCurIndex;
      SetTextEndXY();
   } else {
      curStrBlock = pStrBlock;
      textCurIndex = nCurIndex;
      SetTextCurXY();
   }
}

static
int PartOfAWord(double_byte, ch)
   int double_byte;
   char ch;
{
   if (double_byte) {
      return TRUE;
   }
   return (ch == '_' || (ch >= '0' && ch <= '9') ||
         (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z'));
}

#include "xbm/text_cur.xbm"

static
int GetCursorPositionInCurText(x_off, y_off, use_abs, pn_abs_dx,
      pn_abs_baseline_dy, ppStrBlock, pnIndex)
   int x_off, y_off, use_abs, *pn_abs_dx, *pn_abs_baseline_dy, *pnIndex;
   StrBlockInfo **ppStrBlock;
   /*
    * returns -1 if caller needs to figure out where the beginning is
    * returns 0 if *pn_abs_dx and *pn_abs_baseline_dy are set
    * returns 1 if caller needs to figure out where the end is
    */
{
   struct TextRec *text_ptr=curTextObj->detail.t;
   int x=0, y=0, orig_x=0, baseline_y=0, dx=0, dy=0, rc=TRUE;

   if (use_abs) {
      x = ABS_X(x_off);
      y = ABS_Y(y_off);
      orig_x = textAbsX;
      baseline_y = textAbsBaselineY;
   } else {
      x = x_off;
      y = y_off;
      orig_x = textOrigX;
      baseline_y = textOrigBaselineY;
   }
   if (y < baseline_y-text_ptr->minilines.first->des-(text_cur_height>>1)) {
      if (text_cur_bits == NULL) { } /* no compiler warning */
      return INVALID;
   }
   PushCurFont();
   rc = GetCursorPositionInMiniLines(&text_ptr->minilines, x-orig_x,
         y-baseline_y, &dx, &dy, ppStrBlock, pnIndex);
   PopCurFont();

   if (rc == 0) {
      *pn_abs_dx = dx;
      *pn_abs_baseline_dy = dy;
   }
   return rc;
}

static
void HandleClickOnCanvas(x_off, y_off)
   int x_off, y_off;
{
   if (SzUnitToFontSize(curSzUnit) != canvasFontSize) {
      sprintf(gszMsgBox, TgLoadString(STID_TEXT_SZ_NOT_AVAIL_TRY_DIFF),
            SzUnitToFontSize(curSzUnit));
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
   } else {
      int grid_x=0, grid_y=0;

      GridXY(x_off, y_off, &grid_x, &grid_y);

      textOrigX = grid_x;
      textOrigBaselineY = grid_y;
      textOrigY = textOrigBaselineY-canvasFontAsc;
      textAbsMinLBearing = 0;
      textAbsMaxRExtra = 0;
      textAbsX = ABS_X(grid_x);
      textAbsBaselineY = ABS_Y(grid_y);
      textAbsY = textAbsBaselineY-canvasFontAsc;
      tmpAdjAbsX = tmpAdjAbsY = 0;
      curTextMovedAbsX = curTextMovedAbsY = 0;
      NewCurText();
      RedrawCurText();
      UpdateTextInfoChoices(TRUE);
      if (editTextSize != 0 && curStrBlock != NULL &&
            (curStrBlock->type == SB_SIMPLE ||
            curStrBlock->type == SB_SUPSUB_CENTER)) {
         CurFontMsg(FALSE, TRUE, curStrBlock->seg);
      } else {
         CurFontMsg(FALSE, TRUE, NULL);
      }
      if (gstWBInfo.do_whiteboard) {
         gstWBInfo.BlockRemoteCmdDepth++;
      }
      PrepareToReplaceAnObj(curTextObj);
   }
}

static
void AdjTextIndicesForInheritedAttr()
{
   if (AtBeginningOfInheritedAttrValue(curTextObj, curStrBlock, textCurIndex)) {
      textCurIndex = curTextObj->detail.t->attr->attr_name.sz-1;
      SetTextCurXY();
   }
   if (AtBeginningOfInheritedAttrValue(curTextObj, endStrBlock, textEndIndex)) {
      textEndIndex = curTextObj->detail.t->attr->attr_name.sz-1;
      SetTextEndXY();
   }
}

static
void AdvanceForDoubleClickOnWord(double_byte)
   int double_byte;
{
   char *psz=(curStrBlock->seg->dyn_str.s);
   int len=(curStrBlock->seg->dyn_str.sz-1);
   int step=(double_byte ? 2 : 1);

   for ( ; textCurIndex > 0 &&
         PartOfAWord(double_byte, psz[textCurIndex-step]); textCurIndex-=step) {
   }
   for (endStrBlock=curStrBlock, textEndIndex=textCurIndex;
         textEndIndex < len &&
         PartOfAWord(double_byte, psz[textEndIndex]); textEndIndex+=step) {
   }
}

static
void AdvanceForDoubleClickOnSpaceChar()
{
   char *psz=(curStrBlock->seg->dyn_str.s);
   int len=(curStrBlock->seg->dyn_str.sz-1);

   for ( ; textCurIndex > 0 && psz[textCurIndex-1] == ' '; textCurIndex--) {
   }
   for (endStrBlock=curStrBlock, textEndIndex=textCurIndex;
         textEndIndex < len && psz[textEndIndex] == ' '; textEndIndex++) {
   }
}

static
void AdvanceForDoubleClickOnText()
{
   char cur_char=curStrBlock->seg->dyn_str.s[textCurIndex];
   int double_byte=curStrBlock->seg->double_byte;
   int select_word=PartOfAWord(double_byte, cur_char);

   if (select_word) {
      AdvanceForDoubleClickOnWord(double_byte);
   } else {
      AdvanceForDoubleClickOnSpaceChar();
   }
}

static
void SetTextIndices(drag, x_off, y_off, pressed_in_same_text)
   int drag, x_off, y_off, pressed_in_same_text;
{
   MiniLinesInfo *minilines=(&curTextObj->detail.t->minilines);
   StrBlockInfo *pStrBlock=NULL;
   int index=INVALID, position_end=FALSE;
   int abs_dx=0, abs_baseline_dy=0;

   position_end = BeginAdvance(drag, INVALID, &pStrBlock, &index);

   switch (GetCursorPositionInCurText(x_off, y_off, !pressed_in_same_text,
         &abs_dx, &abs_baseline_dy, &pStrBlock, &index)) {
   case 0:
      if (position_end) {
         textEndX = textOrigX + abs_dx;
         textEndBaselineY = textOrigBaselineY + abs_baseline_dy;
         endStrBlock = pStrBlock;
         textEndIndex = index;
      } else {
         textCurX = textOrigX + abs_dx;
         textCurBaselineY = textOrigBaselineY + abs_baseline_dy;
         curStrBlock = pStrBlock;
         textCurIndex = index;
      }
      break;
   case 1:
      pStrBlock = minilines->last->last_block;
      index = pStrBlock->seg->dyn_str.sz-1;;
      if (position_end) {
         endStrBlock = pStrBlock;
         textEndIndex = index;
      } else {
         curStrBlock = pStrBlock;
         textCurIndex = index;
      }
      EndAdvance(position_end, pStrBlock, index);
      break;
   case (-1):
      pStrBlock = minilines->first->first_block;
      index = 0;
      if (position_end) {
         endStrBlock = pStrBlock;
         textEndIndex = index;
      } else {
         curStrBlock = pStrBlock;
         textCurIndex = index;
      }
      EndAdvance(position_end, pStrBlock, index);
      break;
   }
   AdjTextIndicesForInheritedAttr();
}

static
void AdvanceStrBlockToParentBlock(advance_left, position_end)
   int advance_left, position_end;
{
   MiniLinesInfo *pOwnerMinilines=NULL;

   if (advance_left) {
      if (position_end) {
         if (endStrBlock->type == SB_SUPSUB_CENTER) {
            endStrBlock = endStrBlock->prev;
         } else {
            pOwnerMinilines = endStrBlock->owner_mini_line->owner_minilines;
            endStrBlock = pOwnerMinilines->owner_block->prev;
         }
         textEndIndex = (endStrBlock->seg->dyn_str.sz-1);
         SetTextEndXY();
      } else {
         if (curStrBlock->type == SB_SUPSUB_CENTER) {
            curStrBlock = curStrBlock->prev;
         } else {
            pOwnerMinilines = curStrBlock->owner_mini_line->owner_minilines;
            curStrBlock = pOwnerMinilines->owner_block->prev;
         }
         textCurIndex = (curStrBlock->seg->dyn_str.sz-1);
         SetTextCurXY();
      }
   } else {
      if (position_end) {
         if (endStrBlock->type == SB_SUPSUB_CENTER) {
            endStrBlock = endStrBlock->next;
         } else {
            pOwnerMinilines = endStrBlock->owner_mini_line->owner_minilines;
            endStrBlock = pOwnerMinilines->owner_block->next;
         }
         textEndIndex = 0;
         SetTextEndXY();
      } else {
         if (curStrBlock->type == SB_SUPSUB_CENTER) {
            curStrBlock = curStrBlock->next;
         } else {
            pOwnerMinilines = curStrBlock->owner_mini_line->owner_minilines;
            curStrBlock = pOwnerMinilines->owner_block->next;
         }
         textCurIndex = 0;
         SetTextCurXY();
      }
   }
}

static
int FindStrBlockPosInMiniline(pMiniline, pStrBlockTarget, pnPos)
   MiniLineInfo *pMiniline;
   StrBlockInfo *pStrBlockTarget;
   int *pnPos;
{
   StrBlockInfo *psbi=NULL;

   for (psbi=pMiniline->first_block; psbi != NULL; psbi=psbi->next) {
      if (pnPos != NULL) (*pnPos)++;
      if (psbi->type == SB_SIMPLE) {
         if (psbi == pStrBlockTarget) {
            return TRUE;
         }
      }
   }
   return FALSE;
}

static
int FindStrBlockPosInMinilines(pMinilines, pStrBlockTarget)
   MiniLinesInfo *pMinilines;
   StrBlockInfo *pStrBlockTarget;
{
   MiniLineInfo *pMiniline=NULL;
   int pos=0;

   for (pMiniline=pMinilines->first; pMiniline != NULL;
         pMiniline=pMiniline->next) {
      if (FindStrBlockPosInMiniline(pMiniline, pStrBlockTarget, &pos)) {
         return pos;
      }
   }
   return 0;
}

void SwitchTextCurAndEndStrBlocks()
{
   StrBlockInfo *pStrBlock=curStrBlock;
   int index=textCurIndex;

   curStrBlock = endStrBlock;
   textCurIndex = textEndIndex;

   endStrBlock = pStrBlock;
   textEndIndex = index;

   SetTextCurXY();
   SetTextEndXY();
}

int FixHighlightedStrBlockDepths()
{
   if (!textHighlight) {
      return 0;
   } else if (curStrBlock->depth == endStrBlock->depth) {
      if (curStrBlock->owner_mini_line->owner_minilines ==
            endStrBlock->owner_mini_line->owner_minilines) {
         if (curStrBlock == endStrBlock) {
            return textEndIndex - textCurIndex;
         } else {
            int cur_count = FindStrBlockPosInMinilines(
                  curStrBlock->owner_mini_line->owner_minilines, curStrBlock);
            int end_count = FindStrBlockPosInMinilines(
                  curStrBlock->owner_mini_line->owner_minilines, endStrBlock);

#ifdef _TGIF_DBG
            TgAssert(cur_count!=end_count,
                  "cur_count==end_count in FixHighlightedStrBlockDepths()",
                  NULL);
#endif /* _TGIF_DBG */
            return end_count - cur_count;
         }
      }
   }
   if (curStrBlock->depth < endStrBlock->depth) {
      /* move endStrBlock */
      while (curStrBlock->depth < endStrBlock->depth) {
         if (curStrBlock->pre_order < endStrBlock->pre_order) {
            /* move endStrBlock to the right */
            AdvanceStrBlockToParentBlock(ADVANCE_RIGHT, POSITION_END);
         } else {
            /* move endStrBlock to the left */
            AdvanceStrBlockToParentBlock(ADVANCE_LEFT, POSITION_END);
         }
      }
   } else if (curStrBlock->depth > endStrBlock->depth) {
      /* move curStrBlock */
      while (curStrBlock->depth > endStrBlock->depth) {
         if (curStrBlock->pre_order < endStrBlock->pre_order) {
            /* move curStrBlock to the left */
            AdvanceStrBlockToParentBlock(ADVANCE_LEFT, POSITION_CUR);
         } else {
            /* move curStrBlock to the right */
            AdvanceStrBlockToParentBlock(ADVANCE_RIGHT, POSITION_CUR);
         }
      }
   }
   if (curStrBlock->depth == endStrBlock->depth) {
      while (curStrBlock->owner_mini_line->owner_minilines !=
            endStrBlock->owner_mini_line->owner_minilines) {
         if (curStrBlock->pre_order < endStrBlock->pre_order) {
            /* move curStrBlock to the left and endStrBlock to the right */
            AdvanceStrBlockToParentBlock(ADVANCE_LEFT, POSITION_CUR);
            AdvanceStrBlockToParentBlock(ADVANCE_RIGHT, POSITION_END);
         } else {
            /* move curStrBlock to the right and endStrBlock to the left */
            AdvanceStrBlockToParentBlock(ADVANCE_LEFT, POSITION_END);
            AdvanceStrBlockToParentBlock(ADVANCE_RIGHT, POSITION_CUR);
         }
      }
   } else {
      /* Should not get here! */
   }
   return 0;
}
 
static Time lastClickTime=(Time)0;
static int textJustClicked=FALSE;

static
void DragButtonOnText()
{
   StrBlockInfo *pStrBlock=endStrBlock;
   int index=textEndIndex, done=FALSE;

   if (!debugNoPointerGrab) {
      XGrabPointer(mainDisplay, drawWindow, FALSE,
            PointerMotionMask | ButtonReleaseMask,
            GrabModeAsync, GrabModeAsync, None, textCursor, CurrentTime);
   }
   SetTextHighlight();
   FixHighlightedStrBlockDepths();
   UpdatePinnedMenu(MENU_EDIT);
   RedrawCurText();
   UpdateHighLightedTextBBoxes(TRUE);

   while (!done) {
      XEvent input;

      XNextEvent(mainDisplay, &input);

      if (input.type == Expose || input.type == VisibilityNotify) {
         ExposeEventHandler(&input, TRUE);
      } else if (input.type == ButtonRelease) {
         XUngrabPointer(mainDisplay, CurrentTime);
         done = TRUE;
      } else if (input.type == MotionNotify) {
         XEvent ev;
         int x_off=input.xmotion.x;
         int y_off=input.xmotion.y;

         SetTextIndices(TRUE, x_off, y_off, TRUE);

         if (pStrBlock != endStrBlock || index != textEndIndex) {
            SetTextHighlight();
            FixHighlightedStrBlockDepths();
            UpdatePinnedMenu(MENU_EDIT);
            UpdateHighLightedTextBBoxes(FALSE);
            RedrawCurText();
            UpdateHighLightedTextBBoxes(TRUE);

            pStrBlock = endStrBlock;
            index = textEndIndex;
         }
         while (XCheckMaskEvent(mainDisplay,PointerMotionMask,&ev)) ;
      }
   }
}

static
void AdjustTextHighlight(drag, saved_text_highlight,
      update_highlighted_text_bboxes)
   int drag, saved_text_highlight, update_highlighted_text_bboxes;
{
   SetTextHighlight();
   if (!drag) {
      int rc=FixHighlightedStrBlockDepths();

      if (rc < 0) {
         SwitchTextCurAndEndStrBlocks();
      }
   } else if (curStrBlock != NULL && endStrBlock != NULL &&
         curStrBlock->depth != endStrBlock->depth) {
      int rc=FixHighlightedStrBlockDepths();

      if (rc < 0) {
         SwitchTextCurAndEndStrBlocks();
      }
   }
   UpdatePinnedMenu(MENU_EDIT);
   if (update_highlighted_text_bboxes) {
      UpdateHighLightedTextBBoxes(FALSE);
   }
   if (textCursorShown && !saved_text_highlight && !drag) {
      PutTextCursor(); 
   } else {
      RedrawCurText();
   }
   ScrollTo(textCurX, textCurBaselineY);
}

static
void HandleClickOnText(drag, from_cursor_keys, x_off, y_off,
      pressed_in_same_text, obj_ptr, double_clicked, saved_text_highlight,
      skip_post_processing, click_time)
   int drag, from_cursor_keys, x_off, y_off, pressed_in_same_text;
   int double_clicked, saved_text_highlight;
   struct ObjRec *obj_ptr;
   Time click_time;
{
   SetTextIndices(drag, x_off, y_off, pressed_in_same_text);

   textCursorShown = TRUE;

   if (gnInputMethod != TGIM_NONE) {
      if (!tgIMHandleNewCurText(mainDisplay, drawWindow)) {
      }
   }
   if (double_clicked) {
      ResetOnCursorKey(FALSE);
      SetTextHighlight();
      UpdatePinnedMenu(MENU_EDIT);

      AdvanceForDoubleClickOnText();

      SetTextCurXY();
      SetTextEndXY();
      textJustClicked = FALSE;
   } else if (!from_cursor_keys) {
      textJustClicked = TRUE;
      lastClickTime = click_time;

      DragButtonOnText();
   }
   if (!skip_post_processing) {
      AdjustTextHighlight(drag, saved_text_highlight,
            from_cursor_keys || double_clicked);
   }
}

static
void HandleButton(Button_Ev)
   XButtonEvent *Button_Ev;
{
   int x_off=0, y_off=0, update_highlighted_text_bboxes=FALSE;

   if (!textCursorShown) {
      ClearCopyUTF8Info();
   }
   escPressed = FALSE;
   if (Button_Ev->button == Button1 &&
         !(Button_Ev->state & (ShiftMask|ControlMask))) {
      struct ObjRec *obj_ptr=NULL;
      int pressed_in_same_text=FALSE, x=0, abs_x=0, abs_y=0;

      x_off = Button_Ev->x; abs_x = ABS_X(x_off);
      y_off = Button_Ev->y; abs_y = ABS_Y(y_off);

      if (textCursorShown) {
         switch (curTextObj->detail.t->minilines.just) {
         case JUST_L: x = textOrigX-2; break;
         case JUST_C: x = textOrigX-textW/2-2; break;
         case JUST_R: x = textOrigX-textW-2; break;
         }
         if (x_off >= x && x_off <= x+textW+4 &&
               y_off >= textOrigY-2 && y_off <= textOrigY+textH+2) {
            pressed_in_same_text = TRUE;
            if (textHighlight) {
               UpdateHighLightedTextBBoxes(TRUE);
               update_highlighted_text_bboxes = TRUE;
            } else {
               EraseTextCursor();
            }
         } else {
            CreateTextObj(TRUE, TRUE); /* end editing on the old text */
            curTextModified = FALSE;
         }
      } else {
         editingText = FALSE;
         curTextModified = FALSE;
      }
      ResetOnCursorKey(FALSE);

      if (!pressed_in_same_text &&
            (obj_ptr=FindTextObj(x_off, y_off, NULL)) == NULL) {
         /* cursor not within any existing text object */
         HandleClickOnCanvas(x_off, y_off);
      } else {
         /* cursor inside an existing text object */
         Time click_time=Button_Ev->time;
         int double_clicked=FALSE;

         double_clicked = (pressed_in_same_text && textJustClicked &&
               (click_time-lastClickTime) < doubleClickInterval);
         if (pressed_in_same_text) {
            obj_ptr = curTextObj;
            if (textJustClicked && (click_time-lastClickTime) <
                  doubleClickInterval) {
               double_clicked = TRUE;
            }
         } else {
            if (!PrepareEditExistingText(obj_ptr, abs_x, abs_y,
                  &x_off, &y_off)) {
               return;
            }
         }
         if (!update_highlighted_text_bboxes && curStrBlock != NULL) {
            UpdateHighLightedTextBBoxes(TRUE);
         }
         HandleClickOnText(FALSE, FALSE, x_off, y_off, pressed_in_same_text,
               obj_ptr, double_clicked, TRUE, FALSE, click_time);
         UpdateTextInfoChoices(TRUE);
      }
   } else if (Button_Ev->button == Button1 &&
         (Button_Ev->state & (ShiftMask|ControlMask))) {
      if (!textCursorShown) return;

      x_off = Button_Ev->x;
      y_off = Button_Ev->y;

      if (curStrBlock != NULL) UpdateHighLightedTextBBoxes(TRUE);
      HandleClickOnText(TRUE, FALSE, x_off, y_off, TRUE, curTextObj, FALSE,
            TRUE, FALSE, (Time)0);
      UpdateTextInfoChoices(FALSE);
   }
}

static
void AdvanceRight(drag)
   int drag;
{
   StrBlockInfo *pStrBlock=NULL;
   int index=INVALID, position_end=FALSE;
   int done=FALSE, something_skipped=FALSE;

   position_end = BeginAdvance(drag, ADVANCE_RIGHT, &pStrBlock, &index);

   while (!done) {
      switch (pStrBlock->type) {
      case SB_SIMPLE:
      case SB_SUPSUB_CENTER:
         if (something_skipped) {
            done = TRUE;
         } else if (index < pStrBlock->seg->dyn_str.sz-1) {
            if (pStrBlock->seg->double_byte) {
               index += 2;
            } else {
               index++;
            }
            done = TRUE;
         }
         break;
      default: something_skipped = TRUE; break;
      }
      if (done) {
         EndAdvance(position_end, pStrBlock, index);
      } else if (pStrBlock->next == NULL) {
         MiniLineInfo *owner_mini_line=pStrBlock->owner_mini_line;
         MiniLinesInfo *owner_minilines=owner_mini_line->owner_minilines;

         if (owner_mini_line->next == NULL &&
               owner_minilines->owner_block != NULL &&
               owner_minilines->owner_block->next != NULL) {
            pStrBlock = owner_minilines->owner_block->next;
            something_skipped = TRUE;
         } else {
            done = TRUE;
         }
      } else {
         if (pStrBlock->type == SB_SUPSUB_CENTER) {
            pStrBlock = pStrBlock->next;
            something_skipped = TRUE;
         } else {
            pStrBlock = pStrBlock->next;
            if (pStrBlock->type == SB_SUPSUB_CENTER) {
               pStrBlock = pStrBlock->next;
               something_skipped = TRUE;
            }
         }
      }
      if (!done) {
         if (pStrBlock->type == SB_SIMPLE) {
            index = 0;
         }
      }
   }
}
 
static
void AdvanceLeft(drag)
   int drag;
{
   StrBlockInfo *pStrBlock=NULL;
   int index=INVALID, position_end=FALSE;
   int done=FALSE, something_skipped=FALSE;

   position_end = BeginAdvance(drag, ADVANCE_LEFT, &pStrBlock, &index);

   while (!done) {
      switch (pStrBlock->type) {
      case SB_SIMPLE:
      case SB_SUPSUB_CENTER:
         if (something_skipped) {
            done = TRUE;
         } else if (index != 0) {
            if (pStrBlock->seg->double_byte) {
               index -= 2;
            } else {
               index--;
            }
            done = TRUE;
         }
         break;
      default: something_skipped = TRUE; break;
      }
      if (done) {
         EndAdvance(position_end, pStrBlock, index);
      } else if (pStrBlock->prev == NULL) {
         MiniLineInfo *owner_mini_line=pStrBlock->owner_mini_line;
         MiniLinesInfo *owner_minilines=owner_mini_line->owner_minilines;

         if (owner_mini_line->prev == NULL &&
               owner_minilines->owner_block != NULL) {
            pStrBlock = owner_minilines->owner_block->prev;
            something_skipped = TRUE;
         } else {
            done = TRUE;
         }
      } else {
         if (pStrBlock->type == SB_SUPSUB_CENTER) {
            pStrBlock = pStrBlock->prev;
            something_skipped = TRUE;
         } else {
            pStrBlock = pStrBlock->prev;
            if (pStrBlock->type == SB_SUPSUB_CENTER) {
               pStrBlock = pStrBlock->prev;
               something_skipped = TRUE;
            }
         }
      }
      if (!done) {
         if (pStrBlock->type == SB_SIMPLE) {
            index = pStrBlock->seg->dyn_str.sz-1;
         }
      }
   }
   AdjTextIndicesForInheritedAttr();
}

static
void InsertCRLFIntoCurText()
{
   MiniLineInfo *pNewMiniLine=NewMiniLine();
   MiniLineInfo *owner_mini_line=curStrBlock->owner_mini_line;
   MiniLinesInfo *minilines=owner_mini_line->owner_minilines;
   StrBlockInfo *pStrBlock=NULL, *pRightStrBlock=NULL;
   char *psz=UtilStrDup(curStrBlock->seg->dyn_str.s), saved_ch='\0';

   if (psz == NULL) FailAllocMessage();

   DupStrBlock(curStrBlock, owner_mini_line, &pRightStrBlock,
         &pRightStrBlock);
   saved_ch = psz[textCurIndex];
   psz[textCurIndex] = '\0';
   DynStrSet(&curStrBlock->seg->dyn_str, psz);
   psz[textCurIndex] = saved_ch;
   DynStrSet(&pRightStrBlock->seg->dyn_str, &psz[textCurIndex]);
   UtilFree(psz);

   pRightStrBlock->next = curStrBlock->next;
   if (curStrBlock->next != NULL) {
      curStrBlock->next->prev = pRightStrBlock;
   }
   curStrBlock->next = NULL;
   owner_mini_line->last_block = curStrBlock;
   pNewMiniLine->owner_minilines = minilines;

   InsertMiniLine(owner_mini_line, owner_mini_line->next, pNewMiniLine,
         &minilines->first, &minilines->last);

   pNewMiniLine->first_block = pNewMiniLine->last_block = pRightStrBlock;
   pRightStrBlock->prev = NULL;
   for (pStrBlock=pRightStrBlock; pStrBlock != NULL;
         pStrBlock=pStrBlock->next) {
      pNewMiniLine->last_block = pStrBlock;
      pStrBlock->owner_mini_line = pNewMiniLine;
   }
   curStrBlock = pRightStrBlock;
   textCurIndex = 0;
   ResetOnCursorKey(FALSE);
   SetTextHighlight();
   UpdatePinnedMenu(MENU_EDIT);
}

static
void DoMoveTextCursorToNextMiniLine()
{
   int saved_text_highlight=textHighlight;
   MiniLineInfo *owner_mini_line=curStrBlock->owner_mini_line;
   MiniLinesInfo *owner_minilines=owner_mini_line->owner_minilines;

   if (textCursorShown && !saved_text_highlight) {
      EraseTextCursor();
   }
   UpdateHighLightedTextBBoxes(TRUE);

   if (owner_mini_line->next == NULL) {
      curStrBlock = owner_minilines->first->first_block;
   } else {
      curStrBlock = owner_mini_line->next->first_block;
   }
   textCurIndex = 0;

   ResetOnCursorKey(FALSE);
   SetTextCurXY();
   SetTextHighlight();
   UpdatePinnedMenu(MENU_EDIT);
   if (textCursorShown && !saved_text_highlight) {
      PutTextCursor(); 
   } else {
      RedrawCurText();
   }
   MarkRulers(textCurX, textCurY);
   ScrollTo(textCurX, textCurBaselineY);
   UpdateTextInfoChoices(FALSE);
}

static
void HandleCRLF(key_ev)
   XKeyEvent *key_ev;
{
   int cycle=(key_ev != NULL && (key_ev->state & ControlMask) != 0);

   escPressed = FALSE;

   if (cycle) {
      if (curStrBlock->type != SB_SUPSUB_CENTER) {
         DoMoveTextCursorToNextMiniLine();
      }
      return;
   }
   if (curStrBlock->type == SB_SUPSUB_CENTER) {
      return;
   }
   curTextModified = TRUE;
   if (textHighlight) {
      DeleteHighlightedText();
      EndChangeCurText(FALSE);
   }
   InsertCRLFIntoCurText();
   EndChangeCurText(TRUE);

   if (!gnDontRedrawDuringPaste) {
      ScrollTo(textCurX, textCurBaselineY);
   } else {
      gnPastingLineNum++;
      sprintf(gszMsgBox, TgLoadCachedString(CSTID_PASTING_LINE_NUM_DOTS),
            gnPastingLineNum+1);
      SetStringStatus(gszMsgBox);
   }
}

static
void HandleBS(key_ev, buf, key_sym, pn_has_char)
   XKeyEvent *key_ev;
   char *buf;
   KeySym key_sym;
   int *pn_has_char;
{
   struct AttrRec *attr_ptr=NULL;
   int merged_lines=FALSE;
   int nDeleteNextChar=(deleteNextCharWithDelKey &&
         CharIsDEL(key_ev, buf, key_sym, pn_has_char)) ||
         (!deleteNextCharWithDelKey && (key_ev != NULL &&
         (key_ev->state & (ControlMask | ShiftMask)) &&
         !((key_ev->state & ControlMask) && (key_sym == XK_h))));

   escPressed = FALSE;

   if (textHighlight) {
      DeleteHighlightedText();
      EndChangeCurText(FALSE);
      return;
   }
   attr_ptr = curTextObj->detail.t->attr;
   if (nDeleteNextChar) {
      if (CanAdvanceRight(curStrBlock, textCurIndex)) {
         AdvanceRight(TRUE);
      } else if (curStrBlock->next == NULL) {
         MiniLineInfo *pNextMiniLine=curStrBlock->owner_mini_line->next;

         if (pNextMiniLine == NULL) {
            return;
         }
         endStrBlock = pNextMiniLine->first_block;
         textEndIndex = 0;
         merged_lines = TRUE;
      } else {
         return;
      }
      SetTextHighlight();
      DeleteHighlightedText();
      if (merged_lines) {
         ResetDirtyBBoxInfo();
      }
      EndChangeCurText(merged_lines);
      UpdatePinnedMenu(MENU_EDIT);
      return;
   }
   if (AtBeginningOfInheritedAttrValue(curTextObj, curStrBlock, textCurIndex)) {
      return;
   }
   curTextModified = TRUE;
   if (textCurIndex != 0) {
      StrSegInfo *pStrSeg=curStrBlock->seg;
      int i=0, step=(pStrSeg->double_byte ? 2 : 1);
      char *psz=curStrBlock->seg->dyn_str.s;

      for (i=textCurIndex; i < pStrSeg->dyn_str.sz; i+=step) {
         if (step == 1) {
            psz[i-1] = psz[i];
         } else {
            psz[i-2] = psz[i];
            psz[i-1] = psz[i+1];
         }
      }
      textCurIndex -= step;
      pStrSeg->dyn_str.sz -= step;
   } else {
      if (CanAdvanceLeft(curStrBlock, textCurIndex)) {
         AdvanceLeft(TRUE);
      } else if (curStrBlock->prev == NULL) {
         MiniLineInfo *pNextMiniLine=curStrBlock->owner_mini_line->prev;

         if (pNextMiniLine == NULL) {
            return;
         }
         endStrBlock = pNextMiniLine->last_block;
         textEndIndex = endStrBlock->seg->dyn_str.sz-1;
         merged_lines = TRUE;
      } else {
         return;
      }
      SetTextHighlight();
      DeleteHighlightedText();
      UpdatePinnedMenu(MENU_EDIT);
   }
   if (merged_lines) {
      ResetDirtyBBoxInfo();
   }
   EndChangeCurText(merged_lines);
}

static
int InsertSingleByteCharIntoCurText(Str)
   char *Str;
{
   MiniLineInfo *owner_mini_line=curStrBlock->owner_mini_line;
   StrBlockInfo *pLeftStrBlock=NULL;

   DupStrBlock(curStrBlock, owner_mini_line, &pLeftStrBlock, &pLeftStrBlock);
   pLeftStrBlock->seg->double_byte = FALSE;
   pLeftStrBlock->seg->font = defaultSingleByteFont;
   DynStrSet(&pLeftStrBlock->seg->dyn_str, Str);
   pLeftStrBlock->clean = FALSE;

   if (textCurIndex == 0 && curStrBlock->prev != NULL &&
         curStrBlock->prev->type == SB_SIMPLE &&
         !curStrBlock->prev->seg->double_byte &&
         StrBlocksHasSameProperties(pLeftStrBlock, curStrBlock->prev)) {
      /* just add the current char to curStrBlock->prev */
      char *psz=(char*)malloc(curStrBlock->prev->seg->dyn_str.sz+1);

      if (psz == NULL) FailAllocMessage();
      sprintf(psz, "%s%s", curStrBlock->prev->seg->dyn_str.s, Str);
      DynStrSet(&curStrBlock->prev->seg->dyn_str, psz);
      curStrBlock->prev->clean = FALSE;

      UtilFree(psz);
      FreeStrBlock(pLeftStrBlock);
      return TRUE;
   }
   if (textCurIndex != 0) {
      StrBlockInfo *pDupStrBlock=NULL;

      DupStrBlock(curStrBlock, owner_mini_line, &pDupStrBlock, &pDupStrBlock);
      pDupStrBlock->seg->dyn_str.s[textCurIndex] = '\0';
      DynStrSet(&pDupStrBlock->seg->dyn_str, pDupStrBlock->seg->dyn_str.s);
      DynStrSet(&curStrBlock->seg->dyn_str,
            &curStrBlock->seg->dyn_str.s[textCurIndex]);
      pDupStrBlock->prev = curStrBlock->prev;
      if (curStrBlock->prev == NULL) {
         owner_mini_line->first_block = pDupStrBlock;
      } else {
         curStrBlock->prev->next = pDupStrBlock;
      }
      pDupStrBlock->next = curStrBlock;
      curStrBlock->prev = pDupStrBlock;

      curStrBlock->clean = pDupStrBlock->clean = FALSE;

      textCurIndex = 0;
   }
   pLeftStrBlock->prev = curStrBlock->prev;
   if (curStrBlock->prev == NULL) {
      owner_mini_line->first_block = pLeftStrBlock;
   } else {
      curStrBlock->prev->next = pLeftStrBlock;
   }
   pLeftStrBlock->next = curStrBlock;
   curStrBlock->prev = pLeftStrBlock;

   return TRUE;
}

static
int InsertCharIntoCurText(Str, double_byte)
   char *Str;
   int double_byte;
{
   if (curStrBlock->seg->double_byte && !double_byte) {
      if (curStrBlock->type == SB_SUPSUB_CENTER) {
         MsgBox(TgLoadString(STID_CANT_INS_NON_DB_CH_AT_CUR_REM), TOOL_NAME,
               INFO_MB);
         return FALSE;
      } else {
         InsertSingleByteCharIntoCurText(Str);
      }
   } else {
      char *buf=NULL;
      int cur_index_is_last=(textCurIndex==curStrBlock->seg->dyn_str.sz-1);

      if (lengthLimit256InInsertChar &&
            curStrBlock->seg->dyn_str.sz-1+(double_byte ? 2 : 1) >= MAXSTRING) {
         sprintf(gszMsgBox, TgLoadString(STID_STR_EXCEEDS_NUM_CH_IGNORED),
               MAXSTRING);
         Msg(gszMsgBox);
         return FALSE;
      }
      buf = (char*)malloc((curStrBlock->seg->dyn_str.sz+7)*sizeof(char));
      if (buf == NULL) FailAllocMessage();

      if (double_byte) {
         char tmp_s[3];

         tmp_s[0] = curStrBlock->seg->dyn_str.s[textCurIndex];
         curStrBlock->seg->dyn_str.s[textCurIndex] = '\0';
         if (tmp_s[0] != '\0') {
            tmp_s[1] = curStrBlock->seg->dyn_str.s[textCurIndex+1];
            curStrBlock->seg->dyn_str.s[textCurIndex+1] = '\0';
            sprintf(buf, "%s%c%c%c%c%s", curStrBlock->seg->dyn_str.s,
                  Str[0], Str[1], tmp_s[0], tmp_s[1],
                  (cur_index_is_last ? "" :
                  (&curStrBlock->seg->dyn_str.s[textCurIndex+2])));
         } else {
            sprintf(buf, "%s%c%c", curStrBlock->seg->dyn_str.s, Str[0], Str[1]);
         }
         textCurIndex += 2;
      } else {
         char tmp_char=curStrBlock->seg->dyn_str.s[textCurIndex];

         curStrBlock->seg->dyn_str.s[textCurIndex] = '\0';
         sprintf(buf, "%s%c%c%s", curStrBlock->seg->dyn_str.s, *Str, tmp_char,
               (cur_index_is_last ? "" :
               (&curStrBlock->seg->dyn_str.s[textCurIndex+1])));
         textCurIndex++;
      }
      DynStrSet(&curStrBlock->seg->dyn_str, buf);
      curStrBlock->clean = FALSE;
      free(buf);
   }
   EndChangeCurText(FALSE);

   return TRUE;
}

static
int SwitchToSingleOrDoubleByteFont(pStrSeg)
   StrSegInfo *pStrSeg;
{
   int saved_sz_unit=curSzUnit, saved_font=curFont, saved_style=curStyle;

   if (!(curChoice == DRAWTEXT && textCursorShown)) {
      TieLooseEnds();
   }
   curFont = pStrSeg->font;
   curStyle = pStrSeg->style;
   curSzUnit = pStrSeg->sz_unit;
   if (curChoice == DRAWTEXT && textCursorShown && editTextSize != 0) {
      curSzUnit = GetCurSzUnit();
   }
   allowFontFaceSubstitution = FALSE;
   SetCanvasFont();
   allowFontFaceSubstitution = TRUE;
   if (canvasFontSize == INVALID) {
      char buf[MAXSTRING];

      GetCurFontMsg(buf, NULL);
      curFont = saved_font;
      curStyle = saved_style;
      curSzUnit = saved_sz_unit;
      SetCanvasFont();
      sprintf(gszMsgBox, TgLoadString(STID_FONT_NOT_AVAILABLE), buf);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
   } else if (SzUnitToFontSize(curSzUnit) != canvasFontSize) {
      if (showFontSizeInPoints) {
         sprintf(gszMsgBox, TgLoadString(STID_CANT_CHANGE_SIZEPT_TO_USE_ALT),
               SzUnitToPointSize(curSzUnit),
               SzUnitToPointSize(FontSizeToSzUnit(canvasFontSize)));
      } else {
         sprintf(gszMsgBox, TgLoadString(STID_CANT_CHANGE_SIZE_TO_USE_ALT),
               SzUnitToFontSize(curSzUnit), canvasFontSize);
      }
      curSzUnit = FontSizeToSzUnit(canvasFontSize);
      if (curChoice == DRAWTEXT && textCursorShown && editTextSize != 0) {
         PutCurSzUnit(curSzUnit);
      }
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
   } else if (curChoice == DRAWTEXT && textCursorShown &&
         editTextSize != 0) {
      PutCurSzUnit(curSzUnit);
   }
   if (curChoice == DRAWTEXT && textCursorShown) {
      ChangeEditTextProperty(PROP_MASK_TEXT_FONT, pStrSeg->font);
      ChangeEditTextProperty(PROP_MASK_TEXT_STYLE, pStrSeg->style);
      ChangeEditTextProperty(PROP_MASK_TEXT_SZ_UNIT, pStrSeg->sz_unit);

      curTextModified = TRUE;
      UpdCurTextBBox();
      RedrawCurText();
      SetFileModified(TRUE);
      if (cycleThroughChoice) {
         SetPushedFontValue(PUSH_FONT, curFont);
      }
   } else {
      textCursorShown = FALSE;
   }
   ShowCurFont();
   ShowTextSize();
   UpdatePinnedMenu(MENU_FONT);

   return TRUE;
}

int SwitchToSingleByteFont(pStrSeg)
   StrSegInfo *pStrSeg;
{
   return SwitchToSingleOrDoubleByteFont(pStrSeg);
}

int SwitchToDoubleByteFont(pStrSeg)
   StrSegInfo *pStrSeg;
{
   return SwitchToSingleOrDoubleByteFont(pStrSeg);
}

static
void AdvanceEnd(drag)
   int drag;
{
   StrBlockInfo *pStrBlock=NULL;
   int index=INVALID, position_end=FALSE;
   MiniLinesInfo *minilines=NULL;
   MiniLineInfo *pMiniLine=NULL;

   position_end = BeginAdvance(drag, ADVANCE_RIGHT, &pStrBlock, &index);

   if (pStrBlock->type == SB_SUPSUB_CENTER) {
      index = pStrBlock->seg->dyn_str.sz-1;
      EndAdvance(position_end, pStrBlock, index);
   } else {
      minilines = pStrBlock->owner_mini_line->owner_minilines;
      for (pMiniLine=minilines->first; pMiniLine != NULL;
            pMiniLine=pMiniLine->next) {
         if (CurStrBlockInMiniLine(pMiniLine)) {
            pStrBlock = pMiniLine->last_block;
            /* pStrBlock->type must be SB_SIMPLE */
            index = pStrBlock->seg->dyn_str.sz-1;

            EndAdvance(position_end, pStrBlock, index);
            break;
         }
      }
   }
   AdjTextIndicesForInheritedAttr();
}

static
void HandleEnd(drag)
   int drag;
{
   int saved_text_highlight=textHighlight;

   if (textCursorShown && !saved_text_highlight && !drag) {
      EraseTextCursor();
   }
   UpdateHighLightedTextBBoxes(TRUE);
   if (!drag && endStrBlock != NULL) {
      textCurIndex = textEndIndex;
      curStrBlock = endStrBlock;
   }
   ResetOnCursorKey(drag);

   AdvanceEnd(drag);

   AdjustTextHighlight(drag, saved_text_highlight, TRUE);
   UpdateTextInfoChoices(FALSE);
}

static
int HandleRightMargin()
   /* return TRUE if something changed */
{
   char *psz=NULL;
   int right_margin=OFFSET_X(paperWidth-(rightMargin>>TIK_PER_PIXEL_SHIFTS));
   int x=textCurX, done=FALSE, last_index_beyond_right_margin=(-1);
   StrBlockInfo *psbi=NULL, *psbi_last_beyond_right_margin=NULL;

   if (textCurX <= right_margin) return FALSE;
   if (textHighlight) return FALSE;

   if (textCurIndex > 0 && curStrBlock->seg->dyn_str.s[textCurIndex-1] == ' ') {
      HandleCRLF(NULL);
      done = TRUE;
   }
   for (psbi=curStrBlock; !done && psbi != NULL; psbi=psbi->prev) {
      int saved_x=x;

      if (psbi->type == SB_SIMPLE) {
         StrSegInfo *pssi=psbi->seg;

         psz = strrchr(psbi->seg->dyn_str.s, ' ');
         while (!done && psz != NULL) {
            int w=0;

            if (psz[1] == '\0') {
               w = 0;
            } else {
               PushCurFont();
               curFont = pssi->font;
               curSzUnit = pssi->sz_unit;
               curStyle = pssi->style;
               SetCanvasFont();
               w = MyTextWidth(canvasFontPtr, &psz[1], strlen(&psz[1]));
               PopCurFont();
            }
            if (x-w < right_margin) {
               /* found it! */
               done = TRUE;
               curStrBlock = psbi;
               textCurIndex = ((int)(psz-psbi->seg->dyn_str.s))+1;
               HandleCRLF(NULL);
               HandleEnd(FALSE);
            } else {
               char *psz2=psz, saved_ch=(*psz);

               last_index_beyond_right_margin =
                     ((int)(psz-psbi->seg->dyn_str.s))+1;
               psbi_last_beyond_right_margin = psbi;
               *psz2 = '\0';
               psz = strrchr(psbi->seg->dyn_str.s, ' ');
               *psz2 = saved_ch;
            }
         }
      }
      if (!done) {
         x = saved_x - psbi->w;
      }
   }
   if (!done && psbi_last_beyond_right_margin != NULL) {
      curStrBlock = psbi_last_beyond_right_margin;
      textCurIndex = last_index_beyond_right_margin;
      HandleCRLF(NULL);
      HandleEnd(FALSE);
   }
   return TRUE;
}

static
void HandleChar(Str)
   char *Str;
{
   int double_byte=FALSE, len=strlen(Str);

   TgAssert(len <= 2, TgLoadString(STID_TOO_MANY_BYTES_IN_CH_IGNORED), NULL);

   double_byte = (len == 2);
   if (canvasFontDoubleByte) {
      escPressed = FALSE;
   } else {
      if (escPressed) {
         Str[0] |= 0x80;
         escPressed = FALSE;
      }
      if ((((*Str)&0x80) != '\0') && curFont!=FONT_SYM && !ValidCharCode(Str)) {
         return;
      }
   }
   if (textHighlight) {
      curTextModified = TRUE;
      DeleteHighlightedText();
      EndChangeCurText(FALSE);
      SetRightMarginActive();
   }
   if (canvasFontDoubleByte != double_byte &&
         curStrBlock->type == SB_SUPSUB_CENTER) {
      XEvent ev;

      MsgBox(TgLoadString(STID_CANT_CHNG_FONT_SUPSUB_CENTER), TOOL_NAME,
            INFO_MB);
      if (!gnDontRedrawDuringPaste) {
         RedrawCurText();
         UpdateTextInfoChoices(FALSE);
      }
      while (XCheckWindowEvent(mainDisplay, drawWindow, KeyPressMask, &ev)) ;
      return;
   } else if (canvasFontDoubleByte && !double_byte) {
      StrSegInfo double_byte_ssi;
      StrSegInfo ssi;

      memset(&double_byte_ssi, 0, sizeof(StrSegInfo));
      memcpy(&double_byte_ssi, curStrBlock->seg, sizeof(StrSegInfo));
      memset(&ssi, 0, sizeof(StrSegInfo));
      if (CanFindSingleByteFontAtCursor(&ssi)) {
         if (SwitchToSingleByteFont(&ssi)) {
            SetCopyUTF8FontInfo(&double_byte_ssi, TRUE);
            double_byte = FALSE;
         }
      } else if (gstCopyUTF8Info.single_byte_valid) {
         if (SwitchToSingleByteFont(&gstCopyUTF8Info.single_byte_seg)) {
            SetCopyUTF8FontInfo(&double_byte_ssi, TRUE);
            double_byte = FALSE;
         }
      } else {
         ssi.font = GetInitialFont();
         ssi.style = curStyle;
         ssi.sz_unit = curSzUnit;
         if (SwitchToSingleByteFont(&ssi)) {
            SetCopyUTF8FontInfo(&double_byte_ssi, TRUE);
            double_byte = FALSE;
         }
      }
   } else if (!canvasFontDoubleByte && double_byte) {
      StrSegInfo single_byte_ssi;
      StrSegInfo ssi;

      memset(&single_byte_ssi, 0, sizeof(StrSegInfo));
      memcpy(&single_byte_ssi, curStrBlock->seg, sizeof(StrSegInfo));
      memset(&ssi, 0, sizeof(StrSegInfo));
      if (CanFindDoubleByteFontAtCursor(&ssi)) {
         if (SwitchToDoubleByteFont(&ssi)) {
            SetCopyUTF8FontInfo(&single_byte_ssi, FALSE);
            double_byte = TRUE;
         }
      } else if (gstCopyUTF8Info.double_byte_valid) {
         if (SwitchToDoubleByteFont(&gstCopyUTF8Info.double_byte_seg)) {
            SetCopyUTF8FontInfo(&single_byte_ssi, FALSE);
            double_byte = TRUE;
         }
      }
   }
   if (!InsertCharIntoCurText(Str, double_byte)) {
      XEvent ev;

      if (!gnDontRedrawDuringPaste) {
         RedrawCurText();
         UpdateTextInfoChoices(FALSE);
      }
      while (XCheckWindowEvent(mainDisplay, drawWindow, KeyPressMask, &ev)) ;
      return;
   }
   if ((rightMarginEnabled == TRUE) && rightMarginActive) {
      /* add a <CR> somewhere */
      HandleRightMargin();
   }
   if (gnDontRedrawDuringPaste) {
      if ((rightMarginEnabled == TRUE) && rightMarginActive) {
         gnDontRedrawDuringPaste = FALSE;
      }
   }
   if (!gnDontRedrawDuringPaste) {
      ScrollTo(textCurX, textCurBaselineY);
   }
}

static
void HandleTAB(key_ev)
   XKeyEvent *key_ev;
{
   struct AttrRec *attr_ptr=NULL, *new_attr=NULL;
   struct ObjRec *obj_ptr=NULL;
   int abs_x=0, abs_y=0, x_off=0, y_off=0;

   escPressed = FALSE;

   if ((attr_ptr=curTextObj->detail.t->attr) == NULL) {
      MsgBox(TgLoadString(STID_CANT_TAB_OUT_OF_NON_ATTR_TEXT), TOOL_NAME,
            INFO_MB);
      return;
   }
   CreateTextObj(TRUE, TRUE);
   curTextModified = FALSE;

   if (key_ev != NULL && (key_ev->state & METAMASK) != 0) {
      /* new_attr will be the next attribute */
      if (attr_ptr->next == NULL) {
         new_attr = attr_ptr->owner->fattr;
      } else {
         new_attr = attr_ptr->next;
      }
      while (!new_attr->shown) {
         if (new_attr->next == NULL) {
            new_attr = new_attr->owner->fattr;
         } else {
            new_attr = new_attr->next;
         }
      }
   } else {
      /* new_attr will be the previous attribute */
      if (attr_ptr->prev == NULL) {
         new_attr = attr_ptr->owner->lattr;
      } else {
         new_attr = attr_ptr->prev;
      }
      while (!new_attr->shown) {
         if (new_attr->prev == NULL) {
            new_attr = new_attr->owner->lattr;
         } else {
            new_attr = new_attr->prev;
         }
      }
   }
   obj_ptr = new_attr->obj;
   abs_x = obj_ptr->x; x_off = OFFSET_X(abs_x);
   abs_y = obj_ptr->y; y_off = OFFSET_Y(abs_y);

   if (!PrepareEditExistingText(obj_ptr, abs_x, abs_y, &x_off, &y_off)) {
      return;
   }
   if (curStrBlock != NULL) UpdateHighLightedTextBBoxes(TRUE);
   HandleClickOnText(FALSE, TRUE, x_off, y_off, FALSE, obj_ptr, FALSE, TRUE,
         FALSE, (Time)0);
   UpdateTextInfoChoices(TRUE);
}

static
void AdvanceHome(drag)
   int drag;
{
   StrBlockInfo *pStrBlock=NULL;
   int index=INVALID, position_end=FALSE;
   MiniLinesInfo *minilines=NULL;
   MiniLineInfo *pMiniLine=NULL;

   position_end = BeginAdvance(drag, ADVANCE_LEFT, &pStrBlock, &index);

   if (pStrBlock->type == SB_SUPSUB_CENTER) {
      EndAdvance(position_end, pStrBlock, 0);
   } else {
      minilines = pStrBlock->owner_mini_line->owner_minilines;
      for (pMiniLine=minilines->first; pMiniLine != NULL;
            pMiniLine=pMiniLine->next) {
         if (CurStrBlockInMiniLine(pMiniLine)) {
            EndAdvance(position_end, pMiniLine->first_block, 0);
            break;
         }
      }
   }
   AdjTextIndicesForInheritedAttr();
}

static
void HandleHome(drag)
   int drag;
{
   int saved_text_highlight=textHighlight;

   if (textCursorShown && !saved_text_highlight && !drag) {
      EraseTextCursor();
   }
   UpdateHighLightedTextBBoxes(TRUE);
   ResetOnCursorKey(drag);

   AdvanceHome(drag);

   AdjustTextHighlight(drag, saved_text_highlight, TRUE);
   UpdateTextInfoChoices(FALSE);
}

static
int CanMoveTextCursorIntoLeftScripts()
{
   if (textCursorShown && curStrBlock != NULL && textCurIndex == 0 &&
         curStrBlock->prev != NULL &&
         (curStrBlock->prev->type == SB_SUPSUB_LEFT ||
         curStrBlock->prev->type == SB_SUPSUB_CENTER ||
         curStrBlock->prev->type == SB_SUPSUB_RIGHT)) {
      return TRUE;
   }
   return FALSE;
}

static
void DoMoveTextCursorIntoLeftScripts()
{
   int saved_text_highlight=textHighlight;

   if (textCursorShown && !saved_text_highlight) {
      EraseTextCursor();
   }
   UpdateHighLightedTextBBoxes(TRUE);

   curStrBlock = curStrBlock->prev->sup->first->last_block;

   textCurIndex = curStrBlock->seg->dyn_str.sz-1;
   ResetOnCursorKey(FALSE);
   SetTextCurXY();
   SetTextHighlight();
   UpdatePinnedMenu(MENU_EDIT);
   if (textCursorShown && !saved_text_highlight) {
      PutTextCursor(); 
   } else {
      RedrawCurText();
   }
   MarkRulers(textCurX, textCurY);
   ScrollTo(textCurX, textCurBaselineY);
   UpdateTextInfoChoices(FALSE);
}

static
void HandleLeft(key_ev)
   XKeyEvent *key_ev;
{
   int saved_text_highlight=textHighlight;
   int cycle=((key_ev->state & ControlMask) != 0);
   int drag=((key_ev->state & ShiftMask) != 0);

   if (cycle) {
      if (CanMoveTextCursorIntoLeftScripts()) {
         DoMoveTextCursorIntoLeftScripts();
      }
      return;
   }
   if (textCursorShown && !saved_text_highlight && !drag) {
      EraseTextCursor();
   } else if (textCursorShown && saved_text_highlight && !drag) {
      if (FixHighlightedStrBlockDepths() < 0) {
         SwitchTextCurAndEndStrBlocks();
      }
   }
   UpdateHighLightedTextBBoxes(TRUE);
   ResetOnCursorKey(drag);

   AdvanceLeft(drag);

   AdjustTextHighlight(drag, saved_text_highlight, TRUE);
   UpdateTextInfoChoices(FALSE);
}

static
int CanCycleTextCursorInScripts()
{
   return (textCursorShown && curStrBlock != NULL && curStrBlock->depth != 0);
}

static
void DoCycleTextCursorInScripts(direction)
   int direction;
{
   int saved_text_highlight=textHighlight;
   MiniLinesInfo *pOwnerMinilines=NULL;
   StrBlockInfo *pOwnerStrBlock=NULL;

   if (textCursorShown && !saved_text_highlight) {
      EraseTextCursor();
   }
   UpdateHighLightedTextBBoxes(TRUE);
   ResetOnCursorKey(FALSE);

   switch(curStrBlock->type) {
   case SB_SIMPLE:
      pOwnerMinilines = curStrBlock->owner_mini_line->owner_minilines;
      pOwnerStrBlock = pOwnerMinilines->owner_block;

      switch (pOwnerStrBlock->type) {
      case SB_SUPSUB_LEFT:
      case SB_SUPSUB_RIGHT:
         if (pOwnerMinilines == pOwnerStrBlock->sup) {
            curStrBlock = pOwnerStrBlock->sub->first->first_block;
         } else if (pOwnerMinilines == pOwnerStrBlock->sub) {
            curStrBlock = pOwnerStrBlock->sup->first->first_block;
         } else {
            if (textCursorShown && !saved_text_highlight) {
               PutTextCursor();
            }  
            return;
         }
         break;
      case SB_SUPSUB_CENTER:
         if (pOwnerMinilines == pOwnerStrBlock->sup) {
            if (direction == SCROLL_UP) {
               curStrBlock = pOwnerStrBlock->sub->first->first_block;
            } else {
               curStrBlock = pOwnerStrBlock;
            }
         } else if (pOwnerMinilines == pOwnerStrBlock->sub) {
            if (direction == SCROLL_UP) {
               curStrBlock = pOwnerStrBlock;
            } else {
               curStrBlock = pOwnerStrBlock->sup->first->first_block;
            }
         } else {
            if (textCursorShown && !saved_text_highlight) {
               PutTextCursor();
            }  
            return;
         }
         break;
      }
      break;

   case SB_CHAR_SPACE: /* not possible */
   case SB_SUPSUB_LEFT: /* not possible */
   case SB_SUPSUB_RIGHT: /* not possible */
      if (textCursorShown && !saved_text_highlight) {
         PutTextCursor();
      }  
      return;

   case SB_SUPSUB_CENTER:
      if (curStrBlock->sub != NULL) {
         if (direction == SCROLL_UP) {
            curStrBlock = curStrBlock->sup->first->first_block;
         } else {
            curStrBlock = curStrBlock->sub->first->first_block;
         }
      } else {
         if (textCursorShown && !saved_text_highlight) {
            PutTextCursor();
         }  
         return;
      }
      break;
   }
   textCurIndex = 0;
   SetTextCurXY();
   SetTextHighlight();
   UpdatePinnedMenu(MENU_EDIT);
   if (textCursorShown && !saved_text_highlight) {
      PutTextCursor(); 
   } else {
      RedrawCurText();
   }
   MarkRulers(textCurX, textCurY);
   ScrollTo(textCurX, textCurBaselineY);
   UpdateTextInfoChoices(FALSE);
}

static
void HandleUp(key_ev)
   XKeyEvent *key_ev;
{
   int saved_text_highlight=textHighlight;
   int cycle=((key_ev->state & ControlMask) != 0);
   int drag=((key_ev->state & ShiftMask) != 0), new_x_off=0, new_y_off=0;

   if (cycle) {
      if (CanCycleTextCursorInScripts()) {
         DoCycleTextCursorInScripts(SCROLL_UP);
      }
      return;
   }
   if (textCursorShown && !saved_text_highlight && !drag) {
      EraseTextCursor();
   }
   UpdateHighLightedTextBBoxes(TRUE);
   ResetOnCursorKey(drag);

   if (drag && textHighlight) {
      new_x_off = textEndX;
      if (endStrBlock->type == SB_SUPSUB_CENTER) {
         new_y_off = textEndBaselineY-endStrBlock->seg->asc-2;
      } else {
         MiniLineInfo *pOwnerMiniLine=endStrBlock->owner_mini_line;

         new_y_off = textEndBaselineY - pOwnerMiniLine->asc -
               pOwnerMiniLine->v_gap - 2;
         if (pOwnerMiniLine->owner_minilines != NULL &&
               pOwnerMiniLine->owner_minilines->owner_block == NULL) {
            new_y_off -= pOwnerMiniLine->owner_minilines->v_space;
         }
      }
   } else {
      new_x_off = textCurX;
      if (curStrBlock->type == SB_SUPSUB_CENTER) {
         new_y_off = textCurBaselineY-curStrBlock->seg->asc-2;
      } else {
         MiniLineInfo *pOwnerMiniLine=curStrBlock->owner_mini_line;

         new_y_off = textCurBaselineY - pOwnerMiniLine->asc -
               pOwnerMiniLine->v_gap - 2;
         if (pOwnerMiniLine->owner_minilines != NULL &&
               pOwnerMiniLine->owner_minilines->owner_block == NULL) {
            new_y_off -= pOwnerMiniLine->owner_minilines->v_space;
         }
      }
   }
   HandleClickOnText(drag, TRUE, new_x_off, new_y_off, TRUE, curTextObj, FALSE,
         saved_text_highlight, FALSE, key_ev->time);
   UpdateTextInfoChoices(FALSE);
}
 
static
int CanMoveTextCursorIntoRightScripts()
{
   if (textCursorShown && curStrBlock != NULL &&
         textCurIndex == curStrBlock->seg->dyn_str.sz-1 &&
         curStrBlock->next != NULL &&
         (curStrBlock->next->type == SB_SUPSUB_LEFT ||
         curStrBlock->next->type == SB_SUPSUB_CENTER ||
         curStrBlock->next->type == SB_SUPSUB_RIGHT)) {
      return TRUE;
   }
   return FALSE;
}

static
void DoMoveTextCursorIntoRightScripts()
{
   int saved_text_highlight=textHighlight;

   if (textCursorShown && !saved_text_highlight) {
      EraseTextCursor();
   }
   UpdateHighLightedTextBBoxes(TRUE);

   curStrBlock = curStrBlock->next->sup->first->first_block;

   textCurIndex = 0;
   ResetOnCursorKey(FALSE);
   SetTextCurXY();
   SetTextHighlight();
   UpdatePinnedMenu(MENU_EDIT);
   if (textCursorShown && !saved_text_highlight) {
      PutTextCursor(); 
   } else {
      RedrawCurText();
   }
   MarkRulers(textCurX, textCurY);
   ScrollTo(textCurX, textCurBaselineY);
   UpdateTextInfoChoices(FALSE);
}

static
void HandleRight(key_ev)
   XKeyEvent *key_ev;
{
   int saved_text_highlight=textHighlight;
   int cycle=((key_ev->state & ControlMask) != 0);
   int drag=((key_ev->state & ShiftMask) != 0);

   if (cycle) {
      if (CanMoveTextCursorIntoRightScripts()) {
         DoMoveTextCursorIntoRightScripts();
      }
      return;
   }
   if (textCursorShown && !saved_text_highlight && !drag) {
      EraseTextCursor();
   } else if (textCursorShown && saved_text_highlight && !drag) {
      if (FixHighlightedStrBlockDepths() < 0) {
         SwitchTextCurAndEndStrBlocks();
      }
   }
   UpdateHighLightedTextBBoxes(TRUE);
   if (!drag && endStrBlock != NULL) {
      textCurIndex = textEndIndex;
      curStrBlock = endStrBlock;
   }
   ResetOnCursorKey(drag);

   AdvanceRight(drag);

   AdjustTextHighlight(drag, saved_text_highlight, TRUE);
   UpdateTextInfoChoices(FALSE);
}

static
void HandleDown(key_ev)
   XKeyEvent *key_ev;
{
   int saved_text_highlight=textHighlight;
   int cycle=((key_ev->state & ControlMask) != 0);
   int drag=((key_ev->state & ShiftMask) != 0), new_x_off=0, new_y_off=0;

   if (cycle) {
      if (CanCycleTextCursorInScripts()) {
         DoCycleTextCursorInScripts(SCROLL_DOWN);
      }
      return;
   }
   if (textCursorShown && !saved_text_highlight && !drag) {
      EraseTextCursor();
   }
   UpdateHighLightedTextBBoxes(TRUE);
   ResetOnCursorKey(drag);

   if (drag && textHighlight) {
      new_x_off = textEndX;
      if (endStrBlock->type == SB_SUPSUB_CENTER) {
         new_y_off = textEndBaselineY+endStrBlock->seg->des+2;
      } else {
         MiniLineInfo *pOwnerMiniLine=endStrBlock->owner_mini_line;

         new_y_off = textEndBaselineY+pOwnerMiniLine->des+2;
         if (pOwnerMiniLine->owner_minilines != NULL &&
               pOwnerMiniLine->owner_minilines->owner_block == NULL) {
            new_y_off += pOwnerMiniLine->owner_minilines->v_space;
         }
         if (pOwnerMiniLine->next != NULL) {
            new_y_off += pOwnerMiniLine->v_gap;
         }
      }
   } else {
      new_x_off = textCurX;
      if (curStrBlock->type == SB_SUPSUB_CENTER) {
         new_y_off = textCurBaselineY+curStrBlock->seg->des+2;
      } else {
         MiniLineInfo *pOwnerMiniLine=curStrBlock->owner_mini_line;

         new_y_off = textCurBaselineY+pOwnerMiniLine->des+2;
         if (pOwnerMiniLine->owner_minilines != NULL &&
               pOwnerMiniLine->owner_minilines->owner_block == NULL) {
            new_y_off += pOwnerMiniLine->owner_minilines->v_space;
         }
         if (pOwnerMiniLine->next != NULL) {
            new_y_off += pOwnerMiniLine->v_gap;
         }
      }
   }
   HandleClickOnText(drag, TRUE, new_x_off, new_y_off, TRUE, curTextObj, FALSE,
         saved_text_highlight, FALSE, key_ev->time);
   UpdateTextInfoChoices(FALSE);
}

static
void DoHandleInputFromBuffer(cut_buffer)
   char *cut_buffer;
{
   char *c_ptr=NULL, s[80];

   s[2] = '\0';
   for (c_ptr = cut_buffer; *c_ptr != '\0'; c_ptr+=2) {
      int double_byte=FALSE;

      if (((*c_ptr) & 0x80) == '\0') {
         s[0] = c_ptr[0];
         s[1] = '\0';
      } else {
         double_byte = TRUE;

         if (!canvasFontDoubleByte && double_byte) {
            StrSegInfo ssi;

            memset(&ssi, 0, sizeof(StrSegInfo));
            if (!CanFindDoubleByteFontAtCursor(&ssi) &&
                  !gstCopyUTF8Info.double_byte_valid) {
               double_byte = FALSE;
            }
         }
         if (double_byte) {
            s[0] = c_ptr[0];
            s[1] = c_ptr[1];
         } else {
            s[0] = c_ptr[0];
            s[1] = '\0';
         }
      }
      switch (s[0]) {
      case '\r':
      case '\n': HandleCRLF(NULL); c_ptr--; break;

      case '\177': Msg(TgLoadCachedString(CSTID_CANT_PASTE_DEL)); break;
      case '\b': Msg(TgLoadCachedString(CSTID_CANT_PASTE_BS)); break;
      case '\033': Msg(TgLoadCachedString(CSTID_CANT_PASTE_ESC)); break;

      case '\t':
         Msg(TgLoadCachedString(CSTID_CANT_PASTE_TAB_REPLACE_BY_SPC));
         s[0] = ' ';
         /* drop through */
      default:
         HandleChar(s);
         if (!double_byte) {
            c_ptr--;
         }
         break;
      }
   }
}

void HandleInputFromBuffer(cut_buffer)
   char *cut_buffer;
{
   if (cut_buffer == NULL || *cut_buffer == '\0') return;

   SaveStatusStrings();
   gnPastingLineNum = 0;
   if (!((rightMarginEnabled == TRUE) && rightMarginActive)) {
      gnDontRedrawDuringPaste = TRUE;
   }
   DoHandleInputFromBuffer(cut_buffer);
   gnDontRedrawDuringPaste = FALSE;
   RestoreStatusStrings();

   UpdTextBBox(curTextObj);
   SetTextCurXY();
   if (textHighlight) SetTextEndXY();
   UpdateEditTextArea(
         curTextObj->detail.t->minilines.w,
         curTextObj->detail.t->minilines.h,
         curTextObj->detail.t->minilines.min_lbearing,
         curTextObj->detail.t->minilines.max_rextra);

   RedrawCurText();
   ScrollTo(textCurX, textCurBaselineY);
   MarkRulers(textCurX, textCurY);
   SetFileModified(TRUE);
   UpdateTextInfoChoices(FALSE);

   return;
}

void HandleTextFromBuffer(key_ev, s, key_sym, has_ch)
   XKeyEvent *key_ev;
   char *s;
   KeySym key_sym;
   int has_ch;
{
   int drag=FALSE, cursor_key=FALSE;

   ResetDirtyBBoxInfo();

   if (CharIsCntrlShiftINS(key_ev, key_sym)) {
      char szBuf[MAXSTRING];
      int nBufSize=sizeof(szBuf);

      memset(szBuf, 0, sizeof(szBuf));
      if (InputOctalString(szBuf, &nBufSize)) {
         HandleInputFromBuffer(szBuf);
      }
   } else if (CharIsESC(key_ev, s, key_sym, &has_ch) ||
         CharIsCRorLF(key_ev, s, key_sym, &has_ch) ||
         CharIsBSorDEL(key_ev, s, key_sym, &has_ch, FALSE) ||
         CharIsTAB(key_ev, s, key_sym, &has_ch) ||
         CharIsHome(key_ev, s, key_sym, &has_ch) ||
         CharIsEnd(key_ev, s, key_sym, &has_ch)) {
   } else if (!((s[0]=='\r' && (key_sym & 0xff)=='\r') ||
         (s[0]=='\n' && (key_sym & 0xff)=='\n') ||
         (s[0]=='\b' && (key_sym & 0xff)=='\b') ||
         (s[0]=='\b' && (key_sym & 0xff)=='h' &&
         (key_ev != NULL && (key_ev->state & ControlMask))) ||
         key_sym==XK_Left || key_sym==XK_Up ||
         key_sym==XK_Right || key_sym==XK_Down ||
         key_sym==XK_KP_Left || key_sym==XK_KP_Up ||
         key_sym==XK_KP_Right || key_sym==XK_KP_Down ||
         (key_sym==XK_Tab &&
         (key_ev != NULL && (key_ev->state & (ShiftMask | METAMASK)) != 0)) ||
         (s[0]=='\033' && (key_sym & 0xff)=='\033') ||
         (s[0]=='\177' && (key_sym & 0x7f)=='\177') ||
         ((s[0]&0xff) && canvasFontDoubleByte) ||
         (has_ch==1 && ((unsigned char)s[0])>='\040'))) {
      return;
   }
   drag = ((key_ev->state & ShiftMask) != 0);

   switch (key_sym) {
   case XK_Left: HandleLeft(key_ev); cursor_key = TRUE; break;
   case XK_KP_Left: HandleLeft(key_ev); cursor_key = TRUE; break;
   case XK_Up: HandleUp(key_ev); cursor_key = TRUE; break;
   case XK_KP_Up: HandleUp(key_ev); cursor_key = TRUE; break;
   case XK_Right: HandleRight(key_ev); cursor_key = TRUE; break;
   case XK_KP_Right: HandleRight(key_ev); cursor_key = TRUE; break;
   case XK_Down: HandleDown(key_ev); cursor_key = TRUE; break;
   case XK_KP_Down: HandleDown(key_ev); cursor_key = TRUE; break;
   default:
      if (!canvasFontDoubleByte) {
         s[1] = '\0';
      }
      if (CharIsESC(key_ev, s, key_sym, &has_ch)) {
         if (!escPressed) {
            escPressed = TRUE;
            return;
         }
      } else if (CharIsCRorLF(key_ev, s, key_sym, &has_ch)) {
         HandleCRLF(key_ev);
      } else if (CharIsBSorDEL(key_ev, s, key_sym, &has_ch, FALSE)) {
         if (has_ch && s[0] == '\177') {
            if (escPressed) {
               HandleChar(s);
            } else {
               HandleBS(key_ev, s, key_sym, &has_ch);
            }
         } else {
            HandleBS(key_ev, s, key_sym, &has_ch);
         }
      } else if (CharIsTAB(key_ev, s, key_sym, &has_ch)) {
         HandleTAB(key_ev);
         cursor_key = TRUE;
      } else if (CharIsHome(key_ev, s, key_sym, &has_ch)) {
         HandleHome(drag);
         cursor_key = TRUE;
      } else if (CharIsEnd(key_ev, s, key_sym, &has_ch)) {
         HandleEnd(drag);
         cursor_key = TRUE;
      } else if (has_ch) {
         if (canvasFontDoubleByte) {
            char ch[3], *c_ptr=NULL;

            ch[2] = '\0';
            for (c_ptr=s; *c_ptr != '\0'; c_ptr++) {
               int double_byte=((*c_ptr)&0x80);

               ch[0] = c_ptr[0];
               if (double_byte) {
                  ch[1] = *(++c_ptr);
               } else {
                  ch[1] = '\0';
               }
               HandleChar(ch);
            }
         } else {
            HandleChar(s);
         }
      }
      break;
   }
   if (textCursorShown) {
      MarkRulers(textCurX, textCurY);
      if (!cursor_key) SetFileModified(TRUE);
   }
}

static
void HandlePasteInDrawTextMode()
{
   char *cut_buffer=NULL;
   int xfree_cut_buffer=FALSE;
   int len=0;

   if (pasteFromFileInDrawTextMode) {
      FILE *fp;
      char inbuf[MAXSTRING+1];
      int size=0;

      pasteFromFileInDrawTextMode = FALSE;
      if ((fp=fopen(pasteFromFileName, "r")) == NULL) {
         sprintf(gszMsgBox, TgLoadString(STID_CANNOT_OPEN_FILE_FOR_READING),
               pasteFromFileName);
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
         return;
      }
      while (fgets(inbuf, MAXSTRING, fp) != NULL) size += strlen(inbuf);
      fclose(fp);
      if (size == 0) {
         sprintf(gszMsgBox, TgLoadString(STID_NAMED_FILE_IS_EMPTY),
               pasteFromFileName);
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
         return;
      }
      cut_buffer = (char*)malloc((size+2)*sizeof(char));
      if (cut_buffer == NULL) {
         FailAllocMessage();
         return;
      }
      memset(cut_buffer, 0, (size+2)*sizeof(char));
      if ((fp=fopen(pasteFromFileName, "r")) == NULL) {
         sprintf(gszMsgBox, TgLoadString(STID_CANNOT_OPEN_FILE_FOR_READING),
               pasteFromFileName);
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
         free(cut_buffer);
         return;
      }
      len = 0;
      while (fgets(&cut_buffer[len], MAXSTRING, fp) != NULL) {
         len += strlen(&cut_buffer[len]);
      }
      fclose(fp);
#ifndef _NO_NKF
      if (Tg_useNKF()) {
         char *out_buffer=(char*)malloc(size+2);

         if (out_buffer == NULL) FailAllocMessage();
         Tg_do_kconv(cut_buffer, out_buffer, size+2);
         strcpy(cut_buffer, out_buffer);
         free(out_buffer);
      } else {
#ifndef _NO_KINPUT
         if (copyAndPasteJIS) {
            CvtJisToEuc(cut_buffer, cut_buffer);
         }
#endif /* ~_NO_KINPUT */
      }
#endif /* ~_NO_NKF */
   } else if (pasteCompoundTextInDrawTextMode) {
      unsigned long ul=0L;

      pasteCompoundTextInDrawTextMode = FALSE;
      cut_buffer = GetTextBytesFromSelection(TRUE, &ul);
      if (cut_buffer == NULL || ul == 0L) {
         Msg(TgLoadString(STID_COMPOUND_TEXT_BUF_IS_EMPTY));
         if (cut_buffer != NULL) UtilFree(cut_buffer);
         return;
      }
   } else {
      int from_selection=FALSE;

      cut_buffer = FetchSelectionOrCutBuffer(&len, &from_selection);
      if (cut_buffer == NULL) {
         Msg(TgLoadString(STID_CUT_BUFFER_IS_EMPTY));
         return;
      }
      xfree_cut_buffer = !from_selection;
#ifndef _NO_KINPUT
      if (copyAndPasteJIS) {
         CvtJisToEuc(cut_buffer, cut_buffer);
      }
#endif /* ~_NO_KINPUT */
   }

   if (escPressed) {
      escPressed = FALSE;
      Msg(TgLoadString(STID_AN_ESC_KEY_PRESS_IGNORED));
   }
   HandleInputFromBuffer(cut_buffer);

   if (gnPastingLineNum > 0) {
      sprintf(gszMsgBox, TgLoadString(STID_GIVEN_NUM_LINES_PASTED),
            gnPastingLineNum);
      Msg(gszMsgBox);
   }
   FreeSelectionOrCutBuffer(cut_buffer, !xfree_cut_buffer);
}

static
void HandleCopyInDrawTextMode()
{
   int copy_failed=FALSE, copy_db_utf8_str=FALSE;
   int cut_buffer_size=0, double_byte_font_index=INVALID;
   char *cut_buffer=NULL;

   if (!textHighlight) return;

   if (escPressed) {
      escPressed = FALSE;
      Msg(TgLoadString(STID_AN_ESC_KEY_PRESS_IGNORED));
   }
   if (copyDoubleByteStringInDrawTextMode &&
         CanCopyHighLightedTextAsUTF8Strings(&double_byte_font_index)) {
      /* copy just text, cut_buffer_size will include terminating '\0' */
      GatherHighLightedTextAsStrings(&cut_buffer, &cut_buffer_size);
      copy_db_utf8_str = TRUE;
   } else if (CanCopyHighLightedTextAsStrings()) {
      /* copy just text, cut_buffer_size will include terminating '\0' */
      GatherHighLightedTextAsStrings(&cut_buffer, &cut_buffer_size);
   } else {
#ifdef _TGIF_DBG /* debug, do not translate */
      TgAssert(FALSE,
            "Can only handle simple text in HandleCopyInDrawTextMode()", NULL);
#endif /* _TGIF_DBG */
   }
   if (cut_buffer == NULL) {
      ClearSelection();
      return;
   }
#ifndef _NO_KINPUT
   if (!copy_db_utf8_str && canvasFontDoubleByte &&
         gnInputMethod == TGIM_KINPUT && copyAndPasteJIS) {
      char *tmp_buffer=NULL;

      cut_buffer_size = CvtEucToJis(NULL, cut_buffer);
      tmp_buffer = (char*)malloc((cut_buffer_size+2)*sizeof(char));
      CvtEucToJis(tmp_buffer, cut_buffer);
      free(cut_buffer);
      cut_buffer = tmp_buffer;
   }
#endif /* ~_NO_KINPUT */
   if (copy_db_utf8_str && (double_byte_font_index != INVALID)) {
      if (!ConvertToOrFromUTF8(CONVERT_TO_UTF8, double_byte_font_index, &cut_buffer)) {
         ClearSelection();
         return;
      }
      cut_buffer_size = strlen(cut_buffer)+1;
   }
   copy_failed = !WriteBufToCutBuffer(cut_buffer, cut_buffer_size-1, TRUE,
         copy_db_utf8_str && (double_byte_font_index != INVALID), NULL);

   if (copy_failed) {
      sprintf(gszMsgBox, TgLoadString(STID_COPY_FAIL_SEL_STR_MAY_TOO_LNG));
   } else {
      sprintf(gszMsgBox, TgLoadString(STID_COPY_BUFFER_UPDATED));
   }
   Msg(gszMsgBox);
   free(cut_buffer);
}

static
int HandleFunctionKey(key_sym)
   KeySym key_sym;
{
   int index=0;
   char buf[MAXSTRING], *c_ptr=NULL, tmp_fname[MAXSTRING+1];
   FILE *fp=NULL;
   
   switch (key_sym) {
   case XK_F1: index=1; break;
   case XK_F2: index=2; break;
   case XK_F3: index=3; break;
   case XK_F4: index=4; break;
   case XK_F5: index=5; break;
   case XK_F6: index=6; break;
   case XK_F7: index=7; break;
   case XK_F8: index=8; break;
   case XK_F9: index=9; break;
   case XK_F10: index=10; break;
   case XK_F11: index=11; break;
   case XK_F12: index=12; break;
   default: return FALSE;
   }
   sprintf(buf, "DrawTextFuncKey_F%1d", index);
   if ((c_ptr=XGetDefault(mainDisplay, TOOL_NAME, buf)) == NULL) {
      return FALSE;
   }
   if (MkTempFile(tmp_fname, sizeof(tmp_fname), tmpDir, TOOL_NAME) ==
         NULL) {
      return TRUE;
   }
   if ((fp=fopen(tmp_fname, "w")) == NULL) {
      FailToWriteFileMessage(tmp_fname);
      return TRUE;
   }
   fprintf(fp, "%s\n", c_ptr);
   fclose(fp);

   ExecCmdsFromTmp(tmp_fname);

   unlink(tmp_fname);

   return TRUE;
}

int HandleDoubleByteUTF8Chars(s, has_ch)
   char *s;
   int has_ch;
{
   if (has_ch >= 2 && !canvasFontDoubleByte &&
         Tgim_has_stringprep_convert()) {
      char *psz=NULL, *psz1=NULL, *psz2=NULL, *psz_encoding=NULL;
      char buf[MAXSTRING];

      GetCurFontInfoStr(buf, sizeof(buf));
      psz1 = strrchr(buf, '-');
      if (psz1 != NULL) {
         *psz1 = '\0';
         psz2 = strrchr(buf, '-');
         if (psz2 != NULL) {
            *psz2 = '\0';
         }
         *psz1 = '-';
      }
      if (psz2 != NULL) {
         if (strchr(&psz2[1], '*') == NULL) {
            psz_encoding = &psz2[1];
         }
         *psz2 = '-';
      }
      if (psz_encoding != NULL) {
         psz = Tgim_stringprep_convert(s, psz_encoding, "utf-8");
         if (psz != NULL && strlen(psz) == 1) {
            s[0] = psz[0];
            s[1] = '\0';
            has_ch = 1;
         }
         UtilFree(psz);
      }
      if (has_ch > 1) {
         s[0] = '\0';
         has_ch = 0;
      }
   }
   return has_ch;
}

static XComposeStatus c_stat;

void DrawText(input)
   XEvent *input;
{
   if (input->type == ButtonPress) {
      HandleButton(&(input->xbutton));
   } else if (input->type == PropertyNotify) {
      if (gnInputMethod != TGIM_NONE) {
         char *psz_buf=NULL;
         XPropertyEvent *prop_ev=((XPropertyEvent*)input);

         if (tgIMConvertProperty(mainDisplay, drawWindow, prop_ev, &psz_buf) &&
               psz_buf != NULL) {
            HandleInputFromBuffer(psz_buf);
            free(psz_buf);
         }
      }
   } else if (input->type == KeyPress) {
      char s[80];
      int has_ch=0;
      XKeyEvent *key_ev=NULL;
      KeySym key_sym=(KeySym)0;

      if (!textCursorShown) return;

      if (pasteInDrawTextMode) {
         pasteInDrawTextMode = FALSE;
         HandlePasteInDrawTextMode();
         return;
      } else if (copyInDrawTextMode) {
         copyInDrawTextMode = FALSE;
         HandleCopyInDrawTextMode();
         return;
      }
      key_ev = (&(input->xkey));
      if (gnInputMethod != TGIM_NONE &&
            /*
             * Cannot call: InputMethodTypeMatched(canvasFontDoubleByte) &&
             */
            tgIMExpectLookupString(mainDisplay, drawWindow)) {
         if (tgIMHandleLookupString(mainDisplay, drawWindow, key_ev,
               s, sizeof(s), &key_sym, &has_ch)) {
            if (has_ch > 0) s[has_ch] = '\0';
            has_ch = HandleDoubleByteUTF8Chars(s, has_ch);
         }
         TranslateKeys(s, &key_sym);
      } else {
         has_ch = XLookupString(key_ev, s, sizeof(s), &key_sym, &c_stat);
         if (has_ch > 0) s[has_ch] = '\0';
         if (gnInputMethod != TGIM_NONE) {
            char s1[80];

            *s1 = '\0';
            if (tgIMTranslateKeyEvent(mainDisplay, mainWindow, key_ev, s1)) {
               strcpy(s, s1);
               /* added by Ambrose Li <acli@mingpaoxpress.com> */
               key_sym = (KeySym)0;
            }
            if (tgIMExpectClientMessage(mainDisplay, mainWindow)) {
               /* input characters only come from ClientMessage */
               return;
            }
         } else {
            TranslateKeys(s, &key_sym);
         }
      }
      if (HandleFunctionKey(key_sym)) {
         return;
      }
      if (textHighlight) {
         if (!CanCopyHighLightedTextAsUTF8Strings(NULL)) {
            ClearCopyUTF8Info();
         } else if (gstCopyUTF8Info.double_byte_valid &&
               !gstCopyUTF8Info.single_byte_valid) {
            CalcSingleByteInfoForCopyUTF8(NULL);
         }
      }
      HandleTextFromBuffer(key_ev, s, key_sym, has_ch);
   }
}

void InsertHexOctalChar()
{
   XKeyEvent key_ev;
   KeySym key_sym=(KeySym)XK_Insert;
   char s[1];
   int has_ch=FALSE;

   if (curChoice != DRAWTEXT || !textCursorShown) return;

   memset(&key_ev, 0, sizeof(XKeyEvent));
   key_ev.state = (ControlMask | ShiftMask);
   s[0] = '\0';
   HandleTextFromBuffer(&key_ev, s, key_sym, has_ch);
}

void InsertSymbol()
{
   int need_to_restore=FALSE;

   if (curChoice != DRAWTEXT || !textCursorShown) return;

   if (textHighlight) {
      curTextModified = TRUE;
      DeleteHighlightedText();
      EndChangeCurText(FALSE);
   }
   if (curFont != FONT_SYM) {
      PushCurFont();
      need_to_restore = TRUE;
   }
   if (ChangeFont(FONT_SYM, FALSE)) {
      TgInsertSymbol_HandleCntrlSpace(mainDisplay, drawWindow);
   }
   if (need_to_restore) {
      PopCurFont();
      ChangeFont(curFont, FALSE);
   }
}

void EditTextInAttr(attr_ptr)
   struct AttrRec *attr_ptr;
{
   struct ObjRec *obj_ptr=attr_ptr->obj;
   int abs_x, abs_y, x_off=0, y_off=0;
   int ok=TRUE;
   int saved_grouped_text_editable=groupedTextEditable;

   SetCurChoice(DRAWTEXT);
   if (obj_ptr->ctm == NULL) {
      abs_x = obj_ptr->obbox.ltx;
      abs_y = obj_ptr->obbox.lty;
   } else {
      abs_x = ((obj_ptr->obbox.ltx+obj_ptr->obbox.rbx)>>1);
      abs_y = ((obj_ptr->obbox.lty+obj_ptr->obbox.rby)>>1);
   }
   groupedTextEditable = TRUE;
   ok = (FindTextObj(OFFSET_X(abs_x), OFFSET_Y(abs_y), obj_ptr) == obj_ptr);
   groupedTextEditable = saved_grouped_text_editable;
   if (!ok) {
      return;
   }
   editingText = FALSE;
   curTextModified = FALSE;

   if (!PrepareEditExistingText(obj_ptr, abs_x, abs_y, &x_off, &y_off)) {
      return;
   }
   textJustClicked = FALSE;
   if (curStrBlock != NULL) UpdateHighLightedTextBBoxes(TRUE);
   HandleClickOnText(FALSE, TRUE, x_off, y_off, FALSE, curTextObj, FALSE, TRUE,
         FALSE, (Time)0);
   RedrawCurText();
   UpdateTextInfoChoices(TRUE);
}

void DumpOneStr(FP, FontIndex, DoubleByte, DoubleByteModBytes, Str)
   register FILE *FP;
   int FontIndex, DoubleByte, DoubleByteModBytes;
   register char *Str;
{
   register char *c_ptr;

   if (DoubleByte) {
      for ( ; *Str != '\0'; Str++) {
         switch (*Str) {
         case '(':
         case ')':
         case '\\': if (fprintf(FP, "\\") == EOF) writeFileFailed = TRUE; break;
         }
         if (((*Str) & 0x80) != '\0') {
            if (DoubleByteModBytes) {
               if (fprintf(FP, "\\%03o\\%03o", Str[0]&0x7f, Str[1]&0x7f) == EOF) {
                  writeFileFailed = TRUE;
               }
            } else {
#ifdef _PS_SEVENBIT_DOUBLEBYTE_TEXT
               if (fprintf(FP, "\\%03o\\%03o", Str[0]&0x7f, Str[1]&0x7f) == EOF) {
#else /* ~_PS_SEVENBIT_DOUBLEBYTE_TEXT */
               if (fprintf(FP, "\\%03o\\%03o", Str[0]&0xff, Str[1]&0xff) == EOF) {
#endif /* _PS_SEVENBIT_DOUBLEBYTE_TEXT */
                  writeFileFailed = TRUE;
               }
            }
            Str++;
            if (*Str == '\0') break;
         } else {
            if (fprintf(FP, "%c", *Str) == EOF) writeFileFailed = TRUE;
         }
      }
   } else {
      for ( ; *Str != '\0'; Str++) {
         switch (*Str) {
         case '(':
         case ')':
         case '\\': fprintf(FP, "\\"); break;
         }
         if (((*Str) & 0x80) != '\0') {
            if (FontIndex != FONT_SYM &&
                  (c_ptr=CharCodeTranslate(Str)) != NULL) {
               if (*c_ptr == '\\') {
                  if (fprintf(FP, "%s", c_ptr) == EOF) writeFileFailed = TRUE;
               } else if (*c_ptr == '8') {
                  if (fprintf(FP, "\\%c%c%c", c_ptr[2], c_ptr[3], c_ptr[4]) ==
                        EOF) {
                     writeFileFailed = TRUE;
                  }
               }
            } else {
               if (fprintf(FP, "\\%o", (*Str)&0xff) == EOF) {
                  writeFileFailed = TRUE;
               }
            }
         } else {
            if (fprintf(FP, "%c", *Str)== EOF) writeFileFailed = TRUE;
         }
      }
   }
}

void DrawTextObj(Win, XOff, YOff, ObjPtr)
   Window Win;
   int XOff, YOff;
   struct ObjRec *ObjPtr;
{
   struct TextRec *text_ptr=ObjPtr->detail.t;
   int x, y, xinc=0, yinc=0, trans_pat=0, fill=0, pen=0;
   int use_cache=FALSE;
   XGCValues values;

   if (userDisableRedraw) return;

   if ((text_ptr->fill == NONEPAT ||
         (ObjPtr->trans_pat && text_ptr->fill == BACKPAT)) &&
         (text_ptr->pen == NONEPAT ||
         (ObjPtr->trans_pat && text_ptr->pen == BACKPAT))) {
      return;
   }
   if (BlankMiniLines(&text_ptr->minilines)) {
      return;
   }

   PushCurFont();
   ObjFontInfoToCurFontInfo(text_ptr);
   pen = text_ptr->pen;

   if (NeedsToCacheTextObj(ObjPtr)) {
      if (text_ptr->cached_bitmap == None) {
         MakeCachedTextBitmap(ObjPtr);
      }
      use_cache = TRUE;
   }
   x = ObjPtr->x - XOff;
   y = ObjPtr->y - YOff;

   xinc = 0;
   yinc = textCursorH+textVSpace;

   trans_pat = ObjPtr->trans_pat;
   fill = text_ptr->fill;

   if (curChoice == DRAWTEXT && textCursorShown && ObjPtr == curTextObj) {
      if (RestoreEditTextSize(ObjPtr, TRUE)) {
         UpdTextBBox(ObjPtr);
      }
   }
   if (fill != NONEPAT) {
      int real_x_off, real_y_off, ltx, lty, rbx, rby;

      real_x_off = (zoomedIn ? XOff : (XOff>>zoomScale)<<zoomScale);
      real_y_off = (zoomedIn ? YOff : (YOff>>zoomScale)<<zoomScale);
      ltx = ZOOMED_SIZE(ObjPtr->bbox.ltx + 1 - real_x_off);
      lty = ZOOMED_SIZE(ObjPtr->bbox.lty + 1 - real_y_off);
      rbx = ZOOMED_SIZE(ObjPtr->bbox.rbx - 1 - real_x_off);
      rby = ZOOMED_SIZE(ObjPtr->bbox.rby - 1 - real_y_off);

      values.foreground = GetDrawingBgPixel(fill, colorPixels[ObjPtr->color]);
      values.function = GXcopy;
      values.fill_style = (trans_pat ? FillStippled : FillOpaqueStippled);
      values.stipple = patPixmap[fill];
      XChangeGC(mainDisplay, drawGC,
            GCForeground | GCFunction | GCFillStyle | GCStipple, &values);
      if (ObjPtr->ctm != NULL) {
         XFillPolygon(mainDisplay, Win, drawGC, ObjPtr->rotated_obbox, 5,
               Convex, CoordModeOrigin);
      } else {
         XFillRectangle(mainDisplay, Win, drawGC, ltx, lty, rbx-ltx, rby-lty);
      }
   }
   if (pen == NONEPAT || (trans_pat && pen == BACKPAT)) {
   } else if (use_cache || ObjPtr->ctm != NULL || zoomScale != 0 ||
         text_ptr->read_only) {
      if (ObjPtr->obbox.ltx != ObjPtr->obbox.rbx &&
            ObjPtr->obbox.lty != ObjPtr->obbox.rby) {
         int ltx, lty, w, h;

         ltx = OFFSET_X(ObjPtr->bbox.ltx+1);
         lty = OFFSET_Y(ObjPtr->bbox.lty+1);
         w = ZOOMED_SIZE(ObjPtr->bbox.rbx-ObjPtr->bbox.ltx-2);
         h = ZOOMED_SIZE(ObjPtr->bbox.rby-ObjPtr->bbox.lty-2);

         if (!trans_pat) {
            values.foreground = myBgPixel;
            values.function = GXcopy;
            values.fill_style = FillSolid;
            values.clip_mask = text_ptr->cached_bg_bitmap;
            values.clip_x_origin = ltx;
            values.clip_y_origin = lty;
            XChangeGC(mainDisplay, drawGC,
                  GCForeground | GCFunction | GCFillStyle | GCClipMask |
                  GCClipXOrigin | GCClipYOrigin, &values);
            FillClippedRectangle(Win, drawGC, ltx, lty, w, h);
         }
         values.function = GXcopy;
         values.fill_style = (trans_pat ? FillStippled : FillOpaqueStippled);
         values.stipple = patPixmap[pen];
         values.clip_mask = text_ptr->cached_bitmap;
         values.clip_x_origin = ltx;
         values.clip_y_origin = lty;
         XChangeGC(mainDisplay, drawGC,
               GCFunction | GCFillStyle | GCStipple | GCClipMask |
               GCClipXOrigin | GCClipYOrigin, &values);

         DrawClippedPixmap(text_ptr->cached_pixmap, Win, drawGC, w, h,
               ltx, lty);

         values.clip_mask = None;
         values.clip_x_origin = 0;
         values.clip_y_origin = 0;
         XChangeGC(mainDisplay, drawGC,
               GCClipMask | GCClipXOrigin | GCClipYOrigin, &values);
         if (numClipRecs > 0) {
            XSetClipRectangles(mainDisplay, drawGC, 0, 0, clipRecs,
               numClipRecs, clipOrdering);
         }
      }
   } else {
      MiniLinesInfo *minilines=(&text_ptr->minilines);
      int saved_pen=penPat, saved_trans_pat=transPat;
      int baseline_y=text_ptr->baseline_y-YOff;

      ResetDirtyBBoxInfo();
      penPat = pen;
      transPat = trans_pat;
      PaintMiniLines(mainDisplay, Win, drawGC, mainDepth, x, baseline_y,
            minilines, FALSE, FALSE);
      penPat = saved_pen;
      transPat = saved_trans_pat;
   }
   if (curChoice == DRAWTEXT && textCursorShown && ObjPtr == curTextObj) {
      if (RestoreEditTextSize(ObjPtr, FALSE)) {
         UpdTextBBox(ObjPtr);
      }
   }
   PopCurFont();
}

#define FONTS_PER_DPI (((MAXFONTS-1)*MAXFONTSTYLES+1)*MAXFONTSIZES)

static
int OldFontIndex(dpi_index, font_index, size_index, style_index)
   register int dpi_index, font_index, size_index, style_index;
   /* obsoleted procedure, kept to remain compatible with old versions */
{
   if (font_index == FONT_SYM) {
      return (size_index+MAXFONTSIZES*(MAXFONTSTYLES*font_index) +
            dpi_index*FONTS_PER_DPI);
   } else {
      return (size_index+MAXFONTSIZES*(style_index+MAXFONTSTYLES*font_index) +
            dpi_index*FONTS_PER_DPI);
   }
}

static
void ReadTextSetFontStyleSzUnit(font_str, sb_font_str, style, sz_unit,
      size, dpi, pn_double_byte, pn_cur_sb_font, pn_cur_db_font)
   char *font_str, *sb_font_str;
   int style, sz_unit, size, dpi;
   int *pn_double_byte, *pn_cur_sb_font, *pn_cur_db_font;
   /*
    * set curFont, curStyle, and curSzUnit
    */
{
   char *psz=NULL;
   int len=strlen(font_str);

   if (len > 1 && *font_str == '\'' && font_str[len-1] == '\'') {
      char *psz1=(&font_str[1]);

      font_str[len-1] = '\0';
      psz = font_str;
      while (*psz1 != '\0') *psz++ = *psz1++;
      *psz = '\0';
   }
   if (fileVersion <= 33 && (psz=strchr(font_str, '%')) != NULL) {
      *pn_double_byte = TRUE;
   }
   if (*pn_double_byte) {
      if ((psz=strchr(font_str, '%')) != NULL) {
         *psz++ = '\0';
         strcpy(sb_font_str, font_str);
         strcpy(font_str, psz);
         /*
          * e.g., font_str="Ryumin-Light-EUC-H", sb_font_str="Helvetica-Bold"
          */
      }
   }
   /*
    * Set curFont and cur_sb_font so that they correspond to the text object.
    */
   if (PRTGIF && !cmdLineOpenDisplay) {
      curFont = GetFontIndex(font_str, style, FALSE);
      if (*pn_double_byte) {
         *pn_cur_db_font = curFont;
         *pn_cur_sb_font = GetFontIndex(sb_font_str, style, FALSE);
      }
   } else {
      curFont = GetFontIndex(font_str, style, TRUE);
      if (curFont == INVALID) {
         sprintf(gszMsgBox, TgLoadString(STID_CANNOT_FIND_FONT_USE_ALT),
               font_str, "Times");
         Msg(gszMsgBox);
         SetFileModified(TRUE);
         curFont = FONT_TIM;
         if (*pn_double_byte) {
            /*
             * curFont would have been the double-byte font.
             * But since it can't be found and Times is used, it should
             *     not be double-byte any more.
             */
            *pn_double_byte = FALSE;
         }
      }
      if (*pn_double_byte) {
         *pn_cur_db_font = curFont;
         *pn_cur_sb_font = GetFontIndex(sb_font_str, style, TRUE);
         if (*pn_cur_sb_font == INVALID) {
            sprintf(gszMsgBox, TgLoadString(STID_CANNOT_FIND_FONT_USE_ALT),
                  sb_font_str, "Times");
            Msg(gszMsgBox);
            SetFileModified(TRUE);
            *pn_cur_sb_font = FONT_TIM;
         }
      }
   }
   curStyle = style;
   if (fileVersion <= 29) {
      size = GetCompatibleSize(dpi, size);
   }
   if (fileVersion <= 35) {
      curSzUnit = FontSizeToSzUnit(size);
   } else {
      curSzUnit = sz_unit;
   }
}

static
void ReadTextSetCanvasFont(text_ptr, dpi, size, asc, des, double_byte,
      db_mod_bytes, db_vertical, direction)
   struct TextRec *text_ptr;
   int dpi, size, asc, des, double_byte, db_mod_bytes, db_vertical, direction;
   /*
    * set canvasFontAsc, canvasFontDes, textCursorH, and canvasFontDoubleByte...
    */
{
   if (PRTGIF) {
      if (fileVersion < 10) {
         canvasFontAsc =
               pDrawFontAsc[OldFontIndex(dpi,curFont,size,curStyle)];
         canvasFontDes =
               pDrawFontDes[OldFontIndex(dpi,curFont,size,curStyle)];
      } else {
         canvasFontAsc = asc;
         canvasFontDes = des;
      }
      textCursorH = canvasFontAsc + canvasFontDes;
      text_ptr->read_only = FALSE;
      if (fileVersion > 31) {
         canvasFontDoubleByte = double_byte;
         canvasFontDirection = direction;
         /*
          * Need to fix this in the future.
          */
         canvasFontDontReencode = FALSE;

         canvasFontDoubleByteModBytes = db_mod_bytes;
         canvasFontDoubleByteVertical = db_vertical;
      } else {
         canvasFontDoubleByte = FALSE;
         canvasFontDoubleByteModBytes = FALSE;
         canvasFontDoubleByteVertical = FALSE;
         canvasFontDirection = FontLeftToRight;
         canvasFontDontReencode = FALSE;
      }
   } else {
      /*
       * since fileVersion 37 there's no such a thing as a read_only
       *       text object.  If after SetCanvasFont() and
       *       canvasFontSize != INVALID &&
       *       SzUnitToFontSize(curSzUnit) != canvasFontSize
       */
      if (fileVersion <= 36) {
         text_ptr->read_only = SetCanvasFont();
      } else {
         /* curFont, curStyle, and curSzUnit are undefined here */
      }
   }
}

static
int ReadStrSeg(FP, ppStrSeg)
   FILE *FP;
   StrSegInfo **ppStrSeg;
{
   int font=0, style=0, sz_unit=0;
   int w=0, asc=0, des=0, min_lbearing=0, max_rextra=0;
   int underline_on=0, direction=0, new_alloc=FALSE, modified=FALSE;
   int double_byte=0, db_mod_bytes=0, db_vertical=0, dontreencode=FALSE;
   char *s=NULL, *line=NULL, color_str[40], font_str[MAXSTRING], *psz=NULL;
   StrSegInfo *pStrSeg=NULL;

   *font_str = '\0';
   *ppStrSeg = NULL;
   if ((line=UtilGetALine(FP)) == NULL) {
      sprintf(gszMsgBox, TgLoadString(STID_UNEXPECTED_EOF_IN_ABORT_READ),
            scanFileName, scanLineNum, "ReadStrSeg()");
      if (PRTGIF) {
         fprintf(stderr, "%s\n", gszMsgBox);
      } else {
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      }
      return FALSE;
   }
   scanLineNum++;

   s = FindChar((int)'(', line);
   s = ParseStr(s, (int)',', color_str, sizeof(color_str));
   InitScan(s, ", \t\n");

   if (fileVersion >= 37) {
      if (GETSTR("str_seg", font_str,       "font_str") == INVALID ||
          GETINT("str_seg", style,          "style") == INVALID ||
          GETINT("str_seg", sz_unit,        "sz_unit") == INVALID ||
          GETINT("str_seg", w,              "w") == INVALID ||
          GETINT("str_seg", asc,            "asc") == INVALID ||
          GETINT("str_seg", des,            "des") == INVALID ||
          GETINT("str_seg", min_lbearing,   "min_lbearing") == INVALID ||
          GETINT("str_seg", max_rextra,     "max_rextra") == INVALID ||
          GETINT("str_seg", underline_on,   "underline_on") == INVALID ||
          GETINT("str_seg", double_byte,    "double_byte") == INVALID ||
          GETINT("str_seg", db_mod_bytes,   "db_mod_bytes") == INVALID ||
          GETINT("str_seg", db_vertical,    "db_vertical") == INVALID ||
          GETINT("str_seg", direction,      "direction") == INVALID) {
         return FALSE;
      }
   }
   free(line);

   font = GetFontIndexFromPSFontAndStyle(font_str, style, &double_byte,
         &modified);
   if (modified) SetFileModified(TRUE);

   if ((line=UtilGetALine(FP)) == NULL) {
      sprintf(gszMsgBox, TgLoadString(STID_UNEXPECTED_EOF_IN_ABORT_READ),
            scanFileName, scanLineNum, "ReadStrSeg()");
      if (PRTGIF) {
         fprintf(stderr, "%s\n", gszMsgBox);
      } else {
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      }
      return FALSE;
   }
   scanLineNum++;

   psz = FindChar((int)'"', line);
   s = ReadString(psz);
   *(--s) = '\0';

   pStrSeg = NewStrSeg();
   DynStrSet(&pStrSeg->dyn_str, psz);

   free(line);

   pStrSeg->color = QuickFindColorIndex(NULL, color_str, &new_alloc, TRUE);
   UtilStrCpyN(pStrSeg->color_str, sizeof(pStrSeg->color_str), color_str);

   if (PRTGIF) {
      if (*font_str != '\0') {
         pStrSeg->font_name = UtilStrDup(font_str);
         if (pStrSeg->font_name == NULL) FailAllocMessage();
      }
   } else {
      curFont = font;
      curStyle = style;
      curSzUnit = sz_unit;
      SetCanvasFont();
      if (canvasFontSize != INVALID &&
            SzUnitToFontSize(curSzUnit) != canvasFontSize) {
         SetFileModified(TRUE);
         sz_unit = curSzUnit = FontSizeToSzUnit(canvasFontSize);
      }
      double_byte = canvasFontDoubleByte;
      db_mod_bytes = canvasFontDoubleByteModBytes;
      db_vertical = canvasFontDoubleByteVertical;
      direction = canvasFontDirection;
      dontreencode = canvasFontDontReencode;
   }
   pStrSeg->font = font;
   pStrSeg->style = style;
   pStrSeg->sz_unit = sz_unit;
   pStrSeg->w = w;
   pStrSeg->asc = asc;
   pStrSeg->des = des;
   pStrSeg->min_lbearing = min_lbearing;
   pStrSeg->max_rextra = max_rextra;

   pStrSeg->underline_on = (underline_on & 0x1);
   pStrSeg->overline_on = ((underline_on & 0x2) == 0x2);
   pStrSeg->double_byte = double_byte;
   pStrSeg->double_byte_mod_bytes = db_mod_bytes;
   pStrSeg->double_byte_vertical = db_vertical;
   pStrSeg->direction = direction;
   pStrSeg->dontreencode = dontreencode;

   *ppStrSeg = pStrSeg;

   return TRUE;
}

static
int ReadStrBlock(FP, pOwnerMiniLine, ppStrBlock)
   FILE *FP;
   MiniLineInfo *pOwnerMiniLine;
   StrBlockInfo **ppStrBlock;
{
   int type=INVALID, w=0, asc=0, des=0, min_lbearing=0, max_rextra=0;
   int special_char_w=0, has_sup=0, has_sub=0;
   char *s=NULL, *line=NULL;
   StrBlockInfo *pStrBlock=NULL;
   StrSegInfo *pStrSeg=NULL;
   MiniLinesInfo *minilines=NULL;

   *ppStrBlock = NULL;
   if ((line=UtilGetALine(FP)) == NULL) {
      sprintf(gszMsgBox, TgLoadString(STID_UNEXPECTED_EOF_IN_ABORT_READ),
            scanFileName, scanLineNum, "ReadStrBlock()");
      if (PRTGIF) {
         fprintf(stderr, "%s\n", gszMsgBox);
      } else {
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      }
      return FALSE;
   }
   scanLineNum++;

   if (*line == ']') {
      free(line);
      return TRUE;
   }
   s = FindChar((int)'(', line);
   InitScan(s, ", \t\n");

   if (fileVersion >= 37) {
      if (GETINT("str_block", type,           "type") == INVALID ||
          GETINT("str_block", w,              "w") == INVALID ||
          GETINT("str_block", asc,            "asc") == INVALID ||
          GETINT("str_block", des,            "des") == INVALID ||
          GETINT("str_block", min_lbearing,   "min_lbearing") == INVALID ||
          GETINT("str_block", max_rextra,     "max_rextra") == INVALID ||
          GETINT("str_block", special_char_w, "special_char_w") == INVALID ||
          GETINT("str_block", has_sup,        "has_sup") == INVALID ||
          GETINT("str_block", has_sub,        "has_sub") == INVALID) {
         return FALSE;
      }
   }
   pStrBlock = NewStrBlock();
   pStrBlock->type = type;
   pStrBlock->w = w;
   pStrBlock->asc = asc;
   pStrBlock->des = des;
   pStrBlock->min_lbearing = min_lbearing;
   pStrBlock->max_rextra = max_rextra;
   pStrBlock->special_char_w = special_char_w;

   free(line);

   switch (type) {
   case SB_SIMPLE:
      pStrSeg = NULL;
      if (!ReadStrSeg(FP, &pStrSeg)) {
         return FALSE;
      }
      pStrBlock->seg = pStrSeg;
      break;

   case SB_CHAR_SPACE: break;

   case SB_SUPSUB_LEFT:
   case SB_SUPSUB_CENTER:
   case SB_SUPSUB_RIGHT:
      if (has_sup) {
         minilines = NULL;
         if (!ReadMiniLines(FP, pStrBlock, &minilines) || minilines == NULL) {
            return FALSE;
         }
         pStrBlock->sup = minilines;
      }
      if (has_sub) {
         minilines = NULL;
         if (!ReadMiniLines(FP, pStrBlock, &minilines) || minilines == NULL) {
            return FALSE;
         }
         pStrBlock->sub = minilines;
      }
      if (type == SB_SUPSUB_CENTER) {
         pStrSeg = NULL;
         if (!ReadStrSeg(FP, &pStrSeg)) {
            return FALSE;
         }
         pStrBlock->seg = pStrSeg;
      }
      if ((line=UtilGetALine(FP)) == NULL) {
         sprintf(gszMsgBox, TgLoadString(STID_UNEXPECTED_EOF_IN_ABORT_READ),
               scanFileName, scanLineNum, "ReadStrBlock()");
         if (PRTGIF) {
            fprintf(stderr, "%s\n", gszMsgBox);
         } else {
            MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
         }
         return FALSE;
      }
      scanLineNum++;

      if (*line != ']') {
         sprintf(gszMsgBox, TgLoadString(STID_UNEXPECTED_TOK_IN_ABORT_READ),
               scanFileName, scanLineNum, ']', "ReadStrBlock()");
         if (PRTGIF) {
            fprintf(stderr, "%s\n", gszMsgBox);
         } else {
            MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
         }
         return FALSE;
      }
      free(line);
      break;
   }
   pStrBlock->owner_mini_line = pOwnerMiniLine;

   *ppStrBlock = pStrBlock;

   return TRUE;
}

static
int ReadMiniLine(FP, pOwnerMiniLines, ppMiniLine)
   FILE *FP;
   MiniLinesInfo *pOwnerMiniLines;
   MiniLineInfo **ppMiniLine;
{
   int w=0, asc=0, des=0, min_lbearing=0, max_rextra=0, v_gap=0;
   char *s=NULL, *line=NULL;
   MiniLineInfo *pMiniLine=NULL;
   StrBlockInfo *pFirstStrBlock=NULL, *pLastStrBlock=NULL;

   *ppMiniLine = NULL;
   if ((line=UtilGetALine(FP)) == NULL) {
      sprintf(gszMsgBox, TgLoadString(STID_UNEXPECTED_EOF_IN_ABORT_READ),
            scanFileName, scanLineNum, "ReadMiniLine()");
      if (PRTGIF) {
         fprintf(stderr, "%s\n", gszMsgBox);
      } else {
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      }
      return FALSE;
   }
   scanLineNum++;

   if (*line == ']') {
      free(line);
      return TRUE;
   }
   s = FindChar((int)'(', line);
   InitScan(s, ", \t\n");

   if (fileVersion >= 37) {
      if (GETINT("mini_line", w,            "w") == INVALID ||
          GETINT("mini_line", asc,          "asc") == INVALID ||
          GETINT("mini_line", des,          "des") == INVALID ||
          GETINT("mini_line", min_lbearing, "min_lbearing") == INVALID ||
          GETINT("mini_line", max_rextra,   "max_rextra") == INVALID ||
          GETINT("mini_line", v_gap,        "v_gap") == INVALID) {
         return FALSE;
      }
   }
   pMiniLine = NewMiniLine();
   pMiniLine->w = w;
   pMiniLine->asc = asc;
   pMiniLine->des = des;
   pMiniLine->min_lbearing = min_lbearing;
   pMiniLine->max_rextra = max_rextra;
   pMiniLine->v_gap = v_gap;

   free(line);

   for (;;) {
      StrBlockInfo *pStrBlock=NULL;

      if (!ReadStrBlock(FP, pMiniLine, &pStrBlock)) {
         FreeStrBlockList(pFirstStrBlock);
         return FALSE;
      }
      if (pStrBlock == NULL) {
         /* shouldn't come here if num_lines is correct */
         break;
      }
      InsertStrBlock(pLastStrBlock, NULL, pStrBlock, &pFirstStrBlock,
            &pLastStrBlock);
   }
   pMiniLine->first_block = pFirstStrBlock;
   pMiniLine->last_block = pLastStrBlock;
   pMiniLine->owner_minilines = pOwnerMiniLines;

   *ppMiniLine = pMiniLine;

   return TRUE;
}

int ReadMiniLines(FP, pOwnerBlock, ppMiniLines)
   FILE *FP;
   StrBlockInfo *pOwnerBlock;
   MiniLinesInfo **ppMiniLines;
{
   int w=0, h=0, min_lbearing=0, max_rextra=0;
   int just=0, v_space=0, baseline_offset=0;
   char *s=NULL, *line=NULL;
   MiniLinesInfo *minilines=NULL;
   MiniLineInfo *pFirstMiniLine=NULL, *pLastMiniLine=NULL;

   *ppMiniLines = NULL;
   if ((line=UtilGetALine(FP)) == NULL) {
      sprintf(gszMsgBox, TgLoadString(STID_UNEXPECTED_EOF_IN_ABORT_READ),
            scanFileName, scanLineNum, "ReadMiniLines()");
      if (PRTGIF) {
         fprintf(stderr, "%s\n", gszMsgBox);
      } else {
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      }
      return FALSE;
   }
   scanLineNum++;

   s = FindChar((int)'(', line);
   InitScan(s, ", \t\n");

   if (fileVersion >= 37) {
      if (GETINT("minilines", w,               "w") == INVALID ||
          GETINT("minilines", h,               "h") == INVALID ||
          GETINT("minilines", min_lbearing,    "min_lbearing") == INVALID ||
          GETINT("minilines", max_rextra,      "max_rextra") == INVALID ||
          GETINT("minilines", just,            "just") == INVALID ||
          GETINT("minilines", v_space,         "v_space") == INVALID ||
          GETINT("minilines", baseline_offset, "baseline_offset") == INVALID) {
         return FALSE;
      }
   }
   minilines = NewMiniLines();
   minilines->w = w;
   minilines->h = h;
   minilines->min_lbearing = min_lbearing;
   minilines->max_rextra = max_rextra;
   minilines->just = just;
   minilines->v_space = v_space;
   minilines->baseline_offset = baseline_offset;

   free(line);

   for (;;) {
      MiniLineInfo *pMiniLine=NULL;

      if (!ReadMiniLine(FP, minilines, &pMiniLine)) {
         FreeMiniLineList(pFirstMiniLine);
         return FALSE;
      }
      if (pMiniLine == NULL) {
         /* shouldn't come here if num_lines is correct */
         break;
      }
      if (pMiniLine->first_block == NULL) {
         StrBlockInfo *pStrBlock=CreateStrBlockFromString("", pMiniLine);

         pMiniLine->first_block = pMiniLine->last_block = pStrBlock;
      }
      InsertMiniLine(pLastMiniLine, NULL, pMiniLine, &pFirstMiniLine,
            &pLastMiniLine);
   }
   minilines->first = pFirstMiniLine;
   minilines->last = pLastMiniLine;
   minilines->owner_block = pOwnerBlock;

   *ppMiniLines = minilines;

   return TRUE;
}

static
int ReadTextLines(FP, ObjPtr, text_ptr, color_str, num_lines, has_ps_bitmap,
      cur_sb_font, cur_db_font, double_byte, db_mod_bytes, db_vertical,
      direction, x, baseline_y, text_w, pn_max_len, pn_max_h)
   FILE *FP;
   struct ObjRec **ObjPtr;
   struct TextRec *text_ptr;
   char *color_str;
   int num_lines, has_ps_bitmap, cur_sb_font, cur_db_font;
   int double_byte, db_mod_bytes, db_vertical, direction;
   int x, baseline_y, *pn_max_len, *pn_max_h;
   /*
    * (Note: text_w is only used for fileVersion <= 36)
    */
{
   int len=0;

   /*
    * Set readingTextObject to TRUE to *enable* the setting of the read_only
    *       fields in the string segments and the text object.
    */
   readingTextObject = TRUE;
   if (deserializingFile) ReadCreatorID(FP, ObjPtr);
   if (fileVersion <= 36) {
      int i=0, line_h=canvasFontAsc+canvasFontDes+textVSpace;

      for (i=0; i < num_lines; i++) {
         int new_alloc=FALSE;
         char *tmp_str=NULL, *s=NULL, *line=NULL;
         MiniLineInfo *pMiniLine=NULL;
         StrSegInfo *pStrSeg=NULL;
         RecalcMetricsInfo rmi;

         if ((line=UtilGetALine(FP)) == NULL) {
            sprintf(gszMsgBox, TgLoadString(STID_UNEXPECTED_EOF_IN_ABORT_READ),
                  scanFileName, scanLineNum, "ReadTextLines()");
            if (PRTGIF) {
               fprintf(stderr, "%s\n", gszMsgBox);
            } else {
               MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
            }
            return FALSE;
         }
         scanLineNum++;

         tmp_str = FindChar((int)'"', line);
         s = ReadString(tmp_str);
         *(--s) = '\0';

         pMiniLine = NewMiniLine();
         if (double_byte) {
            /*
             * Note: should not get here for PRTGIF because
             *       double_byte is always 0 in any released version
             *       of tgif for fileVersion <= 36.
             */
            struct StrRec *pStr=NULL, *pStr1=NULL;

            pStr = SegmentDoubleByteString(tmp_str);
            for (pStr1=pStr; pStr1 != NULL; pStr1=pStr1->next) {
               int str_seg_double_byte=((*pStr1->dyn_str.s) & 0x80);
               StrBlockInfo *pStrBlock=NewStrBlock();

               pStrBlock->prev = pMiniLine->last_block;
               pStrBlock->next = NULL;
               if (pMiniLine->first_block == NULL) {
                  pMiniLine->first_block = pStrBlock;
               } else {
                  pMiniLine->last_block->next = pStrBlock;
               }
               pMiniLine->last_block = pStrBlock;

               pStrBlock->type = SB_SIMPLE;
               pStrBlock->seg = pStrSeg = NewStrSeg();
               DynStrSet(&pStrSeg->dyn_str, pStr1->dyn_str.s);

               pStrSeg->color = QuickFindColorIndex(NULL, color_str, &new_alloc,
                     TRUE);
               UtilStrCpyN(pStrSeg->color_str, sizeof(pStrSeg->color_str),
                     color_str);

               curFont = (str_seg_double_byte ? cur_db_font : cur_sb_font);
               if (!PRTGIF) {
                  SetCanvasFont();
                  if (canvasFontSize != INVALID &&
                        SzUnitToFontSize(curSzUnit) != canvasFontSize) {
                     SetFileModified(TRUE);
                     curSzUnit = FontSizeToSzUnit(canvasFontSize);
                  }
               }
               pStrSeg->double_byte = canvasFontDoubleByte;
               pStrSeg->double_byte_mod_bytes = canvasFontDoubleByteModBytes;
               pStrSeg->double_byte_vertical = canvasFontDoubleByteVertical;
               pStrSeg->font = curFont;
               pStrSeg->style = curStyle;
               pStrSeg->sz_unit = curSzUnit;
               pStrSeg->direction = direction;
               pStrSeg->read_only = text_ptr->read_only;
               pStrSeg->orig_w = text_ptr->orig_w; /* valid if read_only */
               pStrSeg->orig_h = text_ptr->orig_h; /* valid if read_only */
               pStrSeg->underline_on = curUnderlineOn;
               pStrSeg->overline_on = curOverlineOn;
               pStrSeg->asc = canvasFontAsc;
               pStrSeg->des = canvasFontDes;
               /*
                * pStrSeg->w = text_w;
                *
                * Well, this doesn't really work here for PRTGIF.  But since
                *       double-byte for version <= 36 is really not supported,
                *       (and the file does not contain enough information)
                *       this bug really cannot be fixed.
                */
            }
            FreeStrList(pStr);
         } else {
            pMiniLine->first_block = pMiniLine->last_block = NewStrBlock();
            pMiniLine->first_block->type = SB_SIMPLE;
            pMiniLine->first_block->seg = pStrSeg = NewStrSeg();
            DynStrSet(&pStrSeg->dyn_str, tmp_str);

            pStrSeg->color = QuickFindColorIndex(NULL, color_str, &new_alloc,
                  TRUE);
            UtilStrCpyN(pStrSeg->color_str, sizeof(pStrSeg->color_str),
                  color_str);

            if (!PRTGIF) {
               SetCanvasFont();
               if (canvasFontSize != INVALID &&
                     SzUnitToFontSize(curSzUnit) != canvasFontSize) {
                  SetFileModified(TRUE);
                  curSzUnit = FontSizeToSzUnit(canvasFontSize);
               }
            }
            pStrSeg->font = curFont;
            pStrSeg->style = curStyle;
            pStrSeg->sz_unit = curSzUnit;
            pStrSeg->direction = direction;
            pStrSeg->read_only = text_ptr->read_only;
            pStrSeg->orig_w = text_ptr->orig_w; /* valid if read_only */
            pStrSeg->orig_h = text_ptr->orig_h; /* valid if read_only */
            pStrSeg->underline_on = curUnderlineOn;
            pStrSeg->overline_on = curOverlineOn;
            pStrSeg->asc = canvasFontAsc;
            pStrSeg->des = canvasFontDes;
            pStrSeg->w = text_w;
         }
         memset(&rmi, 0, sizeof(RecalcMetricsInfo));
         rmi.x = rmi.orig_x = x;
         rmi.orig_baseline_y = text_ptr->baseline_y;
         rmi.baseline_y = baseline_y;

         if (PRTGIF) {
            dontRecalcStrSegMetrics = TRUE;
            RecalcMiniLineMetrics(pMiniLine, &rmi);
            dontRecalcStrSegMetrics = FALSE;
         } else {
            PushCurFont();
            if (!RecalcMiniLineMetrics(pMiniLine, &rmi)) {
               text_ptr->read_only = TRUE;
            }
            PopCurFont();
         }
         InsertMiniLine(lastMiniLine, NULL, pMiniLine, &firstMiniLine,
               &lastMiniLine);

         if (PRTGIF) {
            len = strlen(tmp_str); /* assume string width = 1 pixel per char */
            if (len > (*pn_max_len)) (*pn_max_len) = len;
         }
         baseline_y += line_h;
         free(line);
      }
      *pn_max_h = baseline_y - text_ptr->baseline_y;
      if (has_ps_bitmap) {
         sprintf(gszMsgBox, TgLoadString(STID_ILLEGAL_FIELD_IN_ABORT_READ),
               scanFileName, scanLineNum, "has_ps_bitmap", "ReadTextLines()");
         if (PRTGIF) {
            fprintf(stderr, "%s\n", gszMsgBox);
         } else {
            MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
         }
         return FALSE;
      }
   } else {
      MiniLinesInfo *minilines=NULL;
      MiniLineInfo *pMiniLine=NULL;
      int v_space=0;

      PushCurFont();
      ReadMiniLines(FP, NULL, &minilines);
      PopCurFont();
      if (minilines == NULL) {
         return FALSE;
      }
      firstMiniLine = minilines->first;
      lastMiniLine = minilines->last;
      v_space = minilines->v_space;
      free(minilines);

      for (pMiniLine=firstMiniLine; pMiniLine != NULL;
            pMiniLine=pMiniLine->next) {
         RecalcMetricsInfo rmi;

         memset(&rmi, 0, sizeof(RecalcMetricsInfo));
         rmi.x = rmi.orig_x = x;
         rmi.orig_baseline_y = text_ptr->baseline_y;
         rmi.baseline_y = baseline_y;

         if (PRTGIF) {
            dontRecalcStrSegMetrics = TRUE;
            RecalcMiniLineMetrics(pMiniLine, &rmi);
            dontRecalcStrSegMetrics = FALSE;

            len = pMiniLine->w;
            if (len > (*pn_max_len)) (*pn_max_len) = len;
         } else {
            PushCurFont();
            if (!RecalcMiniLineMetrics(pMiniLine, &rmi)) {
               text_ptr->read_only = TRUE;
            }
            PopCurFont();
         }
         baseline_y += pMiniLine->des + v_space;
         if (pMiniLine->next != NULL) {
            baseline_y += pMiniLine->next->asc;
         }
      }
      *pn_max_h = baseline_y - text_ptr->baseline_y;
   }
   readingTextObject = FALSE;

   return TRUE;
}

static
void OldStyleRotateGetFinalOBBoxLtXY(ObjPtr, rotate, text_just, pnFinalLtX,
      pnFinalLtY)
   struct ObjRec *ObjPtr;
   int rotate, text_just, *pnFinalLtX, *pnFinalLtY;
   /*
    * For fileVersion >= 33, rotate is always ROTATE0.  So we need
    *       to convert the old format to the new one.  The way we will
    *       do it is to:
    * 1) Calculate where the final obbox.ltx and obbox.lty will be.
    * 2) Rotate the object.
    * 3) Move the object to matched the final position calculated in (1).
    */
{
   /*
    * This this point, coordinates in ObjPtr have been computed using ROTATE0.
    * Remember, positive rotate is clockwise.
    * In the old style, for JUST_L, pivot is left top corner, for JUST_C, pivot
    *       is center top corner, and for JUST_R, pivot is right top corner.
    */
   int x=ObjPtr->x, y=ObjPtr->y, ltx=ObjPtr->obbox.ltx, lty=ObjPtr->obbox.lty;
   int w=ObjPtr->obbox.rbx-ltx, h=ObjPtr->obbox.rby-lty;

   switch (rotate) {
   case ROTATE90:
      switch (text_just) {
      case JUST_L: ltx=x-h; lty=y; break;
      case JUST_C: ltx=x-h; lty=y-(w>>1); break;
      case JUST_R: ltx=x-h; lty=y-w; break;
      }
      break;
   case ROTATE180:
      switch (text_just) {
      case JUST_L: ltx=x-(w-(w>>1)); lty=y-h; break;
      case JUST_C: ltx=x-w;          lty=y-h; break;
      case JUST_R: ltx=x;            lty=y-h; break;
      }
      break;
   case ROTATE270:
      switch (text_just) {
      case JUST_L: ltx=x; lty=y-w; break;
      case JUST_C: ltx=x; lty=y-(w-(w>>1)); break;
      case JUST_R: ltx=x; lty=y; break;
      }
      break;
   }
   *pnFinalLtX = ltx;
   *pnFinalLtY = lty;
}

void ReadTextObj(FP, Inbuf, ObjPtr)
   FILE *FP;
   char *Inbuf;
   struct ObjRec **ObjPtr;
{
   int max_len=0, max_h=0, len;
   struct TextRec *text_ptr;
   char color_str[40], bg_color_str[40], *s, *c_ptr, font_str[MAXSTRING];
   char sb_font_str[MAXSTRING], inbuf[MAXSTRING+1];
   char cust_font_name[MAXSTRING+1]; /* this is not really used any more */
   int has_ps_bitmap=FALSE;
   int cur_sb_font=(-1), cur_db_font=(-1), baseline_y=0;
   int num_lines=0, x, y, font=0, style=0, size=0, sz_unit=0, id=0;
   int text_just=0, rotate=0, pen=0, rotation=0, adj_bbox_for_third_party=FALSE;
   int obbox_w=0, obbox_h=0, dpi=0, asc=0, des=0, fill=0, trans_pat=FALSE;
   int locked=FALSE, max_rextra=0, v_space=0, new_alloc=FALSE;
   int underline_on=FALSE, underline=2, min_lbearing=0;
   int underline_y_offset=0, overline_y_offset=0;
   int double_byte=FALSE, compressed=FALSE;
   int db_mod_bytes=FALSE, db_vertical=FALSE;
   int transformed=FALSE, invisible=FALSE;
   int direction=FontLeftToRight, real_x=0, real_y=0;
   int first_block_asc=0, first_block_des=0, text_w=0, text_h=0;
   struct XfrmMtrxRec *ctm=NULL;
   struct BBRec orig_obbox, orig_bbox;

   dpi = FONT_DPI_75;
   fill = NONEPAT;
   v_space = 0;
   *cust_font_name = '\0';
   *font_str = *sb_font_str = '\0';

   *ObjPtr = NULL;

   s = FindChar((int)'(', Inbuf);
   s = ParseStr(s, (int)',', color_str, sizeof(color_str));

   InitScan(s, ", \t\n");

   rotate = 0;
   pen = 1;
   rotation = 0;
   if (fileVersion <= 2) {
      if (GETINT("text", x,         "x") == INVALID ||
          GETINT("text", y,         "y") == INVALID ||
          GETINT("text", font,      "font") == INVALID ||
          GETINT("text", style,     "style") == INVALID ||
          GETINT("text", size,      "size") == INVALID ||
          GETINT("text", num_lines, "num_lines") == INVALID ||
          GETINT("text", text_just, "text_just") == INVALID) {
         return;
      }
      id = objId++;
   } else if (fileVersion <= 6) {
      if (GETINT("text", x,         "x") == INVALID ||
          GETINT("text", y,         "y") == INVALID ||
          GETINT("text", font,      "font") == INVALID ||
          GETINT("text", style,     "style") == INVALID ||
          GETINT("text", size,      "size") == INVALID ||
          GETINT("text", num_lines, "num_lines") == INVALID ||
          GETINT("text", text_just, "text_just") == INVALID ||
          GETINT("text", rotate,    "rotate") == INVALID ||
          GETINT("text", pen,       "pen") == INVALID) {
         return;
      }
      id = objId++;
   } else if (fileVersion <= 7) {
      if (GETINT("text", x,         "x") == INVALID ||
          GETINT("text", y,         "y") == INVALID ||
          GETINT("text", font,      "font") == INVALID ||
          GETINT("text", style,     "style") == INVALID ||
          GETINT("text", size,      "size") == INVALID ||
          GETINT("text", num_lines, "num_lines") == INVALID ||
          GETINT("text", text_just, "text_just") == INVALID ||
          GETINT("text", rotate,    "rotate") == INVALID ||
          GETINT("text", pen,       "pen") == INVALID ||
          GETINT("text", obbox_w,   "bbox w") == INVALID ||
          GETINT("text", obbox_h,   "bbox h") == INVALID) {
         return;
      }
      id = objId++;
   } else if (fileVersion <= 9) {
      if (GETINT("text", x,         "x") == INVALID ||
          GETINT("text", y,         "y") == INVALID ||
          GETINT("text", font,      "font") == INVALID ||
          GETINT("text", style,     "style") == INVALID ||
          GETINT("text", size,      "size") == INVALID ||
          GETINT("text", num_lines, "num_lines") == INVALID ||
          GETINT("text", text_just, "text_just") == INVALID ||
          GETINT("text", rotate,    "rotate") == INVALID ||
          GETINT("text", pen,       "pen") == INVALID ||
          GETINT("text", obbox_w,   "bbox w") == INVALID ||
          GETINT("text", obbox_h,   "bbox h") == INVALID ||
          GETINT("text", id,        "id") == INVALID ||
          GETINT("text", dpi,       "dpi") == INVALID) {
         return;
      }
      if (id >= objId) objId = id+1;
   } else if (fileVersion <= 10) {
      if (GETINT("text", x,         "x") == INVALID ||
          GETINT("text", y,         "y") == INVALID ||
          GETINT("text", font,      "font") == INVALID ||
          GETINT("text", style,     "style") == INVALID ||
          GETINT("text", size,      "size") == INVALID ||
          GETINT("text", num_lines, "num_lines") == INVALID ||
          GETINT("text", text_just, "text_just") == INVALID ||
          GETINT("text", rotate,    "rotate") == INVALID ||
          GETINT("text", pen,       "pen") == INVALID ||
          GETINT("text", obbox_w,   "bbox w") == INVALID ||
          GETINT("text", obbox_h,   "bbox h") == INVALID ||
          GETINT("text", id,        "id") == INVALID ||
          GETINT("text", dpi,       "dpi") == INVALID ||
          GETINT("text", asc,       "ascent") == INVALID ||
          GETINT("text", des,       "descent") == INVALID) {
         return;
      }
      if (id >= objId) objId = id+1;
   } else if (fileVersion <= 12) {
      if (GETINT("text", x,         "x") == INVALID ||
          GETINT("text", y,         "y") == INVALID ||
          GETINT("text", font,      "font") == INVALID ||
          GETINT("text", style,     "style") == INVALID ||
          GETINT("text", size,      "size") == INVALID ||
          GETINT("text", num_lines, "num_lines") == INVALID ||
          GETINT("text", text_just, "text_just") == INVALID ||
          GETINT("text", rotate,    "rotate") == INVALID ||
          GETINT("text", pen,       "pen") == INVALID ||
          GETINT("text", obbox_w,   "bbox w") == INVALID ||
          GETINT("text", obbox_h,   "bbox h") == INVALID ||
          GETINT("text", id,        "id") == INVALID ||
          GETINT("text", dpi,       "dpi") == INVALID ||
          GETINT("text", asc,       "ascent") == INVALID ||
          GETINT("text", des,       "descent") == INVALID ||
          GETINT("text", fill,      "fill") == INVALID) {
         return;
      }
      if (id >= objId) objId = id+1;
   } else if (fileVersion <= 13) {
      if (GETINT("text", x,         "x") == INVALID ||
          GETINT("text", y,         "y") == INVALID ||
          GETINT("text", font,      "font") == INVALID ||
          GETINT("text", style,     "style") == INVALID ||
          GETINT("text", size,      "size") == INVALID ||
          GETINT("text", num_lines, "num_lines") == INVALID ||
          GETINT("text", text_just, "text_just") == INVALID ||
          GETINT("text", rotate,    "rotate") == INVALID ||
          GETINT("text", pen,       "pen") == INVALID ||
          GETINT("text", obbox_w,   "bbox w") == INVALID ||
          GETINT("text", obbox_h,   "bbox h") == INVALID ||
          GETINT("text", id,        "id") == INVALID ||
          GETINT("text", dpi,       "dpi") == INVALID ||
          GETINT("text", asc,       "ascent") == INVALID ||
          GETINT("text", des,       "descent") == INVALID ||
          GETINT("text", fill,      "fill") == INVALID ||
          GETINT("text", v_space,   "vertical spacing") == INVALID) {
         return;
      }
      if (id >= objId) objId = id+1;
   } else if (fileVersion <= 25) {
      if (GETINT("text", x,         "x") == INVALID ||
          GETINT("text", y,         "y") == INVALID ||
          GETINT("text", font,      "font") == INVALID ||
          GETINT("text", style,     "style") == INVALID ||
          GETINT("text", size,      "size") == INVALID ||
          GETINT("text", num_lines, "num_lines") == INVALID ||
          GETINT("text", text_just, "text_just") == INVALID ||
          GETINT("text", rotate,    "rotate") == INVALID ||
          GETINT("text", pen,       "pen") == INVALID ||
          GETINT("text", obbox_w,   "bbox w") == INVALID ||
          GETINT("text", obbox_h,   "bbox h") == INVALID ||
          GETINT("text", id,        "id") == INVALID ||
          GETINT("text", dpi,       "dpi") == INVALID ||
          GETINT("text", asc,       "ascent") == INVALID ||
          GETINT("text", des,       "descent") == INVALID ||
          GETINT("text", fill,      "fill") == INVALID ||
          GETINT("text", v_space,   "vertical spacing") == INVALID ||
          GETINT("text", rotation,  "rotation") == INVALID) {
         return;
      }
      if (id >= objId) objId = id+1;
   } else if (fileVersion <= 29) {
      if (GETINT("text", x,         "x") == INVALID ||
          GETINT("text", y,         "y") == INVALID ||
          GETINT("text", font,      "font") == INVALID ||
          GETINT("text", style,     "style") == INVALID ||
          GETINT("text", size,      "size") == INVALID ||
          GETINT("text", num_lines, "num_lines") == INVALID ||
          GETINT("text", text_just, "text_just") == INVALID ||
          GETINT("text", rotate,    "rotate") == INVALID ||
          GETINT("text", pen,       "pen") == INVALID ||
          GETINT("text", obbox_w,   "bbox w") == INVALID ||
          GETINT("text", obbox_h,   "bbox h") == INVALID ||
          GETINT("text", id,        "id") == INVALID ||
          GETINT("text", dpi,       "dpi") == INVALID ||
          GETINT("text", asc,       "ascent") == INVALID ||
          GETINT("text", des,       "descent") == INVALID ||
          GETINT("text", fill,      "fill") == INVALID ||
          GETINT("text", v_space,   "vertical spacing") == INVALID ||
          GETINT("text", rotation,  "rotation") == INVALID ||
          GETINT("text", locked,    "locked") == INVALID) {
         return;
      }
      if (id >= objId) objId = id+1;
   } else if (fileVersion <= 32) {
      if (GETINT("text", x,         "x") == INVALID ||
          GETINT("text", y,         "y") == INVALID ||
          GETSTR("text", font_str,  "font_str") == INVALID ||
          GETINT("text", style,     "style") == INVALID ||
          GETINT("text", size,      "size") == INVALID ||
          GETINT("text", num_lines, "num_lines") == INVALID ||
          GETINT("text", text_just, "text_just") == INVALID ||
          GETINT("text", rotate,    "rotate") == INVALID ||
          GETINT("text", pen,       "pen") == INVALID ||
          GETINT("text", obbox_w,   "bbox w") == INVALID ||
          GETINT("text", obbox_h,   "bbox h") == INVALID ||
          GETINT("text", id,        "id") == INVALID ||
          GETINT("text", dpi,       "dpi") == INVALID ||
          GETINT("text", asc,       "ascent") == INVALID ||
          GETINT("text", des,       "descent") == INVALID ||
          GETINT("text", fill,      "fill") == INVALID ||
          GETINT("text", v_space,   "vertical spacing") == INVALID ||
          GETINT("text", rotation,  "rotation") == INVALID ||
          GETINT("text", locked,    "locked") == INVALID) {
         return;
      }
      if (id >= objId) objId = id+1;
   } else if (fileVersion <= 33) {
      if (GETINT("text", x,              "x") == INVALID ||
          GETINT("text", y,              "y") == INVALID ||
          GETSTR("text", font_str,       "font_str") == INVALID ||
          GETINT("text", style,          "style") == INVALID ||
          GETINT("text", size,           "size") == INVALID ||
          GETINT("text", num_lines,      "num_lines") == INVALID ||
          GETINT("text", text_just,      "text_just") == INVALID ||
          GETINT("text", rotate,         "rotate") == INVALID ||
          GETINT("text", pen,            "pen") == INVALID ||
          GETINT("text", obbox_w,        "bbox w") == INVALID ||
          GETINT("text", obbox_h,        "bbox h") == INVALID ||
          GETINT("text", id,             "id") == INVALID ||
          GETINT("text", dpi,            "dpi") == INVALID ||
          GETINT("text", asc,            "ascent") == INVALID ||
          GETINT("text", des,            "descent") == INVALID ||
          GETINT("text", fill,           "fill") == INVALID ||
          GETINT("text", v_space,        "vertical spacing") == INVALID ||
          GETINT("text", rotation,       "rotation") == INVALID ||
          GETINT("text", locked,         "locked") == INVALID ||
          GETINT("text", underline_on,   "underline_on") == INVALID ||
          GETINT("text", underline,      "underline") == INVALID ||
          GETINT("text", min_lbearing,   "min_lbearing") == INVALID ||
          GETINT("text", max_rextra,     "max_rextra") == INVALID ||
          GETINT("text", double_byte,    "double_byte") == INVALID ||
          GETINT("text", direction,      "direction") == INVALID ||
          GETSTR("text", cust_font_name, "cust_font_name") == INVALID ||
          GETINT("text", compressed,     "compressed") == INVALID ||
          GETINT("text", transformed,    "transformed") == INVALID ||
          GETINT("text", invisible,      "invisible") == INVALID) {
         return;
      }
      if (id >= objId) objId = id+1;
      len = strlen(cust_font_name);
      if (len >= 2 && *cust_font_name=='"' && cust_font_name[len-1]=='"') {
         for (c_ptr=cust_font_name; c_ptr[2] != '\0'; c_ptr++) {
            *c_ptr = c_ptr[1];
         }
         *c_ptr = '\0';
      }
   } else if (fileVersion <= 34) {
      if (GETINT("text", x,              "x") == INVALID ||
          GETINT("text", y,              "y") == INVALID ||
          GETSTR("text", font_str,       "font_str") == INVALID ||
          GETINT("text", style,          "style") == INVALID ||
          GETINT("text", size,           "size") == INVALID ||
          GETINT("text", num_lines,      "num_lines") == INVALID ||
          GETINT("text", text_just,      "text_just") == INVALID ||
          GETINT("text", rotate,         "rotate") == INVALID ||
          GETINT("text", pen,            "pen") == INVALID ||
          GETINT("text", obbox_w,        "bbox w") == INVALID ||
          GETINT("text", obbox_h,        "bbox h") == INVALID ||
          GETINT("text", id,             "id") == INVALID ||
          GETINT("text", dpi,            "dpi") == INVALID ||
          GETINT("text", asc,            "ascent") == INVALID ||
          GETINT("text", des,            "descent") == INVALID ||
          GETINT("text", fill,           "fill") == INVALID ||
          GETINT("text", v_space,        "vertical spacing") == INVALID ||
          GETINT("text", rotation,       "rotation") == INVALID ||
          GETINT("text", locked,         "locked") == INVALID ||
          GETINT("text", underline_on,   "underline_on") == INVALID ||
          GETINT("text", underline,      "underline") == INVALID ||
          GETINT("text", min_lbearing,   "min_lbearing") == INVALID ||
          GETINT("text", max_rextra,     "max_rextra") == INVALID ||
          GETINT("text", double_byte,    "double_byte") == INVALID ||
          GETINT("text", direction,      "direction") == INVALID ||
          GETSTR("text", cust_font_name, "cust_font_name") == INVALID ||
          GETINT("text", compressed,     "compressed") == INVALID ||
          GETINT("text", transformed,    "transformed") == INVALID ||
          GETINT("text", invisible,      "invisible") == INVALID ||
          GETINT("text", has_ps_bitmap,  "has_ps_bitmap") == INVALID) {
         return;
      }
      if (id >= objId) objId = id+1;
      len = strlen(cust_font_name);
      if (len >= 2 && *cust_font_name=='"' && cust_font_name[len-1]=='"') {
         for (c_ptr=cust_font_name; c_ptr[2] != '\0'; c_ptr++) {
            *c_ptr = c_ptr[1];
         }
         *c_ptr = '\0';
      }
   } else if (fileVersion <= 35) {
      if (GETINT("text", x,              "x") == INVALID ||
          GETINT("text", y,              "y") == INVALID ||
          GETSTR("text", font_str,       "font_str") == INVALID ||
          GETINT("text", style,          "style") == INVALID ||
          GETINT("text", size,           "size") == INVALID ||
          GETINT("text", num_lines,      "num_lines") == INVALID ||
          GETINT("text", text_just,      "text_just") == INVALID ||
          GETINT("text", rotate,         "rotate") == INVALID ||
          GETINT("text", pen,            "pen") == INVALID ||
          GETINT("text", obbox_w,        "bbox w") == INVALID ||
          GETINT("text", obbox_h,        "bbox h") == INVALID ||
          GETINT("text", id,             "id") == INVALID ||
          GETINT("text", dpi,            "dpi") == INVALID ||
          GETINT("text", asc,            "ascent") == INVALID ||
          GETINT("text", des,            "descent") == INVALID ||
          GETINT("text", fill,           "fill") == INVALID ||
          GETINT("text", v_space,        "vertical spacing") == INVALID ||
          GETINT("text", rotation,       "rotation") == INVALID ||
          GETINT("text", locked,         "locked") == INVALID ||
          GETINT("text", underline_on,   "underline_on") == INVALID ||
          GETINT("text", underline,      "underline") == INVALID ||
          GETINT("text", min_lbearing,   "min_lbearing") == INVALID ||
          GETINT("text", max_rextra,     "max_rextra") == INVALID ||
          GETINT("text", double_byte,    "double_byte") == INVALID ||
          GETINT("text", direction,      "direction") == INVALID ||
          GETSTR("text", cust_font_name, "cust_font_name") == INVALID ||
          GETINT("text", compressed,     "compressed") == INVALID ||
          GETINT("text", transformed,    "transformed") == INVALID ||
          GETINT("text", invisible,      "invisible") == INVALID ||
          GETINT("text", has_ps_bitmap,  "has_ps_bitmap") == INVALID ||
          GETINT("text", db_mod_bytes,   "double_byte_mod_bytes") == INVALID ||
          GETINT("text", db_vertical,    "double_byte_vertical") == INVALID ||
          GETINT("text", trans_pat,      "trans_pat") == INVALID) {
         return;
      }
      if (id >= objId) objId = id+1;
      len = strlen(cust_font_name);
      if (len >= 2 && *cust_font_name=='"' && cust_font_name[len-1]=='"') {
         for (c_ptr=cust_font_name; c_ptr[2] != '\0'; c_ptr++) {
            *c_ptr = c_ptr[1];
         }
         *c_ptr = '\0';
      }
   } else if (fileVersion <= 36) {
      if (GETINT("text", x,              "x") == INVALID ||
          GETINT("text", y,              "y") == INVALID ||
          GETSTR("text", font_str,       "font_str") == INVALID ||
          GETINT("text", style,          "style") == INVALID ||
          GETINT("text", sz_unit,        "size unit") == INVALID ||
          GETINT("text", num_lines,      "num_lines") == INVALID ||
          GETINT("text", text_just,      "text_just") == INVALID ||
          GETINT("text", rotate,         "rotate") == INVALID ||
          GETINT("text", pen,            "pen") == INVALID ||
          GETINT("text", obbox_w,        "bbox w") == INVALID ||
          GETINT("text", obbox_h,        "bbox h") == INVALID ||
          GETINT("text", id,             "id") == INVALID ||
          GETINT("text", dpi,            "dpi") == INVALID ||
          GETINT("text", asc,            "ascent") == INVALID ||
          GETINT("text", des,            "descent") == INVALID ||
          GETINT("text", fill,           "fill") == INVALID ||
          GETINT("text", v_space,        "vertical spacing") == INVALID ||
          GETINT("text", rotation,       "rotation") == INVALID ||
          GETINT("text", locked,         "locked") == INVALID ||
          GETINT("text", underline_on,   "underline_on") == INVALID ||
          GETINT("text", underline,      "underline") == INVALID ||
          GETINT("text", min_lbearing,   "min_lbearing") == INVALID ||
          GETINT("text", max_rextra,     "max_rextra") == INVALID ||
          GETINT("text", double_byte,    "double_byte") == INVALID ||
          GETINT("text", direction,      "direction") == INVALID ||
          GETSTR("text", cust_font_name, "cust_font_name") == INVALID ||
          GETINT("text", compressed,     "compressed") == INVALID ||
          GETINT("text", transformed,    "transformed") == INVALID ||
          GETINT("text", invisible,      "invisible") == INVALID ||
          GETINT("text", has_ps_bitmap,  "has_ps_bitmap") == INVALID ||
          GETINT("text", db_mod_bytes,   "double_byte_mod_bytes") == INVALID ||
          GETINT("text", db_vertical,    "double_byte_vertical") == INVALID ||
          GETINT("text", trans_pat,      "trans_pat") == INVALID) {
         return;
      }
      if (id >= objId) objId = id+1;
      len = strlen(cust_font_name);
      if (len >= 2 && *cust_font_name=='"' && cust_font_name[len-1]=='"') {
         for (c_ptr=cust_font_name; c_ptr[2] != '\0'; c_ptr++) {
            *c_ptr = c_ptr[1];
         }
         *c_ptr = '\0';
      }
   } else {
      if (GETINT("text", x,               "x") == INVALID ||
          GETINT("text", y,               "y") == INVALID ||
          GETINT("text", num_lines,       "num_lines") == INVALID ||
          GETINT("text", text_just,       "text_just") == INVALID ||
          GETINT("text", pen,             "pen") == INVALID ||
          GETINT("text", obbox_w,         "bbox w") == INVALID ||
          GETINT("text", obbox_h,         "bbox h") == INVALID ||
          GETINT("text", id,              "id") == INVALID ||
          GETINT("text", first_block_asc, "first block ascent") == INVALID ||
          GETINT("text", first_block_des, "first block descent") == INVALID ||
          GETINT("text", fill,            "fill") == INVALID ||
          GETINT("text", v_space,         "vertical spacing") == INVALID ||
          GETINT("text", rotation,        "rotation") == INVALID ||
          GETINT("text", locked,          "locked") == INVALID ||
          GETINT("text", underline,       "underline") == INVALID ||
          GETINT("text", text_w,          "text_w") == INVALID ||
          GETINT("text", text_h,          "text_h") == INVALID ||
          GETINT("text", min_lbearing,    "min_lbearing") == INVALID ||
          GETINT("text", max_rextra,      "max_rextra") == INVALID ||
          GETSTR("text", cust_font_name,  "cust_font_name") == INVALID ||
          GETINT("text", compressed,      "compressed") == INVALID ||
          GETINT("text", transformed,     "transformed") == INVALID ||
          GETINT("text", invisible,       "invisible") == INVALID ||
          GETINT("text", trans_pat,       "trans_pat") == INVALID ||
          GETINT("text", baseline_y,      "baseline_y") == INVALID) {
         return;
      }
      /*
       * This is a hack!  If the file version becomes >= 38, this code
       *       must be removed!
       */
      if (GETSTR("text", bg_color_str,    "bg_color_str") == INVALID) {
         /* should not get here because we should have read in: "[" */
         return;
      } else if (*bg_color_str == '[') {
         *bg_color_str = '\0';
      } else if (*bg_color_str == '\'') {
         UtilRemoveQuotes(bg_color_str);
      }
      if (id >= objId) objId = id+1;
      len = strlen(cust_font_name);
      if (len >= 2 && *cust_font_name=='"' && cust_font_name[len-1]=='"') {
         for (c_ptr=cust_font_name; c_ptr[2] != '\0'; c_ptr++) {
            *c_ptr = c_ptr[1];
         }
         *c_ptr = '\0';
      }
   }
   if (fileVersion >= 33 && transformed) {
      int do_adjust=FALSE, dx=0, t[2];
      double dreal_x, dreal_y, m[4];

      (void)fgets(inbuf, MAXSTRING, FP);
      scanLineNum++;
      InitScan(inbuf, "\t\n, ");

      ctm = (struct XfrmMtrxRec *)malloc(sizeof(struct XfrmMtrxRec));
      if (ctm == NULL) FailAllocMessage();
      if (GETDBL("text", dreal_x,        "real_x") == INVALID ||
          GETDBL("text", dreal_y,        "real_y") == INVALID ||
          GETINT("text", orig_obbox.ltx, "orig_obbox.ltx") == INVALID ||
          GETINT("text", orig_obbox.lty, "orig_obbox.lty") == INVALID ||
          GETINT("text", orig_obbox.rbx, "orig_obbox.rbx") == INVALID ||
          GETINT("text", orig_obbox.rby, "orig_obbox.rby") == INVALID ||
          GETDBL("text", m[CTM_SX],      "CTM_SX") == INVALID ||
          GETDBL("text", m[CTM_SIN],     "CTM_SIN") == INVALID ||
          GETDBL("text", m[CTM_MSIN],    "CTM_MSIN") == INVALID ||
          GETDBL("text", m[CTM_SY],      "CTM_SY") == INVALID ||
          GETINT("text", t[CTM_TX],      "CTM_TX") == INVALID ||
          GETINT("text", t[CTM_TY],      "CTM_TY") == INVALID ||
          GETINT("text", orig_bbox.ltx,  "orig_bbox.ltx") == INVALID ||
          GETINT("text", orig_bbox.lty,  "orig_bbox.lty") == INVALID ||
          GETINT("text", orig_bbox.rbx,  "orig_bbox.rbx") == INVALID ||
          GETINT("text", orig_bbox.rby,  "orig_bbox.rby") == INVALID) {
         return;
      }
      real_x = round(dreal_x);
      real_y = round(dreal_y);
      ctm->m[CTM_SX] = m[CTM_SX];
      ctm->m[CTM_SIN] = m[CTM_SIN];
      ctm->m[CTM_MSIN] = m[CTM_MSIN];
      ctm->m[CTM_SY] = m[CTM_SY];
      ctm->t[CTM_TX] = t[CTM_TX];
      ctm->t[CTM_TY] = t[CTM_TY];
      if (orig_obbox.ltx == 0 && orig_obbox.lty == 0 &&
            orig_obbox.rbx == 0 && orig_obbox.rby == 0) {
         adj_bbox_for_third_party = TRUE;
      }
      switch (text_just) {
      case JUST_L:
         if (real_x != orig_obbox.ltx) {
            dx = real_x - orig_obbox.ltx;
            do_adjust = TRUE;
         }
         break;
      case JUST_C:
         if (real_x != ((orig_obbox.ltx+orig_obbox.rbx)>>1)) {
            dx = real_x - ((orig_obbox.ltx+orig_obbox.rbx)>>1);
            do_adjust = TRUE;
         }
         break;
      case JUST_R:
         if (real_x != orig_obbox.rbx) {
            dx = real_x - orig_obbox.rbx;
            do_adjust = TRUE;
         }
         break;
      }
      if (adj_bbox_for_third_party || do_adjust) {
         orig_obbox.ltx += dx;
         orig_obbox.rbx += dx;
         orig_bbox.ltx += dx;
         orig_bbox.rbx += dx;
      }
   }
   if (fileVersion <= 36) {
      baseline_y = y+asc;
      if (ctm == NULL) {
         text_w = obbox_w;
         text_h = obbox_h;
      } else {
         text_w = orig_obbox.rbx-orig_obbox.ltx;
         text_h = orig_obbox.rby-orig_obbox.lty;
      }
      first_block_asc = asc;
      first_block_des = des;
   } else {
      asc = first_block_asc;
      des = first_block_des;
   }
   fill = UpgradePenFill(fill);
   pen = UpgradePenFill(pen);

   *ObjPtr = (struct ObjRec *)malloc(sizeof(struct ObjRec));
   if (*ObjPtr == NULL) FailAllocMessage();
   memset(*ObjPtr, 0, sizeof(struct ObjRec));

   text_ptr = (struct TextRec *)malloc(sizeof(struct TextRec));
   if (text_ptr == NULL) FailAllocMessage();
   memset(text_ptr, 0, sizeof(struct TextRec));
   text_ptr->lines = num_lines;
   text_ptr->cached_bitmap = None;
   text_ptr->cached_zoom = 0;

   if (!PRTGIF) SaveCurFont();

   if (fileVersion <= 36) {
      /* set curFont, curStyle, and curSzUnit */
      if (fileVersion <= 29) {
         GetCompatibleFontName(font, style, font_str);
      }
      ReadTextSetFontStyleSzUnit(font_str, sb_font_str, style, sz_unit,
            size, dpi, &double_byte, &cur_sb_font, &cur_db_font);
   } else {
      /* curFont, curStyle, and curSzUnit are undefined here */
   }
   if (fileVersion >= 37) {
      UncombineOverUnderYOffsets(underline, &underline_y_offset,
            &overline_y_offset);
   } else {
      underline_y_offset = underline;
      overline_y_offset = 0;
   }
   transPat = (*ObjPtr)->trans_pat = trans_pat;

   textJust = text_ptr->minilines.just = text_just;
   textVSpace = text_ptr->minilines.v_space = v_space;
   canvasLineAsc = asc;
   canvasLineDes = des;

   penPat = text_ptr->pen = pen;
   objFill = text_ptr->fill = fill;
   curUnderlineOn = (underline_on & 0x1);
   curOverlineOn = ((underline_on & 0x2) == 0x2);
   curUnderlineYOffset = text_ptr->underline_y_offset = underline_y_offset;
   curOverlineYOffset = text_ptr->overline_y_offset = overline_y_offset;
   curOverlineOn = FALSE;

   /*
    * set canvasFontAsc, canvasFontDes, textCursorH, and canvasFontDoubleByte...
    */
   ReadTextSetCanvasFont(text_ptr, dpi, size, asc, des, double_byte,
         db_mod_bytes, db_vertical, direction);
   if (!PRTGIF && fileVersion <= 36) {
      baseline_y = y+canvasFontAsc;
   }

   if (text_ptr->read_only) {
      switch (rotate) {
      case ROTATE0:
      case ROTATE180:
         text_ptr->orig_w = obbox_w;
         text_ptr->orig_h = obbox_h;
         break;
      case ROTATE90:
      case ROTATE270:
         text_ptr->orig_w = obbox_h;
         text_ptr->orig_h = obbox_w;
         break;
      }
      text_ptr->min_lbearing = min_lbearing;
      text_ptr->max_rextra = max_rextra;
   } else {
      text_ptr->orig_w = text_ptr->orig_h = 0;
      text_ptr->min_lbearing = text_ptr->max_rextra = 0;
   }
   text_ptr->baseline_y = baseline_y;

   /*
    * Now read all the lines in.
    */
   firstMiniLine = lastMiniLine = NULL;
   if (!ReadTextLines(FP, ObjPtr, text_ptr, color_str, num_lines, has_ps_bitmap,
         cur_sb_font, cur_db_font, double_byte, db_mod_bytes, db_vertical,
         direction, x, baseline_y, text_w, &max_len, &max_h)) {
      if (!PRTGIF) RestoreCurFont();
      return;
   }
   text_ptr->minilines.first = firstMiniLine;
   text_ptr->minilines.last = lastMiniLine;

   firstMiniLine = lastMiniLine = NULL;
   textCurIndex = 0;

   if (!PRTGIF && !text_ptr->read_only) {
      RecalcTextMetrics(text_ptr, x, text_ptr->baseline_y);

      max_len = text_ptr->w;
      max_h = text_ptr->h;
      min_lbearing = text_ptr->min_lbearing;
      max_rextra = text_ptr->max_rextra;
   }
   if (!PRTGIF) {
      if (text_ptr->minilines.baseline_offset != 0 ||
            text_ptr->minilines.first->v_gap != 0) {
#ifdef _TGIF_DBG /* debug, do not translate */
         TgAssert(FALSE,
               "Non-zero first baseline_offset or v_gap in ReadTextObj()",
               "Fixed");
#endif /* _TGIF_DBG */
         text_ptr->minilines.baseline_offset = 0;
         text_ptr->minilines.first->v_gap = 0;
      }
      y = text_ptr->baseline_y - text_ptr->minilines.first->asc +
            text_ptr->minilines.first->v_gap;
   }
   text_ptr->min_lbearing = min_lbearing;
   text_ptr->max_rextra = max_rextra;
   (*ObjPtr)->type = OBJ_TEXT;
   (*ObjPtr)->color = QuickFindColorIndex(*ObjPtr, color_str, &new_alloc, TRUE);
   UtilStrCpyN((*ObjPtr)->color_str, sizeof((*ObjPtr)->color_str), color_str);
   (*ObjPtr)->dirty = FALSE;
   (*ObjPtr)->id = id;
   (*ObjPtr)->rotation = rotation;
   (*ObjPtr)->locked = locked;
   (*ObjPtr)->detail.t = text_ptr;
   (*ObjPtr)->x = x;
   (*ObjPtr)->y = y; /* NOTE: this y is derived from text_ptr->baseline_y */
   (*ObjPtr)->ctm = ctm;
   (*ObjPtr)->invisible = invisible;

   if (ctm != NULL) {
      memcpy(&(*ObjPtr)->orig_obbox, &orig_obbox, sizeof(struct BBRec));
      memcpy(&text_ptr->orig_bbox, &orig_bbox, sizeof(struct BBRec));
      (*ObjPtr)->x = real_x;
      (*ObjPtr)->y = real_y;
      if (adj_bbox_for_third_party) {
         SetTextOrigBBoxes(*ObjPtr, text_just, max_len, max_h,
               min_lbearing, max_rextra, rotate);
      }
      GetTransformedOBBoxOffsetVs(*ObjPtr, (*ObjPtr)->rotated_obbox);
   }
/*
 * This this point, (*ObjPtr)->x, (*ObjPtr)->y, text_just, obbox_w, obbox_h,
 * max_len, min_lbearing, max_rextra, and rotate are all valid
 */
   if (PRTGIF && fileVersion > 6) {
      switch (rotate) {
      case ROTATE0:
      case ROTATE180:
         SetTextBBox(*ObjPtr, text_just, obbox_w, obbox_h,
               min_lbearing, max_rextra, rotate);
         break;
      case ROTATE90:
      case ROTATE270:
         SetTextBBox(*ObjPtr, text_just, obbox_h, obbox_w,
               min_lbearing, max_rextra, rotate);
         break;
      }
   } else if (!PRTGIF && text_ptr->read_only) {
      SetTextBBox(*ObjPtr, text_just, text_ptr->orig_w, text_ptr->orig_h,
            min_lbearing, max_rextra, rotate);
   } else {
      if (fileVersion < 33 && rotate != ROTATE0) {
         /* pretend there's no rotate so we can get the coordiantes for obbox */
         SetTextBBox(*ObjPtr, text_just, max_len, max_h,
               min_lbearing, max_rextra, ROTATE0);
      } else {
         SetTextBBox(*ObjPtr, text_just, max_len, max_h,
               min_lbearing, max_rextra, rotate);
      }
   }
   if (fileVersion < 33 && rotate != ROTATE0) {
      int ltx=0, lty=0, saved_rotation_increment=rotationIncrement;

      /*
       * For fileVersion >= 33, rotate is always ROTATE0.  So we need
       *       to convert the old format to the new one.  The way we will
       *       do it is to:
       * 1) Calculate where the final obbox.ltx and obbox.lty will be.
       * 2) Rotate the object.
       * 3) Move the object to matched the final position calculated in (1).
       *
       * Well, turns out this is not needed!
       */
      OldStyleRotateGetFinalOBBoxLtXY(*ObjPtr, rotate, text_just, &ltx, &lty);

      SetRotatePivotByObject(*ObjPtr);
      switch (rotate) {
      case ROTATE90: rotationIncrement = (90<<6); break;
      case ROTATE180: rotationIncrement = (180<<6); break;
      case ROTATE270: rotationIncrement = (270<<6); break;
      }
      RotateObjClockWise(*ObjPtr);
      rotationIncrement = saved_rotation_increment;
   }
   if (!PRTGIF && ctm != NULL &&
         strcmp(gGenerateByInfo.name, "pstoedit") == 0) {
      int asc=text_ptr->minilines.first->first_block->asc, dx=0, dy=0;

      text_ptr->baseline_y = (*ObjPtr)->y + asc;
      TransformPointThroughCTM(0, asc, ctm, &dx, &dy);
      MoveObj(*ObjPtr, -dx, -dy);
   }
   if (!PRTGIF) RestoreCurFont();
}

void UpdCurTextBBox()
{
   struct TextRec *text_ptr=curTextObj->detail.t;
   MiniLinesInfo *minilines=NULL;

   if (!textCursorShown) return;

   RecalcTextMetrics(text_ptr, textOrigX, text_ptr->baseline_y);
   minilines = (&text_ptr->minilines);

   SetTextCurXY();
   if (textHighlight) SetTextEndXY();

   UpdateEditTextArea(minilines->w, minilines->h, minilines->min_lbearing,
         minilines->max_rextra);
}

void AdjustCurText(XOff, YOff)
   int XOff, YOff;
{
   textOrigX += XOff;
   textOrigY += YOff;
   textOrigBaselineY += YOff;
   textCurX += XOff;
   textCurY += YOff;
   textCurBaselineY += YOff;
   textEndX += XOff;
   textEndY += YOff;
   textEndBaselineY += YOff;
}

void PrepareZoomCurText(AbsXc, AbsYc)
   int *AbsXc, *AbsYc;
{
   switch (textJust) {
   case JUST_L: *AbsXc = ABS_X(textOrigX+(textW>>1)); break;
   case JUST_C: *AbsXc = ABS_X(textOrigX); break;
   case JUST_R: *AbsXc = ABS_X(textOrigX-(textW>>1)); break;
   }
   *AbsYc = ABS_Y(textOrigY+(textH>>1));
}

void PreZoomCurText()
{
   InvalidateTextCache(curTextObj->detail.t);
}

void PostZoomCurText(AbsXc, AbsYc)
   int AbsXc, AbsYc;
{
   int x=0, y=0, orig_text_w=0, orig_text_h=0;

   switch (textJust) {
   case JUST_L: x = OFFSET_X(AbsXc)-(textW>>1); break;
   case JUST_C: x = OFFSET_X(AbsXc); break;
   case JUST_R: x = OFFSET_X(AbsXc)+(textW>>1); break;
   }
   y = OFFSET_Y(AbsYc)-(textH>>1);
   AdjustCurText(x-textOrigX, y-textOrigY);
   textAbsX -= (tmpAdjAbsX+curTextMovedAbsX);
   textAbsY -= (tmpAdjAbsY+curTextMovedAbsY);
   textAbsBaselineY -= (tmpAdjAbsY+curTextMovedAbsY);

   if (editTextSize != 0) {
      if (RestoreEditTextSize(curTextObj, TRUE)) {
         UpdTextBBox(curTextObj);
      }
   }
   orig_text_w = curTextObj->obbox.rbx - curTextObj->obbox.ltx;
   orig_text_h = curTextObj->obbox.rby - curTextObj->obbox.lty;

   if (editTextSize != 0) {
      if (RestoreEditTextSize(curTextObj, FALSE)) {
         UpdTextBBox(curTextObj);
      }
   }
   switch (textJust) {
   case JUST_L: tmpAdjAbsX = (orig_text_w-ABS_SIZE(textW))/2; break;
   case JUST_C: tmpAdjAbsX = 0; break;
   case JUST_R: tmpAdjAbsX = (ABS_SIZE(textW)-orig_text_w)/2; break;
   }
   tmpAdjAbsY = (orig_text_h-ABS_SIZE(textH))/2;

   textAbsX += (tmpAdjAbsX+curTextMovedAbsX);
   textAbsY += (tmpAdjAbsY+curTextMovedAbsY);
   textAbsBaselineY += (tmpAdjAbsY+curTextMovedAbsY);

   SetTextCurXY();
   SetTextEndXY();
   RedrawCurText();
   UpdateTextInfoChoices(FALSE);
}

void HighLightText(obj_ptr, pStartStrBlock, start_index, pEndStrBlock,
      end_index)
   struct ObjRec *obj_ptr;
   StrBlockInfo *pStartStrBlock, *pEndStrBlock;
   int start_index, end_index;
{
   int abs_x=0, abs_y=0, x_off=0, y_off=0;
   int update_highlighted_text_bboxes=FALSE;

   if (curChoice == DRAWTEXT && textCursorShown) {
      if (textHighlight) {
         update_highlighted_text_bboxes = TRUE;
      } else {
         EraseTextCursor();
      }
      escPressed = FALSE;
      CreateTextObj(TRUE, TRUE);
      curTextModified = FALSE;
   }
   outerSel = outerSelForFind;
   innerSel = innerSelForFind;
   outerSelForFind = innerSelForFind = NULL;
   abs_x = obj_ptr->x; x_off = OFFSET_X(abs_x);
   abs_y = obj_ptr->detail.t->baseline_y; y_off = OFFSET_Y(abs_y);

   curTextModified = FALSE;
   if (!PrepareEditExistingText(obj_ptr, abs_x, abs_y, &x_off, &y_off)) {
      return;
   }
   if (update_highlighted_text_bboxes && curStrBlock != NULL) {
      UpdateHighLightedTextBBoxes(TRUE);
   }
   HandleClickOnText(FALSE, TRUE, x_off, y_off, FALSE, curTextObj, FALSE, TRUE,
         TRUE, (Time)0);

   if (pStartStrBlock == NULL) {
      curStrBlock = curTextObj->detail.t->minilines.first->first_block;
      textCurIndex = 0;
      ResetOnCursorKey(FALSE);
   } else {
      curStrBlock = pStartStrBlock;
      textCurIndex = start_index;
      endStrBlock = pEndStrBlock;
      textEndIndex = end_index;
   }
   SetTextHighlight();

   SetTextCurXY();
   if (endStrBlock != NULL) SetTextEndXY();
   textJustClicked = FALSE;

   UpdatePinnedMenu(MENU_EDIT);
   RedrawCurText();
   ScrollTo(textCurX, textCurBaselineY);
   UpdateTextInfoChoices(FALSE);
}

int FindTextInCurTextObj(str_to_match, str_len, case_sensitive,
      ppFoundStartStrBlock, pn_start_index, ppFoundEndStrBlock, pn_end_index)
   char *str_to_match;
   int str_len, case_sensitive, *pn_start_index, *pn_end_index;
   StrBlockInfo **ppFoundStartStrBlock, **ppFoundEndStrBlock;
{
   int found_starting_point=FALSE;
   MiniLinesInfo *minilines=(&curTextObj->detail.t->minilines);

   SaveCursorPositionInCurText();

   if (textHighlight) {
      ReorderCursorStrBlocks();
      SetTextHighlight();
      UpdatePinnedMenu(MENU_EDIT);
   }
   *ppFoundStartStrBlock = *ppFoundEndStrBlock = NULL;
   *pn_start_index = *pn_end_index = INVALID;
   /*
    * This is what's going on in FindStringInMiniLines()...
    * First, find curStrBlock.  Set *pn_found_starting_point to TRUE
    *       once curStrBlock is found.
    * After curStrBlock is found, if *ppStartStrBlock is NULL, this means
    *       that we are still trying to find a prefix match.
    * If *ppStartStrBlock is non-NULL, that means the prefix is found, so
    *       we must match starting at the beginning of every string.
    */
   if (FindStringInMiniLines(minilines, &found_starting_point,
         str_to_match, str_len, case_sensitive, ppFoundStartStrBlock,
         pn_start_index, ppFoundEndStrBlock, pn_end_index)) {
      RestoreCursorPositionInCurText();
      return TRUE;
   }
   RestoreCursorPositionInCurText();

   return FALSE;
}

/* --------------------- Insert Special Routines --------------------- */

int SetScriptFractionValue(spec)
   char *spec;
{
   float fv=(float)0.0;
   char buf[80];

   fv = (float)atof(spec);
   UtilStrCpyN(buf, sizeof(buf), spec);
   if (fabs(fv - ((double)1.01)) < INT_TOL) {
      fv = (float)1.0;
      strcpy(buf, "1.0");
   } else if (fabs(fv - ((double)0.2)) < INT_TOL) {
      fv = (float)0.2;
      strcpy(buf, "0.2");
   } else if (fv < 0.2 || fv > 1.01) {
      sprintf(gszMsgBox, TgLoadString(STID_INVALID_GIVEN_VALUE_SPECIFIED), spec);
      if (PRTGIF) {
         fprintf(stderr, "%s\n", gszMsgBox);
      } else {
         Msg(gszMsgBox);
      }
      return FALSE;
   }
   if (strcmp(spec, scriptFractionStr) != 0) {
      scriptFraction = fv;
      strcpy(scriptFractionStr, buf);
   }
   return TRUE;
}

void SetScriptFraction()
{
   char spec[MAXSTRING+1];

   *spec = '\0';
   sprintf(gszMsgBox, TgLoadString(STID_ENTER_SCRIPT_FRACTION_CUR_IS),
         scriptFractionStr);
   if (Dialog(gszMsgBox, NULL, spec) == INVALID) return;
   UtilTrimBlanks(spec);
   if (*spec == '\0') return;

   if (SetScriptFractionValue(spec)) {
      sprintf(gszMsgBox, TgLoadString(STID_SCRIPT_FRACTION_SET_TO_STR),
            scriptFractionStr);
      Msg(gszMsgBox);
   }
}

static
int CanInsertThinSpace()
{
   return (textCursorShown && curStrBlock != NULL &&
         curStrBlock->type != SB_SUPSUB_CENTER && !textHighlight);
}

void InsertThinSpace()
{
   int w=0;
   char spec[MAXSTRING];

   if (!CanInsertThinSpace()) return;

   *spec = '\0';
   if (Dialog(TgLoadString(STID_ENTER_INT_WIDTH_FOR_THIN_SPC), NULL, spec) ==
         INVALID) {
      return;
   }
   UtilTrimBlanks(spec);
   if (*spec == '\0') return;
   w = atoi(spec);
   if (w == 0) {
      sprintf(gszMsgBox, TgLoadString(STID_INVALID_GIVEN_VALUE_SPECIFIED),
            spec);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return;
   }
   escPressed = FALSE;

   ResetDirtyBBoxInfo();
   curTextModified = TRUE;
   if (textHighlight) {
      /* Well, should never get here! */
#ifdef _TGIF_DBG /* debug, do not translate */
      TgAssert(FALSE, "textHighlight in InsertThinSpace()", NULL);
#endif /* _TGIF_DBG */
      InsertHighlightedThinSpace(w);
   } else {
      DoInsertThinSpace(w);
   }
   EndChangeCurText(FALSE);
   MarkRulers(textCurX, textCurY);
   SetFileModified(TRUE);
   ScrollTo(textCurX, textCurBaselineY);
}

static
int CanInsertOrRemoveVerticalOffset()
{
   if (textCursorShown && !textHighlight && curStrBlock != NULL &&
         curStrBlock->type == SB_SIMPLE) {
      MiniLinesInfo *minilines=(&curTextObj->detail.t->minilines);

      if (curStrBlock->owner_mini_line->owner_minilines == minilines &&
            curStrBlock->owner_mini_line == minilines->first) {
         return FALSE;
      }
      return TRUE;
   }
   return FALSE;
}

static
void DoInsertVerticalOffset(baseline_offset)
   int baseline_offset;
{
   MiniLinesInfo *pOwnerMinilines=NULL;

   ResetOnCursorKey(FALSE);

   switch(curStrBlock->type) {
   case SB_SIMPLE:
      pOwnerMinilines = curStrBlock->owner_mini_line->owner_minilines;
      if (curStrBlock->owner_mini_line == pOwnerMinilines->first) {
         pOwnerMinilines->baseline_offset += baseline_offset;
      } else {
         curStrBlock->owner_mini_line->v_gap += baseline_offset;
      }
      break;
   case SB_CHAR_SPACE: /* not possible */ return;
   case SB_SUPSUB_LEFT: /* not possible */ return;
   case SB_SUPSUB_RIGHT: /* not possible */ return;
   case SB_SUPSUB_CENTER: /* not possible */ return;
   }
   EndChangeCurText(FALSE);
   MarkRulers(textCurX, textCurY);
   SetFileModified(TRUE);
   ScrollTo(textCurX, textCurBaselineY);
}

void InsertVerticalOffset()
{
   int baseline_offset=0;
   char spec[MAXSTRING];

   if (!CanInsertOrRemoveVerticalOffset()) return;

   *spec = '\0';
   if (Dialog(TgLoadString(STID_ENTER_INT_VAL_FOR_VERT_OFFSET), NULL, spec) ==
         INVALID) {
      return;
   }
   UtilTrimBlanks(spec);
   if (*spec == '\0') return;
   baseline_offset = atoi(spec);
   if (baseline_offset == 0) {
      return;
   }
   ResetOnCursorKey(TRUE);
   SetTextHighlight();
   UpdatePinnedMenu(MENU_EDIT);

   ResetDirtyBBoxInfo();
   curTextModified = TRUE;

   DoInsertVerticalOffset(baseline_offset);
}

static
void DoRemoveVerticalOffset()
{
   MiniLinesInfo *pOwnerMinilines=NULL;

   ResetOnCursorKey(FALSE);

   switch(curStrBlock->type) {
   case SB_SIMPLE:
      pOwnerMinilines = curStrBlock->owner_mini_line->owner_minilines;
      if (curStrBlock->owner_mini_line == pOwnerMinilines->first) {
         pOwnerMinilines->baseline_offset = 0;
      } else {
         curStrBlock->owner_mini_line->v_gap = 0;
      }
      break;
   case SB_CHAR_SPACE: /* not possible */ return;
   case SB_SUPSUB_LEFT: /* not possible */ return;
   case SB_SUPSUB_RIGHT: /* not possible */ return;
   case SB_SUPSUB_CENTER: /* not possible */ return;
   }
   EndChangeCurText(FALSE);
   MarkRulers(textCurX, textCurY);
   SetFileModified(TRUE);
   ScrollTo(textCurX, textCurBaselineY);
}

void RemoveVerticalOffset()
{
   if (!CanInsertOrRemoveVerticalOffset()) return;

   ResetOnCursorKey(TRUE);
   SetTextHighlight();
   UpdatePinnedMenu(MENU_EDIT);

   ResetDirtyBBoxInfo();
   curTextModified = TRUE;

   DoRemoveVerticalOffset();
}

#define INSERT_SUPERSCRIPT (TRUE)
#define INSERT_SUBSCRIPT (FALSE)

static
int CanInsertLeftRightScripts()
{
   return (textCursorShown && !textHighlight && curStrBlock != NULL &&
         curStrBlock->type != SB_SUPSUB_CENTER);
}

static
MiniLinesInfo *CreateMinilinesForInsertScripts(type)
   int type;
   /* assuming curFont, curStyle, curSzUnit, textVSpace are all correct */
{
   MiniLinesInfo *new_minilines=NewMiniLines();

   SetCanvasFont();

   CreateMiniLineFromString("", &new_minilines->first, &new_minilines->last);

   switch (type) {
   case SB_SUPSUB_LEFT: new_minilines->just = JUST_R; break;
   case SB_SUPSUB_CENTER: new_minilines->just = JUST_C; break;
   case SB_SUPSUB_RIGHT: new_minilines->just = JUST_L; break;
   }
   new_minilines->v_space = textVSpace;
   new_minilines->first->first_block->seg->sz_unit = curSzUnit;

   return new_minilines;
}

static
void DetermineWhatToDoForLeftRightScripts(type, pnJustMove,
      pnSplitAtIndex, pnInsertBefore)
   int type, *pnJustMove, *pnSplitAtIndex, *pnInsertBefore;
{
   if (textCurIndex == 0 && curStrBlock->seg->dyn_str.sz == 1) {
      if (type == SB_SUPSUB_LEFT && curStrBlock->prev != NULL &&
            curStrBlock->prev->type == type) {
         *pnJustMove = TRUE;
         return;
      } else if (type == SB_SUPSUB_RIGHT && curStrBlock->next != NULL &&
            curStrBlock->next->type == type) {
         *pnJustMove = TRUE;
         return;
      }
      if ((type == SB_SUPSUB_LEFT && curStrBlock->prev == NULL) ||
            (type == SB_SUPSUB_RIGHT && curStrBlock->next == NULL)) {
         *pnSplitAtIndex = TRUE;
         return;
      }
   }
   if (textCurIndex == 0) {
      if (curStrBlock->prev == NULL) {
         *pnSplitAtIndex = TRUE;
      } else if (curStrBlock->prev->type == SB_SIMPLE) {
         *pnInsertBefore = TRUE;
      } else {
         if (curStrBlock->prev->type == type && type == SB_SUPSUB_LEFT) {
            *pnJustMove = TRUE;
         } else {
            *pnSplitAtIndex = TRUE;
         }
      }
   } else if (textCurIndex == curStrBlock->seg->dyn_str.sz-1) {
      if (curStrBlock->next == NULL) {
         *pnSplitAtIndex = TRUE;
      } else if (curStrBlock->next->type == SB_SIMPLE) {
         *pnInsertBefore = FALSE;
      } else {
         if (curStrBlock->next->type == type && type == SB_SUPSUB_RIGHT) {
            *pnJustMove = TRUE;
         } else {
            *pnSplitAtIndex = TRUE;
         }
      }
   } else {
      *pnSplitAtIndex = TRUE;
   }
}

static
void GetNewSize(pStrSeg, pn_new_size)
   StrSegInfo *pStrSeg;
   int *pn_new_size;
{
   int new_sz_unit=0;

   if (editTextSize != 0) {
      new_sz_unit = round(((double)pStrSeg->real_sz_unit) *
            ((double)scriptFraction));
   } else {
      new_sz_unit = round(((double)pStrSeg->sz_unit) *
            ((double)scriptFraction));
   }
   if (pn_new_size != NULL) {
      *pn_new_size = SzUnitToFontSize(new_sz_unit);
   }
}

static
int DoInsertLeftRightScripts(type, superscript)
   int type, superscript;
{
   int just_move=FALSE, split_at_index=FALSE, insert_before=FALSE;
   int new_size=0, cur_v_space=textVSpace, font_h=0;
   MiniLineInfo *owner_mini_line=curStrBlock->owner_mini_line;
   StrBlockInfo *pSupSubStrBlock=NULL;

   PushCurFont();
   if (editTextSize != 0) {
      if (RestoreEditTextSize(curTextObj, TRUE)) {
         UpdTextBBox(curTextObj);
      }
   }
   CopyCurInfoFromStrSeg(curStrBlock->seg);

   GetNewSize(curStrBlock->seg, &new_size);

   if (!TrySetCanvasFont(curFont, curStyle, new_size, TRUE, NULL)) {
      if (editTextSize != 0) {
         if (RestoreEditTextSize(curTextObj, FALSE)) {
            UpdTextBBox(curTextObj);
         }
      }
      PopCurFont();
      return FALSE;
   }
   DetermineWhatToDoForLeftRightScripts(type, &just_move, &split_at_index,
         &insert_before);

   if (just_move) {
      if (type == SB_SUPSUB_LEFT) {
         pSupSubStrBlock = curStrBlock->prev;
      } else {
         pSupSubStrBlock = curStrBlock->next;
      }
   } else {
      pSupSubStrBlock = NewStrBlock();
      pSupSubStrBlock->type = type;
      pSupSubStrBlock->owner_mini_line = owner_mini_line;

      if (split_at_index) {
         StrBlockInfo *pLeftStrBlock=NULL;
         char *psz=UtilStrDup(curStrBlock->seg->dyn_str.s), saved_ch='\0';
   
         if (psz == NULL) FailAllocMessage();
   
         DupStrBlock(curStrBlock, owner_mini_line, &pLeftStrBlock,
               &pLeftStrBlock);
         saved_ch = psz[textCurIndex];
         psz[textCurIndex] = '\0';
         DynStrSet(&pLeftStrBlock->seg->dyn_str, psz);
         psz[textCurIndex] = saved_ch;
         DynStrSet(&curStrBlock->seg->dyn_str, &psz[textCurIndex]);
         UtilFree(psz);
   
         pLeftStrBlock->prev = curStrBlock->prev;
         if (curStrBlock->prev == NULL) {
            owner_mini_line->first_block = pLeftStrBlock;
         } else {
            curStrBlock->prev->next = pLeftStrBlock;
         }
         pLeftStrBlock->next = pSupSubStrBlock;
         pSupSubStrBlock->prev = pLeftStrBlock;
   
         pSupSubStrBlock->next = curStrBlock;
         curStrBlock->prev = pSupSubStrBlock;
      } else if (insert_before) {
         pSupSubStrBlock->prev = curStrBlock->prev;
         curStrBlock->prev->next = pSupSubStrBlock;
   
         pSupSubStrBlock->next = curStrBlock;
         curStrBlock->prev = pSupSubStrBlock;
      } else {
         /* insert after */
         pSupSubStrBlock->next = curStrBlock->next;
         curStrBlock->next->prev = pSupSubStrBlock;

         pSupSubStrBlock->prev = curStrBlock;
         curStrBlock->next = pSupSubStrBlock;
      }
      pSupSubStrBlock->sup = CreateMinilinesForInsertScripts(type);
      font_h = pSupSubStrBlock->sup->first->first_block->seg->asc +
            pSupSubStrBlock->sup->first->first_block->seg->des;
      pSupSubStrBlock->sup->baseline_offset = -curStrBlock->seg->asc +
            curStrBlock->seg->des + (font_h>>1) -
            pSupSubStrBlock->sup->first->first_block->seg->des;

      pSupSubStrBlock->sub = CreateMinilinesForInsertScripts(type);
      font_h = pSupSubStrBlock->sub->first->first_block->seg->asc +
            pSupSubStrBlock->sub->first->first_block->seg->des;
      pSupSubStrBlock->sub->baseline_offset = (font_h>>1) -
            pSupSubStrBlock->sub->first->first_block->seg->des;
   }
   if (superscript) {
      curStrBlock = pSupSubStrBlock->sup->first->first_block;
   } else {
      curStrBlock = pSupSubStrBlock->sub->first->first_block;
   }
   textCurIndex = 0;
   textVSpace = cur_v_space;

   if (editTextSize != 0) {
      if (RestoreEditTextSize(curTextObj, FALSE)) {
         UpdTextBBox(curTextObj);
      }
   }
   PopCurFont();

   EndChangeCurText(FALSE);

   MarkRulers(textCurX, textCurY);
   SetFileModified(TRUE);
   ScrollTo(textCurX, textCurBaselineY);

   return TRUE;
}

static
void InsertLeftRightScripts(type, superscript)
   int type, superscript;
{
   escPressed = FALSE;

   ResetDirtyBBoxInfo();
   if (!DoInsertLeftRightScripts(type, superscript)) {
      MsgBox(TgLoadString(STID_FAIL_INSERT_SCRIPT_FONT_SZ), TOOL_NAME, INFO_MB);
   } else {
      curTextModified = TRUE;
   }
}

void InsertRightSuperscript()
{
   if (!CanInsertLeftRightScripts()) return;

   InsertLeftRightScripts(SB_SUPSUB_RIGHT, INSERT_SUPERSCRIPT);
}

void InsertRightSubscript()
{
   if (!CanInsertLeftRightScripts()) return;

   InsertLeftRightScripts(SB_SUPSUB_RIGHT, INSERT_SUBSCRIPT);
}

void InsertLeftSuperscript()
{
   if (!CanInsertLeftRightScripts()) return;

   InsertLeftRightScripts(SB_SUPSUB_LEFT, INSERT_SUPERSCRIPT);
}

void InsertLeftSubscript()
{
   if (!CanInsertLeftRightScripts()) return;

   InsertLeftRightScripts(SB_SUPSUB_LEFT, INSERT_SUBSCRIPT);
}

static
int CanInsertCenterScripts()
{
   if (textCursorShown) {
      if (textHighlight) {
         if (curStrBlock == endStrBlock && textCurIndex != textEndIndex) {
            return TRUE;
         }
      } else if (curStrBlock->type == SB_SUPSUB_CENTER) {
         return TRUE;
      }
   }
   return FALSE;
}

static
int DoInsertCenterScripts(superscript)
   int superscript;
{
   int new_size=0;
   MiniLineInfo *owner_mini_line=curStrBlock->owner_mini_line;

   PushCurFont();
   CopyCurInfoFromStrSeg(curStrBlock->seg);

   GetNewSize(curStrBlock->seg, &new_size);

   if (!TrySetCanvasFont(curFont, curStyle, new_size, TRUE, NULL)) {
      PopCurFont();
      return FALSE;
   }
   if (curStrBlock->type == SB_SUPSUB_CENTER) {
      if (superscript) {
         curStrBlock = curStrBlock->sup->first->first_block;
      } else {
         curStrBlock = curStrBlock->sub->first->first_block;
      }
   } else {
      int min_index=min(textCurIndex, textEndIndex);
      int max_index=max(textCurIndex, textEndIndex);
      StrBlockInfo *pLeftStrBlock=NULL, *pRightStrBlock=curStrBlock;
      StrBlockInfo *pCenterStrBlock=NULL;
      char *psz=NULL, saved_ch='\0';

      pCenterStrBlock = NewStrBlock();
      pCenterStrBlock->type = SB_SUPSUB_CENTER;
      pCenterStrBlock->owner_mini_line = owner_mini_line;
      DupStrSeg(pCenterStrBlock, curStrBlock->seg);

      psz = UtilStrDup(curStrBlock->seg->dyn_str.s);
      if (psz == NULL) FailAllocMessage();
   
      DupStrBlock(pRightStrBlock, owner_mini_line, &pLeftStrBlock,
            &pLeftStrBlock);
      saved_ch = psz[min_index];
      psz[min_index] = '\0';
      DynStrSet(&pLeftStrBlock->seg->dyn_str, psz);
      psz[min_index] = saved_ch;
      DynStrSet(&pRightStrBlock->seg->dyn_str, &psz[max_index]);
      psz[max_index] = '\0';
      DynStrSet(&pCenterStrBlock->seg->dyn_str, &psz[min_index]);
      UtilFree(psz);

      pLeftStrBlock->prev = pRightStrBlock->prev;
      if (pRightStrBlock->prev == NULL) {
         owner_mini_line->first_block = pLeftStrBlock;
      } else {
         pRightStrBlock->prev->next = pLeftStrBlock;
      }
      pLeftStrBlock->next = pCenterStrBlock;
      pCenterStrBlock->prev = pLeftStrBlock;

      pCenterStrBlock->next = pRightStrBlock;
      pRightStrBlock->prev = pCenterStrBlock;

      pCenterStrBlock->sup = CreateMinilinesForInsertScripts(SB_SUPSUB_CENTER);
      /*
       * pCenterStrBlock->sup->baseline_offset =
       *       (-pCenterStrBlock->sup->first->first_block->seg->des);
       */
      pCenterStrBlock->sup->baseline_offset = 0;

      pCenterStrBlock->sub = CreateMinilinesForInsertScripts(SB_SUPSUB_CENTER);
      /*
       * pCenterStrBlock->sub->baseline_offset =
       *       pCenterStrBlock->sub->first->first_block->seg->asc;
       */
      pCenterStrBlock->sub->baseline_offset =
            (pCenterStrBlock->sub->first->first_block->seg->asc -
            pCenterStrBlock->seg->des);
      if (superscript) {
         curStrBlock = pCenterStrBlock->sup->first->first_block;
      } else {
         curStrBlock = pCenterStrBlock->sub->first->first_block;
      }
      /* consolidate */
      if (*pLeftStrBlock->seg->dyn_str.s == '\0') {
         if (pLeftStrBlock->prev != NULL &&
               pLeftStrBlock->prev->type == SB_SIMPLE) {
            UnlinkStrBlock(pLeftStrBlock);
            FreeStrBlock(pLeftStrBlock);
            pLeftStrBlock = NULL;
         }
      }
      if (*pRightStrBlock->seg->dyn_str.s == '\0') {
         if (pRightStrBlock->next != NULL &&
               pRightStrBlock->next->type == SB_SIMPLE) {
            UnlinkStrBlock(pRightStrBlock);
            FreeStrBlock(pRightStrBlock);
            pRightStrBlock = NULL;
         }
      }
   }
   textCurIndex = 0;
   ResetOnCursorKey(FALSE);
   SetTextHighlight();

   PopCurFont();

   EndChangeCurText(FALSE);

   UpdatePinnedMenu(MENU_EDIT);
   MarkRulers(textCurX, textCurY);
   SetFileModified(TRUE);
   ScrollTo(textCurX, textCurBaselineY);

   return TRUE;
}

static
void InsertCenterScripts(superscript)
   int superscript;
{
   escPressed = FALSE;

   ResetDirtyBBoxInfo();
   if (!DoInsertCenterScripts(superscript)) {
      MsgBox(TgLoadString(STID_FAIL_INSERT_SCRIPT_FONT_SZ), TOOL_NAME, INFO_MB);
   } else {
      curTextModified = TRUE;
   }
}

void InsertCenterSuperscript()
{
   if (!CanInsertCenterScripts()) return;

   InsertCenterScripts(INSERT_SUPERSCRIPT);
}

void InsertCenterSubscript()
{
   if (!CanInsertCenterScripts()) return;

   InsertCenterScripts(INSERT_SUBSCRIPT);
}

int RefreshEditTextMenu(menu)
   TgMenu *menu;
{
   int ok=TRUE;

   ok &= TgEnableMenuItemById(menu, CMDID_INSERTTHINSPACE,
         CanInsertThinSpace());
   ok &= TgEnableMenuItemById(menu, CMDID_INSERTVERTICALOFFSET,
         CanInsertOrRemoveVerticalOffset());

   ok &= TgEnableMenuItemById(menu, CMDID_INSERTRIGHTSUPERSCRIPT,
         CanInsertLeftRightScripts());
   ok &= TgEnableMenuItemById(menu, CMDID_INSERTRIGHTSUBSCRIPT,
         CanInsertLeftRightScripts());
   ok &= TgEnableMenuItemById(menu, CMDID_INSERTLEFTSUPERSCRIPT,
         CanInsertLeftRightScripts());
   ok &= TgEnableMenuItemById(menu, CMDID_INSERTLEFTSUBSCRIPT,
         CanInsertLeftRightScripts());
   ok &= TgEnableMenuItemById(menu, CMDID_INSERTCENTERSUPERSCRIPT,
         CanInsertCenterScripts());
   ok &= TgEnableMenuItemById(menu, CMDID_INSERTCENTERSUBSCRIPT,
         CanInsertCenterScripts());

   ok &= TgEnableMenuItemById(menu, CMDID_RESETINMETHOD,
         gnInputMethod != TGIM_NONE);

   return ok;
}

TgMenu *CreateEditTextMenu(parent_menu, x, y, menu_info, status_str_xlated)
   TgMenu *parent_menu;
   int x, y;
   TgMenuInfo *menu_info;
   int status_str_xlated; /* ignored, always 0 */
{
   TgMenu *menu=TgCreateMenuFromMenuInfo(parent_menu, x, y, menu_info, FALSE);

   if (menu != NULL) {
      if (!RefreshEditTextMenu(menu)) {
         return TgDestroyMenu(menu, TRUE);
      }
   }
   return menu;
}

static
void DetermineWhatToDoForChangeProperty(lWhich, nValue, pStrBlock, nIndex,
      pnMoveLeft, pnMoveRight, pnSplitAtIndex, pnInsertBefore)
   long lWhich;
   int nValue;
   StrBlockInfo *pStrBlock;
   int nIndex, *pnMoveLeft, *pnMoveRight, *pnSplitAtIndex, *pnInsertBefore;
{
   if (nIndex == 0) {
      if (pStrBlock->prev == NULL) {
         *pnInsertBefore = TRUE;
      } else if (pStrBlock->prev->type == SB_SIMPLE) {
         if (SameProperty(lWhich, nValue, pStrBlock->prev->seg, FALSE)) {
            *pnMoveLeft = TRUE;
         } else {
            *pnInsertBefore = TRUE;
         }
      } else {
         *pnInsertBefore = TRUE;
      }
   } else if (nIndex == pStrBlock->seg->dyn_str.sz-1) {
      if (pStrBlock->next == NULL) {
         *pnInsertBefore = FALSE;
      } else if (pStrBlock->next->type == SB_SIMPLE) {
         if (SameProperty(lWhich, nValue, pStrBlock->next->seg, FALSE)) {
            *pnMoveRight = TRUE;
         } else {
            *pnInsertBefore = FALSE;
         }
      } else {
         *pnInsertBefore = FALSE;
      }
   } else {
      *pnSplitAtIndex = TRUE;
   }
}

static
int ChangeEditTextPropertyAtCursor(lWhich, nValue)
   long lWhich;
   int nValue;
{
   StrBlockInfo *pNewStrBlock=NULL;
   MiniLineInfo *owner_mini_line=curStrBlock->owner_mini_line;
   int move_left=FALSE, move_right=FALSE;
   int split_at_index=FALSE, insert_before=FALSE;

   DetermineWhatToDoForChangeProperty(lWhich, nValue, curStrBlock, textCurIndex,
         &move_left, &move_right, &split_at_index, &insert_before);
   if (move_left) {
      curStrBlock = curStrBlock->prev;
      textCurIndex = curStrBlock->seg->dyn_str.sz-1;
   } else if (move_right) {
      curStrBlock = curStrBlock->next;
      textCurIndex = 0;
   } else {
      DupStrBlock(curStrBlock, owner_mini_line, &pNewStrBlock,
            &pNewStrBlock);
      DynStrSet(&pNewStrBlock->seg->dyn_str, "");
      if (lWhich == PROP_MASK_TEXT_FONT &&
            curStrBlock->seg->double_byte != IsFontDoubleByte(nValue)) {
         pNewStrBlock->seg->double_byte = IsFontDoubleByte(nValue);
         if (pNewStrBlock->seg->double_byte) {
            pNewStrBlock->seg->double_byte_vertical =
                  IsDoubleByteFontVertical(nValue, pNewStrBlock->seg->style);
         } else {
            pNewStrBlock->seg->double_byte_vertical = FALSE;
         }
      }
      if (!SetStrSegProperty(lWhich, nValue, pNewStrBlock->seg)) {
         /*
          * don't call UnlinkStrBlock() because DupStrBlock() does not
          *       insert pNewStrBlock into owner_mini_line
          */
         FreeStrBlock(pNewStrBlock);
         return FALSE;
      }
      if (split_at_index) {
         StrBlockInfo *pLeftStrBlock=NULL;
         char *psz=UtilStrDup(curStrBlock->seg->dyn_str.s), saved_ch='\0';

         if (psz == NULL) FailAllocMessage();

         DupStrBlock(curStrBlock, owner_mini_line, &pLeftStrBlock,
               &pLeftStrBlock);
         saved_ch = psz[textCurIndex];
         psz[textCurIndex] = '\0';
         DynStrSet(&pLeftStrBlock->seg->dyn_str, psz);
         psz[textCurIndex] = saved_ch;
         DynStrSet(&curStrBlock->seg->dyn_str, &psz[textCurIndex]);
         UtilFree(psz);

         pLeftStrBlock->prev = curStrBlock->prev;
         if (curStrBlock->prev == NULL) {
            owner_mini_line->first_block = pLeftStrBlock;
         } else {
            curStrBlock->prev->next = pLeftStrBlock;
         }
         pLeftStrBlock->next = pNewStrBlock;
         pNewStrBlock->prev = pLeftStrBlock;

         pNewStrBlock->next = curStrBlock;
         curStrBlock->prev = pNewStrBlock;
      } else if (insert_before) {
         pNewStrBlock->prev = curStrBlock->prev;
         if (curStrBlock->prev == NULL) {
            owner_mini_line->first_block = pNewStrBlock;
         } else {
            curStrBlock->prev->next = pNewStrBlock;
         }
         pNewStrBlock->next = curStrBlock;
         curStrBlock->prev = pNewStrBlock;
      } else {
         /* insert after */
         pNewStrBlock->next = curStrBlock->next;
         if (curStrBlock->next == NULL) {
            owner_mini_line->last_block = pNewStrBlock;
         } else {
            curStrBlock->next->prev = pNewStrBlock;
         }
         pNewStrBlock->prev = curStrBlock;
         curStrBlock->next = pNewStrBlock;
      }
      curStrBlock = pNewStrBlock;
      textCurIndex = 0;
   }
   return TRUE;
}

int ChangeEditTextProperty(lWhich, nValue)
   long lWhich;
   int nValue;
   /* returns TRUE if anything is changed */
{
   int changed=FALSE;

   ResetDirtyBBoxInfo();

   PushCurFont();
   if (editTextSize != 0) {
      if (RestoreEditTextSize(curTextObj, TRUE)) {
         UpdTextBBox(curTextObj);
      }
   }
   if (curStrBlock->type == SB_SUPSUB_CENTER) {
      if (SameProperty(lWhich, nValue, curStrBlock->seg, TRUE)) {
         /* nothing need to change */
         changed = FALSE;
      } else {
         int saved_double_byte=INVALID;
         int saved_double_byte_vertical=INVALID;
         int saved_double_byte_mod_bytes=INVALID;

         if (lWhich == PROP_MASK_TEXT_FONT &&
               curStrBlock->seg->double_byte != IsFontDoubleByte(nValue)) {
            saved_double_byte = curStrBlock->seg->double_byte;
            saved_double_byte_vertical = curStrBlock->seg->double_byte_vertical;
            saved_double_byte_mod_bytes =
                  curStrBlock->seg->double_byte_mod_bytes;
            curStrBlock->seg->double_byte = IsFontDoubleByte(nValue);
         }
         changed = SetStrSegProperty(lWhich, nValue, curStrBlock->seg);
         if (!changed && saved_double_byte != INVALID) {
            curStrBlock->seg->double_byte = saved_double_byte;
            curStrBlock->seg->double_byte_vertical = saved_double_byte_vertical;
            curStrBlock->seg->double_byte_mod_bytes =
                  saved_double_byte_mod_bytes;
         }
      }
   } else {
      if (textHighlight) {
         changed = ChangeHighlightedTextProperty(lWhich, nValue);
      } else {
         if (SameProperty(lWhich, nValue, curStrBlock->seg, FALSE)) {
            /* nothing need to change */
            changed = FALSE;
         } else if (textCurIndex == 0 && curStrBlock->seg->dyn_str.sz == 1) {
            int saved_double_byte=INVALID;
            int saved_double_byte_vertical=INVALID;
            int saved_double_byte_mod_bytes=INVALID;

            if (lWhich == PROP_MASK_TEXT_FONT &&
                  curStrBlock->seg->double_byte != IsFontDoubleByte(nValue)) {
               saved_double_byte = curStrBlock->seg->double_byte;
               saved_double_byte_vertical =
                     curStrBlock->seg->double_byte_vertical;
               saved_double_byte_mod_bytes =
                     curStrBlock->seg->double_byte_mod_bytes;
               curStrBlock->seg->double_byte = IsFontDoubleByte(nValue);
            }
            changed = SetStrSegProperty(lWhich, nValue, curStrBlock->seg);
            if (!changed && saved_double_byte != INVALID) {
               curStrBlock->seg->double_byte = saved_double_byte;
               curStrBlock->seg->double_byte_vertical =
                     saved_double_byte_vertical;
               curStrBlock->seg->double_byte_mod_bytes =
                     saved_double_byte_mod_bytes;
            }
         } else {
            changed = ChangeEditTextPropertyAtCursor(lWhich, nValue);
         }
      }
   }
   if (editTextSize != 0) {
      if (RestoreEditTextSize(curTextObj, FALSE)) {
         UpdTextBBox(curTextObj);
      }
   }
   PopCurFont();

   if (changed) {
      EndChangeCurText(FALSE);
      if (editTextSize != 0 && curStrBlock != NULL &&
            (curStrBlock->type == SB_SIMPLE ||
            curStrBlock->type == SB_SUPSUB_CENTER)) {
         CurFontMsg(TRUE, TRUE, curStrBlock->seg);
      } else {
         CurFontMsg(TRUE, TRUE, NULL);
      }
      SetFileModified(TRUE);
   }
   return changed;
}

static
void SplitCurStrBlock()
{
   StrBlockInfo *pLeftStrBlock=NULL;
   MiniLineInfo *owner_mini_line=curStrBlock->owner_mini_line;
   char *buf=NULL, saved_ch='\0';

   DupStrBlock(curStrBlock, owner_mini_line, &pLeftStrBlock, &pLeftStrBlock);
   buf = curStrBlock->seg->dyn_str.s;
   saved_ch = buf[textCurIndex];
   buf[textCurIndex] = '\0';
   DynStrSet(&pLeftStrBlock->seg->dyn_str, buf);
   buf[textCurIndex] = saved_ch;
   DynStrSet(&curStrBlock->seg->dyn_str, &buf[textCurIndex]);

   pLeftStrBlock->prev = curStrBlock->prev;
   if (curStrBlock->prev == NULL) {
      owner_mini_line->first_block = pLeftStrBlock;
   } else {
      curStrBlock->prev->next = pLeftStrBlock;
   }
   pLeftStrBlock->next = curStrBlock;
   curStrBlock->prev = pLeftStrBlock;

   textCurIndex = 0;
}

void PasteMiniLinesAtCursor(partial_text_obj_ptr)
   struct ObjRec *partial_text_obj_ptr;
{
   struct TextRec *src_text_ptr=partial_text_obj_ptr->detail.t;
   MiniLinesInfo *src_minilines=(&src_text_ptr->minilines);
   MiniLineInfo *src_mini_line=NULL, *next_mini_line=NULL;
   MiniLineInfo *pFirstMiniLine=NULL, *pLastMiniLine=NULL;
   StrBlockInfo *src_str_block=NULL, *next_str_block=NULL;
   StrBlockInfo *pFirstStrBlock=NULL, *pLastStrBlock=NULL;

   ResetDirtyBBoxInfo();
   if (textHighlight) {
      DeleteHighlightedText();
      EndChangeCurText(FALSE);
   }
   if (editTextSize != 0) {
      if (RestoreEditTextSize(curTextObj, TRUE)) {
         UpdTextBBox(curTextObj);
      }
   }
   SplitCurStrBlock();

   pFirstStrBlock = curStrBlock->owner_mini_line->first_block;
   pLastStrBlock = curStrBlock->owner_mini_line->last_block;
   for (src_str_block=src_minilines->first->first_block; src_str_block != NULL;
         src_str_block=next_str_block) {
      next_str_block = src_str_block->next;
      src_str_block->owner_mini_line = curStrBlock->owner_mini_line;
      InsertStrBlock(curStrBlock->prev, curStrBlock, src_str_block,
            &pFirstStrBlock, &pLastStrBlock);
   }
   curStrBlock->owner_mini_line->first_block = pFirstStrBlock;
   curStrBlock->owner_mini_line->last_block = pLastStrBlock;

   if (src_minilines->first->next != NULL) {
      InsertCRLFIntoCurText();
      EndChangeCurText(TRUE);

      pFirstMiniLine = curStrBlock->owner_mini_line->owner_minilines->first;
      pLastMiniLine = curStrBlock->owner_mini_line->owner_minilines->last;
      for (src_mini_line=src_minilines->first->next;
            src_mini_line->next != NULL; src_mini_line=next_mini_line) {
         next_mini_line = src_mini_line->next;
         src_mini_line->owner_minilines =
               curStrBlock->owner_mini_line->owner_minilines;
         InsertMiniLine(curStrBlock->owner_mini_line->prev,
               curStrBlock->owner_mini_line, src_mini_line, &pFirstMiniLine,
               &pLastMiniLine);
      }
      curStrBlock->owner_mini_line->owner_minilines->first = pFirstMiniLine;
      curStrBlock->owner_mini_line->owner_minilines->last = pLastMiniLine;

      pFirstStrBlock = curStrBlock->owner_mini_line->first_block;
      pLastStrBlock = curStrBlock->owner_mini_line->last_block;
      for (src_str_block=src_mini_line->first_block;
            src_str_block != NULL; src_str_block=next_str_block) {
         next_str_block = src_str_block->next;
         src_str_block->owner_mini_line = curStrBlock->owner_mini_line;
         InsertStrBlock(curStrBlock->prev, curStrBlock, src_str_block,
               &pFirstStrBlock, &pLastStrBlock);
      }
      curStrBlock->owner_mini_line->first_block = pFirstStrBlock;
      curStrBlock->owner_mini_line->last_block = pLastStrBlock;
   }
   if (editTextSize != 0) {
      if (RestoreEditTextSize(curTextObj, FALSE)) {
         UpdTextBBox(curTextObj);
      }
   }
   EndChangeCurText(TRUE);
   MarkRulers(textCurX, textCurY);
   SetFileModified(TRUE);

   ScrollTo(textCurX, textCurBaselineY);

   if (src_minilines->first != NULL) {
      if (src_minilines->first != src_minilines->last) {
         free(src_minilines->last);
      }
      free(src_minilines->first);
      src_minilines->first = src_minilines->last = NULL;
   }
}

/* --------------------- Insert Special Routines --------------------- */

static
void DoMoveEditText(dx, dy)
   int dx, dy;
{
   struct TextRec *text_ptr=curTextObj->detail.t;
   int abs_dx=ABS_SIZE(dx), abs_dy=ABS_SIZE(dy), w=0, h=0;

   curTextMovedAbsX += abs_dx;
   curTextMovedAbsY += abs_dy;

   textAbsX += abs_dx;
   textAbsY += abs_dy;
   textAbsBaselineY += abs_dy;

   textOrigX = OFFSET_X(textAbsX);
   textOrigY = OFFSET_Y(textAbsY);
   textOrigBaselineY = textOrigY + text_ptr->baseline_y - curTextObj->y;

   SetTextCurXY();
   if (textHighlight) SetTextEndXY();

   w = text_ptr->minilines.w;
   h = (textOrigBaselineY - textOrigY) +
         (text_ptr->minilines.h - text_ptr->minilines.first->asc);

   SetEditTextArea(w, h, text_ptr->minilines.min_lbearing,
         text_ptr->minilines.max_rextra);
}

static
void ContinueMoveEditText(OrigX, OrigY)
   int OrigX, OrigY;
{
   int done=FALSE, abort=FALSE, dx=0, dy=0;
   int saved_x=OrigX, saved_y=OrigY;
   XEvent input, ev;
   struct BBRec bbox;

   GetCurTextBBoxes(NULL, &bbox);
   InflateBBox(&bbox, -2, -2, &bbox);

   if (!debugNoPointerGrab) {
      XGrabPointer(mainDisplay, drawWindow, FALSE,
            PointerMotionMask | ButtonReleaseMask,
            GrabModeAsync, GrabModeAsync, None, moveCursor, CurrentTime);
   }
   while (!done) {
      XNextEvent(mainDisplay, &input);

      if (input.type == Expose || input.type == VisibilityNotify) {
         ExposeEventHandler(&input, TRUE);
      } else if (input.type == ButtonRelease) {
         XUngrabPointer(mainDisplay, CurrentTime);
         SelBox(drawWindow, revDefaultGC, bbox.ltx+dx, bbox.lty+dy,
               bbox.rbx+dx, bbox.rby+dy);
         done = TRUE;
      } else if (input.type == MotionNotify) {
         int cur_x=input.xmotion.x, cur_y=input.xmotion.y;

         if (cur_x != saved_x || cur_y != saved_y) {
            SelBox(drawWindow, revDefaultGC, bbox.ltx+dx, bbox.lty+dy,
                  bbox.rbx+dx, bbox.rby+dy);
            saved_x = cur_x;
            saved_y = cur_y;
            dx = saved_x-OrigX;
            dy = saved_y-OrigY;
            SelBox(drawWindow, revDefaultGC, bbox.ltx+dx, bbox.lty+dy,
                  bbox.rbx+dx, bbox.rby+dy);
         }
         while (XCheckMaskEvent(mainDisplay, PointerMotionMask, &ev)) ;
      } else if (input.type == KeyPress) {
         if (KeyPressEventIsEscape(&input.xkey)) {
            XUngrabPointer(mainDisplay, CurrentTime);
            SelBox(drawWindow, revDefaultGC, bbox.ltx+dx, bbox.lty+dy,
                  bbox.rbx+dx, bbox.rby+dy);
            abort = TRUE;
            done = TRUE;
         }
      }
   }
   SelBox(drawWindow, revDefaultGC, bbox.ltx, bbox.lty, bbox.rbx, bbox.rby);
   if (debugNoPointerGrab) XSync(mainDisplay, False);

   if (!abort && (OrigX != saved_x || OrigY != saved_y)) {
      int abs_dx=ABS_SIZE(dx), abs_dy=ABS_SIZE(dy);
      struct BBRec new_bbox;

      bbox.ltx = ABS_X(bbox.ltx); bbox.lty = ABS_Y(bbox.lty);
      bbox.rbx = ABS_X(bbox.rbx); bbox.rby = ABS_Y(bbox.rby);
      SetBBRec(&new_bbox, bbox.ltx+abs_dx, bbox.lty+abs_dy, bbox.rbx+abs_dx,
             bbox.rby+abs_dy);
      InflateBBox(&bbox, -(curTextOutlineHalfW<<1), -(curTextOutlineHalfW<<1),
            &bbox);

      DoMoveEditText(dx, dy);

      RedrawAreas(botObj, bbox.ltx-GRID_ABS_SIZE(2), bbox.lty-GRID_ABS_SIZE(2),
            bbox.rbx+GRID_ABS_SIZE(2), bbox.rby+GRID_ABS_SIZE(2),
            new_bbox.ltx-GRID_ABS_SIZE(2), new_bbox.lty-GRID_ABS_SIZE(2),
            new_bbox.rbx+GRID_ABS_SIZE(2), new_bbox.rby+GRID_ABS_SIZE(2));
      RedrawCurText();
   }
   ShowCurChoiceMouseStatus(curChoice, 0, FALSE);
}

int MouseInCurText(input)
   XEvent *input;
{
   if (curChoice == DRAWTEXT && textCursorShown) {
      int mouse_x=input->xbutton.x, mouse_y=input->xbutton.y;
      struct BBRec bbox;

      GetCurTextBBoxes(NULL, &bbox);
      InflateBBox(&bbox, -(curTextOutlineHalfW<<1), -(curTextOutlineHalfW<<1),
            &bbox);
      if (PointInBBox(mouse_x, mouse_y, bbox)) {
         return TRUE;
      }
   }
   return FALSE;
}

int MouseOnCurTextBoundary(input)
   XEvent *input;
{
   if (MouseInCurText(input)) {
      int mouse_x=input->xbutton.x, mouse_y=input->xbutton.y;
      struct BBRec bbox;

      GetCurTextBBoxes(NULL, &bbox);
      if (!PointInBBox(mouse_x, mouse_y, bbox)) {
         return TRUE;
      }
   }
   return FALSE;
}

void MoveEditText(input)
   XEvent *input;
{
   if (MouseInCurText(input)) {
      ContinueMoveEditText(input->xbutton.x, input->xbutton.y);
   }
}

void MoveEditTextBox()
{
   unsigned int button;
   int mouse_x=0, mouse_y=0;

   Msg(TgLoadString(STID_DRAG_MOUSE_MOVE_EDITTEXT_DOTS));
   SetMouseStatus(TgLoadCachedString(CSTID_START_MOVE_EDIT_TEXT_BOX),
         TgLoadCachedString(CSTID_CANCEL), TgLoadCachedString(CSTID_CANCEL));
   button = DrawWindowLoop(&mouse_x, &mouse_y, handCursor, FALSE);
   if (button == Button1) {
      ContinueMoveEditText(mouse_x, mouse_y);
   } else {
      Msg(TgLoadString(STID_MOVE_EDIT_TEXT_BOX_ABORTED));
   }
}
