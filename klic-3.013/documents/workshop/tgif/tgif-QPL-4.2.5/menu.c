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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/menu.c,v 1.17 2011/05/16 16:21:58 william Exp $
 */

#define _INCLUDE_FROM_MENU_C_

#include "tgifdefs.h"
#include "cmdids.h"

#include "align.e"
#include "box.e"
#include "choice.e"
#include "cmd.e"
#include "color.e"
#include "cursor.e"
#include "dialog.e"
#include "drawing.e"
#include "edit.e"
#include "exec.e"
#include "file.e"
#include "font.e"
#include "grid.e"
#include "help.e"
#include "imgproc.e"
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
#include "pattern.e"
#include "raster.e"
#include "rect.e"
#include "remote.e"
#include "scroll.e"
#include "select.e"
#include "setup.e"
#include "shape.e"
#include "special.e"
#include "stk.e"
#include "strtbl.e"
#include "tangram2.e"
#include "text.e"
#include "util.e"
#include "version.e"

#include "xbm/check.xbm"
#include "xbm/submenu.xbm"

int iconWindowCreated=FALSE;
int iconWindowShown=FALSE;
int importingIconFile=FALSE;

int showVersion=FALSE;
int activeMenu=INVALID;

#define SEPARATOR_PADDING 1

static int separatorHeight=SEPARATOR_PADDING;

GC textMenuGC=(GC)0;
GC rvPixmapMenuGC=(GC)0;

int menuRowsBeforeScroll=21;
int menuColsBeforeScroll=27;

int maxScrollableMenuWidth=0;
int maxScrollableMenuHeight=0;

int deleteCmdAsCut=FALSE;

MenuDontSendCommandInfo gstMenuDontSendCommandInfo;

static int savedZoomScale=0, savedDrawOrigX=0, savedDrawOrigY=0;
static int savedZoomedIn=FALSE;
static int savedDrawWinW=0, savedDrawWinH=0, savedFileModified=FALSE;

static struct BBRec excludeMenubarWinBBox;
static int excludeMenubarIndex=INVALID;

static int gnNumMenubarItems=0;
static int gnNumMainMenuItems=0;
/*
 * gpMenubarItemInfos[i].menu_str and gpMenubarItemInfos[i].status_str
 *       are raw English strings
 */
static TgMenuItemInfo *gpMenubarItemInfos=NULL;
static TgMenuItemInfo *gpMainMenuItemInfos=NULL;

static int gnMinimalMenubar=TRUE;
static int gnAutoWrapMenubar=FALSE;

static
void BuildMenubarInfo()
{
   static int nInitialized=FALSE;
   char *c_ptr=NULL;
   TgMenuItemInfo *item_info=NULL;

   if (nInitialized) return;

   nInitialized = TRUE;

   gnMinimalMenubar = TRUE;
   if (((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"MinimalMenubar")) !=
         NULL) && UtilStrICmp(c_ptr,"false") == 0) {
      gnMinimalMenubar = FALSE;
   }
   gnAutoWrapMenubar = FALSE;
   if (((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"AutoWrapMenubar")) !=
         NULL) && UtilStrICmp(c_ptr,"true") == 0) {
      gnAutoWrapMenubar = TRUE;
   }
   /* menubar */
   gpMenubarItemInfos = (gnMinimalMenubar ? minimalMenubarMenuInfo.items :
         maximalMenubarMenuInfo.items);
   gnNumMenubarItems = 0;
   for (item_info=gpMenubarItemInfos; item_info->menu_str != NULL;
         item_info++) {
      gnNumMenubarItems++;
   }
   if (!cmdLineTgrm2) {
      int i=0, j=0;
      TgMenuItemInfo *pMenubarItemInfos=(TgMenuItemInfo*)malloc(
            (gnNumMenubarItems+1)*sizeof(TgMenuItemInfo));

      for (i=0; i <= gnNumMenubarItems; i++) {
         if (gpMenubarItemInfos[i].cmdid != MENU_TANGRAM2) {
            memcpy(&pMenubarItemInfos[j++], &gpMenubarItemInfos[i],
                  sizeof(TgMenuItemInfo));
         }
      }
      memcpy(&pMenubarItemInfos[j], &gpMenubarItemInfos[i],
            sizeof(TgMenuItemInfo));

      gpMenubarItemInfos = pMenubarItemInfos;
      gnNumMenubarItems--;
   }

   /* main menu */
   gpMainMenuItemInfos = mainMenuInfo.items;
   gnNumMainMenuItems = 0;
   for (item_info=gpMainMenuItemInfos; item_info->menu_str != NULL;
         item_info++) {
      gnNumMainMenuItems++;
   }
   if (!cmdLineTgrm2) {
      int i=0, j=0;
      TgMenuItemInfo *pMainMenuItemInfos=(TgMenuItemInfo*)malloc(
            (gnNumMainMenuItems+1)*sizeof(TgMenuItemInfo));

      for (i=0; i <= gnNumMainMenuItems; i++) {
         if (gpMainMenuItemInfos[i].cmdid != MENU_TANGRAM2) {
            memcpy(&pMainMenuItemInfos[j++], &gpMainMenuItemInfos[i],
                  sizeof(TgMenuItemInfo));
         }
      }
      memcpy(&pMainMenuItemInfos[j], &gpMainMenuItemInfos[i],
            sizeof(TgMenuItemInfo));

      gpMainMenuItemInfos = pMainMenuItemInfos;
      gnNumMainMenuItems--;
   }
}

static
void UpdateMenubarItemInfo(x, w, h, MenuX, MenuY, TextBBox)
   int x, w, h, *MenuX, *MenuY;
   struct BBRec *TextBBox;
{
   if (MenuX != NULL || MenuY != NULL) {
      int win_x, win_y, main_win_x, main_win_y;
      unsigned int win_w, win_h, win_brdr_w, win_d;
      Window root_win;

      ComputeMainWinXY(&main_win_x, &main_win_y);
      XGetGeometry(mainDisplay, menubarWindow, &root_win, &win_x,
            &win_y, &win_w, &win_h, &win_brdr_w, &win_d);
      if (MenuX != NULL) {
         *MenuX = main_win_x + win_x + x - 3 -
               ((menuFontSet==NULL && menuFontPtr==NULL) ? 0 :
               (menuFontWidth>>1));
         if (!threeDLook) {
            *MenuX += 4;
         }
      }
      if (MenuY != NULL) {
         *MenuY = main_win_y+brdrW+win_y+h+(win_brdr_w<<1)-1;
      }
   }
   if (TextBBox != NULL) {
      TextBBox->ltx = x-((menuFontSet==NULL && menuFontPtr==NULL) ? 0 :
            (menuFontWidth>>1));
      TextBBox->rbx = x+w+((menuFontSet==NULL && menuFontPtr==NULL) ? 0 :
            menuFontWidth);
      TextBBox->lty = h-((menuFontSet==NULL && menuFontPtr == NULL) ?
            initialMenubarWindowH : menuFontHeight);
      TextBBox->rby = h;
   }
}

static
int WhichMenubarItem(X, Y, MenuX, MenuY, TextBBox)
   int X, Y, *MenuX, *MenuY;
   struct BBRec *TextBBox;
{
   int i, x=0, w=0, h=0, len=0, gap=0, padding=(windowPadding>>1);
   int min_x=((menuFontSet==NULL && menuFontPtr==NULL) ? 2 : menuFontWidth);

   if (noMenubar ||
         Y < 0 || Y >= menubarWindowH || X < min_x || X >= menubarWindowW) {
      return INVALID;
   }
   if (menuFontSet != NULL || menuFontPtr != NULL) {
      x = menuFontWidth;
      h = menuFontHeight;
      gap = (x<<1);
      x += padding;
      h += padding;
      for (i=0; i < gnNumMenubarItems; i++) {
         len = strlen(_(gpMenubarItemInfos[i].menu_str));
         w = MenuTextWidth(menuFontPtr, _(gpMenubarItemInfos[i].menu_str), len);
         if ((!noMinWinSize || !gnMinimalMenubar || gnAutoWrapMenubar) &&
               x+w+padding >= menubarWindowW) {
            if (Y < h) return INVALID;
            x = menuFontWidth+padding;
            h += menuFontHeight+padding;
         }
         if (Y < h && X < x+w+gap+padding) {
            if (!colorDisplay && gpMenubarItemInfos[i].cmdid == MENU_COLOR) {
               return INVALID;
            }
            UpdateMenubarItemInfo(x+padding, w, h, MenuX, MenuY, TextBBox);
            return i;
         }
         x += w+gap+padding;
      }
   } else {
      x = 2;
      h = initialMenubarWindowH;
      gap = defaultFontWidth+(defaultFontWidth>>1);
      x += padding;
      h += padding;
      for (i=0; i < gnNumMenubarItems; i++) {
         len = strlen(_(gpMenubarItemInfos[i].menu_str));
         w = defaultFontWidth*len;
         if ((!noMinWinSize || !gnMinimalMenubar || gnAutoWrapMenubar) &&
               x+w+padding >= menubarWindowW) {
            if (Y < h) return INVALID;
            x = 2+padding;
            h += initialMenubarWindowH+padding;
         }
         if (Y < h && X < x+w+gap+padding) {
            if (!colorDisplay && gpMenubarItemInfos[i].cmdid == MENU_COLOR) {
               return INVALID;
            }
            UpdateMenubarItemInfo(x+padding, w, h, MenuX, MenuY, TextBBox);
            return i;
         }
         x += w+gap+padding;
      }
   }
   return INVALID;
}

static
int GetMenubarItemInfo(index, MenuX, MenuY, TextBBox)
   int index, *MenuX, *MenuY;
   struct BBRec *TextBBox;
{
   int i, x=0, w=0, h=0, len=0, gap=0, padding=(windowPadding>>1);

   if (menuFontSet != NULL || menuFontPtr != NULL) {
      x = menuFontWidth;
      h = menuFontHeight;
      gap = (x<<1);
      x += padding;
      h += padding;
      for (i=0; i < gnNumMenubarItems; i++) {
         len = strlen(_(gpMenubarItemInfos[i].menu_str));
         w = MenuTextWidth(menuFontPtr, _(gpMenubarItemInfos[i].menu_str), len);
         if ((!noMinWinSize || !gnMinimalMenubar || gnAutoWrapMenubar) &&
               x+w+padding >= menubarWindowW) {
            x = menuFontWidth+padding;
            h += menuFontHeight+padding;
         }
         if (i == index) {
            UpdateMenubarItemInfo(x+padding, w, h, MenuX, MenuY, TextBBox);
            return i;
         }
         x += w+gap+padding;
      }
   } else {
      x = 2;
      h = initialMenubarWindowH;
      gap = defaultFontWidth+(defaultFontWidth>>1);
      x += padding;
      h += padding;
      for (i=0; i < gnNumMenubarItems; i++) {
         len = strlen(_(gpMenubarItemInfos[i].menu_str));
         w = defaultFontWidth*len;
         if ((!noMinWinSize || !gnMinimalMenubar || gnAutoWrapMenubar) &&
               x+w+padding >= menubarWindowW) {
            x = 2+padding;
            h += initialMenubarWindowH+padding;
         }
         if (i == index) {
            UpdateMenubarItemInfo(x+padding, w, h, MenuX, MenuY, TextBBox);
            return i;
         }
         x += w+gap+padding;
      }
   }
   return INVALID;
}

void SendCommandToSelf(nCmdId, nIndex)
   int nCmdId, nIndex;
{
   XClientMessageEvent client_ev;

   memset(&client_ev, 0, sizeof(XClientMessageEvent));
   client_ev.type = ClientMessage;
   client_ev.window = mainWindow;
   client_ev.message_type = executeCmdByIDAtom;
   client_ev.format = 16;
   client_ev.data.s[0] = (short)TG_COMMAND;
   client_ev.data.s[1] = (short)(nCmdId & 0x0ffff);
   client_ev.data.s[2] = (short)(nIndex & 0x0ffff);
   XSendEvent(mainDisplay, mainWindow, False, NoEventMask, (XEvent*)&client_ev);
}

TgMenuItem *FindMenuItemByCmdId(menu, cmdid)
   TgMenu *menu;
   int cmdid;
{
   int i, num_items=menu->num_items;
   TgMenuItem *menuitems=menu->menuitems;

   for (i=0; i < num_items; i++) {
      TgMenuItem *menu_item=(&menuitems[i]);

      if ((menu_item->flags & TGMU_SEPARATOR) == 0) {
         if (menu_item->cmdid == cmdid) {
            return menu_item;
         }
      }
   }
   return NULL;
}

TgMenuItem *FindMenuItemByIndex(menu, index)
   TgMenu *menu;
   int index;
   /* skip TGMU_SEPARATOR */
{
   int i, num_items=menu->num_items;
   TgMenuItem *menuitems=menu->menuitems;

   for (i=0; i < num_items; i++) {
      TgMenuItem *menu_item=(&menuitems[i]);

      if ((menu_item->flags & TGMU_SEPARATOR) == 0) {
         if (i == index) {
            return menu_item;
         }
      }
   }
   return NULL;
}

TgMenuItem *FindMenuItemBySubMenuInfoPtr(menu, submenu_info)
   TgMenu *menu;
   TgMenuInfo *submenu_info;
{
   int i, num_items=menu->num_items;
   TgMenuItem *menuitems=menu->menuitems;

   for (i=0; i < num_items; i++) {
      TgMenuItem *menu_item=(&menuitems[i]);

      if ((menu_item->flags & TGMU_HAS_SUBMENU) == TGMU_HAS_SUBMENU) {
         TgMenuItemInfo *create_info=menu_item->submenu_create_info;

         if (create_info->submenu_info == submenu_info) {
            return menu_item;
         }
      }
   }
   return NULL;
}

static
int TgSetMenuItemCheckOrRadioById(menu, cmdid, checked, mask)
   TgMenu *menu;
   int cmdid, checked, mask;
{
   TgMenuItem stMenuItem;
   TgMenuItem *menu_item=NULL;

   if (menu == NULL) return FALSE;

   if ((menu_item=FindMenuItemByCmdId(menu, cmdid)) == NULL) {
      return FALSE;
   }
   memset(&stMenuItem, 0, sizeof(TgMenuItem));
   stMenuItem.checked = checked;
   if (!TgSetMenuItemInfo(menu_item, mask, &stMenuItem)) {
      return FALSE;
   }
   return TRUE;
}

int TgSetMenuItemCheckById(menu, cmdid, checked)
   TgMenu *menu;
   int cmdid, checked;
{
   return TgSetMenuItemCheckOrRadioById(menu, cmdid, checked, TGMU_MASK_CHECK);
}

int TgSetMenuItemRadioById(menu, cmdid, checked)
   TgMenu *menu;
   int cmdid, checked;
{
   return TgSetMenuItemCheckOrRadioById(menu, cmdid, checked, TGMU_MASK_RADIO);
}

int TgIsMenuItemChecked(menu, index)
   TgMenu *menu;
   int index;
{
   TgMenuItem *menuitems=menu->menuitems;

   if (index >= 0 && index < menu->num_items) {
      int flags=(menuitems[index].flags);

      return (menuitems[index].checked &&
            (flags & TGMU_HAS_CHECK) == TGMU_HAS_CHECK);
   }
   return FALSE;
}

int TgIsMenuItemRadio(menu, index)
   TgMenu *menu;
   int index;
{
   TgMenuItem *menuitems=menu->menuitems;

   if (index >= 0 && index < menu->num_items) {
      int flags=(menuitems[index].flags);

      return (menuitems[index].checked &&
            (flags & TGMU_HAS_RADIO) == TGMU_HAS_RADIO);
   }
   return FALSE;
}

int TgIsMenuItemEnabled(menu, index)
   TgMenu *menu;
   int index;
{
   TgMenuItem *menuitems=menu->menuitems;

   if (index >= 0 && index < menu->num_items) {
      int flags=(menuitems[index].flags);

      return ((flags & TGMU_DISABLED) != TGMU_DISABLED);
   }
   return FALSE;
}

int TgEnableMenuItemById(menu, cmdid, enabled)
   TgMenu *menu;
   int cmdid, enabled;
{
   TgMenuItem *menu_item=NULL;

   if (menu == NULL) return FALSE;

   if ((menu_item=FindMenuItemByCmdId(menu, cmdid)) == NULL) {
      return FALSE;
   }
   if (enabled) {
      menu_item->flags &= (~TGMU_DISABLED);
   } else {
      menu_item->flags |= TGMU_DISABLED;
   }
   return TRUE;
}

int TgEnableMenuItemByIndex(menu, index, enabled)
   TgMenu *menu;
   int index, enabled;
{
   TgMenuItem *menu_item=NULL;

   if (menu == NULL) return FALSE;

   if ((menu_item=FindMenuItemByIndex(menu, index)) == NULL) {
      return FALSE;
   }
   if (enabled) {
      menu_item->flags &= (~TGMU_DISABLED);
   } else {
      menu_item->flags |= TGMU_DISABLED;
   }
   return TRUE;
}

int TgEnableMenuItemBySubMenuInfoPtr(menu, submenu_info, enabled)
   TgMenu *menu;
   TgMenuInfo *submenu_info;
   int enabled;
{
   TgMenuItem *menu_item=NULL;

   if (menu == NULL) return FALSE;

   if ((menu_item=FindMenuItemBySubMenuInfoPtr(menu, submenu_info)) == NULL) {
      return FALSE;
   }
   if (enabled) {
      menu_item->flags &= (~TGMU_DISABLED);
   } else {
      menu_item->flags |= TGMU_DISABLED;
   }
   return TRUE;
}

