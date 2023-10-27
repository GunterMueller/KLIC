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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/ruler.c,v 1.20 2011/05/16 16:21:59 william Exp $
 */

#define _INCLUDE_FROM_RULER_C_

#include "tgifdefs.h"

#include "choice.e"
#include "cursor.e"
#include "dialog.e"
#include "file.e"
#include "font.e"
#include "grid.e"
#include "mainloop.e"
#include "mainmenu.e"
#include "menu.e"
#include "msg.e"
#include "raster.e"
#include "rect.e"
#include "ruler.e"
#include "select.e"
#include "setup.e"
#include "strtbl.e"
#include "text.e"
#include "util.e"
#include "xpixmap.e"

#define INCH_H (RULER_W-2)

#define HALF_INCH_H 10
#define QUARTER_INCH_H 6
#define MIN_INCH_H 3

#define HALF_CM_H 8
#define MIN_CM_H 4

int showMeasurement=FALSE;
int showMeasurementInTooltip=TRUE;
int simpleMotionInDrawWin=FALSE;
int showCrossHair=FALSE;

static int measureTooltipVerbose=FALSE;

static float gfPixelsPerUnit=(float)1.0;
static float gfNumUnits=(float)1.0;
static float gfNumFracUnits=(float)1.0;

static char numUnitStr[80], baseUnitStr[80], unitStr[80], formatUnitStr[80];

static GC rulerGC;

static int oldXOff=(-1), oldYOff=(-1);
static int rulerLen=0;
static int hRulerJustRedrawn=TRUE, justUnFrozen=FALSE;
static int freezeMarkRulerText=FALSE, frozenXOff=0, frozenYOff=0;
static char *gpszFrozenDeltaStr=NULL;

static char *gpszOldDeltaStr=NULL;

void GetUnitSpec(buf)
   char *buf;
{
   if (*formatUnitStr == '\0') {
      sprintf(buf, "%s %s/%s",
            (*numUnitStr)=='\0' ? "1" : numUnitStr,
            (*baseUnitStr)=='\0' ? "pixel" : baseUnitStr,
            (*unitStr)=='\0' ? "pixel" : unitStr);
   } else {
      char buf1[80];

      FormatFloat(&gfNumFracUnits, buf1);
      sprintf(buf, "%s %s/%s;%s;%s",
            (*numUnitStr)=='\0' ? "1" : numUnitStr,
            (*baseUnitStr)=='\0' ? "pixel" : baseUnitStr,
            (*unitStr)=='\0' ? "pixel" : unitStr, formatUnitStr, buf1);
   }
}

void ShowUnitMsg()
{
   sprintf(gszMsgBox, TgLoadString(STID_MEASUREMENT_SHOWN_IN_UNIT),
         (*unitStr)=='\0' ? "pixel" : unitStr,
         (*numUnitStr)=='\0' ? "1" : numUnitStr,
         (*baseUnitStr)=='\0' ? "pixel" : baseUnitStr);
   Msg(gszMsgBox);
}

static
int BadUnit(spec)
   char *spec;
{
   if (msgWindow != None) {
      sprintf(gszMsgBox, TgLoadString(STID_BAD_MEASUREMENT_UNIT_SPEC), spec);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
   }
   return FALSE;
}

static
int VerifyFormatUnitStr(format_unit_str)
   char *format_unit_str;
{
   char *c_ptr=format_unit_str, *semi_ptr=NULL;
   int seek_percent=TRUE, count=0;

   if ((semi_ptr=strchr(format_unit_str, ';')) == NULL) {
      return FALSE;
   } else {
      float fval;

      *semi_ptr++ = '\0';
      UtilTrimBlanks(semi_ptr);
      if (sscanf(semi_ptr, "%f", &fval) != 1) {
         return FALSE;
      }
      gfNumFracUnits = fval;
   }
   for (c_ptr=format_unit_str; *c_ptr != '\0'; c_ptr++) {
      if (seek_percent) {
         if (*c_ptr == '%') {
            seek_percent = FALSE;
         }
      } else if (*c_ptr == 'f') {
         seek_percent = TRUE;
         count++;
      } else if (!(*c_ptr == '.' || (*c_ptr >= '0' && *c_ptr <= '9'))) {
         return FALSE;
      }
   }
   if (seek_percent && count==2) {
      strcpy(formatUnitStr, format_unit_str);
      return TRUE;
   }
   return FALSE;
}

int SetUnit(spec)
   char *spec;
{
   char *spec_copy=UtilStrDup(spec);
   char *num_ptr=NULL, *base_ptr=NULL, *unit_ptr=NULL, *semi_ptr=NULL;
   int ok=TRUE;

   if (spec_copy == NULL) {
      return FALSE;
   }
   if ((semi_ptr=strchr(spec_copy, ';')) != NULL) {
      *semi_ptr++ = '\0';
      if (!VerifyFormatUnitStr(semi_ptr)) {
         ok = BadUnit(spec);
      }
   } else {
      *formatUnitStr = '\0';
   }
   if ((num_ptr=strtok(spec_copy, " \t\n\r")) != NULL &&
         (base_ptr=strtok(NULL, "/ \t\n\r")) != NULL &&
         (unit_ptr=strtok(NULL, "/ \t\n\r")) != NULL) {
      float fval;

      if (sscanf(num_ptr, "%f", &fval) == 1 && fval > INT_TOL &&
            *base_ptr != '\0' && *unit_ptr != '\0') {
         gfNumUnits = fval;
         strcpy(numUnitStr, num_ptr);
         if (UtilStrICmp("pixel", unit_ptr) == 0) {
            *unitStr = '\0';
         } else {
            strcpy(unitStr, unit_ptr);
         }
         switch (*base_ptr) {
         case 'i':
         case 'I':
            gfPixelsPerUnit = gfNumUnits * ((float)ONE_INCH);
            strcpy(baseUnitStr, "in");
            break;
         case 'c':
         case 'C':
            gfPixelsPerUnit = gfNumUnits * ((float)ONE_CM);
            strcpy(baseUnitStr, "cm");
            break;
         case 'p':
         case 'P':
            gfPixelsPerUnit = gfNumUnits;
            *baseUnitStr = '\0';
            break;
         default: ok = BadUnit(spec); break;
         }
      } else {
         ok = BadUnit(spec);
      }
   } else {
      ok = BadUnit(spec);
   }
   if (!ok) {
      gfPixelsPerUnit = gfNumUnits = (float)1.0;
      *numUnitStr = *baseUnitStr = *unitStr = *formatUnitStr = '\0';
   }
   free(spec_copy);
   return ok;
}

