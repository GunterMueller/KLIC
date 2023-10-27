/*
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/nkf.e,v 1.4 2011/06/14 02:32:19 william Exp $
 */

#ifndef _NO_NKF

#ifndef _NKF_E_
#define _NKF_E_

#ifdef _INCLUDE_FROM_NKF_C_
#undef extern
#define extern
#endif /*_INCLUDE_FROM_NKF_C_*/

extern int	Tg_useNKF ARGS_DECL((void));
extern int	Tg_do_kconv ARGS_DECL((char *i, char *o, int siz));
extern void	CleanUpNKF ARGS_DECL((void));
extern int	InitNKF ARGS_DECL((void));

#ifdef _INCLUDE_FROM_NKF_C_
#undef extern
#ifndef _NO_RECURSIVE_EXTERN
#define extern extern
#endif /* ~_NO_RECURSIVE_EXTERN */
#endif /*_INCLUDE_FROM_NKF_C_*/

#endif /*_NKF_E_*/

#endif /* ~_NO_NKF */

