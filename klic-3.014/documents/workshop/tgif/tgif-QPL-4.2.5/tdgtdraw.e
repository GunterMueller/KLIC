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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/tdgtdraw.e,v 1.6 2011/05/16 16:22:00 william Exp $
 */

#ifndef _TDGTDRAW_E_
#define _TDGTDRAW_E_

#include "tidget.h"

#define TDGTDRAW_DEF_H_PAD 2
#define TDGTDRAW_DEF_V_PAD 2

struct tagTdgtDraw;

typedef int (TdgtDrawRedrawCallbackFunc)ARGS_DECL((struct tagTdgtDraw *));
typedef int (TdgtDrawEvHandlerCallbackFunc)ARGS_DECL((struct tagTdgtDraw *,
		XEvent*));
typedef int (TdgtDrawResetCallbackFunc)ARGS_DECL((struct tagTdgtDraw *));
typedef int (TdgtDrawSendCmdCallbackFunc)ARGS_DECL((struct tagTdgtDraw *,
		int cmd_type, int cmd_arg, void *pv_cmd_userdata));

/*
 * A drawing area tidget uses callback routings to handl redraws
 *         and mouse events.
 */
typedef struct tagTdgtDraw {
   TidgetInfo *pti;

   SimpleWinInfo client_area;

   void *pv_userdata;

   /* callback functions */
   TdgtDrawRedrawCallbackFunc *pf_redraw_callback;
   TdgtDrawEvHandlerCallbackFunc *pf_ev_handler_callback;
   TdgtDrawResetCallbackFunc *pf_reset_callback;
   TdgtDrawSendCmdCallbackFunc *pf_sendcmd_callback;
} TdgtDraw;

#ifdef _INCLUDE_FROM_TDGTDRAW_C_
#undef extern
#define extern
#endif /*_INCLUDE_FROM_TDGTDRAW_C_*/

extern void	TdgtDrawReset ARGS_DECL((TdgtDraw*));
extern TdgtDraw	*CreateTdgtDraw ARGS_DECL((Window parent_win,
				TidgetInfo *parent_tidgetinfo, int ctl_id,
				int x, int y, int client_w, int client_h,
				int h_pad, int v_pad, int state,
				void *pv_userdata));

extern int	TdgtDrawSetUserData ARGS_DECL((TdgtDraw*, void *pv_userdata));
extern char	*TdgtDrawGetUserData ARGS_DECL((TdgtDraw*));
extern int	TdgtDrawSetState ARGS_DECL((TdgtDraw*, int));
extern int	TdgtDrawGetState ARGS_DECL((TdgtDraw*));

extern void	TdgtDrawSetRedrawCallback ARGS_DECL((TdgtDraw*,
				TdgtDrawRedrawCallbackFunc*));
extern void	TdgtDrawSetEvHandlerCallback ARGS_DECL((TdgtDraw*,
				TdgtDrawEvHandlerCallbackFunc*));
extern void	TdgtDrawSetResetCallback ARGS_DECL((TdgtDraw*,
				TdgtDrawResetCallbackFunc*));
extern void	TdgtDrawSetSendCmdCallback ARGS_DECL((TdgtDraw*,
				TdgtDrawSendCmdCallbackFunc*));

extern int	InitTdgtDraw ARGS_DECL((void));
extern void	CleanUpTdgtDraw ARGS_DECL((void));

#ifdef _INCLUDE_FROM_TDGTDRAW_C_
#undef extern
#ifndef _NO_RECURSIVE_EXTERN
#define extern extern
#endif /* ~_NO_RECURSIVE_EXTERN */
#endif /*_INCLUDE_FROM_TDGTDRAW_C_*/

#endif /*_TDGTDRAW_E_*/
