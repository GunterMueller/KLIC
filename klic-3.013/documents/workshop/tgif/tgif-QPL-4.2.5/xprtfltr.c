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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/xprtfltr.c,v 1.6 2011/05/16 16:22:00 william Exp $
 */

#define _INCLUDE_FROM_XPRTFLTR_C_

#include "tgifdefs.h"
#include "expfdefs.h"

#include "color.e"
#include "dialog.e"
#include "drawing.e"
#include "file.e"
#include "grid.e"
#include "msg.e"
#include "names.e"
#include "obj.e"
#include "page.e"
#include "ps.e"
#include "raster.e"
#include "setup.e"
#include "strtbl.e"
#include "text.e"
#include "util.e"
#include "xbitmap.e"
#include "xprtfltr.e"

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

int gnMaxExportFilters=0;

Pixmap *gaExportFilterBitmaps=NULL;

TgEFInfo *gpTgEFInfo=NULL;

/* --------------------- Utility Functions --------------------- */

static
int GetSharedLibVersion(pszPath)
   char *pszPath;
{
   char *psz=UtilStrRChr(pszPath, DIR_SEP), *psz1=NULL;
   int nReturn=0;

   if (psz == NULL) {
      psz = pszPath;
   } else {
      psz++;
   }
   if ((psz=strchr(psz, '.')) == NULL) return 0;
   if ((psz=strchr(++psz, '.')) == NULL) return 0;
   if ((psz1=strchr(++psz, '.')) != NULL) {
      *psz1 = '\0';
      nReturn = atoi(psz);
      *psz1 = '.';
   } else {
      nReturn = atoi(psz);
   }
   return nReturn;
}

static
long TgEFSendMsg(pTgEFInfo, nUserInfo, nMsg, nArg, pvArg)
   TgEFInfo *pTgEFInfo;
   int nUserInfo; /* if TRUE, use &pTgEFInfo->pTgEFUserInfo, else use NULL */
   int nMsg; /* one of TGEF_MSG_* */
   int nArg;
   void *pvArg;
{
   if (nUserInfo) {
      return (pTgEFInfo->pfnTgEFMain)(&pTgEFInfo->pTgEFUserInfo,
            nMsg, nArg, pvArg);
   } else {
      return (pTgEFInfo->pfnTgEFMain)(NULL, nMsg, nArg, pvArg);
   }
}

/* --------------------- Global Routines --------------------- */

void CleanUpExportFilters()
{
   int i=0;

   for (i=0; i < gnMaxExportFilters; i++) {
      if (gaExportFilterBitmaps[i] != None) {
         XFreePixmap(mainDisplay, gaExportFilterBitmaps[i]);
      }
   }
   if (gpTgEFInfo != NULL) {
      for (i=0; i < gnMaxExportFilters; i++) {
         TgEFInfo *pTgEFInfo=(&gpTgEFInfo[i]);

         if (pTgEFInfo != NULL) {
            if (pTgEFInfo->pSharedLibHandle != NULL) {
               TGEF_MAIN *pfnTgEFMain=pTgEFInfo->pfnTgEFMain;

               if (pfnTgEFMain != NULL) {
                  TgEFSendMsg(pTgEFInfo, TRUE, TGEF_MSG_UNLOAD, 0, NULL);
                  TgEFSendMsg(pTgEFInfo, FALSE, TGEF_MSG_CLEANUP, 0, NULL);
               }
               CLOSE_DL(pTgEFInfo->pSharedLibHandle);
            }
            if (pTgEFInfo->pszSharedLibPath != NULL) {
               UtilFree(pTgEFInfo->pszSharedLibPath);
            }
         }
      }
      free(gpTgEFInfo);
   }
   gpTgEFInfo = NULL;
}

static
int SharedLibLoadFailed(pTgEFInfo, pszSharedLibPath, nNeedUnload)
   TgEFInfo *pTgEFInfo;
   char *pszSharedLibPath;
   int nNeedUnload;
   /*
    * This is called when TGEF_MSG_LOAD failed for one reason or another.
    */
{
   if (nNeedUnload) {
      TgEFSendMsg(pTgEFInfo, TRUE, TGEF_MSG_UNLOAD, 0, NULL);
   }
   TgEFSendMsg(pTgEFInfo, FALSE, TGEF_MSG_CLEANUP, 0, NULL);
   CLOSE_DL(pTgEFInfo->pSharedLibHandle);
   return FALSE;
}

