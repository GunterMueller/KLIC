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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/mainmenu.c,v 1.8 2011/05/16 16:21:58 william Exp $
 */

#define _INCLUDE_FROM_MAINMENU_C_

#include "tgifdefs.h"

#include "align.e"
#include "choice.e"
#include "color.e"
#include "cursor.e"
#include "dialog.e"
#include "edit.e"
#include "file.e"
#include "font.e"
#include "grid.e"
#include "help.e"
#include "imgproc.e"
#include "mainloop.e"
#include "mainmenu.e"
#include "menu.e"
#include "menuinfo.e"
#include "msg.e"
#include "navigate.e"
#include "obj.e"
#include "page.e"
#include "pattern.e"
#include "raster.e"
#include "setup.e"
#include "shape.e"
#include "special.e"
#include "stretch.e"
#include "strtbl.e"
#include "tangram2.e"
#include "text.e"
#include "util.e"

#define DEF_MAINMENUPINDIST 80

int pinnedMainMenu=FALSE;
int mainMenuPinDistance=DEF_MAINMENUPINDIST;
Window mainMenuWindow=None;

int numStacking=0;
Window *stackingWins=NULL;

int titledPinnedMenu=TRUE;
int btn3PopupModeMenu=FALSE;

static int mainMenuX=0;
static int mainMenuY=0;
static int mainMenuW=0;
static int mainMenuH=0;

typedef struct SubMenuRec {
   Window win;
   int x, y, w, h, extra_index;
} * SubMenuRecPtr;

static struct SubMenuRec subMenuInfo[MAXMENUS+1];

static int savedMainWinX, savedMainWinY;

static
void InitPinnedMenus()
{
   int i;

   for (i=0; i < MAXMENUS+1; i++) subMenuInfo[i].win = None;
}

void InitMainMenu()
{
   int menu_w, menu_h;
   int bg_pixel=(threeDLook ? myLtGryPixel : myBgPixel);
   char *c_ptr;
   XWMHints wmhints;
   XSizeHints sizehints;
   XSetWindowAttributes win_attrs;

   mainMenuPinDistance = DEF_MAINMENUPINDIST;
   if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"MainMenuPinDistance")) !=
         NULL) {
      mainMenuPinDistance = atoi(c_ptr);
      if (mainMenuPinDistance <= 0) {
         sprintf(gszMsgBox, TgLoadString(STID_INVALID_XDEF_USE_ALT_VALUE),
               TOOL_NAME, "MainMenuPinDistance", c_ptr, DEF_MAINMENUPINDIST);
         fprintf(stderr, "%s\n", gszMsgBox);
         mainMenuPinDistance = DEF_MAINMENUPINDIST;
      }
   }
   pinnedMainMenu = FALSE;

   btn3PopupModeMenu = FALSE;
   if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"Btn3PopupModeMenu")) != NULL &&
         UtilStrICmp(c_ptr, "true") == 0) {
      fprintf(stderr, TgLoadString(STID_NAMED_XDEF_IS_OBSOLETE), TOOL_NAME,
            "Btn3PopupModeMenu");
      fprintf(stderr, "\n");
   }
   menuRowsBeforeScroll = 20;
   if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"MenuRowsBeforeScroll")) !=
         NULL) {
      /* well, this is really not used, yet */
      menuRowsBeforeScroll = atoi(c_ptr);
      if (menuRowsBeforeScroll <= 0) {
         sprintf(gszMsgBox, TgLoadString(STID_INVALID_XDEF_USE_ALT_VALUE),
               TOOL_NAME, "MenuRowsBeforeScroll", c_ptr, 20);
         fprintf(stderr, "%s\n", gszMsgBox);
         menuRowsBeforeScroll = 20;
      }
   }
   menuColsBeforeScroll = 26;
   if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"MenuColsBeforeScroll")) !=
         NULL) {
      /* well, this is really not used, yet */
      menuColsBeforeScroll = atoi(c_ptr);
      if (menuColsBeforeScroll <= 0) {
         sprintf(gszMsgBox, TgLoadString(STID_INVALID_XDEF_USE_ALT_VALUE),
               TOOL_NAME, "MenuColsBeforeScroll", c_ptr, 26);
         fprintf(stderr, "%s\n", gszMsgBox);
         menuColsBeforeScroll = 26;
      }
   }
   menu_w = defaultFontWidth;
   menu_h = defaultFontHeight;

   mainMenuX = 0;
   mainMenuY = 0;
   mainMenuW = menu_w+2*brdrW;
   mainMenuH = menu_h+2*brdrW;

   if ((mainMenuWindow=XCreateSimpleWindow(mainDisplay, rootWindow, 0, 0,
         menu_w, menu_h, brdrW, myBorderPixel, bg_pixel)) == 0) {
      FailToCreateWindowMessage("InitMainMenu()", NULL, TRUE);
   }
   win_attrs.save_under = True;
   win_attrs.override_redirect = (titledPinnedMenu ? False : True);
   XChangeWindowAttributes(mainDisplay, mainMenuWindow,
         CWSaveUnder | CWOverrideRedirect, &win_attrs);

   XSelectInput(mainDisplay, mainMenuWindow, StructureNotifyMask |
         ExposureMask | ButtonPressMask | ButtonReleaseMask | KeyPressMask |
         EnterWindowMask | LeaveWindowMask);

   sizehints.flags = PSize | PMinSize | PMaxSize;
   sizehints.width = sizehints.min_width = sizehints.max_width =
         menu_w+2*brdrW;
   sizehints.height = sizehints.min_height = sizehints.max_height =
         menu_h+2*brdrW;

   sizehints.flags |= USPosition | PPosition;
