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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/setup.c,v 1.86 2011/06/02 17:35:50 cvsps Exp $
 */

#define _INCLUDE_FROM_SETUP_C_

#include "tgifdefs.h"
#include "cmdids.h"
#include "expfdefs.h"

#include "attr.e"
#include "auxtext.e"
#include "chat.e"
#include "choice.e"
#include "choose.e"
#include "cmd.e"
#include "color.e"
#include "cursor.e"
#include "cutpaste.e"
#include "dialog.e"
#include "drawing.e"
#include "dup.e"
#include "edit.e"
#include "exec.e"
#include "file.e"
#include "font.e"
#include "grid.e"
#include "help.e"
#include "imgproc.e"
#include "import.e"
#include "inmethod.e"
#include "mainloop.e"
#include "mainmenu.e"
#include "mark.e"
#include "markup.e"
#include "menu.e"
#include "miniline.e"
#include "move.e"
#include "msg.e"
#include "names.e"
#include "navigate.e"
#include "nkf.e"
#include "page.e"
#include "pattern.e"
#include "pin.e"
#include "poly.e"
#include "ps.e"
#include "raster.e"
#include "remote.e"
#include "ruler.e"
#include "scroll.e"
#include "select.e"
#include "setup.e"
#include "shape.e"
#include "shortcut.e"
#include "special.e"
#include "spline.e"
#include "stk.e"
#include "stretch.e"
#include "strtbl.e"
#include "tangram2.e"
#include "tdgtlist.e"
#include "text.e"
#include "tgcwheel.e"
#include "tginssym.e"
#include "tidget.e"
#include "util.e"
#include "version.e"
#include "wb.e"
#include "xbitmap.e"
#include "xpixmap.e"
#include "xprtfltr.e"
#include "z_intrf.e"

#include "tgificon.xbm"

#define DEF_VSBAR_H 64
#define DEF_HSBAR_W 128

#define DRAW_WINDOW_W (5*ONE_INCH)
#define DRAW_WINDOW_H (4*ONE_INCH)
#define CHOICE_WINDOW_W (12*CHOICE_IMAGE_W)
#define CHOICE_WINDOW_H (CHOICE_IMAGE_H<<1)
#define VSBAR_H (DRAW_WINDOW_H+RULER_W+(BRDR_W<<1))
#define HSBAR_W 128
#define PAGE_DUMMY_WINDOW_W 3
#define PAGE_WINDOW_W (DRAW_WINDOW_W+RULER_W-HSBAR_W-PAGE_DUMMY_WINDOW_W)
#define COLOR_DUMMY_WINDOW_H 3
#define COLOR_WINDOW_H (DRAW_WINDOW_H+RULER_W-VSBAR_H-COLOR_DUMMY_WINDOW_H)
#define TITLE_WINDOW_W (DRAW_WINDOW_W+RULER_W+BRDR_W+SCROLLBAR_W+(BRDR_W<<1))
#define TITLE_WINDOW_H (MENU_IMAGE_H<<1)
#define MSG_WINDOW_W (TITLE_WINDOW_W-CHOICE_WINDOW_W-(BRDR_W<<1))
#define MSG_WINDOW_H (CHOICE_WINDOW_H)
#define ICON_WINDOW_W 64
#define ICON_WINDOW_H 64
#define MENUBAR_WINDOW_W (TITLE_WINDOW_W)
#define MENUBAR_WINDOW_H 20
#define USER_REDRAW_WINDOW_W 44
#define USER_REDRAW_WINDOW_H 20
#define STATUS_WINDOW_W (TITLE_WINDOW_W-USER_REDRAW_WINDOW_W-(BRDR_W<<1))
#define STATUS_WINDOW_H 20
#define MODE_WINDOW_W (CHOICE_IMAGE_W)
#define MODE_WINDOW_H (DRAW_WINDOW_H+RULER_W+(BRDR_W<<1))
#define CHAT_WINDOW_W (TITLE_WINDOW_W)
#define CHAT_WINDOW_H 40

int	debugNoPointerGrab=FALSE;
int	debugCopyPaste=FALSE;

unsigned int	mainWinW = 0;
unsigned int	mainWinH = 0;
int	vSBarH = VSBAR_H;
int	hSBarW = HSBAR_W;
int	scrollBarW = SCROLLBAR_W;
int	rulerW = RULER_W;
int	brdrW = BRDR_W;
int	windowPadding = 0;
int	pageWindowW = PAGE_WINDOW_W;
int	pageDummyWindowW = PAGE_DUMMY_WINDOW_W;
int	colorWindowH = COLOR_WINDOW_H;
int	colorDummyWindowH = COLOR_DUMMY_WINDOW_H;
int	msgWindowW = MSG_WINDOW_W;
int	msgWindowH = MSG_WINDOW_H;
int	choiceImageW = CHOICE_IMAGE_W;
int	choiceImageH = CHOICE_IMAGE_H;
int	choiceWindowW = CHOICE_WINDOW_W;
int	choiceWindowH = CHOICE_WINDOW_H;
int	menuImageW = MENU_IMAGE_W;
int	menuImageH = MENU_IMAGE_H;
int	titleWindowW = TITLE_WINDOW_W;
int	titleWindowH = TITLE_WINDOW_H;
int	iconWindowW = ICON_WINDOW_W;
int	iconWindowH = ICON_WINDOW_H;
int	menubarWindowW = MENUBAR_WINDOW_W;
int	menubarWindowH = MENUBAR_WINDOW_H;
int	userRedrawWindowW = USER_REDRAW_WINDOW_W;
int	userRedrawWindowH = USER_REDRAW_WINDOW_H;
int	statusWindowW = STATUS_WINDOW_W;
int	statusWindowH = STATUS_WINDOW_H;
int	statusSubWindowW[MAX_STATUS_BTNS];
int	statusSubWindowH[MAX_STATUS_BTNS];
int	modeWindowW = MODE_WINDOW_W;
int	modeWindowH = MODE_WINDOW_H;
int	chatWindowW = CHAT_WINDOW_W;
int	chatWindowH = CHAT_WINDOW_H;

int	initialMenubarWindowH=MENUBAR_WINDOW_H;

static int	statusSubWindowX[MAX_STATUS_BTNS];
static int	statusSubWindowY[MAX_STATUS_BTNS];

Display		* mainDisplay=NULL;
Colormap	mainColormap=(Colormap)0;
unsigned int	mainDepth;
int		mainScreen;
Visual		* mainVisual=NULL;

Window	rootWindow=None;
Window	mainWindow=None;
Window	drawWindow=None;
Window	choiceWindow=None;
Window	titleWindow=None;
Window	msgWindow=None;
Window	vSBarWindow=None;
Window	hSBarWindow=None;
Window	vRuleWindow=None;
Window	hRuleWindow=None;
Window	iconWindow=None;
Window	iconBaseWindow=None;
Window	menubarWindow=None;
Window	userRedrawWindow=None;
Window	statusWindow=None;
Window	modeWindow=None;
Window	pageWindow=None;
Window	pageDummyWindow=None;
Window	colorWindow=None;
Window	colorDummyWindow=None;
Window	chatWindow=None;

#ifndef A4PAPER
int	onePageWidth = (85*ONE_INCH)/10;
int	onePageHeight = 11*ONE_INCH;
int	paperWidth = (85*ONE_INCH)/10;
int	paperHeight = 11*ONE_INCH;
#else /* A4PAPER */
int	onePageWidth = (825*ONE_INCH)/100;
int	onePageHeight = (117*ONE_INCH)/10;
int	paperWidth = (825*ONE_INCH)/100;
int	paperHeight = (117*ONE_INCH)/10;
#endif /* A4PAPER */
int	drawOrigX = 0;
int	drawOrigY = 0;
int	drawWinW = DRAW_WINDOW_W;
int	drawWinH = DRAW_WINDOW_H;

int	zoomScale = 0;
int	zoomedIn = FALSE;

struct BBRec	drawWinBBox;

int	colorDisplay = FALSE;
int	fullTrueColorMode = FALSE;
int	noAntiAlias = TRUE;
int	threeDLook = FALSE;
int	fileModified = FALSE;
int	objId = 0;

int	myBgPixel=INVALID;
int	myFgPixel=INVALID;
int	myRubberBandPixel=INVALID;
int	myBorderPixel=INVALID;
int	myWhitePixel=INVALID;
int	myBlackPixel=INVALID;
int	myLtGryPixel=INVALID;
int	myDkGryPixel=INVALID;
int	myVryLtPixel=INVALID;
int	myYellowPixel=INVALID;
int	reverseVideo = FALSE;

char	drawPath[MAXPATHLENGTH]; /* last char is NOT '/' */
char	bootDir[MAXPATHLENGTH+2];
char	homeDir[MAXPATHLENGTH];
char	tgifDir[MAXPATHLENGTH];
char	tmpDir[MAXPATHLENGTH];

int	symPathNumEntries = INVALID;
char	* * symPath=NULL;

int	initDrawWinW=INVALID, initDrawWinH=INVALID;

short	handleSize=3;
int	resizeTextOnStretch=FALSE;

Window	dummyWindow1=None, dummyWindow2=None;

Window	statusSubWindow[MAX_STATUS_BTNS];

int	noMenubar=FALSE;
int	noStatusWindow=FALSE;
int	noChoiceWindow=FALSE;
int	noModeWindow=FALSE;
int	noChatWindow=TRUE;

Atom	executeCmdByIDAtom=(Atom)0;

int	curChoiceBeforeMakeQuiescent=NOTHING;

int	noMinWinSize=TRUE;

int	canvasWindowOnly=FALSE;

int	talkToSelfFiledesInitialized=FALSE;
int	talkToSelfFiledes[2];

Atom	compoundTextAtom=(Atom)0;
Atom	textAtom=(Atom)0;
Atom	utf8StringAtom=(Atom)0;
Atom	tgifProtocolAtom=(Atom)0;
Atom	tmpSelectionAtom=(Atom)0;

static Atom	wmDeleteWindowAtom=(Atom)0;
static Atom	targetsAtom=(Atom)0;

static int	baseScrollBarW=SCROLLBAR_W;

void RegisterWM_DELETE_WINDOW(win)
   Window win;
{
#ifndef NOTR4MODE
   XSetWMProtocols(mainDisplay, win, &wmDeleteWindowAtom, 1);
#endif
}

int IsWM_DELETE_WINDOW(pXEvent)
   XEvent *pXEvent;
{
   if (pXEvent->type == ClientMessage) {
      char *psz=XGetAtomName(mainDisplay, pXEvent->xclient.message_type);

      if (psz == NULL) {
         /* this should not happen, but just in case */
      } else if (strcmp("WM_PROTOCOLS", psz) == 0 &&
            pXEvent->xclient.data.l[0] == wmDeleteWindowAtom) {
         XFree((void*)psz);
         return TRUE;
      }
      if (psz != NULL) XFree((void*)psz);
   }
   return FALSE;
}

void UpdDrawWinWH()
{
   drawWinW = ABS_SIZE(initDrawWinW);
   drawWinH = ABS_SIZE(initDrawWinH);
}

void UpdDrawWinBBox()
{
   drawWinBBox.ltx = drawOrigX;
   drawWinBBox.lty = drawOrigY;
   drawWinBBox.rbx = drawOrigX + drawWinW-1;
   drawWinBBox.rby = drawOrigY + drawWinH-1;
}

#include "xbm/btn1.xbm"

static
void CalcStatusSubWinGeom()
{
   register int i;
   int left=0, w=(int)(statusWindowW/3), win_y, right=0, win_h;

   win_y = 1;
   if (threeDLook) {
      win_y += (windowPadding>>1);
      win_h = statusWindowH-4;
   } else {
      win_h = statusWindowH-2-(brdrW<<1);
   }
   for (i=0; i < MAX_STATUS_BTNS; i++) {
      int win_x, win_w;

      right += w;
      if (right >= statusWindowW) right = statusWindowW-1;
      win_x = left+(brdrW<<2)+btn1_width+2+(windowPadding>>1);
      win_w = right-left-(brdrW*6)-btn1_width-2-(windowPadding>>1);
      statusSubWindowX[i] = win_x;
      statusSubWindowY[i] = win_y;
      statusSubWindowW[i] = win_w;
      statusSubWindowH[i] = win_h;
      left += w;
   }
}

void InitWinSizes()
{
   int choice_cols=(CHOICE_WINDOW_W/choiceImageW);
   int choice_rows=(CHOICE_WINDOW_H/choiceImageH);
   int min_draw_win_w=0;

   initDrawWinW = drawWinW;
   initDrawWinH = drawWinH;
   choiceWindowW = CHOICE_WINDOW_W+windowPadding*(choice_cols+2);
   choiceWindowH = CHOICE_WINDOW_H+windowPadding*(choice_rows+2);
   msgWindowW = ((msgFontSet==NULL && msgFontPtr==NULL) ? defaultFontWidth :
         msgFontWidth) + (windowPadding<<1);
   msgWindowH = choiceWindowH;
   titleWindowH = (((msgFontSet==NULL && msgFontPtr==NULL) ? defaultFontHeight :
         msgFontHeight) << (showVersion ? 1 : 0)) + windowPadding + 4;
   userRedrawWindowH = statusWindowH;
   rulerW = RULER_W + windowPadding;
   scrollBarW = baseScrollBarW+windowPadding;

   modeWindowW = CHOICE_IMAGE_W+windowPadding+(windowPadding<<1);

   if (drawWinH < ONE_INCH) drawWinH = ONE_INCH;
   min_draw_win_w = choiceWindowW + msgWindowW -
         (noModeWindow ? 0 : modeWindowW+(brdrW<<1)) - rulerW - scrollBarW;
   if (drawWinW < min_draw_win_w) drawWinW = min_draw_win_w;

   modeWindowH = drawWinH+rulerW+(brdrW<<2)+windowPadding;
   if (threeDLook) {
      colorDummyWindowH = COLOR_DUMMY_WINDOW_H + windowPadding;
      pageDummyWindowW = PAGE_DUMMY_WINDOW_W + windowPadding;
   }
   pageWindowW = drawWinW+rulerW-hSBarW-pageDummyWindowW;
   if (pageWindowW <= 0) pageWindowW = 1;
   colorWindowH = drawWinH+rulerW-DEF_VSBAR_H-colorDummyWindowH;
   vSBarH = drawWinH+rulerW+(brdrW<<1);
   titleWindowW = (noModeWindow ? 0 : modeWindowW+(brdrW<<1)) +
         drawWinW+rulerW+brdrW+scrollBarW+(brdrW<<1);
   menubarWindowW = titleWindowW;
   statusWindowW = titleWindowW-userRedrawWindowW-(brdrW<<1);
   chatWindowW = titleWindowW;
   CalcMenubarWindowHeight();
   msgWindowW = titleWindowW-choiceWindowW-(brdrW<<1);
   mainWinW = titleWindowW+(brdrW<<1);
   mainWinH = (noMenubar ? 0 : menubarWindowH+(brdrW<<1)) +
         (noStatusWindow ? 0 : statusWindowH+(brdrW<<1)) +
         (noChoiceWindow ? 0 : msgWindowH+(brdrW<<1)) +
         (noChatWindow ? 0 : chatWindowH+(brdrW<<1)) +
         titleWindowH+vSBarH+scrollBarW+6*brdrW;
   if (colorLayers) vSBarH = DEF_VSBAR_H;
   CalcStatusSubWinGeom();
   SetChatWindowGeom();
}

