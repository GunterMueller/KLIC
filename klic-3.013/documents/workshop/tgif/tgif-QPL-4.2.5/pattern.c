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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/pattern.c,v 1.26 2011/05/16 16:21:58 william Exp $
 */

#define _INCLUDE_FROM_PATTERN_C_

#include "tgifdefs.h"
#include "cmdids.h"

#include "arc.e"
#include "choice.e"
#include "color.e"
#include "cmd.e"
#include "cutpaste.e"
#include "dialog.e"
#include "drawing.e"
#include "file.e"
#include "font.e"
#include "mainmenu.e"
#include "mark.e"
#include "menu.e"
#include "menuinfo.e"
#include "miniline.e"
#include "msg.e"
#include "navigate.e"
#include "obj.e"
#include "pattern.e"
#include "poly.e"
#include "raster.e"
#include "select.e"
#include "setup.e"
#include "spline.e"
#include "strtbl.e"
#include "text.e"
#include "util.e"

int     objFill=NONEPAT;
int     transPat=FALSE;
int     lineStyle=LS_RIGHT;
int     lineWidth=0;
int     penPat=SOLIDPAT;
int     curSpline=LT_STRAIGHT;
int     curDash=0;
int     rcbRadius=DEF_RCB_RADIUS;
int     useGray=FALSE;
char    patternStr[]="8 1 0 72 300 32 div div tgifsetpattern";

int	stickyMenuSelection=FALSE;

static int	tileAsGrayDetected=FALSE;
static int	canFakeGray=FALSE;
static char	*grayStr[9] =
{
   "0.995", "0.94", "0.868", "0.779", "0.763", "0.55", "0.41", "0.253", "0.079"
};

void ResetGrayDetection()
{
   tileAsGrayDetected = FALSE;
   canFakeGray = FALSE;
}

char *GrayStr(index)
   int index;
   /* this routine should only be called when useGray == TRUE */
{
   if (index <= 2) {
      fprintf(stderr, "%s\n",
            TgLoadCachedString(CSTID_GRAYSTR_CALLED_WITH_IDX_2));
      return("");
   } else if (index >= 12) {
      if (!tileAsGrayDetected) {
         char buf[MAXSTRING];

         tileAsGrayDetected = TRUE;
         strcpy(buf, TgLoadCachedString(CSTID_GRAY_SCALE_USED_FOR_PAT));
         if (PRTGIF) {
            fprintf(stderr, "%s.\n", buf);
         } else {
            MsgBox(buf, TOOL_NAME, INFO_MB);
         }
      }
      switch (index) {
      case 12: index = 5; break;
      case 13: index = 4; break;
      case 14: index = 7; break;
      case 15: index = 6; break;
      case 16: index = 7; break;
      case 17: index = 6; break;
      case 18: index = 6; break;
      case 19: index = 6; break;
      case 20: index = 5; break;
      case 21: index = 6; break;
      case 22: index = 8; break;
      case 23: index = 7; break;
      case 24: index = 9; break;
      case 25: index = 8; break;
      case 26: index = 5; break;
      case 27: index = 6; break;
      case 28: index = 8; break;
      case 29: index = 7; break;
      case 30: index = 9; break;
      case 31: index = 8; break;
      }
   }
   return (grayStr[index-3]);
}

void GrayCheck(index)
   int index;
   /* this routine should only be called when useGray == FALSE */
{
   if (index > BACKPAT) {
      if (index >= 12) {
         tileAsGrayDetected = TRUE;
      } else {
         canFakeGray = TRUE;
      }
   }
}

void EndGrayDetection()
   /* this routine should only be called when useGray == FALSE */
{
   int num_msgs = 1;
   char msg1[MAXSTRING], msg2[MAXSTRING];

   if (colorDump) return;

   if (useGray) {
      if (!tileAsGrayDetected && !canFakeGray) return;

      strcpy(msg1, TgLoadString(STID_GRAY_SCALE_USED_IN_PRINT_PAT));
   } else if (tileAsGrayDetected) {
      strcpy(msg1, TgLoadString(STID_NOTE_SLOW_PRINT_DUE_USE_PAT));
   } else if (canFakeGray) {
      num_msgs = 2;
      strcpy(msg1, TgLoadString(STID_NOTE_SLOW_PRINT_DUE_USE_PAT));
      sprintf(msg2, "      %s",
            TgLoadString(STID_SUGGEST_USEGRAYSCALE_TO_SPEED));
   } else {
      return;
   }
   if (PRTGIF) {
      fprintf(stderr, "%s.\n", msg1);
      if (num_msgs==2) fprintf(stderr, "%s.\n", msg2);
   } else {
      if (num_msgs==1) {
         Msg(msg1);
      } else {
         TwoLineMsg(msg1, msg2);
      }
   }
}

int RefreshModeMenu(menu)
   TgMenu *menu;
{
   int i, num_items=menu->num_items, rc=TRUE;
   TgMenuItem *menuitems=menu->menuitems;

   for (i=0; i < num_items; i++) {
      TgMenuItem *menu_item=(&menuitems[i]);
      TgMenuItem stMenuItem;

      memset(&stMenuItem, 0, sizeof(TgMenuItem));
      stMenuItem.state = TGBS_NORMAL;
      stMenuItem.checked = (i == curChoice);
      if (!TgSetMenuItemInfo(menu_item, TGMU_MASK_STATE|TGMU_MASK_CHECK,
            &stMenuItem)) {
         rc = FALSE;
      }
   }
   if (inSlideShow && !goHyperSpaceInSlideShow) {
      TgEnableMenuItemByIndex(menu, NOTHING, FALSE);
      TgEnableMenuItemByIndex(menu, VERTEXMODE, FALSE);
      TgEnableMenuItemByIndex(menu, ROTATEMODE, FALSE);
   }
   return rc;
}

TgMenu *CreateModeMenu(parent_menu, x, y, menu_info, status_str_xlated)
   TgMenu *parent_menu;
   int x, y;
   TgMenuInfo *menu_info;
   int status_str_xlated; /* ignored, always 0 */
{
   TgMenu *menu=TgCreateMenuFromMenuInfo(parent_menu, x, y, menu_info, FALSE);

   if (menu != NULL) {
      TgMenuItem *menu_item=NULL;
      TgMenuItem stMenuItem;

      menu->track_menubar = FALSE;
      TgAdjustMenuGeometry(menu, choiceImageW, choiceImageH, MAXCHOICES);
      menu_item = (&menu->menuitems[curChoice]);

      memset(&stMenuItem, 0, sizeof(TgMenuItem));
      stMenuItem.checked = TRUE;
      if (!TgSetMenuItemInfo(menu_item, TGMU_MASK_CHECK, &stMenuItem)) {
         return TgDestroyMenu(menu, TRUE);
      }
      if (!RefreshModeMenu(menu)) {
         return TgDestroyMenu(menu, TRUE);
      }
      menu->refresh_proc = ((RefreshMenuFunc*)RefreshModeMenu);
   }
   return menu;
}

int ModeMenu(X, Y, TrackMenubar)
   int X, Y, TrackMenubar;
{
   int rc=INVALID;
   TgMenu *menu=(modeMenuInfo.create_proc)(NULL, X, Y, &modeMenuInfo, INVALID);

   activeMenu = MENU_MODE;
   if (menu != NULL) {
      menu->track_menubar = TrackMenubar;

      rc = TgMenuLoop(menu);
      TgDestroyMenu(menu, TRUE);
   }
   return rc;
}

/* ------------------ TransPat Functions ------------------ */

int ChangeObjTransPat(ObjPtr, TransPat)
   struct ObjRec *ObjPtr;
   int TransPat;
{
   if (ObjPtr->trans_pat != TransPat) {
      ObjPtr->trans_pat = TransPat;
      return TRUE;
   }
   return FALSE;
}

void ChangeAllSelTransPat(TransPat, HighLight)
   int TransPat, HighLight;
{
   struct SelRec *sel_ptr=NULL;
   int changed=FALSE, ltx=selLtX, lty=selLtY, rbx=selRbX, rby=selRbY;

   if (topSel == NULL || stickyMenuSelection) {
      if (!(curChoice == DRAWTEXT && textCursorShown)) {
         TieLooseEnds();
      }
      transPat = TransPat;
      if (curChoice == DRAWTEXT && textCursorShown) {
         if (ChangeObjTransPat(curTextObj, TransPat)) {
            curTextModified = TRUE;
            UpdCurTextBBox();
            RedrawCurText();
            SetFileModified(TRUE);
            if (cycleThroughChoice) {
               SetPushedFontValue(PUSH_TRANSPAT, transPat);
            }
         }
      } else {
         textCursorShown = FALSE;
      }
      if (topSel == NULL) {
         switch (transPat) {
         case NO_TRANSPAT_MODE:
            Msg(TgLoadString(STID_FILL_PEN_PAT_OPAQUE));
            break;
         case TRANSPAT_MODE:
            Msg(TgLoadString(STID_FILL_PEN_PAT_TRANSPARENT));
            break;
         }
      }
      ShowTransPatMode();
      UpdatePinnedMenu(MENU_TRANSPAT);
      if (topSel == NULL) return;
   }

   if (HighLight) HighLightReverse();
   StartCompositeCmd();
   for (sel_ptr=botSel; sel_ptr != NULL; sel_ptr=sel_ptr->prev) {
      PrepareToReplaceAnObj(sel_ptr->obj);
      if (ChangeObjTransPat(sel_ptr->obj, TransPat)) {
         changed = TRUE;
         RecordReplaceAnObj(sel_ptr->obj);
      } else {
         AbortPrepareCmd(CMD_REPLACE);
      }
   }
   EndCompositeCmd();

   if (changed) {
      SetFileModified(TRUE);
      UpdSelBBox();
      RedrawAreas(botObj, ltx-GRID_ABS_SIZE(1), lty-GRID_ABS_SIZE(1),
            rbx+GRID_ABS_SIZE(1), rby+GRID_ABS_SIZE(1), selLtX-GRID_ABS_SIZE(1),
            selLtY-GRID_ABS_SIZE(1), selRbX+GRID_ABS_SIZE(1),
            selRbY+GRID_ABS_SIZE(1));
   }
   if (HighLight) HighLightForward();
}

/* ------------------ Fill Functions ------------------ */

int ChangeObjFill(ObjPtr, FillIndex)
   struct ObjRec *ObjPtr;
   int FillIndex;
{
   register struct ObjRec *obj_ptr;
   int changed=FALSE;

   switch (ObjPtr->type) {
   case OBJ_TEXT:
      if (ObjPtr->detail.t->fill != FillIndex) {
         ObjPtr->detail.t->fill = FillIndex;
         changed = TRUE;
      }
      break;
   case OBJ_BOX:
      if (ObjPtr->detail.b->fill != FillIndex) {
         ObjPtr->detail.b->fill = FillIndex;
         changed = TRUE;
      }
      break;
   case OBJ_OVAL:
      if (ObjPtr->detail.o->fill != FillIndex) {
         ObjPtr->detail.o->fill = FillIndex;
         changed = TRUE;
      }
      break;
   case OBJ_POLY:
      if (ObjPtr->detail.p->fill != FillIndex) {
         ObjPtr->detail.p->fill = FillIndex;
         changed = TRUE;
      }
      break;
   case OBJ_POLYGON:
      if (ObjPtr->detail.g->fill != FillIndex) {
         ObjPtr->detail.g->fill = FillIndex;
         changed = TRUE;
      }
      break;
   case OBJ_ARC:
      if (ObjPtr->detail.a->fill != FillIndex) {
         if (ObjPtr->detail.a->fill == NONEPAT || FillIndex == NONEPAT) {
            ObjPtr->detail.a->fill = FillIndex;
            AdjObjBBox(ObjPtr);
         } else {
            ObjPtr->detail.a->fill = FillIndex;
         }
         changed = TRUE;
      }
      break;
   case OBJ_RCBOX:
      if (ObjPtr->detail.rcb->fill != FillIndex) {
         ObjPtr->detail.rcb->fill = FillIndex;
         changed = TRUE;
      }
      break;
   case OBJ_XBM:
      if (ObjPtr->detail.xbm->fill != FillIndex) {
         ObjPtr->detail.xbm->fill = FillIndex;
         changed = TRUE;
      }
      break;
   case OBJ_XPM:
      if (ObjPtr->detail.xpm->fill != FillIndex) {
         ObjPtr->detail.xpm->fill = FillIndex;
         changed = TRUE;
      }
      break;

   case OBJ_SYM:
   case OBJ_GROUP:
      for (obj_ptr = ObjPtr->detail.r->last; obj_ptr != NULL;
            obj_ptr = obj_ptr->prev) {
         if (ChangeObjFill(obj_ptr, FillIndex)) {
            changed = TRUE;
         }
      }
      break;
   }
   if (changePropertiesOfAttrs && ObjPtr->type != OBJ_TEXT) {
      struct AttrRec *attr_ptr=ObjPtr->fattr;

      for ( ; attr_ptr != NULL; attr_ptr=attr_ptr->next) {
         changed |= ChangeObjFill(attr_ptr->obj, FillIndex);
      }
   }
   return changed;
}

