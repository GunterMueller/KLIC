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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/cmd.c,v 1.15 2011/05/16 16:21:56 william Exp $
 */

#define _INCLUDE_FROM_CMD_C_

#include "tgifdefs.h"

#include "attr.e"
#include "choice.e"
#include "cmd.e"
#include "color.e"
#include "dialog.e"
#include "drawing.e"
#include "dup.e"
#include "file.e"
#include "grid.e"
#include "imgproc.e"
#include "mainmenu.e"
#include "mark.e"
#include "miniline.e"
#include "move.e"
#include "msg.e"
#include "navigate.e"
#include "obj.e"
#include "page.e"
#include "remote.e"
#include "select.e"
#include "setup.e"
#include "stk.e"
#include "strtbl.e"
#include "text.e"
#include "util.e"
#include "wb.e"
#include "xpixmap.e"

int		recordCmdIncludeTgifObj=FALSE;
int		recordCmdUsesNewColormap=FALSE;
int		undoingOrRedoing=FALSE;
int		historyDepth=(-1), historyCount=0, defaultHistoryDepth=(-1);
struct CmdRec	*firstCmd=NULL, *lastCmd=NULL, *curCmd=NULL;

struct SelRec	*topSelBeforeInCmd=NULL, *botSelBeforeInCmd=NULL;
int		*stackingPosition=NULL;
int		stackingCount=0;
int		stackingPositionHasIds=FALSE;

int		composingCommand=FALSE;
int		undoRedoRestoreDrawingMode=FALSE;
Colormap	preparedColormap=None;

void InsertCmd(PrevCmd, NextCmd, CmdPtr, ppFirstCmd, ppLastCmd)
   struct CmdRec *PrevCmd, *NextCmd, *CmdPtr;
   struct CmdRec **ppFirstCmd, **ppLastCmd;
   /* add CmdPtr between PrevCmd and NextCmd */
   /* update *ppFirstCmd and *ppLastCmd */
{
   CmdPtr->prev = PrevCmd;
   CmdPtr->next = NextCmd;

   if (PrevCmd == NULL) {
      (*ppFirstCmd) = CmdPtr;
   } else {
      PrevCmd->next = CmdPtr;
   }
   if (NextCmd == NULL) {
      (*ppLastCmd) = CmdPtr;
   } else {
      NextCmd->prev = CmdPtr;
   }
}

static
char *GetCmdType(CmdType)
   int CmdType;
{
   switch (CmdType) {
   case CMD_COMPOSITE: return "CMD_COMPOSITE";
   case CMD_NEW: return "CMD_NEW";
   case CMD_DELETE: return "CMD_DELETE";
   case CMD_MOVE: return "CMD_MOVE";
   case CMD_STRETCH: return "CMD_STRETCH";
   case CMD_ONE_TO_MANY: return "CMD_ONE_TO_MANY";
   case CMD_MANY_TO_ONE: return "CMD_MANY_TO_ONE";
   case CMD_REPLACE: return "CMD_REPLACE";
   case CMD_GOTO_PAGE: return "CMD_GOTO_PAGE";
   case CMD_WB_CLEARALL: return "CMD_WB_CLEARALL";
   case CMD_CHAT_A_LINE: return "CMD_CHAT_A_LINE";
   case CMD_WB_SLIDESHOW: return "CMD_WB_SLIDESHOW";
   default: return "(unknown)";
   }
}

static
struct CmdRec *CreateCmdFromSerializationBuf(cmd_type, buf)
   int cmd_type;
   char *buf;
{
   struct CmdRec *cmd_ptr=NULL;

   cmd_ptr = (struct CmdRec *)malloc(sizeof(struct CmdRec));
   if (cmd_ptr == NULL) FailAllocMessage(); 
   memset(cmd_ptr, 0, sizeof(struct CmdRec)); 

   cmd_ptr->type = cmd_type;
   cmd_ptr->serialization_buf = UtilStrDup(buf);
   if (cmd_ptr->serialization_buf == NULL) FailAllocMessage();

   return cmd_ptr;
}

void CopyAndInsertCmd(from_remote, serialization_buf, PrevCmd, NextCmd, CmdPtr,
      ppFirstCmd, ppLastCmd)
   int from_remote;
   char *serialization_buf;
   struct CmdRec *PrevCmd, *NextCmd, *CmdPtr;
   struct CmdRec **ppFirstCmd, **ppLastCmd;
   /* add CmdPtr between PrevCmd and NextCmd */
   /* update *ppFirstCmd and *ppLastCmd */
{
   if ((!from_remote && CmdPtr->undone) || (from_remote && !CmdPtr->undone)) {
      sprintf(gszMsgBox, "(%s) Found an %s command, type = %s.",
            (from_remote ? "from_remote" : "local"),
            (CmdPtr->undone ? "undone" : "not undone"),
            GetCmdType(CmdPtr->type));
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
   } else {
      struct CmdRec *cmd_ptr=NULL;

#ifdef _TGIF_DBG_WB2
      fprintf(stdout,
            "<*><*><*>(%s) CopyAndInsertCmd() a command of type: %s.\n",
            (from_remote ? "from_remote" : "local"),
            GetCmdType(CmdPtr->type));
      fflush(stdout);
#endif /* _TGIF_DBG_WB2 */

      cmd_ptr = CreateCmdFromSerializationBuf(CmdPtr->type, serialization_buf);

      cmd_ptr->prev = PrevCmd;
      cmd_ptr->next = NextCmd;

      if (PrevCmd == NULL) {
         (*ppFirstCmd) = cmd_ptr;
      } else {
         PrevCmd->next = cmd_ptr;
      }
      if (NextCmd == NULL) {
         (*ppLastCmd) = cmd_ptr;
      } else {
         NextCmd->prev = cmd_ptr;
      }
   }
}

static
void UnlinkCmd(CmdPtr, ppFirstCmd, ppLastCmd)
   struct CmdRec *CmdPtr;
   struct CmdRec **ppFirstCmd, **ppLastCmd;
{
   if ((*ppFirstCmd) == CmdPtr) {
      (*ppFirstCmd) = CmdPtr->next;
   } else {
      CmdPtr->prev->next = CmdPtr->next;
   }
   if ((*ppLastCmd) == CmdPtr) {
      (*ppLastCmd) = CmdPtr->prev;
   } else {
      CmdPtr->next->prev = CmdPtr->prev;
   }
}

void DeleteARedoRecord(CmdPtr, percent_start, percent_end)
   struct CmdRec *CmdPtr;
   double percent_start, percent_end;
{
   struct SelRec *sel_ptr=NULL, *next_sel=NULL;
   struct CmdRec *cmd_ptr=NULL, *prev_cmd=NULL;
   int num_records=0;
   double inc=0;

   if (CmdPtr->pos_before_has_ids) {
      if (CmdPtr->pos_before != NULL) {
         int i=0;
         char **s_ptr=(char**)(CmdPtr->pos_before);

         for (i=0; i < CmdPtr->count_before; i++) {
            if (s_ptr[i] != NULL) {
               UtilFree(s_ptr[i]);
            } else {
               break;
            }
         }
         free(CmdPtr->pos_before);
      }
   } else {
      if (CmdPtr->pos_before != NULL) free(CmdPtr->pos_before);
   }
   if (CmdPtr->pos_after != NULL) free(CmdPtr->pos_after);

   if (percent_start >= 0.0) {
      sprintf(gszMsgBox, TgLoadCachedString(CSTID_FLUSH_UNDO_PERCENT),
            round(percent_start));
      SetStringStatus(gszMsgBox);
      CheckInterrupt(TRUE);
   }
   switch (CmdPtr->type) {
   case CMD_NEW:
      for (sel_ptr=CmdPtr->top_after; sel_ptr!=NULL; sel_ptr=next_sel) {
         next_sel = sel_ptr->next;
         if (CmdPtr->undone) FreeObj(sel_ptr->obj);
         free(sel_ptr);
      }
      break;
   case CMD_DELETE:
      for (sel_ptr=CmdPtr->top_before; sel_ptr!=NULL; sel_ptr=next_sel) {
         next_sel = sel_ptr->next;
         if (!CmdPtr->undone) FreeObj(sel_ptr->obj);
         free(sel_ptr);
      }
      break;
   case CMD_MOVE:
      for (sel_ptr=CmdPtr->top_before; sel_ptr!=NULL; sel_ptr=next_sel) {
         next_sel = sel_ptr->next;
         free(sel_ptr);
      }
      if (CmdPtr->subcmd != NULL) {
         free(CmdPtr->subcmd);
      }
      break;
   case CMD_STRETCH:
      for (sel_ptr=CmdPtr->top_before; sel_ptr!=NULL; sel_ptr=next_sel) {
         next_sel = sel_ptr->next;
         FreeObj(sel_ptr->obj);
         free(sel_ptr);
      }
      if (CmdPtr->serialized) {
         for (sel_ptr=CmdPtr->top_after; sel_ptr!=NULL; sel_ptr=next_sel) {
            next_sel = sel_ptr->next;
            free(sel_ptr);
         }
      }
      break;
   case CMD_REPLACE:
      for (sel_ptr=CmdPtr->top_before; sel_ptr!=NULL; sel_ptr=next_sel) {
         next_sel = sel_ptr->next;
         FreeObj(sel_ptr->obj);
         free(sel_ptr);
      }
      if (CmdPtr->serialized) {
         for (sel_ptr=CmdPtr->top_after; sel_ptr!=NULL; sel_ptr=next_sel) {
            next_sel = sel_ptr->next;
            free(sel_ptr);
         }
      }
      break;
   case CMD_ONE_TO_MANY:
      for (sel_ptr=CmdPtr->top_before; sel_ptr!=NULL; sel_ptr=next_sel) {
         next_sel = sel_ptr->next;
         if (!CmdPtr->undone) FreeObj(sel_ptr->obj);
         free(sel_ptr);
      }
      for (sel_ptr=CmdPtr->top_after; sel_ptr!=NULL; sel_ptr=next_sel) {
         next_sel = sel_ptr->next;
         if (CmdPtr->undone) FreeObj(sel_ptr->obj);
         free(sel_ptr);
      }
      break;
   case CMD_MANY_TO_ONE:
      for (sel_ptr=CmdPtr->top_before; sel_ptr!=NULL; sel_ptr=next_sel) {
         next_sel = sel_ptr->next;
         if (!CmdPtr->undone) FreeObj(sel_ptr->obj);
         free(sel_ptr);
      }
      for (sel_ptr=CmdPtr->top_after; sel_ptr!=NULL; sel_ptr=next_sel) {
         next_sel = sel_ptr->next;
         if (CmdPtr->undone) FreeObj(sel_ptr->obj);
         free(sel_ptr);
      }
      break;
   case CMD_COMPOSITE:
      if (percent_start >= 0) {
         for (cmd_ptr=CmdPtr->first; cmd_ptr!=NULL; cmd_ptr=cmd_ptr->next) {
            num_records++;
         }
         if (num_records > 0) {
            inc = (percent_end-percent_start)/((double)num_records);
         }
      }
      for (cmd_ptr=CmdPtr->last; cmd_ptr!=NULL; cmd_ptr=prev_cmd) {
         prev_cmd = cmd_ptr->prev;
         if (percent_start >= 0) {
            DeleteARedoRecord(cmd_ptr, percent_start,
                  min(((double)percent_start+inc),((double)100.0)));
            percent_start += inc;
         } else {
            DeleteARedoRecord(cmd_ptr, percent_start, percent_end);
         }
      }
      break;
   case CMD_CHAT_A_LINE:
      if (CmdPtr->subcmd != NULL) {
         UtilFree(CmdPtr->subcmd->detail.chat.buf);
         free(CmdPtr->subcmd);
      }
      break;
   case CMD_WB_SLIDESHOW:
      if (CmdPtr->subcmd != NULL) free(CmdPtr->subcmd);
      break;
   case CMD_WB_CLEARALL:
      if (CmdPtr->subcmd != NULL) free(CmdPtr->subcmd);
      break;
   }
   if (gstWBInfo.do_whiteboard) {
      if (CmdPtr->sender_process_id != NULL) {
         UtilFree(CmdPtr->sender_process_id);
      }
      if (CmdPtr->serialization_buf != NULL) {
         UtilFree(CmdPtr->serialization_buf);
      }
   }
   free(CmdPtr);
}

