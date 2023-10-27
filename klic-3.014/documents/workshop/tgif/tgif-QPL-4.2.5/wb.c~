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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/wb.c,v 1.45 2011/05/16 16:22:00 william Exp $
 */

#define _INCLUDE_FROM_WB_C_

#if (defined(PTHREAD) || defined(HAVE_LIBPTHREAD))
#include <pthread.h>
#endif /* (defined(PTHREAD) || defined(HAVE_LIBPTHREAD)) */

#include "tgifdefs.h"
#include "cmdids.h"

#ifdef _HAS_STREAMS_SUPPORT
#include <stropts.h>
#include <sys/types.h>
#endif /* _HAS_STREAMS_SUPPORT */
#include "rmcast/src/rmcast.h"

#include "auxtext.e"
#include "chat.e"
#include "choice.e"
#include "color.e"
#include "cmd.e"
#include "dialog.e"
#include "drawing.e"
#include "file.e"
#include "font.e"
#include "grid.e"
#include "http.e"
#include "import.e"
#include "mark.e"
#include "menu.e"
#include "msg.e"
#include "obj.e"
#include "page.e"
#include "raster.e"
#include "remote.e"
#include "rm_intrf.e"
#include "ruler.e"
#include "scroll.e"
#include "select.e"
#include "setup.e"
#include "stk.e"
#include "strtbl.e"
#include "tcp.e"
#include "util.e"
#include "version.e"
#include "wb.e"
#include "wb_buff.e"
#include "wb_seg.e"
#include "xpixmap.e"
#include "z_intrf.e"

struct WhiteBoardRec	gstWBInfo;

int	cmdLineNoWhiteBoard=FALSE;
int	cmdLineWhiteBoardListenOnly=FALSE;

static int	recordCmdLogicalClock=0;
static char	recordCmdSenderProcID[MAXSTRING];

#ifdef _TGIF_DBG_WB2
static int		wb2DebugLevel=0;
#endif /* _TGIF_DBG_WB2 */

/* =================== Remote Related Functions =================== */

static
void CleanUpWBSockets()
{
   if (gstWBInfo.listening) {
      if (gstWBInfo.listen_socket != (-1)) {
         close(gstWBInfo.listen_socket);
         gstWBInfo.listen_socket = (-1);
      }
      if (gstWBInfo.listen_fp != NULL) {
         fclose(gstWBInfo.listen_fp);
         gstWBInfo.listen_fp = NULL;
      }
      if (gstWBInfo.gz_listen_fp != NULL) {
         fclose(gstWBInfo.gz_listen_fp);
         gstWBInfo.gz_listen_fp = NULL;
      }
      if (*gstWBInfo.gz_listen_fname != '\0') {
         unlink(gstWBInfo.gz_listen_fname);
         *gstWBInfo.gz_listen_fname = '\0';
      }
      if (*gstWBInfo.listen_fname != '\0') {
         unlink(gstWBInfo.listen_fname);
         *gstWBInfo.listen_fname = '\0';
      }
      gstWBInfo.listening = FALSE;
   }
   if (gstWBInfo.send_socket != (-1)) {
      close(gstWBInfo.send_socket);
      gstWBInfo.send_socket = (-1);
   }
   if (gstWBInfo.gz_send_fp != NULL) {
      fclose(gstWBInfo.gz_send_fp);
      gstWBInfo.gz_send_fp = NULL;
   }
   if (gstWBInfo.send_fp != NULL) {
      fclose(gstWBInfo.send_fp);
      gstWBInfo.send_fp = NULL;
   }
   if (*gstWBInfo.gz_send_fname != '\0') {
      unlink(gstWBInfo.gz_send_fname);
      *gstWBInfo.gz_send_fname = '\0';
   }
   if (*gstWBInfo.send_fname != '\0') {
      unlink(gstWBInfo.send_fname);
      *gstWBInfo.send_fname = '\0';
   }
}

static
void CleanUpWBShadowCmds()
{
   struct CmdRec *cmd_ptr=NULL;
   int num_records=0;

   for (cmd_ptr=gstWBInfo.last_shadow_cmd; cmd_ptr != NULL;
         cmd_ptr=cmd_ptr->prev) {
      num_records++;
   }
   if (num_records > 0) {
      struct CmdRec *prev_cmd=NULL;
      double inc=(100.0/((double)num_records)), percent_start=0.0;

      ShowInterrupt(1);
      SaveStatusStrings();
      for (cmd_ptr=gstWBInfo.last_shadow_cmd; cmd_ptr != NULL; cmd_ptr=prev_cmd,
            percent_start+=inc) {
         prev_cmd = cmd_ptr->prev;
         DeleteARedoRecord(cmd_ptr, percent_start,
               min(((double)percent_start+inc),((double)100.0)));
      }
      RestoreStatusStrings();
      HideInterrupt();
   }
   gstWBInfo.first_shadow_cmd = gstWBInfo.last_shadow_cmd = NULL;
}

static
void CleanUpWBCmds()
{
   struct CmdRec *cmd_ptr=NULL;
   int num_records=0;

   for (cmd_ptr=gstWBInfo.last_cmd; cmd_ptr != NULL; cmd_ptr=cmd_ptr->prev) {
      num_records++;
   }
   if (num_records > 0) {
      struct CmdRec *prev_cmd=NULL;
      double inc=(100.0/((double)num_records)), percent_start=0.0;

      ShowInterrupt(1);
      SaveStatusStrings();
      for (cmd_ptr=gstWBInfo.last_cmd; cmd_ptr != NULL; cmd_ptr=prev_cmd,
            percent_start+=inc) {
         prev_cmd = cmd_ptr->prev;
         DeleteARedoRecord(cmd_ptr, percent_start,
               min(((double)percent_start+inc),((double)100.0)));
      }
      RestoreStatusStrings();
      HideInterrupt();
   }
   gstWBInfo.first_cmd = gstWBInfo.last_cmd = gstWBInfo.cur_cmd = NULL;

   CleanUpWBShadowCmds();
}

static
int CreateWBListenSocket()
{
   gstWBInfo.listening = TRUE;

   if (MkTempFile(gstWBInfo.listen_fname, sizeof(gstWBInfo.listen_fname),
         tmpDir, TOOL_NAME) == NULL) {
      fprintf(stderr, "Fail to create '%s' for deserialization.\n",
            gstWBInfo.listen_fname);
      return FALSE;
   }
   snprintf(gstWBInfo.gz_listen_fname, sizeof(gstWBInfo.gz_listen_fname),
         "%s.z", gstWBInfo.listen_fname);
   if ((gstWBInfo.listen_fp=fopen(gstWBInfo.listen_fname, "w+")) == NULL) {
      fprintf(stderr, "Fail to create '%s' for deserialization.\n",
            gstWBInfo.listen_fname);
      return FALSE;
   }
   if ((gstWBInfo.gz_listen_fp=fopen(gstWBInfo.gz_listen_fname, "w+")) ==
         NULL) {
      fprintf(stderr, "Fail to create '%s' for deserialization.\n",
            gstWBInfo.gz_listen_fname);
      return FALSE;
   }

   return TRUE;
}

void PrintFullIDsOfObjects(psz_prefix)
   char *psz_prefix;
{
#ifdef _TGIF_DBG_WB2
   if (wb2DebugLevel > 0) {
      struct ObjRec *obj_ptr=NULL;
      int stacking_order=0;

      if (psz_prefix != NULL) {
         if (strcmp(psz_prefix, "\t") == 0) {
            fprintf(stderr, "\tin PrintFullIDsOfObjects():\n");
         } else {
            fprintf(stderr, "%s, in PrintFullIDsOfObjects():\n", psz_prefix);
         }
      } else {
         fprintf(stderr, "In PrintFullIDsOfObjects():\n");
      }
      for (obj_ptr=topObj; obj_ptr != NULL; obj_ptr=obj_ptr->next,
            stacking_order++) {
         char buf[MAXSTRING], obj_type[MAXSTRING];

         if (obj_ptr->creator_full_id == NULL) {
            sprintf(buf, "(NULL)%1d/%s", obj_ptr->id, gszLocalPID);
         } else {
            strcpy(buf, obj_ptr->creator_full_id);
         }
         switch (obj_ptr->type) {
         case OBJ_POLY: strcpy(obj_type, "p"); break;
         case OBJ_BOX: strcpy(obj_type, "b"); break;
         case OBJ_OVAL: strcpy(obj_type, "o"); break;
         case OBJ_TEXT:
            strcpy(obj_type, "t");
            {
               MiniLinesInfo *minilines=(&obj_ptr->detail.t->minilines);
               char *psz=minilines->first->first_block->seg->dyn_str.s;
               char buf1[MAXSTRING];
               int len=strlen(psz);

               if (len > 5) {
                  char saved_ch=psz[5];

                  sprintf(buf1, " - \"%s...\"", psz);
                  psz[5] = saved_ch;
               } else {
                  sprintf(buf1, " - \"%s\"", psz);
               }
               strcat(buf, buf1);
            }
            break;
         case OBJ_POLYGON: strcpy(obj_type, "g"); break;
         case OBJ_ARC: strcpy(obj_type, "a"); break;
         case OBJ_RCBOX: strcpy(obj_type, "rcb"); break;
         case OBJ_XBM: strcpy(obj_type, "xbm"); break;
         case OBJ_XPM: strcpy(obj_type, "xpm"); break;
         case OBJ_GROUP:
         case OBJ_ICON:
         case OBJ_SYM:
         case OBJ_PIN: strcpy(obj_type, "r"); break;
         }
         fprintf(stderr, "\t%d/%s:\t%s\n", stacking_order, obj_type, buf);
      }
   }
#endif /* _TGIF_DBG_WB2 */
}


int RecvWBData(flag, ppsz_buf, pn_buf_sz)
   int flag, *pn_buf_sz;
   char **ppsz_buf;
{
   
   int retval;
   
   retval = buff_rem(gstWBInfo.bd_commands, (void**)ppsz_buf);
   
   return retval;
}

static
void ProcessWBInputData(buf)
   char *buf;
   /*
    * Here the WB input data is in the HTTP data format.
    * So we use HttpExtractText() to convert the data into
    *       something that can be handled by ProcessRemoteCmd().
    * In the future, if the WB input data is binary, this is
    *       where the conversion takes place.
    */
{
   char *psz_content=NULL, *psz_content_type=NULL;
   int content_sz=0;

   psz_content = HttpExtractText(buf, &content_sz, NULL, &psz_content_type);
   if (psz_content != NULL) {
      ProcessRemoteCmd(psz_content_type, psz_content, content_sz);
      FreeRemoteBuf(psz_content);
   }
   if (psz_content_type != NULL) UtilFree(psz_content_type);
}

int WBHasReadData(pn_flag, pn_retry)
   int *pn_flag, *pn_retry;
{
   /* leave *pn_retry alone */
   
   if ((!(gstWBInfo.BlockRemoteCmdDepth <= 1)) && (buff_items(gstWBInfo.bd_commands) > 0) ) 
   {
      /* FIXME fprintf(stderr,"gstWBInfo.BlockRemoteCmdDepth > 1 e existem dados no buffer. (WBHasReadData)\n");
      exit(1); */
   }
   
   return (gstWBInfo.BlockRemoteCmdDepth <= 1 &&
         buff_items(gstWBInfo.bd_commands) > 0);
}



void TryHandleWBInputData()
{
   int flag=0, retry=TRUE;
   char *buf;
   int buf_sz;

     while (retry && WBHasReadData(&flag, &retry)) { 
      
      buf=NULL;
      buf_sz=0;

      deserializingFile = TRUE;

      if (RecvWBData(flag, &buf, &buf_sz)) {
         ProcessWBInputData(buf);
         FreeRemoteBuf(buf);
      }
      deserializingFile = FALSE;
     } 
}

int SendWBData(pszWBData, logical_clock)
   char *pszWBData;
   int   logical_clock;
{
   int    nPackets1, i;
   struct SegmentationPack *pack1;
   
#ifdef DEBUG   
   fprintf(stderr,"Mensagem enviada:\n%s\n", pszWBData);
#endif   

   pack1 = Segment( pszWBData, strlen(pszWBData), gszLocalPID, logical_clock, &nPackets1 );

   for( i = 0; i < nPackets1; i++ )
   {
      RM_sendto(gstWBInfo.MCastSock, (char*)&pack1[i], sizeof(struct SegmentationPack));
   }
   free( pack1 );

#ifdef _TGIF_DBG_WB2
   fprintf(stderr, "SendWBData(), pid = %ld\n", gstWBInfo.pid);
#endif /* _TGIF_DBG_WB2 */

   return TRUE;
}

/* =================== Cmd Related Functions =================== */

