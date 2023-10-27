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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/group.c,v 1.7 2011/05/16 16:21:57 william Exp $
 */

#define _INCLUDE_FROM_GROUP_C_

#include "tgifdefs.h"

#include "attr.e"
#include "choice.e"
#include "cmd.e"
#include "dialog.e"
#include "drawing.e"
#include "dup.e"
#include "file.e"
#include "group.e"
#include "mark.e"
#include "msg.e"
#include "obj.e"
#include "page.e"
#include "select.e"
#include "setup.e"
#include "strtbl.e"

int gnDeleteAttrsWhileUngrouping=FALSE;

struct ObjRec *JustCreateGroupObj()
{
   struct GroupRec *group_ptr=NULL;
   struct ObjRec *obj_ptr=NULL;

   group_ptr = (struct GroupRec *)malloc(sizeof(struct GroupRec));
   if (group_ptr == NULL) FailAllocMessage();
   memset(group_ptr, 0, sizeof(struct GroupRec));
   group_ptr->first = NULL;
   group_ptr->last = NULL;
   group_ptr->rotate = ROTATE0;
   group_ptr->flip = NO_FLIP;
   group_ptr->deck_index = (-1);
   group_ptr->pin_connected = 0;
   group_ptr->first_conn = group_ptr->last_conn = NULL;
   obj_ptr = (struct ObjRec *)malloc(sizeof(struct ObjRec));
   if (obj_ptr == NULL) FailAllocMessage();
   memset(obj_ptr, 0, sizeof(struct ObjRec));
   obj_ptr->x = 0;
   obj_ptr->y = 0;
   obj_ptr->id = objId++;
   obj_ptr->locked = FALSE;
   obj_ptr->type = OBJ_GROUP;
   obj_ptr->bbox.ltx = 0;
   obj_ptr->bbox.lty = 0;
   obj_ptr->bbox.rbx = 0;
   obj_ptr->bbox.rby = 0;
   obj_ptr->obbox.ltx = 0;
   obj_ptr->obbox.lty = 0;
   obj_ptr->obbox.rbx = 0;
   obj_ptr->obbox.rby = 0;
   obj_ptr->detail.r = group_ptr;
   obj_ptr->fattr = obj_ptr->lattr = NULL;
   obj_ptr->ctm = NULL;
   obj_ptr->invisible = FALSE;

   return obj_ptr;
}

void CreateGroupObj(TopObjPtr, BotObjPtr)
   struct ObjRec *TopObjPtr, *BotObjPtr;
{
   struct ObjRec *obj_ptr=JustCreateGroupObj();
   struct GroupRec *group_ptr=obj_ptr->detail.r;

   group_ptr->first = TopObjPtr;
   group_ptr->last = BotObjPtr;
   obj_ptr->x = selObjLtX; /* note:  selLtX, selLtY are absolute */
   obj_ptr->y = selObjLtY;
   obj_ptr->bbox.ltx = selLtX;
   obj_ptr->bbox.lty = selLtY;
   obj_ptr->bbox.rbx = selRbX;
   obj_ptr->bbox.rby = selRbY;
   obj_ptr->obbox.ltx = selObjLtX;
   obj_ptr->obbox.lty = selObjLtY;
   obj_ptr->obbox.rbx = selObjRbX;
   obj_ptr->obbox.rby = selObjRbY;
   if (numObjLocked > 0) obj_ptr->locked = TRUE;
   AddObj(NULL, topObj, obj_ptr);
}

void SaveGroupObj(FP, ObjPtr, Level)
   FILE *FP;
   struct ObjRec *ObjPtr;
   int Level;
{
   if (fprintf(FP, "group([\n") == EOF) writeFileFailed = TRUE;
   Save(FP, ObjPtr->detail.r->last, Level+1, INVALID);
   if (fprintf(FP, "],\n") == EOF) writeFileFailed = TRUE;
   if (fprintf(FP, "%1d,%1d,%1d,", ObjPtr->id, ObjPtr->locked,
         ObjPtr->invisible) == EOF) {
      writeFileFailed = TRUE;
   }
   if (serializingFile) SaveCreatorID(FP, ObjPtr, "\t");
   SaveAttrs(FP, ObjPtr->lattr);
   if (fprintf(FP, ")") == EOF) writeFileFailed = TRUE;
}