void ClearRedoRecords(CmdPtr)
   struct CmdRec *CmdPtr;
{
   struct CmdRec *cmd_ptr=NULL, *prev_cmd=NULL;

   for (cmd_ptr=lastCmd; cmd_ptr != curCmd; cmd_ptr=prev_cmd) {
      prev_cmd = cmd_ptr->prev;
      DeleteARedoRecord(cmd_ptr, (-1.0), (-1.0));
      historyCount--;
   }
   if ((lastCmd=curCmd) == NULL) firstCmd = NULL;
}

void CleanUpCmds()
{
   struct CmdRec *cmd_ptr=NULL, *prev_cmd=NULL;
   int num_records=0;

   for (cmd_ptr=lastCmd; cmd_ptr != NULL; cmd_ptr=cmd_ptr->prev) {
      num_records++;
   }
   if (num_records > 0) {
      double inc=(100.0/((double)num_records)), percent_start=0.0;

      ShowInterrupt(1);
      SaveStatusStrings();
      for (cmd_ptr=lastCmd; cmd_ptr != NULL; cmd_ptr=prev_cmd,
            percent_start += inc) {
         prev_cmd = cmd_ptr->prev;
         DeleteARedoRecord(cmd_ptr, percent_start,
               min(((double)percent_start+inc),((double)100.0)));
      }
      RestoreStatusStrings();
      HideInterrupt();
   }
   firstCmd = lastCmd = curCmd = NULL;
   historyCount = 0;
}

void CopySel(from_top_sel, count, to_top_sel, to_bot_sel)
   struct SelRec *from_top_sel;
   int count;
   struct SelRec **to_top_sel, **to_bot_sel;
{
   struct SelRec *sel_ptr=NULL;

   *to_top_sel = *to_bot_sel = NULL;

   for (sel_ptr=from_top_sel; sel_ptr!=NULL; sel_ptr=sel_ptr->next, count--) {
      AddObjIntoSel(sel_ptr->obj, *to_bot_sel, NULL, to_top_sel, to_bot_sel);
   }
   if (count != 0) {
      FatalUnexpectedError(
            "Count != 0 in CopySel().", /* debug, do not translate */
            TgLoadString(STID_UNDO_REDO_MAY_CAUSE_CRASH));
   }
}

static
void LinkJustTheObjects(TopSel, BotSel)
   struct SelRec *TopSel, *BotSel;
{
   struct SelRec *sel_ptr=NULL, *sel_ptr_next=NULL;

   sel_ptr = TopSel;
   sel_ptr_next = TopSel->next;
   sel_ptr->obj->prev = NULL;
   for ( ; sel_ptr_next!=NULL; sel_ptr=sel_ptr_next,
         sel_ptr_next=sel_ptr->next) {
      sel_ptr->obj->next = sel_ptr_next->obj;
      sel_ptr_next->obj->prev = sel_ptr->obj;
   }
   sel_ptr->obj->next = NULL;
}

static
void UndoNewCmd(CmdPtr)
   struct CmdRec *CmdPtr;
{
   struct SelRec *sel_ptr=NULL;
   struct ObjRec *obj_ptr=NULL, *next_obj=NULL;
   int pos=0, count=0;

   topSel = CmdPtr->top_after;
   botSel = CmdPtr->bot_after;

   sel_ptr = topSel;
   pos = count = 0;
   for (obj_ptr=topObj; obj_ptr != NULL; obj_ptr=next_obj, pos++) {
      next_obj = obj_ptr->next;
      if (pos == CmdPtr->pos_after[count]) {
         count++;
         sel_ptr->obj = obj_ptr;
         UnlinkObj(obj_ptr);
         sel_ptr = sel_ptr->next;
         if (count == CmdPtr->count_after) break;
      }
   }
   LinkJustTheObjects(CmdPtr->top_after, CmdPtr->bot_after);

   UpdSelBBox();
   topSel = botSel = NULL;
   RedrawAnArea(botObj, selLtX-GRID_ABS_SIZE(1), selLtY-GRID_ABS_SIZE(1),
         selRbX+GRID_ABS_SIZE(1), selRbY+GRID_ABS_SIZE(1));
   SetFileModified(TRUE);
   justDupped = FALSE;
}

static
void RedoNewCmd(CmdPtr)
   struct CmdRec *CmdPtr;
{
   struct ObjRec *saved_top_obj, *saved_bot_obj;

   CopySel(CmdPtr->top_after, CmdPtr->count_after, &topSel, &botSel);

   saved_top_obj = topObj;
   saved_bot_obj = botObj;

   curPage->top = topObj = CmdPtr->top_after->obj;
   curPage->bot = botObj = CmdPtr->bot_after->obj;

   AdjSplineVs();
   AdjCaches();

   curPage->top = topObj = saved_top_obj;
   curPage->bot = botObj = saved_bot_obj;

   if (topObj == NULL) {
      curPage->bot = botObj = botSel->obj;
   } else {
      topObj->prev = botSel->obj;
   }
   botSel->obj->next = topObj;
   curPage->top = topObj = topSel->obj;

   UpdSelBBox();
   RedrawAnArea(botObj, selLtX-GRID_ABS_SIZE(1), selLtY-GRID_ABS_SIZE(1),
         selRbX+GRID_ABS_SIZE(1), selRbY+GRID_ABS_SIZE(1));
   if (!deserializingFile) HighLightForward();
   SetFileModified(TRUE);
   justDupped = FALSE;
}

static
int ObjHasFullID(obj_ptr, psz_full_id)
   struct ObjRec *obj_ptr;
   char *psz_full_id;
{
   if (*psz_full_id != '#') {
      if (gstWBInfo.do_whiteboard) { /* debug, do not translate */
         sprintf(gszMsgBox, "%s, psz_full_id = %s, pid = %ld.",
               "Unexpected *psz_full_id != '#' in ObjHasFullID()",
               psz_full_id, gstWBInfo.pid);
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      }
   }
   psz_full_id = strchr(psz_full_id, ':');
   psz_full_id++;
   if (obj_ptr->creator_full_id == NULL) {
      char buf[MAXSTRING];

      sprintf(buf, "%1d/%s", obj_ptr->id, gszLocalPID);
      return (strcmp(buf, psz_full_id) == 0);
   }
   return (strcmp(obj_ptr->creator_full_id, psz_full_id) == 0);
}

static
int GetPositionFromFullID(psz_full_id)
   char *psz_full_id;
{
   int pos_to_match=INVALID;
   char *psz=NULL;

   if (*psz_full_id != '#') {
      if (gstWBInfo.do_whiteboard) { /* debug, do not translate */
         sprintf(gszMsgBox, "%s, psz_full_id = %s, pid = %ld.",
               "Unexpected *psz_full_id != '#' in GetPositionFromFullID()",
               psz_full_id, gstWBInfo.pid);
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      }
   }
   psz = strchr(++psz_full_id, ':');
   *psz = '\0';
   if (sscanf(psz_full_id, "%d", &pos_to_match) != 1 ||
         pos_to_match == INVALID) {
      if (gstWBInfo.do_whiteboard) { /* debug, do not translate */
         sprintf(gszMsgBox, "%s %s (%s), psz_full_id = %s, pid = %ld.",
               "Format error in parsing pos_to_match in",
               "GetPositionFromFullID()", "cannot get pos_to_match",
               psz_full_id, gstWBInfo.pid);
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      }
   }
   *psz = ':';

   return pos_to_match;
}

static
void UndoDeleteCmd(CmdPtr)
   struct CmdRec *CmdPtr;
{
   struct SelRec *sel_ptr=NULL;
   struct ObjRec *obj_ptr=NULL, *next_obj=NULL;
   struct ObjRec *saved_top_obj=NULL, *saved_bot_obj=NULL;
   int count=0, pos=0, pos_to_match=INVALID;

   CopySel(CmdPtr->top_before, CmdPtr->count_before, &topSel, &botSel);

   saved_top_obj = topObj;
   saved_bot_obj = botObj;

   LinkJustTheObjects(CmdPtr->top_before, CmdPtr->bot_before);

   curPage->top = topObj = CmdPtr->top_before->obj;
   curPage->bot = botObj = CmdPtr->bot_before->obj;

   AdjSplineVs();
   AdjCaches();

   curPage->top = topObj = saved_top_obj;
   curPage->bot = botObj = saved_bot_obj;

   count = 0;
   sel_ptr = topSel;

   if (CmdPtr->pos_before_has_ids) {
      pos_to_match = GetPositionFromFullID(
            (char*)(long)(CmdPtr->pos_before[count]));
   } else {
      pos_to_match = CmdPtr->pos_before[count];
   }
   for (obj_ptr=topObj; obj_ptr != NULL; obj_ptr=next_obj, pos++) {
      if (pos == pos_to_match) {
         AddObj(obj_ptr->prev, obj_ptr, sel_ptr->obj);
         next_obj = obj_ptr;
         count++;
         sel_ptr = sel_ptr->next;
         if (count == CmdPtr->count_before) break;

         if (CmdPtr->pos_before_has_ids) {
            pos_to_match = GetPositionFromFullID(
                  (char*)(long)(CmdPtr->pos_before[count]));
         } else {
            pos_to_match = CmdPtr->pos_before[count];
         }
      } else {
         next_obj = obj_ptr->next;
      }
   }
   for ( ; sel_ptr != NULL; sel_ptr=sel_ptr->next) {
      AddObj(botObj, NULL, sel_ptr->obj);
   }
   UpdSelBBox();
   RedrawAnArea(botObj, selLtX-GRID_ABS_SIZE(1), selLtY-GRID_ABS_SIZE(1),
         selRbX+GRID_ABS_SIZE(1), selRbY+GRID_ABS_SIZE(1));
   if (!deserializingFile) HighLightForward();
   SetFileModified(TRUE);
   justDupped = FALSE;
}

