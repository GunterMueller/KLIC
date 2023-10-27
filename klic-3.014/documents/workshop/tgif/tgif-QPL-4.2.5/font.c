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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/font.c,v 1.42 2011/05/16 16:21:57 william Exp $
 */

#define _INCLUDE_FROM_FONT_C_

#include "tgifdefs.h"
#include "cmdids.h"

#include "auxtext.e"
#include "choice.e"
#include "cmd.e"
#include "color.e"
#include "cursor.e"
#include "dialog.e"
#include "drawing.e"
#include "exec.e"
#include "file.e"
#include "font.e"
#include "ini.e"
#include "list.e"
#include "mainmenu.e"
#include "mark.e"
#include "menu.e"
#include "menuinfo.e"
#include "miniline.e"
#include "msg.e"
#include "obj.e"
#include "pattern.e"
#include "page.e"
#include "ps.e"
#include "raster.e"
#include "rect.e"
#include "select.e"
#include "setup.e"
#include "strtbl.e"
#include "text.e"
#include "util.e"
#include "version.e"
#include "xpixmap.e"

#define DEFAULT_FONT_SIZE 14

struct FontSizeRec {
   /*
    * For a font (given face and style), the xfs for various font sizes
    *      are stored in a link list.
    * If the font is not scalable (faked==TRUE), the head of the link list
    *      is used to scale fonts.
    * The vert field is TRUE for double-byte vertical fonts.
    */
   XFontStruct		*xfs;
   int			sz_unit, faked, vert;
   struct FontSizeRec	*next;
};

struct FontFmlyRec {
   /* fr[i] points to the default size used for the given font */
   /*	this font is used if the requested size is not found */
   struct FontSizeRec	fr[MAXFONTSTYLES];
   char			*name_faked;
   int			bitmapped_ps_font;
   int			double_byte;
   char			choice_char[3];
   char			*conv_from_utf8_cmd, *conv_to_utf8_cmd;
};

typedef struct tagPSCharSubs {
   char *token_name;
   char *ps_char_name[128];
} PSCharSubs;

XFontStruct	*canvasFontPtr=NULL;
int	canvasFontHeight=0;
int	canvasFontAsc=0;
int	canvasFontDes=0;
int	canvasFontDirection=0;
int	canvasFontDontReencode=FALSE;
int	canvasFontDoubleByte=FALSE;
int	canvasFontDoubleByteModBytes=FALSE;
int	canvasFontDoubleByteVertical=FALSE;
int	canvasFontSize=0;
int	canvasFontIsFaked=FALSE;
int	canvasLineAsc=0;
int	canvasLineDes=0;

XFontStruct	*rulerFontPtr=NULL;
int	rulerFontWidth=0;
int	rulerFontHeight=0;
int	rulerFontAsc=0;
int	rulerFontDes=0;
static char *rulerFontName=NULL;

XFontStruct	*defaultFontPtr=NULL;
int	defaultFontWidth=0;
int	defaultFontHeight=0;
int	defaultFontAsc=0;
int	defaultFontDes=0;
static char *defaultFontName=NULL;

XFontStruct	*menuFontPtr=NULL;
int	menuFontWidth=0;
int	menuFontHeight=0;
int	menuFontAsc=0;
int	menuFontDes=0;
static char *menuFontName=NULL;

#ifdef ENABLE_NLS
XFontSet	menuFontSet=NULL;
XFontSet	msgFontSet=NULL;
XFontSet	boldMsgFontSet=NULL;
XFontSet	italicMsgFontSet=NULL;
XFontSet	boldItalicMsgFontSet=NULL;
#else /* ~ENABLE_NLS */
char     	*menuFontSet=NULL;
char     	*msgFontSet=NULL;
char     	*boldMsgFontSet=NULL;
char     	*italicMsgFontSet=NULL;
char     	*boldItalicMsgFontSet=NULL;
#endif /* ENABLE_NLS */
int	boldMsgFontDoubleByte=FALSE;

static char *menuFontSetName=NULL;
static char *msgFontSetName=NULL;
static char *boldMsgFontSetName=NULL;
static char *italicMsgFontSetName=NULL;
static char *boldItalicMsgFontSetName=NULL;

XFontStruct	*boldMsgFontPtr=NULL;
int	boldMsgFontWidth=0;
int	boldMsgFontHeight=0;
int	boldMsgFontAsc=0;
int	boldMsgFontDes=0;
static char *boldMsgFontName=NULL;

XFontStruct	*italicMsgFontPtr=NULL;
int	italicMsgFontWidth=0;
int	italicMsgFontHeight=0;
int	italicMsgFontAsc=0;
int	italicMsgFontDes=0;
static char *italicMsgFontName=NULL;

XFontStruct	*boldItalicMsgFontPtr=NULL;
int	boldItalicMsgFontWidth=0;
int	boldItalicMsgFontHeight=0;
int	boldItalicMsgFontAsc=0;
int	boldItalicMsgFontDes=0;
static char *boldItalicMsgFontName=NULL;

XFontStruct	*msgFontPtr=NULL;
int	msgFontWidth=0;
int	msgFontHeight=0;
int	msgFontAsc=0;
int	msgFontDes=0;
static char *msgFontName=NULL;

int	curFont=FONT_COU;
int	initialFont=FONT_COU;
int	curSzUnit=(20*NUM_SZ_UNIT_PER_FONT_SIZE), curRealSzUnit=INVALID;
int	curStyle=STYLE_NR;
int	textRotation=0; /* degrees*64 */
int	curUnderlineOn=FALSE;
int	curOverlineOn=FALSE;

int	actualCurFont=(-1);
int	actualCurSzUnit=(-1);
int	actualCurStyle=(-1);

int	curUnderlineYOffset=2;
int	curOverlineYOffset=(-1);

int	changePropertiesOfAttrs=FALSE;

static int	defaultCurFont=(-1);
static int	defaultCurSzUnit=(-1);
static int	defaultCurStyle=(-1);

static char	*initSizeMenuStr[] = {
      "8", "10", "11", "12", "14", "17", "18", "20", "24", "25", "34"
};
char	**fontMenuStr=NULL;
char	*defFontMenuStr[] = {
        "Times", "Courier", "Helvetica", "NewCenturySchlbk", "Symbol"
};
char	**sizeMenuStr=NULL;
int	*fontSzUnits=NULL;
int	numFonts=MAXFONTS;
int	numFontSizes=MAXFONTS;

int	changingFontSizeFromRead=TRUE;
int	allowFontFaceSubstitution=TRUE;

char	cmdLineDontReencode[MAXSTRING<<1];

int	defaultFontSize=DEFAULT_FONT_SIZE;

int	showFontSizeInPoints=FALSE;

int	substitutedFont=INVALID;
int	substitutedFontSzUnit=0;

int	defaultSingleByteFont=FONT_TIM;

int		gnNumPSFontAliases=0;
KeyValInfo	*gaPSFontAliases=NULL;

int		gnNumPSFontNeedCharSubs=0;
KeyValInfo	*gaPSFontNeedCharSubs=NULL;

CVList		psCharSubsList;

static int	numFakedFonts=0;
static char	fontNamePrefix[MAXSTRING+1];
static char	defaultLatin1FontCharEncoding[MAXSTRING+1];
static int	hasAlternateDefaultFonts=FALSE;
static int	reportMissingFonts=FALSE;

static struct FontFmlyRec	*fontFamilies=NULL;

static char	*initFontInfoStr[]={
   /* do not translate -- program constants */
   "times-medium-r-normal", "iso8859-1", "Times-Roman",
   "times-bold-r-normal", "iso8859-1", "Times-Bold",
   "times-medium-i-normal", "iso8859-1", "Times-Italic",
   "times-bold-i-normal", "iso8859-1", "Times-BoldItalic",
   "courier-medium-r-normal", "iso8859-1", "Courier",
   "courier-bold-r-normal", "iso8859-1", "Courier-Bold",
   "courier-medium-o-normal", "iso8859-1", "Courier-Oblique",
   "courier-bold-o-normal", "iso8859-1", "Courier-BoldOblique",
   "helvetica-medium-r-normal", "iso8859-1", "Helvetica",
   "helvetica-bold-r-normal", "iso8859-1", "Helvetica-Bold",
   "helvetica-medium-o-normal", "iso8859-1", "Helvetica-Oblique",
   "helvetica-bold-o-normal", "iso8859-1", "Helvetica-BoldOblique",
   "new century schoolbook-medium-r-normal", "iso8859-1",
         "NewCenturySchlbk-Roman",
   "new century schoolbook-bold-r-normal", "iso8859-1",
         "NewCenturySchlbk-Bold",
   "new century schoolbook-medium-i-normal", "iso8859-1",
         "NewCenturySchlbk-Italic",
   "new century schoolbook-bold-i-normal", "iso8859-1",
         "NewCenturySchlbk-BoldItalic",
   "*symbol*-medium-r-normal", "adobe-fontspecific", "Symbol",
   "*symbol*-medium-r-normal", "adobe-fontspecific", "Symbol",
   "*symbol*-medium-r-normal", "adobe-fontspecific", "Symbol",
   "*symbol*-medium-r-normal", "adobe-fontspecific", "Symbol",
   NULL, NULL, NULL
};

static char	**altFontInfoStr=NULL;

static char	**fontInfoStr=NULL;

static char	*charCodeToName[] = {
           /* do not translate -- program constants */
/* \200 */ "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
/* \220 */ "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
/* \240 */ "x",
           "8#241 /exclamdown",
           "8#242 /cent",
           "8#243 /sterling",
           "8#244 /currency",
           "8#245 /yen",
           "8#246 /bar",
           "8#247 /section",
           "8#250 /dieresis",
           "8#251 /copyright",
           "8#252 /ordfeminine",
           "8#253 /guillemotleft",
           "8#254 /logicalnot",
           "8#255 /emdash",
           "8#256 /registered",
           "8#257 /macron",
/* \260 */ "8#260 /degree",
           "8#261 /plusminus",
           "8#262 /twosuperior",
           "8#263 /threesuperior",
           "8#264 /acute",
           "8#265 /mu",
           "8#266 /paragraph",
           "8#267 /periodcentered",
           "8#270 /cedilla",
           "8#271 /onesuperior",
           "8#272 /ordmasculine",
           "8#273 /guillemotright",
           "8#274 /onequarter",
           "8#275 /onehalf",
           "8#276 /threequarters",
           "8#277 /questiondown",
/* \300 */ "8#300 /Agrave",
           "8#301 /Aacute",
           "8#302 /Acircumflex",
           "8#303 /Atilde",
           "8#304 /Adieresis",
           "8#305 /Aring",
           "8#306 /AE",
           "8#307 /Ccedilla",
           "8#310 /Egrave",
           "8#311 /Eacute",
           "8#312 /Ecircumflex",
           "8#313 /Edieresis",
           "8#314 /Igrave",
           "8#315 /Iacute",
           "8#316 /Icircumflex",
           "8#317 /Idieresis",
/* \320 */ "8#320 /Eth",
           "8#321 /Ntilde",
           "8#322 /Ograve",
           "8#323 /Oacute",
           "8#324 /Ocircumflex",
           "8#325 /Otilde",
           "8#326 /Odieresis",
           "8#327 /multiply",
           "8#330 /Oslash",
           "8#331 /Ugrave",
           "8#332 /Uacute",
           "8#333 /Ucircumflex",
           "8#334 /Udieresis",
           "8#335 /Yacute",
           "8#336 /Thorn",
           "8#337 /germandbls",
/* \340 */ "8#340 /agrave",
           "8#341 /aacute",
           "8#342 /acircumflex",
           "8#343 /atilde",
           "8#344 /adieresis",
           "8#345 /aring",
           "8#346 /ae",
           "8#347 /ccedilla",
           "8#350 /egrave",
           "8#351 /eacute",
           "8#352 /ecircumflex",
           "8#353 /edieresis",
           "8#354 /igrave",
           "8#355 /iacute",
           "8#356 /icircumflex",
           "8#357 /idieresis",
/* \360 */ "8#360 /eth",
           "8#361 /ntilde",
           "8#362 /ograve",
           "8#363 /oacute",
           "8#364 /ocircumflex",
           "8#365 /otilde",
           "8#366 /odieresis",
           "8#367 /divide",
           "8#370 /oslash",
           "8#371 /ugrave",
           "8#372 /uacute",
           "8#373 /ucircumflex",
           "8#374 /udieresis",
           "8#375 /yacute",
           "8#376 /thorn",
           "8#377 /ydieresis"
};

#define InfoIndex(FontIdx,StyleIdx) (((FontIdx)*MAXFONTSTYLES+(StyleIdx))*3)
#define InfoIndexToFontIndex(InfoIdx) ((int)(((int)(InfoIdx/3))/MAXFONTSTYLES))

static int	debugScalableFonts=FALSE; /* debug scalable font */
static char	gszAttemptedFontName[MAXSTRING+1];

static struct PushedFontRec gstSavedFontInfo;

int IsFontDoubleByte(font_index)
   int font_index;
{
   return fontFamilies[font_index].double_byte;
}

int IsDoubleByteFontVertical(font_index, style)
   int font_index, style;
{
   return fontFamilies[font_index].fr[style].vert;
}

int CanConvertUTF8ToFont(font_index)
   int font_index;
{
   return (fontFamilies[font_index].double_byte &&
         fontFamilies[font_index].conv_from_utf8_cmd != NULL);
}

int CanConvertFontToUTF8(font_index)
   int font_index;
{
   return (fontFamilies[font_index].double_byte &&
         fontFamilies[font_index].conv_to_utf8_cmd != NULL);
}

int ConvertToOrFromUTF8(which, font_index, ppsz_cut_buffer)
   int which, font_index;
   char **ppsz_cut_buffer;
{
   int bytes_read=0, len=strlen(*ppsz_cut_buffer), total_bytes=0;
   char *cmd=NULL, buf[1024], tmp_fname[MAXPATHLENGTH], *c_ptr=NULL;
   FILE *fp=NULL, *pfp=NULL;
   CVList list;
   CVListElem *pElem=NULL;

   if (MkTempFile(tmp_fname, sizeof(tmp_fname), tmpDir, TOOL_NAME) == NULL) {
      return FALSE;
   }
   if ((fp=fopen(tmp_fname, "w")) == NULL) {
      return FailToOpenMessage(tmp_fname, "w", NULL);
   }
   if ((int)fwrite(*ppsz_cut_buffer, sizeof(char), len, fp) <= 0) {
      fclose(fp);
      FailToWriteFileMessage(tmp_fname);
      unlink(tmp_fname);
      return FALSE;
   }
   fclose(fp);

   if (which == CONVERT_TO_UTF8) {
      if (strstr(fontFamilies[font_index].conv_to_utf8_cmd, "%s") == NULL) {
         snprintf(gszMsgBox, sizeof(gszMsgBox), "%s %s",
               fontFamilies[font_index].conv_to_utf8_cmd, tmp_fname);
      } else {
         snprintf(gszMsgBox, sizeof(gszMsgBox),
               fontFamilies[font_index].conv_to_utf8_cmd, tmp_fname);
      }
   } else {
      if (strstr(fontFamilies[font_index].conv_from_utf8_cmd, "%s") == NULL) {
         snprintf(gszMsgBox, sizeof(gszMsgBox), "%s %s",
               fontFamilies[font_index].conv_from_utf8_cmd, tmp_fname);
      } else {
         snprintf(gszMsgBox, sizeof(gszMsgBox),
               fontFamilies[font_index].conv_from_utf8_cmd, tmp_fname);
      }
   }
   cmd = UtilStrDup(gszMsgBox);
   if (cmd == NULL) FailAllocMessage();

   sprintf(gszMsgBox, TgLoadCachedString(CSTID_EXECUTING_GIVEN_PROGRAM), cmd);
   Msg(gszMsgBox);
   SetStringStatus(gszMsgBox);
   XSync(mainDisplay, False);

   if ((pfp=(FILE*)popen(cmd, "r")) == NULL) {
      sprintf(gszMsgBox, TgLoadString(STID_FAIL_TO_EXECUTE_CMD), cmd);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      unlink(tmp_fname);
      UtilFree(cmd);
      return FALSE;
   }
   CVListInit(&list);
   *buf = '\0';
   while ((bytes_read=fread(buf, sizeof(char), sizeof(buf), pfp)) > 0) {
      struct DynStrRec *dyn_str=NewDynStr();

      total_bytes += bytes_read;
      dyn_str->s = (char*)malloc(bytes_read+1);
      if (dyn_str->s == NULL) FailAllocMessage();
      memcpy(dyn_str->s, buf, bytes_read);
      dyn_str->sz = bytes_read+1;
      ListAppend(&list, dyn_str);
   }
   pclose(pfp);
   unlink(tmp_fname);
   UtilFree(cmd);

   UtilFree(*ppsz_cut_buffer);
   *ppsz_cut_buffer = (char*)malloc(total_bytes+1);
   if (*ppsz_cut_buffer == NULL) FailAllocMessage();

   c_ptr = (*ppsz_cut_buffer);
   for (pElem=ListFirst(&list); pElem != NULL; pElem=ListNext(&list, pElem)) {
      struct DynStrRec *dyn_str=(struct DynStrRec *)(pElem->obj);

      memcpy(c_ptr, dyn_str->s, dyn_str->sz-1);
      c_ptr += dyn_str->sz-1;
      FreeDynStrBuf(dyn_str);
   }
   ListUnlinkAll(&list);

   *c_ptr = '\0';

   return TRUE;
}

int GetInitialFont()
{
   return initialFont;
}

static
int DoesFontExist(font_name, double_byte)
   char *font_name;
   int double_byte;
{
   char *buf=NULL, **font_list=NULL;
   int actual_count_return=FALSE;

   if (font_name == NULL) return FALSE;

   UtilTrimBlanks(font_name);
   buf = (char*)malloc((strlen(font_name)<<1) + 1);
   if (buf == NULL) FailAllocMessage();
   memset(buf, 0, (strlen(font_name)<<1) + 1);

   /* replace "%d" to "*" */
   if (strchr(font_name, '%') == NULL) {
      sprintf(buf, "*%s*", font_name);
   } else {
      char *dest=NULL, *src=NULL;

      dest = buf;
      if (!double_byte) {
         *dest++ = '*';
      }
      for (src=font_name; *src != '\0'; src++) {
         char ch2=src[1];

         if (*src=='%' && ((ch2>='a' && ch2<='z') || (ch2>='A' && ch2<='Z'))) {
            src++;
            *dest++ = '*';
         } else {
            *dest++ = (*src);
         }
      }
      if (!double_byte) {
         *dest++ = '*';
      }
      *dest = '\0';
   }
   font_list = XListFonts(mainDisplay, buf, 1, &actual_count_return);
   if (font_list) {
      XFreeFontNames(font_list);
   }
   free(buf);

   return (actual_count_return > 0);
}

int SzUnitToFontSize(sz_unit)
   int sz_unit;
{
   double d=(double)(((double)sz_unit)/((double)NUM_SZ_UNIT_PER_FONT_SIZE));

   return ((int)(round(d)));
}

int SzUnitToPointSize(sz_unit)
   int sz_unit;
{
   double d=(double)(((double)sz_unit)/((double)NUM_SZ_UNIT_PER_POINT_SIZE));

   return ((int)(round(d)));
}

int BadDoubleByteString(pszStr, pszZero)
   char *pszStr, *pszZero;
{
   fprintf(stderr, "%s\n", TgLoadString(STID_ILLEGAL_DOUBLE_BYTE_TRUNC));
   SaveDoubleByteString(stderr, pszStr);
   fprintf(stderr, "\n");
   *(--pszZero) = '\0';
   return FALSE;
}

char *GetPrevChar(nDoubleByte, pszStr, pszCur)
   int nDoubleByte;
   char *pszStr, *pszCur;
{
   if (pszStr == pszCur) return NULL;

   if (nDoubleByte) {
      char *psz=NULL, *pszLast=NULL;

      for (psz=pszStr; *psz != '\0'; psz++) {
         int double_byte=FALSE;

         if (psz == pszCur) return pszLast;

         pszLast = psz;
         double_byte = ((*psz)&0x80);
         if (double_byte) {
            if (*(++psz) == '\0') {
               BadDoubleByteString(pszStr, psz);
               return NULL;
            }
         }
      }
      return pszLast;
   } else {
      return (--pszCur);
   }
}

char *GetShowFontChar(FontIndex)
   int FontIndex;
{
   if (*fontFamilies[FontIndex].choice_char == '\0') {
      if (canvasFontDoubleByte) {
         int min_byte1=canvasFontPtr->min_byte1;
         int max_byte1=canvasFontPtr->max_byte1;
         int min_byte2=canvasFontPtr->min_char_or_byte2;
         int max_byte2=canvasFontPtr->max_char_or_byte2;

         fontFamilies[FontIndex].choice_char[0] = ((min_byte1+max_byte1)>>1);
         fontFamilies[FontIndex].choice_char[1] = ((min_byte2+max_byte2)>>1);
         fontFamilies[FontIndex].choice_char[2] = '\0';
         return fontFamilies[FontIndex].choice_char;
      }
      return NULL;
   }
   return fontFamilies[FontIndex].choice_char;
}

static
void RecordDoubleByteFontInfo(xfs, pszSection)
   XFontStruct *xfs;
   char *pszSection;
{
#ifdef _TGIF_DBG /* debug, do not translate */
   char szIniFile[MAXPATHLENGTH];

   sprintf(szIniFile, "%s%cfontinfo.ini", tgifDir, DIR_SEP);
   tgWriteProfileString(pszSection, NULL, NULL, szIniFile);

   sprintf(gszMsgBox, "\\%03o", (int)(xfs->min_byte1));
   tgWriteProfileString(pszSection, "min_byte1", gszMsgBox, szIniFile);
   sprintf(gszMsgBox, "\\%03o", (int)(xfs->max_byte1));
   tgWriteProfileString(pszSection, "max_byte1", gszMsgBox, szIniFile);
   sprintf(gszMsgBox, "\\%03o", (int)(xfs->min_char_or_byte2));
   tgWriteProfileString(pszSection, "min_byte2", gszMsgBox, szIniFile);
   sprintf(gszMsgBox, "\\%03o", (int)(xfs->max_char_or_byte2));
   tgWriteProfileString(pszSection, "max_byte2", gszMsgBox, szIniFile);
   sprintf(gszMsgBox, "%1d", (int)(xfs->ascent));
   tgWriteProfileString(pszSection, "ascent", gszMsgBox, szIniFile);
   sprintf(gszMsgBox, "%1d", (int)(xfs->descent));
   tgWriteProfileString(pszSection, "descent", gszMsgBox, szIniFile);

   tgWriteProfileString(NULL, NULL, NULL, szIniFile);
#endif /* _TGIF_DBG */
}

#define X_FONT_PREFIX 0
#define X_FONT_SUFFIX 1
#define PS_FONT_NAME  2

void GetCurFontInfoStr(buf, buf_sz)
   char* buf;
   int buf_sz;
{
   int font_index=curFont;
   int style_index=curStyle;
   int size=SzUnitToFontSize(curSzUnit);
   int info_index=InfoIndex(font_index, style_index);

   if (strstr(fontInfoStr[info_index], "%d") != NULL) {
      sprintf(gszAttemptedFontName, fontInfoStr[info_index], size);
   } else {
      sprintf(gszAttemptedFontName, "%s-%s-*-%1d-*-*-*-*-*-%s", fontNamePrefix,
            fontInfoStr[info_index+X_FONT_PREFIX], size,
            fontInfoStr[info_index+X_FONT_SUFFIX]);
   }
   UtilStrCpyN(buf, buf_sz, gszAttemptedFontName);
}

XFontStruct *LoadAFontByName(psz_font_name, double_byte, can_debug)
   char *psz_font_name;
   int double_byte, can_debug;
{
   XFontStruct *xfs=NULL;

   sprintf(gszMsgBox, TgLoadCachedString(CSTID_LOADING_FILE),
         psz_font_name);
   SetStringStatus(gszMsgBox);
   xfs = XLoadQueryFont(mainDisplay, psz_font_name);
   if (xfs != NULL && double_byte && can_debug &&
         (xfs->min_byte1 != 0 || xfs->max_byte1 != 0)) {
      RecordDoubleByteFontInfo(xfs, psz_font_name);
      if ((xfs->min_byte1 & 0x80) != (xfs->max_byte1 & 0x80)) {
         sprintf(gszMsgBox, TgLoadString(STID_TOOL_CANT_HANDLE_DB_FONT),
               psz_font_name, (int)(xfs->min_byte1),
               (int)(xfs->max_byte1), TOOL_NAME, authorEmailString);
         fprintf(stderr, "%s\n", gszMsgBox);
         MsgBox(gszMsgBox, TOOL_NAME, STOP_MB);
      }
   }
   return xfs;
}

static
void LoadAFont(info_index, size, can_debug, pxfs, pn_vert)
   int info_index, size, can_debug, *pn_vert;
   XFontStruct **pxfs;
{
   int watch_cursor=watchCursorOnMainWindow;
   int font_index=InfoIndexToFontIndex(info_index);
   int supposedly_double_byte=fontFamilies[font_index].double_byte;
   XFontStruct *xfs=NULL;

   *pxfs = NULL;
   *pn_vert = FALSE;

   if (!watch_cursor && mainWindow != None && !RedrawDuringScrolling()) {
      SetWatchCursor(drawWindow);
      SetWatchCursor(mainWindow);
   }
   if (strstr(fontInfoStr[info_index], "%d") != NULL) {
      sprintf(gszAttemptedFontName, fontInfoStr[info_index], size);
      if (UtilStrICmp(fontInfoStr[info_index+X_FONT_SUFFIX], "V") == 0) {
         *pn_vert = TRUE;
      }
   } else {
      sprintf(gszAttemptedFontName, "%s-%s-*-%1d-*-*-*-*-*-%s", fontNamePrefix,
            fontInfoStr[info_index+X_FONT_PREFIX], size,
            fontInfoStr[info_index+X_FONT_SUFFIX]);
   }
   xfs = LoadAFontByName(gszAttemptedFontName, supposedly_double_byte,
         can_debug);
   if (xfs != NULL && can_debug && debugScalableFonts) {
      int i;

      for (i=0; i < (MAXFONTSIZES<<1)-1; i++) {
         if (atoi(initSizeMenuStr[i]) == size) {
            break;
         }
      }
      if (i >= (MAXFONTSIZES<<1)-1) {
         XFreeFont(mainDisplay, xfs);
         xfs = NULL;
      }
   }
   if (!watch_cursor && mainWindow != None && !RedrawDuringScrolling()) {
      SetDefaultCursor(mainWindow);
      ShowCursor();
   }
   *pxfs = xfs;
}

