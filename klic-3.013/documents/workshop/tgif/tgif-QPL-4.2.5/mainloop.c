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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/mainloop.c,v 1.37 2011/05/16 16:21:58 william Exp $
 */

#define _INCLUDE_FROM_MAINLOOP_C_

#include "tgifdefs.h"
#include "expfdefs.h"
#include "patchlvl.h"
#include "cmdids.h"

#ifdef USE_XT_INITIALIZE
#include <X11/Intrinsic.h>
#endif /* USE_XT_INITIALIZE */

#include "animate.e"
#include "attr.e"
#include "auxtext.e"
#include "chat.e"
#include "choice.e"
#include "cmd.e"
#include "color.e"
#include "cursor.e"
#include "cutpaste.e"
#include "dialog.e"
#include "drawing.e"
#include "edit.e"
#include "eps.e"
#include "exec.e"
#include "expr.e"
#include "file.e"
#include "font.e"
#include "ftp.e"
#include "grid.e"
#include "help.e"
#include "http.e"
#include "imgproc.e"
#include "import.e"
#include "ini.e"
#include "inmethod.e"
#include "mainloop.e"
#include "mainmenu.e"
#include "mark.e"
#include "markup.e"
#include "menu.e"
#include "miniline.e"
#include "msg.e"
#include "names.e"
#include "navigate.e"
#include "obj.e"
#include "page.e"
#include "pin.e"
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
#include "stk.e"
#include "strtbl.e"
#include "tangram2.e"
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

#ifdef USE_XT_INITIALIZE
Widget		toplevel=(Widget)0;
#endif /* USE_XT_INITIALIZE */

int	x11R6OrAbove=FALSE;
int	origArgC=0;
char	**origArgV=NULL;

int	cmdLineBW=FALSE;
int	cmdLineRV=INVALID;
int	cmdLineCWO=FALSE;
int	cmdLineBtn1Warp=FALSE;
char	*cmdLineForeground=NULL;
char	*cmdLineBackground=NULL;
char	*cmdLineBorder=NULL;
int	cmdLineMerge=FALSE;
int	cmdLineMergeArgc=0;
char	**cmdLineMergeArgv=NULL;
int	geometrySpecified=FALSE;
int	exitNormally=FALSE;
char	geometrySpec[80];
char	*displayName=NULL;
int	initializingMain=FALSE;
int	cmdLineUseXlib=FALSE;
int	cmdLineVersion=FALSE;
int	cmdLineNoModeWindow=FALSE;
int	cmdLineNoChatWindow=FALSE;

KeyOrBtnEvInfo	lastKeyOrBtnEvInfo;

int			numExtraWins=0;
struct WinInfoRec	*extraWinInfo=NULL;

PageInfo	mergingPageInfo;

static int	maxExtraWins=0;
static int	quitDraw=TRUE;

static int	useXlib=FALSE;

int AddExtraWinInfo(win, mapped, raise, expose_handler, ev_handler,
      cleanup_routine, userdata)
   Window win;
   int mapped, raise;
   void (*expose_handler)();
   int (*ev_handler)();
   void (*cleanup_routine)();
   void *userdata;
{
   register int i;

   for (i = 0; i < numExtraWins; i++) {
      if (extraWinInfo[i].window == None) {
         break;
      }
   }
   if (i == numExtraWins && numExtraWins == maxExtraWins) {
      maxExtraWins += 10;
      extraWinInfo = (struct WinInfoRec *)realloc(extraWinInfo,
            maxExtraWins*sizeof(struct WinInfoRec));
   }
   extraWinInfo[i].window = win;
   extraWinInfo[i].mapped = mapped;
   extraWinInfo[i].raise = raise;
   extraWinInfo[i].expose_handler = expose_handler;
   extraWinInfo[i].ev_handler = ev_handler;
   extraWinInfo[i].cleanup = cleanup_routine;
   extraWinInfo[i].userdata = userdata;
   return ((i==numExtraWins) ? numExtraWins++ : i);
}

static
void InitExtraWinInfo()
{
   extraWinInfo = (struct WinInfoRec *)malloc(10*sizeof(struct WinInfoRec));
   if (extraWinInfo == NULL) FailAllocMessage();
   memset(extraWinInfo, 0, 10*sizeof(struct WinInfoRec));
   maxExtraWins += 10;
}

static
void CleanUpExtraWinInfo()
{
   register int i;

   if (extraWinInfo != NULL) {
      for (i = 0; i < numExtraWins; i++) {
         if (extraWinInfo[i].window != None) {
            (*(extraWinInfo[i].cleanup))(&extraWinInfo[i]);
         }
      }
      free(extraWinInfo);
   }
   numExtraWins = 0;
   maxExtraWins = 0;
   extraWinInfo = NULL;
}

static
void InitLastKeyOrBtnEvInfo()
{
   memset(&lastKeyOrBtnEvInfo, 0, sizeof(KeyOrBtnEvInfo));
}

void CleanUpTalkToSelfFiledes()
{
   if (talkToSelfFiledesInitialized) {
       if (talkToSelfFiledes[0] != (-1)) close(talkToSelfFiledes[0]);
       if (talkToSelfFiledes[1] != (-1)) close(talkToSelfFiledes[1]);
   }
   talkToSelfFiledesInitialized = FALSE;
   talkToSelfFiledes[0] = talkToSelfFiledes[1] = (-1);
}

int InitTalkToSelfFiledes()
{
   talkToSelfFiledesInitialized = FALSE;
   talkToSelfFiledes[0] = talkToSelfFiledes[1] = (-1);

   if (pipe(talkToSelfFiledes) != 0) {
      sprintf(gszMsgBox, TgLoadString(STID_CREATE_SELF_PIPE_FAIL), TOOL_NAME);
      fprintf(stderr, "%s\n", gszMsgBox);
      return FALSE;
   }
   talkToSelfFiledesInitialized = TRUE;

   return TRUE;
}

static
char *MySetLocale(category, new_locale)
   int category;
   char *new_locale;
{
#ifndef _NO_LOCALE_SUPPORT
   return setlocale(category, new_locale);
#else /* _NO_LOCALE_SUPPORT */
   return NULL;
#endif /* ~_NO_LOCALE_SUPPORT */
}

static
void ExecWithFile(CmdName, FileName)
   char *CmdName, *FileName;
{
   char s[255];

#ifdef _BACKGROUND_DONT_FORK
   /* do not translate -- program constants */
   sprintf(s, "xterm -bd red -e %s %s", CmdName, FileName);
   strcat(s, " &");
   (void)system(s);
#else /* ~_BACKGROUND_DONT_FORK */
   int pid;

   /* do not translate -- program constants */
   sprintf(s, "xterm -bd red -e %s %s", CmdName, FileName);
   pid = fork();
   if (pid == 0) {
      (void)system(s);
      exit(0);
   }
#endif /* _BACKGROUND_DONT_FORK */
}

void DeallocStrings(FStr, Str1, Menu1, Str2, Menu2, Str3, Menu3)
   char **FStr, **Str1, **Menu1, **Str2, **Menu2, **Str3, **Menu3;
{
   free(*FStr);
   free(*Str1);
   free(*Menu1);
   free(*Str2);
   free(*Menu2);
   free(*Str3);
   free(*Menu3);
}

static
void AllocStrings(FStr, Str1, Menu1, Str2, Menu2, Str3, Menu3)
   char **FStr, **Str1, **Menu1, **Str2, **Menu2, **Str3, **Menu3;
{
   char *s;

   if((s=(char*)malloc(80*sizeof(char))) == NULL) FailAllocMessage();
   *FStr = s; **FStr = '\0';
   if((s=(char*)malloc(80*sizeof(char))) == NULL) FailAllocMessage();
   *Str1 = s; **Str1 = '\0';
   if((s=(char*)malloc(80*sizeof(char))) == NULL) FailAllocMessage();
   *Menu1 = s; **Menu1 = '\0';
   if((s=(char*)malloc(80*sizeof(char))) == NULL) FailAllocMessage();
   *Str2 = s; **Str2 = '\0';
   if((s=(char*)malloc(80*sizeof(char))) == NULL) FailAllocMessage();
   *Menu2 = s; **Menu2 = '\0';
   if((s=(char*)malloc(80*sizeof(char))) == NULL) FailAllocMessage();
   *Str3 = s; **Str3 = '\0';
   if((s=(char*)malloc(80*sizeof(char))) == NULL) FailAllocMessage();
   *Menu3 = s; **Menu3 = '\0';
}

/* 
 * static
 * int MyErrorHandler(display, event)
 *    Display *display;
 *    XErrorEvent *event;
 * {
 *    if (event->type == 0) return TRUE;
 *    printf("\tError: type -- %1d\n", event->type);
 *    exit(-1);
 * }
 */

static
void CleanUpMergeArgs()
{
   if (cmdLineMergeArgv != NULL) {
      int i=0;

      for (i=0; i < cmdLineMergeArgc; i++) {
         UtilFree(cmdLineMergeArgv[i]);
      }
      free(cmdLineMergeArgv);
   }
   cmdLineMergeArgc = 0;
   cmdLineMergeArgv = NULL;
}

