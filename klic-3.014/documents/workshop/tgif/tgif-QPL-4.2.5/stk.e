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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/stk.e,v 1.6 2011/05/16 16:21:59 william Exp $
 */

#ifndef _STK_E_
#define _STK_E_

extern struct ObjRec	* tgifObj;
extern struct StkRec	* topStk;

#ifdef _INCLUDE_FROM_STK_C_
#undef extern
#define extern
#endif /*_INCLUDE_FROM_STK_C_*/

extern int		AncesterModified ARGS_DECL((void));
extern void		InitTgifObj ARGS_DECL((void));
extern void		InitStk ARGS_DECL((void));
extern struct StkRec	* SaveFileInfo ARGS_DECL((void));
extern void		ResetFileInfo ARGS_DECL((void));
extern int		PushIcon ARGS_DECL((void));
extern void		RestoreFileInfo ARGS_DECL((struct StkRec *));
extern void		PopIcon ARGS_DECL((void));
extern void		CleanUpTgifObj ARGS_DECL((void));
extern void		CleanUpStk ARGS_DECL((void));

#ifdef _INCLUDE_FROM_STK_C_
#undef extern
#ifndef _NO_RECURSIVE_EXTERN
#define extern extern
#endif /* ~_NO_RECURSIVE_EXTERN */
#endif /*_INCLUDE_FROM_STK_C_*/

#endif /*_STK_E_*/
