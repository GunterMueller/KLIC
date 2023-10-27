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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/tdgtbase.e,v 1.7 2011/05/16 16:21:59 william Exp $
 */

#ifndef _TDGTBASE_E_
#define _TDGTBASE_E_

#include "tidget.h"

#include "list.e"

#define TDGTBASE_DEF_H_PAD 4
#define TDGTBASE_DEF_V_PAD 4

#define TDGTCMD_CAN_HANDLE_FOCUS_CHANGE 1
#define TDGTCMD_REMOVE_FOCUS            2

struct tagTdgtBase;

typedef int (TdgtBaseKeyPressEvHandlerCallbackFunc)ARGS_DECL((
		struct tagTdgtBase *, XEvent*));
typedef void (TdgtBaseWmDeleteEvHandlerCallbackFunc)ARGS_DECL((
		struct tagTdgtBase *));
typedef int (TdgtBaseFocusCallbackFunc)ARGS_DECL((
		struct tagTdgtBase *, int cmd_type, int cmd_arg,
		void *pv_cmd_userdata));

/*
 * A base tidget is a container of which other tidgets can sit on top.
 */
typedef struct tagTdgtBase {
   TidgetInfo *pti;

   /* callback functions */
   TdgtBaseKeyPressEvHandlerCallbackFunc *pf_key_press_ev_handler_callback;
   TdgtBaseWmDeleteEvHandlerCallbackFunc *pf_wm_delete_ev_handler_callback;
   TdgtBaseFocusCallbackFunc *pf_focus_callback;
} TdgtBase;

#ifdef _INCLUDE_FROM_TDGTBASE_C_
#undef extern
#define extern
#endif /*_INCLUDE_FROM_TDGTBASE_C_*/

extern void	TdgtBaseReset ARGS_DECL((TdgtBase*));
extern TdgtBase	*CreateTdgtBase ARGS_DECL((Window parent_win,
				TidgetInfo *parent_tidgetinfo, int ctl_id,
				int x, int y, int w, int h, int h_pad,
				int v_pad, int state, char *caption));

extern int	TdgtBaseSetText ARGS_DECL((TdgtBase*, char *str));
extern char	*TdgtBaseGetText ARGS_DECL((TdgtBase*));
extern int	TdgtBaseSetState ARGS_DECL((TdgtBase*, int));
extern int	TdgtBaseGetState ARGS_DECL((TdgtBase*));

extern int	TdgtBaseAddChild ARGS_DECL((TdgtBase*, TidgetInfo*));

extern void	TdgtBaseWillHandleKeyPressEvents ARGS_DECL((TdgtBase*,
				TdgtBaseKeyPressEvHandlerCallbackFunc*));
extern void	TdgtBaseSetWmDeleteCallback ARGS_DECL((TdgtBase*,
				TdgtBaseWmDeleteEvHandlerCallbackFunc*));
extern void	TdgtBaseSetFocusCallback ARGS_DECL((TdgtBase*,
				TdgtBaseFocusCallbackFunc*));

extern int	InitTdgtBase ARGS_DECL((void));
extern void	CleanUpTdgtBase ARGS_DECL((void));

#ifdef _INCLUDE_FROM_TDGTBASE_C_
#undef extern
#ifndef _NO_RECURSIVE_EXTERN
#define extern extern
#endif /* ~_NO_RECURSIVE_EXTERN */
#endif /*_INCLUDE_FROM_TDGTBASE_C_*/

#endif /*_TDGTBASE_E_*/