void CleanUp()
{
   tgCleanUpProfile();
   CleanUpChat();
   CleanUpWhiteBoard();

   TieLooseEnds();

   if (cmdLineTgrm2) {
      CleanUpTangram2();
   }
   CleanUpMiniLines();
   CleanUpCutPaste();
   CleanUpProperties();
   CleanUpExportFilters();
   ResetPSInfo();
   CleanUpEdit();
   CleanUpMark();
   CleanUpShape();
   CleanUpHelp();
   CleanUpImport();
   CleanUpImageProc();
   CleanUpExec();
   CleanUpCmds();
   CleanUpExpr();
   CleanUpNavigate();
   CleanUpRemote();

   CleanUpExtraWinInfo();
   CleanUpDrawingWindow();
   CleanUpPS();
   CleanUpInputMethods();
   CleanUpPins();
   CleanUpPage();
   CleanUpStk();
   CleanUpStatus();
   CleanUpChoices();

   CleanUpScrolls();
   CleanUpCursors();

   CleanUpNames();
   CleanUpRuler();
   CleanUpRasters();
   CleanUpFonts();
   CleanUpMenu();
   CleanUpText();
   CleanUpColors();
   CleanUpFiles();
   CleanUpGrids();
   CleanUpCutBuffer();
   CleanUpXBm();
   CleanUpXPm();
   CleanUpMsg();
   CleanUpShortCut();
   CleanUpHtml();
   CleanUpRecentFiles();
   CleanUpMeasureTooltip();
   CleanUpSpecial();
   CleanUpAttr();
   CleanUpTalkToSelfFiledes();
   CleanUpInsertSymbol();
   CleanUpColorWheel();
   CleanUpTidget();

   if (iconWindowCreated) {
      XDestroyWindow(mainDisplay, iconBaseWindow);
      iconWindowCreated = FALSE;
   }
   CleanUpResiduals();
   CleanUpStrTable();

   XDestroyWindow(mainDisplay, mainWindow);
   mainWindow = None;
#ifndef DONT_FREE_COLORMAP
   if (newColormapUsed) {
      XFreeColormap(mainDisplay, mainColormap);
      newColormapUsed = FALSE;
   }
#endif /* ~DONT_FREE_COLORMAP */
}

void ExposeEventHandler(input, recursive)
   XEvent *input;
   int recursive;
{
   int i=0;
   XEvent ev;
   TidgetInfo *handling_pti=NULL;

   if (input->xany.window == choiceWindow) {
      while (XCheckWindowEvent(mainDisplay, choiceWindow, ExposureMask, &ev)) ;
      RedrawChoiceWindow();
   } else if (input->xany.window == drawWindow) {
      while (XCheckWindowEvent(mainDisplay, drawWindow, ExposureMask, &ev)) ;
      if (topSel != NULL || curChoice==VERTEXMODE || SomethingDirty() ||
            showCrossHair) {
         ClearAndRedrawDrawWindow();
      } else {
         RedrawDrawWindow(botObj);
         RedrawCurText();
      }
   } else if (input->xany.window == vRuleWindow) {
      while (XCheckWindowEvent(mainDisplay, vRuleWindow, ExposureMask, &ev)) ;
      RedrawVRulerWindow();
   } else if (input->xany.window == hRuleWindow) {
      while (XCheckWindowEvent(mainDisplay, hRuleWindow, ExposureMask, &ev)) ;
      RedrawHRulerWindow();
   } else if (input->xany.window == iconWindow && input->type == Expose) {
      while (XCheckWindowEvent(mainDisplay, iconWindow, ExposureMask, &ev)) ;
      RedrawIconWindow();
   } else if (input->xany.window == titleWindow) {
      while (XCheckWindowEvent(mainDisplay, titleWindow, ExposureMask, &ev)) ;
      RedrawTitleWindow();
   } else if (input->xany.window == menubarWindow) {
      while (XCheckWindowEvent(mainDisplay,menubarWindow,ExposureMask,&ev)) ;
      RedrawMenubarWindow();
   } else if (input->xany.window == msgWindow) {
      while (XCheckWindowEvent(mainDisplay, msgWindow, ExposureMask, &ev)) ;
      RedrawMsg(TRUE);
   } else if (input->xany.window == vSBarWindow) {
      while (XCheckWindowEvent(mainDisplay, vSBarWindow, ExposureMask, &ev)) ;
      RedrawVScrollWindow();
   } else if (input->xany.window == hSBarWindow) {
      while (XCheckWindowEvent(mainDisplay, hSBarWindow, ExposureMask, &ev)) ;
      RedrawHScrollWindow();
   } else if (input->xany.window == userRedrawWindow) {
      while (XCheckWindowEvent(mainDisplay,userRedrawWindow,ExposureMask,&ev)) ;
      RedrawUserRedrawWindow();
   } else if (input->xany.window == statusWindow) {
      while (XCheckWindowEvent(mainDisplay, statusWindow, ExposureMask, &ev)) ;
      RedrawStatusWindow();
   } else if (IsChatWindowEvent(input, &handling_pti)) {
      ChatEventHandler(input, handling_pti);
   } else if (input->xany.window == modeWindow) {
      while (XCheckWindowEvent(mainDisplay, modeWindow, ExposureMask, &ev)) ;
      RedrawModeWindow();
   } else if (input->xany.window == dummyWindow1) {
      while (XCheckWindowEvent(mainDisplay, dummyWindow1, ExposureMask, &ev)) ;
      RedrawDummyWindow1();
   } else if (input->xany.window == pageWindow) {
      RedrawPageWindow();
   } else if (input->xany.window == pageDummyWindow) {
      RedrawPageDummyWindow();
   } else if (input->xany.window == colorWindow) {
      RedrawColorWindow();
   } else if (input->xany.window == colorDummyWindow) {
      RedrawColorDummyWindow();
   } else if (input->xany.window==mainWindow && input->type==VisibilityNotify &&
         input->xvisibility.state==VisibilityUnobscured) {
      XEvent tmp_ev;

      while (XCheckWindowEvent(mainDisplay, mainWindow, VisibilityChangeMask,
            &tmp_ev)) ;
      if (iconWindowShown) {
         UnIconify();
      } else {
         if (pinnedMainMenu) XMapRaised(mainDisplay, mainMenuWindow);
         for (i = 0; i < numExtraWins; i++) {
            if (extraWinInfo[i].window != None && extraWinInfo[i].mapped &&
                  extraWinInfo[i].raise) {
               XMapRaised(mainDisplay, extraWinInfo[i].window);
            }
         }
      }
   } else if (TidgetManagerHandleEvent(input)) {
      /* input has been handled by the TidgetManager */
   } else {
      for (i = 0; i < numExtraWins; i++) {
         if (input->xany.window == extraWinInfo[i].window &&
               extraWinInfo[i].window != None) {
            while (XCheckWindowEvent(mainDisplay, extraWinInfo[i].window,
                  ExposureMask, &ev)) ;
            (*(extraWinInfo[i].expose_handler))(input, &extraWinInfo[i]);
            break;
         }
      }
   }
   if (recursive) {
      while (XCheckMaskEvent(mainDisplay, ExposureMask, &ev) ||
            XCheckMaskEvent(mainDisplay, VisibilityChangeMask, &ev)) {
         ExposeEventHandler(&ev, FALSE);
      }
   }
}

int GzipFile(fname, gzipped_fname)
   char *fname, *gzipped_fname;
{
   int rc=TRUE;
   FILE *pfp=NULL, *out_fp=NULL;
   char *compress_cmd=NULL, *cmd=NULL;

   if ((out_fp=fopen(gzipped_fname, "w")) == NULL) {
      return FailToOpenMessage(gzipped_fname, "w", NULL);
   }
   SaveStatusStrings();
   compress_cmd = gzipCmd;

   cmd = (char*)malloc((strlen(fname)+strlen(compress_cmd)+20) * sizeof(char));
   if (cmd == NULL) FailAllocMessage();

   if (strstr(compress_cmd, "%s") == NULL) {
      sprintf(cmd, "%s %s", compress_cmd, fname);
   } else {
      sprintf(cmd, compress_cmd, fname);
   }
   if (!FindProgramInPath(cmd, NULL, FALSE)) {
      fclose(out_fp);
      free(cmd);
      RestoreStatusStrings();
      return FALSE;
   }
   sprintf(gszMsgBox, TgLoadCachedString(CSTID_EXECUTING_GIVEN_PROGRAM),
         gzipCmd);
   SetStringStatus(gszMsgBox);

   if ((pfp=(FILE*)popen(cmd, "r")) == NULL) {
      sprintf(gszMsgBox, TgLoadString(STID_FAIL_TO_EXECUTE_CMD), cmd);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      fclose(out_fp);
      rc = FALSE;
   } else {
      int bytes_read=0, watch_cursor=watchCursorOnMainWindow;

      if (!watch_cursor) {
         SetWatchCursor(drawWindow);
         SetWatchCursor(mainWindow);
      }
      writeFileFailed = FALSE;
      while ((bytes_read=fread(gszMsgBox, sizeof(char), sizeof(gszMsgBox),
            pfp)) > 0) {
         if ((int)fwrite(gszMsgBox, sizeof(char), bytes_read, out_fp) <= 0) {
            writeFileFailed = TRUE;
            break;
         }
      }
      pclose(pfp);
      if (!watch_cursor) {
         SetDefaultCursor(mainWindow);
         ShowCursor();
      }
      fclose(out_fp);
      if (writeFileFailed) {
         FailToWriteFileMessage(gzipped_fname);
         rc = FALSE;
      }
   }
   RestoreStatusStrings();
   return rc;
}