static
int LoadSharedLib(pszSharedLibPath)
   char *pszSharedLibPath;
{
   TgEFInfo *pTgEFInfo=(&gpTgEFInfo[gnMaxExportFilters]);
   int nSharedLibVersion=0;
   long lReturn=0L;

   pTgEFInfo->pszSharedLibPath = UtilStrDup(pszSharedLibPath);
   if (pTgEFInfo->pszSharedLibPath == NULL) {
      FailAllocMessage();
   }
   pTgEFInfo->nSharedLibVersion =
         GetSharedLibVersion(pTgEFInfo->pszSharedLibPath);
   if (pTgEFInfo->nSharedLibVersion == 0) {
      sprintf(gszMsgBox, TgLoadString(STID_INVALID_SHARELIB_VER_IN_FILE),
            pszSharedLibPath);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return FALSE;
   }
   pTgEFInfo->pSharedLibHandle = OPEN_DL(pszSharedLibPath, OPEN_DL_MODE);
   if (pTgEFInfo->pSharedLibHandle == NULL) {
      sprintf(gszMsgBox, TgLoadString(STID_FAIL_LOAD_SHARELIB),
            pszSharedLibPath);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return FALSE;
   }
   pTgEFInfo->pfnTgEFMain = (TGEF_MAIN*)GET_DL_SYM(pTgEFInfo->pSharedLibHandle,
         TGEF_MAIN_NAME);
   if (pTgEFInfo->pfnTgEFMain == NULL) {
      sprintf(gszMsgBox, TgLoadString(STID_FAIL_GET_ENTRY_PT_IN_SHARELIB),
            TGEF_MAIN_NAME, pszSharedLibPath);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return FALSE;
   }
   lReturn = TgEFSendMsg(pTgEFInfo, FALSE, TGEF_MSG_INIT, 0, NULL);
   if (lReturn != TGEF_STATUS_OK) {
      sprintf(gszMsgBox, TgLoadString(STID_SHARELIB_TGEF_MSG_INIT_FAIL),
            pszSharedLibPath, lReturn);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      CLOSE_DL(pTgEFInfo->pSharedLibHandle);
      return FALSE;
   }
   nSharedLibVersion = TgEFSendMsg(pTgEFInfo, FALSE,
         TGEF_MSG_GET_PROTOCOL_VERSION, 0, NULL);
   if (nSharedLibVersion != pTgEFInfo->nSharedLibVersion) {
      sprintf(gszMsgBox, TgLoadString(STID_SHARELIB_CNFLCT_PROTOCOL_VER),
            nSharedLibVersion, pszSharedLibPath);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      CLOSE_DL(pTgEFInfo->pSharedLibHandle);
      return FALSE;
   }
   switch (nSharedLibVersion) {
   case 1:
      memset(&pTgEFInfo->detail.stProtocolInfo_1, 0,
            sizeof(TgEFProtocolInfo_1));

      pTgEFInfo->detail.stProtocolInfo_1.dpy = mainDisplay;
      pTgEFInfo->detail.stProtocolInfo_1.win = mainWindow;
      pTgEFInfo->detail.stProtocolInfo_1.pfnMsgBox = (NLFN*)MsgBox;
      pTgEFInfo->detail.stProtocolInfo_1.pfnDialog = (NLFN*)Dialog;
      pTgEFInfo->detail.stProtocolInfo_1.pfnFailAllocMessage =
            (NLFN*)FailAllocMessage;
      pTgEFInfo->detail.stProtocolInfo_1.pfnUtilCopyFile = (NLFN*)UtilCopyFile;
      pTgEFInfo->detail.stProtocolInfo_1.pfnObjInVisibleLayer =
            (NLFN*)ObjInVisibleLayer;
      pTgEFInfo->detail.stProtocolInfo_1.pfnUtilStrDup = (NLFN*)UtilStrDup;
      pTgEFInfo->detail.stProtocolInfo_1.pfnUtilFree = (VDFN*)UtilFree;
      pTgEFInfo->detail.stProtocolInfo_1.pfnMsg = (VDFN*)Msg;
      pTgEFInfo->detail.stProtocolInfo_1.pfnSetStringStatus =
            (VDFN*)SetStringStatus;
      pTgEFInfo->detail.stProtocolInfo_1.pfnUtilTrimBlanks =
            (VDFN*)UtilTrimBlanks;
      pTgEFInfo->detail.stProtocolInfo_1.pfnDynStrSet = (VDFN*)DynStrSet;

      strcpy(pTgEFInfo->detail.stProtocolInfo_1.sz_sl_path, pszSharedLibPath);
      pTgEFInfo->detail.stProtocolInfo_1.bm_bits = NULL;
      pTgEFInfo->detail.stProtocolInfo_1.bm_width = 0;
      pTgEFInfo->detail.stProtocolInfo_1.bm_height = 0;
      lReturn = TgEFSendMsg(pTgEFInfo, TRUE, TGEF_MSG_LOAD, 0,
            &pTgEFInfo->detail.stProtocolInfo_1);
      if (lReturn != TGEF_STATUS_OK) {
         sprintf(gszMsgBox, TgLoadString(STID_SHARELIB_TGEF_MSG_LOAD_FAIL),
               pszSharedLibPath, lReturn);
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
         return SharedLibLoadFailed(pTgEFInfo, pszSharedLibPath, FALSE);
      }
      if (pTgEFInfo->detail.stProtocolInfo_1.bm_bits == None) {
         sprintf(gszMsgBox, TgLoadString(STID_SHARELIB_TGEF_MSG_LOAD_FAIL),
               pszSharedLibPath);
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
         return SharedLibLoadFailed(pTgEFInfo, pszSharedLibPath, TRUE);
      }
      gaExportFilterBitmaps[gnMaxExportFilters] =
            XCreateBitmapFromData(mainDisplay, mainWindow,
            pTgEFInfo->detail.stProtocolInfo_1.bm_bits,
            pTgEFInfo->detail.stProtocolInfo_1.bm_width,
            pTgEFInfo->detail.stProtocolInfo_1.bm_height);
      if (gaExportFilterBitmaps[gnMaxExportFilters] == None) {
         sprintf(gszMsgBox, TgLoadString(STID_ALLOC_BMP_FAIL_FOR_SHARELIB),
               pszSharedLibPath);
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
         return SharedLibLoadFailed(pTgEFInfo, pszSharedLibPath, TRUE);
      }
      break;
   default:
      sprintf(gszMsgBox, TgLoadString(STID_UNSUP_PROTOCOL_VER_SHARELIB),
            nSharedLibVersion, pszSharedLibPath);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      CLOSE_DL(pTgEFInfo->pSharedLibHandle);
      return FALSE;
   }
   return TRUE;
}

