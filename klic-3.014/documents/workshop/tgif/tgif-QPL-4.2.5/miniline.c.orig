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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/miniline.c,v 1.38 2011/05/18 22:41:20 william Exp $
 */

#define _INCLUDE_FROM_MINILINE_C_

#include "tgifdefs.h"

#include "auxtext.e"
#include "choice.e"
#include "color.e"
#include "cutpaste.e"
#include "dialog.e"
#include "drawing.e"
#include "dup.e"
#include "file.e"
#include "font.e"
#include "inmethod.e"
#include "miniline.e"
#include "move.e"
#include "msg.e"
#include "obj.e"
#include "pattern.e"
#include "ps.e"
#include "raster.e"
#include "rect.e"
#include "setup.e"
#include "strtbl.e"
#include "text.e"
#include "util.e"
#include "wb.e"

#define USE_SEG 0
#define USE_SUP 1
#define USE_SUB 2

TextHighlightInfo gstTextHighlightInfo;

CopyUTF8Info gstCopyUTF8Info;

/*
 * textOrigX, textOrigY, textCurX, textCurY, textCurBaselineY are
 *       UNSCALED screen offsets
 * textAbsX, and textAbsY are absolute coordinates
 */
int textOrigX=20, textOrigY=20, textOrigBaselineY=20;
int textAbsX=INVALID, textAbsY=INVALID, textAbsBaselineY=INVALID;

int textCurX=20, textCurY=20, textCurBaselineY=0;
int textCurIndex=0;
int textEndX=20, textEndY=20, textEndBaselineY=0;
int textEndIndex=0;
/*
 * The following is updated to grown in UpdateEditTextArea().
 */
int textAbsMinLBearing=0, textAbsMaxRExtra=0;
int textW=0, textH=0; /* absolute for the current text font */

int textHighlight=FALSE;

struct ObjRec *curTextObj=NULL;

struct BBRec curTextOBBox, curTextBBox;

StrBlockInfo *curStrBlock=NULL; /* block must always be SB_SIMPLE */
StrBlockInfo *endStrBlock=NULL; /* block must always be SB_SIMPLE */
MiniLineInfo *firstMiniLine=NULL, *lastMiniLine=NULL;

int escPressed=FALSE;

int dontRecalcStrSegMetrics=FALSE;

int curTextOutlineW=7, curTextOutlineHalfW=3;

int drawWinHasFocus=FALSE;

/* --------------------- ClearCopyUTF8Info() --------------------- */

void ClearCopyUTF8Info()
{
   memset(&gstCopyUTF8Info, 0, sizeof(CopyUTF8Info));
}

static
void DebugCopyUTF8Info()
{
#ifdef NOT_DEFINED
   if (gstCopyUTF8Info.single_byte_valid) {
      fprintf(stderr, "Single byte:\n");
      fprintf(stderr, "\tFont: %s\n",
            fontMenuStr[gstCopyUTF8Info.single_byte_seg.font]);
      fprintf(stderr, "\tStyle: %1d\n",
            gstCopyUTF8Info.single_byte_seg.style);
      fprintf(stderr, "\tSize: %1d\n",
            SzUnitToFontSize(gstCopyUTF8Info.single_byte_seg.sz_unit));
   }
   if (gstCopyUTF8Info.double_byte_valid) {
      fprintf(stderr, "Double byte:\n");
      fprintf(stderr, "\tFont: %s\n",
            fontMenuStr[gstCopyUTF8Info.double_byte_seg.font]);
      fprintf(stderr, "\tStyle: %1d\n",
            gstCopyUTF8Info.double_byte_seg.style);
      fprintf(stderr, "\tSize: %1d\n",
            SzUnitToFontSize(gstCopyUTF8Info.double_byte_seg.sz_unit));
   }
#endif /* NOT_DEFINED */
}

void SetCopyUTF8FontInfo(pStrSeg, double_byte)
   StrSegInfo *pStrSeg;
   int double_byte;
{
   if (double_byte) {
      memcpy(&gstCopyUTF8Info.double_byte_seg, pStrSeg, sizeof(StrSegInfo));
      gstCopyUTF8Info.double_byte_seg.font_name = NULL;
      gstCopyUTF8Info.double_byte_seg.dyn_str.sz = 0;
      gstCopyUTF8Info.double_byte_seg.dyn_str.s = NULL;
      gstCopyUTF8Info.double_byte_seg.owner = NULL;

      gstCopyUTF8Info.double_byte_valid = TRUE;
   } else {
      memcpy(&gstCopyUTF8Info.single_byte_seg, pStrSeg, sizeof(StrSegInfo));
      gstCopyUTF8Info.single_byte_seg.font_name = NULL;
      gstCopyUTF8Info.single_byte_seg.dyn_str.sz = 0;
      gstCopyUTF8Info.single_byte_seg.dyn_str.s = NULL;
      gstCopyUTF8Info.single_byte_seg.owner = NULL;

      gstCopyUTF8Info.single_byte_valid = TRUE;
   }
}

int CalcSingleByteInfoForCopyUTF8(pn_font_index)
   int *pn_font_index;
{
   StrBlockInfo *pStrBlock=NULL;

   if (curStrBlock != endStrBlock || curStrBlock->type != SB_SIMPLE ||
         !curStrBlock->seg->double_byte) {
      return FALSE;
   }
   for (pStrBlock=curStrBlock->prev; pStrBlock != NULL;
         pStrBlock=pStrBlock->prev) {
      if (pStrBlock->type == SB_SIMPLE && !pStrBlock->seg->double_byte) {
         SetCopyUTF8FontInfo(pStrBlock->seg, FALSE);
         DebugCopyUTF8Info();
         return TRUE;
      }
   }
   return FALSE;
}

static
int FoundSingleByteFontAtStrBlock(pStrBlock, pStrSeg, double_byte)
   StrBlockInfo *pStrBlock;
   StrSegInfo *pStrSeg;
   int double_byte;
{
   if (pStrBlock->type == SB_SIMPLE &&
         pStrBlock->seg->double_byte == double_byte) {
      SetCopyUTF8FontInfo(pStrBlock->seg, FALSE);
      DebugCopyUTF8Info();

      if (pStrSeg != NULL) {
         memcpy(pStrSeg, pStrBlock->seg, sizeof(StrSegInfo));
         pStrSeg->font_name = NULL;
         pStrSeg->dyn_str.sz = 0;
         pStrSeg->dyn_str.s = NULL;
         pStrSeg->owner = NULL;
      }
      return TRUE;
   }
   return FALSE;
}

static
int CanFindSingleOrDoubleByteFontAtCursor(pStrSeg, double_byte)
   StrSegInfo *pStrSeg;
   int double_byte;
{
   StrBlockInfo *pStrBlock=NULL;
   MiniLineInfo *pMiniLine=NULL;

   if (curStrBlock->type != SB_SIMPLE) {
      return FALSE;
   }
   if (curStrBlock->seg->double_byte == double_byte) {
      return TRUE;
   }
   for (pStrBlock=curStrBlock->prev; pStrBlock != NULL;
         pStrBlock=pStrBlock->prev) {
      if (FoundSingleByteFontAtStrBlock(pStrBlock, pStrSeg, double_byte)) {
         return TRUE;
      }
   }
   for (pMiniLine=curStrBlock->owner_mini_line->prev; pMiniLine != NULL;
         pMiniLine=pMiniLine->prev) {
      for (pStrBlock=pMiniLine->last_block; pStrBlock != NULL;
            pStrBlock=pStrBlock->prev) {
         if (FoundSingleByteFontAtStrBlock(pStrBlock, pStrSeg, double_byte)) {
            return TRUE;
         }
      }
   }
   for (pStrBlock=curStrBlock->next; pStrBlock != NULL;
         pStrBlock=pStrBlock->next) {
      if (FoundSingleByteFontAtStrBlock(pStrBlock, pStrSeg, double_byte)) {
         return TRUE;
      }
   }
   for (pMiniLine=curStrBlock->owner_mini_line->next; pMiniLine != NULL;
         pMiniLine=pMiniLine->next) {
      for (pStrBlock=pMiniLine->first_block; pStrBlock != NULL;
            pStrBlock=pStrBlock->next) {
         if (FoundSingleByteFontAtStrBlock(pStrBlock, pStrSeg, double_byte)) {
            return TRUE;
         }
      }
   }
   return FALSE;
}

int CanFindSingleByteFontAtCursor(pStrSeg)
   StrSegInfo *pStrSeg;
{
   return CanFindSingleOrDoubleByteFontAtCursor(pStrSeg, FALSE);
}

int CanFindDoubleByteFontAtCursor(pStrSeg)
   StrSegInfo *pStrSeg;
{
   return CanFindSingleOrDoubleByteFontAtCursor(pStrSeg, TRUE);
}

/* --------------------- DrawTextInfo Functions --------------------- */

static DrawTextInfo gstDrawTextInfo;
static int gnDrawTextInfoValid=FALSE;

void SaveDrawTextInfo(pDrawTextInfo)
   DrawTextInfo *pDrawTextInfo;
{
   memset(pDrawTextInfo, 0, sizeof(DrawTextInfo));

   memcpy(&pDrawTextInfo->text_highlight_info, &gstTextHighlightInfo,
         sizeof(TextHighlightInfo));

   pDrawTextInfo->text_orig_x = textOrigX;
   pDrawTextInfo->text_orig_y = textOrigY;
   pDrawTextInfo->text_orig_baseline_y = textOrigBaselineY;
   pDrawTextInfo->text_abs_x = textAbsX;
   pDrawTextInfo->text_abs_y = textAbsY;
   pDrawTextInfo->text_abs_baseline_y = textAbsBaselineY;

   pDrawTextInfo->text_cur_x = textCurX;
   pDrawTextInfo->text_cur_y = textCurY;
   pDrawTextInfo->text_cur_baseline_y = textCurBaselineY;
   pDrawTextInfo->text_cur_index = textCurIndex;
   pDrawTextInfo->text_end_x = textEndX;
   pDrawTextInfo->text_end_y = textEndY;
   pDrawTextInfo->text_end_baseline_y = textEndBaselineY;
   pDrawTextInfo->text_end_index = textEndIndex;

   pDrawTextInfo->text_abs_min_lbearing = textAbsMinLBearing;
   pDrawTextInfo->text_abs_max_rextra = textAbsMaxRExtra;
   pDrawTextInfo->text_w = textW;
   pDrawTextInfo->text_h = textH;

   pDrawTextInfo->text_highlight = textHighlight;

   pDrawTextInfo->cur_text_obj = curTextObj;

   memcpy(&pDrawTextInfo->cur_text_obbox, &curTextOBBox, sizeof(struct BBRec));
   memcpy(&pDrawTextInfo->cur_text_bbox, &curTextBBox, sizeof(struct BBRec));

   pDrawTextInfo->cur_str_block = curStrBlock;
   pDrawTextInfo->end_str_block = endStrBlock;
   pDrawTextInfo->first_miniLine = firstMiniLine;
   pDrawTextInfo->last_miniLine = lastMiniLine;

   pDrawTextInfo->esc_pressed = escPressed;

   pDrawTextInfo->cur_text_outline_w = curTextOutlineW;
   pDrawTextInfo->cur_text_outline_half_w = curTextOutlineHalfW;

   /* the following are from text.e */

   pDrawTextInfo->text_drawn = textDrawn;   
   pDrawTextInfo->cur_text_modified = curTextModified;
   pDrawTextInfo->text_vspace = textVSpace;  

   pDrawTextInfo->text_just = textJust;    
   pDrawTextInfo->text_cursor_shown = textCursorShown;
   pDrawTextInfo->text_cursor_h = textCursorH; 
   pDrawTextInfo->editing_text = editingText; 
   pDrawTextInfo->cur_text_is_new = curTextIsNew; 

   pDrawTextInfo->edit_text_size = editTextSize;
}

void RestoreDrawTextInfo(pDrawTextInfo)
   DrawTextInfo *pDrawTextInfo;
{
   memcpy(&gstTextHighlightInfo, &pDrawTextInfo->text_highlight_info,
         sizeof(TextHighlightInfo));

   textOrigX = pDrawTextInfo->text_orig_x;
   textOrigY = pDrawTextInfo->text_orig_y;
   textOrigBaselineY = pDrawTextInfo->text_orig_baseline_y;
   textAbsX = pDrawTextInfo->text_abs_x;
   textAbsY = pDrawTextInfo->text_abs_y;
   textAbsBaselineY = pDrawTextInfo->text_abs_baseline_y;

   textCurX = pDrawTextInfo->text_cur_x;
   textCurY = pDrawTextInfo->text_cur_y;
   textCurBaselineY = pDrawTextInfo->text_cur_baseline_y;
   textCurIndex = pDrawTextInfo->text_cur_index;
   textEndX = pDrawTextInfo->text_end_x;
   textEndY = pDrawTextInfo->text_end_y;
   textEndBaselineY = pDrawTextInfo->text_end_baseline_y;
   textEndIndex = pDrawTextInfo->text_end_index;

   textAbsMinLBearing = pDrawTextInfo->text_abs_min_lbearing;
   textAbsMaxRExtra = pDrawTextInfo->text_abs_max_rextra;
   textW = pDrawTextInfo->text_w;
   textH = pDrawTextInfo->text_h;

   textHighlight = pDrawTextInfo->text_highlight;

   curTextObj = pDrawTextInfo->cur_text_obj;

   memcpy(&curTextOBBox, &pDrawTextInfo->cur_text_obbox, sizeof(struct BBRec));
   memcpy(&curTextBBox, &pDrawTextInfo->cur_text_bbox, sizeof(struct BBRec));

   curStrBlock = pDrawTextInfo->cur_str_block;
   endStrBlock = pDrawTextInfo->end_str_block;
   firstMiniLine = pDrawTextInfo->first_miniLine;
   lastMiniLine = pDrawTextInfo->last_miniLine;

   escPressed = pDrawTextInfo->esc_pressed;

   curTextOutlineW = pDrawTextInfo->cur_text_outline_w;
   curTextOutlineHalfW = pDrawTextInfo->cur_text_outline_half_w;

   /* the following are from text.e */

   textDrawn = pDrawTextInfo->text_drawn;
   curTextModified = pDrawTextInfo->cur_text_modified;
   textVSpace = pDrawTextInfo->text_vspace;

   textJust = pDrawTextInfo->text_just;
   textCursorShown = pDrawTextInfo->text_cursor_shown;
   textCursorH = pDrawTextInfo->text_cursor_h;
   editingText = pDrawTextInfo->editing_text;
   curTextIsNew = pDrawTextInfo->cur_text_is_new;

   editTextSize = pDrawTextInfo->edit_text_size;
}

void ResetDrawTextInfo()
{
   memset(&gstDrawTextInfo, 0, sizeof(DrawTextInfo));

   RestoreDrawTextInfo(&gstDrawTextInfo);
}

void SaveDrawWinDrawTextInfo(forced)
   int forced;
{
   if (gstWBInfo.do_whiteboard) {
      if (curChoice == DRAWTEXT) {
         if (!forced && textCursorShown) {
            PutTextCursor();
         }
         SaveDrawTextInfo(&gstDrawTextInfo);
         gnDrawTextInfoValid = TRUE;
      }
      if (!forced) {
         drawWinHasFocus = FALSE;
      }
   }
}

void RestoreDrawWinDrawTextInfo(forced)
   int forced;
{
   if (!forced) {
      drawWinHasFocus = TRUE;
   }
   if (gstWBInfo.do_whiteboard) {
      if (gnDrawTextInfoValid && curChoice == DRAWTEXT) {
         if (forced) {
            RestoreDrawTextInfo(&gstDrawTextInfo);
         }
         if (!forced) {
            gnDrawTextInfoValid = FALSE;
            if (textCursorShown) {
               PutTextCursor();
            }
         }
      }
   }
}

/* --------------------- DirtyBBox Functions --------------------- */

static DirtyBBoxInfo gstDirtyBBoxInfo;

void ResetDirtyBBoxInfo()
{
   memset(&gstDirtyBBoxInfo, 0, sizeof(DirtyBBoxInfo));
   gstDirtyBBoxInfo.valid = FALSE;
   gstDirtyBBoxInfo.force_redraw_all = FALSE;
}

void AddToDirtyBBox(pBBox)
   struct BBRec *pBBox;
{
   if (gstDirtyBBoxInfo.valid) {
      UnionRect(&gstDirtyBBoxInfo.bbox, pBBox, &gstDirtyBBoxInfo.bbox);
   } else {
      gstDirtyBBoxInfo.valid = TRUE;
      memcpy(&gstDirtyBBoxInfo.bbox, pBBox, sizeof(struct BBRec));
   }
}

void ForceDirtyBBoxToRedrawAll()
{
   gstDirtyBBoxInfo.force_redraw_all = TRUE;
}

int GetDirtyBBox(pBBox)
   struct BBRec *pBBox;
{
   if (pBBox != NULL) {
      if (gstDirtyBBoxInfo.valid) {
         memcpy(pBBox, &gstDirtyBBoxInfo.bbox, sizeof(struct BBRec));
      } else {
         memset(pBBox, 0, sizeof(struct BBRec));
      }
   }
   return (gstDirtyBBoxInfo.valid && !gstDirtyBBoxInfo.force_redraw_all);
}

static
int IntersectDirtyBBox(pBBox)
   struct BBRec *pBBox;
{
   return BBoxIntersect(*pBBox, gstDirtyBBoxInfo.bbox);
}

/* --------------------- Indent Functions --------------------- */

static char **gaszIndentStrings=NULL;
static int gnMaxIndent=0;

void CleanUpIndentStrings()
{
   if (gaszIndentStrings != NULL) {
      int i=0;

      for (i=0; i < gnMaxIndent; i++) {
         UtilFree(gaszIndentStrings[i]);
      }
      free(gaszIndentStrings);
   }
   gaszIndentStrings = NULL;
   gnMaxIndent = 0;
}

static
void InitIndentStrings()
{
   gaszIndentStrings = NULL;
   gnMaxIndent = 0;
}

char *GetIndentString(indent)
   int indent;
{
   int i=0;

   if (indent < 0) return NULL;
   if (indent == 0) return "";

   if (gaszIndentStrings == NULL) {
      gaszIndentStrings = (char**)malloc(indent*sizeof(char*));
      if (gaszIndentStrings == NULL) FailAllocMessage();
      memset(gaszIndentStrings, 0, indent*sizeof(char*));
      gnMaxIndent = indent;
   } else if (indent > gnMaxIndent) {
      gaszIndentStrings = (char**)realloc(gaszIndentStrings,
            indent*sizeof(char*));
      if (gaszIndentStrings == NULL) FailAllocMessage();
      for (i=gnMaxIndent; i < indent; i++) {
         gaszIndentStrings[i] = NULL;
      }
      gnMaxIndent = indent;
   } else {
      if (gaszIndentStrings[indent-1] != NULL) {
         return gaszIndentStrings[indent-1];
      }
   }
   gaszIndentStrings[indent-1] = (char*)malloc((indent+1)*sizeof(char));
   if (gaszIndentStrings[indent-1] == NULL) FailAllocMessage();
   memset(gaszIndentStrings[indent-1], 0, sizeof((indent+1)*sizeof(char)));

   for (i=0; i < indent; i++) {
      gaszIndentStrings[indent-1][i] = ' ';
   }
   gaszIndentStrings[indent-1][i] = '\0';

   return gaszIndentStrings[indent-1];
}

void DumpIndentString(FP, indent)
   FILE *FP;
   int indent;
{
   char *psz=GetIndentString(indent);

   if (psz != NULL) {
      fprintf(FP, "%s", psz);
   }
}

/* --------------------- Init Functions --------------------- */

void CleanUpMiniLines()
{
   CleanUpIndentStrings();
   memset(&gstDrawTextInfo, 0, sizeof(DrawTextInfo));
   gnDrawTextInfoValid = FALSE;
}

int InitMiniLines()
{
   InitIndentStrings();
   memset(&gstDrawTextInfo, 0, sizeof(DrawTextInfo));
   gnDrawTextInfoValid = FALSE;

   return TRUE;
}

/* --------------------- Free Functions --------------------- */

void FreeStrSeg(pStrSeg)
   StrSegInfo *pStrSeg;
{
   if (pStrSeg->font_name != NULL) free(pStrSeg->font_name);
   if (pStrSeg->dyn_str.s != NULL) free(pStrSeg->dyn_str.s);
   free(pStrSeg);
}

void FreeStrBlock(pStrBlock)
   StrBlockInfo *pStrBlock;
{
   switch (pStrBlock->type) {
   case SB_SIMPLE:
      FreeStrSeg(pStrBlock->seg);
      break;
   case SB_SUPSUB_LEFT:
   case SB_SUPSUB_CENTER:
   case SB_SUPSUB_RIGHT:
      if (pStrBlock->sup != NULL) FreeMiniLines(pStrBlock->sup, TRUE);
      if (pStrBlock->sub != NULL) FreeMiniLines(pStrBlock->sub, TRUE);
      if (pStrBlock->type == SB_SUPSUB_CENTER) FreeStrSeg(pStrBlock->seg);
      break;
   }
   free(pStrBlock);
}

void FreeMiniLine(pMiniLine)
   MiniLineInfo *pMiniLine;
{
   StrBlockInfo *pStrBlock=NULL, *pNextStrBlock=NULL;

   for (pStrBlock=pMiniLine->first_block; pStrBlock != NULL;
         pStrBlock=pNextStrBlock) {
      pNextStrBlock = pStrBlock->next;
      FreeStrBlock(pStrBlock);
   }
   free(pMiniLine);
}

void FreeMiniLines(minilines, free_minilines)
   MiniLinesInfo *minilines;
   int free_minilines;
{
   MiniLineInfo *pMiniLine=NULL, *pNextMiniLine=NULL;

   for (pMiniLine=minilines->first; pMiniLine != NULL;
         pMiniLine=pNextMiniLine) {
      pNextMiniLine = pMiniLine->next;
      FreeMiniLine(pMiniLine);
   }
   if (free_minilines) {
      free(minilines);
   } else {
      minilines->first = minilines->last = NULL;
   }
}

void FreeStrBlockList(pFirstStrBlock)
   StrBlockInfo *pFirstStrBlock;
{
   StrBlockInfo *pNextStrBlock=NULL;

   while (pFirstStrBlock != NULL) {
      pNextStrBlock = pFirstStrBlock->next;
      FreeStrBlock(pFirstStrBlock);
      pFirstStrBlock = pNextStrBlock;
   }
}

void FreeMiniLineList(pFirstMiniLine)
   MiniLineInfo *pFirstMiniLine;
{
   MiniLineInfo *pNextMiniLine=NULL;

   while (pFirstMiniLine != NULL) {
      pNextMiniLine = pFirstMiniLine->next;
      FreeMiniLine(pFirstMiniLine);
      pFirstMiniLine = pNextMiniLine;
   }
}

void UnlinkStrBlock(pStrBlock)
   StrBlockInfo *pStrBlock;
{
   MiniLineInfo *pOwnerMiniLine=pStrBlock->owner_mini_line;

   if (pStrBlock->prev == NULL) {
      pOwnerMiniLine->first_block = pStrBlock->next;
   } else {
      pStrBlock->prev->next = pStrBlock->next;
   }
   if (pStrBlock->next == NULL) {
      pOwnerMiniLine->last_block = pStrBlock->prev;
   } else {
      pStrBlock->next->prev = pStrBlock->prev;
   }
   pStrBlock->prev = pStrBlock->next = NULL;
}

void UnlinkMiniLine(pMiniLine)
   MiniLineInfo *pMiniLine;
{
   MiniLinesInfo *pOwnerMiniLines=pMiniLine->owner_minilines;

   if (pMiniLine->prev == NULL) {
      pOwnerMiniLines->first = pMiniLine->next;
   } else {
      pMiniLine->prev->next = pMiniLine->next;
   }
   if (pMiniLine->next == NULL) {
      pOwnerMiniLines->last = pMiniLine->prev;
   } else {
      pMiniLine->next->prev = pMiniLine->prev;
   }
   pMiniLine->prev = pMiniLine->next = NULL;
}

StrSegInfo *NewStrSeg()
{
   StrSegInfo *pStrSeg=(StrSegInfo*)malloc(sizeof(StrSegInfo));

   if (pStrSeg == NULL) FailAllocMessage();
   memset(pStrSeg, 0, sizeof(StrSegInfo));
   DynStrSet(&pStrSeg->dyn_str, "");
   return pStrSeg;
}

StrBlockInfo *NewStrBlock()
{
   StrBlockInfo *pStrBlock=(StrBlockInfo*)malloc(sizeof(StrBlockInfo));

   if (pStrBlock == NULL) FailAllocMessage();
   memset(pStrBlock, 0, sizeof(StrBlockInfo));
   return pStrBlock;
}

MiniLineInfo *NewMiniLine()
{
   MiniLineInfo *pMiniLine=(MiniLineInfo*)malloc(sizeof(MiniLineInfo));

   if (pMiniLine == NULL) FailAllocMessage();
   memset(pMiniLine, 0, sizeof(MiniLineInfo));
   return pMiniLine;
}

MiniLinesInfo *NewMiniLines()
{
   MiniLinesInfo *minilines=(MiniLinesInfo*)malloc(sizeof(MiniLinesInfo));

   if (minilines == NULL) FailAllocMessage();
   memset(minilines, 0, sizeof(MiniLinesInfo));
   return minilines;
}

void InsertStrBlock(pPrevStrBlock, pNextStrBlock, pStrBlock,
      ppFirstStrBlock, ppLastStrBlock)
   StrBlockInfo *pPrevStrBlock, *pNextStrBlock, *pStrBlock;
   StrBlockInfo **ppFirstStrBlock, **ppLastStrBlock;
{
   pStrBlock->prev = pPrevStrBlock;
   pStrBlock->next = pNextStrBlock;

   if (pPrevStrBlock == NULL) {
      (*ppFirstStrBlock) = pStrBlock;
   } else {
      pPrevStrBlock->next = pStrBlock;
   }
   if (pNextStrBlock == NULL) {
      (*ppLastStrBlock) = pStrBlock;
   } else {
      pNextStrBlock->prev = pStrBlock;
   }
}

void InsertMiniLine(pPrevMiniLine, pNextMiniLine, pMiniLine,
      ppFirstMiniLine, ppLastMiniLine)
   MiniLineInfo *pPrevMiniLine, *pNextMiniLine, *pMiniLine;
   MiniLineInfo **ppFirstMiniLine, **ppLastMiniLine;
{
   pMiniLine->prev = pPrevMiniLine;
   pMiniLine->next = pNextMiniLine;

   if (pPrevMiniLine == NULL) {
      (*ppFirstMiniLine) = pMiniLine;
   } else {
      pPrevMiniLine->next = pMiniLine;
   }
   if (pNextMiniLine == NULL) {
      (*ppLastMiniLine) = pMiniLine;
   } else {
      pNextMiniLine->prev = pMiniLine;
   }
}

int BlankStrSeg(pStrSeg)
   StrSegInfo *pStrSeg;
{
   return (*pStrSeg->dyn_str.s == '\0');
}

int BlankStrBlock(pStrBlock)
   StrBlockInfo *pStrBlock;
{
   switch (pStrBlock->type) {
   case SB_SIMPLE: return BlankStrSeg(pStrBlock->seg);

   case SB_CHAR_SPACE: return (pStrBlock->special_char_w == 0);

   case SB_SUPSUB_LEFT:
   case SB_SUPSUB_CENTER:
   case SB_SUPSUB_RIGHT:
      if (pStrBlock->sup != NULL) {
         if (!BlankMiniLines(pStrBlock->sup)) {
            return FALSE;
         }
      }
      if (pStrBlock->sub != NULL) {
         if (!BlankMiniLines(pStrBlock->sub)) {
            return FALSE;
         }
      }
      if (pStrBlock->type == SB_SUPSUB_CENTER) {
         return BlankStrSeg(pStrBlock->seg);
      }
      break;
   }
   return TRUE;
}

int BlankMiniLine(pMiniLine)
   MiniLineInfo *pMiniLine;
{
   StrBlockInfo *pStrBlock=NULL;

   for (pStrBlock=pMiniLine->first_block; pStrBlock != NULL;
         pStrBlock=pStrBlock->next) {
      if (!BlankStrBlock(pStrBlock)) {
         return FALSE;
      }
   }
   return TRUE;
}

int BlankMiniLines(minilines)
   MiniLinesInfo *minilines;
{
   MiniLineInfo *pMiniLine=NULL;

   for (pMiniLine=minilines->first; pMiniLine != NULL;
         pMiniLine=pMiniLine->next) {
      if (!BlankMiniLine(pMiniLine)) {
         return FALSE;
      }
   }
   return TRUE;
}

StrSegInfo *GetTextFirstStrSeg(obj_ptr)
   struct ObjRec *obj_ptr;
{
   StrBlockInfo *pStrBlock=obj_ptr->detail.t->minilines.first->first_block;

   for ( ; pStrBlock != NULL; pStrBlock=pStrBlock->next) {
      if (pStrBlock->type == SB_SIMPLE) {
         return (pStrBlock->seg);
      }
   }
   return NULL;
}

struct DynStrRec *GetTextFirstDynStr(obj_ptr)
   struct ObjRec *obj_ptr;
{
   StrBlockInfo *pStrBlock=obj_ptr->detail.t->minilines.first->first_block;

   for ( ; pStrBlock != NULL; pStrBlock=pStrBlock->next) {
      if (pStrBlock->type == SB_SIMPLE) {
         return (&pStrBlock->seg->dyn_str);
      }
   }
   return NULL;
}

/* ===================== TextRec Routines ===================== */

void CopyCurInfoIntoStrSeg(pStrBlock, pStrSeg)
   StrBlockInfo *pStrBlock;
   StrSegInfo *pStrSeg;
{
   pStrSeg->color = colorIndex;
   UtilStrCpyN(pStrSeg->color_str, sizeof(pStrSeg->color_str),
         colorMenuItems[colorIndex]);
   pStrSeg->font = curFont;
   pStrSeg->style = curStyle;
   pStrSeg->sz_unit = curSzUnit;
   pStrSeg->double_byte = canvasFontDoubleByte;
   pStrSeg->double_byte_mod_bytes = canvasFontDoubleByteModBytes;
   pStrSeg->double_byte_vertical = canvasFontDoubleByteVertical;
   pStrSeg->direction = canvasFontDirection;
   pStrSeg->dontreencode = canvasFontDontReencode;
   pStrSeg->underline_on = curUnderlineOn;
   pStrSeg->overline_on = curOverlineOn;
   pStrSeg->asc = canvasFontAsc;
   pStrSeg->des = canvasFontDes;
   /*
    * The following are set in RecalcTextMetrics():
    *       w, min_lbearing, max_rextra
    */
   /*
    * If read_only is TRUE, the following are set in ReadTextObj():
    *       read_only, orig_w, orig_h, orig_asc, orig_des
    */
   /*
    * These fields are not touched: font_name, dyn_str, owner
    */
}

