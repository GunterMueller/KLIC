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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/grid.c,v 1.15 2011/05/16 16:21:57 william Exp $
 */

#define _INCLUDE_FROM_GRID_C_

#include "tgifdefs.h"
#include "cmdids.h"
#include "expfdefs.h"

#include "choice.e"
#include "color.e"
#include "cursor.e"
#include "dialog.e"
#include "drawing.e"
#include "dup.e"
#include "file.e"
#include "grid.e"
#include "mainmenu.e"
#include "menu.e"
#include "menuinfo.e"
#include "msg.e"
#include "navigate.e"
#include "obj.e"
#include "page.e"
#include "pattern.e"
#include "raster.e"
#include "ruler.e"
#include "scroll.e"
#include "select.e"
#include "setup.e"
#include "stretch.e"
#include "strtbl.e"
#include "text.e"
#include "util.e"
#include "wb.e"
#include "xprtfltr.e"

#define ENGLISH_GRID_STEP 8
#define METRIC_GRID_STEP 5

int	gridSystem = ENGLISH_GRID;
int	snapOn = TRUE;
int	xyEnglishGrid = DEFAULT_ENGLISH_GRID;
int	xyMetricGrid = DEFAULT_METRIC_GRID;
int	pageStyle = PORTRAIT;
int	whereToPrint = LATEX_FIG;
int	moveMode = UNCONST_MOVE;
int	gridShown = TRUE;
int	mapShown = TRUE;
int	usePaperSizeStoredInFile = FALSE;
int	oneMotionSelectMove = FALSE;
int	queryZoomInPoint=FALSE;

int shiftForDiagMouseMove=TRUE;
#ifdef RESTRICTED_MOUSE_MOVE
int useRecentForDiagMouseMove=TRUE;
#else /* ~RESTRICTED_MOUSE_MOVE */
int useRecentForDiagMouseMove=FALSE;
#endif /* RESTRICTED_MOUSE_MOVE */

int	defRightMargin=(ONE_INCH*TIK_PER_PIXEL);
int	defRightMarginEnabled=INVALID;
int	rightMargin=(ONE_INCH*TIK_PER_PIXEL);
int	rightMarginEnabled=INVALID;
int	rightMarginActive=TRUE;

static
void MyHDotLine(Win, gc, Y, XStart, XEnd)
   Window Win;
   GC gc;
   int Y, XStart, XEnd;
{
   int x, step=0;

   switch (gridSystem) {
   case ENGLISH_GRID: step = ENGLISH_GRID_STEP; break;
   case METRIC_GRID: step = METRIC_GRID_STEP; break;
   }
   for (x=XStart; x < XEnd; x += step) {
      PUT_A_POINT(mainDisplay, Win, gc, x, Y);
   }
}

static
void MyVDotLine(Win, gc, X, YStart, YEnd)
   Window Win;
   GC gc;
   int X, YStart, YEnd;
{
   int y, step=0;

   switch (gridSystem) {
   case ENGLISH_GRID: step = ENGLISH_GRID_STEP; break;
   case METRIC_GRID: step = METRIC_GRID_STEP; break;
   }
   for (y=YStart; y < YEnd; y += step) {
      PUT_A_POINT(mainDisplay, Win, gc, X, y);
   }
}

#define FAKE_CM 80

static
void DrawGridLinesSetVars(pn_inc, pn_step, pn_abs_inc)
   int *pn_inc, *pn_step, *pn_abs_inc;
{
   switch (gridSystem) {
   case ENGLISH_GRID:
      *pn_inc = HALF_INCH;
      *pn_step = ENGLISH_GRID_STEP;
      break;
   case METRIC_GRID:
      if (zoomedIn && zoomScale > 1) {
         *pn_inc = (FAKE_CM>>1);
      } else {
         *pn_inc = ONE_CM;
      }
      *pn_step = METRIC_GRID_STEP;
      break;
   }
   *pn_abs_inc = ABS_SIZE(*pn_inc);
}

static
void PrepareDrawGCForGridPoints()
{
   XGCValues values;

   values.foreground = myFgPixel;
   values.function = GXcopy;
   values.fill_style = FillSolid;
   values.line_width = 1;
   values.line_style = LineSolid;
   XChangeGC(mainDisplay, drawGC,
         GCForeground | GCFunction | GCFillStyle | GCLineWidth | GCLineStyle,
         &values);
}

void RedrawGridLines(Win)
   Window Win;
{
   int i, inc=0, abs_inc=0, step=ENGLISH_GRID_STEP;
   int x_start, y_start, x_end, y_end, x_grid_start, y_grid_start;

   if (!gridShown || (inSlideShow && !visibleGridInSlideShow)) return;

   DrawGridLinesSetVars(&inc, &step, &abs_inc);

   if (drawOrigX % abs_inc == 0) {
      x_start = 0;
   } else {
      x_start = OFFSET_X(((int)(drawOrigX / abs_inc) + 1) * abs_inc);
   }
   if (drawOrigY % abs_inc == 0) {
      y_start = 0;
   } else {
      y_start = OFFSET_Y(((int)(drawOrigY / abs_inc) + 1) * abs_inc);
   }
   x_end = min(OFFSET_X(paperWidth), OFFSET_X(drawOrigX+drawWinW));
   y_end = min(OFFSET_Y(paperHeight), OFFSET_Y(drawOrigY+drawWinH));

   if (ZOOMED_SIZE(drawOrigX) % step == 0) {
      x_grid_start = 0;
   } else {
      x_grid_start = ((int)(ZOOMED_SIZE(drawOrigX) / step)) * step -
            ZOOMED_SIZE(drawOrigX);
   }
   if (ZOOMED_SIZE(drawOrigY) % step == 0) {
      y_grid_start = 0;
   } else {
      y_grid_start = ((int)(ZOOMED_SIZE(drawOrigY) / step)) * step -
            ZOOMED_SIZE(drawOrigY);
   }
   PrepareDrawGCForGridPoints();
   for (i=x_start; i < x_end; i += inc) {
      MyVDotLine(Win, drawGC, i, y_grid_start, y_end);
   }
   for (i=y_start; i < y_end; i += inc) {
      MyHDotLine(Win, drawGC, i, x_grid_start, x_end);
   }
}

void DrawGridLines(Win, LtX, LtY, W, H)
   Window Win;
   int  LtX, LtY, W, H; /* screen offsets */
{
   int i, inc=0, abs_inc=0, step=ENGLISH_GRID_STEP;
   int x_start, y_start, x_end, y_end, x_grid_start, y_grid_start;
   int x_grid_end=0, y_grid_end=0, x_max=0, y_max=0;

   if (!gridShown || (inSlideShow && !visibleGridInSlideShow)) return;

   DrawGridLinesSetVars(&inc, &step, &abs_inc);

   /* large divisions - use 'inc' */
   if (drawOrigX % abs_inc == 0) {
      x_start = 0;
   } else {
      x_start = OFFSET_X(((int)(drawOrigX / abs_inc) + 1) * abs_inc);
   }
   if (drawOrigY % abs_inc == 0) {
      y_start = 0;
   } else {
      y_start = OFFSET_Y(((int)(drawOrigY / abs_inc) + 1) * abs_inc);
   }
   x_max = min(OFFSET_X(paperWidth), OFFSET_X(drawOrigX+drawWinW));
   y_max = min(OFFSET_Y(paperHeight), OFFSET_Y(drawOrigY+drawWinH));
   while (x_start+1 < LtX) x_start += inc;
   while (y_start+1 < LtY) y_start += inc;
   for (x_end=x_start; x_end < LtX+W+1; x_end += inc) {
      if (x_end >= x_max) {
         break;
      }
   }
   x_end -= inc;
   for (y_end=y_start; y_end < LtY+H+1; y_end += inc) {
      if (y_end >= y_max) {
         break;
      }
   }
   y_end -= inc;
   /* small divisions - use 'step' */
   x_grid_start = x_start-inc;
   y_grid_start = y_start-inc;
   while (x_grid_start+1 < LtX) x_grid_start += step;
   while (y_grid_start+1 < LtY) y_grid_start += step;
   for (x_grid_end=x_end+step; x_grid_end < LtX+W+1; x_grid_end += step) {
      if (x_grid_end >= x_max) {
         break;
      }
   }
   for (y_grid_end=y_end+step; y_grid_end < LtY+H+1; y_grid_end += step) {
      if (y_grid_end >= y_max) {
         break;
      }
   }
   PrepareDrawGCForGridPoints();
   for (i=x_start; i <= x_end; i += inc) {
      MyVDotLine(Win, drawGC, i, y_grid_start, y_grid_end);
   }
   for (i=y_start; i <= y_end; i += inc) {
      MyHDotLine(Win, drawGC, i, x_grid_start, x_grid_end);
   }
}

