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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/page.c,v 1.15 2011/05/16 16:21:58 william Exp $
 */

#define _INCLUDE_FROM_PAGE_C_

#include "tgifdefs.h"
#include "expfdefs.h"

#include "attr.e"
#include "auxtext.e"
#include "button.e"
#include "cmd.e"
#include "choice.e"
#include "choose.e"
#include "color.e"
#include "cursor.e"
#include "dialog.e"
#include "drawing.e"
#include "dup.e"
#include "file.e"
#include "font.e"
#include "grid.e"
#include "mark.e"
#include "mainloop.e"
#include "mainmenu.e"
#include "menu.e"
#include "menuinfo.e"
#include "move.e"
#include "msg.e"
#include "names.e"
#include "navigate.e"
#include "obj.e"
#include "page.e"
#include "raster.e"
#include "rect.e"
#include "scroll.e"
#include "select.e"
#include "setup.e"
#include "stk.e"
#include "strtbl.e"
#include "text.e"
#include "util.e"
#include "wb.e"
#include "xpixmap.e"
#include "xprtfltr.e"

#include "xbm/leftend.xbm"

struct PageRec	*firstPage=NULL, *lastPage=NULL, *curPage=NULL;
int		curPageNum=1, lastPageNum=1;
int		paperRow=1, paperCol=1;
int		pageLayoutMode=PAGE_STACK;
int		pageLineShownInTileMode=TRUE;

static int pageWindowFirstIndex=1;

static int gnPageNumOnTab=TRUE;

static
int DrawAHorizontalTab(page_num, page_ptr, x, y, on_top, skip)
   int page_num, x, y, on_top, skip;
   struct PageRec *page_ptr;
{
   char s[20];
   XPoint v[5];
   int w=0, num_digits=0, x_offset, y_offset;
   int bg_pixel=(threeDLook ? myLtGryPixel : myBgPixel);

   if (gnPageNumOnTab) {
      sprintf(s, "%1d", page_num);
      num_digits = strlen(s);
      w = (num_digits+2)*rulerFontWidth;
   } else {
      /* not implemented yet */
      if (page_ptr->name == NULL) {
      } else {
      }
      w = (num_digits+2)*rulerFontWidth;
   }
   if (skip) return w;

   v[0].x = v[4].x = x;         v[0].y = v[4].y = y-1;
   v[1].x = x+rulerFontWidth;   v[1].y = y+2+rulerFontHeight;
   v[2].x = x+w;                v[2].y = y+2+rulerFontHeight;
   v[3].x = x+w+rulerFontWidth; v[3].y = y-1;

   XSetForeground(mainDisplay, defaultGC, (on_top ? myBgPixel: bg_pixel));
   XFillPolygon(mainDisplay, pageWindow, defaultGC, v, 5, Convex,
         CoordModeOrigin);
   XSetForeground(mainDisplay, defaultGC, myFgPixel);
   if (on_top) {
      XDrawLines(mainDisplay, pageWindow, defaultGC, v, 4, CoordModeOrigin);
      XSetForeground(mainDisplay, defaultGC, myBgPixel);
      XDrawLines(mainDisplay, pageWindow, defaultGC, &v[3], 2, CoordModeOrigin);
      XSetForeground(mainDisplay, defaultGC, myFgPixel);
   } else {
      XDrawLines(mainDisplay, pageWindow, defaultGC, v, 5, CoordModeOrigin);
   }
   y_offset = ((rulerFontHeight-7)>>1)+1;
   x_offset = (rulerFontWidth>>1)+rulerFontWidth;
   XDrawString(mainDisplay, pageWindow, defaultGC, x+x_offset,
         y+1+rulerFontAsc, s, num_digits);
   XSetForeground(mainDisplay, defaultGC, myFgPixel);
   return w;
}

static
int MaxPageTabsWidth()
{
   int i=0, total=0;

   for (i=1; i <= lastPageNum; i++) {
      total += DrawAHorizontalTab(i, NULL, 0, 0, FALSE, TRUE);
   }
   return total;
}

static
int PageTabsVisible(pn_last_tab_visible)
   int *pn_last_tab_visible;
{
   int i=0, total=pageWindowW-(scrollBarW<<2)-1;
   int last_tab_visible=pageWindowFirstIndex;

   for (i=pageWindowFirstIndex; i <= lastPageNum; i++) {
      total -= DrawAHorizontalTab(i, NULL, 0, 0, FALSE, TRUE);
      if (total < 0) {
         break;
      }
      last_tab_visible = i;
   }
   if (pn_last_tab_visible != NULL) {
      *pn_last_tab_visible = last_tab_visible;
   }
   if (curPageNum >= pageWindowFirstIndex &&
         curPageNum <= last_tab_visible) {
      return TRUE;
   }
   return FALSE;
}

static
void MakePageTabsVisible(last_tab_visible)
   int last_tab_visible;
{
   int total=pageWindowW-(scrollBarW<<2)-1;

   if (MaxPageTabsWidth() <= total) {
      pageWindowFirstIndex = 1;
      return;
   }
   if (curPageNum < pageWindowFirstIndex) {
      while (pageWindowFirstIndex > 1) {
         pageWindowFirstIndex--;
         if (PageTabsVisible(NULL)) {
            return;
         }
      }
   } else if (curPageNum >= pageWindowFirstIndex &&
         curPageNum <= last_tab_visible) {
      return;
   } else {
      while (pageWindowFirstIndex <= lastPageNum) {
         pageWindowFirstIndex++;
         if (PageTabsVisible(NULL)) {
            return;
         }
      }
#ifdef _TGIF_DBG /* debug, do not translate */
      TgAssert(pageWindowFirstIndex > lastPageNum,
            "pageWindowFirstIndex > lastPageNum in MakePageTabsVisible()", NULL);
#endif /* _TGIF_DBG */
      pageWindowFirstIndex = curPageNum;
   }
}

void RedrawPageWindow()
{
   int i=0, x=0, y=1;
   struct PageRec *page_ptr=NULL;

   XClearArea(mainDisplay, pageWindow, 0, 0, pageWindowW,
         scrollBarW+(brdrW<<1), FALSE);
   XDrawRectangle(mainDisplay, pageWindow, defaultGC, 0, 0, pageWindowW-1,
         scrollBarW+(brdrW<<1)-1);
   if (pageLayoutMode == PAGE_TILE) return;

   x = 1 + (leftend_width<<2);
   XSetFont(mainDisplay, defaultGC, rulerFontPtr->fid);
   for (i=1, page_ptr=firstPage; page_ptr != NULL;
         page_ptr=page_ptr->next, i++) {
      if (i >= pageWindowFirstIndex) {
         int w=0;

         if (i == curPageNum) {
            w = DrawAHorizontalTab(i, page_ptr, x, y, TRUE, TRUE);
         } else {
            w = DrawAHorizontalTab(i, page_ptr, x, y, FALSE, FALSE);
         }
         x += w;
      }
   }
   x = 1 + (leftend_width<<2);
   for (i=1, page_ptr=firstPage; page_ptr != NULL;
         page_ptr=page_ptr->next, i++) {
      if (i >= pageWindowFirstIndex) {
         int w=0;

         if (i == curPageNum) {
            w = DrawAHorizontalTab(i, page_ptr, x, y, TRUE, FALSE);
            break;
         } else {
            w = DrawAHorizontalTab(i, page_ptr, x, y, FALSE, TRUE);
         }
         x += w;
      }
   }
   XSetFont(mainDisplay, defaultGC, defaultFontPtr->fid);

   y = (threeDLook ? (windowPadding>>1) : brdrW);
   for (i=0, x=y; i < 4; i++, x += leftend_width) {
      XSetTSOrigin(mainDisplay, rasterGC, x, y);
      XSetStipple(mainDisplay, rasterGC, scrollPixmap[i]);
      XFillRectangle(mainDisplay, pageWindow, rasterGC,
            x, y, leftend_width, leftend_height);
      if (threeDLook) {
         struct BBRec bbox;

         SetBBRec(&bbox, x, y, x+leftend_height, y+leftend_height);
         TgDrawThreeDButton(mainDisplay, pageWindow, textMenuGC, &bbox,
               TGBS_RAISED, 1, FALSE);
      }
   }
   XSetTSOrigin(mainDisplay, rasterGC, 0, 0);
}

void MakeCurPageTabVisible()
{
   int last_tab_visible=(-1);

   if (pageLayoutMode == PAGE_TILE) return;

   if (!PageTabsVisible(&last_tab_visible)) {
      MakePageTabsVisible(last_tab_visible);
   }
}

void GotoPageNum(new_page_num)
   int new_page_num;
{
   int i=1;
   struct PageRec *page_ptr=NULL;

   if (new_page_num <= 0 || curPage == NULL) return;

   curPage->draw_orig_x = drawOrigX;
   curPage->draw_orig_y = drawOrigY;
   curPage->zoom_scale = zoomScale;
   curPage->zoomed_in = zoomedIn;

   for (page_ptr=firstPage; page_ptr != NULL; page_ptr=page_ptr->next, i++) {
      if (i == new_page_num) {
         break;
      }
   }
   curPageNum = new_page_num;
   curPage = page_ptr;
   topObj = curPage->top;
   botObj = curPage->bot;
   if (curPage->draw_orig_x != drawOrigX || curPage->draw_orig_y != drawOrigY ||
         curPage->zoom_scale != zoomScale || curPage->zoomed_in != zoomedIn) {
      if (!PRTGIF) AdjSplineVs();
      if (!PRTGIF) AdjCaches();
      curPage->draw_orig_x = drawOrigX;
      curPage->draw_orig_y = drawOrigY;
      curPage->zoom_scale = zoomScale;
      curPage->zoomed_in = zoomedIn;
   }
   MakeCurPageTabVisible();
}

static int curChoiceBeforePageChange=NOTHING;

void SetCurPage(page_number)
   int page_number;
{
   int need_to_redraw=TRUE;

   if (pageLayoutMode == PAGE_TILE) return;

   MakeQuiescent();
   if (gstWBInfo.do_whiteboard) {
      /* No warning!  Just clear the whiteboard! */
      RecordWBClearAll();
   } else {
      PrepareToRecord(CMD_GOTO_PAGE, NULL, NULL, curPageNum);
   }
   GotoPageNum(page_number);
   if (!gstWBInfo.do_whiteboard) {
      RecordCmd(CMD_GOTO_PAGE, NULL, NULL, NULL, curPageNum);
   }
   if (resetOriginOnAdvancePage) {
      int saved_x=0, saved_y=0, saved_zoom_scale=0, saved_zoomed_in=0;

      GetOrigin(&saved_x, &saved_y, &saved_zoom_scale, &saved_zoomed_in);
      SetOrigin(0, 0, 0, FALSE);
      if (ScrollToOrigin()) {
         /* scrolled or un-zoomed */
         need_to_redraw = FALSE;
      }
      SetOrigin(saved_x, saved_y, saved_zoom_scale, saved_zoomed_in);
   }
   if (need_to_redraw) {
      ClearAndRedrawDrawWindow();
   }
   RedrawTitleWindow();
   MakeCurPageTabVisible();
   ShowPage();
   SetCurChoice(curChoiceBeforePageChange);
   if (gstWBInfo.do_whiteboard && topObj != NULL) {
#ifdef _TGIF_WB2_NO_DELAY_BUG
      sleep(1);
#endif /* ~_TGIF_WB2_NO_DELAY_BUG */
      /* 
       * In slideshow mode, SelAllObj() will *not* select anything!
       * Since we really want the objects to be selected (because
       *         we want to record them), we must set ignore_slideshow
       *         to TRUE when we call SelAllObj().
       */
      SelAllObj(FALSE, TRUE);
      PrepareToRecord(CMD_NEW, NULL, NULL, 0);
      RecordCmd(CMD_NEW, NULL, topSel, botSel, numObjSelected);
      RemoveAllSel();
   }
}

