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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/align.c,v 1.25 2011/05/16 16:21:56 william Exp $
 */

#define _INCLUDE_FROM_ALIGN_C_

#include "tgifdefs.h"
#include "cmdids.h"

#include "align.e"
#include "auxtext.e"
#include "button.e"
#include "choice.e"
#include "cmd.e"
#include "color.e"
#include "dialog.e"
#include "drawing.e"
#include "dup.e"
#include "grid.e"
#include "mark.e"
#include "mainmenu.e"
#include "menu.e"
#include "menuinfo.e"
#include "move.e"
#include "msg.e"
#include "obj.e"
#include "poly.e"
#include "raster.e"
#include "rect.e"
#include "select.e"
#include "setup.e"
#include "stretch.e"
#include "strtbl.e"
#include "util.e"

typedef struct DistrVRec {
   struct ObjRec *obj;
   struct VSelRec *vsel;
   int index, vindex, x, y;
   struct DistrVRec *next, *prev;
} * DistrVRecPtr;

int	horiAlign=ALIGN_L;
int	vertAlign=ALIGN_T;

int	alignDirectType=INVALID;

static
void SetSSSIForDistr(obj_ptr, vsel_ptr, update_pmvi)
   struct ObjRec *obj_ptr;
   struct VSelRec *vsel_ptr;
   int update_pmvi;
{
   IntPoint *vlist=NULL;
   int i=0, n=0;
   StretchStructuredSplineInfo *psssi=NULL;

   switch (obj_ptr->type) {
   case OBJ_POLY:
      vlist = obj_ptr->detail.p->vlist;
      n = obj_ptr->detail.p->n;
      if (obj_ptr->userdata == NULL &&
            obj_ptr->detail.p->curved == LT_STRUCT_SPLINE) {
         psssi = (StretchStructuredSplineInfo*)malloc(
               sizeof(StretchStructuredSplineInfo));
         if (psssi == NULL) FailAllocMessage();
         memset(psssi, 0, sizeof(StretchStructuredSplineInfo));

         SetIPTInfoForStretchPoly(vsel_ptr->v_index[0], n, vlist, psssi);

         psssi->orig_abs_x = psssi->new_abs_x = vlist[vsel_ptr->v_index[0]].x;
         psssi->orig_abs_y = psssi->new_abs_y = vlist[vsel_ptr->v_index[0]].y;
         obj_ptr->userdata = psssi;
         if (obj_ptr->ctm != NULL) {
            psssi->rotated_orig_abs_x = psssi->rotated_new_abs_x =
                  vsel_ptr->x[0];
            psssi->rotated_orig_abs_y = psssi->rotated_new_abs_y =
                  vsel_ptr->y[0];
         }
      }
      if (update_pmvi && vsel_ptr->pmvi == NULL &&
            obj_ptr->detail.p->curved != LT_STRUCT_SPLINE) {
         vsel_ptr->pmvi = (MoveVertexInfo*)malloc(
               vsel_ptr->n*sizeof(MoveVertexInfo));
         memset(vsel_ptr->pmvi, 0, vsel_ptr->n*sizeof(MoveVertexInfo));

         for (i=0; i < vsel_ptr->n; i++) {
            vsel_ptr->pmvi[i].orig_abs_x = vsel_ptr->pmvi[i].new_abs_x =
                  vlist[vsel_ptr->v_index[i]].x;
            vsel_ptr->pmvi[i].orig_abs_y = vsel_ptr->pmvi[i].new_abs_y =
                  vlist[vsel_ptr->v_index[i]].y;
            if (obj_ptr->ctm != NULL) {
               vsel_ptr->pmvi[i].rotated_orig_abs_x =
                     vsel_ptr->pmvi[i].rotated_new_abs_x = vsel_ptr->x[i];
               vsel_ptr->pmvi[i].rotated_orig_abs_y =
                     vsel_ptr->pmvi[i].rotated_new_abs_y = vsel_ptr->y[i];
            }
         }
      }
      break;
   case OBJ_POLYGON:
      vlist = obj_ptr->detail.g->vlist;
      n = obj_ptr->detail.g->n;
      if (obj_ptr->userdata == NULL &&
            obj_ptr->detail.g->curved == LT_STRUCT_SPLINE) {
         psssi = (StretchStructuredSplineInfo*)malloc(
               sizeof(StretchStructuredSplineInfo));
         if (psssi == NULL) FailAllocMessage();
         memset(psssi, 0, sizeof(StretchStructuredSplineInfo));

         SetIPTInfoForStretchPoly(vsel_ptr->v_index[0], n, vlist, psssi);

         psssi->orig_abs_x = psssi->new_abs_x = vlist[vsel_ptr->v_index[0]].x;
         psssi->orig_abs_y = psssi->new_abs_y = vlist[vsel_ptr->v_index[0]].y;
         obj_ptr->userdata = psssi;
         if (obj_ptr->ctm != NULL) {
            psssi->rotated_orig_abs_x = psssi->rotated_new_abs_x =
                  vsel_ptr->x[0];
            psssi->rotated_orig_abs_y = psssi->rotated_new_abs_y =
                  vsel_ptr->y[0];
         }
      }
      if (update_pmvi && vsel_ptr->pmvi == NULL &&
            obj_ptr->detail.g->curved != LT_STRUCT_SPLINE) {
         vsel_ptr->pmvi = (MoveVertexInfo*)malloc(
               vsel_ptr->n*sizeof(MoveVertexInfo));
         memset(vsel_ptr->pmvi, 0, vsel_ptr->n*sizeof(MoveVertexInfo));

         for (i=0; i < vsel_ptr->n; i++) {
            vsel_ptr->pmvi[i].orig_abs_x = vsel_ptr->pmvi[i].new_abs_x =
                  vlist[vsel_ptr->v_index[i]].x;
            vsel_ptr->pmvi[i].orig_abs_y = vsel_ptr->pmvi[i].new_abs_y =
                  vlist[vsel_ptr->v_index[i]].y;
            if (obj_ptr->ctm != NULL) {
               vsel_ptr->pmvi[i].rotated_orig_abs_x =
                     vsel_ptr->pmvi[i].rotated_new_abs_x = vsel_ptr->x[i];
               vsel_ptr->pmvi[i].rotated_orig_abs_y =
                     vsel_ptr->pmvi[i].rotated_new_abs_y = vsel_ptr->y[i];
            }
         }
      }
      break;
   }
}

