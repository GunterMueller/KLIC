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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/color.e,v 1.12 2011/05/16 16:21:57 william Exp $
 */

#ifndef _COLOR_E_
#define _COLOR_E_

extern int	maxColors;
extern int	defaultColorIndex;
extern int	colorIndex;
extern char	myFgColorStr[];
extern char	myBgColorStr[];
extern char	myRubberBandColorStr[];
extern char	* * colorMenuItems;
extern int	* colorPixels;
extern int	* xorColorPixels;
extern int	* colorLayerOn;
extern XColor	* tgifColors;
extern XColor	* tgifRequestedColors;
extern XColor	myBgColor;
extern int	maxRGB;
extern int	colorDump;
extern int	useLocalRGBTxt;
extern int	printUsingRequestedColor;
extern int	colorLayers;
extern int	needToRedrawColorWindow;
extern int	initColorDontReload;
extern int	gnUpdatePixelObjCount;

extern char	defaultBgColorStr[];
extern int	defaultBgColorIndex;

extern char	altEditTextBgColorStr[];
extern int	altEditTextBgIndex;
extern char	altEditTextHighlightColorStr[];
extern int	altEditTextHighlightIndex;
extern int	useAltEditTextBgColor;

extern char	pngExportTransparentColor[];

#ifdef _INCLUDE_FROM_COLOR_C_
#undef extern
#define extern
#endif /*_INCLUDE_FROM_COLOR_C_*/

extern void	DefaultColorArrays ARGS_DECL((int Entries, int **ForePixels,
		                              int **Valid, int **InitRV,
		                              char ***StatusStr));
extern int	GetDrawingBgPixel ARGS_DECL((int index, int pixel));
extern int	GetDrawingFgPixel ARGS_DECL((int index, int pixel));
extern void	GetDrawingFgColorStr ARGS_DECL((int index, int pixel,
		                                char *color_buf, int buf_sz));
extern int	TgifParseColor ARGS_DECL((char*, XColor*));
extern int	ParseAndAllocColorByName ARGS_DECL((char*, XColor*,
		                                    unsigned short *,
		                                    unsigned short *,
		                                    unsigned short *));
extern void	RecalcXorPixels ARGS_DECL((void));
extern void	InitColor ARGS_DECL((void));
extern void	RedrawColorWindow ARGS_DECL((void));
extern int	OneColorObject ARGS_DECL((struct ObjRec *, int *ColorIndex));
extern int	ChangeObjColor ARGS_DECL((struct ObjRec *, int ColorIndex));
extern void	ChangeAllSelColor ARGS_DECL((int ColorIndex, int HighLight));
extern void	RefreshColorMenu ARGS_DECL((TgMenu*));
extern void	SetUpColorMenuPixmap ARGS_DECL((int **ForeColors,
		                                int **InitRV, Pixmap **,
		                                int *Rows, int *Cols));
extern TgMenu	*CreateColorMenu ARGS_DECL((TgMenu*, int X, int Y,
		                            TgMenuInfo*,
		                            int status_str_xlated));
extern int	ColorMenu ARGS_DECL((int X, int Y, int TrackMenubar));
extern void	CleanUpColors ARGS_DECL((void));
extern void	ColorEventHandler ARGS_DECL((XEvent*));
extern void	RedrawColorDummyWindow ARGS_DECL((void));
extern void	ColorDummyEventHandler ARGS_DECL((XEvent*));
extern int	UpdatePixel ARGS_DECL((struct ObjRec *));
extern void 	UpdateXPmObjects ARGS_DECL((struct ObjRec *));
extern int	FlushColormap ARGS_DECL((void));
extern void	AddColor ARGS_DECL((void));
extern void	ChooseColor ARGS_DECL((void));
extern void	SetAltEditTextBgColor ARGS_DECL((void));
extern void	SetAltEditTextHighlightColor ARGS_DECL((void));
extern void	ToggleAltEditTextBgColor ARGS_DECL((void));
extern void	SaveColors ARGS_DECL((FILE*));
extern int	ReadColors ARGS_DECL((FILE*, char*));
extern void	DumpRGBColorLine ARGS_DECL((FILE *FP, int ColorIndex,
		                            int Indent, int EndOfLine));

#ifdef _INCLUDE_FROM_COLOR_C_
#undef extern
#ifndef _NO_RECURSIVE_EXTERN
#define extern extern
#endif /* ~_NO_RECURSIVE_EXTERN */
#endif /*_INCLUDE_FROM_COLOR_C_*/

#endif /*_COLOR_E_*/
