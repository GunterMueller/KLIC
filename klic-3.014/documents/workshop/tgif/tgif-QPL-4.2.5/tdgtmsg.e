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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/tdgtmsg.e,v 1.6 2011/05/16 16:22:00 william Exp $
 */

#ifndef _TDGTMSG_E_
#define _TDGTMSG_E_

#include "tidget.h"

#define TDGTMSG_DEF_H_PAD 4
#define TDGTMSG_DEF_V_PAD 2

/*
 * A message tidget displays a string.
 */
typedef struct tagTdgtMsg {
   TidgetInfo *pti;

   int just; /* JUST_L, JUST_C, or JUST_R */
} TdgtMsg;

#ifdef _INCLUDE_FROM_TDGTMSG_C_
#undef extern
#define extern
#endif /*_INCLUDE_FROM_TDGTMSG_C_*/

extern void	TdgtMsgReset ARGS_DECL((TdgtMsg*));
extern TdgtMsg	*CreateTdgtMsg ARGS_DECL((Window parent_win,
				TidgetInfo *parent_tidgetinfo, int ctl_id,
				int x, int y, int h_pad, int v_pad, int state,
				char *caption, int just));

extern int	TdgtMsgSetText ARGS_DECL((TdgtMsg*, char *str));
extern char	*TdgtMsgGetText ARGS_DECL((TdgtMsg*));
extern int	TdgtMsgSetState ARGS_DECL((TdgtMsg*, int));
extern int	TdgtMsgGetState ARGS_DECL((TdgtMsg*));

extern int	InitTdgtMsg ARGS_DECL((void));
extern void	CleanUpTdgtMsg ARGS_DECL((void));

#ifdef _INCLUDE_FROM_TDGTMSG_C_
#undef extern
#ifndef _NO_RECURSIVE_EXTERN
#define extern extern
#endif /* ~_NO_RECURSIVE_EXTERN */
#endif /*_INCLUDE_FROM_TDGTMSG_C_*/

#endif /*_TDGTMSG_E_*/
