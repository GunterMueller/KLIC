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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/choose.e,v 1.6 2011/05/16 16:21:56 william Exp $
 */

#ifndef _CHOOSE_E_
#define _CHOOSE_E_

extern GC	nameGC;
extern GC	revNameGC;

#ifdef _INCLUDE_FROM_CHOOSE_C_
#undef extern
#define extern
#endif /*_INCLUDE_FROM_CHOOSE_C_*/

extern int	GetWorkingDirectory ARGS_DECL((char *buf, int buf_sz));
extern int	SetWorkingDirectory ARGS_DECL((char *dir));
extern void	CleanUpCheckArray ARGS_DECL((struct CheckArrayRec *));
extern int	CopyCheckArray ARGS_DECL((struct CheckArrayRec *pTo,
		                          struct CheckArrayRec *pFrom));
extern void	NamesSetTitle ARGS_DECL((char*));
extern void	ResetNamesInfo ARGS_DECL((void));
extern void	InitNamesInfo ARGS_DECL((void));
extern void	CleanUpNamesInfo ARGS_DECL((void));
extern void	NamesAddButton ARGS_DECL((char*, int));

extern void	DrawCheckbox ARGS_DECL((Display*, Window, GC, int x, int y,
				int w, int h, int checked));

extern void	NamesSetDefaultBtnId ARGS_DECL((int def_btn_id,
		                                int double_click_btn_id));
extern void	NamesSetStyle ARGS_DECL((int edit_style, int loop_once));
extern void	NamesSetEntries ARGS_DECL((DspList*, char**, int num_entries,
		                           struct CheckArrayRec*,
		                           int dont_free_entries,
		                           int marked_index, int leading));
extern void	NamesSetCallback ARGS_DECL((GetEntriesFunc *pf_before_loop,
		                            AfterLoopFunc *pf_after_loop,
		                            CheckUpdateFunc *pf_check_update));
extern void	NamesSetDir ARGS_DECL((char*));
extern int	Names ARGS_DECL((char *win_name, int *pn_selected_index,
		                 char *selected_str, int str_sz, void*));

#ifdef _INCLUDE_FROM_CHOOSE_C_
#undef extern
#ifndef _NO_RECURSIVE_EXTERN
#define extern extern
#endif /* ~_NO_RECURSIVE_EXTERN */
#endif /*_INCLUDE_FROM_CHOOSE_C_*/

#endif /*_CHOOSE_E_*/
