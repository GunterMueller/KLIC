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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/inmethod.c,v 1.14 2011/06/14 02:32:18 william Exp $
 */

#define _INCLUDE_FROM_INMETHOD_C_

#include "tgifdefs.h"

#ifndef _NO_XCIN
#include "cli_xcin.e"
#endif /* ~_NO_XCIN */
#ifndef _NO_CHINPUT
#include "chinput.e"
#endif /* ~_NO_CHINPUT */
#ifndef _NO_KINPUT
#include "convkinput.e"
#endif /* ~_NO_KINPUT */
#ifndef _NO_XIM
#include "convxim.e"
#endif /* ~_NO_XIM */
#ifndef _NO_TGTWB5
#include "tgtwb5.e"
#endif /* ~_NO_TGTWB5 */

#include "dialog.e"
#include "inmethod.e"
#include "msg.e"
#include "util.e"
#include "setup.e"
#include "strtbl.e"
#include "tidget.e"

#ifndef _NO_DL_SUPPORT
#define CLOSE_DL(handle) dlclose(handle)
#define OPEN_DL(path,flag) dlopen((path),(flag))
#define GET_DL_SYM(handle,symbol) dlsym((handle),(symbol))
#ifndef RTLD_NOW
#define OPEN_DL_MODE 1
#else /* RTLD_NOW */
#define OPEN_DL_MODE (RTLD_NOW|RTLD_GLOBAL)
#endif /* ~RTLD_NOW */
#else /* _NO_DL_SUPPORT */
#define CLOSE_DL(handle)
#define OPEN_DL(path,flag) NULL
#define GET_DL_SYM(handle,symbol) NULL
#define OPEN_DL_MODE 0
#endif /* ~_NO_DL_SUPPORT */

int gnInputMethod=TGIM_NONE; /* one of TGIM_* */
int gnOverTheSpot=FALSE;

int cmdLineHasInputMethod=FALSE;
int cmdLineDoubleByteInputMethod=FALSE;
char cmdLineInputMethod[MAXSTRING];

static int gnInputMethodIndex=INVALID;
static int gnSingleOrDoubleByteInputMethod=0;

typedef void (CleanUpFunc)ARGS_DECL((Display *dpy, Window win));
typedef int (InitFunc)ARGS_DECL((Display *dpy, Window win, char *arg));
typedef int (FocusInFunc)ARGS_DECL((Display *dpy, Window win));
typedef int (FocusOutFunc)ARGS_DECL((Display *dpy, Window win));
typedef int (SendKeyFunc)ARGS_DECL((Display *dpy, Window win,
                                    XKeyEvent *key_ev, char *buf));
typedef int (ConvPropertyFunc)ARGS_DECL((Display *dpy, Window win,
                                         XPropertyEvent *prop_ev,
                                         char **ppsz_buf));
typedef int (ExpectCMFunc)ARGS_DECL((Display *dpy, Window win));
typedef int (HandleCMFunc)ARGS_DECL((Display *dpy, Window win,
                                     XClientMessageEvent *cm_ev,
                                     XKeyEvent *key_ev, char *buf));
typedef int (ActivateOnCntrlSpaceFunc)ARGS_DECL((Display *dpy, Window win));
typedef int (HandleCntrlSpaceFunc)ARGS_DECL((Display *dpy, Window win));
typedef int (DeactivateOnCreateTextFunc)ARGS_DECL((Display *dpy, Window win));
typedef int (HandleCreateTextFunc)ARGS_DECL((Display *dpy, Window win));
typedef int (TellCursorPosFunc)ARGS_DECL((Display *dpy, Window win,
                                          int cur_x, int cur_y));
typedef int (ExpectNextEventFunc)ARGS_DECL((Display *dpy, Window win));
typedef int (HandleNextEventFunc)ARGS_DECL((Display *dpy, Window win,
                                            XEvent *ev));
typedef int (HandleNewCurTextFunc)ARGS_DECL((Display *dpy, Window win));
typedef int (ExpectLookupStringFunc)ARGS_DECL((Display *dpy, Window win));
typedef int (HandleLookupStringFunc)ARGS_DECL((Display *dpy, Window win,
                                               XKeyEvent *key_ev, char *psz_buf,
                                               int buf_size, KeySym *p_key_sym,
                                               int *pn_has_ch));

