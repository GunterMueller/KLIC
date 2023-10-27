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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/const.h,v 1.24 2011/05/16 16:21:57 william Exp $
 */

#ifndef _TGIF_CONST_H_
#define _TGIF_CONST_H_

#if __STDC__ || defined(__cplusplus) || defined(c_plusplus)
/* ANSI || C++ */
#ifdef _NO_PROTO
   /* just in case ANSI or C++ doesn't handle function prototypes well */
#define ARGS_DECL(args) ()
#undef ARGS_DECL_USED
#else /* ~_NO_PROTO */
#define ARGS_DECL(args) args
#undef ARGS_DECL_USED
#define ARGS_DECL_USED 1
#endif /* _NO_PROTO */
#else /* ~(__STDC__ || defined(__cplusplus) || defined(c_plusplus)) */
#define ARGS_DECL(args) ()
#undef ARGS_DECL_USED
#endif /* __STDC__ || defined(__cplusplus) || defined(c_plusplus) */

#include "tgif_dbg.h"

#ifndef _NO_LOCALE_SUPPORT
#ifdef ENABLE_NLS
#define _(text)  gettext(text)
#define N_(text) text
#else /* ~ENABLE_NLS */
#define _(text)  text
#define N_(text) text
#endif /* ENABLE_NLS */
#else /* _NO_LOCALE_SUPPORT */
#define _(text)  text
#define N_(text) text
#endif /* ~_NO_LOCALE_SUPPORT */

#define TOOL_NAME "Tgif"

#ifndef NULL
#define NULL 0
#endif /* ~NULL */

#ifndef TRUE
#define FALSE 0
#define TRUE 1
#endif /* ~TRUE */

#define INVALID (-1)
#undef BAD
#define BAD (-2)

#ifdef WIN32
#define DIR_SEP '\\'
#define DIR_SEP_STR "\\"
#else /* ~WIN32 */
#define DIR_SEP '/'
#define DIR_SEP_STR "/"
#endif /* WIN32 */

#define SINGLECOLOR (FALSE)
#define MULTICOLOR (TRUE)

#define BUTTONSMASK ((Button1Mask)|(Button2Mask)|(Button3Mask))

#ifndef max
#define max(A,B) (((A)>(B)) ? (A) : (B))
#define min(A,B) (((A)>(B)) ? (B) : (A))
#endif /* ~max */

#ifndef round
#define round(X) (((X) >= 0) ? (int)((X)+0.5) : (int)((X)-0.5))
#endif /* ~round */

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif /* ~M_PI */

#define INT_TOL (1.0e-5)
#define EQ_TOL (1.0e-8)

#ifndef XK_KP_Left
#define XK_KP_Left      0xFF96
#define XK_KP_Up        0xFF97
#define XK_KP_Right     0xFF98
#define XK_KP_Down      0xFF99
#endif /* ~XK_KP_LEFT */

#ifndef XK_KP_Delete
#define XK_KP_Insert    0xFF9E
#define XK_KP_Delete    0xFF9F
#endif /* ~XK_KP_Delete */

#ifndef XK_Page_Up
#define XK_Page_Up      0xFF55
#define XK_Page_Down    0xFF56
#endif /* ~XK_Page_Up */

#ifndef XK_KP_Page_Up
#define XK_KP_Page_Up   0xFF9A
#define XK_KP_Page_Down 0xFF9B
#endif /* ~XK_KP_Page_Up */

#ifndef XK_KP_Home
#define XK_KP_Home      0xFF95
#define XK_KP_End       0xFF9C
#endif /* ~XK_KP_Home */

#ifndef XK_Home
#define XK_Home         0xFF50
#define XK_End          0xFF57
#endif /* ~XK_Home */

#define MAX_ZOOMED_IN 3
#define HALF_W(w) (((w)&0x1) ? ((w)>>1)+1 : ((w)>>1))
#define ZOOMED_HALF_W(w) ((zoomedIn) ? ((w)<<(zoomScale-1)) : \
      ((((w)>>(zoomScale))&0x1) ? (((w)>>(zoomScale+1))+1) : \
      ((w)>>(zoomScale+1))))

#define OFFSET_X(AbsX) ((zoomedIn) ? (((AbsX)-drawOrigX)<<zoomScale) : \
                                     (((AbsX)-drawOrigX)>>zoomScale))
#define OFFSET_Y(AbsY) ((zoomedIn) ? (((AbsY)-drawOrigY)<<zoomScale) : \
                                     (((AbsY)-drawOrigY)>>zoomScale))

