/*
 * Author:      Daniel Ratton Figueiredo <ratton@land.ufrj.br>
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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/tangram2.e,v 1.9 2011/05/16 16:21:59 william Exp $
 */

#ifndef _TGIF_TANGRAM2_E_
#define _TGIF_TANGRAM2_E_

#define CMDID_TANGRAM2_BASE 0x4000

#define CMDID_ABOUT_TANGRAM2           (CMDID_TANGRAM2_BASE+0)
#define CMDID_TANGRAM2_GENERATE_CHAIN  (CMDID_TANGRAM2_BASE+1)
#define CMDID_TANGRAM2_START_SIMULATOR (CMDID_TANGRAM2_BASE+2)
#define CMDID_TANGRAM2_SIMULATION_STEP (CMDID_TANGRAM2_BASE+3)
#define CMDID_TANGRAM2_END_SIMULATOR   (CMDID_TANGRAM2_BASE+4)

#define MAXTANGRAM2CMDIDS (CMDID_ABOUT_TANGRAM2+5)

extern		char cmdLineGenParser[MAXSTRING];

extern		TgMenuInfo tangram2MenuInfo;

#ifdef _INCLUDE_FROM_TANGRAM2_C_
#undef extern
#define extern
#endif /*_INCLUDE_FROM_TANGRAM2_C_*/

/* ======================= Init & CleanUp ======================= */

extern void	CleanUpTangram2 ARGS_DECL((void));
extern int	InitTangram2 ARGS_DECL((void));

/* ======================= Tangram2 Shortcuts ======================= */

extern void	CleanUpTangram2ShortCut ARGS_DECL((void));
extern int	InitTangram2ShortCut ARGS_DECL((void));

extern int	ValidTangram2CmdName ARGS_DECL((char *buf, int buf_sz,
		                                int *pn_num_args));
extern int	ValidTangram2CmdId ARGS_DECL((int nCmdId));
extern int	DoTangram2Cmd ARGS_DECL((int nCmdId, char *args));
extern int	FetchTangram2ShortCutNumArgs ARGS_DECL((int index,
		                                        int *pn_num_args));
extern int	DoTangram2ShortCut ARGS_DECL((int index, char *args));

/* ======================= Tangram2 Internal Commands ======================= */

extern ExecInfo	*Tangram2GetExecInfo ARGS_DECL((char *func_name));

/* ======================= Tangram2 Menu ======================= */

extern int	RefreshTangram2Menu ARGS_DECL((TgMenu*));
extern TgMenu	*CreateTangram2Menu ARGS_DECL((TgMenu*, int X, int Y,
		                               TgMenuInfo*,
		                               int status_str_xlated));
extern int	Tangram2Menu ARGS_DECL((int X, int Y, int TrackMenubar));

/* ======================= Tangram2 Menu Functions ======================= */

extern void	AboutTangram2 ARGS_DECL((void));
extern void	Tangram2GenerateChain ARGS_DECL((char*));
extern void	Tangram2StartSimulator ARGS_DECL((char*));
extern void	Tangram2SimulationStep ARGS_DECL((void));
extern void	Tangram2EndSimulator ARGS_DECL((void));

/* ======================= Tangram2 Misc Functions ======================= */

extern void	Tangram2UpdateInitAttr ARGS_DECL((struct ObjRec *, char*));

#ifdef _INCLUDE_FROM_TANGRAM2_C_
#undef extern
#ifndef _NO_RECURSIVE_EXTERN
#define extern extern
#endif /* ~_NO_RECURSIVE_EXTERN */
#endif /*_INCLUDE_FROM_TANGRAM2_C_*/

#endif /*_TGIF_TANGRAM2_E_*/
