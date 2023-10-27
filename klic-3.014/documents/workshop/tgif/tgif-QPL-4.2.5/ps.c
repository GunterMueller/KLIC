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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/ps.c,v 1.16 2011/05/16 16:21:59 william Exp $
 */

#define _INCLUDE_FROM_PS_C_

#include "tgifdefs.h"

#include "file.e"
#include "msg.e"
#include "ps.e"
#include "raster.e"
#include "util.e"

#define DEF_PS_DICT_COUNT 3

int preDumpSetup=FALSE;
int needsTgifOrigCTM=TRUE;
int psUseShortHand=FALSE;

char **gPsCmd=NULL;

static int psDictCount=DEF_PS_DICT_COUNT;

static int psUsePSAdobe=TRUE;
static int psUseMinRadius=FALSE;
static int psUseEllipse=FALSE;
static int psUseArrow=FALSE;
static int psUseArc=FALSE;
static int psUsePattern=FALSE;
static int psUseBWPattern=FALSE;
static int psUseColorImage=FALSE;
static int psUseColorPattern=FALSE;
static int psUseCenterText=FALSE;
static int psUseRightText=FALSE;
static int psUseMiniLines=FALSE;
static int psUseReencode=FALSE;
static int psUseReencodeFF8=FALSE;

#ifdef _PS_USE_EUC_FONT
static int psUseEucFont=FALSE;
#endif /* _PS_USE_EUC_FONT */

void PSUsePSAdobe()
{
   if (preDumpSetup && !psUsePSAdobe) {
      psUsePSAdobe = FALSE;
   }
}

void PSUseMinRadius()
{
   if (preDumpSetup && !psUseMinRadius) {
      psUseMinRadius = TRUE;
      psDictCount++;
   }
}

void PSUseEllipse()
{
   if (preDumpSetup && !psUseEllipse) {
      psUseEllipse = TRUE;
      psDictCount += 2;
   }
}

void PSUseArrow()
{
   if (preDumpSetup && !psUseArrow) {
      psUseArrow = TRUE;
      psDictCount += 2;
   }
}

void PSUseArc()
{
   if (preDumpSetup && !psUseArc) {
      psUseArc = TRUE;
      psDictCount += 3;
   }
}

static
void PSUsePattern()
{
   if (preDumpSetup && !psUsePattern) {
      psUsePattern = TRUE;
      psDictCount += 29;
   }
}

void PSUseBWPattern()
{
   if (preDumpSetup && !psUseBWPattern) {
      PSUsePattern();
      psUseBWPattern = TRUE;
      psDictCount += 5;
   }
}

void PSUseColorImage()
{
   if (preDumpSetup && !psUseColorImage) {
      psUseColorImage = TRUE;
      psDictCount += 13;
   }
}

void PSUseColorPattern()
{
   if (preDumpSetup && !psUseColorPattern) {
      PSUsePattern();
      psUseColorPattern = TRUE;
      psDictCount += 4;
   }
}

void PSUseCenterText()
{
   if (preDumpSetup && !psUseCenterText) {
      psUseCenterText = TRUE;
      psDictCount += 1;
   }
}

void PSUseRightText()
{
   if (preDumpSetup && !psUseRightText) {
      psUseRightText = TRUE;
      psDictCount += 1;
   }
}

void PSUseMiniLines()
{
   if (preDumpSetup && !psUseMiniLines) {
      psUseMiniLines = TRUE;
      psDictCount += 3;
   }
}

typedef struct ReEncodeRec {
   char *font_str;
   struct ReEncodeRec *next;
} *ReEncodePtr;

static struct ReEncodeRec *topReEncodeInfo=NULL;

void PSUseReencode(font_str)
   char *font_str;
{
   if (preDumpSetup && !psUseReencode) {
      psUseReencode = TRUE;
      psDictCount += 2;
   }
#ifdef _PRTGIF_USE_ISO_LATIN_1_ALWAYS
   if (PRTGIF && !psUseReencodeFF8) {
      psUseReencodeFF8 = TRUE;
      psDictCount += 2;
   }
#endif /* _PRTGIF_USE_ISO_LATIN_1_ALWAYS */
   if (font_str != NULL && preDumpSetup && psUseReencode) {
      struct ReEncodeRec *p_reencode;

      for (p_reencode=topReEncodeInfo; p_reencode != NULL;
            p_reencode=p_reencode->next) {
         if (strcmp(p_reencode->font_str, font_str) == 0) {
            return;
         }
      }
      p_reencode = (struct ReEncodeRec *)malloc(sizeof(struct ReEncodeRec));
      if (p_reencode == NULL) FailAllocMessage();
      p_reencode->font_str = UtilStrDup(font_str);
      p_reencode->next = topReEncodeInfo;
      topReEncodeInfo = p_reencode;
      psDictCount += 1;
   }
}