static
struct FontSizeRec *FindFontInfo(font_index, style_index, sz_unit, must_find)
   int font_index, style_index, sz_unit, must_find;
{
   struct FontSizeRec *fs_ptr, *prev_fs=NULL, *new_fs_ptr;
   int info_index, watch_cursor=watchCursorOnMainWindow, vert=FALSE;
   XFontStruct *xfs=NULL;

   canvasFontIsFaked = FALSE;
   *gszAttemptedFontName = '\0';
   for (fs_ptr=fontFamilies[font_index].fr[style_index].next; fs_ptr != NULL;
         fs_ptr=fs_ptr->next) {
      if (fs_ptr->sz_unit == sz_unit) {
         canvasFontIsFaked = fs_ptr->faked;
         return (canvasFontIsFaked ?
               &fontFamilies[font_index].fr[style_index] : fs_ptr);
      }
      if (fs_ptr->sz_unit > sz_unit) break;
      prev_fs = fs_ptr;
   }
   info_index = InfoIndex(font_index, style_index);

   SaveStatusStrings();
   LoadAFont(info_index, SzUnitToFontSize(sz_unit), TRUE, &xfs, &vert);

   if (xfs == NULL) {
      if (must_find) return NULL;

      canvasFontIsFaked = TRUE;
      xfs = fontFamilies[font_index].fr[style_index].xfs;
      if (xfs == NULL) {
         LoadAFont(info_index, defaultFontSize, FALSE, &xfs, &vert);

         fontFamilies[font_index].fr[style_index].xfs = xfs;
         fontFamilies[font_index].fr[style_index].sz_unit =
               FontSizeToSzUnit(defaultFontSize);
         if (xfs != NULL && defaultCurFont == (-1) &&
               !IsFontDoubleByte(font_index)) {
            defaultCurFont = font_index;
            defaultCurStyle = style_index;
            defaultCurSzUnit = FontSizeToSzUnit(defaultFontSize);
         }
         if (xfs == NULL) {
            if (allowFontFaceSubstitution) {
               /*
                * Even for a double-byte font, substitute Courier font anyway!
                */
               int default_info_index=(-1);
               char failed_font_name[MAXSTRING];

               if (defaultCurFont == (-1)) {
                  defaultCurFont = FONT_COU;
                  defaultCurStyle = STYLE_NR;
                  defaultCurSzUnit = FontSizeToSzUnit(14);
               }
               default_info_index = InfoIndex(defaultCurFont,defaultCurStyle);
               LoadAFont(default_info_index, defaultFontSize, FALSE, &xfs,
                     &vert);

               fontFamilies[font_index].fr[style_index].xfs = xfs;
               fontFamilies[font_index].fr[style_index].sz_unit =
                     defaultCurSzUnit;
               if (xfs == NULL) {
                  if (!watch_cursor && mainWindow!=None) {
                     SetDefaultCursor(mainWindow);
                     ShowCursor();
                  }
                  RestoreStatusStrings();
                  return NULL;
               }
               GetCurFontInfoStr(failed_font_name, sizeof(failed_font_name));
               sprintf(gszMsgBox, TgLoadString(STID_FAIL_LOAD_FONT_FONT_SUBS),
                     gszAttemptedFontName,
                     fontInfoStr[default_info_index+PS_FONT_NAME],
                     SzUnitToFontSize(defaultCurSzUnit),
                     fontInfoStr[info_index+PS_FONT_NAME],
                     SzUnitToFontSize(sz_unit));
               MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);

               actualCurFont = defaultCurFont;
               actualCurStyle = defaultCurStyle;
               actualCurSzUnit = defaultCurSzUnit;
            }
         } else if (changingFontSizeFromRead) {
            sprintf(gszMsgBox, TgLoadString(STID_FONT_SUBSTITUTED),
                  fontInfoStr[info_index+PS_FONT_NAME], defaultFontSize,
                  fontInfoStr[info_index+PS_FONT_NAME],
                  SzUnitToFontSize(sz_unit));
            Msg(gszMsgBox);
         }
      }
   } else if (defaultCurFont == (-1) && !IsFontDoubleByte(font_index)) {
      defaultCurFont = font_index;
      defaultCurStyle = style_index;
      defaultCurSzUnit = sz_unit;
   }
   if (!watch_cursor && mainWindow!=None) {
      SetDefaultCursor(mainWindow);
      ShowCursor();
   }
   RestoreStatusStrings();
   if (xfs == NULL) return NULL;

   new_fs_ptr = (struct FontSizeRec *)malloc(sizeof(struct FontSizeRec));
   if (new_fs_ptr == NULL) FailAllocMessage();
   memset(new_fs_ptr, 0, sizeof(struct FontSizeRec));
   new_fs_ptr->next = fs_ptr;
   new_fs_ptr->faked = canvasFontIsFaked;
   new_fs_ptr->xfs = xfs;
   new_fs_ptr->sz_unit = sz_unit;
   new_fs_ptr->vert = vert;
   if (prev_fs == NULL) {
      fontFamilies[font_index].fr[style_index].next = new_fs_ptr;
   } else {
      prev_fs->next = new_fs_ptr;
   }
   return (canvasFontIsFaked ? &fontFamilies[font_index].fr[style_index]:
         new_fs_ptr);
}

void GetPSFontStr(FontIndex, StyleIndex, FontStr)
   int FontIndex, StyleIndex;
   char *FontStr;
{
   if (PRTGIF && !cmdLineOpenDisplay) {
      if (FontIndex < MAXFONTS) {
         sprintf(FontStr, "/%s",
               initFontInfoStr[InfoIndex(FontIndex,StyleIndex)+PS_FONT_NAME]);
      } else {
         sprintf(FontStr, "/%s", fontFamilies[FontIndex].name_faked);
      }
   } else {
      if (FontIndex < numFonts) {
         sprintf(FontStr, "/%s",
               fontInfoStr[InfoIndex(FontIndex,StyleIndex)+PS_FONT_NAME]);
      } else {
         sprintf(FontStr, "/%s", fontFamilies[FontIndex].name_faked);
      }
   }
}

XFontStruct *LoadASymbolFont(font_size)
   int font_size;
{
   int info_index=InfoIndex(FONT_SYM, STYLE_NR), vert=FALSE;
   XFontStruct *xfs=NULL;

   SaveStatusStrings();
   LoadAFont(info_index, font_size, TRUE, &xfs, &vert);
   RestoreStatusStrings();

   return xfs;
}

static
char *LookupPSFontAliases(buf)
   char *buf;
{
   int i=0;

   for (i=0; i < gnNumPSFontAliases; i++) {
      KeyValInfo *pKeyValue=(&gaPSFontAliases[i]);

      if (pKeyValue == NULL) return FALSE;

      if (pKeyValue != NULL && pKeyValue->key != NULL &&
            strcmp(pKeyValue->key, buf) == 0) {
         return pKeyValue->value;
      }
   }
   return NULL;
}

void MapAliasedPSFontName(buf, buf_sz)
   char *buf;
   int buf_sz;
{
   char *real_ps_font_name=NULL;

   if ((real_ps_font_name=LookupPSFontAliases(&buf[1])) != NULL) {
      UtilStrCpyN(&buf[1], buf_sz-1, real_ps_font_name);
   }
}

void SetTextExtentsInfo(pTextExtents)
   TextExtentsInfo *pTextExtents;
{
   if (pTextExtents->buf == NULL || pTextExtents->len == 0) {
      pTextExtents->w = pTextExtents->lbearing = pTextExtents->rextra = 0;
      pTextExtents->bbox_w = 0;
   } else {
      XCharStruct xcs;

      MyTextExtents(canvasFontPtr, pTextExtents->buf, pTextExtents->len, &xcs);
      pTextExtents->w = xcs.width;
      pTextExtents->lbearing = (xcs.lbearing >= 0 ? 0 : xcs.lbearing);
      pTextExtents->rextra = xcs.rbearing - xcs.width;
      pTextExtents->bbox_w = pTextExtents->w - pTextExtents->lbearing +
            pTextExtents->rextra;
   }
   pTextExtents->bbox_h = canvasFontHeight;
   pTextExtents->asc = canvasFontAsc;
   pTextExtents->des = canvasFontDes;
}

struct StrRec *SegmentDoubleByteString(pszStr)
   char *pszStr;
{
   struct StrRec *pFirstStr=NULL, *pLastStr=NULL;
   int nLen=strlen(pszStr), nIndex=0;

   if (nLen == 0) {
      return NewStr();
   }
   while (nIndex < nLen) {
      int nSubStrIndex=0, nDoubleByte;
      char *psz=NULL, *pszStart=NULL, saved_ch='\0';
      struct StrRec *pStr=NULL;

      pszStart = &pszStr[nIndex];
      nDoubleByte = ((*pszStart) & 0x80);
      if (nDoubleByte) {
         int nOdd=FALSE;

         for (psz=pszStart, nSubStrIndex=0;
               nIndex+nSubStrIndex < nLen &&
               (nOdd || ((*psz)&0x80) == nDoubleByte);
               psz++, nSubStrIndex++) {
            nOdd = !nOdd;
         }
      } else {
         for (psz=pszStart, nSubStrIndex=0;
               nIndex+nSubStrIndex < nLen && ((*psz)&0x80) == 0;
               psz++, nSubStrIndex++) {
         }
      }
      pStr = NewStr();
      saved_ch = (*psz);
      *psz = '\0';
      DynStrSet(&pStr->dyn_str, pszStart);
      *psz = saved_ch;

      pStr->prev = pLastStr;
      pStr->next = NULL;
      if (pFirstStr == NULL) {
         pFirstStr = pStr;
      } else {
         pLastStr->next = pStr;
      }
      pLastStr = pStr;

      nIndex += nSubStrIndex;
   }
   return pFirstStr;
}

static char *gpszTmpStr=NULL;
static XChar2b *gpszTmpStr16=NULL;
static int gnTmpStrLen=0;

static
void AllocTmp16Strings(pszStr, nLen)
   char *pszStr;
   int nLen;
{
   if (nLen > gnTmpStrLen) {
      char *psz=(char*)malloc(nLen+3);
      XChar2b *psz16=(XChar2b*)malloc((nLen+3)*sizeof(XChar2b));

      if (psz == NULL || psz16 == NULL) FailAllocMessage();
      if (gpszTmpStr != NULL) free(gpszTmpStr);
      if (gpszTmpStr16 != NULL) free(gpszTmpStr16);
      gpszTmpStr = psz;
      gpszTmpStr16 = psz16;
      gnTmpStrLen = nLen;
   }
   if (gpszTmpStr != NULL) *gpszTmpStr = '\0';
   if (gpszTmpStr16 != NULL) {
      gpszTmpStr16[0].byte1 = gpszTmpStr16[0].byte2 = (unsigned char)0;
   }
}

void MyTextExtents(xfs, pszStr, nLen, pxcs)
   XFontStruct *xfs;
   char *pszStr;
   int nLen;
   XCharStruct *pxcs;
   /* nLen does not include trailing '\0' */
{
   int dir=0, asc=0, des=0;

   memset(pxcs, 0, sizeof(XCharStruct));
   if (canvasFontDoubleByte) {
      int nDoubleByte, nIndex=0;
      XCharStruct xcs;

      memset(&xcs, 0, sizeof(XCharStruct));
      AllocTmp16Strings(pszStr, nLen);
      while (nIndex < nLen) {
         int nSubStrIndex=0;
         char *psz=NULL, *pszStart=NULL;

         pszStart = &pszStr[nIndex];
         nDoubleByte = ((*pszStart) & 0x80);
         if (nDoubleByte) {
            int nDoubleByteIndex=0, nOdd=FALSE;

            for (psz=pszStart, nSubStrIndex=0;
                  nIndex+nSubStrIndex < nLen &&
                  (nOdd || ((*psz)&0x80) == nDoubleByte);
                  psz++, nSubStrIndex++) {
               if (nOdd) {
                  gpszTmpStr16[nDoubleByteIndex].byte2 = (unsigned char)(*psz);
                  nDoubleByteIndex++;
               } else {
                  gpszTmpStr16[nDoubleByteIndex].byte1 = (unsigned char)(*psz);
               }
               nOdd = !nOdd;
            }
            if (canvasFontDoubleByteModBytes) {
               int i;

               for (i=0; i < nDoubleByteIndex; i++) {
                  gpszTmpStr16[i].byte1 &= 0x7f;
                  gpszTmpStr16[i].byte2 &= 0x7f;
               }
            }
            XTextExtents16(xfs, gpszTmpStr16, nDoubleByteIndex, &dir, &asc,
                  &des, &xcs);
         } else {
            for (psz=pszStart, nSubStrIndex=0;
                  nIndex+nSubStrIndex < nLen && ((*psz)&0x80) == 0;
                  psz++, nSubStrIndex++) {
            }
            XTextExtents(xfs, pszStart, nSubStrIndex, &dir, &asc,
                  &des, &xcs);
         }
         if (pszStart == pszStr) {
            pxcs->lbearing = xcs.lbearing;
         }
         pxcs->width += xcs.width;
         pxcs->rbearing = xcs.rbearing-xcs.width;

         nIndex += nSubStrIndex;
      }
      pxcs->rbearing += pxcs->width;
   } else {
      XTextExtents(xfs, pszStr, nLen, &dir, &asc, &des, pxcs);
   }
}

int MyTextWidth(xfs, pszStr, nLen)
   XFontStruct *xfs;
   char *pszStr;
   int nLen;
   /* nLen does not include trailing '\0' */
{
   if (canvasFontDoubleByte) {
      int nDoubleByte, nIndex=0, nTotal=0;

      AllocTmp16Strings(pszStr, nLen);
      while (nIndex < nLen) {
         int nSubStrIndex=0;
         char *psz=NULL, *pszStart=NULL;

         pszStart = &pszStr[nIndex];
         nDoubleByte = ((*pszStart) & 0x80);
         if (nDoubleByte) {
            int nDoubleByteIndex=0, nOdd=FALSE;

            for (psz=pszStart, nSubStrIndex=0;
                  nIndex+nSubStrIndex < nLen &&
                  (nOdd || ((*psz)&0x80) == nDoubleByte);
                  psz++, nSubStrIndex++) {
               if (nOdd) {
                  gpszTmpStr16[nDoubleByteIndex].byte2 = (unsigned char)(*psz);
                  nDoubleByteIndex++;
               } else {
                  gpszTmpStr16[nDoubleByteIndex].byte1 = (unsigned char)(*psz);
               }
               nOdd = !nOdd;
            }
            nTotal += XTextWidth16(xfs, gpszTmpStr16, nDoubleByteIndex);
         } else {
            for (psz=pszStart, nSubStrIndex=0;
                  nIndex+nSubStrIndex < nLen && ((*psz)&0x80) == 0;
                  psz++, nSubStrIndex++) {
            }
            nTotal += XTextWidth(xfs, pszStart, nSubStrIndex);
         }
         nIndex += nSubStrIndex;
      }
      return nTotal;
   } else {
      return XTextWidth(xfs, pszStr, nLen);
   }
}

void MyDrawString(dpy, d, gc, nDepth, x, y, pszStr, nLen)
   Display *dpy;
   Drawable d;
   GC gc;
   int nDepth, x, y, nLen;
   char *pszStr;
   /* nLen does not include trailing '\0' */
{
   if (canvasFontDoubleByte) {
      int nDoubleByte, nIndex=0, nTotal=0;

      AllocTmp16Strings(pszStr, nLen);
      while (nIndex < nLen) {
         int nSubStrIndex=0;
         char *psz=NULL, *pszStart=NULL;

         pszStart = &pszStr[nIndex];
         nDoubleByte = ((*pszStart) & 0x80);
         if (nDoubleByte) {
            int nDoubleByteIndex=0, nOdd=FALSE, w=0, i=0;

            for (psz=pszStart, nSubStrIndex=0;
                  nIndex+nSubStrIndex < nLen &&
                  (nOdd || ((*psz)&0x80) == nDoubleByte);
                  psz++, nSubStrIndex++) {
               if (nOdd) {
                  gpszTmpStr16[nDoubleByteIndex].byte2 = (unsigned char)(*psz);
                  nDoubleByteIndex++;
               } else {
                  gpszTmpStr16[nDoubleByteIndex].byte1 = (unsigned char)(*psz);
               }
               nOdd = !nOdd;
            }
            if (canvasFontDoubleByteModBytes) {
               for (i=0; i < nDoubleByteIndex; i++) {
                  gpszTmpStr16[i].byte1 &= 0x7f;
                  gpszTmpStr16[i].byte2 &= 0x7f;
               }
            }
            if (canvasFontDoubleByteVertical) {
               for (i=0; i < nDoubleByteIndex; i++) {
                  char buf[3];
                  Pixmap bitmap=None;
                  TextExtentsInfo stTextExtents;

                  w = XTextWidth16(canvasFontPtr, &gpszTmpStr16[i], 1);
                  buf[0] = pszStart[i<<1];
                  buf[1] = pszStart[(i<<1)+1];
                  buf[2] = '\0';
                  memset(&stTextExtents, 0, sizeof(TextExtentsInfo));
                  stTextExtents.buf = buf;
                  stTextExtents.len = 2;
                  bitmap = PaintVertChar(&stTextExtents, ROTATE270);
                  if (bitmap != None) {
                     XGCValues values;

                     values.clip_mask = bitmap;
                     values.clip_x_origin = x+nTotal;
                     values.clip_y_origin = y-canvasFontAsc;
                     XChangeGC(dpy, gc,
                           GCClipMask | GCClipXOrigin | GCClipYOrigin, &values);

                     XFillRectangle(dpy, d, gc, x+nTotal, y-canvasFontAsc,
                           stTextExtents.bbox_h, stTextExtents.bbox_w);

                     values.clip_mask = None;
                     values.clip_x_origin = 0;
                     values.clip_y_origin = 0;
                     XChangeGC(dpy, gc,
                           GCClipMask | GCClipXOrigin | GCClipYOrigin, &values);
                  }
                  nTotal += w;
               }
            } else {
               XDrawString16(dpy, d, gc, x+nTotal, y, gpszTmpStr16,
                     nDoubleByteIndex);
               w = XTextWidth16(canvasFontPtr, gpszTmpStr16, nDoubleByteIndex);
               nTotal += w;
            }
         } else {
            for (psz=pszStart, nSubStrIndex=0;
                  nIndex+nSubStrIndex < nLen && ((*psz)&0x80) == 0;
                  psz++, nSubStrIndex++) {
            }
            XDrawString(dpy, d, gc, x+nTotal, y, pszStart, nSubStrIndex);
            nTotal += XTextWidth(canvasFontPtr, pszStart, nSubStrIndex);
         }
         nIndex += nSubStrIndex;
      }
   } else {
      XDrawString(dpy, d, gc, x, y, pszStr, nLen);
   }
}

static Pixmap	vertFontBitmap=None, rotatedVertFontBitmap=None;
static XImage	*rotatedVertFontImage=NULL;
static int	vertFontBitmapW=0, vertFontBitmapH=0;

Pixmap PaintVertChar(pTextExtents, nRotate)
   TextExtentsInfo *pTextExtents;
   int nRotate;
   /* Caller must not delete the returned pixmap! */
{
   int w=0, lbearing=0, rextra=0, image_w=0, image_h=0;
   int r=0, dest_cols=0, dest_rows=0;
   int saved_db_vert=canvasFontDoubleByteVertical;
   XImage *src_image=NULL;
   XGCValues values;

   SetTextExtentsInfo(pTextExtents);
   w = pTextExtents->w;
   lbearing = pTextExtents->lbearing;
   rextra = pTextExtents->rextra;
   image_w = pTextExtents->bbox_w;
   image_h = pTextExtents->bbox_h;

   if (image_w > vertFontBitmapW || image_h > vertFontBitmapH) {
      int tmp_w, tmp_h;

      if (vertFontBitmap != None) XFreePixmap(mainDisplay, vertFontBitmap);
      if (rotatedVertFontBitmap != None) {
         XFreePixmap(mainDisplay, rotatedVertFontBitmap);
      }
      tmp_w = max(image_w, vertFontBitmapW);
      tmp_h = max(image_h, vertFontBitmapH);
      vertFontBitmap = XCreatePixmap(mainDisplay, mainWindow, tmp_w, tmp_h, 1);
      rotatedVertFontBitmap = XCreatePixmap(mainDisplay, mainWindow, tmp_h,
            tmp_w, 1);
      if (vertFontBitmap == None) {
         sprintf(gszMsgBox, TgLoadString(STID_CANNOT_ALLOC_PIXMAP_OF_SIZE),
               tmp_w, tmp_h);
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
         return FALSE;
      } else if (rotatedVertFontBitmap == None) {
         sprintf(gszMsgBox, TgLoadString(STID_CANNOT_ALLOC_PIXMAP_OF_SIZE),
               tmp_h, tmp_w);
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
         return FALSE;
      }
      vertFontBitmapW = tmp_w;
      vertFontBitmapH = tmp_h;
   }
   dest_cols = image_h;
   dest_rows = image_w;

   XSetForeground(mainDisplay, rotateGC, 0);
   XFillRectangle(mainDisplay, vertFontBitmap, rotateGC, 0, 0,
         vertFontBitmapW, vertFontBitmapH);
   XFillRectangle(mainDisplay, rotatedVertFontBitmap, rotateGC, 0, 0,
         image_h, image_w);

   values.foreground = 1;
   values.font = canvasFontPtr->fid;
   XChangeGC(mainDisplay, rotateGC, GCForeground | GCFont, &values);
   canvasFontDoubleByteVertical = FALSE;
   MyDrawString(mainDisplay, vertFontBitmap, rotateGC, 1, -lbearing,
         canvasFontAsc, pTextExtents->buf, pTextExtents->len);
   canvasFontDoubleByteVertical = saved_db_vert;
   src_image = XGetImage(mainDisplay, vertFontBitmap, 0, 0, image_w, image_h, 1,
         ZPixmap);
   if (src_image == NULL) {
      sprintf(gszMsgBox, TgLoadString(STID_CANNOT_GET_IMAGE_OF_SIZE),
            image_w, image_h);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return FALSE;
   }
   if (rotatedVertFontImage != NULL) XDestroyImage(rotatedVertFontImage);
   rotatedVertFontImage = XGetImage(mainDisplay, rotatedVertFontBitmap, 0, 0,
         dest_cols, dest_rows, 1, ZPixmap);
   if (rotatedVertFontImage == NULL) {
      XDestroyImage(src_image);
      sprintf(gszMsgBox, TgLoadString(STID_CANNOT_GET_IMAGE_OF_SIZE),
            dest_cols, dest_rows);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return FALSE;
   }
   switch (nRotate) {
   case ROTATE90:
      for (r=0; r < dest_rows; r++) {
         int c=0;

         for (c=0; c < dest_cols; c++) {
            int x=r;
            int y=image_h-c-1;

            if (XGetPixel(src_image, x, y)==1) {
               XPutPixel(rotatedVertFontImage, c, r, 1);
            }
         }
      }
      break;
   case ROTATE270:
      for (r=0; r < dest_rows; r++) {
         int c=0;

         for (c=0; c < dest_cols; c++) {
            int x=image_w-r-1;
            int y=c;

            if (XGetPixel(src_image, x, y)==1) {
               XPutPixel(rotatedVertFontImage, c, r, 1);
            }
         }
      }
      break;
   }
   XPutImage(mainDisplay, rotatedVertFontBitmap, rotateGC,
         rotatedVertFontImage, 0, 0, 0, 0, dest_cols, dest_rows);
   XDestroyImage(src_image);

   return rotatedVertFontBitmap;
}

static
void CleanUpTmpStrings()
{
   if (gpszTmpStr != NULL) free(gpszTmpStr);
   if (gpszTmpStr16 != NULL) free(gpszTmpStr16);
   gpszTmpStr = NULL;
   gpszTmpStr16 = NULL;
   gnTmpStrLen = 0;
}

int ValidCharCode(c_ptr)
   char *c_ptr;
{
   int index=(int)(*c_ptr - '\200');

   if (*charCodeToName[index] == '\0' || *charCodeToName[index] == '8' ||
         *charCodeToName[index] == '\\') {
      return TRUE;
   }
   sprintf(gszMsgBox, TgLoadString(STID_UNRECOG_CHARCODE_DISCARD),
         (*c_ptr)&0xff);
   Msg(gszMsgBox);
   return FALSE;
}

char *CharCodeTranslate(c_ptr)
   char *c_ptr;
{
   int index=(int)(*c_ptr - '\200');

   if (*charCodeToName[index] == '\\' || *charCodeToName[index] == '8') {
      return charCodeToName[index];
   }
   return NULL;
}

static int	*encodeFont=NULL;
static short	**encodeCharFlags=NULL;
static int	encodeCharFlagsAllocated=FALSE;
static int	numEncodeCharFonts=0;

static int	gnDontReencodeInitialized=FALSE;
static char	*gpszDontReencode=NULL;

static
void CleanUpEncodeCharFonts()
{
   int i;

   if (encodeCharFlagsAllocated) {
      for (i = 0; i < numEncodeCharFonts*MAXFONTSTYLES; i++) {
         free(encodeCharFlags[i]);
      }
      free(encodeFont);
      free(encodeCharFlags);
      encodeCharFlagsAllocated = FALSE;
      numEncodeCharFonts = 0;
      encodeFont = NULL;
      encodeCharFlags = NULL;
   }
   *cmdLineDontReencode = '\0';
}

static
void RemovePSFontNameVariations(ps_font_name_str)
   char *ps_font_name_str;
   /*
    * Get rid of the '-' as in 'ZapfChancery-MediumItalic'.
    * Also get rid of the '--' as in 'Helvetica-Narrow--Bold' and
    *       'Helvetica-Narrow--'.
    */
{
   char *psz=NULL;

   if ((psz=strstr(ps_font_name_str, "--")) != NULL) {
      *psz = '\0';
   } else if ((psz=strchr(ps_font_name_str, '-')) != NULL) {
      *psz = '\0';
   }
}

void SaveDontReencode(FP)
   FILE *FP;
{
   char *c_ptr=NULL;

   if (gpszDontReencode == NULL) return;

   if (fprintf(FP, "dont_reencode(\"") == EOF) {
      writeFileFailed = TRUE;
   }
   for (c_ptr=gpszDontReencode; *c_ptr != '\0'; c_ptr++) {
      if (fprintf(FP, "%s", c_ptr) == EOF) writeFileFailed = TRUE;
      c_ptr += strlen(c_ptr);
      if (c_ptr[1] != '\0') {
         if (fprintf(FP, ":") == EOF) writeFileFailed = TRUE;
      }
   }
   if (fprintf(FP, "\").\n") == EOF) writeFileFailed = TRUE;
}

int ReadDontReencode(buf)
   char *buf;
{
   char *s=NULL, *c_ptr=NULL;

   if (!PRTGIF || cmdLineOpenDisplay) return TRUE;

#ifndef _PRTGIF_USE_ISO_LATIN_1_ALWAYS
   s = FindChar((int)'(', buf);
   c_ptr = FindChar((int)'"', s);
   s = ReadString(c_ptr);
   *(--s) = '\0';
   InitDontReencode(c_ptr);
#endif /* ~_PRTGIF_USE_ISO_LATIN_1_ALWAYS */

   return TRUE;
}

void SavePSFontAliases(FP)
   FILE *FP;
{
   int i=0, count=0, total=0;

   if (gnNumPSFontAliases == 0 || gaPSFontAliases == NULL) return;

   for (i=0; i < gnNumPSFontAliases; i++) {
      KeyValInfo *pKeyValue=(&gaPSFontAliases[i]);

      if (pKeyValue == NULL || pKeyValue->key == NULL ||
            pKeyValue->value == NULL) {
         continue;
      }
      count++;
   }
   if (count == 0) return;

   if (fprintf(FP, "ps_font_aliases([\n") == EOF) {
      writeFileFailed = TRUE;
   }
   for (i=0; i < gnNumPSFontAliases; i++) {
      KeyValInfo *pKeyValue=(&gaPSFontAliases[i]);

      if (pKeyValue == NULL || pKeyValue->key == NULL ||
            pKeyValue->value == NULL) {
         continue;
      }
      total++;
      if (fprintf(FP, "\t\"%s=%s\"", pKeyValue->key, pKeyValue->value) == EOF) {
         writeFileFailed = TRUE;
      }
      if (total != count) {
         if (fprintf(FP, ",\n") == EOF) writeFileFailed = TRUE;
      }
   }
   /*
    * For the next file version, the '\t' below should be removed.
    */
   if (fprintf(FP, "\n\t]).\n") == EOF) writeFileFailed = TRUE;
}

void SaveDoubleByteModBytes(FP)
   FILE *FP;
   /* This function doesn't do anything.  May be there is no need for this. */
{
   int i=0;

   for (i=0; i < numFonts; i++) {
      if (IsFontDoubleByte(i)) {
      }
   }
}

void SavePSFontNeedCharSubs(FP)
   FILE *FP;
{
   int i=0, count=0, total=0;

   if (gnNumPSFontNeedCharSubs == 0 || gaPSFontNeedCharSubs == NULL) return;

   for (i=0; i < gnNumPSFontNeedCharSubs; i++) {
      KeyValInfo *pKeyValue=(&gaPSFontNeedCharSubs[i]);

      if (pKeyValue == NULL || pKeyValue->key == NULL ||
            pKeyValue->value == NULL) {
         continue;
      }
      count++;
   }
   if (count == 0) return;

   if (fprintf(FP, "ps_font_need_char_subs([\n") == EOF) {
      writeFileFailed = TRUE;
   }
   for (i=0; i < gnNumPSFontNeedCharSubs; i++) {
      KeyValInfo *pKeyValue=(&gaPSFontNeedCharSubs[i]);

      if (pKeyValue == NULL || pKeyValue->key == NULL ||
            pKeyValue->value == NULL) {
         continue;
      }
      total++;
      if (fprintf(FP, "\t\"%s=%s\"", pKeyValue->key, pKeyValue->value) == EOF) {
         writeFileFailed = TRUE;
      }
      if (total != count) {
         if (fprintf(FP, ",\n") == EOF) writeFileFailed = TRUE;
      }
   }
   /*
    * For the next file version, the '\t' below should be removed.
    */
   if (fprintf(FP, "\n\t]).\n") == EOF) writeFileFailed = TRUE;
}

void SavePSCharSubs(FP)
   FILE *FP;
{
   CVListElem *elem=NULL;

   for (elem=ListFirst(&psCharSubsList); elem != NULL;
         elem=ListNext(&psCharSubsList, elem)) {
      PSCharSubs *ppscs=(PSCharSubs*)elem->obj;

      if (ppscs != NULL && ppscs->token_name != NULL) {
         int i=0, first_one=TRUE;

         if (fprintf(FP, "ps_char_subs(\"%s\",[\n", ppscs->token_name) == EOF) {
            writeFileFailed = TRUE;
         }
         for (i=0; i < 128; i++) {
            char *ps_char_name=ppscs->ps_char_name[i];

            if (ps_char_name != NULL) {
               if (first_one) {
                  if (fprintf(FP, "\t\"8#%03o%s\"",
                        (i|0x080), ps_char_name) == EOF) {
                     writeFileFailed = TRUE;
                  }
                  first_one = FALSE;
               } else {
                  if (fprintf(FP, ",\n\t\"8#%03o%s\"",
                        (i|0x080), ps_char_name) == EOF) {
                     writeFileFailed = TRUE;
                  }
               }
            }
         }
         if (fprintf(FP, "\n\t]).\n") == EOF) writeFileFailed = TRUE;
      }
   }
}

