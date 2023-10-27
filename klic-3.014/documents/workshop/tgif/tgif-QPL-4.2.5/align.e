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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/align.e,v 1.6 2011/05/16 16:21:56 william Exp $
 */

#ifndef _ALIGN_E_
#define _ALIGN_E_

extern int	horiAlign;
extern int	vertAlign;

extern int	alignDirectType;

#ifdef _INCLUDE_FROM_ALIGN_C_
#undef extern
#define extern
#endif /*_INCLUDE_FROM_ALIGN_C_*/

extern void	DistrSelObjs ARGS_DECL((void));
extern void	DistributeDirect ARGS_DECL((int));
extern TgMenu	*CreateDistributeDirectMenu ARGS_DECL((TgMenu*, int X, int Y,
		                                       TgMenuInfo*,
		                                       int status_str_xlated));
extern void	AlignSelObjs ARGS_DECL((void));
extern void	AlignSelToPage ARGS_DECL((void));
extern void	AlignSelToGrid ARGS_DECL((void));
extern void	HoriAlignSubMenu ARGS_DECL((int Index));
extern void	RefreshHoriAlignMenu ARGS_DECL((TgMenu*));
extern TgMenu	*CreateHoriAlignMenu ARGS_DECL((TgMenu*, int X, int Y,
		                                TgMenuInfo*,
		                                int status_str_xlated));
extern int	HoriAlignMenu ARGS_DECL((int X, int Y, int TrackMenubar));
extern void	VertAlignSubMenu ARGS_DECL((int Index));
extern void	RefreshVertAlignMenu ARGS_DECL((TgMenu*));
extern TgMenu	*CreateVertAlignMenu ARGS_DECL((TgMenu*, int X, int Y,
		                                TgMenuInfo*,
		                                int status_str_xlated));
extern int	VertAlignMenu ARGS_DECL((int X, int Y, int TrackMenubar));
extern void	AlignDirect ARGS_DECL((int));
extern TgMenu	*CreateAlignObjsDirectMenu ARGS_DECL((TgMenu*, int X, int Y,
		                                      TgMenuInfo*,
		                                      int status_str_xlated));
extern TgMenu	*CreateAlignToGridDirectMenu ARGS_DECL((TgMenu*, int X, int Y,
		                                        TgMenuInfo*,
		                                        int status_str_xlated));
extern TgMenu	*CreateAlignToPageDirectMenu ARGS_DECL((TgMenu*, int X, int Y,
		                                        TgMenuInfo*,
		                                        int status_str_xlated));
extern void	CenterAnEndPoint ARGS_DECL((void));

#ifdef _INCLUDE_FROM_ALIGN_C_
#undef extern
#ifndef _NO_RECURSIVE_EXTERN
#define extern extern
#endif /* ~_NO_RECURSIVE_EXTERN */
#endif /*_INCLUDE_FROM_ALIGN_C_*/

#endif /*_ALIGN_E_*/