static
void HandleClickInPageWindow(button_ev)
   XButtonEvent *button_ev;
{
   int i, index=(int)(button_ev->x / scrollBarW), total, offset;
   struct PageRec *page_ptr;

   if (button_ev->x <= 0) {
      index = 0;
   } else {
      index = (int)((button_ev->x-1) / scrollBarW);
   }
   switch (index) {
   case SCROLL_LEFTEND:
      pageWindowFirstIndex = 1;
      RedrawPageWindow();
      break;
   case SCROLL_LEFT:
      if (pageWindowFirstIndex > 1) {
         if (button_ev->state & (ShiftMask | ControlMask)) {
            pageWindowFirstIndex -= 10;
            if (pageWindowFirstIndex < 1) pageWindowFirstIndex = 1;
         } else {
            pageWindowFirstIndex--;
         }
         RedrawPageWindow();
      }
      break;
   case SCROLL_RIGHT:
      if (pageWindowFirstIndex < lastPageNum) {
         if (button_ev->state & (ShiftMask | ControlMask)) {
            pageWindowFirstIndex += 10;
            if (pageWindowFirstIndex > lastPageNum) {
               pageWindowFirstIndex = lastPageNum;
            }
         } else {
            pageWindowFirstIndex++;
         }
         RedrawPageWindow();
      }
      break;
   case SCROLL_RIGHTEND:
      total = pageWindowW-(scrollBarW<<2)-1;
      pageWindowFirstIndex = lastPageNum;
      for (i=lastPageNum, page_ptr=lastPage; page_ptr != NULL;
            page_ptr=page_ptr->prev, i--) {
         int w=DrawAHorizontalTab(i, page_ptr, 0, 0, FALSE, TRUE);

         total -= w;
         if (total > 0) {
            pageWindowFirstIndex = i;
         } else {
            break;
         }
      }
      RedrawPageWindow();
      break;
   default:
      offset = button_ev->x-(scrollBarW<<2)-1;
      for (i=1, page_ptr=firstPage; page_ptr != NULL;
            page_ptr=page_ptr->next, i++) {
         if (i >= pageWindowFirstIndex) {
            int w=DrawAHorizontalTab(i, page_ptr, 0, 0, FALSE, TRUE);

            if (w >= offset) {
               /* int start=(((rulerFontWidth<<1)-7)>>1)+rulerFontWidth; */

               if (page_ptr != curPage) {
                  curChoiceBeforePageChange = curChoice;
                  SetCurPage(i);
               }
               break;
            }
            offset -= w;
         }
      }
      break;
   }
}

static
void HandleMotionInPageWindow(motion_ev)
   XMotionEvent *motion_ev;
{
   int i=0, index=0, offset=0, left_stid=0;
   struct PageRec *page_ptr=NULL;

   if (motion_ev->x <= 0) {
      index = 0;
   } else {
      index = (int)((motion_ev->x-1) / scrollBarW);
   }
   switch (index) {
   case SCROLL_LEFTEND:
   case SCROLL_LEFT:
   case SCROLL_RIGHT:
   case SCROLL_RIGHTEND:
      if (motion_ev->state & (ShiftMask | ControlMask)) {
         switch (index) {
         case SCROLL_LEFTEND: left_stid = CSTID_SHIFT_ALL_TABS_RIGHT; break;
         case SCROLL_LEFT: left_stid = CSTID_SHIFT_10_TABS_RIGHT; break;
         case SCROLL_RIGHT: left_stid = CSTID_SHIFT_10_TABS_LEFT; break;
         case SCROLL_RIGHTEND: left_stid = CSTID_SHIFT_ALL_TABS_LEFT; break;
         }
      } else {
         switch (index) {
         case SCROLL_LEFTEND: left_stid = CSTID_SHIFT_ALL_TABS_RIGHT; break;
         case SCROLL_LEFT: left_stid = CSTID_SHIFT_TABS_RIGHT; break;
         case SCROLL_RIGHT: left_stid = CSTID_SHIFT_TABS_LEFT; break;
         case SCROLL_RIGHTEND: left_stid = CSTID_SHIFT_ALL_TABS_LEFT; break;
         }
      }
      SetMouseStatus(TgLoadCachedString(left_stid),
            TgLoadCachedString(CSTID_PAGE_MENU),
            TgLoadCachedString(CSTID_PARANED_NONE));
      break;
   default:
      offset = motion_ev->x-(scrollBarW<<2)-1;
      for (i=1, page_ptr=firstPage; page_ptr != NULL;
            page_ptr=page_ptr->next, i++) {
         if (i >= pageWindowFirstIndex) {
            int w=DrawAHorizontalTab(i, page_ptr, 0, 0, FALSE, TRUE);

            if (w >= offset) {
               /* int start=(((rulerFontWidth<<1)-7)>>1)+rulerFontWidth; */

               if (page_ptr->name == NULL) {
                  sprintf(gszMsgBox, TgLoadCachedString(CSTID_GOTO_GIVEN_PAGE),
                        i);
               } else {
                  sprintf(gszMsgBox,
                        TgLoadCachedString(CSTID_GOTO_GIVEN_PAGE_WITH_NAME), i,
                        page_ptr->name);
               }
               SetStringStatus(gszMsgBox);
               break;
            }
            offset -= w;
         }
      }
      if (page_ptr == NULL) {
         SetMouseStatus(TgLoadCachedString(CSTID_PARANED_NONE),
               TgLoadCachedString(CSTID_PAGE_MENU),
               TgLoadCachedString(CSTID_PARANED_NONE));
      }
      break;
   }
}

void RedrawPageDummyWindow()
{
   XClearWindow(mainDisplay, pageDummyWindow);
   if (threeDLook) {
      struct BBRec bbox;

      SetBBRec(&bbox, 0, 0, pageDummyWindowW, scrollBarW);
      TgDrawThreeDButton(mainDisplay, pageDummyWindow, textMenuGC, &bbox,
            TGBS_RAISED, 2, FALSE);
   }
}

void PageDummyEventHandler(input)
   XEvent *input;
{
   if (input->type == Expose) {
      XEvent ev;

      while (XCheckWindowEvent(mainDisplay,pageDummyWindow,ExposureMask,&ev)) ;
      RedrawPageDummyWindow();
   } else if (input->type == EnterNotify) {
      SetMouseStatusToAllNone();
   }
}

static
void FreePage(page_ptr)
   struct PageRec *page_ptr;
{
   topObj = page_ptr->top;
   botObj = page_ptr->bot;
   DelAllObj();
   if (page_ptr->name != NULL) free(page_ptr->name);
   if (page_ptr->page_file_name != NULL) free(page_ptr->page_file_name);
   free(page_ptr);
   topObj = NULL;
   botObj = NULL;
}

static
void InitPageOnlyOnce()
{
   static int initialized=FALSE;

   if (initialized) return;
   initialized = TRUE;

   if (PRTGIF) return;
}

void InitPage()
   /* given lastPageNum, allocate enough pages */
{
   int i;

   InitPageOnlyOnce();
   while (firstPage != NULL) {
      curPage = firstPage->next;
      FreePage(firstPage);
      firstPage = curPage;
   }
   firstPage = lastPage = curPage = NULL;
   for (i=1; i <= lastPageNum; i++) {
      curPage = (struct PageRec *)malloc(sizeof(struct PageRec));
      if (curPage == NULL) FailAllocMessage();
      memset(curPage, 0, sizeof(struct PageRec));
      curPage->layer_on = TRUE;
      curPage->name = NULL;
      curPage->page_file_name = NULL;
      curPage->top = curPage->bot = topObj = botObj = NULL;
      curPage->next = NULL;
      curPage->prev = lastPage;
      curPage->draw_orig_x = drawOrigX;
      curPage->draw_orig_y = drawOrigY;
      curPage->zoom_scale = zoomScale;
      curPage->zoomed_in = zoomedIn;
      if (firstPage == NULL) {
         firstPage = curPage;
      } else {
         lastPage->next = curPage;
      }
      lastPage = curPage;
   }
   curPageNum = (lastPageNum > 0 ? 1 : 0);
   curPage = (lastPageNum > 0 ? firstPage : NULL);
}

void NextPage()
{
   if (pageLayoutMode == PAGE_TILE) {
      MsgBox(TgLoadString(STID_CANT_DO_NEXTPAGE_IN_TILED), TOOL_NAME, INFO_MB);
      return;
   }
   if (curPageNum == lastPageNum) {
      if (!inSlideShow) {
         MsgBox(TgLoadString(STID_ALREADY_AT_LAST_PAGE), TOOL_NAME, INFO_MB);
      }
      return;
   }
   curChoiceBeforePageChange = curChoice;
   SetCurPage(curPageNum+1);
}

void PrevPage()
{
   if (pageLayoutMode == PAGE_TILE) {
      MsgBox(TgLoadString(STID_CANT_DO_PREVPAGE_IN_TILED), TOOL_NAME, INFO_MB);
      return;
   }
   if (curPageNum == 1) {
      if (!inSlideShow) {
         MsgBox(TgLoadString(STID_ALREADY_AT_FIRST_PAGE), TOOL_NAME, INFO_MB);
      }
      return;
   }
   curChoiceBeforePageChange = curChoice;
   SetCurPage(curPageNum-1);
}

static int	leadingChars=(-1);
static char	formatStr[20];
static DspList	*pageNameDspPtr=NULL;

static
DspList *PageNameListing(pn_entries)
   int *pn_entries;
{
   int i;
   DspList *dsp_ptr=NULL;
   struct PageRec *page_ptr=NULL;

   for (leadingChars=1, i=lastPageNum; ;leadingChars++, i /= 10) {
      if (i < 10) {
         break;
      }
   }
   sprintf(formatStr, "%%%1dd %%s", leadingChars++);
   pageNameDspPtr = (DspList*)malloc(lastPageNum*sizeof(DspList));
   if (pageNameDspPtr == NULL) FailAllocMessage();
   memset(pageNameDspPtr, 0, lastPageNum*sizeof(DspList));
   for (i=1, dsp_ptr=pageNameDspPtr, page_ptr=firstPage; i <= lastPageNum;
         i++, dsp_ptr++, page_ptr=page_ptr->next) {
      sprintf(gszMsgBox, formatStr, i,
            ((page_ptr->name == NULL) ? "" : page_ptr->name));
      UtilStrCpyN(dsp_ptr->itemstr, sizeof(dsp_ptr->itemstr), gszMsgBox);
      dsp_ptr->directory = FALSE;
      dsp_ptr->next = ((i == lastPageNum) ? NULL : &dsp_ptr[1]);
   }
   *pn_entries = lastPageNum;
   return pageNameDspPtr;
}

static
int EditOrSelectPageNames(TopStr, Which, entries, num_entries)
   char *TopStr, **entries;
   int Which; /* either PAGE_GOTO or PAGE_NAME */
   int num_entries;
{
   int marked_index=curPageNum-1, rc=INVALID;
   char win_name[128];

   ResetNamesInfo();
   NamesSetTitle(TopStr);
   NamesAddButton(TgLoadCachedString(CSTID_OK), BUTTON_OK);
   NamesAddButton(TgLoadCachedString(CSTID_CANCEL), BUTTON_CANCEL);
   NamesSetEntries(NULL, entries, num_entries, NULL, TRUE, marked_index,
         leadingChars);
   if (Which==PAGE_NAME) {
      /* ignore double-click and <CR> */
      NamesSetDefaultBtnId(INVALID, INVALID);
      sprintf(win_name, TgLoadString(STID_TOOL_NAME_PAGES), TOOL_NAME);
      NamesSetStyle(NAMES_EDIT_NAME, NAMES_LOOP_ONCE);
      if (Names(win_name, NULL, NULL, 0, NULL) == BUTTON_OK) {
         rc = TRUE;
      }
   } else {
      int selected_index=INVALID;

      sprintf(win_name, TgLoadString(STID_TOOL_GOTO_PAGE), TOOL_NAME);
      NamesSetStyle(NAMES_COMPLEX_SELECT_NAME, NAMES_LOOP_ONCE);
      if (Names(win_name, &selected_index, NULL, 0, NULL) != INVALID) {
         return selected_index;
      }
   }
   return rc;
}

