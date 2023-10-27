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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/menu.e,v 1.6 2011/05/16 16:21:58 william Exp $
 */

#ifndef _MENU_E_
#define _MENU_E_

typedef struct tagMenuDontSendCommandInfo {
   int dont_send_command;
   int selected_index; /* equals INVALID if nothing is selected */
} MenuDontSendCommandInfo;

extern int	iconWindowShown;
extern int	iconWindowCreated;
extern int	importingIconFile;
extern int	showVersion;
extern int	activeMenu;
extern GC	textMenuGC;
extern GC	rvPixmapMenuGC;
extern int	menuRowsBeforeScroll;
extern int	menuColsBeforeScroll;

extern int	maxScrollableMenuWidth;
extern int	maxScrollableMenuHeight;

extern int	deleteCmdAsCut;

extern MenuDontSendCommandInfo	gstMenuDontSendCommandInfo;

#ifdef _INCLUDE_FROM_MENU_C_
#undef extern
#define extern
#endif /*_INCLUDE_FROM_MENU_C_*/

extern void	SendCommandToSelf ARGS_DECL((int nCmdId, int nIndex));
extern TgMenuItem	*FindMenuItemByCmdId ARGS_DECL((TgMenu*, int cmdid));
extern TgMenuItem	*FindMenuItemByIndex ARGS_DECL((TgMenu*, int index));
extern TgMenuItem	*FindMenuItemBySubMenuInfoPtr ARGS_DECL((TgMenu*,
			                                         TgMenuInfo*));
extern int	TgSetMenuItemCheckById ARGS_DECL((TgMenu*, int cmdid,
		                                  int checked));
extern int	TgSetMenuItemRadioById ARGS_DECL((TgMenu*, int cmdid,
		                                  int checked));
extern int	TgIsMenuItemChecked ARGS_DECL((TgMenu*, int index));
extern int	TgIsMenuItemRadio ARGS_DECL((TgMenu*, int index));
extern int	TgIsMenuItemEnabled ARGS_DECL((TgMenu*, int index));
extern int	TgEnableMenuItemById ARGS_DECL((TgMenu*, int cmdid,
		                                int enabled));
extern int	TgEnableMenuItemByIndex ARGS_DECL((TgMenu*, int index,
		                                   int enabled));
extern int	TgEnableMenuItemBySubMenuInfoPtr ARGS_DECL((TgMenu*,
		                                            TgMenuInfo*,
		                                            int enabled));
extern void	TgClearThreeDButton ARGS_DECL((Display*, Window, GC,
		                              struct BBRec *, int linewidth));
extern void	TgDrawThreeDButton ARGS_DECL((Display*, Window, GC,
		                              struct BBRec *, int state,
		                              int linewidth, int button));
extern void	TgDrawStipple ARGS_DECL((Display*, Window, GC, Pixmap,
		                         int x, int y, int w, int h,
		                         int fg_pixel, int bg_pixel,
		                         int fill_style));
extern void	TgDraw2DGrayedPixmap ARGS_DECL((Display*, Window, Pixmap,
		                                int x, int y, int w, int h,
		                                int fg_pixel, int bg_pixel));
extern void	TgDrawMenuItem ARGS_DECL((TgMenu*, TgMenuItem*));
extern void	TgDrawEntireMenu ARGS_DECL((TgMenu*));
extern int	TgWhichMenuIndex ARGS_DECL((TgMenu*, int x, int y,
		                            int must_be_in_menu));
extern TgMenu	*TgCreatePopUpSubMenu ARGS_DECL((TgMenu*, int selected_index));
extern int	TgPopUpSubMenu ARGS_DECL((TgMenu*, int win_x, int win_y));
extern void	ScrollMenu ARGS_DECL((TgMenu*, int x, int y, int menu_w,
		                      int menu_h, int button_state));
extern int	TgMenuLoop ARGS_DECL((TgMenu*));
extern TgMenu	*TgDestroyMenu ARGS_DECL((TgMenu*, int free_menu));
extern int	TgSetMenuItemInfo ARGS_DECL((TgMenuItem *to_menu_item, int mask,
		                             TgMenuItem *from_menu_item));
extern void	TgSetMenuCanScroll ARGS_DECL((TgMenu*));
extern void	TgAdjustMenuGeometry ARGS_DECL((TgMenu*, int image_w,
		                                int image_h, int max_rows));
extern TgMenu	*TgCreateMenuFromMenuInfo ARGS_DECL((TgMenu*, int X, int Y,
		                                     TgMenuInfo*,
		                                     int status_str_xlated));
extern void	SetScrollableMenuFirstIndex ARGS_DECL((TgMenu*, int index));
extern int	RefreshMainMenu ARGS_DECL((TgMenu*));
extern TgMenu	*CreateMainMenu ARGS_DECL((TgMenu*, int X, int Y, TgMenuInfo*,
		                           int status_str_xlated));
extern int	MainMenu ARGS_DECL((void));
extern int	IsPrefix ARGS_DECL((char *Prefix, char *Str, char **Rest));
extern void	RedrawTitleWindow ARGS_DECL((void));
extern void	RedrawIconWindow ARGS_DECL((void));
extern void	InitTitle ARGS_DECL((void));
extern void	InitMenu ARGS_DECL((void));
extern void	CleanUpMenu ARGS_DECL((void));
extern void	SaveDrawWinInfo ARGS_DECL((void));
extern void	UnIconify ARGS_DECL((void));
extern void	Iconify ARGS_DECL((void));
extern void	IconEventHandler ARGS_DECL((XEvent *));
extern void	TitleEventHandler ARGS_DECL((XEvent *));
extern void	CalcMenubarWindowHeight ARGS_DECL((void));
extern void	RedrawMenubarWindow ARGS_DECL((void));
extern int	MenubarEventHandler ARGS_DECL((XEvent *));

#ifdef _INCLUDE_FROM_MENU_C_
#undef extern
#ifndef _NO_RECURSIVE_EXTERN
#define extern extern
#endif /* ~_NO_RECURSIVE_EXTERN */
#endif /*_INCLUDE_FROM_MENU_C_*/

#endif /*_MENU_E_*/