void CopyCurInfoFromStrSeg(pStrSeg)
   StrSegInfo *pStrSeg;
{
   colorIndex = pStrSeg->color;
   curFont = pStrSeg->font;
   curStyle = pStrSeg->style;
   curSzUnit = pStrSeg->sz_unit;
   if (PRTGIF) {
      canvasFontDoubleByte = pStrSeg->double_byte;
      canvasFontDoubleByteModBytes = pStrSeg->double_byte_mod_bytes;
      canvasFontDoubleByteVertical = pStrSeg->double_byte_vertical;
      canvasFontDirection = pStrSeg->direction;
      canvasFontDontReencode = pStrSeg->dontreencode;
      canvasFontAsc = pStrSeg->asc;
      canvasFontDes = pStrSeg->des;
   } else {
      SetCanvasFont();
   }
   curUnderlineOn = pStrSeg->underline_on;
   curOverlineOn = pStrSeg->overline_on;
}

void CopyCurInfoIntoTextPtr(obj_ptr, text_ptr)
   struct ObjRec *obj_ptr;
   struct TextRec *text_ptr;
{
   obj_ptr->trans_pat = transPat;

   text_ptr->minilines.just = textJust;
   text_ptr->minilines.v_space = textVSpace;
   text_ptr->pen = penPat;
   text_ptr->fill = objFill;
   text_ptr->minilines.first->asc = canvasLineAsc;
   text_ptr->minilines.first->des = canvasLineDes;
   text_ptr->underline_y_offset = curUnderlineYOffset;
   text_ptr->overline_y_offset = curOverlineYOffset;

   /*
    * The following are set in RecalcTextMetrics():
    *       w, h, min_lbearing, max_rextra
    */
   text_ptr->w = 0;
   text_ptr->h = 0;
   text_ptr->min_lbearing = 0;
   text_ptr->max_rextra = 0;

   /*
    * If read_only is TRUE, the following are set in ReadTextObj():
    *       read_only, orig_w, orig_h, orig_asc, orig_des
    */
   text_ptr->read_only = FALSE;
   text_ptr->orig_w = text_ptr->orig_h = 0;

   /*
    * The following fields are not touched: cached_ctm, orig_bbox, baseline_y,
    *       cached_bitmap, cached_zoom, cached_zoomed.
    */
}

void CopyCurInfoFromTextPtr(obj_ptr, text_ptr)
   struct ObjRec *obj_ptr;
   struct TextRec *text_ptr;
{
   transPat = obj_ptr->trans_pat;

   textJust = text_ptr->minilines.just;
   textVSpace = text_ptr->minilines.v_space;
   penPat = text_ptr->pen;
   objFill = text_ptr->fill;
   canvasLineAsc = text_ptr->minilines.first->asc;
   canvasLineDes = text_ptr->minilines.first->des;
   curUnderlineYOffset = text_ptr->underline_y_offset;
   curOverlineYOffset = text_ptr->overline_y_offset;
}

/* ===================== RecalcTextMetrics() ===================== */

static
int RecalcStrSegMetrics(pStrSeg, prmi)
   StrSegInfo *pStrSeg;
   RecalcMetricsInfo *prmi;
   /*
    * This function recomputes text metrics of the string segments pStrSeg.
    * Returns FALSE if the block is read_only.
    */
{
   int read_only=FALSE;
   XCharStruct xcs;

   curFont = pStrSeg->font;
   curStyle = pStrSeg->style;
   curSzUnit = pStrSeg->sz_unit;

   if (mainDisplay == NULL) return FALSE;

   SetCanvasFont();
   if (SzUnitToFontSize(curSzUnit) != canvasFontSize) {
#ifdef _TGIF_DBG /* debug, do not translate */
      fprintf(stderr, "RecalcStrSegMetrics(): read_only detected.\n");
#endif /* _TGIF_DBG */
      read_only = TRUE;
   } else {
      MyTextExtents(canvasFontPtr, pStrSeg->dyn_str.s, pStrSeg->dyn_str.sz-1,
            &xcs);

      pStrSeg->w = xcs.width;
      pStrSeg->min_lbearing = (xcs.lbearing >= 0 ? 0 : xcs.lbearing);
      pStrSeg->max_rextra = xcs.rbearing-xcs.width;
      pStrSeg->asc = canvasFontAsc;
      pStrSeg->des = canvasFontDes;
   }
   if (read_only && readingTextObject) {
      pStrSeg->read_only = TRUE;
   }
   return (!read_only);
}

static
void UpdateMetricsFromValues(p_found, w, asc, des, min_lbearing, max_rextra,
         pTextExtents)
   int *p_found, w, asc, des, min_lbearing, max_rextra;
   TextExtentsInfo *pTextExtents;
{
   if (p_found == NULL || !(*p_found)) {
      pTextExtents->w = w;
      pTextExtents->lbearing = min_lbearing;
      pTextExtents->rextra = max_rextra;
      pTextExtents->asc = asc;
      pTextExtents->des = des;
      if (p_found != NULL) *p_found = TRUE;
   } else {
      if (w > (pTextExtents->w)) pTextExtents->w = w;
      if (min_lbearing > (pTextExtents->lbearing)) {
         pTextExtents->lbearing = min_lbearing;
      }
      if (max_rextra > (pTextExtents->rextra)) {
         pTextExtents->rextra = max_rextra;
      }
      if (asc > pTextExtents->asc) {
         pTextExtents->asc = asc;
      }
      if (des > pTextExtents->des) {
         pTextExtents->des = des;
      }
   }
   pTextExtents->bbox_w = pTextExtents->w - pTextExtents->lbearing +
         pTextExtents->rextra;
   pTextExtents->bbox_h = pTextExtents->asc + pTextExtents->des;
}

static
void UpdateMetricsFromStrSegInfo(p_found, pStrSeg, pTextExtents)
   int *p_found;
   StrSegInfo *pStrSeg;
   TextExtentsInfo *pTextExtents;
{
   UpdateMetricsFromValues(p_found, pStrSeg->w, pStrSeg->asc, pStrSeg->des,
         pStrSeg->min_lbearing, pStrSeg->max_rextra, pTextExtents);
}

static
int StrBlockTypeToMiniLinesJust(type)
   int type;
{
   switch (type) {
   case SB_SUPSUB_LEFT: return JUST_R;
   case SB_SUPSUB_CENTER: return JUST_C;
   case SB_SUPSUB_RIGHT: return JUST_L;
   }
   return JUST_L;
}

static
int RecalcStrBlockMetrics(pStrBlock, prmi)
   StrBlockInfo *pStrBlock;
   RecalcMetricsInfo *prmi;
   /*
    * This function recursively recomputes all text metrics of the
    *       string segments and mini-lines in pStrBlock.
    * Returns FALSE if the block is read_only.
    */
{
   int found=FALSE, saved_baseline_y=prmi->baseline_y;
   int read_only=FALSE;
   TextExtentsInfo stTextExtents;

   pStrBlock->depth = prmi->depth;
   pStrBlock->pre_order = (prmi->pre_order)++;

   memset(&stTextExtents, 0, sizeof(TextExtentsInfo));
   switch (pStrBlock->type) {
   case SB_SIMPLE:
      /*
       * For a simple block, the block only contains a string segment.
       * Other than the vertical offset, the metrics between the string
       *       segment and this block are identical.
       */
      pStrBlock->seg->owner = pStrBlock;
      if (!dontRecalcStrSegMetrics) {
         if (!RecalcStrSegMetrics(pStrBlock->seg, prmi)) {
            read_only = TRUE;
         }
      }
      UpdateMetricsFromStrSegInfo(NULL, pStrBlock->seg, &stTextExtents);
      break;
   /*
    * Special characters are very much like simple blocks, except for
    *       the differences noted below.
    */
   case SB_CHAR_SPACE:
      /*
       * The space character is not the same as (char*)0x20.  It's used for
       *       ``extra space'' or ``negative space''.  The width of it is
       *       in special_char_w.  All other metrics are 0.  Since it
       *       doesn't have an ascent, its vertical offset from the parent
       *       block is meaningless.
       */
      stTextExtents.w = pStrBlock->special_char_w;
      stTextExtents.lbearing = stTextExtents.rextra = 0;
      stTextExtents.asc = stTextExtents.des = 0;
      break;
   case SB_SUPSUB_LEFT:
   case SB_SUPSUB_CENTER:
   case SB_SUPSUB_RIGHT:
      prmi->depth++;
      if (pStrBlock->type == SB_SUPSUB_CENTER) {
         /* pStrBlock->seg better not be NULL or it will crash */
         pStrBlock->depth = prmi->depth;
         pStrBlock->seg->owner = pStrBlock;
         if (!dontRecalcStrSegMetrics) {
            if (!RecalcStrSegMetrics(pStrBlock->seg, prmi)) {
               read_only = TRUE;
            }
         }
         UpdateMetricsFromStrSegInfo(&found, pStrBlock->seg, &stTextExtents);
      }
      if (pStrBlock->sup != NULL) {
         MiniLinesInfo *minilines=pStrBlock->sup;
         int asc=0, des=0;

         pStrBlock->sup->owner_block = pStrBlock;

         prmi->baseline_y = saved_baseline_y+minilines->baseline_offset;
         if (pStrBlock->type == SB_SUPSUB_CENTER) {
            /*
             * Note, by default, for SB_SUPSUB_CENTER,
             *       minilines.baseline_offset is the descent of the
             *       superscript font.  Adding the ascent of the font for
             *       the string segment below makes the superscript sits
             *       right on top of the string segment.
             * Doing it this way, when you change the font of the string
             *       segment, the relative position of the superscript and
             *       the string segment doesn't change.
             */
            prmi->baseline_y -= pStrBlock->seg->asc;
         }
         if (!RecalcMiniLinesMetrics(minilines, prmi)) {
            read_only = TRUE;
         }
         if (pStrBlock->type == SB_SUPSUB_CENTER) {
            asc = (-minilines->baseline_offset) + minilines->first->asc +
                  pStrBlock->seg->asc;
            des = minilines->h - asc;
         } else {
            asc = (-minilines->baseline_offset) + minilines->first->asc;
            des = minilines->h - asc;
         }
         UpdateMetricsFromValues(&found, minilines->w, asc, des,
               minilines->min_lbearing, minilines->max_rextra, &stTextExtents);

         minilines->just = StrBlockTypeToMiniLinesJust(pStrBlock->type);
      }
      if (pStrBlock->sub != NULL) {
         MiniLinesInfo *minilines=pStrBlock->sub;
         int asc=0, des=0;

         pStrBlock->sub->owner_block = pStrBlock;

         prmi->baseline_y = saved_baseline_y+pStrBlock->sub->baseline_offset;
         if (pStrBlock->type == SB_SUPSUB_CENTER) {
            /*
             * Note, by default, for SB_SUPSUB_CENTER,
             *       pStrBlock->sub->baseline_offset is the ascent of the
             *       subscript font.  Adding the descent of the font for
             *       the string segment below makes the subscript sits
             *       right below the string segment.
             * Doing it this way, when you change the font of the string
             *       segment, the relative position of the subscript and
             *       the string segment doesn't change.
             */
            prmi->baseline_y += pStrBlock->seg->des;
         }
         if (!RecalcMiniLinesMetrics(minilines, prmi)) {
            read_only = TRUE;
         }
         if (pStrBlock->type == SB_SUPSUB_CENTER) {
            asc = minilines->first->asc - minilines->baseline_offset -
                  pStrBlock->seg->des;
            des = minilines->h - asc;
         } else {
            asc = minilines->first->asc - minilines->baseline_offset;
            des = minilines->h - asc;
         }
         UpdateMetricsFromValues(&found, minilines->w, asc, des,
               minilines->min_lbearing, minilines->max_rextra, &stTextExtents);

         minilines->just = StrBlockTypeToMiniLinesJust(pStrBlock->type);
      }
      prmi->depth--;
      break;
   }
   pStrBlock->w = stTextExtents.w;
   pStrBlock->min_lbearing = stTextExtents.lbearing;
   pStrBlock->max_rextra = stTextExtents.rextra;
   pStrBlock->asc = stTextExtents.asc;
   pStrBlock->des = stTextExtents.des;

   prmi->baseline_y = saved_baseline_y;
   if (read_only && readingTextObject) {
      pStrBlock->read_only = TRUE;
   }
   return (!read_only);
}

int RecalcMiniLineMetrics(pMiniLine, prmi)
   MiniLineInfo *pMiniLine;
   RecalcMetricsInfo *prmi;
   /*
    * This function recursively recomputes all text metrics of the
    *       blocks in pMiniLine.
    * Returns FALSE if the text is read_only.
    */
{
   StrBlockInfo *pStrBlock=NULL;
   int min_lbearing=0, max_rextra=0, max_asc=0, max_des=0;
   int saved_baseline_y=prmi->baseline_y;
   int saved_x=prmi->x, read_only=FALSE;

   for (pStrBlock=pMiniLine->first_block; pStrBlock != NULL;
         pStrBlock=pStrBlock->next) {
      pStrBlock->owner_mini_line = pMiniLine;

      if (!RecalcStrBlockMetrics(pStrBlock, prmi)) {
         read_only = FALSE;
      }
      if (pStrBlock->asc > max_asc) max_asc = pStrBlock->asc;
      if (pStrBlock->des > max_des) max_des = pStrBlock->des;
      if (pStrBlock->min_lbearing < min_lbearing) {
         min_lbearing = pStrBlock->min_lbearing;
      }
      if (pStrBlock->max_rextra > max_rextra) {
         max_rextra = pStrBlock->max_rextra;
      }
      prmi->x += pStrBlock->w;
   }
   pMiniLine->w = prmi->x - saved_x;
   pMiniLine->min_lbearing = min_lbearing;
   pMiniLine->max_rextra = max_rextra;
   pMiniLine->asc = max_asc;
   pMiniLine->des = max_des;

   prmi->x = saved_x;
   prmi->baseline_y = saved_baseline_y;

   if (read_only && readingTextObject) {
      pMiniLine->read_only = TRUE;
   }
   return (!read_only);
}

int RecalcMiniLinesMetrics(minilines, prmi)
   MiniLinesInfo *minilines;
   RecalcMetricsInfo *prmi;
   /*
    * This function recursively recomputes all text metrics of the minilines.
    * Returns FALSE if the text is read_only.
    */
{
   MiniLineInfo *pMiniLine=NULL;
   int v_space=minilines->v_space;
   int saved_x=prmi->x, saved_baseline_y=prmi->baseline_y;
   int min_y=prmi->baseline_y, max_y=prmi->baseline_y;
   int max_w=0, min_lbearing=0, max_rextra=0;
   int read_only=FALSE, saved_depth=prmi->depth;

   for (pMiniLine=minilines->first; pMiniLine != NULL;
         pMiniLine=pMiniLine->next) {
      pMiniLine->owner_minilines = minilines;

#ifdef _TGIF_DBG /* debug, do not translate */
      if (pMiniLine == minilines->first) {
         TgAssert(pMiniLine->v_gap == 0,
               "First mini_line has non-zero v_gap in RecalcMiniLinesMetrics()",
               NULL);
      }
#endif /* _TGIF_DBG */
      prmi->baseline_y += pMiniLine->v_gap;
      if (!RecalcMiniLineMetrics(pMiniLine, prmi)) {
         read_only = TRUE;
      }
      if (pMiniLine != minilines->first) {
         prmi->baseline_y += pMiniLine->asc;
      }
      if (prmi->baseline_y-pMiniLine->asc < min_y) {
         min_y = prmi->baseline_y-pMiniLine->asc;
      }
      if (prmi->baseline_y+pMiniLine->des > max_y) {
         max_y = prmi->baseline_y+pMiniLine->des;
      }
      if (pMiniLine->min_lbearing < min_lbearing) {
         min_lbearing = pMiniLine->min_lbearing;
      }
      if (pMiniLine->max_rextra > max_rextra) {
         max_rextra = pMiniLine->max_rextra;
      }
      if (pMiniLine->w > max_w) max_w = pMiniLine->w;

      prmi->baseline_y += pMiniLine->des + v_space;
   }
   minilines->w = max_w;
   minilines->h = max_y-min_y;
   if (minilines->h < 0) minilines->h = min_y-max_y;
   minilines->min_lbearing = min_lbearing;
   minilines->max_rextra = max_rextra;

   prmi->x = saved_x;
   prmi->baseline_y = saved_baseline_y;
   prmi->depth = saved_depth;

   if (read_only && readingTextObject) {
      minilines->read_only = TRUE;
   }
   return (!read_only);
}

int RecalcTextMetrics(text_ptr, x, orig_baseline_y)
   struct TextRec *text_ptr;
   int x, orig_baseline_y;
   /*
    * Returns FALSE if the text is read_only.
    */
{
   MiniLinesInfo *minilines=(&text_ptr->minilines);
   int read_only=FALSE;
   RecalcMetricsInfo rmi;

   if (text_ptr->read_only) return FALSE;

   memset(&rmi, 0, sizeof(RecalcMetricsInfo));
   rmi.x = rmi.orig_x = x;
   rmi.baseline_y = rmi.orig_baseline_y = orig_baseline_y;

   PushCurFont();
   if (!RecalcMiniLinesMetrics(minilines, &rmi)) {
      read_only = TRUE;
   }
   PopCurFont();

   text_ptr->w = minilines->w;
   text_ptr->h = minilines->h;
   text_ptr->min_lbearing = minilines->min_lbearing;
   text_ptr->max_rextra = minilines->max_rextra;
   text_ptr->lines = GetNumberOfMiniLines(minilines);

   if (read_only && readingTextObject) {
      text_ptr->read_only = TRUE;
   }
   return (!read_only);
}

/* --------------------- UpdateCurTextBBoxes() --------------------- */

static
void UpdateStrBlockBBoxes(pStrBlock, x, baseline_y, clean)
   StrBlockInfo *pStrBlock;
   int x, baseline_y, clean;
{
   int saved_baseline_y=baseline_y;
   int block_w=pStrBlock->w;

   SetBBRec((clean ? &pStrBlock->clean_bbox : &pStrBlock->bbox),
         x+textAbsMinLBearing, baseline_y-pStrBlock->asc,
         x+pStrBlock->w+textAbsMaxRExtra, baseline_y+pStrBlock->des);
   switch (pStrBlock->type) {
   case SB_SIMPLE: break;
   case SB_CHAR_SPACE: break;

   case SB_SUPSUB_LEFT:
   case SB_SUPSUB_CENTER:
   case SB_SUPSUB_RIGHT:
      switch (pStrBlock->type) {
      case SB_SUPSUB_LEFT: x += block_w; break;
      case SB_SUPSUB_CENTER: x += (block_w>>1); break;
      case SB_SUPSUB_RIGHT: break;
      }
      if (pStrBlock->sup != NULL) {
         baseline_y = saved_baseline_y + pStrBlock->sup->baseline_offset;
         if (pStrBlock->type == SB_SUPSUB_CENTER) {
            baseline_y -= pStrBlock->seg->asc;
         }
         UpdateMiniLinesBBoxes(pStrBlock->sup, x, baseline_y, clean);
      }
      if (pStrBlock->sub != NULL) {
         baseline_y = saved_baseline_y + pStrBlock->sub->baseline_offset;
         if (pStrBlock->type == SB_SUPSUB_CENTER) {
            baseline_y += pStrBlock->seg->des;
         }
         UpdateMiniLinesBBoxes(pStrBlock->sub, x, baseline_y, clean);
      }
      break;
   }
   if (clean) {
      pStrBlock->clean = TRUE;
      memcpy(&pStrBlock->bbox, &pStrBlock->clean_bbox, sizeof(struct BBRec));
   } else if (!pStrBlock->clean ||
         CompareRect(&pStrBlock->clean_bbox, &pStrBlock->bbox) != 0) {
      pStrBlock->clean = FALSE;
      AddToDirtyBBox(&pStrBlock->clean_bbox);
      AddToDirtyBBox(&pStrBlock->bbox);
   }
}

static
void UpdateMiniLineBBoxes(pMiniLine, x, baseline_y, clean)
   MiniLineInfo *pMiniLine;
   int x, baseline_y, clean;
{
   int saved_x=x;
   StrBlockInfo *pStrBlock=NULL;

   for (pStrBlock=pMiniLine->first_block; pStrBlock != NULL;
         pStrBlock=pStrBlock->next) {
      UpdateStrBlockBBoxes(pStrBlock, x, baseline_y, clean);

      x += pStrBlock->w;
   }
   SetBBRec((clean ? &pMiniLine->clean_bbox : &pMiniLine->bbox),
         saved_x+textAbsMinLBearing, baseline_y-pMiniLine->asc,
         saved_x+pMiniLine->w+textAbsMaxRExtra, baseline_y+pMiniLine->des);
   if (clean) {
      pMiniLine->clean = TRUE;
      memcpy(&pMiniLine->bbox, &pMiniLine->clean_bbox, sizeof(struct BBRec));
   } else if (!pMiniLine->clean ||
         CompareRect(&pMiniLine->clean_bbox, &pMiniLine->bbox) != 0) {
      pMiniLine->clean = FALSE;
      AddToDirtyBBox(&pMiniLine->clean_bbox);
      AddToDirtyBBox(&pMiniLine->bbox);
   }
}

void UpdateMiniLinesBBoxes(minilines, x, baseline_y, clean)
   MiniLinesInfo *minilines;
   int x, baseline_y, clean;
{
   MiniLineInfo *pMiniLine=NULL;
   int v_space=minilines->v_space;
   int saved_x=x, saved_baseline_y=baseline_y;

   for (pMiniLine=minilines->first; pMiniLine != NULL;
         pMiniLine=pMiniLine->next) {
      baseline_y += pMiniLine->v_gap;

      switch (minilines->just) {
      case JUST_L: x = saved_x; break;
      case JUST_C: x = saved_x - ((pMiniLine->w)>>1); break;
      case JUST_R: x = saved_x - pMiniLine->w; break;
      }
      UpdateMiniLineBBoxes(pMiniLine, x, baseline_y, clean);

      baseline_y += pMiniLine->des + v_space;
      if (pMiniLine->next != NULL) {
         baseline_y += pMiniLine->next->asc;
      }
   }
   switch (minilines->just) {
   case JUST_L: x = saved_x; break;
   case JUST_C: x = saved_x - ((minilines->w)>>1); break;
   case JUST_R: x = saved_x - minilines->w; break;
   }
   SetBBRec((clean ? &minilines->clean_bbox : &minilines->bbox),
         x+textAbsMinLBearing, saved_baseline_y-minilines->first->asc,
         x+minilines->w+textAbsMaxRExtra,
         saved_baseline_y-minilines->first->asc+minilines->h);
   if (clean) {
      minilines->clean = TRUE;
      memcpy(&minilines->bbox, &minilines->clean_bbox, sizeof(struct BBRec));
   } else if (!minilines->clean ||
         CompareRect(&minilines->clean_bbox, &minilines->bbox) != 0) {
      minilines->clean = FALSE;
      AddToDirtyBBox(&minilines->clean_bbox);
      AddToDirtyBBox(&minilines->bbox);
   }
}

void UpdateCurTextBBoxes(clean)
   int clean;
{
   struct TextRec *text_ptr=curTextObj->detail.t;

   if (text_ptr->read_only) return;

   UpdateMiniLinesBBoxes(&text_ptr->minilines, textOrigX, textOrigBaselineY,
         clean);
}

/* --------------------- DoFuncOnStrSegForMiniLines() --------------------- */

static
void DoFuncOnStrSegForStrSeg(pStrSeg, pFunc, pUserData)
   StrSegInfo *pStrSeg;
   STRSEGFN *pFunc;
   void *pUserData;
{
   (pFunc)(pStrSeg, pUserData);
}

static
void DoFuncOnStrSegForStrBlock(pStrBlock, pFunc, pUserData)
   StrBlockInfo *pStrBlock;
   STRSEGFN *pFunc;
   void *pUserData;
{
   switch (pStrBlock->type) {
   case SB_SIMPLE:
      DoFuncOnStrSegForStrSeg(pStrBlock->seg, pFunc, pUserData);
      break;

   case SB_CHAR_SPACE: break;

   case SB_SUPSUB_LEFT:
   case SB_SUPSUB_CENTER:
   case SB_SUPSUB_RIGHT:
      if (pStrBlock->sup != NULL) {
         DoFuncOnStrSegForMiniLines(pStrBlock->sup, pFunc, pUserData);
      }
      if (pStrBlock->sub != NULL) {
         DoFuncOnStrSegForMiniLines(pStrBlock->sub, pFunc, pUserData);
      }
      if (pStrBlock->type == SB_SUPSUB_CENTER) {
         DoFuncOnStrSegForStrSeg(pStrBlock->seg, pFunc, pUserData);
      }
      break;
   }
}

static
void DoFuncOnStrSegForMiniLine(pMiniLine, pFunc, pUserData)
   MiniLineInfo *pMiniLine;
   STRSEGFN *pFunc;
   void *pUserData;
{
   StrBlockInfo *pStrBlock=NULL;

   for (pStrBlock=pMiniLine->first_block; pStrBlock != NULL;
         pStrBlock=pStrBlock->next) {
      DoFuncOnStrSegForStrBlock(pStrBlock, pFunc, pUserData);
   }
}

void DoFuncOnStrSegForMiniLines(minilines, pFunc, pUserData)
   MiniLinesInfo *minilines;
   STRSEGFN *pFunc;
   void *pUserData;
{
   MiniLineInfo *pMiniLine=NULL;

   for (pMiniLine=minilines->first; pMiniLine != NULL;
         pMiniLine=pMiniLine->next) {
      DoFuncOnStrSegForMiniLine(pMiniLine, pFunc, pUserData);
   }
}

/* ===================== Text Routines ===================== */

/* --------------------- ResetOnCursorKey() --------------------- */

void ResetOnCursorKey(drag)
   int drag;
{
   escPressed = FALSE;
   if (!drag) {
      endStrBlock = NULL;
      textEndIndex = INVALID;
      textHighlight = FALSE;
   }
}

/* --------------------- PutTextCursor() --------------------- */

void PutTextCursor()
{
   int asc=curStrBlock->seg->asc;
   int des=curStrBlock->seg->des;

   if (!textHighlight) {
      if (drawWinHasFocus) {
         XDrawLine(mainDisplay, drawWindow, revDefaultGC, textCurX,
               textCurBaselineY-asc, textCurX, textCurBaselineY+des);
      }
   }
   if (canvasFontDoubleByte && gnInputMethod != TGIM_NONE && gnOverTheSpot) {
      if (!tgIMTellCursorPosition(mainDisplay, drawWindow, textCurX,
            textCurBaselineY+des)) {
      }
   }
   SetRightMarginActive();
}

/* --------------------- EraseTextCursor() --------------------- */

void EraseTextCursor()
{
   int asc=curStrBlock->seg->asc;
   int des=curStrBlock->seg->des;

   XDrawLine(mainDisplay, drawWindow, revDefaultGC, textCurX,
         textCurBaselineY-asc, textCurX, textCurBaselineY+des);
}

/* --------------------- GetCurTextBBoxes() --------------------- */

void GetCurTextBBoxes(pOBBox, pBBox)
   struct BBRec *pOBBox, *pBBox;
   /*
    * pOBBox bounds the inner curTextObj
    * pBBox bounds the outer curTextObj and textW and textH
    */
{
   struct TextRec *text_ptr=curTextObj->detail.t;
   MiniLinesInfo *minilines=(&text_ptr->minilines);
   int ltx=0, lty=0, w=0, h=0;

   if (pOBBox != NULL) {
      ltx = textOrigX;
      lty = textOrigBaselineY-minilines->first->asc;
      w = minilines->w;
      h = minilines->h;
      switch (minilines->just) {
      case JUST_L: break;
      case JUST_C: ltx -= (w>>1); break;
      case JUST_R: ltx -= w; break;
      }
      SetBBRec(pOBBox, ltx, lty, ltx+w, lty+h);
   }
   if (pBBox != NULL) {
      ltx = textOrigX;
      lty = textOrigY;
      w = textW;
      h = textH;
      switch (minilines->just) {
      case JUST_L: break;
      case JUST_C: ltx -= (w>>1); break;
      case JUST_R: ltx -= w; break;
      }
      SetBBRec(pBBox, ltx+textAbsMinLBearing, lty,
            ltx+w+textAbsMaxRExtra, lty+h);
   }
}

/* --------------------- RedrawCurText() --------------------- */

static
int UpdateTextHighlightInfo()
{
   int min_y_1=0, max_y_1=0, min_y_2=0, max_y_2=0;
   int reverse=FALSE;

   if (curStrBlock->depth != endStrBlock->depth) {
#ifdef _TGIF_DBG /* debug, do not translate */
      fprintf(stderr, "curStrBlock->depth = %1d, endStrBlock->depth = %1d\n",
            curStrBlock->depth, endStrBlock->depth);
#endif /* _TGIF_DBG */
      sprintf(gszMsgBox, TgLoadString(STID_UNEQUAL_DEPTHS_IN_FUNC),
            "UpdateTextHighlightInfo()");
      FatalUnexpectedError(gszMsgBox, NULL);
      return FALSE;
   }
   if (curStrBlock->type == SB_SUPSUB_CENTER) {
      min_y_1 = textCurBaselineY - curStrBlock->seg->asc;
      max_y_1 = textCurBaselineY + curStrBlock->seg->des;
   } else {
      min_y_1 = textCurBaselineY - curStrBlock->owner_mini_line->asc;
      max_y_1 = textCurBaselineY + curStrBlock->owner_mini_line->des;
   }
   if (endStrBlock->type == SB_SUPSUB_CENTER) {
      min_y_2 = textEndBaselineY - endStrBlock->seg->asc;
      max_y_2 = textEndBaselineY + endStrBlock->seg->des;
   } else {
      min_y_2 = textEndBaselineY - endStrBlock->owner_mini_line->asc;
      max_y_2 = textEndBaselineY + endStrBlock->owner_mini_line->des;
   }

   if (curStrBlock->pre_order == endStrBlock->pre_order) {
      if (textCurX < textEndX) {
         reverse = FALSE;
      } else if (textCurX > textEndX) {
         reverse = TRUE;
      } else {
         textHighlight = FALSE;
         endStrBlock = NULL;
         textEndIndex = INVALID;
         return FALSE;
      }
   } else if (curStrBlock->pre_order < endStrBlock->pre_order) {
      reverse = FALSE;
   } else {
      reverse = TRUE;
   }
   if (reverse) {
      gstTextHighlightInfo.start_x = textEndX;
      gstTextHighlightInfo.start_min_y = min_y_2;
      gstTextHighlightInfo.start_max_y = max_y_2;
      gstTextHighlightInfo.start_baseline_y = textEndBaselineY;

      gstTextHighlightInfo.end_x = textCurX;
      gstTextHighlightInfo.end_min_y = min_y_1;
      gstTextHighlightInfo.end_max_y = max_y_1;
      gstTextHighlightInfo.end_baseline_y = textCurBaselineY;

      gstTextHighlightInfo.start_str_block_ptr = endStrBlock;
      gstTextHighlightInfo.start_index = textEndIndex;
      gstTextHighlightInfo.end_str_block_ptr = curStrBlock;
      gstTextHighlightInfo.end_index = textCurIndex;
   } else {
      gstTextHighlightInfo.start_x = textCurX;
      gstTextHighlightInfo.start_min_y = min_y_1;
      gstTextHighlightInfo.start_max_y = max_y_1;
      gstTextHighlightInfo.start_baseline_y = textCurBaselineY;

      gstTextHighlightInfo.end_x = textEndX;
      gstTextHighlightInfo.end_min_y = min_y_2;
      gstTextHighlightInfo.end_max_y = max_y_2;
      gstTextHighlightInfo.end_baseline_y = textEndBaselineY;

      gstTextHighlightInfo.start_str_block_ptr = curStrBlock;
      gstTextHighlightInfo.start_index = textCurIndex;
      gstTextHighlightInfo.end_str_block_ptr = endStrBlock;
      gstTextHighlightInfo.end_index = textEndIndex;
   }
   return TRUE;
}

