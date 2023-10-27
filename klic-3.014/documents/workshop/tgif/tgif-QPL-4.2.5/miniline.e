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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/miniline.e,v 1.20 2011/05/16 16:21:58 william Exp $
 */

#ifndef _MINILINE_E_
#define _MINILINE_E_

#define PAINT           0x1
#define FRONT_HIGHLIGHT 0x2
#define MID_HIGHLIGHT   0x4
#define BACK_HIGHLIGHT  0x8

#define ERASE 0
#define PAINT_NORM (PAINT)
#define PAINT_INV (PAINT|FRONT_HIGHLIGHT|MID_HIGHLIGHT|BACK_HIGHLIGHT)
#define PAINT_NORM_INV (PAINT|MID_HIGHLIGHT|BACK_HIGHLIGHT)
#define PAINT_INV_NORM (PAINT|FRONT_HIGHLIGHT)
#define PAINT_NORM_INV_NORM (PAINT|MID_HIGHLIGHT)

typedef void (STRSEGFN)ARGS_DECL((StrSegInfo*, void*));

extern TextHighlightInfo	gstTextHighlightInfo;

extern CopyUTF8Info	gstCopyUTF8Info;

extern int		textOrigX, textOrigY, textOrigBaselineY;
extern int		textAbsX, textAbsY, textAbsBaselineY;

extern int		textCurX, textCurY, textCurBaselineY;
extern int		textCurIndex;
extern int		textEndX, textEndY, textEndBaselineY;
extern int		textEndIndex;

extern int		textAbsMinLBearing, textAbsMaxRExtra;
extern int		textW, textH;

extern int		textHighlight;

extern struct ObjRec	*curTextObj;

extern struct BBRec	curTextOBBox, curTextBBox;

extern StrBlockInfo	*curStrBlock; /* block must always be SB_SIMPLE */
extern StrBlockInfo	*endStrBlock; /* block must always be SB_SIMPLE */
extern MiniLineInfo	*firstMiniLine, *lastMiniLine;

extern int		escPressed;

extern int		dontRecalcStrSegMetrics;

extern int		curTextOutlineW, curTextOutlineHalfW;

extern int		drawWinHasFocus;

#ifdef _INCLUDE_FROM_MINILINE_C_
#undef extern
#define extern
#endif /*_INCLUDE_FROM_MINILINE_C_*/

extern void	ClearCopyUTF8Info ARGS_DECL((void));
extern void	SetCopyUTF8FontInfo ARGS_DECL((StrSegInfo*, int double_byte));
extern int	CalcSingleByteInfoForCopyUTF8 ARGS_DECL((int *pn_font_index));
extern int	CanFindSingleByteFontAtCursor ARGS_DECL((StrSegInfo*));
extern int	CanFindDoubleByteFontAtCursor ARGS_DECL((StrSegInfo*));

extern void	SaveDrawTextInfo ARGS_DECL((DrawTextInfo*));
extern void	RestoreDrawTextInfo ARGS_DECL((DrawTextInfo*));
extern void	ResetDrawTextInfo ARGS_DECL((void));
extern void	SaveDrawWinDrawTextInfo ARGS_DECL((int forced));
extern void	RestoreDrawWinDrawTextInfo ARGS_DECL((int forced));

extern void	ResetDirtyBBoxInfo ARGS_DECL((void));
extern void	AddToDirtyBBox ARGS_DECL((struct BBRec *));
extern void	ForceDirtyBBoxToRedrawAll ARGS_DECL((void));
extern int	GetDirtyBBox ARGS_DECL((struct BBRec *));

extern void	CleanUpIndentStrings ARGS_DECL((void));
extern char	*GetIndentString ARGS_DECL((int indent));
extern void	DumpIndentString ARGS_DECL((FILE*, int indent));

extern void	CleanUpMiniLines ARGS_DECL((void));
extern int	InitMiniLines ARGS_DECL((void));

extern void	FreeStrSeg ARGS_DECL((StrSegInfo*));
extern void	FreeStrBlock ARGS_DECL((StrBlockInfo*));
extern void	FreeMiniLine ARGS_DECL((MiniLineInfo*));
extern void	FreeMiniLines ARGS_DECL((MiniLinesInfo*,
		                                 int free_minilines));
extern void	FreeStrBlockList ARGS_DECL((StrBlockInfo*));
extern void	FreeMiniLineList ARGS_DECL((MiniLineInfo*));

extern void	UnlinkStrBlock ARGS_DECL((StrBlockInfo*));
extern void	UnlinkMiniLine ARGS_DECL((MiniLineInfo*));

extern StrSegInfo	*NewStrSeg ARGS_DECL((void));
extern StrBlockInfo	*NewStrBlock ARGS_DECL((void));
extern MiniLineInfo	*NewMiniLine ARGS_DECL((void));
extern MiniLinesInfo	*NewMiniLines ARGS_DECL((void));

