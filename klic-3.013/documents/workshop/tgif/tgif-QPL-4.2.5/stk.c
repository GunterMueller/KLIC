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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/stk.c,v 1.7 2011/05/16 16:21:59 william Exp $
 */

#define _INCLUDE_FROM_STK_C_

#include "tgifdefs.h"

#include "align.e"
#include "attr.e"
#include "box.e"
#include "button.e"
#include "choice.e"
#include "cmd.e"
#include "color.e"
#include "cursor.e"
#include "dialog.e"
#include "drawing.e"
#include "dup.e"
#include "file.e"
#include "font.e"
#include "grid.e"
#include "mainmenu.e"
#include "mark.e"
#include "menu.e"
#include "msg.e"
#include "names.e"
#include "navigate.e"
#include "obj.e"
#include "page.e"
#include "pattern.e"
#include "raster.e"
#include "rect.e"
#include "ruler.e"
#include "scroll.e"
#include "select.e"
#include "setup.e"
#include "stk.e"
#include "strtbl.e"
#include "text.e"
#include "util.e"
#include "version.e"
#include "wb.e"

struct ObjRec	*tgifObj=NULL;
struct StkRec	*topStk=NULL;

int AncesterModified()
{
   struct StkRec *stk_ptr;

   for (stk_ptr = topStk; stk_ptr != NULL; stk_ptr = stk_ptr->next) {
      if (stk_ptr->file_mod) {
         return TRUE;
      }
   }
   return FALSE;
}

void InitTgifObj()
{
   tgifObj = (struct ObjRec *)malloc(sizeof(struct ObjRec));
   if (tgifObj == NULL) FailAllocMessage();
   memset(tgifObj, 0, sizeof(struct ObjRec));
   tgifObj->type = OBJ_BOX;
   tgifObj->x = tgifObj->y = 0;
   tgifObj->color = 0;
   tgifObj->id = (-1);
   tgifObj->dirty = FALSE;
   tgifObj->rotation = 0;
   tgifObj->marked = tgifObj->locked = FALSE;
   tgifObj->obbox.ltx = tgifObj->obbox.lty = 0;
   tgifObj->obbox.rbx = tgifObj->obbox.rby = 0;
   tgifObj->bbox.ltx = tgifObj->bbox.lty = 0;
   tgifObj->bbox.rbx = tgifObj->bbox.rby = 0;
   tgifObj->next = tgifObj->prev = NULL;
   tgifObj->fattr = tgifObj->lattr = NULL;
   tgifObj->invisible = FALSE;
   tgifObj->trans_pat = FALSE;
   tgifObj->detail.b = (struct BoxRec *)malloc(sizeof(struct BoxRec));
   if (tgifObj->detail.b == NULL) FailAllocMessage();
   memset(tgifObj->detail.b, 0, sizeof(struct BoxRec));
   tgifObj->detail.b->fill = NONEPAT;
   tgifObj->detail.b->pen = NONEPAT;
   tgifObj->detail.b->width = 0;
   tgifObj->detail.b->dash = 0;
}

void InitStk()
{
   curSymDir[0] = '\0';
   InitTgifObj();
}

struct StkRec *SaveFileInfo()
{
   struct StkRec *stk_ptr;

   stk_ptr = (struct StkRec *)malloc(sizeof(struct StkRec));
   if (stk_ptr == NULL) FailAllocMessage();
   memset(stk_ptr, 0, sizeof(struct StkRec));

   stk_ptr->next = topStk;
   stk_ptr->sel = (topSel == NULL ? NULL : topSel->obj);
   stk_ptr->first = topObj;
   stk_ptr->last = botObj;
   stk_ptr->file_mod = fileModified;
   stk_ptr->id = objId;
   stk_ptr->page_style = pageStyle;

