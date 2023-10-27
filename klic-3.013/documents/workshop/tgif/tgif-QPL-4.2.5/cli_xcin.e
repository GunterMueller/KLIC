/*
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/cli_xcin.e,v 1.3 2011/06/14 02:32:18 william Exp $
 */

#ifndef _NO_XCIN

#ifndef _CLI_XCIN_E_
#define _CLI_XCIN_E_

#ifdef _INCLUDE_FROM_CLI_XCIN_C_
#undef extern
#define extern
#endif /*_INCLUDE_FROM_CLI_XCIN_C_*/

extern void	Tg_send_FocusIn ARGS_DECL((Display*, Window));
extern void	Tg_send_FocusOut ARGS_DECL((Display*, Window));
extern int	Tg_send_key ARGS_DECL((Display*, Window, XKeyEvent*, char*));

#ifdef _INCLUDE_FROM_CLI_XCIN_C_
#undef extern
#ifndef _NO_RECURSIVE_EXTERN
#define extern extern
#endif /* ~_NO_RECURSIVE_EXTERN */
#endif /*_INCLUDE_FROM_CLI_XCIN_C_*/

#endif /*_CLI_XCIN_E_*/

#endif /* ~_NO_XCIN */