typedef struct tagIMInfo {
   char *pszName;
   int nInputMethod;
   int nSingleDouble;
   CleanUpFunc *pCleanUpFunc;
   InitFunc *pInitFunc;
   FocusInFunc *pFocusInFunc;
   FocusOutFunc *pFocusOutFunc;
   SendKeyFunc *pSendKeyFunc;
   ConvPropertyFunc *pConvPropertyFunc;
   ExpectCMFunc *pExpectCMFunc;
   HandleCMFunc *pHandleCMFunc;
   ActivateOnCntrlSpaceFunc *pActivateOnCntrlSpaceFunc;
   HandleCntrlSpaceFunc *pHandleCntrlSpaceFunc;
   DeactivateOnCreateTextFunc *pDeactivateOnCreateTextFunc;
   HandleCreateTextFunc *pHandleCreateTextFunc;
   TellCursorPosFunc *pTellCursorPosFunc;
   ExpectNextEventFunc *pExpectNextEventFunc;
   HandleNextEventFunc *pHandleNextEventFunc;
   HandleNewCurTextFunc *pHandleNewCurTextFunc;
   ExpectLookupStringFunc *pExpectLookupStringFunc;
   HandleLookupStringFunc *pHandleLookupStringFunc;
} IMInfo;

static IMInfo *gpIMInfo=NULL;

/* --------------------- xcin Routines --------------------- */

#ifndef _NO_XCIN
static
int XcinInit(dpy, win, arg)
   Display *dpy;
   Window win;
   char *arg;
{
   return TRUE;
}

static
int XcinFocusIn(dpy, win)
   Display *dpy;
   Window win;
{
   Tg_send_FocusIn(dpy, win);
   return TRUE;
}

static
int XcinFocusOut(dpy, win)
   Display *dpy;
   Window win;
{
   Tg_send_FocusOut(dpy, win);
   return TRUE;
}

static
int XcinSendKey(dpy, win, key_ev, buf)
   Display *dpy;
   Window win;
   XKeyEvent *key_ev;
   char *buf;
{
   return Tg_send_key(dpy, win, key_ev, buf);
}

static
int XcinExpectCM(dpy, win)
   Display *dpy;
   Window win;
{
   return FALSE;
}
#endif /* ~_NO_XCIN */

/* --------------------- chinput Routines --------------------- */

#ifndef _NO_CHINPUT
static
int ChinputInit(dpy, win, arg)
   Display *dpy;
   Window win;
   char *arg;
{
   return (Tg_HZclientInit(dpy)==0);
}

static
int ChinputFocusIn(dpy, win)
   Display *dpy;
   Window win;
{
   return TRUE;
}

static
int ChinputFocusOut(dpy, win)
   Display *dpy;
   Window win;
{
   return TRUE;
}

static
int ChinputSendKey(dpy, win, key_ev, buf)
   Display *dpy;
   Window win;
   XKeyEvent *key_ev;
   char *buf;
{
   Tg_HZsendKey(dpy, win, key_ev);
   *buf = '\0';
   return TRUE;
}

static
int ChinputExpectCM(dpy, win)
   Display *dpy;
   Window win;
{
   return TRUE;
}

static
int ChinputHandleCM(dpy, win, cm_ev, key_ev, buf)
   Display *dpy;
   Window win;
   XClientMessageEvent *cm_ev;
   XKeyEvent *key_ev;
   char *buf;
{
   return Tg_HZhandleCM(cm_ev, key_ev, buf);
}
#endif /* ~_NO_CHINPUT */

/* --------------------- kinput Routines --------------------- */

#ifndef _NO_KINPUT
static
void KinputCleanUp(dpy, win)
   Display *dpy;
   Window win;
{
   imProtocol = IM_NONE;
}

static
int KinputInit(dpy, win, arg)
   Display *dpy;
   Window win;
   char *arg;
{
   char *c_ptr=NULL;

   copyAndPasteJIS = FALSE;
   if ((c_ptr=XGetDefault(mainDisplay, TOOL_NAME, "CopyAndPasteJIS")) != NULL &&
         UtilStrICmp(c_ptr, "true") == 0) {
      copyAndPasteJIS = TRUE;
   }
   gnOverTheSpot = FALSE;
   if ((c_ptr=XGetDefault(mainDisplay, TOOL_NAME, "PreeditType")) != NULL &&
         UtilStrICmp(c_ptr, "overthespot") == 0) {
      gnOverTheSpot = TRUE;
   }
   KinputSetConvOverSpot(gnOverTheSpot);

   if ((c_ptr=XGetDefault(mainDisplay, TOOL_NAME, "ConvSelection")) != NULL) {
      UtilTrimBlanks(c_ptr);
      strncpy(kinputConvSelName, c_ptr, MAXSTRING-1);
      kinputConvSelName[MAXSTRING-1] = '\0';
   }

   imProtocol = IM_KINPUT;
   return TRUE;
}

