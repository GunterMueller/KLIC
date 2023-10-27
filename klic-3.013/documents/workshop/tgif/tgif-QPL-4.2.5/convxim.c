/*
 * Author:	Yoichiro Ueno (ueno@cs.titech.ac.jp)
 *
 * Copyright (C) 1991, 1992, Yoichiro Ueno.
 *
 * Permission to use, copy, modify, and distribute this software and
 * its documentation for any purpose is hereby granted by the Author without
 * fee, provided that the above copyright notice appear in all copies and
 * that both the copyright notice and this permission notice appear in
 * supporting documentation, and that the name of the Author not be used
 * in advertising or publicity pertaining to distribution of the software
 * without specific, written prior permission.  The Author makes no
 * representations about the suitability of this software for any purpose.
 * It is provided "as is" without express or implied warranty.
 *
 * THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/convxim.c,v 1.5 2009/08/07 03:41:16 william Exp $
 */
#ifndef _NO_XIM

#define _INCLUDE_FROM_CONVXIM_C_

#include "tgifdefs.h"

#include "convkinput.e"
#include "convxim.e"
#include "dialog.e"
#include "font.e"
#include "list.e"
#include "miniline.e"
#include "msg.e"
#include "setup.e"
#include "strtbl.e"
#include "util.e"

#ifndef _NO_XIMP
#define DEFAULT_MODIFIERS	""
#define DEFAULT_LANG		"ja_JP.EUC"
#else /* _NO_XIMP */
#define DEFAULT_MODIFIERS	"@im=_XWNMO"
#define DEFAULT_LANG		"ja_JP.ujis"
#endif /* ~_NO_XIMP */

typedef struct tagXICInfo {
   XIC ic;
   Window win;
} XICInfo;

static CVList gXICInfoList;

char	ximConvModifiers[MAXSTRING] = DEFAULT_MODIFIERS;
char	ximConvLang[MAXSTRING] = DEFAULT_LANG;

static char	*locale = NULL;
static char	*modifiers = NULL;
static XIM	im = NULL;
static XIC	ic = NULL;

typedef void (MYIOErrHandler)ARGS_DECL((void));

static MYIOErrHandler	*oldhandler = NULL;
static Bool		_XIMErrorFlag = False;
static Bool		overthespot = False;

static Bool	modscim = False;
static int	pre_x = 0;
static int	pre_y = 0;

#ifndef _NO_XIMP
static MYIOErrHandler *_XipSetIOErrorHandler(new_handler)
    MYIOErrHandler *new_handler;
{
    return NULL;
}
#else /* _NO_XIMP */
extern void	(*_XipSetIOErrorHandler())();
#endif /* ~_NO_XIMP */

/* --------------------- Utility Functions --------------------- */

static
CVListElem *FindXIC(win)
   Window win;
{
    CVListElem *pElem=NULL;

    for (pElem=ListFirst(&gXICInfoList); pElem != NULL;
            pElem=ListNext(&gXICInfoList, pElem)) {
        XICInfo *pxi=(XICInfo*)(pElem->obj);

        if (pxi->win == win) {
           return pElem;
        }
    }
    return NULL;
}

static
void DeleteXICInfo(pElem)
    CVListElem *pElem;
{
    XICInfo *pxi=(XICInfo*)(pElem->obj);

    if (pxi->ic != NULL) XDestroyIC(pxi->ic);

    ListUnlink(&gXICInfoList, pElem);
    free(pElem);
}

/* --------------------- Exported Functions --------------------- */

static
void _XIMIOError()
{
    _XIMErrorFlag = True;
}

static
void XIMClose(win)
   Window win;
{
    CVListElem *pElem=FindXIC(win);

    if (pElem != NULL) {
        DeleteXICInfo(pElem);
    }
}

#define ROOT 1
#define OVERTHESPOT 2
static int style_type=ROOT;
XFontSet XIMfs;
XFontSetExtents * fs_ext=NULL;
char** missing_list=NULL;
int missing_count=0;
char* def_string=NULL;
char* XIMFontSetStr="-misc-fixed-medium-r-normal--14-*";

