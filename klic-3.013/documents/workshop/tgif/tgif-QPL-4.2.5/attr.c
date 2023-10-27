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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/attr.c,v 1.15 2011/05/16 16:21:56 william Exp $
 */

#define _INCLUDE_FROM_ATTR_C_

#include "tgifdefs.h"

#include "attr.e"
#include "auxtext.e"
#include "button.e"
#include "choice.e"
#include "choose.e"
#include "cmd.e"
#include "color.e"
#include "cursor.e"
#include "dialog.e"
#include "drawing.e"
#include "dup.e"
#include "file.e"
#include "font.e"
#include "grid.e"
#include "mainloop.e"
#include "mainmenu.e"
#include "mark.e"
#include "menu.e"
#include "miniline.e"
#include "move.e"
#include "msg.e"
#include "names.e"
#include "obj.e"
#include "pattern.e"
#include "raster.e"
#include "rect.e"
#include "ruler.e"
#include "select.e"
#include "setup.e"
#include "stk.e"
#include "strtbl.e"
#include "text.e"
#include "util.e"
#include "wb.e"

int dropObsIconAttrWhenUpdate=FALSE;
int maxAttrGroups=0;

AttrGroupInfo	**gAttrGroupInfo=NULL;
EditAttrInfo *gpEditAttrInEditorAttrInfo=NULL;

static struct AttrRec *topAttr=NULL, *botAttr=NULL;

void LinkInAttr(PrevPtr, NextPtr, AttrPtr)
   struct AttrRec *PrevPtr, *NextPtr, *AttrPtr;
   /* add AttrPtr between PrevPtr and NextPtr */
{
   AttrPtr->prev = PrevPtr;
   AttrPtr->next = NextPtr;

   if (PrevPtr == NULL) {
      topAttr = AttrPtr;
   } else {
      PrevPtr->next = AttrPtr;
   }
   if (NextPtr == NULL) {
      botAttr = AttrPtr;
   } else {
      NextPtr->prev = AttrPtr;
   }
}

void FreeAttr(AttrPtr)
   struct AttrRec *AttrPtr;
   /* This routine only frees the attribute record, not   */
   /*    the text record, which must be freed explicitly. */
{
   if (AttrPtr->attr_name.s != NULL) free(AttrPtr->attr_name.s);
   if (AttrPtr->attr_value.s != NULL) free(AttrPtr->attr_value.s);
   free(AttrPtr);
}

void UnlinkAttr(AttrPtr)
   struct AttrRec *AttrPtr;
{
   struct ObjRec *own_ptr;
   struct AttrRec **top_attr_ad;
   struct AttrRec **bot_attr_ad;

   own_ptr = AttrPtr->owner;
 
   top_attr_ad = &(own_ptr->fattr);
   bot_attr_ad = &(own_ptr->lattr);
     
   if (*top_attr_ad == AttrPtr) {
      *top_attr_ad = AttrPtr->next;
   } else {
      AttrPtr->prev->next = AttrPtr->next;
   }
   if (*bot_attr_ad == AttrPtr) {
      *bot_attr_ad = AttrPtr->prev;
   } else {
      AttrPtr->next->prev = AttrPtr->prev;
   }
}

void UpdAttrOwner(first_attr, owner_obj)
   struct AttrRec *first_attr;
   struct ObjRec *owner_obj;
{
   while (first_attr != NULL) {
      first_attr->owner = owner_obj;
      first_attr = first_attr->next;
   }
}

static
char *FindEqual(s)
   char *s;
{
   while (*s != '=' && *s != '\0') s++;
   return ((*s == '=') ? (s) : (char*)NULL);
}

void ParseAttrStr(buf, name, name_sz, value, value_sz)
   char *buf, *name, *value;
   int name_sz, value_sz;
{
   char *eq_ptr=NULL, *str_ptr=NULL, *psz=NULL;

   if ((eq_ptr=FindEqual(buf)) != NULL) {
      eq_ptr++;
      if (name != NULL) {
         int index=0;

         psz = name;
         str_ptr = buf;
         for (;;) {
            *psz = *str_ptr; 
            psz++;
            str_ptr++;
            index++;
            if (str_ptr == eq_ptr) {
               break;
            } else if (index >= name_sz-2) {
               *psz++ = '=';
               break;
            }
         }
         *psz = '\0';
      }
      if (value != NULL) {
         int index=0;

         str_ptr = eq_ptr;
         psz = value;
         do {
            *psz = *str_ptr; 
            psz++;
            str_ptr++;
            index++;
         } while (*str_ptr != '\0' && index < value_sz-1);

         *psz = '\0';
      }
   } else {
      if (name != NULL) *name = '\0';
      if (value != NULL) UtilStrCpyN(value, value_sz, buf);
   }
} 

void UpdateAttr(TextPtr, AttrPtr)
   struct TextRec *TextPtr;
   struct AttrRec *AttrPtr;
   /* This routine updates the name and value in the AttrRec */
   /*   and its ObjRec after an attribute was edited.        */
{
   if (AttrPtr->nameshown) {
      char *s=GetTextPtrFirstStrSeg(TextPtr)->dyn_str.s;
      int buf_sz=(strlen(s)<<1)+5;
      char *name=(char*)malloc(buf_sz), *value=(char*)malloc(buf_sz);

      if (name == NULL || value == NULL) FailAllocMessage();
      *name = *value = '\0';
      ParseAttrStr(GetTextPtrFirstStrSeg(TextPtr)->dyn_str.s,
            name, buf_sz, value, buf_sz);
      DynStrSet(&AttrPtr->attr_value, value);
      DynStrSet(&AttrPtr->attr_name, name);
      strcat(name, value);
      DynStrSet(&GetTextPtrFirstStrSeg(TextPtr)->dyn_str, name);
      free(name);
      free(value);
   } else {
      char *value=GetTextPtrFirstStrSeg(TextPtr)->dyn_str.s;

      DynStrSet(&AttrPtr->attr_value, value);
      DynStrSet(&GetTextPtrFirstStrSeg(TextPtr)->dyn_str, value);
   }
   UpdTextBBox(AttrPtr->obj);
   AdjObjCache(AttrPtr->obj);
}

void DrawAttrs(Win, XOff, YOff, AttrPtr)
   Window Win;
   int XOff, YOff;
   struct AttrRec *AttrPtr;
{
   struct AttrRec *ptr;

   for (ptr=AttrPtr; ptr != NULL; ptr=ptr->next) {
      if (ptr->shown) {
         if (!editingText) {
            /* UpdAttr(ptr); */ /* ALERT - not sure if this is needed. */
         }
         DrawTextObj(Win, XOff, YOff, ptr->obj);
      }
   }
}

void MoveAttrs(AttrPtr, Dx, Dy)
   int Dx, Dy;
   struct AttrRec *AttrPtr;
{
   struct AttrRec *attr_ptr;

   for (attr_ptr=AttrPtr; attr_ptr != NULL; attr_ptr=attr_ptr->next) {
      MoveObj(attr_ptr->obj, Dx, Dy);
   }
}

void DelAllAttrs(AttrPtr)
   struct AttrRec *AttrPtr;
{
   struct AttrRec *ptr, *next_attr;

   for (ptr=AttrPtr; ptr != NULL; ptr=next_attr) {
      next_attr = ptr->next;
      FreeObj(ptr->obj);
      FreeAttr(ptr);
   }
}

struct AttrRec *AddAttrByNameAndValue(ObjPtr, AttrName, AttrValue)
   struct ObjRec *ObjPtr;
   char *AttrName, *AttrValue;
{
   struct ObjRec *text_obj_ptr=NULL;
   struct TextRec *text_ptr=NULL;
   struct AttrRec *attr_ptr=NULL;
   StrSegInfo *pStrSeg=NULL;
   MiniLinesInfo *minilines=NULL;
   char *buf=(char*)malloc(strlen(AttrName)+strlen(AttrValue)+1);

   if (buf == NULL) FailAllocMessage();
   sprintf(buf, "%s%s", AttrName, AttrValue);

   text_obj_ptr = (struct ObjRec *)malloc(sizeof(struct ObjRec));
   text_ptr = (struct TextRec *)malloc(sizeof(struct TextRec));
   attr_ptr = (struct AttrRec *)malloc(sizeof(struct AttrRec));
   if (text_obj_ptr == NULL || text_ptr == NULL || attr_ptr == NULL) {
      FailAllocMessage();
   }
   memset(text_obj_ptr, 0, sizeof(struct ObjRec));
   memset(text_ptr, 0, sizeof(struct TextRec));
   memset(attr_ptr, 0, sizeof(struct AttrRec));

   minilines = (&text_ptr->minilines);
   CreateMiniLineFromString("", &minilines->first, &minilines->last);
   minilines->first->owner_minilines = minilines;

   CopyCurInfoIntoTextPtr(text_obj_ptr, text_ptr);
   text_ptr->lines = 1;
   text_ptr->cached_zoom = 0;
   text_ptr->cached_zoomed = FALSE;
   text_ptr->cached_bitmap = None;

   pStrSeg = GetTextPtrFirstStrSeg(text_ptr);

   DynStrSet(&pStrSeg->dyn_str, buf);

   pStrSeg->font = curFont;
   pStrSeg->style = curStyle;
   pStrSeg->sz_unit = GetCurSzUnit();
   pStrSeg->underline_on = curUnderlineOn;
   pStrSeg->overline_on = curOverlineOn;

   pStrSeg->asc = canvasFontAsc;
   pStrSeg->des = canvasFontDes;
   pStrSeg->direction = canvasFontDirection;
   pStrSeg->dontreencode = canvasFontDontReencode;
   pStrSeg->double_byte = canvasFontDoubleByte;
   pStrSeg->double_byte_mod_bytes = canvasFontDoubleByteModBytes;
   pStrSeg->double_byte_vertical = canvasFontDoubleByteVertical;

   text_obj_ptr->x = ObjPtr->obbox.ltx;
   text_obj_ptr->y = ObjPtr->obbox.rby;
   text_obj_ptr->type = OBJ_TEXT;
   text_obj_ptr->color = ObjPtr->color;
   memcpy(text_obj_ptr->color_str, ObjPtr->color_str,
         sizeof(text_obj_ptr->color_str));
   text_obj_ptr->dirty = FALSE;
   text_obj_ptr->id = objId++;
   text_obj_ptr->rotation = 0;
   text_obj_ptr->locked = FALSE;
   text_obj_ptr->ctm = NULL;
   text_obj_ptr->detail.t = text_ptr;

   UpdTextBBox(text_obj_ptr);

   attr_ptr->shown = FALSE;
   attr_ptr->nameshown = TRUE;
   attr_ptr->inherited = FALSE;
   attr_ptr->obj = text_obj_ptr;
   attr_ptr->owner = ObjPtr;
   DynStrSet(&attr_ptr->attr_name, AttrName);
   DynStrSet(&attr_ptr->attr_value, AttrValue);

   text_ptr->attr = attr_ptr;

   attr_ptr->prev = NULL;
   attr_ptr->next = ObjPtr->fattr;
   if (ObjPtr->fattr == NULL) {
      ObjPtr->lattr = attr_ptr;
   } else {
      ObjPtr->fattr->prev = attr_ptr;
   }
   ObjPtr->fattr = attr_ptr;
   AdjObjBBox(ObjPtr);
   return attr_ptr;
}

static
struct AttrRec *NewAttr(OwnerObjPtr, ObjPtr, Inherited)
   struct ObjRec *ObjPtr;
   struct ObjRec *OwnerObjPtr;
   short Inherited;
{
   struct AttrRec *attr_ptr;
    
   attr_ptr = (struct AttrRec *)malloc(sizeof(struct AttrRec));
   if (attr_ptr == NULL) FailAllocMessage();
   memset(attr_ptr, 0, sizeof(struct AttrRec));
   attr_ptr->shown = TRUE;
   attr_ptr->nameshown = TRUE;
   attr_ptr->inherited = Inherited;
   attr_ptr->obj = ObjPtr;
   attr_ptr->next= attr_ptr->prev = NULL;
   attr_ptr->owner = OwnerObjPtr;
   DynStrSet(&attr_ptr->attr_name, "");
   DynStrSet(&attr_ptr->attr_value, "");
   ObjPtr->detail.t->attr = attr_ptr;

   return attr_ptr;
}

static
void DupAnAttr(FromAttrPtr, ToAttrPtr)
   struct AttrRec *FromAttrPtr, *ToAttrPtr;
{
   struct ObjRec *text_obj_ptr;

   text_obj_ptr = (struct ObjRec *)malloc(sizeof(struct ObjRec));
   if (text_obj_ptr == NULL) FailAllocMessage();
   memset(text_obj_ptr, 0, sizeof(struct ObjRec));
   DupObjBasics(FromAttrPtr->obj, text_obj_ptr);
   DupTextObj(FromAttrPtr->obj->detail.t, FromAttrPtr->obj, text_obj_ptr);

