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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/grid.e,v 1.10 2011/05/16 16:21:57 william Exp $
 */

#ifndef _GRID_E_
#define _GRID_E_

extern int	gridSystem;
extern int	snapOn;
extern int	xyEnglishGrid;
extern int	xyMetricGrid;
extern int	pageStyle;
extern int	whereToPrint;
extern int	moveMode;
extern int	gridShown;
extern int	mapShown;
extern int	usePaperSizeStoredInFile;
extern int	oneMotionSelectMove;
extern int	queryZoomInPoint;

extern int	shiftForDiagMouseMove;
extern int	useRecentForDiagMouseMove;

extern int	defRightMargin;
extern int	defRightMarginEnabled;
extern int	rightMargin;
extern int	rightMarginEnabled;
extern int	rightMarginActive;

#ifdef _INCLUDE_FROM_GRID_C_
#undef extern
#define extern
#endif /*_INCLUDE_FROM_GRID_C_*/

extern void	RedrawGridLines ARGS_DECL((Window));
extern void	DrawGridLines ARGS_DECL((Window, int LtX, int LtY, int W,
		                         int H));
extern void	RedrawPageLines ARGS_DECL((Window));
extern void	DrawPageLines ARGS_DECL((Window, int LtX, int LtY, int W,
		                         int H));
extern void	ToggleRightMargin ARGS_DECL((void));
extern void	SpecifyRightMargin ARGS_DECL((void));
extern void	ToggleGridSystem ARGS_DECL((void));
extern void	IncGrid ARGS_DECL((void));
extern void	DecGrid ARGS_DECL((void));
extern void	ToggleGridShown ARGS_DECL((void));
extern void	ToggleSnapOn ARGS_DECL((void));
extern void	ToggleColorPostScript ARGS_DECL((void));
extern void	ToggleMoveMode ARGS_DECL((void));
extern void	ToggleMapShown ARGS_DECL((void));
extern void	ToggleUseGray ARGS_DECL((void));
extern void	SetMeasureUnit ARGS_DECL((void));
extern void	ToggleShowMenubar ARGS_DECL((void));
extern void	ToggleShowStatus ARGS_DECL((void));
extern void	ToggleShowChoicebar ARGS_DECL((void));
extern void	ToggleShowMode ARGS_DECL((void));
extern void	SetWhereToPrint ARGS_DECL((int nIndex));
extern void	ToggleWhereToPrint ARGS_DECL((void));
extern void	ToggleOneMotionSelectMove ARGS_DECL((void));
extern void	ToggleColorLayers ARGS_DECL((void));
extern void	ToggleStretchableText ARGS_DECL((void));
extern void	ToggleTransPatMode ARGS_DECL((void));
extern void	DefaultZoom ARGS_DECL((void));
extern void	ZoomIn ARGS_DECL((void));
extern void	ZoomInAtCursor ARGS_DECL((int AbsX, int AbsY));
extern void	ZoomWayOut ARGS_DECL((void));
extern void	ZoomOut ARGS_DECL((void));
extern void	CenterAtCursor ARGS_DECL((int AbsX, int AbsY));
extern void	PreciseZoom ARGS_DECL((int ZoomedIn, int ZoomScale, int Force));
extern void	SetPSPageWidthHeight ARGS_DECL((void));
extern void	ResetOnePageSize ARGS_DECL((void));
extern int	UpdPageStyle ARGS_DECL((int));
extern void	ChangePageStyle ARGS_DECL((int PageStyle));
extern int	RefreshPageStyleMenu ARGS_DECL((TgMenu*));
extern TgMenu	*CreatePageStyleMenu ARGS_DECL((TgMenu*, int X, int Y,
		                                TgMenuInfo*,
		                                int status_str_xlated));
extern int	PageStyleMenu ARGS_DECL((int X, int Y, int TrackMenubar));
extern void	RefreshExportFormatMenu ARGS_DECL((TgMenu*));
extern TgMenu	*CreateExportFormatMenu ARGS_DECL((TgMenu*, int X, int Y,
		                                   TgMenuInfo*,
		                                   int status_str_xlated));
extern int	ExportFormatMenu ARGS_DECL((int X, int Y, int TrackMenubar));
extern int	RefreshShowHideMenu ARGS_DECL((TgMenu*));
extern TgMenu	*CreateShowHideMenu ARGS_DECL((TgMenu*, int X, int Y,
		                               TgMenuInfo*,
		                               int status_str_xlated));
extern int	RefreshLayoutMenu ARGS_DECL((TgMenu*));
extern TgMenu	*CreateLayoutMenu ARGS_DECL((TgMenu*, int X, int Y,
		                             TgMenuInfo*,
		                             int status_str_xlated));
extern int	LayoutMenu ARGS_DECL((int X, int Y, int TrackMenubar));
extern int	GridSize ARGS_DECL((void));
extern void	GridXY ARGS_DECL((int X, int Y, int *GridX, int *GridY));
extern int	ControlKeyPressedEvent ARGS_DECL((XEvent *ev));
extern int	DiagEventCheck ARGS_DECL((XEvent *ev));
extern void	DiagGridXY ARGS_DECL((int OrigX, int OrigY, int *X, int *Y));
extern void	CleanUpGrids ARGS_DECL((void));
extern void	MoveModeSubMenu ARGS_DECL((int Index));
extern void	RefreshMoveModeMenu ARGS_DECL((TgMenu*));
extern TgMenu	*CreateMoveModeMenu ARGS_DECL((TgMenu*, int X, int Y,
		                               TgMenuInfo*,
		                               int status_str_xlated));
extern int	MoveModeMenu ARGS_DECL((int X, int Y, int TrackMenubar));
extern void	StretchableTextModeSubMenu ARGS_DECL((int Index));
extern void	RefreshStretchableTextModeMenu ARGS_DECL((TgMenu*));
extern TgMenu	*CreateStretchableTextModeMenu ARGS_DECL((TgMenu*, int X, int Y,
				TgMenuInfo*, int status_str_xlated));
extern int	StretchableTextModeMenu ARGS_DECL((int X, int Y,
		                                   int TrackMenubar));
extern void	RefreshTransPatModeMenu ARGS_DECL((TgMenu*));
extern TgMenu	*CreateTransPatModeMenu ARGS_DECL((TgMenu*, int X, int Y,
		                                   TgMenuInfo*,
		                                   int status_str_xlated));
extern int	TransPatModeMenu ARGS_DECL((int X, int Y, int TrackMenubar));
extern int	RefreshGridMenu ARGS_DECL((TgMenu*));
extern TgMenu	*CreateGridMenu ARGS_DECL((TgMenu*, int X, int Y, TgMenuInfo*,
		                           int status_str_xlated));
extern int	GridMenu ARGS_DECL((int X, int Y, int TrackMenubar));

#ifdef _INCLUDE_FROM_GRID_C_
#undef extern
#ifndef _NO_RECURSIVE_EXTERN
#define extern extern
#endif /* ~_NO_RECURSIVE_EXTERN */
#endif /*_INCLUDE_FROM_GRID_C_*/

#endif /*_GRID_E_*/
