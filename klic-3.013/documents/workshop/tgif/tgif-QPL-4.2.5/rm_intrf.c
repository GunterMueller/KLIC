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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/rm_intrf.c,v 1.17 2011/05/16 16:21:59 william Exp $
 */

#define _INCLUDE_FROM_RM_INTRF_C_

#include "tgifdefs.h"
#include "cmdids.h"

#include "dialog.e"
#include "menu.e"
#include "msg.e"
#include "rm_intrf.e"
#include "strtbl.e"
#include "util.e"

#ifndef _NO_DL_SUPPORT
#define CLOSE_DL(handle) dlclose(handle)
#define OPEN_DL(path,flag) dlopen((path),(flag))
#define GET_DL_SYM(handle,symbol) dlsym((handle),(symbol))
#ifndef RTLD_NOW
#define OPEN_DL_MODE 1
#else /* RTLD_NOW */
#define OPEN_DL_MODE (RTLD_NOW|RTLD_GLOBAL)
#endif /* ~RTLD_NOW */
#else /* _NO_DL_SUPPORT */
#define CLOSE_DL(handle)
#define OPEN_DL(path,flag) NULL
#define GET_DL_SYM(handle,symbol) NULL
#define OPEN_DL_MODE 0
#endif /* ~_NO_DL_SUPPORT */

char cmdLineRMCastLibDir[MAXPATHLENGTH+1];
char cmdLineRMCastLibPath[MAXPATHLENGTH+1];
RMCastIntrfInfo gRMCastIntrfInfo;

/* ------------------ Utility Functions ------------------ */

/* ------------------ RMCastCleanUp() ------------------ */

void RMCastCleanUp()
{
#ifndef _NO_RMCAST_SO
   if (gRMCastIntrfInfo.pSharedLibHandle != NULL) {
      CLOSE_DL(gRMCastIntrfInfo.pSharedLibHandle);
   }
#endif /* ~_NO_RMCAST_SO */
   memset(&gRMCastIntrfInfo, 0, sizeof(RMCastIntrfInfo));
}

/* ------------------ RMCastInit() ------------------ */

static
int GetEntryPointFailed(psz_funcname)
   char *psz_funcname;
{
   snprintf(gszMsgBox, sizeof(gszMsgBox),
         TgLoadString(STID_FAIL_GET_ENTRY_PT_IN_SHARELIB),
         psz_funcname, gRMCastIntrfInfo.szPath);
   fprintf(stderr, "%s\n", gszMsgBox);
   if (gRMCastIntrfInfo.pSharedLibHandle != NULL) {
      CLOSE_DL(gRMCastIntrfInfo.pSharedLibHandle);
   }
   memset(&gRMCastIntrfInfo, 0, sizeof(RMCastIntrfInfo));

   return FALSE;
}

typedef struct tagInterfaceVerInfo {
   char sz_interface_type[80];
   int major_version;
   char sz_other_version_info[80];
   char sz_additional_info[MAXSTRING];
} InterfaceVerInfo;

static
int ParseInterfaceVersion(buf, pivi)
   char *buf;
   InterfaceVerInfo *pivi;
{
   char *psz=NULL, *psz2=NULL;

   memset(pivi, 0, sizeof(InterfaceVerInfo));

   psz = strchr(buf, ' ');
   if (psz == NULL) return FALSE;
   *psz = '\0';
   UtilStrCpyN(pivi->sz_interface_type, sizeof(pivi->sz_interface_type), buf);
   *psz++ = ' ';

   buf = psz;
   psz = strchr(buf, ' ');
   if (psz != NULL) {
      *psz = '\0';
      UtilStrCpyN(pivi->sz_additional_info, sizeof(pivi->sz_additional_info),
            &psz[1]);
   }
   psz2 = strchr(buf, '.');
   if (psz2  == NULL) return FALSE;
   *psz2 = '\0';
   if (sscanf(buf, "%d", &pivi->major_version) != 1) {
      *psz2 = '.';
      return FALSE;
   }
   *psz2++ = '.';
   UtilStrCpyN(pivi->sz_other_version_info,
         sizeof(pivi->sz_other_version_info), psz2);
   if (psz != NULL) {
      *psz++ = ' ';
   }
   return TRUE;
}