static char *gaszPsShortHand[] = {
  "GS",   /* "gsave", */
  "GR",   /* "grestore", */
  "NP",   /* "newpath", */
  "CP",   /* "closepath", */
  "CHP",  /* "charpath", */
  "CT",   /* "curveto", */
  "L",    /* "lineto", */
  "RL",   /* "rlineto", */
  "M",    /* "moveto", */
  "RM",   /* "rmoveto", */
  "S",    /* "stroke", */
  "F",    /* "fill", */
  "TR",   /* "translate", */
  "RO",   /* "rotate", */
  "SC",   /* "scale", */
  "MU",   /* "mul", */
  "DI",   /* "div", */
  "DU",   /* "dup", */
  "NE",   /* "neg", */
  "AD",   /* "add", */
  "SU",   /* "sub", */
  "PO",   /* "pop", */
  "EX",   /* "exch", */
  "CO",   /* "concat", */
  "CL",   /* "clip", */
  "EC",   /* "eoclip", */
  "EF",   /* "eofill", */
  "IM",   /* "image", */
  "IMM",  /* "imagemask", */
  "ARY",  /* "array", */
  "SG",   /* "setgray", */
  "RG",   /* "setrgbcolor", */
  "SD",   /* "setdash", */
  "W",    /* "setlinewidth", */
  "SM",   /* "setmiterlimit", */
  "SLC",  /* "setlinecap", */
  "SLJ",  /* "setlinejoin", */
  "SH",   /* "show", */
  "FF",   /* "findfont", */
  "MS",   /* "makefont setfont", */
  "AR",   /* "arcto 4 {pop} repeat", */
  "CURP", /* "currentpoint", */
  "FLAT", /* "flattenpath strokepath clip newpath", */
  "\tTGSM", /* "tgiforigctm setmatrix", */
  "\tTGRM", /* "savematrix setmatrix", */
  NULL
};

static char *gaszPsLongHand[] = {
  /* "GS",   */ "gsave",
  /* "GR",   */ "grestore",
  /* "NP",   */ "newpath",
  /* "CP",   */ "closepath",
  /* "CHP",  */ "charpath",
  /* "CT",   */ "curveto",
  /* "L",    */ "lineto",
  /* "RL",   */ "rlineto",
  /* "M",    */ "moveto",
  /* "RM",   */ "rmoveto",
  /* "S",    */ "stroke",
  /* "F",    */ "fill",
  /* "TR",   */ "translate",
  /* "RO",   */ "rotate",
  /* "SC",   */ "scale",
  /* "MU",   */ "mul",
  /* "DI",   */ "div",
  /* "DU",   */ "dup",
  /* "NE",   */ "neg",
  /* "AD",   */ "add",
  /* "SU",   */ "sub",
  /* "PO",   */ "pop",
  /* "EX",   */ "exch",
  /* "CO",   */ "concat",
  /* "CL",   */ "clip",
  /* "EC",   */ "eoclip",
  /* "EF",   */ "eofill",
  /* "IM",   */ "image",
  /* "IMM",  */ "imagemask",
  /* "ARY",  */ "array",
  /* "SG",   */ "setgray",
  /* "RG",   */ "setrgbcolor",
  /* "SD",   */ "setdash",
  /* "W",    */ "setlinewidth",
  /* "SM",   */ "setmiterlimit",
  /* "SLC",  */ "setlinecap",
  /* "SLJ",  */ "setlinejoin",
  /* "SH",   */ "show",
  /* "FF",   */ "findfont",
  /* "MS",   */ "makefont setfont",
  /* "AR",   */ "arcto 4 {pop} repeat",
  /* "CURP", */ "currentpoint",
  /* "FLAT", */ "flattenpath strokepath clip newpath",
  /* "TGSM", */ "\ttgiforigctm setmatrix",
  /* "TGRM", */ "\tsavematrix setmatrix",
  NULL
};

void PSDontUseShortHand()
{
   if (preDumpSetup && psUseShortHand) {
      char **ppsz=NULL;

      psUseShortHand = FALSE;
      psDictCount -= 1;
      for (ppsz=gaszPsShortHand; *ppsz != NULL; ppsz++) {
         psDictCount--;
      }
      gPsCmd = gaszPsLongHand;
   }
}

void PSUseShortHand()
{
   if (preDumpSetup && !psUseShortHand) {
      char **ppsz=NULL;

      psUseShortHand = TRUE;
      psDictCount += 1;
      for (ppsz=gaszPsShortHand; *ppsz != NULL; ppsz++) {
         psDictCount++;
      }
      gPsCmd = gaszPsShortHand;
   }
}

#ifdef _PS_USE_EUC_FONT
void PSUseEucFont()
{
   if (preDumpSetup && !psUseEucFont) {
      psUseEucFont = TRUE;
      psDictCount += 4;
   }
}
#endif /* _PS_USE_EUC_FONT */

