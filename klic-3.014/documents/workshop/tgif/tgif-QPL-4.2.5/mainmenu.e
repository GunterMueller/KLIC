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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/mainmenu.e,v 1.6 2011/05/16 16:21:58 william Exp $
 */

#ifndef _MAINMENU_E_
#define _MAINMENU_E_

typedef int (ENUMPOPUPMENUFN)ARGS_DECL((int, Window, void*));

extern int	pinnedMainMenu;
extern int	mainMenuPinDistance;
extern Window	mainMenuWindow;

extern int	numStacking;
extern Window	* stackingWins;

extern int	titledPinnedMenu;
extern int	btn3PopupModeMenu;

#ifdef _INCLUDE_FROM_MAINMENU_C_
#undef extern
#define extern
#endif /*_INCLUDE_FROM_MAINMENU_C_*/

extern void	InitMainMenu ARGS_DECL((void));
extern void	CleanUpMainMenu ARGS_DECL((void));
extern void	SaveMainWinPosition ARGS_DECL((unsigned int X, unsigned int Y));
extern void	MoveMainMenuWindow ARGS_DECL((unsigned int X, unsigned int Y));
extern void	GetPopupXY ARGS_DECL((Window, int *px, int *py));
extern void	GetPopupWH ARGS_DECL((Window, int *pw, int *ph));
extern void	SaveStackingOrder ARGS_DECL((void));
extern Window	TgRealizePinnedMenuWindow ARGS_DECL((TgMenu*, int X, int Y,
		                                     int W, int H));
extern int	TgWindowIsPinnedMenu ARGS_DECL((Window, int Index));
extern int	TgHandlePinnedMenuEvent ARGS_DECL((Window, int Index, XEvent*));
extern void	UpdatePinnedMenu ARGS_DECL((int Index));
extern void	UpdateAllPinnedMenus ARGS_DECL((void));
extern void	UseWireMenuItemInModeItem ARGS_DECL((int connecting));
extern void	DestroyPinnedMenu ARGS_DECL((int Index));
extern void	HidePopupMenusForSlideShow ARGS_DECL((void));
extern void	ShowPopupMenusForSlideShow ARGS_DECL((void));
extern void	EnumPopupMenuWindow ARGS_DECL((ENUMPOPUPMENUFN*, void*));

#ifdef _INCLUDE_FROM_MAINMENU_C_
#undef extern
#ifndef _NO_RECURSIVE_EXTERN
#define extern extern
#endif /* ~_NO_RECURSIVE_EXTERN */
#endif /*_INCLUDE_FROM_MAINMENU_C_*/

#endif /*_MAINMENU_E_*/