void ChangeAllSelFill(FillIndex, HighLight)
   int FillIndex, HighLight;
{
   register struct SelRec *sel_ptr;
   int changed=FALSE, ltx, lty, rbx, rby;

   if (topSel == NULL || stickyMenuSelection) {
      if (!(curChoice == DRAWTEXT && textCursorShown)) {
         TieLooseEnds();
      }
      objFill = FillIndex;
      if (curChoice == DRAWTEXT && textCursorShown) {
         if (ChangeObjFill(curTextObj, FillIndex)) {
            curTextModified = TRUE;
            UpdCurTextBBox();
            RedrawCurText();
            SetFileModified(TRUE);
            if (cycleThroughChoice) {
               SetPushedFontValue(PUSH_FILL, objFill);
            }
         }
      } else {
         textCursorShown = FALSE;
      }
      ShowFill();
      UpdatePinnedMenu(MENU_FILL);
      if (topSel == NULL) return;
   }

   if (HighLight) HighLightReverse();
   StartCompositeCmd();
   for (sel_ptr=botSel; sel_ptr != NULL; sel_ptr=sel_ptr->prev) {
      PrepareToReplaceAnObj(sel_ptr->obj);
      if (ChangeObjFill(sel_ptr->obj, FillIndex)) {
         changed = TRUE;
         RecordReplaceAnObj(sel_ptr->obj);
      } else {
         AbortPrepareCmd(CMD_REPLACE);
      }
   }
   EndCompositeCmd();

   if (changed) {
      SetFileModified(TRUE);
      ltx = selLtX; lty = selLtY; rbx = selRbX; rby = selRbY;
      UpdSelBBox();
      RedrawAreas(botObj, ltx-GRID_ABS_SIZE(1), lty-GRID_ABS_SIZE(1),
            rbx+GRID_ABS_SIZE(1), rby+GRID_ABS_SIZE(1), selLtX-GRID_ABS_SIZE(1),
            selLtY-GRID_ABS_SIZE(1), selRbX+GRID_ABS_SIZE(1),
            selRbY+GRID_ABS_SIZE(1));
   }
   if (HighLight) HighLightForward();
}

void RefreshFillMenu(menu)
   TgMenu *menu;
{
   int i, num_items=menu->num_items;
   TgMenuItem *menuitems=menu->menuitems;

   for (i=0; i < num_items; i++) {
      TgMenuItem *menu_item=(&menuitems[i]);
      TgMenuItem stMenuItem;

      memset(&stMenuItem, 0, sizeof(TgMenuItem));
      stMenuItem.state = TGBS_NORMAL;
      stMenuItem.checked = (i == objFill);
      TgSetMenuItemInfo(menu_item, TGMU_MASK_STATE|TGMU_MASK_CHECK,
            &stMenuItem);
   }
}

TgMenu *CreateFillMenu(parent_menu, x, y, menu_info, status_str_xlated)
   TgMenu *parent_menu;
   int x, y;
   TgMenuInfo *menu_info;
   int status_str_xlated; /* ignored, always 0 */
{
   int i=0;
   TgMenu *menu=NULL;
   TgMenuInfo stMenuInfo;
   TgMenuItemInfo *item_info=NULL;

   memcpy(&stMenuInfo, menu_info, sizeof(TgMenuInfo));
   stMenuInfo.items = (TgMenuItemInfo*)malloc(
         (MAXPATTERNS+1)*sizeof(TgMenuItemInfo));
   if (stMenuInfo.items == NULL) FailAllocMessage();
   memset(stMenuInfo.items, 0, (MAXPATTERNS+1)*sizeof(TgMenuItemInfo));
   for (item_info=stMenuInfo.items, i=0; i < MAXPATTERNS; item_info++, i++) {
      item_info->menu_str = (char*)(Pixmap*)(&patPixmap[i]);
      item_info->shortcut_str = NULL;
      switch (i) {
      case NONEPAT:
         strcpy(gszMsgBox, TgLoadCachedString(CSTID_SET_FILL_TO_NONE));
         break;
      case SOLIDPAT:
         strcpy(gszMsgBox, TgLoadCachedString(CSTID_SET_FILL_TO_SOLID));
         break;
      case BACKPAT:
         strcpy(gszMsgBox, TgLoadCachedString(CSTID_SET_FILL_TO_BACKGROUND));
         break;
      default:
         sprintf(gszMsgBox, TgLoadCachedString(CSTID_SET_FILL_TO_PAT_NUMBER),
               i);
         break;
      }
      item_info->status_str = UtilStrDup(gszMsgBox);
      if (item_info->status_str == NULL) FailAllocMessage();
      item_info->submenu_info = NULL;
      item_info->cmdid = CMDID_CHANGEALLSELFILL;
   }
   stMenuInfo.items[MAXPATTERNS].cmdid = INVALID;

   /* the status_str has been translated in TgLoadCachedString() */
   menu = TgCreateMenuFromMenuInfo(parent_menu, x, y, &stMenuInfo, TRUE);
   for (item_info=stMenuInfo.items, i=0; i < MAXPATTERNS; item_info++, i++) {
      UtilFree(item_info->status_str);
   }
   memset(stMenuInfo.items, 0, (MAXPATTERNS+1)*sizeof(TgMenuItemInfo));
   free(stMenuInfo.items);
   stMenuInfo.items = NULL;
   if (menu != NULL) {
      TgMenuItem *menu_item=NULL;
      TgMenuItem stMenuItem;

      menu->track_menubar = TRUE;
      TgAdjustMenuGeometry(menu, choiceImageW, choiceImageH, 8);
      menu_item = (&menu->menuitems[objFill]);

      memset(&stMenuItem, 0, sizeof(TgMenuItem));
      stMenuItem.checked = TRUE;
      if (!TgSetMenuItemInfo(menu_item, TGMU_MASK_CHECK, &stMenuItem)) {
         return TgDestroyMenu(menu, TRUE);
      }
   }
   return menu;
}

int FillMenu(X, Y, TrackMenubar)
   int X, Y, TrackMenubar;
{
   int rc=INVALID;
   TgMenu *menu=(fillMenuInfo.create_proc)(NULL, X, Y, &fillMenuInfo, INVALID);

   activeMenu = MENU_FILL;
   if (menu != NULL) {
      menu->track_menubar = TrackMenubar;

      rc = TgMenuLoop(menu);
      TgDestroyMenu(menu, TRUE);
   }
   return rc;
}

/* ------------------ LineStyle Functions ------------------ */

static
int ChangeObjLineStyle(ObjPtr, StyleIndex)
   struct ObjRec *ObjPtr;
   int StyleIndex;
{
   register struct ObjRec *obj_ptr;
   register int changed=FALSE;

   switch (ObjPtr->type) {
   case OBJ_POLY:
      if (ObjPtr->detail.p->style != StyleIndex) {
         ObjPtr->detail.p->style = StyleIndex;
         changed = TRUE;
         AdjObjSplineVs(ObjPtr);
         if (ObjPtr->detail.p->curved != LT_INTSPLINE) {
            UpdPolyBBox(ObjPtr, ObjPtr->detail.p->n,
                  ObjPtr->detail.p->vlist);
         } else {
            UpdPolyBBox(ObjPtr, ObjPtr->detail.p->intn,
                  ObjPtr->detail.p->intvlist);
         }
      }
      break;
   case OBJ_ARC:
      if (ObjPtr->detail.a->style != StyleIndex) {
         ObjPtr->detail.a->style = StyleIndex;
         changed = TRUE;
         AdjObjSplineVs(ObjPtr);
      }
      break;
   case OBJ_GROUP:
   case OBJ_SYM:
      for (obj_ptr=ObjPtr->detail.r->last; obj_ptr != NULL;
            obj_ptr=obj_ptr->prev) {
         if (ChangeObjLineStyle(obj_ptr, StyleIndex)) {
            changed = TRUE;
         }
      }
      break;
   }
   if (changed) AdjObjBBox(ObjPtr);
   return changed;
}

void ChangeAllSelLineStyle(StyleIndex, HighLight)
   int StyleIndex, HighLight;
{
   register struct SelRec *sel_ptr;
   int ltx, lty, rbx, rby, changed=FALSE;

   if (topSel == NULL || stickyMenuSelection) {
      lineStyle = StyleIndex;
      ShowLineStyle();
      UpdatePinnedMenu(MENU_LINESTYLE);
      if (topSel == NULL) return;
   }

   if (HighLight) HighLightReverse();
   StartCompositeCmd();
   for (sel_ptr=botSel; sel_ptr != NULL; sel_ptr=sel_ptr->prev) {
      PrepareToReplaceAnObj(sel_ptr->obj);
      if (ChangeObjLineStyle(sel_ptr->obj, StyleIndex)) {
         changed = TRUE;
         RecordReplaceAnObj(sel_ptr->obj);
      } else {
         AbortPrepareCmd(CMD_REPLACE);
      }
   }
   EndCompositeCmd();

   if (changed) {
      SetFileModified(TRUE);
      ltx = selLtX; lty = selLtY; rbx = selRbX; rby = selRbY;
      UpdSelBBox();
      RedrawAreas(botObj, ltx-GRID_ABS_SIZE(1), lty-GRID_ABS_SIZE(1),
            rbx+GRID_ABS_SIZE(1), rby+GRID_ABS_SIZE(1), selLtX-GRID_ABS_SIZE(1),
            selLtY-GRID_ABS_SIZE(1), selRbX+GRID_ABS_SIZE(1),
            selRbY+GRID_ABS_SIZE(1));
   }
   if (HighLight) HighLightForward();
}

void RefreshLineStyleMenu(menu)
   TgMenu *menu;
{
   int i, num_items=menu->num_items;
   TgMenuItem *menuitems=menu->menuitems;

   for (i=0; i < num_items; i++) {
      TgMenuItem *menu_item=(&menuitems[i]);
      TgMenuItem stMenuItem;

      memset(&stMenuItem, 0, sizeof(TgMenuItem));
      stMenuItem.state = TGBS_NORMAL;
      stMenuItem.checked = (i == lineStyle);
      TgSetMenuItemInfo(menu_item, TGMU_MASK_STATE|TGMU_MASK_CHECK,
            &stMenuItem);
   }
}

TgMenu *CreateLineStyleMenu(parent_menu, x, y, menu_info, status_str_xlated)
   TgMenu *parent_menu;
   int x, y;
   TgMenuInfo *menu_info;
   int status_str_xlated; /* ignored, always 0 */
{
   TgMenu *menu=TgCreateMenuFromMenuInfo(parent_menu, x, y, menu_info, FALSE);

   if (menu != NULL) {
      TgMenuItem *menu_item=NULL;
      TgMenuItem stMenuItem;

      menu->track_menubar = TRUE;
      TgAdjustMenuGeometry(menu, menuImageW, menuImageH, MAXLINESTYLES);
      menu_item = (&menu->menuitems[lineStyle]);

      memset(&stMenuItem, 0, sizeof(TgMenuItem));
      stMenuItem.checked = TRUE;
      if (!TgSetMenuItemInfo(menu_item, TGMU_MASK_CHECK, &stMenuItem)) {
         return TgDestroyMenu(menu, TRUE);
      }
   }
   return menu;
}

int LineStyleMenu(X, Y, TrackMenubar)
   int X, Y, TrackMenubar;
{
   int rc=INVALID;
   TgMenu *menu=(lineStyleMenuInfo.create_proc)(NULL, X, Y, &lineStyleMenuInfo,
         INVALID);

   activeMenu = MENU_LINESTYLE;
   if (menu != NULL) {
      menu->track_menubar = TrackMenubar;

      rc = TgMenuLoop(menu);
      TgDestroyMenu(menu, TRUE);
   }
   return rc;
}

/* ------------------ LineType Functions ------------------ */

int GetPolyOrPolygonControlPoints(obj_ptr, pp_poly, pp_polygon, pn_curved,
      pn_num_pts, pp_ip, ppsz_smooth)
   struct ObjRec *obj_ptr;
   struct PolyRec **pp_poly;
   struct PolygonRec **pp_polygon;
   int *pn_curved, *pn_num_pts;
   IntPoint **pp_ip;
   char **ppsz_smooth;
{
   struct PolyRec *poly_ptr=NULL;
   struct PolygonRec *polygon_ptr=NULL;

   switch (obj_ptr->type) {
   case OBJ_POLY:
      poly_ptr = obj_ptr->detail.p;
      *pn_curved = poly_ptr->curved;
      if ((*pn_curved) == LT_STRUCT_SPLINE) {
         *pn_num_pts = poly_ptr->ssn;
         *pp_ip = poly_ptr->ssvlist;
         *ppsz_smooth = poly_ptr->ssmooth;
      } else {
         *pn_num_pts = poly_ptr->n;
         *pp_ip = poly_ptr->vlist;
         *ppsz_smooth = poly_ptr->smooth;
      }
      break;
   case OBJ_POLYGON:
      polygon_ptr = obj_ptr->detail.g;
      *pn_curved = polygon_ptr->curved;
      if ((*pn_curved) == LT_STRUCT_SPLINE) {
         *pn_num_pts = polygon_ptr->ssn;
         *pp_ip = polygon_ptr->ssvlist;
         *ppsz_smooth = polygon_ptr->ssmooth;
      } else {
         *pn_num_pts = polygon_ptr->n;
         *pp_ip = polygon_ptr->vlist;
         *ppsz_smooth = polygon_ptr->smooth;
      }
      break;
   default: return FALSE;
   }
   if (pp_poly != NULL) *pp_poly = poly_ptr;
   if (pp_polygon != NULL) *pp_polygon = polygon_ptr;

   return TRUE;
}

