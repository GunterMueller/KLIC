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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/expfdefs.h,v 1.6 2011/05/16 16:21:57 william Exp $
 */

#ifndef _EXPFDEFS_H_
#define _EXPFDEFS_H_

/*
 * A tgif export filter is a shared library having a name like
 *     lib<name>.so.<ver> where <name> names the export filter type
 *     and <ver> is a ``protocol version''.
 *
 * The main entry point of the shared library must be:
 *
 *         long TgifExportFilterMain(
 *                 TGEF_USER_INFO *ppUserInfo;
 *                 int nMsg;
 *                 int nArg;
 *                 void *pvArg
 *         );
 *
 *     *ppUserInfo is a place to hang internal data for the export filter
 *     nMsg is the message type, which is one of TGEF_MSG_*
 *     nArg and pvArg are arguments for a message
 */

#include "tgifdefs.h"

#ifdef _DL_EXPORT_UNDERSCORE
#define TGEF_MAIN_NAME "_TgifExportFilterMain"
#else /* ~_DL_EXPORT_UNDERSCORE */
#define TGEF_MAIN_NAME "TgifExportFilterMain"
#endif /* _DL_EXPORT_UNDERSCORE */

typedef void *TGEF_USER_INFO;
typedef long (TGEF_MAIN)ARGS_DECL((TGEF_USER_INFO*, int, int, void*));

typedef int   (MsgBoxFunc)ARGS_DECL((char*, char*, int));
typedef int   (DialogFunc)ARGS_DECL((char*, char*, char*));
typedef int   (FailAllocMessageFunc)ARGS_DECL((void));
typedef int   (UtilCopyFileFunc)ARGS_DECL((char *pszFromFile, char *pszToFile));
typedef int   (ObjInVisibleLayerFunc)ARGS_DECL((struct ObjRec *));
typedef char *(UtilStrDupFunc)ARGS_DECL((char*));
typedef void  (UtilFreeFunc)ARGS_DECL((char*));
typedef void  (MsgFunc)ARGS_DECL((char*));
typedef void  (SetStringStatusFunc)ARGS_DECL((char*));
typedef void  (UtilTrimBlanksFunc)ARGS_DECL((char*));
typedef void  (DynStrSetFunc)ARGS_DECL((struct DynStrRec *, char*));

typedef struct tagTgEFProtocolInfo_1 {
   /* in  */ Display *dpy;
   /* in  */ Window win;
   /* in  */ char sz_sl_path[MAXPATHLENGTH];
   /* in  */ NLFN *pfnMsgBox;
   /* in  */ NLFN *pfnDialog;
   /* in  */ NLFN *pfnFailAllocMessage;
   /* in  */ NLFN *pfnUtilCopyFile;
   /* in  */ NLFN *pfnObjInVisibleLayer;
   /* in  */ NLFN *pfnUtilStrDup;
   /* in  */ VDFN *pfnUtilFree;
   /* in  */ VDFN *pfnMsg;
   /* in  */ VDFN *pfnSetStringStatus;
   /* in  */ VDFN *pfnUtilTrimBlanks;
   /* in  */ VDFN *pfnDynStrSet;

   /* Must fill in the following information in response to TGEF_MSG_LOAD: */
      /* out */ char *bm_bits;
      /* out */ int bm_width;
      /* out */ int bm_height;
      /* out */ char color_format_name[14]; /* format name */
      /* out */ char bw_format_name[14]; /* format name */
      /* out */ char color_ext[14]; /* file extension */
      /* out */ char bw_ext[14]; /* file extension */
      /* out */ int capability; /* TGEF_CAP_* */

   /* These are filled in when TGEF_MSG_DO_EXPORT is sent */
      /* in  */ FILE *fp;
      /* in  */ int color_dump;
      /* in  */ int color_layers;
      /* in  */ int printing_page_num;
      /* in  */ int printing_first_page_num;
      /* in  */ int printing_page_row;
      /* in  */ int printing_page_col;
      /* in  */ int page_layout_mode;
      /* in  */ int last_page_num;
      /* in  */ struct ObjRec *top_obj;
      /* in  */ struct ObjRec *bot_obj;
      /* out */ int write_file_failed;
} TgEFProtocolInfo_1;

typedef struct tagTgEFInfo {
   TGEF_USER_INFO pTgEFUserInfo;
   TGEF_MAIN *pfnTgEFMain;
   void *pSharedLibHandle;
   char *pszSharedLibPath;
   int nSharedLibVersion;
   union {
      TgEFProtocolInfo_1 stProtocolInfo_1;
   } detail;
} TgEFInfo; /* Used by tgif only.  Not to be used by any export filter. */