int InitExportFilters()
{
   char *psz=NULL;

   gnMaxExportFilters = 0;
   if ((psz=XGetDefault(mainDisplay, TOOL_NAME, "MaxExportFilters")) !=
         NULL) {
      int nMax=atoi(psz);

      if (nMax <= 0) {
         sprintf(gszMsgBox, TgLoadString(STID_BAD_XDEF_MUST_BE_GT_0),
               TOOL_NAME, "MaxExportFilters", psz);
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      } else {
         int i=0;

         gpTgEFInfo = (TgEFInfo*)malloc(nMax*sizeof(TgEFInfo));
         if (gpTgEFInfo == NULL) FailAllocMessage();
         memset(gpTgEFInfo, 0, nMax*sizeof(TgEFInfo));
         gaExportFilterBitmaps = (Pixmap*)malloc(nMax*sizeof(Pixmap));
         if (gaExportFilterBitmaps == NULL) FailAllocMessage();
         memset(gaExportFilterBitmaps, 0, nMax*sizeof(Pixmap));

         for (i=0; i < nMax; i++) {
            char szBuf[80];
            int nOK=TRUE;

            sprintf(szBuf, "ExportFilter%1d", i);
            if ((psz=XGetDefault(mainDisplay, TOOL_NAME, szBuf)) == NULL) {
               nOK = FALSE;
               sprintf(gszMsgBox,
                     TgLoadString(STID_CANT_FIND_XDEF_CONT_LOOK_FLTR),
                     TOOL_NAME, szBuf);
               if (MsgBox(gszMsgBox, TOOL_NAME, YNC_MB) != MB_ID_YES) {
                  break;
               }
            } else {
               UtilTrimBlanks(psz);
               nOK = LoadSharedLib(psz);
            }
            if (nOK) {
               gnMaxExportFilters++;
            }
         }
         if (gnMaxExportFilters > 0) {
            whereToPrintPixmap = (Pixmap*)realloc(whereToPrintPixmap,
                  (MAXDEFWHERETOPRINT+gnMaxExportFilters)*sizeof(Pixmap));
            if (whereToPrintPixmap == NULL) FailAllocMessage();
            for (i=0; i < gnMaxExportFilters; i++) {
               whereToPrintPixmap[MAXDEFWHERETOPRINT+i] =
                     gaExportFilterBitmaps[i];
            }
            maxWhereToPrint += gnMaxExportFilters;
         }
      }
   }
   return TRUE;
}