void DistrSelObjs()
{
   int i=0, dx=0, dy=0, ltx, lty, rbx, rby, count, w, h;
   struct SelRec *sel_ptr=NULL, *next_sel=NULL;
   struct ObjRec *obj_ptr=NULL;
   struct VSelRec *vsel_ptr=NULL;
   double x=0.0, y=0.0, h_dist=0.0, v_dist=0.0;

   if ((topSel==NULL && topVSel==NULL) ||
         (horiAlign==ALIGN_N && vertAlign==ALIGN_N)) {
      return;
   }
   if (numObjLocked != 0) {
      MsgBox(TgLoadString(STID_CANNOT_DISTRIBUTE_LOCKED_OBJS),
            TOOL_NAME, INFO_MB);
      return;
   }
   if (curChoice == VERTEXMODE) {
      int start=0, vertices_count=0;
      struct DistrVRec *dv_ptr=NULL, *left_dv=NULL, *right_dv=NULL;
      struct DistrVRec *top_dv=NULL, *bottom_dv=NULL;
      struct DistrVRec *ptr=NULL, *hori_dv=NULL, *vert_dv=NULL;
      struct SelRec *tmp_top_sel=NULL, *tmp_bot_sel=NULL, *tmp_sel_ptr=NULL;
      StretchStructuredSplineInfo *psssi=NULL;

      if ((vertices_count=CountSelectedVertices()) <= 2) return;

      HighLightReverse();

      dv_ptr = (struct DistrVRec *)malloc(
            2*vertices_count*sizeof(struct DistrVRec));
      if (dv_ptr == NULL) FailAllocMessage();

      start = 1;

      ptr = dv_ptr;
      ptr->obj = topVSel->obj;
      ptr->vsel = topVSel;
      ptr->index = topVSel->v_index[0];
      ptr->vindex = 0;
      ptr->x = topVSel->x[0];
      ptr->y = topVSel->y[0];
      ptr->next = ptr->prev = NULL;
      left_dv = right_dv = ptr;

      ptr++;
      ptr->obj = topVSel->obj;
      ptr->vsel = topVSel;
      ptr->index = topVSel->v_index[0];
      ptr->vindex = 0;
      ptr->x = topVSel->x[0];
      ptr->y = topVSel->y[0];
      ptr->next = ptr->prev = NULL;
      top_dv = bottom_dv = ptr;

      ptr++;
      for (sel_ptr=topSel; sel_ptr != NULL; sel_ptr=sel_ptr->next) {
         sel_ptr->obj->marked = FALSE;
         sel_ptr->obj->userdata = NULL;
      }
      for (vsel_ptr=topVSel; vsel_ptr != NULL; vsel_ptr=vsel_ptr->next) {
         int obj_type=vsel_ptr->obj->type, last_index=0, n=0;

         if (obj_type == OBJ_POLYGON) {
            last_index = vsel_ptr->obj->detail.g->n-1;
         }
         obj_ptr = vsel_ptr->obj;
         n = vsel_ptr->n;
         for (i=start; i < n; i++) {
            if (!(obj_type==OBJ_POLYGON && vsel_ptr->v_index[i]==last_index)) {
               for (hori_dv=left_dv; hori_dv!=NULL; hori_dv=hori_dv->next) {
                  if (hori_dv->x > vsel_ptr->x[i] ||
                        (hori_dv->x == vsel_ptr->x[i] &&
                        hori_dv->y > vsel_ptr->y[i])) {
                     break;
                  }
               }
               ptr->obj = obj_ptr;
               ptr->vsel = vsel_ptr;
               ptr->index = vsel_ptr->v_index[i];
               ptr->vindex = i;
               ptr->x = vsel_ptr->x[i];
               ptr->y = vsel_ptr->y[i];

               ptr->next = hori_dv;
               if (hori_dv == NULL) {
                  ptr->prev = right_dv;
                  right_dv->next = ptr;
                  right_dv = ptr;
               } else {
                  ptr->prev = hori_dv->prev;
                  if (hori_dv->prev == NULL) {
                     left_dv = ptr;
                  } else {
                     hori_dv->prev->next = ptr;
                  }
                  hori_dv->prev = ptr;
               }
               ptr++;

               for (vert_dv=top_dv; vert_dv!=NULL; vert_dv=vert_dv->next) {
                  if (vert_dv->y > vsel_ptr->y[i] ||
                        (vert_dv->y == vsel_ptr->y[i] &&
                        vert_dv->x > vsel_ptr->x[i])) {
                     break;
                  }
               }
               ptr->obj = obj_ptr;
               ptr->vsel = vsel_ptr;
               ptr->index = vsel_ptr->v_index[i];
               ptr->vindex = i;
               ptr->x = vsel_ptr->x[i];
               ptr->y = vsel_ptr->y[i];

               ptr->next = vert_dv;
               if (vert_dv == NULL) {
                  ptr->prev = bottom_dv;
                  bottom_dv->next = ptr;
                  bottom_dv = ptr;
               } else {
                  ptr->prev = vert_dv->prev;
                  if (vert_dv->prev == NULL) {
                     top_dv = ptr;
                  } else {
                     vert_dv->prev->next = ptr;
                  }
                  vert_dv->prev = ptr;
               }
               ptr++;
            }
         }
         start = 0;
      }

      if (horiAlign != ALIGN_N) {
         for (ptr=left_dv; ptr->next!=right_dv; ptr=ptr->next) {
            obj_ptr = ptr->next->obj;
            vsel_ptr = ptr->next->vsel;
            if (!(obj_ptr->type==OBJ_POLYGON &&
                  ptr->next->index==obj_ptr->detail.g->n-1)) {
               obj_ptr->marked = TRUE;
               SetSSSIForDistr(obj_ptr, vsel_ptr, TRUE);
            }
         }
      }
      if (vertAlign != ALIGN_N) {
         for (ptr=top_dv; ptr->next!=bottom_dv; ptr=ptr->next) {
            obj_ptr = ptr->next->obj;
            vsel_ptr = ptr->next->vsel;
            if (!(obj_ptr->type==OBJ_POLYGON &&
                  ptr->next->index==obj_ptr->detail.g->n-1)) {
               obj_ptr->marked = TRUE;
               SetSSSIForDistr(obj_ptr, vsel_ptr, TRUE);
            }
         }
      }

      tmp_top_sel = tmp_bot_sel = NULL;
      count = 0;
      for (sel_ptr=topSel; sel_ptr != NULL; sel_ptr=sel_ptr->next) {
         obj_ptr = sel_ptr->obj;
         if (obj_ptr->marked) {
            count++;
            AddObjIntoSel(obj_ptr, tmp_bot_sel, NULL, &tmp_top_sel,
                  &tmp_bot_sel);
         }
      }
      if (count != 0) {
         PrepareToRecord(CMD_REPLACE, tmp_top_sel, tmp_bot_sel, count);
      }
      if (horiAlign != ALIGN_N) {
         x = (double)(left_dv->x);
         h_dist = ((double)(right_dv->x-x))/((double)(vertices_count-1));
         for (ptr=left_dv; ptr->next!=right_dv; ptr=ptr->next) {
            int new_x=0;

            obj_ptr = ptr->next->obj;
            vsel_ptr = ptr->next->vsel;

            if (obj_ptr->type==OBJ_POLYGON &&
                  ptr->next->index==obj_ptr->detail.g->n-1) {
               continue;
            }
            obj_ptr->marked = TRUE;
            new_x = round(x+h_dist);

            if (obj_ptr->userdata != NULL) {
               psssi = (StretchStructuredSplineInfo *)obj_ptr->userdata;

               if (obj_ptr->ctm == NULL) {
                  psssi->new_abs_x = new_x;
                  psssi->abs_dx = new_x - psssi->orig_abs_x;
               } else {
                  psssi->rotated_new_abs_x = new_x;
                  psssi->rotated_abs_dx = new_x - psssi->rotated_orig_abs_x;
               }
            } else {
               int vindex=ptr->next->vindex;
               MoveVertexInfo *pmvi=vsel_ptr->pmvi;

               if (obj_ptr->ctm == NULL) {
                  pmvi[vindex].new_abs_x = new_x;
                  pmvi[vindex].abs_dx = new_x - pmvi[vindex].orig_abs_x;
               } else {
                  pmvi[vindex].rotated_new_abs_x = new_x;
                  pmvi[vindex].rotated_abs_dx = new_x -
                        pmvi[vindex].rotated_orig_abs_x;
               }
            }
            vsel_ptr->x[ptr->next->vindex] = new_x;
            x += h_dist;
         }
      }
      if (vertAlign != ALIGN_N) {
         y = (double)(top_dv->y);
         v_dist = ((double)(bottom_dv->y-y))/((double)(vertices_count-1));
         for (ptr=top_dv; ptr->next!=bottom_dv; ptr=ptr->next) {
            int new_y=0;

            obj_ptr = ptr->next->obj;
            vsel_ptr = ptr->next->vsel;

            if (obj_ptr->type==OBJ_POLYGON &&
                  ptr->next->index==obj_ptr->detail.g->n-1) {
               continue;
            }
            obj_ptr->marked = TRUE;
            new_y = round(y+v_dist);

            if (obj_ptr->userdata != NULL) {
               psssi = (StretchStructuredSplineInfo *)obj_ptr->userdata;

               if (obj_ptr->ctm == NULL) {
                  psssi->new_abs_y = new_y;
                  psssi->abs_dy = new_y - psssi->orig_abs_y;
               } else {
                  psssi->rotated_new_abs_y = new_y;
                  psssi->rotated_abs_dy = new_y - psssi->rotated_orig_abs_y;
               }
            } else {
               int vindex=ptr->next->vindex;
               MoveVertexInfo *pmvi=vsel_ptr->pmvi;

               if (obj_ptr->ctm == NULL) {
                  pmvi[vindex].new_abs_y = new_y;
                  pmvi[vindex].abs_dy = new_y - pmvi[vindex].orig_abs_y;
               } else {
                  pmvi[vindex].rotated_new_abs_y = new_y;
                  pmvi[vindex].rotated_abs_dy = new_y -
                        pmvi[vindex].rotated_orig_abs_y;
               }
            }
            vsel_ptr->y[ptr->next->vindex] = new_y;
            y += v_dist;
         }
      }
      for (vsel_ptr=topVSel; vsel_ptr != NULL; vsel_ptr=vsel_ptr->next) {
         obj_ptr = vsel_ptr->obj;
         if (obj_ptr->marked) {
            IntPoint *vlist=NULL;
            
            switch (obj_ptr->type) {
            case OBJ_POLY: vlist = obj_ptr->detail.p->vlist; break;
            case OBJ_POLYGON: vlist = obj_ptr->detail.g->vlist; break;
            }
            if (obj_ptr->type==OBJ_POLYGON) {
               struct PolygonRec *polygon_ptr=obj_ptr->detail.g;

               polygon_ptr->vlist[polygon_ptr->n-1].x = polygon_ptr->vlist[0].x;
               polygon_ptr->vlist[polygon_ptr->n-1].y = polygon_ptr->vlist[0].y;
            }
            if (obj_ptr->userdata != NULL) {
               StretchStructuredSplineInfo *psssi=
                     (StretchStructuredSplineInfo *)obj_ptr->userdata;

               if (obj_ptr->ctm == NULL) {
                  FinishMoveVertexForStretchStructSpline(vsel_ptr,
                        psssi->abs_dx, psssi->abs_dy, psssi);
               } else {
                  FinishMoveVertexForStretchStructSpline(vsel_ptr,
                        psssi->rotated_abs_dx, psssi->rotated_abs_dy, psssi);
               }
               free(psssi);
               obj_ptr->userdata = NULL;
            } else {
               MoveVertexInfo *pmvi=vsel_ptr->pmvi;

               if (obj_ptr->ctm == NULL) {
                  for (i=0; i < vsel_ptr->n; i++) {
                     if (pmvi[i].abs_dx != 0 || pmvi[i].abs_dy != 0) {
                        vlist[vsel_ptr->v_index[i]].x += pmvi[i].abs_dx;
                        vlist[vsel_ptr->v_index[i]].y += pmvi[i].abs_dy;
                     }
                  }
               } else {
                  for (i=0; i < vsel_ptr->n; i++) {
                     if (pmvi[i].rotated_abs_dx != 0 ||
                           pmvi[i].rotated_abs_dy != 0) {
                        int tmp_x=0, tmp_y=0;

                        ReverseTransformPointThroughCTM(
                              pmvi[i].rotated_new_abs_x-obj_ptr->x,
                              pmvi[i].rotated_new_abs_y-obj_ptr->y,
                              obj_ptr->ctm, &tmp_x, &tmp_y);
                        vlist[vsel_ptr->v_index[i]].x = tmp_x + obj_ptr->x;
                        vlist[vsel_ptr->v_index[i]].y = tmp_y + obj_ptr->y;
                     }
                  }
               }
               free(pmvi);
               vsel_ptr->pmvi = NULL;
            }
            AdjObjSplineVs(obj_ptr);
            UpdPolyOrPolygonBBox(obj_ptr);
         }
      }
      if (count != 0) {
         RecordCmd(CMD_REPLACE, NULL, tmp_top_sel, tmp_bot_sel, count);
      }
      for (tmp_sel_ptr=tmp_top_sel; tmp_sel_ptr!=NULL; tmp_sel_ptr=next_sel) {
         next_sel = tmp_sel_ptr->next;
         free(tmp_sel_ptr);
      }
      free(dv_ptr);

      if (horiAlign != ALIGN_N) {
         sprintf(gszMsgBox, TgLoadString(STID_VERTICES_HORI_APART),
               round(h_dist));
         Msg(gszMsgBox);
      }
      if (vertAlign != ALIGN_N) {
         sprintf(gszMsgBox, TgLoadString(STID_VERTICES_VERT_APART),
               round(v_dist));
         Msg(gszMsgBox);
      }
   } else {
      struct SelRec *left_sel=NULL, *right_sel=NULL;
      struct SelRec *top_sel=NULL, *bottom_sel=NULL;
      struct SelRec *vert_sel=NULL, *hori_sel=NULL, *new_sel=NULL;
      struct SubCmdRec *sub_cmd=NULL;
      struct SelRec *tmp_sel_ptr=NULL;

      if (topSel==botSel || topSel->next==botSel) return;

      tmp_sel_ptr = (struct SelRec *)malloc(sizeof(struct SelRec));
      if (tmp_sel_ptr == NULL) FailAllocMessage();
      tmp_sel_ptr->next = tmp_sel_ptr->prev = NULL;

      sub_cmd = (struct SubCmdRec *)malloc(sizeof(struct SubCmdRec));
      if (sub_cmd == NULL) FailAllocMessage();
      memset(sub_cmd, 0, sizeof(struct SubCmdRec));

      StartCompositeCmd();
      HighLightReverse();

      left_sel = right_sel = (struct SelRec *)malloc(sizeof(struct SelRec));
      top_sel = bottom_sel = (struct SelRec *)malloc(sizeof(struct SelRec));
      if (left_sel == NULL || right_sel == NULL ||
            top_sel == NULL || bottom_sel == NULL) {
         FailAllocMessage();
      }
      left_sel->obj = right_sel->obj = botSel->obj;
      top_sel->obj = bottom_sel->obj = botSel->obj;
      left_sel->prev = right_sel->next = NULL;
      top_sel->prev = bottom_sel->next = NULL;

      count = 1;
      w = left_sel->obj->obbox.rbx - left_sel->obj->obbox.ltx;
      h = left_sel->obj->obbox.rby - left_sel->obj->obbox.lty;
      for (sel_ptr=botSel->prev; sel_ptr != NULL; sel_ptr=sel_ptr->prev) {
         count++;
         obj_ptr = sel_ptr->obj;
         w += obj_ptr->obbox.rbx - obj_ptr->obbox.ltx;
         h += obj_ptr->obbox.rby - obj_ptr->obbox.lty;
         switch (horiAlign) {
         case ALIGN_N:
         case ALIGN_L:
         case ALIGN_S:
            for (hori_sel=left_sel; hori_sel!=NULL; hori_sel=hori_sel->next) {
               if (hori_sel->obj->obbox.ltx > obj_ptr->obbox.ltx ||
                     (hori_sel->obj->obbox.ltx == obj_ptr->obbox.ltx &&
                     hori_sel->obj->obbox.lty > obj_ptr->obbox.lty)) {
                  break;
               }
            }
            break;
         case ALIGN_C:
            for (hori_sel=left_sel; hori_sel!=NULL; hori_sel=hori_sel->next) {
               if (hori_sel->obj->obbox.ltx+hori_sel->obj->obbox.rbx >
                     obj_ptr->obbox.ltx+obj_ptr->obbox.rbx ||
                     (hori_sel->obj->obbox.ltx+hori_sel->obj->obbox.rbx ==
                     obj_ptr->obbox.ltx+obj_ptr->obbox.rbx &&
                     hori_sel->obj->obbox.lty+hori_sel->obj->obbox.rby >
                     obj_ptr->obbox.lty+obj_ptr->obbox.rby)) {
                  break;
               }
            }
            break;
         case ALIGN_R:
            for (hori_sel=left_sel; hori_sel!=NULL; hori_sel=hori_sel->next) {
               if (hori_sel->obj->obbox.rbx > obj_ptr->obbox.rbx ||
                     (hori_sel->obj->obbox.rbx == obj_ptr->obbox.rbx &&
                     hori_sel->obj->obbox.rby > obj_ptr->obbox.rby)) {
                  break;
               }
            }
            break;
         }
         new_sel = (struct SelRec *)malloc(sizeof(struct SelRec));
         if (new_sel == NULL) FailAllocMessage();
         new_sel->obj = obj_ptr;
         new_sel->next = hori_sel;
         if (hori_sel == NULL) {
            new_sel->prev = right_sel;
            right_sel->next = new_sel;
            right_sel = new_sel;
         } else {
            new_sel->prev = hori_sel->prev;
            if (hori_sel->prev == NULL) {
               left_sel = new_sel;
            } else {
               hori_sel->prev->next = new_sel;
            }
            hori_sel->prev = new_sel;
         }
         switch (vertAlign) {
         case ALIGN_N:
         case ALIGN_T:
         case ALIGN_S:
            for (vert_sel=top_sel; vert_sel!=NULL; vert_sel=vert_sel->next) {
               if (vert_sel->obj->obbox.lty > obj_ptr->obbox.lty ||
                     (vert_sel->obj->obbox.lty == obj_ptr->obbox.lty &&
                     vert_sel->obj->obbox.ltx > obj_ptr->obbox.ltx)) {
                  break;
               }
            }
            break;
         case ALIGN_M:
            for (vert_sel=top_sel; vert_sel!=NULL; vert_sel=vert_sel->next) {
               if (vert_sel->obj->obbox.lty+vert_sel->obj->obbox.rby >
                     obj_ptr->obbox.lty+obj_ptr->obbox.rby ||
                     (vert_sel->obj->obbox.lty+vert_sel->obj->obbox.rby ==
                     obj_ptr->obbox.lty+obj_ptr->obbox.rby &&
                     vert_sel->obj->obbox.ltx+vert_sel->obj->obbox.rbx >
                     obj_ptr->obbox.ltx+obj_ptr->obbox.rbx)) {
                  break;
               }
            }
            break;
         case ALIGN_B:
            for (vert_sel=top_sel; vert_sel!=NULL; vert_sel=vert_sel->next) {
               if (vert_sel->obj->obbox.rby > obj_ptr->obbox.rby ||
                     (vert_sel->obj->obbox.rby == obj_ptr->obbox.rby &&
                     vert_sel->obj->obbox.rbx > obj_ptr->obbox.rbx)) {
                  break;
               }
            }
            break;
         }
         new_sel = (struct SelRec *)malloc(sizeof(struct SelRec));
         if (new_sel == NULL) FailAllocMessage();
         new_sel->obj = obj_ptr;
         new_sel->next = vert_sel;
         if (vert_sel == NULL)
         {
            new_sel->prev = bottom_sel;
            bottom_sel->next = new_sel;
            bottom_sel = new_sel;
         } else {
            new_sel->prev = vert_sel->prev;
            if (vert_sel->prev == NULL) {
               top_sel = new_sel;
            } else {
               vert_sel->prev->next = new_sel;
            }
            vert_sel->prev = new_sel;
         }
      }
      switch (horiAlign) {
      case ALIGN_N:
      case ALIGN_L:
         x = (double)(left_sel->obj->obbox.ltx);
         h_dist = ((double)(right_sel->obj->obbox.ltx-x))/((double)(count-1));
         break;
      case ALIGN_C:
         x = (double)(left_sel->obj->obbox.rbx+left_sel->obj->obbox.ltx);
         h_dist = ((double)(right_sel->obj->obbox.rbx +
               right_sel->obj->obbox.ltx - x)) / ((double)(count-1));
         break;
      case ALIGN_R:
         x = (double)(left_sel->obj->obbox.rbx);
         h_dist = ((double)(right_sel->obj->obbox.rbx-x))/((double)(count-1));
         break;
      case ALIGN_S:
         x = (double)(left_sel->obj->obbox.rbx);
         h_dist = ((double)(right_sel->obj->obbox.rbx -
               left_sel->obj->obbox.ltx - w)) / ((double)(count-1));
         break;
      }
      switch (vertAlign) {
      case ALIGN_N:
      case ALIGN_T:
         y = (double)(top_sel->obj->obbox.lty);
         v_dist = ((double)(bottom_sel->obj->obbox.lty-y))/((double)(count-1));
         break;
      case ALIGN_M:
         y = (double)(top_sel->obj->obbox.rby+top_sel->obj->obbox.lty);
         v_dist = ((double)(bottom_sel->obj->obbox.rby +
               bottom_sel->obj->obbox.lty - y)) / ((double)(count-1));
         break;
      case ALIGN_B:
         y = (double)(top_sel->obj->obbox.rby);
         v_dist = ((double)(bottom_sel->obj->obbox.rby-y))/((double)(count-1));
         break;
      case ALIGN_S:
         y = (double)(top_sel->obj->obbox.rby);
         v_dist = ((double)(bottom_sel->obj->obbox.rby -
               top_sel->obj->obbox.lty - h)) / ((double)(count-1));
         break;
      }
      for (sel_ptr=left_sel; sel_ptr->next!=right_sel; sel_ptr=next_sel) {
         switch (horiAlign) {
         case ALIGN_N: dx = 0; break;
         case ALIGN_L:
            dx = round(x+h_dist-sel_ptr->next->obj->obbox.ltx);
            break;
         case ALIGN_C:
            dx = round((x + h_dist - sel_ptr->next->obj->obbox.rbx -
                  sel_ptr->next->obj->obbox.ltx) / ((double)2.0));
            break;
         case ALIGN_R:
            dx = round(x+h_dist-sel_ptr->next->obj->obbox.rbx);
            break;
         case ALIGN_S:
            dx = round(x+h_dist-sel_ptr->next->obj->obbox.ltx);
            break;
         }
         if (dx != 0) {
            sub_cmd->detail.move.dx = dx;
            sub_cmd->detail.move.dy = 0;
            tmp_sel_ptr->obj = sel_ptr->next->obj;
            PrepareToRecord(CMD_MOVE, tmp_sel_ptr, tmp_sel_ptr, 1);
            RecordCmd(CMD_MOVE, sub_cmd, NULL, NULL, 0);

            MoveObj(sel_ptr->next->obj, dx, 0);
         }

         x += h_dist;
         if (horiAlign == ALIGN_S) {
            x += sel_ptr->next->obj->obbox.rbx - sel_ptr->next->obj->obbox.ltx;
         }
         next_sel = sel_ptr->next;
         free(sel_ptr);
      }
      free(sel_ptr);
      free(right_sel);

      for (sel_ptr=top_sel; sel_ptr->next!=bottom_sel; sel_ptr=next_sel) {
         switch (vertAlign) {
         case ALIGN_N: dy = 0; break;
         case ALIGN_T:
            dy = round(y+v_dist-sel_ptr->next->obj->obbox.lty);
            break;
         case ALIGN_M:
            dy = round((y + v_dist - sel_ptr->next->obj->obbox.rby -
                  sel_ptr->next->obj->obbox.lty) / ((double)2.0));
            break;
         case ALIGN_B:
            dy = round(y+v_dist-sel_ptr->next->obj->obbox.rby);
            break;
         case ALIGN_S:
            dy = round(y+v_dist-sel_ptr->next->obj->obbox.lty);
            break;
         }
         if (dy != 0) {
            sub_cmd->detail.move.dx = 0;
            sub_cmd->detail.move.dy = dy;
            tmp_sel_ptr->obj = sel_ptr->next->obj;
            PrepareToRecord(CMD_MOVE, tmp_sel_ptr, tmp_sel_ptr, 1);
            RecordCmd(CMD_MOVE, sub_cmd, NULL, NULL, 0);

            MoveObj(sel_ptr->next->obj, 0, dy);
         }

         y += v_dist;
         if (vertAlign == ALIGN_S) {
            y += sel_ptr->next->obj->obbox.rby - sel_ptr->next->obj->obbox.lty;
         }
         next_sel = sel_ptr->next;
         free(sel_ptr);
      }
      free(sel_ptr);
      free(bottom_sel);

      EndCompositeCmd();
      free(sub_cmd);
      free(tmp_sel_ptr);

      switch (horiAlign) {
      case ALIGN_L:
         sprintf(gszMsgBox, TgLoadString(STID_LEFT_SIDES_APART), round(h_dist));
         Msg(gszMsgBox);
         break;
      case ALIGN_C:
         sprintf(gszMsgBox, TgLoadString(STID_CENTERS_APART),
               round(h_dist/((double)2.0)));
         Msg(gszMsgBox);
         break;
      case ALIGN_R:
         sprintf(gszMsgBox, TgLoadString(STID_RIGHT_SIDES_APART),
               round(h_dist));
         Msg(gszMsgBox);
         break;
      case ALIGN_S:
         sprintf(gszMsgBox, TgLoadString(STID_SPACED_APART_HORI),
               round(h_dist));
         Msg(gszMsgBox);
         break;
      }
      switch (vertAlign) {
      case ALIGN_T:
         sprintf(gszMsgBox, TgLoadString(STID_TOP_SIDES_APART),
               round(v_dist));
         Msg(gszMsgBox);
         break;
      case ALIGN_M:
         sprintf(gszMsgBox, TgLoadString(STID_MIDDLES_APART),
               round(v_dist/((double)2.0)));
         Msg(gszMsgBox);
         break;
      case ALIGN_B:
         sprintf(gszMsgBox, TgLoadString(STID_BOTTOM_SIDES_APART),
               round(v_dist));
         Msg(gszMsgBox);
         break;
      case ALIGN_S:
         sprintf(gszMsgBox, TgLoadString(STID_SPACED_APART_VERT),
               round(v_dist));
         Msg(gszMsgBox);
         break;
      }
   }
   ltx = selLtX; lty = selLtY; rbx = selRbX, rby = selRbY;
   UpdSelBBox();
   RedrawAreas(botObj, ltx-GRID_ABS_SIZE(1), lty-GRID_ABS_SIZE(1),
         rbx+GRID_ABS_SIZE(1), rby+GRID_ABS_SIZE(1),
         selLtX-GRID_ABS_SIZE(1), selLtY-GRID_ABS_SIZE(1),
         selRbX+GRID_ABS_SIZE(1), selRbY+GRID_ABS_SIZE(1));
   HighLightForward();
   SetFileModified(TRUE);
   justDupped = FALSE;
}

