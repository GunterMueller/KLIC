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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/tgcwdl.c,v 1.21 2011/05/16 16:22:00 william Exp $
 */

#define _INCLUDE_FROM_TGCWDL_C_

#include "tgifdefs.h"
#include "tidget.h"

#include "auxtext.e"
#include "button.e"
#include "color.e"
#include "cursor.e"
#include "dialog.e"
#include "file.e"
#include "font.e"
#include "imgproc.e"
#include "mainloop.e"
#include "msg.e"
#include "raster.e"
#include "rect.e"
#include "setup.e"
#include "strtbl.e"
#include "tdgtbase.e"
#include "tdgtbmpl.e"
#include "tdgtbrow.e"
#include "tdgtbtn.e"
#include "tdgtdraw.e"
#include "tdgtsedt.e"
#include "tdgtlist.e"
#include "tdgtmsg.e"
#include "tidget.e"
#include "tgcwdl.e"
#include "util.e"
#include "xbitmap.e"
#include "xpixmap.e"

ColorWheelInfo gColorWheelInfo;

typedef struct tagColorWheelDlgInfo {
   Window parent_win; /* should be the rootWindow */

   TdgtDraw *hs_pixmap_ctl; /* user pick HS values here */
   TdgtDraw *v_pixmap_ctl; /* user pick V value here */
   TdgtMsg *r_msg_ctl, *g_msg_ctl, *b_msg_ctl;
   TdgtMsg *h_msg_ctl, *s_msg_ctl, *v_msg_ctl;
   TdgtMsg *cname_msg_ctl;
   TdgtSmplEdit *r_edit_ctl, *g_edit_ctl, *b_edit_ctl;
   TdgtSmplEdit *h_edit_ctl, *s_edit_ctl, *v_edit_ctl;
   TdgtSmplEdit *cname_edit_ctl;
   TdgtDraw *csmpl_ctl; /* display selected color here */
   TdgtBtnRow *btn_row_ctl;

   int screen_w, screen_h;
   int base_win_w, base_win_h;

   int hs_x, hs_y, v_y;

   int r_msg_x, r_msg_y, r_msg_w, r_msg_h;
   int g_msg_x, g_msg_y, g_msg_w, g_msg_h;
   int b_msg_x, b_msg_y, b_msg_w, b_msg_h;
   int r_ctl_x, r_ctl_y, r_ctl_w, r_ctl_h;
   int g_ctl_x, g_ctl_y, g_ctl_w, g_ctl_h;
   int b_ctl_x, b_ctl_y, b_ctl_w, b_ctl_h;

   int h_msg_x, h_msg_y, h_msg_w, h_msg_h;
   int s_msg_x, s_msg_y, s_msg_w, s_msg_h;
   int v_msg_x, v_msg_y, v_msg_w, v_msg_h;
   int h_ctl_x, h_ctl_y, h_ctl_w, h_ctl_h;
   int s_ctl_x, s_ctl_y, s_ctl_w, s_ctl_h;
   int v_ctl_x, v_ctl_y, v_ctl_w, v_ctl_h;

   int btn_row_x, btn_row_y, btn_row_w, btn_row_h;

   char *title_str, *formatted_title_str;
   int def_btn_id;

   int hs_pixmap_x, hs_pixmap_y, v_pixmap_x, v_pixmap_y;

   int red_msg_x, red_msg_y, red_edit_x, red_edit_y;
   int green_msg_x, green_msg_y, green_edit_x, green_edit_y;
   int blue_msg_x, blue_msg_y, blue_edit_x, blue_edit_y;
   int hue_msg_x, hue_msg_y, hue_edit_x, hue_edit_y;
   int sat_msg_x, sat_msg_y, sat_edit_x, sat_edit_y;
   int val_msg_x, val_msg_y, val_edit_x, val_edit_y;

   int cname_msg_x, cname_msg_y, cname_edit_x, cname_edit_y;
   int csmpl_msg_x, csmpl_msg_y, csmpl_edit_x, csmpl_edit_y;

   unsigned char r, g, b;
   int h, s, v; /* 0 <= h,s,v <= 255 */
   int pixel;
   char color_str[40];

   TrueColorInfo tci;
} ColorWheelDlgInfo;

/* --------------------- Utility Functions --------------------- */

#define TDGTARG_COLORHWEEL_DPY_CLEAR 1 /* pv_cmd_userdata is NULL */
#define TDGTARG_COLORHWEEL_DPY_APPEND 2 /* pv_cmd_userdata is (ColorWheelDpyAppendInfo*) */

static
void ClearAllControls(pTdgtBase)
   TdgtBase *pTdgtBase;
{
   ColorWheelDlgInfo *pcwdi=(ColorWheelDlgInfo*)(pTdgtBase->pti->userdata);

   TidgetSendCmd(pcwdi->hs_pixmap_ctl->pti, TDGTCMD_DRAW_USER,
         TDGTARG_COLORHWEEL_DPY_CLEAR, pTdgtBase);
   TidgetSendCmd(pcwdi->v_pixmap_ctl->pti, TDGTCMD_DRAW_USER,
         TDGTARG_COLORHWEEL_DPY_CLEAR, pTdgtBase);
   TidgetSendCmd(pcwdi->csmpl_ctl->pti, TDGTCMD_DRAW_USER,
         TDGTARG_COLORHWEEL_DPY_CLEAR, pTdgtBase);

   TdgtSmplEditSetText(pcwdi->r_edit_ctl, "");
   TdgtSmplEditSetText(pcwdi->g_edit_ctl, "");
   TdgtSmplEditSetText(pcwdi->b_edit_ctl, "");
   TdgtSmplEditSetText(pcwdi->h_edit_ctl, "");
   TdgtSmplEditSetText(pcwdi->s_edit_ctl, "");
   TdgtSmplEditSetText(pcwdi->v_edit_ctl, "");
   TdgtSmplEditSetText(pcwdi->cname_edit_ctl, "");
}

static
int RGBtoPixel(pcwdi, r, g, b)
   ColorWheelDlgInfo *pcwdi;
   unsigned int r, g, b;
{
   return (((r << pcwdi->tci.r_shift) & mainVisual->red_mask) |
      ((g << pcwdi->tci.g_shift) & mainVisual->green_mask) |
      ((b << pcwdi->tci.b_shift) & mainVisual->blue_mask));
}

static
void PixeltoRGB(pcwdi, pixel, pn_r, pn_g, pn_b)
   ColorWheelDlgInfo *pcwdi;
   int pixel;
   unsigned int *pn_r, *pn_g, *pn_b;
   /* 0 <= r,g,b <= 255 */
{
   uint32_t pix=(uint32_t)(unsigned int)pixel;
   unsigned int r=0, g=0, b=0;
   double dr=(double)0, dg=(double)0, db=(double)0;

   r = (pix & pcwdi->tci.r_mask) >> pcwdi->tci.r_shift;
   g = (pix & pcwdi->tci.g_mask) >> pcwdi->tci.g_shift;
   b = (pix & pcwdi->tci.b_mask) >> pcwdi->tci.b_shift;
   dr = ((double)r) / pcwdi->tci.dr_maxval * ((double)255);
   dg = ((double)g) / pcwdi->tci.dg_maxval * ((double)255);
   db = ((double)b) / pcwdi->tci.db_maxval * ((double)255);
   r = (dr < ((double)0)) ? 0 : ((unsigned int)dr);
   g = (dg < ((double)0)) ? 0 : ((unsigned int)dg);
   b = (db < ((double)0)) ? 0 : ((unsigned int)db);
   if (r > 255) r = 255;
   if (g > 255) g = 255;
   if (b > 255) b = 255;
   *pn_r = r;
   *pn_g = g;
   *pn_b = b;
}

static
void HStoXY(pcwdi, h, s, pn_x, pn_y)
   ColorWheelDlgInfo *pcwdi;
   int h, s, *pn_x, *pn_y;
{
   double dval=((double)h)/((double)255)*((double)(HS_WIDTH-1));
   int x=0, y=0;

   x = round(dval);
   if (x < 0) x = 0;
   if (x > (HS_WIDTH-1)) x = HS_WIDTH-1;

   dval = ((double)s)/((double)255)*((double)(HS_HEIGHT-1));
   y = round(dval);
   if (y < 0) y = 0;
   if (y > (HS_HEIGHT-1)) y = HS_HEIGHT-1;

   *pn_x = x;
   *pn_y = HS_HEIGHT-y;
}

static
void XYtoHS(pcwdi, x, y, pn_h, pn_s)
   ColorWheelDlgInfo *pcwdi;
   int x, y, *pn_h, *pn_s;
{
   double dval=((double)x)/((double)(HS_WIDTH-1))*((double)(255));
   int h=0, s=0;

   h = round(dval);
   if (h < 0) h = 0;
   if (h > 255) h = 255;

   dval = ((double)(HS_HEIGHT-y))/((double)(HS_HEIGHT-1))*((double)255);
   s = round(dval);
   if (s < 0) s = 0;
   if (s > 255) s = 255;

   *pn_h = h;
   *pn_s = s;
}

static
void VtoY(pcwdi, v, pn_y)
   ColorWheelDlgInfo *pcwdi;
   int v, *pn_y;
{
   double dval=((double)v)/((double)255)*((double)(HS_HEIGHT-1));
   int y=round(dval);

   if (y < 0) y = 0;
   if (y > (HS_HEIGHT-1)) y = HS_HEIGHT-1;

   *pn_y = HS_HEIGHT-y;
}

static
void YtoV(pcwdi, y, pn_v)
   ColorWheelDlgInfo *pcwdi;
   int y, *pn_v;
{
   double dval=((double)(HS_HEIGHT-y))/((double)(HS_HEIGHT-1))*((double)255);
   int v=0;

   v = round(dval);
   if (v < 0) v = 0;
   if (v > 255) v = 255;

   *pn_v = v;
}

static
int SetupVPixmap(pcwdi)
   ColorWheelDlgInfo *pcwdi;
{
   int row=0;
   Pixmap pixmap=gColorWheelInfo.v_pixmap;
   double dh=(((double)pcwdi->h)/((double)255)*((double)360));
   double ds=(((double)pcwdi->s)/((double)255)*((double)0x0ffff));
   int hue=round(dh);

   if (hue < 0) hue = 0;
   if (hue > 360) hue = 360;

   if (pixmap == None) return FALSE;

   for (row=0; row < V_HEIGHT; row++) {
      int r=0, g=0, b=0;
      double dr=(double)0, dg=(double)0, db=(double)0;
      uint32_t pixel=0;
      double val=(((double)(V_HEIGHT-1-row))/((double)(V_HEIGHT-1)));
      XPoint vs[2];

      val *= ((double)0x0ffff);

      HSVtoRGB(hue, ds, val, &r, &g, &b);
      dr = ((double)r) / ((double)0x0ffff) * pcwdi->tci.dr_maxval;
      dg = ((double)g) / ((double)0x0ffff) * pcwdi->tci.dr_maxval;
      db = ((double)b) / ((double)0x0ffff) * pcwdi->tci.dr_maxval;
      r = round(dr);
      g = round(dg);
      b = round(db);
      if (r < 0) r = 0;
      if (g < 0) g = 0;
      if (b < 0) b = 0;
      if (r > 255) r = 255;
      if (g > 255) g = 255;
      if (b > 255) b = 255;
      pixel = RGBtoPixel(pcwdi, r, g, b);
      XSetForeground(mainDisplay, gTidgetManager.gc, pixel);

      vs[0].x = 0;
      vs[0].y = row;
      vs[1].x = V_WIDTH-1;
      vs[1].y = row;
      XDrawLines(mainDisplay, pixmap, gTidgetManager.gc, vs, 2,
            CoordModeOrigin);
   }
   TidgetManagerResetGC();

   return TRUE;
}

