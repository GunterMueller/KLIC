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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/imgproc.e,v 1.18 2011/05/16 16:21:58 william Exp $
 */

#ifndef _IMGPROC_E_
#define _IMGPROC_E_

extern int numImageProc;
extern int gnInImageProc;
extern int gnConvolving;
extern int gnNumNewColorsInPixmapFile;
extern char gszImageProcXPmFile[MAXPATHLENGTH+1];

extern float gfVectorWarpSoftness;

extern int threshFillReplaceEnabled;
extern int fillReplaceRedThresh;
extern int fillReplaceGreenThresh;
extern int fillReplaceBlueThresh;

#ifdef _INCLUDE_FROM_IMGPROC_C_
#undef extern
#define extern
#endif /*_INCLUDE_FROM_IMGPROC_C_*/

extern void	CleanUpConvolution ARGS_DECL((void));
extern int	DoConvolution ARGS_DECL((FILE*, XImage *image,
		                         XImage *bitmap_image, int W, int H,
		                         struct XPmRec *));
extern FILE	*GetImageProcOutputFileName ARGS_DECL((char *pszPath,
		                                       int path_buf_sz,
		                                       int *pnShortName,
		                                       char **ppszRest));

extern void	RGBtoHSV ARGS_DECL((int r, int g, int b, int *h, double *s,
		                    int *v));
extern void	HSVtoRGB ARGS_DECL((int h, double s, int v, int *r, int *g,
		                    int *b));
extern void	MakeGray ARGS_DECL((void));
extern void	InvertColor ARGS_DECL((void));
extern void	InterpolateColor ARGS_DECL((void));
extern void	BrightenDarken ARGS_DECL((void));
extern void	ChangeSaturation ARGS_DECL((void));
extern void	ChangeHue ARGS_DECL((void));
extern void	ContrastEnhance ARGS_DECL((void));
extern void	ColorBalance ARGS_DECL((void));
extern void	Gamma ARGS_DECL((char*));
extern void	EdgeDetect ARGS_DECL((void));
extern void	Emboss ARGS_DECL((void));
extern void	ReduceColors ARGS_DECL((void));
extern void	ReduceToPixmapColors ARGS_DECL((void));
extern void	ReduceToMobileWebSafeColors ARGS_DECL((void));
extern void	SetDefaultColorLevels ARGS_DECL((void));
extern void	ReduceToDefaultColors ARGS_DECL((void));
extern void	DefaultErrorDiffuse ARGS_DECL((void));
extern void	Spread ARGS_DECL((void));
extern void	Sharpen ARGS_DECL((void));
extern void	Blur3 ARGS_DECL((void));
extern void	Blur5 ARGS_DECL((void));
extern void	Blur7 ARGS_DECL((void));
extern void	RunBggen ARGS_DECL((void));
extern void	CircularBggen ARGS_DECL((void));
extern void	SimpleRectBggen ARGS_DECL((void));
extern void	RegenerateImage ARGS_DECL((void));
extern void	CreatePixmapFromSelected ARGS_DECL((void));
extern void	CropImage ARGS_DECL((void));
extern void	GetColor ARGS_DECL((void));
extern void	ReplaceColor ARGS_DECL((void));
extern void	ReplaceColorWithTrans ARGS_DECL((void));
extern void	FloodFill ARGS_DECL((void));
extern void	ToggleFloodReplaceColorThreshold ARGS_DECL((void));
extern void	SetFloodReplaceColorThreshold ARGS_DECL((void));
extern void	CreateContour ARGS_DECL((void));
extern void	Subtract ARGS_DECL((void));
extern void	XorColors ARGS_DECL((void));
extern void	AlphaCombine ARGS_DECL((void));
extern int	BuildTrapMapNode ARGS_DECL((TrapMapSpec*, int*, TrapMap*));
extern void	VectorWarp ARGS_DECL((void));

extern void	CleanUpImageProc ARGS_DECL((void));
extern void	InitImageProc ARGS_DECL((void));
extern int	RefreshImageProcMenu ARGS_DECL((TgMenu*));
extern TgMenu	*CreateImageProcMenu ARGS_DECL((TgMenu*, int X, int Y,
		                                TgMenuInfo*,
		                                int status_str_xlated));
extern int	ImageProcMenu ARGS_DECL((int X, int Y, int TrackMenubar));

#ifdef _INCLUDE_FROM_IMGPROC_C_
#undef extern
#ifndef _NO_RECURSIVE_EXTERN
#define extern extern
#endif /* ~_NO_RECURSIVE_EXTERN */
#endif /*_INCLUDE_FROM_IMGPROC_C_*/

#endif /*_IMGPROC_E_*/
