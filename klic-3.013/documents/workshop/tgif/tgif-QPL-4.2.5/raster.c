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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/raster.c,v 1.21 2011/05/16 16:21:59 william Exp $
 */

#define _INCLUDE_FROM_RASTER_C_

#include "tgifdefs.h"
#include "expfdefs.h"

#include "choice.e"
#include "color.e"
#include "dialog.e"
#include "file.e"
#include "font.e"
#include "menu.e"
#include "miniline.e"
#include "msg.e"
#include "pattern.e"
#include "poly.e"
#include "shape.e"
#include "raster.e"
#include "setup.e"
#include "strtbl.e"
#include "text.e"
#include "util.e"
#include "xprtfltr.e"

#include "xbm/arrow.xbm"
#include "xbm/text.xbm"
#include "xbm/box.xbm"
#include "xbm/oval.xbm"
#include "xbm/ovalcent.xbm"
#include "xbm/ovaledge.xbm"
#include "xbm/poly.xbm"
#include "xbm/polygon.xbm"
#include "xbm/arc.xbm"
#include "xbm/arc_edge.xbm"
#include "xbm/rcbox.xbm"
#include "xbm/freehand.xbm"
#include "xbm/vmode.xbm"
#include "xbm/rot_mode.xbm"
#include "xbm/rot_mode2.xbm"

#include "xbm/pat0.xbm"
#include "xbm/pat1.xbm"
#include "xbm/pat2.xbm"
#include "xbm/pat3.xbm"
#include "xbm/pat4.xbm"
#include "xbm/pat5.xbm"
#include "xbm/pat6.xbm"
#include "xbm/pat7.xbm"
#include "xbm/pat8.xbm"
#include "xbm/pat9.xbm"
#include "xbm/pat10.xbm"
#include "xbm/pat11.xbm"
#include "xbm/pat12.xbm"
#include "xbm/pat13.xbm"
#include "xbm/pat14.xbm"
#include "xbm/pat15.xbm"
#include "xbm/pat16.xbm"
#include "xbm/pat17.xbm"
#include "xbm/pat18.xbm"
#include "xbm/pat19.xbm"
#include "xbm/pat20.xbm"
#include "xbm/pat21.xbm"
#include "xbm/pat22.xbm"
#include "xbm/pat23.xbm"
#include "xbm/pat24.xbm"
#include "xbm/pat25.xbm"
#include "xbm/pat26.xbm"
#include "xbm/pat27.xbm"
#include "xbm/pat28.xbm"
#include "xbm/pat29.xbm"
#include "xbm/pat30.xbm"
#include "xbm/pat31.xbm"
#include "xbm/pat32.xbm"

#include "xbm/shape0.xbm"
#include "xbm/shape1.xbm"
#include "xbm/shape2.xbm"
#include "xbm/shape3.xbm"
#include "xbm/shape4.xbm"
#include "xbm/shape5.xbm"
#include "xbm/shape6.xbm"
#include "xbm/shape7.xbm"
#include "xbm/shape8.xbm"
#include "xbm/shape9.xbm"
#include "xbm/shape10.xbm"
#include "xbm/shape11.xbm"
#include "xbm/shape12.xbm"
#include "xbm/shape13.xbm"
#include "xbm/shape14.xbm"
#include "xbm/shape15.xbm"
#include "xbm/shape16.xbm"
#include "xbm/shape17.xbm"
#include "xbm/shape18.xbm"
#include "xbm/shape19.xbm"

#include "xbm/just_l.xbm"
#include "xbm/just_c.xbm"
#include "xbm/just_r.xbm"

#include "xbm/align_n.xbm"

#include "xbm/align_l.xbm"
#include "xbm/align_c.xbm"
#include "xbm/align_r.xbm"

#include "xbm/align_t.xbm"
#include "xbm/align_m.xbm"
#include "xbm/align_b.xbm"

#include "xbm/align_s.xbm"

#include "xbm/align_lt.xbm"
#include "xbm/align_lm.xbm"
#include "xbm/align_lb.xbm"
#include "xbm/align_ct.xbm"
#include "xbm/align_cm.xbm"
#include "xbm/align_cb.xbm"
#include "xbm/align_rt.xbm"
#include "xbm/align_rm.xbm"
#include "xbm/align_rb.xbm"

#include "xbm/align_sn.xbm"
#include "xbm/align_st.xbm"
#include "xbm/align_sm.xbm"
#include "xbm/align_sb.xbm"
#include "xbm/align_ns.xbm"
#include "xbm/align_ls.xbm"
#include "xbm/align_cs.xbm"
#include "xbm/align_rs.xbm"
#include "xbm/align_ss.xbm"

#include "xbm/lw0.xbm"
#include "xbm/lw1.xbm"
#include "xbm/lw2.xbm"
#include "xbm/lw3.xbm"
#include "xbm/lw4.xbm"
#include "xbm/lw5.xbm"
#include "xbm/lw6.xbm"

#include "xbm/lt0.xbm"
#include "xbm/lt1.xbm"
#include "xbm/lt2.xbm"
#include "xbm/lt3.xbm"

#include "xbm/ls0.xbm"
#include "xbm/ls1.xbm"
#include "xbm/ls2.xbm"
#include "xbm/ls3.xbm"

#include "xbm/ld0.xbm"
#include "xbm/ld1.xbm"
#include "xbm/ld2.xbm"
#include "xbm/ld3.xbm"
#include "xbm/ld4.xbm"
#include "xbm/ld5.xbm"
#include "xbm/ld6.xbm"
#include "xbm/ld7.xbm"
#include "xbm/ld8.xbm"

#include "xbm/lw0s.xbm"
#include "xbm/lw1s.xbm"
#include "xbm/lw2s.xbm"
#include "xbm/lw3s.xbm"
#include "xbm/lw4s.xbm"
#include "xbm/lw5s.xbm"
#include "xbm/lw6s.xbm"

#include "xbm/lt0s.xbm"
#include "xbm/lt1s.xbm"
#include "xbm/lt2s.xbm"
#include "xbm/lt3s.xbm"

#include "xbm/ls0s.xbm"
#include "xbm/ls1s.xbm"
#include "xbm/ls2s.xbm"
#include "xbm/ls3s.xbm"

#include "xbm/ld0s.xbm"
#include "xbm/ld1s.xbm"
#include "xbm/ld2s.xbm"
#include "xbm/ld3s.xbm"
#include "xbm/ld4s.xbm"
#include "xbm/ld5s.xbm"
#include "xbm/ld6s.xbm"
#include "xbm/ld7s.xbm"
#include "xbm/ld8s.xbm"

#include "xbm/printer.xbm"
#include "xbm/latex.xbm"
#include "xbm/psfile.xbm"
#include "xbm/xbm.xbm"
#include "xbm/ascii.xbm"
#include "xbm/epsi.xbm"
#include "xbm/gif.xbm"
#include "xbm/html.xbm"
#include "xbm/pdf.xbm"
#include "xbm/tiffepsi.xbm"
#include "xbm/png.xbm"
#include "xbm/jpeg.xbm"
#include "xbm/pbm.xbm"
#include "xbm/netlist.xbm"
#include "xbm/svg.xbm"

#include "xbm/file.xbm"
#include "xbm/landscap.xbm"
#include "xbm/special.xbm"
#include "xbm/vspace.xbm"

#include "xbm/rcb_rad.xbm"
#include "xbm/const_mv.xbm"
#include "xbm/uncon_mv.xbm"
#include "xbm/edit.xbm"
#include "xbm/intr.xbm"
#include "xbm/intr90.xbm"
#include "xbm/trek.xbm"
#include "xbm/stack.xbm"
#include "xbm/tile.xbm"
#include "xbm/leftend.xbm"
#include "xbm/lfarrow.xbm"
#include "xbm/rtarrow.xbm"
#include "xbm/rightend.xbm"
#include "xbm/upend.xbm"
#include "xbm/uparrow.xbm"
#include "xbm/dnarrow.xbm"
#include "xbm/downend.xbm"
#include "xbm/chkall.xbm"
#include "xbm/unchkall.xbm"
#include "xbm/scrl_up.xbm"
#include "xbm/scrl_dn.xbm"
#include "xbm/scrl_lf.xbm"
#include "xbm/scrl_rt.xbm"
#include "xbm/stretch.xbm"
#include "xbm/nstretch.xbm"
#include "xbm/pattrans.xbm"
#include "xbm/patopaq.xbm"

#include "xbm/rot_0.xbm"
#include "xbm/rot_90.xbm"
#include "xbm/rot_180.xbm"
#include "xbm/rot_270.xbm"

#include "xbm/wire.xbm"
#include "xbm/run.xbm"
#include "xbm/check.xbm"
#include "xbm/radio.xbm"
#include "xbm/submenu.xbm"
#include "xbm/gsubmenu.xbm"

#include "xbm/redraw.xbm"

#include "xbm/btn1.xbm"
#include "xbm/btn2.xbm"
#include "xbm/btn3.xbm"

#include "xbm/stop.xbm"
#include "xbm/question.xbm"
#include "xbm/info.xbm"
#include "xbm/dialog.xbm"

GC rasterGC=NULL;
GC patGC=NULL;

unsigned long xorOne=0L;
unsigned long xorZero=0L;

GC drawGC=NULL;
GC drawSBGC=NULL;
GC defaultGC=NULL;
GC revDefaultGC=NULL;
GC revGrayGC=NULL;