#ifdef NOTR4MODE
   XSetNormalHints(mainDisplay, mainMenuWindow, &sizehints);
#else
   XSetWMNormalHints(mainDisplay, mainMenuWindow, &sizehints);
#endif

   wmhints.flags = InputHint;
   wmhints.input = True;
   XSetWMHints(mainDisplay, mainMenuWindow, &wmhints);

   if (!titledPinnedMenu) {
      XSetTransientForHint(mainDisplay, mainMenuWindow, mainWindow);
   }
   RegisterWM_DELETE_WINDOW(mainMenuWindow);
   XStoreName(mainDisplay, mainMenuWindow, TgLoadCachedString(CSTID_MAIN_MENU));

   InitPinnedMenus();
}

void CleanUpMainMenu()
{
   int i;

   XDestroyWindow(mainDisplay, mainMenuWindow);

   for (i=0; i < MAXMENUS+1; i++) {
      if (subMenuInfo[i].win != None) {
         XDestroyWindow(mainDisplay, subMenuInfo[i].win);
      }
      subMenuInfo[i].win = None;
   }
}

static int mainMenuMoveDX=0, mainMenuMoveDY=0;

void SaveMainWinPosition(x, y)
   unsigned int x, y;
{
   mainMenuMoveDX = x - savedMainWinX;
   mainMenuMoveDY = y - savedMainWinY;
    
   savedMainWinX = x;
   savedMainWinY = y;
}

void MoveMainMenuWindow(x, y)
   unsigned int x, y;
{
   mainMenuMoveDX = x - savedMainWinX;
   mainMenuMoveDY = y - savedMainWinY;
    
   if (x == savedMainWinX && y == savedMainWinX) return;

   mainMenuX += mainMenuMoveDX;
   mainMenuY += mainMenuMoveDY;

   /* no need to move the windows */
   /* XMoveWindow(mainDisplay, mainMenuWindow, mainMenuX, mainMenuY); */

   savedMainWinX = x;
   savedMainWinY = y;
}
    
void GetPopupXY(win, px, py)
   Window win;
   int *px, *py;
{
   *px = *py = 0;
   for (;;) {
      Window root_win=None, parent_win=None, *child_wins=NULL;
      unsigned int num_child=0;
      XWindowAttributes win_attrs;

      XGetWindowAttributes(mainDisplay, win, &win_attrs);
      *px += win_attrs.x;
      *py += win_attrs.y;
      if (XQueryTree(mainDisplay, win, &root_win,
            &parent_win, &child_wins, &num_child) == 0) {
         return;
      }
      if (child_wins != NULL) XFree((void*)child_wins);
      if (parent_win != rootWindow) {
         win = parent_win;
      } else {
         break;
      }
   }
}

void GetPopupWH(win, pw, ph)
   Window win;
   int *pw, *ph;
{
   XWindowAttributes win_attrs;

   XGetWindowAttributes(mainDisplay, win, &win_attrs);
   *pw = win_attrs.width;
   *ph = win_attrs.height;
}

static char checkExtra[33];

void SaveStackingOrder()
{
   register int i, j;
   register unsigned char hash_value;
   Window root_win, parent_win, *child_wins=NULL;
   unsigned int num_child;

   for (i=0; i < 33; i++) checkExtra[i] = '\0';
   if (pinnedMainMenu) {
      hash_value = mainMenuWindow & 0xff;
      checkExtra[hash_value>>3] |= (1<<(hash_value&0x7));
   }
   for (i = 0; i < numExtraWins; i++) {
      if (extraWinInfo[i].raise && extraWinInfo[i].mapped &&
            extraWinInfo[i].window != None) {
         hash_value = extraWinInfo[i].window & 0xff;
         checkExtra[hash_value>>3] |= (1<<(hash_value&0x7));
      }
   }

   XQueryTree(mainDisplay, rootWindow, &root_win, &parent_win, &child_wins,
         &num_child);

   numStacking = 0;
   if (stackingWins != NULL) free(stackingWins);
   stackingWins = (Window*)malloc((numExtraWins+1)*sizeof(Window));
   if (stackingWins == NULL) FailAllocMessage();

   for (i = 0; i < num_child; i++) {
      hash_value = child_wins[i] & 0xff;
      if (checkExtra[hash_value>>3] & (1<<(hash_value&0x7))) {
         if (child_wins[i] == mainMenuWindow) {
            stackingWins[numStacking++] = mainMenuWindow;
         } else {
            for (j = 0; j < numExtraWins; j++) {
               if (extraWinInfo[j].raise && extraWinInfo[j].mapped &&
                     extraWinInfo[j].window == child_wins[i]) {
                  stackingWins[numStacking++] = child_wins[i];
                  break;
               }
            }
         }
      }
   }
   if (child_wins != NULL) XFree((void*)child_wins);
}