void RedrawPageLines(Win)
   Window Win;
{
   register int i, x, y;
   int end;

   if (pageLayoutMode == PAGE_STACK || !pageLineShownInTileMode) return;

   PrepareDrawGCForGridPoints();
   end = OFFSET_Y(min(drawOrigY+drawWinH,paperHeight));
   for (i=0; i < paperWidth; i += onePageWidth) {
      x = OFFSET_X(i);
      if (i >= drawOrigX && i < drawOrigX+drawWinW) {
         XDrawLine(mainDisplay, Win, drawGC, x, 0, x, end);
      }
      if (i > drawOrigX+drawWinW) {
         break;
      }
   }
   end = OFFSET_X(min(drawOrigX+drawWinW,paperWidth));
   for (i=0; i < paperHeight; i += onePageHeight) {
      y = OFFSET_Y(i);
      if (i >= drawOrigY && i < drawOrigY+drawWinH) {
         XDrawLine(mainDisplay, Win, drawGC, 0, y, end, y);
      }
      if (i > drawOrigY+drawWinH) {
         break;
      }
   }
}

void DrawPageLines(Win, LtX, LtY, W, H)
   Window Win;
   int  LtX, LtY, W, H; /* screen offsets */
{
   int i, x, y, start, end;

   if (pageLayoutMode == PAGE_STACK || !pageLineShownInTileMode) return;

   PrepareDrawGCForGridPoints();
   start = max(0,LtY);
   end = min(LtY+H,OFFSET_Y(min(drawOrigY+drawWinH,paperHeight)));
   if (end > start) {
      for (i=0; i < paperWidth; i += onePageWidth) {
         x = OFFSET_X(i);
         if (x >= LtX && x < LtX+W) {
            XDrawLine(mainDisplay, Win, drawGC, x, start, x, end);
         }
         if (i > drawOrigX+drawWinW) {
            break;
         }
      }
   }
   start = max(0,LtX);
   end = min(LtX+W,OFFSET_X(min(drawOrigX+drawWinW,paperWidth)));
   if (end > start) {
      for (i=0; i < paperHeight; i += onePageHeight) {
         y = OFFSET_Y(i);
         if (y >= LtY && y < LtY+H) {
            XDrawLine(mainDisplay, Win, drawGC, start, y, end, y);
         }
         if (i > drawOrigY+drawWinH) {
            break;
         }
      }
   }
}

static
void GetRightMarginSpec(buf)
   char *buf;
{
   double dval=(double)0;

   dval = ((double)rightMargin) * ((double)printMag) /
         ((double)TIK_PER_PIXEL);
   dval /= ((double)100);
   PixelToCurrentUnit(buf, round(dval));
}

void ToggleRightMargin()
{
   switch (rightMarginEnabled) {
   case INVALID: rightMarginEnabled = TRUE; break;
   case FALSE: rightMarginEnabled = TRUE; break;
   case TRUE: rightMarginEnabled = FALSE; break;
   }
   SetFileModified(TRUE);
   RedrawHRulerWindow();

   if (rightMarginEnabled == TRUE) {
      char buf[80];

      *buf = '\0';
      GetRightMarginSpec(buf);
      sprintf(gszMsgBox, TgLoadString(STID_RIGHT_MARGIN_IS), buf);
      Msg(gszMsgBox);
   } else {
      sprintf(gszMsgBox, TgLoadString(STID_RIGHT_MARGIN_DISABLED));
      Msg(gszMsgBox);
   }
}

void SpecifyRightMargin()
{
   char buf[MAXSTRING+1], spec[MAXSTRING+1];
   double dval=(double)0;

   *gszMsgBox = '\0';
   GetRightMarginSpec(gszMsgBox);

   sprintf(buf, TgLoadString(STID_CURRENT_RIGHT_MARGIN_IS), gszMsgBox);
   strcpy(gszMsgBox, TgLoadString(STID_ENTER_RIGHT_MARGIN));
   *spec = '\0';
   Dialog(gszMsgBox, buf, spec);
   UtilTrimBlanks(spec);
   if (*spec == '\0') return;
   if (GetDimensionInDouble(spec, TRUE, &dval)) {
      dval = dval * ((double)100) * ((double)TIK_PER_PIXEL) /
            ((double)printMag);
      rightMargin = round(dval);
      RedrawHRulerWindow();
      SetFileModified(TRUE);

      GetRightMarginSpec(buf);
      sprintf(gszMsgBox, TgLoadString(STID_RIGHT_MARGIN_IS), buf);
      Msg(gszMsgBox);
   }
}

void ToggleGridSystem()
{
   int saved_orig_x, saved_orig_y;

   switch (gridSystem) {
   case ENGLISH_GRID: Msg(TgLoadString(STID_USING_METRIC_SYSTEM)); break;
   case METRIC_GRID: Msg(TgLoadString(STID_USING_ENGLISH_SYSTEM)); break;
   }
   gridSystem = !gridSystem;
   if (drawOrigX != 0 || drawOrigY != 0) {
      saved_orig_x = drawOrigX;
      saved_orig_y = drawOrigY;
      drawOrigX = drawOrigY = 0;
      RedrawScrollBars();
      UpdDrawWinBBox();
      AdjSplineVs();
      AdjustCurText(-(saved_orig_x), -(saved_orig_y));
   }
   ClearAndRedrawDrawWindow();
   RedrawRulers();
   UpdatePinnedMenu(MENU_LAYOUT);
}

void IncGrid()
{
   if (!snapOn) {
      Msg(TgLoadString(STID_SNAP_NOT_ON_GRID_SZ_SAME));
   } else if (gridSystem == ENGLISH_GRID && xyEnglishGrid < HALF_INCH) {
      xyEnglishGrid <<= 1;
      RedrawRulers();
   } else if (gridSystem == METRIC_GRID && xyMetricGrid < ONE_CM) {
      switch (xyMetricGrid) {
      case ONE_MM: xyMetricGrid = TWO_MM; break;
      case TWO_MM: xyMetricGrid = FIVE_MM; break;
      case FIVE_MM: xyMetricGrid = ONE_CM; break;
      }
      RedrawRulers();
   } else {
      Msg(TgLoadString(STID_AT_MAX_GRID_GRID_SZ_SAME));
   }
}

void DecGrid()
{
   if (!snapOn) {
      Msg(TgLoadString(STID_SNAP_NOT_ON_GRID_SZ_SAME));
   } else if (gridSystem == ENGLISH_GRID && xyEnglishGrid > 4) {
      xyEnglishGrid >>= 1;
      RedrawRulers();
   } else if (gridSystem == METRIC_GRID && xyMetricGrid > ONE_MM) {
      switch (xyMetricGrid) {
      case TWO_MM: xyMetricGrid = ONE_MM; break;
      case FIVE_MM: xyMetricGrid = TWO_MM; break;
      case ONE_CM: xyMetricGrid = FIVE_MM; break;
      }
      RedrawRulers();
   } else {
      Msg(TgLoadString(STID_AT_MIN_GRID_GRID_SZ_SAME));
   }
}

void ToggleGridShown()
{
   gridShown = !gridShown;
   ClearAndRedrawDrawWindow();
   UpdatePinnedMenu(MENU_LAYOUT);
}

void ToggleSnapOn()
{
   snapOn = !snapOn;
   if (snapOn) {
      Msg(TgLoadString(STID_SNAP_TO_GRID_ACTV));
   } else {
      Msg(TgLoadString(STID_SNAP_TO_GRID_DISABLED));
   }
   RedrawRulers();
   UpdatePinnedMenu(MENU_LAYOUT);
}

void ToggleColorPostScript()
{
   colorDump = !colorDump;
   if (colorDump) {
      Msg(TgLoadString(STID_WILL_PRINT_IN_COLOR));
   } else {
      Msg(TgLoadString(STID_WILL_PRINT_IN_BW));
   }
   ShowWhereToPrint();
   SetFileModified(TRUE);
   UpdatePinnedMenu(MENU_LAYOUT);
   UpdatePinnedMenu(MENU_FILE);
}

void ToggleMoveMode()
{
   moveMode = !moveMode;
   switch (moveMode) {
   case CONST_MOVE: Msg(TgLoadString(STID_CONSTRAINED_MOVE)); break;
   case UNCONST_MOVE: Msg(TgLoadString(STID_UNCONSTRAINED_MOVE)); break;
   }
   ShowMoveMode();
   UpdatePinnedMenu(MENU_LAYOUT);
   UpdatePinnedMenu(MENU_MOVEMODE);
}

void ToggleMapShown()
{
   mapShown = !mapShown;
   if (mapShown) {
      Msg(TgLoadString(STID_WILL_DISPLAY_XBM_XPM));
   } else {
      Msg(TgLoadString(STID_WILL_NOT_DISPLAY_XBM_XPM));
   }
   ClearAndRedrawDrawWindow();
   UpdatePinnedMenu(MENU_LAYOUT);
}

void ToggleUseGray()
{
   useGray = !useGray;
   if (useGray) {
      Msg(TgLoadString(STID_GRAY_SCALE_IN_BW_PRINTING));
   } else {
      Msg(TgLoadString(STID_NO_GRAY_SCALE_IN_BW_PRINTING));
   }
   SetFileModified(TRUE);
   UpdatePinnedMenu(MENU_LAYOUT);
}