Pixmap rotateModePixmap[2];
Pixmap choicePixmap[MAXCHOICES];
Pixmap patPixmap[MAXPATTERNS+1];
Pixmap *lineWidthPixmap=NULL;
Pixmap lineTypePixmap[MAXLINETYPES];
Pixmap lineDashPixmap[MAXDASHES];
Pixmap lineStylePixmap[MAXLINESTYLES];
Pixmap justPixmap[MAXJUSTS];
Pixmap alignHoriPixmap[MAXALIGNS];
Pixmap alignVertPixmap[MAXALIGNS];
Pixmap alignDirectPixmap[(MAXALIGNS-1)*(MAXALIGNS-1)];
Pixmap distrDirectPixmap[MAXALIGNS*MAXALIGNS];
Pixmap *shortLineWidthPixmap=NULL;
Pixmap filePixmap=None;
Pixmap landscapePixmap=None;
Pixmap specialPixmap=None;
Pixmap vspacePixmap=None;
Pixmap rcbRadiusPixmap=None;
Pixmap moveModePixmap[MAXMOVEMODES];
Pixmap editPixmap=None;
Pixmap intrPixmap[MAXINTRS];
Pixmap trekPixmap=None;
Pixmap pageLayoutPixmap[MAXPAGELAYOUTMODES];
Pixmap scrollPixmap[MAXSCROLLBUTTONS];
Pixmap scrlBitmap[4];
Pixmap shapePixmap[MAXSHAPES];
Pixmap stretchableModePixmap[MAXSTRETCHABLEMODES];
Pixmap transPatPixmap[MAXTRANSPATMODES];

Pixmap rotatePixmap[4];
Pixmap *whereToPrintPixmap=NULL;
Pixmap shortLineTypePixmap[MAXLINETYPES];
Pixmap shortLineStylePixmap[MAXLINESTYLES];
Pixmap shortDashPixmap[MAXDASHES];

Pixmap wireBitmap=None;
Pixmap runBitmap=None;
Pixmap checkBitmap=None;
Pixmap radioBitmap=None;
Pixmap submenuBitmap=None;
Pixmap graySubmenuBitmap=None;
Pixmap userRedrawBitmap=None;
Pixmap statusBtnPixmap[MAX_STATUS_BTNS];
Pixmap msgBoxPixmap[MAX_MB_ICONS];

int maxLineWidths=MAXLINEWIDTHS;
int maxWhereToPrint=MAXDEFWHERETOPRINT;

int dashListLength[MAXDASHES] = { 0, 2, 2, 2, 4, 2, 6, 2, 2 };
char *dashList[MAXDASHES] =
      {"","\004\014","\010\010","\014\004","\024\004\004\004",
       "\004\004","\014\004\004\004\004\004","\020\020","\002\002"};

static int pat_w[MAXPATTERNS+1], pat_h[MAXPATTERNS+1];
static int shape_w[MAXSHAPES+1], shape_h[MAXSHAPES+1];

static unsigned char *patData[MAXPATTERNS+1];
static int needToFreePat[MAXPATTERNS+1];
static unsigned char *shapeData[MAXSHAPES+1];

static int newLineWidthAllocated=FALSE;

static Pixmap defaultLineWidthPixmap[MAXLINEWIDTHS];
static Pixmap defaultShortLineWidthPixmap[MAXLINEWIDTHS];

static
void InitLines()
{
   int i, j, y, short_y, len, x;
   char *c_ptr, buf[80], s[MAXSTRING];
   XGCValues values;
   GC tmp_gc, tmp_rev_gc;
   Pixmap dummy_pixmap;

   newLineWidthAllocated = FALSE;

   maxLineWidths = MAXLINEWIDTHS;
   if ((c_ptr=XGetDefault(mainDisplay, TOOL_NAME, "MaxLineWidths")) != NULL) {
      maxLineWidths = atoi(c_ptr);
      if (maxLineWidths <= 0) {
         fprintf(stderr, TgLoadString(STID_INVALID_XDEF_USE_ALT_VALUE),
               TOOL_NAME, "MaxLineWidths", c_ptr, MAXLINEWIDTHS);
         maxLineWidths = MAXLINEWIDTHS;
      }
   }

   curWidthOfLine = (short*)malloc(maxLineWidths*sizeof(short));
   curArrowHeadW = (short*)malloc(maxLineWidths*sizeof(short));
   curArrowHeadH = (short*)malloc(maxLineWidths*sizeof(short));
   curWidthOfLineSpec = (char**)malloc(maxLineWidths*sizeof(char*));
   curArrowHeadWSpec = (char**)malloc(maxLineWidths*sizeof(char*));
   curArrowHeadHSpec = (char**)malloc(maxLineWidths*sizeof(char*));
   memset(curWidthOfLineSpec, 0, sizeof(maxLineWidths*sizeof(char*)));
   memset(curArrowHeadWSpec, 0, sizeof(maxLineWidths*sizeof(char*)));
   memset(curArrowHeadHSpec, 0, sizeof(maxLineWidths*sizeof(char*)));

   *buf = '\0';
   for (i = 0; i < maxLineWidths; i++) {
      float fval;
      char *sval;

      sprintf(buf, "LineWidth%1d", i);
      if ((c_ptr=XGetDefault(mainDisplay, TOOL_NAME, buf)) != NULL) {
         strcpy(gszMsgBox, c_ptr);
         sval = strtok(gszMsgBox, ", \t\n\r");
         if (sval == NULL) break;
         if (sscanf(sval, "%f", &fval) != 1) break;
         curWidthOfLine[i] = round(fval);
         curWidthOfLineSpec[i] = UtilStrDup(sval);
      } else if (i < MAXLINEWIDTHS) {
         curWidthOfLine[i] = origWidthOfLine[i];
         sprintf(gszMsgBox, "%1d", curWidthOfLine[i]);
         curWidthOfLineSpec[i] = UtilStrDup(gszMsgBox);
      } else {
         break;
      }
      sprintf(buf, "ArrowWidth%1d", i);
      if ((c_ptr=XGetDefault(mainDisplay, TOOL_NAME, buf)) != NULL) {
         strcpy(gszMsgBox, c_ptr);
         sval = strtok(gszMsgBox, ", \t\n\r");
         if (sval == NULL) break;
         if (sscanf(sval, "%f", &fval) != 1) break;
         curArrowHeadW[i] = round(fval);
         curArrowHeadWSpec[i] = UtilStrDup(sval);
      } else if (i < MAXLINEWIDTHS) {
         curArrowHeadW[i] = origArrowHeadW[i];
         sprintf(gszMsgBox, "%1d", curArrowHeadW[i]);
         curArrowHeadWSpec[i] = UtilStrDup(gszMsgBox);
      } else {
         break;
      }
      sprintf(buf, "ArrowHeight%1d", i);
      if ((c_ptr=XGetDefault(mainDisplay, TOOL_NAME, buf)) != NULL) {
         strcpy(gszMsgBox, c_ptr);
         sval = strtok(gszMsgBox, ", \t\n\r");
         if (sval == NULL) break;
         if (sscanf(sval, "%f", &fval) != 1) break;
         curArrowHeadH[i] = round(fval);
         curArrowHeadHSpec[i] = UtilStrDup(sval);
      } else if (i < MAXLINEWIDTHS) {
         curArrowHeadH[i] = origArrowHeadH[i];
         sprintf(gszMsgBox, "%1d", curArrowHeadH[i]);
         curArrowHeadHSpec[i] = UtilStrDup(gszMsgBox);
      } else {
         break;
      }
   }
   if (i != maxLineWidths) {
      fprintf(stderr, TgLoadString(STID_CANT_GET_XDEF_DEF_WIDTHS_USED),
            TOOL_NAME, buf);
      fprintf(stderr, "\n");

      free(curWidthOfLine);
      free(curArrowHeadW);
      free(curArrowHeadH);
      for (j=0; j < i; j++) {
         if (curWidthOfLineSpec[j] != NULL) free(curWidthOfLineSpec[j]);
         if (curArrowHeadWSpec[j] != NULL) free(curArrowHeadWSpec[j]);
         if (curArrowHeadHSpec[j] != NULL) free(curArrowHeadHSpec[j]);
      }
      free(curWidthOfLine);
      free(curArrowHeadW);
      free(curArrowHeadH);
   } else {
      y = HALF_W(menuImageH);
      short_y = HALF_W(choiceImageH);

      lineWidthPixmap = (Pixmap*)malloc(maxLineWidths*sizeof(Pixmap));
      if (lineWidthPixmap == NULL) FailAllocMessage();
      shortLineWidthPixmap = (Pixmap*)malloc(maxLineWidths*sizeof(Pixmap));
      if (shortLineWidthPixmap == NULL) FailAllocMessage();

      dummy_pixmap = XCreatePixmap(mainDisplay, mainWindow, 1, 1, 1);

      values.foreground = 1;
      values.background = 0;
      values.fill_style = FillSolid;
      values.function = GXcopy;
      values.font = rulerFontPtr->fid;
      tmp_gc = XCreateGC(mainDisplay, dummy_pixmap,
            GCForeground | GCBackground | GCFillStyle | GCFunction | GCFont,
            &values);

      values.foreground = 0;
      tmp_rev_gc = XCreateGC(mainDisplay, dummy_pixmap,
            GCForeground | GCBackground | GCFillStyle | GCFunction | GCFont,
            &values);

      for (i = 0; i < maxLineWidths; i++) {
         lineWidthPixmap[i] = XCreatePixmap(mainDisplay, mainWindow,
            menuImageW, menuImageH, 1);
         shortLineWidthPixmap[i] = XCreatePixmap(mainDisplay, mainWindow,
            choiceImageW, choiceImageH, 1);

         XFillRectangle(mainDisplay, lineWidthPixmap[i], tmp_rev_gc,
               0, 0, menuImageW, menuImageH);
         XFillRectangle(mainDisplay, shortLineWidthPixmap[i], tmp_rev_gc,
               0, 0, choiceImageW, choiceImageH);

         values.line_width = curWidthOfLine[i];
#ifdef NO_THIN_LINE
         if (values.line_width < 1) values.line_width = 1;
#endif
         XChangeGC(mainDisplay, tmp_gc, GCLineWidth, &values);

         XDrawLine(mainDisplay, lineWidthPixmap[i], tmp_gc,
             4, y, menuImageW-4, y);
         XDrawLine(mainDisplay, shortLineWidthPixmap[i], tmp_gc,
             4, short_y, choiceImageW-4, short_y);

         sprintf(s, "%1d", curWidthOfLine[i]);
         len = strlen(s);

         x = HALF_W(menuImageW-len*rulerFontWidth);
         if (curWidthOfLine[i] > menuImageH-4) {
            XDrawString(mainDisplay, lineWidthPixmap[i], tmp_rev_gc, x,
               y+HALF_W(rulerFontHeight)-rulerFontDes, s, len);
         }
         x = HALF_W(choiceImageW-len*rulerFontWidth);
         if (curWidthOfLine[i] > choiceImageH-4) {
            XDrawString(mainDisplay, shortLineWidthPixmap[i], tmp_rev_gc, x,
               y+HALF_W(rulerFontHeight)-rulerFontDes, s, len);
         }
      }
      XFreeGC(mainDisplay, tmp_gc);
      XFreeGC(mainDisplay, tmp_rev_gc);
      XFreePixmap(mainDisplay, dummy_pixmap);

      newLineWidthAllocated = TRUE;
   }
   if (!newLineWidthAllocated) {
      maxLineWidths = MAXLINEWIDTHS;

      curWidthOfLine = (short*)malloc(maxLineWidths*sizeof(short));
      curArrowHeadW = (short*)malloc(maxLineWidths*sizeof(short));
      curArrowHeadH = (short*)malloc(maxLineWidths*sizeof(short));
      curWidthOfLineSpec = (char**)malloc(maxLineWidths*sizeof(char*));
      curArrowHeadWSpec = (char**)malloc(maxLineWidths*sizeof(char*));
      curArrowHeadHSpec = (char**)malloc(maxLineWidths*sizeof(char*));
      memset(curWidthOfLineSpec, 0, sizeof(maxLineWidths*sizeof(char*)));
      memset(curArrowHeadWSpec, 0, sizeof(maxLineWidths*sizeof(char*)));
      memset(curArrowHeadHSpec, 0, sizeof(maxLineWidths*sizeof(char*)));

      lineWidthPixmap = (Pixmap*)malloc(maxLineWidths*sizeof(Pixmap));
      if (lineWidthPixmap == NULL) FailAllocMessage();
      shortLineWidthPixmap = (Pixmap*)malloc(maxLineWidths*sizeof(Pixmap));
      if (shortLineWidthPixmap == NULL) FailAllocMessage();

      for (i = 0; i < maxLineWidths; i++) {
         curWidthOfLine[i] = origWidthOfLine[i];
         curArrowHeadW[i] = origArrowHeadW[i];
         curArrowHeadH[i] = origArrowHeadH[i];
         lineWidthPixmap[i] = defaultLineWidthPixmap[i];
         shortLineWidthPixmap[i] = defaultShortLineWidthPixmap[i];
         sprintf(gszMsgBox, "%1d", curWidthOfLine[i]);
         curWidthOfLineSpec[i] = UtilStrDup(gszMsgBox);
         sprintf(gszMsgBox, "%s", curArrowHeadWSpec[i]);
         curArrowHeadWSpec[i] = UtilStrDup(gszMsgBox);
         sprintf(gszMsgBox, "%s", curArrowHeadHSpec[i]);
         curArrowHeadHSpec[i] = UtilStrDup(gszMsgBox);
      }
   }
   lineWidth = 0;
   if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"DefaultLineWidth"))!=NULL) {
      lineWidth = atoi(c_ptr);
      if (lineWidth < 0 || lineWidth >= maxLineWidths) {
         fprintf(stderr, TgLoadString(STID_INVALID_XDEF_USE_ALT_VALUE),
               TOOL_NAME, "DefaultLineWidth", c_ptr, 0);
      }
   }
}

