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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/inmethod.e,v 1.7 2011/05/16 16:21:58 william Exp $
 */

#ifndef _INMETHOD_E_
#define _INMETHOD_E_

typedef char *(Tgim_stringprep_convert_Func)ARGS_DECL((const char *str,
		const char *to_codeset, const char *from_codeset));

extern int	gnInputMethod; /* one of TGIM_* */
extern int	gnOverTheSpot;

extern int	cmdLineHasInputMethod;
extern int	cmdLineDoubleByteInputMethod;
extern char	cmdLineInputMethod[];

#ifdef _INCLUDE_FROM_INMETHOD_C_
#undef extern
#define extern
#endif /*_INCLUDE_FROM_INMETHOD_C_*/

extern int	Tgim_has_stringprep_convert ARGS_DECL((void));
extern char	*Tgim_stringprep_convert ARGS_DECL((const char *str,
		                                    const char *to_codeset,
		                                    const char *from_codeset));

extern void	CleanUpInputMethods ARGS_DECL((void));
extern int	InitInputMethods ARGS_DECL((void));

extern int	ResetInputMethod ARGS_DECL((void));
extern int	InputMethodTypeMatched ARGS_DECL((int DoubleByte));

extern int	tgIMFocusIn ARGS_DECL((Display *dpy, Window win));
extern int	tgIMFocusOut ARGS_DECL((Display *dpy, Window win));
extern int	tgIMTranslateKeyEvent ARGS_DECL((Display *dpy,
		                                 Window win,
		                                 XKeyEvent *key_ev,
		                                 char *buf));
extern int	tgIMConvertProperty ARGS_DECL((Display *dpy,
		                               Window win,
		                               XPropertyEvent *prop_ev,
		                               char **ppsz_buf));
extern int	tgIMExpectClientMessage ARGS_DECL((Display *dpy,
		                                   Window win));
extern int	tgIMHandleClientMessage ARGS_DECL((Display *dpy,
		                                   Window win,
		                                   XClientMessageEvent *cm_ev,
		                                   XKeyEvent *key_ev,
		                                   char *buf));
extern int	tgIMActiveOnCntrlSpace ARGS_DECL((Display *dpy,
		                                  Window win));
extern int	tgIMHandleCntrlSpace ARGS_DECL((Display *dpy,
		                                Window win));
extern int	tgIMDeactiveOnCreateText ARGS_DECL((Display *dpy,
		                                    Window win));
extern int	tgIMHandleCreateText ARGS_DECL((Display *dpy,
		                                Window win));
extern int	tgIMTellCursorPosition ARGS_DECL((Display *dpy,
		                                  Window win,
		                                  int cur_x,
		                                  int cur_y));
extern int	tgIMExpectNextEvent ARGS_DECL((Display *dpy,
		                               Window win));
extern int	tgIMHandleNextEvent ARGS_DECL((Display *dpy,
		                               Window win,
		                               XEvent *ev));
extern int	tgIMHandleNewCurText ARGS_DECL((Display *dpy,
		                                Window win));
extern int	tgIMExpectLookupString ARGS_DECL((Display *dpy,
		                                  Window win));
extern int	tgIMHandleLookupString ARGS_DECL((Display *dpy,
		                                  Window win,
		                                  XKeyEvent *key_ev,
		                                  char *psz_buf,
		                                  int buf_size,
		                                  KeySym *p_key_sym,
		                                  int *pn_has_ch));

#ifdef _INCLUDE_FROM_INMETHOD_C_
#undef extern
#ifndef _NO_RECURSIVE_EXTERN
#define extern extern
#endif /* ~_NO_RECURSIVE_EXTERN */
#endif /*_INCLUDE_FROM_INMETHOD_C_*/

#endif /*_INMETHOD_E_*/