void SetMeasureUnit()
{
   char buf[MAXSTRING+1], spec[MAXSTRING+1];

   GetUnitSpec(gszMsgBox);
   sprintf(buf, TgLoadString(STID_CURRENT_SPEC_IS), gszMsgBox);
   strcpy(gszMsgBox, TgLoadString(STID_ENTER_MEASURE_UNIT_SPEC));
   *spec = '\0';
   Dialog(gszMsgBox, buf, spec);
   UtilTrimBlanks(spec);
   if (*spec == '\0') return;
   if (SetUnit(spec)) {
      RedrawRulers();
      ShowUnitMsg();
      SetFileModified(TRUE);
   }
}

void ToggleShowMenubar()
{
   noMenubar = !noMenubar;
   if (noMenubar) {
      XUnmapWindow(mainDisplay, menubarWindow);
   } else {
      XMapWindow(mainDisplay, menubarWindow);
   }
   Reconfigure(TRUE);
   UpdatePinnedMenu(MENU_LAYOUT);
}

void ToggleShowStatus()
{
   noStatusWindow = !noStatusWindow;
   if (noStatusWindow) {
      XUnmapWindow(mainDisplay, userRedrawWindow);
      XUnmapWindow(mainDisplay, statusWindow);
   } else {
      XMapWindow(mainDisplay, userRedrawWindow);
      XMapWindow(mainDisplay, statusWindow);
   }
   Reconfigure(TRUE);
   UpdatePinnedMenu(MENU_LAYOUT);
}

void ToggleShowChoicebar()
{
   noChoiceWindow = !noChoiceWindow;
   if (noChoiceWindow) {
      XUnmapWindow(mainDisplay, choiceWindow);
      XUnmapWindow(mainDisplay, msgWindow);
   } else {
      XMapWindow(mainDisplay, choiceWindow);
      XMapWindow(mainDisplay, msgWindow);
   }
   Reconfigure(TRUE);
   UpdatePinnedMenu(MENU_LAYOUT);
}

void ToggleShowMode()
{
   noModeWindow = !noModeWindow;
   if (noModeWindow) {
      XUnmapWindow(mainDisplay, modeWindow);
   } else {
      XMapWindow(mainDisplay, modeWindow);
   }
   Reconfigure(TRUE);
   UpdatePinnedMenu(MENU_LAYOUT);
}

static
void WhereToPrintMsg()
{
   if (whereToPrint >= MAXDEFWHERETOPRINT) {
      sprintf(gszMsgBox, TgLoadString(STID_WILL_EXPORT_FORMAT_FILE),
            GetExportName(whereToPrint, EXPORT_THIS));
      Msg(gszMsgBox);
   } else {
      switch (whereToPrint) {
      case PRINTER: Msg(TgLoadString(STID_PRINT_DEV_SET_TO_PRINTER)); break;
      case LATEX_FIG: Msg(TgLoadString(STID_WILL_EXPORT_EPS_FILE)); break;
      case PS_FILE: Msg(TgLoadString(STID_WILL_EXPORT_RAW_PS_FILE)); break;
      case XBM_FILE:
         Msg(TgLoadString(colorDump ? STID_WILL_EXPORT_XPM_FILE :
               STID_WILL_EXPORT_XBM_FILE));
         break;
      case TEXT_FILE: Msg(TgLoadString(STID_WILL_EXPORT_TEXT_FILE)); break;
      case EPSI_FILE: Msg(TgLoadString(STID_WILL_EXPORT_EPSI_FILE)); break;
      case GIF_FILE: Msg(TgLoadString(STID_WILL_EXPORT_GIF_FILE)); break;
      case HTML_FILE: Msg(TgLoadString(STID_WILL_EXPORT_HTML_FILE)); break;
      case PDF_FILE: Msg(TgLoadString(STID_WILL_EXPORT_PDF_FILE)); break;
      case TIFFEPSI_FILE:
         Msg(TgLoadString(STID_WILL_EXPORT_TIFFEPSI_FILE));
         break;
      case PNG_FILE: Msg(TgLoadString(STID_WILL_EXPORT_PNG_FILE)); break;
      case JPEG_FILE: Msg(TgLoadString(STID_WILL_EXPORT_JPEG_FILE)); break;
      case PPM_FILE:
         Msg(TgLoadString(colorDump ? STID_WILL_EXPORT_PPM_FILE :
               STID_WILL_EXPORT_PBM_FILE));
         break;
      case NETLIST_FILE:
         Msg(TgLoadString(STID_WILL_EXPORT_NETLIST_FILE));
         break;
      case SVG_FILE: Msg(TgLoadString(STID_WILL_EXPORT_SVG_FILE)); break;
      }
   }
}

void SetWhereToPrint(nIndex)
   int nIndex;
{
   whereToPrint = nIndex;
   WhereToPrintMsg();
   ShowWhereToPrint();
   UpdatePinnedMenu(MENU_LAYOUT);
   UpdatePinnedMenu(MENU_FILE);
}

void ToggleWhereToPrint()
{
   int nIndex=0;
   char szLeft[MAXSTRING], szRight[MAXSTRING];

   whereToPrint = (whereToPrint+1) % maxWhereToPrint;
   WhereToPrintMsg();
   ShowWhereToPrint();
   UpdatePinnedMenu(MENU_FILE);

   nIndex = GetExportIndex(whereToPrint, EXPORT_NEXT);
   if (nIndex == PRINTER) {
      sprintf(szLeft, TgLoadCachedString(CSTID_PRINT_MODE),
            GetExportName(whereToPrint, EXPORT_NEXT));
   } else {
      sprintf(szLeft, TgLoadCachedString(CSTID_EXPORT_MODE),
            GetExportName(whereToPrint, EXPORT_NEXT));
   }
   nIndex = GetExportIndex(whereToPrint, EXPORT_PREV);
   if (nIndex == PRINTER) {
      sprintf(szRight, TgLoadCachedString(CSTID_PRINT_MODE),
            GetExportName(whereToPrint, EXPORT_PREV));
   } else {
      sprintf(szRight, TgLoadCachedString(CSTID_EXPORT_MODE),
            GetExportName(whereToPrint, EXPORT_PREV));
   }
   SetMouseStatus(szLeft, TgLoadCachedString(CSTID_LAYOUT_MENU), szRight);
}

void ToggleOneMotionSelectMove()
{
   oneMotionSelectMove = !oneMotionSelectMove;
   if (oneMotionSelectMove) {
      Msg(TgLoadString(STID_ONE_MOTION_SEL_MOVE_MODE));
   } else {
      Msg(TgLoadString(STID_CLICK_SEL_CLICK_MOVE_MODE));
   }
   UpdatePinnedMenu(MENU_LAYOUT);
}

void ToggleColorLayers()
{
   if (!colorDisplay) {
      MsgBox(TgLoadString(STID_CANT_TOGGLE_COLOR_LAYER_BW), TOOL_NAME, INFO_MB);
      return;
   }
   colorLayers = !colorLayers;
   if (colorLayers) {
      XMapWindow(mainDisplay, colorWindow);
      XMapWindow(mainDisplay, colorDummyWindow);
      Msg(TgLoadString(STID_COLOR_LAYERS_ENABLED));
   } else {
      XUnmapWindow(mainDisplay, colorWindow);
      XUnmapWindow(mainDisplay, colorDummyWindow);
      Msg(TgLoadString(STID_COLOR_LAYERS_DISABLED));
   }
   Reconfigure(TRUE);
   UpdatePinnedMenu(MENU_LAYOUT);
   ClearAndRedrawDrawWindow();
}

void ToggleStretchableText()
{
   stretchableText = !stretchableText;
   if (stretchableText) {
      Msg(TgLoadString(STID_STRETCHABLE_TEXT_MODE));
   } else {
      Msg(TgLoadString(STID_NON_STRETCHABLE_TEXT_MODE));
   }
   ShowStretchableTextMode();
   UpdatePinnedMenu(MENU_STRETCHTEXT);
}

void ToggleTransPatMode()
{
   transPat = !transPat;
   if (transPat) {
      Msg(TgLoadString(STID_TRANS_PAT_MODE));
   } else {
      Msg(TgLoadString(STID_NON_TRANS_PAT_MODE));
   }
   ShowTransPatMode();
   UpdatePinnedMenu(MENU_TRANSPAT);
}