static
int CanConvertSplineToStructuredSpline(ObjPtr, pn_hinge_count, vlist_updated)
   struct ObjRec *ObjPtr;
   int *pn_hinge_count;
   IntPoint *vlist_updated;
   /*
    * returns TRUE means that conversion will be made (and something
    *       will change)
    */
{
   int i=0, last_hinge_index=0, hinge_count=1, num_ss_pts=0;
   struct PolyRec *poly_ptr=NULL;
   int curved=(-1), n=0, earlier_smooth=FALSE;
   IntPoint *vs=NULL;
   char *smooth=NULL;

   if (!GetPolyOrPolygonControlPoints(ObjPtr, &poly_ptr, NULL, &curved, &n, &vs,
         &smooth)) {
      return FALSE;
   }
   if (vlist_updated != NULL) {
      num_ss_pts = ((*pn_hinge_count)*3)-2;
      vlist_updated[0].x = vs[0].x;
      vlist_updated[0].y = vs[0].y;
      vlist_updated[num_ss_pts-1].x = vs[n-1].x;
      vlist_updated[num_ss_pts-1].y = vs[n-1].y;
   }
   for (i=1; i < n; i++) {
      if (smooth == NULL || !smooth[i]) {
         int idx=0;

         hinge_count++;

         switch (i-last_hinge_index) {
         case 3:
            if (last_hinge_index == 0) {
               /* the first point can be anything, so still okay */
               if (vlist_updated != NULL) {
                  vlist_updated[1].x = vs[1].x;
                  vlist_updated[1].y = vs[1].y;
               }
            } else if (earlier_smooth) {
               if ((vs[last_hinge_index].x<<1) ==
                     (vs[last_hinge_index-1].x+vs[last_hinge_index+1].x) &&
                     (vs[last_hinge_index].y<<1) ==
                     (vs[last_hinge_index-1].y+vs[last_hinge_index+1].y)) {
                  /* still okay */
                  if (vlist_updated != NULL) {
                     idx = (hinge_count-2)*3;
#ifdef _TGIF_DBG /* debug, do not translate */
                     TgAssert(
                           vlist_updated[idx-1].x==vs[last_hinge_index-1].x &&
                           vlist_updated[idx-1].y==vs[last_hinge_index-1].y &&
                           vlist_updated[idx].x==vs[last_hinge_index].x &&
                           vlist_updated[idx].y==vs[last_hinge_index].y,
                           "check #1 failed in CanConvertSplineToStructuredSpline()",
                           NULL);
#endif /* _TGIF_DBG */
                     vlist_updated[idx+1].x = vs[last_hinge_index+1].x;
                     vlist_updated[idx+1].y = vs[last_hinge_index+1].y;
                  }
               } else {
                  return FALSE;
               }
            } else {
               return FALSE;
            }
            earlier_smooth = TRUE;
            if (vlist_updated != NULL) {
               idx = (hinge_count-1)*3;
               vlist_updated[idx-1].x = vs[i-1].x;
               vlist_updated[idx-1].y = vs[i-1].y;
               vlist_updated[idx].x = vs[i].x;
               vlist_updated[idx].y = vs[i].y;
            }
            break;
         case 2:
            if (last_hinge_index == 0) {
               /* it can go either way */
               if (i == n-1) {
                  /* still can go either way */
                  earlier_smooth = TRUE;
                  if (vlist_updated != NULL) {
                     vlist_updated[1].x = vs[0].x;
                     vlist_updated[1].y = vs[0].y;
                  }
               } else if (smooth != NULL && smooth[i+1]) {
                  if ((vs[i].x<<1) == (vs[i-1].x+vs[i+1].x) &&
                        (vs[i].y<<1) == (vs[i-1].y+vs[i+1].y)) {
                     /* still okay */
                     earlier_smooth = TRUE;
                     if (vlist_updated != NULL) {
                        vlist_updated[1].x = vs[0].x;
                        vlist_updated[1].y = vs[0].y;
                     }
                  } else {
                     /* still okay */
                     earlier_smooth = FALSE;
                  }
               } else {
                  /* still okay */
                  earlier_smooth = FALSE;
               }
               if (vlist_updated != NULL) {
                  idx = (hinge_count-2)*3;
                  if (!earlier_smooth) {
                     vlist_updated[idx+1].x = vs[i-1].x;
                     vlist_updated[idx+1].y = vs[i-1].y;
                  }
               }
            } else if (earlier_smooth) {
               if ((vs[last_hinge_index].x<<1) ==
                     (vs[last_hinge_index-1].x+vs[last_hinge_index+1].x) &&
                     (vs[last_hinge_index].y<<1) ==
                     (vs[last_hinge_index-1].y+vs[last_hinge_index+1].y)) {
                  /* still okay */
                  earlier_smooth = FALSE;
                  if (vlist_updated != NULL) {
                     idx = (hinge_count-2)*3;
#ifdef _TGIF_DBG /* debug, do not translate */
                     TgAssert(
                           vlist_updated[idx-1].x==vs[last_hinge_index-1].x &&
                           vlist_updated[idx-1].y==vs[last_hinge_index-1].y &&
                           vlist_updated[idx].x==vs[last_hinge_index].x &&
                           vlist_updated[idx].y==vs[last_hinge_index].y,
                           "check #2 failed in CanConvertSplineToStructuredSpline()",
                           NULL);
#endif /* _TGIF_DBG */
                     vlist_updated[idx+1].x = vs[last_hinge_index+1].x;
                     vlist_updated[idx+1].y = vs[last_hinge_index+1].y;
                  }
               } else {
                  return FALSE;
               }
            } else {
               /* still okay */
               if (vlist_updated != NULL) {
                  idx = (hinge_count-2)*3;
                  vlist_updated[idx+1].x = vlist_updated[idx].x;
                  vlist_updated[idx+1].y = vlist_updated[idx].y;
               }
               earlier_smooth = TRUE;
            }
            if (vlist_updated != NULL) {
               idx = (hinge_count-1)*3;
               if (earlier_smooth) {
                  vlist_updated[idx-1].x = vs[i-1].x;
                  vlist_updated[idx-1].y = vs[i-1].y;
               } else {
                  vlist_updated[idx-1].x = vs[i].x;
                  vlist_updated[idx-1].y = vs[i].y;
               }
               vlist_updated[idx].x = vs[i].x;
               vlist_updated[idx].y = vs[i].y;
            }
            break;
         case 1:
            if (earlier_smooth) {
               return FALSE;
            }
            if (vlist_updated != NULL) {
               idx = (hinge_count-2)*3;
               vlist_updated[idx+1].x = vs[last_hinge_index].x;
               vlist_updated[idx+1].y = vs[last_hinge_index].y;

               idx = (hinge_count-1)*3;
               vlist_updated[idx-1].x = vs[i].x;
               vlist_updated[idx-1].y = vs[i].y;
               vlist_updated[idx].x = vs[i].x;
               vlist_updated[idx].y = vs[i].y;
            }
            break;
         default: /* too many consecutive smooth points */ return FALSE;
         }
         last_hinge_index = i;
      }
   }
   if (pn_hinge_count != NULL && vlist_updated == NULL) {
      *pn_hinge_count = hinge_count;
   }
   return TRUE;
}

static
int CanConvertClosedSplineToStructuredSpline(ObjPtr, pn_hinge_count,
      vlist_updated)
   struct ObjRec *ObjPtr;
   int *pn_hinge_count;
   IntPoint *vlist_updated;
   /*
    * returns TRUE means that conversion will be made (and something
    *       will change)
    */
{
   int i=0, last_hinge_index=0, hinge_count=1, num_ss_pts=0;
   struct PolygonRec *polygon_ptr=NULL;
   int curved=(-1), n=0, earlier_smooth=FALSE;
   IntPoint *vs=NULL;
   char *smooth=NULL;

   if (!GetPolyOrPolygonControlPoints(ObjPtr, NULL, &polygon_ptr, &curved, &n,
         &vs, &smooth)) {
      return FALSE;
   }
   if (vlist_updated != NULL) {
      num_ss_pts = ((*pn_hinge_count)*3)-2;
      vlist_updated[0].x = vs[0].x;
      vlist_updated[0].y = vs[0].y;
      vlist_updated[num_ss_pts-1].x = vs[n-1].x;
      vlist_updated[num_ss_pts-1].y = vs[n-1].y;
   }
   for (i=1; i < n; i++) {
      if (smooth == NULL || !smooth[i]) {
         int idx=0;

         hinge_count++;

         switch (i-last_hinge_index) {
         case 3:
            if (last_hinge_index == 0) {
               if (smooth != NULL && smooth[n-2]) {
                  if ((vs[0].x<<1) == (vs[n-2].x+vs[1].x) &&
                        (vs[0].y<<1) == (vs[n-2].y+vs[1].y)) {
                     /* still okay */
                     if (vlist_updated != NULL) {
                        idx = (*pn_hinge_count)*3-3;
                        vlist_updated[1].x = vs[1].x;
                        vlist_updated[1].y = vs[1].y;
                        vlist_updated[idx-1].x = vs[n-2].x;
                        vlist_updated[idx-1].y = vs[n-2].y;
                     }
                  } else {
                     return FALSE;
                  }
               } else {
                  return FALSE;
               }
            } else if (earlier_smooth) {
               if ((vs[last_hinge_index].x<<1) ==
                     (vs[last_hinge_index-1].x+vs[last_hinge_index+1].x) &&
                     (vs[last_hinge_index].y<<1) ==
                     (vs[last_hinge_index-1].y+vs[last_hinge_index+1].y)) {
                  /* still okay */
                  if (vlist_updated != NULL) {
                     idx = (hinge_count-2)*3;
#ifdef _TGIF_DBG /* debug, do not translate */
                     TgAssert(
                           vlist_updated[idx-1].x==vs[last_hinge_index-1].x &&
                           vlist_updated[idx-1].y==vs[last_hinge_index-1].y &&
                           vlist_updated[idx].x==vs[last_hinge_index].x &&
                           vlist_updated[idx].y==vs[last_hinge_index].y,
                           "check #1 failed in CanConvertClosedSplineToStructuredSpline()",
                           NULL);
#endif /* _TGIF_DBG */
                     vlist_updated[idx+1].x = vs[last_hinge_index+1].x;
                     vlist_updated[idx+1].y = vs[last_hinge_index+1].y;
                  }
               } else {
                  return FALSE;
               }
            } else {
               return FALSE;
            }
            earlier_smooth = TRUE;
            if (vlist_updated != NULL) {
               idx = (hinge_count-1)*3;
               vlist_updated[idx-1].x = vs[i-1].x;
               vlist_updated[idx-1].y = vs[i-1].y;
               vlist_updated[idx].x = vs[i].x;
               vlist_updated[idx].y = vs[i].y;
            }
            if (i == n-1) {
               if (smooth != NULL && smooth[1]) {
                  if ((vs[0].x<<1) == (vs[n-2].x+vs[1].x) &&
                        (vs[0].y<<1) == (vs[n-2].y+vs[1].y)) {
                     /* still okay */
                  } else {
                     return FALSE;
                  }
               } else {
                  return FALSE;
               }
            }
            break;
         case 2:
            if (last_hinge_index == 0) {
               /* it can go either way */
               if (smooth != NULL && smooth[i+1]) {
                  if ((vs[i].x<<1) == (vs[i-1].x+vs[i+1].x) &&
                        (vs[i].y<<1) == (vs[i-1].y+vs[i+1].y)) {
                     /* still okay */
                     earlier_smooth = TRUE;
                     if (vlist_updated != NULL) {
                        vlist_updated[1].x = vs[0].x;
                        vlist_updated[1].y = vs[0].y;

                        idx = (hinge_count-1)*3;
                        vlist_updated[idx-1].x = vs[i-1].x;
                        vlist_updated[idx-1].y = vs[i-1].y;
                        vlist_updated[idx].x = vs[i].x;
                        vlist_updated[idx].y = vs[i].y;
                     }
                  } else {
                     /* still okay */
                     if (smooth != NULL && smooth[n-2]) {
                        if ((vs[0].x<<1) == (vs[n-2].x+vs[1].x) &&
                              (vs[0].y<<1) == (vs[n-2].y+vs[1].y)) {
                           /* still okay */
                        } else {
                           return FALSE;
                        }
                     } else {
                        return FALSE;
                     }
                     earlier_smooth = FALSE;
                  }
               } else {
                  /* still okay */
                  earlier_smooth = FALSE;
               }
            } else if (earlier_smooth) {
               if ((vs[last_hinge_index].x<<1) ==
                     (vs[last_hinge_index-1].x+vs[last_hinge_index+1].x) &&
                     (vs[last_hinge_index].y<<1) ==
                     (vs[last_hinge_index-1].y+vs[last_hinge_index+1].y)) {
                  /* still okay */
                  earlier_smooth = FALSE;
               } else {
                  return FALSE;
               }
            } else {
               /* still okay */
               earlier_smooth = TRUE;
               if (i == n-1) {
                  if (smooth != NULL && smooth[1]) {
                     if ((vs[0].x<<1) == (vs[n-2].x+vs[1].x) &&
                           (vs[0].y<<1) == (vs[n-2].y+vs[1].y)) {
                        /* still okay */
                     } else {
                        return FALSE;
                     }
                  } else {
                     return FALSE;
                  }
               }
            }
            if (vlist_updated != NULL) {
               idx = (hinge_count-1)*3;
               if (earlier_smooth) {
                  vlist_updated[idx-1].x = vs[i-1].x;
                  vlist_updated[idx-1].y = vs[i-1].y;
               } else {
                  vlist_updated[idx-1].x = vs[i].x;
                  vlist_updated[idx-1].y = vs[i].y;
               }
               vlist_updated[idx].x = vs[i].x;
               vlist_updated[idx].y = vs[i].y;
            }
            break;
         case 1:
            if (earlier_smooth) {
               return FALSE;
            }
            if (vlist_updated != NULL) {
               idx = (hinge_count-2)*3;
               vlist_updated[idx+1].x = vs[last_hinge_index].x;
               vlist_updated[idx+1].y = vs[last_hinge_index].y;

               idx = (hinge_count-1)*3;
               vlist_updated[idx-1].x = vs[i].x;
               vlist_updated[idx-1].y = vs[i].y;
               vlist_updated[idx].x = vs[i].x;
               vlist_updated[idx].y = vs[i].y;
            }
            break;
         default: /* too many consecutive smooth points */ return FALSE;
         }
         last_hinge_index = i;
      }
   }
   if (last_hinge_index == 0) {
      return FALSE;
   }
   if (pn_hinge_count != NULL && vlist_updated == NULL) {
      *pn_hinge_count = hinge_count;
   }
   return TRUE;
}