static
void SetHSVFromRGB(pcwdi)
   ColorWheelDlgInfo *pcwdi;
{
   unsigned int r=pcwdi->r, g=pcwdi->g, b=pcwdi->b;
   int h=0, s=0, v=0;
   double dr=(double)0, dg=(double)0, db=(double)0;
   double dh=(double)0, ds=(double)0, dv=(double)0;

   dr = ((double)r) / ((double)255) * ((double)0x0ffff);
   dg = ((double)g) / ((double)255) * ((double)0x0ffff);
   db = ((double)b) / ((double)255) * ((double)0x0ffff);
   r = round(dr);
   g = round(dg);
   b = round(db);
   RGBtoHSV(r, g, b, &h, &ds, &v);
   dh = ((double)h) / ((double)360) * ((double)255);
   ds = ds / ((double)0x0ffff) * ((double)255);
   dv = ((double)v) / ((double)0x0ffff) * ((double)255);
   h = round(dh);
   s = round(ds);
   v = round(dv);
   if (h < 0) h = 0;
   if (s < 0) s = 0;
   if (v < 0) v = 0;
   if (h > 255) h = 255;
   if (s > 255) s = 255;
   if (v > 255) v = 255;
   pcwdi->h = h;
   pcwdi->s = s;
   pcwdi->v = v;
}

static
void SetRGBFromHSV(pcwdi)
   ColorWheelDlgInfo *pcwdi;
{
   double dh=(((double)pcwdi->h)/((double)255)*((double)360));
   double ds=(((double)pcwdi->s)/((double)255)*((double)0x0ffff));
   double dv=(((double)pcwdi->v)/((double)255)*((double)0x0ffff));
   double dr=(double)0, dg=(double)0, db=(double)0;
   int hue=round(dh), val=round(dv), r=0, g=0, b=0;

   if (hue < 0) hue = 0;
   if (hue > 360) hue = 360;

   if (val < 0) val = 0;
   if (val > 0x0ffff) val = 0x0ffff;

   HSVtoRGB(hue, ds, val, &r, &g, &b);
   dr = ((double)r) / ((double)0x0ffff) * pcwdi->tci.dr_maxval;
   dg = ((double)g) / ((double)0x0ffff) * pcwdi->tci.dr_maxval;
   db = ((double)b) / ((double)0x0ffff) * pcwdi->tci.dr_maxval;
   r = round(dr);
   g = round(dg);
   b = round(db);
   if (r < 0) r = 0;
   if (g < 0) g = 0;
   if (b < 0) b = 0;
   if (r > 255) r = 255;
   if (g > 255) g = 255;
   if (b > 255) b = 255;
   pcwdi->r = r;
   pcwdi->g = g;
   pcwdi->b = b;
}

static
void SetColorNameFromRGB(pcwdi)
   ColorWheelDlgInfo *pcwdi;
{
   sprintf(pcwdi->color_str, "%02x%02x%02x", pcwdi->r, pcwdi->g, pcwdi->b);
   UtilStrUpper(pcwdi->color_str);
}

static
void SetPixelFromRGB(pcwdi)
   ColorWheelDlgInfo *pcwdi;
{
   pcwdi->pixel = RGBtoPixel(pcwdi, pcwdi->r, pcwdi->g, pcwdi->b);
}

static
int SetupColorsFromCurrentColor(pcwdi)
   ColorWheelDlgInfo *pcwdi;
{
   PixeltoRGB(pcwdi, colorPixels[colorIndex], &pcwdi->r, &pcwdi->g, &pcwdi->b);
   SetPixelFromRGB(pcwdi);
   SetHSVFromRGB(pcwdi);
   HStoXY(pcwdi, pcwdi->h, pcwdi->s, &pcwdi->hs_x, &pcwdi->hs_y);
   VtoY(pcwdi, pcwdi->v, &pcwdi->v_y);
   SetupVPixmap(pcwdi);
   SetColorNameFromRGB(pcwdi);

   return TRUE;
}

static
int AllHexChars(buf)
   char *buf;
{
   int i=0, len=strlen(buf);

   for (i=0; i < len; i++) {
      char ch=buf[i];

      if ((ch >= '0' && ch <= '9') || (ch >= 'a' && ch <= 'f') ||
            (ch >= 'A' && ch <= 'F')) {
         /* keep going */
      } else {
         return FALSE;
      }
   }
   return TRUE;
}

static
int SetColorInAppl(pTdgtBase)
   TdgtBase *pTdgtBase;
{
   ColorWheelDlgInfo *pcwdi=(ColorWheelDlgInfo*)(pTdgtBase->pti->userdata);
   char *c_ptr=TdgtSmplEditGetText(pcwdi->cname_edit_ctl), color_str[40];
   int new_alloc=FALSE, index=0;

   if (strlen(c_ptr) == 6 && AllHexChars(c_ptr)) {
      snprintf(color_str, sizeof(color_str), "#%s", c_ptr);
   } else {
      UtilStrCpyN(color_str, sizeof(color_str), c_ptr);
   }
   index = QuickFindColorIndex(NULL, color_str, &new_alloc, FALSE);
   if (index == INVALID) {
      sprintf(gszMsgBox, TgLoadString(STID_FAIL_ALLOC_NAMED_COLOR), c_ptr);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return FALSE;
   } else {
      ChangeAllSelColor(index, TRUE);
   }
   return TRUE;
}

/* --------------------- Focus Stuff --------------------- */

static
void ColorWheelDlgRemoveFocus(pTdgtBase)
   TdgtBase *pTdgtBase;
{
   ColorWheelDlgInfo *pcwdi=(ColorWheelDlgInfo*)(pTdgtBase->pti->userdata);
   Window win=TidgetGetFocusWindow();

   if (win == None) return;
   if (win == pcwdi->hs_pixmap_ctl->pti->tci.win) {
      TidgetDrawFocusRect(pcwdi->hs_pixmap_ctl->pti,
            &pcwdi->hs_pixmap_ctl->client_area);
   } else if (win == pcwdi->v_pixmap_ctl->pti->tci.win) {
      TidgetDrawFocusRect(pcwdi->v_pixmap_ctl->pti,
            &pcwdi->v_pixmap_ctl->client_area);
   } else if (win == pcwdi->r_edit_ctl->pti->tci.win) {
      TidgetDrawFocusRect(pcwdi->r_edit_ctl->pti,
            &pcwdi->r_edit_ctl->client_area);
      TdgtSmplEditSetFocus(pcwdi->r_edit_ctl, FALSE);
   } else if (win == pcwdi->g_edit_ctl->pti->tci.win) {
      TidgetDrawFocusRect(pcwdi->g_edit_ctl->pti,
            &pcwdi->g_edit_ctl->client_area);
      TdgtSmplEditSetFocus(pcwdi->g_edit_ctl, FALSE);
   } else if (win == pcwdi->b_edit_ctl->pti->tci.win) {
      TidgetDrawFocusRect(pcwdi->b_edit_ctl->pti,
            &pcwdi->b_edit_ctl->client_area);
      TdgtSmplEditSetFocus(pcwdi->b_edit_ctl, FALSE);
   } else if (win == pcwdi->h_edit_ctl->pti->tci.win) {
      TidgetDrawFocusRect(pcwdi->h_edit_ctl->pti,
            &pcwdi->h_edit_ctl->client_area);
      TdgtSmplEditSetFocus(pcwdi->h_edit_ctl, FALSE);
   } else if (win == pcwdi->s_edit_ctl->pti->tci.win) {
      TidgetDrawFocusRect(pcwdi->s_edit_ctl->pti,
            &pcwdi->s_edit_ctl->client_area);
      TdgtSmplEditSetFocus(pcwdi->s_edit_ctl, FALSE);
   } else if (win == pcwdi->v_edit_ctl->pti->tci.win) {
      TidgetDrawFocusRect(pcwdi->v_edit_ctl->pti,
            &pcwdi->v_edit_ctl->client_area);
      TdgtSmplEditSetFocus(pcwdi->v_edit_ctl, FALSE);
   } else if (win == pcwdi->cname_edit_ctl->pti->tci.win) {
      TidgetDrawFocusRect(pcwdi->cname_edit_ctl->pti,
            &pcwdi->cname_edit_ctl->client_area);
      TdgtSmplEditSetFocus(pcwdi->cname_edit_ctl, FALSE);
   }
}

/* --------------------- HSRedrawCallback --------------------- */

static
void SetRGBControlValues(pTdgtBase)
   TdgtBase *pTdgtBase;
{
   ColorWheelDlgInfo *pcwdi=(ColorWheelDlgInfo*)(pTdgtBase->pti->userdata);
   char caption[MAXSTRING];

   sprintf(caption, "%1d", pcwdi->r);
   TdgtSmplEditSetText(pcwdi->r_edit_ctl, caption);
   sprintf(caption, "%1d", pcwdi->g);
   TdgtSmplEditSetText(pcwdi->g_edit_ctl, caption);
   sprintf(caption, "%1d", pcwdi->b);
   TdgtSmplEditSetText(pcwdi->b_edit_ctl, caption);
}

static
void SetHSVControlValues(pTdgtBase)
   TdgtBase *pTdgtBase;
{
   ColorWheelDlgInfo *pcwdi=(ColorWheelDlgInfo*)(pTdgtBase->pti->userdata);
   char caption[MAXSTRING];

   sprintf(caption, "%1d", pcwdi->h);
   TdgtSmplEditSetText(pcwdi->h_edit_ctl, caption);
   sprintf(caption, "%1d", pcwdi->s);
   TdgtSmplEditSetText(pcwdi->s_edit_ctl, caption);
   sprintf(caption, "%1d", pcwdi->v);
   TdgtSmplEditSetText(pcwdi->v_edit_ctl, caption);
}

static
void SetColorNameControlValue(pTdgtBase)
   TdgtBase *pTdgtBase;
{
   ColorWheelDlgInfo *pcwdi=(ColorWheelDlgInfo*)(pTdgtBase->pti->userdata);

   TdgtSmplEditSetText(pcwdi->cname_edit_ctl, pcwdi->color_str);
}

static
void SetupControls(pTdgtBase)
   TdgtBase *pTdgtBase;
{
   SetRGBControlValues(pTdgtBase);
   SetHSVControlValues(pTdgtBase);
   SetColorNameControlValue(pTdgtBase);
}

static
void RedrawHSPixmapControlValues(pTdgtBase)
   TdgtBase *pTdgtBase;
{
   ColorWheelDlgInfo *pcwdi=(ColorWheelDlgInfo*)(pTdgtBase->pti->userdata);

   RedrawTidget(pcwdi->hs_pixmap_ctl->pti);
}

static
void RedrawVPixmapControlValues(pTdgtBase)
   TdgtBase *pTdgtBase;
{
   ColorWheelDlgInfo *pcwdi=(ColorWheelDlgInfo*)(pTdgtBase->pti->userdata);

   RedrawTidget(pcwdi->v_pixmap_ctl->pti);
}

static
void RedrawRGBControlValues(pTdgtBase)
   TdgtBase *pTdgtBase;
{
   ColorWheelDlgInfo *pcwdi=(ColorWheelDlgInfo*)(pTdgtBase->pti->userdata);

   RedrawTidget(pcwdi->r_edit_ctl->pti);
   RedrawTidget(pcwdi->g_edit_ctl->pti);
   RedrawTidget(pcwdi->b_edit_ctl->pti);
}

static
void RedrawHSVControlValues(pTdgtBase)
   TdgtBase *pTdgtBase;
{
   ColorWheelDlgInfo *pcwdi=(ColorWheelDlgInfo*)(pTdgtBase->pti->userdata);

   RedrawTidget(pcwdi->h_edit_ctl->pti);
   RedrawTidget(pcwdi->s_edit_ctl->pti);
   RedrawTidget(pcwdi->v_edit_ctl->pti);
}