static
void GetMinilineHomeXY(pMiniLine, pn_x, pn_baseline_y)
   MiniLineInfo *pMiniLine;
   int *pn_x, *pn_baseline_y;
{
#ifdef _TGIF_DBG /* debug, do not translate */
   TgAssert(pMiniLine->first_block->type == SB_SIMPLE ||
         pMiniLine->first_block->type == SB_SUPSUB_CENTER,
         "First StrBlock has invalid type in GetMinilineHomeXY()",
         NULL);
#endif /* _TGIF_DBG */
   GetTextXY(pMiniLine->first_block, 0, pn_x, pn_baseline_y);
}

static
void GetMinilineEndXY(pMiniLine, pn_x, pn_baseline_y)
   MiniLineInfo *pMiniLine;
   int *pn_x, *pn_baseline_y;
{
   StrBlockInfo *pStrBlock=NULL;
   int index=INVALID;

   for (pStrBlock=pMiniLine->first_block; pStrBlock->next != NULL;
         pStrBlock=pStrBlock->next) {
   }
   switch (pStrBlock->type) {
   case SB_SIMPLE:
   case SB_SUPSUB_CENTER:
      index = pStrBlock->seg->dyn_str.sz-1;
      break;
   case SB_CHAR_SPACE:
#ifdef _TGIF_DBG /* debug, do not translate */
      TgAssert(FALSE,
            "Last StrBlock has type of SB_CHAR_SPACE in GetMinilineEndXY()",
            NULL);
#endif /* _TGIF_DBG */
      index = 1;
      break;
   }
   GetTextXY(pStrBlock, index, pn_x, pn_baseline_y);
}

static
void PaintTextHighlight()
{
   XGCValues values;

   if (!UpdateTextHighlightInfo()) return;

   if (useAltEditTextBgColor) {
      if (altEditTextHighlightIndex == INVALID) {
         values.foreground = myBgPixel;
      } else {
         values.foreground = colorPixels[altEditTextHighlightIndex];
      }
   } else {
      values.foreground = myFgPixel;
   }
   values.function = GXcopy;
   values.fill_style = FillSolid;
   XChangeGC(mainDisplay, drawGC, GCForeground | GCFunction | GCFillStyle,
         &values);
   if (gstTextHighlightInfo.start_min_y == gstTextHighlightInfo.end_min_y) {
      int w=(gstTextHighlightInfo.end_x-gstTextHighlightInfo.start_x);
      int h=(gstTextHighlightInfo.start_max_y-gstTextHighlightInfo.start_min_y);

      XFillRectangle(mainDisplay, drawWindow, drawGC,
            gstTextHighlightInfo.start_x, gstTextHighlightInfo.start_min_y,
            w, h);
   } else {
      int start_x=0, end_x=0, min_y=0, max_y=0;
      MiniLineInfo *pMiniLine=NULL;
      MiniLineInfo *pStartMiniLine=
            gstTextHighlightInfo.start_str_block_ptr->owner_mini_line;
      MiniLineInfo *pEndMiniLine=
            gstTextHighlightInfo.end_str_block_ptr->owner_mini_line;

      start_x = gstTextHighlightInfo.start_x;
      GetMinilineEndXY(pStartMiniLine, &end_x, NULL);
      min_y = gstTextHighlightInfo.start_min_y;
      max_y = gstTextHighlightInfo.start_max_y;
      XFillRectangle(mainDisplay, drawWindow, drawGC,
            start_x, min_y, end_x-start_x, max_y-min_y);
      for (pMiniLine=pStartMiniLine->next; pMiniLine != pEndMiniLine;
            pMiniLine=pMiniLine->next) {
         int left_baseline_y=0, right_baseline_y=0;

         GetMinilineHomeXY(pMiniLine, &start_x, &left_baseline_y);
         GetMinilineEndXY(pMiniLine, &end_x, &right_baseline_y);
         if (left_baseline_y != right_baseline_y) {
            sprintf(gszMsgBox, TgLoadString(STID_BASELINE_YS_NOT_EQ_IN_FUNC),
                  "PaintTextHighlight()");
            MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
         }
         min_y = left_baseline_y - pMiniLine->asc;
         max_y = left_baseline_y + pMiniLine->des;
         XFillRectangle(mainDisplay, drawWindow, drawGC,
               start_x, min_y, end_x-start_x, max_y-min_y);
      }
      GetMinilineHomeXY(pMiniLine, &start_x, NULL);
      end_x = gstTextHighlightInfo.end_x;
      min_y = gstTextHighlightInfo.end_min_y;
      max_y = gstTextHighlightInfo.end_max_y;
      XFillRectangle(mainDisplay, drawWindow, drawGC,
            start_x, min_y, end_x-start_x, max_y-min_y);
   }
}

static
void GetPaintMode(pStrBlock, pnMode, pnFirstIndex, pnSecondIndex)
   int *pnMode, *pnFirstIndex, *pnSecondIndex;
   StrBlockInfo *pStrBlock;
{
   *pnMode = gstTextHighlightInfo.mode;
   *pnFirstIndex = INVALID;
   *pnSecondIndex = INVALID;

   if (gstTextHighlightInfo.highlighting) {
      if (pStrBlock == gstTextHighlightInfo.end_str_block_ptr) {
         if (pStrBlock == gstTextHighlightInfo.end_str_block_ptr) {
            *pnMode = PAINT_INV_NORM;
            *pnFirstIndex = gstTextHighlightInfo.end_index;
         } else {
            *pnMode = PAINT_INV;
         }
      }
   } else {
      if (pStrBlock == gstTextHighlightInfo.start_str_block_ptr) {
         if (pStrBlock == gstTextHighlightInfo.end_str_block_ptr) {
            *pnMode = PAINT_NORM_INV_NORM;
            *pnFirstIndex = gstTextHighlightInfo.start_index;
            *pnSecondIndex = gstTextHighlightInfo.end_index;
         } else {
            *pnMode = PAINT_NORM_INV;
            *pnFirstIndex = gstTextHighlightInfo.start_index;
            gstTextHighlightInfo.highlighting = TRUE;
         }
      }
   }
   gstTextHighlightInfo.mode = (*pnMode);
}

static
void PaintString(dpy, d, gc, depth, x, baseline_y, buf, len, color_pixel,
      underline_on, overline_on, mode, first_index, second_index)
   Display *dpy;
   Drawable d;
   GC gc;
   int depth, x, baseline_y, len, color_pixel, underline_on, overline_on;
   int mode, first_index, second_index;
   char *buf;
   /*
    * x is the left edge of the string.
    */
{
   int inv_pixel=myBgPixel, w=0;
   int underline_yoffset=curUnderlineYOffset;
   int overline_yoffset=curOverlineYOffset;
   int overunder=(underline_on || overline_on);

   if (useAltEditTextBgColor) {
      if (altEditTextBgIndex == INVALID) {
         inv_pixel = myFgPixel;
      } else {
         inv_pixel = colorPixels[altEditTextBgIndex];
      }
   } else {
      inv_pixel = myBgPixel;
   }
   if (mode & PAINT) {
      switch (mode) {
      case PAINT_NORM:
         XSetForeground(dpy, gc, color_pixel);
         MyDrawString(dpy, d, gc, depth, x, baseline_y, buf, len);
         if (overunder) {
            w = MyTextWidth(canvasFontPtr, buf, len);
            if (underline_on) {
               XDrawLine(dpy, d, gc, x, baseline_y+underline_yoffset,
                     x+w, baseline_y+underline_yoffset);
            }
            if (overline_on) {
               XDrawLine(dpy, d, gc,
                     x, baseline_y-canvasFontAsc-overline_yoffset,
                     x+w, baseline_y-canvasFontAsc-overline_yoffset);
            }
         }
         break;
      case PAINT_INV:
         XSetForeground(dpy, gc, inv_pixel);
         MyDrawString(dpy, d, gc, depth, x, baseline_y, buf, len);
         if (overunder) {
            w = MyTextWidth(canvasFontPtr, buf, len);
            if (underline_on) {
               XDrawLine(dpy, d, gc, x, baseline_y+underline_yoffset,
                     x+w, baseline_y+underline_yoffset);
            }
            if (overline_on) {
               XDrawLine(dpy, d, gc,
                     x, baseline_y-canvasFontAsc-overline_yoffset,
                     x+w, baseline_y-canvasFontAsc-overline_yoffset);
            }
         }
         break;
      case PAINT_NORM_INV:
         XSetForeground(dpy, gc, color_pixel);
         MyDrawString(dpy, d, gc, depth, x, baseline_y, buf, first_index);
         w = MyTextWidth(canvasFontPtr, buf, first_index);
         if (underline_on) {
            XDrawLine(dpy, d, gc, x, baseline_y+underline_yoffset,
                  x+w, baseline_y+underline_yoffset);
         }
         if (overline_on) {
            XDrawLine(dpy, d, gc, x, baseline_y-canvasFontAsc-overline_yoffset,
                  x+w, baseline_y-canvasFontAsc-overline_yoffset);
         }
         x += w;
         XSetForeground(dpy, gc, inv_pixel);
         MyDrawString(dpy, d, gc, depth, x, baseline_y, &buf[first_index],
               len-first_index);
         if (overunder) {
            w = MyTextWidth(canvasFontPtr, &buf[first_index], len-first_index);
            if (underline_on) {
               XDrawLine(dpy, d, gc, x, baseline_y+underline_yoffset,
                     x+w, baseline_y+underline_yoffset);
            }
            if (overline_on) {
               XDrawLine(dpy, d, gc,
                     x, baseline_y-canvasFontAsc-overline_yoffset,
                     x+w, baseline_y-canvasFontAsc-overline_yoffset);
            }
         }
         break;
      case PAINT_INV_NORM:
         XSetForeground(dpy, gc, inv_pixel);
         MyDrawString(dpy, d, gc, depth, x, baseline_y, buf, first_index);
         w = MyTextWidth(canvasFontPtr, buf, first_index);
         if (underline_on) {
            XDrawLine(dpy, d, gc, x, baseline_y+underline_yoffset,
                  x+w, baseline_y+underline_yoffset);
         }
         if (overline_on) {
            XDrawLine(dpy, d, gc, x, baseline_y-canvasFontAsc-overline_yoffset,
                  x+w, baseline_y-canvasFontAsc-overline_yoffset);
         }
         x += w;
         XSetForeground(dpy, gc, color_pixel);
         MyDrawString(dpy, d, gc, depth, x, baseline_y, &buf[first_index],
               len-first_index);
         if (overunder) {
            w = MyTextWidth(canvasFontPtr, &buf[first_index], len-first_index);
            if (underline_on) {
               XDrawLine(dpy, d, gc, x, baseline_y+underline_yoffset,
                     x+w, baseline_y+underline_yoffset);
            }
            if (overline_on) {
               XDrawLine(dpy, d, gc,
                     x, baseline_y-canvasFontAsc-overline_yoffset,
                     x+w, baseline_y-canvasFontAsc-overline_yoffset);
            }
         }
         break;
      case PAINT_NORM_INV_NORM:
         XSetForeground(dpy, gc, color_pixel);
         MyDrawString(dpy, d, gc, depth, x, baseline_y, buf, first_index);
         w = MyTextWidth(canvasFontPtr, buf, first_index);
         if (underline_on) {
            XDrawLine(dpy, d, gc, x, baseline_y+underline_yoffset,
                  x+w, baseline_y+underline_yoffset);
         }
         if (overline_on) {
            XDrawLine(dpy, d, gc, x, baseline_y-canvasFontAsc-overline_yoffset,
                  x+w, baseline_y-canvasFontAsc-overline_yoffset);
         }
         x += w;
         XSetForeground(dpy, gc, inv_pixel);
         MyDrawString(dpy, d, gc, depth, x, baseline_y, &buf[first_index],
               second_index-first_index);
         w = MyTextWidth(canvasFontPtr, &buf[first_index],
               second_index-first_index);
         if (underline_on) {
            XDrawLine(dpy, d, gc, x, baseline_y+underline_yoffset,
                  x+w, baseline_y+underline_yoffset);
         }
         if (overline_on) {
            XDrawLine(dpy, d, gc, x, baseline_y-canvasFontAsc-overline_yoffset,
                  x+w, baseline_y-canvasFontAsc-overline_yoffset);
         }
         x += w;
         XSetForeground(dpy, gc, color_pixel);
         MyDrawString(dpy, d, gc, depth, x, baseline_y, &buf[second_index],
               len-second_index);
         if (overunder) {
            w = MyTextWidth(canvasFontPtr, &buf[second_index],
                  len-second_index);
            if (underline_on) {
               XDrawLine(dpy, d, gc, x, baseline_y+underline_yoffset,
                     x+w, baseline_y+underline_yoffset);
            }
            if (overline_on) {
               XDrawLine(dpy, d, gc,
                     x, baseline_y-canvasFontAsc-overline_yoffset,
                     x+w, baseline_y-canvasFontAsc-overline_yoffset);
            }
         }
         break;
      }
   }
}

static
void PaintStrSeg(dpy, d, gc, depth, x, baseline_y, pStrSeg, use_highlight,
      use_as_mask, mode, first_index, second_index)
   Display *dpy;
   Drawable d;
   GC gc;
   int depth, x, baseline_y, use_highlight, use_as_mask;
   int mode, first_index, second_index;
   StrSegInfo *pStrSeg;
   /*
    * x is the left edge of the string segment.
    */
{
   XGCValues values;

   curFont = pStrSeg->font;
   curStyle = pStrSeg->style;
   curSzUnit = pStrSeg->sz_unit;

   SetCanvasFont();

   if (use_as_mask) {
      values.foreground = 1;
   } else if (colorDisplay) {
      values.foreground = colorPixels[pStrSeg->color];
   } else {
      values.foreground = myFgPixel;
   }
   values.function = GXcopy;
   values.fill_style = (transPat ? FillStippled : FillOpaqueStippled);
   values.stipple = patPixmap[penPat];
   values.font = canvasFontPtr->fid;
   values.line_width = 1;
   values.line_style = LineSolid;
   XChangeGC(dpy, gc, GCForeground | GCFunction | GCFillStyle | GCStipple |
         GCFont | GCLineWidth | GCLineStyle, &values);

   if (use_highlight && !use_as_mask) {
      PaintString(dpy, d, gc, depth, x, baseline_y, pStrSeg->dyn_str.s,
            pStrSeg->dyn_str.sz-1,
            (colorDisplay ? colorPixels[pStrSeg->color] : myFgPixel),
            pStrSeg->underline_on, pStrSeg->overline_on, mode, first_index,
            second_index);
      switch (mode) {
      case PAINT_NORM:
      case PAINT_INV_NORM:
      case PAINT_NORM_INV_NORM:
         gstTextHighlightInfo.mode = PAINT_NORM;
         break;
      case PAINT_INV:
      case PAINT_NORM_INV:
         gstTextHighlightInfo.mode = PAINT_INV;
         break;
      }
   } else {
      MyDrawString(dpy, d, gc, depth, x, baseline_y, pStrSeg->dyn_str.s,
            pStrSeg->dyn_str.sz-1);
      if (pStrSeg->underline_on) {
         int offset=curUnderlineYOffset;

         XDrawLine(dpy, d, gc, x, baseline_y+offset, x+pStrSeg->w,
               baseline_y+offset);
      }
      if (pStrSeg->overline_on) {
         int offset=curOverlineYOffset;

         XDrawLine(dpy, d, gc, x, baseline_y-canvasFontAsc-offset, x+pStrSeg->w,
               baseline_y-canvasFontAsc-offset);
      }
   }
}

static
void PaintStrBlock(dpy, d, gc, depth, x, baseline_y, pStrBlock, use_highlight,
      use_as_mask)
   Display *dpy;
   Drawable d;
   GC gc;
   int depth, x, baseline_y, use_highlight, use_as_mask;
   StrBlockInfo *pStrBlock;
   /*
    * x is the left edge of the string block.
    */
{
   int saved_baseline_y=baseline_y;
   int block_w=pStrBlock->w;
   int mode=INVALID, first_index=INVALID, second_index=INVALID;

   if (GetDirtyBBox(NULL) && !IntersectDirtyBBox(&pStrBlock->bbox)) {
      return;
   }
   if (use_highlight && !use_as_mask) {
      switch (pStrBlock->type) {
      case SB_SIMPLE:
      case SB_CHAR_SPACE:
      case SB_SUPSUB_CENTER:
         GetPaintMode(pStrBlock, &mode, &first_index, &second_index);
         break;
      }
   }
   switch (pStrBlock->type) {
   case SB_SIMPLE:
      PaintStrSeg(dpy, d, gc, depth, x, baseline_y, pStrBlock->seg,
            use_highlight, use_as_mask, mode, first_index, second_index);
      break;

   case SB_CHAR_SPACE: break;

   case SB_SUPSUB_LEFT:
   case SB_SUPSUB_CENTER:
   case SB_SUPSUB_RIGHT:
      switch (pStrBlock->type) {
      case SB_SUPSUB_LEFT: x += block_w; break;
      case SB_SUPSUB_CENTER: x += (block_w>>1); break;
      case SB_SUPSUB_RIGHT: break;
      }
      if (pStrBlock->type == SB_SUPSUB_CENTER) {
         /* pStrBlock->seg better not be NULL or it will crash */
         baseline_y = saved_baseline_y;
         PaintStrSeg(dpy, d, gc, depth, x-((pStrBlock->seg->w)>>1), baseline_y,
               pStrBlock->seg, use_highlight, use_as_mask, mode, first_index,
               second_index);
      }
      if (pStrBlock->sup != NULL) {
         baseline_y = saved_baseline_y + pStrBlock->sup->baseline_offset;
         if (pStrBlock->type == SB_SUPSUB_CENTER) {
            baseline_y -= pStrBlock->seg->asc;
         }
         PaintMiniLines(dpy, d, gc, depth, x, baseline_y, pStrBlock->sup,
               use_highlight, use_as_mask);
      }
      if (pStrBlock->sub != NULL) {
         baseline_y = saved_baseline_y + pStrBlock->sub->baseline_offset;
         if (pStrBlock->type == SB_SUPSUB_CENTER) {
            baseline_y += pStrBlock->seg->des;
         }
         PaintMiniLines(dpy, d, gc, depth, x, baseline_y, pStrBlock->sub,
               use_highlight, use_as_mask);
      }
      break;
   }
}

static
void PaintMiniLine(dpy, d, gc, depth, x, baseline_y, pMiniLine, use_highlight,
      use_as_mask)
   Display *dpy;
   Drawable d;
   GC gc;
   int depth, x, baseline_y, use_highlight, use_as_mask;
   MiniLineInfo *pMiniLine;
   /*
    * x is the left edge of the miniline and baseline_y has been adjusted by
    *       pMiniLine->v_gap.
    */
{
   StrBlockInfo *pStrBlock=NULL;

   if (GetDirtyBBox(NULL) && !IntersectDirtyBBox(&pMiniLine->bbox)) {
      return;
   }
   for (pStrBlock=pMiniLine->first_block; pStrBlock != NULL;
         pStrBlock=pStrBlock->next) {
      PaintStrBlock(dpy, d, gc, depth, x, baseline_y, pStrBlock, use_highlight,
            use_as_mask);

      x += pStrBlock->w;
   }
}

void PaintMiniLines(dpy, d, gc, depth, x, baseline_y, minilines, use_highlight,
      use_as_mask)
   Display *dpy;
   Drawable d;
   GC gc;
   int depth, x, baseline_y, use_highlight, use_as_mask;
   MiniLinesInfo *minilines;
   /*
    * x is the origin of the minilines (not the left edge)
    */
{
   MiniLineInfo *pMiniLine=NULL;
   int v_space=minilines->v_space;
   int saved_x=x;

   if (GetDirtyBBox(NULL) && !IntersectDirtyBBox(&minilines->bbox)) {
      return;
   }
   for (pMiniLine=minilines->first; pMiniLine != NULL;
         pMiniLine=pMiniLine->next) {
#ifdef _TGIF_DBG /* debug, do not translate */
      if (pMiniLine == minilines->first) {
         TgAssert(pMiniLine->v_gap == 0,
               "First mini_line has non-zero v_gap in PaintMiniLines()",
               NULL);
      }
#endif /* _TGIF_DBG */
      baseline_y += pMiniLine->v_gap;

      switch (minilines->just) {
      case JUST_L: x = saved_x; break;
      case JUST_C: x = saved_x - ((pMiniLine->w)>>1); break;
      case JUST_R: x = saved_x - pMiniLine->w; break;
      }
      PaintMiniLine(dpy, d, gc, depth, x, baseline_y, pMiniLine, use_highlight,
            use_as_mask);

      baseline_y += pMiniLine->des + v_space;
      if (pMiniLine->next != NULL) {
         baseline_y += pMiniLine->next->asc;
      }
   }
}

static
void PaintCurText()
{
   struct TextRec *text_ptr=curTextObj->detail.t;
   MiniLinesInfo *minilines=(&text_ptr->minilines);
   int baseline_y=textOrigBaselineY, saved_pen=penPat;

   PushCurFont();

   memset(&gstTextHighlightInfo, 0, sizeof(TextHighlightInfo));
   if (textHighlight) {
      PaintTextHighlight();
   }
   gstTextHighlightInfo.highlighting = FALSE;
   gstTextHighlightInfo.mode = PAINT_NORM;

   curUnderlineYOffset = text_ptr->underline_y_offset;
   curOverlineYOffset = text_ptr->overline_y_offset;
   penPat = SOLIDPAT;
   PaintMiniLines(mainDisplay, drawWindow, drawGC, mainDepth,
         textOrigX, baseline_y, minilines, TRUE, FALSE);
   penPat = saved_pen;

   PopCurFont();
}

void DrawCurTextOutLine(x, y, w, h)
   int x, y, w, h;
{
   XGCValues values;

   values.foreground = myFgPixel;
   values.function = GXcopy;
   values.fill_style = FillOpaqueStippled;
   values.stipple = patPixmap[SCROLLPAT];
   values.line_width = curTextOutlineW;
   values.line_style = LineSolid;
   values.join_style = JoinMiter;
   XChangeGC(mainDisplay, drawGC,
         GCForeground | GCFunction | GCFillStyle | GCStipple | GCLineWidth |
         GCLineStyle | GCJoinStyle, &values);

   XDrawRectangle(mainDisplay, drawWindow, drawGC,
         x-curTextOutlineHalfW, y-curTextOutlineHalfW,
         w+(curTextOutlineHalfW<<1), h+(curTextOutlineHalfW<<1));
}

void RedrawCurText()
{
   int x=0, y=0, w=0, h=0, clipped=FALSE;
   XGCValues values;

   if (curChoice != DRAWTEXT || !textCursorShown) return;

   if (GetDirtyBBox(NULL)) {
      struct BBRec draw_win_bbox;

      draw_win_bbox.ltx = draw_win_bbox.lty = 0;
      draw_win_bbox.rbx = initDrawWinW;
      draw_win_bbox.rby = initDrawWinH;
      if (!IntersectRect(gstDirtyBBoxInfo.bbox, draw_win_bbox,
            &gstDirtyBBoxInfo.bbox)) {
         return;
      }
      SetRecVals(clipRecs[0], gstDirtyBBoxInfo.bbox.ltx,
            gstDirtyBBoxInfo.bbox.lty,
            gstDirtyBBoxInfo.bbox.rbx-gstDirtyBBoxInfo.bbox.ltx+1,
            gstDirtyBBoxInfo.bbox.rby-gstDirtyBBoxInfo.bbox.lty+1);
      numClipRecs = 1;
      clipOrdering = YXBanded;
      XSetClipRectangles(mainDisplay, drawGC, 0, 0, clipRecs, numClipRecs,
            clipOrdering);
      clipped = TRUE;
   }
   GetCurTextBBoxes(NULL, &curTextBBox);
   x = curTextBBox.ltx-2;
   y = curTextBBox.lty-2;
   w = curTextBBox.rbx-curTextBBox.ltx+4;
   h = curTextBBox.rby-curTextBBox.lty+4;

   if (useAltEditTextBgColor) { 
      if (altEditTextBgIndex == INVALID) {
         values.foreground = myFgPixel;
      } else {
         values.foreground = colorPixels[altEditTextBgIndex];
      }
   } else {
      values.foreground = GetDrawingBgPixel(INVALID, INVALID);
   }
   values.function = GXcopy;
   values.fill_style = FillSolid;
   values.line_width = 1;
   values.line_style = LineSolid;
   XChangeGC(mainDisplay, drawGC,
         GCForeground | GCFunction | GCFillStyle | GCLineWidth | GCLineStyle,
         &values);
   XFillRectangle(mainDisplay, drawWindow, drawGC, x, y, w, h);
   DrawCurTextOutLine(x, y, w, h);

   PaintCurText();
   if (clipped) {
      SetDefaultDrawWinClipRecs();
   }
   PutTextCursor();
   ResetDirtyBBoxInfo();
   UpdateCurTextBBoxes(TRUE);
}

/* --------------------- UpdateHighLightedTextBBoxes() --------------------- */

void UpdateHighLightedTextBBoxes(clean)
   int clean;
{
   if (clean) {
      ResetDirtyBBoxInfo();
   } else {
      UpdateCurTextBBoxes(FALSE);
   }
   AddToDirtyBBox(&curStrBlock->clean_bbox);

   if (textHighlight && UpdateTextHighlightInfo()) {
      int w=(gstTextHighlightInfo.end_x-gstTextHighlightInfo.start_x);
      int h=(gstTextHighlightInfo.start_max_y-gstTextHighlightInfo.start_min_y);
      struct BBRec bbox;

      SetBBRec(&bbox, gstTextHighlightInfo.start_x,
            gstTextHighlightInfo.start_min_y, gstTextHighlightInfo.start_x+w,
            gstTextHighlightInfo.start_min_y+h);
      AddToDirtyBBox(&endStrBlock->clean_bbox);
      AddToDirtyBBox(&bbox);

      if (gstTextHighlightInfo.start_min_y != gstTextHighlightInfo.end_min_y) {
         int start_x=0, end_x=0, min_y=0, max_y=0;
         MiniLineInfo *pMiniLine=NULL;
         MiniLineInfo *pStartMiniLine=
               gstTextHighlightInfo.start_str_block_ptr->owner_mini_line;
         MiniLineInfo *pEndMiniLine=
               gstTextHighlightInfo.end_str_block_ptr->owner_mini_line;

         AddToDirtyBBox(&endStrBlock->clean_bbox);

         start_x = gstTextHighlightInfo.start_x;
         GetMinilineEndXY(pStartMiniLine, &end_x, NULL);
         min_y = gstTextHighlightInfo.start_min_y;
         max_y = gstTextHighlightInfo.start_max_y;
         SetBBRec(&bbox, start_x, min_y, end_x, max_y);
         AddToDirtyBBox(&bbox);
         for (pMiniLine=pStartMiniLine->next; pMiniLine != pEndMiniLine;
               pMiniLine=pMiniLine->next) {
            int left_baseline_y=0, right_baseline_y=0;

            GetMinilineHomeXY(pMiniLine, &start_x, &left_baseline_y);
            GetMinilineEndXY(pMiniLine, &end_x, &right_baseline_y);
            if (left_baseline_y != right_baseline_y) {
               sprintf(gszMsgBox, TgLoadString(STID_BASELINE_YS_NOT_EQ_IN_FUNC),
                     "PaintTextHighlight()");
               MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
            }
            min_y = left_baseline_y - pMiniLine->asc;
            max_y = left_baseline_y + pMiniLine->des;
            SetBBRec(&bbox, start_x, min_y, end_x, max_y);
            AddToDirtyBBox(&bbox);
         }
         GetMinilineHomeXY(pMiniLine, &start_x, NULL);
         end_x = gstTextHighlightInfo.end_x;
         min_y = gstTextHighlightInfo.end_min_y;
         max_y = gstTextHighlightInfo.end_max_y;
         SetBBRec(&bbox, start_x, min_y, end_x, max_y);
         AddToDirtyBBox(&bbox);
      }
   }
}

/* --------------------- DeleteHighlightedText() --------------------- */

static
int DeleteHighlightedTextInStrSeg(pStrSeg, mode, first_index, second_index)
   StrSegInfo *pStrSeg;
   int mode, first_index, second_index;
   /* return TRUE if all done deleting */
{
   char *psz=NULL, *psz1=NULL, *psz2=NULL;

   switch (mode) {
   case PAINT_NORM: break;
   case PAINT_INV: break;

   case PAINT_NORM_INV:
      /* delete the tail */
      pStrSeg->dyn_str.s[first_index] = '\0';
      if ((psz=UtilStrDup(pStrSeg->dyn_str.s)) == NULL) FailAllocMessage();
      DynStrSet(&pStrSeg->dyn_str, psz);
      UtilFree(psz);
      gstTextHighlightInfo.mode = PAINT_INV;
      break;

   case PAINT_INV_NORM:
      if ((psz=UtilStrDup(&pStrSeg->dyn_str.s[first_index])) == NULL) {
         FailAllocMessage();
      }
      DynStrSet(&pStrSeg->dyn_str, psz);
      UtilFree(psz);
      gstTextHighlightInfo.mode = PAINT_NORM;
      return TRUE;

   case PAINT_NORM_INV_NORM:
      if ((psz=UtilStrDup(pStrSeg->dyn_str.s)) == NULL) {
         FailAllocMessage();
      }
      for (psz1=(&psz[first_index]), psz2=(&psz[second_index]);
            *psz2 != '\0'; psz1++, psz2++) {
         *psz1 = *psz2;
      }
      *psz1 = '\0';
      DynStrSet(&pStrSeg->dyn_str, psz);
      UtilFree(psz);
      gstTextHighlightInfo.mode = PAINT_NORM;
      return TRUE;
   }
   return FALSE;
}