char *GunzipFileIntoTemp(gzipped_fname)
   char *gzipped_fname;
{
   FILE *pfp=NULL, *out_fp=NULL;
   char *uncompress_cmd=gunzipCmd, *tmp_fname=NULL, *cmd=NULL;

   tmp_fname = (char*)malloc((strlen(tmpDir)+20)*sizeof(char));
   if (tmp_fname == NULL) {
      FailAllocMessage();
      return NULL;
   }
   if (MkTempFile(tmp_fname, (strlen(tmpDir)+19)*sizeof(char), tmpDir,
         TOOL_NAME) == NULL) {
      return NULL;
   }
   if ((out_fp=fopen(tmp_fname, "w")) == NULL) {
      sprintf(gszMsgBox, TgLoadString(STID_CANNOT_OPEN_FILE_FOR_WRITING),
            tmp_fname);
      if (PRTGIF) {
         fprintf(stderr, "%s\n", gszMsgBox);
      } else {
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      }
      return NULL;
   }
   SaveStatusStrings();
   uncompress_cmd = gunzipCmd;

   cmd = (char*)malloc((strlen(gzipped_fname)+strlen(uncompress_cmd)+20) *
         sizeof(char));
   if (cmd == NULL) FailAllocMessage();

   if (strstr(uncompress_cmd, "%s") == NULL) {
      sprintf(cmd, "%s %s", uncompress_cmd, gzipped_fname);
   } else {
      sprintf(cmd, uncompress_cmd, gzipped_fname);
   }
   if (!FindProgramInPath(cmd, NULL, FALSE)) {
      fclose(out_fp);
      unlink(tmp_fname);
      free(cmd);
      RestoreStatusStrings();
      return NULL;
   }
   sprintf(gszMsgBox, TgLoadCachedString(CSTID_EXECUTING_GIVEN_PROGRAM),
         gunzipCmd);
   if (PRTGIF) {
      fprintf(stderr, "%s\n", gszMsgBox);
   } else {
      SetStringStatus(gszMsgBox);
   }
   if ((pfp=(FILE*)popen(cmd, "r")) == NULL) {
      sprintf(gszMsgBox, TgLoadString(STID_FAIL_TO_EXECUTE_CMD), cmd);
      if (PRTGIF) {
         fprintf(stderr, "%s\n", gszMsgBox);
      } else {
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      }
      fclose(out_fp);
      unlink(tmp_fname);
      tmp_fname = NULL;
   } else {
      int bytes_read=0, watch_cursor=watchCursorOnMainWindow;

      if (!watch_cursor) {
         SetWatchCursor(drawWindow);
         SetWatchCursor(mainWindow);
      }
      writeFileFailed = FALSE;
      while ((bytes_read=fread(gszMsgBox, sizeof(char), sizeof(gszMsgBox),
            pfp)) > 0) {
         if ((int)fwrite(gszMsgBox, sizeof(char), bytes_read, out_fp) <= 0) {
            writeFileFailed = TRUE;
            break;
         }
      }
      pclose(pfp);
      if (!watch_cursor) {
         SetDefaultCursor(mainWindow);
         ShowCursor();
      }
      fclose(out_fp);
      if (writeFileFailed) {
         FailToWriteFileMessage(tmp_fname);
         unlink(tmp_fname);
         tmp_fname = NULL;
      }
   }
   RestoreStatusStrings();
   return tmp_fname;
}

#ifdef USE_XAPPLRESDIR
static
int FindXApplResDir(psz_srch_path, psz_target_dir, cb_target_dir)
   char *psz_srch_path, *psz_target_dir;
   int cb_target_dir;
{
   char *c_ptr, *buf=UtilStrDup(psz_srch_path);

   if (buf == NULL) return FALSE;

   for (c_ptr=strtok(buf, ":"); c_ptr != NULL; c_ptr=strtok(NULL, ":")) {
      int len=strlen(c_ptr);
      char path[MAXPATHLENGTH+1];
      FILE *fp;

      if (len > 0 && c_ptr[len-1] == DIR_SEP) c_ptr[--len] = DIR_SEP;
      sprintf(path, "%s%c%s", c_ptr, DIR_SEP, TOOL_NAME);
      if ((fp=fopen(path, "r")) != NULL) {
         UtilStrCpyN(psz_target_dir, cb_target_dir, c_ptr);
         UtilFree(buf);
         return TRUE;
      }
   }
   UtilFree(buf);
   return FALSE;
}
#endif /* USE_XAPPLRESDIR */

#ifndef _NO_LOCALE_SUPPORT
#ifdef ENABLE_NLS
static
void InitLocale()
{
   char *c_ptr=NULL;

   c_ptr = MySetLocale(LC_ALL, "");
   if (c_ptr == NULL) {
      fprintf(stderr, "%s\n", TgLoadString(STID_CANNOT_SET_LOCALE_ENV_VARS));
   }
   if (x11R6OrAbove) {
      if (useXlib) {
         if (!XSupportsLocale()) {
            fprintf(stderr, "%s\n",
                  TgLoadString(STID_XLIB_CANT_SET_LOCALE_ENV_VARS));
            MySetLocale(LC_ALL, "C");
         }
         if (!XSetLocaleModifiers("")) {
            if (getenv("LANG") != NULL) {
               fprintf(stderr, "%s\n",
                     TgLoadString(STID_CANT_SET_LOCALE_MODIFIERS));
            }
         }
      } else {
#ifdef USE_XT_INITIALIZE
         if (XtSetLanguageProc(NULL, NULL, NULL) == NULL) {
            fprintf(stderr, "%s\n",
                  TgLoadString(STID_XT_CANNOT_SET_LOCALE_ENV_VARS));
            MySetLocale(LC_ALL, "C");
         }
#endif /* USE_XT_INITIALIZE */
      }
   }
   MySetLocale(LC_ALL, NULL);

   /*
    * Some locale uses "," as decimal point.  That won't work in
    *       saving files or generating EPS files.  Must use "."
    *       as decimal point.
    * From this point on, setlocale() should NEVER be called again!
    */
   setlocale(LC_NUMERIC, "C");
}

static
void InitLocale2()
{
   DIR *dirp=NULL;
   char package[80], *c_ptr=NULL, locale_dir[MAXPATHLENGTH];

   *locale_dir = '\0';
   if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"LocaleDir")) != NULL) {
      strcpy(locale_dir, c_ptr);
   } else {
      strcpy(locale_dir, LOCALEDIR);
   }
   UtilTrimBlanks(locale_dir);

   strcpy(package, TOOL_NAME);
   UtilStrLower(package);

   if ((dirp=opendir(locale_dir)) == NULL) {
      fprintf(stderr, TgLoadString(STID_LOCALE_DIR_NOT_EXIST_USE_XDEF),
            locale_dir, TOOL_NAME, "LocaleDir");
      fprintf(stderr, "\n");
   } else {
      closedir(dirp);
      bindtextdomain(package, locale_dir);
      textdomain(package);
   }
}
#endif /* ENABLE_NLS */
#endif /* ~_NO_LOCALE_SUPPORT */