static
int SaveCmd(FP, CmdType, psi)
   FILE *FP;
   int CmdType;
   SerializationInfo *psi;
{
   struct SelRec *sel_ptr=NULL;
   int count=0;

   /*
    * This part should work now!
    *
    * if (recordCmdIncludeTgifObj) {
    *    sprintf(gszMsgBox, "%s.\n%s %s.\n\n%s %s.",
    *          "Warning: recordCmdIncludeTgifObj is TRUE in SaveCmd()",
    *          TOOL_NAME, "WhiteBoard may crash",
    *          "Please try to reproduce this error and send bug report to",
    *          authorEmailString);
    *    MsgBox(gszMsgBox, TOOL_NAME, STOP_MB);
    *    return FALSE;
    * } else
    */
   if (CmdType == CMD_GOTO_PAGE) {
      /*
       * GotoPage() is disabled for _TGIF_WB2.
       */
      return FALSE;
   }
   if (fprintf(FP, "cmd(%1d,", CmdType) == EOF) {
      writeFileFailed = TRUE;
   }
   if (CmdType == CMD_MOVE) {
      if (fprintf(FP, "%1d,%1d,",
            psi->subcmd->detail.move.dx, psi->subcmd->detail.move.dy) == EOF) {
         writeFileFailed = TRUE;
      }
   } else if (CmdType == CMD_WB_CLEARALL) {
      if (fprintf(FP, "%1d,%.3f,",
            psi->subcmd->detail.clearall.page_style,
            psi->subcmd->detail.clearall.print_mag) == EOF) {
         writeFileFailed = TRUE;
      }
   } else if (CmdType == CMD_WB_SLIDESHOW) {
      if (fprintf(FP, "%1d,",
            psi->subcmd->detail.slideshow.into_slideshow) ==
            EOF) {
         writeFileFailed = TRUE;
      }
   } else if (CmdType == CMD_CHAT_A_LINE) {
      if (fprintf(FP, "%1d,'", psi->subcmd->detail.chat.type) == EOF) {
         writeFileFailed = TRUE;
      }
      SaveString(FP, psi->subcmd->detail.chat.nick_name);
      if (fprintf(FP, "','%s',%1d,'%s',",
            psi->subcmd->detail.chat.tfi.color_str,
            psi->subcmd->detail.chat.tfi.font_style,
            psi->subcmd->detail.chat.encoding) == EOF) {
         writeFileFailed = TRUE;
      }
      if (fprintf(FP, "\"") == EOF) writeFileFailed = TRUE;
      SaveString(FP, psi->subcmd->detail.chat.buf);
      if (fprintf(FP, "\",") == EOF) writeFileFailed = TRUE;
   }
   if (fprintf(FP, "%1d,%1d,%1d,\"%s\").\n",
         psi->include_tgif_obj, psi->new_colormap, psi->logical_clock,
         psi->sender_process_id) == EOF) {
      writeFileFailed = TRUE;
   }
   /* Lamport's Algorithm increments logical_clock */
   gstWBInfo.logical_clock++;

   if (psi->bot_before != NULL) {
      if (fprintf(FP, "before_image(%1d,[\n", psi->count_before) == EOF) {
         writeFileFailed = TRUE;
      }
      for (count=0, sel_ptr=psi->top_before; sel_ptr != NULL;
            sel_ptr=sel_ptr->next, count++) {
         struct ObjRec *obj_ptr=sel_ptr->obj;
         char buf[MAXSTRING];

         if (psi->pos_before_has_ids) {
            sprintf(buf, "%s", ((char**)psi->pos_before)[count]);
         } else {
            if (obj_ptr->creator_full_id == NULL) {
               sprintf(buf, "%1d/%s", obj_ptr->id, psi->sender_process_id);
            } else {
               strcpy(buf, obj_ptr->creator_full_id);
            }
         }
         if (fprintf(FP, "\t\"") == EOF) writeFileFailed = TRUE;
         SaveString(FP, buf);
         if (sel_ptr->next == NULL) {
            if (fprintf(FP, "\"\n") == EOF) writeFileFailed = TRUE;
         } else {
            if (fprintf(FP, "\",\n") == EOF) writeFileFailed = TRUE;
         }
      }
      if (fprintf(FP, "]).\n") == EOF) writeFileFailed = TRUE;
   }
   if (psi->bot_sel != NULL) {
      struct SelRec *sel_ptr;
      int *stacking_pos=NULL, stacking_count=0;

      sel_ptr = PrepareStackingInfo(psi->top_sel,
            psi->bot_sel, psi->num_objs, FALSE,
            &stacking_pos, &stacking_count, NULL);
      if (sel_ptr == NULL && stacking_count == psi->num_objs) {
         if (fprintf(FP, "after_positions(%1d,[\n",
               psi->num_objs) == EOF) {
            writeFileFailed = TRUE;
         }
         for (count=0, sel_ptr=psi->top_sel;
               count < stacking_count && sel_ptr != NULL;
               count++, sel_ptr=sel_ptr->next) {
            char buf[MAXSTRING];

            sprintf(buf, "%1d", stacking_pos[count]);
            if (fprintf(FP, "\t") == EOF) writeFileFailed = TRUE;
            SaveString(FP, buf);
            if (sel_ptr->next == NULL) {
               if (fprintf(FP, "\n") == EOF) writeFileFailed = TRUE;
            } else {
               if (fprintf(FP, ",\n") == EOF) writeFileFailed = TRUE;
            }
         }
         if (fprintf(FP, "]).\n") == EOF) writeFileFailed = TRUE;

         if (fprintf(FP, "after_image(%1d,[\n", psi->num_objs) ==
               EOF) {
            writeFileFailed = TRUE;
         }
         for (count=0, sel_ptr=psi->top_sel; sel_ptr != NULL;
               sel_ptr=sel_ptr->next, count++) {
            SaveObj(FP, sel_ptr->obj, 1);
            if (sel_ptr->next == NULL) {
               if (fprintf(FP, "\n") == EOF) writeFileFailed = TRUE;
            } else {
               if (fprintf(FP, ",\n") == EOF) writeFileFailed = TRUE;
            }
         }
         if (fprintf(FP, "]).\n") == EOF) writeFileFailed = TRUE;
      }
      if (stacking_pos != NULL) free(stacking_pos);
   }
   return TRUE;
}

static
void PrepareExtendedSerializationInfo(CmdPtr, psi)
   struct CmdRec *CmdPtr;
   SerializationInfo *psi;
{
   psi->subcmd = CmdPtr->subcmd;
   psi->top_sel = NULL; /* not used */
   psi->bot_sel = NULL; /* not used */
   psi->num_objs = 0; /* not used */
   
   psi->include_tgif_obj = CmdPtr->include_tgif_obj;
   psi->new_colormap = CmdPtr->new_colormap;
   psi->logical_clock = CmdPtr->logical_clock;
   psi->sender_process_id = CmdPtr->sender_process_id;

   psi->top_before = CmdPtr->top_before;
   psi->bot_before = CmdPtr->bot_before;
   psi->pos_before = CmdPtr->pos_before;
   psi->count_before = CmdPtr->count_before;
   psi->pos_before_has_ids = CmdPtr->pos_before_has_ids;
}

static
int SaveExtendedCmd(FP, CmdPtr, psi)
   FILE *FP;
   struct CmdRec *CmdPtr;
   SerializationInfo *psi;
{
   if (CmdPtr->type == CMD_GOTO_PAGE) {
      /*
       * GotoPage() is disabled for _TGIF_WB2.
       */
      return FALSE;
   } else if (CmdPtr->type == CMD_CHAT_A_LINE) {
      /*
       * intentionally forget what has been chatted
       */
      return FALSE;
   }
   if (CmdPtr->serialization_buf == NULL) {
      fprintf(stderr, "serialization_buf is NULL\n");
      return FALSE;
   }
#ifdef NOT_DEFINED
   /* this is replaced by simply printing CmdPtr->serialization_buf */
   if (fprintf(FP, "cmdx(%1d,%1d,", CmdPtr->type, CmdPtr->undone) == EOF) {
      writeFileFailed = TRUE;
   }
   if (CmdPtr->type == CMD_MOVE) {
      if (fprintf(FP, "%1d,%1d,",
            CmdPtr->subcmd->detail.move.dx,
            CmdPtr->subcmd->detail.move.dy) == EOF) {
         writeFileFailed = TRUE;
      }
   } else if (CmdPtr->type == CMD_WB_CLEARALL) {
      if (fprintf(FP, "%1d,%.3f,", 
            CmdPtr->subcmd->detail.clearall.page_style,
            CmdPtr->subcmd->detail.clearall.print_mag) == EOF) {
         writeFileFailed = TRUE;
      }
   } else if (CmdPtr->type == CMD_WB_SLIDESHOW) {
      if (fprintf(FP, "%1d,",
            CmdPtr->subcmd->detail.slideshow.into_slideshow) == EOF) {
         writeFileFailed = TRUE;
      }
   } else if (CmdPtr->type == CMD_CHAT_A_LINE) {
      /* intentionally forget what has been chatted */
      return FALSE;
   }
   if (fprintf(FP, "%1d,%1d,%1d,\"%s\",%1d).\n",
         CmdPtr->include_tgif_obj, CmdPtr->new_colormap, CmdPtr->logical_clock,
         CmdPtr->sender_process_id, CmdPtr->undone) == EOF) {
      writeFileFailed = TRUE;
   }
#endif /* NOT_DEFINED */
   if (fprintf(FP, "%s", CmdPtr->serialization_buf) == EOF) {
      writeFileFailed = TRUE;
   }
   return TRUE;
}

static
int SaveExtendedCmdInfo(FP, nNumCmds, nCurCmdPos)
   FILE *FP;
   int nNumCmds, nCurCmdPos;
{
   if (fprintf(FP, "cmdxinfo(%1d,%1d).\n", nNumCmds, nCurCmdPos) == EOF) {
      writeFileFailed = TRUE;
   }
   return TRUE;
}

#ifdef _TGIF_DBG_WB2
static
void DebugDumpCmd(psz_prefix, cmd_type, logical_clock, psz_process_id)
   char *psz_prefix, *psz_process_id;
   int cmd_type, logical_clock;
{
   if (psz_prefix == NULL) {
      fprintf(stderr, "cmd(%1d,%1d,\"%s\")\n", cmd_type, logical_clock,
            psz_process_id);
   } else {
      fprintf(stderr, "%s, cmd(%1d,%1d,\"%s\")\n", psz_prefix, cmd_type,
            logical_clock, psz_process_id);
   }
}
#endif /* _TGIF_DBG_WB2 */

char *SerializeCmd(CmdType, psi, pnAssignedLogicalClock, ppszPlainWBData,
      pnPlainWBDataSz)
   int CmdType, *pnAssignedLogicalClock, *pnPlainWBDataSz;
   SerializationInfo *psi;
   char **ppszPlainWBData;
{
   char *buf=NULL, header[MAXSTRING<<1];
   int content_sz=0, ok=TRUE, header_sz=0, do_deflate=TRUE;
   long loc_sec=0L, loc_msec=0L;

   if (HasZlibSupport()) {
#ifdef _TGIF_DBG_WB2
      do_deflate = FALSE;
#endif /* _TGIF_DBG_WB2 */
   } else {
      do_deflate = FALSE;
   }
   if (gstWBInfo.send_fp == NULL || gstWBInfo.gz_send_fp == NULL) {
      sprintf(gszMsgBox, "Cannot open '%s' or '%s'.\n\nSerialization aborted.",
            gstWBInfo.send_fname, gstWBInfo.gz_send_fname);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return NULL;
   }
   *pnAssignedLogicalClock = gstWBInfo.logical_clock;

   rewind(gstWBInfo.send_fp);
   writeFileFailed = FALSE;

   UtilGetMilliSecTime(&loc_sec, &loc_msec);
   if (fprintf(gstWBInfo.send_fp, "%%TGWB begin - %ld.%ld\n",
         loc_sec, loc_msec) == EOF) {
      writeFileFailed = TRUE;
   }
   Save(gstWBInfo.send_fp, NULL, 0, 1);
   ok = SaveCmd(gstWBInfo.send_fp, CmdType, psi);
   /* gstWBInfo.logical_clock has been incremented in SaveCmd() */

   if (fprintf(gstWBInfo.send_fp, "%%TGWB end - %ld.%ld\n",
         loc_sec, loc_msec) == EOF) {
      writeFileFailed = TRUE;
   }
   if (writeFileFailed) {
      FailToWriteFileMessage(gstWBInfo.send_fname);
      return NULL;
   }
   if (!ok) {
      return NULL;
   }
   content_sz = (int)ftell(gstWBInfo.send_fp);
   /*
    * From: <PID>:<IP>\r\n
    * Content-Type: application/x-tgif-cmd\r\n
    * Content-Length: <LENGTH>\r\n
    * \r\n
    * <DATA of size LENGTH>
    */
   if (do_deflate && HasZlibSupport()) {
      int rc=0;
      char *new_buf=NULL;

      rewind(gstWBInfo.send_fp);
      new_buf = (char*)malloc((content_sz+1)*sizeof(char));
      if (new_buf == NULL) FailAllocMessage();
      if (fread(new_buf, sizeof(char), content_sz, gstWBInfo.send_fp) !=
            content_sz) {
#ifdef _TGIF_DBG_WB2
         fprintf(stderr, "Failed to read %1d bytes in SerializeCmd().\n",
               content_sz);
         fflush(stderr);
#endif /* _TGIF_DBG_WB2 */
         do_deflate = FALSE;
      } else {
         new_buf[content_sz] = '\0';
         *pnPlainWBDataSz = content_sz+1;
         *ppszPlainWBData = new_buf;

         if (!DoDeflate(gstWBInfo.send_fp, NULL, content_sz,
               gstWBInfo.gz_send_fp, TRUE, TRUE, &rc)) {
            ZlibError(rc, TRUE);
            do_deflate = FALSE;
         } else {
            content_sz = (int)ftell(gstWBInfo.gz_send_fp);
         }
      }
   }
   if (do_deflate) {
      sprintf(header, "%s%s\r\n%s%s\r\n%s%1d\r\n\r\n",
            "From: ", gszLocalPID,
            "Content-Type: ", "application/x-tgif-cmd-z",
            "Content-Length: ", content_sz);
      header_sz = strlen(header);
      buf = (char*)malloc((header_sz+content_sz+1)*sizeof(char));
      if (buf == NULL) FailAllocMessage();
      strcpy(buf, header);
      rewind(gstWBInfo.gz_send_fp);
      if (fread(&buf[header_sz], sizeof(char), content_sz,
            gstWBInfo.gz_send_fp) != content_sz) {
         sprintf(gszMsgBox, "Error in reading '%s'.\n\nSerialization aborted.",
               gstWBInfo.gz_send_fname);
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
   
         UtilFree(buf);
         return NULL;
      }
   } else {
      sprintf(header, "%s%s\r\n%s%s\r\n%s%1d\r\n\r\n",
            "From: ", gszLocalPID,
            "Content-Type: ", "application/x-tgif-cmd",
            "Content-Length: ", content_sz);
      header_sz = strlen(header);
      buf = (char*)malloc((header_sz+content_sz+1)*sizeof(char));
      if (buf == NULL) FailAllocMessage();
      strcpy(buf, header);
      rewind(gstWBInfo.send_fp);
      if (fread(&buf[header_sz], sizeof(char), content_sz, gstWBInfo.send_fp) !=
            content_sz) {
         sprintf(gszMsgBox, "Error in reading '%s'.\n\nSerialization aborted.",
               gstWBInfo.send_fname);
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
   
         UtilFree(buf);
         return NULL;
      }
   }
   buf[header_sz+content_sz] = '\0';
#ifdef _TGIF_DBG_WB2
   if (wb2DebugLevel == 0) {
      fprintf(stderr, "Sending %1d bytes, pid = %ld!\n", header_sz+content_sz+2,
            gstWBInfo.pid);
   } else {
      fprintf(stderr, "===========================\n");
      if (do_deflate) {
         fprintf(stderr, "%s", header);
         fprintf(stderr, "[content not displayed] ...\n");
      } else {
         fprintf(stderr, "%s", buf);
      }
      fprintf(stderr, "===========================\n");
      fprintf(stderr, "Sending %1d bytes, pid = %ld!\n", header_sz+content_sz+2,
            gstWBInfo.pid);
   }
   DebugDumpCmd(">>> In SerializeCmd()",
         CmdType, *pnAssignedLogicalClock, gszLocalPID);
   PrintFullIDsOfObjects("In SerializeCmd()");
#endif /* _TGIF_DBG_WB2 */

   return buf;
}

