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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/raster.e,v 1.12 2011/05/16 16:21:59 william Exp $
 */

#ifndef _RASTER_E_
#define _RASTER_E_

extern GC	rasterGC;
extern GC	patGC;

extern unsigned long	xorOne;
extern unsigned long	xorZero;

extern GC	drawGC;
extern GC	drawSBGC;
extern GC	defaultGC;
extern GC	revDefaultGC;
extern GC	revGrayGC;

extern Pixmap	rotateModePixmap[];

extern Pixmap	choicePixmap[];
extern Pixmap	patPixmap[];
extern Pixmap	* lineWidthPixmap;
extern Pixmap	lineTypePixmap[];
extern Pixmap	lineDashPixmap[];
extern Pixmap	lineStylePixmap[];
extern Pixmap	justPixmap[];
extern Pixmap	alignHoriPixmap[];
extern Pixmap	alignVertPixmap[];
extern Pixmap	alignDirectPixmap[];
extern Pixmap	distrDirectPixmap[];
extern Pixmap	filePixmap;
extern Pixmap	landscapePixmap;
extern Pixmap	specialPixmap;
extern Pixmap	vspacePixmap;
extern Pixmap	rcbRadiusPixmap;
extern Pixmap	moveModePixmap[];
extern Pixmap	editPixmap;
extern Pixmap	intrPixmap[];
extern Pixmap	trekPixmap;
extern Pixmap	pageLayoutPixmap[];
extern Pixmap	scrollPixmap[];
extern Pixmap	scrlBitmap[];
extern Pixmap	shapePixmap[];
extern Pixmap	stretchableModePixmap[];
extern Pixmap	transPatPixmap[];

extern Pixmap	rotatePixmap[];
extern Pixmap	* whereToPrintPixmap;
extern Pixmap	* shortLineWidthPixmap;
extern Pixmap	shortLineStylePixmap[];
extern Pixmap	shortLineTypePixmap[];
extern Pixmap	shortDashPixmap[];

extern Pixmap	wireBitmap;
extern Pixmap	runBitmap;
extern Pixmap	checkBitmap;
extern Pixmap	radioBitmap;
extern Pixmap	submenuBitmap;
extern Pixmap	graySubmenuBitmap;
extern Pixmap	userRedrawBitmap;
extern Pixmap	statusBtnPixmap[];
extern Pixmap	msgBoxPixmap[];

extern int	maxLineWidths;
extern int	maxWhereToPrint;

extern int	dashListLength[];
extern char	*dashList[];

#ifdef _INCLUDE_FROM_RASTER_C_
#undef extern
#define extern
#endif /*_INCLUDE_FROM_RASTER_C_*/

extern void	InitPatFill ARGS_DECL((void));
extern void	Init2PatFill ARGS_DECL((void));
extern void	InitPattern ARGS_DECL((void));
extern void	RedrawChoiceWindow ARGS_DECL((void));
extern void	CleanUpPatFill ARGS_DECL((void));
extern void	CleanUpRasters ARGS_DECL((void));
extern int	UpgradePenFill ARGS_DECL((int));

extern unsigned char	ByteReverse ARGS_DECL((unsigned char));
extern void	GetPatternCellsize ARGS_DECL((int pat_index, int *pn_cell_w,
		                              int *pn_cell_h));
extern unsigned char	*GetPatternBytes ARGS_DECL((int pat_index,
			                            int *pn_num_bytes,
			                            int *pn_need_to_free));

#ifdef _INCLUDE_FROM_RASTER_C_
#undef extern
#ifndef _NO_RECURSIVE_EXTERN
#define extern extern
#endif /* ~_NO_RECURSIVE_EXTERN */
#endif /*_INCLUDE_FROM_RASTER_C_*/

#endif /*_RASTER_E_*/
