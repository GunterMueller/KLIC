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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/poly.e,v 1.11 2011/05/16 16:21:59 william Exp $
 */

#ifndef _POLY_E_
#define _POLY_E_

#include "list.e"

typedef struct tagWiringInfo {
   int num_ports_to_connect; /* this used to be drawPolyToEndInANode */

   struct ObjRec *first_port_obj, *first_port_owner_obj;
   char first_port_name[MAXSTRING]; /* this used to be drawPolyFirstNodeName */

   struct ObjRec *last_port_obj, *last_port_owner_obj;
   char last_port_name[MAXSTRING]; /* this used to be drawPolyLastNodeName */
} WiringInfo;

extern WiringInfo	gstWiringInfo;

extern int	polyDrawn;
extern int	queryPolyNameOnConnect;

extern short	origWidthOfLine[];
extern short	origArrowHeadH[];
extern short	origArrowHeadW[];

extern short	*curWidthOfLine;
extern short	*curArrowHeadH;
extern short	*curArrowHeadW;

extern char	**curWidthOfLineSpec;
extern char	**curArrowHeadHSpec;
extern char	**curArrowHeadWSpec;

extern int	numPtsInPoly;
extern CVList	gStructSplineList;

extern struct PtRec	*lastPtPtr;
extern struct ObjRec	* drawPolyHighlightedNode;

#ifdef _INCLUDE_FROM_POLY_C_
#undef extern
#define extern
#endif /*_INCLUDE_FROM_POLY_C_*/

extern IntPoint	*GetPolyOrPolygonAbsVs ARGS_DECL((int *pnReturn,
                                                  int *pnAllocated,
                                                  struct ObjRec *));
extern XPoint	*MakePolyVertex ARGS_DECL((int XOff, int YOff, int NumVs,
		                            IntPoint*));
extern void	CalcPolyBBox ARGS_DECL((struct ObjRec *));
extern void	UpdPolyBBox ARGS_DECL((struct ObjRec *, int NumPts,
		                       IntPoint*));
extern void	UpdNonIntSplinePolyBBox ARGS_DECL((struct ObjRec *, int NumPts,
		                                   IntPoint*));
extern void	UpdPolyOrPolygonBBox ARGS_DECL((struct ObjRec *));
extern void	CreatePolyObj ARGS_DECL((int NumPts, int CreateAbsolute));
extern void	ResetCreatePoly ARGS_DECL((void));
extern void	AddPtToCreatePoly ARGS_DECL((int AbsX, int AbsY));
extern void	FreePointsForCont ARGS_DECL((int *pn_ltx, int *pn_lty,
		                             int *pn_rbx, int *pn_rby));
extern int	SetFirstPoint ARGS_DECL((int grid_x, int grid_y,
		                         IntPointTriplet*));
extern int	AddPointForCont ARGS_DECL((int grid_x, int grid_y,
		                           IntPointTriplet*));
extern void	UpdateLastPointForCont ARGS_DECL((IntPointTriplet*));
extern void	DrawAllStructSplinePointsForCont ARGS_DECL((int num_pts));
extern void	UpdatePrevToLastPointForCont ARGS_DECL((IntPointTriplet*));
extern void	DrawPoly ARGS_DECL((XEvent*));
extern void	SetWiringNodeInfo ARGS_DECL((struct ObjRec *port_obj,
		                             struct ObjRec *port_owner_obj,
		                             char *port_name, int first));
extern void	ResetWiringNodeInfo ARGS_DECL((void));
extern void	InputPolyPts ARGS_DECL((void));
extern void	JoinPoly ARGS_DECL((void));
extern void	CutPoly ARGS_DECL((void));
extern double	GetWidthInDouble ARGS_DECL((int nVal, char *pszSpec,
		                            int *pnIsInt));
extern void	SetWidthAndSpec ARGS_DECL((double dWidth, int *pnWidth,
		                           char *pszWidthSpec));
extern void	ScaleWidthAndSpec ARGS_DECL((double dScale, int *pnWidth,
		                             char *pszWidthSpec));
extern void	DumpArrow ARGS_DECL((FILE*, IntPoint *TailV, IntPoint *HeadV,
		                     int ArrowW, int ArrowH, char *AWSpec,
		                     char *AHSpec, int Pen, int TransPat,
		                     int ColorIndex));
extern void	DumpPolyObj ARGS_DECL((FILE*, struct ObjRec *));
extern int	NeedsToCachePolyObj ARGS_DECL((struct ObjRec *));
extern void	DrawPolyObj ARGS_DECL((Window, int XOff, int YOff,
		                       struct ObjRec *));
extern void	SaveSmoothHinge ARGS_DECL((FILE*, int Curved, int NumPts,
		                           char *Smooth));
extern void	SavePolyObj ARGS_DECL((FILE*, struct ObjRec *));
extern int	ReadSmoothHinge ARGS_DECL((FILE*, int Curved, int NumPts,
		                           char *Smooth));
extern void	ReadPolyObj ARGS_DECL((FILE*, char *Inbuf, struct ObjRec **));
extern void	SetPolyPropMask ARGS_DECL((struct ObjRec *, long *plMask,
		                           long *plSkip,
		                           struct PropertiesRec *pProp));
extern void	FreePolyObj ARGS_DECL((struct ObjRec *));
extern void	InitPoly ARGS_DECL((void));

#ifdef _INCLUDE_FROM_POLY_C_
#undef extern
#ifndef _NO_RECURSIVE_EXTERN
#define extern extern
#endif /* ~_NO_RECURSIVE_EXTERN */
#endif /*_INCLUDE_FROM_POLY_C_*/

#endif /*_POLY_E_*/
