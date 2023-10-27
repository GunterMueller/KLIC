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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/markup.e,v 1.6 2011/05/16 16:21:58 william Exp $
 */

#ifndef _TGIF_MARKUP_E_
#define _TGIF_MARKUP_E_

extern int	cmdLineParseHtml;

#ifdef _INCLUDE_FROM_MARKUP_C_
#undef extern
#define extern
#endif /*_INCLUDE_FROM_MARKUP_C_*/

extern int	ParseMarkUpFile ARGS_DECL((char *buf, int buf_sz, int *pn_html,
		                           char *psz_content_type));

extern void	CleanUpHtml ARGS_DECL((void));
extern int	InitHtml ARGS_DECL((void));

#ifdef _INCLUDE_FROM_MARKUP_C_
#undef extern
#ifndef _NO_RECURSIVE_EXTERN
#define extern extern
#endif /* ~_NO_RECURSIVE_EXTERN */
#endif /*_INCLUDE_FROM_MARKUP_C_*/

#endif /*_TGIF_MARKUP_E_*/