void DistributeDirect(alignment)
   int alignment;
{
   int saved_h_align=horiAlign, saved_v_align=vertAlign;
   int v_mode=(alignment%MAXALIGNS);
   int h_mode=((alignment-v_mode)/MAXALIGNS);

   horiAlign = h_mode;
   vertAlign = v_mode;
   DistrSelObjs();
   horiAlign = saved_h_align;
   vertAlign = saved_v_align;
}

static
char *GetDistrDirectMenuStr(i)
   int i;
   /* returned string has been translated */
{
   char *psz=NULL, *msg=NULL;

   if (curChoice == VERTEXMODE) {
      int v_mode=(i%MAXALIGNS);
      int h_mode=((i-v_mode)/MAXALIGNS);

      if (v_mode == ALIGN_N) {
         if (h_mode == ALIGN_N) {
            msg = TgLoadCachedString(CSTID_PARANED_NONE);
         } else {
            msg = TgLoadString(STID_DISTR_VERTEX_HORI);
         }
      } else {
         if (h_mode == ALIGN_N) {
            msg = TgLoadString(STID_DISTR_VERTEX_VERT);
         } else {
            msg = TgLoadString(STID_DISTR_VERTEX_VERT_AND_HORI);
         }
      }
   } else {
      msg = DistrDirectLoadString(i);
   }
   psz = UtilStrDup(msg);
   if (psz == NULL) FailAllocMessage();
   return psz;
}