void InitRuler()
{
   XGCValues values;
   char *c_ptr;

   values.foreground = myFgPixel;
   values.background = (threeDLook ? myLtGryPixel : myBgPixel);
   values.fill_style = FillSolid;
   values.font = rulerFontPtr->fid;

   rulerGC = XCreateGC(mainDisplay, mainWindow,
         GCForeground | GCBackground | GCFillStyle | GCFont, &values);

   showMeasurement = FALSE;
   if ((c_ptr=XGetDefault(mainDisplay, TOOL_NAME, "ShowMeasurement")) != NULL &&
         UtilStrICmp(c_ptr, "true") == 0) {
      showMeasurement = TRUE;
   }
   showCrossHair = FALSE;
/* if ((c_ptr=XGetDefault(mainDisplay, TOOL_NAME, "ShowCrossHair")) != NULL &&
         UtilStrICmp(c_ptr, "true") == 0) {
      showCrossHair = TRUE;
   } */

   gfPixelsPerUnit = gfNumUnits = (float)1.0;
   *numUnitStr = *baseUnitStr = *unitStr = *formatUnitStr = '\0';
   if ((c_ptr=XGetDefault(mainDisplay, TOOL_NAME, "ShowMeasurementUnit")) !=
         NULL) {
      char spec[80];

      if (strcmp("pixel", c_ptr) == 0) {
         strcpy(spec, "1 pixel/pixel");
      } else if (strcmp("inch", c_ptr) == 0) {
         sprintf(spec, "%1d pixel/in", ONE_INCH);
      } else if (strcmp("cm", c_ptr) == 0) {
         sprintf(spec, "%1d pixel/cm", ONE_CM);
      } else {
         strcpy(spec, c_ptr);
      }
      if (!SetUnit(spec)) {
         fprintf(stderr, TgLoadString(STID_INVALID_XDEF_USE_ALT_STR),
               TOOL_NAME, "ShowMeasurementUnit", spec, "pixel");
         fprintf(stderr, "\n");
      }
   }
   rulerLen = rulerW-1;
}

void CleanUpRuler()
{
   XFreeGC(mainDisplay, rulerGC);
}

void PixelToMeasurementUnit(Buf, NumPixels)
   char *Buf;
   int NumPixels;
{
   float fval;
   int ival;

   if (*unitStr == '\0') {
      sprintf(Buf, "%+1d", NumPixels);
   } else {
      fval = (float)((float)NumPixels*1000.0)/(gfPixelsPerUnit);
      ival = ((int)round(fval));
      fval = ((float)ival)/((float)1000.0);
      if (*formatUnitStr == '\0') {
         sprintf(Buf, "%+.3f%s", fval, unitStr);
      } else {
         float frac;

         ival = (int)fval;
         frac = ((ival >= 0) ? fval-((float)ival) : ((float)ival)-fval);
         fval = (float)ival;
         sprintf(Buf, formatUnitStr, fval, frac*gfNumFracUnits);
      }
   }
}

void SquarePixelToMeasurementUnit(Buf, NumSquarePixels)
   char *Buf;
   int NumSquarePixels;
{
   float fval;
   int ival;

   if (*unitStr == '\0') {
      sprintf(Buf, "%+1d", NumSquarePixels);
   } else {
      fval = (float)((float)NumSquarePixels*1000.0)/(gfPixelsPerUnit);
      fval = (float)(fval/gfPixelsPerUnit);
      ival = ((int)round(fval));
      fval = ((float)ival)/((float)1000.0);
      if (*formatUnitStr == '\0') {
         sprintf(Buf, "%+.3f sq %s", fval, unitStr);
      } else {
         float frac;

         ival = (int)fval;
         frac = ((ival >= 0) ? fval-((float)ival) : ((float)ival)-fval);
         fval = (float)ival;
         sprintf(Buf, formatUnitStr, fval, frac*gfNumFracUnits);
      }
   }
}

void PixelToCurrentUnit(Buf, NumPixels)
   char *Buf;
   int NumPixels;
{
   int unit_str_was_empty=(*unitStr == '\0');
   char num_unit_str[80], base_unit_str[80];
   float pixel_per_unit=gfPixelsPerUnit, num_units=gfNumUnits;

   if (unit_str_was_empty) {
      UtilStrCpyN(num_unit_str, sizeof(num_unit_str), numUnitStr);
      UtilStrCpyN(base_unit_str, sizeof(base_unit_str), baseUnitStr);
      switch (gridSystem) {
      case ENGLISH_GRID:
         gfPixelsPerUnit = 128;
         gfNumUnits = 1;
         UtilStrCpyN(unitStr, sizeof(unitStr), "inch");
         break;
      case METRIC_GRID:
         gfPixelsPerUnit = 50;
         gfNumUnits = 2.5;
         UtilStrCpyN(unitStr, sizeof(unitStr), "cm");
         break;
      }
      PixelToMeasurementUnit(Buf, NumPixels);
   } else {
      PixelToMeasurementUnit(Buf, NumPixels);
   }
   if (unit_str_was_empty) {
      gfPixelsPerUnit = pixel_per_unit;
      gfNumUnits = num_units;
      *unitStr = '\0';
      UtilStrCpyN(numUnitStr, sizeof(numUnitStr), num_unit_str);
      UtilStrCpyN(baseUnitStr, sizeof(baseUnitStr), base_unit_str);
   }
}

static
int RightMarginMarkVisible()
{
   return ((rightMarginEnabled == TRUE) && (drawOrigX+drawWinW >
         paperWidth-(rightMargin>>TIK_PER_PIXEL_SHIFTS)));
}

static
void GetRightMarginMarkVs(vs)
   XPoint vs[6];
{
   int x=OFFSET_X(paperWidth-(rightMargin>>TIK_PER_PIXEL_SHIFTS))-1;
   int delta=(rulerW>>2);
   int y_end=rulerW-(threeDLook ? ((windowPadding>>1)+2) : 1);
   int y_start=(threeDLook ? ((windowPadding>>1)-2) : (-1))+(delta<<1);

   vs[0].x = x;        vs[0].y = y_start;
   vs[1].x = x-delta;  vs[1].y = y_start+delta;
   vs[2].x = x-delta;  vs[2].y = y_end;
   vs[3].x = x+delta;  vs[3].y = y_end;
   vs[4].x = x+delta;  vs[4].y = y_start+delta;
   vs[5].x = x;        vs[5].y = y_start;
}

static
int PtInRightMarginMark(button_x, button_y)
   int button_x, button_y;
{
   XPoint vs[6];

   GetRightMarginMarkVs(vs);

   return PointInPolygon(button_x, button_y, 6, vs);
}

static
void RedrawRightMargin(dpy, win)
   Display *dpy;
   Window win;
{
   if (rightMarginEnabled != TRUE) return;

   if (drawOrigX+drawWinW > paperWidth-(rightMargin>>TIK_PER_PIXEL_SHIFTS)) {
      int delta=(rulerW>>2);
      int x=OFFSET_X(paperWidth-(rightMargin>>TIK_PER_PIXEL_SHIFTS))-1;
      int y_end=rulerW-(threeDLook ? ((windowPadding>>1)+2) : 1);
      int y_start=(threeDLook ? ((windowPadding>>1)-2) : (-1))+(delta<<1);
      int bg_pixel=(threeDLook ? myLtGryPixel : myBgPixel);
      GC gc=defaultGC;
      XPoint vs[6];

      GetRightMarginMarkVs(vs);
      if (threeDLook) {
         XSetForeground(mainDisplay, textMenuGC, bg_pixel);
         XFillPolygon(dpy, win, textMenuGC, vs, 6, Convex, CoordModeOrigin);
         XDrawLines(dpy, win, gc, vs, 6, CoordModeOrigin);
      } else {
         XDrawLines(dpy, win, gc, vs, 6, CoordModeOrigin);
      }
      rightMarginActive = ShouldRightMarginBeActive();
      if (!rightMarginActive) {
         XDrawLine(dpy, win, gc, x-delta, y_start+1, x+delta-1, y_end);
         XDrawLine(dpy, win, gc, x-delta+1, y_start+1, x+delta, y_end);
         /*
          * The code doesn't look right, but it look symmetrical on the screen!
          *
          * XDrawLine(dpy, win, gc, x+delta, y_start+1, x-delta+1, y_end);
          * XDrawLine(dpy, win, gc, x+delta-1, y_start+1, x-delta, y_end);
          */
         XDrawLine(dpy, win, gc, x+delta+1, y_start, x-delta+1, y_end);
         XDrawLine(dpy, win, gc, x+delta, y_start, x-delta, y_end);
      }
   }
}

