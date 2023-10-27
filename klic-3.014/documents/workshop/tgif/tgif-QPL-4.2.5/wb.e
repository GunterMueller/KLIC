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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/wb.e,v 1.15 2011/05/16 16:22:00 william Exp $
 */

#ifndef _WB_E_
#define _WB_E_

#define WB_PACK 60000

typedef struct WhiteBoardRec {
   int	do_whiteboard;

   char	send_fname[MAXPATHLENGTH];
   char	gz_send_fname[MAXPATHLENGTH]; /* zlib, not gzip */
   FILE *send_fp;
   FILE *gz_send_fp; /* zlib, not gzip */
   int	send_socket;

   int	listening;

   char	listen_fname[MAXPATHLENGTH];
   char	gz_listen_fname[MAXPATHLENGTH]; /* zlib, not gzip */
   FILE	*listen_fp;
   FILE	*gz_listen_fp; /* zlib, not gzip */
   int	listen_socket;

   int	MCastSock;
   int	bd_commands;
   int	BlockRemoteCmdDepth;

   int	logical_clock;
   long	pid;

   int	dont_serialize; /* true will skip the call to SerializeCmd() */

   struct CmdRec	*first_cmd, *last_cmd, *cur_cmd;
   struct CmdRec	*first_shadow_cmd, *last_shadow_cmd;

   int	join_session_in_progress; /* there is current WB state info */
} *WhiteBoardPtr;

typedef struct WbMessage{
   char	WbCmd[WB_PACK];
}wbmsgPtr;

typedef struct tagSerializationInfo {
   struct SubCmdRec *subcmd;
   struct SelRec *top_sel; /* TopSel */
   struct SelRec *bot_sel; /* BotSel */
   int num_objs; /* NumObjs */

   int include_tgif_obj; /* recordCmdIncludeTgifObj */
   int new_colormap; /* recordCmdUsesNewColormap */
   int logical_clock; /* gstWBInfo.logical_clock */
   char *sender_process_id; /* just a pointer, gszLocalPID */

   struct SelRec *top_before; /* topSelBeforeInCmd */
   struct SelRec *bot_before; /* botSelBeforeInCmd */
   int *pos_before; /* stackingPosition */
   int count_before; /* stackingCount */
   int pos_before_has_ids; /* stackingPositionHasIds */
} SerializationInfo;

extern struct WhiteBoardRec	gstWBInfo;

extern int	cmdLineNoWhiteBoard;
extern int	cmdLineWhiteBoardListenOnly;

#ifdef _INCLUDE_FROM_WB_C_
#undef extern
#define extern
#endif /*_INCLUDE_FROM_WB_C_*/

/* =================== Remote Related Functions =================== */

extern void	PrintFullIDsOfObjects ARGS_DECL((char *psz_prefix));

extern int	WBHasReadData ARGS_DECL((int *pn_flag, int *pn_retry));
extern int	RecvWBData ARGS_DECL((
			int flag, /* *pn_flag of WBHasReadData() */
			char **ppsz_buf, int *pn_buf_sz));
extern void	TryHandleWBInputData ARGS_DECL((void));
extern int	SendWBData ARGS_DECL((char *pszWBData, int logical_clock));
//extern int	SendWBData ARGS_DECL((char *pszWBData));

/* =================== Cmd Related Functions =================== */

extern char	*SerializeCmd ARGS_DECL((int CmdType, SerializationInfo*,
		                         int *pnAssignedLogicalClock,
		                         char **ppszPlainWBData,
		                         int *pnPlainWBDataSz));
extern void	SerializeFreeBuf ARGS_DECL((char *psz_buf));

extern int	ReadExtendedCmd ARGS_DECL((FILE *FP, char *Inbuf));
extern int	ReadExtendedCmdInfo ARGS_DECL((FILE *FP, char *Inbuf));
extern int	ReadCmd ARGS_DECL((FILE *FP, char *Inbuf));
extern int	ReadBeforeImage ARGS_DECL((FILE *FP, char *Inbuf));
extern int	ReadAfterPositions ARGS_DECL((FILE *FP, char *Inbuf));
extern int	ReadAfterImage ARGS_DECL((FILE *FP, char *Inbuf));
extern void	FindCmdInsertionPoint ARGS_DECL((struct CmdRec *cmd_to_insert,
				struct CmdRec **pp_immed_right_cmd));
extern void	FindShadowCmdInsertionPoint ARGS_DECL((
				struct CmdRec *cmd_to_insert,
				struct CmdRec **pp_immed_right_cmd));
extern void	CleanUpObsoletedWBCmds ARGS_DECL((struct CmdRec *));
extern void	ProcessRemoteCmd ARGS_DECL((char *psz_content_type,
				char *psz_buf, int buf_sz));

/* =================== Handle New User Requests =================== */

extern void	HandleNewUserRequest ARGS_DECL((void));

/* =================== Init and CleanUp Functions =================== */

extern void	CleanUpWhiteBoard ARGS_DECL((void));
extern int	InitWhiteBoard ARGS_DECL((void));
extern int	InitWhiteBoard2 ARGS_DECL((void));
extern void	RMcallback_terminate ARGS_DECL((void));

#ifdef _INCLUDE_FROM_WB_C_
#undef extern
#ifndef _NO_RECURSIVE_EXTERN
#define extern extern
#endif /* ~_NO_RECURSIVE_EXTERN */
#endif /*_INCLUDE_FROM_WB_C_*/

#endif /*_WB_E_*/