static
void PostZoom(xc, yc)
   int xc, yc;
{
   int new_orig_x=0, new_orig_y=0;

   UpdDrawWinWH();

   new_orig_x = ((xc<<1)-drawWinW)>>1;
   new_orig_y = ((yc<<1)-drawWinH)>>1;

   drawOrigX = max(0,new_orig_x);
   drawOrigY = max(0,new_orig_y);

   if (drawOrigX>=0 && drawWinW<paperWidth && drawOrigX+drawWinW>=paperWidth) {
      switch (gridSystem) {
      case ENGLISH_GRID:
         if ((paperWidth-drawWinW) % ABS_SIZE(HALF_INCH) == 0) {
            drawOrigX = paperWidth-drawWinW;
         } else {
            drawOrigX = max(0, ((int)((paperWidth-drawWinW)/
                  ABS_SIZE(HALF_INCH)) + 1) * ABS_SIZE(HALF_INCH));
         }
         break;
      case METRIC_GRID:
         if (zoomedIn && zoomScale > 1) {
            if ((paperWidth-drawWinW) % ABS_SIZE(FAKE_CM) == 0) {
               drawOrigX = paperWidth-drawWinW;
            } else {
               drawOrigX = max(0, ((int)((paperWidth-drawWinW)/
                     ABS_SIZE(FAKE_CM)) + 1) * ABS_SIZE(FAKE_CM));
            }
         } else {
            if ((paperWidth-drawWinW) % ABS_SIZE(ONE_CM) == 0) {
               drawOrigX = paperWidth-drawWinW;
            } else {
               drawOrigX = max(0, ((int)((paperWidth-drawWinW)/
                     ABS_SIZE(ONE_CM)) + 1) * ABS_SIZE(ONE_CM));
            }
         }
         break;
      }
   } else if (drawOrigX < 0 || drawWinW >= paperWidth) {
      drawOrigX = 0;
   }
   if (drawOrigY>=0 && drawWinH<paperHeight &&
         drawOrigY+drawWinH>=paperHeight) {
      switch (gridSystem) {
      case ENGLISH_GRID:
         if ((paperHeight-drawWinH) % ABS_SIZE(HALF_INCH) == 0) {
            drawOrigY = paperHeight-drawWinH;
         } else {
            drawOrigY = max(0, ((int)((paperHeight-drawWinH)/
                  ABS_SIZE(HALF_INCH)) + 1) * ABS_SIZE(HALF_INCH));
         }
         break;
      case METRIC_GRID:
         if (zoomedIn && zoomScale > 1) {
            if ((paperHeight-drawWinH) % ABS_SIZE(FAKE_CM) == 0) {
               drawOrigY = paperHeight-drawWinH;
            } else {
               drawOrigY = max(0, ((int)((paperHeight-drawWinH)/
                     ABS_SIZE(FAKE_CM)) + 1) * ABS_SIZE(FAKE_CM));
            }
         } else {
            if ((paperHeight-drawWinH) % ABS_SIZE(ONE_CM) == 0) {
               drawOrigY = paperHeight-drawWinH;
            } else {
               drawOrigY = max(0, ((int)((paperHeight-drawWinH)/
                     ABS_SIZE(ONE_CM)) + 1) * ABS_SIZE(ONE_CM));
            }
         }
         break;
      }
   } else if (drawOrigY < 0 || drawWinH >= paperHeight) {
      drawOrigY = 0;
   }
   drawOrigX = ABS_SIZE(ZOOMED_SIZE(drawOrigX));
   drawOrigY = ABS_SIZE(ZOOMED_SIZE(drawOrigY));

   UpdDrawWinBBox();

   SetWatchCursor(drawWindow);
   SetWatchCursor(mainWindow);
   RedrawRulers();
   RedrawScrollBars();
   AdjSplineVs();
   AdjCaches();
   ShowZoom();
   /* use to be ClearAndRedrawDrawWindowDontDrawCurText() */
   ClearAndRedrawDrawWindowNoCurT();
   SetDefaultCursor(mainWindow);
   ShowCursor();

   justDupped = FALSE;
   if (!(curChoice==DRAWTEXT && textCursorShown)) {
      ShowCurChoiceMouseStatus(curChoice, 0, FALSE);
   }
}

void DefaultZoom()
{
   int xc, yc;

   if (zoomScale == 0) {
      Msg(TgLoadString(STID_ALREADY_AT_DEF_ZOOM));
      return;
   }
   if (curChoice==DRAWTEXT && textCursorShown) {
      PrepareZoomCurText(&xc, &yc);
   } else {
      xc = (topSel==NULL) ? ((drawOrigX<<1)+drawWinW)>>1 : (selLtX+selRbX)>>1;
      yc = (topSel==NULL) ? ((drawOrigY<<1)+drawWinH)>>1 : (selLtY+selRbY)>>1;
   }
   if (curChoice == VERTEXMODE) {
      TieLooseEnds();
      SetCurChoice(NOTHING);
   }
   zoomScale = 0;
   zoomedIn = FALSE;

   if (curChoice==DRAWTEXT && textCursorShown) PreZoomCurText();
   PostZoom(xc, yc);
   if (curChoice==DRAWTEXT && textCursorShown) PostZoomCurText(xc, yc);
}

static int zoomInAbsX=0, zoomInAbsY=0;
static int zoomInAtCursor=FALSE;
static int panAtCursor=FALSE;

static
void FixUpZoomCenter(p_abs_cx, p_abs_cy, zooming_in)
   int *p_abs_cx, *p_abs_cy, zooming_in;
{
   int saved_zoomed_in=zoomedIn, saved_zoom_scale=zoomScale;
   int anchor_x_off=OFFSET_X(*p_abs_cx), anchor_y_off=OFFSET_Y(*p_abs_cy);
   int new_anchor_abs_x=0, new_anchor_abs_y=0;
   int abs_dx=0, abs_dy=0, new_abs_cx=0, new_abs_cy=0;

   switch (zooming_in) {
   case INVALID: break;
   case FALSE:
      if (!zoomedIn) {
         zoomScale++;
      } else if (zoomScale == 1) {
         zoomedIn = FALSE;
         zoomScale--;
      } else {
         zoomScale--;
      }
      break;
   default:
      if (zoomedIn) {
         zoomScale++;
      } else if (zoomScale == 0) {
         zoomedIn = TRUE;
         zoomScale++;
      } else {
         zoomScale--;
      }
      break;
   }
   UpdDrawWinWH();

   new_anchor_abs_x = ABS_X(anchor_x_off);
   new_anchor_abs_y = ABS_Y(anchor_y_off);

   new_abs_cx = (((drawOrigX<<1)+drawWinW)>>1);
   new_abs_cy = (((drawOrigY<<1)+drawWinH)>>1);

   abs_dx = new_abs_cx - new_anchor_abs_x;
   abs_dy = new_abs_cy - new_anchor_abs_y;

   if (zooming_in) {
      *p_abs_cx += (abs_dx);
      *p_abs_cy += (abs_dy);
   } else {
      *p_abs_cx += (abs_dx);
      *p_abs_cy += (abs_dy);
   }
   zoomedIn = saved_zoomed_in;
   zoomScale = saved_zoom_scale;
   UpdDrawWinWH();
}

void ZoomIn()
{
   int xc=0, yc=0, need_to_restore_cur_choice=FALSE;

   if (zoomedIn && zoomScale == MAX_ZOOMED_IN &&
         !(zoomInAtCursor && panAtCursor)) {
      MsgBox(TgLoadString(STID_ALREADY_AT_HIGHEST_MAG), TOOL_NAME, INFO_MB);
      return;
   }
   if (curChoice==DRAWTEXT && textCursorShown) {
      if (zoomInAtCursor) {
         sprintf(gszMsgBox, "    %s", TgLoadString(STID_CURSOR_IGNORED_CSBTN1));
         TwoLineMsg(TgLoadString(STID_ZOOMIN_AROUND_EDIT_TEXT), gszMsgBox);
      }
      PrepareZoomCurText(&xc, &yc);
   } else if (zoomInAtCursor) {
      xc = zoomInAbsX;
      yc = zoomInAbsY;
      if (!panAtCursor) {
         FixUpZoomCenter(&xc, &yc, TRUE);
      }
   } else if (queryZoomInPoint==TRUE || (queryZoomInPoint==INVALID &&
         topSel==NULL)) {
      if (queryZoomInPoint==INVALID && topSel==NULL) {
         MakeQuiescent();
         need_to_restore_cur_choice = TRUE;
      }
      SetMouseStatus(TgLoadString(STID_SEL_ZOOM_CENTER),
            TgLoadCachedString(CSTID_CANCEL),
            TgLoadCachedString(CSTID_CANCEL));
      if (DrawWindowLoop(&xc, &yc, magCursor, FALSE) != Button1) {
         Msg(TgLoadString(STID_ZOOMIN_CANCEL_BY_USER));
         return;
      }
      xc = ABS_X(xc);
      yc = ABS_Y(yc);
      FixUpZoomCenter(&xc, &yc, TRUE);
   } else if (queryZoomInPoint == BAD) {
      Window root_win, child_win;
      int root_x, root_y;
      unsigned int status;

      XQueryPointer(mainDisplay, drawWindow, &root_win, &child_win,
            &root_x, &root_y, &xc, &yc, &status);
      xc = ABS_X(xc);
      yc = ABS_Y(yc);
      FixUpZoomCenter(&xc, &yc, TRUE);
   } else {
      xc = (topSel==NULL) ? ((drawOrigX<<1)+drawWinW)>>1 : (selLtX+selRbX)>>1;
      yc = (topSel==NULL) ? ((drawOrigY<<1)+drawWinH)>>1 : (selLtY+selRbY)>>1;
   }
   if (curChoice == VERTEXMODE) {
      TieLooseEnds();
      SetCurChoice(NOTHING);
   }
   if (!panAtCursor) {
      if (zoomedIn) {
         zoomScale++;
      } else if (zoomScale == 0) {
         zoomedIn = TRUE;
         zoomScale++;
      } else {
         zoomScale--;
      }
   }
   if (curChoice==DRAWTEXT && textCursorShown) PreZoomCurText();
   PostZoom(xc, yc);
   if (curChoice==DRAWTEXT && textCursorShown) PostZoomCurText(xc, yc);
   if (need_to_restore_cur_choice) {
      SetCurChoice(curChoiceBeforeMakeQuiescent);
   }
}