static
void XIMSetLocale()
{
#ifndef _NO_LOCALE_SUPPORT
    char buf[80], *c_ptr=NULL;

    c_ptr = (char*)getenv("LC_ALL");
    if (c_ptr != NULL) {
        UtilStrCpyN(ximConvLang, sizeof(ximConvLang), c_ptr);
    } else {
        c_ptr = (char*)getenv("LANG");
        if (c_ptr != NULL) {
            UtilStrCpyN(ximConvLang, sizeof(ximConvLang), c_ptr);
        } else if ((c_ptr=XGetDefault(mainDisplay, TOOL_NAME, "Lang")) !=
                NULL) {
            UtilTrimBlanks(c_ptr);
            UtilStrCpyN(ximConvLang, sizeof(ximConvLang), c_ptr);
        }
    }
    locale = setlocale(LC_ALL, ximConvLang);
    sprintf(buf, TgLoadString(STID_LOCALE_IS),
            locale==NULL ? TgLoadCachedString(CSTID_PARANED_NULL) : locale);
    Msg(buf);

    c_ptr = (char*)getenv("LC_CTYPE");
    if (c_ptr != NULL) {
        UtilStrCpyN(buf, sizeof(buf), c_ptr);
        locale = setlocale(LC_CTYPE, buf);
#ifdef _TGIF_DBG /* debug, do not translate */
        sprintf(buf, "LC_CTYPE is '%s'.", locale==NULL ? "null" : locale);
        Msg(buf);
#endif /* _TGIF_DBG */
    }
    setlocale(LC_NUMERIC, "C");
#endif /* ~_NO_LOCALE_SUPPORT */
}