   stk_ptr->orig_x = drawOrigX;
   stk_ptr->orig_y = drawOrigY;
   stk_ptr->zoom = zoomScale;
   stk_ptr->zoomed = zoomedIn;
   stk_ptr->grid_system = gridSystem;
   stk_ptr->english_grid = xyEnglishGrid;
   stk_ptr->metric_grid = xyMetricGrid;
   stk_ptr->snap_on = snapOn;
   stk_ptr->color = colorIndex;
   stk_ptr->h_align = horiAlign;
   stk_ptr->v_align = vertAlign;
   stk_ptr->line_w = lineWidth;
   stk_ptr->line_s = lineStyle;
   stk_ptr->trans_pat = transPat;
   stk_ptr->fill = objFill;
   stk_ptr->pen = penPat;
   stk_ptr->just = textJust;
   stk_ptr->v_space = textVSpace;
   stk_ptr->font = curFont;
   stk_ptr->f_style = curStyle;
   stk_ptr->f_sz_unit = curSzUnit;
   stk_ptr->print_mag = printMag;
   stk_ptr->grid_shown = gridShown;
   stk_ptr->move_mode = moveMode;
   stk_ptr->rcb_radius = rcbRadius;
   stk_ptr->underline_on = curUnderlineOn;
   stk_ptr->underline_y_offset = curUnderlineYOffset;
   stk_ptr->overline_on = curOverlineOn;
   stk_ptr->overline_y_offset = curOverlineYOffset;
   stk_ptr->first_file_attr = tgifObj->fattr;
   stk_ptr->last_file_attr = tgifObj->lattr;

   stk_ptr->first_page = firstPage;
   stk_ptr->last_page = lastPage;
   stk_ptr->cur_page = curPage;
   stk_ptr->cur_page_num = curPageNum;
   stk_ptr->last_page_num = lastPageNum;
   stk_ptr->cols = paperCol;
   stk_ptr->rows = paperRow;
   stk_ptr->page_layout_mode = pageLayoutMode;

   stk_ptr->color_dump = colorDump;
   stk_ptr->one_page_width = onePageWidth;
   stk_ptr->one_page_height = onePageHeight;

   stk_ptr->first_cmd = firstCmd;
   stk_ptr->last_cmd = lastCmd;
   stk_ptr->cur_cmd = curCmd;
   stk_ptr->history_count = historyCount;

   strcpy(stk_ptr->dir, curDir);
   stk_ptr->name_valid = curFileDefined;
   if (stk_ptr->name_valid) strcat(stk_ptr->name, curFileName);
   strcpy(stk_ptr->sym_dir, curSymDir);
   strcpy(stk_ptr->domain, curDomainName);

   stk_ptr->saved_comments = savedComments;
   stk_ptr->saved_comments_len = savedCommentsLen;

   stk_ptr->file_bg_pixel = myFileBgPixel;
   stk_ptr->file_fg_pixel = myFileFgPixel;
   stk_ptr->file_bg_color_str = myFileBgColorStr;
   stk_ptr->file_fg_color_str = myFileFgColorStr;
   stk_ptr->file_bg_gif_url = myFileBgGifURL;
   stk_ptr->file_bg_pixmap_w = myFileBgPixmapW;
   stk_ptr->file_bg_pixmap_h = myFileBgPixmapH;
   stk_ptr->file_bg_pixmap = myFileBgPixmap;
   myFileBgPixel = myFileFgPixel = INVALID;
   myFileBgColorStr = myFileFgColorStr = myFileBgGifURL = NULL;
   myFileBgPixmap = None;

   return stk_ptr;
}

void ResetFileInfo()
{
   ClearBgColorInfo(FALSE);
   ClearBgPixmapInfo(FALSE);

   tgifObj->fattr = NULL;
   tgifObj->lattr = NULL;

   firstCmd = lastCmd = curCmd = NULL;
   historyCount = 0;

   topObj = botObj = NULL;
   firstPage = lastPage = curPage = NULL;
   lastPageNum = 0;
   InitPage();

   savedComments = NULL;
   savedCommentsLen = 0;
}

