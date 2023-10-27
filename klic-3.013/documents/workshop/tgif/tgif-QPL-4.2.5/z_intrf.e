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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/z_intrf.e,v 1.7 2011/05/16 16:22:00 william Exp $
 */

#ifndef _Z_INTRF_E_
#define _Z_INTRF_E_

#ifdef _INCLUDE_FROM_Z_INTRF_C_
#undef extern
#define extern
#endif /*_INCLUDE_FROM_Z_INTRF_C_*/

extern int	HasZlibSupport ARGS_DECL((void));
extern int	DeflateFile ARGS_DECL((char *fname, char *deflated_fname));
extern int	DoDeflate ARGS_DECL((FILE *in_fp, char *in_buf, int bytes_left,
				FILE *out_fp, int use_def_compression,
				int use_byte_stuffing, int *pn_rc));
extern int	DoInflate ARGS_DECL((char *in_buf, int bytes_left,
				FILE *out_fp, int use_byte_unstuffing,
				int *pn_rc));
extern void	ZlibError ARGS_DECL((int status, int deflate));

#ifdef _INCLUDE_FROM_Z_INTRF_C_
#undef extern
#ifndef _NO_RECURSIVE_EXTERN
#define extern extern
#endif /* ~_NO_RECURSIVE_EXTERN */
#endif /*_INCLUDE_FROM_Z_INTRF_C_*/

#endif /*_Z_INTRF_E_*/