void TgClearThreeDButton(dpy, win, gc, bbox, linewidth)
   Display *dpy;
   Window win;
   GC gc;
   struct BBRec *bbox;
   int linewidth;
{
   int x=bbox->ltx, y=bbox->lty, w=bbox->rbx-bbox->ltx, h=bbox->rby-bbox->lty;

   XSetForeground(dpy, gc, myLtGryPixel);
   if (linewidth == 1) {
      XDrawLine(dpy, win, gc, x, y+h-1, x+w-1, y+h-1);
      XDrawLine(dpy, win, gc, x+w-1, y+h-1, x+w-1, y);
      XDrawLine(dpy, win, gc, x, y+h-2, x, y);
      XDrawLine(dpy, win, gc, x, y, x+w-2, y);
   } else if (linewidth >= 2) {
      XDrawLine(dpy, win, gc, x, y+h-1, x+w-1, y+h-1);
      XDrawLine(dpy, win, gc, x+w-1, y+h-1, x+w-1, y);
      XDrawLine(dpy, win, gc, x+1, y+h-2, x+w-2, y+h-2);
      XDrawLine(dpy, win, gc, x+w-2, y+h-2, x+w-2, y+1);
      XDrawLine(dpy, win, gc, x+1, y+h-3, x+1, y+1);
      XDrawLine(dpy, win, gc, x+1, y+1, x+w-3, y+1);
      XDrawLine(dpy, win, gc, x, y+h-2, x, y);
      XDrawLine(dpy, win, gc, x, y, x+w-2, y);
   }
}

void TgDrawThreeDButton(dpy, win, gc, bbox, state, linewidth, button)
   Display *dpy;
   Window win;
   GC gc;
   struct BBRec *bbox;
   int state, linewidth, button; /* button is TRUE if the item is a button */
{
   int x=bbox->ltx, y=bbox->lty, w=bbox->rbx-bbox->ltx, h=bbox->rby-bbox->lty;
   int pixels[4];

   memset(pixels, 0, 4*sizeof(int));
   switch (state) {
   case TGBS_NORMAL: return;
   case TGBS_RAISED:
      if (linewidth == 1) {
         pixels[0] = myDkGryPixel; /* inner lower right */
         pixels[1] = myBlackPixel; /* outer lower right */
         pixels[2] = myLtGryPixel; /* inner upper left */
         pixels[3] = myWhitePixel; /* outer upper left */
      } else {
         pixels[0] = myBlackPixel; /* outer lower right */
         pixels[1] = myDkGryPixel; /* inner lower right */
         if (button) {
            pixels[2] = myLtGryPixel; /* inner upper left */
            pixels[3] = myWhitePixel; /* outer upper left */
         } else {
            pixels[2] = myWhitePixel; /* inner upper left */
            pixels[3] = myLtGryPixel; /* outer upper left */
         }
      }
      break;
   case TGBS_LOWRED:
      if (linewidth == 1) {
         if (button) {
            pixels[0] = myDkGryPixel; /* outer lower right */
            pixels[1] = myBlackPixel; /* inner lower right */ /* useless here */
            pixels[2] = myWhitePixel; /* inner upper left */ /* useless here */
            pixels[3] = myLtGryPixel; /* outer upper left */
         } else {
            pixels[0] = myWhitePixel; /* outer lower right */
            pixels[1] = myLtGryPixel; /* inner lower right */ /* useless here */
            pixels[2] = myBlackPixel; /* inner upper left */ /* useless here */
            pixels[3] = myDkGryPixel; /* outer upper left */
         }
      } else {
         if (button) {
            pixels[0] = myLtGryPixel; /* outer lower right */
            pixels[1] = myWhitePixel; /* inner lower right */
            pixels[2] = myDkGryPixel; /* inner upper left */
            pixels[3] = myBlackPixel; /* outer upper left */
         } else {
            pixels[0] = myWhitePixel; /* outer lower right */
            pixels[1] = myLtGryPixel; /* inner lower right */
            pixels[2] = myBlackPixel; /* inner upper left */
            pixels[3] = myDkGryPixel; /* outer upper left */
         }
      }
      break;
   case TGBS_GRAYED: return;
   }
   if (linewidth == 1) {
      XSetForeground(dpy, gc, pixels[0]);
      XDrawLine(dpy, win, gc, x, y+h-1, x+w-1, y+h-1);
      XDrawLine(dpy, win, gc, x+w-1, y+h-1, x+w-1, y);
      XSetForeground(dpy, gc, pixels[3]);
      XDrawLine(dpy, win, gc, x, y+h-2, x, y);
      XDrawLine(dpy, win, gc, x, y, x+w-2, y);
   } else if (linewidth >= 2) {
      XSetForeground(dpy, gc, pixels[0]);
      XDrawLine(dpy, win, gc, x, y+h-1, x+w-1, y+h-1);
      XDrawLine(dpy, win, gc, x+w-1, y+h-1, x+w-1, y);
      XSetForeground(dpy, gc, pixels[1]);
      XDrawLine(dpy, win, gc, x+1, y+h-2, x+w-2, y+h-2);
      XDrawLine(dpy, win, gc, x+w-2, y+h-2, x+w-2, y+1);
      XSetForeground(dpy, gc, pixels[2]);
      XDrawLine(dpy, win, gc, x+1, y+h-3, x+1, y+1);
      XDrawLine(dpy, win, gc, x+1, y+1, x+w-3, y+1);
      XSetForeground(dpy, gc, pixels[3]);
      XDrawLine(dpy, win, gc, x, y+h-2, x, y);
      XDrawLine(dpy, win, gc, x, y, x+w-2, y);
   }
}

void TgDrawStipple(dpy, win, gc, stipple, x, y, w, h, fg_pixel, bg_pixel,
      fill_style)
   Display *dpy;
   Window win;
   GC gc;
   Pixmap stipple;
   int x, y, w, h, fg_pixel, bg_pixel, fill_style;
{
   XGCValues values;

   values.foreground = fg_pixel;
   values.background = bg_pixel;
   values.fill_style = fill_style;
   values.stipple = stipple;
   values.ts_x_origin = x;
   values.ts_y_origin = y;
   XChangeGC(dpy, gc,
         GCForeground | GCBackground | GCFillStyle | GCStipple |
         GCTileStipXOrigin | GCTileStipYOrigin, &values);
   XFillRectangle(dpy, win, gc, values.ts_x_origin, values.ts_y_origin, w, h);
   values.fill_style = FillSolid;
   values.ts_x_origin = 0;
   values.ts_y_origin = 0;
   XChangeGC(dpy, gc,
         GCFillStyle | GCTileStipXOrigin | GCTileStipYOrigin, &values);
}

void TgDraw2DGrayedPixmap(dpy, win, bitmap, x, y, w, h, fg_pixel, bg_pixel)
   Display *dpy;
   Window win;
   Pixmap bitmap;
   int x, y, w, h, fg_pixel, bg_pixel;
{
   XGCValues values;
   GC gc=NULL;

   if (threeDLook) {
      /* not meant for threeDLook */
      return;
   }
   values.foreground = fg_pixel;
   values.background = bg_pixel;
   values.function = GXcopy;
   values.fill_style = FillStippled;
   values.stipple = patPixmap[SCROLLPAT];
   values.ts_x_origin = x;
   values.ts_y_origin = y;
   values.clip_mask = bitmap;
   values.clip_x_origin = x;
   values.clip_y_origin = y;
   gc = XCreateGC(dpy, win,
         GCForeground | GCBackground | GCFunction | GCFillStyle |
         GCStipple | GCTileStipXOrigin | GCTileStipYOrigin |
         GCClipMask | GCClipXOrigin | GCClipYOrigin, &values);
   if (gc != NULL) {
      XFillRectangle(dpy, win, gc, x, y, w, h);
      XFreeGC(mainDisplay, gc);
   }
}

static struct BBRec gstMenubarWinBBox;

static
void SetMenubarWinBBox()
{
   unsigned int win_w, win_h, win_brdr_w, win_d;
   int win_x, win_y, main_win_x, main_win_y;
   Window root_win=None;

   ComputeMainWinXY(&main_win_x, &main_win_y);
   XGetGeometry(mainDisplay, menubarWindow, &root_win, &win_x, &win_y,
         &win_w, &win_h, &win_brdr_w, &win_d);
   gstMenubarWinBBox.ltx = main_win_x + win_x;
   gstMenubarWinBBox.lty = main_win_y + win_y;
   gstMenubarWinBBox.rbx = gstMenubarWinBBox.ltx + win_w;
   gstMenubarWinBBox.rby = gstMenubarWinBBox.lty + win_h;
}

static
void SetParentMenuWinBBox(menu)
   TgMenu *menu;
{
   memcpy(&gstMenubarWinBBox, &menu->parent_menu->bbox, sizeof(struct BBRec));
}

static
void AdjMenuGeometry(menu, pn_orig_x, pn_orig_y, pn_min_pin, pn_max_pin)
   TgMenu *menu;
   int *pn_orig_x, *pn_orig_y, *pn_min_pin, *pn_max_pin;
{
   int dsp_w=DisplayWidth(mainDisplay, mainScreen);
   int dsp_h=DisplayHeight(mainDisplay, mainScreen);
   int orig_x=menu->bbox.ltx, orig_y=menu->bbox.lty;
   int menu_w=menu->bbox.rbx-orig_x+((menu->padding)<<1);
   int menu_h=menu->bbox.rby-orig_y+((menu->padding)<<1);
   int min_pin=0, max_pin=0, dx=0, dy=0;
   TgMenu *parent_menu=menu->parent_menu;

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
   if (orig_x+menu_w >= dsp_w-1-((menu->brdr_w)<<1)) {
      orig_x = dsp_w - 1 - menu_w + ((menu->padding)<<1);
      if (parent_menu != NULL) {
         orig_x = parent_menu->bbox.ltx - 1 - menu_w + ((menu->padding)<<1);
      }
   }
   if (orig_y+menu_h >= dsp_h-1-((menu->brdr_w)<<1)) {
      orig_y = dsp_h - 1 - menu_h + ((menu->padding)<<1);
   }
   if (parent_menu != NULL) {
      if (orig_x < parent_menu->bbox.ltx-((menu->brdr_w)<<1)) {
         min_pin = orig_x - mainMenuPinDistance;
      } else {
         min_pin = parent_menu->bbox.ltx - ((menu->brdr_w)<<1) -
               mainMenuPinDistance;
      }
      if (orig_x+menu_w+((menu->brdr_w)<<2) >=
            parent_menu->bbox.rbx+((menu->brdr_w)<<1)) {
         max_pin = orig_x + menu_w + ((menu->brdr_w)<<2) + mainMenuPinDistance;
      } else {
         max_pin = parent_menu->bbox.rbx + ((menu->brdr_w)<<1) +
               mainMenuPinDistance;
      }
   } else {
      min_pin = orig_x - mainMenuPinDistance;
      max_pin = orig_x + menu_w + ((menu->brdr_w)<<2) + mainMenuPinDistance;
   }
   dx = orig_x - menu->bbox.ltx;
   dy = orig_y - menu->bbox.lty;
   menu->bbox.ltx += dx; menu->bbox.lty += dy;
   menu->bbox.rbx += dx; menu->bbox.rby += dy;
   *pn_orig_x = orig_x;
   *pn_orig_y = orig_y;
   *pn_min_pin = min_pin;
   *pn_max_pin = max_pin;
}

static
void TgDrawTextMenuItem(menu, menu_item)
   TgMenu *menu;
   TgMenuItem *menu_item;
{
   int x=0, baseline=0, y_offset=0;
   int item_w=(menu_item->bbox.rbx-menu_item->bbox.ltx);
   int item_h=(menu_item->bbox.rby-menu_item->bbox.lty);
   int flags=menu_item->flags; /* TGMU_* */
   int state=menu_item->state; /* TGBS* */
   int checked=menu_item->checked;
   int multicolor=((flags & TGMU_MULTICOLOR)==TGMU_MULTICOLOR);
   int bg_pixel=(threeDLook ? myLtGryPixel : myBgPixel);
   int fg_pixels[4], bg_pixels[4];
   XGCValues values;

   memset(fg_pixels, 0, 4*sizeof(int));
   memset(bg_pixels, 0, 4*sizeof(int));
   if (menu->scroll_start > 0 && menu->first_index > 0) {
      y_offset = (menu->first_index *
            (((menuFontSet==NULL && menuFontPtr == NULL) ?
            initialMenubarWindowH : menuFontHeight) + 1));
   }
   if (multicolor) {
      fg_pixels[TGBS_NORMAL] = menu_item->multicolor_pixel;
      bg_pixels[TGBS_NORMAL] = bg_pixel;
      fg_pixels[TGBS_GRAYED] = myDkGryPixel;
      bg_pixels[TGBS_GRAYED] = bg_pixel;
      /* use reversed video */
      fg_pixels[TGBS_RAISED] = myBgPixel;
      bg_pixels[TGBS_RAISED] = menu_item->multicolor_pixel;
   } else {
      fg_pixels[TGBS_NORMAL] = myFgPixel;
      bg_pixels[TGBS_NORMAL] = bg_pixel;
      fg_pixels[TGBS_GRAYED] = myDkGryPixel;
      bg_pixels[TGBS_GRAYED] = bg_pixel;
      /* use reversed video */
      fg_pixels[TGBS_RAISED] = myBgPixel;
      bg_pixels[TGBS_RAISED] = myFgPixel;
   }
   if (flags & TGMU_DISABLED) state = TGBS_GRAYED;

   switch (state) {
   case TGBS_NORMAL: /* not selected */
   case TGBS_GRAYED: /* disabled */
   case TGBS_RAISED: /* selected */
      break;
   case TGBS_LOWRED: /* not valid for a text menu item state */ return;
   }
   XSetForeground(mainDisplay, textMenuGC, bg_pixels[state]);
   XFillRectangle(mainDisplay, menu->window, textMenuGC,
         menu_item->bbox.ltx, menu_item->bbox.lty-y_offset,
         item_w+WINDOW_PADDING-windowPadding, item_h+(threeDLook?0:1));
   if ((flags & TGMU_SEPARATOR) == TGMU_SEPARATOR) {
      if (threeDLook) {
         XSetForeground(mainDisplay, textMenuGC, myDkGryPixel);
         XDrawLine(mainDisplay, menu->window, textMenuGC,
               menu_item->bbox.ltx, menu_item->bbox.lty-y_offset,
               item_w, menu_item->bbox.lty-y_offset);
         XSetForeground(mainDisplay, textMenuGC, myWhitePixel);
         XDrawLine(mainDisplay, menu->window, textMenuGC,
               menu_item->bbox.ltx, menu_item->bbox.lty+1-y_offset,
               item_w, menu_item->bbox.lty+1-y_offset);
      } else {
         XSetForeground(mainDisplay, textMenuGC, myFgPixel);
         XDrawLine(mainDisplay, menu->window, textMenuGC,
               menu_item->bbox.ltx, menu_item->bbox.lty+1-y_offset,
               item_w+1, menu_item->bbox.lty+1-y_offset);
      }
      return;
   }
   baseline = menu_item->bbox.lty - y_offset +
         ((menuFontSet==NULL && menuFontPtr==NULL) ?
         defaultFontAsc : menuFontAsc);
   x = menu->check_start;
   if ((flags & (TGMU_HAS_CHECK|TGMU_HAS_RADIO)) != 0 && checked) {
      int dy=((item_h-check_height)>>1);

      values.foreground = fg_pixels[state];
      values.background = bg_pixels[state];
      values.fill_style = FillStippled;
      values.stipple = None;
      if ((flags & TGMU_HAS_CHECK) == TGMU_HAS_CHECK) {
         values.stipple = checkBitmap;
      } else if ((flags & TGMU_HAS_RADIO) == TGMU_HAS_RADIO) {
         values.stipple = radioBitmap;
      }
      if (values.stipple != None) {
         values.ts_x_origin = x;
         values.ts_y_origin = menu_item->bbox.lty+dy-y_offset;
         XChangeGC(mainDisplay, textMenuGC,
               GCForeground | GCBackground | GCFillStyle | GCStipple |
               GCTileStipXOrigin | GCTileStipYOrigin, &values);
         XFillRectangle(mainDisplay, menu->window, textMenuGC,
               values.ts_x_origin, values.ts_y_origin,
               check_width, check_height);
         values.fill_style = FillSolid;
         values.ts_x_origin = 0;
         values.ts_y_origin = 0;
         XChangeGC(mainDisplay, textMenuGC,
               GCFillStyle | GCTileStipXOrigin | GCTileStipYOrigin, &values);
      }
   }
   x = menu->str_start;
   if (threeDLook) {
      if (state == TGBS_GRAYED) {
         XSetForeground(mainDisplay, textMenuGC, myWhitePixel);
         DrawMenuString(mainDisplay, menu->window, textMenuGC, x+1,
               baseline+1, _(menu_item->menu_str),
               strlen(_(menu_item->menu_str)));
      }
      XSetForeground(mainDisplay, textMenuGC, fg_pixels[state]);
      DrawMenuString(mainDisplay, menu->window, textMenuGC, x,
            baseline, _(menu_item->menu_str), strlen(_(menu_item->menu_str)));
   } else {
      if (state == TGBS_GRAYED) {
         DrawMenuString(mainDisplay, menu->window, revGrayGC, x, baseline,
               _(menu_item->menu_str), strlen(_(menu_item->menu_str)));
      } else {
         XSetForeground(mainDisplay, textMenuGC, fg_pixels[state]);
         DrawMenuString(mainDisplay, menu->window, textMenuGC, x, baseline,
               _(menu_item->menu_str), strlen(_(menu_item->menu_str)));
      }
   }
   if ((flags & TGMU_HAS_SHORTCUT) == TGMU_HAS_SHORTCUT) {
      x = menu->shortcut_start;
      if (state == TGBS_GRAYED) {
         XSetForeground(mainDisplay, textMenuGC, myWhitePixel);
         DrawMenuString(mainDisplay, menu->window, textMenuGC, x+1,
               baseline+1, menu_item->detail.shortcut_str,
               strlen(menu_item->detail.shortcut_str));
      }
      XSetForeground(mainDisplay, textMenuGC, fg_pixels[state]);
      DrawMenuString(mainDisplay, menu->window, textMenuGC, x, baseline,
            menu_item->detail.shortcut_str,
            strlen(menu_item->detail.shortcut_str));
   }
   if ((flags & TGMU_HAS_SUBMENU) == TGMU_HAS_SUBMENU) {
      int dy=((item_h-check_height)>>1);

      x = menu->arrow_start;

      values.foreground = fg_pixels[state];
      values.background = bg_pixels[state];
      values.fill_style = FillStippled;
      if (!threeDLook && state == TGBS_GRAYED) {
         values.stipple = graySubmenuBitmap;
      } else {
         values.stipple = submenuBitmap;
      }
      values.ts_x_origin = x;
      values.ts_y_origin = menu_item->bbox.lty+dy-y_offset;
      XChangeGC(mainDisplay, textMenuGC,
            GCForeground | GCBackground | GCFillStyle | GCStipple |
            GCTileStipXOrigin | GCTileStipYOrigin, &values);
      XFillRectangle(mainDisplay, menu->window, textMenuGC,
            values.ts_x_origin, values.ts_y_origin,
            check_width, check_height);
      values.fill_style = FillSolid;
      values.ts_x_origin = 0;
      values.ts_y_origin = 0;
      XChangeGC(mainDisplay, textMenuGC,
            GCFillStyle | GCTileStipXOrigin | GCTileStipYOrigin, &values);
   }
}

