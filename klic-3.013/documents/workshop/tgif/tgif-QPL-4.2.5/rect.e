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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/rect.e,v 1.6 2011/05/16 16:21:59 william Exp $
 */

#ifndef _RECT_E_
#define _RECT_E_

#ifdef _INCLUDE_FROM_RECT_C_
#undef extern
#define extern
#endif /*_INCLUDE_FROM_RECT_C_*/

extern void	DumpRectPath ARGS_DECL((FILE *, int LtX, int LtY, int RbX,
		                        int RbY, int Indent, int LastLF));
extern void	SetPoint ARGS_DECL((IntPoint*, int X, int Y));
extern void	ClipRect ARGS_DECL((struct BBRec *, int W, int H));
extern int	CompareRect ARGS_DECL((struct BBRec *, struct BBRec *));
extern void	SetRotateVs ARGS_DECL((XPoint *, int LtX, int LtY, int RbX,
		                       int RbY));
extern void	SetBBRec ARGS_DECL((struct BBRec *, int LtX, int LtY, int RbX,
		                    int RbY));
extern void	ConcatCTM ARGS_DECL((struct XfrmMtrxRec *ctm,
		                     struct XfrmMtrxRec *orig_ctm,
		                     struct XfrmMtrxRec *new_ctm));
extern void	TransformDoublePointThroughCTM ARGS_DECL((double X, double Y,
		                                          struct XfrmMtrxRec *,
		                                          double *NewX,
		                                          double *NewY));
extern void	TransformPointThroughCTM ARGS_DECL((int X, int Y,
		                                    struct XfrmMtrxRec *,
		                                    int *NewX, int *NewY));
extern void	ReverseTransformDoublePointThroughCTM ARGS_DECL((double X,
		                                           double Y,
		                                           struct XfrmMtrxRec *,
		                                           double *NewDx,
		                                           double *NewDy));
extern void	ReverseTransformPointThroughCTM ARGS_DECL((int X, int Y,
		                                           struct XfrmMtrxRec *,
		                                           int *NewX,
		                                           int *NewY));
extern void	TransformObjectV ARGS_DECL((struct ObjRec *,
		                            IntPoint *VIn, IntPoint *VOut));
extern void	ReversedTransformObjectV ARGS_DECL((struct ObjRec *,
		                                   IntPoint *VIn,
		                                   IntPoint *VOut));
extern void	TransformOffsetBBoxThroughCTM ARGS_DECL((struct BBRec *,
		                                         struct XfrmMtrxRec *,
		                                         IntPoint *));
extern void	GetTransformedOBBoxOffsetVs ARGS_DECL((struct ObjRec *,
		                                       XPoint *));
extern void	GetTransformedOBBoxAbsVs ARGS_DECL((struct ObjRec *,
		                                    IntPoint *));
extern void	SetCTM ARGS_DECL((struct ObjRec *, struct XfrmMtrxRec *));
extern void	ExpandExtents ARGS_DECL((struct BBRec *pBBox, int *pnLtX,
		                         int *pnLtY, int *pnRbX, int *pnRbY));
extern void	ExpandBBox ARGS_DECL((struct BBRec *pBBox,
		                      struct BBRec *pBBoxToExpand));
extern void	InflateBBox ARGS_DECL((struct BBRec *pBBoxIn, int Dx, int Dy,
		                       struct BBRec *pBBoxOut));
extern void	OffsetBBox ARGS_DECL((struct BBRec *pBBoxIn, int Dx, int Dy,
		                      struct BBRec *pBBoxOut));
extern int	BBoxIntersect ARGS_DECL((struct BBRec BBox1,
		                         struct BBRec BBox2));
extern int	IntersectRect ARGS_DECL((struct BBRec BBox1, struct BBRec BBox2,
		                         struct BBRec *BBox3));
extern void	UnionRect ARGS_DECL((struct BBRec *BBox1, struct BBRec *BBox2,
		                     struct BBRec *UnionBBox));
extern int	Inside ARGS_DECL((struct BBRec Rect1, struct BBRec Rect2));
extern int	PointInBBox ARGS_DECL((int X, int Y, struct BBRec Rect));
extern int	PointInIntPolygon ARGS_DECL((int X, int Y, int NumPts,
		                             IntPoint *));
extern int	PointInPolygon ARGS_DECL((int X, int Y, int NumPts, XPoint *));
extern int	PointInPoly ARGS_DECL((int X, int Y, int NumPts, XPoint *,
		                       int LineWidth));
extern int	PointOnPoly ARGS_DECL((int X, int Y, int NumPts, XPoint *,
		                       int LineWidth));
extern int	FindGoodText ARGS_DECL((int XOff, int YOff, struct ObjRec *));
extern int	FindGoodXBm ARGS_DECL((int XOff, int YOff, struct ObjRec *));
extern int	FindGoodXPm ARGS_DECL((int XOff, int YOff, struct ObjRec *));
extern int	FindGoodBox ARGS_DECL((int XOff, int YOff, struct ObjRec *));
extern int	FindGoodRCBox ARGS_DECL((int XOff, int YOff, struct ObjRec *));
extern int	FindGoodOval ARGS_DECL((int XOff, int YOff, struct ObjRec *));
extern int	FindGoodPoly ARGS_DECL((int XOff, int YOff, struct ObjRec *));
extern int	FindGoodPolygon ARGS_DECL((int XOff, int YOff,
		                           struct ObjRec *));
extern int	FindGoodArc ARGS_DECL((int XOff, int YOff, struct ObjRec *));
extern int	FindGoodObj ARGS_DECL((int XOff, int YOff, struct ObjRec *First,
		                       struct ObjRec **SubObj,
		                       struct ObjRec **ImmediateChildObj));

#ifdef _INCLUDE_FROM_RECT_C_
#undef extern
#ifndef _NO_RECURSIVE_EXTERN
#define extern extern
#endif /* ~_NO_RECURSIVE_EXTERN */
#endif /*_INCLUDE_FROM_RECT_C_*/

#endif /*_RECT_E_*/