void RedrawHRuler(dpy, win)
   Display *dpy;
   Window win;
{
   register int i, pos, len, index;
   int inc, abs_inc, y=rulerW-(threeDLook ? (windowPadding>>1) : 0);
   char s[5];

   switch (gridSystem) {
   case ENGLISH_GRID:
      inc = (zoomedIn ? (xyEnglishGrid<<zoomScale) : xyEnglishGrid);
      abs_inc = ABS_SIZE(inc);

      if (drawOrigX % abs_inc == 0) {
         i = 0;
         pos = 0;
      } else {
         i = ((int)(drawOrigX / abs_inc) + 1) * abs_inc - drawOrigX;
         pos = ZOOMED_SIZE(i);
      }
      pos--; /* since drawWindow has no border */
      for ( ; i<drawWinW; i+=abs_inc, pos+=inc) {
         if ((GRID_ZOOMED_SIZE(i+drawOrigX)) % ONE_INCH == 0) {
            index = (int)((GRID_ZOOMED_SIZE(i+drawOrigX)) / ONE_INCH);
            sprintf(s, "%1d", GRID_ABS_SIZE(index));
            len = strlen(s);
            XDrawString(dpy, win, rulerGC, pos+2, y-INCH_H+rulerFontAsc,
                  s, len);
            XDrawLine(dpy, win, defaultGC, pos, y, pos, y-INCH_H);
         } else if ((GRID_ZOOMED_SIZE(i+drawOrigX)) % HALF_INCH == 0) {
            XDrawLine(dpy, win, defaultGC, pos, y, pos, y-HALF_INCH_H);
         } else if ((GRID_ZOOMED_SIZE(i+drawOrigX)) % QUARTER_INCH == 0) {
            XDrawLine(dpy, win, defaultGC, pos, y, pos, y-QUARTER_INCH_H);
         } else {
            XDrawLine(dpy, win, defaultGC, pos, y, pos, y-MIN_INCH_H);
         }
      }
      break;
   case METRIC_GRID:
      inc = (zoomedIn ? (xyMetricGrid<<zoomScale) : xyMetricGrid);
      abs_inc = ABS_SIZE(inc);

      if (drawOrigX % abs_inc == 0) {
         i = 0;
         pos = 0;
      } else {
         i = ((int)(drawOrigX / abs_inc) + 1) * abs_inc - drawOrigX;
         pos = ZOOMED_SIZE(i);
      }
      pos--; /* since drawWindow has no border */
      for ( ; i<drawWinW; i+=abs_inc, pos+=inc) {
         if ((GRID_ZOOMED_SIZE(i+drawOrigX)) % ONE_CM == 0) {
            index = (int)((GRID_ZOOMED_SIZE(i+drawOrigX)) / ONE_CM);
            sprintf(s, "%1d", GRID_ABS_SIZE(index));
            len = strlen(s);
            XDrawString(dpy, win, rulerGC, pos+2, y-INCH_H+rulerFontAsc,
                  s, len);
            XDrawLine(dpy, win, defaultGC, pos, y, pos, y-INCH_H);
         } else if ((GRID_ZOOMED_SIZE(i+drawOrigX)) % FIVE_MM == 0) {
            XDrawLine(dpy, win, defaultGC, pos, y, pos, y-HALF_CM_H);
         } else {
            XDrawLine(dpy, win, defaultGC, pos, y, pos, y-MIN_CM_H);
         }
      }
      break;
   }
   RedrawRightMargin(dpy, win);
}

static
void DrawHRuleTick(XOff)
   int XOff;
{
   XDrawLine(mainDisplay, hRuleWindow, revDefaultGC, XOff, 0, XOff, rulerLen);
}

static
void DrawVRuleTick(YOff)
{
   XDrawLine(mainDisplay, vRuleWindow, revDefaultGC, 0, YOff, rulerLen, YOff);
}

void RedrawHRulerWindow()
{
   XEvent ev;

   XClearWindow(mainDisplay, hRuleWindow);

   XSync(mainDisplay, False);
   while (XCheckWindowEvent(mainDisplay, hRuleWindow, ExposureMask, &ev)) ;

   RedrawHRuler(mainDisplay, hRuleWindow);

   oldXOff = (-1);
   DrawHRuleTick(oldXOff-1);
   hRulerJustRedrawn = TRUE;
   justUnFrozen = FALSE;
}

void RedrawVRuler(dpy, win)
   Display *dpy;
   Window win;
{
   register int i, pos, len, index;
   int inc, abs_inc, x=rulerW-(threeDLook ? (windowPadding>>1) : 0);
   char s[5];

   switch (gridSystem) {
   case ENGLISH_GRID:
      inc = (zoomedIn ? (xyEnglishGrid<<zoomScale) : xyEnglishGrid);
      abs_inc = ABS_SIZE(inc);

      if (drawOrigY % abs_inc == 0) {
         i = 0;
         pos = 0;
      } else {
         i = ((int)(drawOrigY / abs_inc) + 1) * abs_inc - drawOrigY;
         pos = ZOOMED_SIZE(i);
      }
      pos--; /* since drawWindow has no border */
      for ( ; i<drawWinH; i+=abs_inc, pos+=inc) {
         if ((GRID_ZOOMED_SIZE(i+drawOrigY)) % ONE_INCH == 0) {
            index = (int)((GRID_ZOOMED_SIZE(i+drawOrigY)) / ONE_INCH);
            sprintf(s, "%1d", GRID_ABS_SIZE(index));
            len = strlen(s);
            XDrawString(dpy, win, rulerGC, 1, pos+rulerFontAsc+2, s, len);
            XDrawLine(dpy, win, defaultGC, x, pos, x-INCH_H, pos);
         } else if ((GRID_ZOOMED_SIZE(i+drawOrigY)) % HALF_INCH == 0) {
            XDrawLine(dpy, win, defaultGC, x, pos, x-HALF_INCH_H, pos);
         } else if ((GRID_ZOOMED_SIZE(i+drawOrigY)) % QUARTER_INCH == 0) {
            XDrawLine(dpy, win, defaultGC, x, pos, x-QUARTER_INCH_H, pos);
         } else {
            XDrawLine(dpy, win, defaultGC, x, pos, x-MIN_INCH_H, pos);
         }
      }
      break;
   case METRIC_GRID:
      inc = (zoomedIn ? (xyMetricGrid<<zoomScale) : xyMetricGrid);
      abs_inc = ABS_SIZE(inc);

      if (drawOrigY % abs_inc == 0) {
         i = 0;
         pos = 0;
      } else {
         i = ((int)(drawOrigY / abs_inc) + 1) * abs_inc - drawOrigY;
         pos = ZOOMED_SIZE(i);
      }
      pos--; /* since drawWindow has no border */
      for ( ; i<drawWinH; i+=abs_inc, pos+=inc) {
         if ((GRID_ZOOMED_SIZE(i+drawOrigY)) % ONE_CM == 0) {
            index = (int)((GRID_ZOOMED_SIZE(i+drawOrigY)) / ONE_CM);
            sprintf(s, "%1d", GRID_ABS_SIZE(index));
            len = strlen(s);
            XDrawString(dpy, win, rulerGC, 1, pos+rulerFontAsc+2, s, len);
            XDrawLine(dpy, win, defaultGC, x, pos, x-INCH_H, pos);
         } else if ((GRID_ZOOMED_SIZE(i+drawOrigY)) % FIVE_MM == 0) {
            XDrawLine(dpy, win, defaultGC, x, pos, x-HALF_CM_H, pos);
         } else {
            XDrawLine(dpy, win, defaultGC, x, pos, x-MIN_CM_H, pos);
         }
      }
      break;
   }
}