static
int KinputFocusIn(dpy, win)
   Display *dpy;
   Window win;
{
   return TRUE;
}

static
int KinputFocusOut(dpy, win)
   Display *dpy;
   Window win;
{
   return TRUE;
}

static
int KinputSendKey(dpy, win, key_ev, buf)
   Display *dpy;
   Window win;
   XKeyEvent *key_ev;
   char *buf;
{
   return FALSE;
}

static
int KinputConvProperty(dpy, win, prop_ev, ppsz_buf)
   Display *dpy;
   Window win;
   XPropertyEvent *prop_ev;
   char **ppsz_buf;
{
   char *c_ptr=KinputCheckConvProperty(dpy, win, prop_ev);

   if (c_ptr != NULL && ppsz_buf != NULL) {
      *ppsz_buf = c_ptr;
      return TRUE;
   }
   return FALSE;
}

static
int KinputExpectCM(dpy, win)
   Display *dpy;
   Window win;
{
   return (win == drawWindow ||
         (TidgetHasFocus() && win == TidgetGetFocusWindow()));
}

static
int KinputHandleCM(dpy, win, cm_ev, key_ev, buf)
   Display *dpy;
   Window win;
   XClientMessageEvent *cm_ev;
   XKeyEvent *key_ev;
   char *buf;
{
   KinputCheckClientMessage(dpy, win, cm_ev);
   return FALSE;
}

static
int KinputActiveOnCntrlSpace(dpy, win)
   Display *dpy;
   Window win;
{
   return (win == drawWindow ||
         (TidgetHasFocus() && win == TidgetGetFocusWindow()));
}

static
int KinputHandleCntrlSpace(dpy, win)
   Display *dpy;
   Window win;
{
   KinputBeginConversion(dpy, win);
   return TRUE;
}

static
int KinputDeactiveOnCreateText(dpy, win)
   Display *dpy;
   Window win;
{
   return TRUE;
}

static
int KinputHandleCreateText(dpy, win)
   Display *dpy;
   Window win;
{
   KinputEndConversion(dpy, win);
   return TRUE;
}

static
int KinputTellCursorPos(dpy, win, cur_x, cur_y)
   Display *dpy;
   Window win;
   int cur_x, cur_y;
{
   KinputTellCursorPosition(dpy, win, cur_x, cur_y);
   return TRUE;
}
#endif /* ~_NO_KINPUT */

/* --------------------- xim Routines --------------------- */

#ifndef _NO_XIM
static
void XimCleanUp(dpy, win)
   Display *dpy;
   Window win;
{
   XIMCleanUp();
}

static
int XimInit(dpy, win, arg)
   Display *dpy;
   Window win;
   char *arg;
{
   char *c_ptr=NULL;
   XIMInit();
   gnOverTheSpot = FALSE;
   if ((c_ptr=XGetDefault(mainDisplay, TOOL_NAME, "PreeditType")) != NULL &&
         UtilStrICmp(c_ptr, "overthespot") == 0) {
      gnOverTheSpot = TRUE;
   }
   XIMSetConvOverSpot(gnOverTheSpot);
   return TRUE;
}

static
int XimFocusIn(dpy, win)
   Display *dpy;
   Window win;
{
   XIMSetICFocus(dpy, win);
   return TRUE;
}

static
int XimFocusOut(dpy, win)
   Display *dpy;
   Window win;
{
   XIMUnsetICFocus(win);
   return TRUE;
}

static
int XimDeactiveOnCreateText(dpy, win)
   Display *dpy;
   Window win;
{
   return TRUE;
}

static
int XimHandleCreateText(dpy, win)
   Display *dpy;
   Window win;
{
   XIMUnsetICFocus(win);
   return TRUE;
}

