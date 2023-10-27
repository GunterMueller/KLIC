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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/xprtfltr.e,v 1.6 2011/05/16 16:22:00 william Exp $
 */

#ifndef _XPRTFLTR_E_
#define _XPRTFLTR_E_

extern int	gnMaxExportFilters; /* if 0, no export filters */

extern Pixmap	*gaExportFilterBitmaps;

extern TgEFInfo	*gpTgEFInfo;

#ifdef _INCLUDE_FROM_XPRTFLTR_C_
#undef extern
#define extern
#endif /*_INCLUDE_FROM_XPRTFLTR_C_*/

extern void	CleanUpExportFilters ARGS_DECL((void));
extern int	InitExportFilters ARGS_DECL((void));
extern int	GetExportIndex ARGS_DECL((int nWhereToPrint, int nMode));
extern char	* GetExportName ARGS_DECL((int nWhereToPrint, int nMode));
extern char	* GetExportExt ARGS_DECL((int nWhereToPrint));
extern int	GetExportFilterCapability ARGS_DECL((int nWhereToPrint));
extern int	ExportVectorFile ARGS_DECL((FILE *pDumpFile,
		                            int nPageNumInFileName,
		                            int nPrintingPageNum,
		                            int nPrintingFirstPageNum,
		                            int nPrintingPageRow,
		                            int nPrintingPageCol));

#ifdef _INCLUDE_FROM_XPRTFLTR_C_
#undef extern
#ifndef _NO_RECURSIVE_EXTERN
#define extern extern
#endif /* ~_NO_RECURSIVE_EXTERN */
#endif /*_INCLUDE_FROM_XPRTFLTR_C_*/

#endif /*_XPRTFLTR_E_*/