static
void ProcessAPSFontAlias(fake_font_name, real_font_name)
   char *fake_font_name, *real_font_name;
{
   if (gaPSFontAliases == NULL) {
      gaPSFontAliases = (KeyValInfo*)malloc(sizeof(KeyValInfo));
   } else {
      gaPSFontAliases = (KeyValInfo*)realloc(gaPSFontAliases,
            (gnNumPSFontAliases+1)*sizeof(KeyValInfo));
   }
   if (gaPSFontAliases == NULL) FailAllocMessage();
   memset(&gaPSFontAliases[gnNumPSFontAliases], 0, sizeof(KeyValInfo));

   gaPSFontAliases[gnNumPSFontAliases].key =
         UtilStrDup(fake_font_name);
   gaPSFontAliases[gnNumPSFontAliases].value =
         UtilStrDup(&real_font_name[1]);
   if (gaPSFontAliases[gnNumPSFontAliases].key == NULL ||
         gaPSFontAliases[gnNumPSFontAliases].value == NULL) {
      FailAllocMessage();
   }
   *real_font_name = '=';

   UtilTrimBlanks(gaPSFontAliases[gnNumPSFontAliases].key);
   UtilTrimBlanks(gaPSFontAliases[gnNumPSFontAliases].value);
   gnNumPSFontAliases++;
}

int ReadPSFontAliases(FP, buf)
   FILE *FP;
   char *buf;
{
   char *line=NULL;
   int skip_processing=FALSE;

   if (!PRTGIF || cmdLineOpenDisplay) skip_processing = TRUE;
   if (gaPSFontAliases != NULL || gnNumPSFontAliases > 0) {
      skip_processing = TRUE;
   }
   while ((line=UtilGetALine(FP)) != NULL) {
      char *c_ptr=NULL, *fake_font_name=NULL, *real_font_name=NULL;
      char info[MAXSTRING];

      scanLineNum++;
      if (*line == ']' || (line[0] == '\t' && line[1] == ']')) {
         free(line);
         break;
      }
      if (!skip_processing) {
#ifndef _PRTGIF_USE_ISO_LATIN_1_ALWAYS
         c_ptr = FindChar((int)'"', line);
         c_ptr = ParseStr(c_ptr, (int)'"', info, sizeof(info));
         fake_font_name = info;
         real_font_name = strchr(fake_font_name, '=');
         if (real_font_name == NULL || real_font_name[1] == '\0') {
            free(line);
            continue;
         }
         *real_font_name = '\0';
         ProcessAPSFontAlias(fake_font_name, real_font_name);
         *real_font_name = '=';
#endif /* ~_PRTGIF_USE_ISO_LATIN_1_ALWAYS */
      }
      free(line);
   }
   return TRUE;
}

static
void FreePSCharSubs(pppscs)
   PSCharSubs **pppscs;
{
   PSCharSubs *ppscs=(*pppscs);

   if (ppscs != NULL) {
      int i=0;

      UtilFree(ppscs->token_name);
      for (i=0; i < 128; i++) {
         if (ppscs->ps_char_name[i] != NULL) {
            free(ppscs->ps_char_name[i]);
         }
      }
      free(ppscs);
   }
   *pppscs = NULL;
}

static
int ValidCharCodeSpec(psz_char_code, token_name, pn_char_code, resource_name)
   char *psz_char_code, *token_name, *resource_name;
   int *pn_char_code;
{
   int char_code=0;

   if (strncmp(psz_char_code, "8#", 2) == 0) {
      if (sscanf(&psz_char_code[2], "%o", &char_code) != 1) {
         if (resource_name == NULL) {
            sprintf(gszMsgBox, TgLoadString(STID_FAIL_TO_PARSE_CHAR_CODE_FILE),
                  psz_char_code, token_name);
         } else {
            sprintf(gszMsgBox, TgLoadString(STID_FAIL_TO_PARSE_CHAR_CODE_XDEF),
                  psz_char_code, token_name, TOOL_NAME, resource_name);
         }
         fprintf(stderr, "%s\n", gszMsgBox);
         return FALSE;
      }
   } else if (strncmp(psz_char_code, "0x", 2) == 0) {
      if (sscanf(&psz_char_code[2], "%x", &char_code) != 1) {
         if (resource_name == NULL) {
            sprintf(gszMsgBox, TgLoadString(STID_FAIL_TO_PARSE_CHAR_CODE_FILE),
                  psz_char_code, token_name);
         } else {
            sprintf(gszMsgBox, TgLoadString(STID_FAIL_TO_PARSE_CHAR_CODE_XDEF),
                  psz_char_code, token_name, TOOL_NAME, resource_name);
         }
         fprintf(stderr, "%s\n", gszMsgBox);
         return FALSE;
      }
   } else {
      if (sscanf(psz_char_code, "%d", &char_code) != 1) {
         if (resource_name == NULL) {
            sprintf(gszMsgBox, TgLoadString(STID_FAIL_TO_PARSE_CHAR_CODE_FILE),
                  psz_char_code, token_name);
         } else {
            sprintf(gszMsgBox, TgLoadString(STID_FAIL_TO_PARSE_CHAR_CODE_XDEF),
                  psz_char_code, token_name, TOOL_NAME, resource_name);
         }
         fprintf(stderr, "%s\n", gszMsgBox);
         return FALSE;
      }
   }
   if (char_code < 161 || char_code > 255) {
      if (resource_name == NULL) {
         sprintf(gszMsgBox, TgLoadString(STID_CHAR_CODE_OUT_OF_RANGE_FILE),
               psz_char_code, token_name);
      } else {
         sprintf(gszMsgBox, TgLoadString(STID_CHAR_CODE_OUT_OF_RANGE_XDEF),
               psz_char_code, token_name, TOOL_NAME, resource_name);
      }
      fprintf(stderr, "%s\n", gszMsgBox);
      return FALSE;
   }
   *pn_char_code = char_code;

   return TRUE;
}

static
void ProcessCharSubs(ppscs, psz_char_code, ps_char_name, resource_name)
   PSCharSubs *ppscs;
   char *psz_char_code, *ps_char_name, *resource_name;
{
   int char_code=0, len=strlen(ps_char_name)+1;

   if (!ValidCharCodeSpec(psz_char_code, ppscs->token_name, &char_code,
         resource_name)) {
      return;
   }
   if (ppscs->ps_char_name[char_code-128] != NULL) {
      if (resource_name == NULL) {
         sprintf(gszMsgBox, TgLoadString(STID_DUP_CHAR_CODE_FILE_IGNORED),
               char_code, char_code, TOOL_NAME, resource_name);
      } else {
         sprintf(gszMsgBox, TgLoadString(STID_DUP_CHAR_CODE_XDEF_IGNORED),
               char_code, char_code, TOOL_NAME, resource_name);
      }
      fprintf(stderr, "%s\n", gszMsgBox);
      return;
   }
   ppscs->ps_char_name[char_code-128] = (char*)malloc(len+1);
   if (ppscs->ps_char_name[char_code-128] == NULL) FailAllocMessage();
   sprintf(ppscs->ps_char_name[char_code-128], "/%s", ps_char_name);
}

static
int ParsePSCharSubs(ppscs, c_ptr, resource_name)
   PSCharSubs *ppscs;
   char *c_ptr, *resource_name;
{
   char *buf=UtilStrDup(c_ptr), *buf1=NULL, *psz_state=NULL;

   if (buf == NULL) FailAllocMessage();
   for (buf1=UtilStrTok(buf, " ,\t\n\r", &psz_state); buf1 != NULL;
         buf1=UtilStrTok(NULL, " ,\t\n\r", &psz_state)) {
      char *ps_char_name=strchr(buf1, '/');

      if (ps_char_name == NULL) {
         fprintf(stderr, TgLoadString(STID_BAD_XDEF_CANT_FIND_TOKEN_SKIP), buf1,
               "'='", TOOL_NAME, resource_name);
         fprintf(stderr, "\n");
         continue;
      }
      if (ps_char_name[1] == '\0') {
         fprintf(stderr, TgLoadString(STID_BAD_XDEF_MISS_CHAR_NAME_SKIP),
               buf1, TOOL_NAME, resource_name);
         fprintf(stderr, "\n");
         continue;
      }
      *ps_char_name = '\0';
      ProcessCharSubs(ppscs, buf1, &ps_char_name[1], resource_name);
      *ps_char_name = '/';
   }
   UtilFree(buf);

   return TRUE;
}

static
char *FindPSFontNeedCharSubs(ps_font_name)
   char *ps_font_name;
{
   int i=0;

   for (i=0; i < gnNumPSFontNeedCharSubs; i++) {
      if (strcmp(ps_font_name, gaPSFontNeedCharSubs[i].key) == 0) {
         return gaPSFontNeedCharSubs[i].value;
      }
   }
   return NULL;
}

static
PSCharSubs *FindTokenInPSCharSubs(token_name)
   char *token_name;
{
   CVListElem *elem=NULL;

   for (elem=ListFirst(&psCharSubsList); elem != NULL;
         elem=ListNext(&psCharSubsList, elem)) {
      PSCharSubs *ppscs=(PSCharSubs*)elem->obj;

      if (ppscs != NULL && ppscs->token_name != NULL &&
            strcmp(ppscs->token_name, token_name) == 0) {
         return ppscs;
      }
   }
   return NULL;
}

static
PSCharSubs *FindPSCharSubs(ps_font_name)
   char *ps_font_name;
{
   char *token_name=FindPSFontNeedCharSubs(ps_font_name);

   if (token_name == NULL) return NULL;

   return FindTokenInPSCharSubs(token_name);
}

static
void ProcessPSCharSubs(font_name, token_name)
   char *font_name, *token_name;
{
   if (gaPSFontNeedCharSubs == NULL) {
      gaPSFontNeedCharSubs = (KeyValInfo*)malloc(sizeof(KeyValInfo));
   } else {
      gaPSFontNeedCharSubs = (KeyValInfo*)realloc(gaPSFontNeedCharSubs,
            (gnNumPSFontNeedCharSubs+1)*sizeof(KeyValInfo));
   }
   if (gaPSFontNeedCharSubs == NULL) FailAllocMessage();
   memset(&gaPSFontNeedCharSubs[gnNumPSFontNeedCharSubs], 0,
         sizeof(KeyValInfo));

   gaPSFontNeedCharSubs[gnNumPSFontNeedCharSubs].key =
         UtilStrDup(font_name);
   gaPSFontNeedCharSubs[gnNumPSFontNeedCharSubs].value =
         UtilStrDup(token_name);
   if (gaPSFontNeedCharSubs[gnNumPSFontNeedCharSubs].key == NULL ||
         gaPSFontNeedCharSubs[gnNumPSFontNeedCharSubs].value == NULL) {
      FailAllocMessage();
   }
   UtilTrimBlanks(gaPSFontNeedCharSubs[gnNumPSFontNeedCharSubs].key);
   UtilTrimBlanks(gaPSFontNeedCharSubs[gnNumPSFontNeedCharSubs].value);
   gnNumPSFontNeedCharSubs++;

   if (FindTokenInPSCharSubs(token_name) == NULL) {
      if (mainDisplay != NULL) {
         char *c_ptr=NULL, buf[MAXSTRING];

         sprintf(buf, "PSCharSubs_%s", token_name);
         if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,buf)) == NULL) {
            sprintf(gszMsgBox, TgLoadString(STID_CANNOT_GET_X_DEFAULT),
                  TOOL_NAME, buf);
            fprintf(stderr, "%s\n", gszMsgBox);
         } else {
            PSCharSubs *ppscs=NULL;

            ppscs = (PSCharSubs*)malloc(sizeof(PSCharSubs));
            if (ppscs == NULL) FailAllocMessage();
            memset(ppscs, 0, sizeof(PSCharSubs));
            ppscs->token_name = UtilStrDup(token_name);
            if (ppscs->token_name == NULL) FailAllocMessage();
            if (ParsePSCharSubs(ppscs, c_ptr, buf)) {
               ListAppend(&psCharSubsList, ppscs);
            } else {
               FreePSCharSubs(&ppscs);
            }
         }
      } else {
         sprintf(gszMsgBox, TgLoadString(STID_CANT_FIND_PS_CHAR_SUBS_READ),
               token_name);
         fprintf(stderr, "%s\n", gszMsgBox);
      }
   }
}

static
void CleanUpPSFontCharSubs()
{
   if (gnNumPSFontNeedCharSubs > 0) {
      CVListElem *elem=NULL;

      for (elem=ListFirst(&psCharSubsList); elem != NULL;
            elem=ListNext(&psCharSubsList, elem)) {
         PSCharSubs *ppscs=(PSCharSubs*)elem->obj;

         if (ppscs != NULL) {
            FreePSCharSubs(&ppscs);
         }
      }
      ListUnlinkAll(&psCharSubsList);

      while (gnNumPSFontNeedCharSubs-- > 0) {
         UtilFree(gaPSFontNeedCharSubs[gnNumPSFontNeedCharSubs].key);
         UtilFree(gaPSFontNeedCharSubs[gnNumPSFontNeedCharSubs].value);
      }
      free(gaPSFontNeedCharSubs);
   }
   gaPSFontNeedCharSubs = NULL;
   gnNumPSFontNeedCharSubs = 0;
}

int ReadPSFontNeedCharSubs(FP, buf)
   FILE *FP;
   char *buf;
{
   char *line=NULL;
   int skip_processing=FALSE;

   if (!PRTGIF || cmdLineOpenDisplay) skip_processing = TRUE;
   if (gaPSFontNeedCharSubs != NULL || gnNumPSFontNeedCharSubs > 0) {
      skip_processing = TRUE;
   }
   while ((line=UtilGetALine(FP)) != NULL) {
      char *c_ptr=NULL, *font_name=NULL, *token_name=NULL;
      char info[MAXSTRING];

      scanLineNum++;
      if (*line == ']' || (line[0] == '\t' && line[1] == ']')) {
         free(line);
         break;
      }
      if (!skip_processing) {
#ifndef _PRTGIF_USE_ISO_LATIN_1_ALWAYS
         c_ptr = FindChar((int)'"', line);
         c_ptr = ParseStr(c_ptr, (int)'"', info, sizeof(info));
         font_name = info;
         token_name = strchr(font_name, '=');
         if (token_name == NULL || token_name[1] == '\0') {
            free(line);
            continue;
         }
         *token_name = '\0';
         ProcessPSCharSubs(font_name, &token_name[1]);
         *token_name = '=';
#endif /* ~_PRTGIF_USE_ISO_LATIN_1_ALWAYS */
      }
      free(line);
   }
   return TRUE;
}

int ReadPSCharSubs(FP, buf)
   FILE *FP;
   char *buf;
{
   char *line=NULL;
   int skip_processing=FALSE, ok=FALSE;
   PSCharSubs *ppscs=NULL;

   if (!PRTGIF || cmdLineOpenDisplay) skip_processing = TRUE;
   if (!skip_processing) {
      char *s=FindChar((int)'(', buf), *c_ptr=NULL;

      c_ptr = FindChar((int)'"', s);
      s = ReadString(c_ptr);
      *(--s) = '\0';

      if (FindTokenInPSCharSubs(c_ptr) != NULL) {
         sprintf(gszMsgBox, TgLoadString(STID_DUP_TOKEN_PS_CHAR_SUBS_READ),
               c_ptr);
         fprintf(stderr, "%s\n", gszMsgBox);
         skip_processing = TRUE;
      } else {
         ppscs = (PSCharSubs*)malloc(sizeof(PSCharSubs));
         if (ppscs == NULL) FailAllocMessage();
         memset(ppscs, 0, sizeof(PSCharSubs));
         ppscs->token_name = UtilStrDup(c_ptr);
         if (ppscs->token_name == NULL) FailAllocMessage();
         ok = TRUE;
      }
   }
   while ((line=UtilGetALine(FP)) != NULL) {
      char *c_ptr=NULL, *buf1=NULL, *ps_char_name=NULL;
      char info[MAXSTRING];

      scanLineNum++;
      if (*line == ']' || (line[0] == '\t' && line[1] == ']')) {
         free(line);
         break;
      }
      if (!skip_processing) {
#ifndef _PRTGIF_USE_ISO_LATIN_1_ALWAYS
         c_ptr = FindChar((int)'"', line);
         c_ptr = ParseStr(c_ptr, (int)'"', info, sizeof(info));
         buf1 = info;
         ps_char_name = strchr(buf1, '/');
         if (ps_char_name == NULL || ps_char_name[1] == '\0') {
            free(line);
            continue;
         }
         *ps_char_name = '\0';
         ProcessCharSubs(ppscs, buf1, &ps_char_name[1], NULL);
         *ps_char_name = '/';
#endif /* ~_PRTGIF_USE_ISO_LATIN_1_ALWAYS */
      }
      free(line);
   }
   if (ppscs != NULL) {
      if (ok) {
         ListAppend(&psCharSubsList, ppscs);
      } else {
         FreePSCharSubs(&ppscs);
      }
   }
   return TRUE;
}

static
int DontReencodeInXDefOrCmdLine(pszFontName, nFontIndex, nFontStyle)
   char *pszFontName;
   int nFontIndex, nFontStyle;
{
   char *c_ptr=NULL;

   if (gpszDontReencode != NULL) {
      char szFontStr[MAXSTRING];

      *szFontStr = '\0';
      if (pszFontName == NULL) {
         GetPSFontStr(nFontIndex, nFontStyle, szFontStr);
         if (strncmp(szFontStr, "/(", 2) == 0) {
            /* bitmapped PS font! */
            return FALSE;
         } else if (*szFontStr == '/') {
            *szFontStr = ' ';
         }
      } else {
         UtilStrCpyN(szFontStr, sizeof(szFontStr), pszFontName);
      }
      RemovePSFontNameVariations(szFontStr);
      UtilTrimBlanks(szFontStr);
      pszFontName = szFontStr;

      for (c_ptr=gpszDontReencode; *c_ptr != '\0'; c_ptr++) {
         if (UtilStrICmp(c_ptr, pszFontName) == 0) {
            return TRUE;
         }
         c_ptr += strlen(c_ptr);
      }
   }
   return FALSE;
}

static
void UpdateEncodeCharInfo(nIndex, psz)
   int nIndex;
   char *psz;
{
   int byte_index=(int)(((*psz) & 0x7f)>>3);
   short *flag_ptr=encodeCharFlags[nIndex];

   encodeFont[nIndex] = TRUE;
   flag_ptr[byte_index] |= (1<<((*psz) & 0x07));
}

static
void PrepareStrSeg(pStrSeg)
   StrSegInfo *pStrSeg;
{
   char *c_ptr=NULL;
   int font_and_style_index=0;

   if (pStrSeg->double_byte) return;

   if (pStrSeg->font_name == NULL) {
      if (pStrSeg->font == FONT_SYM) {
         return;
      }
   } else if (strcmp(pStrSeg->font_name, "Symbol") == 0) {
      return;
   }
   if (DontReencodeInXDefOrCmdLine(pStrSeg->font_name, pStrSeg->font,
         pStrSeg->style)) {
      return;
   } else if (PRTGIF && !cmdLineOpenDisplay && pStrSeg->font < 0) {
      /*
       * Since pStrSeg->font is < 0, there's no way to call
       *       UpdateEncodeCharInfo()!  Therefore, font encoding is forced!
       */
      return;
   }
   font_and_style_index = pStrSeg->font*MAXFONTSTYLES + pStrSeg->style;
   for (c_ptr=pStrSeg->dyn_str.s; *c_ptr != '\0'; c_ptr++) {
      if (((*c_ptr)&0x80) && *charCodeToName[(int)(*c_ptr-'\200')]=='8') {
         UpdateEncodeCharInfo(font_and_style_index, c_ptr);
      }
   }
}

static
void PrepareStrBlock(pStrBlock)
   StrBlockInfo *pStrBlock;
{
   switch (pStrBlock->type) {
   case SB_SIMPLE:
      PrepareStrSeg(pStrBlock->seg);
      break;

   case SB_CHAR_SPACE:
      break;

   case SB_SUPSUB_LEFT:
   case SB_SUPSUB_CENTER:
   case SB_SUPSUB_RIGHT:
      if (pStrBlock->sup != NULL) PrepareMiniLines(pStrBlock->sup);
      if (pStrBlock->sub != NULL) PrepareMiniLines(pStrBlock->sub);
      if (pStrBlock->type == SB_SUPSUB_CENTER) {
         PrepareStrSeg(pStrBlock->seg);
      }
      break;
   }
}

static
void PrepareMiniLine(pMiniLine)
   MiniLineInfo *pMiniLine;
{
   StrBlockInfo *pStrBlock=NULL;

   for (pStrBlock=pMiniLine->first_block; pStrBlock != NULL;
         pStrBlock=pStrBlock->next) {
      PrepareStrBlock(pStrBlock);
   }
}

void PrepareMiniLines(minilines)
   MiniLinesInfo *minilines;
{
   MiniLineInfo *pMiniLine=NULL;

   for (pMiniLine=minilines->first; pMiniLine != NULL;
         pMiniLine=pMiniLine->next) {
      PrepareMiniLine(pMiniLine);
   }
}

static
void PrepareText(ObjPtr)
   struct ObjRec *ObjPtr;
{
   MiniLinesInfo *minilines=(&ObjPtr->detail.t->minilines);

   PrepareMiniLines(minilines);
}

static
void PrepareObjFontInfo(ObjPtr)
   struct ObjRec *ObjPtr;
{
   struct ObjRec *obj_ptr;
   struct AttrRec *attr_ptr;

   for (obj_ptr=ObjPtr; obj_ptr != NULL; obj_ptr=obj_ptr->prev) {
      switch (obj_ptr->type) {
      case OBJ_TEXT: PrepareText(obj_ptr); break;

      case OBJ_GROUP:
      case OBJ_SYM:
      case OBJ_ICON: PrepareObjFontInfo(obj_ptr->detail.r->last); break;

      case OBJ_PIN:
         PrepareObjFontInfo(GetPinObj(obj_ptr)->detail.r->last);
         break;
      }
      for (attr_ptr=obj_ptr->lattr; attr_ptr!=NULL; attr_ptr=attr_ptr->prev) {
         PrepareText(attr_ptr->obj);
      }
   }
}

void PrepareEightBitFontInfo()
{
   struct ObjRec *obj_ptr;
   struct AttrRec *attr_ptr;
   int j, i;
   short *flag_ptr;
   struct PageRec *page_ptr;

   if (encodeCharFlagsAllocated && numEncodeCharFonts < numFonts) {
      CleanUpEncodeCharFonts();
   }
   if (!encodeCharFlagsAllocated) {
      int font_count=0;

      font_count = ((PRTGIF && !cmdLineOpenDisplay) ?
            MAXFONTS+numFakedFonts : numFonts+numFakedFonts);

      encodeCharFlags =
            (short**)malloc(font_count*MAXFONTSTYLES*sizeof(short*));
      if (encodeCharFlags == NULL) FailAllocMessage();
      encodeFont = (int*)malloc(font_count*MAXFONTSTYLES*sizeof(int));
      if (encodeFont == NULL) FailAllocMessage();
      memset(encodeFont, 0, font_count*MAXFONTSTYLES*sizeof(int));
      for (i = 0; i < font_count*MAXFONTSTYLES; i++) {
         encodeCharFlags[i] = (short*)malloc(16*sizeof(short));
         if (encodeCharFlags[i] == NULL) FailAllocMessage();
         memset(encodeCharFlags[i], 0, 16*sizeof(short));
      }
      encodeCharFlagsAllocated = TRUE;
      numEncodeCharFonts = font_count;
   }

   for (i=0; i < numEncodeCharFonts*MAXFONTSTYLES; i++) {
      encodeFont[i] = FALSE;
      flag_ptr = encodeCharFlags[i];
      for (j=0; j < 16; j++) flag_ptr[j] = 0;
   }
   for (page_ptr=firstPage; page_ptr != NULL; page_ptr=page_ptr->next) {
      for (obj_ptr=page_ptr->bot; obj_ptr != NULL; obj_ptr=obj_ptr->prev) {
         switch (obj_ptr->type) {
         case OBJ_TEXT: PrepareText(obj_ptr); break;

         case OBJ_GROUP:
         case OBJ_SYM:
         case OBJ_ICON: PrepareObjFontInfo(obj_ptr->detail.r->last); break;

         case OBJ_PIN:
            PrepareObjFontInfo(GetPinObj(obj_ptr)->detail.r->last);
            break;
         }
         for (attr_ptr=obj_ptr->lattr; attr_ptr!=NULL;
               attr_ptr=attr_ptr->prev) {
            PrepareText(attr_ptr->obj);
         }
      }
   }
}

int NeedEncode(FontName, FontIndex, StyleIndex)
   char *FontName;
   int FontIndex, StyleIndex;
{
   if (PRTGIF && !cmdLineOpenDisplay) {
      /* No need to do anything here because encodeFont[] takes care of it */
      if (FontIndex < 0) {
         /*
          * Since FontIndex is < 0, there's no way to compute an index into
          *       encodeFont[]!  Therefore, the decision to encode a font
          *       or not must be made here!  (By default, a font is encoded!)
          */
         return (FontName==NULL ? FALSE :
               !DontReencodeInXDefOrCmdLine(FontName, FontIndex, StyleIndex));
      }
   }
   if (FontIndex == FONT_SYM) {
      return FALSE;
   }
   return encodeFont[FontIndex*MAXFONTSTYLES+StyleIndex];
}

int GetFontIndex(FontStr, StyleIndex, MustFind)
   char *FontStr;
   int StyleIndex, MustFind;
{
   int i;
   int num_base_fonts=((PRTGIF && !cmdLineOpenDisplay) ? MAXFONTS : numFonts);
   char **base_font_info_str=((PRTGIF && !cmdLineOpenDisplay) ?
         initFontInfoStr : fontInfoStr);

   for (i=0; i < num_base_fonts; i++) {
      if (strcmp(
            base_font_info_str[(i*MAXFONTSTYLES+StyleIndex)*3+PS_FONT_NAME],
            FontStr) == 0) {
         return i;
      }
   }
   for ( ; i < num_base_fonts+numFakedFonts; i++) {
      if (strcmp(fontFamilies[i].name_faked, FontStr) == 0) {
         return i;
      }
   }
   if (MustFind) return INVALID;

   numFakedFonts++;
   if (fontFamilies == NULL) {
      fontFamilies = (struct FontFmlyRec *)malloc(
            (num_base_fonts+numFakedFonts)*sizeof(struct FontFmlyRec));
      if (fontFamilies == NULL) FailAllocMessage();
      memset(fontFamilies, 0,
            (num_base_fonts+numFakedFonts)*sizeof(struct FontFmlyRec));
   } else {
      fontFamilies = (struct FontFmlyRec *)realloc(fontFamilies,
            (num_base_fonts+numFakedFonts)*sizeof(struct FontFmlyRec));
      if (fontFamilies == NULL) FailAllocMessage();
      memset(&fontFamilies[num_base_fonts+numFakedFonts-1], 0,
            sizeof(struct FontFmlyRec));
   }
   fontFamilies[num_base_fonts+numFakedFonts-1].name_faked =
         (char*)malloc((strlen(FontStr)+1)*sizeof(char));
   if (fontFamilies[num_base_fonts+numFakedFonts-1].name_faked == NULL) {
      FailAllocMessage();
   }
   strcpy(fontFamilies[num_base_fonts+numFakedFonts-1].name_faked, FontStr);
   for (i=0; i < MAXFONTSTYLES; i++) {
      fontFamilies[num_base_fonts+numFakedFonts-1].fr[i].next = NULL;
      fontFamilies[num_base_fonts+numFakedFonts-1].fr[i].xfs = NULL;
   }
   return (num_base_fonts+numFakedFonts-1);
}

int GetFontAndStyle(FontStr, pnFont, pnStyle, MustFind)
   char *FontStr;
   int *pnFont, *pnStyle, MustFind;
{
   int font=0;
   int num_fonts=((PRTGIF && !cmdLineOpenDisplay) ? MAXFONTS : numFonts);

   for (font=0; font < num_fonts; font++) {
      int style=0;

      for (style=0; style < MAXFONTSTYLES; style++) {
         char font_str[MAXSTRING];

         GetPSFontStr(font, style, font_str);
         /* font_str[0] is '/' */
         if (UtilStrICmp(FontStr, &font_str[1]) == 0) {
            if (pnFont != NULL) *pnFont = font;
            if (pnStyle != NULL) *pnStyle = style;
            return TRUE;
         }
      }
   }
   if (MustFind) return FALSE;

   return TRUE;
}