void RedrawVRulerWindow()
{
   XEvent ev;

   XClearWindow(mainDisplay, vRuleWindow);

   XSync(mainDisplay, False);
   while (XCheckWindowEvent(mainDisplay, vRuleWindow, ExposureMask, &ev)) ;

   RedrawVRuler(mainDisplay, vRuleWindow);

   if (!freezeMarkRulerText && showMeasurement && !hRulerJustRedrawn &&
         oldYOff != 0) {
      char x_buf[80], y_buf[80], buf[80];

      PixelToMeasurementUnit(x_buf, ABS_X(oldXOff));
      PixelToMeasurementUnit(y_buf, ABS_Y(oldYOff));
      sprintf(buf, "[%s,%s]", x_buf, y_buf);
      if (!showMeasurementInTooltip) {
         XDrawString(mainDisplay, hRuleWindow, revDefaultGC,
               8, 2+defaultFontAsc, buf, strlen(buf));
      }
      hRulerJustRedrawn = TRUE;
   }
   oldYOff = (-1);
   DrawVRuleTick(oldYOff-1);
}

void MarkRulers(XOff, YOff)
   int XOff, YOff;
{
   char x_buf[80], y_buf[80], buf[MAXSTRING];

   DrawHRuleTick(oldXOff-1);
   DrawVRuleTick(oldYOff-1);
   if (showCrossHair) {
      XDrawLine(mainDisplay, drawWindow, revDefaultGC, oldXOff, 0,
            oldXOff, ZOOMED_SIZE(drawWinH));
      XDrawLine(mainDisplay, drawWindow, revDefaultGC, 0, oldYOff,
            ZOOMED_SIZE(drawWinW), oldYOff);
   }
   if (hRulerJustRedrawn) {
      hRulerJustRedrawn= FALSE;
   } else if (!freezeMarkRulerText && showMeasurement) {
      if (justUnFrozen) {
         justUnFrozen = FALSE;
         PixelToMeasurementUnit(x_buf, ABS_X(frozenXOff));
         PixelToMeasurementUnit(y_buf, ABS_Y(frozenYOff));
         sprintf(buf, "[%s,%s]", x_buf, y_buf);
      } else {
         PixelToMeasurementUnit(x_buf, ABS_X(oldXOff));
         PixelToMeasurementUnit(y_buf, ABS_Y(oldYOff));
         sprintf(buf, "[%s,%s]", x_buf, y_buf);
      }
      if (!showMeasurementInTooltip) {
         XDrawString(mainDisplay, hRuleWindow, revDefaultGC,
               8, 2+defaultFontAsc, buf, strlen(buf));
      }
   }
   DrawHRuleTick(XOff-1);
   DrawVRuleTick(YOff-1);

   if (showCrossHair) {
      XDrawLine(mainDisplay, drawWindow, revDefaultGC, XOff, 0,
            XOff, ZOOMED_SIZE(drawWinH));
      XDrawLine(mainDisplay, drawWindow, revDefaultGC, 0, YOff,
            ZOOMED_SIZE(drawWinW), YOff);
   }
   if (!freezeMarkRulerText && showMeasurement) {
      PixelToMeasurementUnit(x_buf, ABS_X(XOff));
      PixelToMeasurementUnit(y_buf, ABS_Y(YOff));
      if (simpleMotionInDrawWin && curChoice == NOTHING && topSel != NULL &&
            VerboseMeasureTooltip()) {
         char w_buf[80], h_buf[80];
         char lx_buf[80], ty_buf[80], rx_buf[80], by_buf[80];

         PixelToMeasurementUnit(w_buf, selObjRbX-selObjLtX);
         PixelToMeasurementUnit(h_buf, selObjRbY-selObjLtY);
         PixelToMeasurementUnit(lx_buf, selObjLtX);
         PixelToMeasurementUnit(rx_buf, selObjRbX);
         PixelToMeasurementUnit(ty_buf, selObjLtY);
         PixelToMeasurementUnit(by_buf, selObjRbY);
         sprintf(buf,
         "[%s,%s]\nsel_lx=%s sel_rx=%s\nsel_ty=%s sel_by=%s\nsel_w=%s sel_h=%s",
               x_buf, y_buf, lx_buf, rx_buf, ty_buf, by_buf, w_buf, h_buf);
      } else {
         sprintf(buf, "[%s,%s]", x_buf, y_buf);
      }
      if (showMeasurementInTooltip) {
         SetMeasureTooltip(buf);
      } else {
         XDrawString(mainDisplay, hRuleWindow, revDefaultGC,
               8, 2+defaultFontAsc, buf, strlen(buf));
      }
   } else if (VerboseMeasureTooltip()) {
      switch (curChoice) {
      case NOTHING:
      case DRAWPOLY:
      case DRAWPOLYGON:
      case VERTEXMODE:
         DoIntervalMeasureTooltip(frozenXOff, frozenYOff, XOff, YOff,
               (frozenXOff+XOff)>>1, (frozenYOff+YOff)>>1, gpszFrozenDeltaStr);
         break;
      }
   }
   oldXOff = XOff;
   oldYOff = YOff;
}

void RedrawRulers()
{
   RedrawHRulerWindow();
   RedrawVRulerWindow();
}

void GetCrossHairPosition(pnXOff, pnYOff, pnShown)
   int *pnXOff, *pnYOff, *pnShown;
{
   if (pnXOff != NULL) *pnXOff = oldXOff;
   if (pnYOff != NULL) *pnYOff = oldYOff;
   if (pnShown != NULL) *pnShown = showCrossHair;
}

void RedrawCrossHair()
{
   if (showCrossHair) {
      XDrawLine(mainDisplay, drawWindow, revDefaultGC, oldXOff, 0,
            oldXOff, ZOOMED_SIZE(drawWinH));
      XDrawLine(mainDisplay, drawWindow, revDefaultGC, 0, oldYOff,
            ZOOMED_SIZE(drawWinW), oldYOff);
   }
}

void ToggleShowCrossHair()
{
   /* the cross-hair stuff is disabled -- cannot get it to work right */
   if (!showCrossHair) return;

   RedrawCrossHair();

/* showCrossHair = !showCrossHair; */
   showCrossHair = FALSE;
   RedrawRulers();
   if (showCrossHair) {
      Msg(TgLoadString(STID_SHOW_CROSSHAIR_ENABLED));
      SetNullCursor(drawWindow);
   } else {
      SetDefaultCursor(drawWindow);
      ShowCursor();
      Msg(TgLoadString(STID_SHOW_CROSSHAIR_DISABLED));
   }
}