TgMenu *CreateDistributeDirectMenu(parent_menu, x, y, menu_info,
      status_str_xlated)
   TgMenu *parent_menu;
   int x, y;
   TgMenuInfo *menu_info;
   int status_str_xlated; /* ignored, always 0 */
{
   int i=0, num_entries=(MAXALIGNS*MAXALIGNS);
   TgMenu *menu=NULL;
   TgMenuInfo stMenuInfo;
   TgMenuItemInfo *item_info=NULL;

   memcpy(&stMenuInfo, menu_info, sizeof(TgMenuInfo));
   stMenuInfo.items = (TgMenuItemInfo*)malloc(
         (num_entries+1)*sizeof(TgMenuItemInfo));
   if (stMenuInfo.items == NULL) FailAllocMessage();
   memset(stMenuInfo.items, 0,
         (num_entries+1)*sizeof(TgMenuItemInfo));
   for (item_info=stMenuInfo.items, i=0; i < num_entries; item_info++, i++) {
      item_info->menu_str = (char*)(Pixmap*)(&distrDirectPixmap[i]);
      item_info->shortcut_str = NULL;
      item_info->status_str = GetDistrDirectMenuStr(i);
      item_info->submenu_info = NULL;
      item_info->cmdid = CMDID_DISTRIBUTEDIRECT;
   }
   stMenuInfo.items[num_entries].cmdid = INVALID;

   /* the status_str has been translated in GetDistrDirectMenuStr() */
   menu = TgCreateMenuFromMenuInfo(parent_menu, x, y, &stMenuInfo, TRUE);
   for (item_info=stMenuInfo.items, i=0; i < num_entries; item_info++, i++) {
      UtilFree(item_info->status_str);
   }
   memset(stMenuInfo.items, 0, (num_entries+1)*sizeof(TgMenuItemInfo));
   free(stMenuInfo.items);
   stMenuInfo.items = NULL;
   if (menu != NULL) {
      menu->track_menubar = TRUE;
      TgAdjustMenuGeometry(menu, choiceImageW, choiceImageH, MAXALIGNS);
   }
   return menu;
}

static
int AutoJustifiable(obj_ptr)
   struct ObjRec *obj_ptr;
{
   struct TextRec *text_ptr;
   int auto_justifiable=TRUE, just;

   if (obj_ptr->type!=OBJ_TEXT || obj_ptr->detail.t->lines!=1) return FALSE;
   if (obj_ptr->ctm != NULL) return FALSE;

   text_ptr = obj_ptr->detail.t;
   just = text_ptr->minilines.just;

   if ((horiAlign==ALIGN_L && just==JUST_L) ||
         (horiAlign==ALIGN_C && just==JUST_C) ||
         (horiAlign==ALIGN_R && just==JUST_R)) {
      auto_justifiable = FALSE;
   }
   return auto_justifiable;
}

static
void AutoJustify(obj_ptr)
   struct ObjRec *obj_ptr;
{
   struct TextRec *text_ptr;
   int new_ltx, new_lty, dx, dy, ltx, lty;

   text_ptr = obj_ptr->detail.t;

   ltx = obj_ptr->obbox.ltx; lty = obj_ptr->obbox.lty;
   switch (horiAlign) {
   case ALIGN_L: text_ptr->minilines.just = JUST_L; break;
   case ALIGN_C: text_ptr->minilines.just = JUST_C; break;
   case ALIGN_R: text_ptr->minilines.just = JUST_R; break;
   }
   UpdTextBBox(obj_ptr);
   dx = dy = 0;
   new_ltx = obj_ptr->obbox.ltx; new_lty = obj_ptr->obbox.lty;
   dx = ltx-new_ltx;

   if (text_ptr->cached_bitmap != None) {
      XFreePixmap(mainDisplay, text_ptr->cached_bitmap);
   }
   text_ptr->cached_zoom = 0;
   text_ptr->cached_bitmap = None;

   MoveObj(obj_ptr, dx, dy);
}