   DynStrCpy(&ToAttrPtr->attr_name, &FromAttrPtr->attr_name);
   DynStrCpy(&ToAttrPtr->attr_value, &FromAttrPtr->attr_value);
   ToAttrPtr->shown = FromAttrPtr->shown;
   ToAttrPtr->nameshown = FromAttrPtr->nameshown;
   ToAttrPtr->inherited = FromAttrPtr->inherited;
   ToAttrPtr->obj = text_obj_ptr;
   ToAttrPtr->next= ToAttrPtr->prev = NULL;
   text_obj_ptr->detail.t->attr = ToAttrPtr;
}

void DupAttrs(FromObjPtr, ToObjPtr)
   struct ObjRec *FromObjPtr, *ToObjPtr;
{
   struct AttrRec *to_attr_ptr, *from_attr_ptr;

   topAttr = botAttr = NULL;
   from_attr_ptr = FromObjPtr->lattr;
   for ( ; from_attr_ptr != NULL; from_attr_ptr=from_attr_ptr->prev) {
      to_attr_ptr = (struct AttrRec *)malloc(sizeof(struct AttrRec));
      if (to_attr_ptr == NULL) FailAllocMessage();
      memset(to_attr_ptr, 0, sizeof(struct AttrRec));
      to_attr_ptr->owner = ToObjPtr;
      DupAnAttr(from_attr_ptr, to_attr_ptr);
      LinkInAttr((struct AttrRec *)NULL, topAttr, to_attr_ptr);
   }
   ToObjPtr->fattr = topAttr;
   ToObjPtr->lattr = botAttr;
}

static
void AddAttr(ObjPtr, TextObjPtr)
   struct ObjRec *ObjPtr, *TextObjPtr;
{
   struct AttrRec *attr_ptr=NULL;
   struct TextRec *text_ptr=TextObjPtr->detail.t;

   topAttr = ObjPtr->fattr;
   botAttr = ObjPtr->lattr;

   UnlinkObj(TextObjPtr);
   TextObjPtr->next = TextObjPtr->prev = NULL;
   attr_ptr = NewAttr(ObjPtr, TextObjPtr, FALSE); 
   UpdateAttr(text_ptr, attr_ptr); 
   LinkInAttr((struct AttrRec *)NULL, topAttr, attr_ptr);

   ObjPtr->fattr = topAttr;
   ObjPtr->lattr = botAttr;
}

void AddAttrs()
{
   struct ObjRec *owner_ptr=NULL;
   struct SelRec *sel_ptr;
   int text_count=0, sel_ltx, sel_lty, sel_rbx, sel_rby;
   int locked_text_count=0;

   if (topSel == NULL) {
      MsgBox(TgLoadString(STID_SELECT_AT_LEAST_ONE_TEXT), TOOL_NAME, INFO_MB);
      return;
   }
   for (sel_ptr=topSel; sel_ptr != NULL; sel_ptr=sel_ptr->next) {
      switch (sel_ptr->obj->type) {
      case OBJ_TEXT: text_count++; break;

      default:
         if (owner_ptr != NULL) {
            MsgBox(TgLoadString(STID_MORE_THAN_ONE_NON_TEXT), TOOL_NAME,
                  INFO_MB);
            return;
         }
         owner_ptr = sel_ptr->obj;
         break; 
      }
   }

   if (text_count == 0) {
      MsgBox(TgLoadString(STID_NO_TEXT_SELECTED_ADD_ATTRS), TOOL_NAME,
            INFO_MB);
      return;
   }
   if (owner_ptr == NULL) {
      MsgBox(TgLoadString(STID_NO_NON_TEXT_SELECTED), TOOL_NAME, INFO_MB);
      return;
   }
   HighLightReverse();
   PrepareToRecord(CMD_REPLACE, topSel, botSel, numObjSelected);
   sel_ltx = selLtX; sel_lty = selLtY;
   sel_rbx = selRbX; sel_rby = selRbY;

   for (sel_ptr=botSel;  sel_ptr != NULL; sel_ptr=sel_ptr->prev) {
      if (sel_ptr->obj->type == OBJ_TEXT) {
         if (sel_ptr->obj->locked) {
            locked_text_count++;
            sel_ptr->obj->locked = FALSE;
         }
         AddAttr(owner_ptr, sel_ptr->obj);
      }
   }

   RemoveAllSel();
   UnlinkObj(owner_ptr);
   AddObj(NULL, topObj, owner_ptr);
   topSel = botSel = (struct SelRec *)malloc(sizeof(struct SelRec));
   if (topSel == NULL) FailAllocMessage();
   topSel->obj = owner_ptr;
   topSel->prev = NULL;
   botSel->next = NULL;
   AdjObjBBox(owner_ptr);
   UpdSelBBox();
   RecordCmd(CMD_MANY_TO_ONE, NULL, topSel, botSel, 1);
   RedrawAreas(botObj, sel_ltx-GRID_ABS_SIZE(1), sel_lty-GRID_ABS_SIZE(1),
         sel_rbx+GRID_ABS_SIZE(1), sel_rby+GRID_ABS_SIZE(1),
         selLtX-GRID_ABS_SIZE(1), selLtY-GRID_ABS_SIZE(1),
         selRbX+GRID_ABS_SIZE(1), selRbY+GRID_ABS_SIZE(1));
   HighLightForward();
   SetFileModified(TRUE);
   justDupped = FALSE;
   if (locked_text_count==0) {
      Msg(TgLoadString(STID_TEXT_ATTACHED));
   } else {
      Msg(TgLoadString(STID_TEXT_UNLOCKED_AND_ATTACHED));
   }
}

static
void SaveAttr(FP, AttrPtr)
   FILE *FP;
   struct AttrRec *AttrPtr;
{
   struct TextRec *text_ptr=AttrPtr->obj->detail.t;

   if (fprintf(FP, "attr(\"") == EOF) writeFileFailed = TRUE;
   if (GetTextPtrFirstStrSeg(text_ptr)->double_byte) {
      SaveDoubleByteString(FP, AttrPtr->attr_name.s);
   } else {
      SaveString(FP, AttrPtr->attr_name.s);
   }
   if (fprintf(FP, "\", \"") == EOF) writeFileFailed = TRUE;
   if (GetTextPtrFirstStrSeg(text_ptr)->double_byte) {
      SaveDoubleByteString(FP, AttrPtr->attr_value.s);
   } else {
      SaveString(FP, AttrPtr->attr_value.s);
   }
   if (fprintf(FP, "\", %1d, %1d, %1d,\n",
         AttrPtr->shown, AttrPtr->nameshown, AttrPtr->inherited) == EOF) {
      writeFileFailed = TRUE;
   }
   SaveTextObj(FP, AttrPtr->obj);
   if (fprintf(FP, ")") == EOF) writeFileFailed = TRUE;
}

void SaveAttrs(FP, BotAttrPtr)
   FILE *FP;
   struct AttrRec *BotAttrPtr;
{
   struct AttrRec *ptr;

   if (fprintf(FP, "[\n") == EOF) writeFileFailed = TRUE;

   for (ptr=BotAttrPtr; ptr != NULL; ptr=ptr->prev) {
      SaveAttr(FP, ptr);
      if (ptr->prev != NULL) {
         if (fprintf(FP, ",\n") == EOF) {
            writeFileFailed = TRUE;
         }
      }
   }
   if (BotAttrPtr == NULL) {
      if (fprintf(FP, "]") == EOF) writeFileFailed = TRUE;
   } else {
      if (fprintf(FP, "\n]") == EOF) writeFileFailed = TRUE;
   }
}

char *ReadAttrString(Str)
   char *Str;
{
   char *s;

   for (s=Str; *s != '\0'; s++) {
      if (*s == '"') {
         if (s[1] == '"') {
            s++;
         } else {
            break;
         }
      } else if (*s == '\\') {
         s++;
      }
   }
   if (*s == '"') s++;
   return s;
}

static
int FreeBufAndReturn(line, rc)
   char *line;
   int rc;
{
   if (line != NULL) free(line);
   return rc;
}

int ReadAttr(FP, AttrPtr)
   FILE *FP;
   struct AttrRec **AttrPtr;
{
   struct ObjRec *TextObj;
   char *s, *line;
   char *name, *value;
   int len, shown, nameshown, inherited;
 
   if ((line=UtilGetALine(FP)) == NULL) {
      sprintf(gszMsgBox, TgLoadString(STID_UNEXPECTED_EOF_IN_ABORT_READ),
            scanFileName, scanLineNum, "ReadAttr()");
      if (PRTGIF) {
         fprintf(stderr, "%s\n", gszMsgBox);
      } else {
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      }
      return FALSE;
   }
   scanLineNum++;

   if (*line == ']') return FreeBufAndReturn(line, FALSE);

   *AttrPtr = NULL;
   len = strlen(line);

   name = (char *)malloc((len+1)*sizeof(char));
   if (name == NULL) FailAllocMessage();
   s = FindChar((int)'"', line);
   strcpy(name, s);
   s = ReadAttrString(s);
   s = FindChar((int)',', s);
   value = (char *)malloc((len+1)*sizeof(char));
   if (value == NULL) FailAllocMessage();
   strcpy(value, FindChar((int)'"', s));
   s = ReadAttrString(value);
   s = FindChar((int)',', s);

   InitScan(s, "\t\n ,");
   if (GETINT("attribute", shown, "shown") == INVALID ||
       GETINT("attribute", nameshown, "name shown") == INVALID ||
       GETINT("attribute", inherited, "inherited") == INVALID) {
      free(name);
      free(value);
      return FreeBufAndReturn(line, FALSE);
   }

   *AttrPtr = (struct AttrRec *)malloc(sizeof(struct AttrRec)); 
   if (*AttrPtr == NULL) FailAllocMessage();
   memset(*AttrPtr, 0, sizeof(struct AttrRec));

   s = ReadString(name);
   *(--s) = '\0';
   DynStrSet(&((*AttrPtr)->attr_name), name);
   s = ReadString(value);
   *(--s) = '\0';
   DynStrSet(&((*AttrPtr)->attr_value), value);

   free(name);
   free(value);

   (*AttrPtr)->shown = shown;
   (*AttrPtr)->nameshown = nameshown;
   (*AttrPtr)->inherited = inherited;

   if (ReadObj(FP, &TextObj) == FALSE) {
     free(*AttrPtr);
     *AttrPtr = NULL;
     return FreeBufAndReturn(line, FALSE);
   }
   TextObj->detail.t->attr = *AttrPtr;
   (*AttrPtr)->obj = TextObj;

   return FreeBufAndReturn(line, TRUE);
}

struct ObjRec *FindTopLevelObjWithType(type_str)
   char *type_str;
{
   struct ObjRec *obj_ptr=NULL;

   for (obj_ptr=botObj; obj_ptr != NULL; obj_ptr=obj_ptr->prev) {
      struct AttrRec *attr_ptr=NULL;

      for (attr_ptr=obj_ptr->fattr; attr_ptr != NULL; attr_ptr=attr_ptr->next) {
         if (strcmp(attr_ptr->attr_name.s, "type=") == 0 &&
               strcmp(attr_ptr->attr_value.s, type_str) == 0) {
            return obj_ptr;
         }
      }
   }
   return NULL;
}

static
int ObjMightChangeForShowAndUpdAttrNames(ObjPtr)
   struct ObjRec *ObjPtr;
{
   struct AttrRec *attr_ptr;

   if ((attr_ptr=ObjPtr->fattr) != NULL) {
      for ( ; attr_ptr != NULL; attr_ptr = attr_ptr->next) {
         if (!(attr_ptr->nameshown)) {
            return TRUE;
         }
      }
   }
   return FALSE;
}

static
void UpdateFirstTextStringWithAttrNameAndValue(ObjPtr, AttrPtr)
   struct ObjRec *ObjPtr;
   struct AttrRec *AttrPtr;
{
   char *buf=(char*)malloc(AttrPtr->attr_name.sz+AttrPtr->attr_value.sz);

   /* one extra char allocated, no big deal */
   if (buf == NULL) FailAllocMessage();
   sprintf(buf, "%s%s", AttrPtr->attr_name.s, AttrPtr->attr_value.s);
   DynStrSet(&GetTextObjFirstStrSeg(ObjPtr)->dyn_str, buf);
   free(buf);
}

static
int DoShowAndUpdAttrNames(ObjPtr, Force)
   struct ObjRec *ObjPtr;
   int Force;
   /* Force will force attribute name to be shown whether the attribute */
   /*    is inherited or not.                                           */
{
   struct AttrRec *attr_ptr=ObjPtr->fattr;
   int picture_changed=FALSE;

   for ( ; attr_ptr != NULL; attr_ptr=attr_ptr->next) {
      if (!(attr_ptr->nameshown) && (Force || !(attr_ptr->inherited))) {
         UpdateFirstTextStringWithAttrNameAndValue(attr_ptr->obj, attr_ptr);

         attr_ptr->nameshown = TRUE;
         UpdTextBBox(attr_ptr->obj);
         if (attr_ptr->shown) {
            picture_changed = TRUE;

            if (attr_ptr->obj->detail.t->cached_bitmap != None) {
               XFreePixmap(mainDisplay,
                     attr_ptr->obj->detail.t->cached_bitmap);
            }
            attr_ptr->obj->detail.t->cached_zoom = 0;
            attr_ptr->obj->detail.t->cached_bitmap = None;
         }
      }
   }
   AdjObjBBox(ObjPtr);

   return picture_changed;
}

