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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/tdgtbrow.e,v 1.6 2011/05/16 16:22:00 william Exp $
 */

#ifndef _TDGTBROW_E_
#define _TDGTBROW_E_

#include "tidget.h"

#include "tdgtbtn.e"
#include "list.e"

#define TDGTBROW_DEF_H_PAD 2
#define TDGTBROW_DEF_V_PAD 2

/*
 * A button row tidget is a container of which other buttons can sit on top.
 */
typedef struct tagTdgtBtnRow {
   TidgetInfo *pti;

   SimpleWinInfo client_area;

   int content_w, content_h; /* the area occupied by the buttons */
   int h_gap;
   int just; /* JUST_L, JUST_C, or JUST_R */
} TdgtBtnRow;

#ifdef _INCLUDE_FROM_TDGTBROW_C_
#undef extern
#define extern
#endif /*_INCLUDE_FROM_TDGTBROW_C_*/

extern void	TdgtBtnRowReset ARGS_DECL((TdgtBtnRow*));
extern TdgtBtnRow	*CreateTdgtBtnRow ARGS_DECL((Window parent_win,
				TidgetInfo *parent_tidgetinfo, int ctl_id,
				int x, int y, int h_pad, int v_pad, int state,
				int h_gap, int just));

extern int	TdgtBtnRowAddBtn ARGS_DECL((TdgtBtnRow*, TdgtBtn*));

extern int	InitTdgtBtnRow ARGS_DECL((void));
extern void	CleanUpTdgtBtnRow ARGS_DECL((void));

#ifdef _INCLUDE_FROM_TDGTBROW_C_
#undef extern
#ifndef _NO_RECURSIVE_EXTERN
#define extern extern
#endif /* ~_NO_RECURSIVE_EXTERN */
#endif /*_INCLUDE_FROM_TDGTBROW_C_*/

#endif /*_TDGTBROW_E_*/