void SaveCompObj(FP, ObjPtr, Level)
   FILE *FP;
   struct ObjRec *ObjPtr;
   int Level;
{
   if (fprintf(FP, "sym([\n") == EOF) writeFileFailed = TRUE;
   Save(FP, ObjPtr->detail.r->last, Level+1, INVALID);
   if (fprintf(FP, "],\n") == EOF) writeFileFailed = TRUE;
   if (fprintf(FP, "%1d,%1d,%1d,", ObjPtr->id, ObjPtr->locked,
         ObjPtr->invisible) == EOF) {
      writeFileFailed = TRUE;
   }
   if (serializingFile) SaveCreatorID(FP, ObjPtr, "\t");
   SaveAttrs(FP, ObjPtr->lattr);
   if (fprintf(FP, ")") == EOF) writeFileFailed = TRUE;
}

void SaveIconObj(FP, ObjPtr, Level)
   FILE *FP;
   struct ObjRec *ObjPtr;
   int Level;
{
   if (fprintf(FP, "icon([\n") == EOF) writeFileFailed = TRUE;
   Save(FP, ObjPtr->detail.r->last, Level+1, INVALID);
   if (fprintf(FP, "],\n") == EOF) writeFileFailed = TRUE;
   if (fprintf(FP, "\"%s\",%1d,%1d,%1d,%1d,%1d,",
         ObjPtr->detail.r->s, ObjPtr->id, ObjPtr->detail.r->rotate,
         ObjPtr->detail.r->flip, ObjPtr->locked, ObjPtr->invisible) == EOF) {
      writeFileFailed = TRUE;
   }
   if (serializingFile) SaveCreatorID(FP, ObjPtr, "\t");
   SaveAttrs(FP, ObjPtr->lattr);
   if (fprintf(FP, ")") == EOF) writeFileFailed = TRUE;
}

void SavePinObj(FP, ObjPtr, Level)
   FILE *FP;
   struct ObjRec *ObjPtr;
   int Level;
{
   if (fprintf(FP, "pin([\n") == EOF) writeFileFailed = TRUE;
   Save(FP, ObjPtr->detail.r->last, Level+1, INVALID);
   if (fprintf(FP, "],\n") == EOF) writeFileFailed = TRUE;
/*
 * Need to check for pins.
 * Need to handle the case where connection objects need to be dupped!
 */
   if (fprintf(FP, "\"%s\",%1d,%1d,%1d,%1d,%1d,%1d,",
         ObjPtr->detail.r->s, ObjPtr->id, ObjPtr->detail.r->rotate,
         ObjPtr->detail.r->flip, ObjPtr->locked, ObjPtr->invisible,
         ObjPtr->detail.r->pin_connected) == EOF) {
      writeFileFailed = TRUE;
   }
   if (serializingFile) SaveCreatorID(FP, ObjPtr, "\t");
   SaveAttrs(FP, ObjPtr->lattr);
   if (fprintf(FP, ")") == EOF) writeFileFailed = TRUE;
}

static int gnPinWarning=FALSE;

#define GETGROUPVALUE(val,name) ScanValue("%d", &(val), name, "group")
#define GETSYMVALUE(val,name) ScanValue("%d", &(val), name, "sym")
#define GETICONVALUE(val,name) ScanValue("%d", &(val), name, "icon")
#define GETPINVALUE(val,name) ScanValue("%d", &(val), name, "pin")

void ReadGroupObj(FP, ObjType, ObjPtr)
   FILE *FP;
   int ObjType;
   struct ObjRec **ObjPtr;
{
   struct GroupRec *group_ptr;
   struct ObjRec *top_obj=NULL, *bot_obj=NULL, *obj_ptr;
   int ltx, lty, rbx, rby, id=0, locked=FALSE;
   int obj_ltx, obj_lty, obj_rbx, obj_rby, rotate=0, flip=0;
   int invisible=FALSE, pin_connected=0;
   char line[MAXSTRING+1], *s, *s1, tmp_str[MAXSTRING+1];

   *ObjPtr = NULL;
   while (ReadObj(FP, &obj_ptr)) {
      if (obj_ptr == NULL) return;

      obj_ptr->next = top_obj;
      if (top_obj == NULL) {
         bot_obj = obj_ptr;
      } else {
         top_obj->prev = obj_ptr;
      }
      top_obj = obj_ptr;
   }
   if (top_obj == NULL) return;

