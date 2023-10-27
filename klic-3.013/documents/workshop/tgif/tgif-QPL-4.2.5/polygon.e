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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/polygon.e,v 1.6 2011/05/16 16:21:59 william Exp $
 */

#ifndef _POLYGON_E_
#define _POLYGON_E_

extern int	polygonDrawn;

#ifdef _INCLUDE_FROM_POLYGON_C_
#undef extern
#define extern
#endif /*_INCLUDE_FROM_POLYGON_C_*/

extern XPoint	* MakePolygonVertex ARGS_DECL((int XOff, int YOff, int NumVs,
		                               IntPoint *));
extern void	DumpPoints ARGS_DECL((FILE *, int NumPts, IntPoint *,
		                      int Indent));
extern void	DumpPolygonObj ARGS_DECL((FILE *, struct ObjRec *));
extern int	NeedsToCachePolygonObj ARGS_DECL((struct ObjRec *));
extern void	DrawPolygonObj ARGS_DECL((Window, int XOff, int YOff,
		                          struct ObjRec *));
extern void	CreatePolygonObj ARGS_DECL((int NumPts, int CreateAbsolute));
extern void	ResetCreatePolygon ARGS_DECL((void));
extern void	AddPtToCreatePolygon ARGS_DECL((int AbsX, int AbsY));
extern void	DrawPolygon ARGS_DECL((XEvent *));
extern void	SelectAndHighLightNewObjects ARGS_DECL((struct ObjRec *));
extern void	InputPolygonPts ARGS_DECL((void));
extern void	GetBoundingBox ARGS_DECL((void));
extern void	SavePolygonObj ARGS_DECL((FILE *, struct ObjRec *));
extern void	ReadPolygonObj ARGS_DECL((FILE *, char *Inbuf,
		                          struct ObjRec **));
extern void	SetPolygonPropMask ARGS_DECL((struct ObjRec *, long *plMask,
		                              long *plSkip,
		                              struct PropertiesRec *pProp));
extern void	FreePolygonObj ARGS_DECL((struct ObjRec *));

#ifdef _INCLUDE_FROM_POLYGON_C_
#undef extern
#ifndef _NO_RECURSIVE_EXTERN
#define extern extern
#endif /* ~_NO_RECURSIVE_EXTERN */
#endif /*_INCLUDE_FROM_POLYGON_C_*/

#endif /*_POLYGON_E_*/