static
int XimExpectNextEvent(dpy, win)
   Display *dpy;
   Window win;
{
   return (win == drawWindow ||
         (TidgetHasFocus() && win == TidgetGetFocusWindow()));
}

static
int XimHandleNextEvent(dpy, win, ev)
   Display *dpy;
   Window win;
   XEvent *ev;
{
   XIMNextEvent(dpy, win, ev);
   return TRUE;
}

static
int XimHandleNewCurText(dpy, win)
   Display *dpy;
   Window win;
{
   XIMSetICFocus(dpy, win);
   return TRUE;
}

static
int XimExpectLookupString(dpy, win)
   Display *dpy;
   Window win;
{
   return (win == drawWindow ||
         (TidgetHasFocus() && win == TidgetGetFocusWindow()));
}

static XIMStatus c_stat;

static
int XimHandleLookupString(dpy, win, key_ev, psz_buf, buf_size, p_key_sym,
      pn_has_ch)
   Display *dpy;
   Window win;
   XKeyEvent *key_ev;
   char *psz_buf;
   int buf_size;
   KeySym *p_key_sym;
   int *pn_has_ch;
{
   *pn_has_ch = XIMLookupString(key_ev, psz_buf, buf_size, p_key_sym, &c_stat);
   return TRUE;
}

static
int XimTellCursorPos(dpy, win, cur_x, cur_y)
   Display *dpy;
   Window win;
   int cur_x, cur_y;
{
   XIMTellCursorPosition(dpy, win, cur_x, cur_y);
   return TRUE;
}
#endif /* ~_NO_XIM */

/* --------------------- tgtwb5 Routines --------------------- */

#ifndef _NO_TGTWB5
static
void Tgtwb5CleanUp(dpy, win)
   Display *dpy;
   Window win;
{
   Tgtwb5_CleanUp(dpy, win);
}

static
int Tgtwb5Init(dpy, win, arg)
   Display *dpy;
   Window win;
   char *arg;
{
   return Tgtwb5_Init(dpy, win, arg);
}

static
int Tgtwb5SendKey(dpy, win, key_ev, buf)
   Display *dpy;
   Window win;
   XKeyEvent *key_ev;
   char *buf;
{
   return Tgtwb5_SendKey(dpy, win, key_ev, buf);
}

static
int Tgtwb5ExpectCM(dpy, win)
   Display *dpy;
   Window win;
{
   return FALSE;
}

static
int Tgtwb5ActiveOnCntrlSpace(dpy, win)
   Display *dpy;
   Window win;
{
   return (win == drawWindow ||
         (TidgetHasFocus() && win == TidgetGetFocusWindow()));
}

static
int Tgtwb5HandleCntrlSpace(dpy, win)
   Display *dpy;
   Window win;
{
   return Tgtwb5_HandleCntrlSpace(dpy, win);
}

static
int Tgtwb5DeactiveOnCreateText(dpy, win)
   Display *dpy;
   Window win;
{
   return TRUE;
}

static
int Tgtwb5HandleCreateText(dpy, win)
   Display *dpy;
   Window win;
{
   Tgtwb5_HandleCreateText(dpy, win);
   return TRUE;
}
#endif /* ~_NO_XCIN */

/* --------------------- Global Routines --------------------- */