static
void RedrawColorNameControlValue(pTdgtBase)
   TdgtBase *pTdgtBase;
{
   ColorWheelDlgInfo *pcwdi=(ColorWheelDlgInfo*)(pTdgtBase->pti->userdata);

   RedrawTidget(pcwdi->cname_edit_ctl->pti);
}

static
void RedrawColorSampleControlValue(pTdgtBase)
   TdgtBase *pTdgtBase;
{
   ColorWheelDlgInfo *pcwdi=(ColorWheelDlgInfo*)(pTdgtBase->pti->userdata);

   RedrawTidget(pcwdi->csmpl_ctl->pti);
}

#define SKIP_NONE      0x0
#define SKIP_HS_PIXMAP 0x1
#define SKIP_V_PIXMAP  0x2

static
void RedrawControls(pTdgtBase, skip)
   TdgtBase *pTdgtBase;
   int skip;
{
   if ((skip & SKIP_HS_PIXMAP) != SKIP_HS_PIXMAP) {
      RedrawHSPixmapControlValues(pTdgtBase);
   }
   if ((skip & SKIP_V_PIXMAP) != SKIP_V_PIXMAP) {
      RedrawVPixmapControlValues(pTdgtBase);
   }
   RedrawRGBControlValues(pTdgtBase);
   RedrawHSVControlValues(pTdgtBase);
   RedrawColorNameControlValue(pTdgtBase);
   RedrawColorSampleControlValue(pTdgtBase);
}

static
void HSDrawSeg(dpy, win, gc, pTdgtDraw, vs)
   Display *dpy;
   Window win;
   GC gc;
   TdgtDraw *pTdgtDraw;
   XPoint *vs;
{
   if (vs[0].x < 0) vs[0].x = 0;
   if (vs[0].x >= HS_WIDTH) vs[0].x = HS_WIDTH-1;
   if (vs[0].y < 0) vs[0].y = 0;
   if (vs[0].y >= HS_HEIGHT) vs[0].y = HS_HEIGHT-1;

   if (vs[1].x < 1) vs[1].x = 1;
   if (vs[1].x >= HS_WIDTH) vs[1].x = HS_WIDTH-1;
   if (vs[1].y < 1) vs[1].y = 1;
   if (vs[1].y >= HS_HEIGHT) vs[1].y = HS_HEIGHT-1;

   vs[0].x += pTdgtDraw->client_area.x;
   vs[0].y += pTdgtDraw->client_area.y;
   vs[1].x += pTdgtDraw->client_area.x;
   vs[1].y += pTdgtDraw->client_area.y;

   XDrawLines(dpy, win, gc, vs, 2, CoordModeOrigin);
}

static
void HSRedrawCrosshair(pcwdi, pTdgtDraw)
   ColorWheelDlgInfo *pcwdi;
   TdgtDraw *pTdgtDraw;
{
   int seg_len=6;
   XPoint vs[2];
   XGCValues values;

   values.line_width = 3;
   XChangeGC(mainDisplay, revDefaultGC, GCLineWidth, &values);

   vs[0].x = pcwdi->hs_x;
   vs[0].y = pcwdi->hs_y - 2;
   vs[1].x = pcwdi->hs_x;
   vs[1].y = pcwdi->hs_y - 2 - seg_len;
   HSDrawSeg(mainDisplay, pTdgtDraw->pti->tci.win, revDefaultGC, pTdgtDraw, vs);

   vs[0].x = pcwdi->hs_x;
   vs[0].y = pcwdi->hs_y + 2;
   vs[1].x = pcwdi->hs_x;
   vs[1].y = pcwdi->hs_y + 2 + seg_len;
   HSDrawSeg(mainDisplay, pTdgtDraw->pti->tci.win, revDefaultGC, pTdgtDraw, vs);

   vs[0].x = pcwdi->hs_x - 2;
   vs[0].y = pcwdi->hs_y;
   vs[1].x = pcwdi->hs_x - 2 - seg_len;
   vs[1].y = pcwdi->hs_y;
   HSDrawSeg(mainDisplay, pTdgtDraw->pti->tci.win, revDefaultGC, pTdgtDraw, vs);

   vs[0].x = pcwdi->hs_x + 2;
   vs[0].y = pcwdi->hs_y;
   vs[1].x = pcwdi->hs_x + 2 + seg_len;
   vs[1].y = pcwdi->hs_y;
   HSDrawSeg(mainDisplay, pTdgtDraw->pti->tci.win, revDefaultGC, pTdgtDraw, vs);

   values.line_width = 1;
   XChangeGC(mainDisplay, revDefaultGC, GCLineWidth, &values);
}

static
void VDrawSeg(dpy, win, gc, pTdgtDraw, vs)
   Display *dpy;
   Window win;
   GC gc;
   TdgtDraw *pTdgtDraw;
   XPoint *vs;
{
   if (vs[0].x < 0) vs[0].x = 0;
   if (vs[0].x >= V_WIDTH) vs[0].x = V_WIDTH-1;
   if (vs[0].y < 0) vs[0].y = 0;
   if (vs[0].y >= V_HEIGHT) vs[0].y = V_HEIGHT-1;

   if (vs[1].x < 1) vs[1].x = 1;
   if (vs[1].x >= V_WIDTH) vs[1].x = V_WIDTH-1;
   if (vs[1].y < 1) vs[1].y = 1;
   if (vs[1].y >= V_HEIGHT) vs[1].y = V_HEIGHT-1;

   vs[0].x += pTdgtDraw->client_area.x;
   vs[0].y += pTdgtDraw->client_area.y;
   vs[1].x += pTdgtDraw->client_area.x;
   vs[1].y += pTdgtDraw->client_area.y;

   XDrawLines(dpy, win, gc, vs, 2, CoordModeOrigin);
}

static
void VRedrawLine(pcwdi, pTdgtDraw)
   ColorWheelDlgInfo *pcwdi;
   TdgtDraw *pTdgtDraw;
{
   XPoint vs[2];
   XGCValues values;

   values.line_width = 3;
   XChangeGC(mainDisplay, revDefaultGC, GCLineWidth, &values);

   vs[0].x = 0;
   vs[0].y = pcwdi->v_y;
   vs[1].x = V_WIDTH;
   vs[1].y = pcwdi->v_y;
   VDrawSeg(mainDisplay, pTdgtDraw->pti->tci.win, revDefaultGC, pTdgtDraw, vs);

   values.line_width = 1;
   XChangeGC(mainDisplay, revDefaultGC, GCLineWidth, &values);

}

static
int HSRedrawCallback(pTdgtDraw)
   TdgtDraw *pTdgtDraw;
{
   TdgtBase *pTdgtBase=
         (TdgtBase*)(pTdgtDraw->pti->tci.parent_tidgetinfo->tidget);
   ColorWheelDlgInfo *pcwdi=(ColorWheelDlgInfo*)(pTdgtBase->pti->userdata);

   if (pTdgtDraw == NULL) return FALSE;

   XCopyArea(mainDisplay, gColorWheelInfo.hs_pixmap, pTdgtDraw->pti->tci.win,
         gTidgetManager.gc, 0, 0, pTdgtDraw->client_area.w,
         pTdgtDraw->client_area.h, pTdgtDraw->client_area.x,
         pTdgtDraw->client_area.y);

   HSRedrawCrosshair(pcwdi, pTdgtDraw);
   if (TidgetGetFocusWindow() == pTdgtDraw->pti->tci.win) {
      TidgetDrawFocusRect(pTdgtDraw->pti, &pTdgtDraw->client_area);
   }
   TidgetManagerResetGC();

   return TRUE;
}

/* --------------------- HSEvHandlerCallback() --------------------- */

static
int ClickInColorWheelHSPixmap(pTdgtDraw, button_ev)
   TdgtDraw *pTdgtDraw;
   XButtonEvent *button_ev;
{
   TdgtBase *pTdgtBase=
         (TdgtBase*)(pTdgtDraw->pti->tci.parent_tidgetinfo->tidget);
   ColorWheelDlgInfo *pcwdi=(ColorWheelDlgInfo*)(pTdgtBase->pti->userdata);
   int x=button_ev->x-pTdgtDraw->client_area.x;
   int y=button_ev->y-pTdgtDraw->client_area.y;

   if (TidgetGetFocusWindow() == pTdgtDraw->pti->tci.win) {
      /* no need to change focus */
   } else {
      /* change focus */
      TidgetSendCmd(pTdgtBase->pti, TDGTCMD_REMOVE_FOCUS, 0, NULL);
      TidgetSetFocusWindow(pTdgtDraw->pti->tci.win);
   }
   if (x == pcwdi->hs_x && y == pcwdi->hs_y) {
      /* no change */
   } else {
      if (x < 0) x = 0;
      if (x >= HS_WIDTH) x = HS_WIDTH-1;
      if (y < 0) y = 0;
      if (y >= HS_HEIGHT) y = HS_HEIGHT-1;
      pcwdi->hs_x = x;
      pcwdi->hs_y = y;
      XYtoHS(pcwdi, x, y, &pcwdi->h, &pcwdi->s);
      SetupVPixmap(pcwdi);
      SetRGBFromHSV(pcwdi);
      SetPixelFromRGB(pcwdi);
      SetColorNameFromRGB(pcwdi);

      SetupControls(pTdgtBase);
      RedrawControls(pTdgtBase, SKIP_NONE);
   }
   return FALSE;
}

static
int HSEvHandlerCallback(pTdgtDraw, pXEv)
   TdgtDraw *pTdgtDraw;
   XEvent *pXEv;
{
   if (pXEv->xany.window == pTdgtDraw->pti->tci.win) {
      if (pXEv->type == ButtonPress) {
         XButtonEvent *button_ev=(&(pXEv->xbutton));

         return ClickInColorWheelHSPixmap(pTdgtDraw, button_ev);
      }
   }
   return FALSE;
}

/* --------------------- VRedrawCallback --------------------- */

static
int VRedrawCallback(pTdgtDraw)
   TdgtDraw *pTdgtDraw;
{
   TdgtBase *pTdgtBase=
         (TdgtBase*)(pTdgtDraw->pti->tci.parent_tidgetinfo->tidget);
   ColorWheelDlgInfo *pcwdi=(ColorWheelDlgInfo*)(pTdgtBase->pti->userdata);

   if (pTdgtDraw == NULL) return FALSE;

   XCopyArea(mainDisplay, gColorWheelInfo.v_pixmap, pTdgtDraw->pti->tci.win,
         gTidgetManager.gc, 0, 0, pTdgtDraw->client_area.w,
         pTdgtDraw->client_area.h, pTdgtDraw->client_area.x,
         pTdgtDraw->client_area.y);

   VRedrawLine(pcwdi, pTdgtDraw);
   if (TidgetGetFocusWindow() == pTdgtDraw->pti->tci.win) {
      TidgetDrawFocusRect(pTdgtDraw->pti, &pTdgtDraw->client_area);
   }
   TidgetManagerResetGC();

   return TRUE;
}

/* --------------------- VEvHandlerCallback() --------------------- */

