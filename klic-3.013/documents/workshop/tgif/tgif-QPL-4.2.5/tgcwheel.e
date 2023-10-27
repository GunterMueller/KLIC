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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/tgcwheel.e,v 1.3 2011/05/16 16:22:00 william Exp $
 */

#ifndef _TGCWHEEL_E_
#define _TGCWHEEL_E_

#ifdef _INCLUDE_FROM_TGCWHEEL_C_
#undef extern
#define extern
#endif /*_INCLUDE_FROM_TGCWHEEL_C_*/

extern void	TgColorWheel_CleanUp ARGS_DECL((Display*, Window));
extern int	TgColorWheel_Init ARGS_DECL((Display*, Window, char *arg));
extern void	CleanUpColorWheel ARGS_DECL((void));
extern int	InitColorWheel ARGS_DECL((void));
extern int	TgColorWheel_SendKey ARGS_DECL((Display*, Window, XKeyEvent*,
				char*));
extern int	TgColorWheel_HandleCntrlSpace ARGS_DECL((Display*, Window));
extern int	TgColorWheel_HandleCreateText ARGS_DECL((Display*, Window));

#ifdef _INCLUDE_FROM_TGCWHEEL_C_
#undef extern
#ifndef _NO_RECURSIVE_EXTERN
#define extern extern
#endif /* ~_NO_RECURSIVE_EXTERN */
#endif /*_INCLUDE_FROM_TGCWHEEL_C_*/

#endif /*_TGCWHEEL_E_*/
