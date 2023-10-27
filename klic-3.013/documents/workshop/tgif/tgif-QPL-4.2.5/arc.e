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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/arc.e,v 1.6 2011/05/16 16:21:56 william Exp $
 */

#ifndef _ARC_E_
#define _ARC_E_

extern int	arcDrawn;

#ifdef _INCLUDE_FROM_ARC_C_
#undef extern
#define extern
#endif /*_INCLUDE_FROM_ARC_C_*/

extern void	PointsToArc ARGS_DECL((int xc, int yc, int x1, int y1,
		                       int x2, int y2, int dir, int int_degree,
		                       int *ltx, int *lty, int *w, int *h,
		                       int *angle1, int *angle2));
extern void	ArcRealX2Y2 ARGS_DECL((struct ArcRec *, int *X2, int *Y2));
extern void	GetArcArrowInfo ARGS_DECL((struct ObjRec *,
		                           IntPoint *tipvs1, IntPoint *tailvs1,
		                           IntPoint *vs1, int *a_angle1,
		                           IntPoint *tipvs2, IntPoint *tailvs2,
		                           IntPoint *vs2, int *a_angle2));
extern void	CalcArcOBBox ARGS_DECL((struct ObjRec *));
extern void	CalcArcBBox ARGS_DECL((struct ObjRec *, struct BBRec obbox,
		                       struct BBRec *bbox));
extern void	DumpArcObj ARGS_DECL((FILE *, struct ObjRec *));
extern int	NeedsToCacheArcObj ARGS_DECL((struct ObjRec *));
extern void	DrawArcObj ARGS_DECL((Window, int X, int Y, struct ObjRec *));
extern struct ObjRec	*CreateArcObj ARGS_DECL((int xc, int yc, int x1, int y1,
			                         int x2, int y2, int dir,
			                         int ltx, int lty, int w, int h,
			                         int angle1, int angle2,
			                         int CreateAbsolute));
extern void	DrawArc ARGS_DECL((XEvent *));
extern void	SaveArcObj ARGS_DECL((FILE *, struct ObjRec *));
extern void	ReadArcObj ARGS_DECL((FILE *, char *, struct ObjRec **));
extern void	SetArcPropMask ARGS_DECL((struct ObjRec *, long *plMask,
		                          long *plSkip,
		                          struct PropertiesRec *pProp));
extern void	FreeArcObj ARGS_DECL((struct ObjRec *));
extern void	MakePreciseArc ARGS_DECL((void));
extern void	PreciseRotateAnArc ARGS_DECL((void));

#ifdef _INCLUDE_FROM_ARC_C_
#undef extern
#ifndef _NO_RECURSIVE_EXTERN
#define extern extern
#endif /* ~_NO_RECURSIVE_EXTERN */
#endif /*_INCLUDE_FROM_ARC_C_*/

#endif /*_ARC_E_*/