void ZoomInAtCursor(abs_x, abs_y)
   int abs_x, abs_y;
{
   zoomInAbsX = abs_x;
   zoomInAbsY = abs_y;
   zoomInAtCursor = TRUE;
   ZoomIn();
   zoomInAtCursor = FALSE;
}

void ZoomWayOut()
{
   int saved_zoom_scale=zoomScale, saved_zoomed_in=zoomedIn;

   while ((drawWinW>>1) >= paperWidth && (drawWinH>>1) >= paperHeight) {
      if (zoomedIn) {
         zoomScale++;
      } else if (zoomScale == 0) {
         zoomedIn = TRUE;
         zoomScale++;
      } else {
         zoomScale--;
      }
      drawWinW >>= 1;
      drawWinH >>= 1;
   }
   while (drawWinW < paperWidth || drawWinH < paperHeight) {
      if (!zoomedIn) {
         zoomScale++;
      } else if (zoomScale == 1) {
         zoomedIn = FALSE;
         zoomScale--;
      } else {
         zoomScale--;
      }
      drawWinW <<= 1;
      drawWinH <<= 1;
   }
   if (saved_zoom_scale==zoomScale && saved_zoomed_in==zoomedIn &&
         drawOrigX==0 && drawOrigY==0) {
      return;
   }
   TieLooseEnds();
   SetCurChoice(NOTHING);

   PostZoom(0, 0);
}

void ZoomOut()
{
   int xc, yc;

   if (paperWidth <= drawWinW && paperHeight <= drawWinH) {
      MsgBox(TgLoadString(STID_AT_PAPER_EDGE_CANT_ZOOMOUT), TOOL_NAME, INFO_MB);
      return;
   }
   if (curChoice==DRAWTEXT && textCursorShown) {
      PrepareZoomCurText(&xc, &yc);
   } else if (queryZoomInPoint == BAD) {
      Window root_win, child_win;
      int root_x, root_y;
      unsigned int status;

      XQueryPointer(mainDisplay, drawWindow, &root_win, &child_win,
            &root_x, &root_y, &xc, &yc, &status);
      xc = ABS_X(xc);
      yc = ABS_Y(yc);
      FixUpZoomCenter(&xc, &yc, FALSE);
   } else {
      xc = (topSel==NULL) ? ((drawOrigX<<1)+drawWinW)>>1 : (selLtX+selRbX)>>1;
      yc = (topSel==NULL) ? ((drawOrigY<<1)+drawWinH)>>1 : (selLtY+selRbY)>>1;
   }
   if (curChoice == VERTEXMODE) {
      TieLooseEnds();
      SetCurChoice(NOTHING);
   }
   if (!zoomedIn) {
      zoomScale++;
   } else if (zoomScale == 1) {
      zoomedIn = FALSE;
      zoomScale--;
   } else {
      zoomScale--;
   }
   if (curChoice==DRAWTEXT && textCursorShown) PreZoomCurText();
   PostZoom(xc, yc);
   if (curChoice==DRAWTEXT && textCursorShown) PostZoomCurText(xc, yc);
}

void CenterAtCursor(abs_x, abs_y)
   int abs_x, abs_y;
{
   panAtCursor = TRUE;
   ZoomInAtCursor(abs_x, abs_y);
   panAtCursor = FALSE;
}

void PreciseZoom(zoomed_in, zoom_scale, force_zoom)
   int zoomed_in, zoom_scale, force_zoom;
{
   int xc, yc;

   if (!force_zoom && zoomedIn == zoomed_in && zoomScale == zoom_scale) return;

   if (curChoice==DRAWTEXT && textCursorShown) {
      PrepareZoomCurText(&xc, &yc);
   } else {
      xc = (topSel==NULL) ? ((drawOrigX<<1)+drawWinW)>>1 : (selLtX+selRbX)>>1;
      yc = (topSel==NULL) ? ((drawOrigY<<1)+drawWinH)>>1 : (selLtY+selRbY)>>1;
   }
   if (curChoice == VERTEXMODE) {
      TieLooseEnds();
      SetCurChoice(NOTHING);
   }
   zoomedIn = zoomed_in;
   zoomScale = zoom_scale;

   if (curChoice==DRAWTEXT && textCursorShown) PreZoomCurText();
   PostZoom(xc, yc);
   if (curChoice==DRAWTEXT && textCursorShown) PostZoomCurText(xc, yc);
}

void SetPSPageWidthHeight()
{

   if (psYOffStr != NULL && psYOff != NULL) {
      float fval=(float)0;

      fval = (float)(((float)onePageHeight)/((float)PIX_PER_INCH));
      FormatFloat(&fval, psYOffStr[0]);
      psYOff[0] = ((float)onePageHeight)/((float)PIX_PER_INCH);
      psPageWidthInInch[0]=((float)onePageWidth)/((float)PIX_PER_INCH);
      psPageWidthInInch[1]=((float)onePageHeight)/((float)PIX_PER_INCH);
      psPageHeightInInch[0]=((float)onePageHeight)/((float)PIX_PER_INCH);
      psPageHeightInInch[1]=((float)onePageWidth)/((float)PIX_PER_INCH);
   }
}

void ResetOnePageSize()
{  /* reset 8.5in x 11in */
#ifndef A4PAPER
   onePageWidth = (85*PIX_PER_INCH)/10;
   onePageHeight = 11*PIX_PER_INCH;
#else /* A4PAPER */
   onePageWidth = (825*PIX_PER_INCH)/100;
   onePageHeight = (117*PIX_PER_INCH)/10;
#endif /* A4PAPER */
   SetPSPageWidthHeight();
}

int UpdPageStyle(NewPageStyle)
   int NewPageStyle;
{
   int changed=FALSE, old_w=paperWidth, old_h=paperHeight;

   pageStyle = NewPageStyle;
   onePageWidth =
         round(psPageWidthInInch[NewPageStyle]*PIX_PER_INCH*100.0/printMag);
   onePageHeight =
         round(psPageHeightInInch[NewPageStyle]*PIX_PER_INCH*100.0/printMag);
   paperWidth = onePageWidth * paperCol;
   paperHeight = onePageHeight * paperRow;
   if (drawOrigX+drawWinW > paperWidth || drawOrigX+drawWinW > old_w) {
      if (drawOrigX+drawWinW > paperWidth) {
         int tmp_x=paperWidth-drawWinW;

         switch (gridSystem) {
         case ENGLISH_GRID:
            if (zoomedIn) {
               if ((ZOOMED_SIZE(tmp_x) % HALF_INCH) == 0) {
                  drawOrigX = max(0,tmp_x);
               } else {
                  drawOrigX = max(0,((int)(ZOOMED_SIZE(tmp_x)/HALF_INCH)) *
                        HALF_INCH);
               }
            } else {
               if ((tmp_x % (ABS_SIZE(HALF_INCH))) == 0) {
                  drawOrigX = max(0,tmp_x);
               } else {
                  drawOrigX = max(0,((int)(tmp_x/ABS_SIZE(HALF_INCH))) *
                        ABS_SIZE(HALF_INCH));
               }
            }
            break;
         case METRIC_GRID:
            if (zoomedIn) {
               if (zoomScale > 1) {
                  if ((ZOOMED_SIZE(tmp_x) % ONE_CM) == 0) {
                     drawOrigX = max(0,tmp_x);
                  } else {
                     drawOrigX = max(0,((int)(ZOOMED_SIZE(tmp_x)/ONE_CM)) *
                           ONE_CM);
                  }
               } else {
                  if ((ZOOMED_SIZE(tmp_x) % FAKE_CM) == 0) {
                     drawOrigX = max(0,tmp_x);
                  } else {
                     drawOrigX = max(0,((int)(ZOOMED_SIZE(tmp_x)/FAKE_CM)) *
                           FAKE_CM);
                  }
               }
            } else {
               if ((tmp_x % (ABS_SIZE(ONE_CM))) == 0) {
                  drawOrigX = max(0,tmp_x);
               } else {
                  drawOrigX = max(0,((int)(tmp_x/ABS_SIZE(ONE_CM))) *
                        ABS_SIZE(ONE_CM));
               }
            }
            break;
         }
      }
      drawOrigX = ABS_SIZE(ZOOMED_SIZE(drawOrigX));
      changed = TRUE;
   }
   if (drawOrigY+drawWinH > paperHeight || drawOrigY+drawWinH > old_h) {
      if (drawOrigY+drawWinH > paperHeight) {
         int tmp_y=paperHeight-drawWinH;

         switch (gridSystem) {
         case ENGLISH_GRID:
            if (zoomedIn) {
               if ((ZOOMED_SIZE(tmp_y) % HALF_INCH) == 0) {
                  drawOrigY = max(0,tmp_y);
               } else {
                  drawOrigY = max(0,((int)(ZOOMED_SIZE(tmp_y)/HALF_INCH)) *
                        HALF_INCH);
               }
            } else {
               if ((tmp_y % (ABS_SIZE(HALF_INCH))) == 0) {
                  drawOrigY = max(0,tmp_y);
               } else {
                  drawOrigY = max(0,((int)(tmp_y/ABS_SIZE(HALF_INCH))) *
                        ABS_SIZE(HALF_INCH));
               }
            }
            break;
         case METRIC_GRID:
            if (zoomedIn) {
               if (zoomScale > 1) {
                  if ((ZOOMED_SIZE(tmp_y) % FAKE_CM) == 0) {
                     drawOrigY = max(0,tmp_y);
                  } else {
                     drawOrigY = max(0,((int)(ZOOMED_SIZE(tmp_y)/FAKE_CM)) *
                           FAKE_CM);
                  }
               } else {
                  if ((ZOOMED_SIZE(tmp_y) % ONE_CM) == 0) {
                     drawOrigY = max(0,tmp_y);
                  } else {
                     drawOrigY = max(0,((int)(ZOOMED_SIZE(tmp_y)/ONE_CM)) *
                           ONE_CM);
                  }
               }
            } else {
               if ((tmp_y % (ABS_SIZE(ONE_CM))) == 0) {
                  drawOrigY = max(0,tmp_y);
               } else {
                  drawOrigY = max(0,((int)(tmp_y/ABS_SIZE(ONE_CM))) *
                        ABS_SIZE(ONE_CM));
               }
            }
            break;
         }
      }
      drawOrigY = ABS_SIZE(ZOOMED_SIZE(drawOrigY));
      changed = TRUE;
   }
   return changed;
}