static
void RepositionSubMenuWindow(win, menu_index, OrigCursorX, OrigCursorY)
   Window win;
   int menu_index, OrigCursorX, OrigCursorY;
{
   int moving=TRUE, x, y, w, h;
   XEvent input;

   x = subMenuInfo[menu_index].x;
   y = subMenuInfo[menu_index].y;
   w = subMenuInfo[menu_index].w;
   h = subMenuInfo[menu_index].h;

   if (!debugNoPointerGrab) {
      XGrabPointer(mainDisplay, win, FALSE,
         PointerMotionMask | ButtonReleaseMask,
         GrabModeAsync, GrabModeAsync, None, handCursor, CurrentTime);
   }
   XSetSubwindowMode(mainDisplay, revDefaultGC, IncludeInferiors);
   XDrawRectangle(mainDisplay, rootWindow, revDefaultGC, x, y, w, h);
    
   while (moving) {
      XNextEvent(mainDisplay, &input);

      if (input.type == Expose || input.type == VisibilityNotify) {
         ExposeEventHandler(&input, TRUE);
      } else if (input.type == ButtonRelease) {
         XDrawRectangle(mainDisplay, rootWindow, revDefaultGC, x, y, w, h);
         XSetSubwindowMode(mainDisplay, revDefaultGC, ClipByChildren);
         XMoveWindow(mainDisplay, win, x, y);
         XRaiseWindow(mainDisplay, win);
         subMenuInfo[menu_index].x = x;
         subMenuInfo[menu_index].y = y;

         moving = FALSE;
         XUngrabPointer(mainDisplay, CurrentTime);
         XDefineCursor(mainDisplay, win, defaultCursor);
      } else if (input.type == MotionNotify) {
         XDrawRectangle(mainDisplay, rootWindow, revDefaultGC, x, y, w, h);
         x = input.xbutton.x_root-OrigCursorX+subMenuInfo[menu_index].x;
         y = input.xbutton.y_root-OrigCursorY+subMenuInfo[menu_index].y;
         XDrawRectangle(mainDisplay, rootWindow, revDefaultGC, x, y, w, h);
      }
   }
}

static
void TgSubMenuExposeHandler(input, win_info)
   XEvent *input;
   struct WinInfoRec *win_info;
{
   int menu_index;
   Window win;
   XEvent ev;

   win = input->xany.window;
   while (XCheckWindowEvent(mainDisplay, win, ExposureMask, &ev));
   while (XCheckWindowEvent(mainDisplay, win, StructureNotifyMask, &ev));

   for (menu_index=0; menu_index <= MAXMENUS+1; menu_index++) {
      if (subMenuInfo[menu_index].win == win && win != None) {
         TgMenu *menu=(TgMenu*)(win_info->userdata);

         if (menu != NULL) TgDrawEntireMenu(menu);
         break;
      }
   }
}

static
void ComputeSubMenuWinXY(win, x, y)
   Window win;
   int *x, *y;
{
   int win_x, win_y, done=FALSE;
   unsigned int win_w, win_h, win_brdr_w, win_d, num_child;
   Window root_win, parent_win, *child_wins=NULL;

   *x = *y = 0;
   while (!done) {
      XGetGeometry(mainDisplay, win, &root_win, &win_x, &win_y, &win_w,
            &win_h, &win_brdr_w, &win_d);
      *x += win_x;
      *y += win_y;
      if (XQueryTree(mainDisplay, win, &root_win, &parent_win, &child_wins,
            &num_child) == 0) {
         return;
      }
      if (child_wins != NULL) XFree((void*)child_wins);
      if (parent_win == rootWindow) {
         done = TRUE;
      } else {
         win = parent_win;
      }
   }
}

static
void TgUpdateMenuBBox(menu, pn_win_x, pn_win_y)
   TgMenu *menu;
   int *pn_win_x, *pn_win_y;
{
   int dx=0, dy=0;

   ComputeSubMenuWinXY(menu->window, pn_win_x, pn_win_y);
   dx = (*pn_win_x) - menu->bbox.ltx;
   dy = (*pn_win_y) - menu->bbox.lty;
   menu->bbox.ltx += dx; menu->bbox.lty += dy;
   menu->bbox.rbx += dx; menu->bbox.rby += dy;
}

static
void TgShowPullDownStatus(menu, new_selected)
   TgMenu *menu;
   int new_selected;
{
   TgMenuItem *menuitems=menu->menuitems;

   if (menuitems[new_selected].status_str != NULL) {
      if (!titledPinnedMenu) {
         SetMouseStatus(TgLoadCachedString(CSTID_MOVE_PINNED_MENU),
               menuitems[new_selected].status_str,
               TgLoadCachedString(CSTID_CLOSE_PINNED_MENU));
      } else {
         SetMouseStatus(menuitems[new_selected].status_str, "",
               TgLoadCachedString(CSTID_CLOSE_PINNED_MENU));
      }
   }
}