static
int RMCastInterface1Init(pivi)
   InterfaceVerInfo *pivi;
{
   gRMCastIntrfInfo.pfn_RM_initialize =
         (RM_initialize_Func*)GET_DL_SYM(gRMCastIntrfInfo.pSharedLibHandle,
         "RM_initialize");
   if (gRMCastIntrfInfo.pfn_RM_initialize == NULL) {
      return GetEntryPointFailed("RM_initialize");
   }
   gRMCastIntrfInfo.pfn_RM_joinGroup =
         (RM_joinGroup_Func*)GET_DL_SYM(gRMCastIntrfInfo.pSharedLibHandle,
         "RM_joinGroup");
   if (gRMCastIntrfInfo.pfn_RM_joinGroup == NULL) {
      return GetEntryPointFailed("RM_joinGroup");
   }
   gRMCastIntrfInfo.pfn_RM_leaveGroup =
         (RM_leaveGroup_Func*)GET_DL_SYM(gRMCastIntrfInfo.pSharedLibHandle,
         "RM_leaveGroup");
   if (gRMCastIntrfInfo.pfn_RM_leaveGroup == NULL) {
      return GetEntryPointFailed("RM_leaveGroup");
   }
   gRMCastIntrfInfo.pfn_RM_terminate =
         (RM_terminate_Func*)GET_DL_SYM(gRMCastIntrfInfo.pSharedLibHandle,
         "RM_terminate");
   if (gRMCastIntrfInfo.pfn_RM_terminate == NULL) {
      return GetEntryPointFailed("RM_terminate");
   }
   gRMCastIntrfInfo.pfn_RM_sendto =
         (RM_sendto_Func*)GET_DL_SYM(gRMCastIntrfInfo.pSharedLibHandle,
         "RM_sendto");
   if (gRMCastIntrfInfo.pfn_RM_sendto == NULL) {
      return GetEntryPointFailed("RM_sendto");
   }
   gRMCastIntrfInfo.pfn_RM_recv =
         (RM_recv_Func*)GET_DL_SYM(gRMCastIntrfInfo.pSharedLibHandle,
         "RM_recv");
   if (gRMCastIntrfInfo.pfn_RM_recv == NULL) {
      return GetEntryPointFailed("RM_recv");
   }
   gRMCastIntrfInfo.pfn_RM_getCurStatus =
         (RM_getCurStatus_Func*)GET_DL_SYM(gRMCastIntrfInfo.pSharedLibHandle,
         "RM_getCurStatus");
   if (gRMCastIntrfInfo.pfn_RM_getCurStatus == NULL) {
      return GetEntryPointFailed("RM_getCurStatus");
   }
   gRMCastIntrfInfo.pfn_RM_sendCurStatus =
         (RM_sendCurStatus_Func*)GET_DL_SYM(gRMCastIntrfInfo.pSharedLibHandle,
         "RM_sendCurStatus");
   if (gRMCastIntrfInfo.pfn_RM_sendCurStatus == NULL) {
      return GetEntryPointFailed("RM_sendCurStatus");
   }
   gRMCastIntrfInfo.pfn_RM_readConfigFile =
         (RM_readConfigFile_Func*)GET_DL_SYM(gRMCastIntrfInfo.pSharedLibHandle,
         "RM_readConfigFile");
   if (gRMCastIntrfInfo.pfn_RM_readConfigFile == NULL) {
      return GetEntryPointFailed("RM_readConfigFile");
   }
   gRMCastIntrfInfo.pfn_RM_getOption =
         (RM_getOption_Func*)GET_DL_SYM(gRMCastIntrfInfo.pSharedLibHandle,
         "RM_getOption");
   if (gRMCastIntrfInfo.pfn_RM_getOption == NULL) {
      return GetEntryPointFailed("RM_getOption");
   }
   gRMCastIntrfInfo.pfn_RM_setOption =
         (RM_setOption_Func*)GET_DL_SYM(gRMCastIntrfInfo.pSharedLibHandle,
         "RM_setOption");
   if (gRMCastIntrfInfo.pfn_RM_setOption == NULL) {
      return GetEntryPointFailed("RM_setOption");
   }
   gRMCastIntrfInfo.pfn_RM_setHostDelay =
         (RM_setHostDelay_Func*)GET_DL_SYM(gRMCastIntrfInfo.pSharedLibHandle,
         "RM_setHostDelay");
   if (gRMCastIntrfInfo.pfn_RM_setHostDelay == NULL) {
      return GetEntryPointFailed("RM_setHostDelay");
   }
   gRMCastIntrfInfo.pfn_RM_getHostDelay =
         (RM_getHostDelay_Func*)GET_DL_SYM(gRMCastIntrfInfo.pSharedLibHandle,
         "RM_getHostDelay");
   if (gRMCastIntrfInfo.pfn_RM_getHostDelay == NULL) {
      return GetEntryPointFailed("RM_getHostDelay");
   }
   gRMCastIntrfInfo.pfn_RMDEBUG_setpidip =
         (RMDEBUG_setpidip_Func*)GET_DL_SYM(gRMCastIntrfInfo.pSharedLibHandle,
         "RMDEBUG_setpidip");
   if (gRMCastIntrfInfo.pfn_RMDEBUG_setpidip == NULL) {
      return GetEntryPointFailed("RMDEBUG_setpidip");
   }
   gRMCastIntrfInfo.pfn_RMDEBUG_setsn =
         (RMDEBUG_setsn_Func*)GET_DL_SYM(gRMCastIntrfInfo.pSharedLibHandle,
         "RMDEBUG_setsn");
   if (gRMCastIntrfInfo.pfn_RMDEBUG_setsn == NULL) {
      return GetEntryPointFailed("RMDEBUG_setsn");
   }
   return TRUE;
}

