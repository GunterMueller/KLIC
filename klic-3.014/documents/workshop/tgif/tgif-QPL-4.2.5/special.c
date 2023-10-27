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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/special.c,v 1.19 2011/05/16 16:21:59 william Exp $
 */

#define _INCLUDE_FROM_SPECIAL_C_

#include "tgifdefs.h"
#include "cmdids.h"

#include "animate.e"
#include "attr.e"
#include "auxtext.e"
#include "choice.e"
#include "cmd.e"
#include "color.e"
#include "cutpaste.e"
#include "cursor.e"
#include "dialog.e"
#include "drawing.e"
#include "dup.e"
#include "edit.e"
#include "exec.e"
#include "file.e"
#include "font.e"
#include "grid.e"
#include "group.e"
#include "list.e"
#include "mainloop.e"
#include "mainmenu.e"
#include "mark.e"
#include "menu.e"
#include "menuinfo.e"
#include "miniline.e"
#include "msg.e"
#include "move.e"
#include "names.e"
#include "obj.e"
#include "page.e"
#include "pattern.e"
#include "poly.e"
#include "raster.e"
#include "rect.e"
#include "remote.e"
#include "ruler.e"
#include "scroll.e"
#include "select.e"
#include "setup.e"
#include "special.e"
#include "stk.e"
#include "stretch.e"
#include "strtbl.e"
#include "tangram2.e"
#include "text.e"
#include "util.e"
#include "version.e"
#include "wb.e"

int placingTopObj=FALSE;
int connectingPortsByWire=FALSE;
int connectingPortsFromInternalCommand=TRUE;

char gszAttrSeparator[]=
"// -------- attribute separator (please do not modify or delete) -------- //";

static int showWireSignalName=TRUE;
static int gnPinWarning=FALSE;

static
struct ObjRec *CreatePinObj(obj_ptr1, obj_ptr2, sym_count)
   struct ObjRec *obj_ptr1, *obj_ptr2;
   int sym_count;
{
   struct ObjRec *obj_ptr=NULL;
   struct ObjRec *saved_top_obj=topObj;
   struct ObjRec *saved_bot_obj=botObj;
   int cx1, cy1, cx2, cy2;

   curPage->top = topObj = NULL;
   curPage->bot = botObj = NULL;
   if (sym_count == 1) {
      topSel = botSel = (struct SelRec *)malloc(sizeof(struct SelRec));
      if (topSel == NULL) FailAllocMessage();
      topObj = botObj = obj_ptr1;
      topSel->obj = obj_ptr1;
      topSel->next = topSel->prev = NULL;
      numObjSelected = 1;

      GroupSingleObj(TRUE);
      obj_ptr = topSel->obj;
      RemoveAllSel();
   } else {
      if (obj_ptr2->fattr != NULL) {
         obj_ptr = obj_ptr2;
         obj_ptr2 = obj_ptr1;
         obj_ptr1 = obj_ptr;
      }
      cx1 = ((obj_ptr1->obbox.ltx+obj_ptr1->obbox.rbx)>>1);
      cy1 = ((obj_ptr1->obbox.lty+obj_ptr1->obbox.rby)>>1);
      cx2 = ((obj_ptr2->obbox.ltx+obj_ptr2->obbox.rbx)>>1);
      cy2 = ((obj_ptr2->obbox.lty+obj_ptr2->obbox.rby)>>1);
      MoveObj(obj_ptr2, cx1-cx2, cy1-cy2);
      obj_ptr1->next = obj_ptr2;
      obj_ptr1->prev = NULL;
      obj_ptr2->next = NULL;
      obj_ptr2->prev = obj_ptr1;
      CreateGroupObj(obj_ptr1, obj_ptr2);
      obj_ptr = topObj;
   }
   AdjObjBBox(obj_ptr);
   obj_ptr->type = OBJ_PIN;

   curPage->top = topObj = saved_top_obj;
   curPage->bot = botObj = saved_bot_obj;

   if (!gnPinWarning) {
      gnPinWarning = TRUE;
      strcpy(gszMsgBox, TgLoadString(STID_WARN_PIN_NOT_SUPPORTED));
      if (PRTGIF) {
         fprintf(stderr, "%s\n", gszMsgBox);
      } else {
         MsgBox(gszMsgBox, TOOL_NAME, STOP_MB);
      }
   }
   return obj_ptr;
}

struct ObjRec *ReadSymbol(FP, FileType)
   FILE *FP;
   int FileType;
{
   register struct AttrRec *attr_ptr;
   struct ObjRec *obj_ptr, *obj_ptr1=NULL, *obj_ptr2=NULL;
   int read_status, sym_count=0;

   SetWatchCursor(drawWindow);
   SetWatchCursor(mainWindow);

   importingFile = TRUE; /* ignore 'state' info but set fileVersion */
   readingPageNum = loadedCurPageNum = 0;
   foundGoodStateObject = FALSE;
   while ((read_status=ReadObj(FP, &obj_ptr)) == TRUE) {
      if (obj_ptr != NULL) {
         if (obj_ptr->type == OBJ_SYM) {
            int done=FALSE;

            UnlockAnObj(obj_ptr);
            obj_ptr->type = OBJ_ICON;

            attr_ptr = obj_ptr->lattr;
            for ( ; attr_ptr != NULL; attr_ptr = attr_ptr->prev) {
               attr_ptr->inherited = TRUE;
            }
            if (FileType == SYM_FILE_TYPE) {
               done = TRUE;
            } else {
               if (obj_ptr1 == NULL) {
                  obj_ptr1 = obj_ptr;
               } else if (obj_ptr2 == NULL) {
                  obj_ptr2 = obj_ptr;
               }
               if (++sym_count >= 2) {
                  obj_ptr = CreatePinObj(obj_ptr1, obj_ptr2, sym_count);
                  done = TRUE;
               }
            }
            if (done) {
               importingFile = FALSE;
               SetDefaultCursor(mainWindow);
               ShowCursor();
               return obj_ptr;
            }
         } else {
            FreeObj(obj_ptr);
         }
      }
   }
   importingFile = FALSE;
   if (!PRTGIF && colorLayers && needToRedrawColorWindow) {
      RedrawColorWindow();
   }
   obj_ptr = NULL;
   if (read_status == INVALID) {
      sprintf(gszMsgBox, TgLoadString(STID_FILE_VER_ABORT_READ_SYMBOL),
            fileVersion, TOOL_NAME, homePageURL);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
   } else if (FileType == PIN_FILE_TYPE && sym_count == 1) {
      obj_ptr = CreatePinObj(obj_ptr1, obj_ptr2, sym_count);
   }
   SetDefaultCursor(mainWindow);
   ShowCursor();
   return obj_ptr;
}

struct ObjRec *GetObjRepresentation(PathName, SymName, FileType)
   char *PathName, *SymName;
   int FileType;
{
   char file_name[MAXPATHLENGTH], *rest=NULL;
   char tmp_filename[MAXPATHLENGTH], tmp_filefullpath[MAXPATHLENGTH];
   int short_name, tmp_linenum;
   struct ObjRec *obj_ptr;
   FILE *fp;

   sprintf(file_name, "%s%c%s.%s", PathName, DIR_SEP, SymName, SYM_FILE_EXT);
   if ((short_name=IsPrefix(bootDir, file_name, &rest))) ++rest;

   if ((fp=fopen(file_name, "r")) == NULL) {
      sprintf(gszMsgBox, TgLoadString(STID_CANNOT_OPEN_FILE_FOR_READING),
            (short_name ? rest : file_name));
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return NULL;
   }

   strcpy(tmp_filefullpath, scanFileFullPath);
   strcpy(tmp_filename, scanFileName);
   tmp_linenum = scanLineNum;
   UtilStrCpyN(scanFileFullPath, sizeof(scanFileFullPath), file_name);
   strcpy(scanFileName, (short_name ? rest : file_name));
   scanLineNum = 0;

   if ((obj_ptr=ReadSymbol(fp, FileType)) != NULL) {
      obj_ptr->id = objId++;
      obj_ptr->dirty = FALSE;
      strcpy(obj_ptr->detail.r->s, SymName);
      obj_ptr->detail.r->rotate = ROTATE0;
      obj_ptr->detail.r->flip = NO_FLIP;
      obj_ptr->detail.r->deck_index = (-1);
      obj_ptr->detail.r->pin_connected = 0;
      obj_ptr->detail.r->first_conn = obj_ptr->detail.r->last_conn = NULL;
      AdjObjBBox(obj_ptr);
   }

   strcpy(scanFileFullPath, tmp_filefullpath);
   strcpy(scanFileName, tmp_filename);
   scanLineNum = tmp_linenum;

   fclose(fp);
   return obj_ptr;
}

unsigned int PlaceTopObj(ObjPtr, savedTopObj, savedBotObj)
   struct ObjRec *ObjPtr, *savedTopObj, *savedBotObj;
{
   int x, y, grid_x, grid_y, dx, dy, placing=TRUE;
   int cursor_x, cursor_y, orig_x, orig_y, obj_ltx, obj_lty, obj_rbx, obj_rby;
   int grid_obj_ltx, grid_obj_lty, grid_dx=0, grid_dy=0;
   Window root_win, child_win;
   int root_x, root_y, first_time=TRUE;
   unsigned int status, button_pressed=Button1;
   XEvent input, ev;

   RedrawMsg(TRUE);

   XFlush(mainDisplay);
   XSync(mainDisplay, False);

   placingTopObj = TRUE;
   if (XCheckMaskEvent(mainDisplay, ExposureMask, &ev) ||
         XCheckMaskEvent(mainDisplay, VisibilityChangeMask, &ev)) {
      ExposeEventHandler(&ev, TRUE);
   }
   obj_ltx = OFFSET_X(ObjPtr->obbox.ltx); obj_lty = OFFSET_Y(ObjPtr->obbox.lty);
   obj_rbx = OFFSET_X(ObjPtr->obbox.rbx); obj_rby = OFFSET_Y(ObjPtr->obbox.rby);
   GridXY(obj_ltx, obj_lty, &grid_obj_ltx, &grid_obj_lty);

   dx = dy = 0;
   if (!debugNoPointerGrab) {
      XGrabPointer(mainDisplay, drawWindow, False,
            PointerMotionMask | ButtonPressMask,
            GrabModeAsync, GrabModeAsync, None, handCursor, CurrentTime);
   }
   if (XCheckMaskEvent(mainDisplay, PointerMotionMask, &input)) {
      first_time = FALSE;

      cursor_x = input.xmotion.x;
      cursor_y = input.xmotion.y;
      while (XCheckMaskEvent(mainDisplay, PointerMotionMask, &input)) {
         cursor_x = input.xmotion.x;
         cursor_y = input.xmotion.y;
      }
      GridXY(cursor_x, cursor_y, &orig_x, &orig_y);
      grid_dx = orig_x-grid_obj_ltx; grid_dy = orig_y-grid_obj_lty;
      SelBox(drawWindow, revDefaultGC, obj_ltx+grid_dx, obj_lty+grid_dy,
            obj_rbx+grid_dx, obj_rby+grid_dy);
   }

   while (placing) {
      XNextEvent(mainDisplay, &input);

      if (first_time) {
         first_time = FALSE;

         XQueryPointer(mainDisplay, drawWindow, &root_win, &child_win,
               &root_x, &root_y, &cursor_x, &cursor_y, &status);

         GridXY(cursor_x, cursor_y, &orig_x, &orig_y);
         grid_dx = orig_x-grid_obj_ltx; grid_dy = orig_y-grid_obj_lty;
         SelBox(drawWindow, revDefaultGC, obj_ltx+grid_dx, obj_lty+grid_dy,
               obj_rbx+grid_dx, obj_rby+grid_dy);
      }

      if (input.type == Expose || input.type == VisibilityNotify) {
/*       SelBox(drawWindow, revDefaultGC, obj_ltx+grid_dx, obj_lty+grid_dy,
               obj_rbx+grid_dx, obj_rby+grid_dy); */
         if (savedTopObj != NULL && savedBotObj != NULL) {
            struct ObjRec *saved_top_obj=topObj, *saved_bot_obj=botObj;

            curPage->top = topObj = NULL;
            curPage->bot = botObj = NULL;
            curPage->top = topObj = savedTopObj;
            curPage->bot = botObj = savedBotObj;
            placingTopObj = FALSE;
            RedrawDrawWindow(botObj);
            placingTopObj = TRUE;
            curPage->top = topObj = saved_top_obj;
            curPage->bot = botObj = saved_bot_obj;
         }
         ExposeEventHandler(&input, TRUE);
         SelBox(drawWindow, revDefaultGC, obj_ltx+grid_dx, obj_lty+grid_dy,
               obj_rbx+grid_dx, obj_rby+grid_dy);
      } else if (input.type == ButtonPress) {
         XUngrabPointer(mainDisplay, CurrentTime);
         button_pressed = input.xbutton.button;
         placing = FALSE;
         SelBox(drawWindow, revDefaultGC, obj_ltx+grid_dx+dx,
               obj_lty+grid_dy+dy, obj_rbx+grid_dx+dx, obj_rby+grid_dy+dy);
         grid_dx = ABS_SIZE(grid_dx+dx);
         grid_dy = ABS_SIZE(grid_dy+dy);
         MoveObj(ObjPtr, grid_dx, grid_dy);
         numRedrawBBox = 0;
         ShowInterrupt(1);
         placingTopObj = FALSE;
         ObjPtr->tmp_parent = NULL;
         DrawObj(drawWindow, ObjPtr);
         HideInterrupt();
      } else if (input.type == MotionNotify) {
         int anywhere=(input.xmotion.state & (ShiftMask | ControlMask));
         int saved_snap=snapOn;

         x = input.xmotion.x;
         y = input.xmotion.y;
         if (anywhere) snapOn = FALSE;
         GridXY(x, y, &grid_x, &grid_y);
         if (anywhere) snapOn = saved_snap;
         SelBox(drawWindow, revDefaultGC, obj_ltx+grid_dx+dx,
               obj_lty+grid_dy+dy, obj_rbx+grid_dx+dx, obj_rby+grid_dy+dy);
         dx = grid_x - orig_x;
         dy = grid_y - orig_y;
         SelBox(drawWindow, revDefaultGC, obj_ltx+grid_dx+dx,
               obj_lty+grid_dy+dy, obj_rbx+grid_dx+dx, obj_rby+grid_dy+dy);
         MarkRulers(grid_x, grid_y);
         while (XCheckMaskEvent(mainDisplay, PointerMotionMask, &ev)) ;
      }
   }
   XSync(mainDisplay, True);
   placingTopObj = FALSE;
   return button_pressed;
}

static
int UnMakeIconicOnInstantiate(obj_ptr)
   struct ObjRec *obj_ptr;
{
   register struct AttrRec *attr_ptr;

   for (attr_ptr=obj_ptr->lattr; attr_ptr!=NULL; attr_ptr=attr_ptr->prev) {
      if (*attr_ptr->attr_name.s=='\0' &&
            (strcmp(attr_ptr->attr_value.s, "unmakeiconic_on_instantiate")==0 ||
            strcmp(attr_ptr->attr_value.s,
            "unmakeiconic_on_instantiate_delete_attrs")==0)) {
         return TRUE;
      }
   }
   return FALSE;
}

static
int UnMakeIconicOnInstantiateDeleteAttrs(obj_ptr)
   struct ObjRec *obj_ptr;
{
   register struct AttrRec *attr_ptr;

   for (attr_ptr=obj_ptr->lattr; attr_ptr!=NULL; attr_ptr=attr_ptr->prev) {
      if (*attr_ptr->attr_name.s=='\0' && strcmp(attr_ptr->attr_value.s,
            "unmakeiconic_on_instantiate_delete_attrs")==0) {
         return TRUE;
      }
   }
   return FALSE;
}