#define OFFSET_DOUBLE_X(AbsX) \
      ((zoomedIn) ? \
      (((AbsX)-((double)drawOrigX))*((double)(1<<zoomScale))) : \
      (((AbsX)-((double)drawOrigX))/((double)(1<<zoomScale))))
#define OFFSET_DOUBLE_Y(AbsY) \
      ((zoomedIn) ? \
      (((AbsY)-((double)drawOrigY))*((double)(1<<zoomScale))) : \
      (((AbsY)-((double)drawOrigY))/((double)(1<<zoomScale))))

#define ABS_X(OffsetX) ((zoomedIn) ? (((OffsetX)>>zoomScale)+drawOrigX) : \
                                     (((OffsetX)<<zoomScale)+drawOrigX))
#define ABS_Y(OffsetY) ((zoomedIn) ? (((OffsetY)>>zoomScale)+drawOrigY) : \
                                     (((OffsetY)<<zoomScale)+drawOrigY))

#define ZOOMED_SIZE(AbsSize) ((zoomedIn) ? ((AbsSize)<<zoomScale) : \
                                           ((AbsSize)>>zoomScale))

#define ZOOMED_DOUBLE_SIZE(AbsSize) ((zoomedIn) ? \
                                    ((AbsSize)*((double)(1<<zoomScale))) : \
                                    ((AbsSize)/((double)(1<<zoomScale))))

#define GRID_ZOOMED_SIZE(AbsSize) ((zoomedIn) ? (AbsSize) : \
                                                ((AbsSize)>>zoomScale))

#define ABS_SIZE(ZoomedSize) ((zoomedIn) ? ((ZoomedSize)>>zoomScale) : \
                                           ((ZoomedSize)<<zoomScale))
#define GRID_ABS_SIZE(ZoomedSize) ((zoomedIn) ? (ZoomedSize) : \
                                                ((ZoomedSize)<<zoomScale))

#define SetRecVals(R,X,Y,W,H) ((R).x=(X),(R).y=(Y),(R).width=(W),(R).height=(H))

#define MARK(W,GC,X,Y) XFillRectangle(mainDisplay,W,GC, \
      (X)-handleSize,(Y)-handleSize,(handleSize<<1)+1,(handleSize<<1)+1)
#define MARKHR(W,GC,X,Y) XDrawRectangle(mainDisplay,W,GC, \
      (X)-handleSize,(Y)-handleSize,(handleSize<<1),(handleSize<<1))
#define MARKV(W,GC,X,Y) XDrawLine(mainDisplay,W,GC, \
      (X)-(handleSize+1),(Y),(X)+(handleSize+1),(Y)); \
      XDrawLine(mainDisplay,W,GC,(X),(Y)-(handleSize+1),(X),(Y)+(handleSize+1))
#define MARKO(W,GC,X,Y) XFillArc(mainDisplay,W,GC, \
      (X)-handleSize,(Y)-handleSize,(handleSize<<1)+1,(handleSize<<1)+1, \
      0,(360<<6))
#define MARKHO(W,GC,X,Y) XDrawArc(mainDisplay,W,GC, \
      (X)-handleSize,(Y)-handleSize,(handleSize<<1),(handleSize<<1), \
      0,(360<<6))
#define PtInMark(PtX,PtY,MarkX,MarkY) ((PtX) >= (MarkX)-handleSize && \
      (PtY) >= (MarkY)-handleSize && (PtX) <= (MarkX)+handleSize && \
      (PtY) <= (MarkY)+handleSize)
#define MyDashedLine(W,GC,V,N) XDrawLines(mainDisplay,W,GC,V,N,CoordModeOrigin)

#define GETINT(category,val,name) ScanValue("%d", &(val), name, category)
#define GETFLT(category,val,name) ScanValue("%f", &(val), name, category)
#define GETDBL(category,val,name) ScanValue("%lg", &(val), name, category)
#define GETSTR(category,val,name) ScanValue("%s", (val), name, category)
#define GETDYN(category,val,name) ScanDynStrValue(&(val), name, category)

#define GetPinObj(ObjPtr) ((ObjPtr)->detail.r->pin_connected ? \
      (ObjPtr)->detail.r->last : (ObjPtr)->detail.r->first)
#define GetTextObjFirstStrSeg(ObjPtr) \
      ((ObjPtr)->detail.t->minilines.first->first_block->seg)
#define GetTextPtrFirstStrSeg(TextPtr) \
      ((TextPtr)->minilines.first->first_block->seg)

#define MAXSTRING 256
#define MAXPATHLENGTH 256

#define COLORSTRLEN 40

/* geometry stuff */

