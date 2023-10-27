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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/ruler.e,v 1.7 2011/05/16 16:21:59 william Exp $
 */

#ifndef _RULER_E_
#define _RULER_E_

extern int	showMeasurement;
extern int	showMeasurementInTooltip;
extern int	simpleMotionInDrawWin;
extern int	showCrossHair;

#ifdef _INCLUDE_FROM_RULER_C_
#undef extern
#define extern
#endif /*_INCLUDE_FROM_RULER_C_*/

extern void	GetUnitSpec ARGS_DECL((char*));
extern void	ShowUnitMsg ARGS_DECL((void));
extern int	SetUnit ARGS_DECL((char*));
extern void	InitRuler ARGS_DECL((void));
extern void	CleanUpRuler ARGS_DECL((void));
extern void	PixelToMeasurementUnit ARGS_DECL((char *Buf, int NumPixels));
extern void	SquarePixelToMeasurementUnit ARGS_DECL((char *Buf,
                                                        int NumSquarePixels));
extern void	PixelToCurrentUnit ARGS_DECL((char *Buf, int NumPixels));
extern void	RedrawHRuler ARGS_DECL((Display*, Window));
extern void	RedrawHRulerWindow ARGS_DECL((void));
extern void	RedrawVRuler ARGS_DECL((Display*, Window));
extern void	RedrawVRulerWindow ARGS_DECL((void));
extern void	MarkRulers ARGS_DECL((int XOff, int YOff));
extern void	RedrawRulers ARGS_DECL((void));
extern void	GetCrossHairPosition ARGS_DECL((int *XOff, int *YOff,
		                                int *Shown));
extern void	RedrawCrossHair ARGS_DECL((void));
extern void	ToggleShowCrossHair ARGS_DECL((void));
extern void	RulersEventHandler ARGS_DECL((XEvent *));
extern void	FreezeMarkRulerText ARGS_DECL((void));
extern void	UnFreezeMarkRulerText ARGS_DECL((void));
extern int	VerboseMeasureTooltip ARGS_DECL((void));
extern void	BeginIntervalRulers ARGS_DECL((int LtX, int LtY, int RbX,
		                               int RbY));
extern void	DrawIntervalRulers ARGS_DECL((int LtX, int LtY, int RbX,
		                              int RbY, char *delta_str));
extern void	EndIntervalRulers ARGS_DECL((int X, int Y));
extern void	StartShowMeasureCursor ARGS_DECL((int XOff, int YOff,
                                                  char *Str, int ExtraSpace));
extern void	ShowMeasureCursor ARGS_DECL((int XOff, int YOff, char *Str,
                                             int ExtraSpace));
extern void	EndShowMeasureCursor ARGS_DECL((int XOff, int YOff, char *Str,
                                                int ExtraSpace));
extern void	ToggleShowMeasurement ARGS_DECL((void));
extern void	ToggleShowMeasurementInTooltip ARGS_DECL((void));

extern void	DoIntervalMeasureTooltip ARGS_DECL((int ltx, int lty, int rbx,
		                                    int rby, int mdx, int mdy,
		                                    char *delta_str));
extern void	StartMeasureTooltip ARGS_DECL((char*));
extern void	SetMeasureTooltip ARGS_DECL((char*));
extern void	EndMeasureTooltip ARGS_DECL((int force));
extern void	CleanUpMeasureTooltip ARGS_DECL((void));
extern int	InitMeasureTooltip ARGS_DECL((void));

#ifdef _INCLUDE_FROM_RULER_C_
#undef extern
#ifndef _NO_RECURSIVE_EXTERN
#define extern extern
#endif /* ~_NO_RECURSIVE_EXTERN */
#endif /*_INCLUDE_FROM_RULER_C_*/

#endif /*_RULER_E_*/