static
void SetCustomPatDir()
{
   *customPatDir = '\0';
   if (*cmdLineCustomPatternDir != '\0') {
      UtilStrCpyN(customPatDir, sizeof(customPatDir), cmdLineCustomPatternDir);
   } else if (!PRTGIF || cmdLineOpenDisplay) {
      char *c_ptr=NULL;

      if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"CustomPatternDir"))!=NULL) {
         if (c_ptr != NULL && *c_ptr != '\0' && UtilIsDirectory(c_ptr)) {
            int len=0;

            UtilStrCpyN(customPatDir, sizeof(customPatDir), c_ptr);
            len = strlen(customPatDir);
            if (len > 0 && customPatDir[len-1] == DIR_SEP) {
               customPatDir[len-1] = '\0';
            }
         }
      }
   }
}

void InitPatFill()
{
   register int i;

   pat_w[0] = pat0_width; pat_h[0] = pat0_height; patData[0] = pat0_bits;
   pat_w[1] = pat1_width; pat_h[1] = pat1_height; patData[1] = pat1_bits;
   pat_w[2] = pat2_width; pat_h[2] = pat2_height; patData[2] = pat2_bits;
   pat_w[3] = pat3_width; pat_h[3] = pat3_height; patData[3] = pat3_bits;
   pat_w[4] = pat4_width; pat_h[4] = pat4_height; patData[4] = pat4_bits;
   pat_w[5] = pat5_width; pat_h[5] = pat5_height; patData[5] = pat5_bits;
   pat_w[6] = pat6_width; pat_h[6] = pat6_height; patData[6] = pat6_bits;
   pat_w[7] = pat7_width; pat_h[7] = pat7_height; patData[7] = pat7_bits;
   pat_w[8] = pat8_width; pat_h[8] = pat8_height; patData[8] = pat8_bits;
   pat_w[9] = pat9_width; pat_h[9] = pat9_height; patData[9] = pat9_bits;
   pat_w[10] = pat10_width; pat_h[10] = pat10_height; patData[10] = pat10_bits;
   pat_w[11] = pat11_width; pat_h[11] = pat11_height; patData[11] = pat11_bits;
   pat_w[12] = pat12_width; pat_h[12] = pat12_height; patData[12] = pat12_bits;
   pat_w[13] = pat13_width; pat_h[13] = pat13_height; patData[13] = pat13_bits;
   pat_w[14] = pat14_width; pat_h[14] = pat14_height; patData[14] = pat14_bits;
   pat_w[15] = pat15_width; pat_h[15] = pat15_height; patData[15] = pat15_bits;
   pat_w[16] = pat16_width; pat_h[16] = pat16_height; patData[16] = pat16_bits;
   pat_w[17] = pat17_width; pat_h[17] = pat17_height; patData[17] = pat17_bits;
   pat_w[18] = pat18_width; pat_h[18] = pat18_height; patData[18] = pat18_bits;
   pat_w[19] = pat19_width; pat_h[19] = pat19_height; patData[19] = pat19_bits;
   pat_w[20] = pat20_width; pat_h[20] = pat20_height; patData[20] = pat20_bits;
   pat_w[21] = pat21_width; pat_h[21] = pat21_height; patData[21] = pat21_bits;
   pat_w[22] = pat22_width; pat_h[22] = pat22_height; patData[22] = pat22_bits;
   pat_w[23] = pat23_width; pat_h[23] = pat23_height; patData[23] = pat23_bits;
   pat_w[24] = pat24_width; pat_h[24] = pat24_height; patData[24] = pat24_bits;
   pat_w[25] = pat25_width; pat_h[25] = pat25_height; patData[25] = pat25_bits;
   pat_w[26] = pat26_width; pat_h[26] = pat26_height; patData[26] = pat26_bits;
   pat_w[27] = pat27_width; pat_h[27] = pat27_height; patData[27] = pat27_bits;
   pat_w[28] = pat28_width; pat_h[28] = pat28_height; patData[28] = pat28_bits;
   pat_w[29] = pat29_width; pat_h[29] = pat29_height; patData[29] = pat29_bits;
   pat_w[30] = pat30_width; pat_h[30] = pat30_height; patData[30] = pat30_bits;
   pat_w[31] = pat31_width; pat_h[31] = pat31_height; patData[31] = pat31_bits;
   pat_w[32] = pat32_width; pat_h[32] = pat32_height; patData[32] = pat32_bits;

   for (i=0; i < MAXPATTERNS+1; i++) {
      needToFreePat[i] = FALSE;
   }
}

static int gnPatFillInitialized=FALSE;