#define MENU_IMAGE_W 64
#define MENU_IMAGE_H 20
#define CHOICE_IMAGE_W 32
#define CHOICE_IMAGE_H 20
#define SCROLLBAR_W 16
#define RULER_W 20
#define BRDR_W 1
#define WINDOW_PADDING 2

/* object types */

#define OBJ_POLY 0
#define OBJ_BOX 1
#define OBJ_OVAL 2
#define OBJ_TEXT 3
#define OBJ_POLYGON 4
#define OBJ_GROUP 5
#define OBJ_SYM 6
#define OBJ_ICON 7
#define OBJ_ARC 8
#define OBJ_RCBOX 9
#define OBJ_XBM 10
#define OBJ_XPM 11
#define OBJ_PIN 12 /* Please note that pins are not implemented yet! */
#define OBJ_SS 13 /* This is reserved */

/* drawing modes */

#define NOTHING 0
#define DRAWTEXT 1
#define DRAWBOX 2
#define DRAWCORNEROVAL 3
#define DRAWCENTEROVAL 4
#define DRAWEDGECIRCLE 5
#define DRAWPOLY 6
#define DRAWPOLYGON 7
#define DRAWARC 8
#define DRAWEDGEARC 9
#define DRAWRCBOX 10
#define FREEHAND 11
#define VERTEXMODE 12
#define ROTATEMODE 13

#define MAXCHOICES 14

/* stipple patterns */

#define NONEPAT 0
#define SOLIDPAT 1
#define BACKPAT 2
#define SCROLLPAT 7
#define MAXPATTERNS 32

#define NO_TRANSPAT_MODE 0
#define TRANSPAT_MODE 1

#define MAXTRANSPATMODES 2

/* line stuff */

#define LINE_THIN 0
#define LINE_MEDIUM 1
#define LINE_THICK 2
#define LINE_CURVED 3 /* compatibility hack for fileVersion <= 3 */

#define MAXLINEWIDTHS 7

#define LT_STRAIGHT 0
#define LT_SPLINE 1
#define LT_INTSPLINE 2
#define LT_STRUCT_SPLINE 3

#define MAXLINETYPES 4

#define LS_PLAIN 0
#define LS_RIGHT 1
#define LS_LEFT 2
#define LS_DOUBLE 3

#define MAXLINESTYLES 4

#define MAXDASHES 9

#define NOCONT (FALSE)
#define CONT (TRUE)

#define NORB (FALSE)
#define RB (TRUE)

#define CHANGE_WIDTH 0x1
#define CHANGE_AW    0x2
#define CHANGE_AH    0x4

#define CHANGE_LINE_ALL (CHANGE_WIDTH|CHANGE_AW|CHANGE_AH)

#define LCAP_BUTT 0
#define LCAP_ROUND 1
#define LCAP_SQUARE 2

#define MAXLINECAPS 3

#define ASTY_MASK_COMPAT  0x0001
#define ASTY_MASK_FILL    0x0002
#define ASTY_MASK_HALVES  0x000c
#define ASTY_MASK_SHAPES  0x0010

#define ASTY_COMPAT       0x0 /* ASTY_MASK_COMPAT */
#define ASTY_FLEX         0x1 /* ASTY_MASK_COMPAT */

#define ASTY_SOLIDPAT     0x0 /* ASTY_MASK_FILL */
#define ASTY_BACKPAT      0x2 /* ASTY_MASK_FILL */

#define ASTY_LF_HALF_ONLY 0x4 /* ASTY_MASK_HALVES */
#define ASTY_RT_HALF_ONLY 0x8 /* ASTY_MASK_HALVES */

#define ASTY_CIRCLE       1 /* not used */
#define ASTY_BOX          2 /* not used */

/* font stuff */

#define FONT_TIM 0
#define FONT_COU 1
#define FONT_HEL 2
#define FONT_CEN 3
#define FONT_SYM 4

#define MAXFONTS 5

#define STYLE_NR 0
#define STYLE_BR 1
#define STYLE_NI 2
#define STYLE_BI 3

#define MAXFONTSTYLES 4

#define FONT_DPI_75 0
#define FONT_DPI_100 1

#define MAXFONTDPIS 2

#define MAXFONTSIZES 6

#define JUST_L 0
#define JUST_C 1
#define JUST_R 2

#define MAXJUSTS 3

#define PUSH_FONT 0
#define PUSH_SZ_UNIT 1
#define PUSH_STYLE 2
#define PUSH_JUST 3
#define PUSH_ROTATE 4
#define PUSH_PEN 5
#define PUSH_FILL 6
#define PUSH_TRANSPAT 7
#define PUSH_VSPACE 8
#define PUSH_COLORINDEX 9
#define PUSH_UNDERLINE_ON 10
#define PUSH_UNDERLINE_Y_OFFSET 11
#define PUSH_DOUBLEBYTE 12
#define PUSH_OVERLINE_ON 13
#define PUSH_OVERLINE_Y_OFFSET 14