int GetFontIndexFromPSFontAndStyle(font_str, style, pnDoubleByte, pnModified)
   char *font_str;
   int style, *pnDoubleByte, *pnModified;
{
   int len=strlen(font_str), font=FONT_TIM;

   if (pnModified != NULL) *pnModified = FALSE;
   if (len > 1 && *font_str == '\'' && font_str[len-1] == '\'') {
      char *psz1=(&font_str[1]), *psz2=NULL;

      font_str[len-1] = '\0';
      psz2 = font_str;
      while (*psz1 != '\0') *psz2++ = *psz1++;
      *psz2 = '\0';
   }
   font = GetFontIndex(font_str, style, (!PRTGIF || cmdLineOpenDisplay));
   if (font == INVALID && (!PRTGIF || cmdLineOpenDisplay)) {
      sprintf(gszMsgBox, TgLoadString(STID_CANNOT_FIND_FONT_USE_ALT),
            font_str, "Times");
      Msg(gszMsgBox);
      if (pnModified != NULL) *pnModified = TRUE;
      font = FONT_TIM;
      if (pnDoubleByte != NULL && *pnDoubleByte) {
         /*
          * font would have been the double-byte font.
          * But since it can't be found and Times is used, it should
          *     not be double-byte any more.
          */
         *pnDoubleByte = FALSE;
      }
   }
   return font;
}

void CleanUpDontReencode()
{
   if (gpszDontReencode != NULL) {
      UtilFree(gpszDontReencode);
      gpszDontReencode = NULL;
   }
   gnDontReencodeInitialized = FALSE;
}

void InitDontReencode(buf)
   char *buf;
{
   char *c_ptr=NULL, *pszDontReencode=NULL;

   if (buf == NULL && gnDontReencodeInitialized) return;

   if (buf == NULL) {
      gnDontReencodeInitialized = TRUE;
   }
   if (buf != NULL) {
      if (gpszDontReencode != NULL) {
         /* if it is already initialized, don't parse the content of buf */
         return;
      }
      if ((pszDontReencode=UtilStrDup(buf)) == NULL) FailAllocMessage();
      UtilTrimBlanks(pszDontReencode);
   } else if (PRTGIF && !cmdLineOpenDisplay) {
      if (*cmdLineDontReencode != '\0') {
         UtilTrimBlanks(cmdLineDontReencode);
         if ((pszDontReencode=UtilStrDup(cmdLineDontReencode)) == NULL) {
            FailAllocMessage();
         }
#ifdef _DONT_REENCODE
      } else {
         if ((pszDontReencode=UtilStrDup(_DONT_REENCODE)) == NULL) {
            FailAllocMessage();
         }
         UtilTrimBlanks(pszDontReencode);
#endif /* _DONT_REENCODE */
      }
   } else {
      if ((c_ptr=XGetDefault(mainDisplay, TOOL_NAME,
            "DontReencode")) != NULL) {
         if ((pszDontReencode=UtilStrDup(c_ptr)) == NULL) FailAllocMessage();
         UtilTrimBlanks(pszDontReencode);
#ifdef _DONT_REENCODE
      } else {
         if ((pszDontReencode=UtilStrDup(_DONT_REENCODE)) == NULL) {
            FailAllocMessage();
         }
         UtilTrimBlanks(pszDontReencode);
#endif /* _DONT_REENCODE */
      }
      if ((c_ptr=XGetDefault(mainDisplay, TOOL_NAME,
            "AdditionalDontReencode")) != NULL) {
         while (*c_ptr == ':') c_ptr++;
         if (pszDontReencode == NULL) {
            if ((pszDontReencode=UtilStrDup(c_ptr)) == NULL) FailAllocMessage();
         } else {
            int len=strlen(pszDontReencode);
            char *psz=(char*)malloc(sizeof(char)*(len+2+strlen(c_ptr)));

            if (psz == NULL) FailAllocMessage();
            sprintf(psz, "%s:%s", pszDontReencode, c_ptr);
            UtilFree(pszDontReencode);
            pszDontReencode = psz;
         }
         UtilTrimBlanks(pszDontReencode);
      }
   }
   if (pszDontReencode != NULL && *pszDontReencode != '\0') {
      char *c_ptr1=NULL;

      gpszDontReencode = (char*)malloc(strlen(pszDontReencode)+2);
      if (gpszDontReencode == NULL) FailAllocMessage();
      for (c_ptr1=gpszDontReencode, c_ptr=strtok(pszDontReencode,
            " ,:;\t\n\r"); c_ptr != NULL;
            c_ptr=strtok(NULL, " ,:;\t\n\r")) {
         strcpy(c_ptr1, c_ptr);
         c_ptr1 += strlen(c_ptr)+1;
      }
      *c_ptr1 = '\0';
   }
   UtilFree(pszDontReencode);
}

void InitPSFontCharSubs()
{
   char *c_ptr=NULL;

   gaPSFontNeedCharSubs = NULL;
   gnNumPSFontNeedCharSubs = 0;
   CVListInit(&psCharSubsList);

   if (mainDisplay == NULL) return;

   if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"PSFontNeedCharSubs")) !=
         NULL) {
      char *buf=UtilStrDup(c_ptr), *buf1=NULL, *psz_state=NULL;

      if (buf == NULL) FailAllocMessage();
      for (buf1=UtilStrTok(buf, " ,\t\n\r", &psz_state); buf1 != NULL;
            buf1=UtilStrTok(NULL, " ,\t\n\r", &psz_state)) {
         char *token_name=strchr(buf1, '=');

         if (token_name == NULL) {
            fprintf(stderr, TgLoadString(STID_BAD_XDEF_CANT_FIND_EQ_SKIP), buf1,
                  TOOL_NAME, "PSFontNeedCharSubs");
            fprintf(stderr, "\n");
            continue;
         }
         if (token_name[1] == '\0') {
            fprintf(stderr, TgLoadString(STID_BAD_XDEF_MISS_TOKEN_NAME_SKIP),
                  buf1, TOOL_NAME, "PSFontNeedCharSubs");
            fprintf(stderr, "\n");
            continue;
         }
         *token_name = '\0';
         if (LookupPSFontAliases(buf1) == NULL) {
            fprintf(stderr, TgLoadString(STID_BAD_FAKE_FONT_NAME_NO_ALIAS),
                  TOOL_NAME, "PSFontNeedCharSubs", buf1, TOOL_NAME,
                  "PSFontAliases");
            fprintf(stderr, "\n");
            *token_name = '=';
            continue;
         }
         ProcessPSCharSubs(buf1, &token_name[1]);
         *token_name = '=';
      }
      UtilFree(buf);
   }
}

void PrTgifInitBaseFonts()
{
   int i;

   if (fontFamilies != NULL) return;

   fontFamilies = (struct FontFmlyRec *)malloc(
         MAXFONTS*sizeof(struct FontFmlyRec));
   if (fontFamilies == NULL) FailAllocMessage();
   memset(fontFamilies, 0, MAXFONTS*sizeof(struct FontFmlyRec));
   for (i=0; i < MAXFONTS; i++) {
      int j;

      fontFamilies[i].name_faked = NULL;
      fontFamilies[i].bitmapped_ps_font = FALSE;
      *fontFamilies[i].choice_char = '\0';
      for (j=0; j < MAXFONTSTYLES; j++) {
         fontFamilies[i].fr[j].next = NULL;
         fontFamilies[i].fr[j].xfs = NULL;
      }
   }
}

int GetCurSzUnit()
{
   if (curChoice == DRAWTEXT && textCursorShown && editTextSize != 0) {
      return curRealSzUnit;
   } else {
      return curSzUnit;
   }
}

void PutCurSzUnit(sz_unit)
   int sz_unit;
{
   if (curChoice == DRAWTEXT && textCursorShown && editTextSize != 0) {
      curRealSzUnit = sz_unit;
   } else {
      curSzUnit = sz_unit;
   }
}

void GetCurFontMsg(buf, pStrSeg)
   char *buf;
   StrSegInfo *pStrSeg;
{
   char font_str[MAXSTRING];

   GetPSFontStr(curFont, curStyle, font_str);

   if (pStrSeg != NULL && editTextSize != 0) {
      if (showFontSizeInPoints) {
         sprintf(buf, "%s-%1dpt (%s %1dpt)",
               &font_str[1], SzUnitToPointSize(pStrSeg->real_sz_unit),
               TgLoadCachedString(CSTID_EDIT_TEXT_SIZE_SUBSTR),
               SzUnitToPointSize(curSzUnit));
      } else {
         sprintf(buf, "%s-%1d (%s %1d)",
               &font_str[1], SzUnitToFontSize(pStrSeg->real_sz_unit),
               TgLoadCachedString(CSTID_EDIT_TEXT_SIZE_SUBSTR),
               SzUnitToFontSize(curSzUnit));
      }
   } else {
      if (showFontSizeInPoints) {
         sprintf(buf, "%s-%1dpt", &font_str[1], SzUnitToPointSize(curSzUnit));
      } else {
         sprintf(buf, "%s-%1d", &font_str[1], SzUnitToFontSize(curSzUnit));
      }
   }
}

void CurFontMsg(nShowInMsgWindow, nShowInStatusWindow, pStrSeg)
   int nShowInMsgWindow, nShowInStatusWindow;
   StrSegInfo *pStrSeg;
{
   char buf[MAXSTRING];

   GetCurFontMsg(buf, pStrSeg);

   if (nShowInMsgWindow) Msg(buf);
   if (nShowInStatusWindow) SetStringStatus(buf);
}

void DumpReEncodeVector(FP, vec_prefix, vec_suffix, flag_ptr)
   FILE *FP;
   char *vec_prefix, *vec_suffix;
   short *flag_ptr;
{
   int j=0;
   PSCharSubs *ppscs=FindPSCharSubs(&vec_prefix[1]);

   fprintf(FP, "%s%s%s [\n",
         ((vec_prefix != NULL && *vec_prefix == '/') ? "" : "/"),
         vec_prefix, vec_suffix);
   for (j=0; j < 16; j++) {
      short flag=0;

      if (flag_ptr != NULL) {
         flag = *flag_ptr++;
      } else {
         /* dump the whole vector if flag_ptr is NULL */
         flag = 0xff;
      }
      if ((flag & 0xff) != 0) {
         int k=0;

         for (k=0; k < 8; k++) {
            if (flag & (1<<k)) {
               char *spec=charCodeToName[(j<<3)+k];

               if (spec && *spec && *spec != 'x') {
                  int index=((j<<3)+k);

                  /* avoid writing blank lines and "x" */
                  if (ppscs != NULL && ppscs->ps_char_name[index] != NULL) {
                     fprintf(FP, " 8#%03o %s\n", (index|0x080),
                           ppscs->ps_char_name[index]);
                  } else {
                     fprintf(FP, " %s\n", spec);
                  }
               }
            }
         }
      }
   }
   fprintf(FP, " ] def\n");
}

void DumpEightBitFontInfo(FP)
   FILE *FP;
{
   int font_index=0, style_index=0, font_count=0;

   font_count = ((PRTGIF && !cmdLineOpenDisplay) ? MAXFONTS+numFakedFonts :
         numFonts+numFakedFonts);

   for (font_index=0; font_index < font_count; font_index++) {
      for (style_index=0; style_index < MAXFONTSTYLES; style_index++) {
         if (NeedEncode(NULL, font_index, style_index)) {
            char font_str[MAXSTRING];

            *font_str = '\0';
            GetPSFontStr(font_index, style_index, font_str);
            if (strncmp(font_str, "/(", 2) == 0) {
               /* bitmapped PS font! */
            } else {
               char real_font_str[MAXSTRING];

               DumpReEncodeVector(FP, font_str, "-vec",
                     encodeCharFlags[font_index*MAXFONTSTYLES+style_index]);

               strcpy(real_font_str, font_str);
               MapAliasedPSFontName(real_font_str, sizeof(real_font_str));
               fprintf(FP, "%s %s-8 %s-vec tgifReEncodeSmall\n\n",
                     real_font_str, font_str, &font_str[1]);
               if (preDumpSetup) PSUseReencode(font_str);
            }
         }
      }
   }
}

static int fontSize75[]={ 8, 10, 12, 14, 18, 24 };
static int fontSize100[]={ 11, 14, 17, 20, 25, 34 };

int GetCompatibleSize(font_dpi, font_size)
   int font_dpi, font_size;
{
   switch (font_dpi) {
   case FONT_DPI_75: return fontSize75[font_size];
   case FONT_DPI_100: return fontSize100[font_size];
   }
   return INVALID;
}

int SetCanvasFont()
   /*
    * Use curFont, curStyle, and curSzUnit to setup canvasFontPtr,
    *       canvasFontDoubleByte, metrics related to the font, etc.
    * Returns TRUE to mean this font should be ``read-only''.
    */
{
   struct FontSizeRec *fs_ptr=NULL;
   int vert=FALSE, rc=TRUE;

   if (mainDisplay == NULL) return TRUE;

   actualCurFont = curFont;
   actualCurStyle = curStyle;
   actualCurSzUnit = curSzUnit;
   fs_ptr = FindFontInfo(curFont, curStyle, curSzUnit, FALSE);

   if (allowFontFaceSubstitution && fs_ptr == NULL) {
#ifdef _TGIF_DBG /* debug, do not translate */
      TgAssert(FALSE, "Font substitution detected in SetCanvasFont()", NULL);
#endif /* _TGIF_DBG */
      actualCurFont = defaultCurFont;
      actualCurStyle = defaultCurStyle;
      actualCurSzUnit = defaultCurSzUnit;
      fs_ptr = FindFontInfo(defaultCurFont, defaultCurStyle,
            defaultCurSzUnit, FALSE);
   }
   if (!allowFontFaceSubstitution && fs_ptr == NULL) {
      canvasFontSize = INVALID;
      return TRUE;
   }
   if (fs_ptr == NULL) {
      char buf[MAXSTRING];

      GetCurFontMsg(buf, NULL);
      fprintf(stderr, TgLoadString(STID_CANT_FIND_FONT_USE_ALT),
            buf, defaultFontName);
      fprintf(stderr, "\n");
      canvasFontPtr = defaultFontPtr;
      canvasFontSize = INVALID;
   } else {
      canvasFontPtr = fs_ptr->xfs;
      canvasFontSize = SzUnitToFontSize(fs_ptr->sz_unit);
      vert = fs_ptr->vert;
      rc = FALSE;
   }
   canvasFontAsc = canvasFontPtr->max_bounds.ascent;
   canvasFontDes = canvasFontPtr->max_bounds.descent;
   canvasFontDirection = canvasFontPtr->direction;
   /*
    * Need to fix this.
    */
   canvasFontDontReencode = FALSE;

   /*
    * The following does not seem to work any more because of UTF-8 fonts
    *     such as the ones that are iso10646-1 encoded.
    * canvasFontDoubleByte = (canvasFontPtr->min_byte1 != 0 ||
    *       canvasFontPtr->max_byte1 != 0);
    */
   canvasFontDoubleByte = (canvasFontPtr->min_byte1 != 0);
   if (canvasFontDoubleByte) {
      canvasFontDoubleByteModBytes = ((canvasFontPtr->min_byte1 & 0x80) == 0);
      canvasFontDoubleByteVertical = vert;
   } else {
      canvasFontDoubleByteModBytes = FALSE;
   }
   canvasFontHeight = canvasFontAsc + canvasFontDes;
   XSetFont(mainDisplay, drawGC, canvasFontPtr->fid);

   textCursorH = canvasFontHeight;

   return rc;
}

int CurFontCheck()
{
   changingFontSizeFromRead = FALSE;
   allowFontFaceSubstitution = FALSE;
   SetCanvasFont();
   allowFontFaceSubstitution = TRUE;
   changingFontSizeFromRead = TRUE;

   return (SzUnitToFontSize(curSzUnit) == canvasFontSize);
}

int TrySetCanvasFont(font, style, size, must_find_requested, pnGotRequested)
   int font, style, size, must_find_requested, *pnGotRequested;
   /*
    * if size is not available, try all sizes in fontSzUnits[]
    */
{
   int i, last=0, saved_size=size, saved_sz_unit=curSzUnit, sz_unit=0;

   if (pnGotRequested != NULL) *pnGotRequested = FALSE;

   curFont = font;
   curStyle = style;

   sz_unit = FontSizeToSzUnit(size);

   curSzUnit = sz_unit;
   SetCanvasFont();
   if (canvasFontSize != INVALID) {
      if (SzUnitToFontSize(curSzUnit) != canvasFontSize) {
         if (must_find_requested) {
            curSzUnit = saved_sz_unit;
            SetCanvasFont();
            return FALSE;
         }
         SetFileModified(TRUE);
         curSzUnit = FontSizeToSzUnit(canvasFontSize);
      }
      if (pnGotRequested != NULL) {
         *pnGotRequested = (canvasFontSize==saved_size);
      }
      return TRUE;
   }
   if (must_find_requested) {
      curSzUnit = saved_sz_unit;
      SetCanvasFont();
      return FALSE;
   }
   /* now try to find something close to the requested font */
   for (i=0; i < numFontSizes; i++) {
      int size1=SzUnitToFontSize(fontSzUnits[i]);

      if (size1 >= saved_size) {
         break;
      }
   }
   if (i == numFontSizes) {
      return FALSE;
   }
   last = i;
   for (i=0; i < last; i++) {
      curSzUnit = fontSzUnits[i];
      SetCanvasFont();
      if (canvasFontSize != INVALID) {
         if (SzUnitToFontSize(curSzUnit) != canvasFontSize) {
            SetFileModified(TRUE);
            curSzUnit = FontSizeToSzUnit(canvasFontSize);
         }
         if (pnGotRequested != NULL) {
            *pnGotRequested = (canvasFontSize==saved_size);
         }
         return TRUE;
      }
   }
   if (SzUnitToFontSize(fontSzUnits[last]) == saved_size) {
      last++;
   }
   for (i=last; i < numFontSizes; i++) {
      curSzUnit = fontSzUnits[i];
      SetCanvasFont();
      if (canvasFontSize != INVALID) {
         if (SzUnitToFontSize(curSzUnit) != canvasFontSize) {
            SetFileModified(TRUE);
            curSzUnit = FontSizeToSzUnit(canvasFontSize);
         }
         if (pnGotRequested != NULL) {
            *pnGotRequested = (canvasFontSize==saved_size);
         }
         return TRUE;
      }
   }
   curSzUnit = saved_sz_unit;
   SetCanvasFont();

   return FALSE;
}

void ObjFontInfoToCurFontInfo(TextPtr)
   struct TextRec *TextPtr;
{
   textJust = TextPtr->minilines.just;
   textVSpace = TextPtr->minilines.v_space;
   curUnderlineYOffset = TextPtr->underline_y_offset;
   curOverlineYOffset = TextPtr->overline_y_offset;
}

void StrSegInfoToCurFontInfo(pStrSeg)
   StrSegInfo *pStrSeg;
{
   curFont = pStrSeg->font;
   curStyle = pStrSeg->style;
   curSzUnit = pStrSeg->sz_unit;
   curUnderlineOn = pStrSeg->underline_on;
   curOverlineOn = pStrSeg->overline_on;
}

struct TmpFontFmlyRec {
   char				**font_strings;
   int				double_byte;
   struct TmpFontFmlyRec	*next, *prev;
};

static
void SetupFontInfoStr(num_new_fonts, first_fmly)
   int num_new_fonts;
   struct TmpFontFmlyRec *first_fmly;
{
   struct TmpFontFmlyRec *fmly_ptr=NULL, *next_fmly=NULL;
   int index=0, fmly_index=0;

   numFonts = MAXFONTS+num_new_fonts;
   fontFamilies = (struct FontFmlyRec *)malloc(
         numFonts*sizeof(struct FontFmlyRec));
   if (fontFamilies == NULL) FailAllocMessage();
   memset(fontFamilies, 0, numFonts*sizeof(struct FontFmlyRec));

   fontInfoStr =
         (char**)malloc(numFonts*MAXFONTSTYLES*3*sizeof(char*));
   fontMenuStr = (char**)malloc(numFonts*sizeof(char*));
   if (fontInfoStr == NULL || fontMenuStr == NULL) FailAllocMessage();

   memset(fontInfoStr, 0, numFonts*MAXFONTSTYLES*3*sizeof(char*));
   memset(fontMenuStr, 0, numFonts*sizeof(char*));

   index = MAXFONTS*MAXFONTSTYLES*3;
   for (fmly_ptr=first_fmly, fmly_index=MAXFONTS; fmly_ptr != NULL;
         fmly_ptr=next_fmly, fmly_index++) {
      char *c_ptr=NULL, font_name[MAXSTRING], choice_char[3];
      int i=0, j=0, bitmapped_ps_font=FALSE;
      char *ps_font_name=fmly_ptr->font_strings[PS_FONT_NAME];
      int font_name_len=strlen(ps_font_name);

      *choice_char = '\0';
      next_fmly = fmly_ptr->next;
      if (*ps_font_name == '(' &&
            ps_font_name[font_name_len-1] == ')') {
         strcpy(font_name, &ps_font_name[1]);
         font_name[font_name_len-2] = '\0';
         strcpy(ps_font_name, font_name);
         RemovePSFontNameVariations(font_name);
         bitmapped_ps_font = TRUE;
      } else {
         strcpy(font_name, ps_font_name);
         RemovePSFontNameVariations(font_name);
      }
      for (i=0, j=0; i<MAXFONTSTYLES; i++, j+=3) {
         char *psz=fmly_ptr->font_strings[j+PS_FONT_NAME];
         int len=strlen(psz);

         if (len > 0 && *psz == '(' && psz[len-1] == ')') {
            strcpy(gszMsgBox, &psz[1]);
            gszMsgBox[len-2] = '\0';
            strcpy(psz, gszMsgBox);
         }
      }
      fontMenuStr[fmly_index] =
            (char*)malloc((strlen(font_name)+3)*sizeof(char));
      if (fontMenuStr[fmly_index] == NULL) FailAllocMessage();
      strcpy(fontMenuStr[fmly_index], font_name);
      if (strstr(fmly_ptr->font_strings[X_FONT_PREFIX], "%d") != NULL &&
            UtilStrICmp(fmly_ptr->font_strings[X_FONT_SUFFIX], "V") == 0) {
         strcat(fontMenuStr[fmly_index], "-V");
      }
      for (i=0, j=0; i<MAXFONTSTYLES; i++) {
         if (UtilStrICmp(fmly_ptr->font_strings[j+X_FONT_SUFFIX], "V") == 0) {
            fontFamilies[fmly_index].fr[i].vert = TRUE;
         }
         fontInfoStr[index++] = fmly_ptr->font_strings[j++];
         fontInfoStr[index++] = fmly_ptr->font_strings[j++];
         fontInfoStr[index++] = fmly_ptr->font_strings[j++];
      }
      fontFamilies[fmly_index].double_byte = fmly_ptr->double_byte;
      free(fmly_ptr->font_strings);
      free(fmly_ptr);
      fontFamilies[fmly_index].fr[STYLE_NR].next =
            fontFamilies[fmly_index].fr[STYLE_BR].next =
            fontFamilies[fmly_index].fr[STYLE_NI].next =
            fontFamilies[fmly_index].fr[STYLE_BI].next = NULL;
      fontFamilies[fmly_index].fr[STYLE_NR].xfs =
            fontFamilies[fmly_index].fr[STYLE_BR].xfs =
            fontFamilies[fmly_index].fr[STYLE_NI].xfs =
            fontFamilies[fmly_index].fr[STYLE_BI].xfs = NULL;
      fontFamilies[fmly_index].bitmapped_ps_font = bitmapped_ps_font;
      *fontFamilies[fmly_index].choice_char = '\0';
      sprintf(gszMsgBox, "%sShowFontChar", font_name);
      if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,gszMsgBox)) != NULL) {
         int len=0;

         UtilStrCpyN(gszMsgBox, sizeof(gszMsgBox), c_ptr);
         UtilTrimBlanks(gszMsgBox);
         len = strlen(gszMsgBox);
         if (len > 0) {
            if (((*gszMsgBox) & 0x80) == 0) {
               if (gszMsgBox[len-1] == '"') gszMsgBox[--len] = '\0';
               gszMsgBox[len++] = '"';
               gszMsgBox[len] = '\0';
               c_ptr = ReadString(gszMsgBox);
               if (c_ptr != gszMsgBox) c_ptr--;
               *c_ptr = '\0';
            }
            strcpy(fontFamilies[fmly_index].choice_char, gszMsgBox);
         }
      }
      if (fontFamilies[fmly_index].double_byte) {
         char resource_name[MAXSTRING];
         int count=0;

         sprintf(resource_name, "%sConvFromUTF8", font_name);
         if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,resource_name)) != NULL) {
            UtilStrCpyN(gszMsgBox, sizeof(gszMsgBox), c_ptr);
            UtilTrimBlanks(gszMsgBox);
            if (*gszMsgBox != '\0') {
               for (count=0, c_ptr=strstr(gszMsgBox,"%s"); c_ptr!=NULL;
                     c_ptr=strstr(++c_ptr,"%s")) {
                  count++;
               }
               if (count > 1) {
                  fprintf(stderr, TgLoadString(STID_INVALID_XDEF), TOOL_NAME,
                        resource_name, gszMsgBox);
                  fprintf(stderr, "\n");
               } else {
                  fontFamilies[fmly_index].conv_from_utf8_cmd =
                        UtilStrDup(gszMsgBox);
                  if (fontFamilies[fmly_index].conv_from_utf8_cmd == NULL) {
                     FailAllocMessage();
                  }
               }
            }
         }
         sprintf(resource_name, "%sConvToUTF8", font_name);
         if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,resource_name)) != NULL) {
            UtilStrCpyN(gszMsgBox, sizeof(gszMsgBox), c_ptr);
            UtilTrimBlanks(gszMsgBox);
            if (*gszMsgBox != '\0') {
               for (count=0, c_ptr=strstr(gszMsgBox,"%s"); c_ptr!=NULL;
                     c_ptr=strstr(++c_ptr,"%s")) {
                  count++;
               }
               if (count > 1) {
                  fprintf(stderr, TgLoadString(STID_INVALID_XDEF), TOOL_NAME,
                        resource_name, gszMsgBox);
                  fprintf(stderr, "\n");
               } else {
                  fontFamilies[fmly_index].conv_to_utf8_cmd =
                        UtilStrDup(gszMsgBox);
                  if (fontFamilies[fmly_index].conv_to_utf8_cmd == NULL) {
                     FailAllocMessage();
                  }
               }
            }
         }
      }
   }
}

static
void CleanUpPSFontAliases()
{
   if (gnNumPSFontAliases > 0) {
      while (gnNumPSFontAliases-- > 0) {
         UtilFree(gaPSFontAliases[gnNumPSFontAliases].key);
         UtilFree(gaPSFontAliases[gnNumPSFontAliases].value);
      }
      free(gaPSFontAliases);
   }
   gaPSFontAliases = NULL;
   gnNumPSFontAliases = 0;
}

static
void InitPSFontAliases()
{
   char *c_ptr=NULL;

   gaPSFontAliases = NULL;
   gnNumPSFontAliases = 0;

   if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"PSFontAliases")) != NULL) {
      char *buf=UtilStrDup(c_ptr), *fake_font_name=NULL;

      if (buf == NULL) FailAllocMessage();
      for (fake_font_name=strtok(buf, ",\t\n\r"); fake_font_name != NULL;
            fake_font_name=strtok(NULL, ",\t\n\r")) {
         char *real_font_name=strchr(fake_font_name, '=');

         if (real_font_name == NULL) {
            fprintf(stderr, TgLoadString(STID_BAD_XDEF_CANT_FIND_EQ_SKIP),
                  fake_font_name, TOOL_NAME, "PSFontAliases");
            fprintf(stderr, "\n");
            continue;
         }
         *real_font_name = '\0';
         if (real_font_name[1] == '\0') {
            *real_font_name = '=';
            fprintf(stderr, TgLoadString(STID_BAD_XDEF_MISS_FONT_NAME_SKIP),
                  fake_font_name, TOOL_NAME, "PSFontAliases");
            fprintf(stderr, "\n");
            continue;
         }
         ProcessAPSFontAlias(fake_font_name, real_font_name);
         *real_font_name = '=';
      }
      UtilFree(buf);
   }
}

