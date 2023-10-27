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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/xpixmap.e,v 1.23 2011/05/16 16:22:00 william Exp $
 */

#ifndef _XPIXMAP_E_
#define _XPIXMAP_E_

typedef struct tagXpmTransIndexInfo {
   int has_transparent_pixel;
   int found_transparent_pixel;
   int transparent_pixel_index;
   int *dump_index_to_color_index;
} XpmTransIndexInfo;

extern GC	xpmGC;
extern int	newColormapUsed;
extern int	allocColorFailed;
extern int	myReadTransparentPixmap;
extern int	xpmIsArray;
extern long	xpmArraySeek;
extern int	xpmHasTooManyColorsForPrinting;
extern int	hasReadHexString;

extern PngHeaderInfo	gPngHeaderInfo;

extern double	rotatedSine[];
extern double	rotatedCosine[];

#ifdef _INCLUDE_FROM_XPIXMAP_C_
#undef extern
#define extern
#endif /*_INCLUDE_FROM_XPIXMAP_C_*/

extern void	ResetXPmErrorMessage ARGS_DECL((void));
extern void	InitXPm ARGS_DECL((void));
extern void	CleanUpXPm ARGS_DECL((void));
extern int	IsLinkedJpegObj ARGS_DECL((struct ObjRec *));
extern int	IsPpmTrueObj ARGS_DECL((int real_type, int ppm_data_compress,
		                        char *ppm_data));
extern int	ObjHasIndexedTransPixel ARGS_DECL((struct ObjRec *,
		                                   int *pn_index));
extern int	ObjHasTrueColorTransPixel ARGS_DECL((struct ObjRec *,
				unsigned char *puch_trans_color_r, 
				unsigned char *puch_trans_color_g,
				unsigned char *puch_trans_color_b));
extern int	ObjHasTransPixel ARGS_DECL((struct ObjRec *));
extern int	BuildXPmBuckets ARGS_DECL((int NColors, int *Pixels,
		                          int *DumpIndexToColorIndex,
		                          int CharsPerPixel, char *ColorChr,
		                          XpmTransIndexInfo*));
extern int	XPmLookUp ARGS_DECL((int Pixel, int CharsPerPixel,
		                     char *ColorChr, int *pnColorIndexReturn));
extern void	MakeCachedPixmap ARGS_DECL((struct ObjRec *));
extern int	ExtractPixmap ARGS_DECL((Pixmap, XImage*, Pixmap, XImage*,
		                         int, int, int, int, Pixmap*, XImage**,
		                         Pixmap*, XImage**));
extern struct ObjRec	* CutXPixmap ARGS_DECL((struct ObjRec *ObjPtrToCut,
		                                int *AbsX, int *AbsY,
		                                int *AbsW, int *AbsH));
extern void	CleanUpCachedColorsForPrinting ARGS_DECL((void));
extern void	CacheColorsForPrinting ARGS_DECL((void));
extern int	ConvertPpmTrueToPpm6 ARGS_DECL((char *ppm_data,
		                                int ppm_datasize,
		                                int ppm_data_compress,
		                                char *tmp_ppm6_fname,
		                                int tmp_ppm6_sz));
extern void	DumpXPmObj ARGS_DECL((FILE *, struct ObjRec *));
extern int	NeedsToCacheXPmObj ARGS_DECL((struct ObjRec *));
extern void	RecolorXPmObj ARGS_DECL((struct ObjRec *, int color_index));
extern void	DrawXPmObj ARGS_DECL((Window, int XOff, int YOff,
		                      struct ObjRec *));
extern struct ObjRec	* CreateXPmObj ARGS_DECL((int ImageW, int ImageH,
			                          int W, int H, Pixmap pixmap,
			                          XImage *image, Pixmap bitmap,
			                          XImage *bitmap_image,
			                          int NColors,
			                          int CharsPerPixel,
			                          int FirstPixelIsBg,
			                          char *ColorChar,
			                          char **ColorStr,
			                          int *Pixels, char *Data));
extern void	BreakUpXPixmap ARGS_DECL((struct ObjRec *, int ColsAndRows,
		                          int W, int H));
extern void	SaveXPmObj ARGS_DECL((FILE *, struct ObjRec *));
extern int	PrTgifFindColorIndex ARGS_DECL((struct ObjRec *, char *Str));
extern int	QuickFindColorIndex ARGS_DECL((struct ObjRec *, char *Str,
		                               int *NewAlloc, int UseDefault));
extern int	MyReadPixmapFile ARGS_DECL((char *FileName, int *ImageW,
		                            int *ImageH, int *W, int *H,
		                            Pixmap *pixmap, XImage **image,
		                            Pixmap *bitmap,
		                            XImage **bitmap_image, int *NColors,
		                            int *CharsPerPixel,
		                            int *FirstPixelIsBg,
		                            char **ColorChar, char ***ColorStr,
		                            int **Pixels, char **Data));
extern void	ReadXPmObj ARGS_DECL((FILE *, char *Inbuf, struct ObjRec **));
extern void	ReadJpegObj ARGS_DECL((FILE *, char *Inbuf, struct ObjRec **));
extern int	SetupTrueColorInfo ARGS_DECL((TrueColorInfo*));
extern struct ObjRec	* CreatePpmTrueObjFromFile ARGS_DECL((char *ppm_fname));
extern void	ReadPpmTrueObj ARGS_DECL((FILE *, char *Inbuf,
		                          struct ObjRec **));
extern void	SetXPmPropMask ARGS_DECL((struct ObjRec *, long *plMask,
		                          long *plSkip,
		                          struct PropertiesRec *pProp));
extern void	FreeXPmObj ARGS_DECL((struct ObjRec *));

#ifdef _INCLUDE_FROM_XPIXMAP_C_
#undef extern
#ifndef _NO_RECURSIVE_EXTERN
#define extern extern
#endif /* ~_NO_RECURSIVE_EXTERN */
#endif /*_INCLUDE_FROM_XPIXMAP_C_*/

#endif /*_XPIXMAP_E_*/