void ChangePageStyle(NewPageStyle)
   int NewPageStyle;
{
   if (pageStyle != NewPageStyle) {
      if (UpdPageStyle(NewPageStyle)) {
         UpdDrawWinBBox();
         AdjSplineVs();
         ClearAndRedrawDrawWindow();
      }
      ShowFile();
      UpdatePinnedMenu(MENU_LAYOUT);
      RedrawScrollBars();
      RedrawRulers();
      SetFileModified(TRUE);
      switch (NewPageStyle) {
      case LANDSCAPE: Msg(TgLoadString(STID_PAGE_STYLE_CHANGE_TO_LAND)); break;
      case PORTRAIT: Msg(TgLoadString(STID_PAGE_STYLE_CHANGE_TO_PORT)); break;
      }
   }
}

int RefreshPageStyleMenu(menu)
   TgMenu *menu;
{
   int ok=TRUE;

   /* Portrait */
   ok &= TgSetMenuItemRadioById(menu, CMDID_PORTRAITMODE,
         (pageStyle==PORTRAIT));
   /* Landscape */
   ok &= TgSetMenuItemRadioById(menu, CMDID_LANDSCAPEMODE,
         (pageStyle==LANDSCAPE));

   return ok;
}

TgMenu *CreatePageStyleMenu(parent_menu, x, y, menu_info, status_str_xlated)
   TgMenu *parent_menu;
   int x, y;
   TgMenuInfo *menu_info;
   int status_str_xlated; /* ignored, always 0 */
{
   TgMenu *menu=TgCreateMenuFromMenuInfo(parent_menu, x, y, menu_info, FALSE);

   if (menu != NULL) {
      if (!RefreshPageStyleMenu(menu)) {
         return TgDestroyMenu(menu, TRUE);
      }
   }
   return menu;
}

int PageStyleMenu(X, Y, TrackMenubar)
   int X, Y, TrackMenubar;
{
   int rc=INVALID;
   TgMenu *menu=(pageStyleMenuInfo.create_proc)(NULL, X, Y, &pageStyleMenuInfo,
         INVALID);

   activeMenu = INVALID;
   if (menu != NULL) {
      menu->track_menubar = TrackMenubar;

      rc = TgMenuLoop(menu);
      TgDestroyMenu(menu, TRUE);
   }
   return rc;
}

void RefreshExportFormatMenu(menu)
   TgMenu *menu;
{
   int i, num_items=menu->num_items;
   TgMenuItem *menuitems=menu->menuitems;

   for (i=0; i < num_items; i++) {
      TgMenuItem *menu_item=(&menuitems[i]);
      TgMenuItem stMenuItem;

      memset(&stMenuItem, 0, sizeof(TgMenuItem));
      stMenuItem.state = TGBS_NORMAL;
      stMenuItem.checked = (i == whereToPrint);
      TgSetMenuItemInfo(menu_item, TGMU_MASK_STATE|TGMU_MASK_CHECK,
            &stMenuItem);
   }
}

TgMenu *CreateExportFormatMenu(parent_menu, x, y, menu_info, status_str_xlated)
   TgMenu *parent_menu;
   int x, y;
   TgMenuInfo *menu_info;
   int status_str_xlated; /* ignored, always 0 */
{
   int i=0;
   TgMenu *menu=NULL;
   TgMenuInfo stMenuInfo;
   TgMenuItemInfo *item_info=NULL;

   memcpy(&stMenuInfo, menu_info, sizeof(TgMenuInfo));
   stMenuInfo.items = (TgMenuItemInfo*)malloc(
         (maxWhereToPrint+1)*sizeof(TgMenuItemInfo));
   if (stMenuInfo.items == NULL) FailAllocMessage();
   memset(stMenuInfo.items, 0, (maxWhereToPrint+1)*sizeof(TgMenuItemInfo));
   for (item_info=stMenuInfo.items, i=0; i < maxWhereToPrint;
         item_info++, i++) {
      item_info->menu_str = (char*)(Pixmap*)(&whereToPrintPixmap[i]);
      item_info->shortcut_str = NULL;
      if (i < MAXDEFWHERETOPRINT) {
         strcpy(gszMsgBox, ExportFormatMenuLoadString(i, colorDump));
      } else {
         sprintf(gszMsgBox, ExportFormatMenuLoadString(i, colorDump),
               GetExportName(i, EXPORT_THIS));
      }
      item_info->status_str = UtilStrDup(gszMsgBox);
      if (item_info->status_str == NULL) FailAllocMessage();
      item_info->submenu_info = NULL;
      item_info->cmdid = CMDID_SETWHERETOPRINT;
   }
   stMenuInfo.items[maxWhereToPrint].cmdid = INVALID;

   /* the status_str has been translated in ExportFormatMenuLoadString() */
   menu = TgCreateMenuFromMenuInfo(parent_menu, x, y, &stMenuInfo, TRUE);
   for (item_info=stMenuInfo.items, i=0; i < maxWhereToPrint;
         item_info++, i++) {
      UtilFree(item_info->status_str);
   }
   memset(stMenuInfo.items, 0, (maxWhereToPrint+1)*sizeof(TgMenuItemInfo));
   free(stMenuInfo.items);
   stMenuInfo.items = NULL;
   if (menu != NULL) {
      TgMenuItem *menu_item=NULL;
      TgMenuItem stMenuItem;

      menu->track_menubar = TRUE;
      TgAdjustMenuGeometry(menu, choiceImageW, choiceImageH, maxWhereToPrint);
      menu_item = (&menu->menuitems[whereToPrint]);

      memset(&stMenuItem, 0, sizeof(TgMenuItem));
      stMenuItem.checked = TRUE;
      if (!TgSetMenuItemInfo(menu_item, TGMU_MASK_CHECK, &stMenuItem)) {
         return TgDestroyMenu(menu, TRUE);
      }
      menu->refresh_proc = ((RefreshMenuFunc*)RefreshExportFormatMenu);
   }
   return menu;
}

int ExportFormatMenu(X, Y, TrackMenubar)
   int X, Y, TrackMenubar;
{
   int rc=INVALID;
   TgMenu *menu=(exportFormatMenuInfo.create_proc)(NULL, X, Y,
         &exportFormatMenuInfo, INVALID);

   activeMenu = INVALID;
   if (menu != NULL) {
      menu->track_menubar = TrackMenubar;

      rc = TgMenuLoop(menu);
      TgDestroyMenu(menu, TRUE);
   }
   return rc;
}

int RefreshShowHideMenu(menu)
   TgMenu *menu;
{
   int ok=TRUE;

   /* ShowBit/Pixmap */
   ok &= TgSetMenuItemCheckById(menu, CMDID_TOGGLEMAPSHOWN, mapShown);
   /* ShowMeasurement */
   ok &= TgSetMenuItemCheckById(menu, CMDID_TOGGLESHOWMEASUREMENT,
         showMeasurement);
   if (!showMeasurement) {
      ok &= TgEnableMenuItemById(menu, CMDID_TOGGLESHOWMEASUREMENTINTT, FALSE);
   }
   /* ShowMeasurementInTooltip */
   ok &= TgSetMenuItemCheckById(menu, CMDID_TOGGLESHOWMEASUREMENTINTT,
         showMeasurementInTooltip);
   /* ShowMenubar */
   ok &= TgSetMenuItemCheckById(menu, CMDID_TOGGLESHOWMENUBAR, !noMenubar);
   /* ShowStatus */
   ok &= TgSetMenuItemCheckById(menu, CMDID_TOGGLESHOWSTATUS,
         !noStatusWindow);
   /* ShowChoice */
   ok &= TgSetMenuItemCheckById(menu, CMDID_TOGGLESHOWCHOICEBAR,
         !noChoiceWindow);
   /* ShowMode */
   ok &= TgSetMenuItemCheckById(menu, CMDID_TOGGLESHOWMODE, !noModeWindow);

   /* ToggleRightMargin */
   ok &= TgSetMenuItemCheckById(menu, CMDID_TOGGLERIGHTMARGIN,
         rightMarginEnabled == TRUE);
   /* SpecifyRightMargin */
   ok &= TgEnableMenuItemById(menu, CMDID_SPECIFYRIGHTMARGIN,
         rightMarginEnabled == TRUE);

   if (gstWBInfo.do_whiteboard) {
      /* ShowChat */
      ok &= TgEnableMenuItemById(menu, CMDID_TOGGLESHOWCHAT, TRUE);
      ok &= TgSetMenuItemCheckById(menu, CMDID_TOGGLESHOWCHAT, !noChatWindow);
   } else {
      ok &= TgEnableMenuItemById(menu, CMDID_TOGGLESHOWCHAT, FALSE);
   }
   return ok;
}