#define LIBRMCAST_SO "librmcast.so"

int RMCastInit()
{
#ifndef _NO_RMCAST_SO
   char sz_interface_version[MAXSTRING];
   InterfaceVerInfo ivi;

   memset(&gRMCastIntrfInfo, 0, sizeof(RMCastIntrfInfo));
   memset(&ivi, 0, sizeof(InterfaceVerInfo));

   if (*cmdLineRMCastLibPath != '\0') {
      UtilStrCpyN(gRMCastIntrfInfo.szPath, sizeof(gRMCastIntrfInfo.szPath),
            cmdLineRMCastLibPath);
      if (!UtilPathExists(gRMCastIntrfInfo.szPath)) {
         snprintf(gszMsgBox, sizeof(gszMsgBox),
               TgLoadString(STID_INVALID_RMCAST_DLIB_PATH),
               gRMCastIntrfInfo.szPath, TOOL_NAME);
         MsgBox(gszMsgBox, TOOL_NAME, STOP_MB);
         SendCommandToSelf(CMDID_QUIT, 0);
         return FALSE;
      }
   } else if (*cmdLineRMCastLibDir == '\0') {
      char *psz=getenv("LD_LIBRARY_PATH"), *psz_copy=NULL, *psz_state=NULL;
      int found=FALSE;

      if (psz == NULL) {
         snprintf(gRMCastIntrfInfo.szPath, sizeof(gRMCastIntrfInfo.szPath),
               "%s", LIBRMCAST_SO);
         gRMCastIntrfInfo.pSharedLibHandle = OPEN_DL(gRMCastIntrfInfo.szPath,
               OPEN_DL_MODE);
         if (gRMCastIntrfInfo.pSharedLibHandle == NULL) {
            snprintf(gszMsgBox, sizeof(gszMsgBox),
                  TgLoadString(STID_NO_INFO_LIBRMCAST_SO),
                  LIBRMCAST_SO, LIBRMCAST_SO, TOOL_NAME, TOOL_NAME);
            MsgBox(gszMsgBox, TOOL_NAME, STOP_MB);
            SendCommandToSelf(CMDID_QUIT, 0);
            return FALSE;
         }
      } else {
         psz_copy = UtilStrDup(psz);
         if (psz_copy == NULL) FailAllocMessage();
         for (psz=UtilStrTok(psz_copy, ":", &psz_state); psz != NULL;
               psz=UtilStrTok(NULL, ":", &psz_state)) {
            snprintf(gRMCastIntrfInfo.szPath, sizeof(gRMCastIntrfInfo.szPath),
                  "%s%c%s", psz, DIR_SEP, LIBRMCAST_SO);
            if (UtilPathExists(gRMCastIntrfInfo.szPath)) {
               found = TRUE;
               break;
            }
         }
         UtilFree(psz_copy);
         if (!found) {
            snprintf(gRMCastIntrfInfo.szPath, sizeof(gRMCastIntrfInfo.szPath),
                  "%s", LIBRMCAST_SO);
            gRMCastIntrfInfo.pSharedLibHandle = OPEN_DL(gRMCastIntrfInfo.szPath,
                  OPEN_DL_MODE);
            if (gRMCastIntrfInfo.pSharedLibHandle == NULL) {
               snprintf(gszMsgBox, sizeof(gszMsgBox),
                     TgLoadString(STID_NO_INFO_LIBRMCAST_SO),
                     LIBRMCAST_SO, LIBRMCAST_SO, TOOL_NAME, TOOL_NAME);
               MsgBox(gszMsgBox, TOOL_NAME, STOP_MB);
               SendCommandToSelf(CMDID_QUIT, 0);
               return FALSE;
            }
            /* return FALSE; -- seems to not work with -parent, shouldn't really get here anyway */
         }
      }
   } else {
      snprintf(gRMCastIntrfInfo.szPath, sizeof(gRMCastIntrfInfo.szPath),
            "%s%c%s", cmdLineRMCastLibDir, DIR_SEP, LIBRMCAST_SO);
      if (!UtilPathExists(gRMCastIntrfInfo.szPath)) {
         snprintf(gszMsgBox, sizeof(gszMsgBox),
               TgLoadString(STID_INVALID_RMCAST_DLIB_PATH),
               gRMCastIntrfInfo.szPath, TOOL_NAME);
         MsgBox(gszMsgBox, TOOL_NAME, STOP_MB);
         SendCommandToSelf(CMDID_QUIT, 0);
         return FALSE;
      }
   }
   if (gRMCastIntrfInfo.pSharedLibHandle == NULL) {
       gRMCastIntrfInfo.pSharedLibHandle = OPEN_DL(gRMCastIntrfInfo.szPath,
             OPEN_DL_MODE);
       if (gRMCastIntrfInfo.pSharedLibHandle == NULL) {
          snprintf(gszMsgBox, sizeof(gszMsgBox),
                TgLoadString(STID_FAIL_LOAD_SHARELIB),
                gRMCastIntrfInfo.szPath);
          MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
          return FALSE;
       }
   }
   gRMCastIntrfInfo.pfn_RM_getVersion =
         (RM_getVersion_Func*)GET_DL_SYM(
         gRMCastIntrfInfo.pSharedLibHandle, "RM_getVersion");
   if (gRMCastIntrfInfo.pfn_RM_getVersion == NULL) {
      return GetEntryPointFailed("RM_getVersion");
   }
   RM_getVersion(sz_interface_version, sizeof(sz_interface_version));
   if (!ParseInterfaceVersion(sz_interface_version, &ivi)) {
      snprintf(gszMsgBox, sizeof(gszMsgBox),
            TgLoadString(STID_BAD_VER_RMCAST_DLIB),
            sz_interface_version, gRMCastIntrfInfo.szPath);
      fprintf(stderr, "%s\n", gszMsgBox);
      CLOSE_DL(gRMCastIntrfInfo.pSharedLibHandle);
      memset(&gRMCastIntrfInfo, 0, sizeof(RMCastIntrfInfo));
      return FALSE;
   }
   if (strcmp(ivi.sz_interface_type, "RMCAST") == 0 &&
         ivi.major_version == 2) {
      return RMCastInterface1Init(&ivi);
   } else {
      snprintf(gszMsgBox, sizeof(gszMsgBox),
            TgLoadString(STID_CANNOT_HANDLE_VER_RMCAST_DLIB),
            TOOL_NAME, sz_interface_version, gRMCastIntrfInfo.szPath);
      fprintf(stderr, "%s\n", gszMsgBox);
      CLOSE_DL(gRMCastIntrfInfo.pSharedLibHandle);
      memset(&gRMCastIntrfInfo, 0, sizeof(RMCastIntrfInfo));
      return FALSE;
   }
#else /* ~_NO_RMCAST_SO */
   memset(&gRMCastIntrfInfo, 0, sizeof(RMCastIntrfInfo));
#endif /* _NO_RMCAST_SO */

   return FALSE;
}

