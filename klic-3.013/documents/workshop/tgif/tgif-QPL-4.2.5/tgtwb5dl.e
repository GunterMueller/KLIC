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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/tgtwb5dl.e,v 1.9 2011/05/16 16:22:00 william Exp $
 */

#ifndef _TGTWB5DL_E_
#define _TGTWB5DL_E_

#include "tidget.h"

#define DEFAULT_CHAR_SIZE 16
#define CHINESE_ROWS 10
#define CHINESE_COLS 20

#define SPELL_CHINESE 0
#define SPELL_ENGLISH 1

#define ID_ZYFH_DIALOG    101
#define ID_ZYFH_DPY       102
#define ID_ZYFH_KEYPAD    103
#define ID_ZYFH_EDIT      104
#define ID_ZYFH_BMPLIST   105
#define ID_ZYFH_BTN_FETCH 106
#define ID_ZYFH_BTN_SEND  107
#define ID_ZYFH_BTN_CR    108
#define ID_ZYFH_BTN_CLOSE 109

#define GB_ENCODING   0
#define BIG5_ENCODING 1

typedef struct tagZyfhInfo {
   int initialized;
   int mapped;

   char *b5_font_name;
   char im_name[40];
   int b5_font_real_encoding; /* INVALID, GB_ENCODING, or BIG5_ENCODING */
   int b5_font_mod_bytes;
   XFontStruct *xfs;
   int b5_font_w, b5_font_h, b5_font_asc, b5_font_des;

   int zyfh_num_cols /* 13 */, zyfh_num_rows /* 4 */;
   int zyfh_max_symbols /* 13*4 */;
   int zyfh_bitmap_w /* 13*16 */, zyfh_bitmap_h /* 4*16 */;
   int zyfh_image_wh;
   Pixmap zyfh_bitmap; /* the zhu-yin-fu-hao keypad */
   Pixmap symbol_bitmap; /* a 16x16 bitmap for the zyfh_dpy_ctl */

   TidgetInfo *dialogbox_tidgetinfo;
} ZyfhInfo;

extern ZyfhInfo gZyfhInfo;

#ifdef _INCLUDE_FROM_TGTWB5DL_C_
#undef extern
#define extern
#endif /*_INCLUDE_FROM_TGTWB5DL_C_*/

extern void	ShowTdgtb5DialogBox ARGS_DECL((void));
extern void	HideTdgtb5DialogBox ARGS_DECL((void));

extern TidgetInfo	*CreateTdgtb5DialogBox ARGS_DECL((Display*, Window));
extern void	DestroyTdgtb5DialogBox ARGS_DECL((Display*, TidgetInfo*));
extern int	Tdgtb5DlgLoop ARGS_DECL((TidgetInfo*));

extern int	InitTdgtb5Dlg ARGS_DECL((void));
extern void	CleanUpTdgtb5Dlg ARGS_DECL((void));

#ifdef _INCLUDE_FROM_TGTWB5DL_C_
#undef extern
#ifndef _NO_RECURSIVE_EXTERN
#define extern extern
#endif /* ~_NO_RECURSIVE_EXTERN */
#endif /*_INCLUDE_FROM_TGTWB5DL_C_*/

#endif /*_TGTWB5DL_E_*/
