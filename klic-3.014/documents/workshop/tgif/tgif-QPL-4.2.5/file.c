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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/file.c,v 1.69 2011/05/16 16:21:57 william Exp $
 */

#define _INCLUDE_FROM_FILE_C_

#include "tgifdefs.h"
#include "expfdefs.h"
#include "cmdids.h"

#include "align.e"
#include "arc.e"
#include "attr.e"
#include "auxtext.e"
#include "box.e"
#include "button.e"
#include "choice.e"
#include "cmd.e"
#include "color.e"
#include "cutpaste.e"
#include "cursor.e"
#include "dialog.e"
#include "drawing.e"
#include "dup.e"
#include "edit.e"
#include "eps.e"
#include "exec.e"
#include "file.e"
#include "font.e"
#include "grid.e"
#include "group.e"
#include "imgproc.e"
#include "import.e"
#include "ini.e"
#include "mainloop.e"
#include "mainmenu.e"
#include "mark.e"
#include "menu.e"
#include "menuinfo.e"
#include "miniline.e"
#include "move.e"
#include "msg.e"
#include "names.e"
#include "navigate.e"
#include "obj.e"
#include "oval.e"
#include "page.e"
#include "pattern.e"
#include "poly.e"
#include "polygon.e"
#include "prtgif.e"
#include "ps.e"
#include "raster.e"
#include "rcbox.e"
#include "rect.e"
#include "remote.e"
#include "ruler.e"
#include "scroll.e"
#include "select.e"
#include "setup.e"
#include "shape.e"
#include "special.e"
#include "spline.e"
#include "stk.e"
#include "stretch.e"
#include "strtbl.e"
#include "text.e"
#include "util.e"
#include "version.e"
#include "wb.e"
#include "xbitmap.e"
#include "xpixmap.e"
#include "xprtfltr.e"

#ifdef _METRIC_PIX_PER_INCH
#define METRIC_PIX_PER_INCH 127
#endif /* _METRIC_PIX_PER_INCH */

#define CUR_VERSION 37

int	PRTGIF=FALSE;
int	prTgifFoundColorInfo=FALSE;
char	curFileName[MAXPATHLENGTH+1];
int	curFileDefined=FALSE;
int	fileVersion=INVALID;
int	curFileWriteVersion=CUR_VERSION;
int	importingFile=FALSE;
int	mergingFile=FALSE;
int	serializingFile=FALSE;
int	deserializingFile=FALSE;
int	psDotsPerInch=72;
float	printMag=(float)100.0;
int	saveTmpOnReturn=TRUE;
int	warpToWinCenter=TRUE;
float	tiledPageScaling=(float)0.9;

char	*psXOffStr[MAXPAGESTYLES] = {"0","0"};
float	psXOff[MAXPAGESTYLES] =     { 0,  0 };
char	* * psYOffStr=NULL;
float	* psYOff=NULL;
float	* psPageWidthInInch=NULL;
float	* psPageHeightInInch=NULL;

char	printCommand[MAXSTRING+1];
char	outputDir[MAXPATHLENGTH+1];

char	* savedComments=NULL;
int	savedCommentsLen=0;
int	saveCommentsInSaveNew=TRUE;
int	usePsAdobeString=FALSE;
char	adobeString[80], epsfString[80];

int	readingPageNum=0;
int	loadedCurPageNum=0;

int	writeFileFailed=FALSE;
int	foundGoodStateObject=FALSE;

int	cmdLineHasPageNum=FALSE;
int	cmdLinePageNum=(-1);
char	cmdLinePageNumStr[80];

int	cmdLineOneFilePerPage=FALSE;
int	cmdLineA4=FALSE;
int	cmdLineLetter=FALSE;
int	cmdLineNoShowPageInEPS=FALSE;
int	cmdLineColor=FALSE;
int	cmdLineRequestedColor=FALSE;

int	cmdLineDumpURL=FALSE;
int	cmdLineDumpURLWithHeader=FALSE;
int	cmdLineDumpURLHeaderOnly=FALSE;
int	cmdLineDumpURLShowStatus=FALSE;

int	cmdLineDosEpsFilter=FALSE;
int	cmdLinePreviewOnly=FALSE;

int	cmdLinePsSetup=FALSE;

int	tmpFileMode=PSFILE_MOD;
char	cmdLineTmpFileModeStr[MAXSTRING];

int	cmdLineOpenDisplay=FALSE;
int	cmdLineStdOut=FALSE;
int	cmdLineWhereToPrint=INVALID;
int	cmdLineTiffEPSI=INVALID;
int	cmdLinePdfSetPageDevice=INVALID;

char	cmdLineFileToExec[MAXSTRING];
char	cmdLineProducedBy[MAXSTRING];
char	cmdLineOpenFile[MAXSTRING];
char	cmdLineJpegToPpm6Cmd[MAXSTRING];

int	cmdLineQuiet=FALSE;

char	cmdLineCustomPatternDir[MAXSTRING];
char	customPatDir[MAXSTRING];

int	generateTiffEPSI=FALSE;
int	showPageInEPS=TRUE;

int	gnQuit=FALSE;

int	myFileBgPixel=INVALID, myFileFgPixel=INVALID;
char	*myFileBgColorStr=NULL, *myFileFgColorStr=NULL;
XColor	myFileBgColor;
int	myFileBgPixmapW=(-1), myFileBgPixmapH=(-1);
Pixmap	myFileBgPixmap=None;
char	*myFileBgGifURL=NULL;
char	gszProducedBy[MAXSTRING];

int	dumpOneFilePerPage=FALSE;

int	bufferAsFileForWhiteBoard=FALSE;

int	flushColormapOnOpen=FALSE;

struct BBRec	*gpExportClipBBox=NULL;

int		gnNumFilePSFontAliases=0;
KeyValInfo	*gaFilePSFontAliases=NULL;

SpecifyPagesInfo	gPagesToPrintSpec;

GenerateByInfo	gGenerateByInfo;

static int	pdfSetPageDevice=INVALID;

static int	psRegMarksInTiledPageMode=INVALID;
static float	psRegMarksGray=(float)0.95;

static int importingPageNum=(-1);
static char importingPageName[MAXSTRING+1];

static int unsavableFile=FALSE;

static char	*gpszPsSetup=NULL;
static int	psSetupStatus=INVALID;
static int	psDistillerNoImageCompress=INVALID;

struct DocFontRec {
   char name[80];
   int len;
   struct DocFontRec *next;
};

static struct DocFontRec *firstDocFont=NULL;

char *MkTempFile(buf, buf_sz, psz_dir, psz_prefix)
   char *buf, *psz_dir, *psz_prefix;
   int buf_sz;
{
   int mkstemp_succes=FALSE;

#ifdef _TGIF_DBG /* debug, do not translate */
   TgAssert(buf_sz > sizeof(char*),
         "buf_sz <= sizeof(char*) in MkTempFile()", NULL);
#endif /* _TGIF_DBG */

#ifndef _DONT_USE_MKTEMP
   sprintf(buf, "%s%sXXXXXX", psz_dir, psz_prefix);
   mktemp(buf);
#else /* _DONT_USE_MKTEMP */
   {
      int fd=(-1);

      sprintf(buf, "%s%sXXXXXX", psz_dir, psz_prefix);
      if ((fd=mkstemp(buf)) == (-1)) {
         /*
          * UtilStrCpyN(buf, buf_sz, (char*)tempnam(psz_dir, psz_prefix));
          */
         sprintf(buf, "%s%sXXXXXX", psz_dir, psz_prefix);
         sprintf(gszMsgBox, TgLoadString(STID_CANT_OBTAIN_TMP_FILE_NAME),
               buf);
         if (PRTGIF) {
            fprintf(stderr, "%s\n", gszMsgBox);
         } else {
            MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
         }
         return NULL;
      } else {
         close(fd);
         mkstemp_succes = TRUE;
      }
   }
#endif /* ~_DONT_USE_MKTEMP */

   if (!mkstemp_succes) {
#ifdef O_EXCL
      int fd=open(buf, O_CREAT|O_EXCL|O_WRONLY, S_IRUSR|S_IWUSR);

      if (fd == -1) {
         /* barf loudly that someone is trying to hack the box */
         sprintf(gszMsgBox, TgLoadString(STID_RACE_IN_CREATE_TMP_FILE), buf);
         if (PRTGIF) {
            fprintf(stderr, "%s\n", gszMsgBox);
         } else {
            MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
         }
         return NULL;
      } else {
         close(fd);
      }
#endif /* O_EXCL */
   }
   unlink(buf);

   return buf;
}

int IsFiletUnSavable()
{
   return unsavableFile;
}

void SetUnSavableFile(unsavable)
   int unsavable;
{
   if (!unsavableFile && unsavable) {
      Msg(TgLoadString(STID_FILE_IS_MARKED_UNSAVABLE));
   }
   unsavableFile = unsavable;
}

void UpdateDocumentFonts(ps_font_name)
   char *ps_font_name;
{
   int len=strlen(ps_font_name);
   struct DocFontRec *df_ptr;

   for (df_ptr=firstDocFont; df_ptr != NULL; df_ptr=df_ptr->next) {
      if (df_ptr->len == len && strcmp(df_ptr->name, ps_font_name) == 0) {
         return;
      }
   }
   df_ptr = (struct DocFontRec *)malloc(sizeof(struct DocFontRec));
   if (df_ptr == NULL) FailAllocMessage();
   memset(df_ptr, 0, sizeof(struct DocFontRec));
   df_ptr->len = len;
   UtilStrCpyN(df_ptr->name, sizeof(df_ptr->name), ps_font_name);
   df_ptr->next = firstDocFont;
   firstDocFont = df_ptr;
}

static
void DumpDocumentFonts(dump_fp)
   FILE *dump_fp;
{
   struct DocFontRec *next_df=NULL;

   if (fprintf(dump_fp, "%%%%DocumentFonts: ") == EOF) {
      writeFileFailed = TRUE;
   }
   for ( ; firstDocFont != NULL; firstDocFont=next_df) {
      next_df = firstDocFont->next;
      if (fprintf(dump_fp, "%s", firstDocFont->name) == EOF) {
         writeFileFailed = TRUE;
      }
      if (next_df != NULL) {
         if (fprintf(dump_fp, "\n%%%%+ ") == EOF) {
            writeFileFailed = TRUE;
         }
      }
      free(firstDocFont);
   }
   if (fprintf(dump_fp, "\n") == EOF) writeFileFailed = TRUE;
}

#define LANGUAGELEVEL 0
#define EXTENSIONS 1
#define DOCUMENTFONTS 2
#define DOCUMENTNEEDEDRESOURCES 3
#define DOCUMENTNEEDEDFONTS 4

#define MAXPROPAGATEDEPSINFO 5

struct tagPropagatedEPSInfoRec {
   char *name;
   int status;
} gstPropagatedEPSInfo[] = {
   { "%%LanguageLevel", LANGUAGELEVEL },
   { "%%Extensions", EXTENSIONS },
   { "%%DocumentFonts", DOCUMENTFONTS },
   { "%%DocumentNeededResources", DOCUMENTNEEDEDRESOURCES },
   { "%%DocumentNeededFonts", DOCUMENTNEEDEDFONTS },
   { NULL, INVALID }
};

static KeyValInfo *gPropagatedEPSInfo[MAXPROPAGATEDEPSINFO];
static int maxLanguageLevel=0;

static
int FindPropagatedEPSInfo(buf)
   char *buf;
{
   struct tagPropagatedEPSInfoRec *ppei=gstPropagatedEPSInfo;

   for (ppei=gstPropagatedEPSInfo; ppei->name != NULL; ppei++) {
      if (strcmp(buf, ppei->name) == 0) {
         return ppei->status;
      }
   }
   return INVALID;
}

static
void CleanUpPropagatedEPSInfo()
{
   int i=0;

   maxLanguageLevel = 0;
   for (i=0; i < MAXPROPAGATEDEPSINFO; i++) {
      KeyValInfo *pkvi=NULL, *p_next_kvi=NULL;

      for (pkvi=gPropagatedEPSInfo[i]; pkvi != NULL; pkvi=p_next_kvi) {
         p_next_kvi = pkvi->next;
         UtilFree(pkvi->value);
         free(pkvi);
      }
   }
   memset(gPropagatedEPSInfo, 0, MAXPROPAGATEDEPSINFO*sizeof(KeyValInfo*));
}

static
void DumpPropagatedEPSInfo(dump_fp)
   FILE *dump_fp;
{
   int i=0;

   if (maxLanguageLevel > 0) {
      if (fprintf(dump_fp, "%%%%LanguageLevel: %1d\n", maxLanguageLevel) ==
            EOF) {
         writeFileFailed = TRUE;
      }
   }
   for (i=0; i < MAXPROPAGATEDEPSINFO; i++) {
      if (gPropagatedEPSInfo[i] != NULL) {
         KeyValInfo *pkvi=NULL, *p_next_kvi=NULL;

         if (fprintf(dump_fp, "%s: ", gstPropagatedEPSInfo[i].name) == EOF) {
            writeFileFailed = TRUE;
         }
         for (pkvi=gPropagatedEPSInfo[i]; pkvi != NULL; pkvi=p_next_kvi) {
            p_next_kvi = pkvi->next;
            if (fprintf(dump_fp, "%s", pkvi->value) == EOF) {
               writeFileFailed = TRUE;
            }
            if (p_next_kvi != NULL) {
               if (fprintf(dump_fp, "\n%%%%+ ") == EOF) {
                  writeFileFailed = TRUE;
               }
            }
            UtilFree(pkvi->value);
            free(pkvi);
         }
         if (fprintf(dump_fp, "\n") == EOF) writeFileFailed = TRUE;
         gPropagatedEPSInfo[i] = NULL;
      }
   }
}

static
void DoUpdatePropagatedEPSInfo(index, psz_value)
   int index;
   char *psz_value;
{
   int len=strlen(psz_value);
   KeyValInfo *pkvi=NULL, *p_last_kvi=NULL;

   for (pkvi=gPropagatedEPSInfo[index]; pkvi != NULL; pkvi=pkvi->next) {
      if (pkvi->key == ((char*)(long)len) &&
            strcmp(pkvi->value, psz_value) == 0) {
         return;
      }
      p_last_kvi = pkvi;
   }
   pkvi = (KeyValInfo *)malloc(sizeof(KeyValInfo));
   if (pkvi == NULL) FailAllocMessage();
   memset(pkvi, 0, sizeof(KeyValInfo));
   pkvi->key = ((char*)(long)len);
   pkvi->value = UtilStrDup(psz_value);
   if (pkvi->value == NULL) FailAllocMessage();
   if (p_last_kvi == NULL) {
      gPropagatedEPSInfo[index] = pkvi;
   } else {
      p_last_kvi->next = pkvi;
   }
}

void UpdatePropagatedEPSInfo(line, pn_status)
   char *line;
   int *pn_status;
{
   int propagated_eps_status=INVALID, level=0;
   char *arg=NULL;

   if (*pn_status == INVALID) {
      arg = strchr(line, ':');

      if (arg == NULL) return;
      *arg++ = '\0';
      propagated_eps_status = FindPropagatedEPSInfo(line);
      if (propagated_eps_status == INVALID) return;
   } else {
      propagated_eps_status = (*pn_status);
      arg = (&line[3]);
   }
#ifdef _TGIF_DBG /* debug, do not translate */
   TgAssert(propagated_eps_status != INVALID,
         "propagated_eps_status != INVALID in UpdatePropagatedEPSInfo()",
         NULL);
#endif /* _TGIF_DBG */
   UtilTrimBlanks(arg);
   if (arg[0] == '(' && UtilStrICmp(arg, "(atend)") == 0) {
      *pn_status = INVALID;
      return;
   }
   switch (propagated_eps_status) {
   case LANGUAGELEVEL:
      if (sscanf(arg, "%d", &level) == 1) {
         if (level > maxLanguageLevel) {
            maxLanguageLevel = level;
         }
      }
      break;
   case EXTENSIONS:
   case DOCUMENTNEEDEDFONTS:
   case DOCUMENTNEEDEDRESOURCES:
      DoUpdatePropagatedEPSInfo(propagated_eps_status, arg);
      break;
   case DOCUMENTFONTS:
      UpdateDocumentFonts(arg);
      break;
   }
   *pn_status = propagated_eps_status;
}

static int dontUseShortHandPS=INVALID;
static int dontCondense=INVALID;
static int stripSimpleComments=INVALID;

void SetCmdLineDontCondense()
{
   dontUseShortHandPS = TRUE;
   dontCondense = TRUE;
   stripSimpleComments = FALSE;
}

void SetCmdLineCondensedPS()
{
   dontUseShortHandPS = FALSE;
   dontCondense = FALSE;
   stripSimpleComments = TRUE;
}

static char gszResidual[80];
static int gnResidualLen=0;

static
void DumpResidual(write_fp)
   FILE *write_fp;
{
   if (gnResidualLen > 0) {
      if (fputs(gszResidual, write_fp) == EOF) writeFileFailed = TRUE;
      if (fputs("\n", write_fp) == EOF) writeFileFailed = TRUE;
      *gszResidual = '\0';
      gnResidualLen = 0;
   }
}

static
void AddToResidual(write_fp, buf)
   FILE *write_fp;
   char *buf;
{
   int len=strlen(buf);

   if (len <= 0) return;

   if (len+gnResidualLen >= 77) {
      DumpResidual(write_fp);
      if (len >= 77) {
         if (fputs(buf, write_fp) == EOF) writeFileFailed = TRUE;
         if (fputs("\n", write_fp) == EOF) writeFileFailed = TRUE;
         return;
      }
   }
   if (gnResidualLen == 0) {
      sprintf(&gszResidual[gnResidualLen], "%s", buf);
      gnResidualLen += len;
   } else {
      sprintf(&gszResidual[gnResidualLen], " %s", buf);
      gnResidualLen += len+1;
   }
}

static
char *GetPostScriptToken(read_fname, pszStart, ppszEnd)
   char *read_fname, *pszStart, **ppszEnd;
{
   char *psz=NULL;

   *ppszEnd = NULL;
   while (*pszStart == ' ' || *pszStart == '\t') pszStart++;
   if (*pszStart == '\0') return NULL;
   if (*pszStart == '%') return NULL;
   if (*pszStart == '(') {
      for (psz=pszStart; *psz != ')' && *psz != '\0'; psz++) {
         if (*psz == '\\') {
            psz++;
            if (*psz >= '0' && *psz <= '7') {
               psz++;
               psz++;
            }
         }
      }
      if (*psz == ')') {
         *ppszEnd = (&psz[1]);
      } else {
         /* no problem - Adobe Illustrator 6.0 does this a lot */
      }
   } else {
      for (psz=pszStart;
            *psz!='\0' && *psz!=' ' && *psz!='\t' && *psz!='(' && *psz!='%';
            psz++) {
      }
      if (*psz == '%') {
         *psz = '\0';
      }
      if (*psz != '\0') {
         *ppszEnd = psz;
      }
   }
   return pszStart;
}

static
void CondenseAndCopyPostScriptFile(read_fp, write_fp, read_fname, condense)
   FILE *read_fp, *write_fp;
   char *read_fname;
   int condense;
{
   if (condense) {
      char *buf;
      int previewing=FALSE, begin_preview_len=strlen("%%BeginPreview:");

      if (stripSimpleComments == INVALID) stripSimpleComments = FALSE;
      *gszResidual = '\0';
      gnResidualLen = 0;
      while ((buf=UtilGetALine(read_fp)) != NULL) {
         if (*buf == '%') {
            if (strncmp(buf, "%%BeginPreview:", begin_preview_len) == 0) {
               previewing = TRUE;
            } else if (strncmp(buf, "%%EndPreview", begin_preview_len-3) == 0) {
               previewing = FALSE;
            }
            if (!stripSimpleComments || buf[1] == '%' || buf[1] == '!' ||
                  previewing) {
               DumpResidual(write_fp);
               AddToResidual(write_fp, buf);
               DumpResidual(write_fp);
            }
         } else {
            char *psz=NULL, *pszStart=buf, *pszEnd=NULL;

            while ((psz=GetPostScriptToken(read_fname, pszStart, &pszEnd)) !=
                  NULL) {
               char saved_ch='\0';

               if (pszEnd != NULL) {
                  saved_ch = (*pszEnd);
                  *pszEnd = '\0';
               }
               AddToResidual(write_fp, psz);
               if (pszEnd == NULL) {
                  break;
               }
               *pszEnd = saved_ch;
               pszStart = pszEnd;
               pszEnd = NULL;
            }
         }
         UtilFree(buf);
      }
      DumpResidual(write_fp);
   } else {
      char tmp_str[MAXSTRING<<1];

      while (fgets(tmp_str, MAXSTRING, read_fp) != NULL) {
         if (fputs(tmp_str, write_fp) == EOF) {
            writeFileFailed = TRUE;
         }
      }
   }
}

static
int CondensePostScriptFile(read_fname, buf_sz)
   char *read_fname;
   int buf_sz;
{
   char write_fname[MAXSTRING+1];
   FILE *read_fp=NULL, *write_fp=NULL;

   if (MkTempFile(write_fname, sizeof(write_fname), tmpDir, TOOL_NAME) ==
         NULL) {
      return FALSE;
   }
   if ((read_fp=fopen(read_fname, "r")) == NULL) {
      sprintf(gszMsgBox, TgLoadString(STID_CANNOT_OPEN_FOR_READ_PRINT),
            read_fname);
      if (PRTGIF) {
         fprintf(stderr, "%s\n", gszMsgBox);
      } else {
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      }
      return FALSE;
   }
   if ((write_fp=fopen(write_fname, "w")) == NULL) {
      sprintf(gszMsgBox, TgLoadString(STID_CANNOT_OPEN_FOR_WRITE_PRINT),
            write_fname);
      if (PRTGIF) {
         fprintf(stderr, "%s\n", gszMsgBox);
      } else {
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      }
      fclose(read_fp);
      return FALSE;
   }
   CondenseAndCopyPostScriptFile(read_fp, write_fp, read_fname, TRUE);
   fclose(read_fp);
   fclose(write_fp);
   unlink(read_fname);
   UtilStrCpyN(read_fname, buf_sz, write_fname);

   return TRUE;
}

static
int CopyAFile(file1, file2, condense)
   char *file1, *file2;
   int condense;
{
   char tmp_str[MAXSTRING<<1], *rest;
   FILE *fp1, *fp2;

   if ((fp1=fopen(file1, "r")) == NULL) {
      sprintf(tmp_str, TgLoadString(STID_CANNOT_OPEN_FILE_FOR_READING), file1);
      if (PRTGIF) {
         fprintf(stderr, "%s\n", tmp_str);
      } else {
         MsgBox(tmp_str, TOOL_NAME, INFO_MB);
      }
      return FALSE;
   }
   if (file2 == NULL) {
      /* PRTGIF && cmdLineStdOut */
      fp2 = stdout;
   } else {
      int short_name=FALSE;

      if ((short_name=IsPrefix(bootDir, file2, &rest))) ++rest;
      if ((fp2=fopen(file2, "w")) == NULL) {
         if (PRTGIF) {
            sprintf(tmp_str, TgLoadString(STID_CANNOT_OPEN_FILE_FOR_WRITING),
                  file2);
            fprintf(stderr, "%s\n", tmp_str);
         } else {
            if (short_name) {
               sprintf(tmp_str, TgLoadString(STID_CANNOT_OPEN_FILE_FOR_WRITING),
                     rest);
            } else {
               sprintf(tmp_str, TgLoadString(STID_CANNOT_OPEN_FILE_FOR_WRITING),
                     file2);
            }
            MsgBox(tmp_str, TOOL_NAME, INFO_MB);
         }
         fclose(fp1);
         return FALSE;
      }
   }
   writeFileFailed = FALSE;
   CondenseAndCopyPostScriptFile(fp1, fp2, file1, (condense && !dontCondense));
   fclose(fp1);
   if (file2 != NULL) fclose(fp2);
   if (writeFileFailed) {
      writeFileFailed = FALSE;
      if (file2 == NULL) {
         strcpy(tmp_str, TgLoadString(STID_FAIL_TO_WRITE_TO_STDOUT));
         if (PRTGIF) {
            fprintf(stderr, "%s\n", tmp_str);
         } else {
            MsgBox(tmp_str, TOOL_NAME, INFO_MB);
         }
      } else {
         FailToWriteFileMessage(file2);
      }
      return FALSE;
   }
   return TRUE;
}

int FindProgramInPath(cmd, msg, no_msg)
   char *cmd, *msg;
   int no_msg;
{
   char *psz=strchr(cmd, ' '), szPath[MAXPATHLENGTH];

   if (psz != NULL) *psz = '\0';
   UtilStrCpyN(szPath, sizeof(szPath), cmd);
   if (psz != NULL) *psz = ' ';

   if (*szPath == '\0') {
      /* no program specified, assume it's okay */
      return TRUE;
   } else if (*szPath == DIR_SEP) {
      if (UtilPathExists(szPath)) return TRUE;
   } else {
      char *env=NULL;

      psz = getenv("PATH");
      if (psz == NULL) {
         /* cannot get the PATH environment, assume it's okay */
         return TRUE;
      }
      env = UtilStrDup(psz);
      if (env == NULL) FailAllocMessage();
      for (psz=strtok(env, ":"); psz != NULL; psz=strtok(NULL, ":")) {
         sprintf(gszMsgBox, "%s%c%s", psz, DIR_SEP, szPath);
         if (UtilPathExists(gszMsgBox)) {
            UtilFree(env);
            return TRUE;
         }
      }
      UtilFree(env);
   }
   if (no_msg) {
      return FALSE;
   }
   if (msg == NULL || *msg == '\0') {
      sprintf(gszMsgBox, TgLoadString(*szPath==DIR_SEP ?
            STID_CANNOT_FIND_CMD_EXEC : STID_CANNOT_FIND_CMD_IN_PATH_EXEC),
            szPath, cmd);
   } else {
      sprintf(gszMsgBox, TgLoadString(*szPath==DIR_SEP ?
            STID_CANT_FIND_CMD_MSG_EXEC : STID_CANT_FIND_CMD_INPATH_MSG_EXEC),
            szPath, msg, cmd);
   }
   if (PRTGIF) {
      fprintf(stderr, "%s [no]\n", gszMsgBox);
      return FALSE;
   }
   return (MsgBox(gszMsgBox, TOOL_NAME, YNC_MB)==MB_ID_YES);
}

int ExecuteCmd(cmd, done_msg)
   char *cmd;
   int done_msg;
{
   int watch_cursor=watchCursorOnMainWindow;
   char tmp_str[MAXSTRING+1];
   FILE *fp=NULL;

   EndMeasureTooltip(FALSE);
   if (!FindProgramInPath(cmd, NULL, FALSE)) return FALSE;
   sprintf(gszMsgBox, TgLoadCachedString(CSTID_EXECUTING_GIVEN_PROGRAM), cmd);
   SetStringStatus(gszMsgBox);
   if (!PRTGIF) XSync(mainDisplay, False);
   if ((fp=(FILE*)popen(cmd, "r")) == NULL) return FALSE;

   if (!watch_cursor) {
      SetWatchCursor(drawWindow);
      SetWatchCursor(mainWindow);
   }
   while (fgets(tmp_str, MAXSTRING, fp) != NULL) {
      if (PRTGIF) {
         fprintf(stderr, "%s", tmp_str);
      } else {
         Msg(tmp_str);
      }
   }
   pclose(fp);
   if (!watch_cursor) {
      SetDefaultCursor(mainWindow);
      ShowCursor();
   }
   if (done_msg) {
      SetStringStatus(TgLoadCachedString(CSTID_DOTS_DONE));
   }
   return TRUE;
}

void CleanUpComments()
{
   if (savedComments != NULL) {
      free(savedComments);
      savedComments = NULL;
      savedCommentsLen = 0;
   }
}

void ClearFileInfo(clean_up_comments)
   int clean_up_comments;
   /*
    * This function is called everything a new file is initialized.
    */
{
   *curFileName = '\0';
   curFileDefined = FALSE;
   if (!curDirIsLocal) strcpy(curDir, curLocalDir);
   curDirIsLocal = TRUE;
   *curLocalDir = '\0';
   *curSymDir = '\0';

   if (clean_up_comments) {
      CleanUpComments();
   }
   *gszHhtmlExportTemplate = '\0';

   SetUnSavableFile(FALSE);
   memset(&gGenerateByInfo, 0, sizeof(GenerateByInfo));
}

int OkayToCreateFile(FileName)
   char *FileName;
{
   FILE *fp;

   if ((fp=fopen(FileName, "r")) == NULL) return TRUE;
   fclose(fp);
   sprintf(gszMsgBox, TgLoadString(STID_FILE_EXISTS_OK_OVERWRITE_YNC),
         FileName);
   switch (MsgBox(gszMsgBox, TOOL_NAME, YNC_MB)) {
   case MB_ID_YES: break;
   case MB_ID_NO: return FALSE;
   case MB_ID_CANCEL: return FALSE;
   }
   unlink(FileName);
   return TRUE;
}

void SaveObj(FP, ObjPtr, Level)
   FILE *FP;
   struct ObjRec *ObjPtr;
   int Level;
{
   switch (ObjPtr->type) {
   case OBJ_POLY: SavePolyObj(FP, ObjPtr); break;
   case OBJ_BOX: SaveBoxObj(FP, ObjPtr); break;
   case OBJ_OVAL: SaveOvalObj(FP, ObjPtr); break;
   case OBJ_TEXT: SaveTextObj(FP, ObjPtr); break;
   case OBJ_POLYGON: SavePolygonObj(FP, ObjPtr); break;
   case OBJ_ARC: SaveArcObj(FP, ObjPtr); break;
   case OBJ_RCBOX: SaveRCBoxObj(FP, ObjPtr); break;
   case OBJ_XBM: SaveXBmObj(FP, ObjPtr); break;
   case OBJ_XPM: SaveXPmObj(FP, ObjPtr); break;
   case OBJ_GROUP: SaveGroupObj(FP, ObjPtr, Level); break;
   case OBJ_SYM: SaveCompObj(FP, ObjPtr, Level); break;
   case OBJ_ICON: SaveIconObj(FP, ObjPtr, Level); break;
   case OBJ_PIN: SavePinObj(FP, ObjPtr, Level); break;
   /* reserved */
   case OBJ_SS: SaveSimpleStringObj(FP, ObjPtr); break;
   }
}

int FileNameHasExtension(file_name, file_type, pn_gzipped, pn_no_name)
   char *file_name;
   int file_type, *pn_gzipped, *pn_no_name;
{
   static char stszObjFileExt[20], stszGzObjFileExt[20];
   static char stszSymFileExt[20];
   static char stszPinFileExt[20];
   static int initialized=FALSE;
   char *psz=NULL;

   if (!initialized) {
      sprintf(stszObjFileExt, ".%s", OBJ_FILE_EXT);
      sprintf(stszGzObjFileExt, ".%s.gz", OBJ_FILE_EXT);
      sprintf(stszSymFileExt, ".%s", SYM_FILE_EXT);
      sprintf(stszPinFileExt, ".%s", PIN_FILE_EXT);
      initialized = TRUE;
   }
   if (pn_gzipped != NULL) *pn_gzipped = FALSE;
   switch (file_type) {
   case OBJ_FILE_TYPE:
      if ((psz=strstr(file_name, ".obj")) != NULL &&
            strcmp(psz, ".obj") == 0) {
         if (pn_no_name != NULL) *pn_no_name = (psz == file_name);
         return TRUE;
      } else if ((psz=strstr(file_name, ".obj.gz")) != NULL &&
            strcmp(psz, ".obj.gz") == 0) {
         if (pn_no_name != NULL) *pn_no_name = (psz == file_name);
         if (pn_gzipped != NULL) *pn_gzipped = TRUE;
         return TRUE;
      } else if ((psz=strstr(file_name, ".tgo")) != NULL &&
            strcmp(psz, ".tgo") == 0) {
         if (pn_no_name != NULL) *pn_no_name = (psz == file_name);
         return TRUE;
      } else if ((psz=strstr(file_name, ".tgo.gz")) != NULL &&
            strcmp(psz, ".tgo.gz") == 0) {
         if (pn_no_name != NULL) *pn_no_name = (psz == file_name);
         if (pn_gzipped != NULL) *pn_gzipped = TRUE;
         return TRUE;
      } else if ((psz=strstr(file_name, stszObjFileExt)) != NULL &&
            strcmp(psz, stszObjFileExt) == 0) {
         if (pn_no_name != NULL) *pn_no_name = (psz == file_name);
         return TRUE;
      } else if ((psz=strstr(file_name, stszGzObjFileExt)) != NULL &&
            strcmp(psz, stszGzObjFileExt) == 0) {
         if (pn_no_name != NULL) *pn_no_name = (psz == file_name);
         if (pn_gzipped != NULL) *pn_gzipped = TRUE;
         return TRUE;
      }
      break;
   case SYM_FILE_TYPE:
      if ((psz=strstr(file_name, ".sym")) != NULL &&
            strcmp(psz, ".sym") == 0) {
         if (pn_no_name != NULL) *pn_no_name = (psz == file_name);
         return TRUE;
      } else if ((psz=strstr(file_name, ".tgs")) != NULL &&
            strcmp(psz, ".tgs") == 0) {
         if (pn_no_name != NULL) *pn_no_name = (psz == file_name);
         return TRUE;
      } else if ((psz=strstr(file_name, stszSymFileExt)) != NULL &&
            strcmp(psz, stszSymFileExt) == 0) {
         if (pn_no_name != NULL) *pn_no_name = (psz == file_name);
         return TRUE;
      }
      break;
   case PIN_FILE_TYPE:
      if ((psz=strstr(file_name, ".pin")) != NULL &&
            strcmp(psz, ".pin") == 0) {
         if (pn_no_name != NULL) *pn_no_name = (psz == file_name);
         return TRUE;
      } else if ((psz=strstr(file_name, ".tgp")) != NULL &&
            strcmp(psz, ".tgp") == 0) {
         if (pn_no_name != NULL) *pn_no_name = (psz == file_name);
         return TRUE;
      } else if ((psz=strstr(file_name, stszPinFileExt)) != NULL &&
            strcmp(psz, stszPinFileExt) == 0) {
         if (pn_no_name != NULL) *pn_no_name = (psz == file_name);
         return TRUE;
      }
      break;
   }
   if (pn_no_name != NULL) *pn_no_name = (*file_name == '\0');

   return FALSE;
}

void RemoveFileNameExtension(file_name)
   char *file_name;
{
   static char stszObjFileExt[20], stszGzObjFileExt[20];
   static char stszSymFileExt[20];
   static char stszPinFileExt[20];
   static int initialized=FALSE;
   char *psz=NULL;
   int len=strlen(file_name);

   if (!initialized) {
      sprintf(stszObjFileExt, ".%s", OBJ_FILE_EXT);
      sprintf(stszGzObjFileExt, ".%s.gz", OBJ_FILE_EXT);
      sprintf(stszSymFileExt, ".%s", SYM_FILE_EXT);
      sprintf(stszPinFileExt, ".%s", PIN_FILE_EXT);
      initialized = TRUE;
   }
   if ((psz=strstr(file_name, ".obj")) != NULL &&
         strcmp(psz, ".obj") == 0) {
      file_name[len-strlen(".obj")] = '\0';
   } else if ((psz=strstr(file_name, ".obj.gz")) != NULL &&
         strcmp(psz, ".obj.gz") == 0) {
      file_name[len-strlen(".obj.gz")] = '\0';
   } else if ((psz=strstr(file_name, ".tgo")) != NULL &&
         strcmp(psz, ".tgo") == 0) {
      file_name[len-strlen(".tgo")] = '\0';
   } else if ((psz=strstr(file_name, ".tgo.gz")) != NULL &&
         strcmp(psz, ".tgo.gz") == 0) {
      file_name[len-strlen(".tgo.gz")] = '\0';
   } else if ((psz=strstr(file_name, stszObjFileExt)) != NULL &&
         strcmp(psz, stszObjFileExt) == 0) {
      file_name[len-strlen(stszObjFileExt)] = '\0';
   } else if ((psz=strstr(file_name, stszGzObjFileExt)) != NULL &&
         strcmp(psz, stszGzObjFileExt) == 0) {
      file_name[len-strlen(stszGzObjFileExt)] = '\0';
   } else if ((psz=strstr(file_name, ".sym")) != NULL &&
         strcmp(psz, ".sym") == 0) {
      file_name[len-strlen(".sym")] = '\0';
   } else if ((psz=strstr(file_name, ".tgs")) != NULL &&
         strcmp(psz, ".tgs") == 0) {
      file_name[len-strlen(".tgs")] = '\0';
   } else if ((psz=strstr(file_name, stszSymFileExt)) != NULL &&
         strcmp(psz, stszSymFileExt) == 0) {
      file_name[len-strlen(stszSymFileExt)] = '\0';
   } else if ((psz=strstr(file_name, ".pin")) != NULL &&
         strcmp(psz, ".pin") == 0) {
      file_name[len-strlen(".pin")] = '\0';
   } else if ((psz=strstr(file_name, ".tgp")) != NULL &&
         strcmp(psz, ".tgp") == 0) {
      file_name[len-strlen(".tgp")] = '\0';
   } else if ((psz=strstr(file_name, stszPinFileExt)) != NULL &&
         strcmp(psz, stszPinFileExt) == 0) {
      file_name[len-strlen(stszPinFileExt)] = '\0';
   }
}

int FixDecimalPoint(buf)
   char *buf;
{
   char *psz=strchr(buf, ','), *psz2=NULL;

   if (psz == NULL) {
      return TRUE;
   }
   psz2 = strchr(&psz[1], ',');
   if (psz2 == NULL) {
      *psz = '.';
      return TRUE;
   }
   return FALSE;
}

void Save(FP, BotObjPtr, Level, PageNumber)
   FILE *FP;
   struct ObjRec *BotObjPtr;
   int Level, PageNumber;
{
   int watch_cursor=watchCursorOnMainWindow;
   struct ObjRec *obj_ptr=NULL;

   if (BotObjPtr != NULL && !copyInDrawTextMode) {
      SetCurChoice(NOTHING);

      if (!watch_cursor) {
         SetWatchCursor(drawWindow);
         SetWatchCursor(mainWindow);
      }
   }
   if (Level == 0 && PageNumber == 1) {
      char font_str[MAXSTRING], print_mag_str[MAXSTRING];

      ResetXPmErrorMessage();
      GetPSFontStr(curFont, curStyle, font_str);
      SetFullVersionString();
      /* do not translate -- program constants */
      if (fprintf(FP, "%%TGIF %s\n", fullVersionString) == EOF) {
         writeFileFailed = TRUE;
      }
      snprintf(print_mag_str, sizeof(print_mag_str), "%.3f", printMag);
      if (!FixDecimalPoint(print_mag_str)) {
         /* do not translate -- program constants */
         sprintf(gszMsgBox, TgLoadString(STID_BAD_GIVEN_LINE_WRITTEN),
               "print_mag");
         if (PRTGIF) {
            fprintf(stderr, "%s\n", gszMsgBox);
         } else {
            Msg(gszMsgBox);
         }
         writeFileFailed = TRUE;
      }
      if (fprintf(FP, "state(%1d,%1d,%s,", pageStyle, CUR_VERSION,
            print_mag_str) == EOF) {
         writeFileFailed = TRUE;
      }
      if (fprintf(FP, "%1d,%1d,%1d,", drawOrigX, drawOrigY, zoomScale) == EOF) {
         writeFileFailed = TRUE;
      }
      if (fprintf(FP, "%1d,%1d,%1d,", xyEnglishGrid, snapOn, colorIndex) ==
            EOF) {
         writeFileFailed = TRUE;
      }
      if (fprintf(FP, "%1d,%1d,%1d,", horiAlign, vertAlign, lineWidth) == EOF) {
         writeFileFailed = TRUE;
      }
      if (fprintf(FP, "%1d,%1d,%1d,%1d,", curSpline, lineStyle, objFill,
            penPat) == EOF) {
         writeFileFailed = TRUE;
      }
      if (fprintf(FP, "%1d,'%s',%1d,%1d,", textJust,
            /* font_str starts with the '/' character */
            &font_str[1], curStyle, GetCurSzUnit()) == EOF) {
         writeFileFailed = TRUE;
      }
      if (fprintf(FP, "%1d,%1d,%1d,", 0, curDash, gridSystem) == EOF) {
         writeFileFailed = TRUE;
      }
      if (fprintf(FP, "%1d,%1d,%1d,", xyMetricGrid, textVSpace, zoomedIn) ==
            EOF) {
         writeFileFailed = TRUE;
      }
      if (fprintf(FP, "%1d,%1d,%1d,", gridShown, moveMode, ROTATE0) == EOF) {
         writeFileFailed = TRUE;
      }
      if (fprintf(FP, "%1d,%1d,", rcbRadius, useGray) == EOF) {
         writeFileFailed = TRUE;
      }
      if (fprintf(FP, "%1d,%1d,%1d,%1d,%1d,%1d,%1d,%1d,%1d,%1d,%1d).\n",
            pageLayoutMode,
            (pageLayoutMode == PAGE_STACK) ? curPageNum : paperCol,
            (pageLayoutMode == PAGE_STACK) ? lastPageNum : paperRow,
            pageLineShownInTileMode, colorDump,
            round(((float)onePageWidth)*printMag/100.0),
            round(((float)onePageHeight)*printMag/100.0),
            stretchableText, textRotation, rotationIncrement,
            transPat) == EOF) {
         writeFileFailed = TRUE;
      }
      if ((BotObjPtr != NULL || tgifObj != NULL) && !copyInDrawTextMode &&
            !serializingFile) {
         char script_frac_str[MAXSTRING];

         if (fprintf(FP, "%%\n") == EOF) {
            writeFileFailed = TRUE;
         }
         if (fprintf(FP, "%% @%s%s\n", "(#)$H", "eader$") == EOF) {
            writeFileFailed = TRUE;
         }
         if (fprintf(FP, "%% %s\n", "%W%") == EOF) {
            writeFileFailed = TRUE;
         }
         if (fprintf(FP, "%%\n") == EOF) {
            writeFileFailed = TRUE;
         }
         if (savedComments != NULL) {
            if (fputs(savedComments, FP) == EOF) {
               writeFileFailed = TRUE;
            }
         }
         if (tgifObj->lattr != NULL) {
            if (fprintf(FP, "file_attr(") == EOF) writeFileFailed = TRUE;
            SaveAttrs(FP, tgifObj->lattr);
            if (fprintf(FP, ").\n") == EOF) writeFileFailed = TRUE;
         }
         GetUnitSpec(gszMsgBox);
         if (fprintf(FP, "unit(\"") == EOF) writeFileFailed = TRUE;
         SaveString(FP, gszMsgBox);
         if (fprintf(FP, "\").\n") == EOF) writeFileFailed = TRUE;
         if (shapeShadowInResource ||
               shapeShadowDx != 0 || shapeShadowDy != 0) {
            if (fprintf(FP, "shapeshadow(%1d,%1d).\n", shapeShadowDx,
                  shapeShadowDy) == EOF) {
               writeFileFailed = TRUE;
            }
         }
         SaveColors(FP);
         if (slideShowInfoValid) {
            if (fprintf(FP, "slideshow_info('%s',%1d,%1d).\n",
                  (slideShowBorderColor==NULL ? "" : slideShowBorderColor),
                  slideShowXOffset, slideShowYOffset) == EOF) {
               writeFileFailed = TRUE;
            }
         }
         UtilStrCpyN(script_frac_str, sizeof(script_frac_str), scriptFractionStr);
         if (!FixDecimalPoint(script_frac_str)) {
            /* do not translate -- program constants */
            sprintf(gszMsgBox, TgLoadString(STID_BAD_GIVEN_LINE_WRITTEN),
                  "script_frac");
            if (PRTGIF) {
               fprintf(stderr, "%s\n", gszMsgBox);
            } else {
               Msg(gszMsgBox);
            }
            writeFileFailed = TRUE;
         }
         if (fprintf(FP, "script_frac(\"%s\").\n", script_frac_str) == EOF) {
            writeFileFailed = TRUE;
         }
         if (fprintf(FP, "fg_bg_colors('%s','%s').\n",
               colorMenuItems[colorIndex], (*defaultBgColorStr=='\0' ?
               myBgColorStr : defaultBgColorStr)) == EOF) {
            /*
             * The above should use colorMenuItmes[bgColorIndex]
             *       and not myBgColorStr.
             */
            writeFileFailed = TRUE;
         }
         if (*gszHhtmlExportTemplate != '\0') {
            if (fprintf(FP, "html_export_template(\"%s\").\n",
                  gszHhtmlExportTemplate) == EOF) {
               writeFileFailed = TRUE;
            }
         }
         SaveDontReencode(FP);
         SavePSFontAliases(FP);
         SaveDoubleByteModBytes(FP);
         if (fprintf(FP, "objshadow_info('%s',%1d,%1d).\n",
               objShadowColorStr, objShadowXOffset, objShadowYOffset) == EOF) {
            writeFileFailed = TRUE;
         }
         if (fprintf(FP, "rotate_pivot(%1d,%1d,%1d,%1d).\n",
               autoRotatePivot, rotatePivotAbsXYValid, rotatePivotAbsX,
               rotatePivotAbsY) == EOF) {
            writeFileFailed = TRUE;
         }
         if (fprintf(FP, "spline_tightness(%1d).\n", tighterStructSplines) ==
               EOF) {
            writeFileFailed = TRUE;
         }
         if (rightMarginEnabled != INVALID) {
            if (fprintf(FP, "right_margin(%1d,%1d).\n", rightMarginEnabled,
                  rightMargin) == EOF) {
               writeFileFailed = TRUE;
            }
         }
         if (threshFillReplaceEnabled) {
            if (fprintf(FP, "threshold_fill_replace(%1d,%1d,%1d).\n",
                  fillReplaceRedThresh, fillReplaceGreenThresh,
                  fillReplaceBlueThresh) == EOF) {
               writeFileFailed = TRUE;
            }
         }
         SavePSCharSubs(FP);
         SavePSFontNeedCharSubs(FP);
      }
   }
   if (Level == 0 && BotObjPtr != NULL && !copyInDrawTextMode) {
      if (fprintf(FP, "page(%1d,\"", PageNumber) == EOF) writeFileFailed = TRUE;
      SaveString(FP, ((pageLayoutMode==PAGE_TILE || curPage->name==NULL) ? "" :
            curPage->name));
      if (fprintf(FP, "\",%1d,'%s').\n", curPage->layer_on,
            ((pageLayoutMode==PAGE_TILE || curPage->page_file_name==NULL) ? "" :
            curPage->page_file_name)) == EOF) {
         writeFileFailed = TRUE;
      }
   }

   for (obj_ptr=BotObjPtr; obj_ptr != NULL; obj_ptr=obj_ptr->prev) {
      SaveObj(FP, obj_ptr, Level);
      if (obj_ptr->prev == NULL) {
         if (Level == 0) {
            if (fprintf(FP, ".\n") == EOF) writeFileFailed = TRUE;
         } else {
            if (fprintf(FP, "\n") == EOF) writeFileFailed = TRUE;
         }
      } else {
         if (Level == 0) {
            if (fprintf(FP, ".\n") == EOF) writeFileFailed = TRUE;
         } else {
            if (fprintf(FP, ",\n") == EOF) writeFileFailed = TRUE;
         }
      }
   }
   if (BotObjPtr != NULL && !copyInDrawTextMode) {
      if (!watch_cursor) {
         SetDefaultCursor(mainWindow);
         ShowCursor();
      }
   }
}

int SaveTmpFile(NewFileName)
   char *NewFileName;
   /* return TRUE if file successfully saved */
{
   char new_file_name[MAXPATHLENGTH+1], *rest=NULL;
   FILE *fp=NULL;
   int count=0, file_type=INVALID, short_name, watch_cursor=FALSE;
   struct PageRec *saved_cur_page;
   struct ObjRec *obj_ptr;
   struct ObjRec *obj_ptr1=NULL, *obj_ptr2=NULL;
   struct AttrRec *attr_ptr;

   UtilStrCpyN(new_file_name, sizeof(new_file_name), NewFileName);

   saved_cur_page = curPage;
   for (curPage=firstPage; curPage != NULL; curPage=curPage->next) {
      for (obj_ptr=curPage->top; obj_ptr != NULL; obj_ptr=obj_ptr->next) {
         if (obj_ptr->type == OBJ_SYM) {
            if (obj_ptr1 == NULL) {
               obj_ptr1 = obj_ptr;
            } else if (obj_ptr2 == NULL) {
               obj_ptr2 = obj_ptr;
            }
            count++;
         }
      }
   }
   curPage = saved_cur_page;

   switch (count) {
   case 0:
      sprintf(new_file_name, "%s.%s", NewFileName, OBJ_FILE_EXT);
      file_type = OBJ_FILE_TYPE;
      break;
   case 1:
      if (lastPageNum != 1) {
         MsgBox(TgLoadString(STID_ONE_PAGE_SYM_ABORT_SAVE), TOOL_NAME, INFO_MB);
         return INVALID;
      }
      if ((attr_ptr=FindAttrWithName(obj_ptr1, "type=", NULL)) != NULL &&
            strcmp(attr_ptr->attr_value.s, "tgif_pin") == 0) {
         sprintf(new_file_name, "%s.%s", NewFileName, PIN_FILE_EXT);
         file_type = PIN_FILE_TYPE;
      } else {
         sprintf(new_file_name, "%s.%s", NewFileName, SYM_FILE_EXT);
         file_type = SYM_FILE_TYPE;
      }
      break;
   case 2:
      if (lastPageNum != 1) {
         MsgBox(TgLoadString(STID_ONE_PAGE_PIN_ABORT_SAVE), TOOL_NAME, INFO_MB);
         return INVALID;
      }
      sprintf(new_file_name, "%s.%s", NewFileName, PIN_FILE_EXT);
      file_type = PIN_FILE_TYPE;
      break;
   default:
      MsgBox(TgLoadString(STID_TOO_MANY_SYM_ABORT_SAVE), TOOL_NAME, INFO_MB);
      return INVALID;
   }

   unlink(new_file_name);

   if ((short_name=IsPrefix(bootDir, new_file_name, &rest))) ++rest;
   if ((fp=fopen(new_file_name, "w")) == NULL) {
      sprintf(gszMsgBox, TgLoadString(STID_CANT_OPEN_WRITE_FILE_NOT_SAVE),
            (short_name ? rest : new_file_name));
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return INVALID;
   }
   sprintf(gszMsgBox, TgLoadString(STID_SAVE_TMP_FILE_DOTS),
         (short_name ? rest : new_file_name));
   Msg(gszMsgBox);

   watch_cursor = watchCursorOnMainWindow;
   if (!watch_cursor) {
      SetWatchCursor(drawWindow);
      SetWatchCursor(mainWindow);
   }
   writeFileFailed = FALSE;
   MakeQuiescent();
   saved_cur_page = curPage;
   for (curPage=firstPage, count=1; curPage != NULL;
         curPage=curPage->next, count++) {
      topObj = curPage->top;
      botObj = curPage->bot;
      Save(fp, botObj, 0, count);
   }
   curPage = saved_cur_page;
   topObj = curPage->top;
   botObj = curPage->bot;
   fclose(fp);
   if (!watch_cursor) {
      SetDefaultCursor(mainWindow);
      ShowCursor(); 
   }
   if (writeFileFailed) {
      writeFileFailed = FALSE;
      FailToWriteFileMessage(new_file_name);
   } else {
      sprintf(gszMsgBox, TgLoadString(STID_TMP_FILE_SAVED),
            (short_name ? rest : new_file_name));
      Msg(gszMsgBox);
   }

   if (tmpFileMode != 0 && chmod(new_file_name, tmpFileMode)) {
      sprintf(gszMsgBox, TgLoadString(STID_CANNOT_CHMOD),
            (short_name ? rest : new_file_name), tmpFileMode);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
   }
   SetCurChoice(curChoiceBeforeMakeQuiescent);

   return file_type;
}

void SaveNewFile(SaveSelectedOnly, pszNewFile)
   int SaveSelectedOnly;
   char *pszNewFile;
{
   char new_file_name[MAXPATHLENGTH+1], gzipped_fname[MAXPATHLENGTH+1];
   char new_full_name[MAXPATHLENGTH+1], tmp_str[MAXPATHLENGTH+1], *rest=NULL;
   char saved_cur_dir[MAXPATHLENGTH+1], saved_cur_file_name[MAXPATHLENGTH+1];
   char obj_ext_str[MAXSTRING+1], gz_obj_ext_str[MAXSTRING+1];
   char sym_ext_str[MAXSTRING+1], pin_ext_str[MAXSTRING+1];
   int saved_cur_file_defined=FALSE, no_name=FALSE, gzipped=FALSE, file_type=0;
   int count=0, len, short_name=FALSE, ok=TRUE, watch_cursor=FALSE;
   struct ObjRec *obj_ptr=NULL, *saved_top_obj=NULL, *saved_bot_obj=NULL;
   struct ObjRec *obj_ptr1=NULL, *obj_ptr2=NULL;
   struct SelRec *top_sel_ptr=NULL, *bot_sel_ptr=NULL;
   struct SelRec *sel_ptr=NULL, *next_sel=NULL;
   struct PageRec *saved_cur_page=NULL;
   struct AttrRec *attr_ptr=NULL;
   FILE *fp=NULL;

   file_type = OBJ_FILE_TYPE;
   *gzipped_fname = '\0';

   if (SaveSelectedOnly && pszNewFile == NULL && topSel == NULL) {
      MsgBox("No objects selected!\n\nNothing saved!", TOOL_NAME, INFO_MB);
      return;
   }
   if (SaveSelectedOnly && pszNewFile != NULL) {
      UtilStrCpyN(new_file_name, sizeof(new_file_name), pszNewFile);
   } else if (pszNewFile != NULL && strcmp(pszNewFile, "-1") != 0) {
      UtilStrCpyN(new_file_name, sizeof(new_file_name), pszNewFile);
      len = strlen(new_file_name);
      if (len > 0 && new_file_name[len-1] == ')') {
         new_file_name[len-1] = '\0';
      }
   } else {
      sprintf(gszMsgBox, TgLoadString(STID_WORKING_DIRECTORY_IS),
            (curDirIsLocal ? curDir : curLocalDir));
      *new_file_name = '\0';
      Dialog(TgLoadString(STID_ENTER_NEW_FNAME_ACCEPT_CANCEL), gszMsgBox,
            new_file_name);
   }
   UtilTrimBlanks(new_file_name);
   if (*new_file_name == '\0') return;
   len = strlen(new_file_name);

   if (SaveSelectedOnly) {
      for (sel_ptr=topSel; sel_ptr != NULL; sel_ptr=sel_ptr->next) {
         if (sel_ptr->obj->type == OBJ_SYM) {
            if (obj_ptr1 == NULL) {
               obj_ptr1 = sel_ptr->obj;
            } else if (obj_ptr2 == NULL) {
               obj_ptr2 = sel_ptr->obj;
            }
            count++;
         }
      }
   } else {
      saved_cur_page = curPage;
      for (curPage=firstPage; curPage != NULL; curPage=curPage->next) {
         for (obj_ptr=curPage->top; obj_ptr != NULL; obj_ptr=obj_ptr->next) {
            if (obj_ptr->type == OBJ_SYM) {
               if (obj_ptr1 == NULL) {
                  obj_ptr1 = obj_ptr;
               } else if (obj_ptr2 == NULL) {
                  obj_ptr2 = obj_ptr;
               }
               count++;
            }
         }
      }
      curPage = saved_cur_page;
   }
   sprintf(obj_ext_str, ".%s", OBJ_FILE_EXT);
   sprintf(gz_obj_ext_str, ".%s.gz", OBJ_FILE_EXT);
   sprintf(sym_ext_str, ".%s", SYM_FILE_EXT);
   sprintf(pin_ext_str, ".%s", PIN_FILE_EXT);

   if (count > 2) {
      MsgBox(TgLoadString(STID_TOO_MANY_SYM_ABORT_SAVE), TOOL_NAME, INFO_MB);
      return;
   }
   if (count >= 1 && lastPageNum != 1) {
      MsgBox(TgLoadString(count==1 ? STID_ONE_PAGE_SYM_ABORT_SAVE :
            STID_ONE_PAGE_PIN_ABORT_SAVE), TOOL_NAME, INFO_MB);
      return;
   }
   switch (count) {
   case 0:
      if (FileNameHasExtension(new_file_name, OBJ_FILE_TYPE, &gzipped,
            &no_name)) {
         /* don't modify */
      } else if (FileNameHasExtension(new_file_name, SYM_FILE_TYPE, NULL,
            &no_name)) {
         MsgBox(TgLoadString(STID_CANT_SAVE_SYM_NO_SYM_FOUND), TOOL_NAME,
               INFO_MB);
         return;
      } else if (FileNameHasExtension(new_file_name, PIN_FILE_TYPE, NULL,
            &no_name)) {
         MsgBox(TgLoadString(STID_CANT_SAVE_PIN_NO_SYM_FOUND), TOOL_NAME,
               INFO_MB);
         return;
      } else {
         strcat(new_file_name, obj_ext_str);
      }
      file_type = OBJ_FILE_TYPE;
      break;
   case 1:
      if ((attr_ptr=FindAttrWithName(obj_ptr1, "type=", NULL)) != NULL &&
            strcmp(attr_ptr->attr_value.s, "tgif_pin") == 0) {
         if (FileNameHasExtension(new_file_name, OBJ_FILE_TYPE, NULL,
               &no_name)) {
            MsgBox(TgLoadString(STID_CANT_SAVE_OJB_ONE_SYM_FOUND), TOOL_NAME,
                  INFO_MB);
            return;
         } else if (FileNameHasExtension(new_file_name, SYM_FILE_TYPE, NULL,
               &no_name)) {
            MsgBox(TgLoadString(STID_CANT_SAVE_SYM_SAVE_AS_PIN), TOOL_NAME,
                  INFO_MB);
            return;
         } else if (FileNameHasExtension(new_file_name, PIN_FILE_TYPE, NULL,
               &no_name)) {
            /* don't modify */
         } else {
            strcat(new_file_name, pin_ext_str);
         }
         file_type = PIN_FILE_TYPE;
      } else {
         if (FileNameHasExtension(new_file_name, OBJ_FILE_TYPE, NULL,
               &no_name)) {
            MsgBox(TgLoadString(STID_CANT_SAVE_OJB_ONE_SYM_FOUND), TOOL_NAME,
                  INFO_MB);
            return;
         } else if (FileNameHasExtension(new_file_name, SYM_FILE_TYPE, NULL,
               &no_name)) {
            /* don't modify */
         } else if (FileNameHasExtension(new_file_name, PIN_FILE_TYPE, NULL,
               &no_name)) {
            MsgBox(TgLoadString(STID_CANT_SAVE_PIN_ONE_SYM_FOUND), TOOL_NAME,
                  INFO_MB);
            return;
         } else {
            strcat(new_file_name, sym_ext_str);
         }
         file_type = SYM_FILE_TYPE;
      }
      break;
   case 2:
      if (FileNameHasExtension(new_file_name, OBJ_FILE_TYPE, NULL, &no_name)) {
         MsgBox(TgLoadString(STID_CANT_SAVE_OJB_TWO_SYM_FOUND), TOOL_NAME,
               INFO_MB);
         return;
      } else if (FileNameHasExtension(new_file_name, SYM_FILE_TYPE, NULL,
            &no_name)) {
         MsgBox(TgLoadString(STID_CANT_SAVE_SYM_TWO_SYM_FOUND), TOOL_NAME,
               INFO_MB);
         return;
      } else if (FileNameHasExtension(new_file_name, PIN_FILE_TYPE, NULL,
            &no_name)) {
         /* don't modify */
      } else {
         strcat(new_file_name, pin_ext_str);
      }
      if (obj_ptr2->fattr != NULL) {
         obj_ptr = obj_ptr2;
         obj_ptr2 = obj_ptr1;
         obj_ptr1 = obj_ptr;
      }
      if ((attr_ptr=FindAttrWithName(obj_ptr1, "type=", NULL)) == NULL) {
         MsgBox(TgLoadString(STID_CANT_FIND_TYPE_ATTR_NOT_SAVED), TOOL_NAME,
               INFO_MB);
         return;
      }
      if (strcmp(attr_ptr->attr_value.s, "tgif_pin") != 0) {
         sprintf(gszMsgBox, TgLoadString(STID_SYM_TYPE_WRONG_PIN_NOT_SAVED),
               "tgif_pin");
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
         return;
      }
      file_type = PIN_FILE_TYPE;
      break;
   default: return;
   }
   if (no_name) {
      MsgBox(TgLoadString(STID_NO_FILE_NAME_FILE_NOT_SAVED), TOOL_NAME,
            INFO_MB);
      return;
   }
   if (*new_file_name == DIR_SEP) {
      UtilStrCpyN(new_full_name, sizeof(new_full_name), new_file_name);
   } else if (curDirIsLocal) {
      sprintf(new_full_name, "%s%c%s", curDir, DIR_SEP, new_file_name);
   } else {
      sprintf(new_full_name, "%s%c%s", curLocalDir, DIR_SEP, new_file_name);
   }
   if (file_type == OBJ_FILE_TYPE && gzipped) {
      char tmp_fname[MAXPATHLENGTH];

      if (MkTempFile(tmp_fname, sizeof(tmp_fname), tmpDir, TOOL_NAME) == NULL) {
         return;
      }
      strcpy(gzipped_fname, new_full_name);
      strcpy(new_full_name, tmp_fname);
   }
   if (!OkayToCreateFile((*gzipped_fname=='\0') ? new_full_name :
         gzipped_fname)) {
      if (*gzipped_fname != '\0') unlink(new_full_name);
      return;
   }
   if (*gzipped_fname == '\0') {
      if ((short_name=IsPrefix(bootDir, new_full_name, &rest))) ++rest;
   } else {
      if ((short_name=IsPrefix(bootDir, gzipped_fname, &rest))) ++rest;
   }
   if ((fp=fopen(new_full_name, "w")) == NULL) {
      if (short_name) {
         sprintf(gszMsgBox, TgLoadString(STID_CANT_OPEN_WRITE_FILE_NOT_SAVE),
               rest);
      } else {
         if (*gzipped_fname == '\0') {
            sprintf(gszMsgBox, TgLoadString(STID_CANT_OPEN_WRITE_FILE_NOT_SAVE),
                  new_full_name);
         } else {
            sprintf(gszMsgBox, TgLoadString(STID_CANT_OPEN_WRITE_FILE_NOT_SAVE),
                  gzipped_fname);
         }
      }
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      if (*gzipped_fname != '\0') unlink(new_full_name);
      return;
   }
   if (!SaveSelectedOnly) BeforeNavigate();

   if (SaveSelectedOnly) {
      PushPageInfo();
      if (pageLayoutMode == PAGE_STACK) {
         paperCol = paperRow = curPageNum = lastPageNum = 1;
      }
      if (curDirIsLocal) {
         strcpy(saved_cur_dir, curDir);
      } else {
         strcpy(saved_cur_dir, curLocalDir);
      }
      strcpy(saved_cur_file_name, curFileName);
      saved_cur_file_defined = curFileDefined;
      saved_top_obj = topObj;
      saved_bot_obj = botObj;

      if (topSel == NULL) {
         topObj = botObj = NULL;
      } else {
         JustDupSelObj(&top_sel_ptr, &bot_sel_ptr);
         topObj = top_sel_ptr->obj;
         botObj = bot_sel_ptr->obj;
      }
      firstPage = lastPage = curPage =
            (struct PageRec *)malloc(sizeof(struct PageRec));
      if (firstPage == NULL) FailAllocMessage();
      memset(firstPage, 0, sizeof(struct PageRec));
      firstPage->layer_on = TRUE;
      firstPage->top = topObj;
      firstPage->bot = botObj;
      firstPage->next = firstPage->prev = NULL;
      if (pageLayoutMode == PAGE_STACK) {
         firstPage->draw_orig_x = drawOrigX;
         firstPage->draw_orig_y = drawOrigY;
         firstPage->zoom_scale = zoomScale;
         firstPage->zoomed_in = zoomedIn;
         curPageNum = lastPageNum = 1;
      }
      for (sel_ptr=topSel, obj_ptr=topObj; obj_ptr!=NULL;
            sel_ptr=sel_ptr->next, obj_ptr=obj_ptr->next) {
         CopyObjId(sel_ptr->obj, obj_ptr);
         CopyObjLocks(sel_ptr->obj, obj_ptr);
      }
   }

   if (curDirIsLocal) {
      strcpy(tmp_str, curDir);
   } else {
      strcpy(tmp_str, curLocalDir);
   }
   if (*gzipped_fname == '\0') {
      SetCurDir(new_full_name);
   } else {
      SetCurDir(gzipped_fname);
   }
   curFileDefined = TRUE;
   if (cmdLineMerge) {
      cmdLineMerge = FALSE;
   }
   switch (count) {
   case 0:
      *curSymDir = '\0';
      if ((strcmp(tmp_str, (curDirIsLocal ? curDir : curLocalDir)) != 0) ||
            (!NameInCurDir(curFileName))) {
         UpdateDirInfo();
      }
      break;
   default:
      strcpy(curSymDir, (curDirIsLocal ? curDir : curLocalDir));
      if (!DirInSymPath(curDirIsLocal ? curDir : curLocalDir)) {
         UpdateSymInfo();
      }
      break;
   }
   if (short_name) {
      sprintf(gszMsgBox, TgLoadString(STID_SAVING_DOTS), rest);
   } else {
      if (*gzipped_fname == '\0') {
         sprintf(gszMsgBox, TgLoadString(STID_SAVING_DOTS), new_full_name);
      } else {
         sprintf(gszMsgBox, TgLoadString(STID_SAVING_DOTS), gzipped_fname);
      }
   }
   Msg(gszMsgBox);

   if (!saveCommentsInSaveNew) CleanUpComments();
   watch_cursor = watchCursorOnMainWindow;
   if (!watch_cursor) {
      SetWatchCursor(drawWindow);
      SetWatchCursor(mainWindow);
   }
   writeFileFailed = FALSE;
   saved_cur_page = curPage;
   for (curPage=firstPage, count=1; curPage != NULL;
         curPage=curPage->next, count++) {
      topObj = curPage->top;
      botObj = curPage->bot;
      Save(fp, botObj, 0, count);
   }
   curPage = saved_cur_page;
   topObj = curPage->top;
   botObj = curPage->bot;
   fclose(fp);
   if (!watch_cursor) {
      SetDefaultCursor(mainWindow);
      ShowCursor(); 
   }
   if (writeFileFailed) {
      writeFileFailed = FALSE;
      FailToWriteFileMessage(new_full_name);
      ok = FALSE;
   } else {
      if (*gzipped_fname != '\0') {
         if (!GzipFile(new_full_name, gzipped_fname)) {
            sprintf(gszMsgBox, TgLoadString(STID_PROBLEM_ZIPPING_PLEASE_SAVE),
                  new_full_name, gzipped_fname, new_full_name);
            MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
            ok = FALSE;
         }
         unlink(new_full_name);
         strcpy(new_full_name, gzipped_fname);
      }
      if (ok) {
         sprintf(gszMsgBox, TgLoadString(STID_FILE_SAVED),
               (short_name ? rest : new_full_name));
         Msg(gszMsgBox);
      } else {
         sprintf(gszMsgBox, TgLoadString(STID_FILE_NOT_SAVED),
               (short_name ? rest : new_full_name));
         Msg(gszMsgBox);
      }
   }
   if (SaveSelectedOnly) {
      CleanUpPage();
      for (sel_ptr=top_sel_ptr; sel_ptr != NULL; sel_ptr=next_sel) {
         next_sel = sel_ptr->next;
         free(sel_ptr);
      }

      topObj = saved_top_obj;
      botObj = saved_bot_obj;
      if (curDirIsLocal) {
         strcpy(curDir, saved_cur_dir);
      } else {
         strcpy(curLocalDir, saved_cur_dir);
      }
      strcpy(curFileName, saved_cur_file_name);
      curFileDefined = saved_cur_file_defined;
      PopPageInfo();
   } else if (ok) {
      SetFileModified(FALSE);
      RedrawTitleWindow();
      SetUnSavableFile(FALSE);
   }
   if (!SaveSelectedOnly && !PRTGIF) CommitNavigate();
}

void SaveSymInLibrary()
{
   char new_file_name[MAXPATHLENGTH+1];
   char new_full_name[MAXPATHLENGTH+1], dir_name[MAXPATHLENGTH+1];
   char saved_dir[MAXPATHLENGTH+1], saved_file[MAXPATHLENGTH+1];
   char saved_sym_dir[MAXPATHLENGTH+1], *rest=NULL;
   char sym_ext_str[MAXSTRING+1], pin_ext_str[MAXSTRING+1], *c_ptr;
   FILE *fp;
   int count=0, short_name, watch_cursor=FALSE;
   int saved_cur_file_defined, file_type=INVALID;
   struct ObjRec *obj_ptr;
   struct PageRec *saved_cur_page;
   struct ObjRec *obj_ptr1=NULL, *obj_ptr2=NULL;
   struct AttrRec *attr_ptr;

   saved_cur_page = curPage;
   for (curPage=firstPage; curPage != NULL; curPage=curPage->next) {
      for (obj_ptr=curPage->top; obj_ptr != NULL; obj_ptr=obj_ptr->next) {
         if (obj_ptr->type == OBJ_SYM) {
            if (obj_ptr1 == NULL) {
               obj_ptr1 = obj_ptr;
            } else if (obj_ptr2 == NULL) {
               obj_ptr2 = obj_ptr;
            }
            count++;
         }
      }
   }
   curPage = saved_cur_page;

   switch (count) {
   case 0:
      MsgBox(TgLoadString(STID_NO_SYM_FOUND_SYM_NOT_SAVED), TOOL_NAME, INFO_MB);
      return;
   case 1:
      if (lastPageNum != 1) {
         MsgBox(TgLoadString(STID_ONE_PAGE_SYM_ABORT_SAVE), TOOL_NAME, INFO_MB);
         return;
      }
      if ((attr_ptr=FindAttrWithName(obj_ptr1, "type=", NULL)) != NULL &&
            strcmp(attr_ptr->attr_value.s, "tgif_pin") == 0) {
         file_type = PIN_FILE_TYPE;
      } else {
         file_type = SYM_FILE_TYPE;
      }
      break;
   case 2:
      if (lastPageNum != 1) {
         MsgBox(TgLoadString(STID_ONE_PAGE_PIN_ABORT_SAVE), TOOL_NAME, INFO_MB);
         return;
      }
      if (obj_ptr2->fattr != NULL) {
         obj_ptr = obj_ptr2;
         obj_ptr2 = obj_ptr1;
         obj_ptr1 = obj_ptr;
      }
      if ((attr_ptr=FindAttrWithName(obj_ptr1, "type=", NULL)) == NULL) {
         MsgBox(TgLoadString(STID_CANT_FIND_TYPE_ATTR_NOT_SAVED), TOOL_NAME,
               INFO_MB);
         return;
      }
      if (strcmp(attr_ptr->attr_value.s, "tgif_pin") != 0) {
         sprintf(gszMsgBox, TgLoadString(STID_SYM_TYPE_WRONG_PIN_NOT_SAVED),
               "tgif_pin");
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
         return;
      }
      file_type = PIN_FILE_TYPE;
      break;
   default:
      MsgBox(TgLoadString(STID_TOO_MANY_SYM_ABORT_SAVE), TOOL_NAME, INFO_MB);
      return;
   }
   MakeQuiescent();

   sprintf(sym_ext_str, ".%s", SYM_FILE_EXT);
   sprintf(pin_ext_str, ".%s", PIN_FILE_EXT);

   if (*curFileName == '\0') {
      int no_name=FALSE;

      *new_file_name = '\0';
      Dialog(TgLoadString(STID_ENTER_NEW_FILE_NAME),
            TgLoadCachedString(CSTID_DLG_ACCEPT_CANCEL), new_file_name);
      UtilTrimBlanks(new_file_name);
      if (*new_file_name == '\0') return;

      if (FileNameHasExtension(new_file_name, SYM_FILE_TYPE, NULL, &no_name)) {
         /* do nothing */
      } else if (FileNameHasExtension(new_file_name, PIN_FILE_TYPE, NULL,
            &no_name)) {
         /* do nothing */
      } else if (count == 1) {
         if (file_type == SYM_FILE_TYPE) {
            strcat(new_file_name, sym_ext_str);
         } else {
            strcat(new_file_name, pin_ext_str);
         }
      } else {
         strcat(new_file_name, pin_ext_str);
      }
      if (no_name) {
         MsgBox(TgLoadString(STID_NO_FILE_NAME_FILE_NOT_SAVED), TOOL_NAME,
               INFO_MB);
         return;
      }
   } else {
      c_ptr = UtilStrRChr(curFileName, DIR_SEP);
      strcpy(new_file_name, (c_ptr==NULL) ? curFileName : ++c_ptr);
   }
   if (SelectSymDir(dir_name) == INVALID) {
      Msg("");
      return;
   }
   if (strcmp(dir_name, ".") == 0) {
      sprintf(new_full_name, "%s%c%s", (curDirIsLocal ? curDir : curLocalDir),
            DIR_SEP, new_file_name);
   } else {
      sprintf(new_full_name, "%s%c%s", dir_name, DIR_SEP, new_file_name);
   }

   if (!OkayToCreateFile(new_full_name)) return;

   if ((short_name=IsPrefix(bootDir, new_full_name, &rest))) ++rest;
   if ((fp=fopen(new_full_name, "w")) == NULL) {
      sprintf(gszMsgBox, TgLoadString(STID_CANT_OPEN_WRITE_FILE_NOT_SAVE),
            (short_name ? rest : new_full_name));
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return;
   }

   strcpy(saved_dir, (curDirIsLocal ? curDir : curLocalDir));
   strcpy(saved_file, curFileName);
   strcpy(saved_sym_dir, curSymDir);
   saved_cur_file_defined = curFileDefined;
   SetCurDir(new_full_name);
   curFileDefined = TRUE;

   strcpy(curSymDir, (curDirIsLocal ? curDir : curLocalDir));
   if (!DirInSymPath(curDirIsLocal ? curDir : curLocalDir)) UpdateSymInfo();

   sprintf(gszMsgBox, TgLoadString(STID_SAVING_DOTS),
         (short_name ? rest : new_full_name));
   Msg(gszMsgBox);

   if (!saveCommentsInSaveNew) CleanUpComments();
   watch_cursor = watchCursorOnMainWindow;
   if (!watch_cursor) {
      SetWatchCursor(drawWindow);
      SetWatchCursor(mainWindow);
   }
   writeFileFailed = FALSE;
   saved_cur_page = curPage;
   for (curPage=firstPage, count=1; curPage != NULL;
         curPage=curPage->next, count++) {
      topObj = curPage->top;
      botObj = curPage->bot;
      Save(fp, botObj, 0, count);
   }
   curPage = saved_cur_page;
   topObj = curPage->top;
   botObj = curPage->bot;
   fclose(fp);
   if (!watch_cursor) {
      SetDefaultCursor(mainWindow);
      ShowCursor(); 
   }
   if (writeFileFailed) {
      writeFileFailed = FALSE;
      FailToWriteFileMessage(new_full_name);
   } else {
      sprintf(gszMsgBox, TgLoadString(STID_FILE_SAVED),
            (short_name ? rest : new_full_name));
      Msg(gszMsgBox);
   }

   if (curDirIsLocal) {
      strcpy(curDir, saved_dir);
   } else {
      strcpy(curLocalDir, saved_dir);
   }
   strcpy(curFileName, saved_file);
   strcpy(curSymDir, saved_sym_dir);
   curFileDefined = saved_cur_file_defined;

   RedrawTitleWindow();
   SetCurChoice(curChoiceBeforeMakeQuiescent);
}

void SaveFile()
{
   int count=0, short_name, file_type=INVALID, no_name=FALSE;
   struct ObjRec *obj_ptr=NULL;
   FILE *fp=NULL;
   char ext[MAXPATHLENGTH+1], gzipped_fname[MAXPATHLENGTH+1];
   char full_name[MAXPATHLENGTH+1], *rest=NULL, *psz=NULL;
   struct PageRec *saved_cur_page=NULL;
   struct ObjRec *obj_ptr1=NULL, *obj_ptr2=NULL;
   struct AttrRec *attr_ptr=NULL;
   int watch_cursor=FALSE, gzipped=FALSE;

   *gzipped_fname = '\0';
   if (!curFileDefined || !curDirIsLocal) {
      SaveNewFile(FALSE, NULL);
      return;
   }
   if (unsavableFile) {
      if (MsgBox(TgLoadString(STID_Q_FILE_UNSAVABLE_SAVE_NEW_YNC), TOOL_NAME,
            YNC_MB) == MB_ID_YES) {
         SaveNewFile(FALSE, NULL);
      }
      return;
   }
   if ((psz=UtilStrRChr(curFileName, '.')) == NULL) {
      sprintf(gszMsgBox, TgLoadString(STID_CANT_FIND_DOT_IN_SAVEFILE),
            "SaveFile()");
      TgAssert(FALSE, gszMsgBox, NULL);
      return;
   }
   if (strcmp(psz, ".gz") == 0) {
      char *dot_ptr=psz;

      gzipped = TRUE;
      *dot_ptr = '\0';
      if ((psz=UtilStrRChr(curFileName, '.')) == NULL) {
         sprintf(gszMsgBox, TgLoadString(STID_CANT_FIND_DOT_IN_SAVEFILE),
               "SaveFile()");
         TgAssert(FALSE, gszMsgBox, NULL);
         *dot_ptr = '.';
         return;
      }
      UtilStrCpyN(ext, sizeof(ext), psz);
      *dot_ptr = '.';
   } else {
      UtilStrCpyN(ext, sizeof(ext), psz);
   }
   saved_cur_page = curPage;
   for (curPage=firstPage; curPage != NULL; curPage=curPage->next) {
      for (obj_ptr=curPage->top; obj_ptr != NULL; obj_ptr=obj_ptr->next) {
         if (obj_ptr->type == OBJ_SYM) {
            if (obj_ptr1 == NULL) {
               obj_ptr1 = obj_ptr;
            } else if (obj_ptr2 == NULL) {
               obj_ptr2 = obj_ptr;
            }
            count++;
         }
      }
   }
   curPage = saved_cur_page;

   switch (count) {
   case 0:
      if (FileNameHasExtension(ext, SYM_FILE_TYPE, NULL, &no_name)) {
         MsgBox(TgLoadString(STID_NO_SYM_FOUND_SYM_NOT_SAVED), TOOL_NAME,
               INFO_MB);
         return;
      } else if (FileNameHasExtension(ext, PIN_FILE_TYPE, NULL, &no_name)) {
         MsgBox(TgLoadString(STID_NO_SYM_FOUND_PIN_NOT_SAVED), TOOL_NAME,
               INFO_MB);
         return;
      }
      file_type = OBJ_FILE_TYPE;
      break;
   case 1:
      if (FileNameHasExtension(ext, OBJ_FILE_TYPE, NULL, &no_name)) {
         MsgBox(TgLoadString(STID_CANT_SAVE_OJB_ONE_SYM_FOUND), TOOL_NAME,
               INFO_MB);
         return;
      }
      if ((attr_ptr=FindAttrWithName(obj_ptr1, "type=", NULL)) != NULL &&
            strcmp(attr_ptr->attr_value.s, "tgif_pin") == 0) {
         if (FileNameHasExtension(ext, SYM_FILE_TYPE, NULL, &no_name)) {
            sprintf(gszMsgBox, TgLoadString(STID_SYM_TYPE_IS_SHOULD_SAVE_PIN),
                  "tgif_pin");
            MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
            return;
         }
         file_type = PIN_FILE_TYPE;
      } else {
         if (FileNameHasExtension(ext, PIN_FILE_TYPE, NULL, &no_name)) {
            MsgBox(TgLoadString(STID_CANT_SAVE_PIN_ONE_SYM_FOUND), TOOL_NAME,
                  INFO_MB);
            return;
         }
         file_type = SYM_FILE_TYPE;
      }
      break;
   case 2:
      if (FileNameHasExtension(ext, OBJ_FILE_TYPE, NULL, &no_name)) {
         MsgBox(TgLoadString(STID_CANT_SAVE_OJB_ONE_SYM_FOUND), TOOL_NAME,
               INFO_MB);
         return;
      } else if (FileNameHasExtension(ext, SYM_FILE_TYPE, NULL, &no_name)) {
         MsgBox(TgLoadString(STID_TOO_MANY_SYM_ABORT_SAVE), TOOL_NAME,
               INFO_MB);
         return;
      }
      file_type = PIN_FILE_TYPE;
      break;
   default:
      MsgBox(TgLoadString(STID_TOO_MANY_SYM_FILE_NOT_SAVED), TOOL_NAME,
            INFO_MB);
      return;
   }
   if (file_type == SYM_FILE_TYPE || file_type == PIN_FILE_TYPE) {
      sprintf(full_name, "%s%c%s", curSymDir, DIR_SEP, curFileName);
   } else {
      sprintf(full_name, "%s%c%s", curDir, DIR_SEP, curFileName);
   }
   if (file_type == OBJ_FILE_TYPE && gzipped) {
      char tmp_fname[MAXPATHLENGTH];

      if (MkTempFile(tmp_fname, sizeof(tmp_fname), tmpDir, TOOL_NAME) == NULL) {
         return;
      }
      strcpy(gzipped_fname, full_name);
      strcpy(full_name, tmp_fname);
   }
   if (*gzipped_fname == '\0') {
      if ((short_name=IsPrefix(bootDir, full_name, &rest))) ++rest;
   } else {
      if ((short_name=IsPrefix(bootDir, gzipped_fname, &rest))) ++rest;
   }
   if ((fp=fopen(full_name, "w")) == NULL) {
      sprintf(gszMsgBox, TgLoadString(STID_CANT_OPEN_WRITE_FILE_NOT_SAVE),
            (short_name ? rest : full_name));
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      if (*gzipped_fname != '\0') unlink(full_name);
      return;
   }
   if (short_name) {
      sprintf(gszMsgBox, TgLoadString(STID_SAVING_DOTS), rest);
   } else {
      sprintf(gszMsgBox, TgLoadString(STID_SAVING_DOTS),
            (*gzipped_fname == '\0' ? full_name : gzipped_fname));
   }
   Msg(gszMsgBox);

   watch_cursor = watchCursorOnMainWindow;
   if (!watch_cursor) {
      SetWatchCursor(drawWindow);
      SetWatchCursor(mainWindow);
   }
   writeFileFailed = FALSE;
   MakeQuiescent();
   saved_cur_page = curPage;
   for (curPage=firstPage, count=1; curPage != NULL;
         curPage=curPage->next, count++) {
      topObj = curPage->top;
      botObj = curPage->bot;
      Save(fp, botObj, 0, count);
   }
   curPage = saved_cur_page;
   topObj = curPage->top;
   botObj = curPage->bot;

   fclose(fp);

   if (!watch_cursor) {
      SetDefaultCursor(mainWindow);
      ShowCursor();
   }
   if (writeFileFailed) {
      writeFileFailed = FALSE;
      FailToWriteFileMessage(full_name);
   } else {
      if (*gzipped_fname != '\0') {
         if (!GzipFile(full_name, gzipped_fname)) {
            sprintf(gszMsgBox, TgLoadString(STID_PROBLEM_ZIPPING_PLEASE_SAVE),
                  full_name, gzipped_fname, full_name);
            MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
            unlink(full_name);
            return;
         }
         unlink(full_name);
         strcpy(full_name, gzipped_fname);
      }
      sprintf(gszMsgBox, TgLoadString(STID_FILE_SAVED),
            (short_name ? rest : full_name));
      Msg(gszMsgBox);
      SetFileModified(FALSE);
      if (!NameInCurDir(curFileName)) UpdateDirInfo();
   }
   SetCurChoice(curChoiceBeforeMakeQuiescent);
}

void SavePages(pSpecifyPagesInfo)
   SpecifyPagesInfo *pSpecifyPagesInfo;
{
   char new_file_name[MAXPATHLENGTH+1], gzipped_fname[MAXPATHLENGTH+1];
   char new_full_name[MAXPATHLENGTH+1], *rest=NULL;
   char obj_ext_str[MAXSTRING+1], gz_obj_ext_str[MAXSTRING+1];
   char sym_ext_str[MAXSTRING+1], pin_ext_str[MAXSTRING+1];
   int no_name=FALSE, gzipped=FALSE, file_type=0, page_num=0;
   int count=0, len, short_name=FALSE, ok=TRUE, watch_cursor=FALSE;
   struct ObjRec *obj_ptr=NULL;
   struct ObjRec *obj_ptr1=NULL, *obj_ptr2=NULL;
   struct PageRec *saved_cur_page=NULL;
   struct AttrRec *attr_ptr=NULL;
   FILE *fp=NULL;

   file_type = OBJ_FILE_TYPE;
   *gzipped_fname = '\0';

   sprintf(gszMsgBox, TgLoadString(STID_WORKING_DIRECTORY_IS),
         (curDirIsLocal ? curDir : curLocalDir));
   *new_file_name = '\0';
   Dialog(TgLoadString(STID_ENTER_NEW_FNAME_ACCEPT_CANCEL), gszMsgBox,
         new_file_name);

   UtilTrimBlanks(new_file_name);
   if (*new_file_name == '\0') return;
   len = strlen(new_file_name);

   saved_cur_page = curPage;
   for (page_num=0, curPage=firstPage; curPage != NULL;
         curPage=curPage->next, page_num++) {
      if (pSpecifyPagesInfo->page_specified[page_num]) {
         for (obj_ptr=curPage->top; obj_ptr != NULL; obj_ptr=obj_ptr->next) {
            if (obj_ptr->type == OBJ_SYM) {
               if (obj_ptr1 == NULL) {
                  obj_ptr1 = obj_ptr;
               } else if (obj_ptr2 == NULL) {
                  obj_ptr2 = obj_ptr;
               }
               count++;
            }
         }
      }
   }
   curPage = saved_cur_page;

   sprintf(obj_ext_str, ".%s", OBJ_FILE_EXT);
   sprintf(gz_obj_ext_str, ".%s.gz", OBJ_FILE_EXT);
   sprintf(sym_ext_str, ".%s", SYM_FILE_EXT);
   sprintf(pin_ext_str, ".%s", PIN_FILE_EXT);

   if (count > 2) {
      MsgBox(TgLoadString(STID_TOO_MANY_SYM_ABORT_SAVE), TOOL_NAME, INFO_MB);
      return;
   }
   if (count >= 1 && pSpecifyPagesInfo->num_pages_specified != 1) {
      MsgBox(TgLoadString(count==1 ? STID_ONE_PAGE_SYM_ABORT_SAVE :
            STID_ONE_PAGE_PIN_ABORT_SAVE), TOOL_NAME, INFO_MB);
      return;
   }
   switch (count) {
   case 0:
      if (FileNameHasExtension(new_file_name, OBJ_FILE_TYPE, &gzipped,
            &no_name)) {
         /* don't modify */
      } else if (FileNameHasExtension(new_file_name, SYM_FILE_TYPE, NULL,
            &no_name)) {
         MsgBox(TgLoadString(STID_CANT_SAVE_SYM_NO_SYM_FOUND), TOOL_NAME,
               INFO_MB);
         return;
      } else if (FileNameHasExtension(new_file_name, PIN_FILE_TYPE, NULL,
            &no_name)) {
         MsgBox(TgLoadString(STID_CANT_SAVE_PIN_NO_SYM_FOUND), TOOL_NAME,
               INFO_MB);
         return;
      } else {
         strcat(new_file_name, obj_ext_str);
      }
      file_type = OBJ_FILE_TYPE;
      break;
   case 1:
      if ((attr_ptr=FindAttrWithName(obj_ptr1, "type=", NULL)) != NULL &&
            strcmp(attr_ptr->attr_value.s, "tgif_pin") == 0) {
         if (FileNameHasExtension(new_file_name, OBJ_FILE_TYPE, NULL,
               &no_name)) {
            MsgBox(TgLoadString(STID_CANT_SAVE_OJB_ONE_SYM_FOUND), TOOL_NAME,
                  INFO_MB);
            return;
         } else if (FileNameHasExtension(new_file_name, SYM_FILE_TYPE, NULL,
               &no_name)) {
            MsgBox(TgLoadString(STID_CANT_SAVE_SYM_SAVE_AS_PIN), TOOL_NAME,
                  INFO_MB);
            return;
         } else if (FileNameHasExtension(new_file_name, PIN_FILE_TYPE, NULL,
               &no_name)) {
            /* don't modify */
         } else {
            strcat(new_file_name, pin_ext_str);
         }
         file_type = PIN_FILE_TYPE;
      } else {
         if (FileNameHasExtension(new_file_name, OBJ_FILE_TYPE, NULL,
               &no_name)) {
            MsgBox(TgLoadString(STID_CANT_SAVE_OJB_ONE_SYM_FOUND), TOOL_NAME,
                  INFO_MB);
            return;
         } else if (FileNameHasExtension(new_file_name, SYM_FILE_TYPE, NULL,
               &no_name)) {
            /* don't modify */
         } else if (FileNameHasExtension(new_file_name, PIN_FILE_TYPE, NULL,
               &no_name)) {
            MsgBox(TgLoadString(STID_CANT_SAVE_PIN_ONE_SYM_FOUND), TOOL_NAME,
                  INFO_MB);
            return;
         } else {
            strcat(new_file_name, sym_ext_str);
         }
         file_type = SYM_FILE_TYPE;
      }
      break;
   case 2:
      if (FileNameHasExtension(new_file_name, OBJ_FILE_TYPE, NULL, &no_name)) {
         MsgBox(TgLoadString(STID_CANT_SAVE_OJB_TWO_SYM_FOUND), TOOL_NAME,
               INFO_MB);
         return;
      } else if (FileNameHasExtension(new_file_name, SYM_FILE_TYPE, NULL,
            &no_name)) {
         MsgBox(TgLoadString(STID_CANT_SAVE_SYM_TWO_SYM_FOUND), TOOL_NAME,
               INFO_MB);
         return;
      } else if (FileNameHasExtension(new_file_name, PIN_FILE_TYPE, NULL,
            &no_name)) {
         /* don't modify */
      } else {
         strcat(new_file_name, pin_ext_str);
      }
      if (obj_ptr2->fattr != NULL) {
         obj_ptr = obj_ptr2;
         obj_ptr2 = obj_ptr1;
         obj_ptr1 = obj_ptr;
      }
      if ((attr_ptr=FindAttrWithName(obj_ptr1, "type=", NULL)) == NULL) {
         MsgBox(TgLoadString(STID_CANT_FIND_TYPE_ATTR_NOT_SAVED), TOOL_NAME,
               INFO_MB);
         return;
      }
      if (strcmp(attr_ptr->attr_value.s, "tgif_pin") != 0) {
         sprintf(gszMsgBox, TgLoadString(STID_SYM_TYPE_WRONG_PIN_NOT_SAVED),
               "tgif_pin");
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
         return;
      }
      file_type = PIN_FILE_TYPE;
      break;
   default: return;
   }
   if (no_name) {
      MsgBox(TgLoadString(STID_NO_FILE_NAME_FILE_NOT_SAVED), TOOL_NAME,
            INFO_MB);
      return;
   }
   if (*new_file_name == DIR_SEP) {
      strcpy(new_full_name, new_file_name);
   } else if (curDirIsLocal) {
      sprintf(new_full_name, "%s%c%s", curDir, DIR_SEP, new_file_name);
   } else {
      sprintf(new_full_name, "%s%c%s", curLocalDir, DIR_SEP, new_file_name);
   }
   if (file_type == OBJ_FILE_TYPE && gzipped) {
      char tmp_fname[MAXPATHLENGTH];

      if (MkTempFile(tmp_fname, sizeof(tmp_fname), tmpDir, TOOL_NAME) == NULL) {
         return;
      }
      strcpy(gzipped_fname, new_full_name);
      strcpy(new_full_name, tmp_fname);
   }
   if (!OkayToCreateFile((*gzipped_fname=='\0') ? new_full_name :
         gzipped_fname)) {
      if (*gzipped_fname != '\0') unlink(new_full_name);
      return;
   }
   if (*gzipped_fname == '\0') {
      if ((short_name=IsPrefix(bootDir, new_full_name, &rest))) ++rest;
   } else {
      if ((short_name=IsPrefix(bootDir, gzipped_fname, &rest))) ++rest;
   }
   if ((fp=fopen(new_full_name, "w")) == NULL) {
      if (short_name) {
         sprintf(gszMsgBox, TgLoadString(STID_CANT_OPEN_WRITE_FILE_NOT_SAVE),
               rest);
      } else {
         if (*gzipped_fname == '\0') {
            sprintf(gszMsgBox, TgLoadString(STID_CANT_OPEN_WRITE_FILE_NOT_SAVE),
                  new_full_name);
         } else {
            sprintf(gszMsgBox, TgLoadString(STID_CANT_OPEN_WRITE_FILE_NOT_SAVE),
                  gzipped_fname);
         }
      }
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      if (*gzipped_fname != '\0') unlink(new_full_name);
      return;
   }
   if (short_name) {
      sprintf(gszMsgBox, TgLoadString(STID_SAVING_DOTS), rest);
   } else {
      if (*gzipped_fname == '\0') {
         sprintf(gszMsgBox, TgLoadString(STID_SAVING_DOTS), new_full_name);
      } else {
         sprintf(gszMsgBox, TgLoadString(STID_SAVING_DOTS), gzipped_fname);
      }
   }
   Msg(gszMsgBox);

   if (!saveCommentsInSaveNew) CleanUpComments();
   watch_cursor = watchCursorOnMainWindow;
   if (!watch_cursor) {
      SetWatchCursor(drawWindow);
      SetWatchCursor(mainWindow);
   }
   writeFileFailed = FALSE;
   saved_cur_page = curPage;
   for (page_num=0, curPage=firstPage, count=1; curPage != NULL;
         curPage=curPage->next, page_num++) {
      if (pSpecifyPagesInfo->page_specified[page_num]) {
         int saved_cur_page_num=curPageNum, saved_last_page_num=lastPageNum;
         int need_to_restore_page_nums=FALSE;

         topObj = curPage->top;
         botObj = curPage->bot;
         if (count == 1) {
            curPageNum = 1;
            lastPageNum = pSpecifyPagesInfo->num_pages_specified;
            need_to_restore_page_nums = TRUE;
         }
         Save(fp, botObj, 0, count++);
         if (need_to_restore_page_nums) {
            curPageNum = saved_cur_page_num;
            lastPageNum = saved_last_page_num;
         }
      }
   }
   curPage = saved_cur_page;
   topObj = curPage->top;
   botObj = curPage->bot;
   fclose(fp);
   if (!watch_cursor) {
      SetDefaultCursor(mainWindow);
      ShowCursor(); 
   }
   if (writeFileFailed) {
      writeFileFailed = FALSE;
      FailToWriteFileMessage(new_full_name);
      ok = FALSE;
   } else {
      if (*gzipped_fname != '\0') {
         if (!GzipFile(new_full_name, gzipped_fname)) {
            sprintf(gszMsgBox, TgLoadString(STID_PROBLEM_ZIPPING_PLEASE_SAVE),
                  new_full_name, gzipped_fname, new_full_name);
            MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
            ok = FALSE;
         }
         unlink(new_full_name);
         strcpy(new_full_name, gzipped_fname);
      }
      if (ok) {
         if (pSpecifyPagesInfo->num_pages_specified > 1) {
            sprintf(gszMsgBox, TgLoadString(STID_MANY_PAGES_SAVED_INTO_GIVEN),
                  pSpecifyPagesInfo->num_pages_specified,
                  (short_name ? rest : new_full_name));
         } else {
            sprintf(gszMsgBox, TgLoadString(STID_ONE_PAGE_SAVED_INTO_GIVEN),
                  (short_name ? rest : new_full_name));
         }
         Msg(gszMsgBox);
      } else {
         sprintf(gszMsgBox, TgLoadString(STID_FILE_NOT_SAVED),
               (short_name ? rest : new_full_name));
         Msg(gszMsgBox);
      }
   }
}

char *ParseStr(Str, C, Left, LeftSz)
   char *Str, *Left;
   int C, LeftSz;
{
   register char *s=Str, *l=Left;
   register int len=0;
   int max_len=LeftSz-1;
   char the_char=(char)C;

   while (*s != '\0' && *s != the_char) {
      if (len < max_len) {
         *l++ = *s++;
         len++;
      } else {
         break;
      }
   }

   if (*s == the_char) s++;
   *l = '\0';

   while (len >= 2 && *Left == '\'' && *(--l) == '\'') {
      char *c_ptr, *c_ptr1;

      *l-- = '\0';
      len -= 2;
      c_ptr = &Left[1];
      c_ptr1 = Left;
      while (*c_ptr != '\0') *c_ptr1++ = *c_ptr++;
      *c_ptr1 = '\0';
   }
   return s;
}

char *FindChar(C, Str)
   int C;
   char *Str;
   /* returns the address of the character right after C of the string Str */
{
   register char *s=Str, the_char=(char)C;

   while (*s != '\0' && *s != the_char) s++;

   if (*s == the_char) s++;
   return s;
}

void SaveCreatorID(FP, obj_ptr, psz_prefix)
   FILE *FP;
   struct ObjRec *obj_ptr;
   char *psz_prefix;
{
   char buf[MAXSTRING];

   if (obj_ptr->creator_full_id == NULL) {
      sprintf(buf, "%1d/%s", obj_ptr->id, gszLocalPID);
   } else {
      strcpy(buf, obj_ptr->creator_full_id);
   }
   if (fprintf(FP, "\n%s\"", psz_prefix) == EOF) writeFileFailed = TRUE;
   SaveString(FP, buf);
   if (fprintf(FP, "\",") == EOF) writeFileFailed = TRUE;
}

int ReadCreatorID(FP, ObjPtr)
   FILE *FP;
   struct ObjRec **ObjPtr;
{
   int ok=TRUE;
   char *c_ptr=NULL, *line=NULL, full_id[MAXSTRING];

   if ((line=UtilGetALine(FP)) == NULL) {
      sprintf(gszMsgBox, TgLoadString(STID_UNEXPECTED_EOF_IN_ABORT_READ),
            scanFileName, scanLineNum, "ReadCreatorID()");
      if (PRTGIF) {
         fprintf(stderr, "%s\n", gszMsgBox);
      } else {
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      }
      return FALSE;
   }
   scanLineNum++;
   c_ptr = FindChar((int)'"', line);
   c_ptr = ParseStr(c_ptr, (int)'"', full_id, sizeof(full_id));
   if (c_ptr == NULL) {
      sprintf(gszMsgBox, TgLoadString(STID_BAD_FIELD_IN_FUNC_ABORT_READ),
            scanFileName, scanLineNum, "creator_full_id", "ReadCreatorID()");
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      ok = FALSE;
   }
   (*ObjPtr)->creator_full_id = UtilStrDup(full_id);
   if ((*ObjPtr)->creator_full_id == NULL) FailAllocMessage();
   free(line);
   return ok;
}

static
int NumFieldsToFontStr(buf)
   char *buf;
   /* 300.000,0,0,0,16,1,0,1,1,0,0,1,0,1,0,'...' */
{
   char *psz=NULL, *psz_single_quote=strchr(buf, '\'');
   int count=0;

   if (psz_single_quote == NULL) {
      return (-1);
   }
   *psz_single_quote = '\0';
   for (psz=strchr(buf, ','); psz != NULL; psz=strchr(&psz[1], ',')) {
      count++;
   }
   *psz_single_quote = '\'';

   return count;
}

static int gnCannotFindColorMsg=FALSE;

static
int ReadState(Inbuf)
   char *Inbuf;
{
   char *s, font_str[MAXSTRING], sb_font_str[MAXSTRING];
   int page_style, forced_use_gray=FALSE, compat_dpi, font_sz=0, sz_unit=0;
   int page_arg1=0, page_arg2=0, rotate=ROTATE0;
   int one_page_width=0, one_page_height=0;

   *font_str = *sb_font_str = '\0';
   s = FindChar((int)'(', Inbuf);
   if (sscanf(s, "%d", &page_style) != 1) return FALSE;
   s = FindChar((int)',', s);
   if (*s == '\0') {
      fileVersion = INVALID;
   } else if (sscanf(s, "%d", &fileVersion) != 1) {
      return FALSE;
   }

   if (fileVersion > CUR_VERSION) return FALSE;

   gnCannotFindColorMsg = FALSE;

   if (!importingFile && !mergingFile) {
      if (fileVersion <= 13) {
         switch (page_style) {
         case PORTRAIT: printMag = (float)100.0; break;
         case LANDSCAPE: printMag = (float)100.0; break;
         case HIGHPORT: printMag = (float)50.0; page_style = PORTRAIT; break;
         case HIGHLAND: printMag = (float)50.0; page_style = LANDSCAPE; break;
         case SLIDEPORT: printMag = (float)200.0; page_style = PORTRAIT; break;
         case SLIDELAND: printMag = (float)200.0; page_style = LANDSCAPE; break;
         default:
            sprintf(gszMsgBox, TgLoadString(STID_UNRECOG_PAGE_STYLE),
                  page_style);
            TwoLineMsg(gszMsgBox, TgLoadString(STID_PORTRAIT_STYLE_ASSUMED));
            page_style = PORTRAIT;
            printMag = (float)100.0;
            break;
         }
      } else {
         int num_fields_to_font_str=0;

         if (page_style != PORTRAIT && page_style != LANDSCAPE) {
            sprintf(gszMsgBox, TgLoadString(STID_UNRECOG_PAGE_STYLE),
                  page_style);
            TwoLineMsg(gszMsgBox, TgLoadString(STID_PORTRAIT_STYLE_ASSUMED));
            page_style = PORTRAIT;
         }
         s = FindChar((int)',', s);
         /* begin locale bug fix kluge */
         num_fields_to_font_str = NumFieldsToFontStr(s);
         if (num_fields_to_font_str == (-1)) {
             /* cannot find single quote */
         } else if (num_fields_to_font_str == 15) {
             /* normal case */
         } else if (num_fields_to_font_str == 16) {
             /* extra one, must be the locale bug, fix it! */
             char *psz_comma=strchr(s, ',');

             *psz_comma = '.';
         } else {
             /* don't know what happened */
         }
         /* end locale bug fix kluge */
         sscanf(s, "%f", &printMag);
      }
      pageStyle = page_style;
   }

   if (PRTGIF && useGray) forced_use_gray = TRUE;
   if (importingFile || mergingFile) return TRUE;

   page_arg1 = page_arg2 = 1;
   one_page_width = onePageWidth;
   one_page_height = onePageHeight;

   if (fileVersion >= 2) {
      compat_dpi = FONT_DPI_75;
      curDash = 0;
      gridSystem = ENGLISH_GRID;
      xyMetricGrid = DEFAULT_METRIC_GRID;
      textVSpace = 0;
      zoomedIn = FALSE;
      rcbRadius = DEF_RCB_RADIUS;
      pageLayoutMode = PAGE_STACK;
      paperCol = paperRow = 1;
      curPageNum = lastPageNum = 1;
      pageLineShownInTileMode = TRUE;

      if (usePaperSizeStoredInFile) ResetOnePageSize();

      s = FindChar((int)',', s);

      InitScan(s, "\t\n, ");

      if (fileVersion <= 3) {
         GETINT("state", drawOrigX,     "X Draw Origin");
         GETINT("state", drawOrigY,     "Y Draw Origin");
         GETINT("state", zoomScale,     "Zoom scale");
         GETINT("state", xyEnglishGrid, "English Grid");
         GETINT("state", snapOn,        "Grid");
         GETINT("state", colorIndex,    "Color");
         GETINT("state", horiAlign,     "Horizontal Align");
         GETINT("state", vertAlign,     "Vertical Align");
         GETINT("state", lineWidth,     "Line Width");
         GETINT("state", lineStyle,     "Line Style");
         GETINT("state", objFill,       "Fill Pattern");
         GETINT("state", penPat,        "Pen Pattern");
         GETINT("state", textJust,      "Text Justify");
         GETINT("state", curFont,       "Font Name");
         GETINT("state", curStyle,      "Font Style");
         GETINT("state", font_sz,       "Font Size");

         if (lineWidth == LINE_CURVED) {
            lineWidth = 0;
            curSpline = LT_SPLINE;
         } else {
            curSpline = LT_STRAIGHT;
         }
      } else if (fileVersion <= 7) {
         GETINT("state", drawOrigX,     "X Draw Origin");
         GETINT("state", drawOrigY,     "Y Draw Origin");
         GETINT("state", zoomScale,     "Zoom scale");
         GETINT("state", xyEnglishGrid, "English Grid");
         GETINT("state", snapOn,        "Grid");
         GETINT("state", colorIndex,    "Color");
         GETINT("state", horiAlign,     "Horizontal Align");
         GETINT("state", vertAlign,     "Vertical Align");
         GETINT("state", lineWidth,     "Line Width");
         GETINT("state", curSpline,     "Spline");
         GETINT("state", lineStyle,     "Line Style");
         GETINT("state", objFill,       "Fill Pattern");
         GETINT("state", penPat,        "Pen Pattern");
         GETINT("state", textJust,      "Text Justify");
         GETINT("state", curFont,       "Font Name");
         GETINT("state", curStyle,      "Font Style");
         GETINT("state", font_sz,       "Font Size");
      } else if (fileVersion <= 8) {
         GETINT("state", drawOrigX,     "X Draw Origin");
         GETINT("state", drawOrigY,     "Y Draw Origin");
         GETINT("state", zoomScale,     "Zoom scale");
         GETINT("state", xyEnglishGrid, "English Grid");
         GETINT("state", snapOn,        "Grid");
         GETINT("state", colorIndex,    "Color");
         GETINT("state", horiAlign,     "Horizontal Align");
         GETINT("state", vertAlign,     "Vertical Align");
         GETINT("state", lineWidth,     "Line Width");
         GETINT("state", curSpline,     "Spline");
         GETINT("state", lineStyle,     "Line Style");
         GETINT("state", objFill,       "Fill Pattern");
         GETINT("state", penPat,        "Pen Pattern");
         GETINT("state", textJust,      "Text Justify");
         GETINT("state", curFont,       "Font Name");
         GETINT("state", curStyle,      "Font Style");
         GETINT("state", font_sz,       "Font Size");
         GETINT("state", compat_dpi,    "Font DPI");
      } else if (fileVersion <= 11) {
         GETINT("state", drawOrigX,     "X Draw Origin");
         GETINT("state", drawOrigY,     "Y Draw Origin");
         GETINT("state", zoomScale,     "Zoom scale");
         GETINT("state", xyEnglishGrid, "English Grid");
         GETINT("state", snapOn,        "Grid");
         GETINT("state", colorIndex,    "Color");
         GETINT("state", horiAlign,     "Horizontal Align");
         GETINT("state", vertAlign,     "Vertical Align");
         GETINT("state", lineWidth,     "Line Width");
         GETINT("state", curSpline,     "Spline");
         GETINT("state", lineStyle,     "Line Style");
         GETINT("state", objFill,       "Fill Pattern");
         GETINT("state", penPat,        "Pen Pattern");
         GETINT("state", textJust,      "Text Justify");
         GETINT("state", curFont,       "Font Name");
         GETINT("state", curStyle,      "Font Style");
         GETINT("state", font_sz,       "Font Size");
         GETINT("state", compat_dpi,    "Font DPI");
         GETINT("state", curDash,       "Dash Style");
      } else if (fileVersion <= 12) {
         GETINT("state", drawOrigX,     "X Draw Origin");
         GETINT("state", drawOrigY,     "Y Draw Origin");
         GETINT("state", zoomScale,     "Zoom scale");
         GETINT("state", xyEnglishGrid, "English Grid");
         GETINT("state", snapOn,        "Grid");
         GETINT("state", colorIndex,    "Color");
         GETINT("state", horiAlign,     "Horizontal Align");
         GETINT("state", vertAlign,     "Vertical Align");
         GETINT("state", lineWidth,     "Line Width");
         GETINT("state", curSpline,     "Spline");
         GETINT("state", lineStyle,     "Line Style");
         GETINT("state", objFill,       "Fill Pattern");
         GETINT("state", penPat,        "Pen Pattern");
         GETINT("state", textJust,      "Text Justify");
         GETINT("state", curFont,       "Font Name");
         GETINT("state", curStyle,      "Font Style");
         GETINT("state", font_sz,       "Font Size");
         GETINT("state", compat_dpi,    "Font DPI");
         GETINT("state", curDash,       "Dash Style");
         GETINT("state", gridSystem,    "Grid System");
         GETINT("state", xyMetricGrid,  "Metric Grid");
      } else if (fileVersion <= 18) {
         GETINT("state", drawOrigX,     "X Draw Origin");
         GETINT("state", drawOrigY,     "Y Draw Origin");
         GETINT("state", zoomScale,     "Zoom scale");
         GETINT("state", xyEnglishGrid, "English Grid");
         GETINT("state", snapOn,        "Grid");
         GETINT("state", colorIndex,    "Color");
         GETINT("state", horiAlign,     "Horizontal Align");
         GETINT("state", vertAlign,     "Vertical Align");
         GETINT("state", lineWidth,     "Line Width");
         GETINT("state", curSpline,     "Spline");
         GETINT("state", lineStyle,     "Line Style");
         GETINT("state", objFill,       "Fill Pattern");
         GETINT("state", penPat,        "Pen Pattern");
         GETINT("state", textJust,      "Text Justify");
         GETINT("state", curFont,       "Font Name");
         GETINT("state", curStyle,      "Font Style");
         GETINT("state", font_sz,       "Font Size");
         GETINT("state", compat_dpi,    "Font DPI");
         GETINT("state", curDash,       "Dash Style");
         GETINT("state", gridSystem,    "Grid System");
         GETINT("state", xyMetricGrid,  "Metric Grid");
         GETINT("state", textVSpace,    "Text Vertical Spacing");
      } else if (fileVersion <= 19) {
         GETINT("state", drawOrigX,     "X Draw Origin");
         GETINT("state", drawOrigY,     "Y Draw Origin");
         GETINT("state", zoomScale,     "Zoom scale");
         GETINT("state", xyEnglishGrid, "English Grid");
         GETINT("state", snapOn,        "Grid");
         GETINT("state", colorIndex,    "Color");
         GETINT("state", horiAlign,     "Horizontal Align");
         GETINT("state", vertAlign,     "Vertical Align");
         GETINT("state", lineWidth,     "Line Width");
         GETINT("state", curSpline,     "Spline");
         GETINT("state", lineStyle,     "Line Style");
         GETINT("state", objFill,       "Fill Pattern");
         GETINT("state", penPat,        "Pen Pattern");
         GETINT("state", textJust,      "Text Justify");
         GETINT("state", curFont,       "Font Name");
         GETINT("state", curStyle,      "Font Style");
         GETINT("state", font_sz,       "Font Size");
         GETINT("state", compat_dpi,    "Font DPI");
         GETINT("state", curDash,       "Dash Style");
         GETINT("state", gridSystem,    "Grid System");
         GETINT("state", xyMetricGrid,  "Metric Grid");
         GETINT("state", textVSpace,    "Text Vertical Spacing");
         GETINT("state", zoomedIn,      "Zoomed In");
      } else if (fileVersion <= 21) {
         GETINT("state", drawOrigX,     "X Draw Origin");
         GETINT("state", drawOrigY,     "Y Draw Origin");
         GETINT("state", zoomScale,     "Zoom scale");
         GETINT("state", xyEnglishGrid, "English Grid");
         GETINT("state", snapOn,        "Grid");
         GETINT("state", colorIndex,    "Color");
         GETINT("state", horiAlign,     "Horizontal Align");
         GETINT("state", vertAlign,     "Vertical Align");
         GETINT("state", lineWidth,     "Line Width");
         GETINT("state", curSpline,     "Spline");
         GETINT("state", lineStyle,     "Line Style");
         GETINT("state", objFill,       "Fill Pattern");
         GETINT("state", penPat,        "Pen Pattern");
         GETINT("state", textJust,      "Text Justify");
         GETINT("state", curFont,       "Font Name");
         GETINT("state", curStyle,      "Font Style");
         GETINT("state", font_sz,       "Font Size");
         GETINT("state", compat_dpi,    "Font DPI");
         GETINT("state", curDash,       "Dash Style");
         GETINT("state", gridSystem,    "Grid System");
         GETINT("state", xyMetricGrid,  "Metric Grid");
         GETINT("state", textVSpace,    "Text Vertical Spacing");
         GETINT("state", zoomedIn,      "Zoomed In");
         GETINT("state", gridShown,     "Grid Shown");
         GETINT("state", moveMode,      "Move Mode");
      } else if (fileVersion <= 26) {
         GETINT("state", drawOrigX,     "X Draw Origin");
         GETINT("state", drawOrigY,     "Y Draw Origin");
         GETINT("state", zoomScale,     "Zoom scale");
         GETINT("state", xyEnglishGrid, "English Grid");
         GETINT("state", snapOn,        "Grid");
         GETINT("state", colorIndex,    "Color");
         GETINT("state", horiAlign,     "Horizontal Align");
         GETINT("state", vertAlign,     "Vertical Align");
         GETINT("state", lineWidth,     "Line Width");
         GETINT("state", curSpline,     "Spline");
         GETINT("state", lineStyle,     "Line Style");
         GETINT("state", objFill,       "Fill Pattern");
         GETINT("state", penPat,        "Pen Pattern");
         GETINT("state", textJust,      "Text Justify");
         GETINT("state", curFont,       "Font Name");
         GETINT("state", curStyle,      "Font Style");
         GETINT("state", font_sz,       "Font Size");
         GETINT("state", compat_dpi,    "Font DPI");
         GETINT("state", curDash,       "Dash Style");
         GETINT("state", gridSystem,    "Grid System");
         GETINT("state", xyMetricGrid,  "Metric Grid");
         GETINT("state", textVSpace,    "Text Vertical Spacing");
         GETINT("state", zoomedIn,      "Zoomed In");
         GETINT("state", gridShown,     "Grid Shown");
         GETINT("state", moveMode,      "Move Mode");
         GETINT("state", rotate,        "Text Rotation");
         GETINT("state", rcbRadius,     "RCBox Radius");
      } else if (fileVersion <= 27) {
         GETINT("state", drawOrigX,     "X Draw Origin");
         GETINT("state", drawOrigY,     "Y Draw Origin");
         GETINT("state", zoomScale,     "Zoom scale");
         GETINT("state", xyEnglishGrid, "English Grid");
         GETINT("state", snapOn,        "Grid");
         GETINT("state", colorIndex,    "Color");
         GETINT("state", horiAlign,     "Horizontal Align");
         GETINT("state", vertAlign,     "Vertical Align");
         GETINT("state", lineWidth,     "Line Width");
         GETINT("state", curSpline,     "Spline");
         GETINT("state", lineStyle,     "Line Style");
         GETINT("state", objFill,       "Fill Pattern");
         GETINT("state", penPat,        "Pen Pattern");
         GETINT("state", textJust,      "Text Justify");
         GETINT("state", curFont,       "Font Name");
         GETINT("state", curStyle,      "Font Style");
         GETINT("state", font_sz,       "Font Size");
         GETINT("state", compat_dpi,    "Font DPI");
         GETINT("state", curDash,       "Dash Style");
         GETINT("state", gridSystem,    "Grid System");
         GETINT("state", xyMetricGrid,  "Metric Grid");
         GETINT("state", textVSpace,    "Text Vertical Spacing");
         GETINT("state", zoomedIn,      "Zoomed In");
         GETINT("state", gridShown,     "Grid Shown");
         GETINT("state", moveMode,      "Move Mode");
         GETINT("state", rotate,        "Text Rotation");
         GETINT("state", rcbRadius,     "RCBox Radius");
         GETINT("state", useGray,       "Use Gray Scale");
      } else if (fileVersion <= 28) {
         /* Matsuda's Version */
         GETINT("state", drawOrigX,     "X Draw Origin");
         GETINT("state", drawOrigY,     "Y Draw Origin");
         GETINT("state", zoomScale,     "Zoom scale");
         GETINT("state", xyEnglishGrid, "English Grid");
         GETINT("state", snapOn,        "Grid");
         GETINT("state", colorIndex,    "Color");
         GETINT("state", horiAlign,     "Horizontal Align");
         GETINT("state", vertAlign,     "Vertical Align");
         GETINT("state", lineWidth,     "Line Width");
         GETINT("state", curSpline,     "Spline");
         GETINT("state", lineStyle,     "Line Style");
         GETINT("state", objFill,       "Fill Pattern");
         GETINT("state", penPat,        "Pen Pattern");
         GETINT("state", textJust,      "Text Justify");
         GETINT("state", curFont,       "Font Name");
         GETINT("state", curStyle,      "Font Style");
         GETINT("state", font_sz,       "Font Size");
         GETINT("state", compat_dpi,    "Font DPI");
         GETINT("state", curDash,       "Dash Style");
         GETINT("state", gridSystem,    "Grid System");
         GETINT("state", xyMetricGrid,  "Metric Grid");
         GETINT("state", textVSpace,    "Text Vertical Spacing");
         GETINT("state", zoomedIn,      "Zoomed In");
         GETINT("state", gridShown,     "Grid Shown");
         GETINT("state", moveMode,      "Move Mode");
         GETINT("state", rotate,        "Text Rotation");
         GETINT("state", rcbRadius,     "RCBox Radius");
         GETINT("state", useGray,       "Use Gray Scale");
         GETINT("state", paperCol,      "Num Columns");
         GETINT("state", paperRow,      "Num Rows");
         GETINT("state", curPageNum,    "Current Page Number");
         GETINT("state", lastPageNum,   "Last Page Number");
      } else if (fileVersion <= 29) {
         GETINT("state", drawOrigX,     "X Draw Origin");
         GETINT("state", drawOrigY,     "Y Draw Origin");
         GETINT("state", zoomScale,     "Zoom scale");
         GETINT("state", xyEnglishGrid, "English Grid");
         GETINT("state", snapOn,        "Grid");
         GETINT("state", colorIndex,    "Color");
         GETINT("state", horiAlign,     "Horizontal Align");
         GETINT("state", vertAlign,     "Vertical Align");
         GETINT("state", lineWidth,     "Line Width");
         GETINT("state", curSpline,     "Spline");
         GETINT("state", lineStyle,     "Line Style");
         GETINT("state", objFill,       "Fill Pattern");
         GETINT("state", penPat,        "Pen Pattern");
         GETINT("state", textJust,      "Text Justify");
         GETINT("state", curFont,       "Font Name");
         GETINT("state", curStyle,      "Font Style");
         GETINT("state", font_sz,       "Font Size");
         GETINT("state", compat_dpi,    "Font DPI");
         GETINT("state", curDash,       "Dash Style");
         GETINT("state", gridSystem,    "Grid System");
         GETINT("state", xyMetricGrid,  "Metric Grid");
         GETINT("state", textVSpace,    "Text Vertical Spacing");
         GETINT("state", zoomedIn,      "Zoomed In");
         GETINT("state", gridShown,     "Grid Shown");
         GETINT("state", moveMode,      "Move Mode");
         GETINT("state", rotate,        "Text Rotation");
         GETINT("state", rcbRadius,     "RCBox Radius");
         GETINT("state", useGray,       "Use Gray Scale");
         GETINT("state", pageLayoutMode,"Page Layout Mode");
         GETINT("state", page_arg1,     "Page Layout Subarg 1");
         GETINT("state", page_arg2,     "Page Layout Subarg 2");
         GETINT("state", pageLineShownInTileMode,"Page Lines Shown");
      } else if (fileVersion <= 30) {
         GETINT("state", drawOrigX,     "X Draw Origin");
         GETINT("state", drawOrigY,     "Y Draw Origin");
         GETINT("state", zoomScale,     "Zoom scale");
         GETINT("state", xyEnglishGrid, "English Grid");
         GETINT("state", snapOn,        "Grid");
         GETINT("state", colorIndex,    "Color");
         GETINT("state", horiAlign,     "Horizontal Align");
         GETINT("state", vertAlign,     "Vertical Align");
         GETINT("state", lineWidth,     "Line Width");
         GETINT("state", curSpline,     "Spline");
         GETINT("state", lineStyle,     "Line Style");
         GETINT("state", objFill,       "Fill Pattern");
         GETINT("state", penPat,        "Pen Pattern");
         GETINT("state", textJust,      "Text Justify");
         GETSTR("state", font_str,      "Font Name String");
         GETINT("state", curStyle,      "Font Style");
         GETINT("state", font_sz,       "Font Size");
         GETINT("state", compat_dpi,    "Font DPI");
         GETINT("state", curDash,       "Dash Style");
         GETINT("state", gridSystem,    "Grid System");
         GETINT("state", xyMetricGrid,  "Metric Grid");
         GETINT("state", textVSpace,    "Text Vertical Spacing");
         GETINT("state", zoomedIn,      "Zoomed In");
         GETINT("state", gridShown,     "Grid Shown");
         GETINT("state", moveMode,      "Move Mode");
         GETINT("state", rotate,        "Text Rotation");
         GETINT("state", rcbRadius,     "RCBox Radius");
         GETINT("state", useGray,       "Use Gray Scale");
         GETINT("state", pageLayoutMode,"Page Layout Mode");
         GETINT("state", page_arg1,     "Page Layout Subarg 1");
         GETINT("state", page_arg2,     "Page Layout Subarg 2");
         GETINT("state", pageLineShownInTileMode,"Page Lines Shown");
      } else if (fileVersion <= 31) {
         GETINT("state", drawOrigX,     "X Draw Origin");
         GETINT("state", drawOrigY,     "Y Draw Origin");
         GETINT("state", zoomScale,     "Zoom scale");
         GETINT("state", xyEnglishGrid, "English Grid");
         GETINT("state", snapOn,        "Grid");
         GETINT("state", colorIndex,    "Color");
         GETINT("state", horiAlign,     "Horizontal Align");
         GETINT("state", vertAlign,     "Vertical Align");
         GETINT("state", lineWidth,     "Line Width");
         GETINT("state", curSpline,     "Spline");
         GETINT("state", lineStyle,     "Line Style");
         GETINT("state", objFill,       "Fill Pattern");
         GETINT("state", penPat,        "Pen Pattern");
         GETINT("state", textJust,      "Text Justify");
         GETSTR("state", font_str,      "Font Name String");
         GETINT("state", curStyle,      "Font Style");
         GETINT("state", font_sz,       "Font Size");
         GETINT("state", compat_dpi,    "Font DPI");
         GETINT("state", curDash,       "Dash Style");
         GETINT("state", gridSystem,    "Grid System");
         GETINT("state", xyMetricGrid,  "Metric Grid");
         GETINT("state", textVSpace,    "Text Vertical Spacing");
         GETINT("state", zoomedIn,      "Zoomed In");
         GETINT("state", gridShown,     "Grid Shown");
         GETINT("state", moveMode,      "Move Mode");
         GETINT("state", rotate,        "Text Rotation");
         GETINT("state", rcbRadius,     "RCBox Radius");
         GETINT("state", useGray,       "Use Gray Scale");
         GETINT("state", pageLayoutMode,"Page Layout Mode");
         GETINT("state", page_arg1,     "Page Layout Subarg 1");
         GETINT("state", page_arg2,     "Page Layout Subarg 2");
         GETINT("state", pageLineShownInTileMode,"Page Lines Shown");
         GETINT("state", colorDump,     "Print In Color");
      } else if (fileVersion <= 32) {
         GETINT("state", drawOrigX,      "X Draw Origin");
         GETINT("state", drawOrigY,      "Y Draw Origin");
         GETINT("state", zoomScale,      "Zoom scale");
         GETINT("state", xyEnglishGrid,  "English Grid");
         GETINT("state", snapOn,         "Grid");
         GETINT("state", colorIndex,     "Color");
         GETINT("state", horiAlign,      "Horizontal Align");
         GETINT("state", vertAlign,      "Vertical Align");
         GETINT("state", lineWidth,      "Line Width");
         GETINT("state", curSpline,      "Spline");
         GETINT("state", lineStyle,      "Line Style");
         GETINT("state", objFill,        "Fill Pattern");
         GETINT("state", penPat,         "Pen Pattern");
         GETINT("state", textJust,       "Text Justify");
         GETSTR("state", font_str,       "Font Name String");
         GETINT("state", curStyle,       "Font Style");
         GETINT("state", font_sz,        "Font Size");
         GETINT("state", compat_dpi,     "Font DPI");
         GETINT("state", curDash,        "Dash Style");
         GETINT("state", gridSystem,     "Grid System");
         GETINT("state", xyMetricGrid,   "Metric Grid");
         GETINT("state", textVSpace,     "Text Vertical Spacing");
         GETINT("state", zoomedIn,       "Zoomed In");
         GETINT("state", gridShown,      "Grid Shown");
         GETINT("state", moveMode,       "Move Mode");
         GETINT("state", rotate,         "Text Rotation");
         GETINT("state", rcbRadius,      "RCBox Radius");
         GETINT("state", useGray,        "Use Gray Scale");
         GETINT("state", pageLayoutMode, "Page Layout Mode");
         GETINT("state", page_arg1,      "Page Layout Subarg 1");
         GETINT("state", page_arg2,      "Page Layout Subarg 2");
         GETINT("state", pageLineShownInTileMode,"Page Lines Shown");
         GETINT("state", colorDump,      "Print In Color");
         GETINT("state", one_page_width, "One Page Width");
         GETINT("state", one_page_height,"One Page Height");
      } else if (fileVersion <= 34) {
         GETINT("state", drawOrigX,      "X Draw Origin");
         GETINT("state", drawOrigY,      "Y Draw Origin");
         GETINT("state", zoomScale,      "Zoom scale");
         GETINT("state", xyEnglishGrid,  "English Grid");
         GETINT("state", snapOn,         "Grid");
         GETINT("state", colorIndex,     "Color");
         GETINT("state", horiAlign,      "Horizontal Align");
         GETINT("state", vertAlign,      "Vertical Align");
         GETINT("state", lineWidth,      "Line Width");
         GETINT("state", curSpline,      "Spline");
         GETINT("state", lineStyle,      "Line Style");
         GETINT("state", objFill,        "Fill Pattern");
         GETINT("state", penPat,         "Pen Pattern");
         GETINT("state", textJust,       "Text Justify");
         GETSTR("state", font_str,       "Font Name String");
         GETINT("state", curStyle,       "Font Style");
         GETINT("state", font_sz,        "Font Size");
         GETINT("state", compat_dpi,     "Font DPI");
         GETINT("state", curDash,        "Dash Style");
         GETINT("state", gridSystem,     "Grid System");
         GETINT("state", xyMetricGrid,   "Metric Grid");
         GETINT("state", textVSpace,     "Text Vertical Spacing");
         GETINT("state", zoomedIn,       "Zoomed In");
         GETINT("state", gridShown,      "Grid Shown");
         GETINT("state", moveMode,       "Move Mode");
         GETINT("state", rotate,         "Text Rotation");
         GETINT("state", rcbRadius,      "RCBox Radius");
         GETINT("state", useGray,        "Use Gray Scale");
         GETINT("state", pageLayoutMode, "Page Layout Mode");
         GETINT("state", page_arg1,      "Page Layout Subarg 1");
         GETINT("state", page_arg2,      "Page Layout Subarg 2");
         GETINT("state", pageLineShownInTileMode,"Page Lines Shown");
         GETINT("state", colorDump,      "Print In Color");
         GETINT("state", one_page_width, "One Page Width");
         GETINT("state", one_page_height,"One Page Height");
         GETINT("state", stretchableText,"Stretchable Text");
         GETINT("state", textRotation,   "Text Rotation");
         GETINT("state", rotationIncrement,"Rotation Increment");
      } else if (fileVersion <= 35) {
         GETINT("state", drawOrigX,      "X Draw Origin");
         GETINT("state", drawOrigY,      "Y Draw Origin");
         GETINT("state", zoomScale,      "Zoom scale");
         GETINT("state", xyEnglishGrid,  "English Grid");
         GETINT("state", snapOn,         "Grid");
         GETINT("state", colorIndex,     "Color");
         GETINT("state", horiAlign,      "Horizontal Align");
         GETINT("state", vertAlign,      "Vertical Align");
         GETINT("state", lineWidth,      "Line Width");
         GETINT("state", curSpline,      "Spline");
         GETINT("state", lineStyle,      "Line Style");
         GETINT("state", objFill,        "Fill Pattern");
         GETINT("state", penPat,         "Pen Pattern");
         GETINT("state", textJust,       "Text Justify");
         GETSTR("state", font_str,       "Font Name String");
         GETINT("state", curStyle,       "Font Style");
         GETINT("state", font_sz,        "Font Size");
         GETINT("state", compat_dpi,     "Font DPI");
         GETINT("state", curDash,        "Dash Style");
         GETINT("state", gridSystem,     "Grid System");
         GETINT("state", xyMetricGrid,   "Metric Grid");
         GETINT("state", textVSpace,     "Text Vertical Spacing");
         GETINT("state", zoomedIn,       "Zoomed In");
         GETINT("state", gridShown,      "Grid Shown");
         GETINT("state", moveMode,       "Move Mode");
         GETINT("state", rotate,         "Text Rotation");
         GETINT("state", rcbRadius,      "RCBox Radius");
         GETINT("state", useGray,        "Use Gray Scale");
         GETINT("state", pageLayoutMode, "Page Layout Mode");
         GETINT("state", page_arg1,      "Page Layout Subarg 1");
         GETINT("state", page_arg2,      "Page Layout Subarg 2");
         GETINT("state", pageLineShownInTileMode,"Page Lines Shown");
         GETINT("state", colorDump,      "Print In Color");
         GETINT("state", one_page_width, "One Page Width");
         GETINT("state", one_page_height,"One Page Height");
         GETINT("state", stretchableText,"Stretchable Text");
         GETINT("state", textRotation,   "Text Rotation");
         GETINT("state", rotationIncrement,"Rotation Increment");
         GETINT("state", transPat,       "Pattern Transparency");
      } else {
         GETINT("state", drawOrigX,      "X Draw Origin");
         GETINT("state", drawOrigY,      "Y Draw Origin");
         GETINT("state", zoomScale,      "Zoom scale");
         GETINT("state", xyEnglishGrid,  "English Grid");
         GETINT("state", snapOn,         "Grid");
         GETINT("state", colorIndex,     "Color");
         GETINT("state", horiAlign,      "Horizontal Align");
         GETINT("state", vertAlign,      "Vertical Align");
         GETINT("state", lineWidth,      "Line Width");
         GETINT("state", curSpline,      "Spline");
         GETINT("state", lineStyle,      "Line Style");
         GETINT("state", objFill,        "Fill Pattern");
         GETINT("state", penPat,         "Pen Pattern");
         GETINT("state", textJust,       "Text Justify");
         GETSTR("state", font_str,       "Font Name String");
         GETINT("state", curStyle,       "Font Style");
         GETINT("state", sz_unit,        "Font Size Unit");
         GETINT("state", compat_dpi,     "Font DPI");
         GETINT("state", curDash,        "Dash Style");
         GETINT("state", gridSystem,     "Grid System");
         GETINT("state", xyMetricGrid,   "Metric Grid");
         GETINT("state", textVSpace,     "Text Vertical Spacing");
         GETINT("state", zoomedIn,       "Zoomed In");
         GETINT("state", gridShown,      "Grid Shown");
         GETINT("state", moveMode,       "Move Mode");
         GETINT("state", rotate,         "Text Rotation");
         GETINT("state", rcbRadius,      "RCBox Radius");
         GETINT("state", useGray,        "Use Gray Scale");
         GETINT("state", pageLayoutMode, "Page Layout Mode");
         GETINT("state", page_arg1,      "Page Layout Subarg 1");
         GETINT("state", page_arg2,      "Page Layout Subarg 2");
         GETINT("state", pageLineShownInTileMode,"Page Lines Shown");
         GETINT("state", colorDump,      "Print In Color");
         GETINT("state", one_page_width, "One Page Width");
         GETINT("state", one_page_height,"One Page Height");
         GETINT("state", stretchableText,"Stretchable Text");
         GETINT("state", textRotation,   "Text Rotation");
         GETINT("state", rotationIncrement,"Rotation Increment");
         GETINT("state", transPat,       "Pattern Transparency");
      }
      if (fileVersion <= 28) readingPageNum++;
      if (fileVersion <= 29) {
         VerifyCompatibleFontIndex(&curFont);
         font_sz = GetCompatibleSize(compat_dpi, font_sz);
         curSzUnit = FontSizeToSzUnit(font_sz);
         if (PRTGIF) {
            PrTgifInitBaseFonts();
         }
      } else {
         char *psz=NULL;
         int len=strlen(font_str);

         if (fileVersion <= 35) {
            curSzUnit = FontSizeToSzUnit(font_sz);
         } else {
            curSzUnit = sz_unit;
         }
         if (len > 1 && *font_str == '\'' && font_str[len-1] == '\'') {
            char *psz1=(&font_str[1]);

            font_str[len-1] = '\0';
            psz = font_str;
            while (*psz1 != '\0') *psz++ = *psz1++;
            *psz = '\0';
         }
         if ((psz=strchr(font_str, '%')) != NULL) {
            *psz++ = '\0';
            /* Ex., ignore last '%' if font_str eq "Courier%". */
            if (*psz != '\0') {
               strcpy(sb_font_str, font_str);
               strcpy(font_str, psz);
            }
         }
         curFont = GetFontIndex(font_str, curStyle, TRUE);
         if (curFont == INVALID && (!PRTGIF || cmdLineOpenDisplay)) {
            sprintf(gszMsgBox, TgLoadString(STID_CANNOT_FIND_FONT_USE_ALT),
                  font_str, "Times");
            MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
            curFont = FONT_TIM;
         }
      }
      switch (pageLayoutMode) {
      case PAGE_STACK:
         curPageNum = page_arg1;
         lastPageNum = page_arg2;
         paperCol = paperRow = 1;
         pageLineShownInTileMode = TRUE;
         break;
      case PAGE_TILE:
         paperCol = page_arg1;
         paperRow = page_arg2;
         curPageNum = lastPageNum = 1;
         break;
      }
      loadedCurPageNum = curPageNum;
      InitPage();
      curPageNum = loadedCurPageNum;

      if (fileVersion <= 13) {
         switch (gridSystem) {
         case ENGLISH_GRID:
            drawOrigX += HALF_INCH;
            drawOrigY += HALF_INCH;
            break;
         case METRIC_GRID:
            drawOrigX += 2.5*ONE_CM;
            drawOrigY += 2.5*ONE_CM;
            break;
         }
      }
      if ((usePaperSizeStoredInFile || PRTGIF) && fileVersion >= 32) {
         if (pageStyle == LANDSCAPE) {
            onePageWidth = one_page_height;
            onePageHeight = one_page_width;
         } else {
            onePageWidth = one_page_width;
            onePageHeight = one_page_height;
         }
         SetPSPageWidthHeight();
         if (PRTGIF) {
            UpdPageStyle(pageStyle);
         }
      }
      if (PRTGIF && !cmdLineOpenDisplay) {
         if (forced_use_gray) useGray = TRUE;
         return TRUE;
      }
      if (rotate != 0) {
         switch (rotate) {
         case ROTATE0: textRotation = 0; break;
         case ROTATE90: textRotation = (90<<6); break;
         case ROTATE180: textRotation = (180<<6); break;
         case ROTATE270: textRotation = (270<<6); break;
         }
         rotationIncrement = (90<<6);
         rotate = ROTATE0;
      }
      if (colorIndex >= maxColors) {
         fprintf(stderr, TgLoadString(STID_CANT_FIND_COLOR_NUM_USE_CUR),
               colorIndex, colorMenuItems[defaultColorIndex]);
         fprintf(stderr, "\n");
         colorIndex = defaultColorIndex;
         gnCannotFindColorMsg = TRUE;
      }
      SetCanvasFont();
/*
 *    if (!PRTGIF && SzUnitToFontSize(curSzUnit) != canvasFontSize) {
 *       curFont = actualCurFont;
 *       curSzUnit = actualCurSzUnit;
 *       SetFileModified(TRUE);
 *    }
 */
   }
   /*
    * Move from below.  Hope this won't change anything!
    */
   UpdDrawWinWH();
   UpdPageStyle(pageStyle);
   if (PRTGIF && !cmdLineOpenDisplay) return TRUE;

   if (lineWidth >= maxLineWidths) {
      fprintf(stderr, TgLoadString(STID_LINEWIDTH_IDX_RANGE_SET_TO_0),
            lineWidth);
      fprintf(stderr, "\n");
      lineWidth = 0;
   }

   RedrawScrollBars();
   ShowPage();
   ShowPageLayout();
   UpdDrawWinBBox();

   SetDefaultDrawWinClipRecs();

   DrawPaperBoundary(drawWindow);
   RedrawGridLines(drawWindow);
   RedrawPageLines(drawWindow);
   RedrawRulers();
   RedrawChoiceWindow();

   return TRUE;
}

static
void ReadObjAttrs(MinFileVersion, FP, ObjPtr)
   int MinFileVersion;
   FILE *FP;
   struct ObjRec **ObjPtr;
{
   struct AttrRec *top_attr=NULL, *bot_attr=NULL, *attr_ptr;

   if (fileVersion <= MinFileVersion) return;

   while (ReadAttr(FP, &attr_ptr)) {
      attr_ptr->owner = *ObjPtr;
      attr_ptr->prev = NULL;
      attr_ptr->next = top_attr;
      if (top_attr == NULL) {
         bot_attr = attr_ptr;
      } else {
         top_attr->prev = attr_ptr;
      }
      top_attr = attr_ptr;
   }
   if (bot_attr != NULL) bot_attr->next = NULL;
   if (*ObjPtr == NULL) {
      DelAllAttrs(top_attr);
   } else {
      (*ObjPtr)->fattr = top_attr;
      (*ObjPtr)->lattr = bot_attr;
   }
}

static
int ReadPageObj(Inbuf, ppsz_page_name)
   char *Inbuf, **ppsz_page_name;
{
   int page_num=0, reading_page_num=0;
   char *s=NULL, *c_ptr=NULL;

   if (ppsz_page_name != NULL) *ppsz_page_name = NULL;
   s = FindChar((int)'(', Inbuf);
   if (sscanf(s, "%d", &page_num) != 1) {
      sprintf(gszMsgBox, TgLoadString(STID_BAD_FIELD_IN_OBJ_ABORT_READ),
            scanFileName, scanLineNum, "page_num", "page");
      if (PRTGIF) {
         fprintf(stderr, "%s\n", gszMsgBox);
      } else {
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      }
      return FALSE;
   }
   reading_page_num = page_num;
   if (!importingFile) {
      char saved_ch='\0';

      for (curPage=firstPage; page_num != 1 && curPage != NULL;
            curPage=curPage->next, page_num--) {
      }
      if (curPage == NULL) {
         if (!mergingFile) {
            sprintf(gszMsgBox, TgLoadString(STID_BAD_INPUT_WHILE_READ_PAGE),
                  page_num);
            if (PRTGIF) {
               fprintf(stderr, "%s\n", gszMsgBox);
            } else {
               MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
            }
            return FALSE;
         }
         AddPageAfter();
      }
      s = FindChar((int)',', s);
      c_ptr = FindChar((int)'"', s);
      s = ReadString(c_ptr);
      saved_ch = *(--s);
      *s = '\0';
      if (*c_ptr != '\0') {
         curPage->name = UtilStrDup(c_ptr);
         if (curPage->name == NULL) FailAllocMessage();
      }
      *s++ = saved_ch;
      InitScan(s, "\t\n, )");

      topObj = curPage->top;
      botObj = curPage->bot;

      curPage->layer_on = TRUE;
      if (fileVersion <= 32) {
      } else {
         int layer_on=TRUE;
         char page_file_name[MAXSTRING];

         if (GETINT("page", layer_on, "color layer on") == INVALID) {
            return FALSE;
         }
         curPage->layer_on = layer_on;
         /*
          * This is a hack!  If the file version becomes >= 38, this code
          *       must be removed!
          */
         if (GETSTR("page", page_file_name, "page_file_name") == INVALID) {
            /* should not get here because we should have read in: "." */
            return FALSE;
         } else if (strcmp(page_file_name, ".") == 0) {
            *page_file_name = '\0';
         } else if (*page_file_name == '\'') {
            UtilRemoveQuotes(page_file_name);
         }
         if (*page_file_name != '\0') {
            char *psz=strchr(page_file_name, ' ');

            if (psz != NULL) {
               char truncated_name[MAXSTRING];

               *psz = *truncated_name = '\0';
               UtilStrCpyN(truncated_name, sizeof(truncated_name),
                     page_file_name);
               *psz = ' ';
               sprintf(gszMsgBox,
                     TgLoadString(STID_BAD_PAGE_FILE_NAME_TRUNC_TO),
                     page_file_name, truncated_name);
               if (PRTGIF) {
                  fprintf(stderr, "%s\n", gszMsgBox);
               } else {
                  MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
               }
               strcpy(page_file_name, truncated_name);
            }
            curPage->page_file_name = UtilStrDup(page_file_name);
            if (curPage->page_file_name == NULL) FailAllocMessage();
         }
      }
   } else {
      s = FindChar((int)',', s);
      c_ptr = FindChar((int)'"', s);
      s = ReadString(c_ptr);
      *(--s) = '\0';
      if (*c_ptr != '\0' && ppsz_page_name != NULL) {
         *ppsz_page_name = UtilStrDup(c_ptr);
         if (*ppsz_page_name == NULL) FailAllocMessage();
      }
   }
   readingPageNum++;
   if (readingPageNum < reading_page_num) {
      readingPageNum = reading_page_num;
   }
   return TRUE;
}

static
int ReadUnitObj(Inbuf)
   char *Inbuf;
{
   if (!importingFile) {
      char *s=FindChar((int)'(', Inbuf), *c_ptr;

      c_ptr = FindChar((int)'"', s);
      s = ReadString(c_ptr);
      *(--s) = '\0';
      if (SetUnit(c_ptr)) {
         return TRUE;
      } else {
         return FALSE;
      }
   }
   return TRUE;
}

static
int ReadGeneratedBy(Inbuf)
   char *Inbuf;
{
   int ok=TRUE;
   char *s=FindChar((int)'(', Inbuf);

   memset(&gGenerateByInfo, 0, sizeof(GenerateByInfo));

   if (s == NULL) {
      ok = FALSE;
   } else {
      GenerateByInfo *pgbi=(&gGenerateByInfo);

      InitScan(s, "\t\n, ");

      if (GETSTR("generated_by", pgbi->name,        "name") == INVALID ||
          GETINT("generated_by", pgbi->version,     "version") == INVALID ||
          GETSTR("generated_by", pgbi->version_str, "version_str") == INVALID) {
         ok = FALSE;
         memset(&gGenerateByInfo, 0, sizeof(GenerateByInfo));
      } else {
         UtilRemoveQuotes(pgbi->name);
         UtilRemoveQuotes(pgbi->version_str);
      }
   }
   if (!ok) {
      fprintf(stderr, TgLoadCachedString(CSTID_MALFORMED_LINE_NUM_IN_FILE),
            scanLineNum, scanFileName);
      fprintf(stderr, "\n");
   }
   return TRUE;
}

static
int ReadScriptFracObj(Inbuf)
   char *Inbuf;
{
   if (!importingFile) {
      char *s=FindChar((int)'(', Inbuf), *c_ptr=NULL;

      c_ptr = FindChar((int)'"', s);
      s = ReadString(c_ptr);
      *(--s) = '\0';
      if (!FixDecimalPoint(c_ptr)) {
#ifdef _TGIF_DBG /* debug, do not translate */
         fprintf(stderr, "FixDecimalPoint() failed in ReadScriptFracObj().\n");
#endif /* _TGIF_DBG */
      }
      if (SetScriptFractionValue(c_ptr)) {
         return TRUE;
      } else {
         return FALSE;
      }
   }
   return TRUE;
}

static
int ReadRightMarginObj(Inbuf)
   char *Inbuf;
{
   if (!importingFile) {
      int right_margin_enabled=FALSE, right_margin=0;
      char *s=FindChar((int)'(', Inbuf);

      InitScan(s, "\t\n, )");
      GETINT("right_margin", right_margin_enabled, "Right Margin Enabled");
      GETINT("right_margin", right_margin,         "Right Margin");

      rightMarginEnabled = right_margin_enabled;
      rightMargin = right_margin;
      rightMarginActive = rightMarginEnabled;

      if (!PRTGIF) RedrawHRulerWindow();
   }
   return TRUE;
}

static
int ReadThreshFillReplaceObj(Inbuf)
   char *Inbuf;
{
   if (!importingFile) {
      int do_msg=FALSE;
      int saved_thresh_fill_replace_enabled=threshFillReplaceEnabled;
      int saved_r=fillReplaceRedThresh, r=0;
      int saved_g=fillReplaceGreenThresh, g=0;
      int saved_b=fillReplaceBlueThresh, b=0;
      char *s=FindChar((int)'(', Inbuf);

      InitScan(s, "\t\n, )");
      GETINT("threshold_fill_replace", r, "Red Threshold");
      GETINT("threshold_fill_replace", g, "Green Threshold");
      GETINT("threshold_fill_replace", b, "Blue Threshold");

      do_msg = (saved_thresh_fill_replace_enabled && (saved_r != r ||
            saved_g != g || saved_b != b));

      threshFillReplaceEnabled = TRUE;
      fillReplaceRedThresh = r;
      fillReplaceGreenThresh = g;
      fillReplaceBlueThresh = b;

      if (!PRTGIF && do_msg) {
         sprintf(gszMsgBox, TgLoadString(STID_FLOOD_REPLACE_ENABLED),
               fillReplaceRedThresh, fillReplaceGreenThresh,
               fillReplaceBlueThresh);
         Msg(gszMsgBox);
      }
   }
   return TRUE;
}

#define GETSSVALUE(val,name) ScanValue("%d", &(val), name, "shapeshadow")

static
int ReadShapeShadowObj(Inbuf)
   char *Inbuf;
{
   if (!importingFile) {
      char *s=FindChar((int)'(', Inbuf);
      int dx=0, dy=0;

      InitScan(s, "\t\n, ");

      if (GETSSVALUE(dx, "dx") == INVALID ||
          GETSSVALUE(dy, "dy") == INVALID) {
         return FALSE;
      }
      shapeShadowDx = dx;
      shapeShadowDy = dy;
   }
   return TRUE;
}

static
int ReadFgBgColors(Inbuf)
   char *Inbuf;
{
   if (!importingFile && !PRTGIF) {
      int new_alloc=FALSE, bg_color_index=INVALID;
      char *s=FindChar((int)'(', Inbuf), fg_color_str[40], bg_color_str[40];

      s = ParseStr(s, (int)',', fg_color_str, sizeof(fg_color_str));
      s = ParseStr(s, (int)')', bg_color_str, sizeof(bg_color_str));
      UtilTrimBlanks(fg_color_str);
      UtilTrimBlanks(bg_color_str);
      allocColorFailed = FALSE;
      colorIndex = QuickFindColorIndex(NULL, fg_color_str, &new_alloc,
            TRUE);
      if (gnCannotFindColorMsg && !allocColorFailed) {
         sprintf(gszMsgBox, TgLoadString(STID_CUR_COLOR_CORRECTED_TO_BE),
               colorMenuItems[colorIndex]);
         fprintf(stderr, "    %s\n", gszMsgBox);
      }
      bg_color_index = QuickFindColorIndex(NULL, bg_color_str, &new_alloc,
            FALSE);
      if (bg_color_index == INVALID) {
         sprintf(gszMsgBox, TgLoadString(STID_FAIL_ALLOC_BGCOLOR_USE_DEF),
               bg_color_str, defaultBgColorStr);
         fprintf(stderr, "%s\n", gszMsgBox);
      } else {
         strcpy(defaultBgColorStr, bg_color_str);
         defaultBgColorIndex = bg_color_index;
      }
   }
   return TRUE;
}

static
int FreeBufAndReturn(buf, rc)
   char *buf;
   int rc;
{
   if (buf != NULL) free(buf);
   return rc;
}

int ReadObj(FP, ObjPtr)
   FILE *FP;
   struct ObjRec **ObjPtr;
{
   char *line, obj_name[80];
   int read_state_ok;

   *ObjPtr = NULL;
   while ((line=UtilGetALine(FP)) != NULL) {
      scanLineNum++;
      if (*line == ']') return FreeBufAndReturn(line, FALSE);

      if (*line == '%') {
         if (!importingFile && line[1]=='%') {
            int line_len=strlen(line);

            if (savedComments == NULL) {
               if ((savedComments=(char*)malloc((line_len+2)*sizeof(char)))
                     == NULL) {
                  FailAllocMessage();
               }
               *savedComments = '\0';
            } else {
               if ((savedComments=(char*)realloc(savedComments,
                     savedCommentsLen+line_len+2)) == NULL) {
                  FailAllocMessage();
               }
               savedComments[savedCommentsLen] = '\0';
            }
            strcat(savedComments, line);
            savedCommentsLen += line_len;
            savedComments[savedCommentsLen++] = '\n';
            savedComments[savedCommentsLen] = '\0';
         }
         /* do not translate -- program constants */
         if (strncmp(line, "%TGWB end - ", strlen("%TGWB end - ")) == 0) {
            return FreeBufAndReturn(line, FALSE);
         }
         free(line);
         continue;
      }
      /* do not translate -- program constants */
      if (ParseStr(line, (int)'(', obj_name, sizeof(obj_name)) == NULL) {
      } else if (strcmp(obj_name, "poly") == 0) {
         ReadPolyObj(FP, line, ObjPtr);
         if (*ObjPtr == NULL) return FreeBufAndReturn(line, FALSE);
         if (deserializingFile) ReadCreatorID(FP, ObjPtr);
         ReadObjAttrs(INVALID, FP, ObjPtr);
         if (RetractedArrowAttr(*ObjPtr) ||
               AutoRetractedArrowAttr(*ObjPtr, TRUE)) {
            /* fake the undoingOrRedoing so that no */
            /*          actual auto-adjusting is done */
            undoingOrRedoing = TRUE;
            AdjObjSplineVs(*ObjPtr);
            undoingOrRedoing = FALSE;
         }
         AdjObjBBox(*ObjPtr);
         return FreeBufAndReturn(line, TRUE);
      } else if (strcmp(obj_name, "box") == 0) {
         ReadBoxObj(FP, line, ObjPtr);
         if (*ObjPtr == NULL) return FreeBufAndReturn(line, FALSE);
         if (deserializingFile) ReadCreatorID(FP, ObjPtr);
         ReadObjAttrs(START_HAVING_ATTRS-1, FP, ObjPtr);
         AdjObjBBox(*ObjPtr);
         return FreeBufAndReturn(line, TRUE);
      } else if (strcmp(obj_name, "oval") == 0) {
         ReadOvalObj(FP, line, ObjPtr);
         if (*ObjPtr == NULL) return FreeBufAndReturn(line, FALSE);
         if (deserializingFile) ReadCreatorID(FP, ObjPtr);
         ReadObjAttrs(START_HAVING_ATTRS-1, FP, ObjPtr);
         AdjObjBBox(*ObjPtr);
         return FreeBufAndReturn(line, TRUE);
      } else if (strcmp(obj_name, "text") == 0) {
         ReadTextObj(FP, line, ObjPtr);
         if (*ObjPtr == NULL) return FreeBufAndReturn(line, FALSE);
         return FreeBufAndReturn(line, TRUE);
      } else if (strcmp(obj_name, "polygon") == 0) {
         ReadPolygonObj(FP, line, ObjPtr);
         if (*ObjPtr == NULL) return FreeBufAndReturn(line, FALSE);
         if (deserializingFile) ReadCreatorID(FP, ObjPtr);
         ReadObjAttrs(START_HAVING_ATTRS-1, FP, ObjPtr);
         AdjObjBBox(*ObjPtr);
         return FreeBufAndReturn(line, TRUE);
      } else if (strcmp(obj_name, "arc") == 0) {
         ReadArcObj(FP, line, ObjPtr);
         if (*ObjPtr == NULL) return FreeBufAndReturn(line, FALSE);
         if (deserializingFile) ReadCreatorID(FP, ObjPtr);
         ReadObjAttrs(START_HAVING_ATTRS-1, FP, ObjPtr);
         AdjObjBBox(*ObjPtr);
         return FreeBufAndReturn(line, TRUE);
      } else if (strcmp(obj_name, "rcbox") == 0) {
         ReadRCBoxObj(FP, line, ObjPtr);
         if (*ObjPtr == NULL) return FreeBufAndReturn(line, FALSE);
         if (deserializingFile) ReadCreatorID(FP, ObjPtr);
         ReadObjAttrs(START_HAVING_ATTRS-1, FP, ObjPtr);
         AdjObjBBox(*ObjPtr);
         return FreeBufAndReturn(line, TRUE);
      } else if (strcmp(obj_name, "xbm") == 0) {
         ReadXBmObj(FP, line, ObjPtr);
         if (*ObjPtr == NULL) return FreeBufAndReturn(line, FALSE);
         if (deserializingFile) ReadCreatorID(FP, ObjPtr);
         ReadObjAttrs(START_HAVING_ATTRS-1, FP, ObjPtr);
         AdjObjBBox(*ObjPtr);
         return FreeBufAndReturn(line, TRUE);
      } else if (strcmp(obj_name, "xpm") == 0) {
         ReadXPmObj(FP, line, ObjPtr);
         if (*ObjPtr == NULL) return FreeBufAndReturn(line, FALSE);
         if (deserializingFile) ReadCreatorID(FP, ObjPtr);
         ReadObjAttrs(START_HAVING_ATTRS-1, FP, ObjPtr);
         AdjObjBBox(*ObjPtr);
         return FreeBufAndReturn(line, TRUE);
      } else if (strcmp(obj_name, "jpeg") == 0) {
         ReadJpegObj(FP, line, ObjPtr);
         if (*ObjPtr == NULL) return FreeBufAndReturn(line, FALSE);
         if (deserializingFile) ReadCreatorID(FP, ObjPtr);
         ReadObjAttrs(START_HAVING_ATTRS-1, FP, ObjPtr);
         AdjObjBBox(*ObjPtr);
         return FreeBufAndReturn(line, TRUE);
      } else if (strcmp(obj_name, "ppm_true") == 0) {
         ReadPpmTrueObj(FP, line, ObjPtr);
         if (*ObjPtr == NULL) return FreeBufAndReturn(line, FALSE);
         if (deserializingFile) ReadCreatorID(FP, ObjPtr);
         ReadObjAttrs(START_HAVING_ATTRS-1, FP, ObjPtr);
         AdjObjBBox(*ObjPtr);
         return FreeBufAndReturn(line, TRUE);
      } else if (strcmp(obj_name, "group") == 0) {
         ReadGroupObj(FP, OBJ_GROUP, ObjPtr);
         if (*ObjPtr == NULL) return FreeBufAndReturn(line, FALSE);
         if (deserializingFile) ReadCreatorID(FP, ObjPtr);
         ReadObjAttrs(INVALID, FP, ObjPtr);
         AdjObjBBox(*ObjPtr);
         return FreeBufAndReturn(line, TRUE);
      } else if (strcmp(obj_name, "sym") == 0) {
         ReadGroupObj(FP, OBJ_SYM, ObjPtr);
         if (*ObjPtr == NULL) return FreeBufAndReturn(line, FALSE);
         if (deserializingFile) ReadCreatorID(FP, ObjPtr);
         ReadObjAttrs(INVALID, FP, ObjPtr);
         AdjObjBBox(*ObjPtr);
         return FreeBufAndReturn(line, TRUE);
      } else if (strcmp(obj_name, "icon") == 0) {
         ReadGroupObj(FP, OBJ_ICON, ObjPtr);
         if (*ObjPtr == NULL) return FreeBufAndReturn(line, FALSE);
         if (deserializingFile) ReadCreatorID(FP, ObjPtr);
         ReadObjAttrs(INVALID, FP, ObjPtr);
         AdjObjBBox(*ObjPtr);
         return FreeBufAndReturn(line, TRUE);
      } else if (strcmp(obj_name, "pin") == 0) {
         ReadGroupObj(FP, OBJ_PIN, ObjPtr);
         if (*ObjPtr == NULL) return FreeBufAndReturn(line, FALSE);
         if (deserializingFile) ReadCreatorID(FP, ObjPtr);
         ReadObjAttrs(INVALID, FP, ObjPtr);
         AdjObjBBox(*ObjPtr);
         return FreeBufAndReturn(line, TRUE);
      } else if (strcmp(obj_name, "simple_string") == 0) {
         /* reserved */
         ReadSimpleStringObj(FP, line, ObjPtr);
         if (*ObjPtr == NULL) return FreeBufAndReturn(line, FALSE);
         if (deserializingFile) ReadCreatorID(FP, ObjPtr);
         return FreeBufAndReturn(line, TRUE);
      } else if (strcmp(obj_name, "page") == 0) {
         int ok;
         char *page_name=NULL;

         *ObjPtr = NULL;
         if (importingFile && !importingIconFile) {
            if (importingPageNum == (-1) && *importingPageName == '\0') {
               if (readingPageNum == 1) {
                  /* as if read to the end of file */
                  if (!PRTGIF && !pastingFile) {
                     MsgBox(TgLoadString(STID_ONLY_PAGE_1_IMPORTED), TOOL_NAME,
                           INFO_MB);
                  }
                  return FreeBufAndReturn(line, FALSE);
               }
            } else {
               if (readingPageNum == importingPageNum) {
                  /* as if read to the end of file */
                  if (!PRTGIF && !pastingFile) {
                     sprintf(gszMsgBox,
                           TgLoadString(STID_ONLY_GIVEN_PAGE_IMPORTED),
                           importingPageNum);
                     MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
                  }
                  return FreeBufAndReturn(line, FALSE);
               }
            }
         }
         ok = ReadPageObj(line, &page_name);
         if (importingFile && !importingIconFile &&
               importingPageNum == (-1) && *importingPageName != '\0' &&
               page_name != NULL && strcmp(page_name,importingPageName)==0) {
            *importingPageName = '\0';
            importingPageNum = readingPageNum;
         }
         if (page_name != NULL) free(page_name);
         return FreeBufAndReturn(line, ok);
      } else if (strcmp(obj_name, "state") == 0) {
         if ((read_state_ok=ReadState(line)) == TRUE) {
            foundGoodStateObject = TRUE;
         }
         *ObjPtr = NULL;
         return FreeBufAndReturn(line, (read_state_ok) ? TRUE : INVALID);
      } else if (strcmp(obj_name, "file_attr") == 0) {
         if (mergingFile) {
            struct AttrRec *saved_first_attr=NULL, *saved_last_attr=NULL;

            saved_first_attr = tgifObj->fattr;
            saved_last_attr = tgifObj->lattr;
            tgifObj->fattr = NULL;
            tgifObj->lattr = NULL;

            ReadObjAttrs(START_HAVING_ATTRS-1, FP, &tgifObj);

            if (tgifObj->fattr == NULL) {
               /* this file has no file attribute -- which should not happen */
               tgifObj->fattr = saved_first_attr;
               tgifObj->lattr = saved_last_attr;
            } else if (saved_last_attr == NULL) {
               /* this is this first file that has file attributes, keep it */
            } else {
               tgifObj->fattr->prev = saved_last_attr;
               saved_last_attr->next = tgifObj->fattr;
               tgifObj->fattr = saved_first_attr;
            }
         } else if (importingFile && !importingIconFile) {
            struct AttrRec *saved_first_attr=NULL, *saved_last_attr=NULL;

            saved_first_attr = tgifObj->fattr;
            saved_last_attr = tgifObj->lattr;
            tgifObj->fattr = NULL;
            tgifObj->lattr = NULL;

            ReadObjAttrs(START_HAVING_ATTRS-1, FP, &tgifObj);
            DelAllAttrs(tgifObj->fattr);

            tgifObj->fattr = saved_first_attr;
            tgifObj->lattr = saved_last_attr;
         } else {
            ReadObjAttrs(START_HAVING_ATTRS-1, FP, &tgifObj);
         }
         return FreeBufAndReturn(line, TRUE);
      } else if (strcmp(obj_name, "unit") == 0) {
         int ok=ReadUnitObj(line);

         *ObjPtr = NULL;
         return FreeBufAndReturn(line, ok);
      } else if (strcmp(obj_name, "generated_by") == 0) {
         int ok=ReadGeneratedBy(line);

         *ObjPtr = NULL;
         return FreeBufAndReturn(line, ok);
      } else if (strcmp(obj_name, "shapeshadow") == 0) {
         int ok=ReadShapeShadowObj(line);

         *ObjPtr = NULL;
         return FreeBufAndReturn(line, ok);
      } else if (strcmp(obj_name, "color_info") == 0) {
         int ok=ReadColors(FP, line);

         *ObjPtr = NULL;
         return FreeBufAndReturn(line, ok);
      } else if (strcmp(obj_name, "cmdx") == 0) {
         int ok=ReadExtendedCmd(FP, line);

         *ObjPtr = NULL;
         return FreeBufAndReturn(line, ok);
      } else if (strcmp(obj_name, "cmdxinfo") == 0) {
         int ok=ReadExtendedCmdInfo(FP, line);

         *ObjPtr = NULL;
         return FreeBufAndReturn(line, ok);
      } else if (strcmp(obj_name, "cmd") == 0) {
         int ok=ReadCmd(FP, line);

         *ObjPtr = NULL;
         return FreeBufAndReturn(line, ok);
      } else if (strcmp(obj_name, "before_image") == 0) {
         int ok=ReadBeforeImage(FP, line);

         *ObjPtr = NULL;
         return FreeBufAndReturn(line, ok);
      } else if (strcmp(obj_name, "after_positions") == 0) {
         int ok=ReadAfterPositions(FP, line);

         *ObjPtr = NULL;
         return FreeBufAndReturn(line, ok);
      } else if (strcmp(obj_name, "after_image") == 0) {
         int ok=ReadAfterImage(FP, line);

         *ObjPtr = NULL;
         return FreeBufAndReturn(line, ok);
      } else if (strcmp(obj_name, "slideshow_info") == 0) {
         int ok=ReadSlideShowInfo(line);

         *ObjPtr = NULL;
         return FreeBufAndReturn(line, ok);
      } else if (strcmp(obj_name, "script_frac") == 0) {
         int ok=ReadScriptFracObj(line);

         *ObjPtr = NULL;
         return FreeBufAndReturn(line, ok);
      } else if (strcmp(obj_name, "fg_bg_colors") == 0) {
         int ok=ReadFgBgColors(line);

         *ObjPtr = NULL;
         return FreeBufAndReturn(line, ok);
      } else if (strcmp(obj_name, "html_export_template") == 0) {
         int ok=ReadHtmlExportTemplate(line);

         *ObjPtr = NULL;
         return FreeBufAndReturn(line, ok);
      } else if (strcmp(obj_name, "objshadow_info") == 0) {
         int ok=ReadObjectShadowInfo(line);

         *ObjPtr = NULL;
         return FreeBufAndReturn(line, ok);
      } else if (strcmp(obj_name, "rotate_pivot") == 0) {
         int ok=ReadRotatePivotInfo(line);

         *ObjPtr = NULL;
         return FreeBufAndReturn(line, ok);
      } else if (strcmp(obj_name, "spline_tightness") == 0) {
         int ok=ReadSplineTightness(line);

         *ObjPtr = NULL;
         return FreeBufAndReturn(line, ok);
      } else if (strcmp(obj_name, "right_margin") == 0) {
         int ok=ReadRightMarginObj(line);

         *ObjPtr = NULL;
         return FreeBufAndReturn(line, ok);
      } else if (strcmp(obj_name, "threshold_fill_replace") == 0) {
         int ok=ReadThreshFillReplaceObj(line);

         *ObjPtr = NULL;
         return FreeBufAndReturn(line, ok);
      } else if (strcmp(obj_name, "dont_reencode") == 0) {
         int ok=ReadDontReencode(line);

         *ObjPtr = NULL;
         return FreeBufAndReturn(line, ok);
      } else if (strcmp(obj_name, "ps_font_aliases") == 0) {
         int ok=ReadPSFontAliases(FP, line);

         *ObjPtr = NULL;
         return FreeBufAndReturn(line, ok);
      } else if (strcmp(obj_name, "ps_char_subs") == 0) {
         int ok=ReadPSCharSubs(FP, line);

         *ObjPtr = NULL;
         return FreeBufAndReturn(line, ok);
      } else if (strcmp(obj_name, "ps_font_need_char_subs") == 0) {
         int ok=ReadPSFontNeedCharSubs(FP, line);

         *ObjPtr = NULL;
         return FreeBufAndReturn(line, ok);
      }
      free(line);
   }
   return FALSE;
}

void AdjForOldVersion(obj_ptr)
   struct ObjRec *obj_ptr;
{
   if (fileVersion <= 13) {
      switch (gridSystem) {
      case ENGLISH_GRID:
         MoveObj(obj_ptr, (int)(HALF_INCH), (int)(HALF_INCH));
         break;
      case METRIC_GRID:
         MoveObj(obj_ptr, (int)(2.5*ONE_CM), (int)(2.5*ONE_CM));
         break;
      }
   }
}

int DownloadRemoteFile(file_name, ppsz_content_type, ppsz_page_spec,
      pn_is_html, return_tmp_fname, psz_final_url, cb_final_url)
   char *file_name, **ppsz_content_type, **ppsz_page_spec, *return_tmp_fname,
         *psz_final_url;
   int *pn_is_html, cb_final_url;
   /* file_name is assumed to be remote */
{
   int ok=TRUE;
   char remote_fname[MAXPATHLENGTH+1], *tmp_remote_fname=NULL;

   if (!FileIsRemote(file_name)) return FALSE;

   *remote_fname = '\0';
   if (ppsz_page_spec != NULL) *ppsz_page_spec = NULL;
   if (pn_is_html != NULL) *pn_is_html = FALSE;
   if (!FormNewFileName(curDir, file_name, NULL, remote_fname,
         ppsz_page_spec)) {
      sprintf(gszMsgBox, TgLoadString(STID_INVALID_REMOTE_FNAME), file_name);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      ok = FALSE;
   } else {
      int remote_buf_sz=0;
      char *remote_buf=NULL;

      SaveStatusStrings();
      ok = LoadRemoteFileInMem(remote_fname, &remote_buf, ppsz_content_type,
            &remote_buf_sz, pn_is_html, navigateRefresh, psz_final_url,
            cb_final_url);
      RestoreStatusStrings();
      if (ok && remote_buf != NULL) {
         if ((tmp_remote_fname=WriteRemoteFileIntoTemp(remote_buf,
               remote_buf_sz, NULL)) == NULL) {
            ok = FALSE;
         }
      }
      if (remote_buf != NULL) FreeRemoteBuf(remote_buf);
   }
   if (tmp_remote_fname != NULL) {
      strcpy(return_tmp_fname, tmp_remote_fname);
      FreeRemoteBuf(tmp_remote_fname);
   }
   return ok;
}

int ImportGivenFile(file_name, group_and_lock, highlight)
   char *file_name;
   int group_and_lock, highlight;
   /* returns TRUE if ok */
   /* returns FALSE if file_name looks ok, only have temporary problems */
   /* returns BAD if file_name is bad */
{
   struct ObjRec *obj_ptr, *saved_top_obj, *saved_bot_obj;
   char *rest, remote_fname[MAXPATHLENGTH+1], *remote_buf=NULL;
   char gzipped_fname[MAXPATHLENGTH+1], *tmp_remote_fname=NULL, *page_spec=NULL;
   char tmp_filename[MAXPATHLENGTH+1], tmp_filefullpath[MAXPATHLENGTH+1];
   int short_name=FALSE, read_status=0, remote_buf_sz=0;
   int tmp_linenum, file_is_remote=FALSE, interrupted;
   FILE *fp=NULL;
   XEvent ev;

   *gzipped_fname = '\0';
   if (FileIsRemote(file_name)) {
      int rc=TRUE;

      if (!FormNewFileName(curDir, file_name, NULL, remote_fname, &page_spec)) {
         sprintf(gszMsgBox, TgLoadString(STID_INVALID_REMOTE_FNAME), file_name);
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
         rc = BAD;
      } else {
         int is_html=FALSE;;

         SaveStatusStrings();
         rc = LoadRemoteFileInMem(remote_fname, &remote_buf, NULL,
               &remote_buf_sz, &is_html, TRUE, NULL, 0);
         RestoreStatusStrings();
         if (rc && remote_buf != NULL) {
            if ((tmp_remote_fname=WriteRemoteFileIntoTemp(remote_buf,
                  remote_buf_sz, NULL)) != NULL) {
               file_is_remote = TRUE;
            } else {
               rc = FALSE;
            }
         }
      }
      if (!file_is_remote) {
         if (remote_buf != NULL) FreeRemoteBuf(remote_buf);
         return rc;
      }
   } else {
      int gzipped=FALSE;

      if (FileNameHasExtension(file_name, OBJ_FILE_TYPE, &gzipped, NULL) &&
            gzipped) {
         char *tmp_fname=NULL;

         if ((tmp_fname=GunzipFileIntoTemp(file_name)) == NULL) {
            return BAD;
         } else {
            strcpy(gzipped_fname, file_name);
            strcpy(file_name, tmp_fname);
            free(tmp_fname);
         }
      }
   }
   XSync(mainDisplay, False);
   if (XCheckMaskEvent(mainDisplay, ExposureMask, &ev)) {
      ExposeEventHandler(&ev, TRUE);
   }
   if (file_is_remote) {
      if ((fp=fopen(tmp_remote_fname, "r")) == NULL) {
         sprintf(gszMsgBox, TgLoadString(STID_CANNOT_READ_TMP_FILE),
               tmp_remote_fname);
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
         unlink(tmp_remote_fname);
         FreeRemoteBuf(remote_buf);
         FreeRemoteBuf(tmp_remote_fname);
         /* temporary problem */
         return FALSE;
      }
   } else {
      if (*gzipped_fname == '\0') {
         if ((short_name=IsPrefix(bootDir, file_name, &rest))) ++rest;
      } else {
         if ((short_name=IsPrefix(bootDir, gzipped_fname, &rest))) ++rest;
      }
      if ((fp=fopen(file_name, "r")) == NULL) {
         if (short_name) {
            sprintf(gszMsgBox, TgLoadString(STID_CANNOT_IMPORT_FILE), rest);
         } else {
            if (*gzipped_fname == '\0') {
               sprintf(gszMsgBox, TgLoadString(STID_CANNOT_IMPORT_FILE),
                     file_name);
            } else {
               sprintf(gszMsgBox, TgLoadString(STID_CANNOT_IMPORT_FILE),
                     gzipped_fname);
            }
         }
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
         if (*gzipped_fname != '\0') unlink(file_name);
         /* temporary problem -- may be the file does not exist yet */
         return FALSE;
      }
   }
   strcpy(tmp_filefullpath, scanFileFullPath);
   strcpy(tmp_filename, scanFileName);
   tmp_linenum = scanLineNum;
   UtilStrCpyN(scanFileFullPath, sizeof(scanFileFullPath), file_name);
   if (file_is_remote) {
      strcpy(scanFileName, tmp_remote_fname);
   } else {
      strcpy(scanFileName, (short_name ? rest : file_name));
   }
   scanLineNum = 0;

   saved_top_obj = topObj;
   saved_bot_obj = botObj;
   curPage->top = curPage->bot = topObj = botObj = NULL;

   if (file_is_remote) {
      sprintf(gszMsgBox, TgLoadCachedString(CSTID_IMPORTING_FILE),
            remote_fname);
   } else {
      if (short_name) {
         sprintf(gszMsgBox, TgLoadCachedString(CSTID_IMPORTING_FILE), rest);
      } else {
         if (*gzipped_fname == '\0') {
            sprintf(gszMsgBox, TgLoadCachedString(CSTID_IMPORTING_FILE),
                  file_name);
         } else {
            sprintf(gszMsgBox, TgLoadCachedString(CSTID_IMPORTING_FILE),
                  gzipped_fname);
         }
      }
   }
   Msg(gszMsgBox);

   SetWatchCursor(drawWindow);
   SetWatchCursor(mainWindow);

   numRedrawBBox = 0;
   readingPageNum = loadedCurPageNum = 0;
   foundGoodStateObject = FALSE;
   ShowInterrupt(1);
   interrupted = FALSE;
   importingPageNum = (-1);
   *importingPageName = '\0';
   if (page_spec != NULL) {
      if (*page_spec == '#') {
         importingPageNum = atoi(&page_spec[1]);
         if (importingPageNum < 1) {
            importingPageNum = (-1);
            sprintf(gszMsgBox, TgLoadString(STID_INVALID_PAGE_NUM), page_spec);
            MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
         }
      } else {
         strcpy(importingPageName, page_spec);
      }
   }
   if (importingPageNum == (-1) && *importingPageName == '\0') {
      while (!interrupted && (read_status=ReadObj(fp, &obj_ptr)) == TRUE) {
         if (obj_ptr != NULL) {
            obj_ptr->tmp_parent = NULL;
            AdjForOldVersion(obj_ptr);
            UnlockAnObj(obj_ptr);
            AddObj(NULL, topObj, obj_ptr);
            if (!interrupted &&
                  (PointInBBox(obj_ptr->x, obj_ptr->y, drawWinBBox) ||
                  BBoxIntersect(obj_ptr->bbox, drawWinBBox))) {
               if (!DrawObj(drawWindow, obj_ptr)) interrupted = TRUE;
               if (CheckInterrupt(TRUE)) interrupted = TRUE;
            }
         }
      }
   } else if (importingPageNum == (-1)) {
      while (!interrupted && (read_status=ReadObj(fp, &obj_ptr)) == TRUE) {
         if (obj_ptr != NULL) {
            obj_ptr->tmp_parent = NULL;
            if (importingPageNum == (-1)) {
               FreeObj(obj_ptr);
               if (!interrupted && CheckInterrupt(TRUE)) interrupted = TRUE;
            } else {
               AdjForOldVersion(obj_ptr);
               UnlockAnObj(obj_ptr);
               AddObj(NULL, topObj, obj_ptr);
               if (!interrupted &&
                     (PointInBBox(obj_ptr->x, obj_ptr->y, drawWinBBox) ||
                     BBoxIntersect(obj_ptr->bbox, drawWinBBox))) {
                  if (!DrawObj(drawWindow, obj_ptr)) interrupted = TRUE;
                  if (CheckInterrupt(TRUE)) interrupted = TRUE;
               }
            }
         }
      }
   } else {
      while (!interrupted && (read_status=ReadObj(fp, &obj_ptr)) == TRUE) {
         if (obj_ptr != NULL) {
            obj_ptr->tmp_parent = NULL;
            if (importingPageNum != readingPageNum) {
               FreeObj(obj_ptr);
               if (!interrupted && CheckInterrupt(TRUE)) interrupted = TRUE;
            } else {
               AdjForOldVersion(obj_ptr);
               UnlockAnObj(obj_ptr);
               AddObj(NULL, topObj, obj_ptr);
               if (!interrupted &&
                     (PointInBBox(obj_ptr->x, obj_ptr->y, drawWinBBox) ||
                     BBoxIntersect(obj_ptr->bbox, drawWinBBox))) {
                  if (!DrawObj(drawWindow, obj_ptr)) interrupted = TRUE;
                  if (CheckInterrupt(TRUE)) interrupted = TRUE;
               }
            }
         }
      }
   }
   if (!PRTGIF && colorLayers && needToRedrawColorWindow) {
      RedrawColorWindow();
   }
   if (interrupted) {
      MsgBox(TgLoadString(STID_USER_INTR_ABORT_DRAW), TOOL_NAME, INFO_MB);
   }
   HideInterrupt();

   if (fp != NULL) fclose(fp);
   if (*gzipped_fname != '\0') {
      unlink(file_name);
      strcpy(file_name, gzipped_fname);
   }
   strcpy(scanFileFullPath, tmp_filefullpath);
   strcpy(scanFileName, tmp_filename);
   scanLineNum = tmp_linenum;

   if (read_status == INVALID) {
      if (fileVersion > CUR_VERSION) {
         sprintf(gszMsgBox, TgLoadString(STID_FILE_VER_ABORT_IMPORT),
               fileVersion, TOOL_NAME, homePageURL);
      } else {
         sprintf(gszMsgBox, TgLoadString(STID_FILE_CORRUPTED_ABORT_IMPORT));
      }
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      SetDefaultCursor(mainWindow);
      ShowCursor();
      if (file_is_remote) {
         unlink(tmp_remote_fname);
         FreeRemoteBuf(remote_buf);
         FreeRemoteBuf(tmp_remote_fname);
      }
      /* temporary problem -- may be the file will be fixed later */
      return FALSE;
   }
   if (file_is_remote) {
      if (!foundGoodStateObject) PasteString(remote_buf, TRUE, TRUE);

      unlink(tmp_remote_fname);
      FreeRemoteBuf(remote_buf);
      FreeRemoteBuf(tmp_remote_fname);
   }

   if (topObj != NULL) SetFileModified(TRUE);
   justDupped = FALSE;

   if (group_and_lock && topObj != NULL) {
      if (topObj != botObj || topObj->type==OBJ_POLY ||
            topObj->type==OBJ_POLYGON) {
         struct ObjRec *top_obj=topObj, *bot_obj=botObj;

         SelAllObj(FALSE, FALSE);
         curPage->top = curPage->bot = topObj = botObj = NULL;
         CreateGroupObj(top_obj, bot_obj);
         RemoveAllSel();
      }
      topObj->locked = TRUE;
   }
   SelAllObj(FALSE, FALSE);

   if (botObj != NULL) {
      botObj->next = saved_top_obj;
   } else {
      topObj = saved_top_obj;
   }
   if (saved_top_obj != NULL) {
      saved_top_obj->prev = botObj;
      botObj = saved_bot_obj;
   }
   curPage->top = topObj;
   curPage->bot = botObj;

   if (!(file_is_remote && !foundGoodStateObject) && topSel != NULL) {
      PrepareToRecord(CMD_NEW, NULL, NULL, 0);
      RecordCmd(CMD_NEW, NULL, topSel, botSel, numObjSelected);
   }
   if (highlight) HighLightForward();

   if (!importFromLibrary && !file_is_remote && !importingFromExec) {
      SetCurImportDir(file_name);
   }
   if (file_is_remote) {
      sprintf(gszMsgBox, TgLoadString(STID_FILE_IMPORTED), remote_fname);
   } else {
      if (short_name) {
         sprintf(gszMsgBox, TgLoadString(STID_FILE_IMPORTED), rest);
      } else {
         sprintf(gszMsgBox, TgLoadString(STID_FILE_IMPORTED), file_name);
      }
   }
   Msg(gszMsgBox);
   SetDefaultCursor(mainWindow);
   ShowCursor();
   if (page_spec != NULL) free(page_spec);
   return TRUE;
}

void ImportFile()
{
   char file_name[MAXPATHLENGTH+1];

   MakeQuiescent();

   importingFile = TRUE;
   if (importFromLibrary) {
      char name[MAXSTRING+1], path[MAXSTRING+1];

      if (SelectFromLibrary(TgLoadString(STID_SEL_AN_OBJ_FILE_TO_IMPORT),
            OBJ_FILE_EXT, name, path) == INVALID) {
         importingFile = FALSE;
         return;
      }
      sprintf(file_name, "%s%c%s", path, DIR_SEP, name);
   } else if (SelectFileNameToImport(
         TgLoadString(STID_SEL_AN_OBJ_FILE_TO_IMPORT),
         OBJ_FILE_EXT, file_name) == INVALID) {
      importingFile = FALSE;
      return;
   }
   ImportGivenFile(file_name, FALSE, TRUE);
   importingFile = FALSE;
   SetCurChoice(curChoiceBeforeMakeQuiescent);
}

int ClearBgColorInfo(nRedraw)
   int nRedraw;
{
   myFileBgPixel = myFileFgPixel = INVALID;
   if (myFileFgColorStr != NULL) {
      free(myFileFgColorStr);
      myFileFgColorStr = NULL;
   }
   if (myFileBgColorStr != NULL) {
      free(myFileBgColorStr);
      myFileBgColorStr = NULL;
      XSetWindowBackground(mainDisplay, drawWindow, myBgPixel);
      if (nRedraw) {
         ClearAndRedrawDrawWindow();
      }
      RecalcXorPixels();
      return TRUE;
   }
   return FALSE;
}

int ClearBgPixmapInfo(nRedraw)
   int nRedraw;
{
   int changed=FALSE;

   if (myFileBgGifURL != NULL) {
      free(myFileBgGifURL);
      myFileBgGifURL = NULL;
      changed = TRUE;
   }
   myFileBgPixmapW = myFileBgPixmapH = (-1);
   if (myFileBgPixmap != None) {
      XFreePixmap(mainDisplay, myFileBgPixmap);
      myFileBgPixmap = None;
      XSetWindowBackgroundPixmap(mainDisplay, drawWindow, None);
      changed = TRUE;
   }
   if (changed && nRedraw) {
      ClearAndRedrawDrawWindow();
   }
   return changed;
}

int SetFileBgColor()
   /* assumes myFileBgColorStr is set */
{
   int new_alloc=FALSE;
   int index=QuickFindColorIndex(NULL, myFileBgColorStr, &new_alloc, TRUE);

   if (index != INVALID) {
      myFileBgPixel = colorPixels[index];
      myFileBgColor.pixel = colorPixels[index];
      myFileBgColor.red = tgifColors[index].red;
      myFileBgColor.green = tgifColors[index].green;
      myFileBgColor.blue = tgifColors[index].blue;
      XSetWindowBackground(mainDisplay, drawWindow, colorPixels[index]);
      RecalcXorPixels();
      return TRUE;
   }
   sprintf(gszMsgBox, TgLoadString(STID_CANNOT_ALLOC_BGCOLOR_USE_DEF),
         myFileBgColorStr);
   Msg(gszMsgBox);
   return FALSE;
}

int SetFileFgColor()
   /* assumes myFileFgColorStr is set */
{
   int new_alloc=FALSE;
   int index=QuickFindColorIndex(NULL, myFileFgColorStr, &new_alloc, TRUE);

   if (index != INVALID) {
      myFileFgPixel = colorPixels[index];
      return TRUE;
   }
   sprintf(gszMsgBox, TgLoadString(STID_CANNOT_ALLOC_FGCOLOR_USE_DEF),
         myFileFgColorStr);
   Msg(gszMsgBox);
   return FALSE;
}

int SetFileBgPixmap()
{
   int ok=TRUE;
   char tmp_xpm_fname[MAXPATHLENGTH+1];

   if (myFileBgGifURL == NULL) return FALSE;

   *tmp_xpm_fname = '\0';
   if (FileIsRemote(myFileBgGifURL)) {
      char tmp_gif_fname[MAXPATHLENGTH+1], *content_type=NULL, *page_spec=NULL;
      int is_html=FALSE;

      *tmp_gif_fname = '\0';
      if (DownloadRemoteFile(myFileBgGifURL, &content_type, &page_spec,
            &is_html, tmp_gif_fname, NULL, 0)) {
         if (page_spec != NULL || is_html) {
            sprintf(gszMsgBox, TgLoadString(STID_INVALID_REMOTE_FNAME),
                  myFileBgGifURL);
            MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
            ok = FALSE;
         } else {
            SetWatchCursor(drawWindow);
            SetWatchCursor(mainWindow);
            SaveStatusStrings();
            ok = ConvertGifToXpm(tmp_gif_fname, tmp_xpm_fname,
                  sizeof(tmp_xpm_fname));
            RestoreStatusStrings();
            SetDefaultCursor(mainWindow);
            ShowCursor();
         }
      } else {
         ok = FALSE;
      }
      if (content_type != NULL) FreeRemoteBuf(content_type);
      if (page_spec != NULL) UtilFree(page_spec);
      if (*tmp_gif_fname != '\0') unlink(tmp_gif_fname);
   } else {
      SetWatchCursor(drawWindow);
      SetWatchCursor(mainWindow);
      SaveStatusStrings();
      ok = ConvertGifToXpm(myFileBgGifURL, tmp_xpm_fname,
            sizeof(tmp_xpm_fname));
      RestoreStatusStrings();
      SetDefaultCursor(mainWindow);
      ShowCursor();
   }
   if (ok) {
      int image_w, image_h, w, h, ncolors=0;
      int chars_per_pixel=0, first_pixel_is_bg=FALSE, *pixels=NULL;
      char *color_char=NULL, **color_str=NULL, *xpm_data=NULL;
      Pixmap pixmap=None, bitmap=None;
      XImage *image=NULL, *bitmap_image=NULL;

      SetWatchCursor(drawWindow);
      SetWatchCursor(mainWindow);
      if (!MyReadPixmapFile(tmp_xpm_fname, &image_w, &image_h, &w, &h, &pixmap,
            &image, &bitmap, &bitmap_image, &ncolors, &chars_per_pixel,
            &first_pixel_is_bg, &color_char, &color_str, &pixels,
            &xpm_data) == BitmapSuccess) {
         ok = FALSE;
      }
      SetDefaultCursor(mainWindow);
      ShowCursor();
      unlink(tmp_xpm_fname);
      if (bitmap != None) XFreePixmap(mainDisplay, bitmap);
      if (image != NULL) XDestroyImage(image);
      if (bitmap_image != NULL) XDestroyImage(bitmap_image);
      if (color_char != NULL) free(color_char);
      if (color_str != NULL) {
         int i;

         for (i=0; i < ncolors; i++) free(color_str[i]);
         free(color_str);
      }
      if (ok) {
         myFileBgPixmapW = image_w;
         myFileBgPixmapH = image_h;
         myFileBgPixmap = pixmap;
         XSetWindowBackgroundPixmap(mainDisplay, drawWindow, myFileBgPixmap);
      }
   }
   if (!ok) {
      sprintf(gszMsgBox, TgLoadString(STID_CANNOT_SET_BGXPM_TO),
            myFileBgGifURL);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
   }
   return ok;
}

void CheckFileAttrsInLoad()
{
   struct AttrRec *attr_ptr;
   int changed=FALSE;

   if ((attr_ptr=FindFileAttrWithName("bg_color=")) != NULL) {
      struct AttrRec *fg_attr_ptr;
      int ok=TRUE;

      if ((fg_attr_ptr=FindFileAttrWithName("fg_color=")) != NULL) {
         if (myFileFgColorStr == NULL ||
               strcmp(fg_attr_ptr->attr_value.s, myFileFgColorStr) != 0) {
            myFileFgColorStr = UtilStrDup(fg_attr_ptr->attr_value.s);
            if (myFileFgColorStr == NULL) FailAllocMessage();
            if (!SetFileFgColor()) {
               ClearBgColorInfo(TRUE);
               ok = FALSE;
            }
         }
      }
      if (ok && (myFileBgColorStr == NULL ||
            strcmp(attr_ptr->attr_value.s, myFileBgColorStr) != 0)) {
         myFileBgColorStr = UtilStrDup(attr_ptr->attr_value.s);
         if (myFileBgColorStr == NULL) FailAllocMessage();
         if (SetFileBgColor()) {
            ClearAndRedrawDrawWindow();
         } else {
            ClearBgColorInfo(TRUE);
         }
      }
      return;
   } else {
      changed = ClearBgColorInfo(FALSE);
   }
   /*
    * Can't really get this to work because of scrolling and zooming.
    */
/*
   if ((attr_ptr=FindFileAttrWithName("bg_gif=")) != NULL) {
      if (myFileBgGifURL == NULL ||
            strcmp(attr_ptr->attr_value.s, myFileBgGifURL) != 0) {
         if (myFileBgPixmap != None) XFreePixmap(mainDisplay, myFileBgPixmap);
         myFileBgPixmap = None;
         myFileBgPixmapW = myFileBgPixmapH = (-1);
         if (myFileBgGifURL != NULL) free(myFileBgGifURL);
         myFileBgGifURL = UtilStrDup(attr_ptr->attr_value.s);
         if (myFileBgGifURL == NULL) FailAllocMessage();
         if (SetFileBgPixmap()) {
            ClearAndRedrawDrawWindow();
         } else {
            ClearBgPixmapInfo(TRUE);
         }
      }
      return;
   } else {
      if (!changed) {
         changed = ClearBgPixmapInfo(FALSE);
      } else {
         ClearBgPixmapInfo(FALSE);
      }
   }
 */
   if (changed) {
      if (myFileBgPixmap == None) {
         XSetWindowBackground(mainDisplay, drawWindow, myBgPixel);
      }
      ClearAndRedrawDrawWindow();
   }
}

int LoadFile(FullName, ObjFile, GzippedObjFile)
   char *FullName;
   int ObjFile; /* equals TRUE if the file is an OBJ file */
                /* equals FALSE if the file is a SYM or PIN file */
                /* equals -1 if the file is an temporary OBJ file */
{
   struct ObjRec *obj_ptr=NULL;
   char tmp_filename[MAXPATHLENGTH+1], tmp_filefullpath[MAXPATHLENGTH+1];
   char file_name[MAXPATHLENGTH+1], saved_cur_dir[MAXPATHLENGTH+1], *rest=NULL;
   char gzipped_fname[MAXPATHLENGTH+1];
   int read_status=0, short_name=FALSE, tmp_linenum=0, interrupted=FALSE;
   int done_clear_all_wb=FALSE;
   FILE *fp=NULL;
   XEvent ev;

   *gzipped_fname = '\0';
   if (ObjFile != FALSE) {
      strcpy(saved_cur_dir, curDir);
   } else {
      strcpy(saved_cur_dir, curSymDir);
   }
   if (GzippedObjFile) {
      char *tmp_fname=NULL;

      if ((tmp_fname=GunzipFileIntoTemp(FullName)) == NULL) {
         return FALSE;
      } else {
         strcpy(gzipped_fname, FullName);
         strcpy(file_name, tmp_fname);
         free(tmp_fname);
      }
   } else {
      strcpy(file_name, FullName);
   }
   if (*gzipped_fname == '\0') {
      if ((short_name=IsPrefix(bootDir, file_name, &rest))) ++rest;
   } else {
      if ((short_name=IsPrefix(bootDir, gzipped_fname, &rest))) ++rest;
   }
   if ((fp=fopen(file_name, "r")) == NULL) {
      if (short_name) {
         sprintf(gszMsgBox, TgLoadString(STID_CANNOT_OPEN_FILE_FOR_READING),
               rest);
      } else {
         if (*gzipped_fname == '\0') {
            sprintf(gszMsgBox, TgLoadString(STID_CANNOT_OPEN_FILE_FOR_READING),
                  file_name);
         } else {
            sprintf(gszMsgBox, TgLoadString(STID_CANNOT_OPEN_FILE_FOR_READING),
                  gzipped_fname);
         }
      }
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      if (*gzipped_fname != '\0') unlink(file_name);
      return FALSE;
   }
   if (!navigatingBackAndForth) BeforeNavigate();

   ClearFileInfo(TRUE);
   if (usePaperSizeStoredInFile) ResetOnePageSize();

   strcpy(tmp_filefullpath, scanFileFullPath);
   strcpy(tmp_filename, scanFileName);
   tmp_linenum = scanLineNum;
   UtilStrCpyN(scanFileFullPath, sizeof(scanFileFullPath), FullName);
   strcpy(scanFileName, (short_name ? rest : file_name));
   scanLineNum = 0;

   TieLooseEnds();
   CleanUpDrawingWindow();
   SetFileModified(FALSE);

   XSync(mainDisplay, False);
   while (XCheckWindowEvent(mainDisplay, drawWindow, ExposureMask, &ev)) ;

   SaveStatusStrings();

   if (short_name) {
      sprintf(gszMsgBox, TgLoadCachedString(CSTID_LOADING_FILE), rest);
   } else {
      if (*gzipped_fname == '\0') {
         sprintf(gszMsgBox, TgLoadCachedString(CSTID_LOADING_FILE), file_name);
      } else {
         sprintf(gszMsgBox, TgLoadCachedString(CSTID_LOADING_FILE),
               gzipped_fname);
      }
   }
   SetStringStatus(gszMsgBox);
   SetWatchCursor(drawWindow);
   SetWatchCursor(mainWindow);

   if (flushColormapOnOpen) {
      if (FlushColormap()) {
         Msg(TgLoadString(STID_COLORMAP_FLUSHED));
      }
   }
   XClearWindow(mainDisplay, drawWindow);
   somethingHighLighted = FALSE;

   numRedrawBBox = 0;
   ShowInterrupt(1);
   interrupted = FALSE;
   readingPageNum = loadedCurPageNum = 0;
   foundGoodStateObject = FALSE;
   while ((read_status=ReadObj(fp, &obj_ptr)) == TRUE) {
      if (foundGoodStateObject && !done_clear_all_wb) {
         done_clear_all_wb = TRUE;
         if (gstWBInfo.do_whiteboard) {
            RecordWBClearAll();
         }
      }
      if (obj_ptr != NULL) {
         obj_ptr->tmp_parent = NULL;
         AdjForOldVersion(obj_ptr);
         AddObj(NULL, topObj, obj_ptr);
         if (!interrupted && readingPageNum == loadedCurPageNum &&
               (PointInBBox(obj_ptr->x, obj_ptr->y, drawWinBBox) ||
               BBoxIntersect(obj_ptr->bbox, drawWinBBox))) {
            if (!DrawObj(drawWindow, obj_ptr)) interrupted = TRUE;
            if (CheckInterrupt(TRUE)) interrupted = TRUE;
         }
      }
   }
   fclose(fp);
   if (*gzipped_fname != '\0') unlink(file_name);

   if (!PRTGIF && colorLayers && needToRedrawColorWindow) {
      RedrawColorWindow();
   }
   if (interrupted) {
      MsgBox(TgLoadString(STID_USER_INTR_ABORT_DRAW), TOOL_NAME, INFO_MB);
   }
   HideInterrupt();

   strcpy(scanFileFullPath, tmp_filefullpath);
   strcpy(scanFileName, tmp_filename);
   scanLineNum = tmp_linenum;

   if (read_status == INVALID) {
      if (fileVersion > CUR_VERSION) {
         sprintf(gszMsgBox, TgLoadString(STID_FILE_VER_ABORT_OPEN),
               fileVersion, TOOL_NAME, homePageURL);
      } else {
         sprintf(gszMsgBox, TgLoadString(STID_FILE_CORRUPTED_ABORT_OPEN));
      }
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      DrawPaperBoundary(drawWindow);
      RedrawGridLines(drawWindow);
      SetDefaultCursor(mainWindow);
      ShowCursor();

      RestoreStatusStrings();

      return FALSE;
   }
   if (ObjFile == TRUE) {
      if (*gzipped_fname == '\0') {
         SetCurDir(file_name);
      } else {
         SetCurDir(gzipped_fname);
      }
      *curSymDir = '\0';
   } else if (ObjFile == FALSE) {
      SetCurSymDir(file_name);
   }
   curFileDefined = TRUE;

   if (loadedCurPageNum > 0 && curPage != NULL) {
      GotoPageNum(loadedCurPageNum);
   }
   CheckFileAttrsInLoad();

   if (loadedCurPageNum <= 0 || curPage == NULL) {
      DelAllPages();
      loadedCurPageNum = curPageNum = lastPageNum = 1;
      InitPage();
      DrawPaperBoundary(drawWindow);
      RedrawGridLines(drawWindow);
      RedrawPageLines(drawWindow);
      RedrawChoiceWindow();
   }

   if (ObjFile != FALSE) {
      if (strcmp(saved_cur_dir, curDir) != 0 && DirInSymPath(".")) {
         UpdateSymInfo();
      }
   } else {
      if (strcmp(saved_cur_dir, curSymDir) != 0 && DirInSymPath(".")) {
         UpdateSymInfo();
      }
   }

   if (short_name) {
      sprintf(gszMsgBox, TgLoadCachedString(CSTID_CUR_FILE_IS), rest);
   } else {
      sprintf(gszMsgBox, TgLoadCachedString(CSTID_CUR_FILE_IS),
            (*gzipped_fname == '\0' ? file_name : gzipped_fname));
   }
   Msg(gszMsgBox);
   RestoreStatusStrings();

   RedrawTitleWindow();
   justDupped = FALSE;
   SetDefaultCursor(mainWindow);
   RedrawHRulerWindow();
   ShowCursor();

   if (!gstWBInfo.do_whiteboard) {
      CleanUpCmds();
   }
   UpdateAllPinnedMenus();

   if (!navigatingBackAndForth && !PRTGIF) CommitNavigate();
   return foundGoodStateObject;
}

void DumpPatFill(FP, Fill, BBox, Indent, EndOfLine)
   FILE *FP;
   int Fill, Indent, EndOfLine;
   struct BBRec BBox;
{
   int ltx=0, lty=0, rbx=0, rby=0, cellw=0, cellh=0;
   struct BBRec bbox;

   GetPatternCellsize(Fill, &cellw, &cellh);
   if ((cellw&0x7) != 0) {
      sprintf(gszMsgBox, "cellw = %1d in DumpPatFill()\n", cellw);
      TgAssert(FALSE, gszMsgBox, NULL);
   }
   /* if ((cellw % 8) != 0) cellw += (8-(cellw%8)); */

   bbox.ltx = BBox.ltx-1; bbox.lty = BBox.lty-1;
   bbox.rbx = BBox.rbx+1; bbox.rby = BBox.rby+1;

   ltx = ((bbox.ltx % cellw) == 0) ? bbox.ltx :
         ((bbox.ltx > 0) ? ((int)(bbox.ltx / cellw))*cellw :
         ((int)(bbox.ltx / cellw)-1)*cellw);
   lty = ((bbox.lty % cellh) == 0) ? bbox.lty :
         ((bbox.lty > 0) ? ((int)(bbox.lty / cellh))*cellh :
         ((int)(bbox.lty / cellh)-1)*cellh);
   rbx = ((bbox.rbx % cellw) == 0) ? bbox.rbx :
         ((bbox.rbx > 0) ? ((int)(bbox.rbx / cellw)+1)*cellw :
         ((int)(bbox.rbx / cellw))*cellw);
   rby = ((bbox.rby % cellh) == 0) ? bbox.rby :
         ((bbox.rby > 0) ? ((int)(bbox.rby / cellh)+1)*cellh :
         ((int)(bbox.rby / cellh))*cellh);

   if (fprintf(FP, "%spat%1d %1d %1d %1d %1d %1d %1d TGPF%s",
         GetIndentString(Indent), Fill, cellw, cellh,
         ltx, lty, rbx-ltx, rby-lty, (EndOfLine?"\n":"")) == EOF) {
      writeFileFailed = TRUE;
   }
}

void DumpSymOutline(FP, ObjPtr)
   FILE *FP;
   register struct ObjRec *ObjPtr;
{
   int ltx, lty, rbx, rby;

   ltx = ObjPtr->obbox.ltx - QUARTER_INCH + 1;
   lty = ObjPtr->obbox.lty - QUARTER_INCH + 1;
   rbx = ObjPtr->obbox.rbx + QUARTER_INCH - 1;
   rby = ObjPtr->obbox.rby + QUARTER_INCH - 1;

   fprintf(FP, "%s\n", gPsCmd[PS_GSAVE]);
   fprintf(FP, "   0 %s\n", gPsCmd[PS_SETGRAY]);
   fprintf(FP, "   [4 4] 0 %s\n", gPsCmd[PS_SETDASH]);
   fprintf(FP, "   %s\n   %1d %1d %s ",
         gPsCmd[PS_NEWPATH], ltx, lty, gPsCmd[PS_MOVETO]);
   fprintf(FP, "%1d %1d %s ", rbx, lty, gPsCmd[PS_LINETO]);
   fprintf(FP, "%1d %1d %s ", rbx, rby, gPsCmd[PS_LINETO]);
   fprintf(FP, "%1d %1d %s\n", ltx, rby, gPsCmd[PS_LINETO]);
   fprintf(FP, "   %s %s\n", gPsCmd[PS_CLOSEPATH], gPsCmd[PS_STROKE]);
   fprintf(FP, "%s\n", gPsCmd[PS_GRESTORE]);
}

static int	printingFirstPageNum=1;
static int	printingPageNum=1, printingPageRow=1, printingPageCol=1;
static int	printingLastPageNum=0;
static int	dumpOnePageInTileMode=FALSE;
static int	dumpOnePageInStackMode=FALSE;
static int	dumpPages=FALSE;
static int	dumpPageNum=0;

static FILE	*dumpFP=NULL;
static char	tmpFile[MAXSTRING+1];
static int	llxTotal=0, llyTotal=0, urxTotal=0, uryTotal=0;
static int	totalBBoxValid=FALSE;

static int	msgAboutTiledPageScalingSeen=FALSE;

static
void DumpAttrs(FP, AttrPtr)
   FILE *FP;
   register struct AttrRec *AttrPtr;
{
   for ( ; AttrPtr != NULL; AttrPtr=AttrPtr->prev) {
      if (!AttrPtr->shown) {
         continue;
      }
      /* do not translate -- program constants */
      if (!AttrPtr->nameshown && *AttrPtr->attr_name.s == '!' &&
            strcmp(AttrPtr->attr_name.s, "!PAGE_NUM=") == 0) {
         if (pageLayoutMode == PAGE_STACK) {
            MiniLineInfo *pMiniLine=AttrPtr->obj->detail.t->minilines.first;
            char *c_ptr;

            if (pMiniLine != NULL &&
                  pMiniLine->first_block == pMiniLine->last_block &&
                  (strstr(pMiniLine->first_block->seg->dyn_str.s,
                  "!(STACKED_PAGE_NUM)") != NULL ||
                  strstr(pMiniLine->first_block->seg->dyn_str.s,
                  "!(STACKED_NUM_PAGES)") != NULL)) {
               char *psz_saved=NULL, *c_ptr1=NULL;
               int page_num_str_len=strlen("!(STACKED_PAGE_NUM)");
               int num_pages_str_len=strlen("!(STACKED_NUM_PAGES)");

               psz_saved = UtilStrDup(pMiniLine->first_block->seg->dyn_str.s);
               if (psz_saved == NULL) FailAllocMessage();
               while ((c_ptr=strstr(pMiniLine->first_block->seg->dyn_str.s,
                     "!(STACKED_PAGE_NUM)")) != NULL) {
                  c_ptr1 = &c_ptr[page_num_str_len];
                  sprintf(c_ptr, "%1d", printingPageNum);
                  c_ptr = &c_ptr[strlen(c_ptr)];
                  while (*c_ptr1 != '\0') *c_ptr++ = *c_ptr1++;
                  *c_ptr = '\0';
               }
               while ((c_ptr=strstr(pMiniLine->first_block->seg->dyn_str.s,
                     "!(STACKED_NUM_PAGES)")) != NULL) {
                  c_ptr1 = &c_ptr[num_pages_str_len];
                  sprintf(c_ptr, "%1d", lastPageNum);
                  c_ptr = &c_ptr[strlen(c_ptr)];
                  while (*c_ptr1 != '\0') *c_ptr++ = *c_ptr1++;
                  *c_ptr = '\0';
               }
               DumpTextObj(FP, AttrPtr->obj);
               DynStrSet(&pMiniLine->first_block->seg->dyn_str, psz_saved);
               UtilFree(psz_saved);
            } else {
               DumpTextObj(FP, AttrPtr->obj);
            }
         } else {
            /* pageLayoutMode == PAGE_TILE */
            MiniLineInfo *pMiniLine=AttrPtr->obj->detail.t->minilines.first;
            char *c_ptr=NULL;

            if (pMiniLine != NULL) {
               char *psz_saved=NULL, *c_ptr1=NULL, *c_ptr2=NULL;

               psz_saved = UtilStrDup(pMiniLine->first_block->seg->dyn_str.s);
               if (psz_saved == NULL) FailAllocMessage();
               for (c_ptr=pMiniLine->first_block->seg->dyn_str.s;
                     *c_ptr != '\0'; c_ptr++) {
                  int len=strlen("!(TILED_PAGE_ROW)");

                  if (*c_ptr == '!' &&
                        (strncmp(c_ptr, "!(TILED_PAGE_ROW)", len) == 0 ||
                        strncmp(c_ptr, "!(TILED_PAGE_COL)", len) == 0)) {
                     if (strncmp(c_ptr, "!(TILED_PAGE_ROW)", len) == 0) {
                        sprintf(c_ptr, "%1d", printingPageRow);
                     } else {
                        sprintf(c_ptr, "%1d", printingPageCol);
                     }
                     c_ptr1 = &c_ptr[len];
                     c_ptr = c_ptr2 = &c_ptr[strlen(c_ptr)];
                     while (*c_ptr1 != '\0') *c_ptr2++ = *c_ptr1++;
                     *c_ptr2 = '\0';
                     c_ptr--;
                  }
               }
               DumpTextObj(FP, AttrPtr->obj);
               DynStrSet(&pMiniLine->first_block->seg->dyn_str, psz_saved);
               UtilFree(psz_saved);
            } else {
               DumpTextObj(FP, AttrPtr->obj);
            }
         }
      } else {
         DumpTextObj(FP, AttrPtr->obj);
      }
   }
}

static
void DumpAnObj(FP, ObjPtr)
   FILE *FP;
   register struct ObjRec *ObjPtr;
{
   switch (ObjPtr->type) {
   case OBJ_POLY:
      if (!colorLayers ||
            ObjPtr->tmp_parent!=NULL || ObjInVisibleLayer(ObjPtr)) {
         DumpPolyObj(FP, ObjPtr);
         DumpAttrs(FP, ObjPtr->lattr);
      }
      break;
   case OBJ_BOX:
      if (!colorLayers ||
            ObjPtr->tmp_parent!=NULL || ObjInVisibleLayer(ObjPtr)) {
         DumpBoxObj(FP, ObjPtr);
         DumpAttrs(FP, ObjPtr->lattr);
      }
      break;
   case OBJ_OVAL:
      if (!colorLayers ||
            ObjPtr->tmp_parent!=NULL || ObjInVisibleLayer(ObjPtr)) {
         DumpOvalObj(FP, ObjPtr);
         DumpAttrs(FP, ObjPtr->lattr);
      }
      break;
   case OBJ_TEXT:
      if (!colorLayers ||
            ObjPtr->tmp_parent!=NULL || ObjInVisibleLayer(ObjPtr)) {
         DumpTextObj(FP, ObjPtr);
      }
      break;
   case OBJ_POLYGON:
      if (!colorLayers ||
            ObjPtr->tmp_parent!=NULL || ObjInVisibleLayer(ObjPtr)) {
         DumpPolygonObj(FP, ObjPtr);
         DumpAttrs(FP, ObjPtr->lattr);
      }
      break;
   case OBJ_ARC:
      if (!colorLayers ||
            ObjPtr->tmp_parent!=NULL || ObjInVisibleLayer(ObjPtr)) {
         DumpArcObj(FP, ObjPtr);
         DumpAttrs(FP, ObjPtr->lattr);
      }
      break;
   case OBJ_RCBOX:
      if (!colorLayers ||
            ObjPtr->tmp_parent!=NULL || ObjInVisibleLayer(ObjPtr)) {
         DumpRCBoxObj(FP, ObjPtr);
         DumpAttrs(FP, ObjPtr->lattr);
      }
      break;
   case OBJ_XBM:
      if (!colorLayers ||
            ObjPtr->tmp_parent!=NULL || ObjInVisibleLayer(ObjPtr)) {
         DumpXBmObj(FP, ObjPtr);
         DumpAttrs(FP, ObjPtr->lattr);
      }
      break;
   case OBJ_XPM:
      if (!colorLayers ||
            ObjPtr->tmp_parent!=NULL || ObjInVisibleLayer(ObjPtr)) {
         DumpXPmObj(FP, ObjPtr);
         DumpAttrs(FP, ObjPtr->lattr);
      }
      break;
   case OBJ_SYM:
   case OBJ_ICON:
   case OBJ_GROUP:
      if (!colorLayers ||
            ObjPtr->tmp_parent!=NULL || ObjInVisibleLayer(ObjPtr)) {
         struct ObjRec *obj_ptr=ObjPtr->detail.r->last;

         for ( ; obj_ptr != NULL; obj_ptr = obj_ptr->prev) {
            obj_ptr->tmp_parent = ObjPtr;
            DumpAnObj(FP, obj_ptr);
         }
         DumpAttrs(FP, ObjPtr->lattr);
         if (ObjPtr->type == OBJ_SYM) DumpSymOutline(FP, ObjPtr);
      }
      break;
   case OBJ_PIN:
      if (!colorLayers ||
            ObjPtr->tmp_parent!=NULL || ObjInVisibleLayer(ObjPtr)) {
         struct ObjRec *obj_ptr=GetPinObj(ObjPtr);

         obj_ptr->tmp_parent = ObjPtr;
         DumpAnObj(FP, obj_ptr);

         DumpAttrs(FP, ObjPtr->lattr);
      }
      break;
   }
}

static int	llxPage=0, llyPage=0, urxPage=0, uryPage=0;
static int	minimalEPS=INVALID;

#define ALL_BBOX 0
#define PAGE_BBOX 1

int DumpBBox(fp, page_only, page_bbox)
   FILE *fp;
   int page_only;
   struct BBRec *page_bbox;
{
   struct ObjRec *obj_ptr=NULL;
   int ltx=0, lty=0, rbx=0, rby=0, rc=TRUE;
   double llx1=0, lly1=0, urx1=0, ury1=0;

   if ((obj_ptr = topObj) == NULL) {
      ltx = lty = rbx = rby = 0;
      if (!(pageLayoutMode == PAGE_STACK &&
            printingFirstPageNum != printingLastPageNum)) {
         strcpy(gszMsgBox, TgLoadString(STID_WARN_PS_BBOX_EMPTY));
         if (PRTGIF) {
            fprintf(stderr, "%s\n", gszMsgBox);
         } else {
            Msg(gszMsgBox);
         }
      }
      rc = FALSE;
   } else if (pageLayoutMode == PAGE_STACK || page_bbox == NULL) {
      ltx = obj_ptr->bbox.ltx; lty = obj_ptr->bbox.lty;
      rbx = obj_ptr->bbox.rbx; rby = obj_ptr->bbox.rby;

      for (obj_ptr=topObj->next; obj_ptr != NULL; obj_ptr=obj_ptr->next) {
         if (obj_ptr->bbox.ltx < ltx) ltx = obj_ptr->bbox.ltx;
         if (obj_ptr->bbox.lty < lty) lty = obj_ptr->bbox.lty;
         if (obj_ptr->bbox.rbx > rbx) rbx = obj_ptr->bbox.rbx;
         if (obj_ptr->bbox.rby > rby) rby = obj_ptr->bbox.rby;
      }
   } else {
      int found=FALSE;

      for (obj_ptr=botObj; obj_ptr != NULL; obj_ptr=obj_ptr->prev) {
         if (Inside(obj_ptr->bbox, *page_bbox) ||
               BBoxIntersect(obj_ptr->bbox, *page_bbox)) {
            if (found) {
               if (obj_ptr->bbox.ltx < ltx) ltx = obj_ptr->bbox.ltx;
               if (obj_ptr->bbox.lty < lty) lty = obj_ptr->bbox.lty;
               if (obj_ptr->bbox.rbx > rbx) rbx = obj_ptr->bbox.rbx;
               if (obj_ptr->bbox.rby > rby) rby = obj_ptr->bbox.rby;
            } else {
               found = TRUE;
               ltx = obj_ptr->bbox.ltx; lty = obj_ptr->bbox.lty;
               rbx = obj_ptr->bbox.rbx; rby = obj_ptr->bbox.rby;
            }
         }
      }
      if (!found) {
         rc = FALSE;
         ltx = lty = rbx = rby = 0;
      } else if (page_bbox != NULL) {
         struct BBRec bbox, bbox1;

         bbox.ltx = ltx; bbox.lty = lty; bbox.rbx = rbx; bbox.rby = rby;
         if (IntersectRect(bbox, *page_bbox, &bbox1)) {
            ltx = bbox1.ltx-page_bbox->ltx; lty = bbox1.lty-page_bbox->lty;
            rbx = bbox1.rbx-page_bbox->ltx; rby = bbox1.rby-page_bbox->lty;
         }
      }
   }
   switch (pageStyle) {
   case PORTRAIT:
      llx1 = (double)(1.0*ltx*psDotsPerInch/PIX_PER_INCH*printMag/100 +
            psXOff[pageStyle]*psDotsPerInch);
      lly1 = (double)(-1.0*rby*psDotsPerInch/PIX_PER_INCH*printMag/100 +
            psYOff[pageStyle]*psDotsPerInch);
      urx1 = (double)(1.0*rbx*psDotsPerInch/PIX_PER_INCH*printMag/100 +
            psXOff[pageStyle]*psDotsPerInch);
      ury1 = (double)(-1.0*lty*psDotsPerInch/PIX_PER_INCH*printMag/100 +
            psYOff[pageStyle]*psDotsPerInch);
      break;
   case LANDSCAPE:
      llx1 = (double)(1.0*lty*psDotsPerInch/PIX_PER_INCH*printMag/100 -
            psYOff[pageStyle]*psDotsPerInch);
      lly1 = (double)(1.0*ltx*psDotsPerInch/PIX_PER_INCH*printMag/100 +
            psXOff[pageStyle]*psDotsPerInch);
      urx1 = (double)(1.0*rby*psDotsPerInch/PIX_PER_INCH*printMag/100 -
            psYOff[pageStyle]*psDotsPerInch);
      ury1 = (double)(1.0*rbx*psDotsPerInch/PIX_PER_INCH*printMag/100 +
            psXOff[pageStyle]*psDotsPerInch);
      break;
   default:
      fprintf(stderr, TgLoadString(STID_UNRECOG_PAGE_STYLE), pageStyle);
      fprintf(stderr, "\n");
      break;
   }

   if (rc) {
      llxPage = (llx1 >= 0.0) ? ((int)llx1)-1 : (-((int)(-llx1)))-1;
      llyPage = (lly1 >= 0.0) ? ((int)lly1)-1 : (-((int)(-lly1)))-1;
      urxPage = (urx1 >= 0.0) ? ((int)urx1)+1 : (-((int)(0.999-urx1)))+1;
      uryPage = (ury1 >= 0.0) ? ((int)ury1)+1 : (-((int)(0.999-ury1)))+1;
   } else {
      llxPage = llyPage = urxPage = uryPage = 0;
   }
   if (!page_only || !minimalEPS) {
      fprintf(fp,"%%%%%sBoundingBox: %1d %1d %1d %1d\n",
            (page_only ? "Page" : ""), llxPage, llyPage, urxPage, uryPage);
   }
   return rc;
}

void ModifyOutputFileName(FileName)
   char *FileName;
{
   char *psz=NULL, s[MAXPATHLENGTH<<1];

   if (*outputDir == '\0') return;

   strcpy(s, FileName);
   psz = UtilStrRChr(s, DIR_SEP);
   if (psz != NULL) {
      sprintf(FileName, "%s%c%s", outputDir, DIR_SEP, ++psz);
   } else {
      sprintf(FileName, "%s%c%s", outputDir, DIR_SEP, s);
   }
}

static
void DumpTextObjInAscii(FP, ObjPtr)
   FILE *FP;
   register struct ObjRec *ObjPtr;
{
   struct TextRec *text_ptr=ObjPtr->detail.t;
   MiniLinesInfo *minilines=(&text_ptr->minilines);

   if (text_ptr->pen == NONEPAT) return;
   DumpMiniLinesInAscii(FP, minilines, &totalBBoxValid);
   if (fprintf(FP, "\n") == EOF) writeFileFailed = TRUE;
}

static
void DumpAttrsInAscii(FP, AttrPtr)
   FILE *FP;
   register struct AttrRec *AttrPtr;
{
   for ( ; AttrPtr != NULL; AttrPtr=AttrPtr->prev) {
      if (!AttrPtr->shown) {
         continue;
      }
      /* do not translate -- program constants */
      if (!AttrPtr->nameshown && *AttrPtr->attr_name.s == '!' &&
            strcmp(AttrPtr->attr_name.s, "!PAGE_NUM=") == 0) {
         if (pageLayoutMode == PAGE_STACK) {
            MiniLineInfo *pMiniLine=AttrPtr->obj->detail.t->minilines.first;
            char *c_ptr;

            if (pMiniLine != NULL &&
                  pMiniLine->first_block == pMiniLine->last_block &&
                  (strstr(pMiniLine->first_block->seg->dyn_str.s,
                  "!(STACKED_PAGE_NUM)") != NULL ||
                  strstr(pMiniLine->first_block->seg->dyn_str.s,
                  "!(STACKED_NUM_PAGES)") != NULL)) {
               char *psz_saved=NULL, *c_ptr1;
               int page_num_str_len=strlen("!(STACKED_PAGE_NUM)");
               int num_pages_str_len=strlen("!(STACKED_NUM_PAGES)");

               psz_saved = UtilStrDup(pMiniLine->first_block->seg->dyn_str.s);
               if (psz_saved == NULL) FailAllocMessage();
               while ((c_ptr=strstr(pMiniLine->first_block->seg->dyn_str.s,
                     "!(STACKED_PAGE_NUM)")) != NULL) {
                  c_ptr1 = &c_ptr[page_num_str_len];
                  sprintf(c_ptr, "%1d", printingPageNum);
                  c_ptr = &c_ptr[strlen(c_ptr)];
                  while (*c_ptr1 != '\0') *c_ptr++ = *c_ptr1++;
                  *c_ptr = '\0';
               }
               while ((c_ptr=strstr(pMiniLine->first_block->seg->dyn_str.s,
                     "!(STACKED_NUM_PAGES)")) != NULL) {
                  c_ptr1 = &c_ptr[num_pages_str_len];
                  sprintf(c_ptr, "%1d", lastPageNum);
                  c_ptr = &c_ptr[strlen(c_ptr)];
                  while (*c_ptr1 != '\0') *c_ptr++ = *c_ptr1++;
                  *c_ptr = '\0';
               }
               DumpTextObjInAscii(FP, AttrPtr->obj);
               DynStrSet(&pMiniLine->first_block->seg->dyn_str, psz_saved);
               UtilFree(psz_saved);
            } else {
               DumpTextObjInAscii(FP, AttrPtr->obj);
            }
         } else {
            MiniLineInfo *pMiniLine=AttrPtr->obj->detail.t->minilines.first;
            char *c_ptr;

            if (pMiniLine != NULL) {
               char *psz_saved, *c_ptr1, *c_ptr2;

               psz_saved = UtilStrDup(pMiniLine->first_block->seg->dyn_str.s);
               if (psz_saved == NULL) FailAllocMessage();
               for (c_ptr=pMiniLine->first_block->seg->dyn_str.s;
                     *c_ptr != '\0'; c_ptr++) {
                  int len=strlen("!(TILED_PAGE_ROW)");

                  if (*c_ptr == '!' &&
                        (strncmp(c_ptr, "!(TILED_PAGE_ROW)", len) == 0 ||
                        strncmp(c_ptr, "!(TILED_PAGE_COL)", len) == 0)) {
                     if (strncmp(c_ptr, "!(TILED_PAGE_ROW)", len) == 0) {
                        sprintf(c_ptr, "%1d", printingPageRow);
                     } else {
                        sprintf(c_ptr, "%1d", printingPageCol);
                     }
                     c_ptr1 = &c_ptr[len];
                     c_ptr = c_ptr2 = &c_ptr[strlen(c_ptr)];
                     while (*c_ptr1 != '\0') *c_ptr2++ = *c_ptr1++;
                     *c_ptr2 = '\0';
                     c_ptr--;
                  }
               }
               DumpTextObjInAscii(FP, AttrPtr->obj);
               DynStrSet(&pMiniLine->first_block->seg->dyn_str, psz_saved);
               UtilFree(psz_saved);
            } else {
               DumpTextObjInAscii(FP, AttrPtr->obj);
            }
         }
      } else {
         DumpTextObjInAscii(FP, AttrPtr->obj);
      }
   }
}

static
void DumpAnObjInAscii(FP, ObjPtr)
   FILE *FP;
   register struct ObjRec *ObjPtr;
{
   switch (ObjPtr->type) {
   case OBJ_POLY:
   case OBJ_BOX:
   case OBJ_OVAL:
   case OBJ_POLYGON:
   case OBJ_ARC:
   case OBJ_RCBOX:
   case OBJ_XBM:
   case OBJ_XPM:
      if (!colorLayers ||
            ObjPtr->tmp_parent!=NULL || ObjInVisibleLayer(ObjPtr)) {
         DumpAttrsInAscii(FP, ObjPtr->lattr);
      }
      break;
   case OBJ_TEXT:
      if (!colorLayers ||
            ObjPtr->tmp_parent!=NULL || ObjInVisibleLayer(ObjPtr)) {
         DumpTextObjInAscii(FP, ObjPtr);
      }
      break;
   case OBJ_SYM:
   case OBJ_ICON:
   case OBJ_GROUP:
      if (!colorLayers ||
            ObjPtr->tmp_parent!=NULL || ObjInVisibleLayer(ObjPtr)) {
         struct ObjRec *obj_ptr=ObjPtr->detail.r->last;

         for ( ; obj_ptr != NULL; obj_ptr = obj_ptr->prev) {
            obj_ptr->tmp_parent = ObjPtr;
            DumpAnObjInAscii(FP, obj_ptr);
         }
         DumpAttrsInAscii(FP, ObjPtr->lattr);
      }
      break;
   case OBJ_PIN:
      if (!colorLayers ||
            ObjPtr->tmp_parent!=NULL || ObjInVisibleLayer(ObjPtr)) {
         struct ObjRec *obj_ptr=GetPinObj(ObjPtr);

         obj_ptr->tmp_parent = ObjPtr;
         DumpAnObjInAscii(FP, obj_ptr);

         DumpAttrsInAscii(FP, ObjPtr->lattr);
      }
      break;
   }
}

static
int SetPortName(name, buf, buf_sz, pn_buf_index)
   char *name, *buf;
   int buf_sz, *pn_buf_index;
{
   int len=strlen(name)+1, index=(*pn_buf_index);

   if (index+len+1 > buf_sz) {
      sprintf(gszMsgBox, TgLoadCachedString(CSTID_PORTNAME_TOO_LONG_SKIPPED),
            buf);
      if (PRTGIF) {
         fprintf(stderr, "%s\n", gszMsgBox);
      } else {
         Msg(gszMsgBox);
      }
      return FALSE;
   }
   buf[index] = '!';
   strcpy(&buf[index+1], name);
   (*pn_buf_index) = index+len;
   buf[(*pn_buf_index)] = '\0';

   return TRUE;
}

static
void DumpNetlListLine(FP, port_name, pMiniLine)
   FILE *FP;
   char *port_name;
   MiniLineInfo *pMiniLine;
{
   int need_to_free_tmp_buf=FALSE;
   char *tmp_buf=ConvertMiniLineToString(pMiniLine, &need_to_free_tmp_buf);
   char *c_ptr=strchr(tmp_buf, '=');

   if (fprintf(FP, "\"%s\",\t\"%s\"\n", port_name, ++c_ptr) == EOF) {
      writeFileFailed = TRUE;
   }
   if (need_to_free_tmp_buf) UtilFree(tmp_buf);
}

static
void DumpNetListForAPort(FP, ObjPtr, buf, buf_sz, pn_buf_index)
   FILE *FP;
   struct ObjRec *ObjPtr;
   char *buf;
   int buf_sz, *pn_buf_index;
{
   struct AttrRec *attr_ptr=FindObjAttrWithName(ObjPtr, "name=");
   int ok=TRUE;

   if (attr_ptr != NULL) {
      if (BlankStr(attr_ptr->attr_value.s)) {
         /* do not translate -- program constants */
         ok = SetPortName("(unknown)", buf, buf_sz, pn_buf_index);
      } else {
         ok = SetPortName(attr_ptr->attr_value.s, buf, buf_sz,
               pn_buf_index);
      }
   }
   if (ok) {
      attr_ptr = FindAttrWithName(ObjPtr, "signal_name=", NULL);
      DumpNetlListLine(FP, buf, attr_ptr->obj->detail.t->minilines.first);
   }
}

static
void DoDumpNetListForAnObj(FP, ObjPtr)
   FILE *FP;
   struct ObjRec *ObjPtr;
{
   struct ObjRec *obj_ptr=NULL;
   struct AttrRec *attr_ptr=NULL;
   char port_name[MAXSTRING];

   switch (ObjPtr->type) {
   case OBJ_SYM:
   case OBJ_ICON:
      if ((attr_ptr=FindObjAttrWithName(ObjPtr, "name=")) != NULL) {
         int index=0, ok=TRUE;

         if (BlankStr(attr_ptr->attr_value.s)) {
            /* do not translate -- program constants */
            ok = SetPortName("(unknown)", port_name, sizeof(port_name), &index);
         } else {
            ok = SetPortName(attr_ptr->attr_value.s, port_name,
                  sizeof(port_name), &index);
         }
         if (ok) {
            if (ObjIsAPort(ObjPtr)) {
               attr_ptr = FindAttrWithName(ObjPtr, "signal_name=", NULL);
               DumpNetlListLine(FP, port_name,
                     attr_ptr->obj->detail.t->minilines.first);
            } else {
               for (obj_ptr=ObjPtr->detail.r->last; obj_ptr != NULL;
                     obj_ptr=obj_ptr->prev) {
                  obj_ptr->tmp_parent = ObjPtr;
                  if (obj_ptr->type != OBJ_TEXT && ObjIsAPort(obj_ptr)) {
                     int saved_index=index;

                     port_name[saved_index] = '\0';
                     DumpNetListForAPort(FP, obj_ptr, port_name,
                           sizeof(port_name), &index);
                     index = saved_index;
                  }
               }
            }
         }
      }
      break;
   case OBJ_GROUP:
      for (obj_ptr=ObjPtr->detail.r->last; obj_ptr != NULL;
            obj_ptr=obj_ptr->prev) {
         obj_ptr->tmp_parent = ObjPtr;
         DoDumpNetListForAnObj(FP, obj_ptr);
      }
      break;
   case OBJ_PIN:
      obj_ptr = GetPinObj(ObjPtr);
      if ((attr_ptr=FindObjAttrWithName(obj_ptr, "name=")) != NULL) {
         int index=0, ok=TRUE;

         if (BlankStr(attr_ptr->attr_value.s)) {
            /* do not translate -- program constants */
            ok = SetPortName("(unknown)", port_name, sizeof(port_name), &index);
         } else {
            ok = SetPortName(attr_ptr->attr_value.s, port_name,
                  sizeof(port_name), &index);
         }
         if (ok) {
            if (ObjIsAPort(obj_ptr)) {
               attr_ptr = FindAttrWithName(obj_ptr, "signal_name=", NULL);
               DumpNetlListLine(FP, port_name,
                     attr_ptr->obj->detail.t->minilines.first);
            }
         }
      }
      break;
   }
}

static
void DumpComponentAttrs(FP, ObjPtr)
   FILE *FP;
   struct ObjRec *ObjPtr;
{
   struct AttrRec *attr_ptr=NULL;

   for (attr_ptr=ObjPtr->fattr; attr_ptr != NULL; attr_ptr=attr_ptr->next) {
      if (strcmp(attr_ptr->attr_name.s, "name=") != 0 &&
            strcmp(attr_ptr->attr_name.s, "type=") != 0) {
         int need_to_free_tmp_buf=FALSE;
         char *tmp_buf;

         tmp_buf = ConvertAttrNameFirstMiniLineToString(attr_ptr,
               &need_to_free_tmp_buf);
         if (fprintf(FP, "\t%s\n", tmp_buf) == EOF) writeFileFailed = TRUE;
         if (need_to_free_tmp_buf) UtilFree(tmp_buf);
      }
   }
}

static
void DoDumpComponentForAnObj(FP, ObjPtr)
   FILE *FP;
   struct ObjRec *ObjPtr;
{
   struct ObjRec *obj_ptr=NULL;
   struct AttrRec *attr_ptr=NULL;

   switch (ObjPtr->type) {
   case OBJ_SYM:
   case OBJ_ICON:
      if ((attr_ptr=FindObjAttrWithName(ObjPtr, "name=")) != NULL) {
         char *comp_name=NULL;

         if (BlankStr(attr_ptr->attr_value.s)) {
            /* do not translate -- program constants */
            comp_name = "(unknown)";
         } else {
            comp_name = attr_ptr->attr_value.s;
         }
         attr_ptr = FindAttrWithName(ObjPtr, "type=", NULL);
         if (attr_ptr != NULL) {
            char *comp_type=attr_ptr->attr_value.s;

            if (fprintf(FP, "%s: %s\n", comp_name, comp_type) == EOF) {
               writeFileFailed = TRUE;
            }
            DumpComponentAttrs(FP, ObjPtr);
         }
      }
      break;
   case OBJ_GROUP:
      for (obj_ptr=ObjPtr->detail.r->last; obj_ptr != NULL;
            obj_ptr=obj_ptr->prev) {
         obj_ptr->tmp_parent = ObjPtr;
         DoDumpComponentForAnObj(FP, obj_ptr);
      }
      break;
   case OBJ_PIN:
      obj_ptr = GetPinObj(ObjPtr);
      if ((attr_ptr=FindObjAttrWithName(obj_ptr, "name=")) != NULL) {
         char *comp_name=NULL;

         if (BlankStr(attr_ptr->attr_value.s)) {
            /* do not translate -- program constants */
            comp_name = "(unknown)";
         } else {
            comp_name = attr_ptr->attr_value.s;
         }
         attr_ptr = FindAttrWithName(obj_ptr, "type=", NULL);
         if (attr_ptr != NULL) {
            char *comp_type=attr_ptr->attr_value.s;

            if (fprintf(FP, "%s: %s\n", comp_name, comp_type) == EOF) {
               writeFileFailed = TRUE;
            }
            DumpComponentAttrs(FP, ObjPtr);
         }
      }
      break;
   }
}

static
void DumpNetListForAnObj(FP, ObjPtr)
   FILE *FP;
   struct ObjRec *ObjPtr;
{
   connectingPortsFromInternalCommand = FALSE;
   if (preDumpSetup) {
      /* dumping components */
      DoDumpComponentForAnObj(FP, ObjPtr);
   } else {
      /* dumping netlist */
      DoDumpNetListForAnObj(FP, ObjPtr);
   }
   connectingPortsFromInternalCommand = TRUE;
}

#define WRITEBYTE(fp,byte) fputc((byte),(fp))

static
void WriteWord(fp, word)
   FILE *fp;
   unsigned short word;
{
   WRITEBYTE(fp, word&0xff);
   WRITEBYTE(fp, ((unsigned short)(word&0xff00))>>8);
}

static
void WriteDoubleWord(fp, dword)
   FILE *fp;
   unsigned long dword;
{
   WRITEBYTE(fp, dword&0xff);
   WRITEBYTE(fp, (dword&0xff00)>>8);
   WRITEBYTE(fp, (dword&0xff0000)>>16);
   WRITEBYTE(fp, (dword&0xff000000)>>24);
}

static char	psBopHook[MAXSTRING], psEopHook[MAXSTRING];
static int	psBopHookStatus=INVALID, psEopHookStatus=INVALID;
static int	noOrientationIfPdfSetPageDevice=INVALID;
static int	colorBgInPrintingColorPS=INVALID;
static char	xbmToTiffCmd[MAXSTRING+1];
static char	psToPdfCmd[MAXSTRING+1];
static char	epsToTmpSvgCmd[(MAXSTRING<<1)+1];
static char	tmpSvgToSvgCmd[(MAXSTRING<<1)+1];
static char	epsiExportExtension[MAXSTRING];
static int	overrideEPSIExportExtension=INVALID;
static char	pdfExportExtension[MAXSTRING];
static char	tmpSvgExportExtension[MAXSTRING];
static int	numberFileInPrintOnePage=INVALID;

static char defXbmToTiffCmd[]="xbmtopbm %s | pnmtotiff -none > %s";
static char defPsToPdfCmd[]="ps2pdf \"%s\" \"%s\"";
static char defEpsToTmpSvgCmd[]="pstoedit -dt -f sk \"%s\" \"%s\"";
static char defTmpSvgToSvgCmd[]="uniconvertor \"%s\" \"%s\"";

/*
 * 1)  pstoedit -dt -f sk FILE.eps FILE.sk
 *     skconvert FILE.sk FILE.svg
 * 
 * 2)  pstoedit -dt -f sk FILE.eps FILE.sk
 *     uniconvertor FILE.sk FILE.svg
 * 
 * 3)  pstoedit -dt -f ps2ai FILE.eps FILE.ai
 *     uniconvertor FILE.ai FILE.svg
 */

void SetPsSetup(buf)
   char *buf;
{
   gpszPsSetup = UtilStrDup(buf);
   if (gpszPsSetup == NULL) FailAllocMessage();
   psSetupStatus = TRUE;
}

void SetBopHook(buf)
   char *buf;
{
   UtilStrCpyN(psBopHook, sizeof(psBopHook), buf);
   psBopHookStatus = TRUE;
}

void SetEopHook(buf)
   char *buf;
{
   UtilStrCpyN(psEopHook, sizeof(psEopHook), buf);
   psEopHookStatus = TRUE;
}

static
void CleanUpDumpInitDefaults()
{
   *psBopHook = *psEopHook = '\0';
   psBopHookStatus = psEopHookStatus = INVALID;
   minimalEPS = INVALID;
   noOrientationIfPdfSetPageDevice = INVALID;
   colorBgInPrintingColorPS = INVALID;
   *xbmToTiffCmd = '\0';
   *psToPdfCmd = '\0';
   *epsToTmpSvgCmd = '\0';
   *tmpSvgToSvgCmd = '\0';
   *epsiExportExtension = '\0';
   *pdfExportExtension = '\0';
   *tmpSvgExportExtension = '\0';
   overrideEPSIExportExtension = INVALID;
   numberFileInPrintOnePage = INVALID;
}

int GetCmdUsingDefAndXDef(buf, buf_sz, def_cmd, option, verify_count,
      msg_box_if_error)
   char *buf, *def_cmd, *option;
   int buf_sz, verify_count, msg_box_if_error;
{
   char *c_ptr=NULL;

   UtilStrCpyN(buf, buf_sz, def_cmd);
   if (!PRTGIF || cmdLineOpenDisplay) {
      if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,option)) != NULL) {
         UtilStrCpyN(buf, buf_sz, c_ptr);
         if (verify_count > 0) {
            int count=0;

            for (c_ptr=strstr(buf,"%s"); c_ptr!=NULL;
                  c_ptr=strstr(++c_ptr,"%s")) {
               count++;
            }
            if (count != verify_count) {
               sprintf(gszMsgBox, TgLoadString(STID_INVALID_XDEF_USE_ALT_STR),
                     TOOL_NAME, option, buf, def_cmd);
               fprintf(stderr, "%s\n", gszMsgBox);
               if (!PRTGIF && msg_box_if_error) {
                  MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
               }
               UtilStrCpyN(buf, buf_sz, def_cmd);
               return FALSE;
            }
         }
      }
   }
   return TRUE;
}

static
void GenDumpInitDefaults()
{
   static int init_other_xdefs=FALSE;
   char *c_ptr;

   if (minimalEPS == INVALID) {
      minimalEPS = !usePsAdobeString;
#ifdef _MINIMAL_EPS
      minimalEPS = (_MINIMAL_EPS != 0);
#endif /* _MINIMAL_EPS */
      if (!PRTGIF || cmdLineOpenDisplay) {
         if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"MinimalEPS")) != NULL) {
            minimalEPS = (UtilStrICmp(c_ptr, "true") == 0);
         }
      }
   }
   if (noOrientationIfPdfSetPageDevice == INVALID) {
      noOrientationIfPdfSetPageDevice = FALSE;
      if (!PRTGIF) {
         if ((c_ptr=XGetDefault(mainDisplay, TOOL_NAME,
               "NoOrientationIfPdfSetPageDevice")) != NULL &&
               UtilStrICmp(c_ptr, "true") == 0) {
            noOrientationIfPdfSetPageDevice = TRUE;
         }
      }
   }
   if (colorBgInPrintingColorPS == INVALID) {
      colorBgInPrintingColorPS = FALSE;
      if (!PRTGIF) {
         if ((c_ptr=XGetDefault(mainDisplay, TOOL_NAME,
               "ColorBgInPrintingColorPS")) != NULL &&
               UtilStrICmp(c_ptr, "true") == 0) {
            colorBgInPrintingColorPS = TRUE;
         }
      }
   }
   if (psBopHookStatus == INVALID) {
      *psBopHook = '\0';
      if (!PRTGIF) {
         if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"PSBopHook")) != NULL) {
            UtilStrCpyN(psBopHook, sizeof(psBopHook), c_ptr);
            psBopHookStatus = TRUE;
         } else {
            psBopHookStatus = FALSE;
         }
      }
   }
   if (psEopHookStatus == INVALID) {
      *psEopHook = '\0';
      if (!PRTGIF) {
         if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"PSEopHook")) != NULL) {
            UtilStrCpyN(psEopHook, sizeof(psEopHook), c_ptr);
            psEopHookStatus = TRUE;
         } else {
            psEopHookStatus = FALSE;
         }
      }
   }
   if (psSetupStatus == INVALID) {
      psSetupStatus = TRUE;
      if (gpszPsSetup == NULL && (!PRTGIF || cmdLineOpenDisplay) &&
            (c_ptr=XGetDefault(mainDisplay, TOOL_NAME, "AdditionalPSSetup")) !=
            NULL) {
         SetPsSetup(c_ptr);
      }
   }
   if (psDistillerNoImageCompress == INVALID) {
      psDistillerNoImageCompress = FALSE;
      if (!PRTGIF || cmdLineOpenDisplay) {
         if ((c_ptr=XGetDefault(mainDisplay, TOOL_NAME,
               "PSDistillerNoImageCompress")) !=
               NULL && UtilStrICmp(c_ptr, "true") == 0) {
            psDistillerNoImageCompress = TRUE;
         }
      }
   }
   if (!init_other_xdefs) {
      init_other_xdefs = TRUE;

      if (PRTGIF) {
         generateTiffEPSI = cmdLineTiffEPSI;
         strcpy(xbmToTiffCmd, defXbmToTiffCmd);
      } else {
         if ((c_ptr=XGetDefault(mainDisplay, TOOL_NAME, "TiffEPSI")) != NULL) {
            if (UtilStrICmp(c_ptr, "true") == 0) {
               fprintf(stderr, TgLoadString(STID_TIFFEPSI_DIRECTLY_SUPPORTED),
                     TOOL_NAME, "TiffEPSI", TOOL_NAME);
               fprintf(stderr, "\n");
            }
         }
         if ((c_ptr=XGetDefault(mainDisplay, TOOL_NAME, "XbmToTiff")) != NULL) {
            int count=0;

            UtilStrCpyN(xbmToTiffCmd, sizeof(xbmToTiffCmd), c_ptr);
            for (c_ptr=strstr(xbmToTiffCmd,"%s"); c_ptr!=NULL;
                  c_ptr=strstr(++c_ptr,"%s")) {
               count++;
            }
            if (count != 1 && count != 2) {
               sprintf(gszMsgBox, TgLoadString(STID_INVALID_XDEF_USE_ALT_STR),
                     TOOL_NAME, "XbmToTiff", xbmToTiffCmd, defXbmToTiffCmd);
               MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
               strcpy(xbmToTiffCmd, defXbmToTiffCmd);
            }
         } else {
            strcpy(xbmToTiffCmd, defXbmToTiffCmd);
         }
      }
      strcpy(pdfExportExtension, "pdf");
      if (!PRTGIF || cmdLineOpenDisplay) {
         if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"PdfFileExtension")) !=
               NULL) {
            UtilStrCpyN(pdfExportExtension, sizeof(pdfExportExtension), c_ptr);
            UtilTrimBlanks(pdfExportExtension);
            if (*pdfExportExtension == '\0') {
               fprintf(stderr, TgLoadString(STID_INVALID_XDEF_USE_ALT_STR),
                     TOOL_NAME, "PdfFileExtension", pdfExportExtension, "pdf");
               fprintf(stderr, "\n");
               strcpy(pdfExportExtension, "pdf");
            }
         }
      }
      strcpy(tmpSvgExportExtension, "sk");
      if (!PRTGIF || cmdLineOpenDisplay) {
         if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"TmpSvgFileExtension")) !=
               NULL) {
            UtilStrCpyN(tmpSvgExportExtension, sizeof(tmpSvgExportExtension),
                  c_ptr);
            UtilTrimBlanks(tmpSvgExportExtension);
            if (*tmpSvgExportExtension == '\0') {
               fprintf(stderr, TgLoadString(STID_INVALID_XDEF_USE_ALT_STR),
                     TOOL_NAME, "TmpSvgFileExtension", tmpSvgExportExtension,
                     "sk");
               fprintf(stderr, "\n");
               strcpy(tmpSvgExportExtension, "sk");
            }
         }
      }
      if (!GetCmdUsingDefAndXDef(psToPdfCmd, sizeof(psToPdfCmd), defPsToPdfCmd,
            "PsToPdf", 2, TRUE)) {
      }
      if (!GetCmdUsingDefAndXDef(epsToTmpSvgCmd, sizeof(epsToTmpSvgCmd),
            defEpsToTmpSvgCmd, "EpsToTmpSvg", 2, TRUE)) {
      }
      if (!GetCmdUsingDefAndXDef(tmpSvgToSvgCmd, sizeof(tmpSvgToSvgCmd),
            defTmpSvgToSvgCmd, "TmpSvgToSvg", 2, TRUE)) {
      }
      strcpy(gszProducedBy, TgLoadCachedString(CSTID_PARANED_UNKNOWN));
      if (!PRTGIF || cmdLineOpenDisplay) {
         if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"ProducedBy")) != NULL) {
            UtilStrCpyN(gszProducedBy, sizeof(gszProducedBy), c_ptr);
         }
      } else if (*cmdLineProducedBy != '\0') {
         UtilStrCpyN(gszProducedBy, sizeof(gszProducedBy), cmdLineProducedBy);
      }
      pdfSetPageDevice = INVALID;
      if (!PRTGIF || cmdLineOpenDisplay) {
         if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME, "PdfSetPageDevice")) !=
               NULL) {
            if (UtilStrICmp(c_ptr, "true") == 0) {
               pdfSetPageDevice = TRUE;
            } else if (UtilStrICmp(c_ptr, "false") == 0) {
               pdfSetPageDevice = FALSE;
            } else {
               fprintf(stderr, TgLoadString(STID_INVALID_XDEF_IGNORED),
                     TOOL_NAME, "PdfSetPageDevice", c_ptr);
               fprintf(stderr, "\n");
            }
         } else if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,
               "LandscapePdfSetPageDevice")) != NULL) {
            if (UtilStrICmp(c_ptr, "true") == 0) {
               fprintf(stderr, TgLoadString(STID_OBSOLETE_XDEF_REPLACED),
                     TOOL_NAME, "LandscapePdfSetPageDevice",
                     TOOL_NAME, "PdfSetPageDevice", "true");
               fprintf(stderr, "\n");
               pdfSetPageDevice = TRUE;
            } else if (UtilStrICmp(c_ptr, "false") == 0) {
               fprintf(stderr, TgLoadString(STID_OBSOLETE_XDEF_REPLACED),
                     TOOL_NAME, "LandscapePdfSetPageDevice",
                     TOOL_NAME, "PdfSetPageDevice", "false");
               fprintf(stderr, "\n");
               pdfSetPageDevice = FALSE;
            } else {
               fprintf(stderr, TgLoadString(STID_INVALID_XDEF_IGNORED),
                     TOOL_NAME, "LandscapePdfSetPageDevice");
               fprintf(stderr, "\n");
            }
         }
      }
   }
   if (overrideEPSIExportExtension == INVALID) {
      *epsiExportExtension = '\0';
      overrideEPSIExportExtension = FALSE;
      if (!PRTGIF) {
         if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"EPSIExportExtension")) !=
               NULL) {
            UtilStrCpyN(epsiExportExtension, sizeof(epsiExportExtension),
                  c_ptr);
            if (strchr(epsiExportExtension, '.') != NULL ||
                  strchr(epsiExportExtension, ' ') != NULL) {
               fprintf(stderr, TgLoadString(STID_INVALID_XDEF),
                     TOOL_NAME, "EPSIExportExtension", epsiExportExtension);
               fprintf(stderr, "\n");
               *epsiExportExtension = '\0';
            }
         }
      }
   }
   if (numberFileInPrintOnePage == INVALID) {
      numberFileInPrintOnePage = FALSE;
      if (PRTGIF) {
         if (cmdLineOneFilePerPage) numberFileInPrintOnePage = TRUE;
      } else {
         if ((c_ptr=XGetDefault(mainDisplay, TOOL_NAME,
               "NumberFileInPrintOnePage")) != NULL &&
               UtilStrICmp(c_ptr, "true") == 0) {
            numberFileInPrintOnePage = TRUE;
         }
      }
   }
   if (dontUseShortHandPS == INVALID) {
      dontUseShortHandPS = FALSE;
      if (!PRTGIF || cmdLineOpenDisplay) {
         if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"DontUseShortHandPS")) !=
               NULL && UtilStrICmp(c_ptr, "true") == 0) {
            dontUseShortHandPS = TRUE;
         }
      }
   }
   if (dontCondense == INVALID) {
      dontCondense = TRUE;
      if (!PRTGIF || cmdLineOpenDisplay) {
         if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"DontCondensePSFile")) !=
               NULL && UtilStrICmp(c_ptr, "false") == 0) {
            dontCondense = FALSE;
         }
      }
   }
   if (stripSimpleComments == INVALID) {
      stripSimpleComments = FALSE;
      if (!PRTGIF || cmdLineOpenDisplay) {
         if ((c_ptr=XGetDefault(mainDisplay, TOOL_NAME,
               "StripCondensedPSComments")) != NULL &&
               UtilStrICmp(c_ptr, "false") != 0) {
            static int shown_once=FALSE;

            if (!shown_once) {
               sprintf(gszMsgBox, TgLoadString(STID_NAMED_XDEF_IS_OBSOLETE),
                     TOOL_NAME, "StripCondensedPSComments");
               MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
               shown_once = TRUE;
            }
         }
      }
   }
   if (psRegMarksInTiledPageMode == INVALID) {
      psRegMarksInTiledPageMode = FALSE;
      psRegMarksGray = 0.95;
      if (!PRTGIF || cmdLineOpenDisplay) {
         if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,
               "PsRegMarksInTiledPageMode")) != NULL &&
               UtilStrICmp(c_ptr, "true") == 0) {
            psRegMarksInTiledPageMode = TRUE;
         }
         if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"PsRegMarksGray")) !=
               NULL) {
            psRegMarksGray = (float)atof(c_ptr);
            if (psRegMarksGray < 0.0 || psRegMarksGray > 1.0) {
               fprintf(stderr, TgLoadString(STID_INVALID_XDEF_USE_ALT_STR),
                     TOOL_NAME, "PsRegMarksGray", c_ptr, "0.95");
               psRegMarksGray = 0.95;
            }
         }
      }
   }
}

static
void GenTiffFile(tmp_fname, epsi_fname, condense, condensed_already)
   char *tmp_fname, *epsi_fname;
   int condense, condensed_already;
{
   register int i;
   char tiff_fname[MAXPATHLENGTH+1], xbm_fname[MAXPATHLENGTH+1];
   char cmd[MAXPATHLENGTH+1], buf[1024], *c_ptr=NULL;
   int bytes_read=0, count=0, tiff_fd=(-1), epsi_fd=(-1);
   FILE *tmp_fp=NULL, *epsi_fp=NULL, *pfp=NULL;
   long tmp_bytes=0L, tiff_bytes=0L;
   struct stat stat_buf;

   strcpy(buf, curFileName);
   GetRealBaseName(buf);
   sprintf(xbm_fname, "%s%c%s", curDir, DIR_SEP, buf);
   i = strlen(xbm_fname);
   sprintf(&xbm_fname[i], ".%s", XBM_FILE_EXT);
   ModifyOutputFileName(xbm_fname);

   sprintf(tiff_fname, "%s%c%s", curDir, DIR_SEP, buf);
   i = strlen(tiff_fname);
   sprintf(&tiff_fname[i], ".tif");
   ModifyOutputFileName(tiff_fname);

   count = 0;
   for (c_ptr=strstr(xbmToTiffCmd,"%s"); c_ptr!=NULL;
         c_ptr=strstr(++c_ptr,"%s")) {
      count++;
   }
   if (count == 1) {
      sprintf(cmd, xbmToTiffCmd, xbm_fname);
   } else {
      sprintf(cmd, xbmToTiffCmd, xbm_fname, tiff_fname);
   }
   unlink(tiff_fname);
   if (!FindProgramInPath(cmd, NULL, FALSE)) {
      return;
   }
   sprintf(gszMsgBox, TgLoadCachedString(CSTID_EXECUTING_GIVEN_PROGRAM), cmd);
   SetStringStatus(gszMsgBox);
   XSync(mainDisplay, False);
   if ((pfp=(FILE*)popen(cmd, "r")) == NULL) {
      sprintf(gszMsgBox, TgLoadString(STID_FAIL_EXEC_PROG_EPSI_NOT_GEN), cmd);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return;
   }
   while (fgets(buf, sizeof(buf), pfp) != NULL) {
      Msg(buf);
   }
   pclose(pfp);
   SetStringStatus(TgLoadCachedString(CSTID_DOTS_DONE));

   if (stat(tmp_fname, &stat_buf) != 0) {
      sprintf(gszMsgBox, TgLoadString(STID_FAIL_GET_FINFO_EPSI_NOT_GEN),
            tmp_fname);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return;
   }
   tmp_bytes = (long)stat_buf.st_size;
   if (stat(tiff_fname, &stat_buf) != 0) {
      sprintf(gszMsgBox, TgLoadString(STID_FAIL_GET_FINFO_EPSI_NOT_GEN),
            tiff_fname);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return;
   }
   tiff_bytes = (long)stat_buf.st_size;

   if ((tmp_fp=fopen(tmp_fname, "r")) == NULL) {
      sprintf(gszMsgBox, TgLoadString(STID_CANNOT_OPEN_FILE_FOR_READING),
            tmp_fname);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return;
   }
   if ((tiff_fd=open(tiff_fname, O_RDONLY)) == (-1)) {
      sprintf(gszMsgBox, TgLoadString(STID_CANNOT_OPEN_FILE_FOR_READING),
            tiff_fname);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      fclose(tmp_fp);
      return;
   }
   if (PRTGIF && cmdLineStdOut) {
      epsi_fp = stdout;
   } else if ((epsi_fp=fopen(epsi_fname, "w")) == NULL) {
      sprintf(gszMsgBox, TgLoadString(STID_CANNOT_OPEN_FILE_FOR_WRITING),
            tiff_fname);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      fclose(tmp_fp);
      close(tiff_fd);
      unlink(tiff_fname);
      return;
   }
   WRITEBYTE(epsi_fp, 0xc5);
   WRITEBYTE(epsi_fp, 0xd0);
   WRITEBYTE(epsi_fp, 0xd3);
   WRITEBYTE(epsi_fp, 0xc6);
   WriteDoubleWord(epsi_fp, 0x0000001e);
   WriteDoubleWord(epsi_fp, tmp_bytes);
   WriteDoubleWord(epsi_fp, 0L);
   WriteDoubleWord(epsi_fp, 0L);
   WriteDoubleWord(epsi_fp, tmp_bytes+0x1e);
   WriteDoubleWord(epsi_fp, tiff_bytes);
   WriteWord(epsi_fp, 0xffff);

   if (!writeFileFailed) {
      CondenseAndCopyPostScriptFile(tmp_fp, epsi_fp, tmp_fname,
            (!condensed_already && condense && !dontCondense));
   }
   fclose(tmp_fp);
   if (!(PRTGIF && cmdLineStdOut)) fclose(epsi_fp);
   if (writeFileFailed) {
      writeFileFailed = FALSE;
      if (PRTGIF && cmdLineStdOut) {
         fprintf(stderr, "%s\n", TgLoadString(STID_FAIL_TO_WRITE_TO_STDOUT));
      } else {
         FailToWriteFileMessage(epsi_fname);
      }
      close(tiff_fd);
      unlink(tiff_fname);
      return;
   }
   if (PRTGIF && cmdLineStdOut) {
      epsi_fd = 1;
   } else if ((epsi_fd=open(epsi_fname, O_WRONLY|O_APPEND, S_IRUSR|S_IWUSR)) ==
         (-1)) {
      sprintf(gszMsgBox, TgLoadString(STID_CANNOT_OPEN_FILE_FOR_APPEND),
            epsi_fname);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      close(tiff_fd);
      unlink(tiff_fname);
      return;
   }
   while ((bytes_read=read(tiff_fd, buf, sizeof(buf))) > 0) {
      if (write(epsi_fd, buf, bytes_read) <= 0) {
         writeFileFailed = TRUE;
         break;
      }
   }
   if (writeFileFailed) {
      writeFileFailed = FALSE;
      FailToWriteFileMessage(epsi_fname);
   }
   if (!(PRTGIF && cmdLineStdOut)) close(epsi_fd);
   close(tiff_fd);
   unlink(tiff_fname);
}

static int psA4PaperSize=FALSE;

static
void InitNeedA4()
{
   static int nInitialized=FALSE;

   if (nInitialized) return;
   nInitialized = TRUE;
   if (cmdLineA4) {
      psA4PaperSize = TRUE;
   } else if (!PRTGIF) {
      char *c_ptr;

      if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"PSA4PaperSize")) != NULL &&
            UtilStrICmp(c_ptr, "true") == 0) {
         psA4PaperSize = TRUE;
      }
   }
}

static
int NeedA4()
{
   int width_to_match, height_to_match;

   InitNeedA4();
   if (!psA4PaperSize) return FALSE;
   width_to_match = (int)(825.0*PIX_PER_INCH/printMag);
   height_to_match = (int)(117.0*PIX_PER_INCH*10.0/printMag);
   if (onePageWidth == width_to_match && onePageHeight == height_to_match) {
      return TRUE;
   }
   width_to_match = (int)(21000.0*PIX_PER_MM/printMag);
   height_to_match = (int)(29700.0*PIX_PER_MM/printMag);
   return (onePageWidth == width_to_match && onePageHeight == height_to_match);
}

void SetOutputFileName(ps_file, file_ext, pn_short_name, ppsz_rest)
   char *ps_file, *file_ext, **ppsz_rest;
   int *pn_short_name;
{
   if (PRTGIF) {
      char *psz_file=(*ppsz_rest);

      if (cmdLineOneFilePerPage) {
         if (curPage != NULL && curPage->page_file_name != NULL &&
               *curPage->page_file_name != '\0') {
            sprintf(ps_file, "%s.%s", curPage->page_file_name, file_ext);
         } else {
            sprintf(ps_file, "%s-%1d.%s", psz_file, cmdLinePageNum, file_ext);
         }
      } else {
         sprintf(ps_file, "%s.%s", psz_file, file_ext);
      }
   } else {
      int len=0, use_page_num=FALSE;
      char *psz=NULL;

      if (dumpOneFilePerPage ||
            (dumpOnePageInStackMode && numberFileInPrintOnePage)) {
         if (curPage != NULL && curPage->page_file_name != NULL &&
               *curPage->page_file_name != '\0') {
            sprintf(ps_file, "%s%c%s.", curDir, DIR_SEP,
                  curPage->page_file_name);
         } else {
            sprintf(ps_file, "%s%c%s", curDir, DIR_SEP, curFileName);
            use_page_num = TRUE;
         }
      } else {
         sprintf(ps_file, "%s%c%s", curDir, DIR_SEP, curFileName);
      }
      len = strlen(ps_file);
      if (len > 3 && strcmp(&ps_file[len-3], ".gz") == 0) {
         ps_file[len-3] = '\0';
         len -= 3;
      }
      if ((psz=UtilStrRChr(ps_file, '.')) == NULL) {
         sprintf(gszMsgBox, TgLoadString(STID_CANT_FIND_DOT_IN_SAVEFILE),
               "SetOutputFileName()");
         TgAssert(FALSE, gszMsgBox, NULL);
      }
      if (use_page_num) {
         sprintf(psz, "-%1d.%s", curPageNum, file_ext);
      } else {
         sprintf(psz, ".%s", file_ext);
      }
   }
   ModifyOutputFileName(ps_file);

   if (!PRTGIF && pn_short_name != NULL && ppsz_rest != NULL) {
      if (((*pn_short_name)=IsPrefix(bootDir, ps_file, ppsz_rest))) {
         *ppsz_rest = (&(*ppsz_rest)[1]);
      }
   }
}

static
void DumpRegMark(fp, pstem)
   FILE *fp;
   float *pstem;
{
   float fval=(*pstem);

   fprintf(dumpFP, "   %s ", gPsCmd[PS_GSAVE]);
   fprintf(dumpFP, "%.3f 0 %s ", -fval, gPsCmd[PS_RMOVETO]);
   fprintf(dumpFP, "2 %.3f %s 0 %s ", fval, gPsCmd[PS_MUL], gPsCmd[PS_RLINETO]);
   fprintf(dumpFP, "%s %s\n", gPsCmd[PS_STROKE], gPsCmd[PS_GRESTORE]);
   fprintf(dumpFP, "   %s ", gPsCmd[PS_GSAVE]);
   fprintf(dumpFP, "0 %.3f %s ", -fval, gPsCmd[PS_RMOVETO]);
   fprintf(dumpFP, "0 2 %.3f %s %s ", fval, gPsCmd[PS_MUL], gPsCmd[PS_RLINETO]);
   fprintf(dumpFP, "%s %s\n", gPsCmd[PS_STROKE], gPsCmd[PS_GRESTORE]);
}

static
int FixUpSVGLine(out_fp, dup_buf)
   FILE *out_fp;
   char *dup_buf;
{
   char *psz=NULL, *psz_start=&dup_buf[5];
   int found_xmlns=FALSE;

   while (psz_start != NULL) {
      char saved_ch='\0', key[MAXSTRING];

      /* skip all the blank characters */
      while (*psz_start == ' ' || *psz_start == '\t') psz_start++;

      *key = '\0';
      for (psz=psz_start; *psz != '>'; psz++) {
         /* looking for key */
         saved_ch = '\0';
         if (*psz == ' ' || *psz == '\t' || *psz ==  '=' || *psz == '>') {
            saved_ch = *psz;
            *psz = '\0';
            UtilStrCpyN(key, sizeof(key), psz_start);
            *psz = saved_ch;
            break;
         }
      }
      if (*key == '\0') {
         /* no key, this should not happen, may be malformed, give up */
         break;
      } else if (saved_ch != '=') {
         /* saved_ch is either '\0' or a blank character, key with no value */
         psz_start = ++psz;
      } else {
         int find_double_quote=FALSE;
         char value[MAXSTRING];

         *value = '\0';
         /* looking for value */
         psz_start = ++psz;
         if (*psz_start == '"') {
            find_double_quote = TRUE;
            psz_start++;
         }
         if (find_double_quote) {
            for (psz=psz_start; *psz != '"' && *psz != '>'; psz++) ;

            if (*psz == '>') {
               /* may be malformed, give up */
               break;
            } else if (*psz == '"') {
               saved_ch = *psz;
               *psz = '\0';
               UtilStrCpyN(value, sizeof(value), psz_start);
               *psz = saved_ch;
            }
         } else {
            for (psz=psz_start; *psz != '>'; psz++) {
               saved_ch = '\0';
               if (*psz == ' ' || *psz == '\t' || *psz == '>') {
                  saved_ch = *psz;
                  *psz = '\0';
                  UtilStrCpyN(value, sizeof(value), psz_start);
                  *psz = saved_ch;
                  break;
               }
            }
            if (*key == '\0') {
               /* no value, this should not happen, may be malformed, give up */
               break;
            }
         }
         /* found both key and value */
         if (UtilStrICmp(key, "xmlns") == 0) {
            found_xmlns = TRUE;
            break;
         }
         psz_start = ++psz;
      }
   }
   if (found_xmlns) {
      fprintf(out_fp, "%s\n", dup_buf);
   } else {
      int len=strlen(dup_buf);

      dup_buf[len-1] = '\0';
      fprintf(out_fp, "%s xmlns=\"http://www.w3.org/2000/svg\">\n", dup_buf);
      dup_buf[len-1] = '>';
   }
   return TRUE;
}

static
int FixUpSVGFile(tmpsvg_fname, svg_fname)
   char *tmpsvg_fname, *svg_fname;
{
   FILE *in_fp=fopen(tmpsvg_fname, "r"), *out_fp=NULL;
   char *buf=NULL;
   int line_num=0, fixed=FALSE;

   if (in_fp == NULL) {
      return FailToOpenMessage(tmpsvg_fname, "r", NULL);
   }
   out_fp = fopen(svg_fname, "w");
   if (out_fp == NULL) {
      return FailToOpenMessage(tmpsvg_fname, "w", NULL);
   }
   while ((buf=UtilGetALine(in_fp)) != NULL) {
      line_num++;
      if (!fixed && buf[0] == '<') {
         if (UtilStrNCaseCmp(buf, "<svg ", 5) == 0) {
            int len=0;

            UtilTrimBlanks(buf);
            len = strlen(buf);
            if (len > 0 && buf[len-1] == '>') {
               char *dup_buf=UtilStrDup(buf);

               if (dup_buf == NULL) {
                  FailAllocMessage();
                  UtilFree(buf);
                  fclose(in_fp);
                  fclose(out_fp);
                  return FALSE;
               }
               if (FixUpSVGLine(out_fp, dup_buf)) {
                  fixed = TRUE;
               } else {
                  fprintf(out_fp, "%s\n", buf);
               }
               UtilFree(dup_buf);
            } else {
               fprintf(out_fp, "%s\n", buf);
            }
         } else {
            fprintf(out_fp, "%s\n", buf);
         }
      } else {
         fprintf(out_fp, "%s\n", buf);
      }
      UtilFree(buf);
   }
   fclose(in_fp);
   fclose(out_fp);

   return TRUE;
}

static
int DoGenDump(FileName)
   char *FileName;
{
   static char loc_time[MAXSTRING+1];

   struct ObjRec *obj_ptr=NULL;
   char cmd[(MAXSTRING<<1)+1], tmp_str[MAXSTRING+1], ps_file[MAXSTRING+1];
   char *rest=NULL, *file_ext=NULL, *output_type=NULL;
   int i, len, short_name=FALSE, export_cap=0;
   time_t tloc;

   if (!curDirIsLocal) {
      MsgBox(TgLoadString(STID_CANNOT_PRINT_REMOTE_FILE), TOOL_NAME, INFO_MB);
      return FALSE;
   }
   GenDumpInitDefaults();

   if (botObj==NULL && ((pageLayoutMode==PAGE_STACK &&
         dumpOnePageInStackMode) || pageLayoutMode==PAGE_TILE)) {
      int rc=FALSE;

      if (PRTGIF && cmdLineOneFilePerPage) {
         sprintf(gszMsgBox, TgLoadString(STID_NO_OBJ_TO_PRINT_ON_PAGE),
               cmdLinePageNum);
         rc = TRUE;
      } else if (!PRTGIF && dumpOneFilePerPage) {
         sprintf(gszMsgBox, TgLoadString(STID_NO_OBJ_TO_PRINT_ON_PAGE),
               curPageNum);
         rc = TRUE;
      }
      if (preDumpSetup && rc) {
         return rc;
      }
      if (rc) {
         if (PRTGIF) {
            fprintf(stderr, "%s\n", gszMsgBox);
         } else {
            Msg(gszMsgBox);
         }
      } else {
         strcpy(gszMsgBox, TgLoadString(STID_NO_OBJ_TO_PRINT));
         if (PRTGIF) {
            fprintf(stderr, "%s\n", gszMsgBox);
         } else {
            MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
         }
      }
      return rc;
   }
   if (whereToPrint != PRINTER) {
      if (!PRTGIF && !curFileDefined) {
         sprintf(gszMsgBox, TgLoadString(STID_NO_CUR_FILE_CANNOT_GEN_FORMAT),
               GetExportName(whereToPrint, EXPORT_THIS));
         if (PRTGIF) {
            fprintf(stderr, "%s\n", gszMsgBox);
         } else {
            MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
         }
         return FALSE;
      }
   }
   if (!PRTGIF && printingPageNum==printingFirstPageNum && !preDumpSetup) {
      Msg(TgLoadString(STID_GENERATING_PRINT_FILE_DOTS));
   }
   if (whereToPrint >= MAXDEFWHERETOPRINT) {
      export_cap = GetExportFilterCapability(whereToPrint);
   }
   if (whereToPrint == XBM_FILE || whereToPrint == GIF_FILE ||
         whereToPrint == HTML_FILE || whereToPrint == PNG_FILE ||
         whereToPrint == JPEG_FILE || whereToPrint == PPM_FILE) {
      if (topObj == NULL) {
         sprintf(gszMsgBox, TgLoadString(STID_NO_OBJ_TO_EXPORT));
         if (PRTGIF) {
            fprintf(stderr, "%s\n", gszMsgBox);
         } else {
            MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
         }
      } else {
         int saved_colordump=colorDump, watch_cursor=watchCursorOnMainWindow;
         int dump_ppm_preferred=FALSE;

         if (!watch_cursor) {
            SetWatchCursor(drawWindow);
            SetWatchCursor(mainWindow);
         }
         if (whereToPrint == GIF_FILE || whereToPrint == HTML_FILE ||
               whereToPrint == PNG_FILE || whereToPrint == JPEG_FILE) {
            colorDump = TRUE;
         }
         dump_ppm_preferred = colorDump && fullTrueColorMode &&
               (whereToPrint==GIF_FILE || whereToPrint==HTML_FILE ||
               whereToPrint==PNG_FILE || whereToPrint==JPEG_FILE ||
               whereToPrint==PPM_FILE || whereToPrint==XBM_FILE);

         DumpXBitmapFile(FALSE, dump_ppm_preferred, dumpOneFilePerPage ||
               (dumpOnePageInStackMode && numberFileInPrintOnePage));
         colorDump = saved_colordump;
         if (!watch_cursor) {
            SetDefaultCursor(mainWindow);
            ShowCursor(); 
         }
      }
      return TRUE;
   }
   /*
    * At this point, whereToPrint can only be PostScript, ASCII Text, NetList,
    *      or external export type (if whereToPrint >= MAXDEFWHERETOPRINT).
    */
   if (printingPageNum == printingFirstPageNum) {
      if (MkTempFile(tmpFile, sizeof(tmpFile), tmpDir, TOOL_NAME) == NULL) {
         return FALSE;
      }
      if ((dumpFP=fopen(tmpFile, "w")) == NULL) {
         sprintf(tmp_str, TgLoadString(STID_CANNOT_OPEN_FOR_WRITE_PRINT),
               tmpFile);
         if (PRTGIF) {
            fprintf(stderr, "%s\n", tmp_str);
         } else {
            MsgBox(tmp_str, TOOL_NAME, INFO_MB);
         }
         return FALSE;
      }
      if (PRTGIF && !cmdLineStdOut) {
         if (!cmdLineQuiet) {
            fprintf(stderr, TgLoadString(STID_WRITING_TO_DOTS), tmpFile);
            fprintf(stderr, "\n");
         }
      }
      writeFileFailed = FALSE;
      ResetGrayDetection();
   }
   if (whereToPrint < MAXDEFWHERETOPRINT &&
         printingPageNum == printingFirstPageNum &&
         whereToPrint != TEXT_FILE && whereToPrint != NETLIST_FILE) {
      PSUseShortHand();
      if (dontUseShortHandPS) {
         PSDontUseShortHand();
      }
      if (usePsAdobeString) {
         switch (whereToPrint) {
         case PRINTER:
         case PS_FILE:
         case PDF_FILE:
            if (*adobeString == '\0') {
               if (fprintf(dumpFP, "%%!PS-Adobe-2.0\n") == EOF) {
                  writeFileFailed = TRUE;
               }
            } else if (fprintf(dumpFP,"%%!PS-Adobe-%s\n",adobeString) == EOF) {
               writeFileFailed = TRUE;
            }
            break;
         case LATEX_FIG:
         case EPSI_FILE:
         case SVG_FILE:
            if (*adobeString == '\0') {
               if (fprintf(dumpFP, "%%!PS-Adobe-2.0 EPSF-1.2\n") == EOF) {
                  writeFileFailed = TRUE;
               }
            } else if (*epsfString == '\0') {
               if (fprintf(dumpFP, "%%!PS-Adobe-%s\n", adobeString) == EOF) {
                  writeFileFailed = TRUE;
               }
            } else {
               if (fprintf(dumpFP, "%%!PS-Adobe-%s EPSF-%s\n",
                     adobeString, epsfString) == EOF) {
                  writeFileFailed = TRUE;
               }
            }
            break;
         }
      } else {
         if (preDumpSetup) PSUsePSAdobe();
         if (fprintf(dumpFP, "%%!\n") == EOF) {
            writeFileFailed = TRUE;
         }
      }
      if (pageLayoutMode == PAGE_STACK &&
            printingFirstPageNum == printingLastPageNum) {
         DumpBBox(dumpFP, ALL_BBOX, NULL);
      } else if (fprintf(dumpFP,"%%%%BoundingBox: (atend)\n") == EOF) {
         writeFileFailed = TRUE;
      }
      if (PRTGIF && !cmdLineOpenDisplay) {
         if (fprintf(dumpFP, "%%%%Title: %s\n", FileName) == EOF) {
            writeFileFailed = TRUE;
         }
      } else if (curFileDefined) {
         int gzipped=FALSE;

         strcpy(tmp_str, curFileName);
         if (FileNameHasExtension(tmp_str, OBJ_FILE_TYPE, &gzipped, NULL) &&
               gzipped) {
            char *psz=UtilStrRChr(tmp_str, '.');

            if (psz != NULL) *psz = '\0';
         }
         len = strlen(tmp_str);
         for (i=len-1; tmp_str[i] != '.'; i--) ;
         tmp_str[i] = '\0';
         if (fprintf(dumpFP, "%%%%Title: %s\n", tmp_str) == EOF) {
            writeFileFailed = TRUE;
         }
      } else if (fprintf(dumpFP, "%%%%Title: [Unnamed]\n") == EOF) {
         writeFileFailed = TRUE;
      }
      time(&tloc);
      strcpy(loc_time, ctime(&tloc));
      loc_time[24] = '\0';
      if (fprintf(dumpFP, "%%%%CreationDate: %s\n", loc_time) == EOF) {
         writeFileFailed = TRUE;
      }
      SetFullVersionString();
      if (fprintf(dumpFP, "%%%%Creator: %s written by %s\n", fullToolName,
            "William Chia-Wei Cheng (bill.cheng@acm.org)") == EOF) {
         writeFileFailed = TRUE;
      }
      if (fprintf(dumpFP, "%%%%ProducedBy: %s\n", gszProducedBy) == EOF) {
         writeFileFailed = TRUE;
      }
      if (whereToPrint != LATEX_FIG && whereToPrint != EPSI_FILE &&
            whereToPrint != SVG_FILE) {
         if (!(pdfSetPageDevice && noOrientationIfPdfSetPageDevice)) {
            if (fprintf(dumpFP, "%%%%Orientation: %s\n",
                  (pageStyle==PORTRAIT ? "Portrait" : "Landscape")) == EOF) {
               writeFileFailed = TRUE;
            }
         }
      }
      if ((whereToPrint != LATEX_FIG && whereToPrint != EPSI_FILE &&
            whereToPrint != SVG_FILE) || !minimalEPS) {
         int total_pages=0;

         if (dumpOnePageInStackMode || dumpOnePageInTileMode ||
               whereToPrint == LATEX_FIG || whereToPrint == EPSI_FILE ||
               whereToPrint == SVG_FILE) {
            total_pages = 1;
         } else if (pageLayoutMode == PAGE_STACK) {
            total_pages = (dumpPages ? gPagesToPrintSpec.num_pages_specified :
                  lastPageNum);
         } else {
            total_pages = paperCol * paperRow;
         }
         if (fprintf(dumpFP, "%%%%Pages: %1d\n", total_pages) == EOF) {
            writeFileFailed = TRUE;
         }
         if (fprintf(dumpFP, "%%%%DocumentFonts: (atend)\n") == EOF) {
            writeFileFailed = TRUE;
         }
         if (NeedA4()) {
            if (fprintf(dumpFP, "%%%%DocumentPaperSizes: a4\n") == EOF) {
               writeFileFailed = TRUE;
            }
         }
         if (fprintf(dumpFP, "%%%%EndComments\n") == EOF) {
            writeFileFailed = TRUE;
         }
      }
      if (whereToPrint == EPSI_FILE && !generateTiffEPSI) {
         SaveStatusStrings();
         SetStringStatus(TgLoadString(STID_GENERATING_PREVIEW_BITMAP));
         GenPreviewBitmap(dumpFP, llxPage, llyPage, urxPage, uryPage);
         RestoreStatusStrings();
      }

      if ((whereToPrint != LATEX_FIG && whereToPrint != EPSI_FILE &&
            whereToPrint != SVG_FILE) || !minimalEPS) {
         if (fprintf(dumpFP, "%%%%BeginProlog\n") == EOF) {
            writeFileFailed = TRUE;
         }
      }

      DumpPSMacro(dumpFP);

      PrepareEightBitFontInfo();
      DumpEightBitFontInfo(dumpFP);

      if (fprintf(dumpFP, "end\n\n") == EOF) writeFileFailed = TRUE;
      if ((whereToPrint != LATEX_FIG && whereToPrint != EPSI_FILE &&
            whereToPrint != SVG_FILE) || !minimalEPS) {
         int need_a4=NeedA4(), need_page_size=FALSE, need_setup=FALSE;

#ifndef _NO_LANDSCAPE_PDF_SETPAGEDEVICE
         if (whereToPrint == PDF_FILE) {
            need_page_size = TRUE;
         }
#endif /* ~_NO_LANDSCAPE_PDF_SETPAGEDEVICE */
         if (whereToPrint != LATEX_FIG && whereToPrint != EPSI_FILE &&
               whereToPrint != SVG_FILE) {
            if (pdfSetPageDevice == INVALID) {
               if (cmdLinePdfSetPageDevice == INVALID) {
                  /* do nothing */
               } else if (cmdLinePdfSetPageDevice) {
                  need_page_size = TRUE;
               } else {
                  need_page_size = FALSE;
               }
            } else if (pdfSetPageDevice) {
               if (cmdLinePdfSetPageDevice == FALSE) {
                  need_page_size = FALSE;
               } else {
                  need_page_size = TRUE;
               }
            } else {
               if (cmdLinePdfSetPageDevice == TRUE) {
                  need_page_size = TRUE;
               } else {
                  need_page_size = FALSE;
               }
            }
         }
         if (fprintf(dumpFP, "%%%%EndProlog\n") == EOF) writeFileFailed = TRUE;
         need_setup = (psDistillerNoImageCompress == TRUE ||
               gpszPsSetup != NULL || need_a4 || need_page_size);
         if (need_setup) {
            if (fprintf(dumpFP, "%%%%BeginSetup\n") == EOF) {
               writeFileFailed = TRUE;
            }
         }
         if (need_a4) {
            if (fprintf(dumpFP, "%%%%PaperSize: a4\n") == EOF ||
                  fprintf(dumpFP, "%%%%BeginPaperSize: a4\n") == EOF ||
                  fprintf(dumpFP, "%%%%EndPaperSize\n") == EOF) {
               writeFileFailed = TRUE;
            }
         }
         if (need_page_size) {
            double scale=((double)printMag)*((double)72.0)/((double)100.0);
            double w=((double)onePageWidth)/((double)PIX_PER_INCH)*scale;
            double h=((double)onePageHeight)/((double)PIX_PER_INCH)*scale;

            if (pageStyle == LANDSCAPE) {
               fprintf(dumpFP, "<< /PageSize [%1d %1d] ", round(h), round(w));
               fprintf(dumpFP,
                     "/ImagingBBox null /Orientation 3 >> setpagedevice\n");
            } else {
               fprintf(dumpFP, "<< /PageSize [%1d %1d] ", round(w), round(h));
               fprintf(dumpFP,
                     "/ImagingBBox null /Orientation 0 >> setpagedevice\n");
            }
         }
         if (psDistillerNoImageCompress == TRUE) {
            fprintf(dumpFP, "%s\n", "systemdict /setdistillerparams known");
            fprintf(dumpFP, " { << %s %s\n",
                  "/AutoFilterGrayImages false",
                  "/AutoFilterColorImages false");
            fprintf(dumpFP, "   %s %s\n",
                  "/ColorImageFilter /FlateEncode",
                  "/GrayImageFilter /FlateEncode >>");
            fprintf(dumpFP, "   %s\n", "setdistillerparams");
            fprintf(dumpFP, " %s\n", "} if");
         }
         if (gpszPsSetup != NULL) {
            if (fprintf(dumpFP, "%s\n", gpszPsSetup) == EOF) {
               writeFileFailed = TRUE;
            }
         }
         if (need_setup) {
            if (fprintf(dumpFP, "%%%%EndSetup\n") == EOF) {
               writeFileFailed = TRUE;
            }
         }
      }
   }
   if (whereToPrint == NETLIST_FILE && !preDumpSetup &&
         printingPageNum == printingFirstPageNum) {
      if (fprintf(dumpFP, "PORTNAME\tSIGNALNAME\n") == EOF) {
         writeFileFailed = TRUE;
      }
   }
   if (pageLayoutMode == PAGE_STACK || (pageLayoutMode == PAGE_TILE &&
         ((paperCol == 1 && paperRow == 1) ||
         ((whereToPrint == LATEX_FIG || whereToPrint == EPSI_FILE ||
         whereToPrint == SVG_FILE) &&
         !dumpOnePageInTileMode)))) {
      printingPageRow = printingPageCol = 1;
      if (whereToPrint >= MAXDEFWHERETOPRINT) {
          if (!ExportVectorFile(dumpFP, dumpOneFilePerPage ||
                (dumpOnePageInStackMode && numberFileInPrintOnePage),
                printingPageNum, printingFirstPageNum, printingPageRow,
                printingPageCol)) {
             return FALSE;
          }
      } else if (whereToPrint == TEXT_FILE) {
         if (printingPageNum != printingFirstPageNum) {
            fprintf(dumpFP, "\014\n");
         }
         for (obj_ptr = botObj; obj_ptr != NULL; obj_ptr = obj_ptr->prev) {
            obj_ptr->tmp_parent = NULL;
            DumpAnObjInAscii(dumpFP, obj_ptr);
         }
      } else if (whereToPrint == NETLIST_FILE) {
         for (obj_ptr=botObj; obj_ptr != NULL; obj_ptr=obj_ptr->prev) {
            obj_ptr->tmp_parent = NULL;
            DumpNetListForAnObj(dumpFP, obj_ptr);
         }
      } else {
         if ((whereToPrint != LATEX_FIG && whereToPrint != EPSI_FILE &&
               whereToPrint != SVG_FILE) || !minimalEPS) {
            if (dumpPages) {
               fprintf(dumpFP, "%%%%Page: %1d %1d\n\n",
                     printingPageNum, dumpPageNum);
            } else {
               fprintf(dumpFP, "%%%%Page: %1d %1d\n\n",
                     printingPageNum-printingFirstPageNum+1,
                     printingPageNum-printingFirstPageNum+1);
            }
         }
         if (!minimalEPS) {
            DumpBBox(dumpFP, PAGE_BBOX, NULL);
         }
         if (totalBBoxValid) {
            if (topObj != NULL) {
               if (llxPage < llxTotal) llxTotal = llxPage;
               if (llyPage < llyTotal) llyTotal = llyPage;
               if (urxPage > urxTotal) urxTotal = urxPage;
               if (uryPage > uryTotal) uryTotal = uryPage;
            }
         } else if (topObj != NULL) {
            totalBBoxValid = TRUE;
            llxTotal = llxPage; llyTotal = llyPage;
            urxTotal = urxPage; uryTotal = uryPage;
         }
         if (psBopHookStatus == TRUE && (whereToPrint == PRINTER ||
               whereToPrint == PS_FILE || whereToPrint == PDF_FILE)) {
            fprintf(dumpFP, "userdict /%s known { %s } if\n\n",
                  psBopHook, psBopHook);
         }
         fprintf(dumpFP, "tgifdict begin\n");
         fprintf(dumpFP, "/tgifsavedpage save def\n\n");

         fprintf(dumpFP, "1 %s\n", gPsCmd[PS_SETMITERLIMIT]);
         fprintf(dumpFP, "1 %s\n\n", gPsCmd[PS_SETLINEWIDTH]);
         fprintf(dumpFP, "0 %s\n\n", gPsCmd[PS_SETGRAY]);

         if (pageStyle == LANDSCAPE) {
            fprintf(dumpFP, "90 %s\n", gPsCmd[PS_ROTATE]);
         }
         fprintf(dumpFP, "%1d %s %s %1d %s %s %s\n", psDotsPerInch,
               psXOffStr[pageStyle], gPsCmd[PS_MUL], psDotsPerInch,
               psYOffStr[pageStyle], gPsCmd[PS_MUL], gPsCmd[PS_TRANSLATE]);

#ifndef METRIC_PIX_PER_INCH
         fprintf(dumpFP, "%1d %1d %s %.3f %s 100 %s %s %s %s\n\n",
               psDotsPerInch, PIX_PER_INCH, gPsCmd[PS_DIV], printMag,
               gPsCmd[PS_MUL], gPsCmd[PS_DIV], gPsCmd[PS_DUP],
               gPsCmd[PS_NEG], gPsCmd[PS_SCALE]);
#else /* METRIC_PIX_PER_INCH */
         fprintf(dumpFP, "%1d %1d %s %.3f %s 100 %s %s %s %s\n\n",
               psDotsPerInch, (gridSystem==ENGLISH_GRID ? PIX_PER_INCH :
               METRIC_PIX_PER_INCH), gPsCmd[PS_DIV], printMag,
               gPsCmd[PS_MUL], gPsCmd[PS_DIV], gPsCmd[PS_DUP],
               gPsCmd[PS_NEG], gPsCmd[PS_SCALE]);
#endif /* ~METRIC_PIX_PER_INCH */

         fprintf(dumpFP, "%s\n\n", gPsCmd[PS_GSAVE]);

         if (needsTgifOrigCTM) {
            fprintf(dumpFP, "/tgiforigctm matrix currentmatrix def\n\n");
         }
         if (colorDump && colorBgInPrintingColorPS &&
               (whereToPrint == PS_FILE || whereToPrint == PDF_FILE ||
               whereToPrint == PRINTER)) {
            fprintf(dumpFP, "%% Background\n");
            if (myBgColor.red==0 && myBgColor.green==0 && myBgColor.blue==0) {
               fprintf(dumpFP, "0 %s\n", gPsCmd[PS_SETGRAY]);
            } else if (myBgColor.red==maxRGB && myBgColor.green==maxRGB &&
                  myBgColor.blue==maxRGB) {
               fprintf(dumpFP, "1 %s\n", gPsCmd[PS_SETGRAY]);
            } else {
               fprintf(dumpFP, "%.3f %.3f %.3f %s\n",
                     ((float)myBgColor.red/maxRGB),
                     ((float)myBgColor.green/maxRGB),
                     ((float)myBgColor.blue/maxRGB), gPsCmd[PS_SETRGBCOLOR]);
            }
            fprintf(dumpFP, "%s\n", gPsCmd[PS_NEWPATH]);
            fprintf(dumpFP,
                  "   0 0 %s 0 %d %s %d %d %s %d 0 %s\n",
                  gPsCmd[PS_MOVETO], onePageHeight, gPsCmd[PS_LINETO],
                  onePageWidth, onePageHeight, gPsCmd[PS_LINETO],
                  onePageWidth, gPsCmd[PS_LINETO]);
            fprintf(dumpFP, "%s %s\n\n", gPsCmd[PS_CLOSEPATH], gPsCmd[PS_FILL]);
         }

         for (obj_ptr = botObj; obj_ptr != NULL; obj_ptr = obj_ptr->prev) {
            obj_ptr->tmp_parent = NULL;
            DumpAnObj(dumpFP, obj_ptr);
         }
         fprintf(dumpFP, "%s\n", gPsCmd[PS_GRESTORE]);
         fprintf(dumpFP, "tgifsavedpage restore\n");
         fprintf(dumpFP, "end\n");

         switch (whereToPrint) {
         case PRINTER:
         case PS_FILE:
         case PDF_FILE:
            if (psEopHookStatus == TRUE) {
               fprintf(dumpFP, "userdict /%s known { %s } if\n",
                     psEopHook, psEopHook);
            }
            fprintf(dumpFP, "showpage\n\n");
            break;
         case EPSI_FILE:
         case LATEX_FIG:
         case SVG_FILE:
            if (showPageInEPS) {
               fprintf(dumpFP, "showpage\n");
            }
            fprintf(dumpFP, "\n");
            break;
         }
      }
   } else if (whereToPrint >= MAXDEFWHERETOPRINT) {
       if (!ExportVectorFile(dumpFP, dumpOneFilePerPage ||
             (dumpOnePageInStackMode && numberFileInPrintOnePage),
             printingPageNum, printingFirstPageNum, printingPageRow,
             printingPageCol)) {
          return FALSE;
       }
   } else if (whereToPrint == TEXT_FILE) {
      for (obj_ptr = botObj; obj_ptr != NULL; obj_ptr = obj_ptr->prev) {
         obj_ptr->tmp_parent = NULL;
         DumpAnObjInAscii(dumpFP, obj_ptr);
      }
   } else if (whereToPrint == NETLIST_FILE) {
      for (obj_ptr=botObj; obj_ptr != NULL; obj_ptr=obj_ptr->prev) {
         obj_ptr->tmp_parent = NULL;
         DumpNetListForAnObj(dumpFP, obj_ptr);
      }
   } else {
      int row, col;
      float one=(float)atof("1.0"), f_rbx, f_rby, x_dist, y_dist;

      if (tiledPageScaling == one && !msgAboutTiledPageScalingSeen) {
         char msg1[MAXSTRING+1];

         msgAboutTiledPageScalingSeen = TRUE;
         sprintf(gszMsgBox, TgLoadString(STID_OUTPUT_TRUNC_IN_PRINT_TILED),
               TOOL_NAME, "TiledPageScaling");
         sprintf(msg1, "    %s", TgLoadString(STID_RECOMMEND_REDUCING_SETTING));
         if (PRTGIF) {
            fprintf(stderr, "%s\n", gszMsgBox);
            fprintf(stderr, "%s\n", msg1);
         } else {
            TwoLineMsg(gszMsgBox, msg1);
         }
      }
      f_rbx = (float)(((float)PIX_PER_INCH) * 100.0 / printMag *
            psPageWidthInInch[pageStyle]);
      f_rby = (float)(((float)PIX_PER_INCH) * 100.0 / printMag *
            psPageHeightInInch[pageStyle]);
      x_dist = (float)(((float)PIX_PER_INCH) * psPageWidthInInch[pageStyle] *
            (1.0-tiledPageScaling) * 50.0 / printMag);
      y_dist = (float)(((float)PIX_PER_INCH) * psPageHeightInInch[pageStyle] *
            (1.0-tiledPageScaling) * 50.0 / printMag);

      for (row = 0; row < paperRow; row++) {
         printingPageRow = row+1;
         for (col = 0; col < paperCol; col++) {
            struct BBRec page_bbox;
            float real_ltx, real_lty;

            if (dumpOnePageInTileMode &&
                  row*paperCol+col+1 != printingPageNum) {
               continue;
            }
            printingPageCol = col+1;
            page_bbox.ltx = col * onePageWidth;
            page_bbox.lty = row * onePageHeight;
            page_bbox.rbx = page_bbox.ltx + onePageWidth;
            page_bbox.rby = page_bbox.lty + onePageHeight;

            if ((whereToPrint != LATEX_FIG && whereToPrint != EPSI_FILE &&
                  whereToPrint != SVG_FILE) || !minimalEPS) {
               if (dumpOnePageInTileMode) {
                  fprintf(dumpFP, "%%%%Page: 1 1\n\n");
               } else {
                  fprintf(dumpFP, "%%%%Page: %1d %1d\n\n",
                        col+1+row*paperCol, col+1+row*paperCol);
               }
            }
            if (DumpBBox(dumpFP, PAGE_BBOX, &page_bbox)) {
               if (totalBBoxValid) {
                  if (topObj != NULL) {
                     if (llxPage < llxTotal) llxTotal = llxPage;
                     if (llyPage < llyTotal) llyTotal = llyPage;
                     if (urxPage > urxTotal) urxTotal = urxPage;
                     if (uryPage > uryTotal) uryTotal = uryPage;
                  }
               } else if (topObj != NULL) {
                  totalBBoxValid = TRUE;
                  llxTotal = llxPage; llyTotal = llyPage;
                  urxTotal = urxPage; uryTotal = uryPage;
               }
            }
            if (psBopHookStatus == TRUE && (whereToPrint == PRINTER ||
                  whereToPrint == PS_FILE || whereToPrint == PDF_FILE)) {
               fprintf(dumpFP, "userdict /%s known { %s } if\n\n",
                     psBopHook, psBopHook);
            }
            fprintf(dumpFP, "tgifdict begin\n");
            fprintf(dumpFP, "/tgifsavedpage save def\n\n");
            fprintf(dumpFP, "1 %s\n", gPsCmd[PS_SETMITERLIMIT]);
            fprintf(dumpFP, "1 %s\n\n", gPsCmd[PS_SETLINEWIDTH]);
            fprintf(dumpFP, "0 %s\n\n", gPsCmd[PS_SETGRAY]);

            if (pageStyle == LANDSCAPE) {
               fprintf(dumpFP, "90 %s\n", gPsCmd[PS_ROTATE]);
            }
            fprintf(dumpFP, "%1d %s %s %1d %s %s %s\n",
                  psDotsPerInch, psXOffStr[pageStyle], gPsCmd[PS_MUL],
                  psDotsPerInch, psYOffStr[pageStyle], gPsCmd[PS_MUL],
                  gPsCmd[PS_TRANSLATE]);

#ifndef METRIC_PIX_PER_INCH
            fprintf(dumpFP, "%1d %1d %s %.3f %s 100 %s %s %s %s\n\n",
                  psDotsPerInch, PIX_PER_INCH, gPsCmd[PS_DIV], printMag,
                  gPsCmd[PS_MUL], gPsCmd[PS_DIV], gPsCmd[PS_DUP],
                  gPsCmd[PS_NEG], gPsCmd[PS_SCALE]);
#else /* METRIC_PIX_PER_INCH */
            fprintf(dumpFP, "%1d %1d %s %.3f %s 100 %s %s %s %s\n\n",
                  psDotsPerInch, (gridSystem==ENGLISH_GRID ? PIX_PER_INCH :
                  METRIC_PIX_PER_INCH), gPsCmd[PS_DIV], printMag,
                  gPsCmd[PS_MUL], gPsCmd[PS_DIV], gPsCmd[PS_DUP],
                  gPsCmd[PS_NEG], gPsCmd[PS_SCALE]);
#endif /* ~METRIC_PIX_PER_INCH */

            fprintf(dumpFP, "%s\n\n", gPsCmd[PS_GSAVE]);

            fprintf(dumpFP, "%% SETUP CLIP FOR PAGE\n\n");
            fprintf(dumpFP, "%.3f %.3f %s\n", x_dist, y_dist,
                  gPsCmd[PS_TRANSLATE]);
            fprintf(dumpFP, "%.3f %.3f %s\n", tiledPageScaling,
                  tiledPageScaling, gPsCmd[PS_SCALE]);

            fprintf(dumpFP, "-%1d -%1d %s\n", page_bbox.ltx,
                  page_bbox.lty, gPsCmd[PS_TRANSLATE]);
            real_ltx = (float)(((float)col) *
                  ((float)psPageWidthInInch[pageStyle]) *
                  ((float)PIX_PER_INCH) * 100.0 / printMag);
            real_lty = (float)(((float)row) *
                  ((float)psPageHeightInInch[pageStyle]) *
                  ((float)PIX_PER_INCH) * 100.0 / printMag);
            if (psRegMarksInTiledPageMode && tiledPageScaling < (float)1.0) {
               float stem=((float)(min(x_dist,y_dist)))*((float)0.5);

               fprintf(dumpFP, "%s\n", gPsCmd[PS_GSAVE]);
               fprintf(dumpFP, "   0.1 %s %.3f %s\n", gPsCmd[PS_SETLINEWIDTH],
                     psRegMarksGray, gPsCmd[PS_SETGRAY]);

               fprintf(dumpFP, "   %s %.3f %.3f %s\n", gPsCmd[PS_NEWPATH],
                     real_ltx, real_lty, gPsCmd[PS_MOVETO]);
               DumpRegMark(dumpFP, &stem);
               fprintf(dumpFP, "   %.3f 0 %s\n", f_rbx, gPsCmd[PS_RMOVETO]);
               DumpRegMark(dumpFP, &stem);
               fprintf(dumpFP, "   0 %.3f %s\n", f_rby, gPsCmd[PS_RMOVETO]);
               DumpRegMark(dumpFP, &stem);
               fprintf(dumpFP, "   -%.3f 0 %s\n", f_rbx, gPsCmd[PS_RMOVETO]);
               DumpRegMark(dumpFP, &stem);

               fprintf(dumpFP, "%s\n", gPsCmd[PS_GRESTORE]);
            }
            fprintf(dumpFP, "%s\n", gPsCmd[PS_NEWPATH]);
            fprintf(dumpFP, "   %.3f %.3f %s\n", real_ltx, real_lty,
                  gPsCmd[PS_MOVETO]);
            fprintf(dumpFP, "   %.3f 0 %s\n", f_rbx, gPsCmd[PS_RLINETO]);
            fprintf(dumpFP, "   0 %.3f %s\n", f_rby, gPsCmd[PS_RLINETO]);
            fprintf(dumpFP, "   -%.3f 0 %s\n", f_rbx, gPsCmd[PS_RLINETO]);
            fprintf(dumpFP, "%s %s %s\n\n", gPsCmd[PS_CLOSEPATH],
                  gPsCmd[PS_CLIP], gPsCmd[PS_NEWPATH]);

            if (needsTgifOrigCTM) {
               fprintf(dumpFP, "/tgiforigctm matrix currentmatrix def\n\n");
            }
            if (colorDump && colorBgInPrintingColorPS &&
                  (whereToPrint == PS_FILE || whereToPrint == PDF_FILE ||
                  whereToPrint == PRINTER)) {
               fprintf(dumpFP, "%% Background\n");
               if (myBgColor.red==0 && myBgColor.green==0 &&
                     myBgColor.blue==0) {
                  fprintf(dumpFP, "0 %s\n", gPsCmd[PS_SETGRAY]);
               } else if (myBgColor.red==maxRGB && myBgColor.green==maxRGB &&
                     myBgColor.blue==maxRGB) {
                  fprintf(dumpFP, "1 %s\n", gPsCmd[PS_SETGRAY]);
               } else {
                  fprintf(dumpFP, "%.3f %.3f %.3f %s\n",
                        ((float)myBgColor.red/maxRGB),
                        ((float)myBgColor.green/maxRGB),
                        ((float)myBgColor.blue/maxRGB), gPsCmd[PS_SETRGBCOLOR]);
               }
               fprintf(dumpFP, "%s\n", gPsCmd[PS_NEWPATH]);
               fprintf(dumpFP, "   %.3f %.3f %s\n", real_ltx, real_lty,
                     gPsCmd[PS_MOVETO]);
               fprintf(dumpFP, "   %.3f 0 %s\n", f_rbx, gPsCmd[PS_LINETO]);
               fprintf(dumpFP, "   0 %.3f %s\n", f_rby, gPsCmd[PS_LINETO]);
               fprintf(dumpFP, "   -%.3f 0 %s\n", f_rbx, gPsCmd[PS_LINETO]);
               fprintf(dumpFP, "%s %s %s\n\n", gPsCmd[PS_CLOSEPATH],
                     gPsCmd[PS_FILL], gPsCmd[PS_NEWPATH]);
            }
            for (obj_ptr = botObj; obj_ptr != NULL; obj_ptr = obj_ptr->prev) {
               if (Inside(obj_ptr->bbox, page_bbox) ||
                     BBoxIntersect(obj_ptr->bbox, page_bbox)) {
                  obj_ptr->tmp_parent = NULL;
                  DumpAnObj(dumpFP, obj_ptr);
               }
            }
            fprintf(dumpFP, "%s\n", gPsCmd[PS_GRESTORE]);
            fprintf(dumpFP, "tgifsavedpage restore\n");
            fprintf(dumpFP, "end\n");

            switch (whereToPrint) {
            case PRINTER:
            case PS_FILE:
            case PDF_FILE:
               if (psEopHookStatus == TRUE) {
                  fprintf(dumpFP, "userdict /%s known { %s } if\n",
                        psEopHook, psEopHook);
               }
               fprintf(dumpFP, "showpage\n\n");
               break;
            case EPSI_FILE:
            case LATEX_FIG:
            case SVG_FILE:
               if (showPageInEPS) {
                  fprintf(dumpFP, "showpage\n");
               }
               fprintf(dumpFP, "\n");
               break;
            }
         }
      }
   }
   if (dumpPages) {
      if (dumpPageNum != gPagesToPrintSpec.num_pages_specified) {
         return TRUE;
      }
   } else if (printingPageNum != printingLastPageNum) {
      return TRUE;
   }
   if (whereToPrint < MAXDEFWHERETOPRINT && whereToPrint != TEXT_FILE &&
         whereToPrint != NETLIST_FILE) {
      if ((whereToPrint != LATEX_FIG && whereToPrint != EPSI_FILE &&
            whereToPrint != SVG_FILE) || !minimalEPS) {
         if (fprintf(dumpFP, "%%%%Trailer\n") == EOF) {
            writeFileFailed = TRUE;
         }
      }
      if (pageLayoutMode!=PAGE_STACK ||
            printingFirstPageNum!=printingLastPageNum) {
         if (fprintf(dumpFP, "%%%%BoundingBox: %1d %1d %1d %1d\n",
               llxTotal, llyTotal, urxTotal, uryTotal) == EOF) {
            writeFileFailed = TRUE;
         }
      }
      if (fprintf(dumpFP, "%%MatchingCreationDate: %s\n", loc_time) == EOF) {
         writeFileFailed = TRUE;
      }
      if ((whereToPrint != LATEX_FIG && whereToPrint != EPSI_FILE &&
            whereToPrint != SVG_FILE) || !minimalEPS) {
         DumpDocumentFonts(dumpFP);
         DumpPropagatedEPSInfo(dumpFP);
         if (fprintf(dumpFP, "%%%%EOF\n") == EOF) writeFileFailed = TRUE;
      }
   }
   fclose(dumpFP);
   dumpFP = NULL;

   EndGrayDetection();

   if (whereToPrint < MAXDEFWHERETOPRINT && whereToPrint != TEXT_FILE &&
         whereToPrint != NETLIST_FILE && !totalBBoxValid) {
      strcpy(gszMsgBox, TgLoadString(STID_NO_OBJ_TO_PRINT));
      if (PRTGIF) {
         fprintf(stderr, "%s\n", gszMsgBox);
      } else {
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      }
      return FALSE;
   }
   if (writeFileFailed) {
      writeFileFailed = FALSE;
      FailToWriteFileMessage(tmpFile);
#ifdef KEEP_WHEN_PRINT
      if (whereToPrint != PRINTER) unlink(tmpFile);
#else
      unlink(tmpFile);
#endif
      return FALSE;
   }
   if (preDumpSetup && whereToPrint != NETLIST_FILE) {
      unlink(tmpFile);
      return TRUE;
   }
   switch (whereToPrint) {
   case PRINTER:
#ifdef VMS
#define PRINT_TRAILER ""
#else
#define PRINT_TRAILER " 2>&1"
#endif
      if (PRTGIF) {
         /* do not translate -- program constants */
         if (strstr(printCommand, "%s") == NULL) {
            if (lastFile) {
               sprintf(cmd, "%s %s", printCommand, tmpFile);
            } else {
               sprintf(cmd, "%s -h %s", printCommand, tmpFile);
            }
         } else {
            sprintf(cmd, printCommand, tmpFile);
         }
         if (strcmp(printCommand, "cat") != 0) {
            strcat(cmd, PRINT_TRAILER);
         }
         fprintf(stderr, "%s\n", cmd);
      } else {
         if (strstr(printCommand, "%s") == NULL) {
            sprintf(cmd, "%s %s", printCommand, tmpFile);
         } else {
            sprintf(cmd, printCommand, tmpFile);
         }
         sprintf(gszMsgBox, TgLoadString(STID_PRINTING_WITH_GIVEN_CMD),
               printCommand);
         if (strcmp(printCommand, "cat") != 0) {
            strcat(cmd, PRINT_TRAILER);
         }
         Msg(gszMsgBox);
      }
      if (strcmp(printCommand, "cat") == 0) {
         /*
          * Well, you can't really "cat" to a printer (without piping to
          *       anything else), so you must mean "catting" to stdout!
          */
         FILE *pfp=NULL;

         sprintf(gszMsgBox, TgLoadCachedString(CSTID_EXECUTING_GIVEN_PROGRAM),
               cmd);
         SetStringStatus(gszMsgBox);
         if (!PRTGIF) XSync(mainDisplay, False);
         if ((pfp=(FILE*)popen(cmd, "r")) == NULL) return FALSE;

         while (fgets(gszMsgBox, sizeof(gszMsgBox)-1, pfp) != NULL) {
            printf("%s", gszMsgBox);
         }
         pclose(pfp);
         SetStringStatus(TgLoadCachedString(CSTID_DOTS_DONE));
      } else if (!ExecuteCmd(cmd, FALSE)) {
         if (PRTGIF) {
            fprintf(stderr, TgLoadString(STID_CANNOT_EXEC_ABORT_PRINT), cmd);
            fprintf(stderr, "\n");
         } else {
            sprintf(gszMsgBox, TgLoadString(STID_CANNOT_EXEC_ABORT_PRINT), cmd);
            MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
         }
         unlink(tmpFile);
         return FALSE;
      }
      if (PRTGIF) {
         fprintf(stderr, TgLoadString(STID_NAMED_FILE_PRINTED), tmpFile);
         fprintf(stderr, "\n");
      } else {
         Msg(TgLoadString(STID_PRINT_COMPLETED));
      }
      break;
   case EPSI_FILE:
   case LATEX_FIG:
   case SVG_FILE:
   case PS_FILE:
   case PDF_FILE:
      switch (whereToPrint) {
      case EPSI_FILE:
         if (*epsiExportExtension == '\0') {
            file_ext = EPSF_FILE_EXT;
         } else {
            file_ext = epsiExportExtension;
         }
         output_type = "EPSI";
         break;
      case LATEX_FIG:
         file_ext = EPSF_FILE_EXT;
         output_type = "EPS";
         break;
      case SVG_FILE:
         file_ext = EPSF_FILE_EXT;
         output_type = "SVG";
         break;
      case PS_FILE:
         file_ext = PS_FILE_EXT;
         output_type = "PostScript";
         break;
      case PDF_FILE:
         file_ext = "p~f";
         output_type = "PDF";
         break;
      }
      if (PRTGIF) {
         SetOutputFileName(ps_file, file_ext, NULL, &FileName);
      } else {
         if (!curFileDefined) {
            sprintf(gszMsgBox, TgLoadString(STID_NO_CUR_FILE_CANNOT_GEN_FORMAT),
                  output_type);
            MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
            unlink(tmpFile);
            return FALSE;
         }
         SetOutputFileName(ps_file, file_ext, &short_name, &rest);
         if (short_name && *outputDir=='\0') {
            sprintf(gszMsgBox, TgLoadString(STID_PRINTING_INTO_NAMED_FILE),
                  rest);
         } else {
            sprintf(gszMsgBox, TgLoadString(STID_PRINTING_INTO_NAMED_FILE),
                  ps_file);
         }
         Msg(gszMsgBox);
      }
      if (whereToPrint == EPSI_FILE && generateTiffEPSI) {
         int saved_colordump=colorDump, consensed_already=FALSE, ok=TRUE;

         SaveStatusStrings();
         SetStringStatus(TgLoadString(STID_GENERATING_TIFF_PREVIEW_BMP));

         colorDump = FALSE;
         DumpXBitmapFile(FALSE, FALSE, FALSE);
         colorDump = saved_colordump;

         if (psUseShortHand && !dontCondense) {
            if (CondensePostScriptFile(tmpFile, sizeof(tmpFile))) {
               consensed_already = TRUE;
            } else {
               ok = FALSE;
            }
         }
         if (ok) {
            GenTiffFile(tmpFile, ps_file, psUseShortHand, consensed_already);
         }
         RestoreStatusStrings();
         unlink(tmpFile);
      } else if (whereToPrint == PDF_FILE) {
         char real_ps_file[MAXSTRING+1];

         if (!CopyAFile(tmpFile, (PRTGIF && cmdLineStdOut) ? NULL : ps_file,
               psUseShortHand)) {
            sprintf(gszMsgBox, TgLoadString(STID_FORMAT_FILE_NOT_GENERATED),
                  output_type);
            if (PRTGIF) {
               fprintf(stderr, "%s\n", gszMsgBox);
            } else {
               Msg(gszMsgBox);
            }
            unlink(tmpFile);
            return FALSE;
         }
         UtilStrCpyN(real_ps_file, sizeof(real_ps_file), ps_file);
         if (PRTGIF) {
            SetOutputFileName(ps_file, pdfExportExtension, NULL, &FileName);
         } else {
            SetOutputFileName(ps_file, pdfExportExtension, &short_name,
                  &rest);
            if (short_name && *outputDir=='\0') {
               sprintf(gszMsgBox,
                     TgLoadString(STID_CONVERTING_INTO_NAMED_FILE), rest);
            } else {
               sprintf(gszMsgBox,
                     TgLoadString(STID_CONVERTING_INTO_NAMED_FILE), ps_file);
            }
            Msg(gszMsgBox);
         }
         /* actually, ps_file contains the full path to the PDF file */
         sprintf(cmd, psToPdfCmd, real_ps_file, ps_file);
         if (!ExecuteCmd(cmd, FALSE)) {
            sprintf(gszMsgBox, TgLoadString(STID_CANNOT_EXEC_FORMAT_NOT_GEN),
                  cmd, output_type);
            if (PRTGIF) {
               fprintf(stderr, "%s\n", gszMsgBox);
            } else {
               Msg(gszMsgBox);
            }
            unlink(real_ps_file);
            unlink(tmpFile);
            return FALSE;
         }
         unlink(real_ps_file);
      } else if (whereToPrint == SVG_FILE) {
         char sk_fname[MAXPATHLENGTH], svg_fname[MAXPATHLENGTH];
         char eps_fname[MAXPATHLENGTH];

         /* ps_file contains the full path to an EPS file */
         *sk_fname = *svg_fname = *eps_fname = '\0';
         snprintf(sk_fname, sizeof(sk_fname), "%s.sk", tmpFile);
         snprintf(svg_fname, sizeof(svg_fname), "%s.svg", tmpFile);
         snprintf(eps_fname, sizeof(eps_fname), "%s.eps", tmpFile);
         unlink(sk_fname);
         unlink(svg_fname);
         unlink(eps_fname);

         if (!CopyAFile(tmpFile, (PRTGIF && cmdLineStdOut) ? NULL : eps_fname,
               psUseShortHand)) {
            sprintf(gszMsgBox, TgLoadString(STID_FORMAT_FILE_NOT_GENERATED),
                  output_type);
            if (PRTGIF) {
               fprintf(stderr, "%s\n", gszMsgBox);
            } else {
               Msg(gszMsgBox);
            }
            unlink(tmpFile);
            return FALSE;
         }
         if (PRTGIF) {
            SetOutputFileName(ps_file, "svg", NULL, &FileName);
         } else {
            SetOutputFileName(ps_file, "svg", &short_name, &rest);
            if (short_name && *outputDir=='\0') {
               sprintf(gszMsgBox,
                     TgLoadString(STID_CONVERTING_INTO_NAMED_FILE), rest);
            } else {
               sprintf(gszMsgBox,
                     TgLoadString(STID_CONVERTING_INTO_NAMED_FILE), ps_file);
            }
            Msg(gszMsgBox);
         }
         sprintf(cmd, epsToTmpSvgCmd, eps_fname, sk_fname);
         if (!ExecuteCmd(cmd, FALSE)) {
            sprintf(gszMsgBox, TgLoadString(STID_CANNOT_EXEC_FORMAT_NOT_GEN),
                  cmd, output_type);
            if (PRTGIF) {
               fprintf(stderr, "%s\n", gszMsgBox);
            } else {
               Msg(gszMsgBox);
            }
            unlink(eps_fname);
            unlink(tmpFile);
            return FALSE;
         }
         unlink(eps_fname);

         sprintf(cmd, tmpSvgToSvgCmd, sk_fname, svg_fname);
         if (!ExecuteCmd(cmd, FALSE)) {
            sprintf(gszMsgBox, TgLoadString(STID_CANNOT_EXEC_FORMAT_NOT_GEN),
                  cmd, output_type);
            if (PRTGIF) {
               fprintf(stderr, "%s\n", gszMsgBox);
            } else {
               Msg(gszMsgBox);
            }
            unlink(sk_fname);
            unlink(tmpFile);
            return FALSE;
         }
         unlink(sk_fname);

         if (!FixUpSVGFile(svg_fname, ps_file)) {
            unlink(svg_fname);
            unlink(tmpFile);
            return FALSE;
         }
         unlink(svg_fname);
         /* at this point, ps_file contains the full path to the SVG file */
      } else {
         if (!CopyAFile(tmpFile, (PRTGIF && cmdLineStdOut) ? NULL : ps_file,
               psUseShortHand)) {
            sprintf(gszMsgBox, TgLoadString(STID_FORMAT_FILE_NOT_GENERATED),
                  output_type);
            if (PRTGIF) {
               fprintf(stderr, "%s\n", gszMsgBox);
            } else {
               Msg(gszMsgBox);
            }
            unlink(tmpFile);
            return FALSE;
         }
      }
      if (tmpFileMode != 0 && chmod(ps_file, tmpFileMode)) {
         if (PRTGIF) {
            fprintf(stderr, TgLoadString(STID_CANNOT_CHMOD), ps_file,
                  tmpFileMode);
            fprintf(stderr, "\n");
         } else {
            if (short_name && *outputDir=='\0') {
               sprintf(gszMsgBox, TgLoadString(STID_CANNOT_CHMOD), rest,
                     tmpFileMode);
            } else {
               sprintf(gszMsgBox, TgLoadString(STID_CANNOT_CHMOD), ps_file,
                     tmpFileMode);
            }
            MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
         }
      }
      if (PRTGIF) {
         if (!cmdLineStdOut && !cmdLineQuiet) {
            fprintf(stderr, TgLoadString(STID_FORMAT_FILE_PRINTED_INTO),
                  output_type, ps_file);
            fprintf(stderr, "\n");
         }
      } else {
         if (short_name && *outputDir=='\0') {
            sprintf(gszMsgBox, TgLoadString(STID_FORMAT_FILE_PRINTED_INTO),
                  output_type, rest);
         } else {
            sprintf(gszMsgBox, TgLoadString(STID_FORMAT_FILE_PRINTED_INTO),
                  output_type, ps_file);
         }
         Msg(gszMsgBox);
      }
      break;
   case TEXT_FILE:
      if (PRTGIF) {
         SetOutputFileName(ps_file, TEXT_FILE_EXT, NULL, &FileName);
      } else {
         if (!curFileDefined) {
            sprintf(gszMsgBox, TgLoadString(STID_NO_CUR_FILE_CANNOT_GEN_TEXT));
            MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
            unlink(tmpFile);
            return FALSE;
         }
         SetOutputFileName(ps_file, TEXT_FILE_EXT, &short_name, &rest);
         if (short_name && *outputDir=='\0') {
            sprintf(gszMsgBox, TgLoadString(STID_PRINTING_INTO_NAMED_FILE),
                  rest);
         } else {
            sprintf(gszMsgBox, TgLoadString(STID_PRINTING_INTO_NAMED_FILE),
                  ps_file);
         }
         Msg(gszMsgBox);
      }
      if (!CopyAFile(tmpFile, (PRTGIF && cmdLineStdOut) ? NULL : ps_file,
            FALSE)) {
         if (PRTGIF) {
            fprintf(stderr, "%s\n", TgLoadString(STID_TEXT_OUTPUT_NOT_GEN));
         } else {
            Msg(TgLoadString(STID_TEXT_OUTPUT_NOT_GEN));
         }
         unlink(tmpFile);
         return FALSE;
      }
      if (tmpFileMode != 0 && chmod(ps_file, tmpFileMode)) {
         if (PRTGIF) {
            fprintf(stderr, TgLoadString(STID_CANNOT_CHMOD), ps_file,
                  tmpFileMode);
         } else {
            if (short_name && *outputDir=='\0') {
               sprintf(gszMsgBox, TgLoadString(STID_CANNOT_CHMOD), rest,
                     tmpFileMode);
            } else {
               sprintf(gszMsgBox, TgLoadString(STID_CANNOT_CHMOD), ps_file,
                     tmpFileMode);
            }
            MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
         }
      }
      if (PRTGIF) {
         if (!cmdLineStdOut && !cmdLineQuiet) {
            fprintf(stderr, TgLoadString(STID_TEXT_PRINTED_INTO_NAMED_FILE),
                  ps_file);
            fprintf(stderr, "\n");
         }
      } else {
         if (short_name && *outputDir=='\0') {
            sprintf(gszMsgBox, TgLoadString(STID_TEXT_PRINTED_INTO_NAMED_FILE),
                  rest);
         } else {
            sprintf(gszMsgBox, TgLoadString(STID_TEXT_PRINTED_INTO_NAMED_FILE),
                  ps_file);
         }
         Msg(gszMsgBox);
      }
      break;
   default:
      /*
       * whereToPrint >= MAXDEFWHERETOPRINT or whereToPrint == NETLIST_FILE
       */
      if (PRTGIF) {
         SetOutputFileName(ps_file, GetExportExt(whereToPrint), NULL,
               &FileName);
      } else {
         if (!curFileDefined) {
            sprintf(gszMsgBox, TgLoadString(STID_NO_CUR_FILE_CANNOT_GEN_FORMAT),
                  GetExportName(whereToPrint, EXPORT_THIS));
            MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
            unlink(tmpFile);
            return FALSE;
         }
         SetOutputFileName(ps_file, GetExportExt(whereToPrint), &short_name,
               &rest);
         if (short_name && *outputDir=='\0') {
            sprintf(gszMsgBox, TgLoadString(STID_PRINTING_INTO_NAMED_FILE),
                  rest);
         } else {
            sprintf(gszMsgBox, TgLoadString(STID_PRINTING_INTO_NAMED_FILE),
                  ps_file);
         }
         Msg(gszMsgBox);
      }
      if (!CopyAFile(tmpFile, (PRTGIF && cmdLineStdOut) ? NULL : ps_file,
            FALSE)) {
         sprintf(gszMsgBox, TgLoadString(STID_FORMAT_FILE_NOT_GENERATED),
               GetExportName(whereToPrint, EXPORT_THIS));
         if (PRTGIF) {
            fprintf(stderr, "%s\n", gszMsgBox);
         } else {
            Msg(gszMsgBox);
         }
         unlink(tmpFile);
         return FALSE;
      }
      if (tmpFileMode != 0 && chmod(ps_file, tmpFileMode)) {
         if (PRTGIF) {
            fprintf(stderr, TgLoadString(STID_CANNOT_CHMOD), ps_file,
                  tmpFileMode);
         } else {
            if (short_name && *outputDir=='\0') {
               sprintf(gszMsgBox, TgLoadString(STID_CANNOT_CHMOD), rest,
                     tmpFileMode);
            } else {
               sprintf(gszMsgBox, TgLoadString(STID_CANNOT_CHMOD), ps_file,
                     tmpFileMode);
            }
            MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
         }
      }
      if (PRTGIF) {
         if (!cmdLineStdOut && !cmdLineQuiet) {
            fprintf(stderr, TgLoadString(STID_FORMAT_FILE_PRINTED_INTO),
                  GetExportName(whereToPrint, EXPORT_THIS), ps_file);
            fprintf(stderr, "\n");
         }
      } else {
         if (short_name && *outputDir=='\0') {
            sprintf(gszMsgBox, TgLoadString(STID_FORMAT_FILE_PRINTED_INTO),
                  GetExportName(whereToPrint, EXPORT_THIS), rest);
         } else {
            sprintf(gszMsgBox, TgLoadString(STID_FORMAT_FILE_PRINTED_INTO),
                  GetExportName(whereToPrint, EXPORT_THIS), ps_file);
         }
         Msg(gszMsgBox);
      }
      break;
   }
#ifdef KEEP_WHEN_PRINT
   if (whereToPrint != PRINTER) {
      unlink(tmpFile);
   }
#else
   unlink(tmpFile);
#endif
   return TRUE;
}

static
int GenDump(FileName)
   char *FileName;
{
   int rc=INVALID, saved_where_to_print=whereToPrint;
   static int saved_dont_condense=INVALID;

   if (preDumpSetup) {
      CleanUpPropagatedEPSInfo();
      saved_dont_condense = dontCondense;
      hasReadHexString = FALSE;
   }
   xpmHasTooManyColorsForPrinting = FALSE;
   if (whereToPrint == TIFFEPSI_FILE) {
      whereToPrint = EPSI_FILE;
      generateTiffEPSI = TRUE;
   }
   SaveStatusStrings();
   rc = DoGenDump(FileName);
   RestoreStatusStrings();
   if (saved_where_to_print == TIFFEPSI_FILE) {
      whereToPrint = TIFFEPSI_FILE;
      generateTiffEPSI = FALSE;
   }
   if (preDumpSetup && hasReadHexString) {
      if (!dontCondense) {
         strcpy(gszMsgBox, TgLoadString(STID_WONT_CONDENSE_READHEXSTRING));
         if (PRTGIF) {
            fprintf(stderr, "%s\n", gszMsgBox);
         } else {
            Msg(gszMsgBox);
         }
      }
      dontCondense = TRUE;
   }
   if (!preDumpSetup && xpmHasTooManyColorsForPrinting) {
      MsgBox(TgLoadString(STID_TOO_MANY_COLOR_FOR_XPM_EXPORT), TOOL_NAME,
            INFO_MB);
   }
   if (!preDumpSetup) {
      if (hasReadHexString) {
         dontCondense = saved_dont_condense;
         hasReadHexString = FALSE;
      }
      CleanUpPropagatedEPSInfo();
   }
   return rc;
}

void Dump(FileName)
   char *FileName;
{
   int ok=TRUE;
   struct PageRec *saved_cur_page;

   if ((whereToPrint == LATEX_FIG || whereToPrint == EPSI_FILE ||
         whereToPrint == TIFFEPSI_FILE || whereToPrint == SVG_FILE) &&
         pageLayoutMode == PAGE_TILE && (paperCol != 1 || paperRow != 1)) {
      strcpy(gszMsgBox, TgLoadString(STID_CANNOT_PRINT_EPS_TILED_PAGE));
      if (PRTGIF) {
         fprintf(stderr, "%s\n", gszMsgBox);
      } else {
         MsgBox(TgLoadString(STID_CANNOT_PRINT_EPS_TILED_PAGE), TOOL_NAME,
               INFO_MB);
      }
      return;
   }
   if (PRTGIF) {
      RemoveFileNameExtension(FileName);
      if (cmdLineOneFilePerPage ||
            (cmdLineHasPageNum && pageLayoutMode==PAGE_STACK &&
            (whereToPrint==PS_FILE || whereToPrint==PDF_FILE ||
            whereToPrint==PRINTER))) {
         totalBBoxValid = FALSE;
         printingFirstPageNum = printingPageNum = printingLastPageNum =
               curPageNum;
         dumpOnePageInStackMode = TRUE;
      }
   } else {
      HighLightReverse();
      SetWatchCursor(drawWindow);
      SetWatchCursor(mainWindow);
   }
   totalBBoxValid = FALSE;
   if (whereToPrint == PRINTER || whereToPrint == PS_FILE ||
         whereToPrint == PDF_FILE || whereToPrint == TEXT_FILE ||
         whereToPrint == NETLIST_FILE || (whereToPrint >= MAXDEFWHERETOPRINT &&
         (GetExportFilterCapability(whereToPrint) & TGEF_CAP_MULTI_PAGE) !=
         0)) {
      if (PRTGIF && (cmdLineOneFilePerPage || (cmdLineHasPageNum &&
            pageLayoutMode==PAGE_STACK && (whereToPrint==PS_FILE ||
            whereToPrint==PDF_FILE || whereToPrint==PRINTER)))) {
         if (whereToPrint==PRINTER || whereToPrint==LATEX_FIG ||
               whereToPrint==PS_FILE || whereToPrint==PDF_FILE ||
               whereToPrint==EPSI_FILE || whereToPrint==TIFFEPSI_FILE ||
               whereToPrint==NETLIST_FILE || whereToPrint==SVG_FILE) {
            ResetPSInfo();
            CacheColorsForPrinting();

            ok = GenDump(FileName);

            DoneResetPSInfo();
         }
         if (ok) GenDump(FileName);
         CleanUpCachedColorsForPrinting();
      } else {
         XColor *saved_tgif_colors=tgifColors;

         if (printUsingRequestedColor) tgifColors = tgifRequestedColors;

         if (whereToPrint==PRINTER || whereToPrint==LATEX_FIG ||
               whereToPrint==PS_FILE || whereToPrint==PDF_FILE ||
               whereToPrint==EPSI_FILE || whereToPrint==TIFFEPSI_FILE ||
               whereToPrint==NETLIST_FILE || whereToPrint==SVG_FILE) {
            ResetPSInfo();
            CacheColorsForPrinting();

            printingFirstPageNum = 1;
            printingPageNum = 1;
            printingLastPageNum = lastPageNum;
            saved_cur_page = curPage;
            SaveStatusStrings();
            for (curPage=firstPage; ok && curPage != NULL;
                  curPage=curPage->next, printingPageNum++) {
               topObj = curPage->top;
               botObj = curPage->bot;
               sprintf(gszMsgBox, TgLoadString(STID_PREPROCESS_PAGE_OF),
                     printingPageNum, lastPageNum);
               SetStringStatus(gszMsgBox);
               ok = GenDump(FileName);
            }
            RestoreStatusStrings();
            curPage = saved_cur_page;
            topObj = curPage->top;
            botObj = curPage->bot;

            DoneResetPSInfo();
         }
         printingFirstPageNum = 1;
         printingPageNum = 1;
         printingLastPageNum = lastPageNum;
         saved_cur_page = curPage;
         SaveStatusStrings();
         for (curPage=firstPage; ok && curPage != NULL;
               curPage=curPage->next, printingPageNum++) {
            topObj = curPage->top;
            botObj = curPage->bot;
            sprintf(gszMsgBox, TgLoadString(STID_GENERATING_PAGE_OF),
                  printingPageNum, lastPageNum);
            SetStringStatus(gszMsgBox);
            ok = GenDump(FileName);
         }
         CleanUpCachedColorsForPrinting();
         RestoreStatusStrings();
         curPage = saved_cur_page;
         topObj = curPage->top;
         botObj = curPage->bot;

         if (printUsingRequestedColor) tgifColors = saved_tgif_colors;
      }
   } else {
      XColor *saved_tgif_colors=tgifColors;

      if (printUsingRequestedColor) tgifColors = tgifRequestedColors;

      if (whereToPrint==PRINTER || whereToPrint==LATEX_FIG ||
            whereToPrint==PS_FILE || whereToPrint==PDF_FILE ||
            whereToPrint==EPSI_FILE || whereToPrint==TIFFEPSI_FILE ||
            whereToPrint==SVG_FILE) {
         ResetPSInfo();
         CacheColorsForPrinting();

         printingFirstPageNum = 1;
         printingPageNum = 1;
         printingLastPageNum = 1;
         ok = GenDump(FileName);

         DoneResetPSInfo();
      }
      printingFirstPageNum = 1;
      printingPageNum = 1;
      printingLastPageNum = 1;
      if (ok) GenDump(FileName);
      CleanUpCachedColorsForPrinting();

      if (printUsingRequestedColor) tgifColors = saved_tgif_colors;
   }
   if (PRTGIF) {
      if (cmdLineOneFilePerPage || (cmdLineHasPageNum &&
            pageLayoutMode==PAGE_STACK && (whereToPrint==PS_FILE ||
            whereToPrint==PDF_FILE || whereToPrint==PRINTER))) {
         dumpOnePageInStackMode = FALSE;
      }
   } else {
      SetDefaultCursor(mainWindow);
      ShowCursor();
      HighLightForward();
   }
}

void DumpPages()
{
   struct PageRec *saved_cur_page=NULL;
   int ok=TRUE;
   XColor *saved_tgif_colors=tgifColors;

   if (printUsingRequestedColor) tgifColors = tgifRequestedColors;

   SetWatchCursor(drawWindow);
   SetWatchCursor(mainWindow);

   ResetPSInfo();
   CacheColorsForPrinting();

   printingFirstPageNum = INVALID;
   printingPageNum = 1;
   printingLastPageNum = lastPageNum;
   saved_cur_page = curPage;
   totalBBoxValid = FALSE;
   SaveStatusStrings();
   for (curPage=firstPage; ok && curPage != NULL; curPage=curPage->next,
         printingPageNum++) {
      if (gPagesToPrintSpec.page_specified[printingPageNum-1]) {
         if (printingFirstPageNum == INVALID) {
            printingFirstPageNum = printingPageNum;
         }
         topObj = curPage->top;
         botObj = curPage->bot;
         sprintf(gszMsgBox, TgLoadString(STID_PREPROCESS_PAGE_OF),
               printingPageNum, lastPageNum);
         SetStringStatus(gszMsgBox);
         ok = GenDump("");
      }
   }
   RestoreStatusStrings();
   curPage = saved_cur_page;
   topObj = curPage->top;
   botObj = curPage->bot;

   DoneResetPSInfo();

   dumpPages = TRUE;
   dumpPageNum = 0;
   printingFirstPageNum = INVALID;
   printingPageNum = 1;
   printingLastPageNum = lastPageNum;
   saved_cur_page = curPage;
   totalBBoxValid = FALSE;
   SaveStatusStrings();
   for (curPage=firstPage; ok && curPage != NULL; curPage=curPage->next,
         printingPageNum++) {
      if (gPagesToPrintSpec.page_specified[printingPageNum-1]) {
         if (printingFirstPageNum == INVALID) {
            printingFirstPageNum = printingPageNum;
         }
         dumpPageNum++;
         topObj = curPage->top;
         botObj = curPage->bot;
         sprintf(gszMsgBox, TgLoadString(STID_GENERATING_PAGE_OF),
               printingPageNum, lastPageNum);
         SetStringStatus(gszMsgBox);
         ok = GenDump("");
      }
   }
   CleanUpCachedColorsForPrinting();
   RestoreStatusStrings();
   curPage = saved_cur_page;
   topObj = curPage->top;
   botObj = curPage->bot;

   dumpPages = FALSE;
   dumpPageNum = INVALID;

   SetDefaultCursor(mainWindow);
   ShowCursor();

   if (printUsingRequestedColor) tgifColors = saved_tgif_colors;
}

void DumpOnePageInTileMode(row, col)
   int row, col;
{
   int ok=TRUE;
   XColor *saved_tgif_colors=tgifColors;

   if (printUsingRequestedColor) tgifColors = tgifRequestedColors;

   SetWatchCursor(drawWindow);
   SetWatchCursor(mainWindow);

   totalBBoxValid = FALSE;
   printingFirstPageNum = printingPageNum = printingLastPageNum =
         row*paperCol+col+1;
   dumpOnePageInTileMode = TRUE;
   if (whereToPrint==PRINTER || whereToPrint==LATEX_FIG ||
         whereToPrint==PS_FILE || whereToPrint==PDF_FILE ||
         whereToPrint==EPSI_FILE || whereToPrint==TIFFEPSI_FILE ||
         whereToPrint==SVG_FILE) {
      ResetPSInfo();
      CacheColorsForPrinting();
      ok = GenDump("");
      DoneResetPSInfo();
   }
   if (ok) GenDump("");
   CleanUpCachedColorsForPrinting();
   dumpOnePageInTileMode = FALSE;

   SetDefaultCursor(mainWindow);
   ShowCursor();

   if (printUsingRequestedColor) tgifColors = saved_tgif_colors;
}

void DumpOnePageInStackMode()
{
   int ok=TRUE;
   XColor *saved_tgif_colors=tgifColors;

   if (printUsingRequestedColor) tgifColors = tgifRequestedColors;

   SetWatchCursor(drawWindow);
   SetWatchCursor(mainWindow);

   totalBBoxValid = FALSE;
   printingFirstPageNum = printingPageNum = printingLastPageNum = curPageNum;
   dumpOnePageInStackMode = TRUE;
   if (whereToPrint==PRINTER || whereToPrint==LATEX_FIG ||
         whereToPrint==PS_FILE || whereToPrint==PDF_FILE ||
         whereToPrint==EPSI_FILE || whereToPrint==TIFFEPSI_FILE ||
         whereToPrint==SVG_FILE) {
      ResetPSInfo();
      CacheColorsForPrinting();
      ok = GenDump("");
      DoneResetPSInfo();
   }
   if (ok) GenDump("");
   CleanUpCachedColorsForPrinting();
   dumpOnePageInStackMode = FALSE;

   SetDefaultCursor(mainWindow);
   ShowCursor();

   if (printUsingRequestedColor) tgifColors = saved_tgif_colors;
}

void DumpOneFilePerPage()
{
   int ok=TRUE, saved_cur_page_num=curPageNum;
   XColor *saved_tgif_colors=tgifColors;

   if (printUsingRequestedColor) tgifColors = tgifRequestedColors;

   SetWatchCursor(drawWindow);
   SetWatchCursor(mainWindow);

   dumpOneFilePerPage = TRUE;
   dumpOnePageInStackMode = TRUE;
   for (curPageNum=1; ok && curPageNum <= lastPageNum; curPageNum++) {
      totalBBoxValid = FALSE;
      GotoPageNum(curPageNum);
      ShowPage();
      XSync(mainDisplay, False);

      printingFirstPageNum = printingPageNum = printingLastPageNum = curPageNum;
      if (gPagesToPrintSpec.page_specified[curPageNum-1]) {
         if (whereToPrint==LATEX_FIG || whereToPrint==PS_FILE ||
               whereToPrint==PDF_FILE || whereToPrint==EPSI_FILE ||
               whereToPrint==TIFFEPSI_FILE || whereToPrint==SVG_FILE) {
            ResetPSInfo();
            CacheColorsForPrinting();
            ok = GenDump("");
            DoneResetPSInfo();
            if (ok) ok = GenDump("");
            CleanUpCachedColorsForPrinting();
         } else {
            ok = GenDump("");
         }
      }
   }
   dumpOnePageInStackMode = FALSE;
   dumpOneFilePerPage = FALSE;

   SetDefaultCursor(mainWindow);
   ShowCursor();

   if (printUsingRequestedColor) tgifColors = saved_tgif_colors;

   if (!ok && curPageNum <= lastPageNum) {
      if (curPageNum == lastPageNum) {
         sprintf(gszMsgBox, TgLoadString(STID_GIVEN_PAGE_NOT_GENERATED),
               lastPageNum);
      } else {
         sprintf(gszMsgBox, TgLoadString(STID_PAGE_RANGE_NOT_GENERATED),
               curPageNum, lastPageNum);
      }
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
   }
   if (saved_cur_page_num != curPageNum) {
      GotoPageNum(saved_cur_page_num);
      ClearAndRedrawDrawWindow();
      ShowPage();
   }
}

void PrintWithCommand(FileName)
   char *FileName;
{
   struct PageRec *saved_cur_page=NULL;
   int ok=TRUE;
   XColor *saved_tgif_colors=tgifColors;

   if (whereToPrint != PRINTER) {
      MsgBox(TgLoadString(STID_PRINTWITHCMD_WORKS_PRINTER), TOOL_NAME, INFO_MB);
      return;
   }
   Dialog(TgLoadString(STID_ENTER_PRINT_CMD_NAME),
         TgLoadCachedString(CSTID_DLG_ACCEPT_CANCEL), printCommand);
   if (*printCommand == '\0') return;

   if (printUsingRequestedColor) tgifColors = tgifRequestedColors;

   SetWatchCursor(drawWindow);
   SetWatchCursor(mainWindow);

   if (whereToPrint==PRINTER || whereToPrint==LATEX_FIG ||
         whereToPrint==PS_FILE || whereToPrint==PDF_FILE ||
         whereToPrint==EPSI_FILE || whereToPrint==TIFFEPSI_FILE ||
         whereToPrint==SVG_FILE) {
      ResetPSInfo();
      CacheColorsForPrinting();

      printingPageNum = 1;
      printingLastPageNum = lastPageNum;
      saved_cur_page = curPage;
      totalBBoxValid = FALSE;
      SaveStatusStrings();
      for (curPage=firstPage; ok && curPage != NULL; curPage=curPage->next,
            printingPageNum++) {
         topObj = curPage->top;
         botObj = curPage->bot;
         sprintf(gszMsgBox, TgLoadString(STID_PREPROCESS_PAGE_OF),
               printingPageNum, lastPageNum);
         SetStringStatus(gszMsgBox);
         ok = GenDump(FileName);
      }
      RestoreStatusStrings();
      curPage = saved_cur_page;
      topObj = curPage->top;
      botObj = curPage->bot;

      DoneResetPSInfo();
   }
   printingPageNum = 1;
   printingLastPageNum = lastPageNum;
   saved_cur_page = curPage;
   totalBBoxValid = FALSE;
   SaveStatusStrings();
   for (curPage=firstPage; ok && curPage != NULL; curPage=curPage->next,
         printingPageNum++) {
      topObj = curPage->top;
      botObj = curPage->bot;
      sprintf(gszMsgBox, TgLoadString(STID_GENERATING_PAGE_OF), printingPageNum,
            lastPageNum);
      SetStringStatus(gszMsgBox);
      ok = GenDump(FileName);
   }
   CleanUpCachedColorsForPrinting();
   RestoreStatusStrings();
   curPage = saved_cur_page;
   topObj = curPage->top;
   botObj = curPage->bot;

   SetDefaultCursor(mainWindow);
   ShowCursor();

   if (printUsingRequestedColor) tgifColors = saved_tgif_colors;
}

void PrintSelectedObjs()
{
   struct SelRec *top_sel_ptr, *bot_sel_ptr, *sel_ptr, *next_sel;
   struct ObjRec *saved_top_obj, *saved_bot_obj, *obj_ptr;

   if (topSel == NULL) {
      MsgBox(TgLoadString(STID_NO_OBJ_SEL_NOTHING_TO_PRINT), TOOL_NAME,
            INFO_MB);
      return;
   }
   HighLightReverse();
   PushPageInfo();
   saved_top_obj = topObj;
   saved_bot_obj = botObj;

   JustDupSelObj(&top_sel_ptr, &bot_sel_ptr);
   curPage->top = topObj = top_sel_ptr->obj;
   curPage->bot = botObj = bot_sel_ptr->obj;
   for (sel_ptr=topSel, obj_ptr=topObj; obj_ptr!=NULL;
         sel_ptr=sel_ptr->next, obj_ptr=obj_ptr->next) {
      CopyObjId(sel_ptr->obj, obj_ptr);
      CopyObjLocks(sel_ptr->obj, obj_ptr);
   }

   if (pageLayoutMode == PAGE_STACK) {
      DumpOnePageInStackMode();
   } else {
      int ok=TRUE;
      XColor *saved_tgif_colors=tgifColors;

      if (printUsingRequestedColor) tgifColors = tgifRequestedColors;

      printingFirstPageNum = 1;
      printingPageNum = 1;
      printingLastPageNum = 1;
      totalBBoxValid = FALSE;
      if (whereToPrint==PRINTER || whereToPrint==LATEX_FIG ||
            whereToPrint==PS_FILE || whereToPrint==PDF_FILE ||
            whereToPrint==EPSI_FILE || whereToPrint==TIFFEPSI_FILE ||
            whereToPrint==SVG_FILE) {
         ResetPSInfo();
         CacheColorsForPrinting();
         ok = GenDump("");
         DoneResetPSInfo();
      }
      if (ok) GenDump("");
      CleanUpCachedColorsForPrinting();

      if (printUsingRequestedColor) tgifColors = saved_tgif_colors;
   }
   DelAllObj();
   for (sel_ptr = top_sel_ptr; sel_ptr != NULL; sel_ptr = next_sel) {
      next_sel = sel_ptr->next;
      free(sel_ptr);
   }
   PopPageInfo();
   curPage->top = topObj = saved_top_obj;
   curPage->bot = botObj = saved_bot_obj;
   RedrawAnArea(botObj, selLtX-GRID_ABS_SIZE(1), selLtY-GRID_ABS_SIZE(1),
         selRbX+GRID_ABS_SIZE(1), selRbY+GRID_ABS_SIZE(1));
   HighLightForward();
}

void SetPrintReduction()
{
   float fval=(float)0;
   double right_margin=(double)0;
   char buf[MAXSTRING+1];

   *buf = '\0';
   Dialog(TgLoadString(STID_SPECIFY_PERCENT_REDUCTION),
         TgLoadCachedString(CSTID_DLG_ACCEPT_CANCEL), buf);
   if (*buf == '\0') return;

   if (sscanf(buf, "%f", &fval) != 1) {
      sprintf(gszMsgBox, TgLoadString(STID_INVALID_REDUCTION), buf);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return;
   }
   if (rightMarginEnabled == TRUE) {
      right_margin = rightMargin * ((double)printMag) / ((double)100);
   }
   printMag = fval;
   if (UpdPageStyle(pageStyle)) {
      UpdDrawWinBBox();
      AdjSplineVs();
      ClearAndRedrawDrawWindow();
   }
   if (rightMarginEnabled == TRUE) {
      rightMargin = right_margin * ((double)100) / ((double)printMag);
   }
   RedrawScrollBars();
   RedrawRulers();
   RedrawTitleWindow();
   SetFileModified(TRUE);

   FormatFloat(&printMag, buf);
   if (printMag <= 100.0) {
      sprintf(gszMsgBox, TgLoadString(STID_NEW_REDUCTION_IS_PERCENT), buf);
   } else {
      sprintf(gszMsgBox, TgLoadString(STID_NEW_ENLARGEMENT_IS_PERCENT), buf);
   }
   Msg(gszMsgBox);
}

static
void DoNewProc(clear_all_wb)
   int clear_all_wb;
{
   if (!gstWBInfo.do_whiteboard) {
      CleanUpCmds();
   }
   CleanUpDrawingWindow();
   ClearFileInfo(TRUE);
   ClearAndRedrawDrawWindow();

   CheckFileAttrsInLoad();

   Msg(TgLoadString(STID_EDITING_NO_FILE));
   if (gstWBInfo.do_whiteboard) {
      if (clear_all_wb) {
         RecordWBClearAll();
      }
   } else {
      objId = 0;
   }
   RedrawTitleWindow();
   DelAllPages();
   lastPageNum = 1;
   InitPage();
   ShowPage();

   rightMargin = defRightMargin;
   rightMarginEnabled = defRightMarginEnabled;
   rightMarginActive = (rightMarginEnabled == TRUE);
   RedrawHRulerWindow();
}

void NewProc()
{
   if (gstWBInfo.do_whiteboard) {
      XBell(mainDisplay, 0);
      if (MsgBox(TgLoadString(STID_OK_TO_CLEAR_WHITEBOARD), TOOL_NAME,
            YNC_MB) != MB_ID_YES) {
         return;
      }
      TieLooseEnds();
      SetFileModified(FALSE);
   } else {
      while (!DirIsRemote(curDir) && fileModified && !IsFiletUnSavable()) {
         XBell(mainDisplay, 0);
         switch (MsgBox(TgLoadString(STID_FILE_MOD_SAVE_BEFORE_CLEAR),
               TOOL_NAME, YNC_MB)) {
         case MB_ID_YES: SaveFile(); break;
         case MB_ID_NO: TieLooseEnds(); SetFileModified(FALSE); break;
         case MB_ID_CANCEL: return;
         }
      }
      if (!DirIsRemote(curDir) && fileModified && IsFiletUnSavable()) {
         TieLooseEnds();
         SetFileModified(FALSE);
      }
   }
   if (inHyperSpace && !inSlideShow) ToggleHyperSpace(FALSE);

   DoNewProc(TRUE);
}

int BeforeOpenURL(pn_do_not_save)
   int *pn_do_not_save;
{
   int do_not_save=FALSE;

   if (inSlideShow) {
      XBell(mainDisplay, 0);
      MsgBox(TgLoadString(STID_CANNOT_OPEN_IN_SLIDESHOW), TOOL_NAME, INFO_MB);
      return FALSE;
   } else if (gstWBInfo.do_whiteboard) {
      XBell(mainDisplay, 0);
      if (MsgBox(TgLoadString(STID_OK_TO_CLEAR_WHITEBOARD), TOOL_NAME,
            YNC_MB) != MB_ID_YES) {
         return FALSE;
      }
   } else {
      while (!DirIsRemote(curDir) && fileModified && !IsFiletUnSavable()) {
         XBell(mainDisplay, 0);
         switch (MsgBox(TgLoadString(STID_FILE_MOD_SAVE_BEFORE_OPEN), TOOL_NAME,
               YNC_MB)) {
         case MB_ID_YES: SaveFile(); break;
         case MB_ID_NO: do_not_save = TRUE; SetFileModified(FALSE); break;
         case MB_ID_CANCEL: return FALSE;
         }
      }
      if (!DirIsRemote(curDir) && fileModified && IsFiletUnSavable()) {
         do_not_save = TRUE;
         SetFileModified(FALSE);
      }
   }
   if (pn_do_not_save != NULL) *pn_do_not_save = do_not_save;

   return TRUE;
}

void OpenURL(file_name, do_not_save, pn_need_to_check_auto_exec)
   char *file_name;
   int do_not_save, *pn_need_to_check_auto_exec;
{
   int need_to_check_auto_exec=FALSE;

   if (gstWBInfo.do_whiteboard) {
      DoNewProc(FALSE);
   }
   SetWatchCursor(drawWindow);
   SetWatchCursor(mainWindow);

   if (FileIsRemote(file_name)) {
      char remote_fname[MAXPATHLENGTH+1];
      char *page_spec=NULL;

      if (!FormNewFileName(curDir, file_name, NULL, remote_fname,
            &page_spec)) {
         sprintf(gszMsgBox, TgLoadString(STID_INVALID_REMOTE_FNAME),
               file_name);
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
         if (do_not_save) SetFileModified(TRUE);
      } else {
         char *buf=NULL, *content_type=NULL, final_url[MAXPATHLENGTH+1];
         int rc, buf_sz=0, is_html=FALSE;

         *final_url = '\0';
         SaveStatusStrings();
         rc = LoadRemoteFileInMem(remote_fname, &buf, &content_type,
               &buf_sz, &is_html, TRUE, final_url, sizeof(final_url));
         RestoreStatusStrings();
         if (rc && buf != NULL) {
            if (*final_url != '\0') {
               UtilStrCpyN(remote_fname, sizeof(remote_fname), final_url);
            }
            if (LoadRemoteFileFromMem(remote_fname, buf, content_type,
                  buf_sz, is_html)) {
               need_to_check_auto_exec = TRUE;
            }
         } else if (do_not_save) {
            SetFileModified(TRUE);
         }
         if (content_type != NULL) FreeRemoteBuf(content_type);
         if (buf != NULL) FreeRemoteBuf(buf);
         if (page_spec != NULL) {
            int new_page_num=(-1);

            need_to_check_auto_exec = FALSE;
            if (!GetPageNumFromPageSpec(page_spec, &new_page_num)) {
               sprintf(gszMsgBox,
                     TgLoadString(STID_INVALID_PAGE_SPECIFIED_FOR),
                     file_name);
               MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
            } else if (new_page_num != curPageNum) {
               BeforeNavigate();
               GotoPageNum(new_page_num);
               ShowPage();
               ClearAndRedrawDrawWindow();
               RedrawTitleWindow();
               RedrawRulers();
               RedrawScrollBars();
               justDupped = FALSE;
            }
         }
      }
      if (page_spec != NULL) free(page_spec);
   } else {
      int gzipped=FALSE, obj_file=FALSE;

      obj_file = FileNameHasExtension(file_name, OBJ_FILE_TYPE, &gzipped,
            NULL);
      if (!LoadFile(file_name, obj_file, obj_file && gzipped)) {
         if (do_not_save) {
            SetFileModified(TRUE);
         }
      } else {
         need_to_check_auto_exec = TRUE;
      }
   }
   SetDefaultCursor(mainWindow);
   ShowCursor();
   if (gstWBInfo.do_whiteboard && (topObj != NULL || tgifObj->fattr != NULL)) {
#ifdef _TGIF_WB2_NO_DELAY_BUG
      sleep(1);
#endif /* ~_TGIF_WB2_NO_DELAY_BUG */
      /*
       * In slideshow mode, SelAllObj() will *not* select anything!
       * Since we really want the objects to be selected (because
       *         we want to record them), we must set ignore_slideshow
       *         to TRUE when we call SelAllObj().
       */
      if (tgifObj->fattr != NULL) {
         StartCompositeCmd();
      }
      if (topObj != NULL) {
         SelAllObj(FALSE, TRUE);
         PrepareToRecord(CMD_NEW, NULL, NULL, 0);
         RecordCmd(CMD_NEW, NULL, topSel, botSel, numObjSelected);
         RemoveAllSel();
      }
      if (tgifObj->fattr != NULL) {
         struct AttrRec *saved_fattr=tgifObj->fattr;
         struct AttrRec *saved_lattr=tgifObj->lattr;

         tgifObj->fattr = tgifObj->lattr = NULL;

         AddObj(NULL, topObj, tgifObj);
         AddNewSelObj(topObj);
         PrepareToReplaceAnObj(tgifObj);

         tgifObj->fattr = saved_fattr;
         tgifObj->lattr = saved_lattr;

         recordCmdIncludeTgifObj = TRUE;
         RecordReplaceAnObj(tgifObj);
         recordCmdIncludeTgifObj = FALSE;

         UnlinkObj(topObj);
         FreeTopSel();

         EndCompositeCmd();
      }
   }
   if (pn_need_to_check_auto_exec != NULL) {
      *pn_need_to_check_auto_exec = need_to_check_auto_exec;
   }
}

void AfterOpenURL(need_to_check_auto_exec)
   int need_to_check_auto_exec;
{
   if (need_to_check_auto_exec) {
      struct AttrRec *exec_attr=FindFileAttrWithName("auto_exec=");

      if (exec_attr != NULL) {
         DoExecLoop(NULL, exec_attr);
      }
   }
}

void OpenProc(psz_fname_spec)
   char *psz_fname_spec;
{
   char file_name[MAXPATHLENGTH+1];
   int do_not_save=FALSE, need_to_check_auto_exec=FALSE;

   if (!BeforeOpenURL(&do_not_save)) {
      return;
   }
   if (psz_fname_spec != NULL && strcmp(psz_fname_spec, "-1") != 0) {
      int len=strlen(psz_fname_spec);
      char new_full_name[MAXPATHLENGTH<<2];

      if (len > 0 && psz_fname_spec[len-1] == ')') {
         psz_fname_spec[len-1] = '\0';
         UtilStrCpyN(file_name, sizeof(file_name), psz_fname_spec);
         psz_fname_spec[len-1] = ')';
      } else {
         UtilStrCpyN(file_name, sizeof(file_name), psz_fname_spec);
      }
      if (FileIsRemote(file_name)) {
         OpenURL(file_name, do_not_save, &need_to_check_auto_exec);
      } else {
         if (*file_name == DIR_SEP) {
            strcpy(new_full_name, file_name);
         } else if (curDirIsLocal) {
            sprintf(new_full_name, "%s%c%s", curDir, DIR_SEP, file_name);
         } else {
            sprintf(new_full_name, "%s%c%s", curLocalDir, DIR_SEP, file_name);
         }
         OpenURL(new_full_name, do_not_save, &need_to_check_auto_exec);
      }
   } else {
      if (SelectFileName(TgLoadString(STID_SELECT_A_FILE_TO_OPEN), file_name) !=
            INVALID) {
         OpenURL(file_name, do_not_save, &need_to_check_auto_exec);
      } else if (do_not_save) {
         SetFileModified(TRUE);
      }
   }
   AfterOpenURL(need_to_check_auto_exec);
}

void SetTemplate()
{
   char file_name[MAXPATHLENGTH+1], *rest=NULL;
   int short_name, rc;
   FILE *fp=NULL;
   struct AttrRec *attr_ptr=NULL;
   struct SelRec *saved_top_sel=NULL, *saved_bot_sel=NULL;

   if (gstWBInfo.do_whiteboard) {
      /*
       * Now it's allowed.
       *
       * MsgBox(TgLoadString(STID_SETTEMPLATE_IN_WB), TOOL_NAME, INFO_MB);
       * return;
       */
   }
   MakeQuiescent();

   if (importFromLibrary) {
      char name[MAXSTRING+1], path[MAXSTRING+1];

      if (SelectFromLibrary(TgLoadString(STID_SEL_AN_OBJ_FILE_TO_IMPORT),
            OBJ_FILE_EXT, name, path) == INVALID) {
         return;
      }
      sprintf(file_name, "%s%c%s", path, DIR_SEP, name);
   } else {
      sprintf(gszMsgBox, TgLoadString(STID_SELECT_FILE_AS_TEMPLATE));
      if (SelectFileNameToImport(gszMsgBox, OBJ_FILE_EXT, file_name) ==
            INVALID) {
         return;
      } else if (FileIsRemote(file_name)) {
         MsgBox(TgLoadString(STID_NOT_SUP_REMOTE_TEMPLATE_FILE), TOOL_NAME,
               INFO_MB);
         return;
      }
   }
   if (FileIsRemote(file_name)) {
      MsgBox(TgLoadString(STID_NOT_SUP_REMOTE_TEMPLATE_FILE), TOOL_NAME,
            INFO_MB);
      return;
   }
   if ((short_name=IsPrefix(bootDir, file_name, &rest))) ++rest;
   if ((fp=fopen(file_name, "r")) == NULL) {
      sprintf(gszMsgBox, TgLoadString(STID_CANNOT_OPEN_STILL_TEMPLATE),
            (short_name ? rest : file_name));
      if (MsgBox(gszMsgBox, TOOL_NAME, YNC_MB) != MB_ID_YES) {
         return;
      }
   } else {
      fclose(fp);
   }
   StartCompositeCmd();
   importingFile = TRUE;
   rc = ImportGivenFile(file_name, TRUE, TRUE);
   importingFile = FALSE;
   if (rc == BAD) {
      EndCompositeCmd();
      return;
   } else if (rc == FALSE) {
      sprintf(gszMsgBox, TgLoadString(STID_STILL_USE_AS_TEMPLATE),
            (short_name ? rest : file_name));
      if (MsgBox(gszMsgBox, TOOL_NAME, YNC_MB) != MB_ID_YES) {
         EndCompositeCmd();
         return;
      }
   }
   HighLightReverse();
   saved_top_sel = topSel;
   saved_bot_sel = botSel;
   topSel = botSel = NULL;
   UpdSelBBox();

   if ((attr_ptr=FindAttrWithName(tgifObj, "template=", NULL)) != NULL) {
      ReplaceAttrFirstValue(tgifObj, attr_ptr, short_name ? rest : file_name);
   } else {
      int x=tgifObj->obbox.ltx, y=tgifObj->obbox.rby;

      AddObj(NULL, topObj, tgifObj);
      UpdSelBBox();

      PrepareToReplaceAnObj(tgifObj);
      attr_ptr = AddAttrByNameAndValue(tgifObj, "template=",
            short_name ? rest : file_name);
      attr_ptr->shown = TRUE;
      attr_ptr->obj->color = colorIndex;
      if (mainDisplay != NULL) {
         UtilStrCpyN(attr_ptr->obj->color_str, sizeof(attr_ptr->obj->color_str),
               colorMenuItems[colorIndex]);
      }
      MoveObj(attr_ptr->obj, x-attr_ptr->obj->x, y-attr_ptr->obj->y);
      UpdTextBBox(attr_ptr->obj);
      AdjObjBBox(tgifObj);

      recordCmdIncludeTgifObj = TRUE;
      RecordReplaceAnObj(tgifObj);
      recordCmdIncludeTgifObj = FALSE;

      RemoveAllSel();
      UnlinkObj(topObj);
      UpdSelBBox();

      SetFileModified(TRUE);
      justDupped = FALSE;
   }
   EndCompositeCmd();
   sprintf(gszMsgBox, TgLoadString(STID_TEMPLATE_SET_TO),
         (short_name ? rest : file_name));
   Msg(gszMsgBox);
   topSel = saved_top_sel;
   botSel = saved_bot_sel;
   UpdSelBBox();
   HighLightForward();
   SetCurChoice(curChoiceBeforeMakeQuiescent);
}

/* do not translate -- program constants */
char * fileMenuStr[] =
      { "New             ^n",
        "Open            ^o",
        "Save            ^s",
        "SaveNew        ^#s",
        "Import          #p",
        "ImportXBitmap  ^#.",
        "ImportXPixmap  ^#,",
        "ImportEPSFile   #(",
        "ImportGIFFile     ",
        "ImportOtherFile   ",
        "EmbedEPSFile      ",
        "BrowseXBitmap     ",
        "BrowseXPixmap     ",
        "BrowseOther       ",
        "ChangeDomain    ^c",
        "Print           ^p",
        "PrintWithCmd    ^-",
        "PrintSelectedObjs ",
        "PrintOnePage      ",
        "SetExportPixelTrim",
        "InputPolyPts      ",
        "InputPolygonPts   ",
        "SetTemplate       ",
        "Solve           #s",
        "Simulate        #y",
        "Probe           #b",
        "Animate         ^z",
        "Escape          #x",
        "SaveSelectedAs  #~",
        "SaveSymInLibrary  ",
        "Quit            ^q",
        NULL
      };

int QuitProc()
{
   int do_not_save=FALSE;

   while (!DirIsRemote(curDir) && fileModified && !IsFiletUnSavable()) {
      XBell(mainDisplay, 0);
      switch (MsgBox(TgLoadString(STID_FILE_MOD_SAVE_BEFORE_QUIT), TOOL_NAME,
            YNC_MB)) {
      case MB_ID_YES: SaveFile(); break;
      case MB_ID_NO: do_not_save = TRUE; SetFileModified(FALSE); break;
      case MB_ID_CANCEL: return INVALID;
      }
   }
   if (!DirIsRemote(curDir) && fileModified && IsFiletUnSavable()) {
      do_not_save = TRUE;
      SetFileModified(FALSE);
   }
   if (AncesterModified()) {
      switch (MsgBox(TgLoadString(STID_ANCESTER_MOD_STILL_QUIT), TOOL_NAME,
            YNC_MB)) {
      case MB_ID_YES: return FILE_QUIT;
      case MB_ID_NO:
         if (do_not_save) SetFileModified(TRUE);
         return INVALID;
      case MB_ID_CANCEL:
         if (do_not_save) SetFileModified(TRUE);
         return INVALID;
      }
   }
   gnQuit = TRUE;
   return FILE_QUIT;
}

int SolveProc()
{
   if (!saveTmpOnReturn) return FILE_SOLVE;

   switch (SaveTmpFile("tmpmodel")) {
   case OBJ_FILE_TYPE: return FILE_SOLVE;
   case SYM_FILE_TYPE: return INVALID;
   case PIN_FILE_TYPE: return INVALID;
   case INVALID: return INVALID;
   }
   return INVALID;
}

int SimulateProc()
{
   if (!saveTmpOnReturn) return FILE_SIMULATE;

   switch (SaveTmpFile("tmpmodel")) {
   case OBJ_FILE_TYPE: return FILE_SIMULATE;
   case SYM_FILE_TYPE: return INVALID;
   case PIN_FILE_TYPE: return INVALID;
   case INVALID: return INVALID;
   }
   return INVALID;
}

int ProbeProc()
{
   if (!saveTmpOnReturn) return FILE_PROBE;

   switch (SaveTmpFile("tmpmodel")) {
   case OBJ_FILE_TYPE: return FILE_PROBE;
   case SYM_FILE_TYPE: return INVALID;
   case PIN_FILE_TYPE: return INVALID;
   case INVALID: return INVALID;
   }
   return INVALID;
}

int AnimateProc()
{
   if (!saveTmpOnReturn) return FILE_ANIMATE;

   switch (SaveTmpFile("tmpmodel")) {
   case OBJ_FILE_TYPE: return FILE_ANIMATE;
   case SYM_FILE_TYPE: return INVALID;
   case PIN_FILE_TYPE: return INVALID;
   case INVALID: return INVALID;
   }
   return INVALID;
}

int EscapeProc()
{
   return FILE_ESCAPE;
}

int RefreshFileMenu(menu)
   TgMenu *menu;
{
   int ok=TRUE, allow_print_pages=TRUE;

   /* Print/Export In Color */
   ok &= TgSetMenuItemCheckById(menu, CMDID_TOGGLEBWCOLORPS, colorDump);

   /* Print With Cmd */
   ok &= TgEnableMenuItemById(menu, CMDID_PRINTWITHCMD,
         (whereToPrint==PRINTER));

   /*
    * Print Pages - only allows PRINTER, PS_FILE, PDF_FILE, or user supplied
    *       export filters
    */
   if (pageLayoutMode == PAGE_TILE) {
      allow_print_pages = FALSE;
   } else {
      switch (whereToPrint) {
      case LATEX_FIG:
      case XBM_FILE:
      case TEXT_FILE:
      case NETLIST_FILE:
      case EPSI_FILE:
      case GIF_FILE:
      case HTML_FILE:
      case TIFFEPSI_FILE:
      case PNG_FILE:
      case JPEG_FILE:
      case PPM_FILE:
      case SVG_FILE:
         allow_print_pages = FALSE;
         break;
      default: break;
      }
   }
   ok &= TgEnableMenuItemById(menu, CMDID_PRINTPAGES, allow_print_pages);

   /* Print One File Per Page */
   ok &= TgEnableMenuItemById(menu, CMDID_PRINTONEFILEPERPAGE,
         (pageLayoutMode == PAGE_STACK && whereToPrint != PRINTER));

   return ok;
}

TgMenu *CreateFileMenu(parent_menu, x, y, menu_info, status_str_xlated)
   TgMenu *parent_menu;
   int x, y;
   TgMenuInfo *menu_info;
   int status_str_xlated; /* ignored, always 0 */
{
   TgMenu *menu=TgCreateMenuFromMenuInfo(parent_menu, x, y, menu_info, FALSE);

   if (menu != NULL) {
      if (!RefreshFileMenu(menu)) {
         return TgDestroyMenu(menu, TRUE);
      }
      menu->refresh_proc = ((RefreshMenuFunc*)RefreshFileMenu);
   }
   return menu;
}

int FileMenu(X, Y, TrackMenubar)
   int X, Y, TrackMenubar;
{
   int rc=INVALID;
   TgMenu *menu=(fileMenuInfo.create_proc)(NULL, X, Y, &fileMenuInfo, INVALID);

   activeMenu = MENU_FILE;
   if (menu != NULL) {
      menu->track_menubar = TrackMenubar;

      rc = TgMenuLoop(menu);
      TgDestroyMenu(menu, TRUE);
   }
   return rc;
}

TgMenu *CreateRecentFilesMenu(parent_menu, x, y, menu_info, status_str_xlated)
   TgMenu *parent_menu;
   int x, y;
   TgMenuInfo *menu_info;
   int status_str_xlated; /* ignored, always 0 */
{
   int i=0, count=0;
   TgMenu *menu=NULL;
   TgMenuInfo stMenuInfo;
   TgMenuItemInfo *item_info=NULL;
   KeyValInfo *pkvi=NULL;

   memcpy(&stMenuInfo, menu_info, sizeof(TgMenuInfo));
   pkvi = RecentFilesListing(&count);

   if (pkvi == NULL || count == 0) {
      count = 0;
      stMenuInfo.items = (TgMenuItemInfo*)malloc((1+1)*sizeof(TgMenuItemInfo));
      if (stMenuInfo.items == NULL) FailAllocMessage();
      memset(stMenuInfo.items, 0, (1+1)*sizeof(TgMenuItemInfo));

      item_info = stMenuInfo.items;
      item_info->menu_str = UtilStrDup(TgLoadCachedString(CSTID_PARANED_EMPTY));
      if (item_info->menu_str == NULL) FailAllocMessage();
      item_info->shortcut_str = NULL;
      item_info->status_str = UtilStrDup("");
      if (item_info->status_str == NULL) FailAllocMessage();
      item_info->submenu_info = NULL;
      item_info->cmdid = CMDID_RECENTLYUSEDFILES;
      stMenuInfo.items[1].cmdid = INVALID;
   } else {
      stMenuInfo.items = (TgMenuItemInfo*)malloc(
            (count+1)*sizeof(TgMenuItemInfo));
      if (stMenuInfo.items == NULL) FailAllocMessage();
      memset(stMenuInfo.items, 0, (count+1)*sizeof(TgMenuItemInfo));

      for (item_info=stMenuInfo.items, i=0; i < count; item_info++, i++) {
         sprintf(gszMsgBox, "%2d.  %s", i+1, pkvi[i].key);
         item_info->menu_str = UtilStrDup(gszMsgBox);
         if (item_info->menu_str == NULL) FailAllocMessage();
         item_info->shortcut_str = NULL;
         sprintf(gszMsgBox, TgLoadCachedString(CSTID_OPEN_FILE),
               pkvi[i].value);
         item_info->status_str = UtilStrDup(gszMsgBox);
         if (item_info->status_str == NULL) FailAllocMessage();
         item_info->submenu_info = NULL;
         item_info->cmdid = CMDID_RECENTLYUSEDFILES;
      }
      stMenuInfo.items[count].cmdid = INVALID;
   }
   /* the status_str has actual file names, no tralslation needed */
   menu = TgCreateMenuFromMenuInfo(parent_menu, x, y, &stMenuInfo, TRUE);
   if (count == 0) {
      UtilFree(stMenuInfo.items->status_str);
      memset(stMenuInfo.items, 0, (1+1)*sizeof(TgMenuItemInfo));
   } else {
      for (item_info=stMenuInfo.items, i=0; i < count; item_info++, i++) {
         UtilFree(item_info->status_str);
      }
      memset(stMenuInfo.items, 0, (count+1)*sizeof(TgMenuItemInfo));
   }
   free(stMenuInfo.items);
   stMenuInfo.items = NULL;
   if (menu != NULL) {
      if (count == 0) {
         TgEnableMenuItemByIndex(menu, 0, FALSE);
         menu->menuitems[0].menu_str_allocated = TRUE;
      } else {
         for (i=0; i < count; i++) {
            menu->menuitems[i].menu_str_allocated = TRUE;
         }
      }
   }
   FreeRecentFilesListing(pkvi, count);

   return menu;
}

char *ReadFileIntoBuf(fname, pn_buf_sz)
   char *fname;
   unsigned int *pn_buf_sz;
{
   struct stat stat_buf;

   if (stat(fname, &stat_buf) == 0) {
      char *psz=NULL, buf[0x10000], *psz_return=NULL;
      FILE *fp=NULL;
      unsigned int bytes_to_read=0, file_size=0, bytes_read=0, total_read=0;

      if ((fp=fopen(fname, "r")) == NULL) {
         FailToOpenMessage(fname, "r", NULL);
         return NULL;
      }
      file_size = (unsigned int)(stat_buf.st_size);
      psz = psz_return = (char*)malloc(file_size);
      if (psz_return == NULL) FailAllocMessage();

      bytes_to_read = min(file_size, sizeof(buf));
      while ((bytes_read=fread(psz, sizeof(char), bytes_to_read, fp)) > 0) {
         total_read += bytes_read;
         psz += bytes_read;
         file_size -= bytes_read;
         bytes_to_read = min(file_size, sizeof(buf));
         if (bytes_to_read == 0) break;
      }
      fclose(fp);
      if (total_read == (unsigned int)(stat_buf.st_size)) {
         if (pn_buf_sz != NULL) *pn_buf_sz = total_read;
      }
      return psz_return;
   }
   return NULL;
}

int WriteBufIntoTmpFile(buf, buf_sz, tmp_fname, tmp_fname_sz)
   char *buf, *tmp_fname;
   int buf_sz, tmp_fname_sz;
{
   FILE *fp=NULL;

   if (MkTempFile(tmp_fname, tmp_fname_sz, tmpDir, TOOL_NAME) == NULL) {
      return FALSE;
   }
   if ((fp=fopen(tmp_fname, "w")) == NULL) {
      return FailToOpenMessage(tmp_fname, "w", NULL);
   }
   if ((int)fwrite(buf, sizeof(char), buf_sz, fp) != buf_sz) {
      fclose(fp);
      return FailToWriteFileMessage(tmp_fname);
   }
   fclose(fp);

   return TRUE;
}

void CleanUpFiles()
{
   UtilFree(gpszPsSetup);
   gpszPsSetup = NULL;

   ClearBgColorInfo(FALSE);
   ClearBgPixmapInfo(FALSE);

   CleanUpDumpInitDefaults();
   ClearFileInfo(TRUE);
   if (usePaperSizeStoredInFile) ResetOnePageSize();
   fileModified = FALSE;
}
