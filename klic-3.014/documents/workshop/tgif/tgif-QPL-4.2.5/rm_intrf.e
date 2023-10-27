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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/rm_intrf.e,v 1.10 2011/05/16 16:21:59 william Exp $
 */

#ifndef _RM_INTRF_E_
#define _RM_INTRF_E_

#include "rmcast/src/rmcast.h"

typedef void *TGEF_USER_INFO;
typedef long (TGEF_MAIN)ARGS_DECL((TGEF_USER_INFO*, int, int, void*));

typedef void (RM_getVersion_Func)ARGS_DECL((char *buf, int buf_sz));
typedef int  (RM_initialize_Func)ARGS_DECL((void *callbackterm( void )));
typedef int  (RM_joinGroup_Func)ARGS_DECL((char *group, int port));
typedef void (RM_leaveGroup_Func)ARGS_DECL((int sock, char *group));
typedef void (RM_terminate_Func)ARGS_DECL((void));
typedef int  (RM_sendto_Func)ARGS_DECL((int socket, void *buffer,
                                        int buffsize));
typedef int  (RM_recv_Func)ARGS_DECL((int socket, void *buffer, int buffsize));
typedef int  (RM_getCurStatus_Func)ARGS_DECL((char *group, int port,
                                              CurStatus *c));
typedef int  (RM_sendCurStatus_Func)ARGS_DECL((int connfd, char *buff,
                                               int buffsize));
typedef int  (RM_readConfigFile_Func)ARGS_DECL((char *filename,
                                                char show_config_file));
typedef void (RM_getOption_Func)ARGS_DECL((int opt, void *return_value));
typedef void (RM_setOption_Func)ARGS_DECL((int opt, void *optvalue));
typedef int  (RM_setHostDelay_Func)ARGS_DECL((char *hostname,
                                              int estimated_one_way_delay));
typedef int  (RM_getHostDelay_Func)ARGS_DECL((char *hostname,
                                              int *estimated_one_way_delay));
typedef void (RMDEBUG_setpidip_Func)ARGS_DECL((int pid, char *ip));
typedef void (RMDEBUG_setsn_Func)ARGS_DECL((int sn));

typedef struct tagRMCastIntrfInfo {
   char szPath[MAXPATHLENGTH+1];

   RM_getVersion_Func *pfn_RM_getVersion;
   RM_initialize_Func *pfn_RM_initialize;
   RM_joinGroup_Func *pfn_RM_joinGroup;
   RM_leaveGroup_Func *pfn_RM_leaveGroup;
   RM_terminate_Func *pfn_RM_terminate; /* no need */
   RM_sendto_Func *pfn_RM_sendto;
   RM_recv_Func *pfn_RM_recv;
   RM_getCurStatus_Func *pfn_RM_getCurStatus;
   RM_sendCurStatus_Func *pfn_RM_sendCurStatus;
   RM_readConfigFile_Func *pfn_RM_readConfigFile;
   RM_getOption_Func *pfn_RM_getOption;
   RM_setOption_Func *pfn_RM_setOption;
   RM_setHostDelay_Func *pfn_RM_setHostDelay; /* no need */
   RM_getHostDelay_Func *pfn_RM_getHostDelay; /* no need */
   RMDEBUG_setpidip_Func *pfn_RMDEBUG_setpidip; /* no need */
   RMDEBUG_setsn_Func *pfn_RMDEBUG_setsn; /* no need */

   void *pSharedLibHandle;
} RMCastIntrfInfo;

extern char cmdLineRMCastLibDir[MAXPATHLENGTH+1];
extern char cmdLineRMCastLibPath[MAXPATHLENGTH+1];
extern RMCastIntrfInfo gRMCastIntrfInfo;

#ifdef _INCLUDE_FROM_RM_INTRF_C_
#undef extern
#define extern
#endif /*_INCLUDE_FROM_RM_INTRF_C_*/

extern void	RMCastCleanUp ARGS_DECL((void));
extern int	RMCastInit ARGS_DECL((void));

#ifndef _NO_RMCAST_SO
extern void	RM_getVersion ARGS_DECL((char *buf, int buf_sz));
extern int	RM_initialize ARGS_DECL((void *callbackterm( void )));
extern int	RM_joinGroup ARGS_DECL((char *group, int port));
extern void	RM_leaveGroup ARGS_DECL((int sock, char *group));
extern void	RM_terminate ARGS_DECL((void));
extern int	RM_sendto ARGS_DECL((int socket, void *buffer, int buffsize));
extern int	RM_recv ARGS_DECL((int socket, void *buffer, int buffsize));
extern int	RM_getCurStatus ARGS_DECL((char *group, int port,
                                           CurStatus *c));
extern int	RM_sendCurStatus ARGS_DECL((int connfd, char *buff,
                                            int buffsize));
extern int	RM_readConfigFile ARGS_DECL((char *filename,
                                             char show_config_file));
extern void	RM_getOption ARGS_DECL((int opt, void *return_value));
extern void	RM_setOption ARGS_DECL((int opt, void *optvalue));
extern int	RM_setHostDelay ARGS_DECL((char *hostname,
                                           int estimated_one_way_delay));
extern int	RM_getHostDelay ARGS_DECL((char *hostname,
                                           int *estimated_one_way_delay));
extern void	RMDEBUG_setpidip ARGS_DECL((int pid, char *ip));
extern void	RMDEBUG_setsn ARGS_DECL((int sn));
#endif /* _NO_RMCAST_SO */

#ifdef _INCLUDE_FROM_RM_INTRF_C_
#undef extern
#ifndef _NO_RECURSIVE_EXTERN
#define extern extern
#endif /* ~_NO_RECURSIVE_EXTERN */
#endif /*_INCLUDE_FROM_RM_INTRF_C_*/

#endif /*_RM_INTRF_E_*/