void InverseInitWinSizes()
   /* derive other win sizes from mainWinW and mainWinH */
{
   titleWindowW = mainWinW-(brdrW<<1);
   menubarWindowW = titleWindowW;
   chatWindowW = titleWindowW;
   statusWindowW = titleWindowW-userRedrawWindowW-(brdrW<<1);
   CalcMenubarWindowHeight();
   msgWindowW = titleWindowW-choiceWindowW-(brdrW<<1);
   vSBarH = mainWinH - titleWindowH - scrollBarW - 6*brdrW -
         (noMenubar ? 0 : menubarWindowH+(brdrW<<1)) -
         (noStatusWindow ? 0 : statusWindowH+(brdrW<<1)) -
         (noChoiceWindow ? 0 : msgWindowH+(brdrW<<1)) -
         (noChatWindow ? 0 : chatWindowH+(brdrW<<1));
   drawWinH = initDrawWinH = vSBarH-rulerW-(brdrW<<1);
   drawWinW = initDrawWinW = titleWindowW-scrollBarW-rulerW-(brdrW<<2) -
         (noModeWindow ? 0 : modeWindowW+(brdrW<<1));
   modeWindowH = drawWinH+rulerW+(brdrW<<2);
   pageWindowW = titleWindowW-scrollBarW-hSBarW-pageDummyWindowW-(brdrW<<2);
   if (pageWindowW <= 0) pageWindowW = 1;
   colorWindowH = drawWinH+rulerW-DEF_VSBAR_H-colorDummyWindowH;
   if (colorLayers) vSBarH = DEF_VSBAR_H;
   CalcMenubarWindowHeight();
   CalcStatusSubWinGeom();
   SetChatWindowGeom();
}

void ComputeMainWinXY(MainWinX, MainWinY)
   int *MainWinX, *MainWinY;
{
   int win_x, win_y, done = FALSE;
   unsigned int win_w, win_h, win_brdr_w, win_d, num_child;
   Window win=mainWindow, root_win, parent_win, *child_wins;

   *MainWinX = *MainWinY = 0;
   while (!done) {
      XGetGeometry(mainDisplay, win, &root_win, &win_x, &win_y, &win_w,
            &win_h, &win_brdr_w, &win_d);
      *MainWinX += win_x;
      *MainWinY += win_y;
      if (XQueryTree(mainDisplay, win, &root_win, &parent_win, &child_wins,
            &num_child) == 0) {
         return;
      }
      if (child_wins != NULL) XFree((void *)child_wins);
      if (parent_win == rootWindow) {
         done = TRUE;
      } else {
         win = parent_win;
      }
   }
}

static
void DoReconfigure()
{
   int cur_y=0, x_offset=(noModeWindow ? 0 : modeWindowW+(brdrW<<1));
   int draw_win_x_offset=0, draw_win_y_offset=0;

   XResizeWindow(mainDisplay, titleWindow, titleWindowW, titleWindowH);
   cur_y += titleWindowH + (brdrW<<1);
   if (!noMenubar) {
      XResizeWindow(mainDisplay, menubarWindow, menubarWindowW,
            menubarWindowH);
      cur_y += menubarWindowH + (brdrW<<1);
   }
   if (!noChoiceWindow) {
      XMoveWindow(mainDisplay, choiceWindow, 0, cur_y);
      if (msgWindowW > 0) {
         XMoveResizeWindow(mainDisplay, msgWindow, choiceWindowW+(brdrW<<1),
               cur_y, msgWindowW, msgWindowH);
      } else {
         XMoveWindow(mainDisplay, msgWindow, choiceWindowW+(brdrW<<1), cur_y);
      }
      cur_y += msgWindowH + (brdrW<<1);
   }
   if (!noModeWindow) {
      XMoveResizeWindow(mainDisplay, modeWindow, 0, cur_y,
            modeWindowW, modeWindowH);
   }
   XMoveResizeWindow(mainDisplay, dummyWindow1, x_offset, cur_y,
         rulerW, rulerW);
   XMoveResizeWindow(mainDisplay, hRuleWindow, x_offset+rulerW+(brdrW<<1),
         cur_y, drawWinW, rulerW-windowPadding);
   if (colorLayers) {
      XMoveResizeWindow(mainDisplay, colorWindow,
            x_offset+drawWinW+rulerW+(brdrW<<2), cur_y, scrollBarW,
            colorWindowH);
      XMoveWindow(mainDisplay, colorDummyWindow,
            x_offset+drawWinW+rulerW+(brdrW<<2),
            cur_y+colorWindowH+(brdrW<<1));
      XMoveResizeWindow(mainDisplay, vSBarWindow,
            x_offset+drawWinW+rulerW+(brdrW<<2),
            cur_y+colorWindowH+colorDummyWindowH+(brdrW<<2), scrollBarW,
            DEF_VSBAR_H);
   } else {
      XMoveResizeWindow(mainDisplay, vSBarWindow,
            x_offset+drawWinW+rulerW+(brdrW<<2), cur_y, scrollBarW, vSBarH);
   }
   cur_y += rulerW + (brdrW<<1);
   XMoveResizeWindow(mainDisplay, vRuleWindow, x_offset, cur_y,
         rulerW-windowPadding, drawWinH);
   XMoveResizeWindow(mainDisplay, drawWindow, x_offset+rulerW+(brdrW<<1),
         cur_y, drawWinW, drawWinH);
   if (inSlideShow) {
      draw_win_x_offset = x_offset+rulerW+(brdrW<<1);
      draw_win_y_offset = cur_y;
   }
   cur_y += drawWinH + (brdrW<<1);
   if (pageWindowW > 0) {
      XMoveResizeWindow(mainDisplay, pageWindow, 0, cur_y, pageWindowW,
            scrollBarW+(brdrW<<1));
      MakeCurPageTabVisible();
   } else {
      XMoveWindow(mainDisplay, pageWindow, 0, cur_y);
   }
   XMoveWindow(mainDisplay, pageDummyWindow, pageWindowW, cur_y);
   XMoveWindow(mainDisplay, hSBarWindow,
         pageWindowW+pageDummyWindowW+(brdrW<<1), cur_y);
   XMoveWindow(mainDisplay, dummyWindow2,
         x_offset+drawWinW+rulerW+(brdrW<<2), cur_y);
   cur_y += scrollBarW + (brdrW<<1);
   if (!noStatusWindow) {
      int i;

      XMoveWindow(mainDisplay, userRedrawWindow, 0, cur_y);
      XMoveResizeWindow(mainDisplay, statusWindow,
            userRedrawWindowW+(brdrW<<1), cur_y,
            statusWindowW, statusWindowH);
      for (i=0; i < MAX_STATUS_BTNS; i++) {
         XMoveResizeWindow(mainDisplay, statusSubWindow[i],
               statusSubWindowX[i], statusSubWindowY[i],
               statusSubWindowW[i], statusSubWindowH[i]);
      }
      cur_y += statusWindowH + (brdrW<<1);
   }
   if (!noChatWindow) {
      MoveResizeChatWindow(0, cur_y, chatWindowW, chatWindowH);
      cur_y += chatWindowH + (brdrW<<1);
   }
   if (inSlideShow) {
      int dpy_w=DisplayWidth(mainDisplay,mainScreen);
      int dpy_h=DisplayHeight(mainDisplay,mainScreen);
      int x_off=((dpy_w-ZOOMED_SIZE(onePageWidth))>>1);
      int y_off=((dpy_h-ZOOMED_SIZE(onePageHeight))>>1);
      int new_x=slideShowXOffset+x_off;
      int new_y=slideShowYOffset+y_off;
      int changed=FALSE;

      XMoveResizeWindow(mainDisplay, drawWindow,
            new_x+draw_win_x_offset, new_y+draw_win_y_offset,
            ZOOMED_SIZE(onePageWidth)-(slideShowXOffset<<1),
            ZOOMED_SIZE(onePageHeight-(slideShowYOffset<<1)));
      if (ZOOMED_SIZE(onePageWidth) > drawWinW) {
         drawWinW = ZOOMED_SIZE(onePageWidth);
         changed = TRUE;
      }
      if (ZOOMED_SIZE(onePageHeight) > drawWinH) {
         drawWinH = ZOOMED_SIZE(onePageHeight);
         changed = TRUE;
      }
      if (changed) {
         InitWinSizes();
      }
   }
}

void Reconfigure(Forced)
   int Forced;
{
   int i;
   Window root_win;
   int win_x, win_y, main_win_x, main_win_y;
   unsigned int win_w, win_h, win_brdr_w, win_d;
   XEvent ev;

   ComputeMainWinXY(&main_win_x, &main_win_y);
   XGetGeometry(mainDisplay, mainWindow, &root_win, &win_x, &win_y, &win_w,
         &win_h, &win_brdr_w, &win_d);
   if (!iconWindowShown && !Forced && win_w == mainWinW && win_h == mainWinH) {
      SaveStackingOrder();
      if (pinnedMainMenu) {
         MoveMainMenuWindow(main_win_x, main_win_y);
      } else {
         SaveMainWinPosition(main_win_x, main_win_y);
      }
      for (i = 0; i < numStacking; i++) {
         XMapRaised(mainDisplay, stackingWins[i]);
      }
      while (XCheckWindowEvent(mainDisplay, mainWindow,
            VisibilityChangeMask | StructureNotifyMask, &ev)) ;

      XFlush(mainDisplay);
      return;
   }
   if (!Forced && !iconWindowShown) {
      SaveStackingOrder();
      for (i = 0; i < numStacking; i++) {
         XMapRaised(mainDisplay, stackingWins[i]);
      }
   }

   mainWinW = win_w;
   mainWinH = win_h;

   menubarWindowW = mainWinW - (brdrW<<1);
   CalcMenubarWindowHeight();
   if (canvasWindowOnly) {
      initDrawWinW = mainWinW-(brdrW<<1);
      initDrawWinH = mainWinH-(brdrW<<1);
   } else {
      initDrawWinW = mainWinW - rulerW - scrollBarW - 6*brdrW -
         (noModeWindow ? 0 : modeWindowW+(brdrW<<1));
      initDrawWinH = mainWinH - titleWindowH -
            (noMenubar ? 0 : menubarWindowH+(brdrW<<1)) -
            (noStatusWindow ? 0 : statusWindowH+(brdrW<<1)) -
            (noChoiceWindow ? 0 : msgWindowH+(brdrW<<1)) -
            (noChatWindow ? 0 : chatWindowH+(brdrW<<1)) -
            rulerW - scrollBarW - 8*brdrW;
   }
   drawWinW = initDrawWinW;
   drawWinH = initDrawWinH;
   modeWindowH = drawWinH+rulerW+(brdrW<<2);
   colorWindowH = initDrawWinH+rulerW-DEF_VSBAR_H-colorDummyWindowH-(brdrW<<1);
   vSBarH = (colorLayers ? DEF_VSBAR_H : initDrawWinH + rulerW + (brdrW<<1));
   if (noModeWindow) {
      pageWindowW = initDrawWinW+rulerW-hSBarW-pageDummyWindowW;
      titleWindowW = initDrawWinW + rulerW + scrollBarW + (brdrW<<2);
   } else {
      pageWindowW = initDrawWinW+rulerW+modeWindowW+(brdrW<<1)-hSBarW-
            pageDummyWindowW;
      titleWindowW = initDrawWinW + rulerW + modeWindowW + scrollBarW +
            (brdrW*6);
   }
   if (pageWindowW <= 0) pageWindowW = 1;
   menubarWindowW = titleWindowW;
   chatWindowW = titleWindowW;
   if (!noChatWindow) SetChatWindowGeom();
   statusWindowW = titleWindowW-userRedrawWindowW-(brdrW<<1);
   CalcMenubarWindowHeight();
   msgWindowW = titleWindowW - choiceWindowW - (brdrW<<1);
   CalcStatusSubWinGeom();
   if (canvasWindowOnly) {
      XResizeWindow(mainDisplay, drawWindow, drawWinW, drawWinH);
   } else {
      DoReconfigure();
   }
   UpdDrawWinWH();
   UpdDrawWinBBox();
   UpdScrollWinWH();
   SetDefaultDrawWinClipRecs();
   SaveMainWinPosition(main_win_x, main_win_y);
}

int TgHandleCmdEvent(pXEv)
   XEvent *pXEv;
   /* must call TgIsCmdEvent() right before calling this function */
{
   return ExecuteCmdById((int)(pXEv->xclient.data.s[1]),
         (int)(pXEv->xclient.data.s[2]));
}

int TgIsCmdEvent(pXEv)
   XEvent *pXEv;
{
   if (pXEv->xany.window == mainWindow && pXEv->type == ClientMessage) {
      if (executeCmdByIDAtom == pXEv->xclient.message_type &&
            pXEv->xclient.data.s[0] == TG_COMMAND) {
         return TRUE;
      }
   }
   return FALSE;
}

