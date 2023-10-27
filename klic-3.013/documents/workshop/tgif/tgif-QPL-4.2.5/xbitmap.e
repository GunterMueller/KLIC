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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/xbitmap.e,v 1.12 2011/05/16 16:22:00 william Exp $
 */

#ifndef _XBITMAP_E_
#define _XBITMAP_E_

extern GC	xbmGC;
extern int	askForXBmSpec;
extern int	stripEPSComments;
extern int	saveEPSLines;
extern int	leftExportPixelTrim;
extern int	topExportPixelTrim;
extern int	rightExportPixelTrim;
extern int	bottomExportPixelTrim;
extern int	xpmOutputVersion;
extern Pixmap	dummyBitmap;
extern char	gszHhtmlExportTemplate[MAXPATHLENGTH];

#ifdef _INCLUDE_FROM_XBITMAP_C_
#undef extern
#define extern
#endif /*_INCLUDE_FROM_XBITMAP_C_*/

extern void	SetHtmlExportTemplate ARGS_DECL((void));
extern int	ReadHtmlExportTemplate ARGS_DECL((char *buf));
extern void	SetExportPixelTrim ARGS_DECL((int cur_val_is_too_large));
extern int	GetXpmOutputVersion ARGS_DECL((void));
extern void	SetXpmOutputVersion ARGS_DECL((int));
extern void	InitXBm ARGS_DECL((void));
extern void	ExportHalfToneBitmap ARGS_DECL((void));
extern void	ExportThresholdBitmap ARGS_DECL((void));
extern void	SetExportBitmapThreshold ARGS_DECL((char*));
extern int	RefreshBitPixmapMenu ARGS_DECL((TgMenu*));
extern TgMenu	*CreateBitPixmapMenu ARGS_DECL((TgMenu*, int X, int Y,
                                                TgMenuInfo*,
                                                int status_str_xlated));
extern void	CleanUpXBm ARGS_DECL((void));
extern void	CalcTransform ARGS_DECL((struct MtrxRec *));
extern void	MakeCachedBitmap ARGS_DECL((struct ObjRec *));
extern int	ExtractBitmap ARGS_DECL((Pixmap OrigBitmap, XImage *OrigImage,
		                         int X, int Y, int W, int H,
		                         Pixmap *Bitmap, XImage **Image));
extern int	AutoTrimBitmap ARGS_DECL((Pixmap OrigBitmap, int *pnImageW,
		                          int *pnImageH, Pixmap *Bitmap,
		                          XImage **Image, int *pnEmptyImage,
		                          int *pnDontFreeOrigBitmap));
extern void	InvertXBitmaps ARGS_DECL((void));
extern void	ParseCutSpec ARGS_DECL((char *Spec, int ImageW, int ImageH,
		                        float *Mag, int *SrcX, int *SrcY,
		                        int *SrcW, int *SrcH));
extern void	CutXBitmap ARGS_DECL((void));
extern int	InitTrueColorInfo ARGS_DECL((XImage*, TrueColorInfo*,
                                             int image_w));
extern void	GenPreviewBitmap ARGS_DECL((FILE *, int llxPage, int llyPage,
		                            int urxPage, int uryPage));
extern void	GetRealBaseName ARGS_DECL((char *fname));
extern void	SetAColorByteInfo ARGS_DECL((ColorBytes*, XColor*));
extern int	DumpXImageToPpmFile ARGS_DECL((XImage*, int w, int h,
		                               char *tmp_fname, int deflate));
extern void	DumpXBitmapFile ARGS_DECL((int nInImageProc,
		                           int nDumpPpmPreferred,
		                           int nPageNumInFileName));
extern int	DumpXImageToFile ARGS_DECL((XImage*, int W, int H,
		                            char *tmp_fname, char *ext));
extern void	ExportXPixmapDeckToGIF ARGS_DECL((void));
extern void	DumpBitmap ARGS_DECL((FILE*, XImage*, char*, int ImageW,
		                      int ImageH, int Transformed, int OrigX,
		                      int OrigY, int AdjX, struct MtrxRec *,
		                      int Indent));
extern void	DumpXBmObj ARGS_DECL((FILE*, struct ObjRec *));
extern int	NeedsToCacheXBmObj ARGS_DECL((struct ObjRec *));
extern void	DrawXBmObj ARGS_DECL((Window, int XOff, int YOff,
		                      struct ObjRec *));
extern struct ObjRec	* CreateXBmObj ARGS_DECL((int ImageW, int ImageH, int W,
			                          int H, Pixmap, XImage *));
extern void	BreakUpXBitmap ARGS_DECL((struct ObjRec *, int ColsAndRows,
		                          int W, int H));
extern void	DumpXBmData ARGS_DECL((FILE*, Pixmap bitmap, XImage**,
		                       int W, int H));
extern void	SaveXBmObj ARGS_DECL((FILE *, struct ObjRec *));
extern void	ReadXBmObj ARGS_DECL((FILE *, char *Inbuf, struct ObjRec **));
extern void	SetXBmPropMask ARGS_DECL((struct ObjRec *, long *plMask,
		                          long *plSkip,
		                          struct PropertiesRec *pProp));
extern void	FreeXBmObj ARGS_DECL((struct ObjRec *));

#ifdef _INCLUDE_FROM_XBITMAP_C_
#undef extern
#ifndef _NO_RECURSIVE_EXTERN
#define extern extern
#endif /* ~_NO_RECURSIVE_EXTERN */
#endif /*_INCLUDE_FROM_XBITMAP_C_*/

#endif /*_XBITMAP_E_*/
