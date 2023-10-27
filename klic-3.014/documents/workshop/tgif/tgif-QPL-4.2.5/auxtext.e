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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/auxtext.e,v 1.7 2011/05/16 16:21:56 william Exp $
 */

#ifndef _AUXTEXT_E_
#define _AUXTEXT_E_

extern GC	rotateGC;
extern Pixmap	textBackingBitmap;
extern Pixmap	textBackingBgBitmap;
extern Pixmap	textBackingPixmap;
extern int	textBackingBitmapW;
extern int	textBackingBitmapH;
extern int	doubleQuoteDoubleQuote;
extern int	groupedTextEditable;

extern struct SelRec	* outerSel, * innerSel;

#ifdef _INCLUDE_FROM_AUXTEXT_C_
#undef extern
#define extern
#endif /*_INCLUDE_FROM_AUXTEXT_C_*/

extern void	GetStrSizeInfo ARGS_DECL((struct DynStrRec *, int *w,
		                          int *lbearing, int *rbearing));
extern int	GetNumberOfMiniLines ARGS_DECL((MiniLinesInfo*));
extern int	IsHex ARGS_DECL((char, int*));
extern int	InputOctalString ARGS_DECL((char *pszBuf, int *pnBufSize));
extern int	CharIsCntrlINS ARGS_DECL((XKeyEvent*, KeySym));
extern int	CharIsShiftINS ARGS_DECL((XKeyEvent*, KeySym));
extern int	CharIsCntrlShiftINS ARGS_DECL((XKeyEvent*, KeySym));
extern int	CharIsESC ARGS_DECL((XKeyEvent*, char*, KeySym, int *pnHasCh));
extern int	CharIsCRorLF ARGS_DECL((XKeyEvent*, char*, KeySym,
		                        int *pnHasCh));
extern int	CharIsDEL ARGS_DECL((XKeyEvent*, char*, KeySym, int *pnHasCh));
extern int	CharIsBS ARGS_DECL((XKeyEvent*, char*, KeySym, int *pnHasCh,
                                    int LeftIsBS));
extern int	CharIsBSorDEL ARGS_DECL((XKeyEvent*, char*, KeySym,
		                         int *pnHasCh, int LeftIsBS));
extern int	CharIsTAB ARGS_DECL((XKeyEvent*, char*, KeySym, int *pnHasCh));
extern int	CharIsHome ARGS_DECL((XKeyEvent*, char*, KeySym, int *pnHasCh));
extern int	CharIsEnd ARGS_DECL((XKeyEvent*, char*, KeySym, int *pnHasCh));
extern int	CharIsCntrlSpace ARGS_DECL((XKeyEvent*, char*, KeySym,
		                            int *pnHasCh));
extern void	TranslateKeys ARGS_DECL((char *, KeySym *));
extern void	SetTextOBBox ARGS_DECL((struct ObjRec *, int Just,
		                        int W, int H, int LBearing,
		                        int RightExtra, int Rotate));
extern void	SetTextBBox ARGS_DECL((struct ObjRec *, int Just,
		                       int W, int H, int LBearing,
		                       int RightExtra, int Rotate));
extern void	SetTextOrigBBoxes ARGS_DECL((struct ObjRec *, int Just,
		                             int W, int H, int LBearing,
		                             int RightExtra, int Rotate));
extern int	UpdTextBBox ARGS_DECL((struct ObjRec *));
extern int	PixelOnOff ARGS_DECL((XImage *, int Col, int Row, int Scale));
extern void	MakeCachedTextBitmap ARGS_DECL((struct ObjRec *));
extern void	CleanOuterInnerSel ARGS_DECL((void));
extern void	UpdateOuterInnerSel ARGS_DECL((struct ObjRec *));
extern struct ObjRec	* FindTextObj ARGS_DECL((int XOff, int YOff,
			                         struct ObjRec *ObjToBeFound));
extern void	UnlinkCurTextFromInnerSel ARGS_DECL((void));
extern void	AdjAncestorsBBox ARGS_DECL((void));
extern void	DumpTextObj ARGS_DECL((FILE *, struct ObjRec *));
extern int	NeedToCacheMiniLines ARGS_DECL((MiniLinesInfo*));
extern int	NeedsToCacheTextObj ARGS_DECL((struct ObjRec *));
extern void	SaveString ARGS_DECL((FILE *, char *));
extern void	SaveDoubleByteString ARGS_DECL((FILE *, char *));
extern void	SaveMiniLines ARGS_DECL((FILE*, MiniLinesInfo*));
extern void	CombineOverUnderYOffsets ARGS_DECL((int underline_y_offset,
		                                    int overline_y_offset,
		                                    int *pn_combined));
extern void	UncombineOverUnderYOffsets ARGS_DECL((int combined,
				int *pn_underline_y_offset,
				int *pn_overline_y_offset));
extern void	SaveTextObj ARGS_DECL((FILE *, struct ObjRec *));
extern void	SaveHexString ARGS_DECL((FILE *, char *));
extern char	* ReadString ARGS_DECL((char *));
extern StrSegInfo	* CreateStrSegFromString ARGS_DECL((char*,
			                                    StrBlockInfo*));
extern StrBlockInfo	* CreateStrBlockFromString ARGS_DECL((char*,
			                                      MiniLineInfo*));
extern MiniLineInfo	* CreateMiniLineFromString ARGS_DECL((char*,
			                                      MiniLineInfo**,
			                                      MiniLineInfo**));
extern struct ObjRec	* FormTextObjectFromFile ARGS_DECL((FILE *, int AbsX,
			                                    int AbsBaselineY));
extern void	RepaintFirstStr ARGS_DECL((struct ObjRec *, char *));

#ifdef _INCLUDE_FROM_AUXTEXT_C_
#undef extern
#ifndef _NO_RECURSIVE_EXTERN
#define extern extern
#endif /* ~_NO_RECURSIVE_EXTERN */
#endif /*_INCLUDE_FROM_AUXTEXT_C_*/

#endif /*_AUXTEXT_E_*/
