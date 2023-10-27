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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/cmd.e,v 1.7 2011/05/16 16:21:56 william Exp $
 */

#ifndef _CMD_E_
#define _CMD_E_

extern int		recordCmdIncludeTgifObj;
extern int		recordCmdUsesNewColormap;
extern int		undoingOrRedoing;
extern int		historyDepth, historyCount, defaultHistoryDepth;
extern struct CmdRec	*firstCmd, *lastCmd, *curCmd;

extern struct SelRec	*topSelBeforeInCmd, *botSelBeforeInCmd;
extern int		*stackingPosition;
extern int		stackingCount;
extern int		stackingPositionHasIds;

extern int		composingCommand;
extern int		undoRedoRestoreDrawingMode;
extern Colormap		preparedColormap;

#ifdef _INCLUDE_FROM_CMD_C_
#undef extern
#define extern
#endif /*_INCLUDE_FROM_CMD_C_*/

extern void	InsertCmd ARGS_DECL((struct CmdRec *PrevCmd,
		                     struct CmdRec *NextCmd,
		                     struct CmdRec *CmdPtr,
		                     struct CmdRec **ppFirstCmd,
		                     struct CmdRec **ppLastCmd));
extern void	CopyAndInsertCmd ARGS_DECL((int from_remote,
                                     char *serialization_buf,
                                     struct CmdRec *PrevCmd,
		                     struct CmdRec *NextCmd,
		                     struct CmdRec *CmdPtr,
		                     struct CmdRec **ppFirstCmd,
		                     struct CmdRec **ppLastCmd));
extern void	DeleteARedoRecord ARGS_DECL((struct CmdRec *, double PercStart,
		                             double PercEnd));
extern void	ClearRedoRecords ARGS_DECL((struct CmdRec *));
extern void	CleanUpCmds ARGS_DECL((void));
extern void	CopySel ARGS_DECL((struct SelRec *FromTopSel, int Count,
		                   struct SelRec **ToTopSel,
		                   struct SelRec **ToBotSel));
extern void	UndoACmd ARGS_DECL((struct CmdRec *CmdPtr, int HighLight,
		                   int PerformAction));
extern void	UndoCmd ARGS_DECL((void));
extern int	RedoACmd ARGS_DECL((struct CmdRec *CmdPtr, int HighLight,
		                    int PerformAction));
extern void	RedoCmd ARGS_DECL((void));
extern void	StartCompositeCmd ARGS_DECL((void));
extern void	EndCompositeCmd ARGS_DECL((void));
extern void	RestoreDefaultHistoryDepth ARGS_DECL((void));
extern void	DisableUndo ARGS_DECL((void));
extern void	EnableUndo ARGS_DECL((void));
extern struct SelRec	* PrepareStackingInfo ARGS_DECL((
				struct SelRec *TopSel,
				struct SelRec *BotSel,
				int NumObjs,
				int PreparePhase,
				int **ppnStackingPosition,
				int *pnStackingCount,
				int *pnStackingPositionHasIds));
extern void	PrepareToRecord ARGS_DECL((int, struct SelRec *Top,
		                           struct SelRec *Bot, int NumObjs));
extern void	FreeAfterSel ARGS_DECL((struct CmdRec *));
extern void	RecordCmd ARGS_DECL((int, struct SubCmdRec *,
		                     struct SelRec *Top, struct SelRec *Bot,
		                     int NumObjs));
extern void	AbortPrepareCmd ARGS_DECL((int CmdType));
extern void	RecordNewObjCmd ARGS_DECL((void));
extern void	PrepareToReplaceAnObj ARGS_DECL((struct ObjRec *));
extern void	RecordReplaceAnObj ARGS_DECL((struct ObjRec *));
extern void	ChangeReplaceOneCmdToDeleteCmd ARGS_DECL((void));
extern void	RecordWBClearAll ARGS_DECL((void));
extern void	RecordWBSlideShow ARGS_DECL((int nIntoSlideShow));
extern void	RecordWBChatALine ARGS_DECL((struct SubCmdRec *));

#ifdef _INCLUDE_FROM_CMD_C_
#undef extern
#ifndef _NO_RECURSIVE_EXTERN
#define extern extern
#endif /* ~_NO_RECURSIVE_EXTERN */
#endif /*_INCLUDE_FROM_CMD_C_*/

#endif /*_CMD_E_*/