static
int SetupBeforePointers(CmdPtr, modify_before_pointers, pp_actual_pos_before)
   struct CmdRec *CmdPtr;
   int modify_before_pointers, **pp_actual_pos_before;
   /*
    * CmdPtr->pos_before is an array of strings containing
    *    "#<pos>:<oid>/<pid>:<hostname>".
    * CmdPtr->count_before is assumed to be the number of elements in
    *    CmdPtr->pos_before.
    *
    * if (modify_before_pointers) {
    *    Setup CmdPtr->top_before and CmdPtr->bot_before.
    *    For these cases, <oid>/<pid>:<hostname> is used to locate the
    *          object positions to build the actual before pointers.
    * } else {
    *    This can only happen if (CmdPtr->serialized &&
    *          CmdPtr->type == CMD_REPLACE && !CmdPtr->undone)
    *    In this case, the #<pos> should be used to setup up the actual
    *          before pointers.
    * }
    * if (pp_actual_pos_before != NULL) {
    *    Create an array of integers containing the positions of the
    *    objects referenced in CmdPtr->pos_before and return this array.
    * }
    */
{
   int index, ok=TRUE, cur_pos=0;
   struct ObjRec *obj_ptr=topObj;
   struct SelRec *sel_ptr=NULL, *next_sel=NULL, *top_sel=NULL, *bot_sel=NULL;

   if (modify_before_pointers) {
      for (sel_ptr=CmdPtr->top_before; sel_ptr != NULL; sel_ptr=next_sel) {
         next_sel = sel_ptr->next;
         free(sel_ptr);
      }
      CmdPtr->top_before = CmdPtr->bot_before = NULL;
   }
   if (pp_actual_pos_before != NULL) {
      *pp_actual_pos_before = (int*)malloc(CmdPtr->count_before*sizeof(int));
      if (*pp_actual_pos_before == NULL) FailAllocMessage();
      memset(*pp_actual_pos_before, 0, CmdPtr->count_before*sizeof(int));
   }
   for (index=0; index < CmdPtr->count_before; index++) {
      char *psz_full_id=(((char**)CmdPtr->pos_before)[index]), *psz=NULL;
      char pid_and_host[MAXSTRING];
      int id=(-1), found=FALSE, pos_to_match=INVALID, obj_is_tgifobj=FALSE;

      if (*psz_full_id == '#') {
         psz = strchr(++psz_full_id, ':');
         *psz = '\0';
         if (!modify_before_pointers) {
            if (sscanf(psz_full_id, "%d", &pos_to_match) != 1 ||
                  pos_to_match == INVALID) {
               if (gstWBInfo.do_whiteboard) { /* debug, do not translate */
                  sprintf(gszMsgBox, "%s %s (%s), psz_full_id = %s, pid = %ld.",
                        "Format error in parsing pos_to_match in",
                        "SetupBeforePointers()", "cannot get pos_to_match",
                        psz_full_id, gstWBInfo.pid);
                  MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
               }
            }
         }
         *psz++ = ':';
         psz_full_id = psz;
      }
      psz = strchr(psz_full_id, '/');
      *pid_and_host = '\0';
      if (psz == NULL) {
         if (gstWBInfo.do_whiteboard) { /* debug, do not translate */
            sprintf(gszMsgBox, "%s (%s), psz_full_id = %s, pid = %ld.",
                  "Format error in parsing id in SetupBeforePointers()",
                  "cannot find ':'", psz_full_id, gstWBInfo.pid);
            MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
         }
         ok = FALSE;
         break;
      } else {
         *psz = '\0';
         if (sscanf(psz_full_id, "%d", &id) == 1) {
            strcpy(pid_and_host, &psz[1]);
            if (id == INVALID) {
               obj_is_tgifobj = TRUE;
            }
         } else {
            if (gstWBInfo.do_whiteboard) { /* debug, do not translate */
               sprintf(gszMsgBox, "%s (%s), psz_full_id = %s, pid = %ld.",
                     "Format error in parsing id in SetupBeforePointers()",
                     "cannot find id", psz_full_id, gstWBInfo.pid);
               MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
            }
            ok = FALSE;
            break;
         }
         *psz = '/';
      }
      if (ok) {
         for (obj_ptr=topObj, cur_pos=0 ; ok && obj_ptr != NULL;
               obj_ptr=obj_ptr->next, cur_pos++) {
            if (pos_to_match == INVALID) {
               if (obj_ptr->creator_full_id == NULL) {
                  if (obj_ptr->id == id &&
                        strcmp(pid_and_host, gszLocalPID) == 0) {
                     found = TRUE;
                  } else if (obj_ptr->id == INVALID &&
                        CmdPtr->include_tgif_obj && obj_is_tgifobj) {
                     found = TRUE;
                  }
               } else {
                  if (strcmp(obj_ptr->creator_full_id, psz_full_id) == 0) {
                     found = TRUE;
                  }
               }
            } else if (pos_to_match == cur_pos) {
               found = TRUE;
            }
            if (found) {
               if (modify_before_pointers) {
                  AddObjIntoSel(obj_ptr, bot_sel, NULL, &top_sel, &bot_sel);
               }
               if (pp_actual_pos_before != NULL) {
                  (*pp_actual_pos_before)[index] = cur_pos;
               }
               obj_ptr = obj_ptr->next;
               cur_pos++;
               break;
            }
         }
      }
      if (ok && !found) {
         if (gstWBInfo.do_whiteboard) { /* debug, do not translate */
#ifdef _TGIF_DBG_WB2 /* debug, do not translate */
            fprintf(stderr, "%s: '%s' %s, pid = %ld.\n\t(%s=%1d, %s='%s')\n",
                  "@*@ Cannot find object id", psz_full_id,
                  "in SetupBeforePointers()", gstWBInfo.pid, "id", id,
                  "pid_and_host", pid_and_host);
#endif /* _TGIF_DBG_WB2 */
         }
         ok = FALSE;
         break;
      }
   }
   if (ok && index != CmdPtr->count_before) {
      if (gstWBInfo.do_whiteboard) { /* debug, do not translate */
         sprintf(gszMsgBox, "%s (%1d) %s (%1d) %s, pid = %ld.",
               "Index", index, "!= CmdPtr->count_before", CmdPtr->count_before,
               "in SetupBeforePointers()", gstWBInfo.pid);
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      }
      ok = FALSE;
   }
   if (!ok) {
      for (sel_ptr=top_sel; sel_ptr != NULL; sel_ptr=next_sel) {
         next_sel = sel_ptr->next;
         free(sel_ptr);
      }
      if (pp_actual_pos_before != NULL) {
         free(*pp_actual_pos_before);
         *pp_actual_pos_before = NULL;
      }
   } else {
      if (modify_before_pointers) {
         CmdPtr->top_before = top_sel;
         CmdPtr->bot_before = bot_sel;
      }
   }
   return ok;
}

static
int RedoDeleteCmd(CmdPtr)
   struct CmdRec *CmdPtr;
{
   struct SelRec *sel_ptr;

   if (CmdPtr->serialized) {
      if (CmdPtr->include_tgif_obj) {
         AddObj(NULL, topObj, tgifObj);
      }
      if (!SetupBeforePointers(CmdPtr, TRUE, NULL)) {
         if (CmdPtr->include_tgif_obj) {
            UnlinkObj(topObj);
         }
         return FALSE;
      }
      if (CmdPtr->include_tgif_obj) {
         UnlinkObj(topObj);
      }
   }
   topSel = CmdPtr->top_before;
   botSel = CmdPtr->bot_before;

   sel_ptr = topSel;
   if (CmdPtr->serialized) {
      for ( ; sel_ptr != NULL; sel_ptr=sel_ptr->next) {
         UnlinkObj(sel_ptr->obj);
      }
   } else {
      struct ObjRec *obj_ptr=NULL, *next_obj=NULL;
      int count=0;

      if (CmdPtr->pos_before_has_ids) {
         for (obj_ptr=topObj; obj_ptr != NULL; obj_ptr=next_obj) {
            next_obj = obj_ptr->next;
            if (ObjHasFullID(obj_ptr,
                  (char*)(long)(CmdPtr->pos_before[count]))) {
               count++;
               sel_ptr->obj = obj_ptr;
               UnlinkObj(obj_ptr);
               sel_ptr = sel_ptr->next;
               if (count == CmdPtr->count_before) break;
            }
         }
      } else {
         int pos=0;

         for (obj_ptr=topObj; obj_ptr != NULL; obj_ptr=next_obj, pos++) {
            next_obj = obj_ptr->next;
            if (pos == CmdPtr->pos_before[count]) {
               count++;
               sel_ptr->obj = obj_ptr;
               UnlinkObj(obj_ptr);
               sel_ptr = sel_ptr->next;
               if (count == CmdPtr->count_before) break;
            }
         }
      }
   }
   LinkJustTheObjects(CmdPtr->top_before, CmdPtr->bot_before);

   UpdSelBBox();
   topSel = botSel = NULL;
   RedrawAnArea(botObj, selLtX-GRID_ABS_SIZE(1), selLtY-GRID_ABS_SIZE(1),
         selRbX+GRID_ABS_SIZE(1), selRbY+GRID_ABS_SIZE(1));
   SetFileModified(TRUE);
   justDupped = FALSE;
   return TRUE;
}

static
int UndoOrRedoMoveCmd(CmdPtr)
   struct CmdRec *CmdPtr;
{
   struct SelRec *sel_ptr;
   struct ObjRec *obj_ptr;
   int dx, dy;

   if (CmdPtr->serialized) {
      if (CmdPtr->include_tgif_obj) {
         AddObj(NULL, topObj, tgifObj);
      }
      if (!SetupBeforePointers(CmdPtr, TRUE, NULL)) {
         if (CmdPtr->include_tgif_obj) {
            UnlinkObj(topObj);
         }
         return FALSE;
      }
      if (CmdPtr->include_tgif_obj) {
         UnlinkObj(topObj);
      }
   }
   dx = (CmdPtr->undone) ? CmdPtr->subcmd->detail.move.dx :
           -(CmdPtr->subcmd->detail.move.dx);
   dy = (CmdPtr->undone) ? CmdPtr->subcmd->detail.move.dy :
           -(CmdPtr->subcmd->detail.move.dy);

   CopySel(CmdPtr->top_before, CmdPtr->count_before, &topSel, &botSel);