static
void CleanUpReEncodeInfo()
{
   struct ReEncodeRec *p_next_reencode;

   for ( ; topReEncodeInfo != NULL; topReEncodeInfo=p_next_reencode) {
      p_next_reencode = topReEncodeInfo->next;
      if (topReEncodeInfo->font_str != NULL) free(topReEncodeInfo->font_str);
      free(topReEncodeInfo);
   }
}

void ResetPSInfo()
{
   psDictCount = DEF_PS_DICT_COUNT;
   preDumpSetup = TRUE;

   needsTgifOrigCTM = TRUE;

   psUsePSAdobe = TRUE;
#ifdef INVERT_CTM_BUG
   UseMinRadius();
#else /* ~INVERT_CTM_BUG */
   psUseMinRadius = FALSE;
#endif /* INVERT_CTM_BUG */
   psUseEllipse = FALSE;
   psUseArrow = FALSE;
   psUseArc = FALSE;
   psUsePattern = FALSE;
   psUseBWPattern = FALSE;
   psUseColorImage = FALSE;
   psUseColorPattern = FALSE;
   psUseCenterText = FALSE;
   psUseRightText = FALSE;
   psUseMiniLines = FALSE;
   psUseReencode = FALSE;
   psUseReencodeFF8 = FALSE;
   psUseShortHand = FALSE;
#ifdef _PS_USE_EUC_FONT
   psUseEucFont = FALSE;
#endif /* _PS_USE_EUC_FONT */
   CleanUpReEncodeInfo();
}

void DoneResetPSInfo()
{
   preDumpSetup = FALSE;
   CleanUpReEncodeInfo();
}

static char *psAdobeMacro[] =
{ "%",
  "%\tDue to bugs in Transcript, the 'PS-Adobe-' stuff is omitted from line 1",
  "%",
  "",
  NULL
};

static char *psMinRadiusMacro[] = {
  "%",
  "%\tUsing a zero value radius for an ellipse or an arc would result",
  "%\t\tin a non-invertible CTM matrix which causes problem when this",
  "%\t\twhen this PostScript is wrapped inside other routines, such as",
  "%\t\tthe multi.ps package from",
  "%\t\tftp.ucc.su.oz.au:/pub/ps_printing/multi.  You can overcome such",
  "%\t\terror by uncommenting the sole line of the procedure below:",
  "%",
  "/tgif_min_radius",
  " {",
  "%    dup 0.01 lt { pop 0.01 } if",
  " } bind def",
  "",
  NULL
};

static char *psEllipseMacro[] = {
  "/tgifellipsedict 6 dict def",
  "tgifellipsedict /mtrx matrix put",
  "",
  "/TGEL % tgifellipse",
  " { tgifellipsedict begin",
  "      /yrad exch def",
  "      /xrad exch def",
  "      /y exch def",
  "      /x exch def",
  "      /savematrix mtrx currentmatrix def",
  "      x y translate",
  "      xrad yrad scale",
  "      0 0 1 0 360 arc",
  "      savematrix setmatrix",
  "   end",
  " } def",
  "",
  NULL
};

static char *psArrowMacro[] = {
  "/tgifarrowtipdict 8 dict def",
  "tgifarrowtipdict /mtrx matrix put",
  "",
  "/TGAT % tgifarrowtip",
  " { tgifarrowtipdict begin",
  "      /dy exch def",
  "      /dx exch def",
  "      /h exch def",
  "      /w exch def",
  "      /y exch def",
  "      /x exch def",
  "      /savematrix mtrx currentmatrix def",
  "      x y translate",
  "      dy dx atan rotate",
  "      0 0 moveto",
  "      w neg h lineto",
  "      w neg h neg lineto",
  "      savematrix setmatrix",
  "   end",
  " } def",
  "",
  NULL
};

static char *psArcMacro[] = {
  "/tgifarcdict 8 dict def",
  "tgifarcdict /mtrx matrix put",
  "",
  "/TGAN % tgifarcn",
  " { tgifarcdict begin",
  "      /endangle exch def",
  "      /startangle exch def",
  "      /yrad exch def",
  "      /xrad exch def",
  "      /y exch def",
  "      /x exch def",
  "      /savematrix mtrx currentmatrix def",
  "      x y translate",
  "      xrad yrad scale",
  "      0 0 1 startangle endangle arc",
  "      savematrix setmatrix",
  "   end",
  " } def",
  "",
  "/TGAR % tgifarc",
  " { tgifarcdict begin",
  "      /endangle exch def",
  "      /startangle exch def",
  "      /yrad exch def",
  "      /xrad exch def",
  "      /y exch def",
  "      /x exch def",
  "      /savematrix mtrx currentmatrix def",
  "      x y translate",
  "      xrad yrad scale",
  "      0 0 1 startangle endangle arcn",
  "      savematrix setmatrix",
  "   end",
  " } def",
  "",
  NULL
};