static
void ContinueMoveRightMarginMark(dpy, OrigX, OrigY)
   Display *dpy;
   int OrigX, OrigY; /* these are mouse coordinates */
{
   int moving=TRUE, dx=0, grid_x=0, grid_y=0, h=ZOOMED_SIZE(drawWinH);
   int prev_x=0, orig_grid_x=0, orig_grid_y=0, orig_right_margin=0;
   char buf[80], buf2[80];
   double dval=(double)0;
   XPoint orig_vs[6], vs[6];
   XEvent ev;

   XFlush(dpy);
   XSync(dpy, False);

   if (XCheckMaskEvent(dpy, ExposureMask, &ev) ||
         XCheckMaskEvent(dpy, VisibilityChangeMask, &ev)) {
      ExposeEventHandler(&ev, TRUE);
   }
   GridXY(OrigX, OrigY, &grid_x, &grid_y);
   orig_grid_x = grid_x;
   orig_grid_y = grid_y;
   orig_right_margin = OFFSET_X(paperWidth-(rightMargin>>TIK_PER_PIXEL_SHIFTS));
   prev_x = orig_right_margin+0;
   GetRightMarginMarkVs(orig_vs);
   memcpy(vs, orig_vs, 6*sizeof(XPoint));
   dval = ((double)rightMargin) * ((double)printMag) / ((double)TIK_PER_PIXEL);
   dval /= ((double)100);
   PixelToMeasurementUnit(buf, round(dval));
   sprintf(buf2, "margin=%s", buf);
   StartShowMeasureCursor(OrigX, 4, buf2, FALSE);

   /* draw */
   XDrawLine(dpy, drawWindow, revDefaultGC, prev_x, 0, prev_x, h);
   XDrawLines(dpy, hRuleWindow, revDefaultGC, vs, 6, CoordModeOrigin);
   if (!debugNoPointerGrab) {
      XGrabPointer(dpy, drawWindow, FALSE,
            PointerMotionMask | ButtonReleaseMask,
            GrabModeAsync, GrabModeAsync, None, moveCursor, CurrentTime);
   }
   while (moving) {
      XEvent input;

      XNextEvent(dpy, &input);

      if (input.type == Expose || input.type == VisibilityNotify) {
         ExposeEventHandler(&input, TRUE);
      } else if (input.type == ButtonRelease) {
         int abs_dx=ABS_SIZE(dx);

         XUngrabPointer(dpy, CurrentTime);
         XSync(dpy, False);

         moving = FALSE;

         /* erase */
         XDrawLine(dpy, drawWindow, revDefaultGC, prev_x, 0, prev_x, h);
         XDrawLines(dpy, hRuleWindow, revDefaultGC, vs, 6, CoordModeOrigin);
         EndShowMeasureCursor(OrigX+dx, 4, buf2, FALSE);

         rightMargin -= (abs_dx<<TIK_PER_PIXEL_SHIFTS);
         RedrawHRulerWindow();
      } else if (input.type == MotionNotify) {
         int i=0, mouse_x=0, mouse_y=0;

         /* erase */
         XDrawLine(dpy, drawWindow, revDefaultGC, prev_x, 0, prev_x, h);
         XDrawLines(dpy, hRuleWindow, revDefaultGC, vs, 6, CoordModeOrigin);
         ShowMeasureCursor(OrigX+dx, 4, buf2, FALSE);

         mouse_x = input.xmotion.x;
         mouse_y = input.xmotion.y;
         GridXY(mouse_x, mouse_y, &grid_x, &grid_y);

         dx = grid_x - orig_grid_x;
         prev_x = orig_right_margin+0+dx;
         for (i=0; i < 6; i++) {
            vs[i].x = orig_vs[i].x+dx;
         }
         /* draw */
         XDrawLine(dpy, drawWindow, revDefaultGC, prev_x, 0, prev_x, h);
         XDrawLines(dpy, hRuleWindow, revDefaultGC, vs, 6, CoordModeOrigin);

         dval = (((double)(rightMargin-(ABS_SIZE(dx)<<TIK_PER_PIXEL_SHIFTS))) *
               ((double)printMag) / ((double)TIK_PER_PIXEL)) / ((double)100);
         PixelToMeasurementUnit(buf, round(dval));
         sprintf(buf2, "margin=%s", buf);
         ShowMeasureCursor(OrigX+dx, 4, buf2, FALSE);
      }
   }
}

void RulersEventHandler(input)
   XEvent *input;
{
   if (input->type == ButtonPress) {
      XButtonEvent *button_ev=(&input->xbutton);
      int mouse_x=button_ev->x, mouse_y=button_ev->y;

      if (RightMarginMarkVisible() && PtInRightMarginMark(mouse_x, mouse_y)) {
         ContinueMoveRightMarginMark(mainDisplay, mouse_x, mouse_y);
      } else if (button_ev->button == Button1) {
         IncGrid();
      } else if (button_ev->button == Button2) {
         GridMenu(button_ev->x_root, button_ev->y_root, FALSE);
      } else if (button_ev->button == Button3) {
         DecGrid();
      }
   } else if (input->xany.window == vRuleWindow) {
      if (input->type == Expose) {
         RedrawVRulerWindow();
      } else if (input->type == EnterNotify) {
         SetMouseStatus(TgLoadCachedString(CSTID_INC_GRID_SIZE),
               TgLoadCachedString(CSTID_GRID_MENU),
               TgLoadCachedString(CSTID_DEC_GRID_SIZE));
      }
   } else if (input->xany.window == hRuleWindow) {
      if (input->type == Expose) {
         RedrawHRulerWindow();
      } else if (input->type == EnterNotify) {
         SetMouseStatus(TgLoadCachedString(CSTID_INC_GRID_SIZE),
               TgLoadCachedString(CSTID_GRID_MENU),
               TgLoadCachedString(CSTID_DEC_GRID_SIZE));
      }
   }
}

void FreezeMarkRulerText()
{
   if (!freezeMarkRulerText) {
      EndMeasureTooltip(FALSE);
   }
   freezeMarkRulerText = TRUE;
   frozenXOff = oldXOff;
   frozenYOff = oldYOff;
   UtilFree(gpszFrozenDeltaStr);
   if (gpszOldDeltaStr != NULL) {
      gpszFrozenDeltaStr = UtilStrDup(gpszOldDeltaStr);
      if (gpszFrozenDeltaStr == NULL) FailAllocMessage();
   }
}

void UnFreezeMarkRulerText()
{
   freezeMarkRulerText = FALSE;
   justUnFrozen = TRUE;
}

static int oldLtX, oldLtY, oldRbX, oldRbY, oldMdX, oldMdY;

int VerboseMeasureTooltip()
{
   return (showMeasurement && showMeasurementInTooltip &&
         measureTooltipVerbose);
}