static struct tagIMInfo gstIMInfo[] = {
#ifndef _NO_XCIN
   { "xcin", TGIM_XCIN, TGIM_DBIM,
     NULL, XcinInit, XcinFocusIn, XcinFocusOut,
     XcinSendKey, NULL, XcinExpectCM, NULL,
     NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
   },
#endif /* ~_NO_XCIN */
#ifndef _NO_CHINPUT
   { "chinput", TGIM_CHINPUT, TGIM_DBIM,
     NULL, ChinputInit, ChinputFocusIn, ChinputFocusOut,
     ChinputSendKey, NULL, ChinputExpectCM, ChinputHandleCM,
     NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
   },
#endif /* ~_NO_CHINPUT */
#ifndef _NO_KINPUT
   { "kinput2", TGIM_KINPUT, TGIM_DBIM,
     KinputCleanUp, KinputInit, KinputFocusIn, KinputFocusOut,
     KinputSendKey, KinputConvProperty, KinputExpectCM, KinputHandleCM,
     KinputActiveOnCntrlSpace, KinputHandleCntrlSpace,
     KinputDeactiveOnCreateText, KinputHandleCreateText,
     KinputTellCursorPos, NULL, NULL, NULL, NULL, NULL
   },
#endif /* ~_NO_KINPUT */
#ifndef _NO_XIM
   { "xim", TGIM_XIM, (TGIM_DBIM|TGIM_SBIM),
     XimCleanUp, XimInit, XimFocusIn, XimFocusOut,
     NULL, NULL, NULL, NULL, NULL, NULL,
     XimDeactiveOnCreateText, XimHandleCreateText,
     XimTellCursorPos, XimExpectNextEvent, XimHandleNextEvent,
     XimHandleNewCurText, XimExpectLookupString, XimHandleLookupString
   },
#endif /* ~_NO_XIM */
#ifndef _NO_TGTWB5
   { "tgtwb5/tgchgb", TGIM_TGTWB5, TGIM_DBIM,
     Tgtwb5CleanUp, Tgtwb5Init, NULL, NULL,
     Tgtwb5SendKey, NULL, Tgtwb5ExpectCM, NULL,
     Tgtwb5ActiveOnCntrlSpace, Tgtwb5HandleCntrlSpace,
     Tgtwb5DeactiveOnCreateText, Tgtwb5HandleCreateText,
     NULL, NULL, NULL, NULL, NULL, NULL
   },
#endif /* ~_NO_TGTWB5 */
   { NULL, TGIM_NONE, 0, NULL, NULL, NULL, NULL,
     NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
     NULL, NULL, NULL, NULL, NULL
   }
};

static void *gpLibIdnHandle=NULL;
static Tgim_stringprep_convert_Func *gpfn_stringprep_convert=NULL;

static
void CleanUpTgim_stringprep_convert()
{
   if (gpLibIdnHandle != NULL) {
      gpfn_stringprep_convert = NULL;
      CLOSE_DL(gpLibIdnHandle);
      gpLibIdnHandle = NULL;
   }
}

static
void InitTgim_stringprep_convert()
{
#if (!defined(_NO_IDNLIB) || defined(HAVE_LIBZ))
   gpLibIdnHandle = OPEN_DL("libidn.so", OPEN_DL_MODE);
   if (gpLibIdnHandle != NULL) {
      gpfn_stringprep_convert =
            (Tgim_stringprep_convert_Func*)GET_DL_SYM(gpLibIdnHandle,
            "stringprep_convert");
      if (gpfn_stringprep_convert == NULL) {
         CLOSE_DL(gpLibIdnHandle);
         gpLibIdnHandle = NULL;
      }
   }
#endif /* (!defined(_NO_IDNLIB) || defined(HAVE_LIBZ)) */
}

int Tgim_has_stringprep_convert()
{
#if (!defined(_NO_IDNLIB) || defined(HAVE_LIBZ))
   if (gpLibIdnHandle != NULL && gpfn_stringprep_convert != NULL) {
      return TRUE;
   }
#endif /* (!defined(_NO_IDNLIB) || defined(HAVE_LIBZ)) */
   return FALSE;
}

char *Tgim_stringprep_convert(const char *str, const char *to_codeset,
      const char *from_codeset)
{
#if (!defined(_NO_IDNLIB) || defined(HAVE_LIBZ))
   if (gpLibIdnHandle != NULL && gpfn_stringprep_convert != NULL) {
       return (gpfn_stringprep_convert)(str, to_codeset, from_codeset);
   }
#endif /* (!defined(_NO_IDNLIB) || defined(HAVE_LIBZ)) */
   return NULL;
}

void CleanUpInputMethods()
{
   CleanUpTgim_stringprep_convert();
   if (gnInputMethodIndex != INVALID &&
         gstIMInfo[gnInputMethodIndex].pCleanUpFunc != NULL) {
      (gstIMInfo[gnInputMethodIndex].pCleanUpFunc)(mainDisplay, mainWindow);
   }
   gnInputMethodIndex = INVALID;
   cmdLineHasInputMethod = FALSE;
   cmdLineDoubleByteInputMethod = FALSE;
   *cmdLineInputMethod = '\0';
   gpIMInfo = NULL;
   gnSingleOrDoubleByteInputMethod = 0;
}