void GotoPage(psz_page_num)
   char *psz_page_num;
{
   int index=(-1);

   if (pageLayoutMode == PAGE_TILE) {
      MsgBox(TgLoadString(STID_CANT_DO_GOTOPAGE_IN_TILED), TOOL_NAME, INFO_MB);
      return;
   }
   curChoiceBeforePageChange = curChoice;
   MakeQuiescent();

   if (psz_page_num != NULL && strcmp(psz_page_num, "-1") != 0) {
      index = atoi(psz_page_num);
   } else {
      int num_entries=0;
      DspList *dsp_ptr;
      char **entries=NULL;

      dsp_ptr = PageNameListing(&num_entries);
      ignoreDirectoryFlag = TRUE;
      entries = MakeNameDspItemArray(num_entries, dsp_ptr);
      ignoreDirectoryFlag = FALSE;

      index = EditOrSelectPageNames(TgLoadString(STID_GOTO_PAGE_DOTS),
            PAGE_GOTO, entries, num_entries);

      free(dsp_ptr);
      free(*entries);
      free(entries);

      if (index++ == (-1)) {
         Msg(TgLoadString(STID_NO_PAGE_SELECTED));
         return;
      }
   }
   if (index < 1 || index > lastPageNum) {
      sprintf(gszMsgBox, TgLoadString(STID_INVALID_GIVEN_PAGE_NUMBER), index);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return;
   }
   if (psz_page_num == NULL && index == curPageNum) {
      sprintf(gszMsgBox, TgLoadString(STID_ALREADY_AT_GIVEN_PAGE), curPageNum);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return;
   }
   SetCurPage(index);

   sprintf(gszMsgBox, TgLoadString(STID_CUR_PAGE_IS_GIVEN_PAGE_NUM),
         curPageNum);
   Msg(gszMsgBox);
}

void NamePages()
{
   int i, num_entries=0;
   DspList *dsp_ptr;
   char **entries=NULL;

   if (pageLayoutMode == PAGE_TILE) {
      MsgBox(TgLoadString(STID_CANT_DO_NAMEPAGES_IN_TILED), TOOL_NAME, INFO_MB);
      return;
   }
   MakeQuiescent();

   dsp_ptr = PageNameListing(&num_entries);
   ignoreDirectoryFlag = TRUE;
   entries = MakeNameDspItemArray(num_entries, dsp_ptr);
   ignoreDirectoryFlag = FALSE;
   if (EditOrSelectPageNames(TgLoadString(STID_EDIT_PAGE_NAMES_DOTS), PAGE_NAME,
         entries, num_entries)) {
      int modified=FALSE;
      struct PageRec *page_ptr;

      for (page_ptr=firstPage, i=0; page_ptr!=NULL;
            page_ptr=page_ptr->next, i++) {
         int blank_str=BlankStr(&(entries[i])[leadingChars]);

         if (page_ptr->name == NULL) {
            if (!blank_str) {
               modified = TRUE;
               page_ptr->name = UtilStrDup(&(entries[i])[leadingChars]);
               if (page_ptr->name == NULL) FailAllocMessage();
            }
         } else {
            if (blank_str || strcmp(page_ptr->name,
                  &(entries[i])[leadingChars]) != 0) {
               modified = TRUE;
               UtilFree(page_ptr->name);
               if (blank_str) {
                  page_ptr->name = NULL;
               } else {
                  page_ptr->name = UtilStrDup(&(entries[i])[leadingChars]);
                  if (page_ptr->name == NULL) FailAllocMessage();
               }
            }
         }
      }
      if (modified) {
         SetFileModified(TRUE);
         RedrawTitleWindow();
      }
   }
   free(dsp_ptr);
   free(*entries);
   free(entries);
   Msg("");
   SetCurChoice(curChoiceBeforeMakeQuiescent);
}

static
DspList *PageFileNameListing(pn_entries)
   int *pn_entries;
{
   int i;
   DspList *dsp_ptr=NULL;
   struct PageRec *page_ptr=NULL;

   for (leadingChars=1, i=lastPageNum; ;leadingChars++, i /= 10) {
      if (i < 10) {
         break;
      }
   }
   sprintf(formatStr, "%%%1dd %%s", leadingChars++);
   pageNameDspPtr = (DspList*)malloc(lastPageNum*sizeof(DspList));
   if (pageNameDspPtr == NULL) FailAllocMessage();
   memset(pageNameDspPtr, 0, lastPageNum*sizeof(DspList));
   for (i=1, dsp_ptr=pageNameDspPtr, page_ptr=firstPage; i <= lastPageNum;
         i++, dsp_ptr++, page_ptr=page_ptr->next) {
      char *psz=NULL;

      sprintf(gszMsgBox, formatStr, i,
            ((page_ptr->page_file_name == NULL) ? "" :
            page_ptr->page_file_name));
      UtilTrimBlanks(&gszMsgBox[leadingChars]);
      if ((psz=strchr(&gszMsgBox[leadingChars], ' ')) != NULL) *psz = '\0';
      UtilStrCpyN(dsp_ptr->itemstr, sizeof(dsp_ptr->itemstr), gszMsgBox);
      dsp_ptr->directory = FALSE;
      dsp_ptr->next = ((i == lastPageNum) ? NULL : &dsp_ptr[1]);
   }
   *pn_entries = lastPageNum;
   return pageNameDspPtr;
}

void EditPageFileNames()
{
   int i, num_entries=0;
   DspList *dsp_ptr=NULL;
   char **entries=NULL;

   if (pageLayoutMode == PAGE_TILE) {
      MsgBox(TgLoadString(STID_CANT_EDITPAGEFNAME_IN_TILED), TOOL_NAME,
            INFO_MB);
      return;
   }
   MakeQuiescent();

   dsp_ptr = PageFileNameListing(&num_entries);
   ignoreDirectoryFlag = TRUE;
   entries = MakeNameDspItemArray(num_entries, dsp_ptr);
   ignoreDirectoryFlag = FALSE;
   if (EditOrSelectPageNames(TgLoadString(STID_EDIT_PAGE_FILE_NAMES_DOTS),
         PAGE_NAME, entries, num_entries)) {
      int modified=FALSE;
      struct PageRec *page_ptr=NULL;
      char *psz=NULL;

      for (page_ptr=firstPage, i=0; page_ptr!=NULL;
            page_ptr=page_ptr->next, i++) {
         int blank_str=BlankStr(&(entries[i])[leadingChars]);

         if (page_ptr->page_file_name == NULL) {
            if (!blank_str) {
               modified = TRUE;
               page_ptr->page_file_name =
                     UtilStrDup(&(entries[i])[leadingChars]);
               if (page_ptr->page_file_name == NULL) FailAllocMessage();
               if ((psz=strchr(page_ptr->page_file_name, ' ')) != NULL) {
                  char truncated_name[MAXSTRING];

                  *psz = *truncated_name = '\0';
                  UtilStrCpyN(truncated_name, sizeof(truncated_name),
                        page_ptr->page_file_name);
                  *psz = ' ';
                  sprintf(gszMsgBox,
                        TgLoadString(STID_PAGE_FNAME_NO_SPACE_TRUNC),
                        page_ptr->page_file_name, truncated_name);
                  MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
                  strcpy(page_ptr->page_file_name, truncated_name);
               }
            }
         } else {
            if (blank_str || strcmp(page_ptr->page_file_name,
                  &(entries[i])[leadingChars]) != 0) {
               modified = TRUE;
               UtilFree(page_ptr->page_file_name);
               if (blank_str) {
                  page_ptr->page_file_name = NULL;
               } else {
                  page_ptr->page_file_name =
                        UtilStrDup(&(entries[i])[leadingChars]);
                  if (page_ptr->page_file_name == NULL) FailAllocMessage();
                  if ((psz=strchr(page_ptr->page_file_name, ' ')) != NULL) {
                     char truncated_name[MAXSTRING];

                     *psz = *truncated_name = '\0';
                     UtilStrCpyN(truncated_name, sizeof(truncated_name),
                           page_ptr->page_file_name);
                     *psz = ' ';
                     sprintf(gszMsgBox,
                           TgLoadString(STID_PAGE_FNAME_NO_SPACE_TRUNC),
                           page_ptr->page_file_name, truncated_name);
                     MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
                     strcpy(page_ptr->page_file_name, truncated_name);
                  }
               }
            }
         }
      }
      if (modified) {
         SetFileModified(TRUE);
         RedrawTitleWindow();
      }
   }
   free(dsp_ptr);
   free(*entries);
   free(entries);
   Msg("");
   SetCurChoice(curChoiceBeforeMakeQuiescent);
}

int OkToFlushUndoBuffer(msg_prefix)
   char *msg_prefix;
{
   if (firstCmd != NULL) {
      int len=strlen(msg_prefix), rc=0;
      char *buf=(char*)malloc((len+MAXSTRING)*sizeof(char));

      if (buf == NULL) FailAllocMessage();
      XBell(mainDisplay, 0);
      sprintf(buf, "%s\n\n%s",
            msg_prefix, TgLoadString(STID_OK_TO_FLUSH_UNDO_AND_PROCEED));
      rc = MsgBox(buf, TOOL_NAME, YNC_MB);
      UtilFree(buf);
      if (rc == MB_ID_YES) {
         CleanUpCmds();
         return TRUE;
      }
      return FALSE;
   }
   return TRUE;
}

#define BEFORE_CUR_PAGE 0
#define AFTER_CUR_PAGE 1

typedef struct tagAddPageCopyAllInfo {
   struct ObjRec *top_obj;
   struct ObjRec *bot_obj;
} AddPageCopyAllInfo;