void MergeNextMiniLineWhileDeleting(pMiniLine)
   MiniLineInfo *pMiniLine;
   /* combine pMiniLine and pMiniLine->next */
{
   StrBlockInfo *pStrBlock=NULL;
   MiniLineInfo *pMiniLineToFree=pMiniLine->next;

#ifdef _TGIF_DBG /* debug, do not translate */
   if (pMiniLine->next == NULL) {
      sprintf(gszMsgBox, "%s: %s %s.",
            "Failed assertion", "pMiniLine->next == NULL in",
            "MergeNextMiniLineWhileDeleting()");
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
   }
#endif /* _TGIF_DBG */
   for (pStrBlock=pMiniLineToFree->first_block; pStrBlock != NULL;
         pStrBlock=pStrBlock->next) {
      pStrBlock->owner_mini_line = pMiniLine;
   }
   pMiniLine->last_block->next = pMiniLineToFree->first_block;
   pMiniLineToFree->first_block->prev = pMiniLine->last_block;
   pMiniLine->last_block = pMiniLineToFree->last_block;
   UnlinkMiniLine(pMiniLineToFree);
   pMiniLineToFree->first_block = pMiniLineToFree->last_block = NULL;
   FreeMiniLine(pMiniLineToFree);
}

int StrBlocksHasSameProperties(pStrBlock1, pStrBlock2)
   StrBlockInfo *pStrBlock1, *pStrBlock2;
{
   if (pStrBlock1->type != pStrBlock2->type) {
      return FALSE;
   } else if (pStrBlock1->type == SB_SIMPLE) {
      StrSegInfo *pStrSeg1=pStrBlock1->seg, *pStrSeg2=pStrBlock2->seg;

      if (pStrSeg1->color == pStrSeg2->color &&
            pStrSeg1->underline_on == pStrSeg2->underline_on &&
            pStrSeg1->overline_on == pStrSeg2->overline_on &&
            pStrSeg1->font == pStrSeg2->font &&
            pStrSeg1->style == pStrSeg2->style &&
            pStrSeg1->sz_unit == pStrSeg2->sz_unit) {
         return TRUE;
      }
   } else if (pStrBlock1->type == SB_CHAR_SPACE) {
      return TRUE;
   }
   return FALSE;
}

static
void MergeNextStrBlockWhileDeleting(pStrBlock, pNextStrBlock)
   StrBlockInfo *pStrBlock, *pNextStrBlock;
   /* combine pStrBlock and pNextStrBlock */
{
   int len=pStrBlock->seg->dyn_str.sz+pNextStrBlock->seg->dyn_str.sz-1;
   char *psz=(char*)malloc(len+1);

   if (psz == NULL) FailAllocMessage();
   sprintf(psz, "%s%s",
         pStrBlock->seg->dyn_str.s, pNextStrBlock->seg->dyn_str.s);
   DynStrSet(&pStrBlock->seg->dyn_str, psz);
   free(psz);

   UnlinkStrBlock(pNextStrBlock);
   FreeStrBlock(pNextStrBlock);
}

int MergeStrBlocks(pStrBlock, pNextStrBlock)
   StrBlockInfo *pStrBlock, *pNextStrBlock;
{
   if (pStrBlock->type == SB_SIMPLE && StrBlocksHasSameProperties(pStrBlock,
         pNextStrBlock)) {
      MergeNextStrBlockWhileDeleting(pStrBlock, pNextStrBlock);
      return TRUE;
   }
   return FALSE;
}

void DeleteHighlightedText()
{
   MiniLineInfo *pOwnerMiniLine=NULL;
   StrBlockInfo *saved_str_block=NULL, *pStrBlock=NULL, *pNextStrBlock=NULL;
   int saved_index=INVALID, first_index=0, second_index=0, mode=PAINT_NORM;

   if (!textHighlight) return;

   /* setup gstTextHighlightInfo */
   if (!UpdateTextHighlightInfo()) return;

   gstTextHighlightInfo.highlighting = FALSE;
   gstTextHighlightInfo.mode = PAINT_NORM;

   pStrBlock = saved_str_block = gstTextHighlightInfo.start_str_block_ptr;
   saved_index = gstTextHighlightInfo.start_index;
   pOwnerMiniLine = pStrBlock->owner_mini_line;

   AddToDirtyBBox(&pStrBlock->clean_bbox);

   GetPaintMode(pStrBlock, &mode, &first_index, &second_index);
   DeleteHighlightedTextInStrSeg(pStrBlock->seg, mode, first_index,
         second_index);
   mode = gstTextHighlightInfo.mode;
   pStrBlock = pStrBlock->next;
   while (mode != PAINT_NORM) {
      if (pStrBlock == NULL) {
         /* combine pOwnerMiniLine and pOwnerMiniLine->next */
         pNextStrBlock = pOwnerMiniLine->next->first_block;
         MergeNextMiniLineWhileDeleting(pOwnerMiniLine);
         pStrBlock = pNextStrBlock;
         continue;
      }
      pNextStrBlock = pStrBlock->next;
      GetPaintMode(pStrBlock, &mode, &first_index, &second_index);
      AddToDirtyBBox(&pStrBlock->clean_bbox);
      if (mode == PAINT_INV) {
         UnlinkStrBlock(pStrBlock);
         FreeStrBlock(pStrBlock);
         pStrBlock = NULL;
      } else {
         /* pStrBlock->type better be SB_SIMPLE */
#ifdef _TGIF_DBG /* debug, do not translate */
         TgAssert(pStrBlock->type == SB_SIMPLE,
               "pStrBlock->type != SB_SIMPLE in DeleteHighlightedText()",
               NULL);
#endif /* _TGIF_DBG */
         DeleteHighlightedTextInStrSeg(pStrBlock->seg, mode, first_index,
               second_index);
      }
      mode = gstTextHighlightInfo.mode;
      pStrBlock = pNextStrBlock;
   }
   curStrBlock = saved_str_block;
   textCurIndex = saved_index;

   ResetOnCursorKey(FALSE);
   textHighlight = FALSE;

   while (*curStrBlock->seg->dyn_str.s == '\0' &&
         curStrBlock->next != NULL &&
         curStrBlock->next->type == SB_SIMPLE &&
         *curStrBlock->next->seg->dyn_str.s == '\0') {
      pStrBlock = curStrBlock->next;
      UnlinkStrBlock(pStrBlock);
      FreeStrBlock(pStrBlock);
      pStrBlock = NULL;
   }
   while (curStrBlock->next != NULL) {
      if (!MergeStrBlocks(curStrBlock, curStrBlock->next)) {
         break;
      }
   }
   SetTextCurXY();
}

/* --------------------- DoInsertThinSpace() --------------------- */

static
void DetermineWhatToDoForSpace(pnSplitAtIndex, pnInsertBefore)
   int *pnSplitAtIndex, *pnInsertBefore;
{
   if (textCurIndex == 0 && curStrBlock->seg->dyn_str.sz != 1) {
      if (curStrBlock->prev == NULL ||
            curStrBlock->prev->type != SB_SIMPLE) {
         *pnSplitAtIndex = TRUE;
      } else {
         *pnInsertBefore = TRUE;
      }
   } else if (textCurIndex == curStrBlock->seg->dyn_str.sz-1) {
      if (curStrBlock->next == NULL ||
            curStrBlock->next->type != SB_SIMPLE) {
         *pnSplitAtIndex = TRUE;
      } else {
         *pnInsertBefore = FALSE;
      }
   } else {
      *pnSplitAtIndex = TRUE;
   }
}

void DoInsertThinSpace(thin_space_w)
   int thin_space_w;
{
   int split_at_index=FALSE, insert_before=FALSE;
   MiniLineInfo *owner_mini_line=curStrBlock->owner_mini_line;
   StrBlockInfo *pThinSpaceStrBlock=NewStrBlock();

   pThinSpaceStrBlock->type = SB_CHAR_SPACE;
   pThinSpaceStrBlock->special_char_w = pThinSpaceStrBlock->w = thin_space_w;
   pThinSpaceStrBlock->owner_mini_line = owner_mini_line;

   DetermineWhatToDoForSpace(&split_at_index, &insert_before);

   if (split_at_index) {
      StrBlockInfo *pLeftStrBlock=NULL;
      char *psz=UtilStrDup(curStrBlock->seg->dyn_str.s), saved_ch='\0';

      if (psz == NULL) FailAllocMessage();

      DupStrBlock(curStrBlock, owner_mini_line, &pLeftStrBlock, &pLeftStrBlock);
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
      pLeftStrBlock->next = pThinSpaceStrBlock;
      pThinSpaceStrBlock->prev = pLeftStrBlock;

      pThinSpaceStrBlock->next = curStrBlock;
      curStrBlock->prev = pThinSpaceStrBlock;

      textCurIndex = 0;
   } else if (insert_before) {
      pThinSpaceStrBlock->prev = curStrBlock->prev;
      curStrBlock->prev->next = pThinSpaceStrBlock;
      pThinSpaceStrBlock->next = curStrBlock;
      curStrBlock->prev = pThinSpaceStrBlock;
   } else {
      /* insert after */
      pThinSpaceStrBlock->next = curStrBlock->next;
      curStrBlock->next->prev = pThinSpaceStrBlock;

      pThinSpaceStrBlock->prev = curStrBlock;
      curStrBlock->next = pThinSpaceStrBlock;
   }
}

/* --------------------- InsertHighlightedThinSpace() --------------------- */

static
int InsertHighlightedThinSpaceInStrSeg(pStrSeg, mode, first_index, second_index)
   StrSegInfo *pStrSeg;
   int mode, first_index, second_index;
   /* return TRUE if all done deleting */
{
   char *psz=NULL, *psz1=NULL, *psz2=NULL;

   switch (mode) {
   case PAINT_NORM: break;
   case PAINT_INV: break;

   case PAINT_NORM_INV:
      /* delete the tail */
      pStrSeg->dyn_str.s[first_index] = '\0';
      if ((psz=UtilStrDup(pStrSeg->dyn_str.s)) == NULL) FailAllocMessage();
      DynStrSet(&pStrSeg->dyn_str, psz);
      UtilFree(psz);
      gstTextHighlightInfo.mode = PAINT_INV;
      break;

   case PAINT_INV_NORM:
      if ((psz=UtilStrDup(&pStrSeg->dyn_str.s[first_index])) == NULL) {
         FailAllocMessage();
      }
      DynStrSet(&pStrSeg->dyn_str, psz);
      UtilFree(psz);
      gstTextHighlightInfo.mode = PAINT_NORM;
      return TRUE;

   case PAINT_NORM_INV_NORM:
      if ((psz=UtilStrDup(pStrSeg->dyn_str.s)) == NULL) {
         FailAllocMessage();
      }
      for (psz1=(&psz[first_index]), psz2=(&psz[second_index]);
            *psz2 != '\0'; psz1++, psz2++) {
         *psz1 = *psz2;
      }
      *psz1 = '\0';
      DynStrSet(&pStrSeg->dyn_str, psz);
      UtilFree(psz);
      gstTextHighlightInfo.mode = PAINT_NORM;
      return TRUE;
   }
   return FALSE;
}

void InsertHighlightedThinSpace(thin_space_w)
   int thin_space_w;
{
   MiniLineInfo *pOwnerMiniLine=NULL;
   StrBlockInfo *saved_str_block=NULL, *pStrBlock=NULL, *pNextStrBlock=NULL;
   int saved_index=INVALID, first_index=0, second_index=0, mode=PAINT_NORM;

   if (!textHighlight) return;

   /* setup gstTextHighlightInfo */
   if (!UpdateTextHighlightInfo()) return;

   gstTextHighlightInfo.highlighting = FALSE;
   gstTextHighlightInfo.mode = PAINT_NORM;

   pStrBlock = saved_str_block = gstTextHighlightInfo.start_str_block_ptr;
   saved_index = gstTextHighlightInfo.start_index;
   pOwnerMiniLine = pStrBlock->owner_mini_line;

   AddToDirtyBBox(&pStrBlock->clean_bbox);

   GetPaintMode(pStrBlock, &mode, &first_index, &second_index);
   InsertHighlightedThinSpaceInStrSeg(pStrBlock->seg, mode, first_index,
         second_index);
   mode = gstTextHighlightInfo.mode;
   pStrBlock = pStrBlock->next;
   while (mode != PAINT_NORM) {
      if (pStrBlock == NULL) {
         /* combine pOwnerMiniLine and pOwnerMiniLine->next */
         pNextStrBlock = pOwnerMiniLine->next->first_block;
         pStrBlock = pNextStrBlock;
         continue;
      }
      pNextStrBlock = pStrBlock->next;
      GetPaintMode(pStrBlock, &mode, &first_index, &second_index);
      AddToDirtyBBox(&pStrBlock->clean_bbox);
      if (mode == PAINT_INV) {
         UnlinkStrBlock(pStrBlock);
         FreeStrBlock(pStrBlock);
         pStrBlock = NULL;
      } else {
         /* pStrBlock->type better be SB_SIMPLE */
#ifdef _TGIF_DBG /* debug, do not translate */
         TgAssert(pStrBlock->type == SB_SIMPLE,
               "pStrBlock->type != SB_SIMPLE in InsertHighlightedThinSpace()",
               NULL);
#endif /* _TGIF_DBG */
         InsertHighlightedThinSpaceInStrSeg(pStrBlock->seg, mode, first_index,
               second_index);
      }
      mode = gstTextHighlightInfo.mode;
      pStrBlock = pNextStrBlock;
   }
   curStrBlock = saved_str_block;
   textCurIndex = saved_index;

   ResetOnCursorKey(FALSE);
   textHighlight = FALSE;

   while (*curStrBlock->seg->dyn_str.s == '\0' &&
         curStrBlock->next != NULL &&
         curStrBlock->next->type == SB_SIMPLE &&
         *curStrBlock->next->seg->dyn_str.s == '\0') {
      pStrBlock = curStrBlock->next;
      UnlinkStrBlock(pStrBlock);
      FreeStrBlock(pStrBlock);
      pStrBlock = NULL;
   }
   SetTextCurXY();
}

/* --------------------- ShrinkMiniLines() --------------------- */

static
int ShrinkMiniLine(pMiniLine, pn_retry)
   MiniLineInfo *pMiniLine;
   int *pn_retry;
{
   StrBlockInfo *pStrBlock=pMiniLine->first_block;
   int changed=FALSE;

   while (pStrBlock != NULL) {
      StrBlockInfo *pNextStrBlock=pStrBlock->next;
      int advance=TRUE;

      switch (pStrBlock->type) {
      case SB_SIMPLE:
         if (pNextStrBlock != NULL && pNextStrBlock->type == SB_SIMPLE) {
            if (*pNextStrBlock->seg->dyn_str.s == '\0' ||
                  StrBlocksHasSameProperties(pStrBlock, pNextStrBlock)) {
               MergeNextStrBlockWhileDeleting(pStrBlock, pNextStrBlock);
               changed = TRUE;
               advance = FALSE;
            } else if (*pStrBlock->seg->dyn_str.s == '\0') {
               UnlinkStrBlock(pStrBlock);
               FreeStrBlock(pStrBlock);
               *pn_retry = TRUE;
               changed = TRUE;
               advance = FALSE;
               pStrBlock = pNextStrBlock;
            }
         }
         break;
      case SB_CHAR_SPACE:
         if (pStrBlock->special_char_w == 0) {
            UnlinkStrBlock(pStrBlock);
            FreeStrBlock(pStrBlock);
            *pn_retry = TRUE;
            changed = TRUE;
            advance = FALSE;
            pStrBlock = pNextStrBlock;
         }
         break;

      case SB_SUPSUB_LEFT:
      case SB_SUPSUB_CENTER:
      case SB_SUPSUB_RIGHT:
         if (pStrBlock->sup != NULL) {
            if (ShrinkMiniLines(pStrBlock->sup, pn_retry)) {
               changed = TRUE;
            }
         }
         if (pStrBlock->sub != NULL) {
            if (ShrinkMiniLines(pStrBlock->sub, pn_retry)) {
               changed = TRUE;
            }
         }
         if (pStrBlock->sup != NULL && pStrBlock->sub != NULL &&
               BlankMiniLines(pStrBlock->sup) &&
               BlankMiniLines(pStrBlock->sub)) {
            if ((pStrBlock->type == SB_SUPSUB_CENTER &&
                  *pStrBlock->seg->dyn_str.s == '\0') ||
                  pStrBlock->type != SB_SUPSUB_CENTER) {
               UnlinkStrBlock(pStrBlock);
               FreeStrBlock(pStrBlock);
               *pn_retry = TRUE;
               changed = TRUE;
               advance = FALSE;
               pStrBlock = pNextStrBlock;
            } else if (pStrBlock->type == SB_SUPSUB_CENTER &&
                  *pStrBlock->seg->dyn_str.s != '\0') {
               if (pStrBlock->sup != NULL) FreeMiniLines(pStrBlock->sup, TRUE);
               if (pStrBlock->sub != NULL) FreeMiniLines(pStrBlock->sub, TRUE);
               pStrBlock->sup = pStrBlock->sub = NULL;
               pStrBlock->type = SB_SIMPLE;
               *pn_retry = TRUE;
               changed = TRUE;
               advance = FALSE;
            }
         }
         break;
      }
      if (advance) {
         pStrBlock = pStrBlock->next;
      }
   }
   return changed;
}

int ShrinkMiniLines(minilines, pn_retry)
   MiniLinesInfo *minilines;
   int *pn_retry;
{
   MiniLineInfo *pMiniLine=NULL;
   int changed=FALSE;

   for (pMiniLine=minilines->first; pMiniLine != NULL;
         pMiniLine=pMiniLine->next) {
      if (ShrinkMiniLine(pMiniLine, pn_retry)) {
         changed = TRUE;
      }
   }
   return changed;
}

/* --------------------- GetTextXY() --------------------- */

static
int GetTextXYInStrSeg(pStrSeg, index_to_match, pn_dx)
   StrSegInfo *pStrSeg;
   int index_to_match, *pn_dx;
   /*
    * *pn_x is the amount moved by this routine
    */
{
   curFont = pStrSeg->font;
   curSzUnit = pStrSeg->sz_unit;
   curStyle = pStrSeg->style;
   SetCanvasFont();
   *pn_dx += MyTextWidth(canvasFontPtr, pStrSeg->dyn_str.s, index_to_match);

   return TRUE;
}

static
int GetTextXYInStrBlock(pStrBlock, pStrBlockToMatch, index_to_match,
      pn_dx, pn_dy)
   StrBlockInfo *pStrBlock;
   StrBlockInfo *pStrBlockToMatch;
   int index_to_match, *pn_dx, *pn_dy;
   /*
    * *pn_x and *pn_y are the amount moved by this routine
    */
{
   int dx=0, dy=0;

   switch (pStrBlock->type) {
   case SB_SIMPLE:
      if (pStrBlockToMatch == pStrBlock) {
         return GetTextXYInStrSeg(pStrBlock->seg, index_to_match, pn_dx);
      }
      break;

   case SB_CHAR_SPACE:
      if (pStrBlockToMatch == pStrBlock) {
#ifdef _TGIF_DBG /* debug, do not translate */
         TgAssert(FALSE,
               "StrBlock has type of SB_CHAR_SPACE in GetTextXYInStrBlock()",
               NULL);
#endif /* _TGIF_DBG */
         if (index_to_match == 1) {
            *pn_dx += pStrBlock->special_char_w;
         }
         return TRUE;
      }
      break;

   case SB_SUPSUB_LEFT:
   case SB_SUPSUB_CENTER:
   case SB_SUPSUB_RIGHT:
      dx = dy = 0;
      if (pStrBlock->sup != NULL) {
         switch (pStrBlock->type) {
         case SB_SUPSUB_LEFT: dx += (-pStrBlock->w); break;
         case SB_SUPSUB_CENTER: dx += (-((pStrBlock->w)>>1)); break;
         case SB_SUPSUB_RIGHT: break;
         }
         dy += pStrBlock->sup->baseline_offset;
         if (pStrBlock->type == SB_SUPSUB_CENTER) {
            dy -= pStrBlock->seg->asc;
         }
         if (GetTextXYInMiniLines(pStrBlock->sup, pStrBlockToMatch,
               index_to_match, pn_dx, pn_dy)) {
            *pn_dx -= dx;
            *pn_dy += dy;
            return TRUE;
         }
      }
      dx = dy = 0;
      if (pStrBlock->sub != NULL) {
         switch (pStrBlock->type) {
         case SB_SUPSUB_LEFT: dx += (-pStrBlock->w); break;
         case SB_SUPSUB_CENTER: dx += (-((pStrBlock->w)>>1)); break;
         case SB_SUPSUB_RIGHT: break;
         }
         dy += pStrBlock->sub->baseline_offset;
         if (pStrBlock->type == SB_SUPSUB_CENTER) {
            dy += pStrBlock->seg->des;
         }
         if (GetTextXYInMiniLines(pStrBlock->sub, pStrBlockToMatch,
               index_to_match, pn_dx, pn_dy)) {
            *pn_dx -= dx;
            *pn_dy += dy;
            return TRUE;
         }
      }
      dx = dy = 0;
      if (pStrBlock->type == SB_SUPSUB_CENTER &&
            pStrBlockToMatch == pStrBlock) {
         dx -= ((pStrBlock->w-pStrBlock->seg->w)>>1);
         if (GetTextXYInStrSeg(pStrBlock->seg, index_to_match, pn_dx)) {
            *pn_dx -= dx;
            *pn_dy += dy;
            return TRUE;
         }
         break;
      }
      break;
   }
   return FALSE;
}

static
int GetTextXYInMiniLine(pMiniLine, pStrBlockToMatch, index_to_match,
      pn_dx, pn_dy)
   MiniLineInfo *pMiniLine;
   StrBlockInfo *pStrBlockToMatch;
   int index_to_match, *pn_dx, *pn_dy;
   /*
    * *pn_x and *pn_y are the amount moved by this routine
    */
{
   StrBlockInfo *pStrBlock=NULL;

   for (pStrBlock=pMiniLine->first_block; pStrBlock != NULL;
         pStrBlock=pStrBlock->next) {
      int w=pStrBlock->w;

      if (GetTextXYInStrBlock(pStrBlock, pStrBlockToMatch, index_to_match,
            pn_dx, pn_dy)) {
         return TRUE;
      }
      *pn_dx += w;
   }
   return FALSE;
}

int GetTextXYInMiniLines(minilines, pStrBlockToMatch, index_to_match,
      pn_dx, pn_dy)
   MiniLinesInfo *minilines;
   StrBlockInfo *pStrBlockToMatch;
   int index_to_match, *pn_dx, *pn_dy;
   /*
    * *pn_x and *pn_y are the amount moved by this routine
    */
{
   MiniLineInfo *pMiniLine=NULL;
   int v_space=minilines->v_space;
   int dx=0, dy=0;

   for (pMiniLine=minilines->first; pMiniLine != NULL;
         pMiniLine=pMiniLine->next) {
      int saved_dx=(*pn_dx), saved_dy=(*pn_dy);

#ifdef _TGIF_DBG /* debug, do not translate */
      if (pMiniLine == minilines->first) {
         TgAssert(pMiniLine->v_gap == 0,
               "First mini_line has non-zero v_gap in GetTextXYInMiniLines()",
               NULL);
      }
#endif /* _TGIF_DBG */
      dy += pMiniLine->v_gap;

      switch (minilines->just) {
      case JUST_L: break;
      case JUST_C: dx += ((pMiniLine->w)>>1); break;
      case JUST_R: dx += pMiniLine->w; break;
      }
      if (GetTextXYInMiniLine(pMiniLine, pStrBlockToMatch, index_to_match,
            pn_dx, pn_dy)) {
         *pn_dx -= dx;
         *pn_dy += dy;
         return TRUE;
      }
      *pn_dx = saved_dx;
      *pn_dy = saved_dy;

      switch (minilines->just) {
      case JUST_L: break;
      case JUST_C: dx -= ((pMiniLine->w)>>1); break;
      case JUST_R: dx -= pMiniLine->w; break;
      }
      dy += pMiniLine->des + v_space;
      if (pMiniLine->next != NULL) {
         dy += pMiniLine->next->asc;
      }
   }
   return FALSE;
}

void GetTextXY(pStrBlock, nIndex, pn_x, pn_baseline_y)
   StrBlockInfo *pStrBlock;
   int nIndex, *pn_x, *pn_baseline_y;
{
   int dx=0, dy=0;
   struct TextRec *text_ptr=curTextObj->detail.t;

   PushCurFont();
   GetTextXYInMiniLines(&text_ptr->minilines, pStrBlock, nIndex,
         &dx, &dy);
   PopCurFont();

   if (pn_x != NULL) {
      *pn_x = textOrigX + dx;
   }
   if (pn_baseline_y != NULL) {
      *pn_baseline_y = textOrigBaselineY + dy;
   }
}

/* --------------------- SetTextCurXY() --------------------- */

void SetTextCurXY()
   /* set textCurX & textCurBaselineY according to curStrBlock & textCurIndex */
{
   GetTextXY(curStrBlock, textCurIndex, &textCurX, &textCurBaselineY);
}

/* --------------------- SetTextEndXY() --------------------- */

void SetTextEndXY()
   /* set textEndX & textEndBaselineY according to endStrBlock & textEndIndex */
{
   GetTextXY(endStrBlock, textEndIndex, &textEndX, &textEndBaselineY);
}

/* --------------------- CurStrBlockInMiniLines() --------------------- */

int CurStrBlockInStrBlock(pStrBlock)
   StrBlockInfo *pStrBlock;
{
   switch (pStrBlock->type) {
   case SB_SIMPLE:
      if (curStrBlock == pStrBlock) {
         return TRUE;
      }
      break;

   case SB_CHAR_SPACE:
      if (curStrBlock == pStrBlock) {
         for (pStrBlock=pStrBlock->prev; pStrBlock != NULL;
               pStrBlock=pStrBlock->prev) {
            if (pStrBlock->type == SB_SIMPLE) {
               curStrBlock = pStrBlock;
               textCurIndex = pStrBlock->seg->dyn_str.sz-1;
#ifdef _TGIF_DBG /* debug, do not translate */
         TgAssert(FALSE,
               "curStrBlock->type is SB_CHAR_SPACE in CurStrBlockInStrBlock()",
               "Fixed by returning to end of the previous SB_SIMPLE block");
#endif /* _TGIF_DBG */
               return TRUE;
            }
         }
#ifdef _TGIF_DBG /* debug, do not translate */
         TgAssert(FALSE,
               "curStrBlock->type is SB_CHAR_SPACE in CurStrBlockInStrBlock()",
               "Don't know how to fix this one");
#endif /* _TGIF_DBG */
         return FALSE;
      }
      break;

   case SB_SUPSUB_LEFT:
   case SB_SUPSUB_CENTER:
   case SB_SUPSUB_RIGHT:
      if (pStrBlock->sup != NULL) {
         if (CurStrBlockInMiniLines(pStrBlock->sup)) {
            return TRUE;
         }
      }
      if (pStrBlock->sub != NULL) {
         if (CurStrBlockInMiniLines(pStrBlock->sub)) {
            return TRUE;
         }
      }
      if (pStrBlock->type == SB_SUPSUB_CENTER &&
            curStrBlock == pStrBlock) {
         return TRUE;
      }
      break;
   }
   return FALSE;
}

int CurStrBlockInMiniLine(pMiniLine)
   MiniLineInfo *pMiniLine;
{
   StrBlockInfo *pStrBlock=NULL;

   for (pStrBlock=pMiniLine->first_block; pStrBlock != NULL;
         pStrBlock=pStrBlock->next) {
      if (CurStrBlockInStrBlock(pStrBlock)) {
         return TRUE;
      }
   }
   return FALSE;
}

int CurStrBlockInMiniLines(minilines)
   MiniLinesInfo *minilines;
{
   MiniLineInfo *pMiniLine=NULL;

   for (pMiniLine=minilines->first; pMiniLine != NULL;
         pMiniLine=pMiniLine->next) {
      if (CurStrBlockInMiniLine(pMiniLine)) {
         return TRUE;
      }
   }
   return FALSE;
}

/* --------------------- GetCursorPositionInMiniLines() --------------------- */

static
void GetCursorPositionInStrSeg(pStrSeg, dx, pn_dx, ppStrBlock, pnIndex)
   StrSegInfo *pStrSeg;
   int dx, *pn_dx, *pnIndex;
   StrBlockInfo **ppStrBlock;
   /*
    * dx is relative to the left side of the string segment
    * dy is relative to the baseline of the string segment
    * *pn_x is the amount moved by this routine
    */
{
   char *psz=NULL;
   int inc=0, index=0;

   curFont = pStrSeg->font;
   curSzUnit = pStrSeg->sz_unit;
   curStyle = pStrSeg->style;
   SetCanvasFont();
   inc = (canvasFontDoubleByte ? 2 : 1);

   for (psz=pStrSeg->dyn_str.s; *psz != '\0'; psz=(&psz[inc]), index+=inc) {
      int w=MyTextWidth(canvasFontPtr, psz, inc);

      if (dx < (w>>1)) {
         break;
      }
      dx -= w;
      *pn_dx += w;
   }
   *pnIndex = index;
}

