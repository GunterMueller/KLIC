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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/scroll.e,v 1.6 2011/05/16 16:21:59 william Exp $
 */

#ifndef _SCROLL_E_
#define _SCROLL_E_

extern int	autoPan;

extern int	scrollingCanvas;
extern int	smoothScrollingCanvas;

extern int	resetOriginOnAdvancePage;

#ifdef _INCLUDE_FROM_SCROLL_C_
#undef extern
#define extern
#endif /*_INCLUDE_FROM_SCROLL_C_*/

extern int	TgAnyButtonDown ARGS_DECL((Display*, Window));
extern int	TgPressButtonLoop ARGS_DECL((Display*, Window, struct BBRec *,
		                             ScrollBtnCallbackInfo*));
extern int	TgGetScrollHit ARGS_DECL((int x, int y, int orientation,
		                          int scroll_area_w, int scroll_area_h,
		                          double start_frac, int length,
		                          int total, int *pn_offset));
extern void	TgDrawScrollBar ARGS_DECL((Display*, Window, int orientation,
		                           int x_off, int y_off,
		                           int scroll_area_w, int scroll_area_h,
		                           double start_frac, int length,
		                           int total));
extern void	RedrawVScrollWindow ARGS_DECL((void));
extern void	RedrawHScrollWindow ARGS_DECL((void));
extern void	RedrawScrollBars ARGS_DECL((void));
extern void	GetMaxScrollOrigin ARGS_DECL((int *pn_orig_x, int *pn_orig_y));
extern void	ScrollTo ARGS_DECL((int XOff, int YOff));
extern void	ScrollUp ARGS_DECL((XButtonEvent *));
extern void	ForceScrollDown ARGS_DECL((int));
extern void	ScrollDown ARGS_DECL((XButtonEvent *));
extern void	ScrollLeft ARGS_DECL((XButtonEvent *));
extern void	ScrollRight ARGS_DECL((XButtonEvent *));
extern void	ScrollEventHandler ARGS_DECL((XEvent *));
extern void	ScrollToSpecifiedOrigin ARGS_DECL((int page_num, int orig_x,
		                                   int orig_y, int zoom_scale,
		                                   int zoomed_in));
extern int	ScrollToOrigin ARGS_DECL((void));
extern void	SaveOrigin ARGS_DECL((void));
extern void	SetOrigin ARGS_DECL((int x, int y, int zoom_scale,
		                     int zoomed_in));
extern void	GetOrigin ARGS_DECL((int *pn_x, int *pn_y, int *pn_zoom_scale,
		                     int *pn_zoomed_in));
extern void	CleanUpScrolls ARGS_DECL((void));
extern void	UpdScrollWinWH ARGS_DECL((void));
extern void	InitScroll ARGS_DECL((void));
extern void	ChangeScrollMode ARGS_DECL((int ModeIndex));
extern int	RefreshScrollModeMenu ARGS_DECL((TgMenu*));
extern TgMenu	*CreateScrollModeMenu ARGS_DECL((TgMenu*, int X, int Y,
                                                 TgMenuInfo*,
                                                 int status_str_xlated));

#ifdef _INCLUDE_FROM_SCROLL_C_
#undef extern
#ifndef _NO_RECURSIVE_EXTERN
#define extern extern
#endif /* ~_NO_RECURSIVE_EXTERN */
#endif /*_INCLUDE_FROM_SCROLL_C_*/

#endif /*_SCROLL_E_*/