static
void DeleteUnMakeIconicOnInstantiateText()
{
   struct SelRec *sel_ptr;
   struct ObjRec *text_obj_ptr=NULL;

   for (sel_ptr=botSel; sel_ptr != NULL; sel_ptr=sel_ptr->prev) {
      if (sel_ptr->obj->type == OBJ_TEXT) {
         struct DynStrRec *dyn_str_ptr=GetTextFirstDynStr(sel_ptr->obj);

         if (strcmp(dyn_str_ptr->s, "unmakeiconic_on_instantiate") == 0) {
            text_obj_ptr = sel_ptr->obj;
            break;
         }
      }
   }
   if (text_obj_ptr == NULL) {
      sprintf(gszMsgBox, TgLoadString(STID_CANT_FIND_NAMED_OBJ_IN_FUNC),
            "unmakeiconic_on_instantiate",
            "DeleteUnMakeIconicOnInstantiateText()");
      TgAssert(FALSE, gszMsgBox, NULL);
      XFlush(mainDisplay);
      XSync(mainDisplay, False);
      return;
   }
   MakeQuiescent();
   topSel = (struct SelRec *)malloc(sizeof(struct SelRec));
   if (topSel == NULL) FailAllocMessage();
   topSel->next = NULL;
   topSel->obj = text_obj_ptr;
   topSel->prev = NULL;
   botSel = topSel;
   UpdSelBBox();
   HighLightForward();
   DelAllSelObj();
}

void Instantiate()
{
   char file_name[MAXPATHLENGTH], *rest=NULL, msg[MAXSTRING];
   char sym_name[MAXPATHLENGTH], path_name[MAXPATHLENGTH];
   char ext_str[MAXPATHLENGTH];
   char tmp_filename[MAXPATHLENGTH], tmp_filefullpath[MAXPATHLENGTH];
   int short_name=FALSE, file_type=INVALID, tmp_linenum=0, len=0, ext_len=0;
   struct ObjRec *obj_ptr=NULL;
   FILE *fp=NULL;
   XEvent ev;

   MakeQuiescent();

   if (*curDomainName == '\0') {
      strcpy(msg, TgLoadString(STID_SEL_A_SYM_TO_INSTAN_IN_NONE));
   } else {
      sprintf(msg, TgLoadString(STID_SEL_A_SYM_TO_INSTAN_IN_DOMAIN),
            curDomainName);
   }
   if (SelectFromLibrary(msg, SYM_FILE_EXT, sym_name, path_name) == INVALID) {
      return;
   }
   if (XCheckMaskEvent(mainDisplay, ExposureMask, &ev) ||
         XCheckMaskEvent(mainDisplay, VisibilityChangeMask, &ev)) {
      ExposeEventHandler(&ev, TRUE);
   }
   sprintf(file_name, "%s%c%s", path_name, DIR_SEP, sym_name);

   len = strlen(sym_name);
   sprintf(ext_str, ".%s", SYM_FILE_EXT);
   ext_len = strlen(ext_str);
   if (len > ext_len && strcmp(ext_str, &sym_name[len-ext_len]) == 0) {
      sym_name[len-ext_len] = '\0';
      file_type = SYM_FILE_TYPE;
   } else {
      sprintf(ext_str, ".%s", PIN_FILE_EXT);
      ext_len = strlen(ext_str);
      if (len > ext_len && strcmp(ext_str, &sym_name[len-ext_len]) == 0) {
         sym_name[len-ext_len] = '\0';
         file_type = PIN_FILE_TYPE;
      }
   }
   if ((short_name=IsPrefix(bootDir, file_name, &rest))) ++rest;

   if (file_type == INVALID) {
      sprintf(gszMsgBox, TgLoadString(STID_CANT_INSTAN_FILE_UNKNOWN_EXT),
            (short_name ? rest : file_name));
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return;
   }
   if ((fp=fopen(file_name, "r")) == NULL) {
      sprintf(gszMsgBox, TgLoadString(STID_CANNOT_OPEN_FILE_FOR_READING),
            (short_name ? rest : file_name));
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return;
   }
   strcpy(tmp_filefullpath, scanFileFullPath);
   strcpy(tmp_filename, scanFileName);
   tmp_linenum = scanLineNum;
   UtilStrCpyN(scanFileFullPath, sizeof(scanFileFullPath), file_name);
   strcpy(scanFileName, (short_name ? rest : file_name));
   scanLineNum = 0;

   SetWatchCursor(drawWindow);
   SetWatchCursor(mainWindow);
   if ((obj_ptr=ReadSymbol(fp, file_type)) != NULL) {
      char *psz=NULL;
      struct AttrRec *name_attr=NULL;

      sprintf(msg, TgLoadCachedString(CSTID_INSTANTIATING_FILE_DOTS),
            (short_name ? rest : file_name));
      SetStringStatus(msg);
      obj_ptr->id = objId++;
      obj_ptr->dirty = FALSE;
      strcpy(obj_ptr->detail.r->s, sym_name);
      obj_ptr->detail.r->rotate = ROTATE0;
      obj_ptr->detail.r->flip = NO_FLIP;
      obj_ptr->detail.r->deck_index = (-1);
      obj_ptr->detail.r->pin_connected = 0;
      obj_ptr->detail.r->first_conn = obj_ptr->detail.r->last_conn = NULL;
      name_attr = FindAttrWithName(obj_ptr, "name=", NULL);
      if (name_attr != NULL &&
            (psz=strstr(name_attr->attr_value.s, "%d")) != NULL &&
            (strstr(++psz, "%d")) == NULL) {
         sprintf(gszMsgBox, name_attr->attr_value.s, obj_ptr->id-1);
         DynStrSet(&name_attr->attr_value, gszMsgBox);
         UpdAttr(name_attr);
         UpdTextBBox(name_attr->obj);
      }
      AssignNewObjIds(obj_ptr);
      AddObj(NULL, topObj, obj_ptr);
      AdjObjBBox(obj_ptr);
      PlaceTopObj(obj_ptr, NULL, NULL);

      SelectTopObj();
      if (UnMakeIconicOnInstantiate(obj_ptr)) {
         StartCompositeCmd();
         RecordNewObjCmd();
         UnMakeIconic();
         if (UnMakeIconicOnInstantiateDeleteAttrs(obj_ptr)) {
            gnDeleteAttrsWhileUngrouping = TRUE;
            UngroupSelObj(TRUE, TRUE);
            gnDeleteAttrsWhileUngrouping = FALSE;
         } else {
            UngroupSelObj(TRUE, TRUE);
            DeleteUnMakeIconicOnInstantiateText();
         }
         EndCompositeCmd();
      } else {
         RecordNewObjCmd();
      }
      SetFileModified(TRUE);
      justDupped = FALSE;
   } else {
      sprintf(gszMsgBox, TgLoadString(STID_CANT_FND_SYM_IN_NAMED_TO_INST),
            (short_name ? rest : file_name));
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
   }
   fclose(fp);

   strcpy(scanFileFullPath, tmp_filefullpath);
   strcpy(scanFileName, tmp_filename);
   scanLineNum = tmp_linenum;

   SetDefaultCursor(mainWindow);
   SetDefaultCursor(drawWindow);
   Msg("");

   if (topSel != NULL && topSel == botSel && topSel->obj->type == OBJ_ICON) {
      /* something has been instantiated */
      char *psz=NULL;
      struct AttrRec *name_attr=NULL, *exec_attr=NULL;

      name_attr = FindAttrWithName(topSel->obj, "name=", NULL);
      if (name_attr != NULL &&
            (psz=strstr(name_attr->attr_value.s, "%d")) != NULL &&
            (strstr(++psz, "%d")) == NULL) {
         sprintf(gszMsgBox, name_attr->attr_value.s, obj_ptr->id-1);
         DynStrSet(&name_attr->attr_value, gszMsgBox);
         UpdAttr(name_attr);
         UpdTextBBox(name_attr->obj);
         AdjObjBBox(name_attr->obj);
         AdjObjCache(name_attr->obj);
      }
      exec_attr = FindAttrWithName(topSel->obj, "on_instantiate=", NULL);
      if (exec_attr != NULL) {
         DoExecLoop(topSel->obj, exec_attr);
      }
   }
/*
 * Need to check for pins.
 */
}

int ObjIsAPort(obj_ptr)
   struct ObjRec *obj_ptr;
{
   struct AttrRec *attr_ptr=NULL;

   attr_ptr = FindAttrWithName(obj_ptr, "type=", NULL);
   if (attr_ptr != NULL && strcmp(attr_ptr->attr_value.s, "port") == 0) {
      /*
       * if this is not called from ConnectTwoPortsByAWire() but from
       * an internal command, "type=port" is enough for a point
       */
      if (connectingPortsFromInternalCommand) return TRUE;

      if (FindAttrWithName(obj_ptr, "signal_name=", NULL) != NULL &&
            FindAttrWithName(obj_ptr, "name=", NULL) != NULL) {
         struct ObjRec *owner_obj=obj_ptr->tmp_parent, *tmp_obj=NULL;

         owner_obj = obj_ptr->tmp_parent;
         if (owner_obj == NULL) {
            /* obj_ptr is a top-level object */
            return (obj_ptr->type == OBJ_ICON || obj_ptr->type == OBJ_SYM);
         } else {
            if (owner_obj->type == OBJ_ICON || owner_obj->type == OBJ_SYM) {
               if (FindAttrWithName(owner_obj, "name=", NULL) == NULL) {
                  return FALSE;
               }
            } else {
               if (owner_obj->type == OBJ_GROUP) {
                  attr_ptr = FindAttrWithName(owner_obj, "type=", NULL);
                  if (attr_ptr != NULL && strcmp(attr_ptr->attr_value.s,
                        "tgBroadcastWire") == 0) {
                     return TRUE;
                  }
               }
               return FALSE;
            }
         }
         for (tmp_obj=owner_obj->tmp_parent; tmp_obj != NULL;
               tmp_obj=tmp_obj->tmp_parent) {
            if (tmp_obj->type == OBJ_ICON || tmp_obj->type == OBJ_SYM) {
               return FALSE;
            }
         }
         return TRUE;
      }
   }
   return FALSE;
}

static
int DoConnectTwoPorts()
{
   int poly_created=FALSE;
   XGCValues values;

   MakeQuiescent();

   ResetWiringNodeInfo();
   connectingPortsByWire = TRUE;
   connectingPortsFromInternalCommand = FALSE;
   UseWireMenuItemInModeItem(TRUE);

   values.line_width = 3;
   XChangeGC(mainDisplay, revGrayGC, GCLineWidth, &values);
   SetCurChoice(DRAWPOLY);
   gstWiringInfo.num_ports_to_connect = 2;
   drawPolyHighlightedNode = NULL;
   SetStringStatus(TgLoadString(STID_DRAW_AN_EDGE_BETWEEN_PORTS));
   polyDrawn = FALSE;
   for (;;) {
      XEvent input;

      XNextEvent(mainDisplay, &input);
      if (input.type == Expose || input.type == VisibilityNotify) {
         ExposeEventHandler(&input, TRUE);
      } else if (input.type == ConfigureNotify) {
         Reconfigure(FALSE);
      } else if (input.xany.window == drawWindow) {
         polyDrawn = FALSE;
         DrawingEventHandler(&input);
         if (curChoice == DRAWPOLY) {
            if (polyDrawn) {
               break;
            } else if (gstWiringInfo.num_ports_to_connect == (-1)) {
               break;
            }
         } else {
            polyDrawn = FALSE;
            break;
         }
      }
   }
   gstWiringInfo.num_ports_to_connect = 0;
   poly_created = polyDrawn;
   values.line_width = 1;
   XChangeGC(mainDisplay, revGrayGC, GCLineWidth, &values);
   connectingPortsByWire = FALSE;
   connectingPortsFromInternalCommand = TRUE;
   SetCurChoice(NOTHING);
   RedrawModeWindow();
   UseWireMenuItemInModeItem(FALSE);
   UpdatePinnedMenu(MENU_MODE);

   return poly_created;
}

static
void AddWireAttributes(poly_obj_ptr, signal_name, p_obbox, auto_center)
   struct ObjRec *poly_obj_ptr;
   char *signal_name;
   struct BBRec *p_obbox;
   int auto_center;
{
   struct ObjRec *text_obj_ptr=NULL;

   PrepareToReplaceAnObj(poly_obj_ptr);

   AddAttrByNameAndValue(poly_obj_ptr, "signal_name=", signal_name);
   poly_obj_ptr->fattr->shown = TRUE;
   poly_obj_ptr->fattr->nameshown = FALSE;
   UpdAttr(poly_obj_ptr->fattr);
   text_obj_ptr = poly_obj_ptr->fattr->obj;
   if (auto_center) {
      CenterObjInOBBox(text_obj_ptr, *p_obbox, NULL);
      memcpy(p_obbox, &text_obj_ptr->obbox, sizeof(struct BBRec));
      poly_obj_ptr->fattr->shown = FALSE;
   } else {
      MoveObj(text_obj_ptr, p_obbox->ltx-text_obj_ptr->obbox.ltx,
            p_obbox->lty-text_obj_ptr->obbox.lty);
   }
   AddAttrByNameAndValue(poly_obj_ptr, "type=", "tgWire");
   text_obj_ptr = poly_obj_ptr->fattr->obj;
   MoveObj(text_obj_ptr, p_obbox->ltx-text_obj_ptr->obbox.ltx,
         p_obbox->rby-text_obj_ptr->obbox.lty);

   AdjObjBBox(poly_obj_ptr);

   RecordReplaceAnObj(poly_obj_ptr);
}

typedef struct tagChangeConnectionViewInfo {
   /* only change view if do_change_view is TRUE */
   int do_change_view;
   int change_to_connect_view;
   struct ObjRec *conn_view_obj, *disconn_view_obj;
   int conn_fill, conn_pen, disconn_fill, disconn_pen;
   /* _TANGRAM_II stuff */
   struct ObjRec *port_obj;
   char *signal_name;
} ChangeConnectionViewInfo;

static
int ChangeConnectionView(pravci)
   ReplAttrValueCallbackInfo *pravci;
{
   ChangeConnectionViewInfo *pccvi=(
         (ChangeConnectionViewInfo*)pravci->pv_userdata);

   if (pccvi == NULL) return FALSE;

   if (pccvi->do_change_view) {
      if (pccvi->change_to_connect_view) {
         ChangeObjFill(pccvi->disconn_view_obj, NONEPAT);
         ChangeObjPen(pccvi->disconn_view_obj, NONEPAT);
         ChangeObjFill(pccvi->conn_view_obj, pccvi->conn_fill);
         ChangeObjPen(pccvi->conn_view_obj, pccvi->conn_pen);
      } else {
         ChangeObjFill(pccvi->conn_view_obj, NONEPAT);
         ChangeObjPen(pccvi->conn_view_obj, NONEPAT);
         ChangeObjFill(pccvi->disconn_view_obj, pccvi->disconn_fill);
         ChangeObjPen(pccvi->disconn_view_obj, pccvi->disconn_pen);
      }
   }
   if (cmdLineTgrm2) {
      Tangram2UpdateInitAttr(pccvi->port_obj, pccvi->signal_name);
   }
   return TRUE;
}

static
int HasConnAndDisConnViewObjSubObjs(obj_ptr, pccvi)
   struct ObjRec *obj_ptr;
   ChangeConnectionViewInfo *pccvi;
{
   struct ObjRec *subobj=NULL;

   if (obj_ptr == NULL) return FALSE;
   if (obj_ptr->type != OBJ_SYM && obj_ptr->type != OBJ_ICON &&
         obj_ptr->type != OBJ_GROUP) {
      return FALSE;
   }
   for (subobj=obj_ptr->detail.r->first; subobj != NULL; subobj=subobj->next) {
      struct AttrRec *attr_ptr=FindAttrWithName(subobj, "view=", NULL);

      if (attr_ptr != NULL) {
         char *psz_fill=NULL, *psz_pen=NULL, buf[MAXSTRING];

         UtilStrCpyN(buf, sizeof(buf), attr_ptr->attr_value.s);
         psz_fill = strchr(buf, ',');
         if (psz_fill != NULL) {
            *psz_fill++ = '\0';
            psz_pen = strchr(psz_fill, ',');
            if (psz_pen != NULL) *psz_pen++ = '\0';
         }
         if (psz_fill != NULL && psz_pen != NULL) {
            int fill=0, pen=0, ok=TRUE;

            UtilTrimBlanks(buf);
            UtilTrimBlanks(psz_fill);
            UtilTrimBlanks(psz_pen);
            if (sscanf(psz_fill, "%d", &fill) != 1) ok = FALSE;
            if (sscanf(psz_pen, "%d", &pen) != 1) ok = FALSE;
            if (ok) {
               fill = fill % MAXPATTERNS;
               pen = pen % MAXPATTERNS;
               if (ok && UtilStrICmp(buf, "conn") == 0) {
                  pccvi->conn_fill = fill;
                  pccvi->conn_pen = pen;
                  pccvi->conn_view_obj = subobj;
               } else if (ok && UtilStrICmp(buf, "disconn") == 0) {
                  pccvi->disconn_fill = fill;
                  pccvi->disconn_pen = pen;
                  pccvi->disconn_view_obj = subobj;
               }
            }
         }
      }
   }
   return (pccvi->conn_view_obj != NULL && pccvi->disconn_view_obj != NULL);
}