static char *psBWPatternMacro[] = {
  "/tgifsetuserscreendict 22 dict def",
  "tgifsetuserscreendict begin",
  "   /tempctm matrix def",
  "   /temprot matrix def",
  "   /tempscale matrix def",
  "",
  "   /concatprocs",
  "    { /proc2 exch cvlit def",
  "      /proc1 exch cvlit def",
  "      /newproc proc1 length proc2 length add array def",
  "      newproc 0 proc1 putinterval",
  "      newproc proc1 length proc2 putinterval",
  "      newproc cvx",
  "    } def",
  "   /resmatrix matrix def",
  "   /findresolution",
  "    { 72 0 resmatrix defaultmatrix dtransform",
  "      /yres exch def /xres exch def",
  "      xres dup mul yres dup mul add sqrt",
  "    } def",
  "end",
  "",
  "/tgifsetuserscreen",
  " { tgifsetuserscreendict begin",
  "      /spotfunction exch def",
  "      /screenangle exch def",
  "      /cellsize exch def",
  "",
  "      /m tempctm currentmatrix def",
  "      /rm screenangle temprot rotate def",
  "      /sm cellsize dup tempscale scale def",
  "",
  "      sm rm m m concatmatrix m concatmatrix pop",
  "",
  "      1 0 m dtransform /y1 exch def /x1 exch def",
  "",
  "      /veclength x1 dup mul y1 dup mul add sqrt def",
  "      /frequency findresolution veclength div def",
  "",
  "      /newscreenangle y1 x1 atan def",
  "",
  "      m 2 get m 1 get mul m 0 get m 3 get mul sub 0 gt",
  "",
  "      {{neg} /spotfunction load concatprocs",
  "         /spotfunction exch def",
  "      } if",
  "",
  "      frequency newscreenangle /spotfunction load setscreen",
  "   end",
  " } def",
  "",
  "/tgifsetpatterndict 18 dict def",
  "tgifsetpatterndict begin",
  "   /bitison",
  "    { /ybit exch def /xbit exch def",
  "      /bytevalue bstring ybit bwidth mul xbit 8 idiv add get def",
  "",
  "      /mask 1 7 xbit 8 mod sub bitshift def",
  "      bytevalue mask and 0 ne",
  "    } def",
  "end",
  "",
  "/tgifbitpatternspotfunction",
  " { tgifsetpatterndict begin",
  "      /y exch def /x exch def",
  "",
  "      /xindex x 1 add 2 div bpside mul cvi def",
  "      /yindex y 1 add 2 div bpside mul cvi def",
  "",
  "      xindex yindex bitison",
  "       { /onbits onbits 1 add def 1 }",
  "       { /offbits offbits 1 add def 0 }",
  "       ifelse",
  "   end",
  " } def",
  "",
  "/tgifsetpattern",
  " { tgifsetpatterndict begin",
  "      /cellsz exch def",
  "      /angle exch def",
  "      /bwidth exch def",
  "      /bpside exch def",
  "      /bstring exch def",
  "",
  "      /onbits 0 def /offbits 0 def",
  "      cellsz angle /tgifbitpatternspotfunction load tgifsetuserscreen",
  "      {} settransfer",
  "      offbits offbits onbits add div setgray",
  "   end",
  " } def",
  "",
  NULL
};

