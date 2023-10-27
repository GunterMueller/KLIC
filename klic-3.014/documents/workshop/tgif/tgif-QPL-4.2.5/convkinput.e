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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/convkinput.e,v 1.1 2004/06/18 23:19:08 william Exp $
 */

#ifndef _NO_KINPUT

#ifndef _CONVKINPUT_E_
#define _CONVKINPUT_E_

/* protocol of input method */
#define IM_NONE 0
#define IM_KINPUT 1
#define IM_XIM 2

extern int	imProtocol;
extern int	copyAndPasteJIS;
extern char	kinputConvSelName[];

#ifdef _INCLUDE_FROM_CONVKINPUT_C_
#undef extern
#define extern
#endif /*_INCLUDE_FROM_CONVKINPUT_C_*/

extern void	KinputSetConvOverSpot ARGS_DECL((int nConvOverSpot));
extern void	KinputTellCursorPosition ARGS_DECL((Display*, Window,
		                                    int X, int Y));
extern void	KinputBeginConversion ARGS_DECL((Display*, Window));
extern void	KinputCheckClientMessage ARGS_DECL((Display*, Window,
		                                    XClientMessageEvent *));
extern void	CvtCompoundTextToEuc ARGS_DECL((char *dest, char *src));
extern char	* KinputCheckConvProperty ARGS_DECL((Display*, Window,
		                                     XPropertyEvent *));
extern void	KinputEndConversion ARGS_DECL((Display*, Window));

extern void	CvtJisToEuc ARGS_DECL((char *dest, char *src));
extern int	CvtEucToJis ARGS_DECL((char *dest, char *src));

#ifdef _INCLUDE_FROM_CONVKINPUT_C_
#undef extern
#ifndef _NO_RECURSIVE_EXTERN
#define extern extern
#endif /* ~_NO_RECURSIVE_EXTERN */
#endif /*_INCLUDE_FROM_CONVKINPUT_C_*/

#endif /*_CONVKINPUT_E_*/

#endif /* ~_NO_KINPUT */