void SerializeFreeBuf(pszWBData)
   char *pszWBData;
{
   UtilFree(pszWBData);
}

static struct CmdRec gstDeserializeCmd;
static struct SubCmdRec *gpDeserializeSubCmd=NULL;

static char **gppszBeforeImage=NULL;
static int gnBeforeImageCount=0;

static int *gpnAfterPositions=NULL;
static int gnAfterPositionsCount=0;

static int seenCmdXInfo=FALSE;

static
void CleanUpDeserializeCmd()
{
   if (gpDeserializeSubCmd != NULL) free(gpDeserializeSubCmd);
   memset(&gstDeserializeCmd, 0, sizeof(struct CmdRec));
   gpDeserializeSubCmd = NULL;
}

static
int InitDeserializeCmd()
{
   gpDeserializeSubCmd = (struct SubCmdRec *)malloc(sizeof(struct SubCmdRec));
   if (gpDeserializeSubCmd == NULL) FailAllocMessage();
   memset(gpDeserializeSubCmd, 0, sizeof(struct SubCmdRec));
   memset(&gstDeserializeCmd, 0, sizeof(struct CmdRec));
   gstDeserializeCmd.subcmd = gpDeserializeSubCmd;

   return TRUE;
}

static
void DoDeserializationCleanUp()
{
   switch (gstDeserializeCmd.type) {
   case CMD_WB_CLEARALL: break;
   case CMD_CHAT_A_LINE:
      UtilFree(gpDeserializeSubCmd->detail.chat.buf);
      break;
   case CMD_WB_SLIDESHOW: break;
   default: break;
   }
   memset(gpDeserializeSubCmd, 0, sizeof(struct SubCmdRec));
   memset(&gstDeserializeCmd, 0, sizeof(gstDeserializeCmd));
   gstDeserializeCmd.subcmd = gpDeserializeSubCmd;
   gstDeserializeCmd.type = INVALID;
}

static
void CleanUpBeforeImage()
{
   if (gppszBeforeImage != NULL) {
      int i;

      for (i=0; i < gnBeforeImageCount; i++) {
         if (gppszBeforeImage[i] != NULL) {
            UtilFree(gppszBeforeImage[i]);
         } else {
            break;
         }
      }
      free(gppszBeforeImage);
   }
   gppszBeforeImage = NULL;
   gnBeforeImageCount = 0;
}

static
void CleanUpAfterPositions()
{
   if (gpnAfterPositions != NULL) free(gpnAfterPositions);
   gpnAfterPositions = NULL;
   gnAfterPositionsCount = 0;
}

static
void DeserializationCleanUp()
{
   DoDeserializationCleanUp();
   CleanUpBeforeImage();
   CleanUpAfterPositions();
}

static
void ResetDeserializeCmd()
{
   recordCmdIncludeTgifObj = FALSE;
   recordCmdUsesNewColormap = FALSE;

   memset(&gstDeserializeCmd, 0, sizeof(gstDeserializeCmd));
   gstDeserializeCmd.subcmd = gpDeserializeSubCmd;
   gstDeserializeCmd.type = INVALID;
   gppszBeforeImage = NULL;
   gpnAfterPositions = NULL;
   gnBeforeImageCount = gnAfterPositionsCount = 0;
}

int ReadExtendedCmd(FP, Inbuf)
   FILE *FP;
   char *Inbuf;
{
   char *c_ptr=NULL;

   c_ptr = FindChar((int)'(', Inbuf);
   InitScan(c_ptr, "\t\n, ()");
   if (GETINT("cmdx", gstDeserializeCmd.type, "cmd_type") == INVALID ||
       GETINT("cmdx", gstDeserializeCmd.undone, "undone") == INVALID) {
      return FALSE;
   }
   if (gstDeserializeCmd.type == INVALID) {
      return FALSE;
   } else if (gstDeserializeCmd.type == CMD_GOTO_PAGE) {
      return FALSE;
   } else if (gstDeserializeCmd.type == CMD_WB_CLEARALL) {
      int page_style=PORTRAIT;
      float print_mag=(float)100.0;

      if (GETINT("cmdx", page_style, "page_style") == INVALID ||
          GETFLT("cmdx", print_mag, "print_mag") == INVALID) {
         return FALSE;
      }
      gpDeserializeSubCmd->detail.clearall.page_style = page_style;
      gpDeserializeSubCmd->detail.clearall.print_mag = print_mag;
   } else if (gstDeserializeCmd.type == CMD_WB_SLIDESHOW) {
      int into_slideshow=FALSE;

      if (GETINT("cmdx", into_slideshow, "into_slideshow") == INVALID) {
         return FALSE;
      }
      gpDeserializeSubCmd->detail.slideshow.into_slideshow = into_slideshow;
   } else if (gstDeserializeCmd.type == CMD_CHAT_A_LINE) {
      char nick_name[MAXSTRING], color_str[40], font_str[MAXSTRING];
      char encoding[MAXSTRING];
      int chat_type=FALSE, font_style=0, new_alloc=0;
      struct DynStrRec dyn_str;

      memset(&dyn_str, 0, sizeof(struct DynStrRec));
      *nick_name = *color_str = *font_str = *encoding = '\0';

      if (GETINT("cmdx", chat_type,      "type") == INVALID ||
          GETSTR("cmdx", nick_name,      "nick_name") == INVALID ||
          GETSTR("cmdx", color_str,      "color_str") == INVALID ||
          GETINT("cmdx", font_style,     "font_style") == INVALID ||
          GETSTR("cmdx", encoding,       "encoding") == INVALID ||
          GETDYN("cmdx", dyn_str, "buf") == INVALID) {
         return FALSE;
      }
      UtilRemoveQuotes(nick_name);
      UtilRemoveQuotes(color_str);
      UtilRemoveQuotes(encoding);

      gpDeserializeSubCmd->detail.chat.tfi.color_index =
            QuickFindColorIndex(NULL, color_str, &new_alloc, TRUE);
      UtilStrCpyN(gpDeserializeSubCmd->detail.chat.tfi.color_str,
            sizeof(gpDeserializeSubCmd->detail.chat.tfi.color_str),
            colorMenuItems[gpDeserializeSubCmd->detail.chat.tfi.color_index]);

      gpDeserializeSubCmd->detail.chat.tfi.font_style = font_style;
      gpDeserializeSubCmd->detail.chat.type = chat_type;
      UtilStrCpyN(gpDeserializeSubCmd->detail.chat.nick_name,
            sizeof(gpDeserializeSubCmd->detail.chat.nick_name), nick_name);
      UtilStrCpyN(gpDeserializeSubCmd->detail.chat.encoding,
            sizeof(gpDeserializeSubCmd->detail.chat.encoding), encoding);
      gpDeserializeSubCmd->detail.chat.buf = dyn_str.s;
      dyn_str.s = NULL;
   } else if (gstDeserializeCmd.type == CMD_MOVE) {
      int dx=0, dy=0;

      if (GETINT("cmdx", dx, "dx") == INVALID ||
          GETINT("cmdx", dy, "dy") == INVALID) {
         return FALSE;
      }
      gpDeserializeSubCmd->detail.move.dx = dx;
      gpDeserializeSubCmd->detail.move.dy = dy;
   }
   *recordCmdSenderProcID = '\0';
   if (GETINT("cmdx", recordCmdIncludeTgifObj, "include_tgif_obj")==INVALID ||
       GETINT("cmdx", recordCmdUsesNewColormap, "use_new_colormap")==INVALID ||
       GETINT("cmdx", recordCmdLogicalClock, "logical_clock")==INVALID ||
       GETSTR("cmdx", recordCmdSenderProcID, "sender_process_id")==INVALID ||
       GETINT("cmdx", gstDeserializeCmd.undone, "undone")==INVALID) {
      return FALSE;
   }
   UtilRemoveQuotes(recordCmdSenderProcID);

   return TRUE;
}

int ReadExtendedCmdInfo(FP, Inbuf)
   FILE *FP;
   char *Inbuf;
{
   char *c_ptr=NULL;
   int num_cmds=0, cur_cmd_pos=0;

   c_ptr = FindChar((int)'(', Inbuf);
   InitScan(c_ptr, "\t\n, ()");
   if (GETINT("cmdxinfo", num_cmds, "num_cmds") == INVALID ||
       GETINT("cmdxinfo", cur_cmd_pos, "cur_cmd_pos") == INVALID) {
      return FALSE;
   }
   seenCmdXInfo = TRUE;

   return TRUE;
}

int ReadCmd(FP, Inbuf)
   FILE *FP;
   char *Inbuf;
{
   char *c_ptr=NULL;

   ResetDeserializeCmd();

   c_ptr = FindChar((int)'(', Inbuf);
   InitScan(c_ptr, "\t\n, ()");
   if (GETINT("cmd", gstDeserializeCmd.type, "cmd_type") == INVALID) {
      return FALSE;
   }
   if (gstDeserializeCmd.type == INVALID) {
      return FALSE;
   } else if (gstDeserializeCmd.type == CMD_GOTO_PAGE) {
      return FALSE;
   } else if (gstDeserializeCmd.type == CMD_WB_CLEARALL) {
      int page_style=PORTRAIT;
      float print_mag=(float)100.0;

      if (GETINT("cmd", page_style, "page_style") == INVALID ||
          GETFLT("cmd", print_mag, "print_mag") == INVALID) {
         return FALSE;
      }
      gpDeserializeSubCmd->detail.clearall.page_style = page_style;
      gpDeserializeSubCmd->detail.clearall.print_mag = print_mag;
   } else if (gstDeserializeCmd.type == CMD_WB_SLIDESHOW) {
      int into_slideshow=FALSE;

      if (GETINT("cmd", into_slideshow, "into_slideshow") == INVALID) {
         return FALSE;
      }
      gpDeserializeSubCmd->detail.slideshow.into_slideshow = into_slideshow;
   } else if (gstDeserializeCmd.type == CMD_CHAT_A_LINE) {
      char nick_name[MAXSTRING], color_str[40], font_str[MAXSTRING];
      char encoding[MAXSTRING];
      int chat_type=FALSE, font_style=0, new_alloc=0;
      struct DynStrRec dyn_str;

      memset(&dyn_str, 0, sizeof(struct DynStrRec));
      *nick_name = *color_str = *font_str = *encoding = '\0';

      if (GETINT("cmd", chat_type,      "type") == INVALID ||
          GETSTR("cmd", nick_name,      "nick_name") == INVALID ||
          GETSTR("cmd", color_str,      "color_str") == INVALID ||
          GETINT("cmd", font_style,     "font_style") == INVALID ||
          GETSTR("cmd", encoding,       "encoding") == INVALID ||
          GETDYN("cmd", dyn_str, "buf") == INVALID) {
         return FALSE;
      }
      UtilRemoveQuotes(nick_name);
      UtilRemoveQuotes(color_str);
      UtilRemoveQuotes(encoding);

      gpDeserializeSubCmd->detail.chat.tfi.color_index =
            QuickFindColorIndex(NULL, color_str, &new_alloc, TRUE);
      UtilStrCpyN(gpDeserializeSubCmd->detail.chat.tfi.color_str,
            sizeof(gpDeserializeSubCmd->detail.chat.tfi.color_str),
            colorMenuItems[gpDeserializeSubCmd->detail.chat.tfi.color_index]);

      gpDeserializeSubCmd->detail.chat.tfi.font_style = font_style;
      gpDeserializeSubCmd->detail.chat.type = chat_type;
      UtilStrCpyN(gpDeserializeSubCmd->detail.chat.nick_name,
            sizeof(gpDeserializeSubCmd->detail.chat.nick_name), nick_name);
      UtilStrCpyN(gpDeserializeSubCmd->detail.chat.encoding,
            sizeof(gpDeserializeSubCmd->detail.chat.encoding), encoding);
      gpDeserializeSubCmd->detail.chat.buf = dyn_str.s;
      dyn_str.s = NULL;
   } else if (gstDeserializeCmd.type == CMD_MOVE) {
      int dx=0, dy=0;

      if (GETINT("cmd", dx, "dx") == INVALID ||
          GETINT("cmd", dy, "dy") == INVALID) {
         return FALSE;
      }
      gpDeserializeSubCmd->detail.move.dx = dx;
      gpDeserializeSubCmd->detail.move.dy = dy;
   }
   *recordCmdSenderProcID = '\0';
   if (GETINT("cmd", recordCmdIncludeTgifObj, "include_tgif_obj") == INVALID ||
       GETINT("cmd", recordCmdUsesNewColormap, "use_new_colormap") == INVALID ||
       GETINT("cmd", recordCmdLogicalClock, "logical_clock") == INVALID ||
       GETSTR("cmd", recordCmdSenderProcID, "sender_process_id") == INVALID) {
      return FALSE;
   }
   UtilRemoveQuotes(recordCmdSenderProcID);
   return TRUE;
}