/* do not translate -- program constants */
static char *gpszColorExportName[MAXDEFWHERETOPRINT] = {
   "Printer", "EPS", "PS", "XPM", "ASCII", "EPSI", "GIF/ISMAP", "HTML/USEMAP",
   "PDF", "TiffEPSI", "PNG", "JPEG", "PPM", "NetList"
};

static char *gpszBWExportName[MAXDEFWHERETOPRINT] = {
   "Printer", "EPS", "PS", "XBM", "ASCII", "EPSI", "GIF/ISMAP", "HTML/USEMAP",
   "PDF", "TiffEPSI", "PNG", "JPEG", "PBM", "NetList"
};

static char *gpszColorExportExt[MAXDEFWHERETOPRINT] = {
   NULL, EPSF_FILE_EXT, PS_FILE_EXT, XPM_FILE_EXT, TEXT_FILE_EXT,
   EPSF_FILE_EXT, "gif", "html", "pdf", EPSF_FILE_EXT, "png", "jpg", "ppm",
   NETLIST_FILE_EXT
};

static char *gpszBWExportExt[MAXDEFWHERETOPRINT] = {
   NULL, EPSF_FILE_EXT, PS_FILE_EXT, XBM_FILE_EXT, TEXT_FILE_EXT,
   EPSF_FILE_EXT, "gif", "html", "pdf", EPSF_FILE_EXT, "png", "jpg", "pbm",
   NETLIST_FILE_EXT
};

int GetExportIndex(nWhereToPrint, nMode)
   int nWhereToPrint, nMode;
{
   if (nMode == 0) {
   } else if (nMode > 0 ){
      nWhereToPrint = (nWhereToPrint+1) % maxWhereToPrint;
   } else if (nMode < 0 ){
      nWhereToPrint = (nWhereToPrint+maxWhereToPrint-1) % maxWhereToPrint;
   }
   return nWhereToPrint;
}

char *GetExportName(nWhereToPrint, nMode)
   int nWhereToPrint, nMode;
{
   TgEFInfo *pTgEFInfo=NULL;

   nWhereToPrint = GetExportIndex(nWhereToPrint, nMode);

   if (nWhereToPrint < MAXDEFWHERETOPRINT) {
      if (nWhereToPrint == NETLIST_FILE && preDumpSetup) {
         /* do not translate -- program constants */
         return "Component";
      } else if (colorDump) {
         return gpszColorExportName[nWhereToPrint];
      } else {
         return gpszBWExportName[nWhereToPrint];
      }
   }
   if (gpTgEFInfo == NULL) return TgLoadCachedString(CSTID_PARANED_UNKNOWN);

   pTgEFInfo = (&gpTgEFInfo[nWhereToPrint-MAXDEFWHERETOPRINT]);

   switch (pTgEFInfo->nSharedLibVersion) {
   case 1:
      if (colorDump) {
         return pTgEFInfo->detail.stProtocolInfo_1.color_format_name;
      } else {
         return pTgEFInfo->detail.stProtocolInfo_1.bw_format_name;
      }
   }
   return TgLoadCachedString(CSTID_PARANED_UNKNOWN);
}