int InitInputMethods()
{
   char *psz=NULL, single_double=0;

   gnInputMethod = gnInputMethodIndex = INVALID;
   if (cmdLineHasInputMethod) {
      UtilTrimBlanks(cmdLineInputMethod);
      psz = cmdLineInputMethod;
      single_double = (cmdLineDoubleByteInputMethod ? TGIM_DBIM : TGIM_SBIM);
   } else {
      psz = XGetDefault(mainDisplay, TOOL_NAME, "DoubleByteInputMethod");
      if (psz == NULL) {
         psz = XGetDefault(mainDisplay, TOOL_NAME, "SingleByteInputMethod");
         single_double = TGIM_SBIM;
      } else {
         if (XGetDefault(mainDisplay, TOOL_NAME, "SingleByteInputMethod") !=
               NULL) {
            sprintf(gszMsgBox, TgLoadString(STID_TWO_XDEF_ONE_IGNORED),
                  TOOL_NAME, "DoubleByteInputMethod", TOOL_NAME,
                  "SingleByteInputMethod", TOOL_NAME, "SingleByteInputMethod");
            Msg(gszMsgBox);
            fprintf(stderr, "%s\n", gszMsgBox);
         }
         single_double = TGIM_DBIM;
      }
   }
   gnSingleOrDoubleByteInputMethod = single_double;

   if (psz != NULL) {
      IMInfo *pIMInfo=gstIMInfo;
      char *psz_comma=strchr(psz, ',');
      int i=0;

      if (psz_comma != NULL) *psz_comma = '\0';
      for ( ; pIMInfo->pszName != NULL; pIMInfo++, i++) {
         char *psz_slash=strchr(pIMInfo->pszName,'/');

         if (psz_slash == NULL) {
            if (UtilStrICmp(psz, pIMInfo->pszName) == 0 &&
                  (single_double & pIMInfo->nSingleDouble) == single_double) {
               gnInputMethod = pIMInfo->nInputMethod;
               gnInputMethodIndex = i;
               gpIMInfo = pIMInfo;
               if (gstIMInfo[gnInputMethodIndex].pInitFunc != NULL) {
                  if (psz_comma == NULL) {
                     if (!((gstIMInfo[gnInputMethodIndex].pInitFunc)(mainDisplay,
                           mainWindow, NULL))) {
                        /* may not be a problem */
                     }
                  } else {
                     if (!((gstIMInfo[gnInputMethodIndex].pInitFunc)(mainDisplay,
                           mainWindow, &psz_comma[1]))) {
                        /* may not be a problem */
                     }
                  }
               }
               break;
            }
         } else {
            char *psz_dup=UtilStrDup(pIMInfo->pszName), *psz_cur=NULL;
            int done=FALSE;

            if (psz_dup == NULL) FailAllocMessage();
            psz_cur = psz_dup;
            psz_slash = strchr(psz_cur, '/');
            while (!done) {
               if (psz_slash != NULL) *psz_slash = '\0';
               if (UtilStrICmp(psz, psz_cur) == 0 &&
                     (single_double & pIMInfo->nSingleDouble) == single_double) {
                  gnInputMethod = pIMInfo->nInputMethod;
                  gnInputMethodIndex = i;
                  gpIMInfo = pIMInfo;
                  if (gstIMInfo[gnInputMethodIndex].pInitFunc != NULL) {
                     if (psz_comma == NULL) {
                        if (!((gstIMInfo[gnInputMethodIndex].pInitFunc)(mainDisplay,
                              mainWindow, NULL))) {
                           /* may not be a problem */
                        }
                     } else {
                        if (!((gstIMInfo[gnInputMethodIndex].pInitFunc)(mainDisplay,
                              mainWindow, &psz_comma[1]))) {
                           /* may not be a problem */
                        }
                     }
                  }
                  done = TRUE;
               }
               if (psz_slash != NULL) {
                  *psz_slash++ = '/';
                  psz_cur = psz_slash;
                  psz_slash = strchr(psz_cur, '/');
               } else {
                  break;
               }
            }
            UtilFree(psz_dup);
            if (done) {
               break;
            }
         }
      }
      if (psz_comma != NULL) *psz_comma++ = ',';

      if (gnInputMethod == INVALID) {
         if (single_double == TGIM_DBIM) {
            fprintf(stderr, TgLoadString(STID_DBIM_UNSUPPORTED), psz);
         } else {
            fprintf(stderr, TgLoadString(STID_SBIM_UNSUPPORTED), psz);
         }
         fprintf(stderr, "\n");
      }
   }
   if (gnSingleOrDoubleByteInputMethod == TGIM_SBIM &&
         gnInputMethod != INVALID) {
      InitTgim_stringprep_convert();
   }
   return TRUE;
}