int ReadBeforeImage(FP, Inbuf)
   FILE *FP;
   char *Inbuf;
{
   char *c_ptr, *line;
   int index=0, ok=TRUE;

   c_ptr = FindChar((int)'(', Inbuf);
   InitScan(c_ptr, "\t\n, ");
   if (GETINT("before_image", gnBeforeImageCount, "before_image_count") ==
         INVALID) {
      return FALSE;
   }
   if (gnBeforeImageCount > 0) {
      gppszBeforeImage = (char**)malloc(gnBeforeImageCount*sizeof(char*));
      if (gppszBeforeImage == NULL) FailAllocMessage();
      memset(gppszBeforeImage, 0, gnBeforeImageCount*sizeof(char*));
   }
   index = 0;
   while ((line=UtilGetALine(FP)) != NULL) {
      char full_id[MAXSTRING];

      scanLineNum++;
      if (*line == ']') {
         free(line);
         break;
      }
      if (index >= gnBeforeImageCount) {
         fprintf(stderr, "Warning: too many lines in before_image.\n");
         free(line);
         break;
      }
      c_ptr = FindChar((int)'"', line);
      c_ptr = ParseStr(c_ptr, (int)'"', full_id, sizeof(full_id));
      if (c_ptr == NULL) {
         ok = FALSE;
         free(line);
         break;
      }
      gppszBeforeImage[index] = UtilStrDup(full_id);
      if (gppszBeforeImage[index] == NULL) FailAllocMessage();
      index++;
      free(line);
   }
   if (ok && index < gnBeforeImageCount) {
      fprintf(stderr, "Warning: too few lines in before_image.\n");
      ok = FALSE;
   }
   if (!ok) {
      CleanUpBeforeImage();
   }
   return ok;
}

int ReadAfterPositions(FP, Inbuf)
   FILE *FP;
   char *Inbuf;
{
   char *c_ptr, *line;
   int index=0, ok=TRUE;

   c_ptr = FindChar((int)'(', Inbuf);
   InitScan(c_ptr, "\t\n, ");
   if (GETINT("after_positions", gnAfterPositionsCount,
         "after_positions_count") == INVALID) {
      return FALSE;
   }
   if (gnAfterPositionsCount > 0) {
      gpnAfterPositions = (int*)malloc(gnAfterPositionsCount*sizeof(int));
      if (gpnAfterPositions == NULL) FailAllocMessage();
      memset(gpnAfterPositions, 0, gnAfterPositionsCount*sizeof(int));
   }
   index = 0;
   while ((line=UtilGetALine(FP)) != NULL) {
      int pos=(-1);

      scanLineNum++;
      if (*line == ']') {
         free(line);
         break;
      }
      if (index >= gnAfterPositionsCount) {
         fprintf(stderr, "Warning: too many lines in before_image.\n");
         free(line);
         break;
      }
      InitScan(line, "\t\n, ");
      if (GETINT("after_positions", pos, "after_positions_pos") == INVALID) {
         ok = FALSE;
         free(line);
         break;
      }
      gpnAfterPositions[index++] = pos;
      free(line);
   }
   if (ok && index < gnAfterPositionsCount) {
      fprintf(stderr, "Warning: too few lines in after_positions.\n");
      ok = FALSE;
   }
   if (!ok) {
      CleanUpBeforeImage();
   }
   return ok;
}

int ReadAfterImage(FP, Inbuf)
   FILE *FP;
   char *Inbuf;
{
   struct ObjRec *obj_ptr=NULL;
   char *c_ptr;
   int ok=TRUE, count_expected=(-1), count=0;

   c_ptr = FindChar((int)'(', Inbuf);
   InitScan(c_ptr, "\t\n, ");
   if (GETINT("after_image", count_expected, "after_image_count") == INVALID) {
      return FALSE;
   }
   count = 0;
   while (ReadObj(FP, &obj_ptr)) {
      if (obj_ptr == NULL) {
         ok = FALSE;
         break;
      }
      obj_ptr->next = NULL;
      obj_ptr->prev = botObj;
      if (botObj == NULL) {
         topObj = obj_ptr;
      } else {
         botObj->next = obj_ptr;
      }
      botObj = obj_ptr;
      obj_ptr = NULL;
      count++;
   }
   if (ok && count != count_expected) {
      fprintf(stderr, "Warning: object count mismatched in after_image.\n");
      ok = FALSE;
   }
   return ok;
}

static
void SelectObjects(top_obj, bot_obj)
   struct ObjRec *top_obj, *bot_obj;
{
   struct ObjRec *obj_ptr;

   for (obj_ptr=bot_obj; obj_ptr != NULL; obj_ptr=obj_ptr->prev) {
      obj_ptr->tmp_parent = NULL;
      AddObjIntoSel(obj_ptr, NULL, topSel, &topSel, &botSel);
   }
   UpdSelBBox();
}

static
struct CmdRec *DeserializeCmd(psz_content_type, psz_buf, buf_sz, ppsz_plain_buf,
      pn_plain_buf_sz)
   char *psz_content_type, *psz_buf, **ppsz_plain_buf;
   int buf_sz, *pn_plain_buf_sz;
{
   struct StkRec *stk_ptr=NULL;
   struct ObjRec *obj_ptr=NULL, *saved_top_obj=NULL, *saved_bot_obj=NULL;
   int read_status=TRUE, ok=TRUE, do_inflate=FALSE, need_to_free_buf=FALSE;
   struct CmdRec *cmd_ptr=NULL;

   if (gstWBInfo.listen_fp == NULL) {
      sprintf(gszMsgBox, "Cannot open '%s'.\n\n%s pid=%ld.",
            gstWBInfo.listen_fname, "Deserialization aborted for",
            gstWBInfo.pid);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return NULL;
   }
#ifdef _TGIF_DBG_WB2
   fprintf(stderr, "Content-Type: %s\n",
         (psz_content_type!=NULL) ? psz_content_type : "(unknown)");
   fprintf(stderr, "Content-Length: %1d\n", buf_sz);
#endif /* _TGIF_DBG_WB2 */
   if (psz_content_type != NULL &&
         strcmp(psz_content_type, "application/x-tgif-cmd-z") == 0) {
      if (!HasZlibSupport()) {
         sprintf(gszMsgBox, "%s %s.\n\nDeserialization aborted.",
               TOOL_NAME, "is not compiled with ZLIB and cannot inflate()");
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
         return NULL;
      } else {
         if (gstWBInfo.gz_listen_fp == NULL) {
            sprintf(gszMsgBox, "Cannot open '%s'.\n\n%s pid=%ld.",
                  gstWBInfo.gz_listen_fname, "Deserialization aborted for",
                  gstWBInfo.pid);
            MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
            return NULL;
         }
         do_inflate = TRUE;
      }
   }

   if (do_inflate && HasZlibSupport()) {
      int rc=0;

      if (!DoInflate(psz_buf, buf_sz, gstWBInfo.gz_listen_fp, TRUE, &rc)) {
         ZlibError(rc, FALSE);
         return NULL;
      }
      buf_sz = (int)ftell(gstWBInfo.gz_listen_fp);
      psz_buf = (char*)malloc(buf_sz+1);
      if (psz_buf == NULL) FailAllocMessage();
      rewind(gstWBInfo.gz_listen_fp);
      if (fread(psz_buf, sizeof(char), buf_sz, gstWBInfo.gz_listen_fp) !=
            buf_sz) {
         sprintf(gszMsgBox, "Cannot read %1d bytes from '%s'.\n\n%s.",
               buf_sz, gstWBInfo.gz_listen_fname, "Deserialization aborted");
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
         UtilFree(psz_buf);
         return NULL;
      }
      psz_buf[buf_sz] = '\0';
      if (ppsz_plain_buf != NULL) {
         *pn_plain_buf_sz = buf_sz;
         *ppsz_plain_buf = UtilStrDup(psz_buf);
         if (*ppsz_plain_buf == NULL) FailAllocMessage();
      }
      need_to_free_buf = TRUE;
   }

#ifdef _TGIF_DBG_WB2
   if (wb2DebugLevel == 0) {
      fprintf(stderr, "Received %1d bytes, pid = %ld, timestamp = (below)\n",
            buf_sz, gstWBInfo.pid);
   } else {
      fprintf(stderr, "<<<------------------------\n");
      fprintf(stderr, "%s\n", psz_buf);
      fprintf(stderr, "<<<------------------------\n");
      fprintf(stderr, "Received %1d bytes, pid = %ld, timestamp = (below)\n",
            buf_sz, gstWBInfo.pid);
   }
   PrintFullIDsOfObjects("In the beginning of DeserializeCmd()");
#endif /* _TGIF_DBG_WB2 */

   rewind(gstWBInfo.listen_fp);
   writeFileFailed = FALSE;
   if (fprintf(gstWBInfo.listen_fp, "%s", psz_buf) == EOF) {
      writeFileFailed = TRUE;
   }
   if (writeFileFailed) {
      FailToWriteFileMessage(gstWBInfo.listen_fname);
      return NULL;
   }
   stk_ptr = SaveFileInfo();
   ResetFileInfo();
   /* if (usePaperSizeStoredInFile) ResetOnePageSize(); */

   ResetDeserializeCmd();

   rewind(gstWBInfo.listen_fp);
   importingFile = TRUE;

   strcpy(scanFileName, gstWBInfo.listen_fname);
   scanLineNum = 0;
   readingPageNum = loadedCurPageNum = 0;
   foundGoodStateObject = FALSE;
   while ((read_status=ReadObj(gstWBInfo.listen_fp, &obj_ptr)) == TRUE) {
      if (obj_ptr != NULL) {
         obj_ptr->tmp_parent = NULL;
         AdjForOldVersion(obj_ptr);
         AddObj(NULL, topObj, obj_ptr);
      }
   }
   importingFile = FALSE;

   if (read_status == INVALID) {
      sprintf(gszMsgBox, "File version too large (=%1d).  %s!",
            fileVersion, "Deserialization aborted");
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      ok = FALSE;
   } else {
#ifdef _TGIF_DBG_WB2
      fprintf(stderr, "%s, pid = %ld, timestamp = %1d/%s\n",
            "Command successfully parsed", gstWBInfo.pid, recordCmdLogicalClock,
            recordCmdSenderProcID);
#endif /* _TGIF_DBG_WB2 */
      if (recordCmdLogicalClock+1 > gstWBInfo.logical_clock) {
         /* Lamport's Algorithm set logical_clock */
         gstWBInfo.logical_clock = recordCmdLogicalClock+1;
#ifdef _TGIF_DBG_WB2
         fprintf(stderr, "Logical clock for pid = %ld set to %1d.\n",
               gstWBInfo.pid, gstWBInfo.logical_clock);
#endif /* _TGIF_DBG_WB2 */
      }
   }
   if (!ok) {
      DelAllObj();
   } else {
      saved_top_obj = topObj;
      saved_bot_obj = botObj;
      topObj = botObj = NULL;
   }
   RestoreFileInfo(stk_ptr);
   ResetOnePageSize();
   free(stk_ptr);
   /*
    * This is new.  Does it work?!
    */
   UpdPageStyle(pageStyle);

   if (ok) {
      cmd_ptr = (struct CmdRec *)malloc(sizeof(struct CmdRec));
      if (cmd_ptr == NULL) FailAllocMessage();
      memset(cmd_ptr, 0, sizeof(struct CmdRec));
      cmd_ptr->serialized = TRUE; /* created from deserialization */
      cmd_ptr->top_before = cmd_ptr->bot_before = NULL;
      cmd_ptr->pos_before = (int*)gppszBeforeImage;
      cmd_ptr->count_before = gnBeforeImageCount;
      cmd_ptr->type = gstDeserializeCmd.type;
      cmd_ptr->undone = TRUE;
      cmd_ptr->include_tgif_obj = recordCmdIncludeTgifObj;
      cmd_ptr->new_colormap = recordCmdUsesNewColormap;
      cmd_ptr->logical_clock = recordCmdLogicalClock;
      if (*recordCmdSenderProcID != '\0') {
         cmd_ptr->sender_process_id = UtilStrDup(recordCmdSenderProcID);
         if (cmd_ptr->sender_process_id == NULL) FailAllocMessage();
      }
      cmd_ptr->first_redo_after_deserialize = TRUE;
      cmd_ptr->skipped = FALSE;
#ifdef _TGIF_DBG_WB2
      DebugDumpCmd("<<< In DeserializeCmd()", cmd_ptr->type,
            cmd_ptr->logical_clock, cmd_ptr->sender_process_id);
#endif /* _TGIF_DBG_WB2 */
      if (saved_top_obj != NULL) {
         SelectObjects(saved_top_obj, saved_bot_obj);
         CopySel(topSel, numObjSelected,
               &(cmd_ptr->top_after), &(cmd_ptr->bot_after));
         cmd_ptr->pos_after = gpnAfterPositions;
         cmd_ptr->count_after = gnAfterPositionsCount;
         RemoveAllSel();
      } else {
         cmd_ptr->top_after = cmd_ptr->bot_after = NULL;
         cmd_ptr->pos_after = NULL;
         cmd_ptr->count_after = 0;
      }
      gppszBeforeImage = NULL;
      gnBeforeImageCount = 0;
      gpnAfterPositions = NULL;
      gnAfterPositionsCount = 0;
   }
   /*
    * Move this to the end of ProcessRemoteCmd() because we may need
    *         some information from gpDeserializeSubCmd.
    *
    * DeserializationCleanUp();
    */

#ifdef _TGIF_DBG_WB2
   PrintFullIDsOfObjects("At the end of DeserializeCmd()");
#endif /* _TGIF_DBG_WB2 */

   if (need_to_free_buf) UtilFree(psz_buf);

   return cmd_ptr;
}