void AlignSelObjs()
{
   register int x=0, y=0, i;
   struct SelRec *sel_ptr=NULL;
   struct ObjRec *obj_ptr=NULL;
   struct VSelRec *vsel_ptr=NULL;
   int pivot_x=0, pivot_y=0, auto_justifiable=FALSE;
   int dx=0, dy=0, ltx=0, lty=0, rbx=0, rby=0;

   if (topSel == NULL && topVSel == NULL) return;

   if (curChoice == VERTEXMODE) {
      int changed=FALSE, n=0, first_time=TRUE, start;
      IntPoint *vlist=NULL;

      if (topVSel!=NULL && (topVSel->next!=NULL || topVSel->n>=3  ||
            (topVSel->n==2 &&
            !(topVSel->obj->type==OBJ_POLYGON && topVSel->v_index[0]==0))) &&
            horiAlign!=ALIGN_N && horiAlign!=ALIGN_S &&
            vertAlign!=ALIGN_N && vertAlign!=ALIGN_S) {
         if (MsgBox(TgLoadString(STID_MOVE_ALL_VERTICES_YNC),
               TOOL_NAME, YNC_MB) != MB_ID_YES) {
            return;
         }
      }
      StartCompositeCmd();
      HighLightReverse();
      for (vsel_ptr = topVSel; vsel_ptr != NULL; vsel_ptr = vsel_ptr->next) {
         if (first_time) {
            first_time = FALSE;
            ltx = vsel_ptr->x[0]; lty = vsel_ptr->y[0];
            rbx = vsel_ptr->x[0]; rby = vsel_ptr->y[0];
            start = 1;
         } else {
            start = 0;
         }
         for (i = start; i < vsel_ptr->n; i++) {
            if (vsel_ptr->x[i] < ltx) ltx = vsel_ptr->x[i];
            if (vsel_ptr->y[i] < lty) lty = vsel_ptr->y[i];
            if (vsel_ptr->x[i] > rbx) rbx = vsel_ptr->x[i];
            if (vsel_ptr->y[i] > rby) rby = vsel_ptr->y[i];
         }
      }
      switch (horiAlign) {
      case ALIGN_L: pivot_x = ltx; break;
      case ALIGN_C: pivot_x = (ltx + rbx) / 2; break;
      case ALIGN_R: pivot_x = rbx; break;
      }
      switch (vertAlign) {
      case ALIGN_T: pivot_y = lty; break;
      case ALIGN_M: pivot_y = (lty + rby) / 2; break;
      case ALIGN_B: pivot_y = rby; break;
      }

      for (vsel_ptr = topVSel; vsel_ptr != NULL; vsel_ptr = vsel_ptr->next) {
         StretchStructuredSplineInfo sssi;
         int curved=(-1), ssn=(-1);
         IntPoint *ssvlist=NULL;
         struct PolyRec *poly_ptr=NULL;
         struct PolygonRec *polygon_ptr=NULL;

         memset(&sssi, 0, sizeof(StretchStructuredSplineInfo));
         obj_ptr = vsel_ptr->obj;
         switch (obj_ptr->type) {
         case OBJ_POLY:
            poly_ptr = obj_ptr->detail.p;
            curved = poly_ptr->curved;
            n = poly_ptr->n;
            vlist = poly_ptr->vlist;
            if (curved == LT_STRUCT_SPLINE) {
               ssn = poly_ptr->ssn;
               ssvlist = poly_ptr->ssvlist;
               SetIPTInfoForStretchPoly(vsel_ptr->v_index[0], n, vlist, &sssi);
            }
            break;
         case OBJ_POLYGON:
            polygon_ptr = obj_ptr->detail.g;
            curved = polygon_ptr->curved;
            n = polygon_ptr->n;
            vlist = polygon_ptr->vlist;
            if (curved == LT_STRUCT_SPLINE) {
               ssn = polygon_ptr->ssn;
               ssvlist = polygon_ptr->ssvlist;
               SetIPTInfoForStretchPolygon(vsel_ptr->v_index[0], n, vlist,
                     &sssi);
            }
            break;
         }
         PrepareToReplaceAnObj(obj_ptr);
         for (i=0; i < vsel_ptr->n; i++) {
            int abs_dx=0,  abs_dy=0;

            if (horiAlign!=ALIGN_N && horiAlign!=ALIGN_S &&
                  vsel_ptr->x[i]!=pivot_x) {
               changed = TRUE;
               abs_dx = pivot_x - vsel_ptr->x[i];
            }
            if (vertAlign!=ALIGN_N && vertAlign!=ALIGN_S &&
                  vsel_ptr->y[i]!=pivot_y) {
               changed = TRUE;
               abs_dy = pivot_y - vsel_ptr->y[i];
            }
            if (changed) {
               if (obj_ptr->ctm == NULL) {
                  vsel_ptr->x[i] = pivot_x;
                  vsel_ptr->y[i] = pivot_y;
                  if (curved == LT_STRUCT_SPLINE) {
                     /*
                      * don't do anything here, will be handled in
                      *       FinishMoveVertexForStretchStructSpline();
                      */
                  } else {
                     vlist[vsel_ptr->v_index[i]].x = pivot_x;
                     vlist[vsel_ptr->v_index[i]].y = pivot_y;
                  }
               } else {
                  int tmp_x=0, tmp_y=0, tmp_x2=0, tmp_y2=0;

                  ReverseTransformPointThroughCTM(
                        pivot_x-obj_ptr->x, pivot_y-obj_ptr->y, obj_ptr->ctm,
                        &tmp_x, &tmp_y);
                  if (curved == LT_STRUCT_SPLINE) {
                     /*
                      * don't do anything here, will be handled in
                      *       FinishMoveVertexForStretchStructSpline();
                      */
                  } else {
                     dx = tmp_x + obj_ptr->x - vlist[vsel_ptr->v_index[i]].x;
                     dy = tmp_y + obj_ptr->y - vlist[vsel_ptr->v_index[i]].y;
                     vlist[vsel_ptr->v_index[i]].x = tmp_x + obj_ptr->x;
                     vlist[vsel_ptr->v_index[i]].y = tmp_y + obj_ptr->y;
                  }
                  TransformPointThroughCTM(tmp_x, tmp_y, obj_ptr->ctm,
                         &tmp_x2, &tmp_y2);
                  vsel_ptr->x[i] = tmp_x2 + obj_ptr->x;     
                  vsel_ptr->y[i] = tmp_y2 + obj_ptr->y; 
               }
            }
            if (curved == LT_STRUCT_SPLINE) {
               FinishMoveVertexForStretchStructSpline(vsel_ptr, abs_dx, abs_dy,
                     &sssi);
            }
         }
         if (changed) {
            AdjObjSplineVs(obj_ptr);
            UpdNonIntSplinePolyBBox(obj_ptr, n, vlist);
            RecordReplaceAnObj(obj_ptr);
         } else {
            AbortPrepareCmd(CMD_REPLACE);
         }
      }
      EndCompositeCmd();
   } else {
      struct SubCmdRec *sub_cmd=NULL;
      struct SelRec *tmp_sel_ptr=NULL;
      struct ObjRec *locked_obj=NULL;
      char *psz=NULL;

      if (numObjLocked > 1) {
         MsgBox(TgLoadString(STID_CANNOT_ALIGN_OBJS_TOO_MANY), TOOL_NAME,
               INFO_MB);
         return;
      } else if (numObjLocked == 1) {
         for (sel_ptr=topSel; sel_ptr!=NULL; sel_ptr=sel_ptr->next) {
            if (sel_ptr->obj->locked) {
               locked_obj = sel_ptr->obj;
               break;
            }
         }
      }
      tmp_sel_ptr = (struct SelRec *)malloc(sizeof(struct SelRec));
      if (tmp_sel_ptr == NULL) FailAllocMessage();
      tmp_sel_ptr->next = tmp_sel_ptr->prev = NULL;

      sub_cmd = (struct SubCmdRec *)malloc(sizeof(struct SubCmdRec));
      if (sub_cmd == NULL) FailAllocMessage();
      memset(sub_cmd, 0, sizeof(struct SubCmdRec));

      StartCompositeCmd();
      HighLightReverse();
      switch (horiAlign) {
      case ALIGN_L:
         pivot_x = (locked_obj==NULL) ? selObjLtX : locked_obj->obbox.ltx;
         break;
      case ALIGN_C:
         pivot_x = (locked_obj==NULL) ? ((selObjLtX+selObjRbX)>>1) :
               ((locked_obj->obbox.ltx+locked_obj->obbox.rbx)>>1);
         break;
      case ALIGN_R:
         pivot_x = (locked_obj==NULL) ? selObjRbX : locked_obj->obbox.rbx;
         break;
      }
      switch (vertAlign) {
      case ALIGN_T:
         pivot_y = (locked_obj==NULL) ? selObjLtY : locked_obj->obbox.lty;
         break;
      case ALIGN_M:
         pivot_y = (locked_obj==NULL) ? ((selObjLtY+selObjRbY)>>1) :
               ((locked_obj->obbox.lty+locked_obj->obbox.rby)>>1);
         break;
      case ALIGN_B:
         pivot_y = (locked_obj==NULL) ? selObjRbY : locked_obj->obbox.rby;
         break;
      }

      for (sel_ptr = topSel; sel_ptr != NULL; sel_ptr = sel_ptr->next) {
         obj_ptr = sel_ptr->obj;
         if (obj_ptr->locked) continue;

         if ((auto_justifiable = AutoJustifiable(obj_ptr))) {
            PrepareToReplaceAnObj(obj_ptr);
            AutoJustify(obj_ptr);
         }
         switch (horiAlign) {
         case ALIGN_L: x = obj_ptr->obbox.ltx; break;
         case ALIGN_C: x = (obj_ptr->obbox.ltx+obj_ptr->obbox.rbx)/2; break;
         case ALIGN_R: x = obj_ptr->obbox.rbx; break;
         }
         switch (vertAlign) {
         case ALIGN_T: y = obj_ptr->obbox.lty; break;
         case ALIGN_M: y = (obj_ptr->obbox.lty+obj_ptr->obbox.rby)/2; break;
         case ALIGN_B: y = obj_ptr->obbox.rby; break;
         }
         if (horiAlign==ALIGN_N || horiAlign==ALIGN_S) x = pivot_x;
         if (vertAlign==ALIGN_N || vertAlign==ALIGN_S) y = pivot_y;

         dx = pivot_x - x;
         dy = pivot_y - y;
         if (dx != 0 || dy != 0) {
            if (auto_justifiable) {
               MoveObj(obj_ptr, dx, dy);
               RecordReplaceAnObj(obj_ptr);
            } else {
               sub_cmd->detail.move.dx = dx;
               sub_cmd->detail.move.dy = dy;
               tmp_sel_ptr->obj = obj_ptr;
               PrepareToRecord(CMD_MOVE, tmp_sel_ptr, tmp_sel_ptr, 1);
               RecordCmd(CMD_MOVE, sub_cmd, NULL, NULL, 0);

               MoveObj(obj_ptr, dx, dy);
            }
         } else if (auto_justifiable) {
            RecordReplaceAnObj(obj_ptr);
         }
      }
      EndCompositeCmd();

      free(sub_cmd);
      free(tmp_sel_ptr);

      psz = AlignedLoadString(horiAlign, vertAlign);
      if (psz != NULL) Msg(psz);
   }
   ltx = selLtX; lty = selLtY; rbx = selRbX, rby = selRbY;
   UpdSelBBox();
   RedrawAreas(botObj, ltx-GRID_ABS_SIZE(1), lty-GRID_ABS_SIZE(1),
         rbx+GRID_ABS_SIZE(1), rby+GRID_ABS_SIZE(1),
         selLtX-GRID_ABS_SIZE(1), selLtY-GRID_ABS_SIZE(1),
         selRbX+GRID_ABS_SIZE(1), selRbY+GRID_ABS_SIZE(1));
   HighLightForward();
   SetFileModified(TRUE);
   justDupped = FALSE;
}