static
void TgPullDownFromSubMenu(menu, new_selected)
   TgMenu *menu;
   int new_selected;
{
   TgMenuItem *menuitems=menu->menuitems;
   int rc=BAD, last_selected_non_submenu_item=FALSE;
   XEvent ev;

   while (rc == BAD || rc == (-3)) {
      if (new_selected != INVALID &&
            (menuitems[new_selected].flags & TGMU_DISABLED) == 0) {
         if ((menuitems[new_selected].flags & TGMU_HAS_SUBMENU) ==
               TGMU_HAS_SUBMENU &&
               menuitems[new_selected].submenu_create_info != NULL) {
            /*
             * highlight the submenu item, popup the submenu,
             * and unhighlight it
             */
            int win_x=0, win_y=0;
            TgMenu *submenu=NULL;

            menuitems[new_selected].state = TGBS_RAISED;
            TgDrawMenuItem(menu, &menuitems[new_selected]);
            TgShowPullDownStatus(menu, new_selected);

            TgUpdateMenuBBox(menu, &win_x, &win_y);
   
            submenu = TgCreatePopUpSubMenu(menu, new_selected);
            if (submenu != NULL) {
               submenu->track_menubar = FALSE;
               submenu->track_parent_menu = TRUE;

               rc = TgPopUpSubMenu(menu, win_x, win_y);

               submenu = menuitems[new_selected].detail.submenu;
               if (submenu != NULL) {
                  TgDestroyMenu(submenu, TRUE);
                  menuitems[new_selected].detail.submenu = NULL;
               }
            }
            menuitems[menu->selected_index].state = TGBS_NORMAL;
            TgDrawMenuItem(menu, &menuitems[menu->selected_index]);
         } else {
            if (new_selected != last_selected_non_submenu_item) {
               menuitems[new_selected].state = TGBS_RAISED;
               TgDrawMenuItem(menu, &menuitems[new_selected]);
               TgShowPullDownStatus(menu, new_selected);
               last_selected_non_submenu_item = new_selected;
            }
         }
      }
      if (rc == BAD || rc == (-3)) {
         Window root_win, child_win;
         int mouse_x, mouse_y, root_x, root_y;
         unsigned int status;

         XQueryPointer(mainDisplay, menu->window, &root_win, &child_win,
               &root_x, &root_y, &mouse_x, &mouse_y, &status);
         new_selected = TgWhichMenuIndex(menu, mouse_x, mouse_y, TRUE);
         if (!(status & BUTTONSMASK)) {
            break;
         }
         if (!(status & BUTTONSMASK) && !(mouse_x >= 0 && mouse_x <
               menu->bbox.rbx-menu->bbox.ltx && mouse_y >= 0 &&
               mouse_y < menu->bbox.rby-menu->bbox.lty)) {
            break;
         }
         if (new_selected != last_selected_non_submenu_item &&
               last_selected_non_submenu_item != INVALID) {
            menuitems[last_selected_non_submenu_item].state = TGBS_NORMAL;
            TgDrawMenuItem(menu, &menuitems[last_selected_non_submenu_item]);
            last_selected_non_submenu_item = INVALID;
         }
         switch (new_selected) {
         case INVALID: break;
         case (-2): new_selected = INVALID; break; /* separator */
         case (-3): new_selected = INVALID; break; /* scrollbar */
         default: break;
         }
         menu->selected_index = new_selected;
      }
   }
   XSync(mainDisplay, False);
   while (XCheckWindowEvent(mainDisplay, menu->window,
         ButtonPressMask|ButtonReleaseMask|PointerMotionMask, &ev)) ;
}

static
int MouseInsideWindow(win, win_w, win_h)
   Window win;
   int win_w, win_h;
{
   Window root_win, child_win;
   int x, y, root_x, root_y;
   unsigned int status;

   XQueryPointer(mainDisplay, win, &root_win, &child_win,
         &root_x, &root_y, &x, &y, &status);
   return (x >= 0 && x < win_w && y >=0 && y < win_h);
}

static
int TgSubMenuEventHandler(input, win_info)
   XEvent *input;
   struct WinInfoRec *win_info;
{
   static TgMenu *clicked_menu=NULL;
   static int clicked_index=INVALID;
   TgMenu *menu=(TgMenu*)(win_info->userdata);
   TgMenuItem *menuitems=menu->menuitems;
   int menu_index=INVALID, new_selected=INVALID, rc=INVALID;
   int menu_w=menu->bbox.rbx-menu->bbox.ltx+((menu->padding)<<1);
   int menu_h=menu->bbox.rby-menu->bbox.lty+((menu->padding)<<1);
   XButtonEvent *button_ev=NULL;
   XEvent ev;
   Window win=None;

   if (input->type == MapNotify || input->type == Expose) {
      TgSubMenuExposeHandler(input, win_info);
      clicked_menu = NULL;
      clicked_index = INVALID;
      return INVALID;
   }
   if (menu == NULL) return INVALID;

   win = input->xany.window;
   for (menu_index=0; menu_index <= MAXMENUS; menu_index++) {
      if (subMenuInfo[menu_index].win == win && win != None) {
         break;
      }
   }
   if (menu_index > MAXMENUS) return INVALID;