static
int ShowAndUpdAttrNames()
   /* returns TRUE if any attribute names are updated                   */
   /* This routine concatinate the 'name' and 's' first of every        */
   /*    attribute of the selected object and assign that to the        */
   /*    first line of the text object the attribute pointer points to. */
{
   struct SelRec *sel_ptr;
   int picture_changed=FALSE;

   for (sel_ptr=topSel; sel_ptr != NULL; sel_ptr=sel_ptr->next) {
      if (ObjMightChangeForShowAndUpdAttrNames(sel_ptr->obj)) {
         PrepareToReplaceAnObj(sel_ptr->obj);
         picture_changed = DoShowAndUpdAttrNames(sel_ptr->obj, TRUE);
         RecordReplaceAnObj(sel_ptr->obj);
      }
   }
   return picture_changed;
}

void ShowAllAttrNames()
{
   HighLightReverse();
   StartCompositeCmd();
   if (ShowAndUpdAttrNames()) {
      UpdSelBBox();
      RedrawAnArea(botObj, selLtX-GRID_ABS_SIZE(1),
            selLtY-GRID_ABS_SIZE(1), selRbX+GRID_ABS_SIZE(1),
            selRbY+GRID_ABS_SIZE(1));
      SetFileModified(TRUE);
   }
   EndCompositeCmd();
   HighLightForward();
}

static
int ShowAndUpdAttrs()
   /* returns TRUE if any attribute was not shown before */
{
   struct SelRec *sel_ptr;
   struct ObjRec *obj_ptr;
   struct AttrRec *attr_ptr;
   int picture_changed=FALSE;

   for (sel_ptr=topSel; sel_ptr != NULL; sel_ptr=sel_ptr->next) {
      obj_ptr = sel_ptr->obj;
      attr_ptr = obj_ptr->fattr;
      if (attr_ptr != NULL) {
         int obj_changed=FALSE;

         for ( ; attr_ptr != NULL; attr_ptr=attr_ptr->next) {
            if (!attr_ptr->shown) {
               obj_changed = TRUE;
               break;
            }
         }
         if (obj_changed) {
            PrepareToReplaceAnObj(obj_ptr);
            for (attr_ptr=obj_ptr->fattr; attr_ptr!=NULL;
                  attr_ptr=attr_ptr->next) {
               if (!attr_ptr->shown) {
                  attr_ptr->shown = TRUE;
               }
            }
            picture_changed = TRUE;
            AdjObjBBox(obj_ptr);
            RecordReplaceAnObj(obj_ptr);
         }
      }
   }
   return picture_changed;
}

void ShowAllAttrs()
{
   HighLightReverse();
   StartCompositeCmd();
   if (ShowAndUpdAttrs()) {
      UpdSelBBox();
      RedrawAnArea(botObj, selLtX-GRID_ABS_SIZE(1),
            selLtY-GRID_ABS_SIZE(1), selRbX+GRID_ABS_SIZE(1),
            selRbY+GRID_ABS_SIZE(1));
      SetFileModified(TRUE);
   }
   EndCompositeCmd();
   HighLightForward();
}

static
int HideAndUpdAttrs()
   /* returns TRUE if any attribute was shown */
{
   struct SelRec *sel_ptr;
   struct ObjRec *obj_ptr;
   struct AttrRec *attr_ptr;
   int picture_changed=FALSE;

   for (sel_ptr=topSel; sel_ptr != NULL; sel_ptr=sel_ptr->next) {
      obj_ptr = sel_ptr->obj;
      attr_ptr = obj_ptr->fattr;
      if (attr_ptr != NULL) {
         int obj_changed= FALSE;

         for ( ; attr_ptr != NULL; attr_ptr=attr_ptr->next) {
            if (attr_ptr->shown) {
               obj_changed = TRUE;
               break;
            }
         }
         if (obj_changed) {
            PrepareToReplaceAnObj(obj_ptr);
            for (attr_ptr=obj_ptr->fattr; attr_ptr!=NULL;
                  attr_ptr=attr_ptr->next) {
               if (attr_ptr->shown) {
                  attr_ptr->shown = FALSE;
               }
            }
            picture_changed = TRUE;
            AdjObjBBox(obj_ptr);
            RecordReplaceAnObj(obj_ptr);
         }
      }
   }
   return picture_changed;
}

void HideAllAttrs()
{
   int sel_ltx, sel_lty, sel_rbx, sel_rby;

   sel_ltx = selLtX; sel_lty = selLtY; sel_rbx = selRbX; sel_rby = selRbY;

   HighLightReverse();
   StartCompositeCmd();
   if (HideAndUpdAttrs()) {
      UpdSelBBox();
      RedrawAnArea(botObj, sel_ltx-GRID_ABS_SIZE(1),
            sel_lty-GRID_ABS_SIZE(1), sel_rbx+GRID_ABS_SIZE(1),
            sel_rby+GRID_ABS_SIZE(1));
      SetFileModified(TRUE);
   }
   EndCompositeCmd();
   HighLightForward();
}

static
int HideAndUpdAttrNames()
   /* returns TRUE if any attribute names are updated */
   /* For all the first line of the selected object's attributes,    */
   /*    this routine change them to the 's' field of the attribute. */
{
   struct SelRec *sel_ptr;
   struct ObjRec *obj_ptr;
   struct AttrRec *attr_ptr;
   int picture_changed=FALSE;

   for (sel_ptr=topSel; sel_ptr != NULL; sel_ptr=sel_ptr->next) {
      obj_ptr = sel_ptr->obj;
      attr_ptr = obj_ptr->fattr;
      if (attr_ptr != NULL) {
         int obj_change=FALSE;

         for ( ; attr_ptr != NULL; attr_ptr=attr_ptr->next) {
            if (attr_ptr->nameshown && *(attr_ptr->attr_name.s) != '\0') {
               obj_change = TRUE;
               break;
            }
         }
         if (obj_change) {
            PrepareToReplaceAnObj(obj_ptr);
            for (attr_ptr=obj_ptr->fattr; attr_ptr!=NULL;
                  attr_ptr=attr_ptr->next) {
               if (attr_ptr->nameshown && *(attr_ptr->attr_name.s) != '\0') {
                  struct TextRec *text_ptr=attr_ptr->obj->detail.t;

                  attr_ptr->nameshown = FALSE;
                  DynStrCpy(
                        &GetTextPtrFirstStrSeg(text_ptr)->dyn_str,
                        &attr_ptr->attr_value);
                  UpdTextBBox(attr_ptr->obj);
                  if (attr_ptr->shown) {
                     picture_changed = TRUE;

                     if (attr_ptr->obj->detail.t->cached_bitmap != None) {
                        XFreePixmap(mainDisplay,
                              attr_ptr->obj->detail.t->cached_bitmap);
                     }
                     attr_ptr->obj->detail.t->cached_zoom = 0;
                     attr_ptr->obj->detail.t->cached_bitmap = None;
                  }
               }
            }
            AdjObjBBox(obj_ptr);
            RecordReplaceAnObj(obj_ptr);
         }
      }
   }
   return picture_changed;
}

void HideAllAttrNames()
{
   int sel_ltx, sel_lty, sel_rbx, sel_rby;

   sel_ltx = selLtX; sel_lty = selLtY; sel_rbx = selRbX; sel_rby = selRbY;

   HighLightReverse();
   StartCompositeCmd();
   if (HideAndUpdAttrNames()) {
      UpdSelBBox();
      RedrawAnArea(botObj, sel_ltx-GRID_ABS_SIZE(1),
            sel_lty-GRID_ABS_SIZE(1), sel_rbx+GRID_ABS_SIZE(1),
            sel_rby+GRID_ABS_SIZE(1));
      SetFileModified(TRUE);
   }
   EndCompositeCmd();
   HighLightForward();
}

void DetachGroupAttrs(ObjPtr, TopSelPtr, BotSelPtr)
   struct ObjRec *ObjPtr;
   struct SelRec ** TopSelPtr, **BotSelPtr;
{
   struct AttrRec *attr_ptr, *next_attr;
   struct SelRec *new_sel_ptr;

   for (attr_ptr=ObjPtr->fattr; attr_ptr!=NULL; attr_ptr=next_attr) {
      next_attr = attr_ptr->next;
      if (!(attr_ptr->nameshown)) {
         UpdateFirstTextStringWithAttrNameAndValue(attr_ptr->obj, attr_ptr);
         UpdTextBBox(attr_ptr->obj);
      }

      attr_ptr->obj->detail.t->attr = NULL;

      attr_ptr->obj->prev = NULL;
      attr_ptr->obj->next = ObjPtr->detail.r->first;

      if (attr_ptr->obj->next == NULL) {
         ObjPtr->detail.r->last = attr_ptr->obj;
      } else {
         attr_ptr->obj->next->prev = attr_ptr->obj;
      }
      ObjPtr->detail.r->first = attr_ptr->obj;

      new_sel_ptr = (struct SelRec *)malloc(sizeof(struct SelRec));
      if (new_sel_ptr == NULL) FailAllocMessage();
      new_sel_ptr->obj = attr_ptr->obj;

      new_sel_ptr->prev = NULL;
      new_sel_ptr->next = *TopSelPtr;

      if (new_sel_ptr->next == NULL) {
         *BotSelPtr = new_sel_ptr;
      } else {
         (*TopSelPtr)->prev = new_sel_ptr;
      }
      *TopSelPtr = new_sel_ptr;

      FreeAttr(attr_ptr);
   }
}

void DetachAllObjAttrs(obj_ptr, TopSelPtr, BotSelPtr)
   struct ObjRec *obj_ptr;
   struct SelRec **TopSelPtr, **BotSelPtr;
{
   struct AttrRec *attr_ptr, *prev_attr;

   DoShowAndUpdAttrNames(obj_ptr, FALSE);

   (*TopSelPtr) = (*BotSelPtr) = (struct SelRec *)malloc(sizeof(struct SelRec));
   if (*TopSelPtr == NULL) FailAllocMessage();
   (*TopSelPtr)->next = (*TopSelPtr)->prev = NULL;
   (*TopSelPtr)->obj = obj_ptr;

   topAttr = botAttr = NULL;
   for (attr_ptr=obj_ptr->lattr; attr_ptr!=NULL; attr_ptr=prev_attr) {
      struct SelRec *tmp_sel_ptr;

      prev_attr = attr_ptr->prev;

      tmp_sel_ptr = (struct SelRec *)malloc(sizeof(struct SelRec));
      if (tmp_sel_ptr == NULL) FailAllocMessage();

      tmp_sel_ptr->prev = NULL;
      tmp_sel_ptr->next = (*TopSelPtr);
      (*TopSelPtr)->prev = tmp_sel_ptr;
      (*TopSelPtr) = tmp_sel_ptr;
      tmp_sel_ptr->obj = attr_ptr->obj;

      attr_ptr->obj->detail.t->attr = NULL;
      AddObj(obj_ptr->prev, obj_ptr, attr_ptr->obj);
      FreeAttr(attr_ptr);
   }
   obj_ptr->fattr = obj_ptr->lattr = NULL;
}

void DetachAttrs()
{
   struct SelRec *sel_ptr, *new_sel_ptr;
   struct ObjRec *obj_ptr;
   struct AttrRec *attr_ptr, *next_attr;
   int picture_changed=FALSE, name_changed=FALSE;

   HighLightReverse();
   StartCompositeCmd();

   for (sel_ptr=topSel; sel_ptr != NULL; sel_ptr=sel_ptr->next) {
      obj_ptr = sel_ptr->obj;
      attr_ptr = obj_ptr->fattr;
      if (attr_ptr != NULL) {
         int obj_change=FALSE;

         for ( ; attr_ptr != NULL; attr_ptr=attr_ptr->next) {
            if (!attr_ptr->inherited) {
               obj_change = TRUE;
               break;
            }
         }
         if (obj_change) {
            struct SelRec *tmp_top_sel, *tmp_bot_sel;
            struct SelRec *tmp_sel_ptr, *next_sel;
            int count;

            PrepareToReplaceAnObj(obj_ptr);
            if (DoShowAndUpdAttrNames(obj_ptr, FALSE)) name_changed = TRUE;

            tmp_top_sel = tmp_bot_sel = (struct SelRec *)malloc(
                  sizeof(struct SelRec));
            if (tmp_top_sel == NULL) FailAllocMessage();
            tmp_top_sel->next = tmp_top_sel->prev = NULL;
            tmp_top_sel->obj = obj_ptr;
            count = 1;

            topAttr = botAttr = NULL;
            for (attr_ptr=obj_ptr->fattr; attr_ptr!=NULL; attr_ptr=next_attr) {
               next_attr = attr_ptr->next;
               if (attr_ptr->inherited &&
                     (obj_ptr->type == OBJ_ICON || obj_ptr->type == OBJ_PIN)) {
                  LinkInAttr((struct AttrRec *)NULL, topAttr, attr_ptr);
                  continue;
               }
               picture_changed = TRUE;

               tmp_sel_ptr = (struct SelRec *)malloc(sizeof(struct SelRec));
               if (tmp_sel_ptr == NULL) FailAllocMessage();
               tmp_sel_ptr->next = tmp_bot_sel;
               tmp_sel_ptr->obj = attr_ptr->obj;
               if (tmp_top_sel == tmp_bot_sel) {
                  tmp_sel_ptr->prev = NULL;
                  tmp_top_sel->prev = tmp_sel_ptr;
                  tmp_top_sel = tmp_sel_ptr;
               } else {
                  tmp_sel_ptr->prev = tmp_bot_sel->prev;
                  tmp_bot_sel->prev->next = tmp_sel_ptr;
                  tmp_bot_sel->prev = tmp_sel_ptr;
               }
               count++;

               attr_ptr->obj->detail.t->attr = NULL;
               AddObj(obj_ptr->prev, obj_ptr, attr_ptr->obj);
               new_sel_ptr = (struct SelRec *)malloc(sizeof(struct SelRec));
               if (new_sel_ptr == NULL) FailAllocMessage();
               new_sel_ptr->obj = obj_ptr->prev;
               AddSel(sel_ptr->prev, sel_ptr, new_sel_ptr);
               FreeAttr(attr_ptr);
            }
            obj_ptr->fattr = topAttr;
            obj_ptr->lattr = botAttr;
            AdjObjBBox(obj_ptr);
            RecordCmd(CMD_ONE_TO_MANY, NULL, tmp_top_sel, tmp_bot_sel, count);

            for (tmp_sel_ptr=tmp_top_sel; tmp_sel_ptr!=NULL;
                  tmp_sel_ptr=next_sel) {
               next_sel = tmp_sel_ptr->next;
               free(tmp_sel_ptr);
            }
         }
      }
   }
   EndCompositeCmd();
   UpdSelBBox();
   if (picture_changed || name_changed) {
      RedrawAnArea(botObj, selLtX-GRID_ABS_SIZE(1),
            selLtY-GRID_ABS_SIZE(1), selRbX+GRID_ABS_SIZE(1),
            selRbY+GRID_ABS_SIZE(1));
      SetFileModified(TRUE);
   }
   HighLightForward();
}