extern void	InsertStrBlock ARGS_DECL((StrBlockInfo *pPrevStrBlock,
		                          StrBlockInfo *pNextStrBlock,
		                          StrBlockInfo *pStrBlock,
		                          StrBlockInfo **ppFirstStrBlock,
		                          StrBlockInfo **ppLastStrBlock));
extern void	InsertMiniLine ARGS_DECL((MiniLineInfo *pPrevMiniLine,
		                          MiniLineInfo *pNextMiniLine,
		                          MiniLineInfo *pMiniLine,
		                          MiniLineInfo **ppFirstMiniLine,
		                          MiniLineInfo **ppLastMiniLine));
extern int	BlankStrSeg ARGS_DECL((StrSegInfo*));
extern int	BlankStrBlock ARGS_DECL((StrBlockInfo*));
extern int	BlankMiniLine ARGS_DECL((MiniLineInfo*));
extern int	BlankMiniLines ARGS_DECL((MiniLinesInfo*));

extern StrSegInfo	*GetTextFirstStrSeg ARGS_DECL((struct ObjRec *));
extern struct DynStrRec	*GetTextFirstDynStr ARGS_DECL((struct ObjRec *));

/* ===================== TextRec Routines ===================== */

extern void	CopyCurInfoIntoStrSeg ARGS_DECL((StrBlockInfo*,
		                                 StrSegInfo*));
extern void	CopyCurInfoFromStrSeg ARGS_DECL((StrSegInfo*));
extern void	CopyCurInfoIntoTextPtr ARGS_DECL((struct ObjRec *,
		                                  struct TextRec *));
extern void	CopyCurInfoFromTextPtr ARGS_DECL((struct ObjRec *,
		                                  struct TextRec *));

/* ===================== RecalcTextMetrics ===================== */

extern int	RecalcMiniLineMetrics ARGS_DECL((MiniLineInfo*,
		                                 RecalcMetricsInfo*));
extern int	RecalcMiniLinesMetrics ARGS_DECL((MiniLinesInfo*,
		                                  RecalcMetricsInfo*));
extern int	RecalcTextMetrics ARGS_DECL((struct TextRec *, int x,
		                             int baseline_y));
extern void	UpdateMiniLinesBBoxes ARGS_DECL((MiniLinesInfo*, int x,
		                                 int baseline_y, int clean));
extern void	UpdateCurTextBBoxes ARGS_DECL((int clean));

extern void	DoFuncOnStrSegForMiniLines ARGS_DECL((MiniLinesInfo*,
		                                      STRSEGFN*, void*));

/* ===================== Text Routines ===================== */

extern void	ResetOnCursorKey ARGS_DECL((int drag));
extern void	PutTextCursor ARGS_DECL((void));
extern void	EraseTextCursor ARGS_DECL((void));
extern void	GetCurTextBBoxes ARGS_DECL((struct BBRec *pOBBox,
		                            struct BBRec *pBBox));
extern void	PaintMiniLines ARGS_DECL((Display*, Drawable, GC, int depth,
		                          int x, int baseline_y, MiniLinesInfo*,
		                          int use_highlight, int use_as_mask));
extern void	DrawCurTextOutLine ARGS_DECL((int x, int y, int w, int h));
extern void	RedrawCurText ARGS_DECL((void));
extern void	UpdateHighLightedTextBBoxes ARGS_DECL((int clean));

extern void	MergeNextMiniLineWhileDeleting ARGS_DECL((MiniLineInfo*));
extern int	StrBlocksHasSameProperties ARGS_DECL((StrBlockInfo*,
		                                      StrBlockInfo*));
extern int	MergeStrBlocks ARGS_DECL((StrBlockInfo *pStrBlock,
		                          StrBlockInfo *pNextStrBlock));
extern void	DoInsertThinSpace ARGS_DECL((int thin_space_w));
extern void	InsertHighlightedThinSpace ARGS_DECL((int thin_space_w));
extern void	DeleteHighlightedText ARGS_DECL((void));
extern int	ShrinkMiniLines ARGS_DECL((MiniLinesInfo*, int *pnRetry));

extern int	GetTextXYInMiniLines ARGS_DECL((MiniLinesInfo*, StrBlockInfo*,
		                                int index_to_match,
		                                int *pn_dx, int *pn_dy));
extern void	GetTextXY ARGS_DECL((StrBlockInfo*, int nIndex, int *pn_x,
		                     int *pn_baseline_y));
extern void	SetTextCurXY ARGS_DECL((void));
extern void	SetTextEndXY ARGS_DECL((void));
extern int	CurStrBlockInStrBlock ARGS_DECL((StrBlockInfo*));
extern int	CurStrBlockInMiniLine ARGS_DECL((MiniLineInfo*));
extern int	CurStrBlockInMiniLines ARGS_DECL((MiniLinesInfo*));