static
int ClickInColorWheelVPixmap(pTdgtDraw, button_ev)
   TdgtDraw *pTdgtDraw;
   XButtonEvent *button_ev;
{
   TdgtBase *pTdgtBase=
         (TdgtBase*)(pTdgtDraw->pti->tci.parent_tidgetinfo->tidget);
   ColorWheelDlgInfo *pcwdi=(ColorWheelDlgInfo*)(pTdgtBase->pti->userdata);
   int y=button_ev->y-pTdgtDraw->client_area.y;

   if (TidgetGetFocusWindow() == pTdgtDraw->pti->tci.win) {
      /* no need to change focus */
   } else {
      /* change focus */
      TidgetSendCmd(pTdgtBase->pti, TDGTCMD_REMOVE_FOCUS, 0, NULL);
      TidgetSetFocusWindow(pTdgtDraw->pti->tci.win);
   }
   if (y == pcwdi->v_y) {
      /* no change */
   } else {
      if (y < 0) y = 0;
      if (y >= HS_HEIGHT) y = HS_HEIGHT-1;
      pcwdi->v_y = y;
      YtoV(pcwdi, y, &pcwdi->v);
      SetRGBFromHSV(pcwdi);
      SetPixelFromRGB(pcwdi);
      SetColorNameFromRGB(pcwdi);

      SetupControls(pTdgtBase);
      RedrawControls(pTdgtBase, SKIP_HS_PIXMAP);
   }
   return FALSE;
}

static
int VEvHandlerCallback(pTdgtDraw, pXEv)
   TdgtDraw *pTdgtDraw;
   XEvent *pXEv;
{
   if (pXEv->xany.window == pTdgtDraw->pti->tci.win) {
      if (pXEv->type == ButtonPress) {
         XButtonEvent *button_ev=(&(pXEv->xbutton));

         return ClickInColorWheelVPixmap(pTdgtDraw, button_ev);
      }
   }
   return FALSE;
}

/* --------------------- CSampleRedrawCallback --------------------- */

static
int CSampleRedrawCallback(pTdgtDraw)
   TdgtDraw *pTdgtDraw;
{
   TdgtBase *pTdgtBase=
         (TdgtBase*)(pTdgtDraw->pti->tci.parent_tidgetinfo->tidget);
   ColorWheelDlgInfo *pcwdi=(ColorWheelDlgInfo*)(pTdgtBase->pti->userdata);

   if (pTdgtDraw == NULL) return FALSE;

   XSetForeground(mainDisplay, gTidgetManager.gc, pcwdi->pixel);
   XFillRectangle(mainDisplay, pTdgtDraw->pti->tci.win, gTidgetManager.gc,
         pTdgtDraw->client_area.x, pTdgtDraw->client_area.y,
         pTdgtDraw->client_area.w, pTdgtDraw->client_area.h);

   TidgetManagerResetGC();

   return TRUE;
}

/* --------------- KeyPress Event Handlers --------------- */

static
int ValidateNumber(buf, max, pn_val)
   char *buf;
   int max, *pn_val;
{
   int i=0, len=strlen(buf), ival=0;;

   for (i=0; i < len; i++) {
      if (buf[i] < '0' || buf[i] > '9') return FALSE;
   }
   if (sscanf(buf, "%d", &ival) != 1) return FALSE;
   if (ival >= 0 && ival <= max) {
      *pn_val = ival;
      return TRUE;
   }
   return FALSE;
}

static
int ValidateRGB(pcwdi, pn_r, pn_g, pn_b)
   ColorWheelDlgInfo *pcwdi;
   int *pn_r, *pn_g, *pn_b;
{
   if (!ValidateNumber(TdgtSmplEditGetText(pcwdi->r_edit_ctl), 255, pn_r)) {
      snprintf(gszMsgBox, sizeof(gszMsgBox),
            TgLoadString(STID_GIVEN_VAL_MUST_BE_LE), "R", 255);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return FALSE;
   }
   if (!ValidateNumber(TdgtSmplEditGetText(pcwdi->g_edit_ctl), 255, pn_g)) {
      snprintf(gszMsgBox, sizeof(gszMsgBox),
            TgLoadString(STID_GIVEN_VAL_MUST_BE_LE), "G", 255);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return FALSE;
   }
   if (!ValidateNumber(TdgtSmplEditGetText(pcwdi->b_edit_ctl), 255, pn_b)) {
      snprintf(gszMsgBox, sizeof(gszMsgBox),
            TgLoadString(STID_GIVEN_VAL_MUST_BE_LE), "B", 255);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return FALSE;
   }
   return TRUE;
}

static
int ValidateHSV(pcwdi, pn_h, pn_s, pn_v)
   ColorWheelDlgInfo *pcwdi;
   int *pn_h, *pn_s, *pn_v;
{
   if (!ValidateNumber(TdgtSmplEditGetText(pcwdi->h_edit_ctl), 255, pn_h)) {
      snprintf(gszMsgBox, sizeof(gszMsgBox),
            TgLoadString(STID_GIVEN_VAL_MUST_BE_LE), "H", 255);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return FALSE;
   }
   if (!ValidateNumber(TdgtSmplEditGetText(pcwdi->s_edit_ctl), 255, pn_s)) {
      snprintf(gszMsgBox, sizeof(gszMsgBox),
            TgLoadString(STID_GIVEN_VAL_MUST_BE_LE), "S", 255);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return FALSE;
   }
   if (!ValidateNumber(TdgtSmplEditGetText(pcwdi->v_edit_ctl), 255, pn_v)) {
      snprintf(gszMsgBox, sizeof(gszMsgBox),
            TgLoadString(STID_GIVEN_VAL_MUST_BE_LE), "V", 255);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return FALSE;
   }
   return TRUE;
}

static
int ValidateColorName(pcwdi, pn_pixel)
   ColorWheelDlgInfo *pcwdi;
   int *pn_pixel;
{
   XColor xcolor;
   char color_str[40];
   char *c_ptr=TdgtSmplEditGetText(pcwdi->cname_edit_ctl);

   if (strlen(c_ptr) == 6 && AllHexChars(c_ptr)) {
      snprintf(color_str, sizeof(color_str), "#%s", c_ptr);
   } else {
      UtilStrCpyN(color_str, sizeof(color_str), c_ptr);
   }
   if (!TgifParseColor(color_str, &xcolor) || !XAllocColor(mainDisplay,
         mainColormap, &xcolor)) {
      snprintf(gszMsgBox, sizeof(gszMsgBox),
            TgLoadString(STID_CANNOT_PARSE_NAMED_COLOR), color_str);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return FALSE;
   }
   *pn_pixel = xcolor.pixel;

   return TRUE;
}

static
int HandleCRorLF(pTdgtBase)
   TdgtBase *pTdgtBase;
{
   ColorWheelDlgInfo *pcwdi=(ColorWheelDlgInfo*)(pTdgtBase->pti->userdata);
   Window win=TidgetGetFocusWindow();
   int r=0, g=0, b=0, h=0, s=0, v=0, pixel=0;
   int rgb_changed=FALSE, hsv_changed=FALSE, color_name_changed=FALSE;

   if (win == None) return FALSE;

   if (win == pcwdi->r_edit_ctl->pti->tci.win) {
      if (!ValidateRGB(pcwdi, &r, &g, &b)) {
         return FALSE;
      }
      pcwdi->r = r;
      pcwdi->g = g;
      pcwdi->b = b;
      rgb_changed = TRUE;
   } else if (win == pcwdi->g_edit_ctl->pti->tci.win) {
      if (!ValidateRGB(pcwdi, &r, &g, &b)) {
         return FALSE;
      }
      pcwdi->r = r;
      pcwdi->g = g;
      pcwdi->b = b;
      rgb_changed = TRUE;
   } else if (win == pcwdi->b_edit_ctl->pti->tci.win) {
      if (!ValidateRGB(pcwdi, &r, &g, &b)) {
         return FALSE;
      }
      pcwdi->r = r;
      pcwdi->g = g;
      pcwdi->b = b;
      rgb_changed = TRUE;
   } else if (win == pcwdi->h_edit_ctl->pti->tci.win) {
      if (!ValidateHSV(pcwdi, &h, &s, &v)) {
         return FALSE;
      }
      pcwdi->h = h;
      pcwdi->s = s;
      pcwdi->v = v;
      hsv_changed = TRUE;
   } else if (win == pcwdi->s_edit_ctl->pti->tci.win) {
      if (!ValidateHSV(pcwdi, &h, &s, &v)) {
         return FALSE;
      }
      pcwdi->h = h;
      pcwdi->s = s;
      pcwdi->v = v;
      hsv_changed = TRUE;
   } else if (win == pcwdi->v_edit_ctl->pti->tci.win) {
      if (!ValidateHSV(pcwdi, &h, &s, &v)) {
         return FALSE;
      }
      pcwdi->h = h;
      pcwdi->s = s;
      pcwdi->v = v;
      hsv_changed = TRUE;
   } else if (win == pcwdi->cname_edit_ctl->pti->tci.win) {
      if (!ValidateColorName(pcwdi, &pixel)) {
         return FALSE;
      }
      UtilStrCpyN(pcwdi->color_str, sizeof(pcwdi->color_str),
            TdgtSmplEditGetText(pcwdi->cname_edit_ctl));
      pcwdi->pixel = pixel;
      color_name_changed = TRUE;
   } else {
      return FALSE;
   }
   if (rgb_changed || hsv_changed) {
      if (rgb_changed) {
         SetPixelFromRGB(pcwdi);
         SetHSVFromRGB(pcwdi);
      } else if (hsv_changed) {
         SetRGBFromHSV(pcwdi);
         SetPixelFromRGB(pcwdi);
      } else {
         /* should not come here */
      }
      HStoXY(pcwdi, pcwdi->h, pcwdi->s, &pcwdi->hs_x, &pcwdi->hs_y);
      VtoY(pcwdi, pcwdi->v, &pcwdi->v_y);
      SetupVPixmap(pcwdi);
      SetColorNameFromRGB(pcwdi);
   } else if (color_name_changed) {
      PixeltoRGB(pcwdi, pcwdi->pixel, &pcwdi->r, &pcwdi->g, &pcwdi->b);
      SetHSVFromRGB(pcwdi);
      HStoXY(pcwdi, pcwdi->h, pcwdi->s, &pcwdi->hs_x, &pcwdi->hs_y);
      VtoY(pcwdi, pcwdi->v, &pcwdi->v_y);
      SetupVPixmap(pcwdi);
   }
   SetupControls(pTdgtBase);
   RedrawControls(pTdgtBase, SKIP_NONE);

   return FALSE;
}