static
int TotalOrderForTwo(older_cmd, newer_cmd)
   struct CmdRec *older_cmd, *newer_cmd;
   /*
    * Note: If logical clocks and host names are the same, and if pid in
    *       older_cmd is less than pid in newer_cmd, will return TRUE!
    */
{
   int comparison=0;

   if (older_cmd == NULL || newer_cmd == NULL) return TRUE;

   if (older_cmd->logical_clock > newer_cmd->logical_clock) {
      return FALSE;
   } else if (older_cmd->logical_clock < newer_cmd->logical_clock) {
      return TRUE;
   }
   comparison = strcmp(older_cmd->sender_process_id,
         newer_cmd->sender_process_id);
   TgAssert(comparison != 0,
         "Identical logical clock detected in TotalOrderForTwo()", NULL);
   return (comparison < 0);
}

#ifdef _TGIF_DBG_WB2
static
void DumpCmdStack(prefix_string)
   char *prefix_string;
   /* Dump the command stack, last one first */
{
   struct CmdRec *cmd_ptr=NULL;

   if (prefix_string != NULL) fprintf(stderr, "%s\n", prefix_string);
   for (cmd_ptr=gstWBInfo.last_cmd; cmd_ptr != NULL; cmd_ptr=cmd_ptr->prev) {
      char buf[256];

      if (cmd_ptr->skipped) {
         sprintf(buf, "\tcmd_ptr->next = 0x%08lx, (SKIPPED)",
               (long)cmd_ptr->next);
      } else {
         sprintf(buf, "\tcmd_ptr->next = 0x%08lx", (long)cmd_ptr->next);
      }
      DebugDumpCmd(buf, cmd_ptr->type, cmd_ptr->logical_clock,
            cmd_ptr->sender_process_id);
   }
}
#endif /* _TGIF_DBG_WB2 */

void FindCmdInsertionPoint(cmd_to_insert, pp_immed_right_cmd)
   struct CmdRec *cmd_to_insert, **pp_immed_right_cmd;
   /*
    * On return, cmd_to_insert is to be inserted immediately to the left
    *       of *pp_immed_right_cmd.
    */
{
   struct CmdRec *cmd_ptr=NULL;

   for (cmd_ptr=gstWBInfo.last_cmd; cmd_ptr != NULL; cmd_ptr=cmd_ptr->prev) {
      if (TotalOrderForTwo(cmd_ptr, cmd_to_insert)) {
         *pp_immed_right_cmd = cmd_ptr->next;
         return;
      }
   }
   *pp_immed_right_cmd = gstWBInfo.first_cmd;
}

void FindShadowCmdInsertionPoint(cmd_to_insert, pp_immed_right_cmd)
   struct CmdRec *cmd_to_insert, **pp_immed_right_cmd;
   /*
    * On return, cmd_to_insert is to be inserted immediately to the left
    *       of *pp_immed_right_cmd.
    */
{
   struct CmdRec *cmd_ptr=NULL;

   for (cmd_ptr=gstWBInfo.last_shadow_cmd; cmd_ptr != NULL;
         cmd_ptr=cmd_ptr->prev) {
      if (TotalOrderForTwo(cmd_ptr, cmd_to_insert)) {
         *pp_immed_right_cmd = cmd_ptr->next;
         return;
      }
   }
   *pp_immed_right_cmd = gstWBInfo.first_shadow_cmd;
}

static
void HighLightExistingObjects()
   /*
    * If select object no longer exists, don't select and highlight it!
    */
{
   int something_deleted=FALSE;
   struct SelRec *sel_ptr=NULL, *next_sel=NULL;

   for (sel_ptr=topSel; sel_ptr != NULL; sel_ptr=next_sel) {
      next_sel = sel_ptr->next;
      if (!IsTopLevelObject(sel_ptr->obj)) {
         UnlinkSel(sel_ptr, &topSel, &botSel);
         free(sel_ptr);
         something_deleted = TRUE;
      }
   }
   if (something_deleted) UpdSelBBox();
   HighLightForward();
}

void CleanUpObsoletedWBCmds(stopped_cmd_ptr)
   struct CmdRec *stopped_cmd_ptr;
{
   struct CmdRec *cmd_ptr=NULL;
   int num_records=0;

   for (cmd_ptr=stopped_cmd_ptr->prev; cmd_ptr != NULL; cmd_ptr=cmd_ptr->prev) {
      num_records++;
   }
   if (num_records > 0) {
      struct CmdRec *prev_cmd=NULL;
      double inc=(100.0/((double)num_records)), percent_start=0.0;

      ShowInterrupt(1);
      SaveStatusStrings();
      for (cmd_ptr=stopped_cmd_ptr->prev; cmd_ptr != NULL; cmd_ptr=prev_cmd,
            percent_start+=inc) {
         prev_cmd = cmd_ptr->prev;
         DeleteARedoRecord(cmd_ptr, percent_start,
               min(((double)percent_start+inc),((double)100.0)));
      }
      RestoreStatusStrings();
      HideInterrupt();
   }
   gstWBInfo.first_cmd = stopped_cmd_ptr;
   gstWBInfo.first_cmd->prev = NULL;

   CleanUpWBShadowCmds();
}

static
void HandleChatLine(cmd_ptr)
   struct CmdRec *cmd_ptr;
{
   struct ChatSubCmdRec *pChatSubCmdInfo=(&gpDeserializeSubCmd->detail.chat);

   switch (pChatSubCmdInfo->type) {
   case CHAT_STATE_NORMAL:
      ChatAppendChatLine(&pChatSubCmdInfo->tfi, pChatSubCmdInfo->nick_name,
            pChatSubCmdInfo->encoding, pChatSubCmdInfo->buf);
      break;
   case CHAT_STATE_HELLO:
      break;
   case CHAT_STATE_DISCONNECT:
      /* handle explicit user disconnect */
      break;
   }
   /*
    * Handle the chat line...
    */
   DeleteARedoRecord(cmd_ptr, 0, 0);
   gstWBInfo.cur_cmd = NULL;
}

void ProcessRemoteCmd(psz_content_type, psz_buf, buf_sz)
   char *psz_content_type, *psz_buf;
   int buf_sz;
{
   struct SelRec *saved_top_sel=topSel, *saved_bot_sel=botSel;
   struct CmdRec *cmd_ptr=NULL;
   char *psz_plain_buf=NULL;
   int highlighted_before=FALSE, plain_buf_sz=0;

   if (topSel != NULL) {
      highlighted_before = TRUE;
      HighLightReverse();
      topSel = botSel = NULL;
   }
   gstWBInfo.cur_cmd = DeserializeCmd(psz_content_type, psz_buf, buf_sz,
         &psz_plain_buf, &plain_buf_sz);

   if (gstWBInfo.cur_cmd != NULL) {
      struct CmdRec *immed_right_cmd=NULL;
      int cur_cmd_is_wb_clearall=(gstWBInfo.cur_cmd->type == CMD_WB_CLEARALL);

      if (gstWBInfo.cur_cmd->type == CMD_CHAT_A_LINE) {
         HandleChatLine(gstWBInfo.cur_cmd);
         /*
          * May be we need to allocate subcmd pointer here.
          */
      } else {
         if (gstWBInfo.cur_cmd->type == CMD_MOVE) {
            gstWBInfo.cur_cmd->subcmd =
                  (struct SubCmdRec *)malloc(sizeof(struct SubCmdRec));
            if (gstWBInfo.cur_cmd->subcmd == NULL) FailAllocMessage();
            memset(gstWBInfo.cur_cmd->subcmd, 0, sizeof(struct SubCmdRec));
            gstWBInfo.cur_cmd->subcmd->detail.move.dx =
                  gpDeserializeSubCmd->detail.move.dx;
            gstWBInfo.cur_cmd->subcmd->detail.move.dy =
                  gpDeserializeSubCmd->detail.move.dy;
         } else if (gstWBInfo.cur_cmd->type == CMD_WB_SLIDESHOW) {
            gstWBInfo.cur_cmd->subcmd =
                  (struct SubCmdRec *)malloc(sizeof(struct SubCmdRec));
            if (gstWBInfo.cur_cmd->subcmd == NULL) FailAllocMessage();
            memset(gstWBInfo.cur_cmd->subcmd, 0, sizeof(struct SubCmdRec));
            gstWBInfo.cur_cmd->subcmd->detail.slideshow.into_slideshow =
                  gpDeserializeSubCmd->detail.slideshow.into_slideshow;
         }
#ifdef _TGIF_DBG_WB2
         DumpCmdStack("Before FindCmdInsertionPoint()");
#endif /* _TGIF_DBG_WB2 */
         /*
          * FindShadowCmdInsertionPoint() examine the logical clock of the new
          *       command and decides where to insert it in the shadow cmd list.
          */
         FindShadowCmdInsertionPoint(gstWBInfo.cur_cmd, &immed_right_cmd);

         if (immed_right_cmd == NULL) {
            /* append */
            CopyAndInsertCmd(TRUE,
                  (psz_plain_buf==NULL ? psz_buf : psz_plain_buf),
                  gstWBInfo.last_shadow_cmd, NULL, gstWBInfo.cur_cmd,
                  &gstWBInfo.first_shadow_cmd, &gstWBInfo.last_shadow_cmd);
         } else {
            /* insert */
            CopyAndInsertCmd(TRUE,
                  (psz_plain_buf==NULL ? psz_buf : psz_plain_buf),
                  immed_right_cmd->prev, immed_right_cmd, gstWBInfo.cur_cmd,
                  &gstWBInfo.first_shadow_cmd, &gstWBInfo.last_shadow_cmd);
         }
         immed_right_cmd = NULL;
         /*
          * FindCmdInsertionPoint() examine the logical clock of the new command
          *       and decides where to insert it.
          */
         FindCmdInsertionPoint(gstWBInfo.cur_cmd, &immed_right_cmd);

         if (immed_right_cmd == NULL) {
            /* append */
            InsertCmd(gstWBInfo.last_cmd, NULL, gstWBInfo.cur_cmd,
                  &gstWBInfo.first_cmd, &gstWBInfo.last_cmd);
         } else {
            /* insert */
            InsertCmd(immed_right_cmd->prev, immed_right_cmd, gstWBInfo.cur_cmd,
                  &gstWBInfo.first_cmd, &gstWBInfo.last_cmd);
         }
#ifdef _TGIF_DBG_WB2
         DumpCmdStack("After FindCmdInsertionPoint()");
#endif /* _TGIF_DBG_WB2 */
         if (immed_right_cmd != NULL) {
            /*
             * Undo all the commands to the right of gstWBInfo.cur_cmd.
             * Do not perform the undo if the command is skipped.
             */
            for (cmd_ptr=gstWBInfo.last_cmd; cmd_ptr != gstWBInfo.cur_cmd &&
                  cmd_ptr->type != CMD_WB_CLEARALL; cmd_ptr=cmd_ptr->prev) {
               if (cmd_ptr->skipped) {
#ifdef _TGIF_DBG_WB2
                  strcpy(gszMsgBox, "*** No need to UndoACmd()");
                  DebugDumpCmd(gszMsgBox, cmd_ptr->type, cmd_ptr->logical_clock,
                        cmd_ptr->sender_process_id);
#endif /* _TGIF_DBG_WB2 */
                  cmd_ptr->skipped = FALSE;
               } else {
#ifdef _TGIF_DBG_WB2
                  strcpy(gszMsgBox, "*** Just before UndoACmd()");
                  DebugDumpCmd(gszMsgBox, cmd_ptr->type, cmd_ptr->logical_clock,
                        cmd_ptr->sender_process_id);
                  PrintFullIDsOfObjects("\t");
#endif /* _TGIF_DBG_WB2 */
                  UndoACmd(cmd_ptr, FALSE, TRUE);
#ifdef _TGIF_DBG_WB2
                  DebugDumpCmd("*** After UndoACmd()",
                        cmd_ptr->type, cmd_ptr->logical_clock,
                        cmd_ptr->sender_process_id);
                  PrintFullIDsOfObjects("\t");
#endif /* _TGIF_DBG_WB2 */
                  if (topSel != NULL) RemoveAllSel();
               }
            }
            if (cmd_ptr != gstWBInfo.cur_cmd) {
               gstWBInfo.cur_cmd = cmd_ptr;
            }
         }
         /*
          * Redo all the commands starting from gstWBInfo.cur_cmd.
          */
         if (gstWBInfo.cur_cmd->type == CMD_WB_CLEARALL) {
            CleanUpObsoletedWBCmds(gstWBInfo.cur_cmd);
            if (cur_cmd_is_wb_clearall) {
               struct ClearAllSubCmdRec *clearall=
                     &gpDeserializeSubCmd->detail.clearall;

               CleanUpDrawingWindow();
               ClearFileInfo(TRUE);
               SetFileModified(FALSE);
               ClearAndRedrawDrawWindow();
               CheckFileAttrsInLoad();
               Msg("WhiteBoard cleared by peer.");
               DelAllPages();
               lastPageNum = 1;
               InitPage();
               ShowPage();
               if (!(pageStyle == clearall->page_style &&
                     fabs(printMag-clearall->print_mag) < INT_TOL)) {
                  pageStyle = clearall->page_style;
                  printMag = clearall->print_mag;
                  UpdPageStyle(pageStyle);

                  UpdDrawWinWH();
                  RedrawScrollBars();
                  UpdDrawWinBBox();

                  SetDefaultDrawWinClipRecs();

                  DrawPaperBoundary(drawWindow);
                  RedrawGridLines(drawWindow);
                  RedrawPageLines(drawWindow);
                  RedrawRulers();
                  RedrawChoiceWindow();

                  RedrawTitleWindow();
               }
            }
         }
         for (cmd_ptr=gstWBInfo.cur_cmd; cmd_ptr != NULL;
               cmd_ptr=cmd_ptr->next) {
#ifdef _TGIF_DBG_WB2
            DebugDumpCmd("--- Just before RedoACmd()",
                  cmd_ptr->type, cmd_ptr->logical_clock,
                  cmd_ptr->sender_process_id);
            PrintFullIDsOfObjects("\t");
#endif /* _TGIF_DBG_WB2 */
            if (!RedoACmd(cmd_ptr, FALSE, TRUE)) {
               cmd_ptr->skipped = TRUE;
#ifdef _TGIF_DBG_WB2
               DebugDumpCmd("=== Skip RedoACmd()",
                     cmd_ptr->type, cmd_ptr->logical_clock,
                     cmd_ptr->sender_process_id);
               PrintFullIDsOfObjects("\t");
#endif /* _TGIF_DBG_WB2 */
            } else {
#ifdef NOT_DEFINED
               /* insanity check! */
               fprintf(stderr, "Sanity check: %s\n",
                     "need to comment this before distributing the code!");
               if (topSel != NULL) RemoveAllSel();
               UndoACmd(cmd_ptr, FALSE, TRUE);
               if (topSel != NULL) RemoveAllSel();
               if (!RedoACmd(cmd_ptr, FALSE, TRUE)) {
                  fprintf(stderr, "\7\7\7Oh no!!!  RedoACmd() failed!\n");
               }
#endif /* NOT_DEFINED */
#ifdef _TGIF_DBG_WB2
               DebugDumpCmd("--- Just after RedoACmd()",
                     cmd_ptr->type, cmd_ptr->logical_clock,
                     cmd_ptr->sender_process_id);
               PrintFullIDsOfObjects("\t");
#endif /* _TGIF_DBG_WB2 */
               if (topSel != NULL) RemoveAllSel();
            }
         }
#ifdef _TGIF_DBG_WB2
         PrintFullIDsOfObjects("At end of ProcessRemoteCmd()");
         fprintf(stderr, "%s%s\n",
               "========================================",
               "========================================");
#endif /* _TGIF_DBG_WB2 */
      }
   }
   DeserializationCleanUp();
   ResetDeserializeCmd();

   topSel = saved_top_sel;
   botSel = saved_bot_sel;
   UpdSelBBox();
   if (highlighted_before) {
      HighLightExistingObjects();
   }
   UtilFree(psz_plain_buf);
}