int PushIcon()
{
   struct StkRec *stk_ptr;
   struct ObjRec *obj_ptr;
   char sym_name[MAXPATHLENGTH], path_name[MAXPATHLENGTH];
   char file_name[MAXPATHLENGTH], *rest;
   int read_status, short_name;
   FILE *fp;
   int tmp_linenum, interrupted, is_pin;
   char tmp_filename[MAXPATHLENGTH], tmp_filefullpath[MAXPATHLENGTH];

   if (gstWBInfo.do_whiteboard) {
      MsgBox(TgLoadString(STID_PUSH_IN_WB), TOOL_NAME, INFO_MB);
      return FALSE;
   }
   if (topSel == NULL || topSel != botSel ||
      (topSel->obj->type != OBJ_ICON && topSel->obj->type != OBJ_PIN)) {
      MsgBox(TgLoadString(STID_SEL_ONE_ICON_OBJ_TO_PUSH), TOOL_NAME, INFO_MB);
      return FALSE;
   }
   is_pin = (topSel->obj->type==OBJ_PIN);

   strcpy(sym_name, topSel->obj->detail.r->s);
   if (*sym_name == DIR_SEP) {
      char *psz=UtilStrRChr(sym_name, DIR_SEP), *psz1=sym_name;

      *psz = '\0';
      strcpy(path_name, sym_name);
      *psz++ = DIR_SEP;
      while (*psz != '\0') *psz1++ = *psz++;
      *psz1 = '\0';
   } else {
      if (!GetSymbolPath(sym_name, is_pin, path_name)) {
         sprintf(gszMsgBox, TgLoadString(STID_CANT_FND_SYM_FILE_IN_PATH),
               sym_name, is_pin ? PIN_FILE_EXT : SYM_FILE_EXT, curDomainPath);
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
         return FALSE;
      }
   }
   strcat(sym_name, ".");
   if (is_pin) {
      strcat(sym_name, PIN_FILE_EXT);
   } else {
      strcat(sym_name, SYM_FILE_EXT);
   }
   sprintf(file_name, "%s%c%s", path_name, DIR_SEP, sym_name);

   if ((short_name=IsPrefix(bootDir, file_name, &rest))) ++rest;

   if ((fp=fopen(file_name, "r")) == NULL) {
      sprintf(gszMsgBox, TgLoadString(STID_CANNOT_OPEN_FOR_READ_PUSH),
            (short_name ? rest : file_name));
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return FALSE;
   }

   strcpy(tmp_filefullpath, scanFileFullPath);
   strcpy(tmp_filename, scanFileName);
   tmp_linenum = scanLineNum;
   UtilStrCpyN(scanFileFullPath, sizeof(scanFileFullPath), file_name);
   strcpy(scanFileName, (short_name ? rest : file_name));
   scanLineNum = 0;

   HighLightReverse();

   stk_ptr = SaveFileInfo();
   ResetFileInfo();
   if (usePaperSizeStoredInFile) ResetOnePageSize();

   topStk = stk_ptr;

   sprintf(gszMsgBox, TgLoadCachedString(CSTID_PUSHING_INTO_NAMED),
         (short_name ? rest : file_name));
   Msg(gszMsgBox);
   CleanUpDrawingWindow();
   XClearWindow(mainDisplay, drawWindow);
   somethingHighLighted = FALSE;
   SetFileModified(FALSE);

   SetWatchCursor(drawWindow);
   SetWatchCursor(mainWindow);
   numRedrawBBox = 0;
   ShowInterrupt(1);
   interrupted = FALSE;
   readingPageNum = loadedCurPageNum = 0;
   foundGoodStateObject = FALSE;
   while ((read_status=ReadObj(fp, &obj_ptr)) == TRUE) {
      if (obj_ptr != NULL) {
         obj_ptr->tmp_parent = NULL;
         AdjForOldVersion(obj_ptr);
         AddObj(NULL, topObj, obj_ptr);
         if (!interrupted &&
               (PointInBBox(obj_ptr->x, obj_ptr->y, drawWinBBox) ||
               BBoxIntersect(obj_ptr->bbox, drawWinBBox))) {
            if (!DrawObj(drawWindow, obj_ptr)) interrupted = TRUE;
            if (CheckInterrupt(TRUE)) interrupted = TRUE;
         }
      }
   }
   fclose(fp);

   if (!PRTGIF && colorLayers && needToRedrawColorWindow) {
      RedrawColorWindow();
   }
   if (interrupted) Msg(TgLoadString(STID_USER_INTR_ABORT_DRAW));
   HideInterrupt();

   strcpy(scanFileFullPath, tmp_filefullpath);
   strcpy(scanFileName, tmp_filename);
   scanLineNum = tmp_linenum;

   SetDefaultCursor(mainWindow);
   SetDefaultCursor(drawWindow);

   if (read_status == INVALID) {
      sprintf(gszMsgBox, TgLoadString(STID_FILE_VER_ABORT_PUSH),
            fileVersion, TOOL_NAME, homePageURL);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      DrawPaperBoundary(drawWindow);
      RedrawGridLines(drawWindow);
      return FALSE;
   }
   if (loadedCurPageNum > 0 && curPage != NULL) {
      GotoPageNum(loadedCurPageNum);
   }
   CheckFileAttrsInLoad();

   strcpy(curFileName, sym_name);
   strcpy(curSymDir, path_name);
   curFileDefined = TRUE;
   if (!curDirIsLocal) {
      strcpy(curDir, curLocalDir);
      *curLocalDir = '\0';
      curDirIsLocal = TRUE;
   }

   sprintf(gszMsgBox, TgLoadCachedString(CSTID_CUR_FILE_IS),
         (short_name ? rest : file_name));
   Msg(gszMsgBox);

   RedrawTitleWindow();
   UpdateAllPinnedMenus();
   if (!navigatingBackAndForth) CommitNavigate();
   return foundGoodStateObject;
}