static
void TgDrawBitmapOrColorMenuItem(menu, menu_item)
   TgMenu *menu;
   TgMenuItem *menu_item;
{
   int item_x=menu_item->bbox.ltx, item_y=menu_item->bbox.lty, x_offset=0;
   int item_w=menu_item->bbox.rbx-item_x, item_h=menu_item->bbox.rby-item_y;
   int flags=menu_item->flags; /* TGMU_* */
   int state=menu_item->state; /* TGBS* */
   int checked=menu_item->checked, color_menu=(menu->type==TGMUTYPE_COLOR);
   int bg_pixel=(threeDLook ? myLtGryPixel : myBgPixel), color_pixel=INVALID;
   Pixmap bitmap=None;

   if (menu->scroll_start > 0 && menu->first_index > 0) {
      x_offset = ((menu->image_w+(windowPadding<<1)) * menu->first_index);
   }
   if (color_menu) {
      color_pixel = (*(int*)(menu_item->menu_str));
   } else {
      bitmap = (*(Pixmap*)(menu_item->menu_str));
   }
   if (state == TGBS_NORMAL && checked) state = TGBS_LOWRED;
   if (flags & TGMU_DISABLED) state = TGBS_GRAYED;

   switch (state) {
   case TGBS_NORMAL: /* not selected */
   case TGBS_RAISED: /* selected, i.e., mouse overed */
   case TGBS_LOWRED: /* checked but not selected */
   case TGBS_GRAYED: /* not valid for a bitmap menu item state */
      break;
   }
   XSetForeground(mainDisplay, textMenuGC, bg_pixel);
   XFillRectangle(mainDisplay, menu->window, textMenuGC,
         menu_item->bbox.ltx-x_offset, menu_item->bbox.lty, item_w, item_h);
   if ((flags & TGMU_SEPARATOR) == TGMU_SEPARATOR) {
      return;
   }
   if (threeDLook) {
      if (x_offset != 0) {
         OffsetBBox(&menu_item->bbox, -x_offset, 0, &menu_item->bbox);
         TgDrawThreeDButton(mainDisplay, menu->window, textMenuGC,
               &menu_item->bbox, state, 2, TRUE);
         OffsetBBox(&menu_item->bbox, +x_offset, 0, &menu_item->bbox);
      } else {
         TgDrawThreeDButton(mainDisplay, menu->window, textMenuGC,
               &menu_item->bbox, state, 2, TRUE);
      }
      if (color_menu) {
         TgDrawStipple(mainDisplay, menu->window, textMenuGC,
               patPixmap[SOLIDPAT], item_x+menu->padding-x_offset,
               item_y+menu->padding, menu->image_w, menu->image_h, color_pixel,
               bg_pixel, FillSolid);
      } else {
         if (state == TGBS_GRAYED) {
            TgDrawStipple(mainDisplay, menu->window, textMenuGC, bitmap,
                  item_x+menu->padding-x_offset+1, item_y+menu->padding+1,
                  menu->image_w, menu->image_h, myWhitePixel, bg_pixel,
                  FillStippled);
            TgDrawStipple(mainDisplay, menu->window, textMenuGC, bitmap,
                  item_x+menu->padding-x_offset, item_y+menu->padding,
                  menu->image_w, menu->image_h, myDkGryPixel, bg_pixel,
                  FillStippled);
         } else {
            TgDrawStipple(mainDisplay, menu->window, textMenuGC, bitmap,
                  item_x+menu->padding-x_offset, item_y+menu->padding,
                  menu->image_w, menu->image_h, myFgPixel, bg_pixel,
                  FillStippled);
         }
      }
   } else if (color_menu) {
      XSetForeground(mainDisplay, textMenuGC, color_pixel);
      XFillRectangle(mainDisplay, menu->window, textMenuGC,
            item_x+menu->padding-x_offset, item_y+menu->padding,
            menu->image_w, menu->image_h);
   } else {
      if (state == TGBS_NORMAL) {
         XSetStipple(mainDisplay, rasterGC, bitmap);
         XFillRectangle(mainDisplay, menu->window, rasterGC,
               item_x+menu->padding-x_offset, item_y+menu->padding,
               menu->image_w, menu->image_h);
      } else if (state == TGBS_GRAYED) {
         TgDraw2DGrayedPixmap(mainDisplay, menu->window, bitmap,
               item_x+menu->padding-x_offset, item_y+menu->padding,
               menu->image_w, menu->image_h, myFgPixel, myBgPixel);
      } else {
         XSetForeground(mainDisplay, textMenuGC, myFgPixel);
         XFillRectangle(mainDisplay, menu->window, textMenuGC,
               item_x+menu->padding-x_offset, item_y+menu->padding,
               menu->image_w, menu->image_h);
         XSetStipple(mainDisplay, rvPixmapMenuGC, bitmap);
         XFillRectangle(mainDisplay, menu->window, rvPixmapMenuGC,
               item_x+menu->padding-x_offset, item_y+menu->padding,
               menu->image_w, menu->image_h);
      }
   }
}

void TgDrawMenuItem(menu, menu_item)
   TgMenu *menu;
   TgMenuItem *menu_item;
{
   switch (menu->type) {
   case TGMUTYPE_TEXT:
      TgDrawTextMenuItem(menu, menu_item);
      break;
   case TGMUTYPE_COLOR:
   case TGMUTYPE_BITMAP:
      TgDrawBitmapOrColorMenuItem(menu, menu_item);
      break;
   }
}

void TgDrawEntireMenu(menu)
   TgMenu *menu;
{
   int i, num_items=menu->num_items;
   TgMenuItem *menuitems=menu->menuitems;

   XClearWindow(mainDisplay, menu->window);
   if (menu->scroll_start > 0) {
      int min_index=0, max_index=0;

      if (menu->type == TGMUTYPE_COLOR || menu->type == TGMUTYPE_BITMAP) {
         min_index = menu->first_index*menu->num_rows;
         max_index = (menu->first_index+menuColsBeforeScroll)*menu->num_rows;
      }
      for (i=0; i < num_items; i++) {
         switch (menu->type) {
         case TGMUTYPE_TEXT:
            if (i >= menu->first_index &&
                  i < menu->first_index+menuRowsBeforeScroll) {
               TgDrawMenuItem(menu, &menuitems[i]);
            }
            break;
         case TGMUTYPE_COLOR:
         case TGMUTYPE_BITMAP:
            if (i >= min_index && i < max_index) {
               TgDrawMenuItem(menu, &menuitems[i]);
            }
            break;
         }
      }
   } else {
      for (i=0; i < num_items; i++) {
         TgDrawMenuItem(menu, &menuitems[i]);
      }
   }
   if (threeDLook) {
      struct BBRec bbox;

      bbox.ltx = 0;
      bbox.lty = 0;
      if (menu->scroll_start > 0) {
         double start_frac=(double)0;

         switch (menu->type) {
         case TGMUTYPE_TEXT:
            start_frac = (double)((double)(menu->first_index) /
                  (double)(num_items));
            bbox.rbx = menu->scroll_start+(windowPadding<<1);
            bbox.rby = maxScrollableMenuHeight;
            TgDrawThreeDButton(mainDisplay, menu->window, textMenuGC, &bbox,
                  TGBS_RAISED, 2, FALSE);
            TgDrawScrollBar(mainDisplay, menu->window, VERT_SCROLLBAR,
                  bbox.rbx+(windowPadding>>1), (windowPadding>>1), scrollBarW,
                  maxScrollableMenuHeight-windowPadding, start_frac,
                  menuRowsBeforeScroll, num_items);
            bbox.ltx = bbox.rbx;
            bbox.lty = 0;
            bbox.rbx = bbox.ltx+scrollBarW+windowPadding;
            bbox.rby = maxScrollableMenuHeight;
            TgDrawThreeDButton(mainDisplay, menu->window, textMenuGC, &bbox,
                  TGBS_RAISED, 1, FALSE);
            break;
         case TGMUTYPE_COLOR:
         case TGMUTYPE_BITMAP:
            start_frac = (double)((double)(menu->first_index) /
                  (double)(menu->num_cols));
            bbox.rbx = maxScrollableMenuWidth;
            bbox.rby = menu->scroll_start+(windowPadding<<1);
            TgDrawThreeDButton(mainDisplay, menu->window, textMenuGC, &bbox,
                  TGBS_RAISED, 2, FALSE);
            TgDrawScrollBar(mainDisplay, menu->window, HORI_SCROLLBAR,
                  (windowPadding>>1), bbox.rby+(windowPadding>>1),
                  maxScrollableMenuWidth-windowPadding, scrollBarW, start_frac,
                  menuColsBeforeScroll, menu->num_cols);
            bbox.ltx = 0;
            bbox.lty = bbox.rby;
            bbox.rbx = maxScrollableMenuWidth;
            bbox.rby = bbox.lty+scrollBarW+windowPadding;
            TgDrawThreeDButton(mainDisplay, menu->window, textMenuGC, &bbox,
                  TGBS_LOWRED, 1, TRUE);
            break;
         }
      } else {
         bbox.rbx = menu->bbox.rbx-menu->bbox.ltx+(windowPadding<<1);
         bbox.rby = menu->bbox.rby-menu->bbox.lty+(windowPadding<<1);
         TgDrawThreeDButton(mainDisplay, menu->window, textMenuGC, &bbox,
               TGBS_RAISED, 2, FALSE);
      }
   } else if (menu->scroll_start > 0) {
      double start_frac=(double)0;

      switch (menu->type) {
      case TGMUTYPE_TEXT:
         start_frac = (double)((double)(menu->first_index)/(double)(num_items));
         TgDrawScrollBar(mainDisplay, menu->window, VERT_SCROLLBAR,
               menu->scroll_start, 0, scrollBarW, maxScrollableMenuHeight,
               start_frac, menuRowsBeforeScroll, num_items);
         MyBox(menu->window, textMenuGC, menu->scroll_start, 0,
               menu->scroll_start+scrollBarW, maxScrollableMenuHeight);
         break;
      case TGMUTYPE_COLOR:
      case TGMUTYPE_BITMAP:
         start_frac = (double)((double)(menu->first_index) /
               (double)(menu->num_cols));
         TgDrawScrollBar(mainDisplay, menu->window, HORI_SCROLLBAR,
               0, menu->scroll_start, maxScrollableMenuWidth, scrollBarW,
               start_frac, menuColsBeforeScroll, menu->num_cols);
         MyBox(menu->window, textMenuGC, 0, menu->scroll_start,
               maxScrollableMenuWidth, menu->scroll_start+scrollBarW);
         break;
      }
   }
}

int TgWhichMenuIndex(menu, x, y, must_be_in_menu)
   TgMenu *menu;
   int x, y, must_be_in_menu;
   /*
    * Returns INVALID if clicked outside of the menu.
    * Returns -2 if clicked on a separator.
    * Returns -3 if clicked on scrollbar.
    * Otherwise, returned a menu index.
    */
{
   int i=0, num_items=menu->num_items;
   int first_index=0, max_index_plus_1=num_items, x_offset=0, y_offset=0;

   if (must_be_in_menu) {
      if (!(x >= 0 && x < menu->bbox.rbx-menu->bbox.ltx &&
            y >= 0 && y < menu->bbox.rby-menu->bbox.lty)) {
         return INVALID;
      }
   }
   switch (menu->type) {
   case TGMUTYPE_TEXT:
      if (menu->scroll_start > 0) {
         if (x >= menu->scroll_start+windowPadding) {
            return (-3);
         }
         first_index = menu->first_index;
         max_index_plus_1 = menu->first_index+menuRowsBeforeScroll;
         if (first_index > 0) {
            y_offset = (first_index *
                  (((menuFontSet==NULL && menuFontPtr == NULL) ?
                  initialMenubarWindowH : menuFontHeight) + 1));
         }
      }
      for (i=first_index; i < max_index_plus_1; i++) {
         TgMenuItem *menu_item=(&menu->menuitems[i]);

         if (y >= menu_item->bbox.lty-y_offset &&
               y < menu_item->bbox.rby+1-y_offset) {
            if (menu_item->flags & TGMU_SEPARATOR) {
               return (-2);
            }
            return i;
         }
      }
      break;
   case TGMUTYPE_COLOR:
   case TGMUTYPE_BITMAP:
      if (menu->scroll_start > 0) {
         if (y >= menu->scroll_start+windowPadding) {
            return (-3);
         }
         first_index = (menu->first_index*menu->num_rows);
         max_index_plus_1 =
               (menu->first_index+menuColsBeforeScroll)*menu->num_rows;
         if (first_index > 0) {
            x_offset = ((menu->image_w+(windowPadding<<1)) * menu->first_index);
         }
      }
      for (i=first_index; i < num_items; i++) {
         TgMenuItem *menu_item=(&menu->menuitems[i]);

         if ((x >= menu_item->bbox.ltx-x_offset &&
               x < menu_item->bbox.rbx+1-x_offset) &&
               (y >= menu_item->bbox.lty && y < menu_item->bbox.rby+1)) {
            if (menu_item->flags & TGMU_SEPARATOR) {
               return (-2);
            }
            return i;
         }
         if (i+1 == max_index_plus_1) break;
      }
      break;
   }
   return INVALID;
}

static int gnUncheckWhenMoused=FALSE;

static
int TgPointInAnAncestorMenu(menu, root_x, root_y)
   TgMenu *menu;
   int root_x, root_y;
{
   if (menu == NULL) return FALSE;

   if (PointInBBox(root_x, root_y, menu->bbox)) return TRUE;

   return TgPointInAnAncestorMenu(menu->parent_menu, root_x, root_y);
}

static
int TgPointInParentMenuSelectedItem(parent_menu)
   TgMenu *parent_menu;
{
   Window root_win=None, child_win=None;
   unsigned int status;
   int parent_root_x, parent_root_y, x=0, y=0;

   XQueryPointer(mainDisplay, parent_menu->window, &root_win,
         &child_win, &parent_root_x, &parent_root_y, &x, &y, &status);
   return (TgWhichMenuIndex(parent_menu, x, y, FALSE) ==
         parent_menu->selected_index);
}

TgMenu *TgCreatePopUpSubMenu(menu, selected_index)
   TgMenu *menu;
   int selected_index;
{
   TgMenu *submenu=NULL;
   TgMenuItem *menuitems=menu->menuitems;
   TgMenuItemInfo *create_info=menuitems[selected_index].submenu_create_info;

   if (create_info == NULL) return NULL;

   if (create_info->submenu_info->create_proc == TgCreateMenuFromMenuInfo) {
      submenu = menuitems[selected_index].detail.submenu =
            (create_info->submenu_info->create_proc)(menu, 0, 0,
            create_info->submenu_info, FALSE);
   } else {
      /*
       * if the creation procedure is not the default TgCreateMenuFromMenuInfo,
       *       the status_str_xlated will be ignored
       */
      submenu = menuitems[selected_index].detail.submenu =
            (create_info->submenu_info->create_proc)(menu, 0, 0,
            create_info->submenu_info, INVALID);
   }
   return submenu;
}

int TgPopUpSubMenu(menu, win_x, win_y)
   TgMenu *menu;
   int win_x, win_y;
{
   int rc=INVALID, new_selected=menu->selected_index;
   TgMenuItem *menuitems=menu->menuitems;
   TgMenu *submenu=menuitems[new_selected].detail.submenu;
   int menu_w=menu->bbox.rbx-menu->bbox.ltx+((menu->padding)<<1);
   int i, x=0, y=0, dx=0, dy=0;
   int saved_active_menu=INVALID, need_to_restore_active_menu=FALSE;

   submenu->parent_menu = menu;
   submenu->disallow_pinning = ((menuitems[new_selected].flags &
         TGMU_SUBMENU_PINNABLE) == 0);
   if ((menuitems[new_selected].flags &
         TGMU_SUBMENU_PINNABLE) == TGMU_SUBMENU_PINNABLE &&
         menuitems[new_selected].cmdid != INVALID) {
      saved_active_menu = activeMenu;
      activeMenu = menuitems[new_selected].cmdid;
      need_to_restore_active_menu = TRUE;
   }
   x = menu_w+win_x-((menu->padding)<<1);
   y = menuitems[new_selected].bbox.lty+win_y-menu->padding;
   dx = x-submenu->bbox.ltx;
   dy = y-submenu->bbox.lty;
   submenu->bbox.ltx += dx; submenu->bbox.lty += dy;
   submenu->bbox.rbx += dx; submenu->bbox.rby += dy;
   for (i=0; i < submenu->num_items; i++) {
      submenu->menuitems[i].state = TGBS_NORMAL;
   }
   if (submenu->refresh_proc != NULL) {
      (submenu->refresh_proc)(submenu);
   }
   rc = TgMenuLoop(submenu);

   if (need_to_restore_active_menu) {
      activeMenu = saved_active_menu;
   }
   return rc;
}

