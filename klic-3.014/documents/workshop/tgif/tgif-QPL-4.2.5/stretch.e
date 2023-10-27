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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/stretch.e,v 1.28 2011/05/16 16:21:59 william Exp $
 */

#ifndef _STRETCH_E_
#define _STRETCH_E_

extern int	stretchableText;
extern int	compoundObjWithTextStretchableForPSE;
extern int	rotationIncrement;

extern int	autoRotatePivot;
extern int	rotatePivotAbsXYValid;
extern int	rotatePivotAbsX;
extern int	rotatePivotAbsY;

#ifdef _INCLUDE_FROM_STRETCH_C_
#undef extern
#define extern
#endif /*_INCLUDE_FROM_STRETCH_C_*/

extern int	PtInPolyMark ARGS_DECL((struct ObjRec *, int XOff, int YOff,
		                        int NumPts, IntPoint *, int *Index));
extern int	RetractedArrowAttr ARGS_DECL((struct ObjRec *));
extern int	AutoRetractedArrowAttr ARGS_DECL((struct ObjRec *,
		                                  int CheckVCount));
extern int	AutoCenterAttr ARGS_DECL((struct ObjRec *));
extern void	CenterObjInOBBox ARGS_DECL((struct ObjRec *TextObjPtr,
                                            struct BBRec OBBox,
                                            struct BBRec *OBBoxReturn));
extern struct SelRec	* PtInSelMark ARGS_DECL((int XOff, int YOff,
			                         int * Corner));
extern int	HasOnReshape ARGS_DECL((struct ObjRec *, struct AttrRec **));
extern int	GetVlistIndexFromStretchStructuredSplineInfo ARGS_DECL((
				StretchStructuredSplineInfo*, int ss_index));
extern void	SetIPTInfoForStretchPoly ARGS_DECL((int index, int n, IntPoint*,
				StretchStructuredSplineInfo*));
extern void	SetIPTInfoForStretchPolygon ARGS_DECL((int index, int n,
				IntPoint*, StretchStructuredSplineInfo*));
extern void	SetVsAndVs2ForStretchStructSpline ARGS_DECL((
				StretchStructuredSplineInfo*, int dx, int dy,
				int *pn_num_vs, IntPoint *vs, int *pn_num_vs2,
				IntPoint *vs2));
extern void	FixUpSmoothAndSmooth2ForStretchStructSpline ARGS_DECL((
				int num_vs, char *smooth, int num_vs2,
				char *smooth2));
extern void	EraseHighLightForHinge ARGS_DECL((StretchStructuredSplineInfo*,
				int dx, int dy, int draw_dashed_line,
				int draw_vertices));
extern void	EraseHighLightForSmooth ARGS_DECL((StretchStructuredSplineInfo*,
				int dx, int dy, int draw_dashed_line,
				int draw_vertices));
extern void	EraseHighLightForStretchStructSpline ARGS_DECL((
				StretchStructuredSplineInfo*, int dx, int dy,
				int draw_dashed_line, int draw_vertices));
extern XPoint	*DupVs ARGS_DECL((int *pn_return, XPoint *vs, int n));
extern void	MoveATransformedPoint ARGS_DECL((struct ObjRec *, IntPoint*,
				int abs_dx, int abs_dy));
extern void	UpdateObjForStretchStructSpline ARGS_DECL((struct ObjRec *,
				int n, IntPoint *vs, int abs_dx, int abs_dy,
				StretchStructuredSplineInfo*));
extern void	ShearObj ARGS_DECL((struct ObjRec *, int Corner, double dXShear,
		                    double dYShear, double dXScale,
		                    double dYScale, int *RealLtX,
		                    int *RealLtY));
extern void	StretchSel ARGS_DECL((int XGridOff, int YGridOff,
		                      struct ObjRec *, int Corner));
extern void	ScaleAnEPSObj ARGS_DECL((struct ObjRec *, float * Scale));
extern void	ScaleAllSelObj ARGS_DECL((void));
extern void	PreciseScaleEverything ARGS_DECL((void));
extern void	ScaleObjLikeScaleEverything ARGS_DECL((struct ObjRec *,
		                                       double Scale,
		                                       int Redraw));