void Init2PatFill()
{
   register int i;

   if (gnPatFillInitialized) return;

   SetCustomPatDir();

   for (i=3; i < MAXPATTERNS+1; i++) {
      if (*customPatDir != '\0') {
         unsigned int w_return=0, h_return=0;
         int x_hot=0, y_hot=0;
         unsigned char *data_return=NULL;
         char path[MAXPATHLENGTH<<1];

         snprintf(path, sizeof(path), "%s%cpat%1d.xbm", customPatDir, DIR_SEP,
               i);
         if (UtilPathExists(path)) {
            if (XReadBitmapFileData(path, &w_return, &h_return, &data_return,
                  &x_hot, &y_hot) == BitmapSuccess) {
               pat_w[i] = w_return;
               pat_h[i] = h_return;
               patData[i] = data_return;
               needToFreePat[i] = TRUE;
            } else {
               fprintf(stderr, TgLoadString(STID_INVALID_XBM_CUST_PAT), path);
               fprintf(stderr, "\n");
            }
         }
      }
   }
   gnPatFillInitialized = TRUE;
}

void InitPattern()
{
   register int i;
   XGCValues values;

   values.foreground = myFgPixel;
   values.background = (threeDLook ? myLtGryPixel : myBgPixel);
   values.fill_style = FillOpaqueStippled;

   rasterGC = XCreateGC(mainDisplay, mainWindow,
         GCForeground | GCBackground | GCFillStyle, &values);

   values.fill_style = FillOpaqueStippled;
   patGC = XCreateGC(mainDisplay, mainWindow,
         GCForeground | GCBackground | GCFillStyle, &values);

   values.background = myBgPixel;
   values.graphics_exposures = False;
   drawGC = XCreateGC(mainDisplay, mainWindow,
         GCBackground | GCGraphicsExposures, &values);
   drawSBGC = XCreateGC(mainDisplay, mainWindow,
         GCBackground | GCGraphicsExposures, &values);

   values.background = (threeDLook ? myLtGryPixel : myBgPixel);
   values.font = defaultFontPtr->fid;
   values.line_width = 1;
   defaultGC = XCreateGC(mainDisplay, mainWindow,
         GCForeground | GCBackground | GCFont | GCLineWidth, &values);

   values.foreground = xorOne;
   values.background = xorZero;
   values.function = GXxor;
   values.fill_style = FillSolid;
   revDefaultGC = XCreateGC(mainDisplay, mainWindow, GCForeground |
         GCBackground | GCFunction | GCFillStyle | GCFont, &values);

   InitPatFill();
   Init2PatFill();
   for (i = 0; i < MAXPATTERNS+1; i++) {
      patPixmap[i] = XCreateBitmapFromData(mainDisplay, mainWindow,
            (char *)patData[i], pat_w[i], pat_h[i]);
   }
   values.foreground = xorOne;
   values.background = xorZero;
   values.function = GXxor;
   values.line_width = 1;
   values.fill_style = FillStippled;
   values.stipple = patPixmap[SCROLLPAT];
   revGrayGC = XCreateGC(mainDisplay, mainWindow,
         GCForeground | GCBackground | GCFunction | GCLineWidth | GCFillStyle |
         GCStipple, &values);

   justPixmap[JUST_L] = XCreateBitmapFromData(mainDisplay, mainWindow,
         (char *)just_l_bits, just_l_width, just_l_height);
   justPixmap[JUST_C] = XCreateBitmapFromData(mainDisplay, mainWindow,
         (char *)just_c_bits, just_c_width, just_c_height);
   justPixmap[JUST_R] = XCreateBitmapFromData(mainDisplay, mainWindow,
         (char *)just_r_bits, just_r_width, just_r_height);

   alignHoriPixmap[ALIGN_N] = XCreateBitmapFromData(mainDisplay, mainWindow,
         (char *)align_n_bits, align_n_width, align_n_height);
   alignHoriPixmap[ALIGN_L] = XCreateBitmapFromData(mainDisplay, mainWindow,
         (char *)align_l_bits, align_l_width, align_l_height);
   alignHoriPixmap[ALIGN_C] = XCreateBitmapFromData(mainDisplay, mainWindow,
         (char *)align_c_bits, align_c_width, align_c_height);
   alignHoriPixmap[ALIGN_R] = XCreateBitmapFromData(mainDisplay, mainWindow,
         (char *)align_r_bits, align_r_width, align_r_height);
   alignHoriPixmap[ALIGN_S] = XCreateBitmapFromData(mainDisplay, mainWindow,
         (char *)align_s_bits, align_s_width, align_s_height);

   alignVertPixmap[ALIGN_N] = XCreateBitmapFromData(mainDisplay, mainWindow,
         (char *)align_n_bits, align_n_width, align_n_height);
   alignVertPixmap[ALIGN_T] = XCreateBitmapFromData(mainDisplay, mainWindow,
         (char *)align_t_bits, align_t_width, align_t_height);
   alignVertPixmap[ALIGN_M] = XCreateBitmapFromData(mainDisplay, mainWindow,
         (char *)align_m_bits, align_m_width, align_m_height);
   alignVertPixmap[ALIGN_B] = XCreateBitmapFromData(mainDisplay, mainWindow,
         (char *)align_b_bits, align_b_width, align_b_height);
   alignVertPixmap[ALIGN_S] = XCreateBitmapFromData(mainDisplay, mainWindow,
         (char *)align_s_bits, align_s_width, align_s_height);

   distrDirectPixmap[0] = XCreateBitmapFromData(mainDisplay, mainWindow,
         (char *)patData[0], pat_w[0], pat_h[0]);
   distrDirectPixmap[1] = XCreateBitmapFromData(mainDisplay, mainWindow,
         (char *)align_t_bits, align_t_width, align_t_height);
   distrDirectPixmap[2] = XCreateBitmapFromData(mainDisplay, mainWindow,
         (char *)align_m_bits, align_m_width, align_m_height);
   distrDirectPixmap[3] = XCreateBitmapFromData(mainDisplay, mainWindow,
         (char *)align_b_bits, align_b_width, align_b_height);
   distrDirectPixmap[4] = XCreateBitmapFromData(mainDisplay, mainWindow,
         (char *)align_ns_bits, align_ns_width, align_ns_height);
   distrDirectPixmap[5] = XCreateBitmapFromData(mainDisplay, mainWindow,
         (char *)align_l_bits, align_l_width, align_l_height);
   distrDirectPixmap[6] = XCreateBitmapFromData(mainDisplay, mainWindow,
         (char *)align_lt_bits, align_lt_width, align_lt_height);
   distrDirectPixmap[7] = XCreateBitmapFromData(mainDisplay, mainWindow,
         (char *)align_lm_bits, align_lm_width, align_lm_height);
   distrDirectPixmap[8] = XCreateBitmapFromData(mainDisplay, mainWindow,
         (char *)align_lb_bits, align_lb_width, align_lb_height);
   distrDirectPixmap[9] = XCreateBitmapFromData(mainDisplay, mainWindow,
         (char *)align_ls_bits, align_ls_width, align_ls_height);
   distrDirectPixmap[10] = XCreateBitmapFromData(mainDisplay, mainWindow,
         (char *)align_c_bits, align_c_width, align_c_height);
   distrDirectPixmap[11] = XCreateBitmapFromData(mainDisplay, mainWindow,
         (char *)align_ct_bits, align_ct_width, align_ct_height);
   distrDirectPixmap[12] = XCreateBitmapFromData(mainDisplay, mainWindow,
         (char *)align_cm_bits, align_cm_width, align_cm_height);
   distrDirectPixmap[13] = XCreateBitmapFromData(mainDisplay, mainWindow,
         (char *)align_cb_bits, align_cb_width, align_cb_height);
   distrDirectPixmap[14] = XCreateBitmapFromData(mainDisplay, mainWindow,
         (char *)align_cs_bits, align_cs_width, align_cs_height);
   distrDirectPixmap[15] = XCreateBitmapFromData(mainDisplay, mainWindow,
         (char *)align_r_bits, align_r_width, align_r_height);
   distrDirectPixmap[16] = XCreateBitmapFromData(mainDisplay, mainWindow,
         (char *)align_rt_bits, align_rt_width, align_rt_height);
   distrDirectPixmap[17] = XCreateBitmapFromData(mainDisplay, mainWindow,
         (char *)align_rm_bits, align_rm_width, align_rm_height);
   distrDirectPixmap[18] = XCreateBitmapFromData(mainDisplay, mainWindow,
         (char *)align_rb_bits, align_rb_width, align_rb_height);
   distrDirectPixmap[19] = XCreateBitmapFromData(mainDisplay, mainWindow,
         (char *)align_rs_bits, align_rs_width, align_rs_height);
   distrDirectPixmap[20] = XCreateBitmapFromData(mainDisplay, mainWindow,
         (char *)align_sn_bits, align_sn_width, align_sn_height);
   distrDirectPixmap[21] = XCreateBitmapFromData(mainDisplay, mainWindow,
         (char *)align_st_bits, align_st_width, align_st_height);
   distrDirectPixmap[22] = XCreateBitmapFromData(mainDisplay, mainWindow,
         (char *)align_sm_bits, align_sm_width, align_sm_height);
   distrDirectPixmap[23] = XCreateBitmapFromData(mainDisplay, mainWindow,
         (char *)align_sb_bits, align_sb_width, align_sb_height);
   distrDirectPixmap[24] = XCreateBitmapFromData(mainDisplay, mainWindow,
         (char *)align_ss_bits, align_ss_width, align_ss_height);

   alignDirectPixmap[0] = distrDirectPixmap[0];
   alignDirectPixmap[1] = distrDirectPixmap[1];
   alignDirectPixmap[2] = distrDirectPixmap[2];
   alignDirectPixmap[3] = distrDirectPixmap[3];
   alignDirectPixmap[4] = distrDirectPixmap[5];
   alignDirectPixmap[5] = distrDirectPixmap[6];
   alignDirectPixmap[6] = distrDirectPixmap[7];
   alignDirectPixmap[7] = distrDirectPixmap[8];
   alignDirectPixmap[8] = distrDirectPixmap[10];
   alignDirectPixmap[9] = distrDirectPixmap[11];
   alignDirectPixmap[10] = distrDirectPixmap[12];
   alignDirectPixmap[11] = distrDirectPixmap[13];
   alignDirectPixmap[12] = distrDirectPixmap[15];
   alignDirectPixmap[13] = distrDirectPixmap[16];
   alignDirectPixmap[14] = distrDirectPixmap[17];
   alignDirectPixmap[15] = distrDirectPixmap[18];

   defaultLineWidthPixmap[0] = XCreateBitmapFromData(mainDisplay, mainWindow,
         (char *)lw0_bits, lw0_width, lw0_height);
   defaultLineWidthPixmap[1] = XCreateBitmapFromData(mainDisplay, mainWindow,
         (char *)lw1_bits, lw1_width, lw1_height);
   defaultLineWidthPixmap[2] = XCreateBitmapFromData(mainDisplay, mainWindow,
         (char *)lw2_bits, lw2_width, lw2_height);
   defaultLineWidthPixmap[3] = XCreateBitmapFromData(mainDisplay, mainWindow,
         (char *)lw3_bits, lw3_width, lw3_height);
   defaultLineWidthPixmap[4] = XCreateBitmapFromData(mainDisplay, mainWindow,
         (char *)lw4_bits, lw4_width, lw4_height);
   defaultLineWidthPixmap[5] = XCreateBitmapFromData(mainDisplay, mainWindow,
         (char *)lw5_bits, lw5_width, lw5_height);
   defaultLineWidthPixmap[6] = XCreateBitmapFromData(mainDisplay, mainWindow,
         (char *)lw6_bits, lw6_width, lw6_height);

   lineStylePixmap[0] = XCreateBitmapFromData(mainDisplay, mainWindow,
         (char *)ls0_bits, ls0_width, ls0_height);
   lineStylePixmap[1] = XCreateBitmapFromData(mainDisplay, mainWindow,
         (char *)ls1_bits, ls1_width, ls1_height);
   lineStylePixmap[2] = XCreateBitmapFromData(mainDisplay, mainWindow,
         (char *)ls2_bits, ls2_width, ls2_height);
   lineStylePixmap[3] = XCreateBitmapFromData(mainDisplay, mainWindow,
         (char *)ls3_bits, ls3_width, ls3_height);

   lineTypePixmap[0] = XCreateBitmapFromData(mainDisplay, mainWindow,
         (char *)lt0_bits, lt0_width, lt0_height);
   lineTypePixmap[1] = XCreateBitmapFromData(mainDisplay, mainWindow,
         (char *)lt1_bits, lt1_width, lt1_height);
   lineTypePixmap[2] = XCreateBitmapFromData(mainDisplay, mainWindow,
         (char *)lt2_bits, lt2_width, lt2_height);
   lineTypePixmap[3] = XCreateBitmapFromData(mainDisplay, mainWindow,
         (char *)lt3_bits, lt3_width, lt3_height);

   lineDashPixmap[0] = XCreateBitmapFromData(mainDisplay, mainWindow,
         (char *)ld0_bits, ld0_width, ld0_height);
   lineDashPixmap[1] = XCreateBitmapFromData(mainDisplay, mainWindow,
         (char *)ld1_bits, ld1_width, ld1_height);
   lineDashPixmap[2] = XCreateBitmapFromData(mainDisplay, mainWindow,
         (char *)ld2_bits, ld2_width, ld2_height);
   lineDashPixmap[3] = XCreateBitmapFromData(mainDisplay, mainWindow,
         (char *)ld3_bits, ld3_width, ld3_height);
   lineDashPixmap[4] = XCreateBitmapFromData(mainDisplay, mainWindow,
         (char *)ld4_bits, ld4_width, ld4_height);
   lineDashPixmap[5] = XCreateBitmapFromData(mainDisplay, mainWindow,
         (char *)ld5_bits, ld5_width, ld5_height);
   lineDashPixmap[6] = XCreateBitmapFromData(mainDisplay, mainWindow,
         (char *)ld6_bits, ld6_width, ld6_height);
   lineDashPixmap[7] = XCreateBitmapFromData(mainDisplay, mainWindow,
         (char *)ld7_bits, ld7_width, ld7_height);
   lineDashPixmap[8] = XCreateBitmapFromData(mainDisplay, mainWindow,
         (char *)ld8_bits, ld8_width, ld8_height);

   shortLineStylePixmap[0] = XCreateBitmapFromData(mainDisplay, mainWindow,
         (char *)ls0s_bits, ls0s_width, ls0s_height);
   shortLineStylePixmap[1] = XCreateBitmapFromData(mainDisplay, mainWindow,
         (char *)ls1s_bits, ls1s_width, ls1s_height);
   shortLineStylePixmap[2] = XCreateBitmapFromData(mainDisplay, mainWindow,
         (char *)ls2s_bits, ls2s_width, ls2s_height);
   shortLineStylePixmap[3] = XCreateBitmapFromData(mainDisplay, mainWindow,
         (char *)ls3s_bits, ls3s_width, ls3s_height);

   filePixmap = XCreateBitmapFromData(mainDisplay, mainWindow,
         (char *)file_bits, file_width, file_height);

   landscapePixmap = XCreateBitmapFromData(mainDisplay, mainWindow,
         (char *)landscap_bits, landscap_width, landscap_height);

   specialPixmap = XCreateBitmapFromData(mainDisplay, mainWindow,
         (char *)special_bits, special_width, special_height);

   vspacePixmap = XCreateBitmapFromData(mainDisplay, mainWindow,
         (char *)vspace_bits, vspace_width, vspace_height);

   rcbRadiusPixmap = XCreateBitmapFromData(mainDisplay, mainWindow,
         (char *)rcb_rad_bits, rcb_rad_width, rcb_rad_height);

   moveModePixmap[CONST_MOVE] = XCreateBitmapFromData(mainDisplay, mainWindow,
         (char *)const_mv_bits, const_mv_width, const_mv_height);

   moveModePixmap[UNCONST_MOVE] = XCreateBitmapFromData(mainDisplay,
         mainWindow, (char *)uncon_mv_bits, uncon_mv_width,
         uncon_mv_height);

   editPixmap = XCreateBitmapFromData(mainDisplay, mainWindow,
         (char *)edit_bits, edit_width, edit_height);

   intrPixmap[0] = XCreateBitmapFromData(mainDisplay, mainWindow,
         (char *)intr_bits, intr_width, intr_height);
   intrPixmap[1] = XCreateBitmapFromData(mainDisplay, mainWindow,
         (char *)intr90_bits, intr90_width, intr90_height);

   trekPixmap = XCreateBitmapFromData(mainDisplay, mainWindow,
         (char *)trek_bits, trek_width, trek_height);

   pageLayoutPixmap[PAGE_STACK] = XCreateBitmapFromData(mainDisplay,
         mainWindow, (char *)stack_bits, stack_width, stack_height);

   pageLayoutPixmap[PAGE_TILE] = XCreateBitmapFromData(mainDisplay,
         mainWindow, (char *)tile_bits, tile_width, tile_height);

   scrollPixmap[SCROLL_LEFTEND] = XCreateBitmapFromData(mainDisplay,
         mainWindow, (char *)leftend_bits, leftend_width, leftend_height);
   scrollPixmap[SCROLL_LEFT] = XCreateBitmapFromData(mainDisplay,
         mainWindow, (char *)lfarrow_bits, lfarrow_width, lfarrow_height);
   scrollPixmap[SCROLL_RIGHT] = XCreateBitmapFromData(mainDisplay,
         mainWindow, (char *)rtarrow_bits, rtarrow_width, rtarrow_height);
   scrollPixmap[SCROLL_RIGHTEND] = XCreateBitmapFromData(mainDisplay,
         mainWindow, (char *)rightend_bits, rightend_width, rightend_height);
   scrollPixmap[SCROLL_UPEND] = XCreateBitmapFromData(mainDisplay,
         mainWindow, (char *)upend_bits, upend_width, upend_height);
   scrollPixmap[SCROLL_UP] = XCreateBitmapFromData(mainDisplay,
         mainWindow, (char *)uparrow_bits, uparrow_width, uparrow_height);
   scrollPixmap[SCROLL_CHECKALL] = XCreateBitmapFromData(mainDisplay,
         mainWindow, (char *)chkall_bits, chkall_width, chkall_height);
   scrollPixmap[SCROLL_UNCHECKALL] = XCreateBitmapFromData(mainDisplay,
         mainWindow, (char *)unchkall_bits, unchkall_width, unchkall_height);
   scrollPixmap[SCROLL_DOWN] = XCreateBitmapFromData(mainDisplay,
         mainWindow, (char *)dnarrow_bits, dnarrow_width, dnarrow_height);
   scrollPixmap[SCROLL_DOWNEND] = XCreateBitmapFromData(mainDisplay,
         mainWindow, (char *)downend_bits, downend_width, downend_height);

   scrlBitmap[SCRL_UP] = XCreateBitmapFromData(mainDisplay,
         mainWindow, (char *)scrl_up_bits, scrl_up_width, scrl_up_height);
   scrlBitmap[SCRL_DN] = XCreateBitmapFromData(mainDisplay,
         mainWindow, (char *)scrl_dn_bits, scrl_dn_width, scrl_dn_height);
   scrlBitmap[SCRL_LF] = XCreateBitmapFromData(mainDisplay,
         mainWindow, (char *)scrl_lf_bits, scrl_lf_width, scrl_lf_height);
   scrlBitmap[SCRL_RT] = XCreateBitmapFromData(mainDisplay,
         mainWindow, (char *)scrl_rt_bits, scrl_rt_width, scrl_rt_height);

   shape_w[0] = shape0_width; shape_h[0] = shape0_height; shapeData[0] = shape0_bits;
   shape_w[1] = shape1_width; shape_h[1] = shape1_height; shapeData[1] = shape1_bits;
   shape_w[2] = shape2_width; shape_h[2] = shape2_height; shapeData[2] = shape2_bits;
   shape_w[3] = shape3_width; shape_h[3] = shape3_height; shapeData[3] = shape3_bits;
   shape_w[4] = shape4_width; shape_h[4] = shape4_height; shapeData[4] = shape4_bits;
   shape_w[5] = shape5_width; shape_h[5] = shape5_height; shapeData[5] = shape5_bits;
   shape_w[6] = shape6_width; shape_h[6] = shape6_height; shapeData[6] = shape6_bits;
   shape_w[7] = shape7_width; shape_h[7] = shape7_height; shapeData[7] = shape7_bits;
   shape_w[8] = shape8_width; shape_h[8] = shape8_height; shapeData[8] = shape8_bits;
   shape_w[9] = shape9_width; shape_h[9] = shape9_height; shapeData[9] = shape9_bits;
   shape_w[10] = shape10_width; shape_h[10] = shape10_height; shapeData[10] = shape10_bits;
   shape_w[11] = shape11_width; shape_h[11] = shape11_height; shapeData[11] = shape11_bits;
   shape_w[12] = shape12_width; shape_h[12] = shape12_height; shapeData[12] = shape12_bits;
   shape_w[13] = shape13_width; shape_h[13] = shape13_height; shapeData[13] = shape13_bits;
   shape_w[14] = shape14_width; shape_h[14] = shape14_height; shapeData[14] = shape14_bits;
   shape_w[15] = shape15_width; shape_h[15] = shape15_height; shapeData[15] = shape15_bits;
   shape_w[16] = shape16_width; shape_h[16] = shape16_height; shapeData[16] = shape16_bits;
   shape_w[17] = shape17_width; shape_h[17] = shape17_height; shapeData[17] = shape17_bits;
   shape_w[18] = shape18_width; shape_h[18] = shape18_height; shapeData[18] = shape18_bits;
   shape_w[19] = shape19_width; shape_h[19] = shape19_height; shapeData[19] = shape19_bits;

   for (i=0; i < MAXSHAPES; i++) {
      shapePixmap[i] = XCreateBitmapFromData(mainDisplay, mainWindow,
            (char *)shapeData[i], shape_w[i], shape_h[i]);
   }
   stretchableModePixmap[NO_STRETCHABLE_TEXT] = XCreateBitmapFromData(
         mainDisplay, mainWindow, (char *)nstretch_bits, nstretch_width,
         nstretch_height);
   stretchableModePixmap[STRETCHABLE_TEXT] = XCreateBitmapFromData(mainDisplay,
         mainWindow, (char *)stretch_bits, stretch_width, stretch_height);

   transPatPixmap[NO_TRANSPAT_MODE] = XCreateBitmapFromData(mainDisplay,
         mainWindow, (char *)patopaq_bits, patopaq_width, patopaq_height);
   transPatPixmap[TRANSPAT_MODE] = XCreateBitmapFromData(mainDisplay,
         mainWindow, (char *)pattrans_bits, pattrans_width, pattrans_height);

   rotatePixmap[0] = XCreateBitmapFromData(mainDisplay, mainWindow,
         (char *)rot_0_bits, rot_0_width, rot_0_height);
   rotatePixmap[1] = XCreateBitmapFromData(mainDisplay, mainWindow,
         (char *)rot_90_bits, rot_90_width, rot_90_height);
   rotatePixmap[2] = XCreateBitmapFromData(mainDisplay, mainWindow,
         (char *)rot_180_bits, rot_180_width, rot_180_height);
   rotatePixmap[3] = XCreateBitmapFromData(mainDisplay, mainWindow,
         (char *)rot_270_bits, rot_270_width, rot_270_height);

   whereToPrintPixmap = (Pixmap*)malloc(MAXDEFWHERETOPRINT*sizeof(Pixmap));
   if (whereToPrintPixmap == NULL) FailAllocMessage();
   whereToPrintPixmap[PRINTER] = XCreateBitmapFromData(mainDisplay, mainWindow,
         (char *)printer_bits, printer_width, printer_height);
   whereToPrintPixmap[LATEX_FIG] = XCreateBitmapFromData(mainDisplay,
         mainWindow, (char *)latex_bits, latex_width, latex_height);
   whereToPrintPixmap[PS_FILE] = XCreateBitmapFromData(mainDisplay,
         mainWindow, (char *)psfile_bits, psfile_width, psfile_height);
   whereToPrintPixmap[XBM_FILE] = XCreateBitmapFromData(mainDisplay,
         mainWindow, (char *)xbm_bits, xbm_width, xbm_height);
   whereToPrintPixmap[TEXT_FILE] = XCreateBitmapFromData(mainDisplay,
         mainWindow, (char *)ascii_bits, ascii_width, ascii_height);
   whereToPrintPixmap[EPSI_FILE] = XCreateBitmapFromData(mainDisplay,
         mainWindow, (char *)epsi_bits, epsi_width, epsi_height);
   whereToPrintPixmap[GIF_FILE] = XCreateBitmapFromData(mainDisplay,
         mainWindow, (char *)gif_bits, gif_width, gif_height);
   whereToPrintPixmap[HTML_FILE] = XCreateBitmapFromData(mainDisplay,
         mainWindow, (char *)html_bits, html_width, html_height);
   whereToPrintPixmap[PDF_FILE] = XCreateBitmapFromData(mainDisplay,
         mainWindow, (char *)pdf_bits, pdf_width, pdf_height);
   whereToPrintPixmap[TIFFEPSI_FILE] = XCreateBitmapFromData(mainDisplay,
         mainWindow, (char *)tiffepsi_bits, tiffepsi_width, tiffepsi_height);
   whereToPrintPixmap[PNG_FILE] = XCreateBitmapFromData(mainDisplay,
         mainWindow, (char *)png_bits, png_width, png_height);
   whereToPrintPixmap[JPEG_FILE] = XCreateBitmapFromData(mainDisplay,
         mainWindow, (char *)jpeg_bits, jpeg_width, jpeg_height);
   whereToPrintPixmap[PPM_FILE] = XCreateBitmapFromData(mainDisplay,
         mainWindow, (char *)pbm_bits, pbm_width, pbm_height);
   whereToPrintPixmap[NETLIST_FILE] = XCreateBitmapFromData(mainDisplay,
         mainWindow, (char *)netlist_bits, netlist_width, netlist_height);
   whereToPrintPixmap[SVG_FILE] = XCreateBitmapFromData(mainDisplay,
         mainWindow, (char *)svg_bits, svg_width, svg_height);

   shortLineTypePixmap[0] = XCreateBitmapFromData(mainDisplay, mainWindow,
         (char *)lt0s_bits, lt0s_width, lt0s_height);
   shortLineTypePixmap[1] = XCreateBitmapFromData(mainDisplay, mainWindow,
         (char *)lt1s_bits, lt1s_width, lt1s_height);
   shortLineTypePixmap[2] = XCreateBitmapFromData(mainDisplay, mainWindow,
         (char *)lt2s_bits, lt2s_width, lt2s_height);
   shortLineTypePixmap[3] = XCreateBitmapFromData(mainDisplay, mainWindow,
         (char *)lt3s_bits, lt3s_width, lt3s_height);

   shortDashPixmap[0] = XCreateBitmapFromData(mainDisplay, mainWindow,
         (char *)ld0s_bits, ld0s_width, ld0s_height);
   shortDashPixmap[1] = XCreateBitmapFromData(mainDisplay, mainWindow,
         (char *)ld1s_bits, ld1s_width, ld1s_height);
   shortDashPixmap[2] = XCreateBitmapFromData(mainDisplay, mainWindow,
         (char *)ld2s_bits, ld2s_width, ld2s_height);
   shortDashPixmap[3] = XCreateBitmapFromData(mainDisplay, mainWindow,
         (char *)ld3s_bits, ld3s_width, ld3s_height);
   shortDashPixmap[4] = XCreateBitmapFromData(mainDisplay, mainWindow,
         (char *)ld4s_bits, ld4s_width, ld4s_height);
   shortDashPixmap[5] = XCreateBitmapFromData(mainDisplay, mainWindow,
         (char *)ld5s_bits, ld5s_width, ld5s_height);
   shortDashPixmap[6] = XCreateBitmapFromData(mainDisplay, mainWindow,
         (char *)ld6s_bits, ld6s_width, ld6s_height);
   shortDashPixmap[7] = XCreateBitmapFromData(mainDisplay, mainWindow,
         (char *)ld7s_bits, ld7s_width, ld7s_height);
   shortDashPixmap[8] = XCreateBitmapFromData(mainDisplay, mainWindow,
         (char *)ld8s_bits, ld8s_width, ld8s_height);

   defaultShortLineWidthPixmap[0] = XCreateBitmapFromData(mainDisplay,
         mainWindow, (char *)lw0s_bits, lw0s_width, lw0s_height);
   defaultShortLineWidthPixmap[1] = XCreateBitmapFromData(mainDisplay,
         mainWindow, (char *)lw1s_bits, lw1s_width, lw1s_height);
   defaultShortLineWidthPixmap[2] = XCreateBitmapFromData(mainDisplay,
         mainWindow, (char *)lw2s_bits, lw2s_width, lw2s_height);
   defaultShortLineWidthPixmap[3] = XCreateBitmapFromData(mainDisplay,
         mainWindow, (char *)lw3s_bits, lw3s_width, lw3s_height);
   defaultShortLineWidthPixmap[4] = XCreateBitmapFromData(mainDisplay,
         mainWindow, (char *)lw4s_bits, lw4s_width, lw4s_height);
   defaultShortLineWidthPixmap[5] = XCreateBitmapFromData(mainDisplay,
         mainWindow, (char *)lw5s_bits, lw5s_width, lw5s_height);
   defaultShortLineWidthPixmap[6] = XCreateBitmapFromData(mainDisplay,
         mainWindow, (char *)lw6s_bits, lw6s_width, lw6s_height);

   rotateModePixmap[0] = XCreateBitmapFromData(mainDisplay, mainWindow,
         (char *)rot_mode_bits, rot_mode_width, rot_mode_height);
   rotateModePixmap[1] = XCreateBitmapFromData(mainDisplay, mainWindow,
         (char *)rot_mode2_bits, rot_mode2_width, rot_mode2_height);

   choicePixmap[NOTHING] = XCreateBitmapFromData(mainDisplay, mainWindow,
         (char *)arrow_bits, arrow_width, arrow_height);
   choicePixmap[DRAWTEXT] = XCreateBitmapFromData(mainDisplay, mainWindow,
         (char *)text_bits, text_width, text_height);
   choicePixmap[DRAWBOX] = XCreateBitmapFromData(mainDisplay, mainWindow,
         (char *)box_bits, box_width, box_height);
   choicePixmap[DRAWCORNEROVAL] = XCreateBitmapFromData(mainDisplay, mainWindow,
         (char *)oval_bits, oval_width, oval_height);
   choicePixmap[DRAWCENTEROVAL] = XCreateBitmapFromData(mainDisplay, mainWindow,
         (char *)ovalcent_bits, ovalcent_width, ovalcent_height);
   choicePixmap[DRAWEDGECIRCLE] = XCreateBitmapFromData(mainDisplay, mainWindow,
         (char *)ovaledge_bits, ovaledge_width, ovaledge_height);
   choicePixmap[DRAWPOLY] = XCreateBitmapFromData(mainDisplay, mainWindow,
         (char *)poly_bits, poly_width, poly_height);
   choicePixmap[DRAWPOLYGON] = XCreateBitmapFromData(mainDisplay, mainWindow,
         (char *)polygon_bits, polygon_width, polygon_height);
   choicePixmap[DRAWARC] = XCreateBitmapFromData(mainDisplay, mainWindow,
         (char *)arc_bits, arc_width, arc_height);
   choicePixmap[DRAWEDGEARC] = XCreateBitmapFromData(mainDisplay, mainWindow,
         (char *)arc_edge_bits, arc_edge_width, arc_edge_height);
   choicePixmap[DRAWRCBOX] = XCreateBitmapFromData(mainDisplay, mainWindow,
         (char *)rcbox_bits, rcbox_width, rcbox_height);
   choicePixmap[FREEHAND] = XCreateBitmapFromData(mainDisplay, mainWindow,
         (char *)freehand_bits, freehand_width, freehand_height);
   choicePixmap[VERTEXMODE] = XCreateBitmapFromData(mainDisplay, mainWindow,
         (char *)vmode_bits, vmode_width, vmode_height);
   choicePixmap[ROTATEMODE] = rotateModePixmap[1];

   wireBitmap = XCreateBitmapFromData(mainDisplay, mainWindow,
         (char *)wire_bits, wire_width, wire_height);

   runBitmap = XCreateBitmapFromData(mainDisplay, mainWindow,
         (char *)run_bits, run_width, run_height);

   checkBitmap = XCreateBitmapFromData(mainDisplay, mainWindow,
         (char *)check_bits, check_width, check_height);

   radioBitmap = XCreateBitmapFromData(mainDisplay, mainWindow,
         (char *)radio_bits, radio_width, radio_height);

   submenuBitmap = XCreateBitmapFromData(mainDisplay, mainWindow,
         (char *)submenu_bits, submenu_width, submenu_height);
   graySubmenuBitmap = XCreateBitmapFromData(mainDisplay, mainWindow,
         (char *)gsubmenu_bits, gsubmenu_width, gsubmenu_height);

   userRedrawBitmap = XCreateBitmapFromData(mainDisplay, mainWindow,
         (char *)redraw_bits, redraw_width, redraw_height);

   statusBtnPixmap[0] = XCreateBitmapFromData(mainDisplay, mainWindow,
         (char *)btn1_bits, btn1_width, btn1_height);
   statusBtnPixmap[1] = XCreateBitmapFromData(mainDisplay, mainWindow,
         (char *)btn2_bits, btn2_width, btn2_height);
   statusBtnPixmap[2] = XCreateBitmapFromData(mainDisplay, mainWindow,
         (char *)btn3_bits, btn3_width, btn3_height);

   msgBoxPixmap[0] = XCreateBitmapFromData(mainDisplay, mainWindow,
         (char *)stop_bits, stop_width, stop_height);
   msgBoxPixmap[1] = XCreateBitmapFromData(mainDisplay, mainWindow,
         (char *)question_bits, question_width, question_height);
   msgBoxPixmap[2] = XCreateBitmapFromData(mainDisplay, mainWindow,
         (char *)info_bits, info_width, info_height);
   msgBoxPixmap[3] = XCreateBitmapFromData(mainDisplay, mainWindow,
         (char *)dialog_bits, dialog_width, dialog_height);

   InitLines();
}