TgMenu *CreateShowHideMenu(parent_menu, x, y, menu_info, status_str_xlated)
   TgMenu *parent_menu;
   int x, y;
   TgMenuInfo *menu_info;
   int status_str_xlated; /* ignored, always 0 */
{
   TgMenu *menu=TgCreateMenuFromMenuInfo(parent_menu, x, y, menu_info, FALSE);

   if (menu != NULL) {
      if (!RefreshShowHideMenu(menu)) {
         return TgDestroyMenu(menu, TRUE);
      }
   }
   return menu;
}

int RefreshLayoutMenu(menu)
   TgMenu *menu;
{
   int ok=TRUE;

   /* VisibleGrid */
   ok &= TgSetMenuItemCheckById(menu, CMDID_TOGGLEGRID, gridShown);
   /* SnapToGrid */
   ok &= TgSetMenuItemCheckById(menu, CMDID_TOGGLESNAP, snapOn);
   /* Landscape */
   ok &= TgSetMenuItemRadioById(menu, CMDID_LANDSCAPEMODE,
         (pageStyle==LANDSCAPE));
   /* Portrait */
   ok &= TgSetMenuItemRadioById(menu, CMDID_PORTRAITMODE,
         (pageStyle==PORTRAIT));
   /* Print/ExportInColor */
   ok &= TgSetMenuItemCheckById(menu, CMDID_TOGGLEBWCOLORPS, colorDump);
   /* MetricGrid */
   ok &= TgSetMenuItemCheckById(menu, CMDID_TOGGLEGRIDSYSTEM,
         (gridSystem==METRIC_GRID));
   /* OneMotionSelMove */
   ok &= TgSetMenuItemCheckById(menu, CMDID_TOGGLEONEMOTIONSELMOVE,
         oneMotionSelectMove);
   /* UseGrayScale */
   ok &= TgSetMenuItemCheckById(menu, CMDID_TOGGLEUSEGRAYSCALE, useGray);
   /* UseColorLayers */
   ok &= TgEnableMenuItemById(menu, CMDID_TOGGLECOLORLAYERS, colorDisplay);
   if (colorDisplay) {
      ok &= TgSetMenuItemCheckById(menu, CMDID_TOGGLECOLORLAYERS, colorLayers);
   } else {
      ok &= TgSetMenuItemCheckById(menu, CMDID_TOGGLECOLORLAYERS, FALSE);
   }
   /* StackedPage */
   ok &= TgEnableMenuItemById(menu, MENU_STACKEDPAGE,
         (pageLayoutMode==PAGE_STACK));
   /* TiledPage */
   ok &= TgEnableMenuItemById(menu, MENU_TILEDPAGE,
         (pageLayoutMode!=PAGE_STACK));

   return ok;
}

TgMenu *CreateLayoutMenu(parent_menu, x, y, menu_info, status_str_xlated)
   TgMenu *parent_menu;
   int x, y;
   TgMenuInfo *menu_info;
   int status_str_xlated; /* ignored, always 0 */
{
   TgMenu *menu=TgCreateMenuFromMenuInfo(parent_menu, x, y, menu_info, FALSE);

   if (menu != NULL) {
      if (!RefreshLayoutMenu(menu)) {
         return TgDestroyMenu(menu, TRUE);
      }
   }
   return menu;
}

int LayoutMenu(X, Y, TrackMenubar)
   int X, Y, TrackMenubar;
{
   int rc=INVALID;
   TgMenu *menu=(layoutMenuInfo.create_proc)(NULL, X, Y, &layoutMenuInfo,
         INVALID);

   activeMenu = MENU_LAYOUT;
   if (menu != NULL) {
      menu->track_menubar = TrackMenubar;

      rc = TgMenuLoop(menu);
      TgDestroyMenu(menu, TRUE);
   }
   return rc;
}

int GridSize()
{
   int grid=1;

   if (snapOn) {
      switch (gridSystem) {
      case ENGLISH_GRID: grid = xyEnglishGrid; break;
      case METRIC_GRID: grid = xyMetricGrid; break;
      }
      if (zoomedIn) {
         grid <<= zoomScale;
      }
   } else {
      if (zoomedIn) {
         grid = 1 << zoomScale;
      }
   }
   return grid;
}

void GridXY(X, Y, GridX, GridY)
   int X, Y, *GridX, *GridY;
{
   int dx, dy, grid=GridSize();

   if (snapOn) {
      if (zoomedIn) {
         dx = (X+(drawOrigX<<zoomScale)) % grid;
         dy = (Y+(drawOrigY<<zoomScale)) % grid;
      } else {
         dx = (X+(drawOrigX>>zoomScale)) % grid;
         dy = (Y+(drawOrigY>>zoomScale)) % grid;
      }
      *GridX = ((dx<<1) < grid) ? X-dx : X-dx+grid;
      *GridY = ((dy<<1) < grid) ? Y-dy : Y-dy+grid;
   } else {
      if (zoomedIn) {
         dx = (X+(drawOrigX<<zoomScale)) % grid;
         dy = (Y+(drawOrigY<<zoomScale)) % grid;
         *GridX = ((dx<<1) < grid) ? X-dx : X-dx+grid;
         *GridY = ((dy<<1) < grid) ? Y-dy : Y-dy+grid;
      } else {
         *GridX = X;
         *GridY = Y;
      }
   }
}

int ControlKeyPressedEvent(ev)
   XEvent *ev;
{
   if (ev->type == KeyPress || ev->type == KeyRelease) {
      char s[80];
      KeySym key_sym;

      XLookupString(&(ev->xkey), s, sizeof(s), &key_sym, NULL);
      if (key_sym == XK_Control_L || key_sym == XK_Control_R) {
         return (ev->type == KeyPress);
      }
   }
   return FALSE;
}

int DiagEventCheck(ev)
   XEvent *ev;
{
   static int prev_diag_state=FALSE;

   if (ev->type == MotionNotify) {
      prev_diag_state = (ev->xmotion.state & ShiftMask) &&
            (!(ev->xmotion.state & ControlMask));
   } else if (ev->type == KeyPress || ev->type == KeyRelease) {
      char s[80];
      KeySym key_sym;

      XLookupString(&(ev->xkey), s, sizeof(s), &key_sym, NULL);
      if (key_sym == XK_Shift_L || key_sym == XK_Shift_R) {
         prev_diag_state = (ev->type == KeyPress);
         return TRUE;
      }
   }
   return prev_diag_state;
}

void DiagGridXY(OrigX, OrigY, NewX, NewY)
   int OrigX, OrigY, *NewX, *NewY;
{
   int X=(*NewX), Y=(*NewY);
   int dx, dy;
   int p0, p1, p2, p3;

   dx = OrigX - X;
   dy = OrigY - Y;

   p0 = dy - (dx >> 1); /* y = x/2 */
   p1 = (dy >> 1) - dx; /* y = 2x */
   p2 = (dy >> 1) + dx; /* y = -2x */
   p3 = dy + (dx >> 1); /* y = -x/2 */

   if ( (p3 ^ p0) < 0) {
      Y = OrigY;
   } else if ( (p0 ^ p1) < 0) {
      X = OrigX - ((dx + dy)>>1);
      Y = OrigY - ((dx + dy)>>1);
   } else if ( (p1 ^ p2) >= 0) {
      X = OrigX;
   } else {
      X = OrigX - ((dx - dy)>>1);
      Y = OrigY + ((dx - dy)>>1);
      if ((GridSize() & 0x1) == 0) {
         X--;
      }
   }
   (*NewX) = X;
   (*NewY) = Y;
}

void CleanUpGrids()
{
}

void MoveModeSubMenu(index)
   int index;
{
   moveMode = index;
   switch (moveMode) {
   case CONST_MOVE: Msg(TgLoadString(STID_CONSTRAINED_MOVE)); break;
   case UNCONST_MOVE: Msg(TgLoadString(STID_UNCONSTRAINED_MOVE)); break;
   }
   ShowMoveMode();
   UpdatePinnedMenu(MENU_LAYOUT);
   UpdatePinnedMenu(MENU_MOVEMODE);
}