static
int SetPortSignalName(port_obj, attr_ptr, attr_owner_obj, new_signal_name)
   struct ObjRec *port_obj, *attr_owner_obj;
   struct AttrRec *attr_ptr;
   char *new_signal_name;
{
   if (attr_ptr != NULL &&
         strcmp(new_signal_name, attr_ptr->attr_value.s) != 0) {
      char *old_signal_name=attr_ptr->attr_value.s;
      ChangeConnectionViewInfo ccvi;

      memset(&ccvi, 0, sizeof(ChangeConnectionViewInfo));
      if ((*old_signal_name == '\0' || *new_signal_name == '\0') &&
            HasConnAndDisConnViewObjSubObjs(attr_ptr->owner, &ccvi)) {
         ccvi.do_change_view = TRUE;
         if (*old_signal_name == '\0') {
            /* the port will become connected */
            ccvi.change_to_connect_view = TRUE;
         } else {
            /* the port will become disconnected */
            ccvi.change_to_connect_view = FALSE;
         }
      }
      ccvi.port_obj = port_obj;
      ccvi.signal_name = new_signal_name;
      SetReplaceAttrValueCallback(
            (ReplAttrValueCallback*)ChangeConnectionView, &ccvi);

      ReplaceAttrFirstValue(attr_owner_obj, attr_ptr, new_signal_name);
      SetReplaceAttrValueCallback(NULL, NULL);
      RecursivelyAdjObjBBox(attr_ptr->owner, attr_ptr->owner,
            attr_owner_obj);
      return TRUE;
   }
   return FALSE;
}

static
void DoNameWire(automatic)
   int automatic;
{
   struct AttrRec *first_attr_ptr=NULL, *last_attr_ptr=NULL;
   struct ObjRec *first_attr_owner_obj=NULL, *last_attr_owner_obj=NULL;
   struct BBRec bbox;
   char signal_name[MAXSTRING], *existing_signal_name=NULL;
   int need_to_clear=FALSE, need_to_redraw=FALSE;
   int do_prompt=TRUE, must_prompt=FALSE;

   memcpy(&bbox, &topObj->bbox, sizeof(struct BBRec));
   first_attr_ptr = FindAttrWithName(gstWiringInfo.first_port_obj,
         "signal_name=", &first_attr_owner_obj);
   while (first_attr_owner_obj != NULL &&
         first_attr_owner_obj->tmp_parent != NULL) {
      first_attr_owner_obj = first_attr_owner_obj->tmp_parent;
   }
   last_attr_ptr = FindAttrWithName(gstWiringInfo.last_port_obj,
         "signal_name=", &last_attr_owner_obj);
   while (last_attr_owner_obj != NULL &&
         last_attr_owner_obj->tmp_parent != NULL) {
      last_attr_owner_obj = last_attr_owner_obj->tmp_parent;
   }
   if (first_attr_ptr == NULL && last_attr_ptr == NULL) return;
   if (first_attr_ptr != NULL && last_attr_ptr != NULL) {
      if (strcmp(first_attr_ptr->attr_value.s, last_attr_ptr->attr_value.s) !=
            0) {
         if (*first_attr_ptr->attr_value.s == '\0' &&
               *last_attr_ptr->attr_value.s == '\0') {
            must_prompt = TRUE;
         } else if (*first_attr_ptr->attr_value.s == '\0') {
            existing_signal_name = last_attr_ptr->attr_value.s;
         } else if (*last_attr_ptr->attr_value.s == '\0') {
            existing_signal_name = first_attr_ptr->attr_value.s;
         } else {
            sprintf(gszMsgBox, TgLoadString(STID_Q_CNFLCT_SIG_NAME_OKAY_TO_CLR),
                  first_attr_ptr->attr_value.s, gstWiringInfo.first_port_name,
                  last_attr_ptr->attr_value.s, gstWiringInfo.last_port_name);
            switch (MsgBox(gszMsgBox, TOOL_NAME, YNC_MB)) {
            case MB_ID_YES:
               need_to_clear = TRUE;
               must_prompt = TRUE;
               break;
            case MB_ID_NO:
            case MB_ID_CANCEL:
               return;
            }
         }
      } else {
         existing_signal_name = first_attr_ptr->attr_value.s;
      }
   } else if (first_attr_ptr == NULL) {
      if (*last_attr_ptr->attr_value.s != '\0') {
         existing_signal_name = last_attr_ptr->attr_value.s;
      }
   } else {
      if (*first_attr_ptr->attr_value.s != '\0') {
         existing_signal_name = first_attr_ptr->attr_value.s;
      }
   }
   StartCompositeCmd();
   if (need_to_clear) {
      if (SetPortSignalName(gstWiringInfo.first_port_obj, first_attr_ptr,
            first_attr_owner_obj, "")) {
         UnionRect(&bbox, &first_attr_owner_obj->bbox, &bbox);
         need_to_redraw = TRUE;
      }
      if (SetPortSignalName(gstWiringInfo.last_port_obj, last_attr_ptr,
            last_attr_owner_obj, "")) {
         UnionRect(&bbox, &last_attr_owner_obj->bbox, &bbox);
         need_to_redraw = TRUE;
      }
   }
   if (do_prompt) {
      struct ObjRec *signal_name_obj=NULL;
      struct BBRec obbox;
      XEvent ev;
      int show_wire_signal_name=(!automatic && showWireSignalName);

      *signal_name = '\0';
      if (existing_signal_name != NULL) {
         UtilStrCpyN(signal_name, sizeof(signal_name), existing_signal_name);
      }
      UtilTrimBlanks(signal_name);
      if (must_prompt || !automatic) {
         Dialog(TgLoadString(STID_PLS_ENT_SIG_NAME), NULL, signal_name);
         UtilTrimBlanks(signal_name);
      }
      if (*signal_name == '\0') {
         if (need_to_clear) {
            sprintf(gszMsgBox, TgLoadString(STID_SIGNAL_NAMES_CLEARED),
                  gstWiringInfo.first_port_name, gstWiringInfo.last_port_name);
            MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
         }
         EndCompositeCmd();

         if (need_to_redraw) {
            SetFileModified(TRUE);
            justDupped = FALSE;
            RedrawAnArea(botObj,
                  bbox.ltx-QUARTER_INCH-GRID_ABS_SIZE(1),
                  bbox.lty-QUARTER_INCH-GRID_ABS_SIZE(1),
                  bbox.rbx+QUARTER_INCH+GRID_ABS_SIZE(1),
                  bbox.rby+QUARTER_INCH+GRID_ABS_SIZE(1));
         }
         return;
      }
      if (show_wire_signal_name) {
         unsigned int button_pressed=Button1;

         SaveStatusStrings();
         sprintf(gszMsgBox, TgLoadString(STID_PLACING_NAMED_SIGNAL),
               signal_name);
         SetStringStatus(gszMsgBox);
         button_pressed = PasteString(signal_name, FALSE, FALSE);
         RestoreStatusStrings();

         if (button_pressed != Button1) show_wire_signal_name = FALSE;

         memcpy(&obbox, &topObj->obbox, sizeof(struct BBRec));
         signal_name_obj = topObj;
         UnlinkObj(signal_name_obj);
         FreeObj(signal_name_obj);

         XSync(mainDisplay, False);
         if (XCheckMaskEvent(mainDisplay, ExposureMask, &ev)) {
            ExposeEventHandler(&ev, TRUE);
         }
      } else {
         memcpy(&obbox, &topObj->obbox, sizeof(struct BBRec));
      }
      AddWireAttributes(topObj, signal_name, &obbox, !show_wire_signal_name);
      UnionRect(&bbox, &topObj->bbox, &bbox);
      if (SetPortSignalName(gstWiringInfo.first_port_obj, first_attr_ptr,
            first_attr_owner_obj, signal_name)) {
         UnionRect(&bbox, &first_attr_owner_obj->bbox, &bbox);
         need_to_redraw = TRUE;
      }
      if (SetPortSignalName(gstWiringInfo.last_port_obj, last_attr_ptr,
            last_attr_owner_obj, signal_name)) {
         UnionRect(&bbox, &last_attr_owner_obj->bbox, &bbox);
         need_to_redraw = TRUE;
      }
   }
   EndCompositeCmd();

   if (need_to_redraw) {
      SetFileModified(TRUE);
      justDupped = FALSE;
      RedrawAnArea(botObj,
            bbox.ltx-QUARTER_INCH-GRID_ABS_SIZE(1),
            bbox.lty-QUARTER_INCH-GRID_ABS_SIZE(1),
            bbox.rbx+QUARTER_INCH+GRID_ABS_SIZE(1),
            bbox.rby+QUARTER_INCH+GRID_ABS_SIZE(1));
      if (topSel!=NULL) HighLightForward();
   }
}

void ConnectTwoPortsByAWire()
{
   StartCompositeCmd();
   if (DoConnectTwoPorts()) {
      DoNameWire(FALSE);
   }
   EndCompositeCmd();
}

void RepeatConnectTwoPortsByAWire()
{
   StartCompositeCmd();
   while (DoConnectTwoPorts()) {
      DoNameWire(TRUE);

      if (topSel!=NULL) { HighLightReverse(); RemoveAllSel(); }
   }
   EndCompositeCmd();
}

static
void AddObjToAnchor(anchor, obj_ptr)
   struct ObjRec *anchor, *obj_ptr;
{
   obj_ptr->prev = NULL;
   obj_ptr->next = anchor->detail.r->first;

   anchor->detail.r->first->prev = obj_ptr;
   anchor->detail.r->first = obj_ptr;
   AdjObjOBBox(anchor);
}

static
void AddBroadcastWireAttributes(poly_obj_ptr, signal_name, p_obbox, auto_center)
   struct ObjRec *poly_obj_ptr;
   char *signal_name;
   struct BBRec *p_obbox;
   int auto_center;
{
   struct ObjRec *text_obj_ptr=NULL;

   PrepareToReplaceAnObj(poly_obj_ptr);

   AddAttrByNameAndValue(poly_obj_ptr, "broadcast_signal_name=", signal_name);
   poly_obj_ptr->fattr->shown = TRUE;
   poly_obj_ptr->fattr->nameshown = FALSE;
   UpdAttr(poly_obj_ptr->fattr);
   text_obj_ptr = poly_obj_ptr->fattr->obj;
   if (auto_center) {
      CenterObjInOBBox(text_obj_ptr, *p_obbox, NULL);
      memcpy(p_obbox, &text_obj_ptr->obbox, sizeof(struct BBRec));
      poly_obj_ptr->fattr->shown = FALSE;
   } else {
      MoveObj(text_obj_ptr, p_obbox->ltx-text_obj_ptr->obbox.ltx,
            p_obbox->lty-text_obj_ptr->obbox.lty);
   }
   AdjObjBBox(poly_obj_ptr);

   RecordReplaceAnObj(poly_obj_ptr);
}

static
void DoNameBroadcastWire(signal_name, signal_name_diff)
   char *signal_name;
   int signal_name_diff;
{
   struct ObjRec *obj_ptr=NULL, *signal_name_obj=NULL;
   struct AttrRec *attr_ptr=NULL;
   struct BBRec bbox, obbox;
   int show_wire_signal_name=showWireSignalName;
   int already_has_broadcast_signal_name=FALSE;
   XEvent ev;

   memset(&obbox, 0, sizeof(obbox));
   memcpy(&bbox, &topObj->bbox, sizeof(struct BBRec));

   StartCompositeCmd();

   attr_ptr = FindAttrWithName(topObj, "broadcast_signal_name=", NULL);
   if (attr_ptr != NULL) {
      already_has_broadcast_signal_name = TRUE;
      if (strcmp(attr_ptr->attr_value.s, signal_name) != 0) {
         ReplaceAttrFirstValue(topObj, attr_ptr, signal_name);
         UnionRect(&bbox, &topObj->bbox, &bbox);
      }
   }
   for (obj_ptr=topObj->detail.r->last; obj_ptr != NULL;
         obj_ptr=obj_ptr->prev) {
      if (ObjIsAPort(obj_ptr)) {
         struct ObjRec *attr_owner_obj=NULL;

         attr_ptr = FindAttrWithName(obj_ptr, "signal_name=", &attr_owner_obj);
         while (attr_owner_obj != NULL && attr_owner_obj->tmp_parent != NULL) {
            attr_owner_obj = attr_owner_obj->tmp_parent;
         }
         if (SetPortSignalName(obj_ptr, attr_ptr, attr_owner_obj, "")) {
            UnionRect(&bbox, &attr_owner_obj->bbox, &bbox);
         }
      }
   }
   if (show_wire_signal_name && !already_has_broadcast_signal_name) {
      unsigned int button_pressed=Button1;

      SaveStatusStrings();
      sprintf(gszMsgBox, TgLoadString(STID_PLACING_NAMED_SIGNAL),
            signal_name);
      SetStringStatus(gszMsgBox);
      button_pressed = PasteString(signal_name, FALSE, FALSE);
      RestoreStatusStrings();

      if (button_pressed != Button1) show_wire_signal_name = FALSE;

      memcpy(&obbox, &topObj->obbox, sizeof(struct BBRec));
      signal_name_obj = topObj;
      UnlinkObj(signal_name_obj);
      FreeObj(signal_name_obj);

      XSync(mainDisplay, False);
      if (XCheckMaskEvent(mainDisplay, ExposureMask, &ev)) {
         ExposeEventHandler(&ev, TRUE);
      }
   } else {
      memcpy(&obbox, &topObj->obbox, sizeof(struct BBRec));
   }
   if (!already_has_broadcast_signal_name) {
      AddBroadcastWireAttributes(topObj, signal_name, &obbox,
            !show_wire_signal_name);
      UnionRect(&bbox, &topObj->bbox, &bbox);
   }
   for (obj_ptr=topObj->detail.r->last; obj_ptr != NULL;
         obj_ptr=obj_ptr->prev) {
      if (ObjIsAPort(obj_ptr)) {
         struct ObjRec *attr_owner_obj=NULL;
         struct AttrRec *attr_ptr=NULL;

         attr_ptr = FindAttrWithName(obj_ptr, "signal_name=", &attr_owner_obj);
         while (attr_owner_obj != NULL && attr_owner_obj->tmp_parent != NULL) {
            attr_owner_obj = attr_owner_obj->tmp_parent;
         }
         if (SetPortSignalName(obj_ptr, attr_ptr, attr_owner_obj,
               signal_name)) {
            UnionRect(&bbox, &attr_owner_obj->bbox, &bbox);
         }
      }
   }
   EndCompositeCmd();

   SetFileModified(TRUE);
   justDupped = FALSE;
   RedrawAnArea(botObj,
         bbox.ltx-QUARTER_INCH-GRID_ABS_SIZE(1),
         bbox.lty-QUARTER_INCH-GRID_ABS_SIZE(1),
         bbox.rbx+QUARTER_INCH+GRID_ABS_SIZE(1),
         bbox.rby+QUARTER_INCH+GRID_ABS_SIZE(1));
   if (topSel!=NULL) HighLightForward();
}