   if (menu->scroll_start > 0) {
      switch (menu->type) {
      case TGMUTYPE_TEXT:
         menu_w += scrollBarW + menu->padding;
         menu_h = maxScrollableMenuHeight;
         break;
      case TGMUTYPE_COLOR:
      case TGMUTYPE_BITMAP:
         menu_w = maxScrollableMenuWidth;
         menu_h += scrollBarW + menu->padding;
         break;
      }
   }
   menu->parent_menu = NULL;
   if (input->type == MotionNotify || input->type == ButtonPress ||
         input->type == ButtonRelease) {
      if (!MouseInsideWindow(menu->window, menu_w, menu_h)) {
         return INVALID;
      }
   }
   if (input->type == ConfigureNotify) {
      ComputeSubMenuWinXY(win, &(subMenuInfo[menu_index].x),
            &(subMenuInfo[menu_index].y));
      clicked_menu = NULL;
      clicked_index = INVALID;
   } else if (input->type == EnterNotify) {
      if (menu->selected_index != INVALID) {
         menuitems[menu->selected_index].state = TGBS_NORMAL;
         TgDrawMenuItem(menu, &menuitems[menu->selected_index]);
      }
      menu->selected_index = INVALID;
      clicked_menu = NULL;
      clicked_index = INVALID;
      SetStringStatus("");
   } else if (input->type == LeaveNotify) {
      if (menu->selected_index != INVALID) {
         menuitems[menu->selected_index].state = TGBS_NORMAL;
         TgDrawMenuItem(menu, &menuitems[menu->selected_index]);
      }
      clicked_menu = NULL;
      clicked_index = INVALID;
      menu->selected_index = INVALID;
   } else if (input->type == MotionNotify) {
      while (XCheckWindowEvent(mainDisplay, menu->window,
            PointerMotionMask, &ev)) ;
      new_selected = TgWhichMenuIndex(menu, input->xmotion.x, input->xmotion.y,
            FALSE);

      if (menu->selected_index != new_selected) {
         if (menu->selected_index != INVALID) {
            menuitems[menu->selected_index].state = TGBS_NORMAL;
            TgDrawMenuItem(menu, &menuitems[menu->selected_index]);
         }
         switch (new_selected) {
         case INVALID: break;
         case (-2): new_selected = INVALID; break; /* separator */
         case (-3): new_selected = INVALID; break; /* scrollbar */
         default: break;
         }
         if (new_selected != INVALID) {
            menuitems[new_selected].state = TGBS_RAISED;
            TgDrawMenuItem(menu, &menuitems[new_selected]);
            TgShowPullDownStatus(menu, new_selected);
         }
         menu->selected_index = new_selected;
         if (clicked_menu != NULL && clicked_index != INVALID) {
            clicked_index = INVALID;
         }
      }
   } else if (input->type == ButtonPress) {
      button_ev = &(input->xbutton);
      switch (button_ev->button) {
      case Button1:
         if (!titledPinnedMenu) {
            RepositionSubMenuWindow(win, menu_index, button_ev->x_root,
                  button_ev->y_root);
            break;
         }
         /* dropping through if titledPinnedMenu ! */
      case Button2:
         new_selected = TgWhichMenuIndex(menu, button_ev->x, button_ev->y,
               FALSE);
         clicked_menu = menu;
         clicked_index = (new_selected < 0 ? INVALID : new_selected);
         if (new_selected != INVALID) {
            if (menu->selected_index != new_selected) {
               if (menu->selected_index != INVALID) {
                  menuitems[menu->selected_index].state = TGBS_NORMAL;
                  TgDrawMenuItem(menu, &menuitems[menu->selected_index]);
               }
               switch (new_selected) {
               case INVALID: break;
               case (-2): new_selected = INVALID; break; /* separator */
               case (-3):
                  /* scrollbar */
                  if (menu->scroll_start > 0) {
                     /* loop to scroll */
                     ScrollMenu(menu, button_ev->x, button_ev->y, menu_w,
                           menu_h, (button_ev->button == Button1 ?
                           Button1Mask : Button2Mask));
                  }
                  new_selected = INVALID;
                  break;
               default: break;
               }
               if (new_selected != INVALID) {
                  menuitems[new_selected].state = TGBS_RAISED;
                  TgDrawMenuItem(menu, &menuitems[new_selected]);
                  TgShowPullDownStatus(menu, new_selected);
               }
               menu->selected_index = new_selected;
            }
            if (new_selected != INVALID &&
                  (menuitems[new_selected].flags & TGMU_HAS_SUBMENU) ==
                  TGMU_HAS_SUBMENU &&
                  (menuitems[new_selected].flags & TGMU_DISABLED) == 0 &&
                  menuitems[new_selected].submenu_create_info != NULL) {
               TgPullDownFromSubMenu(menu, new_selected);
               clicked_index = INVALID;
            }
         }
         break;
      case Button3:
         if (TgWhichMenuIndex(menu, button_ev->x, button_ev->y, FALSE) !=
               (-3)) {
            clicked_menu = NULL;
            clicked_index = INVALID;
         } else if (!threeDLook && menu->scroll_start > 0) {
            /* loop to scroll */
            ScrollMenu(menu, button_ev->x, button_ev->y, menu_w,
                  menu_h, Button3Mask);
         }
         break;
      }
   } else if (input->type == ButtonRelease) {
      button_ev = &(input->xbutton);
      switch (button_ev->button) {
      case Button1:
         if (!titledPinnedMenu) {
            break;
         }
         /* dropping through if titledPinnedMenu ! */
      case Button2:
         new_selected = TgWhichMenuIndex(menu, button_ev->x, button_ev->y,
               FALSE);
         if (new_selected != INVALID) {
            if (menu->selected_index != new_selected) {
               if (menu->selected_index != INVALID) {
                  menuitems[menu->selected_index].state = TGBS_NORMAL;
                  TgDrawMenuItem(menu, &menuitems[menu->selected_index]);
               }
               switch (new_selected) {
               case INVALID: break;
               case (-2): new_selected = INVALID; break; /* separator */
               case (-3): new_selected = INVALID; break; /* scrollbar */
               default: break;
               }
               if (new_selected != INVALID) {
                  menuitems[new_selected].state = TGBS_RAISED;
                  TgDrawMenuItem(menu, &menuitems[new_selected]);
                  TgShowPullDownStatus(menu, new_selected);
               }
               menu->selected_index = new_selected;
            }
            if (new_selected != INVALID &&
                  (menuitems[new_selected].flags & TGMU_HAS_SUBMENU) ==
                  TGMU_HAS_SUBMENU &&
                  (menuitems[new_selected].flags & TGMU_DISABLED) == 0 &&
                  menuitems[new_selected].submenu_create_info != NULL) {
               TgPullDownFromSubMenu(menu, new_selected);
               clicked_index = INVALID;
            } else if (menu->menuitems[new_selected].cmdid != INVALID &&
                  ((menu->menuitems[new_selected].flags & TGMU_DISABLED) !=
                  TGMU_DISABLED)) {
               if (clicked_menu != NULL && clicked_index != INVALID &&
                     clicked_index == new_selected) {
                  SendCommandToSelf(menu->menuitems[new_selected].cmdid,
                        new_selected);
               }
            }
         }
         break;
      case Button3:
         if (TgWhichMenuIndex(menu, button_ev->x, button_ev->y, FALSE) !=
               (-3)) {
            if (button_ev->x>=0 && button_ev->x<=subMenuInfo[menu_index].w &&
                  button_ev->y>=0 && button_ev->y<=subMenuInfo[menu_index].h) {
               XDestroyWindow(mainDisplay, subMenuInfo[menu_index].win);
               subMenuInfo[menu_index].win = None;
               extraWinInfo[subMenuInfo[menu_index].extra_index].window = None;
               TgDestroyMenu(menu, TRUE);
               win_info->userdata = NULL;
            }
            clicked_menu = NULL;
            clicked_index = INVALID;
         }
         break;
      }
   } else if (IsWM_DELETE_WINDOW(input)) {
      XDestroyWindow(mainDisplay, subMenuInfo[menu_index].win);
      subMenuInfo[menu_index].win = None;
      extraWinInfo[subMenuInfo[menu_index].extra_index].window = None;
      TgDestroyMenu(menu, TRUE);
      win_info->userdata = NULL;
      clicked_menu = NULL;
      clicked_index = INVALID;
   }
   return rc;
}