extern int	GetCursorPositionInMiniLines ARGS_DECL((MiniLinesInfo*, int dx,
		                                        int dy, int *pn_dx,
		                                        int *pn_dy,
		                                        StrBlockInfo**,
		                                        int *pnIndex));

extern int	SameProperty ARGS_DECL((long lWhich, int nValue, StrSegInfo*,
		                        int nCheckDoubleByte));
extern int	SetStrSegProperty ARGS_DECL((long lWhich, int nValue,
		                             StrSegInfo*));
extern int	SetMiniLinesProperty ARGS_DECL((long lWhich, int nValue,
		                                MiniLinesInfo*));
extern int	ChangeHighlightedTextProperty ARGS_DECL((long lWhich,
		                                         int nValue));
extern int	OnePropertyMiniLines ARGS_DECL((long lWhich, int nValue,
		                                MiniLinesInfo*,
		                                int nCheckDoubleByte));
extern int	SingleFontMiniLines ARGS_DECL((MiniLinesInfo*, int *pnSzUnit,
		                               int *pnDoubleByte, int *pnFont,
		                               int *pnStyle,
		                               int *pnUnderlineOn,
		                               int *pnOverlineOn));
extern int	SingleFontText ARGS_DECL((struct TextRec*, int *pnSzUnit,
		                          int *pnDoubleByte, int *pnFont,
		                          int *pnStyle, int *pnUnderlineOn,
		                          int *pnOverlineOn));
extern int	SingleColorMiniLines ARGS_DECL((MiniLinesInfo*,
		                               int *pnColorIndex));
extern int	SingleColorText ARGS_DECL((struct TextRec*, int *pnColorIndex));
extern int	HighlightedTextHasSameProperty ARGS_DECL((long lWhich,
		                                          int nValue,
		                                          int nChkDoubleByte));
extern int	CheckMiniLinesProperties ARGS_DECL((MiniLinesInfo*,
		                                    int *pn_valid, int *pn_font,
		                                    int *pn_style,
		                                    int *pn_sz_unit,
		                                    int *pn_color,
		                                    int *pn_underline_on,
		                                    int *pn_overline_on));
extern int	CanCopyHighLightedTextAsStrings ARGS_DECL((void));
extern int	CanCopyHighLightedTextAsUTF8Strings ARGS_DECL((
				int *pn_double_byte_font_index));
extern int	CanPasteUTF8StringIntoText ARGS_DECL((StrSegInfo*));
extern void	GatherHighLightedTextAsStrings ARGS_DECL((char **ppsz_buf,
		                                          int *pn_buf_sz));
extern struct ObjRec	*CreateTextObjFromHighLightedText ARGS_DECL((void));

extern void	ReorderCursorStrBlocks ARGS_DECL((void));
extern int	FindStringInMiniLines ARGS_DECL((MiniLinesInfo*,
		                                 int *pn_found_starting_point,
		                                 char *str_to_match,
		                                 int str_len_to_match,
		                                 int case_sensitive,
		                                 StrBlockInfo **ppStartStrBlock,
		                                 int *pn_start_index,
		                                 StrBlockInfo **ppEndStrBlock,
		                                 int *pn_end_index));

extern int	CanBreakMiniLinesIntoWords ARGS_DECL((MiniLinesInfo*));
extern void	BreakMiniLines ARGS_DECL((MiniLinesInfo*, int how,
		                          int x, int baseline_y,
		                          struct ObjRec *prototype,
		                          int tx_to_move, int ty_to_move,
		                          ObjListInfo*));
extern void	DumpMiniLines ARGS_DECL((MiniLinesInfo*, int x, int baseline_y,
		                         FILE*, struct BBRec *, int do_dump,
		                         int indent));
extern int	SimpleTextObj ARGS_DECL((struct TextRec *));
extern void	DumpSimpleTextPath ARGS_DECL((FILE*, struct ObjRec *));
extern void	DumpMiniLineInAscii ARGS_DECL((FILE*, MiniLineInfo*,
		                               int *pnSomethingPrinted));
extern void	DumpMiniLinesInAscii ARGS_DECL((FILE*, MiniLinesInfo*,
		                                int *pnSomethingPrinted));
extern char	*ConvertMiniLineToString ARGS_DECL((MiniLineInfo*,
		                                    int *pnNeedToFree));
extern char	*ConvertAttrNameFirstMiniLineToString ARGS_DECL((
				struct AttrRec *, int *pnNeedToFree));
extern int	MiniLinesInVisibleLayer ARGS_DECL((MiniLinesInfo*));

#ifdef _INCLUDE_FROM_MINILINE_C_
#undef extern
#ifndef _NO_RECURSIVE_EXTERN
#define extern extern
#endif /* ~_NO_RECURSIVE_EXTERN */
#endif /*_INCLUDE_FROM_MINILINE_C_*/

#endif /*_MINILINE_E_*/