static
void TrySetCommonSignalName(attr_ptr, pszSignalName, szSignalName,
      pnSignalNameDiff)
   struct AttrRec *attr_ptr;
   char *pszSignalName;
   int szSignalName, *pnSignalNameDiff;
{
   if (*pnSignalNameDiff) {
      /* already see multiple signal names */
   } else if (*attr_ptr->attr_value.s == '\0') {
      /* nothing to do */
   } else if (*pszSignalName == '\0') {
      UtilStrCpyN(pszSignalName, szSignalName, attr_ptr->attr_value.s);
   } else if (strcmp(pszSignalName, attr_ptr->attr_value.s) != 0) {
      (*pnSignalNameDiff) = TRUE;
      *pszSignalName = '\0';
   }
}

static
int CheckPortsAndBroadcastObj(ppAnchor, pnAnchorIsPoly, pnNumPorts,
      pszSignalName, szSignalName, pnSignalNameDiff,
      pnAlreadyHasBroadcastSignalName)
   struct ObjRec **ppAnchor;
   int *pnAnchorIsPoly, *pnNumPorts, szSignalName, *pnSignalNameDiff;
   int *pnAlreadyHasBroadcastSignalName;
   char *pszSignalName;
{
   int num_ports=0, anchor_is_poly=FALSE, ok=TRUE, signal_name_diff=FALSE;
   int already_has_broadcast_signal_name=FALSE;
   struct ObjRec *anchor=NULL;
   struct SelRec *sel_ptr=NULL;

   *pszSignalName = '\0';
   for (sel_ptr=botSel; ok && sel_ptr != NULL; sel_ptr=sel_ptr->prev) {
      struct AttrRec *attr_ptr=NULL;

      switch (sel_ptr->obj->type) {
      case OBJ_GROUP:
      case OBJ_SYM:
      case OBJ_ICON:
      case OBJ_PIN:
         if ((attr_ptr=FindAttrWithName(sel_ptr->obj, "type=", NULL)) != NULL) {
            if (strcmp(attr_ptr->attr_value.s, "port") == 0) {
               if ((attr_ptr=FindAttrWithName(sel_ptr->obj, "signal_name=",
                     NULL)) != NULL) {
                  TrySetCommonSignalName(attr_ptr, pszSignalName, szSignalName,
                        &signal_name_diff);
                  num_ports++;
               }
            } else if (strcmp(attr_ptr->attr_value.s, "tgBroadcastWire") == 0) {
               anchor = sel_ptr->obj;
               anchor_is_poly = FALSE;
               if ((attr_ptr=FindAttrWithName(sel_ptr->obj,
                     "broadcast_signal_name=", NULL)) != NULL) {
                  TrySetCommonSignalName(attr_ptr, pszSignalName, szSignalName,
                        &signal_name_diff);
                  already_has_broadcast_signal_name = TRUE;
               }
            } else {
               return FALSE;
            }
         } else {
            return FALSE;
         }
         break;
      case OBJ_POLY:
         if (anchor == NULL) {
            anchor = sel_ptr->obj;
            anchor_is_poly = TRUE;
         } else {
            return FALSE;
         }
         break;
      default:
         return FALSE;
         break;
      }
   }
   if (anchor == NULL || num_ports == 0) {
      return FALSE;
   }
   *pnNumPorts = num_ports;
   *ppAnchor = anchor;
   *pnAnchorIsPoly = anchor_is_poly;
   *pnSignalNameDiff = signal_name_diff;
   *pnAlreadyHasBroadcastSignalName = already_has_broadcast_signal_name;

   return TRUE;
}

void ConnectPortsToBroadcastWire()
{
   struct ObjRec *anchor=NULL;
   struct SelRec *sel_ptr=NULL;
   int num_ports=0, anchor_is_poly=FALSE, signal_name_diff=FALSE;
   int sel_ltx=selLtX, sel_lty=selLtY, sel_rbx=selRbX, sel_rby=selRbY;
   int already_has_broadcast_signal_name=FALSE;
   char existing_signal_name[MAXSTRING], new_signal_name[MAXSTRING];

   *existing_signal_name = '\0';
   if (!CheckPortsAndBroadcastObj(&anchor, &anchor_is_poly, &num_ports,
         existing_signal_name, sizeof(existing_signal_name),
         &signal_name_diff, &already_has_broadcast_signal_name)) {
      MsgBox(TgLoadString(STID_SEL_ONE_NONE_PORT_PLUS_BRDCST), TOOL_NAME,
            INFO_MB);
      return;
   }
   *new_signal_name = '\0';
   if (*existing_signal_name == '\0') {
      if (signal_name_diff) {
         /* conflicting signal names */
         sprintf(gszMsgBox, TgLoadString(STID_CONFLICT_SIG_NAME_ENT_NEW));
      } else {
         /* all ports have no signal names */
         sprintf(gszMsgBox, TgLoadString(STID_PLS_ENT_SIG_NAME));
      }
   } else {
      UtilStrCpyN(new_signal_name, sizeof(new_signal_name),
            existing_signal_name);
      sprintf(gszMsgBox, TgLoadString(STID_PLS_ENT_SIG_NAME));
   }
   if (!(*new_signal_name != '\0' && already_has_broadcast_signal_name)) {
      UtilTrimBlanks(new_signal_name);
      Dialog(gszMsgBox, TgLoadCachedString(CSTID_DLG_ACCEPT_CANCEL),
            new_signal_name);
      UtilTrimBlanks(new_signal_name);
      if (*new_signal_name == '\0') {
         return;
      }
   }
   StartCompositeCmd();

   HighLightReverse();
   if (anchor_is_poly) {
      struct SelRec *saved_top_sel=NULL, *saved_bot_sel=NULL;
      struct ObjRec *text_obj_ptr=NULL;
      struct BBRec obbox;
      int saved_num_obj_selected=0;
      char name[40];

      PrepareToReplaceAnObj(anchor);
      for (sel_ptr=botSel; sel_ptr != NULL; sel_ptr=sel_ptr->prev) {
         if (sel_ptr->obj == anchor) {
            UnlinkSel(sel_ptr, &topSel, &botSel);
            numObjSelected--;
            break;
         }
      }
      saved_top_sel = topSel;
      saved_bot_sel = botSel;
      saved_num_obj_selected = numObjSelected;
      topSel = botSel = (struct SelRec *)malloc(sizeof(struct SelRec));
      if (topSel == NULL) FailAllocMessage();
      topSel->obj = anchor;
      topSel->next = topSel->prev = NULL;
      numObjSelected = 1;
      GroupSingleObj(FALSE);
      topSel = saved_top_sel;
      botSel = saved_bot_sel;
      numObjSelected = saved_num_obj_selected;
      AddObjIntoSel(topObj, NULL, topSel, &topSel, &botSel);
      numObjSelected++;
      anchor = topObj;

      memcpy(&obbox, &anchor->obbox, sizeof(struct BBRec));
      AddAttrByNameAndValue(anchor, "type=", "tgBroadcastWire");
      text_obj_ptr = anchor->fattr->obj;
      MoveObj(text_obj_ptr, obbox.ltx-text_obj_ptr->obbox.ltx,
            obbox.rby-text_obj_ptr->obbox.lty);
      AdjObjBBox(anchor);

      sprintf(name, "tgAutoObj%d", anchor->id);
      memcpy(&obbox, &anchor->obbox, sizeof(struct BBRec));
      AddAttrByNameAndValue(anchor, "name=", name);
      text_obj_ptr = anchor->fattr->obj;
      MoveObj(text_obj_ptr, obbox.ltx-text_obj_ptr->obbox.ltx,
            obbox.rby-text_obj_ptr->obbox.lty);
      AdjObjBBox(anchor);

      RecordReplaceAnObj(anchor);
   }
   PrepareToRecord(CMD_REPLACE, topSel, botSel, numObjSelected);

   for (sel_ptr=botSel; sel_ptr != NULL; sel_ptr=sel_ptr->prev) {
      if (sel_ptr->obj != anchor) {
         UnlinkObj(sel_ptr->obj);
         AddObjToAnchor(anchor, sel_ptr->obj);
      }
   }
   RemoveAllSel();
   UnlinkObj(anchor);
   AddObj(NULL, topObj, anchor);
   topSel = botSel = (struct SelRec *)malloc(sizeof(struct SelRec));
   if (topSel == NULL) FailAllocMessage();
   topSel->obj = anchor;
   topSel->prev = NULL;
   botSel->next = NULL;
   AdjObjBBox(anchor);
   UpdSelBBox();
   RecordCmd(CMD_MANY_TO_ONE, NULL, topSel, botSel, 1);
   RedrawAreas(botObj, sel_ltx-GRID_ABS_SIZE(1), sel_lty-GRID_ABS_SIZE(1),
         sel_rbx+GRID_ABS_SIZE(1), sel_rby+GRID_ABS_SIZE(1),
         selLtX-GRID_ABS_SIZE(1), selLtY-GRID_ABS_SIZE(1),
         selRbX+GRID_ABS_SIZE(1), selRbY+GRID_ABS_SIZE(1));
   HighLightForward();
   SetFileModified(TRUE);
   justDupped = FALSE;

   DoNameBroadcastWire(new_signal_name, signal_name_diff);
   EndCompositeCmd();
}

#define DO_RENAME_SIGNAL_NAME 99
#define DO_CLEAR_SIGNAL_NAME 999

static
void DoNameOrClearAPort(which)
   int which;
{
   struct AttrRec *first_attr_ptr=NULL;
   struct ObjRec *first_attr_owner_obj=NULL;
   int ltx=0, lty=0, rbx=0, rby=0;
   char signal_name[MAXSTRING];
   ChangeConnectionViewInfo ccvi;

   memset(&ccvi, 0, sizeof(ChangeConnectionViewInfo));
   first_attr_ptr = FindAttrWithName(gstWiringInfo.first_port_obj,
         "signal_name=", &first_attr_owner_obj);
   while (first_attr_owner_obj != NULL &&
         first_attr_owner_obj->tmp_parent != NULL) {
      first_attr_owner_obj = first_attr_owner_obj->tmp_parent;
   }
   if (first_attr_ptr == NULL) {
#ifdef _TGIF_DBG /* debug, do not translate */
      TgAssert(FALSE,
            "first_attr_ptr is NULL in DoNameOrClearAPort()", NULL);
      return;
#endif /* _TGIF_DBG */
   }
   *signal_name = '\0';
   if (which == DO_CLEAR_SIGNAL_NAME && *first_attr_ptr->attr_value.s == '\0') {
      /* already cleared */
      return;
   }
   if (which == DO_RENAME_SIGNAL_NAME) {
      UtilStrCpyN(signal_name, sizeof(signal_name),
            first_attr_ptr->attr_value.s);
      sprintf(gszMsgBox, TgLoadString(STID_PLS_ENT_SIG_NAME_FOR_PORT),
            gstWiringInfo.first_port_name);
      if (Dialog(gszMsgBox, NULL, signal_name) == INVALID) return;
      UtilTrimBlanks(signal_name);
   }
   if (strcmp(signal_name, first_attr_ptr->attr_value.s) == 0) {
      return;
   }
   ltx = first_attr_owner_obj->bbox.ltx;
   lty = first_attr_owner_obj->bbox.lty;
   rbx = first_attr_owner_obj->bbox.rbx;
   rby = first_attr_owner_obj->bbox.rby;

   if (HasConnAndDisConnViewObjSubObjs(first_attr_ptr->owner, &ccvi)) {
      ccvi.do_change_view = TRUE;
      if (*signal_name != '\0') {
         /* the port will become connected */
         ccvi.change_to_connect_view = TRUE;
      } else {
         /* the port will become disconnected */
         ccvi.change_to_connect_view = FALSE;
      }
   }
   ccvi.port_obj = gstWiringInfo.first_port_obj;
   ccvi.signal_name = signal_name;
   SetReplaceAttrValueCallback(
         (ReplAttrValueCallback*)ChangeConnectionView, &ccvi);

   ReplaceAttrFirstValue(first_attr_owner_obj, first_attr_ptr,
         signal_name);
   SetReplaceAttrValueCallback(NULL, NULL);
   RecursivelyAdjObjBBox(first_attr_ptr->owner, first_attr_ptr->owner,
         first_attr_owner_obj);

   SetFileModified(TRUE);
   justDupped = FALSE;
   RedrawAreas(botObj,
         ltx-QUARTER_INCH-GRID_ABS_SIZE(1), lty-QUARTER_INCH-GRID_ABS_SIZE(1),
         rbx+QUARTER_INCH+GRID_ABS_SIZE(1), rby+QUARTER_INCH+GRID_ABS_SIZE(1),
         first_attr_owner_obj->bbox.ltx-QUARTER_INCH-GRID_ABS_SIZE(1),
         first_attr_owner_obj->bbox.lty-QUARTER_INCH-GRID_ABS_SIZE(1),
         first_attr_owner_obj->bbox.rbx+QUARTER_INCH+GRID_ABS_SIZE(1),
         first_attr_owner_obj->bbox.rby+QUARTER_INCH+GRID_ABS_SIZE(1));
}

static
void DoRenameOrClearSignalNameForAPort(which)
   int which;
{
   int naming=TRUE;

   ShowCurChoiceMouseStatus(DRAWPOLY, 0, FALSE);
   SetHandCursor(drawWindow);
   while (naming) {
      XEvent input;

      XNextEvent(mainDisplay, &input);
      if (input.type == Expose || input.type == VisibilityNotify) {
         ExposeEventHandler(&input, TRUE);
      } else if (input.type == ConfigureNotify) {
         Reconfigure(FALSE);
      } else if (input.xany.window == drawWindow && input.type == ButtonPress) {
         if (input.xbutton.button == Button1) {
            if (drawPolyHighlightedNode != NULL) {
               HandlePressForPortInDrawWindow(FALSE);
               DoNameOrClearAPort(which);
               drawPolyHighlightedNode = NULL;
               SetWiringNodeInfo(NULL, NULL, NULL, TRUE);
               SetHandCursor(drawWindow);
               ShowCurChoiceMouseStatus(DRAWPOLY, 0, FALSE);
            }
         } else {
            XUngrabPointer(mainDisplay, CurrentTime);
            naming = FALSE;
            HandlePressForPortInDrawWindow(TRUE);
         }
      } else if (input.xany.window == drawWindow &&
            input.type == MotionNotify) {
         XEvent ev;

         HandleMotionForPortInDrawWindow(input.xmotion.x, input.xmotion.y);
         while (XCheckMaskEvent(mainDisplay, PointerMotionMask, &ev)) ;
      } else if (input.type == KeyPress) {
         if (KeyPressEventIsEscape(&input.xkey)) {
            XUngrabPointer(mainDisplay, CurrentTime);
            naming = FALSE;
            HandlePressForPortInDrawWindow(TRUE);
         }
      }
   }
   SetDefaultCursor(drawWindow);
}

static
void RenameOrClearSignalNameForAPort(which)
   int which;
{
   XGCValues values;

   MakeQuiescent();

   ResetWiringNodeInfo();
   connectingPortsByWire = TRUE;
   connectingPortsFromInternalCommand = FALSE;
   gstWiringInfo.num_ports_to_connect = which;
   drawPolyHighlightedNode = NULL;

   values.line_width = 3;
   XChangeGC(mainDisplay, revGrayGC, GCLineWidth, &values);

   SaveStatusStrings();
   DoRenameOrClearSignalNameForAPort(which);
   RestoreStatusStrings();

   values.line_width = 1;
   XChangeGC(mainDisplay, revGrayGC, GCLineWidth, &values);

   gstWiringInfo.num_ports_to_connect = 0;
   connectingPortsByWire = FALSE;
   connectingPortsFromInternalCommand = TRUE;
   ResetWiringNodeInfo();
   drawPolyHighlightedNode = NULL;
}

void RenameSignalNameForAPort()
{
   RenameOrClearSignalNameForAPort(DO_RENAME_SIGNAL_NAME);
}

void ClearSignalNameForAPort()
{
   RenameOrClearSignalNameForAPort(DO_CLEAR_SIGNAL_NAME);
}