#define NUM_SZ_UNIT_PER_FONT_SIZE 5760
#define NUM_SZ_UNIT_PER_POINT_SIZE 10240

#define FontSizeToSzUnit(font_sz) ((font_sz)*NUM_SZ_UNIT_PER_FONT_SIZE)
#define PointSizeToSzUnit(point_sz) ((point_sz)*NUM_SZ_UNIT_PER_POINT_SIZE)

/* alignment */

#define ALIGN_N 0

#define ALIGN_L 1
#define ALIGN_C 2
#define ALIGN_R 3

#define ALIGN_T 1
#define ALIGN_M 2
#define ALIGN_B 3

#define ALIGN_S 4

#define MAXALIGNS 5

#define ALIGN_SHIFT (MAXALIGNS)

#define ALIGN_NN ((ALIGN_N<<ALIGN_SHIFT)|ALIGN_N)
#define ALIGN_NT ((ALIGN_N<<ALIGN_SHIFT)|ALIGN_T)
#define ALIGN_NM ((ALIGN_N<<ALIGN_SHIFT)|ALIGN_M)
#define ALIGN_NB ((ALIGN_N<<ALIGN_SHIFT)|ALIGN_B)
#define ALIGN_NS ((ALIGN_N<<ALIGN_SHIFT)|ALIGN_S)
#define ALIGN_LN ((ALIGN_L<<ALIGN_SHIFT)|ALIGN_N)
#define ALIGN_LT ((ALIGN_L<<ALIGN_SHIFT)|ALIGN_T)
#define ALIGN_LM ((ALIGN_L<<ALIGN_SHIFT)|ALIGN_M)
#define ALIGN_LB ((ALIGN_L<<ALIGN_SHIFT)|ALIGN_B)
#define ALIGN_LS ((ALIGN_L<<ALIGN_SHIFT)|ALIGN_S)
#define ALIGN_CN ((ALIGN_C<<ALIGN_SHIFT)|ALIGN_N)
#define ALIGN_CT ((ALIGN_C<<ALIGN_SHIFT)|ALIGN_T)
#define ALIGN_CM ((ALIGN_C<<ALIGN_SHIFT)|ALIGN_M)
#define ALIGN_CB ((ALIGN_C<<ALIGN_SHIFT)|ALIGN_B)
#define ALIGN_CS ((ALIGN_C<<ALIGN_SHIFT)|ALIGN_S)
#define ALIGN_RN ((ALIGN_R<<ALIGN_SHIFT)|ALIGN_N)
#define ALIGN_RT ((ALIGN_R<<ALIGN_SHIFT)|ALIGN_T)
#define ALIGN_RM ((ALIGN_R<<ALIGN_SHIFT)|ALIGN_M)
#define ALIGN_RB ((ALIGN_R<<ALIGN_SHIFT)|ALIGN_B)
#define ALIGN_RS ((ALIGN_R<<ALIGN_SHIFT)|ALIGN_S)
#define ALIGN_SN ((ALIGN_S<<ALIGN_SHIFT)|ALIGN_N)
#define ALIGN_ST ((ALIGN_S<<ALIGN_SHIFT)|ALIGN_T)
#define ALIGN_SM ((ALIGN_S<<ALIGN_SHIFT)|ALIGN_M)
#define ALIGN_SB ((ALIGN_S<<ALIGN_SHIFT)|ALIGN_B)
#define ALIGN_SS ((ALIGN_S<<ALIGN_SHIFT)|ALIGN_S)

#define ALIGN_OBJS_DIRECT 0
#define ALIGN_TO_GRID_DIRECT 1
#define ALIGN_TO_PAGE_DIRECT 2

/* color */

#define MAXCOLORS 11

/* button stuff */

#define BUTTON_INVERT 0
#define BUTTON_NORMAL 1

/* page style */

#define PORTRAIT 0
#define LANDSCAPE 1
#define HIGHPORT 2 /* obsolete */
#define HIGHLAND 3 /* obsolete */
#define SLIDEPORT 4 /* obsolete */
#define SLIDELAND 5 /* obsolete */

#define MAXPAGESTYLES 2

/* where to print */