   sel_ptr = topSel;
   if (CmdPtr->serialized) {
      for ( ; sel_ptr != NULL; sel_ptr=sel_ptr->next) {
         obj_ptr = sel_ptr->obj;
         if (!obj_ptr->locked) MoveObj(obj_ptr, dx, dy);
      }
   } else {
      struct ObjRec *next_obj;
      int count=0;

      if (CmdPtr->pos_before_has_ids) {
         for (obj_ptr=topObj; obj_ptr != NULL; obj_ptr=next_obj) {
            next_obj = obj_ptr->next;
            if (ObjHasFullID(obj_ptr,
                  (char*)(long)(CmdPtr->pos_before[count]))) {
               count++;
               sel_ptr->obj = obj_ptr;
               if (!obj_ptr->locked) MoveObj(obj_ptr, dx, dy);
               sel_ptr = sel_ptr->next;
               if (count == CmdPtr->count_before) break;
            }
         }
      } else {
         int pos=0;

         for (obj_ptr=topObj; obj_ptr != NULL; obj_ptr=next_obj, pos++) {
            next_obj = obj_ptr->next;
            if (pos == CmdPtr->pos_before[count]) {
               count++;
               sel_ptr->obj = obj_ptr;
               if (!obj_ptr->locked) MoveObj(obj_ptr, dx, dy);
               sel_ptr = sel_ptr->next;
               if (count == CmdPtr->count_before) break;
            }
         }
      }
   }
   UpdSelBBox();
   RedrawAreas(botObj, selLtX-GRID_ABS_SIZE(1)-dx, selLtY-GRID_ABS_SIZE(1)-dy,
         selRbX+GRID_ABS_SIZE(1)-dx, selRbY+GRID_ABS_SIZE(1)-dy,
         selLtX-GRID_ABS_SIZE(1), selLtY-GRID_ABS_SIZE(1),
         selRbX+GRID_ABS_SIZE(1), selRbY+GRID_ABS_SIZE(1));
   if (!deserializingFile) HighLightForward();
   SetFileModified(TRUE);
   justDupped = FALSE;
   return TRUE;
}

static
void UpdateXPmObjectsInACmd(CmdPtr)
   struct CmdRec *CmdPtr;
{
   struct CmdRec *cmd_ptr;
   struct SelRec *sel_ptr;

   switch (CmdPtr->type) {
   case CMD_COMPOSITE:
      for (cmd_ptr=CmdPtr->first; cmd_ptr!=NULL; cmd_ptr=cmd_ptr->next) {
         UpdateXPmObjectsInACmd(cmd_ptr);
      }
      break;
   case CMD_NEW:
      for (sel_ptr=CmdPtr->top_after; sel_ptr!=NULL; sel_ptr=sel_ptr->next) {
         UpdateXPmObjects(sel_ptr->obj);
      }
      break;
   case CMD_DELETE: break;
   case CMD_MOVE: break;
   case CMD_STRETCH: break;
   case CMD_REPLACE:
      for (sel_ptr=CmdPtr->top_after; sel_ptr!=NULL; sel_ptr=sel_ptr->next) {
         UpdateXPmObjects(sel_ptr->obj);
      }
      break;
   case CMD_ONE_TO_MANY: break;
   case CMD_MANY_TO_ONE:
      for (sel_ptr=CmdPtr->top_before; sel_ptr!=NULL; sel_ptr=sel_ptr->next) {
         UpdateXPmObjects(sel_ptr->obj);
      }
      for (sel_ptr=CmdPtr->top_after; sel_ptr!=NULL; sel_ptr=sel_ptr->next) {
         UpdateXPmObjects(sel_ptr->obj);
      }
      break;
   case CMD_GOTO_PAGE: break;
   }
}

static
int UpdatePixelInACmd(CmdPtr)
   struct CmdRec *CmdPtr;
{
   struct CmdRec *cmd_ptr;
   struct SelRec *sel_ptr;
   int changed=FALSE;

   switch (CmdPtr->type) {
   case CMD_COMPOSITE:
      for (cmd_ptr=CmdPtr->first; cmd_ptr!=NULL; cmd_ptr=cmd_ptr->next) {
         if (UpdatePixelInACmd(cmd_ptr)) {
            changed = TRUE;
         }
      }
      break;
   case CMD_NEW:
      for (sel_ptr=CmdPtr->top_after; sel_ptr!=NULL; sel_ptr=sel_ptr->next) {
         if (UpdatePixel(sel_ptr->obj)) {
            changed = TRUE;
         }
      }
      break;
   case CMD_DELETE: break;
   case CMD_MOVE: break;
   case CMD_STRETCH: break;
   case CMD_REPLACE:
      for (sel_ptr=CmdPtr->top_after; sel_ptr!=NULL; sel_ptr=sel_ptr->next) {
         if (UpdatePixel(sel_ptr->obj)) {
            changed = TRUE;
         }
      }
      break;
   case CMD_ONE_TO_MANY: break;
   case CMD_MANY_TO_ONE:
      for (sel_ptr=CmdPtr->top_before; sel_ptr!=NULL; sel_ptr=sel_ptr->next) {
         if (UpdatePixel(sel_ptr->obj)) {
            changed = TRUE;
         }
      }
      for (sel_ptr=CmdPtr->top_after; sel_ptr!=NULL; sel_ptr=sel_ptr->next) {
         if (UpdatePixel(sel_ptr->obj)) {
            changed = TRUE;
         }
      }
      break;
   case CMD_GOTO_PAGE: break;
   }
   return changed;
}

static
int RefreshColormap(nRedoing, CmdPtr)
   int nRedoing;
   struct CmdRec *CmdPtr;
   /*
    * This function mimics FlushColormap() in color.c.
    */
{
   int changed=FALSE, saved_color_layers;
   struct ObjRec *obj_ptr=NULL;
   struct PageRec *page_ptr=NULL;
   Colormap colormap=XCopyColormapAndFree(mainDisplay, mainColormap);

   mainColormap = colormap;
   newColormapUsed = TRUE;
   XSetWindowColormap(mainDisplay, mainWindow, mainColormap);

   if (nRedoing) {
      /* need to scan forward to pick up objects */
      struct CmdRec *cmd_ptr;

      for (page_ptr=firstPage; page_ptr != NULL; page_ptr=page_ptr->next) {
         for (obj_ptr=page_ptr->bot; obj_ptr!=NULL; obj_ptr=obj_ptr->prev) {
            UpdateXPmObjects(obj_ptr);
         }
      }
      for (cmd_ptr=CmdPtr->next; cmd_ptr != NULL; cmd_ptr=cmd_ptr->next) {
         UpdateXPmObjectsInACmd(cmd_ptr);
      }
   } else {
      for (page_ptr=firstPage; page_ptr != NULL; page_ptr=page_ptr->next) {
         for (obj_ptr=page_ptr->bot; obj_ptr!=NULL; obj_ptr=obj_ptr->prev) {
            UpdateXPmObjects(obj_ptr);
         }
      }
   }
   initColorDontReload = TRUE;
   CleanUpColors();
   XFreeColormap(mainDisplay, mainColormap);
   mainColormap = DefaultColormap(mainDisplay, mainScreen);
   XSetWindowColormap(mainDisplay, mainWindow, mainColormap);
   newColormapUsed = FALSE;
   saved_color_layers = colorLayers;
   InitColor();
   initColorDontReload = FALSE;
   colorLayers = saved_color_layers;

   ShowColor(TRUE);

   SaveStatusStrings();
   gnUpdatePixelObjCount = 0;
   if (nRedoing) {
      /* need to scan forward to pick up objects */
      struct CmdRec *cmd_ptr;

      for (page_ptr=firstPage; page_ptr != NULL; page_ptr=page_ptr->next) {
         for (obj_ptr=page_ptr->bot; obj_ptr!=NULL; obj_ptr=obj_ptr->prev) {
            if (UpdatePixel(obj_ptr)) {
               changed = TRUE;
            }
         }
      }
      for (cmd_ptr=CmdPtr->next; cmd_ptr != NULL; cmd_ptr=cmd_ptr->next) {
         if (UpdatePixelInACmd(cmd_ptr)) {
            changed = TRUE;
         }
      }
   } else {
      for (page_ptr=firstPage; page_ptr != NULL; page_ptr=page_ptr->next) {
         for (obj_ptr=page_ptr->bot; obj_ptr!=NULL; obj_ptr=obj_ptr->prev) {
            if (UpdatePixel(obj_ptr)) {
               changed = TRUE;
            }
         }
      }
   }
   RestoreStatusStrings();

   DestroyPinnedMenu(MENU_COLOR);
   if (colorLayers) {
      RedrawColorWindow();
   }
   return changed;
}

