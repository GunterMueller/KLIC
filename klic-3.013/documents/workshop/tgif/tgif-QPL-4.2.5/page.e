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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/page.e,v 1.8 2011/05/16 16:21:58 william Exp $
 */

#ifndef _PAGE_E_
#define _PAGE_E_

extern struct PageRec	* firstPage, * lastPage, * curPage;
extern int		curPageNum, lastPageNum;
extern int		paperRow, paperCol;
extern int		pageLayoutMode;
extern int		pageLineShownInTileMode;

#ifdef _INCLUDE_FROM_PAGE_C_
#undef extern
#define extern
#endif /*_INCLUDE_FROM_PAGE_C_*/

extern void	RedrawPageWindow ARGS_DECL((void));
extern void	MakeCurPageTabVisible ARGS_DECL((void));
extern void	GotoPageNum ARGS_DECL((int));
extern void	SetCurPage ARGS_DECL((int));
extern void	RedrawPageDummyWindow ARGS_DECL((void));
extern void	PageDummyEventHandler ARGS_DECL((XEvent*));
extern void	InitPage ARGS_DECL((void));
extern void	NextPage ARGS_DECL((void));
extern void	PrevPage ARGS_DECL((void));
extern void	GotoPage ARGS_DECL((char*));
extern void	NamePages ARGS_DECL((void));
extern void	EditPageFileNames ARGS_DECL((void));
extern int	OkToFlushUndoBuffer ARGS_DECL((char *msg_prefix));
extern void	AddPageBefore ARGS_DECL((void));
extern void	AddPageAfter ARGS_DECL((void));
extern void	AddPageBeforeCopyAll ARGS_DECL((void));
extern void	AddPageAfterCopyAll ARGS_DECL((void));
extern void	DeleteCurPage ARGS_DECL((void));
extern void	TogglePageLineShown ARGS_DECL((void));
extern void	SpecifyDrawingSize ARGS_DECL((void));
extern void	PrintOnePage ARGS_DECL((void));
extern int	GetDimension ARGS_DECL((char*, int, int*));
extern int	GetDimensionInDouble ARGS_DECL((char*, int, double*));
extern int	SetPaperSize ARGS_DECL((char *));
extern void	SpecifyPaperSize ARGS_DECL((void));
extern void	FreePageSpec ARGS_DECL((SpecifyPagesInfo*));
extern int	ParsePagesSpec ARGS_DECL((char*, SpecifyPagesInfo*));
extern void	DeletePages ARGS_DECL((void));
extern void	SavePagesAs ARGS_DECL((void));
extern void	PrintPages ARGS_DECL((void));
extern void	PrintOneFilePerPage ARGS_DECL((void));
extern int	StackedPageMenu ARGS_DECL((int X, int Y, int TrackMenubar));
extern int	TiledPageMenu ARGS_DECL((int X, int Y, int TrackMenubar));
extern int	RefreshPageMenu ARGS_DECL((TgMenu*));
extern TgMenu	*CreatePageMenu ARGS_DECL((TgMenu*, int X, int Y, TgMenuInfo*,
		                           int status_str_xlated));
extern int	PageMenu ARGS_DECL((int X, int Y, int TrackMenubar));
extern void	DelAllPages ARGS_DECL((void));
extern void	PageLayoutSubMenu ARGS_DECL((int Index));
extern void	RefreshPageLayoutMenu ARGS_DECL((TgMenu*));
extern TgMenu	*CreatePageLayoutMenu ARGS_DECL((TgMenu*, int X, int Y,
		                                 TgMenuInfo*,
		                                 int status_str_xlated));
extern int	PageLayoutMenu ARGS_DECL((int X, int Y, int TrackMenubar));
extern void	PageEventHandler ARGS_DECL((XEvent*));
extern void	CleanUpPage ARGS_DECL((void));
extern void	PushPageInfo ARGS_DECL((void));
extern void	PopPageInfo ARGS_DECL((void));
extern void	GetPageInfo ARGS_DECL((PageInfo*));
extern int	FixPageNumInMiniLines ARGS_DECL((MiniLinesInfo*,
		                                 struct AttrRec *,
		                                 int just_checking));
extern void	FixPageNumbersForSlideShow ARGS_DECL((void));
extern int	NeedToFixPageNumbersForSlideShow ARGS_DECL((void));
extern void	SetPaperColor ARGS_DECL((void));

#ifdef _INCLUDE_FROM_PAGE_C_
#undef extern
#ifndef _NO_RECURSIVE_EXTERN
#define extern extern
#endif /* ~_NO_RECURSIVE_EXTERN */
#endif /*_INCLUDE_FROM_PAGE_C_*/

#endif /*_PAGE_E_*/