static
int CanConvertObjLineType(ObjPtr, TypeIndex, pn_failed_count, pn_hinge_count)
   struct ObjRec *ObjPtr;
   int TypeIndex, *pn_failed_count, *pn_hinge_count;
   /*
    * returns FALSE if ObjPtr has the same line type as TypeIndex or
    *       if there is no way to make the conversion (for example,
    *       from spline to structured spline where the smooth vertices
    *       are not all at the right places); this object will not change
    *       in this case
    * returns TRUE means that conversion will be made (and something
    *       will change)
    */
{
   int i=0;
   int curved=(-1), n=0;
   IntPoint *vs=NULL;
   char *smooth=NULL;

   if (!GetPolyOrPolygonControlPoints(ObjPtr, NULL, NULL, &curved, &n, &vs,
         &smooth)) {
      return FALSE;
   }
   switch (TypeIndex) {
   case LT_STRAIGHT:
      if (curved == LT_INTSPLINE) return TRUE;
      if (curved == LT_STRUCT_SPLINE) return TRUE;
      if (curved == LT_STRAIGHT && smooth == NULL) return FALSE;
      if (curved == LT_SPLINE && smooth == NULL) return TRUE;
      switch (ObjPtr->type) {
      case OBJ_POLY:
         for (i=1; i < n-1; i++) if (smooth[i]) return TRUE;
         break;
      case OBJ_POLYGON:
         for (i=0; i < n; i++) if (smooth[i]) return TRUE;
         break;
      }
      return FALSE;
   case LT_SPLINE:
      if (curved == LT_INTSPLINE) return TRUE;
      if (curved == LT_STRUCT_SPLINE) return TRUE;
      if (curved == LT_STRAIGHT && smooth == NULL) return TRUE;
      if (curved == LT_SPLINE && smooth == NULL) return FALSE;
      switch (ObjPtr->type) {
      case OBJ_POLY:
         for (i=1; i < n-1; i++) if (!smooth[i]) return TRUE;
         break;
      case OBJ_POLYGON:
         for (i=0; i < n; i++) if (!smooth[i]) return TRUE;
         break;
      }
      return FALSE;
   case LT_INTSPLINE:
      if (curved == LT_INTSPLINE) return FALSE;
      if (curved == LT_STRUCT_SPLINE) {
         int min_num_vertices=0, max_vs_to_check=n, count=1;

         if (ObjPtr->type == OBJ_POLY) {
            min_num_vertices = 2;
         } else { /* ObjPtr->type is OBJ_POLYGON */
            min_num_vertices = 4;
         }
         if (smooth == NULL) {
            if (n < min_num_vertices) {
               (*pn_failed_count)++;
               return FALSE;
            }
         } else {
            for (i=1; i < max_vs_to_check; i++) {
               if (!smooth[i]) count++;
            }
            if (count < min_num_vertices) {
               (*pn_failed_count)++;
               return FALSE;
            }
         }
      }
      return TRUE;
   case LT_STRUCT_SPLINE:
      if (curved == LT_INTSPLINE) {
         if (pn_hinge_count != NULL) *pn_hinge_count = n;
         return TRUE;
      }
      if (curved == LT_STRUCT_SPLINE) return FALSE;
      if (curved == LT_STRAIGHT && smooth == NULL) {
         if (pn_hinge_count != NULL) *pn_hinge_count = n;
         return TRUE;
      }
      if (curved == LT_SPLINE && smooth == NULL) return FALSE;
      switch (ObjPtr->type) {
      case OBJ_POLY:
         if (!CanConvertSplineToStructuredSpline(ObjPtr, pn_hinge_count, 
               NULL)) {
            (*pn_failed_count)++;
            return FALSE;
         }
         break;
      case OBJ_POLYGON:
         if (!CanConvertClosedSplineToStructuredSpline(ObjPtr, pn_hinge_count,
               NULL)) {
            (*pn_failed_count)++;
            return FALSE;
         }
         break;
      }
      break;
   }
   return TRUE;
}

static
IntPoint *GetNumUsableStraightVs(pn_num_usable_vs, num_pts, vs)
   int *pn_num_usable_vs, num_pts;
   IntPoint *vs;
{
   int i=0, index=1, x=0, y=0;
   IntPoint *pv=(IntPoint*)malloc((num_pts+1)*sizeof(IntPoint));

   if (pv == NULL) FailAllocMessage();
   memset(pv, 0, num_pts*sizeof(IntPoint));
   x = pv[0].x = vs[0].x;
   y = pv[0].y = vs[0].y;
   for (i=1; i < num_pts; i++) {
      if (vs[i].x != x || vs[i].y != y) {
         x = pv[index].x = vs[i].x;
         y = pv[index].y = vs[i].y;
         index++;
      }
   }
   if (index == 1) {
      *pn_num_usable_vs = 0;
      free(pv);
      return NULL;
   }
   *pn_num_usable_vs = index;

   return pv;
}

static
IntPoint *GetNumUsableSplineVs(pn_num_usable_vs, ppsz_smooth, num_pts, vs,
      smooth)
   int *pn_num_usable_vs, num_pts;
   char **ppsz_smooth, *smooth;
   IntPoint *vs;
{
   int i=0, index=1, x=0, y=0;
   IntPoint *pv=(IntPoint*)malloc((num_pts+1)*sizeof(IntPoint));
   char *psz_smooth=(char*)malloc((num_pts+1)*sizeof(char));

   if (pv == NULL || psz_smooth == NULL) FailAllocMessage();
   memset(pv, 0, num_pts*sizeof(IntPoint));
   memset(psz_smooth, 0, num_pts*sizeof(char));
   x = pv[0].x = vs[0].x;
   y = pv[0].y = vs[0].y;
   psz_smooth[0] = FALSE;
   for (i=1; i < num_pts; i++) {
      if (vs[i].x != x || vs[i].y != y) {
         psz_smooth[index] = smooth[i];
         x = pv[index].x = vs[i].x;
         y = pv[index].y = vs[i].y;
         index++;
      }
   }
   psz_smooth[index] = FALSE;

   if (index == 1) {
      *pn_num_usable_vs = 0;
      *ppsz_smooth = NULL;
      free(pv);
      free(psz_smooth);
      return NULL;
   }
   *pn_num_usable_vs = index;
   *ppsz_smooth = psz_smooth;

   return pv;
}

static
IntPoint *GetNumUsableIntSplineVsFromStructuredSpline(pn_num_usable_vs, num_pts,
      vs)
   int *pn_num_usable_vs, num_pts;
   IntPoint *vs;
{
   int j=0, index=1, x=0, y=0;
   IntPoint *pv=(IntPoint*)malloc((num_pts+1)*sizeof(IntPoint));

   if (pv == NULL) FailAllocMessage();
   memset(pv, 0, num_pts*sizeof(IntPoint));
   x = pv[0].x = vs[0].x;
   y = pv[0].y = vs[0].y;
   for (j=3; j < num_pts; j+=3) {
      if (abs(vs[j].x-x) > 1 || abs(vs[j].y-y) > 1) {
         x = pv[index].x = vs[j].x;
         y = pv[index].y = vs[j].y;
         index++;
      }
   }
   if (index == 1) {
      *pn_num_usable_vs = 0;
      free(pv);
      return NULL;
   }
   *pn_num_usable_vs = index;

   return pv;
}

static
IntPoint *GetNumUsableIntSplineVs(pn_num_usable_vs, num_pts, vs, smooth)
   int *pn_num_usable_vs, num_pts;
   IntPoint *vs;
   char *smooth;
{
   int i=0, index=1, x=0, y=0;
   IntPoint *pv=(IntPoint*)malloc((num_pts+1)*sizeof(IntPoint));

   if (pv == NULL) FailAllocMessage();
   memset(pv, 0, num_pts*sizeof(IntPoint));
   x = pv[0].x = vs[0].x;
   y = pv[0].y = vs[0].y;
   for (i=1; i < num_pts; i++) {
      if (abs(vs[i].x-x) > 1 || abs(vs[i].y-y) > 1) {
         x = pv[index].x = vs[i].x;
         y = pv[index].y = vs[i].y;
         index++;
      }
   }
   if (index == 1) {
      *pn_num_usable_vs = 0;
      free(pv);
      return NULL;
   }
   *pn_num_usable_vs = index;

   return pv;
}

static
IntPoint *GetNumUsableStructuredSplineVs(pn_num_usable_vs, ppsz_smooth,
      obj_type, n, vs, intn, intvs)
   int *pn_num_usable_vs, obj_type, n, intn;
   char **ppsz_smooth;
   IntPoint *vs, *intvs;
{
   int i=0, j=0, k=0, index=1, x=0, y=0, num_ss_pts=(n*3)-2;
   IntPoint *pv=(IntPoint*)malloc((num_ss_pts+2)*sizeof(IntPoint));
   char *psz_smooth=(char*)malloc((num_ss_pts+2)*sizeof(char));

   if (pv == NULL || psz_smooth == NULL) FailAllocMessage();
   memset(pv, 0, (num_ss_pts+1)*sizeof(IntPoint));
   memset(psz_smooth, 0, (num_ss_pts+1)*sizeof(char));
   x = pv[0].x = vs[0].x;
   y = pv[0].y = vs[0].y;
   psz_smooth[0] = psz_smooth[num_ss_pts-1] = FALSE;
   if (obj_type == OBJ_POLY) {
      pv[1].x = vs[0].x;
      pv[1].y = vs[0].y;
      psz_smooth[1] = psz_smooth[num_ss_pts-2] = FALSE;
   } else { /* obj_type == OBJ_POLYGON */
      pv[1].x = intvs[0].x;
      pv[1].y = intvs[0].y;
      psz_smooth[1] = psz_smooth[num_ss_pts-2] = TRUE;
   }
   for (i=1; i < n; i++) {
      if (abs(vs[i].x-x) > 3 || abs(vs[i].y-y) > 3) {
         j = (index*3);
         k = (i<<1)-1;
         pv[j-1].x = intvs[k].x;
         pv[j-1].y = intvs[k].y;
         x = pv[j].x = vs[i].x;
         y = pv[j].y = vs[i].y;
         pv[j+1].x = (x<<1) - pv[j-1].x;
         pv[j+1].y = (y<<1) - pv[j-1].y;
         if (pv[j-1].x == pv[j+1].x && pv[j-1].y == pv[j+1].y) {
            psz_smooth[j-1] = psz_smooth[j+1] = FALSE;
         } else {
            psz_smooth[j-1] = psz_smooth[j+1] = TRUE;
         }
         psz_smooth[j] = FALSE;
         index++;
      }
   }
   if (index == 1) {
      *pn_num_usable_vs = 0;
      *ppsz_smooth = NULL;
      free(pv);
      free(psz_smooth);
      return NULL;
   }
   j = (index-1)*3;
   pv[j].x = x;
   pv[j].y = y;
   psz_smooth[j] = FALSE;
   if (obj_type == OBJ_POLY) {
      pv[j-1].x = x;
      pv[j-1].y = y;
      psz_smooth[j-1] = FALSE;
   } else { /* obj_type == OBJ_POLYGON */
      if (x != pv[0].x || y != pv[0].y) {
         /*
          * need to add more vertices?
          * let's assume this cannot happen for now 
          */
         if (index == n) {
            x = pv[j].x = pv[0].x;
            y = pv[j].y = pv[0].y;
         } else {
            psz_smooth[j-1] = psz_smooth[j] = psz_smooth[j+1] = FALSE;
            pv[j-1].x = pv[j+1].x = pv[j].x;
            pv[j-1].y = pv[j+1].y = pv[j].y;
            j += 3;
            x = pv[j-1].x = pv[j].x = pv[0].x;
            y = pv[j-1].y = pv[j].y = pv[0].y;
            psz_smooth[j-1] = psz_smooth[j] = FALSE;
         }
      } else {
         pv[j-1].x = (x<<1) - pv[1].x;
         pv[j-1].y = (y<<1) - pv[1].y;
         psz_smooth[j-1] = TRUE;
      }
   }
   *pn_num_usable_vs = j+1;
   *ppsz_smooth = psz_smooth;

   return pv;
}

