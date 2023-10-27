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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/edit.c,v 1.108 2011/06/18 04:44:51 william Exp $
 */

#define _INCLUDE_FROM_EDIT_C_

#include "tgifdefs.h"
#include "cmdids.h"

#include "align.e"
#include "arc.e"
#include "attr.e"
#include "auxtext.e"
#include "button.e"
#include "choice.e"
#include "cmd.e"
#include "color.e"
#include "cutpaste.e"
#include "cursor.e"
#include "dialog.e"
#include "drawing.e"
#include "dup.e"
#include "edit.e"
#include "eps.e"
#include "exec.e"
#include "file.e"
#include "font.e"
#include "grid.e"
#include "group.e"
#include "mainloop.e"
#include "mainmenu.e"
#include "mark.e"
#include "menu.e"
#include "menuinfo.e"
#include "miniline.e"
#include "move.e"
#include "msg.e"
#include "names.e"
#include "navigate.e"
#include "obj.e"
#include "page.e"
#include "pattern.e"
#include "pin.e"
#include "poly.e"
#include "polygon.e"
#include "raster.e"
#include "rect.e"
#include "ruler.e"
#include "select.e"
#include "setup.e"
#include "shape.e"
#include "special.e"
#include "spline.e"
#include "stretch.e"
#include "strtbl.e"
#include "text.e"
#include "util.e"
#include "xbitmap.e"
#include "xpixmap.e"

struct SelRec *outerSelForFind=NULL;
struct SelRec *innerSelForFind=NULL;

int ignoreObjectShadowInfoInFile=TRUE;
int objShadowXOffset=2, objShadowYOffset=2;
char objShadowColorStr[MAXSTRING];

int pngExportHasTransparentColor=FALSE;

static struct ObjRec *tmpTopObj=NULL, *tmpBotObj=NULL;
static struct SelRec *tmpTopSel=NULL, *tmpBotSel=NULL;

static char *gpszSearch=NULL;
static int gnSearchLen=0;
static int gnSearchCaseSensitive=TRUE;

static int gnFoundStartCharIndex=0, gnFoundEndCharIndex=0;
static StrBlockInfo *gpFoundStartStrBlock=NULL, *gpFoundEndStrBlock=NULL;

static int convertToBezierNumSegs=50;

void CleanOuterInnerSelForFind()
{
   struct SelRec *sel_ptr=NULL, *next_sel=NULL;
 
   if (outerSelForFind != NULL) {
      for (sel_ptr=outerSelForFind; sel_ptr != NULL; sel_ptr=next_sel) {
         next_sel = sel_ptr->next;
         free(sel_ptr);
      }
      outerSelForFind = innerSelForFind = NULL;
   }
}

static
void CopyOuterSelToOuterSelForFind()
{
   struct SelRec *sel_ptr=NULL;

   CleanOuterInnerSelForFind();
   for (sel_ptr=outerSel; sel_ptr != NULL; sel_ptr=sel_ptr->next) {
      AddObjIntoSel(sel_ptr->obj, NULL, outerSelForFind, &outerSelForFind,
            &innerSelForFind);
   }
}

void CleanUpEdit()
{
   CleanOuterInnerSelForFind();
   if (gpszSearch != NULL) free(gpszSearch);
   gpszSearch = NULL;
}

void InitEdit()
{
   char *c_ptr=NULL, spec[MAXSTRING];

   if (PRTGIF && !cmdLineOpenDisplay) return;

   convertToBezierNumSegs = 50;
   if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,
         "ConvertToBezierSegments")) != NULL) {
      SetBezierConvertNumSegsValue(c_ptr);
   }
   objShadowXOffset = objShadowYOffset = 2;
   if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME, "ObjectShadowOffsets")) !=
         NULL) {
      UtilStrCpyN(spec, sizeof(spec), c_ptr);
      UtilTrimBlanks(spec);
      if (!ParseXYSpec(spec, &objShadowXOffset, &objShadowYOffset)) {
         fprintf(stderr, TgLoadString(STID_INVALID_XDEF_USE_ALT_STR),
               TOOL_NAME, "ObjectShadowOffsets", spec, "2,2");
         fprintf(stderr, "\n");
      }
   }
   strcpy(objShadowColorStr, "#c0c0c0");
   if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"ObjectShadowColor")) != NULL) {
      int new_alloc=0;

      UtilStrCpyN(spec, sizeof(spec), c_ptr);
      UtilTrimBlanks(spec);

      if (QuickFindColorIndex(NULL, spec, &new_alloc, FALSE) == INVALID) {
         fprintf(stderr, TgLoadString(STID_INVALID_XDEF_COLORXPM_GET),
               TOOL_NAME, "ObjectShadowColor", spec);
         fprintf(stderr, "\n");
         return;
      }
      UtilStrCpyN(objShadowColorStr, sizeof(objShadowColorStr), spec);
   }
   ignoreObjectShadowInfoInFile = TRUE;
   if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,
         "IgnoreObjectShadowInfoInFile")) != NULL &&
         UtilStrICmp(c_ptr, "false") == 0) {
      ignoreObjectShadowInfoInFile = FALSE;
   }
   pngExportHasTransparentColor = FALSE;
   if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,
         "PNGExportHasTransparentColor")) != NULL &&
         UtilStrICmp(c_ptr, "true") == 0) {
      pngExportHasTransparentColor = TRUE;
   }
   tighterStructSplines = TRUE;
}

int CanPerformImageProc()
{
   struct SelRec *sel_ptr=NULL;

   if (topSel == NULL) return FALSE;

   for (sel_ptr=topSel; sel_ptr != NULL; sel_ptr=sel_ptr->next) {
      if (sel_ptr->obj->type != OBJ_XPM) return FALSE;
   }
   return TRUE;
   /*
    * if (topSel != NULL && topSel == botSel && topSel->obj->type == OBJ_XPM) {
    *    return !IsLinkedJpegObj(topSel->obj);
    * }
    * return FALSE;
    */
}

/* --------------------- ConvertIntSpline() --------------------- */

static
int ConvertObjIntSpline(ObjPtr)
   struct ObjRec *ObjPtr;
{
   register struct ObjRec *obj_ptr;
   register int i, changed=FALSE;

   switch (ObjPtr->type) {
   case OBJ_POLY:
      if (ObjPtr->detail.p->curved == LT_INTSPLINE) {
         struct PolyRec *poly_ptr=ObjPtr->detail.p;
         int new_n, n, index=0;
         char *smooth=poly_ptr->smooth;
         IntPoint *vs;

         changed = TRUE;
         if (smooth != NULL) free(smooth);
         n = poly_ptr->n;
         new_n = (n == 2 ? n : ((n-2)<<1)+n);

         vs = (IntPoint*)malloc((new_n+1)*sizeof(IntPoint));
         if (vs == NULL) FailAllocMessage();
         smooth = (char*)malloc((new_n+1)*sizeof(char));
         if (smooth == NULL) FailAllocMessage();

         smooth[0] = smooth[new_n-1] = FALSE;
         vs[0] = poly_ptr->vlist[0];
         vs[new_n-1] = poly_ptr->vlist[n-1];
         for (i=1; i < n-1; i++) {
            index++;
            smooth[index] = TRUE;
            vs[index] = poly_ptr->intvlist[(i<<1)-1];
            index++;
            smooth[index] = FALSE;
            vs[index] = poly_ptr->vlist[i];
            index++;
            smooth[index] = TRUE;
            vs[index] = poly_ptr->intvlist[i<<1];
         }
         poly_ptr->curved = LT_SPLINE;
         free(poly_ptr->vlist);
         free(poly_ptr->intvlist);
         poly_ptr->vlist = vs;
         poly_ptr->n = new_n;
         poly_ptr->smooth = smooth;
         poly_ptr->intvlist = NULL;
         poly_ptr->intn = 0;
         AdjObjSplineVs(ObjPtr);
         UpdPolyBBox(ObjPtr, poly_ptr->n, poly_ptr->vlist);
      }
      break;
   case OBJ_POLYGON:
      if (ObjPtr->detail.g->curved == LT_INTSPLINE) {
         struct PolygonRec *polygon_ptr=ObjPtr->detail.g;
         int new_n, n, index=0;
         char *smooth=polygon_ptr->smooth;
         IntPoint *vs;

         changed = TRUE;
         if (smooth != NULL) free(smooth);
         n = polygon_ptr->n;
         new_n = ((n-1)<<1)+n;

         vs = (IntPoint*)malloc((new_n+1)*sizeof(IntPoint));
         if (vs == NULL) FailAllocMessage();
         smooth = (char*)malloc((new_n+1)*sizeof(char));
         if (smooth == NULL) FailAllocMessage();

         smooth[0] = FALSE;
         vs[0] = polygon_ptr->vlist[0];
         for (i=0; i < n-1; i++) {
            index++;
            smooth[index] = TRUE;
            vs[index] = polygon_ptr->intvlist[i<<1];
            index++;
            smooth[index] = TRUE;
            vs[index] = polygon_ptr->intvlist[(i<<1)+1];
            index++;
            smooth[index] = FALSE;
            vs[index] = polygon_ptr->vlist[i+1];
         }
         polygon_ptr->curved = LT_SPLINE;
         free(polygon_ptr->vlist);
         free(polygon_ptr->intvlist);
         polygon_ptr->vlist = vs;
         polygon_ptr->n = new_n;
         polygon_ptr->smooth = smooth;
         polygon_ptr->intvlist = NULL;
         polygon_ptr->intn = 0;
         AdjObjSplineVs(ObjPtr);
         UpdPolyBBox(ObjPtr, polygon_ptr->n, polygon_ptr->vlist);
      }
      break;
   case OBJ_GROUP:
   case OBJ_SYM:
      for (obj_ptr=ObjPtr->detail.r->last; obj_ptr!=NULL;
            obj_ptr=obj_ptr->prev) {
         if (ConvertObjIntSpline(obj_ptr)) {
            changed = TRUE;
         }
      }
      break;
   }
   if (changed) AdjObjBBox(ObjPtr);
   return changed;
}