/* ------------------ Functions in RMCAST library ------------------ */

#ifndef _NO_RMCAST_SO
void RM_getVersion(buf, buf_sz)
   char *buf;
   int buf_sz;
{
   if (gRMCastIntrfInfo.pSharedLibHandle != NULL &&
         gRMCastIntrfInfo.pfn_RM_getVersion != NULL) {
      (gRMCastIntrfInfo.pfn_RM_getVersion)(buf, buf_sz);
      return;
   }
   TgAssert(FALSE, "RM_getVersion() called when it's not available.",
         NULL);
}

int RM_initialize(void *callbackterm( void ))
{
   if (gRMCastIntrfInfo.pSharedLibHandle != NULL &&
         gRMCastIntrfInfo.pfn_RM_initialize != NULL) {
      return (gRMCastIntrfInfo.pfn_RM_initialize)(*callbackterm);
   }
   TgAssert(FALSE, "RM_initialize() called when it's not available.", NULL);

   return 0;
}

int RM_joinGroup(group, port)
   char *group;
   int port;
{
   if (gRMCastIntrfInfo.pSharedLibHandle != NULL &&
         gRMCastIntrfInfo.pfn_RM_joinGroup != NULL) {
      return (gRMCastIntrfInfo.pfn_RM_joinGroup)(group, port);
   }
   TgAssert(FALSE, "RM_joinGroup() called when it's not available.", NULL);

   return 0;
}

