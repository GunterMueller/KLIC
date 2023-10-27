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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/pin.c,v 1.6 2011/05/16 16:21:58 william Exp $
 */

#define _INCLUDE_FROM_PIN_C_

#include "tgifdefs.h"

#include "auxtext.e"
#include "choice.e"
#include "color.e"
#include "dialog.e"
#include "drawing.e"
#include "mainloop.e"
#include "msg.e"
#include "navigate.e"
#include "obj.e"
#include "pin.e"
#include "poly.e"
#include "raster.e"
#include "rect.e"
#include "select.e"
#include "setup.e"
#include "strtbl.e"

int drawPolyToConnectPins=0;

struct ObjRec *gpStartPin=NULL, *gpEndPin=NULL;

static struct SelRec *topPinSel=NULL, *botPinSel=NULL;

static struct ObjRec *gpHighLightedPin=NULL;

static
void CleanUpPinInfo()
{
   struct SelRec *next_sel=NULL;

   for ( ; topPinSel != NULL; topPinSel=next_sel) {
      next_sel = topPinSel->next;
      free(topPinSel);
   }
   topPinSel = botPinSel = NULL;
   gpStartPin = gpEndPin = NULL;
}

static
void CachePin(ObjPtr)
   struct ObjRec *ObjPtr;
{
   struct SelRec *sel_ptr=(struct SelRec *)malloc(sizeof(struct SelRec));

   if (sel_ptr == NULL) FailAllocMessage();
   memset(sel_ptr, 0, sizeof(struct SelRec));
   sel_ptr->obj = ObjPtr;
   sel_ptr->next = topPinSel;
   sel_ptr->prev = NULL;
   if (topPinSel == NULL) {
      botPinSel = sel_ptr;
   } else {
      topPinSel->prev = sel_ptr;
   }
   topPinSel = sel_ptr;
}

static
void PreparePinInfoForAnObject(ObjPtr, nInsideAnIcon)
   struct ObjRec *ObjPtr;
   int nInsideAnIcon;
{
   struct ObjRec *obj_ptr=NULL;

   switch (ObjPtr->type) {
   case OBJ_GROUP:
   case OBJ_SYM:
      for (obj_ptr=ObjPtr->detail.r->last; obj_ptr != NULL;
            obj_ptr=obj_ptr->prev) {
         if (colorLayers && !ObjInVisibleLayer(obj_ptr)) {
            continue;
         }
         PreparePinInfoForAnObject(obj_ptr, nInsideAnIcon);
      }
      break;
   case OBJ_ICON:
      if (nInsideAnIcon) return;
      for (obj_ptr=ObjPtr->detail.r->last; obj_ptr != NULL;
            obj_ptr=obj_ptr->prev) {
         if (colorLayers && !ObjInVisibleLayer(obj_ptr)) {
            continue;
         }
         PreparePinInfoForAnObject(obj_ptr, TRUE);
      }
      break;
   case OBJ_PIN:
      CachePin(ObjPtr);
      break;
   default: return;
   }
}

static
void PreparePinInfo()
{
   struct ObjRec *obj_ptr=NULL;

   gpStartPin = gpEndPin = NULL;
   for (obj_ptr=botObj; obj_ptr != NULL; obj_ptr=obj_ptr->prev) {
      obj_ptr->tmp_child = NULL;
      obj_ptr->tmp_parent = NULL;
      if (colorLayers && !ObjInVisibleLayer(obj_ptr)) {
         continue;
      }
      PreparePinInfoForAnObject(obj_ptr, FALSE);
   }
}

static
int GetPins(pObj, ppTopSel, ppBotSel)
   struct ObjRec *pObj;
   struct SelRec **ppTopSel, **ppBotSel;
{
   switch (pObj->type) {
   case OBJ_GROUP: break;
   case OBJ_SYM: break;
   case OBJ_ICON: break;
   case OBJ_PIN: break;
   default: return FALSE;
   }
   topPinSel = botPinSel = NULL;
   if (pObj->type == OBJ_PIN) {
      PreparePinInfoForAnObject(pObj, FALSE);
   } else {
      struct ObjRec *pSubObj=NULL;
      int nInsideAnIcon=(pObj->type==OBJ_ICON);

      for (pSubObj=pObj->detail.r->first; pSubObj != NULL;
            pSubObj=pSubObj->next) {
         PreparePinInfoForAnObject(pSubObj, nInsideAnIcon);
      }
   }
   if (topPinSel == NULL) return FALSE;

