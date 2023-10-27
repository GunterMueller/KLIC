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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/tdgtlist.e,v 1.6 2011/05/16 16:22:00 william Exp $
 */

#ifndef _TDGTLIST_E_
#define _TDGTLIST_E_

#include "tidget.h"

#include "list.e"

#define TDGTLIST_DEF_H_PAD 1
#define TDGTLIST_DEF_V_PAD 1

typedef struct tagListInfo {
   char **entries; /* this is a cache, updated by TdgtListUpdateEntries() */
   DspList *dsp_ptr;

   int first_index;
   int marked_index;
   int num_visible_lines;

   struct CheckArrayRec *p_check_array;

   CVList list;
} ListInfo;

typedef struct tagTdgtList {
   TidgetInfo *pti;

   Window dsp_win, scr_win;
   SimpleWinInfo dsp_win_info, scr_win_info;

   int can_select; /* if FALSE, cannot select an item */
   int multicolor;
   int auto_scroll_on_insert;

   int scr_area_h;
   int scr_dir; /* temporary variable, which direction it's scrolling */

   ListInfo list_info;
} TdgtList;

typedef struct tagListItemInfo {
   char nick_name[MAXSTRING];
   int color_index, font_style;
   char *buf;
} ListItemInfo;

#ifdef _INCLUDE_FROM_TDGTLIST_C_
#undef extern
#define extern
#endif /*_INCLUDE_FROM_TDGTLIST_C_*/

extern void	TdgtListReset ARGS_DECL((TdgtList*));
extern TdgtList	*CreateTdgtList ARGS_DECL((Window parent_win,
				TidgetInfo *parent_tidgetinfo, int ctl_id,
				int x, int y, int w, int h_pad, int v_pad,
				int num_visible_lines, int can_select,
				int multicolor, int auto_scroll_on_insert));

extern int	TdgtListCleanUpEntries ARGS_DECL((TdgtList*));
extern int	TdgtListUpdateEntries ARGS_DECL((TdgtList*));
extern int	TdgtListScrollToBottom ARGS_DECL((TdgtList*));
extern int	TdgtListInsertString ARGS_DECL((TdgtList*, char*));
extern int	TdgtListInsertListItemInfo ARGS_DECL((TdgtList*,
				ListItemInfo*));
extern int	TdgtListSetAutoScrollOnInsert ARGS_DECL((TdgtList*, int));
extern int	TdgtListGetAutoScrollOnInsert ARGS_DECL((TdgtList*));

extern int	InitTdgtList ARGS_DECL((void));
extern void	CleanUpTdgtList ARGS_DECL((void));

#ifdef _INCLUDE_FROM_TDGTLIST_C_
#undef extern
#ifndef _NO_RECURSIVE_EXTERN
#define extern extern
#endif /* ~_NO_RECURSIVE_EXTERN */
#endif /*_INCLUDE_FROM_TDGTLIST_C_*/

#endif /*_TDGTLIST_E_*/