void RM_leaveGroup(sock, group)
   int sock;
   char *group;
{
   if (gRMCastIntrfInfo.pSharedLibHandle != NULL &&
         gRMCastIntrfInfo.pfn_RM_leaveGroup != NULL) {
      (gRMCastIntrfInfo.pfn_RM_leaveGroup)(sock, group);
      return;
   }
   TgAssert(FALSE, "RM_leaveGroup() called when it's not available.", NULL);
}

void RM_terminate()
{
   if (gRMCastIntrfInfo.pSharedLibHandle != NULL &&
         gRMCastIntrfInfo.pfn_RM_terminate != NULL) {
      (gRMCastIntrfInfo.pfn_RM_terminate)();
      return ;
   }
   TgAssert(FALSE, "RM_terminate() called when it's not available.", NULL);
}

int RM_sendto(socket, buffer, buffsize)
   int socket, buffsize;
   void *buffer;
{
   if (gRMCastIntrfInfo.pSharedLibHandle != NULL &&
         gRMCastIntrfInfo.pfn_RM_sendto != NULL) {
      return (gRMCastIntrfInfo.pfn_RM_sendto)(socket, buffer, buffsize);
   }
   TgAssert(FALSE, "RM_sendto() called when it's not available.", NULL);

   return 0;
}

int RM_recv(socket, buffer, buffsize)
   int socket, buffsize;
   void *buffer;
{
   if (gRMCastIntrfInfo.pSharedLibHandle != NULL &&
         gRMCastIntrfInfo.pfn_RM_recv != NULL) {
      return (gRMCastIntrfInfo.pfn_RM_recv)(socket, buffer, buffsize);
   }
   TgAssert(FALSE, "RM_recv() called when it's not available.", NULL);

   return 0;
}

int RM_getCurStatus(group, port, c)
   char *group;
   int port;
   CurStatus *c;
{
   if (gRMCastIntrfInfo.pSharedLibHandle != NULL &&
         gRMCastIntrfInfo.pfn_RM_getCurStatus != NULL) {
      return (gRMCastIntrfInfo.pfn_RM_getCurStatus)(group, port, c);
   }
   TgAssert(FALSE, "RM_getCurStatus() called when it's not available.", NULL);

   return 0;
}