int ResetInputMethod()
{
   int rc=FALSE;

   if (gpIMInfo == NULL) return FALSE;

   sprintf(gszMsgBox, TgLoadString(STID_OK_TO_RESET_GIVEN_IM),
         gpIMInfo->pszName);
   if (MsgBox(gszMsgBox, TOOL_NAME, YNC_MB) != MB_ID_YES) {
      return FALSE;
   }
   CleanUpInputMethods();
   rc = InitInputMethods();

   sprintf(gszMsgBox, TgLoadString(STID_GIVEN_IM_RESETED), gpIMInfo->pszName);
   Msg(gszMsgBox);

   return rc;
}

int InputMethodTypeMatched(double_byte)
   int double_byte;
{
   if (gnInputMethodIndex == INVALID || gpIMInfo == NULL) return FALSE;

   if (double_byte) {
      return (gnSingleOrDoubleByteInputMethod == TGIM_DBIM &&
            (gpIMInfo->nSingleDouble & TGIM_DBIM) == TGIM_DBIM);
   } else {
      return (gnSingleOrDoubleByteInputMethod == TGIM_SBIM &&
            (gpIMInfo->nSingleDouble & TGIM_SBIM) == TGIM_SBIM);
   }
}

int tgIMFocusIn(dpy, win)
   Display *dpy;
   Window win;
{
   if (gnInputMethodIndex == INVALID ||
         gstIMInfo[gnInputMethodIndex].pFocusInFunc == NULL) {
      return FALSE;
   }
   return ((gstIMInfo[gnInputMethodIndex].pFocusInFunc)(dpy, win));
}

int tgIMFocusOut(dpy, win)
   Display *dpy;
   Window win;
{
   if (gnInputMethodIndex == INVALID ||
         gstIMInfo[gnInputMethodIndex].pFocusOutFunc == NULL) {
      return FALSE;
   }
   return ((gstIMInfo[gnInputMethodIndex].pFocusOutFunc)(dpy, win));
}

int tgIMTranslateKeyEvent(dpy, win, key_ev, buf)
   Display *dpy;
   Window win;
   XKeyEvent *key_ev;
   char *buf;
{
   if (gnInputMethodIndex == INVALID ||
         gstIMInfo[gnInputMethodIndex].pSendKeyFunc == NULL) {
      return FALSE;
   }
   return ((gstIMInfo[gnInputMethodIndex].pSendKeyFunc)(dpy, win, key_ev, buf));
}

int tgIMConvertProperty(dpy, win, prop_ev, ppsz_buf)
   Display *dpy;
   Window win;
   XPropertyEvent *prop_ev;
   char **ppsz_buf;
{
   if (gnInputMethodIndex == INVALID ||
         gstIMInfo[gnInputMethodIndex].pConvPropertyFunc == NULL) {
      return FALSE;
   }
   return ((gstIMInfo[gnInputMethodIndex].pConvPropertyFunc)(dpy, win, prop_ev,
         ppsz_buf));
}

int tgIMExpectClientMessage(dpy, win)
   Display *dpy;
   Window win;
{
   if (gnInputMethodIndex == INVALID ||
         gstIMInfo[gnInputMethodIndex].pExpectCMFunc == NULL) {
      return FALSE;
   }
   return ((gstIMInfo[gnInputMethodIndex].pExpectCMFunc)(dpy, win));
}

int tgIMHandleClientMessage(dpy, win, cm_ev, key_ev, buf)
   Display *dpy;
   Window win;
   XClientMessageEvent *cm_ev;
   XKeyEvent *key_ev;
   char *buf;
{
   if (gnInputMethodIndex == INVALID ||
         gstIMInfo[gnInputMethodIndex].pHandleCMFunc == NULL) {
      return FALSE;
   }
   return ((gstIMInfo[gnInputMethodIndex].pHandleCMFunc)(dpy, win, cm_ev,
         key_ev, buf));
}

