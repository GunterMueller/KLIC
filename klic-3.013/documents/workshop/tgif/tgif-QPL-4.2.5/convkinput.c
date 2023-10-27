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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/convkinput.c,v 1.2 2009/09/28 22:39:48 william Exp $
 */

#ifndef _NO_KINPUT

#define _INCLUDE_FROM_CONVKINPUT_C_

#include "tgifdefs.h"

#include "choice.e"
#include "convkinput.e"
#include "dialog.e"
#include "msg.e"
#include "strtbl.e"
#include "text.e"

/* from kinput2/include/ConvProto.h */
#define CONV_ATTR(code,len)     ((unsigned long)((code)<<16)+(len))
#define CONVATTR_INDIRECT               1
#define CONVATTR_SPOT_LOCATION          3
#define CONVATTR_INPUT_STYLE            128
#define CONVARG_OVERTHESPOT             4L

#define CONV_OFF		0
#define WAIT_CONV_ON		1
#define CONV_ON			2
#define WAIT_CONV_OFF		3

#define KANJI0			0
#define KANJI1			1
#define KANJI2			2
#define KANJI3			3
#define KANJI4			4
#define KANJI5			5

#define LATIN			0
#define ESC			1
#define ESC_24			2
#define ESC_28			3
#define ESC_GL			4
#define ESC_GR			5
#define KANJI_GL		6
#define KANJI_GR		7

/* do not translate -- program constants */
#define KINPUT2_SELECTION	"_JAPANESE_CONVERSION"
#define KINPUT_SELECTION	"JAPANESE_CONVERSION"
#define TARGET			"COMPOUND_TEXT"
#define CONV_REQUEST		"CONVERSION_REQUEST"
#define CONV_NOTIFY		"CONVERSION_NOTIFY"
#define CONV_END		"CONVERSION_END"
#define CONV_END_REQ		"CONVERSION_END_REQUEST"

static int		conv_mode = CONV_OFF;
static Atom		conv_selection = None;
static Window		conv_owner_win = None;
static Atom		conv_target = None;
static Atom		conv_property = None;
static Atom		conv_req = None;
static Atom		conv_notify = None;
static Atom		conv_end = None;
static Atom		conv_end_req = None;

static Atom conv_attr = None;
static Atom conv_attr_notify = None;

static Window		conv_win = None;

int	imProtocol = IM_NONE;
int	copyAndPasteJIS = FALSE;
char	kinputConvSelName[MAXSTRING] = KINPUT2_SELECTION;

static int	kinputConvOverSpot = 0;

void KinputSetConvOverSpot(nConvOverSpot)
    int nConvOverSpot;
{
    kinputConvOverSpot = nConvOverSpot;
}

/* for over-the-spot: from convlib.c in kinput2/client */
void KinputTellCursorPosition(dpy, win, x, y)
  Display *dpy;
  Window win;
  int x, y;
{
    Window			owner_win;
    char			msg[80];
    char *			str1;
    XClientMessageEvent	xcme;
    unsigned long data[4];
    int len = 0;

    if(imProtocol != IM_KINPUT)
	return;

    if (conv_attr == None || conv_attr_notify == None)
	return;

#if 0
    switch(conv_mode) {
      case WAIT_CONV_ON :
	Msg(TgLoadString(STID_WAIT_FOR_START_CONV));
	return;
      case WAIT_CONV_OFF :
      case CONV_OFF :
	return;
    }
#endif

    if((owner_win = XGetSelectionOwner(dpy, conv_selection))
       == None) {
	str1 = XGetAtomName(dpy, conv_selection);
	sprintf(msg, TgLoadString(STID_NO_NAMED_SELECTION_OWNER), str1);
	Msg(msg);
	XFree(str1);
	conv_owner_win = None;
	conv_mode = CONV_OFF;
	return;
    }

    if(conv_owner_win != owner_win)
	return;

    data[len++] = CONV_ATTR(CONVATTR_SPOT_LOCATION, 1);
    data[len++] = (x << 16) | (y & 0xffff);

    data[len++] = CONV_ATTR(CONVATTR_INPUT_STYLE, 1);
    data[len++] = CONVARG_OVERTHESPOT;

    XChangeProperty(dpy, win,
		    conv_attr, conv_attr, 32,
		    PropModeReplace, (unsigned char *)data, len);

    xcme.type = ClientMessage;
    xcme.display = dpy;
    xcme.window = owner_win;
    xcme.message_type = conv_attr_notify;
    xcme.format = 32;
    xcme.data.l[0] = conv_selection;
    xcme.data.l[1] = win;
    xcme.data.l[2] = CONV_ATTR(CONVATTR_INDIRECT, 1);
    xcme.data.l[3] = conv_attr;
    if (XSendEvent(dpy, owner_win, False, NoEventMask, (XEvent *)&xcme)
	== 0) {
	MsgBox(TgLoadString(STID_FAIL_TO_CHANGE_CONV_PROP), TOOL_NAME, INFO_MB);
	return;
    }
}