static
int HandleTab(pTdgtBase)
   TdgtBase *pTdgtBase;
{
   ColorWheelDlgInfo *pcwdi=(ColorWheelDlgInfo*)(pTdgtBase->pti->userdata);
   Window win=TidgetGetFocusWindow();

   if (win == None) {
      TidgetSetFocusWindow(pcwdi->cname_edit_ctl->pti->tci.win);
      return FALSE;
   }
   if (win == pcwdi->hs_pixmap_ctl->pti->tci.win) {
      TidgetSendCmd(pTdgtBase->pti, TDGTCMD_REMOVE_FOCUS, 0, NULL);
      TidgetSetFocusWindow(pcwdi->v_pixmap_ctl->pti->tci.win);
      TidgetSendCmd(pTdgtBase->pti, TDGTCMD_REMOVE_FOCUS, 0, NULL);
   } else if (win == pcwdi->v_pixmap_ctl->pti->tci.win) {
      TidgetSendCmd(pTdgtBase->pti, TDGTCMD_REMOVE_FOCUS, 0, NULL);
      TdgtSmplEditSetFocus(pcwdi->r_edit_ctl, TRUE);
   } else if (win == pcwdi->r_edit_ctl->pti->tci.win) {
      HandleCRorLF(pTdgtBase);
      TidgetSendCmd(pTdgtBase->pti, TDGTCMD_REMOVE_FOCUS, 0, NULL);
      TdgtSmplEditSetFocus(pcwdi->h_edit_ctl, TRUE);
   } else if (win == pcwdi->g_edit_ctl->pti->tci.win) {
      HandleCRorLF(pTdgtBase);
      TidgetSendCmd(pTdgtBase->pti, TDGTCMD_REMOVE_FOCUS, 0, NULL);
      TdgtSmplEditSetFocus(pcwdi->s_edit_ctl, TRUE);
   } else if (win == pcwdi->b_edit_ctl->pti->tci.win) {
      HandleCRorLF(pTdgtBase);
      TidgetSendCmd(pTdgtBase->pti, TDGTCMD_REMOVE_FOCUS, 0, NULL);
      TdgtSmplEditSetFocus(pcwdi->v_edit_ctl, TRUE);
   } else if (win == pcwdi->h_edit_ctl->pti->tci.win) {
      HandleCRorLF(pTdgtBase);
      TidgetSendCmd(pTdgtBase->pti, TDGTCMD_REMOVE_FOCUS, 0, NULL);
      TdgtSmplEditSetFocus(pcwdi->g_edit_ctl, TRUE);
   } else if (win == pcwdi->s_edit_ctl->pti->tci.win) {
      HandleCRorLF(pTdgtBase);
      TidgetSendCmd(pTdgtBase->pti, TDGTCMD_REMOVE_FOCUS, 0, NULL);
      TdgtSmplEditSetFocus(pcwdi->b_edit_ctl, TRUE);
   } else if (win == pcwdi->v_edit_ctl->pti->tci.win) {
      HandleCRorLF(pTdgtBase);
      TidgetSendCmd(pTdgtBase->pti, TDGTCMD_REMOVE_FOCUS, 0, NULL);
      TdgtSmplEditSetFocus(pcwdi->cname_edit_ctl, TRUE);
   } else if (win == pcwdi->cname_edit_ctl->pti->tci.win) {
      HandleCRorLF(pTdgtBase);
      TidgetSendCmd(pTdgtBase->pti, TDGTCMD_REMOVE_FOCUS, 0, NULL);
      TidgetSetFocusWindow(pcwdi->hs_pixmap_ctl->pti->tci.win);
      TidgetSendCmd(pTdgtBase->pti, TDGTCMD_REMOVE_FOCUS, 0, NULL);
   } else {
      return FALSE;
   }
   return FALSE;
}

static
int HandleBackSpace(pTdgtBase)
   TdgtBase *pTdgtBase;
{
   ColorWheelDlgInfo *pcwdi=(ColorWheelDlgInfo*)(pTdgtBase->pti->userdata);
   Window win=TidgetGetFocusWindow();

   if (win == None) return FALSE;

   if (win == pcwdi->r_edit_ctl->pti->tci.win) {
      TdgtSmplEditDelLastChar(pcwdi->r_edit_ctl);
   } else if (win == pcwdi->g_edit_ctl->pti->tci.win) {
      TdgtSmplEditDelLastChar(pcwdi->g_edit_ctl);
   } else if (win == pcwdi->b_edit_ctl->pti->tci.win) {
      TdgtSmplEditDelLastChar(pcwdi->b_edit_ctl);
   } else if (win == pcwdi->h_edit_ctl->pti->tci.win) {
      TdgtSmplEditDelLastChar(pcwdi->h_edit_ctl);
   } else if (win == pcwdi->s_edit_ctl->pti->tci.win) {
      TdgtSmplEditDelLastChar(pcwdi->s_edit_ctl);
   } else if (win == pcwdi->v_edit_ctl->pti->tci.win) {
      TdgtSmplEditDelLastChar(pcwdi->v_edit_ctl);
   } else if (win == pcwdi->cname_edit_ctl->pti->tci.win) {
      TdgtSmplEditDelLastChar(pcwdi->cname_edit_ctl);
   }
   return FALSE;
}

static
int HandleCursorKey(pTdgtBase, key_sym)
   TdgtBase *pTdgtBase;
   KeySym key_sym;
{
   ColorWheelDlgInfo *pcwdi=(ColorWheelDlgInfo*)(pTdgtBase->pti->userdata);
   TdgtSmplEdit *pTdgtSmplEdit=NULL;
   char name[40];
   Window win=TidgetGetFocusWindow();
   int r=0, g=0, b=0, h=0, s=0, v=0;
   int dir=(-1), rgb_may_change=FALSE, hsv_may_change=FALSE;

   *name = '\0';
   if (win == None) return FALSE;

   switch (key_sym) {
   case XK_Up: dir = SCROLL_UP; break;
   case XK_KP_Up: dir = SCROLL_UP; break;
   case XK_Down: dir = SCROLL_DOWN; break;
   case XK_KP_Down: dir = SCROLL_DOWN; break;
   case XK_Left: dir = SCROLL_LEFT; break;
   case XK_KP_Left: dir = SCROLL_LEFT; break;
   case XK_Right: dir = SCROLL_RIGHT; break;
   case XK_KP_Right: dir = SCROLL_RIGHT; break;
   default: return FALSE;
   }
   if (win == pcwdi->hs_pixmap_ctl->pti->tci.win) {
      int x=pcwdi->hs_x, y=pcwdi->hs_y;

      switch (dir) {
      case SCROLL_UP: y--; break;
      case SCROLL_DOWN: y++; break;
      case SCROLL_LEFT: x--; break;
      case SCROLL_RIGHT: x++; break;
      }
      if (x < 0) x = 0;
      if (x > (HS_WIDTH-1)) x = (HS_WIDTH)-1;
      if (y < 0) y = 0;
      if (y > (HS_HEIGHT-1)) y = (HS_HEIGHT)-1;

      if (x == pcwdi->hs_x && y == pcwdi->hs_y) {
         /* no change */
      } else {
         pcwdi->hs_x = x;
         pcwdi->hs_y = y;
         XYtoHS(pcwdi, x, y, &pcwdi->h, &pcwdi->s);
         SetupVPixmap(pcwdi);
         SetRGBFromHSV(pcwdi);
         SetPixelFromRGB(pcwdi);
         SetColorNameFromRGB(pcwdi);

         SetupControls(pTdgtBase);
         RedrawControls(pTdgtBase, SKIP_NONE);
      }
   } else if (win == pcwdi->v_pixmap_ctl->pti->tci.win) {
      int y=pcwdi->v_y;

      switch (dir) {
      case SCROLL_UP: y--; break;
      case SCROLL_DOWN: y++; break;
      default: return FALSE;
      }
      if (y < 0) y = 0;
      if (y > (V_HEIGHT-1)) y = (V_HEIGHT)-1;

      if (y == pcwdi->v_y) {
         /* no change */
      } else {
         pcwdi->v_y = y;
         YtoV(pcwdi, y, &pcwdi->v);
         SetRGBFromHSV(pcwdi);
         SetPixelFromRGB(pcwdi);
         SetColorNameFromRGB(pcwdi);

         SetupControls(pTdgtBase);
         RedrawControls(pTdgtBase, SKIP_HS_PIXMAP);
      }
   } else if (win == pcwdi->r_edit_ctl->pti->tci.win) {
      strcpy(name, "R");
      rgb_may_change = TRUE;
      pTdgtSmplEdit = pcwdi->r_edit_ctl;
   } else if (win == pcwdi->g_edit_ctl->pti->tci.win) {
      strcpy(name, "G");
      rgb_may_change = TRUE;
      pTdgtSmplEdit = pcwdi->g_edit_ctl;
   } else if (win == pcwdi->b_edit_ctl->pti->tci.win) {
      strcpy(name, "B");
      rgb_may_change = TRUE;
      pTdgtSmplEdit = pcwdi->b_edit_ctl;
   } else if (win == pcwdi->h_edit_ctl->pti->tci.win) {
      strcpy(name, "H");
      hsv_may_change = TRUE;
      pTdgtSmplEdit = pcwdi->h_edit_ctl;
   } else if (win == pcwdi->s_edit_ctl->pti->tci.win) {
      strcpy(name, "S");
      hsv_may_change = TRUE;
      pTdgtSmplEdit = pcwdi->s_edit_ctl;
   } else if (win == pcwdi->v_edit_ctl->pti->tci.win) {
      strcpy(name, "V");
      hsv_may_change = TRUE;
      pTdgtSmplEdit = pcwdi->v_edit_ctl;
   } else if (win == pcwdi->cname_edit_ctl->pti->tci.win) {
      pTdgtSmplEdit = NULL;
   }
   if (pTdgtSmplEdit != NULL) {
      int ival=0, new_ival=0;

      if (rgb_may_change) {
         if (!ValidateRGB(pcwdi, &r, &g, &b)) {
            return FALSE;
         }
      } else if (hsv_may_change) {
         if (!ValidateHSV(pcwdi, &h, &s, &v)) {
            return FALSE;
         }
      } else {
         /* should not get here */
      }
      switch (dir) {
      case SCROLL_UP: break;
      case SCROLL_DOWN: break;
      default: return FALSE;
      }
      if (!ValidateNumber(TdgtSmplEditGetText(pTdgtSmplEdit), 255, &ival)) {
         /* this shouldn't happen because validated above already */
         return FALSE;
      }
      new_ival = ival;

      switch (dir) {
      case SCROLL_UP: new_ival++; break;
      case SCROLL_DOWN: new_ival--; break;
      default: return FALSE;
      }
      if (new_ival < 0) new_ival = 0;
      if (new_ival > 255) new_ival = 255;

      if (ival == new_ival) {
         /* no change */
      } else {
         switch (name[0]) {
         case 'R': pcwdi->r = new_ival; break;
         case 'G': pcwdi->g = new_ival; break;
         case 'B': pcwdi->b = new_ival; break;
         case 'H': pcwdi->h = new_ival; break;
         case 'S': pcwdi->s = new_ival; break;
         case 'V': pcwdi->v = new_ival; break;
         }
         if (rgb_may_change) {
            SetPixelFromRGB(pcwdi);
            SetHSVFromRGB(pcwdi);
         } else if (hsv_may_change) {
            SetRGBFromHSV(pcwdi);
            SetPixelFromRGB(pcwdi);
         } else {
            /* should not get here */
         }
         HStoXY(pcwdi, pcwdi->h, pcwdi->s, &pcwdi->hs_x, &pcwdi->hs_y);
         VtoY(pcwdi, pcwdi->v, &pcwdi->v_y);
         SetupVPixmap(pcwdi);
         SetColorNameFromRGB(pcwdi);

         SetupControls(pTdgtBase);
         RedrawControls(pTdgtBase, SKIP_NONE);
      }
   }
   return FALSE;
}

static
int HandleChar(pTdgtBase, key_sym, buf, has_ch)
   TdgtBase *pTdgtBase;
   KeySym key_sym;
   char *buf;
   int has_ch;
{
   ColorWheelDlgInfo *pcwdi=(ColorWheelDlgInfo*)(pTdgtBase->pti->userdata);
   Window win=TidgetGetFocusWindow();
   TdgtSmplEdit *pTdgtSmplEdit=NULL;
   int len=0, len_limit=3;
   char *caption=NULL;

   if (win == None) return FALSE;

   if (win == pcwdi->r_edit_ctl->pti->tci.win) {
      pTdgtSmplEdit = pcwdi->r_edit_ctl;
   } else if (win == pcwdi->g_edit_ctl->pti->tci.win) {
      pTdgtSmplEdit = pcwdi->g_edit_ctl;
   } else if (win == pcwdi->b_edit_ctl->pti->tci.win) {
      pTdgtSmplEdit = pcwdi->b_edit_ctl;
   } else if (win == pcwdi->h_edit_ctl->pti->tci.win) {
      pTdgtSmplEdit = pcwdi->h_edit_ctl;
   } else if (win == pcwdi->s_edit_ctl->pti->tci.win) {
      pTdgtSmplEdit = pcwdi->s_edit_ctl;
   } else if (win == pcwdi->v_edit_ctl->pti->tci.win) {
      pTdgtSmplEdit = pcwdi->v_edit_ctl;
   } else if (win == pcwdi->cname_edit_ctl->pti->tci.win) {
      pTdgtSmplEdit = pcwdi->cname_edit_ctl;
      len_limit = 40;
   } else {
      return FALSE;
   }
   caption = TdgtSmplEditGetText(pTdgtSmplEdit);
   len = strlen(caption);
   if (len >= len_limit) return FALSE;
   if (len_limit == 3) {
      /* rgb or hsv */
      if (*buf < '0' || *buf > '9') return FALSE;
   } else {
      /* anything goes */
   }
   TdgtSmplEditAppendStr(pTdgtSmplEdit, buf, 1);

   return FALSE;
}