void ConvertIntSpline()
{
   struct SelRec *sel_ptr=NULL;
   int changed=FALSE;

   if (topSel == NULL) {
      MsgBox(TgLoadString(STID_NO_INTSPLINE_SELECTED), TOOL_NAME, INFO_MB);
      return;
   }
   HighLightReverse();
   StartCompositeCmd();
   for (sel_ptr=botSel; sel_ptr != NULL; sel_ptr=sel_ptr->prev) {
      PrepareToReplaceAnObj(sel_ptr->obj);
      if (ConvertObjIntSpline(sel_ptr->obj)) {
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
      UpdSelBBox();
      justDupped = FALSE;
      Msg(TgLoadString(STID_INTSPLINE_CONVERTED_TO_SPLINE));
   } else {
      HighLightForward();
      MsgBox(TgLoadString(STID_NO_INTSPLINE_SELECTED), TOOL_NAME, INFO_MB);
      return;
   }
   HighLightForward();
}

/* --------------------- ConvertToBezier() --------------------- */

static
void SetBezierPoints(n, vs, new_n, bezier_vs)
   int n, new_n;
   IntPoint *vs, *bezier_vs;
{
   int i=0, *n_choose_i=(int*)malloc((n+1)*sizeof(int)), index=0;
   double t=(double)0, dt=(((double)1.0)/((double)(new_n-1)));
   double final_t=((double)1)+(dt/((double)2));

   if (n_choose_i == NULL) FailAllocMessage();
   memset(n_choose_i, 0, (n+1)*sizeof(int));

   n--;
   n_choose_i[0] = 1;
   for (i=1; i <= n; i++) {
      n_choose_i[i] = n_choose_i[i-1] * (n-i+1) / i;
   }
   for (t=(double)0; t <= final_t; t+=dt, index++) {
      double s=((double)1)-t, xt=(double)0, yt=(double)0;

      for (i=0; i <= n; i++) {
         /* Bi = B_{i,n}(t) */
         double Bi=((double)(n_choose_i[i]));
         int j=0;

         for (j=1; j <= (n-i); j++) Bi *= s;
         for (j=1; j <= i; j++) Bi *= t;
         xt += Bi * ((double)(vs[i].x));
         yt += Bi * ((double)(vs[i].y));
      }
      bezier_vs[index].x = round(xt);
      bezier_vs[index].y = round(yt);
   }
   free(n_choose_i);
}

static
int ConvertObjToBezier(ObjPtr)
   struct ObjRec *ObjPtr;
{
   struct ObjRec *obj_ptr=NULL;
   int i=0, changed=FALSE;

   switch (ObjPtr->type) {
   case OBJ_POLY:
      if (ObjPtr->detail.p->curved == LT_STRAIGHT ||
            ObjPtr->detail.p->curved == LT_SPLINE) {
         struct PolyRec *poly_ptr=ObjPtr->detail.p;
         int n=poly_ptr->n;
         char *smooth=poly_ptr->smooth;

         if (n == 3) {
            if (!smooth[1]) {
               smooth[1] = TRUE;
               changed = TRUE;
               poly_ptr->curved = LT_SPLINE;
               AdjObjSplineVs(ObjPtr);
               UpdPolyBBox(ObjPtr, poly_ptr->n, poly_ptr->vlist);
            }
         } else if (n > 3) {
            int new_n=convertToBezierNumSegs+1;
            IntPoint *vs=NULL;

            changed = TRUE;
            if (smooth != NULL) free(smooth);

            vs = (IntPoint*)malloc((new_n+1)*sizeof(IntPoint));
            if (vs == NULL) FailAllocMessage();
            memset(vs, 0, (new_n+1)*sizeof(IntPoint));
            smooth = (char*)malloc((new_n+1)*sizeof(char));
            if (smooth == NULL) FailAllocMessage();
            memset(smooth, 0, (new_n+1)*sizeof(char));

            SetBezierPoints(n, poly_ptr->vlist, new_n, vs);

            smooth[0] = smooth[new_n-1] = FALSE;
            for (i=1; i < new_n-1; i++) smooth[i] = TRUE;
            poly_ptr->curved = LT_SPLINE;
            free(poly_ptr->vlist);
            poly_ptr->vlist = vs;
            poly_ptr->n = new_n;
            poly_ptr->smooth = smooth;
            AdjObjSplineVs(ObjPtr);
            UpdPolyBBox(ObjPtr, poly_ptr->n, poly_ptr->vlist);
         }
      }
      break;
   case OBJ_GROUP:
   case OBJ_SYM:
      for (obj_ptr=ObjPtr->detail.r->last; obj_ptr!=NULL;
            obj_ptr=obj_ptr->prev) {
         if (ConvertObjToBezier(obj_ptr)) {
            changed = TRUE;
         }
      }
      break;
   }
   if (changed) AdjObjBBox(ObjPtr);
   return changed;
}

void ConvertToBezier()
{
   struct SelRec *sel_ptr=NULL;
   int changed=FALSE;

   if (topSel == NULL) {
      MsgBox(TgLoadString(STID_NO_SPLINE_SELECTED), TOOL_NAME, INFO_MB);
      return;
   }
   HighLightReverse();
   StartCompositeCmd();
   for (sel_ptr=botSel; sel_ptr != NULL; sel_ptr=sel_ptr->prev) {
      PrepareToReplaceAnObj(sel_ptr->obj);
      if (ConvertObjToBezier(sel_ptr->obj)) {
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
      UpdSelBBox();
      justDupped = FALSE;
      Msg(TgLoadString(STID_SPLINE_CONVERTED_TO_BEZIER));
   } else {
      HighLightForward();
      MsgBox(TgLoadString(STID_NO_SPLINE_SELECTED), TOOL_NAME, INFO_MB);
      return;
   }
   HighLightForward();
}

int SetBezierConvertNumSegsValue(spec)
   char *spec;
{
   int ival=0;

   if (sscanf(spec, "%d", &ival) != 1) {
      sprintf(gszMsgBox, TgLoadCachedString(CSTID_MALFORMED_INPUT_STR), spec);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return FALSE;
   } else if (ival < 2) {
      sprintf(gszMsgBox, TgLoadString(STID_ENT_VAL_RANGE_ENTER_GE_INT),
            spec, 3);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return FALSE;
   }
   convertToBezierNumSegs = ival;

   return TRUE;
}

void SetBezierConvertNumSegs(psz_num_segs)
   char *psz_num_segs;
{
   char spec[MAXSTRING+1];

   *spec = '\0';
   if (psz_num_segs != NULL && strcmp(psz_num_segs, "-1") != 0) {
      UtilStrCpyN(spec, sizeof(spec), psz_num_segs);
   } else {
      sprintf(gszMsgBox, TgLoadString(STID_ENTER_BEZIER_NUM_SEGS_CUR_IS),
            convertToBezierNumSegs);
      if (Dialog(gszMsgBox, NULL, spec) == INVALID) return;
   }
   UtilTrimBlanks(spec);
   if (*spec == '\0') return;

   if (SetBezierConvertNumSegsValue(spec)) {
      sprintf(gszMsgBox, TgLoadString(STID_BEZIER_NUM_SEGS_SET_TO_INT),
            convertToBezierNumSegs);
      Msg(gszMsgBox);
   }
}

static
void SelectModeToggleSmoothHinge()
{
   register struct ObjRec *obj_ptr;
   struct PolyRec *poly_ptr=NULL;
   struct PolygonRec *polygon_ptr=NULL;
   int index, n, pt_toggled=FALSE, toggling=TRUE;
   int root_x, root_y, old_x, old_y, curved=(-1);
   unsigned int status;
   Window root_win, child_win;
   XEvent input, ev;
   char *smooth=NULL;

   if (!(topSel != NULL && topSel == botSel &&
         (topSel->obj->type == OBJ_POLY || topSel->obj->type == OBJ_POLYGON))) {
      MsgBox(TgLoadString(STID_SELECT_ONLY_ONE_POLY_POLYGON),
            TOOL_NAME, INFO_MB);
      return;
   }
   obj_ptr = topSel->obj;
   switch (obj_ptr->type) {
   case OBJ_POLY:
      poly_ptr = obj_ptr->detail.p;
      smooth = poly_ptr->smooth;
      curved = poly_ptr->curved;
      break;
   case OBJ_POLYGON:
      polygon_ptr = obj_ptr->detail.g;
      smooth = polygon_ptr->smooth;
      curved = polygon_ptr->curved;
      break;
   }
   if (curved == LT_INTSPLINE) {
      MsgBox(TgLoadString(STID_CANNOT_TOGGLE_FOR_INTSPLINE), TOOL_NAME,
            INFO_MB);
      return;
   } else if (curved == LT_STRUCT_SPLINE) {
      MsgBox(TgLoadString(STID_CANNOT_TOGGLE_FOR_STRUCT_SPLN), TOOL_NAME,
            INFO_MB);
      return;
   }
   if (smooth == NULL) {
      FatalUnexpectedError(TgLoadString(STID_BAD_POLY_IN_TOGGLE_SMOOTH), NULL);
      return;
   }
   PrepareToRecord(CMD_REPLACE, topSel, botSel, numObjSelected);
   SaveStatusStrings();
   SetMouseStatus(TgLoadCachedString(CSTID_TOGGLE_SMOOTH_HINGE),
         TgLoadCachedString(CSTID_FINISH), TgLoadCachedString(CSTID_FINISH));
   TwoLineMsg(TgLoadString(STID_CLICK_LEFT_BUTTON_TO_TOGGLE),
         TgLoadString(STID_CLICK_OTHER_BUTTON_TO_QUIT));

   if (!debugNoPointerGrab) {
      XGrabPointer(mainDisplay, drawWindow, False,
            PointerMotionMask | ButtonPressMask | ButtonReleaseMask,
            GrabModeAsync, GrabModeAsync, None, defaultCursor, CurrentTime);
   }
   XQueryPointer(mainDisplay, drawWindow, &root_win, &child_win,
         &root_x, &root_y, &old_x, &old_y, &status);
   XSetFont(mainDisplay, revDefaultGC, defaultFontPtr->fid);
   /* do not translate -- program constants */
   XDrawString(mainDisplay, drawWindow, revDefaultGC,
         old_x+4, old_y+defaultFontAsc, "S/H", 3);
   MarkRulers(old_x, old_y);

   while (toggling) {
      XNextEvent(mainDisplay, &input);

      if (input.type == Expose || input.type == VisibilityNotify) {
         ExposeEventHandler(&input, TRUE);
      } else if (input.type == ButtonPress) {
         if (input.xbutton.button == Button1) {
            if ((obj_ptr->type == OBJ_POLY &&
                  PtInPolyMark(obj_ptr, input.xbutton.x, input.xbutton.y,
                  poly_ptr->n, poly_ptr->vlist, &index) &&
                  index != 0 && index != poly_ptr->n-1) ||
                  (obj_ptr->type == OBJ_POLYGON &&
                  PtInPolyMark(obj_ptr, input.xbutton.x, input.xbutton.y,
                  polygon_ptr->n-1, polygon_ptr->vlist, &index))) {
               int sel_ltx=selLtX, sel_lty=selLtY;
               int sel_rbx=selRbX, sel_rby=selRbY;

               pt_toggled = TRUE;
               HighLightReverse();
               switch (obj_ptr->type) {
               case OBJ_POLY:
                  n = poly_ptr->n;
                  if (smooth[index]) {
                     smooth[index] = FALSE;
                  } else {
                     smooth[index] = TRUE;
                  }
                  AdjObjSplineVs(obj_ptr);
                  UpdPolyBBox(obj_ptr, poly_ptr->n, poly_ptr->vlist);
                  break;
               case OBJ_POLYGON:
                  n = polygon_ptr->n;
                  if (smooth[index]) {
                     smooth[index] = FALSE;
                  } else {
                     smooth[index] = TRUE;
                  }
                  if (index == 0) smooth[n-1] = smooth[0];
                  AdjObjSplineVs(obj_ptr);
                  UpdPolyBBox(obj_ptr, polygon_ptr->n, polygon_ptr->vlist);
                  break;
               }
               AdjObjBBox(obj_ptr);

               XDrawString(mainDisplay, drawWindow, revDefaultGC, old_x+4,
                     old_y+defaultFontAsc, "S/H", 3);
               old_x = input.xbutton.x;
               old_y = input.xbutton.y;
               UpdSelBBox();
               RedrawAreas(botObj,
                     sel_ltx-GRID_ABS_SIZE(1), sel_lty-GRID_ABS_SIZE(1),
                     sel_rbx+GRID_ABS_SIZE(1), sel_rby+GRID_ABS_SIZE(1),
                     selLtX-GRID_ABS_SIZE(1), selLtY-GRID_ABS_SIZE(1),
                     selRbX+GRID_ABS_SIZE(1), selRbY+GRID_ABS_SIZE(1));
               HighLightForward();
               if (obj_ptr != NULL) {
                  XDrawString(mainDisplay, drawWindow, revDefaultGC,
                        old_x+4, old_y+defaultFontAsc, "S/H", 3);
               }
               SetFileModified(TRUE);
               justDupped = FALSE;
            }
         } else {
            XUngrabPointer(mainDisplay, CurrentTime);
            Msg("");
            toggling = FALSE;
            XDrawString(mainDisplay, drawWindow, revDefaultGC,
                  old_x+4, old_y+defaultFontAsc, "S/H", 3);
         }
      } else if (input.type == MotionNotify) {
         XDrawString(mainDisplay, drawWindow, revDefaultGC,
               old_x+4, old_y+defaultFontAsc, "S/H", 3);
         old_x = input.xmotion.x;
         old_y = input.xmotion.y;
         XDrawString(mainDisplay, drawWindow, revDefaultGC,
               old_x+4, old_y+defaultFontAsc, "S/H", 3);
         MarkRulers(old_x, old_y);
         while (XCheckMaskEvent(mainDisplay, PointerMotionMask, &ev)) ;
      }
   }
   RestoreStatusStrings();
   if (pt_toggled) {
      RecordCmd(CMD_REPLACE, NULL, topSel, botSel, numObjSelected);
   } else {
      AbortPrepareCmd(CMD_REPLACE);
   }
}

void ToggleSmoothHinge()
{
   register int i;
   int changed=FALSE, ltx=selLtX, lty=selLtY, rbx=selRbX, rby=selRbY;
   struct VSelRec *vsel_ptr;
   struct ObjRec *obj_ptr;

   if (curChoice == NOTHING) {
      SelectModeToggleSmoothHinge();
      return;
   }
   if (curChoice != VERTEXMODE) {
      MsgBox(TgLoadString(STID_ONLY_TOGGLE_SMOOTH_IN_MODES), TOOL_NAME,
            INFO_MB);
      return;
   }
   if (topVSel == NULL) {
      if (topSel == NULL) return;
      HighLightReverse();
      JustRemoveAllVSel();
      HighLightForward();
      SelectModeToggleSmoothHinge();
      return;
   }
   for (vsel_ptr=topVSel; vsel_ptr != NULL; vsel_ptr=vsel_ptr->next) {
      if ((vsel_ptr->obj->type == OBJ_POLY &&
            vsel_ptr->obj->detail.p->curved == LT_INTSPLINE) ||
            (vsel_ptr->obj->type == OBJ_POLYGON &&
            vsel_ptr->obj->detail.g->curved == LT_INTSPLINE)) {
         MsgBox(TgLoadString(STID_CANNOT_TOGGLE_FOR_INTSPLINE), TOOL_NAME,
               INFO_MB);
         return;
      } else if ((vsel_ptr->obj->type == OBJ_POLY &&
            vsel_ptr->obj->detail.p->curved == LT_STRUCT_SPLINE) ||
            (vsel_ptr->obj->type == OBJ_POLYGON &&
            vsel_ptr->obj->detail.g->curved == LT_STRUCT_SPLINE)) {
         MsgBox(TgLoadString(STID_CANNOT_TOGGLE_FOR_STRUCT_SPLN), TOOL_NAME,
               INFO_MB);
         return;
      }
   }
   HighLightReverse();
   StartCompositeCmd();
   for (vsel_ptr=botVSel; vsel_ptr != NULL; vsel_ptr=vsel_ptr->prev) {
      int obj_changed=FALSE, n;
      IntPoint *v;
      char *smooth=NULL;

      obj_ptr = vsel_ptr->obj;
      switch (obj_ptr->type) {
      case OBJ_POLY:
         v = obj_ptr->detail.p->vlist;
         n = obj_ptr->detail.p->n;
         smooth = obj_ptr->detail.p->smooth;
         for (i=0; i < vsel_ptr->n; i++) {
            if (vsel_ptr->v_index[i] != 0 ||
                  vsel_ptr->v_index[i] != n-1) {
               break;
            }
         }
         if (i == vsel_ptr->n) continue;
         break;
      case OBJ_POLYGON:
         v = obj_ptr->detail.g->vlist;
         n = obj_ptr->detail.g->n;
         smooth = obj_ptr->detail.g->smooth;
         break;
      default: continue;
      }
      PrepareToReplaceAnObj(obj_ptr);
      for (i=0; i < vsel_ptr->n; i++) {
         int index=vsel_ptr->v_index[i];

         if (!(obj_ptr->type == OBJ_POLY && (index == 0 || index == n-1))) {
            smooth[index] = (smooth[index] ? FALSE : TRUE);
            obj_changed = TRUE;
         }
      }
      AdjObjSplineVs(obj_ptr);
      UpdPolyBBox(obj_ptr, n, v);
      if (obj_changed) {
         RecordReplaceAnObj(obj_ptr);
         changed = TRUE;
      } else {
         AbortPrepareCmd(CMD_REPLACE);
      }
   }
   EndCompositeCmd();
   if (changed) {
      Msg(TgLoadString(STID_SMOOTHNESS_TOGGLED));
      UpdSelBBox();
      RedrawAreas(botObj, ltx-GRID_ABS_SIZE(1), lty-GRID_ABS_SIZE(1),
         rbx+GRID_ABS_SIZE(1), rby+GRID_ABS_SIZE(1),
         selLtX-GRID_ABS_SIZE(1), selLtY-GRID_ABS_SIZE(1),
         selRbX+GRID_ABS_SIZE(1), selRbY+GRID_ABS_SIZE(1));
      SetFileModified(TRUE);
      justDupped = FALSE;
   }
   HighLightForward();
}

static
void BreakATextObj(ObjPtr, how, poli)
   struct ObjRec *ObjPtr;
   int how;
   ObjListInfo *poli;
{
   struct ObjRec *prototype=NULL;
   struct TextRec *text_ptr=NULL;
   struct SelRec *sel_ptr=NULL;
   MiniLinesInfo *minilines=NULL;
   int tx_to_move=0, ty_to_move=0;
   char proto_buf[3];

   prototype = DupObj(ObjPtr);
   text_ptr = prototype->detail.t;
   InvalidateTextCache(text_ptr);
   minilines = (&text_ptr->minilines);
   FreeMiniLines(minilines, FALSE);
   /* do not translate -- program constants */
   strcpy(proto_buf, "XX");
   CreateMiniLineFromString(proto_buf, &minilines->first, &minilines->last);
   minilines->first->owner_minilines = minilines;
   text_ptr->lines = 1;

   text_ptr = ObjPtr->detail.t;
   minilines = (&text_ptr->minilines);

   PushCurFont();
   ObjFontInfoToCurFontInfo(text_ptr);
   SetCanvasFont();

   if (ObjPtr->ctm != NULL) {
      TransformPointThroughCTM(0, 0, prototype->ctm, &tx_to_move, &ty_to_move);
   }
   BreakMiniLines(minilines, how, ObjPtr->x, text_ptr->baseline_y,
         prototype, tx_to_move, ty_to_move, poli);

   PopCurFont();
   FreeTextObj(prototype);
   for (sel_ptr=poli->top_sel; sel_ptr != NULL; sel_ptr=sel_ptr->next) {
      struct ObjRec *obj_ptr=sel_ptr->obj;

      UpdTextBBox(obj_ptr);
      AdjObjSplineVs(obj_ptr);
      AdjObjBBox(obj_ptr);
   }
}

static
int GetBreakSpec()
   /* return TRUE if breaking into words */
{
   int how=INVALID, can_do_lines=FALSE, can_do_words=FALSE;
   char spec[MAXSTRING+1];
   struct SelRec *sel_ptr=NULL;

   for (sel_ptr=topSel; sel_ptr != NULL; sel_ptr=sel_ptr->next) {
      struct ObjRec *obj_ptr=sel_ptr->obj;

      if (obj_ptr->type == OBJ_TEXT && !obj_ptr->locked) {
         struct TextRec *text_ptr=obj_ptr->detail.t;
         MiniLinesInfo *minilines=(&text_ptr->minilines);

         if (text_ptr->read_only) continue;
         if (CanBreakMiniLinesIntoWords(&text_ptr->minilines)) {
            can_do_words = TRUE;
         }
         if (minilines->first->next != NULL) can_do_lines = TRUE;
      }
      if (can_do_lines && can_do_words) break;
   }
   if (can_do_lines) {
      if (can_do_words) {
         strcpy(gszMsgBox, TgLoadString(STID_BREAK_TEXT_CWL));
      } else {
         strcpy(gszMsgBox, TgLoadString(STID_BREAK_TEXT_CL));
      }
   } else if (can_do_words) {
      strcpy(gszMsgBox, TgLoadString(STID_BREAK_TEXT_CW));
   } else {
      return BREAK_CHAR;
   }
   *spec = '\0';
   if (Dialog(gszMsgBox, NULL, spec) == INVALID) {
      return INVALID;
   }
   UtilTrimBlanks(spec);
   if (*spec == '\0') strcpy(spec, "c");
   strcpy(gszMsgBox, spec);
   gszMsgBox[1] = '\0';
   UtilStrLower(gszMsgBox);
   if (can_do_lines) {
      if (can_do_words) {
         switch (*gszMsgBox) {
         case 'c': how = BREAK_CHAR; break;
         case 'w': how = BREAK_WORD; break;
         case 'l': how = BREAK_LINE; break;
         }
      } else {
         switch (*gszMsgBox) {
         case 'c': how = BREAK_CHAR; break;
         case 'l': how = BREAK_LINE; break;
         }
      }
   } else if (can_do_words) {
      switch (*gszMsgBox) {
      case 'c': how = BREAK_CHAR; break;
      case 'w': how = BREAK_WORD; break;
      }
   }
   if (how == INVALID) {
      sprintf(gszMsgBox, TgLoadString(STID_INVALID_SPEC_ON_BREAK_TEXT), spec);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
   }
   return how;
}

static
int GetBreakSpecFromCh(spec)
   char *spec;
{
   char buf[2];
   int how=INVALID;

   buf[0] = *spec;
   buf[1] = '\0';
   UtilStrLower(buf);

   switch (*buf) {
   case 'c': how = BREAK_CHAR; break;
   case 'w': how = BREAK_WORD; break;
   case 'l': how = BREAK_LINE; break;
   }
   if (how == INVALID) {
      sprintf(gszMsgBox, TgLoadString(STID_INVALID_SPEC_ON_BREAK_TEXT), spec);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
   }
   return how;
}

void DoBreakUpText(spec, forced_justify)
   char *spec;
   int forced_justify;
{
   struct ObjRec *obj_ptr=NULL;
   struct SelRec *sel_ptr=NULL, *next_sel=NULL;
   int sel_ltx=selLtX, sel_lty=selLtY, sel_rbx=selRbX, sel_rby=selRbY;
   int how=INVALID, changed=FALSE, read_only_text_exists=FALSE;

   if (topSel == NULL) {
      MsgBox(TgLoadString(STID_NO_TEXT_OBJ_TO_BREAK_UP), TOOL_NAME, INFO_MB);
      return;
   }
   if (spec == NULL || *spec == '\0' || strcmp(spec, "-1") == 0) {
      if ((how=GetBreakSpec()) == INVALID) return;
   } else {
      if ((how=GetBreakSpecFromCh(spec)) == INVALID) return;
   }
   HighLightReverse();

   StartCompositeCmd();
   for (sel_ptr=topSel; sel_ptr != NULL; sel_ptr=next_sel) {
      next_sel = sel_ptr->next;
      obj_ptr = sel_ptr->obj;
      if (obj_ptr->type == OBJ_TEXT && !obj_ptr->locked) {
         ObjListInfo oli;

         if (obj_ptr->detail.t->read_only) {
            read_only_text_exists = TRUE;
            continue;
         }
         memset(&oli, 0, sizeof(ObjListInfo));

         changed = TRUE;

         PrepareToReplaceAnObj(obj_ptr);
         BreakATextObj(obj_ptr, how, &oli);

         oli.top_sel->obj->prev = obj_ptr->prev;
         if (obj_ptr->prev == NULL) {
            curPage->top = topObj = oli.top_sel->obj;
         } else {
            obj_ptr->prev->next = oli.top_sel->obj;
         }
         oli.bot_sel->obj->next = obj_ptr->next;
         if (obj_ptr->next == NULL) {
            curPage->bot = botObj = oli.bot_sel->obj;
         } else {
            obj_ptr->next->prev = oli.bot_sel->obj;
         }
         RecordCmd(CMD_ONE_TO_MANY, NULL, oli.top_sel, oli.bot_sel, oli.count);

         oli.top_sel->prev = sel_ptr->prev;
         if (sel_ptr->prev == NULL) {
            topSel = oli.top_sel;
         } else {
            sel_ptr->prev->next = oli.top_sel;
         }
         oli.bot_sel->next = sel_ptr->next;
         if (sel_ptr->next == NULL) {
            botSel = oli.bot_sel;
         } else {
            sel_ptr->next->prev = oli.bot_sel;
         }
         FreeObj(obj_ptr);
         free(sel_ptr);
      }
   }
   EndCompositeCmd();
   if (read_only_text_exists) {
      MsgBox(TgLoadString(STID_SOME_TEXT_NOT_BROKEN_UP_SIZE), TOOL_NAME,
            INFO_MB);
   }
   if (changed) {
      UpdSelBBox();
      RedrawAreas(botObj, selLtX-GRID_ABS_SIZE(1), selLtY-GRID_ABS_SIZE(1),
            selRbX+GRID_ABS_SIZE(1), selRbY+GRID_ABS_SIZE(1),
            sel_ltx-GRID_ABS_SIZE(1), sel_lty-GRID_ABS_SIZE(1),
            sel_rbx+GRID_ABS_SIZE(1), sel_rby+GRID_ABS_SIZE(1));
      SetFileModified(TRUE);
      justDupped = FALSE;
      Msg(TgLoadString(STID_TEXT_BROKEN_INTO_CHARS));
   } else if (!read_only_text_exists) {
      MsgBox(TgLoadString(STID_NO_TEXT_OBJ_TO_BREAK_UP), TOOL_NAME, INFO_MB);
   }
   HighLightForward();
}

void BreakUpText(spec)
   char *spec;
{
   DoBreakUpText(spec, '\0');
}

static
int DoSetTextFillPatternColor(ObjPtr)
   struct ObjRec *ObjPtr;
{
   struct ObjRec *obj_ptr=NULL;
   int changed=FALSE;

   switch (ObjPtr->type) {
   case OBJ_TEXT:
      if (ObjPtr->color != colorIndex) {
         ObjPtr->color = colorIndex;
         if (mainDisplay != NULL) {
            UtilStrCpyN(ObjPtr->color_str, sizeof(ObjPtr->color_str),
                  colorMenuItems[colorIndex]);
         }
         changed = TRUE;
      }
      break;

   case OBJ_SYM:
   case OBJ_GROUP:
   case OBJ_ICON:
      for (obj_ptr=ObjPtr->detail.r->last; obj_ptr != NULL;
            obj_ptr=obj_ptr->prev) {
         if (DoSetTextFillPatternColor(obj_ptr)) {
            changed = TRUE;
         }
      }
      break;
   case OBJ_PIN:
      obj_ptr = GetPinObj(ObjPtr);
      if (DoSetTextFillPatternColor(obj_ptr)) {
         changed = TRUE;
      }
      break;
   }
   return changed;
}

void SetTextFillPatternColor()
{
   struct SelRec *sel_ptr=NULL;
   int changed=FALSE;

   if (curChoice == DRAWTEXT) {
      if (curTextObj->color != colorIndex) {
         curTextObj->color = colorIndex;
         if (mainDisplay != NULL) {
            UtilStrCpyN(curTextObj->color_str, sizeof(curTextObj->color_str),
                  colorMenuItems[colorIndex]);
         }
         SetFileModified(TRUE);
         sprintf(gszMsgBox, TgLoadString(STID_CUR_TEXT_BG_SET_TO_NAMED),
               colorMenuItems[colorIndex]);
         Msg(gszMsgBox);
      }
      return;
   }
   if (topSel == NULL) {
      MsgBox(TgLoadCachedString(CSTID_NO_OBJ_SELECTED), TOOL_NAME, INFO_MB);
      return;
   }
   HighLightReverse();
   StartCompositeCmd();
   for (sel_ptr=topSel; sel_ptr != NULL; sel_ptr=sel_ptr->next) {
      struct ObjRec *obj_ptr=sel_ptr->obj;

      PrepareToReplaceAnObj(obj_ptr);
      if (DoSetTextFillPatternColor(obj_ptr)) {
         changed = TRUE;
         RecordReplaceAnObj(obj_ptr);
      } else {
         AbortPrepareCmd(CMD_REPLACE);
      }
   }
   EndCompositeCmd();
   if (changed) {
      RedrawAnArea(botObj,
            selLtX-GRID_ABS_SIZE(1), selLtY-GRID_ABS_SIZE(1),
            selRbX+GRID_ABS_SIZE(1), selRbY+GRID_ABS_SIZE(1));
      HighLightForward();
      SetFileModified(TRUE);
      justDupped = FALSE;
      sprintf(gszMsgBox, TgLoadString(STID_SOME_TEXT_BG_SET_TO_NAMED),
            colorMenuItems[colorIndex]);
      Msg(gszMsgBox);
   }
}

void MakeRegularPolygon()
{
   register int i;
   int vertex_at_right, xc, yc;
   int sel_ltx, sel_lty, sel_rbx, sel_rby, sides, radius;
   int ltx, lty, rbx, rby;
   double inc, angle, r;
   struct ObjRec *obj_ptr, *new_obj_ptr;
   struct PolygonRec *polygon_ptr;

   if (topSel == NULL) {
      MsgBox(TgLoadCachedString(CSTID_NO_OBJ_SELECTED), TOOL_NAME, INFO_MB);
      return;
   } else if (topSel != botSel || topSel->obj->type != OBJ_POLYGON) {
      MsgBox(TgLoadString(STID_SEL_ONE_POLYGON_TO_MAKE_REG), TOOL_NAME,
            INFO_MB);
      return;
   } else if (topSel->obj->locked) {
      MsgBox(TgLoadString(STID_POLYGON_LOCKED), TOOL_NAME, INFO_MB);
      return;
   }
   PrepareToRecord(CMD_REPLACE, topSel, botSel, numObjSelected);

   sel_ltx = selLtX; sel_lty = selLtY;
   sel_rbx = selRbX; sel_rby = selRbY;

   obj_ptr = topSel->obj;

   radius = (min(obj_ptr->obbox.rbx-obj_ptr->obbox.ltx,
         obj_ptr->obbox.rby-obj_ptr->obbox.lty))>>1;
   if (radius < 1) {
      MsgBox(TgLoadString(STID_POLYGON_TOO_SMALL_FOR_REGULAR), TOOL_NAME,
            INFO_MB);
      return;
   }
   sprintf(gszMsgBox, TgLoadString(STID_VERTEX_AT_3_OCLOCK_YNC));
   if ((vertex_at_right=MsgBox(gszMsgBox, TOOL_NAME, YNC_MB)) ==
         MB_ID_CANCEL) {
      return;
   }
   tmpTopObj = tmpBotObj = NULL;
   tmpTopSel = tmpBotSel = NULL;

   HighLightReverse();

   new_obj_ptr = DupObj(obj_ptr);

   UnlinkObj(obj_ptr);

   polygon_ptr = new_obj_ptr->detail.g;
   sides = polygon_ptr->n-1;
   inc = 2*M_PI/sides;
   angle = (vertex_at_right==MB_ID_YES) ? 0 : inc/2;

   xc = obj_ptr->obbox.ltx+radius;
   yc = obj_ptr->obbox.lty+radius;

   if ((sides%4)==0 && vertex_at_right==MB_ID_NO) {
      r = ((double)(min(obj_ptr->obbox.rbx-obj_ptr->obbox.ltx,
            obj_ptr->obbox.rby-obj_ptr->obbox.lty)) / cos(angle)) / 2;
   } else {
      r = radius;
   }
   ltx = obj_ptr->obbox.rbx;
   lty = obj_ptr->obbox.rby;
   rbx = obj_ptr->obbox.ltx;
   rby = obj_ptr->obbox.lty;
   for (i=0; i < sides; i++, angle+=inc) {
      polygon_ptr->vlist[i].x = xc + round(r*cos(angle));
      polygon_ptr->vlist[i].y = yc - round(r*sin(angle));
      if (polygon_ptr->vlist[i].x < ltx) ltx = polygon_ptr->vlist[i].x;
      if (polygon_ptr->vlist[i].y < lty) lty = polygon_ptr->vlist[i].y;
      if (polygon_ptr->vlist[i].x > rbx) rbx = polygon_ptr->vlist[i].x;
      if (polygon_ptr->vlist[i].y > rby) rby = polygon_ptr->vlist[i].y;
   }
   polygon_ptr->vlist[sides].x = polygon_ptr->vlist[0].x;
   polygon_ptr->vlist[sides].y = polygon_ptr->vlist[0].y;
   new_obj_ptr->obbox.ltx = ltx;
   new_obj_ptr->obbox.lty = lty;
   new_obj_ptr->obbox.rbx = rbx;
   new_obj_ptr->obbox.rby = rby;
   AdjObjSplineVs(new_obj_ptr);
   AdjObjBBox(new_obj_ptr);

   topSel->obj = botSel->obj = new_obj_ptr;
   AddObj(NULL, topObj, new_obj_ptr);
   RecordCmd(CMD_REPLACE, NULL, topSel, botSel, numObjSelected);
   FreeObj(obj_ptr);

   UpdSelBBox();
   RedrawAnArea(botObj, sel_ltx-GRID_ABS_SIZE(1), sel_lty-GRID_ABS_SIZE(1),
         sel_rbx+GRID_ABS_SIZE(1), sel_rby+GRID_ABS_SIZE(1));
   SetFileModified(TRUE);
   justDupped = FALSE;
   HighLightForward();
}

int DeleteStructuredSplinePoint(index, poly_ptr, polygon_ptr)
   int index; /* index into ssvlist */
   struct PolyRec *poly_ptr;
   struct PolygonRec *polygon_ptr;
   /* return FALSE if the object should be deleted */
{
   int i=0, j=0, n=0, idx=0, num_hinge_points=0, ssn=0;
   IntPoint *vs=NULL;
   StretchStructuredSplineInfo sssi;

   memset(&sssi, 0, sizeof(StretchStructuredSplineInfo));

   if (poly_ptr != NULL) {
      ssn = poly_ptr->ssn;
      n = poly_ptr->n;
      vs = poly_ptr->vlist;
      SetIPTInfoForStretchPoly(index, n, vs, &sssi);
   } else if (polygon_ptr != NULL) {
      ssn = polygon_ptr->ssn;
      n = polygon_ptr->n;
      vs = polygon_ptr->vlist;
      SetIPTInfoForStretchPolygon(index, n, vs, &sssi);
   }
   if (sssi.hinge) {
      num_hinge_points = (n+2)/3;
      if ((poly_ptr != NULL && num_hinge_points-1 < 2) ||
            (polygon_ptr != NULL && num_hinge_points-1 < 3)) {
         return FALSE;
      }
      if (!sssi.prev_valid) {
         /* first point of poly */
         if (sssi.ipt.later_valid) {
            if (ssn-2 < 2) return FALSE;
         } else {
            if (ssn-1 < 2) return FALSE;
         }
         for (i=0; i < n-3; i++) {
            vs[i].x = vs[i+3].x;
            vs[i].y = vs[i+3].y;
         }
         poly_ptr->n = n-3;
      } else if (!sssi.next_valid) {
         /* last point of poly */
         if (sssi.ipt.earlier_valid) {
            if (ssn-2 < 2) return FALSE;
         } else {
            if (ssn-1 < 2) return FALSE;
         }
         poly_ptr->n = n-3;
      } else {
         if (poly_ptr != NULL) {
            if (sssi.ipt.earlier_valid) {
               if (ssn-3 < 2) return FALSE;
            } else {
               if (ssn-1 < 2) return FALSE;
            }
            for (i=0, j=0; i < num_hinge_points; i++, j+=3) {
               if (j != sssi.orig_hinge_index) {
                  if (i == 0) {
                     vs[idx].x = vs[j].x;
                     vs[idx].y = vs[j].y;
                     vs[idx+1].x = vs[j+1].x;
                     vs[idx+1].y = vs[j+1].y;
                     idx += 2;
                  } else if (i == num_hinge_points-1) {
                     vs[idx].x = vs[j-1].x;
                     vs[idx].y = vs[j-1].y;
                     vs[idx+1].x = vs[j].x;
                     vs[idx+1].y = vs[j].y;
                     idx += 2;
                  } else {
                     vs[idx].x = vs[j-1].x;
                     vs[idx].y = vs[j-1].y;
                     vs[idx+1].x = vs[j].x;
                     vs[idx+1].y = vs[j].y;
                     vs[idx+2].x = vs[j+1].x;
                     vs[idx+2].y = vs[j+1].y;
                     idx += 3;
                  }
               }
            }
            poly_ptr->n = n-3;
         } else if (polygon_ptr != NULL) {
            if (sssi.orig_hinge_index == 0 || sssi.orig_hinge_index == n-1) {
               IntPoint v2, v3;

               if ((sssi.orig_hinge_index == 0 && sssi.ipt.later_valid) ||
                     (sssi.orig_hinge_index == n-1 && sssi.ipt.earlier_valid)) {
                  if (ssn-3 < 4) return FALSE;
               } else {
                  if (ssn-1 < 4) return FALSE;
               }
               v2.x = vs[2].x;
               v2.y = vs[2].y;
               v3.x = vs[3].x;
               v3.y = vs[3].y;
               for (j=0; j < n-5; j++) {
                  vs[j].x = vs[j+3].x;
                  vs[j].y = vs[j+3].y;
               }
               vs[n-5].x = v2.x;
               vs[n-5].y = v2.y;
               vs[n-4].x = v3.x;
               vs[n-4].y = v3.y;
            } else {
               if (sssi.ipt.later_valid) {
                  if (ssn-3 < 4) return FALSE;
               } else {
                  if (ssn-1 < 4) return FALSE;
               }
               for (i=0, j=0; i < num_hinge_points; i++, j+=3) {
                  if (j != sssi.orig_hinge_index) {
                     if (i == 0) {
                        vs[idx].x = vs[j].x;
                        vs[idx].y = vs[j].y;
                        vs[idx+1].x = vs[j+1].x;
                        vs[idx+1].y = vs[j+1].y;
                        idx += 2;
                     } else if (i == num_hinge_points-1) {
                        vs[idx].x = vs[j-1].x;
                        vs[idx].y = vs[j-1].y;
                        vs[idx+1].x = vs[j].x;
                        vs[idx+1].y = vs[j].y;
                        idx += 2;
                     } else {
                        vs[idx].x = vs[j-1].x;
                        vs[idx].y = vs[j-1].y;
                        vs[idx+1].x = vs[j].x;
                        vs[idx+1].y = vs[j].y;
                        vs[idx+2].x = vs[j+1].x;
                        vs[idx+2].y = vs[j+1].y;
                        idx += 3;
                     }
                  }
               }
            }
            polygon_ptr->n = n-3;
         }
      }
   } else {
      if (!sssi.prev_valid) {
         /* first point of poly */
         ssn--;
         if ((poly_ptr != NULL && ssn < 2) ||
               (polygon_ptr != NULL && ssn < 4)) {
            return FALSE;
         }
         vs[1].x = vs[0].x;
         vs[1].y = vs[0].y;
      } else if (!sssi.next_valid) {
         /* last point of poly */
         ssn--;
         if ((poly_ptr != NULL && ssn < 2) ||
               (polygon_ptr != NULL && ssn < 4)) {
            return FALSE;
         }
         vs[n-2].x = vs[n-1].x;
         vs[n-2].y = vs[n-1].y;
      } else {
         ssn -= 2;
         if ((poly_ptr != NULL && ssn < 2) ||
               (polygon_ptr != NULL && ssn < 4)) {
            return FALSE;
         }
         if (poly_ptr != NULL) {
            vs[sssi.orig_hinge_index-1].x = vs[sssi.orig_hinge_index].x;
            vs[sssi.orig_hinge_index-1].y = vs[sssi.orig_hinge_index].y;
            vs[sssi.orig_hinge_index+1].x = vs[sssi.orig_hinge_index].x;
            vs[sssi.orig_hinge_index+1].y = vs[sssi.orig_hinge_index].y;
         } else if (polygon_ptr != NULL) {
            if (sssi.orig_hinge_index == 0 || sssi.orig_hinge_index == n-1) {
               vs[1].x = vs[0].x;
               vs[1].y = vs[0].y;
               vs[n-2].x = vs[n-1].x;
               vs[n-2].y = vs[n-1].y;
            } else {
               vs[sssi.orig_hinge_index-1].x = vs[sssi.orig_hinge_index].x;
               vs[sssi.orig_hinge_index-1].y = vs[sssi.orig_hinge_index].y;
               vs[sssi.orig_hinge_index+1].x = vs[sssi.orig_hinge_index].x;
               vs[sssi.orig_hinge_index+1].y = vs[sssi.orig_hinge_index].y;
            }
         }
      }
   }
   return TRUE;
}

void DeletePoint()
{
   int i=0, n=0, pt_deleted=FALSE, deleting=TRUE, curved=(-1);
   int root_x=0, root_y=0, old_x=0, old_y=0, delete_obj=FALSE;
   struct ObjRec *obj_ptr=NULL;
   struct PolyRec *poly_ptr=NULL;
   struct PolygonRec *polygon_ptr=NULL;
   unsigned int status;
   Window root_win, child_win;
   IntPoint *vs=NULL;
   char *smooth=NULL;

   if (!(topSel != NULL && topSel == botSel &&
         (topSel->obj->type == OBJ_POLY || topSel->obj->type == OBJ_POLYGON))) {
      MsgBox(TgLoadString(STID_SELECT_ONLY_ONE_POLY_POLYGON), TOOL_NAME,
            INFO_MB);
      return;
   } else if (topSel->obj->locked) {
      MsgBox(TgLoadString(STID_CANNOT_DEL_PT_FOR_LOCKED), TOOL_NAME, INFO_MB);
      return;
   }
   if (curChoice == VERTEXMODE) {
      HighLightReverse();
      JustRemoveAllVSel();
      HighLightForward();
   }
   PrepareToRecord(CMD_REPLACE, topSel, botSel, numObjSelected);
   obj_ptr = topSel->obj;
   if (!GetPolyOrPolygonControlPoints(obj_ptr, &poly_ptr, &polygon_ptr, &curved,
         &n, &vs, &smooth)) {
      return;
   }
   SaveStatusStrings();
   SetMouseStatus(TgLoadCachedString(CSTID_DEL_A_VERTEX),
         TgLoadCachedString(CSTID_FINISH), TgLoadCachedString(CSTID_FINISH));
   TwoLineMsg(TgLoadString(STID_LEFT_BTN_TO_DEL_PTS),
         TgLoadString(STID_CLICK_OTHER_BUTTON_TO_QUIT));

   if (!debugNoPointerGrab) {
      XGrabPointer(mainDisplay, drawWindow, False,
            PointerMotionMask | ButtonPressMask,
            GrabModeAsync, GrabModeAsync, None, defaultCursor, CurrentTime);
   }
   XQueryPointer(mainDisplay, drawWindow, &root_win, &child_win,
         &root_x, &root_y, &old_x, &old_y, &status);
   XSetFont(mainDisplay, revDefaultGC, defaultFontPtr->fid);
   /* do not translate -- program constants */
   XDrawString(mainDisplay, drawWindow, revDefaultGC,
         old_x+4, old_y+defaultFontAsc, "DEL", 3);
   MarkRulers(old_x, old_y);

   while (deleting) {
      XEvent input;

      XNextEvent(mainDisplay, &input);

      if (input.type == Expose || input.type == VisibilityNotify) {
         ExposeEventHandler(&input, TRUE);
      } else if (input.type == ButtonPress) {
         if (input.xbutton.button == Button1) {
            int index=0;

            if (curved == LT_STRUCT_SPLINE) {
               if ((poly_ptr != NULL && PtInPolyMark(obj_ptr, input.xbutton.x,
                     input.xbutton.y, poly_ptr->ssn, poly_ptr->ssvlist,
                     &index)) || (polygon_ptr != NULL && PtInPolyMark(obj_ptr,
                     input.xbutton.x, input.xbutton.y, polygon_ptr->ssn-1,
                     polygon_ptr->ssvlist, &index))) {
                  int sel_ltx=selLtX, sel_lty=selLtY;
                  int sel_rbx=selRbX, sel_rby=selRbY;

                  /* index is the index into ssvlist */
                  pt_deleted = TRUE;
                  HighLightReverse();
                  if (DeleteStructuredSplinePoint(index, poly_ptr,
                        polygon_ptr)) {
                     AdjObjSplineVs(obj_ptr);
                     if (poly_ptr != NULL) {
                        UpdPolyBBox(obj_ptr, poly_ptr->n, poly_ptr->vlist);
                     } else if (polygon_ptr != NULL) {
                        UpdPolyBBox(obj_ptr, polygon_ptr->n,
                              polygon_ptr->vlist);
                     }
                     AdjObjBBox(obj_ptr);

                     XDrawString(mainDisplay, drawWindow, revDefaultGC, old_x+4,
                           old_y+defaultFontAsc, "DEL", 3);
                     old_x = input.xbutton.x;
                     old_y = input.xbutton.y;
                     UpdSelBBox();
                     RedrawAreas(botObj,
                           sel_ltx-GRID_ABS_SIZE(1), sel_lty-GRID_ABS_SIZE(1),
                           sel_rbx+GRID_ABS_SIZE(1), sel_rby+GRID_ABS_SIZE(1),
                           selLtX-GRID_ABS_SIZE(1), selLtY-GRID_ABS_SIZE(1),
                           selRbX+GRID_ABS_SIZE(1), selRbY+GRID_ABS_SIZE(1));
                     HighLightForward();
                     if (obj_ptr != NULL) {
                        XDrawString(mainDisplay, drawWindow, revDefaultGC,
                              old_x+4, old_y+defaultFontAsc, "DEL", 3);
                     }
                  } else {
                     XUngrabPointer(mainDisplay, CurrentTime);
                     Msg("");
                     deleting = FALSE;
                     delete_obj = TRUE;
                  }
                  SetFileModified(TRUE);
                  justDupped = FALSE;
               }
            } else if ((obj_ptr->type == OBJ_POLY &&
                  PtInPolyMark(obj_ptr, input.xbutton.x, input.xbutton.y,
                  poly_ptr->n, poly_ptr->vlist, &index)) ||
                  (obj_ptr->type == OBJ_POLYGON &&
                  PtInPolyMark(obj_ptr, input.xbutton.x, input.xbutton.y,
                  polygon_ptr->n-1, polygon_ptr->vlist, &index))) {
               pt_deleted = TRUE;
               HighLightReverse();
               if ((obj_ptr->type == OBJ_POLY && poly_ptr->n == 2) ||
                     (obj_ptr->type == OBJ_POLYGON && polygon_ptr->n == 4)) {
                  XUngrabPointer(mainDisplay, CurrentTime);
                  Msg("");
                  deleting = FALSE;
                  delete_obj = TRUE;
               } else {
                  int sel_ltx=selLtX, sel_lty=selLtY;
                  int sel_rbx=selRbX, sel_rby=selRbY;

                  switch (obj_ptr->type) {
                  case OBJ_POLY:
                     n = poly_ptr->n;
                     smooth = poly_ptr->smooth;
                     for (i = index+1; i < n; i++) {
                        poly_ptr->vlist[i-1] = poly_ptr->vlist[i];
                        if (smooth != NULL) smooth[i-1] = smooth[i];
                     }
                     if (smooth != NULL) {
                        if (index == 0) {
                           smooth[0] = FALSE;
                        } else if (index == n-1) {
                           smooth[n-2] = FALSE;
                        }
                     }
                     poly_ptr->n--;
                     AdjObjSplineVs(obj_ptr);
                     if (poly_ptr->curved != LT_INTSPLINE) {
                        UpdPolyBBox(obj_ptr, poly_ptr->n, poly_ptr->vlist);
                     } else {
                        UpdPolyBBox(obj_ptr, poly_ptr->intn,
                              poly_ptr->intvlist);
                     }
                     break;
                  case OBJ_POLYGON:
                     n = polygon_ptr->n;
                     smooth = polygon_ptr->smooth;
                     for (i = index+1; i < n; i++) {
                        polygon_ptr->vlist[i-1] = polygon_ptr->vlist[i];
                        if (smooth != NULL) smooth[i-1] = smooth[i];
                     }
                     polygon_ptr->n--;
                     n--;
                     if (index == 0) {
                        polygon_ptr->vlist[n-1] = polygon_ptr->vlist[0];
                        if (smooth != NULL) smooth[n-1] = smooth[0];
                     }
                     AdjObjSplineVs(obj_ptr);
                     if (polygon_ptr->curved != LT_INTSPLINE) {
                        UpdPolyBBox(obj_ptr, polygon_ptr->n,
                              polygon_ptr->vlist);
                     } else {
                        UpdPolyBBox(obj_ptr, polygon_ptr->intn,
                              polygon_ptr->intvlist);
                     }
                     break;
                  }
                  AdjObjBBox(obj_ptr);

                  XDrawString(mainDisplay, drawWindow, revDefaultGC, old_x+4,
                        old_y+defaultFontAsc, "DEL", 3);
                  old_x = input.xbutton.x;
                  old_y = input.xbutton.y;
                  UpdSelBBox();
                  RedrawAreas(botObj,
                        sel_ltx-GRID_ABS_SIZE(1), sel_lty-GRID_ABS_SIZE(1),
                        sel_rbx+GRID_ABS_SIZE(1), sel_rby+GRID_ABS_SIZE(1),
                        selLtX-GRID_ABS_SIZE(1), selLtY-GRID_ABS_SIZE(1),
                        selRbX+GRID_ABS_SIZE(1), selRbY+GRID_ABS_SIZE(1));
                  HighLightForward();
                  if (obj_ptr != NULL) {
                     XDrawString(mainDisplay, drawWindow, revDefaultGC,
                           old_x+4, old_y+defaultFontAsc, "DEL", 3);
                  }
               }
               SetFileModified(TRUE);
               justDupped = FALSE;
            }
         } else {
            XUngrabPointer(mainDisplay, CurrentTime);
            Msg("");
            deleting = FALSE;
            XDrawString(mainDisplay, drawWindow, revDefaultGC,
                  old_x+4, old_y+defaultFontAsc, "DEL", 3);
         }
      } else if (input.type == MotionNotify &&
            input.xany.window == drawWindow) {
         XEvent ev;

         XDrawString(mainDisplay, drawWindow, revDefaultGC,
               old_x+4, old_y+defaultFontAsc, "DEL", 3);
         old_x = input.xmotion.x;
         old_y = input.xmotion.y;
         XDrawString(mainDisplay, drawWindow, revDefaultGC,
               old_x+4, old_y+defaultFontAsc, "DEL", 3);
         MarkRulers(old_x, old_y);
         while (XCheckMaskEvent(mainDisplay, PointerMotionMask, &ev)) ;
      }
   }
   if (delete_obj) {
      DelObj(obj_ptr);
      obj_ptr = NULL;
      free(topSel);
      topSel = botSel = NULL;

      XDrawString(mainDisplay, drawWindow, revDefaultGC, old_x+4,
            old_y+defaultFontAsc, "DEL", 3);
      RedrawAnArea(botObj,
            selLtX-GRID_ABS_SIZE(1), selLtY-GRID_ABS_SIZE(1),
            selRbX+GRID_ABS_SIZE(1), selRbY+GRID_ABS_SIZE(1));
      HighLightForward();
      UpdSelBBox();
   }
   RestoreStatusStrings();
   if (pt_deleted) {
      if (topSel == NULL) {
         ChangeReplaceOneCmdToDeleteCmd();
      } else {
         RecordCmd(CMD_REPLACE, NULL, topSel, botSel, numObjSelected);
      }
   } else {
      AbortPrepareCmd(CMD_REPLACE);
   }
}

#define ADDPOINT_DRAW  (FALSE)
#define ADDPOINT_ERASE (TRUE)
#define ADDPOINT_CLICK (FALSE)
#define ADDPOINT_DRAG  (TRUE)

#define ADDPOINT_STARTSHOW 0
#define ADDPOINT_DOSHOW    1
#define ADDPOINT_ENDSHOW   2

static
void AddPointMeasureCursor(start, abs_w, abs_h, abs_x, abs_y)
   int start, abs_w, abs_h, abs_x, abs_y;
{
   char buf[MAXSTRING], w_buf[80], h_buf[80], x_buf[80], y_buf[80];

   PixelToMeasurementUnit(w_buf, abs_w);
   PixelToMeasurementUnit(h_buf, abs_h);
   PixelToMeasurementUnit(x_buf, abs_x);
   PixelToMeasurementUnit(y_buf, abs_y);
   /* do not translate -- program constants */
   sprintf(buf, "ADD: dx=%s\n     dy=%s\n     x=%s\n     y=%s",
         w_buf, h_buf, x_buf, y_buf);
   switch (start) {
   case ADDPOINT_STARTSHOW:
      StartShowMeasureCursor(OFFSET_X(abs_x), OFFSET_Y(abs_y), buf, FALSE);
      break;
   case ADDPOINT_DOSHOW:
      ShowMeasureCursor(OFFSET_X(abs_x), OFFSET_Y(abs_y), buf, FALSE);
      break;
   case ADDPOINT_ENDSHOW:
      EndShowMeasureCursor(OFFSET_X(abs_x), OFFSET_Y(abs_y), buf, FALSE);
      break;
   }
}

static
int DetermineBefore(prev_dist, next_dist, new_prev_dist, new_next_dist)
   double prev_dist, next_dist, new_prev_dist, new_next_dist;
{
   double prev_diff, next_diff;
   int before=FALSE;

   if (new_prev_dist < prev_dist) {
      if (new_next_dist < next_dist) {
         /* got closer to both previous and next vertices */
         prev_diff = prev_dist-new_prev_dist;
         next_diff = next_dist-new_next_dist;
         if (prev_diff < next_diff) {
            before = FALSE;
         } else if (prev_diff > next_diff) {
            before = TRUE;
         } else {
            before = INVALID;
         }
      } else {
         /* got closer to previous vertex */
         before = TRUE;
      }
   } else if (new_prev_dist > prev_dist) {
      if (new_next_dist > next_dist) {
         /* got further from both previous and next vertices */
         prev_diff = prev_dist-new_prev_dist;
         next_diff = next_dist-new_next_dist;
         if (prev_diff < next_diff) {
            before = FALSE;
         } else if (prev_diff > next_diff) {
            before = TRUE;
         } else {
            before = INVALID;
         }
      } else {
         /* got closer to next vertex */
         before = FALSE;
      }
   } else {
      /* same distance from previous vertex */
      if (new_next_dist < next_dist) {
         before = FALSE;
      } else if (new_next_dist > next_dist) {
         before = TRUE;
      } else {
         /* same distance from both previous and next vertices */
         before = INVALID;
      }
   }
   return before;
}

static
int CanAddSmoothPointForStructuredSpline(button, psssi)
   unsigned int button;
   StretchStructuredSplineInfo *psssi;
{
   if (button == Button2) {
      if (psssi->hinge) {
         if (!psssi->prev_valid) {
            /* first point of poly */
            if (psssi->ipt.later_valid) {
               return FALSE;
            }
         } else if (!psssi->next_valid) {
            /* last point of poly */
            if (psssi->ipt.earlier_valid) {
               return FALSE;
            }
         } else {
            if (psssi->ipt.later_valid) {
               return FALSE;
            }
         }
      } else {
         return FALSE;
      }
   }
   return TRUE;
}

typedef struct tagAddStructuredPointInfo {
   struct ObjRec *obj_ptr;
   struct PolyRec *poly_ptr;
   struct PolygonRec *polygon_ptr;
   unsigned int button;
   int index; /* index into ssvlist */
   int start_mouse_x, start_mouse_y, n, ssn, already_moved, before;
   IntPoint *vlist, *ssvlist, vs[5], vs2[5];
   char smooth[5], smooth2[5];
   int num_vs, num_vs2, sn, sn2, saved_sn, saved_sn2;
   int dx_off, dy_off, extra_smooth, extra_smooth2;
   XPoint *sv, *sv2, dashed_vs[2];
   StretchStructuredSplineInfo sssi;
   int prev_x, prev_y, prev_tx, prev_ty, x, y, tx, ty;
   int next_x, next_y, next_tx, next_ty, ruler_abs_x, ruler_abs_y;
   int orig_grid_x, orig_grid_y, grid_x, grid_y, mouse_x, mouse_y;
   int sel_ltx, sel_lty, sel_rbx, sel_rby;
   double prev_angle, next_angle, prev_dist, next_dist, ddx, ddy;
} AddStructuredPointInfo;

static
int ContinueAddStructuredPolyOrPolygonPointSetup(paspi)
   AddStructuredPointInfo *paspi;
{
   int i=0, tmp_x=0, tmp_y=0;
   IntPoint *vs=NULL, *vs2=NULL, *ssvlist=NULL;
   IntPoint *obj_ssvlist=NULL, *obj_vlist=NULL;
   char *smooth=NULL, *smooth2=NULL;
   struct ObjRec *obj_ptr=paspi->obj_ptr;
   struct PolyRec *poly_ptr=paspi->poly_ptr;
   struct PolygonRec *polygon_ptr=paspi->polygon_ptr;

   if (poly_ptr != NULL) {
      paspi->ssn = poly_ptr->ssn;
      paspi->n = poly_ptr->n;
      obj_ssvlist = poly_ptr->ssvlist;
      obj_vlist = poly_ptr->vlist;
   } else if (polygon_ptr != NULL) {
      paspi->ssn = polygon_ptr->ssn;
      paspi->n = polygon_ptr->n;
      obj_ssvlist = polygon_ptr->ssvlist;
      obj_vlist = polygon_ptr->vlist;
   }
   paspi->ssvlist = (IntPoint*)malloc((paspi->ssn+1)*sizeof(IntPoint));
   if (paspi->ssvlist == NULL) FailAllocMessage();
   for (i=0; i < paspi->ssn; i++) {
      paspi->ssvlist[i].x = obj_ssvlist[i].x;
      paspi->ssvlist[i].y = obj_ssvlist[i].y;
   }
   if (obj_ptr->ctm != NULL) {
      for (i=0; i < paspi->ssn; i++) {
         TransformPointThroughCTM(paspi->ssvlist[i].x-obj_ptr->x,
               paspi->ssvlist[i].y-obj_ptr->y, paspi->obj_ptr->ctm,
               &tmp_x, &tmp_y);
         paspi->ssvlist[i].x = obj_ptr->x+tmp_x;
         paspi->ssvlist[i].y = obj_ptr->y+tmp_y;
      }
   }
   paspi->vlist = (IntPoint*)malloc((paspi->n+1)*sizeof(IntPoint));
   if (paspi->vlist == NULL) FailAllocMessage();
   for (i=0; i < paspi->n; i++) {
      paspi->vlist[i].x = obj_vlist[i].x;
      paspi->vlist[i].y = obj_vlist[i].y;
   }
   if (obj_ptr->ctm != NULL) {
      for (i=0; i < paspi->n; i++) {
         TransformPointThroughCTM(paspi->vlist[i].x-obj_ptr->x,
               paspi->vlist[i].y-obj_ptr->y, paspi->obj_ptr->ctm,
               &tmp_x, &tmp_y);
         paspi->vlist[i].x = obj_ptr->x+tmp_x;
         paspi->vlist[i].y = obj_ptr->y+tmp_y;
      }
   }
   if (poly_ptr != NULL) {
      SetIPTInfoForStretchPoly(paspi->index, paspi->n, paspi->vlist,
            &paspi->sssi);
   } else if (polygon_ptr != NULL) {
      SetIPTInfoForStretchPolygon(paspi->index, paspi->n, paspi->vlist,
            &paspi->sssi);
   }
   if (!CanAddSmoothPointForStructuredSpline(paspi->button, &paspi->sssi)) {
      Msg(TgLoadString(STID_CANNOT_ADD_SMOOTH_VERTEX));
      return FALSE;
   }
   paspi->sel_ltx = selLtX; paspi->sel_lty = selLtY;
   paspi->sel_rbx = selRbX; paspi->sel_rby = selRbY;

   ssvlist = paspi->ssvlist;
   vs = paspi->vs;
   smooth = paspi->smooth;
   vs2 = paspi->vs2;
   smooth2 = paspi->smooth2;
   memset(smooth, 0, sizeof(5*sizeof(char)));
   memset(smooth2, 0, sizeof(5*sizeof(char)));

   paspi->x = paspi->tx = ssvlist[paspi->index].x;
   paspi->y = paspi->ty = ssvlist[paspi->index].y;
   if (paspi->sssi.hinge) {
      /* hinge point, need to wait */
   } else {
      /* must be adding a hinge point, i.e., Button1 is clicked */
      if (!paspi->sssi.prev_valid) {
         /* first point of poly */
         paspi->before = FALSE;

         paspi->num_vs = 3;
         vs[0].x = ssvlist[1].x; vs[0].y = ssvlist[1].y;
         vs[1].x = ssvlist[1].x; vs[1].y = ssvlist[1].y;
         vs[2].x = ssvlist[0].x; vs[2].y = ssvlist[0].y;
         smooth[1] = TRUE;

         vs2[0].x = ssvlist[1].x; vs2[0].y = ssvlist[1].y;
         if (paspi->sssi.ipt_next.earlier_valid) {
            paspi->num_vs2 = 3;
            vs2[1].x = paspi->sssi.ipt_next.earlier_smooth_pt.x;
            vs2[1].y = paspi->sssi.ipt_next.earlier_smooth_pt.y;
            smooth2[1] = TRUE;
         } else {
            paspi->num_vs2 = 2;
         }
         vs2[paspi->num_vs2-1].x = paspi->sssi.ipt_next.hinge_pt.x;
         vs2[paspi->num_vs2-1].y = paspi->sssi.ipt_next.hinge_pt.y;
      } else if (!paspi->sssi.next_valid) {
         /* last point of poly */
         paspi->before = TRUE;

         vs[0].x = ssvlist[paspi->ssn-2].x; vs[0].y = ssvlist[paspi->ssn-2].y;
         if (paspi->sssi.ipt_prev.later_valid) {
            paspi->num_vs = 3;
            vs[1].x = paspi->sssi.ipt_prev.later_smooth_pt.x;
            vs[1].y = paspi->sssi.ipt_prev.later_smooth_pt.y;
            smooth[1] = TRUE;
         } else {
            paspi->num_vs = 2;
         }
         vs[paspi->num_vs-1].x = paspi->sssi.ipt_prev.hinge_pt.x;
         vs[paspi->num_vs-1].y = paspi->sssi.ipt_prev.hinge_pt.y;

         paspi->num_vs2 = 3;
         vs2[0].x = ssvlist[paspi->ssn-2].x; vs2[0].y = ssvlist[paspi->ssn-2].y;
         vs2[1].x = ssvlist[paspi->ssn-2].x; vs2[1].y = ssvlist[paspi->ssn-2].y;
         vs2[2].x = ssvlist[paspi->ssn-1].x; vs2[2].y = ssvlist[paspi->ssn-1].y;
         smooth2[1] = TRUE;
      } else {
         if (paspi->sssi.earlier_smooth_selected) {
            paspi->before = TRUE;

            vs[0].x = ssvlist[paspi->index].x;
            vs[0].y = ssvlist[paspi->index].y;
            if (paspi->sssi.ipt_prev.later_valid) {
               paspi->num_vs = 3;
               vs[1].x = paspi->sssi.ipt_prev.later_smooth_pt.x;
               vs[1].y = paspi->sssi.ipt_prev.later_smooth_pt.y;
               smooth[1] = TRUE;
            } else {
               paspi->num_vs = 2;
            }
            vs[paspi->num_vs-1].x = paspi->sssi.ipt_prev.hinge_pt.x;
            vs[paspi->num_vs-1].y = paspi->sssi.ipt_prev.hinge_pt.y;

            vs2[0].x = ssvlist[paspi->index].x;
            vs2[0].y = ssvlist[paspi->index].y;
            vs2[1].x = paspi->sssi.ipt.earlier_smooth_pt.x;
            vs2[1].y = paspi->sssi.ipt.earlier_smooth_pt.y;
            vs2[2].x = paspi->sssi.ipt.hinge_pt.x;
            vs2[2].y = paspi->sssi.ipt.hinge_pt.y;
            smooth2[1] = TRUE;
            paspi->num_vs2 = 3;
         } else {
            paspi->before = FALSE;

            vs[0].x = ssvlist[paspi->index].x;
            vs[0].y = ssvlist[paspi->index].y;
            vs[1].x = paspi->sssi.ipt.later_smooth_pt.x;
            vs[1].y = paspi->sssi.ipt.later_smooth_pt.y;
            vs[2].x = paspi->sssi.ipt.hinge_pt.x;
            vs[2].y = paspi->sssi.ipt.hinge_pt.y;
            smooth[1] = TRUE;
            paspi->num_vs = 3;

            vs2[0].x = ssvlist[paspi->index].x;
            vs2[0].y = ssvlist[paspi->index].y;
            if (paspi->sssi.ipt_next.earlier_valid) {
               paspi->num_vs2 = 3;
               vs2[1].x = paspi->sssi.ipt_next.earlier_smooth_pt.x;
               vs2[1].y = paspi->sssi.ipt_next.earlier_smooth_pt.y;
               smooth2[1] = TRUE;
            } else {
               paspi->num_vs2 = 2;
            }
            vs2[paspi->num_vs2-1].x = paspi->sssi.ipt_next.hinge_pt.x;
            vs2[paspi->num_vs2-1].y = paspi->sssi.ipt_next.hinge_pt.y;
         }
      }
      paspi->already_moved = TRUE;
      paspi->sv = MakeMultiSplinePolyVertex(LT_STRUCT_SPLINE, &paspi->sn,
            paspi->smooth, drawOrigX, drawOrigY, paspi->num_vs, vs);
      paspi->sv2 = MakeMultiSplinePolyVertex(LT_STRUCT_SPLINE, &paspi->sn2,
            paspi->smooth2, drawOrigX, drawOrigY, paspi->num_vs2, vs2);
      XDrawLines(mainDisplay, drawWindow, revDefaultGC, paspi->sv, paspi->sn,
            CoordModeOrigin);
      XDrawLines(mainDisplay, drawWindow, revDefaultGC, paspi->sv2, paspi->sn2,
            CoordModeOrigin);

      GridXY(paspi->start_mouse_x, paspi->start_mouse_y,
            &paspi->orig_grid_x, &paspi->orig_grid_y);
      paspi->grid_x = paspi->orig_grid_x;
      paspi->grid_y = paspi->orig_grid_y;
      paspi->mouse_x = paspi->start_mouse_x;
      paspi->mouse_y = paspi->start_mouse_y;
      paspi->ruler_abs_x = paspi->tx;
      paspi->ruler_abs_y = paspi->ty;

      MARKHR(drawWindow, revDefaultGC, OFFSET_X(paspi->ruler_abs_x),
            OFFSET_Y(paspi->ruler_abs_y));
      AddPointMeasureCursor(ADDPOINT_STARTSHOW, 0, 0, paspi->tx, paspi->ty);

      return TRUE;
   }
   if (paspi->index == 0) {
      paspi->next_x = paspi->next_tx = paspi->ssvlist[1].x;
      paspi->next_y = paspi->next_ty = paspi->ssvlist[1].y;
      paspi->prev_x = paspi->prev_tx = (paspi->x<<1)-paspi->next_x;
      paspi->prev_y = paspi->prev_ty = (paspi->y<<1)-paspi->next_y;
   } else if (paspi->index == paspi->n-1) {
      paspi->prev_x = paspi->prev_tx = paspi->ssvlist[paspi->n-2].x;
      paspi->prev_y = paspi->prev_ty = paspi->ssvlist[paspi->n-2].y;
      paspi->next_x = paspi->next_tx = (paspi->x<<1)-paspi->prev_x;
      paspi->next_y = paspi->next_ty = (paspi->y<<1)-paspi->prev_y;
   } else {
      paspi->prev_x = paspi->prev_tx = ssvlist[paspi->index-1].x;
      paspi->prev_y = paspi->prev_ty = ssvlist[paspi->index-1].y;
      paspi->next_x = paspi->next_tx = ssvlist[paspi->index+1].x;
      paspi->next_y = paspi->next_ty = ssvlist[paspi->index+1].y;
   }
   paspi->ddx = (double)(paspi->prev_tx - paspi->tx);
   paspi->ddy = (double)(paspi->prev_ty - paspi->ty);
   paspi->prev_dist = paspi->ddx*paspi->ddx+paspi->ddy*paspi->ddy;
   paspi->ddx = (double)(paspi->next_tx - paspi->tx);
   paspi->ddy = (double)(paspi->next_ty - paspi->ty);
   paspi->next_dist = paspi->ddx*paspi->ddx+paspi->ddy*paspi->ddy;

   paspi->prev_angle = (paspi->prev_tx==paspi->tx) ?
         ((paspi->prev_ty>=paspi->ty) ? M_PI/2.0 : -M_PI/2.0) :
         atan2((double)(paspi->prev_ty-paspi->ty),
         (double)(paspi->prev_tx-paspi->tx));
   paspi->next_angle = (paspi->next_tx==paspi->tx) ?
         ((paspi->next_ty>=paspi->ty) ? M_PI/2.0 : -M_PI/2.0) :
         atan2((double)(paspi->next_ty-paspi->ty),
         (double)(paspi->next_tx-paspi->tx));
   GridXY(paspi->start_mouse_x, paspi->start_mouse_y,
         &paspi->orig_grid_x, &paspi->orig_grid_y);
   paspi->grid_x = paspi->orig_grid_x;
   paspi->grid_y = paspi->orig_grid_y;
   paspi->mouse_x = paspi->start_mouse_x;
   paspi->mouse_y = paspi->start_mouse_y;

   AddPointMeasureCursor(ADDPOINT_STARTSHOW, 0, 0, paspi->tx, paspi->ty);

   return TRUE;
}

static
void ContinueAddStructuredPolyOrPolygonPointFirstMoved(paspi)
   AddStructuredPointInfo *paspi;
{
   double new_prev_dist=(double)0, new_next_dist=(double)0;
   int new_x=paspi->ruler_abs_x, new_y=paspi->ruler_abs_y;

   paspi->already_moved = TRUE;

   paspi->ddx = (double)(paspi->prev_tx - paspi->mouse_x);
   paspi->ddy = (double)(paspi->prev_ty - paspi->mouse_y);
   new_prev_dist = paspi->ddx*paspi->ddx+paspi->ddy*paspi->ddy;
   paspi->ddx = (double)(paspi->next_tx - paspi->mouse_x);
   paspi->ddy = (double)(paspi->next_ty - paspi->mouse_y); 
   new_next_dist = paspi->ddx*paspi->ddx+paspi->ddy*paspi->ddy;

   paspi->before = DetermineBefore(paspi->prev_dist, paspi->next_dist,
         new_prev_dist, new_next_dist);
   if (paspi->before == INVALID) {
      double new_angle=(double)0, theta_1=(double)0, theta_2=(double)0;

      new_angle = (new_x==paspi->tx) ? ((new_y>=paspi->ty) ?
            M_PI/2.0 : -M_PI/2.0) : atan2((double)(new_y-paspi->ty),
            (double)(new_x-paspi->tx));
      theta_1 = fabs(paspi->prev_angle - new_angle);
      theta_2 = fabs(paspi->next_angle - new_angle);
      if (theta_1 > M_PI) theta_1 = 2*M_PI-theta_1;
      if (theta_2 > M_PI) theta_2 = 2*M_PI-theta_2;
      paspi->before = (theta_1 <= theta_2);
   }
#ifdef _TGIF_DBG /* debug, do not translate */
   TgAssert(paspi->sssi.hinge,
         "paspi->sssi.hinge is FALSE in ContinueAddStructuredPolyOrPolygonPointFirstMoved()", NULL);
#endif /* _TGIF_DBG */
   if (paspi->button == Button1) {
      IntPoint *vs=paspi->vs, *vs2=paspi->vs2;
      char *smooth=paspi->smooth, *smooth2=paspi->smooth2;

      memset(smooth, 0, sizeof(5*sizeof(char)));
      memset(smooth2, 0, sizeof(5*sizeof(char)));

      if (paspi->before) {
         /* Add a point between the current and the previous point */
         if (!paspi->sssi.prev_valid) {
            /* first point of poly */
            paspi->num_vs = 0;

            vs2[0].x = new_x;
            vs2[0].y = new_y;
            if (paspi->sssi.ipt.later_valid) {
               paspi->num_vs2 = 3;
               vs2[1].x = (paspi->sssi.ipt.hinge_pt.x<<1) -
                     paspi->sssi.ipt.later_smooth_pt.x;
               vs2[1].y = (paspi->sssi.ipt.hinge_pt.y<<1) -
                     paspi->sssi.ipt.later_smooth_pt.y;
               smooth2[1] = TRUE;
               paspi->extra_smooth2 = TRUE;
               MARKHO(drawWindow, revDefaultGC, OFFSET_X(vs2[1].x),
                     OFFSET_Y(vs2[1].y));
            } else {
               paspi->num_vs2 = 2;
            }
            vs2[paspi->num_vs2-1].x = paspi->sssi.ipt.hinge_pt.x;
            vs2[paspi->num_vs2-1].y = paspi->sssi.ipt.hinge_pt.y;
         } else if (!paspi->sssi.next_valid) {
            /* last point of poly */
            vs[0].x = new_x;
            vs[0].y = new_y;
            if (paspi->sssi.ipt.earlier_valid) {
               paspi->num_vs = 3;
               vs[1].x = paspi->sssi.ipt.earlier_smooth_pt.x;
               vs[1].y = paspi->sssi.ipt.earlier_smooth_pt.y;
               smooth[1] = TRUE;
            } else {
               paspi->num_vs = 2;
            }
            vs[paspi->num_vs-1].x = paspi->sssi.ipt.hinge_pt.x;
            vs[paspi->num_vs-1].y = paspi->sssi.ipt.hinge_pt.y;

            vs2[0].x = new_x;
            vs2[0].y = new_y;
            if (paspi->sssi.ipt_prev.later_valid) {
               paspi->num_vs2 = 3;
               vs2[1].x = paspi->sssi.ipt_prev.later_smooth_pt.x;
               vs2[1].y = paspi->sssi.ipt_prev.later_smooth_pt.y;
               smooth2[1] = TRUE;
            } else {
               paspi->num_vs2 = 2;
            }
            vs2[paspi->num_vs2-1].x = paspi->sssi.ipt_prev.hinge_pt.x;
            vs2[paspi->num_vs2-1].y = paspi->sssi.ipt_prev.hinge_pt.y;
         } else {
            vs[0].x = new_x;
            vs[0].y = new_y;
            if (paspi->sssi.ipt_prev.later_valid) {
               paspi->num_vs = 3;
               vs[1].x = paspi->sssi.ipt_prev.later_smooth_pt.x;
               vs[1].y = paspi->sssi.ipt_prev.later_smooth_pt.y;
               smooth[1] = TRUE;
            } else {
               paspi->num_vs = 2;
            }
            vs[paspi->num_vs-1].x = paspi->sssi.ipt_prev.hinge_pt.x;
            vs[paspi->num_vs-1].y = paspi->sssi.ipt_prev.hinge_pt.y;

            vs2[0].x = new_x;
            vs2[0].y = new_y;
            if (paspi->sssi.ipt.earlier_valid) {
               paspi->num_vs2 = 3;
               vs2[1].x = paspi->sssi.ipt.earlier_smooth_pt.x;
               vs2[1].y = paspi->sssi.ipt.earlier_smooth_pt.y;
               smooth2[1] = TRUE;
            } else {
               paspi->num_vs2 = 2;
            }
            vs2[paspi->num_vs2-1].x = paspi->sssi.ipt.hinge_pt.x;
            vs2[paspi->num_vs2-1].y = paspi->sssi.ipt.hinge_pt.y;
         }
      } else {
         /* Add a point between the current and the next point */
         if (!paspi->sssi.prev_valid) {
            /* first point of poly */
            vs[0].x = new_x;
            vs[0].y = new_y;
            if (paspi->sssi.ipt.later_valid) {
               paspi->num_vs = 3;
               vs[1].x = paspi->sssi.ipt.later_smooth_pt.x;
               vs[1].y = paspi->sssi.ipt.later_smooth_pt.y;
               smooth[1] = TRUE;
            } else {
               paspi->num_vs = 2;
            }
            vs[paspi->num_vs-1].x = paspi->sssi.ipt.hinge_pt.x;
            vs[paspi->num_vs-1].y = paspi->sssi.ipt.hinge_pt.y;

            vs2[0].x = new_x;
            vs2[0].y = new_y;
            if (paspi->sssi.ipt_next.earlier_valid) {
               paspi->num_vs2 = 3;
               vs2[1].x = paspi->sssi.ipt_next.earlier_smooth_pt.x;
               vs2[1].y = paspi->sssi.ipt_next.earlier_smooth_pt.y;
               smooth2[1] = TRUE;
            } else {
               paspi->num_vs2 = 2;
            }
            vs2[paspi->num_vs2-1].x = paspi->sssi.ipt_next.hinge_pt.x;
            vs2[paspi->num_vs2-1].y = paspi->sssi.ipt_next.hinge_pt.y;
         } else if (!paspi->sssi.next_valid) {
            /* last point of poly */
            vs[0].x = new_x;
            vs[0].y = new_y;
            if (paspi->sssi.ipt.earlier_valid) {
               paspi->num_vs = 3;
               vs[1].x = (paspi->sssi.ipt.hinge_pt.x<<1) -
                     paspi->sssi.ipt.earlier_smooth_pt.x;
               vs[1].y = (paspi->sssi.ipt.hinge_pt.y<<1) -
                     paspi->sssi.ipt.earlier_smooth_pt.y;
               smooth[1] = TRUE;
               paspi->extra_smooth = TRUE;
               MARKHO(drawWindow, revDefaultGC, OFFSET_X(vs[1].x),
                     OFFSET_Y(vs[1].y));
            } else {
               paspi->num_vs = 2;
            }
            vs[paspi->num_vs-1].x = paspi->sssi.ipt.hinge_pt.x;
            vs[paspi->num_vs-1].y = paspi->sssi.ipt.hinge_pt.y;

            paspi->num_vs2 = 0;
         } else {
            vs[0].x = new_x;
            vs[0].y = new_y;
            if (paspi->sssi.ipt.later_valid) {
               paspi->num_vs = 3;
               vs[1].x = paspi->sssi.ipt.later_smooth_pt.x;
               vs[1].y = paspi->sssi.ipt.later_smooth_pt.y;
               smooth[1] = TRUE;
            } else {
               paspi->num_vs = 2;
            }
            vs[paspi->num_vs-1].x = paspi->sssi.ipt.hinge_pt.x;
            vs[paspi->num_vs-1].y = paspi->sssi.ipt.hinge_pt.y;

            vs2[0].x = new_x;
            vs2[0].y = new_y;
            if (paspi->sssi.ipt_next.earlier_valid) {
               paspi->num_vs2 = 3;
               vs2[1].x = paspi->sssi.ipt_next.earlier_smooth_pt.x;
               vs2[1].y = paspi->sssi.ipt_next.earlier_smooth_pt.y;
               smooth2[1] = TRUE;
            } else {
               paspi->num_vs2 = 2;
            }
            vs2[paspi->num_vs2-1].x = paspi->sssi.ipt_next.hinge_pt.x;
            vs2[paspi->num_vs2-1].y = paspi->sssi.ipt_next.hinge_pt.y;
         }
      }
   } else {
      IntPoint *vs=paspi->vs, *vs2=paspi->vs2;
      char *smooth=paspi->smooth, *smooth2=paspi->smooth2;

      memset(smooth, 0, sizeof(5*sizeof(char)));
      memset(smooth2, 0, sizeof(5*sizeof(char)));

      if (!paspi->sssi.prev_valid) {
         /* first point of poly */
         paspi->num_vs = 0;

         vs2[0].x = paspi->sssi.ipt.hinge_pt.x;
         vs2[0].y = paspi->sssi.ipt.hinge_pt.y;
         vs2[1].x = new_x;
         vs2[1].y = new_y;
         smooth2[1] = TRUE;
         if (paspi->sssi.ipt_next.earlier_valid) {
            paspi->num_vs2 = 4;
            vs2[2].x = paspi->sssi.ipt_next.earlier_smooth_pt.x;
            vs2[2].y = paspi->sssi.ipt_next.earlier_smooth_pt.y;
            smooth2[2] = TRUE;
         } else {
            paspi->num_vs2 = 3;
         }
         vs2[paspi->num_vs2-1].x = paspi->sssi.ipt_next.hinge_pt.x;
         vs2[paspi->num_vs2-1].y = paspi->sssi.ipt_next.hinge_pt.y;
      } else if (!paspi->sssi.next_valid) {
         /* last point of poly */
         vs[0].x = paspi->sssi.ipt.hinge_pt.x;
         vs[0].y = paspi->sssi.ipt.hinge_pt.y;
         vs[1].x = new_x;
         vs[1].y = new_y;
         smooth[1] = TRUE;
         if (paspi->sssi.ipt_prev.later_valid) {
            paspi->num_vs = 4;
            vs[2].x = paspi->sssi.ipt_prev.later_smooth_pt.x;
            vs[2].y = paspi->sssi.ipt_prev.later_smooth_pt.y;
            smooth[2] = TRUE;
         } else {
            paspi->num_vs = 3;
         }
         vs[paspi->num_vs-1].x = paspi->sssi.ipt_prev.hinge_pt.x;
         vs[paspi->num_vs-1].y = paspi->sssi.ipt_prev.hinge_pt.y;

         paspi->num_vs2 = 0;
      } else {
         int new_x_prime=(paspi->sssi.ipt.hinge_pt.x<<1)-new_x;
         int new_y_prime=(paspi->sssi.ipt.hinge_pt.y<<1)-new_y;

         vs[0].x = paspi->sssi.ipt.hinge_pt.x;
         vs[0].y = paspi->sssi.ipt.hinge_pt.y;
         if (paspi->before) {
            vs[1].x = new_x;
            vs[1].y = new_y;
         } else {
            vs[1].x = new_x_prime;
            vs[1].y = new_y_prime;
         }
         smooth[1] = TRUE;
         if (paspi->sssi.ipt_prev.later_valid) {
            paspi->num_vs = 4;
            vs[2].x = paspi->sssi.ipt_prev.later_smooth_pt.x;
            vs[2].y = paspi->sssi.ipt_prev.later_smooth_pt.y;
            smooth[2] = TRUE;
         } else {
            paspi->num_vs = 3;
         }
         vs[paspi->num_vs-1].x = paspi->sssi.ipt_prev.hinge_pt.x;
         vs[paspi->num_vs-1].y = paspi->sssi.ipt_prev.hinge_pt.y;

         vs2[0].x = paspi->sssi.ipt.hinge_pt.x;
         vs2[0].y = paspi->sssi.ipt.hinge_pt.y;
         if (paspi->before) {
            vs2[1].x = new_x_prime;
            vs2[1].y = new_y_prime;
         } else {
            vs2[1].x = new_x;
            vs2[1].y = new_y;
         }
         smooth2[1] = TRUE;
         if (paspi->sssi.ipt_next.earlier_valid) {
            paspi->num_vs2 = 4;
            vs2[2].x = paspi->sssi.ipt_next.earlier_smooth_pt.x;
            vs2[2].y = paspi->sssi.ipt_next.earlier_smooth_pt.y;
            smooth2[2] = TRUE;
         } else {
            paspi->num_vs2 = 3;
         }
         vs2[paspi->num_vs2-1].x = paspi->sssi.ipt_next.hinge_pt.x;
         vs2[paspi->num_vs2-1].y = paspi->sssi.ipt_next.hinge_pt.y;
      }
   }
}

static
void ContinueAddStructuredPolyOrPolygonPointRestMoved(paspi)
   AddStructuredPointInfo *paspi;
{
   if (paspi->button == Button1) {
      MARKHR(drawWindow, revDefaultGC, OFFSET_X(paspi->ruler_abs_x),
            OFFSET_Y(paspi->ruler_abs_y));
      paspi->vs[0].x = paspi->vs2[0].x = paspi->ruler_abs_x;
      paspi->vs[0].y = paspi->vs2[0].y = paspi->ruler_abs_y;
   } else {
      XGCValues values;

      XSetDashes(mainDisplay, revDefaultGC, 0, dashList[8], dashListLength[8]);
      if (!paspi->sssi.prev_valid) {
         /* first point of poly */
         paspi->vs2[1].x = paspi->ruler_abs_x;
         paspi->vs2[1].y = paspi->ruler_abs_y;
         paspi->dashed_vs[0].x = OFFSET_X(paspi->vs2[1].x);
         paspi->dashed_vs[0].y = OFFSET_Y(paspi->vs2[1].y);
         paspi->dashed_vs[1].x = OFFSET_X(paspi->vs2[0].x);
         paspi->dashed_vs[1].y = OFFSET_Y(paspi->vs2[0].y);
         MARKHO(drawWindow, revDefaultGC, paspi->dashed_vs[0].x,
               paspi->dashed_vs[0].y);

         values.line_style = LineOnOffDash;
         XChangeGC(mainDisplay, revDefaultGC, GCLineStyle, &values);
         MyDashedLine(drawWindow, revDefaultGC, paspi->dashed_vs, 2);
         values.line_style = LineSolid;
         XChangeGC(mainDisplay, revDefaultGC, GCLineStyle, &values);
      } else if (!paspi->sssi.next_valid) {
         /* last point of poly */
         paspi->vs[1].x = paspi->ruler_abs_x;
         paspi->vs[1].y = paspi->ruler_abs_y;
         paspi->dashed_vs[0].x = OFFSET_X(paspi->vs[1].x);
         paspi->dashed_vs[0].y = OFFSET_Y(paspi->vs[1].y);
         paspi->dashed_vs[1].x = OFFSET_X(paspi->vs[0].x);
         paspi->dashed_vs[1].y = OFFSET_Y(paspi->vs[0].y);
         MARKHO(drawWindow, revDefaultGC, paspi->dashed_vs[0].x,
               paspi->dashed_vs[0].y);

         values.line_style = LineOnOffDash;
         XChangeGC(mainDisplay, revDefaultGC, GCLineStyle, &values);
         MyDashedLine(drawWindow, revDefaultGC, paspi->dashed_vs, 2);
         values.line_style = LineSolid;
         XChangeGC(mainDisplay, revDefaultGC, GCLineStyle, &values);
      } else {
         if (paspi->before) {
            paspi->vs[1].x = paspi->ruler_abs_x;
            paspi->vs[1].y = paspi->ruler_abs_y;
            paspi->vs2[1].x = (paspi->vs[0].x<<1) - paspi->ruler_abs_x;
            paspi->vs2[1].y = (paspi->vs[0].y<<1) - paspi->ruler_abs_y;
            paspi->dashed_vs[0].x = OFFSET_X(paspi->vs[1].x);
            paspi->dashed_vs[0].y = OFFSET_Y(paspi->vs[1].y);
            paspi->dashed_vs[1].x = OFFSET_X(paspi->vs2[1].x);
            paspi->dashed_vs[1].y = OFFSET_Y(paspi->vs2[1].y);
            MARKHO(drawWindow, revDefaultGC, paspi->dashed_vs[0].x,
                  paspi->dashed_vs[0].y);
            MARKHO(drawWindow, revDefaultGC, paspi->dashed_vs[1].x,
                  paspi->dashed_vs[1].y);

            values.line_style = LineOnOffDash;
            XChangeGC(mainDisplay, revDefaultGC, GCLineStyle, &values);
            MyDashedLine(drawWindow, revDefaultGC, paspi->dashed_vs, 2);
            values.line_style = LineSolid;
            XChangeGC(mainDisplay, revDefaultGC, GCLineStyle, &values);
         } else {
            paspi->vs2[1].x = paspi->ruler_abs_x;
            paspi->vs2[1].y = paspi->ruler_abs_y;
            paspi->vs[1].x = (paspi->vs2[0].x<<1) - paspi->ruler_abs_x;
            paspi->vs[1].y = (paspi->vs2[0].y<<1) - paspi->ruler_abs_y;
            paspi->dashed_vs[0].x = OFFSET_X(paspi->vs2[1].x);
            paspi->dashed_vs[0].y = OFFSET_Y(paspi->vs2[1].y);
            paspi->dashed_vs[1].x = OFFSET_X(paspi->vs[1].x);
            paspi->dashed_vs[1].y = OFFSET_Y(paspi->vs[1].y);
            MARKHO(drawWindow, revDefaultGC, paspi->dashed_vs[0].x,
                  paspi->dashed_vs[0].y);
            MARKHO(drawWindow, revDefaultGC, paspi->dashed_vs[1].x,
                  paspi->dashed_vs[1].y);

            values.line_style = LineOnOffDash;
            XChangeGC(mainDisplay, revDefaultGC, GCLineStyle, &values);
            MyDashedLine(drawWindow, revDefaultGC, paspi->dashed_vs, 2);
            values.line_style = LineSolid;
            XChangeGC(mainDisplay, revDefaultGC, GCLineStyle, &values);
         }
      }
   }
   if (paspi->num_vs > 0) {
      paspi->sv = MakeMultiSplinePolyVertex(LT_STRUCT_SPLINE, &paspi->sn,
            paspi->smooth, drawOrigX, drawOrigY, paspi->num_vs, paspi->vs);
      XDrawLines(mainDisplay, drawWindow, revDefaultGC, paspi->sv, paspi->sn,
            CoordModeOrigin);
   }
   if (paspi->num_vs2 > 0) {
      paspi->sv2 = MakeMultiSplinePolyVertex(LT_STRUCT_SPLINE, &paspi->sn2,
            paspi->smooth2, drawOrigX, drawOrigY, paspi->num_vs2, paspi->vs2);
      XDrawLines(mainDisplay, drawWindow, revDefaultGC, paspi->sv2, paspi->sn2,
            CoordModeOrigin);
   }
   MarkRulers(OFFSET_X(paspi->ruler_abs_x), OFFSET_Y(paspi->ruler_abs_y));
   AddPointMeasureCursor(ADDPOINT_DOSHOW, ABS_SIZE(paspi->dx_off),
         ABS_SIZE(paspi->dy_off), paspi->ruler_abs_x, paspi->ruler_abs_y);
}

static
int ContinueAddStructuredPolyOrPolygonPointFinished(paspi)
   AddStructuredPointInfo *paspi;
{
   int i=0, tmp_x=0, tmp_y=0, new_abs_x=0, new_abs_y=0, orig_hinge_index=0;
   int ok=TRUE;
   IntPoint *obj_vlist=NULL;
   struct ObjRec *obj_ptr=paspi->obj_ptr;
   struct PolyRec *poly_ptr=paspi->poly_ptr;
   struct PolygonRec *polygon_ptr=paspi->polygon_ptr;

   if (paspi->extra_smooth) {
      MARKHO(drawWindow, revDefaultGC, OFFSET_X(paspi->vs[1].x),
            OFFSET_Y(paspi->vs[1].y));
   }
   if (paspi->extra_smooth2) {
      MARKHO(drawWindow, revDefaultGC, OFFSET_X(paspi->vs2[1].x),
            OFFSET_Y(paspi->vs2[1].y));
   }
   if (!paspi->already_moved) {
      ok = FALSE;
   } else {
      if (paspi->ruler_abs_x == paspi->sssi.ipt.hinge_pt.x &&
            paspi->ruler_abs_y == paspi->sssi.ipt.hinge_pt.y) {
         ok = FALSE;
      }
      if (paspi->before) {
         if (paspi->ruler_abs_x == paspi->sssi.ipt_prev.hinge_pt.x &&
               paspi->ruler_abs_y == paspi->sssi.ipt_prev.hinge_pt.y) {
            ok = FALSE;
         }
      } else {
         if (paspi->ruler_abs_x == paspi->sssi.ipt_next.hinge_pt.x &&
               paspi->ruler_abs_y == paspi->sssi.ipt_next.hinge_pt.y) {
            ok = FALSE;
         }
      }
   }
   if (!ok) {
      free(paspi->ssvlist);
      free(paspi->vlist);

      return FALSE;
   }
   if (poly_ptr != NULL) {
      obj_vlist = poly_ptr->vlist;
   } else if (polygon_ptr != NULL) {
      obj_vlist = polygon_ptr->vlist;
   }
   HighLightReverse();
   if (paspi->button == Button1) {
      /* add a hinge point */
      IntPoint *new_vlist=(IntPoint*)malloc((paspi->n+4)*sizeof(IntPoint));

      if (new_vlist == NULL) FailAllocMessage();
      memset(new_vlist, 0, (paspi->n+4)*sizeof(IntPoint));
      obj_ptr = paspi->obj_ptr;
      new_abs_x = paspi->ruler_abs_x;
      new_abs_y = paspi->ruler_abs_y;
      if (obj_ptr->ctm != NULL) {
         ReverseTransformPointThroughCTM(new_abs_x-obj_ptr->x,
               new_abs_y-obj_ptr->y, obj_ptr->ctm, &tmp_x, &tmp_y);
         new_abs_x = obj_ptr->x+tmp_x;
         new_abs_y = obj_ptr->y+tmp_y;
      }
      if (paspi->before) {
         if (!paspi->sssi.prev_valid) {
            /* first point of poly */
            new_vlist[0].x = new_vlist[1].x = new_abs_x;
            new_vlist[0].y = new_vlist[1].y = new_abs_y;
            if (paspi->sssi.ipt.later_valid) {
               new_vlist[2].x = (obj_vlist[0].x<<1) -
                     obj_vlist[1].x;
               new_vlist[2].y = (obj_vlist[0].y<<1) -
                     obj_vlist[1].y;
            } else {
               new_vlist[2].x = obj_vlist[0].x;
               new_vlist[2].y = obj_vlist[0].y;
            }
            for (i=0; i < paspi->n; i++) {
               new_vlist[i+3].x = obj_vlist[i].x;
               new_vlist[i+3].y = obj_vlist[i].y;
            }
         } else if (!paspi->sssi.next_valid) {
            /* last point of poly */
            for (i=0; i < paspi->n-2; i++) {
               new_vlist[i].x = obj_vlist[i].x;
               new_vlist[i].y = obj_vlist[i].y;
            }
            new_vlist[paspi->n-2].x = new_vlist[paspi->n-1].x =
                  new_vlist[paspi->n].x = new_abs_x;
            new_vlist[paspi->n-2].y = new_vlist[paspi->n-1].y =
                  new_vlist[paspi->n].y = new_abs_y;
            new_vlist[paspi->n+1].x = obj_vlist[paspi->n-2].x;
            new_vlist[paspi->n+1].y = obj_vlist[paspi->n-2].y;
            new_vlist[paspi->n+2].x = obj_vlist[paspi->n-1].x;
            new_vlist[paspi->n+2].y = obj_vlist[paspi->n-1].y;
         } else {
            orig_hinge_index = paspi->sssi.orig_hinge_index;
            if (polygon_ptr != NULL && orig_hinge_index == 0) {
               orig_hinge_index = polygon_ptr->n-1;
            }
            for (i=0; i < orig_hinge_index-1; i++) {
               new_vlist[i].x = obj_vlist[i].x;
               new_vlist[i].y = obj_vlist[i].y;
            }
            new_vlist[orig_hinge_index-1].x = new_vlist[orig_hinge_index].x =
                  new_vlist[orig_hinge_index+1].x = new_abs_x;
            new_vlist[orig_hinge_index-1].y = new_vlist[orig_hinge_index].y =
                  new_vlist[orig_hinge_index+1].y = new_abs_y;
            for (i=orig_hinge_index-1; i < paspi->n; i++) {
               new_vlist[i+3].x = obj_vlist[i].x;
               new_vlist[i+3].y = obj_vlist[i].y;
            }
         }
      } else {
         if (!paspi->sssi.prev_valid) {
            /* first point of poly */
            for (i=0; i < 2; i++) {
               new_vlist[i].x = obj_vlist[i].x;
               new_vlist[i].y = obj_vlist[i].y;
            }
            new_vlist[2].x = new_vlist[3].x = new_vlist[4].x = new_abs_x;
            new_vlist[2].y = new_vlist[3].y = new_vlist[4].y = new_abs_y;
            for (i=2; i < paspi->n; i++) {
               new_vlist[i+3].x = obj_vlist[i].x;
               new_vlist[i+3].y = obj_vlist[i].y;
            }
         } else if (!paspi->sssi.next_valid) {
            /* last point of poly */
            for (i=0; i < paspi->n; i++) {
               new_vlist[i].x = obj_vlist[i].x;
               new_vlist[i].y = obj_vlist[i].y;
            }
            if (paspi->sssi.ipt.earlier_valid) {
               new_vlist[paspi->n].x =
                     (obj_vlist[paspi->n-1].x<<1) -
                     obj_vlist[paspi->n-2].x;
               new_vlist[paspi->n].y =
                     (obj_vlist[paspi->n-1].y<<1) -
                     obj_vlist[paspi->n-2].y;
            } else {
               new_vlist[paspi->n].x = obj_vlist[paspi->n-1].x;
               new_vlist[paspi->n].y = obj_vlist[paspi->n-1].y;
            }
            new_vlist[paspi->n+1].x = new_vlist[paspi->n+2].x = new_abs_x;
            new_vlist[paspi->n+1].y = new_vlist[paspi->n+2].y = new_abs_y;
         } else {
            orig_hinge_index = paspi->sssi.orig_hinge_index;
            for (i=0; i < orig_hinge_index+2; i++) {
               new_vlist[i].x = obj_vlist[i].x;
               new_vlist[i].y = obj_vlist[i].y;
            }
            new_vlist[orig_hinge_index+2].x = new_vlist[orig_hinge_index+3].x =
                  new_vlist[orig_hinge_index+4].x = new_abs_x;
            new_vlist[orig_hinge_index+2].y = new_vlist[orig_hinge_index+3].y =
                  new_vlist[orig_hinge_index+4].y = new_abs_y;
            for (i=orig_hinge_index+2; i < paspi->n; i++) {
               new_vlist[i+3].x = obj_vlist[i].x;
               new_vlist[i+3].y = obj_vlist[i].y;
            }
         }
      }
      free(obj_vlist);
      if (poly_ptr != NULL) {
         paspi->poly_ptr->vlist = new_vlist;
         paspi->poly_ptr->n += 3;
      } else if (polygon_ptr != NULL) {
         paspi->polygon_ptr->vlist = new_vlist;
         paspi->polygon_ptr->n += 3;
      }
   } else { /* paspi->button == Button2 */
      obj_ptr = paspi->obj_ptr;
      new_abs_x = paspi->ruler_abs_x;
      new_abs_y = paspi->ruler_abs_y;
      if (obj_ptr->ctm != NULL) {
         ReverseTransformPointThroughCTM(new_abs_x-obj_ptr->x,
               new_abs_y-obj_ptr->y, obj_ptr->ctm, &tmp_x, &tmp_y);
         new_abs_x = obj_ptr->x+tmp_x;
         new_abs_y = obj_ptr->y+tmp_y;
      }
      if (!paspi->sssi.prev_valid) {
         /* first point of poly */
         obj_vlist[1].x = new_abs_x;
         obj_vlist[1].y = new_abs_y;
      } else if (!paspi->sssi.next_valid) {
         /* last point of poly */
         obj_vlist[paspi->n-2].x = new_abs_x;
         obj_vlist[paspi->n-2].y = new_abs_y;
      } else {
         orig_hinge_index = paspi->sssi.orig_hinge_index;
         if (paspi->before) {
            if (polygon_ptr != NULL && orig_hinge_index == 0) {
               obj_vlist[paspi->n-2].x = new_abs_x;
               obj_vlist[paspi->n-2].y = new_abs_y;
               obj_vlist[orig_hinge_index+1].x =
                     (obj_vlist[orig_hinge_index].x<<1) - new_abs_x;
               obj_vlist[orig_hinge_index+1].y =
                     (obj_vlist[orig_hinge_index].y<<1) - new_abs_y;
            } else {
               obj_vlist[orig_hinge_index-1].x = new_abs_x;
               obj_vlist[orig_hinge_index-1].y = new_abs_y;
               obj_vlist[orig_hinge_index+1].x =
                     (obj_vlist[orig_hinge_index].x<<1) - new_abs_x;
               obj_vlist[orig_hinge_index+1].y =
                     (obj_vlist[orig_hinge_index].y<<1) - new_abs_y;
            }
         } else {
            if (polygon_ptr != NULL && orig_hinge_index == 0) {
               obj_vlist[orig_hinge_index+1].x = new_abs_x;
               obj_vlist[orig_hinge_index+1].y = new_abs_y;
               obj_vlist[paspi->n-2].x =
                     (obj_vlist[orig_hinge_index].x<<1) - new_abs_x;
               obj_vlist[paspi->n-2].y =
                     (obj_vlist[orig_hinge_index].y<<1) - new_abs_y;
            } else {
               obj_vlist[orig_hinge_index+1].x = new_abs_x;
               obj_vlist[orig_hinge_index+1].y = new_abs_y;
               obj_vlist[orig_hinge_index-1].x =
                     (obj_vlist[orig_hinge_index].x<<1) - new_abs_x;
               obj_vlist[orig_hinge_index-1].y =
                     (obj_vlist[orig_hinge_index].y<<1) - new_abs_y;
            }
         }
      }
   }
   AdjObjSplineVs(obj_ptr);
   if (poly_ptr != NULL) {
      UpdPolyBBox(obj_ptr, poly_ptr->n, poly_ptr->vlist);
   } else if (polygon_ptr != NULL) {
      UpdPolyBBox(obj_ptr, polygon_ptr->n, polygon_ptr->vlist);
   }
   AdjObjBBox(obj_ptr);

   UpdSelBBox();
   RedrawAreas(botObj,
         paspi->sel_ltx-GRID_ABS_SIZE(1), paspi->sel_lty-GRID_ABS_SIZE(1),
         paspi->sel_rbx+GRID_ABS_SIZE(1), paspi->sel_rby+GRID_ABS_SIZE(1),
         selLtX-GRID_ABS_SIZE(1), selLtY-GRID_ABS_SIZE(1),
         selRbX+GRID_ABS_SIZE(1), selRbY+GRID_ABS_SIZE(1));
   HighLightForward();
   SetFileModified(TRUE);
   justDupped = FALSE;

   free(paspi->ssvlist);
   free(paspi->vlist);

   return TRUE;
}

static
void EraseHighLightForAddStructuredPolyPoint(paspi, abs_dx, abs_dy,
      measure_cursor_mode)
   AddStructuredPointInfo *paspi;
   int abs_dx, abs_dy, measure_cursor_mode;
{
   if (paspi->sv != NULL) {
      XDrawLines(mainDisplay, drawWindow, revDefaultGC, paspi->sv, paspi->sn,
            CoordModeOrigin);
      free(paspi->sv);
      paspi->sv = NULL;
   }
   if (paspi->sv2 != NULL) {
      XDrawLines(mainDisplay, drawWindow, revDefaultGC, paspi->sv2, paspi->sn2,
            CoordModeOrigin);
      free(paspi->sv2);
      paspi->sv2 = NULL;
   }
   AddPointMeasureCursor(measure_cursor_mode, abs_dx, abs_dy, paspi->tx+abs_dx,
         paspi->ty+abs_dy);
   if (paspi->button == Button1) {
      MARKHR(drawWindow, revDefaultGC, OFFSET_X(paspi->ruler_abs_x),
            OFFSET_Y(paspi->ruler_abs_y));
   } else {
      XGCValues values;

      if (paspi->num_vs > 0) {
         MARKHO(drawWindow, revDefaultGC, OFFSET_X(paspi->vs[1].x),
               OFFSET_Y(paspi->vs[1].y));
      }
      if (paspi->num_vs2 > 0) {
         MARKHO(drawWindow, revDefaultGC, OFFSET_X(paspi->vs2[1].x),
               OFFSET_Y(paspi->vs2[1].y));
      }
      XSetDashes(mainDisplay, revDefaultGC, 0, dashList[8], dashListLength[8]);
      values.line_style = LineOnOffDash;
      XChangeGC(mainDisplay, revDefaultGC, GCLineStyle, &values);
      MyDashedLine(drawWindow, revDefaultGC, paspi->dashed_vs, 2);
      values.line_style = LineSolid;
      XChangeGC(mainDisplay, revDefaultGC, GCLineStyle, &values);
   }
}

static
int ContinueAddStructuredPolyOrPolygonPoint(obj_ptr, button, start_mouse_x,
      start_mouse_y, index, poly_ptr, polygon_ptr, pn_last_mouse_x,
      pn_last_mouse_y)
   struct ObjRec *obj_ptr;
   unsigned int button;
   int start_mouse_x, start_mouse_y, index, *pn_last_mouse_x, *pn_last_mouse_y;
   struct PolyRec *poly_ptr;
   struct PolygonRec *polygon_ptr;
   /* (start_mouse_x,start_mouse_y) is the mouse's origin in screen offsets */
{
   AddStructuredPointInfo aspi;
   int done=FALSE;

   memset(&aspi, 0, sizeof(AddStructuredPointInfo));
   aspi.before = INVALID;
   aspi.obj_ptr = obj_ptr;
   aspi.button = button;
   aspi.start_mouse_x = start_mouse_x;
   aspi.start_mouse_y = start_mouse_y;
   aspi.index = index;
   if (poly_ptr != NULL) {
      aspi.poly_ptr = poly_ptr;
   } else if (polygon_ptr != NULL) {
      aspi.polygon_ptr = polygon_ptr;
   } else {
      /* should not come here */
      return FALSE;
   }
   if (!ContinueAddStructuredPolyOrPolygonPointSetup(&aspi)) {
      return FALSE;
   }
   while (!done) {
      XEvent input, ev;

      XNextEvent(mainDisplay, &input);

      if (input.type == Expose || input.type == VisibilityNotify) {
         ExposeEventHandler(&input, TRUE);
      } else if (input.type == MotionNotify || input.type == KeyPress ||
            input.type == KeyRelease) {
         if (input.type == KeyPress || input.type == KeyRelease) {
            aspi.mouse_x = aspi.grid_x;
            aspi.mouse_y = aspi.grid_y;
         } else {
            aspi.mouse_x = input.xmotion.x;
            aspi.mouse_y = input.xmotion.y;
         }
         if (shiftForDiagMouseMove && DiagEventCheck(&input)) {
            if (input.type == KeyRelease) {
               aspi.mouse_x = input.xkey.x;
               aspi.mouse_y = input.xkey.y;
            } else {
               DiagGridXY(aspi.orig_grid_x, aspi.orig_grid_y, &aspi.mouse_x,
                     &aspi.mouse_y);
            }
         }
         GridXY(aspi.mouse_x, aspi.mouse_y, &aspi.grid_x, &aspi.grid_y);
         /* erase */
         EraseHighLightForAddStructuredPolyPoint(&aspi, ABS_SIZE(aspi.dx_off),
               ABS_SIZE(aspi.dy_off), ADDPOINT_DOSHOW);
         aspi.dx_off = aspi.grid_x - aspi.orig_grid_x;
         aspi.dy_off = aspi.grid_y - aspi.orig_grid_y;
         aspi.ruler_abs_x = aspi.tx + ABS_SIZE(aspi.dx_off);
         aspi.ruler_abs_y = aspi.ty + ABS_SIZE(aspi.dy_off);
         MarkRulers(OFFSET_X(aspi.ruler_abs_x), OFFSET_Y(aspi.ruler_abs_y));

         if (!aspi.already_moved) {
            ContinueAddStructuredPolyOrPolygonPointFirstMoved(&aspi);
         }
         ContinueAddStructuredPolyOrPolygonPointRestMoved(&aspi);
         while (XCheckMaskEvent(mainDisplay, PointerMotionMask, &ev)) ;
      } else if (input.type == ButtonRelease) {
         done = TRUE;
         *pn_last_mouse_x = aspi.mouse_x;
         *pn_last_mouse_y = aspi.mouse_y;
         /* erase */
         EraseHighLightForAddStructuredPolyPoint(&aspi, ABS_SIZE(aspi.dx_off),
               ABS_SIZE(aspi.dy_off), ADDPOINT_DOSHOW);

         return ContinueAddStructuredPolyOrPolygonPointFinished(&aspi);
      }
   }
   return TRUE;
}

static
int ContinueAddPolyPoint(ObjPtr, MouseX, MouseY, Index, PolyPtr,
      LastMouseX, LastMouseY)
   struct ObjRec *ObjPtr;
   int MouseX, MouseY, Index;
   struct PolyRec *PolyPtr;
   int *LastMouseX, *LastMouseY;
   /*
    * (MouseX,MouseY) is the mouse's origin in screen offsets
    *
    * This routine is only called for LT_STRAIGHT, LT_SPLINE, or LT_INTSPLINE.
    */
{
   int n=PolyPtr->n, sn=0, curved=PolyPtr->curved;
   int already_moved=FALSE, done=FALSE, before=INVALID;
   XPoint v[3], *sv=NULL;
   IntPoint *vs=PolyPtr->vlist, *pv=NULL, *cntrlv=NULL;
   int prev_x, prev_y, prev_tx, prev_ty, x, y, tx, ty, tmp_x, tmp_y;
   int next_x, next_y, next_tx, next_ty;
   int orig_x, orig_y, grid_x, grid_y, new_mouse_x, new_mouse_y;
   int sel_ltx, sel_lty, sel_rbx, sel_rby, num=0, i, intn=0;
   char *smooth=PolyPtr->smooth, *tmp_smooth=NULL;
   double prev_angle, next_angle, prev_dist, next_dist, dx, dy;

#ifdef _TGIF_DBG /* debug, do not translate */
   TgAssert(curved != LT_STRUCT_SPLINE,
         "curved == LT_STRUCT_SPLINE in ContinueAddPolyPoint()", NULL);
#endif /* _TGIF_DBG */
   memset(v, 0, sizeof(XPoint)*3);
   sel_ltx = selLtX; sel_lty = selLtY;
   sel_rbx = selRbX; sel_rby = selRbY;

   x = tx = vs[Index].x;
   y = ty = vs[Index].y;
   if (ObjPtr->ctm != NULL) {
      TransformPointThroughCTM(x-ObjPtr->x, y-ObjPtr->y, ObjPtr->ctm,
            &tmp_x, &tmp_y);
      tx = ObjPtr->x+tmp_x;
      ty = ObjPtr->y+tmp_y;
   }
   if (curved == LT_INTSPLINE || smooth == NULL || !smooth[Index]) {
      MARK(drawWindow, revDefaultGC, OFFSET_X(tx), OFFSET_Y(ty));
   } else {
      MARKO(drawWindow, revDefaultGC, OFFSET_X(tx), OFFSET_Y(ty));
   }
   if (Index == 0) {
      next_x = next_tx = vs[1].x;
      next_y = next_ty = vs[1].y;
      prev_x = prev_tx = 2*x-next_x;
      prev_y = prev_ty = 2*y-next_y;
   } else if (Index == n-1) {
      prev_x = prev_tx = vs[n-2].x;
      prev_y = prev_ty = vs[n-2].y;
      next_x = next_tx = 2*x-prev_x;
      next_y = next_ty = 2*y-prev_y;
   } else {
      prev_x = prev_tx = vs[Index-1].x;
      prev_y = prev_ty = vs[Index-1].y;
      next_x = next_tx = vs[Index+1].x;
      next_y = next_ty = vs[Index+1].y;
   }
   if (ObjPtr->ctm != NULL) {
      TransformPointThroughCTM(next_x-ObjPtr->x, next_y-ObjPtr->y,
            ObjPtr->ctm, &tmp_x, &tmp_y);
      next_tx = ObjPtr->x+tmp_x;
      next_ty = ObjPtr->y+tmp_y;
      TransformPointThroughCTM(prev_x-ObjPtr->x, prev_y-ObjPtr->y,
            ObjPtr->ctm, &tmp_x, &tmp_y);
      prev_tx = ObjPtr->x+tmp_x;
      prev_ty = ObjPtr->y+tmp_y;
   }
   dx = (double)(prev_tx - tx);
   dy = (double)(prev_ty - ty);
   prev_dist = dx*dx+dy*dy;
   dx = (double)(next_tx - tx);
   dy = (double)(next_ty - ty);
   next_dist = dx*dx+dy*dy;

   prev_angle = (prev_tx==tx) ? ((prev_ty>=ty) ? M_PI/2.0 : -M_PI/2.0) :
         atan2((double)(prev_ty-ty), (double)(prev_tx-tx));
   next_angle = (next_tx==tx) ? ((next_ty>=ty) ? M_PI/2.0 : -M_PI/2.0) :
         atan2((double)(next_ty-ty), (double)(next_tx-tx));
   pv = (IntPoint*)malloc((n+2)*sizeof(IntPoint));
   if (pv == NULL) FailAllocMessage();
   if (curved != LT_INTSPLINE && smooth != NULL) {
      tmp_smooth = (char*)malloc((n+2)*sizeof(char));
      if (tmp_smooth == NULL) FailAllocMessage();
   }
   if (ObjPtr->ctm == NULL) {
      for (i = 0; i <= Index; i++) {
         pv[i].x = vs[i].x;
         pv[i].y = vs[i].y;
         if (tmp_smooth != NULL) tmp_smooth[i] = smooth[i];
      }
      for (i = Index; i < n; i++) {
         pv[i+1].x = vs[i].x;
         pv[i+1].y = vs[i].y;
         if (tmp_smooth != NULL) tmp_smooth[i+1] = smooth[i];
      }
   } else {
      for (i = 0; i <= Index; i++) {
         TransformPointThroughCTM(vs[i].x-ObjPtr->x, vs[i].y-ObjPtr->y,
               ObjPtr->ctm, &tmp_x, &tmp_y);
         pv[i].x = ObjPtr->x+tmp_x;
         pv[i].y = ObjPtr->y+tmp_y;
         if (tmp_smooth != NULL) tmp_smooth[i] = smooth[i];
      }
      for (i = Index; i < n; i++) {
         TransformPointThroughCTM(vs[i].x-ObjPtr->x, vs[i].y-ObjPtr->y,
               ObjPtr->ctm, &tmp_x, &tmp_y);
         pv[i+1].x = ObjPtr->x+tmp_x;
         pv[i+1].y = ObjPtr->y+tmp_y;
         if (tmp_smooth != NULL) tmp_smooth[i+1] = smooth[i];
      }
   }
   GridXY(MouseX, MouseY, &orig_x, &orig_y);
   grid_x = orig_x;
   grid_y = orig_y;
   new_mouse_x = MouseX; new_mouse_y = MouseY;
   AddPointMeasureCursor(ADDPOINT_STARTSHOW, 0, 0, tx, ty);

   while (!done) {
      double new_angle, theta_1, theta_2;
      XEvent input, ev;

      XNextEvent(mainDisplay, &input);

      if (input.type == Expose || input.type == VisibilityNotify) {
         ExposeEventHandler(&input, TRUE);
      } else if (input.type == MotionNotify || input.type == KeyPress ||
            input.type == KeyRelease) {
         int new_x, new_y;

         if (input.type == KeyPress || input.type == KeyRelease) {
            new_mouse_x = grid_x;
            new_mouse_y = grid_y;
         } else {
            new_mouse_x = input.xmotion.x;
            new_mouse_y = input.xmotion.y;
         }
         if (shiftForDiagMouseMove && DiagEventCheck(&input)) {
            if (input.type == KeyRelease) {
               new_mouse_x = input.xkey.x;
               new_mouse_y = input.xkey.y;
            } else {
               DiagGridXY(orig_x, orig_y, &new_mouse_x, &new_mouse_y);
            }
         }
         GridXY(new_mouse_x, new_mouse_y, &grid_x, &grid_y);
         new_x = ABS_SIZE(new_mouse_x-MouseX) + tx;
         new_y = ABS_SIZE(new_mouse_y-MouseY) + ty;
         if (!already_moved) {
            double new_prev_dist, new_next_dist;

            already_moved = TRUE;

            dx = (double)(prev_tx - new_mouse_x);
            dy = (double)(prev_ty - new_mouse_y);
            new_prev_dist = dx*dx+dy*dy;
            dx = (double)(next_tx - new_mouse_x);
            dy = (double)(next_ty - new_mouse_y); 
            new_next_dist = dx*dx+dy*dy;

            before = DetermineBefore(prev_dist, next_dist, new_prev_dist,
                  new_next_dist);
            if (before == INVALID) {
               new_angle = (new_x==tx) ? ((new_y>=ty) ? M_PI/2.0 : -M_PI/2.0) :
                     atan2((double)(new_y-ty), (double)(new_x-tx));
               theta_1 = fabs(prev_angle - new_angle);
               theta_2 = fabs(next_angle - new_angle);
               if (theta_1 > M_PI) theta_1 = 2*M_PI-theta_1;
               if (theta_2 > M_PI) theta_2 = 2*M_PI-theta_2;
               before = (theta_1 <= theta_2);
            }
            if (before) {
               /* Add a point between the current and the previous point */
               if (Index == 0) {
                  num = 2;
                  v[0].x = OFFSET_X(tx); v[0].y = OFFSET_Y(ty);
                  v[1].x = OFFSET_X(tx); v[1].y = OFFSET_Y(ty);
               } else {
                  num = 3;
                  v[0].x = OFFSET_X(prev_tx); v[0].y = OFFSET_Y(prev_ty);
                  v[1].x = OFFSET_X(tx);      v[1].y = OFFSET_Y(ty);
                  v[2].x = OFFSET_X(tx);      v[2].y = OFFSET_Y(ty);
               }
            } else {
               /* Add a point between the current and the next point */
               if (Index == n-1) {
                  num = 2;
                  v[0].x = OFFSET_X(tx);      v[0].y = OFFSET_Y(ty);
                  v[1].x = OFFSET_X(tx);      v[1].y = OFFSET_Y(ty);
               } else {
                  num = 3;
                  v[0].x = OFFSET_X(tx);      v[0].y = OFFSET_Y(ty);
                  v[1].x = OFFSET_X(tx);      v[1].y = OFFSET_Y(ty);
                  v[2].x = OFFSET_X(next_tx); v[2].y = OFFSET_Y(next_ty);
               }
            }
            switch (curved) {
            case LT_STRAIGHT:
            case LT_SPLINE:
               sv = MakeMultiSplinePolyVertex(curved, &sn, tmp_smooth,
                     drawOrigX, drawOrigY, n+1, pv);
               break;
            case LT_INTSPLINE:
               sv = MakeIntSplinePolyVertex(&sn, &intn, &cntrlv,
                     drawOrigX, drawOrigY, n+1, pv);
               break;
            case LT_STRUCT_SPLINE:
               /* should never get here */
               break;
            }
            XDrawLines(mainDisplay, drawWindow, revDefaultGC, sv, sn,
                  CoordModeOrigin);
         } else {
            AddPointMeasureCursor(ADDPOINT_DOSHOW, abs(ABS_X(v[1].x)-tx),
                  abs(ABS_Y(v[1].y)-ty), ABS_X(v[1].x), ABS_Y(v[1].y));
            XDrawLines(mainDisplay, drawWindow, revDefaultGC, sv, sn,
                  CoordModeOrigin);
            v[1].x = OFFSET_X(tx) + grid_x - orig_x;
            v[1].y = OFFSET_Y(ty) + grid_y - orig_y;
            free(sv);
            if (before) {
               pv[Index].x = tx + ABS_SIZE(grid_x-orig_x);
               pv[Index].y = ty + ABS_SIZE(grid_y-orig_y);
            } else {
               pv[Index+1].x = tx + ABS_SIZE(grid_x-orig_x);
               pv[Index+1].y = ty + ABS_SIZE(grid_y-orig_y);
            }
            switch (curved) {
            case LT_STRAIGHT:
            case LT_SPLINE:
               sv = MakeMultiSplinePolyVertex(curved, &sn, tmp_smooth,
                     drawOrigX, drawOrigY, n+1, pv);
               break;
            case LT_INTSPLINE:
               free(cntrlv);
               sv = MakeIntSplinePolyVertex(&sn, &intn, &cntrlv,
                     drawOrigX, drawOrigY, n+1, pv);
               break;
            case LT_STRUCT_SPLINE:
               /* should never get here */
               break;
            }
            XDrawLines(mainDisplay, drawWindow, revDefaultGC, sv, sn,
                  CoordModeOrigin);
            MarkRulers(v[1].x, v[1].y);
            AddPointMeasureCursor(ADDPOINT_DOSHOW, abs(ABS_X(v[1].x)-tx),
                  abs(ABS_Y(v[1].y)-ty), ABS_X(v[1].x), ABS_Y(v[1].y));
         }
         while (XCheckMaskEvent(mainDisplay, PointerMotionMask, &ev)) ;
      } else if (input.type == ButtonRelease) {
         done = TRUE;
         *LastMouseX = new_mouse_x; *LastMouseY = new_mouse_y;
         if (curved == LT_INTSPLINE || smooth == NULL || !smooth[Index]) {
            MARK(drawWindow, revDefaultGC, OFFSET_X(tx), OFFSET_Y(ty));
         } else {
            MARKO(drawWindow, revDefaultGC, OFFSET_X(tx), OFFSET_Y(ty));
         }
         if (already_moved) {
            AddPointMeasureCursor(ADDPOINT_ENDSHOW, abs(ABS_X(v[1].x)-tx),
                  abs(ABS_Y(v[1].y)-ty), ABS_X(v[1].x), ABS_Y(v[1].y));
         } else {
            AddPointMeasureCursor(ADDPOINT_STARTSHOW, 0, 0, tx, ty);
         }
         if (!already_moved) {
            return FALSE;
         } else {
            XDrawLines(mainDisplay, drawWindow, revDefaultGC, sv, sn,
                  CoordModeOrigin);
            if (grid_x == orig_x && grid_y == orig_y) {
               return FALSE;
            }
         }
         HighLightReverse();
         vs = (IntPoint*)realloc(vs, (n+2)*sizeof(IntPoint));
         if (vs == NULL) return FailAllocMessage();
         if (smooth != NULL) {
            smooth = (char*)realloc(smooth, (n+2)*sizeof(char));
            if (smooth == NULL) return FailAllocMessage();
         }
         PolyPtr->vlist = vs;
         PolyPtr->smooth = smooth;
         if (before) {
            for (i = n-1; i >= Index; i--) {
               vs[i+1] = vs[i];
               if (smooth != NULL) smooth[i+1] = smooth[i];
            }
            if (ObjPtr->ctm == NULL) {
               vs[Index].x = x + ABS_SIZE(grid_x-orig_x);
               vs[Index].y = y + ABS_SIZE(grid_y-orig_y);
            } else {
               vs[Index].x = tx + ABS_SIZE(grid_x-orig_x);
               vs[Index].y = ty + ABS_SIZE(grid_y-orig_y);
               ReverseTransformPointThroughCTM(vs[Index].x-ObjPtr->x,
                        vs[Index].y-ObjPtr->y, ObjPtr->ctm, &tmp_x, &tmp_y);
               vs[Index].x = ObjPtr->x+tmp_x;
               vs[Index].y = ObjPtr->y+tmp_y;
            }
            if (smooth != NULL) smooth[Index] = smooth[Index+1];
         } else {
            for (i = n-1; i > Index; i--) {
               vs[i+1] = vs[i];
               if (smooth != NULL) smooth[i+1] = smooth[i];
            }
            if (ObjPtr->ctm == NULL) {
               vs[Index+1].x = x + ABS_SIZE(grid_x-orig_x);
               vs[Index+1].y = y + ABS_SIZE(grid_y-orig_y);
            } else {
               vs[Index+1].x = tx + ABS_SIZE(grid_x-orig_x);
               vs[Index+1].y = ty + ABS_SIZE(grid_y-orig_y);
               ReverseTransformPointThroughCTM(vs[Index+1].x-ObjPtr->x,
                        vs[Index+1].y-ObjPtr->y, ObjPtr->ctm, &tmp_x, &tmp_y);
               vs[Index+1].x = ObjPtr->x+tmp_x;
               vs[Index+1].y = ObjPtr->y+tmp_y;
            }
            if (smooth != NULL) smooth[Index+1] = smooth[Index];
         }
         if (sv != NULL) {
            free(sv);
            sv = NULL;
         }
         if (pv != NULL) {
            free(pv);
            pv = NULL;
         }
         if (tmp_smooth != NULL) free(tmp_smooth);
         if (curved == LT_INTSPLINE && cntrlv != NULL) free(cntrlv);
         PolyPtr->n++;
         n++;
         AdjObjSplineVs(ObjPtr);
         if (curved != LT_INTSPLINE) {
            UpdPolyBBox(ObjPtr, PolyPtr->n, PolyPtr->vlist);
         } else {
            UpdPolyBBox(ObjPtr, PolyPtr->intn, PolyPtr->intvlist);
         }
         AdjObjBBox(ObjPtr);

         UpdSelBBox();
         RedrawAreas(botObj,
               sel_ltx-GRID_ABS_SIZE(1), sel_lty-GRID_ABS_SIZE(1),
               sel_rbx+GRID_ABS_SIZE(1), sel_rby+GRID_ABS_SIZE(1),
               selLtX-GRID_ABS_SIZE(1), selLtY-GRID_ABS_SIZE(1),
               selRbX+GRID_ABS_SIZE(1), selRbY+GRID_ABS_SIZE(1));
         HighLightForward();
         SetFileModified(TRUE);
         justDupped = FALSE;
      } else if (input.type == KeyPress) {
         if (KeyPressEventIsEscape(&input.xkey)) {
            done = TRUE;
            *LastMouseX = new_mouse_x; *LastMouseY = new_mouse_y;
            if (curved == LT_INTSPLINE || smooth == NULL || !smooth[Index]) {
               MARK(drawWindow, revDefaultGC, OFFSET_X(tx), OFFSET_Y(ty));
            } else {
               MARKO(drawWindow, revDefaultGC, OFFSET_X(tx), OFFSET_Y(ty));
            }
            if (already_moved) {
               AddPointMeasureCursor(ADDPOINT_ENDSHOW, abs(ABS_X(v[1].x)-tx),
                     abs(ABS_Y(v[1].y)-ty), ABS_X(v[1].x), ABS_Y(v[1].y));
            } else {
               AddPointMeasureCursor(ADDPOINT_STARTSHOW, 0, 0, tx, ty);
            }
            if (already_moved) {
               XDrawLines(mainDisplay, drawWindow, revDefaultGC, sv, sn,
                     CoordModeOrigin);
               if (grid_x == orig_x && grid_y == orig_y) {
                  return FALSE;
               }
            }
            return FALSE;
         }
      }
   }
   return TRUE;
}

static
int ContinueAddPolygonPoint(ObjPtr, MouseX, MouseY, Index, PolygonPtr,
      LastMouseX, LastMouseY)
   struct ObjRec *ObjPtr;
   int MouseX, MouseY, Index;
   struct PolygonRec *PolygonPtr;
   int *LastMouseX, *LastMouseY;
   /*
    * (MouseX,MouseY) is the mouse's origin in screen offsets
    *
    * This routine is only called for LT_STRAIGHT, LT_SPLINE, or LT_INTSPLINE.
    */
{
   int n=PolygonPtr->n, sn=0, curved=PolygonPtr->curved;
   int already_moved=FALSE, done=FALSE, before=FALSE;
   XPoint v[3], *sv=NULL;
   IntPoint *vs=PolygonPtr->vlist, *pv=NULL, *cntrlv=NULL;
   int prev_x, prev_y, prev_tx, prev_ty, x, y, tx, ty, tmp_x, tmp_y;
   int next_x, next_y, next_tx, next_ty;
   int orig_x, orig_y, grid_x, grid_y, new_mouse_x, new_mouse_y;
   int sel_ltx, sel_lty, sel_rbx, sel_rby, i, intn=0;
   char *smooth=PolygonPtr->smooth, *tmp_smooth=NULL;
   double prev_angle, next_angle, prev_dist, next_dist, dx, dy;

   memset(v, 0, sizeof(XPoint)*3);
#ifdef _TGIF_DBG /* debug, do not translate */
   TgAssert(curved != LT_STRUCT_SPLINE,
         "curved == LT_STRUCT_SPLINE in ContinueAddPolygonPoint()", NULL);
#endif /* _TGIF_DBG */
   sel_ltx = selLtX; sel_lty = selLtY;
   sel_rbx = selRbX; sel_rby = selRbY;

   x = tx = vs[Index].x;
   y = ty = vs[Index].y;
   if (ObjPtr->ctm != NULL) {
      TransformPointThroughCTM(x-ObjPtr->x, y-ObjPtr->y, ObjPtr->ctm,
            &tmp_x, &tmp_y);
      tx = ObjPtr->x+tmp_x;
      ty = ObjPtr->y+tmp_y;
   }
   if (curved == LT_INTSPLINE || smooth == NULL || !smooth[Index]) {
      MARK(drawWindow, revDefaultGC, OFFSET_X(tx), OFFSET_Y(ty));
   } else {
      MARKO(drawWindow, revDefaultGC, OFFSET_X(tx), OFFSET_Y(ty));
   }
   if (Index == 0 || Index == n-1) {
      next_x = next_tx = vs[1].x;
      next_y = next_ty = vs[1].y;
      prev_x = prev_tx = vs[n-2].x;
      prev_y = prev_ty = vs[n-2].y;
   } else {
      prev_x = prev_tx = vs[Index-1].x;
      prev_y = prev_ty = vs[Index-1].y;
      next_x = next_tx = vs[Index+1].x;
      next_y = next_ty = vs[Index+1].y;
   }
   if (ObjPtr->ctm != NULL) {
      TransformPointThroughCTM(next_x-ObjPtr->x, next_y-ObjPtr->y,
            ObjPtr->ctm, &tmp_x, &tmp_y);
      next_tx = ObjPtr->x+tmp_x;
      next_ty = ObjPtr->y+tmp_y;
      TransformPointThroughCTM(prev_x-ObjPtr->x, prev_y-ObjPtr->y,
            ObjPtr->ctm, &tmp_x, &tmp_y);
      prev_tx = ObjPtr->x+tmp_x;
      prev_ty = ObjPtr->y+tmp_y;
   }
   dx = (double)(prev_tx - tx);
   dy = (double)(prev_ty - ty);
   prev_dist = dx*dx+dy*dy;
   dx = (double)(next_tx - tx);
   dy = (double)(next_ty - ty);
   next_dist = dx*dx+dy*dy;

   prev_angle = (prev_tx==tx) ? ((prev_ty>=ty) ? M_PI/2.0 : -M_PI/2.0) :
         atan2((double)(prev_ty-ty), (double)(prev_tx-tx));
   next_angle = (next_tx==tx) ? ((next_ty>=ty) ? M_PI/2.0 : -M_PI/2.0) :
         atan2((double)(next_ty-ty), (double)(next_tx-tx));
   pv = (IntPoint *)malloc((n+2)*sizeof(IntPoint));
   if (pv == NULL) FailAllocMessage();
   if (curved != LT_INTSPLINE && smooth != NULL) {
      tmp_smooth = (char*)malloc((n+2)*sizeof(char));
      if (tmp_smooth == NULL) FailAllocMessage();
   }
   if (ObjPtr->ctm == NULL) {
      for (i = 0; i <= Index; i++) {
         pv[i].x = vs[i].x;
         pv[i].y = vs[i].y;
         if (tmp_smooth != NULL) tmp_smooth[i] = smooth[i];
      }
      for (i = Index; i < n; i++) {
         pv[i+1].x = vs[i].x;
         pv[i+1].y = vs[i].y;
         if (tmp_smooth != NULL) tmp_smooth[i+1] = smooth[i];
      }
   } else {
      for (i = 0; i <= Index; i++) {
         TransformPointThroughCTM(vs[i].x-ObjPtr->x, vs[i].y-ObjPtr->y,
               ObjPtr->ctm, &tmp_x, &tmp_y);
         pv[i].x = ObjPtr->x+tmp_x;
         pv[i].y = ObjPtr->y+tmp_y;
         if (tmp_smooth != NULL) tmp_smooth[i] = smooth[i];
      }
      for (i = Index; i < n; i++) {
         TransformPointThroughCTM(vs[i].x-ObjPtr->x, vs[i].y-ObjPtr->y,
               ObjPtr->ctm, &tmp_x, &tmp_y);
         pv[i+1].x = ObjPtr->x+tmp_x;
         pv[i+1].y = ObjPtr->y+tmp_y;
         if (tmp_smooth != NULL) tmp_smooth[i+1] = smooth[i];
      }
   }
   GridXY(MouseX, MouseY, &orig_x, &orig_y);
   grid_x = orig_x;
   grid_y = orig_y;
   new_mouse_x = MouseX; new_mouse_y = MouseY;
   AddPointMeasureCursor(ADDPOINT_STARTSHOW, 0, 0, tx, ty);

   while (!done) {
      double new_angle, theta_1, theta_2;
      XEvent input, ev;

      XNextEvent(mainDisplay, &input);

      if (input.type == Expose || input.type == VisibilityNotify) {
         ExposeEventHandler(&input, TRUE);
      } else if (input.type == MotionNotify || input.type == KeyPress ||
            input.type == KeyRelease) {
         int new_x, new_y;

         if (input.type == KeyPress || input.type == KeyRelease) {
            new_mouse_x = grid_x;
            new_mouse_y = grid_y;
         } else {
            new_mouse_x = input.xmotion.x;
            new_mouse_y = input.xmotion.y;
         }
         if (shiftForDiagMouseMove && DiagEventCheck(&input)) {
            if (input.type == KeyRelease) {
               new_mouse_x = input.xkey.x;
               new_mouse_y = input.xkey.y;
            } else {
               DiagGridXY(orig_x, orig_y, &new_mouse_x, &new_mouse_y);
            }
         }
         GridXY(new_mouse_x, new_mouse_y, &grid_x, &grid_y);
         new_x = ABS_SIZE(new_mouse_x-MouseX) + tx;
         new_y = ABS_SIZE(new_mouse_y-MouseY) + ty;
         if (!already_moved) {
            double new_prev_dist, new_next_dist;

            already_moved = TRUE;

            dx = (double)(prev_tx - new_mouse_x);
            dy = (double)(prev_ty - new_mouse_y);
            new_prev_dist = dx*dx+dy*dy;
            dx = (double)(next_tx - new_mouse_x);
            dy = (double)(next_ty - new_mouse_y);
            new_next_dist = dx*dx+dy*dy;

            before = DetermineBefore(prev_dist, next_dist, new_prev_dist,
                  new_next_dist);
            if (before == INVALID) {
               new_angle = (new_x==tx) ? ((new_y>=ty) ? M_PI/2.0 : -M_PI/2.0) :
                     atan2((double)(new_y-ty), (double)(new_x-tx));
               theta_1 = fabs(prev_angle - new_angle);
               theta_2 = fabs(next_angle - new_angle);
               if (theta_1 > M_PI) theta_1 = 2*M_PI-theta_1;
               if (theta_2 > M_PI) theta_2 = 2*M_PI-theta_2;
               before = (theta_1 <= theta_2);
            }
            if (before) {
               /* Add a point between the current and the previous point */
               v[0].x = OFFSET_X(prev_tx); v[0].y = OFFSET_Y(prev_ty);
               v[1].x = OFFSET_X(tx);      v[1].y = OFFSET_Y(ty);
               v[2].x = OFFSET_X(tx);      v[2].y = OFFSET_Y(ty);
            } else {
               /* Add a point between the current and the next point */
               v[0].x = OFFSET_X(tx);      v[0].y = OFFSET_Y(ty);
               v[1].x = OFFSET_X(tx);      v[1].y = OFFSET_Y(ty);
               v[2].x = OFFSET_X(next_tx); v[2].y = OFFSET_Y(next_ty);
            }
            switch (curved) {
            case LT_STRAIGHT:
            case LT_SPLINE:
               sv = MakeMultiSplinePolygonVertex(curved, &sn, tmp_smooth,
                     drawOrigX, drawOrigY, n+1, pv);
               break;
            case LT_INTSPLINE:
               sv = MakeIntSplinePolygonVertex(&sn, &intn, &cntrlv,
                     drawOrigX, drawOrigY, n+1, pv);
               break;
            case LT_STRUCT_SPLINE:
               /* should never get here */
               break;
            }
            XDrawLines(mainDisplay, drawWindow, revDefaultGC, sv, sn,
                  CoordModeOrigin);
         } else {
            AddPointMeasureCursor(ADDPOINT_DOSHOW, abs(ABS_X(v[1].x)-tx),
                  abs(ABS_Y(v[1].y)-ty), ABS_X(v[1].x), ABS_Y(v[1].y));
            XDrawLines(mainDisplay, drawWindow, revDefaultGC, sv, sn,
                  CoordModeOrigin);
            v[1].x = OFFSET_X(tx) + grid_x - orig_x;
            v[1].y = OFFSET_Y(ty) + grid_y - orig_y;
            if (sv != NULL) {
               free(sv);
               sv = NULL;
            }
            if (before) {
               pv[Index].x = tx + ABS_SIZE(grid_x-orig_x);
               pv[Index].y = ty + ABS_SIZE(grid_y-orig_y);
               if (Index == 0) {
                  pv[n].x = tx + ABS_SIZE(grid_x-orig_x);
                  pv[n].y = ty + ABS_SIZE(grid_y-orig_y);
               }
            } else {
               pv[Index+1].x = tx + ABS_SIZE(grid_x-orig_x);
               pv[Index+1].y = ty + ABS_SIZE(grid_y-orig_y);
               if (Index == n-1) {
                  pv[0].x = tx + ABS_SIZE(grid_x-orig_x);
                  pv[0].y = ty + ABS_SIZE(grid_y-orig_y);
               }
            }
            switch (curved) {
            case LT_STRAIGHT:
            case LT_SPLINE:
               sv = MakeMultiSplinePolygonVertex(curved, &sn, tmp_smooth,
                     drawOrigX, drawOrigY, n+1, pv);
               break;
            case LT_INTSPLINE:
               free(cntrlv);
               sv = MakeIntSplinePolygonVertex(&sn, &intn, &cntrlv,
                     drawOrigX, drawOrigY, n+1, pv);
               break;
            case LT_STRUCT_SPLINE:
               /* should never get here */
               break;
            }
            XDrawLines(mainDisplay, drawWindow, revDefaultGC, sv, sn,
                  CoordModeOrigin);
            MarkRulers(v[1].x, v[1].y);
            AddPointMeasureCursor(ADDPOINT_DOSHOW, abs(ABS_X(v[1].x)-tx),
                  abs(ABS_Y(v[1].y)-ty), ABS_X(v[1].x), ABS_Y(v[1].y));
         }
         while (XCheckMaskEvent(mainDisplay, PointerMotionMask, &ev)) ;
      } else if (input.type == ButtonRelease) {
         done = TRUE;
         *LastMouseX = new_mouse_x; *LastMouseY = new_mouse_y;
         if (curved == LT_INTSPLINE || smooth == NULL || !smooth[Index]) {
            MARK(drawWindow, revDefaultGC, OFFSET_X(tx), OFFSET_Y(ty));
         } else {
            MARKO(drawWindow, revDefaultGC, OFFSET_X(tx), OFFSET_Y(ty));
         }
         if (already_moved) {
            AddPointMeasureCursor(ADDPOINT_ENDSHOW, abs(ABS_X(v[1].x)-tx),
                  abs(ABS_Y(v[1].y)-ty), ABS_X(v[1].x), ABS_Y(v[1].y));
         } else {
            AddPointMeasureCursor(ADDPOINT_STARTSHOW, 0, 0, tx, ty);
         }
         if (!already_moved) {
            return FALSE;
         } else {
            XDrawLines(mainDisplay, drawWindow, revDefaultGC, sv, sn,
                  CoordModeOrigin);
            if (grid_x == orig_x && grid_y == orig_y) {
               return FALSE;
            }
         }
         HighLightReverse();
         vs = (IntPoint*)realloc(vs, (n+2)*sizeof(IntPoint));
         if (vs == NULL) return FailAllocMessage();
         if (smooth != NULL) {
            smooth = (char*)realloc(smooth, (n+2)*sizeof(char));
            if (smooth == NULL) return FailAllocMessage();
         }
         PolygonPtr->vlist = vs;
         PolygonPtr->smooth = smooth;
         if (Index == 0 || Index == n-1) {
            if (before) {
               vs[n].x = vs[n-1].x;
               vs[n].y = vs[n-1].y;
               if (ObjPtr->ctm == NULL) {
                  vs[n-1].x = x + ABS_SIZE(grid_x-orig_x);
                  vs[n-1].y = y + ABS_SIZE(grid_y-orig_y);
               } else {
                  vs[n-1].x = tx + ABS_SIZE(grid_x-orig_x);
                  vs[n-1].y = ty + ABS_SIZE(grid_y-orig_y);
                  ReverseTransformPointThroughCTM(vs[n-1].x-ObjPtr->x,
                        vs[n-1].y-ObjPtr->y, ObjPtr->ctm, &tmp_x, &tmp_y);
                  vs[n-1].x = ObjPtr->x+tmp_x;
                  vs[n-1].y = ObjPtr->y+tmp_y;
               }
               if (smooth != NULL) smooth[n] = smooth[n-1];
            } else {
               for (i = n-1; i > 0; i--) {
                  vs[i+1].x = vs[i].x;
                  vs[i+1].y = vs[i].y;
                  if (smooth != NULL) smooth[i+1] = smooth[i];
               }
               if (ObjPtr->ctm == NULL) {
                  vs[1].x = x + ABS_SIZE(grid_x-orig_x);
                  vs[1].y = y + ABS_SIZE(grid_y-orig_y);
               } else {
                  vs[1].x = tx + ABS_SIZE(grid_x-orig_x);
                  vs[1].y = ty + ABS_SIZE(grid_y-orig_y);
                  ReverseTransformPointThroughCTM(vs[1].x-ObjPtr->x,
                        vs[1].y-ObjPtr->y, ObjPtr->ctm, &tmp_x, &tmp_y);
                  vs[1].x = ObjPtr->x+tmp_x;
                  vs[1].y = ObjPtr->y+tmp_y;
               }
               if (smooth != NULL) smooth[1] = smooth[0];
            }
         } else {
            if (before) {
               for (i = n-1; i >= Index; i--) {
                  vs[i+1].x = vs[i].x;
                  vs[i+1].y = vs[i].y;
                  if (smooth != NULL) smooth[i+1] = smooth[i];
               }
               if (ObjPtr->ctm == NULL) {
                  vs[Index].x = x + ABS_SIZE(grid_x-orig_x);
                  vs[Index].y = y + ABS_SIZE(grid_y-orig_y);
               } else {
                  vs[Index].x = tx + ABS_SIZE(grid_x-orig_x);
                  vs[Index].y = ty + ABS_SIZE(grid_y-orig_y);
                  ReverseTransformPointThroughCTM(vs[Index].x-ObjPtr->x,
                        vs[Index].y-ObjPtr->y, ObjPtr->ctm, &tmp_x, &tmp_y);
                  vs[Index].x = ObjPtr->x+tmp_x;
                  vs[Index].y = ObjPtr->y+tmp_y;
               }
               if (smooth != NULL) smooth[Index] = smooth[Index+1];
            } else {
               for (i = n-1; i > Index; i--) {
                  vs[i+1].x = vs[i].x;
                  vs[i+1].y = vs[i].y;
                  if (smooth != NULL) smooth[i+1] = smooth[i];
               }
               if (ObjPtr->ctm == NULL) {
                  vs[Index+1].x = x + ABS_SIZE(grid_x-orig_x);
                  vs[Index+1].y = y + ABS_SIZE(grid_y-orig_y);
               } else {
                  vs[Index+1].x = tx + ABS_SIZE(grid_x-orig_x);
                  vs[Index+1].y = ty + ABS_SIZE(grid_y-orig_y);
                  ReverseTransformPointThroughCTM(vs[Index+1].x-ObjPtr->x,
                        vs[Index+1].y-ObjPtr->y, ObjPtr->ctm, &tmp_x, &tmp_y);
                  vs[Index+1].x = ObjPtr->x+tmp_x;
                  vs[Index+1].y = ObjPtr->y+tmp_y;
               }
               if (smooth != NULL) smooth[Index+1] = smooth[Index];
            }
         }
         if (sv != NULL) { free(sv); sv = NULL; }
         if (pv != NULL) { free(pv); pv = NULL; }
         if (tmp_smooth != NULL) { free(tmp_smooth); tmp_smooth = NULL; }
         if (curved == LT_INTSPLINE && cntrlv != NULL) {
            free(cntrlv);
            cntrlv = NULL;
         }
         PolygonPtr->n++;
         n++;
         AdjObjSplineVs(ObjPtr);
         if (curved != LT_INTSPLINE) {
            UpdPolyBBox(ObjPtr, PolygonPtr->n, PolygonPtr->vlist);
         } else {
            UpdPolyBBox(ObjPtr, PolygonPtr->intn, PolygonPtr->intvlist);
         }
         AdjObjBBox(ObjPtr);

         UpdSelBBox();
         RedrawAreas(botObj,
               sel_ltx-GRID_ABS_SIZE(1), sel_lty-GRID_ABS_SIZE(1),
               sel_rbx+GRID_ABS_SIZE(1), sel_rby+GRID_ABS_SIZE(1),
               selLtX-GRID_ABS_SIZE(1), selLtY-GRID_ABS_SIZE(1),
               selRbX+GRID_ABS_SIZE(1), selRbY+GRID_ABS_SIZE(1));
         HighLightForward();
         SetFileModified(TRUE);
         justDupped = FALSE;
      } else if (input.type == KeyPress) {
         if (KeyPressEventIsEscape(&input.xkey)) {
            done = TRUE;
            *LastMouseX = new_mouse_x; *LastMouseY = new_mouse_y;
            if (curved == LT_INTSPLINE || smooth == NULL || !smooth[Index]) {
               MARK(drawWindow, revDefaultGC, OFFSET_X(tx), OFFSET_Y(ty));
            } else {
               MARKO(drawWindow, revDefaultGC, OFFSET_X(tx), OFFSET_Y(ty));
            }
            if (already_moved) {
            AddPointMeasureCursor(ADDPOINT_ENDSHOW, abs(ABS_X(v[1].x)-tx),
                  abs(ABS_Y(v[1].y)-ty), ABS_X(v[1].x), ABS_Y(v[1].y));
            } else {
               AddPointMeasureCursor(ADDPOINT_STARTSHOW, 0, 0, tx, ty);
            }
            if (already_moved) {
               XDrawLines(mainDisplay, drawWindow, revDefaultGC, sv, sn,
                     CoordModeOrigin);
               if (grid_x == orig_x && grid_y == orig_y) {
                  return FALSE;
               }
            }
            return FALSE;
         }
      }
   }
   return TRUE;
}

void AddPoint()
{
   int adding=TRUE, pt_added=FALSE, root_x=0, root_y=0, old_x=0, old_y=0;
   int curved=(-1);
   struct ObjRec *obj_ptr;
   struct PolyRec *poly_ptr=NULL;
   struct PolygonRec *polygon_ptr=NULL;
   unsigned int status=0;
   Window root_win=None, child_win=None;

   if (!(topSel != NULL && topSel == botSel &&
         (topSel->obj->type == OBJ_POLY || topSel->obj->type == OBJ_POLYGON))) {
      MsgBox(TgLoadString(STID_SELECT_ONLY_ONE_POLY_POLYGON), TOOL_NAME,
            INFO_MB);
      return;
   } else if (topSel->obj->locked) {
      MsgBox(TgLoadString(STID_CANNOT_ADD_PT_FOR_LOCKED), TOOL_NAME, INFO_MB);
      return;
   } else if (AutoRetractedArrowAttr(topSel->obj, TRUE)) {
      MsgBox(TgLoadString(STID_CANNOT_ADD_PT_FOR_AUTO_ARROW), TOOL_NAME,
            INFO_MB);
      return;
   }
   if (curChoice == VERTEXMODE) {
      HighLightReverse();
      JustRemoveAllVSel();
      HighLightForward();
   }
   PrepareToRecord(CMD_REPLACE, topSel, botSel, numObjSelected);
   obj_ptr = topSel->obj;
   switch (obj_ptr->type) {
   case OBJ_POLY:
      poly_ptr = obj_ptr->detail.p;
      curved = poly_ptr->curved;
      break;
   case OBJ_POLYGON:
      polygon_ptr = obj_ptr->detail.g;
      curved = polygon_ptr->curved;
      break;
   }
   SaveStatusStrings();
   if (curved == LT_STRUCT_SPLINE) {
      SetMouseStatus(TgLoadString(STID_ADD_HINGE_VERTEX),
            TgLoadString(STID_ADD_A_SMOOTH_VERTEX),
            TgLoadCachedString(CSTID_FINISH));
      TwoLineMsg(TgLoadString(STID_LEFT_ADD_HINGE_MID_ADD_SMOOTH),
            TgLoadString(STID_CLICK_RIGHT_BUTTON_TO_QUIT));
   } else {
      SetMouseStatus(TgLoadCachedString(CSTID_ADD_A_VERTEX),
            TgLoadCachedString(CSTID_FINISH), TgLoadCachedString(CSTID_FINISH));
      TwoLineMsg(TgLoadString(STID_LEFT_BTN_TO_ADD_PTS),
            TgLoadString(STID_CLICK_OTHER_BUTTON_TO_QUIT));
   }
   if (!debugNoPointerGrab) {
      XGrabPointer(mainDisplay, drawWindow, False,
            PointerMotionMask | ButtonPressMask | ButtonReleaseMask,
            GrabModeAsync, GrabModeAsync, None, defaultCursor, CurrentTime);
   }
   XQueryPointer(mainDisplay, drawWindow, &root_win, &child_win,
         &root_x, &root_y, &old_x, &old_y, &status);
   XSetFont(mainDisplay, revDefaultGC, defaultFontPtr->fid);
   /* do not translate -- program constants */
   XDrawString(mainDisplay, drawWindow, revDefaultGC,
         old_x+4, old_y+defaultFontAsc, "ADD", 3);
   MarkRulers(old_x, old_y);

   while (adding) {
      XEvent input;

      XNextEvent(mainDisplay, &input);

      if (input.type == Expose || input.type == VisibilityNotify) {
         ExposeEventHandler(&input, TRUE);
      } else if (input.type == ButtonPress) {
         int index=0;

         /* erase */
         XDrawString(mainDisplay, drawWindow, revDefaultGC, old_x+4,
               old_y+defaultFontAsc, "ADD", 3);
         if (curved == LT_STRUCT_SPLINE) {
            if (input.xbutton.button == Button1 ||
                  input.xbutton.button == Button2) {
               if (obj_ptr->type == OBJ_POLY &&
                     PtInPolyMark(obj_ptr, input.xbutton.x, input.xbutton.y,
                     poly_ptr->ssn, poly_ptr->ssvlist, &index)) {
                  if (ContinueAddStructuredPolyOrPolygonPoint(obj_ptr,
                        input.xbutton.button, input.xbutton.x, input.xbutton.y,
                        index, poly_ptr, NULL, &old_x, &old_y)) {
                     pt_added = TRUE;
                  }
               } else if (obj_ptr->type == OBJ_POLYGON &&
                     PtInPolyMark(obj_ptr, input.xbutton.x, input.xbutton.y,
                     polygon_ptr->ssn-1, polygon_ptr->ssvlist, &index)) {
                  if (ContinueAddStructuredPolyOrPolygonPoint(obj_ptr,
                        input.xbutton.button, input.xbutton.x, input.xbutton.y,
                        index, NULL, polygon_ptr, &old_x, &old_y)) {
                     pt_added = TRUE;
                  }
               }
               XDrawString(mainDisplay, drawWindow, revDefaultGC, old_x+4,
                     old_y+defaultFontAsc, "ADD", 3);
            } else {
               XUngrabPointer(mainDisplay, CurrentTime);
               Msg("");
               adding = FALSE;
            }
         } else if (input.xbutton.button == Button1) {
            if (obj_ptr->type == OBJ_POLY &&
                  PtInPolyMark(obj_ptr, input.xbutton.x, input.xbutton.y,
                  poly_ptr->n, poly_ptr->vlist, &index)) {
               if (ContinueAddPolyPoint(obj_ptr, input.xbutton.x,
                     input.xbutton.y, index, poly_ptr, &old_x, &old_y)) {
                  pt_added = TRUE;
               }
            } else if (obj_ptr->type == OBJ_POLYGON &&
                  PtInPolyMark(obj_ptr, input.xbutton.x, input.xbutton.y,
                  polygon_ptr->n-1, polygon_ptr->vlist, &index)) {
               if (ContinueAddPolygonPoint(obj_ptr, input.xbutton.x,
                     input.xbutton.y, index, polygon_ptr, &old_x, &old_y)) {
                  pt_added = TRUE;
               }
            }
            XDrawString(mainDisplay, drawWindow, revDefaultGC,
                  old_x+4, old_y+defaultFontAsc, "ADD", 3);
         } else {
            XUngrabPointer(mainDisplay, CurrentTime);
            Msg("");
            adding = FALSE;
         }
      } else if (input.type == MotionNotify) {
         XEvent ev;

         /* erase */
         XDrawString(mainDisplay, drawWindow, revDefaultGC,
               old_x+4, old_y+defaultFontAsc, "ADD", 3);
         old_x = input.xmotion.x;
         old_y = input.xmotion.y;
         /* draw */
         XDrawString(mainDisplay, drawWindow, revDefaultGC,
               old_x+4, old_y+defaultFontAsc, "ADD", 3);
         MarkRulers(old_x, old_y);
         while (XCheckMaskEvent(mainDisplay, PointerMotionMask, &ev)) ;
      }
   }
   RestoreStatusStrings();
   if (pt_added) {
      RecordCmd(CMD_REPLACE, NULL, topSel, botSel, numObjSelected);
   } else {
      AbortPrepareCmd(CMD_REPLACE);
   }
}

void FlushUndoBuffer()
{
   CleanUpMsg();
   CleanUpCmds();
   if (FlushColormap()) {
      Msg(TgLoadString(STID_UNDO_BUF_AND_CMAP_FLUSHED));
      sprintf(gszMsgBox, TgLoadString(STID_NUM_COLORS_ALLOCATED), maxColors);
      Msg(gszMsgBox);
   } else {
      Msg(TgLoadString(STID_UNDO_BUF_FLUSHED));
   }
}

void RestoreImageWH()
{
   struct XBmRec *xbm_ptr=NULL;
   struct XPmRec *xpm_ptr=NULL;
   int w, h, image_w=0, image_h=0;
   int ltx, lty, rbx, rby;

   if (topSel == NULL) {
      MsgBox(TgLoadCachedString(CSTID_NO_OBJ_SELECTED), TOOL_NAME, INFO_MB);
      return;
   } else if (topSel != botSel || (!(topSel->obj->type==OBJ_XBM ||
         topSel->obj->type==OBJ_XPM))) {
      MsgBox(TgLoadString(STID_SEL_ONE_XBM_OR_XPM_TO_RESTORE), TOOL_NAME,
            INFO_MB);
      return;
   } else if (topSel->obj->locked) {
      MsgBox(TgLoadString(STID_CANNOT_RESTORE_LOCKED), TOOL_NAME, INFO_MB);
      return;
   }
   w = topSel->obj->obbox.rbx - topSel->obj->obbox.ltx;
   h = topSel->obj->obbox.rby - topSel->obj->obbox.lty;
   switch (topSel->obj->type) {
   case OBJ_XBM:
      xbm_ptr = topSel->obj->detail.xbm;
      if (xbm_ptr->real_type==XBM_EPS && xbm_ptr->bitmap==None) {
         image_w = xbm_ptr->eps_w;
         image_h = xbm_ptr->eps_h;
      } else {
         image_w = xbm_ptr->image_w;
         image_h = xbm_ptr->image_h;
      }
      if (w == image_w && h == image_h) return;
      break;
   case OBJ_XPM:
      xpm_ptr = topSel->obj->detail.xpm;
      image_w = xpm_ptr->image_w; image_h = xpm_ptr->image_h;
      if (w == image_w && h == image_h) return;
      break;
   }
   ltx = selLtX; lty = selLtY; rbx = selRbX; rby = selRbY;
   HighLightReverse();

   PrepareToReplaceAnObj(topSel->obj);
   if (topSel->obj->ctm == NULL) {
      topSel->obj->obbox.rbx = topSel->obj->obbox.ltx+image_w;
      topSel->obj->obbox.rby = topSel->obj->obbox.lty+image_h;
   } else {
      topSel->obj->obbox.rbx = topSel->obj->obbox.ltx+image_w;
      topSel->obj->obbox.rby = topSel->obj->obbox.lty+image_h;
      free(topSel->obj->ctm);
      topSel->obj->ctm = NULL;
   }
   topSel->obj->x = topSel->obj->obbox.ltx;
   topSel->obj->y = topSel->obj->obbox.lty;
   switch (topSel->obj->type) {
   case OBJ_XBM:
      if (xbm_ptr->cached_bitmap != None) {
         XFreePixmap(mainDisplay, xbm_ptr->cached_bitmap);
      }
      xbm_ptr->cached_bitmap = None;
      xbm_ptr->cached_zoom = 0;
      break;
   case OBJ_XPM:
      if (xpm_ptr->cached_pixmap != None) {
         XFreePixmap(mainDisplay, xpm_ptr->cached_pixmap);
      }
      xpm_ptr->cached_pixmap = None;
      if (xpm_ptr->cached_bitmap != None) {
         XFreePixmap(mainDisplay, xpm_ptr->cached_bitmap);
      }
      xpm_ptr->cached_bitmap = None;
      if (xpm_ptr->clip_mask != None) {
         XFreePixmap(mainDisplay, xpm_ptr->clip_mask);
      }
      xpm_ptr->clip_mask = None;
      xpm_ptr->cached_zoom = 0;
      xpm_ptr->cached_color = (-1);
      break;
   }
   AdjObjBBox(topSel->obj);
   RecordReplaceAnObj(topSel->obj);

   UpdSelBBox();
   RedrawAreas(botObj, ltx-GRID_ABS_SIZE(1), lty-GRID_ABS_SIZE(1),
         rbx+GRID_ABS_SIZE(1), rby+GRID_ABS_SIZE(1),
         selLtX-GRID_ABS_SIZE(1), selLtY-GRID_ABS_SIZE(1),
         selRbX+GRID_ABS_SIZE(1), selRbY+GRID_ABS_SIZE(1));
   HighLightForward();
   SetFileModified(TRUE);
   justDupped = FALSE;
}

void CutMaps()
{
   if (topSel == NULL || topSel != botSel) {
      MsgBox(TgLoadString(STID_SEL_ONE_XBM_OR_XPM_TO_CUT), TOOL_NAME,
            INFO_MB);
      return;
   } else if (topSel->obj->locked) {
      MsgBox(TgLoadString(STID_CANNOT_CUT_LOCKED), TOOL_NAME, INFO_MB);
      return;
   } else if (topSel->obj->ctm != NULL) {
      MsgBox(TgLoadString(STID_CANNOT_CUT_TRANSFORMED_X_OBJ), TOOL_NAME,
            INFO_MB);
      return;
   }
   switch (topSel->obj->type) {
   case OBJ_XBM: CutXBitmap(); break;
   case OBJ_XPM: CutXPixmap(NULL, NULL, NULL, NULL, NULL); break;
   default:
      MsgBox(TgLoadString(STID_SEL_ONE_XBM_OR_XPM_TO_CUT), TOOL_NAME,
            INFO_MB);
      break;
   }
}

void BreakUpMaps()
{
   int cols=0, rows=0, cols_and_rows=TRUE, ok=TRUE, image_w=0, image_h=0;
   char spec[MAXSTRING+1], *dup_spec=NULL, *part1=NULL, *part2=NULL;
   struct ObjRec *obj_ptr=NULL;

   if (topSel == NULL || topSel != botSel ||
         (topSel->obj->type != OBJ_XBM && topSel->obj->type != OBJ_XPM)) {
      MsgBox(TgLoadString(STID_SEL_ONE_XBM_OR_XPM_TO_BREAKUP), TOOL_NAME,
            INFO_MB);
      return;
   } else if (topSel->obj->ctm != NULL) {
      MsgBox(TgLoadString(STID_CANNOT_BREAK_XFORMED_X_OBJ), TOOL_NAME,
            INFO_MB);
      return;
   } else if (topSel->obj->locked) {
      MsgBox(TgLoadString(STID_CANNOT_BREAKUP_LOCKED), TOOL_NAME,
            INFO_MB);
      return;
   }
   obj_ptr = topSel->obj;

   switch (obj_ptr->type) {
   case OBJ_XBM:
      image_w=obj_ptr->detail.xbm->image_w;
      image_h=obj_ptr->detail.xbm->image_h;
      break;
   case OBJ_XPM:
      image_w=obj_ptr->detail.xpm->image_w;
      image_h=obj_ptr->detail.xpm->image_h;
      break;
   default: return;
   }
   sprintf(gszMsgBox, TgLoadString(STID_ENTER_NUM_ROWCOL_TO_BREAK),
         image_w, image_h);
   *spec = '\0';
   if (Dialog(gszMsgBox, TgLoadCachedString(CSTID_DLG_ACCEPT_CANCEL), spec) ==
         INVALID) {
      return;
   }
   UtilTrimBlanks(spec);
   if (*spec == '\0') return;
   if ((dup_spec=UtilStrDup(spec)) == NULL) {
      FailAllocMessage();
      return;
   }
   if (*dup_spec == '=') cols_and_rows = FALSE;

   /* do not translate -- program constants */
   if ((part1=strtok(dup_spec, " ,xX=[]")) != NULL &&
         (part2=strtok(NULL, " ,xX=[]")) != NULL) {
      cols = atoi(part1);
      rows = atoi(part2);
      if (cols > 0 && rows > 0) {
         switch (obj_ptr->type) {
         case OBJ_XBM:
            BreakUpXBitmap(obj_ptr, cols_and_rows, cols, rows);
            break;
         case OBJ_XPM:
            BreakUpXPixmap(obj_ptr, cols_and_rows, cols, rows);
            break;
         }
      } else {
         ok = FALSE;
      }
   } else {
      ok = FALSE;
   }
   if (!ok) {
      sprintf(gszMsgBox, TgLoadString(STID_INVALID_ROWCOL_SPEC_REENTER),
            dup_spec);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
   }
   free(dup_spec);
}

/* ----------------------- RemoveTransparentPixel ----------------------- */

static
int ReproducePixmap(pixel_to_use, orig_pixmap, orig_image, orig_bitmap,
      orig_bitmap_image, w, h, pixmap_return, image_return, bitmap_return,
      bitmap_image_return)
   int pixel_to_use, w, h;
   Pixmap orig_pixmap, orig_bitmap, *pixmap_return, *bitmap_return;
   XImage *orig_image, *orig_bitmap_image, **image_return, **bitmap_image_return;
{
   int i=0;
   XImage *src_image=NULL, *src_bitmap_image=NULL;

   SetWatchCursor(drawWindow);
   SetWatchCursor(mainWindow);

   *pixmap_return = XCreatePixmap(mainDisplay, mainWindow, w, h, mainDepth);
   *bitmap_return = XCreatePixmap(mainDisplay, mainWindow, w, h, 1);

   XFillRectangle(mainDisplay, *pixmap_return, xpmGC, 0, 0, w, h);
   XSetForeground(mainDisplay, xbmGC, 1);
   XFillRectangle(mainDisplay, *bitmap_return, xbmGC, 0, 0, w, h);
   XSetForeground(mainDisplay, xbmGC, 0);

   *image_return = (*pixmap_return==None ? NULL : XGetImage(mainDisplay,
         *pixmap_return, 0, 0, w, h, AllPlanes, ZPixmap));
   *bitmap_image_return = (*bitmap_return==None ? NULL : XGetImage(mainDisplay,
         *bitmap_return, 0, 0, w, h, 1, ZPixmap));
   if (orig_image != NULL) {
      src_image = orig_image;
   } else {
      src_image = XGetImage(mainDisplay, orig_pixmap, 0, 0, w, h, AllPlanes, ZPixmap);
   }
   if (orig_bitmap_image != NULL) {
      src_bitmap_image = orig_bitmap_image;
   } else {
      src_bitmap_image = XGetImage(mainDisplay, orig_bitmap, 0, 0, w, h, 1, ZPixmap);
   }
   if (*pixmap_return == None || *bitmap_return == None || *image_return==NULL ||
         *bitmap_image_return==NULL || src_image==NULL || src_bitmap_image==NULL) {
      if (*pixmap_return == None) {
         FailAllocPixmapMessage(w, h);
      } else if (*bitmap_return == None) {
         FailAllocBitmapMessage(w, h);
      } else {
         MsgBox(TgLoadString(STID_XGETIMAGE_MAY_RUN_OUT_VMEM), TOOL_NAME,
               INFO_MB);
      }
      if (*pixmap_return != None) XFreePixmap(mainDisplay, *pixmap_return);
      if (*bitmap_return != None) XFreePixmap(mainDisplay, *bitmap_return);
      if (*image_return != NULL) XDestroyImage(*image_return);
      if (*bitmap_image_return != NULL) XDestroyImage(*bitmap_image_return);
      if (orig_image != NULL) XDestroyImage(src_image);
      if (orig_bitmap_image != NULL) XDestroyImage(src_bitmap_image);
      if (orig_image == NULL && src_image != NULL) XDestroyImage(src_image);
      if (orig_bitmap_image == NULL && src_bitmap_image != NULL) XDestroyImage(src_bitmap_image);

      *pixmap_return = *bitmap_return = None;
      *image_return = *bitmap_image_return = NULL;
      SetDefaultCursor(mainWindow);
      SetDefaultCursor(drawWindow);
      return FALSE;
   }
   for (i=0; i < h; i++) {
      int j=0;

      for (j=0; j < w; j++) {
         if (src_bitmap_image == NULL) {
            XPutPixel(*image_return, j, i, XGetPixel(src_image, j, i));
         } else {
            if (XGetPixel(src_bitmap_image, j, i) != 0) {
               XPutPixel(*image_return, j, i, XGetPixel(src_image, j, i));
            } else {
               XPutPixel(*image_return, j, i, pixel_to_use);
            }
         }
      }
   }
   XPutImage(mainDisplay, *pixmap_return, xpmGC, *image_return, 0, 0, 0, 0, w, h);
   XPutImage(mainDisplay, *bitmap_return, xbmGC, *bitmap_image_return, 0, 0, 0, 0, w, h);
   SetDefaultCursor(mainWindow);
   SetDefaultCursor(drawWindow);

   if (orig_image == NULL && src_image != NULL) XDestroyImage(src_image);
   if (orig_bitmap_image == NULL && src_bitmap_image != NULL) XDestroyImage(src_bitmap_image);

   return TRUE;
}

static
int RemoveObjTransPixel(obj_ptr, ptci)
   struct ObjRec *obj_ptr;
   TrueColorInfo *ptci;
{
   struct XPmRec *xpm_ptr=obj_ptr->detail.xpm;
   Pixmap dest_pixmap=None, dest_bitmap=None;
   XImage *dest_image=NULL, *dest_bitmap_image=NULL;
   int index=(-1);
   unsigned char trans_color_r='\0', trans_color_g='\0', trans_color_b='\0';

   switch (xpm_ptr->real_type) {
   case XPM_XPM:
      if (ObjHasIndexedTransPixel(obj_ptr, &index)) {
         xpm_ptr->pixels[index] = colorPixels[colorIndex];
         UtilFree(xpm_ptr->color_str[index]);
         xpm_ptr->color_str[index] = UtilStrDup(colorMenuItems[colorIndex]);
         if (xpm_ptr->color_str[index] == NULL) FailAllocMessage();
         if (!ReproducePixmap(colorPixels[colorIndex], xpm_ptr->pixmap, xpm_ptr->image, xpm_ptr->bitmap,
               xpm_ptr->bitmap_image, xpm_ptr->image_w, xpm_ptr->image_h,
               &dest_pixmap, &dest_image, &dest_bitmap, &dest_bitmap_image)) {
            return FALSE;
         }
         xpm_ptr->pixmap = dest_pixmap;
         xpm_ptr->image = dest_image;
         xpm_ptr->bitmap = dest_bitmap;
         xpm_ptr->bitmap_image = dest_bitmap_image;
         return TRUE;
      }
      break;
   case XPM_JPEG: break;
   case PPM_TRUE:
      if (ObjHasTrueColorTransPixel(obj_ptr, &trans_color_r, &trans_color_g, &trans_color_b)) {
         unsigned int r=(unsigned int)trans_color_r;
         unsigned int g=(unsigned int)trans_color_g;
         unsigned int b=(unsigned int)trans_color_b;
         int pixel_to_use=((r << ptci->r_shift) & mainVisual->red_mask) |
                 ((g << ptci->g_shift) & mainVisual->green_mask) |
                 ((b << ptci->b_shift) & mainVisual->blue_mask) ;

         if (!ReproducePixmap(pixel_to_use, xpm_ptr->pixmap, xpm_ptr->image, xpm_ptr->bitmap,
               xpm_ptr->bitmap_image, xpm_ptr->image_w, xpm_ptr->image_h,
               &dest_pixmap, &dest_image, &dest_bitmap, &dest_bitmap_image)) {
            return FALSE;
         }
         xpm_ptr->pixmap = dest_pixmap;
         xpm_ptr->image = dest_image;
         xpm_ptr->bitmap = dest_bitmap;
         xpm_ptr->bitmap_image = dest_bitmap_image;
         xpm_ptr->has_transparent_color = FALSE;
         xpm_ptr->transparent_color[0] = (unsigned char)0;
         xpm_ptr->transparent_color[1] = (unsigned char)0;
         xpm_ptr->transparent_color[2] = (unsigned char)0;
         return TRUE;
      }
      break;
   }
   /* no change */
   return FALSE;
}

void RemoveTransparentPixel()
{
   struct ObjRec *obj_ptr=NULL;
   struct SelRec *sel_ptr=NULL, *tmp_top_sel=NULL, *tmp_bot_sel=NULL;
   int count=0, ltx=0, lty=0, rbx=0, rby=0;
   TrueColorInfo tci;

   for (sel_ptr=topSel; sel_ptr != NULL; sel_ptr=sel_ptr->next) {
      obj_ptr = sel_ptr->obj;
      if (obj_ptr->type == OBJ_XPM && ObjHasTransPixel(obj_ptr)) {
         AddObjIntoSel(obj_ptr, tmp_bot_sel, NULL, &tmp_top_sel, &tmp_bot_sel);
         count++;
      }
   }
   if (count == 0) {
      MsgBox(TgLoadString(STID_SEL_ONE_XPM_TRANSPIX), TOOL_NAME, INFO_MB);
      return;
   }
   if (fullTrueColorMode && !SetupTrueColorInfo(&tci)) return;

   HighLightReverse();
   RemoveAllSel();
   topSel = tmp_top_sel;
   botSel = tmp_bot_sel;
   tmp_top_sel = tmp_bot_sel = NULL;
   ltx = selLtX; lty = selLtY; rbx = selRbX; rby = selRbY;

   StartCompositeCmd();
   for (sel_ptr=topSel; sel_ptr != NULL; sel_ptr=sel_ptr->next) {
      obj_ptr = sel_ptr->obj;
      PrepareToReplaceAnObj(obj_ptr);

      if (RemoveObjTransPixel(obj_ptr, &tci)) {
         RecordReplaceAnObj(obj_ptr);
         AdjObjCache(obj_ptr);
      } else {
         AbortPrepareCmd(CMD_REPLACE);
      }
   }
   EndCompositeCmd();

   UpdSelBBox();
   RedrawAnArea(botObj, ltx-GRID_ABS_SIZE(1), lty-GRID_ABS_SIZE(1),
      rbx+GRID_ABS_SIZE(1), rby+GRID_ABS_SIZE(1));
   HighLightForward();
   SetFileModified(TRUE);
   justDupped = FALSE;
}

/* ----------------------- LayoutOnArc ----------------------- */

typedef struct BoxInfoRec {
   struct ObjRec *obj;
   int w, h;
   int valid_v;
   double half_w, angle_in_radian, angle_to_rotate;
   XPoint v[5];
} *BoxInfoPtr;

#define LAYOUT_DIR_NONE 0
#define LAYOUT_DIR_S 1 /* convex */
#define LAYOUT_DIR_N 2 /* concave */

int gnLayoutDirection=LAYOUT_DIR_NONE;

static
void RotateXY(x, y, angle_in_radian, new_x, new_y)
   int x, y;
   double angle_in_radian;
   short *new_x, *new_y;
{
   if (x == 0 && y == 0) {
      *new_x = 0;
      *new_y = 0;
   } else {
      double sin_val = sin(angle_in_radian);
      double cos_val = cos(angle_in_radian);

      *new_x = (short)round(x*cos_val - y*sin_val);
      *new_y = (short)round(x*sin_val + y*cos_val);
   }
}

static
void RotateBBoxByRadian(bbox, angle_in_radian, v)
   struct BBRec *bbox;
   double angle_in_radian;
   XPoint *v; /* array of 5 points */
{
   RotateXY(bbox->ltx, bbox->lty, angle_in_radian, &(v[0].x), &(v[0].y));
   RotateXY(bbox->rbx, bbox->lty, angle_in_radian, &(v[1].x), &(v[1].y));
   RotateXY(bbox->rbx, bbox->rby, angle_in_radian, &(v[2].x), &(v[2].y));
   RotateXY(bbox->ltx, bbox->rby, angle_in_radian, &(v[3].x), &(v[3].y));
   v[4].x = v[0].x; v[4].y = v[0].y;
}

#define SET_LAYOUT_VS (FALSE)
#define FINALIZE_LAYOUT (TRUE)

static
int HighLightOrFinalLayout(arc_ptr, box_info_ptr, grid_x, grid_y, finalize)
   struct ArcRec *arc_ptr;
   struct BoxInfoRec *box_info_ptr;
   int grid_x, grid_y, finalize;
{
   int i, num_objects=numObjSelected-1, circular=FALSE;
   int abs_cx, abs_cy, abs_x=ABS_X(grid_x), abs_y=ABS_Y(grid_y), dx, dy;
   double abs_radius, total_arc_radian, total_radian=0, inc_radian=0, angle;

   if (abs(arc_ptr->angle2) == (360<<6)) {
      circular = TRUE;
   }
   abs_cx = arc_ptr->xc;
   abs_cy = arc_ptr->yc;
   dx = abs_x - abs_cx;
   dy = abs_y - abs_cy;
   if (dx == 0 && dy == 0) {
      box_info_ptr[0].valid_v = FALSE;
      return FALSE;
   }
   box_info_ptr[0].valid_v = TRUE;
   total_arc_radian = ((double)arc_ptr->angle2)*M_PI/((double)180.0*64.0);
   abs_radius = (double)sqrt((double)(dx*dx+dy*dy));
   for (i=0; i < num_objects; i++) {
      if (!finalize) {
         box_info_ptr[i].angle_in_radian = atan2(box_info_ptr[i].half_w,
               abs_radius) * ((double)2.0);
      }
      total_radian += fabs(box_info_ptr[i].angle_in_radian);
   }
   switch (gnLayoutDirection) {
   case LAYOUT_DIR_S:
      if (circular) {
         inc_radian = (total_arc_radian-total_radian)/((double)num_objects);
      } else {
         inc_radian = (total_arc_radian-total_radian)/((double)(num_objects-1));
      }
      break;
   case LAYOUT_DIR_N:
      if (circular) {
         inc_radian = (total_arc_radian+total_radian)/((double)num_objects);
      } else {
         inc_radian = (total_arc_radian+total_radian)/((double)(num_objects-1));
      }
      break;
   }
   angle = arc_ptr->angle1*M_PI/((double)180.0*64.0);
   for (i=0; i < num_objects; i++, angle+=inc_radian) {
      struct ObjRec *obj_ptr=box_info_ptr[i].obj;
      int x=0, y=0, w=0, h=0, orig_x=0, orig_y=0;
      XPoint v[5];
      struct BBRec bbox;
      double half_way_angle=(double)0.0;

      w = box_info_ptr[i].w;
      h = box_info_ptr[i].h;
      if (finalize) {
         if (obj_ptr->type == OBJ_TEXT &&
               obj_ptr->detail.t->minilines.just != JUST_C) {
            ChangeObjTextJust(obj_ptr, JUST_C);
         }
      }
      /*
       * half_way_angle measures angle in the arc sense (see the beginning
       *     of "arc.c").
       * the RoateaBBoxByRadian() rotates things in the drawing sense
       *     (the one that's vertically flipped w.r.t. PostScript)
       */
      switch (gnLayoutDirection) {
      case LAYOUT_DIR_S:
         half_way_angle = angle+(box_info_ptr[i].angle_in_radian/2.0);
         if (finalize) {
            orig_x = ((obj_ptr->obbox.rbx+obj_ptr->obbox.ltx)>>1);
            orig_y = obj_ptr->obbox.lty;
            RotateObjForLayout(obj_ptr, box_info_ptr[i].angle_to_rotate,
                  CORNER_BOTTOM);
            x = abs_cx + (int)((abs_radius-h)*cos(half_way_angle));
            y = abs_cy - (int)((abs_radius-h)*sin(half_way_angle));
         } else {
            SetBBRec(&bbox, -(w>>1), -h, w-(w>>1), 0);
            box_info_ptr[i].angle_to_rotate = (-half_way_angle-(M_PI/2.0));
            RotateBBoxByRadian(&bbox, box_info_ptr[i].angle_to_rotate, v);
            x = abs_cx + (int)(abs_radius*cos(half_way_angle));
            y = abs_cy - (int)(abs_radius*sin(half_way_angle));
         }
         break;
      case LAYOUT_DIR_N:
         half_way_angle = angle-(box_info_ptr[i].angle_in_radian/2.0);
         if (finalize) {
            orig_x = ((obj_ptr->obbox.rbx+obj_ptr->obbox.ltx)>>1);
            orig_y = obj_ptr->obbox.rby;
            RotateObjForLayout(obj_ptr, box_info_ptr[i].angle_to_rotate,
                  CORNER_TOP);
            x = abs_cx + (int)((abs_radius-h)*cos(half_way_angle));
            y = abs_cy - (int)((abs_radius-h)*sin(half_way_angle));
         } else {
            SetBBRec(&bbox, -(w>>1), 0, w-(w>>1), h);
            box_info_ptr[i].angle_to_rotate = (-half_way_angle+(M_PI/2.0));
            RotateBBoxByRadian(&bbox, box_info_ptr[i].angle_to_rotate, v);
            x = abs_cx + (int)(abs_radius*cos(half_way_angle));
            y = abs_cy - (int)(abs_radius*sin(half_way_angle));
         }
         break;
      }
      if (finalize) {
         MoveObj(obj_ptr, x-orig_x, y-orig_y);
      } else {
         box_info_ptr[i].v[0].x = OFFSET_X(x + v[0].x);
         box_info_ptr[i].v[0].y = OFFSET_Y(y + v[0].y);
         box_info_ptr[i].v[1].x = OFFSET_X(x + v[1].x);
         box_info_ptr[i].v[1].y = OFFSET_Y(y + v[1].y);
         box_info_ptr[i].v[2].x = OFFSET_X(x + v[2].x);
         box_info_ptr[i].v[2].y = OFFSET_Y(y + v[2].y);
         box_info_ptr[i].v[3].x = OFFSET_X(x + v[3].x);
         box_info_ptr[i].v[3].y = OFFSET_Y(y + v[3].y);
         box_info_ptr[i].v[4].x = box_info_ptr[i].v[0].x;
         box_info_ptr[i].v[4].y = box_info_ptr[i].v[0].y;
      }
      switch (gnLayoutDirection) {
      case LAYOUT_DIR_S:
         angle += box_info_ptr[i].angle_in_radian;
         break;
      case LAYOUT_DIR_N:
         angle -= box_info_ptr[i].angle_in_radian;
         break;
      }
   }
   return TRUE;
}

#define NO_GENERATE 0
#define GENERATE 1

static
void HighLightLayout(arc_ptr, box_info_ptr, grid_x, grid_y, generate)
   struct ArcRec *arc_ptr;
   struct BoxInfoRec *box_info_ptr;
   int grid_x, grid_y, generate;
{
   int i, num_objects=numObjSelected-1;

   if (generate) {
      HighLightOrFinalLayout(arc_ptr, box_info_ptr, grid_x, grid_y,
            SET_LAYOUT_VS);
   }
   if (box_info_ptr[0].valid_v) {
      for (i=0; i < num_objects; i++) {
         XDrawLines(mainDisplay, drawWindow, revDefaultGC, box_info_ptr[i].v, 5,
               CoordModeOrigin);
      }
   }
}

static
void DoLayoutOnArc(arc_obj, box_info_ptr)
   struct ObjRec *arc_obj;
   struct BoxInfoRec *box_info_ptr;
{
   struct ArcRec *arc_ptr=arc_obj->detail.a;
   int done=FALSE, something_changed=FALSE, grid_x=0, grid_y=0;

   SetMouseStatus(TgLoadCachedString(CSTID_START_LAYOUT_ON_ARC),
         TgLoadCachedString(CSTID_FINISH), TgLoadCachedString(CSTID_FINISH));
   if (!debugNoPointerGrab) {
      XGrabPointer(mainDisplay, drawWindow, FALSE,
            PointerMotionMask | ButtonPressMask | ButtonReleaseMask,
            GrabModeAsync, GrabModeAsync, None, handCursor, CurrentTime);
   }
   while (!done) {
      XEvent input;

      XNextEvent(mainDisplay, &input);
      switch (input.type) {
      case Expose: ExposeEventHandler(&input, TRUE); break;
      case VisibilityNotify: ExposeEventHandler(&input, TRUE); break;
      case ButtonPress:
         if (input.xbutton.button == Button1) {
            SetMouseStatus(TgLoadCachedString(CSTID_END_LAYOUT_ON_ARC), "", "");
            GridXY(input.xbutton.x, input.xbutton.y, &grid_x, &grid_y);
            HighLightLayout(arc_ptr, box_info_ptr, grid_x, grid_y, GENERATE);
            something_changed = TRUE;
         } else {
            XUngrabPointer(mainDisplay, CurrentTime);
            XSync(mainDisplay, False);
            done = TRUE;
         }
         break;
      case MotionNotify:
         if (something_changed) {
            HighLightLayout(arc_ptr, box_info_ptr, grid_x, grid_y, NO_GENERATE);
            GridXY(input.xmotion.x, input.xmotion.y, &grid_x, &grid_y);
            HighLightLayout(arc_ptr, box_info_ptr, grid_x, grid_y, GENERATE);
         }
         break;
      case ButtonRelease:
         XUngrabPointer(mainDisplay, CurrentTime);
         XSync(mainDisplay, False);
         done = TRUE;
         HighLightLayout(arc_ptr, box_info_ptr, grid_x, grid_y, NO_GENERATE);
      }
   }
   if (something_changed && box_info_ptr[0].valid_v) {
      int ltx=selLtX, lty=selLtY, rbx=selRbX, rby=selRbY;

      HighLightReverse();
      PrepareToRecord(CMD_REPLACE, topSel, botSel, numObjSelected);
      if (HighLightOrFinalLayout(arc_ptr, box_info_ptr, grid_x, grid_y,
            FINALIZE_LAYOUT)) {
         RecordCmd(CMD_REPLACE, NULL, topSel, botSel, numObjSelected);
         UpdSelBBox();
         RedrawAreas(botObj, ltx-GRID_ABS_SIZE(1), lty-GRID_ABS_SIZE(1),
               rbx+GRID_ABS_SIZE(1), rby+GRID_ABS_SIZE(1),
               selLtX-GRID_ABS_SIZE(1), selLtY-GRID_ABS_SIZE(1),
               selRbX+GRID_ABS_SIZE(1), selRbY+GRID_ABS_SIZE(1));
         SetFileModified(TRUE);
         justDupped = FALSE;
      } else {
         AbortPrepareCmd(CMD_REPLACE);
      }
      HighLightForward();
   }
}

static
int ObjsAlreadySorted(box_info_ptr, min_index, max_index)
   struct BoxInfoRec *box_info_ptr;
   int min_index, max_index;
{
   int i;

   for (i=min_index; i < max_index; i++) {
      int d=box_info_ptr[i].obj->obbox.ltx-box_info_ptr[i+1].obj->obbox.ltx;

      if (d > 0) {
         return FALSE;
      } else if (d == 0 &&
            box_info_ptr[i].obj->obbox.lty > box_info_ptr[i+1].obj->obbox.lty) {
         return FALSE;
      }
   }
   return TRUE;
}

static
void QuickSortObjs(box_info_ptr, min_index, max_index, level)
   struct BoxInfoRec *box_info_ptr;
   int min_index, max_index, level;
{
   int i, j, pivot_index, pivot_x_value, pivot_y_value, something_swapped;
   struct ObjRec *tmp_obj;

   if (min_index > max_index) return;
   if (ObjsAlreadySorted(box_info_ptr, min_index, max_index)) return;
   pivot_index = max_index;
   pivot_x_value = box_info_ptr[pivot_index].obj->obbox.ltx;
   pivot_y_value = box_info_ptr[pivot_index].obj->obbox.lty;
   i = min_index;
   j = max_index-1;
   something_swapped = FALSE;
   do {
      int d;

      while (TRUE) {
         d = box_info_ptr[i].obj->obbox.ltx-pivot_x_value;
         if (d < 0 || (d == 0 &&
               box_info_ptr[i].obj->obbox.lty < pivot_y_value)) {
            i++;
         } else {
            break;
         }
      }
      while (j > i) {
         d = box_info_ptr[j].obj->obbox.ltx-pivot_x_value;
         if (d > 0 || (d == 0 &&
               box_info_ptr[j].obj->obbox.lty > pivot_y_value)) {
            j--;
         } else {
            break;
         }
      }
      if (j > i) {
         tmp_obj = box_info_ptr[j].obj;
         box_info_ptr[j].obj = box_info_ptr[i].obj;
         box_info_ptr[i].obj = tmp_obj;
         something_swapped = TRUE;
         if (j == i+1) break;
         i++; j--;
      } else {
         break;
      }
   } while (TRUE);
   if (i == max_index) {
      /* pivot corresponds to the largest */
      if (something_swapped) {
#ifdef _TGIF_DBG /* debug, do not translate */
         fprintf(stderr, "Huh? min_index=%1d, max_index=%1d, level=%1d\n",
               min_index, max_index, level);
#endif /* _TGIF_DBG */
      } else {
         QuickSortObjs(box_info_ptr, min_index, j, level+1);
      }
   } else if (j > i) {
      tmp_obj = box_info_ptr[max_index].obj;
      box_info_ptr[max_index].obj = box_info_ptr[j].obj;
      box_info_ptr[j].obj = tmp_obj;
      QuickSortObjs(box_info_ptr, min_index, j-1, level+1);
      QuickSortObjs(box_info_ptr, j+1, max_index, level+1);
   } else {
      tmp_obj = box_info_ptr[max_index].obj;
      box_info_ptr[max_index].obj = box_info_ptr[i].obj;
      box_info_ptr[i].obj = tmp_obj;
      QuickSortObjs(box_info_ptr, min_index, i-1, level+1);
      QuickSortObjs(box_info_ptr, i+1, max_index, level+1);
   }
}

static
int DecideLayoutDirection(obj_ptr)
   struct ObjRec *obj_ptr;
{
   struct ArcRec *arc_ptr=obj_ptr->detail.a;
   struct BBRec *p_obbox=(&obj_ptr->obbox);
   int cx=arc_ptr->xc, cy=arc_ptr->yc, h_slack, v_slack;
   char spec[MAXSTRING+1];

   h_slack = ((p_obbox->ltx+p_obbox->rbx)>>1) - cx;
   v_slack = ((p_obbox->lty+p_obbox->rby)>>1) - cy;
   if (h_slack == 0) {
      if (v_slack == 0) {
         return LAYOUT_DIR_S;
      } else if (v_slack > 0) {
         return LAYOUT_DIR_S;
      } else {
         return LAYOUT_DIR_N;
      }
   }
   *spec = '\0';
   if (Dialog(TgLoadString(STID_ENTER_CONCAVE_OR_CONVEX), NULL, spec) ==
         INVALID) {
      return LAYOUT_DIR_NONE;
   }
   UtilTrimBlanks(spec);
   /* do not translate -- program constants */
   if (UtilStrICmp(spec, "concave") == 0) {
      return LAYOUT_DIR_N;
   } else if (UtilStrICmp(spec, "convex") == 0) {
      return LAYOUT_DIR_S;
   } else {
      switch (*spec) {
      case 'c': case 'C': return LAYOUT_DIR_N;
      case 'v': case 'V': return LAYOUT_DIR_S;
      }
   }
   sprintf(gszMsgBox, TgLoadString(STID_INVALID_SPEC), spec);
   MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
   return LAYOUT_DIR_NONE;
}

void LayoutOnArc()
{
   struct SelRec *sel_ptr;
   struct ObjRec *arc_obj=NULL;
   int arc_obj_count=0, i, something_locked=FALSE;
   struct BoxInfoRec *box_info_ptr;

   if (curChoice != NOTHING) {
      MsgBox(TgLoadString(STID_SEL_AN_ARC_OBJ), TOOL_NAME, INFO_MB);
      return;
   }
   for (sel_ptr=topSel; sel_ptr != NULL; sel_ptr=sel_ptr->next) {
      struct ObjRec *obj_ptr=sel_ptr->obj;

      if (obj_ptr->type == OBJ_ARC) {
         arc_obj_count++;
         arc_obj = obj_ptr;
      } else if (obj_ptr->locked) {
         something_locked = TRUE;
      }
   }
   if (arc_obj_count == 0) {
      MsgBox(TgLoadString(STID_NO_ARC_OBJ_SELECTED), TOOL_NAME, INFO_MB);
      return;
   } else if (arc_obj_count > 1) {
      MsgBox(TgLoadString(STID_TOO_MANY_ARC_SEL_ONLY_ONE_ARC), TOOL_NAME,
            INFO_MB);
      return;
   } else if (numObjSelected == 1) {
      MsgBox(TgLoadString(STID_NO_OTHER_OBJ_FOR_LAYOUTONARC), TOOL_NAME,
            INFO_MB);
      return;
   } else if (something_locked) {
      MsgBox(TgLoadString(STID_CANNOT_LAYOUTONARC_LOCKED), TOOL_NAME, INFO_MB);
      return;
   }
   if (arc_obj->ctm != NULL || arc_obj->detail.a->w != arc_obj->detail.a->h) {
      MsgBox(TgLoadString(STID_ARC_XFORMED_FOR_LAYOUTONARC), TOOL_NAME,
            INFO_MB);
      return;
   }
   gnLayoutDirection = DecideLayoutDirection(arc_obj);
   if (gnLayoutDirection == LAYOUT_DIR_NONE) {
      return;
   }
   box_info_ptr = (struct BoxInfoRec *)malloc(
         (numObjSelected-1)*sizeof(struct BoxInfoRec));
   if (box_info_ptr == NULL) {
      FailAllocMessage();
      return;
   }
   for (i=0, sel_ptr=topSel; sel_ptr != NULL; sel_ptr=sel_ptr->next) {
      struct ObjRec *obj_ptr=sel_ptr->obj;

      if (obj_ptr->type != OBJ_ARC) {
         box_info_ptr[i].obj = obj_ptr;
         i++;
      }
   }
   QuickSortObjs(box_info_ptr, 0, numObjSelected-2, 0);
   for (i=0; i < numObjSelected-1; i++) {
      struct ObjRec *obj_ptr=box_info_ptr[i].obj;

      box_info_ptr[i].w = obj_ptr->obbox.rbx-obj_ptr->obbox.ltx;
      box_info_ptr[i].h = obj_ptr->obbox.rby-obj_ptr->obbox.lty;
      box_info_ptr[i].half_w = (double)(box_info_ptr[i].w>>1);
      box_info_ptr[i].valid_v = FALSE;
   }
   SaveStatusStrings();
   DoLayoutOnArc(arc_obj, box_info_ptr);
   RestoreStatusStrings();
   free(box_info_ptr);
}

/* ----------------------- PreciseRotate ----------------------- */

static
int FinishPreciseRotate(angle_spec, pivot_x, pivot_y)
   double angle_spec;
   int pivot_x, pivot_y;
{
   struct SelRec *sel_ptr;
   double angle_in_radian=angle_spec*M_PI/180.0;
   double sin_val=sin(angle_in_radian);
   double cos_val=cos(angle_in_radian);

   for (sel_ptr=topSel; sel_ptr != NULL; sel_ptr=sel_ptr->next) {
      struct ObjRec *obj_ptr=sel_ptr->obj;
      int orig_x=((obj_ptr->obbox.ltx+obj_ptr->obbox.rbx)>>1);
      int orig_y=obj_ptr->obbox.lty;
      int x=0, y=0, dx=orig_x-pivot_x, dy=orig_y-pivot_y;

      if (dx != 0 || dy != 0) {
         x = (short)round(dx*cos_val - dy*sin_val);
         y = (short)round(dx*sin_val + dy*cos_val);
      }
      x += pivot_x;
      y += pivot_y;
      /* RotateObjForLayout() rotates about center-top */
      RotateObjForLayout(obj_ptr, angle_in_radian, CORNER_BOTTOM);
      MoveObj(obj_ptr, x-orig_x, y-orig_y);
   }
   return TRUE;
}

void PreciseRotate()
{
   char spec[MAXSTRING+1];
   double angle_spec;
   int arc_count=0, pivot_x=0, pivot_y=0, ltx, lty, rbx, rby;
   struct SelRec *sel_ptr;
   struct ObjRec *arc_obj=NULL;

   if (curChoice == VERTEXMODE) {
      MsgBox(TgLoadString(STID_ROT_NOT_AVAIL_ON_VERTEX_MODE), TOOL_NAME,
            INFO_MB);
      return;
   } else if (curChoice != NOTHING || topSel == NULL) {
      MsgBox(TgLoadCachedString(CSTID_NO_OBJ_SELECTED), TOOL_NAME, INFO_MB);
      return;
   }
   for (sel_ptr=topSel; sel_ptr != NULL; sel_ptr=sel_ptr->next) {
      if (sel_ptr->obj->type == OBJ_ARC) {
         arc_obj = sel_ptr->obj;
         arc_count++;
      }
   }
   if (!autoRotatePivot && rotatePivotAbsXYValid) {
      pivot_x = rotatePivotAbsX;
      pivot_y = rotatePivotAbsY;
   } else {
      if (arc_count == 1) {
         if (arc_obj->ctm == NULL) {
            pivot_x = arc_obj->detail.a->xc;
            pivot_y = arc_obj->detail.a->yc;
         } else {
            struct ArcRec *arc_ptr=arc_obj->detail.a;
            int x, y;

            TransformPointThroughCTM(arc_ptr->xc-arc_obj->x,
                  arc_ptr->yc-arc_obj->y, arc_obj->ctm, &x, &y);
            pivot_x = x + arc_obj->x;
            pivot_y = y + arc_obj->y;
         }
      } else {
         pivot_x = (selObjLtX+selObjRbX)>>1;
         pivot_y = (selObjLtY+selObjRbY)>>1;
      }
   }
   *spec = '\0';
   Dialog(TgLoadString(STID_ENTER_AN_ANGLE_IN_DEGREES), NULL, spec);
   UtilTrimBlanks(spec);
   if (*spec == '\0') return;
   if (sscanf(spec, "%lf", &angle_spec) != 1) {
      sprintf(gszMsgBox, TgLoadString(STID_INVALID_SPEC_NUM_EXPECTED), spec);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return;
   }
   if (fabs(angle_spec) < (1.0e-5)) return;

   ltx = selLtX; lty = selLtY; rbx = selRbX; rby = selRbY;
   HighLightReverse();
   PrepareToRecord(CMD_REPLACE, topSel, botSel, numObjSelected);
   if (FinishPreciseRotate(angle_spec, pivot_x, pivot_y)) {
      RecordCmd(CMD_REPLACE, NULL, topSel, botSel, numObjSelected);
      UpdSelBBox();
      RedrawAreas(botObj, ltx-GRID_ABS_SIZE(1), lty-GRID_ABS_SIZE(1),
            rbx+GRID_ABS_SIZE(1), rby+GRID_ABS_SIZE(1),
            selLtX-GRID_ABS_SIZE(1), selLtY-GRID_ABS_SIZE(1),
            selRbX+GRID_ABS_SIZE(1), selRbY+GRID_ABS_SIZE(1));
      SetFileModified(TRUE);
      justDupped = FALSE;
   } else {
      AbortPrepareCmd(CMD_REPLACE);
   }
   HighLightForward();
}

void RotateAllSelObj(angle_spec)
   double angle_spec;
{
   int arc_count=0, pivot_x=0, pivot_y=0, ltx, lty, rbx, rby;
   struct SelRec *sel_ptr;
   struct ObjRec *arc_obj=NULL;

   if (curChoice != NOTHING || topSel == NULL) {
      MsgBox(TgLoadCachedString(CSTID_NO_OBJ_SELECTED), TOOL_NAME, INFO_MB);
      return;
   }
   for (sel_ptr=topSel; sel_ptr != NULL; sel_ptr=sel_ptr->next) {
      if (sel_ptr->obj->type == OBJ_ARC) {
         arc_obj = sel_ptr->obj;
         arc_count++;
      }
   }
   if (arc_count == 1) {
      if (arc_obj->ctm == NULL) {
         pivot_x = arc_obj->detail.a->xc;
         pivot_y = arc_obj->detail.a->yc;
      } else {
         struct ArcRec *arc_ptr=arc_obj->detail.a;
         int x, y;

         TransformPointThroughCTM(arc_ptr->xc-arc_obj->x,
               arc_ptr->yc-arc_obj->y, arc_obj->ctm, &x, &y);
         pivot_x = x + arc_obj->x;
         pivot_y = y + arc_obj->y;
      }
   } else {
      pivot_x = (selObjLtX+selObjRbX)>>1;
      pivot_y = (selObjLtY+selObjRbY)>>1;
   }
   if (fabs(angle_spec) < (1.0e-5)) return;

   ltx = selLtX; lty = selLtY; rbx = selRbX; rby = selRbY;
   HighLightReverse();
   PrepareToRecord(CMD_REPLACE, topSel, botSel, numObjSelected);
   FinishPreciseRotate(angle_spec, pivot_x, pivot_y);
   RecordCmd(CMD_REPLACE, NULL, topSel, botSel, numObjSelected);
   UpdSelBBox();
   RedrawAreas(botObj, ltx-GRID_ABS_SIZE(1), lty-GRID_ABS_SIZE(1),
         rbx+GRID_ABS_SIZE(1), rby+GRID_ABS_SIZE(1),
         selLtX-GRID_ABS_SIZE(1), selLtY-GRID_ABS_SIZE(1),
         selRbX+GRID_ABS_SIZE(1), selRbY+GRID_ABS_SIZE(1));
   HighLightForward();
   SetFileModified(TRUE);
   justDupped = FALSE;
}

void NoTransform()
{
   struct SelRec *sel_ptr;
   int changed=FALSE, grouped=FALSE;
   int ltx=selLtX, lty=selLtY, rbx=selRbX, rby=selRbY;

   if (topSel == NULL) {
      MsgBox(TgLoadCachedString(CSTID_NO_OBJ_SELECTED), TOOL_NAME, INFO_MB);
      return;
   }
   HighLightReverse();
   StartCompositeCmd();
   for (sel_ptr=topSel; sel_ptr != NULL; sel_ptr=sel_ptr->next) {
      struct ObjRec *obj_ptr=sel_ptr->obj;

      if (obj_ptr->ctm != NULL) {
         int cx=0, cy=0;

         switch (obj_ptr->type) {
         case OBJ_GROUP:
         case OBJ_SYM:
         case OBJ_ICON:
         case OBJ_PIN:
            grouped = TRUE;
            break;

         default:
            cx = ((obj_ptr->obbox.ltx+obj_ptr->obbox.rbx)>>1);
            cy = ((obj_ptr->obbox.lty+obj_ptr->obbox.rby)>>1);

            changed = TRUE;
            PrepareToReplaceAnObj(obj_ptr);
            free(obj_ptr->ctm);
            obj_ptr->ctm = NULL;
            memcpy(&obj_ptr->obbox, &obj_ptr->orig_obbox, sizeof(struct BBRec));
            if (obj_ptr->type == OBJ_TEXT) {
               memcpy(&obj_ptr->bbox, &obj_ptr->detail.t->orig_bbox,
                     sizeof(struct BBRec));
            }
            AdjObjSplineVs(obj_ptr);
            AdjObjCache(obj_ptr);
            AdjObjBBox(obj_ptr);
            MoveObj(obj_ptr,
                  cx-((obj_ptr->obbox.ltx+obj_ptr->obbox.rbx)>>1),
                  cy-((obj_ptr->obbox.lty+obj_ptr->obbox.rby)>>1));
            RecordReplaceAnObj(obj_ptr);
            break;
         }
      } else {
         switch (obj_ptr->type) {
         case OBJ_GROUP:
         case OBJ_SYM:
         case OBJ_ICON:
         case OBJ_PIN:
            grouped = TRUE;
            break;

         default: break;
         }
      }
   }
   EndCompositeCmd();
   if (changed) {
      UpdSelBBox();
      RedrawAreas(botObj, ltx-GRID_ABS_SIZE(1), lty-GRID_ABS_SIZE(1),
            rbx+GRID_ABS_SIZE(1), rby+GRID_ABS_SIZE(1),
            selLtX-GRID_ABS_SIZE(1), selLtY-GRID_ABS_SIZE(1),
            selRbX+GRID_ABS_SIZE(1), selRbY+GRID_ABS_SIZE(1));
      HighLightForward();
      SetFileModified(TRUE);
      justDupped = FALSE;

      if (grouped) {
         Msg(TgLoadString(STID_SOME_GROUP_OBJ_NOT_MODIFIED));
      }
   } else {
      if (grouped) {
         MsgBox(TgLoadString(STID_CANNOT_REM_XFORM_FOR_GROUPED), TOOL_NAME,
               INFO_MB);
      }
      HighLightForward();
   }
}

void SetEditTextSize()
{
   char spec[MAXSTRING+1];
   int size=0;

   MakeQuiescent();
   *spec = '\0';
   sprintf(gszMsgBox, TgLoadString(STID_ENTER_EDIT_TEXT_SIZE), editTextSize);
   *spec = '\0';
   Dialog(gszMsgBox, NULL, spec);
   UtilTrimBlanks(spec);
   if (*spec == '\0') return;
   size = atoi(spec);
   if (size != 0 && (size < 4 || size > 34)) {
      sprintf(gszMsgBox, TgLoadString(STID_EDIT_TEXT_SIZE_OUT_OF_RANGE), spec);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
   } else {
      SetEditTextSizeValue(size);

      if (editTextSize == 0) {
         Msg(TgLoadString(STID_ACTUAL_EDIT_TEXT_SIZE));
      } else {
         sprintf(gszMsgBox, TgLoadString(STID_USE_SPECIFIED_EDIT_TEXT_SIZE),
               editTextSize);
         Msg(gszMsgBox);
      }
   }
}

static
void UpdateOuterInnerSelForFind(obj_ptr)
   struct ObjRec *obj_ptr;
   /* outerSel is at the top of the chain and innerSel is at the bottom */
{
   AddObjIntoSel(obj_ptr, NULL, outerSelForFind, &outerSelForFind,
         &innerSelForFind);
}

static
struct ObjRec *DoFind(obj_ptr)
   struct ObjRec *obj_ptr;
{
   int found_starting_point=TRUE;
   MiniLinesInfo *minilines=NULL;
   struct ObjRec *ptr=NULL;
   struct ObjRec *return_obj=NULL;

   if (obj_ptr->type != OBJ_TEXT) {
      struct AttrRec *attr_ptr;

      for (attr_ptr=obj_ptr->fattr; attr_ptr != NULL; attr_ptr=attr_ptr->next) {
         if (attr_ptr->shown) {
            return_obj = DoFind(attr_ptr->obj);
            if (return_obj != NULL) {
               return return_obj;
            }
         }
      }
   }
   switch (obj_ptr->type) {
   case OBJ_SYM:
   case OBJ_GROUP:
   case OBJ_ICON:
      for (ptr=obj_ptr->detail.r->last; ptr != NULL; ptr=ptr->prev) {
         ptr->tmp_parent = obj_ptr;
         return_obj = DoFind(ptr);
         if (return_obj != NULL) {
            UpdateOuterInnerSelForFind(obj_ptr);
            return return_obj;
         }
      }
      break;
   case OBJ_PIN:
      ptr = GetPinObj(obj_ptr);
      ptr->tmp_parent = obj_ptr;
      return_obj = DoFind(ptr);
      if (return_obj != NULL) {
         return return_obj;
      }
      break;
   case OBJ_TEXT:
      found_starting_point = TRUE;
      minilines = (&obj_ptr->detail.t->minilines);

      SaveCursorPositionInCurText();

      curStrBlock = minilines->first->first_block;
      textCurIndex = 0;
      ResetOnCursorKey(FALSE);
      SetTextHighlight();
      UpdatePinnedMenu(MENU_EDIT);

      if (FindStringInMiniLines(minilines, &found_starting_point,
            gpszSearch, gnSearchLen, gnSearchCaseSensitive,
            &gpFoundStartStrBlock, &gnFoundStartCharIndex,
            &gpFoundEndStrBlock, &gnFoundEndCharIndex)) {
         return obj_ptr;
      }
      RestoreCursorPositionInCurText();
      break;
   default: break;
   }
   return NULL;
}

static int gnFoundStartingPoint=FALSE;

static
struct ObjRec *DoFindAlready(obj_ptr, bottom_half, pn_give_up)
   struct ObjRec *obj_ptr;
   int bottom_half; /* TRUE if searching from curTextObj to topObj */
   int *pn_give_up;
{
   struct ObjRec *ptr=NULL;
   struct ObjRec *return_obj=NULL;

   if (obj_ptr->type != OBJ_TEXT) {
      struct AttrRec *attr_ptr;

      for (attr_ptr=obj_ptr->fattr; attr_ptr != NULL; attr_ptr=attr_ptr->next) {
         if (attr_ptr->shown) {
            return_obj = DoFindAlready(attr_ptr->obj, bottom_half, pn_give_up);
            if (return_obj != NULL) {
               return return_obj;
            }
            if (pn_give_up != NULL && *pn_give_up) {
               return NULL;
            }
         }
      }
   }
   switch (obj_ptr->type) {
   case OBJ_SYM:
   case OBJ_GROUP:
   case OBJ_ICON:
      for (ptr=obj_ptr->detail.r->last; ptr != NULL; ptr=ptr->prev) {
         ptr->tmp_parent = obj_ptr;
         return_obj = DoFindAlready(ptr, bottom_half, pn_give_up);
         if (return_obj != NULL) {
            UpdateOuterInnerSelForFind(obj_ptr);
            return return_obj;
         }
         if (pn_give_up != NULL && *pn_give_up) {
            return NULL;
         }
      }
      break;
   case OBJ_PIN:
      ptr = GetPinObj(obj_ptr);
      ptr->tmp_parent = obj_ptr;
      return_obj = DoFindAlready(ptr, bottom_half, pn_give_up);
      if (return_obj != NULL) {
         return return_obj;
      }
      if (pn_give_up != NULL && *pn_give_up) {
         return NULL;
      }
      break;
   case OBJ_TEXT:
      if ((bottom_half && gnFoundStartingPoint) ||
            (!bottom_half && obj_ptr != curTextObj)) {
         int found_starting_point=TRUE;
         MiniLinesInfo *minilines=(&obj_ptr->detail.t->minilines);

         SaveCursorPositionInCurText();

         curStrBlock = minilines->first->first_block;
         textCurIndex = 0;
         ResetOnCursorKey(FALSE);
         SetTextHighlight();
         UpdatePinnedMenu(MENU_EDIT);

         if (FindStringInMiniLines(minilines, &found_starting_point,
               gpszSearch, gnSearchLen, gnSearchCaseSensitive,
               &gpFoundStartStrBlock, &gnFoundStartCharIndex,
               &gpFoundEndStrBlock, &gnFoundEndCharIndex)) {
            return obj_ptr;
         }
         RestoreCursorPositionInCurText();
      } else if (bottom_half) {
         if (obj_ptr == curTextObj) {
            gnFoundStartingPoint = TRUE;
         }
      } else {
         if (obj_ptr == curTextObj) {
            if (pn_give_up != NULL) *pn_give_up = TRUE;
         }
      }
      break;
   default: break;
   }
   return NULL;
}

static
void Find()
{
   int wrapped=FALSE;
   struct ObjRec *obj_ptr=NULL;
   struct ObjRec *found_obj=NULL;

   CleanOuterInnerSelForFind();
   UpdatePinnedMenu(MENU_EDIT);

   if (gpszSearch == NULL) return;
   gnSearchLen = strlen(gpszSearch);

   gnFoundStartCharIndex = 0;
   gpFoundStartStrBlock = NULL;

   SetWatchCursor(drawWindow);
   SetWatchCursor(mainWindow);

   if (curChoice == DRAWTEXT) {
      if (curTextObj != NULL && FindTextInCurTextObj(gpszSearch, gnSearchLen,
            gnSearchCaseSensitive, &gpFoundStartStrBlock,
            &gnFoundStartCharIndex, &gpFoundEndStrBlock,
            &gnFoundEndCharIndex)) {
         found_obj = curTextObj;
         CopyOuterSelToOuterSelForFind();
      }
      if (found_obj == NULL) {
         gnFoundStartingPoint = FALSE;
         for (obj_ptr=botObj; obj_ptr != NULL; obj_ptr=obj_ptr->prev) {
            obj_ptr->tmp_parent = NULL;
            if (!ObjInVisibleLayer(obj_ptr)) {
               continue;
            }
            found_obj = DoFindAlready(obj_ptr, TRUE, NULL);
            if (found_obj != NULL) {
               break;
            }
         }
         if (found_obj == NULL && gnFoundStartingPoint) {
            wrapped = TRUE;
            for (obj_ptr=botObj; obj_ptr != NULL; obj_ptr=obj_ptr->prev) {
               int give_up=FALSE;

               obj_ptr->tmp_parent = NULL;
               if (!ObjInVisibleLayer(obj_ptr)) {
                  continue;
               }
               found_obj = DoFindAlready(obj_ptr, FALSE, &give_up);
               if (found_obj != NULL) {
                  break;
               }
               if (give_up) {
                  break;
               }
            }
         }
      }
   }
   if (found_obj == NULL) {
      if (curChoice == DRAWTEXT) {
         wrapped = TRUE;
      }
      for (obj_ptr=botObj; obj_ptr != NULL; obj_ptr=obj_ptr->prev) {
         obj_ptr->tmp_parent = NULL;
         if (!ObjInVisibleLayer(obj_ptr)) {
            continue;
         }
         found_obj = DoFind(obj_ptr);
         if (found_obj != NULL) {
            break;
         }
      }
      if (found_obj != NULL) {
         if (curChoice != DRAWTEXT) {
            SetCurChoice(DRAWTEXT);
         }
      }
   }
   SetDefaultCursor(mainWindow);
   ShowCursor();

   if (found_obj == NULL) {
      sprintf(gszMsgBox, TgLoadString(STID_CANNOT_FIND_NAMED_STRING),
            gpszSearch);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
   } else {
      HighLightText(found_obj, gpFoundStartStrBlock, gnFoundStartCharIndex,
            gpFoundEndStrBlock, gnFoundEndCharIndex);
      if (wrapped) {
         Msg(TgLoadString(STID_FIND_CMD_WRAPPED));
      }
   }
}

void FindCaseSensitive()
{
   char spec[MAXSTRING];

   *spec = '\0';
   Dialog(TgLoadString(STID_ENTER_CASE_STR_TO_FIND), NULL, spec);
   if (*spec == '\0') {
      return;
   }
   if (gpszSearch != NULL) free(gpszSearch);
   if ((gpszSearch=UtilStrDup(spec)) == NULL) FailAllocMessage();

   gnSearchCaseSensitive = TRUE;
   if (curChoice != DRAWTEXT) {
      MakeQuiescent();
   }
   Find();
}

void FindNoCase()
{
   char spec[MAXSTRING];

   *spec = '\0';
   Dialog(TgLoadString(STID_ENTER_NOCASE_STR_TO_FIND), NULL, spec);
   if (*spec == '\0') {
      return;
   }
   if (gpszSearch != NULL) free(gpszSearch);
   if ((gpszSearch=UtilStrDup(spec)) == NULL) FailAllocMessage();

   gnSearchCaseSensitive = FALSE;
   if (curChoice != DRAWTEXT) {
      MakeQuiescent();
   }
   Find();
}

void FindAgain()
{
   if (gpszSearch == NULL) {
      MsgBox(TgLoadString(STID_NO_PREVIOUS_FIND), TOOL_NAME, INFO_MB);
   } else {
      Find();
   }
}

/* ----------------------- EditMenu ----------------------- */

int RefreshEditMenu(menu)
   TgMenu *menu;
{
   int ok=TRUE;
   StrSegInfo ssi;

   memset(&ssi, 0, sizeof(StrSegInfo));

   /* CopyPlainTextAsObject */
   ok &= TgEnableMenuItemById(menu, CMDID_COPYPLAINTEXTASOBJECT,
         (curChoice == DRAWTEXT && textHighlight));

   /* CopyUTF8String */
   ok &= TgEnableMenuItemById(menu, CMDID_COPYUTF8,
         (curChoice == DRAWTEXT && textHighlight &&
         CanCopyHighLightedTextAsUTF8Strings(NULL)));
   /* PasteUTF8String */
   ok &= TgEnableMenuItemById(menu, CMDID_PASTEUTF8,
         (curChoice == DRAWTEXT && textCursorShown &&
         CanPasteUTF8StringIntoText(&ssi)));

   /* ImageProc submenu */
   ok &= TgEnableMenuItemById(menu, MENU_IMAGEPROC, CanPerformImageProc());

   /* FindAgain */
   ok &= TgEnableMenuItemById(menu, CMDID_FINDAGAIN, (gpszSearch != NULL));

   return ok;
}

TgMenu *CreateEditMenu(parent_menu, x, y, menu_info, status_str_xlated)
   TgMenu *parent_menu;
   int x, y;
   TgMenuInfo *menu_info;
   int status_str_xlated; /* ignored, always 0 */
{
   TgMenu *menu=TgCreateMenuFromMenuInfo(parent_menu, x, y, menu_info, FALSE);

   if (menu != NULL) {
      if (!RefreshEditMenu(menu)) {
         return TgDestroyMenu(menu, TRUE);
      }
      menu->refresh_proc = ((RefreshMenuFunc*)RefreshEditMenu);
   }
   return menu;
}

int EditMenu(X, Y, TrackMenubar)
   int X, Y, TrackMenubar;
{
   int rc=INVALID;
   TgMenu *menu=(editMenuInfo.create_proc)(NULL, X, Y, &editMenuInfo, INVALID);

   activeMenu = MENU_EDIT;
   if (menu != NULL) {
      menu->track_menubar = TrackMenubar;

      rc = TgMenuLoop(menu);
      TgDestroyMenu(menu, TRUE);
   }
   return rc;
}

/* ======================= ArrangeMenu Related ======================= */

void FrontProc()
{
   if (topSel != NULL) {
      HighLightReverse();
      MoveSelToTop();
      RedrawAnArea(botObj, selLtX-GRID_ABS_SIZE(1), selLtY-GRID_ABS_SIZE(1),
            selRbX+GRID_ABS_SIZE(1), selRbY+GRID_ABS_SIZE(1));
      HighLightForward();
      SetFileModified(TRUE);
   }
}

void BackProc()
{
   if (topSel != NULL) {
      HighLightReverse();
      MoveSelToBot();
      RedrawAnArea(botObj, selLtX-GRID_ABS_SIZE(1), selLtY-GRID_ABS_SIZE(1),
            selRbX+GRID_ABS_SIZE(1), selRbY+GRID_ABS_SIZE(1));
      HighLightForward();
      SetFileModified(TRUE);
   }
}

void AlignObjsTop()
{
   register int saved_h_align = horiAlign, saved_v_align = vertAlign;

   horiAlign = ALIGN_N; vertAlign = ALIGN_T;
   AlignSelObjs();
   horiAlign = saved_h_align; vertAlign = saved_v_align;
}

void AlignObjsMiddle()
{
   register int saved_h_align = horiAlign, saved_v_align = vertAlign;

   horiAlign = ALIGN_N; vertAlign = ALIGN_M;
   AlignSelObjs();
   horiAlign = saved_h_align; vertAlign = saved_v_align;
}

void AlignObjsBottom()
{
   register int saved_h_align = horiAlign, saved_v_align = vertAlign;

   horiAlign = ALIGN_N; vertAlign = ALIGN_B;
   AlignSelObjs();
   horiAlign = saved_h_align; vertAlign = saved_v_align;
}

void AlignObjsLeft()
{
   register int saved_h_align = horiAlign, saved_v_align = vertAlign;

   horiAlign = ALIGN_L; vertAlign = ALIGN_N;
   AlignSelObjs();
   horiAlign = saved_h_align; vertAlign = saved_v_align;
}

void AlignObjsCenter()
{
   register int saved_h_align = horiAlign, saved_v_align = vertAlign;

   horiAlign = ALIGN_C; vertAlign = ALIGN_N;
   AlignSelObjs();
   horiAlign = saved_h_align; vertAlign = saved_v_align;
}

void AlignObjsRight()
{
   register int saved_h_align = horiAlign, saved_v_align = vertAlign;

   horiAlign = ALIGN_R; vertAlign = ALIGN_N;
   AlignSelObjs();
   horiAlign = saved_h_align; vertAlign = saved_v_align;
}

static
void Abut(Dir)
   int Dir;
{
   struct ObjRec *obj_ptr=NULL;
   struct SelRec *sel_ptr=NULL;
   struct SelRec *top_sort=NULL, *bot_sort=NULL, *sort_ptr=NULL;
   struct SelRec *new_sort_ptr=NULL, *tmp_sel_ptr=NULL, *next_sort=NULL;
   struct ObjRec *sorted_obj=NULL, *locked_obj=NULL;
   struct SubCmdRec *sub_cmd=NULL;
   int sel_ltx=0, sel_lty=0, sel_rbx=0, sel_rby=0, rbx=0, rby=0;
   int found=FALSE, delta=0, dx=0, dy=0;

   if (topSel == NULL) {
      MsgBox(TgLoadCachedString(CSTID_NO_OBJ_SELECTED), TOOL_NAME, INFO_MB);
      return;
   } else if (curChoice == VERTEXMODE) {
      MsgBox(TgLoadString(STID_CANNOT_ABUT_IN_VERTEX_MODE), TOOL_NAME, INFO_MB);
      return;
   } else if (numObjLocked > 1) {
      MsgBox(TgLoadString(STID_CANNOT_ABUT_LOCKED), TOOL_NAME, INFO_MB);
      return;
   }
   HighLightReverse();
   StartCompositeCmd();
   sel_ltx = selLtX; sel_lty = selLtY;
   sel_rbx = selRbX; sel_rby = selRbY;

   top_sort = (struct SelRec *)malloc(sizeof(struct SelRec));
   if (top_sort == NULL) FailAllocMessage();
   top_sort->next = top_sort->prev = NULL;

   top_sort->obj = sorted_obj = botSel->obj;
   if (botSel->obj->locked) locked_obj = botSel->obj;
   for (sel_ptr=botSel->prev; sel_ptr != NULL; sel_ptr=sel_ptr->prev) {
      obj_ptr = sel_ptr->obj;
      if (obj_ptr->locked) locked_obj = obj_ptr;
      switch (Dir) {
      case ABUT_HORIZONTAL:
         if (obj_ptr->obbox.ltx < sorted_obj->obbox.ltx ||
               (obj_ptr->obbox.ltx == sorted_obj->obbox.ltx &&
               obj_ptr->obbox.lty < sorted_obj->obbox.lty)) {
            top_sort->obj = sorted_obj = sel_ptr->obj;
         }
         break;
      case ABUT_VERTICAL:
         if (obj_ptr->obbox.lty < sorted_obj->obbox.lty ||
               (obj_ptr->obbox.lty == sorted_obj->obbox.lty &&
               obj_ptr->obbox.ltx < sorted_obj->obbox.ltx)) {
            top_sort->obj = sorted_obj = sel_ptr->obj;
         }
         break;
      }
   }
   bot_sort = top_sort;

   for (sel_ptr=botSel; sel_ptr != NULL; sel_ptr=sel_ptr->prev) {
      if (sel_ptr->obj == top_sort->obj) continue;

      obj_ptr = sel_ptr->obj;
      new_sort_ptr = (struct SelRec *)malloc(sizeof(struct SelRec));
      if (new_sort_ptr == NULL) FailAllocMessage();
      new_sort_ptr->obj = obj_ptr;
      found = FALSE;
      for (sort_ptr=top_sort->next; sort_ptr!=NULL; sort_ptr=sort_ptr->next) {
         switch (Dir) {
         case ABUT_HORIZONTAL:
            if (sort_ptr->obj->obbox.ltx > obj_ptr->obbox.ltx ||
                  (sort_ptr->obj->obbox.ltx == obj_ptr->obbox.ltx &&
                  sort_ptr->obj->obbox.lty > obj_ptr->obbox.lty)) {
               found = TRUE;
            }
            break;
         case ABUT_VERTICAL:
            if (sort_ptr->obj->obbox.lty > obj_ptr->obbox.lty ||
                  (sort_ptr->obj->obbox.lty == obj_ptr->obbox.lty &&
                  sort_ptr->obj->obbox.ltx > obj_ptr->obbox.ltx)) {
               found = TRUE;
            }
            break;
         }
         if (found) break;
      }
      new_sort_ptr->next = sort_ptr;
      if (sort_ptr == NULL) {
         new_sort_ptr->prev = bot_sort;
         bot_sort->next = new_sort_ptr;
         bot_sort = new_sort_ptr;
      } else {
         new_sort_ptr->prev = sort_ptr->prev;
         sort_ptr->prev->next = new_sort_ptr;
         sort_ptr->prev = new_sort_ptr;
      }
   }

   tmp_sel_ptr = (struct SelRec *)malloc(sizeof(struct SelRec));
   if (tmp_sel_ptr == NULL) FailAllocMessage();
   tmp_sel_ptr->next = tmp_sel_ptr->prev = NULL;

   if (locked_obj != NULL) {
      switch (Dir) {
      case ABUT_HORIZONTAL:
         rbx = top_sort->obj->obbox.rbx;
         for (sort_ptr=top_sort; sort_ptr->obj!=locked_obj &&
               sort_ptr->next!=NULL; sort_ptr=next_sort) {
            next_sort = sort_ptr->next;
            delta = rbx-next_sort->obj->obbox.ltx;
            dx = (-delta);
            rbx = next_sort->obj->obbox.rbx+delta;
         }
         break;
      case ABUT_VERTICAL:
         rby = top_sort->obj->obbox.rby;
         for (sort_ptr=top_sort; sort_ptr->obj!=locked_obj &&
               sort_ptr->next!=NULL; sort_ptr=next_sort) {
            next_sort = sort_ptr->next;
            delta = rby-next_sort->obj->obbox.lty;
            dy = (-delta);
            rby = next_sort->obj->obbox.rby+delta;
         }
         break;
      }
   }
   sub_cmd = (struct SubCmdRec *)malloc(sizeof(struct SubCmdRec));
   if (sub_cmd == NULL) FailAllocMessage();
   memset(sub_cmd, 0, sizeof(struct SubCmdRec));

   rbx = top_sort->obj->obbox.rbx;
   rby = top_sort->obj->obbox.rby;
   found = (locked_obj == NULL);
   if (!found && locked_obj != top_sort->obj) {
      tmp_sel_ptr->obj = top_sort->obj;
      switch (Dir) {
      case ABUT_HORIZONTAL:
         sub_cmd->detail.move.dx = dx;
         sub_cmd->detail.move.dy = 0;
         PrepareToRecord(CMD_MOVE, tmp_sel_ptr, tmp_sel_ptr, 1);
         RecordCmd(CMD_MOVE, sub_cmd, NULL, NULL, 0);
         MoveObj(top_sort->obj, dx, 0);
         break;
      case ABUT_VERTICAL:
         sub_cmd->detail.move.dx = 0;
         sub_cmd->detail.move.dy = dy;
         PrepareToRecord(CMD_MOVE, tmp_sel_ptr, tmp_sel_ptr, 1);
         RecordCmd(CMD_MOVE, sub_cmd, NULL, NULL, 0);
         MoveObj(top_sort->obj, 0, dy);
         break;
      }
   }
   for (sort_ptr=top_sort; sort_ptr->next!=NULL; sort_ptr=next_sort) {
      next_sort = sort_ptr->next;
      tmp_sel_ptr->obj = next_sort->obj;
      switch (Dir) {
      case ABUT_HORIZONTAL:
         delta = rbx-next_sort->obj->obbox.ltx;
         rbx = next_sort->obj->obbox.rbx+delta;
         if (!found) delta += dx;
         sub_cmd->detail.move.dx = delta;
         sub_cmd->detail.move.dy = 0;
         PrepareToRecord(CMD_MOVE, tmp_sel_ptr, tmp_sel_ptr, 1);
         RecordCmd(CMD_MOVE, sub_cmd, NULL, NULL, 0);
         MoveObj(next_sort->obj, delta, 0);
         break;
      case ABUT_VERTICAL:
         delta = rby-next_sort->obj->obbox.lty;
         rby = next_sort->obj->obbox.rby+delta;
         if (!found) delta += dy;
         sub_cmd->detail.move.dx = 0;
         sub_cmd->detail.move.dy = delta;
         PrepareToRecord(CMD_MOVE, tmp_sel_ptr, tmp_sel_ptr, 1);
         RecordCmd(CMD_MOVE, sub_cmd, NULL, NULL, 0);
         MoveObj(next_sort->obj, 0, delta);
         break;
      }
      free(sort_ptr);
   }
   EndCompositeCmd();
   free(sort_ptr);
   free(sub_cmd);
   free(tmp_sel_ptr);

   UpdSelBBox();
   RedrawAreas(botObj, selLtX-GRID_ABS_SIZE(1), selLtY-GRID_ABS_SIZE(1),
         selRbX+GRID_ABS_SIZE(1), selRbY+GRID_ABS_SIZE(1),
         sel_ltx-GRID_ABS_SIZE(1), sel_lty-GRID_ABS_SIZE(1),
         sel_rbx+GRID_ABS_SIZE(1), sel_rby+GRID_ABS_SIZE(1));
   HighLightForward();
   justDupped = FALSE;
   switch (Dir) {
   case ABUT_HORIZONTAL: Msg(TgLoadString(STID_ABUTTED_HORI)); break;
   case ABUT_VERTICAL: Msg(TgLoadString(STID_ABUTTED_VERT)); break;
   }
   SetFileModified(TRUE);
}

void AbutHorizontal()
{
   Abut(ABUT_HORIZONTAL);
}

void AbutVertical()
{
   Abut(ABUT_VERTICAL);
}

void RefreshArrangeMenu(menu)
   TgMenu *menu;
{
}

int ArrangeMenu(X, Y, TrackMenubar)
   int X, Y, TrackMenubar;
{
   int rc=INVALID;
   TgMenu *menu=(arrangeMenuInfo.create_proc)(NULL, X, Y, &arrangeMenuInfo,
         FALSE);

   activeMenu = MENU_ARRANGE;
   if (menu != NULL) {
      menu->track_menubar = TrackMenubar;

      rc = TgMenuLoop(menu);
      TgDestroyMenu(menu, TRUE);
   }
   return rc;
}

int RefreshPropertiesMenu(menu)
   TgMenu *menu;
{
   int ok=TRUE;

   /* UseAltEditTextBgColor */
   ok &= TgSetMenuItemCheckById(menu, CMDID_TOGGLEALTEDITTEXTBGCOLOR,
         useAltEditTextBgColor);
   /* AddColor */
   ok &= TgEnableMenuItemById(menu, CMDID_ADDCOLOR, colorDisplay);

   return ok;
}

TgMenu *CreatePropertiesMenu(parent_menu, x, y, menu_info, status_str_xlated)
   TgMenu *parent_menu;
   int x, y;
   TgMenuInfo *menu_info;
   int status_str_xlated; /* ignored, always 0 */
{
   TgMenu *menu=TgCreateMenuFromMenuInfo(parent_menu, x, y, menu_info, FALSE);

   if (menu != NULL) {
      if (!RefreshPropertiesMenu(menu)) {
         return TgDestroyMenu(menu, TRUE);
      }
      menu->refresh_proc = ((RefreshMenuFunc*)RefreshPropertiesMenu);
   }
   return menu;
}

int PropertiesMenu(X, Y, TrackMenubar)
   int X, Y, TrackMenubar;
{
   int rc=INVALID;
   TgMenu *menu=(propertiesMenuInfo.create_proc)(NULL, X, Y,
         &propertiesMenuInfo, INVALID);

   activeMenu = MENU_PROPERTIES;
   if (menu != NULL) {
      menu->track_menubar = TrackMenubar;

      rc = TgMenuLoop(menu);
      TgDestroyMenu(menu, TRUE);
   }
   return rc;
}

void UpdateSymbols()
{
   int dx=0, dy=0, changed=FALSE;
   int sel_ltx, sel_lty, sel_rbx, sel_rby, file_type=INVALID;
   char path_name[MAXPATHLENGTH], sym_name[MAXPATHLENGTH];
   struct ObjRec *obj_ptr, *new_obj_ptr;
   struct SelRec *sel_ptr;
   struct GroupRec *icon_ptr;

   if (topSel == NULL) {
      MsgBox(TgLoadCachedString(CSTID_NO_OBJ_SELECTED), TOOL_NAME, INFO_MB);
      return;
   }
   tmpTopObj = tmpBotObj = NULL;
   tmpTopSel = tmpBotSel = NULL;

   sel_ltx = selLtX; sel_lty = selLtY;
   sel_rbx = selRbX; sel_rby = selRbY;

   HighLightReverse();

   StartCompositeCmd();
   for (sel_ptr=botSel; sel_ptr != NULL; sel_ptr=sel_ptr->prev) {
      obj_ptr = sel_ptr->obj;
      if (obj_ptr->type != OBJ_ICON && obj_ptr->type != OBJ_PIN) {
         continue;
      }
      icon_ptr = obj_ptr->detail.r;
      strcpy(sym_name, icon_ptr->s);
      if (obj_ptr->type == OBJ_ICON) {
         file_type = SYM_FILE_TYPE;
      } else {
         file_type = PIN_FILE_TYPE;
      }
      if (GetSymbolPath(icon_ptr->s, obj_ptr->type==OBJ_PIN, path_name)) {
         if ((new_obj_ptr=GetObjRepresentation(path_name, sym_name,
               file_type)) != NULL) {
            PrepareToReplaceAnObj(obj_ptr);
            if (icon_ptr->flip != NO_FLIP) {
               if (icon_ptr->flip & HORI_EVEN) FlipIconHorizontal(new_obj_ptr);
               if (icon_ptr->flip & VERT_EVEN) FlipIconVertical(new_obj_ptr);
               if (icon_ptr->flip & (HORI_ODD | VERT_ODD)) {
                  RotateIconClockWise(new_obj_ptr);
                  if (icon_ptr->flip & HORI_ODD) {
                     FlipIconHorizontal(new_obj_ptr);
                  }
                  if (icon_ptr->flip & VERT_ODD) {
                     FlipIconVertical(new_obj_ptr);
                  }
                  RotateIconCounter(new_obj_ptr);
               }
            }
            switch (horiAlign) {
            case ALIGN_L:
               dx = obj_ptr->obbox.ltx - new_obj_ptr->obbox.ltx;
               break;
            case ALIGN_N:
            case ALIGN_S:
            case ALIGN_C:
               dx = (int)(((obj_ptr->obbox.ltx+obj_ptr->obbox.rbx) -
                     (new_obj_ptr->obbox.ltx+new_obj_ptr->obbox.rbx))/2);
               break;
            case ALIGN_R:
               dx = obj_ptr->obbox.rbx - new_obj_ptr->obbox.rbx;
               break;
            }
            switch (vertAlign) {
            case ALIGN_T:
               dy = obj_ptr->obbox.lty - new_obj_ptr->obbox.lty;
               break;
            case ALIGN_N:
            case ALIGN_S:
            case ALIGN_M:
               dy = (int)(((obj_ptr->obbox.lty+obj_ptr->obbox.rby) -
                     (new_obj_ptr->obbox.lty+new_obj_ptr->obbox.rby))/2);
               break;
            case ALIGN_B:
               dy = obj_ptr->obbox.rby - new_obj_ptr->obbox.rby;
               break;
            }
            MoveObj(new_obj_ptr, dx, dy);

            changed = TRUE;

            UnlinkObj(obj_ptr);
            CopyAndUpdateAttrs(new_obj_ptr, obj_ptr);
            ExpandCurSelBBoxes(new_obj_ptr);

            sel_ptr->obj = new_obj_ptr;
            AssignNewObjIds(new_obj_ptr);
            AddObj(NULL, topObj, new_obj_ptr);
            RecordReplaceAnObj(new_obj_ptr);
            FreeObj(obj_ptr);
         }
      }
   }
   EndCompositeCmd();

   if (changed) {
      UpdSelBBox();
      RedrawAreas(botObj, sel_ltx-GRID_ABS_SIZE(1), sel_lty-GRID_ABS_SIZE(1),
            sel_rbx+GRID_ABS_SIZE(1), sel_rby+GRID_ABS_SIZE(1),
            selLtX-GRID_ABS_SIZE(1), selLtY-GRID_ABS_SIZE(1),
            selRbX+GRID_ABS_SIZE(1), selRbY+GRID_ABS_SIZE(1));
      SetFileModified(TRUE);
      justDupped = FALSE;
      Msg(TgLoadString(STID_ICONS_BROUGHT_UP_TO_DATE));
   }
   HighLightForward();
}

void SizeToWidest()
{
   struct SelRec *sel_ptr=NULL;
   int abs_w=0;

   if (topSel == NULL || topSel == botSel) {
      MsgBox(TgLoadString(STID_SEL_AT_LEAST_TWO_OBJS), TOOL_NAME, INFO_MB);
      return;
   }
   for (sel_ptr=topSel; sel_ptr != NULL; sel_ptr=sel_ptr->next) {
      struct ObjRec *obj_ptr=sel_ptr->obj;
      int w=obj_ptr->obbox.rbx-obj_ptr->obbox.ltx;

      if (w > abs_w) abs_w = w;
   }
   if (abs_w <= 1) {
      sprintf(gszMsgBox, TgLoadString(STID_CANNOT_SIZE_OF_GIVEN_WIDTH), abs_w);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return;
   }
   SizeAllSelToGivenWidth(abs_w);
}

void SizeToNarrowest()
{
   struct SelRec *sel_ptr=NULL;
   int abs_w=0;

   if (topSel == NULL || topSel == botSel) {
      MsgBox(TgLoadString(STID_SEL_AT_LEAST_TWO_OBJS), TOOL_NAME, INFO_MB);
      return;
   }
   abs_w = topSel->obj->obbox.rbx-topSel->obj->obbox.ltx;
   for (sel_ptr=topSel; sel_ptr != NULL; sel_ptr=sel_ptr->next) {
      struct ObjRec *obj_ptr=sel_ptr->obj;
      int w=obj_ptr->obbox.rbx-obj_ptr->obbox.ltx;

      if (w < abs_w) abs_w = w;
   }
   if (abs_w <= 1) {
      sprintf(gszMsgBox, TgLoadString(STID_CANNOT_SIZE_OF_GIVEN_WIDTH), abs_w);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return;
   }
   SizeAllSelToGivenWidth(abs_w);
}

void SizeToTallest()
{
   struct SelRec *sel_ptr=NULL;
   int abs_h=0;

   if (topSel == NULL || topSel == botSel) {
      MsgBox(TgLoadString(STID_SEL_AT_LEAST_TWO_OBJS), TOOL_NAME, INFO_MB);
      return;
   }
   for (sel_ptr=topSel; sel_ptr != NULL; sel_ptr=sel_ptr->next) {
      struct ObjRec *obj_ptr=sel_ptr->obj;
      int h=obj_ptr->obbox.rby-obj_ptr->obbox.lty;

      if (h > abs_h) abs_h = h;
   }
   if (abs_h <= 1) {
      sprintf(gszMsgBox, TgLoadString(STID_CANNOT_SIZE_OF_GIVEN_HEIGHT), abs_h);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return;
   }
   SizeAllSelToGivenHeight(abs_h);
}

void SizeToShortest()
{
   struct SelRec *sel_ptr=NULL;
   int abs_h=0;

   if (topSel == NULL || topSel == botSel) {
      MsgBox(TgLoadString(STID_SEL_AT_LEAST_TWO_OBJS), TOOL_NAME, INFO_MB);
      return;
   }
   abs_h = topSel->obj->obbox.rby-topSel->obj->obbox.lty;
   for (sel_ptr=topSel; sel_ptr != NULL; sel_ptr=sel_ptr->next) {
      struct ObjRec *obj_ptr=sel_ptr->obj;
      int h=obj_ptr->obbox.rby-obj_ptr->obbox.lty;

      if (h < abs_h) abs_h = h;
   }
   if (abs_h <= 1) {
      sprintf(gszMsgBox, TgLoadString(STID_CANNOT_SIZE_OF_GIVEN_HEIGHT), abs_h);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return;
   }
   SizeAllSelToGivenHeight(abs_h);
}

void SizeToGivenWidthHeight()
{
   int abs_w=0, abs_h=0;
   char spec[MAXSTRING];

   if (topSel == NULL) {
      MsgBox(TgLoadCachedString(CSTID_NO_OBJ_SELECTED), TOOL_NAME, INFO_MB);
      return;
   }
   *spec = '\0';
   if (Dialog(TgLoadString(STID_SPECIFY_WIDTH_HEIGHT), NULL, spec) == INVALID) {
      return;
   }
   UtilTrimBlanks(spec);
   if (*spec == '\0') return;

   if (!ParseWHSpec(spec, &abs_w, &abs_h)) {
      sprintf(gszMsgBox, TgLoadString(STID_INVALID_SPEC), spec);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return;
   }
   if (abs_w <= 1) {
      sprintf(gszMsgBox, TgLoadString(STID_CANNOT_SIZE_OF_GIVEN_WIDTH), abs_w);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return;
   }
   if (abs_h <= 1) {
      sprintf(gszMsgBox, TgLoadString(STID_CANNOT_SIZE_OF_GIVEN_HEIGHT), abs_h);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return;
   }
   SizeAllSelToGivenWidthHeight(abs_w, abs_h);
}

void SizeToGivenWidth()
{
   int abs_w=0;
   char spec[MAXSTRING];

   if (topSel == NULL) {
      MsgBox(TgLoadCachedString(CSTID_NO_OBJ_SELECTED), TOOL_NAME, INFO_MB);
      return;
   }
   *spec = '\0';
   if (Dialog(TgLoadString(STID_SPECIFY_WIDTH), NULL, spec) == INVALID) return;
   UtilTrimBlanks(spec);
   if (*spec == '\0') return;

   if (!GetDimension(spec, FALSE, &abs_w)) {
      sprintf(gszMsgBox, TgLoadString(STID_INVALID_SPEC), spec);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return;
   }
   if (abs_w <= 1) {
      sprintf(gszMsgBox, TgLoadString(STID_CANNOT_SIZE_OF_GIVEN_WIDTH), abs_w);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return;
   }
   SizeAllSelToGivenWidth(abs_w);
}

void SizeToGivenHeight()
{
   int abs_h=0;
   char spec[MAXSTRING];

   if (topSel == NULL) {
      MsgBox(TgLoadCachedString(CSTID_NO_OBJ_SELECTED), TOOL_NAME, INFO_MB);
      return;
   }
   *spec = '\0';
   if (Dialog(TgLoadString(STID_SPECIFY_HEIGHT), NULL, spec) == INVALID) return;
   UtilTrimBlanks(spec);
   if (*spec == '\0') return;

   if (!GetDimension(spec, FALSE, &abs_h)) {
      sprintf(gszMsgBox, TgLoadString(STID_INVALID_SPEC), spec);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return;
   }
   if (abs_h <= 1) {
      sprintf(gszMsgBox, TgLoadString(STID_CANNOT_SIZE_OF_GIVEN_HEIGHT), abs_h);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return;
   }
   SizeAllSelToGivenHeight(abs_h);
}

/* ----------------------- Generic Object Shadow ----------------------- */

void SetObjectShadowColor()
{
   char spec[MAXSTRING], buf[MAXSTRING];
   int new_alloc=0;
   
   *spec = '\0';
   sprintf(buf, TgLoadString(STID_ENTER_A_COLOR_FOR_OBJ_SHADOW),
         objShadowColorStr);
   UtilStrCpyN(spec, sizeof(spec), objShadowColorStr);
   if (Dialog(buf, TgLoadString(STID_PRESS_ENTER_FOR_DEF_COLOR), spec) ==
         INVALID) {
      return;
   }
   UtilTrimBlanks(spec);
   if (*spec == '\0') {
      strcpy(objShadowColorStr, "#c0c0c0");
      sprintf(gszMsgBox, TgLoadString(STID_OBJ_SHADOW_COLOR_SET_TO_NAMED),
            objShadowColorStr);
      Msg(gszMsgBox);
      return;
   }
   if (QuickFindColorIndex(NULL, spec, &new_alloc, FALSE) == INVALID) {
      sprintf(gszMsgBox, TgLoadString(STID_FAIL_ALLOC_NAMED_COLOR), spec);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return;
   }
   UtilStrCpyN(objShadowColorStr, sizeof(objShadowColorStr), spec);

   sprintf(gszMsgBox, TgLoadString(STID_OBJ_SHADOW_COLOR_SET_TO_NAMED),
         objShadowColorStr);
   Msg(gszMsgBox);
}

void SetObjectShadowOffsets()
{
   char spec[MAXSTRING];

   *spec = '\0';
   sprintf(gszMsgBox, TgLoadString(STID_ENTER_XY_OFFSET_OBJ_SHADOW),
         objShadowXOffset, objShadowYOffset);
   if (Dialog(gszMsgBox, NULL, spec) == INVALID) return;
   UtilTrimBlanks(spec);
   if (*spec == '\0') return;
   
   if (ParseXYSpec(spec, &objShadowXOffset, &objShadowYOffset)) {
      sprintf(gszMsgBox, TgLoadString(STID_OBJ_SHADOW_XY_OFFSETS_SET_TO),
            objShadowXOffset, objShadowYOffset);
      Msg(gszMsgBox);
   }
}

static
void DoAddObjectShadow()
{
   struct SelRec *sel_ptr=NULL;
   int new_alloc=0, color_index=QuickFindColorIndex(NULL, objShadowColorStr,
         &new_alloc, FALSE);

   for (sel_ptr=botSel; sel_ptr != NULL; sel_ptr=sel_ptr->prev) {
      struct ObjRec *obj_ptr=sel_ptr->obj, *dup_obj_ptr=NULL, *group_obj=NULL;
      struct GroupRec *group_ptr=NULL;
      struct BBRec obbox, bbox;
      int locked=obj_ptr->locked;

      dup_obj_ptr = DupObj(obj_ptr);
      AdjObjSplineVs(dup_obj_ptr);
      MoveObj(dup_obj_ptr, objShadowXOffset, objShadowYOffset);
      ChangeObjColor(dup_obj_ptr, color_index);
      UnionRect(&(obj_ptr->obbox), &(dup_obj_ptr->obbox), &obbox);
      UnionRect(&(obj_ptr->bbox), &(dup_obj_ptr->bbox), &bbox);

      group_obj = JustCreateGroupObj();
      group_ptr = group_obj->detail.r;

      group_obj->prev = obj_ptr->prev;
      group_obj->next = obj_ptr->next;
      if (obj_ptr == topObj) {
         curPage->top = topObj = group_obj;
      } else {
         obj_ptr->prev->next = group_obj;
      }
      if (obj_ptr == botObj) {
         curPage->bot = botObj = group_obj;
      } else {
         obj_ptr->next->prev = group_obj;
      }
      obj_ptr->prev = NULL;
      obj_ptr->next = dup_obj_ptr;
      dup_obj_ptr->prev = obj_ptr;
      dup_obj_ptr->next = NULL;
      group_ptr->first = obj_ptr;
      group_ptr->last = dup_obj_ptr;

      obj_ptr->x = min(obj_ptr->x, dup_obj_ptr->x);
      obj_ptr->y = min(obj_ptr->y, dup_obj_ptr->y);

      memcpy(&group_obj->obbox, &obbox, sizeof(struct BBRec));
      memcpy(&group_obj->bbox, &bbox, sizeof(struct BBRec));

      group_obj->locked = locked;

      sel_ptr->obj = group_obj;
   }
   UpdSelBBox();
}

void AddObjectShadow()
{
   if (curChoice != NOTHING || topSel == NULL) {
      MsgBox(TgLoadCachedString(CSTID_NO_OBJ_SELECTED), TOOL_NAME, INFO_MB);
      return;
   }
   HighLightReverse();
   PrepareToRecord(CMD_REPLACE, topSel, botSel, numObjSelected);
   DoAddObjectShadow();
   RecordCmd(CMD_REPLACE, NULL, topSel, botSel, numObjSelected);
   RedrawAnArea(botObj, selLtX-GRID_ABS_SIZE(1), selLtY-GRID_ABS_SIZE(1),
         selRbX+GRID_ABS_SIZE(1), selRbY+GRID_ABS_SIZE(1));
   HighLightForward();
   SetFileModified(TRUE);
   justDupped = FALSE;
}

static
void DoRemoveObjectShadow()
{
   struct SelRec *sel_ptr=NULL;

   for (sel_ptr=botSel; sel_ptr != NULL; sel_ptr=sel_ptr->prev) {
      struct ObjRec *obj_ptr=sel_ptr->obj;

      if (obj_ptr->type == OBJ_GROUP) {
         struct GroupRec *group_ptr=obj_ptr->detail.r;
         struct ObjRec *first_obj=group_ptr->first;

         if (first_obj != NULL) {
            struct ObjRec *second_obj=first_obj->next;

            if (second_obj->next == NULL) {
               int first_w=(first_obj->obbox.rbx-first_obj->obbox.ltx);
               int first_h=(first_obj->obbox.rby-first_obj->obbox.lty);
               int second_w=(second_obj->obbox.rbx-second_obj->obbox.ltx);
               int second_h=(second_obj->obbox.rby-second_obj->obbox.lty);

               if (first_w == second_w && first_h == second_h) {
                  group_ptr->first = group_ptr->last = NULL;
                  
                  first_obj->prev = obj_ptr->prev;
                  first_obj->next = obj_ptr->next;
                  if (obj_ptr == topObj) { 
                     curPage->top = topObj = first_obj;
                  } else {
                     obj_ptr->prev->next = first_obj;
                  }
                  if (obj_ptr == botObj) {
                     curPage->bot = botObj = first_obj;
                  } else {
                     obj_ptr->next->prev = first_obj;
                  }
                  obj_ptr->prev = NULL;
                  obj_ptr->next = NULL;

                  FreeObj(second_obj);
                  FreeObj(obj_ptr);

                  sel_ptr->obj = first_obj;
               }
            }
         }
      }
   }
}

static
int CountObjectWithShadow()
{
   struct SelRec *sel_ptr=NULL;
   int count=0;

   for (sel_ptr=botSel; sel_ptr != NULL; sel_ptr=sel_ptr->prev) {
      struct ObjRec *obj_ptr=sel_ptr->obj;

      if (obj_ptr->type == OBJ_GROUP) {
         struct GroupRec *group_ptr=obj_ptr->detail.r;
         struct ObjRec *first_obj=group_ptr->first;

         if (first_obj != NULL) {
            struct ObjRec *second_obj=first_obj->next;

            if (second_obj->next == NULL) {
               int first_w=(first_obj->obbox.rbx-first_obj->obbox.ltx);
               int first_h=(first_obj->obbox.rby-first_obj->obbox.lty);
               int second_w=(second_obj->obbox.rbx-second_obj->obbox.ltx);
               int second_h=(second_obj->obbox.rby-second_obj->obbox.lty);

               if (first_w == second_w && first_h == second_h) {
                  count++;
               }
            }
         }
      }
   }
   return count;
}

void RemoveObjectShadow()
{
   if (curChoice != NOTHING || topSel == NULL) {
      MsgBox(TgLoadCachedString(CSTID_NO_OBJ_SELECTED), TOOL_NAME, INFO_MB);
      return;
   }
   if (CountObjectWithShadow() == 0) {
      sprintf(gszMsgBox, TgLoadString(STID_NO_OBJ_SHADOW_FOUND));
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return;
   }
   HighLightReverse();
   PrepareToRecord(CMD_REPLACE, topSel, botSel, numObjSelected);
   DoRemoveObjectShadow();
   RecordCmd(CMD_REPLACE, NULL, topSel, botSel, numObjSelected);
   RedrawAnArea(botObj, selLtX-GRID_ABS_SIZE(1), selLtY-GRID_ABS_SIZE(1),
         selRbX+GRID_ABS_SIZE(1), selRbY+GRID_ABS_SIZE(1));
   HighLightForward();
   SetFileModified(TRUE);
   justDupped = FALSE;
}

int ReadObjectShadowInfo(buf)
   char *buf;
{
   int x=0, y=0;
   char color_str[40], *psz=NULL;

   if (importingFile) return TRUE;

   psz = FindChar((int)'(', buf);
   *color_str = '\0';
   psz = ParseStr(psz, (int)',', color_str, sizeof(color_str));
   InitScan(psz, "\t\n, []");
   if (GETINT("objshadow_info", x, "x offset") == INVALID ||
       GETINT("objshadow_info", y, "y offset") == INVALID) {
      return FALSE;
   }
   UtilTrimBlanks(color_str);
   if (!ignoreObjectShadowInfoInFile) {
      UtilStrCpyN(objShadowColorStr, sizeof(objShadowColorStr), color_str);
      objShadowXOffset = x;
      objShadowYOffset = y;
   }

   return TRUE;
}

/* ----------------------- ExtendSegment ----------------------- */

static
int ConsecutiveVerticesSelected(pn_index0, pn_index1)
   int *pn_index0, *pn_index1;
{
   int index0=topVSel->v_index[0], index1=topVSel->v_index[1];

   if (index0+1 == index1) {
      *pn_index0 = index0;
      *pn_index1 = index1;
      return TRUE;
   } else if (index1+1 == index0) {
      *pn_index0 = index1;
      *pn_index1 = index0;
      return TRUE;
   }
   return FALSE;
}

void ExtendSegment()
{
   char *c_ptr=NULL, sz_value[MAXSTRING+1];
   char sz_spec[MAXSTRING+1], sz_spec_copy[MAXSTRING+1];
   double dval=(double)0;
   struct ObjRec *obj_ptr=NULL;
   struct PolyRec *poly_ptr=NULL;
   int dx=0, dy=0, x0=0, y0=0, x1=0, y1=0;
   double dnew_x=0, dnew_y=0;
   double ddx=(double)0, ddy=(double)0;
   struct AttrRec *name_attr=NULL, *on_reshape_attr=NULL;
   int ltx, lty, rbx, rby, auto_center_attr=FALSE;
   int has_on_reshape=FALSE, two_point_polyline=TRUE, index0=0, index1=1;

   if (curChoice != NOTHING && curChoice != VERTEXMODE) {
      UtilStrCpyN(gszMsgBox, sizeof(gszMsgBox),
            TgLoadString(STID_TWO_VERTEX_POLY_ONLY));
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return;
   } else if (curChoice == NOTHING) {
      if (topSel == NULL || topSel != botSel ||
            topSel->obj->type != OBJ_POLY || topSel->obj->detail.p->n != 2) {
         UtilStrCpyN(gszMsgBox, sizeof(gszMsgBox),
               TgLoadString(STID_TWO_VERTEX_POLY_ONLY));
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
         return;
      }
      obj_ptr = topSel->obj;
   } else if (curChoice == VERTEXMODE) {
      if (topVSel == NULL) {
         if (topSel == NULL || topSel != botSel ||
               topSel->obj->type != OBJ_POLY || topSel->obj->detail.p->n != 2) {
            UtilStrCpyN(gszMsgBox, sizeof(gszMsgBox),
                  TgLoadString(STID_TWO_VERTEX_POLY_ONLY));
            MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
            return;
         }
         obj_ptr = topSel->obj;
      } else if (topVSel == botVSel && topVSel->n == 1 &&
            topVSel->obj->type == OBJ_POLY &&
            topVSel->obj->detail.p->n == 2) {
         /* treat this as if only the poly object is selected */
         obj_ptr = topVSel->obj;
      } else if (topVSel == botVSel && topVSel->n == 2 &&
            topVSel->obj->type == OBJ_POLY &&
            ConsecutiveVerticesSelected(&index0, &index1)) {
         /* treat this as if only the poly object is selected */
         obj_ptr = topVSel->obj;
         two_point_polyline = FALSE;
      } else {
         UtilStrCpyN(gszMsgBox, sizeof(gszMsgBox),
               TgLoadString(STID_TWO_CONSEC_VERTICES_ONLY));
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
         return;
      }
   }
   ltx = obj_ptr->bbox.ltx; lty = obj_ptr->bbox.lty;
   rbx = obj_ptr->bbox.rbx; rby = obj_ptr->bbox.rby;
   poly_ptr = obj_ptr->detail.p;

   *sz_spec = '\0';
   Dialog(TgLoadString(STID_ENTER_MULT_FACTOR),
         TgLoadCachedString(CSTID_DLG_ACCEPT_CANCEL), sz_spec);
   UtilTrimBlanks(sz_spec);
   if (*sz_spec == '\0') return;

   strcpy(sz_spec_copy, sz_spec);
   if ((c_ptr=strtok(sz_spec, " ,\t\n\r")) == NULL) return;
   if (*c_ptr == '+') {
      UtilStrCpyN(sz_value, sizeof(sz_value), &c_ptr[1]);
   } else {
      UtilStrCpyN(sz_value, sizeof(sz_value), c_ptr);
   }
   if (sscanf(sz_value, "%lf", &dval) != 1) {
      sprintf(gszMsgBox, TgLoadString(STID_FAIL_TO_PARSE_FOR_A_VAL),
            sz_spec_copy);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return;
   }
   if (obj_ptr->ctm == NULL) {
      dx = poly_ptr->vlist[index1].x - poly_ptr->vlist[index0].x;
      dy = poly_ptr->vlist[index1].y - poly_ptr->vlist[index0].y;
      ddx = ((double)dx) * dval;
      ddy = ((double)dy) * dval;
      dx = round(ddx);
      dy = round(ddy);
      if (dx == 0 && dy == 0) {
         UtilStrCpyN(gszMsgBox, sizeof(gszMsgBox),
               TgLoadString(STID_RES_LINESEG_HAVE_ZERO_LEN));
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
         return;
      }
   } else {
      TransformPointThroughCTM(poly_ptr->vlist[index0].x-obj_ptr->x,
            poly_ptr->vlist[index0].y-obj_ptr->y, obj_ptr->ctm, &x0, &y0);
      TransformPointThroughCTM(poly_ptr->vlist[index1].x-obj_ptr->x,
            poly_ptr->vlist[index1].y-obj_ptr->y, obj_ptr->ctm, &x1, &y1);
      dx = x1 - x0;
      dy = y1 - y0;
      ddx = ((double)dx) * dval;
      ddy = ((double)dy) * dval;
      dx = round(ddx);
      dy = round(ddy);
      if (dx == 0 && dy == 0) {
         UtilStrCpyN(gszMsgBox, sizeof(gszMsgBox),
               TgLoadString(STID_RES_LINESEG_HAVE_ZERO_LEN));
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
         return;
      }
   }
   auto_center_attr = AutoCenterAttr(obj_ptr);
   has_on_reshape = HasOnReshape(obj_ptr, &name_attr);

   if (has_on_reshape && name_attr != NULL) {
      on_reshape_attr = FindAttrWithName(obj_ptr, "on_reshape=", NULL);
   }
   HighLightReverse();
   if (on_reshape_attr != NULL) {
      StartCompositeCmd();
   }
   PrepareToReplaceAnObj(obj_ptr);
   if (obj_ptr->ctm == NULL) {
      if (dval > (double)0) {
         poly_ptr->vlist[index1].x = poly_ptr->vlist[index0].x + dx;
         poly_ptr->vlist[index1].y = poly_ptr->vlist[index0].y + dy;
      } else {
         poly_ptr->vlist[index0].x = poly_ptr->vlist[index1].x + dx;
         poly_ptr->vlist[index0].y = poly_ptr->vlist[index1].y + dy;
      }
      if (!two_point_polyline) {
         topVSel->x[0] = poly_ptr->vlist[index0].x;
         topVSel->y[0] = poly_ptr->vlist[index0].y;
         topVSel->x[1] = poly_ptr->vlist[index1].x;
         topVSel->y[1] = poly_ptr->vlist[index1].y;
      }
   } else {
      if (dval > (double)0) {
         x1 = x0 + obj_ptr->x + ddx;
         y1 = y0 + obj_ptr->y + ddy;
         ReverseTransformDoublePointThroughCTM(((double)x0)+ddx,
               ((double)y0)+ddy, obj_ptr->ctm, &dnew_x, &dnew_y);
         poly_ptr->vlist[index1].x = round(dnew_x) + obj_ptr->x;
         poly_ptr->vlist[index1].y = round(dnew_y) + obj_ptr->y;
      } else {
         x0 = x1 + obj_ptr->x + dx;
         y0 = y1 + obj_ptr->y + dy;
         ReverseTransformDoublePointThroughCTM(((double)x1)+ddx,
               ((double)y1)+ddy, obj_ptr->ctm, &dnew_x, &dnew_y);
         poly_ptr->vlist[index0].x = round(dnew_x) + obj_ptr->x;
         poly_ptr->vlist[index0].y = round(dnew_y) + obj_ptr->y;
      }
      if (!two_point_polyline) {
         int new_x=0, new_y=0;

         TransformPointThroughCTM(poly_ptr->vlist[index0].x-obj_ptr->x,
               poly_ptr->vlist[index0].y-obj_ptr->y, obj_ptr->ctm, &new_x,
               &new_y);
         topVSel->x[0] = obj_ptr->x + new_x;
         topVSel->y[0] = obj_ptr->y + new_y;
         TransformPointThroughCTM(poly_ptr->vlist[index1].x-obj_ptr->x,
               poly_ptr->vlist[index1].y-obj_ptr->y, obj_ptr->ctm, &new_x,
               &new_y);
         topVSel->x[1] = obj_ptr->x + new_x;
         topVSel->y[1] = obj_ptr->y + new_y;
      }
   }
   AdjObjSplineVs(obj_ptr);
   if (poly_ptr->curved != LT_INTSPLINE) {
      UpdPolyBBox(obj_ptr, poly_ptr->n, poly_ptr->vlist);
   } else {
      UpdPolyBBox(obj_ptr, poly_ptr->intn, poly_ptr->intvlist);
   }
   if (auto_center_attr) {
      struct AttrRec *attr_ptr=obj_ptr->fattr;
      int modified=FALSE;

      for ( ; attr_ptr != NULL; attr_ptr=attr_ptr->next) {
         if (attr_ptr->shown) {
            struct BBRec bbox;

            CenterObjInOBBox(attr_ptr->obj, obj_ptr->obbox, &bbox);
            if (bbox.ltx < ltx) ltx = bbox.ltx;
            if (bbox.lty < lty) lty = bbox.lty;
            if (bbox.rbx > rbx) rbx = bbox.rbx;
            if (bbox.rby > rby) rby = bbox.rby;
            modified = TRUE;
         }
      }
      if (modified) AdjObjBBox(obj_ptr);
   }
   RecordReplaceAnObj(obj_ptr);
   if (on_reshape_attr != NULL) {
      DoExec(on_reshape_attr, obj_ptr);
   }
   if (on_reshape_attr != NULL) {
      EndCompositeCmd();
   }
   UpdSelBBox();
   RedrawAreas(botObj, ltx-GRID_ABS_SIZE(1), lty-GRID_ABS_SIZE(1),
         rbx+GRID_ABS_SIZE(1), rby+GRID_ABS_SIZE(1),
         obj_ptr->bbox.ltx-GRID_ABS_SIZE(1),
         obj_ptr->bbox.lty-GRID_ABS_SIZE(1),
         obj_ptr->bbox.rbx+GRID_ABS_SIZE(1),
         obj_ptr->bbox.rby+GRID_ABS_SIZE(1));
   SetFileModified(TRUE);
   justDupped = FALSE;

   HighLightForward();
}

/* -------------------- ToggleTighterStructuredSplines() -------------------- */

void ToggleTighterStructuredSplines()
{
   tighterStructSplines = !tighterStructSplines;
   AdjSplineVs();
   SetFileModified(TRUE);
   UpdSelBBox();
   justDupped = FALSE;
   ClearAndRedrawDrawWindow();
   UtilStrCpyN(gszMsgBox, sizeof(gszMsgBox), TgLoadString(tighterStructSplines ?
         STID_WILL_USE_TIGHTER_SPLINES : STID_WILL_USE_LOOSER_SPLINES));
   Msg(gszMsgBox);
}

/* ----------------------- CreatePolySplineMenu() ----------------------- */

int RefreshPolySplineMenu(menu)
   TgMenu *menu;
{
   int ok=TRUE;

   /* UseTighterSplines */
#ifdef NOT_DEFINED
   ok &= TgSetMenuItemCheckById(menu, CMDID_TOGGLETIGHTERSPLINES,
         tighterStructSplines);
#endif /* NOT_DEFINED */

   return ok;
}

TgMenu *CreatePolySplineMenu(parent_menu, x, y, menu_info, status_str_xlated)
   TgMenu *parent_menu;
   int x, y;
   TgMenuInfo *menu_info;
   int status_str_xlated; /* ignored, always 0 */
{
   TgMenu *menu=TgCreateMenuFromMenuInfo(parent_menu, x, y, menu_info, FALSE);

   if (menu != NULL) {
      if (!RefreshPolySplineMenu(menu)) {
         return TgDestroyMenu(menu, TRUE);
      }
      menu->refresh_proc = ((RefreshMenuFunc*)RefreshPolySplineMenu);
   }
   return menu;
}