#define PRINTER 0
#define LATEX_FIG 1
#define PS_FILE 2
#define XBM_FILE 3
#define TEXT_FILE 4
#define EPSI_FILE 5
#define GIF_FILE 6
#define HTML_FILE 7
#define PDF_FILE 8
#define TIFFEPSI_FILE 9
#define PNG_FILE 10
#define JPEG_FILE 11
#define PPM_FILE 12
#define NETLIST_FILE 13
#define SVG_FILE 14

#define MAXDEFWHERETOPRINT 15

/* measurement */

#define TIK_PER_PIXEL_SHIFTS 12
#define TIK_PER_PIXEL (1<<TIK_PER_PIXEL_SHIFTS)

#define ENGLISH_GRID 0
#define METRIC_GRID 1

#define PIX_PER_INCH 128
#define ONE_INCH (PIX_PER_INCH)
#define HALF_INCH (PIX_PER_INCH/2)
#define QUARTER_INCH (PIX_PER_INCH/4)
#define EIGHTH_INCH (PIX_PER_INCH/8)

#define DEFAULT_ENGLISH_GRID (EIGHTH_INCH)

#define PIX_PER_MM 5
#define ONE_MM (PIX_PER_MM)
#define TWO_MM (PIX_PER_MM*2)
#define FIVE_MM (PIX_PER_MM*5)
#define ONE_CM (PIX_PER_MM*10)

#define DEFAULT_METRIC_GRID (TWO_MM)

/* rotation -- clockwise */

#define ROTATE0 0
#define ROTATE90 1
#define ROTATE180 2
#define ROTATE270 3

#define CLOCKWISE90 (90<<6)
#define COUNTER90 ((-90)<<6)
#define CLOCKWISE180 (180<<6)
#define COUNTER180 ((-180)<<6)
#define CLOCKWISE270 (270<<6)
#define COUNTER270 ((-270)<<6)

/* flipping */

#define NO_FLIP 0
#define HORI_ODD (1<<0)
#define HORI_EVEN (1<<1)
#define VERT_ODD (1<<2)
#define VERT_EVEN (1<<3)

/* arc */

#define ARC_CCW 0 /* counter-clock-wise */
#define ARC_CW 1 /* clock-wise */

/* main menu */

#define MENU_FILE 0
#define MENU_EDIT 1
#define MENU_LAYOUT 2
#define MENU_ARRANGE 3
#define MENU_PROPERTIES 4
#define MENU_MOVEMODE 5
#define MENU_STACKEDPAGE 6
#define MENU_TILEDPAGE 7
#define MENU_PAGE 8
#define MENU_PAGELAYOUT 9
#define MENU_HORIALIGN 10
#define MENU_VERTALIGN 11
#define MENU_FONT 12
#define MENU_STYLE 13
#define MENU_SIZE 14
#define MENU_SHAPE 15
#define MENU_STRETCHTEXT 16
#define MENU_LINEDASH 17
#define MENU_LINESTYLE 18
#define MENU_LINETYPE 19
#define MENU_LINEWIDTH 20
#define MENU_FILL 21
#define MENU_PEN 22
#define MENU_TRANSPAT 23
#define MENU_COLOR 24
#define MENU_IMAGEPROC 25
#define MENU_NAVIGATE 26
#define MENU_SPECIAL 27
#define MENU_HELP 28
#define MENU_MODE 29
#define MENU_TANGRAM2 30

#define MAXMENUS 31
#define MENU_MAIN 31 /* mainMenu is treated differently */

/* file menu */

#define FILE_NEW 0
#define FILE_OPEN 1
#define FILE_SAVE 2
#define FILE_SAVENEW 3
#define FILE_IMPORT 4
#define FILE_IMPORTXBM 5
#define FILE_IMPORTXPM 6
#define FILE_IMPORTEPS 7
#define FILE_IMPORTGIF 8
#define FILE_IMPORTOTHERS 9
#define FILE_EMBEDEPS 10
#define FILE_BROWSEXBM 11
#define FILE_BROWSEXPM 12
#define FILE_BROWSEOTHERS 13
#define FILE_DOMAIN 14
#define FILE_DUMP 15
#define FILE_USR_DUMP 16
#define FILE_DUMPSELECTED 17
#define FILE_PRINTONE 18
#define FILE_SETEXPORTTRIM 19
#define FILE_INPUT_POLY 20
#define FILE_INPUT_POLYGON 21
#define FILE_SET_TEMPLATE 22
#define FILE_SOLVE 23
#define FILE_SIMULATE 24
#define FILE_PROBE 25
#define FILE_ANIMATE 26
#define FILE_ESCAPE 27
#define FILE_SAVESELAS 28
#define FILE_SAVESYMINLIB 29
#define FILE_QUIT 30