   if (ppTopSel != NULL) *ppTopSel = topPinSel;
   if (ppBotSel != NULL) *ppBotSel = botPinSel;
   topPinSel = botPinSel = NULL;
   return TRUE;
}

static
int AddObjToSorted(pObj, ppTopSel, ppBotSel)
   struct ObjRec *pObj;
   struct SelRec **ppTopSel, **ppBotSel;
{
   int nIndex=(int)(long)(pObj->userdata);
   struct SelRec *pSel=NULL;

   for (pSel=(*ppTopSel); pSel != NULL; pSel=pSel->next) {
      if (nIndex > (int)(long)(pSel->obj->userdata)) {
         return (AddObjIntoSel(pObj, pSel->prev, pSel, ppTopSel, ppBotSel) !=
               NULL);
      }
   }
   return (AddObjIntoSel(pObj, (*ppBotSel), NULL, ppTopSel, ppBotSel) != NULL);
}

int FindExtraPoly(pTopSel, pBotSel, ppTopSel, ppBotSel, pnCount, pBBox)
   struct SelRec *pTopSel, *pBotSel;
   struct SelRec **ppTopSel, **ppBotSel;
   int *pnCount;
   struct BBRec *pBBox;
{
   struct SelRec *pSel=NULL, *first_poly_sel=NULL, *last_poly_sel=NULL;

   pBBox->ltx = pTopSel->obj->bbox.ltx;
   pBBox->lty = pTopSel->obj->bbox.lty;
   pBBox->rbx = pTopSel->obj->bbox.rbx;
   pBBox->rby = pTopSel->obj->bbox.rby;
   if (pnCount != NULL) (*pnCount) = 0;
   for (pSel=pTopSel; pSel != NULL; pSel=pSel->next) {
      struct SelRec *top_pin_sel=NULL, *bot_pin_sel=NULL;

      if (GetPins(pSel->obj, &top_pin_sel, &bot_pin_sel)) {
         struct SelRec *pin_sel_ptr=NULL;

         for (pin_sel_ptr=top_pin_sel; pin_sel_ptr != NULL;
               pin_sel_ptr=pin_sel_ptr->next) {
            struct ConnRec *conn_ptr=NULL;

            for (conn_ptr=pin_sel_ptr->obj->detail.r->first_conn;
                  conn_ptr != NULL; conn_ptr=conn_ptr->next) {
               if (FindObjInSel(conn_ptr->poly_obj,
                     first_poly_sel, last_poly_sel) == NULL) {
                  PrependObjToSel(conn_ptr->poly_obj, &first_poly_sel,
                        &last_poly_sel);
               }
            }
         }
         JustFreeSel(top_pin_sel, bot_pin_sel);
      }
   }
   if (first_poly_sel == NULL) {
      return FALSE;
   } else {
      struct SelRec *pPolySel=NULL, *pNextPolySel=NULL;
      struct SelRec *pTopSortedSel=NULL, *pBotSortedSel=NULL;

      for (pPolySel=first_poly_sel; pPolySel != NULL; pPolySel=pNextPolySel) {
         struct ObjRec *pPolyObj=pPolySel->obj;

         pNextPolySel = pPolySel->next;
         for (pSel=pTopSel; pSel != NULL; pSel=pSel->next) {
            if (pSel->obj == pPolyObj) {
               UnlinkSel(pPolySel, &first_poly_sel, &last_poly_sel);
               free(pPolySel);
               break;
            }
         }
         if (pSel == NULL) {
            struct ObjRec *pObj=NULL;
            int nIndex=0;

            for (pObj=topObj; pObj != NULL; pObj=pObj->next, nIndex++) {
               if (pObj == pPolyObj) {
                  pPolyObj->userdata = (void*)(long)nIndex;
                  if (AddObjToSorted(pPolyObj,
                        &pTopSortedSel, &pBotSortedSel)) {
                  }
                  if (pnCount != NULL) (*pnCount)++;
                  ExpandBBox(&pPolyObj->bbox, pBBox);
                  break;
               }
            }
         }
      }
      JustFreeSel(first_poly_sel, last_poly_sel);
      first_poly_sel = pTopSortedSel;
      last_poly_sel = pBotSortedSel;
   }
   if (first_poly_sel == NULL) return FALSE;

