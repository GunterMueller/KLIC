/*
 * Author:	Yoichiro Ueno (ueno@cs.titech.ac.jp)
 *
 * Copyright (C) 1991, 1992, Yoichiro Ueno.
 *
 * Permission to use, copy, modify, and distribute this software and
 * its documentation for any purpose is hereby granted by the Author without
 * fee, provided that the above copyright notice appear in all copies and
 * that both the copyright notice and this permission notice appear in
 * supporting documentation, and that the name of the Author not be used
 * in advertising or publicity pertaining to distribution of the software
 * without specific, written prior permission.  The Author makes no
 * representations about the suitability of this software for any purpose.
 * It is provided "as is" without express or implied warranty.
 *
 * THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/convxim.e,v 1.1 2004/06/18 23:19:08 william Exp $
 */

#ifndef _NO_XIM

#ifndef _CONVXIM_E_
#define _CONVXIM_E_

#define	COMPOSE_STATUS	0
#define MB_STATUS	1

typedef struct _XIMStatus {
    char		valid;
    XComposeStatus	status;
    Status		mb_status;
} XIMStatus;

extern char	ximConvModifiers[];
extern char	ximConvLang[];

#ifdef _INCLUDE_FROM_CONVXIM_C_
#undef extern
#define extern
#endif /*_INCLUDE_FROM_CONVXIM_C_*/

extern void	XIMSetICFocus ARGS_DECL((Display*, Window));
extern void	XIMNextEvent ARGS_DECL((Display*, Window, XEvent*));
extern int	XIMLookupString ARGS_DECL((XKeyEvent*, char*, int, KeySym*,
		                           XIMStatus*));
extern void	XIMUnsetICFocus ARGS_DECL((Window));
extern void	XIMSetConvOverSpot ARGS_DECL((Bool));
extern void	XIMTellCursorPosition ARGS_DECL((Display*, Window, int, int));

extern void	XIMCleanUp ARGS_DECL((void));
extern int	XIMInit ARGS_DECL((void));

#ifdef _INCLUDE_FROM_CONVXIM_C_
#undef extern
#ifndef _NO_RECURSIVE_EXTERN
#define extern extern
#endif /* ~_NO_RECURSIVE_EXTERN */
#endif /*_INCLUDE_FROM_CONVXIM_C_*/

#endif /*_CONVXIM_E_*/

#endif /* ~_NO_XIM */