static
int ScrollItemCallback(pv_userdata)
   void *pv_userdata;
   /* returns TRUE to cancel scrolling */
{
   TgMenu *menu=((TgMenu*)pv_userdata);
   int v_scroll=TRUE;

   switch (menu->type) {
   case TGMUTYPE_TEXT: v_scroll = TRUE; break;
   case TGMUTYPE_COLOR: v_scroll = FALSE; break;
   case TGMUTYPE_BITMAP: v_scroll = FALSE; break;
   }
   if (menu->can_scroll == SCRL_UP || menu->can_scroll == SCRL_LF) {
      /* can_scroll temporary used as scroll_dir */
      if (menu->first_index == 0) return FALSE;
      menu->first_index--;
   } else if (v_scroll) {
      if (menu->num_items <= menuRowsBeforeScroll ||
            menu->first_index+menuRowsBeforeScroll == menu->num_items) {
         return FALSE;
      }
      menu->first_index++;
   } else {
      if (menu->num_cols <= menuColsBeforeScroll ||
            menu->first_index+menuColsBeforeScroll == menu->num_cols) {
         return FALSE;
      }
      menu->first_index++;
   }
   TgDrawEntireMenu(menu);
   XSync(mainDisplay, False);

   return FALSE;
}

static
int ScrollPageCallback(pv_userdata)
   void *pv_userdata;
   /* returns TRUE to cancel scrolling */
{
   TgMenu *menu=((TgMenu*)pv_userdata);
   int v_scroll=TRUE;

   switch (menu->type) {
   case TGMUTYPE_TEXT: v_scroll = TRUE; break;
   case TGMUTYPE_COLOR: v_scroll = FALSE; break;
   case TGMUTYPE_BITMAP: v_scroll = FALSE; break;
   }
   if (menu->can_scroll == SCRL_UP || menu->can_scroll == SCRL_LF) {
      /* can_scroll temporary used as scroll_dir */
      if (menu->first_index == 0) return FALSE;
      menu->first_index -= menuRowsBeforeScroll;
      if (menu->first_index < 0) menu->first_index = 0;
   } else if (v_scroll) {
      if (menu->num_items <= menuRowsBeforeScroll ||
            menu->first_index+menuRowsBeforeScroll == menu->num_items) {
         return FALSE;
      }
      menu->first_index += menuRowsBeforeScroll;
      if (menu->first_index+menuRowsBeforeScroll >= menu->num_items) {
         menu->first_index = menu->num_items-menuRowsBeforeScroll;
      }
   } else {
      if (menu->num_cols <= menuColsBeforeScroll ||
            menu->first_index+menuColsBeforeScroll == menu->num_cols) {
         return FALSE;
      }
      menu->first_index += menuColsBeforeScroll;
      if (menu->first_index+menuColsBeforeScroll >= menu->num_cols) {
         menu->first_index = menu->num_cols-menuColsBeforeScroll;
      }
   }
   TgDrawEntireMenu(menu);
   XSync(mainDisplay, False);

   return FALSE;
}

static
int DoScrollMenu(menu, scroll_page, scroll_dir, pbbox)
   TgMenu *menu;
   int scroll_page, scroll_dir;
   struct BBRec *pbbox;
{
   int v_scroll=TRUE;
   ScrollBtnCallbackInfo sbci;

   /*
    * temporary use can_scroll for scroll_dir
    * menu->can_scroll must be restored to TRUE before returning
    */
   menu->can_scroll = scroll_dir;
   memset(&sbci, 0, sizeof(ScrollBtnCallbackInfo));

   switch (menu->type) {
   case TGMUTYPE_TEXT: v_scroll = TRUE; break;
   case TGMUTYPE_COLOR: v_scroll = FALSE; break;
   case TGMUTYPE_BITMAP: v_scroll = FALSE; break;
   }
   if (scroll_page) {
      sbci.ms = 200;
      sbci.pv_userdata = menu;
      sbci.pf_scroll_btn_callback = ScrollPageCallback;
      if (TgPressButtonLoop(mainDisplay, menu->window, NULL, &sbci)) {
         if (scroll_dir == SCRL_UP || scroll_dir == SCRL_LF) {
            if (menu->first_index == 0) {
               menu->can_scroll = TRUE;
               return TRUE;
            }
            menu->first_index -= menuRowsBeforeScroll;
            if (menu->first_index < 0) menu->first_index = 0;
         } else if (v_scroll) {
            if (menu->num_items <= menuRowsBeforeScroll ||
                  menu->first_index+menuRowsBeforeScroll == menu->num_items) {
               menu->can_scroll = TRUE;
               return TRUE;
            }
            menu->first_index += menuRowsBeforeScroll;
            if (menu->first_index+menuRowsBeforeScroll >= menu->num_items) {
               menu->first_index = menu->num_items-menuRowsBeforeScroll;
            }
         } else {
            if (menu->num_cols <= menuColsBeforeScroll ||
                  menu->first_index+menuColsBeforeScroll == menu->num_cols) {
               menu->can_scroll = TRUE;
               return TRUE;
            }
            menu->first_index += menuColsBeforeScroll;
            if (menu->first_index+menuColsBeforeScroll >= menu->num_cols) {
               menu->first_index = menu->num_cols-menuColsBeforeScroll;
            }
         }
      }
   } else {
      sbci.ms = 50;
      sbci.pv_userdata = menu;
      sbci.pf_scroll_btn_callback = ScrollItemCallback;
      if (TgPressButtonLoop(mainDisplay, menu->window, pbbox, &sbci)) {
         if (scroll_dir == SCRL_UP || scroll_dir == SCRL_LF) {
            if (menu->first_index == 0) {
               menu->can_scroll = TRUE;
               return TRUE;
            }
            menu->first_index--;
         } else if (v_scroll) {
            if (menu->num_items <= menuRowsBeforeScroll ||
                  menu->first_index+menuRowsBeforeScroll == menu->num_items) {
               menu->can_scroll = TRUE;
               return TRUE;
            }
            menu->first_index++;
         } else {
            if (menu->num_cols <= menuColsBeforeScroll ||
                  menu->first_index+menuColsBeforeScroll == menu->num_cols) {
               menu->can_scroll = TRUE;
               return TRUE;
            }
            menu->first_index++;
         }
      }
   }
   menu->can_scroll = TRUE;
   return FALSE;
}

static
void DoDragInScrollMenu(menu, menu_w, menu_h, mouse_x, mouse_y, btn_offset)
   TgMenu *menu;
   int menu_w, menu_h, mouse_x, mouse_y, btn_offset;
{
   double start_frac=(double)0, frac=(double)0;
   int done=FALSE, block_start=0, block_h=0, block_w=0, v_scroll=TRUE;
   XEvent ev;

   switch (menu->type) {
   case TGMUTYPE_TEXT: v_scroll = TRUE; break;
   case TGMUTYPE_COLOR: v_scroll = FALSE; break;
   case TGMUTYPE_BITMAP: v_scroll = FALSE; break;
   }
   if (v_scroll) {
      if (menu->num_items <= menuRowsBeforeScroll) return;

      frac = (double)((double)menuRowsBeforeScroll / (double)(menu->num_items));
      if (threeDLook) {
         block_h = (int)((menu_h-(scrollBarW<<1)) * frac);
      } else {
         block_h = (int)(menu_h * frac);
      }
      if (threeDLook) {
         block_start = mouse_y + btn_offset;;
         start_frac = (double)((double)(block_start-scrollBarW) /
               (double)(menu_h-(scrollBarW<<1)));
         if (block_start+block_h >= menu_h-scrollBarW) {
            menu->first_index = menu->num_items - menuRowsBeforeScroll;
         } else {
            menu->first_index = (int)(menu->num_items * start_frac);
         }
      } else {
         block_start = mouse_y;
         start_frac = (double)((double)(block_start) / (double)(menu_h));
         if (block_start+block_h >= menu_h) {
            menu->first_index = menu->num_items - menuRowsBeforeScroll;
         } else {
            menu->first_index = (int)(menu->num_items * start_frac);
         }
      }
   } else {
      if (menu->num_cols <= menuColsBeforeScroll) return;

      frac = (double)((double)menuColsBeforeScroll / (double)(menu->num_cols));
      if (threeDLook) {
         block_w = (int)((menu_w-(scrollBarW<<1)) * frac);
      } else {
         block_w = (int)(menu_w * frac);
      }
      if (threeDLook) {
         block_start = mouse_x + btn_offset;;
         start_frac = (double)((double)(block_start-scrollBarW) /
               (double)(menu_w-(scrollBarW<<1)));
         if (block_start+block_w >= menu_w-scrollBarW) {
            menu->first_index = menu->num_cols - menuColsBeforeScroll;
         } else {
            menu->first_index = (int)(menu->num_cols * start_frac);
         }
      } else {
         block_start = mouse_x;
         start_frac = (double)((double)(block_start) / (double)(menu_w));
         if (block_start+block_w >= menu_w) {
            menu->first_index = menu->num_cols - menuColsBeforeScroll;
         } else {
            menu->first_index = (int)(menu->num_cols * start_frac);
         }
      }
   }
   TgDrawEntireMenu(menu);

   if (!(menu->parent_menu == NULL && !debugNoPointerGrab)) {
      XGrabPointer(mainDisplay, menu->window, False,
            PointerMotionMask | ButtonReleaseMask, GrabModeAsync,
            GrabModeAsync, None, handCursor, CurrentTime);
   }
   while (!done) {
      XNextEvent(mainDisplay, &ev);

      if (ev.type == Expose || ev.type == VisibilityNotify) {
         ExposeEventHandler(&ev, TRUE);
      } else if (ev.type == ButtonRelease) {
         if (!(menu->parent_menu == NULL && !debugNoPointerGrab)) {
            XUngrabPointer(mainDisplay, CurrentTime);
            if (debugNoPointerGrab) XSync(mainDisplay, False);
         }
         done = TRUE;
      } else if (ev.type == MotionNotify) {
         int new_name_first=0;

         if (v_scroll) {
            int y=ev.xmotion.y;

            if (threeDLook) {
               y += btn_offset;
               start_frac = (double)(((double)(y-scrollBarW)) /
                    ((double)(menu_h-(scrollBarW<<1)-block_h)));

               if (y <= scrollBarW) {
                  new_name_first = 0;
               } else if (y+block_h >= menu_h-scrollBarW) {
                  new_name_first = menu->num_items - menuRowsBeforeScroll;
               } else {
                  new_name_first =
                        (int)((menu->num_items-menuRowsBeforeScroll) *
                        start_frac);
               }
            } else {
               start_frac = (double)(((double)y) / ((double)menu_h));

               if (y <= 0) {
                  new_name_first = 0;
               } else if (y+block_h >= menu_h) {
                  new_name_first = menu->num_items - menuRowsBeforeScroll;
               } else {
                  new_name_first = (int)(menu->num_items * start_frac);
               }
            }
         } else {
            int x=ev.xmotion.x;

            if (threeDLook) {
               x += btn_offset;
               start_frac = (double)(((double)(x-scrollBarW)) /
                    ((double)(menu_w-(scrollBarW<<1)-block_w)));

               if (x <= scrollBarW) {
                  new_name_first = 0;
               } else if (x+block_w >= menu_w-scrollBarW) {
                  new_name_first = menu->num_cols - menuColsBeforeScroll;
               } else {
                  new_name_first =
                        (int)((menu->num_cols-menuColsBeforeScroll) *
                        start_frac);
               }
            } else {
               start_frac = (double)(((double)x) / ((double)menu_w));

               if (x <= 0) {
                  new_name_first = 0;
               } else if (x+block_w >= menu_w) {
                  new_name_first = menu->num_cols - menuColsBeforeScroll;
               } else {
                  new_name_first = (int)(menu->num_cols * start_frac);
               }
            }
         }
         if (menu->first_index != new_name_first) {
            menu->first_index = new_name_first;
            TgDrawEntireMenu(menu);
         }
         while (XCheckMaskEvent(mainDisplay, PointerMotionMask, &ev)) ;
      }
   }
}

void ScrollMenu(menu, x, y, menu_w, menu_h, button_state)
   TgMenu *menu;
   int x, y, menu_w, menu_h, button_state;
{
   int menu_x=x, menu_y=y, x_off=0, y_off=0;
   int do_drag=FALSE, btn_offset=0, v_scroll=TRUE;
   unsigned int button=Button1;

   if ((button_state & Button1Mask) == Button1Mask) button = Button1;
   if ((button_state & Button2Mask) == Button2Mask) button = Button2;
   if ((button_state & Button3Mask) == Button3Mask) button = Button3;

   switch (menu->type) {
   case TGMUTYPE_TEXT: v_scroll = TRUE; break;
   case TGMUTYPE_COLOR: v_scroll = FALSE; break;
   case TGMUTYPE_BITMAP: v_scroll = FALSE; break;
   }
   if (v_scroll) {
      menu_x = x-menu->scroll_start-windowPadding;
   } else {
      menu_y = y-menu->scroll_start-windowPadding;
   }
   if (!threeDLook && button == Button3) {
      if (DoScrollMenu(menu, FALSE, (v_scroll ? SCRL_UP : SCRL_LF), NULL)) {
         return;
      }
   } else if (!threeDLook && button == Button1) {
      if (DoScrollMenu(menu, FALSE, (v_scroll ? SCRL_DN : SCRL_RT), NULL)) {
         return;
      }
   } else if (button == Button1) {
      if (v_scroll) {
         /* vertical scroll bar */
         if (menu_y < scrollBarW || menu_y >= menu_h-scrollBarW) {
            int which=0;
            struct BBRec bbox;

            x_off = menu->scroll_start+(windowPadding<<1)+(windowPadding>>1);
            if (menu_y < scrollBarW) {
               y_off = (windowPadding>>1);
               which = SCRL_UP;
               SetBBRec(&bbox, x_off, y_off, x_off+scrollBarW,
                     y_off+scrollBarW);
            } else {
               y_off = -(windowPadding>>1);
               which = SCRL_DN;
               SetBBRec(&bbox, x_off, y_off+menu_h-scrollBarW, x_off+scrollBarW,
                     y_off+menu_h);
            }
            if (DoScrollMenu(menu, FALSE, which, &bbox)) {
               return;
            }
         } else {
            double start_frac=(double)0;
            int hit=0;

            start_frac = (menu->num_items > 0) ?
                  (double)((double)(menu->first_index) /
                  (double)(menu->num_items)) : ((double)0.0);
            hit = TgGetScrollHit(menu_x, menu_y, VERT_SCROLLBAR,
                  scrollBarW, menu_h, start_frac, menuRowsBeforeScroll,
                  menu->num_items, &btn_offset);
            if (hit == 0) {
               do_drag = TRUE;
            } else {
               if (DoScrollMenu(menu, TRUE, (hit<0 ? SCRL_UP : SCRL_DN),
                     NULL)) {
                  return;
               }
            }
         }
      } else {
         /* horizontal scroll bar */
         if (menu_x < scrollBarW || menu_x >= menu_w-scrollBarW) {
            int which=0;
            struct BBRec bbox;

            y_off = menu->scroll_start+(windowPadding<<1)+(windowPadding>>1);
            if (menu_x < scrollBarW) {
               x_off = (windowPadding>>1);
               which = SCRL_LF;
               SetBBRec(&bbox, x_off, y_off, x_off+scrollBarW,
                     y_off+scrollBarW);
            } else {
               x_off = -(windowPadding>>1);
               which = SCRL_RT;
               SetBBRec(&bbox, x_off+menu_w-scrollBarW, y_off, x_off+menu_w,
                     y_off+scrollBarW);
            }
            if (DoScrollMenu(menu, FALSE, which, &bbox)) {
               return;
            }
         } else {
            double start_frac=(double)0;
            int hit=0;

            start_frac = (menu->num_items > 0) ?
                  (double)((double)(menu->first_index) /
                  (double)(menu->num_cols)) : ((double)0.0);
            hit = TgGetScrollHit(menu_x, menu_y, HORI_SCROLLBAR,
                  menu_w, scrollBarW, start_frac, menuColsBeforeScroll,
                  menu->num_cols, &btn_offset);
            if (hit == 0) {
               do_drag = TRUE;
            } else {
               if (DoScrollMenu(menu, TRUE, (hit<0 ? SCRL_LF : SCRL_RT),
                     NULL)) {
                  return;
               }
            }
         }
      }
   } else if (!threeDLook && button == Button2) {
      do_drag = TRUE;
   }
   if (do_drag) {
      DoDragInScrollMenu(menu, menu_w, menu_h, x, y, btn_offset);
      return;
   }
   TgDrawEntireMenu(menu);
}