static
void GetCursorPositionInStrBlock(pStrBlock, dx, dy, pn_dx, pn_dy, ppStrBlock,
      pnIndex)
   StrBlockInfo *pStrBlock;
   int dx, dy, *pn_dx, *pn_dy, *pnIndex;
   StrBlockInfo **ppStrBlock;
   /*
    * dx is relative to the left side of the string block
    * dy is relative to the baseline of the string block
    * *pn_x and *pn_y are the amount moved by this routine
    */
{
   int which=USE_SEG, saved_dx=dx, saved_dy=dy;

   switch (pStrBlock->type) {
   case SB_SIMPLE:
      GetCursorPositionInStrSeg(pStrBlock->seg, dx, pn_dx, ppStrBlock, pnIndex);
      *ppStrBlock = pStrBlock;
      break;

   case SB_CHAR_SPACE:
#ifdef _TGIF_DBG /* debug, do not translate */
      TgAssert(FALSE,
            "StrBlock has type SB_CHAR_SPACE in GetCursorPositionInStrBlock()",
            NULL);
#endif /* _TGIF_DBG */
      *ppStrBlock = pStrBlock;
      *pn_dx += pStrBlock->special_char_w;
      break;

   case SB_SUPSUB_LEFT:
   case SB_SUPSUB_CENTER:
   case SB_SUPSUB_RIGHT:
      if (pStrBlock->type == SB_SUPSUB_CENTER) {
         which = USE_SUB;
         if (pStrBlock->sup != NULL) {
            if (dy < (-pStrBlock->seg->asc)+pStrBlock->sup->baseline_offset-
                  pStrBlock->sup->first->asc+pStrBlock->sup->h) {
               which = USE_SUP;
            }
         }
         if (which == USE_SUB) {
            if (dy < pStrBlock->seg->des) {
               which = USE_SEG;
            }
         }
      } else {
         if (pStrBlock->sup != NULL && pStrBlock->sub != NULL) {
            int y=0, half_h=0;

            y = dy+pStrBlock->asc;
            half_h = ((pStrBlock->asc+pStrBlock->des)>>1);
            if (y < half_h) {
               which = USE_SUP;
            } else {
               which = USE_SUB;
            }
         } else if (pStrBlock->sup != NULL) {
            which = USE_SUP;
         } else {
            which = USE_SUB;
         }
      }
      switch (pStrBlock->type) {
      case SB_SUPSUB_LEFT: dx += (-pStrBlock->w); break;
      case SB_SUPSUB_CENTER: dx += (-((pStrBlock->w)>>1)); break;
      case SB_SUPSUB_RIGHT: break;
      }

      switch (which) {
      case USE_SUP:
         dy += pStrBlock->sup->baseline_offset;
         if (pStrBlock->type == SB_SUPSUB_CENTER) {
            dy -= pStrBlock->seg->asc;
         }
         GetCursorPositionInMiniLines(pStrBlock->sup, dx, dy, pn_dx, pn_dy,
               ppStrBlock, pnIndex);
         break;
      case USE_SUB:
         dy += pStrBlock->sub->baseline_offset;
         if (pStrBlock->type == SB_SUPSUB_CENTER) {
            dy += pStrBlock->seg->des;
         }
         GetCursorPositionInMiniLines(pStrBlock->sub, dx, dy, pn_dx, pn_dy,
               ppStrBlock, pnIndex);
         break;
      case USE_SEG:
         dx += ((pStrBlock->seg->w)>>1);
         GetCursorPositionInStrSeg(pStrBlock->seg, dx, pn_dx, ppStrBlock,
               pnIndex);
         *ppStrBlock = pStrBlock;
         break;
      }
      *pn_dx += saved_dx - dx;
      *pn_dy += dy - saved_dy;
      break;
   }
}

static
void GetCursorPositionInMiniLine(pMiniLine, dx, dy, pn_dx, pn_dy, ppStrBlock,
      pnIndex)
   MiniLineInfo *pMiniLine;
   int dx, dy, *pn_dx, *pn_dy, *pnIndex;
   StrBlockInfo **ppStrBlock;
   /*
    * dx is relative to the left side of the miniline
    * dy is relative to the baseline of the miniline
    * *pn_x and *pn_y are the amount moved by this routine
    */
{
   StrBlockInfo *pStrBlock=NULL;

   for (pStrBlock=pMiniLine->first_block; pStrBlock != NULL;
         pStrBlock=pStrBlock->next) {
      int w=pStrBlock->w;

      if (pStrBlock->type != SB_CHAR_SPACE &&
            (pStrBlock->next == NULL || dx < w)) {
         GetCursorPositionInStrBlock(pStrBlock, dx, dy, pn_dx, pn_dy,
               ppStrBlock, pnIndex);
         return;
      }
      dx -= w;
      *pn_dx += w;
   }
}

int GetCursorPositionInMiniLines(minilines, dx, dy, pn_dx, pn_dy, ppStrBlock,
      pnIndex)
   MiniLinesInfo *minilines;
   int dx, dy, *pn_dx, *pn_dy, *pnIndex;
   StrBlockInfo **ppStrBlock;
   /*
    * returns 0 if *pn_dx and *pn_dy are set
    * returns 1 if the caller needs to figure out where the end is
    * dx is relative to the origin of the miniline
    * dy is relative to the baseline of the miniline
    * *pn_x and *pn_y are the amount moved by this routine
    */
{
   MiniLineInfo *pMiniLine=NULL;
   int v_space=minilines->v_space;
   int saved_dx=dx, saved_dy=dy;

   for (pMiniLine=minilines->first; pMiniLine != NULL;
         pMiniLine=pMiniLine->next) {
#ifdef _TGIF_DBG /* debug, do not translate */
      if (pMiniLine == minilines->first) {
         TgAssert(pMiniLine->v_gap == 0,
         "First mini_line has non-zero v_gap in GetCursorPositionInMiniLines()",
               NULL);
      }
#endif /* _TGIF_DBG */
      dy -= pMiniLine->v_gap;
      if ((pMiniLine->next == NULL && minilines->owner_block != NULL) ||
            dy < pMiniLine->des+minilines->v_space) {
         switch (minilines->just) {
         case JUST_L: break;
         case JUST_C: dx += ((pMiniLine->w)>>1); break;
         case JUST_R: dx += pMiniLine->w; break;
         }
         GetCursorPositionInMiniLine(pMiniLine, dx, dy,
               pn_dx, pn_dy, ppStrBlock, pnIndex);
         *pn_dx += saved_dx - dx;
         *pn_dy += saved_dy - dy;
         return 0;
      }
      dy -= pMiniLine->des + v_space;
      if (pMiniLine->next != NULL) {
         dy -= pMiniLine->next->asc;
      }
   }
#ifdef _TGIF_DBG /* debug, do not translate */
   TgAssert(minilines->owner_block == NULL,
         "Invalid position reached in GetCursorPositionInMiniLines()",
         NULL);
#endif /* _TGIF_DBG */
   return 1;
}

/* --------------------- SameProperty() --------------------- */

int SameProperty(lWhich, nValue, pStrSeg, nCheckDoubleByte)
   long lWhich;
   int nValue, nCheckDoubleByte;
   StrSegInfo *pStrSeg;
   /*
    * If lWhich is PROP_MASK_TEXT_FONT and nCheckDoubleByte is TRUE, if
    *       pStrSeg->double_byte != IsFontDoubleByte(nValue), this function
    *       should return TRUE because the font cannot be changed.
    */
{
   switch (lWhich) {
   case PROP_MASK_COLOR: return (pStrSeg->color == nValue);
   case PROP_MASK_TEXT_FONT:
      if (pStrSeg->font == nValue) return TRUE;
      return (nCheckDoubleByte && (pStrSeg->double_byte !=
            IsFontDoubleByte(nValue)));
   case PROP_MASK_TEXT_STYLE: return (pStrSeg->style == nValue);
   case PROP_MASK_TEXT_SZ_UNIT: return (pStrSeg->sz_unit == nValue);
   case PROP_MASK_UNDERLINE_ON: return (pStrSeg->underline_on == nValue);
   case PROP_MASK_OVERLINE_ON: return (pStrSeg->overline_on == nValue);
   }
   return FALSE;
}

/* --------------------- SetMiniLinesProperty() --------------------- */

int SetStrSegProperty(lWhich, nValue, pStrSeg)
   long lWhich;
   int nValue;
   StrSegInfo *pStrSeg;
{
   int changed=FALSE, got_requested_size=TRUE;

   PushCurFont();

   switch (lWhich) {
   case PROP_MASK_COLOR:
      if (pStrSeg->color != nValue) {
         pStrSeg->color = nValue;
         UtilStrCpyN(pStrSeg->color_str, sizeof(pStrSeg->color_str),
               colorMenuItems[pStrSeg->color]);
         changed = TRUE;
      }
      break;
   case PROP_MASK_TEXT_FONT:
      if (pStrSeg->font != nValue &&
            pStrSeg->double_byte == IsFontDoubleByte(nValue)) {
         if (TrySetCanvasFont(nValue, pStrSeg->style,
               SzUnitToFontSize(pStrSeg->sz_unit), FALSE,
               &got_requested_size)) {
            pStrSeg->font = nValue;
            if (pStrSeg->double_byte) {
               pStrSeg->double_byte_vertical = canvasFontDoubleByteVertical;
               pStrSeg->double_byte_mod_bytes = canvasFontDoubleByteModBytes;
            } else {
               pStrSeg->double_byte_vertical = FALSE;
               pStrSeg->double_byte_mod_bytes = FALSE;
            }
            if (!got_requested_size) {
               pStrSeg->sz_unit = FontSizeToSzUnit(canvasFontSize);
            }
            changed = TRUE;
         }
      }
      break;
   case PROP_MASK_TEXT_STYLE:
      if (pStrSeg->style != nValue) {
         if (TrySetCanvasFont(pStrSeg->font, nValue,
               SzUnitToFontSize(pStrSeg->sz_unit), FALSE,
               &got_requested_size)) {
            pStrSeg->style = nValue;
            if (!got_requested_size) {
               pStrSeg->sz_unit = FontSizeToSzUnit(canvasFontSize);
            }
            changed = TRUE;
         }
      }
      break;
   case PROP_MASK_TEXT_SZ_UNIT:
      if (pStrSeg->sz_unit != nValue) {
         if (TrySetCanvasFont(pStrSeg->font, pStrSeg->style,
               SzUnitToFontSize(nValue), TRUE, &got_requested_size) &&
               got_requested_size) {
            pStrSeg->sz_unit = nValue;
            changed = TRUE;
         }
      }
      break;
   case PROP_MASK_UNDERLINE_ON:
      if (pStrSeg->underline_on != nValue) {
         pStrSeg->underline_on = nValue;
         changed = TRUE;
      }
      break;
   case PROP_MASK_OVERLINE_ON:
      if (pStrSeg->overline_on != nValue) {
         pStrSeg->overline_on = nValue;
         changed = TRUE;
      }
      break;
   }
   PopCurFont();

   return changed;
}

static
int SetStrBlockProperty(lWhich, nValue, pStrBlock)
   long lWhich;
   int nValue;
   StrBlockInfo *pStrBlock;
{
   int changed=FALSE;

   switch (pStrBlock->type) {
   case SB_SIMPLE:
      changed = SetStrSegProperty(lWhich, nValue, pStrBlock->seg);
      break;

   case SB_CHAR_SPACE:
      break;

   case SB_SUPSUB_LEFT:
   case SB_SUPSUB_CENTER:
   case SB_SUPSUB_RIGHT:
      if (pStrBlock->sup != NULL) {
         if (SetMiniLinesProperty(lWhich, nValue, pStrBlock->sup)) {
            changed = TRUE;
         }
      }
      if (pStrBlock->sub != NULL) {
         if (SetMiniLinesProperty(lWhich, nValue, pStrBlock->sub)) {
            changed = TRUE;
         }
      }
      if (pStrBlock->type == SB_SUPSUB_CENTER) {
         if (SetStrSegProperty(lWhich, nValue, pStrBlock->seg)) {
            changed = TRUE;
         }
      }
      break;
   }
   return changed;
}

static
int SetMiniLineProperty(lWhich, nValue, pMiniLine)
   long lWhich;
   int nValue;
   MiniLineInfo *pMiniLine;
{
   StrBlockInfo *pStrBlock=NULL;
   int changed=FALSE;

   for (pStrBlock=pMiniLine->first_block; pStrBlock != NULL;
         pStrBlock=pStrBlock->next) {
      if (SetStrBlockProperty(lWhich, nValue, pStrBlock)) {
         changed = TRUE;
      }
   }
   return changed;
}

int SetMiniLinesProperty(lWhich, nValue, minilines)
   long lWhich;
   int nValue;
   MiniLinesInfo *minilines;
{
   MiniLineInfo *pMiniLine=NULL;
   int changed=FALSE;

   for (pMiniLine=minilines->first; pMiniLine != NULL;
         pMiniLine=pMiniLine->next) {
      if (SetMiniLineProperty(lWhich, nValue, pMiniLine)) {
         changed = TRUE;
      }
   }
   return changed;
}

/* -------------------- ChangeHighlightedTextProperty() -------------------- */

static
int ChangeStrSegPropertyForTail(pStrBlock, first_index, lWhich, nValue)
   StrBlockInfo *pStrBlock;
   int first_index, nValue;
   long lWhich;
   /* return TRUE anything is changed */
{
   StrSegInfo *pStrSeg=pStrBlock->seg;
   StrBlockInfo *pNewStrBlock=NULL;
   MiniLineInfo *owner_mini_line=pStrBlock->owner_mini_line;
   int changed=FALSE;

   DupStrBlock(pStrBlock, owner_mini_line, &pNewStrBlock,
         &pNewStrBlock);
   DynStrSet(&pNewStrBlock->seg->dyn_str,
         &pStrSeg->dyn_str.s[first_index]);
   pStrSeg->dyn_str.s[first_index] = '\0';
   DynStrSet(&pStrSeg->dyn_str, pStrSeg->dyn_str.s);
   if (!SetStrSegProperty(lWhich, nValue, pNewStrBlock->seg)) {
      /*
       * don't call UnlinkStrBlock() because DupStrBlock() does not
       *       insert pNewStrBlock into owner_mini_line
       */
      FreeStrBlock(pNewStrBlock);
   } else {
      /* insert after */
      pNewStrBlock->next = pStrBlock->next;
      if (pStrBlock->next == NULL) {
         owner_mini_line->last_block = pNewStrBlock;
      } else {
         pStrBlock->next->prev = pNewStrBlock;
      }
      pNewStrBlock->prev = pStrBlock;
      pStrBlock->next = pNewStrBlock;
      changed = TRUE;
   }
   if (changed) {
      if (pStrBlock == gstTextHighlightInfo.start_str_block_ptr) {
         if (curStrBlock == endStrBlock) {
            if (textCurIndex == first_index) {
               curStrBlock = endStrBlock = pNewStrBlock;
               textCurIndex = 0;
               textEndIndex = endStrBlock->seg->dyn_str.sz-1;
            } else {
               endStrBlock = curStrBlock = pNewStrBlock;
               textEndIndex = 0;
               textCurIndex = endStrBlock->seg->dyn_str.sz-1;
            }
         } else if (pStrBlock == curStrBlock) {
            curStrBlock = pNewStrBlock;
            textCurIndex = 0;
         } else {
            endStrBlock = pNewStrBlock;
            textEndIndex = 0;
         }
      }
   }
   return changed;
}

static
int ChangeStrSegPropertyForHead(pStrBlock, first_index, lWhich, nValue)
   StrBlockInfo *pStrBlock;
   int first_index, nValue;
   long lWhich;
   /* return TRUE anything is changed */
{
   StrSegInfo *pStrSeg=pStrBlock->seg;
   StrBlockInfo *pNewStrBlock=NULL;
   MiniLineInfo *owner_mini_line=pStrBlock->owner_mini_line;
   char saved_ch=pStrSeg->dyn_str.s[first_index];
   int changed=FALSE;

   pStrSeg->dyn_str.s[first_index] = '\0';
   DupStrBlock(pStrBlock, owner_mini_line, &pNewStrBlock,
         &pNewStrBlock);
   DynStrSet(&pNewStrBlock->seg->dyn_str, pStrSeg->dyn_str.s);
   pStrSeg->dyn_str.s[first_index] = saved_ch;
   DynStrSet(&pStrSeg->dyn_str, &pStrSeg->dyn_str.s[first_index]);

   if (!SetStrSegProperty(lWhich, nValue, pNewStrBlock->seg)) {
      /*
       * don't call UnlinkStrBlock() because DupStrBlock() does not
       *       insert pNewStrBlock into owner_mini_line
       */
      FreeStrBlock(pNewStrBlock);
   } else {
      /* insert before */
      pNewStrBlock->prev = pStrBlock->prev;
      if (pStrBlock->prev == NULL) {
         owner_mini_line->first_block = pNewStrBlock;
      } else {
         pStrBlock->prev->next = pNewStrBlock;
      }
      pNewStrBlock->next = pStrBlock;
      pStrBlock->prev = pNewStrBlock;
      changed = TRUE;
   }
   if (changed) {
      if (pStrBlock == gstTextHighlightInfo.end_str_block_ptr) {
         if (curStrBlock == endStrBlock) {
            if (textCurIndex == first_index) {
               curStrBlock = endStrBlock = pNewStrBlock;
               textCurIndex = 0;
               textEndIndex = endStrBlock->seg->dyn_str.sz-1;
            } else {
               endStrBlock = curStrBlock = pNewStrBlock;
               textEndIndex = 0;
               textCurIndex = endStrBlock->seg->dyn_str.sz-1;
            }
         } else if (pStrBlock == curStrBlock) {
            textCurIndex = 0;
         } else {
            textEndIndex = 0;
         }
      }
   }
   return changed;
}

static
int ChangeStrSegPropertyForMiddle(pStrBlock, first_index, second_index, lWhich,
      nValue)
   StrBlockInfo *pStrBlock;
   int first_index, second_index, nValue;
   long lWhich;
   /* return TRUE anything is changed */
{
   StrSegInfo *pStrSeg=pStrBlock->seg;
   StrBlockInfo *pNewStrBlock=NULL;
   MiniLineInfo *owner_mini_line=pStrBlock->owner_mini_line;
   int changed=FALSE;

   DupStrBlock(pStrBlock, owner_mini_line, &pNewStrBlock,
         &pNewStrBlock);
   if (!SetStrSegProperty(lWhich, nValue, pNewStrBlock->seg)) {
      /*
       * don't call UnlinkStrBlock() because DupStrBlock() does not
       *       insert pNewStrBlock into owner_mini_line
       */
      FreeStrBlock(pNewStrBlock);
   } else {
      /* split */
      StrBlockInfo *pLeftStrBlock=NULL;
      char *psz=NULL, *psz1=NULL, *psz2=NULL, saved_ch='\0';

      DupStrBlock(pStrBlock, owner_mini_line, &pLeftStrBlock,
            &pLeftStrBlock);
      if ((psz=UtilStrDup(pStrSeg->dyn_str.s)) == NULL) {
         FailAllocMessage();
      }
      saved_ch = psz[first_index];
      psz[first_index] = '\0';
      DynStrSet(&pLeftStrBlock->seg->dyn_str, psz);
      psz[first_index] = saved_ch;
      DynStrSet(&pStrBlock->seg->dyn_str, &psz[second_index]);
      psz[second_index] = '\0';

      for (psz1=psz, psz2=(&psz[first_index]); *psz2 != '\0'; ) {
         *psz1++ = *psz2++;
      }
      *psz1 = '\0';
      DynStrSet(&pNewStrBlock->seg->dyn_str, psz);
      UtilFree(psz);

      pLeftStrBlock->prev = pStrBlock->prev;
      if (pStrBlock->prev == NULL) {
         owner_mini_line->first_block = pLeftStrBlock;
      } else {
         pStrBlock->prev->next = pLeftStrBlock;
      }
      pLeftStrBlock->next = pNewStrBlock;
      pNewStrBlock->prev = pLeftStrBlock;

      pNewStrBlock->next = pStrBlock;
      pStrBlock->prev = pNewStrBlock;

      changed = TRUE;
   }
   if (changed) {
      if (pStrBlock == gstTextHighlightInfo.end_str_block_ptr) {
         if (curStrBlock == endStrBlock) {
            if (textCurIndex == first_index) {
               curStrBlock = endStrBlock = pNewStrBlock;
               textCurIndex = 0;
               textEndIndex = endStrBlock->seg->dyn_str.sz-1;
            } else {
               endStrBlock = curStrBlock = pNewStrBlock;
               textEndIndex = 0;
               textCurIndex = endStrBlock->seg->dyn_str.sz-1;
            }
         }
      }
   }
   return changed;
}

static
int ChangePropertyForHighlightedTextInStrSeg(pStrBlock, mode, first_index,
      second_index, lWhich, nValue)
   StrBlockInfo *pStrBlock;
   int mode, first_index, second_index, nValue;
   long lWhich;
   /* return TRUE anything is changed */
{
   StrSegInfo *pStrSeg=pStrBlock->seg;
   int changed=FALSE;

   switch (mode) {
   case PAINT_NORM: break;

   case PAINT_INV:
      /* can only get here if pStrBlock->type is SB_SIMPLE */
      changed = SetStrSegProperty(lWhich, nValue, pStrSeg);
      break;

   case PAINT_NORM_INV:
      /* change the tail */
      if (SameProperty(lWhich, nValue, pStrSeg, TRUE)) {
         /* nothing need to change */
      } else if (first_index == pStrSeg->dyn_str.sz-1) {
         /* nothing need to change */
      } else {
         changed = ChangeStrSegPropertyForTail(pStrBlock, first_index, lWhich,
               nValue);
      }
      gstTextHighlightInfo.mode = PAINT_INV;
      break;

   case PAINT_INV_NORM:
      /* change the head */
      if (SameProperty(lWhich, nValue, pStrSeg, TRUE)) {
         /* nothing need to change */
      } else if (first_index == 0) {
         /* nothing need to change */
      } else {
         changed = ChangeStrSegPropertyForHead(pStrBlock, first_index, lWhich,
               nValue);
      }
      gstTextHighlightInfo.mode = PAINT_NORM;
      break;

   case PAINT_NORM_INV_NORM:
      if (SameProperty(lWhich, nValue, pStrSeg, TRUE)) {
         /* nothing need to change */
      } else if (first_index == 0 && second_index == pStrSeg->dyn_str.sz-1) {
         changed = SetStrSegProperty(lWhich, nValue, pStrSeg);
      } else if (first_index == 0) {
         /* just change the head */
         changed = ChangeStrSegPropertyForHead(pStrBlock, second_index, lWhich,
               nValue);
      } else if (second_index == pStrSeg->dyn_str.sz-1) {
         /* just change the tail */
         changed = ChangeStrSegPropertyForTail(pStrBlock, first_index, lWhich,
               nValue);
      } else {
         /* change the middle */
         changed = ChangeStrSegPropertyForMiddle(pStrBlock, first_index,
               second_index, lWhich, nValue);
      }
      gstTextHighlightInfo.mode = PAINT_NORM;
      break;
   }
   return changed;
}

int ChangeHighlightedTextProperty(lWhich, nValue)
   long lWhich;
   int nValue;
   /* returns TRUE if anything is changed */
{
   MiniLineInfo *pOwnerMiniLine=NULL;
   StrBlockInfo *pStrBlock=NULL, *pNextStrBlock=NULL;
   int first_index=0, second_index=0, mode=PAINT_NORM;
   int changed=FALSE;

   /* setup gstTextHighlightInfo */
   if (!UpdateTextHighlightInfo()) return FALSE;

   gstTextHighlightInfo.highlighting = FALSE;
   gstTextHighlightInfo.mode = PAINT_NORM;

   pStrBlock = gstTextHighlightInfo.start_str_block_ptr;
   pOwnerMiniLine = pStrBlock->owner_mini_line;

   GetPaintMode(pStrBlock, &mode, &first_index, &second_index);
   if (ChangePropertyForHighlightedTextInStrSeg(pStrBlock, mode, first_index,
         second_index, lWhich, nValue)) {
      changed = TRUE;
   }
   mode = gstTextHighlightInfo.mode;
   pStrBlock = pStrBlock->next;
   while (mode != PAINT_NORM) {
      if (pStrBlock == NULL) {
         pOwnerMiniLine = pOwnerMiniLine->next;
         if (pOwnerMiniLine == NULL) {
            break;
         }
         pStrBlock = pOwnerMiniLine->first_block;
         continue;
      }
      pNextStrBlock = pStrBlock->next;
      GetPaintMode(pStrBlock, &mode, &first_index, &second_index);
      if (mode == PAINT_INV) {
         if (SetStrBlockProperty(lWhich, nValue, pStrBlock)) {
            changed = TRUE;
         }
      } else {
         /* pStrBlock->type better be SB_SIMPLE */
#ifdef _TGIF_DBG /* debug, do not translate */
         TgAssert(pStrBlock->type == SB_SIMPLE,
            "pStrBlock->type != SB_SIMPLE in ChangeHighlightedTextProperty()",
            NULL);
#endif /* _TGIF_DBG */
         if (ChangePropertyForHighlightedTextInStrSeg(pStrBlock, mode,
               first_index, second_index, lWhich, nValue)) {
            changed = TRUE;
         }
      }
      mode = gstTextHighlightInfo.mode;
      pStrBlock = pNextStrBlock;
   }
   return changed;
}

/* --------------------- OnePropertyMiniLines() --------------------- */

static
int OnePropertyStrBlock(lWhich, nValue, pStrBlock, nCheckDoubleByte)
   long lWhich;
   int nValue, nCheckDoubleByte;
   StrBlockInfo *pStrBlock;
{
   switch (pStrBlock->type) {
   case SB_SIMPLE:
      return SameProperty(lWhich, nValue, pStrBlock->seg, nCheckDoubleByte);

   case SB_CHAR_SPACE:
      break;

   case SB_SUPSUB_LEFT:
   case SB_SUPSUB_CENTER:
   case SB_SUPSUB_RIGHT:
      if (pStrBlock->sup != NULL) {
         if (!OnePropertyMiniLines(lWhich, nValue, pStrBlock->sup,
               nCheckDoubleByte)) {
            return FALSE;
         }
      }
      if (pStrBlock->sub != NULL) {
         if (!OnePropertyMiniLines(lWhich, nValue, pStrBlock->sub,
               nCheckDoubleByte)) {
            return FALSE;
         }
      }
      if (pStrBlock->type == SB_SUPSUB_CENTER) {
         if (!SameProperty(lWhich, nValue, pStrBlock->seg, nCheckDoubleByte)) {
            return FALSE;
         }
      }
      break;
   }
   return TRUE;
}

static
int OnePropertyMiniLine(lWhich, nValue, pMiniLine, nCheckDoubleByte)
   long lWhich;
   int nValue, nCheckDoubleByte;
   MiniLineInfo *pMiniLine;
{
   StrBlockInfo *pStrBlock=NULL;

   for (pStrBlock=pMiniLine->first_block; pStrBlock != NULL;
         pStrBlock=pStrBlock->next) {
      if (!OnePropertyStrBlock(lWhich, nValue, pStrBlock, nCheckDoubleByte)) {
         return FALSE;
      }
   }
   return TRUE;
}

int OnePropertyMiniLines(lWhich, nValue, minilines, nCheckDoubleByte)
   long lWhich;
   int nValue, nCheckDoubleByte;
   MiniLinesInfo *minilines;
{
   MiniLineInfo *pMiniLine=NULL;

   for (pMiniLine=minilines->first; pMiniLine != NULL;
         pMiniLine=pMiniLine->next) {
      if (!OnePropertyMiniLine(lWhich, nValue, pMiniLine, nCheckDoubleByte)) {
         return FALSE;
      }
   }
   return TRUE;
}

/* --------------------- SingleFontText() --------------------- */

static
int SingleFontStrSeg(pStrSeg, pnSzUnit, pnDoubleByte, pnFont, pnStyle,
      pnUnderlineOn, pnOverlineOn)
   StrSegInfo *pStrSeg;
   int *pnSzUnit, *pnDoubleByte, *pnFont, *pnStyle, *pnUnderlineOn;
   int *pnOverlineOn;
{
   if (*pnDoubleByte == INVALID) {
      *pnSzUnit = pStrSeg->sz_unit;
      *pnDoubleByte = pStrSeg->double_byte;
      *pnFont = pStrSeg->font;
      *pnStyle = pStrSeg->style;
      *pnUnderlineOn = pStrSeg->underline_on;
      *pnOverlineOn = pStrSeg->overline_on;
      return TRUE;
   }
   return ((*pnSzUnit) == pStrSeg->sz_unit &&
         (*pnDoubleByte) == pStrSeg->double_byte &&
         (*pnFont) == pStrSeg->font &&
         (*pnStyle) == pStrSeg->style &&
         (*pnUnderlineOn) == pStrSeg->underline_on &&
         (*pnOverlineOn) == pStrSeg->overline_on);
}

static
int SingleFontStrBlock(pStrBlock, pnSzUnit, pnDoubleByte, pnFont, pnStyle,
      pnUnderlineOn, pnOverlineOn)
   StrBlockInfo *pStrBlock;
   int *pnSzUnit, *pnDoubleByte, *pnFont, *pnStyle, *pnUnderlineOn;
   int *pnOverlineOn;
{
   switch (pStrBlock->type) {
   case SB_SIMPLE:
      return SingleFontStrSeg(pStrBlock->seg, pnSzUnit, pnDoubleByte, pnFont,
            pnStyle, pnUnderlineOn, pnOverlineOn);
      break;
   case SB_CHAR_SPACE: return TRUE;

   case SB_SUPSUB_LEFT:
   case SB_SUPSUB_CENTER:
   case SB_SUPSUB_RIGHT:
      if (pStrBlock->sup != NULL && !SingleFontMiniLines(pStrBlock->sup,
            pnSzUnit, pnDoubleByte, pnFont, pnStyle, pnUnderlineOn,
            pnOverlineOn)) {
         return FALSE;
      }
      if (pStrBlock->sub != NULL && !SingleFontMiniLines(pStrBlock->sub,
            pnSzUnit, pnDoubleByte, pnFont, pnStyle, pnUnderlineOn,
            pnOverlineOn)) {
         return FALSE;
      }
      if (pStrBlock->type == SB_SUPSUB_CENTER) {
         return SingleFontStrSeg(pStrBlock->seg, pnSzUnit, pnDoubleByte,
               pnFont, pnStyle, pnUnderlineOn, pnOverlineOn);
      }
      break;
   }
   return TRUE;
}

static
int SingleFontMiniLine(pMiniLine, pnSzUnit, pnDoubleByte, pnFont, pnStyle,
      pnUnderlineOn, pnOverlineOn)
   MiniLineInfo *pMiniLine;
   int *pnSzUnit, *pnDoubleByte, *pnFont, *pnStyle, *pnUnderlineOn;
   int *pnOverlineOn;
{
   StrBlockInfo *pStrBlock=NULL;

   for (pStrBlock=pMiniLine->first_block; pStrBlock != NULL;
         pStrBlock=pStrBlock->next) {
      if (!SingleFontStrBlock(pStrBlock, pnSzUnit, pnDoubleByte, pnFont,
            pnStyle, pnUnderlineOn, pnOverlineOn)) {
         return FALSE;
      }
   }
   return TRUE;
}