static
Atom InternAtom(dpy, name)
Display	*dpy;
char *	name;
{
	Atom	ret;
	char	msg[80];

	if((ret = XInternAtom(dpy, name, False)) == None) {
		sprintf(msg, TgLoadString(STID_FAIL_TO_INTERN_NAMED_ATOM),
			name);
		MsgBox(msg, TOOL_NAME, INFO_MB);
	}

	return ret;
}

void KinputBeginConversion (dpy, win)
	Display			*dpy;
	Window			win;
{
	XClientMessageEvent	xcme;
	Window			owner_win;
	char			msg[80];
	char *			str1;
	int			curX, curY;

	if(imProtocol != IM_KINPUT)
		return;

#ifdef DEBUG /* debug, do not translate */
	fprintf(stderr, "KinputBeginConversion() : %d\n", conv_mode);
#endif
	if(curChoice != DRAWTEXT || !textCursorShown)
		return;

	if(conv_selection == None &&
			(conv_selection = InternAtom(dpy, kinputConvSelName)) == None)
		return;

	if(conv_target == None && (conv_target = InternAtom(dpy, TARGET)) == None)
		return;

#if 1 /* which better? */
	if(conv_mode == CONV_OFF)
#else
	if(conv_mode != CONV_ON)
#endif
	conv_property = None;

	if(conv_req == None && (conv_req = InternAtom(dpy, CONV_REQUEST)) == None)
		return;

	if(conv_notify == None &&
			(conv_notify = InternAtom(dpy, CONV_NOTIFY)) == None)
		return;

	if(conv_end == None && (conv_end = InternAtom(dpy, CONV_END)) == None)
		return;

	if(conv_end_req == None &&
			(conv_end_req = InternAtom(dpy, CONV_END_REQ)) == None)
		return;

  if (kinputConvOverSpot) {
	if(conv_attr == None &&
	   (conv_attr = InternAtom(dpy, "CONVERSION_ATTRIBUTE")) == None)
		return;

	if(conv_attr_notify == None &&
	   (conv_attr_notify = InternAtom(dpy, "CONVERSION_ATTRIBUTE_NOTIFY")) == None)
		return;
  }

	if((owner_win = XGetSelectionOwner(dpy, conv_selection))
			== None) {
		str1 = XGetAtomName(dpy, conv_selection);
		sprintf(msg, TgLoadString(STID_NO_NAMED_SELECTION_OWNER), str1);
		MsgBox(msg, TOOL_NAME, INFO_MB);
		XFree(str1);
		conv_owner_win = None;
		conv_mode = CONV_OFF;
#ifdef DEBUG /* debug, do not translate */
		fprintf(stderr, "0x%08x\n", owner_win);
#endif
		return;
	}
#ifdef DEBUG /* debug, do not translate */
	fprintf(stderr, "0x%08x\n", owner_win);
#endif

	switch(conv_mode) {
	    case WAIT_CONV_ON :
		Msg(TgLoadString(STID_WAIT_FOR_START_CONV));
		return;
	    case CONV_ON :
		if (conv_owner_win == owner_win)
		    return;
		break;
	}

	xcme.type = ClientMessage;
	xcme.display = dpy;
	xcme.window = owner_win;
	xcme.message_type = conv_req;
	xcme.format = 32;
	xcme.data.l[0] = conv_selection;
	xcme.data.l[1] = win;
	xcme.data.l[2] = conv_target;
	xcme.data.l[3] = conv_property;
	xcme.data.l[4] = conv_attr;
	if(XSendEvent(dpy, owner_win, False, NoEventMask, (XEvent *)&xcme)
			== 0) {
		MsgBox(TgLoadString(STID_FAIL_TO_CONNECT_TO_OWNER), TOOL_NAME,
			INFO_MB);
		return;
	}

	conv_owner_win = owner_win;
	conv_mode = WAIT_CONV_ON;

    	TellTextCursorPosition(&curX, &curY);
    	KinputTellCursorPosition(dpy, win, curX, curY);
}

