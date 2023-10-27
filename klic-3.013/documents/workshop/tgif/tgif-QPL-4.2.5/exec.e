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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/exec.e,v 1.8 2011/05/16 16:21:57 william Exp $
 */

#ifndef _EXEC_E_
#define _EXEC_E_

#include "list.e"

extern int		execAnimating;
extern int		execAnimateRedraw;
extern int		execCurDepth;
extern int		replaceAttrFirstValueRedraw;
extern int		execNavigateBack;
extern int		userAbortExec;

extern struct AttrRec	* warpToAttr;

extern char		* cmdToExecAfterHyperJump;

extern struct StrRec	* topTmpStr;
extern struct StrRec	* botTmpStr;

typedef struct tagReplAttrValueCallbackInfo {
   struct ObjRec *obj_ptr; /* a top level object */
   struct AttrRec *attr_ptr;
   char *new_attr_value;
   void *pv_userdata;
} ReplAttrValueCallbackInfo;

typedef int (ReplAttrValueCallback)ARGS_DECL((ReplAttrValueCallbackInfo*));

#ifdef _INCLUDE_FROM_EXEC_C_
#undef extern
#define extern
#endif /*_INCLUDE_FROM_EXEC_C_*/

extern void	CleanTmpStr ARGS_DECL((void));
extern int	PrependToTmpStr ARGS_DECL((char*));
extern int	AppendToTmpStr ARGS_DECL((char*));
extern int	ExecuteCmdGetResult ARGS_DECL((char*, CVList*, int max_lines));
extern struct ObjRec	* FindObjWithName ARGS_DECL((struct ObjRec *BotObj,
			                             struct ObjRec *ObjPtr,
			                             char *ObjName,
			                             int InsideRootObj,
			                             int InsideThisObj,
			                             struct ObjRec **OwnerObj,
			                             struct ObjRec **TopOwner));

extern int	PipeReachedEOF ARGS_DECL((FILE*));
extern int	WaitForEvent ARGS_DECL((FILE*, int fp_is_pipe,
		                        int fp_is_named_pipe, int *pn_quit,
		                        int what_to_do_with_x_events,
		                        AbortCallbackFunc*, void*));
extern int	DoLaunch ARGS_DECL((struct AttrRec *, struct ObjRec *));
extern void	SetReplaceAttrValueCallback ARGS_DECL((ReplAttrValueCallback*,
		                                       void *pv_userdata));
extern int	ReplaceAttrFirstValue ARGS_DECL((struct ObjRec *,
		                                 struct AttrRec *, char *));
extern int	ReplaceAttrAllValues ARGS_DECL((struct ObjRec *,
		                                struct AttrRec *,
		                                struct StrRec **ppTopStr,
		                                struct StrRec **ppBotStr));
extern void	JustReadFileIntoAttr ARGS_DECL((FILE*, struct AttrRec *attr_ptr,
		                                struct ObjRec *attr_owner_obj));
extern int	DoExec ARGS_DECL((struct AttrRec *, struct ObjRec *));
extern void	ResetExec ARGS_DECL((int nStart));
extern void	ExecCmdsFromFile ARGS_DECL((char*));
extern void	ExecCmdsFromTmp ARGS_DECL((char *tmp_fname));
extern void	StartExecCmdsFromFile ARGS_DECL((void));
extern int	BeginExecCommandsFromBuffer ARGS_DECL((void));
extern int	ExecACommandFromBuffer ARGS_DECL((char *buf, int *pn_quit));
extern void	EndExecCommandsFromBuffer ARGS_DECL((void));
extern void	CleanUpExec ARGS_DECL((void));
extern void	InitExec ARGS_DECL((void));

#ifdef _INCLUDE_FROM_EXEC_C_
#undef extern
#ifndef _NO_RECURSIVE_EXTERN
#define extern extern
#endif /* ~_NO_RECURSIVE_EXTERN */
#endif /*_INCLUDE_FROM_EXEC_C_*/

#endif /*_EXEC_E_*/