int TgMenuLoop(menu)
   TgMenu *menu;
{
   int menuing=TRUE;
   int orig_x=menu->bbox.ltx, orig_y=menu->bbox.lty;
   int menu_w=menu->bbox.rbx-orig_x+((menu->padding)<<1);
   int menu_h=menu->bbox.rby-orig_y+((menu->padding)<<1);
   int min_pin=0, max_pin=0, suspended=FALSE;
   TgMenuItem *menuitems=NULL;
   Window root_win=None, child_win=None;
   TgMenu *parent_menu=menu->parent_menu;
   int x, y, rc=INVALID, saved_root_x, saved_root_y;
   int bg_pixel=(threeDLook ? myLtGryPixel : myBgPixel);
   unsigned int status;
   XEvent ev;
   XWMHints wmhints;
   XSizeHints sizehints;
   XSetWindowAttributes win_attrs;
   int pinned_x=0, pinned_y=0, menu_pinned=FALSE;
   int tmp_disallow_pinning=inSlideShow;
   int one_line_status=FALSE;
   char status_buf[MAX_STATUS_BTNS+1][MAXSTRING+1];

   SaveStatusStringsIntoBuf(status_buf, &one_line_status);

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
   menuitems = menu->menuitems;

   if (menu->track_menubar && menubarWindow != None) {
      SetMenubarWinBBox();
   } else if (menu->parent_menu != NULL && menu->track_parent_menu) {
      SetParentMenuWinBBox(menu);
   }
   AdjMenuGeometry(menu, &orig_x, &orig_y, &min_pin, &max_pin);

   if ((menu->window=XCreateSimpleWindow(mainDisplay, rootWindow,
         orig_x+windowPadding, orig_y+windowPadding, menu_w, menu_h,
         menu->brdr_w, myBorderPixel, bg_pixel)) == 0) {
      FailToCreateWindowMessage("TgMenuLoop()", NULL, FALSE);
      return rc;
   }
   win_attrs.save_under = True;
   win_attrs.override_redirect = True;
   win_attrs.colormap = mainColormap;
   XChangeWindowAttributes(mainDisplay, menu->window,
         CWSaveUnder | CWOverrideRedirect | CWColormap, &win_attrs);

   wmhints.flags = InputHint | StateHint;
   wmhints.input = True;
   wmhints.initial_state = NormalState;
   XSetWMHints(mainDisplay, menu->window, &wmhints);

   sizehints.flags = PPosition | PSize | USPosition | PMinSize | PMaxSize;
   sizehints.x = orig_x+windowPadding;
   sizehints.y = orig_y+windowPadding;
   sizehints.width = sizehints.min_width = sizehints.max_width = menu_w;
   sizehints.height = sizehints.min_height = sizehints.max_height = menu_h;
#ifdef NOTR4MODE
   XSetNormalHints(mainDisplay, menu->window, &sizehints);
#else
   XSetWMNormalHints(mainDisplay, menu->window, &sizehints);
#endif

   menu->selected_index = INVALID;

   RegisterWM_DELETE_WINDOW(menu->window);
   XSetTransientForHint(mainDisplay, menu->window, mainWindow);

#ifdef MAPBEFORESELECT
   XMapWindow(mainDisplay, menu->window);
   XSelectInput(mainDisplay, menu->window, StructureNotifyMask |
         ExposureMask | KeyPressMask | ButtonPressMask | ButtonReleaseMask |
         PointerMotionMask | EnterWindowMask | LeaveWindowMask);
#else
   XSelectInput(mainDisplay, menu->window, StructureNotifyMask |
         ExposureMask | KeyPressMask | ButtonPressMask | ButtonReleaseMask |
         PointerMotionMask | EnterWindowMask | LeaveWindowMask);
   XMapWindow(mainDisplay, menu->window);
#endif

   if (menu->parent_menu == NULL &&
         !(menu->track_menubar && menubarWindow != None) &&
         !menu->track_parent_menu) {
      XWarpPointer(mainDisplay,None,rootWindow,0,0,0,0,orig_x-2,orig_y-2);
   }
   XQueryPointer(mainDisplay, menu->window, &root_win, &child_win,
         &saved_root_x, &saved_root_y, &x, &y, &status);

   if (!debugNoPointerGrab) {
      XGrabPointer(mainDisplay, menu->window, FALSE,
            PointerMotionMask | ButtonPressMask | ButtonReleaseMask,
            GrabModeAsync, GrabModeAsync, None, handCursor, CurrentTime);
   }
   while (menuing) {
      int root_x, root_y, any_button_down;

      XNextEvent(mainDisplay, &ev);
      if (ev.type == Expose || ev.type == VisibilityNotify) {
         if (ev.xany.window == menu->window) {
            TgDrawEntireMenu(menu);
         } else {
            ExposeEventHandler(&ev, FALSE);
         }
      }
      if (!debugNoPointerGrab &&
            (ev.type == ButtonPress || ev.type == ButtonRelease)) {
         XButtonEvent *button_ev=(&(ev.xbutton));

         XQueryPointer(mainDisplay, menu->window, &root_win, &child_win,
               &root_x, &root_y, &x, &y, &status);
         x = button_ev->x;
         y = button_ev->y;
         if (ev.type == ButtonPress) {
            switch (button_ev->button) {
            case Button1: status = Button1Mask; break;
            case Button2: status = Button2Mask; break;
            case Button3: status = Button3Mask; break;
            }
         }
      } else {
         XQueryPointer(mainDisplay, menu->window, &root_win, &child_win,
               &root_x, &root_y, &x, &y, &status);
      }
      any_button_down = ((status & BUTTONSMASK) != 0);
      if (suspended) {
         if (!any_button_down) {
            continue;
         }
         suspended = FALSE;
         tmp_disallow_pinning = TRUE;
      }
      if (!any_button_down) {
         if (!menu_pinned &&
               abs(root_x-saved_root_x)<=2 && abs(root_y-saved_root_y)<=2) {
            suspended = TRUE;
            continue;
         } else if (menu->parent_menu != NULL &&
               PointInBBox(root_x, root_y, menu->parent_menu->bbox) &&
               TgPointInParentMenuSelectedItem(menu->parent_menu)) {
            suspended = TRUE;
            continue;
         }
         if (menu_pinned) {
            XDrawRectangle(mainDisplay, rootWindow, revDefaultGC,
                  pinned_x, pinned_y, menu_w+(brdrW<<1), menu_h+(brdrW<<1));
            XSetSubwindowMode(mainDisplay, revDefaultGC, ClipByChildren);
            pinned_x = root_x;
            pinned_y = root_y;
            TgRealizePinnedMenuWindow(menu, pinned_x, pinned_y,
                     menu_w+(brdrW<<1), menu_h+(brdrW<<1));
            rc = INVALID;
         } else if (x >= 0 && x < menu_w && y >= 0 && y < menu_h) {
            rc = TgWhichMenuIndex(menu, x, y, FALSE);
            switch (rc) {
            case INVALID: break;
            case (-2):
               /* separator */
               rc = INVALID;
               suspended = TRUE;
               continue;
            case (-3):
               /* scrollbar */
               rc = INVALID;
               suspended = TRUE;
               continue;
            default:
               if (menu->menuitems[rc].cmdid != INVALID &&
                     ((menu->menuitems[rc].flags & TGMU_DISABLED) ==
                     TGMU_DISABLED)) {
                  rc = INVALID;
                  suspended = TRUE;
                  continue;
               } else if (menu->is_main_menu) {
                  rc = menu->menuitems[rc].cmdid;
               } else if (menu->menuitems[rc].cmdid != INVALID &&
                     ((menu->menuitems[rc].flags & TGMU_DISABLED) !=
                     TGMU_DISABLED)) {
                  if (gstMenuDontSendCommandInfo.dont_send_command) {
                     gstMenuDontSendCommandInfo.selected_index = rc;
                  } else {
                     SendCommandToSelf(menu->menuitems[rc].cmdid, rc);
                  }
                  /*
                   * Nothing catches -4, so all the parent menus will close.
                   */
                  rc = (-4);
               }
               break;
            }
         } else {
            rc = INVALID;
         }
         menuing = FALSE;
      } else if (menu_pinned) {
         XDrawRectangle(mainDisplay, rootWindow, revDefaultGC,
               pinned_x, pinned_y, menu_w+(brdrW<<1), menu_h+(brdrW<<1));
         pinned_x = root_x;
         pinned_y = root_y;
         XDrawRectangle(mainDisplay, rootWindow, revDefaultGC,
               pinned_x, pinned_y, menu_w+(brdrW<<1), menu_h+(brdrW<<1));
      } else if (x >= 0 && x < menu_w && y >=0 && y < menu_h) {
         int new_selected=TgWhichMenuIndex(menu, x, y, FALSE);

         if (new_selected == (-3) || menu->selected_index != new_selected) {
            if (menu->selected_index != INVALID) {
               if (gnUncheckWhenMoused && !threeDLook &&
                     menu->type != TGMUTYPE_TEXT &&
                     ((menuitems[menu->selected_index].flags &
                     TGMU_SEPARATOR) == 0) &&
                     menuitems[menu->selected_index].checked) {
                  menuitems[menu->selected_index].checked = FALSE;
               }
               menuitems[menu->selected_index].state = TGBS_NORMAL;
               TgDrawMenuItem(menu, &menuitems[menu->selected_index]);
            }
            if (new_selected >= 0) {
               menuitems[new_selected].state = TGBS_RAISED;
               TgDrawMenuItem(menu, &menuitems[new_selected]);
               if (menuitems[new_selected].status_str != NULL) {
                  /* the status_str has been translated */
                  SetStringStatus(menuitems[new_selected].status_str);
               }
            }
            switch (new_selected) {
            case INVALID: break;
            case (-2): new_selected = INVALID; break; /* separator */
            case (-3):
               /* scrollbar */
               if (menu->scroll_start > 0 && tmp_disallow_pinning) {
                  /* loop to scroll */
                  ScrollMenu(menu, x, y, menu_w, menu_h, (status&BUTTONSMASK));
               }
               new_selected = INVALID;
               XQueryPointer(mainDisplay, menu->window, &root_win, &child_win,
                     &root_x, &root_y, &x, &y, &status);
               any_button_down = ((status & BUTTONSMASK) != 0);
               if (!any_button_down) {
                  suspended = TRUE;
               }
               if (!debugNoPointerGrab) {
                  XGrabPointer(mainDisplay, menu->window, FALSE,
                        PointerMotionMask | ButtonPressMask | ButtonReleaseMask,
                        GrabModeAsync, GrabModeAsync, None, handCursor,
                        CurrentTime);
               }
               break;
            default: break;
            }
            menu->selected_index = new_selected;
            if (new_selected != INVALID &&
                  (menuitems[new_selected].flags & TGMU_HAS_SUBMENU) ==
                  TGMU_HAS_SUBMENU &&
                  (menuitems[new_selected].flags & TGMU_DISABLED) == 0 &&
                  menuitems[new_selected].submenu_create_info != NULL) {
               TgMenu *submenu=NULL;

               submenu = TgCreatePopUpSubMenu(menu, new_selected);
               if (submenu != NULL) {
#ifdef _CHAMELEON_BUG
                  if (!debugNoPointerGrab) {
                     XUngrabPointer(mainDisplay, CurrentTime);
                  }
#endif /* _CHAMELEON_BUG */
                  rc = TgPopUpSubMenu(menu, orig_x, orig_y);
                  submenu = menuitems[new_selected].detail.submenu;
                  if (submenu != NULL) {
                     TgDestroyMenu(submenu, TRUE);
                     menuitems[new_selected].detail.submenu = NULL;
                  }
               }
               if (rc != INVALID && rc != (-3)) {
                  menuing = FALSE;
               } else if (!debugNoPointerGrab) {
                  XGrabPointer(mainDisplay, menu->window, FALSE,
                        PointerMotionMask | ButtonPressMask | ButtonReleaseMask,
                        GrabModeAsync, GrabModeAsync, None, handCursor,
                        CurrentTime);
               }
            }
         }
         tmp_disallow_pinning = inSlideShow;
      } else if (menu->parent_menu != NULL && menu->track_parent_menu &&
            PointInBBox(root_x, root_y, gstMenubarWinBBox) &&
            !TgPointInParentMenuSelectedItem(menu->parent_menu)) {
         menuing = FALSE;
         rc = BAD;
      } else if (menu->parent_menu != NULL &&
            PointInBBox(root_x, root_y, menu->parent_menu->bbox)) {
         /* we are in the parent menu */
         if (!TgPointInParentMenuSelectedItem(menu->parent_menu)) {
            menuing = FALSE;
            rc = (-3);
         }
      } else if (menu->parent_menu != NULL &&
            TgPointInAnAncestorMenu(menu->parent_menu, root_x, root_y)) {
         /* we are in an ancestor menu */
         menuing = FALSE;
         rc = (-3);
      } else if (menu->track_menubar && menubarWindow != None &&
            PointInBBox(root_x, root_y, gstMenubarWinBBox) &&
            WhichMenubarItem(root_x-gstMenubarWinBBox.ltx,
            root_y-gstMenubarWinBBox.lty, NULL, NULL, NULL) != INVALID &&
            !PointInBBox(root_x-gstMenubarWinBBox.ltx,
            root_y-gstMenubarWinBBox.lty, excludeMenubarWinBBox)) {
         menuing = FALSE;
         rc = BAD;
      } else if (!menu->disallow_pinning && !tmp_disallow_pinning &&
            !menu_pinned && (activeMenu != INVALID || menu->is_main_menu) &&
            (root_x < min_pin || root_x > max_pin)) {
         pinned_x = root_x;
         pinned_y = root_y;
         XSetSubwindowMode(mainDisplay, revDefaultGC, IncludeInferiors);
         XDrawRectangle(mainDisplay, rootWindow, revDefaultGC,
               pinned_x, pinned_y, menu_w+(brdrW<<1), menu_h+(brdrW<<1));
         SetStringStatus(TgLoadCachedString(CSTID_RELEASE_MOUSE_TO_PIN_MENU));
         menu_pinned = TRUE;
      } else if (menu->selected_index != INVALID) {
         menuitems[menu->selected_index].state = TGBS_NORMAL;
         TgDrawMenuItem(menu, &menuitems[menu->selected_index]);
         menu->selected_index = INVALID;
      }
   }
   if (parent_menu == NULL && !debugNoPointerGrab) {
      XUngrabPointer(mainDisplay, CurrentTime);
   }
   if (menu->window != None) XDestroyWindow(mainDisplay, menu->window);

   XSync(mainDisplay, False);

   RestoreStatusStringsFromBuf(status_buf, one_line_status);

   return rc;
}

TgMenu *TgDestroyMenu(menu, free_menu)
   TgMenu *menu;
   int free_menu;
   /* return NULL if menu is freed */
{
   int i, num_items=menu->num_items;
   TgMenuItem *menuitems=NULL;

   menuitems = menu->menuitems;
   if (menuitems != NULL) {
      for (i=0; i < num_items; i++) {
         TgMenuItem *menu_item=(&menuitems[i]);

         UtilFree(menu_item->status_str);
         if (menu_item->menu_str_allocated) {
            UtilFree(menu_item->menu_str);
            menu_item->menu_str = NULL;
         }
         menu_item->status_str = NULL;
         if ((menu_item->flags & TGMU_HAS_SUBMENU) &&
               menu_item->detail.submenu != NULL) {
            TgDestroyMenu(menu_item->detail.submenu, TRUE);
         }
      }
      free(menuitems);
   }
   if (free_menu) {
      free(menu);
      return NULL;
   }
   return menu;
}

static
int TgGetTextMenuItemWidths(menu_item, pn_check_w, pn_str_w, pn_shortcut_w)
   TgMenuItem *menu_item;
   int *pn_check_w, *pn_str_w, *pn_shortcut_w;
{
   int flags=menu_item->flags;

   if ((flags & TGMU_SEPARATOR) == TGMU_SEPARATOR) {
      if (pn_check_w != NULL) *pn_check_w = 0;
      if (pn_str_w != NULL) *pn_str_w = 0;
      if (pn_shortcut_w != NULL) *pn_shortcut_w = 0;
      return TRUE;
   }
   if (pn_check_w != NULL) {
      if ((flags & (TGMU_HAS_CHECK|TGMU_HAS_RADIO)) != 0) {
         *pn_check_w = check_width;
      } else {
         *pn_check_w = 0;
      }
   }
   if (pn_str_w != NULL) {
      char *menu_str=menu_item->menu_str;

      if (menuFontSet != NULL || menuFontPtr != NULL) {
         *pn_str_w = MenuTextWidth(menuFontPtr, _(menu_str),
               strlen(_(menu_str)));
      } else {
         *pn_str_w = defaultFontWidth * strlen(_(menu_str));
      }
   }
   if (pn_shortcut_w != NULL) {
      if (flags & TGMU_HAS_SHORTCUT) {
         char *shortcut_str=menu_item->detail.shortcut_str;

         /* don't translate shortuct_str */
         if (menuFontSet != NULL || menuFontPtr != NULL) {
            *pn_shortcut_w = MenuTextWidth(menuFontPtr, shortcut_str,
                  strlen(shortcut_str));
         } else {
            *pn_shortcut_w = defaultFontWidth * strlen(shortcut_str);
         }
      } else {
         *pn_shortcut_w = 0;
      }
   }
   return TRUE;
}

int TgSetMenuItemInfo(to_menu_item, mask, from_menu_item)
   TgMenuItem *to_menu_item, *from_menu_item;
   int mask;
{
   if (mask & TGMU_MASK_LTXY) {
      to_menu_item->bbox.ltx = from_menu_item->bbox.ltx;
      to_menu_item->bbox.lty = from_menu_item->bbox.lty;
   }
   if (mask & TGMU_SEPARATOR) {
      to_menu_item->flags |= TGMU_SEPARATOR;
      return TRUE;
   }
   if (mask & TGMU_MASK_STATE) to_menu_item->state = from_menu_item->state;
   if (mask & TGMU_MASK_CMDID) to_menu_item->cmdid = from_menu_item->cmdid;
   if (mask & TGMU_MASK_MULTICOLOR) {
      to_menu_item->flags |= TGMU_MULTICOLOR;
      to_menu_item->multicolor_pixel = from_menu_item->multicolor_pixel;
   }
   if (mask & TGMU_MASK_PXMPBTN1) {
      to_menu_item->checked_pxmpbtn = from_menu_item->checked_pxmpbtn;
   }
   if (mask & TGMU_MASK_PXMPBTN2) {
      to_menu_item->unchecked_pxmpbtn = from_menu_item->unchecked_pxmpbtn;
   }
   if (mask & TGMU_MASK_CHECK) {
      to_menu_item->flags |= TGMU_HAS_CHECK;
      to_menu_item->flags &= (~TGMU_HAS_RADIO);
      to_menu_item->checked = from_menu_item->checked;
   }
   if (mask & TGMU_MASK_RADIO) {
      to_menu_item->flags |= TGMU_HAS_RADIO;
      to_menu_item->flags &= (~TGMU_HAS_CHECK);
      to_menu_item->checked = from_menu_item->checked;
   }
   if (mask & TGMU_MASK_MENUSTR) {
      to_menu_item->menu_str = from_menu_item->menu_str;
   }
   if (mask & TGMU_MASK_STATUSSTR) {
      char *psz=NULL;

      UtilFree(to_menu_item->status_str);

      if (mask & TGMU_MASK_RAWSTATUSSTR) {
         /* the from_menu_item->status_str has NOT been translated */
         to_menu_item->status_str = UtilStrDup(_(from_menu_item->status_str));
      } else {
         /* the from_menu_item->status_str has been translated */
         to_menu_item->status_str = UtilStrDup(from_menu_item->status_str);
      }
      if (to_menu_item->status_str == NULL) FailAllocMessage();
      /* do not translate -- program constants */
      if (from_menu_item->status_str != NULL &&
            (psz=strstr(from_menu_item->status_str, "<<PROGRAM_NAME>>")) !=
            NULL) {
         char *psz1=(&psz[strlen("<<PROGRAM_NAME>>")]);

         sprintf(&to_menu_item->status_str[psz-from_menu_item->status_str],
               "%s%s", TOOL_NAME, psz1);
      }
   }
   if (mask & TGMU_MASK_SUBMENU) {
      to_menu_item->flags |= TGMU_HAS_SUBMENU;
      to_menu_item->flags &= (~TGMU_HAS_SHORTCUT);
      to_menu_item->submenu_create_info = from_menu_item->submenu_create_info;
      if (mask & TGMU_MASK_PINNABLESUBMENU) {
         to_menu_item->flags |= TGMU_SUBMENU_PINNABLE;
      }
   }
   if (mask & TGMU_MASK_SHORTCUTSTR) {
      to_menu_item->flags |= TGMU_HAS_SHORTCUT;
      to_menu_item->flags &= (~TGMU_HAS_SUBMENU);
      to_menu_item->detail.shortcut_str = from_menu_item->detail.shortcut_str;
   }
   if (mask & TGMU_MASK_USERDATA) {
      to_menu_item->userdata = from_menu_item->userdata;
   }
   return TRUE;
}