void AlignSelToPage()
{
   int i=0;
   struct SelRec *sel_ptr=NULL;
   struct ObjRec *obj_ptr=NULL;
   struct VSelRec *vsel_ptr=NULL;
   int pivot_x=0, pivot_y=0, dx=0, dy=0, ltx=0, lty=0, rbx=0, rby=0;

   if (topSel == NULL && topVSel == NULL) return;

   if (curChoice == VERTEXMODE) {
      int changed=FALSE, n=0;
      IntPoint *vlist=NULL;

      if (topVSel!=NULL && (topVSel->next!=NULL || topVSel->n>=3  ||
            (topVSel->n==2 &&
            !(topVSel->obj->type==OBJ_POLYGON && topVSel->v_index[0]==0))) &&
            horiAlign!=ALIGN_N && horiAlign!=ALIGN_S &&
            vertAlign!=ALIGN_N && vertAlign!=ALIGN_S) {
         if (MsgBox(TgLoadString(STID_MOVE_ALL_VERTICES_YNC), TOOL_NAME,
               YNC_MB) != MB_ID_YES) {
            return;
         }
      }
      for (sel_ptr=topSel; sel_ptr != NULL; sel_ptr=sel_ptr->next) {
         sel_ptr->obj->userdata = NULL;
      }
      StartCompositeCmd();
      HighLightReverse();

      switch (horiAlign) {
      case ALIGN_L: pivot_x = 0; break;
      case ALIGN_C: pivot_x = paperWidth / 2; break;
      case ALIGN_R: pivot_x = paperWidth; break;
      }
      switch (vertAlign) {
      case ALIGN_T: pivot_y = 0; break;
      case ALIGN_M: pivot_y = paperHeight / 2; break;
      case ALIGN_B: pivot_y = paperHeight; break;
      }
      for (vsel_ptr=topVSel; vsel_ptr != NULL; vsel_ptr=vsel_ptr->next) {
         obj_ptr = vsel_ptr->obj;
         SetSSSIForDistr(obj_ptr, vsel_ptr, FALSE);

         switch (obj_ptr->type) {
         case OBJ_POLY:
            n = obj_ptr->detail.p->n;
            vlist = obj_ptr->detail.p->vlist;
            break;
         case OBJ_POLYGON:
            n = obj_ptr->detail.g->n;
            vlist = obj_ptr->detail.g->vlist;
            break;
         }
         PrepareToReplaceAnObj(obj_ptr);
         for (i = 0; i < vsel_ptr->n; i++) {
            if (horiAlign!=ALIGN_N && horiAlign!=ALIGN_S &&
                  vsel_ptr->x[i]!=pivot_x) {
               changed = TRUE;
               if (obj_ptr->userdata != NULL) {
                  StretchStructuredSplineInfo *psssi=
                        (StretchStructuredSplineInfo *)obj_ptr->userdata;

                  if (obj_ptr->ctm == NULL) {
                     psssi->new_abs_x = pivot_x;
                     psssi->abs_dx = pivot_x - psssi->orig_abs_x;
                  } else {
                     psssi->rotated_new_abs_x = pivot_x;
                     psssi->rotated_abs_dx = pivot_x -
                           psssi->rotated_orig_abs_x;
                  }
               } else {
                  vlist[vsel_ptr->v_index[i]].x = pivot_x;
               }
               vsel_ptr->x[i] = pivot_x;
            }
            if (vertAlign!=ALIGN_N && vertAlign!=ALIGN_S &&
                  vsel_ptr->y[i]!=pivot_y) {
               changed = TRUE;
               if (obj_ptr->userdata != NULL) {
                  StretchStructuredSplineInfo *psssi=
                        (StretchStructuredSplineInfo *)obj_ptr->userdata;

                  if (obj_ptr->ctm == NULL) {
                     psssi->new_abs_y = pivot_y;
                     psssi->abs_dy = pivot_y - psssi->orig_abs_y;
                  } else {
                     psssi->rotated_new_abs_y = pivot_y;
                     psssi->rotated_abs_dy = pivot_y -
                           psssi->rotated_orig_abs_y;
                  }
               } else {
                  vlist[vsel_ptr->v_index[i]].y = pivot_y;
               }
               vsel_ptr->y[i] = pivot_y;
            }
            if (obj_ptr->userdata != NULL) {
               StretchStructuredSplineInfo *psssi=
                     (StretchStructuredSplineInfo *)obj_ptr->userdata;

               if (changed) {
                  if (obj_ptr->ctm == NULL) {
                     FinishMoveVertexForStretchStructSpline(vsel_ptr,
                           psssi->abs_dx, psssi->abs_dy, psssi);
                  } else {
                     FinishMoveVertexForStretchStructSpline(vsel_ptr,
                           psssi->rotated_abs_dx, psssi->rotated_abs_dy, psssi);
                  }
               }
               free(psssi);
               obj_ptr->userdata = NULL;
            }
         }
         if (changed) {
            AdjObjSplineVs(obj_ptr);
            UpdNonIntSplinePolyBBox(obj_ptr, n, vlist);
            RecordReplaceAnObj(obj_ptr);
         } else {
            AbortPrepareCmd(CMD_REPLACE);
         }
      }
      EndCompositeCmd();
   } else {
      HighLightReverse();
      dx = dy = 0;
      switch (horiAlign) {
      case ALIGN_L: dx = 0 - selLtX; break;
      case ALIGN_C: dx = (paperWidth>>1) - ((selRbX+selLtX)>>1); break;
      case ALIGN_R: dx = paperWidth - selRbX; break;
      }
      switch (vertAlign) {
      case ALIGN_T: dy = 0 - selLtY; break;
      case ALIGN_M: dy = (paperHeight>>1) - ((selRbY+selLtY)>>1); break;
      case ALIGN_B: dy = paperHeight - selRbY; break;
      }
      if (dx != 0 || dy != 0) {
         struct SubCmdRec *sub_cmd=NULL;

         sub_cmd = (struct SubCmdRec *)malloc(sizeof(struct SubCmdRec));
         if (sub_cmd == NULL) FailAllocMessage();
         memset(sub_cmd, 0, sizeof(struct SubCmdRec));

         sub_cmd->detail.move.dx = dx;
         sub_cmd->detail.move.dy = dy;

         PrepareToRecord(CMD_MOVE, topSel, botSel, numObjSelected);
         RecordCmd(CMD_MOVE, sub_cmd, NULL, NULL, 0);
         for (sel_ptr = topSel; sel_ptr != NULL; sel_ptr = sel_ptr->next) {
            if (!sel_ptr->obj->locked) {
               MoveObj(sel_ptr->obj, dx, dy);
            }
         }
         free(sub_cmd);
      }
   }
   ltx = selLtX; lty = selLtY; rbx = selRbX, rby = selRbY;
   UpdSelBBox();
   RedrawAreas(botObj, ltx-GRID_ABS_SIZE(1), lty-GRID_ABS_SIZE(1),
         rbx+GRID_ABS_SIZE(1), rby+GRID_ABS_SIZE(1),
         selLtX-GRID_ABS_SIZE(1), selLtY-GRID_ABS_SIZE(1),
         selRbX+GRID_ABS_SIZE(1), selRbY+GRID_ABS_SIZE(1));
   HighLightForward();
   SetFileModified(TRUE);
   justDupped = FALSE;
}