#define FILEMENUENTRIES 31

/* move mode stuff */

#define CONST_MOVE 0
#define UNCONST_MOVE 1

#define MAXMOVEMODES 2

/* stretchable text stuff */

#define NO_STRETCHABLE_TEXT 0
#define STRETCHABLE_TEXT 1

#define MAXSTRETCHABLEMODES 2

/* arrange menu */

#define ABUT_HORIZONTAL 0
#define ABUT_VERTICAL 1

/* nagivate menu */

#define NAVIGATE_BACK 0
#define NAVIGATE_FORWARD 1
#define NAVIGATE_REFRESH 2
#define NAVIGATE_HOTLIST 3
#define NAVIGATE_ADD 4
#define NAVIGATE_HISTORY 5
#define NAVIGATE_HYPERSPACE 6

#define MAXNAVIGATEMENUS 7

/* page menu */

#define PAGE_NEXT 0
#define PAGE_PREV 1
#define PAGE_NAME 2
#define PAGE_GOTO 3
#define PAGE_ADDBEFORE 4
#define PAGE_ADDAFTER 5
#define PAGE_DEL 6
#define PAGE_PRINT_ONE_IN_STACK 7
#define PAGE_PAPER_SIZE_IN_STACK 8
#define PAGE_DEL_MANY 9
#define PAGE_PRINT_ONE_FILE_PER_PAGE 10

#define MAXPAGESTACKMENUS 11

#define PAGE_TOGGLELINE 0
#define PAGE_SIZING 1
#define PAGE_PRINT_ONE 2
#define PAGE_PAPER_SIZE 3

#define MAXPAGETILEMENUS 4

#define PAGE_STACK 0
#define PAGE_TILE 1

#define MAXPAGELAYOUTMODES 2

/* scroll stuff */

#define VERT_SCROLLBAR 0
#define HORI_SCROLLBAR 1

#define SCROLL_LEFTEND 0
#define SCROLL_LEFT 1
#define SCROLL_RIGHT 2
#define SCROLL_RIGHTEND 3
#define SCROLL_UPEND 4
#define SCROLL_UP 5
#define SCROLL_CHECKALL 6
#define SCROLL_UNCHECKALL 7
#define SCROLL_DOWN 8
#define SCROLL_DOWNEND 9

#define MAXSCROLLBUTTONS 10

#define SCRL_UP 0
#define SCRL_DN 1
#define SCRL_LF 2
#define SCRL_RT 3

#define NO_UPDATE_SCROLLING 0
#define JUMP_SCROLLING 1
#define SMOOTH_SCROLLING 2

/* pixel stuff */

#ifdef sun
#define PUT_A_POINT(dpy,win,gc,x,y) XDrawPoint(dpy,win,gc,x,y)
#else /* ~sun */
#ifdef ultrix
#define PUT_A_POINT(dpy,win,gc,x,y) XDrawPoint(dpy,win,gc,x,y)
#else /* ~ultrix */
#ifdef _USE_XDRAWPOINT_TO_PUT_A_POINT
#define PUT_A_POINT(dpy,win,gc,x,y) XDrawPoint(dpy,win,gc,x,y)
#else /* ~_USE_XDRAWPOINT_TO_PUT_A_POINT */
#define PUT_A_POINT(dpy,win,gc,x,y) XDrawLine(dpy,win,gc,x,y,x,y)
#endif /* _USE_XDRAWPOINT_TO_PUT_A_POINT */
#endif /* ultrix */
#endif /* sun */

/* file stuff */

#define OBJ_FILE_TYPE 0
#define SYM_FILE_TYPE 1
#define PIN_FILE_TYPE 2

/* xbm real_type */

#define XBM_XBM 0
#define XBM_EPS 1
#define XBM_EPSI 2

/* xpm real_type */

#define XPM_XPM  0
#define XPM_JPEG 1
#define PPM_TRUE 2
#define XPM_EPS  3 /* not implemented */
#define XPM_EPSI 4 /* not implemented */

#define PPM_DATA_RAW      0 /* not used */
#define PPM_JPEG_COMPRESS 1
#define PPM_DATA_DEFLATED 2

/* cmd stuff */

#define CMD_COMPOSITE     0
#define CMD_NEW           1
#define CMD_DELETE        2
#define CMD_MOVE          3
#define CMD_STRETCH       4
#define CMD_ONE_TO_MANY   5
#define CMD_MANY_TO_ONE   6
#define CMD_REPLACE       7
#define CMD_GOTO_PAGE     8
#define CMD_WB_CLEARALL   9
#define CMD_CHAT_A_LINE  10
#define CMD_WB_SLIDESHOW 11