void RedrawChoiceWindow()
{
   if (threeDLook) {
      struct BBRec bbox;

      bbox.ltx = 0;
      bbox.lty = 0;
      bbox.rbx = choiceWindowW;
      bbox.rby = choiceWindowH;
      TgDrawThreeDButton(mainDisplay, choiceWindow, textMenuGC, &bbox,
            TGBS_RAISED, 1, FALSE);
   }
   ShowColor(FALSE);
   ShowFile();
   ShowWhereToPrint();
   ShowHoriAlign();
   ShowVertAlign();
   ShowRotate();
   ShowMoveMode();
   ShowRCBRadius();
   ShowZoom();
   ShowJust();
   ShowCurFont();
   ShowTextSize();
   ShowTextVSpace();
   ShowShape();
   ShowStretchableTextMode();
   ShowLineWidth();
   ShowLineStyle();
   ShowLineType();
   ShowDash();
   ShowFill();
   ShowTransPatMode();
   ShowPen();
   ShowPage();
   ShowPageLayout();
}

void CleanUpPatFill()
{
   register int i;

   if (!gnPatFillInitialized) return;

   for (i = 0; i < MAXPATTERNS+1; i++) {
      if (needToFreePat[i]) {
         XFree(patData[i]);
      }
   }
   gnPatFillInitialized = FALSE;
}

