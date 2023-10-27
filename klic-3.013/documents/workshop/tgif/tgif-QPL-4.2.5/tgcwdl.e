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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/tgcwdl.e,v 1.7 2011/05/16 16:22:00 william Exp $
 */

#ifndef _TGCWDL_E_
#define _TGCWDL_E_

#include "tidget.h"

#define HS_WIDTH  181
#define HS_HEIGHT 181
#define V_WIDTH   14
#define V_HEIGHT  181

#define ID_CWHEEL_DIALOG     101
#define ID_CWHEEL_HS_AREA    102
#define ID_CWHEEL_V_AREA     103
#define ID_CWHEEL_EDIT_R     104
#define ID_CWHEEL_EDIT_G     105
#define ID_CWHEEL_EDIT_B     106
#define ID_CWHEEL_EDIT_H     107
#define ID_CWHEEL_EDIT_S     108
#define ID_CWHEEL_EDIT_V     109
#define ID_CWHEEL_EDIT_CNAME 110
#define ID_CWHEEL_SELCOLOR   111
#define ID_CWHEEL_BTN_ADD    112
#define ID_CWHEEL_BTN_OK     113
#define ID_CWHEEL_BTN_CANCEL 114

typedef struct tagColorWheelInfo {
   int initialized;
   int mapped;

   unsigned int r, g, b, h, s, v;
   char color_str[40]; /* input and output */
   int hs_x, hs_y, v_y;
   Pixmap hs_pixmap;
   Pixmap v_pixmap;

   TidgetInfo *dialogbox_tidgetinfo;
} ColorWheelInfo;

extern ColorWheelInfo gColorWheelInfo;

#ifdef _INCLUDE_FROM_TGCWDL_C_
#undef extern
#define extern
#endif /*_INCLUDE_FROM_TGCWDL_C_*/

extern void	ShowTdgtColorWheelDialogBox ARGS_DECL((void));
extern void	HideTdgtColorWheelDialogBox ARGS_DECL((void));

extern TidgetInfo	*CreateTdgtColorWheelDialogBox ARGS_DECL((Display*,
					Window));
extern void	DestroyTdgtColorWheelDialogBox ARGS_DECL((Display*,
				TidgetInfo*));
extern int	TdgtColorWheelDlgLoop ARGS_DECL((TidgetInfo*));

extern int	InitTdgtColorWheelDlg ARGS_DECL((void));
extern void	CleanUpTdgtColorWheelDlg ARGS_DECL((void));

#ifdef _INCLUDE_FROM_TGCWDL_C_
#undef extern
#ifndef _NO_RECURSIVE_EXTERN
#define extern extern
#endif /* ~_NO_RECURSIVE_EXTERN */
#endif /*_INCLUDE_FROM_TGCWDL_C_*/

#endif /*_TGCWDL_E_*/
