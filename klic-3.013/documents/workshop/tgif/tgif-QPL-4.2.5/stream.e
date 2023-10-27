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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/stream.e,v 1.6 2011/05/16 16:21:59 william Exp $
 */

#ifndef _TGIF_STREAM_E_
#define _TGIF_STREAM_E_

#ifdef _INCLUDE_FROM_STREAM_C_
#undef extern
#define extern
#endif /*_INCLUDE_FROM_STREAM_C_*/

extern void	CleanUpMime ARGS_DECL((void));
extern void	InitMime ARGS_DECL((void));

extern void	ResetMultipartReplace ARGS_DECL((int redraw));
extern int	IsJustLF ARGS_DECL((char*));
extern char	* GetHeaderLine ARGS_DECL((char*, int just_lf));
extern void	HandleMultipartReplace ARGS_DECL((char*, int*));
extern void	FakeUserAgent ARGS_DECL((char*));
extern void	FakeReferer ARGS_DECL((char*));
extern void	ToggleKeepAlive ARGS_DECL((void));

#ifdef _INCLUDE_FROM_STREAM_C_
#undef extern
#ifndef _NO_RECURSIVE_EXTERN
#define extern extern
#endif /* ~_NO_RECURSIVE_EXTERN */
#endif /*_INCLUDE_FROM_STREAM_C_*/

#endif /*_TGIF_STREAM_E_*/