void CleanUpRasters()
{
   register int i;

   CleanUpPatFill();
   XFreePixmap(mainDisplay, rotateModePixmap[0]);
   XFreePixmap(mainDisplay, rotateModePixmap[1]);
   for (i = 0; i < MAXCHOICES-1; i++) XFreePixmap(mainDisplay, choicePixmap[i]);
   for (i = 0; i < MAXPATTERNS+1; i++) XFreePixmap(mainDisplay, patPixmap[i]);
   for (i = 0; i < MAXLINEWIDTHS; i++) {
      XFreePixmap(mainDisplay, defaultLineWidthPixmap[i]);
   }
   for (i = 0; i < MAXLINESTYLES; i++) {
      XFreePixmap(mainDisplay, lineStylePixmap[i]);
   }
   for (i = 0; i < MAXLINETYPES; i++) {
      XFreePixmap(mainDisplay, lineTypePixmap[i]);
   }
   for (i = 0; i < MAXDASHES; i++) {
      XFreePixmap(mainDisplay, lineDashPixmap[i]);
   }
   for (i = 0; i < MAXJUSTS; i++) XFreePixmap(mainDisplay, justPixmap[i]);
   for (i = 0; i < MAXALIGNS; i++) {
      XFreePixmap(mainDisplay, alignHoriPixmap[i]);
   }
   for (i = 0; i < MAXALIGNS; i++) {
      XFreePixmap(mainDisplay, alignVertPixmap[i]);
   }
   for (i = 0; i < MAXALIGNS*MAXALIGNS; i++) {
      XFreePixmap(mainDisplay, distrDirectPixmap[i]);
   }
   for (i = 0; i < MAXLINEWIDTHS; i++) {
      XFreePixmap(mainDisplay, defaultShortLineWidthPixmap[i]);
   }
   XFreePixmap(mainDisplay, filePixmap);
   XFreePixmap(mainDisplay, landscapePixmap);
   XFreePixmap(mainDisplay, specialPixmap);
   XFreePixmap(mainDisplay, vspacePixmap);
   XFreePixmap(mainDisplay, rcbRadiusPixmap);
   for (i = 0; i < MAXMOVEMODES; i++) {
      XFreePixmap(mainDisplay, moveModePixmap[i]);
   }
   XFreePixmap(mainDisplay, editPixmap);
   for (i = 0; i < MAXINTRS; i++) {
      XFreePixmap(mainDisplay, intrPixmap[i]);
   }
   XFreePixmap(mainDisplay, trekPixmap);
   for (i = 0; i < MAXPAGELAYOUTMODES; i++) {
      XFreePixmap(mainDisplay, pageLayoutPixmap[i]);
   }
   for (i = 0; i < MAXSCROLLBUTTONS; i++) {
      XFreePixmap(mainDisplay, scrollPixmap[i]);
   }
   for (i = 0; i < 4; i++) {
      XFreePixmap(mainDisplay, scrlBitmap[i]);
   }
   for (i = 0; i < MAXSHAPES; i++) {
      XFreePixmap(mainDisplay, shapePixmap[i]);
   }
   for (i = 0; i < MAXSTRETCHABLEMODES; i++) {
      XFreePixmap(mainDisplay, stretchableModePixmap[i]);
   }
   for (i = 0; i < MAXTRANSPATMODES; i++) {
      XFreePixmap(mainDisplay, transPatPixmap[i]);
   }
   for (i = 0; i < 4; i++) {
      XFreePixmap(mainDisplay, rotatePixmap[i]);
   }
   for (i = 0; i < MAXDEFWHERETOPRINT; i++) {
      XFreePixmap(mainDisplay, whereToPrintPixmap[i]);
   }
   free(whereToPrintPixmap);
   whereToPrintPixmap = NULL;
   for (i = 0; i < MAXLINETYPES; i++) {
      XFreePixmap(mainDisplay, shortLineTypePixmap[i]);
   }
   for (i = 0; i < MAXLINESTYLES; i++) {
      XFreePixmap(mainDisplay, shortLineStylePixmap[i]);
   }
   for (i = 0; i < MAXDASHES; i++) {
      XFreePixmap(mainDisplay, shortDashPixmap[i]);
   }
   XFreePixmap(mainDisplay, wireBitmap);
   XFreePixmap(mainDisplay, runBitmap);
   XFreePixmap(mainDisplay, checkBitmap);
   XFreePixmap(mainDisplay, radioBitmap);
   XFreePixmap(mainDisplay, submenuBitmap);
   XFreePixmap(mainDisplay, graySubmenuBitmap);
   XFreePixmap(mainDisplay, userRedrawBitmap);
   for (i = 0; i < MAX_STATUS_BTNS; i++) {
      XFreePixmap(mainDisplay, statusBtnPixmap[i]);
   }
   for (i = 0; i < MAX_MB_ICONS; i++) {
      XFreePixmap(mainDisplay, msgBoxPixmap[i]);
   }
   if (newLineWidthAllocated) {
      for (i = 0; i < maxLineWidths; i++)
         XFreePixmap(mainDisplay, lineWidthPixmap[i]);
      for (i = 0; i < maxLineWidths; i++)
         XFreePixmap(mainDisplay, shortLineWidthPixmap[i]);
   }
   free(lineWidthPixmap); lineWidthPixmap = NULL;
   free(shortLineWidthPixmap); shortLineWidthPixmap = NULL;
   free(curWidthOfLine); curWidthOfLine = NULL;
   free(curArrowHeadW); curArrowHeadW = NULL;
   free(curArrowHeadH); curArrowHeadH = NULL;
   for (i=0; i < maxLineWidths; i++) {
      if (curWidthOfLineSpec[i] != NULL) free(curWidthOfLineSpec[i]);
      if (curArrowHeadWSpec[i] != NULL) free(curArrowHeadWSpec[i]);
      if (curArrowHeadHSpec[i] != NULL) free(curArrowHeadHSpec[i]);
   }
   free(curWidthOfLineSpec); curWidthOfLineSpec = NULL;
   free(curArrowHeadWSpec); curArrowHeadWSpec = NULL;
   free(curArrowHeadHSpec); curArrowHeadHSpec = NULL;

   XFreeGC(mainDisplay, rasterGC);
   XFreeGC(mainDisplay, patGC);
   XFreeGC(mainDisplay, drawGC); drawGC = NULL;
   XFreeGC(mainDisplay, drawSBGC); drawSBGC = NULL;
   XFreeGC(mainDisplay, defaultGC); defaultGC = NULL;
   XFreeGC(mainDisplay, revDefaultGC); revDefaultGC = NULL;
   XFreeGC(mainDisplay, revGrayGC); revGrayGC = NULL;
}

