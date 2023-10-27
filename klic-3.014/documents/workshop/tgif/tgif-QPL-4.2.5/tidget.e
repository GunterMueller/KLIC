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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/tidget.e,v 1.8 2011/05/16 16:22:00 william Exp $
 */

#ifndef _TIDGET_E_
#define _TIDGET_E_

#include "tidget.h"

#include "list.e"

#define TDGT_NOTIFY ((long)0x80000002)

typedef void (TidgetDrawMsgStringFunc)ARGS_DECL((Display*, Window, GC, int,
                int, char*, int));
typedef int (TidgetMsgTextWidthFunc)ARGS_DECL((XFontStruct*, char*, int));

typedef struct tagTidgetManager {
   GC gc;

   int want_all_key_press_events;
   TidgetInfo *key_press_base_tidgetinfo;

   Atom notify_atom;

   CVList toplevel_tidgets; /* list of (TidgetInfo*) */
} TidgetManager;

extern TidgetManager	gTidgetManager;

#ifdef _INCLUDE_FROM_TIDGET_C_
#undef extern
#define extern
#endif /*_INCLUDE_FROM_TIDGET_C_*/

extern void	TidgetWindowNotify ARGS_DECL((Window, int ctl_id, int nf_type,
				int nf_arg, int nf_arg2));
extern void	TidgetNotify ARGS_DECL((TidgetInfo*, int ctl_id, int nf_type,
				int nf_arg, int nf_arg2));
extern void	TidgetControlNotify ARGS_DECL((TidgetInfo*, int nf_type,
				int nf_arg, int nf_arg2));

extern int	IsTdgtWindowNotifyEvent ARGS_DECL((Window, XEvent*, TdgtNtfy*));
extern int	IsTdgtNotifyEvent ARGS_DECL((TidgetInfo*, XEvent*, TdgtNtfy*));

extern void	TidgetDrawFocusRect ARGS_DECL((TidgetInfo*, SimpleWinInfo*));
extern void	RedrawTidget ARGS_DECL((TidgetInfo*));
extern int	TidgetEventHandler ARGS_DECL((TidgetInfo*, XEvent*,
				TidgetInfo *pti_handler_tidget));
extern int	IsTidgetEvent ARGS_DECL((TidgetInfo*, XEvent*,
				TidgetInfo **ppti_handler_tidget_return));

extern void	DestroyTidget ARGS_DECL((TidgetInfo**));
extern void	SetTidgetInfoBasic ARGS_DECL((TidgetInfo*, int tidget_type,
				void *tidget, Window parent_win, int x, int y,
				int w, int h, int h_pad, int v_pad, int state,
				char *caption));
extern void	TidgetCanHaveChildren ARGS_DECL((TidgetInfo*, int));
extern void	TidgetSetCallbacks ARGS_DECL((TidgetInfo*,
				TidgetRedrawCallbackFunc*,
				TidgetEvHandlerCallbackFunc*,
				TidgetIsEventCallbackFunc*,
				TidgetDestroyCallbackFunc*,
				TidgetMapCallbackFunc*,
				TidgetMoveResizeCallbackFunc*,
				TidgetSendCmdCallbackFunc*));
extern TidgetInfo	*NewTidgetInfo ARGS_DECL((TidgetInfo *parent_tidgetinfo,
				int type, void *tidget, int ctl_id,
				void *userdata));
extern void	MapTidget ARGS_DECL((TidgetInfo*));
extern void	TidgetMoveResize ARGS_DECL((TidgetInfo*, int x, int y, int w,
				int h));
extern void	ResetTidgetCommon ARGS_DECL((TidgetCommonInfo*));

extern int	TidgetSendCmd ARGS_DECL((TidgetInfo*, int cmd_type, int cmd_arg,
				void *pv_cmd_userdata));

extern int	TidgetDisableRedraw ARGS_DECL((TidgetInfo*, int));
extern int	TidgetSetDirty ARGS_DECL((TidgetInfo*, int));

extern int	TidgetHasFocus ARGS_DECL((void));
extern int	TidgetSetHasFocus ARGS_DECL((int nHasFocus));
extern Window	TidgetGetFocusWindow ARGS_DECL((void));
extern Window	TidgetSetFocusWindow ARGS_DECL((Window));
extern int	TidgetIsFocus ARGS_DECL((TidgetInfo*));

extern void	TidgetGetFontInfo ARGS_DECL((int *pnFontIndex, int *pnSzUnit));

extern void	TidgetGetFontInfoGivenStyle ARGS_DECL((int font_style,
				XFontStruct **pp_font_ptr, int *pn_font_width,
				int *pn_font_height, int *pn_font_asc,
				int *pn_font_des));

extern void	TidgetManagerResetGC ARGS_DECL((void));
extern int	TidgetManagerHandleEvent ARGS_DECL((XEvent*));
extern int	TidgetManagerHandleAllKeyPressEvent ARGS_DECL((XEvent*));
extern int	TidgetManagerWantAllKeyPressEvents ARGS_DECL((void));
extern int	TidgetManagerSetWantAllKeyPressEvents ARGS_DECL((TidgetInfo*,
				int want_all_key_press_events));

extern int	InitTidget ARGS_DECL((void));
extern void	CleanUpTidget ARGS_DECL((void));

#ifdef _INCLUDE_FROM_TIDGET_C_
#undef extern
#ifndef _NO_RECURSIVE_EXTERN
#define extern extern
#endif /* ~_NO_RECURSIVE_EXTERN */
#endif /*_INCLUDE_FROM_TIDGET_C_*/

#endif /*_TIDGET_E_*/