static
int UndoOrRedoReplaceCmd(CmdPtr, HighLightSingleObj)
   struct CmdRec *CmdPtr;
   int HighLightSingleObj;
{
   struct ObjRec *obj_ptr=NULL, *next_obj=NULL;
   struct SelRec *sel_ptr=NULL;
   struct SelRec *saved_top_sel=NULL, *saved_bot_sel=NULL;
   struct ObjRec *saved_top_obj=NULL, *saved_bot_obj=NULL;
   int ltx, lty, rbx, rby, pos, count, *pos_table, max_count;
   int need_clear_and_redraw=FALSE;
   int *actual_pos_before=NULL;

#ifdef _TGIF_DBG_WB2 /* debug, do not translate */
   if (gstWBInfo.do_whiteboard) {
      fprintf(stderr, "@@@ Beginning of UndoOrRedoReplaceCmd()...\n");
      fprintf(stderr, "\tCmdPtr->serialized=%1d...\n", CmdPtr->serialized);
   }
#endif /* _TGIF_DBG_WB2 */
   if (CmdPtr->serialized) {
      int shuffle=FALSE;

      if (CmdPtr->first_redo_after_deserialize && CmdPtr->undone) {
         CmdPtr->first_redo_after_deserialize = FALSE;
         shuffle = TRUE;
      }
#ifdef _TGIF_DBG_WB2 /* debug, do not translate */
      if (gstWBInfo.do_whiteboard) {
         fprintf(stderr, "In UndoOrRedoReplaceCmd(), shuffle=%1d.\n", shuffle);
      }
#endif /* _TGIF_DBG_WB2 */
      if (CmdPtr->include_tgif_obj) {
         AddObj(NULL, topObj, tgifObj);
      }
      if (shuffle) {
         if (!SetupBeforePointers(CmdPtr, TRUE, NULL)) {
            if (CmdPtr->include_tgif_obj) {
               UnlinkObj(topObj);
            }
            return FALSE;
         }
         topSel = CmdPtr->top_before;
         botSel = CmdPtr->bot_before;
         CmdPtr->top_before = CmdPtr->top_after;
         CmdPtr->bot_before = CmdPtr->bot_after;
         CmdPtr->top_after = topSel;
         CmdPtr->bot_after = botSel;
         topSel = botSel = NULL;
      } else {
         if (CmdPtr->undone) {
            struct SelRec *next_sel=NULL;

            for (sel_ptr=CmdPtr->top_after; sel_ptr != NULL;
                  sel_ptr=next_sel) {
               next_sel = sel_ptr->next;
               free(sel_ptr);
            }
            CmdPtr->top_after = CmdPtr->bot_after = NULL;

            saved_top_sel = CmdPtr->top_before;
            saved_bot_sel = CmdPtr->bot_before;
            CmdPtr->top_before = CmdPtr->bot_before = NULL;
            if (!SetupBeforePointers(CmdPtr, TRUE, NULL)) {
               if (CmdPtr->include_tgif_obj) {
                  UnlinkObj(topObj);
               }
               return FALSE;
            }
            CmdPtr->top_after = CmdPtr->top_before;
            CmdPtr->bot_after = CmdPtr->bot_before;
            CmdPtr->top_before = saved_top_sel;
            CmdPtr->bot_before = saved_bot_sel;
         } else {
            if (!SetupBeforePointers(CmdPtr, FALSE, &actual_pos_before)) {
               if (CmdPtr->include_tgif_obj) {
                  UnlinkObj(topObj);
               }
               return FALSE;
            }
         }
      }
      if (CmdPtr->include_tgif_obj) {
         UnlinkObj(topObj);
      }
   }
   LinkJustTheObjects(CmdPtr->top_before, CmdPtr->bot_before);

   CopySel(CmdPtr->top_before, CmdPtr->count_before, &topSel, &botSel);

   if (CmdPtr->include_tgif_obj) {
      AddObj(NULL, topObj, tgifObj);
   }
   pos_table = (CmdPtr->undone) ? CmdPtr->pos_before : CmdPtr->pos_after;
   max_count = (CmdPtr->undone) ? CmdPtr->count_before : CmdPtr->count_after;

   sel_ptr = topSel;
   if (CmdPtr->serialized && CmdPtr->undone) {
      struct SelRec *sel_ptr1;

      for (sel_ptr1=CmdPtr->top_after; sel_ptr1 != NULL;
            sel_ptr1=sel_ptr1->next) {
         sel_ptr->obj = sel_ptr1->obj;
         UnlinkObj(sel_ptr1->obj);
         sel_ptr = sel_ptr->next;
      }
   } else {
      if (CmdPtr->serialized && !CmdPtr->undone) {
         pos_table = actual_pos_before;
      }
      pos = count = 0;
      for (obj_ptr=topObj; obj_ptr!=NULL; obj_ptr=next_obj, pos++) {
         next_obj = obj_ptr->next;
         if (pos == pos_table[count]) {
            count++;
            sel_ptr->obj = obj_ptr;
            UnlinkObj(obj_ptr);
            sel_ptr = sel_ptr->next;
            if (count == max_count) break;
         }
      }
   }
   UpdSelBBox();
   ltx = selLtX; lty = selLtY; rbx = selRbX; rby = selRbY;

   saved_top_sel = topSel;
   saved_bot_sel = botSel;
   topSel = CmdPtr->top_before;
   botSel = CmdPtr->bot_before;
   CmdPtr->top_before = saved_top_sel;
   CmdPtr->bot_before = saved_bot_sel;

   saved_top_obj = topObj;
   saved_bot_obj = botObj;

   curPage->top = topObj = topSel->obj;
   curPage->bot = botObj = botSel->obj;

   AdjSplineVs();
   AdjCaches();

   curPage->top = topObj = saved_top_obj;
   curPage->bot = botObj = saved_bot_obj;

   pos_table = (CmdPtr->undone) ? CmdPtr->pos_after : CmdPtr->pos_before;
   max_count = (CmdPtr->undone) ? CmdPtr->count_after : CmdPtr->count_before;

   pos = count = 0;
   sel_ptr = topSel;
   if (CmdPtr->serialized && !CmdPtr->undone) {
      pos_table = actual_pos_before;
   }
   for (obj_ptr=topObj; obj_ptr!=NULL; obj_ptr=next_obj, pos++) {
      if (pos == pos_table[count]) {
         AddObj(obj_ptr->prev, obj_ptr, sel_ptr->obj);
         next_obj = obj_ptr;
         count++;
         sel_ptr = sel_ptr->next;
         if (count == max_count) break;
      } else {
         next_obj = obj_ptr->next;
      }
   }
   for ( ; sel_ptr != NULL; sel_ptr=sel_ptr->next) {
      AddObj(botObj, NULL, sel_ptr->obj);
   }
   if (CmdPtr->include_tgif_obj) {
      tgifObj = topObj;
      UnlinkObj(topObj);

      sel_ptr = topSel;
      topSel = topSel->next;
      if (topSel == NULL) {
         botSel = NULL;
      } else {
         topSel->prev = NULL;
      }
      free(sel_ptr);
   }
   if (CmdPtr->new_colormap) {
      if (RefreshColormap(CmdPtr->undone, CmdPtr)) {
         need_clear_and_redraw = TRUE;
      }
   }
   UpdSelBBox();
   if (need_clear_and_redraw) {
      ClearAndRedrawDrawWindow();
   } else {
      RedrawAreas(botObj, ltx-GRID_ABS_SIZE(1), lty-GRID_ABS_SIZE(1),
            rbx+GRID_ABS_SIZE(1), rby+GRID_ABS_SIZE(1),
            selLtX-GRID_ABS_SIZE(1), selLtY-GRID_ABS_SIZE(1),
            selRbX+GRID_ABS_SIZE(1), selRbY+GRID_ABS_SIZE(1));
   }
   if (HighLightSingleObj && topSel != NULL) {
      if (!need_clear_and_redraw) {
         if (!deserializingFile) HighLightForward();
      }
   } else {
      RemoveAllSel();
   }
   SetFileModified(TRUE);
   justDupped = FALSE;

   if (actual_pos_before != NULL) free(actual_pos_before);

   return TRUE;
}

static
int UndoOrRedoOneToManyCmd(CmdPtr)
   struct CmdRec *CmdPtr;
{
   struct ObjRec *obj_ptr, *next_obj;
   struct SelRec *sel_ptr;
   struct ObjRec *saved_top_obj, *saved_bot_obj;
   int ltx, lty, rbx, rby, pos, count, *pos_table, max_count;
   int need_clear_and_redraw=FALSE;
   int *actual_pos_before=NULL;

   if (CmdPtr->serialized) {
      if (CmdPtr->include_tgif_obj) {
         AddObj(NULL, topObj, tgifObj);
      }
      if (CmdPtr->undone) {
         if (!SetupBeforePointers(CmdPtr, TRUE, &actual_pos_before)) {
            if (CmdPtr->include_tgif_obj) {
               UnlinkObj(topObj);
            }
            return FALSE;
         }
      }
      if (CmdPtr->include_tgif_obj) {
         UnlinkObj(topObj);
      }
   }
   if (CmdPtr->include_tgif_obj) {
      AddObj(NULL, topObj, tgifObj);
      AddNewSelObj(topObj);
   }

   sel_ptr = (CmdPtr->undone) ? CmdPtr->top_before : CmdPtr->top_after;
   if (CmdPtr->serialized && CmdPtr->undone) {
      for ( ; sel_ptr != NULL; sel_ptr=sel_ptr->next) {
         UnlinkObj(sel_ptr->obj);
      }
   } else {
      pos_table = (CmdPtr->undone) ? CmdPtr->pos_before : CmdPtr->pos_after;
      max_count = (CmdPtr->undone) ? CmdPtr->count_before : CmdPtr->count_after;

      if (CmdPtr->serialized && CmdPtr->undone) {
         pos_table = actual_pos_before;
      }
      pos = count = 0;
      for (obj_ptr=topObj; obj_ptr!=NULL; obj_ptr=next_obj, pos++) {
         next_obj = obj_ptr->next;
         if (pos == pos_table[count]) {
            count++;
            sel_ptr->obj = obj_ptr;
            UnlinkObj(obj_ptr);
            sel_ptr = sel_ptr->next;
            if (count == max_count) break;
         }
      }
   }
   topSel = (CmdPtr->undone) ? CmdPtr->top_before : CmdPtr->top_after;
   botSel = (CmdPtr->undone) ? CmdPtr->bot_before : CmdPtr->bot_after;
   UpdSelBBox();
   ltx = selLtX; lty = selLtY; rbx = selRbX; rby = selRbY;

   if (CmdPtr->undone) {
      CopySel(CmdPtr->top_after, CmdPtr->count_after, &topSel, &botSel);
   } else {
      CopySel(CmdPtr->top_before, CmdPtr->count_before, &topSel, &botSel);
   }
   LinkJustTheObjects(topSel, botSel);

   saved_top_obj = topObj;
   saved_bot_obj = botObj;

   curPage->top = topObj = topSel->obj;
   curPage->bot = botObj = botSel->obj;

   AdjSplineVs();
   AdjCaches();

   curPage->top = topObj = saved_top_obj;
   curPage->bot = botObj = saved_bot_obj;

   pos_table = (CmdPtr->undone) ? CmdPtr->pos_after : CmdPtr->pos_before;
   max_count = (CmdPtr->undone) ? CmdPtr->count_after : CmdPtr->count_before;

   pos = count = 0;
   sel_ptr = topSel;
   if (CmdPtr->serialized && CmdPtr->undone) {
      pos_table = actual_pos_before;
   }
   for (obj_ptr=topObj; obj_ptr!=NULL; obj_ptr=next_obj, pos++) {
      if (pos == pos_table[count]) {
         AddObj(obj_ptr->prev, obj_ptr, sel_ptr->obj);
         next_obj = obj_ptr;
         count++;
         sel_ptr = sel_ptr->next;
         if (count == max_count) break;
      } else {
         next_obj = obj_ptr->next;
      }
   }
   for ( ; sel_ptr != NULL; sel_ptr=sel_ptr->next) {
      AddObj(botObj, NULL, sel_ptr->obj);
   }
   if (CmdPtr->include_tgif_obj) {
      tgifObj = topObj;
      UnlinkObj(topObj);

      sel_ptr = topSel;
      topSel = topSel->next;
      if (topSel == NULL) {
         botSel = NULL;
      } else {
         topSel->prev = NULL;
      }
      free(sel_ptr);
   }
   if (CmdPtr->new_colormap) {
      if (RefreshColormap(CmdPtr->undone, CmdPtr)) {
         need_clear_and_redraw = TRUE;
      }
   }
   UpdSelBBox();
   if (need_clear_and_redraw) {
      ClearAndRedrawDrawWindow();
   } else {
      RedrawAreas(botObj, selLtX-GRID_ABS_SIZE(1), selLtY-GRID_ABS_SIZE(1),
            selRbX+GRID_ABS_SIZE(1), selRbY+GRID_ABS_SIZE(1),
            ltx-GRID_ABS_SIZE(1), lty-GRID_ABS_SIZE(1),
            rbx+GRID_ABS_SIZE(1), rby+GRID_ABS_SIZE(1));
   }
   if (!need_clear_and_redraw) {
      if (!deserializingFile) HighLightForward();
   }
   SetFileModified(TRUE);
   justDupped = FALSE;

   if (actual_pos_before != NULL) free(actual_pos_before);

   return TRUE;
}

static
void UndoOrRedoGotoPageCmd(CmdPtr)
   struct CmdRec *CmdPtr;
{
   int new_page_num;

   new_page_num = (CmdPtr->undone) ? CmdPtr->count_after : CmdPtr->count_before;