void KinputCheckClientMessage (dpy, win, xclient)
Display			*dpy;
Window			win;
XClientMessageEvent *	xclient;
{
	if(imProtocol != IM_KINPUT)
		return;

	if(xclient->message_type == conv_notify) {
#ifdef DEBUG /* debug, do not translate */
		fprintf(stderr, "KinputCheckClientMessage() : conv_notify : %d\n", conv_mode);
#endif
		if( conv_mode != WAIT_CONV_ON ||
		    xclient->window != win ||
		    xclient->format != 32 ||
		    xclient->data.l[0] != conv_selection) {
			return;
		}
		if( xclient->data.l[2] == None ||
		    xclient->data.l[1] != conv_target) {
			conv_mode = CONV_OFF;
			return;
		}
		conv_mode = CONV_ON;
		conv_property = xclient->data.l[2];
		conv_win = xclient->data.l[3];
	}
	else if(xclient->message_type == conv_end) {
#ifdef DEBUG /* debug, do not translate */
		fprintf(stderr, "KinputCheckClientMessage() : conv_end : %d\n", conv_mode);
#endif
		if((conv_mode != WAIT_CONV_OFF && conv_mode != CONV_ON) ||
		    xclient->window != win ||
		    xclient->format != 32 ||
		    xclient->data.l[0] != conv_selection ||
		   (xclient->data.l[1] != conv_owner_win &&
		    xclient->data.l[1] != conv_win)) {
			return;
		}
		conv_mode = CONV_OFF;
	}
}

void CvtCompoundTextToEuc(dst, src)
register char *	dst;
register char *	src;
{
	int	status;

	status = LATIN;

	while(*src != '\0') {
		switch(status) {
		    case LATIN :
			if(*src == '\033')
				status = ESC;
			else
				*dst++= *src;
			break;
		    case ESC :
			if(*src == '$')
				status = ESC_24;
			else if(*src == '(')
				status = ESC_28;
			else {
				status = LATIN;
				*dst++= *src;
			}
			break;
		    case ESC_28 :
			if(*src == 'B' || *src == 'J')	/* noda */
				status = LATIN;
			else {
				status = LATIN;
				*dst++= *src;
			}
			break;
		    case ESC_24 :
			if(*src == '(')
				status = ESC_GL;
			else if(*src == ')')
				status = ESC_GR;
			else {
				status = LATIN;
				*dst++= *src;
			}
			break;
		    case ESC_GL :
			if(*src == 'B' || *src == '@')
				status = KANJI_GL;
			else {
				status = LATIN;
				*dst++= *src;
			}
			break;
		    case ESC_GR :
			if(*src == 'B' || *src == '@')
				status = KANJI_GR;
			else {
				status = LATIN;
				*dst++= *src;
			}
			break;
		    case KANJI_GL :
			if(*src == '\033')
				status = ESC;
			else
			if((*src & 0xff) < ' ')
				*dst++= *src;
			else
				*dst++= (*src | 0x80);
			break;
		    case KANJI_GR :
			if(*src == '\033')
				status = ESC;
			else
				*dst++= *src;
			break;
		    default :
			status = LATIN;
			break;
		}
		src ++;
	}

	*dst = '\0';
}

static
char* CvtCompoundTextToEuc_N(src, items)
char * src;
int items;
{
	char	*dst;
	if ((dst = (char *)malloc(sizeof(*src) * (items + 1))) != NULL) {
		strncpy(dst, src, items);
		dst[items] = '\0';
		CvtCompoundTextToEuc(dst, dst);
		return dst;
	} else {
		FailAllocMessage();
		return NULL;
	}
}

char* KinputCheckConvProperty (dpy, win, xprop)
Display			*dpy;
Window			win;
XPropertyEvent *	xprop;
{
	Atom		act_target;
	int		act_format;
	unsigned long	nitems;
	unsigned long	remain_bytes;
	unsigned char *		data;
	char *		stmp;

	if(imProtocol != IM_KINPUT)
		return NULL;

	if( xprop->window != win ||
	    xprop->atom != conv_property ||
	    xprop->state != PropertyNewValue ||
	    conv_mode != CONV_ON) {
		return NULL;
	}

	if(XGetWindowProperty(dpy, win, conv_property,
			0, MAXSTRING/sizeof(long),
			True, conv_target, &act_target, &act_format,
			&nitems, &remain_bytes, &data) != Success) {
		MsgBox(TgLoadString(STID_FAIL_TO_GET_WIN_PROP), TOOL_NAME,
			INFO_MB);
		return NULL;
	}
	if (remain_bytes > 0)
		XDeleteProperty(dpy, win, conv_property);

	if(act_target == None || conv_target != act_target)
		return NULL;
	if(act_format != 8) {
		XFree(data);
		return NULL;
	}

	stmp = CvtCompoundTextToEuc_N((char*)data, nitems);

	XFree(data);
	return stmp;
}