static
void TgSubMenuCleanUp(win_info)
   struct WinInfoRec *win_info;
{
   TgMenu *menu=(TgMenu*)(win_info->userdata);

   if (menu != NULL) {
      TgDestroyMenu(menu, TRUE);
      win_info->userdata = NULL;
   }
}

Window TgRealizePinnedMenuWindow(menu, win_x, win_y, win_w, win_h)
   TgMenu *menu;
   int win_x, win_y, win_w, win_h;
{
   Window win;
   XWMHints wmhints;
   XSizeHints sizehints;
   XSetWindowAttributes win_attrs;
   TgMenu *dup_menu=NULL;
   int bg_pixel=(threeDLook ? myLtGryPixel : myBgPixel);
   TgMenuItemInfo *item_info=NULL;

   win_w -= (brdrW<<1);
   win_h -= (brdrW<<1);

   if ((win=XCreateSimpleWindow(mainDisplay, rootWindow, win_x, win_y,
         win_w, win_h, brdrW, myBorderPixel, bg_pixel)) == 0) {
      FailToCreateWindowMessage("TgRealizePinnedMenuWindow()", NULL, TRUE);
   }
   XDefineCursor(mainDisplay, win, defaultCursor);

   win_attrs.save_under = True;
   win_attrs.override_redirect = (titledPinnedMenu ? False : True);
   win_attrs.colormap = mainColormap;
   XChangeWindowAttributes(mainDisplay, win,
         CWSaveUnder | CWOverrideRedirect | CWColormap, &win_attrs);

   if (!titledPinnedMenu) {
      XSetTransientForHint(mainDisplay, win, mainWindow);
   }
   if (activeMenu == MENU_MAIN) {
      XStoreName(mainDisplay, win, TgLoadCachedString(CSTID_MAIN_MENU));
   } else {
      for (item_info=mainMenuInfo.items; item_info->menu_str != NULL;
            item_info++) {
         if (item_info->menu_str != TGMUITEM_SEPARATOR &&
               item_info->cmdid == activeMenu) {
            XStoreName(mainDisplay, win, item_info->status_str);
            break;
         }
      }
   }
   sizehints.flags = PSize | PMinSize | PMaxSize;
   sizehints.width = sizehints.min_width = sizehints.max_width = win_w;
   sizehints.height = sizehints.min_height = sizehints.max_height = win_h;

   sizehints.flags |= USPosition | PPosition;
#ifdef NOTR4MODE
   XSetNormalHints(mainDisplay, win, &sizehints);
#else
   XSetWMNormalHints(mainDisplay, win, &sizehints);
#endif

   wmhints.flags = InputHint;
   wmhints.input = True;
   XSetWMHints(mainDisplay, win, &wmhints);

   RegisterWM_DELETE_WINDOW(win);

#ifdef MAPBEFORESELECT
   XMapWindow(mainDisplay, win);
   XSelectInput(mainDisplay, win, StructureNotifyMask |
         ExposureMask | ButtonPressMask | ButtonReleaseMask | KeyPressMask |
         EnterWindowMask | LeaveWindowMask | PointerMotionMask);
#else
   XSelectInput(mainDisplay, win, StructureNotifyMask |
         ExposureMask | ButtonPressMask | ButtonReleaseMask | KeyPressMask |
         EnterWindowMask | LeaveWindowMask | PointerMotionMask);
   XMapWindow(mainDisplay, win);
#endif

   if (subMenuInfo[activeMenu].win != None) {
      int extra_index=subMenuInfo[activeMenu].extra_index;

      XDestroyWindow(mainDisplay, subMenuInfo[activeMenu].win);
      extraWinInfo[extra_index].window = None;
      TgDestroyMenu(menu, FALSE);
      dup_menu = (TgMenu*)(extraWinInfo[extra_index].userdata);
   } else {
      if ((dup_menu=(TgMenu*)malloc(sizeof(TgMenu))) == NULL) {
         FailAllocMessage();
      }
      memcpy(dup_menu, menu, sizeof(TgMenu));
   }
   dup_menu->window = win;
   subMenuInfo[activeMenu].extra_index =
         AddExtraWinInfo(win, TRUE, TRUE, TgSubMenuExposeHandler,
         TgSubMenuEventHandler, TgSubMenuCleanUp, dup_menu);

   subMenuInfo[activeMenu].x = win_x;
   subMenuInfo[activeMenu].y = win_y;
   subMenuInfo[activeMenu].w = win_w;
   subMenuInfo[activeMenu].h = win_h;
   subMenuInfo[activeMenu].win = win;

   XDestroyWindow(mainDisplay, menu->window);
   menu->window = None;
   memset(menu, 0, sizeof(TgMenu));

   return win;
}

