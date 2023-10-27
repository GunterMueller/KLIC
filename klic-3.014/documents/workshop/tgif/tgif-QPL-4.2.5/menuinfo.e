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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/menuinfo.e,v 1.12 2011/05/16 16:21:58 william Exp $
 */

#ifndef _MENUINFO_E_
#define _MENUINFO_E_

extern TgMenuInfo	colorMenuInfo;

extern TgMenuInfo	modeMenuInfo;

extern TgMenuInfo	fillMenuInfo;
extern TgMenuInfo	penMenuInfo;
extern TgMenuInfo	lineWidthMenuInfo;
extern TgMenuInfo	lineStyleMenuInfo;
extern TgMenuInfo	lineTypeMenuInfo;
extern TgMenuInfo	lineDashMenuInfo;

extern TgMenuInfo	horiAlignMenuInfo;
extern TgMenuInfo	vertAlignMenuInfo;
extern TgMenuInfo	moveModeMenuInfo;
extern TgMenuInfo	stretchableTextModeMenuInfo;
extern TgMenuInfo	shapeMenuInfo;
extern TgMenuInfo	pageLayoutMenuInfo;
extern TgMenuInfo	transPatModeMenuInfo;
extern TgMenuInfo	pageStyleMenuInfo;

extern TgMenuInfo	fontMenuInfo;
extern TgMenuInfo	fontStyleMenuInfo;
extern TgMenuInfo	fontSizeMenuInfo;

extern TgMenuInfo	stackedPageMenuInfo;
extern TgMenuInfo	tiledPageMenuInfo;
extern TgMenuInfo	pageMenuInfo;

extern TgMenuInfo	imageProcMenuInfo;

extern TgMenuInfo	exportFormatMenuInfo;
extern TgMenuInfo	fileMenuInfo;
extern TgMenuInfo	polyMenuInfo;
extern TgMenuInfo	editMenuInfo;
extern TgMenuInfo	autoRotatePivotMenuInfo;
extern TgMenuInfo	arrangeMenuInfo;
extern TgMenuInfo	slideShowModeMenuInfo;
extern TgMenuInfo	layoutMenuInfo;
extern TgMenuInfo	propertiesMenuInfo;

extern TgMenuInfo	navigateMenuInfo;
extern TgMenuInfo	specialMenuInfo;
extern TgMenuInfo	helpMenuInfo;

extern TgMenuInfo	editAttrInEditorMenuInfo;
extern TgMenuInfo	editAttrGroupInEditorMenuInfo;
extern TgMenuInfo	getPropertyMenuInfo;
extern TgMenuInfo	peekDimensionMenuInfo;
extern TgMenuInfo	baseContextMenuInfo;
extern TgMenuInfo	editTextContextMenuInfo;

extern TgMenuInfo	gridMenuInfo;

extern TgMenuInfo	mainMenuInfo;

extern TgMenuInfo	minimalMenubarMenuInfo;
extern TgMenuInfo	maximalMenubarMenuInfo;

#ifdef _INCLUDE_FROM_MENUINFO_C_
#undef extern
#define extern
#endif /*_INCLUDE_FROM_MENUINFO_C_*/

/*
 * extern function declarations
 */

#ifdef _INCLUDE_FROM_MENUINFO_C_
#undef extern
#ifndef _NO_RECURSIVE_EXTERN
#define extern extern
#endif /* ~_NO_RECURSIVE_EXTERN */
#endif /*_INCLUDE_FROM_MENUINFO_C_*/

#endif /*_MENUINFO_E_*/