static
void HandleSelectionClear(input)
   XEvent *input;
{
   Window win=(Window)0;
   Atom selection=(Atom)0;
   Time time;

   win = input->xselectionclear.window;
   selection = input->xselectionclear.selection;
   time = input->xselectionclear.time;

   if (debugCopyPaste) {
      static int id=0;
      char *atom_name=NULL;

      printf("\n%1d: SelectionClear.\n", ++id);

      atom_name = XGetAtomName(mainDisplay, selection);
      printf("%1d: Selection name is '%s'.\n", id, atom_name);
      XFree(atom_name);
   }
   if (startSelectionOwnershipTimeValid) {
      endSelectionOwnershipTime = time;
      endSelectionOwnershipTimeValid = TRUE;
   }
   ClearSelection();
}

static
void DebugSelectRequest(property, target, level, msg)
   Atom property, target;
   int level;
   char *msg;
{
   if (debugCopyPaste > level) {
      /*
       * After tgif does a Copy, tgif will get SelectionRequest at
       *         a regular interval
       */
      static int id=0;
      char *atom_name=NULL;

      if (msg == NULL) {
         printf("\n%1d: SelectionRequest.\n", ++id);
      } else {
         printf("\n%1d: SelectionRequest (%s).\n", ++id, msg);
      }
      atom_name = XGetAtomName(mainDisplay, property);
      printf("%1d: Property name is '%s'.\n", id, atom_name);
      XFree(atom_name);

      atom_name = XGetAtomName(mainDisplay, target);
      printf("%1d: Target name is '%s'.\n", id, atom_name);
      XFree(atom_name);
   }
}

void HandleSelectionRequest(input)
   XEvent *input;
{
   Window requestor=(Window)0;
   Atom property=(Atom)0, target=(Atom)0, selection=(Atom)0;
   Time time=(Time)0;
   XEvent sent_ev;
   unsigned char empty_buf[1];

   *empty_buf = '\0';
   requestor = input->xselectionrequest.requestor;
   selection = input->xselectionrequest.selection;
   target = input->xselectionrequest.target;
   property = input->xselectionrequest.property;
   time = input->xselectionrequest.time;

   /* do not translate -- program constants */
   DebugSelectRequest(property, target, 1, "function entry point");
   memset(&sent_ev, 0, sizeof(XEvent));

   sent_ev.xselection.type = SelectionNotify;
   sent_ev.xselection.display = mainDisplay;
   sent_ev.xselection.requestor = requestor;
   sent_ev.xselection.selection = selection;
   sent_ev.xselection.time = input->xselectionrequest.time;
   sent_ev.xselection.target = target;
   sent_ev.xselection.property = None;

   if (target == targetsAtom) {
      /*
       * The selection manager is asking tgif what can you convert the
       *         object copied into (i.e., what are the possobiel targets)?
       */
      Atom targets[5];
      int num_targets=0;

      DebugSelectRequest(property, target, 0, "TARGETS");
      if (gSetCutBufferInfo.tgif_valid) {
         targets[num_targets++] = tgifProtocolAtom;
      }
      if (gSetCutBufferInfo.utf8_valid) {
         targets[num_targets++] = utf8StringAtom;
      }
      if (gSetCutBufferInfo.text_valid) {
         targets[num_targets++] = compoundTextAtom;
         targets[num_targets++] = textAtom;
         targets[num_targets++] = XA_STRING;
      }
      XChangeProperty(mainDisplay, requestor, property, target, 32,
            PropModeReplace, (unsigned char *)targets, num_targets);
      sent_ev.xselection.property = property;
   } else if (target == XA_STRING || target == textAtom) {
      DebugSelectRequest(property, target, 0, "XA_STRING or TEXT");
      if (gSetCutBufferInfo.text_valid) {
         XChangeProperty(mainDisplay, requestor, property, target, 8,
               PropModeReplace,
               (unsigned char *)(gSetCutBufferInfo.text_dyn_str.s),
               gSetCutBufferInfo.text_dyn_str.sz);
      } else {
         XChangeProperty(mainDisplay, requestor, property, target, 8,
               PropModeReplace, empty_buf, 0);
      }
      sent_ev.xselection.property = property;
   } else if (target == utf8StringAtom) {
      DebugSelectRequest(property, target, 0, "UTF8_STRING");
      if (gSetCutBufferInfo.utf8_valid) {
         XChangeProperty(mainDisplay, requestor, property, target, 8,
               PropModeReplace,
               (unsigned char *)(gSetCutBufferInfo.utf8_dyn_str.s),
               gSetCutBufferInfo.utf8_dyn_str.sz);
      } else {
         XChangeProperty(mainDisplay, requestor, property, target, 8,
               PropModeReplace, empty_buf, 0);
      }
      sent_ev.xselection.property = property;
   } else if (target == tgifProtocolAtom) {
      DebugSelectRequest(property, target, 0, "_TGIF_PROTOCOL_ATOM");
      if (gSetCutBufferInfo.tgif_valid) {
         XChangeProperty(mainDisplay, requestor, property, target, 8,
               PropModeReplace,
               (unsigned char *)(gSetCutBufferInfo.tgif_dyn_str.s),
               gSetCutBufferInfo.tgif_dyn_str.sz);
      } else {
         XChangeProperty(mainDisplay, requestor, property, target, 8,
               PropModeReplace, empty_buf, 0);
      }
      sent_ev.xselection.property = property;
   } else if (target == compoundTextAtom) {
      XTextProperty text_property;

      if (gSetCutBufferInfo.utf8_valid) {
         DebugSelectRequest(property, target, 0, "COMPOUND_TEXT for UTF8");
#ifndef _NO_XUTF8FUNCTIONS
         Xutf8TextListToTextProperty(mainDisplay,
               &(gSetCutBufferInfo.utf8_dyn_str.s), 1,
               XCompoundTextStyle, &text_property);
#endif /* ~_NO_XUTF8FUNCTIONS */
      } else if (gSetCutBufferInfo.text_valid) {
         DebugSelectRequest(property, target, 0, "COMPOUND_TEXT not UTF8");
         XmbTextListToTextProperty(mainDisplay,
               &(gSetCutBufferInfo.text_dyn_str.s), 1,
               XCompoundTextStyle, &text_property);
      } else {
         char *psz_buf=(char*)empty_buf;

         DebugSelectRequest(property, target, 0, "COMPOUND_TEXT (none)");
         XmbTextListToTextProperty(mainDisplay, &psz_buf, 1,
               XCompoundTextStyle, &text_property);
      }
      XChangeProperty(mainDisplay, requestor, property, target,
            text_property.format, PropModeReplace, text_property.value,
            text_property.nitems);
      sent_ev.xselection.property = property;
   }
   XSendEvent(mainDisplay, requestor, False, NoEventMask, &sent_ev);
}

static XComposeStatus c_stat;

int mainWinEventHandler(input)
   XEvent *input;
{
   register int i;
   int configure=FALSE;

   if (inSlideShow && input->type == KeyPress) {
      char s[80];
      KeySym key_sym;
      int has_ch=FALSE;
      XKeyEvent *key_ev=(&(input->xkey));

      has_ch = XLookupString(key_ev, s, sizeof(s), &key_sym, &c_stat);
      TranslateKeys(s, &key_sym);

      if (CharIsESC(key_ev, s, key_sym, &has_ch)) {
         LeaveSlideShow();
         return INVALID;
      }
      switch (key_sym) {
      case XK_Left: PrevSlide(); return INVALID;
      case XK_KP_Left: PrevSlide(); return INVALID;
      case XK_Right: NextSlide(); return INVALID;
      case XK_KP_Right: NextSlide(); return INVALID;
      default: break;
      }
   } else if (input->type == SelectionRequest) {
      HandleSelectionRequest(input);
      return INVALID;
   } else if (input->type == SelectionClear) {
      HandleSelectionClear(input);
      return INVALID;
   } else if (input->type == UnmapNotify) {
      Iconify();
   } else if (input->type == MapNotify) {
      UnIconify();
   } else if (input->type == ConfigureNotify) {
#ifdef _DO_BENCHMARK_AT_START
      static int first_time=TRUE;

      if (first_time) {
         SendCommandToSelf(CMDID_BENCHMARK, INVALID);
         first_time = FALSE;
      }
#endif /* _DO_BENCHMARK_AT_START */
      configure = TRUE;
   } else if (input->type == VisibilityNotify &&
         input->xvisibility.state == VisibilityUnobscured) {
      if (iconWindowShown) {
         UnIconify();
      } else {
         SaveStackingOrder();
         for (i = 0; i < numStacking; i++) {
            XMapRaised(mainDisplay, stackingWins[i]);
         }
      }
   } else if (input->type == ClientMessage) {
      if (executeCmdByIDAtom == input->xclient.message_type &&
            input->xclient.data.s[0] == TG_COMMAND) {
         return ExecuteCmdById((int)(input->xclient.data.s[1]),
               (int)(input->xclient.data.s[2]));
      } else {
         char *c_ptr=NULL;

         c_ptr = XGetAtomName(mainDisplay, input->xclient.message_type);

         if (c_ptr == NULL) {
            /* this should not happen, but just in case */
         } else if (strcmp("WM_MOVED", c_ptr) == 0) {
            configure = TRUE;
         } else if (strcmp("WM_PROTOCOLS", c_ptr) == 0 &&
               input->xclient.data.l[0] == wmDeleteWindowAtom) {
            XFree((void*)c_ptr);
            return QuitProc();
         } else if (curChoice == DRAWTEXT && canvasFontDoubleByte &&
               textCursorShown && tgIMExpectClientMessage(mainDisplay,
               mainWindow)) {
            char buf[80];
            XKeyEvent key_ev;

            *buf = '\0';
            memset(&key_ev, 0, sizeof(XKeyEvent));
            key_ev.type = KeyPress;
            key_ev.display = mainDisplay;
            key_ev.window = drawWindow;
            key_ev.root = rootWindow;
            key_ev.same_screen = 1;
            if (tgIMHandleClientMessage(mainDisplay,
                  mainWindow, &(input->xclient), &key_ev, buf)) {
               int has_ch=TRUE;
               KeySym key_sym=(KeySym)0;

               if (((*buf) & 0x80) != '\0') {
                  key_sym = XK_space;
                  has_ch = strlen(buf);
               } else {
                  key_sym = (KeySym)(key_ev.keycode);
                  TranslateKeys(buf, &key_sym);
               }
               /*
                * This is probably not needed.
                *
                * ClearCopyUTF8Info(); 
                * if (textHighlight) {
                *    if (!CanCopyHighLightedTextAsUTF8Strings(NULL)) {
                *       ClearCopyUTF8Info();
                *    } else if (gstCopyUTF8Info.double_byte_valid &&
                *          !gstCopyUTF8Info.single_byte_valid) {
                *       CalcSingleByteInfoForCopyUTF8(NULL); 
                *    }
                * }
                */
               HandleTextFromBuffer(&key_ev, buf, key_sym, has_ch);
            }
         } else {
            sprintf(gszMsgBox, "mainWindow ClientMessage: '%s'.", c_ptr);
            Msg(gszMsgBox);
         }
         if (c_ptr != NULL) XFree((void*)c_ptr);
      }
   }

   if (configure) Reconfigure(FALSE);
   if (showCrossHair) {
      SetNullCursor(drawWindow);
   }
   return INVALID;
}

void CleanUpPaperSize()
{
   if (psPageHeightInInch != NULL) free(psPageHeightInInch);
   if (psPageWidthInInch != NULL) free(psPageWidthInInch);
   if (psYOff != NULL) free(psYOff);
   if (psYOffStr != NULL) {
      free(psYOffStr[0]);
      free(psYOffStr[1]);
      free(psYOffStr);
   }
   psYOffStr = NULL;
   psYOff = psPageWidthInInch = psPageHeightInInch = NULL;
}

void InitPaperSize()
{
   float fval=(float)0;

   if (psYOffStr != NULL) CleanUpPaperSize();

   psYOffStr = (char**)malloc(MAXPAGESTYLES*sizeof(char *));
   if (psYOffStr == NULL) FailAllocMessage();
   psYOffStr[0] = (char*)malloc(20*sizeof(char));
   psYOffStr[1] = (char*)malloc(2*sizeof(char));
   if (psYOffStr[0] == NULL || psYOffStr[1] == NULL) FailAllocMessage();
   fval = (float)(((float)onePageHeight)/((float)ONE_INCH));
   FormatFloat(&fval, psYOffStr[0]);
   sprintf(psYOffStr[1], "0");
   psYOff = (float*)malloc(MAXPAGESTYLES*sizeof(float));
   if (psYOff == NULL) FailAllocMessage();
   psYOff[0] = ((float)onePageHeight)/((float)ONE_INCH);
   psYOff[1] = (float)0.0;
   psPageWidthInInch = (float*)malloc(MAXPAGESTYLES*sizeof(float));
   if (psPageWidthInInch == NULL) FailAllocMessage();
   psPageWidthInInch[0] = ((float)onePageWidth)/((float)ONE_INCH);
   psPageWidthInInch[1] = ((float)onePageHeight)/((float)ONE_INCH);
   psPageHeightInInch = (float*)malloc(MAXPAGESTYLES*sizeof(float));
   if (psPageHeightInInch == NULL) FailAllocMessage();
   psPageHeightInInch[0] = ((float)onePageHeight)/((float)ONE_INCH);
   psPageHeightInInch[1] = ((float)onePageWidth)/((float)ONE_INCH);
}

static Pixmap	wmIconPixmap=None, wmIconPixmapMask=None;