int tgIMActiveOnCntrlSpace(dpy, win)
   Display *dpy;
   Window win;
{
   if (gnInputMethodIndex == INVALID ||
         gstIMInfo[gnInputMethodIndex].pActivateOnCntrlSpaceFunc == NULL) {
      return FALSE;
   }
   return ((gstIMInfo[gnInputMethodIndex].pActivateOnCntrlSpaceFunc)(dpy, win));
}

int tgIMHandleCntrlSpace(dpy, win)
   Display *dpy;
   Window win;
{
   if (gnInputMethodIndex == INVALID ||
         gstIMInfo[gnInputMethodIndex].pHandleCntrlSpaceFunc == NULL) {
      return FALSE;
   }
   return ((gstIMInfo[gnInputMethodIndex].pHandleCntrlSpaceFunc)(dpy, win));
}

int tgIMDeactiveOnCreateText(dpy, win)
   Display *dpy;
   Window win;
{
   if (gnInputMethodIndex == INVALID ||
         gstIMInfo[gnInputMethodIndex].pDeactivateOnCreateTextFunc == NULL) {
      return FALSE;
   }
   return ((gstIMInfo[gnInputMethodIndex].pDeactivateOnCreateTextFunc)(dpy,
         win));
}

int tgIMHandleCreateText(dpy, win)
   Display *dpy;
   Window win;
{
   if (gnInputMethodIndex == INVALID ||
         gstIMInfo[gnInputMethodIndex].pHandleCreateTextFunc == NULL) {
      return FALSE;
   }
   return ((gstIMInfo[gnInputMethodIndex].pHandleCreateTextFunc)(dpy, win));
}

int tgIMTellCursorPosition(dpy, win, cur_x, cur_y)
   Display *dpy;
   Window win;
   int cur_x, cur_y;
{
   if (gnInputMethodIndex == INVALID ||
         gstIMInfo[gnInputMethodIndex].pTellCursorPosFunc == NULL) {
      return FALSE;
   }
   return ((gstIMInfo[gnInputMethodIndex].pTellCursorPosFunc)(dpy, win,
         cur_x, cur_y));
}

int tgIMExpectNextEvent(dpy, win)
   Display *dpy;
   Window win;
{
   if (gnInputMethodIndex == INVALID ||
         gstIMInfo[gnInputMethodIndex].pExpectNextEventFunc == NULL) {
      return FALSE;
   }
   return ((gstIMInfo[gnInputMethodIndex].pExpectNextEventFunc)(dpy, win));
}

int tgIMHandleNextEvent(dpy, win, ev)
   Display *dpy;
   Window win;
   XEvent *ev;
{
   if (gnInputMethodIndex == INVALID ||
         gstIMInfo[gnInputMethodIndex].pHandleNextEventFunc == NULL) {
      return FALSE;
   }
   return ((gstIMInfo[gnInputMethodIndex].pHandleNextEventFunc)(dpy, win, ev));
}

int tgIMHandleNewCurText(dpy, win)
   Display *dpy;
   Window win;
{
   if (gnInputMethodIndex == INVALID ||
         gstIMInfo[gnInputMethodIndex].pHandleNewCurTextFunc == NULL) {
      return FALSE;
   }
   return ((gstIMInfo[gnInputMethodIndex].pHandleNewCurTextFunc)(dpy, win));
}

int tgIMExpectLookupString(dpy, win)
   Display *dpy;
   Window win;
{
   if (gnInputMethodIndex == INVALID ||
         gstIMInfo[gnInputMethodIndex].pExpectLookupStringFunc == NULL) {
      return FALSE;
   }
   return ((gstIMInfo[gnInputMethodIndex].pExpectLookupStringFunc)(dpy, win));
}

int tgIMHandleLookupString(dpy, win, key_ev, psz_buf, buf_size, p_key_sym,
      pn_has_ch)
   Display *dpy;
   Window win;
   XKeyEvent *key_ev;
   char *psz_buf;
   int buf_size;
   KeySym *p_key_sym;
   int *pn_has_ch;
{
   if (gnInputMethodIndex == INVALID ||
         gstIMInfo[gnInputMethodIndex].pHandleLookupStringFunc == NULL) {
      return FALSE;
   }
   return ((gstIMInfo[gnInputMethodIndex].pHandleLookupStringFunc)(dpy, win,
         key_ev, psz_buf, buf_size, p_key_sym, pn_has_ch));
}