static
int CheckPortsAndObj(ppAnchor, pnNumPorts)
   struct ObjRec **ppAnchor;
   int *pnNumPorts;
{
   int num_ports=0, ok=TRUE;
   struct ObjRec *anchor=NULL;
   struct SelRec *sel_ptr=NULL;

   for (sel_ptr=botSel; ok && sel_ptr != NULL; sel_ptr=sel_ptr->prev) {
      struct AttrRec *attr_ptr=NULL;

      switch (sel_ptr->obj->type) {
      case OBJ_GROUP:
      case OBJ_SYM:
      case OBJ_ICON:
      case OBJ_PIN:
         if ((attr_ptr=FindAttrWithName(sel_ptr->obj, "type=", NULL)) != NULL) {
            if (strcmp(attr_ptr->attr_value.s, "port") == 0 &&
                  FindAttrWithName(sel_ptr->obj, "signal_name=",
                  NULL) != NULL) {
               num_ports++;
            } else if (anchor == NULL) {
               anchor = sel_ptr->obj;
            } else {
               return FALSE;
            }
         } else if (anchor == NULL) {
            anchor = sel_ptr->obj;
         } else {
            return FALSE;
         }
         break;
      default:
         return FALSE;
         break;
      }
   }
   if (anchor == NULL || num_ports == 0) {
      return FALSE;
   }
   *pnNumPorts = num_ports;
   *ppAnchor = anchor;

   return TRUE;
}

void MergePortsWithAnObject()
{
   int num_ports=0;
   int sel_ltx=selLtX, sel_lty=selLtY, sel_rbx=selRbX, sel_rby=selRbY;
   struct ObjRec *anchor=NULL;
   struct SelRec *sel_ptr=NULL;

   if (!CheckPortsAndObj(&anchor, &num_ports)) {
      MsgBox(TgLoadString(STID_SEL_ONE_NONE_PORT_PLUS_COMP), TOOL_NAME,
            INFO_MB);
      return;
   }
   HighLightReverse();
   PrepareToRecord(CMD_REPLACE, topSel, botSel, numObjSelected);

   for (sel_ptr=botSel; sel_ptr != NULL; sel_ptr=sel_ptr->prev) {
      if (sel_ptr->obj != anchor) {
         UnlinkObj(sel_ptr->obj);
         AddObjToAnchor(anchor, sel_ptr->obj);
      }
   }
   RemoveAllSel();
   UnlinkObj(anchor);
   AddObj(NULL, topObj, anchor);
   topSel = botSel = (struct SelRec *)malloc(sizeof(struct SelRec));
   if (topSel == NULL) FailAllocMessage();
   topSel->obj = anchor;
   topSel->prev = NULL;
   botSel->next = NULL;
   AdjObjBBox(anchor);
   UpdSelBBox();
   RecordCmd(CMD_MANY_TO_ONE, NULL, topSel, botSel, 1);
   RedrawAreas(botObj, sel_ltx-GRID_ABS_SIZE(1), sel_lty-GRID_ABS_SIZE(1),
         sel_rbx+GRID_ABS_SIZE(1), sel_rby+GRID_ABS_SIZE(1),
         selLtX-GRID_ABS_SIZE(1), selLtY-GRID_ABS_SIZE(1),
         selRbX+GRID_ABS_SIZE(1), selRbY+GRID_ABS_SIZE(1));
   HighLightForward();
   SetFileModified(TRUE);
   justDupped = FALSE;
}

static
void RenumberObjId(obj_ptr)
   struct ObjRec *obj_ptr;
{
   struct AttrRec *attr_ptr=NULL;
   struct ObjRec *sub_obj=NULL;

   obj_ptr->id = objId++;

   switch (obj_ptr->type) {
   case OBJ_GROUP:
   case OBJ_ICON:
   case OBJ_SYM:
   case OBJ_PIN:
      for (sub_obj=obj_ptr->detail.r->last; sub_obj != NULL;
            sub_obj=sub_obj->prev) {
         RenumberObjId(sub_obj);
      }
      break;
   }
   for (attr_ptr=obj_ptr->lattr; attr_ptr != NULL; attr_ptr=attr_ptr->prev) {
      RenumberObjId(attr_ptr->obj);
   }
}

void RenumberObjectIds()
{
   struct PageRec *page_ptr=NULL;

   if (gstWBInfo.do_whiteboard) {
      MsgBox(TgLoadString(STID_RENUMBER_OBJ_IDS_IN_WB), TOOL_NAME, INFO_MB);
      return;
   }
   if (!OkToFlushUndoBuffer(TgLoadString(STID_RENUMBER_OBJ_IDS_CAUSE_FLUSH))) {
      return;
   }
   MakeQuiescent();
   CleanUpCmds();

   objId = 0;
   for (page_ptr=firstPage; page_ptr != NULL; page_ptr=page_ptr->next) {
      struct ObjRec *obj_ptr=NULL;

      for (obj_ptr=page_ptr->bot; obj_ptr != NULL; obj_ptr=obj_ptr->prev) {
         RenumberObjId(obj_ptr);
      }
   }
   MsgBox(TgLoadString(STID_RENUMBER_OBJ_IDS_DONE), TOOL_NAME, INFO_MB);
   SetFileModified(TRUE);
}

void MakeSymbolic()
{
   int ltx=selLtX, lty=selLtY, rbx=selRbX, rby=selRbY;
   struct ObjRec *obj_ptr=NULL;

   if (curChoice == VERTEXMODE) {
      MsgBox(TgLoadString(STID_CANT_MAKESYMBOLIC_VERTEX_MODE), TOOL_NAME,
            INFO_MB);
      return;
   } else if (topSel == NULL || topSel != botSel) {
      MsgBox(TgLoadString(STID_SEL_ONLY_ONE_FOR_MAKESYMBOLIC), TOOL_NAME,
            INFO_MB);
      return;
   }
   obj_ptr = topSel->obj;
   if (obj_ptr->type == OBJ_SYM) return;
   if (obj_ptr->type == OBJ_PIN) {
      MsgBox(TgLoadString(STID_PIN_CANT_MADE_INTO_SYMBOL), TOOL_NAME, INFO_MB);
      return;
   }
   HighLightReverse();
   PrepareToReplaceAnObj(obj_ptr);
/*
 * Need to check for pins.
 */
   if (obj_ptr->type != OBJ_GROUP && obj_ptr->type != OBJ_ICON) {
      GroupSingleObj(FALSE);
      obj_ptr = topSel->obj;
      if (obj_ptr->fattr != NULL) {
         Msg(TgLoadCachedString(CSTID_ATTR_PROMOTED_TO_NEW_SYMBOL));
      }
   }
   obj_ptr->type = OBJ_SYM;
   AdjObjBBox(obj_ptr);
   UpdSelBBox();
   RecordReplaceAnObj(obj_ptr);

   UpdSelBBox();
   RedrawAnArea(botObj,
         ltx-QUARTER_INCH-GRID_ABS_SIZE(1), lty-QUARTER_INCH-GRID_ABS_SIZE(1),
         rbx+QUARTER_INCH+GRID_ABS_SIZE(1), rby+QUARTER_INCH+GRID_ABS_SIZE(1));
   HighLightForward();
   SetFileModified(TRUE);
   justDupped = FALSE;

   Msg(TgLoadString(STID_SEL_OBJ_IS_NOW_SYMBOLIC));
}

void UnMakeSymbolic()
{
   register struct ObjRec *obj_ptr;
   register int ltx=0, lty=0, rbx=0, rby=0;
   struct SelRec *sel_ptr=topSel;
   int modified=FALSE;

   StartCompositeCmd();
   for ( ; sel_ptr != NULL; sel_ptr=sel_ptr->next) {
      obj_ptr = sel_ptr->obj;
      if (obj_ptr->type == OBJ_SYM) {
         PrepareToReplaceAnObj(obj_ptr);
         obj_ptr->type = OBJ_GROUP;
         AdjObjBBox(obj_ptr);
         RecordReplaceAnObj(obj_ptr);
         if (modified) {
            if (obj_ptr->bbox.ltx < ltx) ltx = obj_ptr->bbox.ltx;
            if (obj_ptr->bbox.lty < lty) lty = obj_ptr->bbox.lty;
            if (obj_ptr->bbox.rbx > rbx) rbx = obj_ptr->bbox.rbx;
            if (obj_ptr->bbox.rby > rby) rby = obj_ptr->bbox.rby;
         } else {
            ltx = obj_ptr->bbox.ltx; lty = obj_ptr->bbox.lty;
            rbx = obj_ptr->bbox.rbx; rby = obj_ptr->bbox.rby;
            modified = TRUE;
         }
      }
   }
   EndCompositeCmd();
   if (modified) {
      HighLightReverse();
      UpdSelBBox();
      RedrawAnArea(botObj, ltx-QUARTER_INCH-GRID_ABS_SIZE(1),
            lty-QUARTER_INCH-GRID_ABS_SIZE(1),rbx+QUARTER_INCH+GRID_ABS_SIZE(1),
            rby+QUARTER_INCH+GRID_ABS_SIZE(1));
      HighLightForward();
      SetFileModified(TRUE);
      justDupped = FALSE;
   }
}

int MakeIconic(sym_path, record_cmd)
   char *sym_path;
   int record_cmd;
{
   char icon_name[MAXPATHLENGTH], file_name[MAXPATHLENGTH];
   char icon_full_name[MAXPATHLENGTH], *rest=NULL, *psz=NULL;
   char sym_ext_str[MAXSTRING];
   FILE *fp=NULL;
   int len, short_name, sym_ext_len, ltx, lty, rbx, rby, no_name=FALSE;

   if (topSel == NULL || topSel != botSel) {
      MsgBox(TgLoadString(STID_SEL_ONLY_ONE_FOR_MAKEICONIC), TOOL_NAME,
            INFO_MB);
      return FALSE;
   }
   if (sym_path == NULL) {
      *icon_name = '\0';
      Dialog(TgLoadString(STID_ENTER_NAME_FOR_THE_ICON), NULL, icon_name);
      len = strlen(icon_name);
      if (*icon_name == '\0') {
         Msg(TgLoadString(STID_NAME_NOT_SPEC_ICON_NOT_CREATE));
         return FALSE;
      }
   } else {
      strncpy(icon_name, sym_path, sizeof(icon_name)-1);
      icon_name[sizeof(icon_name)-1] = '\0';
      len = strlen(icon_name);
   }
   sprintf(sym_ext_str, ".%s", SYM_FILE_EXT);
   sym_ext_len = strlen(sym_ext_str);

   if (FileNameHasExtension(icon_name, OBJ_FILE_TYPE, NULL, NULL)) {
      MsgBox(TgLoadString(STID_CANT_SAVE_OBJ_ICON_NOT_CREATE), TOOL_NAME,
            INFO_MB);
      return FALSE;
   } else if (FileNameHasExtension(icon_name, SYM_FILE_TYPE, NULL, NULL)) {
      strcpy(icon_full_name, icon_name);
      if ((psz=UtilStrRChr(icon_name, '.')) != NULL) {
         *psz = '\0';
      } else {
         TgAssert(FALSE, TgLoadString(STID_CANT_FIND_DOT_IN_ICON_NAME_MI),
               NULL);
         return FALSE;
      }
   } else if (FileNameHasExtension(icon_name, PIN_FILE_TYPE, NULL, NULL)) {
      MsgBox(TgLoadString(STID_CANT_SAVE_PIN_ICON_NOT_CREATE), TOOL_NAME,
            INFO_MB);
      return FALSE;
   } else {
      strcpy(icon_full_name, icon_name);
      strcat(icon_full_name, sym_ext_str);
   }
   if (no_name) {
      MsgBox(TgLoadString(STID_NO_FILE_NAME_FILE_NOT_SAVED), TOOL_NAME,
            INFO_MB);
      return FALSE;
   }
   if (sym_path == NULL) {
      sprintf(file_name, "%s%c%s", curDir, DIR_SEP, icon_full_name);
   } else {
      strcpy(file_name, icon_full_name);
   }
   if (!OkayToCreateFile(file_name)) return FALSE;
   if ((short_name=IsPrefix(bootDir, file_name, &rest))) ++rest;

   if ((fp=fopen(file_name, "w")) == NULL) {
      sprintf(gszMsgBox, TgLoadString(STID_CANT_OPEN_FOR_WRITE_ICON_CRET),
            (short_name ? rest : file_name));
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return FALSE;
   }
   if (sym_path == NULL) {
      if (!DirInSymPath(curDir)) UpdateSymInfo();
   }
   sprintf(gszMsgBox, TgLoadCachedString(CSTID_CREATING_FILE_DOTS),
         (short_name ? rest : file_name));
   Msg(gszMsgBox);
   SetStringStatus(gszMsgBox);

   ltx = selLtX; lty = selLtY; rbx = selRbX; rby = selRbY;

   if (record_cmd) {
      PrepareToRecord(CMD_REPLACE, topSel, botSel, numObjSelected);
   }
   if (topSel->obj->type == OBJ_GROUP || topSel->obj->type == OBJ_SYM ||
         topSel->obj->type == OBJ_ICON || topSel->obj->type == OBJ_PIN) {
      JustMoveSelToTop();
   } else {
      GroupSingleObj(FALSE);
      if (topSel->obj->fattr != NULL) {
         Msg(TgLoadCachedString(CSTID_ATTR_PROMOTED_TO_NEW_ICON));
      }
   }

   topSel->obj->type = OBJ_SYM;
   strcpy(topSel->obj->detail.r->s, icon_name);
   topSel->obj->detail.r->rotate = ROTATE0;
   topSel->obj->detail.r->flip = NO_FLIP;
   topSel->obj->detail.r->deck_index = (-1);
/*
 * Need to check for pins.
 * Need to handle the case where connection objects need to be dupped!
 */
   topSel->obj->detail.r->pin_connected = 0;
   topSel->obj->detail.r->first_conn = topSel->obj->detail.r->last_conn =
         NULL;

   writeFileFailed = FALSE;
   Save(fp, topSel->obj, 0, 1);

   if (writeFileFailed) {
      writeFileFailed = FALSE;
      FailToWriteFileMessage(file_name);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      unlink(file_name);
   } else {
      sprintf(gszMsgBox, TgLoadCachedString(CSTID_NAMED_FILE_CREATED),
            (short_name ? rest : file_name));
      Msg(gszMsgBox);
   }
   fclose(fp);

   if (record_cmd) {
      HighLightReverse();
   }
   topSel->obj->type = OBJ_ICON;
   topSel->obj->id = objId++;
   Msg(TgLoadString(STID_SEL_OBJ_IS_NOW_ICONIC));
   AdjObjBBox(topSel->obj);
   UpdSelBBox();
   if (record_cmd) {
      RecordCmd(CMD_REPLACE, NULL, topSel, botSel, numObjSelected);
      ltx = min(ltx,selLtX); lty = min(lty,selLtY);
      rbx = max(rbx,selRbX); rby = max(rby,selRbY);
      RedrawAnArea(botObj, ltx-GRID_ABS_SIZE(1), lty-GRID_ABS_SIZE(1),
            rbx+GRID_ABS_SIZE(1), rby+GRID_ABS_SIZE(1));
      HighLightForward();
      SetFileModified(TRUE);
      justDupped = FALSE;
   }
   return TRUE;
}

void UnMakeIconic()
{
   register struct ObjRec *obj_ptr;
   struct SelRec *sel_ptr;
   struct AttrRec *attr_ptr;
   int modified=FALSE;

   HighLightReverse();
   StartCompositeCmd();
   for (sel_ptr=topSel; sel_ptr != NULL; sel_ptr=sel_ptr->next) {
      obj_ptr = sel_ptr->obj;
/*
 * Need to check for pins.
 */
      if (obj_ptr->type == OBJ_ICON) {
         modified = TRUE;
         PrepareToReplaceAnObj(obj_ptr);
         obj_ptr->type = OBJ_GROUP;
         attr_ptr = obj_ptr->fattr;
         for ( ; attr_ptr != NULL; attr_ptr=attr_ptr->next) {
            attr_ptr->inherited = FALSE;
         }
         AdjObjBBox(obj_ptr);
         RecordReplaceAnObj(obj_ptr);
      }
   }
   EndCompositeCmd();
   if (modified) {
      Msg(TgLoadString(STID_SEL_OBJS_ARE_NOW_GROUP));
      UpdSelBBox();
      RedrawAnArea(botObj, selLtX-GRID_ABS_SIZE(1), selLtY-GRID_ABS_SIZE(1),
            selRbX+GRID_ABS_SIZE(1), selRbY+GRID_ABS_SIZE(1));
      SetFileModified(TRUE);
      justDupped = FALSE;
   }
   HighLightForward();
}