static
int TgAdjustMenuItemHeight(menu, menu_item)
   TgMenu *menu;
   TgMenuItem *menu_item;
{
   int flags=menu_item->flags, item_w=0, item_h=0;

   if (flags & TGMU_SEPARATOR) {
      menu_item->bbox.rby = menu_item->bbox.lty+separatorHeight;
      return TRUE;
   }
   switch (menu->type) {
   case TGMUTYPE_TEXT:
      if (menuFontSet != NULL || menuFontPtr != NULL) {
         menu_item->bbox.rby = menu_item->bbox.lty+menuFontHeight;
      } else {
         menu_item->bbox.rby = menu_item->bbox.lty+defaultFontHeight;
      }
      break;
   case TGMUTYPE_COLOR:
   case TGMUTYPE_BITMAP:
      item_w = menu->image_w;
      item_h = menu->image_h;
      if (threeDLook) {
         item_w += (windowPadding<<1);
         item_h += (windowPadding<<1);
      }
      menu_item->bbox.rbx = menu_item->bbox.ltx+item_w;
      menu_item->bbox.rby = menu_item->bbox.lty+item_h;
      break;
   }
   return TRUE;
}

void TgAdjustMenuGeometry(menu, image_w, image_h, max_rows)
   TgMenu *menu;
   int image_w, image_h, max_rows;
   /* only for TGMUTYPE_BITMAP or TGMUTYPE_COLOR */
{
   int max_h=0, item_w=image_w, item_h=image_h;
   int i, xoff=0, yoff=0, max_col_w=0, max_w=0, num_items=menu->num_items;

   if (threeDLook) {
      item_w += (windowPadding<<1);
      item_h += (windowPadding<<1);
   }
   menu->image_w = image_w;
   menu->image_h = image_h;
   menu->num_rows = max_rows;
   menu->num_cols = (((num_items % max_rows) == 0) ? num_items / max_rows :
         ((int)(num_items/max_rows))+1);
   xoff = menu->padding;
   yoff = menu->padding;
   for (i=0; i < num_items; i++) {
      int flags=TGMU_MASK_LTXY, col_w=0, item_h=0;
      TgMenuItem *menu_item=(&menu->menuitems[i]);
      TgMenuItem stMenuItem;

      memset(&stMenuItem, 0, sizeof(TgMenuItem));

      stMenuItem.bbox.ltx = xoff;
      stMenuItem.bbox.lty = yoff;
      if (menu_item->menu_str == TGMUITEM_SEPARATOR) {
         flags |= TGMU_SEPARATOR;
      }
      if (!TgSetMenuItemInfo(menu_item, flags, &stMenuItem)) {
         TgDestroyMenu(menu, TRUE); /* frees menu in TgDestroyMenu() */
         return;
      }
      /* now adjust the width and height of menu_item->bbox */
      TgAdjustMenuItemHeight(menu, menu_item);
      col_w = menu_item->bbox.rbx-menu_item->bbox.ltx;
      item_h = menu_item->bbox.rby-menu_item->bbox.lty;
      if (col_w > max_col_w) max_col_w = col_w;
      yoff += item_h;
      if (xoff+max_col_w > max_w) max_w = xoff+max_col_w;
      if (yoff > max_h) max_h = yoff;
      if (((i+1) % max_rows) == 0) {
         xoff += max_col_w;
         yoff = menu->padding;
      }
   }
   menu->check_start = menu->str_start = menu->shortcut_start =
         menu->arrow_start = xoff;
   menu->bbox.rbx = menu->bbox.ltx+max_w-menu->padding;
   menu->bbox.rby = menu->bbox.lty+max_h-menu->padding;

   maxScrollableMenuWidth = (menuColsBeforeScroll*item_w) +
         (windowPadding<<1);
   if (menu->can_scroll && (max_w+windowPadding) > maxScrollableMenuWidth) {
      menu->scroll_start = (item_h*max_rows);
   }
}

static
int CreateTextMenuItemsFromMenuItemInfo(menu, menu_iteminfos, status_str_xlated)
   TgMenu *menu;
   TgMenuItemInfo *menu_iteminfos;
   int status_str_xlated;
{
   TgMenuItemInfo *item_info=NULL;
   int i, xoff=0, yoff=0, max_str_w=0, max_shortcut_w=submenu_width, total_w=0;
   int num_items=0;

   for (item_info=menu_iteminfos; item_info->menu_str != NULL; item_info++) {
      num_items++;
   }
   menu->num_items = num_items;
   menu->menuitems = (TgMenuItem*)malloc(num_items*sizeof(TgMenuItem));
   if (menu->menuitems == NULL) FailAllocMessage();
   memset(menu->menuitems, 0, num_items*sizeof(TgMenuItem));

   xoff = menu->padding;
   yoff = menu->padding;
   for (item_info=menu_iteminfos, i=0; item_info->menu_str != NULL;
         item_info++, i++) {
      int flags=0, check_w=0, str_w=0, shortcut_w=0;
      TgMenuItem *menu_item=(&menu->menuitems[i]);
      TgMenuItem stMenuItem;

      memset(menu_item, 0, sizeof(TgMenuItem));
      memset(&stMenuItem, 0, sizeof(TgMenuItem));

      stMenuItem.bbox.ltx = xoff;
      stMenuItem.bbox.lty = yoff;
      if (item_info->menu_str == TGMUITEM_SEPARATOR) {
         flags |= TGMU_SEPARATOR | TGMU_MASK_LTXY;
      } else {
         stMenuItem.state = TGBS_NORMAL;
         stMenuItem.cmdid = item_info->cmdid;
         stMenuItem.menu_str = item_info->menu_str;
         /* the from_menu_item->status_str has been translated */
         stMenuItem.status_str = item_info->status_str;
         flags |= TGMU_MASK_LTXY | TGMU_MASK_STATE | TGMU_MASK_CMDID |
               TGMU_MASK_MENUSTR | TGMU_MASK_STATUSSTR;
         if (!status_str_xlated) {
            flags |= TGMU_MASK_RAWSTATUSSTR;
         }
         if (item_info->shortcut_str == TGMUITEM_SUBMENU ||
               item_info->shortcut_str == TGMUITEM_PINNABLESUBMENU) {
            flags |= TGMU_MASK_SUBMENU;
            if (item_info->shortcut_str == TGMUITEM_PINNABLESUBMENU) {
               flags |= TGMU_MASK_PINNABLESUBMENU;
            }
            stMenuItem.submenu_create_info = item_info;
         } else if (item_info->shortcut_str != NULL) {
            flags |= TGMU_MASK_SHORTCUTSTR;
            stMenuItem.detail.shortcut_str = item_info->shortcut_str;
         }
      }
      if (!TgSetMenuItemInfo(menu_item, flags, &stMenuItem)) {
         TgDestroyMenu(menu, TRUE); /* frees menu in TgDestroyMenu() */
         return FALSE;
      }
      TgAdjustMenuItemHeight(menu, menu_item);
      TgGetTextMenuItemWidths(menu_item, &check_w, &str_w, &shortcut_w);
      if (str_w > max_str_w) max_str_w = str_w;
      if (shortcut_w > max_shortcut_w) max_shortcut_w = shortcut_w;
      yoff += menu_item->bbox.rby-menu_item->bbox.lty+1;
   }
   menu->check_start = xoff+2;
   menu->str_start = menu->check_start+check_width+2;
   menu->shortcut_start = menu->str_start+max_str_w+(menuFontWidth<<1)+2;
   menu->arrow_start = menu->shortcut_start+max_shortcut_w-submenu_width;
   total_w = menu->arrow_start+submenu_width-xoff+2+menuFontWidth;
   menu->bbox.rbx = menu->bbox.ltx + total_w;
   menu->bbox.rby = menu->bbox.lty + yoff;
   for (i=0; i < num_items; i++) {
      TgMenuItem *menu_item=(&menu->menuitems[i]);

      menu_item->bbox.rbx = menu->shortcut_start+max_shortcut_w+menuFontWidth;
   }
   if (menu->can_scroll && yoff > maxScrollableMenuHeight) {
      menu->scroll_start = total_w;
   }
   return TRUE;
}

static
void SetScrollableMenuSize(menu)
   TgMenu *menu;
{
   switch (menu->type) {
   case TGMUTYPE_TEXT:
      maxScrollableMenuHeight = (menuRowsBeforeScroll *
            (((menuFontSet==NULL && menuFontPtr == NULL) ?
            initialMenubarWindowH : menuFontHeight) + 1)) + (windowPadding<<1);
      break;
   case TGMUTYPE_COLOR:
   case TGMUTYPE_BITMAP:
      /*
       * Do nothing!
       * maxScrollableMenuWidth will be set in TgAdjustMenuGeometry()
       */
      break;
   }
}

static
int CreateBitmapOrColorMenuItemsFromMenuItemInfo(menu, menu_iteminfos,
      status_str_xlated)
   TgMenu *menu;
   TgMenuItemInfo *menu_iteminfos;
   int status_str_xlated;
{
   TgMenuItemInfo *item_info=NULL;
   int i, xoff=0, yoff=0, num_items=0;

   for (item_info=menu_iteminfos; item_info->menu_str != NULL; item_info++) {
      num_items++;
   }
   menu->num_items = num_items;
   menu->menuitems = (TgMenuItem*)malloc(num_items*sizeof(TgMenuItem));
   if (menu->menuitems == NULL) FailAllocMessage();
   memset(menu->menuitems, 0, num_items*sizeof(TgMenuItem));

   xoff = menu->padding;
   yoff = menu->padding;
   for (item_info=menu_iteminfos, i=0; item_info->menu_str != NULL;
         item_info++, i++) {
      int flags=0;
      TgMenuItem *menu_item=(&menu->menuitems[i]);
      TgMenuItem stMenuItem;

      memset(menu_item, 0, sizeof(TgMenuItem));
      memset(&stMenuItem, 0, sizeof(TgMenuItem));

      stMenuItem.bbox.ltx = xoff;
      stMenuItem.bbox.lty = yoff;
      if (item_info->menu_str == TGMUITEM_SEPARATOR) {
         flags |= TGMU_SEPARATOR | TGMU_MASK_LTXY;
      } else {
         stMenuItem.state = TGBS_NORMAL;
         stMenuItem.cmdid = item_info->cmdid;
         stMenuItem.menu_str = item_info->menu_str;
         /* the from_menu_item->status_str has been translated */
         stMenuItem.status_str = item_info->status_str;
         flags |= TGMU_MASK_LTXY | TGMU_MASK_STATE | TGMU_MASK_CMDID |
               TGMU_MASK_MENUSTR | TGMU_MASK_STATUSSTR;
         if (!status_str_xlated) {
            flags |= TGMU_MASK_RAWSTATUSSTR;
         }
         if (item_info->shortcut_str != NULL) {
            flags |= TGMU_MASK_SHORTCUTSTR;
            stMenuItem.detail.shortcut_str = item_info->shortcut_str;
         }
      }
      if (!TgSetMenuItemInfo(menu_item, flags, &stMenuItem)) {
         TgDestroyMenu(menu, TRUE); /* frees menu in TgDestroyMenu() */
         return FALSE;
      }
      TgAdjustMenuItemHeight(menu, menu_item);
   }
   return TRUE;
}

TgMenu *TgCreateMenuFromMenuInfo(parent_menu, x, y, menu_info,
      status_str_xlated)
   TgMenu *parent_menu;
   int x, y, status_str_xlated;
   TgMenuInfo *menu_info;
{
   TgMenu *menu=(TgMenu*)malloc(sizeof(TgMenu));
   TgMenuItemInfo *menu_iteminfos=menu_info->items;

#ifdef _TGIF_DBG /* debug, do not translate */
   if (status_str_xlated == INVALID) {
      XUngrabPointer(mainDisplay, CurrentTime);
      sprintf(gszMsgBox, "Error: %s() is called with %s=INVALID.",
            "TgCreateMenuFromMenuInfo", "status_str_xlated");
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
   }
#endif /* _TGIF_DBG */
   if (menu == NULL) FailAllocMessage();
   memset(menu, 0, sizeof(TgMenu));

   menu->type = (menu_info->type & TGMUTYPE_MASK);
   if ((menu_info->type & TGMUTYPE_CANSCROLL) == TGMUTYPE_CANSCROLL) {
      menu->can_scroll = TRUE; /* scroll_start and first_index already 0 */
   }
   menu_info->type &= TGMUTYPE_MASK;

   menu->num_items = 0;
   menu->selected_index = INVALID;
   menu->bbox.ltx = menu->bbox.rbx = x;
   menu->bbox.lty = menu->bbox.rby = y;
   menu->padding = (threeDLook ? windowPadding : 0);
   menu->brdr_w = (threeDLook ? 0 : (brdrW<<1));
   menu->track_menubar = TRUE;
   menu->parent_menu = parent_menu;

   SetScrollableMenuSize(menu);

   switch (menu_info->type) {
   case TGMUTYPE_TEXT:
      if (!CreateTextMenuItemsFromMenuItemInfo(menu, menu_iteminfos,
            status_str_xlated)) {
         free(menu);
         return NULL;
      }
      break;
   case TGMUTYPE_COLOR:
   case TGMUTYPE_BITMAP:
      if (!CreateBitmapOrColorMenuItemsFromMenuItemInfo(menu, menu_iteminfos,
            status_str_xlated)) {
         free(menu);
         return NULL;
      }
      break;
   }
   return menu;
}

void SetScrollableMenuFirstIndex(menu, selected_item_index)
   TgMenu *menu;
   int selected_item_index;
   /* use this only for radio-button style menu, no separators */
{
   if (menu->scroll_start > 0) {
      int col=0;

      switch (menu->type) {
      case TGMUTYPE_TEXT:
         if (selected_item_index >= menuRowsBeforeScroll) {
            menu->first_index = selected_item_index;
            if (menu->first_index < 0) menu->first_index = 0;
            if (menu->first_index+menuRowsBeforeScroll >= menu->num_items) {
               menu->first_index = menu->num_items-menuRowsBeforeScroll;
            }
         }
         break;
      case TGMUTYPE_COLOR:
      case TGMUTYPE_BITMAP:
         col = (int)(selected_item_index/menu->num_rows);
         if (col >= menuColsBeforeScroll) {
            menu->first_index = col;
            if (menu->first_index < 0) menu->first_index = 0;
            if (menu->first_index+menuColsBeforeScroll >= menu->num_cols) {
               menu->first_index = menu->num_cols-menuColsBeforeScroll;
            }
         }
         break;
      }
   }
}

int RefreshMainMenu(menu)
   TgMenu *menu;
{
   int ok=TRUE;

   ok &= TgEnableMenuItemById(menu, MENU_STACKEDPAGE,
         (pageLayoutMode==PAGE_STACK));
   ok &= TgEnableMenuItemById(menu, MENU_TILEDPAGE,
         (pageLayoutMode!=PAGE_STACK));
   ok &= TgEnableMenuItemById(menu, MENU_COLOR, colorDisplay);

   return ok;
}

TgMenu *CreateMainMenu(parent_menu, x, y, menu_info, status_str_xlated)
   TgMenu *parent_menu;
   int x, y;
   TgMenuInfo *menu_info;
   int status_str_xlated; /* ignored, always 0 */
{
   TgMenu *menu=TgCreateMenuFromMenuInfo(parent_menu, x, y, menu_info, FALSE);

   if (menu != NULL) {
      menu->track_menubar = FALSE;
      menu->is_main_menu = TRUE;
      if (!RefreshMainMenu(menu)) {
         return TgDestroyMenu(menu, TRUE);
      }
      menu->refresh_proc = ((RefreshMenuFunc*)RefreshMainMenu);
   }
   return menu;
}

int MainMenu()
{
   int x=0, y=0, root_x=0, root_y=0;
   int rc=INVALID, index=INVALID;
   Window root_win=None, child_win=None;
   unsigned int status=0;
   TgMenu *menu=NULL;

   Msg("");
   XQueryPointer(mainDisplay, rootWindow, &root_win, &child_win, &root_x,
         &root_y, &x, &y, &status);

   activeMenu = MENU_MAIN;
   if (cmdLineTgrm2) {
      menu = (mainMenuInfo.create_proc)(NULL, x, y, &mainMenuInfo, 0);
   } else {
      TgMenuInfo tgmi;

      memcpy(&tgmi, &mainMenuInfo, sizeof(TgMenuInfo));
      tgmi.items = gpMainMenuItemInfos;
      menu = (tgmi.create_proc)(NULL, x, y, &tgmi, 0);
   }
   if (menu != NULL) {
      index = TgMenuLoop(menu);
      TgDestroyMenu(menu, TRUE);
   }
   activeMenu = INVALID;
   if (index == INVALID || index == (-4)) return INVALID;

   return rc;
}

int IsPrefix(Prefix, Str, Rest)
   char *Prefix, *Str, **Rest;
{
   register char *c_ptr=Prefix;

   for (*Rest=Str; *c_ptr!='\0' && **Rest!='\0'; (*Rest)++, c_ptr++) {
      if (**Rest != *c_ptr) {
         return FALSE;
      }
   }
   return (*c_ptr == '\0' && **Rest == DIR_SEP);
}