static
int GetWMIconPixmap(pPixmap, pPixmapMask)
   Pixmap *pPixmap, *pPixmapMask;
{
   Pixmap pixmap=None, bitmap=None;
   XImage *image=NULL, *bitmap_image=NULL;
   int rc=0, image_w=0, image_h=0, w=0, h=0, ncolors=0, chars_per_pixel=0;
   int first_pixel_is_bg=FALSE, *pixels=NULL;
   char fname[MAXPATHLENGTH], *color_char=NULL, **color_str=NULL;
   char *xpm_data=NULL;
   char *c_ptr=XGetDefault(mainDisplay,TOOL_NAME, "WMIconPixmap");

   *pPixmap = *pPixmapMask = None;
   if (c_ptr == NULL) return FALSE;
   if (*c_ptr == DIR_SEP) {
      strncpy(fname, c_ptr, sizeof(fname));
   } else {
      sprintf(fname, "%s%c%s", drawPath, DIR_SEP, c_ptr);
   }
   c_ptr = UtilStrRChr(fname, '.');
   if (c_ptr != NULL && UtilStrICmp(c_ptr, ".xpm") == 0) {
      rc = MyReadPixmapFile(fname, &image_w, &image_h, &w, &h, &pixmap,
            &image, &bitmap, &bitmap_image, &ncolors, &chars_per_pixel,
            &first_pixel_is_bg, &color_char, &color_str, &pixels,
            &xpm_data);
   } else {
      int x_hot=0, y_hot=0;
      unsigned int tmp_w=0, tmp_h=0;

      rc = XReadBitmapFile(mainDisplay, mainWindow, fname, &tmp_w, &tmp_h,
            &pixmap, &x_hot, &y_hot);
   }
   if (rc != BitmapSuccess) {
      fprintf(stderr, TgLoadCachedString(CSTID_FAIL_READ_ICONPXMP_USE_BMP),
            fname);
      fprintf(stderr, "\n");
      return FALSE;
   }
   if (image != NULL) XDestroyImage(image);
   if (bitmap_image != NULL) XDestroyImage(bitmap_image);
   if (color_char != NULL) free(color_char);
   if (color_str != NULL) {
      int i;

      for (i=0; i < ncolors; i++) free(color_str[i]);
      free(color_str);
   }
   if (pixels != NULL) free(pixels);
   *pPixmap = pixmap;
   *pPixmapMask = bitmap;
   return TRUE;
}

void InitTmpDir()
{
   int tmp_dir_in_x_def=FALSE;
   char *c_ptr=NULL;
   struct stat stat_buf;

   if ((c_ptr=getenv("HOME")) == NULL) {
      strcpy(homeDir, DIR_SEP_STR);
   } else if (strlen(c_ptr) >= MAXPATHLENGTH-1) {
      strcpy(homeDir, DIR_SEP_STR);
   } else {
      strcpy(homeDir, c_ptr);
   }
   sprintf(tgifDir, "%s%c.%s", homeDir, DIR_SEP, TOOL_NAME);
   memset(&stat_buf, 0, sizeof(struct stat));
   if (stat(tgifDir, &stat_buf) != 0) {
      /* directory does not exist */
      if (mkdir(tgifDir, 0755) != 0) {
         fprintf(stderr, TgLoadString(STID_CANT_CREATE_NAMED_DIRECTORY),
               tgifDir);
         fprintf(stderr, "\n");
         if (!PRTGIF) {
            fprintf(stderr, "\t");
            fprintf(stderr, TgLoadString(STID_OPR_USING_NAMED_DIR_WILL_FAIL),
                  tgifDir);
            fprintf(stderr, "\n");
         }
      }
   }
   if (*bootDir == '\0') {
      GetWorkingDirectory(bootDir, sizeof(bootDir));
   }
   if (!PRTGIF || cmdLineOpenDisplay) {
      /* do not translate -- program constants */
      if (mainDisplay != NULL &&
            (c_ptr=XGetDefault(mainDisplay, TOOL_NAME, "TmpDir")) != NULL) {
         UtilTrimBlanks(c_ptr);
         if (*c_ptr == '\0') {
            fprintf(stderr, TgLoadString(STID_INVALID_DIR_IN_X_DEFAULT),
                  c_ptr, TOOL_NAME, "TmpDir");
            fprintf(stderr, "\n");
         } else {
            UtilStrCpyN(tmpDir, sizeof(tmpDir)-1, c_ptr);
            strcat(tmpDir, DIR_SEP_STR);
            if (UtilIsDirectory(tmpDir)) {
               tmp_dir_in_x_def = TRUE;
            } else {
               fprintf(stderr, TgLoadString(STID_TMP_DIR_NOT_EXIST_USE_XDEF),
                     tmpDir, TOOL_NAME, "TmpDir");
               fprintf(stderr, "\n");
            }
         }
      }
   }
   if (!tmp_dir_in_x_def) {
#ifndef _TMP_DIR_IN_HOME_DIR
      UtilStrCpyN(tmpDir, sizeof(tmpDir), TMP_DIR);
      if (!PRTGIF || cmdLineOpenDisplay) {
         if (mainDisplay != NULL &&
               (c_ptr=XGetDefault(mainDisplay, TOOL_NAME, "TmpDirInHomeDir")) !=
               NULL && UtilStrICmp(c_ptr, "true") == 0) {
            UtilStrCpyN(tmpDir, sizeof(tmpDir)-1, tgifDir);
            strcat(tmpDir, DIR_SEP_STR);
         }
      }
#else /* _TMP_DIR_IN_HOME_DIR */
      UtilStrCpyN(tmpDir, sizeof(tmpDir)-1, tgifDir);
      strcat(tmpDir, DIR_SEP_STR);
      if (!PRTGIF || cmdLineOpenDisplay) {
         if (mainDisplay != NULL &&
               (c_ptr=XGetDefault(mainDisplay, TOOL_NAME, "TmpDirInHomeDir")) !=
               NULL && UtilStrICmp(c_ptr, "false") == 0) {
            UtilStrCpyN(tmpDir, sizeof(tmpDir), TMP_DIR);
         }
      }
#endif /* ~_TMP_DIR_IN_HOME_DIR */
   }
}

static
int InitAntiAlias()
   /*
    * Get here because fullTrueColorMode is TRUE.  So, by default, we
    *         will use antialiased fonts, unless Tgif.NoAntialias is set
    *         to TRUE.
    * Need to add code to talk to the xft server.
    */
{
#ifdef NOT_DEFINED
   char *c_ptr=NULL;

   noAntiAlias = FALSE;
   if ((c_ptr=XGetDefault(mainDisplay, TOOL_NAME, "NoAntialias")) !=
         NULL && UtilStrICmp(c_ptr, "true") == 0) {
      noAntiAlias = TRUE;
   }
#endif /* NOT_DEFINED */
   /*
    * Add code here to talk to the xft server.
    */
   return TRUE;
}

void Setup()
{
   int bitmask=0, x_neg_in_def=FALSE, y_neg_in_def=FALSE;
   int x_in_cmdline=FALSE, y_in_cmdline=FALSE, cur_y=0, i, x_offset=0;
   int bg_pixel=(-1), def_x_neg=0, def_y_neg=0, paper_size_set=FALSE;
   char *c_ptr=NULL;
   double dval=(double)0;
   XWMHints wmhints;
   XSizeHints sizehints;
   XClassHint classhints;

   *homeDir = *bootDir = *tgifDir = '\0';
   debugNoPointerGrab = FALSE;
#ifdef _TGIF_DBG
   if ((c_ptr=XGetDefault(mainDisplay, TOOL_NAME,
         "DebugNoPointerGrab")) != NULL && UtilStrICmp(c_ptr, "true") == 0) {
      debugNoPointerGrab = TRUE;
   }
#endif /* _TGIF_DBG */
   if ((c_ptr=XGetDefault(mainDisplay, TOOL_NAME, "Synchronize")) != NULL) {
      if (UtilStrICmp(c_ptr, "on") == 0 || UtilStrICmp(c_ptr, "true") == 0) {
         XSynchronize(mainDisplay, True);
      }
   }
   showVersion = FALSE;
   if ((c_ptr=XGetDefault(mainDisplay, TOOL_NAME, "DontShowVersion")) != NULL &&
         UtilStrICmp(c_ptr, "false") == 0) {
      showVersion = TRUE;
   }

   *printCommand = '\0';
   if ((c_ptr=XGetDefault(mainDisplay, TOOL_NAME, "PrintCommand")) != NULL) {
      strcpy(printCommand, c_ptr);
   } else {
#ifdef PRINT_CMD
      strcpy(printCommand, PRINT_CMD);
#else
#ifdef VMS
      strcpy(printCommand, "print");
#else
#ifdef SYSV
      strcpy(printCommand, "lp -dpostscript");
#else
      strcpy(printCommand, "lpr");
#endif /* SYSV */
#endif /* VMS */
#endif /* PRINT_CMD */
   }
   if (!PRTGIF) *outputDir = '\0';
   if ((c_ptr=XGetDefault(mainDisplay, TOOL_NAME, "PrintDirectory")) != NULL) {
      strcpy(outputDir, c_ptr);
   }
   whereToPrint = LATEX_FIG;
   if ((c_ptr=XGetDefault(mainDisplay, TOOL_NAME, "WhereToPrint")) != NULL) {
      if (UtilStrICmp(c_ptr, "Printer") == 0) {
         whereToPrint = PRINTER;
      } else if (UtilStrICmp(c_ptr, "EPS") == 0) {
         whereToPrint = LATEX_FIG;
      } else if (UtilStrICmp(c_ptr, "PS") == 0) {
         whereToPrint = PS_FILE;
      } else if (UtilStrICmp(c_ptr, "Bitmap") == 0) {
         whereToPrint = XBM_FILE;
      } else if (UtilStrICmp(c_ptr, "Text") == 0) {
         whereToPrint = TEXT_FILE;
      } else if (UtilStrICmp(c_ptr, "EPSI") == 0) {
         whereToPrint = EPSI_FILE;
      } else if (UtilStrICmp(c_ptr, "GIF") == 0) {
         whereToPrint = GIF_FILE;
      } else if (UtilStrICmp(c_ptr, "HTML") == 0) {
         whereToPrint = HTML_FILE;
      } else if (UtilStrICmp(c_ptr, "PDF") == 0) {
         whereToPrint = PDF_FILE;
      } else if (UtilStrICmp(c_ptr, "TiffEPSI") == 0 ||
            UtilStrICmp(c_ptr, "WinEPSI") == 0) {
         whereToPrint = TIFFEPSI_FILE;
      } else if (UtilStrICmp(c_ptr, "PNG") == 0) {
         whereToPrint = PNG_FILE;
      } else if (UtilStrICmp(c_ptr, "JPEG") == 0) {
         whereToPrint = JPEG_FILE;
      } else if (UtilStrICmp(c_ptr, "PPM") == 0 ||
            UtilStrICmp(c_ptr, "PBM") == 0) {
         whereToPrint = PPM_FILE;
      } else if (UtilStrICmp(c_ptr, "NetList") == 0) {
         whereToPrint = NETLIST_FILE;
      } else if (UtilStrICmp(c_ptr, "SVG") == 0) {
         whereToPrint = SVG_FILE;
      } else {
         fprintf(stderr, TgLoadString(STID_INVALID_XDEF_USE_ALT_STR),
               TOOL_NAME, "WhereToPrint", c_ptr, "EPS");
         fprintf(stderr, "\n");
      }
   }
   useGray = FALSE;
   if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"UseGrayScale")) != NULL) {
      if (UtilStrICmp("true", c_ptr) == 0) {
         useGray = TRUE;
      }
   }
   autoPan = TRUE;
   if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"AutoPanInEditText")) != NULL) {
      if (UtilStrICmp("false", c_ptr) == 0) {
         autoPan = FALSE;
      }
   }
   moveMode = UNCONST_MOVE;
   if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"ConstrainedMove")) != NULL) {
      if (UtilStrICmp("true", c_ptr) == 0) {
         moveMode = CONST_MOVE;
      }
   }
   doubleQuoteDoubleQuote = FALSE;
   if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"DoubleQuoteDoubleQuote"))
         != NULL) {
      if (UtilStrICmp("true", c_ptr) == 0) {
         doubleQuoteDoubleQuote = TRUE;
      }
   }
   gridSystem = ENGLISH_GRID;
   if ((c_ptr=XGetDefault(mainDisplay, TOOL_NAME, "GridSystem")) != NULL) {
      if (UtilStrICmp("Metric", c_ptr) == 0) {
         gridSystem = METRIC_GRID;
      }
   }
   xyEnglishGrid = DEFAULT_ENGLISH_GRID;
   xyMetricGrid = DEFAULT_METRIC_GRID;
   if ((c_ptr=XGetDefault(mainDisplay, TOOL_NAME, "InitialGrid")) != NULL) {
      int grid_size=atoi(c_ptr);

      switch (gridSystem) {
      case ENGLISH_GRID:
         if (grid_size < -2 || grid_size > 2) {
            fprintf(stderr, TgLoadString(STID_INVALID_XDEF_USE_ALT_VALUE),
                  TOOL_NAME, "InitialGrid", c_ptr, 0);
            fprintf(stderr, "\n");
         } else {
            xyEnglishGrid = HALF_INCH>>(2-grid_size);
         }
         break;
      case METRIC_GRID:
         if (grid_size < -1 || grid_size > 2) {
            fprintf(stderr, TgLoadString(STID_INVALID_XDEF_USE_ALT_VALUE),
                  TOOL_NAME, "InitialGrid", c_ptr, 0);
            fprintf(stderr, "\n");
         } else {
            switch (grid_size) {
            case -1: xyMetricGrid = ONE_MM; break;
            case 0: xyMetricGrid = TWO_MM; break;
            case 1: xyMetricGrid = FIVE_MM; break;
            case 2: xyMetricGrid = ONE_CM; break;
            }
         }
         break;
      }
   }
   splineTol = 9;
   if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"SplineTolerance")) != NULL) {
      splineTol = atoi(c_ptr);
      if (splineTol < 3 || splineTol > 13) {
         fprintf(stderr, TgLoadString(STID_INVALID_XDEF_USE_ALT_VALUE),
               TOOL_NAME, "SplineTolerance", c_ptr, 9);
         fprintf(stderr, "\n");
         splineTol = 9;
      }
   }
   if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"SplineRubberband")) != NULL) {
      if (UtilStrICmp(c_ptr, "false") == 0) {
         sprintf(gszMsgBox, TgLoadString(STID_NAMED_XDEF_IS_OBSOLETE),
               TOOL_NAME, "SplineRubberband");
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      }
   }
   saveTmpOnReturn = TRUE;
   if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"SaveTmpOnReturn")) != NULL) {
      if (UtilStrICmp(c_ptr, "false") == 0) {
         saveTmpOnReturn = FALSE;
      }
   }
   dropObsIconAttrWhenUpdate = FALSE;
   if ((c_ptr=XGetDefault(mainDisplay, TOOL_NAME,
         "DropObsIconAttrWhenUpdate")) != NULL) {
      if (UtilStrICmp("true", c_ptr) == 0) {
         dropObsIconAttrWhenUpdate = TRUE;
      }
   }
   useRecentDupDistance = TRUE;
   if ((c_ptr=XGetDefault(mainDisplay, TOOL_NAME,
         "UseRecentDupDistance")) != NULL) {
      if (UtilStrICmp("false", c_ptr) == 0) {
         useRecentDupDistance = FALSE;
      }
   }
   handleSize = 3;
   if ((c_ptr=XGetDefault(mainDisplay, TOOL_NAME, "HandleSize")) != NULL) {
      handleSize = atoi(c_ptr);
      if (handleSize < 2 || handleSize > 6) {
         fprintf(stderr, TgLoadString(STID_INVALID_XDEF_USE_ALT_VALUE),
               TOOL_NAME, "HandleSize", c_ptr, 3);
         fprintf(stderr, "\n");
         handleSize = 3;
      }
   }
   historyDepth = -1;
   if ((c_ptr=XGetDefault(mainDisplay, TOOL_NAME, "HistoryDepth")) != NULL) {
      historyDepth = atoi(c_ptr);
   }
   defaultHistoryDepth = historyDepth;

   undoRedoRestoreDrawingMode = TRUE;
   if ((c_ptr=XGetDefault(mainDisplay, TOOL_NAME,
         "UndoRedoRestoreDrawingMode")) != NULL &&
         UtilStrICmp("false", c_ptr) == 0) {
      undoRedoRestoreDrawingMode = FALSE;
   }
   canvasWindowOnly = FALSE;
   if (cmdLineCWO) {
      canvasWindowOnly = TRUE;
   } else if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"CanvasWindowOnly")) !=
         NULL) {
      if (UtilStrICmp("true", c_ptr) == 0) {
         canvasWindowOnly = TRUE;
      }
   }
   btn1Warp = FALSE;
   if (canvasWindowOnly) {
      if (cmdLineBtn1Warp) {
         btn1Warp = TRUE;
      } else if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,
            "QuickWarp")) != NULL) {
         if (UtilStrICmp("on", c_ptr) == 0 || UtilStrICmp("true", c_ptr) == 0) {
            btn1Warp = TRUE;
         }
      } else if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,
            "SingleButtonWarp")) != NULL) {
         if (UtilStrICmp("true", c_ptr) == 0) {
            btn1Warp = TRUE;
         }
      }
   }
   saveCommentsInSaveNew = TRUE;
   if ((c_ptr=XGetDefault(mainDisplay, TOOL_NAME,
         "SaveCommentsInSaveNew")) != NULL) {
      if (UtilStrICmp("false", c_ptr) == 0 ) {
         saveCommentsInSaveNew = FALSE;
      }
   }
   usePsAdobeString = FALSE;
   *adobeString = *epsfString = '\0';