   if (ppTopSel != NULL) *ppTopSel = first_poly_sel;
   if (ppBotSel != NULL) *ppBotSel = last_poly_sel;
   return TRUE;
}

static
struct ObjRec *FindPinObj(AbsX, AbsY)
   int AbsX, AbsY;
{
   struct SelRec *sel_ptr=NULL;

   for (sel_ptr=topPinSel; sel_ptr != NULL; sel_ptr=sel_ptr->next) {
      if (PointInBBox(AbsX, AbsY, sel_ptr->obj->obbox)) {
         return sel_ptr->obj;
      }
   }
   return NULL;
}

void HighLightAPin(forward)
   int forward;
{
   if (drawPolyHighlightedNode != NULL) {
      SelBox(drawWindow, revGrayGC,
            OFFSET_X(drawPolyHighlightedNode->obbox.ltx)-2,
            OFFSET_Y(drawPolyHighlightedNode->obbox.lty)-2,
            OFFSET_X(drawPolyHighlightedNode->obbox.rbx)+2,
            OFFSET_Y(drawPolyHighlightedNode->obbox.rby)+2);
      if (forward) {
         gpHighLightedPin = drawPolyHighlightedNode;
      } else {
         gpHighLightedPin = NULL;
      }
   }
}

void HandlePinHighlights(MouseX, MouseY)
   int MouseX, MouseY;
{
   int need_to_highlight=FALSE, something_changed=FALSE;
   struct ObjRec *obj_under_cursor=NULL;

   obj_under_cursor = FindPinObj(ABS_X(MouseX), ABS_Y(MouseY));
   if (drawPolyHighlightedNode != NULL) {
      if (obj_under_cursor != drawPolyHighlightedNode) {
         /* un-highlight */
         HighLightAPin(FALSE);
         if (obj_under_cursor != NULL) {
            drawPolyHighlightedNode = obj_under_cursor;
            /* may need to call something like SetWiringNodeInfo()!? */
         } else {
            drawPolyHighlightedNode = NULL;
            /* may need to call something like SetWiringNodeInfo()!? */
         }
         if (drawPolyHighlightedNode != NULL) {
            need_to_highlight = TRUE;
         }
         something_changed = TRUE;
      }
   } else {
      if (obj_under_cursor != NULL) {
         drawPolyHighlightedNode = obj_under_cursor;
         /* may need to call something like SetWiringNodeInfo()!? */
         need_to_highlight = TRUE;
         something_changed = TRUE;
      }
   }
   if (need_to_highlight) {
      HighLightAPin(TRUE);
   }
}

static
void AppendConnToPin(pPinObj, pConn, pPolyObj)
   struct ObjRec *pPinObj, *pPolyObj;
   struct ConnRec *pConn;
{
   pPinObj->detail.r->pin_connected++;

   pConn->pin_obj = pPinObj;
   pConn->poly_obj = pPolyObj;

   pConn->prev = pPinObj->detail.r->last_conn;
   pConn->next = NULL;
   if (pPinObj->detail.r->last_conn == NULL) {
      pPinObj->detail.r->first_conn = pConn;
   } else {
      pPinObj->detail.r->last_conn->next = pConn;
   }
   pPinObj->detail.r->last_conn = pConn;
}

static
void CreateConnection(pStartPin, pEndPin)
   struct ObjRec *pStartPin, *pEndPin;
{
   struct ConnRec *pStartConn=NULL, *pEndConn=NULL;

   pStartConn = (struct ConnRec *)malloc(sizeof(struct ConnRec));
   pEndConn = (struct ConnRec *)malloc(sizeof(struct ConnRec));
   if (pStartConn == NULL || pEndConn == NULL) {
      FailAllocMessage();
      if (pStartConn != NULL) free(pStartConn);
      if (pEndConn != NULL) free(pEndConn);
      return;
   }
   memset(pStartConn, 0, sizeof(struct ConnRec));
   memset(pEndConn, 0, sizeof(struct ConnRec));