/* --------------- TdgtColorWheelDlgKeyPressEvHandlerCallback --------------- */

static XComposeStatus c_stat;

static
int TdgtColorWheelDlgKeyPressEvHandlerCallback(pTdgtBase, pXEv)
   TdgtBase *pTdgtBase;
   XEvent *pXEv;
{
   ColorWheelDlgInfo *pcwdi=(ColorWheelDlgInfo*)(pTdgtBase->pti->userdata);
   int has_ch=0;
   char buf[80];
   KeySym key_sym=(KeySym)0;
   XKeyEvent *key_ev=NULL;

   if (pXEv->type != KeyPress) return FALSE;
   if (pcwdi == NULL) return FALSE;

   key_ev = (&(pXEv->xkey));
   has_ch = XLookupString(key_ev, buf, sizeof(buf), &key_sym, &c_stat);
   TranslateKeys(buf, &key_sym);

   if (CharIsESC(key_ev, buf, key_sym, &has_ch)) {
      HideTdgtColorWheelDialogBox();
   } else if (CharIsCRorLF(key_ev, buf, key_sym, &has_ch)) {
      return HandleCRorLF(pTdgtBase);
   } else if (CharIsTAB(key_ev, buf, key_sym, &has_ch)) {
      return HandleTab(pTdgtBase);
   } else if (CharIsCntrlSpace(key_ev, buf, key_sym, &has_ch)) {
      HideTdgtColorWheelDialogBox();
   } else if (CharIsBS(key_ev, buf, key_sym, &has_ch, FALSE)) {
      return HandleBackSpace(pTdgtBase);
   } else if (key_sym==XK_Up || key_sym==XK_KP_Up || key_sym==XK_Down ||
         key_sym==XK_KP_Down || key_sym==XK_Left || key_sym==XK_KP_Left ||
         key_sym==XK_Right || key_sym==XK_KP_Right ||
         (key_sym>'\040' && key_sym<='\177' &&
         (key_ev->state & ControlMask) == ControlMask)) {
      return HandleCursorKey(pTdgtBase, key_sym);
   } else if (key_sym>='\040' && key_sym<='\177') {
      return HandleChar(pTdgtBase, key_sym, buf, has_ch);
   }
   return FALSE;
}

/* --------------------- ShowTdgtColorWheelDialogBox --------------------- */

void ShowTdgtColorWheelDialogBox()
{
   TidgetInfo *pti=gColorWheelInfo.dialogbox_tidgetinfo;
   ColorWheelDlgInfo *pcwdi=(ColorWheelDlgInfo*)(pti->userdata);

   TidgetManagerSetWantAllKeyPressEvents(pti, TRUE);
   gColorWheelInfo.mapped = TRUE;
   MapTidget(pti);

   if (pcwdi != NULL) {
      ClearAllControls((TdgtBase*)(pti->tidget));
      SetupColorsFromCurrentColor(pcwdi);
      SetupControls((TdgtBase*)(pti->tidget));
   }
}

/* --------------------- HideTdgtColorWheelDialogBox --------------------- */

void HideTdgtColorWheelDialogBox()
{
   TidgetManagerSetWantAllKeyPressEvents(NULL, FALSE);
   TidgetSetHasFocus(FALSE);
   gColorWheelInfo.mapped = FALSE;
   XUnmapWindow(mainDisplay, gColorWheelInfo.dialogbox_tidgetinfo->tci.win);

   if (warpToWinCenter) {
      XWarpPointer(mainDisplay, None, drawWindow, 0, 0, 0, 0,
            (int)(ZOOMED_SIZE(drawWinW)>>1), (int)(ZOOMED_SIZE(drawWinH)>>1));
   }
}

/* --------------------- CreateTdgtColorWheelDialogBox --------------------- */

static
int TdgtColorWheelDlgSetEntries(pti, dsp_ptr, entries, num_entries, marked_index)
   TidgetInfo *pti;
   DspList *dsp_ptr;
   char **entries;
   int num_entries, marked_index;
{
   return TRUE;
}

static
int TdgtColorWheelDlgSetMsg(pti, msg)
   TidgetInfo *pti;
   char *msg;
{
   return TRUE;
}

static
void InitTdgtColorWheelDlgInfo(pti)
   TidgetInfo *pti;
{
   ColorWheelDlgInfo *pcwdi=NULL;

   if (pti == NULL) return;

   pcwdi = (ColorWheelDlgInfo*)(pti->userdata);
   if (pcwdi == NULL) return;

   pcwdi->screen_w = DisplayWidth(mainDisplay, mainScreen);
   pcwdi->screen_h = DisplayHeight(mainDisplay, mainScreen);
   TdgtColorWheelDlgSetMsg(pti, "");

   pcwdi->def_btn_id = ID_CWHEEL_BTN_OK;
}

typedef struct tagCreateBtnInfo {
   char *text;
   int ctl_id;
} CreateBtnInfo;

static CreateBtnInfo cbi[] = {
   { "Add to Palette", ID_CWHEEL_BTN_ADD    },
   { "Add and Close",  ID_CWHEEL_BTN_OK     },
   { "Close",          ID_CWHEEL_BTN_CANCEL },
   { NULL, 0 }
};

static
int CreateColorWheelButtons(pTdgtBase)
   TdgtBase *pTdgtBase;
{
   ColorWheelDlgInfo *pcwdi=(ColorWheelDlgInfo*)(pTdgtBase->pti->userdata);
   TdgtBtnRow *pTdgtBtnRow=pcwdi->btn_row_ctl;
   int i=0;

   for (i=0; cbi[i].text != NULL; i++) {
      TdgtBtn *pTdgtBtn=NULL;
      int btn_w=0, btn_h=0;

      CalcTdgtBtnDim(cbi[i].text, 8, TDGTBTN_DEF_H_PAD, TDGTBTN_DEF_V_PAD,
            &btn_w, &btn_h);
      pTdgtBtn = CreateTdgtBtn(pTdgtBtnRow->pti->tci.win,
            pTdgtBtnRow->pti, cbi[i].ctl_id, 0, 0, btn_w, btn_h,
            TDGTBTN_DEF_H_PAD, TDGTBTN_DEF_V_PAD, TGMUTYPE_TEXT, TDGTBTN_CLICK,
            TGBS_NORMAL, STYLE_NR, cbi[i].text, NULL);
      if (pTdgtBtn == NULL) {
         DestroyTidget(&pTdgtBase->pti);
         return FALSE;
      }
      if (!TdgtBtnRowAddBtn(pTdgtBtnRow, pTdgtBtn)) {
         DestroyTidget(&pTdgtBase->pti);
         return FALSE;
      }
   }
   return TRUE;
}

static
void AdjustDialogBoxWidth(dpy, pTdgtBase)
   Display *dpy;
   TdgtBase *pTdgtBase;
{
   ColorWheelDlgInfo *pcwdi=(ColorWheelDlgInfo*)(pTdgtBase->pti->userdata);
   int max_w=0, bottom=0, h_pad=pTdgtBase->pti->tci.h_pad, cur_x=0, cur_y=0;
   int v_pad=pTdgtBase->pti->tci.v_pad, end_y=0;
   SimpleWinInfo *pswi=NULL;

   /* hue edit */
   pswi = (&(pcwdi->h_edit_ctl->pti->tci.win_info));
   max_w = pcwdi->hue_edit_x + pswi->w - pcwdi->hs_pixmap_x;

   bottom = pcwdi->btn_row_ctl->pti->tci.win_info.y +
         pcwdi->btn_row_ctl->pti->tci.win_info.h;

   cur_x = windowPadding + h_pad;
   cur_y = windowPadding + v_pad;

   /* hs_pixmap */
   pswi = (&(pcwdi->hs_pixmap_ctl->pti->tci.win_info));
   end_y = pswi->y+pswi->h;

   /* button row */
   pswi = (&(pcwdi->btn_row_ctl->pti->tci.win_info));
   pswi->w = max_w;
   TidgetMoveResize(pcwdi->btn_row_ctl->pti, pswi->x, pswi->y, pswi->w,
         pswi->h);

   pswi = (&(pTdgtBase->pti->tci.win_info));
   pswi->w = max_w+(windowPadding<<1)+(h_pad<<1);
   pswi->h = bottom+windowPadding+v_pad;
   TidgetMoveResize(pTdgtBase->pti, pswi->x, pswi->y, pswi->w, pswi->h);
}

static
void TdgtColorWheelDlgWmDeleteCallback(pTdgtBase)
   TdgtBase *pTdgtBase;
{
   ColorWheelDlgInfo *pcwdi=(ColorWheelDlgInfo*)(pTdgtBase->pti->userdata);

   if (pcwdi == NULL) return;

   HideTdgtColorWheelDialogBox();
}

static
int TdgtColorWheelDlgFocusCallback(pTdgtBase, cmd_type, cmd_arg,
      pv_cmd_userdata)
   TdgtBase *pTdgtBase;
   int cmd_type, cmd_arg;
   void *pv_cmd_userdata;
{
   ColorWheelDlgInfo *pcwdi=(ColorWheelDlgInfo*)(pTdgtBase->pti->userdata);

   if (pcwdi == NULL) return FALSE;

   switch (cmd_type) {
   case TDGTCMD_CAN_HANDLE_FOCUS_CHANGE: return TRUE;
   case TDGTCMD_REMOVE_FOCUS: ColorWheelDlgRemoveFocus(pTdgtBase); return TRUE;
   }
   return FALSE;
}

static
void AdjustDialogBoxSize(dpy, pTdgtBase)
   Display *dpy;
   TdgtBase *pTdgtBase;
{
   ColorWheelDlgInfo *pcwdi=(ColorWheelDlgInfo*)(pTdgtBase->pti->userdata);
   int base_win_x=0, base_win_y=0, base_win_w=0, base_win_h=0;
   XWMHints wmhints;
   XSizeHints sizehints;
   
   if (pTdgtBase == NULL) return;

   AdjustDialogBoxWidth(dpy, pTdgtBase);

   base_win_w = pTdgtBase->pti->tci.win_info.w;
   base_win_h = pTdgtBase->pti->tci.win_info.h;

   memset(&wmhints, 0, sizeof(XWMHints));
   wmhints.flags = InputHint | StateHint;
   wmhints.input = True;
   wmhints.initial_state = NormalState;
   XSetWMHints(dpy, pTdgtBase->pti->tci.win, &wmhints);
   
   base_win_x = (base_win_w > pcwdi->screen_w) ? 0 :
         ((pcwdi->screen_w-base_win_w)>>1);
   base_win_y = (base_win_h > pcwdi->screen_h) ? 0 :
         ((pcwdi->screen_h-base_win_h)/3);

   XMoveResizeWindow(dpy, pTdgtBase->pti->tci.win, base_win_x,
         base_win_y, base_win_w, base_win_h);

   sizehints.flags = PPosition | PSize | USPosition | PMinSize | PMaxSize;
   sizehints.x = base_win_x;
   sizehints.y = base_win_y;
   sizehints.width = sizehints.min_width = sizehints.max_width = base_win_w;
   sizehints.height = sizehints.min_height = sizehints.max_height = base_win_h;
#ifdef NOTR4MODE
   XSetNormalHints(dpy, pTdgtBase->pti->tci.win, &sizehints);
#else
   XSetWMNormalHints(dpy, pTdgtBase->pti->tci.win, &sizehints);
#endif

   XSetTransientForHint(dpy, pTdgtBase->pti->tci.win, mainWindow);
}