#ifdef _USE_PS_ADOBE_STRING
   {
      char buf[MAXSTRING];

      UtilStrCpyN(buf, sizeof(buf), _USE_PS_ADOBE_STRING);
      if (!ParsePsAdobeString(buf,&usePsAdobeString,adobeString,epsfString)) {
         fprintf(stderr, TgLoadString(STID_BAD_COMPILE_OPT_USE_ALT_STR),
               "_USE_PS_ADOBE_STRING", _USE_PS_ADOBE_STRING, "false");
         fprintf(stderr, "\n");
         usePsAdobeString = FALSE;
         *adobeString = *epsfString = '\0';
      }
   }
#endif /* _USE_PS_ADOBE_STRING */
   if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"UsePsAdobeString")) != NULL) {
      char tmp_str[80];

      usePsAdobeString = FALSE;
      *adobeString = *epsfString = '\0';

      UtilStrCpyN(tmp_str, sizeof(tmp_str), c_ptr);
      if (!ParsePsAdobeString(tmp_str, &usePsAdobeString, adobeString,
            epsfString)) {
         fprintf(stderr, TgLoadString(STID_INVALID_XDEF_USE_ALT_STR),
               TOOL_NAME, "UsePsAdobeString", tmp_str, "false");
         fprintf(stderr, "\n");
         usePsAdobeString = FALSE;
         *adobeString = *epsfString = '\0';
      }
   }
   groupedTextEditable = TRUE;
   if ((c_ptr=XGetDefault(mainDisplay, TOOL_NAME, "GroupedTextEditable")) !=
         NULL && UtilStrICmp(c_ptr, "false") == 0) {
      groupedTextEditable = FALSE;
   }
   intrCheckInterval = 10;
   if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"IntrCheckInterval")) != NULL) {
      intrCheckInterval = atoi(c_ptr);
   }
   tiledPageScaling = 0.9;
   if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"TiledPageScaling")) != NULL) {
      tiledPageScaling = (float)atof(c_ptr);
      if (tiledPageScaling <= 0.0 || tiledPageScaling > 1.0) {
         fprintf(stderr, TgLoadString(STID_INVALID_XDEF_USE_ALT_STR),
               TOOL_NAME, "TiledPageScaling", c_ptr, "0.9");
         fprintf(stderr, "\n");
         tiledPageScaling = 0.9;
      }
   }
   stickyMenuSelection = TRUE;
   if ((c_ptr=XGetDefault(mainDisplay, TOOL_NAME,
         "StickyMenuSelection")) != NULL && UtilStrICmp("false", c_ptr) == 0) {
      stickyMenuSelection = FALSE;
   }
   gridShown = TRUE;
   if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"InitialShowGrid")) != NULL) {
      if (UtilStrICmp("false", c_ptr) == 0) {
         gridShown = FALSE;
      }
   }
   snapOn = TRUE;
   if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"InitialSnapOn")) != NULL) {
      if (UtilStrICmp("false", c_ptr) == 0) {
         snapOn = FALSE;
      }
   }
   noMenubar = FALSE;
   if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"NoMenubar")) != NULL) {
      if (UtilStrICmp("true", c_ptr) == 0) {
         noMenubar = TRUE;
      }
   }
   noStatusWindow = FALSE;
   if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"NoStatusWindow")) != NULL) {
      if (UtilStrICmp("true", c_ptr) == 0) {
         noStatusWindow = TRUE;
      }
   }
   noChoiceWindow = FALSE;
   if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"NoChoiceWindow")) != NULL) {
      if (UtilStrICmp("true", c_ptr) == 0) {
         noChoiceWindow = TRUE;
      }
   }
   if (cmdLineNoModeWindow) {
      noModeWindow = TRUE;
   } else {
      noModeWindow = FALSE;
      if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"NoModeWindow")) != NULL) {
         if (UtilStrICmp("true", c_ptr) == 0) {
            noModeWindow = TRUE;
         }
      }
   }
   if (gstWBInfo.do_whiteboard) {
      if (cmdLineNoChatWindow) {
         noChatWindow = TRUE;
      } else {
         noChatWindow = FALSE;
         if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"NoChatWindow"))
               != NULL) {
            if (UtilStrICmp("true", c_ptr) == 0) {
               noChatWindow = TRUE;
            }
         }
      }
   }
   scrollBarW = baseScrollBarW = SCROLLBAR_W;
   if ((c_ptr=XGetDefault(mainDisplay, TOOL_NAME, "ScrollBarWidth")) != NULL) {
      baseScrollBarW = atoi(c_ptr);
      if (baseScrollBarW < 2 || baseScrollBarW > 16) {
         fprintf(stderr, TgLoadString(STID_INVALID_XDEF_RNG_USE_ALT_VAL),
               TOOL_NAME, "ScrollBarWidth", c_ptr, 2, 16, SCROLLBAR_W);
         fprintf(stderr, "\n");
         baseScrollBarW = SCROLLBAR_W;
      }
   }
   paper_size_set = FALSE;
   if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"InitialPaperSize")) != NULL) {
      paper_size_set = SetPaperSize(c_ptr);
   }
   if (cmdLineLetter) {
      paper_size_set = SetPaperSize("letter");
   }
   pageStyle = PORTRAIT;
   if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"PageStyleLandscape")) !=
         NULL && UtilStrICmp("true", c_ptr) == 0) {
      pageStyle = LANDSCAPE;
      paper_size_set = TRUE;
   }
   /* do not translate -- program constants */
   wmDeleteWindowAtom = XInternAtom(mainDisplay, "WM_DELETE_WINDOW", False);
   targetsAtom = XInternAtom(mainDisplay, "TARGETS", False);
   utf8StringAtom = XInternAtom(mainDisplay, "UTF8_STRING", False);
   tgifProtocolAtom = XInternAtom(mainDisplay, TGIF_PROTOCOL_ATOM, False);
   tmpSelectionAtom = XInternAtom(mainDisplay, "TGIF_TMP_SELECTION_ATOM",
         False);
   compoundTextAtom = XInternAtom(mainDisplay, "COMPOUND_TEXT", False);
   textAtom = XInternAtom(mainDisplay, "TEXT", False);
   executeCmdByIDAtom = XInternAtom(mainDisplay, "TGIF_EXECUTE_CMD_BY_ID",
         False);

   InitStrTable();
   InitPaperSize();

   printMag = (float)100.0;
   if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"PercentPrintReduction")) !=
         NULL) {
      float print_reduction=(float)0.0;

      if (sscanf(c_ptr, "%f", &print_reduction) != 1) {
         fprintf(stderr, TgLoadString(STID_INVALID_XDEF_USE_ALT_VALUE),
               TOOL_NAME, "PercentPrintReduction", c_ptr, 100);
         fprintf(stderr, "\n");
         print_reduction = (float)100.0;
      }
      if (printMag != print_reduction) {
         printMag = print_reduction;
         paper_size_set = TRUE;
      }
   }
   if (paper_size_set) UpdPageStyle(pageStyle);

   usePaperSizeStoredInFile = TRUE;
   if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"UsePaperSizeStoredInFile")) !=
         NULL && UtilStrICmp("false", c_ptr) == 0) {
      usePaperSizeStoredInFile = FALSE;
   }
   oneMotionSelectMove = FALSE;
   if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"OneMotionSelMove")) != NULL) {
      if (UtilStrICmp("true", c_ptr) == 0) {
         oneMotionSelectMove = TRUE;
      }
   }
   queryZoomInPoint = FALSE;
   if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"QueryZoomInPoint")) != NULL) {
      if (UtilStrICmp("true",c_ptr)==0 || UtilStrICmp("always",c_ptr)==0) {
         queryZoomInPoint = TRUE;
      } else if (UtilStrICmp("no_select",c_ptr) == 0) {
         queryZoomInPoint = INVALID;
      } else if (UtilStrICmp("no_query",c_ptr) == 0) {
         queryZoomInPoint = BAD;
      }
   }
   lineStyle = LS_RIGHT;
   if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"InitialArrowStyle")) != NULL) {
      if (UtilStrICmp("NONE",c_ptr) == 0) {
         lineStyle = LS_PLAIN;
      } else if (UtilStrICmp("RIGHT",c_ptr) == 0) {
         lineStyle = LS_RIGHT;
      } else if (UtilStrICmp("LEFT",c_ptr) == 0) {
         lineStyle = LS_LEFT;
      } else if (UtilStrICmp("DOUBLE",c_ptr) == 0) {
         lineStyle = LS_DOUBLE;
      } else {
         fprintf(stderr, TgLoadString(STID_INVALID_XDEF_USE_ALT_STR),
               TOOL_NAME, "InitialArrowStyle", c_ptr, "RIGHT");
         fprintf(stderr, "\n");
      }
   }
   showPageInEPS = TRUE;
   if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"ShowPageInEPS")) !=
         NULL && UtilStrICmp("false", c_ptr) == 0) {
      showPageInEPS = FALSE;
   }
   oneMotionTimeout = 200;
   if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"OneMotionTimeout")) != NULL) {
      oneMotionTimeout = atoi(c_ptr);
      if (oneMotionTimeout < 0 || oneMotionTimeout >= 1000) {
         fprintf(stderr, TgLoadString(STID_INVALID_XDEF_RNG_USE_ALT_VAL),
               TOOL_NAME, "OneMotionTimeout", c_ptr, 0, 1000, 200);
         fprintf(stderr, "\n");
         oneMotionTimeout = 200;
      }
   }
   minMoveInterval = 0;
   if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"MinMoveInterval")) != NULL) {
      minMoveInterval = atoi(c_ptr);
      if (minMoveInterval < 0 || minMoveInterval >= 1000) {
         fprintf(stderr, TgLoadString(STID_INVALID_XDEF_RNG_USE_ALT_VAL),
               TOOL_NAME, "MinMoveInterval", c_ptr, 0, 1000, 0);
         fprintf(stderr, "\n");
         minMoveInterval = 0;
      }
   }
   rotationIncrement = (45<<6); /* initially 45 degrees */
   if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"RotationIncrement")) != NULL) {
      float fval;

      if (sscanf(c_ptr, "%f", &fval) != 1 || fval < (float)0.0) {
         fprintf(stderr, TgLoadString(STID_INVALID_XDEF_USE_ALT_VALUE),
               TOOL_NAME, "RotationIncrement", c_ptr, 45);
         fprintf(stderr, "\n");
      } else {
         while (fval < (float)360.0) fval += (float)360.0;
         while (fval >= (float)360.0) fval -= (float)360.0;
         fval *= 64.0;
         rotationIncrement = round(fval);
      }
   }
   stretchableText = TRUE;
   if ((c_ptr=XGetDefault(mainDisplay, TOOL_NAME, "StretchableText")) != NULL &&
         UtilStrICmp(c_ptr, "false") == 0) {
      stretchableText = FALSE;
   }
   compoundObjWithTextStretchableForPSE = FALSE;
   if ((c_ptr=XGetDefault(mainDisplay, TOOL_NAME,
         "CompoundObjWithTextStretchableForPSE")) != NULL &&
         UtilStrICmp(c_ptr, "true") == 0) {
      compoundObjWithTextStretchableForPSE = TRUE;
   }
   SetEditTextSizeValue(0);
   if ((c_ptr=XGetDefault(mainDisplay, TOOL_NAME, "EditTextSize")) != NULL) {
      int new_edit_text_size=atoi(c_ptr);

      if (new_edit_text_size == 0) {
      } else if (new_edit_text_size < 4 || new_edit_text_size > 34) {
         fprintf(stderr, TgLoadString(STID_INVALID_XDEF_RNG_USE_ALT_VAL),
               TOOL_NAME, "EditTextSize", c_ptr, 4, 34, 14);
         fprintf(stderr, "\n");
         SetEditTextSizeValue(14);
      } else {
         SetEditTextSizeValue(new_edit_text_size);
      }
   }
   tmpFileMode = PSFILE_MOD;
   if ((c_ptr=XGetDefault(mainDisplay, TOOL_NAME, "TmpFileMode")) != NULL) {
      int nFileMode=0;

      strcpy(gszMsgBox, c_ptr);

      UtilTrimBlanks(gszMsgBox);
      if (*gszMsgBox != '\0' && sscanf(gszMsgBox, "%o", &nFileMode) == 1) {
         if (nFileMode >= 0) {
            tmpFileMode = nFileMode;
         } else {
            fprintf(stderr, TgLoadString(STID_INVALID_XDEF_USE_ALT_OCTVALUE),
                  TOOL_NAME, "TmpFileMode", gszMsgBox, PSFILE_MOD);
            fprintf(stderr, "\n");
         }
      }
   }
   titledPinnedMenu = TRUE;
   if ((c_ptr=XGetDefault(mainDisplay, TOOL_NAME, "TitledPinnedMenu")) !=
         NULL && UtilStrICmp(c_ptr, "false") == 0) {
      titledPinnedMenu = FALSE;
   }
   gfVectorWarpSoftness = (float)2.0;
   if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"VectorWarpSoftness")) !=
         NULL) {
      if (sscanf(c_ptr, "%f", &gfVectorWarpSoftness) != 1) {
         fprintf(stderr, TgLoadString(STID_INVALID_XDEF_USE_ALT_STR),
               TOOL_NAME, "VectorWarpSoftness", c_ptr, "2.0");
         fprintf(stderr, "\n");
         gfVectorWarpSoftness = (float)2.0;
      } else if (gfVectorWarpSoftness > 4.0 || gfVectorWarpSoftness < 1.0) {
         fprintf(stderr, TgLoadString(STID_INVALID_XDEF_RNG_USE_ALT_STR),
               TOOL_NAME, "VectorWarpSoftness", c_ptr, "1.0", "4.0", "2.0");
         fprintf(stderr, "\n");
         gfVectorWarpSoftness = (float)2.0;
      }
   }
   queryPolyNameOnConnect = TRUE;
   if ((c_ptr=XGetDefault(mainDisplay, TOOL_NAME, "QueryPolyNameOnConnect")) !=
         NULL && UtilStrICmp(c_ptr, "false") == 0) {
      queryPolyNameOnConnect = FALSE;
   }
   shiftForDiagMouseMove = TRUE;
   if ((c_ptr=XGetDefault(mainDisplay, TOOL_NAME,
         "ShiftForDiagMouseMove")) != NULL) {
      if (UtilStrICmp(c_ptr, "true") == 0) {
         shiftForDiagMouseMove = TRUE;
      } else if (UtilStrICmp(c_ptr, "false") == 0) {
         shiftForDiagMouseMove = FALSE;
      }
   }
   useRecentForDiagMouseMove = FALSE;