void RedrawTitleWindow()
{
   int y, len, amount, left;
   char s[MAXPATHLENGTH], name[MAXPATHLENGTH], * c_ptr, * rest;
   struct BBRec bbox;

   XClearWindow(mainDisplay, titleWindow);

   s[0] = '\0';
   if (curFileDefined) {
      if (*curSymDir == '\0') {
         sprintf(name, "%s%c%s", curDir, DIR_SEP, curFileName);
      } else {
         sprintf(name, "%s%c%s", curSymDir, DIR_SEP, curFileName);
      }
      if (IsPrefix(bootDir, name, &rest)) {
         c_ptr = ++rest;
      } else {
         c_ptr = name;
      }
      FormatFloat(&printMag, gszMsgBox);
      sprintf(s, "%s:%s (%s%%)", curDomainName, c_ptr, gszMsgBox);
   } else {
      FormatFloat(&printMag, gszMsgBox);
      sprintf(s, "%s:%s (%s%%)", curDomainName,
            TgLoadCachedString(CSTID_SQUARE_BRACK_UNNAMED), gszMsgBox);
   }
   if (pageLayoutMode==PAGE_STACK && curPage!=NULL) {
      sprintf(&s[strlen(s)], " \"%s\"",
            (curPage->name==NULL) ? "" : curPage->name);
   }
   if (fileModified) {
      if (IsFiletUnSavable()) {
         sprintf(gszMsgBox, " %s",
               TgLoadCachedString(CSTID_SQUARE_BRACK_MODIFIED_UNSAV));
      } else {
         sprintf(gszMsgBox, " %s",
               TgLoadCachedString(CSTID_SQUARE_BRACK_MODIFIED));
      }
      strcat(s, gszMsgBox);
   }
   if (s[0] != '\0') {
      len = strlen(s);
      if (msgFontSet != NULL || msgFontPtr != NULL) {
         if (msgFontPtr != NULL) {
            XSetFont(mainDisplay, defaultGC, msgFontPtr->fid);
         }
         if (showVersion) {
            DrawMsgString(mainDisplay, titleWindow, defaultGC, 1+windowPadding,
                  (titleWindowH>>1)+msgFontAsc+1, s, len);
         } else {
            DrawMsgString(mainDisplay, titleWindow, defaultGC, 1+windowPadding,
                  msgFontAsc+1+windowPadding, s, len);
         }
         XSetFont(mainDisplay, defaultGC, defaultFontPtr->fid);
      } else {
         if (showVersion) {
            DrawMsgString(mainDisplay, titleWindow, defaultGC, 1+windowPadding,
                  (titleWindowH>>1)+defaultFontAsc+1, s, len);
         } else {
            DrawMsgString(mainDisplay, titleWindow, defaultGC, 1+windowPadding,
                  defaultFontAsc+1+windowPadding, s, len);
         }
      }
   }
   if (showVersion) {
      SetFullVersionString();
      strcpy(s, fullToolName);

      len = strlen(s);
      if (msgFontSet != NULL || msgFontPtr != NULL) {
         amount = MsgTextWidth(msgFontPtr, s, len);
         left = ((titleWindowW-amount)>>1);
         if (msgFontPtr != NULL) {
            XSetFont(mainDisplay, defaultGC, msgFontPtr->fid);
         }
         DrawMsgString(mainDisplay, titleWindow, defaultGC, left,
               msgFontAsc+2+(windowPadding>>1), s, len);
         XSetFont(mainDisplay, defaultGC, defaultFontPtr->fid);
         for (y=4+(windowPadding>>1); y < (titleWindowH>>1)-4; y += 2) {
            XDrawLine(mainDisplay, titleWindow, defaultGC, 2+windowPadding, y,
                  left-msgFontWidth, y);
            XDrawLine(mainDisplay, titleWindow, defaultGC,
                  left+amount+msgFontWidth, y, titleWindowW-3, y);
         }
      } else {
         amount = defaultFontWidth * len;
         left = ((titleWindowW-amount)>>1);
         DrawMsgString(mainDisplay, titleWindow, defaultGC, left,
               defaultFontAsc+2+(windowPadding>>1), s, len);
         for (y=4+(windowPadding>>1); y < (titleWindowH>>1)-4; y += 2) {
            XDrawLine(mainDisplay, titleWindow, defaultGC, 2+windowPadding, y,
                  left-defaultFontWidth, y);
            XDrawLine(mainDisplay, titleWindow, defaultGC,
                  left+amount+defaultFontWidth, y, titleWindowW-3, y);
         }
      }
   }
   if (threeDLook) {
      bbox.ltx = 0;
      bbox.lty = 0;
      bbox.rbx = titleWindowW;
      bbox.rby = titleWindowH;
      TgDrawThreeDButton(mainDisplay, titleWindow, textMenuGC, &bbox,
            TGBS_RAISED, 1, FALSE);
   }
}

static struct ObjRec *iconTopObj=NULL, *iconBotObj=NULL;
static struct ObjRec *iconTgifObj=NULL;
static int justIconified=FALSE;

void RedrawIconWindow()
{
   register struct ObjRec *obj_ptr;

   numRedrawBBox = 0;
   for (obj_ptr = iconBotObj; obj_ptr != NULL; obj_ptr = obj_ptr->prev) {
      obj_ptr->tmp_parent = NULL;
      DrawObj(iconWindow, obj_ptr);
   }
   if (justIconified && iconTgifObj != NULL && iconTgifObj->fattr != NULL) {
      struct ObjRec *obj_ptr;
      struct AttrRec *attr_ptr;

      justIconified = FALSE;
      if ((attr_ptr=FindAttrWithName(iconTgifObj,"icon_init_exec_obj=",NULL)) !=
            NULL && (obj_ptr=FindObjWithName(iconBotObj, iconTgifObj,
            attr_ptr->attr_value.s,FALSE,FALSE,NULL,NULL)) != NULL &&
            (attr_ptr=FindAttrWithName(obj_ptr,EXEC_ATTR,NULL)) != NULL) {
         int saved_intr_check_interval=intrCheckInterval;
         int saved_history_depth=historyDepth;

         intrCheckInterval = 1;
         historyDepth = 0;
         ShowInterrupt(1);

         DoExec(attr_ptr, obj_ptr);

         HideInterrupt();
         intrCheckInterval = saved_intr_check_interval;
         historyDepth = saved_history_depth;
      }
   }
   justIconified = FALSE;
}

static char iconFileName[] = "tgificon";

static
void InitIcon()
{
   struct ObjRec *obj_ptr, *saved_tgif_obj;
   char s[MAXPATHLENGTH], *c_ptr, msg[MAXSTRING], ext_str[MAXPATHLENGTH];
   FILE *fp=NULL;
   int ltx=0, lty=0, rbx=0, rby=0, seen_obj=FALSE;
   int dx, dy, w, h, len, ext_len, x, y, read_status;
   unsigned int icon_w, icon_h;
   XSizeHints sizehints;
   int tmp_linenum;
   char tmp_filename[MAXPATHLENGTH], tmp_filefullpath[MAXPATHLENGTH];

   DelAllPages();
   lastPageNum = 1;
   InitPage();

   iconWindowCreated = FALSE;
   if (((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"UseWMIconPixmap")) == NULL) ||
         UtilStrICmp(c_ptr,"false") != 0) {
      return;
   }
   if (((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"NoTgifIcon")) != NULL)
         && UtilStrICmp(c_ptr,"True") == 0) {
      return;
   }
   strcpy(s, drawPath);
   strcat(s, DIR_SEP_STR);
   if ((c_ptr=getenv("TGIFICON")) == NULL) {
      if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"TGIFICON")) != NULL) {
         if (*c_ptr == DIR_SEP) {
            strcpy(s, c_ptr);
         } else {
            strcat(s, c_ptr);
         }
      } else {
         strcat(s, iconFileName);
      }
   } else if (((int)strlen(c_ptr)) >= 200) {
      /* too long, must be an error */
      strcat(s, iconFileName);
   } else if (*c_ptr == DIR_SEP) {
      strcpy(s, c_ptr);
   } else {
      strcat(s, c_ptr);
   }
   sprintf(ext_str, ".%s", OBJ_FILE_EXT);
   ext_len = strlen(ext_str);
   len = strlen(s);

   if (len < ext_len || strcmp(&s[len-ext_len],ext_str) != 0) {
      sprintf(&(s[len]), ".%s", OBJ_FILE_EXT);
   }
   if ((fp=fopen(s, "r")) == NULL) {
      /*
       * fprintf(stderr, "Warning:  Cannot open the tgif icon file '%s'.\n", s);
       */
      return;
   }

   strcpy(tmp_filefullpath, scanFileFullPath);
   strcpy(tmp_filename, scanFileName);
   tmp_linenum = scanLineNum;
   UtilStrCpyN(scanFileFullPath, sizeof(scanFileFullPath), s);
   strcpy(scanFileName, s);
   scanLineNum = 0;

   saved_tgif_obj = tgifObj;
   InitTgifObj();

   importingFile = TRUE; /* ignore the 'state' predicate */
   importingIconFile = TRUE; /* read the 'file_attr' predicate */
   readingPageNum = loadedCurPageNum = 0;
   foundGoodStateObject = FALSE;
   while ((read_status=ReadObj(fp, &obj_ptr)) == TRUE) {
      if (obj_ptr != NULL) {
         AddObj(NULL, topObj, obj_ptr);
         if (!seen_obj) {
            seen_obj = TRUE;
            ltx = obj_ptr->bbox.ltx; lty = obj_ptr->bbox.lty;
            rbx = obj_ptr->bbox.rbx; rby = obj_ptr->bbox.rby;
         } else {
            if (obj_ptr->bbox.ltx < ltx) ltx = obj_ptr->bbox.ltx;
            if (obj_ptr->bbox.lty < lty) lty = obj_ptr->bbox.lty;
            if (obj_ptr->bbox.rbx > rbx) rbx = obj_ptr->bbox.rbx;
            if (obj_ptr->bbox.rby > rby) rby = obj_ptr->bbox.rby;
         }
      }
   }
   strcpy(scanFileFullPath, tmp_filefullpath);
   strcpy(scanFileName, tmp_filename);
   scanLineNum = tmp_linenum;

   importingFile = FALSE;
   importingIconFile = FALSE;

   fclose(fp);

   if (read_status == INVALID) {
      sprintf(msg, TgLoadString(STID_ICON_FILEVER_TOO_LARGE), fileVersion);
      Msg(s);
      CleanUpTgifObj();
      tgifObj = saved_tgif_obj;
      return;
   }

   w = rbx - ltx;
   h = rby - lty;
   if (w > iconWindowW) {
      dx = -ltx;
      iconWindowW = w;
   } else {
      dx = -ltx+((iconWindowW-w)>>1);
   }
   if (h > iconWindowH) {
      dy = -lty;
      iconWindowH = h;
   } else {
      dy = -lty+((iconWindowH-h)>>1);
   }
   for (obj_ptr=topObj; obj_ptr != NULL; obj_ptr=obj_ptr->next) {
      MoveObj(obj_ptr, dx, dy);
   }
   iconTgifObj = tgifObj;
   tgifObj = saved_tgif_obj;

   iconTopObj = topObj;
   iconBotObj = botObj;
   curPage->top = curPage->bot = topObj = botObj = NULL;

   CleanUpPage();

   sizehints.x = 0;
   sizehints.y = 0;

   if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"IconGeometry")) != NULL) {
      int bitmask=XParseGeometry(c_ptr, &x, &y, &icon_w, &icon_h);

      if ((bitmask & XValue) && (bitmask & YValue)) {
         if (bitmask & XValue) sizehints.x = x;
         if (bitmask & YValue) sizehints.y = y;
         if (bitmask & XNegative) sizehints.x += DisplayWidth(mainDisplay,
               mainScreen) - iconWindowW - (brdrW<<1) - 1;
         if (bitmask & YNegative) sizehints.y += DisplayHeight(mainDisplay,
               mainScreen) - iconWindowH - (brdrW<<1) - 1;
     }
   }
   if ((iconBaseWindow = XCreateSimpleWindow(mainDisplay, rootWindow,
         sizehints.x, sizehints.y, iconWindowW+(brdrW<<1),
         iconWindowH+(brdrW<<1), brdrW, myBorderPixel, myBgPixel)) == 0) {
      FailToCreateWindowMessage("InitIcon()", NULL, TRUE);
   }
   if ((iconWindow = XCreateSimpleWindow(mainDisplay, iconBaseWindow, 0, 0,
         iconWindowW, iconWindowH, brdrW, myBorderPixel, myBgPixel)) == 0) {
      FailToCreateWindowMessage("InitIcon()", NULL, TRUE);
   }
   XStoreName(mainDisplay, iconBaseWindow, TOOL_NAME);

   XSelectInput(mainDisplay, iconBaseWindow, StructureNotifyMask |
         VisibilityChangeMask);
   if (((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"DoubleClickUnIconify")) !=
         NULL) && UtilStrICmp(c_ptr,"True") == 0) {
      XSelectInput(mainDisplay, iconWindow,
            ButtonPressMask | KeyPressMask | ExposureMask);
   } else {
      XSelectInput(mainDisplay, iconWindow, KeyPressMask | ExposureMask);
   }
   iconWindowCreated = TRUE;
}

void InitTitle()
{
   InitIcon();
}

void InitMenu()
{
   XGCValues values;
   char *c_ptr=NULL;

   values.foreground = myFgPixel;
   values.background = (threeDLook ? myLtGryPixel : myBgPixel);
   values.fill_style = FillSolid;
   /*
    * If (menuFontSet) is not NULL, it's okay to set values.font to
    *       whatever because it won't get used.
    */
   values.font = (menuFontPtr==NULL ? defaultFontPtr->fid : menuFontPtr->fid);
   textMenuGC = XCreateGC(mainDisplay, rootWindow,
         GCForeground | GCBackground | GCFillStyle | GCFont, &values);

   values.foreground = myBgPixel;
   values.background = myFgPixel;
   values.fill_style = FillStippled;
   rvPixmapMenuGC = XCreateGC(mainDisplay, rootWindow,
         GCForeground | GCBackground | GCFillStyle | GCFont, &values);

   InitMainMenu();

   BuildMenubarInfo();

   separatorHeight = (threeDLook ? SEPARATOR_PADDING+2 : SEPARATOR_PADDING+1);

   deleteCmdAsCut = FALSE;
   if (((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"DeleteCmdAsCut")) != NULL) &&
         UtilStrICmp(c_ptr,"true") == 0) {
      /* this X default is obsolete */
      deleteCmdAsCut = TRUE;
   }
   memset(&gstMenuDontSendCommandInfo, 0, sizeof(MenuDontSendCommandInfo));
}

void CleanUpMenu()
{
   struct ObjRec *saved_top_obj, *saved_bot_obj, *saved_tgif_obj;

   XFreeGC(mainDisplay, textMenuGC);
   XFreeGC(mainDisplay, rvPixmapMenuGC);
   CleanUpMainMenu();
   if (stackingWins != NULL) {
      free(stackingWins);
      stackingWins = NULL;
   }
   if (iconTgifObj != NULL) {
      saved_tgif_obj = tgifObj;
      tgifObj = iconTgifObj;
      CleanUpTgifObj();
      tgifObj = saved_tgif_obj;
   }
   if (iconTopObj != NULL) {
      saved_top_obj = topObj;
      saved_bot_obj = botObj;
      topObj = iconTopObj;
      botObj = iconBotObj;
      DelAllObj();
      topObj = saved_top_obj;
      botObj = saved_bot_obj;
   }
   if (check_bits == NULL) { }
   if (submenu_bits == NULL) { }

   if (!cmdLineTgrm2) {
      free(gpMenubarItemInfos);
      gpMenubarItemInfos = NULL;
      gnNumMenubarItems = 0;

      free(gpMainMenuItemInfos);
      gpMainMenuItemInfos = NULL;
      gnNumMainMenuItems = 0;
   }
}

void SaveDrawWinInfo()
{
   savedZoomScale = zoomScale;
   savedZoomedIn = zoomedIn;
   savedDrawOrigX = drawOrigX;
   savedDrawOrigY = drawOrigY;
   savedDrawWinW = drawWinW;
   savedDrawWinH = drawWinH;
   savedFileModified = fileModified;
}

void UnIconify()
{
   register int j, i;

   if (!iconWindowShown) return;

   iconWindowShown = FALSE;

   zoomScale = savedZoomScale;
   zoomedIn = savedZoomedIn;
   drawOrigX = savedDrawOrigX;
   drawOrigY = savedDrawOrigY;
   drawWinW = savedDrawWinW;
   drawWinH = savedDrawWinH;
   fileModified = savedFileModified;
   UpdDrawWinBBox();
   SetDefaultDrawWinClipRecs();

#ifdef notdef
   XUnmapWindow(mainDisplay, iconWindow);
#endif
   if (iconWindowCreated) XUnmapWindow(mainDisplay, iconBaseWindow);
   XMapWindow(mainDisplay, mainWindow);

   for (i=0; i < numExtraWins; i++) {
      if (extraWinInfo[i].raise && !extraWinInfo[i].mapped &&
            extraWinInfo[i].window != None) {
         XMapRaised(mainDisplay, extraWinInfo[i].window);
         extraWinInfo[i].mapped = TRUE;
      }
   }
   for (i=0; i < numStacking; i++) {
      for (j=0; j < numExtraWins; j++) {
         if (extraWinInfo[j].raise && extraWinInfo[j].window==stackingWins[i]) {
            extraWinInfo[j].mapped = TRUE;
            break;
         }
      }
      XMapRaised(mainDisplay, stackingWins[i]);
   }
   XFlush(mainDisplay);
   XSync(mainDisplay, False);
}

void Iconify()
{
   register int i;

   if (iconWindowShown) return;

   iconWindowShown = TRUE;

   SaveDrawWinInfo();
   zoomScale = 0;
   zoomedIn = FALSE;
   drawOrigX = 0;
   drawOrigY = 0;
   drawWinW = iconWindowW;
   drawWinH = iconWindowH;
   UpdDrawWinBBox();
   SetDefaultIconWinClipRecs();

   justIconified = TRUE;

#ifdef notdef
   XUnmapWindow(mainDisplay, mainWindow);
#endif

   SaveStackingOrder();

   if (pinnedMainMenu) XUnmapWindow(mainDisplay, mainMenuWindow);
   for (i = 0; i < numExtraWins; i++) {
      if (extraWinInfo[i].raise && extraWinInfo[i].mapped &&
            extraWinInfo[i].window != None) {
         XUnmapWindow(mainDisplay, extraWinInfo[i].window);
         extraWinInfo[i].mapped = FALSE;
      }
   }
   if (iconWindowCreated) {
      XMapWindow(mainDisplay, iconBaseWindow);
      XMapWindow(mainDisplay, iconWindow);
   }
}