static
int JustLoadAFile(pszFile, pnNeedToCheckAutoExec, nAttemptToMerge, pnAbortMerge)
   char *pszFile;
   int *pnNeedToCheckAutoExec, nAttemptToMerge, *pnAbortMerge;
   /*
    * return FALSE if nAttemptToMerge is TRUE and pszFile is in
    *       TILED page mode
    * return TRUE otherwise
    */
{
   int short_name=FALSE, obj_file=TRUE;
   int tmp_linenum=0;
   char tmp_filename[MAXPATHLENGTH+1], tmp_filefullpath[MAXPATHLENGTH+1];
   char full_name[MAXPATHLENGTH+1], *rest=NULL;
   char file_name[MAXPATHLENGTH+1], gzipped_fname[MAXPATHLENGTH+1];
   int rc=0, gzipped_obj_file=FALSE;
   int file_is_remote=FALSE, remote_buf_sz=0, is_html=FALSE;
   char remote_fname[MAXPATHLENGTH+1], *tmp_remote_fname=NULL;
   char *remote_buf=NULL, *page_spec=NULL;
   struct ObjRec *obj_ptr=NULL;
   FILE *fp=NULL;

   *remote_fname = *gzipped_fname = '\0';

   if (FileIsRemote(pszFile)) {
      if (!FormNewFileName(curDir, pszFile, NULL, remote_fname, &page_spec)) {
         sprintf(gszMsgBox, TgLoadString(STID_INVALID_REMOTE_FNAME), pszFile);
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
         *pszFile = '\0';
      } else {
         char *content_type=NULL;
         char final_url[MAXPATHLENGTH+1];

         *final_url = '\0';
         SaveStatusStrings();
         rc = LoadRemoteFileInMem(remote_fname, &remote_buf,
               &content_type, &remote_buf_sz, &is_html, TRUE,
               final_url, sizeof(final_url));
         RestoreStatusStrings();
         if (rc && remote_buf != NULL &&
               (tmp_remote_fname=WriteRemoteFileIntoTemp(remote_buf,
               remote_buf_sz, NULL)) != NULL) {
            if (*final_url != '\0') {
               UtilStrCpyN(pszFile, MAXPATHLENGTH+1, final_url);
               UtilStrCpyN(remote_fname, MAXPATHLENGTH+1, final_url);
            }
            if (FileNameHasExtension(remote_fname, OBJ_FILE_TYPE,
                  &gzipped_obj_file, NULL)) {
               file_is_remote = TRUE;
            } else if (UseExternalViewer(is_html, remote_fname, content_type,
                  tmp_remote_fname) != FALSE) {
               unlink(tmp_remote_fname);
               FreeRemoteBuf(tmp_remote_fname);
               if (remote_buf != NULL) FreeRemoteBuf(remote_buf);
               remote_buf = tmp_remote_fname = NULL;
               file_is_remote = FALSE;
               *pszFile = '\0';
            } else {
               file_is_remote = TRUE;
            }
         } else {
            if (remote_buf != NULL) FreeRemoteBuf(remote_buf);
            remote_buf = NULL;
            *pszFile = '\0';
         }
         if (content_type != NULL) FreeRemoteBuf(content_type);
      }
   }
   *file_name = '\0';
   if (file_is_remote) {
      if ((fp=fopen(tmp_remote_fname, "r")) == NULL) {
         sprintf(gszMsgBox, TgLoadString(STID_CANNOT_READ_TMP_FILE),
               tmp_remote_fname);
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);

         unlink(tmp_remote_fname);
         FreeRemoteBuf(tmp_remote_fname);
         FreeRemoteBuf(remote_buf);
         remote_buf = tmp_remote_fname = NULL;
         file_is_remote = FALSE;
      } else {
      if (FileNameHasExtension(remote_fname, SYM_FILE_TYPE, NULL, NULL)) {
            obj_file = FALSE;
         } else if (FileNameHasExtension(remote_fname, PIN_FILE_TYPE,
               NULL, NULL)) {
            obj_file = FALSE;
         } else {
            obj_file = TRUE;
         }
      }
   } else {
      if (FileNameHasExtension(pszFile, OBJ_FILE_TYPE, &gzipped_obj_file,
            NULL)) {
         if (gzipped_obj_file) {
            char *tmp_fname=NULL;

            if ((tmp_fname=GunzipFileIntoTemp(pszFile)) == NULL) {
               *file_name = '\0';
            } else {
               strcpy(gzipped_fname, pszFile);
               strcpy(file_name, tmp_fname);
               free(tmp_fname);
            }
         } else {
            strcpy(file_name, pszFile);
         }
      } else if (FileNameHasExtension(pszFile, SYM_FILE_TYPE, NULL, NULL)) {
         strcpy(file_name, pszFile);
         obj_file = FALSE;
      } else if (FileNameHasExtension(pszFile, PIN_FILE_TYPE, NULL, NULL)) {
         strcpy(file_name, pszFile);
         obj_file = FALSE;
      } else {
         sprintf(file_name, "%s.%s", pszFile, OBJ_FILE_EXT);
      }
      if (*gzipped_fname == '\0') {
         if ((short_name=IsPrefix(bootDir, file_name, &rest))) ++rest;
      } else {
         if ((short_name=IsPrefix(bootDir, gzipped_fname, &rest))) ++rest;
      }
   }
   if (*file_name != '\0' && (fp=fopen(file_name, "r")) == NULL) {
      DelAllPages();
      lastPageNum = 1;
      InitPage();

      sprintf(gszMsgBox, TgLoadString(STID_CANNOT_OPEN_FILE_FOR_READING),
            (short_name ? rest : file_name));
      Msg(gszMsgBox);

      if (*file_name == DIR_SEP) {
         strcpy(full_name, file_name);
      } else {
         sprintf(full_name, "%s%c%s", curDir, DIR_SEP, file_name);
      }
      if (!cmdLineMerge) {
         if (obj_file) {
            SetCurDir(full_name);
            *curSymDir = '\0';
         } else {
            SetCurSymDir(full_name);
         }
         curFileDefined = TRUE;
      }
      SetFileModified(FALSE);
      sprintf(gszMsgBox, TgLoadString(STID_CUR_EMPTY_FILE_IS),
            (short_name ? rest : file_name));
      Msg(gszMsgBox);
   } else if (file_is_remote || *file_name != '\0') {
      int read_status;

      SetFileModified(FALSE);
      SaveStatusStrings();
      if (file_is_remote) {
         sprintf(gszMsgBox, TgLoadCachedString(CSTID_LOADING_FILE),
               remote_fname);
      } else {
         if (short_name) {
            sprintf(gszMsgBox, TgLoadCachedString(CSTID_LOADING_FILE), rest);
         } else {
            if (*gzipped_fname == '\0') {
               sprintf(gszMsgBox, TgLoadCachedString(CSTID_LOADING_FILE),
                     file_name);
            } else {
               sprintf(gszMsgBox, TgLoadCachedString(CSTID_LOADING_FILE),
                     gzipped_fname);
            }
         }
         if (*gzipped_fname == '\0') {
            if (*file_name == DIR_SEP) {
               strcpy(full_name, file_name);
            } else {
               sprintf(full_name, "%s%c%s", curDir, DIR_SEP, file_name);
            }
         } else {
            if (*gzipped_fname == DIR_SEP) {
               strcpy(full_name, gzipped_fname);
            } else {
               sprintf(full_name, "%s%c%s", curDir, DIR_SEP,
                     gzipped_fname);
            }
         }
      }
      SetStringStatus(gszMsgBox);

      strcpy(tmp_filefullpath, scanFileFullPath);
      strcpy(tmp_filename, scanFileName);
      tmp_linenum = scanLineNum;
      UtilStrCpyN(scanFileFullPath, sizeof(scanFileFullPath),
            (file_is_remote ? remote_fname : full_name));
      if (file_is_remote) {
         strcpy(scanFileName, tmp_remote_fname);
      } else {
         strcpy(scanFileName, (short_name ? rest : file_name));
      }
      scanLineNum = 0;

      SetWatchCursor(drawWindow);
      SetWatchCursor(mainWindow);

      readingPageNum = loadedCurPageNum = 0;
      foundGoodStateObject = FALSE;
      while ((read_status=ReadObj(fp, &obj_ptr)) == TRUE) {
         if (obj_ptr != NULL) {
            obj_ptr->tmp_parent = NULL;
            AdjForOldVersion(obj_ptr);
            AddObj(NULL, topObj, obj_ptr);
         }
      }
      strcpy(scanFileFullPath, tmp_filefullpath);
      strcpy(scanFileName, tmp_filename);
      scanLineNum = tmp_linenum;

      fclose(fp);

      if (!mergingFile && loadedCurPageNum <= 0) {
         DelAllPages();
         loadedCurPageNum = curPageNum = lastPageNum = 1;
         InitPage();
      }
      if (read_status == INVALID) {
         sprintf(gszMsgBox, TgLoadString(STID_FILE_VER_ABORT_OPEN),
               fileVersion, TOOL_NAME, homePageURL);
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      } else {
         if (loadedCurPageNum > 0 && curPage != NULL) {
            GotoPageNum(loadedCurPageNum);
         }
         CheckFileAttrsInLoad();

         curFileDefined = TRUE;
         if (file_is_remote) {
            if (!cmdLineMerge) {
               SetCurDir(remote_fname);
            }
            if (!foundGoodStateObject) PasteString(remote_buf, TRUE, TRUE);
         } else {
            if (*gzipped_fname == '\0') {
               if (*file_name == DIR_SEP) {
                  strcpy(full_name, file_name);
               } else {
                  sprintf(full_name, "%s%c%s", curDir, DIR_SEP, file_name);
               }
            } else {
               if (*gzipped_fname == DIR_SEP) {
                  strcpy(full_name, gzipped_fname);
               } else {
                  sprintf(full_name, "%s%c%s", curDir, DIR_SEP,
                        gzipped_fname);
               }
            }
            if (!cmdLineMerge) {
               if (obj_file) {
                  SetCurDir(full_name);
                  *curSymDir = '\0';
               } else {
                  SetCurSymDir(full_name);
               }
            }
         }
         if (file_is_remote) {
            sprintf(gszMsgBox, TgLoadCachedString(CSTID_CUR_FILE_IS),
                  remote_fname);
            RedrawDrawWindow(botObj);
         } else if (short_name) {
            sprintf(gszMsgBox, TgLoadCachedString(CSTID_CUR_FILE_IS), rest);
         } else {
            sprintf(gszMsgBox, TgLoadCachedString(CSTID_CUR_FILE_IS),
                  (*gzipped_fname == '\0' ? file_name : gzipped_fname));
         }
         Msg(gszMsgBox);
      }
      RestoreStatusStrings();

      GotoPageNum(loadedCurPageNum);
      SetDefaultCursor(mainWindow);
      SetDefaultCursor(drawWindow);

      if (file_is_remote) {
         unlink(tmp_remote_fname);
         if (tmp_remote_fname != NULL) FreeRemoteBuf(tmp_remote_fname);
         if (remote_buf != NULL) FreeRemoteBuf(remote_buf);
         remote_buf = tmp_remote_fname = NULL;
         if (!nAttemptToMerge && !mergingFile) CommitNavigate();
      } else {
         if (*gzipped_fname != '\0') {
            unlink(file_name);
         }
      }
      if (foundGoodStateObject && !nAttemptToMerge && !mergingFile) {
         struct AttrRec *exec_attr=FindFileAttrWithName("auto_exec=");

         if (!nAttemptToMerge && !mergingFile) CommitNavigate();
         if (exec_attr != NULL) {
            XSync(mainDisplay, False);
            if (pnNeedToCheckAutoExec != NULL) *pnNeedToCheckAutoExec = TRUE;
         }
      }
   }
   if (page_spec != NULL) {
      int new_page_num=(-1);

      if (!GetPageNumFromPageSpec(page_spec, &new_page_num)) {
         sprintf(gszMsgBox, TgLoadString(STID_INVALID_PAGE_SPECIFIED_FOR),
               pszFile);
         Msg(gszMsgBox);
      } else if (new_page_num != curPageNum) {
         GotoPageNum(new_page_num);
      }
   }
   if (page_spec != NULL) free(page_spec);

   if (pageLayoutMode == PAGE_TILE && nAttemptToMerge && pnAbortMerge != NULL) {
      if (file_is_remote) {
         sprintf(gszMsgBox, TgLoadString(STID_CANNOT_MERGE_WITH_TILED_PAGE),
               remote_fname);
      } else if (short_name) {
         sprintf(gszMsgBox, TgLoadString(STID_CANNOT_MERGE_WITH_TILED_PAGE),
               rest);
      } else {
         sprintf(gszMsgBox, TgLoadString(STID_CANNOT_MERGE_WITH_TILED_PAGE),
               (*gzipped_fname == '\0' ? file_name : gzipped_fname));
      }
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      *pnAbortMerge = TRUE;
   }
   return TRUE;
}

static
void SigchldHandler(sig)
   int sig;
{
   int status;

   /* wait for all the zombie processes to terminate */
   while(waitpid(-1, &status, WNOHANG) > 0);
}

