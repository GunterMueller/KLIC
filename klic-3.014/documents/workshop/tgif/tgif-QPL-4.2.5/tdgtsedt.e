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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/tdgtsedt.e,v 1.6 2011/05/16 16:22:00 william Exp $
 */

#ifndef _TDGTSEDT_E_
#define _TDGTSEDT_E_

#include "tidget.h"

#define TDGTSEDT_DEF_H_PAD 2
#define TDGTSEDT_DEF_V_PAD 1

/*
 * A simple edit tidget is left-justitified, single-line, and single-color.
 */
typedef struct tagTdgtSmplEdit {
   TidgetInfo *pti;

   SimpleWinInfo client_area;

   int font_style; /* can be STYLE_NR, STYLE_BR, STYLE_NI, or STYLE_BI */
   int color_index;
   char color_str[40];

   int cursor_shown;

   int w, h, min_lbearing, max_rextra, x_start;

   Pixmap text_pixmap;
} TdgtSmplEdit;

#ifdef _INCLUDE_FROM_TDGTSEDT_C_
#undef extern
#define extern
#endif /*_INCLUDE_FROM_TDGTSEDT_C_*/

extern void	TdgtSmplEditReset ARGS_DECL((TdgtSmplEdit*));
extern TdgtSmplEdit	*CreateTdgtSmplEdit ARGS_DECL((Window parent_win,
				TidgetInfo *parent_tidgetinfo, int ctl_id,
				int x, int y, int w, int h_pad, int v_pad,
				char *caption, int font_style,
				int color_index));

extern int	TdgtSmplEditSetText ARGS_DECL((TdgtSmplEdit*, char *str));
extern char	*TdgtSmplEditGetText ARGS_DECL((TdgtSmplEdit*));
extern void	TdgtSmplEditGetTextFormatInfo ARGS_DECL((TdgtSmplEdit*,
				TextFormatInfo*));
extern int	TdgtSmplEditSetColorIndex ARGS_DECL((TdgtSmplEdit*, int));
extern int	TdgtSmplEditGetColorIndex ARGS_DECL((TdgtSmplEdit*));
extern int	TdgtSmplEditSetFontStyle ARGS_DECL((TdgtSmplEdit*, int));
extern int	TdgtSmplEditGetFontStyle ARGS_DECL((TdgtSmplEdit*));

extern int	TdgtSmplEditAppendStr ARGS_DECL((TdgtSmplEdit*, char *str,
				int len));
extern int	TdgtSmplEditDelLastChar ARGS_DECL((TdgtSmplEdit*));

extern void	TdgtSmplEditSetFocus ARGS_DECL((TdgtSmplEdit*, int nSet));

extern int	InitTdgtSmplEdit ARGS_DECL((void));
extern void	CleanUpTdgtSmplEdit ARGS_DECL((void));

#ifdef _INCLUDE_FROM_TDGTSEDT_C_
#undef extern
#ifndef _NO_RECURSIVE_EXTERN
#define extern extern
#endif /* ~_NO_RECURSIVE_EXTERN */
#endif /*_INCLUDE_FROM_TDGTSEDT_C_*/

#endif /*_TDGTSEDT_E_*/