static
void AddPage(AfterCurPage, papcai)
   int AfterCurPage;
   AddPageCopyAllInfo *papcai;
{
   int n;
   struct PageRec *page_ptr=NULL;
   struct AttrRec *attr_ptr=NULL;

   if (pageLayoutMode == PAGE_TILE) {
      MsgBox(TgLoadString(STID_CANT_DO_ADDPAGE_IN_TILED), TOOL_NAME, INFO_MB);
      return;
   }
   if (firstCmd != NULL) {
      sprintf(gszMsgBox, TgLoadString(AfterCurPage ?
            STID_ADD_PAGE_AFTER_GIVEN_FLUSH : STID_ADD_PAGE_BEFORE_GIVEN_FLUSH),
            curPageNum);
      if (!OkToFlushUndoBuffer(gszMsgBox)) {
         return;
      }
      CleanUpCmds();
   }
   n = (AfterCurPage ? curPageNum : curPageNum-1);

   MakeQuiescent();

   for (curPageNum=1, curPage=firstPage; curPageNum <= n; curPageNum++) {
      curPage=curPage->next;
   }
   page_ptr = (struct PageRec *)malloc(sizeof(struct PageRec));
   if (page_ptr == NULL) FailAllocMessage();
   memset(page_ptr, 0, sizeof(struct PageRec));
   page_ptr->layer_on = TRUE;
   page_ptr->top = page_ptr->bot = topObj = botObj = NULL;
   page_ptr->next = curPage;
   page_ptr->name = NULL;
   page_ptr->page_file_name = NULL;
   page_ptr->draw_orig_x = drawOrigX;
   page_ptr->draw_orig_y = drawOrigY;
   page_ptr->zoom_scale = zoomScale;
   page_ptr->zoomed_in = zoomedIn;
   if (curPage == NULL) {
      page_ptr->prev = lastPage;
      lastPage->next = page_ptr;
      lastPage = page_ptr;
   } else {
      page_ptr->prev = curPage->prev;
      if (curPage->prev == NULL) {
         firstPage = page_ptr;
      } else {
         curPage->prev->next = page_ptr;
      }
      curPage->prev = page_ptr;
   }
   curPage = page_ptr;
   lastPageNum++;
   topObj = curPage->top;
   botObj = curPage->bot;

   if (papcai != NULL) {
      struct ObjRec *obj_ptr=NULL;
      struct SelRec *top_sel=NULL, *bot_sel=NULL;

      for (obj_ptr=papcai->bot_obj; obj_ptr != NULL; obj_ptr=obj_ptr->prev) {
         obj_ptr->tmp_parent = NULL;
         AddObjIntoSel(obj_ptr, NULL, topSel, &topSel, &botSel);
      }
      DupTheseObjects(topSel, botSel, &top_sel, &bot_sel);
      RemoveAllSel();
      curPage->top = topObj = top_sel->obj;
      curPage->bot = botObj = bot_sel->obj;
      topSel = top_sel;
      botSel = bot_sel;
      RemoveAllSel();
   }
   ClearAndRedrawDrawWindow();
   RedrawTitleWindow();
   SetFileModified(TRUE);
   MakeCurPageTabVisible();
   ShowPage();
   if (papcai == NULL && !cmdLineMerge) {
      if ((attr_ptr=FindAttrWithName(tgifObj, "template=", NULL)) != NULL) {
         char file_name[MAXPATHLENGTH];

         if (*attr_ptr->attr_value.s == DIR_SEP) {
            UtilStrCpyN(file_name, sizeof(file_name), attr_ptr->attr_value.s);
         } else {
            sprintf(file_name, "%s%c%s", bootDir, DIR_SEP,
               attr_ptr->attr_value.s);
         }
         importingFile = TRUE;
         if (ImportGivenFile(file_name, TRUE, TRUE)) {
            sprintf(gszMsgBox, TgLoadString(STID_NAMED_TEMPLATE_LOADED),
                  attr_ptr->attr_value.s);
            Msg(gszMsgBox);
         } else {
            sprintf(gszMsgBox, TgLoadString(STID_PROB_LOADING_NAMED_TEMPLATE),
                  file_name);
            MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
         }
         importingFile = FALSE;
      }
   }
   sprintf(gszMsgBox, TgLoadString(STID_GIVEN_PAGE_NUM_ADDED), curPageNum);
   Msg(gszMsgBox);
   SetCurChoice(curChoiceBeforeMakeQuiescent);
}

void AddPageBefore()
{
   if (gstWBInfo.do_whiteboard) {
      MsgBox(TgLoadString(STID_ADDPAGEBEFORE_IN_WB), TOOL_NAME, INFO_MB);
      return;
   }
   AddPage(BEFORE_CUR_PAGE, NULL);
}

void AddPageAfter()
{
   if (gstWBInfo.do_whiteboard) {
      MsgBox(TgLoadString(STID_ADDPAGEAFTER_IN_WB), TOOL_NAME, INFO_MB);
      return;
   }
   AddPage(AFTER_CUR_PAGE, NULL);
}

void AddPageBeforeCopyAll()
{
   AddPageCopyAllInfo apcai;

   if (gstWBInfo.do_whiteboard) {
      MsgBox(TgLoadString(STID_ADDPAGEBEFORE_IN_WB), TOOL_NAME, INFO_MB);
      return;
   }
   memset(&apcai, 0, sizeof(AddPageCopyAllInfo));
   apcai.top_obj = topObj;
   apcai.bot_obj = botObj;

   AddPage(BEFORE_CUR_PAGE, &apcai);
}

void AddPageAfterCopyAll()
{
   AddPageCopyAllInfo apcai;

   if (gstWBInfo.do_whiteboard) {
      MsgBox(TgLoadString(STID_ADDPAGEAFTER_IN_WB), TOOL_NAME, INFO_MB);
      return;
   }
   memset(&apcai, 0, sizeof(AddPageCopyAllInfo));
   apcai.top_obj = topObj;
   apcai.bot_obj = botObj;

   AddPage(AFTER_CUR_PAGE, &apcai);
}

void DeleteCurPage()
{
   int n;
   struct PageRec *page_ptr=NULL;

   if (pageLayoutMode == PAGE_TILE) {
      MsgBox(TgLoadString(STID_CANT_DO_DELETEPAGE_IN_TILED), TOOL_NAME,
            INFO_MB);
      return;
   }
   if (lastPageNum == 1) {
      MsgBox(TgLoadString(STID_CANT_DEL_ONLY_PAGE_IN_STACKED), TOOL_NAME,
            INFO_MB);
      return;
   }
   if (firstCmd != NULL || topObj != NULL) {
      sprintf(gszMsgBox, TgLoadString(STID_DELETE_GIVEN_PAGE_CAUSE_FLUSH),
            curPageNum);
      if (!OkToFlushUndoBuffer(gszMsgBox)) {
         return;
      }
      CleanUpCmds();
   }
   n = curPageNum;
   SetFileModified(TRUE);

   MakeQuiescent();

   if (curPage == firstPage) {
      page_ptr = firstPage = curPage->next;
      firstPage->prev = NULL;
   } else if (curPage == lastPage) {
      page_ptr = lastPage = curPage->prev;
      lastPage->next = NULL;
      curPageNum--;
   } else {
      curPage->next->prev = curPage->prev;
      curPage->prev->next = curPage->next;
      page_ptr = curPage->next;
   }
   FreePage(curPage);
   curPage = page_ptr;
   lastPageNum--;
   topObj = curPage->top;
   botObj = curPage->bot;

   ClearAndRedrawDrawWindow();
   RedrawTitleWindow();
   MakeCurPageTabVisible();
   ShowPage();

   sprintf(gszMsgBox, TgLoadString(STID_GIVEN_PAGE_NUM_DELETED), n);
   Msg(gszMsgBox);
   SetCurChoice(curChoiceBeforeMakeQuiescent);
}

void TogglePageLineShown()
{
   if (pageLayoutMode == PAGE_STACK) {
      MsgBox(TgLoadString(STID_CANT_TPLS_IN_STACKED), TOOL_NAME, INFO_MB);
      return;
   }
   pageLineShownInTileMode = !pageLineShownInTileMode;
   ClearAndRedrawDrawWindow();
}

static
int JustSpecifyDrawingSize()
{
   char spec[MAXSTRING+1], *c_ptr, *part_1;
   int cols=0, rows=0;

   *spec = '\0';
   if (Dialog(TgLoadString(STID_ENTER_DRAW_SIZE_SPEC_WH),
         TgLoadCachedString(CSTID_DLG_ACCEPT_CANCEL), spec) == INVALID) {
      return FALSE;
   }
   if ((part_1=strtok(spec, " ,xX")) == NULL) return FALSE;
   if ((c_ptr=strtok(NULL, " ,xX")) == NULL) {
      MsgBox(TgLoadString(STID_BAD_DRAW_SIZE_SPEC_WH), TOOL_NAME, INFO_MB);
      return FALSE;
   } else {
      cols = atoi(part_1);
      rows = atoi(c_ptr);
   }
   if (cols > 0 && rows > 0 && (cols*rows >= lastPageNum)) {
      if (cols*rows < lastPageNum) {
         sprintf(gszMsgBox, TgLoadString(STID_BAD_DRAW_SIZE_SPEC_W_TIMES_H),
               lastPageNum);
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      } else {
         paperCol = cols;
         paperRow = rows;
         return TRUE;
      }
   } else if (cols == 0 || rows == 0) {
      MsgBox(TgLoadString(STID_BAD_DRAW_SIZE_SPEC_WH), TOOL_NAME, INFO_MB);
      return FALSE;
   } else if (cols*rows < lastPageNum) {
      sprintf(gszMsgBox, TgLoadString(STID_BAD_DRAW_SIZE_SPEC_W_TIMES_H),
            lastPageNum);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
   } else {
      MsgBox(TgLoadString(STID_INVALID_DRAW_SIZE_SPEC), TOOL_NAME, INFO_MB);
   }
   return FALSE;
}

void SpecifyDrawingSize()
{
   if (pageLayoutMode != PAGE_TILE) {
      MsgBox(TgLoadString(STID_CANT_DO_SPECDRAWSIZE_IN_TILED), TOOL_NAME,
            INFO_MB);
      return;
   }
   if (JustSpecifyDrawingSize()) {
      UpdPageStyle(pageStyle);
      RedrawScrollBars();
      UpdDrawWinBBox();
      AdjSplineVs();
      ClearAndRedrawDrawWindow();

      ShowPage();
   }
}

void PrintOnePage()
{
   int x=0, y=0;

   switch (pageLayoutMode) {
   case PAGE_TILE:
      switch (whereToPrint) {
      case XBM_FILE:
      case TEXT_FILE:
      case NETLIST_FILE:
      case GIF_FILE:
      case HTML_FILE:
      case PNG_FILE:
      case JPEG_FILE:
      case PPM_FILE:
         sprintf(gszMsgBox, TgLoadString(STID_CANT_PRINTONEPAGE_IN_FORMAT),
               GetExportName(whereToPrint, EXPORT_THIS));
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
         return;
      default: break;
      }
      MakeQuiescent();

      sprintf(gszMsgBox, "    %s",
            TgLoadString(STID_OTHER_BTN_CANCEL_PRINTONEPAGE));
      TwoLineMsg(TgLoadString(STID_BTN1_SEL_PAGE_TO_PRINT), gszMsgBox);
      SetMouseStatus(TgLoadString(STID_SELECT_A_PAGE_TO_PRINT),
            TgLoadCachedString(CSTID_CANCEL), TgLoadCachedString(CSTID_CANCEL));
      if (PickAPoint(&x, &y, handCursor) != Button1) {
         Msg(TgLoadString(STID_OPERATION_CANCEL_BY_USER));
      } else if (x>=0 && x<ZOOMED_SIZE(drawWinW) && y>=0 &&
            y<ZOOMED_SIZE(drawWinH)) {
         int abs_x=ABS_X(x), abs_y=ABS_Y(y);
         int col=(int)(abs_x / onePageWidth);
         int row=(int)(abs_y / onePageHeight);

         if (col >= paperCol || row >= paperRow) {
            MsgBox(TgLoadString(STID_MUST_SEL_A_PT_ON_THE_PAPER), TOOL_NAME,
                  INFO_MB);
         } else {
            DumpOnePageInTileMode(row, col);
         }
      } else {
         sprintf(gszMsgBox, TgLoadString(STID_MUST_SEL_A_PT_IN_TOOL_WIN),
               TOOL_NAME);
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      }
      SetCurChoice(curChoiceBeforeMakeQuiescent);
      break;
   case PAGE_STACK:
      MakeQuiescent();

      DumpOnePageInStackMode();
      SetCurChoice(curChoiceBeforeMakeQuiescent);
      break;
   }
}

