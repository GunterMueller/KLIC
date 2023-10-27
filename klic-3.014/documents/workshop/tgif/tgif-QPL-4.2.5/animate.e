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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/animate.e,v 1.6 2011/05/16 16:21:56 william Exp $
 */

#ifndef _ANIMATE_E_
#define _ANIMATE_E_

#ifdef _INCLUDE_FROM_ANIMATE_C_
#undef extern
#define extern
#endif /*_INCLUDE_FROM_ANIMATE_C_*/

extern void	AnimateSend ARGS_DECL((struct PolyRec *, int Speed, int Pixel));
extern void	AnimateSel ARGS_DECL((void));
extern void	AnimateFlashColor ARGS_DECL((struct ObjRec *, int Index));
extern void	FlashSelColor ARGS_DECL((void));

#ifdef _INCLUDE_FROM_ANIMATE_C_
#undef extern
#ifndef _NO_RECURSIVE_EXTERN
#define extern extern
#endif /* ~_NO_RECURSIVE_EXTERN */
#endif /*_INCLUDE_FROM_ANIMATE_C_*/

#endif /*_ANIMATE_E_*/