static
int SetupHSPixmap(pcwdi)
   ColorWheelDlgInfo *pcwdi;
{
   int row=0;
   Pixmap pixmap=gColorWheelInfo.hs_pixmap;
   XImage *image=NULL;

   if (pixmap == None) return FALSE;
   image = XGetImage(mainDisplay, pixmap, 0, 0, HS_WIDTH, HS_HEIGHT,
         AllPlanes, ZPixmap);
   if (image == NULL) {
      Msg(TgLoadString(STID_XGETIMAGE_MAY_RUN_OUT_VMEM));
      return FALSE;
   }
   for (row=0; row < HS_HEIGHT; row++) {
      int col=0;
      double sat=(((double)(HS_HEIGHT-1-row))/((double)(HS_HEIGHT-1)));

      sat *= ((double)0x0ffff);
      for (col=0; col < HS_WIDTH; col++) {
         int hue=(col*2), r=0, g=0, b=0;
         double dr=(double)0, dg=(double)0, db=(double)0;
         uint32_t pixel=0;

         HSVtoRGB(hue, sat, 0x0ffff, &r, &g, &b);
         dr = ((double)r) / ((double)0x0ffff) * pcwdi->tci.dr_maxval;
         dg = ((double)g) / ((double)0x0ffff) * pcwdi->tci.dr_maxval;
         db = ((double)b) / ((double)0x0ffff) * pcwdi->tci.dr_maxval;
         r = round(dr);
         g = round(dg);
         b = round(db);
         if (r < 0) r = 0;
         if (g < 0) g = 0;
         if (b < 0) b = 0;
         if (r > 255) r = 255;
         if (g > 255) g = 255;
         if (b > 255) b = 255;
         pixel = RGBtoPixel(pcwdi, r, g, b);
         XPutPixel(image, col, row, pixel);
      }
   }
   XPutImage(mainDisplay, pixmap, xpmGC, image, 0, 0, 0, 0, HS_WIDTH,
         HS_HEIGHT);
   XDestroyImage(image);

   return TRUE;
}

TidgetInfo *CreateTdgtColorWheelDialogBox(dpy, parent_win)
   Display *dpy;
   Window parent_win; /* should be the rootWindow */
{
   int cur_x=0, cur_y=0, v_gap=20, h_pad=0, v_pad=0;
   int w=0, h=0, vh_padding=0, h_gap=14, end_y=0, rgbhsv_width=0;
   TdgtBase *pTdgtBase=NULL;
   ColorWheelDlgInfo *pcwdi=NULL;
   char caption[MAXSTRING];

   TidgetSetFocusWindow(None);
   if (!colorDisplay || mainVisual->class != TrueColor) {
      sprintf(gszMsgBox, TgLoadString(STID_COLOR_WHEEL_NOT_AVAIL));
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      free(pcwdi);
      return NULL;
   }
   pcwdi = (ColorWheelDlgInfo*)malloc(sizeof(ColorWheelDlgInfo));
   if (pcwdi == NULL) FailAllocMessage();
   memset(pcwdi, 0, sizeof(ColorWheelDlgInfo));

   memset(&pcwdi->tci, 0, sizeof(TrueColorInfo));
   if (!SetupTrueColorInfo(&pcwdi->tci) || !SetupHSPixmap(pcwdi) ||
         !SetupColorsFromCurrentColor(pcwdi)) {
      free(pcwdi);
      return NULL;
   }
   if (boldMsgFontSet != NULL || boldMsgFontPtr != NULL) {
       rgbhsv_width = BoldMsgTextWidth(boldMsgFontPtr, "9999", 4);
   } else if (msgFontSet != NULL || msgFontPtr != NULL) {
       rgbhsv_width = MsgTextWidth(msgFontPtr, "9999", 4);
   } else {
       rgbhsv_width = defaultFontWidth * 4;
   }
   h_pad = TDGTBASE_DEF_H_PAD;
   v_pad = TDGTBASE_DEF_V_PAD;
   if (msgFontSet != NULL || msgFontPtr != NULL) {
      vh_padding = (msgFontWidth<<1);
   } else {
      vh_padding = (defaultFontWidth<<1);
   }
   cur_x = vh_padding+windowPadding;
   cur_y = vh_padding+windowPadding;

   sprintf(caption, TgLoadString(STID_TOOL_COLOR_WHEEL), TOOL_NAME);
   pTdgtBase = CreateTdgtBase(parent_win, NULL, ID_CWHEEL_DIALOG, 0, 0, 100,
         100, vh_padding, vh_padding, (dialogboxUse3DBorder ? TGBS_RAISED :
         TGBS_NORMAL), caption);
   if (pTdgtBase == NULL) {
      free(pcwdi);
      return NULL;
   }
   pTdgtBase->pti->userdata = pcwdi;
   InitTdgtColorWheelDlgInfo(pTdgtBase->pti);

   /* hs_pixmap */
   pcwdi->hs_pixmap_x = cur_x;
   pcwdi->hs_pixmap_y = cur_y;
   pcwdi->hs_pixmap_ctl = CreateTdgtDraw(pTdgtBase->pti->tci.win,
         pTdgtBase->pti, ID_CWHEEL_HS_AREA, cur_x, cur_y, HS_WIDTH, HS_HEIGHT,
         TDGTDRAW_DEF_H_PAD, TDGTDRAW_DEF_V_PAD, TGBS_LOWRED, NULL);
   if (pcwdi->hs_pixmap_ctl == NULL) {
      DestroyTidget(&pTdgtBase->pti);
      return NULL;
   }
   TdgtDrawSetRedrawCallback(pcwdi->hs_pixmap_ctl, HSRedrawCallback);
   TdgtDrawSetEvHandlerCallback(pcwdi->hs_pixmap_ctl, HSEvHandlerCallback);

   cur_x += pcwdi->hs_pixmap_ctl->pti->tci.win_info.w + h_gap;
   end_y = cur_y + pcwdi->hs_pixmap_ctl->pti->tci.win_info.h - windowPadding;

   /* v_pixmap */
   pcwdi->v_pixmap_x = cur_x;
   pcwdi->v_pixmap_y = cur_y;
   pcwdi->v_pixmap_ctl = CreateTdgtDraw(pTdgtBase->pti->tci.win, pTdgtBase->pti,
         ID_CWHEEL_V_AREA, cur_x, cur_y, V_WIDTH, V_HEIGHT, TDGTDRAW_DEF_H_PAD,
         TDGTDRAW_DEF_V_PAD, TGBS_LOWRED, NULL);
   if (pcwdi->v_pixmap_ctl == NULL) {
      DestroyTidget(&pTdgtBase->pti);
      return NULL;
   }
   TdgtDrawSetRedrawCallback(pcwdi->v_pixmap_ctl, VRedrawCallback);
   TdgtDrawSetEvHandlerCallback(pcwdi->v_pixmap_ctl, VEvHandlerCallback);

   cur_x += pcwdi->v_pixmap_ctl->pti->tci.win_info.w + h_gap;

   /* row 1 */

   /* red msg */
   pcwdi->red_msg_x = cur_x;
   pcwdi->red_msg_y = cur_y;
   pcwdi->r_msg_ctl = CreateTdgtMsg(pTdgtBase->pti->tci.win, pTdgtBase->pti,
         INVALID, cur_x, cur_y, TDGTMSG_DEF_H_PAD, TDGTMSG_DEF_V_PAD,
         TGBS_NORMAL, "R:", JUST_L);
   if (pcwdi->r_msg_ctl == NULL) {
      DestroyTidget(&pTdgtBase->pti);
      return NULL;
   }
   cur_x += pcwdi->r_msg_ctl->pti->tci.win_info.w;

   /* red edit */
   pcwdi->red_edit_x = cur_x;
   pcwdi->red_edit_y = cur_y;
   pcwdi->r_edit_ctl = CreateTdgtSmplEdit(pTdgtBase->pti->tci.win,
         pTdgtBase->pti, ID_CWHEEL_EDIT_R, cur_x, cur_y, rgbhsv_width,
         TDGTSEDT_DEF_H_PAD, TDGTSEDT_DEF_V_PAD, "", STYLE_NR, INVALID);
   if (pcwdi->r_edit_ctl == NULL) {
      DestroyTidget(&pTdgtBase->pti);
      return NULL;
   }
   cur_x += pcwdi->r_edit_ctl->pti->tci.win_info.w + 4;

   /* hue msg */
   pcwdi->hue_msg_x = cur_x;
   pcwdi->hue_msg_y = cur_y;
   pcwdi->h_msg_ctl = CreateTdgtMsg(pTdgtBase->pti->tci.win, pTdgtBase->pti,
         INVALID, cur_x, cur_y, TDGTMSG_DEF_H_PAD, TDGTMSG_DEF_V_PAD,
         TGBS_NORMAL, "H:", JUST_L);
   if (pcwdi->h_msg_ctl == NULL) {
      DestroyTidget(&pTdgtBase->pti);
      return NULL;
   }
   cur_x += pcwdi->h_msg_ctl->pti->tci.win_info.w;

   /* hue edit */
   pcwdi->hue_edit_x = cur_x;
   pcwdi->hue_edit_y = cur_y;
   pcwdi->h_edit_ctl = CreateTdgtSmplEdit(pTdgtBase->pti->tci.win,
         pTdgtBase->pti, ID_CWHEEL_EDIT_H, cur_x, cur_y, rgbhsv_width,
         TDGTSEDT_DEF_H_PAD, TDGTSEDT_DEF_V_PAD, "", STYLE_NR, INVALID);
   if (pcwdi->h_edit_ctl == NULL) {
      DestroyTidget(&pTdgtBase->pti);
      return NULL;
   }
   cur_x = pcwdi->red_msg_x;
   cur_y += pcwdi->h_edit_ctl->pti->tci.win_info.h + 4;

   /* row 2 */

   /* green msg */
   pcwdi->green_msg_x = cur_x;
   pcwdi->green_msg_y = cur_y;
   pcwdi->g_msg_ctl = CreateTdgtMsg(pTdgtBase->pti->tci.win, pTdgtBase->pti,
         INVALID, cur_x, cur_y, TDGTMSG_DEF_H_PAD, TDGTMSG_DEF_V_PAD,
         TGBS_NORMAL, "G:", JUST_L);
   if (pcwdi->g_msg_ctl == NULL) {
      DestroyTidget(&pTdgtBase->pti);
      return NULL;
   }
   cur_x = pcwdi->red_edit_x;

   /* green edit */
   pcwdi->green_edit_x = cur_x;
   pcwdi->green_edit_y = cur_y;
   pcwdi->g_edit_ctl = CreateTdgtSmplEdit(pTdgtBase->pti->tci.win,
         pTdgtBase->pti, ID_CWHEEL_EDIT_G, cur_x, cur_y, rgbhsv_width,
         TDGTSEDT_DEF_H_PAD, TDGTSEDT_DEF_V_PAD, "", STYLE_NR, INVALID);
   if (pcwdi->g_edit_ctl == NULL) {
      DestroyTidget(&pTdgtBase->pti);
      return NULL;
   }
   cur_x = pcwdi->hue_msg_x;

   /* sat msg */
   pcwdi->sat_msg_x = cur_x;
   pcwdi->sat_msg_y = cur_y;
   pcwdi->s_msg_ctl = CreateTdgtMsg(pTdgtBase->pti->tci.win, pTdgtBase->pti,
         INVALID, cur_x, cur_y, TDGTMSG_DEF_H_PAD, TDGTMSG_DEF_V_PAD,
         TGBS_NORMAL, "S:", JUST_L);
   if (pcwdi->s_msg_ctl == NULL) {
      DestroyTidget(&pTdgtBase->pti);
      return NULL;
   }
   cur_x = pcwdi->hue_edit_x;

   /* sat edit */
   pcwdi->sat_edit_x = cur_x;
   pcwdi->sat_edit_y = cur_y;
   pcwdi->s_edit_ctl = CreateTdgtSmplEdit(pTdgtBase->pti->tci.win,
         pTdgtBase->pti, ID_CWHEEL_EDIT_S, cur_x, cur_y, rgbhsv_width,
         TDGTSEDT_DEF_H_PAD, TDGTSEDT_DEF_V_PAD, "", STYLE_NR, INVALID);
   if (pcwdi->s_edit_ctl == NULL) {
      DestroyTidget(&pTdgtBase->pti);
      return NULL;
   }
   cur_x = pcwdi->red_msg_x;
   cur_y += pcwdi->s_edit_ctl->pti->tci.win_info.h + 4;

   /* row 3 */

   /* blue msg */
   pcwdi->blue_msg_x = cur_x;
   pcwdi->blue_msg_y = cur_y;
   pcwdi->b_msg_ctl = CreateTdgtMsg(pTdgtBase->pti->tci.win, pTdgtBase->pti,
         INVALID, cur_x, cur_y, TDGTMSG_DEF_H_PAD, TDGTMSG_DEF_V_PAD,
         TGBS_NORMAL, "B:", JUST_L);
   if (pcwdi->b_msg_ctl == NULL) {
      DestroyTidget(&pTdgtBase->pti);
      return NULL;
   }
   cur_x += pcwdi->b_msg_ctl->pti->tci.win_info.w;

   /* blue edit */
   pcwdi->blue_edit_x = cur_x;
   pcwdi->blue_edit_y = cur_y;
   pcwdi->b_edit_ctl = CreateTdgtSmplEdit(pTdgtBase->pti->tci.win,
         pTdgtBase->pti, ID_CWHEEL_EDIT_B, cur_x, cur_y, rgbhsv_width,
         TDGTSEDT_DEF_H_PAD, TDGTSEDT_DEF_V_PAD, "", STYLE_NR, INVALID);
   if (pcwdi->b_edit_ctl == NULL) {
      DestroyTidget(&pTdgtBase->pti);
      return NULL;
   }
   cur_x += pcwdi->b_edit_ctl->pti->tci.win_info.w + 4;

   /* val msg */
   pcwdi->val_msg_x = cur_x;
   pcwdi->val_msg_y = cur_y;
   pcwdi->v_msg_ctl = CreateTdgtMsg(pTdgtBase->pti->tci.win, pTdgtBase->pti,
         INVALID, cur_x, cur_y, TDGTMSG_DEF_H_PAD, TDGTMSG_DEF_V_PAD,
         TGBS_NORMAL, "V:", JUST_L);
   if (pcwdi->v_msg_ctl == NULL) {
      DestroyTidget(&pTdgtBase->pti);
      return NULL;
   }
   cur_x += pcwdi->v_msg_ctl->pti->tci.win_info.w;

   /* val edit */
   pcwdi->val_edit_x = cur_x;
   pcwdi->val_edit_y = cur_y;
   pcwdi->v_edit_ctl = CreateTdgtSmplEdit(pTdgtBase->pti->tci.win,
         pTdgtBase->pti, ID_CWHEEL_EDIT_V, cur_x, cur_y, rgbhsv_width,
         TDGTSEDT_DEF_H_PAD, TDGTSEDT_DEF_V_PAD, "", STYLE_NR, INVALID);
   if (pcwdi->v_edit_ctl == NULL) {
      DestroyTidget(&pTdgtBase->pti);
      return NULL;
   }
   cur_x = pcwdi->red_msg_x;
   cur_y += pcwdi->v_edit_ctl->pti->tci.win_info.h + 4;

   /* row 4 */

   /* cname msg */
   pcwdi->cname_msg_x = cur_x;
   pcwdi->cname_msg_y = cur_y;
   pcwdi->cname_msg_ctl = CreateTdgtMsg(pTdgtBase->pti->tci.win, pTdgtBase->pti,
         INVALID, cur_x, cur_y, TDGTMSG_DEF_H_PAD, TDGTMSG_DEF_V_PAD,
         TGBS_NORMAL, "Name:", JUST_L);
   if (pcwdi->cname_msg_ctl == NULL) {
      DestroyTidget(&pTdgtBase->pti);
      return NULL;
   }
   cur_x += pcwdi->cname_msg_ctl->pti->tci.win_info.w;

   /* cname edit */
   pcwdi->cname_edit_x = cur_x;
   pcwdi->cname_edit_y = cur_y;
   w = pcwdi->val_edit_x + pcwdi->v_edit_ctl->pti->tci.win_info.w - cur_x -
         (TDGTSEDT_DEF_H_PAD<<1) - (windowPadding<<1);
   pcwdi->cname_edit_ctl = CreateTdgtSmplEdit(pTdgtBase->pti->tci.win,
         pTdgtBase->pti, ID_CWHEEL_EDIT_B, cur_x, cur_y, w,
         TDGTSEDT_DEF_H_PAD, TDGTSEDT_DEF_V_PAD, "", STYLE_NR, INVALID);
   if (pcwdi->cname_edit_ctl == NULL) {
      DestroyTidget(&pTdgtBase->pti);
      return NULL;
   }
   cur_x = pcwdi->red_msg_x;
   cur_y += pcwdi->cname_edit_ctl->pti->tci.win_info.h + 4;

   /* row 5 */

   /* csmpl_ctl */
   pcwdi->csmpl_edit_x = cur_x;
   pcwdi->csmpl_edit_y = cur_y;
   w = pcwdi->val_edit_x + pcwdi->v_edit_ctl->pti->tci.win_info.w - cur_x -
         (TDGTSEDT_DEF_H_PAD<<1) - (windowPadding<<1);
   h = end_y - cur_y - (TDGTSEDT_DEF_V_PAD<<1) - (windowPadding<<1);
   pcwdi->csmpl_ctl = CreateTdgtDraw(pTdgtBase->pti->tci.win,
         pTdgtBase->pti, ID_CWHEEL_HS_AREA, cur_x, cur_y, w, h,
         TDGTDRAW_DEF_H_PAD, TDGTDRAW_DEF_V_PAD, TGBS_LOWRED, NULL);
   if (pcwdi->hs_pixmap_ctl == NULL) {
      DestroyTidget(&pTdgtBase->pti);
      return NULL;
   }
   TdgtDrawSetRedrawCallback(pcwdi->csmpl_ctl, CSampleRedrawCallback);

   /* buttons */

   cur_x = vh_padding+windowPadding;
   cur_y = end_y + v_gap;
   pcwdi->btn_row_ctl = CreateTdgtBtnRow(pTdgtBase->pti->tci.win,
         pTdgtBase->pti, INVALID, cur_x, cur_y, TDGTBROW_DEF_H_PAD,
         TDGTBROW_DEF_V_PAD, TGBS_NORMAL, defaultFontWidth, JUST_C);
   if (pcwdi->btn_row_ctl == NULL) {
      DestroyTidget(&pTdgtBase->pti);
      return NULL;
   }
   if (!CreateColorWheelButtons(pTdgtBase)) {
      DestroyTidget(&pTdgtBase->pti);
      return NULL;
   }
   TdgtColorWheelDlgSetEntries(pTdgtBase->pti, NULL, NULL, 0, INVALID);

   /*
    * Specify that the base tidget will handle all the KeyPress events
    *         in all its children tidgets.
    */
   TdgtBaseWillHandleKeyPressEvents(pTdgtBase,
         TdgtColorWheelDlgKeyPressEvHandlerCallback);
   /*
    * Specify that the base tidget will handle all the KeyPress events
    *         in all its children tidgets.
    */
   TdgtBaseSetWmDeleteCallback(pTdgtBase, TdgtColorWheelDlgWmDeleteCallback);

   /*
    * Specify a callback function to answer some focus related questions.
    */
   TdgtBaseSetFocusCallback(pTdgtBase, TdgtColorWheelDlgFocusCallback);

   AdjustDialogBoxSize(dpy, pTdgtBase);
   TidgetSetFocusWindow(pcwdi->cname_edit_ctl->pti->tci.win);

   return pTdgtBase->pti;
}

