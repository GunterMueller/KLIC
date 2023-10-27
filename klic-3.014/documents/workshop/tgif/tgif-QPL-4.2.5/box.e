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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/box.e,v 1.7 2011/06/09 15:55:15 cvsps Exp $
 */

#ifndef _BOX_E_
#define _BOX_E_

extern int	boxDrawn;

#ifdef _INCLUDE_FROM_BOX_C_
#undef extern
#define extern
#endif /*_INCLUDE_FROM_BOX_C_*/

extern void	MyBox ARGS_DECL((Window, GC, int X1, int Y1, int X2, int Y2));
extern void	DumpBoxObj ARGS_DECL((FILE *, struct ObjRec *));
extern int	CreateBoxObj ARGS_DECL((int X1, int Y1, int X2, int Y2,
		                        int CreateAbsolute));
extern void	DrawBoxObj ARGS_DECL((Window, int X, int Y, struct ObjRec *));
extern void	DrawBox ARGS_DECL((XEvent *));
extern void	MakeBoxObjFromBoundingBox ARGS_DECL((void));
extern void	SaveBoxObj ARGS_DECL((FILE *, struct ObjRec *));
extern void	ReadBoxObj ARGS_DECL((FILE *, char *, struct ObjRec **));
extern void	SetBoxPropMask ARGS_DECL((struct ObjRec *, long *plMask,
		                          long *plSkip,
		                          struct PropertiesRec *pProp));
extern void	FreeBoxObj ARGS_DECL((struct ObjRec *));

#ifdef _INCLUDE_FROM_BOX_C_
#undef extern
#ifndef _NO_RECURSIVE_EXTERN
#define extern extern
#endif /* ~_NO_RECURSIVE_EXTERN */
#endif /*_INCLUDE_FROM_BOX_C_*/

#endif /*_BOX_E_*/