static
void InitAdditionalFonts()
{
   int i, j, len, fmly_index, style_index, num_new_fonts=0;
   int got_double_fonts=FALSE;
   char *c_ptr=NULL, *buf=NULL;
   struct TmpFontFmlyRec *first_fmly=NULL, *last_fmly=NULL;

   fontFamilies = NULL;
   if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"AdditionalFonts")) != NULL) {
      char *font_ptr=NULL;

      len = strlen(c_ptr);
      buf = (char*)malloc((len+1)*sizeof(char));
      if (buf == NULL) FailAllocMessage();
      strcpy(buf, c_ptr);
      for (font_ptr=buf; *font_ptr != '\0' &&
            strchr(" ,\t\n\r",*font_ptr) != NULL; font_ptr++) {
      }
      if (font_ptr != NULL && *font_ptr != '\0') {
         int ok=TRUE, index=0, font_exists=FALSE;
         char **font_strings=NULL;
         struct TmpFontFmlyRec *fmly_ptr=NULL;

         while (ok && font_ptr != NULL && *font_ptr != '\0') {
            font_strings = (char**)malloc((MAXFONTSTYLES*3)*sizeof(char*));
            if (font_strings == NULL) FailAllocMessage();
            memset(font_strings, 0, (MAXFONTSTYLES*3)*sizeof(char*));
            for (i=0, index=0; i<MAXFONTSTYLES; i++) {
               char *rgstry=NULL, *ps_name=NULL, *dup_ptr;

               dup_ptr = (font_ptr == NULL ? NULL : UtilStrDup(font_ptr));
               if ((font_ptr=strtok(font_ptr,",\t\n\r")) != NULL &&
                     (rgstry=strtok(NULL,",\t\n\r")) != NULL &&
                     (ps_name=strtok(NULL," ,\t\n\r")) != NULL &&
                     (font_exists=DoesFontExist(font_ptr,FALSE)) != FALSE) {
                  font_strings[index] = (char*)malloc(MAXSTRING*sizeof(char));
                  if (font_strings[index] == NULL) FailAllocMessage();
                  strcpy(font_strings[index], font_ptr);
                  UtilTrimBlanks(font_strings[index++]);
                  font_strings[index] = (char*)malloc(MAXSTRING*sizeof(char));
                  if (font_strings[index] == NULL) FailAllocMessage();
                  strcpy(font_strings[index], rgstry);
                  UtilTrimBlanks(font_strings[index++]);
                  font_strings[index] = (char*)malloc(MAXSTRING*sizeof(char));
                  if (font_strings[index] == NULL) FailAllocMessage();
                  strcpy(font_strings[index], ps_name);
                  UtilTrimBlanks(font_strings[index++]);
                  for (font_ptr=(&ps_name[strlen(ps_name)+1]);
                        *font_ptr != '\0' && 
                        strchr(" ,\t\n\r",*font_ptr) != NULL; font_ptr++) {
                  }
               } else {
                  int dup_len=(dup_ptr == NULL ? 0 : strlen(dup_ptr));

                  if (dup_len > 128) {
                     strcpy(&dup_ptr[128-3], "...");
                  }
                  ok = FALSE;
                  *gszMsgBox = '\0';
                  if (dup_ptr == NULL) {
                     sprintf(gszMsgBox,
                           TgLoadString(STID_MISS_FONT_SPEC_IN_XDEF),
                           TOOL_NAME, "AdditionalFonts");
                  } else if (font_ptr == NULL) {
                     sprintf(gszMsgBox,
                           TgLoadString(STID_INVALID_X_FONT_INFO_IN_XDEF),
                           TOOL_NAME, "AdditionalFonts", dup_ptr);
                  } else if (rgstry == NULL) {
                     sprintf(gszMsgBox,
                           TgLoadString(STID_INVALID_X_REG_INFO_IN_XDEF),
                           TOOL_NAME, "AdditionalFonts", dup_ptr);
                  } else if (ps_name == NULL) {
                     sprintf(gszMsgBox,
                           TgLoadString(STID_INVALID_PS_INFO_IN_XDEF),
                           TOOL_NAME, "AdditionalFonts", dup_ptr);
                  } else if (!font_exists) {
                     ok = TRUE;
                     if (reportMissingFonts) {
                        sprintf(gszMsgBox,
                              TgLoadCachedString(CSTID_NO_SUCH_FONT), font_ptr);
                     }
                     for (font_ptr=(&ps_name[strlen(ps_name)+1]);
                           *font_ptr != '\0' && 
                           strchr(" ,\t\n\r",*font_ptr) != NULL; font_ptr++) {
                     }
                  }
                  if (*gszMsgBox != '\0') {
                     fprintf(stderr, "%s\n", gszMsgBox);
                  }
               }
               if (dup_ptr != NULL) free(dup_ptr);
               if (!ok) break;
            }
            if (ok && font_exists) {
               num_new_fonts++;
               fmly_ptr = (struct TmpFontFmlyRec *)malloc(
                     sizeof(struct TmpFontFmlyRec));
               if (fmly_ptr == NULL) FailAllocMessage();
               memset(fmly_ptr, 0, sizeof(struct TmpFontFmlyRec));
               fmly_ptr->next = NULL;
               fmly_ptr->prev = last_fmly;
               fmly_ptr->font_strings = font_strings;
               if (last_fmly == NULL) {
                  first_fmly = fmly_ptr;
               } else {
                  last_fmly->next = fmly_ptr;
               }
               last_fmly = fmly_ptr;
            } else if (!font_exists) {
               if (font_strings != NULL) {
                  for (i=0; i<MAXFONTSTYLES*3; i++) {
                     UtilFree(font_strings[i]);
                  }
                  free(font_strings);
               }
            }
            if (font_ptr-buf >= len) break;
         }
      }
      free(buf);
   }
   if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"SquareDoubleByteFonts")) !=
         NULL) {
      char *font_ptr=NULL;

      len = strlen(c_ptr);
      buf = (char*)malloc((len+1)*sizeof(char));
      if (buf == NULL) FailAllocMessage();
      strcpy(buf, c_ptr);
      for (font_ptr=buf; *font_ptr != '\0' &&
            strchr(" ,\t\n\r",*font_ptr) != NULL; font_ptr++) {
      }
      if (font_ptr != NULL && *font_ptr != '\0') {
         int ok=TRUE, index=0, font_exists=FALSE;
         char **font_strings=NULL;
         struct TmpFontFmlyRec *fmly_ptr=NULL;

         while (ok && font_ptr != NULL && *font_ptr != '\0') {
            font_strings = (char**)malloc((MAXFONTSTYLES*3)*sizeof(char *));
            if (font_strings == NULL) FailAllocMessage();
            memset(font_strings, 0, (MAXFONTSTYLES*3)*sizeof(char*));
            for (i=0, index=0; i<MAXFONTSTYLES; i++) {
               char *rgstry=NULL, *ps_name=NULL, *dup_ptr;

               dup_ptr = (font_ptr == NULL ? NULL : UtilStrDup(font_ptr));
               if ((font_ptr=strtok(font_ptr,",\t\n\r")) != NULL &&
                     (rgstry=strtok(NULL,",\t\n\r")) != NULL &&
                     (ps_name=strtok(NULL," ,\t\n\r")) != NULL &&
                     (font_exists=DoesFontExist(font_ptr,TRUE)) != FALSE) {
                  font_strings[index] = (char*)malloc(MAXSTRING*sizeof(char));
                  if (font_strings[index] == NULL) FailAllocMessage();
                  strcpy(font_strings[index], font_ptr);
                  UtilTrimBlanks(font_strings[index++]);
                  font_strings[index] = (char*)malloc(MAXSTRING*sizeof(char));
                  if (font_strings[index] == NULL) FailAllocMessage();
                  strcpy(font_strings[index], rgstry);
                  UtilTrimBlanks(font_strings[index++]);
                  font_strings[index] = (char*)malloc(MAXSTRING*sizeof(char));
                  if (font_strings[index] == NULL) FailAllocMessage();
                  strcpy(font_strings[index], ps_name);
                  UtilTrimBlanks(font_strings[index++]);
                  for (font_ptr=(&ps_name[strlen(ps_name)+1]);
                        *font_ptr != '\0' && 
                        strchr(" ,\t\n\r",*font_ptr) != NULL; font_ptr++) {
                  }
               } else {
                  int dup_len=(dup_ptr == NULL ? 0 : strlen(dup_ptr));

                  if (dup_len > 128) {
                     strcpy(&dup_ptr[128-3], "...");
                  }
                  ok = FALSE;
                  *gszMsgBox = '\0';
                  if (dup_ptr == NULL) {
                     sprintf(gszMsgBox,
                           TgLoadString(STID_MISS_FONT_SPEC_IN_XDEF),
                           TOOL_NAME, "SquareDoubleByteFonts", dup_ptr);
                  } else if (font_ptr == NULL) {
                     sprintf(gszMsgBox,
                           TgLoadString(STID_INVALID_X_FONT_INFO_IN_XDEF),
                           TOOL_NAME, "SquareDoubleByteFonts", dup_ptr);
                  } else if (rgstry == NULL) {
                     sprintf(gszMsgBox,
                           TgLoadString(STID_INVALID_X_REG_INFO_IN_XDEF),
                           TOOL_NAME, "SquareDoubleByteFonts", dup_ptr);
                  } else if (ps_name == NULL) {
                     sprintf(gszMsgBox,
                           TgLoadString(STID_INVALID_PS_INFO_IN_XDEF),
                           TOOL_NAME, "SquareDoubleByteFonts", dup_ptr);
                  } else if (!font_exists) {
                     ok = TRUE;
                     if (reportMissingFonts) {
                        sprintf(gszMsgBox,
                              TgLoadCachedString(CSTID_NO_SUCH_FONT), font_ptr);
                     }
                     for (font_ptr=(&ps_name[strlen(ps_name)+1]);
                           *font_ptr != '\0' && 
                           strchr(" ,\t\n\r",*font_ptr) != NULL; font_ptr++) {
                     }
                  }
                  if (*gszMsgBox != '\0') {
                     fprintf(stderr, "%s\n", gszMsgBox);
                  }
               }
               if (dup_ptr != NULL) free(dup_ptr);
               if (!ok) break;
            }
            if (ok && font_exists) {
               num_new_fonts++;
               fmly_ptr = (struct TmpFontFmlyRec *)malloc(
                     sizeof(struct TmpFontFmlyRec));
               if (fmly_ptr == NULL) FailAllocMessage();
               memset(fmly_ptr, 0, sizeof(struct TmpFontFmlyRec));
               fmly_ptr->next = NULL;
               fmly_ptr->prev = last_fmly;
               fmly_ptr->font_strings = font_strings;
               fmly_ptr->double_byte = TRUE;
               if (last_fmly == NULL) {
                  first_fmly = fmly_ptr;
               } else {
                  last_fmly->next = fmly_ptr;
               }
               last_fmly = fmly_ptr;
            } else if (!font_exists) {
               if (font_strings != NULL) {
                  for (i=0; i<MAXFONTSTYLES*3; i++) {
                     UtilFree(font_strings[i]);
                  }
                  free(font_strings);
               }
            }
            if (font_ptr-buf >= len) break;
         }
      }
      free(buf);
      got_double_fonts = TRUE;
   }
   if (num_new_fonts > 0 && first_fmly != NULL) {
      /* setup fontInfoStr[] */
      SetupFontInfoStr(num_new_fonts, first_fmly);
   }
   if (fontFamilies == NULL) {
      fontFamilies =
            (struct FontFmlyRec *)malloc(numFonts*sizeof(struct FontFmlyRec));
      if (fontFamilies == NULL) FailAllocMessage();
      memset(fontFamilies, 0, numFonts*sizeof(struct FontFmlyRec));
      fontInfoStr = (char**)malloc(numFonts*MAXFONTSTYLES*3*sizeof(char*));
      fontMenuStr = (char**)malloc(numFonts*sizeof(char*));
      if (fontInfoStr == NULL || fontMenuStr == NULL) FailAllocMessage();
      memset(fontInfoStr, 0, numFonts*MAXFONTSTYLES*3*sizeof(char*));
      memset(fontMenuStr, 0, numFonts*sizeof(char*));
   }
   altFontInfoStr = (char**)malloc(MAXFONTS*MAXFONTSTYLES*3*sizeof(char*));
   if (altFontInfoStr == NULL) FailAllocMessage();
   for (j=0; j<MAXFONTS*MAXFONTSTYLES*3; j++) altFontInfoStr[j] = NULL;
   i = 0;
   fmly_index = 0;
   for (fmly_index=0; fmly_index < MAXFONTS; fmly_index++) {
      char s[MAXSTRING];
      int bitmapped_ps_font=FALSE;
      int font_name_len=strlen(initFontInfoStr[i+PS_FONT_NAME]);

      if (*initFontInfoStr[i+PS_FONT_NAME] == '(' &&
            initFontInfoStr[i+PS_FONT_NAME][font_name_len-1] == ')') {
         /*
          * This shouldn't happen because none of the initFontInfoStr
          *      should have the bitmapped_ps_font!
          */
         strcpy(s, &(initFontInfoStr[i+PS_FONT_NAME])[1]);
         s[font_name_len-2] = '\0';
         strcpy(initFontInfoStr[i+PS_FONT_NAME], s);
         RemovePSFontNameVariations(s);
         bitmapped_ps_font = TRUE;
      } else {
         strcpy(s, initFontInfoStr[i+PS_FONT_NAME]);
         RemovePSFontNameVariations(s);
      }
      fontMenuStr[fmly_index] = (char*)malloc((strlen(s)+1)*sizeof(char));
      if (fontMenuStr[fmly_index] == NULL) FailAllocMessage();
      strcpy(fontMenuStr[fmly_index], s );
      for (style_index=0; style_index < MAXFONTSTYLES; style_index++) {
         if (hasAlternateDefaultFonts && (c_ptr=XGetDefault(mainDisplay,
               TOOL_NAME, initFontInfoStr[i+PS_FONT_NAME])) != NULL) {
            char *ps_font_name=strchr(c_ptr, ',');

            if (ps_font_name != NULL) *ps_font_name++ = '\0';

            if (strstr(c_ptr, "%d") != NULL) {
               altFontInfoStr[i] = UtilStrDup(c_ptr);
               if (altFontInfoStr[i] == NULL) FailAllocMessage();
               fontInfoStr[i] = altFontInfoStr[i];
            } else {
               fprintf(stderr, TgLoadString(STID_ERR_PROCESS_FONT_USE_ALT),
                     TOOL_NAME, initFontInfoStr[i+PS_FONT_NAME], c_ptr,
                     fontNamePrefix, initFontInfoStr[i],
                     initFontInfoStr[i+X_FONT_SUFFIX]);
               fprintf(stderr, "\n");
               fontInfoStr[i] = initFontInfoStr[i];
            }
            i++;
            fontInfoStr[i] = initFontInfoStr[i];
            i++;
            if (ps_font_name != NULL) {
               altFontInfoStr[i] = UtilStrDup(ps_font_name);
               if (altFontInfoStr[i] == NULL) FailAllocMessage();
               fontInfoStr[i] = altFontInfoStr[i];
            } else {
               fontInfoStr[i] = initFontInfoStr[i];
            }
            i++;
         } else {
            fontInfoStr[i] = initFontInfoStr[i]; i++;
            if (fmly_index == FONT_TIM || fmly_index == FONT_COU ||
                  fmly_index == FONT_HEL || fmly_index == FONT_CEN) {
               fontInfoStr[i] = defaultLatin1FontCharEncoding; i++;
            } else {
               fontInfoStr[i] = initFontInfoStr[i]; i++;
            }
            fontInfoStr[i] = initFontInfoStr[i]; i++;
         }
      }
      fontFamilies[fmly_index].fr[STYLE_NR].next =
            fontFamilies[fmly_index].fr[STYLE_BR].next =
            fontFamilies[fmly_index].fr[STYLE_NI].next =
            fontFamilies[fmly_index].fr[STYLE_BI].next = NULL;
      fontFamilies[fmly_index].fr[STYLE_NR].xfs =
            fontFamilies[fmly_index].fr[STYLE_BR].xfs =
            fontFamilies[fmly_index].fr[STYLE_NI].xfs =
            fontFamilies[fmly_index].fr[STYLE_BI].xfs = NULL;
      fontFamilies[fmly_index].bitmapped_ps_font = bitmapped_ps_font;
      *fontFamilies[fmly_index].choice_char = '\0';
   }
   defaultSingleByteFont = FONT_TIM;
   if (got_double_fonts) {
      if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"DefaultSingleByteFont")) !=
            NULL) {
         for (i=0; i < numFonts; i++) {
            if (UtilStrICmp(c_ptr, fontMenuStr[i]) == 0) {
               if (IsFontDoubleByte(i)) {
                  fprintf(stderr,
                        TgLoadString(STID_INVALID_SB_FONT_USE_DB_FONT),
                        TOOL_NAME, "DefaultSingleByteFont", c_ptr);
                  fprintf(stderr, "\n");
               } else {
                  defaultSingleByteFont = i;
               }
               break;
            }
         }
      }
   }
}

/* do not translate -- program constants */
static char *gaszFontStyles[]={ "Roman", "Bold", "Italic", "BoldItalic", NULL };
static char *gaszFontJusts[]={ "Left", "Center", "Right", NULL };

#ifndef _NO_LOCALE_SUPPORT
#ifdef ENABLE_NLS
static
void InitABasicFontSet(pFontSet, ppszFontSetName, opt_name, pnFontWidth,
      pnFontAsc, pnFontDes, pnFontHeight, def_font_name)
   XFontSet *pFontSet;
   char **ppszFontSetName, *opt_name, *def_font_name;
   int *pnFontWidth, *pnFontAsc, *pnFontDes, *pnFontHeight;
{
   char *c_ptr=NULL;

   if ((c_ptr=XGetDefault(mainDisplay, TOOL_NAME, opt_name)) != NULL) {
      if (UtilStrICmp(c_ptr, "none") == 0) {
         /* don't attempt to call XCreateFontSet() */
      } else {
         char **missing_charsets=NULL, *default_string=NULL;
         int num_missing_charsets=0;
         XFontSet xfset=NULL;

         *ppszFontSetName = UtilStrDup(c_ptr);
         if (*ppszFontSetName == NULL) FailAllocMessage();
         xfset = XCreateFontSet(mainDisplay, *ppszFontSetName,
               &missing_charsets, &num_missing_charsets, &default_string);
         if (xfset == NULL) {
            fprintf(stderr, TgLoadString(STID_CANNOT_LOAD_FONTSET),
                  TOOL_NAME, opt_name, *ppszFontSetName);
            fprintf(stderr, "\n");
            if (*ppszFontSetName != NULL) free(*ppszFontSetName);
            *ppszFontSetName = NULL;
         } else {
            *pFontSet = xfset;
         }
      }
   } else {
      char **missing_charsets=NULL, *default_string=NULL;
      int num_missing_charsets=0;
      char *fn1=def_font_name;
      char *fn2="-*-*-medium-r-*--12-*-*-*-*-*-*-*";
      int sz=strlen(fn1)+strlen(fn2)+1;
      XFontSet xfset=NULL;

      memset(&xfset, 0, sizeof(XFontSet));

      *ppszFontSetName = (char*)malloc((sz+1)*sizeof(char));
      if (*ppszFontSetName == NULL) FailAllocMessage();
      sprintf(*ppszFontSetName, "%s,%s", fn1, fn2);
      xfset = XCreateFontSet(mainDisplay, *ppszFontSetName,
            &missing_charsets, &num_missing_charsets, &default_string);
      if (xfset == NULL) {
         fprintf(stderr, TgLoadString(STID_CANNOT_LOAD_FONTSET),
               TOOL_NAME, opt_name, *ppszFontSetName);
         fprintf(stderr, "\n");
         if (*ppszFontSetName != NULL) free(*ppszFontSetName);
         *ppszFontSetName = NULL;
      } else {
         *pFontSet = xfset;
      }
   }
   if (*pFontSet != NULL) {
      XFontStruct **xfset_list=NULL;
      char **font_name_list=NULL;
      int n=0;

      n = XFontsOfFontSet(*pFontSet, &xfset_list, &font_name_list);
      /* do not translate -- the string is used to measure things */
      *pnFontWidth = XmbTextEscapement(*pFontSet, "x", 1);
      *pnFontAsc = (*xfset_list)->max_bounds.ascent;
      *pnFontDes = (*xfset_list)->max_bounds.descent;
      *pnFontHeight = menuFontAsc + menuFontDes;
   }
}
#endif /* ENABLE_NLS */
#endif /* ~_NO_LOCALE_SUPPORT */

static
void InitABasicFontPtr(ppFontPtr, ppszFontName, opt_name, def_font_name,
      pnFontWidth, pnFontAsc, pnFontDes, pnFontHeight, pnFoundInXDefault,
      msg_font_name)
   XFontStruct **ppFontPtr;
   char **ppszFontName, *opt_name, *def_font_name, *msg_font_name;
   int *pnFontWidth, *pnFontAsc, *pnFontDes, *pnFontHeight, *pnFoundInXDefault;
{
   XFontStruct *xfs=NULL;
   char *c_ptr=NULL;

   *ppFontPtr = NULL;

   if ((c_ptr=XGetDefault(mainDisplay, TOOL_NAME, opt_name)) != NULL) {
      /* First try to see if the default is overridden by an X default */
      *ppszFontName = UtilStrDup(c_ptr);
      if (*ppszFontName == NULL) FailAllocMessage();
      xfs = XLoadQueryFont(mainDisplay, *ppszFontName);
      if (xfs == NULL) {
         fprintf(stderr, TgLoadString(STID_CANNOT_LOAD_FONT),
               TOOL_NAME, opt_name, *ppszFontName);
         fprintf(stderr, "\n");
         if (*ppszFontName != NULL) free(*ppszFontName);
         *ppszFontName = NULL;
      } else {
         *ppFontPtr = xfs;
         if (pnFoundInXDefault != NULL && msg_font_name == NULL) {
            *pnFoundInXDefault = TRUE;
         }
      }
   } else {
      /* Not in X default, now try the default value */
      *ppszFontName = UtilStrDup(def_font_name);
      if (*ppszFontName == NULL) FailAllocMessage();
      xfs = XLoadQueryFont(mainDisplay, *ppszFontName);
      if (xfs == NULL) {
         if (*ppszFontName != NULL) free(*ppszFontName);
         *ppszFontName = NULL;
      } else {
         *ppFontPtr = xfs;
      }
   }
   if (*ppszFontName == NULL && pnFoundInXDefault != NULL &&
         (*pnFoundInXDefault) && msg_font_name != NULL) {
      /*
       * If it's not in X default and the default value does not work,
       *         if MsgFont is valid (from a previous call to this function
       *         with opt_name being "MsgFont", use it!
       */
      *ppszFontName = UtilStrDup(msg_font_name);
      if (*ppszFontName == NULL) FailAllocMessage();
      xfs = XLoadQueryFont(mainDisplay, *ppszFontName);
      if (xfs == NULL) {
         fprintf(stderr, TgLoadString(STID_CANNOT_LOAD_FONT),
               TOOL_NAME, opt_name, *ppszFontName);
         fprintf(stderr, "\n");
         if (*ppszFontName != NULL) free(*ppszFontName);
         *ppszFontName = NULL;
      } else {
         *ppFontPtr = xfs;
      }
   }
   if (*ppFontPtr != NULL) {
      /* do not translate -- the string is used to measure things */
      *pnFontWidth = XTextWidth(*ppFontPtr, "x", 1);
      *pnFontAsc = (*ppFontPtr)->max_bounds.ascent;
      *pnFontDes = (*ppFontPtr)->max_bounds.descent;
      *pnFontHeight = menuFontAsc + menuFontDes;
   }
}

static
void InitFontSetsAndBasicFonts()
{
   int user_msg_font=FALSE;

   boldMsgFontDoubleByte = FALSE;
#ifndef _NO_LOCALE_SUPPORT
#ifdef ENABLE_NLS
   if (boldMsgFontSet != NULL) {
      char *c_ptr=NULL;

      if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"BoldMsgFontDoubleByte")) !=
            NULL && UtilStrICmp(c_ptr, "true") == 0) {
         boldMsgFontDoubleByte = TRUE;
      }
   }
#endif /* ENABLE_NLS */
#endif /* ~_NO_LOCALE_SUPPORT */

   menuFontSet = NULL;
#ifndef _NO_LOCALE_SUPPORT
#ifdef ENABLE_NLS
   InitABasicFontSet(&menuFontSet, &menuFontSetName, "MenuFontSet",
         &menuFontWidth, &menuFontAsc, &menuFontDes, &menuFontHeight,
         "-*-helvetica-medium-r-normal--12-*-*-*-*-*-*-*");
#endif /* ENABLE_NLS */
#endif /* ~_NO_LOCALE_SUPPORT */
   if (menuFontSet == NULL) {
      menuFontPtr = NULL;
      InitABasicFontPtr(&menuFontPtr, &menuFontName, "MenuFont",
            "-*-helvetica-medium-r-normal--12-*-*-*-*-*-*-*", &menuFontWidth,
            &menuFontAsc, &menuFontDes, &menuFontHeight, NULL, NULL);
   }
   msgFontSet = NULL;
#ifndef _NO_LOCALE_SUPPORT
#ifdef ENABLE_NLS
   InitABasicFontSet(&msgFontSet, &msgFontSetName, "MsgFontSet",
         &msgFontWidth, &msgFontAsc, &msgFontDes, &msgFontHeight,
         "-*-helvetica-medium-r-normal--12-*-*-*-*-*-*-*");
#endif /* ENABLE_NLS */
#endif /* ~_NO_LOCALE_SUPPORT */
   if (msgFontSet == NULL) {
      msgFontPtr = NULL;
      InitABasicFontPtr(&msgFontPtr, &msgFontName, "MsgFont",
            "-*-helvetica-medium-r-normal--12-*-*-*-*-*-*-*", &msgFontWidth,
            &msgFontAsc, &msgFontDes, &msgFontHeight, &user_msg_font, NULL);
   }
   boldMsgFontSet = NULL;
#ifndef _NO_LOCALE_SUPPORT
#ifdef ENABLE_NLS
   InitABasicFontSet(&boldMsgFontSet, &boldMsgFontSetName, "BoldMsgFontSet",
         &boldMsgFontWidth, &boldMsgFontAsc, &boldMsgFontDes,
         &boldMsgFontHeight, "-*-helvetica-bold-r-normal--12-*-*-*-*-*-*-*");
#endif /* ENABLE_NLS */
#endif /* ~_NO_LOCALE_SUPPORT */
   if (boldMsgFontSet == NULL) {
      boldMsgFontPtr = NULL;
      InitABasicFontPtr(&boldMsgFontPtr, &boldMsgFontName, "BoldMsgFont",
            "-*-helvetica-bold-r-normal--12-*-*-*-*-*-*-*", &boldMsgFontWidth,
            &boldMsgFontAsc, &boldMsgFontDes, &boldMsgFontHeight,
            &user_msg_font, msgFontName);
   }
   italicMsgFontSet = NULL;
#ifndef _NO_LOCALE_SUPPORT
#ifdef ENABLE_NLS
   InitABasicFontSet(&italicMsgFontSet, &italicMsgFontSetName,
         "ItalicMsgFontSet", &italicMsgFontWidth, &italicMsgFontAsc,
         &italicMsgFontDes, &italicMsgFontHeight,
         "-*-helvetica-medium-o-normal--12-*-*-*-*-*-*-*");
#endif /* ENABLE_NLS */
#endif /* ~_NO_LOCALE_SUPPORT */
   if (italicMsgFontSet == NULL) {
      italicMsgFontPtr = NULL;
      InitABasicFontPtr(&italicMsgFontPtr, &italicMsgFontName, "ItalicMsgFont",
            "-*-helvetica-medium-o-normal--12-*-*-*-*-*-*-*",
            &italicMsgFontWidth, &italicMsgFontAsc, &italicMsgFontDes,
            &italicMsgFontHeight, &user_msg_font, msgFontName);
   }
   boldItalicMsgFontSet = NULL;
#ifndef _NO_LOCALE_SUPPORT
#ifdef ENABLE_NLS
   InitABasicFontSet(&boldItalicMsgFontSet, &boldItalicMsgFontSetName,
         "BoldItalicMsgFontSet", &boldItalicMsgFontWidth, &boldItalicMsgFontAsc,
         &boldItalicMsgFontDes, &boldItalicMsgFontHeight,
         "-*-helvetica-bold-o-normal--12-*-*-*-*-*-*-*");
#endif /* ENABLE_NLS */
#endif /* ~_NO_LOCALE_SUPPORT */
   if (boldItalicMsgFontSet == NULL) {
      boldItalicMsgFontPtr = NULL;
      InitABasicFontPtr(&boldItalicMsgFontPtr, &boldItalicMsgFontName,
            "BoldItalicMsgFont", "-*-helvetica-bold-o-normal--12-*-*-*-*-*-*-*",
            &boldItalicMsgFontWidth, &boldItalicMsgFontAsc,
            &boldItalicMsgFontDes, &boldItalicMsgFontHeight, &user_msg_font,
            msgFontName);
   }
}

