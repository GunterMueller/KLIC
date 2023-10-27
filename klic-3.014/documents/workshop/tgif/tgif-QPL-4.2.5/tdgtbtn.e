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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/tdgtbtn.e,v 1.6 2011/05/16 16:22:00 william Exp $
 */

#ifndef _TDGTBTN_E_
#define _TDGTBTN_E_

#include "tidget.h"

#define TDGTBTN_DEF_H_PAD 0
#define TDGTBTN_DEF_V_PAD 0

#define TDGTBTN_CLICK 0
#define TDGTBTN_STICKY 1

typedef struct tagTdgtBtn {
   TidgetInfo *pti;

   int btn_type; /* TGMUTYPE_TEXT, TGMUTYPE_COLOR, or TGMUTYPE_BITMAP */
   int btn_style; /* TDGTBTN_CLICK or TDGTBTN_STICKY */
   int font_style; /* STYLE_NR, STYLE_BR, STYLE_NI, STYLE_BI */

   /*
    * for TGMUTYPE_TEXT, str is (char*), point to a string
    * for TGMUTYPE_COLOR, str is (int), point to a color index
    * for TGMUTYPE_BITMAP, str is (Pixmap*), point to a pixmap
    */
   char *str;
   MouseOverStatusInfo mosi;
} TdgtBtn;

#ifdef _INCLUDE_FROM_TDGTBTN_C_
#undef extern
#define extern
#endif /*_INCLUDE_FROM_TDGTBTN_C_*/

extern void	CalcTdgtBtnDim ARGS_DECL((char *text, int min_len, int h_pad,
				int v_pad, int *pn_width, int *pn_height));

extern void	TdgtBtnReset ARGS_DECL((TdgtBtn*));
extern TdgtBtn	*CreateTdgtBtn ARGS_DECL((Window parent_win,
				TidgetInfo *parent_tidgetinfo, int ctl_id,
				int x, int y, int w, int h, int h_pad,
				int v_pad, int btn_type, int btn_style,
				int state, int font_style, char *str,
				MouseOverStatusInfo *pmosi));

extern int	TdgtBtnSetText ARGS_DECL((TdgtBtn*, char *str,
				MouseOverStatusInfo *pmosi));
extern char	*TdgtBtnGetText ARGS_DECL((TdgtBtn*));
extern int	TdgtBtnSetState ARGS_DECL((TdgtBtn*, int));
extern int	TdgtBtnGetState ARGS_DECL((TdgtBtn*));

extern int	InitTdgtBtn ARGS_DECL((void));
extern void	CleanUpTdgtBtn ARGS_DECL((void));

#ifdef _INCLUDE_FROM_TDGTBTN_C_
#undef extern
#ifndef _NO_RECURSIVE_EXTERN
#define extern extern
#endif /* ~_NO_RECURSIVE_EXTERN */
#endif /*_INCLUDE_FROM_TDGTBTN_C_*/

#endif /*_TDGTBTN_E_*/