void AlignSelToGrid()
{
   int x=0, y=0, i;
   struct ObjRec *obj_ptr=NULL;
   struct SelRec *sel_ptr=NULL;
   struct VSelRec *vsel_ptr=NULL;
   int grid_x, grid_y, dx, dy, ltx, lty, rbx, rby, auto_justifiable;
   int saved_draw_orig_x=0, saved_draw_orig_y=0;

   if (topSel == NULL && topVSel == NULL) return;

   StartCompositeCmd();
   HighLightReverse();
   if (curChoice == VERTEXMODE) {
      for (vsel_ptr = topVSel; vsel_ptr != NULL; vsel_ptr = vsel_ptr->next) {
         int changed=FALSE, n=0, ssn=(-1), curved=(-1);
         IntPoint *vlist=NULL, *ssvlist=NULL;
         struct PolyRec *poly_ptr=NULL;
         struct PolygonRec *polygon_ptr=NULL;
         StretchStructuredSplineInfo sssi;

         memset(&sssi, 0, sizeof(StretchStructuredSplineInfo));

         obj_ptr = vsel_ptr->obj;
         switch (obj_ptr->type) {
         case OBJ_POLY:
            poly_ptr = obj_ptr->detail.p;
            curved = poly_ptr->curved;
            n = poly_ptr->n;
            vlist = poly_ptr->vlist;
            if (curved == LT_STRUCT_SPLINE) {
               ssn = poly_ptr->ssn;
               ssvlist = poly_ptr->ssvlist;
               SetIPTInfoForStretchPoly(vsel_ptr->v_index[0], n, vlist, &sssi);
            }
            break;
         case OBJ_POLYGON:
            polygon_ptr = obj_ptr->detail.g;
            curved = polygon_ptr->curved;
            n = polygon_ptr->n;
            vlist = polygon_ptr->vlist;
            if (curved == LT_STRUCT_SPLINE) {
               ssn = polygon_ptr->ssn;
               ssvlist = polygon_ptr->ssvlist;
               SetIPTInfoForStretchPolygon(vsel_ptr->v_index[0], n, vlist,
                     &sssi);
            }
            break;
         }
         PrepareToReplaceAnObj(obj_ptr);

         saved_draw_orig_x = drawOrigX;
         saved_draw_orig_y = drawOrigY;

         drawOrigX = drawOrigY = 0;

         for (i = 0; i < vsel_ptr->n; i++) {
            if (horiAlign!=ALIGN_N && horiAlign!=ALIGN_S) x = vsel_ptr->x[i];
            if (vertAlign!=ALIGN_N && vertAlign!=ALIGN_S) y = vsel_ptr->y[i];
            if (zoomedIn) {
               GridXY(ZOOMED_SIZE(x), ZOOMED_SIZE(y), &grid_x, &grid_y);
               if (horiAlign==ALIGN_N || horiAlign==ALIGN_S) {
                  x = ABS_SIZE(grid_x);
               }
               if (vertAlign==ALIGN_N || vertAlign==ALIGN_S) {
                  y = ABS_SIZE(grid_y);
               }
               dx = ABS_SIZE(grid_x) - x;
               dy = ABS_SIZE(grid_y) - y;
            } else {
               GridXY(ZOOMED_SIZE(x), ZOOMED_SIZE(y), &grid_x, &grid_y);
               if (horiAlign==ALIGN_N || horiAlign==ALIGN_S) x = grid_x;
               if (vertAlign==ALIGN_N || vertAlign==ALIGN_S) y = grid_y;

               dx = ABS_SIZE(grid_x) - x;
               dy = ABS_SIZE(grid_y) - y;
            }
            if (dx != 0 || dy != 0) {
               changed = TRUE;
               if (ssvlist != NULL) {
                  /* LT_STRUCT_SPLINE */
                  if (obj_ptr->ctm == NULL) {
                     vsel_ptr->x[i] += dx;
                     vsel_ptr->y[i] += dy;
                  } else {
                     int tmp_x=0, tmp_y=0, tmp_x2=0, tmp_y2=0;

                     /*
                      * looks like of silly here, but it's to fix round-off
                      *         errors
                      */
                     ReverseTransformPointThroughCTM(
                           vsel_ptr->x[i]+dx-obj_ptr->x,
                           vsel_ptr->y[i]+dy-obj_ptr->y, obj_ptr->ctm,
                           &tmp_x, &tmp_y);
                     TransformPointThroughCTM(tmp_x, tmp_y, obj_ptr->ctm,
                            &tmp_x2, &tmp_y2);
                     vsel_ptr->x[i] = tmp_x2 + obj_ptr->x;
                     vsel_ptr->y[i] = tmp_y2 + obj_ptr->y;
                  }
                  FinishMoveVertexForStretchStructSpline(vsel_ptr, dx, dy,
                        &sssi);
               } else {
                  if (obj_ptr->ctm == NULL) {
                     vlist[vsel_ptr->v_index[i]].x += dx;
                     vlist[vsel_ptr->v_index[i]].y += dy;
                     vsel_ptr->x[i] += dx;
                     vsel_ptr->y[i] += dy;
                  } else {
                     int tmp_x=0, tmp_y=0, tmp_x2=0, tmp_y2=0;

                     ReverseTransformPointThroughCTM(
                           vsel_ptr->x[i]+dx-obj_ptr->x,
                           vsel_ptr->y[i]+dy-obj_ptr->y, obj_ptr->ctm,
                           &tmp_x, &tmp_y);
                     vlist[vsel_ptr->v_index[i]].x = tmp_x+obj_ptr->x;
                     vlist[vsel_ptr->v_index[i]].y = tmp_y+obj_ptr->y;

                     TransformPointThroughCTM(tmp_x, tmp_y, obj_ptr->ctm,
                            &tmp_x2, &tmp_y2);
                     vsel_ptr->x[i] = tmp_x2 + obj_ptr->x;
                     vsel_ptr->y[i] = tmp_y2 + obj_ptr->y;
                  }
               }
            }
         }
         drawOrigX = saved_draw_orig_x;
         drawOrigY = saved_draw_orig_y;

         if (changed) {
            AdjObjSplineVs(obj_ptr);
            UpdNonIntSplinePolyBBox(obj_ptr, n, vlist);
            RecordReplaceAnObj(obj_ptr);
         } else {
            AbortPrepareCmd(CMD_REPLACE);
         }
      }
   } else {
      struct SubCmdRec *sub_cmd=NULL;
      struct SelRec *tmp_sel_ptr=NULL;

      tmp_sel_ptr = (struct SelRec *)malloc(sizeof(struct SelRec));
      if (tmp_sel_ptr == NULL) FailAllocMessage();
      tmp_sel_ptr->next = tmp_sel_ptr->prev = NULL;

      sub_cmd = (struct SubCmdRec *)malloc(sizeof(struct SubCmdRec));
      if (sub_cmd == NULL) FailAllocMessage();
      memset(sub_cmd, 0, sizeof(struct SubCmdRec));

      for (sel_ptr = topSel; sel_ptr != NULL; sel_ptr = sel_ptr->next) {
         obj_ptr = sel_ptr->obj;
         if (obj_ptr->locked) continue;

         if ((auto_justifiable=AutoJustifiable(obj_ptr))) {
            PrepareToReplaceAnObj(obj_ptr);
            AutoJustify(obj_ptr);
         }
         switch (horiAlign) {
         case ALIGN_L: x = obj_ptr->obbox.ltx; break;
         case ALIGN_C: x = (obj_ptr->obbox.ltx+obj_ptr->obbox.rbx)/2; break;
         case ALIGN_R: x = obj_ptr->obbox.rbx; break;
         }
         switch (vertAlign) {
         case ALIGN_T: y = obj_ptr->obbox.lty; break;
         case ALIGN_M: y = (obj_ptr->obbox.lty+obj_ptr->obbox.rby)/2; break;
         case ALIGN_B: y = obj_ptr->obbox.rby; break;
         }
         saved_draw_orig_x = drawOrigX;
         saved_draw_orig_y = drawOrigY;

         drawOrigX = drawOrigY = 0;

         if (zoomedIn) {
            GridXY(ZOOMED_SIZE(x), ZOOMED_SIZE(y), &grid_x, &grid_y);
            if (horiAlign==ALIGN_N || horiAlign==ALIGN_S) x = ABS_SIZE(grid_x);
            if (vertAlign==ALIGN_N || vertAlign==ALIGN_S) y = ABS_SIZE(grid_y);
  
            dx = ABS_SIZE(grid_x) - x;
            dy = ABS_SIZE(grid_y) - y;
         } else {
            GridXY(ZOOMED_SIZE(x), ZOOMED_SIZE(y), &grid_x, &grid_y);
            if (horiAlign==ALIGN_N || horiAlign==ALIGN_S) x = grid_x;
            if (vertAlign==ALIGN_N || vertAlign==ALIGN_S) y = grid_y;

            dx = ABS_SIZE(grid_x) - x;
            dy = ABS_SIZE(grid_y) - y;
         }
         drawOrigX = saved_draw_orig_x;
         drawOrigY = saved_draw_orig_y;

         if (dx != 0 || dy != 0) {
            if (auto_justifiable) {
               MoveObj(obj_ptr, dx, dy);
               RecordReplaceAnObj(obj_ptr);
            } else {
               sub_cmd->detail.move.dx = dx;
               sub_cmd->detail.move.dy = dy;
               tmp_sel_ptr->obj = obj_ptr;
               PrepareToRecord(CMD_MOVE, tmp_sel_ptr, tmp_sel_ptr, 1);
               RecordCmd(CMD_MOVE, sub_cmd, NULL, NULL, 0);

               MoveObj(obj_ptr, dx, dy);
            }
         } else if (auto_justifiable) {
            RecordReplaceAnObj(obj_ptr);
         }
      }
      free(sub_cmd);
      free(tmp_sel_ptr);
   }
   EndCompositeCmd();

   ltx = selLtX; lty = selLtY; rbx = selRbX, rby = selRbY;
   UpdSelBBox();
   RedrawAreas(botObj, ltx-GRID_ABS_SIZE(1), lty-GRID_ABS_SIZE(1),
         rbx+GRID_ABS_SIZE(1), rby+GRID_ABS_SIZE(1),
         selLtX-GRID_ABS_SIZE(1), selLtY-GRID_ABS_SIZE(1),
         selRbX+GRID_ABS_SIZE(1), selRbY+GRID_ABS_SIZE(1));
   HighLightForward();
   SetFileModified(TRUE);
   justDupped = FALSE;
}

void HoriAlignSubMenu(index)
   int index;
{
   char *psz=NULL;

   horiAlign = index;
   psz = HoriAlignLoadString(horiAlign);
   if (psz != NULL) Msg(psz);
   ShowHoriAlign();
   UpdatePinnedMenu(MENU_HORIALIGN);
}

void RefreshHoriAlignMenu(menu)
   TgMenu *menu;
{
   int i, num_items=menu->num_items;
   TgMenuItem *menuitems=menu->menuitems;

   for (i=0; i < num_items; i++) {
      TgMenuItem *menu_item=(&menuitems[i]);
      TgMenuItem stMenuItem;

      memset(&stMenuItem, 0, sizeof(TgMenuItem));
      stMenuItem.state = TGBS_NORMAL;
      stMenuItem.checked = (i == horiAlign);
      TgSetMenuItemInfo(menu_item, TGMU_MASK_STATE|TGMU_MASK_CHECK,
            &stMenuItem);
   }
}

TgMenu *CreateHoriAlignMenu(parent_menu, x, y, menu_info, status_str_xlated)
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
      TgAdjustMenuGeometry(menu, choiceImageW, choiceImageH, MAXALIGNS);
      menu_item = (&menu->menuitems[horiAlign]);

      memset(&stMenuItem, 0, sizeof(TgMenuItem));
      stMenuItem.checked = TRUE;
      if (!TgSetMenuItemInfo(menu_item, TGMU_MASK_CHECK, &stMenuItem)) {
         return TgDestroyMenu(menu, TRUE);
      }
      menu->refresh_proc = ((RefreshMenuFunc*)RefreshHoriAlignMenu);
   }
   return menu;
}

int HoriAlignMenu(X, Y, TrackMenubar)
   int X, Y, TrackMenubar;
{
   int rc=INVALID;
   TgMenu *menu=(horiAlignMenuInfo.create_proc)(NULL, X, Y, &horiAlignMenuInfo,
         INVALID);

   activeMenu = MENU_HORIALIGN;
   if (menu != NULL) {
      menu->track_menubar = TrackMenubar;

      rc = TgMenuLoop(menu);
      TgDestroyMenu(menu, TRUE);
   }
   return rc;
}

void VertAlignSubMenu(index)
   int index;
{
   char *psz=NULL;

   vertAlign = index;
   psz = VertAlignLoadString(vertAlign);
   if (psz != NULL) Msg(psz);
   ShowVertAlign();
   UpdatePinnedMenu(MENU_VERTALIGN);
}

void RefreshVertAlignMenu(menu)
   TgMenu *menu;
{
   int i, num_items=menu->num_items;
   TgMenuItem *menuitems=menu->menuitems;

   for (i=0; i < num_items; i++) {
      TgMenuItem *menu_item=(&menuitems[i]);
      TgMenuItem stMenuItem;

      memset(&stMenuItem, 0, sizeof(TgMenuItem));
      stMenuItem.state = TGBS_NORMAL;
      stMenuItem.checked = (i == vertAlign);
      TgSetMenuItemInfo(menu_item, TGMU_MASK_STATE|TGMU_MASK_CHECK,
            &stMenuItem);
   }
}

TgMenu *CreateVertAlignMenu(parent_menu, x, y, menu_info, status_str_xlated)
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
      TgAdjustMenuGeometry(menu, choiceImageW, choiceImageH, MAXALIGNS);
      menu_item = (&menu->menuitems[vertAlign]);

      memset(&stMenuItem, 0, sizeof(TgMenuItem));
      stMenuItem.checked = TRUE;
      if (!TgSetMenuItemInfo(menu_item, TGMU_MASK_CHECK, &stMenuItem)) {
         return TgDestroyMenu(menu, TRUE);
      }
      menu->refresh_proc = ((RefreshMenuFunc*)RefreshVertAlignMenu);
   }
   return menu;
}

int VertAlignMenu(X, Y, TrackMenubar)
   int X, Y, TrackMenubar;
{
   int rc=INVALID;
   TgMenu *menu=(vertAlignMenuInfo.create_proc)(NULL, X, Y, &vertAlignMenuInfo,
         INVALID);

   activeMenu = MENU_VERTALIGN;
   if (menu != NULL) {
      menu->track_menubar = TrackMenubar;

      rc = TgMenuLoop(menu);
      TgDestroyMenu(menu, TRUE);
   }
   return rc;
}