void XIMSetICFocus(dpy, win)
    Display *dpy;
    Window win;
{
    char	buf[80];
    XIMStyles *    styles=NULL;
    Bool	style_flag;
    int		style_attr=0;
    int	i;
    XPoint spot;
    XRectangle s_rect;
    XVaNestedList preedit_attr, status_attr;
    CVListElem *pElem=NULL;

    if(oldhandler == NULL)
	oldhandler = _XipSetIOErrorHandler(_XIMIOError);

    if(locale == NULL) {
        XIMSetLocale();
    }
    if(modifiers == NULL) {
	char *c_ptr=(char*)getenv("XMODIFIERS");

	if (c_ptr != NULL) {
	    UtilStrCpyN(ximConvModifiers, sizeof(ximConvModifiers), c_ptr);
	} else {
            if ((c_ptr=XGetDefault(mainDisplay, TOOL_NAME, "Modifiers")) !=
                    NULL) {
	        UtilTrimBlanks(c_ptr);
	        UtilStrCpyN(ximConvModifiers, sizeof(ximConvModifiers), c_ptr);
            }
	}
	modifiers = XSetLocaleModifiers(ximConvModifiers);
	sprintf(buf, TgLoadString(STID_MODIFIER_IS),
		modifiers==NULL ? TgLoadCachedString(CSTID_PARANED_NULL) :
		modifiers);
	Msg(buf);
    }

    for (i = 0; i <= (int)strlen(modifiers) - 4 ; i++) {
        if (strncmp((char *)&modifiers[i], "SCIM", 4) == 0 ||
                strncmp((char *)&modifiers[i], "scim", 4) == 0) {
            modscim = True;
            break;
        }
    }

    if(im == NULL) {
	im = XOpenIM(mainDisplay, NULL, NULL, NULL);
	if(im == NULL) {
	    MsgBox(TgLoadString(STID_CANNOT_OPEN_XIM), TOOL_NAME, INFO_MB);
	    return;
	}
    }
    pElem = FindXIC(win);
    if (pElem != NULL) {
        XICInfo *pxi=(XICInfo*)(pElem->obj);

        ic = pxi->ic;
    } else {
        XICInfo *pxi=(XICInfo*)malloc(sizeof(XICInfo));

        if (pxi == NULL) FailAllocMessage();
        memset(pxi, 0, sizeof(XICInfo));
        if (!ListAppend(&gXICInfoList, pxi)) {
        }
        pxi->win = win;

	XGetIMValues(im,
		XNQueryInputStyle, &styles,
		NULL, NULL);
	style_flag = False;
	if (overthespot) {
	  for(i = 0; i < (int)(styles->count_styles); i ++){
	    if((styles->supported_styles[i] ==
	       (XIMPreeditPosition | XIMStatusArea)) ||
               (styles->supported_styles[i] ==
               (XIMPreeditPosition | XIMStatusNothing))){
	      style_flag = True;
	      style_type = OVERTHESPOT;
	      style_attr = styles->supported_styles[i];
	      Msg(TgLoadString(STID_OVERTHESPOT_CONV));
	    }
	  }
	}else{
	  for(i = 0; i < (int)(styles->count_styles); i ++){
	    if(styles->supported_styles[i] ==
	       (XIMPreeditNothing | XIMStatusNothing)){
	      style_flag = True;
	      style_type = ROOT;
	      Msg(TgLoadString(STID_ROOT_CONV));
	    }
	  }
	}
	if(!style_flag) {
	    sprintf(gszMsgBox, TgLoadString(STID_IM_NOT_SUPPORT_GIVEN_STYLE),
	          (overthespot ? "OverTheSpot" : "Root"));
	    MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
	    XCloseIM(im);
	    im = NULL;
	    return;
	}
	switch (style_type){
	case ROOT:
	ic = XCreateIC(im,
		XNInputStyle , XIMPreeditNothing | XIMStatusNothing,
		XNClientWindow, win,
		XNFocusWindow, win,
		NULL, NULL);
	break;
	case OVERTHESPOT:
	XIMfs=XCreateFontSet(dpy, XIMFontSetStr,
			  &missing_list, &missing_count, &def_string);
	fs_ext=XExtentsOfFontSet(XIMfs);
	spot.x=0; spot.y=fs_ext->max_logical_extent.height;
	s_rect.x=0; s_rect.y=fs_ext->max_logical_extent.height;
	s_rect.width=640; s_rect.height=fs_ext->max_logical_extent.height;
	preedit_attr=XVaCreateNestedList(0,
					 XNSpotLocation, &spot,
					 XNFontSet, XIMfs,
					 NULL);
	status_attr=XVaCreateNestedList(0,
					XNArea, &s_rect,
					XNFontSet, XIMfs,
					NULL);
	ic = XCreateIC(im,
	        XNInputStyle , style_attr,
		XNClientWindow, win,
		XNFocusWindow, win,
		XNPreeditAttributes, preedit_attr,
		XNStatusAttributes, status_attr,
		NULL, NULL);
	XFree(preedit_attr);
	XFree(status_attr);
	break;
	}
	if(ic == NULL) {
	    fprintf(stderr, "%s\n",
		TgLoadString(STID_FAIL_TO_CREATE_INPUTCONTEXT));
	    XCloseIM(im);
	    im = NULL;
	    return;
	}
	pxi->ic = ic;
	Msg(TgLoadString(STID_OPEN_XIM));
    }
    XSetICFocus(ic);

    if(_XIMErrorFlag) {
	XIMClose(win);
	Msg(TgLoadString(STID_CLOSE_XIM));
    }
}

void XIMNextEvent(dpy, win, xev_p)
Display *	dpy;
Window		win;
XEvent *	xev_p;
{
    Bool	filter_status;

    do {
	XNextEvent(dpy, xev_p);

	/*
	 * The 2nd argument has been changed from win to None
	 *         according to Ambrose Li's investigation.  See his
	 *         e-mail on "Sun, 09 Mar 2003 19:09:38 EST".
	 */
	filter_status = XFilterEvent(xev_p, None);

	if(_XIMErrorFlag) {
	    XIMClose(win);
	    Msg(TgLoadString(STID_CLOSE_XIM));
	}
    } while(filter_status == True);
}