static int iconJustClicked=FALSE;
static Time iconClickTime;

void IconEventHandler(input)
   XEvent *input;
{
   XEvent ev;
   Time click_time;

   if (input->xany.window == iconWindow && input->type == ButtonPress) {
      XButtonEvent *button_ev=(&(input->xbutton));

      if (iconWindowShown && !justIconified && button_ev->button == Button2 &&
            (button_ev->state & (ShiftMask | ControlMask))) {
         justIconified = TRUE;
         RedrawIconWindow();
      } else {
         click_time = input->xbutton.time;
         if (iconJustClicked &&
               (click_time-iconClickTime)<doubleClickInterval) {
            iconJustClicked = FALSE;
            UnIconify();
         } else {
            iconJustClicked = TRUE;
            iconClickTime = click_time;
         }
      }
   } else if (input->xany.window==iconBaseWindow && input->type==UnmapNotify) {
      UnIconify();
   } else if (input->xany.window==iconBaseWindow && input->type==MapNotify) {
      Iconify();
/*
   } else if (input->xany.window==iconBaseWindow && (input->type==MapNotify ||
         input->type == VisibilityNotify && input->xvisibility.state ==
         VisibilityUnobscured)) {
      Iconify();
 */
   } else if (input->xany.window == iconWindow && input->type == Expose) {
      if (!iconWindowShown) return;

      while (XCheckWindowEvent(mainDisplay, iconWindow, ExposureMask, &ev)) ;
      while (XCheckWindowEvent(mainDisplay, iconBaseWindow,
            StructureNotifyMask, &ev)) ;
      RedrawIconWindow();
   }
}

void TitleEventHandler(input)
   XEvent *input;
{
   XEvent ev;

   if (input->type == Expose) {
      XSync(mainDisplay, False);
      while (XCheckWindowEvent(mainDisplay, titleWindow, ExposureMask, &ev)) ;
      RedrawTitleWindow();
   } else if (input->type == EnterNotify) {
      SetMouseStatus(TgLoadCachedString(CSTID_PARANED_NONE),
            TgLoadCachedString(CSTID_MAIN_MENU),
            TgLoadCachedString(CSTID_MAIN_MENU));
   } else if (input->type == ButtonPress && (input->xbutton.button == Button2 ||
         input->xbutton.button == Button3)) {
      MainMenu();
   }
}

void CalcMenubarWindowHeight()
   /* Given menubarWindowW, set manubarWindowH to fit everything */
{
   int i, x=0, w=0, h=0, gap=0, padding=(windowPadding>>1);

   BuildMenubarInfo();

   if (menuFontSet != NULL || menuFontPtr != NULL) {
      x = menuFontWidth;
      h = menuFontHeight;
      gap = (x<<1);
      x += padding;
      h += padding;
      for (i=0; i < gnNumMenubarItems; i++) {
         w = MenuTextWidth(menuFontPtr, _(gpMenubarItemInfos[i].menu_str),
               strlen(_(gpMenubarItemInfos[i].menu_str)));
         if ((!noMinWinSize || !gnMinimalMenubar || gnAutoWrapMenubar) &&
               x+w+padding >= menubarWindowW) {
            x = menuFontWidth+padding;
            h += menuFontHeight+padding;
         }
         x += w+gap+padding;
      }
      x += (padding<<1);
      h += (padding<<1);
   } else {
      x = 2;
      h = initialMenubarWindowH;
      gap = defaultFontWidth+(defaultFontWidth>>1);
      x += padding;
      h += padding;
      for (i=0; i < gnNumMenubarItems; i++) {
         w = defaultFontWidth*strlen(_(gpMenubarItemInfos[i].menu_str));
         if ((!noMinWinSize || !gnMinimalMenubar || gnAutoWrapMenubar) &&
               x+w+padding >= menubarWindowW) {
            x = 2+padding;
            h += initialMenubarWindowH+padding;
         }
         x += w+gap+padding;
      }
      x += (padding<<1);
      h += (padding<<1);
   }
   menubarWindowH = h;
}

static
void HighLightMenubarString(item_str, bbox, highlight)
   char *item_str;
   struct BBRec *bbox;
   int highlight;
   /* HighLightMenubarString() will use _(item_str) internally */
{
   if (threeDLook) {
      struct BBRec real_bbox;

      real_bbox.ltx = bbox->ltx-2;
      real_bbox.lty = bbox->lty;
      real_bbox.rbx = bbox->rbx+2;
      real_bbox.rby = bbox->rby+1;
      if (highlight) {
         TgDrawThreeDButton(mainDisplay, menubarWindow, textMenuGC, &real_bbox,
               TGBS_RAISED, 1, FALSE);
      } else {
         TgClearThreeDButton(mainDisplay, menubarWindow, textMenuGC, &real_bbox,
               1);
      }
   } else {
      int fg_pixel=(highlight ? myBgPixel : myFgPixel);
      int bg_pixel=(highlight ? myFgPixel :
            (threeDLook?myLtGryPixel:myBgPixel));

      XSetForeground(mainDisplay, textMenuGC, bg_pixel);
      XFillRectangle(mainDisplay, menubarWindow, textMenuGC,
            bbox->ltx-2, bbox->lty, bbox->rbx-bbox->ltx+4, bbox->rby-bbox->lty);
      XSetForeground(mainDisplay, textMenuGC, fg_pixel);
      if (menuFontSet != NULL || menuFontPtr != NULL) {
         DrawMenuString(mainDisplay, menubarWindow, textMenuGC,
               bbox->ltx+(menuFontWidth>>1), menuFontAsc+bbox->lty,
               _(item_str), strlen(_(item_str)));
      } else {
         DrawMenuString(mainDisplay, menubarWindow, textMenuGC, bbox->ltx,
               defaultFontAsc+bbox->lty, _(item_str), strlen(_(item_str)));
      }
   }
}

void RedrawMenubarWindow()
{
   int i, x=0, y=0, w=0, len=0, gap=0, padding=(windowPadding>>1);
   struct BBRec bbox;

   XClearWindow(mainDisplay, menubarWindow);

   XSetForeground(mainDisplay, textMenuGC, myFgPixel);
   if (menuFontSet != NULL || menuFontPtr != NULL) {
      x = menuFontWidth;
      y = menuFontAsc;
      gap = (x<<1);
      x += padding;
      y += padding;

      for (i=0; i < gnNumMenubarItems; i++) {
         len = strlen(_(gpMenubarItemInfos[i].menu_str));
         w = MenuTextWidth(menuFontPtr, _(gpMenubarItemInfos[i].menu_str), len);
         if ((!noMinWinSize || !gnMinimalMenubar || gnAutoWrapMenubar) &&
               x+w+padding >= menubarWindowW) {
            x = menuFontWidth+padding;
            y += menuFontHeight+padding;
         }
         if (!colorDisplay && gpMenubarItemInfos[i].cmdid == MENU_COLOR) {
            /* disable color menu -- at this point, threeDLook must be FALSE */
            DrawMenuString(mainDisplay, menubarWindow, revGrayGC, x+padding,
                  y+padding, _(gpMenubarItemInfos[i].menu_str), len);
         } else {
            DrawMenuString(mainDisplay, menubarWindow, textMenuGC, x+padding,
                  y+padding, _(gpMenubarItemInfos[i].menu_str), len);
         }
         x += w+gap+padding;
      }
   } else {
      x = 2;
      y = defaultFontAsc;
      gap = defaultFontWidth+(defaultFontWidth>>1);
      x += padding;
      y += padding;

      for (i=0; i < gnNumMenubarItems; i++) {
         len = strlen(_(gpMenubarItemInfos[i].menu_str));
         w = defaultFontWidth*len;
         if ((!noMinWinSize || !gnMinimalMenubar || gnAutoWrapMenubar) &&
               x+w+padding >= menubarWindowW) {
            x = 2+padding;
            y += initialMenubarWindowH+padding;
         }
         if (!colorDisplay && gpMenubarItemInfos[i].cmdid == MENU_COLOR) {
            /* disable color menu -- at this point, threeDLook must be FALSE */
            DrawMenuString(mainDisplay, menubarWindow, revGrayGC, x+padding,
                  y+padding, _(gpMenubarItemInfos[i].menu_str), len);
         } else {
            DrawMenuString(mainDisplay, menubarWindow, textMenuGC, x+padding,
                  y+padding, _(gpMenubarItemInfos[i].menu_str), len);
         }
         x += w+gap+padding;
      }
   }
   if (threeDLook) {
      bbox.ltx = 0;
      bbox.lty = 0;
      bbox.rbx = menubarWindowW;
      bbox.rby = menubarWindowH;
      TgDrawThreeDButton(mainDisplay, menubarWindow, textMenuGC, &bbox,
            TGBS_RAISED, 1, FALSE);
   }
   if (excludeMenubarIndex != INVALID) {
      struct BBRec text_bbox;
      int exclude_gap=((menuFontSet==NULL && menuFontPtr==NULL) ?
            (defaultFontWidth<<1) : (menuFontWidth<<1));

      SetBBRec(&text_bbox,
            excludeMenubarWinBBox.ltx+2+windowPadding,
            excludeMenubarWinBBox.lty+2+windowPadding,
            excludeMenubarWinBBox.rbx-2-windowPadding-exclude_gap,
            excludeMenubarWinBBox.rby-2-windowPadding);
      /* HighLightMenubarString() doesn't need _() */
      HighLightMenubarString(gpMenubarItemInfos[excludeMenubarIndex].menu_str,
            &text_bbox, TRUE);
   }
}

static
int PullDownFromMenubar(index, x, y, text_bbox)
   int index, x, y;
   struct BBRec *text_bbox;
{
   int rc=BAD;
   int exclude_gap=((menuFontSet==NULL && menuFontPtr==NULL) ?
         (defaultFontWidth<<1) : (menuFontWidth<<1));

   while (rc == BAD) {
      if (index != INVALID) {
         /* HighLightMenubarString() doesn't need _() */
         HighLightMenubarString(gpMenubarItemInfos[index].menu_str, text_bbox,
               TRUE);
         excludeMenubarWinBBox.ltx = text_bbox->ltx-2-windowPadding;
         excludeMenubarWinBBox.lty = text_bbox->lty-2-windowPadding;
         excludeMenubarWinBBox.rbx = text_bbox->rbx+exclude_gap+2+windowPadding;
         excludeMenubarWinBBox.rby = text_bbox->rby+2+windowPadding;
         excludeMenubarIndex = index;
      }
      switch (gpMenubarItemInfos[index].cmdid) {
      case MENU_FILE: rc = FileMenu(x, y, TRUE); break;
      case MENU_EDIT: rc = EditMenu(x, y, TRUE); break;
      case MENU_LAYOUT: rc = LayoutMenu(x, y, TRUE); break;
      case MENU_ARRANGE: rc = ArrangeMenu(x, y, TRUE); break;
      case MENU_PROPERTIES: rc = PropertiesMenu(x, y, TRUE); break;
      case MENU_MOVEMODE: rc = MoveModeMenu(x, y, TRUE); break;
      case MENU_PAGE: rc = PageMenu(x, y, TRUE); break;
      case MENU_PAGELAYOUT: rc = PageLayoutMenu(x, y, TRUE); break;
      case MENU_HORIALIGN: rc = HoriAlignMenu(x, y, TRUE); break;
      case MENU_VERTALIGN: rc = VertAlignMenu(x, y, TRUE); break;
      case MENU_FONT: rc = FontMenu(x, y, TRUE); break;
      case MENU_STYLE: rc = StyleMenu(x, y, TRUE); break;
      case MENU_SIZE: rc = SizeMenu(x, y, TRUE); break;
      case MENU_SHAPE: rc = ShapeMenu(x, y, TRUE); break;
      case MENU_STRETCHTEXT: rc = StretchableTextModeMenu(x, y, TRUE); break;
      case MENU_LINEDASH: rc = LineDashMenu(x, y, TRUE); break;
      case MENU_LINESTYLE: rc = LineStyleMenu(x, y, TRUE); break;
      case MENU_LINETYPE: rc = LineTypeMenu(x, y, TRUE); break;
      case MENU_LINEWIDTH: rc = LineWidthMenu(x, y, TRUE); break;
      case MENU_FILL: rc = FillMenu(x, y, TRUE); break;
      case MENU_PEN: rc = PenMenu(x, y, TRUE); break;
      case MENU_TRANSPAT: rc = TransPatModeMenu(x, y, TRUE); break;
      case MENU_COLOR: rc = ColorMenu(x, y, TRUE); break;
      case MENU_IMAGEPROC: rc = ImageProcMenu(x, y, TRUE); break;
      case MENU_NAVIGATE: rc = NavigateMenu(x, y, TRUE); break;
      case MENU_SPECIAL: rc = SpecialMenu(x, y, TRUE); break;
      case MENU_HELP: rc = HelpMenu(x, y, TRUE); break;
      case MENU_TANGRAM2:
         if (cmdLineTgrm2) {
            rc = Tangram2Menu(x, y, TRUE);
         }
         break;
      }
      if (index != INVALID) {
         /* HighLightMenubarString() doesn't need _() */
         HighLightMenubarString(gpMenubarItemInfos[index].menu_str, text_bbox,
               FALSE);
      }
      if (rc == BAD) {
         Window root_win, child_win;
         int mouse_x, mouse_y, root_x, root_y;
         unsigned int status;

         XQueryPointer(mainDisplay, menubarWindow, &root_win, &child_win,
               &root_x, &root_y, &mouse_x, &mouse_y, &status);
         index = WhichMenubarItem(mouse_x, mouse_y, &x, &y, text_bbox);
         if (!(status & BUTTONSMASK) && index == (-1)) {
            return INVALID;
         }
      } else if (rc == (-4)) {
         return INVALID;
      } else if (index != MENU_FILE) {
         return INVALID;
      }
   }
   return rc;
}

static int curRaisedMenuItem=INVALID;

int MenubarEventHandler(input)
   XEvent *input;
{
   XEvent ev;
   int rc=INVALID;

   if (input->type == Expose) {
      XSync(mainDisplay, False);
      while (XCheckWindowEvent(mainDisplay,menubarWindow,ExposureMask,&ev)) ;
      RedrawMenubarWindow();
   } else if (input->type == EnterNotify || input->type == LeaveNotify) {
      SetMouseStatus("", "", "");
      if (curRaisedMenuItem != INVALID) {
         struct BBRec text_bbox;

         GetMenubarItemInfo(curRaisedMenuItem, NULL, NULL, &text_bbox);
         /* HighLightMenubarString() doesn't need _() */
         HighLightMenubarString(gpMenubarItemInfos[curRaisedMenuItem].menu_str,
               &text_bbox, FALSE);
         curRaisedMenuItem = INVALID;
      }
   } else if (input->type == MotionNotify) {
      int index;

      index = WhichMenubarItem(input->xmotion.x, input->xmotion.y,
            NULL, NULL, NULL);
      if (index == INVALID) {
         SetMouseStatusToAllNone();
      } else {
         SetMouseStatus("", _(gpMenubarItemInfos[index].status_str), "");
      }
      if (threeDLook) {
         if (index != curRaisedMenuItem) {
            struct BBRec text_bbox;

            if (curRaisedMenuItem != INVALID) {
               GetMenubarItemInfo(curRaisedMenuItem, NULL, NULL, &text_bbox);
               /* HighLightMenubarString() doesn't need _() */
               HighLightMenubarString(
                     gpMenubarItemInfos[curRaisedMenuItem].menu_str,
                     &text_bbox, FALSE);
               curRaisedMenuItem = INVALID;
            }
            if (index != INVALID) {
               GetMenubarItemInfo(index, NULL, NULL, &text_bbox);
               /* HighLightMenubarString() doesn't need _() */
               HighLightMenubarString(gpMenubarItemInfos[index].menu_str,
                     &text_bbox, TRUE);
               curRaisedMenuItem = index;
            }
         }
      }
      XSync(mainDisplay, False);
      while (XCheckWindowEvent(mainDisplay, menubarWindow, PointerMotionMask,
            &ev)) ;
   } else if (input->type == ButtonPress) {
      int win_x, win_y, index;
      struct BBRec text_bbox;

      index = WhichMenubarItem(input->xbutton.x, input->xbutton.y,
            &win_x, &win_y, &text_bbox);
      if (index == INVALID) {
         SetMouseStatusToAllNone();
      } else {
         SaveStatusStrings();
         rc = PullDownFromMenubar(index, win_x, win_y, &text_bbox);
         RestoreStatusStrings();
         SetMouseStatus(NULL, NULL, NULL);
         SetBBRec(&excludeMenubarWinBBox, -1, -1, -1, -1);
         excludeMenubarIndex = INVALID;
      }
      if (threeDLook) {
         if (index != curRaisedMenuItem) {
            struct BBRec text_bbox;

            if (curRaisedMenuItem != INVALID) {
               GetMenubarItemInfo(curRaisedMenuItem, NULL, NULL, &text_bbox);
               /* HighLightMenubarString() doesn't need _() */
               HighLightMenubarString(
                     gpMenubarItemInfos[curRaisedMenuItem].menu_str,
                     &text_bbox, FALSE);
               curRaisedMenuItem = INVALID;
            }
            GetMenubarItemInfo(index, NULL, NULL, &text_bbox);
            /* HighLightMenubarString() doesn't need _() */
            HighLightMenubarString(gpMenubarItemInfos[index].menu_str,
                  &text_bbox, TRUE);
            curRaisedMenuItem = index;
         }
      }
      XSync(mainDisplay, False);
      while (XCheckWindowEvent(mainDisplay, menubarWindow, ButtonPressMask,
            &ev)) ;
   }
   return rc;
}
