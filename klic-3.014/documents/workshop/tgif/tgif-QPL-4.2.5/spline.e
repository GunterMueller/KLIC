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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/spline.e,v 1.25 2011/05/16 16:21:59 william Exp $
 */

#ifndef _SPLINE_E_
#define _SPLINE_E_

extern int	intSplineTension;
extern int	splineTol;
extern int	tighterStructSplines;

extern double	splineFactor3;
extern double	splineFactor4;
extern double	curveToFactor;

#ifdef _INCLUDE_FROM_SPLINE_C_
#undef extern
#define extern
#endif /*_INCLUDE_FROM_SPLINE_C_*/

extern int	ReadSplineTightness ARGS_DECL((char*));
extern void	CalcAutoRetractedArrowAttrBend ARGS_DECL((int Style,
				int X0, int Y0, int X2, int Y2, int *X1,
				int *Y1));
extern IntPoint	*GetStructuredSplinePolyTickMarkVs ARGS_DECL((int *pn_N,
				struct ObjRec *, struct PolyRec *,
				struct PolygonRec *));
extern IntPoint	*GetStructuredSplinePolygonTickMarkVs ARGS_DECL((int *pn_N,
				int NumVs, IntPoint *Vs));
extern IntPoint	*MakeStructuredSplinePolyVertex ARGS_DECL((int *pn_N,
				char **ppszSmooth, int NumVs, IntPoint *Vs));
extern IntPoint	*MakeStructuredSplinePolygonVertex ARGS_DECL((int *pn_N,
				char **ppszSmooth, int NumVs, IntPoint *Vs));
extern XPoint	*MakeSplinePolyVertex ARGS_DECL((int Level, int Curved, int *N,
				int XOff, int YOff, int NumVs, IntPoint *));
extern XPoint	*MakeMultiSplinePolyVertex ARGS_DECL((int Curved, int *N,
				char *Smooth, int XOff, int YOff, int NumVs,
				IntPoint *));
extern XPoint	*MakeSplinePolygonVertex ARGS_DECL((int Level, int Curved,
				int *N, int XOff, int YOff, int NumVs,
				IntPoint *));
extern XPoint	*MakeMultiSplinePolygonVertex ARGS_DECL((int Curved, int *N,
				char *Smooth, int XOff, int YOff, int NumVs,
				IntPoint*));
extern XPoint	*MakeIntSplinePolyVertex ARGS_DECL((int *N, int *CntrlN,
				IntPoint **CntrlVs, int XOff, int YOff,
				int NumVs, IntPoint *));
extern XPoint	*MakeIntSplinePolygonVertex ARGS_DECL((int *N, int *CntrlN,
				IntPoint **CntrlVs, int XOff, int YOff,
				int NumVs, IntPoint*));
extern XPoint	*MakeDoubleIntSplinePolygonVertex ARGS_DECL((int *N,
				int *CntrlN, DoublePoint**CntrlVs, int XOff,
				int YOff, int NumVs, DoublePoint*));
extern void	DumpCurvedPolyPoints ARGS_DECL((FILE*, int Curved, int NumPts,
				IntPoint*, int Indent));
extern void	DumpCurvedPolygonPoints ARGS_DECL((FILE*, int Curved,
				int NumPts, IntPoint*, int Indent));
extern void	DumpMultiCurvedPolyPoints ARGS_DECL((FILE*, char *Smooth,
				int Style, int Curved, int NumPts, IntPoint*,
				int Indent));
extern void	DumpMultiCurvedPolygonPoints ARGS_DECL((FILE*, char *Smooth,
				int Curved, int NumPts, IntPoint*, int Indent));

#ifdef _INCLUDE_FROM_SPLINE_C_
#undef extern
#ifndef _NO_RECURSIVE_EXTERN
#define extern extern
#endif /* ~_NO_RECURSIVE_EXTERN */
#endif /*_INCLUDE_FROM_SPLINE_C_*/

#endif /*_SPLINE_E_*/