int XIMLookupString(key_ev, buf, buf_len, key_sym_p, c_stat_p)
XKeyEvent *	key_ev;
char *		buf;
int		buf_len;
KeySym *	key_sym_p;
XIMStatus *	c_stat_p;
{
    int has_ch=0;

    if(im && ic) {
	c_stat_p->valid = MB_STATUS;
	has_ch = XmbLookupString (ic, key_ev, buf, buf_len, key_sym_p, &c_stat_p->mb_status);
/*
 *      switch (c_stat_p->mb_status) {
 *      case XLookupKeySym:
 *      case XLookupBoth:
 *          {
 *              unsigned int key_sym=(unsigned int)(*key_sym_p);
 *
 *              if (key_sym < 0x100) {
 *                 has_ch = 1;
 *                 *buf = (char)(unsigned char)(key_sym & 0x0ff);
 *                 buf[1] = '\0';
 *              }
 *          }
 *          break;
 *      default: break;
 *      }
 */
    }
    else {
	c_stat_p->valid = COMPOSE_STATUS;
	has_ch = XLookupString (key_ev, buf, buf_len, key_sym_p, &c_stat_p->status);
    }
    return has_ch;
}

void XIMUnsetICFocus(win)
    Window win;
{
    CVListElem *pElem=FindXIC(win);

    if (pElem != NULL) {
        XICInfo *pxi=(XICInfo*)(pElem->obj);

        ic = pxi->ic;
    }
    if(im && ic) {
	XUnsetICFocus(ic);
    }
}

void XIMSetConvOverSpot(gnOverTheSpot)
Bool gnOverTheSpot;
{
  overthespot=gnOverTheSpot;
}

void XIMTellCursorPosition(dpy, win, x, y)
Display* dpy;
Window win;
int x;
int y;
{
  XPoint spot;
  XRectangle s_rect;
  XVaNestedList preedit_attr,status_attr;
  char     szAttemptedFontName[MAXSTRING+1];
  CVListElem *pElem=FindXIC(win);

  if (pElem != NULL) {
    XICInfo *pxi=(XICInfo*)(pElem->obj);

    ic = pxi->ic;
  }
  if (im == NULL || ic == NULL || !overthespot) return;

  if (x == pre_x && y == pre_y) return;
  pre_x = x; pre_y = y;

  if (XIMfs) XFreeFontSet(dpy, XIMfs);
  GetCurFontInfoStr(szAttemptedFontName, sizeof(szAttemptedFontName));
  XIMfs=XCreateFontSet(dpy, szAttemptedFontName,
		    &missing_list, &missing_count, &def_string);
  fs_ext=XExtentsOfFontSet(XIMfs);
  spot.x=textCurX; spot.y=textCurBaselineY;
  s_rect.x=textCurX; s_rect.y=textCurBaselineY+3;
  /*s_rect.width=200; s_rect.height=fs_ext->max_logical_extent.height;*/
  s_rect.width=50; s_rect.height=16;

  preedit_attr=XVaCreateNestedList(0,
				   XNSpotLocation, &spot,
				   XNFontSet, XIMfs,
				   NULL);
  status_attr=XVaCreateNestedList(0,
				  XNArea, &s_rect,
				  NULL);
  XSetICValues(ic,
	       XNPreeditAttributes, preedit_attr,
	       XNStatusAttributes, status_attr,
	       NULL);
  XFree(preedit_attr);
  XFree(status_attr);
  if (!modscim) {
    XSetICFocus(ic);
  }
}

void XIMCleanUp()
{
  CVListElem *pElem=NULL;

  for (pElem=ListFirst(&gXICInfoList); pElem != NULL;
      pElem=ListNext(&gXICInfoList, pElem)) {
    XICInfo *pxi=(XICInfo*)(pElem->obj);

    if (pxi->ic != NULL) XDestroyIC(pxi->ic);
    free(pxi);
  }
  ListUnlinkAll(&gXICInfoList);

  if(im != NULL) {
    XCloseIM(im);
  }
  im = NULL;
  ic = NULL;
  _XIMErrorFlag = False;
}

int XIMInit()
{
  im = NULL;
  ic = NULL;
  _XIMErrorFlag = False;
  CVListInit(&gXICInfoList);

  return TRUE;
}

#endif /* ~_NO_XIM */