static char *psColorImageMacro[] = {
  "/tgifxpmdict 4 dict def",
  "/tgifbwpicstr 1 string def",
  "/tgifcolorpicstr 3 string def",
  "",
  "/tgifsetpixels { tgifxpmdict begin /pixels exch def end } def",
  "",
  "/tgifsetpix { tgifxpmdict begin pixels 3 1 roll putinterval end } def",
  "",
  "/tgifsetppmtruetranspix",
  " { tgifxpmdict begin",
  "      3 array /transpixels exch def",
  "      transpixels 2 3 -1 roll put",
  "      transpixels 1 3 -1 roll put",
  "      transpixels 0 3 -1 roll put",
  "      /curpix 3 array def",
  "   end",
  " } def",
  "",
  "/tgifcolorspot",
  " { tgifxpmdict begin",
  "      /index exch def",
  "      pixels index 3 mul 3 getinterval aload pop",
  "      255 mul cvi tgifcolorpicstr 2 3 -1 roll put",
  "      255 mul cvi tgifcolorpicstr 1 3 -1 roll put",
  "      255 mul cvi tgifcolorpicstr 0 3 -1 roll put",
  "      tgifcolorpicstr",
  "   end",
  " } def",
  "",
  "/tgifcolordict 4 dict def",
  "",
  "/colorimage where",
  " { pop }",
  " { /colorimage",
  "   { tgifcolordict begin",
  "        pop pop pop pop pop",
  "        /ih exch def",
  "        /iw exch def",
  "        /x 0 def",
  "        /y 0 def",
  "        1 1 ih",
  "         { pop 1 1 iw",
  "            { pop",
  "              3 { currentfile tgifbwpicstr readhexstring pop 0 get 255 div } repeat setrgbcolor",
  "              newpath x y moveto 1 0 rlineto 0 1 rlineto -1 0 rlineto",
  "              closepath fill",
  "              /x x 1 add def",
  "            } for",
  "           /y y 1 add def",
  "           /x 0 def",
  "         } for",
  "     end",
  "   } def",
  " } ifelse",
  "",
  "/tgiftranscolorspot",
  " { tgifxpmdict begin",
  "      /index exch def",
  "      pixels index 3 mul 3 getinterval aload pop",
  "      dup 0 lt { pop pop pop false } { setrgbcolor true } ifelse",
  "   end",
  " } def",
  "",
  "/tgiftranscolorimage",
  " { tgifcolordict begin",
  "      pop pop pop pop pop",
  "      /ih exch def",
  "      /iw exch def",
  "      /x 0 def",
  "      /y 0 def",
  "      1 1 ih",
  "       { pop 1 1 iw",
  "          { pop currentfile",
  "            tgifbwpicstr readhexstring pop 0 get tgiftranscolorspot",
  "            { newpath x y moveto 1 0 rlineto 0 1 rlineto -1 0 rlineto",
  "              closepath fill",
  "            }",
  "            if",
  "            /x x 1 add def",
  "          } for",
  "         /y y 1 add def",
  "         /x 0 def",
  "       } for",
  "   end",
  " } def",
  "",
  "/tgiftransppmtruecolorspot",
  " { tgifxpmdict begin",
  "      /count 0 def",
  "      curpix 2 3 -1 roll put",
  "      curpix 1 3 -1 roll put",
  "      curpix 0 3 -1 roll put",
  "      curpix 0 get transpixels 0 get ne { /count count 1 add def } if",
  "      curpix 1 get transpixels 1 get ne { /count count 1 add def } if",
  "      curpix 2 get transpixels 2 get ne { /count count 1 add def } if",
  "      count 0 eq { false } { curpix 0 get 255 div curpix 1 get 255 div curpix 2 get 255 div setrgbcolor true } ifelse",
  "   end",
  " } def",
  "",
  "/tgiftransppmtruecolorimage",
  " { tgifcolordict begin",
  "      pop pop pop pop pop",
  "      /ih exch def",
  "      /iw exch def",
  "      /x 0 def",
  "      /y 0 def",
  "      1 1 ih",
  "       { pop 1 1 iw",
  "          { pop",
  "            3 { currentfile tgifbwpicstr readhexstring pop 0 get } repeat tgiftransppmtruecolorspot",
  "            { newpath x y moveto 1 0 rlineto 0 1 rlineto -1 0 rlineto",
  "              closepath fill",
  "            }",
  "            if",
  "            /x x 1 add def",
  "          } for",
  "         /y y 1 add def",
  "         /x 0 def",
  "       } for",
  "   end",
  " } def",
  "",
  NULL
};

static char *psColorPatternMacro[] = {
  "/tgifpatdict 12 dict def",
  "",
  "/tgifpatbyte",
  " { currentdict /retstr get exch",
  "   bytesperrow mul",
  "   pat i cellh mod bytesperrow mul bytesperrow getinterval putinterval",
  " } def",
  "",
  "/tgifpatproc",
  " { 0 1 cells_wd 1 sub {tgifpatbyte} for retstr",
  "   /i i 1 add def",
  " } def",
  "",
  "/TGPF % tgifpatfill",
  " { tgifpatdict begin",
  "      /h exch def",
  "      /w exch def",
  "      /lty exch def",
  "      /ltx exch def",
  "      /cellh exch def",
  "      /cellw exch def",
  "      /pat exch def",
  "",
  "      /cells_wd w cellw div cvi def",
  "      /bytesperrow cellw 8 div ceiling cvi def",
  "      /retstr cells_wd bytesperrow mul string def",
  "      /i 0 def",
  "",
  "      tgiforigctm setmatrix",
  "      ltx lty translate",
  "      w h true [1 0 0 1 0 0] {tgifpatproc} imagemask",
  "      ltx neg lty neg translate",
  "   end",
  " } def",
  "",
  NULL
};