/* =================== Pack and Unpack current state Functions ======= */

static
void PackCurrentTGWBState(p_cur_state)
    CurStatus *p_cur_state;
{
   char tmp_fname[MAXPATHLENGTH], *buf=NULL, header[MAXSTRING<<1];
   char gz_tmp_fname[MAXPATHLENGTH];
   int content_sz=0, ok=TRUE, header_sz=0, num_cmds=0, cur_cmd_pos=0;
   int do_deflate=TRUE;
   struct CmdRec *cmd_ptr=NULL;
   long loc_sec=0L, loc_msec=0L;
   FILE *fp=NULL, *gz_fp=NULL;

   if (HasZlibSupport()) {
#ifdef _TGIF_DBG_WB2
      do_deflate = FALSE;
#endif /* _TGIF_DBG_WB2 */
   } else {
      do_deflate = FALSE;
   }
   memset(p_cur_state, 0, sizeof(CurStatus));
   if (MkTempFile(tmp_fname, sizeof(tmp_fname), tmpDir, TOOL_NAME) == NULL) {
      fprintf(stderr, TgLoadString(STID_CANT_CREATE_NAMED_TMP_FILE), tmp_fname);
      fprintf(stderr, "\n");
      return;
   }
   snprintf(gz_tmp_fname, sizeof(gz_tmp_fname), "%s.z", tmp_fname);

   if ((fp=fopen(tmp_fname, "w+")) == NULL) {
      FailToOpenMessage(tmp_fname, "w", NULL);
      return;
   }
   if ((gz_fp=fopen(gz_tmp_fname, "w+")) == NULL) {
      FailToOpenMessage(gz_tmp_fname, "w", NULL);
      fclose(fp);
      unlink(tmp_fname);
      return;
   }
   writeFileFailed = FALSE;

   UtilGetMilliSecTime(&loc_sec, &loc_msec);
   if (fprintf(fp, "%%TGWB begin packing - %ld.%ld\n", loc_sec, loc_msec) ==
         EOF) {
      writeFileFailed = TRUE;
   }
   for (cmd_ptr=gstWBInfo.first_shadow_cmd; cmd_ptr != NULL;
         cmd_ptr=cmd_ptr->next) {
      SerializationInfo si;

      memset(&si, 0, sizeof(SerializationInfo));
      PrepareExtendedSerializationInfo(cmd_ptr, &si);
      if (SaveExtendedCmd(fp, cmd_ptr, &si)) {
         if (cmd_ptr == gstWBInfo.cur_cmd) {
            cur_cmd_pos = num_cmds;
         }
         num_cmds++;
      }
   }
   ok = SaveExtendedCmdInfo(fp, num_cmds, cur_cmd_pos);

   if (fprintf(fp, "%%TGWB end - %ld.%ld\n", loc_sec, loc_msec) == EOF) {
      writeFileFailed = TRUE;
   }
   if (writeFileFailed) {
      FailToWriteFileMessage(tmp_fname);
      fclose(fp);
      unlink(tmp_fname);
      fclose(gz_fp);
      unlink(gz_tmp_fname);
      return;
   }
   if (!ok) {
      fclose(fp);
      unlink(tmp_fname);
      fclose(gz_fp);
      unlink(gz_tmp_fname);
      return;
   }
   fflush(fp);
   content_sz = (int)ftell(fp);
   /*
    * From: <PID>:<IP>\r\n
    * Content-Type: application/x-tgif-cmd\r\n
    * Content-Length: <LENGTH>\r\n
    * \r\n
    * <DATA of size LENGTH>
    */
   if (do_deflate && HasZlibSupport()) {
      int rc=0;

      if (!DoDeflate(fp, NULL, content_sz, gz_fp, TRUE, TRUE, &rc)) {
         ZlibError(rc, TRUE);
         do_deflate = FALSE;
      } else {
         content_sz = (int)ftell(gz_fp);
      }
   }
   if (do_deflate) {
      sprintf(header, "%s%s\r\n%s%s\r\n%s%1d\r\n\r\n",
            "From: ", gszLocalPID,
            "Content-Type: ", "application/x-tgif-cmd-z",
            "Content-Length: ", content_sz);
      header_sz = strlen(header);
      buf = (char*)malloc((header_sz+content_sz+1)*sizeof(char));
      if (buf == NULL) FailAllocMessage();
      strcpy(buf, header);
      rewind(gz_fp);
      if (fread(&buf[header_sz], sizeof(char), content_sz, gz_fp) !=
            content_sz) {
         sprintf(gszMsgBox,
               "Error in reading '%s'.\n\nPackCurrentTGWBState() aborted.",
               gz_tmp_fname);
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);

         UtilFree(buf);
         fclose(fp);
         unlink(tmp_fname);
         fclose(gz_fp);
         unlink(gz_tmp_fname);
         return;
      }
   } else {
      sprintf(header, "%s%s\r\n%s%s\r\n%s%1d\r\n\r\n",
            "From: ", gszLocalPID,
            "Content-Type: ", "application/x-tgif-cmd",
            "Content-Length: ", content_sz);
      header_sz = strlen(header);
      buf = (char*)malloc((header_sz+content_sz+1)*sizeof(char));
      if (buf == NULL) FailAllocMessage();
      strcpy(buf, header);
      rewind(fp);
      if (fread(&buf[header_sz], sizeof(char), content_sz, fp) !=
            content_sz) {
         sprintf(gszMsgBox,
               "Error in reading '%s'.\n\nPackCurrentTGWBState() aborted.",
               tmp_fname);
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);

         UtilFree(buf);
         fclose(fp);
         unlink(tmp_fname);
         fclose(gz_fp);
         unlink(gz_tmp_fname);
         return;
      }
   }
   buf[header_sz+content_sz] = '\0';
#ifdef _TGIF_DBG_WB2
   if (wb2DebugLevel == 0) {
      fprintf(stderr, "Sending %1d bytes, pid = %ld!\n",
            header_sz+content_sz+2, gstWBInfo.pid);
   } else {
      fprintf(stderr, "---------------------------\n");
      if (do_deflate) {
         fprintf(stderr, "%s", header);
         fprintf(stderr, "[content not displayed] ...\n");
      } else {
         fprintf(stderr, "%s\n", buf);
      }
      fprintf(stderr, "---------------------------\n");
      fprintf(stderr, "Sent %1d bytes, pid = %ld!\n", header_sz+content_sz+2,
            gstWBInfo.pid);
   }
   DebugDumpCmd(">>> In PackCurrentTGWBState()",
         (-1), (-1), gszLocalPID);
   PrintFullIDsOfObjects("In PackCurrentTGWBState()");
#endif /* _TGIF_DBG_WB2 */
   fclose(fp);
   unlink(tmp_fname);
   fclose(gz_fp);
   unlink(gz_tmp_fname);

   /* It is important to include the terminating '\0' */
   p_cur_state->data = buf;
   p_cur_state->size = header_sz+content_sz+1;
}

static
int CheckUnpackState(read_status)
   int read_status;
{
   int ok=TRUE;

   if (read_status == INVALID) {
      sprintf(gszMsgBox, "File version too large (=%1d).  %s!",
            fileVersion, "UnpackCurrentTGWBState() aborted");
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      ok = FALSE;
   } else {
#ifdef _TGIF_DBG_WB2
      fprintf(stderr, "%s, pid = %ld, timestamp = %1d/%s\n",
            "Command successfully parsed", gstWBInfo.pid, recordCmdLogicalClock,
            recordCmdSenderProcID);
#endif /* _TGIF_DBG_WB2 */
      if (recordCmdLogicalClock+1 > gstWBInfo.logical_clock) {
         /* Lamport's Algorithm set logical_clock */
         gstWBInfo.logical_clock = recordCmdLogicalClock+1;
#ifdef _TGIF_DBG_WB2
         fprintf(stderr, "Logical clock for pid = %ld set to %1d.\n",
               gstWBInfo.pid, gstWBInfo.logical_clock);
#endif /* _TGIF_DBG_WB2 */
      }
   }
   return ok;
}

static
struct CmdRec *CreateCmdFromSerializedData(saved_top_obj, saved_bot_obj)
   struct ObjRec *saved_top_obj, *saved_bot_obj;
{
   struct CmdRec *cmd_ptr=(struct CmdRec *)malloc(sizeof(struct CmdRec));

   if (cmd_ptr == NULL) FailAllocMessage();
   memset(cmd_ptr, 0, sizeof(struct CmdRec));
   cmd_ptr->serialized = TRUE; /* created from deserialization */
   cmd_ptr->top_before = cmd_ptr->bot_before = NULL;
   cmd_ptr->pos_before = (int*)gppszBeforeImage;
   cmd_ptr->count_before = gnBeforeImageCount;
   cmd_ptr->type = gstDeserializeCmd.type;
   cmd_ptr->undone = TRUE;
   cmd_ptr->include_tgif_obj = recordCmdIncludeTgifObj;
   cmd_ptr->new_colormap = recordCmdUsesNewColormap;
   cmd_ptr->logical_clock = recordCmdLogicalClock;
   if (*recordCmdSenderProcID != '\0') {
      cmd_ptr->sender_process_id = UtilStrDup(recordCmdSenderProcID);
      if (cmd_ptr->sender_process_id == NULL) FailAllocMessage();
   } else {
      fprintf(stderr,
            "Missing sender_process_id in CreateCmdFromSerializedData().\n");
   }
   cmd_ptr->first_redo_after_deserialize = TRUE;
   cmd_ptr->skipped = FALSE;
#ifdef _TGIF_DBG_WB2
   DebugDumpCmd("<<< In CreateCmdFromSerializedData()", cmd_ptr->type,
         cmd_ptr->logical_clock, cmd_ptr->sender_process_id);
#endif /* _TGIF_DBG_WB2 */
   if (saved_top_obj != NULL) {
      SelectObjects(saved_top_obj, saved_bot_obj);
      CopySel(topSel, numObjSelected,
            &(cmd_ptr->top_after), &(cmd_ptr->bot_after));
      cmd_ptr->pos_after = gpnAfterPositions;
      cmd_ptr->count_after = gnAfterPositionsCount;
      RemoveAllSel();
   } else {
      cmd_ptr->top_after = cmd_ptr->bot_after = NULL;
      cmd_ptr->pos_after = NULL;
      cmd_ptr->count_after = 0;
   }
   gppszBeforeImage = NULL;
   gnBeforeImageCount = 0;
   gpnAfterPositions = NULL;
   gnAfterPositionsCount = 0;

   return cmd_ptr;
}

static
int GetContentInfoFromBuf(buf, content_type_buf, content_type_buf_sz,
      pn_content_length, ppsz_buf_data_start)
   char *buf, *content_type_buf, **ppsz_buf_data_start;
   int content_type_buf_sz, *pn_content_length;
{
   char *psz_start=buf, *psz=NULL;
   char *content_type_prefix="Content-Type: ";
   char *content_len_prefix="Content-Length: ";
   int content_type_prefix_len=strlen(content_type_prefix);
   int content_len_prefix_len=strlen(content_len_prefix);
   int found_content_type=FALSE, found_content_len=FALSE;

   psz = strstr(psz_start, "\r\n");
   while (psz != NULL) {
      *psz = '\0';
      if (*psz_start == '\0') {
         *psz = '\r';
         psz_start = &psz[2];
         if (ppsz_buf_data_start != NULL) *ppsz_buf_data_start = psz_start;
         return (found_content_type && found_content_len);
      } else if (UtilStrNCaseCmp(psz_start, content_type_prefix,
            content_type_prefix_len) == 0) {
         UtilStrCpyN(content_type_buf, content_type_buf_sz,
               &psz_start[content_type_prefix_len]);
         UtilTrimBlanks(content_type_buf);
         found_content_type = TRUE;
      } else if (UtilStrNCaseCmp(psz_start, content_len_prefix,
            content_len_prefix_len) == 0) {
         char buf2[80];
         int ival=0;

         UtilStrCpyN(buf2, sizeof(buf2),
               &psz_start[content_len_prefix_len]);
         UtilTrimBlanks(buf2);
         if (sscanf(buf2, "%d", &ival) == 1) {
            if (pn_content_length != NULL) *pn_content_length = ival;
            found_content_len = TRUE;
         }
      }
      *psz = '\r';
      psz_start = &psz[2];
      psz = strstr(psz_start, "\r\n");
   }
   return FALSE;
}