typedef struct ImportAttrLineRec {
   char *s;
   struct ImportAttrLineRec *next, *prev;
} *ImportAttrLineRecPtr;

static struct ImportAttrLineRec *firstLine=NULL, *lastLine=NULL;
static int numLines=0, nextX=0, nextY=0;
static CVList gLineList;

static
void AddLine(buf)
   char *buf;
{
   struct ImportAttrLineRec *line_ptr;

   line_ptr = (struct ImportAttrLineRec *)malloc(
         sizeof(struct ImportAttrLineRec));
   if (line_ptr == NULL) FailAllocMessage();
   line_ptr->s = buf;
   line_ptr->next = NULL;
   line_ptr->prev = lastLine;
   if (lastLine == NULL) {
      firstLine = line_ptr;
   } else {
      lastLine->next = line_ptr;
   }
   lastLine = line_ptr;
   numLines++;
}

static
void FreeImportLines()
{
   struct ImportAttrLineRec *next_line=NULL;

   for ( ; firstLine != NULL; firstLine=next_line) {
      next_line = firstLine->next;
      if (firstLine->s != NULL) free(firstLine->s);
      free(firstLine);
   }
   lastLine = NULL;
   numLines = 0;
}

static
void ImportAnAttr(obj_ptr, attr_line_num, fname)
   struct ObjRec *obj_ptr;
   int attr_line_num;
   char *fname;
{
   int ok=TRUE;
   char *c_ptr;

   if (firstLine == NULL) return;
   if ((c_ptr=strchr(firstLine->s, '=')) == NULL) {
      sprintf(gszMsgBox, TgLoadString(STID_GIVEN_LINE_SKIP_NOT_AN_ATTR),
            attr_line_num, fname);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      ok = FALSE;
   } else {
      *c_ptr = '\0';
      if (strchr(firstLine->s, '!') != NULL ||
            strchr(firstLine->s, '.') != NULL) {
         sprintf(gszMsgBox, TgLoadString(STID_GIVEN_LINE_SKIP_ILLEGAL_CHAR),
               attr_line_num, fname);
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
         ok = FALSE;
      }
      *c_ptr = '=';
   }
   if (ok) {
      char saved_ch='\0', *attr_name=NULL;
      struct AttrRec *attr_ptr=NULL;
      struct TextRec *text_ptr=NULL;
      MiniLineInfo *pMiniLine=NULL, *pFirstMiniLine=NULL, *pLastMiniLine=NULL;
      struct ImportAttrLineRec *line_ptr;
      int move_next_y=FALSE, underline_on=FALSE, single_font_text=FALSE;
      int sz_unit=INVALID, double_byte=INVALID, font=INVALID, style=INVALID;
      int color_index=INVALID, saved_color_index=INVALID, overline_on=FALSE;

      c_ptr++;
      saved_ch = (*c_ptr);
      *c_ptr = '\0';
      attr_name = UtilStrDup(firstLine->s);
      if (attr_name == NULL) FailAllocMessage();
      *c_ptr = saved_ch;

      if ((attr_ptr=FindAttrWithName(obj_ptr, attr_name, NULL)) == NULL) {
         attr_ptr = AddAttrByNameAndValue(obj_ptr, attr_name, c_ptr);
         attr_ptr->shown = TRUE;
         attr_ptr->obj->color = colorIndex;
         if (mainDisplay != NULL) {
            UtilStrCpyN(attr_ptr->obj->color_str,
                  sizeof(attr_ptr->obj->color_str), colorMenuItems[colorIndex]);
         }
         MoveObj(attr_ptr->obj, nextX-attr_ptr->obj->x, nextY-attr_ptr->obj->y);
         text_ptr = attr_ptr->obj->detail.t;
         pFirstMiniLine = pLastMiniLine = text_ptr->minilines.first;
         move_next_y = TRUE;
      } else {
         MiniLineInfo *pNextMiniLine=NULL;

         single_font_text = SingleFontText(attr_ptr->obj->detail.t, &sz_unit,
               &double_byte, &font, &style, &underline_on, &overline_on) &&
               SingleColorText(attr_ptr->obj->detail.t, &color_index);
         if (single_font_text) {
            saved_color_index = colorIndex;

            PushCurFont();
            colorIndex = color_index;
            curFont = font;
            curSzUnit = sz_unit;
            curStyle = style;
            SetCanvasFont();
         }
         DynStrSet(&attr_ptr->attr_value, c_ptr);
         text_ptr = attr_ptr->obj->detail.t;
         text_ptr->cached_zoom = 0;
         if (text_ptr->cached_bitmap != None) {
            XFreePixmap(mainDisplay, text_ptr->cached_bitmap);
         }
         text_ptr->cached_bitmap = None;
         for (pMiniLine=text_ptr->minilines.first->next; pMiniLine != NULL;
               pMiniLine=pNextMiniLine) {
            pNextMiniLine = pMiniLine->next;
            FreeMiniLine(pMiniLine);
         }
         pFirstMiniLine = pLastMiniLine = text_ptr->minilines.last =
               text_ptr->minilines.first;
         text_ptr->minilines.first->next = text_ptr->minilines.first->prev =
               NULL;
         text_ptr->lines = 1;
         UpdAttr(attr_ptr);
      }
      free(attr_name);
      for (line_ptr=firstLine->next; line_ptr!=NULL; line_ptr=line_ptr->next) {
         CreateMiniLineFromString(line_ptr->s, &pFirstMiniLine, &pLastMiniLine);
         text_ptr->lines++;
      }
      if (single_font_text) {
         colorIndex = saved_color_index;
         PopCurFont();
      }
      text_ptr->minilines.first = pFirstMiniLine;
      text_ptr->minilines.last = pLastMiniLine;
      text_ptr->baseline_y = attr_ptr->obj->y+pFirstMiniLine->asc;
      RecalcTextMetrics(text_ptr, attr_ptr->obj->x, text_ptr->baseline_y);
      UpdTextBBox(attr_ptr->obj);
      if (move_next_y) {
         nextY += attr_ptr->obj->obbox.rby-attr_ptr->obj->obbox.lty;
      }
   }
   FreeImportLines();
}

void ImportAttrs()
{
   char fname[MAXPATHLENGTH+1], *rest, *buf;
   XEvent ev;
   int short_name, ltx, lty, rbx, rby, line_num=0, attr_line_num=1;
   int empty_line=TRUE;
   FILE *fp;
   struct ObjRec *obj_ptr;

   if (topSel == NULL || topSel != botSel) {
      MsgBox(TgLoadString(STID_SEL_ONE_OBJ_FOR_IMPORTATTRS), TOOL_NAME,
            INFO_MB);
      return;
   }
   if (SelectFileNameToImport(TgLoadString(STID_SEL_TEXT_FILE_OF_ATTR_TO_IMP),
         TEXT_FILE_EXT, fname) == INVALID) {
      return;
   } else if (FileIsRemote(fname)) {
      MsgBox(TgLoadString(STID_CANT_IMPORT_REMOTE_TEXT_FILE), TOOL_NAME,
            INFO_MB);
      return;
   }
   XSync(mainDisplay, False);
   if (XCheckMaskEvent(mainDisplay, ExposureMask, &ev)) {
      ExposeEventHandler(&ev, TRUE);
   }
   if ((short_name=IsPrefix(bootDir, fname, &rest))) ++rest;
   if ((fp=fopen(fname, "r")) == NULL) {
      sprintf(gszMsgBox, TgLoadString(STID_CANNOT_OPEN_FILE_FOR_READING),
            (short_name ? rest : fname));
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return;
   }
   obj_ptr = topSel->obj;
   ltx = obj_ptr->bbox.ltx; lty = obj_ptr->bbox.lty;
   rbx = obj_ptr->bbox.rbx; rby = obj_ptr->bbox.rby;

   SetWatchCursor(drawWindow);
   SetWatchCursor(mainWindow);

   HighLightReverse();
   PrepareToReplaceAnObj(obj_ptr);

   nextX = obj_ptr->obbox.ltx;
   nextY = obj_ptr->obbox.rby;
   while ((buf=UtilGetALine(fp)) != NULL) {
      line_num++;
      if (empty_line) {
         empty_line = FALSE;
         attr_line_num = line_num;
      }
      if (*buf == '\0') {
         ImportAnAttr(obj_ptr, attr_line_num, fname);
         empty_line = TRUE;
      } else {
         AddLine(buf);
      }
   }
   ImportAnAttr(obj_ptr, attr_line_num, fname);
   fclose(fp);

   AdjObjBBox(obj_ptr);

   RecordReplaceAnObj(obj_ptr);

   SetDefaultCursor(mainWindow);
   ShowCursor();

   UpdSelBBox();
   RedrawAreas(botObj, ltx-GRID_ABS_SIZE(1), lty-GRID_ABS_SIZE(1),
         rbx+GRID_ABS_SIZE(1), rby+GRID_ABS_SIZE(1),
         obj_ptr->bbox.ltx-GRID_ABS_SIZE(1),
         obj_ptr->bbox.lty-GRID_ABS_SIZE(1),
         obj_ptr->bbox.rbx+GRID_ABS_SIZE(1),
         obj_ptr->bbox.rby+GRID_ABS_SIZE(1));
   HighLightForward();
   SetFileModified(TRUE);
   justDupped = FALSE;
}

#define INA_SKIP      0 /* skip this attribute */
#define INA_OK        1 /* import this attribute */
#define INA_MALFORMED 2 /* illegal attribute */
#define INA_NEW       3 /* import this new attribute */

static
int OkayToImportNamedAttr(num_restricted, ppsz_restricted)
   int num_restricted;
   char **ppsz_restricted;
{
   int i=0, found=FALSE, rc=INA_OK;
   char *psz=NULL, saved_ch='\0';

   if (firstLine == NULL || firstLine->s == NULL) return INA_SKIP;

   psz = strchr(firstLine->s, '=');
   if (psz == NULL) {
      sprintf(gszMsgBox, TgLoadString(STID_BAD_LINE_MAY_HAVE_DEL_EQUAL),
            firstLine->s);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return INA_MALFORMED;
   }
   saved_ch = *(++psz);
   *psz = '\0';
   for (i=0; i < num_restricted; i++) {
      char *attr_name=ppsz_restricted[i];

      if (strcmp(attr_name, firstLine->s) == 0) {
         found = TRUE;
         break;
      }
   }
   *psz = saved_ch;

   if (!found) {
      rc = INA_SKIP;
      *psz = '\0';
      sprintf(gszMsgBox, TgLoadString(STID_ATTR_NAME_CHANGED_IMPORT_Q),
            firstLine->s);
      switch (MsgBox(gszMsgBox, TOOL_NAME, YNC_MB)) {
      case MB_ID_YES: rc = INA_NEW; break;
      case MB_ID_NO: break;
      case MB_ID_CANCEL: break;
      }
      *psz = saved_ch;
   }
   return rc;
}

typedef struct tagLineInfo {
   int status;
   int attr_line_num;
   int num_lines;
   struct ImportAttrLineRec *first, *last;
} LineInfo;

static
void CreateLineInfo(status, attr_line_num)
    int status, attr_line_num;
{
   LineInfo *pli=(LineInfo*)malloc(sizeof(LineInfo));

   if (pli == NULL) FailAllocMessage();
   memset(pli, 0, sizeof(LineInfo));

   pli->status = status;
   pli->attr_line_num = attr_line_num;
   pli->num_lines = numLines;
   pli->first = firstLine;
   pli->last = lastLine;

   firstLine = lastLine = NULL;
   numLines = 0;

   ListAppend(&gLineList, pli);
}

static
void DoImportNamedAttrs(fp, num_restricted, ppsz_restricted, pn_ok_count,
      pn_skip_count, pn_bad_count)
   FILE *fp;
   int num_restricted, *pn_ok_count, *pn_skip_count, *pn_bad_count;
   char **ppsz_restricted;
{
   char *buf=NULL;
   int bad_count=0, skip_count=0, ok_count=0, status=0;
   int line_num=0, attr_line_num=1, just_read_separator=TRUE;

   CVListInit(&gLineList);

   while ((buf=UtilGetALine(fp)) != NULL) {
      line_num++;
      if (just_read_separator) {
         just_read_separator = FALSE;
         attr_line_num = line_num;
      }
      if (strstr(buf, gszAttrSeparator) != NULL) {
         status = OkayToImportNamedAttr(num_restricted, ppsz_restricted);

         switch (status) {
         case INA_SKIP: skip_count++; FreeImportLines(); break;
         case INA_OK: ok_count++; CreateLineInfo(status, attr_line_num); break;
         case INA_MALFORMED: bad_count++; FreeImportLines(); break;
         case INA_NEW: ok_count++; CreateLineInfo(status, attr_line_num); break;
         }
         just_read_separator = TRUE;
      } else {
         AddLine(buf);
      }
   }
   if (!just_read_separator) {
      status = OkayToImportNamedAttr(num_restricted, ppsz_restricted);

      switch (status) {
      case INA_SKIP: skip_count++; FreeImportLines(); break;
      case INA_OK: ok_count++; CreateLineInfo(status, attr_line_num); break;
      case INA_MALFORMED: bad_count++; FreeImportLines(); break;
      case INA_NEW: ok_count++; CreateLineInfo(status, attr_line_num); break;
      }
   }
   if (pn_ok_count != NULL) *pn_ok_count = ok_count;
   if (pn_skip_count != NULL) *pn_skip_count = skip_count;
   if (pn_bad_count != NULL) *pn_bad_count = bad_count;
}

int ImportNamedAttrs(fp, obj_ptr, num_restricted, ppsz_restricted, fname)
   FILE *fp;
   struct ObjRec *obj_ptr;
   int num_restricted;
   char **ppsz_restricted, *fname;
{
   int ltx=0, lty=0, rbx=0, rby=0, bad_count=0, skip_count=0, ok_count=0;
   CVListElem *elem=NULL;

   ltx = obj_ptr->bbox.ltx; lty = obj_ptr->bbox.lty;
   rbx = obj_ptr->bbox.rbx; rby = obj_ptr->bbox.rby;

   DoImportNamedAttrs(fp, num_restricted, ppsz_restricted, &ok_count,
         &skip_count, &bad_count);

   if (bad_count > 0) {
      sprintf(gszMsgBox, TgLoadString(STID_CANT_IMPORT_ATTR_GROUP_RETRY),
            TOOL_NAME);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return FALSE;
   }
   if (ListEmpty(&gLineList)) return TRUE;

   SetWatchCursor(drawWindow);
   SetWatchCursor(mainWindow);

   HighLightReverse();
   PrepareToReplaceAnObj(obj_ptr);

   nextX = obj_ptr->obbox.ltx;
   nextY = obj_ptr->obbox.rby;

   for (elem=ListFirst(&gLineList); elem != NULL;
         elem=ListNext(&gLineList, elem)) {
      LineInfo *pli=(LineInfo*)(elem->obj);

      if (pli != NULL) {
         int attr_line_num=pli->attr_line_num;

         firstLine = pli->first;
         lastLine = pli->last;
         numLines = pli->num_lines;

         ImportAnAttr(obj_ptr, attr_line_num, fname);
      }
      elem->obj = NULL;
   }
   ListUnlinkAll(&gLineList);

   AdjObjBBox(obj_ptr);

   RecordReplaceAnObj(obj_ptr);

   SetDefaultCursor(mainWindow);
   ShowCursor();

   UpdSelBBox();
   RedrawAreas(botObj, ltx-GRID_ABS_SIZE(1), lty-GRID_ABS_SIZE(1),
         rbx+GRID_ABS_SIZE(1), rby+GRID_ABS_SIZE(1),
         obj_ptr->bbox.ltx-GRID_ABS_SIZE(1),
         obj_ptr->bbox.lty-GRID_ABS_SIZE(1),
         obj_ptr->bbox.rbx+GRID_ABS_SIZE(1),
         obj_ptr->bbox.rby+GRID_ABS_SIZE(1));
   HighLightForward();
   SetFileModified(TRUE);
   justDupped = FALSE;

   return TRUE;
}

