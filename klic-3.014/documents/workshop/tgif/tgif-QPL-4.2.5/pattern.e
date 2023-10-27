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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/pattern.e,v 1.7 2011/05/16 16:21:58 william Exp $
 */

#ifndef _PATTERN_E_
#define _PATTERN_E_

extern int	objFill;
extern int	transPat;
extern int	lineStyle;
extern int	lineWidth;
extern int	penPat;
extern int	curSpline;
extern int	curDash;
extern int	rcbRadius;
extern int	useGray;
extern char	patternStr[];

extern int	stickyMenuSelection;

#ifdef _INCLUDE_FROM_PATTERN_C_
#undef extern
#define extern
#endif /*_INCLUDE_FROM_PATTERN_C_*/

extern void	ResetGrayDetection ARGS_DECL((void));
extern char	* GrayStr ARGS_DECL((int Index));
extern void	GrayCheck ARGS_DECL((int Index));
extern void	EndGrayDetection ARGS_DECL((void));

extern int	RefreshModeMenu ARGS_DECL((TgMenu*));
extern TgMenu	*CreateModeMenu ARGS_DECL((TgMenu*, int X, int Y, TgMenuInfo*,
		                           int status_str_xlated));
extern int	ModeMenu ARGS_DECL((int X, int Y, int TrackMenubar));

extern int	ChangeObjTransPat ARGS_DECL((struct ObjRec *, int FillIndex));
extern void	ChangeAllSelTransPat ARGS_DECL((int Index, int HighLight));

extern int	ChangeObjFill ARGS_DECL((struct ObjRec *, int FillIndex));
extern void	ChangeAllSelFill ARGS_DECL((int Index, int HighLight));
extern void	RefreshFillMenu ARGS_DECL((TgMenu*));
extern TgMenu	*CreateFillMenu ARGS_DECL((TgMenu*, int X, int Y, TgMenuInfo*,
		                           int status_str_xlated));
extern int	FillMenu ARGS_DECL((int X, int Y, int TrackMenubar));

extern void	ChangeAllSelLineStyle ARGS_DECL((int Index, int HighLight));
extern void	RefreshLineStyleMenu ARGS_DECL((TgMenu*));
extern TgMenu	*CreateLineStyleMenu ARGS_DECL((TgMenu*, int X, int Y,
		                                TgMenuInfo*,
		                                int status_str_xlated));
extern int	LineStyleMenu ARGS_DECL((int X, int Y, int TrackMenubar));

extern int	GetPolyOrPolygonControlPoints ARGS_DECL((struct ObjRec *,
				struct PolyRec **, struct PolygonRec **,
				int *pn_curved, int *pn_num_pts, IntPoint**,
				char **ppsz_smooth));
extern void	ChangeAllSelLineType ARGS_DECL((int Index, int HighLight));
extern void	RefreshLineTypeMenu ARGS_DECL((TgMenu*));
extern TgMenu	*CreateLineTypeMenu ARGS_DECL((TgMenu*, int X, int Y,
		                               TgMenuInfo*,
		                               int status_str_xlated));
extern int	LineTypeMenu ARGS_DECL((int X, int Y, int TrackMenubar));

extern void	ChangeAllSelLineWidth ARGS_DECL((int Index, int HighLight));
extern int	GetBestLineWidthIndex ARGS_DECL((int width, int aw, int ah));
extern void	RefreshLineWidthMenu ARGS_DECL((TgMenu*));
extern TgMenu	*CreateLineWidthMenu ARGS_DECL((TgMenu*, int X, int Y,
		                                TgMenuInfo*,
		                                int status_str_xlated));
extern int	LineWidthMenu ARGS_DECL((int X, int Y, int TrackMenubar));

extern void	ChangeAllSelLineDash ARGS_DECL((int Index, int HighLight));
extern void	RefreshLineDashMenu ARGS_DECL((TgMenu*));
extern TgMenu	*CreateLineDashMenu ARGS_DECL((TgMenu*, int X, int Y,
		                                TgMenuInfo*,
		                                int status_str_xlated));
extern int	LineDashMenu ARGS_DECL((int X, int Y, int TrackMenubar));

extern int	ChangeObjPen ARGS_DECL((struct ObjRec *, int PenIndex));
extern void	ChangeAllSelPen ARGS_DECL((int Index, int HighLight));
extern void	RefreshPenMenu ARGS_DECL((TgMenu*));
extern TgMenu	*CreatePenMenu ARGS_DECL((TgMenu*, int X, int Y, TgMenuInfo*,
		                          int status_str_xlated));
extern int	PenMenu ARGS_DECL((int X, int Y, int TrackMenubar));

extern void	ToggleAllSelLineType ARGS_DECL((void));
extern void	ChangeAllSelRCBRadius ARGS_DECL((int Radius));
extern void	UpdateSelObjs ARGS_DECL((void));
extern void	ChangeAllSelRealLineWidth ARGS_DECL((int nMask,
		                                     int Width, int AW, int AH,
		                                     char*, char*, char*,
		                                     int HighLight));
extern void	SetSelLineWidth ARGS_DECL((char*));

#ifdef _INCLUDE_FROM_PATTERN_C_
#undef extern
#ifndef _NO_RECURSIVE_EXTERN
#define extern extern
#endif /* ~_NO_RECURSIVE_EXTERN */
#endif /*_INCLUDE_FROM_PATTERN_C_*/

#endif /*_PATTERN_E_*/
