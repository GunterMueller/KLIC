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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/dialog.e,v 1.6 2011/05/16 16:21:57 william Exp $
 */

#ifndef _DIALOG_E_
#define _DIALOG_E_

extern int		doPassword;
extern char		gszMsgBox[2048];

extern int		dialogboxUse3DBorder;

#ifdef _INCLUDE_FROM_DIALOG_C_
#undef extern
#define extern
#endif /*_INCLUDE_FROM_DIALOG_C_*/

extern unsigned int	CornerLoop ARGS_DECL((int *OrigX, int *OrigY));
extern unsigned int	DrawWindowLoop ARGS_DECL((int *X, int *Y, Cursor,
			                          int SnapToGrid));
extern unsigned int	PickAPoint ARGS_DECL((int *OrigX, int *OrigY, Cursor));

extern void	CalcFormatStringInBox ARGS_DECL((char *buf, XFontStruct*,
		                                 int font_height,
		                                 int format_box_w,
		                                 int *pn_num_lines,
		                                 int *pn_max_w, int *pn_max_h,
		                                 char **ppsz_formated_buf));
extern int	MsgBox ARGS_DECL((char *Message, char *Title, int IconAndBtns));
extern int	Dialog ARGS_DECL((char *Message, char *Comment,
		                  char *ReturnStr));

#ifdef _INCLUDE_FROM_DIALOG_C_
#undef extern
#ifndef _NO_RECURSIVE_EXTERN
#define extern extern
#endif /* ~_NO_RECURSIVE_EXTERN */
#endif /*_INCLUDE_FROM_DIALOG_C_*/

#endif /*_DIALOG_E_*/
