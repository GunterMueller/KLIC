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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/ps.e,v 1.6 2011/05/16 16:21:59 william Exp $
 */

#ifndef _PS_E_
#define _PS_E_

extern int	preDumpSetup;
extern int	needsTgifOrigCTM;
extern int	psUseShortHand;

extern char	**gPsCmd;

#ifdef _INCLUDE_FROM_PS_C_
#undef extern
#define extern
#endif /*_INCLUDE_FROM_PS_C_*/

extern void	PSUsePSAdobe ARGS_DECL((void));
extern void	PSUseMinRadius ARGS_DECL((void));
extern void	PSUseEllipse ARGS_DECL((void));
extern void	PSUseArrow ARGS_DECL((void));
extern void	PSUseArc ARGS_DECL((void));
extern void	PSUseBWPattern ARGS_DECL((void));
extern void	PSUseColorImage ARGS_DECL((void));
extern void	PSUseColorPattern ARGS_DECL((void));
extern void	PSUseCenterText ARGS_DECL((void));
extern void	PSUseRightText ARGS_DECL((void));
extern void	PSUseMiniLines ARGS_DECL((void));
extern void	PSUseReencode ARGS_DECL((char *FontName));
extern void	PSDontUseShortHand ARGS_DECL((void));
extern void	PSUseShortHand ARGS_DECL((void));
#ifdef _PS_USE_EUC_FONT
extern void	PSUseEucFont ARGS_DECL((void));
#endif /* _PS_USE_EUC_FONT */

extern void	ResetPSInfo ARGS_DECL((void));
extern void	DoneResetPSInfo ARGS_DECL((void));

extern void	DumpPSMacro ARGS_DECL((FILE *));
extern int	ParsePsAdobeString ARGS_DECL((char *Str, int *UsePSAdobeStr,
		                              char *AdobeStr, char *EPSFStr));

extern void	CleanUpPS ARGS_DECL((void));
extern void	InitPS ARGS_DECL((void));

#ifdef _INCLUDE_FROM_PS_C_
#undef extern
#ifndef _NO_RECURSIVE_EXTERN
#define extern extern
#endif /* ~_NO_RECURSIVE_EXTERN */
#endif /*_INCLUDE_FROM_PS_C_*/

#endif /*_PS_E_*/