/*
 * No longer needed.
 * Now psDynamicPrintPatternDef() generates these from pixmap data.
 *
 * static char *psPatternMacro[] = {
 *   "/pat3 <8000000008000000> def",
 *   "/pat4 <8800000022000000> def",
 *   "/pat5 <8800220088002200> def",
 *   "/pat6 <8822882288228822> def",
 *   "/pat7 <aa55aa55aa55aa55> def",
 *   "/pat8 <77dd77dd77dd77dd> def",
 *   "/pat9 <77ffddff77ffddff> def",
 *   "/pat10 <77ffffffddffffff> def",
 *   "/pat11 <7ffffffff7ffffff> def",
 *   "/pat12 <8040200002040800> def",
 *   "/pat13 <40a00000040a0000> def",
 *   "/pat14 <ff888888ff888888> def",
 *   "/pat15 <ff808080ff080808> def",
 *   "/pat16 <f87422478f172271> def",
 *   "/pat17 <038448300c020101> def",
 *   "/pat18 <081c22c180010204> def",
 *   "/pat19 <8080413e080814e3> def",
 *   "/pat20 <8040201008040201> def",
 *   "/pat21 <8844221188442211> def",
 *   "/pat22 <77bbddee77bbddee> def",
 *   "/pat23 <c1e070381c0e0783> def",
 *   "/pat24 <7fbfdfeff7fbfdfe> def",
 *   "/pat25 <3e1f8fc7e3f1f87c> def",
 *   "/pat26 <0102040810204080> def",
 *   "/pat27 <1122448811224488> def",
 *   "/pat28 <eeddbb77eeddbb77> def",
 *   "/pat29 <83070e1c3870e0c1> def",
 *   "/pat30 <fefdfbf7efdfbf7f> def",
 *   "/pat31 <7cf8f1e3c78f1f3e> def",
 *   "",
 *   NULL
 * };
 */

void psDynamicPrintPatternDef(FP, index)
   FILE *FP;
   int index;
{
   int b=0, numbytes=0, need_to_free=FALSE;
   unsigned char *bytes=NULL;

   bytes = (unsigned char *)GetPatternBytes(index, &numbytes, &need_to_free);
   fprintf(FP, "/pat%d <", index);
   for (b=0; b < numbytes; b++) {
      fprintf(FP, "%02hhx", ByteReverse(bytes[b]));
   }
   fprintf(FP, "> def\n");
   if (need_to_free) UtilFree((char*)bytes);
}

static char *psCenterTextMacro[] = {
  "/TGCTX { dup stringwidth pop 2 div neg 0 rmoveto } def",
  "",
  NULL
};

static char *psMiniLinesMacro[] = {
  "/TGMAX",
  " { exch dup 3 1 roll exch dup 3 1 roll gt { pop } { exch pop } ifelse",
  " } def",
  "/TGMIN",
  " { exch dup 3 1 roll exch dup 3 1 roll lt { pop } { exch pop } ifelse",
  " } def",
  "/TGSW { stringwidth pop } def",
  "/tgifstrw 0 def",
  "",
  NULL
};

static char *psRightTextMacro[] = {
  "/TGRTX { dup stringwidth pop neg 0 rmoveto } def",
  "",
  NULL
};

static char *psReencodeMacro[] = {
  "/tgifreencsmalldict 12 dict def",
  "/tgifReEncodeSmall",
  " { tgifreencsmalldict begin",
  "      /newcodesandnames exch def",
  "      /newfontname exch def",
  "      /basefontname exch def",
  "",
  "      /basefontdict basefontname findfont def",
  "      /newfont basefontdict maxlength dict def",
  "",
  "      basefontdict",
  "      { exch dup /FID ne",
#ifdef GS_REENCODE_BUG
  "        1 index /UniqueID ne and",
#endif
  "         { dup /Encoding eq",
  "            { exch dup length array copy newfont 3 1 roll put }",
  "            { exch newfont 3 1 roll put }",
  "            ifelse",
  "         }",
  "         { pop pop }",
  "         ifelse",
  "      }",
  "      forall",
  "",
  "      newfont /FontName newfontname put",
  "      newcodesandnames aload pop",
  "",
  "      newcodesandnames length 2 idiv",
  "      { newfont /Encoding get 3 1 roll put}",
  "      repeat",
  "",
  "      newfontname newfont definefont pop",
  "   end",
  " } def",
  "",
  NULL
};

#ifdef _PRTGIF_USE_ISO_LATIN_1_ALWAYS
static char *psReEncodeFF8Macro[] = {
  "/TGFF8",
  " { FontDirectory 1 index known",
  "   { pop findfont exch pop }",
  "   { 1 index 4 1 roll tgifReEncodeSmall findfont } ifelse",
  " } bind def",
  "",
  NULL
};
#endif /* _PRTGIF_USE_ISO_LATIN_1_ALWAYS */

static char *psShortHandMacro[] = {
  "/bd { bind def } bind def",
  "",
  NULL
};