   GotoPageNum(new_page_num);
   ClearAndRedrawDrawWindow();
   ShowPage();
}

static
void UndoOrRedoWBSlideShow(CmdPtr)
   struct CmdRec *CmdPtr;
{
   int into_slideshow=CmdPtr->subcmd->detail.slideshow.into_slideshow;

   if (CmdPtr->undone) {
      /* executing the command */
      if (!inSlideShow && into_slideshow) {
         EnterSlideShow();
      } else if (inSlideShow && !into_slideshow) {
         LeaveSlideShow();
      }
   }
}

typedef struct tagUndoRedoRestoreDrawingModeRec {
   int cur_choice;
   int text_cursor_shown;
   int editing_text;
   int cur_text_modified;
   int text_orig_x;
   int text_orig_baseline_y;
} UndoRedoRestoreDrawingModeInfo;

static
void SetUndoRedoRestoreDrawingModeInfo(p_urrdmi, cur_choice, text_cursor_shown,
      editing_text, cur_text_modified, text_orig_x, text_orig_baseline_y)
   UndoRedoRestoreDrawingModeInfo *p_urrdmi;
   int cur_choice, text_cursor_shown, editing_text, cur_text_modified;
   int text_orig_x, text_orig_baseline_y;
{
   p_urrdmi->cur_choice = cur_choice;
   p_urrdmi->text_cursor_shown = text_cursor_shown;
   p_urrdmi->editing_text = editing_text;
   p_urrdmi->cur_text_modified = cur_text_modified;
   p_urrdmi->text_orig_x = text_orig_x;
   p_urrdmi->text_orig_baseline_y = text_orig_baseline_y;
}

static
void UndoRedoRestoreDrawingMode(p_urrdmi)
   UndoRedoRestoreDrawingModeInfo *p_urrdmi;
{
   if (inHyperSpace || !undoRedoRestoreDrawingMode) return;

   switch (p_urrdmi->cur_choice) {
   case NOTHING: break;
   case DRAWTEXT:
      if (p_urrdmi->text_cursor_shown) {
         if (!p_urrdmi->cur_text_modified) {
            /* just leave things in text mode */
            SetCurChoice(p_urrdmi->cur_choice);
         } else if (p_urrdmi->editing_text) {
            /*
             * editing text should be undone, but the cursor should be
             * placed back to where it starts (in p_urrdmi->cur_text_obj)
             */
            SetCurChoice(p_urrdmi->cur_choice);
         } else {
            /*
             * creating a new text object
             * should just put the cursor where it starts
             * (but this may be difficult)
             */
            XEvent ev;

            memset(&ev, 0, sizeof(XEvent));
            ev.type = ButtonPress;
            ev.xbutton.button = Button1;
            ev.xbutton.state = 0;
            ev.xbutton.x = p_urrdmi->text_orig_x;
            ev.xbutton.y = p_urrdmi->text_orig_baseline_y;

            SetCurChoice(p_urrdmi->cur_choice);
            DrawText(&ev);
         }
      } else {
         SetCurChoice(p_urrdmi->cur_choice);
      }
      break;
   default: SetCurChoice(p_urrdmi->cur_choice); break;
   }
}

void UndoACmd(CmdPtr, HighLight, nPerformAction)
   struct CmdRec *CmdPtr;
   int HighLight, nPerformAction;
{
   struct CmdRec *cmd_ptr;

   if (topSel != NULL) { HighLightReverse(); RemoveAllSel(); }

   switch (CmdPtr->type) {
   case CMD_COMPOSITE:
      if (CmdPtr->first->type==CMD_MOVE || CmdPtr->first->type==CMD_STRETCH) {
         for (cmd_ptr=CmdPtr->last; cmd_ptr!=NULL; cmd_ptr=cmd_ptr->prev) {
            UndoACmd(cmd_ptr, FALSE, nPerformAction);
         }
      } else {
         for (cmd_ptr=CmdPtr->last; cmd_ptr!=NULL; cmd_ptr=cmd_ptr->prev) {
            UndoACmd(cmd_ptr, TRUE, nPerformAction);
         }
      }
      break;
   case CMD_NEW: UndoNewCmd(CmdPtr); break;
   case CMD_DELETE: UndoDeleteCmd(CmdPtr); break;
   case CMD_MOVE: UndoOrRedoMoveCmd(CmdPtr); break;
   case CMD_STRETCH: UndoOrRedoReplaceCmd(CmdPtr, TRUE); break;
   case CMD_REPLACE: UndoOrRedoReplaceCmd(CmdPtr, HighLight); break;
   case CMD_ONE_TO_MANY: UndoOrRedoOneToManyCmd(CmdPtr); break;
   case CMD_MANY_TO_ONE: UndoOrRedoOneToManyCmd(CmdPtr); break;
   case CMD_GOTO_PAGE: UndoOrRedoGotoPageCmd(CmdPtr); break;
   }
   CmdPtr->undone = TRUE;
}

void UndoCmd()
{
   UndoRedoRestoreDrawingModeInfo urrdmi;

   if (gstWBInfo.do_whiteboard) {
      MsgBox(TgLoadString(STID_UNDO_IN_WB), TOOL_NAME, INFO_MB);
      return;
   }
   SetUndoRedoRestoreDrawingModeInfo(&urrdmi, curChoice, textCursorShown,
         editingText, curTextModified, textOrigX, textOrigBaselineY);

   TieLooseEnds();
   SetCurChoice(NOTHING);
   if (curCmd == NULL) {
      if (!inHyperSpace && undoRedoRestoreDrawingMode) {
         SetCurChoice(urrdmi.cur_choice);
      }
      MsgBox(TgLoadString(STID_NO_COMMANDS_TO_UNDO), TOOL_NAME, INFO_MB);
      return;
   }
   undoingOrRedoing = TRUE;
   UndoACmd(curCmd, TRUE, TRUE);
   curCmd = curCmd->prev;
   undoingOrRedoing = FALSE;

   UndoRedoRestoreDrawingMode(&urrdmi);
}

int RedoACmd(CmdPtr, HighLight, nPerformAction)
   struct CmdRec *CmdPtr;
   int HighLight, nPerformAction;
{
   int ok=TRUE;
   struct CmdRec *cmd_ptr;

   if (topSel != NULL) { HighLightReverse(); RemoveAllSel(); }

   switch (CmdPtr->type) {
   case CMD_COMPOSITE:
      if (CmdPtr->first->type==CMD_MOVE || CmdPtr->first->type==CMD_STRETCH) {
         for (cmd_ptr=CmdPtr->first->next; cmd_ptr!=NULL;
               cmd_ptr=cmd_ptr->next) {
            if (!RedoACmd(cmd_ptr, FALSE, nPerformAction)) {
               ok = FALSE;
               break;
            }
         }
         if (ok) {
            if (!RedoACmd(CmdPtr->first, TRUE, nPerformAction)) {
               ok = FALSE;
            }
         }
      } else {
         for (cmd_ptr=CmdPtr->first; cmd_ptr!=NULL; cmd_ptr=cmd_ptr->next) {
            if (!RedoACmd(cmd_ptr, TRUE, nPerformAction)) {
               ok = FALSE;
               break;
            }
         }
      }
      break;
   case CMD_NEW: RedoNewCmd(CmdPtr); break;
   case CMD_DELETE: ok = RedoDeleteCmd(CmdPtr); break;
   case CMD_MOVE: ok = UndoOrRedoMoveCmd(CmdPtr); break;
   case CMD_STRETCH: ok = UndoOrRedoReplaceCmd(CmdPtr, TRUE); break;
   case CMD_REPLACE: ok = UndoOrRedoReplaceCmd(CmdPtr, HighLight); break;
   case CMD_ONE_TO_MANY: ok = UndoOrRedoOneToManyCmd(CmdPtr); break;
   case CMD_MANY_TO_ONE: ok = UndoOrRedoOneToManyCmd(CmdPtr); break;
   case CMD_GOTO_PAGE: UndoOrRedoGotoPageCmd(CmdPtr); break;
   case CMD_WB_SLIDESHOW: UndoOrRedoWBSlideShow(CmdPtr); break;
   }
   CmdPtr->undone = FALSE;
   return ok;
}

void RedoCmd()
{
   UndoRedoRestoreDrawingModeInfo urrdmi;

   if (gstWBInfo.do_whiteboard) {
      MsgBox(TgLoadString(STID_REDO_IN_WB), TOOL_NAME, INFO_MB);
      return;
   }
   SetUndoRedoRestoreDrawingModeInfo(&urrdmi, curChoice, textCursorShown,
         editingText, curTextModified, textOrigX, textOrigBaselineY);

   TieLooseEnds();
   SetCurChoice(NOTHING);
   if (firstCmd==NULL || (curCmd!=NULL && curCmd->next==NULL)) {
      if (!inHyperSpace && undoRedoRestoreDrawingMode) {
         SetCurChoice(urrdmi.cur_choice);
      }
      MsgBox(TgLoadString(STID_NO_COMMANDS_TO_REDO), TOOL_NAME, INFO_MB);
      return;
   }
   if (curCmd == NULL) {
      curCmd = firstCmd;
   } else {
      curCmd = curCmd->next;
   }
   undoingOrRedoing = TRUE;
   RedoACmd(curCmd, TRUE, TRUE);
   undoingOrRedoing = FALSE;

   UndoRedoRestoreDrawingMode(&urrdmi);
}

struct CmdStkRec {
   struct CmdRec *first, *last, *cur;
   int history_count;
   struct CmdStkRec *next;
};

static struct CmdStkRec *topCompositeCmdStk=NULL;

void StartCompositeCmd()
{
   struct CmdStkRec *cmd_stk_ptr;

   if (historyDepth == 0) return;

   cmd_stk_ptr = (struct CmdStkRec *)malloc(sizeof(struct CmdStkRec));
   if (cmd_stk_ptr == NULL) FailAllocMessage();
   memset(cmd_stk_ptr, 0, sizeof(struct CmdStkRec));

   cmd_stk_ptr->next = topCompositeCmdStk;
   cmd_stk_ptr->first = firstCmd;
   cmd_stk_ptr->last = lastCmd;
   cmd_stk_ptr->cur = curCmd;
   cmd_stk_ptr->history_count = historyCount;
   topCompositeCmdStk = cmd_stk_ptr;

   firstCmd = lastCmd = curCmd = NULL;
   historyCount = 0;

   composingCommand = TRUE;
}