void RefreshMoveModeMenu(menu)
   TgMenu *menu;
{
   int i, num_items=menu->num_items;
   TgMenuItem *menuitems=menu->menuitems;

   for (i=0; i < num_items; i++) {
      TgMenuItem *menu_item=(&menuitems[i]);
      TgMenuItem stMenuItem;

      memset(&stMenuItem, 0, sizeof(TgMenuItem));
      stMenuItem.state = TGBS_NORMAL;
      stMenuItem.checked = (i == moveMode);
      TgSetMenuItemInfo(menu_item, TGMU_MASK_STATE|TGMU_MASK_CHECK,
            &stMenuItem);
   }
}

TgMenu *CreateMoveModeMenu(parent_menu, x, y, menu_info, status_str_xlated)
   TgMenu *parent_menu;
   int x, y;
   TgMenuInfo *menu_info;
   int status_str_xlated; /* ignored, always 0 */
{
   TgMenu *menu=TgCreateMenuFromMenuInfo(parent_menu, x, y, menu_info, FALSE);

   if (menu != NULL) {
      TgMenuItem *menu_item=NULL;
      TgMenuItem stMenuItem;

      menu->track_menubar = TRUE;
      TgAdjustMenuGeometry(menu, choiceImageW, choiceImageH, MAXMOVEMODES);
      menu_item = (&menu->menuitems[moveMode]);

      memset(&stMenuItem, 0, sizeof(TgMenuItem));
      stMenuItem.checked = TRUE;
      if (!TgSetMenuItemInfo(menu_item, TGMU_MASK_CHECK, &stMenuItem)) {
         return TgDestroyMenu(menu, TRUE);
      }
      menu->refresh_proc = ((RefreshMenuFunc*)RefreshMoveModeMenu);
   }
   return menu;
}

int MoveModeMenu(X, Y, TrackMenubar)
   int X, Y, TrackMenubar;
{
   int rc=INVALID;
   TgMenu *menu=(moveModeMenuInfo.create_proc)(NULL, X, Y, &moveModeMenuInfo,
         INVALID);

   activeMenu = MENU_MOVEMODE;
   if (menu != NULL) {
      menu->track_menubar = TrackMenubar;

      rc = TgMenuLoop(menu);
      TgDestroyMenu(menu, TRUE);
   }
   return rc;
}

void StretchableTextModeSubMenu(index)
   int index;
{
   stretchableText = index;
   switch (stretchableText) {
   case NO_STRETCHABLE_TEXT:
      Msg(TgLoadString(STID_NON_STRETCHABLE_TEXT_MODE));
      break;
   case STRETCHABLE_TEXT: Msg(TgLoadString(STID_STRETCHABLE_TEXT_MODE)); break;
   }
   ShowStretchableTextMode();
   UpdatePinnedMenu(MENU_STRETCHTEXT);
}

void RefreshStretchableTextModeMenu(menu)
   TgMenu *menu;
{
   int i, num_items=menu->num_items;
   TgMenuItem *menuitems=menu->menuitems;

   for (i=0; i < num_items; i++) {
      TgMenuItem *menu_item=(&menuitems[i]);
      TgMenuItem stMenuItem;

      memset(&stMenuItem, 0, sizeof(TgMenuItem));
      stMenuItem.state = TGBS_NORMAL;
      stMenuItem.checked = (i == stretchableText);
      TgSetMenuItemInfo(menu_item, TGMU_MASK_STATE|TGMU_MASK_CHECK,
            &stMenuItem);
   }
}

TgMenu *CreateStretchableTextModeMenu(parent_menu, x, y, menu_info,
      status_str_xlated)
   TgMenu *parent_menu;
   int x, y;
   TgMenuInfo *menu_info;
   int status_str_xlated; /* ignored, always 0 */
{
   TgMenu *menu=TgCreateMenuFromMenuInfo(parent_menu, x, y, menu_info, FALSE);

   if (menu != NULL) {
      TgMenuItem *menu_item=NULL;
      TgMenuItem stMenuItem;

      menu->track_menubar = TRUE;
      TgAdjustMenuGeometry(menu, choiceImageW, choiceImageH,
            MAXSTRETCHABLEMODES);
      menu_item = (&menu->menuitems[stretchableText]);

      memset(&stMenuItem, 0, sizeof(TgMenuItem));
      stMenuItem.checked = TRUE;
      if (!TgSetMenuItemInfo(menu_item, TGMU_MASK_CHECK, &stMenuItem)) {
         return TgDestroyMenu(menu, TRUE);
      }
      menu->refresh_proc = ((RefreshMenuFunc*)RefreshStretchableTextModeMenu);
   }
   return menu;
}

int StretchableTextModeMenu(X, Y, TrackMenubar)
   int X, Y, TrackMenubar;
{
   int rc=INVALID;
   TgMenu *menu=(stretchableTextModeMenuInfo.create_proc)(NULL, X, Y,
         &stretchableTextModeMenuInfo, INVALID);

   activeMenu = MENU_STRETCHTEXT;
   if (menu != NULL) {
      menu->track_menubar = TrackMenubar;

      rc = TgMenuLoop(menu);
      TgDestroyMenu(menu, TRUE);
   }
   return rc;
}

void RefreshTransPatModeMenu(menu)
   TgMenu *menu;
{
   int i, num_items=menu->num_items;
   TgMenuItem *menuitems=menu->menuitems;

   for (i=0; i < num_items; i++) {
      TgMenuItem *menu_item=(&menuitems[i]);
      TgMenuItem stMenuItem;

      memset(&stMenuItem, 0, sizeof(TgMenuItem));
      stMenuItem.state = TGBS_NORMAL;
      stMenuItem.checked = (i == transPat);
      TgSetMenuItemInfo(menu_item, TGMU_MASK_STATE|TGMU_MASK_CHECK,
            &stMenuItem);
   }
}

TgMenu *CreateTransPatModeMenu(parent_menu, x, y, menu_info, status_str_xlated)
   TgMenu *parent_menu;
   int x, y;
   TgMenuInfo *menu_info;
   int status_str_xlated; /* ignored, always 0 */
{
   TgMenu *menu=TgCreateMenuFromMenuInfo(parent_menu, x, y, menu_info, FALSE);

   if (menu != NULL) {
      TgMenuItem *menu_item=NULL;
      TgMenuItem stMenuItem;

      menu->track_menubar = TRUE;
      TgAdjustMenuGeometry(menu, choiceImageW, choiceImageH, MAXTRANSPATMODES);
      menu_item = (&menu->menuitems[transPat]);

      memset(&stMenuItem, 0, sizeof(TgMenuItem));
      stMenuItem.checked = TRUE;
      if (!TgSetMenuItemInfo(menu_item, TGMU_MASK_CHECK, &stMenuItem)) {
         return TgDestroyMenu(menu, TRUE);
      }
      menu->refresh_proc = ((RefreshMenuFunc*)RefreshTransPatModeMenu);
   }
   return menu;
}

int TransPatModeMenu(X, Y, TrackMenubar)
   int X, Y, TrackMenubar;
{
   int rc=INVALID;
   TgMenu *menu=(transPatModeMenuInfo.create_proc)(NULL, X, Y,
         &transPatModeMenuInfo, INVALID);

   activeMenu = MENU_TRANSPAT;
   if (menu != NULL) {
      menu->track_menubar = TrackMenubar;

      rc = TgMenuLoop(menu);
      TgDestroyMenu(menu, TRUE);
   }
   return rc;
}

int RefreshGridMenu(menu)
   TgMenu *menu;
{
   int ok=TRUE;

   /* VisibleGrid */
   ok &= TgSetMenuItemCheckById(menu, CMDID_TOGGLEGRID, gridShown);
   /* SnapToGrid */
   ok &= TgSetMenuItemCheckById(menu, CMDID_TOGGLESNAP, snapOn);
   /* MetricGrid */
   ok &= TgSetMenuItemCheckById(menu, CMDID_TOGGLEGRIDSYSTEM,
         (gridSystem==METRIC_GRID));

   /* ToggleRightMargin */
   ok &= TgSetMenuItemCheckById(menu, CMDID_TOGGLERIGHTMARGIN,
         rightMarginEnabled == TRUE);
   /* SpecifyRightMargin */
   ok &= TgEnableMenuItemById(menu, CMDID_SPECIFYRIGHTMARGIN,
         rightMarginEnabled == TRUE);

   return ok;
}

TgMenu *CreateGridMenu(parent_menu, x, y, menu_info, status_str_xlated)
   TgMenu *parent_menu;
   int x, y;
   TgMenuInfo *menu_info;
   int status_str_xlated; /* ignored, always 0 */
{
   TgMenu *menu=TgCreateMenuFromMenuInfo(parent_menu, x, y, menu_info, FALSE);

   if (menu != NULL) {
      if (!RefreshGridMenu(menu)) {
         return TgDestroyMenu(menu, TRUE);
      }
      menu->refresh_proc = ((RefreshMenuFunc*)RefreshGridMenu);
   }
   return menu;
}

int GridMenu(X, Y, TrackMenubar)
   int X, Y, TrackMenubar;
{
   int rc=INVALID;
   TgMenu *menu=(gridMenuInfo.create_proc)(NULL, X, Y, &gridMenuInfo, INVALID);

   activeMenu = INVALID;
   if (menu != NULL) {
      menu->track_menubar = TrackMenubar;

      rc = TgMenuLoop(menu);
      TgDestroyMenu(menu, TRUE);
   }
   return rc;
}