char *GetExportExt(nWhereToPrint)
   int nWhereToPrint;
{
   TgEFInfo *pTgEFInfo=NULL;

   if (nWhereToPrint < MAXDEFWHERETOPRINT) {
      if (nWhereToPrint == NETLIST_FILE && preDumpSetup) {
         return COMP_FILE_EXT;
      } else if (colorDump) {
         return gpszColorExportExt[nWhereToPrint];
      } else {
         return gpszBWExportExt[nWhereToPrint];
      }
   }
   if (gpTgEFInfo == NULL) return TgLoadCachedString(CSTID_PARANED_UNKNOWN);

   pTgEFInfo = (&gpTgEFInfo[nWhereToPrint-MAXDEFWHERETOPRINT]);

   switch (pTgEFInfo->nSharedLibVersion) {
   case 1:
      if (colorDump) {
         return pTgEFInfo->detail.stProtocolInfo_1.color_ext;
      } else {
         return pTgEFInfo->detail.stProtocolInfo_1.bw_ext;
      }
   }
   return TgLoadCachedString(CSTID_PARANED_UNKNOWN);
}

int GetExportFilterCapability(nWhereToPrint)
   int nWhereToPrint;
{
   TgEFInfo *pTgEFInfo=(&gpTgEFInfo[nWhereToPrint-MAXDEFWHERETOPRINT]);

   switch (pTgEFInfo->nSharedLibVersion) {
   case 1:
      return pTgEFInfo->detail.stProtocolInfo_1.capability;
   }
   return 0;
}

static
int ExportVectorFile_1(pTgEFInfo, pDumpFile, nPageNumInFileName,
      nPrintingPageNum, nPrintingFirstPageNum, nPrintingPageRow,
      nPrintingPageCol)
   TgEFInfo *pTgEFInfo;
   FILE *pDumpFile;
   int nPageNumInFileName, nPrintingPageNum, nPrintingFirstPageNum;
   int nPrintingPageRow, nPrintingPageCol;
{
   long lReturn=0L;

   TgEFProtocolInfo_1 *pProtocolInfo_1=(&pTgEFInfo->detail.stProtocolInfo_1);

   pProtocolInfo_1->fp = pDumpFile;
   pProtocolInfo_1->color_dump = colorDump;
   pProtocolInfo_1->color_layers = colorLayers;
   pProtocolInfo_1->printing_page_num = nPrintingPageNum;
   pProtocolInfo_1->printing_first_page_num = nPrintingFirstPageNum;
   pProtocolInfo_1->printing_page_row = nPrintingPageRow;
   pProtocolInfo_1->printing_page_col = nPrintingPageCol;
   pProtocolInfo_1->page_layout_mode = pageLayoutMode;
   pProtocolInfo_1->last_page_num = lastPageNum;
   pProtocolInfo_1->top_obj = topObj;
   pProtocolInfo_1->bot_obj = botObj;
   pProtocolInfo_1->write_file_failed = FALSE;

   lReturn = TgEFSendMsg(pTgEFInfo, TRUE, TGEF_MSG_DO_EXPORT, 0,
         &pTgEFInfo->detail.stProtocolInfo_1);
   if (lReturn != TGEF_STATUS_OK) {
      sprintf(gszMsgBox, TgLoadString(STID_SHARELIB_TGEF_MSG_DO_EXPORT_F),
            pTgEFInfo->pszSharedLibPath, lReturn);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return FALSE;
   }
   if (pProtocolInfo_1->write_file_failed) writeFileFailed = TRUE;
   return TRUE;
}

int ExportVectorFile(pDumpFile, nPageNumInFileName, nPrintingPageNum,
      nPrintingFirstPageNum, nPrintingPageRow, nPrintingPageCol)
   FILE *pDumpFile;
   int nPageNumInFileName, nPrintingPageNum, nPrintingFirstPageNum;
   int nPrintingPageRow, nPrintingPageCol;
{
   TgEFInfo *pTgEFInfo=(&gpTgEFInfo[whereToPrint-MAXDEFWHERETOPRINT]);

   switch (pTgEFInfo->nSharedLibVersion) {
   case 1:
      return ExportVectorFile_1(pTgEFInfo, pDumpFile, nPageNumInFileName,
            nPrintingPageNum, nPrintingFirstPageNum, nPrintingPageRow,
            nPrintingPageCol);
   }
   return FALSE;
}

