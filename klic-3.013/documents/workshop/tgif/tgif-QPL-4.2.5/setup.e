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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/setup.e,v 1.14 2011/05/16 16:21:59 william Exp $
 */

#ifndef _SETUP_E_
#define _SETUP_E_

extern int	debugNoPointerGrab;
extern int	debugCopyPaste;

extern unsigned int	mainWinW;
extern unsigned int	mainWinH;
extern int	vSBarH;
extern int	hSBarW;
extern int	scrollBarW;
extern int	rulerW;
extern int	brdrW;
extern int	windowPadding;
extern int	pageWindowW;
extern int	pageDummyWindowW;
extern int	colorWindowH;
extern int	colorDummyWindowH;
extern int	msgWindowW;
extern int	msgWindowH;
extern int	choiceImageW;
extern int	choiceImageH;
extern int	choiceWindowW;
extern int	choiceWindowH;
extern int	menuImageW;
extern int	menuImageH;
extern int	titleWindowW;
extern int	titleWindowH;
extern int	iconWindowW;
extern int	iconWindowH;
extern int	menubarWindowW;
extern int	menubarWindowH;
extern int	userRedrawWindowW;
extern int	userRedrawWindowH;
extern int	statusWindowW;
extern int	statusWindowH;
extern int	statusSubWindowW[];
extern int	statusSubWindowH[];
extern int	modeWindowW;
extern int	modeWindowH;
extern int	chatWindowW;
extern int	chatWindowH;

extern int	initialMenubarWindowH;

extern Display		* mainDisplay;
extern Colormap		mainColormap;
extern unsigned int	mainDepth;
extern int		mainScreen;
extern Visual		* mainVisual;

extern Window	rootWindow;
extern Window	mainWindow;
extern Window	drawWindow;
extern Window	choiceWindow;
extern Window	titleWindow;
extern Window	msgWindow;
extern Window	vSBarWindow;
extern Window	hSBarWindow;
extern Window	vRuleWindow;
extern Window	hRuleWindow;
extern Window	iconWindow;
extern Window	iconBaseWindow;
extern Window	menubarWindow;
extern Window	userRedrawWindow;
extern Window	statusWindow;
extern Window	modeWindow;
extern Window	pageWindow;
extern Window	pageDummyWindow;
extern Window	colorWindow;
extern Window	colorDummyWindow;
extern Window	chatWindow;

extern int	paperWidth;
extern int	paperHeight;
extern int	onePageWidth;
extern int	onePageHeight;
extern int	drawOrigX;
extern int	drawOrigY;
extern int	drawWinW;
extern int	drawWinH;

extern int	zoomScale;
extern int	zoomedIn;

extern struct BBRec	drawWinBBox;

extern int	colorDisplay;
extern int	fullTrueColorMode;
extern int	noAntiAlias;
extern int	threeDLook;
extern int	fileModified;
extern int	objId;

extern int	myBgPixel;
extern int	myFgPixel;
extern int	myRubberBandPixel;
extern int	myBorderPixel;
extern int	myWhitePixel;
extern int	myBlackPixel;
extern int	myLtGryPixel;
extern int	myDkGryPixel;
extern int	myVryLtPixel;
extern int	myYellowPixel;
extern int	reverseVideo;

extern char	drawPath[];
extern char	bootDir[];
extern char	homeDir[];
extern char	tgifDir[];
extern char	tmpDir[];

extern int	symPathNumEntries;
extern char	* * symPath;

extern int	initDrawWinW;
extern int	initDrawWinH;

extern short	handleSize;
extern int	resizeTextOnStretch;

extern Window	dummyWindow1, dummyWindow2;

extern Window	statusSubWindow[];

extern int	noMenubar;
extern int	noStatusWindow;
extern int	noChoiceWindow;
extern int	noModeWindow;
extern int	noChatWindow;

extern Atom	executeCmdByIDAtom;

extern int	curChoiceBeforeMakeQuiescent;

extern int	noMinWinSize;

extern int	canvasWindowOnly;

extern int	talkToSelfFiledesInitialized;
extern int	talkToSelfFiledes[2];

extern Atom	compoundTextAtom;
extern Atom	textAtom;
extern Atom	utf8StringAtom;
extern Atom	tgifProtocolAtom;
extern Atom	tmpSelectionAtom;

#ifdef _INCLUDE_FROM_SETUP_C_
#undef extern
#define extern
#endif /*_INCLUDE_FROM_SETUP_C_*/

extern void	RegisterWM_DELETE_WINDOW ARGS_DECL((Window));
extern int	IsWM_DELETE_WINDOW ARGS_DECL((XEvent*));
extern void	UpdDrawWinWH ARGS_DECL((void));
extern void	UpdDrawWinBBox ARGS_DECL((void));
extern void	InitWinSizes ARGS_DECL((void));
extern void	InverseInitWinSizes ARGS_DECL((void));
extern void	ComputeMainWinXY ARGS_DECL((int *MainWinX, int *MainWinY));
extern int	TgHandleCmdEvent ARGS_DECL((XEvent*));
extern int	TgIsCmdEvent ARGS_DECL((XEvent*));
extern void	HandleSelectionRequest ARGS_DECL((XEvent *));
extern int	mainWinEventHandler ARGS_DECL((XEvent *));
extern void	Reconfigure ARGS_DECL((int Forced));
extern void	CleanUpPaperSize ARGS_DECL((void));
extern void	InitPaperSize ARGS_DECL((void));
extern void	InitTmpDir ARGS_DECL((void));
extern void	Setup ARGS_DECL((void));
extern void	CleanUpResiduals ARGS_DECL((void));
extern int	TieLooseEnds ARGS_DECL((void));
extern void	MakeQuiescent ARGS_DECL((void));
extern void	SetFileModified ARGS_DECL((int));

#ifdef _INCLUDE_FROM_SETUP_C_
#undef extern
#ifndef _NO_RECURSIVE_EXTERN
#define extern extern
#endif /* ~_NO_RECURSIVE_EXTERN */
#endif /*_INCLUDE_FROM_SETUP_C_*/

#endif /*_SETUP_E_*/
