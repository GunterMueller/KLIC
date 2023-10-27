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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/util.e,v 1.10 2011/05/16 16:22:00 william Exp $
 */

#ifndef _TGIF_UTIL_E_
#define _TGIF_UTIL_E_

#ifdef _INCLUDE_FROM_UTIL_C_
#undef extern
#define extern
#endif /*_INCLUDE_FROM_UTIL_C_*/

extern void	UtilFree ARGS_DECL((char *pszStr));
extern char	*UtilFreeStringArray ARGS_DECL((char **ppsz, int nItems));
extern char	*UtilStrDup ARGS_DECL((char *pszStr));
extern int	UtilStrCpyN ARGS_DECL((char *pszDest, int nMaxDestSz,
				char *pszSrc));
extern int	UtilStrCpy ARGS_DECL((char *pszDest, char *pszSrc));
extern void	UtilLeftTrim ARGS_DECL((char *pszStr));
extern void	UtilRightTrim ARGS_DECL((char *pszStr));
extern void	UtilTrimBlanks ARGS_DECL((char *pszStr));
extern void	UtilStrUpper ARGS_DECL((char *pszStr));
extern void	UtilStrLower ARGS_DECL((char *pszStr));
extern int	UtilStrNCaseCmp ARGS_DECL((char *pszStr1, char *pszStr2,
				int nCount));
extern int	UtilStrICmp ARGS_DECL((char *pszStr1, char *pszStr2));
extern char	*UtilStrRChr ARGS_DECL((char *pszStr, int int_ch));
extern char	*UtilStrTok ARGS_DECL((char *pszStr, char *pszDelim,
				char **ppszState));
#ifdef NO_STRSTR
extern char	*strstr ARGS_DECL((char *pszStr, char *pszSubStr));
#endif /* NO_STRSTR */
extern int	UtilShrinkName ARGS_DECL((char *pszFile));
extern void	UtilRemoveQuotes ARGS_DECL((char *pszStr));
extern char	*UtilGetALine ARGS_DECL((FILE *lpFile));
extern char	*UtilGetAContinuedLine ARGS_DECL((FILE *lpFile));
extern int	UtilCopyFile ARGS_DECL((char *pszFromFile, char *pszToFile));
extern int	UtilConcatFile ARGS_DECL((char *pszFromFile1,
				char *pszFromFile2, char *pszToFile));

extern int	UtilPathExists ARGS_DECL((char *pszPath));
extern int	UtilPathIsLink ARGS_DECL((char *pszPath));
extern int	UtilIsDirectory ARGS_DECL((char *pszPath));
extern int	UtilCreateDirHier ARGS_DECL((char *pszDir));

extern long	UtilGetMilliSecTime ARGS_DECL((long *pl_sec, long *pl_msec));

#ifdef _INCLUDE_FROM_UTIL_C_
#undef extern
#ifndef _NO_RECURSIVE_EXTERN
#define extern extern
#endif /* ~_NO_RECURSIVE_EXTERN */
#endif /*_INCLUDE_FROM_UTIL_C_*/

#endif /*_TGIF_UTIL_E_*/
