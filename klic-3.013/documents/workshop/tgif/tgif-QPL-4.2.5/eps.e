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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/eps.e,v 1.6 2011/05/16 16:21:57 william Exp $
 */

#ifndef _EPS_E_
#define _EPS_E_

extern float	defaultEPSScaling;
extern char	defaultEPSScalingStr[];

extern int	autoEPSPreviewBitmap;

#ifdef _INCLUDE_FROM_EPS_C_
#undef extern
#define extern
#endif /*_INCLUDE_FROM_EPS_C_*/

extern void	InitEPS ARGS_DECL((void));
extern void	CleanUpEPS ARGS_DECL((void));
extern int	MyReadEPSFile ARGS_DECL((char *FileName, int *ImageW,
		                         int *ImageH, Pixmap *, XImage **,
		                         int *NumLines, char ***EPSFLines,
		                         int *EPSFLevel, float *llx,
		                         float *lly, float *urx, float *ury,
		                         char *Date));
extern void	DumpEPSObj ARGS_DECL((FILE *, struct ObjRec *));
extern void	SetEPSObjCTM ARGS_DECL((struct ObjRec *, struct BBRec *));
extern struct ObjRec	* CreateEPSObj ARGS_DECL((char *FileName, int ImageW,
			                          int ImageH, Pixmap,
			                          XImage *, int NumLines,
			                          char **Lines, int EPSFLevel,
		                                  float *llx, float *lly,
			                          float *urx, float *ury,
			                          char *Date));
extern void	UpdateEPS ARGS_DECL((void));

#ifdef _INCLUDE_FROM_EPS_C_
#undef extern
#ifndef _NO_RECURSIVE_EXTERN
#define extern extern
#endif /* ~_NO_RECURSIVE_EXTERN */
#endif /*_INCLUDE_FROM_EPS_C_*/

#endif /*_EPS_E_*/