void KinputEndConversion (dpy, win)
	Display			*dpy;
	Window			win;
{
	Window			owner_win;
	char			msg[80];
	char *			str1;
	XClientMessageEvent	xcme;

	if(imProtocol != IM_KINPUT)
		return;

#ifdef DEBUG /* debug, do not translate */
	fprintf(stderr, "KinputEndConversion() : %d\n", conv_mode);
#endif
	switch(conv_mode) {
	    case WAIT_CONV_ON :
		Msg(TgLoadString(STID_WAIT_FOR_START_CONV));
		return;
	    case WAIT_CONV_OFF :
	    case CONV_OFF :
		return;
	}

	if((owner_win = XGetSelectionOwner(dpy, conv_selection))
			== None) {
		str1 = XGetAtomName(dpy, conv_selection);
		sprintf(msg, TgLoadString(STID_NO_NAMED_SELECTION_OWNER), str1);
		MsgBox(msg, TOOL_NAME, INFO_MB);
		XFree(str1);
		conv_owner_win = None;
		conv_mode = CONV_OFF;
#ifdef DEBUG /* debug, do not translate */
		fprintf(stderr, "0x%08x\n", owner_win);
#endif
		return;
	}
#ifdef DEBUG /* debug, do not translate */
	fprintf(stderr, "0x%08x\n", owner_win);
#endif

	if(conv_owner_win != owner_win) {
		conv_mode = CONV_OFF;
		return;
	}

	xcme.type = ClientMessage;
	xcme.display = dpy;
	xcme.window = owner_win;
	xcme.message_type = conv_end_req;
	xcme.format = 32;
	xcme.data.l[0] = conv_selection;
	xcme.data.l[1] = win;
	if(XSendEvent(dpy, owner_win, False, NoEventMask, (XEvent *)&xcme)
			== 0) {
		MsgBox(TgLoadString(STID_FAIL_TO_DISCONNECT_FROM_OWNER),
			TOOL_NAME, INFO_MB);
		conv_mode = CONV_OFF;
		return;
	}

	conv_mode = WAIT_CONV_OFF;
}

void CvtJisToEuc(dst, src)
register char *	dst;
register char *	src;
{
	register int	kanjiMode;
	register int	len;

	kanjiMode = KANJI0;
	len = 0;

	while(*src != '\0') {
		switch(kanjiMode) {
		    case KANJI0 :
			if(*src == '\033')
				kanjiMode = KANJI1;
			else
				*dst ++ = *src;
			break;
		    case KANJI1 :
			if(*src == '$')
				kanjiMode = KANJI2;
			else
				kanjiMode = KANJI0;
			break;
		    case KANJI2 :
			if(*src == '@' || *src == 'B') {
				len = 0;
				kanjiMode = KANJI3;
			}
			else
				kanjiMode = KANJI0;
			break;
		    case KANJI3 :
			if(*src == '\033')
				kanjiMode = KANJI4;
			else {
			   len ++;
			   if(*(src + 1) != '\033' || (len & 1) == 0)
				*dst ++ = (*src | 0x80);
			}
			break;
		    case KANJI4 :
			if(*src == '(')
				kanjiMode = KANJI5;
			else {
				len = 0;
				kanjiMode = KANJI3;
			}
			break;
		    case KANJI5 :
			if(*src == 'J' || *src == 'B')
				kanjiMode = KANJI0;
			else {
				len = 0;
				kanjiMode = KANJI3;
			}
			break;
		    default :
			break;
		}
		src ++;
	}

	*dst = '\0';
}

int CvtEucToJis(dst, src)
register char	*dst;
register char	*src;
{
	int	status;
	int	len;

	status = LATIN;
	len = 0;

	while(*src != '\0') {
		switch(status) {
		    case LATIN :
			while((*src & 0x80) == 0 && *src != '\0') {
				if(dst)
					*dst ++ = *src;
				src ++;
				len ++;
			}
			status = KANJI_GR;
			break;
		    case KANJI_GR :
			if(dst) {
				*dst ++ = '\033';
				*dst ++ = '$';
				*dst ++ = 'B';
			}
			len += 3;
			while((*src & 0x80) == 0x80 && *src != '\0') {
				if(dst)
					*dst ++ = *src & 0x7f;
				src ++;
				len ++;
			}
			if(dst) {
				*dst ++ = '\033';
				*dst ++ = '(';
				*dst ++ = 'B';
			}
			len += 3;
			status = LATIN;
			break;
		    default :
			break;
		}
	}

	if(dst)
		*dst = '\0';

	return len;
}

#endif /* ~_NO_KINPUT */