int UpgradePenFill(pen_or_fill)
   int pen_or_fill;
{
   if (fileVersion <= 17 && pen_or_fill >= 8 && pen_or_fill < 12) {
      return (pen_or_fill+12);
   } else {
      return pen_or_fill;
   }
}

unsigned char ByteReverse(byte)
   unsigned char byte;
{
   int b=0;
   unsigned char flip=0, mask=1;

   for (b=0; b < 8; b++) {
      flip <<= 1;
      flip += byte&mask;
      byte >>= 1;
   }
   return flip;
}

void GetPatternCellsize(fill_or_pen, pn_cell_w, pn_cell_h)
   int fill_or_pen, *pn_cell_w, *pn_cell_h;
{
   int w=pat_w[fill_or_pen];
   int bits_in_last_byte=(w & 0x07);
   int bytes_per_row_out=0;

   switch (bits_in_last_byte) {
   case 1: bytes_per_row_out = (w+1); break;
   case 2: bytes_per_row_out = ((w>>3)<<2)+1; break;
   case 3: bytes_per_row_out = (w+3); break;
   case 4: bytes_per_row_out = ((w>>3)<<1)+1; break;
   case 5: bytes_per_row_out = (w+5); break;
   case 6: bytes_per_row_out = ((w>>3)<<2)+3; break;
   case 7: bytes_per_row_out = (w+7); break;
   default: bytes_per_row_out = (w>>3); break;
   }
   if (pn_cell_w != NULL) *pn_cell_w = (bytes_per_row_out<<3);
   if (pn_cell_h != NULL) *pn_cell_h = pat_h[fill_or_pen];
}