void UpdAttr(AttrPtr)
   struct AttrRec *AttrPtr;
   /* Update the text object's string value associated with AttrPtr */
{
   if (AttrPtr->nameshown) {
      UpdateFirstTextStringWithAttrNameAndValue(AttrPtr->obj, AttrPtr);
   } else {
      struct TextRec *text_ptr=AttrPtr->obj->detail.t;

      DynStrCpy(&GetTextPtrFirstStrSeg(text_ptr)->dyn_str,
            &AttrPtr->attr_value);
   }
   UpdTextBBox(AttrPtr->obj);
}

static
int MoveOneAttr(ObjPtr, AttrPtr)
   struct ObjRec *ObjPtr;
   struct AttrRec *AttrPtr;
{
   struct ObjRec *text_obj_ptr;
   int x, y, grid_x, grid_y, dx, dy, placing=TRUE;
   int ltx, lty, rbx, rby;
   int orig_x, orig_y, grid_orig_x, grid_orig_y;
   char buf[80], x_buf[80], y_buf[80];
   XEvent input, ev;

   text_obj_ptr = AttrPtr->obj;
   Msg(TgLoadString(STID_MOVE_ONE_ATTR_MOUSE_MSG));
   SetMouseStatus(TgLoadString(STID_SHOW_MOVE_ATTR),
         TgLoadString(STID_TOGGLE_NAME_SHOWN), TgLoadString(STID_HIDE_ATTR));

   orig_x = OFFSET_X(text_obj_ptr->x);
   orig_y = OFFSET_Y(text_obj_ptr->y);
   GridXY(orig_x, orig_y, &grid_orig_x, &grid_orig_y);
   ltx = OFFSET_X(text_obj_ptr->bbox.ltx);
   lty = OFFSET_Y(text_obj_ptr->bbox.lty);
   rbx = OFFSET_X(text_obj_ptr->bbox.rbx)+1;
   rby = OFFSET_Y(text_obj_ptr->bbox.rby)+1;

   if (!debugNoPointerGrab) {
      XGrabPointer(mainDisplay, drawWindow, FALSE,
            PointerMotionMask | ButtonPressMask,
            GrabModeAsync, GrabModeAsync, None, handCursor, CurrentTime);
   }
   XWarpPointer(mainDisplay, None, drawWindow, 0, 0, 0, 0, orig_x, orig_y);

   dx = dy = 0;
   grid_x = grid_orig_x; grid_y = grid_orig_y;

   SelBox(drawWindow, revDefaultGC, ltx+dx, lty+dy, rbx+dx, rby+dy);
   PixelToMeasurementUnit(x_buf, 0);
   PixelToMeasurementUnit(y_buf, 0);
   sprintf(buf, "dx=%s\ndy=%s", x_buf, y_buf);
   StartShowMeasureCursor(ltx, lty-(defaultFontHeight<<1), buf, TRUE);
   while (placing) {
      XNextEvent(mainDisplay, &input);

      if (input.type == Expose || input.type == VisibilityNotify) {
         PixelToMeasurementUnit(x_buf, ABS_SIZE(dx));
         PixelToMeasurementUnit(y_buf, ABS_SIZE(dy));
         sprintf(buf, "dx=%s\ndy=%s", x_buf, y_buf);
         ShowMeasureCursor(ltx+dx, lty-(defaultFontHeight<<1)+dy, buf, TRUE);
         SelBox(drawWindow, revDefaultGC, ltx+dx, lty+dy, rbx+dx, rby+dy);
         ExposeEventHandler(&input, TRUE);
         SelBox(drawWindow, revDefaultGC, ltx+dx, lty+dy, rbx+dx, rby+dy);
         PixelToMeasurementUnit(x_buf, ABS_SIZE(dx));
         PixelToMeasurementUnit(y_buf, ABS_SIZE(dy));
         sprintf(buf, "dx=%s\ndy=%s", x_buf, y_buf);
         ShowMeasureCursor(ltx+dx, lty-(defaultFontHeight<<1)+dy, buf, TRUE);
      } else if (input.type == MotionNotify) {
         x = input.xmotion.x;
         y = input.xmotion.y;
         GridXY(x, y, &grid_x, &grid_y);

         if (grid_x != grid_orig_x+dx || grid_y != grid_orig_y+dy) {
            PixelToMeasurementUnit(x_buf, ABS_SIZE(dx));
            PixelToMeasurementUnit(y_buf, ABS_SIZE(dy));
            sprintf(buf, "dx=%s\ndy=%s", x_buf, y_buf);
            ShowMeasureCursor(ltx+dx, lty-(defaultFontHeight<<1)+dy, buf, TRUE);
            SelBox(drawWindow, revDefaultGC, ltx+dx, lty+dy, rbx+dx, rby+dy);
            dx = grid_x - grid_orig_x;
            dy = grid_y - grid_orig_y;
            SelBox(drawWindow, revDefaultGC, ltx+dx, lty+dy, rbx+dx, rby+dy);
            PixelToMeasurementUnit(x_buf, ABS_SIZE(dx));
            PixelToMeasurementUnit(y_buf, ABS_SIZE(dy));
            sprintf(buf, "dx=%s\ndy=%s", x_buf, y_buf);
            ShowMeasureCursor(ltx+dx, lty-(defaultFontHeight<<1)+dy, buf, TRUE);
            MarkRulers(grid_x, grid_y);
         }
         while (XCheckMaskEvent(mainDisplay, PointerMotionMask, &ev)) ;
      } else if (input.type == ButtonPress) {
         XUngrabPointer(mainDisplay, CurrentTime);
         XSync(mainDisplay, False);
         placing = FALSE;
         SelBox(drawWindow, revDefaultGC, ltx+dx, lty+dy, rbx+dx, rby+dy);
         PixelToMeasurementUnit(x_buf, ABS_SIZE(dx));
         PixelToMeasurementUnit(y_buf, ABS_SIZE(dy));
         sprintf(buf, "dx=%s\ndy=%s", x_buf, y_buf);
         EndShowMeasureCursor(ltx+dx, lty-(defaultFontHeight<<1)+dy, buf, TRUE);
         Msg("");
         SetMouseStatus("", "", "");
         switch (input.xbutton.button) {
         case Button1:
            if (dx != 0 || dy != 0) {
               if (ObjPtr->locked) {
                  MsgBox(TgLoadString(STID_CANNOT_MOVE_ATTR_IF_LOCKED),
                        TOOL_NAME, INFO_MB);
                  return FALSE;
               }
               HighLightReverse();
               PrepareToReplaceAnObj(ObjPtr);
               AttrPtr->shown = TRUE;
               MoveObj(text_obj_ptr, ABS_SIZE(dx), ABS_SIZE(dy));
               AdjObjBBox(ObjPtr);
               RecordReplaceAnObj(ObjPtr);
               return TRUE;
            } else if (!(AttrPtr->shown)) {
               HighLightReverse();
               PrepareToReplaceAnObj(ObjPtr);
               AttrPtr->shown = TRUE;
               AdjObjBBox(ObjPtr);
               RecordReplaceAnObj(ObjPtr);
               return TRUE;
            }
            return FALSE;
         case Button2:
            if (*(AttrPtr->attr_name.s) != '\0') {
               PrepareToReplaceAnObj(ObjPtr);
            }
            if (!(AttrPtr->nameshown && *(AttrPtr->attr_name.s)=='\0')) {
               AttrPtr->nameshown = !AttrPtr->nameshown;
            }
            UpdAttr(AttrPtr);
            if (AttrPtr->shown) {
               HighLightReverse();
               AdjObjCache(ObjPtr);
               AdjObjBBox(ObjPtr);
               if (*(AttrPtr->attr_name.s) != '\0') {
                  RecordReplaceAnObj(ObjPtr);
               }
               return TRUE;
            }
            if (*(AttrPtr->attr_name.s) != '\0') RecordReplaceAnObj(ObjPtr);
            return FALSE;
         case Button3:
            if (AttrPtr->shown) {
               HighLightReverse();
               PrepareToReplaceAnObj(ObjPtr);
               AttrPtr->shown = FALSE;
               AdjObjBBox(ObjPtr);
               RecordReplaceAnObj(ObjPtr);
               return TRUE;
            }
            return FALSE;
         default: break;
         }
      }
   }
   return FALSE;
}

static
int ChangeAttrJust(ObjPtr, AttrPtr)
   struct ObjRec *ObjPtr;
   struct AttrRec *AttrPtr;
{
   struct ObjRec *text_obj_ptr;
   int x, y, grid_x, grid_y, dx, dy, placing=TRUE;
   int ltx, lty, rbx, rby;
   int orig_x, orig_y, grid_orig_x, grid_orig_y;
   int old_just, new_just=0;
   XEvent input, ev;

   text_obj_ptr = AttrPtr->obj;
   Msg(TgLoadString(STID_CHANGE_ATTR_JUST_MOUSE_MSG));
   SetMouseStatus(TgLoadString(STID_LEFT_JUSTIFY),
         TgLoadString(STID_CENTER_JUSTIFY), TgLoadString(STID_RIGHT_JUSTIFY));

   orig_x = OFFSET_X(text_obj_ptr->x);
   orig_y = OFFSET_Y(text_obj_ptr->y);
   GridXY(orig_x, orig_y, &grid_orig_x, &grid_orig_y);
   ltx = OFFSET_X(text_obj_ptr->bbox.ltx);
   lty = OFFSET_Y(text_obj_ptr->bbox.lty);
   rbx = OFFSET_X(text_obj_ptr->bbox.rbx)+1;
   rby = OFFSET_Y(text_obj_ptr->bbox.rby)+1;

   if (!debugNoPointerGrab) {
      XGrabPointer(mainDisplay, drawWindow, FALSE,
            PointerMotionMask | ButtonPressMask,
            GrabModeAsync, GrabModeAsync, None, handCursor, CurrentTime);
   }
   XWarpPointer(mainDisplay, None, drawWindow, 0, 0, 0, 0, orig_x, orig_y);

   dx = dy = 0;
   grid_x = grid_orig_x; grid_y = grid_orig_y;