void InitFonts()
{
   int i=0, ruler_font_size=0;
   char *c_ptr=NULL, *buf=NULL, buf1[MAXSTRING];
   struct FontSizeRec *fs_ptr=NULL;
   XFontStruct *xfs=NULL;

   memset(&gstSavedFontInfo, 0, sizeof(struct PushedFontRec));

   rotatedVertFontImage = NULL;
   vertFontBitmap = rotatedVertFontBitmap = None;
   vertFontBitmapW = vertFontBitmapH = 0;

   debugScalableFonts = FALSE;
   if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"DebugScalableFonts")) !=
         NULL && UtilStrICmp(c_ptr, "true") == 0) {
      debugScalableFonts = TRUE;
   }
   defaultFontSize = DEFAULT_FONT_SIZE;
   if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"DefaultFontSize")) != NULL) {
      if ((defaultFontSize=atoi(c_ptr)) <= 0) {
         defaultFontSize = DEFAULT_FONT_SIZE;
         fprintf(stderr, TgLoadString(STID_INVALID_XDEF_USE_ALT_VALUE),
               TOOL_NAME, "DefaultFontSize", c_ptr, DEFAULT_FONT_SIZE);
         fprintf(stderr, "\n");
      }
   }
   strcpy(fontNamePrefix, "-*");
   if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"FontNamePrefix")) != NULL) {
      strcpy(fontNamePrefix, c_ptr);
   }
   strcpy(defaultLatin1FontCharEncoding, "iso8859-1");
   if ((c_ptr=
         XGetDefault(mainDisplay,TOOL_NAME,"DefaultLatin1FontCharEncoding")) !=
         NULL) {
      strcpy(defaultLatin1FontCharEncoding, c_ptr);
   }
   showFontSizeInPoints = FALSE;
   if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"ShowFontSizeInPoints")) !=
         NULL && UtilStrICmp(c_ptr, "true") == 0) {
      showFontSizeInPoints = TRUE;
   }
   fontSzUnits = NULL;
   if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"FontSizes")) != NULL) {
      char *size_ptr;
      int allocated=50, ok=TRUE;

      numFontSizes = 0;
      fontSzUnits = (int*)malloc(allocated*sizeof(int));
      if (fontSzUnits == NULL) FailAllocMessage();
      sizeMenuStr = (char**)malloc(allocated*sizeof(char*));
      if (sizeMenuStr == NULL) FailAllocMessage();
      buf = UtilStrDup(c_ptr);
      if (buf == NULL) FailAllocMessage();
      if ((size_ptr=strtok(buf," ,\t\n\r")) != NULL) {
         while (ok && size_ptr != NULL) {
            int size=0, sz_unit=0;
            char *psz=NULL, saved_ch='\0';

            UtilStrLower(size_ptr);
            /* do not translate -- program constants */
            if ((psz=strstr(size_ptr, "pt")) != NULL) {
               saved_ch = *psz;
               *psz = '\0';
               size = atoi(size_ptr);
               sz_unit = PointSizeToSzUnit(size);
               *psz = saved_ch;
            } else {
               size = atoi(size_ptr);
               sz_unit = FontSizeToSzUnit(size);
            }
            if (size <= 0 || size>=1000) {
               ok = FALSE;
               break;
            }
            if (numFontSizes >= allocated) {
               allocated += 50;
               fontSzUnits = (int*)realloc(fontSzUnits, allocated*sizeof(int));
               sizeMenuStr = (char**)realloc(sizeMenuStr,
                     allocated*sizeof(char*));
            }
            sizeMenuStr[numFontSizes] = (char*)malloc(40*sizeof(char));
            if (sizeMenuStr[numFontSizes] == NULL) FailAllocMessage();
            if (showFontSizeInPoints) {
               sprintf(sizeMenuStr[numFontSizes], "%3d pts",
                     SzUnitToPointSize(sz_unit));
            } else {
               sprintf(sizeMenuStr[numFontSizes], "%3d",
                     SzUnitToFontSize(sz_unit));
            }
            fontSzUnits[numFontSizes++] = sz_unit;
            size_ptr = strtok(NULL, " ,\t\n\r");
         }
      }
      UtilFree(buf);
      if (!ok || numFontSizes <= 0) {
         for (i=0; i < numFontSizes; i++) free(sizeMenuStr[i]);
         free(sizeMenuStr);
         sizeMenuStr = NULL;
         free(fontSzUnits);
         fontSzUnits = NULL;
         numFontSizes = 0;
         fprintf(stderr, TgLoadString(STID_ERR_PROCESS_SIZE_USE_DEF_SIZE),
               TOOL_NAME, "FontSizes");
         fprintf(stderr, "\n");
      }
   }
   if (fontSzUnits == NULL) {
      numFontSizes = (MAXFONTSIZES<<1)-1;
      fontSzUnits = (int*)malloc(numFontSizes*sizeof(int));
      if (fontSzUnits == NULL) FailAllocMessage();
      sizeMenuStr = (char**)malloc(numFontSizes*sizeof(char*));
      if (sizeMenuStr == NULL) FailAllocMessage();
      for (i=0; i<numFontSizes; i++) {
         char *psz=initSizeMenuStr[i];

         fontSzUnits[i] = FontSizeToSzUnit(atoi(psz));
         sizeMenuStr[i] = (char*)malloc(40*sizeof(char));
         if (sizeMenuStr[i] == NULL) FailAllocMessage();
         if (showFontSizeInPoints) {
            sprintf(sizeMenuStr[i], "%3d pts",
                  SzUnitToPointSize(FontSizeToSzUnit(atoi(psz))));
         } else {
            strcpy(sizeMenuStr[i], psz);
         }
      }
   }
   hasAlternateDefaultFonts = FALSE;
   if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"HasAlternateDefaultFonts")) !=
         NULL && UtilStrICmp(c_ptr, "true") == 0) {
      hasAlternateDefaultFonts = TRUE;
   }
#ifdef _TGIF_DBG
   reportMissingFonts = TRUE;
   if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"ReportMissingFonts")) !=
         NULL && UtilStrICmp(c_ptr, "false") == 0) {
      reportMissingFonts = FALSE;
   }
#else /* ~ _TGIF_DBG */
   reportMissingFonts = FALSE;
   if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"ReportMissingFonts")) !=
         NULL && UtilStrICmp(c_ptr, "true") == 0) {
      reportMissingFonts = TRUE;
   }
#endif /* _TGIF_DBG */
   InitDontReencode(NULL);
   InitAdditionalFonts();
   InitPSFontAliases();
   InitPSFontCharSubs();

   curFont = FONT_COU;
   curStyle = STYLE_NR;
   curSzUnit = FontSizeToSzUnit(14);

   ruler_font_size = 10;
   if ((c_ptr=XGetDefault(mainDisplay, TOOL_NAME, "RulerFontSize")) != NULL) {
      ruler_font_size = atoi(c_ptr);
      if (ruler_font_size <= 0) {
         ruler_font_size = defaultFontSize;
         fprintf(stderr, TgLoadString(STID_INVALID_XDEF_USE_ALT_VALUE),
               TOOL_NAME, "RulerFontSize", c_ptr, ruler_font_size);
         fprintf(stderr, "\n");
      }
   }
   if ((c_ptr=XGetDefault(mainDisplay, TOOL_NAME, "InitialFontDPI")) != NULL) {
      fprintf(stderr, TgLoadString(STID_OBSOLETE_XDEF_USE_ALT),
            TOOL_NAME, "InitialFontDPI", TOOL_NAME, "InitialFontSize");
      fprintf(stderr, "\n");
   }
   if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"MsgFontSizeIndex")) != NULL) {
      fprintf(stderr, TgLoadString(STID_OBSOLETE_XDEF_USE_ALT),
            TOOL_NAME, "MsgFontSizeIndex", TOOL_NAME, "MsgFontSize");
      fprintf(stderr, "\n");
   }
   if ((c_ptr=XGetDefault(mainDisplay, TOOL_NAME, "MsgFontSize")) != NULL) {
      UtilStrCpyN(buf1, sizeof(buf1), c_ptr);
      curSzUnit = FontSizeToSzUnit(atoi(buf1));
      if (curSzUnit <= 0) {
         curSzUnit = FontSizeToSzUnit(defaultFontSize);
         fprintf(stderr, TgLoadString(STID_INVALID_XDEF_USE_ALT_VALUE),
               TOOL_NAME, "MsgFontSize", buf1, defaultFontSize);
         fprintf(stderr, "\n");
      }
   }
   defaultFontPtr = NULL;
   if ((c_ptr=XGetDefault(mainDisplay, TOOL_NAME, "DefFixedWidthFont")) !=
         NULL) {
      defaultFontName = UtilStrDup(c_ptr);
      if (defaultFontName == NULL) FailAllocMessage();
      xfs = XLoadQueryFont(mainDisplay, defaultFontName);
      if (xfs == NULL) {
         fprintf(stderr, TgLoadString(STID_CANNOT_LOAD_FONT),
               TOOL_NAME, "DefFixedWidthFont", defaultFontName);
         fprintf(stderr, "\n");
         if (defaultFontName != NULL) free(defaultFontName);
         defaultFontName = NULL;
         defaultFontPtr = NULL;
      } else {
         defaultFontPtr = xfs;
         defaultFontWidth = defaultFontPtr->max_bounds.width;
         defaultFontAsc = defaultFontPtr->max_bounds.ascent;
         defaultFontDes = defaultFontPtr->max_bounds.descent;
         defaultFontHeight = defaultFontAsc + defaultFontDes;
      }
   }
   if (defaultFontPtr == NULL) {
      fs_ptr = FindFontInfo(curFont, curStyle, curSzUnit, TRUE);
      if (fs_ptr == NULL) {
         sprintf(gszMsgBox, TgLoadString(STID_CANT_OPEN_DEF_MSG_FONT_ABORT),
               gszAttemptedFontName);
         Error("OpenFont()", gszMsgBox);
      }
      defaultFontPtr = fs_ptr->xfs;
      defaultFontWidth = defaultFontPtr->max_bounds.width;
      defaultFontAsc = defaultFontPtr->max_bounds.ascent;
      defaultFontDes = defaultFontPtr->max_bounds.descent;
      defaultFontHeight = defaultFontAsc + defaultFontDes;
   }
   if (defaultFontWidth < 1 || defaultFontHeight < 1) {
      fprintf(stderr, TgLoadString(STID_WARN_VERY_SMALL_DEF_FONT),
            defaultFontWidth, defaultFontHeight);
      fprintf(stderr, "\n");
      defaultFontWidth = 9;
      defaultFontHeight = 14;
   }
   rulerFontPtr = NULL;
   if ((c_ptr=XGetDefault(mainDisplay, TOOL_NAME, "DefFixedWidthRulerFont")) !=
         NULL) {
      rulerFontName = UtilStrDup(c_ptr);
      if (rulerFontName == NULL) FailAllocMessage();
      xfs = XLoadQueryFont(mainDisplay, rulerFontName);
      if (xfs == NULL) {
         fprintf(stderr, TgLoadString(STID_CANNOT_LOAD_FONT),
               TOOL_NAME, "DefFixedWidthRulerFont", rulerFontName);
         fprintf(stderr, "\n");
         if (rulerFontName != NULL) free(rulerFontName);
         rulerFontName = NULL;
         rulerFontPtr = NULL;
      } else {
         rulerFontPtr = xfs;
         rulerFontWidth = rulerFontPtr->max_bounds.width;
         rulerFontAsc = rulerFontPtr->max_bounds.ascent;
         rulerFontDes = rulerFontPtr->max_bounds.descent;
         rulerFontHeight = rulerFontAsc + rulerFontDes;
      }
   }
   if (rulerFontPtr == NULL) {
      fs_ptr = FindFontInfo(curFont, curStyle,
            FontSizeToSzUnit(ruler_font_size), TRUE);
      if (fs_ptr == NULL) {
         sprintf(gszMsgBox, TgLoadString(STID_CANT_OPEN_RULER_FONT_ABORT),
               gszAttemptedFontName);
         Error("OpenFont()", gszMsgBox);
      }
      rulerFontPtr = fs_ptr->xfs;
      rulerFontWidth = rulerFontPtr->max_bounds.width;
      rulerFontAsc = rulerFontPtr->max_bounds.ascent;
      rulerFontDes = rulerFontPtr->max_bounds.descent;
      rulerFontHeight = rulerFontAsc + rulerFontDes;
   }
   if (rulerFontWidth < 1 || rulerFontHeight < 1) {
      fprintf(stderr, TgLoadString(STID_WARN_VERY_SMALL_RULER_FONT),
            rulerFontWidth, rulerFontHeight);
      fprintf(stderr, "\n");
      rulerFontWidth = 7;
      rulerFontHeight = 12;
   }
   InitFontSetsAndBasicFonts();

   initialFont = curFont;
   if ((c_ptr=XGetDefault(mainDisplay, TOOL_NAME, "InitialFont")) != NULL) {
      for (i = 0; i < numFonts; i++) {
         if (strcmp(c_ptr, fontMenuStr[i]) == 0) {
            break;
         }
      }
      if (i != numFonts) {
         initialFont = curFont = i;
      } else {
         fprintf(stderr, TgLoadString(STID_CANNOT_SET_INITIAL_FONT_TO), c_ptr);
         fprintf(stderr, "\n");
      }
   }
   if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"InitialFontStyle")) != NULL) {
      for (i = 0; i < MAXFONTSTYLES; i++) {
         if (UtilStrICmp(c_ptr, gaszFontStyles[i]) == 0) {
            curStyle = i;
            break;
         }
      }
      if (i == MAXFONTSTYLES) {
         fprintf(stderr, TgLoadString(STID_CANT_SET_INIT_FONTSTYLE_ALT),
               c_ptr, "Roman");
         fprintf(stderr, "\n");
      }
   }
   if ((c_ptr=XGetDefault(mainDisplay, TOOL_NAME, "InitialFontJust")) != NULL) {
      for (i = 0; i < MAXJUSTS; i++) {
         if (UtilStrICmp(c_ptr, gaszFontJusts[i]) == 0) {
            textJust = i;
            break;
         }
      }
      if (i == MAXJUSTS) {
         fprintf(stderr, TgLoadString(STID_CANT_SET_INIT_FONTJUST_ALT),
               c_ptr, "Left");
         fprintf(stderr, "\n");
      }
   }
   if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"InitialFontSizeIndex")) !=
         NULL) {
      fprintf(stderr, TgLoadString(STID_OBSOLETE_XDEF_USE_ALT),
            TOOL_NAME, "InitialFontSizeIndex", TOOL_NAME, "InitialFontSize");
      fprintf(stderr, "\n");
   }
   if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"InitialFontSize")) != NULL) {
      UtilStrCpyN(buf1, sizeof(buf1), c_ptr);
      UtilStrLower(buf1);
      if ((c_ptr=strstr(buf1, "pt")) != NULL) {
         char saved_ch=(*c_ptr);

         *c_ptr = '\0';
         curSzUnit = PointSizeToSzUnit(atoi(buf1));
         *c_ptr = saved_ch;
      } else {
         curSzUnit = FontSizeToSzUnit(atoi(buf1));
      }
      if (curSzUnit <= 0) {
         curSzUnit = FontSizeToSzUnit(defaultFontSize);
         fprintf(stderr, TgLoadString(STID_CANT_SET_INIT_FONTSIZE_ALT),
               buf1, defaultFontSize);
         fprintf(stderr, "\n");
      }
   }
   curUnderlineOn = FALSE;
   if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"InitialUnderlineOn"))!=NULL &&
         UtilStrICmp(c_ptr, "true") == 0) {
      curUnderlineOn = TRUE;
   }
   curUnderlineYOffset = 2;
   if ((c_ptr=XGetDefault(mainDisplay, TOOL_NAME,
         "InitialUnderlineAmount")) != NULL) {
      curUnderlineYOffset = atoi(c_ptr);
   }
   curOverlineOn = FALSE;
   if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"InitialOverlineOn"))!=NULL &&
         UtilStrICmp(c_ptr, "true") == 0) {
      curOverlineOn = TRUE;
   }
   curOverlineYOffset = (-1);
   if ((c_ptr=XGetDefault(mainDisplay, TOOL_NAME,
         "InitialOverlineAmount")) != NULL) {
      curOverlineYOffset = atoi(c_ptr);
   }
   changePropertiesOfAttrs = FALSE;
   if ((c_ptr=XGetDefault(mainDisplay, TOOL_NAME,
         "ChangePropertiesOfAttrs")) != NULL &&
         UtilStrICmp(c_ptr, "true") == 0) {
      changePropertiesOfAttrs = TRUE;
   }
   if (FindFontInfo(curFont, curStyle, curSzUnit, TRUE) == NULL) {
      sprintf(gszMsgBox, TgLoadString(STID_CANT_LOAD_INITIAL_FONT),
            gszAttemptedFontName, TOOL_NAME, TOOL_NAME, TOOL_NAME);
      fprintf(stderr, "%s\n", gszMsgBox);
   }
}

void SaveCurFont()
{
   gstSavedFontInfo.font = curFont;
   gstSavedFontInfo.sz_unit = curSzUnit;
   gstSavedFontInfo.style = curStyle;
   gstSavedFontInfo.double_byte = canvasFontDoubleByte;
   gstSavedFontInfo.double_byte_mod_bytes = canvasFontDoubleByteModBytes;
   gstSavedFontInfo.double_byte_vertical = canvasFontDoubleByteVertical;
   gstSavedFontInfo.just = textJust;
   gstSavedFontInfo.pen = penPat;
   gstSavedFontInfo.fill = objFill;
   gstSavedFontInfo.trans_pat = transPat;
   gstSavedFontInfo.v_space = textVSpace;
   gstSavedFontInfo.underline_on = curUnderlineOn;
   gstSavedFontInfo.underline_y_offset = curUnderlineYOffset;
   gstSavedFontInfo.overline_on = curOverlineOn;
   gstSavedFontInfo.overline_y_offset = curOverlineYOffset;
}

void RestoreCurFont()
{
   curFont = gstSavedFontInfo.font;
   curSzUnit = gstSavedFontInfo.sz_unit;
   curStyle = gstSavedFontInfo.style;
   canvasFontDoubleByte = gstSavedFontInfo.double_byte;
   canvasFontDoubleByteModBytes = gstSavedFontInfo.double_byte_mod_bytes;
   canvasFontDoubleByteVertical = gstSavedFontInfo.double_byte_vertical;
   textJust = gstSavedFontInfo.just;
   penPat = gstSavedFontInfo.pen;
   objFill = gstSavedFontInfo.fill;
   transPat = gstSavedFontInfo.trans_pat;
   textVSpace = gstSavedFontInfo.v_space;
   curUnderlineOn = gstSavedFontInfo.underline_on;
   curUnderlineYOffset = gstSavedFontInfo.underline_y_offset;
   curOverlineOn = gstSavedFontInfo.overline_on;
   curOverlineYOffset = gstSavedFontInfo.overline_y_offset;

   if (!PRTGIF) SetCanvasFont();
}

static struct PushedFontRec *topPushedFontStk=NULL;

struct PushedFontRec *GetTopFontStk()
{
   return topPushedFontStk;
}

struct PushedFontRec *PushCurFont()
{
   struct PushedFontRec *pushed_font_ptr=(struct PushedFontRec *)malloc(
         sizeof(struct PushedFontRec));

   if (pushed_font_ptr == NULL) FailAllocMessage();

   memset(pushed_font_ptr, 0, sizeof(struct PushedFontRec));
   pushed_font_ptr->next = topPushedFontStk;
   pushed_font_ptr->font = curFont;
   pushed_font_ptr->sz_unit = curSzUnit;
   pushed_font_ptr->style = curStyle;
   pushed_font_ptr->just = textJust;
   pushed_font_ptr->pen = penPat;
   pushed_font_ptr->fill = objFill;
   pushed_font_ptr->trans_pat = transPat;
   pushed_font_ptr->v_space = textVSpace;
   pushed_font_ptr->color = colorIndex;
   pushed_font_ptr->underline_on = curUnderlineOn;
   pushed_font_ptr->underline_y_offset = curUnderlineYOffset;
   pushed_font_ptr->overline_on = curOverlineOn;
   pushed_font_ptr->overline_y_offset = curOverlineYOffset;
   pushed_font_ptr->double_byte = canvasFontDoubleByte;
   pushed_font_ptr->double_byte_mod_bytes = canvasFontDoubleByteModBytes;
   pushed_font_ptr->double_byte_vertical = canvasFontDoubleByteVertical;
   topPushedFontStk = pushed_font_ptr;

   return topPushedFontStk;
}

void PopCurFont()
{
   struct PushedFontRec *pushed_font_ptr=NULL;

   if (topPushedFontStk == NULL) {
      /* debug, do not translate */
      FatalUnexpectedError("Empty stack in PopCurFont().", NULL);
      return;
   }
   pushed_font_ptr = topPushedFontStk->next;

   curFont = topPushedFontStk->font;
   curSzUnit = topPushedFontStk->sz_unit;
   curStyle = topPushedFontStk->style;
   textJust = topPushedFontStk->just;
   penPat = topPushedFontStk->pen;
   objFill = topPushedFontStk->fill;
   transPat = topPushedFontStk->trans_pat;
   textVSpace = topPushedFontStk->v_space;
   colorIndex = topPushedFontStk->color;
   curUnderlineOn = topPushedFontStk->underline_on;
   curUnderlineYOffset = topPushedFontStk->underline_y_offset;
   curOverlineOn = topPushedFontStk->overline_on;
   curOverlineYOffset = topPushedFontStk->overline_y_offset;
   canvasFontDoubleByte = topPushedFontStk->double_byte;
   canvasFontDoubleByteModBytes = topPushedFontStk->double_byte_mod_bytes;
   canvasFontDoubleByteVertical = topPushedFontStk->double_byte_vertical;

   free(topPushedFontStk);
   topPushedFontStk = pushed_font_ptr;

   if (!PRTGIF) SetCanvasFont();
}

void SetPushedFontValue(which, value)
   int which, value;
{
   if (topPushedFontStk == NULL) {
      if (!editingText) {
         /* NewCurText() doesn't call PushCurFont() */
         return;
      }
      /* debug, do not translate */
      FatalUnexpectedError("Empty stack in SetPushedFontValue().", NULL);
      return;
   }
   switch(which) {
   case PUSH_FONT: topPushedFontStk->font = value; break;
   case PUSH_SZ_UNIT: topPushedFontStk->sz_unit = value; break;
   case PUSH_STYLE: topPushedFontStk->style = value; break;
   case PUSH_JUST: topPushedFontStk->just = value; break;
   case PUSH_PEN: topPushedFontStk->pen = value; break;
   case PUSH_FILL: topPushedFontStk->fill = value; break;
   case PUSH_TRANSPAT: topPushedFontStk->trans_pat = value; break;
   case PUSH_VSPACE: topPushedFontStk->v_space = value; break;
   case PUSH_COLORINDEX: topPushedFontStk->color = value; break;
   case PUSH_UNDERLINE_ON: topPushedFontStk->underline_on = value; break;
   case PUSH_UNDERLINE_Y_OFFSET:
      topPushedFontStk->underline_y_offset = value;
      break;
   case PUSH_OVERLINE_ON: topPushedFontStk->overline_on = value; break;
   case PUSH_OVERLINE_Y_OFFSET:
      topPushedFontStk->overline_y_offset = value;
      break;
   case PUSH_DOUBLEBYTE: topPushedFontStk->double_byte = value; break;
   }
}

static
int AdjTransformedTextBBox(ObjPtr)
   struct ObjRec *ObjPtr;
   /* a transformed text object has just changed it's font */
   /*      property, so its bounding box needs to be adjusted */
   /* returns FALSE if font size is invalid and the text will be read_only */
   /* returns INVALID if font size is invalid and the text will be read_only */
{
   struct XfrmMtrxRec *ctm=ObjPtr->ctm;

   if (ctm == NULL) {
      if (UpdTextBBox(ObjPtr)) {
         return TRUE;
      } else {
         return INVALID;
      }
   }
   ObjPtr->ctm = NULL;
   if (!UpdTextBBox(ObjPtr)) {
      ObjPtr->ctm = ctm;
      return FALSE;
   }
   SetCTM(ObjPtr, ctm);
   UpdTextBBox(ObjPtr);
   AdjObjSplineVs(ObjPtr);
   AdjObjBBox(ObjPtr);
   return TRUE;
}

static
int ChangeStrSegFontProp(pStrSeg, which, value)
   StrSegInfo *pStrSeg;
   int which, value;
{
   int changed=FALSE, font=0, style=0, sz_unit=0, double_byte=FALSE;

   font = pStrSeg->font;
   style = pStrSeg->style;
   sz_unit = pStrSeg->sz_unit;

   switch (which) {
   case PUSH_STYLE:
      style = value;
      if (pStrSeg->style == style) return FALSE;
      break;
   case PUSH_SZ_UNIT:
      sz_unit = value;
      if (pStrSeg->sz_unit == sz_unit) return FALSE;
      break;
   case PUSH_FONT:
      font = value;
      if (pStrSeg->font == font) return FALSE;
      double_byte = IsFontDoubleByte(font);
      if (pStrSeg->double_byte != double_byte) {
         return FALSE;
      }
      break;
   case PUSH_UNDERLINE_ON:
      if (pStrSeg->underline_on == value) return FALSE;
      pStrSeg->underline_on = value;
      return TRUE;
   case PUSH_OVERLINE_ON:
      if (pStrSeg->overline_on == value) return FALSE;
      pStrSeg->overline_on = value;
      return TRUE;
   }
   if (FindFontInfo(font, style, sz_unit, TRUE) != NULL) {
      switch (which) {
      case PUSH_STYLE: pStrSeg->style = style; break;
      case PUSH_SZ_UNIT: pStrSeg->sz_unit = sz_unit; break;
      case PUSH_FONT:
         pStrSeg->font = font;
         pStrSeg->double_byte = double_byte;
         if (double_byte) {
            pStrSeg->double_byte_vertical =
                  IsDoubleByteFontVertical(font, style);
         } else {
            pStrSeg->double_byte_vertical = FALSE;
         }
         break;
      }
      changed = TRUE;
   }
   return changed;
}

static
int ChangeStrBlockFontProp(pStrBlock, which, value)
   StrBlockInfo *pStrBlock;
   int which, value;
{
   int changed=FALSE;

   switch (pStrBlock->type) {
   case SB_SIMPLE:
      return ChangeStrSegFontProp(pStrBlock->seg, which, value);
      break;

   case SB_CHAR_SPACE: break;

   case SB_SUPSUB_LEFT:
   case SB_SUPSUB_CENTER:
   case SB_SUPSUB_RIGHT:
      if (pStrBlock->sup != NULL) {
         if (ChangeMiniLinesFontProp(pStrBlock->sup, which, value)) {
            changed = TRUE;
         }
      }
      if (pStrBlock->sub != NULL) {
         if (ChangeMiniLinesFontProp(pStrBlock->sub, which, value)) {
            changed = TRUE;
         }
      }
      if (pStrBlock->type == SB_SUPSUB_CENTER) {
         /* pStrBlock->seg better not be NULL or it will crash */
         if (ChangeStrSegFontProp(pStrBlock->seg, which, value)) {
            changed = TRUE;
         }
      }
      break;
   }
   return changed;
}

static
int ChangeMiniLineFontProp(pMiniLine, which, value)
   MiniLineInfo *pMiniLine;
   int which, value;
{
   int changed=FALSE;
   StrBlockInfo *pStrBlock=NULL;

   for (pStrBlock=pMiniLine->first_block; pStrBlock != NULL;
         pStrBlock=pStrBlock->next) {
      if (ChangeStrBlockFontProp(pStrBlock, which, value)) {
         changed = TRUE;
      }
   }
   return changed;
}

int ChangeMiniLinesFontProp(minilines, which, value)
   MiniLinesInfo *minilines;
   int which, value;
{
   int changed=FALSE;
   MiniLineInfo *pMiniLine=NULL;

   for (pMiniLine=minilines->first; pMiniLine != NULL;
         pMiniLine=pMiniLine->next) {
      if (ChangeMiniLineFontProp(pMiniLine, which, value)) {
         changed = TRUE;
      }
   }
   return changed;
}

static
int ChangeTextFontProp(text_ptr, which, value)
   struct TextRec *text_ptr;
   int which, value;
{
   if (which == PUSH_UNDERLINE_Y_OFFSET) {
      if (text_ptr->underline_y_offset == value) return FALSE;

      text_ptr->underline_y_offset = value;
      return TRUE;
   }
   if (which == PUSH_OVERLINE_Y_OFFSET) {
      if (text_ptr->overline_y_offset == value) return FALSE;

      text_ptr->overline_y_offset = value;
      return TRUE;
   }
   return ChangeMiniLinesFontProp(&text_ptr->minilines, which, value);
}