   if (fileVersion <= 20 && (ObjType==OBJ_GROUP || ObjType==OBJ_SYM)) {
      id = objId++;
   } else {
      if (fgets(line, MAXSTRING, FP) == NULL) return;
      scanLineNum++;

      switch (ObjType) {
      case OBJ_GROUP:
         InitScan(line, "\t\n, []");
         if (fileVersion <= 25) {
            if (GETGROUPVALUE(id, "id") == INVALID) return;
         } else if (fileVersion <= 32) {
            if (GETGROUPVALUE(id, "id") == INVALID ||
                GETGROUPVALUE(locked, "locked") == INVALID) {
               return;
            }
         } else {
            if (GETGROUPVALUE(id, "id") == INVALID ||
                GETGROUPVALUE(locked, "locked") == INVALID ||
                GETGROUPVALUE(invisible, "invisible") == INVALID) {
               return;
            }
         }
         if (id >= objId) objId = id+1;
         break;
      case OBJ_SYM:
         InitScan(line, "\t\n, []");
         if (fileVersion <= 25) {
            if (GETSYMVALUE(id, "id") == INVALID) return;
         } else if (fileVersion <= 32) {
            if (GETSYMVALUE(id, "id") == INVALID ||
                GETSYMVALUE(locked, "locked") == INVALID) {
               return;
            }
         } else {
            if (GETSYMVALUE(id, "id") == INVALID ||
                GETSYMVALUE(locked, "locked") == INVALID ||
                GETSYMVALUE(invisible, "invisible") == INVALID) {
               return;
            }
         }
         if (id >= objId) objId = id+1;
         break;
      case OBJ_ICON:
         strcpy(tmp_str, FindChar((int)'"', line));
         s = FindChar((int)'"', tmp_str);
         if (fileVersion == INVALID) return;

         if (fileVersion <= 12) {
            s1 = FindChar((int)',', s);
            InitScan(s1, "\t\n, ");
            if (GETICONVALUE(id, "id") == INVALID) return;
         } else if (fileVersion <= 25) {
            s1 = FindChar((int)',', s);
            InitScan(s1, "\t\n, ");
            if (GETICONVALUE(id, "id") == INVALID ||
                GETICONVALUE(rotate, "rotation") == INVALID ||
                GETICONVALUE(flip, "flip") == INVALID) {
               return;
            }
         } else if (fileVersion <= 32) {
            s1 = FindChar((int)',', s);
            InitScan(s1, "\t\n, ");
            if (GETICONVALUE(id, "id") == INVALID ||
                GETICONVALUE(rotate, "rotation") == INVALID ||
                GETICONVALUE(flip, "flip") == INVALID ||
                GETICONVALUE(locked, "locked") == INVALID) {
               return;
            }
         } else {
            s1 = FindChar((int)',', s);
            InitScan(s1, "\t\n, ");
            if (GETICONVALUE(id, "id") == INVALID ||
                GETICONVALUE(rotate, "rotation") == INVALID ||
                GETICONVALUE(flip, "flip") == INVALID ||
                GETICONVALUE(locked, "locked") == INVALID ||
                GETICONVALUE(invisible, "invisible") == INVALID) {
               return;
            }
         }
         if (id >= objId) objId = id+1;
         *(--s) = '\0';
         break;
      case OBJ_PIN:
         strcpy(tmp_str, FindChar((int)'"', line));
         s = FindChar((int)'"', tmp_str);
         if (fileVersion == INVALID) return;

         if (fileVersion >= 34) {
            s1 = FindChar((int)',', s);
            InitScan(s1, "\t\n, ");
/*
 * Need to check for pins.
 * Need to handle the case where connection objects need to be dupped!
 */
            if (GETPINVALUE(id, "id") == INVALID ||
                GETPINVALUE(rotate, "rotation") == INVALID ||
                GETPINVALUE(flip, "flip") == INVALID ||
                GETPINVALUE(locked, "locked") == INVALID ||
                GETPINVALUE(invisible, "invisible") == INVALID ||
                GETPINVALUE(pin_connected, "pin_connected") == INVALID) {
               return;
            }
         }
         if (id >= objId) objId = id+1;
         *(--s) = '\0';
         if (!gnPinWarning) {
            gnPinWarning = TRUE;
            strcpy(gszMsgBox, TgLoadString(STID_WARN_PIN_NOT_SUPPORTED));
            if (PRTGIF) {
               fprintf(stderr, "%s\n", gszMsgBox);
            } else {
               MsgBox(gszMsgBox, TOOL_NAME, STOP_MB);
            }
         }
         break;
      }
   }