int GetDimension(Spec, nNonPosReturnOK, pnNumPixels)
   char *Spec;
   int nNonPosReturnOK, *pnNumPixels;
{
   char *c_ptr;
   double val;

   /* do not translate -- program constants */
   if ((c_ptr=strstr(Spec, "in")) != NULL ||
         (c_ptr=strstr(Spec, "In")) != NULL ||
         (c_ptr=strstr(Spec, "IN")) != NULL) {
      *c_ptr = '\0';
      val = atof(Spec) * ((double)PIX_PER_INCH);
      *pnNumPixels = round(val);
      if (!nNonPosReturnOK && *pnNumPixels <= 0) return FALSE;
   } else if ((c_ptr=strstr(Spec, "cm")) != NULL ||
         (c_ptr=strstr(Spec, "Cm")) != NULL ||
         (c_ptr=strstr(Spec, "CM")) != NULL) {
      *c_ptr = '\0';
      val = atof(Spec) * ((double)ONE_CM);
      *pnNumPixels = round(val);
      if (!nNonPosReturnOK && *pnNumPixels <= 0) return FALSE;
   } else if (sscanf(Spec, "%d", pnNumPixels) != 1) {
      return FALSE;
   } else {
      if (!nNonPosReturnOK && *pnNumPixels <= 0) return FALSE;
   }
   return TRUE;
}

int GetDimensionInDouble(Spec, nNonPosReturnOK, pd_num_pixels)
   char *Spec;
   int nNonPosReturnOK;
   double *pd_num_pixels;
{
   char *c_ptr=NULL;
   double dval=(double)0;

   /* do not translate -- program constants */
   if ((c_ptr=strstr(Spec, "in")) != NULL ||
         (c_ptr=strstr(Spec, "In")) != NULL ||
         (c_ptr=strstr(Spec, "IN")) != NULL) {
      *c_ptr = '\0';
      dval = atof(Spec) * ((double)PIX_PER_INCH);
      *pd_num_pixels = dval;
      if (!nNonPosReturnOK && dval < (double)0) return FALSE;
   } else if ((c_ptr=strstr(Spec, "cm")) != NULL ||
         (c_ptr=strstr(Spec, "Cm")) != NULL ||
         (c_ptr=strstr(Spec, "CM")) != NULL) {
      *c_ptr = '\0';
      dval = atof(Spec) * ((double)ONE_CM);
      *pd_num_pixels = dval;
      if (!nNonPosReturnOK && dval < (double)0) return FALSE;
   } else if (sscanf(Spec, "%lf", pd_num_pixels) != 1) {
      return FALSE;
   } else {
      if (!nNonPosReturnOK && dval < (double)0) return FALSE;
   }
   return TRUE;
}

int SetPaperSize(Spec)
   char *Spec;
{
   int ok=TRUE;
   char *c_ptr;

   UtilTrimBlanks(Spec);
   /* do not translate -- program constants */
   if (UtilStrICmp(Spec, "letter") == 0) {
      onePageWidth = (85*PIX_PER_INCH)/10;
      onePageHeight = 11*PIX_PER_INCH;
      SetPSPageWidthHeight();
   } else if (UtilStrICmp(Spec, "legal") == 0) {
      onePageWidth = (85*PIX_PER_INCH)/10;
      onePageHeight = 14*PIX_PER_INCH;
      SetPSPageWidthHeight();
   } else if (UtilStrICmp(Spec, "a4") == 0) {
      onePageWidth = (825*PIX_PER_INCH)/100;
      onePageHeight = (117*PIX_PER_INCH)/10;
      SetPSPageWidthHeight();
   } else if ((c_ptr=strstr(Spec, " x ")) == NULL &&
         (c_ptr=strstr(Spec, " X ")) == NULL) {
      ok = FALSE;
   } else {
      int w_in_pixel, h_in_pixel;

      *c_ptr = '\0';
      UtilTrimBlanks(&c_ptr[3]);
      if (GetDimension(Spec, FALSE, &w_in_pixel) &&
            GetDimension(&c_ptr[3], FALSE, &h_in_pixel)) {
         onePageWidth = w_in_pixel;
         onePageHeight = h_in_pixel;
         SetPSPageWidthHeight();
      } else {
         ok = FALSE;
      }
   }
   if (ok) {
      if (mainWindow != None) {
         sprintf(gszMsgBox,
               TgLoadString(STID_NEW_PAPER_SIZE_IS_GIVEN),
               (float)(((float)onePageWidth)/((float)PIX_PER_INCH)),
               (float)(((float)onePageHeight)/((float)PIX_PER_INCH)),
               (float)(((float)onePageWidth)/((float)ONE_CM)),
               (float)(((float)onePageHeight)/((float)ONE_CM)));
         Msg(gszMsgBox);
      }
   } else {
      if (mainWindow == None) {
         sprintf(gszMsgBox, TgLoadString(STID_INVALID_XDEF),
               TOOL_NAME, "InitialPaperSize", Spec);
         fprintf(stderr, "%s\n", gszMsgBox);
      } else {
         sprintf(gszMsgBox, TgLoadString(STID_INVALID_GIVEN_PAPER_SIZE_SPEC),
               Spec);
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      }
   }
   return ok;
}

static
void GetCurPaperSizeSpec(buf)
   char *buf;
{
   char w_str[MAXSTRING], h_str[MAXSTRING];
   float w=((float)onePageWidth)*printMag/((float)100.0);
   float h=((float)onePageHeight)*printMag/((float)100.0);
   float unit=(float)0.0, w_val=(float)0.0, h_val=(float)0.0;

   switch (gridSystem) {
   case ENGLISH_GRID:
      unit = (float)PIX_PER_INCH;

      switch (pageStyle) {
      case PORTRAIT: w_val = w/unit; h_val = h/unit; break;
      case LANDSCAPE: w_val = h/unit; h_val = w/unit; break;
      }
      FormatFloat(&w_val, w_str);
      FormatFloat(&h_val, h_str);
      /* do not translate -- program constants */
      sprintf(buf, "%s in x %s in", w_str, h_str);
      break;
   case METRIC_GRID:
      unit = (float)ONE_CM;

      switch (pageStyle) {
      case PORTRAIT: w_val = w/unit; h_val = h/unit; break;
      case LANDSCAPE: w_val = h/unit; h_val = w/unit; break;
      }
      FormatFloat(&w_val, w_str);
      FormatFloat(&h_val, h_str);
      /* do not translate -- program constants */
      sprintf(buf, "%s cm x %s cm", w_str, h_str);
      break;
   }
}

void SpecifyPaperSize()
{
   char buf[MAXSTRING+1], spec[MAXSTRING+1];

   GetCurPaperSizeSpec(gszMsgBox);
   sprintf(buf, TgLoadString(STID_CUR_PAPER_SIZE_IS_GIVEN), gszMsgBox);
   *spec = '\0';
   if (Dialog(TgLoadString(STID_ENTER_PAPER_SIZE_WH), buf, spec) == INVALID) {
      return;
   }
   MakeQuiescent();

   if (SetPaperSize(spec)) {
      UpdPageStyle(pageStyle);
      SetFileModified(TRUE);
      ClearAndRedrawDrawWindow();
   }
   SetCurChoice(curChoiceBeforeMakeQuiescent);
}

void FreePageSpec(pspi)
   SpecifyPagesInfo *pspi;
{
   if (pspi->page_specified != NULL) free(pspi->page_specified);
}

int ParsePagesSpec(spec, pspi)
   char *spec;
   SpecifyPagesInfo *pspi;
   /* if return TRUE, must call FreePageSpec(pspi) */
{
   char *dup_spec=NULL, *c_ptr=NULL;
   int i, ok=TRUE;

   UtilTrimBlanks(spec);
   if (*spec == '\0') return FALSE;
   if ((pspi->page_specified=(int*)malloc(lastPageNum*sizeof(int))) == NULL) {
      FailAllocMessage();
      return FALSE;
   }
   if ((dup_spec=UtilStrDup(spec)) == NULL) {
      free(pspi->page_specified);
      FailAllocMessage();
      return FALSE;
   }
   if (strcmp(dup_spec, "*") == 0) {
      for (i=0; i < lastPageNum; i++) pspi->page_specified[i] = TRUE;
      pspi->num_pages_specified = lastPageNum;
   } else {
      for (i=0; i < lastPageNum; i++) pspi->page_specified[i] = FALSE;
      if (*dup_spec == '*') *dup_spec = ' ';
      for (c_ptr=strtok(dup_spec, " ,\t"); ok && c_ptr != NULL;
            c_ptr=strtok(NULL, " ,\t")) {
         char *dash_ptr=strchr(c_ptr, '-');

         if (dash_ptr == NULL) {
            int page_num=atoi(c_ptr);

            if (page_num > 0 && page_num <= lastPageNum) {
               if (!pspi->page_specified[page_num-1]) {
                  pspi->num_pages_specified++;
               }
               pspi->page_specified[page_num-1] = TRUE;
            } else {
               sprintf(gszMsgBox,
                     TgLoadString(STID_GIVEN_PAGE_NUM_IS_OUT_OF_RNG), page_num);
               MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
               ok = FALSE;
            }
         } else {
            int start_page_num, end_page_num;

            *dash_ptr = '\0';
            start_page_num = atoi(c_ptr);
            end_page_num = atoi(&dash_ptr[1]);
            *dash_ptr = '-';
            if (start_page_num <= 0 || start_page_num > lastPageNum) {
               sprintf(gszMsgBox,
                     TgLoadString(STID_GIVEN_PAGE_NUM_IS_OUT_OF_RNG),
                     start_page_num);
               MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
               ok = FALSE;
            } else if (end_page_num <= 0 || end_page_num > lastPageNum) {
               sprintf(gszMsgBox,
                     TgLoadString(STID_GIVEN_PAGE_NUM_IS_OUT_OF_RNG),
                     end_page_num);
               MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
               ok = FALSE;
            } else if (start_page_num > end_page_num) {
               sprintf(gszMsgBox,
                     TgLoadString(STID_MALFORMED_SPECIFICATION_STR), spec);
               MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
               ok = FALSE;
            } else {
               for (i=start_page_num; i <= end_page_num; i++) {
                  if (!pspi->page_specified[i-1]) pspi->num_pages_specified++;
                  pspi->page_specified[i-1] = TRUE;
               }
            }
         }
      }
   }
   free(dup_spec);

   if (!ok) FreePageSpec(pspi);
   return ok;
}

void DeletePages()
{
   char spec[MAXSTRING+1];
   SpecifyPagesInfo spi;

   if (pageLayoutMode == PAGE_TILE) {
      MsgBox(TgLoadString(STID_CANT_DO_DELETEPAGES_IN_TILED), TOOL_NAME,
            INFO_MB);
      return;
   }
   if (lastPageNum == 1) {
      MsgBox(TgLoadString(STID_CANT_DEL_ONLY_PAGE_IN_STACKED), TOOL_NAME,
            INFO_MB);
      return;
   }
   *spec = '\0';
   Dialog(TgLoadString(STID_SPECIFY_PAGES_TO_DEL_EX), NULL, spec);
   UtilTrimBlanks(spec);
   if (*spec == '\0') return;

   memset(&spi, 0, sizeof(SpecifyPagesInfo));
   if (!ParsePagesSpec(spec, &spi)) return;

   MakeQuiescent();

   if (spi.num_pages_specified == lastPageNum) {
      MsgBox(TgLoadString(STID_CANT_DEL_ALL_PAGES), TOOL_NAME, INFO_MB);
   } else if (spi.num_pages_specified > 0) {
      sprintf(gszMsgBox, TgLoadString(spi.num_pages_specified > 1 ?
            STID_DELETE_A_PAGE_CAUSE_FLUSH :
            STID_DELETE_PAGES_PAGE_CAUSE_FLUSH));
      if (firstCmd == NULL || OkToFlushUndoBuffer(gszMsgBox)) {
         struct PageRec *page_ptr;
         int page_num;

         for (page_num=lastPageNum; page_num > 0; page_num--) {
            if (spi.page_specified[page_num-1]) {
               GotoPageNum(page_num);
               if (curPage == firstPage) {
                  page_ptr = firstPage = curPage->next;
                  firstPage->prev = NULL;
               } else if (curPage == lastPage) {
                  page_ptr = lastPage = curPage->prev;
                  lastPage->next = NULL;
                  curPageNum--;
               } else {
                  curPage->next->prev = curPage->prev;
                  curPage->prev->next = curPage->next;
                  page_ptr = curPage->next;
               }
               FreePage(curPage);
               curPage = page_ptr;
               lastPageNum--;
               topObj = curPage->top;
               botObj = curPage->bot;
            }
         }
         CleanUpCmds();
         ClearAndRedrawDrawWindow();
         RedrawTitleWindow();
         ShowPage();

         sprintf(gszMsgBox, TgLoadString(spi.num_pages_specified > 1 ?
               STID_MANY_PAGES_DELETED : STID_ONE_PAGE_DELETED),
               spi.num_pages_specified);
         Msg(gszMsgBox);
         SetFileModified(TRUE);
      }
   }
   FreePageSpec(&spi);
   SetCurChoice(curChoiceBeforeMakeQuiescent);
}

