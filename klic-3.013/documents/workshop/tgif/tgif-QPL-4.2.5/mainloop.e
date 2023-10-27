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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/mainloop.e,v 1.8 2011/05/16 16:21:58 william Exp $
 */

#ifndef _MAINLOOP_E_
#define _MAINLOOP_E_

extern int	x11R6OrAbove;
extern int	origArgC;
extern char	* * origArgV;
extern int	cmdLineBW;
extern int	cmdLineRV;
extern int	cmdLineCWO;
extern int	cmdLineBtn1Warp;
extern char	* cmdLineForeground;
extern char	* cmdLineBackground;
extern char	* cmdLineBorder;
extern int	cmdLineMerge;
extern int	cmdLineMergeArgc;
extern char	* * cmdLineMergeArgv;
extern int	geometrySpecified;
extern int	exitNormally;
extern char	geometrySpec[];
extern char	* displayName;
extern int	initializingMain;
extern int	cmdLineUseXlib;
extern int	cmdLineVersion;
extern int	cmdLineNoModeWindow;
extern int	cmdLineNoChatWindow;

extern KeyOrBtnEvInfo	lastKeyOrBtnEvInfo;

extern int	numExtraWins;
extern struct WinInfoRec	* extraWinInfo;

#ifdef _INCLUDE_FROM_MAINLOOP_C_
#undef extern
#define extern
#endif /*_INCLUDE_FROM_MAINLOOP_C_*/

extern int	AddExtraWinInfo ARGS_DECL((Window, int Mapped, int Raise,
				void (*ExposeHandler)ARGS_DECL((XEvent*,
						struct WinInfoRec *)),
				int (*EventHandler)ARGS_DECL((XEvent*,
						struct WinInfoRec *)),
				void (*CleanUpRoutine)ARGS_DECL((
						struct WinInfoRec *)),
				void *userdata));
extern void	CleanUpTalkToSelfFiledes ARGS_DECL((void));
extern int	InitTalkToSelfFiledes ARGS_DECL((void));
extern void	DeallocStrings ARGS_DECL((char **Func,
		                char **Str1, char **Menu1,
		                char **Str2, char **Menu2,
		                char **Str3, char **Menu3));
extern void	CleanUp ARGS_DECL((void));
extern void	ExposeEventHandler ARGS_DECL((XEvent *, int Recursive));
extern int	GzipFile ARGS_DECL((char *fname, char *gzipped_fname));
extern char	*GunzipFileIntoTemp ARGS_DECL((char *gzipped_fname));
extern int	JustInit ARGS_DECL((char *pszFile, int *pnNeedToCheckAutoExec));
extern void	HandleDataInMBuff ARGS_DECL((void));
extern int	TryProcessAnXEvent ARGS_DECL((int *pnNeedToCheckAutoExec));
extern int	TryProcessAnAbortXEvent ARGS_DECL((int esc_to_abort,
		                                   AbortCallbackFunc*, void*));
extern void	MainLoop ARGS_DECL((char *Op, char *FileName, char **Func,
		                    char **Str1, char **Menu1,
		                    char **Str2, char **Menu2,
		                    char **Str3, char **Menu3));
extern void	UrlDump ARGS_DECL((char *FileName));
extern void	DosEpsFilter ARGS_DECL((char *FileName));
extern void	Animate ARGS_DECL((char *Type, char *PolyId, char *Speed,
		                   char *Color, char **ReturnStr));
extern void	UpdAttrVal ARGS_DECL((char *ObjId, char *AttrName,
		                      char *AttrColor, char *AttrVal,
		                      char **ReturnStr));

extern void	SetLastKeyEvInfo ARGS_DECL((XKeyEvent*));
extern void	SetLastBtnEvInfo ARGS_DECL((XButtonEvent*));

#ifdef _INCLUDE_FROM_MAINLOOP_C_
#undef extern
#ifndef _NO_RECURSIVE_EXTERN
#define extern extern
#endif /* ~_NO_RECURSIVE_EXTERN */
#endif /*_INCLUDE_FROM_MAINLOOP_C_*/

#endif /*_MAINLOOP_E_*/