   *ObjPtr = (struct ObjRec *)malloc(sizeof(struct ObjRec));
   if (*ObjPtr == NULL) FailAllocMessage();
   memset(*ObjPtr, 0, sizeof(struct ObjRec));

   top_obj->prev = NULL;

   group_ptr = (struct GroupRec *)malloc(sizeof(struct GroupRec));
   if (group_ptr == NULL) FailAllocMessage();
   memset(group_ptr, 0, sizeof(struct GroupRec));
   group_ptr->first = top_obj;
   group_ptr->last = bot_obj;
   group_ptr->rotate = rotate;
   group_ptr->flip = flip;
   group_ptr->deck_index = (-1);
   group_ptr->pin_connected = pin_connected;
   group_ptr->first_conn = group_ptr->last_conn = NULL;
   if (ObjType == OBJ_ICON || ObjType == OBJ_PIN) {
      strcpy(group_ptr->s, tmp_str);
   }
   ltx = top_obj->bbox.ltx;
   lty = top_obj->bbox.lty;
   rbx = top_obj->bbox.rbx;
   rby = top_obj->bbox.rby;
   obj_ltx = top_obj->obbox.ltx;
   obj_lty = top_obj->obbox.lty;
   obj_rbx = top_obj->obbox.rbx;
   obj_rby = top_obj->obbox.rby;
   for (obj_ptr = top_obj->next; obj_ptr != NULL; obj_ptr = obj_ptr->next) {
      if (obj_ptr->bbox.ltx < ltx) ltx = obj_ptr->bbox.ltx;
      if (obj_ptr->bbox.lty < lty) lty = obj_ptr->bbox.lty;
      if (obj_ptr->bbox.rbx > rbx) rbx = obj_ptr->bbox.rbx;
      if (obj_ptr->bbox.rby > rby) rby = obj_ptr->bbox.rby;
      if (obj_ptr->obbox.ltx < obj_ltx) obj_ltx = obj_ptr->obbox.ltx;
      if (obj_ptr->obbox.lty < obj_lty) obj_lty = obj_ptr->obbox.lty;
      if (obj_ptr->obbox.rbx > obj_rbx) obj_rbx = obj_ptr->obbox.rbx;
      if (obj_ptr->obbox.rby > obj_rby) obj_rby = obj_ptr->obbox.rby;
   }
   
   (*ObjPtr)->x = obj_ltx;
   (*ObjPtr)->y = obj_lty;
   (*ObjPtr)->dirty = FALSE;
   (*ObjPtr)->id = id;
   (*ObjPtr)->locked = locked;
   (*ObjPtr)->type = ObjType;
   (*ObjPtr)->bbox.ltx = ltx;
   (*ObjPtr)->bbox.lty = lty;
   (*ObjPtr)->bbox.rbx = rbx;
   (*ObjPtr)->bbox.rby = rby;
   (*ObjPtr)->obbox.ltx = obj_ltx;
   (*ObjPtr)->obbox.lty = obj_lty;
   (*ObjPtr)->obbox.rbx = obj_rbx;
   (*ObjPtr)->obbox.rby = obj_rby;
   (*ObjPtr)->detail.r = group_ptr;
   (*ObjPtr)->ctm = NULL;
   (*ObjPtr)->invisible = invisible;
}

void FreeGroupObj(ObjPtr)
   struct ObjRec *ObjPtr;
{
   register struct ObjRec *ptr, *next_obj;

   for (ptr=ObjPtr->detail.r->first; ptr != NULL; ptr=next_obj) {
      next_obj = ptr->next;
      FreeObj(ptr);
   }
   free(ObjPtr->detail.r);
   free(ObjPtr);
}

void UngroupObj(ObjPtr, TopSelPtr, BotSelPtr)
   struct ObjRec *ObjPtr;
   struct SelRec **TopSelPtr, **BotSelPtr;
   /* ungroup the grouped object ObjPtr to a list of objects   */
   /* when returns, a list of select pointers will be created, */
   /*    *TopSelPtr will point to the top of the list, and     */
   /*    *BotSelPtr will point to the bottom of the list.      */
{
   struct ObjRec *obj_ptr=ObjPtr->detail.r->last;

   for ( ; obj_ptr != NULL; obj_ptr=obj_ptr->prev) {
      AddObjIntoSel(obj_ptr, NULL, *TopSelPtr, TopSelPtr, BotSelPtr);
   }
   (*TopSelPtr)->prev = NULL;
}