static
void DoIntervalRulers()
{
   if (oldLtX == oldRbX) {
      XDrawLine(mainDisplay, hRuleWindow, revDefaultGC, oldLtX-1, 0, oldLtX-1,
            rulerLen);
   } else if (oldLtX==oldMdX || oldRbX==oldMdX) {
      XDrawLine(mainDisplay, hRuleWindow, revDefaultGC, oldLtX-1, 0, oldLtX-1,
            rulerLen);
      XDrawLine(mainDisplay, hRuleWindow, revDefaultGC, oldRbX-1, 0, oldRbX-1,
            rulerLen);
   } else {
      XDrawLine(mainDisplay, hRuleWindow, revDefaultGC, oldLtX-1, 0, oldLtX-1,
            rulerLen);
      XDrawLine(mainDisplay, hRuleWindow, revDefaultGC, oldMdX-1, 0, oldMdX-1,
            rulerLen);
      XDrawLine(mainDisplay, hRuleWindow, revDefaultGC, oldRbX-1, 0, oldRbX-1,
            rulerLen);
   }
   if (oldLtY == oldRbY) {
      XDrawLine(mainDisplay, vRuleWindow, revDefaultGC, 0, oldLtY-1, rulerLen-1,
            oldLtY);
   } else if (oldLtY==oldMdY || oldRbY==oldMdY) {
      XDrawLine(mainDisplay, vRuleWindow, revDefaultGC, 0, oldLtY-1, rulerLen-1,
            oldLtY);
      XDrawLine(mainDisplay, vRuleWindow, revDefaultGC, 0, oldRbY-1, rulerLen-1,
            oldRbY);
   } else {
      XDrawLine(mainDisplay, vRuleWindow, revDefaultGC, 0, oldLtY-1, rulerLen,
            oldLtY-1);
      XDrawLine(mainDisplay, vRuleWindow, revDefaultGC, 0, oldMdY-1, rulerLen,
            oldMdY-1);
      XDrawLine(mainDisplay, vRuleWindow, revDefaultGC, 0, oldRbY-1, rulerLen,
            oldRbY-1);
   }
   if (showCrossHair) {
      XDrawLine(mainDisplay, drawWindow, revDefaultGC, oldRbX, 0,
            oldRbX, ZOOMED_SIZE(drawWinH));
      XDrawLine(mainDisplay, drawWindow, revDefaultGC, 0, oldRbY,
            ZOOMED_SIZE(drawWinW), oldRbY);
   }
   if (VerboseMeasureTooltip()) {
      DoIntervalMeasureTooltip(oldLtX, oldLtY, oldRbX, oldRbY, oldMdX, oldMdY,
            gpszOldDeltaStr);
   }
}

void BeginIntervalRulers(ltx, lty, rbx, rby)
   int ltx, lty, rbx, rby;
{
   DrawHRuleTick(oldXOff-1);
   DrawVRuleTick(oldYOff-1);
   if (showCrossHair) {
      XDrawLine(mainDisplay, drawWindow, revDefaultGC, oldXOff, 0,
            oldXOff, ZOOMED_SIZE(drawWinH));
      XDrawLine(mainDisplay, drawWindow, revDefaultGC, 0, oldYOff,
            ZOOMED_SIZE(drawWinW), oldYOff);
   }
   oldLtX = ltx; oldLtY = lty;
   oldRbX = rbx; oldRbY = rby;
   oldMdX = (oldLtX+oldRbX)>>1;
   oldMdY = (oldLtY+oldRbY)>>1;
   gpszOldDeltaStr = NULL;
   if (showMeasurement) FreezeMarkRulerText();
   DoIntervalRulers();
}

void DrawIntervalRulers(ltx, lty, rbx, rby, delta_str)
   int ltx, lty, rbx, rby;
   char *delta_str;
{
   DoIntervalRulers();
   oldLtX = ltx; oldLtY = lty;
   oldRbX = rbx; oldRbY = rby;
   oldMdX = (oldLtX+oldRbX)>>1; oldMdY = (oldLtY+oldRbY)>>1;
   if (delta_str != NULL) {
      gpszOldDeltaStr = UtilStrDup(delta_str);
      if (gpszOldDeltaStr == NULL) FailAllocMessage();
   }
   DoIntervalRulers();
   if (showMeasurement) UnFreezeMarkRulerText();
}

void EndIntervalRulers(x, y)
   int x, y;
{
   DoIntervalRulers();
   oldXOff = x;
   oldYOff = y;
   UtilFree(gpszOldDeltaStr);
   gpszOldDeltaStr = NULL;
   DrawHRuleTick(oldXOff-1);
   DrawVRuleTick(oldYOff-1);
   if (showCrossHair) {
      XDrawLine(mainDisplay, drawWindow, revDefaultGC, oldXOff, 0,
            oldXOff, ZOOMED_SIZE(drawWinH));
      XDrawLine(mainDisplay, drawWindow, revDefaultGC, 0, oldYOff,
            ZOOMED_SIZE(drawWinW), oldYOff);
   }
   MarkRulers(x, y);
   frozenXOff = oldXOff;
   frozenYOff = oldYOff;
}

void StartShowMeasureCursor(XOff, YOff, Str, ExtraSpace)
   int XOff, YOff, ExtraSpace;
   char *Str;
{
   if (!showMeasurement) return;
   MarkRulers(XOff, YOff);
   FreezeMarkRulerText();
   if (Str == NULL || *Str == '\0') return;

   if (showMeasurementInTooltip) {
      if (!measureTooltipVerbose) {
         StartMeasureTooltip(Str);
      }
   } else {
      int x=(ExtraSpace ? XOff+18 : XOff+4), y=YOff+defaultFontAsc;
      char *c_ptr=strchr(Str, '\n'), *line=Str;

      while (line != NULL) {
         if (c_ptr != NULL) *c_ptr = '\0';
         XDrawString(mainDisplay, drawWindow, revDefaultGC, x, y,
               line, strlen(line));
         if (c_ptr != NULL) {
            *c_ptr++ = '\n';
            line = c_ptr;
            c_ptr = strchr(line, '\n');
            y += defaultFontHeight;
         } else {
            break;
         }
      }
   }
}

void ShowMeasureCursor(XOff, YOff, Str, ExtraSpace)
   int XOff, YOff, ExtraSpace;
   char *Str;
{
   if (!showMeasurement) return;
   if (Str == NULL || *Str == '\0') return;

   if (showMeasurementInTooltip) {
      if (measureTooltipVerbose) {
         switch (curChoice) {
         case DRAWARC:
         case DRAWEDGEARC:
         case FREEHAND:
         case ROTATEMODE:
            SetMeasureTooltip(Str);
            break;
         }
      } else {
         SetMeasureTooltip(Str);
      }
   } else {
      int x=(ExtraSpace ? XOff+18 : XOff+4), y=YOff+defaultFontAsc;
      char *c_ptr=strchr(Str, '\n'), *line=Str;

      while (line != NULL) {
         if (c_ptr != NULL) *c_ptr = '\0';
         XDrawString(mainDisplay, drawWindow, revDefaultGC, x, y,
               line, strlen(line));
         if (c_ptr != NULL) {
            *c_ptr++ = '\n';
            line = c_ptr;
            c_ptr = strchr(line, '\n');
            y += defaultFontHeight;
         } else {
            break;
         }
      }
   }
}

void EndShowMeasureCursor(XOff, YOff, Str, ExtraSpace)
   int XOff, YOff, ExtraSpace;
   char *Str;
{
   if (!showMeasurement) return;
   if (Str == NULL || *Str == '\0') return;

   if (showMeasurementInTooltip) {
      if (!measureTooltipVerbose) {
         StartMeasureTooltip(Str);
      }
   } else {
      int x=(ExtraSpace ? XOff+18 : XOff+4), y=YOff+defaultFontAsc;
      char *c_ptr=strchr(Str, '\n'), *line=Str;

      while (line != NULL) {
         if (c_ptr != NULL) *c_ptr = '\0';
         XDrawString(mainDisplay, drawWindow, revDefaultGC, x, y,
               line, strlen(line));
         if (c_ptr != NULL) {
            *c_ptr++ = '\n';
            line = c_ptr;
            c_ptr = strchr(line, '\n');
            y += defaultFontHeight;
         } else {
            break;
         }
      }
   }
   UnFreezeMarkRulerText();
}