#ifdef _PS_USE_EUC_FONT
static char *psEucFontMacro[] = {
  "%%Title: eucfont.ps",
  "% old eucfont.ps(OCFeucfont) and new eucfont.ps(CIDeucfont) were combined.",
  "%  1996-09, kawamata@sra.co.jp",
  "% Sample Usage:",
  "%   Operands are only font names permitted. Don't put fontdict itself.",
  "% /Courier-Ryumin /Courier /Ryumin-Light-EUC-V eucfont",
  "",
  "%%Title: CIDeucfont.ps",
  "%%Creator: yohtaki@cis.ibaraki.ac.jp",
  "%%CreationDate: 1996-9-29",
  "%%EndComments",
  "%",
  "% <newfont> <asciifont> <kanjifont> CIDeucfont",
  "/CIDeucfont {",
  "dup findfont /WMode get 1 eq 4 1 roll % push direction info.",
  "12 dict begin",
  "  /CIDInit /ProcSet findresource begin",
  "  exch [ 3 1 roll ] beginrearrangedfont",
  "      3 -1 roll {   % pop direction info",
  "         /WMode 1 def	% vertical font",
#ifdef _VERT_FONT_OFFSET_POINT_4
  "         0 beginusematrix [ 0 1 -1 0 0 0.4 ] endusematrix",
#else /* ~_VERT_FONT_OFFSET_POINT_4 */
  "         0 beginusematrix [ 0 1 -1 0 0 0.3 ] endusematrix",
#endif /* _VERT_FONT_OFFSET_POINT_4 */
  "      } if",
  "      1 usefont",
  "      1 beginbfrange",
  "         <00> <7F> <00>",
  "       endbfrange",
  "     endrearrangedfont",
  "  end",
  "end",
  "} def",
  "",
  "%%Title: OCFeucfont.ps",
  "%%Creator: shikida@cs.titech.ac.jp, ueno@cs.titech.ac.jp",
  "%%CreationDate: 1991-2-27",
  "%%EndComments",
  "% <font> OCFcopyfont <font>",
  "/OCFcopyfont {",
  "    dup maxlength 1 add dict begin",
  "    {",
  "	1 index /FID ne 2 index /UniqueID ne and",
  "	{def} {pop pop} ifelse",
  "    } forall",
  "    currentdict",
  "    end",
  "} bind def",
  "% <newfont> <asciifont> <kanjifont> OCFeucfont",
  "/OCFeucfont {",
  "      12 dict begin",
  "	dup type /nametype eq { findfont } if",
  "	dup /WMode known {",
  "	    dup /WMode get /WMode exch def",
  "	    WMode 1 eq {",
#ifdef _VERT_FONT_OFFSET_POINT_4
  "		[0.0 1.0 -1.0 0.0 0.0 0.40] makefont",
#else /* ~_VERT_FONT_OFFSET_POINT_4 */
  "		[0.0 1.0 -1.0 0.0 0.0 0.30] makefont",
#endif /* _VERT_FONT_OFFSET_POINT_4 */
  "	    } if",
  "	} if",
  "	OCFcopyfont dup begin",
  "	    /Encoding Encoding",
  "	    FMapType dup 2 eq {",
  "		pop 128 128",
  "	    } { 5 eq {",
  "		256 256",
  "	    } {",
  "		/compositefont errordict /invalidfont get exec",
  "	    } ifelse",
  "	    } ifelse",
  "	    getinterval def",
  "	end",
  "	/kanjifont exch definefont",
  "	exch",
  "",
  "	dup type /nametype eq { findfont } if",
  "	exch",
  "",
  "	/FDepVector [ 4 2 roll ] def",
  "	/FontType 0 def",
  "	/FMapType 4 def",
  "	/FontMatrix matrix def",
  "	/Encoding [ 0 1 ] def",
  "	/FontBBox {0 0 0 0} def",
  "	dup /FontName exch def",
  "	currentdict",
  "    end",
  "    definefont pop",
  "} def",
  "",
  "% <newfont> <asciifont> <kanjifont> eucfont",
  "/eucfont {",
  "	dup findfont",
  "	/FMapType get 9 eq",
  "		{ CIDeucfont}",
  "		{ OCFeucfont} ifelse",
  "} def",
  "",
  NULL
};
#endif /* _PS_USE_EUC_FONT */