int JustInit(pszFile, pnNeedToCheckAutoExec)
   char *pszFile;
   int *pnNeedToCheckAutoExec;
{
   int cannot_opendisplay=FALSE, allow_open=TRUE;

#ifdef USE_XT_INITIALIZE
   useXlib = cmdLineUseXlib;
#else /* ~USE_XT_INITIALIZE */
#ifdef USE_XAPPLRESDIR
   FILE *xenv_fp=NULL;
   char *c_ptr=NULL, *xEnvStr=NULL, tmp_buf[MAXPATHLENGTH+1];
   int len=0;

#ifdef XAPPLOADDIR_DEF
   if (getenv("XENVIRONMENT") == NULL) {
      if (((c_ptr=getenv("XAPPLRESDIR")) == NULL)) {
         UtilStrCpyN(tmp_buf, sizeof(tmp_buf), XAPPLOADDIR_DEF);
      } else if (!FindXApplResDir(c_ptr, tmp_buf, sizeof(tmp_buf))) {
         UtilStrCpyN(tmp_buf, sizeof(tmp_buf), XAPPLOADDIR_DEF);
      }
      c_ptr = tmp_buf;
      len = strlen(c_ptr);
#else /* ~XAPPLOADDIR_DEF */
   if ((getenv("XENVIRONMENT") == NULL) &&
         ((c_ptr=getenv("XAPPLRESDIR")) != NULL) && ((len=strlen(c_ptr)) > 0)) {
      if (FindXApplResDir(c_ptr, tmp_buf, sizeof(tmp_buf))) {
         c_ptr = tmp_buf;
      }
      len = strlen(c_ptr);
#endif /* XAPPLOADDIR_DEF */
      xEnvStr = (char*)malloc(
            (strlen("XENVIRONMENT=")+strlen(TOOL_NAME)+len+3)*sizeof(char));
      if (xEnvStr == NULL) FailAllocMessage();
      sprintf(xEnvStr, "%s%s%s", c_ptr,
            ((c_ptr[len-1] == DIR_SEP) ? "" : DIR_SEP_STR), TOOL_NAME);
      if ((xenv_fp=fopen(xEnvStr, "r")) != NULL) {
         fclose(xenv_fp);
         sprintf(xEnvStr, "XENVIRONMENT=%s%s%s", c_ptr,
               ((c_ptr[len-1] == DIR_SEP) ? "" : DIR_SEP_STR), TOOL_NAME);
         if (putenv(xEnvStr)) {
            sprintf(gszMsgBox, TgLoadString(STID_WARN_CANT_PUTENV_GIVEN),
                  xEnvStr);
            Msg(gszMsgBox);
            *xEnvStr = '\0';
            free(xEnvStr);
            xEnvStr = NULL;
         }
      }
   }
#endif /* USE_XAPPLRESDIR */
   useXlib = TRUE;
#endif /* USE_XT_INITIALIZE */

#ifndef _NO_LOCALE_SUPPORT
   MySetLocale(LC_ALL, "C");
   MySetLocale(LC_NUMERIC, "C");
#ifdef ENABLE_NLS
   InitLocale();
#endif /* ENABLE_NLS */
#endif /* ~_NO_LOCALE_SUPPORT */

   if (useXlib) {
      cannot_opendisplay = ((mainDisplay=XOpenDisplay(displayName)) == 0);
   } else {
#ifdef USE_XT_INITIALIZE
      toplevel = XtInitialize(TOOL_NAME, TOOL_NAME, NULL, 0, &origArgC,
            origArgV);
      cannot_opendisplay = (toplevel == NULL ||
            (mainDisplay=XtDisplay(toplevel)) == 0);
#endif /* USE_XT_INITIALIZE */
   }
   if (cannot_opendisplay) {
      if (displayName == NULL) {
         fprintf(stderr, "%s\n", TgLoadString(STID_CANT_OPEN_DISPLAY_ABORT));
         exit(-1);
      } else {
         fprintf(stderr, TgLoadString(STID_CANT_OPEN_GIVEN_DPY_ABORT),
               displayName);
         fprintf(stderr, "\n");
         exit(-1);
      }
   }
   mainScreen = DefaultScreen(mainDisplay);
   mainColormap = DefaultColormap(mainDisplay, mainScreen);
   mainDepth = DefaultDepth(mainDisplay, mainScreen);
   mainVisual = DefaultVisual(mainDisplay, mainScreen);
   rootWindow = RootWindow(mainDisplay, mainScreen);

#ifdef NOT_DEFINED /* debug, do not translate */
   fprintf(stderr,"mainDepth               =%d\n",mainDepth);
   fprintf(stderr,"mainVisual->class       =%d\n",mainVisual->class);
   fprintf(stderr,"mainVisual->red_mask    =0x%08lx\n",mainVisual->red_mask);
   fprintf(stderr,"mainVisual->green_mask  =0x%08lx\n",mainVisual->green_mask);
   fprintf(stderr,"mainVisual->blue_mask   =0x%08lx\n",mainVisual->blue_mask);
   fprintf(stderr,"mainVisual->bits_per_rgb=%d\n",mainVisual->bits_per_rgb);
   fprintf(stderr,"mainVisual->map_entries =%d\n",mainVisual->map_entries);
#endif /* NOT_DEFINED */

   XSetErrorHandler(EmergencySaveForX);
   XSetIOErrorHandler(IOEmergencySaveForX);
   signal(SIGHUP, EmergencySave);
   signal(SIGFPE, EmergencySave);
#ifndef linux
   signal(SIGBUS, EmergencySave);
#endif /* ~linux */
   signal(SIGSEGV, EmergencySave);
   signal(SIGCHLD, SigchldHandler);

#ifndef _NO_LOCALE_SUPPORT
#ifdef ENABLE_NLS
   InitLocale2();
#endif /* ENABLE_NLS */
#endif /* ~_NO_LOCALE_SUPPORT */

   InitWhiteBoard();

   Setup();
   InitExtraWinInfo();
   InitLastKeyOrBtnEvInfo();
   quitDraw = FALSE;
   prTgifFoundColorInfo = FALSE;

   InitWhiteBoard2();
   if (pszFile == NULL) {
      /* Only gets here if PRTGIF of UrlDump() */
      DelAllPages();
      lastPageNum = 1;
      InitPage();
      return TRUE;
   }
   initializingMain = TRUE;

   if (gstWBInfo.do_whiteboard) {
      static int initialized=FALSE;

#ifdef _TGIF_DBG /* debug, do not translate */
      TgAssert(!initialized,
            "Whiteboard initialized too many times in JustInit()", NULL);
#endif /* _TGIF_DBG */
      if (gstWBInfo.join_session_in_progress) {
         allow_open = FALSE;
      }
      initialized = TRUE;
   }
   if (!allow_open) {
      char *fname=NULL;

      *gszMsgBox = '\0';
      if (cmdLineMerge && cmdLineMergeArgc > 0 && cmdLineMergeArgv != NULL) {
         fname = cmdLineMergeArgv[0];
         sprintf(gszMsgBox, TgLoadString(STID_JOIN_WB_IN_PROGRESS_IGNORED),
               fname);
         cmdLineMerge = FALSE;
      } else if (*pszFile != '\0') {
         fname = pszFile;
         sprintf(gszMsgBox, TgLoadString(STID_JOIN_WB_IN_PROGRESS_IGNORED),
               fname);
         *pszFile = '\0';
      }
      if (fname != NULL) {
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
         UtilFree(fname);
      }
   }
   if (cmdLineMerge && cmdLineMergeArgc > 0 && cmdLineMergeArgv != NULL) {
      int arg_index=0, abort_merge=FALSE, num_argc=cmdLineMergeArgc;
      int identical=FALSE;
      char **ppsz_html_export_template=(char**)malloc(cmdLineMergeArgc*
            sizeof(char*));

      if (ppsz_html_export_template == NULL) FailAllocMessage();
      memset(ppsz_html_export_template, 0, cmdLineMergeArgc*sizeof(char*));

      if (JustLoadAFile(cmdLineMergeArgv[0], NULL, TRUE, &abort_merge)) {
         ppsz_html_export_template[0] = UtilStrDup(gszHhtmlExportTemplate);
         if (ppsz_html_export_template[0] == NULL) FailAllocMessage();

         if (!abort_merge) {
            PageInfo pi;

            memset(&pi, 0, sizeof(PageInfo));
            GetPageInfo(&pi);

            CleanUpComments();

            firstPage = lastPage = curPage = NULL;
            topObj = botObj = NULL;
            DelAllPages();
            lastPageNum = 1;
            InitPage();

            mergingFile = TRUE;
            for (arg_index=1; arg_index < cmdLineMergeArgc; arg_index++) {
               if (!JustLoadAFile(cmdLineMergeArgv[arg_index], NULL, FALSE,
                     NULL)) {
                  break;
               }
               ppsz_html_export_template[arg_index] =
                     UtilStrDup(gszHhtmlExportTemplate);
               if (ppsz_html_export_template[arg_index] == NULL) {
                  FailAllocMessage();
               }
               CleanUpComments();

               firstPage->prev = pi.last_page;
               if (pi.last_page == NULL) {
                  pi.first_page = firstPage;
               } else {
                  pi.last_page->next = firstPage;
               }
               pi.last_page = lastPage;
               pi.last_page_num += lastPageNum;

               firstPage = lastPage = curPage = NULL;
               topObj = botObj = NULL;

               DelAllPages();
               lastPageNum = 1;
               InitPage();
            }
            mergingFile = FALSE;
            DelAllPages();

            curPageNum = pi.cur_page_num;
            lastPageNum = pi.last_page_num;
            firstPage = pi.first_page;
            lastPage = pi.last_page;
            curPage = pi.cur_page;
            topObj = curPage->top;
            botObj = curPage->bot;
         }
      }
      CleanUpMergeArgs();
      ClearFileInfo(FALSE);
      SetFileModified(TRUE);

      identical = TRUE;
      for (arg_index=1; arg_index < num_argc; arg_index++) {
         if (strcmp(ppsz_html_export_template[0],
               ppsz_html_export_template[arg_index]) != 0) {
            identical = FALSE;
            break;
         }
      }
      if (!identical) {
         UtilStrCpyN(gszMsgBox, sizeof(gszMsgBox),
               TgLoadString(STID_NO_HTML_TEMPLATE_FILE));
         Msg(gszMsgBox);
      } else if (*ppsz_html_export_template[0] != '\0') {
         UtilStrCpyN(gszHhtmlExportTemplate, sizeof(gszHhtmlExportTemplate),
            ppsz_html_export_template[0]);
         sprintf(gszMsgBox, TgLoadString(STID_HTML_TEMPLATE_SET_TO_GIVEN),
               gszHhtmlExportTemplate);
         Msg(gszMsgBox);
      }
      for (arg_index=0; arg_index < num_argc; arg_index++) {
         UtilFree(ppsz_html_export_template[arg_index]);
      }
      free(ppsz_html_export_template);
   } else if (*pszFile != '\0') {
      JustLoadAFile(pszFile, pnNeedToCheckAutoExec, FALSE, NULL);
   } else {
      if (allow_open) {
         DelAllPages();
         lastPageNum = 1;
         InitPage();
      } else {
         SetFileModified(TRUE);
      }
   }
   SetDefaultDrawWinClipRecs();
   UpdateDirInfo();

   initializingMain = FALSE;

   return TRUE;
}

void HandleDataInMBuff()
{
   if (gstWBInfo.do_whiteboard) {
      TryHandleWBInputData();
   }
}

static
int GetAnXEvent(pXEvent)
   XEvent *pXEvent;
{
   if (gstWBInfo.do_whiteboard) {
      int rc=TRUE;

      gstWBInfo.BlockRemoteCmdDepth--;
#ifdef DEBUG0
      fprintf(stderr,"BlockRemoteCmdDepth (GetAnXEvent): %d\n",
            gstWBInfo.BlockRemoteCmdDepth);
#endif /* DEBUG0 */
      if (XPending(mainDisplay)) {
         if (gnInputMethod != TGIM_NONE && tgIMExpectNextEvent(mainDisplay,
               drawWindow)) {
            if (!tgIMHandleNextEvent(mainDisplay, drawWindow, pXEvent)) {
            }
         } else {
            XNextEvent(mainDisplay, pXEvent);
         }
      } else {
         struct timeval timeout;
         fd_set fdset;
         int select_width=XConnectionNumber(mainDisplay)+1, status=0, nfds=0;

         nfds = max(select_width, (int)(talkToSelfFiledes[0]+1));

         timeout.tv_sec = 15;
         timeout.tv_usec = 0;

         FD_ZERO(&fdset);
         FD_SET(select_width-1, &fdset); /* X file descriptor */
         FD_SET(talkToSelfFiledes[0], &fdset); /* pipe */
#ifdef __hpux
         status = select(nfds, (int*)&fdset, NULL, NULL, &timeout);
#else /* !__hpux */
         status = select(nfds, &fdset, NULL, NULL, &timeout);
#endif /* __hpux */
         if (status < 0) {
            if (errno == EINTR) {
               /* interrupted by a system call, not a problem */
            } else {
               sprintf(gszMsgBox,
                     TgLoadString(STID_FUNC_SELECT_SYS_CALL_FAILED),
                     "GetAnXEvent()"),
               fprintf(stderr, "%s\n", gszMsgBox);
            }
            rc = FALSE;
         } else if (status == 0) {
            /* timeout */
            rc = FALSE;
         } else if (status == 1) {
            if (FD_ISSET(talkToSelfFiledes[0], &fdset)) {
               char buf[2];

               if (read(talkToSelfFiledes[0], buf, 1) != 1) {
                  sprintf(gszMsgBox,
                        TgLoadString(STID_READ_FROM_SELF_PIPE_FAIL), 1,
                        TOOL_NAME),
                  fprintf(stderr, "%s\n", gszMsgBox);
               } else {
                  if (*buf == 'a') {
                     SendCommandToSelf(CMDID_DATA_IN_MBUFF, 0);
                  } else if (*buf == 'c') {
                     /*
                      * Get current tgwb state and establish
                      *         a TCP conection to send the data.
                      */
                     HandleNewUserRequest();
                  } else {
                     sprintf(gszMsgBox,
                           TgLoadString(STID_UNRECOG_BYTE_FROM_SELF_PIPE), *buf,
                           TOOL_NAME),
                     fprintf(stderr, "%s\n", gszMsgBox);
                  }
               }
               rc = FALSE;
            } else if (FD_ISSET(select_width-1, &fdset)) {
               if (gnInputMethod != TGIM_NONE && tgIMExpectNextEvent(mainDisplay,
                     drawWindow)) {
                  if (!tgIMHandleNextEvent(mainDisplay, drawWindow, pXEvent)) {
                  }
               } else {
                  XNextEvent(mainDisplay, pXEvent);
               }
            } else {
               rc = FALSE;
            }
         } else {
            sprintf(gszMsgBox, TgLoadString(STID_FUNC_INVALID_RC_FOR_SELECT),
                  "GetAnXEvent()", status);
            fprintf(stderr, "%s\n", gszMsgBox);
            rc = FALSE;
         }
      }
      gstWBInfo.BlockRemoteCmdDepth++;
#ifdef DEBUG0
      fprintf(stderr,"BlockRemoteCmdDepth (GetAnXEvent): %d\n",
            gstWBInfo.BlockRemoteCmdDepth);
#endif /* DEBUG0 */
      if (!rc) return FALSE;
   } else {
      if (gnInputMethod != TGIM_NONE && tgIMExpectNextEvent(mainDisplay,
            drawWindow)) {
         if (!tgIMHandleNextEvent(mainDisplay, drawWindow, pXEvent)) {
         }
      } else {
         XNextEvent(mainDisplay, pXEvent);
      }
   }
   return TRUE;
}

int TryProcessAnXEvent(pnNeedToCheckAutoExec)
   int *pnNeedToCheckAutoExec;
{
   XEvent input;
   int rc=0, drawtext_cursorshown_keypress_processed=FALSE;
   TidgetInfo *handling_pti=NULL;

   exitNormally = FALSE;
   if (pnNeedToCheckAutoExec != NULL && (*pnNeedToCheckAutoExec) &&
         XPending(mainDisplay) <= 0) {
      struct AttrRec *exec_attr=FindFileAttrWithName("auto_exec=");

      *pnNeedToCheckAutoExec = FALSE;
      if (exec_attr != NULL) {
         DoExecLoop(NULL, exec_attr);
      }
   }
   if (!GetAnXEvent(&input)) {
      return 0;
   }

   /* set the lastKeyOrBtnEvInfo */
   if (input.type == KeyPress) {
      SetLastKeyEvInfo(&input.xkey);
   } else if (input.type == ButtonPress) {
      SetLastBtnEvInfo(&input.xbutton);
   }

   if (input.type == FocusIn) {
      if (curChoice == DRAWTEXT && textCursorShown) {
         tgIMFocusIn(mainDisplay, input.xany.window);
      }
   } else if (input.type == FocusOut) {
      if (curChoice == DRAWTEXT && textCursorShown) {
         tgIMFocusOut(mainDisplay, input.xany.window);
      }
   } else if (input.type == KeyRelease) {
      return 0;
   } else if (input.type == KeyPress) {
      if (TidgetManagerWantAllKeyPressEvents()) {
         TidgetManagerHandleAllKeyPressEvent(&input);
         return 0;
      }
      rc = ShortHand(&input);
      switch (rc) {
      case BAD: /* <CONTROL> or <META> */ return 0;
      case INVALID:
         if (curChoice != DRAWTEXT || !textCursorShown) {
            escPressed = FALSE;
         }
         break;
      default: return rc;
      }
   }

   if (input.xany.window == choiceWindow) {
      if ((rc=ChoiceEventHandler(&input)) != INVALID) {
         return rc;
      }
   } else if (input.xany.window == drawWindow) {
      if (input.type == ButtonPress) {
         if (gstWBInfo.do_whiteboard) {
            gstWBInfo.BlockRemoteCmdDepth++;
         }
      }
      if (curChoice == DRAWTEXT && textCursorShown && input.type == KeyPress) {
          drawtext_cursorshown_keypress_processed = TRUE;
      }
      rc = DrawingEventHandler(&input);
      if (input.type == ButtonPress) {
         if (gstWBInfo.do_whiteboard) {
            gstWBInfo.BlockRemoteCmdDepth--;
            TryHandleWBInputData();
         }
      }
      if (rc != INVALID) {
         return rc;
      }
   } else if (input.xany.window == mainWindow) {
      rc = mainWinEventHandler(&input);
      switch (rc) {
      case BAD: /* <CONTROL> or <META> */ return 0;
      case INVALID: break;
      default: return rc;
      }
   } else if (input.xany.window == vRuleWindow ||
         input.xany.window == hRuleWindow) {
      RulersEventHandler(&input);
   } else if (input.xany.window == iconWindow ||
         input.xany.window == iconBaseWindow) {
      IconEventHandler(&input);
   } else if (input.xany.window == titleWindow) {
      TitleEventHandler(&input);
   } else if (menubarWindow != None && input.xany.window == menubarWindow) {
      if ((rc=MenubarEventHandler(&input)) != INVALID) {
         return rc;
      }
   } else if (input.xany.window == msgWindow) {
      MsgEventHandler(&input);
   } else if (input.xany.window == vSBarWindow ||
         input.xany.window == hSBarWindow) {
      ScrollEventHandler(&input);
   } else if (userRedrawWindow != None &&
         input.xany.window == userRedrawWindow) {
      UserRedrawEventHandler(&input);
   } else if (statusWindow != None && input.xany.window == statusWindow) {
      StatusEventHandler(&input);
   } else if (chatWindow != None && IsChatWindowEvent(&input, &handling_pti)) {
      ChatEventHandler(&input, handling_pti);
      if (curChoice == DRAWTEXT && textCursorShown && input.type == KeyPress) {
          drawtext_cursorshown_keypress_processed = TRUE;
      }
   } else if (modeWindow != None && input.xany.window == modeWindow) {
      if ((rc=ModeEventHandler(&input)) != INVALID) {
         return rc;
      }
   } else if (input.xany.window == dummyWindow1 ||
         input.xany.window == dummyWindow2) {
      DummiesEventHandler(&input);
   } else if (input.xany.type == MappingNotify) {
      XRefreshKeyboardMapping(&(input.xmapping));
   } else if (input.xany.window == pageWindow) {
      PageEventHandler(&input);
   } else if (input.xany.window == pageDummyWindow) {
      PageDummyEventHandler(&input);
   } else if (input.xany.window == colorWindow) {
      ColorEventHandler(&input);
   } else if (input.xany.window == colorDummyWindow) {
      ColorDummyEventHandler(&input);
   } else if (TidgetManagerHandleEvent(&input)) {
      /* input has been handled by the TidgetManager */
   } else {
      int i=0;

      for (i=0; i < numExtraWins; i++) {
         if (input.xany.window == extraWinInfo[i].window &&
               extraWinInfo[i].window != None) {
            if ((rc=(*(extraWinInfo[i].ev_handler))(&input,
                  &extraWinInfo[i])) != INVALID) {
               return rc;
            }
            break;
         }
      }
   }
#ifndef _NO_ENTER_TEXT_EVERYWHERE
   if (curChoice == DRAWTEXT && textCursorShown && input.type == KeyPress &&
         !drawtext_cursorshown_keypress_processed) {
      rc = DrawingEventHandler(&input);
      if (rc != INVALID) {
         return rc;
      }
   }
#endif /* ~_NO_ENTER_TEXT_EVERYWHERE */
   return 0;
}

static XComposeStatus c_stat;

int TryProcessAnAbortXEvent(esc_to_abort, pf_abort_callback, p_void)
   int esc_to_abort;
   AbortCallbackFunc *pf_abort_callback;
   void *p_void;
{
   XEvent input;

   exitNormally = FALSE;
   if (!GetAnXEvent(&input)) {
      return FALSE;
   }
   if (input.type == KeyPress || input.type == KeyRelease) {
      char s[80];
      int has_ch=0;
      XKeyEvent *key_ev=NULL;
      KeySym key_sym=(KeySym)0;

      key_ev = (&(input.xkey));
      has_ch = XLookupString(key_ev, s, sizeof(s), &key_sym, &c_stat);
      if (has_ch > 0) s[has_ch] = '\0';
      TranslateKeys(s, &key_sym);
      if (esc_to_abort && CharIsESC(key_ev, s, key_sym, &has_ch)) {
         if (pf_abort_callback == NULL || (pf_abort_callback)(p_void)) {
            return TRUE;
         }
      } else if (!esc_to_abort && has_ch) {
         unsigned int state=key_ev->state;

         if ((state & ControlMask) && (!(state & METAMASK)) && s[0] == '\003') {
            if (pf_abort_callback == NULL || (pf_abort_callback)(p_void)) {
               return TRUE;
            }
         }
      }
   } else if (input.type == MotionNotify) {
      while (XCheckMaskEvent(mainDisplay, PointerMotionMask, &input)) ;
   } else if (input.type == Expose || input.type == VisibilityNotify) {
      ExposeEventHandler(&input, TRUE);
      while (XCheckMaskEvent(mainDisplay, ExposureMask | VisibilityChangeMask,
            &input)) {
      }
   } else if (input.type == ConfigureNotify) {
      Reconfigure(FALSE);
   }
   return FALSE;
}

void MainLoop(Op, FileName, FuncStr, Str1, Menu1, Str2, Menu2, Str3, Menu3)
   char *Op, *FileName, **FuncStr;
   char **Str1, **Menu1, **Str2, **Menu2, **Str3, **Menu3;
{
   char *c_ptr=NULL, file_name[MAXPATHLENGTH+1];
   int need_to_check_auto_exec=FALSE;

/* printf("--> MainLoop(%s, %s, ...)\n", Op, FileName); */

   /* do not translate -- program constants */
   if (strcmp(Op, "init") == 0 || strcmp(Op, "justinit") == 0) {
      char szMsg1[80], szMsg2[80];

      GetTgifVersionAndPatchLevel(szMsg1, sizeof(szMsg1));
      sprintf(szMsg2, "%s (%s)", copyrightString, authorEmailString);
      TwoLineMsg(szMsg1, szMsg2);
      if (cmdLineVersion) {
         fprintf(stderr, "%s\n", szMsg1);
         fprintf(stderr, "%s\n", szMsg2);
      }
      fflush(stderr);

      exitNormally = FALSE;

#ifdef X_PROTOCOL
#ifdef XlibSpecificationRelease
      x11R6OrAbove = (X_PROTOCOL >= 11 && XlibSpecificationRelease >= 6);
#endif /* XlibSpecificationRelease */
#endif /* X_PROTOCOL */

      JustInit(FileName, &need_to_check_auto_exec);
      if (*cmdLineFileToExec != '\0') {
         SendCommandToSelf(CMDID_STARTEXECCMDSFROMFILE, 0);
      }
   }
   SaveDrawWinInfo();

   if (strcmp(Op, "justinit") == 0) {
      XSync(mainDisplay, False);
      AllocStrings(FuncStr, Str1, Menu1, Str2, Menu2, Str3, Menu3);
      strcpy(*FuncStr, "");
      return;
   }
   if (strcmp(Op, "save") == 0) {
      SaveFile();
      AllocStrings(FuncStr, Str1, Menu1, Str2, Menu2, Str3, Menu3);
      strcpy(*FuncStr, "");
      return;
   }
   if (strcmp(Op, "vi") == 0) {
      ExecWithFile("vi", FileName);
   }
   if (strcmp(Op, "less") == 0) {
      ExecWithFile("less", FileName);
   }
   if (strcmp(Op, "quit") == 0) {
      CleanUp();
      quitDraw = TRUE;
      XFlush(mainDisplay);
      XSync(mainDisplay, True);
      if (useXlib) {
         XCloseDisplay(mainDisplay);
         mainDisplay = NULL;
      } else {
#ifdef USE_XT_INITIALIZE
         XtDestroyWidget(toplevel);
         toplevel = (Widget)0;
#endif /* USE_XT_INITIALIZE */
      }
      exitNormally = TRUE;
      AllocStrings(FuncStr, Str1, Menu1, Str2, Menu2, Str3, Menu3);
      strcpy(*FuncStr, "Quit");
      strcpy(*Str1, "");
      return;
   }
   if (strcmp(Op, "msg") == 0) {
      Msg(FileName);
      AllocStrings(FuncStr, Str1, Menu1, Str2, Menu2, Str3, Menu3);
      strcpy(*FuncStr, "");
      return;
   }
   if (strcmp(Op, "dialog") == 0) {
      *file_name = '\0';
      Dialog(FileName, "", file_name);
      AllocStrings(FuncStr, Str1, Menu1, Str2, Menu2, Str3, Menu3);
      strcpy(*FuncStr, file_name);
      strcpy(*Str1, "");
      return;
   }
   if (strcmp(Op, "mainmenu") == 0 && quitDraw) {
      AllocStrings(FuncStr, Str1, Menu1, Str2, Menu2, Str3, Menu3);
      strcpy(*FuncStr, "Fail");
      strcpy(*Str1, "");
      return;
   }
   while (TRUE) {
      int rc=TryProcessAnXEvent(&need_to_check_auto_exec);

      if (rc != 0) {
         AllocStrings(FuncStr, Str1, Menu1, Str2, Menu2, Str3, Menu3);
         strcpy(*FuncStr, fileMenuStr[rc]);
         strcpy(*Str1, curDomainName);
         sprintf(*Menu1, "tmpmodel.%s", OBJ_FILE_EXT);
         strcpy(*Str2, "");
         for (c_ptr = *FuncStr; *c_ptr != '\0'; c_ptr++) {
            if (*c_ptr == ' ') {
               *c_ptr = '\0';
               break;
            }
         }
         return;
      }
   }
}

void UrlDump(pszUrl)
   char *pszUrl;
{
   int saved_http_debug=debugHttp;
   int saved_ftp_debug=debugFtp;

   debugHttp = 99;
   debugFtp = (-99);
   if (FileIsRemote(pszUrl)) {
      char tmp_fname[MAXPATHLENGTH+1], *content_type=NULL, *page_spec=NULL;
      int is_html=FALSE;

      *tmp_fname = '\0';
      if (cmdLineDumpURL && cmdLineDumpURLWithHeader &&
            cmdLineDumpURLHeaderOnly) {
         gettingHttpHeaderOnly = TRUE;
      }
      if (cmdLineParseHtml) {
         cmdLineDumpURL = FALSE;
         debugHttp = saved_http_debug;
         InitHtml();
      } else {
         if (cmdLineOpenDisplay) {
            JustInit(NULL, NULL);
         }
      }
      DownloadRemoteFile(pszUrl, &content_type, &page_spec,
            &is_html, tmp_fname, NULL, 0);
      gettingHttpHeaderOnly = FALSE;
      if (content_type != NULL) FreeRemoteBuf(content_type);
      if (page_spec != NULL) UtilFree(page_spec);
      if (*tmp_fname != '\0') unlink(tmp_fname);
   } else {
      FILE *fp=fopen(pszUrl, "r");

      if (fp == NULL) {
         fprintf(stderr, TgLoadString(STID_CANNOT_OPEN_FILE_FOR_READING),
               pszUrl);
         fprintf(stderr, "\n");
      } else {
         char *buf;

         while ((buf=UtilGetALine(fp)) != NULL) {
            fprintf(stdout, "%s\n", buf);
            UtilFree(buf);
         }
         fclose(fp);
      }
   }
   debugHttp = saved_http_debug;
   debugFtp = saved_ftp_debug;
}

void DosEpsFilter(pszUrl)
   char *pszUrl;
{
   if (MyReadEPSFile(pszUrl, NULL, NULL, NULL, NULL, NULL,
         NULL, NULL, NULL, NULL, NULL, NULL, NULL)) {
   }
}

static
void HandleSimpleEvent(input)
   XEvent input;
{
   if ((input.type & (PointerMotionMask | EnterWindowMask | LeaveWindowMask))
         != 0) {
      return;
   }
   if (input.xany.window == drawWindow) {
      DrawingEventHandler(&input);
   } else if (input.xany.window == choiceWindow) {
      ChoiceEventHandler(&input);
   } else if (input.xany.window==iconWindow ||
         input.xany.window==iconBaseWindow) {
      IconEventHandler(&input);
   } else if (input.xany.window == titleWindow) {
      TitleEventHandler(&input);
   } else if (input.xany.window == msgWindow) {
      MsgEventHandler(&input);
   } else if (input.xany.window==vSBarWindow ||
         input.xany.window==hSBarWindow) {
      ScrollEventHandler(&input);
   } else if (input.xany.window == hRuleWindow) {
      RedrawHRulerWindow();
   } else if (input.xany.window == vRuleWindow) {
      RedrawVRulerWindow();
   }
}

static
void AllocReturnStr(ReturnStr)
   char **ReturnStr;
{
   if((*ReturnStr=(char*)malloc(80*sizeof(char))) == NULL) FailAllocMessage();
}

void Animate(TypeStr, PolyId, SpeedStr, ColorStr, ReturnStr)
   char *TypeStr, *PolyId, *SpeedStr, *ColorStr;
   char **ReturnStr;
{
   struct ObjRec *obj_ptr;
   char s[80];
   int i, poly_id, speed, pixel, clicked = FALSE;
   XEvent input;
   XButtonEvent *button_event;

   AllocReturnStr(ReturnStr);
   strcpy(*ReturnStr, "");

   while (XPending(mainDisplay) != 0) {
      XPeekEvent(mainDisplay, &input);
      if ((input.type & (PointerMotionMask | ExposureMask | EnterWindowMask |
            LeaveWindowMask)) != 0) {
         XNextEvent(mainDisplay, &input);
         HandleSimpleEvent(input);
      } else {
         if (input.type == ButtonPress) clicked = TRUE;
         /* do not translate -- program constants */
         strcpy(*ReturnStr, "Interrupt");
         break;
      }
   }
   /* do not translate -- program constants */
   printf("--> Animate(%s, %s, %s, %s)\n",TypeStr,PolyId,SpeedStr,ColorStr);
   if (strcmp(TypeStr, "waitclick") == 0) {
      Msg(TgLoadString(STID_LEFT_STEP_MID_RUN_RIGHT_STOP));
      if (!clicked) {
         while (TRUE) {
            if (XPending(mainDisplay) != 0) {
               XNextEvent(mainDisplay, &input);
               if (input.type == ButtonPress) {
                  break;
               }
            }
         }
      } else {
         XNextEvent(mainDisplay, &input);
      }

      button_event = &(input.xbutton);
      /* do not translate -- program constants */
      switch (button_event->button) {
      case Button1 : strcpy(*ReturnStr, "Left"); break;
      case Button2 : strcpy(*ReturnStr, "Middle"); break;
      case Button3 : strcpy(*ReturnStr, "Right"); break;
      }
      Msg("");
   } else {
      poly_id = atoi(PolyId);
      for (obj_ptr=topObj; obj_ptr != NULL; obj_ptr=obj_ptr->next) {
         if (obj_ptr->type == OBJ_POLY && obj_ptr->id == poly_id) {
            break;
         }
      }
      /* do not translate -- program constants */
      if (obj_ptr == NULL) {
         sprintf(s, TgLoadString(STID_CANT_FIND_POLY_WITH_GIVEN_ID), poly_id);
         Msg(s);
      } else if (strcmp(TypeStr, "send") == 0) {
         speed = atoi(SpeedStr);
         pixel = (colorDisplay) ? xorColorPixels[obj_ptr->color] : 1;
         AnimateSend(obj_ptr->detail.p, speed, pixel);
      } else if (strcmp(TypeStr, "flash") == 0) {
         if (colorDisplay) {
            strcpy(s, ColorStr);
         } else {
            strcpy(s, "white");
         }
         for (i = 0; i < maxColors; i++) {
            if (strcmp(colorMenuItems[i], s) == 0) {
               AnimateFlashColor(obj_ptr, i);
               break;
            }
         }
      }
   }
}

static
void MyFormat(Str)
   char *Str;
{
   register char *c_ptr=Str, *period_ptr=NULL;
   register int i;

   for ( ; *c_ptr != '\0'; c_ptr++) {
      if (*c_ptr >= '0' && *c_ptr <= '9') {
         continue;
      } else if (*c_ptr == '.') {
         if (period_ptr != NULL) return;
         period_ptr = c_ptr;
         continue;
      } else {
         return;
      }
   }

   if (period_ptr == NULL) return;
   for (c_ptr = period_ptr, i = 0; *c_ptr != '\0' && i < 7; c_ptr++, i++) ;
   if (*c_ptr != '\0') *c_ptr = '\0';
}

void UpdAttrVal(ObjId, AttrName, AttrColor, AttrVal, ReturnStr)
   char *ObjId, *AttrName, *AttrColor, *AttrVal;
   char **ReturnStr;
{
   struct ObjRec *obj_ptr;
   struct AttrRec *attr_ptr;
   char s[MAXSTRING];
   int obj_id;
   XEvent input;
   XButtonEvent *button_event;

   AllocReturnStr(ReturnStr);
   strcpy(*ReturnStr, "");

   while (XPending(mainDisplay) != 0) {
      XPeekEvent(mainDisplay, &input);
      if ((input.type & (PointerMotionMask | ExposureMask | EnterWindowMask |
            LeaveWindowMask)) != 0) {
         XNextEvent(mainDisplay, &input);
         HandleSimpleEvent(input);
      } else {
         /* do not translate -- program constants */
         strcpy(*ReturnStr, "Interrupt");
         button_event = &(input.xbutton);
         switch (button_event->button) {
         case Button1 : strcpy(*ReturnStr, "Left"); break;
         case Button2 : strcpy(*ReturnStr, "Middle"); break;
         case Button3 : strcpy(*ReturnStr, "Right"); break;
         }
         break;
      }
   }
   /* do not translate -- program constants */
   printf("--> UpdAttrVal(%s, %s, %s, %s)\n", ObjId, AttrName,
         AttrColor, AttrVal);

   obj_id = atoi(ObjId);

   for (obj_ptr=topObj; obj_ptr != NULL; obj_ptr=obj_ptr->next) {
      if (obj_ptr->id == obj_id) {
         break;
      }
   }
   if (obj_ptr == NULL) {
      sprintf(gszMsgBox, TgLoadString(STID_CANT_FIND_OBJ_WITH_GIVEN_ID),
            obj_id);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
   } else {
      attr_ptr = obj_ptr->fattr;
      for ( ; attr_ptr != NULL; attr_ptr=attr_ptr->next) {
         if (strcmp(AttrName, attr_ptr->attr_name.s) == 0 &&
               strcmp(AttrColor, colorMenuItems[attr_ptr->obj->color]) == 0) {
            break;
         }
      }
      if (attr_ptr == NULL) {
         sprintf(gszMsgBox, TgLoadString(STID_CANT_FIND_ATTR_NAME_AND_COLOR),
               AttrName, AttrColor);
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      } else {
         struct DynStrRec *dyn_str_ptr=NULL;

         DynStrSet(&attr_ptr->attr_value, AttrVal);
         MyFormat(attr_ptr->attr_value.s);
         if (attr_ptr->nameshown) {
            sprintf(s, "%s%s", attr_ptr->attr_name.s, attr_ptr->attr_value.s);
         } else {
            strcpy(s, attr_ptr->attr_value.s);
         }
         if (attr_ptr->shown) RepaintFirstStr(attr_ptr->obj, s);
         dyn_str_ptr = GetTextFirstDynStr(attr_ptr->obj);

         DynStrSet(dyn_str_ptr, s);
      }
   }
}

void SetLastKeyEvInfo(key_ev)
    XKeyEvent *key_ev;
{
   lastKeyOrBtnEvInfo.type = key_ev->type;
   lastKeyOrBtnEvInfo.send_event = key_ev->send_event;
   lastKeyOrBtnEvInfo.display = key_ev->display;
   lastKeyOrBtnEvInfo.window = key_ev->window;
   memcpy(&lastKeyOrBtnEvInfo.time, &key_ev->time, sizeof(Time));
   lastKeyOrBtnEvInfo.x = key_ev->x;
   lastKeyOrBtnEvInfo.y = key_ev->y;
   lastKeyOrBtnEvInfo.keycode_or_button = key_ev->keycode;
   lastKeyOrBtnEvInfo.state = key_ev->state;
}

void SetLastBtnEvInfo(button_ev)
    XButtonEvent *button_ev;
{
   lastKeyOrBtnEvInfo.type = button_ev->type;
   lastKeyOrBtnEvInfo.send_event = button_ev->send_event;
   lastKeyOrBtnEvInfo.display = button_ev->display;
   lastKeyOrBtnEvInfo.window = button_ev->window;
   memcpy(&lastKeyOrBtnEvInfo.time, &button_ev->time, sizeof(Time));
   lastKeyOrBtnEvInfo.x = button_ev->x;
   lastKeyOrBtnEvInfo.y = button_ev->y;
   lastKeyOrBtnEvInfo.keycode_or_button = button_ev->button;
   lastKeyOrBtnEvInfo.state = button_ev->state;
}