/* --------------------- DestroyTdgtColorWheelDialogBox --------------------- */

void DestroyTdgtColorWheelDialogBox(dpy, pti)
   Display *dpy;
   TidgetInfo *pti;
{
   ColorWheelDlgInfo *pcwdi=NULL;

   if (pti == NULL) return;

   pcwdi = (ColorWheelDlgInfo*)(pti->userdata);
   if (pcwdi != NULL) {
      free(pcwdi);
   }
   DestroyTidget(&pti);
}

/* --------------------- TdgtColorWheelDlgLoop --------------------- */

static
int HandleTdgtNotifyEvent(pTdgtBase, pTdgtNtfy)
   TdgtBase *pTdgtBase;
   TdgtNtfy *pTdgtNtfy;
{
   switch (pTdgtNtfy->ctl_id) {
   case ID_CWHEEL_BTN_ADD: SetColorInAppl(pTdgtBase); break;
   case ID_CWHEEL_BTN_OK:
      if (SetColorInAppl(pTdgtBase)) {
         HideTdgtColorWheelDialogBox();
      }
      break;
   case ID_CWHEEL_BTN_CANCEL: HideTdgtColorWheelDialogBox(); break;
   }
   return FALSE;
}

int TdgtColorWheelDlgLoop(pti)
   TidgetInfo *pti;
{
   TdgtBase *pTdgtBase=(TdgtBase*)(pti->tidget);
   int which_btn_clicked=INVALID, first_time_config=TRUE;

   if (pTdgtBase == NULL) return which_btn_clicked;

   while (gColorWheelInfo.mapped) {
      XEvent xev;
      TdgtNtfy tdgt_notify;
      TidgetInfo *handling_pti=NULL;

      XNextEvent(mainDisplay, &xev);

      if (xev.type == MotionNotify) {
         /* do nothing */
      } else if (xev.type == ConfigureNotify &&
            xev.xany.window == pti->tci.win) {
         if (first_time_config) {
            first_time_config = FALSE;
            if (warpToWinCenter) {
               /* does not work because the window has been moved */
               XWarpPointer(mainDisplay, None, pti->tci.win, 0, 0, 0, 0,
                     (pti->tci.win_info.w>>1), (pti->tci.win_info.h>>1));
            }
         }
      } else if (xev.type == Expose) {
         ExposeEventHandler(&xev, FALSE);
      } else if (xev.type == ConfigureNotify &&
            xev.xany.window==mainWindow) {
         Reconfigure(FALSE);
      } else if (xev.type == KeyPress) {
         TidgetManagerHandleAllKeyPressEvent(&xev);
      } else if (IsTdgtNotifyEvent(pti, &xev, &tdgt_notify)) {
         HandleTdgtNotifyEvent(pTdgtBase, &tdgt_notify);
      } else if (IsTidgetEvent(pti, &xev, &handling_pti)) {
         TidgetEventHandler(pti, &xev, handling_pti);
      } else {
         /* anything else? */
      }
   }
   return which_btn_clicked;
}

/* --------------------- Init & Clean Up --------------------- */

int InitTdgtColorWheelDlg()
{
   return TRUE;
}

void CleanUpTdgtColorWheelDlg()
{
}