static
void SetBit(buf, bit_offset, ival)
   unsigned char *buf;
   int bit_offset, ival;
{
   int byte_index=(bit_offset>>3);
   int bit_in_byte=(7-(bit_offset&0x7));
   unsigned char *psz=(&buf[byte_index]);

   if (ival) {
      *psz |= (((unsigned char)0x1) << bit_in_byte);
   } else {
      *psz &= (~(((unsigned char)0x1) << bit_in_byte));
   }
}

static
unsigned char GetBit(buf, bit_offset)
   unsigned char *buf;
   int bit_offset;
{
   int byte_index=(bit_offset>>3);
   int bit_in_byte=(7-(bit_offset&0x7));
   unsigned char uch=buf[byte_index];

   return ((uch >> bit_in_byte) & ((unsigned char)0x1));
}

unsigned char *GetPatternBytes(fill_or_pen, pn_num_bytes, pn_need_to_free)
   int fill_or_pen, *pn_num_bytes, *pn_need_to_free;
{
   int w=pat_w[fill_or_pen];
   int h=pat_h[fill_or_pen];
   int bits_in_last_byte=(w & 0x07);
   int multiple_of_8=(bits_in_last_byte == 0);
   int bytes_per_row_in=0, bytes_per_row_out=0, total_bytes=0;

   bytes_per_row_in = (multiple_of_8 ? (w>>3) : (w>>3)+1);

   switch (bits_in_last_byte) {
   case 1: bytes_per_row_out = (w+1); break;
   case 2: bytes_per_row_out = ((w>>3)<<2)+1; break;
   case 3: bytes_per_row_out = (w+3); break;
   case 4: bytes_per_row_out = ((w>>3)<<1)+1; break;
   case 5: bytes_per_row_out = (w+5); break;
   case 6: bytes_per_row_out = ((w>>3)<<2)+3; break;
   case 7: bytes_per_row_out = (w+7); break;
   default: bytes_per_row_out = (w>>3); break;
   }
   total_bytes = h * bytes_per_row_out;

   if (pn_num_bytes != NULL) *pn_num_bytes = total_bytes;
   if (!multiple_of_8) {
      unsigned char *buf=(unsigned char *)malloc(total_bytes);
      unsigned char *buf_in=patData[fill_or_pen], *buf_out=NULL;
      int r=0, i=0;

      if (buf == NULL) FailAllocMessage();
      /* first reverse all the bits in buf_in */
      for (i=0; i < bytes_per_row_in*h; i++) {
         buf_in[i] = ByteReverse(buf_in[i]);
      }
      buf_out = buf;
      for (r=0; r < h; r++) {
         int c=0;

         for (c=0; c < (bytes_per_row_out<<3); c++) {
            int bit_offset_in=(c % w);

            SetBit(buf_out, c, GetBit(buf_in, bit_offset_in));
         }
         buf_in += bytes_per_row_in;
         buf_out += bytes_per_row_out;
      }
      /* reverse all the bits in buf_in back to what they were */
      buf_in = patData[fill_or_pen];
      for (i=0; i < bytes_per_row_in*h; i++) {
         buf_in[i] = ByteReverse(buf_in[i]);
      }
      for (i=0; i < total_bytes; i++) {
         buf[i] = ByteReverse(buf[i]);
      }
      if (pn_need_to_free != NULL) *pn_need_to_free = TRUE;
      return buf;
   }
   if (pn_need_to_free != NULL) *pn_need_to_free = FALSE;

   return patData[fill_or_pen];
}