extern void	SizeAllSelObj ARGS_DECL((int AbsW, int AbsH));
extern void	SizeAnObj ARGS_DECL((struct ObjRec *ObjPtr,
		                     struct ObjRec *TopOwner,
		                     int AbsW, int AbsH));
extern void	SizeAllSelToGivenWidthHeight ARGS_DECL((int AbsW, int AbsH));
extern void	SizeAllSelToGivenWidth ARGS_DECL((int AbsW));
extern void	SizeAllSelToGivenHeight ARGS_DECL((int AbsH));
extern void	FlipObjHorizontal ARGS_DECL((struct ObjRec *));
extern void	FlipIconHorizontal ARGS_DECL((struct ObjRec *));
extern void	FlipObjVertical ARGS_DECL((struct ObjRec *));
extern void	FlipIconVertical ARGS_DECL((struct ObjRec *));

extern void	SetRotatePivot ARGS_DECL((void));
extern void	SetRotatePivotByObject ARGS_DECL((struct ObjRec *));
extern void	RotateObjForLayout ARGS_DECL((struct ObjRec *, double, int));
extern void	RotateObj ARGS_DECL((struct ObjRec *, int Corner,
		                     int AngleDelta, int *RealLtX,
		                     int *RealLtY));
extern void	RotateObjClockWise ARGS_DECL((struct ObjRec *));
extern void	RotateIconClockWise ARGS_DECL((struct ObjRec *));
extern void	RotateObjCounter ARGS_DECL((struct ObjRec *));
extern void	RotateIconCounter ARGS_DECL((struct ObjRec *));
extern void	FlipHorizontal ARGS_DECL((void));
extern void	FlipVertical ARGS_DECL((void));
extern void	RotateClockWise ARGS_DECL((void));
extern void	RotateCounter ARGS_DECL((void));
extern void	SetTextRotation ARGS_DECL((char*));
extern void	SetRotationIncrement ARGS_DECL((char*));
extern void	SetSelCTM ARGS_DECL((int nTransformed, struct XfrmMtrxRec *));
extern void	RotateShearSel ARGS_DECL((int XGridOff, int YGridOff,
		                          struct ObjRec *, int Corner));

extern TgMenu	*CreateFlipRotateMenu ARGS_DECL((TgMenu*, int X, int Y,
		                                 TgMenuInfo*,
		                                 int status_str_xlated));
extern void	HighLightRotatePivot ARGS_DECL((int dir));
extern int	PtInRotatePivot ARGS_DECL((int mouse_x, int mouse_y));
extern TgMenu	*CreateMoveStdRotatePivotMenu ARGS_DECL((TgMenu*, int X, int Y,
				TgMenuInfo*, int status_str_xlated));
extern void	RefreshAutoRotatePivotMenu ARGS_DECL((TgMenu*));
extern TgMenu	*CreateAutoRotatePivotMenu ARGS_DECL((TgMenu*, int X, int Y,
                                                      TgMenuInfo*,
                                                      int status_str_xlated));
extern int	AutoRotatePivotMenu ARGS_DECL((int X, int Y, int TrackMenubar));

extern void	ResetRotatePivotValidInfo ARGS_DECL((void));
extern int	ReadRotatePivotInfo ARGS_DECL((char*));
extern void	ContinueMoveRotatePivot ARGS_DECL((int OrigX, int OrigY));
extern void	ToggleAutoRotatePivot ARGS_DECL((void));
extern void	SpecifyRotatePivot ARGS_DECL((void));
extern void	ResetRotatePivot ARGS_DECL((void));
extern void	NextPolyRotationPivot ARGS_DECL((void));
extern void	MoveRotationPivotToArcCenter ARGS_DECL((void));
extern void	MoveRotatePivot ARGS_DECL((int Corner));
extern void	AutoRotatePivotSubMenu ARGS_DECL((int index));

#ifdef _INCLUDE_FROM_STRETCH_C_
#undef extern
#ifndef _NO_RECURSIVE_EXTERN
#define extern extern
#endif /* ~_NO_RECURSIVE_EXTERN */
#endif /*_INCLUDE_FROM_STRETCH_C_*/

#endif /*_STRETCH_E_*/