/* rcb radius stuff */

#define DEF_RCB_RADIUS (EIGHTH_INCH)
#define MIN_RCB_RADIUS 4

/* select name dialog box stuff */

#define ITEM_DSPED 10
#define ITEM_LEN 30
#define ROW_HEIGHT \
  ((msgFontSet==NULL&&msgFontPtr==NULL)?(defaultFontHeight+1):(msgFontHeight+1))
#define BUTTON_OK 0
#define BUTTON_SETDIR 1
#define BUTTON_CANCEL 2
#define MAXBUTTONS 3

/* user redraw window stuff */

#define MAX_USER_REDRAWS 2

/* status window stuff */

#define MAX_STATUS_BTNS 3

/* copypaste stuff */

#define TGIF_HEADER 0x80
#define TGIF_PROTOCOL_ATOM "_TGIF_PROTOCOL_ATOM"

/* interrupt stuff */

#define MAXINTRS 2

/* remote status */

#define TG_REMOTE_STATUS_OK 0
#define TG_REMOTE_STATUS_INTR 1 /* user interrupt */
#define TG_REMOTE_STATUS_MEM 2 /* memory allocation failed */
#define TG_REMOTE_STATUS_WRITE 3 /* write failed */
#define TG_REMOTE_STATUS_READ 4 /* read failed */
#define TG_REMOTE_STATUS_NET 5 /* unexpect network error */
#define TG_REMOTE_STATUS_HOST 6 /* gethostbyname failed */
#define TG_REMOTE_STATUS_FORMAT 7 /* format error */
#define TG_REMOTE_STATUS_TERM 8 /* terminated by the server */
#define TG_REMOTE_STATUS_FILE 9 /* fail to open file/socket */

#define FTP_LOGGED_IN 0x10000

/* corner stuff */

#define CORNER_NONE 0
#define CORNER_LT 1
#define CORNER_TOP 2
#define CORNER_RT 3
#define CORNER_RIGHT 4
#define CORNER_RB 5
#define CORNER_BOTTOM 6
#define CORNER_LB 7
#define CORNER_LEFT 8

/* free rotation stuff */

#define CTM_SX 0
#define CTM_SIN 1
#define CTM_MSIN 2
#define CTM_SY 3

#define CTM_TX 0
#define CTM_TY 1

/* msgbox stuff */

#define MB_ID_FAILED 0
#define MB_ID_OK 1
#define MB_ID_CANCEL 2
#define MB_ID_YES 3
#define MB_ID_NO 4
#define MB_ID_EXTRA 5

#define MB_BTN_NONE		0
#define MB_BTN_OK		1
#define MB_BTN_YESNOCANCEL	2
#define MB_BTN_OKCANCEL		3
#define MB_BTN_YESNO		4
#define MB_BTN_EXTRA		0x0008

#define MB_ICON_STOP		0x0010
#define MB_ICON_QUESTION	0x0020
#define MB_ICON_INFORMATION	0x0040
#define MB_ICON_DIALOG		0x0080

#define MB_BTNMASK		0x0007
#define MB_ICONMASK		0x00f0

#define INFO_MB (MB_BTN_OK | MB_ICON_INFORMATION)
#define YNC_MB  (MB_BTN_YESNOCANCEL | MB_ICON_QUESTION)
#define YN_MB   (MB_BTN_YESNO | MB_ICON_QUESTION)
#define STOP_MB (MB_BTN_OK | MB_ICON_STOP)

#define MB_PIXMAP_STOP 0
#define MB_PIXMAP_QUESTION 1
#define MB_PIXMAP_INFORMATION 2
#define MB_PIXMAP_DIALOG 3

#define MAX_MB_ICONS 4

/* expr stuff */

#define NULL_VAL 0
#define INT_VAL 1
#define DBL_VAL 2
#define STR_VAL 3

/* file version stuff */

#define START_HAVING_ATTRS 8

/* input method stuff */

#define TGIM_NONE       INVALID
#define TGIM_XCIN       0
#define TGIM_CHINPUT    1
#define TGIM_KINPUT     2
#define TGIM_XIM        3
#define TGIM_TGTWB5     4

#define TGIM_DBIM       0x1
#define TGIM_SBIM       0x2

/* postscript stuff */