void EndCompositeCmd()
{
   struct CmdRec *composite_cmd=NULL;
   int empty=FALSE;

   if (historyDepth == 0) return;

   if (firstCmd == NULL) {
      empty = TRUE;
   } else {
      composite_cmd = (struct CmdRec *)malloc(sizeof(struct CmdRec));
      if (composite_cmd == NULL) FailAllocMessage();
      memset(composite_cmd, 0, sizeof(struct CmdRec));
      composite_cmd->type = CMD_COMPOSITE;
      composite_cmd->include_tgif_obj = FALSE;
      composite_cmd->first = firstCmd;
      composite_cmd->last = lastCmd;
      composite_cmd->top_before = composite_cmd->bot_before = NULL;
      composite_cmd->top_after = composite_cmd->bot_after = NULL;
   }
   if (topCompositeCmdStk != NULL) {
      struct CmdStkRec *cmd_stk_ptr;

      firstCmd = topCompositeCmdStk->first;
      lastCmd = topCompositeCmdStk->last;
      curCmd = topCompositeCmdStk->cur;
      historyCount = topCompositeCmdStk->history_count;

      cmd_stk_ptr = topCompositeCmdStk;
      topCompositeCmdStk = topCompositeCmdStk->next;
      free(cmd_stk_ptr);
   } else {
      firstCmd = lastCmd = curCmd = NULL;
      historyCount = 0;
   }

   if (!empty) {
      if (curCmd == NULL) {
         ClearRedoRecords(firstCmd);
      } else if (curCmd != lastCmd) {
         ClearRedoRecords(curCmd);
      }
      if (++historyCount == historyDepth) {
         struct CmdRec *new_first_cmd=firstCmd->next;

         new_first_cmd->prev = NULL;
         firstCmd->next = NULL;
         DeleteARedoRecord(firstCmd, (-1.0), (-1.0));
         historyCount--;
         firstCmd = new_first_cmd;
      }
      curCmd = composite_cmd;
      InsertCmd(lastCmd, NULL, curCmd, &firstCmd, &lastCmd);
   }
   composingCommand = (topCompositeCmdStk != NULL);
}

void RestoreDefaultHistoryDepth()
{
   CleanUpCmds();
   historyDepth = defaultHistoryDepth;
   historyCount = 0;
   firstCmd = lastCmd = curCmd = NULL;
}

void DisableUndo()
{
   if (gstWBInfo.do_whiteboard) {
      /* ``Undo'' is already disabled in whiteboard mode */
      return;
   }
   CleanUpCmds();
   while (composingCommand) {
      EndCompositeCmd();
      CleanUpCmds();
   }
   historyDepth = 0;
}

void EnableUndo()
{
   if (gstWBInfo.do_whiteboard) {
      /* ``Redo'' is already disabled in whiteboard mode */
      return;
   }
   RestoreDefaultHistoryDepth();
}

struct SelRec *PrepareStackingInfo(TopSel, BotSel, NumObjs, PreparePhase,
      ppnStackingPosition, pnStackingCount, pnStackingPositionHasIds)
   struct SelRec *TopSel, *BotSel;
   int NumObjs, PreparePhase;
   int **ppnStackingPosition, *pnStackingCount, *pnStackingPositionHasIds;
   /*
    * In whiteboard mode and PreparePhase, *ppnStackingPosition will
    *       be a list of full_ids.  Otherwise, it will just be a
    *       list of indices.
    */
{
   int pos=0;
   struct SelRec *sel_ptr=NULL;
   struct ObjRec *obj_ptr=NULL;

   if (gstWBInfo.do_whiteboard && PreparePhase) {
      char **ppsz_full_ids=(char**)malloc(NumObjs*sizeof(char*));

      if (ppsz_full_ids == NULL) FailAllocMessage();
      memset(ppsz_full_ids, 0, NumObjs*sizeof(char*));

      *ppnStackingPosition = NULL;
      *pnStackingCount = 0;
      pos = 0;
      sel_ptr = TopSel;
      for (obj_ptr=topObj; obj_ptr!=NULL; obj_ptr=obj_ptr->next, pos++) {
         if (obj_ptr == sel_ptr->obj) {
            char buf[MAXSTRING];

            if (obj_ptr->creator_full_id == NULL) {
               sprintf(buf, "#%1d:%1d/%s", pos, obj_ptr->id, gszLocalPID);
            } else {
               sprintf(buf, "#%1d:%s", pos, obj_ptr->creator_full_id);
            }
            ppsz_full_ids[(*pnStackingCount)++] = UtilStrDup(buf);
            if (ppsz_full_ids[(*pnStackingCount)-1] == NULL) FailAllocMessage();

            sel_ptr = sel_ptr->next;
            if (*pnStackingCount == NumObjs) break;
         }
      }
      *ppnStackingPosition = (int*)ppsz_full_ids;
      if (pnStackingPositionHasIds != NULL) *pnStackingPositionHasIds = TRUE;
   } else {
      *ppnStackingPosition = (int*)malloc(NumObjs*sizeof(int));
      if (*ppnStackingPosition == NULL) FailAllocMessage();
      memset(*ppnStackingPosition, 0, NumObjs*sizeof(int));
      *pnStackingCount = 0;
      pos = 0;
      sel_ptr = TopSel;
      for (obj_ptr=topObj; obj_ptr!=NULL; obj_ptr=obj_ptr->next, pos++) {
         if (obj_ptr == sel_ptr->obj) {
            (*ppnStackingPosition)[(*pnStackingCount)++] = pos;
            sel_ptr = sel_ptr->next;
            if (*pnStackingCount == NumObjs) break;
         }
      }
      if (pnStackingPositionHasIds != NULL) *pnStackingPositionHasIds = FALSE;
   }
   return sel_ptr;
}

static
void PrepareStacking(TopSel, BotSel, NumObjs, PreparePhase)
   struct SelRec *TopSel, *BotSel;
   int NumObjs, PreparePhase;
{
   struct SelRec *sel_ptr;

   stackingPosition = NULL;
   stackingCount = 0;
   sel_ptr = PrepareStackingInfo(TopSel, BotSel, NumObjs, PreparePhase,
         &stackingPosition, &stackingCount, &stackingPositionHasIds);
   if (sel_ptr != NULL || stackingCount != NumObjs) {
      sprintf(gszMsgBox, TgLoadString(STID_SELECT_LIST_NOT_SORTED_IN),
            "PrepareStacking()");
      FatalUnexpectedError(gszMsgBox,
            TgLoadString(STID_UNDO_REDO_MAY_CAUSE_CRASH));
   }
}

void PrepareToRecord(CmdType, TopSel, BotSel, NumObjs)
   int CmdType, NumObjs;
   struct SelRec *TopSel, *BotSel;
{
   struct SelRec *sel_ptr=NULL, *to_sel_ptr=NULL;

   if (historyDepth == 0) return;

   preparedColormap = (gnInImageProc ? mainColormap : None);

   topSelBeforeInCmd = botSelBeforeInCmd = NULL;
   stackingPosition = NULL;
   stackingCount = 0;
   stackingPositionHasIds = FALSE;

   switch (CmdType) {
   case CMD_NEW: break;

   case CMD_DELETE:
   case CMD_MOVE:
   case CMD_STRETCH:
   case CMD_REPLACE:
      PrepareStacking(TopSel, BotSel, NumObjs, TRUE);
      if (CmdType == CMD_MOVE) {
         CopySel(TopSel, NumObjs, &topSelBeforeInCmd, &botSelBeforeInCmd);
      } else {
         /* stackingPositionHasIds is TRUE if we are doing whiteboard */
         DupTheseObjects(TopSel, BotSel, &topSelBeforeInCmd,
               &botSelBeforeInCmd);
         for (sel_ptr=TopSel, to_sel_ptr=topSelBeforeInCmd; to_sel_ptr!=NULL;
               sel_ptr=sel_ptr->next, to_sel_ptr=to_sel_ptr->next) {
            CopyObjId(sel_ptr->obj, to_sel_ptr->obj);
            CopyObjLocks(sel_ptr->obj, to_sel_ptr->obj);
         }
      }
      break;

   case CMD_GOTO_PAGE: stackingCount = NumObjs; break;
   case CMD_WB_CLEARALL: break;
   }
   return;
}

void FreeAfterSel(CmdPtr)
   struct CmdRec *CmdPtr;
{
   struct SelRec *sel_ptr=NULL, *next_sel=NULL;

   for (sel_ptr=CmdPtr->top_after; sel_ptr!=NULL; sel_ptr=next_sel) {
      next_sel = sel_ptr->next;
      free(sel_ptr);
   }
   CmdPtr->top_after = CmdPtr->bot_after = NULL;
}

static
void PrepareExtendedSerializationInfoForRecordCmd(SubCmdPtr, TopSel, BotSel,
      NumObjs, psi)
   struct SubCmdRec *SubCmdPtr;
   struct SelRec *TopSel, *BotSel;
   int NumObjs;
   SerializationInfo *psi;
{
   psi->subcmd = SubCmdPtr;
   psi->top_sel = TopSel;
   psi->bot_sel = BotSel;
   psi->num_objs = NumObjs;

   psi->include_tgif_obj = recordCmdIncludeTgifObj;
   psi->new_colormap = recordCmdUsesNewColormap;
   psi->logical_clock = gstWBInfo.logical_clock;
   psi->sender_process_id = gszLocalPID;

   psi->top_before = topSelBeforeInCmd;
   psi->bot_before = botSelBeforeInCmd;
   psi->pos_before = stackingPosition;
   psi->count_before = stackingCount;
   psi->pos_before_has_ids = stackingPositionHasIds;
}

void RecordCmd(CmdType, SubCmdPtr, TopSel, BotSel, NumObjs)
   int CmdType, NumObjs;
   struct SubCmdRec *SubCmdPtr;
   struct SelRec *TopSel, *BotSel;
{
   int logical_clock=0;
   int wb_send_data_failed=FALSE, plain_wb_data_sz=0;
   char *psz_wb_data=NULL, *psz_plain_wb_data=NULL;

   if (gstWBInfo.do_whiteboard) {
      if (gstWBInfo.dont_serialize || CmdType == CMD_GOTO_PAGE) {
         /*
          * One of the important thing that happens in SerializeCmd()
          *         is that the local logical clock got incremented.
          *         If we don't increment the logical clock, we may
          *         create a new command that have the same logical clock
          *         as the one created last time we got here and that's
          *         illegal. Therefore, we must increment the logical clock.
          */
         logical_clock = gstWBInfo.logical_clock++;
      } else {
         SerializationInfo si;

         memset(&si, 0, sizeof(SerializationInfo));
         PrepareExtendedSerializationInfoForRecordCmd(SubCmdPtr, TopSel, BotSel,
               NumObjs, &si);
         
         serializingFile = TRUE;

         psz_plain_wb_data = NULL;
         plain_wb_data_sz = 0;
         if ((psz_wb_data=SerializeCmd(CmdType, &si, &logical_clock,
               &psz_plain_wb_data, &plain_wb_data_sz)) != NULL) {
            if (!SendWBData(psz_wb_data,logical_clock)) {
               wb_send_data_failed = TRUE;
            }
         }
         serializingFile = FALSE;
      }
   } else {
      if (historyDepth == 0) return;

      if (curCmd == NULL) {
         ClearRedoRecords(firstCmd);
      } else if (curCmd != lastCmd) {
         ClearRedoRecords(curCmd);
      }
      if (++historyCount == historyDepth && !composingCommand) {
         struct CmdRec *new_first_cmd=firstCmd->next;

         new_first_cmd->prev = NULL;
         firstCmd->next = NULL;
         DeleteARedoRecord(firstCmd, (-1.0), (-1.0));
         historyCount--;
         firstCmd = new_first_cmd;
      }
   }
   curCmd = (struct CmdRec *)malloc(sizeof(struct CmdRec));
   if (curCmd == NULL) FailAllocMessage();
   memset(curCmd, 0, sizeof(struct CmdRec));
   curCmd->top_before = topSelBeforeInCmd;
   curCmd->bot_before = botSelBeforeInCmd;
   if (gstWBInfo.do_whiteboard) {
      curCmd->serialized = TRUE;
   }
   curCmd->pos_before = stackingPosition;
   curCmd->count_before = stackingCount;
   curCmd->pos_before_has_ids = stackingPositionHasIds;
   curCmd->type = CmdType;
   curCmd->undone = FALSE;
   curCmd->include_tgif_obj = recordCmdIncludeTgifObj;
   curCmd->new_colormap = recordCmdUsesNewColormap;