int ChangeObjTextStyle(ObjPtr, StyleIndex)
   struct ObjRec *ObjPtr;
   int StyleIndex;
{
   struct ObjRec *obj_ptr;
   int changed=FALSE, obj_changed;

   switch (ObjPtr->type) {
   case OBJ_TEXT:
      if (ChangeTextFontProp(ObjPtr->detail.t, PUSH_STYLE, StyleIndex)) {
         InvalidateTextCache(ObjPtr->detail.t);
         UpdTextBBox(ObjPtr);
         AdjObjBBox(ObjPtr);
         changed = TRUE;
      }
      break;

   case OBJ_GROUP:
   case OBJ_SYM:
      obj_changed = FALSE;
      for (obj_ptr=ObjPtr->detail.r->last; obj_ptr != NULL;
            obj_ptr=obj_ptr->prev) {
         if (ChangeObjTextStyle(obj_ptr, StyleIndex)) {
            obj_changed = TRUE;
         }
      }
      if (obj_changed) {
         changed = TRUE;
         AdjObjBBox(ObjPtr);
      }
      break;
   }
   if (changePropertiesOfAttrs && ObjPtr->type != OBJ_TEXT) {
      struct AttrRec *attr_ptr=ObjPtr->fattr;

      obj_changed = FALSE;
      for ( ; attr_ptr != NULL; attr_ptr=attr_ptr->next) {
         if (ChangeObjTextStyle(attr_ptr->obj, StyleIndex)) {
            obj_changed = TRUE;
         }
      }
      if (obj_changed) {
         changed = TRUE;
         AdjObjBBox(ObjPtr);
      }
   }
   return changed;
}

void ChangeFontStyle(StyleIndex)
   int StyleIndex;
{
   struct SelRec *sel_ptr;
   int ltx, lty, rbx, rby, changed=FALSE, saved_style=curStyle;

   if (StyleIndex == INVALID) return;

   if (topSel == NULL || stickyMenuSelection) {
      int saved_sz_unit=curSzUnit;

      if (!(curChoice == DRAWTEXT && textCursorShown)) {
         TieLooseEnds();
      }
      curStyle = StyleIndex;
      if (curChoice == DRAWTEXT && textCursorShown && editTextSize != 0) {
         curSzUnit = GetCurSzUnit();
      }
      allowFontFaceSubstitution = FALSE;
      SetCanvasFont();
      allowFontFaceSubstitution = TRUE;
      if (canvasFontSize == INVALID) {
         char buf[MAXSTRING];

         GetCurFontMsg(buf, NULL);
         curStyle = saved_style;
         if (curChoice == DRAWTEXT && textCursorShown && editTextSize != 0) {
            curSzUnit = saved_sz_unit;
         }
         SetCanvasFont();
         sprintf(gszMsgBox, TgLoadString(STID_FONT_NOT_AVAILABLE), buf);
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      } else if (SzUnitToFontSize(curSzUnit) != canvasFontSize) {
         if (showFontSizeInPoints) {
            sprintf(gszMsgBox, TgLoadString(STID_CANT_CHANGE_SIZEPT_TO_USE_ALT),
                  SzUnitToPointSize(curSzUnit),
                  SzUnitToPointSize(FontSizeToSzUnit(canvasFontSize)));
         } else {
            sprintf(gszMsgBox, TgLoadString(STID_CANT_CHANGE_SIZE_TO_USE_ALT),
                  SzUnitToFontSize(curSzUnit), canvasFontSize);
         }
         curSzUnit = FontSizeToSzUnit(canvasFontSize);
         if (curChoice == DRAWTEXT && textCursorShown && editTextSize != 0) {
            PutCurSzUnit(curSzUnit);
         }
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      } else if (curChoice == DRAWTEXT && textCursorShown &&
            editTextSize != 0) {
         PutCurSzUnit(curSzUnit);
      }
      if (curChoice == DRAWTEXT && textCursorShown) {
         if (ChangeEditTextProperty(PROP_MASK_TEXT_STYLE, StyleIndex)) {
            curTextModified = TRUE;
            UpdCurTextBBox();
            RedrawCurText();
            SetFileModified(TRUE);
            if (cycleThroughChoice) {
               SetPushedFontValue(PUSH_STYLE, curStyle);
            }
         }
      } else {
         textCursorShown = FALSE;
      }
      ShowCurFont();
      ShowTextSize();
      UpdatePinnedMenu(MENU_STYLE);
      if (topSel == NULL) return;
   }
   ltx = selLtX; lty = selLtY; rbx = selRbX; rby = selRbY;

   HighLightReverse();
   StartCompositeCmd();
   for (sel_ptr=botSel; sel_ptr != NULL; sel_ptr=sel_ptr->prev) {
      PrepareToReplaceAnObj(sel_ptr->obj);
      if (ChangeObjTextStyle(sel_ptr->obj, StyleIndex)) {
         changed = TRUE;
         RecordReplaceAnObj(sel_ptr->obj);
      } else {
         AbortPrepareCmd(CMD_REPLACE);
      }
   }
   EndCompositeCmd();

   if (changed) {
      UpdSelBBox();
      RedrawAreas(botObj, ltx-GRID_ABS_SIZE(1), lty-GRID_ABS_SIZE(1),
            rbx+GRID_ABS_SIZE(1), rby+GRID_ABS_SIZE(1), selLtX-GRID_ABS_SIZE(1),
            selLtY-GRID_ABS_SIZE(1), selRbX+GRID_ABS_SIZE(1),
            selRbY+GRID_ABS_SIZE(1));
      SetFileModified(TRUE);
   }
   HighLightForward();
}

int ChangeObjTextJust(ObjPtr, JustIndex)
   struct ObjRec *ObjPtr;
   int JustIndex;
{
   struct ObjRec *obj_ptr;
   int changed=FALSE, obj_changed;

   switch (ObjPtr->type) {
   case OBJ_TEXT:
      if (ObjPtr->detail.t->minilines.just != JustIndex) {
         ObjPtr->detail.t->minilines.just = JustIndex;
         UpdTextBBox(ObjPtr);

         if (ObjPtr->detail.t->cached_bitmap != None) {
            XFreePixmap(mainDisplay,ObjPtr->detail.t->cached_bitmap);
         }
         ObjPtr->detail.t->cached_zoom = 0;
         ObjPtr->detail.t->cached_bitmap = None;

         AdjObjSplineVs(ObjPtr);
         changed = TRUE;
      }
      break;

   case OBJ_GROUP:
   case OBJ_SYM:
      obj_changed = FALSE;
      for (obj_ptr=ObjPtr->detail.r->last; obj_ptr != NULL;
            obj_ptr=obj_ptr->prev) {
         if (ChangeObjTextJust(obj_ptr, JustIndex)) {
            obj_changed = TRUE;
         }
      }
      if (obj_changed) {
         changed = TRUE;
         AdjObjBBox(ObjPtr);
      }
      break;
   }
   if (changePropertiesOfAttrs && ObjPtr->type != OBJ_TEXT) {
      struct AttrRec *attr_ptr=ObjPtr->fattr;

      obj_changed = FALSE;
      for ( ; attr_ptr != NULL; attr_ptr=attr_ptr->next) {
         if (ChangeObjTextJust(attr_ptr->obj, JustIndex)) {
            obj_changed = TRUE;
         }
      }
      if (obj_changed) {
         changed = TRUE;
         AdjObjBBox(ObjPtr);
      }
   }
   return changed;
}

void ChangeFontJust(JustIndex)
   int JustIndex;
{
   struct SelRec *sel_ptr;
   int ltx, lty, rbx, rby, changed=FALSE;

   if (JustIndex == INVALID) return;

   if (topSel == NULL || stickyMenuSelection) {
      if (!(curChoice == DRAWTEXT && textCursorShown)) {
         TieLooseEnds();
      }
      textJust = JustIndex;
      ShowJust();
      UpdatePinnedMenu(MENU_STYLE);
      if (curChoice == DRAWTEXT && textCursorShown) {
         if (ChangeObjTextJust(curTextObj, JustIndex)) {
            curTextModified = TRUE;
            UpdCurTextBBox();
            SetFileModified(TRUE);
            ClearAndRedrawDrawWindow();
            if (cycleThroughChoice) {
               SetPushedFontValue(PUSH_JUST, textJust);
            }
         }
      } else {
         textCursorShown = FALSE;
      }
      if (topSel == NULL) return;
   }
   ltx = selLtX; lty = selLtY; rbx = selRbX; rby = selRbY;

   HighLightReverse();
   StartCompositeCmd();
   for (sel_ptr=botSel; sel_ptr != NULL; sel_ptr=sel_ptr->prev) {
      PrepareToReplaceAnObj(sel_ptr->obj);
      if (ChangeObjTextJust(sel_ptr->obj, JustIndex)) {
         changed = TRUE;
         RecordReplaceAnObj(sel_ptr->obj);
      } else {
         AbortPrepareCmd(CMD_REPLACE);
      }
   }
   EndCompositeCmd();

   if (changed) {
      UpdSelBBox();
      RedrawAreas(botObj, ltx-GRID_ABS_SIZE(1), lty-GRID_ABS_SIZE(1),
            rbx+GRID_ABS_SIZE(1), rby+GRID_ABS_SIZE(1), selLtX-GRID_ABS_SIZE(1),
            selLtY-GRID_ABS_SIZE(1), selRbX+GRID_ABS_SIZE(1),
            selRbY+GRID_ABS_SIZE(1));
      SetFileModified(TRUE);
   }
   HighLightForward();
}

int ChangeObjTextUnderline(ObjPtr, UnderlineIndex)
   struct ObjRec *ObjPtr;
   int UnderlineIndex;
{
   struct ObjRec *obj_ptr;
   int changed=FALSE, obj_changed=FALSE;

   switch (ObjPtr->type) {
   case OBJ_TEXT:
      if (ChangeTextFontProp(ObjPtr->detail.t, PUSH_UNDERLINE_ON,
            UnderlineIndex)) {
         InvalidateTextCache(ObjPtr->detail.t);
         UpdTextBBox(ObjPtr);
         AdjObjBBox(ObjPtr);
         changed = TRUE;
      }
      break;

   case OBJ_GROUP:
   case OBJ_SYM:
      obj_changed = FALSE;
      for (obj_ptr=ObjPtr->detail.r->last; obj_ptr != NULL;
            obj_ptr=obj_ptr->prev) {
         if (ChangeObjTextUnderline(obj_ptr, UnderlineIndex)) {
            obj_changed = TRUE;
         }
      }
      if (obj_changed) {
         changed = TRUE;
         AdjObjBBox(ObjPtr);
      }
      break;
   }
   if (changePropertiesOfAttrs && ObjPtr->type != OBJ_TEXT) {
      struct AttrRec *attr_ptr=ObjPtr->fattr;

      obj_changed = FALSE;
      for ( ; attr_ptr != NULL; attr_ptr=attr_ptr->next) {
         if (ChangeObjTextUnderline(attr_ptr->obj, UnderlineIndex)) {
            obj_changed = TRUE;
         }
      }
      if (obj_changed) {
         changed = TRUE;
         AdjObjBBox(ObjPtr);
      }
   }
   return changed;
}

void ChangeFontUnderline(UnderlineIndex)
   int UnderlineIndex;
{
   struct SelRec *sel_ptr;
   int ltx, lty, rbx, rby, changed=FALSE;

   if (UnderlineIndex == INVALID) return;

   if (topSel == NULL || stickyMenuSelection) {
      if (!(curChoice == DRAWTEXT && textCursorShown)) {
         TieLooseEnds();
      }
      curUnderlineOn = UnderlineIndex;
      if (curChoice == DRAWTEXT && textCursorShown) {
         if (ChangeEditTextProperty(PROP_MASK_UNDERLINE_ON,
               UnderlineIndex)) {
            curTextModified = TRUE;
            UpdCurTextBBox();
            RedrawCurText();
            SetFileModified(TRUE);
            if (cycleThroughChoice) {
               SetPushedFontValue(PUSH_UNDERLINE_ON, curUnderlineOn);
            }
         }
      } else {
         textCursorShown = FALSE;
      }
      ShowCurFont();
      UpdatePinnedMenu(MENU_STYLE);
      if (topSel == NULL) return;
   }
   ltx = selLtX; lty = selLtY; rbx = selRbX; rby = selRbY;

   HighLightReverse();
   StartCompositeCmd();
   for (sel_ptr=botSel; sel_ptr != NULL; sel_ptr=sel_ptr->prev) {
      PrepareToReplaceAnObj(sel_ptr->obj);
      if (ChangeObjTextUnderline(sel_ptr->obj, UnderlineIndex)) {
         changed = TRUE;
         RecordReplaceAnObj(sel_ptr->obj);
      } else {
         AbortPrepareCmd(CMD_REPLACE);
      }
   }
   EndCompositeCmd();

   if (changed) {
      UpdSelBBox();
      RedrawAreas(botObj, ltx-GRID_ABS_SIZE(1), lty-GRID_ABS_SIZE(1),
            rbx+GRID_ABS_SIZE(1), rby+GRID_ABS_SIZE(1), selLtX-GRID_ABS_SIZE(1),
            selLtY-GRID_ABS_SIZE(1), selRbX+GRID_ABS_SIZE(1),
            selRbY+GRID_ABS_SIZE(1));
      SetFileModified(TRUE);
   }
   HighLightForward();
}

int ChangeObjTextOverline(ObjPtr, OverlineIndex)
   struct ObjRec *ObjPtr;
   int OverlineIndex;
{
   struct ObjRec *obj_ptr;
   int changed=FALSE, obj_changed=FALSE;

   switch (ObjPtr->type) {
   case OBJ_TEXT:
      if (ChangeTextFontProp(ObjPtr->detail.t, PUSH_OVERLINE_ON,
            OverlineIndex)) {
         InvalidateTextCache(ObjPtr->detail.t);
         UpdTextBBox(ObjPtr);
         AdjObjBBox(ObjPtr);
         changed = TRUE;
      }
      break;

   case OBJ_GROUP:
   case OBJ_SYM:
      obj_changed = FALSE;
      for (obj_ptr=ObjPtr->detail.r->last; obj_ptr != NULL;
            obj_ptr=obj_ptr->prev) {
         if (ChangeObjTextOverline(obj_ptr, OverlineIndex)) {
            obj_changed = TRUE;
         }
      }
      if (obj_changed) {
         changed = TRUE;
         AdjObjBBox(ObjPtr);
      }
      break;
   }
   if (changePropertiesOfAttrs && ObjPtr->type != OBJ_TEXT) {
      struct AttrRec *attr_ptr=ObjPtr->fattr;

      obj_changed = FALSE;
      for ( ; attr_ptr != NULL; attr_ptr=attr_ptr->next) {
         if (ChangeObjTextOverline(attr_ptr->obj, OverlineIndex)) {
            obj_changed = TRUE;
         }
      }
      if (obj_changed) {
         changed = TRUE;
         AdjObjBBox(ObjPtr);
      }
   }
   return changed;
}

void ChangeFontOverline(OverlineIndex)
   int OverlineIndex;
{
   struct SelRec *sel_ptr;
   int ltx, lty, rbx, rby, changed=FALSE;

   if (OverlineIndex == INVALID) return;

   if (topSel == NULL || stickyMenuSelection) {
      if (!(curChoice == DRAWTEXT && textCursorShown)) {
         TieLooseEnds();
      }
      curOverlineOn = OverlineIndex;
      if (curChoice == DRAWTEXT && textCursorShown) {
         if (ChangeEditTextProperty(PROP_MASK_OVERLINE_ON,
               OverlineIndex)) {
            curTextModified = TRUE;
            UpdCurTextBBox();
            RedrawCurText();
            SetFileModified(TRUE);
            if (cycleThroughChoice) {
               SetPushedFontValue(PUSH_OVERLINE_ON, curOverlineOn);
            }
         }
      } else {
         textCursorShown = FALSE;
      }
      ShowCurFont();
      UpdatePinnedMenu(MENU_STYLE);
      if (topSel == NULL) return;
   }
   ltx = selLtX; lty = selLtY; rbx = selRbX; rby = selRbY;

   HighLightReverse();
   StartCompositeCmd();
   for (sel_ptr=botSel; sel_ptr != NULL; sel_ptr=sel_ptr->prev) {
      PrepareToReplaceAnObj(sel_ptr->obj);
      if (ChangeObjTextOverline(sel_ptr->obj, OverlineIndex)) {
         changed = TRUE;
         RecordReplaceAnObj(sel_ptr->obj);
      } else {
         AbortPrepareCmd(CMD_REPLACE);
      }
   }
   EndCompositeCmd();

   if (changed) {
      UpdSelBBox();
      RedrawAreas(botObj, ltx-GRID_ABS_SIZE(1), lty-GRID_ABS_SIZE(1),
            rbx+GRID_ABS_SIZE(1), rby+GRID_ABS_SIZE(1), selLtX-GRID_ABS_SIZE(1),
            selLtY-GRID_ABS_SIZE(1), selRbX+GRID_ABS_SIZE(1),
            selRbY+GRID_ABS_SIZE(1));
      SetFileModified(TRUE);
   }
   HighLightForward();
}

int RefreshFontStyleMenu(menu)
   TgMenu *menu;
{
   int i, rc=TRUE, num_items=menu->num_items;
   TgMenuItem *menuitems=menu->menuitems;

   for (i=0; i < num_items; i++) {
      TgMenuItem *menu_item=(&menuitems[i]);
      TgMenuItem stMenuItem;

      memset(&stMenuItem, 0, sizeof(TgMenuItem));
      stMenuItem.state = TGBS_NORMAL;
      if (i < MAXFONTSTYLES) {
         stMenuItem.checked = (i == curStyle);
         if (!TgSetMenuItemInfo(menu_item, TGMU_MASK_STATE|TGMU_MASK_RADIO,
               &stMenuItem)) {
            rc = FALSE;
         }
      } else if (i > MAXFONTSTYLES && i < MAXFONTSTYLES+MAXJUSTS+1) {
         stMenuItem.checked = (i-MAXFONTSTYLES-1 == textJust);
         if (!TgSetMenuItemInfo(menu_item, TGMU_MASK_STATE|TGMU_MASK_RADIO,
               &stMenuItem)) {
            rc = FALSE;
         }
      } else if (i > MAXFONTSTYLES+MAXJUSTS+1 &&
            i < MAXFONTSTYLES+MAXJUSTS+4) {
         stMenuItem.checked = (i-MAXFONTSTYLES-MAXJUSTS-2 == curUnderlineOn);
         if (!TgSetMenuItemInfo(menu_item, TGMU_MASK_STATE|TGMU_MASK_RADIO,
               &stMenuItem)) {
            rc = FALSE;
         }
      } else if (i > MAXFONTSTYLES+MAXJUSTS+4) {
         stMenuItem.checked = (i-MAXFONTSTYLES-MAXJUSTS-5 == curOverlineOn);
         if (!TgSetMenuItemInfo(menu_item, TGMU_MASK_STATE|TGMU_MASK_RADIO,
               &stMenuItem)) {
            rc = FALSE;
         }
      }
   }
   return rc;
}

TgMenu *CreateFontStyleMenu(parent_menu, x, y, menu_info, status_str_xlated)
   TgMenu *parent_menu;
   int x, y;
   TgMenuInfo *menu_info;
   int status_str_xlated; /* ignored, always 0 */
{
   TgMenu *menu=TgCreateMenuFromMenuInfo(parent_menu, x, y, menu_info, FALSE);

   if (menu != NULL) {
      menu->track_menubar = TRUE;
      if (!RefreshFontStyleMenu(menu)) {
         return TgDestroyMenu(menu, TRUE);
      }
      menu->refresh_proc = ((RefreshMenuFunc*)RefreshFontStyleMenu);
   }
   return menu;
}

int StyleMenu(X, Y, TrackMenubar)
   int X, Y, TrackMenubar;
{
   int rc=INVALID;
   TgMenu *menu=(fontStyleMenuInfo.create_proc)(NULL, X, Y, &fontStyleMenuInfo,
         INVALID);

   activeMenu = MENU_STYLE;
   if (menu != NULL) {
      menu->track_menubar = TrackMenubar;

      rc = TgMenuLoop(menu);
      TgDestroyMenu(menu, TRUE);
   }
   return rc;
}

int GetSizeMenuIndex()
{
   int i=0, sz_unit=GetCurSzUnit();

   for (i=0; i < numFontSizes; i++) {
      if (fontSzUnits[i] == sz_unit) {
         return i;
      }
   }
   return INVALID;
}

int ChangeObjTextSzUnit(ObjPtr, SzUnit)
   struct ObjRec *ObjPtr;
   int SzUnit;
{
   struct ObjRec *obj_ptr;
   int changed=FALSE, obj_changed;

   switch (ObjPtr->type) {
   case OBJ_TEXT:
      if (ChangeTextFontProp(ObjPtr->detail.t, PUSH_SZ_UNIT, SzUnit)) {
         InvalidateTextCache(ObjPtr->detail.t);
         UpdTextBBox(ObjPtr);
         AdjObjBBox(ObjPtr);
         changed = TRUE;
      }
      break;

   case OBJ_GROUP:
   case OBJ_SYM:
      obj_changed = FALSE;
      for (obj_ptr=ObjPtr->detail.r->last; obj_ptr != NULL;
            obj_ptr=obj_ptr->prev) {
         if (ChangeObjTextSzUnit(obj_ptr, SzUnit)) {
            obj_changed = TRUE;
         }
      }
      if (obj_changed) {
         changed = TRUE;
         AdjObjBBox(ObjPtr);
      }
      break;
   }
   if (changePropertiesOfAttrs && ObjPtr->type != OBJ_TEXT) {
      struct AttrRec *attr_ptr=ObjPtr->fattr;

      obj_changed = FALSE;
      for ( ; attr_ptr != NULL; attr_ptr=attr_ptr->next) {
         if (ChangeObjTextSzUnit(attr_ptr->obj, SzUnit)) {
            obj_changed = TRUE;
         }
      }
      if (obj_changed) {
         changed = TRUE;
         AdjObjBBox(ObjPtr);
      }
   }
   return changed;
}

void ChangeFontSize(SizeIndex)
   int SizeIndex;
{
   struct SelRec *sel_ptr;
   int ltx, lty, rbx, rby;
   int changed=FALSE;
   int saved_sz_unit=curSzUnit;

   if (SizeIndex == INVALID) return;

   if (topSel == NULL || stickyMenuSelection) {
      if (!(curChoice == DRAWTEXT && textCursorShown)) {
         TieLooseEnds();
      }
      curSzUnit = fontSzUnits[SizeIndex];
      changingFontSizeFromRead = FALSE;
      allowFontFaceSubstitution = FALSE;
      SetCanvasFont();
      allowFontFaceSubstitution = TRUE;
      changingFontSizeFromRead = TRUE;
      if (canvasFontSize == INVALID) {
         char buf[MAXSTRING];

         GetCurFontMsg(buf, NULL);
         curSzUnit = saved_sz_unit;
         SizeIndex = GetSizeMenuIndex();
         SetCanvasFont();
         sprintf(gszMsgBox, TgLoadString(STID_FONT_NOT_AVAILABLE), buf);
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      } else if (SzUnitToFontSize(curSzUnit) != canvasFontSize) {
         if (showFontSizeInPoints) {
            sprintf(gszMsgBox, TgLoadString(STID_CANT_CHANGE_SIZEPT_TO_USE_ALT),
                  SzUnitToPointSize(curSzUnit),
                  SzUnitToPointSize(FontSizeToSzUnit(canvasFontSize)));
         } else {
            sprintf(gszMsgBox, TgLoadString(STID_CANT_CHANGE_SIZE_TO_USE_ALT),
                  SzUnitToFontSize(curSzUnit), canvasFontSize);
         }        
         curSzUnit = FontSizeToSzUnit(canvasFontSize);
         SizeIndex = GetSizeMenuIndex();
         if (curChoice == DRAWTEXT && textCursorShown && editTextSize != 0) {
            PutCurSzUnit(curSzUnit);
         }
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      } else if (curChoice == DRAWTEXT && textCursorShown &&
            editTextSize != 0) {
         PutCurSzUnit(curSzUnit);
      }
      if (curChoice == DRAWTEXT && textCursorShown) {
         if (ChangeEditTextProperty(PROP_MASK_TEXT_SZ_UNIT,
               fontSzUnits[SizeIndex])) {
            curTextModified = TRUE;
            UpdCurTextBBox();
            RedrawCurText();
            SetFileModified(TRUE);
            if (cycleThroughChoice) {
               SetPushedFontValue(PUSH_SZ_UNIT, GetCurSzUnit());
            }
         }
      } else {
         textCursorShown = FALSE;
      }
      ShowCurFont();
      ShowTextSize();
      UpdatePinnedMenu(MENU_SIZE);
      if (topSel == NULL) return;
   }
   ltx = selLtX; lty = selLtY; rbx = selRbX; rby = selRbY;

   HighLightReverse();
   StartCompositeCmd();
   for (sel_ptr=botSel; sel_ptr != NULL; sel_ptr=sel_ptr->prev) {
      PrepareToReplaceAnObj(sel_ptr->obj);
      if (ChangeObjTextSzUnit(sel_ptr->obj, fontSzUnits[SizeIndex])) {
         changed = TRUE;
         RecordReplaceAnObj(sel_ptr->obj);
      } else {
         AbortPrepareCmd(CMD_REPLACE);
      }
   }
   EndCompositeCmd();

   if (changed) {
      UpdSelBBox();
      RedrawAreas(botObj, ltx-GRID_ABS_SIZE(1), lty-GRID_ABS_SIZE(1),
            rbx+GRID_ABS_SIZE(1), rby+GRID_ABS_SIZE(1), selLtX-GRID_ABS_SIZE(1),
            selLtY-GRID_ABS_SIZE(1), selRbX+GRID_ABS_SIZE(1),
            selRbY+GRID_ABS_SIZE(1));
      SetFileModified(TRUE);
   }
   HighLightForward();
}

int RefreshFontSizeMenu(menu)
   TgMenu *menu;
{
   int i, num_items=menu->num_items, sz_unit=GetCurSzUnit();
   TgMenuItem *menuitems=menu->menuitems;

   for (i=0; i < num_items; i++) {
      TgMenuItem *menu_item=(&menuitems[i]);
      TgMenuItem stMenuItem;

      memset(&stMenuItem, 0, sizeof(TgMenuItem));
      stMenuItem.state = TGBS_NORMAL;
      stMenuItem.checked = (fontSzUnits[i] == sz_unit);
      if (!TgSetMenuItemInfo(menu_item, TGMU_MASK_STATE|TGMU_MASK_RADIO,
            &stMenuItem)) {
         return FALSE;
      }
   }
   return TRUE;
}

TgMenu *CreateFontSizeMenu(parent_menu, x, y, menu_info, status_str_xlated)
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
         (numFontSizes+1)*sizeof(TgMenuItemInfo));
   if (stMenuInfo.items == NULL) FailAllocMessage();
   memset(stMenuInfo.items, 0, (numFontSizes+1)*sizeof(TgMenuItemInfo));
   for (item_info=stMenuInfo.items, i=0; i < numFontSizes; item_info++, i++) {
      item_info->menu_str = UtilStrDup(sizeMenuStr[i]);
      if (item_info->menu_str == NULL) FailAllocMessage();
      item_info->shortcut_str = NULL;
      if (showFontSizeInPoints) {
         sprintf(gszMsgBox, TgLoadCachedString(CSTID_SET_FONT_SIZE_TO_PT),
               SzUnitToPointSize(fontSzUnits[i]));
      } else {
         sprintf(gszMsgBox, TgLoadCachedString(CSTID_SET_FONT_SIZE_TO),
               SzUnitToFontSize(fontSzUnits[i]));
      }
      item_info->status_str = UtilStrDup(gszMsgBox);
      if (item_info->status_str == NULL) FailAllocMessage();
      item_info->submenu_info = NULL;
      item_info->cmdid = CMDID_CHANGEALLSELFONTSIZE;
   }
   stMenuInfo.items[numFontSizes].cmdid = INVALID;
   stMenuInfo.type |= TGMUTYPE_CANSCROLL;

   /* the status_str has been translated in TgLoadCachedString() */
   menu = TgCreateMenuFromMenuInfo(parent_menu, x, y, &stMenuInfo, TRUE);
   for (item_info=stMenuInfo.items, i=0; i < numFontSizes; item_info++, i++) {
      UtilFree(item_info->status_str);
   }
   memset(stMenuInfo.items, 0, (numFontSizes+1)*sizeof(TgMenuItemInfo));
   free(stMenuInfo.items);
   stMenuInfo.items = NULL;
   if (menu != NULL) {
      for (i=0; i < numFontSizes; i++) {
         menu->menuitems[i].menu_str_allocated = TRUE;
      }
      if (!RefreshFontSizeMenu(menu)) {
         return TgDestroyMenu(menu, TRUE);
      }
      for (i=0; i < numFontSizes; i++) {
         if (TgIsMenuItemRadio(menu, i)) {
            SetScrollableMenuFirstIndex(menu, i);
            break;
         }
      }
      menu->refresh_proc = ((RefreshMenuFunc*)RefreshFontSizeMenu);
   }
   return menu;
}