static
int NoObjToUngroup(pn_force_ungroup)
   int *pn_force_ungroup;
{
   struct SelRec *sel_ptr=NULL;
   int every_obj_is_sym_or_icon=TRUE;

   if (pn_force_ungroup != NULL && topSel != NULL && topSel == botSel) {
      int obj_type=topSel->obj->type;

      *pn_force_ungroup = FALSE;
      if (obj_type==OBJ_ICON || obj_type==OBJ_SYM) {
         /*
          * Need to check for pins.
          */
         if (MsgBox(TgLoadString(STID_ONE_SIMPLE_GROUP_UNGROUP_ANY), TOOL_NAME,
               YNC_MB) == MB_ID_YES) {
            *pn_force_ungroup = TRUE;
            return FALSE;
         }
         return TRUE;
      }
   }
   for (sel_ptr=topSel; sel_ptr != NULL; sel_ptr=sel_ptr->next) {
      int obj_type=sel_ptr->obj->type;

      if (obj_type == OBJ_GROUP) {
         return FALSE;
      } else if (obj_type==OBJ_ICON || obj_type==OBJ_SYM) {
         /*
          * Need to check for pins.
          */
      } else {
         every_obj_is_sym_or_icon = FALSE;
      }
   }
   if (every_obj_is_sym_or_icon) {
      if (MsgBox(TgLoadString(STID_ALL_SIMPLE_GROUP_UNGROUP_ANY), TOOL_NAME,
            YNC_MB) == MB_ID_YES) {
         *pn_force_ungroup = TRUE;
         return FALSE;
      }
   }
   return TRUE;
}

void UngroupSelObj(highlight, record_cmd)
   int highlight, record_cmd;
{
   struct SelRec *sel_ptr=NULL, *next_sel=NULL;
   struct ObjRec *obj_ptr=NULL;
   int sel_ltx, sel_lty, sel_rbx, sel_rby;
   int changed=FALSE, force_ungroup_single_obj=FALSE;

   if (topSel==NULL || NoObjToUngroup(&force_ungroup_single_obj)) return;

   sel_ltx = selLtX; sel_lty = selLtY;
   sel_rbx = selRbX; sel_rby = selRbY;

   if (highlight) HighLightReverse();
   if (record_cmd) StartCompositeCmd();
   if (force_ungroup_single_obj) {
      for (sel_ptr=topSel; sel_ptr != NULL; sel_ptr=sel_ptr->next) {
         struct AttrRec *attr_ptr=NULL;

         obj_ptr = sel_ptr->obj;

         switch (obj_ptr->type) {
         case OBJ_ICON:
            PrepareToReplaceAnObj(obj_ptr);
            obj_ptr->type = OBJ_GROUP;
            attr_ptr = obj_ptr->fattr;
            for ( ; attr_ptr != NULL; attr_ptr=attr_ptr->next) {
               attr_ptr->inherited = FALSE;
            }
            AdjObjBBox(obj_ptr);
            RecordReplaceAnObj(obj_ptr);
            break;
         case OBJ_SYM:
            PrepareToReplaceAnObj(obj_ptr);
            obj_ptr->type = OBJ_GROUP;
            AdjObjBBox(obj_ptr);
            RecordReplaceAnObj(obj_ptr);
            break;
         case OBJ_PIN:
            /*
             * Need to check for pins.
             */
            break;
         default: break;
         }
      }
   }
   for (sel_ptr=topSel; sel_ptr != NULL; sel_ptr=next_sel) {
      next_sel = sel_ptr->next;
      obj_ptr = sel_ptr->obj;
      if (obj_ptr->type == OBJ_GROUP) {
         int count;
         struct SelRec *tmp_sel_ptr, *tmp_top_sel=NULL, *tmp_bot_sel=NULL;

         changed = TRUE;
         if (record_cmd) PrepareToReplaceAnObj(obj_ptr);

         UngroupObj(obj_ptr, &tmp_top_sel, &tmp_bot_sel);
         if (gnDeleteAttrsWhileUngrouping) {
            DelAllAttrs(obj_ptr->fattr);
         } else {
            DetachGroupAttrs(obj_ptr, &tmp_top_sel, &tmp_bot_sel);
         }
         obj_ptr->detail.r->first->prev = obj_ptr->prev;
         if (obj_ptr->prev == NULL) {
            curPage->top = topObj = obj_ptr->detail.r->first;
         } else {
            obj_ptr->prev->next = obj_ptr->detail.r->first;
         }
         obj_ptr->detail.r->last->next = obj_ptr->next;
         if (obj_ptr->next == NULL) {
            curPage->bot = botObj = obj_ptr->detail.r->last;
         } else {
            obj_ptr->next->prev = obj_ptr->detail.r->last;
         }
         count = 0;
         for (tmp_sel_ptr=tmp_top_sel; tmp_sel_ptr!=NULL;
               tmp_sel_ptr=tmp_sel_ptr->next) {
            count++;
         }
         if (record_cmd) {
            RecordCmd(CMD_ONE_TO_MANY, NULL, tmp_top_sel, tmp_bot_sel, count);
         }
         tmp_top_sel->prev = sel_ptr->prev;
         if (sel_ptr->prev == NULL) {
            topSel = tmp_top_sel;
         } else {
            sel_ptr->prev->next = tmp_top_sel;
         }
         tmp_bot_sel->next = sel_ptr->next;
         if (sel_ptr->next == NULL) {
            botSel = tmp_bot_sel;
         } else {
            sel_ptr->next->prev = tmp_bot_sel;
         }
         free(sel_ptr);
         /*
          * What about obj_ptr->detail.r->first_conn and
          *       obj_ptr->detail.r->last_conn?
          */
         free(obj_ptr->detail.r);
         free(obj_ptr);
      }
   }
   if (record_cmd) EndCompositeCmd();
   if (changed) {
      UpdSelBBox();
      RedrawAreas(botObj,
            sel_ltx-GRID_ABS_SIZE(1), sel_lty-GRID_ABS_SIZE(1),
            sel_rbx+GRID_ABS_SIZE(1), sel_rby+GRID_ABS_SIZE(1),
            selLtX-GRID_ABS_SIZE(1), selLtY-GRID_ABS_SIZE(1),
            selRbX+GRID_ABS_SIZE(1), selRbY+GRID_ABS_SIZE(1));
      SetFileModified(TRUE);
      justDupped = FALSE;
      Msg(TgLoadString(STID_SEL_OBJ_ARE_UNGROUPED));
   }
   if (highlight) HighLightForward();
}

