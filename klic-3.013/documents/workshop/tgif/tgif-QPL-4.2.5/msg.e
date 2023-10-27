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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/msg.e,v 1.14 2011/05/16 16:21:58 william Exp $
 */

#ifndef _MSG_E_
#define _MSG_E_

extern char	scanFileFullPath[MAXPATHLENGTH+1];
extern char	scanFileName[];
extern int	scanLineNum;
extern char	progName[MAXPATHLENGTH+1];
extern char	fatalMsgBuf1[];
extern char	fatalMsgBuf2[];
extern int	cmdLineParentWindowID;
extern int	cmdLineTgrm2;
extern int	cmdLineWb2;

#ifdef _INCLUDE_FROM_MSG_C_
#undef extern
#define extern
#endif /*_INCLUDE_FROM_MSG_C_*/

extern int	ShowOrPrintMsg ARGS_DECL((char*, int use_msgbox));
extern int	FailAllocMessage ARGS_DECL((void));
extern int	FailAllocBitmapMessage ARGS_DECL((int W, int H));
extern int	FailAllocPixmapMessage ARGS_DECL((int W, int H));
extern int	FailToWriteFileMessage ARGS_DECL((char *file_name));
extern int	FailToOpenMessage ARGS_DECL((char *file_name, char *rwa,
		                             char *extra_msg));
extern int	FailToCreateWindowMessage ARGS_DECL((char *where,
		                                     char *win_name,
		                                     int do_exit));
extern int	FatalUnexpectedError ARGS_DECL((char*, char*));
extern int	TgAssert ARGS_DECL((int must_be_true, char*, char*));
extern int	BoldMsgCharBytes ARGS_DECL((char*));
extern char	*BoldMsgNextChar ARGS_DECL((char*));
extern char	*BoldMsgLastChar ARGS_DECL((char*));
extern char	*BoldMsgStrChr ARGS_DECL((char*, int));
extern int	MenuTextWidth ARGS_DECL((XFontStruct*, char *psz, int nLen));
extern int	MsgTextWidth ARGS_DECL((XFontStruct*, char *psz, int nLen));
extern int	BoldMsgTextWidth ARGS_DECL((XFontStruct*, char *psz, int nLen));
extern int	ItalicMsgTextWidth ARGS_DECL((XFontStruct*, char *psz,
		                              int nLen));
extern int	BoldItalicMsgTextWidth ARGS_DECL((XFontStruct*, char *psz,
		                                  int nLen));
extern void	DrawMenuString ARGS_DECL((Display*, Window, GC, int x,
		                          int baseline_y, char *psz, int nLen));
extern void	DrawMsgString ARGS_DECL((Display*, Window, GC, int x,
		                         int baseline_y, char *psz, int nLen));
extern void	DrawBoldMsgString ARGS_DECL((Display*, Window, GC, int x,
		                             int baseline_y, char *psz,
		                             int nLen));
extern void	DrawItalicMsgString ARGS_DECL((Display*, Window, GC, int x,
		                               int baseline_y, char *psz,
		                               int nLen));
extern void	DrawBoldItalicMsgString ARGS_DECL((Display*, Window, GC, int x,
		                                   int baseline_y, char *psz,
		                                   int nLen));
extern void	CleanUpMsg ARGS_DECL((void));
extern void	RedrawMsg ARGS_DECL((int erase_background));
extern void	Msg ARGS_DECL((char *));
extern void	TwoLineMsg ARGS_DECL((char *Msg1, char *Msg2));
extern void	PrintMsgBuffer ARGS_DECL((void));
extern void	RedrawStatusWindow ARGS_DECL((void));
extern void	SetMouseStatus ARGS_DECL((char *Left, char *Middle,
		                          char *Right));
extern void	SetStringStatus ARGS_DECL((char *StatusStr));
extern void	SaveStatusStrings ARGS_DECL((void));
extern void	RestoreStatusStrings ARGS_DECL((void));
extern void	SaveStatusStringsIntoBuf ARGS_DECL((
		      char ppsz_buf[MAX_STATUS_BTNS+1][MAXSTRING+1], int*));
extern void	RestoreStatusStringsFromBuf ARGS_DECL((
		      char ppsz_buf[MAX_STATUS_BTNS+1][MAXSTRING+1], int));
extern void	BeginProgress ARGS_DECL((ProgressInfo*, int total_size));
extern void	UpdateProgress ARGS_DECL((ProgressInfo*, int cur_size));
extern void	EndProgress ARGS_DECL((ProgressInfo*));
extern void	MsgEventHandler ARGS_DECL((XEvent*));
extern void	StatusEventHandler ARGS_DECL((XEvent*));
extern void	RedrawUserRedrawWindow ARGS_DECL((void));
extern void	UserRedrawEventHandler ARGS_DECL((XEvent*));
extern void	InitStatus ARGS_DECL((void));
extern void	CleanUpStatus ARGS_DECL((void));
extern void	InitScan ARGS_DECL((char *Str, char *Pat));
extern int	ScanValue ARGS_DECL((char *Fmt, void *Value, char *Item,
		                     char *SType));
extern int	ScanDynStrValue ARGS_DECL((struct DynStrRec *Value, char *Item,
		                           char *SType));
extern void	EmergencySave ARGS_DECL((int Sig));
extern int	EmergencySaveForX ARGS_DECL((Display*, XErrorEvent*));
extern int	IOEmergencySaveForX ARGS_DECL((Display *));
extern void	Error ARGS_DECL((char *where, char *msg));
extern void	GetTgifVersionAndPatchLevel ARGS_DECL((char *buf, int buf_sz));
extern void	Usage ARGS_DECL((char*));
extern int	ProcessPrTgifOptions ARGS_DECL((int argc, char *argv[], int));
extern int	ProcessTgifOptions ARGS_DECL((int argc, char *argv[],
		                              char *file_name, int name_sz));

#ifdef _INCLUDE_FROM_MSG_C_
#undef extern
#ifndef _NO_RECURSIVE_EXTERN
#define extern extern
#endif /* ~_NO_RECURSIVE_EXTERN */
#endif /*_INCLUDE_FROM_MSG_C_*/

#endif /*_MSG_E_*/