static
void SetObjSmooth(ObjPtr, TypeIndex, hinge_count)
   struct ObjRec *ObjPtr;
   int TypeIndex, hinge_count;
{
   int i=0, n=0, num_ss_pts=0;
   char *smooth=NULL, *usable_smooth=NULL;
   struct PolyRec *poly_ptr=NULL;
   struct PolygonRec *polygon_ptr=NULL;
   IntPoint *vlist=NULL, *usable_vlist=NULL;

   switch (ObjPtr->type) {
   case OBJ_POLY:
      poly_ptr = ObjPtr->detail.p;
      smooth = poly_ptr->smooth;
      n = poly_ptr->n;
      switch (TypeIndex) {
      case LT_STRAIGHT:
         if (poly_ptr->curved == LT_STRUCT_SPLINE) {
            usable_vlist = GetNumUsableStraightVs(&n, poly_ptr->ssn,
                  poly_ptr->ssvlist);
         }
         if (usable_vlist != NULL) {
            if (poly_ptr->vlist != NULL) free(poly_ptr->vlist);
            poly_ptr->n = n;
            poly_ptr->vlist = usable_vlist;
         }
         if (smooth == NULL) smooth = (char*)malloc((n+1)*sizeof(char));
         if (smooth == NULL) FailAllocMessage();
         for (i=0; i < n; i++) smooth[i] = FALSE;
         poly_ptr->smooth = smooth;
         return;
      case LT_SPLINE:
         if (poly_ptr->curved == LT_STRUCT_SPLINE) {
            usable_vlist = GetNumUsableSplineVs(&n, &usable_smooth,
                  poly_ptr->ssn, poly_ptr->ssvlist, poly_ptr->ssmooth);
         }
         if (usable_vlist != NULL) {
            if (poly_ptr->vlist != NULL) free(poly_ptr->vlist);
            if (poly_ptr->smooth != NULL) free(poly_ptr->smooth);
            poly_ptr->n = n;
            poly_ptr->vlist = usable_vlist;
            poly_ptr->smooth = usable_smooth;
         } else {
            if (smooth == NULL) smooth = (char*)malloc((n+1)*sizeof(char));
            if (smooth == NULL) FailAllocMessage();
            smooth[0] = smooth[n-1] = FALSE;
            for (i=1; i < n-1; i++) smooth[i] = TRUE;
            poly_ptr->smooth = smooth;
         }
         return;
      case LT_INTSPLINE:
         if (poly_ptr->curved == LT_STRUCT_SPLINE) {
            usable_vlist = GetNumUsableIntSplineVsFromStructuredSpline(&n,
                  poly_ptr->n, poly_ptr->vlist);
         } else {
            usable_vlist = GetNumUsableIntSplineVs(&n, poly_ptr->n,
                  poly_ptr->vlist, poly_ptr->smooth);
         }
         if (usable_vlist != NULL) {
            if (poly_ptr->vlist != NULL) free(poly_ptr->vlist);
            poly_ptr->n = n;
            poly_ptr->vlist = usable_vlist;
         }
         if (smooth != NULL) {
            free(smooth);
            poly_ptr->smooth = NULL;
         }
         return;
      case LT_STRUCT_SPLINE:
         if (poly_ptr->curved == LT_INTSPLINE) {
            usable_vlist = GetNumUsableStructuredSplineVs(&n, &usable_smooth,
                  OBJ_POLY, poly_ptr->n, poly_ptr->vlist, poly_ptr->intn,
                  poly_ptr->intvlist);
         }
         if (usable_vlist != NULL) {
            if (poly_ptr->vlist != NULL) free(poly_ptr->vlist);
            poly_ptr->n = n;
            poly_ptr->vlist = usable_vlist;
            if (poly_ptr->smooth != NULL) free(poly_ptr->smooth);
            poly_ptr->smooth = usable_smooth;
         } else {
            num_ss_pts = (hinge_count*3)-2;
            vlist = (IntPoint*)malloc((num_ss_pts+1)*sizeof(IntPoint));
            if (vlist == NULL) FailAllocMessage();
            memset(vlist, 0, (num_ss_pts+1)*sizeof(IntPoint));
            if (!CanConvertSplineToStructuredSpline(ObjPtr, &hinge_count,
                  vlist)) {
#ifdef _TGIF_DBG /* debug, do not translate */
               TgAssert(FALSE, "conversion failed in SetObjSmooth()", NULL);
#endif /* _TGIF_DBG */
               return;
            }
            if (smooth != NULL) free(smooth);
            free(poly_ptr->vlist);
            poly_ptr->vlist = vlist;
            poly_ptr->n = num_ss_pts;
            poly_ptr->smooth = NULL;
         }
         return;
      }
      break;
   case OBJ_POLYGON:
      polygon_ptr = ObjPtr->detail.g;
      smooth = polygon_ptr->smooth;
      n = polygon_ptr->n;
      switch (TypeIndex) {
      case LT_STRAIGHT:
         if (polygon_ptr->curved == LT_STRUCT_SPLINE) {
            usable_vlist = GetNumUsableStraightVs(&n, polygon_ptr->ssn,
                  polygon_ptr->ssvlist);
         }
         if (usable_vlist != NULL) {
            if (polygon_ptr->vlist != NULL) free(polygon_ptr->vlist);
            polygon_ptr->n = n;
            polygon_ptr->vlist = usable_vlist;
         }
         if (smooth == NULL) smooth = (char*)malloc((n+1)*sizeof(char));
         if (smooth == NULL) FailAllocMessage();
         for (i=0; i < n; i++) smooth[i] = FALSE;
         polygon_ptr->smooth = smooth;
         return;
      case LT_SPLINE:
         if (polygon_ptr->curved == LT_STRUCT_SPLINE) {
            usable_vlist = GetNumUsableSplineVs(&n, &usable_smooth,
                  polygon_ptr->ssn, polygon_ptr->ssvlist, polygon_ptr->ssmooth);
         }
         if (usable_vlist != NULL) {
            if (polygon_ptr->vlist != NULL) free(polygon_ptr->vlist);
            if (polygon_ptr->smooth != NULL) free(polygon_ptr->smooth);
            polygon_ptr->n = n;
            polygon_ptr->vlist = usable_vlist;
            polygon_ptr->smooth = usable_smooth;
         } else {
            if (smooth == NULL) smooth = (char*)malloc((n+1)*sizeof(char));
            if (smooth == NULL) FailAllocMessage();
            for (i=0; i < n; i++) smooth[i] = TRUE;
            polygon_ptr->smooth = smooth;
         }
         return;
      case LT_INTSPLINE:
         if (polygon_ptr->curved == LT_STRUCT_SPLINE) {
            usable_vlist = GetNumUsableIntSplineVsFromStructuredSpline(&n,
                  polygon_ptr->n, polygon_ptr->vlist);
         } else {
            usable_vlist = GetNumUsableIntSplineVs(&n, polygon_ptr->n,
                  polygon_ptr->vlist, polygon_ptr->smooth);
         }
         if (usable_vlist != NULL) {
            if (polygon_ptr->vlist != NULL) free(polygon_ptr->vlist);
            polygon_ptr->n = n;
            polygon_ptr->vlist = usable_vlist;
         }
         if (smooth != NULL) {
            free(smooth);
            polygon_ptr->smooth = NULL;
         }
         return;
      case LT_STRUCT_SPLINE:
         if (polygon_ptr->curved == LT_INTSPLINE) {
            usable_vlist = GetNumUsableStructuredSplineVs(&n, &usable_smooth,
                  OBJ_POLYGON, polygon_ptr->n, polygon_ptr->vlist,
                  polygon_ptr->intn, polygon_ptr->intvlist);
         }
         if (usable_vlist != NULL) {
            if (polygon_ptr->vlist != NULL) free(polygon_ptr->vlist);
            polygon_ptr->n = n;
            polygon_ptr->vlist = usable_vlist;
            if (polygon_ptr->smooth != NULL) free(polygon_ptr->smooth);
            polygon_ptr->smooth = usable_smooth;
         } else {
            num_ss_pts = (hinge_count*3)-2;
            vlist = (IntPoint*)malloc((num_ss_pts+2)*sizeof(IntPoint));
            if (vlist == NULL) FailAllocMessage();
            memset(vlist, 0, (num_ss_pts+2)*sizeof(IntPoint));
            if (!CanConvertSplineToStructuredSpline(ObjPtr, &hinge_count,
                  vlist)) {
#ifdef _TGIF_DBG /* debug, do not translate */
               TgAssert(FALSE, "conversion failed in SetObjSmooth()", NULL);
#endif /* _TGIF_DBG */
               return;
            }
            if (smooth != NULL) free(smooth);
            free(polygon_ptr->vlist);
            polygon_ptr->vlist = vlist;
            polygon_ptr->n = num_ss_pts;
            polygon_ptr->smooth = NULL;
         }
         return;
      }
      break;
   }
}

static
int ChangeObjLineType(ObjPtr, TypeIndex, pn_failed_count)
   struct ObjRec *ObjPtr;
   int TypeIndex, *pn_failed_count;
{
   struct ObjRec *obj_ptr=NULL;
   int changed=FALSE, hinge_count=0;

   switch (ObjPtr->type) {
   case OBJ_POLY:
      if (CanConvertObjLineType(ObjPtr, TypeIndex, pn_failed_count,
            &hinge_count)) {
         /* hinge_count is only valid if TypeIndex is LT_STRUCT_SPLINE */
         SetObjSmooth(ObjPtr, TypeIndex, hinge_count);
         ObjPtr->detail.p->curved = TypeIndex;
         changed = TRUE;
         AdjObjSplineVs(ObjPtr);
         if (ObjPtr->detail.p->curved != LT_INTSPLINE) {
            UpdPolyBBox(ObjPtr, ObjPtr->detail.p->n,
                  ObjPtr->detail.p->vlist);
         } else {
            UpdPolyBBox(ObjPtr, ObjPtr->detail.p->intn,
                  ObjPtr->detail.p->intvlist);
         }
      }
      break;
   case OBJ_POLYGON:
         /* hinge_count is only valid if TypeIndex is LT_STRUCT_SPLINE */
      if (CanConvertObjLineType(ObjPtr, TypeIndex, pn_failed_count,
            &hinge_count)) {
         SetObjSmooth(ObjPtr, TypeIndex, hinge_count);
         ObjPtr->detail.g->curved = TypeIndex;
         changed = TRUE;
         AdjObjSplineVs(ObjPtr);
         if (ObjPtr->detail.g->curved != LT_INTSPLINE) {
            UpdPolyBBox(ObjPtr, ObjPtr->detail.g->n,
                  ObjPtr->detail.g->vlist);
         } else {
            UpdPolyBBox(ObjPtr, ObjPtr->detail.g->intn,
                  ObjPtr->detail.g->intvlist);
         }
      }
      break;

   case OBJ_GROUP:
   case OBJ_SYM:
      for (obj_ptr=ObjPtr->detail.r->last; obj_ptr != NULL;
            obj_ptr=obj_ptr->prev) {
         if (ChangeObjLineType(obj_ptr, TypeIndex, pn_failed_count)) {
            changed = TRUE;
         }
      }
      break;
   }
   if (changed) AdjObjBBox(ObjPtr);
   return changed;
}

void ChangeAllSelLineType(TypeIndex, HighLight)
   int TypeIndex, HighLight;
{
   struct SelRec *sel_ptr=NULL;
   int ltx, lty, rbx, rby, changed=FALSE, dont_do_obj=FALSE, failed_count=0;

   if (topSel == NULL || stickyMenuSelection) {
      curSpline = TypeIndex;
      ShowLineType();
      UpdatePinnedMenu(MENU_LINETYPE);
      if (topSel == NULL) dont_do_obj = TRUE;
   }
   *gszMsgBox = '\0';
   switch (curSpline) {
   case LT_STRAIGHT:
      sprintf(gszMsgBox, TgLoadString(STID_LINE_TYPE_IS_STRAIGHT));
      break;
   case LT_SPLINE:
      sprintf(gszMsgBox, TgLoadString(STID_LINE_TYPE_IS_SPLINE));
      break;
   case LT_INTSPLINE:
      sprintf(gszMsgBox, TgLoadString(STID_LINE_TYPE_IS_INTSPLINE));
      break;
   case LT_STRUCT_SPLINE:
      sprintf(gszMsgBox, TgLoadString(STID_LINE_TYPE_IS_STRUCT_SPLINE));
      break;
   }
   Msg(gszMsgBox);
   if (dont_do_obj) return;

   if (HighLight) HighLightReverse();
   StartCompositeCmd();
   for (sel_ptr=botSel; sel_ptr != NULL; sel_ptr=sel_ptr->prev) {
      PrepareToReplaceAnObj(sel_ptr->obj);
      if (ChangeObjLineType(sel_ptr->obj, TypeIndex, &failed_count)) {
         changed = TRUE;
         RecordReplaceAnObj(sel_ptr->obj);
      } else {
         AbortPrepareCmd(CMD_REPLACE);
      }
   }
   EndCompositeCmd();

   if (changed) {
      SetFileModified(TRUE);
      ltx = selLtX; lty = selLtY; rbx = selRbX; rby = selRbY;
      UpdSelBBox();
      RedrawAreas(botObj, ltx-GRID_ABS_SIZE(1), lty-GRID_ABS_SIZE(1),
            rbx+GRID_ABS_SIZE(1), rby+GRID_ABS_SIZE(1), selLtX-GRID_ABS_SIZE(1),
            selLtY-GRID_ABS_SIZE(1), selRbX+GRID_ABS_SIZE(1),
            selRbY+GRID_ABS_SIZE(1));
   }
   if (HighLight) HighLightForward();
   if (failed_count > 0) {
      if (HighLight && failed_count == numObjSelected) {
         MsgBox(TgLoadString(STID_CANNOT_CONVERT_SEL_LINETYPE), TOOL_NAME,
               INFO_MB);
      } else {
         Msg(TgLoadString(STID_SOME_LT_FAILED_TO_CONVERT));
      }
   }
}

void RefreshLineTypeMenu(menu)
   TgMenu *menu;
{
   int i, num_items=menu->num_items;
   TgMenuItem *menuitems=menu->menuitems;

   for (i=0; i < num_items; i++) {
      TgMenuItem *menu_item=(&menuitems[i]);
      TgMenuItem stMenuItem;

      memset(&stMenuItem, 0, sizeof(TgMenuItem));
      stMenuItem.state = TGBS_NORMAL;
      stMenuItem.checked = (i == curSpline);
      TgSetMenuItemInfo(menu_item, TGMU_MASK_STATE|TGMU_MASK_CHECK,
            &stMenuItem);
   }
}

TgMenu *CreateLineTypeMenu(parent_menu, x, y, menu_info, status_str_xlated)
   TgMenu *parent_menu;
   int x, y;
   TgMenuInfo *menu_info;
   int status_str_xlated; /* ignored, always 0 */
{
   TgMenu *menu=TgCreateMenuFromMenuInfo(parent_menu, x, y, menu_info, FALSE);

   if (menu != NULL) {
      TgMenuItem *menu_item=NULL;
      TgMenuItem stMenuItem;

      menu->track_menubar = TRUE;
      TgAdjustMenuGeometry(menu, menuImageW, menuImageH, MAXLINETYPES);
      menu_item = (&menu->menuitems[curSpline]);

      memset(&stMenuItem, 0, sizeof(TgMenuItem));
      stMenuItem.checked = TRUE;
      if (!TgSetMenuItemInfo(menu_item, TGMU_MASK_CHECK, &stMenuItem)) {
         return TgDestroyMenu(menu, TRUE);
      }
   }
   return menu;
}

int LineTypeMenu(X, Y, TrackMenubar)
   int X, Y, TrackMenubar;
{
   int rc=INVALID;
   TgMenu *menu=(lineTypeMenuInfo.create_proc)(NULL, X, Y, &lineTypeMenuInfo,
         INVALID);

   activeMenu = MENU_LINETYPE;
   if (menu != NULL) {
      menu->track_menubar = TrackMenubar;

      rc = TgMenuLoop(menu);
      TgDestroyMenu(menu, TRUE);
   }
   return rc;
}