int TgWindowIsPinnedMenu(win, menu_index)
   Window win;
   int menu_index;
{
   return (subMenuInfo[menu_index].win != None &&
         subMenuInfo[menu_index].win == win);
}

int TgHandlePinnedMenuEvent(win, menu_index, input)
   Window win;
   int menu_index;
   XEvent *input;
{
   if (TgWindowIsPinnedMenu(win, menu_index)) {
      int extra_win_info_index=subMenuInfo[menu_index].extra_index;

      return (*(extraWinInfo[extra_win_info_index].ev_handler))(
            input, &extraWinInfo[extra_win_info_index]);
   }
   return INVALID;
}

void UpdatePinnedMenu(menu_index)
   int menu_index;
{
   Window win=subMenuInfo[menu_index].win;

   if (win != None) {
      int extra_win_info_index=subMenuInfo[menu_index].extra_index;

      if (extra_win_info_index >= 0 && extra_win_info_index < numExtraWins &&
            extraWinInfo[extra_win_info_index].window == win) {
         TgMenu *menu=(TgMenu*)(extraWinInfo[extra_win_info_index].userdata);

         if (menu != NULL) {
            switch (menu_index) {
            case MENU_FILE: RefreshFileMenu(menu); break;
            case MENU_EDIT: RefreshEditMenu(menu); break;
            case MENU_LAYOUT: RefreshLayoutMenu(menu); break;
            case MENU_ARRANGE: RefreshArrangeMenu(menu); break;
            case MENU_PROPERTIES: RefreshPropertiesMenu(menu); break;

            case MENU_MOVEMODE: RefreshMoveModeMenu(menu); break;
            case MENU_HORIALIGN: RefreshHoriAlignMenu(menu); break;
            case MENU_VERTALIGN: RefreshVertAlignMenu(menu); break;
            case MENU_FONT: RefreshFontMenu(menu); break;
            case MENU_STYLE: RefreshFontStyleMenu(menu); break;
            case MENU_SIZE: RefreshFontSizeMenu(menu); break;
            case MENU_SHAPE: RefreshShapeMenu(menu); break;
            case MENU_STRETCHTEXT: RefreshStretchableTextModeMenu(menu); break;
            case MENU_TRANSPAT: RefreshTransPatModeMenu(menu); break;

            case MENU_LINEWIDTH: RefreshLineWidthMenu(menu); break;
            case MENU_LINESTYLE: RefreshLineStyleMenu(menu); break;
            case MENU_LINETYPE: RefreshLineTypeMenu(menu); break;
            case MENU_LINEDASH: RefreshLineDashMenu(menu); break;
            case MENU_FILL: RefreshFillMenu(menu); break;
            case MENU_PEN: RefreshPenMenu(menu); break;

            case MENU_COLOR: RefreshColorMenu(menu); break;

            case MENU_IMAGEPROC: RefreshImageProcMenu(menu); break;
            case MENU_NAVIGATE: RefreshNavigateMenu(menu); break;
            case MENU_SPECIAL: RefreshSpecialMenu(menu); break;
            case MENU_PAGE: RefreshPageMenu(menu); break;
            case MENU_PAGELAYOUT: RefreshPageLayoutMenu(menu); break;
            case MENU_HELP: RefreshHelpMenu(menu); break;
            case MENU_TANGRAM2:
               if (cmdLineTgrm2) {
                  RefreshTangram2Menu(menu);
               }
               break;

            case MENU_MODE: RefreshModeMenu(menu); break;

            case MENU_MAIN: RefreshMainMenu(menu); break;
            }
            TgDrawEntireMenu(menu);
            return;
         }
      }
   }
}

void UpdateAllPinnedMenus()
{
   UpdatePinnedMenu(MENU_HORIALIGN);
   UpdatePinnedMenu(MENU_VERTALIGN);
   UpdatePinnedMenu(MENU_STYLE);
   UpdatePinnedMenu(MENU_FONT);
   UpdatePinnedMenu(MENU_SIZE);
   UpdatePinnedMenu(MENU_LINEWIDTH);
   UpdatePinnedMenu(MENU_LINESTYLE);
   UpdatePinnedMenu(MENU_LINETYPE);
   UpdatePinnedMenu(MENU_LINEDASH);
   UpdatePinnedMenu(MENU_LAYOUT);
   UpdatePinnedMenu(MENU_MOVEMODE);
   UpdatePinnedMenu(MENU_STRETCHTEXT);
   UpdatePinnedMenu(MENU_TRANSPAT);
   UpdatePinnedMenu(MENU_PAGELAYOUT);

   UpdatePinnedMenu(MENU_MAIN);
}

void UseWireMenuItemInModeItem(connecting)
   int connecting;
{
   Window win=subMenuInfo[MENU_MODE].win;

   if (win != None) {
      int extra_win_info_index=subMenuInfo[MENU_MODE].extra_index;

      if (extra_win_info_index >= 0 && extra_win_info_index < numExtraWins &&
            extraWinInfo[extra_win_info_index].window == win) {
         TgMenu *menu=(TgMenu*)(extraWinInfo[extra_win_info_index].userdata);

         if (menu != NULL) {
            TgMenuItem stMenuItem;

            memset(&stMenuItem, 0, sizeof(TgMenuItem));
            if (connecting) {
               stMenuItem.menu_str = (char*)(&wireBitmap);
            } else {
               stMenuItem.menu_str = (char*)(&choicePixmap[DRAWPOLY]);
            }
            if (!TgSetMenuItemInfo(&menu->menuitems[DRAWPOLY],
                  TGMU_MASK_MENUSTR, &stMenuItem)) {
            }
            return;
         }
      }
   }
}