void ExportAttrs()
{
   char fname[MAXPATHLENGTH+1], full_fname[MAXPATHLENGTH+1];
   char *c_ptr, *dot_ptr, *rest;
   int short_name;
   FILE *fp;
   struct AttrRec *attr_ptr;

   if (topSel == NULL || topSel != botSel) {
      MsgBox(TgLoadString(STID_SEL_ONE_OBJ_FOR_EXPORTATTRS), TOOL_NAME,
            INFO_MB);
      return;
   } else if (topSel->obj->lattr == NULL) {
      MsgBox(TgLoadString(STID_SEL_OBJ_HAS_NO_ATTR_TO_EXPORT), TOOL_NAME,
            INFO_MB);
      return;
   }
   sprintf(gszMsgBox, TgLoadString(STID_WORKING_DIRECTORY_IS),
         (curDirIsLocal ? curDir : curLocalDir));
   *fname = '\0';
   Dialog(TgLoadString(STID_ENTER_TXT_FILE_TO_EXPORT_CRES), gszMsgBox, fname);
   if (*fname == '\0') return;
   if (*fname == DIR_SEP) {
      strcpy(full_fname, fname);
   } else {
      sprintf(full_fname, "%s%c%s", curDirIsLocal ? curDir : curLocalDir,
            DIR_SEP, fname);
   }
   if ((c_ptr=UtilStrRChr(full_fname, (int)DIR_SEP)) == NULL) {
      if ((dot_ptr=UtilStrRChr(full_fname, (int)'.')) == NULL) {
         sprintf(&full_fname[strlen(full_fname)], ".%s", TEXT_FILE_EXT);
      } else {
         if (strcmp(&dot_ptr[1], TEXT_FILE_EXT) != 0) {
            sprintf(&dot_ptr[strlen(dot_ptr)], ".%s", TEXT_FILE_EXT);
         }
      }
   } else {
      if ((dot_ptr=UtilStrRChr(c_ptr, (int)'.')) == NULL) {
         sprintf(&c_ptr[strlen(c_ptr)], ".%s", TEXT_FILE_EXT);
      } else {
         if (strcmp(&dot_ptr[1], TEXT_FILE_EXT) != 0) {
            sprintf(&dot_ptr[strlen(dot_ptr)], ".%s", TEXT_FILE_EXT);
         }
      }
   }
   if (!OkayToCreateFile(full_fname)) return;

   if ((short_name=IsPrefix(bootDir, full_fname, &rest))) ++rest;
   if ((fp=fopen(full_fname, "w")) == NULL) {
      sprintf(gszMsgBox, TgLoadString(STID_CANNOT_OPEN_FILE_FOR_WRITING),
            (short_name ? rest : full_fname));
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return;
   }
   sprintf(gszMsgBox, TgLoadCachedString(CSTID_WRITING_ATTR_TO_NAMED_FILE),
         (short_name ? rest : full_fname));
   Msg(gszMsgBox);
   writeFileFailed = FALSE;

   for (attr_ptr=topSel->obj->lattr; !writeFileFailed && attr_ptr != NULL;
         attr_ptr=attr_ptr->prev) {
      MiniLineInfo *pMiniLine=NULL;

      if (attr_ptr != topSel->obj->lattr) fprintf(fp, "\n");
      if (fprintf(fp, "%s%s\n", attr_ptr->attr_name.s,
            attr_ptr->attr_value.s) == EOF) {
         writeFileFailed = TRUE;
      }
      if (!writeFileFailed &&
            (pMiniLine=attr_ptr->obj->detail.t->minilines.first) != NULL) {
         for (pMiniLine=pMiniLine->next; !writeFileFailed && pMiniLine != NULL;
               pMiniLine=pMiniLine->next) {
            int need_to_free_tmp_buf=FALSE;
            char *tmp_buf=ConvertMiniLineToString(pMiniLine,
                  &need_to_free_tmp_buf);

            if (fprintf(fp, "%s\n", tmp_buf) == EOF) {
               writeFileFailed = TRUE;
            }
            if (need_to_free_tmp_buf) UtilFree(tmp_buf);
            if (writeFileFailed) break;
         }
      }
   }
   if (writeFileFailed) {
      writeFileFailed = FALSE;
      FailToWriteFileMessage(full_fname);
   } else {
      sprintf(gszMsgBox, TgLoadString(STID_ATTR_EXPORTED_TO_NAMED_FILE),
            (short_name ? rest : full_fname));
      Msg(gszMsgBox);
   }
   fclose(fp);
}

static
int GetMergeSpec(obj_ptr, attr_name, pn_val, distance)
   struct ObjRec *obj_ptr;
   char *attr_name;
   int *pn_val, distance;
{
   struct AttrRec *attr_ptr=FindAttrWithName(obj_ptr, attr_name, NULL);
   char *c_ptr;
   double val;

   if (attr_ptr == NULL) {
      sprintf(gszMsgBox, TgLoadString(STID_CANT_FIND_ATTR_FOR_MERGEWTBL),
            attr_name);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return FALSE;
   }
   if (distance && ((c_ptr=strstr(attr_ptr->attr_value.s, "in")) != NULL ||
         (c_ptr=strstr(attr_ptr->attr_value.s, "In")) != NULL ||
         (c_ptr=strstr(attr_ptr->attr_value.s, "IN")) != NULL)) {
      char saved_ch=(*c_ptr);

      *c_ptr = '\0';
      if (sscanf(attr_ptr->attr_value.s, "%lf", &val) != 1) {
         *c_ptr = saved_ch;
         sprintf(gszMsgBox, TgLoadString(STID_MALFORMED_ATTR_FOR_MERGEWTBL),
               attr_name);
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
         return FALSE;
      }
      *c_ptr = saved_ch;
      val = val * ((double)PIX_PER_INCH);
      *pn_val = round(val);
   } else if (distance &&
         ((c_ptr=strstr(attr_ptr->attr_value.s, "cm")) != NULL ||
         (c_ptr=strstr(attr_ptr->attr_value.s, "Cm")) != NULL ||
         (c_ptr=strstr(attr_ptr->attr_value.s, "CM")) != NULL)) {
      char saved_ch=(*c_ptr);

      *c_ptr = '\0';
      if (sscanf(attr_ptr->attr_value.s, "%lf", &val) != 1) {
         *c_ptr = saved_ch;
         sprintf(gszMsgBox, TgLoadString(STID_MALFORMED_ATTR_FOR_MERGEWTBL),
               attr_name);
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
         return FALSE;
      }
      *c_ptr = saved_ch;
      val = val * ((double)ONE_CM);
      *pn_val = round(val);
   } else {
      if (sscanf(attr_ptr->attr_value.s, "%d", pn_val) != 1) {
         sprintf(gszMsgBox, TgLoadString(STID_MALFORMED_ATTR_FOR_MERGEWTBL),
               attr_name);
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
         return FALSE;
      }
   }
   return TRUE;
}

void MergeWithTable()
{
   char fname[MAXPATHLENGTH+1], *rest, paper_size_spec[80], *spec, **col_names;
   char *buf, *c_ptr, *val_ptr;
   XEvent ev;
   int i, short_name, columns_in_file, ok=TRUE, line_num, user_placement;
   int left_margin=0, top_margin=0, v_pitch=0, h_pitch=0, strip_double_quotes;
   int num_cols=0, num_rows=0, paper_w=0, paper_h=0, x, y, r, c;
   int tab_separated=TRUE;
   FILE *fp;
   struct ObjRec *template_obj;

   if (topSel == NULL || topSel != botSel) {
      MsgBox(TgLoadString(STID_SEL_ONE_OBJ_FOR_MERGEWTBL), TOOL_NAME, INFO_MB);
      return;
   }
   /* do not translate -- program constants */
   template_obj = topSel->obj;
   user_placement = (FindAttrWithName(template_obj, "USER_PLACEMENT", NULL) !=
         NULL);
   strip_double_quotes = (FindAttrWithName(template_obj, "STRIP_DOUBLE_QUOTES",
         NULL) != NULL);
   if (!user_placement) {
      /* do not translate -- program constants */
      if (!(GetMergeSpec(template_obj, "LEFT_MARGIN=", &left_margin, TRUE) &&
            GetMergeSpec(template_obj, "TOP_MARGIN=", &top_margin, TRUE) &&
            GetMergeSpec(template_obj, "V_PITCH=", &v_pitch, TRUE) &&
            GetMergeSpec(template_obj, "H_PITCH=", &h_pitch, TRUE) &&
            GetMergeSpec(template_obj, "NUM_COLS=", &num_cols, FALSE) &&
            GetMergeSpec(template_obj, "NUM_ROWS=", &num_rows, FALSE) &&
            GetMergeSpec(template_obj, "PAPER_WIDTH=", &paper_w, TRUE) &&
            GetMergeSpec(template_obj, "PAPER_HEIGHT=", &paper_h, TRUE))) {
         return;
      }
   }
   while (!DirIsRemote(curDir) && fileModified && !IsFiletUnSavable()) {
      XBell(mainDisplay, 0);
      switch (MsgBox(TgLoadString(STID_FILE_MOD_SAVE_BEFORE_MERGE), TOOL_NAME,
            YNC_MB)) {
      case MB_ID_YES: SaveFile(); break;
      case MB_ID_NO: SetFileModified(FALSE); break;
      case MB_ID_CANCEL: return;
      }
   }
   if (firstCmd != NULL) {
      if (MsgBox(TgLoadString(STID_Q_MERGEWTBL_CANT_UNDO_PROCEED), TOOL_NAME,
            YNC_MB) != MB_ID_YES) {
         return;
      }
      CleanUpCmds();
   }
   if (SelectFileNameToImport(TgLoadString(STID_SEL_TEXT_FILE_FOR_MERGEWTBL),
         TEXT_FILE_EXT, fname) == INVALID) {
      return;
   } else if (FileIsRemote(fname)) {
      MsgBox(TgLoadString(STID_CANT_MERGE_W_REMOTE_TEXT_FILE), TOOL_NAME,
            INFO_MB);
      return;
   }
   XSync(mainDisplay, False);
   if (XCheckMaskEvent(mainDisplay, ExposureMask, &ev)) {
      ExposeEventHandler(&ev, TRUE);
   }
   if ((short_name=IsPrefix(bootDir, fname, &rest))) ++rest;
   if ((fp=fopen(fname, "r")) == NULL) {
      sprintf(gszMsgBox, TgLoadString(STID_CANNOT_OPEN_FILE_FOR_READING),
            (short_name ? rest : fname));
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return;
   }
   if ((spec=UtilGetALine(fp)) == NULL) {
      fclose(fp);
      sprintf(gszMsgBox, TgLoadString(STID_FIND_COL_NAMES_IN_FILE_ABORT),
            (short_name ? rest : fname));
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return;
   }
   columns_in_file = 1;
   val_ptr = spec;
   c_ptr = strchr(val_ptr, '\t');
   if (c_ptr == NULL && (c_ptr=strchr(val_ptr, ';')) != NULL) {
      tab_separated = FALSE;
      Msg(TgLoadCachedString(CSTID_SEMICOLON_USED_AS_SEPARATOR));
   } else {
      Msg(TgLoadCachedString(CSTID_TAB_USED_AS_SEPARATOR));
   }
   while (val_ptr != NULL) {
      if (c_ptr != NULL) *c_ptr = '\0';
      if (*val_ptr == '\0') {
         free(spec);
         fclose(fp);
         sprintf(gszMsgBox, TgLoadString(STID_MALFORMED_COL_NAMES_ABORT_MRG),
               (short_name ? rest : fname));
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
         return;
      }
      if (c_ptr == NULL) break;
      *c_ptr++ = (tab_separated ? '\t' : ';');
      val_ptr = c_ptr;
      c_ptr = strchr(val_ptr, tab_separated ? '\t' : ';');

      columns_in_file++;
   }
   col_names = (char**)malloc((columns_in_file+1)*sizeof(char*));
   if (col_names == NULL) {
      free(spec);
      fclose(fp);
      FailAllocMessage();
      return;
   }
   col_names[columns_in_file] = NULL;
   i = 0;
   val_ptr = spec;
   for (c_ptr=strchr(val_ptr, tab_separated ? '\t' : ';'); val_ptr != NULL;
         i++) {
      int len;

      if (c_ptr != NULL) *c_ptr = '\0';
      if (!tab_separated) {
         UtilTrimBlanks(val_ptr);
         if (strip_double_quotes && *val_ptr == '"') {
            len = strlen(val_ptr);

            if (val_ptr[len-1] == '"') {
               val_ptr[len-1] = '\0';
               val_ptr++;
            }
         }
      }
      len = strlen(val_ptr);
      if ((col_names[i]=(char*)malloc((len+2)*sizeof(char))) == NULL) {
         FailAllocMessage();
         columns_in_file = i;
         for (i=0; i < columns_in_file; i++) {
            if (col_names[i] != NULL) {
               free(col_names[i]);
            }
         }
         free(col_names);
         free(spec);
         fclose(fp);
         return;
      }
      sprintf(col_names[i], "%s=", val_ptr);
      if (FindAttrWithName(template_obj, col_names[i], NULL) == NULL) {
         free(col_names[i]);
         col_names[i] = NULL;
      }
      if (c_ptr == NULL) break;
      *c_ptr++ = (tab_separated ? '\t' : ';');
      val_ptr = c_ptr;
      c_ptr = strchr(val_ptr, tab_separated ? '\t' : ';');
   }
   free(spec);

   MakeQuiescent();
   UnlinkObj(template_obj);
   NewProc();
   if (pageLayoutMode == PAGE_TILE) {
      PageLayoutSubMenu(PAGE_STACK);
      while (lastPageNum > 1) {
         DeleteCurPage();
      }
   }
   if (!user_placement) {
      sprintf(paper_size_spec, "%1d x %1d", paper_w, paper_h);
      if (!SetPaperSize(paper_size_spec)) {
         return;
      }
      printMag = (float)100.0;
      UpdPageStyle(PORTRAIT);
   }
   UpdDrawWinBBox();
   AdjSplineVs();
   RedrawScrollBars();
   RedrawRulers();
   RedrawTitleWindow();

   SaveStatusStrings();
   if (user_placement) {
      strcpy(gszMsgBox, TgLoadString(STID_LF_BTN_PLACE_MRG_OTHER_CANCEL));
      Msg(gszMsgBox);
      SetStringStatus(gszMsgBox);
   }
   line_num = 1;
   x = left_margin;
   y = top_margin;
   r = c = 0;
   while (ok && (buf=UtilGetALine(fp)) != NULL) {
      struct ObjRec *obj_ptr=DupObj(template_obj);

      if (obj_ptr == NULL) {
         ok = FALSE;
         free(buf);
         break;
      }
      sprintf(gszMsgBox, TgLoadCachedString(CSTID_PROCESSING_OBJ_NUMBER),
            line_num);
      SetStringStatus(gszMsgBox);
      XSync(mainDisplay, False);

      AddObj(NULL, topObj, obj_ptr);
      AdjObjBBox(obj_ptr);
      MoveObj(obj_ptr, x-obj_ptr->obbox.ltx, y-obj_ptr->obbox.lty);

      line_num++;
      i = 0;
      val_ptr = buf;
      replaceAttrFirstValueRedraw = FALSE;
      for (c_ptr=strchr(val_ptr, tab_separated ? '\t' : ';'); val_ptr != NULL;
            i++) {
         struct AttrRec *attr_ptr;

         if (c_ptr != NULL) *c_ptr = '\0';

         if (col_names[i] != NULL && (attr_ptr=FindAttrWithName(obj_ptr,
               col_names[i], NULL)) != NULL) {
            if (!tab_separated) UtilTrimBlanks(val_ptr);
            if (strip_double_quotes && *val_ptr == '"') {
               int len=strlen(val_ptr);

               if (val_ptr[len-1] == '"') {
                  val_ptr[len-1] = '\0';
                  ReplaceAttrFirstValue(obj_ptr, attr_ptr, &val_ptr[1]);
                  val_ptr[len-1] = '"';
               } else {
                  ReplaceAttrFirstValue(obj_ptr, attr_ptr, val_ptr);
               }
            } else {
               ReplaceAttrFirstValue(obj_ptr, attr_ptr, val_ptr);
            }
         }
         if (c_ptr == NULL) break;
         *c_ptr++ = (tab_separated ? '\t' : ';');
         val_ptr = c_ptr;
         c_ptr = strchr(val_ptr, tab_separated ? '\t' : ';');
      }
      replaceAttrFirstValueRedraw = TRUE;
      UnlinkObj(obj_ptr);
      if (firstCmd != NULL) CleanUpCmds();
      if (i+1 != columns_in_file) {
         ok = FALSE;
         sprintf(gszMsgBox, TgLoadString(STID_MALFORMED_TBL_LINE_ABORT_MRG),
               line_num, (short_name ? rest : fname));
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
         FreeObj(obj_ptr);
         ClearAndRedrawDrawWindow();
      } else {
         struct AttrRec *attr_ptr=NULL;
         int saved_history_depth=historyDepth;

         AdjObjBBox(obj_ptr);
         AddObj(NULL, topObj, obj_ptr);
         if (user_placement) {
            if ((attr_ptr=FindAttrWithName(obj_ptr, "name=",
                  NULL)) != NULL) {
               sprintf(gszMsgBox, TgLoadCachedString(CSTID_PLACING_NAMED_OBJ),
                     attr_ptr->attr_value.s);
               SetStringStatus(gszMsgBox);
               XSync(mainDisplay, False);
            }
            if (PlaceTopObj(obj_ptr, NULL, NULL) == Button3) {
               Msg(TgLoadString(STID_OPERATION_CANCEL_BY_USER));
               ok = FALSE;
            }
            AssignNewObjIds(obj_ptr);
         }
         numRedrawBBox = 0;
         obj_ptr->tmp_parent = NULL;
         DrawObj(drawWindow, obj_ptr);
         /* do not translate -- program constants */
         if ((attr_ptr=FindAttrWithName(obj_ptr, "EXEC_AFTER_MERGE=",
               NULL)) != NULL) {
            char name[MAXSTRING+1];
            struct AttrRec *next_attr_ptr=NULL;

            if (*attr_ptr->attr_value.s != '\0') {
               sprintf(name, "%s=", attr_ptr->attr_value.s);
               if ((next_attr_ptr=FindAttrWithName(obj_ptr, name, NULL)) !=
                     NULL) {
                  int saved_intr_check_interval=intrCheckInterval;

                  intrCheckInterval = 1;
                  ShowInterrupt(1);
                  if (!DoExec(next_attr_ptr, obj_ptr)) {
                     ok = FALSE;
                  }
                  while (HideInterrupt() > 0) ;
                  intrCheckInterval = saved_intr_check_interval;
               } else {
                  sprintf(gszMsgBox,
                        TgLoadString(STID_CANT_FND_NAMED_ATTR_ABORT_MRG),
                        attr_ptr->attr_value.s);
                  MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
                  ok = FALSE;
               }
            }
            attr_ptr = next_attr_ptr;
         } else if ((attr_ptr=FindAttrWithName(obj_ptr, EXEC_ATTR, NULL)) !=
               NULL) {
            int saved_intr_check_interval=intrCheckInterval;

            intrCheckInterval = 1;
            ShowInterrupt(1);
            if (!DoExec(attr_ptr, obj_ptr)) {
               ok = FALSE;
            }
            while (HideInterrupt() > 0) ;
            intrCheckInterval = saved_intr_check_interval;
         }
         if (saved_history_depth != historyDepth) RestoreDefaultHistoryDepth();
         if (firstCmd != NULL) CleanUpCmds();
         if (ok && !user_placement) {
            c++;
            if (c >= num_cols) {
               c = 0;
               x = left_margin;
               r++;
               if (r >= num_rows) {
                  r = 0;
                  y = top_margin;
                  AddPageAfter();
               } else {
                  y += v_pitch;
               }
            } else {
               x += h_pitch;
            }
         }
      }
      free(buf);
   }
   fclose(fp);

   RestoreStatusStrings();
   sprintf(gszMsgBox, TgLoadString(STID_NUM_OBJECTS_GENERATED), line_num-1);
   MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);

   FreeObj(template_obj);
   for (i=0; i < columns_in_file; i++) {
      if (col_names[i] != NULL) {
         free(col_names[i]);
      }
   }
   free(col_names);
   if (firstCmd != NULL) CleanUpCmds();
   SetFileModified(TRUE);
   justDupped = FALSE;
}