void AlignDirect(alignment)
   int alignment;
{
   int saved_h_align=horiAlign, saved_v_align=vertAlign;
   int v_align=(alignment%(MAXALIGNS-1));
   int h_align=((alignment-v_align)/(MAXALIGNS-1));

   horiAlign = h_align;
   vertAlign = v_align;
   switch (alignDirectType) {
   case ALIGN_OBJS_DIRECT: AlignSelObjs(); break;
   case ALIGN_TO_GRID_DIRECT: AlignSelToGrid(); break;
   case ALIGN_TO_PAGE_DIRECT: AlignSelToPage(); break;
   }
   horiAlign = saved_h_align;
   vertAlign = saved_v_align;
}

static
char *GetAlignDirectMenuStr(i)
   int i;
   /* returned string has been translated */
{
   char *psz=NULL, *msg=NULL;

   if (curChoice == VERTEXMODE) {
      if (alignDirectType == ALIGN_TO_PAGE_DIRECT) {
         if (i == 0) {
            msg = TgLoadCachedString(CSTID_PARANED_NONE);
         } else {
            msg = AlignVerticesDirectToPageLoadString(i);
         }
      } else {
         int v_align=(i%(MAXALIGNS-1));
         int h_align=((i-v_align)/(MAXALIGNS-1));

         if (v_align == ALIGN_N) {
            if (h_align == ALIGN_N) {
               msg = TgLoadCachedString(CSTID_PARANED_NONE);
            } else {
               msg = (alignDirectType==ALIGN_TO_GRID_DIRECT ?
                     TgLoadString(STID_ALIGN_VERTICES_HORI_TO_GRID) :
                     TgLoadString(STID_ALIGN_VERTICES_VERT));
            }
         } else {
            if (h_align == ALIGN_N) {
               msg = (alignDirectType==ALIGN_TO_GRID_DIRECT ?
                     TgLoadString(STID_ALIGN_VERTICES_VERT_TO_GRID) :
                     TgLoadString(STID_ALIGN_VERTICES_HORI));
            } else {
               msg = (alignDirectType==ALIGN_TO_GRID_DIRECT ?
                     TgLoadString(STID_ALIGN_VERTICES_TO_GRID) :
                     TgLoadString(STID_ALIGN_VERTICES));
            }
         }
      }
   } else {
      if (alignDirectType == ALIGN_TO_PAGE_DIRECT) {
         if (i == 0) {
            msg = TgLoadCachedString(CSTID_PARANED_NONE);
         } else {
            msg = AlignDirectToPageLoadString(i);
         }
      } else {
         if (i == 0) {
            msg = TgLoadCachedString(CSTID_PARANED_NONE);
         } else if (alignDirectType == ALIGN_TO_GRID_DIRECT) {
            msg = AlignDirectToGridLoadString(i);
         } else {
            msg = AlignDirectLoadString(i);
         }
      }
   }
   psz = UtilStrDup(msg);
   if (psz == NULL) FailAllocMessage();
   return psz;
}

static
TgMenu *CreateAlignDirectMenu(parent_menu, x, y, menu_info)
   TgMenu *parent_menu;
   int x, y;
   TgMenuInfo *menu_info;
{
   int i=0, num_entries=((MAXALIGNS-1)*(MAXALIGNS-1));
   TgMenu *menu=NULL;
   TgMenuInfo stMenuInfo;
   TgMenuItemInfo *item_info=NULL;

   memcpy(&stMenuInfo, menu_info, sizeof(TgMenuInfo));
   stMenuInfo.items = (TgMenuItemInfo*)malloc(
         (num_entries+1)*sizeof(TgMenuItemInfo));
   if (stMenuInfo.items == NULL) FailAllocMessage();
   memset(stMenuInfo.items, 0, (num_entries+1)*sizeof(TgMenuItemInfo));
   for (item_info=stMenuInfo.items, i=0; i < num_entries; item_info++, i++) {
      item_info->menu_str = (char*)(Pixmap*)(&alignDirectPixmap[i]);
      item_info->shortcut_str = NULL;
      item_info->status_str = GetAlignDirectMenuStr(i);
      item_info->submenu_info = NULL;
      item_info->cmdid = CMDID_ALIGNDIRECT;
   }
   stMenuInfo.items[num_entries].cmdid = INVALID;

   /* the status_str has been translated in GetAlignDirectMenuStr() */
   menu = TgCreateMenuFromMenuInfo(parent_menu, x, y, &stMenuInfo, TRUE);
   for (item_info=stMenuInfo.items, i=0; i < num_entries; item_info++, i++) {
      UtilFree(item_info->status_str);
   }
   memset(stMenuInfo.items, 0, (num_entries+1)*sizeof(TgMenuItemInfo));
   free(stMenuInfo.items);
   stMenuInfo.items = NULL;
   if (menu != NULL) {
      menu->track_menubar = TRUE;
      TgAdjustMenuGeometry(menu, choiceImageW, choiceImageH, (MAXALIGNS-1));
   }
   return menu;
}

TgMenu *CreateAlignObjsDirectMenu(parent_menu, x, y, menu_info,
      status_str_xlated)
   TgMenu *parent_menu;
   int x, y;
   TgMenuInfo *menu_info;
   int status_str_xlated; /* ignored, always 0 */
{
   alignDirectType = ALIGN_OBJS_DIRECT;
   return CreateAlignDirectMenu(parent_menu, x, y, menu_info);
}

TgMenu *CreateAlignToGridDirectMenu(parent_menu, x, y, menu_info,
      status_str_xlated)
   TgMenu *parent_menu;
   int x, y;
   TgMenuInfo *menu_info;
   int status_str_xlated; /* ignored, always 0 */
{
   alignDirectType = ALIGN_TO_GRID_DIRECT;
   return CreateAlignDirectMenu(parent_menu, x, y, menu_info);
}

TgMenu *CreateAlignToPageDirectMenu(parent_menu, x, y, menu_info,
      status_str_xlated)
   TgMenu *parent_menu;
   int x, y;
   TgMenuInfo *menu_info;
   int status_str_xlated; /* ignored, always 0 */
{
   alignDirectType = ALIGN_TO_PAGE_DIRECT;
   return CreateAlignDirectMenu(parent_menu, x, y, menu_info);
}

void CenterAnEndPoint()
{
   struct ObjRec *other_obj=NULL, *poly_obj=NULL;
   IntPoint *v;
   int cx, cy, x1, y1, xn, yn, d1, dn, n=0, index, x, y, tmp_x, tmp_y;

   if (curChoice != NOTHING) return;
   if (topSel == NULL) {
      MsgBox(TgLoadCachedString(CSTID_NO_OBJ_SELECTED), TOOL_NAME, INFO_MB);
      return;
   }
   if (numObjSelected != 2 ||
         (!((topSel->obj->type == OBJ_POLY && botSel->obj->type != OBJ_POLY) ||
         (topSel->obj->type != OBJ_POLY && botSel->obj->type == OBJ_POLY)))) {
      MsgBox(TgLoadString(STID_SELECT_ONE_POLY_ONE_NON_POLY), TOOL_NAME,
            INFO_MB);
      return;
   }
   if (topSel->obj->type == OBJ_POLY) {
      poly_obj = topSel->obj;
      other_obj = botSel->obj;
   } else {
      poly_obj = botSel->obj;
      other_obj = topSel->obj;
   }
   if (poly_obj->locked) {
      MsgBox(TgLoadString(STID_CANNOT_MOVE_A_VERTEX_LOCKED), TOOL_NAME,
            INFO_MB);
      return;
   }
   cx = (other_obj->obbox.ltx+other_obj->obbox.rbx)>>1;
   cy = (other_obj->obbox.lty+other_obj->obbox.rby)>>1;
   n = poly_obj->detail.p->n;
   v = poly_obj->detail.p->vlist;
   if (poly_obj->ctm == NULL) {
      x1 = v[0].x;
      y1 = v[0].y;
      xn = v[n-1].x;
      yn = v[n-1].y;
   } else {
      TransformPointThroughCTM(v[0].x-poly_obj->x, v[0].y-poly_obj->y,
            poly_obj->ctm, &tmp_x, &tmp_y);
      x1 = tmp_x+poly_obj->x;
      y1 = tmp_y+poly_obj->y;
      TransformPointThroughCTM(v[n-1].x-poly_obj->x,
            v[n-1].y-poly_obj->y, poly_obj->ctm, &tmp_x, &tmp_y);
      xn = tmp_x+poly_obj->x;
      yn = tmp_y+poly_obj->y;
   }
   d1 = (x1-cx)*(x1-cx)+(y1-cy)*(y1-cy);
   dn = (xn-cx)*(xn-cx)+(yn-cy)*(yn-cy);
   if (d1 <= dn) {
      index = 0;
      x = x1;
      y = y1;
   } else {
      index = n-1;
      x = xn;
      y = yn;
   }
   if (cx != x || cy != y) {
      int ltx=0, lty=0, rbx=0, rby=0;

      HighLightReverse();
      PrepareToReplaceAnObj(poly_obj);
      if (poly_obj->ctm == NULL) {
         v[index].x = cx;
         v[index].y = cy;
      } else {
         ReverseTransformPointThroughCTM(cx-poly_obj->x, cy-poly_obj->y,
               poly_obj->ctm, &tmp_x, &tmp_y);
         v[index].x = tmp_x+poly_obj->x;
         v[index].y = tmp_y+poly_obj->y;
      }
      AdjObjSplineVs(poly_obj);
      switch (poly_obj->detail.p->curved) {
      case LT_STRAIGHT:
      case LT_SPLINE:
      case LT_STRUCT_SPLINE:
         UpdPolyBBox(poly_obj, n, v);
         break;
      case LT_INTSPLINE:
         UpdPolyBBox(poly_obj, poly_obj->detail.p->intn,
               poly_obj->detail.p->intvlist);
         break;
      }
      RecordReplaceAnObj(poly_obj);
      ltx = selLtX; lty = selLtY; rbx = selRbX; rby = selRbY;
      UpdSelBBox();
      RedrawAreas(botObj, ltx-GRID_ABS_SIZE(1), lty-GRID_ABS_SIZE(1),
            rbx+GRID_ABS_SIZE(1), rby+GRID_ABS_SIZE(1),
            selLtX-GRID_ABS_SIZE(1), selLtY-GRID_ABS_SIZE(1),
            selRbX+GRID_ABS_SIZE(1), selRbY+GRID_ABS_SIZE(1));
      HighLightForward();
      SetFileModified(TRUE);
      justDupped = FALSE;
   }
}