int SizeMenu(X, Y, TrackMenubar)
   int X, Y, TrackMenubar;
{
   int rc=INVALID;
   TgMenu *menu=(fontSizeMenuInfo.create_proc)(NULL, X, Y, &fontSizeMenuInfo,
         INVALID);

   activeMenu = MENU_SIZE;
   if (menu != NULL) {
      menu->track_menubar = TrackMenubar;

      rc = TgMenuLoop(menu);
      TgDestroyMenu(menu, TRUE);
   }
   return rc;
}

int ChangeObjTextFont(ObjPtr, FontIndex)
   struct ObjRec *ObjPtr;
   int FontIndex;
{
   struct ObjRec *obj_ptr;
   int changed=FALSE, obj_changed=FALSE;

   switch (ObjPtr->type) {
   case OBJ_TEXT:
      if (ChangeTextFontProp(ObjPtr->detail.t, PUSH_FONT, FontIndex)) {
         InvalidateTextCache(ObjPtr->detail.t);
         UpdTextBBox(ObjPtr);
         AdjObjBBox(ObjPtr);
         changed = TRUE;
      }
      break;

   case OBJ_GROUP:
   case OBJ_SYM:
      obj_changed = FALSE;
      for (obj_ptr=ObjPtr->detail.r->last; obj_ptr != NULL;
            obj_ptr=obj_ptr->prev) {
         if (ChangeObjTextFont(obj_ptr, FontIndex)) {
            obj_changed = TRUE;
         }
      }
      if (obj_changed) {
         changed = TRUE;
         AdjObjBBox(ObjPtr);
      }
      break;
   }
   if (changePropertiesOfAttrs && ObjPtr->type != OBJ_TEXT) {
      struct AttrRec *attr_ptr=ObjPtr->fattr;

      obj_changed = FALSE;
      for ( ; attr_ptr != NULL; attr_ptr=attr_ptr->next) {
         if (ChangeObjTextFont(attr_ptr->obj, FontIndex)) {
            obj_changed = TRUE;
         }
      }
      if (obj_changed) {
         changed = TRUE;
         AdjObjBBox(ObjPtr);
      }
   }
   return changed;
}

int ChangeFont(FontIndex, ForceNewChoice)
   int FontIndex, ForceNewChoice;
{
   struct SelRec *sel_ptr=NULL;
   int ltx, lty, rbx, rby, changed=FALSE, saved_font=curFont, rc=TRUE;

   if (FontIndex == INVALID) return FALSE;

   if (topSel == NULL || stickyMenuSelection || ForceNewChoice) {
      int saved_sz_unit=curSzUnit;

      if (!(curChoice == DRAWTEXT && textCursorShown)) {
         TieLooseEnds();
      }
      curFont = FontIndex;
      if (curChoice == DRAWTEXT && textCursorShown && editTextSize != 0) {
         curSzUnit = GetCurSzUnit();
      }
      allowFontFaceSubstitution = FALSE;
      SetCanvasFont();
      allowFontFaceSubstitution = TRUE;
      if (canvasFontSize == INVALID) {
         char buf[MAXSTRING];

         GetCurFontMsg(buf, NULL);
         curFont = FontIndex = saved_font;
         if (curChoice == DRAWTEXT && textCursorShown && editTextSize != 0) {
            curSzUnit = saved_sz_unit;
         }
         SetCanvasFont();
         sprintf(gszMsgBox, TgLoadString(STID_FONT_NOT_AVAILABLE), buf);
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
         rc = FALSE;
      } else if (SzUnitToFontSize(curSzUnit) != canvasFontSize) {
         if (showFontSizeInPoints) {
            sprintf(gszMsgBox, TgLoadString(STID_CANT_CHANGE_SIZEPT_TO_USE_ALT),
                  SzUnitToPointSize(curSzUnit),
                  SzUnitToPointSize(FontSizeToSzUnit(canvasFontSize)));
         } else {
            sprintf(gszMsgBox, TgLoadString(STID_CANT_CHANGE_SIZE_TO_USE_ALT),
                  SzUnitToFontSize(curSzUnit), canvasFontSize);
         }
         curSzUnit = FontSizeToSzUnit(canvasFontSize);
         if (curChoice == DRAWTEXT && textCursorShown && editTextSize != 0) {
            PutCurSzUnit(curSzUnit);
         }
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      } else if (curChoice == DRAWTEXT && textCursorShown &&
            editTextSize != 0) {
         PutCurSzUnit(curSzUnit);
      }
      if (curChoice == DRAWTEXT && textCursorShown) {
         if (ChangeEditTextProperty(PROP_MASK_TEXT_FONT, FontIndex)) {
            curTextModified = TRUE;
            UpdCurTextBBox();
            RedrawCurText();
            SetFileModified(TRUE);
            if (cycleThroughChoice) {
               SetPushedFontValue(PUSH_FONT, curFont);
            }
         }
      } else {
         textCursorShown = FALSE;
      }
      ShowCurFont();
      ShowTextSize();
      UpdatePinnedMenu(MENU_FONT);
      if (topSel == NULL) return rc;
   }
   ltx = selLtX; lty = selLtY; rbx = selRbX; rby = selRbY;

   HighLightReverse();
   StartCompositeCmd();
   for (sel_ptr=botSel; sel_ptr != NULL; sel_ptr=sel_ptr->prev) {
      PrepareToReplaceAnObj(sel_ptr->obj);
      if (ChangeObjTextFont(sel_ptr->obj, FontIndex)) {
         changed = TRUE;
         RecordReplaceAnObj(sel_ptr->obj);
      } else {
         AbortPrepareCmd(CMD_REPLACE);
      }
   }
   EndCompositeCmd();

   if (changed) {
      UpdSelBBox();
      RedrawAreas(botObj, ltx-GRID_ABS_SIZE(1), lty-GRID_ABS_SIZE(1),
            rbx+GRID_ABS_SIZE(1), rby+GRID_ABS_SIZE(1), selLtX-GRID_ABS_SIZE(1),
            selLtY-GRID_ABS_SIZE(1), selRbX+GRID_ABS_SIZE(1),
            selRbY+GRID_ABS_SIZE(1));
      SetFileModified(TRUE);
   }
   HighLightForward();

   return rc;
}

int RefreshFontMenu(menu)
   TgMenu *menu;
{
   int i, num_items=menu->num_items;
   TgMenuItem *menuitems=menu->menuitems;

   for (i=0; i < num_items; i++) {
      TgMenuItem *menu_item=(&menuitems[i]);
      TgMenuItem stMenuItem;

      memset(&stMenuItem, 0, sizeof(TgMenuItem));
      stMenuItem.state = TGBS_NORMAL;
      stMenuItem.checked = (i == curFont);
      TgSetMenuItemInfo(menu_item, TGMU_MASK_STATE|TGMU_MASK_RADIO,
            &stMenuItem);
   }
   return TRUE;
}

TgMenu *CreateFontMenu(parent_menu, x, y, menu_info, status_str_xlated)
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
         (numFonts+1)*sizeof(TgMenuItemInfo));
   if (stMenuInfo.items == NULL) FailAllocMessage();
   memset(stMenuInfo.items, 0, (numFonts+1)*sizeof(TgMenuItemInfo));
   for (item_info=stMenuInfo.items, i=0; i < numFonts; item_info++, i++) {
      item_info->menu_str = UtilStrDup(fontMenuStr[i]);
      if (item_info->menu_str == NULL) FailAllocMessage();
      item_info->shortcut_str = NULL;
      sprintf(gszMsgBox, TgLoadCachedString(CSTID_SET_FONT_TO), fontMenuStr[i]);
      item_info->status_str = UtilStrDup(gszMsgBox);
      if (item_info->status_str == NULL) FailAllocMessage();
      item_info->submenu_info = NULL;
      item_info->cmdid = CMDID_CHANGEALLSELFONT;
   }
   stMenuInfo.items[numFonts].cmdid = INVALID;
   stMenuInfo.type |= TGMUTYPE_CANSCROLL;

   /* the status_str has been translated in TgLoadCachedString() */
   menu = TgCreateMenuFromMenuInfo(parent_menu, x, y, &stMenuInfo, TRUE);
   for (item_info=stMenuInfo.items, i=0; i < numFonts; item_info++, i++) {
      UtilFree(item_info->status_str);
   }
   memset(stMenuInfo.items, 0, (numFonts+1)*sizeof(TgMenuItemInfo));
   free(stMenuInfo.items);
   stMenuInfo.items = NULL;
   if (menu != NULL) {
      TgMenuItem *menu_item=NULL;
      TgMenuItem stMenuItem;

      menu->track_menubar = TRUE;
      menu_item = (&menu->menuitems[curFont]);

      memset(&stMenuItem, 0, sizeof(TgMenuItem));
      stMenuItem.checked = TRUE;
      if (!TgSetMenuItemInfo(menu_item, TGMU_MASK_RADIO, &stMenuItem)) {
         return TgDestroyMenu(menu, TRUE);
      }
      for (i=0; i < numFonts; i++) {
         menu->menuitems[i].menu_str_allocated = TRUE;
      }
      SetScrollableMenuFirstIndex(menu, curFont);
      menu->refresh_proc = ((RefreshMenuFunc*)RefreshFontMenu);
   }
   return menu;
}

int FontMenu(X, Y, TrackMenubar)
   int X, Y, TrackMenubar;
{
   int rc=INVALID;
   TgMenu *menu=(fontMenuInfo.create_proc)(NULL, X, Y, &fontMenuInfo, INVALID);

   activeMenu = MENU_FONT;
   if (menu != NULL) {
      menu->track_menubar = TrackMenubar;

      rc = TgMenuLoop(menu);
      TgDestroyMenu(menu, TRUE);
   }
   return rc;
}

int ChangeObjVSpace(ObjPtr, VSpace)
   struct ObjRec *ObjPtr;
   int VSpace;
{
   struct ObjRec *obj_ptr;
   int changed=FALSE, obj_changed;
   int saved_v_space;

   switch (ObjPtr->type) {
   case OBJ_TEXT:
      if (ObjPtr->detail.t->minilines.v_space != VSpace) {
         saved_v_space = ObjPtr->detail.t->minilines.v_space;
         ObjPtr->detail.t->minilines.v_space = VSpace;
         if (AdjTransformedTextBBox(ObjPtr) == FALSE) {
            Msg(TgLoadString(STID_INVALID_VSPACE_NOT_CHANGED));
            ObjPtr->detail.t->minilines.v_space = saved_v_space;
            UpdTextBBox(ObjPtr);
         } else {
            if (ObjPtr->detail.t->cached_bitmap != None) {
               XFreePixmap(mainDisplay,ObjPtr->detail.t->cached_bitmap);
            }
            ObjPtr->detail.t->cached_zoom = 0;
            ObjPtr->detail.t->cached_bitmap = None;

            AdjObjBBox(ObjPtr);
            changed = TRUE;
         }
      }
      break;

   case OBJ_GROUP:
   case OBJ_SYM:
      obj_changed = FALSE;
      for (obj_ptr=ObjPtr->detail.r->last; obj_ptr != NULL;
            obj_ptr=obj_ptr->prev) {
         if (ChangeObjVSpace(obj_ptr, VSpace)) {
            obj_changed = TRUE;
         }
      }
      if (obj_changed) {
         changed = TRUE;
         AdjObjBBox(ObjPtr);
      }
      break;
   }
   if (changePropertiesOfAttrs && ObjPtr->type != OBJ_TEXT) {
      struct AttrRec *attr_ptr=ObjPtr->fattr;

      obj_changed = FALSE;
      for ( ; attr_ptr != NULL; attr_ptr=attr_ptr->next) {
         if (ChangeObjVSpace(attr_ptr->obj, VSpace)) {
            obj_changed = TRUE;
         }
      }
      if (obj_changed) {
         changed = TRUE;
         AdjObjBBox(ObjPtr);
      }
   }
   return changed;
}

void ChangeVSpace(VSpace)
   int VSpace;
{
   struct SelRec *sel_ptr;
   int ltx, lty, rbx, rby, changed=FALSE;

   if (topSel == NULL || stickyMenuSelection) {
      if (!(curChoice == DRAWTEXT && textCursorShown)) {
         TieLooseEnds();
      }
      if (textCursorH+textVSpace <= 0) {
         Msg(TgLoadString(STID_VSPACE_TOO_SMALL_NOT_CHANGED));
      } else {
         textVSpace = VSpace;
         SetCanvasFont();
         ShowTextVSpace();
      }
      if (curChoice == DRAWTEXT && textCursorShown) {
         if (ChangeObjVSpace(curTextObj, VSpace)) {
            curTextModified = TRUE;
            UpdCurTextBBox();
            RedrawCurText();
            SetFileModified(TRUE);
            if (cycleThroughChoice) {
               SetPushedFontValue(PUSH_VSPACE, textVSpace);
            }
         }
      } else {
         textCursorShown = FALSE;
      }
      if (topSel == NULL) return;
   }
   ltx = selLtX; lty = selLtY; rbx = selRbX; rby = selRbY;

   HighLightReverse();
   StartCompositeCmd();
   for (sel_ptr=botSel; sel_ptr != NULL; sel_ptr=sel_ptr->prev) {
      PrepareToReplaceAnObj(sel_ptr->obj);
      if (ChangeObjVSpace(sel_ptr->obj, VSpace)) {
         changed = TRUE;
         RecordReplaceAnObj(sel_ptr->obj);
      } else {
         AbortPrepareCmd(CMD_REPLACE);
      }
   }
   EndCompositeCmd();

   if (changed) {
      UpdSelBBox();
      RedrawAreas(botObj, ltx-GRID_ABS_SIZE(1), lty-GRID_ABS_SIZE(1),
            rbx+GRID_ABS_SIZE(1), rby+GRID_ABS_SIZE(1), selLtX-GRID_ABS_SIZE(1),
            selLtY-GRID_ABS_SIZE(1), selRbX+GRID_ABS_SIZE(1),
            selRbY+GRID_ABS_SIZE(1));
      SetFileModified(TRUE);
   }
   HighLightForward();
}

void ChangeAllSelRealSzUnit(sz_unit, HighLight)
   int sz_unit, HighLight;
{
   struct SelRec *sel_ptr=NULL;
   int changed=FALSE;

   if (topSel == NULL) {
      MsgBox(TgLoadCachedString(CSTID_NO_OBJ_SELECTED), TOOL_NAME, INFO_MB);
      return;
   }
   if (HighLight) HighLightReverse();
   StartCompositeCmd();
   for (sel_ptr=botSel; sel_ptr != NULL; sel_ptr=sel_ptr->prev) {
      PrepareToReplaceAnObj(sel_ptr->obj);
      if (ChangeObjTextSzUnit(sel_ptr->obj, sz_unit)) {
         changed = TRUE;
         RecordReplaceAnObj(sel_ptr->obj);
      } else {
         AbortPrepareCmd(CMD_REPLACE);
      }
   }
   EndCompositeCmd();

   if (changed) {
      int ltx=selLtX, lty=selLtY, rbx=selRbX, rby=selRbY;

      SetFileModified(TRUE);
      UpdSelBBox();
      RedrawAreas(botObj, ltx-GRID_ABS_SIZE(1), lty-GRID_ABS_SIZE(1),
            rbx+GRID_ABS_SIZE(1), rby+GRID_ABS_SIZE(1), selLtX-GRID_ABS_SIZE(1),
            selLtY-GRID_ABS_SIZE(1), selRbX+GRID_ABS_SIZE(1),
            selRbY+GRID_ABS_SIZE(1));
   }
   if (HighLight) HighLightForward();
}

void SetSelFontSize(pszBuf)
   char *pszBuf;
{
   char *psz=NULL, spec[MAXSTRING+1], spec_copy[MAXSTRING+1];
   int sz_unit=(-1), font_size=(-1), point_size=(-1);

   if (topSel == NULL) {
      int size=0;

      if (pszBuf != NULL && sscanf(pszBuf, "%d", &size) == 1) {
         int i, sz_unit=FontSizeToSzUnit(size);

         for (i=0; i<numFontSizes; i++) {
            if (fontSzUnits[i] == sz_unit) {
               ChangeFontSize(i);
               return;
            }
         }
      } else if (curChoice == DRAWTEXT && textCursorShown) {
         /* will call ChangeEditTextProperty() to handle this */
      } else {
         MsgBox(TgLoadCachedString(CSTID_NO_OBJ_SELECTED), TOOL_NAME, INFO_MB);
         return;
      }
   }
   *spec = '\0';
   if (pszBuf != NULL) {
      UtilStrCpyN(spec, sizeof(spec), pszBuf);
   } else {
      if (showFontSizeInPoints) {
         Dialog(TgLoadString(STID_ENTER_POINT_SIZE),
               TgLoadCachedString(CSTID_DLG_ACCEPT_CANCEL), spec);
      } else {
         Dialog(TgLoadString(STID_ENTER_FONT_SIZE),
               TgLoadCachedString(CSTID_DLG_ACCEPT_CANCEL), spec);
      }
   }
   UtilTrimBlanks(spec);
   if (*spec == '\0') return;
   strcpy(spec_copy, spec);
   UtilStrUpper(spec_copy);
   /* do not translate -- program constants */
   if ((psz=strstr(spec_copy, "point")) != NULL ||
         (psz=strstr(spec_copy, "pt")) != NULL) {
      *psz = '\0';
      point_size = atoi(spec_copy);
      sz_unit = PointSizeToSzUnit(point_size);
   } else if (showFontSizeInPoints) {
      point_size = atoi(spec);
      sz_unit = PointSizeToSzUnit(point_size);
   } else {
      font_size = atoi(spec);
      sz_unit = FontSizeToSzUnit(font_size);
   }
   if (sz_unit <= 0) {
      sprintf(gszMsgBox, TgLoadString(STID_FONT_SIZE_TOO_SMALL), spec);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return;
   }
   if (topSel == NULL && curChoice == DRAWTEXT && textCursorShown) {
      if (ChangeEditTextProperty(PROP_MASK_TEXT_SZ_UNIT, sz_unit)) {
         curTextModified = TRUE;
         UpdCurTextBBox();
         RedrawCurText();
         SetFileModified(TRUE);
         if (cycleThroughChoice) {
            SetPushedFontValue(PUSH_SZ_UNIT, GetCurSzUnit());
         }
      }
      ShowCurFont();
      ShowTextSize();
      UpdatePinnedMenu(MENU_SIZE);
   } else {
      ChangeAllSelRealSzUnit(sz_unit, TRUE);
   }
}

void CleanUpFonts()
{
   int i;
   int fmly_index, style_index;
   struct FontSizeRec *fs_ptr, *next_fs;

   CleanUpPSFontCharSubs();
   CleanUpDontReencode();
   if (vertFontBitmap != None) XFreePixmap(mainDisplay, vertFontBitmap);
   if (rotatedVertFontBitmap != None) {
      XFreePixmap(mainDisplay, rotatedVertFontBitmap);
   }
   if (rotatedVertFontImage != NULL) XDestroyImage(rotatedVertFontImage);
   rotatedVertFontImage = NULL;
   vertFontBitmap = rotatedVertFontBitmap = None;
   vertFontBitmapW = vertFontBitmapH = 0;

   if (sizeMenuStr != NULL) {
      for (i=0; i<numFontSizes; i++) {
         UtilFree(sizeMenuStr[i]);
      }
      free(sizeMenuStr);
      sizeMenuStr = NULL;
   }
   if (fontSzUnits != NULL) {
      free(fontSzUnits);
      fontSzUnits = NULL;
   }
   numFontSizes = 0;

   CleanUpPSFontAliases();
   CleanUpTmpStrings();
   CleanUpEncodeCharFonts();

   if (fontFamilies != NULL) {
      for (fmly_index=0; fmly_index<numFonts; fmly_index++) {
         for (style_index=0; style_index<MAXFONTSTYLES; style_index++) {
            if (fontFamilies[fmly_index].fr[style_index].xfs != NULL) {
               XFreeFont(mainDisplay,
                     fontFamilies[fmly_index].fr[style_index].xfs);
            }
            for (fs_ptr=fontFamilies[fmly_index].fr[style_index].next;
                  fs_ptr != NULL; fs_ptr = next_fs) {
               next_fs = fs_ptr->next;
               if (fs_ptr->xfs != NULL && !fs_ptr->faked) {
                  XFreeFont(mainDisplay, fs_ptr->xfs);
               }
               free(fs_ptr);
            }
         }
         UtilFree(fontFamilies[fmly_index].conv_from_utf8_cmd);
         UtilFree(fontFamilies[fmly_index].conv_to_utf8_cmd);
      }
      for (fmly_index=0; fmly_index<numFakedFonts; fmly_index++) {
         free(fontFamilies[fmly_index+numFonts].name_faked);
      }
      free(fontFamilies);
      fontFamilies = NULL;
   }
   if (fontInfoStr != NULL) {
      for (i=MAXFONTS*MAXFONTSTYLES*3; i<numFonts*MAXFONTSTYLES*3; i++) {
         free(fontInfoStr[i]);
      }
      free(fontInfoStr);
      fontInfoStr = NULL;
   }
   if (altFontInfoStr) {
      for (i=0; i<MAXFONTS*MAXFONTSTYLES*3; i++) {
         if (altFontInfoStr[i] != NULL) {
            free(altFontInfoStr[i]);
         }
      }
      free(altFontInfoStr);
      altFontInfoStr = NULL;
   }
   if (fontMenuStr != NULL) {
      for (fmly_index=0; fmly_index<numFonts; fmly_index++) {
         UtilFree(fontMenuStr[fmly_index]);
      }
      free(fontMenuStr);
      fontMenuStr = NULL;
   }
   numFonts = MAXFONTS;
   numFakedFonts = 0;

#ifdef ENABLE_NLS
   if (menuFontSet != NULL) XFreeFontSet(mainDisplay, menuFontSet);
   if (msgFontSet != NULL) XFreeFontSet(mainDisplay, msgFontSet);
   if (boldMsgFontSet != NULL) XFreeFontSet(mainDisplay, boldMsgFontSet);
   if (italicMsgFontSet != NULL) XFreeFontSet(mainDisplay, italicMsgFontSet);
   if (boldItalicMsgFontSet != NULL) {
      XFreeFontSet(mainDisplay, boldItalicMsgFontSet);
   }
#endif /* ENABLE_NLS */
   menuFontSet = msgFontSet = boldMsgFontSet = italicMsgFontSet =
         boldItalicMsgFontSet = NULL;

   if (menuFontPtr != NULL) XFreeFont(mainDisplay, menuFontPtr);
   if (msgFontPtr != NULL) XFreeFont(mainDisplay, msgFontPtr);
   if (boldMsgFontPtr != NULL) XFreeFont(mainDisplay, boldMsgFontPtr);
   if (italicMsgFontPtr != NULL) XFreeFont(mainDisplay, italicMsgFontPtr);
   if (boldItalicMsgFontPtr != NULL) {
      XFreeFont(mainDisplay, boldItalicMsgFontPtr);
   }
   menuFontPtr = msgFontPtr = boldMsgFontPtr = italicMsgFontPtr =
         boldItalicMsgFontPtr = NULL;

   if (rulerFontName != NULL) free(rulerFontName);
   if (defaultFontName != NULL) free(defaultFontName);
   rulerFontName = defaultFontName = NULL;

   if (menuFontName != NULL) free(menuFontName);
   if (msgFontName != NULL) free(msgFontName);
   if (boldMsgFontName != NULL) free(boldMsgFontName);
   if (italicMsgFontName != NULL) free(italicMsgFontName);
   if (boldItalicMsgFontName != NULL) free(boldItalicMsgFontName);
   menuFontName = msgFontName = boldMsgFontName = italicMsgFontName =
         boldItalicMsgFontName = NULL;

   if (menuFontSetName != NULL) free(menuFontSetName);
   if (msgFontSetName != NULL) free(msgFontSetName);
   if (boldMsgFontSetName != NULL) free(boldMsgFontSetName);
   if (italicMsgFontSetName != NULL) free(italicMsgFontSetName);
   if (boldItalicMsgFontSetName != NULL) free(boldItalicMsgFontSetName);
   menuFontSetName = msgFontSetName = boldMsgFontSetName =
         italicMsgFontSetName = boldItalicMsgFontSetName = NULL;
}

void VerifyCompatibleFontIndex(font_index)
   int *font_index;
   /* should only call this function if fileVersion <= 29 */
{
   if (*font_index >= MAXFONTS) *font_index = defaultCurFont;
}

static
void GetFontStr(FontIndex, StyleIndex, FontStr)
   int FontIndex, StyleIndex;
   char *FontStr;
{
   char font_str[MAXSTRING];

   *font_str = '\0';
   GetPSFontStr(FontIndex, StyleIndex, font_str);
   /* font_str[0] is '/' now */
   strcpy(FontStr, &font_str[1]);
}

void GetCompatibleFontName(old_font_index, style, font_str)
   int old_font_index, style;
   char *font_str;
{
   int i;
   int cSingle=0;
   int cDouble=0;
   int cpSingle=INVALID;
   int cpDouble=INVALID;
   int old_font_single=(old_font_index % 1000);
   int old_font_double=((old_font_index / 1000) - 1);
   int num_fonts=((PRTGIF && !cmdLineOpenDisplay) ? MAXFONTS+numFakedFonts:
         numFonts+numFakedFonts);

   for (i=0; i < num_fonts; i++) {
      if (!IsFontDoubleByte(i)) {
         if (cSingle == old_font_single) cpSingle = i;
         cSingle++;
      } else {
         if (cDouble == old_font_double) cpDouble = i;
         cDouble++;
      }
   }
   if (cpSingle != INVALID) {
      GetFontStr(cpSingle, style, font_str);
   } else {
      GetFontStr(defaultCurFont, style, font_str);
      sprintf(gszMsgBox, TgLoadString(STID_FONT_SUB_NO_SUCH_FONT_USR_DEF),
            font_str, old_font_single);
      if (PRTGIF) {
         fprintf(stderr, "%s\n", gszMsgBox);
      } else {
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      }
   }
   if (cpDouble != INVALID) {
      strcat(font_str, "%");
      GetFontStr(cpDouble, style, font_str+strlen(font_str));
   } else if (old_font_double >=0) {
      sprintf(gszMsgBox, TgLoadString(STID_NO_KANJI_FONT_GIVEN_NUMBER),
            old_font_double);
      if (PRTGIF) {
         fprintf(stderr, "%s\n", gszMsgBox);
      } else {
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      }
   }
}

/* The following procedure is used to generate pdrawFontAsc[] and */
/*    pDrawFontDes[], to be used in ``prtgif.c''.  It is supposed */
/*    to be called within dbx and not tgif.                       */

/*
 * static
 * void GenerateFontInfo()
 * {
 *    register int i, j, num_rows;
 * 
 *    for (i = 0; i < FONTTABLESIZE; i++)
 *       if (!myFontInfo[i].valid)
 *       {
 *          if ((myFontInfo[i].xfs =
 *                XLoadQueryFont(mainDisplay, fontNameStr[i])) == NULL)
 *          {
 *             printf("Cannot open %s.  Abort.\n\n", fontNameStr[i]);
 *             exit(-1);
 *          }
 *          myFontInfo[i].valid = TRUE;
 *       }
 * 
 *    num_rows = FONTTABLESIZE / MAXFONTSIZES;
 *    printf("short\tpDrawFontAsc[] =\n{\n");
 *    for (i = 0; i < num_rows; i++)
 *    {
 *       printf("   ");
 *       for (j = 0; j < MAXFONTSIZES; j++)
 *          if (i == num_rows-1 && j == MAXFONTSIZES-1)
 *             printf("%2d ",
 *                   (myFontInfo[i*MAXFONTSIZES+j].xfs)->max_bounds.ascent);
 *          else
 *             printf("%2d, ",
 *                   (myFontInfo[i*MAXFONTSIZES+j].xfs)->max_bounds.ascent);
 *       printf("\n");
 *    }
 *    printf("};\n\n");
 * 
 *    printf("short\tpDrawFontDes[] =\n{\n");
 *    for (i = 0; i < num_rows; i++)
 *    {
 *       printf("   ");
 *       for (j = 0; j < MAXFONTSIZES; j++)
 *          if (i == num_rows-1 && j == MAXFONTSIZES-1)
 *             printf("%2d ",
 *                   (myFontInfo[i*MAXFONTSIZES+j].xfs)->max_bounds.descent);
 *          else
 *             printf("%2d, ",
 *                   (myFontInfo[i*MAXFONTSIZES+j].xfs)->max_bounds.descent);
 *       printf("\n");
 *    }
 *    printf("};\n");
 * }
 */