int SingleFontMiniLines(minilines, pnSzUnit, pnDoubleByte, pnFont, pnStyle,
      pnUnderlineOn, pnOverlineOn)
   MiniLinesInfo *minilines;
   int *pnSzUnit, *pnDoubleByte, *pnFont, *pnStyle, *pnUnderlineOn;
   int *pnOverlineOn;
{  
   MiniLineInfo *pMiniLine=NULL;
   
   for (pMiniLine=minilines->first; pMiniLine != NULL;
         pMiniLine=pMiniLine->next) {
      if (!SingleFontMiniLine(pMiniLine, pnSzUnit, pnDoubleByte, pnFont,
            pnStyle, pnUnderlineOn, pnOverlineOn)) {
         return FALSE;
      }
   }
   return TRUE; 
}

int SingleFontText(text_ptr, pnSzUnit, pnDoubleByte, pnFont, pnStyle,
      pnUnderlineOn, pnOverlineOn)
   struct TextRec *text_ptr;
   int *pnSzUnit, *pnDoubleByte, *pnFont, *pnStyle, *pnUnderlineOn;
   int *pnOverlineOn;
{
   MiniLinesInfo *minilines=(&text_ptr->minilines);

   *pnDoubleByte = *pnFont = *pnStyle = INVALID;
   if (!SingleFontMiniLines(minilines, pnSzUnit, pnDoubleByte, pnFont,
         pnStyle, pnUnderlineOn, pnOverlineOn)) {
      return FALSE;
   }
   return ((*pnDoubleByte) != INVALID && (*pnFont) != INVALID &&
         (*pnStyle) != INVALID);
}

/* --------------------- SingleColorText() --------------------- */

static
int SingleColorStrSeg(pStrSeg, pnColorIndex)
   StrSegInfo *pStrSeg;
   int *pnColorIndex;
{
   if (*pnColorIndex == INVALID) {
      *pnColorIndex = pStrSeg->color;
      return TRUE;
   }
   return ((*pnColorIndex) == pStrSeg->color);
}

static
int SingleColorStrBlock(pStrBlock, pnColorIndex)
   StrBlockInfo *pStrBlock;
   int *pnColorIndex;
{
   switch (pStrBlock->type) {
   case SB_SIMPLE:
      return SingleColorStrSeg(pStrBlock->seg, pnColorIndex);
      break;
   case SB_CHAR_SPACE: return TRUE;

   case SB_SUPSUB_LEFT:
   case SB_SUPSUB_CENTER:
   case SB_SUPSUB_RIGHT:
      if (pStrBlock->sup != NULL && !SingleColorMiniLines(pStrBlock->sup,
            pnColorIndex)) {
         return FALSE;
      }
      if (pStrBlock->sub != NULL && !SingleColorMiniLines(pStrBlock->sub,
            pnColorIndex)) {
         return FALSE;
      }
      if (pStrBlock->type == SB_SUPSUB_CENTER) {
         return SingleColorStrSeg(pStrBlock->seg, pnColorIndex);
      }
      break;
   }
   return TRUE;
}

static
int SingleColorMiniLine(pMiniLine, pnColorIndex)
   MiniLineInfo *pMiniLine;
   int *pnColorIndex;
{
   StrBlockInfo *pStrBlock=NULL;

   for (pStrBlock=pMiniLine->first_block; pStrBlock != NULL;
         pStrBlock=pStrBlock->next) {
      if (!SingleColorStrBlock(pStrBlock, pnColorIndex)) {
         return FALSE;
      }
   }
   return TRUE;
}

int SingleColorMiniLines(minilines, pnColorIndex)
   MiniLinesInfo *minilines;
   int *pnColorIndex;
{  
   MiniLineInfo *pMiniLine=NULL;
   
   for (pMiniLine=minilines->first; pMiniLine != NULL;
         pMiniLine=pMiniLine->next) {
      if (!SingleColorMiniLine(pMiniLine, pnColorIndex)) {
         return FALSE;
      }
   }
   return TRUE; 
}

int SingleColorText(text_ptr, pnColorIndex)
   struct TextRec *text_ptr;
   int *pnColorIndex;
{
   MiniLinesInfo *minilines=(&text_ptr->minilines);

   *pnColorIndex = INVALID;
   if (!SingleColorMiniLines(minilines, pnColorIndex)) {
      return FALSE;
   }
   return ((*pnColorIndex) != INVALID);
}

/* -------------------- HighlightedTextHasSameProperty() -------------------- */

static
int HighlightedStrSegHasSameProperty(pStrBlock, mode, lWhich, nValue,
      nCheckDoubleByte)
   StrBlockInfo *pStrBlock;
   int mode, nValue, nCheckDoubleByte;
   long lWhich;
{
   StrSegInfo *pStrSeg=pStrBlock->seg;
   int rc=TRUE;

   if (mode == PAINT_NORM) return TRUE;

   rc = SameProperty(lWhich, nValue, pStrSeg, nCheckDoubleByte);

   switch (mode) {
   case PAINT_INV: break;
   case PAINT_NORM_INV: gstTextHighlightInfo.mode = PAINT_INV; break;
   case PAINT_INV_NORM: gstTextHighlightInfo.mode = PAINT_NORM; break;
   case PAINT_NORM_INV_NORM: gstTextHighlightInfo.mode = PAINT_NORM; break;
   }
   return rc;
}

int HighlightedTextHasSameProperty(lWhich, nValue, nCheckDoubleByte)
   long lWhich;
   int nValue, nCheckDoubleByte;
   /* returns TRUE if anything is changed */
{
   MiniLineInfo *pOwnerMiniLine=NULL;
   StrBlockInfo *pStrBlock=NULL, *pNextStrBlock=NULL;
   int first_index=0, second_index=0, mode=PAINT_NORM;

   /* setup gstTextHighlightInfo */
   if (!UpdateTextHighlightInfo()) return FALSE;

   gstTextHighlightInfo.highlighting = FALSE;
   gstTextHighlightInfo.mode = PAINT_NORM;

   pStrBlock = gstTextHighlightInfo.start_str_block_ptr;
   pOwnerMiniLine = pStrBlock->owner_mini_line;

   GetPaintMode(pStrBlock, &mode, &first_index, &second_index);
   if (!HighlightedStrSegHasSameProperty(pStrBlock, mode, lWhich, nValue,
         nCheckDoubleByte)) {
      return FALSE;
   }
   mode = gstTextHighlightInfo.mode;
   pStrBlock = pStrBlock->next;
   while (mode != PAINT_NORM) {
      if (pStrBlock == NULL) {
         pOwnerMiniLine = pOwnerMiniLine->next;
         if (pOwnerMiniLine == NULL) {
            break;
         }
         pStrBlock = pOwnerMiniLine->first_block;
         continue;
      }
      pNextStrBlock = pStrBlock->next;
      GetPaintMode(pStrBlock, &mode, &first_index, &second_index);
      if (mode == PAINT_INV) {
         if (!OnePropertyStrBlock(lWhich, nValue, pStrBlock,
               nCheckDoubleByte)) {
            return FALSE;
         }
      } else {
         /* pStrBlock->type better be SB_SIMPLE */
#ifdef _TGIF_DBG /* debug, do not translate */
         TgAssert(pStrBlock->type == SB_SIMPLE,
            "pStrBlock->type != SB_SIMPLE in HighlightedTextHasSameProperty()",
            NULL);
#endif /* _TGIF_DBG */
         if (!HighlightedStrSegHasSameProperty(pStrBlock, mode, lWhich,
               nValue, nCheckDoubleByte)) {
            return FALSE;
         }
      }
      mode = gstTextHighlightInfo.mode;
      pStrBlock = pNextStrBlock;
   }
   return TRUE;
}

/* --------------------- CheckMiniLinesProperties() --------------------- */

static
int CheckStrSegProperties(pStrSeg, pn_valid, pn_font, pn_style, pn_sz_unit,
      pn_color, pn_underline_on, pn_overline_on)
   StrSegInfo *pStrSeg;
   int *pn_valid, *pn_font, *pn_style, *pn_sz_unit, *pn_color, *pn_underline_on;
   int *pn_overline_on;
   /* return FALSE if any property is different */
{
   if (*pn_valid) {
      return (*pn_font == pStrSeg->font &&
            *pn_style == pStrSeg->style &&
            *pn_sz_unit == pStrSeg->sz_unit &&
            *pn_color == pStrSeg->color &&
            *pn_underline_on == pStrSeg->underline_on &&
            *pn_overline_on == pStrSeg->overline_on);
   } else {
      *pn_valid = TRUE;
      *pn_font = pStrSeg->font;
      *pn_style = pStrSeg->style;
      *pn_sz_unit = pStrSeg->sz_unit;
      *pn_color = pStrSeg->color;
      *pn_underline_on = pStrSeg->underline_on;
      *pn_overline_on = pStrSeg->overline_on;
   }
   return TRUE;
}

static
int CheckStrBlockProperties(pStrBlock, pn_valid, pn_font, pn_style,
      pn_sz_unit, pn_color, pn_underline_on, pn_overline_on)
   StrBlockInfo *pStrBlock;
   int *pn_valid, *pn_font, *pn_style, *pn_sz_unit, *pn_color, *pn_underline_on;
   int *pn_overline_on;
   /* return FALSE if any property is different */
{
   switch (pStrBlock->type) {
   case SB_SIMPLE:
      return CheckStrSegProperties(pStrBlock->seg, pn_valid, pn_font,
            pn_style, pn_sz_unit, pn_color, pn_underline_on, pn_overline_on);

   case SB_CHAR_SPACE:
      break;

   case SB_SUPSUB_LEFT:
   case SB_SUPSUB_CENTER:
   case SB_SUPSUB_RIGHT:
      if (pStrBlock->sup != NULL) {
         if (!CheckMiniLinesProperties(pStrBlock->sup, pn_valid, pn_font,
               pn_style, pn_sz_unit, pn_color, pn_underline_on,
               pn_overline_on)) {
            return FALSE;
         }
      }
      if (pStrBlock->sub != NULL) {
         if (!CheckMiniLinesProperties(pStrBlock->sub, pn_valid, pn_font,
               pn_style, pn_sz_unit, pn_color, pn_underline_on,
               pn_overline_on)) {
            return FALSE;
         }
      }
      if (pStrBlock->type == SB_SUPSUB_CENTER) {
         if (!CheckStrSegProperties(pStrBlock->seg, pn_valid, pn_font,
               pn_style, pn_sz_unit, pn_color, pn_underline_on,
               pn_overline_on)) {
            return FALSE;
         }
      }
      break;
   }
   return TRUE;
}

static
int CheckMiniLineProperties(pMiniLine, pn_valid, pn_font, pn_style,
      pn_sz_unit, pn_color, pn_underline_on, pn_overline_on)
   MiniLineInfo *pMiniLine;
   int *pn_valid, *pn_font, *pn_style, *pn_sz_unit, *pn_color, *pn_underline_on;
   int *pn_overline_on;
   /* return FALSE if any property is different */
{
   StrBlockInfo *pStrBlock=NULL;

   for (pStrBlock=pMiniLine->first_block; pStrBlock != NULL;
         pStrBlock=pStrBlock->next) {
      if (!CheckStrBlockProperties(pStrBlock, pn_valid, pn_font, pn_style,
            pn_sz_unit, pn_color, pn_underline_on, pn_overline_on)) {
         return FALSE;
      }
   }
   return TRUE;
}

int CheckMiniLinesProperties(minilines, pn_valid, pn_font, pn_style,
      pn_sz_unit, pn_color, pn_underline_on, pn_overline_on)
   MiniLinesInfo *minilines;
   int *pn_valid, *pn_font, *pn_style, *pn_sz_unit, *pn_color, *pn_underline_on;
   int *pn_overline_on;
   /* return FALSE if any property is different */
{
   MiniLineInfo *pMiniLine=NULL;

   for (pMiniLine=minilines->first; pMiniLine != NULL;
         pMiniLine=pMiniLine->next) {
      if (!CheckMiniLineProperties(pMiniLine, pn_valid, pn_font, pn_style,
            pn_sz_unit, pn_color, pn_underline_on, pn_overline_on)) {
         return FALSE;
      }
   }
   return TRUE;
}

/* ------------------- CanCopyHighLightedTextAsStrings() ------------------- */

static
int CheckHighlightedStrSegProperties(pStrBlock, mode, pn_valid, pn_font,
      pn_style, pn_sz_unit, pn_color, pn_underline_on, pn_overline_on)
   StrBlockInfo *pStrBlock;
   int mode, *pn_valid, *pn_font, *pn_style, *pn_sz_unit, *pn_color;
   int *pn_underline_on, *pn_overline_on;
   /* return FALSE if any property is different */
{
   StrSegInfo *pStrSeg=pStrBlock->seg;
   int rc=TRUE;

   if (mode == PAINT_NORM) return TRUE;

   rc = CheckStrSegProperties(pStrSeg, pn_valid, pn_font, pn_style, pn_sz_unit,
         pn_color, pn_underline_on, pn_overline_on);
   switch (mode) {
   case PAINT_INV: break;
   case PAINT_NORM_INV: gstTextHighlightInfo.mode = PAINT_INV; break;
   case PAINT_INV_NORM: gstTextHighlightInfo.mode = PAINT_NORM; break;
   case PAINT_NORM_INV_NORM: gstTextHighlightInfo.mode = PAINT_NORM; break;
   }
   return rc;
}

int CanCopyHighLightedTextAsStrings()
   /* returns TRUE if highlighted text is to be copied as simple text */
   /* if the font is a double-byte font, don't treat it like a simple text */
{
   MiniLineInfo *pOwnerMiniLine=NULL;
   StrBlockInfo *pStrBlock=NULL, *pNextStrBlock=NULL;
   int first_index=0, second_index=0, mode=PAINT_NORM, valid=FALSE;
   int font=INVALID, style=INVALID, sz_unit=INVALID, color=INVALID;
   int underline_on=INVALID, overline_on=INVALID;

   /* setup gstTextHighlightInfo */
   if (!UpdateTextHighlightInfo()) return FALSE;

   gstTextHighlightInfo.highlighting = FALSE;
   gstTextHighlightInfo.mode = PAINT_NORM;

   pStrBlock = gstTextHighlightInfo.start_str_block_ptr;
   pOwnerMiniLine = pStrBlock->owner_mini_line;

   GetPaintMode(pStrBlock, &mode, &first_index, &second_index);
   if (!CheckHighlightedStrSegProperties(pStrBlock, mode, &valid, &font, &style,
         &sz_unit, &color, &underline_on, &overline_on)) {
      return FALSE;
   }
   if (valid && IsFontDoubleByte(font)) {
      return FALSE;
   }
   mode = gstTextHighlightInfo.mode;
   pStrBlock = pStrBlock->next;
   while (mode != PAINT_NORM) {
      if (pStrBlock == NULL) {
         pOwnerMiniLine = pOwnerMiniLine->next;
         if (pOwnerMiniLine == NULL) {
            break;
         }
         pStrBlock = pOwnerMiniLine->first_block;
         continue;
      }
      pNextStrBlock = pStrBlock->next;
      GetPaintMode(pStrBlock, &mode, &first_index, &second_index);
      if (mode == PAINT_INV) {
         if (pStrBlock->type == SB_SIMPLE) {
            if (!CheckStrBlockProperties(pStrBlock, &valid, &font, &style,
                  &sz_unit, &color, &underline_on, &overline_on)) {
               return FALSE;
            }
            if (valid && IsFontDoubleByte(font)) {
               return FALSE;
            }
         } else {
            return FALSE;
         }
      } else {
         /* pStrBlock->type better be SB_SIMPLE */
#ifdef _TGIF_DBG /* debug, do not translate */
         TgAssert(pStrBlock->type == SB_SIMPLE,
            "pStrBlock->type != SB_SIMPLE in CanCopyHighLightedTextAsStrings()",
            NULL);
#endif /* _TGIF_DBG */
         if (!CheckHighlightedStrSegProperties(pStrBlock, mode, &valid, &font,
               &style, &sz_unit, &color, &underline_on, &overline_on)) {
            return FALSE;
         }
         if (valid && IsFontDoubleByte(font)) {
            return FALSE;
         }
      }
      mode = gstTextHighlightInfo.mode;
      pStrBlock = pNextStrBlock;
   }
   return TRUE;
}

/* ----------------- CanCopyHighLightedTextAsUTF8Strings() ----------------- */

static
void CheckHighlightedStrSegPropertiesForCopyHighLightedTextAsUTF8Strings(mode)
   int mode;
{
   if (mode == PAINT_NORM) return;

   switch (mode) {
   case PAINT_INV: break;
   case PAINT_NORM_INV: gstTextHighlightInfo.mode = PAINT_INV; break;
   case PAINT_INV_NORM: gstTextHighlightInfo.mode = PAINT_NORM; break;
   case PAINT_NORM_INV_NORM: gstTextHighlightInfo.mode = PAINT_NORM; break;
   }
}

static
int CheckDoubleByteFontForCopyHighLightedTextAsUTF8Strings(mode, pStrSeg,
      pn_double_byte_font, pn_valid)
   int mode, *pn_double_byte_font, *pn_valid;
   StrSegInfo *pStrSeg;
{
   int font_index=pStrSeg->font;

   if (mode == PAINT_NORM) return TRUE;

   if (IsFontDoubleByte(font_index)) {
      if (CanConvertUTF8ToFont(font_index)) {
         if (*pn_valid) {
            if (*pn_double_byte_font != font_index) {
               return FALSE;
            }
         } else {
            *pn_double_byte_font = font_index;
            *pn_valid = TRUE;
            SetCopyUTF8FontInfo(pStrSeg, TRUE);
         }
      } else {
         return FALSE;
      }
   } else {
      if (!gstCopyUTF8Info.single_byte_valid) {
         SetCopyUTF8FontInfo(pStrSeg, FALSE);
      }
   }
   switch (mode) {
   case PAINT_INV: break;
   case PAINT_NORM_INV: gstTextHighlightInfo.mode = PAINT_INV; break;
   case PAINT_INV_NORM: gstTextHighlightInfo.mode = PAINT_NORM; break;
   case PAINT_NORM_INV_NORM: gstTextHighlightInfo.mode = PAINT_NORM; break;
   }
   return TRUE;
}

int CanCopyHighLightedTextAsUTF8Strings(pn_double_byte_font_index)
   int *pn_double_byte_font_index;
   /* returns TRUE if highlighted text is to be copied as simple text */
   /* if the font is a double-byte font, don't treat it like a simple text */
{
   MiniLineInfo *pOwnerMiniLine=NULL;
   StrBlockInfo *pStrBlock=NULL, *pNextStrBlock=NULL;
   int first_index=0, second_index=0, mode=PAINT_NORM, valid=FALSE;
   int double_byte_font=INVALID;

   /* setup gstTextHighlightInfo */
   if (!UpdateTextHighlightInfo()) return FALSE;
   ClearCopyUTF8Info();

   gstTextHighlightInfo.highlighting = FALSE;
   gstTextHighlightInfo.mode = PAINT_NORM;

   pStrBlock = gstTextHighlightInfo.start_str_block_ptr;
   pOwnerMiniLine = pStrBlock->owner_mini_line;

   GetPaintMode(pStrBlock, &mode, &first_index, &second_index);

   CheckHighlightedStrSegPropertiesForCopyHighLightedTextAsUTF8Strings(mode);
   if (IsFontDoubleByte(pStrBlock->seg->font)) {
      if (CanConvertUTF8ToFont(pStrBlock->seg->font)) {
         double_byte_font = pStrBlock->seg->font;
         valid = TRUE;
         SetCopyUTF8FontInfo(pStrBlock->seg, TRUE);
      } else {
         return FALSE;
      }
   } else {
      SetCopyUTF8FontInfo(pStrBlock->seg, FALSE);
   }
   mode = gstTextHighlightInfo.mode;
   pStrBlock = pStrBlock->next;
   while (mode != PAINT_NORM) {
      if (pStrBlock == NULL) {
         pOwnerMiniLine = pOwnerMiniLine->next;
         if (pOwnerMiniLine == NULL) {
            break;
         }
         pStrBlock = pOwnerMiniLine->first_block;
         continue;
      }
      pNextStrBlock = pStrBlock->next;
      GetPaintMode(pStrBlock, &mode, &first_index, &second_index);
      if (mode == PAINT_INV) {
         if (pStrBlock->type == SB_SIMPLE) {
            if (!CheckDoubleByteFontForCopyHighLightedTextAsUTF8Strings(
                  mode, pStrBlock->seg, &double_byte_font, &valid)) {
               return FALSE;
            }
         } else {
            return FALSE;
         }
      } else {
         /* pStrBlock->type better be SB_SIMPLE */
#ifdef _TGIF_DBG /* debug, do not translate */
         TgAssert(pStrBlock->type == SB_SIMPLE,
            "pStrBlock->type != SB_SIMPLE in CanCopyHighLightedTextAsUTF8Strings()",
            NULL);
#endif /* _TGIF_DBG */
         if (!CheckDoubleByteFontForCopyHighLightedTextAsUTF8Strings(
               mode, pStrBlock->seg, &double_byte_font, &valid)) {
            return FALSE;
         }
      }
      mode = gstTextHighlightInfo.mode;
      pStrBlock = pNextStrBlock;
   }
   if (valid && pn_double_byte_font_index != NULL) {
       *pn_double_byte_font_index = double_byte_font;
   }
   if (valid) {
      DebugCopyUTF8Info();
   }
   return valid;
}

/* ----------------- CanPasteUTF8StringIntoText() ----------------- */

int CanPasteUTF8StringIntoText(pssi)
   StrSegInfo *pssi;
   /* returns TRUE if highlighted text is to be copied as simple text */
   /* if the font is a double-byte font, don't treat it like a simple text */
{
   StrBlockInfo *pStrBlock=NULL;

   if (!textHighlight) {
      if (CanConvertUTF8ToFont(curFont)) {
         return TRUE;
      }
      if (CanFindDoubleByteFontAtCursor(pssi)) {
         return CanConvertUTF8ToFont(pssi->font);
      }
      return FALSE;
   }
   /* setup gstTextHighlightInfo */
   if (!UpdateTextHighlightInfo()) return FALSE;

   if (FixHighlightedStrBlockDepths() < 0) {
      SwitchTextCurAndEndStrBlocks();
   }
   pStrBlock = gstTextHighlightInfo.start_str_block_ptr;
   if (pStrBlock->seg->double_byte) {
      return TRUE;
   }
   if (CanFindDoubleByteFontAtCursor(pssi)) {
      return CanConvertUTF8ToFont(pssi->font);
   }
   return FALSE;
}

/* -------------------- GatherHighLightedTextAsStrings() -------------------- */

static
void GatherString(buf, len, ppsz_buf, pn_buf_sz)
   char *buf, **ppsz_buf;
   int len, *pn_buf_sz;
   /* *pn_buf_sz includes the terminating '\0' */
{
   if (*ppsz_buf == NULL) {
      *ppsz_buf = (char*)malloc(len+1);
      if (*ppsz_buf == NULL) FailAllocMessage();
      memcpy(*ppsz_buf, buf, len+1);
      *pn_buf_sz = len+1;
   } else {
      *ppsz_buf = (char*)realloc(*ppsz_buf, (*pn_buf_sz)+len);
      if (*ppsz_buf == NULL) FailAllocMessage();
      memcpy(&(*ppsz_buf)[*pn_buf_sz-1], buf, len+1);
      *pn_buf_sz += len;
   }
}

static
void GatherStrSeg(pStrSeg, ppsz_buf, pn_buf_sz)
   StrSegInfo *pStrSeg;
   char **ppsz_buf;
   int *pn_buf_sz;
   /* *pn_buf_sz includes the terminating '\0' */
{
   GatherString(pStrSeg->dyn_str.s, pStrSeg->dyn_str.sz-1, ppsz_buf, pn_buf_sz);
}

static
void GatherHighlightedStrSeg(pStrBlock, mode, first_index, second_index,
      ppsz_buf, pn_buf_sz)
   StrBlockInfo *pStrBlock;
   int mode, first_index, second_index, *pn_buf_sz;
   char **ppsz_buf;
{
   StrSegInfo *pStrSeg=pStrBlock->seg;
   char *buf=NULL, saved_ch='\0';
   int len=0;

   if (mode == PAINT_NORM) return;

   switch (mode) {
   case PAINT_INV:
      /* can only get here if pStrBlock->type is SB_SIMPLE */
      GatherStrSeg(pStrSeg, ppsz_buf, pn_buf_sz);
      break;

   case PAINT_NORM_INV:
      /* gathering the tail */
      if (first_index == pStrSeg->dyn_str.sz-1) {
         /* nothing need to change */
      } else {
         buf = (&pStrSeg->dyn_str.s[first_index]);
         len = pStrSeg->dyn_str.sz-1-first_index;
         GatherString(buf, len, ppsz_buf, pn_buf_sz);
      }
      gstTextHighlightInfo.mode = PAINT_INV;
      break;

   case PAINT_INV_NORM:
      /* gathering the head */
      if (first_index == 0) {
         /* nothing need to change */
      } else {
         buf = pStrSeg->dyn_str.s;
         saved_ch = buf[first_index];

         buf[first_index] = '\0';
         GatherString(buf, first_index, ppsz_buf, pn_buf_sz);
         buf[first_index] = saved_ch;
      }
      gstTextHighlightInfo.mode = PAINT_NORM;
      break;

   case PAINT_NORM_INV_NORM:
      if (first_index == 0 && second_index == pStrSeg->dyn_str.sz-1) {
         GatherStrSeg(pStrSeg, ppsz_buf, pn_buf_sz);
      } else if (first_index == 0) {
         /* just change the head */
         buf = pStrSeg->dyn_str.s;
         saved_ch = buf[second_index];

         buf[second_index] = '\0';
         GatherString(buf, second_index, ppsz_buf, pn_buf_sz);
         buf[second_index] = saved_ch;
      } else if (second_index == pStrSeg->dyn_str.sz-1) {
         /* just change the tail */
         buf = (&pStrSeg->dyn_str.s[first_index]);
         len = pStrSeg->dyn_str.sz-1-first_index;
         GatherString(buf, len, ppsz_buf, pn_buf_sz);
      } else {
         /* change the middle */
         buf = pStrSeg->dyn_str.s;
         saved_ch = buf[second_index];

         buf[second_index] = '\0';
         GatherString(&buf[first_index], second_index-first_index,
               ppsz_buf, pn_buf_sz);
         buf[second_index] = saved_ch;
      }
      gstTextHighlightInfo.mode = PAINT_NORM;
      break;
   }
}

void GatherHighLightedTextAsStrings(ppsz_buf, pn_buf_sz)
   char **ppsz_buf;
   int *pn_buf_sz;
   /* *pn_buf_sz will include the terminating '\0' */
{
   MiniLineInfo *pOwnerMiniLine=NULL;
   StrBlockInfo *pStrBlock=NULL, *pNextStrBlock=NULL;
   int first_index=0, second_index=0, mode=PAINT_NORM;

   /* setup gstTextHighlightInfo */
   if (!UpdateTextHighlightInfo()) return;

   gstTextHighlightInfo.highlighting = FALSE;
   gstTextHighlightInfo.mode = PAINT_NORM;

   pStrBlock = gstTextHighlightInfo.start_str_block_ptr;
   pOwnerMiniLine = pStrBlock->owner_mini_line;

   GetPaintMode(pStrBlock, &mode, &first_index, &second_index);
   GatherHighlightedStrSeg(pStrBlock, mode, first_index, second_index,
         ppsz_buf, pn_buf_sz);
   mode = gstTextHighlightInfo.mode;
   pStrBlock = pStrBlock->next;
   while (mode != PAINT_NORM) {
      if (pStrBlock == NULL) {
         pOwnerMiniLine = pOwnerMiniLine->next;
         if (pOwnerMiniLine == NULL) {
            break;
         }
         pStrBlock = pOwnerMiniLine->first_block;
         GatherString("\n", 1, ppsz_buf, pn_buf_sz);
         continue;
      }
      pNextStrBlock = pStrBlock->next;
      GetPaintMode(pStrBlock, &mode, &first_index, &second_index);

      /* pStrBlock->type better be SB_SIMPLE */
#ifdef _TGIF_DBG /* debug, do not translate */
      TgAssert(pStrBlock->type == SB_SIMPLE,
            "pStrBlock->type != SB_SIMPLE in GatherHighLightedTextAsStrings()",
            NULL);
#endif /* _TGIF_DBG */
      if (mode == PAINT_INV) {
         GatherStrSeg(pStrBlock->seg, ppsz_buf, pn_buf_sz);
      } else {
         GatherHighlightedStrSeg(pStrBlock, mode, first_index, second_index,
               ppsz_buf, pn_buf_sz);
      }
      mode = gstTextHighlightInfo.mode;
      pStrBlock = pNextStrBlock;
   }
}

/* ------------------- CreateTextObjFromHighLightedText() ------------------- */

