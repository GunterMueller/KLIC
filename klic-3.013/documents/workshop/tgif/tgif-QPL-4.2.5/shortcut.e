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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/shortcut.e,v 1.7 2011/05/16 16:21:59 william Exp $
 */

#ifndef _SHORTCUT_E_
#define _SHORTCUT_E_

#ifdef _INCLUDE_FROM_SHORTCUT_C_
#undef extern
#define extern
#endif /*_INCLUDE_FROM_SHORTCUT_C_*/

extern void	InitShortCut ARGS_DECL((void));
extern void	CleanUpShortCut ARGS_DECL((void));
extern int	FetchShortCut ARGS_DECL((int c, char *code, unsigned int *state,
				char **name, char *args, int args_sz));
extern int	FetchFuncKeyShortCut ARGS_DECL((int c, char *code,
				unsigned int *state, char **name, char *args,
				int args_sz));
extern int	FetchCmdById ARGS_DECL((int nCmdId, char *code,
				unsigned int *state, char **name, char *args));
extern int	ValidShortCut ARGS_DECL((char *name, int num_args, char *code,
				unsigned int *state));

#ifdef _INCLUDE_FROM_SHORTCUT_C_
#undef extern
#ifndef _NO_RECURSIVE_EXTERN
#define extern extern
#endif /* ~_NO_RECURSIVE_EXTERN */
#endif /*_INCLUDE_FROM_SHORTCUT_C_*/

#endif /*_SHORTCUT_E_*/