/* ------------------ LineWidth Functions ------------------ */

static
int ChangeObjLineWidth(ObjPtr, nMask, W, AW, AH, width_spec, aw_spec, ah_spec)
   struct ObjRec *ObjPtr;
   int nMask, W, AW, AH;
   char *width_spec, *aw_spec, *ah_spec;
{
   register struct ObjRec *obj_ptr;
   register int changed=FALSE;

   switch (ObjPtr->type) {
   case OBJ_POLY:
      if ((((nMask & CHANGE_WIDTH) == CHANGE_WIDTH) &&
            (ObjPtr->detail.p->width != W ||
            strcmp(ObjPtr->detail.p->width_spec, width_spec) != 0)) ||
            (AW != (-1) && ((nMask & CHANGE_AW) == CHANGE_AW) &&
            (ObjPtr->detail.p->aw != AW ||
            strcmp(ObjPtr->detail.p->aw_spec, aw_spec) != 0)) ||
            (AH != (-1) && ((nMask & CHANGE_AH) == CHANGE_AH) &&
            (ObjPtr->detail.p->ah != AH ||
            strcmp(ObjPtr->detail.p->ah_spec, ah_spec) != 0))) {
         if (((nMask & CHANGE_WIDTH) == CHANGE_WIDTH)) {
            ObjPtr->detail.p->width = W;
            UtilStrCpyN(ObjPtr->detail.p->width_spec,
                  sizeof(ObjPtr->detail.p->width_spec), width_spec);
         }
         if (AW != (-1) && ((nMask & CHANGE_AW) == CHANGE_AW)) {
            ObjPtr->detail.p->aw = AW;
            UtilStrCpyN(ObjPtr->detail.p->aw_spec,
                  sizeof(ObjPtr->detail.p->aw_spec), aw_spec);
         }
         if (AH != (-1) && ((nMask & CHANGE_AH) == CHANGE_AH)) {
            ObjPtr->detail.p->ah = AH;
            UtilStrCpyN(ObjPtr->detail.p->ah_spec,
                  sizeof(ObjPtr->detail.p->ah_spec), ah_spec);
         }
         changed = TRUE;
         AdjObjSplineVs(ObjPtr);
         if (ObjPtr->detail.p->curved != LT_INTSPLINE) {
            UpdPolyBBox(ObjPtr, ObjPtr->detail.p->n,
                  ObjPtr->detail.p->vlist);
         } else {
            UpdPolyBBox(ObjPtr, ObjPtr->detail.p->intn,
                  ObjPtr->detail.p->intvlist);
         }
      }
      break;
   case OBJ_BOX:
      if ((nMask & CHANGE_WIDTH) == CHANGE_WIDTH) {
         if (ObjPtr->detail.b->width != W ||
               strcmp(ObjPtr->detail.b->width_spec, width_spec) != 0) {
            ObjPtr->detail.b->width = W;
            UtilStrCpyN(ObjPtr->detail.b->width_spec,
                  sizeof(ObjPtr->detail.b->width_spec), width_spec);
            changed = TRUE;
         }
      }
      break;
   case OBJ_OVAL:
      if ((nMask & CHANGE_WIDTH) == CHANGE_WIDTH) {
         if (ObjPtr->detail.o->width != W ||
               strcmp(ObjPtr->detail.o->width_spec, width_spec) != 0) {
            ObjPtr->detail.o->width = W;
            UtilStrCpyN(ObjPtr->detail.o->width_spec,
                  sizeof(ObjPtr->detail.o->width_spec), width_spec);
            changed = TRUE;
         }
      }
      break;
   case OBJ_POLYGON:
      if ((nMask & CHANGE_WIDTH) == CHANGE_WIDTH) {
         if (ObjPtr->detail.g->width != W ||
               strcmp(ObjPtr->detail.g->width_spec, width_spec) != 0) {
            ObjPtr->detail.g->width = W;
            UtilStrCpyN(ObjPtr->detail.g->width_spec,
                  sizeof(ObjPtr->detail.g->width_spec), width_spec);
            changed = TRUE;
         }
      }
      break;
   case OBJ_ARC:
      if ((((nMask & CHANGE_WIDTH) == CHANGE_WIDTH) &&
            (ObjPtr->detail.a->width != W ||
            strcmp(ObjPtr->detail.a->width_spec, width_spec) != 0)) ||
            (AW != (-1) && ((nMask & CHANGE_AW) == CHANGE_AW) &&
            (ObjPtr->detail.a->aw != AW ||
            strcmp(ObjPtr->detail.a->aw_spec, aw_spec) != 0)) ||
            (AH != (-1) && ((nMask & CHANGE_AH) == CHANGE_AH) &&
            (ObjPtr->detail.a->ah != AH ||
            strcmp(ObjPtr->detail.a->ah_spec, ah_spec) != 0))) {
         if (((nMask & CHANGE_WIDTH) == CHANGE_WIDTH)) {
            ObjPtr->detail.a->width = W;
            UtilStrCpyN(ObjPtr->detail.a->width_spec,
                  sizeof(ObjPtr->detail.a->width_spec), width_spec);
         }
         if (AW != (-1) && ((nMask & CHANGE_AW) == CHANGE_AW)) {
            ObjPtr->detail.a->aw = AW;
            UtilStrCpyN(ObjPtr->detail.a->aw_spec,
                  sizeof(ObjPtr->detail.a->aw_spec), aw_spec);
         }
         if (AH != (-1) && ((nMask & CHANGE_AH) == CHANGE_AH)) {
            ObjPtr->detail.a->ah = AH;
            UtilStrCpyN(ObjPtr->detail.a->ah_spec,
                  sizeof(ObjPtr->detail.a->ah_spec), ah_spec);
         }
         changed = TRUE;
         AdjObjSplineVs(ObjPtr);
      }
      break;
   case OBJ_RCBOX:
      if ((nMask & CHANGE_WIDTH) == CHANGE_WIDTH) {
         if (ObjPtr->detail.rcb->width != W ||
               strcmp(ObjPtr->detail.rcb->width_spec, width_spec) != 0) {
            ObjPtr->detail.rcb->width = W;
            UtilStrCpyN(ObjPtr->detail.rcb->width_spec,
                  sizeof(ObjPtr->detail.rcb->width_spec), width_spec);
            changed = TRUE;
         }
      }
      break;

   case OBJ_GROUP:
   case OBJ_SYM:
      for (obj_ptr=ObjPtr->detail.r->last; obj_ptr != NULL;
            obj_ptr=obj_ptr->prev) {
         if (ChangeObjLineWidth(obj_ptr, nMask, W, AW, AH, width_spec, aw_spec,
               ah_spec)) {
            changed = TRUE;
         }
      }
      break;
   }
   if (changed) AdjObjBBox(ObjPtr);
   return changed;
}

void ChangeAllSelLineWidth(WidthIndex, HighLight)
   int WidthIndex, HighLight;
{
   struct SelRec *sel_ptr;
   int ltx, lty, rbx, rby, changed=FALSE, w, aw, ah, dont_do_obj=FALSE;
   char *width_spec, *aw_spec, *ah_spec;

   if (topSel == NULL || stickyMenuSelection) {
      lineWidth = WidthIndex;
      ShowLineWidth();
      UpdatePinnedMenu(MENU_LINEWIDTH);
      if (topSel == NULL) dont_do_obj = TRUE;
   }
   sprintf(gszMsgBox, TgLoadString(STID_LINE_WIDTH_SET_TO_GIVEN_STR),
         curWidthOfLineSpec[WidthIndex]);
   Msg(gszMsgBox);
   if (dont_do_obj) return;

   w = curWidthOfLine[WidthIndex];
   aw = curArrowHeadW[WidthIndex];
   ah = curArrowHeadH[WidthIndex];
   width_spec = curWidthOfLineSpec[WidthIndex];
   aw_spec = curArrowHeadWSpec[WidthIndex];
   ah_spec = curArrowHeadHSpec[WidthIndex];

   if (HighLight) HighLightReverse();
   StartCompositeCmd();
   for (sel_ptr=botSel; sel_ptr != NULL; sel_ptr=sel_ptr->prev) {
      PrepareToReplaceAnObj(sel_ptr->obj);
      if (ChangeObjLineWidth(sel_ptr->obj, CHANGE_LINE_ALL, w, aw, ah,
            width_spec, aw_spec, ah_spec)) {
         changed = TRUE;
         RecordReplaceAnObj(sel_ptr->obj);
      } else {
         AbortPrepareCmd(CMD_REPLACE);
      }
   }
   EndCompositeCmd();

   if (changed) {
      SetFileModified(TRUE);
      ltx = selLtX; lty = selLtY; rbx = selRbX; rby = selRbY;
      UpdSelBBox();
      RedrawAreas(botObj, ltx-GRID_ABS_SIZE(1), lty-GRID_ABS_SIZE(1),
            rbx+GRID_ABS_SIZE(1), rby+GRID_ABS_SIZE(1), selLtX-GRID_ABS_SIZE(1),
            selLtY-GRID_ABS_SIZE(1), selRbX+GRID_ABS_SIZE(1),
            selRbY+GRID_ABS_SIZE(1));
   }
   if (HighLight) HighLightForward();
}

static
int GetLineWidthMatch(index, width, aw, ah)
   int index, width, aw, ah;
{
   int diff=0;

   if (width != INVALID && width != curWidthOfLine[index]) {
      diff += 10*abs(width-curWidthOfLine[index]);
   }
   if (aw != INVALID && aw != curArrowHeadW[index]) {
      diff += abs(aw-curArrowHeadW[index]);
   }
   if (ah != INVALID && ah != curArrowHeadH[index]) {
      diff += abs(ah-curArrowHeadH[index]);
   }
   return diff;
}

int GetBestLineWidthIndex(width, aw, ah)
   int width, aw, ah;
{
   int min_diff=0, min_index=0, index=0;

   min_diff = GetLineWidthMatch(0, width, aw, ah);
   for (index=1; index < maxLineWidths; index++) {
      int diff=GetLineWidthMatch(index, width, aw, ah);

      if (diff < min_diff) {
         min_diff = diff;
         min_index = index;
      }
   }
   return min_index;
}

void RefreshLineWidthMenu(menu)
   TgMenu *menu;
{
   int i, num_items=menu->num_items;
   TgMenuItem *menuitems=menu->menuitems;

   for (i=0; i < num_items; i++) {
      TgMenuItem *menu_item=(&menuitems[i]);
      TgMenuItem stMenuItem;

      memset(&stMenuItem, 0, sizeof(TgMenuItem));
      stMenuItem.state = TGBS_NORMAL;
      stMenuItem.checked = (i == lineWidth);
      TgSetMenuItemInfo(menu_item, TGMU_MASK_STATE|TGMU_MASK_CHECK,
            &stMenuItem);
   }
}

TgMenu *CreateLineWidthMenu(parent_menu, x, y, menu_info, status_str_xlated)
   TgMenu *parent_menu;
   int x, y;
   TgMenuInfo *menu_info;
   int status_str_xlated; /* ignored, always 0 */
{
   int i=0;
   TgMenu *menu=NULL;
   TgMenuInfo stMenuInfo;
   TgMenuItemInfo *item_info=NULL;

   memcpy(&stMenuInfo, menu_info, sizeof(TgMenuInfo));
   stMenuInfo.items = (TgMenuItemInfo*)malloc(
         (maxLineWidths+1)*sizeof(TgMenuItemInfo));
   if (stMenuInfo.items == NULL) FailAllocMessage();
   memset(stMenuInfo.items, 0, (maxLineWidths+1)*sizeof(TgMenuItemInfo));
   for (item_info=stMenuInfo.items, i=0; i < maxLineWidths; item_info++, i++) {
      item_info->menu_str = (char*)(Pixmap*)(&lineWidthPixmap[i]);
      item_info->shortcut_str = NULL;
      sprintf(gszMsgBox, TgLoadCachedString(CSTID_SET_LINE_WIDTH_TO_GIVEN_S),
            curWidthOfLineSpec[i]);
      item_info->status_str = UtilStrDup(gszMsgBox);
      if (item_info->status_str == NULL) FailAllocMessage();
      item_info->submenu_info = NULL;
      item_info->cmdid = CMDID_CHANGEALLSELLINEWIDTH;
   }
   stMenuInfo.items[maxLineWidths].cmdid = INVALID;

   /* the status_str has been translated in TgLoadCachedString() */
   menu = TgCreateMenuFromMenuInfo(parent_menu, x, y, &stMenuInfo, TRUE);
   for (item_info=stMenuInfo.items, i=0; i < maxLineWidths; item_info++, i++) {
      UtilFree(item_info->status_str);
   }
   memset(stMenuInfo.items, 0, (maxLineWidths+1)*sizeof(TgMenuItemInfo));
   free(stMenuInfo.items);
   stMenuInfo.items = NULL;
   if (menu != NULL) {
      TgMenuItem *menu_item=NULL;
      TgMenuItem stMenuItem;

      menu->track_menubar = TRUE;
      TgAdjustMenuGeometry(menu, menuImageW, menuImageH, maxLineWidths);
      menu_item = (&menu->menuitems[lineWidth]);

      memset(&stMenuItem, 0, sizeof(TgMenuItem));
      stMenuItem.checked = TRUE;
      if (!TgSetMenuItemInfo(menu_item, TGMU_MASK_CHECK, &stMenuItem)) {
         return TgDestroyMenu(menu, TRUE);
      }
   }
   return menu;
}

int LineWidthMenu(X, Y, TrackMenubar)
   int X, Y, TrackMenubar;
{
   int rc=INVALID;
   TgMenu *menu=(lineWidthMenuInfo.create_proc)(NULL, X, Y, &lineWidthMenuInfo,
         INVALID);

   activeMenu = MENU_LINEWIDTH;
   if (menu != NULL) {
      menu->track_menubar = TrackMenubar;

      rc = TgMenuLoop(menu);
      TgDestroyMenu(menu, TRUE);
   }
   return rc;
}