void RestoreFileInfo(StkPtr)
   struct StkRec *StkPtr;
{
   char fname[MAXPATHLENGTH], *rest;

   fileModified = StkPtr->file_mod;
   objId = StkPtr->id;

   CleanUpComments();
   CleanUpDrawingWindow();

   drawOrigX = StkPtr->orig_x;
   drawOrigY = StkPtr->orig_y;
   zoomScale = StkPtr->zoom;
   zoomedIn = StkPtr->zoomed;
   gridSystem = StkPtr->grid_system;
   xyEnglishGrid = StkPtr->english_grid;
   xyMetricGrid = StkPtr->metric_grid;
   snapOn = StkPtr->snap_on;
   colorIndex = StkPtr->color;
   horiAlign = StkPtr->h_align;
   vertAlign = StkPtr->v_align;
   lineWidth = StkPtr->line_w;
   lineStyle = StkPtr->line_s;
   transPat = StkPtr->trans_pat;
   objFill = StkPtr->fill;
   penPat = StkPtr->pen;
   textJust = StkPtr->just;
   textVSpace = StkPtr->v_space;
   curFont = StkPtr->font;
   curStyle = StkPtr->f_style;
   curSzUnit = StkPtr->f_sz_unit;
   printMag = StkPtr->print_mag;
   gridShown = StkPtr->grid_shown;
   moveMode = StkPtr->move_mode;
   rcbRadius = StkPtr->rcb_radius;
   curUnderlineOn = StkPtr->underline_on;
   curUnderlineYOffset = StkPtr->underline_y_offset;
   curOverlineOn = StkPtr->overline_on;
   curOverlineYOffset = StkPtr->overline_y_offset;
   tgifObj->fattr = StkPtr->first_file_attr;
   tgifObj->lattr = StkPtr->last_file_attr;

   CleanUpCmds();
   firstCmd = StkPtr->first_cmd;
   lastCmd = StkPtr->last_cmd;
   curCmd = StkPtr->cur_cmd;
   historyCount = StkPtr->history_count;

   firstPage = StkPtr->first_page;
   lastPage = StkPtr->last_page;
   curPage = StkPtr->cur_page;
   curPageNum = StkPtr->cur_page_num;
   lastPageNum = StkPtr->last_page_num;
   paperCol = StkPtr->cols;
   paperRow = StkPtr->rows;
   pageLayoutMode = StkPtr->page_layout_mode;

   colorDump = StkPtr->color_dump;
   onePageWidth = StkPtr->one_page_width;
   onePageHeight = StkPtr->one_page_height;

   if (usePaperSizeStoredInFile) SetPSPageWidthHeight();

