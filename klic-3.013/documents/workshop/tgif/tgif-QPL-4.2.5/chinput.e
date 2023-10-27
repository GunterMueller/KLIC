/*
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/chinput.e,v 1.2 2011/06/14 02:32:18 william Exp $
 */

#ifndef _NO_CHINPUT

#ifndef _CHINPUT_E_
#define _CHINPUT_E_

#ifdef _INCLUDE_FROM_CHINPUT_C_
#undef extern
#define extern
#endif /*_INCLUDE_FROM_CHINPUT_C_*/

extern int	Tg_HZclientInit ARGS_DECL((Display*));
extern int	Tg_HZsendKey ARGS_DECL((Display*, Window, XKeyEvent*));
extern int	Tg_HZhandleCM ARGS_DECL((XClientMessageEvent*, XKeyEvent*,
				char*));

#ifdef _INCLUDE_FROM_CHINPUT_C_
#undef extern
#ifndef _NO_RECURSIVE_EXTERN
#define extern extern
#endif /* ~_NO_RECURSIVE_EXTERN */
#endif /*_INCLUDE_FROM_CHINPUT_C_*/

#endif /*_CHINPUT_E_*/

#endif /* ~_NO_CHINPUT */