   if (TopSel != NULL) {
      CopySel(TopSel, NumObjs, &(curCmd->top_after), &(curCmd->bot_after));
      PrepareStacking(TopSel, BotSel, NumObjs, FALSE);
      curCmd->pos_after = stackingPosition;
      curCmd->count_after = stackingCount;
   } else {
      curCmd->top_after = curCmd->bot_after = NULL;
      curCmd->pos_after = NULL;
      curCmd->count_after = 0;
   }
   if (gstWBInfo.do_whiteboard) {
      struct CmdRec *immed_right_cmd=NULL;

      curCmd->logical_clock = logical_clock;
      curCmd->sender_process_id = UtilStrDup(gszLocalPID);
      if (curCmd->sender_process_id == NULL) FailAllocMessage();

      if (!wb_send_data_failed) {
         /*
          * FindShadowCmdInsertionPoint() examine the logical clock of the new
          *       command and decides where to insert it in the shadow cmd list.
          */
         FindCmdInsertionPoint(curCmd, &immed_right_cmd);

         if (immed_right_cmd == NULL) {
            /* append */
            CopyAndInsertCmd(FALSE,
                  (psz_plain_wb_data==NULL ? psz_wb_data : psz_plain_wb_data),
                  gstWBInfo.last_shadow_cmd, NULL, curCmd,
                  &gstWBInfo.first_shadow_cmd, &gstWBInfo.last_shadow_cmd);
         } else {
            /* insert */
            CopyAndInsertCmd(FALSE,
                  (psz_plain_wb_data==NULL ? psz_wb_data : psz_plain_wb_data),
                  immed_right_cmd->prev, immed_right_cmd, curCmd,
                  &gstWBInfo.first_shadow_cmd, &gstWBInfo.last_shadow_cmd);
         }
      }
      immed_right_cmd = NULL;
      /*
       * FindCmdInsertionPoint() examine the logical clock of the new command
       *       and decides where to insert it.
       */
      FindCmdInsertionPoint(curCmd, &immed_right_cmd);

      if (immed_right_cmd == NULL) {
         /* append */
         InsertCmd(gstWBInfo.last_cmd, NULL, curCmd, &gstWBInfo.first_cmd,
               &gstWBInfo.last_cmd);
      } else {
         /* insert */
         InsertCmd(immed_right_cmd->prev, immed_right_cmd, curCmd,
               &gstWBInfo.first_cmd, &gstWBInfo.last_cmd);
      }
      if (CmdType == CMD_WB_CLEARALL) {
         CleanUpObsoletedWBCmds(curCmd);
      }
   } else {
      InsertCmd(lastCmd, NULL, curCmd, &firstCmd, &lastCmd);
   }
   switch (CmdType) {
   case CMD_NEW: break;
   case CMD_DELETE: break;
   case CMD_MOVE:
      curCmd->subcmd = (struct SubCmdRec *)malloc(sizeof(struct SubCmdRec));
      if (curCmd->subcmd == NULL) FailAllocMessage();
      memset(curCmd->subcmd, 0, sizeof(struct SubCmdRec));
      curCmd->subcmd->detail.move.dx = SubCmdPtr->detail.move.dx;
      curCmd->subcmd->detail.move.dy = SubCmdPtr->detail.move.dy;
      break;
   case CMD_STRETCH: FreeAfterSel(curCmd); break;
   case CMD_REPLACE: FreeAfterSel(curCmd); break;
   case CMD_ONE_TO_MANY: break;
   case CMD_MANY_TO_ONE: break;
   case CMD_GOTO_PAGE: curCmd->count_after = NumObjs; break;
   case CMD_WB_CLEARALL:
      curCmd->subcmd = (struct SubCmdRec *)malloc(sizeof(struct SubCmdRec));
      if (curCmd->subcmd == NULL) FailAllocMessage();
      memset(curCmd->subcmd, 0, sizeof(struct SubCmdRec));
      curCmd->subcmd->detail.clearall.page_style =
            SubCmdPtr->detail.clearall.page_style;
      curCmd->subcmd->detail.clearall.print_mag =
            SubCmdPtr->detail.clearall.print_mag;
      break;
   case CMD_CHAT_A_LINE:
      /* intentionally forget what has been chatted */
      break;
   case CMD_WB_SLIDESHOW:
      curCmd->subcmd = (struct SubCmdRec *)malloc(sizeof(struct SubCmdRec));
      if (curCmd->subcmd == NULL) FailAllocMessage();
      memset(curCmd->subcmd, 0, sizeof(struct SubCmdRec));
      curCmd->subcmd->detail.slideshow.into_slideshow =
            SubCmdPtr->detail.slideshow.into_slideshow;
      break;
   }
   if (gstWBInfo.do_whiteboard) {
      if (wb_send_data_failed) {
         UndoACmd(curCmd, FALSE, TRUE);
         UnlinkCmd(curCmd, &gstWBInfo.first_cmd, &gstWBInfo.last_cmd);
         DeleteARedoRecord(curCmd, (-1.0), (-1.0));
      }
      if (psz_wb_data != NULL) {
         SerializeFreeBuf(psz_wb_data);
      }
      if (psz_plain_wb_data != NULL) {
         UtilFree(psz_plain_wb_data);
      }
   }
   curCmd = lastCmd;
}

void AbortPrepareCmd(CmdType)
   int CmdType;
{
   struct SelRec *sel_ptr=NULL, *next_sel=NULL;

   if (historyDepth == 0) return;

   for (sel_ptr=topSelBeforeInCmd; sel_ptr!=NULL; sel_ptr=next_sel) {
      next_sel = sel_ptr->next;
      switch (CmdType) {
      case CMD_REPLACE: FreeObj(sel_ptr->obj); break;
      }
      free(sel_ptr);
   }
   if (stackingPosition != NULL) {
      if (stackingPositionHasIds) {
         int i=0;

         for (i=0; i < stackingCount; i++) {
            UtilFree((char*)(long)(stackingPosition[i]));
         }
      }
      free(stackingPosition);
   }
   stackingPosition = NULL;
   stackingCount = 0;
   stackingPositionHasIds = FALSE;
   topSelBeforeInCmd = botSelBeforeInCmd = NULL;
}

void RecordNewObjCmd()
{
   if (historyDepth == 0) return;

   if (topSel==NULL) {
      struct SelRec *sel_ptr;

      sel_ptr = (struct SelRec *)malloc(sizeof(struct SelRec));
      if (sel_ptr == NULL) FailAllocMessage();
      memset(sel_ptr, 0, sizeof(struct SelRec));
      sel_ptr->next = sel_ptr->prev = NULL;
      sel_ptr->obj = topObj;
      PrepareToRecord(CMD_NEW, NULL, NULL, 0);
      RecordCmd(CMD_NEW, NULL, sel_ptr, sel_ptr, 1);
      free(sel_ptr);
   } else {
      PrepareToRecord(CMD_NEW, NULL, NULL, 0);
      RecordCmd(CMD_NEW, NULL, topSel, botSel, 1);
   }
}

void PrepareToReplaceAnObj(BeforeObjPtr)
   struct ObjRec *BeforeObjPtr;
{
   struct SelRec *sel_ptr;

   if (historyDepth == 0) return;

   sel_ptr = (struct SelRec *)malloc(sizeof(struct SelRec));
   if (sel_ptr == NULL) FailAllocMessage();
   memset(sel_ptr, 0, sizeof(struct SelRec));
   sel_ptr->next = sel_ptr->prev = NULL;
   sel_ptr->obj = BeforeObjPtr;
   PrepareToRecord(CMD_REPLACE, sel_ptr, sel_ptr, 1);
   free(sel_ptr);
}

void RecordReplaceAnObj(AfterObjPtr)
   struct ObjRec *AfterObjPtr;
{
   struct SelRec *sel_ptr;

   if (historyDepth == 0) return;

   sel_ptr = (struct SelRec *)malloc(sizeof(struct SelRec));
   if (sel_ptr == NULL) FailAllocMessage();
   memset(sel_ptr, 0, sizeof(struct SelRec));
   sel_ptr->next = sel_ptr->prev = NULL;
   sel_ptr->obj = AfterObjPtr;
   RecordCmd(CMD_REPLACE, NULL, sel_ptr, sel_ptr, 1);
   free(sel_ptr);
}

void ChangeReplaceOneCmdToDeleteCmd()
{
   RecordCmd(CMD_DELETE, NULL, NULL, NULL, 0);
}

void RecordWBClearAll()
{
   if (gstWBInfo.do_whiteboard) {
      struct SubCmdRec subcmd;

      memset(&subcmd, 0, sizeof(struct SubCmdRec));
      subcmd.detail.clearall.page_style = pageStyle;
      subcmd.detail.clearall.print_mag = printMag;

      PrepareToRecord(CMD_WB_CLEARALL, NULL, NULL, 0);
      RecordCmd(CMD_WB_CLEARALL, &subcmd, NULL, NULL, 0);
   }
}

void RecordWBSlideShow(into_slideshow)
   int into_slideshow;
{
   if (gstWBInfo.do_whiteboard) {
      if (pageLayoutMode == PAGE_STACK && firstPage != lastPage) {
         struct SubCmdRec subcmd;

         memset(&subcmd, 0, sizeof(struct SubCmdRec));
         subcmd.detail.slideshow.into_slideshow = into_slideshow;

         PrepareToRecord(CMD_WB_SLIDESHOW, NULL, NULL, 0);
         RecordCmd(CMD_WB_SLIDESHOW, &subcmd, NULL, NULL, 0);
      }
   }
}

void RecordWBChatALine(pSubCmdPtr)
   struct SubCmdRec *pSubCmdPtr;
{
   if (gstWBInfo.do_whiteboard) {
      PrepareToRecord(CMD_CHAT_A_LINE, NULL, NULL, 0);
      RecordCmd(CMD_CHAT_A_LINE, pSubCmdPtr, NULL, NULL, 0);
   }
}