void LockSelObj()
{
   register struct SelRec *sel_ptr;
   register struct ObjRec *obj_ptr;
   int changed=FALSE;

   if (topSel==NULL) {
      Msg(TgLoadString(STID_NO_OBJ_TO_LOCK));
      return;
   }
   if (curChoice==VERTEXMODE) {
      Msg(TgLoadString(STID_CANNOT_LOCK_IN_VERTEX_MODE));
      return;
   }
   HighLightReverse();
   StartCompositeCmd();
   for (sel_ptr=topSel; sel_ptr != NULL; sel_ptr=sel_ptr->next) {
      obj_ptr = sel_ptr->obj;
      if (!obj_ptr->locked) {
         changed = TRUE;
         PrepareToReplaceAnObj(obj_ptr);
         obj_ptr->locked = TRUE;
         RecordReplaceAnObj(obj_ptr);
      }
   }
   EndCompositeCmd();
   HighLightForward();
   if (changed) {
      UpdSelBBox();
      SetFileModified(TRUE);
      justDupped = FALSE;
      Msg(TgLoadString(STID_SEL_OBJ_ARE_LOCKED));
   }
}

void UnlockSelObj()
{
   register struct SelRec *sel_ptr;
   register struct ObjRec *obj_ptr;
   int changed=FALSE;

   if (topSel==NULL) {
      Msg(TgLoadString(STID_NO_OBJ_TO_UNLOCK));
      return;
   }
   if (curChoice==VERTEXMODE) {
      Msg(TgLoadString(STID_CANNOT_UNLOCK_IN_VERTEX_MODE));
      return;
   }
   HighLightReverse();
   StartCompositeCmd();
   for (sel_ptr=topSel; sel_ptr != NULL; sel_ptr=sel_ptr->next) {
      obj_ptr = sel_ptr->obj;
      if (obj_ptr->locked) {
         changed = TRUE;
         PrepareToReplaceAnObj(obj_ptr);
         obj_ptr->locked = FALSE;
         RecordReplaceAnObj(obj_ptr);
      }
   }
   EndCompositeCmd();
   HighLightForward();
   if (changed) {
      UpdSelBBox();
      SetFileModified(TRUE);
      justDupped = FALSE;
      Msg(TgLoadString(STID_SEL_OBJ_ARE_UNLOCKED));
   }
}