void SavePagesAs()
{
   char spec[MAXSTRING+1];
   SpecifyPagesInfo spi;

   if (pageLayoutMode == PAGE_TILE) {
      MsgBox(TgLoadString(STID_CANT_DO_SAVEPAGESAS_IN_TILED), TOOL_NAME,
            INFO_MB);
      return;
   }
   if (lastPageNum == 1) {
      SaveNewFile(FALSE, NULL);
      return;
   }
   *spec = '\0';
   Dialog(TgLoadString(STID_SPECIFY_PAGES_TO_SAVE_EX), NULL, spec);
   UtilTrimBlanks(spec);
   if (*spec == '\0') return;

   memset(&spi, 0, sizeof(SpecifyPagesInfo));
   if (!ParsePagesSpec(spec, &spi)) return;

   MakeQuiescent();

   if (spi.num_pages_specified == lastPageNum) {
      SaveNewFile(FALSE, NULL);
   } else if (spi.num_pages_specified > 0) {
      SavePages(&spi);
   }
   FreePageSpec(&spi);
   SetCurChoice(curChoiceBeforeMakeQuiescent);
}

void PrintPages()
{
   char spec[MAXSTRING];

   if (pageLayoutMode == PAGE_TILE) {
      MsgBox(TgLoadString(STID_CANT_PRINT_PAGES_IN_TILED), TOOL_NAME, INFO_MB);
      return;
   }
   /* only allows PRINTER, PS_FILE, PDF_FILE, or export filter formats */
   switch (whereToPrint) {
   case LATEX_FIG:
   case XBM_FILE:
   case TEXT_FILE:
   case NETLIST_FILE:
   case EPSI_FILE:
   case GIF_FILE:
   case HTML_FILE:
   case TIFFEPSI_FILE:
   case PNG_FILE:
   case JPEG_FILE:
   case PPM_FILE:
   case SVG_FILE:
      sprintf(gszMsgBox, TgLoadString(STID_CANT_PRINT_PAGES_IN_FORMAT),
            GetExportName(whereToPrint, EXPORT_THIS));
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return;
   default: break;
   }
   strcpy(spec, "*");
   Dialog(TgLoadString(STID_SPECIFY_PAGES_TO_PRINT_EX), NULL, spec);
   UtilTrimBlanks(spec);
   if (*spec == '\0') return;

   MakeQuiescent();

   memset(&gPagesToPrintSpec, 0, sizeof(SpecifyPagesInfo));
   if (!ParsePagesSpec(spec, &gPagesToPrintSpec)) return;

   DumpPages();

   FreePageSpec(&gPagesToPrintSpec);
   SetCurChoice(curChoiceBeforeMakeQuiescent);
}

void PrintOneFilePerPage()
{
   char spec[MAXSTRING];

   if (pageLayoutMode == PAGE_TILE) {
      MsgBox(TgLoadString(STID_CANT_DO_POFPP_IN_TILED), TOOL_NAME, INFO_MB);
      return;
   } else if (whereToPrint == PRINTER) {
      MsgBox(TgLoadString(STID_CANT_DO_POFPP_TO_PRINTER), TOOL_NAME, INFO_MB);
      return;
   }
   strcpy(spec, "*");
   Dialog(TgLoadString(STID_SPECIFY_PAGES_TO_PRINT_EX), NULL, spec);
   UtilTrimBlanks(spec);
   if (*spec == '\0') return;

   MakeQuiescent();

   memset(&gPagesToPrintSpec, 0, sizeof(SpecifyPagesInfo));
   if (!ParsePagesSpec(spec, &gPagesToPrintSpec)) return;

   DumpOneFilePerPage();

   FreePageSpec(&gPagesToPrintSpec);
   SetCurChoice(curChoiceBeforeMakeQuiescent);
}

int StackedPageMenu(X, Y, TrackMenubar)
   int X, Y, TrackMenubar;
{
   int rc=INVALID;
   TgMenu *menu=(stackedPageMenuInfo.create_proc)(NULL, X, Y,
         &stackedPageMenuInfo, FALSE);

   activeMenu = MENU_STACKEDPAGE;
   if (menu != NULL) {
      menu->track_menubar = TrackMenubar;

      rc = TgMenuLoop(menu);
      TgDestroyMenu(menu, TRUE);
   }
   return rc;
}

int TiledPageMenu(X, Y, TrackMenubar)
   int X, Y, TrackMenubar;
{
   int rc=INVALID;
   TgMenu *menu=(tiledPageMenuInfo.create_proc)(NULL, X, Y, &tiledPageMenuInfo,
         FALSE);

   activeMenu = MENU_TILEDPAGE;
   if (menu != NULL) {
      menu->track_menubar = TrackMenubar;

      rc = TgMenuLoop(menu);
      TgDestroyMenu(menu, TRUE);
   }
   return rc;
}

int RefreshPageMenu(menu)
   TgMenu *menu;
{
   int ok=TRUE;

   ok &= TgEnableMenuItemByIndex(menu, PAGE_STACK,
         (pageLayoutMode==PAGE_STACK));
   ok &= TgEnableMenuItemByIndex(menu, PAGE_TILE,
         (pageLayoutMode!=PAGE_STACK));
   return ok;
}

TgMenu *CreatePageMenu(parent_menu, x, y, menu_info, status_str_xlated)
   TgMenu *parent_menu;
   int x, y;
   TgMenuInfo *menu_info;
   int status_str_xlated; /* ignored, always 0 */
{
   TgMenu *menu=TgCreateMenuFromMenuInfo(parent_menu, x, y, menu_info, FALSE);

   if (menu != NULL) {
      if (!RefreshPageMenu(menu)) {
         return TgDestroyMenu(menu, TRUE);
      }
      menu->refresh_proc = ((RefreshMenuFunc*)RefreshPageMenu);
   }
   return menu;
}

int PageMenu(X, Y, TrackMenubar)
   int X, Y, TrackMenubar;
{
   switch (pageLayoutMode) {
   case PAGE_STACK: return StackedPageMenu(X, Y, TrackMenubar);
   case PAGE_TILE: return TiledPageMenu(X, Y, TrackMenubar);
   }
   return INVALID;
}

void DelAllPages()
{
   register struct PageRec *next_page=NULL;

   for (curPage=firstPage; curPage != NULL; curPage=next_page) {
      next_page = curPage->next;
      FreePage(curPage);
   }
   lastPageNum = 0;
   firstPage = lastPage = curPage = NULL;
   InitPage();
}

static
int AnyObjOnPageBoundary()
{
   register struct ObjRec *obj_ptr;
   int ltx, lty, rbx, rby, next_page_x, next_page_y;

   for (obj_ptr=topObj; obj_ptr != NULL; obj_ptr=obj_ptr->next) {
      ltx = obj_ptr->obbox.ltx; lty = obj_ptr->obbox.lty;
      rbx = obj_ptr->obbox.rbx; rby = obj_ptr->obbox.rby;
      if (ltx < 0 || lty < 0 || rbx >= onePageWidth*paperCol ||
            rby >= onePageHeight*paperRow) {
         MsgBox(TgLoadString(STID_OBJ_OUTSIDE_PAGE_BNRDY_SWITCH), TOOL_NAME,
               INFO_MB);
         return TRUE;
      }
      next_page_x = ((ltx % onePageWidth) == 0) ?
            onePageWidth * (round(ltx / onePageWidth) + 1) :
            onePageWidth * (((int)(ltx / onePageWidth)) + 1);
      next_page_y = ((lty % onePageHeight) == 0) ?
            onePageHeight * (round(lty / onePageHeight) + 1) :
            onePageHeight * (((int)(lty / onePageHeight)) + 1);
      if (rbx >= next_page_x || rby >= next_page_y) {
         MsgBox(TgLoadString(STID_OBJ_OUTSIDE_PAGE_BNRDY_SWITCH), TOOL_NAME,
               INFO_MB);
         return TRUE;
      }
   }
   return FALSE;
}

static
int CalcStackPageNum(obj_ptr)
   struct ObjRec *obj_ptr;
{
   int ltx, lty, col, row;

   ltx = obj_ptr->obbox.ltx; lty = obj_ptr->obbox.lty;
   col = ((ltx % onePageWidth) == 0) ? round(ltx / onePageWidth) + 1 :
         ((int)(ltx / onePageWidth)) + 1;
   row = ((lty % onePageHeight) == 0) ? round(lty / onePageHeight) + 1 :
         ((int)(lty / onePageHeight)) + 1;
   return ((row-1) * paperCol + col);
}

static
void TileToStack()
{
   register int i;
   register struct ObjRec *obj_ptr, *prev_obj;
   int *dx=NULL, *dy=NULL, index;
   struct PageRec **page_handle=NULL;
   struct ObjRec *saved_top_obj, *saved_bot_obj;

   saved_top_obj=curPage->top;
   saved_bot_obj=curPage->bot;
   curPage->top = curPage->bot = NULL;
   DelAllPages();

   firstPage = lastPage = curPage = NULL;
   lastPageNum = paperCol * paperRow;
   InitPage();

   page_handle = (struct PageRec **)malloc(lastPageNum*sizeof(struct PageRec*));
   if (page_handle == NULL) FailAllocMessage();
   dx = (int*)malloc(lastPageNum*sizeof(int));
   dy = (int*)malloc(lastPageNum*sizeof(int));
   if (dx == NULL || dy == NULL) FailAllocMessage();
   for (curPage=firstPage, i=0; curPage != NULL; curPage=curPage->next, i++) {
      dx[i] = (i % paperCol) * onePageWidth;
      dy[i] = (((i % paperCol) == 0) ? round(i / paperCol):
            ((int)(i / paperCol))) * onePageHeight;
      page_handle[i] = curPage;
   }
   for (obj_ptr=saved_bot_obj; obj_ptr != NULL; obj_ptr = prev_obj) {
      prev_obj = obj_ptr->prev;
      index = CalcStackPageNum(obj_ptr)-1;
      curPage = page_handle[index];
      AddObj(NULL, curPage->top, obj_ptr);
      MoveObj(obj_ptr, -(dx[index]), -(dy[index]));
   }
   curPage = firstPage;
   topObj = curPage->top;
   botObj = curPage->bot;
   curPageNum = 1;
   paperCol = paperRow = 1;
   free(dx);
   free(dy);
   free(page_handle);
}