#define TGEF_STATUS_OK 0
#define TGEF_STATUS_UNKNOWN 1
#define TGEF_STATUS_INVALID_ARGS 2
#define TGEF_STATUS_MALLOC 3

/*
 * capability
 *
 *     Use TGEF_CAP_SINGLE_PAGE if the file format is EPS-like where
 *         it doesn't make sense to have a multi-page output.
 *     Otherwise, use TGEF_CAP_MULTI_PAGE.
 */

#define TGEF_CAP_SINGLE_PAGE 0x0000 /* cannot handle multi-pg drawing */
#define TGEF_CAP_MULTI_PAGE  0x0001 /* handle multi-pg drawing */

/*
 * message types
 */

#define TGEF_MSG_INIT 0
#define TGEF_MSG_GET_PROTOCOL_VERSION 1
#define TGEF_MSG_LOAD 2
#define TGEF_MSG_UNLOAD 3
#define TGEF_MSG_CLEANUP 4

#define TGEF_MSG_DO_EXPORT 10

/*
 * Basic messages:
 *
 *     TGEF_MSG_INIT, TGEF_MSG_GET_PROTOCOL_VERSION, TGEF_MSG_LOAD,
 *         TGEF_MSG_UNLOAD, and TGEF_MSG_CLEANUP are basic messages.
 *
 *     They are called in the following sequence:
 *
 *         In InitExportFilters() of "xprtfltr.c":
 *
 *             TGEF_MSG_INIT
 *             TGEF_MSG_GET_PROTOCOL_VERSION
 *             TGEF_MSG_LOAD
 *
 *         In DoGenDump() of "file.c":
 *
 *             Other non-basic messages are sent to do the actual exporting.
 *
 *         In CleanUpExportFilters() of "xprtfltr.c":
 *
 *             TGEF_MSG_UNLOAD
 *             TGEF_MSG_CLEANUP
 *
 * TGEF_MSG_INIT:
 *     On entry:
 *         *ppUserInfo is set to NULL, nArg = 0, pvArg = NULL
 *     On return:
 *         If returns anything other than TGEF_STATUS_OK, the shared
 *             library is unloaded.
 *     Note:
 *         No memory allocation should be performed in the message
 *             handler of this message!  If you want to allocate memory,
 *             do it in TGEF_MSG_LOAD.
 *
 * TGEF_MSG_GET_PROTOCOL_VERSION:
 *     On entry:
 *         nArg = 0, pvArg = NULL
 *     On return:
 *         The returned value should be an integer specifying the
 *             protocol version this export filter implements.  Please
 *             note that if the returned value is different from the
 *             <ver> part of the "lib<name>.so.<ver>", TGEF_MSG_CLEANUP
 *             will be sent immediately.
 *
 * TGEF_MSG_LOAD:
 *     On entry:
 *         *ppUserInfo is set to NULL, nArg = 0, pvArg is a pointer to
 *             TgEFProtocolInfo_<ver> where <ver> is the returned value
 *             of the TGEF_MSG_GET_PROTOCOL_VERSION message.
 *     On return:
 *         If returns anything other than TGEF_STATUS_OK, the shared
 *             library is unloaded.
 *         If returns TGEF_STATUS_OK, *ppUserInfo can be used to hang
 *             internal data for the export filter.  Tgif will not
 *             touch *ppUserInfo from this point on until after the
 *             call with message type TGEF_MSG_UNLOAD.
 *
 * TGEF_MSG_UNLOAD:
 *     On entry:
 *         ppUserInfo = what's set in TGEF_MSG_LOAD, nArg = 0, pvArg = NULL
 *     On return:
 *         *ppUserInfo will be set to NULL immediately afterwards.
 *     Note:
 *         The message handler should free up allocated memory.
 *
 * TGEF_MSG_CLEANUP:
 *     On entry:
 *         ppUserInfo = NULL, nArg = 0, pvArg = NULL
 *     On return:
 *         The shared library is unloaded and the returned value is ignored.
 *     Note:
 *         No memory deallocation should be performed in the message
 *             handler of this message!  If you want to deallocate memory,
 *             do it in TGEF_MSG_UNLOAD.
 *         Usually, there's no need to handle this message if the shared
 *             library can be simply unloaded.
 *
 */

/*
 * TGEF_MSG_DO_EXPORT:
 *     On entry:
 *         ppUserInfo = what's set in TGEF_MSG_LOAD, nArg = 0, pvArg is a
 *             pointer to TgEFProtocolInfo_<ver> where <ver> is the returned
 *             value of the TGEF_MSG_GET_PROTOCOL_VERSION message.
 *     On return:
 *         Should return TGEF_STATUS_OK if everything is okay.
 */

#endif /*_EXPFDEFS_H_*/
