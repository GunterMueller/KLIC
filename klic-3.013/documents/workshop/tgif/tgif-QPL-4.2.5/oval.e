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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/oval.e,v 1.8 2011/06/09 16:21:18 william Exp $
 */

#ifndef _OVAL_E_
#define _OVAL_E_

extern int	ovalDrawn;

#ifdef _INCLUDE_FROM_OVAL_C_
#undef extern
#define extern
#endif /*_INCLUDE_FROM_OVAL_C_*/

extern void	MyOval ARGS_DECL((Window, GC, struct BBRec));
extern void	DumpOvalObj ARGS_DECL((FILE *, struct ObjRec *));
extern int	NeedsToCacheOvalObj ARGS_DECL((struct ObjRec *));
extern void	CreateOvalObj ARGS_DECL((struct BBRec *, int CreateAbsolute));
extern void	DrawOvalObj ARGS_DECL((Window, int XOff, int YOff,
		                       struct ObjRec *));
extern void	DrawOval ARGS_DECL((XEvent *));
extern void	MakeOvalObjFromBoundingBox ARGS_DECL((void));
extern void	SaveOvalObj ARGS_DECL((FILE *, struct ObjRec *));
extern void	ReadOvalObj ARGS_DECL((FILE *, char *Inbuf, struct ObjRec **));
extern void	SetOvalPropMask ARGS_DECL((struct ObjRec *, long *plMask,
		                           long *plSkip,
		                           struct PropertiesRec *pProp));
extern void	FreeOvalObj ARGS_DECL((struct ObjRec *));

#ifdef _INCLUDE_FROM_OVAL_C_
#undef extern
#ifndef _NO_RECURSIVE_EXTERN
#define extern extern
#endif /* ~_NO_RECURSIVE_EXTERN */
#endif /*_INCLUDE_FROM_OVAL_C_*/

#endif /*_OVAL_E_*/