void ToggleShowMeasurement()
{
   showMeasurement = !showMeasurement;
   RedrawRulers();
   if (showMeasurement) {
      Msg(TgLoadString(STID_SHOW_MEASUREMENT_ENABLED));
   } else {
      Msg(TgLoadString(STID_SHOW_MEASUREMENT_DISABLED));
   }
}

void ToggleShowMeasurementInTooltip()
{
   showMeasurementInTooltip = !showMeasurementInTooltip;
   RedrawRulers();
   if (showMeasurementInTooltip) {
      Msg(TgLoadString(STID_SHOW_MEASUREMENT_IN_TT_ENAB));
   } else {
      Msg(TgLoadString(STID_SHOW_MEASUREMENT_IN_TT_DISB));
   }
   EndMeasureTooltip(TRUE);
}

/* ------------------- Measure Tooltip Related Routines ------------------- */

/*
 * Some code in the rest of this file is based on the contributed code
 *       by Raphael Dechenaux <raph_d@club-internet.fr>.
 */

typedef struct tagMeasureTooltipInfo {
   int ltx, lty, w, h; /* this is for the window */
   int bbox_w, bbox_h; /* this is for the text */
   int x_padding, y_padding; /* padding around the text */
   int mapped;
   Window win;
   GC gc;
   /* configuration parameters -- how to display */
   int x_follow_mouse, y_follow_mouse, position_x, position_y;
} MeasureTooltipInfo;

static MeasureTooltipInfo gmti;

static Pixmap tooltipBgPixmap=None;

static
void CalcMeasureTooltipPosition()
{
   if (gmti.x_follow_mouse || gmti.y_follow_mouse) {
      Window root_win=None, child_win=None;
      unsigned int status;
      int root_x=0, root_y=0;

      XQueryPointer(mainDisplay, rootWindow, &root_win, &child_win,
            &root_x, &root_y, &gmti.ltx, &gmti.lty, &status);
      if (gmti.x_follow_mouse) gmti.ltx += 16;
      if (gmti.y_follow_mouse) gmti.lty += 16;
      if (gmti.x_follow_mouse && gmti.y_follow_mouse) {
         return;
      }
   }
   if (!gmti.x_follow_mouse) {
      int dpy_w=DisplayWidth(mainDisplay,mainScreen);

      switch (gmti.position_x) {
      case ALIGN_L: gmti.ltx = 0; break;
      case ALIGN_C: gmti.ltx = ((dpy_w-gmti.bbox_w-gmti.x_padding)>>1); break;
      case ALIGN_R: gmti.ltx = (dpy_w-gmti.bbox_w-(gmti.x_padding<<1)); break;
      default: gmti.ltx = 0; break;
      }
   }
   if (!gmti.y_follow_mouse) {
      int dpy_h=DisplayHeight(mainDisplay,mainScreen);

      switch (gmti.position_y) {
      case ALIGN_T: gmti.lty = 0; break;
      case ALIGN_M: gmti.lty = ((dpy_h-gmti.bbox_h-gmti.y_padding)>>1); break;
      case ALIGN_B: gmti.lty = (dpy_h-gmti.bbox_h-(gmti.y_padding<<1)); break;
      default: gmti.lty = 0; break;
      }
   }
}

static
void SetMeasureTooltipBBox(msg)
   char *msg;
{
   int bbox_w=0, bbox_h=0;
   char *psz=NULL, *line=NULL;

   bbox_h = ((msgFontSet==NULL && msgFontPtr==NULL) ? defaultFontHeight :
         msgFontHeight);
   line = msg;
   psz = strchr(line, '\n');
   while (line != NULL) {
      char saved_ch='\0';
      int len=0, w=0;

      if (psz != NULL) {
         saved_ch = (*psz);
         *psz = '\0';
      }
      len = strlen(line);
      if (msgFontSet == NULL && msgFontPtr == NULL) {
         w = (defaultFontWidth*len);
      } else {
         w = MsgTextWidth(msgFontPtr, line, len);
      }
      if (w > bbox_w) bbox_w = w;

      if (psz == NULL) {
         break;
      }
      *psz = saved_ch;
      line = &psz[1];
      psz = strchr(line, '\n');

      bbox_h += ((msgFontSet==NULL && msgFontPtr==NULL) ? defaultFontHeight :
            msgFontHeight);
   }
   gmti.bbox_w = bbox_w;
   gmti.bbox_h = bbox_h;
   gmti.w = bbox_w+((gmti.x_padding)<<1);
   gmti.h = bbox_h+((gmti.y_padding)<<1);

   CalcMeasureTooltipPosition();
}

void DoIntervalMeasureTooltip(ltx, lty, rbx, rby, mdx, mdy, delta_str)
   int ltx, lty, rbx, rby, mdx, mdy;
   char *delta_str;
{
   char w_buf[80], h_buf[80], buf[256];
   char lx_buf[80], cx_buf[80], rx_buf[80];
   char ty_buf[80], my_buf[80], by_buf[80];

   PixelToMeasurementUnit(w_buf, ABS_SIZE(abs(rbx-ltx)));
   PixelToMeasurementUnit(h_buf, ABS_SIZE(abs(rby-lty)));
   PixelToMeasurementUnit(lx_buf, ABS_X(ltx < rbx ? ltx : rbx));
   PixelToMeasurementUnit(cx_buf, ABS_X(mdx));
   PixelToMeasurementUnit(rx_buf, ABS_X(ltx > rbx ? ltx : rbx));
   PixelToMeasurementUnit(ty_buf, ABS_Y(lty < rby ? lty : rby));
   PixelToMeasurementUnit(my_buf, ABS_Y(mdy));
   PixelToMeasurementUnit(by_buf, ABS_Y(lty > rby ? lty : rby));
   if (delta_str != NULL) {
      sprintf(buf, "lx=%s cx=%s rx=%s\nty=%s my=%s by=%s\nw=%s h=%s\n%s",
            lx_buf, cx_buf, rx_buf, ty_buf, my_buf, by_buf, w_buf, h_buf,
            delta_str);
   } else {
      sprintf(buf, "lx=%s cx=%s rx=%s\nty=%s my=%s by=%s\nw=%s h=%s",
            lx_buf, cx_buf, rx_buf, ty_buf, my_buf, by_buf, w_buf, h_buf);
   }
   SetMeasureTooltip(buf);
}

void StartMeasureTooltip(msg)
   char *msg;
{
   SetMeasureTooltip(msg);
}

void SetMeasureTooltip(msg)
   char *msg;
{
   int x=0, y=0;
   char *psz=NULL, *line=NULL;

   if (!showMeasurementInTooltip) return;

   if (!gmti.mapped) {
      XMapWindow(mainDisplay, gmti.win);
      gmti.mapped = TRUE;
   }
   XRaiseWindow(mainDisplay, gmti.win);

   SetMeasureTooltipBBox(msg);
   XMoveResizeWindow(mainDisplay, gmti.win, gmti.ltx, gmti.lty, gmti.w, gmti.h);

   XClearWindow(mainDisplay, gmti.win);

   x = gmti.x_padding;
   y = gmti.y_padding + ((msgFontSet==NULL && msgFontPtr==NULL) ?
         defaultFontAsc : msgFontAsc);;
   line = msg;
   psz = strchr(line, '\n');
   while (line != NULL) {
      char saved_ch='\0';

      if (psz != NULL) {
         saved_ch = (*psz);
         *psz = '\0';
      }
      DrawMsgString(mainDisplay, gmti.win, gmti.gc, x, y, line, strlen(line));
      if (psz == NULL) {
         break;
      }
      *psz = saved_ch;
      line = &psz[1];
      psz = strchr(line, '\n');

      y += ((msgFontSet==NULL && msgFontPtr==NULL) ? defaultFontHeight :
            msgFontHeight);
   }
}