static
void CopyHighlightedStrSeg(pStrBlock, mode, first_index,
      second_index, pNewOwnerMiniLine)
   StrBlockInfo *pStrBlock;
   int mode, first_index, second_index;
   MiniLineInfo *pNewOwnerMiniLine; /* owner miniline of the new string block */
{
   StrSegInfo *pStrSeg=pStrBlock->seg;
   char *buf=NULL;
   int faked_simple_type=FALSE;

   if (pStrBlock->type == SB_SUPSUB_CENTER) {
      /* only coping the string segment */
      faked_simple_type = TRUE;
      pStrBlock->type = SB_SIMPLE;
   }
   switch (mode) {
   case PAINT_NORM: break;

   case PAINT_INV:
      DupStrBlock(pStrBlock, pNewOwnerMiniLine, &pNewOwnerMiniLine->first_block,
            &pNewOwnerMiniLine->last_block);
      break;

   case PAINT_NORM_INV:
      /* copying the tail */
      buf = (&pStrSeg->dyn_str.s[first_index]);
      DupStrBlock(pStrBlock, pNewOwnerMiniLine,
            &pNewOwnerMiniLine->first_block, &pNewOwnerMiniLine->last_block);
      DynStrSet(&pNewOwnerMiniLine->last_block->seg->dyn_str, buf);
      gstTextHighlightInfo.mode = PAINT_INV;
      break;

   case PAINT_INV_NORM:
      /* copying the head */
      if (first_index == 0) {
         /* nothing to copy */
      } else {
         char saved_ch='\0';

         buf = pStrSeg->dyn_str.s;
         saved_ch = buf[first_index];
         buf[first_index] = '\0';
         DupStrBlock(pStrBlock, pNewOwnerMiniLine,
               &pNewOwnerMiniLine->first_block, &pNewOwnerMiniLine->last_block);
         DynStrSet(&pNewOwnerMiniLine->last_block->seg->dyn_str, buf);
         buf[first_index] = saved_ch;
      }
      gstTextHighlightInfo.mode = PAINT_NORM;
      break;

   case PAINT_NORM_INV_NORM:
      if (first_index == 0 && second_index == pStrSeg->dyn_str.sz-1) {
         DupStrBlock(pStrBlock, pNewOwnerMiniLine,
               &pNewOwnerMiniLine->first_block, &pNewOwnerMiniLine->last_block);
      } else if (first_index == 0) {
         /* just copy the head */
         char saved_ch='\0';

         buf = pStrSeg->dyn_str.s;
         saved_ch = buf[second_index];
         buf[second_index] = '\0';
         DupStrBlock(pStrBlock, pNewOwnerMiniLine,
               &pNewOwnerMiniLine->first_block, &pNewOwnerMiniLine->last_block);
         DynStrSet(&pNewOwnerMiniLine->last_block->seg->dyn_str, buf);
         buf[second_index] = saved_ch;
      } else if (second_index == pStrSeg->dyn_str.sz-1) {
         /* just copy the tail */
         buf = (&pStrSeg->dyn_str.s[first_index]);
         DupStrBlock(pStrBlock, pNewOwnerMiniLine,
               &pNewOwnerMiniLine->first_block, &pNewOwnerMiniLine->last_block);
         DynStrSet(&pNewOwnerMiniLine->last_block->seg->dyn_str, buf);
      } else {
         /* change the middle */
         char saved_ch='\0';

         buf = pStrSeg->dyn_str.s;
         saved_ch = buf[second_index];
         buf[second_index] = '\0';
         DupStrBlock(pStrBlock, pNewOwnerMiniLine,
               &pNewOwnerMiniLine->first_block, &pNewOwnerMiniLine->last_block);
         DynStrSet(&pNewOwnerMiniLine->last_block->seg->dyn_str,
               &buf[first_index]);
         buf[second_index] = saved_ch;
      }
      gstTextHighlightInfo.mode = PAINT_NORM;
      break;
   }
   if (faked_simple_type) {
      pStrBlock->type = SB_SUPSUB_CENTER;
   }
}

struct ObjRec *CreateTextObjFromHighLightedText()
{
   MiniLinesInfo *minilines=NULL;
   MiniLineInfo *pOwnerMiniLine=NULL, *pFirstMiniLine=NULL, *pLastMiniLine=NULL;
   StrBlockInfo *pStrBlock=NULL, *pNextStrBlock=NULL;
   int first_index=0, second_index=0, mode=PAINT_NORM, num_lines=1;
   struct ObjRec *partial_text_obj_ptr=NULL;
   struct TextRec *text_ptr=NULL;

   /* setup gstTextHighlightInfo */
   if (!UpdateTextHighlightInfo()) return NULL;

   partial_text_obj_ptr = DupObj(curTextObj);
   if (partial_text_obj_ptr == NULL) return NULL;
   text_ptr = partial_text_obj_ptr->detail.t;
   minilines = (&text_ptr->minilines);
   FreeMiniLines(minilines, FALSE);
   InvalidateTextCache(text_ptr);
   pFirstMiniLine = pLastMiniLine = NewMiniLine();
   pLastMiniLine->owner_minilines = minilines;

   gstTextHighlightInfo.highlighting = FALSE;
   gstTextHighlightInfo.mode = PAINT_NORM;

   pStrBlock = gstTextHighlightInfo.start_str_block_ptr;
   pOwnerMiniLine = pStrBlock->owner_mini_line;

   GetPaintMode(pStrBlock, &mode, &first_index, &second_index);
   CopyHighlightedStrSeg(pStrBlock, mode, first_index, second_index,
         pLastMiniLine);
   mode = gstTextHighlightInfo.mode;
   pStrBlock = pStrBlock->next;
   while (mode != PAINT_NORM) {
      if (pStrBlock == NULL) {
         MiniLineInfo *pNewMiniLine=NULL;

         pOwnerMiniLine = pOwnerMiniLine->next;
         if (pOwnerMiniLine == NULL) {
            break;
         }
         pStrBlock = pOwnerMiniLine->first_block;
         pNewMiniLine = NewMiniLine();
         pNewMiniLine->owner_minilines = minilines;
         num_lines++;
         InsertMiniLine(pLastMiniLine, NULL, pNewMiniLine, &pFirstMiniLine,
               &pLastMiniLine);
         continue;
      }
      pNextStrBlock = pStrBlock->next;
      GetPaintMode(pStrBlock, &mode, &first_index, &second_index);
      if (mode == PAINT_INV) {
         DupStrBlock(pStrBlock, pLastMiniLine, &pLastMiniLine->first_block,
               &pLastMiniLine->last_block);
      } else {
         /* pStrBlock->type better be SB_SIMPLE */
#ifdef _TGIF_DBG /* debug, do not translate */
         TgAssert(pStrBlock->type == SB_SIMPLE,
         "pStrBlock->type != SB_SIMPLE in CreateTextObjFromHighLightedText()",
               NULL);
#endif /* _TGIF_DBG */
         CopyHighlightedStrSeg(pStrBlock, mode, first_index, second_index,
               pLastMiniLine);
      }
      mode = gstTextHighlightInfo.mode;
      pStrBlock = pNextStrBlock;
   }
   minilines->first = pFirstMiniLine;
   minilines->last = pLastMiniLine;
   text_ptr->lines = num_lines;
   text_ptr->baseline_y = curTextObj->detail.t->baseline_y;
   RecalcTextMetrics(text_ptr, partial_text_obj_ptr->x, text_ptr->baseline_y);
   UpdTextBBox(partial_text_obj_ptr);
   return partial_text_obj_ptr;
}

/* --------------------- ReorderCursorStrBlocks() --------------------- */

void ReorderCursorStrBlocks()
{
   /* setup gstTextHighlightInfo */
   if (!UpdateTextHighlightInfo()) return;

   curStrBlock = gstTextHighlightInfo.start_str_block_ptr;
   textCurIndex = gstTextHighlightInfo.start_index;
   endStrBlock = gstTextHighlightInfo.end_str_block_ptr;
   textEndIndex = gstTextHighlightInfo.end_index;
}

/* --------------------- FindStringInMiniLines() --------------------- */

static
int CaseDependentStrNCmp(buf1, buf2, str_len, case_sensitive)
   char *buf1, *buf2;
   int str_len, case_sensitive;
{
   return (case_sensitive ? strncmp(buf1, buf2, str_len) :
         UtilStrNCaseCmp(buf1, buf2, str_len));
}

static
int MatchStringInOnlyThisStrBlock(pStrBlock, nIndex, str_to_match,
      str_len_to_match, case_sensitive, ppStartStrBlock, pn_start_index,
      ppEndStrBlock, pn_end_index)
   StrBlockInfo *pStrBlock;
   int nIndex, str_len_to_match, case_sensitive;
   char *str_to_match;
   StrBlockInfo **ppStartStrBlock, **ppEndStrBlock;
   int *pn_start_index, *pn_end_index;
   /* return TRUE if found */
{
   char *buf=(&pStrBlock->seg->dyn_str.s[nIndex]);
   int buf_len=pStrBlock->seg->dyn_str.sz-nIndex-1;
   int inc=(pStrBlock->seg->double_byte?2:1);

   for ( ; buf_len >= str_len_to_match; buf+=inc, buf_len-=inc) {
      if (CaseDependentStrNCmp(buf, str_to_match, str_len_to_match,
            case_sensitive) == 0) {
         /* match! */
         *ppStartStrBlock = *ppEndStrBlock = pStrBlock;
         *pn_start_index = buf-pStrBlock->seg->dyn_str.s;
         *pn_end_index = (*pn_start_index)+str_len_to_match;
         return TRUE;
      }
   }
   return FALSE;
}

static
int MatchStringFromCursor(pStrBlock, nIndex, str_to_match, str_len_to_match,
      case_sensitive, ppStartStrBlock, pn_start_index, ppEndStrBlock,
      pn_end_index)
   StrBlockInfo *pStrBlock;
   int nIndex, str_len_to_match, case_sensitive;
   char *str_to_match;
   StrBlockInfo **ppStartStrBlock, **ppEndStrBlock;
   int *pn_start_index, *pn_end_index;
   /*
    * return TRUE if found
    *
    * If *ppStartStrBlock is NULL, this means that we are still trying to
    *       find a prefix match.
    * If *ppStartStrBlock is non-NULL, that means the prefix is found, so
    *       we must match starting at the beginning of every string.
    */
{
   char *buf=NULL;
   int buf_len=0;

   if (pStrBlock == NULL || pStrBlock->type != SB_SIMPLE) {
      return FALSE;
   }
   buf = (&pStrBlock->seg->dyn_str.s[nIndex]);
   buf_len = pStrBlock->seg->dyn_str.sz-nIndex-1;

   if (*ppStartStrBlock == NULL) {
      /* still trying to find a prefix match */
      int inc=(pStrBlock->seg->double_byte?2:1);

      if (buf_len >= str_len_to_match) {
         if (MatchStringInOnlyThisStrBlock(pStrBlock, nIndex, str_to_match,
               str_len_to_match, case_sensitive, ppStartStrBlock,
               pn_start_index, ppEndStrBlock, pn_end_index)) {
            return TRUE;
         }
         for ( ; buf_len >= str_len_to_match; buf+=inc, buf_len-=inc) {
         }
      }
      for ( ; buf_len > 0; buf+=inc, buf_len-=inc) {
         if (CaseDependentStrNCmp(buf, str_to_match, buf_len,
               case_sensitive) == 0) {
            /* got a prefix match */
            *ppStartStrBlock = pStrBlock;
            *pn_start_index = buf-pStrBlock->seg->dyn_str.s;
            if (MatchStringFromCursor(pStrBlock->next, 0,
                  &str_to_match[buf_len], str_len_to_match-buf_len,
                  case_sensitive, ppStartStrBlock, pn_start_index,
                  ppEndStrBlock, pn_end_index)) {
               return TRUE;
            }
            *ppStartStrBlock = NULL;
            *pn_start_index = INVALID;
         }
      }
   } else {
      /* the leading chars of buf much match str_to_match */
      if (buf_len >= str_len_to_match) {
         if (CaseDependentStrNCmp(buf, str_to_match, str_len_to_match,
               case_sensitive) == 0) {
            /* match! */
            *ppEndStrBlock = pStrBlock;
            *pn_end_index = buf-pStrBlock->seg->dyn_str.s+str_len_to_match;
            return TRUE;
         }
      } else {
         if (CaseDependentStrNCmp(buf, str_to_match, buf_len,
               case_sensitive) == 0) {
            if (MatchStringFromCursor(pStrBlock->next, nIndex, str_to_match,
                  str_len_to_match-buf_len, case_sensitive, ppStartStrBlock,
                  pn_start_index, ppEndStrBlock, pn_end_index)) {
               return TRUE;
            }
         }
      }
   }
   return FALSE;
}

static
int FindStringInStrBlock(pStrBlock, pn_found_starting_point, str_to_match,
      str_len_to_match, case_sensitive, ppStartStrBlock, pn_start_index,
      ppEndStrBlock, pn_end_index)
   StrBlockInfo *pStrBlock;
   int *pn_found_starting_point, str_len_to_match, case_sensitive;
   char *str_to_match;
   StrBlockInfo **ppStartStrBlock, **ppEndStrBlock;
   int *pn_start_index, *pn_end_index;
   /* return TRUE if found */
{
   switch (pStrBlock->type) {
   case SB_SIMPLE:
      if (*pn_found_starting_point) {
         if (MatchStringFromCursor(pStrBlock, 0, str_to_match, str_len_to_match,
               case_sensitive, ppStartStrBlock, pn_start_index, ppEndStrBlock,
               pn_end_index)) {
            return TRUE;
         }
      } else if (pStrBlock == curStrBlock) {
         *pn_found_starting_point = TRUE;
         if (textCurIndex != pStrBlock->seg->dyn_str.sz-1 &&
               MatchStringFromCursor(pStrBlock,
               textCurIndex+(textHighlight?1:0), str_to_match, str_len_to_match,
               case_sensitive, ppStartStrBlock, pn_start_index, ppEndStrBlock,
               pn_end_index)) {
            return TRUE;
         }
      } else {
         /* still looking for curStrBlock */
      }
      break;

   case SB_CHAR_SPACE: return FALSE;

   case SB_SUPSUB_LEFT:
   case SB_SUPSUB_CENTER:
   case SB_SUPSUB_RIGHT:
      if (pStrBlock->sup != NULL) {
         if (FindStringInMiniLines(pStrBlock->sup, pn_found_starting_point,
               str_to_match, str_len_to_match, case_sensitive, ppStartStrBlock,
               pn_start_index, ppEndStrBlock, pn_end_index)) {
            return TRUE;
         }
      }
      if (pStrBlock->type == SB_SUPSUB_CENTER) {
         if (*pn_found_starting_point) {
            if (MatchStringInOnlyThisStrBlock(pStrBlock, 0, str_to_match,
                  str_len_to_match, case_sensitive, ppStartStrBlock,
                  pn_start_index, ppEndStrBlock, pn_end_index)) {
               return TRUE;
            }
         } else if (pStrBlock == curStrBlock) {
            *pn_found_starting_point = TRUE;
            if (textCurIndex != pStrBlock->seg->dyn_str.sz-1 &&
                  MatchStringInOnlyThisStrBlock(pStrBlock,
                  textCurIndex+(textHighlight?1:0), str_to_match,
                  str_len_to_match, case_sensitive, ppStartStrBlock,
                  pn_start_index, ppEndStrBlock, pn_end_index)) {
               return TRUE;
            }
         } else {
            /* still looking for curStrBlock */
         }
      }
      if (pStrBlock->sub != NULL) {
         if (FindStringInMiniLines(pStrBlock->sub, pn_found_starting_point,
               str_to_match, str_len_to_match, case_sensitive, ppStartStrBlock,
               pn_start_index, ppEndStrBlock, pn_end_index)) {
            return TRUE;
         }
      }
      break;
   }
   return FALSE;
}

static
int FindStringInMiniLine(pMiniLine, pn_found_starting_point, str_to_match,
      str_len_to_match, case_sensitive, ppStartStrBlock, pn_start_index,
      ppEndStrBlock, pn_end_index)
   MiniLineInfo *pMiniLine;
   int *pn_found_starting_point, str_len_to_match, case_sensitive;
   char *str_to_match;
   StrBlockInfo **ppStartStrBlock, **ppEndStrBlock;
   int *pn_start_index, *pn_end_index;
   /* return TRUE if found */
{
   StrBlockInfo *pStrBlock=NULL;

   for (pStrBlock=pMiniLine->first_block; pStrBlock != NULL;
         pStrBlock=pStrBlock->next) {
      if (FindStringInStrBlock(pStrBlock, pn_found_starting_point,
            str_to_match, str_len_to_match, case_sensitive, ppStartStrBlock,
            pn_start_index, ppEndStrBlock, pn_end_index)) {
         return TRUE;
      }
   }
   return FALSE;
}

int FindStringInMiniLines(minilines, pn_found_starting_point, str_to_match,
      str_len_to_match, case_sensitive, ppStartStrBlock, pn_start_index,
      ppEndStrBlock, pn_end_index)
   MiniLinesInfo *minilines;
   int *pn_found_starting_point, str_len_to_match, case_sensitive;
   char *str_to_match;
   StrBlockInfo **ppStartStrBlock, **ppEndStrBlock;
   int *pn_start_index, *pn_end_index;
   /* return TRUE if found */
{
   MiniLineInfo *pMiniLine=NULL;

   for (pMiniLine=minilines->first; pMiniLine != NULL;
         pMiniLine=pMiniLine->next) {
      if (FindStringInMiniLine(pMiniLine, pn_found_starting_point, str_to_match,
            str_len_to_match, case_sensitive, ppStartStrBlock, pn_start_index,
            ppEndStrBlock, pn_end_index)) {
         return TRUE;
      }
   }
   return FALSE;
}

/* --------------------- CanBreakMiniLinesIntoWords() --------------------- */

static
int CanBreakStrSegIntoWords(pStrSeg)
   StrSegInfo *pStrSeg;
{
   return (strchr(pStrSeg->dyn_str.s, ' ') != NULL);
}

static
int CanBreakStrBlockIntoWords(pStrBlock)
   StrBlockInfo *pStrBlock;
{
   switch (pStrBlock->type) {
   case SB_SIMPLE: return CanBreakStrSegIntoWords(pStrBlock->seg);

   case SB_CHAR_SPACE: return TRUE;

   case SB_SUPSUB_LEFT:
   case SB_SUPSUB_CENTER:
   case SB_SUPSUB_RIGHT:
      if (pStrBlock->sup != NULL) {
         if (!BlankMiniLines(pStrBlock->sup)) {
            return TRUE;
         }
      }
      if (pStrBlock->sub != NULL) {
         if (!BlankMiniLines(pStrBlock->sub)) {
            return TRUE;
         }
      }
      if (pStrBlock->type == SB_SUPSUB_CENTER) {
         /* pStrBlock->seg better not be NULL or it will crash */
         if (CanBreakStrSegIntoWords(pStrBlock->seg)) {
            return TRUE;
         }
      }
      break;
   }
   return FALSE;
}

static
int CanBreakMiniLineIntoWords(pMiniLine)
   MiniLineInfo *pMiniLine;
{
   StrBlockInfo *pStrBlock=NULL;

   for (pStrBlock=pMiniLine->first_block; pStrBlock != NULL;
         pStrBlock=pStrBlock->next) {
      if (pStrBlock->next != NULL) {
         return TRUE;
      }
      if (CanBreakStrBlockIntoWords(pStrBlock)) {
         return TRUE;
      }
   }
   return FALSE;
}

int CanBreakMiniLinesIntoWords(minilines)
   MiniLinesInfo *minilines;
{
   MiniLineInfo *pMiniLine=NULL;

   for (pMiniLine=minilines->first; pMiniLine != NULL;
         pMiniLine=pMiniLine->next) {
      if (pMiniLine->next != NULL) {
         return TRUE;
      }
      if (CanBreakMiniLineIntoWords(pMiniLine)) {
         return TRUE;
      }
   }
   return FALSE;
}

/* --------------------- BreakMiniLines() --------------------- */

static
void FinalizeBreakStrSeg(new_obj, x, baseline_y, prototype, tx_to_move,
      ty_to_move, poli)
   struct ObjRec *new_obj;
   int x, baseline_y;
   struct ObjRec *prototype;
   int tx_to_move, ty_to_move;
   ObjListInfo *poli;
{
   struct SelRec *new_sel_ptr=NULL;
   struct TextRec *text_ptr=new_obj->detail.t;
   MiniLinesInfo *minilines=(&text_ptr->minilines);
   int x_pos=x, y_pos=0, w=0, dx=0, dy=0;

   minilines->first->v_gap = 0;

   new_obj->x = x;
   text_ptr->baseline_y = baseline_y;
   UpdTextBBox(new_obj);
   y_pos = new_obj->y;

   dx = dy = 0;
   w = minilines->w;

   switch (minilines->just) {
   case JUST_L: break;
   case JUST_C: dx = (w>>1); break;
   case JUST_R: dx = w; break;
   }
   if (new_obj->ctm != NULL) {
      int new_x=0, new_y=0;

      TransformPointThroughCTM(x_pos+dx-prototype->x,
            y_pos+dy-prototype->y, prototype->ctm, &new_x, &new_y);
      new_x += prototype->x;
      new_y += prototype->y;
      free(new_obj->ctm);
      new_obj->ctm = NULL;
      UpdTextBBox(new_obj);
      SetCTM(new_obj, prototype->ctm);
      MoveObj(new_obj, new_x-new_obj->x-tx_to_move,
            new_y-new_obj->y-ty_to_move);
   } else {
      MoveObj(new_obj, dx, dy);
   }
   new_sel_ptr = (struct SelRec *)malloc(sizeof(struct SelRec));
   if (new_sel_ptr == NULL) FailAllocMessage();
   new_sel_ptr->obj = new_obj;

   new_sel_ptr->next = poli->top_sel;
   new_obj->next = poli->top_obj;
   if (poli->top_sel == NULL) {
      poli->bot_sel = new_sel_ptr;
      poli->bot_obj = new_obj;
   } else {
      poli->top_sel->prev = new_sel_ptr;
      poli->top_obj->prev = new_obj;
   }
   poli->top_sel = new_sel_ptr;
   poli->top_obj = new_obj;
   poli->top_sel->prev = NULL;
   poli->top_obj->prev = NULL;
   poli->count++;
}

static
void BreakStrSeg(pSrcStrSeg, into_words, x, baseline_y, prototype, tx_to_move,
      ty_to_move, poli)
   StrSegInfo *pSrcStrSeg;
   int into_words, x, baseline_y;
   struct ObjRec *prototype;
   int tx_to_move, ty_to_move;
   ObjListInfo *poli;
   /*
    * x is the left edge of the string segment.
    */
{
   char *c_ptr=pSrcStrSeg->dyn_str.s;
   int is_gap=(*c_ptr == ' ' || *c_ptr == '\t');

   if (!TrySetCanvasFont(pSrcStrSeg->font, pSrcStrSeg->style,
         SzUnitToFontSize(pSrcStrSeg->sz_unit), TRUE, NULL)) {
      return;
   }
   while (*c_ptr != '\0') {
      struct ObjRec *new_obj=DupObj(prototype);
      StrSegInfo *pDestStrSeg=NULL;
      char *word_start=NULL, saved_ch='\0';
      int w=0, all_blanks=FALSE;

      FreeStrSeg(GetTextObjFirstStrSeg(new_obj));
      DupStrSeg(new_obj->detail.t->minilines.first->first_block,
            pSrcStrSeg);
      pDestStrSeg = GetTextObjFirstStrSeg(new_obj);
      if (into_words) {
         char *prev_ptr=NULL;

         word_start = c_ptr;
         if (is_gap) {
            for ( ; *c_ptr == ' ' || *c_ptr == '\t'; c_ptr++) {
               prev_ptr = c_ptr;
            }
            /* all_blanks = TRUE; */
         } else {
            for ( ; *c_ptr != ' ' && *c_ptr != '\t' && *c_ptr != '\0';
                  c_ptr++) {
               prev_ptr = c_ptr;
            }
         }
         saved_ch = *c_ptr;
         *c_ptr = '\0';
         DynStrSet(&pDestStrSeg->dyn_str, word_start);
         w = MyTextWidth(canvasFontPtr, pDestStrSeg->dyn_str.s,
              pDestStrSeg->dyn_str.sz-1);
         *c_ptr = saved_ch;
         if (prev_ptr != NULL) c_ptr = prev_ptr;

         is_gap = !is_gap;
      } else {
         if (pSrcStrSeg->double_byte) {
            strncpy(pDestStrSeg->dyn_str.s, c_ptr, 2);
            pDestStrSeg->dyn_str.s[2] = '\0';
            pDestStrSeg->dyn_str.sz = 3;
            w = MyTextWidth(canvasFontPtr, pDestStrSeg->dyn_str.s, 2);
         } else {
            *pDestStrSeg->dyn_str.s = *c_ptr;
            pDestStrSeg->dyn_str.s[1] = '\0';
            pDestStrSeg->dyn_str.sz = 2;
            w = MyTextWidth(canvasFontPtr, pDestStrSeg->dyn_str.s, 1);
         }
      }
      if (c_ptr == NULL) {
         FreeObj(new_obj);
         break;
      } else if (all_blanks) {
         FreeObj(new_obj);
      } else {
         FinalizeBreakStrSeg(new_obj, x, baseline_y, prototype, tx_to_move,
               ty_to_move, poli);
      }
      x += w;
      if (*c_ptr == '\0') break;

      if (pSrcStrSeg->double_byte) {
         c_ptr += 2;
      } else {
         c_ptr++;
      }
   }
}

static
void BreakStrBlock(pStrBlock, into_words, x, baseline_y, prototype, tx_to_move,
      ty_to_move, poli)
   StrBlockInfo *pStrBlock;
   int into_words, x, baseline_y;
   struct ObjRec *prototype;
   int tx_to_move, ty_to_move;
   ObjListInfo *poli;
   /*
    * x is the left edge of the string block.
    */
{
   int saved_baseline_y=baseline_y;
   int block_w=pStrBlock->w;

   switch (pStrBlock->type) {
   case SB_SIMPLE:
      BreakStrSeg(pStrBlock->seg, into_words, x, baseline_y, prototype,
            tx_to_move, ty_to_move, poli);
      break;

   case SB_CHAR_SPACE: break;

   case SB_SUPSUB_LEFT:
   case SB_SUPSUB_CENTER:
   case SB_SUPSUB_RIGHT:
      switch (pStrBlock->type) {
      case SB_SUPSUB_LEFT: x += block_w; break;
      case SB_SUPSUB_CENTER: x += (block_w>>1); break;
      case SB_SUPSUB_RIGHT: break;
      }
      if (pStrBlock->type == SB_SUPSUB_CENTER) {
         /* pStrBlock->seg better not be NULL or it will crash */
         baseline_y = saved_baseline_y;
         BreakStrSeg(pStrBlock->seg, into_words, x-((pStrBlock->seg->w)>>1),
               baseline_y, prototype, tx_to_move, ty_to_move, poli);
      }
      if (pStrBlock->sup != NULL) {
         baseline_y = saved_baseline_y+pStrBlock->sup->baseline_offset;
         if (pStrBlock->type == SB_SUPSUB_CENTER) {
            baseline_y -= pStrBlock->seg->asc;
         }
         BreakMiniLines(pStrBlock->sup, into_words, x, baseline_y, prototype,
               tx_to_move, ty_to_move, poli);
      }
      if (pStrBlock->sub != NULL) {
         baseline_y = saved_baseline_y+pStrBlock->sub->baseline_offset;
         if (pStrBlock->type == SB_SUPSUB_CENTER) {
            baseline_y += pStrBlock->seg->des;
         }
         BreakMiniLines(pStrBlock->sub, into_words, x, baseline_y, prototype,
               tx_to_move, ty_to_move, poli);
      }
      break;
   }
}

static
void BreakMiniLine(pMiniLine, into_words, x, baseline_y, prototype, tx_to_move,
      ty_to_move, poli)
   MiniLineInfo *pMiniLine;
   int into_words, x, baseline_y;
   struct ObjRec *prototype;
   int tx_to_move, ty_to_move;
   ObjListInfo *poli;
   /*
    * x is the left edge of the miniline and baseline_y has been adjusted by
    *       pMiniLine->v_gap.
    */
{
   StrBlockInfo *pStrBlock=NULL;

   for (pStrBlock=pMiniLine->first_block; pStrBlock != NULL;
         pStrBlock=pStrBlock->next) {
      BreakStrBlock(pStrBlock, into_words, x, baseline_y, prototype, tx_to_move,
            ty_to_move, poli);

      x += pStrBlock->w;
   }
}

void BreakMiniLines(minilines, how, x, baseline_y, prototype, tx_to_move,
      ty_to_move, poli)
   MiniLinesInfo *minilines;
   int how, x, baseline_y;
   struct ObjRec *prototype;
   int tx_to_move, ty_to_move;
   ObjListInfo *poli;
   /*
    * x is the origin of the minilines (not the left edge)
    */
{
   MiniLineInfo *pMiniLine=NULL, *pNextMiniLine=NULL;
   int v_space=minilines->v_space;
   int saved_x=x;

   for (pMiniLine=minilines->first; pMiniLine != NULL;
         pMiniLine=pNextMiniLine) {
      pNextMiniLine = pMiniLine->next;
      baseline_y += pMiniLine->v_gap;

      switch (minilines->just) {
      case JUST_L: x = saved_x; break;
      case JUST_C: x = saved_x - ((pMiniLine->w)>>1); break;
      case JUST_R: x = saved_x - pMiniLine->w; break;
      }
      if (how == BREAK_LINE) {
         struct ObjRec *new_obj=DupObj(prototype);
         MiniLinesInfo *new_minilines=(&new_obj->detail.t->minilines);

         FreeMiniLines(new_minilines, FALSE);
         DupMiniLine(pMiniLine, new_minilines, NULL, &new_minilines->first,
               &new_minilines->last);
         FinalizeBreakStrSeg(new_obj, x, baseline_y, prototype, tx_to_move,
               ty_to_move, poli);
      } else {
         BreakMiniLine(pMiniLine, (how==BREAK_WORD), x, baseline_y, prototype,
               tx_to_move, ty_to_move, poli);
      }
      baseline_y += pMiniLine->des + v_space;
      if (pMiniLine->next != NULL) {
         baseline_y += pMiniLine->next->asc;
      }
   }
}

/* --------------------- DumpMiniLines() --------------------- */

static
void DumpGSave(FP, do_dump, indent)
   FILE *FP;
   int do_dump, indent;
{
   if (do_dump) {
      DumpIndentString(FP, indent);
      fprintf(FP, "%s\n", gPsCmd[PS_GSAVE]);
   }
}

static
void DumpGRestore(FP, do_dump, indent)
   FILE *FP;
   int do_dump, indent;
{
   if (do_dump) {
      DumpIndentString(FP, indent);
      fprintf(FP, "%s\n", gPsCmd[PS_GRESTORE]);
   }
}