static
int ReadPastWrapper(fp, pn_file_pos)
   FILE *fp;
   int *pn_file_pos;
{
   char *buf=NULL;
   char *psz_looking_for="%TGWB begin packing - ";
   int len=strlen(psz_looking_for);

   while ((buf=UtilGetALine(fp)) != NULL) {
      if (strncmp(buf, psz_looking_for, len) == 0) {
         if (pn_file_pos != NULL) *pn_file_pos = (int)ftell(fp);
         UtilFree(buf);
         return TRUE;
      }
      UtilFree(buf);
   }
   return FALSE;
}

static
int ReadPastHeader(fp, pn_file_pos)
   FILE *fp;
   int *pn_file_pos;
   /*
    * The From, Content-Type, Content-Length header is optional.  So,
    *         need to handle the case where there is a header and the case
    *         where there is none.
    */
{
   char *psz_looking_for="%TGWB begin - ";
   int len=strlen(psz_looking_for);
   int cur_pos=(int)ftell(fp);
   char *buf=NULL;

   while ((buf=UtilGetALine(fp)) != NULL) {
      char obj_name[80];

      *obj_name = '\0';
      UtilTrimBlanks(buf);
      if (*buf == '\0') {
         if (pn_file_pos != NULL) *pn_file_pos = (int)ftell(fp);
         UtilFree(buf);
         return TRUE;
      } else if (strncmp(buf, psz_looking_for, len) == 0) {
         if (pn_file_pos != NULL) *pn_file_pos = cur_pos;
         UtilFree(buf);
         return TRUE;
      } else if (*buf == ']' || *buf == '%') {
         fprintf(stderr,
               "Unexpected line (start with '%c' read in ReadPastHeader().\n",
               *buf);
         UtilFree(buf);
         return FALSE;
      } else if (ParseStr(buf, (int)'(', obj_name, sizeof(obj_name)) == NULL) {
         /* it's probably okay */
      } else if (strcmp(obj_name, "cmdxinfo") == 0) {
         int ok=ReadExtendedCmdInfo(fp, buf);

         UtilFree(buf);
         return ok;
      }
      UtilFree(buf);
      cur_pos = (int)ftell(fp);
   }
   return FALSE;
}

static
int ReadAheadOverACmd(fp, pn_file_pos)
   FILE *fp;
   int *pn_file_pos;
{
   char *buf=NULL;
   char *psz_looking_for="%TGWB end - ";
   int len=strlen(psz_looking_for);

   while ((buf=UtilGetALine(fp)) != NULL) {
      if (strncmp(buf, psz_looking_for, len) == 0) {
         if (pn_file_pos != NULL) *pn_file_pos = (int)ftell(fp);
         UtilFree(buf);
         return TRUE;
      }
      UtilFree(buf);
   }
   return FALSE;
}

static
char *ReReadCmd(fp, start_file_pos, end_file_pos, pn_buf_len)
   FILE *fp;
   int start_file_pos, end_file_pos, *pn_buf_len;
{
   char *buf=NULL;
   int ok=TRUE, buf_len=end_file_pos-start_file_pos;

   if (end_file_pos > start_file_pos) {
      buf_len = end_file_pos-start_file_pos;

      if (fseek(fp, start_file_pos, SEEK_SET) != 0) {
         fprintf(stderr,
               "Seek to %1d failed in UnpackCurrentTGWBState().\n",
               start_file_pos);
         ok = FALSE;
      } else {
         buf = (char*)malloc(buf_len+1);
         if (buf == NULL) FailAllocMessage();
         buf[buf_len] = '\0';
         if (buf_len != (int)fread(buf,
               sizeof(char), buf_len, fp)) {
            fprintf(stderr,
                  "Reading %1d bytes failed in %s.\n",
                  buf_len, "UnpackCurrentTGWBState()");
            ok = FALSE;
         }
      }
      if (ok && fseek(fp, start_file_pos, SEEK_SET) != 0) {
         fprintf(stderr,
               "Seek to %1d failed in UnpackCurrentTGWBState().\n",
               start_file_pos);
         ok = FALSE;
      }
   } else {
      fprintf(stderr,
            "Invalid (empty) cmd detected in UnpackCurrentTGWBState().\n");
      ok = FALSE;
   }
   if (!ok) {
      UtilFree(buf);

      return NULL;
   }
   if (pn_buf_len != NULL) *pn_buf_len = buf_len;

   return buf;
}

static
void UnpackCurrentTGWBState(p_cur_state)
    CurStatus *p_cur_state;
{
   struct StkRec *stk_ptr=NULL;
   struct ObjRec *obj_ptr=NULL, *saved_top_obj=NULL, *saved_bot_obj=NULL;
   int read_status=TRUE, ok=TRUE, start_file_pos=0, end_file_pos=0;
   int content_length=0, do_inflate=FALSE, need_to_free_buf=FALSE;
   char *buf=p_cur_state->data, tmp_fname[MAXPATHLENGTH], content_type[80];
   char *buf_data_start=NULL;
   FILE *fp=NULL;
   struct CmdRec *cmd_ptr=NULL;

   *content_type = '\0';
   if (!GetContentInfoFromBuf(buf, content_type, sizeof(content_type),
         &content_length, &buf_data_start)) {
      sprintf(gszMsgBox, TgLoadString(STID_JOIN_WB_IN_FAILED_NO_CONTTYPE));
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return;
   }
   if (strcmp(content_type, "application/x-tgif-cmd-z") == 0) {
      if (!HasZlibSupport()) {
         sprintf(gszMsgBox, "%s %s.\n\nDeserialization aborted in %s.",
               TOOL_NAME, "is not compiled with ZLIB and cannot inflate()",
               "UnpackCurrentTGWBState()");
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
         return;
      } else {
         if (gstWBInfo.gz_listen_fp == NULL) {
            sprintf(gszMsgBox, "Cannot open '%s'.\n\n%s pid=%ld in %s.",
                  gstWBInfo.gz_listen_fname, "Deserialization aborted for",
                  gstWBInfo.pid, "UnpackCurrentTGWBState()");
            MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
            return;
         }
         do_inflate = TRUE;
      }
   }
   if (do_inflate && HasZlibSupport()) {
      int rc=0, buf_sz=0;

      if (!DoInflate(buf_data_start, content_length, gstWBInfo.gz_listen_fp,
            TRUE, &rc)) {
         ZlibError(rc, FALSE);
         return;
      }
      buf_sz = (int)ftell(gstWBInfo.gz_listen_fp);
      buf = (char*)malloc(buf_sz+1);
      if (buf == NULL) FailAllocMessage();
      rewind(gstWBInfo.gz_listen_fp);
      if (fread(buf, sizeof(char), buf_sz, gstWBInfo.gz_listen_fp) !=
            buf_sz) {
         sprintf(gszMsgBox, "Cannot read %1d bytes from '%s'.\n\n%s for %s.",
               buf_sz, gstWBInfo.gz_listen_fname, "Deserialization aborted",
               "UnpackCurrentTGWBState()");
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
         UtilFree(buf);
         return;
      }
      need_to_free_buf = TRUE;
   }
   if (MkTempFile(tmp_fname, sizeof(tmp_fname), tmpDir, TOOL_NAME) == NULL) {
      fprintf(stderr, TgLoadString(STID_CANT_CREATE_NAMED_TMP_FILE), tmp_fname);
      fprintf(stderr, "\n");
      if (need_to_free_buf) UtilFree(buf);
      return;
   }
   if ((fp=fopen(tmp_fname, "w+")) == NULL) {
      FailToOpenMessage(tmp_fname, "w", NULL);
      if (need_to_free_buf) UtilFree(buf);
      return;
   }
   writeFileFailed = FALSE;

#ifdef _TGIF_DBG_WB2
   if (wb2DebugLevel == 0) {
      fprintf(stderr, "Received %1d bytes, pid = %ld\n", strlen(buf),
            gstWBInfo.pid);
   } else {
      fprintf(stderr, "<<<========================\n");
      fprintf(stderr, "%s\n", buf);
      fprintf(stderr, "<<<========================\n");
      fprintf(stderr, "Received %1d bytes, pid = %ld\n", strlen(buf),
            gstWBInfo.pid);
   }
   PrintFullIDsOfObjects("At the beginning of UnpackCurrentTGWBState()");
#endif /* _TGIF_DBG_WB2 */

   writeFileFailed = FALSE;
   if (fprintf(fp, "%s", buf) == EOF) {
      writeFileFailed = TRUE;
   }
   if (writeFileFailed) {
      FailToWriteFileMessage(gstWBInfo.listen_fname);
      fclose(fp);
      unlink(tmp_fname);
      if (need_to_free_buf) UtilFree(buf);
      return;
   }
   stk_ptr = SaveFileInfo();
   ResetFileInfo();
   /* if (usePaperSizeStoredInFile) ResetOnePageSize(); */

   ResetDeserializeCmd();

   rewind(fp);
   importingFile = TRUE;

   strcpy(scanFileName, gstWBInfo.listen_fname);
   scanLineNum = 0;
   foundGoodStateObject = FALSE;

   seenCmdXInfo = FALSE;
   *content_type = '\0';
   ok = ReadPastWrapper(fp, NULL);
   while (ok && !seenCmdXInfo) {
      char *serialization_buf=NULL;
      int serialization_buf_len=0;

      DelAllPages();
      lastPageNum = 1;
      InitPage();
      readingPageNum = loadedCurPageNum = 0;

      ok = ReadPastHeader(fp, &start_file_pos);
      if (ok) {
         if (seenCmdXInfo) {
            break;
         }
         ok = ReadAheadOverACmd(fp, &end_file_pos);
         if (ok) {
            serialization_buf = ReReadCmd(fp, start_file_pos, end_file_pos,
                  &serialization_buf_len);
            if (serialization_buf == NULL) {
               ok = FALSE;
#ifdef _TGIF_DBG_WB2
            } else {
               static int n=1;

               fprintf(stderr, "******* cmd %1d begin *******\n", n);
               fprintf(stderr, "%s", serialization_buf);
               fprintf(stderr, "******* cmd %1d end *******\n", n++);
#endif /* _TGIF_DBG_WB2 */
            }
         }
      }
      if (ok) {
         while ((read_status=ReadObj(fp, &obj_ptr)) == TRUE) {
            if (obj_ptr != NULL) {
               fprintf(stderr,
                     "Unexpected objects read in UnpackCurrentTGWBState().\n");
            }
         }
         ok = CheckUnpackState(read_status);
         if (!ok) {
            DelAllObj();
         } else {
            saved_top_obj = topObj;
            saved_bot_obj = botObj;
            topObj = botObj = NULL;
         }
         DelAllPages();
         lastPageNum = 0;
         InitPage();
      }
      if (ok) {
         cmd_ptr = CreateCmdFromSerializedData(saved_top_obj, saved_bot_obj);
         if (cmd_ptr->type == CMD_MOVE) {
            cmd_ptr->subcmd =
                  (struct SubCmdRec *)malloc(sizeof(struct SubCmdRec));
            if (cmd_ptr->subcmd == NULL) FailAllocMessage();
            memset(cmd_ptr->subcmd, 0, sizeof(struct SubCmdRec));
            cmd_ptr->subcmd->detail.move.dx =
                  gpDeserializeSubCmd->detail.move.dx;
            cmd_ptr->subcmd->detail.move.dy =
                  gpDeserializeSubCmd->detail.move.dy;
         } else if (cmd_ptr->type == CMD_WB_SLIDESHOW) {
            cmd_ptr->subcmd =
                  (struct SubCmdRec *)malloc(sizeof(struct SubCmdRec));
            if (cmd_ptr->subcmd == NULL) FailAllocMessage();
            memset(cmd_ptr->subcmd, 0, sizeof(struct SubCmdRec));
            cmd_ptr->subcmd->detail.slideshow.into_slideshow =
                  gpDeserializeSubCmd->detail.slideshow.into_slideshow;
         }
         if (cmd_ptr != NULL) {
            /* append */
            CopyAndInsertCmd(TRUE, serialization_buf, gstWBInfo.last_shadow_cmd,
                  NULL, cmd_ptr, &gstWBInfo.first_shadow_cmd,
                  &gstWBInfo.last_shadow_cmd);
            InsertCmd(gstWBInfo.last_cmd, NULL, cmd_ptr, &gstWBInfo.first_cmd,
                  &gstWBInfo.last_cmd);
         }
      }
#ifdef _TGIF_DBG_WB2
      PrintFullIDsOfObjects("In UnpackCurrentTGWBState()");
#endif /* _TGIF_DBG_WB2 */
      DeserializationCleanUp();
      ResetDeserializeCmd();
   }
   importingFile = FALSE;

   fclose(fp);
   unlink(tmp_fname);

   RestoreFileInfo(stk_ptr);
   ResetOnePageSize();
   free(stk_ptr);

   UpdPageStyle(pageStyle);
   /*
    * Now execute the commands!
    */
   if (ok) {
      gstWBInfo.cur_cmd = NULL;
      for (cmd_ptr=gstWBInfo.first_cmd; cmd_ptr != NULL;
            cmd_ptr=cmd_ptr->next) {
#ifdef _TGIF_DBG_WB2
         DebugDumpCmd("--- Just before RedoACmd()",
               cmd_ptr->type, cmd_ptr->logical_clock,
               cmd_ptr->sender_process_id);
         PrintFullIDsOfObjects("\t");
#endif /* _TGIF_DBG_WB2 */
         if (!RedoACmd(cmd_ptr, FALSE, TRUE)) {
            cmd_ptr->skipped = TRUE;
#ifdef _TGIF_DBG_WB2
            DebugDumpCmd("=== Skip RedoACmd()",
                  cmd_ptr->type, cmd_ptr->logical_clock,
                  cmd_ptr->sender_process_id);
            PrintFullIDsOfObjects("\t");
#endif /* _TGIF_DBG_WB2 */
            gstWBInfo.cur_cmd = cmd_ptr;
            break;
         } else {
#ifdef _TGIF_DBG_WB2
            DebugDumpCmd("--- Just after RedoACmd()",
                  cmd_ptr->type, cmd_ptr->logical_clock,
                  cmd_ptr->sender_process_id);
            PrintFullIDsOfObjects("\t");
#endif /* _TGIF_DBG_WB2 */
            if (topSel != NULL) RemoveAllSel();
         }
      }
      gstWBInfo.join_session_in_progress = TRUE;
   } else {
      CleanUpWBCmds();
      sprintf(gszMsgBox, TgLoadString(STID_JOIN_WB_IN_PROGRESS_FAILED));
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
   }
   if (need_to_free_buf) UtilFree(buf);
}