#ifdef RESTRICTED_MOUSE_MOVE
   useRecentForDiagMouseMove = TRUE;
#endif /* RESTRICTED_MOUSE_MOVE */
   if ((c_ptr=XGetDefault(mainDisplay, TOOL_NAME,
         "UseRecentForDiagMouseMove")) != NULL) {
      if (UtilStrICmp(c_ptr, "true") == 0) {
         useRecentForDiagMouseMove = TRUE;
      } else if (UtilStrICmp(c_ptr, "false") == 0) {
         useRecentForDiagMouseMove = FALSE;
      }
   }
   flushColormapOnOpen = FALSE;
#ifdef USE_FLUSH_COLORMAP
   flushColormapOnOpen = TRUE;
#endif /* USE_FLUSH_COLORMAP */
   if ((c_ptr=XGetDefault(mainDisplay, TOOL_NAME,
         "FlushColormapOnOpen")) != NULL) {
      if (UtilStrICmp(c_ptr, "true") == 0) {
         flushColormapOnOpen = TRUE;
      } else if (UtilStrICmp(c_ptr, "false") == 0) {
         flushColormapOnOpen = FALSE;
      }
   }
   transPat = FALSE;
   if ((c_ptr=XGetDefault(mainDisplay, TOOL_NAME, "TransparentPattern")) !=
         NULL && UtilStrICmp(c_ptr, "true") == 0) {
      transPat = TRUE;
   }
   initialMenubarWindowH = MENUBAR_WINDOW_H;

   *scanFileFullPath = *scanFileName = '\0';
   scanLineNum = 0;

   colorDisplay = (cmdLineBW || DisplayPlanes(mainDisplay,mainScreen)==1 ||
         mainVisual->class==StaticGray) ? FALSE : TRUE;
   fullTrueColorMode = FALSE;
   if (colorDisplay && mainVisual->class == TrueColor) {
      if (HasZlibSupport()) {
         /* starting with tgif-4.2.3, this is the default */
         fullTrueColorMode = TRUE;
#ifdef _TGIF_DBG
      } else {
         fprintf(stderr, TgLoadString(STID_CANNOT_SET_XDEF_TRUE_NO_ZLIB),
               TOOL_NAME, "EnableTrueColorImages", TOOL_NAME);
         fprintf(stderr, "\n");
#endif /* _TGIF_DBG */
      }
      if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"EnableTrueColorImages")) !=
            NULL && UtilStrICmp(c_ptr, "false") == 0) {
         fullTrueColorMode = FALSE;
      }
   }
   noAntiAlias = TRUE;
   if (fullTrueColorMode) {
      InitAntiAlias();
   }
   threeDLook = colorDisplay;
   if (threeDLook && (c_ptr=XGetDefault(mainDisplay, TOOL_NAME, "3DLook")) !=
         NULL && UtilStrICmp(c_ptr, "false") == 0) {
      threeDLook = FALSE;
   }
   dialogboxUse3DBorder = threeDLook;
   if (threeDLook && (c_ptr=XGetDefault(mainDisplay, TOOL_NAME,
         "DialogboxUse3DBorder")) != NULL && UtilStrICmp(c_ptr, "false") == 0) {
      dialogboxUse3DBorder = FALSE;
   }
   if (threeDLook) {
      windowPadding = WINDOW_PADDING;
      brdrW = 0;
   }
   InitColor();
   bg_pixel = (threeDLook ? myLtGryPixel : myBgPixel);

   if ((c_ptr=getenv("TGIFPATH")) == NULL) {
      if ((c_ptr=XGetDefault(mainDisplay, TOOL_NAME, "TGIFPATH")) != NULL) {
         strcpy(drawPath, c_ptr);
      } else {
         strcpy(drawPath, TGIF_PATH);
      }
   } else if (strlen(c_ptr) >= 255) {
      strcpy(drawPath, TGIF_PATH);
   } else {
      strcpy(drawPath, c_ptr);
   }
   i = strlen(drawPath);
   if (drawPath[i-1] == DIR_SEP) drawPath[i-1] = '\0';

   InitTmpDir();

   enableMouseWheel = TRUE;
   if (threeDLook && (c_ptr=XGetDefault(mainDisplay, TOOL_NAME,
         "EnableMouseWheel")) != NULL && UtilStrICmp(c_ptr, "false") == 0) {
      enableMouseWheel = FALSE;
   }
   btn2PopupMainMenu = TRUE;
   if (threeDLook && (c_ptr=XGetDefault(mainDisplay, TOOL_NAME,
         "Btn2PopupMainMenu")) != NULL && UtilStrICmp(c_ptr, "false") == 0) {
      btn2PopupMainMenu = FALSE;
   }
   defRightMarginEnabled = INVALID;
   if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"EnableRightMargin")) != NULL) {
      if (UtilStrICmp("true", c_ptr) == 0) {
         defRightMarginEnabled = TRUE;
      } else {
         defRightMarginEnabled = FALSE;
      }
   }
   if (gridSystem == ENGLISH_GRID) {
      defRightMargin = ONE_INCH;
   } else {
      defRightMargin = (ONE_CM<<1)+(ONE_CM>>1);
   }
   dval = ((double)defRightMargin) * ((double)100) * ((double)TIK_PER_PIXEL) /
         ((double)printMag);
   defRightMargin = round(dval);
   if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"RightMargin")) != NULL) {
      char spec_buf[40];

      UtilStrCpyN(spec_buf, sizeof(spec_buf), c_ptr);
      UtilTrimBlanks(spec_buf);
      if (GetDimensionInDouble(spec_buf, TRUE, &dval)) {
         dval = dval * ((double)100) * ((double)TIK_PER_PIXEL) /
               ((double)printMag);
         defRightMargin = round(dval);
      } else {
         fprintf(stderr, TgLoadString(STID_INVALID_XDEF_USE_ALT_STR),
               TOOL_NAME, "RightMargin", c_ptr,
               (gridSystem==ENGLISH_GRID) ? "1 in" : "2.5 cm");
         fprintf(stderr, "\n");
      }
   }
   rightMargin = defRightMargin;
   rightMarginEnabled = defRightMarginEnabled;
   rightMarginActive = (rightMarginEnabled == TRUE);

   InitFonts();

   initialMenubarWindowH = defaultFontHeight;
   if (msgFontSet==NULL && msgFontPtr == NULL) {
      statusSubWindowH[0] = defaultFontHeight+2+windowPadding;
   } else {
      statusSubWindowH[0] = msgFontHeight+2+windowPadding;
   }
   statusWindowH = max(statusSubWindowH[0]+(brdrW<<1)+2, btn1_height+2);
   if (threeDLook) {
      statusWindowH += windowPadding;
   }
   userRedrawWindowH = statusWindowH;

   InitChat();
   chatWindowH = GetInitialChatWindowHeight();
   if (threeDLook) {
      chatWindowH += windowPadding;
   }
   noMinWinSize = TRUE;
   if (threeDLook && (c_ptr=XGetDefault(mainDisplay, TOOL_NAME,
         "NoMinWinSize")) != NULL && UtilStrICmp(c_ptr, "false") == 0) {
      noMinWinSize = FALSE;
   }
   if (noMinWinSize) {
      sizehints.flags = PSize;
   } else {
      sizehints.flags = PSize | PMinSize;
   }
   sizehints.x = 0;
   sizehints.y = 0;
   sizehints.width = mainWinW;
   sizehints.height = mainWinH;

   if ((c_ptr=XGetDefault(mainDisplay, TOOL_NAME, "Geometry")) != NULL) {
      bitmask = XParseGeometry(c_ptr, &(sizehints.x), &(sizehints.y),
            (unsigned int *)&(sizehints.width),
            (unsigned int *)&(sizehints.height));
      if (bitmask & (XValue | YValue)) sizehints.flags |= USPosition;
      if (bitmask & (WidthValue | HeightValue)) {
         sizehints.flags |= USSize;
         drawWinW = sizehints.width;
         drawWinH = sizehints.height;
         InitWinSizes();
         mainWinW = sizehints.width;
         mainWinH = sizehints.height;
         InverseInitWinSizes();
      }
      if (bitmask & XNegative) {
         x_neg_in_def = TRUE;
         def_x_neg = sizehints.x;
      }
      if (bitmask & YNegative) {
         y_neg_in_def = TRUE;
         def_y_neg = sizehints.y;
      }
   }

   if (geometrySpecified && *geometrySpec != '\0') {
      bitmask = XParseGeometry(geometrySpec, &(sizehints.x), &(sizehints.y),
            (unsigned int *)&(sizehints.width),
            (unsigned int *)&(sizehints.height));
      if (bitmask & (XValue | YValue)) sizehints.flags |= USPosition;
      if (bitmask & (WidthValue | HeightValue)) {
         sizehints.flags |= USSize;
         drawWinW = sizehints.width;
         drawWinH = sizehints.height;
         InitWinSizes();
         mainWinW = sizehints.width;
         mainWinH = sizehints.height;
         InverseInitWinSizes();
      }
      if (bitmask & XValue) x_in_cmdline = TRUE;
      if (bitmask & YValue) y_in_cmdline = TRUE;
   }
   InitWinSizes();

   if (canvasWindowOnly) {
      noMenubar = TRUE;
      noStatusWindow = TRUE;
      noChoiceWindow = TRUE;
      noModeWindow = TRUE;
      noChatWindow = TRUE;

      mainWinW = initDrawWinW;
      mainWinH = initDrawWinH;

      switch (gridSystem) {
      case ENGLISH_GRID:
         sizehints.min_width = HALF_INCH+3*brdrW;
         sizehints.min_height = HALF_INCH+3*brdrW;
         break;
      case METRIC_GRID:
         sizehints.min_width = ONE_CM+3*brdrW;
         sizehints.min_height = ONE_CM+3*brdrW;
         break;
      }
      if (mainWinW > sizehints.min_width) {
         sizehints.width = mainWinW;
      } else {
         sizehints.width = sizehints.min_width = mainWinW;
      }
      if (mainWinH > sizehints.min_height) {
         sizehints.height = mainWinH;
      } else {
         sizehints.height = sizehints.min_height = mainWinH;
      }
      drawWinW = initDrawWinW = mainWinW-(brdrW<<1);
      drawWinH = initDrawWinH = mainWinH-(brdrW<<1);
      if (msgWindowW <= 0) msgWindowW = 1;
   } else {
      sizehints.min_width =
            (noModeWindow ? 0 : modeWindowW+(brdrW<<1))+
            choiceWindowW+(brdrW<<2)+defaultFontWidth;
      sizehints.min_height =
            (noMenubar ? 0 : menubarWindowH+(brdrW<<1))+
            (noStatusWindow ? 0 : statusWindowH+(brdrW<<1))+
            (noChoiceWindow ? 0 : msgWindowH+(brdrW<<1))+
            (noChatWindow ? 0 : chatWindowH+(brdrW<<1))+
            titleWindowH+ONE_INCH+rulerW+scrollBarW+8*brdrW;
      if (mainWinW > sizehints.min_width) {
         sizehints.width = mainWinW;
      } else {
         mainWinW = sizehints.width = sizehints.min_width;
      }
      if (mainWinH > sizehints.min_height) {
         sizehints.height = mainWinH;
      } else {
         mainWinH = sizehints.height = sizehints.min_height;
      }
      InverseInitWinSizes();
   }
   if (bitmask & XNegative) {
      sizehints.x += DisplayWidth(mainDisplay,mainScreen)-mainWinW-1;
   } else if (!x_in_cmdline && x_neg_in_def) {
      sizehints.x = def_x_neg+DisplayWidth(mainDisplay,mainScreen)-mainWinW-1;
   }
   if (bitmask & YNegative) {
      sizehints.y += DisplayHeight(mainDisplay,mainScreen)-mainWinH-1;
   } else if (!y_in_cmdline && y_neg_in_def) {
      sizehints.y = def_y_neg+DisplayHeight(mainDisplay,mainScreen)-mainWinH-1;
   }
   if ((mainWindow=XCreateSimpleWindow(mainDisplay, rootWindow,
         sizehints.x, sizehints.y, sizehints.width, sizehints.height,
         brdrW, myBorderPixel, myBgPixel)) == 0) {
      FailToCreateWindowMessage("Setup()", NULL, TRUE);
   }
   if (gstWBInfo.do_whiteboard) {
      if ( cmdLineParentWindowID > 0 ) {
         XReparentWindow(mainDisplay, mainWindow, cmdLineParentWindowID, 0, 0);
      }
   } else {
      if ( cmdLineTgrm2 && cmdLineParentWindowID > 0 ) {
         XReparentWindow(mainDisplay, mainWindow, cmdLineParentWindowID, 0, 0);
      }
   }
   if (newColormapUsed) {
      XSetWindowColormap(mainDisplay, mainWindow, mainColormap);
   }
   InitMenu();
   InitPattern();

   InitRuler();
   InitNames();
   InitStk();
   InitXBm();
   InitXPm();
   CreateCursor();
   lastPageNum = 1;
   InitPage();
   InitPins();
   InitInputMethods();
   InitPS();

   if ((titleWindow=XCreateSimpleWindow(mainDisplay, mainWindow, 0, 0,
         titleWindowW, titleWindowH, brdrW, myBorderPixel, bg_pixel)) == 0) {
      FailToCreateWindowMessage("Setup()", NULL, TRUE);
   }
   cur_y += titleWindowH + (brdrW<<1);

   if ((menubarWindow=XCreateSimpleWindow(mainDisplay, mainWindow, 0,
         cur_y, menubarWindowW, menubarWindowH, brdrW, myBorderPixel,
         bg_pixel)) == 0) {
      FailToCreateWindowMessage("Setup()", NULL, TRUE);
   }
   if (!noMenubar) {
      cur_y += menubarWindowH + (brdrW<<1);
   }
   if ((choiceWindow=XCreateSimpleWindow(mainDisplay, mainWindow, 0, cur_y,
         choiceWindowW, choiceWindowH, brdrW, myBorderPixel, bg_pixel)) == 0) {
      FailToCreateWindowMessage("Setup()", NULL, TRUE);
   }
   if ((msgWindow=XCreateSimpleWindow(mainDisplay, mainWindow,
         choiceWindowW+(brdrW<<1), cur_y,
         msgWindowW, msgWindowH, brdrW, myBorderPixel, myBgPixel)) == 0) {
      FailToCreateWindowMessage("Setup()", NULL, TRUE);
   }
   if (!noChoiceWindow) {
      cur_y += msgWindowH + (brdrW<<1);
   }
   InitChoice();

   if ((modeWindow=XCreateSimpleWindow(mainDisplay, mainWindow, 0,
         cur_y, modeWindowW, modeWindowH, brdrW, myBorderPixel,
         bg_pixel)) == 0) {
      FailToCreateWindowMessage("Setup()", NULL, TRUE);
   }
   x_offset = (noModeWindow ? 0 : modeWindowW+(brdrW<<1));
   if ((dummyWindow1=XCreateSimpleWindow(mainDisplay, mainWindow, x_offset,
         cur_y, rulerW, rulerW, 1, myBorderPixel, myBgPixel)) == 0) {
      FailToCreateWindowMessage("Setup()", NULL, TRUE);
   }
   if ((hRuleWindow=XCreateSimpleWindow(mainDisplay, mainWindow,
         x_offset+rulerW+(brdrW<<1), cur_y, drawWinW, rulerW-windowPadding,
         1, myBorderPixel, myBgPixel)) == 0) {
      FailToCreateWindowMessage("Setup()", NULL, TRUE);
   }
   if ((colorWindow=XCreateSimpleWindow(mainDisplay, mainWindow,
         x_offset+(brdrW<<2)+rulerW+drawWinW, cur_y, scrollBarW, colorWindowH,
         brdrW, myBorderPixel, bg_pixel)) == 0) {
      FailToCreateWindowMessage("Setup()", NULL, TRUE);
   }
   if ((colorDummyWindow=XCreateSimpleWindow(mainDisplay, mainWindow,
         x_offset+(brdrW<<2)+rulerW+drawWinW, cur_y+colorWindowH+(brdrW<<1),
         scrollBarW, colorDummyWindowH, brdrW, myBorderPixel, bg_pixel)) == 0) {
      FailToCreateWindowMessage("Setup()", NULL, TRUE);
   }
   if ((vSBarWindow=XCreateSimpleWindow(mainDisplay, mainWindow,
         x_offset+(brdrW<<2)+rulerW+drawWinW,
         colorLayers ? cur_y+colorWindowH+colorDummyWindowH+(brdrW<<2) : cur_y,
         scrollBarW, vSBarH, brdrW, myBorderPixel, bg_pixel)) == 0) {
      FailToCreateWindowMessage("Setup()", NULL, TRUE);
   }
   cur_y += rulerW + (brdrW<<1);

   if ((vRuleWindow=XCreateSimpleWindow(mainDisplay, mainWindow, x_offset,
         cur_y, rulerW-windowPadding, drawWinH, 1, myBorderPixel,
         myBgPixel)) == 0) {
      FailToCreateWindowMessage("Setup()", NULL, TRUE);
   }
   if ((drawWindow=XCreateSimpleWindow(mainDisplay, mainWindow,
         x_offset+rulerW+(brdrW<<1), cur_y, drawWinW, drawWinH, 0,
         myBorderPixel, myBgPixel)) == 0) {
      FailToCreateWindowMessage("Setup()", NULL, TRUE);
   }
   cur_y += drawWinH + (brdrW<<1);

   if ((pageWindow=XCreateSimpleWindow(mainDisplay, mainWindow, 0, cur_y,
         pageWindowW, scrollBarW+(brdrW<<1), 0, myBorderPixel,
         bg_pixel)) == 0) {
      FailToCreateWindowMessage("Setup()", NULL, TRUE);
   }
   if ((pageDummyWindow=XCreateSimpleWindow(mainDisplay, mainWindow,
         pageWindowW, cur_y, pageDummyWindowW, scrollBarW, brdrW,
         myBorderPixel, bg_pixel)) == 0) {
      FailToCreateWindowMessage("Setup()", NULL, TRUE);
   }
   if ((hSBarWindow=XCreateSimpleWindow(mainDisplay, mainWindow,
         pageWindowW+pageDummyWindowW+(brdrW<<1), cur_y,
         hSBarW, scrollBarW, brdrW, myBorderPixel, bg_pixel)) == 0) {
      FailToCreateWindowMessage("Setup()", NULL, TRUE);
   }
   if ((dummyWindow2=XCreateSimpleWindow(mainDisplay, mainWindow,
         x_offset+rulerW+drawWinW+(brdrW<<2), cur_y,
         scrollBarW, scrollBarW, brdrW, myBorderPixel, bg_pixel)) == 0) {
      FailToCreateWindowMessage("Setup()", NULL, TRUE);
   }
   cur_y += scrollBarW + (brdrW<<1);

   if ((userRedrawWindow=XCreateSimpleWindow(mainDisplay, mainWindow, 0,
         cur_y, userRedrawWindowW, userRedrawWindowH, brdrW, myBorderPixel,
         bg_pixel)) == 0) {
      FailToCreateWindowMessage("Setup()", NULL, TRUE);
   }
   if ((statusWindow=XCreateSimpleWindow(mainDisplay, mainWindow,
         userRedrawWindowW+(brdrW<<1), cur_y, statusWindowW, statusWindowH,
         brdrW, myBorderPixel, bg_pixel)) == 0) {
      FailToCreateWindowMessage("Setup()", NULL, TRUE);
   }
   cur_y += statusWindowH + (brdrW<<1);

   if ((chatWindow=XCreateSimpleWindow(mainDisplay, mainWindow,
         0, cur_y, chatWindowW, chatWindowH, brdrW, myBorderPixel,
         bg_pixel)) == 0) {
      FailToCreateWindowMessage("Setup()", NULL, TRUE);
   }
   InitScroll();
   CalcStatusSubWinGeom();
   for (i=0; i < MAX_STATUS_BTNS; i++) {
      if ((statusSubWindow[i]=XCreateSimpleWindow(mainDisplay,
            statusWindow, statusSubWindowX[i], statusSubWindowY[i],
            statusSubWindowW[i], statusSubWindowH[i], brdrW,
            myBorderPixel, bg_pixel)) == 0) {
         FailToCreateWindowMessage("Setup()", NULL, TRUE);
      }
   }
   InitTidget();
   InitColorWheel();
   InitInsertSymbol();

   if (!CreateChatSubWindows()) {
      FailToCreateWindowMessage("Setup()", NULL, TRUE);
   }
   if (!noChatWindow) {
      cur_y += chatWindowH + (brdrW<<1);
   }
   if (!noStatusWindow) {
      cur_y += statusWindowH + (brdrW<<1);
   }
   if (canvasWindowOnly) {
      XMoveResizeWindow(mainDisplay, drawWindow, 0, 0, drawWinW, drawWinH);
      XResizeWindow(mainDisplay, mainWindow, mainWinW, mainWinH);
   }