   topObj->detail.p->start_conn = pStartConn;
   topObj->detail.p->end_conn = pEndConn;

   pStartConn->at_start = TRUE;
   pEndConn->at_start = FALSE;

   AppendConnToPin(pStartPin, pStartConn, topObj);
   AppendConnToPin(pEndPin, pEndConn, topObj);
}

void ConnectPins()
{
   int already_in_hyperspace=inHyperSpace, connecting=TRUE;
   XGCValues values;

   PreparePinInfo();
   if (topPinSel == NULL) {
      MsgBox(TgLoadString(STID_NO_PINS_FOUND_IN_CUR_DRAWING), TOOL_NAME,
            INFO_MB);
      return;
   }
   MakeQuiescent();

   ResetWiringNodeInfo();
   values.line_width = 3;
   XChangeGC(mainDisplay, revGrayGC, GCLineWidth, &values);

   SetCurChoice(DRAWPOLY);
   gstWiringInfo.num_ports_to_connect = 0;

   while (connecting) {
      drawPolyToConnectPins = 2;
      drawPolyHighlightedNode = gpHighLightedPin = NULL;
      gpStartPin = gpEndPin = NULL;
      SetStringStatus(TgLoadCachedString(CSTID_SEL_A_PIN_ESC_TO_END_DOTS));
      for (;;) {
         XEvent input;

         XNextEvent(mainDisplay, &input);
         if (input.type == Expose) {
            ExposeEventHandler(&input, TRUE);
         } else if (input.type == KeyPress) {
            if (KeyPressEventIsEscape(&input.xkey)) {
               connecting = FALSE;
               break;
            }
         } else if (input.xany.window == drawWindow) {
            polyDrawn = FALSE;
            DrawingEventHandler(&input);
            if (curChoice == DRAWPOLY) {
               if (polyDrawn) {
                  break;
               } else if (drawPolyToConnectPins == (-1)) {
                  break;
               }
            } else {
               polyDrawn = FALSE;
               break;
            }
         }
      }
      drawPolyToConnectPins = 0;
      if (gpHighLightedPin != NULL) {
         drawPolyHighlightedNode = gpHighLightedPin;
         /* may need to call something like SetWiringNodeInfo()!? */
         HighLightAPin(FALSE);
      }
      drawPolyHighlightedNode = gpHighLightedPin = NULL;

      if (gpStartPin != NULL && gpEndPin != NULL) {
         CreateConnection(gpStartPin, gpEndPin);
      }
   }
   SetCurChoice(NOTHING);

   values.line_width = 1;
   XChangeGC(mainDisplay, revGrayGC, GCLineWidth, &values);
   if (already_in_hyperspace && !inHyperSpace) ToggleHyperSpace(TRUE);

   CleanUpPinInfo();
}

void FreeConn(pConn)
   struct ConnRec *pConn;
{
   free(pConn);
}

static
void UnlinkConn(pConn)
   struct ConnRec *pConn;
{
   struct GroupRec *pGroup=pConn->pin_obj->detail.r;

   if (pGroup->first_conn == pConn) {
      pGroup->first_conn = pConn->next;
   } else {
      pConn->prev->next = pConn->next;
   }
   if (pGroup->last_conn == pConn) {
      pGroup->last_conn = pConn->prev;
   } else {
      pConn->next->prev = pConn->prev;
   }
   pGroup->pin_connected--;
   pConn->pin_obj = NULL;
}

void DelConnFromPoly(pConn)
   struct ConnRec *pConn;
{
   pConn->poly_obj = NULL;
   UnlinkConn(pConn);
   FreeConn(pConn);
}

void DelConnFromPin(pConn)
   struct ConnRec *pConn;
{
   /* struct ObjRec *pPinObj=pConn->pin_obj; */
   struct ObjRec *pPolyObj=pConn->poly_obj;

   if (pConn->at_start) {
      pPolyObj->detail.p->start_conn = NULL;
   } else {
      pPolyObj->detail.p->end_conn = NULL;
   }
   UnlinkConn(pConn);
   FreeConn(pConn);

   DelObj(pPolyObj);
}

void CleanUpPins()
{
}

int InitPins()
{
   return TRUE;
}