static
void StackToTile()
{
   register struct ObjRec *obj_ptr, *prev_obj;
   int i, dx, dy;
   struct PageRec *saved_first_page, *saved_last_page;
   struct PageRec *tmp_page, *page_ptr;

   if (paperCol * paperRow < lastPageNum && !JustSpecifyDrawingSize()) return;

   saved_first_page = firstPage;
   saved_last_page = lastPage;
   firstPage = lastPage = NULL;
   lastPageNum = 1;
   InitPage();
   tmp_page = firstPage;

   i = 0;
   for (page_ptr=saved_first_page; page_ptr != NULL;
         page_ptr=page_ptr->next, i++) {
      dx = (i % paperCol) * onePageWidth;
      dy = (((i % paperCol) == 0) ? round(i / paperCol):
            ((int)(i / paperCol))) * onePageHeight;
      for (obj_ptr=page_ptr->bot; obj_ptr != NULL; obj_ptr=prev_obj) {
         prev_obj = obj_ptr->prev;
         AddObj(NULL, topObj, obj_ptr);
         MoveObj(obj_ptr, dx, dy);
      }
      page_ptr->top = page_ptr->bot = NULL;
   }
   firstPage = saved_first_page;
   lastPage = saved_last_page;
   DelAllPages();
   firstPage = lastPage = curPage = tmp_page;
   curPageNum = lastPageNum = 1;
   pageLineShownInTileMode = TRUE;
   topObj = curPage->top;
   botObj = curPage->bot;
}

static
int PreservePageNames()
{
   struct PageRec *page_ptr=NULL;

   for (page_ptr=firstPage; page_ptr != NULL; page_ptr=page_ptr->next) {
      if (page_ptr->name != NULL && *page_ptr->name != '\0') {
         break;
      } else if (page_ptr->page_file_name != NULL &&
            *page_ptr->page_file_name != '\0') {
         break;
      }
   }
   if (page_ptr != NULL) {
      if (MsgBox(TgLoadString(STID_SWTCH_TO_TILED_LOSE_ALL_NAMES), TOOL_NAME,
            YNC_MB) != MB_ID_YES) {
         return FALSE;
      }
   }
   return TRUE;
}

void PageLayoutSubMenu(index)
   int index;
{
   if (pageLayoutMode == index) return;

   if (gstWBInfo.do_whiteboard) {
      MsgBox(TgLoadString(STID_CHANGE_PAGE_LAYOUT_MODE_IN_WB), TOOL_NAME,
            INFO_MB);
      return;
   }
   if (!OkToFlushUndoBuffer(TgLoadString(STID_CHNG_PAGE_LAYOUT_FLUSH))) {
      return;
   }
   MakeQuiescent();

   switch (index) {
   case PAGE_STACK:
      if (AnyObjOnPageBoundary()) {
         SetCurChoice(curChoiceBeforeMakeQuiescent);
         return;
      }
      pageLayoutMode = index;
      TileToStack();
      Msg(TgLoadString(STID_PAGE_LAYOUT_MODE_IS_STACKED));
      break;
   case PAGE_TILE:
      if (!PreservePageNames() || !JustSpecifyDrawingSize()) {
         SetCurChoice(curChoiceBeforeMakeQuiescent);
         return;
      }
      pageLayoutMode = index;
      StackToTile();
      Msg(TgLoadString(STID_PAGE_LAYOUT_MODE_IS_TILED));
      break;
   }
   ShowPage();
   ShowPageLayout();
   UpdatePinnedMenu(MENU_FILE);
   UpdatePinnedMenu(MENU_LAYOUT);
   UpdatePinnedMenu(MENU_PAGELAYOUT);
   UpdatePinnedMenu(MENU_MAIN);
   switch (pageLayoutMode) {
   case PAGE_STACK: DestroyPinnedMenu(MENU_TILEDPAGE); break;
   case PAGE_TILE: DestroyPinnedMenu(MENU_STACKEDPAGE); break;
   }
   DestroyPinnedMenu(MENU_PAGE);

   UpdPageStyle(pageStyle);
   RedrawScrollBars();
   UpdDrawWinBBox();
   AdjSplineVs();
   ClearAndRedrawDrawWindow();
   RedrawTitleWindow();
   SetFileModified(TRUE);
   SetCurChoice(curChoiceBeforeMakeQuiescent);
}

void RefreshPageLayoutMenu(menu)
   TgMenu *menu;
{
   int i, num_items=menu->num_items;
   TgMenuItem *menuitems=menu->menuitems;

   for (i=0; i < num_items; i++) {
      TgMenuItem *menu_item=(&menuitems[i]);
      TgMenuItem stMenuItem;

      memset(&stMenuItem, 0, sizeof(TgMenuItem));
      stMenuItem.state = TGBS_NORMAL;
      stMenuItem.checked = (i == pageLayoutMode);
      TgSetMenuItemInfo(menu_item, TGMU_MASK_STATE|TGMU_MASK_CHECK,
            &stMenuItem);
   }
}

TgMenu *CreatePageLayoutMenu(parent_menu, x, y, menu_info, status_str_xlated)
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
      TgAdjustMenuGeometry(menu, choiceImageW, choiceImageH,
            MAXPAGELAYOUTMODES);
      menu_item = (&menu->menuitems[pageLayoutMode]);

      memset(&stMenuItem, 0, sizeof(TgMenuItem));
      stMenuItem.checked = TRUE;
      if (!TgSetMenuItemInfo(menu_item, TGMU_MASK_CHECK, &stMenuItem)) {
         return TgDestroyMenu(menu, TRUE);
      }
      menu->refresh_proc = ((RefreshMenuFunc*)RefreshPageLayoutMenu);
   }
   return menu;
}

int PageLayoutMenu(X, Y, TrackMenubar)
   int X, Y, TrackMenubar;
{
   int rc=INVALID;
   TgMenu *menu=(pageLayoutMenuInfo.create_proc)(NULL, X, Y,
         &pageLayoutMenuInfo, INVALID);

   activeMenu = MENU_PAGELAYOUT;
   if (menu != NULL) {
      menu->track_menubar = TrackMenubar;

      rc = TgMenuLoop(menu);
      TgDestroyMenu(menu, TRUE);
   }
   return rc;
}

void PageEventHandler(input)
   XEvent *input;
{
   XEvent ev;

   if (input->type == Expose) {
      while (XCheckWindowEvent(mainDisplay, pageWindow, ExposureMask, &ev)) ;
      RedrawPageWindow();
   } else if (input->type == EnterNotify) {
      SetMouseStatus("", "", "");
   } else if (input->type == MotionNotify) {
      while (XCheckWindowEvent(mainDisplay,pageWindow,PointerMotionMask,&ev)) ;
      if (pageLayoutMode == PAGE_TILE) {
         SetMouseStatus(TgLoadCachedString(CSTID_PARANED_NONE),
               TgLoadCachedString(CSTID_PAGE_MENU),
               TgLoadCachedString(CSTID_PARANED_NONE));
      } else {
         HandleMotionInPageWindow(&input->xmotion);
      }
   } else if (input->type == ButtonPress) {
      XButtonEvent *button_ev=(&(input->xbutton));

      if (pageLayoutMode == PAGE_TILE) {
         if (button_ev->button == Button2) {
            PageMenu(button_ev->x_root, button_ev->y_root, FALSE);
         }
      } else {
         if (button_ev->button == Button1) {
            HandleClickInPageWindow(button_ev);
         } else if (button_ev->button == Button2) {
            PageMenu(button_ev->x_root, button_ev->y_root, FALSE);
         }
      }
   }
}

void CleanUpPage()
{
   DelAllPages();
   if (leftend_bits == NULL) { }
}

static PageInfo stPageInfo;

void PushPageInfo()
{
   stPageInfo.page_layout_mode = pageLayoutMode;
   stPageInfo.paper_col = paperCol;
   stPageInfo.paper_row = paperRow;
   stPageInfo.cur_page_num = curPageNum;
   stPageInfo.last_page_num = lastPageNum;
   stPageInfo.first_page = firstPage;
   stPageInfo.last_page = lastPage;
   stPageInfo.cur_page = curPage;
}

void PopPageInfo()
{
   pageLayoutMode = stPageInfo.page_layout_mode;
   paperCol = stPageInfo.paper_col;
   paperRow = stPageInfo.paper_row;
   curPageNum = stPageInfo.cur_page_num;
   lastPageNum = stPageInfo.last_page_num;
   firstPage = stPageInfo.first_page;
   lastPage = stPageInfo.last_page;
   curPage = stPageInfo.cur_page;
}

void GetPageInfo(pPageInfo)
   PageInfo *pPageInfo;
{
   pPageInfo->page_layout_mode = pageLayoutMode;
   pPageInfo->paper_col = paperCol;
   pPageInfo->paper_row = paperRow;
   pPageInfo->cur_page_num = curPageNum;
   pPageInfo->last_page_num = lastPageNum;
   pPageInfo->first_page = firstPage;
   pPageInfo->last_page = lastPage;
   pPageInfo->cur_page = curPage;
}

/* --------------------- FixPageNumbersForSlideShow() --------------------- */

static
int FixPageNumInStrSeg(pStrSeg, attr_ptr, just_checking)
   StrSegInfo *pStrSeg;
   struct AttrRec *attr_ptr;
   int just_checking;
{
   static int page_num_str_len=0, num_pages_str_len=0;
   static int stInitialized=FALSE;

   if (!stInitialized) {
      stInitialized = TRUE;
      /* do not translate -- program constants */
      page_num_str_len = strlen("!(STACKED_PAGE_NUM)");
      num_pages_str_len = strlen("!(STACKED_NUM_PAGES)");
   }
   /* do not translate -- program constants */
   if (strstr(pStrSeg->dyn_str.s, "!(STACKED_PAGE_NUM)") != NULL ||
         strstr(pStrSeg->dyn_str.s, "!(STACKED_NUM_PAGES)") != NULL) {
      char *c_ptr=NULL, *c_ptr1=NULL;
      struct ObjRec *attr_owner_obj=NULL;

      if (just_checking) {
         return TRUE;
      }
      attr_owner_obj = GetTopOwner(attr_ptr->owner);
      while ((c_ptr=strstr(pStrSeg->dyn_str.s, "!(STACKED_PAGE_NUM)")) !=
            NULL) {
         c_ptr1 = &c_ptr[page_num_str_len];
         sprintf(c_ptr, "%1d", curPageNum);
         c_ptr = &c_ptr[strlen(c_ptr)];
         while (*c_ptr1 != '\0') *c_ptr++ = *c_ptr1++;
         *c_ptr = '\0';
      }
      while ((c_ptr=strstr(pStrSeg->dyn_str.s, "!(STACKED_NUM_PAGES)")) !=
            NULL) {
         c_ptr1 = &c_ptr[num_pages_str_len];
         sprintf(c_ptr, "%1d", lastPageNum);
         c_ptr = &c_ptr[strlen(c_ptr)];
         while (*c_ptr1 != '\0') *c_ptr++ = *c_ptr1++;
         *c_ptr = '\0';
      }
      RecursivelyAdjObjBBox(attr_ptr->owner, attr_ptr->owner, attr_owner_obj);

      SetFileModified(TRUE);
      return TRUE;
   }
   return FALSE;
}

static
int FixPageNumInStrBlock(pStrBlock, attr_ptr, just_checking)
   StrBlockInfo *pStrBlock;
   struct AttrRec *attr_ptr;
   int just_checking;
{
   int changed=FALSE;

   switch (pStrBlock->type) {
   case SB_SIMPLE:
      if (FixPageNumInStrSeg(pStrBlock->seg, attr_ptr, just_checking)) {
         if (just_checking) {
            return TRUE;
         }
         changed = TRUE;
      }
      break;
   case SB_CHAR_SPACE: break;

   case SB_SUPSUB_LEFT:
   case SB_SUPSUB_CENTER:
   case SB_SUPSUB_RIGHT:
      if (pStrBlock->type == SB_SUPSUB_CENTER) {
         if (FixPageNumInStrSeg(pStrBlock->seg, attr_ptr, just_checking)) {
            if (just_checking) {
               return TRUE;
            }
            changed = TRUE;
         }
      }
      if (pStrBlock->sup != NULL) {
         if (FixPageNumInMiniLines(pStrBlock->sup, attr_ptr, just_checking)) {
            if (just_checking) {
               return TRUE;
            }
            changed = TRUE;
         }
      }
      if (pStrBlock->sub != NULL) {
         if (FixPageNumInMiniLines(pStrBlock->sub, attr_ptr, just_checking)) {
            if (just_checking) {
               return TRUE;
            }
            changed = TRUE;
         }
      }
      break;
   }
   return changed;
}