#define PS_GSAVE             0
#define PS_GRESTORE          1
#define PS_NEWPATH           2
#define PS_CLOSEPATH         3
#define PS_CHARPATH          4
#define PS_CURVETO           5
#define PS_LINETO            6
#define PS_RLINETO           7
#define PS_MOVETO            8
#define PS_RMOVETO           9
#define PS_STROKE            10
#define PS_FILL              11
#define PS_TRANSLATE         12
#define PS_ROTATE            13
#define PS_SCALE             14
#define PS_MUL               15
#define PS_DIV               16
#define PS_DUP               17
#define PS_NEG               18
#define PS_ADD               19
#define PS_SUB               20
#define PS_POP               21
#define PS_EXCH              22
#define PS_CONCAT            23
#define PS_CLIP              24
#define PS_EOCLIP            25
#define PS_EOFILL            26
#define PS_IMAGE             27
#define PS_IMAGEMASK         28
#define PS_ARRAY             29
#define PS_SETGRAY           30
#define PS_SETRGBCOLOR       31
#define PS_SETDASH           32
#define PS_SETLINEWIDTH      33
#define PS_SETMITERLIMIT     34
#define PS_SETLINECAP        35
#define PS_SETLINEJOIN       36
#define PS_SHOW              37
#define PS_FINDFONT          38
#define PS_MAKESETFONT       39
#define PS_ARCTO4            40
#define PS_CURRENTPOINT      41
#define PS_FLATTENPATH       42
#define PS_TGIFSETMATRIX     43
#define PS_TGIFRESTOREMATRIX 44

/* shape stuff */

#define SHAPE_BOX 0
#define SHAPE_PARALLEL 1
#define SHAPE_TRAPEZOID 2
#define SHAPE_RHOMBUS 3
#define SHAPE_RCBOX 4
#define SHAPE_OCTAGON 5
#define SHAPE_CROSS 6
#define SHAPE_OVAL 7
#define SHAPE_WORDS 8
#define SHAPE_HEXAGON 9
#define SHAPE_TRIANGLE 10
#define SHAPE_BLAST 11
#define SHAPE_STAR 12
#define SHAPE_DISK 13
#define SHAPE_RIGHTARROW 14
#define SHAPE_UPARROW 15
#define SHAPE_FATRIGHTARROW 16
#define SHAPE_FATUPARROW 17
#define SHAPE_RIGHTTAB 18
#define SHAPE_UPTAB 19

#define MAXSHAPES 20

/* export filter stuff */

#define EXPORT_NEXT 1
#define EXPORT_PREV -1
#define EXPORT_THIS 0

/* generic object stuff */

#define TGO_STRING 0
#define TGO_VISUAL 1
#define TGO_MATH 2
#define TGO_DATE 3
#define TGO_USER 4

/* break up text stuff */

#define BREAK_CHAR 0
#define BREAK_WORD 1
#define BREAK_LINE 2

/* xpm stuff */

#define MAXXPMEXPORTCOLORS 6400

/* choose a file stuff */

#define NAMES_SIMPLE_SELECT_NAME 0
#define NAMES_COMPLEX_SELECT_NAME 1
#define NAMES_SELECT_FILE 2
#define NAMES_EDIT_ATTR 3
#define NAMES_EDIT_NAME 4

#define NAMES_LOOP_MANY 0
#define NAMES_LOOP_ONCE 1

/* properties stuff */

#define PROP_MASK_CTM		0x00000001
#define PROP_MASK_COLOR		0x00000002
#define PROP_MASK_WIDTH		0x00000004
#define PROP_MASK_AW		0x00000008
#define PROP_MASK_AH		0x00000010
#define PROP_MASK_TRANSPAT	0x00000020
#define PROP_MASK_FILL		0x00000040
#define PROP_MASK_PEN		0x00000080
#define PROP_MASK_DASH		0x00000100
#define PROP_MASK_ARROW_STYLE	0x00000200
#define PROP_MASK_CURVED	0x00000400
#define PROP_MASK_RCB_RADIUS	0x00000800
#define PROP_MASK_TEXT_JUST	0x00001000
#define PROP_MASK_TEXT_SZ_UNIT	0x00002000
#define PROP_MASK_TEXT_FONT	0x00004000
#define PROP_MASK_TEXT_STYLE	0x00008000 /* ChangeEditTextProperty() only */
#define PROP_MASK_VSPACE	0x00010000
#define PROP_MASK_UNDERLINE_ON	0x00020000
#define PROP_MASK_OVERLINE_ON	0x00040000
#define PROP_MASK_WIDTH_INDEX	0x00100000 /* GetProperty() only */

/* mark stuff */

#define FORWARD 0
#define REVERSE 1

/* auto rotate pivot stuff */

#define AUTO_ROTATE_PIVOT 0
#define USER_ROTATE_PIVOT 1

#define MAX_ROTATE_PIVOT 2

#endif /*_TGIF_CONST_H_*/
