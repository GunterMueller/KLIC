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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/edit.e,v 1.23 2011/05/16 16:21:57 william Exp $
 */

#ifndef _EDIT_E_
#define _EDIT_E_

extern struct SelRec *outerSelForFind;
extern struct SelRec *innerSelForFind;

extern int ignoreObjectShadowInfoInFile;
extern int objShadowXOffset;
extern int objShadowYOffset;
extern char objShadowColorStr[];

extern int pngExportHasTransparentColor;

#ifdef _INCLUDE_FROM_EDIT_C_
#undef extern
#define extern
#endif /*_INCLUDE_FROM_EDIT_C_*/

extern void	CleanOuterInnerSelForFind ARGS_DECL((void));
extern void	CleanUpEdit ARGS_DECL((void));
extern void	InitEdit ARGS_DECL((void));
extern int	CanPerformImageProc ARGS_DECL((void));
extern void	ConvertIntSpline ARGS_DECL((void));
extern void	ConvertToBezier ARGS_DECL((void));
extern int	SetBezierConvertNumSegsValue ARGS_DECL((char*));
extern void	SetBezierConvertNumSegs ARGS_DECL((char*));
extern void	ToggleSmoothHinge ARGS_DECL((void));
extern void	DoBreakUpText ARGS_DECL((char *spec, int forced_justify));
extern void	BreakUpText ARGS_DECL((char *spec));
extern void	SetTextFillPatternColor ARGS_DECL((void));
extern void	MakeRegularPolygon ARGS_DECL((void));
extern int	DeleteStructuredSplinePoint ARGS_DECL((int index,
				struct PolyRec *, struct PolygonRec *));
extern void	DeletePoint ARGS_DECL((void));
extern void	AddPoint ARGS_DECL((void));
extern void	FlushUndoBuffer ARGS_DECL((void));
extern void	RestoreImageWH ARGS_DECL((void));
extern void	CutMaps ARGS_DECL((void));
extern void	BreakUpMaps ARGS_DECL((void));
extern void	RemoveTransparentPixel ARGS_DECL((void));
extern void	LayoutOnArc ARGS_DECL((void));
extern void	PreciseRotate ARGS_DECL((void));
extern void	RotateAllSelObj ARGS_DECL((double degrees));
extern void	NoTransform ARGS_DECL((void));
extern void	SetEditTextSize ARGS_DECL((void));
extern void	FindCaseSensitive ARGS_DECL((void));
extern void	FindNoCase ARGS_DECL((void));
extern void	FindAgain ARGS_DECL((void));
extern int	RefreshEditMenu ARGS_DECL((TgMenu*));
extern TgMenu	*CreateEditMenu ARGS_DECL((TgMenu*, int X, int Y, TgMenuInfo*,
		                           int status_str_xlated));
extern int	EditMenu ARGS_DECL((int X, int Y, int TrackMenubar));
extern void	FrontProc ARGS_DECL((void));
extern void	BackProc ARGS_DECL((void));
extern void	AlignObjsTop ARGS_DECL((void));
extern void	AlignObjsMiddle ARGS_DECL((void));
extern void	AlignObjsBottom ARGS_DECL((void));
extern void	AlignObjsLeft ARGS_DECL((void));
extern void	AlignObjsCenter ARGS_DECL((void));
extern void	AlignObjsRight ARGS_DECL((void));
extern void	AbutHorizontal ARGS_DECL((void));
extern void	AbutVertical ARGS_DECL((void));
extern void	RefreshArrangeMenu ARGS_DECL((TgMenu*));
extern int	ArrangeMenu ARGS_DECL((int X, int Y, int TrackMenubar));
extern int	RefreshPropertiesMenu ARGS_DECL((TgMenu*));
extern TgMenu	*CreatePropertiesMenu ARGS_DECL((TgMenu*, int X, int Y,
		                                 TgMenuInfo*,
		                                 int status_str_xlated));
extern int	PropertiesMenu ARGS_DECL((int X, int Y, int TrackMenubar));
extern void	UpdateSymbols ARGS_DECL((void));

extern void	SizeToWidest ARGS_DECL((void));
extern void	SizeToNarrowest ARGS_DECL((void));
extern void	SizeToTallest ARGS_DECL((void));
extern void	SizeToShortest ARGS_DECL((void));
extern void	SizeToGivenWidthHeight ARGS_DECL((void));
extern void	SizeToGivenWidth ARGS_DECL((void));
extern void	SizeToGivenHeight ARGS_DECL((void));
extern void	SetObjectShadowColor ARGS_DECL((void));
extern void	SetObjectShadowOffsets ARGS_DECL((void));
extern void	AddObjectShadow ARGS_DECL((void));
extern void	RemoveObjectShadow ARGS_DECL((void));
extern int	ReadObjectShadowInfo ARGS_DECL((char*));
extern void	ExtendSegment ARGS_DECL((void));
extern void	ToggleTighterStructuredSplines ARGS_DECL((void));
extern int	RefreshPolySplineMenu ARGS_DECL((TgMenu*));
extern TgMenu	*CreatePolySplineMenu ARGS_DECL((TgMenu*, int X, int Y,
		                                 TgMenuInfo*,
		                                 int status_str_xlated));

#ifdef _INCLUDE_FROM_EDIT_C_
#undef extern
#ifndef _NO_RECURSIVE_EXTERN
#define extern extern
#endif /* ~_NO_RECURSIVE_EXTERN */
#endif /*_INCLUDE_FROM_EDIT_C_*/

#endif /*_EDIT_E_*/