   SelBox(drawWindow, revDefaultGC, ltx+dx, lty+dy, rbx+dx, rby+dy);
   StartShowMeasureCursor(ltx, lty, NULL, TRUE);
   while (placing) {
      XNextEvent(mainDisplay, &input);

      if (input.type == Expose || input.type == VisibilityNotify) {
         SelBox(drawWindow, revDefaultGC, ltx+dx, lty+dy, rbx+dx, rby+dy);
         ExposeEventHandler(&input, TRUE);
         SelBox(drawWindow, revDefaultGC, ltx+dx, lty+dy, rbx+dx, rby+dy);
      } else if (input.type == MotionNotify) {
         x = input.xmotion.x;
         y = input.xmotion.y;
         GridXY(x, y, &grid_x, &grid_y);

         if (grid_x != grid_orig_x+dx || grid_y != grid_orig_y+dy) {
            SelBox(drawWindow, revDefaultGC, ltx+dx, lty+dy, rbx+dx, rby+dy);
            dx = grid_x - grid_orig_x;
            dy = grid_y - grid_orig_y;
            SelBox(drawWindow, revDefaultGC, ltx+dx, lty+dy, rbx+dx, rby+dy);
            MarkRulers(grid_x, grid_y);
         }
         while (XCheckMaskEvent(mainDisplay, PointerMotionMask, &ev)) ;
      } else if (input.type == ButtonPress) {
         XUngrabPointer(mainDisplay, CurrentTime);
         XSync(mainDisplay, False);
         placing = FALSE;
         SelBox(drawWindow, revDefaultGC, ltx+dx, lty+dy, rbx+dx, rby+dy);
         EndShowMeasureCursor(ltx+dx, lty+dy, NULL, TRUE);
         Msg("");
         SetMouseStatus("", "", "");
         old_just = text_obj_ptr->detail.t->minilines.just;
         switch (input.xbutton.button) {
         case Button1: new_just = JUST_L; break;
         case Button2: new_just = JUST_C; break;
         case Button3: new_just = JUST_R; break;
         }
         if (old_just != new_just) {
            HighLightReverse();
            PrepareToReplaceAnObj(ObjPtr);
            text_obj_ptr->detail.t->minilines.just = new_just;
            /* UpdAttr(AttrPtr); */ /* ALERT - not sure if this is needed. */
            AdjObjCache(ObjPtr);
            UpdTextBBox(text_obj_ptr);
            AdjObjBBox(ObjPtr);
            RecordReplaceAnObj(ObjPtr);
            return TRUE;
         }
      }
   }
   return FALSE;
}

TgMenu *CreateAttrMenu(parent_menu, x, y, num_items, menu_strings,
      status_strings, pixels)
   TgMenu *parent_menu;
   int x, y, num_items, *pixels;
   char **menu_strings, **status_strings;
{
   int i=0;
   TgMenu *menu=NULL;
   TgMenuInfo stMenuInfo;
   TgMenuItemInfo *item_info=NULL;

   memset(&stMenuInfo, 0, sizeof(TgMenuInfo));
   stMenuInfo.type = TGMUTYPE_TEXT;
   stMenuInfo.items = (TgMenuItemInfo*)malloc(
         (num_items+1)*sizeof(TgMenuItemInfo));
   if (stMenuInfo.items == NULL) FailAllocMessage();
   memset(stMenuInfo.items, 0, (num_items+1)*sizeof(TgMenuItemInfo));
   for (item_info=stMenuInfo.items, i=0; i < num_items; item_info++, i++) {
      if (menu_strings[i] == TGMUITEM_SEPARATOR) {
         item_info->menu_str = TGMUITEM_SEPARATOR;
      } else {
         item_info->menu_str = UtilStrDup(menu_strings[i]);
         if (item_info->menu_str == NULL) FailAllocMessage();
      }
      if (status_strings != NULL && status_strings[i] != NULL) {
         item_info->status_str = UtilStrDup(status_strings[i]);
         if (item_info->status_str == NULL) FailAllocMessage();
      }
      item_info->submenu_info = NULL;
      item_info->cmdid = INVALID;
   }
   stMenuInfo.items[num_items].cmdid = INVALID;

   if (num_items >= menuRowsBeforeScroll) {
      stMenuInfo.type |= TGMUTYPE_CANSCROLL;
   }
   /*
    * Since these are real attribute names and values, no translation is needed.
    */
   menu = TgCreateMenuFromMenuInfo(parent_menu, x, y, &stMenuInfo, TRUE);
   for (item_info=stMenuInfo.items, i=0; i < num_items; item_info++, i++) {
      UtilFree(item_info->status_str);
   }
   memset(stMenuInfo.items, 0, (num_items+1)*sizeof(TgMenuItemInfo));
   free(stMenuInfo.items);
   stMenuInfo.items = NULL;
   if (menu != NULL) {
      TgMenuItem stMenuItem;

      menu->track_menubar = FALSE;
      memset(&stMenuItem, 0, sizeof(TgMenuItem));
      for (i=0; i < num_items; i++) {
         TgMenuItem *menu_item=(&menu->menuitems[i]);

         if (pixels != NULL) {
            stMenuItem.multicolor_pixel = pixels[i];
            if (!TgSetMenuItemInfo(menu_item, TGMU_MASK_MULTICOLOR,
                  &stMenuItem)) {
               return TgDestroyMenu(menu, TRUE);
            }
         }
         menu_item->menu_str_allocated = TRUE;
      }
   }
   return menu;
}

void FreeEditAttrInfo(pEditAttrInfo)
   EditAttrInfo *pEditAttrInfo;
   /* pEditAttrInfo will also be freed before returning */
{
   int i=0, num_attrs=0, *fore_colors=NULL, *attr_indices=NULL;
   char **attr_names=NULL, **attr_values=NULL, **attr_strings=NULL;
   char **status_strings=NULL;

   if (pEditAttrInfo == NULL) return;

   num_attrs = pEditAttrInfo->num_attrs;
   fore_colors = pEditAttrInfo->fore_colors;
   attr_indices = pEditAttrInfo->attr_indices;
   attr_names = pEditAttrInfo->attr_names;
   attr_values = pEditAttrInfo->attr_values;
   attr_strings = pEditAttrInfo->attr_strings;
   status_strings = pEditAttrInfo->status_strings;

   if (pEditAttrInfo->fore_colors != NULL) free(pEditAttrInfo->fore_colors);
   if (pEditAttrInfo->attr_indices != NULL) free(pEditAttrInfo->attr_indices);

   for (i=0; i < num_attrs; i++) {
      if (attr_names != NULL) UtilFree(attr_names[i]);
      if (attr_values != NULL) UtilFree(attr_values[i]);
      if (attr_strings != NULL) {
         if (attr_strings[i] != TGMUITEM_SEPARATOR) UtilFree(attr_strings[i]);
      }
      if (status_strings != NULL) UtilFree(status_strings[i]);
   }
   if (attr_names != NULL) free(attr_names);
   if (attr_values != NULL) free(attr_values);
   if (attr_strings != NULL) free(attr_strings);
   if (status_strings != NULL) free(status_strings);

   free(pEditAttrInfo);
}

int HasEditAttrsInContextMenu(obj_ptr, pp_name_attr)
   struct ObjRec *obj_ptr;
   struct AttrRec **pp_name_attr;
{
   struct AttrRec *attr_ptr=NULL;

   if (pp_name_attr != NULL) *pp_name_attr = NULL;
   for (attr_ptr=obj_ptr->lattr; attr_ptr!=NULL; attr_ptr=attr_ptr->prev) {
      if (strcmp(attr_ptr->attr_name.s, "edit_attrs_in_context_menu=") == 0) {
         if (pp_name_attr != NULL) *pp_name_attr = attr_ptr;
         return TRUE;
      }
   }
   return FALSE;
}

void FreeRestrictedAttrNames(ppsz_restricted, num_restricted)
   char **ppsz_restricted;
   int num_restricted;
{
   if (ppsz_restricted != NULL) {
      int i=0;

      for (i=0; i < num_restricted; i++) {
         if (ppsz_restricted[i] != NULL) {
            UtilFree(ppsz_restricted[i]);
         }
      }
      free(ppsz_restricted);
   }
}

void GetRestrictedAttrNames(restricted_attr_obj, pppsz_restricted,
      pn_num_restricted)
   struct ObjRec *restricted_attr_obj;
   char ***pppsz_restricted;
   int *pn_num_restricted;
{
   MiniLinesInfo *minilines=(&restricted_attr_obj->detail.t->minilines);
   MiniLineInfo *miniline=minilines->first->next;
   char **ppsz_restricted=NULL;
   int num_restricted=0, index=0;

   if (pppsz_restricted != NULL) (*pppsz_restricted) = NULL;
   if (pn_num_restricted != NULL) (*pn_num_restricted) = 0;
   if (miniline == NULL) return;

   for ( ; miniline != NULL; miniline=miniline->next, num_restricted++) {
   }
   ppsz_restricted = (char**)malloc(num_restricted*sizeof(char*));
   if (ppsz_restricted == NULL) FailAllocMessage();
   memset(ppsz_restricted, 0, num_restricted*sizeof(char*));

   miniline = minilines->first->next;
   for ( ; miniline != NULL; miniline=miniline->next) {
      int need_to_free_tmp_buf=FALSE;
      char *tmp_buf=ConvertMiniLineToString(miniline, &need_to_free_tmp_buf);

      if (tmp_buf != NULL) {
         UtilTrimBlanks(tmp_buf);
         if (*tmp_buf == '\0') {
            num_restricted--;
         } else {
            int len=strlen(tmp_buf)+2; /* extra char for '=' */

            ppsz_restricted[index] = (char*)malloc((len+1)*sizeof(char));
            if (ppsz_restricted[index] == NULL) FailAllocMessage();
            sprintf(ppsz_restricted[index], "%s=", tmp_buf);
            index++;
         }
         if (need_to_free_tmp_buf) UtilFree(tmp_buf);
      } else {
         num_restricted--;
      }
      if (num_restricted <= 0) break;
   }
   if (num_restricted <= 0) {
      free(pppsz_restricted);
   } else {
      if (pppsz_restricted != NULL) (*pppsz_restricted) = ppsz_restricted;
      if (pn_num_restricted != NULL) (*pn_num_restricted) = num_restricted;
   }
}

void GetAttrGroupAttrNames(restricted_attr_obj, group_index, pppsz_restricted,
      pn_num_restricted)
   struct ObjRec *restricted_attr_obj;
   int group_index, *pn_num_restricted;
   char ***pppsz_restricted;
{
   char **ppsz_restricted=NULL, **attr_name_array=NULL;
   int num_attrs_in_attr_group=0, i=0;

   num_attrs_in_attr_group = gAttrGroupInfo[group_index]->num_attrs;
   attr_name_array = gAttrGroupInfo[group_index]->attr_name;

   if (pppsz_restricted != NULL) (*pppsz_restricted) = NULL;
   if (pn_num_restricted != NULL) (*pn_num_restricted) = 0;

   ppsz_restricted = (char**)malloc(num_attrs_in_attr_group*sizeof(char*));
   if (ppsz_restricted == NULL) FailAllocMessage();
   memset(ppsz_restricted, 0, num_attrs_in_attr_group*sizeof(char*));

   for (i=0; i < num_attrs_in_attr_group; i++) {
      int len=strlen(attr_name_array[i])+2; /* extra char for '=' */

      ppsz_restricted[i] = (char*)malloc((len+1)*sizeof(char));
      if (ppsz_restricted[i] == NULL) FailAllocMessage();
      sprintf(ppsz_restricted[i], "%s=", attr_name_array[i]);
   }
   if (pppsz_restricted != NULL) (*pppsz_restricted) = ppsz_restricted;
   if (pn_num_restricted != NULL) {
      (*pn_num_restricted) = num_attrs_in_attr_group;
   }
}

int IsRestrictedAttr(attr_name, ppsz_restricted, num_restricted)
   char *attr_name, **ppsz_restricted;
   int num_restricted;
{
   int i=0;

   for (i=0; i < num_restricted; i++) {
      if (strcmp(attr_name, ppsz_restricted[i]) == 0) {
         return TRUE;
      }
   }
   return FALSE;
}

EditAttrInfo *CreateEditAttrInfo(obj_ptr)
   struct ObjRec *obj_ptr;
{
   int i=0, num_attrs=0, *fore_colors=NULL, *pixel_ptr=NULL, restricted=FALSE;
   int num_restricted=0, num_created=0, *attr_indices=NULL;
   struct AttrRec *attr_ptr=NULL, *attr_ptr1=NULL, *restricted_attr=NULL;
   EditAttrInfo *pEditAttrInfo=NULL;
   char **attr_names=NULL, **attr_values=NULL, **attr_strings=NULL;
   char **status_strings=NULL, **ppsz_restricted=NULL;

   pEditAttrInfo = (EditAttrInfo*)malloc(sizeof(EditAttrInfo));
   if (pEditAttrInfo == NULL) FailAllocMessage();
   memset(pEditAttrInfo, 0, sizeof(EditAttrInfo));