   curPage->top = topObj = StkPtr->first;
   curPage->bot = botObj = StkPtr->last;
   strcpy(curDomainName, StkPtr->domain);
   strcpy(curSymDir, StkPtr->sym_dir);
   curFileDefined = StkPtr->name_valid;
   if (curFileDefined) {
      strcpy(curFileName, StkPtr->name);
      if (*curSymDir == '\0') {
         sprintf(fname, "%s%c%s", StkPtr->dir, DIR_SEP, curFileName);
      } else {
         sprintf(fname, "%s%c%s", curSymDir, DIR_SEP, curFileName);
      }
      if (IsPrefix(bootDir, fname, &rest)) {
         sprintf(gszMsgBox, TgLoadString(STID_POP_BACK_TO_NAMED), ++rest);
      } else {
         sprintf(gszMsgBox, TgLoadString(STID_POP_BACK_TO_NAMED), fname);
      }
      if (!deserializingFile) Msg(gszMsgBox);

      SetCurDir(fname);
   } else {
      if (!deserializingFile) Msg(TgLoadString(STID_POP_BACK_TO_PARENT_UNDEF));
   }

   if (strcmp(curDir, StkPtr->dir) != 0) {
      strcpy(curDir, StkPtr->dir);
      UpdateDirInfo();
   }

   savedComments = StkPtr->saved_comments;
   savedCommentsLen = StkPtr->saved_comments_len;

   ClearBgColorInfo(FALSE);
   ClearBgPixmapInfo(FALSE);

   myFileBgPixel = StkPtr->file_bg_pixel;
   myFileFgPixel = StkPtr->file_fg_pixel;
   myFileBgColorStr = StkPtr->file_bg_color_str;
   myFileFgColorStr = StkPtr->file_fg_color_str;
   myFileBgGifURL = StkPtr->file_bg_gif_url;
   myFileBgPixmapW = StkPtr->file_bg_pixmap_w;
   myFileBgPixmapH = StkPtr->file_bg_pixmap_h;
   myFileBgPixmap = StkPtr->file_bg_pixmap;
   if (myFileBgColorStr != NULL) {
      if (!SetFileBgColor()) {
         ClearBgColorInfo(FALSE);
      }
   } else if (myFileBgGifURL != NULL) {
      if (!SetFileBgPixmap()) {
         ClearBgColorInfo(FALSE);
      }
   }
}

void PopIcon()
{
   if (topStk == NULL) {
      MsgBox(TgLoadString(STID_ALREADY_AT_TOP_LEVEL), TOOL_NAME, INFO_MB);
      return;
   }
   while (fileModified) {
      XBell(mainDisplay, 0);
      switch (MsgBox(TgLoadString(STID_FILE_MOD_SAVE_BEFORE_POP), TOOL_NAME,
            YNC_MB)) {
      case MB_ID_YES: SaveFile(); break;
      case MB_ID_NO: SetFileModified(FALSE); break;
      case MB_ID_CANCEL: return;
      }
   }
   AdjustNavigate();
   RestoreFileInfo(topStk);
   ResetOnePageSize();

   topSel = botSel = (struct SelRec *)malloc(sizeof(struct SelRec));
   if (topSel == NULL) FailAllocMessage();
   topSel->next = NULL;
   topSel->prev = NULL;
   topSel->obj = topStk->sel;
   UpdSelBBox();

   if (UpdPageStyle(topStk->page_style)) {
      UpdDrawWinBBox();
      AdjSplineVs();
   }
   UpdDrawWinWH();
   SetCanvasFont();

   RedrawRulers();
   RedrawScrollBars();
   RedrawChoiceWindow();
   RedrawTitleWindow();
   UpdDrawWinBBox();
   UpdateAllPinnedMenus();

   free(topStk);
   topStk = topStk->next;
   ClearAndRedrawDrawWindow();
   XSync(mainDisplay, True);
   justDupped = FALSE;
}

void CleanUpTgifObj()
{
   if (tgifObj != NULL) {
      DelAllAttrs(tgifObj->fattr);
      FreeBoxObj(tgifObj);
   }
   tgifObj = NULL;
}

void CleanUpStk()
{
   register struct StkRec *next_stk;

   for ( ; topStk != NULL; topStk = next_stk) {
      next_stk = topStk->next;
      firstPage = topStk->first_page;
      lastPage = topStk->last_page;
      CleanUpPage();
      free(topStk);
   }
   curSymDir[0] = '\0';
   CleanUpTgifObj();
}