#ifdef NOTR4MODE
   XSetNormalHints(mainDisplay, mainWindow, &sizehints);
#else /* ~NOTR4MODE */
   XSetWMNormalHints(mainDisplay, mainWindow, &sizehints);
#endif /* NOTR4MODE */
   XStoreName(mainDisplay, mainWindow, TOOL_NAME);

   UpdDrawWinBBox();
   InitTitle();
   InitStatus();
   SetCanvasFont();

   InitText();
   InitRemote();
   InitNavigate();
   InitExec();
   InitImageProc();
   InitImport();
   InitHelp();
   InitShape();

   InitExportFilters();
   InitProperties();
   InitCutPaste();
   InitEdit();
   InitMark();
   InitVersion();
   InitMiniLines();

   if (PRTGIF) {
      return;
   }
#ifndef _NO_NKF
   InitNKF();
#endif /* ~_NO_NKF */

   if (cmdLineTgrm2) {
      InitTangram2();
   }
   InitShortCut();
   InitHtml();
   InitRecentFiles();
   InitMeasureTooltip();
   InitPoly();
   InitSpecial();
   InitAttr();
   InitTalkToSelfFiledes();

#ifdef MAPBEFORESELECT
   XMapWindow(mainDisplay, mainWindow);
   XSelectInput(mainDisplay, mainWindow, KeyPressMask | StructureNotifyMask
         | VisibilityChangeMask | FocusChangeMask);
#else
   XSelectInput(mainDisplay, mainWindow, KeyPressMask | StructureNotifyMask
         | VisibilityChangeMask | FocusChangeMask);
   XMapWindow(mainDisplay, mainWindow);
#endif
   XDefineCursor(mainDisplay, mainWindow, defaultCursor);

   RegisterWM_DELETE_WINDOW(mainWindow);