   attr_ptr1 = attr_ptr = obj_ptr->fattr;
   for ( ; attr_ptr1 != NULL; attr_ptr1=attr_ptr1->next, num_attrs++) {
   }
   if (num_attrs == 0) {
      free(pEditAttrInfo);
      return NULL;
   }
   restricted = HasEditAttrsInContextMenu(obj_ptr, &restricted_attr);
   if (restricted) {
      GetRestrictedAttrNames(restricted_attr->obj, &ppsz_restricted,
            &num_restricted);
      if (ppsz_restricted == NULL || num_restricted <= 0) {
         free(pEditAttrInfo);
         return NULL;
      }
   }
   attr_names = (char**)malloc(num_attrs*sizeof(char*));
   attr_values = (char**)malloc(num_attrs*sizeof(char*));
   attr_strings = (char**)malloc(num_attrs*sizeof(char*));
   status_strings = (char**)malloc(num_attrs*sizeof(char*));
   fore_colors = pixel_ptr = (int*)malloc(num_attrs*sizeof(int));
   attr_indices = (int*)malloc(num_attrs*sizeof(int));
   if (attr_names == NULL || attr_values == NULL || attr_strings == NULL ||
         status_strings == NULL || fore_colors == NULL ||
         attr_indices == NULL) {
      FailAllocMessage();
   }
   attr_ptr1 = attr_ptr;
   for (i=0; i < num_attrs; i++, attr_ptr1=attr_ptr1->next) {
      int need_to_free_tmp_buf=FALSE;
      char *tmp_buf=NULL;
      MiniLinesInfo *minilines=(&attr_ptr1->obj->detail.t->minilines);

      tmp_buf = ConvertAttrNameFirstMiniLineToString(attr_ptr1,
            &need_to_free_tmp_buf);

      if (restricted) {
         if (!IsRestrictedAttr(attr_ptr1->attr_name.s, ppsz_restricted,
               num_restricted)) {
            if (need_to_free_tmp_buf) UtilFree(tmp_buf);
            continue;
         }
      }
      attr_names[num_created] = UtilStrDup(attr_ptr1->attr_name.s);
      attr_values[num_created] = UtilStrDup(attr_ptr1->attr_value.s);
      attr_strings[num_created] = UtilStrDup(tmp_buf);
      sprintf(gszMsgBox, TgLoadCachedString(CSTID_EDIT_ATTR_IN_EDITOR),
            tmp_buf);
      status_strings[num_created] = UtilStrDup(gszMsgBox);
      if (attr_names[num_created] == NULL || attr_values[num_created] == NULL ||
            attr_strings[num_created] == NULL ||
            status_strings[num_created] == NULL) {
         FailAllocMessage();
      }
      pixel_ptr[num_created] =
            colorPixels[minilines->first->first_block->seg->color];
      attr_indices[num_created] = i;

      if (need_to_free_tmp_buf) UtilFree(tmp_buf);
      num_created++;
   }
   pEditAttrInfo->num_attrs = num_created;
   pEditAttrInfo->fore_colors = fore_colors;
   pEditAttrInfo->attr_indices = attr_indices;
   pEditAttrInfo->attr_names = attr_names;
   pEditAttrInfo->attr_values = attr_values;
   pEditAttrInfo->attr_strings = attr_strings;
   pEditAttrInfo->status_strings = status_strings;

   FreeRestrictedAttrNames(ppsz_restricted, num_restricted);

   return pEditAttrInfo;
}

EditAttrInfo *CreateEditAttrGroupInfo(obj_ptr)
   struct ObjRec *obj_ptr;
{
   int i=0, num_attrs=maxAttrGroups, *fore_colors=NULL;
   EditAttrInfo *pEditAttrInfo=NULL;
   char **attr_strings=NULL, **status_strings=NULL;

   if (num_attrs == 0) {
      return NULL;
   }
   pEditAttrInfo = (EditAttrInfo*)malloc(sizeof(EditAttrInfo));
   if (pEditAttrInfo == NULL) FailAllocMessage();
   memset(pEditAttrInfo, 0, sizeof(EditAttrInfo));

   /*
    * For this function, we do not need to call HasEditAttrsInContextMenu()
    *     because the "edit_attrs_in_context_menu=" attribute is only for the
    *     "Edit Attribute In Editor" submenu and *not* for the
    *     "Edit Attribute Group In Editor" submenu.
    */
   attr_strings = (char**)malloc(num_attrs*sizeof(char*));
   status_strings = (char**)malloc(num_attrs*sizeof(char*));
   fore_colors = (int*)malloc(num_attrs*sizeof(int));

   if (attr_strings == NULL || status_strings == NULL || fore_colors == NULL) {
      FailAllocMessage();
   }
   for (i=0; i < num_attrs; i++) {
      attr_strings[i] = UtilStrDup(gAttrGroupInfo[i]->group_name);
      sprintf(gszMsgBox, TgLoadCachedString(CSTID_EDIT_ATTR_GROUP_IN_EDITOR),
            gAttrGroupInfo[i]->group_name, gAttrGroupInfo[i]->displayed_names);
      status_strings[i] = UtilStrDup(gszMsgBox);

      if (attr_strings[i] == NULL || status_strings[i] == NULL) {
         FailAllocMessage();
      }
      fore_colors[i] = myFgPixel;
   }
   pEditAttrInfo->num_attrs = num_attrs;
   pEditAttrInfo->attr_strings = attr_strings;
   pEditAttrInfo->status_strings = status_strings;
   pEditAttrInfo->fore_colors = fore_colors;

   return pEditAttrInfo;
}

void MoveAttr()
{
   struct ObjRec *obj_ptr=NULL;
   struct AttrRec *attr_ptr=NULL, *attr_ptr1=NULL;
   int num_attrs=0, i, index=INVALID, x, y;
   int sel_ltx, sel_lty, sel_rbx, sel_rby;
   int *fore_colors=NULL, *pixel_ptr=NULL;
   char **attr_strings=NULL;
   unsigned int button;

   if (topSel != botSel || topSel == NULL) {
      MsgBox(TgLoadString(STID_SELECT_ONLY_ONE_OBJ), TOOL_NAME, INFO_MB);
      return;
   }
   obj_ptr = topSel->obj;
   attr_ptr1 = attr_ptr = obj_ptr->fattr;
   for ( ; attr_ptr1 != NULL; attr_ptr1=attr_ptr1->next, num_attrs++) {
   }
   if (num_attrs == 0) {
      MsgBox(TgLoadString(STID_SELECT_HAS_NO_ATTR), TOOL_NAME, INFO_MB);
      return;
   }
   attr_strings = (char**)malloc(num_attrs*sizeof(char*));
   fore_colors = pixel_ptr = (int*)malloc(num_attrs*sizeof(int));
   if (attr_strings == NULL || fore_colors == NULL) {
      FailAllocMessage();
   }
   attr_ptr1 = attr_ptr;
   for (i=0; i < num_attrs; i++, attr_ptr1=attr_ptr1->next) {
      int need_to_free_tmp_buf=FALSE;
      char *tmp_buf=NULL;
      MiniLinesInfo *minilines=(&attr_ptr1->obj->detail.t->minilines);

      tmp_buf = ConvertAttrNameFirstMiniLineToString(attr_ptr1,
            &need_to_free_tmp_buf);
      if ((attr_strings[i]=UtilStrDup(tmp_buf)) == NULL) FailAllocMessage();

      *pixel_ptr++ = colorPixels[minilines->first->first_block->seg->color];

      if (need_to_free_tmp_buf) UtilFree(tmp_buf);
   }

   Msg(TgLoadString(STID_DRAG_LEFT_BTN_TO_SEE_ATTR));
   SetMouseStatus(TgLoadString(STID_SHOW_MOVE_ATTR),
         TgLoadString(STID_CHANGE_ATTR_JUST),
         TgLoadCachedString(CSTID_PARANED_NONE));
   button = CornerLoop(&x, &y);
   activeMenu = INVALID;
   if (button == Button1 || button == Button2) {
      TgMenu *menu=CreateAttrMenu(NULL, x, y, num_attrs, attr_strings, NULL,
            fore_colors);

      if (menu != NULL) {
         index = TgMenuLoop(menu);
         TgDestroyMenu(menu, TRUE);
      }
      free(fore_colors);
   }
   if (index != INVALID) {
      attr_ptr1 = attr_ptr;
      for (i=0; i < index; i++, attr_ptr1=attr_ptr1->next) {
      }
      sel_ltx = selLtX; sel_lty = selLtY; sel_rbx = selRbX; sel_rby = selRbY;
      if (button == Button1) {
         if (MoveOneAttr(obj_ptr, attr_ptr1)) {
            /* HighLightReverse() is expected to be called */
            UpdSelBBox();
            RedrawAreas(botObj, sel_ltx-GRID_ABS_SIZE(1),
                  sel_lty-GRID_ABS_SIZE(1),
                  sel_rbx+GRID_ABS_SIZE(1), sel_rby+GRID_ABS_SIZE(1),
                  selLtX-GRID_ABS_SIZE(1), selLtY-GRID_ABS_SIZE(1),
                  selRbX+GRID_ABS_SIZE(1), selRbY+GRID_ABS_SIZE(1));
            SetFileModified(TRUE);
            HighLightForward();
         }
      } else if (button == Button2) {
         if (ChangeAttrJust(obj_ptr, attr_ptr1)) {
            /* HighLightReverse() is expected to be called */
            UpdSelBBox();
            RedrawAreas(botObj, sel_ltx-GRID_ABS_SIZE(1),
                  sel_lty-GRID_ABS_SIZE(1), sel_rbx+GRID_ABS_SIZE(1),
                  sel_rby+GRID_ABS_SIZE(1), selLtX-GRID_ABS_SIZE(1),
                  selLtY-GRID_ABS_SIZE(1), selRbX+GRID_ABS_SIZE(1),
                  selRbY+GRID_ABS_SIZE(1));
            SetFileModified(TRUE);
            HighLightForward();
         }
      }
   }
   for (i=0; i < num_attrs; i++) UtilFree(attr_strings[i]);
   free(attr_strings);
   justDupped = FALSE;
}

void CopyAndUpdateAttrs(ToObjPtr, FromObjPtr)
   struct ObjRec *ToObjPtr, *FromObjPtr;
{
   struct AttrRec *to_attr_ptr, *from_attr_ptr;
   int confirm_status=MB_ID_CANCEL;

   topAttr = botAttr = NULL;
   from_attr_ptr = FromObjPtr->fattr;
   for ( ; from_attr_ptr != NULL; from_attr_ptr=from_attr_ptr->next) {
      to_attr_ptr = ToObjPtr->fattr;
      for ( ; to_attr_ptr != NULL; to_attr_ptr=to_attr_ptr->next) {
         if (from_attr_ptr->obj->color==to_attr_ptr->obj->color &&
               strcmp(from_attr_ptr->attr_name.s,
               to_attr_ptr->attr_name.s)==0) {
            if (*(from_attr_ptr->attr_value.s) != '\0') {
               DynStrCpy(&to_attr_ptr->attr_value, &from_attr_ptr->attr_value);
               UpdAttr(to_attr_ptr);
            }
            break;
         }
      }
      if (to_attr_ptr == NULL) {
         /* cannot find the same attribute in the new one */
         confirm_status = MB_ID_NO;

         if (from_attr_ptr->inherited) {
            char msg[MAXSTRING];

            /* obsolete attribute? */
            confirm_status = (dropObsIconAttrWhenUpdate) ? MB_ID_YES :
                  MB_ID_CANCEL;
            if (*from_attr_ptr->attr_name.s == '\0') {
               sprintf(msg, TgLoadString(STID_IS_ATTR_OBSOLETE_FOR_ICON_YNC),
                     from_attr_ptr->attr_value.s, FromObjPtr->detail.r->s);
            } else {
               sprintf(msg, TgLoadString(STID_IS_ATTR_OBSOLETE_FOR_ICON_YNC),
                     from_attr_ptr->attr_name.s, FromObjPtr->detail.r->s);
            }
            while (confirm_status == MB_ID_CANCEL) {
               confirm_status = MsgBox(msg, TOOL_NAME, YN_MB);
               if (confirm_status == MB_ID_CANCEL) {
                  MsgBox(TgLoadCachedString(CSTID_CANCEL_NOT_AVAIL), TOOL_NAME,
                        INFO_MB);
               }
            }
         }
         if (confirm_status == MB_ID_NO) {
            /* new attribute */
            to_attr_ptr = (struct AttrRec *)malloc(sizeof(struct AttrRec));
            if (to_attr_ptr == NULL) FailAllocMessage();
            memset(to_attr_ptr, 0, sizeof(struct AttrRec));
            to_attr_ptr->owner = ToObjPtr;
            DupAnAttr(from_attr_ptr, to_attr_ptr);
            LinkInAttr((struct AttrRec *)NULL, topAttr, to_attr_ptr);
         }
      } else {
         to_attr_ptr->shown = from_attr_ptr->shown;
         to_attr_ptr->nameshown = from_attr_ptr->nameshown;
      }
   }
   if (topAttr != NULL) {
      topAttr->prev = NULL;
      botAttr->next = ToObjPtr->fattr;

      if (ToObjPtr->fattr != NULL) ToObjPtr->fattr->prev = botAttr;
      ToObjPtr->fattr = topAttr;
      if (ToObjPtr->lattr == NULL) ToObjPtr->lattr = botAttr;
   }
   AdjObjBBox(ToObjPtr);
}

void ToggleNamedAttrShown(attr_name)
   char *attr_name;
{
   char *paren_ptr;
   struct AttrRec *attr_ptr;
   int ltx, lty, rbx, rby;

   if (topSel != botSel || topSel == NULL) {
      MsgBox(TgLoadString(STID_SELECT_ONLY_ONE_OBJ), TOOL_NAME, INFO_MB);
      return;
   }
   if ((paren_ptr=strchr(attr_name, ')')) == NULL) {
      MsgBox(TgLoadString(STID_BAD_FORMAT_IN_SHORTCUT_SPEC), TOOL_NAME,
            INFO_MB);
      return;
   }
   *paren_ptr = '\0';
   if ((attr_ptr=FindAttrWithName(topSel->obj, attr_name, NULL)) == NULL) {
      char msg[MAXSTRING+1];

      sprintf(msg, TgLoadString(STID_CANNOT_FIND_NAMED_ATTR), attr_name);
      MsgBox(msg, TOOL_NAME, INFO_MB);
      return;
   }
   ltx = selLtX; lty = selLtY; rbx = selRbX; rby = selRbY;
   HighLightReverse();
   PrepareToReplaceAnObj(topSel->obj);
   attr_ptr->shown = !attr_ptr->shown;
   AdjObjBBox(topSel->obj);
   RecordReplaceAnObj(topSel->obj);
   UpdSelBBox();
   RedrawAreas(botObj, ltx-GRID_ABS_SIZE(1), lty-GRID_ABS_SIZE(1),
         rbx+GRID_ABS_SIZE(1), rby+GRID_ABS_SIZE(1),
         selLtX-GRID_ABS_SIZE(1), selLtY-GRID_ABS_SIZE(1),
         selRbX+GRID_ABS_SIZE(1), selRbY+GRID_ABS_SIZE(1));
   SetFileModified(TRUE);
   HighLightForward();
}