static
void DumpSetFont(FP, font, font_name, double_byte, style, sz_unit)
   FILE *FP;
   int font, double_byte, style, sz_unit;
   char *font_name;
{
   char buf[MAXSTRING], font_str[MAXSTRING], real_font_str[MAXSTRING];
   char *ps_findfont_cmd=gPsCmd[PS_FINDFONT];
   float fSize=(float)0;

   if (double_byte) {
#ifdef _PS_USE_EUC_FONT
      int euc_font=FALSE;
      char combined_font_str[MAXSTRING];
#endif /* _PS_USE_EUC_FONT */

      if (PRTGIF && font_name != NULL) {
         sprintf(font_str, "/%s", font_name);
      } else {
         GetPSFontStr(font, style, font_str);
      }
      strcpy(real_font_str, font_str);
      MapAliasedPSFontName(real_font_str, sizeof(real_font_str));

#ifdef _PS_USE_EUC_FONT
      /* do not translate -- program constants */
      euc_font = (strstr(font_str, "-EUC-") != NULL);
      if (euc_font) {
         if (preDumpSetup) PSUseEucFont();
         sprintf(combined_font_str, "/Courier-%s", &font_str[1]);
         fprintf(FP, "%s /Courier %s eucfont\n", combined_font_str, font_str); 
         fprintf(FP, "%s ", combined_font_str); 
         UpdateDocumentFonts(&font_str[1]);
         UpdateDocumentFonts("Courier");
      } else {
         fprintf(FP, "%s ", font_str); 
         UpdateDocumentFonts(&font_str[1]);
      }
#else /* ~_PS_USE_EUC_FONT */
      fprintf(FP, "%s ", font_str); 
      UpdateDocumentFonts(&real_font_str[1]);
#endif /* _PS_USE_EUC_FONT */
   } else {
      if (PRTGIF && font_name != NULL) {
         sprintf(font_str, "/%s", font_name);
      } else {
         GetPSFontStr(font, style, font_str);
      }
      strcpy(real_font_str, font_str);
      MapAliasedPSFontName(real_font_str, sizeof(real_font_str));

      if (NeedEncode(&font_str[1], font, style)) {
#ifdef _PRTGIF_USE_ISO_LATIN_1_ALWAYS
         if (PRTGIF && !cmdLineOpenDisplay) {
            if (preDumpSetup) PSUseReencode(NULL);
            fprintf(FP, "%s %s-8 latin1.vec ", real_font_str, font_str);
            ps_findfont_cmd = "TGFF8";
         } else {
            fprintf(FP, "%s-8 ", font_str); 
         }
#else /* ~_PRTGIF_USE_ISO_LATIN_1_ALWAYS */
         fprintf(FP, "%s-8 ", font_str); 
#endif /* _PRTGIF_USE_ISO_LATIN_1_ALWAYS */
      } else {
         fprintf(FP, "%s ", real_font_str); 
      }
      UpdateDocumentFonts(&real_font_str[1]);
   }
   fSize = (float)(((float)sz_unit)/((float)NUM_SZ_UNIT_PER_FONT_SIZE));
   FormatFloat(&fSize, buf);

   fprintf(FP, "%s ", ps_findfont_cmd);
#ifndef _PS_USE_EUC_FONT
   if (double_byte) {
      fprintf(FP, "dup /WMode known {dup /WMode get 1 eq ");
#ifdef _VERT_FONT_OFFSET_POINT_4
      fprintf(FP, "{[0 1 -1 0 0 0.4] makefont} if} if\n");
#else /* ~_VERT_FONT_OFFSET_POINT_4 */
      fprintf(FP, "{[0 1 -1 0 0 0.3] makefont} if} if\n");
#endif /* _VERT_FONT_OFFSET_POINT_4 */
   }
#endif /* ~_PS_USE_EUC_FONT */
   fprintf(FP, "[%s 0 0 -%s 0 0] %s\n", buf, buf, gPsCmd[PS_MAKESETFONT]);
}

static
void DumpStrSeg(pStrSeg, just, x, baseline_y, FP, pBBox, do_dump, indent)
   StrSegInfo *pStrSeg;
   int just, x, baseline_y, do_dump, indent;
   FILE *FP;
   struct BBRec *pBBox;
{
   int font=pStrSeg->font, style=pStrSeg->style, sz_unit=pStrSeg->sz_unit;
   int double_byte=pStrSeg->double_byte, color_index=pStrSeg->color;
   int double_byte_mod_bytes=pStrSeg->double_byte_mod_bytes;
   int asc=(pStrSeg->read_only ? pStrSeg->orig_asc : pStrSeg->asc);
   char *font_name=pStrSeg->font_name;

   if (*pStrSeg->dyn_str.s == '\0') {
      if (!do_dump) {
         DumpIndentString(FP, indent);
         fprintf(FP, "0\n");
      }
      return;
   }
   if (do_dump) {
      DumpIndentString(FP, indent);
      if (penPat == BACKPAT) {
         fprintf(FP, "1 %s\n", gPsCmd[PS_SETGRAY]);
      } else if (colorDump || !useGray) {
         DumpRGBColorLine(FP, color_index, 0, TRUE);
      } else if (penPat == SOLIDPAT) {
         fprintf(FP, "0 %s\n", gPsCmd[PS_SETGRAY]);
      } else {
         GrayCheck(penPat);
         fprintf(FP, "%s %s\n", GrayStr(penPat), gPsCmd[PS_SETGRAY]);
      }
   }
   DumpIndentString(FP, indent);
   DumpSetFont(FP, font, font_name, double_byte, style, sz_unit);
   DumpIndentString(FP, indent);
   fprintf(FP, "(");
   DumpOneStr(FP, font, double_byte, double_byte_mod_bytes, pStrSeg->dyn_str.s);
   if (do_dump) {
      int underline_on=pStrSeg->underline_on;
      int overline_on=pStrSeg->overline_on;

      if (just == JUST_C && preDumpSetup) PSUseCenterText();

      if ((colorDump || !useGray) && penPat > BACKPAT) {
         if (just == JUST_C) {
            fprintf(FP, ") TGCTX %s TGSW %s %s true %s %s %s\n",
                  gPsCmd[PS_DUP], gPsCmd[PS_EXCH], gPsCmd[PS_GSAVE],
                  gPsCmd[PS_CHARPATH], gPsCmd[PS_CLIP], gPsCmd[PS_NEWPATH]);
         } else {
            fprintf(FP, ") %s TGSW %s %s true %s %s %s\n",
                  gPsCmd[PS_DUP], gPsCmd[PS_EXCH], gPsCmd[PS_GSAVE],
                  gPsCmd[PS_CHARPATH], gPsCmd[PS_CLIP], gPsCmd[PS_NEWPATH]);
         }
         if (preDumpSetup) PSUseColorPattern();
         DumpPatFill(FP, penPat, *pBBox, indent, FALSE);
         if (underline_on && overline_on) {
            fprintf(FP, " %s %s %s 0 %s\n", gPsCmd[PS_GRESTORE], gPsCmd[PS_DUP],
                  gPsCmd[PS_DUP], gPsCmd[PS_RMOVETO]);
         } else if (underline_on || overline_on) {
            fprintf(FP, " %s %s 0 %s\n", gPsCmd[PS_GRESTORE], gPsCmd[PS_DUP],
                  gPsCmd[PS_RMOVETO]);
         } else {
            fprintf(FP, " %s 0 %s\n", gPsCmd[PS_GRESTORE], gPsCmd[PS_RMOVETO]);
         }
      } else {
         if (just == JUST_C) {
            if (underline_on && overline_on) {
               fprintf(FP, ") %s %s TGSW %s TGCTX %s\n", gPsCmd[PS_DUP],
                     gPsCmd[PS_DUP], gPsCmd[PS_EXCH], gPsCmd[PS_SHOW]);
            } else if (underline_on || overline_on) {
               fprintf(FP, ") %s TGSW %s TGCTX %s\n",
                     gPsCmd[PS_DUP], gPsCmd[PS_EXCH], gPsCmd[PS_SHOW]);
            } else {
               fprintf(FP, ") TGCTX %s\n", gPsCmd[PS_SHOW]);
            }
         } else {
            if (underline_on && overline_on) {
               fprintf(FP, ") %s %s TGSW %s %s\n", gPsCmd[PS_DUP],
                     gPsCmd[PS_DUP], gPsCmd[PS_EXCH], gPsCmd[PS_SHOW]);
            } else if (underline_on || overline_on) {
               fprintf(FP, ") %s TGSW %s %s\n",
                     gPsCmd[PS_DUP], gPsCmd[PS_EXCH], gPsCmd[PS_SHOW]);
            } else {
               fprintf(FP, ") %s\n", gPsCmd[PS_SHOW]);
            }
         }
      }
      if (underline_on) {
         DumpIndentString(FP, indent);
         fprintf(FP, "%s %s %s 0 %1d %s %s 0 %s %s %s\n",
               gPsCmd[PS_GSAVE], gPsCmd[PS_CURRENTPOINT],
               gPsCmd[PS_MOVETO], curUnderlineYOffset, gPsCmd[PS_RMOVETO],
               gPsCmd[PS_NEG], gPsCmd[PS_RLINETO], gPsCmd[PS_STROKE],
               gPsCmd[PS_GRESTORE]);
      }
      if (overline_on) {
         DumpIndentString(FP, indent);
         fprintf(FP, "%s %s %s 0 %1d %s %s 0 %1d %s %s %s 0 %s %s %s\n",
               gPsCmd[PS_GSAVE], gPsCmd[PS_CURRENTPOINT], gPsCmd[PS_MOVETO],
               curOverlineYOffset, gPsCmd[PS_NEG], gPsCmd[PS_RMOVETO],
               asc, gPsCmd[PS_NEG], gPsCmd[PS_RMOVETO],
               gPsCmd[PS_NEG], gPsCmd[PS_RLINETO], gPsCmd[PS_STROKE],
               gPsCmd[PS_GRESTORE]);
      }
   } else {
      fprintf(FP, ") TGSW \n");
   }
}

static
void DumpSupSubBlock(pStrBlock, x, baseline_y, FP, pBBox, do_dump, indent)
   StrBlockInfo *pStrBlock;
   int x, baseline_y, do_dump, indent;
   FILE *FP;
   struct BBRec *pBBox;
{
   int saved_baseline_y=baseline_y;

   if (!do_dump) {
      DumpIndentString(FP, indent);
      fprintf(FP, "0\n");
   }
   if (pStrBlock->type == SB_SUPSUB_CENTER) {
      /* pStrBlock->seg better not be NULL or it will crash */
      DumpGSave(FP, do_dump, indent);

      baseline_y = saved_baseline_y;
      DumpStrSeg(pStrBlock->seg, JUST_C, x-((pStrBlock->seg->w)>>1), baseline_y,
            FP, pBBox, do_dump, indent+2);
      if (!do_dump) {
         DumpIndentString(FP, indent);
         fprintf(FP, "TGMAX\n");
      }
      DumpGRestore(FP, do_dump, indent);
   }
   if (pStrBlock->sup != NULL) {
      if (!BlankMiniLines(pStrBlock->sup)) {
         DumpGSave(FP, do_dump, indent);
         baseline_y = saved_baseline_y+pStrBlock->sup->baseline_offset;
         if (do_dump && pStrBlock->sup->baseline_offset != 0) {
            DumpIndentString(FP, indent+2);
            fprintf(FP, "0 %1d %s\n", pStrBlock->sup->baseline_offset,
                  gPsCmd[PS_RMOVETO]);
         }
         if (pStrBlock->type == SB_SUPSUB_CENTER && do_dump &&
               pStrBlock->seg->asc != 0) {
            /*
             * shift the superscript up more because SB_SUPSUB_CENTER
             */
            baseline_y -= pStrBlock->seg->asc;
            DumpIndentString(FP, indent);
            fprintf(FP, "0 %1d %s %s\n", pStrBlock->seg->asc, gPsCmd[PS_NEG],
                  gPsCmd[PS_RMOVETO]);
         }
         DumpMiniLines(pStrBlock->sup, x, baseline_y, FP, pBBox, do_dump,
               indent+2);
         if (!do_dump) {
            DumpIndentString(FP, indent);
            fprintf(FP, "TGMAX\n");
         }
         DumpGRestore(FP, do_dump, indent);
      }
   }
   if (pStrBlock->sub != NULL) {
      if (!BlankMiniLines(pStrBlock->sub)) {
         DumpGSave(FP, do_dump, indent);
         baseline_y = saved_baseline_y+pStrBlock->sub->baseline_offset;
         if (do_dump && pStrBlock->sub->baseline_offset != 0) {
            DumpIndentString(FP, indent+2);
            fprintf(FP, "0 %1d %s\n", pStrBlock->sub->baseline_offset,
                  gPsCmd[PS_RMOVETO]);
         }
         if (pStrBlock->type == SB_SUPSUB_CENTER && do_dump &&
               pStrBlock->seg->des != 0) {
            /*
             * shift the subscript down more because SB_SUPSUB_CENTER
             */
            baseline_y += pStrBlock->seg->des;
            DumpIndentString(FP, indent);
            fprintf(FP, "0 %1d %s\n", pStrBlock->seg->des, gPsCmd[PS_RMOVETO]);
         }
         DumpMiniLines(pStrBlock->sub, x, baseline_y, FP, pBBox, do_dump,
               indent+2);
         if (!do_dump) {
            DumpIndentString(FP, indent);
            fprintf(FP, "TGMAX\n");
         }
         DumpGRestore(FP, do_dump, indent);
      }
   }
}

static
void DumpStrBlock(pStrBlock, x, baseline_y, FP, pBBox, do_dump, indent)
   StrBlockInfo *pStrBlock;
   int x, baseline_y, do_dump, indent;
   FILE *FP;
   struct BBRec *pBBox;
   /*
    * x is the left edge of the string block.
    */
{
   int block_w=pStrBlock->w;

   switch (pStrBlock->type) {
   case SB_SIMPLE:
      DumpStrSeg(pStrBlock->seg, JUST_L, x, baseline_y, FP, pBBox, do_dump,
            indent+2);
      break;

   case SB_CHAR_SPACE:
      if (do_dump) {
         fprintf(FP, "%1d 0 %s\n", pStrBlock->special_char_w,
               gPsCmd[PS_RMOVETO]);
      } else {
         fprintf(FP, "%1d \n", pStrBlock->special_char_w);
      }
      break;

   case SB_SUPSUB_LEFT:
   case SB_SUPSUB_CENTER:
   case SB_SUPSUB_RIGHT:
      if (!do_dump) {
         DumpIndentString(FP, indent);
         fprintf(FP, "%s\n", gPsCmd[PS_GSAVE]);
         DumpIndentString(FP, indent);
         fprintf(FP, "0\n");
      }
      if (do_dump) {
         switch (pStrBlock->type) {
         case SB_SUPSUB_LEFT:
         case SB_SUPSUB_CENTER:
            if (do_dump) {
               DumpGSave(FP, do_dump, indent);
               DumpIndentString(FP, indent);
               fprintf(FP, "0\n");
            }
            if (pStrBlock->type == SB_SUPSUB_CENTER) {
               x += (block_w>>1);
            } else {
               x += block_w;
            }
            DumpSupSubBlock(pStrBlock, x, baseline_y, FP, pBBox, FALSE,
                  indent+2);
            DumpIndentString(FP, indent);
            fprintf(FP, "TGMAX\n");
            DumpGRestore(FP, do_dump, indent);
            DumpGSave(FP, do_dump, indent);
            DumpIndentString(FP, indent);
            fprintf(FP, "%s\n", gPsCmd[PS_DUP]);

            DumpIndentString(FP, indent);
            if (pStrBlock->type == SB_SUPSUB_CENTER) {
               fprintf(FP, "2 %s 0 %s\n", gPsCmd[PS_DIV], gPsCmd[PS_RMOVETO]);
            } else {
               fprintf(FP, "0 %s\n", gPsCmd[PS_RMOVETO]);
            }
            /* now dump it */
            DumpSupSubBlock(pStrBlock, x, baseline_y, FP, pBBox, TRUE,
                  indent+2);

            DumpGRestore(FP, do_dump, indent);
            DumpIndentString(FP, indent);
            fprintf(FP, "0 %s\n", gPsCmd[PS_RMOVETO]);
            break;
         case SB_SUPSUB_RIGHT:
            DumpGSave(FP, do_dump, indent);
            DumpSupSubBlock(pStrBlock, x, baseline_y, FP, pBBox, TRUE,
                  indent+2);
            DumpGRestore(FP, do_dump, indent);
            DumpSupSubBlock(pStrBlock, x, baseline_y, FP, pBBox, FALSE,
                  indent+2);
            DumpIndentString(FP, indent);
            fprintf(FP, "0 %s\n", gPsCmd[PS_RMOVETO]);
            break;
         }
      } else {
         DumpSupSubBlock(pStrBlock, x, baseline_y, FP, pBBox, FALSE, indent+2);
         DumpIndentString(FP, indent);
         fprintf(FP, "TGMAX\n");
      }
      if (!do_dump) {
         DumpIndentString(FP, indent);
         fprintf(FP, "%s\n", gPsCmd[PS_GRESTORE]);
      }
      break;
   }
}

static
void DumpMiniLine(pMiniLine, x, baseline_y, FP, pBBox, do_dump, indent)
   MiniLineInfo *pMiniLine;
   int x, baseline_y, do_dump, indent;
   FILE *FP;
   struct BBRec *pBBox;
   /*
    * x is the left edge of the miniline and baseline_y has been adjusted by
    *       pMiniLine->v_gap.
    */
{
   StrBlockInfo *pStrBlock=NULL;

   if (!do_dump) {
      /*
       * compute the sum of the widths and leave it on the stop of stack
       */
      DumpIndentString(FP, indent);
      fprintf(FP, "%s\n", gPsCmd[PS_GSAVE]);
      DumpIndentString(FP, indent);
      fprintf(FP, "0\n");
   }
   for (pStrBlock=pMiniLine->first_block; pStrBlock != NULL;
         pStrBlock=pStrBlock->next) {
      DumpStrBlock(pStrBlock, x, baseline_y, FP, pBBox, do_dump, indent+2);
      if (!do_dump) {
         DumpIndentString(FP, indent);
         fprintf(FP, "%s\n", gPsCmd[PS_ADD]);
      }
      x += pStrBlock->w;
   }
   if (!do_dump) {
      DumpIndentString(FP, indent);
      fprintf(FP, "%s\n", gPsCmd[PS_GRESTORE]);
   }
}

void DumpMiniLines(minilines, x, baseline_y, FP, pBBox, do_dump, indent)
   MiniLinesInfo *minilines;
   int x, baseline_y, do_dump, indent;
   FILE *FP;
   struct BBRec *pBBox;
   /*
    * x is the origin of the minilines (not the left edge)
    */
{
   MiniLineInfo *pMiniLine=NULL;
   int v_space=minilines->v_space, just=minilines->just;
   int saved_x=x;

   if (!do_dump) {
      /*
       * compute the max of the widths and leave it on the stop of stack
       */
      DumpIndentString(FP, indent);
      fprintf(FP, "0\n");
   }
   for (pMiniLine=minilines->first; pMiniLine != NULL;
         pMiniLine=pMiniLine->next) {
      baseline_y += pMiniLine->v_gap;
      if (do_dump && pMiniLine->v_gap != 0) {
         DumpIndentString(FP, indent);
         fprintf(FP, "0 %1d %s\n", pMiniLine->v_gap, gPsCmd[PS_RMOVETO]);
      }
      DumpGSave(FP, do_dump, indent);

      if (do_dump) {
         switch (just) {
         case JUST_L:
            DumpMiniLine(pMiniLine, saved_x, baseline_y, FP, pBBox, TRUE,
                  indent+2);
            break;
         case JUST_C:
         case JUST_R:
            /* get the length of the string on top of the stack */
            DumpMiniLine(pMiniLine, x, baseline_y, FP, pBBox, FALSE, indent+2);
            DumpIndentString(FP, indent);
            if (just == JUST_C) {
               fprintf(FP, "2 %s %s 0 %s\n", gPsCmd[PS_DIV], gPsCmd[PS_NEG],
                     gPsCmd[PS_RMOVETO]);
            } else {
               fprintf(FP, "%s 0 %s\n", gPsCmd[PS_NEG], gPsCmd[PS_RMOVETO]);
            }
            /* now dump it */
            DumpMiniLine(pMiniLine, x, baseline_y, FP, pBBox, TRUE, indent+2);
            break;
         }
      } else {
         DumpMiniLine(pMiniLine, x, baseline_y, FP, pBBox, FALSE, indent+2);
         DumpIndentString(FP, indent);
         fprintf(FP, "TGMAX\n");
      }
      DumpGRestore(FP, do_dump, indent);

      if (pMiniLine->next != NULL) {
         baseline_y += pMiniLine->des + v_space + pMiniLine->next->asc;
         if (do_dump && (pMiniLine->des+v_space+pMiniLine->next->asc) != 0) {
            DumpIndentString(FP, indent);
            fprintf(FP, "0 %1d %s\n",
                  (pMiniLine->des+v_space+pMiniLine->next->asc),
                  gPsCmd[PS_RMOVETO]);
         }
      }
   }
}

/* --------------------- DumpSimpleTextPath() --------------------- */

int SimpleTextObj(text_ptr)
   struct TextRec *text_ptr;
{
   MiniLinesInfo *minilines=(&text_ptr->minilines);
   MiniLineInfo *pMiniLine=minilines->first;
   StrBlockInfo *pStrBlock=NULL;

   if (text_ptr->fill == NONEPAT) {
      /*
       * Return FALSE here does not mean that the text object is simple.
       *
       * Since there is no fill, there is no need to measure the width of
       *       text in this case.  Therefore, return FALSE;
       */
      return FALSE;
   }
   if (pMiniLine != minilines->last) return FALSE;

   for (pStrBlock=pMiniLine->first_block; pStrBlock != NULL;
         pStrBlock=pStrBlock->next) {
      if (pStrBlock->type != SB_SIMPLE) return FALSE;
   }
   return TRUE;
}

void DumpSimpleTextPath(FP, ObjPtr)
   FILE *FP;
   struct ObjRec *ObjPtr;
{
   struct TextRec *text_ptr=ObjPtr->detail.t;
   MiniLinesInfo *minilines=(&text_ptr->minilines);
   MiniLineInfo *pMiniLine=minilines->first;
   StrBlockInfo *pStrBlock=NULL;

#ifdef _TGIF_DBG /* debug, do not translate */
   TgAssert(SimpleTextObj(text_ptr),
         "Text is not a simple text object in DumpSimpleTextPath()", NULL);
#endif /* _TGIF_DBG */

   for (pStrBlock=pMiniLine->first_block; pStrBlock != NULL;
         pStrBlock=pStrBlock->next) {
      fprintf(FP, "   /tgifstrw tgifstrw\n");
      DumpStrSeg(pStrBlock->seg, JUST_L, 0, 0, FP, &ObjPtr->obbox, FALSE, 6);
      fprintf(FP, "   add def\n");
   }
}

/* --------------------- DumpMiniLinesInAscii() --------------------- */

static
void DumpStrSegInAscii(FP, pStrSeg, pnSomethingPrinted)
   FILE *FP;
   StrSegInfo *pStrSeg;
   int *pnSomethingPrinted;
{
   if (*pStrSeg->dyn_str.s == '\0') return;

   if (fprintf(FP, "%s", pStrSeg->dyn_str.s) == EOF) {
      writeFileFailed = TRUE;
   }
   if (pnSomethingPrinted != NULL) *pnSomethingPrinted = TRUE;
}

static
void DumpStrBlockInAscii(FP, pStrBlock, pnSomethingPrinted)
   FILE *FP;
   StrBlockInfo *pStrBlock;
   int *pnSomethingPrinted;
{
   switch (pStrBlock->type) {
   case SB_SIMPLE:
      DumpStrSegInAscii(FP, pStrBlock->seg, pnSomethingPrinted);
      break;

   case SB_CHAR_SPACE:
      if (fprintf(FP, " ") == EOF) writeFileFailed = TRUE;
      break;

   case SB_SUPSUB_LEFT:
   case SB_SUPSUB_CENTER:
   case SB_SUPSUB_RIGHT:
      fprintf(FP, "\n");
      if (pStrBlock->type == SB_SUPSUB_CENTER) {
         /* pStrBlock->seg better not be NULL or it will crash */
         DumpStrSegInAscii(FP, pStrBlock->seg, pnSomethingPrinted);
         if (fprintf(FP, "\n") == EOF) writeFileFailed = TRUE;
      }
      if (pStrBlock->sup != NULL) {
         if (!BlankMiniLines(pStrBlock->sup)) {
            DumpMiniLinesInAscii(FP, pStrBlock->sup, pnSomethingPrinted);
         }
      }
      if (pStrBlock->sub != NULL) {
         if (!BlankMiniLines(pStrBlock->sub)) {
            DumpMiniLinesInAscii(FP, pStrBlock->sub, pnSomethingPrinted);
         }
      }
      break;
   }
}

void DumpMiniLineInAscii(FP, pMiniLine, pnSomethingPrinted)
   FILE *FP;
   MiniLineInfo *pMiniLine;
   int *pnSomethingPrinted;
{
   StrBlockInfo *pStrBlock=NULL;

   for (pStrBlock=pMiniLine->first_block; pStrBlock != NULL;
         pStrBlock=pStrBlock->next) {
      DumpStrBlockInAscii(FP, pStrBlock, pnSomethingPrinted);
   }
}

void DumpMiniLinesInAscii(FP, minilines, pnSomethingPrinted)
   FILE *FP;
   MiniLinesInfo *minilines;
   int *pnSomethingPrinted;
{
   MiniLineInfo *pMiniLine=NULL;

   for (pMiniLine=minilines->first; pMiniLine != NULL;
         pMiniLine=pMiniLine->next) {
      DumpMiniLineInAscii(FP, pMiniLine, pnSomethingPrinted);
      if (fprintf(FP, "\n") == EOF) writeFileFailed = TRUE;
   }
}

/* --------------------- ConvertMiniLineToString() --------------------- */

static
void ConvertStrSegToString(pStrSeg, ppsz_buf, pn_len)
   StrSegInfo *pStrSeg;
   char **ppsz_buf;
   int *pn_len;
{
   int len=0;

   if (*pStrSeg->dyn_str.s == '\0') return;

   len = strlen(pStrSeg->dyn_str.s);
   (*ppsz_buf) = realloc(*ppsz_buf, ((*pn_len)+len+1)*sizeof(char));
   if ((*ppsz_buf) == NULL) FailAllocMessage();
   strcpy(&(*ppsz_buf)[*pn_len], pStrSeg->dyn_str.s);
   *pn_len += len;
}

static
int ConvertStrBlockToString(pStrBlock, ppsz_buf, pn_len)
   StrBlockInfo *pStrBlock;
   char **ppsz_buf;
   int *pn_len;
   /* returns TRUE to continue to the next block */
   /* returns FALSE if this string block contains super/subscripts */
{
   switch (pStrBlock->type) {
   case SB_SIMPLE:
      ConvertStrSegToString(pStrBlock->seg, ppsz_buf, pn_len);
      break;

   case SB_CHAR_SPACE:
      if (pStrBlock->special_char_w > 0) {
         /* only generate a space if the width of the thin space is > 0 */
         (*ppsz_buf) = realloc(*ppsz_buf, ((*pn_len)+2)*sizeof(char));
         if ((*ppsz_buf) == NULL) FailAllocMessage();
         strcpy(&(*ppsz_buf)[*pn_len], " ");
         (*pn_len)++;
      }
      break;

   case SB_SUPSUB_LEFT:
   case SB_SUPSUB_CENTER:
   case SB_SUPSUB_RIGHT:
      if (!BlankMiniLines(pStrBlock->sup) || !BlankMiniLines(pStrBlock->sub)) {
         return FALSE;
      }
      if (pStrBlock->type == SB_SUPSUB_CENTER) {
         /* pStrBlock->seg better not be NULL or it will crash */
         ConvertStrSegToString(pStrBlock->seg, ppsz_buf, pn_len);
      }
      break;
   }
   return TRUE;
}

char *ConvertMiniLineToString(pMiniLine, pnNeedToFree)
   MiniLineInfo *pMiniLine;
   int *pnNeedToFree;
   /* must free the returned string with UtilFree() */
{
   char *buf=NULL;
   int len=0;
   StrBlockInfo *pStrBlock=NULL;

   *pnNeedToFree = FALSE;
   if (pMiniLine->first_block->next == NULL) {
      return pMiniLine->first_block->seg->dyn_str.s;
   }
   buf = UtilStrDup(pMiniLine->first_block->seg->dyn_str.s);
   if (buf == NULL) FailAllocMessage();
   len = strlen(buf);
   for (pStrBlock=pMiniLine->first_block->next; pStrBlock != NULL;
         pStrBlock=pStrBlock->next) {
      if (!ConvertStrBlockToString(pStrBlock, &buf, &len)) {
         break;
      }
   }
   *pnNeedToFree = TRUE;

   return buf;
}

/* ----------------- ConvertAttrNameFirstMiniLineToString() ----------------- */

char *ConvertAttrNameFirstMiniLineToString(attr_ptr, pnNeedToFree)
   struct AttrRec *attr_ptr;
   int *pnNeedToFree; /* must not be NULL */
   /* must free the returned string with UtilFree() */
{
   char *tmp_buf=NULL;

   tmp_buf = ConvertMiniLineToString(attr_ptr->obj->detail.t->minilines.first,
         pnNeedToFree);
   if (*attr_ptr->attr_name.s != '\0' && !attr_ptr->nameshown) {
      char *psz=(char*)malloc(sizeof(char) *
            (attr_ptr->attr_name.sz+strlen(tmp_buf)));

      if (psz == NULL) FailAllocMessage();
      sprintf(psz, "%s%s", attr_ptr->attr_name.s, tmp_buf);
      if (*pnNeedToFree) UtilFree(tmp_buf);
      tmp_buf = psz;
      *pnNeedToFree = TRUE;
   }
   return tmp_buf;
}

/* ----------------- MiniLinesInVisibleLayer() ----------------- */

static
int StrSegInVisibleLayer(pStrSeg)
   StrSegInfo *pStrSeg;
{
   return colorLayerOn[pStrSeg->color];
}

static
int StrBlockInVisibleLayer(pStrBlock)
   StrBlockInfo *pStrBlock;
{
   switch (pStrBlock->type) {
   case SB_SIMPLE: return StrSegInVisibleLayer(pStrBlock->seg);
   case SB_CHAR_SPACE: return FALSE;

   case SB_SUPSUB_LEFT:
   case SB_SUPSUB_CENTER:
   case SB_SUPSUB_RIGHT:
      if (pStrBlock->type == SB_SUPSUB_CENTER) {
         if (StrSegInVisibleLayer(pStrBlock->seg)) {
            return TRUE;
         }
      }
      if (pStrBlock->sup != NULL) {
         if (MiniLinesInVisibleLayer(pStrBlock->sup)) {
            return TRUE;
         }
      }
      if (pStrBlock->sub != NULL) {
         if (MiniLinesInVisibleLayer(pStrBlock->sub)) {
            return TRUE;
         }
      }
      break;
   }
   return FALSE;
}

static
int MiniLineInVisibleLayer(pMiniLine)
   MiniLineInfo *pMiniLine;
{
   StrBlockInfo *pStrBlock=NULL;

   for (pStrBlock=pMiniLine->first_block; pStrBlock != NULL;
         pStrBlock=pStrBlock->next) {
      if (StrBlockInVisibleLayer(pStrBlock)) {
         return TRUE;
      }
   }
   return FALSE;
}

int MiniLinesInVisibleLayer(minilines)
   MiniLinesInfo *minilines;
{
   MiniLineInfo *pMiniLine=NULL;

   for (pMiniLine=minilines->first; pMiniLine != NULL;
         pMiniLine=pMiniLine->next) {
      if (MiniLineInVisibleLayer(pMiniLine)) {
         return TRUE;
      }
   }
   return FALSE;
}