#ifdef MAPBEFORESELECT
   if (canvasWindowOnly) {
      XMapWindow(mainDisplay, drawWindow); 
      XSelectInput(mainDisplay, drawWindow, ButtonReleaseMask |
            ButtonPressMask | PointerMotionMask | KeyPressMask |
            KeyReleaseMask | ExposureMask | EnterWindowMask | LeaveWindowMask |
            PropertyChangeMask);
   } else {
      XMapWindow(mainDisplay, titleWindow);
      XSelectInput(mainDisplay, titleWindow, ExposureMask |
            EnterWindowMask | LeaveWindowMask | ButtonPressMask |
            PointerMotionMask);

      if (menubarWindow != None) XMapWindow(mainDisplay, menubarWindow);
      XSelectInput(mainDisplay, menubarWindow, ExposureMask |
            EnterWindowMask | LeaveWindowMask | ButtonPressMask |
            PointerMotionMask);

      if (!noChoiceWindow) XMapWindow(mainDisplay, msgWindow);
      XSelectInput(mainDisplay, msgWindow, ButtonPressMask |
            ButtonReleaseMask | PointerMotionMask | ExposureMask |
            EnterWindowMask);

      if (!noChoiceWindow) XMapWindow(mainDisplay, choiceWindow);
      XSelectInput(mainDisplay, choiceWindow,
            ButtonReleaseMask | ButtonPressMask | ExposureMask |
            PointerMotionMask | EnterWindowMask | LeaveWindowMask);

      XMapWindow(mainDisplay, hRuleWindow);
      XSelectInput(mainDisplay, hRuleWindow,
            ButtonPressMask | ExposureMask | EnterWindowMask);
      XMapWindow(mainDisplay, vRuleWindow);
      XSelectInput(mainDisplay, vRuleWindow,
            ButtonPressMask | ExposureMask | EnterWindowMask);

      XMapWindow(mainDisplay, drawWindow); 
      XSelectInput(mainDisplay, drawWindow, ButtonReleaseMask |
            ButtonPressMask | PointerMotionMask | KeyPressMask |
            KeyReleaseMask | ExposureMask | EnterWindowMask | LeaveWindowMask |
            PropertyChangeMask);

      if (colorLayers) {
         XMapWindow(mainDisplay, colorWindow);
         XSelectInput(mainDisplay, colorWindow, ButtonPressMask |
               PointerMotionMask | ExposureMask | EnterWindowMask);
         XMapWindow(mainDisplay, colorDummyWindow);
         XSelectInput(mainDisplay, colorDummyWindow, ButtonPressMask |
               ExposureMask | EnterWindowMask);
      } else {
         XSelectInput(mainDisplay, colorWindow, ButtonPressMask |
               PointerMotionMask | ExposureMask | EnterWindowMask);
         XSelectInput(mainDisplay, colorDummyWindow, ButtonPressMask |
               ExposureMask | EnterWindowMask);
      }
      XMapWindow(mainDisplay, vSBarWindow);
      XSelectInput(mainDisplay, vSBarWindow, ButtonPressMask |
            ExposureMask | EnterWindowMask | PointerMotionMask);

      XMapWindow(mainDisplay, pageWindow);
      XSelectInput(mainDisplay, pageWindow, ButtonPressMask |
            PointerMotionMask | ExposureMask | EnterWindowMask);
      XMapWindow(mainDisplay, pageDummyWindow);
      XSelectInput(mainDisplay, pageDummyWindow, ButtonPressMask |
            ExposureMask | EnterWindowMask);
      XMapWindow(mainDisplay, hSBarWindow);
      XSelectInput(mainDisplay, hSBarWindow, ButtonPressMask |
            ExposureMask | EnterWindowMask | PointerMotionMask);
      XMapWindow(mainDisplay, dummyWindow1);
      XSelectInput(mainDisplay, dummyWindow1, ButtonPressMask | ExposureMask |
            EnterWindowMask | LeaveWindowMask);
      XMapWindow(mainDisplay, dummyWindow2);
      XSelectInput(mainDisplay, dummyWindow2, ExposureMask | EnterWindowMask);

      if (!noChatWindow) {
         XMapWindow(mainDisplay, chatWindow);
      }
      MapChatSubWindows();
      XSelectInput(mainDisplay, chatWindow,
            KeyPressMask | ButtonPressMask | ExposureMask |
            StructureNotifyMask | EnterWindowMask | LeaveWindowMask);

      if (!noStatusWindow) {
         XMapWindow(mainDisplay, userRedrawWindow);
         XSelectInput(mainDisplay, userRedrawWindow, ButtonPressMask |
               ExposureMask | EnterWindowMask);
         XMapWindow(mainDisplay, statusWindow);
         XSelectInput(mainDisplay, statusWindow, ExposureMask |
               EnterWindowMask);
         for (i=0; i < MAX_STATUS_BTNS; i++) {
            XMapWindow(mainDisplay, statusSubWindow[i]);
         }
      } else {
         XSelectInput(mainDisplay, userRedrawWindow, ButtonPressMask |
               ExposureMask | EnterWindowMask);
         XSelectInput(mainDisplay, statusWindow, ExposureMask |
               EnterWindowMask);
      }
      if (!noModeWindow) XMapWindow(mainDisplay, modeWindow);
      XSelectInput(mainDisplay, modeWindow, ButtonPressMask | ExposureMask |
            PointerMotionMask | EnterWindowMask | LeaveWindowMask);
   }
#else /* ~MAPBEFORESELECT */
   if (canvasWindowOnly) {
      XSelectInput(mainDisplay, drawWindow, ButtonReleaseMask |
            ButtonPressMask | PointerMotionMask | KeyPressMask |
            KeyReleaseMask | ExposureMask | EnterWindowMask | LeaveWindowMask |
            PropertyChangeMask);
      XMapWindow(mainDisplay, drawWindow); 
   } else {
      XSelectInput(mainDisplay, titleWindow, ExposureMask |
            EnterWindowMask | LeaveWindowMask | ButtonPressMask |
            PointerMotionMask);
      XMapWindow(mainDisplay, titleWindow);

      XSelectInput(mainDisplay, menubarWindow, ExposureMask |
            EnterWindowMask | LeaveWindowMask | ButtonPressMask |
            PointerMotionMask);
      if (menubarWindow != None) XMapWindow(mainDisplay, menubarWindow);

      XSelectInput(mainDisplay, msgWindow, ButtonPressMask |
            ButtonReleaseMask | PointerMotionMask | ExposureMask |
            EnterWindowMask);
      if (!noChoiceWindow) XMapWindow(mainDisplay, msgWindow);

      XSelectInput(mainDisplay, choiceWindow,
            ButtonReleaseMask | ButtonPressMask | ExposureMask |
            PointerMotionMask | EnterWindowMask | LeaveWindowMask);
      if (!noChoiceWindow) XMapWindow(mainDisplay, choiceWindow);

      XSelectInput(mainDisplay, hRuleWindow,
            ButtonPressMask | ButtonReleaseMask | PointerMotionMask |
            ExposureMask | EnterWindowMask);
      XMapWindow(mainDisplay, hRuleWindow);
      XSelectInput(mainDisplay, vRuleWindow,
            ButtonPressMask | ButtonReleaseMask | PointerMotionMask |
            ExposureMask | EnterWindowMask);
      XMapWindow(mainDisplay, vRuleWindow);

      XSelectInput(mainDisplay, drawWindow, ButtonReleaseMask |
            ButtonPressMask | PointerMotionMask | KeyPressMask |
            KeyReleaseMask | ExposureMask | EnterWindowMask | LeaveWindowMask |
            PropertyChangeMask);
      XMapWindow(mainDisplay, drawWindow); 

      if (colorLayers) {
         XSelectInput(mainDisplay, colorWindow, ButtonPressMask |
               PointerMotionMask | ExposureMask | EnterWindowMask);
         XMapWindow(mainDisplay, colorWindow);
         XSelectInput(mainDisplay, colorDummyWindow, ButtonPressMask |
               ExposureMask | EnterWindowMask);
         XMapWindow(mainDisplay, colorDummyWindow);
      } else {
         XSelectInput(mainDisplay, colorWindow, ButtonPressMask |
               PointerMotionMask | ExposureMask | EnterWindowMask);
         XSelectInput(mainDisplay, colorDummyWindow, ButtonPressMask |
               ExposureMask | EnterWindowMask);
      }
      XSelectInput(mainDisplay, vSBarWindow, ButtonPressMask |
            ExposureMask | EnterWindowMask | PointerMotionMask);
      XMapWindow(mainDisplay, vSBarWindow);

      XSelectInput(mainDisplay, pageWindow, ButtonPressMask |
            PointerMotionMask | ExposureMask | EnterWindowMask);
      XMapWindow(mainDisplay, pageWindow);
      XSelectInput(mainDisplay, pageDummyWindow, ButtonPressMask |
            ExposureMask | EnterWindowMask);
      XMapWindow(mainDisplay, pageDummyWindow);
      XSelectInput(mainDisplay, hSBarWindow, ButtonPressMask |
            ExposureMask | EnterWindowMask | PointerMotionMask);
      XMapWindow(mainDisplay, hSBarWindow);
      XSelectInput(mainDisplay, dummyWindow1, ButtonPressMask | ExposureMask |
            EnterWindowMask | LeaveWindowMask);
      XMapWindow(mainDisplay, dummyWindow1);
      XSelectInput(mainDisplay, dummyWindow2, ExposureMask | EnterWindowMask);
      XMapWindow(mainDisplay, dummyWindow2);

      XSelectInput(mainDisplay, chatWindow,
            KeyPressMask | ButtonPressMask | ExposureMask |
            StructureNotifyMask | EnterWindowMask | LeaveWindowMask);
      if (!noChatWindow) {
         XMapWindow(mainDisplay, chatWindow);
      }
      MapChatSubWindows();

      if (!noStatusWindow) {
         XSelectInput(mainDisplay, userRedrawWindow, ButtonPressMask |
               ExposureMask | EnterWindowMask);
         XMapWindow(mainDisplay, userRedrawWindow);
         XSelectInput(mainDisplay, statusWindow, ExposureMask |
               EnterWindowMask);
         XMapWindow(mainDisplay, statusWindow);
         for (i=0; i < MAX_STATUS_BTNS; i++) {
            XMapWindow(mainDisplay, statusSubWindow[i]);
         }
      } else {
         XSelectInput(mainDisplay, userRedrawWindow, ButtonPressMask |
               ExposureMask | EnterWindowMask);
         XSelectInput(mainDisplay, statusWindow, ExposureMask |
               EnterWindowMask);
      }
      XSelectInput(mainDisplay, modeWindow, ButtonPressMask | ExposureMask |
            PointerMotionMask | EnterWindowMask | LeaveWindowMask);
      if (!noModeWindow) XMapWindow(mainDisplay, modeWindow);
   }
#endif

   wmhints.flags = InputHint;
   wmhints.input = True;
   wmhints.icon_pixmap = None;

   if (iconWindowCreated) {
      unsigned int dummy_icon_w, dummy_icon_h;

      sizehints.flags = PSize | PMinSize | PMaxSize;
      sizehints.width = sizehints.min_width = sizehints.max_width =
            iconWindowW+(brdrW<<1);
      sizehints.height = sizehints.min_height = sizehints.max_height =
            iconWindowH+(brdrW<<1);

      wmhints.flags |= IconWindowHint;
      wmhints.icon_window = iconBaseWindow;

      if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"IconGeometry")) != NULL) {
         bitmask = XParseGeometry(c_ptr, &(sizehints.x), &(sizehints.y),
               &dummy_icon_w, &dummy_icon_h);
         if ((bitmask & XValue) && (bitmask & YValue)) {
            if (bitmask & XNegative) sizehints.x += DisplayWidth (mainDisplay,
                  mainScreen) - iconWindowW - (brdrW<<1) - 1;
            if (bitmask & YNegative) sizehints.y += DisplayHeight(mainDisplay,
                  mainScreen) - iconWindowH - (brdrW<<1) - 1;

            sizehints.flags |= USPosition;

            wmhints.flags |= IconPositionHint;
            wmhints.icon_x = sizehints.x;
            wmhints.icon_y = sizehints.y;
         } else {
            fprintf(stderr, TgLoadString(STID_INVALID_XDEF),
                  TOOL_NAME, "IconGeometry", c_ptr);
            fprintf(stderr, "\n");
         }
      }
#ifdef NOTR4MODE
      XSetNormalHints(mainDisplay, iconBaseWindow, &sizehints);
#else /* ~NOTR4MODE */
      XSetWMNormalHints(mainDisplay, iconBaseWindow, &sizehints);
#endif /* NOTR4MODE */
   } else {
      if (((c_ptr=XGetDefault(mainDisplay, TOOL_NAME, "UseWMIconPixmap")) ==
            NULL) || UtilStrICmp(c_ptr, "false") != 0) {
         wmhints.icon_mask = None;
         wmhints.initial_state = NormalState;
         if (!GetWMIconPixmap(&wmIconPixmap, &wmIconPixmapMask)) {
            wmhints.icon_pixmap = wmIconPixmap =
                  XCreateBitmapFromData(mainDisplay, mainWindow,
                  (char*)tgificon_bits, tgificon_width, tgificon_height);
         } else {
            wmhints.icon_pixmap = wmIconPixmap;
            if (wmIconPixmapMask != None) {
               wmhints.icon_mask = wmIconPixmapMask;
               wmhints.flags |= IconMaskHint;
            }
         }
         wmhints.flags |= IconPixmapHint | StateHint;
         /* FlushUndoBuffer(); */
      }
   }
   XSetWMHints(mainDisplay, mainWindow, &wmhints);

   wmhints.flags = InputHint;
   wmhints.input = True;
   XSetWMHints(mainDisplay, drawWindow, &wmhints);

   memset(&classhints, 0, sizeof(XClassHint));
   classhints.res_name = TOOL_NAME;
   classhints.res_class = TOOL_NAME;
#ifdef NOTR4MODE
   XSetClassHint(mainDisplay, mainWindow, &classhints);
#else /* ~NOTR4MODE */
   XSetWMProperties(mainDisplay, mainWindow, NULL, NULL, NULL, 0, NULL, NULL,
         &classhints);
#endif /* NOTR4MODE */
}

void CleanUpResiduals()
{
   if (wmIconPixmap != None) {
      XFreePixmap(mainDisplay, wmIconPixmap);
      wmIconPixmap = None;
   }
   if (wmIconPixmapMask != None) {
      XFreePixmap(mainDisplay, wmIconPixmapMask);
      wmIconPixmapMask = None;
   }
   CleanUpPaperSize();
   if (displayName != NULL) {
      UtilFree(displayName);
      displayName = NULL;
   }
   if (btn1_bits == NULL) { }
}

int TieLooseEnds()
   /* returns TRUE if something got created */
   /* returns FALSE otherwise */
{
   if (curChoice == DRAWTEXT) return CreateTextObj(TRUE, TRUE);
   return FALSE;
}

void MakeQuiescent()
{
   curChoiceBeforeMakeQuiescent = curChoice;
   TieLooseEnds();
   SetCurChoice(NOTHING);
   if (topSel != NULL) {
      HighLightReverse();
      RemoveAllSel();
   }
   UpdSelBBox();
}

void SetFileModified(modified)
   int modified;
{
   if (modified != fileModified) {
      fileModified = modified;
      if (fileModified && gstWBInfo.do_whiteboard) {
         SetUnSavableFile(TRUE);
      }
      if (!PRTGIF) {
         RedrawTitleWindow();
      }
   }
}