void DumpPSMacro(FP)
   FILE *FP;
{
   register int i;

   fprintf(FP, "\n");
   if (!psUsePSAdobe) {
      for (i=0; psAdobeMacro[i] != NULL; i++) {
         fprintf(FP, "%s\n", psAdobeMacro[i]);
      }
   }

   fprintf(FP, "/tgifdict %1d dict def\n", psDictCount);
   fprintf(FP, "tgifdict begin\n");
   fprintf(FP, "\n");

   if (psUseMinRadius) {
      for (i=0; psMinRadiusMacro[i] != NULL; i++) {
         fprintf(FP, "%s\n", psMinRadiusMacro[i]);
      }
   }
   if (psUseEllipse) {
      for (i=0; psEllipseMacro[i] != NULL; i++) {
         fprintf(FP, "%s\n", psEllipseMacro[i]);
      }
   }
   if (psUseArrow) {
      for (i=0; psArrowMacro[i] != NULL; i++) {
         fprintf(FP, "%s\n", psArrowMacro[i]);
      }
   }
   if (psUseArc) {
      for (i=0; psArcMacro[i] != NULL; i++) {
         fprintf(FP, "%s\n", psArcMacro[i]);
      }
   }
   if (psUseBWPattern) {
      for (i=0; psBWPatternMacro[i] != NULL; i++) {
         fprintf(FP, "%s\n", psBWPatternMacro[i]);
      }
   }
   if (psUseColorImage) {
      for (i=0; psColorImageMacro[i] != NULL; i++) {
         fprintf(FP, "%s\n", psColorImageMacro[i]);
      }
   }
   if (psUseColorPattern) {
      for (i=0; psColorPatternMacro[i] != NULL; i++) {
         fprintf(FP, "%s\n", psColorPatternMacro[i]);
      }
   }
   if (psUsePattern) {
      /*
       * for (i=0; psPatternMacro[i] != NULL; i++) {
       *    fprintf(FP, "%s\n", psPatternMacro[i]);
       * }
       */
      for (i=3; i < 32; i++) {
         psDynamicPrintPatternDef(FP, i);
      }
      fprintf(FP, "\n");
   }
   if (psUseCenterText) {
      for (i=0; psCenterTextMacro[i] != NULL; i++) {
         fprintf(FP, "%s\n", psCenterTextMacro[i]);
      }
   }
   if (psUseRightText) {
      for (i=0; psRightTextMacro[i] != NULL; i++) {
         fprintf(FP, "%s\n", psRightTextMacro[i]);
      }
   }
   if (psUseMiniLines) {
      for (i=0; psMiniLinesMacro[i] != NULL; i++) {
         fprintf(FP, "%s\n", psMiniLinesMacro[i]);
      }
   }
#ifdef _PRTGIF_USE_ISO_LATIN_1_ALWAYS
   if (PRTGIF && psUseReencodeFF8) {
      for (i=0; psReEncodeFF8Macro[i] != NULL; i++) {
         fprintf(FP, "%s\n", psReEncodeFF8Macro[i]);
      }
      DumpReEncodeVector(FP, "latin1", ".vec", NULL);
   }
#endif /* _PRTGIF_USE_ISO_LATIN_1_ALWAYS */
   if (psUseReencode) {
      for (i=0; psReencodeMacro[i] != NULL; i++) {
         fprintf(FP, "%s\n", psReencodeMacro[i]);
      }
   }
   if (psUseShortHand) {
      for (i=0; psShortHandMacro[i] != NULL; i++) {
         fprintf(FP, "%s\n", psShortHandMacro[i]);
      }
      for (i=0; gaszPsLongHand[i] != NULL; i++) {
         if (*gaszPsLongHand[i] == '\t') {
            fprintf(FP, "/%s { %s } def\n", &(gaszPsShortHand[i])[1],
                  &(gaszPsLongHand[i])[1]);
         } else {
            fprintf(FP, "/%s { %s } bd\n", gaszPsShortHand[i],
                  gaszPsLongHand[i]);
         }
      }
      fprintf(FP, "\n");
      gPsCmd = gaszPsShortHand;
   } else {
      gPsCmd = gaszPsLongHand;
   }
#ifdef _PS_USE_EUC_FONT
   if (psUseEucFont) {
      for (i=0; psEucFontMacro[i] != NULL; i++) {
         fprintf(FP, "%s\n", psEucFontMacro[i]);
      }
   }
#endif /* _PS_USE_EUC_FONT */
}

int ParsePsAdobeString(buf, use_ps_adobe_string, adobe_str, epsf_str)
   char *buf, *adobe_str, *epsf_str;
   int *use_ps_adobe_string;
{
   if (UtilStrICmp("false", buf) == 0 || UtilStrICmp("no", buf) == 0) {
      *use_ps_adobe_string = FALSE;
   } else if (UtilStrICmp("true", buf) == 0 || UtilStrICmp("yes", buf) == 0) {
      *use_ps_adobe_string = TRUE;
   } else if (*buf >= '0' && *buf <= '9') {
      char *c_ptr=buf, *c_ptr1=NULL;

      *use_ps_adobe_string = TRUE;
      for (c_ptr1=c_ptr; *c_ptr1 != '\0'; c_ptr1++) {
         if (*c_ptr1 == '/') {
            *c_ptr1 = ' ';
            break;
         }
      }
      sscanf(c_ptr, "%s", adobe_str);
      c_ptr = &c_ptr[strlen(adobe_str)];
      switch (*c_ptr) {
      case '\0': break;
      case ' ':
         c_ptr++;
         if (*c_ptr >= '0' && *c_ptr <= '9') {
            sscanf(c_ptr, "%s", epsf_str);
         } else {
            return FALSE;
         }
         break;
      default: return FALSE;
      }
   } else {
      return FALSE;
   }
   return TRUE;
}

void CleanUpPS()
{
}

void InitPS()
{
}