void ExportToTable()
{
   char msg[MAXSTRING+1], fname[MAXPATHLENGTH+1], full_fname[MAXPATHLENGTH+1];
   char *c_ptr, *dot_ptr, *rest=NULL, *spec, *spec_copy, **col_names=NULL, *buf;
   int short_name, total, i, num_exported, buf_sz, ok=TRUE;
   FILE *fp;
   struct AttrRec *spec_attr=NULL;
   MiniLineInfo *pMiniLine=NULL;
   struct SelRec *sel_ptr=NULL;

   if (topSel == NULL) {
      MsgBox(TgLoadCachedString(CSTID_NO_OBJ_SELECTED), TOOL_NAME, INFO_MB);
      return;
   }
   /* do not translate -- program constants */
   strcpy(msg, "!.TABLE_ATTRS=");
   spec_attr = FindAttrWithName(NULL, msg, NULL);
   if (spec_attr == NULL) {
      MsgBox(TgLoadString(STID_CANT_FND_TABLE_ATTRS_FILE_ATT), TOOL_NAME,
            INFO_MB);
      return;
   }
   sprintf(gszMsgBox, TgLoadString(STID_WORKING_DIRECTORY_IS),
         (curDirIsLocal ? curDir : curLocalDir));
   *fname = '\0';
   Dialog(TgLoadString(STID_ENTER_TXT_FILE_TO_EXPORT_CRES), gszMsgBox, fname);

   if (*fname == '\0') return;

   if (*fname == DIR_SEP) {
      strcpy(full_fname, fname);
   } else {
      sprintf(full_fname, "%s%c%s", curDirIsLocal ? curDir : curLocalDir,
            DIR_SEP, fname);
   }
   if ((c_ptr=UtilStrRChr(full_fname, (int)DIR_SEP)) == NULL) {
      if ((dot_ptr=UtilStrRChr(full_fname, (int)'.')) == NULL) {
         sprintf(&full_fname[strlen(full_fname)], ".%s", TEXT_FILE_EXT);
      } else {
         if (strcmp(&dot_ptr[1], TEXT_FILE_EXT) != 0) {
            sprintf(&dot_ptr[strlen(dot_ptr)], ".%s", TEXT_FILE_EXT);
         }
      }
   } else {
      if ((dot_ptr=UtilStrRChr(c_ptr, (int)'.')) == NULL) {
         sprintf(&c_ptr[strlen(c_ptr)], ".%s", TEXT_FILE_EXT);
      } else {
         if (strcmp(&dot_ptr[1], TEXT_FILE_EXT) != 0) {
            sprintf(&dot_ptr[strlen(dot_ptr)], ".%s", TEXT_FILE_EXT);
         }
      }
   }
   if (!OkayToCreateFile(full_fname)) return;

   if ((short_name=IsPrefix(bootDir, full_fname, &rest))) ++rest;
   if ((fp=fopen(full_fname, "w")) == NULL) {
      sprintf(gszMsgBox, TgLoadString(STID_CANNOT_OPEN_FILE_FOR_WRITING),
            (short_name ? rest : full_fname));
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return;
   }
   sprintf(gszMsgBox, TgLoadCachedString(CSTID_WRITING_ATTRS_IN_TBL_FORM_TO),
         (short_name ? rest : full_fname));
   Msg(gszMsgBox);
   writeFileFailed = FALSE;

   total = 0;
   for (pMiniLine=spec_attr->obj->detail.t->minilines.first; pMiniLine != NULL;
         pMiniLine=pMiniLine->next) {
      int need_to_free_tmp_buf=FALSE;
      char *tmp_buf=ConvertMiniLineToString(pMiniLine, &need_to_free_tmp_buf);

      total += strlen(tmp_buf)+1;
      if (need_to_free_tmp_buf) UtilFree(tmp_buf);
   }
   if ((spec=(char*)malloc((total+1)*sizeof(char))) == NULL) {
      FailAllocMessage();
      fclose(fp);
      return;
   }
   c_ptr = spec;
   for (pMiniLine=spec_attr->obj->detail.t->minilines.first; pMiniLine != NULL;
         pMiniLine=pMiniLine->next) {
      int len=0, need_to_free_tmp_buf=FALSE;
      char *tmp_buf=NULL;

      if (pMiniLine == spec_attr->obj->detail.t->minilines.first) {
         char *attr_value=NULL;

         tmp_buf = ConvertMiniLineToString(pMiniLine, &need_to_free_tmp_buf);
         attr_value = UtilStrDup(tmp_buf);
         if (attr_value == NULL) FailAllocMessage();
         ParseAttrStr(tmp_buf, NULL, 0, attr_value, strlen(attr_value)+1);
         if (need_to_free_tmp_buf) UtilFree(tmp_buf);
         need_to_free_tmp_buf = TRUE;
         tmp_buf = attr_value;
      } else {
         tmp_buf = ConvertMiniLineToString(pMiniLine, &need_to_free_tmp_buf);
      }
      len = strlen(tmp_buf);
      strcpy(c_ptr, tmp_buf);
      if (need_to_free_tmp_buf) UtilFree(tmp_buf);
      c_ptr += len;
      *c_ptr++ = ',';
   }
   *c_ptr = '\0';
   if ((spec_copy=UtilStrDup(spec)) == NULL) {
      FailAllocMessage();
      free(spec);
      fclose(fp);
      return;
   }
   total = 0;
   for (c_ptr=strtok(spec_copy, " ,;\t\n\r"); c_ptr != NULL;
         c_ptr=strtok(NULL, " ,;\t\n\r")) {
      total++;
   }
   free(spec_copy);

   buf_sz = 0x400;
   spec_copy = UtilStrDup(spec);
   col_names = (char**)malloc((total+1)*sizeof(char*));
   buf = (char*)malloc((buf_sz+2)*sizeof(char));
   if (spec_copy == NULL || col_names == NULL || buf == NULL) {
      FailAllocMessage();
      if (col_names != NULL) free(col_names);
      if (spec_copy != NULL) free(spec_copy);
      if (buf != NULL) free(buf);
      free(spec);
      fclose(fp);
      return;
   }
   col_names[total] = NULL;
   i = 0;
   for (c_ptr=strtok(spec_copy, " ,;\t\n\r"); c_ptr != NULL && !writeFileFailed;
         c_ptr=strtok(NULL, " ,;\t\n\r"), i++) {
      int len=strlen(c_ptr);

      if ((col_names[i]=(char*)malloc((len+2)*sizeof(char))) == NULL) {
         FailAllocMessage();
         for (total=0; total < i; total++) free(col_names[i]);
         free(col_names);
         free(spec_copy);
         free(buf);
         free(spec);
         fclose(fp);
         return;
      }
      if (fprintf(fp, "%s%s", (i==0 ? "" : "\t"), c_ptr) == EOF) {
         writeFileFailed = TRUE;
      }
      sprintf(col_names[i], "%s=", c_ptr);
   }
   fprintf(fp, "\n");

   num_exported = 0;
   SaveStatusStrings();
   for (sel_ptr=botSel; ok && sel_ptr != NULL; sel_ptr=sel_ptr->prev) {
      int percent=(i*10000/numObjSelected)/100, col;
      int something_exported=FALSE, cur_len=0;

      sprintf(gszMsgBox, TgLoadCachedString(CSTID_PROGRESS_PERCENT), percent);
      SetStringStatus(gszMsgBox);
      XSync(mainDisplay, False);

      for (col=0; ok && col < total; col++) {
         int len;
         struct AttrRec *attr_ptr=FindAttrWithName(sel_ptr->obj,
               col_names[col], NULL);

         if (attr_ptr != NULL) something_exported = TRUE;

         sprintf(gszMsgBox, "%s%s", (col==0 ? "" : "\t"),
               (attr_ptr==NULL ? "": attr_ptr->attr_value.s));
         len = strlen(gszMsgBox);
         while (len+cur_len >= buf_sz) {
            buf_sz += 0x400;
            buf = (char*)realloc(buf, (buf_sz+2)*sizeof(char));
            if (buf == NULL) {
               FailAllocMessage();
               ok = FALSE;
               break;
            }
         }
         if (ok) {
            sprintf(&buf[cur_len], gszMsgBox);
            cur_len += len;
         }
      }
      buf[cur_len] = '\0';
      if (something_exported) {
         num_exported++;
         if (fprintf(fp, "%s\n", buf) == EOF) {
            writeFileFailed = TRUE;
            ok = FALSE;
         }
      }
   }
   RestoreStatusStrings();
   sprintf(gszMsgBox, TgLoadString(STID_NUM_OBJECTS_EXPORTED), num_exported);
   Msg(gszMsgBox);

   for (i=0; i < total; i++) free(col_names[i]);
   free(col_names);
   free(spec_copy);
   if (buf != NULL) free(buf);
   free(spec);
   fclose(fp);

   if (writeFileFailed) {
      writeFileFailed = FALSE;
      FailToWriteFileMessage(full_fname);
   } else {
      sprintf(gszMsgBox, TgLoadString(STID_ATTRS_EXPORTED_TO_TBL_FILE),
            (short_name ? rest : full_fname));
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
   }
}

void ToggleShowWireSignalName()
{
   showWireSignalName = !showWireSignalName;
   sprintf(gszMsgBox, TgLoadString(showWireSignalName ?
         STID_WILL_SHOW_WIRE_SIGNAL_NAME : STID_WILL_HIDE_WIRE_SIGNAL_NAME));
   Msg(gszMsgBox);
}

int RefreshPortMenu(menu)
   TgMenu *menu;
{
   int ok=TRUE;

   /* Show Wire Signal Name */
   ok &= TgSetMenuItemCheckById(menu, CMDID_TOGGLESHOWWIRESIGNALNAME,
         showWireSignalName);

   return ok;
}

TgMenu *CreatePortMenu(parent_menu, x, y, menu_info, status_str_xlated)
   TgMenu *parent_menu;
   int x, y;
   TgMenuInfo *menu_info;
   int status_str_xlated; /* ignored, always 0 */
{
   TgMenu *menu=TgCreateMenuFromMenuInfo(parent_menu, x, y, menu_info, FALSE);

   if (menu != NULL) {
      if (!RefreshPortMenu(menu)) {
         return TgDestroyMenu(menu, TRUE);
      }
      menu->refresh_proc = ((RefreshMenuFunc*)RefreshPortMenu);
   }
   return menu;
}

void RefreshSpecialMenu(menu)
   TgMenu *menu;
{
}

int SpecialMenu(X, Y, TrackMenubar)
   int X, Y, TrackMenubar;
{
   int rc=INVALID;
   TgMenu *menu=(specialMenuInfo.create_proc)(NULL, X, Y, &specialMenuInfo,
         FALSE);

   activeMenu = MENU_SPECIAL;
   if (menu != NULL) {
      menu->track_menubar = TrackMenubar;

      rc = TgMenuLoop(menu);
      TgDestroyMenu(menu, TRUE);
   }
   return rc;
}

void CleanUpSpecial()
{
}

int InitSpecial()
{
   char *c_ptr=NULL;

   showWireSignalName = TRUE;
   if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"ShowWireSignalName")) !=
         NULL && UtilStrICmp(c_ptr, "false") == 0) {
      showWireSignalName = FALSE;
   }
   return TRUE;
}
