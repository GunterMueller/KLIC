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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/tgisdl.e,v 1.8 2011/05/16 16:22:00 william Exp $
 */

#ifndef _TGISDL_E_
#define _TGISDL_E_

#include "tidget.h"

#define TGIS_COLS  16
#define TGIS_ROWS  13

#define ID_INSSYM_DIALOG     101
#define ID_INSSYM_SYM_AREA   102
#define ID_INSSYM_BTN_ADD    103
#define ID_INSSYM_BTN_OK     104
#define ID_INSSYM_BTN_CANCEL 105

typedef struct tagInsertSymbolInfo {
   int initialized;
   int mapped;

   XFontStruct *xfs;
   GC gc;

   int font_width, font_asc, font_des, font_height;
   int padding;

   int a_symbol_w, a_symbol_h;
   int pixmap_w, pixmap_h;

   int something_selected, selected_row, selected_col;

   int sym_x, sym_y;
   Pixmap sym_pixmap;

   Time last_click_time;

   TidgetInfo *dialogbox_tidgetinfo;
} InsertSymbolInfo;

extern InsertSymbolInfo gInsertSymbolInfo;

#ifdef _INCLUDE_FROM_TGISDL_C_
#undef extern
#define extern
#endif /*_INCLUDE_FROM_TGISDL_C_*/

extern void	ShowTdgtInsertSymbolDialogBox ARGS_DECL((void));
extern void	HideTdgtInsertSymbolDialogBox ARGS_DECL((void));

extern TidgetInfo	*CreateTdgtInsertSymbolDialogBox ARGS_DECL((Display*,
					Window));
extern void	DestroyTdgtInsertSymbolDialogBox ARGS_DECL((Display*,
				TidgetInfo*));
extern int	TdgtInsertSymbolDlgLoop ARGS_DECL((TidgetInfo*));

extern int	InitTdgtInsertSymbolDlg ARGS_DECL((void));
extern void	CleanUpTdgtInsertSymbolDlg ARGS_DECL((void));

#ifdef _INCLUDE_FROM_TGISDL_C_
#undef extern
#ifndef _NO_RECURSIVE_EXTERN
#define extern extern
#endif /* ~_NO_RECURSIVE_EXTERN */
#endif /*_INCLUDE_FROM_TGISDL_C_*/

#endif /*_TGISDL_E_*/