/* ------------------ LineDash Functions ------------------ */

static
int ChangeObjLineDash(ObjPtr, DashIndex)
   struct ObjRec *ObjPtr;
   int DashIndex;
{
   register struct ObjRec *obj_ptr;
   int changed=FALSE;

   switch (ObjPtr->type) {
   case OBJ_POLY:
      if (ObjPtr->detail.p->dash != DashIndex) {
         ObjPtr->detail.p->dash = DashIndex;
         changed = TRUE;
      }
      break;
   case OBJ_BOX:
      if (ObjPtr->detail.b->dash != DashIndex) {
         ObjPtr->detail.b->dash = DashIndex;
         changed = TRUE;
      }
      break;
   case OBJ_OVAL:
      if (ObjPtr->detail.o->dash != DashIndex) {
         ObjPtr->detail.o->dash = DashIndex;
         changed = TRUE;
      }
      break;
   case OBJ_POLYGON:
      if (ObjPtr->detail.g->dash != DashIndex) {
         ObjPtr->detail.g->dash = DashIndex;
         changed = TRUE;
      }
      break;
   case OBJ_ARC:
      if (ObjPtr->detail.a->dash != DashIndex) {
         ObjPtr->detail.a->dash = DashIndex;
         changed = TRUE;
      }
      break;
   case OBJ_RCBOX:
      if (ObjPtr->detail.rcb->dash != DashIndex) {
         ObjPtr->detail.rcb->dash = DashIndex;
         changed = TRUE;
      }
      break;

   case OBJ_GROUP:
   case OBJ_SYM:
      for (obj_ptr=ObjPtr->detail.r->last; obj_ptr != NULL;
            obj_ptr=obj_ptr->prev) {
         if (ChangeObjLineDash(obj_ptr, DashIndex)) {
            changed = TRUE;
         }
      }
      break;
   }
   return changed;
}

void ChangeAllSelLineDash(DashIndex, HighLight)
   int DashIndex, HighLight;
{
   register struct SelRec *sel_ptr;
   int changed=FALSE;

   if (topSel == NULL || stickyMenuSelection) {
      curDash = DashIndex;
      ShowDash();
      UpdatePinnedMenu(MENU_LINEDASH);
      if (topSel == NULL) return;
   }

   if (HighLight) HighLightReverse();
   StartCompositeCmd();
   for (sel_ptr=botSel; sel_ptr != NULL; sel_ptr=sel_ptr->prev) {
      PrepareToReplaceAnObj(sel_ptr->obj);
      if (ChangeObjLineDash(sel_ptr->obj, DashIndex)) {
         changed = TRUE;
         RecordReplaceAnObj(sel_ptr->obj);
      } else {
         AbortPrepareCmd(CMD_REPLACE);
      }
   }
   EndCompositeCmd();

   if (changed) {
      SetFileModified(TRUE);
      RedrawAnArea(botObj, selLtX-GRID_ABS_SIZE(1), selLtY-GRID_ABS_SIZE(1),
            selRbX+GRID_ABS_SIZE(1), selRbY+GRID_ABS_SIZE(1));
   }
   if (HighLight) HighLightForward();
}

void RefreshLineDashMenu(menu)
   TgMenu *menu;
{
   int i, num_items=menu->num_items;
   TgMenuItem *menuitems=menu->menuitems;

   for (i=0; i < num_items; i++) {
      TgMenuItem *menu_item=(&menuitems[i]);
      TgMenuItem stMenuItem;

      memset(&stMenuItem, 0, sizeof(TgMenuItem));
      stMenuItem.state = TGBS_NORMAL;
      stMenuItem.checked = (i == curDash);
      TgSetMenuItemInfo(menu_item, TGMU_MASK_STATE|TGMU_MASK_CHECK,
            &stMenuItem);
   }
}

TgMenu *CreateLineDashMenu(parent_menu, x, y, menu_info, status_str_xlated)
   TgMenu *parent_menu;
   int x, y;
   TgMenuInfo *menu_info;
   int status_str_xlated; /* ignored, always 0 */
{
   int i=0;
   TgMenu *menu=NULL;
   TgMenuInfo stMenuInfo;
   TgMenuItemInfo *item_info=NULL;

   memcpy(&stMenuInfo, menu_info, sizeof(TgMenuInfo));
   stMenuInfo.items = (TgMenuItemInfo*)malloc(
         (MAXDASHES+1)*sizeof(TgMenuItemInfo));
   if (stMenuInfo.items == NULL) FailAllocMessage();
   memset(stMenuInfo.items, 0, (MAXDASHES+1)*sizeof(TgMenuItemInfo));
   for (item_info=stMenuInfo.items, i=0; i < MAXDASHES; item_info++, i++) {
      item_info->menu_str = (char*)(Pixmap*)(&lineDashPixmap[i]);
      item_info->shortcut_str = NULL;
      if (i == 0) {
         sprintf(gszMsgBox, TgLoadCachedString(CSTID_SET_PEN_DASH_PAT_NO_DASH));
      } else {
         sprintf(gszMsgBox, TgLoadCachedString(CSTID_SET_PEN_DASH_PAT_PAT_NUM),
               i);
      }
      item_info->status_str = UtilStrDup(gszMsgBox);
      if (item_info->status_str == NULL) FailAllocMessage();
      item_info->submenu_info = NULL;
      item_info->cmdid = CMDID_CHANGEALLSELLINEDASH;
   }
   stMenuInfo.items[MAXDASHES].cmdid = INVALID;

   /* the status_str has been translated in TgLoadCachedString() */
   menu = TgCreateMenuFromMenuInfo(parent_menu, x, y, &stMenuInfo, TRUE);
   for (item_info=stMenuInfo.items, i=0; i < MAXDASHES; item_info++, i++) {
      UtilFree(item_info->status_str);
   }
   memset(stMenuInfo.items, 0, (MAXDASHES+1)*sizeof(TgMenuItemInfo));
   free(stMenuInfo.items);
   stMenuInfo.items = NULL;
   if (menu != NULL) {
      TgMenuItem *menu_item=NULL;
      TgMenuItem stMenuItem;

      menu->track_menubar = TRUE;
      TgAdjustMenuGeometry(menu, menuImageW, menuImageH, MAXDASHES);
      menu_item = (&menu->menuitems[curDash]);

      memset(&stMenuItem, 0, sizeof(TgMenuItem));
      stMenuItem.checked = TRUE;
      if (!TgSetMenuItemInfo(menu_item, TGMU_MASK_CHECK, &stMenuItem)) {
         return TgDestroyMenu(menu, TRUE);
      }
   }
   return menu;
}

int LineDashMenu(X, Y, TrackMenubar)
   int X, Y, TrackMenubar;
{
   int rc=INVALID;
   TgMenu *menu=(lineDashMenuInfo.create_proc)(NULL, X, Y, &lineDashMenuInfo,
         INVALID);

   activeMenu = MENU_LINEDASH;
   if (menu != NULL) {
      menu->track_menubar = TrackMenubar;

      rc = TgMenuLoop(menu);
      TgDestroyMenu(menu, TRUE);
   }
   return rc;
}

/* ------------------ Pen Functions ------------------ */

int ChangeObjPen(ObjPtr, PenIndex)
   struct ObjRec *ObjPtr;
   int PenIndex;
{
   register struct ObjRec *obj_ptr;
   int changed=FALSE;

   switch (ObjPtr->type) {
   case OBJ_POLY:
      if (ObjPtr->detail.p->pen != PenIndex) {
         ObjPtr->detail.p->pen = PenIndex;
         changed = TRUE;
      }
      break;
   case OBJ_TEXT:
      if (ObjPtr->detail.t->pen != PenIndex) {
         ObjPtr->detail.t->pen = PenIndex;
         AdjObjCache(ObjPtr);
         changed = TRUE;
      }
      break;
   case OBJ_BOX:
      if (ObjPtr->detail.b->pen != PenIndex) {
         ObjPtr->detail.b->pen = PenIndex;
         changed = TRUE;
      }
      break;
   case OBJ_OVAL:
      if (ObjPtr->detail.o->pen != PenIndex) {
         ObjPtr->detail.o->pen = PenIndex;
         changed = TRUE;
      }
      break;
   case OBJ_POLYGON:
      if (ObjPtr->detail.g->pen != PenIndex) {
         ObjPtr->detail.g->pen = PenIndex;
         changed = TRUE;
      }
      break;
   case OBJ_ARC:
      if (ObjPtr->detail.a->pen != PenIndex) {
         ObjPtr->detail.a->pen = PenIndex;
         changed = TRUE;
      }
      break;
   case OBJ_RCBOX:
      if (ObjPtr->detail.rcb->pen != PenIndex) {
         ObjPtr->detail.rcb->pen = PenIndex;
         changed = TRUE;
      }
      break;

   case OBJ_GROUP:
   case OBJ_SYM:
      for (obj_ptr=ObjPtr->detail.r->last; obj_ptr != NULL;
            obj_ptr=obj_ptr->prev) {
         if (ChangeObjPen(obj_ptr, PenIndex)) {
            changed = TRUE;
         }
      }
      break;
   }
   if (changePropertiesOfAttrs && ObjPtr->type != OBJ_TEXT) {
      struct AttrRec *attr_ptr=ObjPtr->fattr;

      for ( ; attr_ptr != NULL; attr_ptr=attr_ptr->next) {
         changed |= ChangeObjPen(attr_ptr->obj, PenIndex);
      }
   }
   return changed;
}

void ChangeAllSelPen(PenIndex, HighLight)
   int PenIndex, HighLight;
{
   register struct SelRec *sel_ptr;
   int changed=FALSE;

   if (topSel == NULL || stickyMenuSelection) {
      if (!(curChoice == DRAWTEXT && textCursorShown)) {
         TieLooseEnds();
      }
      penPat = PenIndex;
      if (curChoice == DRAWTEXT && textCursorShown) {
         if (ChangeObjPen(curTextObj, PenIndex)) {
            curTextModified = TRUE;
            UpdCurTextBBox();
            RedrawCurText();
            SetFileModified(TRUE);
            if (cycleThroughChoice) {
               SetPushedFontValue(PUSH_PEN, penPat);
            }
         }
      } else {
         textCursorShown = FALSE;
      }
      ShowPen();
      UpdatePinnedMenu(MENU_PEN);
      if (topSel == NULL) return;
   }

   if (HighLight) HighLightReverse();
   StartCompositeCmd();
   for (sel_ptr=botSel; sel_ptr != NULL; sel_ptr=sel_ptr->prev) {
      PrepareToReplaceAnObj(sel_ptr->obj);
      if (ChangeObjPen(sel_ptr->obj, PenIndex)) {
         changed = TRUE;
         RecordReplaceAnObj(sel_ptr->obj);
      } else {
         AbortPrepareCmd(CMD_REPLACE);
      }
   }
   EndCompositeCmd();

   if (changed) {
      SetFileModified(TRUE);
      RedrawAnArea(botObj, selLtX-GRID_ABS_SIZE(1), selLtY-GRID_ABS_SIZE(1),
            selRbX+GRID_ABS_SIZE(1), selRbY+GRID_ABS_SIZE(1));
   }
   if (HighLight) HighLightForward();
}

void RefreshPenMenu(menu)
   TgMenu *menu;
{
   int i, num_items=menu->num_items;
   TgMenuItem *menuitems=menu->menuitems;

   for (i=0; i < num_items; i++) {
      TgMenuItem *menu_item=(&menuitems[i]);
      TgMenuItem stMenuItem;

      memset(&stMenuItem, 0, sizeof(TgMenuItem));
      stMenuItem.state = TGBS_NORMAL;
      stMenuItem.checked = (i == penPat);
      TgSetMenuItemInfo(menu_item, TGMU_MASK_STATE|TGMU_MASK_CHECK,
            &stMenuItem);
   }
}

TgMenu *CreatePenMenu(parent_menu, x, y, menu_info, status_str_xlated)
   TgMenu *parent_menu;
   int x, y;
   TgMenuInfo *menu_info;
   int status_str_xlated; /* ignored, always 0 */
{
   int i=0;
   TgMenu *menu=NULL;
   TgMenuInfo stMenuInfo;
   TgMenuItemInfo *item_info=NULL;

   memcpy(&stMenuInfo, menu_info, sizeof(TgMenuInfo));
   stMenuInfo.items = (TgMenuItemInfo*)malloc(
         (MAXPATTERNS+1)*sizeof(TgMenuItemInfo));
   if (stMenuInfo.items == NULL) FailAllocMessage();
   memset(stMenuInfo.items, 0, (MAXPATTERNS+1)*sizeof(TgMenuItemInfo));
   for (item_info=stMenuInfo.items, i=0; i < MAXPATTERNS; item_info++, i++) {
      item_info->menu_str = (char*)(Pixmap*)(&patPixmap[i]);
      item_info->shortcut_str = NULL;
      switch (i) {
      case NONEPAT:
         sprintf(gszMsgBox, TgLoadCachedString(CSTID_SET_PEN_TO_NONE));
         break;
      case SOLIDPAT:
         sprintf(gszMsgBox, TgLoadCachedString(CSTID_SET_PEN_TO_SOLID));
         break;
      case BACKPAT:
         sprintf(gszMsgBox, TgLoadCachedString(CSTID_SET_PEN_TO_BACKGROUND));
         break;
      default:
         sprintf(gszMsgBox, TgLoadCachedString(CSTID_SET_PEN_TO_PAT_NUM), i);
         break;
      }
      item_info->status_str = UtilStrDup(gszMsgBox);
      if (item_info->status_str == NULL) FailAllocMessage();
      item_info->submenu_info = NULL;
      item_info->cmdid = CMDID_CHANGEALLSELPEN;
   }
   stMenuInfo.items[MAXPATTERNS].cmdid = INVALID;

   /* the status_str has been translated in TgLoadCachedString() */
   menu = TgCreateMenuFromMenuInfo(parent_menu, x, y, &stMenuInfo, TRUE);
   for (item_info=stMenuInfo.items, i=0; i < MAXPATTERNS; item_info++, i++) {
      UtilFree(item_info->status_str);
   }
   memset(stMenuInfo.items, 0, (MAXPATTERNS+1)*sizeof(TgMenuItemInfo));
   free(stMenuInfo.items);
   stMenuInfo.items = NULL;
   if (menu != NULL) {
      TgMenuItem *menu_item=NULL;
      TgMenuItem stMenuItem;

      menu->track_menubar = TRUE;
      TgAdjustMenuGeometry(menu, choiceImageW, choiceImageH, 8);
      menu_item = (&menu->menuitems[penPat]);

      memset(&stMenuItem, 0, sizeof(TgMenuItem));
      stMenuItem.checked = TRUE;
      if (!TgSetMenuItemInfo(menu_item, TGMU_MASK_CHECK, &stMenuItem)) {
         return TgDestroyMenu(menu, TRUE);
      }
   }
   return menu;
}