static
int FixPageNumInMiniLine(pMiniLine, attr_ptr, just_checking)
   MiniLineInfo *pMiniLine;
   struct AttrRec *attr_ptr;
   int just_checking;
{
   int changed=FALSE;
   StrBlockInfo *pStrBlock=NULL;

   for (pStrBlock=pMiniLine->first_block; pStrBlock != NULL;
         pStrBlock=pStrBlock->next) {
      if (FixPageNumInStrBlock(pStrBlock, attr_ptr, just_checking)) {
         if (just_checking) {
            return TRUE;
         }
         changed = TRUE;
      }
   }
   return changed;
}

int FixPageNumInMiniLines(minilines, attr_ptr, just_checking)
   MiniLinesInfo *minilines;
   struct AttrRec *attr_ptr;
   int just_checking;
{
   int changed=FALSE;
   MiniLineInfo *pMiniLine=NULL;

   for (pMiniLine=minilines->first; pMiniLine != NULL;
         pMiniLine=pMiniLine->next) {
      if (FixPageNumInMiniLine(pMiniLine, attr_ptr, just_checking)) {
         if (just_checking) {
            return TRUE;
         }
         changed = TRUE;
      }
   }
   return changed;
}

static
int FixPageNumInAttr(AttrPtr, just_checking)
   struct AttrRec *AttrPtr;
   int just_checking;
{
   int changed=FALSE;

   for ( ; AttrPtr != NULL; AttrPtr=AttrPtr->prev) {
      if (!AttrPtr->shown) {
         continue;
      }
      /* do not translate -- program constants */
      if (!AttrPtr->nameshown && *AttrPtr->attr_name.s == '!' &&
            strcmp(AttrPtr->attr_name.s, "!PAGE_NUM=") == 0) {
         if (FixPageNumInMiniLines(&AttrPtr->obj->detail.t->minilines, AttrPtr,
               just_checking)) {
            if (just_checking) {
               return TRUE;
            }
            UpdateAttr(AttrPtr->obj->detail.t, AttrPtr);
            changed = TRUE;
         }
      }
   }
   return changed;
}

static
int FixPageNumInObj(ObjPtr, just_checking)
   struct ObjRec *ObjPtr;
   int just_checking;
{
   int something_changed=FALSE;

   switch (ObjPtr->type) {
   case OBJ_POLY:
   case OBJ_BOX:
   case OBJ_OVAL:
   case OBJ_POLYGON:
   case OBJ_ARC:
   case OBJ_RCBOX:
   case OBJ_XBM:
   case OBJ_XPM:
      if (!colorLayers ||
            ObjPtr->tmp_parent!=NULL || ObjInVisibleLayer(ObjPtr)) {
         if (FixPageNumInAttr(ObjPtr->lattr, just_checking)) {
            if (just_checking) {
               return TRUE;
            } else {
               something_changed = TRUE;
            }
         }
      }
      break;
   case OBJ_TEXT: break;

   case OBJ_SYM:
   case OBJ_ICON:
   case OBJ_GROUP:
      if (!colorLayers ||
            ObjPtr->tmp_parent!=NULL || ObjInVisibleLayer(ObjPtr)) {
         struct ObjRec *obj_ptr=ObjPtr->detail.r->last;

         for ( ; obj_ptr != NULL; obj_ptr = obj_ptr->prev) {
            obj_ptr->tmp_parent = ObjPtr;
            if (FixPageNumInObj(obj_ptr, just_checking)) {
               if (just_checking) {
                  return TRUE;
               } else {
                  something_changed = TRUE;
               }
            }
         }
         if (FixPageNumInAttr(ObjPtr->lattr, just_checking)) {
            if (just_checking) {
               return TRUE;
            } else {
               something_changed = TRUE;
            }
         }
      }
      break;
   case OBJ_PIN:
      if (!colorLayers ||
            ObjPtr->tmp_parent!=NULL || ObjInVisibleLayer(ObjPtr)) {
         struct ObjRec *obj_ptr=GetPinObj(ObjPtr);

         obj_ptr->tmp_parent = ObjPtr;
         if (FixPageNumInObj(obj_ptr, just_checking)) {
            if (just_checking) {
               return TRUE;
            } else {
               something_changed = TRUE;
            }
         }
         if (FixPageNumInAttr(ObjPtr->lattr, just_checking)) {
            if (just_checking) {
               return TRUE;
            } else {
               something_changed = TRUE;
            }
         }
      }
      break;
   }
   if (just_checking) return FALSE;

   return something_changed;
}

void FixPageNumbersForSlideShow()
{
   int i=0, saved_cur_page_num=curPageNum, page_num=1, num_pages_to_fix=0;
   int *pages_to_fix=NULL;
   struct PageRec *page_ptr=NULL;
   struct ObjRec *obj_ptr=NULL;

   for (page_ptr=firstPage, page_num=1; page_ptr != NULL;
         page_ptr=page_ptr->next, page_num++) {
      for (obj_ptr=page_ptr->bot; obj_ptr != NULL; obj_ptr=obj_ptr->prev) {
         obj_ptr->tmp_parent = NULL;
         if (FixPageNumInObj(obj_ptr, TRUE)) {
            num_pages_to_fix++;
            if (pages_to_fix == NULL) {
               pages_to_fix = (int*)malloc(sizeof(int));
            } else {
               pages_to_fix = (int*)realloc(pages_to_fix,
                     num_pages_to_fix*sizeof(int));
            }
            if (pages_to_fix == NULL) FailAllocMessage();
            pages_to_fix[num_pages_to_fix-1] = page_num;
            break;
         }
      }
   }
   if (num_pages_to_fix == 0) {
      /* nothing to fix */
      return;
   }
   StartCompositeCmd();
   for (i=0; i < num_pages_to_fix; i++) {
      page_num = pages_to_fix[i];
      if (page_num != curPageNum) {
         PrepareToRecord(CMD_GOTO_PAGE, NULL, NULL, curPageNum);
         GotoPageNum(page_num);
         RecordCmd(CMD_GOTO_PAGE, NULL, NULL, NULL, curPageNum);
      }
      if (gstWBInfo.do_whiteboard && curPageNum != saved_cur_page_num) {
         /*
          * Since the whiteboard is active, others only see
          *         the current page.  So, changes to other
          *         pages should not be seen by other whiteboards.
          */
         gstWBInfo.dont_serialize = TRUE;
      }
      for (obj_ptr=botObj; obj_ptr != NULL; obj_ptr=obj_ptr->prev) {
         PrepareToReplaceAnObj(obj_ptr);
         if (FixPageNumInObj(obj_ptr, FALSE)) {
            RecordReplaceAnObj(obj_ptr);
         } else {
            AbortPrepareCmd(CMD_REPLACE);
         }
      }
      if (gstWBInfo.do_whiteboard && curPageNum != saved_cur_page_num) {
         gstWBInfo.dont_serialize = FALSE;
      }
   }
   if (curPageNum != saved_cur_page_num) {
      PrepareToRecord(CMD_GOTO_PAGE, NULL, NULL, curPageNum);
      GotoPageNum(saved_cur_page_num);
      RecordCmd(CMD_GOTO_PAGE, NULL, NULL, NULL, curPageNum);
   }
   EndCompositeCmd();

   free(pages_to_fix);
}

int NeedToFixPageNumbersForSlideShow()
{
   struct PageRec *page_ptr=NULL;
   struct ObjRec *obj_ptr=NULL;

   for (page_ptr=firstPage; page_ptr != NULL; page_ptr=page_ptr->next) {
      for (obj_ptr=page_ptr->bot; obj_ptr != NULL; obj_ptr=obj_ptr->prev) {
         obj_ptr->tmp_parent = NULL;
         if (FixPageNumInObj(obj_ptr, TRUE)) {
            return TRUE;
         }
      }
   }
   return FALSE;
}

static
void ClearPaperColor()
{
   myFileBgPixel = INVALID;
   if (myFileBgColorStr != NULL) {
      free(myFileBgColorStr);
      myFileBgColorStr = NULL;
      XSetWindowBackground(mainDisplay, drawWindow, myBgPixel);
      ClearAndRedrawDrawWindow();
      RecalcXorPixels();
   }
}

static
int RemoveAllBgColorFileAttrs()
   /* This cannot be undone! */
{
   struct AttrRec *attr_ptr=NULL;
   int modified=FALSE;

   for (attr_ptr=tgifObj->fattr; attr_ptr != NULL;
         attr_ptr=attr_ptr->next) {
      if (strcmp(attr_ptr->attr_name.s, "bg_color=") == 0) {
         UnlinkAttr(attr_ptr);
         FreeTextObj(attr_ptr->obj);
         FreeAttr(attr_ptr);

         modified = TRUE;
      }
   }
   return modified;
}

void SetPaperColor()
{
   char spec[MAXSTRING+1];
   int new_alloc=FALSE, index=0;

   if (!OkToFlushUndoBuffer(TgLoadString(STID_SET_PAPER_COLOR_CAUSE_FLUSH))) {
      return;
   }
   *spec = '\0';
   if (myFileBgColorStr == NULL) {
      Dialog(TgLoadString(STID_SPECIFY_A_PAPER_COLOR), NULL, spec);
   } else {
      sprintf(gszMsgBox, TgLoadString(STID_SPECIFY_A_PAPER_COLOR_CUR_IS),
            myFileBgColorStr);
      Dialog(gszMsgBox, TgLoadString(STID_ENTER_NONE_TO_REMOVE_COLOR), spec);
   }
   UtilTrimBlanks(spec);
   if (*spec == '\0') return;

   if (myFileBgColorStr != NULL) {
      if (UtilStrICmp(spec, myFileBgColorStr) == 0) {
         return;
      }
   } else if (UtilStrICmp(spec, "none") == 0) {
      return;
   }
   MakeQuiescent();
   CleanUpCmds();

   if (myFileBgColorStr != NULL) {
      if (UtilStrICmp(spec, myFileBgColorStr) == 0) {
         SetCurChoice(curChoiceBeforeMakeQuiescent);
         return;
      } else if (UtilStrICmp(spec, "none") == 0) {
         ClearPaperColor();
         if (RemoveAllBgColorFileAttrs()) {
            Msg(TgLoadString(STID_PAPER_COLOR_REMOVED));
            SetFileModified(TRUE);
         }
         SetCurChoice(curChoiceBeforeMakeQuiescent);
         return;
      }
   }
   index = QuickFindColorIndex(NULL, spec, &new_alloc, FALSE);

   if (index == INVALID) {
      sprintf(gszMsgBox, TgLoadString(STID_FAIL_ALLOC_NAMED_COLOR), spec);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      SetCurChoice(curChoiceBeforeMakeQuiescent);
      return;
   }
   myFileBgColorStr = UtilStrDup(spec);
   if (myFileBgColorStr == NULL) FailAllocMessage();
   if (SetFileBgColor()) {
      ClearAndRedrawDrawWindow();
   } else {
      ClearPaperColor();
   }
   RemoveAllBgColorFileAttrs();

   AddAttrByNameAndValue(tgifObj, "bg_color=", spec);

   sprintf(gszMsgBox, TgLoadString(STID_PAPER_COLOR_SET_TO_GIVEN), spec);
   Msg(gszMsgBox);

   SetFileModified(TRUE);
   SetCurChoice(curChoiceBeforeMakeQuiescent);
}
