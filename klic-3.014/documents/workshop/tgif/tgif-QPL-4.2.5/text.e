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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/text.e,v 1.17 2011/05/16 16:22:00 william Exp $
 */

#ifndef _TEXT_E_
#define _TEXT_E_

extern int		textDrawn;
extern int		curTextModified;
extern int		textVSpace;

extern int		textJust;
extern int		textCursorShown;
extern int		textCursorH;
extern int		editingText;
extern int		curTextIsNew;

extern int		editTextSize;

extern int		readingTextObject;

extern float		scriptFraction;
extern char		scriptFractionStr[];

extern int		deleteNextCharWithDelKey;
extern int		lengthLimit256InInsertChar;

#ifdef _INCLUDE_FROM_TEXT_C_
#undef extern
#define extern
#endif /*_INCLUDE_FROM_TEXT_C_*/

/* DynStr routines */
extern void		FreeDynStrBuf ARGS_DECL((struct DynStrRec *));
extern void		DynStrCpy ARGS_DECL((struct DynStrRec *Dest,
			                     struct DynStrRec *Src));
extern struct DynStrRec	* NewDynStr ARGS_DECL((void));
extern struct DynStrRec	* DynStrDup ARGS_DECL((struct DynStrRec *));
extern void		DynStrSet ARGS_DECL((struct DynStrRec *, char*));

/* Str routines */
extern void		FreeStr ARGS_DECL((struct StrRec *));
extern void		FreeStrList ARGS_DECL((struct StrRec *));
extern struct StrRec	* NewStr ARGS_DECL((void));

/* text routines */
extern void	SetTextHighlight ARGS_DECL((void));
extern void	SaveCursorPositionInCurText ARGS_DECL((void));
extern void	RestoreCursorPositionInCurText ARGS_DECL((void));
extern void	InvalidateTextCache ARGS_DECL((struct TextRec *));
extern void	TellTextCursorPosition ARGS_DECL((int *pnX, int *pnY));

extern void	BlurText ARGS_DECL((Window, GC, int XOff, int YOff,
		                    int W, int H));

extern void	InitText ARGS_DECL((void));
extern void	CleanUpText ARGS_DECL((void));
extern void	NewCurText ARGS_DECL((void));
extern void	FreeTextObj ARGS_DECL((struct ObjRec *));
extern void	SetEditTextSizeValue ARGS_DECL((int edit_text_size));
extern void	UpdateTextInfoChoices ARGS_DECL((int forced));
extern int	RestoreEditTextSize ARGS_DECL((struct ObjRec *, int restore));
extern int	ShouldRightMarginBeActive ARGS_DECL((void));
extern void	SetRightMarginActive ARGS_DECL((void));
extern int	CreateTextObj ARGS_DECL((int nDeactivateIM, int nRedraw));
extern int	AtBeginningOfInheritedAttrValue ARGS_DECL((struct ObjRec *,
		                                           StrBlockInfo*, int));
extern void	HighLightJustDrawnText ARGS_DECL((void));
extern void	DelSelText ARGS_DECL((void));
extern void	SwitchTextCurAndEndStrBlocks ARGS_DECL((void));
extern int	FixHighlightedStrBlockDepths ARGS_DECL((void));
extern int	SwitchToSingleByteFont ARGS_DECL((StrSegInfo*));
extern int	SwitchToDoubleByteFont ARGS_DECL((StrSegInfo*));
extern void	HandleInputFromBuffer ARGS_DECL((char*));
extern void	HandleTextFromBuffer ARGS_DECL((XKeyEvent*, char*, KeySym,
		                                int has_ch));
extern int	HandleDoubleByteUTF8Chars ARGS_DECL((char *buf, int has_ch));
extern void	DrawText ARGS_DECL((XEvent *));
extern void	InsertHexOctalChar ARGS_DECL((void));
extern void	InsertSymbol ARGS_DECL((void));
extern void	EditTextInAttr ARGS_DECL((struct AttrRec *));
extern void	DumpOneStr ARGS_DECL((FILE*, int FontIndex,
		                      int DoubleByte, int DoubleByteModBytes, char*));
extern void	DrawTextObj ARGS_DECL((Window, int XOff, int YOff,
		                       struct ObjRec *));
extern int	ReadMiniLines ARGS_DECL((FILE *, StrBlockInfo*,
		                         MiniLinesInfo**));
extern void	ReadTextObj ARGS_DECL((FILE *, char *Inbuf, struct ObjRec **));
extern void	UpdCurTextBBox ARGS_DECL((void));
extern void	AdjustCurText ARGS_DECL((int XOff, int YOff));
extern void	PrepareZoomCurText ARGS_DECL((int *AbsXc, int *AbsYc));
extern void	PreZoomCurText ARGS_DECL((void));
extern void	PostZoomCurText ARGS_DECL((int AbsXc, int AbsYc));
extern void	HighLightText ARGS_DECL((struct ObjRec *ObjPtr,
		                         StrBlockInfo *pStartStrBlock,
                                         int start_index,
		                         StrBlockInfo *pEndStrBlock,
                                         int end_index));
extern int	FindTextInCurTextObj ARGS_DECL((char *str_to_match, int str_len,
		                                int case_sensitive,
		                                StrBlockInfo **ppStartStrBlock,
		                                int *pn_start_index,
		                                StrBlockInfo **ppEndStrBlock,
		                                int *pn_end_index));

extern int	SetScriptFractionValue ARGS_DECL((char*));
extern void	SetScriptFraction ARGS_DECL((void));
extern void	InsertThinSpace ARGS_DECL((void));
extern void	InsertVerticalOffset ARGS_DECL((void));
extern void	RemoveVerticalOffset ARGS_DECL((void));
extern void	InsertRightSuperscript ARGS_DECL((void));
extern void	InsertRightSubscript ARGS_DECL((void));
extern void	InsertLeftSuperscript ARGS_DECL((void));
extern void	InsertLeftSubscript ARGS_DECL((void));
extern void	InsertCenterSuperscript ARGS_DECL((void));
extern void	InsertCenterSubscript ARGS_DECL((void));

extern int	RefreshEditTextMenu ARGS_DECL((TgMenu*));
extern TgMenu	*CreateEditTextMenu ARGS_DECL((TgMenu*, int X, int Y,
		                               TgMenuInfo*,
		                               int status_str_xlated));

extern int	ChangeEditTextProperty ARGS_DECL((long lWhich, int nValue));
extern void	PasteMiniLinesAtCursor ARGS_DECL((struct ObjRec *));
extern int	MouseInCurText ARGS_DECL((XEvent*));
extern int	MouseOnCurTextBoundary ARGS_DECL((XEvent*));
extern void	MoveEditText ARGS_DECL((XEvent*));
extern void	MoveEditTextBox ARGS_DECL((void));

#ifdef _INCLUDE_FROM_TEXT_C_
#undef extern
#ifndef _NO_RECURSIVE_EXTERN
#define extern extern
#endif /* ~_NO_RECURSIVE_EXTERN */
#endif /*_INCLUDE_FROM_TEXT_C_*/

#endif /*_TEXT_E_*/