void HandleNewUserRequest()
{
   char buf[80];
   int buf_sz=sizeof(buf);

   /*
    * Get current tgwb state and establish
    *         a TCP conection to send the data.
    */
   int index=0;

   if (!gstWBInfo.do_whiteboard) {
      return;
   }
   for (index=0; index < buf_sz-1; index++) {
      if (read(talkToSelfFiledes[0], &buf[index], 1) != 1) {
         sprintf(gszMsgBox, TgLoadString(STID_READ_FROM_SELF_PIPE_FAIL), 1,
               TOOL_NAME),
         fprintf(stderr, "%s\n", gszMsgBox);
         index = (-1);
         break;
      } else if (buf[index] == '\0') {
         break;
      }
   }
   if (index == buf_sz-1) {
      sprintf(gszMsgBox, TgLoadString(STID_READ_FROM_SELF_PIPE_OVERFLOW), 
            TOOL_NAME),
      fprintf(stderr, "%s\n", gszMsgBox);
      index = (-1);
   }
   if (index != (-1)) {
      int conn=0;
      CurStatus cur_state;

      memset(&cur_state, 0, sizeof(CurStatus));
      sscanf(buf, "%d", &conn);
      serializingFile = TRUE;
      PackCurrentTGWBState(&cur_state);
      serializingFile = FALSE;
      if (cur_state.size != 0) {
         RM_sendCurStatus(conn, (char*)cur_state.data, cur_state.size);
         UtilFree(cur_state.data);
      }
   }
}

/* =================== SIGPOLL Handler function =================== */

/* #define DEBUG0 */

#if (defined(PTHREAD) || defined(HAVE_LIBPTHREAD))
static
void *ReceivePacket( void *arg )
{
    while( 1 )
    {
        char *buf=NULL, *aux=NULL, id[300];
        int byread=0;
        struct SegmentationPack pack1;
    
        if( (byread = RM_recv(gstWBInfo.MCastSock, (char*) &pack1, sizeof(struct SegmentationPack))) > 0 )
        {
            char *buffer1=NULL;
        
#ifdef DEBUG0
            fprintf(stdout, "__________# of received packet: %u of %u\n", ntohl(pack1.NumSeq), ntohl(pack1.NumPackets));
#endif
            if( (buffer1 = DeSegment( pack1, &byread )) != NULL ){
                    buf = (char *)malloc( byread + 1 );
                    memcpy( buf, buffer1, byread );
                    buf[byread]= '\0';
                    free(buffer1);
#ifdef DEBUG0                    
                    fprintf(stdout, "******Desegmentation OK\n");
#endif                    
            }
            else
            {
                continue;
            }
        } else if (byread == 0)
        {
             continue;
        }
        else {
             /* on error, break - no more messages to receive  (shutting down) */
             break;
        }

        /*
         * check if the message is from itself
         */

        
        if (strncmp(buf, "From: ", 6) != 0 || sscanf(&buf[6], "%s\r\n", id) != 1) {
           fprintf(stderr, "%s.  %s, pid = %ld=tid=%s.\nbuf=%s.\n",
                 "Fatal error", "Cannot find the sender id in IO_SignalHandler()",
                 gstWBInfo.pid, id, buf);
           free(buf);
           continue;
        }

        if(strcmp(id, gszLocalPID) == 0) {
           continue;
        }

#ifdef DEBUG0        
        /* Verify if either the command may be processed right now, or if it should be stored */
        fprintf(stderr,"Process the comand now? ");
#endif        

        if ((aux = (char *)malloc( byread + 1 )) == NULL)
        {
         fprintf(stderr,"Allocation error. (ReceivePacket)\n");
         exit(1);
        }
        strncpy(aux, buf, byread);
        aux[byread] = (char)0;
        buff_ins(gstWBInfo.bd_commands, aux);
        free(aux);
        free(buf);

        /*
         * Don't think we need this any more!
         *
         * if(gstWBInfo.BlockRemoteCmdDepth > 0) {

#ifdef DEBUG           
         *    buff_show(gstWBInfo.bd_commands);
#endif           
         *    continue;
         * }
         * else
         */
        {
            static char c='a';

            if (write(talkToSelfFiledes[1], &c, 1) != 1) {
               sprintf(gszMsgBox, TgLoadString(STID_WRITE_TO_SELF_PIPE_FAIL),
                     TOOL_NAME);
               fprintf(stderr, "%s\n", gszMsgBox);
            }
        }
    }
    return( NULL );
}
#endif /* (defined(PTHREAD) || defined(HAVE_LIBPTHREAD)) */

/* =================== Init and CleanUp Functions =================== */

void CleanUpWhiteBoard()
{
   int i=0;
   
   if (!gstWBInfo.do_whiteboard) {
      return;
   }
   snprintf(gszMsgBox, sizeof(gszMsgBox), "****%s**** ",
         TgLoadString(STID_SHUTTING_DOWN_WHITEBOARD));
   Msg(gszMsgBox);
   
   RM_leaveGroup( gstWBInfo.MCastSock, (char*)RM_USE_CURRENT_CONFIG );
   fprintf(stderr, "%s ", gszMsgBox);
   for (i=0; i < 10; i++) {
      fprintf(stderr, ".");
      MillisecondSleep(100);
   }
   fprintf(stderr, "\n");
   RMCastCleanUp();
   buff_destroy(gstWBInfo.bd_commands);

   CleanUpWBSockets();
   CleanUpWBCmds();
   memset(&gstWBInfo, 0, sizeof(struct WhiteBoardRec));

   CleanUpDeserializeCmd();
   buff_cleanup();
}

void handler(int a)
{
   fprintf(stderr,"alarm catched in wb.c\n");
   exit(1);
}

int InitWhiteBoard()
{
   InitLocalPID(); /* initializes gszLocalPID */

   memset(&gstWBInfo, 0, sizeof(struct WhiteBoardRec));

   gstWBInfo.BlockRemoteCmdDepth = 1; /* FIXME Should be zero? */
   gstWBInfo.pid = (long)getpid();

   if (cmdLineWb2) {
      gstWBInfo.do_whiteboard = TRUE;
   } 
   if (cmdLineNoWhiteBoard) gstWBInfo.do_whiteboard = FALSE;

   *gstWBInfo.send_fname = *gstWBInfo.gz_send_fname = '\0';
   gstWBInfo.send_fp = gstWBInfo.gz_send_fp = NULL;
   gstWBInfo.send_socket = (-1);

   gstWBInfo.listening = FALSE;

   *gstWBInfo.listen_fname = *gstWBInfo.gz_listen_fname = '\0';
   gstWBInfo.listen_fp = gstWBInfo.gz_listen_fp = NULL;
   gstWBInfo.listen_socket = (-1);

#ifdef _TGIF_DBG_WB2
   {
      char *c_ptr=NULL;

      wb2DebugLevel = 0;
      if ((c_ptr=XGetDefault(mainDisplay, TOOL_NAME, "DebugWB2")) != NULL) {
         wb2DebugLevel = atoi(c_ptr);
      }
   }
#endif /* _TGIF_DBG_WB2 */

   memset(&gRMCastIntrfInfo, 0, sizeof(RMCastIntrfInfo));

   return TRUE;
}

int InitWhiteBoard2()
{
   int want_new_member_support=0;
#if (defined(PTHREAD) || defined(HAVE_LIBPTHREAD))
   pthread_t tid=(pthread_t)0;
#endif /* (defined(PTHREAD) || defined(HAVE_LIBPTHREAD)) */
   char tgwb_config_file[255];
   char *homedir=NULL;
   int config_ok=0;

   if (!gstWBInfo.do_whiteboard) return TRUE;

#if (!defined(PTHREAD) && !defined(HAVE_LIBPTHREAD))
   sprintf(gszMsgBox, TgLoadString(STID_NO_PTHREAD_CONTINUE), TOOL_NAME,
         TOOL_NAME, TOOL_NAME);
   MsgBox(gszMsgBox, TOOL_NAME, STOP_MB);
   gstWBInfo.do_whiteboard = FALSE;
   SendCommandToSelf(CMDID_QUIT, 0);
   return FALSE;
#endif /* (!defined(PTHREAD) && !defined(HAVE_LIBPTHREAD)) */

   if (!RMCastInit()) {
      gstWBInfo.do_whiteboard = FALSE;
      return FALSE;
   }
   InitDeserializeCmd();

   if (!CreateWBListenSocket()) {
      CleanUpWBSockets();
      exit(-1);
   }

   if (MkTempFile(gstWBInfo.send_fname, sizeof(gstWBInfo.send_fname),
         tmpDir, TOOL_NAME) == NULL) {
      fprintf(stderr, "Fail to create '%s' for serialization.\n",
            gstWBInfo.send_fname);
      exit(-1);
   }
   snprintf(gstWBInfo.gz_send_fname, sizeof(gstWBInfo.gz_send_fname),
         "%s.z", gstWBInfo.send_fname);
   if ((gstWBInfo.send_fp=fopen(gstWBInfo.send_fname, "w+")) == NULL) {
      fprintf(stderr, "Fail to create '%s' for serialization.\n",
            gstWBInfo.send_fname);
      exit(-1);
   }
   if ((gstWBInfo.gz_send_fp=fopen(gstWBInfo.gz_send_fname, "w+")) == NULL) {
      fprintf(stderr, "Fail to create '%s' for serialization.\n",
            gstWBInfo.gz_send_fname);
      exit(-1);
   }

   if ( (homedir=getenv("HOME")) != NULL ){
     sprintf(tgwb_config_file,"%s/.tgwb/tgwb.conf",homedir);
     if (RM_readConfigFile(tgwb_config_file, 0))
          config_ok=2; /* It means that we read $HOME/.tgwb/tgwb.conf */
   }
   else{
      fprintf(stderr,"TGWB Warning: Can't get environment variable 'HOME', using default values.\n");
   }
   if (config_ok==0 && RM_readConfigFile("/etc/tgwb.conf", 0)){
        config_ok=1; /* It means that we read /etc/tgwb.conf */
   }

   switch (config_ok){
      case 1:
         fprintf(stderr,"Using config file: '/etc/tgwb.conf'\n");
         break;
      case 2:
         fprintf(stderr,"Using config file: '%s'\n",tgwb_config_file);
         break;
      default:
         fprintf(stderr,"TGWB Warning:\n Config files:\n\t'%s/.tgwb/tgwb.conf'\n\t'/etc/tgwb.conf'\n not found.\nUsing default values.\n",homedir);
   }

   /* setting defaults */

   RM_setOption(PIPE, &talkToSelfFiledes);
   RM_setOption(SHUT_DOWN_ROUTINE, &CleanUpWBSockets);

   RM_initialize((void *)RMcallback_terminate);

   RM_getOption(NEW_MEMBER_SUPPORT, (void*)&want_new_member_support);

   if (want_new_member_support)
   {
        CurStatus cur_state;

        fprintf(stderr,"Getting the current status.\n");

        RM_getCurStatus( (char*)RM_USE_CURRENT_CONFIG, RM_USE_CURRENT_CONFIG,
                &cur_state);

        if (cur_state.size != -1)
        {
            int saved_disable_redraw=disableRedraw;

            deserializingFile = TRUE;
            disableRedraw = TRUE;
            UnpackCurrentTGWBState(&cur_state);
            disableRedraw = saved_disable_redraw;
            if (!saved_disable_redraw) {
               ClearAndRedrawDrawWindow();
            }
            deserializingFile = FALSE;
        }
   }
   gstWBInfo.MCastSock = RM_joinGroup((char*)RM_USE_CURRENT_CONFIG, RM_USE_CURRENT_CONFIG );

   gstWBInfo.bd_commands = buff_init (10, WB_PACK, UNSORTED, NULL );

/* debugging RMCAST
   fprintf(stderr,"buffer descriptor: %d\n", gstWBInfo.bd_commands);
*/
#if (defined(PTHREAD) || defined(HAVE_LIBPTHREAD))
   if( pthread_create( &tid, NULL, ReceivePacket, NULL ) != 0 )
   {
       fprintf( stderr, "Could not create receive thread\n" );
       exit( -1 );
   }
#endif /* (defined(PTHREAD) || defined(HAVE_LIBPTHREAD)) */
   return TRUE;
}

void RMcallback_terminate( void )
{
    /*
     * tgif must do its clean up here before exit
     *
     * exit(0);
     */
}