void DestroyPinnedMenu(menu_index)
   int menu_index;
{
   if (subMenuInfo[menu_index].win != None) {
      int extra_index=subMenuInfo[menu_index].extra_index;
      TgMenu *menu=(TgMenu*)(extraWinInfo[extra_index].userdata);

      XDestroyWindow(mainDisplay, subMenuInfo[menu_index].win);
      subMenuInfo[menu_index].win = None;
      extraWinInfo[extra_index].window = None;
      if (menu != NULL) {
         TgDestroyMenu(menu, TRUE);
         extraWinInfo[extra_index].userdata = NULL;
      }
   }
}

static IntPoint gaPopupCoords[MAXMENUS+5];

void HidePopupMenusForSlideShow()
{
   int adj_caches=(zoomScale!=0 || zoomedIn!=FALSE);
   int menu_index=0, draw_win_x=0, draw_win_y=0, draw_win_w=0, draw_win_h=0;
   int dpy_w=DisplayWidth(mainDisplay,mainScreen);
   int dpy_h=DisplayHeight(mainDisplay,mainScreen);

   for (menu_index=0; menu_index <= MAXMENUS; menu_index++) {
      if (subMenuInfo[menu_index].win != None) {
         GetPopupXY(subMenuInfo[menu_index].win,
               &gaPopupCoords[menu_index].x, &gaPopupCoords[menu_index].y);
         XMoveWindow(mainDisplay, subMenuInfo[menu_index].win, -dpy_w, -dpy_h);
      }
   }
   GetPopupXY(drawWindow, &draw_win_x, &draw_win_y);
   GetPopupWH(drawWindow, &draw_win_w, &draw_win_h);

   GetPopupXY(mainWindow, &gaPopupCoords[MAXMENUS+1].x,
         &gaPopupCoords[MAXMENUS+1].y);
   GetPopupWH(mainWindow, &gaPopupCoords[MAXMENUS+2].x,
         &gaPopupCoords[MAXMENUS+2].y);
   gaPopupCoords[MAXMENUS+3].x = drawOrigX;
   gaPopupCoords[MAXMENUS+3].y = drawOrigY;
   gaPopupCoords[MAXMENUS+4].x = zoomedIn;
   gaPopupCoords[MAXMENUS+4].y = zoomScale;

   if (gaPopupCoords[MAXMENUS+3].x != 0 || gaPopupCoords[MAXMENUS+3].y != 0 ||
         adj_caches) {
      drawOrigX = drawOrigY = 0;
      zoomedIn = FALSE;
      zoomScale = 0;

      UpdDrawWinWH();
      UpdDrawWinBBox();
      ClearObjCachesInAllPages();
      if (adj_caches) { ShowZoom(); }
   }
#ifndef NOKDE3BUG
   XMoveResizeWindow(mainDisplay, mainWindow, 0, 0,
         dpy_w+gaPopupCoords[MAXMENUS+2].x-draw_win_w+2,
         dpy_h+gaPopupCoords[MAXMENUS+2].y-draw_win_h+2);
#endif /* ~NOKDE3BUG */
   XMoveResizeWindow(mainDisplay, mainWindow,
         gaPopupCoords[MAXMENUS+1].x-draw_win_x-1,
         gaPopupCoords[MAXMENUS+1].y-draw_win_y-1,
         dpy_w+gaPopupCoords[MAXMENUS+2].x-draw_win_w+2,
         dpy_h+gaPopupCoords[MAXMENUS+2].y-draw_win_h+2);
}

void ShowPopupMenusForSlideShow()
{
   int menu_index=0;

   zoomScale = gaPopupCoords[MAXMENUS+4].y;
   zoomScale = gaPopupCoords[MAXMENUS+4].y;
   drawOrigX = gaPopupCoords[MAXMENUS+3].x;
   drawOrigY = gaPopupCoords[MAXMENUS+3].y;
   drawWinW = gaPopupCoords[MAXMENUS+2].x;
   drawWinH = gaPopupCoords[MAXMENUS+2].y;
   XMoveResizeWindow(mainDisplay, mainWindow,
         gaPopupCoords[MAXMENUS+1].x, gaPopupCoords[MAXMENUS+1].y,
         gaPopupCoords[MAXMENUS+2].x, gaPopupCoords[MAXMENUS+2].y);
   InitWinSizes();
   for (menu_index=0; menu_index <= MAXMENUS; menu_index++) {
      if (subMenuInfo[menu_index].win != None) {
         XMoveWindow(mainDisplay, subMenuInfo[menu_index].win,
               gaPopupCoords[menu_index].x, gaPopupCoords[menu_index].y);
         UpdatePinnedMenu(menu_index);
      }
   }
}

void EnumPopupMenuWindow(pFunc, pUserData)
   ENUMPOPUPMENUFN *pFunc;
   void *pUserData;
{
   int menu_index=0;

   for (menu_index=0; menu_index <= MAXMENUS; menu_index++) {
      if (subMenuInfo[menu_index].win != None) {
         if (!((pFunc)(menu_index, subMenuInfo[menu_index].win, pUserData))) {
            break;
         }
      }
   }
}
