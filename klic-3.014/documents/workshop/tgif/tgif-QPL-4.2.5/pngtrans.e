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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/pngtrans.e,v 1.4 2011/05/16 16:21:59 william Exp $
 */

#ifndef _PNGTRANS_E_
#define _PNGTRANS_E_

#ifdef _INCLUDE_FROM_PNGTRANS_C_
#undef extern
#define extern
#endif /*_INCLUDE_FROM_PNGTRANS_C_*/

extern int	ParseTransColor ARGS_DECL((char*,
				unsigned char *puch_trans_color_pixel_r,
				unsigned char *puch_trans_color_pixel_g,
				unsigned char *puch_trans_color_pixel_b));
extern void	ResetPngHeaderInfo ARGS_DECL((PngHeaderInfo*));
extern void	SetPngHeaderInfoForTransColor ARGS_DECL((PngHeaderInfo*,
				unsigned char trans_color_pixel_r,
				unsigned char trans_color_pixel_g,
				unsigned char trans_color_pixel_b));
extern int	PngFileGetTransColorInit ARGS_DECL((PngHeaderInfo*, char *fname));

extern int	PngFileGetTransColor ARGS_DECL((PngHeaderInfo*));

#ifdef _INCLUDE_FROM_PNGTRANS_C_
#undef extern
#ifndef _NO_RECURSIVE_EXTERN
#define extern extern
#endif /* ~_NO_RECURSIVE_EXTERN */
#endif /*_INCLUDE_FROM_PNGTRANS_C_*/

#endif /*_PNGTRANS_E_*/