int PenMenu(X, Y, TrackMenubar)
   int X, Y, TrackMenubar;
{
   int rc=INVALID;
   TgMenu *menu=(penMenuInfo.create_proc)(NULL, X, Y, &penMenuInfo, INVALID);

   activeMenu = MENU_PEN;
   if (menu != NULL) {
      menu->track_menubar = TrackMenubar;

      rc = TgMenuLoop(menu);
      TgDestroyMenu(menu, TRUE);
   }
   return rc;
}

/* ------------------ Toggling and Changing Functions ------------------ */

static
int ToggleObjLineType(ObjPtr)
   struct ObjRec *ObjPtr;
{
   register struct ObjRec *obj_ptr;
   register int changed=FALSE;

   switch (ObjPtr->type) {
   case OBJ_POLY:
      ObjPtr->detail.p->curved = (ObjPtr->detail.p->curved+1) % MAXLINETYPES;
      changed = TRUE;
      AdjObjSplineVs(ObjPtr);
      if (ObjPtr->detail.p->curved != LT_INTSPLINE) {
         UpdPolyBBox(ObjPtr, ObjPtr->detail.p->n,
               ObjPtr->detail.p->vlist);
      } else {
         UpdPolyBBox(ObjPtr, ObjPtr->detail.p->intn,
               ObjPtr->detail.p->intvlist);
      }
      break;
   case OBJ_POLYGON:
      ObjPtr->detail.g->curved = (ObjPtr->detail.g->curved+1) % MAXLINETYPES;
      changed = TRUE;
      AdjObjSplineVs(ObjPtr);
      if (ObjPtr->detail.g->curved != LT_INTSPLINE) {
         UpdPolyBBox(ObjPtr, ObjPtr->detail.g->n,
               ObjPtr->detail.g->vlist);
      } else {
         UpdPolyBBox(ObjPtr, ObjPtr->detail.g->intn,
               ObjPtr->detail.g->intvlist);
      }
      break;

   case OBJ_GROUP:
   case OBJ_SYM:
      for (obj_ptr=ObjPtr->detail.r->last; obj_ptr != NULL;
            obj_ptr=obj_ptr->prev) {
         if (ToggleObjLineType(obj_ptr)) {
            changed = TRUE;
         }
      }
      break;
   }
   if (changed) AdjObjBBox(ObjPtr);
   return changed;
}

void ToggleAllSelLineType()
{
   register struct SelRec *sel_ptr;
   register int changed=FALSE;

   if (topSel == NULL) {
      curSpline = (curSpline+1) % MAXLINETYPES;
      ShowLineType();
      UpdatePinnedMenu(MENU_LINETYPE);
      return;
   }

   HighLightReverse();
   StartCompositeCmd();
   for (sel_ptr=botSel; sel_ptr != NULL; sel_ptr=sel_ptr->prev) {
      PrepareToReplaceAnObj(sel_ptr->obj);
      if (ToggleObjLineType(sel_ptr->obj)) {
         changed = TRUE;
         RecordReplaceAnObj(sel_ptr->obj);
      } else {
         AbortPrepareCmd(CMD_REPLACE);
      }
   }
   EndCompositeCmd();

   if (changed) {
      SetFileModified(TRUE);
      RedrawAnArea(botObj, selLtX-GRID_ABS_SIZE(1), selLtY-GRID_ABS_SIZE(1),
            selRbX+GRID_ABS_SIZE(1), selRbY+GRID_ABS_SIZE(1));
   }
   HighLightForward();
}

static
int ChangeObjRCBRadius(ObjPtr, Radius)
   struct ObjRec *ObjPtr;
   int Radius;
{
   register struct ObjRec *obj_ptr;
   int changed=FALSE;

   switch (ObjPtr->type) {
   case OBJ_RCBOX:
      if (ObjPtr->detail.rcb->radius != Radius) {
         ObjPtr->detail.rcb->radius = Radius;
         AdjObjCache(ObjPtr);
         changed = TRUE;
      }
      break;

   case OBJ_GROUP:
   case OBJ_SYM:
      for (obj_ptr=ObjPtr->detail.r->last; obj_ptr != NULL;
            obj_ptr=obj_ptr->prev) {
         if (ChangeObjRCBRadius(obj_ptr, Radius)) {
            changed = TRUE;
         }
      }
      break;
   }
   return changed;
}

void ChangeAllSelRCBRadius(Radius)
   int Radius;
{
   register struct SelRec *sel_ptr;
   int changed=FALSE;

   if (topSel == NULL) {
      rcbRadius = Radius;
      ShowRCBRadius();
      return;
   }

   HighLightReverse();
   StartCompositeCmd();
   for (sel_ptr=botSel; sel_ptr != NULL; sel_ptr=sel_ptr->prev) {
      PrepareToReplaceAnObj(sel_ptr->obj);
      if (ChangeObjRCBRadius(sel_ptr->obj, Radius)) {
         changed = TRUE;
         RecordReplaceAnObj(sel_ptr->obj);
      } else {
         AbortPrepareCmd(CMD_REPLACE);
      }
   }
   EndCompositeCmd();

   if (changed) {
      SetFileModified(TRUE);
      RedrawAnArea(botObj, selLtX-GRID_ABS_SIZE(1), selLtY-GRID_ABS_SIZE(1),
            selRbX+GRID_ABS_SIZE(1), selRbY+GRID_ABS_SIZE(1));
   }
   HighLightForward();
}

void UpdateSelObjs()
{
   struct SelRec *saved_top_sel=topSel, *saved_bot_sel=botSel;

   if (topSel == NULL) {
      MsgBox(TgLoadCachedString(CSTID_NO_OBJ_SELECTED), TOOL_NAME, INFO_MB);
      return;
   }
   BackupCopiedProperties();
   HighLightReverse();

   topSel = botSel = NULL;
   CopyProperties(FALSE);

   topSel = saved_top_sel;
   botSel = saved_bot_sel;
   PasteProperties(FALSE);

   HighLightForward();
   RestoreCopiedProperties();
}

void ChangeAllSelRealLineWidth(nMask, Width, AW, AH,
      width_spec, aw_spec, ah_spec, HighLight)
   int nMask, Width, AW, AH, HighLight;
   char *width_spec, *aw_spec, *ah_spec;
{
   register struct SelRec *sel_ptr;
   int changed=FALSE;

   if (topSel == NULL) {
      MsgBox(TgLoadCachedString(CSTID_NO_OBJ_SELECTED), TOOL_NAME, INFO_MB);
      return;
   }
   if (HighLight) HighLightReverse();
   StartCompositeCmd();
   for (sel_ptr=botSel; sel_ptr != NULL; sel_ptr=sel_ptr->prev) {
      PrepareToReplaceAnObj(sel_ptr->obj);
      if (ChangeObjLineWidth(sel_ptr->obj, nMask, Width, AW, AH,
            width_spec, aw_spec, ah_spec)) {
         changed = TRUE;
         RecordReplaceAnObj(sel_ptr->obj);
      } else {
         AbortPrepareCmd(CMD_REPLACE);
      }
   }
   EndCompositeCmd();

   if (changed) {
      int ltx=selLtX, lty=selLtY, rbx=selRbX, rby=selRbY;

      SetFileModified(TRUE);
      UpdSelBBox();
      RedrawAreas(botObj, ltx-GRID_ABS_SIZE(1), lty-GRID_ABS_SIZE(1),
            rbx+GRID_ABS_SIZE(1), rby+GRID_ABS_SIZE(1), selLtX-GRID_ABS_SIZE(1),
            selLtY-GRID_ABS_SIZE(1), selRbX+GRID_ABS_SIZE(1),
            selRbY+GRID_ABS_SIZE(1));
   }
   if (HighLight) HighLightForward();
}

void SetSelLineWidth(pszBuf)
   char *pszBuf;
{
   char spec[MAXSTRING+1], *w_str=NULL, *aw_str=NULL, *ah_str=NULL, *c_ptr=NULL;
   int w=(-1), aw=(-1), ah=(-1), mult_set=FALSE;
   char width_spec[40], aw_spec[40], ah_spec[40];
   float fw, faw, fah, mult=((float)1.0);

   if (topSel == NULL) {
      MsgBox(TgLoadCachedString(CSTID_NO_OBJ_SELECTED), TOOL_NAME, INFO_MB);
      return;
   }
   *spec = '\0';
   if (pszBuf == NULL) {
      Dialog(TgLoadString(STID_ENTER_LINE_WIDTH_OPT_AW_AH),
            TgLoadCachedString(CSTID_DLG_ACCEPT_CANCEL), spec);
   } else {
      UtilStrCpyN(spec, sizeof(spec), pszBuf);
   }
   UtilTrimBlanks(spec);
   if (*spec == '\0') return;
   *width_spec = *aw_spec = *ah_spec = '\0';
   if ((w_str=strtok(spec, ", \t\n\r")) == NULL) return;
   UtilTrimBlanks(w_str);
   if ((c_ptr=strstr(w_str, "in")) != NULL ||
         (c_ptr=strstr(w_str, "In")) != NULL ||
         (c_ptr=strstr(w_str, "IN")) != NULL) {
      *c_ptr = '\0';
      mult = (float)PIX_PER_INCH;
      mult_set = TRUE;
   } else if ((c_ptr=strstr(w_str, "cm")) != NULL ||
         (c_ptr=strstr(w_str, "Cm")) != NULL ||
         (c_ptr=strstr(w_str, "CM")) != NULL) {
      *c_ptr = '\0';
      mult = (float)ONE_CM;
      mult_set = TRUE;
   }
   if (sscanf(w_str, "%f", &fw) != 1 || fw < (float)0.0) {
      sprintf(gszMsgBox, TgLoadString(STID_INVALID_GIVEN_STR_WIDTH), w_str);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return;
   }
   if (mult_set) {
      fw *= mult;
      sprintf(width_spec, "%.3f", fw);
   } else {
      UtilStrCpyN(width_spec, sizeof(width_spec), w_str);
   }
   w = round(fw);

   if ((aw_str=strtok(NULL, ", \t\n\r")) != NULL &&
         (ah_str=strtok(NULL, ", \t\n\r")) != NULL) {
      int aw_mult_set=FALSE, ah_mult_set=FALSE;
      float aw_mult=((float)1.0), ah_mult=((float)1.0);

      UtilTrimBlanks(aw_str);
      UtilTrimBlanks(ah_str);

      if ((c_ptr=strstr(aw_str, "in")) != NULL ||
            (c_ptr=strstr(aw_str, "In")) != NULL ||
            (c_ptr=strstr(aw_str, "IN")) != NULL) {
         *c_ptr = '\0';
         aw_mult = (float)PIX_PER_INCH;
         aw_mult_set = TRUE;
      } else if ((c_ptr=strstr(aw_str, "cm")) != NULL ||
            (c_ptr=strstr(aw_str, "Cm")) != NULL ||
            (c_ptr=strstr(aw_str, "CM")) != NULL) {
         *c_ptr = '\0';
         aw_mult = (float)ONE_CM;
         aw_mult_set = TRUE;
      }
      if ((c_ptr=strstr(ah_str, "in")) != NULL ||
            (c_ptr=strstr(ah_str, "In")) != NULL ||
            (c_ptr=strstr(ah_str, "IN")) != NULL) {
         *c_ptr = '\0';
         ah_mult = (float)PIX_PER_INCH;
         ah_mult_set = TRUE;
      } else if ((c_ptr=strstr(ah_str, "cm")) != NULL ||
            (c_ptr=strstr(ah_str, "Cm")) != NULL ||
            (c_ptr=strstr(ah_str, "CM")) != NULL) {
         *c_ptr = '\0';
         ah_mult = (float)ONE_CM;
         ah_mult_set = TRUE;
      }
      if (sscanf(aw_str, "%f", &faw) != 1 || faw < (float)0.0 ||
            sscanf(ah_str, "%f", &fah) != 1 || fah < (float)0.0) {
         sprintf(gszMsgBox, TgLoadString(STID_INVALID_GIVEN_STR_AW_AH),
               aw_str, ah_str);
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
         return;
      }
      if (aw_mult_set) {
         faw *= aw_mult;
         sprintf(aw_spec, "%.3f", faw);
      } else {
         UtilStrCpyN(aw_spec, sizeof(aw_spec), aw_str);
      }
      aw = round(faw);

      if (ah_mult_set) {
         fah *= ah_mult;
         sprintf(ah_spec, "%.3f", fah);
      } else {
         UtilStrCpyN(ah_spec, sizeof(ah_spec), ah_str);
      }
      ah = round(fah);
   }
   ChangeAllSelRealLineWidth(CHANGE_LINE_ALL, w, aw, ah,
         width_spec, aw_spec, ah_spec, TRUE);
}

