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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/rcbox.e,v 1.7 2011/06/09 15:55:15 cvsps Exp $
 */

#ifndef _RCBOX_E_
#define _RCBOX_E_

extern int	rcBoxDrawn;

#ifdef _INCLUDE_FROM_RCBOX_C_
#undef extern
#define extern
#endif /*_INCLUDE_FROM_RCBOX_C_*/

extern void	SetRCBoxVertex ARGS_DECL((int X1, int Y1, int X2, int Y2,
		                          int Radius));
extern void	MyRCBox ARGS_DECL((Window, GC, int X1, int Y1, int X2, int Y2,
		                   int Radius));
extern void	DumpRCBoxObj ARGS_DECL((FILE *, struct ObjRec *));
extern int	NeedsToCacheRCBoxObj ARGS_DECL((struct ObjRec *));
extern void	CreateRCBoxObj ARGS_DECL((int X1, int Y1, int X2, int Y2,
		                          int CreateAbsolute));
extern void	DrawRCBoxObj ARGS_DECL((Window, int XOff, int YOff,
		                        struct ObjRec *));
extern void	DrawRCBox ARGS_DECL((XEvent *));
extern void	MakeRCBoxObjFromBoundingBox ARGS_DECL((void));
extern void	SaveRCBoxObj ARGS_DECL((FILE *, struct ObjRec *));
extern void	ReadRCBoxObj ARGS_DECL((FILE *, char *Inbuf, struct ObjRec **));
extern void	SetRCBoxPropMask ARGS_DECL((struct ObjRec *, long *plMask,
		                            long *plSkip,
		                            struct PropertiesRec *pProp));
extern void	FreeRCBoxObj ARGS_DECL((struct ObjRec *));

#ifdef _INCLUDE_FROM_RCBOX_C_
#undef extern
#ifndef _NO_RECURSIVE_EXTERN
#define extern extern
#endif /* ~_NO_RECURSIVE_EXTERN */
#endif /*_INCLUDE_FROM_RCBOX_C_*/

#endif /*_RCBOX_E_*/