int RM_sendCurStatus(connfd, buff, buffsize)
   int connfd, buffsize;
   char *buff;
{
   if (gRMCastIntrfInfo.pSharedLibHandle != NULL &&
         gRMCastIntrfInfo.pfn_RM_sendCurStatus != NULL) {
      return (gRMCastIntrfInfo.pfn_RM_sendCurStatus)(connfd, buff, buffsize);
   }
   TgAssert(FALSE, "RM_sendCurStatus() called when it's not available.", NULL);

   return 0;
}

int RM_readConfigFile(filename, show_config_file)
   char *filename, show_config_file;
{
   if (gRMCastIntrfInfo.pSharedLibHandle != NULL &&
         gRMCastIntrfInfo.pfn_RM_readConfigFile != NULL) {
      return (gRMCastIntrfInfo.pfn_RM_readConfigFile)(filename,
            show_config_file);
   }
   TgAssert(FALSE, "RM_readConfigFile() called when it's not available.", NULL);

   return 0;
}

void RM_getOption(opt, return_value)
   int opt;
   void *return_value;
{
   if (gRMCastIntrfInfo.pSharedLibHandle != NULL &&
         gRMCastIntrfInfo.pfn_RM_getOption != NULL) {
      (gRMCastIntrfInfo.pfn_RM_getOption)(opt, return_value);
      return;
   }
   TgAssert(FALSE, "RM_getOption() called when it's not available.", NULL);
}

void RM_setOption(opt, optvalue)
   int opt;
   void *optvalue;
{
   if (gRMCastIntrfInfo.pSharedLibHandle != NULL &&
         gRMCastIntrfInfo.pfn_RM_setOption != NULL) {
      (gRMCastIntrfInfo.pfn_RM_setOption)(opt, optvalue);
      return;
   }
   TgAssert(FALSE, "RM_setOption() called when it's not available.", NULL);
}

int RM_setHostDelay(hostname, estimated_one_way_delay)
   char *hostname;
   int estimated_one_way_delay;
{
   if (gRMCastIntrfInfo.pSharedLibHandle != NULL &&
         gRMCastIntrfInfo.pfn_RM_setHostDelay != NULL) {
      return (gRMCastIntrfInfo.pfn_RM_setHostDelay)(hostname,
            estimated_one_way_delay);
   }
   TgAssert(FALSE, "RM_setHostDelay() called when it's not available.", NULL);

   return 0;
}

int RM_getHostDelay(hostname, estimated_one_way_delay)
   char *hostname;
   int *estimated_one_way_delay;
{
   if (gRMCastIntrfInfo.pSharedLibHandle != NULL &&
         gRMCastIntrfInfo.pfn_RM_getHostDelay != NULL) {
      return (gRMCastIntrfInfo.pfn_RM_getHostDelay)(hostname,
            estimated_one_way_delay);
   }
   TgAssert(FALSE, "RM_getHostDelay() called when it's not available.", NULL);

   return 0;
}

void RMDEBUG_setpidip(pid, ip)
   int pid;
   char *ip;
{
   if (gRMCastIntrfInfo.pSharedLibHandle != NULL &&
         gRMCastIntrfInfo.pfn_RMDEBUG_setpidip != NULL) {
      (gRMCastIntrfInfo.pfn_RMDEBUG_setpidip)(pid, ip);
      return;
   }
   TgAssert(FALSE, "RMDEBUG_setpidip() called when it's not available.", NULL);
}

void RMDEBUG_setsn(sn)
   int sn;
{
   if (gRMCastIntrfInfo.pSharedLibHandle != NULL &&
         gRMCastIntrfInfo.pfn_RMDEBUG_setsn != NULL) {
      (gRMCastIntrfInfo.pfn_RMDEBUG_setsn)(sn);
      return;
   }
   TgAssert(FALSE, "RMDEBUG_setsn() called when it's not available.", NULL);
}
#endif /* _NO_RMCAST_SO */