void EndMeasureTooltip(force)
   int force;
{
   if (force || (showMeasurement && showMeasurementInTooltip)) {
      if (gmti.mapped) {
         XUnmapWindow(mainDisplay, gmti.win);
         gmti.mapped = FALSE;
      }
   }
}

void CleanUpMeasureTooltip()
{
   if (tooltipBgPixmap != None) {
      XFreePixmap(mainDisplay, tooltipBgPixmap);
      tooltipBgPixmap = None;
   }
   if (gmti.gc != NULL) {
      XFreeGC(mainDisplay, gmti.gc);
   }
   if (gmti.win != None) {
      XDestroyWindow(mainDisplay, gmti.win);
   }
   memset(&gmti, 0, sizeof(MeasureTooltipInfo));
   showMeasurementInTooltip = FALSE;
   gmti.win = None;
}

static
int CreateMeasureTooltipWindow()
{
   int bg_pixel=(threeDLook ? myLtGryPixel : myBgPixel);
   XGCValues values;
   XWMHints wmhints;
   XSizeHints sizehints;
   XSetWindowAttributes win_attrs;

   if ((gmti.win=XCreateSimpleWindow(mainDisplay, rootWindow, 0, 0, 10, 10,
         1, myBorderPixel, bg_pixel)) == 0) {
      return FailToCreateWindowMessage("CreateMeasureTooltipWindow()", NULL,
            FALSE);
   }
   win_attrs.save_under = True;
   win_attrs.override_redirect = True;
   win_attrs.colormap = mainColormap;
   XChangeWindowAttributes(mainDisplay, gmti.win,
         CWSaveUnder | CWOverrideRedirect | CWColormap, &win_attrs);

   wmhints.flags = InputHint | StateHint;
   wmhints.input = True;
   wmhints.initial_state = NormalState;
   XSetWMHints(mainDisplay, gmti.win, &wmhints);

   sizehints.flags = PPosition | PSize | USPosition | PMinSize | PMaxSize;
   sizehints.x = 0;
   sizehints.y = 0;
   sizehints.width = sizehints.min_width = sizehints.max_width = 10;
   sizehints.height = sizehints.min_height = sizehints.max_height = 10;
#ifdef NOTR4MODE
   XSetNormalHints(mainDisplay, gmti.win, &sizehints);
#else
   XSetWMNormalHints(mainDisplay, gmti.win, &sizehints);
#endif

   XSetTransientForHint(mainDisplay, gmti.win, mainWindow);

   values.foreground = myFgPixel;
   values.background = bg_pixel;
   values.font = (msgFontPtr==NULL ? defaultFontPtr->fid : msgFontPtr->fid);
   gmti.gc = XCreateGC(mainDisplay, gmti.win,
         GCForeground | GCBackground | GCFont, &values);
   return TRUE;
}

static
void CreateLightYellowBackgroundPixmapForTooltip()
{
   XImage *image=NULL;

   tooltipBgPixmap = XCreatePixmap(mainDisplay, mainWindow, 2, 2, mainDepth);
   XSetForeground(mainDisplay, xpmGC, myYellowPixel);
   XFillRectangle(mainDisplay, tooltipBgPixmap, xpmGC, 0, 0, 2, 2);
   XSetForeground(mainDisplay, xpmGC, myFgPixel);
   image = XGetImage(mainDisplay, tooltipBgPixmap, 0, 0, 2, 2, AllPlanes,
         ZPixmap);
   XPutPixel(image, 0, 0, myWhitePixel);
   XPutPixel(image, 1, 1, myWhitePixel);
   XPutImage(mainDisplay, tooltipBgPixmap, xpmGC, image, 0, 0, 0, 0, 2, 2);
   XDestroyImage(image);

   XSetWindowBackgroundPixmap(mainDisplay, gmti.win, tooltipBgPixmap);
}

int InitMeasureTooltip()
{
   char *c_ptr=NULL;

   tooltipBgPixmap = None;

   memset(&gmti, 0, sizeof(MeasureTooltipInfo));
   gmti.win = None;
   gmti.mapped = FALSE;
   gmti.x_padding = 4;
   gmti.y_padding = 2;

   showMeasurementInTooltip = FALSE;
   if ((c_ptr=XGetDefault(mainDisplay, TOOL_NAME,
         "UseMeasureTooltip")) != NULL &&
         UtilStrICmp(c_ptr, "true") == 0) {
      showMeasurementInTooltip = TRUE;
   }
   measureTooltipVerbose = FALSE;
   if ((c_ptr=XGetDefault(mainDisplay, TOOL_NAME,
         "MeasureTooltipVerbose")) != NULL &&
         UtilStrICmp(c_ptr, "true") == 0) {
      measureTooltipVerbose = TRUE;
   }
   gmti.x_follow_mouse = gmti.y_follow_mouse = FALSE;
   if ((c_ptr=XGetDefault(mainDisplay, TOOL_NAME,
         "MeasureTooltipXFollowMouse")) != NULL &&
         UtilStrICmp(c_ptr, "true") == 0) {
      gmti.x_follow_mouse = TRUE;
   }
   if ((c_ptr=XGetDefault(mainDisplay, TOOL_NAME,
         "MeasureTooltipYFollowMouse")) != NULL &&
         UtilStrICmp(c_ptr, "true") == 0) {
      gmti.y_follow_mouse = TRUE;
   }
   gmti.position_y = ALIGN_T;
   if ((c_ptr=XGetDefault(mainDisplay, TOOL_NAME,
         "MeasureTooltipVerticalPosition")) != NULL) {
      if (UtilStrICmp(c_ptr, "top") == 0) {
         gmti.position_y = ALIGN_T;
      } else if (UtilStrICmp(c_ptr, "middle") == 0) {
         gmti.position_y = ALIGN_M;
      } else if (UtilStrICmp(c_ptr, "bottom") == 0) {
         gmti.position_y = ALIGN_B;
      } else {
         fprintf(stderr, TgLoadString(STID_INVALID_XDEF_USE_ALT_STR),
               TOOL_NAME, "MeasureTooltipVerticalPosition", c_ptr, "Top");
         fprintf(stderr, "\n");
      }
   }
   gmti.position_x = ALIGN_L;
   if ((c_ptr=XGetDefault(mainDisplay, TOOL_NAME,
         "MeasureTooltipHorizontalPosition")) != NULL) {
      if (UtilStrICmp(c_ptr, "left") == 0) {
         gmti.position_x = ALIGN_L;
      } else if (UtilStrICmp(c_ptr, "center") == 0) {
         gmti.position_x = ALIGN_C;
      } else if (UtilStrICmp(c_ptr, "right") == 0) {
         gmti.position_x = ALIGN_R;
      } else {
         fprintf(stderr, TgLoadString(STID_INVALID_XDEF_USE_ALT_STR),
               TOOL_NAME, "MeasureTooltipHorizontalPosition", c_ptr, "Left");
         fprintf(stderr, "\n");
      }
   }
   if (!CreateMeasureTooltipWindow()) {
      gmti.win = None;
   } else if (threeDLook) {
      CreateLightYellowBackgroundPixmapForTooltip();
   }
   return TRUE;
}