void ToggleEqAttrShown()
{
   char buf[80];

   strcpy(buf, "eq=)");
   ToggleNamedAttrShown(buf);
}

void AddFileAttrs()
{
   struct SelRec *sel_ptr;
   int ltx, lty, rbx, rby;

   if (gstWBInfo.do_whiteboard) {
      /*
       * Now it's allowed.
       *
       * MsgBox(TgLoadString(STID_ATTACH_FILE_ATTRS_IN_WB), TOOL_NAME, INFO_MB);
       * return;
       */
   }
   if (topSel == NULL) {
      MsgBox(TgLoadString(STID_SELECT_AT_LEAST_ONE_TEXT), TOOL_NAME, INFO_MB);
      return;
   }
   for (sel_ptr=topSel; sel_ptr!=NULL; sel_ptr=sel_ptr->next) {
      if (sel_ptr->obj->type != OBJ_TEXT) {
         MsgBox(TgLoadString(STID_CANNOT_ATTACH_NON_TEXT_OBJ), TOOL_NAME,
               INFO_MB);
         return;
      }
   }

   ltx = selLtX; lty = selLtY; rbx = selRbX; rby = selRbY;

   HighLightReverse();
   AddObj(NULL, topObj, tgifObj);
   AddNewSelObj(topObj);
   PrepareToRecord(CMD_REPLACE, topSel, botSel, numObjSelected);

   for (sel_ptr=topSel->next; sel_ptr!=NULL; sel_ptr=sel_ptr->next) {
      AddAttr(topObj, sel_ptr->obj);
   }
   RemoveAllSel();
   AddNewSelObj(topObj);
   UpdSelBBox();

   recordCmdIncludeTgifObj = TRUE;
   RecordCmd(CMD_MANY_TO_ONE, NULL, topSel, botSel, 1);
   recordCmdIncludeTgifObj = FALSE;
   RemoveAllSel();
   UnlinkObj(topObj);
   RedrawAnArea(botObj, ltx-GRID_ABS_SIZE(1), lty-GRID_ABS_SIZE(1),
         rbx+GRID_ABS_SIZE(1), rby+GRID_ABS_SIZE(1));
   SetFileModified(TRUE);
   justDupped = FALSE;
}

void DetachFileAttrs()
{
   struct AttrRec *attr_ptr, *next_attr;
   struct SelRec *sel_ptr;

   if (gstWBInfo.do_whiteboard) {
      /*
       * Now it's allowed.
       *
       * MsgBox(TgLoadString(STID_DETACH_FILE_ATTRS_IN_WB), TOOL_NAME, INFO_MB);
       * return;
       */
   }
   if (tgifObj->fattr == NULL) {
      MsgBox(TgLoadString(STID_FILE_CONTAINS_NO_ATTRS), TOOL_NAME, INFO_MB);
      return;
   }
   MakeQuiescent();

   AddObj(NULL, topObj, tgifObj);
   AddNewSelObj(topObj);
   PrepareToReplaceAnObj(topObj);

   for (attr_ptr=topObj->fattr; attr_ptr!=NULL; attr_ptr=next_attr) {
      next_attr = attr_ptr->next;

      sel_ptr = (struct SelRec *)malloc(sizeof(struct SelRec));
      if (sel_ptr == NULL) FailAllocMessage();
      sel_ptr->obj = attr_ptr->obj;
      AddSel(topSel, topSel->next, sel_ptr);

      attr_ptr->obj->detail.t->attr = NULL;
      AddObj(topObj, topObj->next, attr_ptr->obj);
      FreeAttr(attr_ptr);
   }
   topObj->fattr = topObj->lattr = NULL;
   recordCmdIncludeTgifObj = TRUE;
   RecordCmd(CMD_ONE_TO_MANY, NULL, topSel, botSel, numObjSelected);
   recordCmdIncludeTgifObj = FALSE;

   UnlinkObj(topObj);
   FreeTopSel();

   RedrawAnArea(botObj, selLtX-GRID_ABS_SIZE(1), selLtY-GRID_ABS_SIZE(1),
         selRbX+GRID_ABS_SIZE(1), selRbY+GRID_ABS_SIZE(1));
   HighLightForward();
   SetFileModified(TRUE);
   justDupped = FALSE;
   SetCurChoice(curChoiceBeforeMakeQuiescent);
}

#define SHOWN_COL  0
#define NSHOWN_COL 1

static DspList *fileAttrNameDspPtr=NULL;

static
DspList *FileAttrNameListing(ObjPtr, pn_entries, pCheckArray)
   /*
    * If pCheckArray != NULL, this routine sets pCheckArray->num_cols,
    *     pCheckArray->num_rows and pCheckArray->value.
    */
   struct ObjRec *ObjPtr;
   int *pn_entries;
   struct CheckArrayRec *pCheckArray;
{
   int i, num_rows=0;
   DspList *dsp_ptr;
   struct AttrRec *attr_ptr;

   for (num_rows=0, attr_ptr=ObjPtr->fattr; attr_ptr!=NULL;
         attr_ptr=attr_ptr->next, num_rows++) {
   }
   if (pCheckArray != NULL) {
      pCheckArray->num_cols = 2;
      pCheckArray->num_rows = num_rows;
      pCheckArray->value = (int**)malloc(2*sizeof(int*));
      if (pCheckArray->value == NULL) FailAllocMessage();
      memset(pCheckArray->value, 0, 2*sizeof(int*));
      pCheckArray->value[SHOWN_COL] = (int*)malloc(num_rows*sizeof(int));
      pCheckArray->value[NSHOWN_COL] = (int*)malloc(num_rows*sizeof(int));
      if (pCheckArray->value[SHOWN_COL] == NULL ||
            pCheckArray->value[NSHOWN_COL] == NULL) {
         FailAllocMessage();
      }
      memset(pCheckArray->value[SHOWN_COL], 0, num_rows*sizeof(int));
      memset(pCheckArray->value[NSHOWN_COL], 0, num_rows*sizeof(int));
   }
   fileAttrNameDspPtr = (DspList*)malloc(num_rows*sizeof(DspList));
   if (fileAttrNameDspPtr == NULL) FailAllocMessage();
   memset(fileAttrNameDspPtr, 0, num_rows*sizeof(DspList));
   for (i=1, dsp_ptr=fileAttrNameDspPtr, attr_ptr=ObjPtr->fattr;
         i<=num_rows; i++, dsp_ptr++, attr_ptr=attr_ptr->next) {
      if (*attr_ptr->attr_name.s == '\0') {
         UtilStrCpyN(dsp_ptr->itemstr, sizeof(dsp_ptr->itemstr),
               attr_ptr->attr_value.s);
      } else {
         sprintf(gszMsgBox, "%s%s", attr_ptr->attr_name.s,
               attr_ptr->attr_value.s);
         UtilStrCpyN(dsp_ptr->itemstr, sizeof(dsp_ptr->itemstr), gszMsgBox);
      }
      /* use the directory field for inherited */
      dsp_ptr->directory = attr_ptr->inherited;
      dsp_ptr->next = ((i == num_rows) ? NULL : &dsp_ptr[1]);
      if (pCheckArray != NULL) {
         if (attr_ptr->shown) pCheckArray->value[SHOWN_COL][i-1] = TRUE;
         if (attr_ptr->nameshown) pCheckArray->value[NSHOWN_COL][i-1] = TRUE;
      }
   }
   *pn_entries = num_rows;
   return fileAttrNameDspPtr;
}

static
int EditAttrCheckUpdate(pp_dsp_ptr, ppsz_entries, pn_num_entries,
      pn_marked_index, pp_check_array, cur_buf, col, row, p_void)
   DspList **pp_dsp_ptr;
   char ***ppsz_entries, *cur_buf;
   int *pn_num_entries, *pn_marked_index, col, row;
   struct CheckArrayRec **pp_check_array;
   void *p_void;
{
   char *psz=NULL, saved_ch='\0', *buf=NULL;
   int shown=FALSE, nameshown=FALSE;

   if (ppsz_entries == NULL || *ppsz_entries == NULL ||
         (*ppsz_entries)[row] == NULL || pp_check_array == NULL ||
         (*pp_check_array) == NULL || (*pp_check_array)->value == NULL) {
      return FALSE;
   }
   buf = (*ppsz_entries)[row];
   shown = (*pp_check_array)->value[SHOWN_COL][row];
   nameshown = (*pp_check_array)->value[NSHOWN_COL][row];

   psz = strchr(buf, '=');
   if (psz != NULL) {
      saved_ch = *psz;
      *psz = '\0';
      if (shown) {
         if (nameshown) {
            sprintf(gszMsgBox, TgLoadString(STID_SHOW_ATTR_NAME_AND_VALUE),
                  buf);
         } else {
            sprintf(gszMsgBox, TgLoadString(STID_SHOW_ATTR_VALUE_ONLY), buf);
         }
      } else {
         sprintf(gszMsgBox, TgLoadString(STID_NOT_SHOW_ATTR), buf);
      }
      *psz = saved_ch;
   } else {
      if (shown) {
         sprintf(gszMsgBox, TgLoadString(STID_SHOW_ATTR), buf);
      } else {
         sprintf(gszMsgBox, TgLoadString(STID_WONT_SHOW_VALUE), buf);
      }
   }
   SetStringStatus(gszMsgBox);

   return TRUE;
}

static
int EditAttrNames(TopStr, ObjAttrDspList, entries, num_entries, pCheckArray)
   char *TopStr, **entries;
   DspList *ObjAttrDspList;
   int num_entries;
   struct CheckArrayRec *pCheckArray;
{
   char win_name[128];

   sprintf(win_name, TgLoadString(STID_TGIF_EDIT_ATTR_NAMES), TOOL_NAME);
   ResetNamesInfo();
   NamesSetTitle(TopStr);
   NamesAddButton(TgLoadCachedString(CSTID_OK), BUTTON_OK);
   NamesAddButton(TgLoadCachedString(CSTID_CANCEL), BUTTON_CANCEL);
   /* ignore double-click and <CR> */
   NamesSetDefaultBtnId(BUTTON_OK, INVALID);
   NamesSetStyle(NAMES_EDIT_ATTR, NAMES_LOOP_ONCE);
   if (pCheckArray != NULL) {
      NamesSetCallback(NULL, NULL, (CheckUpdateFunc*)EditAttrCheckUpdate);
   }
   NamesSetEntries(ObjAttrDspList, entries, num_entries, pCheckArray,
         TRUE, INVALID, 0);
   return (Names(win_name, NULL, NULL, 0, NULL)==BUTTON_OK);
}

int BlankStr(s)
   char *s;
{
   while (*s == ' ') s++;
   return (*s == '\0');
}

void EditFileAttrs()
{
   int i, num_entries=0;
   DspList *dsp_ptr;
   char **entries=NULL;

   if (gstWBInfo.do_whiteboard) {
      /*
       * Now it's allowed.
       *
       * MsgBox(TgLoadString(STID_EDIT_FILE_ATTRS_IN_WB), TOOL_NAME, INFO_MB);
       * return;
       */
   }
   if (tgifObj->fattr == NULL) {
      MsgBox(TgLoadString(STID_FILE_CONTAINS_NO_ATTRS), TOOL_NAME, INFO_MB);
      return;
   }
   MakeQuiescent();

   dsp_ptr = FileAttrNameListing(tgifObj, &num_entries, NULL);
   ignoreDirectoryFlag = TRUE;
   entries = MakeNameDspItemArray(num_entries, dsp_ptr);
   ignoreDirectoryFlag = FALSE;
   if (EditAttrNames(TgLoadString(STID_EDIT_FILE_ATTRS_DOTS), NULL, entries,
         num_entries, NULL)) {
      int modified=FALSE;
      struct AttrRec *attr_ptr, *next_attr;

      AddObj(NULL, topObj, tgifObj);
      AddNewSelObj(topObj);
      PrepareToReplaceAnObj(topObj);

      for (attr_ptr=tgifObj->fattr, i=0; attr_ptr!=NULL;
            attr_ptr=next_attr, i++) {
         int blank_str=BlankStr(entries[i]);
         struct TextRec *text_ptr=attr_ptr->obj->detail.t;

         next_attr = attr_ptr->next;
         if ((blank_str &&
               text_ptr->minilines.first==text_ptr->minilines.last) ||
               strcmp(GetTextPtrFirstStrSeg(text_ptr)->dyn_str.s,
               entries[i]) != 0) {
            modified = TRUE;
            if (blank_str &&
                  text_ptr->minilines.first==text_ptr->minilines.last) {
               UnlinkAttr(attr_ptr);
               FreeTextObj(attr_ptr->obj);
               FreeAttr(attr_ptr);
            } else {
               DynStrSet(&GetTextPtrFirstStrSeg(text_ptr)->dyn_str,
                     entries[i]);
               UpdateAttr(text_ptr, attr_ptr);
            }
         }
      }
      if (modified) {
         recordCmdIncludeTgifObj = TRUE;
         RecordReplaceAnObj(topObj);
         recordCmdIncludeTgifObj = FALSE;

         RemoveAllSel();
         UnlinkObj(topObj);

         SetFileModified(TRUE);
         Msg(TgLoadString(STID_FILE_ATTRS_UPDATED));
      } else {
         AbortPrepareCmd(CMD_REPLACE);

         RemoveAllSel();
         UnlinkObj(topObj);
      }
   }
   free(dsp_ptr);
   free(*entries);
   free(entries);
   fileAttrNameDspPtr = NULL;
   Msg("");
   SetCurChoice(curChoiceBeforeMakeQuiescent);
}

