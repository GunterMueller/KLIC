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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/tdgtbmpl.e,v 1.6 2011/05/16 16:21:59 william Exp $
 */

#ifndef _TDGTBMPL_E_
#define _TDGTBMPL_E_

#include "tidget.h"

#include "list.e"

#define TDGTBMPL_DEF_H_PAD 0
#define TDGTBMPL_DEF_V_PAD 0

struct tagBmpListItemInfo;

typedef void (TdgtBmpListFreeItemCallbackFunc)ARGS_DECL((
		struct tagBmpListItemInfo *));

typedef struct tagBmpListItemInfo {
   Pixmap pixmap;
   int width, height, depth;
   void *pv_userdata;

   /* callback functions */
   TdgtBmpListFreeItemCallbackFunc *pf_free_callback;
} BmpListItemInfo;

typedef struct tagTdgtBmpList {
   TidgetInfo *pti;

   Window dsp_win, scr_win;
   SimpleWinInfo dsp_win_info, scr_win_info;

   int can_select; /* if FALSE, cannot select an item */

   int one_bmp_w, one_bmp_h; /* width and height of one bitmap */
   int gap; /* gap between bitmaps, both horizontal and vertical */

   int first_index;
   int marked_index;

   int num_cols;
   int num_visible_lines;

   CVList list;

   MouseOverStatusInfo mosi;

   /* temporary variables */
   int scr_dir; /* which direction it's scrolling */
   Time last_selected_index; /* for double-clicking */
   Time last_clicked_time; /* for double-clicking */
} TdgtBmpList;

#ifdef _INCLUDE_FROM_TDGTBMPL_C_
#undef extern
#define extern
#endif /*_INCLUDE_FROM_TDGTBMPL_C_*/

extern void	TdgtBmpListReset ARGS_DECL((TdgtBmpList*));
extern TdgtBmpList	*CreateTdgtBmpList ARGS_DECL((Window parent_win,
				TidgetInfo *parent_tidgetinfo, int ctl_ld,
				int x, int y, int h_pad, int v_pad, int bmp_w,
				int bmp_h, int num_cols, int num_visible_lines,
				int gap, int can_select,
				MouseOverStatusInfo *pmosi));

extern int	TdgtBmpListCleanUpEntries ARGS_DECL((TdgtBmpList*));
extern int	TdgtBmpListScrollToBottom ARGS_DECL((TdgtBmpList*));
extern int	TdgtBmpListAddItem ARGS_DECL((TdgtBmpList*, BmpListItemInfo*));

extern int	InitTdgtBmpList ARGS_DECL((void));
extern void	CleanUpTdgtBmpList ARGS_DECL((void));

#ifdef _INCLUDE_FROM_TDGTBMPL_C_
#undef extern
#ifndef _NO_RECURSIVE_EXTERN
#define extern extern
#endif /* ~_NO_RECURSIVE_EXTERN */
#endif /*_INCLUDE_FROM_TDGTBMPL_C_*/

#endif /*_TDGTBMPL_E_*/