void EditAttrs()
{
   int i, num_entries=0;
   DspList *dsp_ptr;
   char **entries=NULL;
   struct CheckArrayRec check_array, orig_check_array;

   if (topSel == NULL || topSel != botSel) {
      MsgBox(TgLoadString(STID_SELECT_ONE_OBJ_FOR_EDITATTRS), TOOL_NAME,
            INFO_MB);
      return;
   }
   if (topSel->obj->fattr == NULL) {
      MsgBox(TgLoadString(STID_OBJ_CONTAINS_NO_ATTRS), TOOL_NAME, INFO_MB);
      return;
   }
   memset(&check_array, 0, sizeof(struct CheckArrayRec));
   memset(&orig_check_array, 0, sizeof(struct CheckArrayRec));
   dsp_ptr = FileAttrNameListing(topSel->obj, &num_entries, &check_array);
   ignoreDirectoryFlag = TRUE;
   entries = MakeNameDspItemArray(num_entries, dsp_ptr);
   ignoreDirectoryFlag = FALSE;
   CopyCheckArray(&orig_check_array, &check_array);
   if (EditAttrNames(TgLoadString(STID_EDIT_OBJ_ATTRS_DOTS), dsp_ptr, entries,
         num_entries, &check_array)) {
      int modified=FALSE, sel_ltx, sel_lty, sel_rbx, sel_rby;
      struct AttrRec *attr_ptr, *next_attr;

      sel_ltx = selLtX; sel_lty = selLtY; sel_rbx = selRbX; sel_rby = selRbY;
      HighLightReverse();
      PrepareToReplaceAnObj(topSel->obj);

      for (attr_ptr=topSel->obj->fattr, i=0; attr_ptr!=NULL;
            attr_ptr=next_attr, i++) {
         int blank_str=BlankStr(entries[i]);
         char orig_str[MAXPATHLENGTH+1];
         struct TextRec *text_ptr=attr_ptr->obj->detail.t;

         next_attr = attr_ptr->next;
         if (*attr_ptr->attr_name.s == '\0') {
            strcpy(orig_str, attr_ptr->attr_value.s);
         } else {
            sprintf(orig_str, "%s%s", attr_ptr->attr_name.s,
                  attr_ptr->attr_value.s);
         }
         if ((blank_str &&
               text_ptr->minilines.first==text_ptr->minilines.last) ||
               strcmp(orig_str, entries[i]) != 0 ||
               (check_array.value[SHOWN_COL][i] !=
               orig_check_array.value[SHOWN_COL][i]) ||
               (check_array.value[NSHOWN_COL][i] !=
               orig_check_array.value[NSHOWN_COL][i])) {
            modified = TRUE;
            if (blank_str &&
                  text_ptr->minilines.first==text_ptr->minilines.last) {
               UnlinkAttr(attr_ptr);
               FreeTextObj(attr_ptr->obj);
               FreeAttr(attr_ptr);
            } else {
               int saved_nameshown;

               attr_ptr->shown = check_array.value[SHOWN_COL][i];
               attr_ptr->nameshown = check_array.value[NSHOWN_COL][i];
               saved_nameshown = attr_ptr->nameshown;
               DynStrSet(&GetTextPtrFirstStrSeg(text_ptr)->dyn_str,
                     entries[i]);
               attr_ptr->nameshown = TRUE;
               UpdateAttr(text_ptr, attr_ptr);
               if (!saved_nameshown) {
                  if (*attr_ptr->attr_name.s != '\0') {
                     attr_ptr->nameshown = saved_nameshown;
                  }
                  UpdAttr(attr_ptr);
                  UpdateAttr(text_ptr, attr_ptr);
               }
            }
         }
      }
      if (modified) {
         AdjObjBBox(topSel->obj);
         RecordReplaceAnObj(topSel->obj);
         UpdSelBBox();
         RedrawAreas(botObj, sel_ltx-GRID_ABS_SIZE(1),
                  sel_lty-GRID_ABS_SIZE(1),
                  sel_rbx+GRID_ABS_SIZE(1), sel_rby+GRID_ABS_SIZE(1),
                  selLtX-GRID_ABS_SIZE(1), selLtY-GRID_ABS_SIZE(1),
                  selRbX+GRID_ABS_SIZE(1), selRbY+GRID_ABS_SIZE(1));
         SetFileModified(TRUE);
         justDupped = FALSE;
         Msg(TgLoadString(STID_OBJ_ATTRS_UPDATED));
      } else {
         AbortPrepareCmd(CMD_REPLACE);
      }
      HighLightForward();
   }
   free(dsp_ptr);
   free(*entries);
   free(entries);
   CleanUpCheckArray(&orig_check_array);
   CleanUpCheckArray(&check_array);
   fileAttrNameDspPtr = NULL;
   Msg("");
}

static
int GetAttrNamesCount(buf)
   char *buf;
{
   int count=0;

   while (*buf != '\0') {
      char *psz=NULL;

      if (*buf == ':') return INVALID;

      psz = strchr(buf, ':');
      if (psz == NULL) {
         count++;
         break;
      } else {
         count++;
         buf = (&psz[1]);
      }
   }
   return count;
}

static
void SetAttrGroupInfoDisplayedNames(pagi)
   AttrGroupInfo *pagi;
{
   int i=0, total=0;
   char *psz=NULL;

   for (i=0; i < pagi->num_attrs; i++) {
      total += strlen(pagi->attr_name[i])+1;
   }
   total += 1;

   pagi->displayed_names = (char*)malloc(total*sizeof(char));
   if (pagi->displayed_names == NULL) FailAllocMessage();
   memset(pagi->displayed_names, 0, total*sizeof(char));

   for (i=0, psz=pagi->displayed_names; i < pagi->num_attrs; i++) {
      if (i == pagi->num_attrs-1) {
         sprintf(psz, "%s", pagi->attr_name[i]);
         psz += strlen(pagi->attr_name[i]);
      } else {
         sprintf(psz, "%s:", pagi->attr_name[i]);
         psz += strlen(pagi->attr_name[i])+1;
      }
   }
}

static
void FreeAttrGroupInfo(pagi)
   AttrGroupInfo *pagi;
{
   UtilFree(pagi->group_name);
   UtilFree(pagi->group_value);
   UtilFree(pagi->displayed_names);
   if (pagi->num_attrs > 0 && pagi->attr_name != NULL) {
      free(pagi->attr_name);
   }
}

static
void CleanUpAttrGroup()
{
   int i=0;

   if (maxAttrGroups != 0 && gAttrGroupInfo != NULL) {
      for (i=0; i < maxAttrGroups; i++) {
         FreeAttrGroupInfo(gAttrGroupInfo[i]);
         free(gAttrGroupInfo[i]);
      }
      free(gAttrGroupInfo);
   }
   maxAttrGroups = 0;
   gAttrGroupInfo = NULL;
}

void CleanUpAttr()
{
    CleanUpAttrGroup();
}

static
int ParseAttrGroupXDef(group_index, xdef_line, buf)
   int group_index;
   char *xdef_line, *buf;
{
   char *psz_attr_names=strchr(buf, ':'), *psz_attr=NULL;
   int count=0, index=0;

   if (psz_attr_names == NULL) {
      if (xdef_line == NULL) {
         fprintf(stderr, TgLoadString(STID_INVALID_COMPILER_OPT),
               "DEFATTRGROUP", buf);
      } else {
         fprintf(stderr, TgLoadString(STID_INVALID_XDEF), TOOL_NAME,
               xdef_line, buf);
      }
      fprintf(stderr, "\n");
      return FALSE;
   }
   *psz_attr_names++ = '\0';
   count = GetAttrNamesCount(psz_attr_names);
   if (count == INVALID) {
      if (xdef_line == NULL) {
         fprintf(stderr, TgLoadString(STID_INVALID_COMPILER_OPT),
               "DEFATTRGROUP", buf);
      } else {
         fprintf(stderr, TgLoadString(STID_INVALID_XDEF), TOOL_NAME,
               xdef_line, buf);
      }
      fprintf(stderr, "\n");
      return FALSE;
   }
   gAttrGroupInfo[group_index]->group_name = UtilStrDup(buf);
   if (gAttrGroupInfo[group_index]->group_name == NULL) {
      FailAllocMessage();
   }
   UtilTrimBlanks(gAttrGroupInfo[group_index]->group_name);

   gAttrGroupInfo[group_index]->group_value = UtilStrDup(psz_attr_names);
   if (gAttrGroupInfo[group_index]->group_value == NULL) {
      FailAllocMessage();
   }
   UtilTrimBlanks(gAttrGroupInfo[group_index]->group_value);

   gAttrGroupInfo[group_index]->attr_name = (char**)malloc(
         count*sizeof(char*));
   if (gAttrGroupInfo[group_index]->attr_name == NULL) {
      FailAllocMessage();
   }
   memset(gAttrGroupInfo[group_index]->attr_name, 0, count*sizeof(char*));

   psz_attr = gAttrGroupInfo[group_index]->group_value;
   while (*psz_attr != '\0') {
      char *psz_colon=strchr(psz_attr, ':');

      if (psz_colon != NULL) *psz_colon = '\0';

      UtilTrimBlanks(psz_attr);
      gAttrGroupInfo[group_index]->attr_name[index] = psz_attr;

      index++;
      if (psz_colon != NULL) {
         psz_attr = ++psz_colon;
      } else {
         break;
      }
   }
   gAttrGroupInfo[group_index]->num_attrs = count;
   SetAttrGroupInfoDisplayedNames(gAttrGroupInfo[group_index]);

   return TRUE;
}

int InitAttr()
{
   char *c_ptr=NULL, buf[MAXSTRING];
   int i=0, attr_group_in_xdef=TRUE;

   maxAttrGroups = 0;
   if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"MaxAttributeGroups")) !=
         NULL) {
      maxAttrGroups = atoi(c_ptr);
   }
   if (cmdLineTgrm2) {
#ifdef DEFATTRGROUP
      if (maxAttrGroups == 0) {
         maxAttrGroups = 1;
         attr_group_in_xdef = FALSE;
      }
#endif /* DEFATTRGROUP */
   }
   if (maxAttrGroups > 0) {
      gAttrGroupInfo = (AttrGroupInfo**)malloc(maxAttrGroups *
            sizeof(AttrGroupInfo*));
      if (gAttrGroupInfo == NULL) FailAllocMessage();
      memset(gAttrGroupInfo, 0, maxAttrGroups*sizeof(AttrGroupInfo*));

      for (i=0; i < maxAttrGroups; i++) {
         gAttrGroupInfo[i] = (AttrGroupInfo*)malloc(sizeof(AttrGroupInfo));
         if (gAttrGroupInfo[i] == NULL) FailAllocMessage();
         memset(gAttrGroupInfo[i], 0, sizeof(AttrGroupInfo));

         if (attr_group_in_xdef) {
            sprintf(buf, "AttributeGroup%1d", i);
            if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,buf)) != NULL) {
               ParseAttrGroupXDef(i, buf, c_ptr);
            } else {
               fprintf(stderr, TgLoadString(STID_CANNOT_GET_X_DEFAULT),
                     TOOL_NAME, buf);
               fprintf(stderr, "\n");
            }
         } else if (cmdLineTgrm2) {
#ifdef DEFATTRGROUP
            char *tmp_buf=UtilStrDup(DEFATTRGROUP);

            if (tmp_buf == NULL) FailAllocMessage();
            if (!ParseAttrGroupXDef(i, NULL, tmp_buf)) {
               CleanUpAttrGroup();
            }
            UtilFree(tmp_buf);
#endif /* DEFATTRGROUP */
         }
      }
   } else if (maxAttrGroups == 0) {
      /* nothing to do */
   } else {
      fprintf(stderr, TgLoadString(STID_INVALID_XDEF_USE_ALT_VALUE),
            TOOL_NAME, "MaxAttributeGroups", c_ptr, 0);
      fprintf(stderr, "\n");
   }
   return TRUE;
}
