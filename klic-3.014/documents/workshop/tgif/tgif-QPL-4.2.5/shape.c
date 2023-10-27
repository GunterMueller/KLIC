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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/shape.c,v 1.8 2011/05/16 16:21:59 william Exp $
 */

#define _INCLUDE_FROM_SHAPE_C_

#include "tgifdefs.h"
#include "cmdids.h"

#include "arc.e"
#include "attr.e"
#include "auxtext.e"
#include "box.e"
#include "cmd.e"
#include "color.e"
#include "cursor.e"
#include "dialog.e"
#include "drawing.e"
#include "dup.e"
#include "grid.e"
#include "group.e"
#include "mainloop.e"
#include "menu.e"
#include "menuinfo.e"
#include "move.e"
#include "msg.e"
#include "navigate.e"
#include "obj.e"
#include "oval.e"
#include "page.e"
#include "pattern.e"
#include "poly.e"
#include "polygon.e"
#include "raster.e"
#include "ruler.e"
#include "select.e"
#include "setup.e"
#include "shape.e"
#include "spline.e"
#include "strtbl.e"
#include "text.e"
#include "util.e"

int numShapes=MAXSHAPES;
int curShape=(-1);
int shapeShadowInResource=FALSE;
int shapeShadowDx=0;
int shapeShadowDy=0;

/* ----------------------- String Functions ----------------------- */

static struct MouseStatusStrRec shapeDescMouseStatus[] = {
   /*
    * note: the m field is used for the localized strings
    */
   { N_("Create a box"),                NULL, NULL },
   { N_("Create a parallelegram"),      NULL, NULL },
   { N_("Create a trapezoid"),          NULL, NULL },
   { N_("Create a rhombus"),            NULL, NULL },
   { N_("Create a rounded-corner box"), NULL, NULL },
   { N_("Create an octagon"),           NULL, NULL },
   { N_("Create a cross"),              NULL, NULL },
   { N_("Create an oval"),              NULL, NULL },
   { N_("Create a word box"),           NULL, NULL },
   { N_("Create a hexagon"),            NULL, NULL },
   { N_("Create a triangle"),           NULL, NULL },
   { N_("Create a blast box"),          NULL, NULL },
   { N_("Create a star"),               NULL, NULL },
   { N_("Create a disk"),               NULL, NULL },
   { N_("Create a right arrow"),        NULL, NULL },
   { N_("Create a up arrow"),           NULL, NULL },
   { N_("Create a fat right arrow"),    NULL, NULL },
   { N_("Create a fat up arrow"),       NULL, NULL },
   { N_("Create a right tab"),          NULL, NULL },
   { N_("Create a up tab"),             NULL, NULL },
   { NULL, NULL, NULL }
};

static
void CleanUpShapeDescMouseStatusStrings()
{
   int index=0;

   for (index=0; index < MAXCHOICES; index++) {
      UtilFree(shapeDescMouseStatus[index].m);
      shapeDescMouseStatus[index].m = NULL;
   }
}

static
char *GetShapeDesc(index)
   int index;
   /* returned string has been translated */
{
   if (shapeDescMouseStatus[index].m == NULL) {
      shapeDescMouseStatus[index].m =
            UtilStrDup(_(shapeDescMouseStatus[index].l));
      if (shapeDescMouseStatus[index].m == NULL) FailAllocMessage();
   }
   return shapeDescMouseStatus[index].m;
}

/* ----------------------- Create Shape Functions ----------------------- */

static IntPoint *gpVertices=NULL;
static int gnNumVs=0;

static XPoint *gpRubberVertices=NULL;
static int gnNumSplineVs=0;

static char *gpnSmooth=NULL;

static int gnOrigX=0, gnOrigY=0, gnEndX=0, gnEndY=0;
static struct BBRec gShapeBBox;

static
int CreatePolygonShape(num_pts, has_smooth)
   int num_pts, has_smooth;
{
   gpVertices = (IntPoint*)malloc(num_pts*sizeof(IntPoint));
   if (gpVertices == NULL) return FailAllocMessage();
   memset(gpVertices, 0, num_pts*sizeof(IntPoint));
   if (has_smooth) {
      gpnSmooth = (char*)malloc(num_pts*sizeof(char));
      if (gpnSmooth == NULL) {
         free(gpVertices);
         gpVertices = NULL;
         return FailAllocMessage();
      }
      memset(gpnSmooth, 0, num_pts*sizeof(char));
   }
   gnNumVs = num_pts;
   return TRUE;
}

static
int CreateOvalShape()
{
   gpVertices = NULL;
   gpnSmooth = NULL;
   gnNumVs = gnNumSplineVs = 0;

   memset(&gShapeBBox, 0, sizeof(struct BBRec));
   return TRUE;
}

static
int CreateDiskShape()
{
   gpVertices = NULL;
   gpnSmooth = NULL;
   gnNumVs = gnNumSplineVs = 0;

   memset(&gShapeBBox, 0, sizeof(struct BBRec));
   return TRUE;
}

static
int StartCreateShape(orig_x, orig_y)
   int orig_x, orig_y;
{
   gnOrigX = gnEndX = orig_x;
   gnOrigY = gnEndY = orig_y;

   switch (curShape) {
   case SHAPE_BOX: return CreatePolygonShape(5, FALSE);
   case SHAPE_PARALLEL: return CreatePolygonShape(5, FALSE);
   case SHAPE_TRAPEZOID: return CreatePolygonShape(5, FALSE);
   case SHAPE_RHOMBUS: return CreatePolygonShape(5, FALSE);
   case SHAPE_RCBOX: return CreatePolygonShape(13, TRUE);
   case SHAPE_OCTAGON: return CreatePolygonShape(9, FALSE);
   case SHAPE_CROSS: return CreatePolygonShape(13, FALSE);
   case SHAPE_OVAL: return CreateOvalShape();
   case SHAPE_WORDS: return CreatePolygonShape(16, TRUE);
   case SHAPE_HEXAGON: return CreatePolygonShape(7, FALSE);
   case SHAPE_TRIANGLE: return CreatePolygonShape(4, FALSE);
   case SHAPE_BLAST: return CreatePolygonShape(34, FALSE);
   case SHAPE_STAR: return CreatePolygonShape(12, FALSE);
   case SHAPE_DISK: return CreateDiskShape();
   case SHAPE_RIGHTARROW: return CreatePolygonShape(8, FALSE);
   case SHAPE_UPARROW: return CreatePolygonShape(8, FALSE);
   case SHAPE_FATRIGHTARROW: return CreatePolygonShape(8, FALSE);
   case SHAPE_FATUPARROW: return CreatePolygonShape(8, FALSE);
   case SHAPE_RIGHTTAB: return CreatePolygonShape(6, FALSE);
   case SHAPE_UPTAB: return CreatePolygonShape(6, FALSE);
   }
   return FALSE;
}

static
void EndCreateShape()
{
   if (gpVertices != NULL) free(gpVertices);
   gpVertices = NULL;
   if (gpRubberVertices != NULL) free(gpRubberVertices);
   gpRubberVertices = NULL;
   if (gpnSmooth != NULL) free(gpnSmooth);
   gpnSmooth = NULL;
   gnNumVs = gnNumSplineVs = 0;
}

/* ----------------------- HighLight Calc Functions ----------------------- */

#define SHAPE_ANGLE (((double)15.0)*M_PI/((double)180.0))

static
double TangentOfAngle()
{
   static int nInitialized=FALSE;
   static double dval=(double)0.0;

   if (nInitialized) return dval;
   nInitialized = TRUE;
   dval = ((double)sin(SHAPE_ANGLE)) / ((double)cos(SHAPE_ANGLE));
   return dval;
}

static
void CalcRegularPolygonVs(sides, vertex_at_right, start_degree, scale,
      start_index, index_step)
   int sides, vertex_at_right, start_degree, start_index, index_step;
   double scale;
   /* if (vertex_at_right), easy */
   /* if (!vertex_at_right && start_degree == 0) self-determined */
   /* if (!vertex_at_right && start_degree != 0) start_degree is the */
   /*    real start angle in degrees */
{
   int i, w=gnEndX-gnOrigX, h=gnEndY-gnOrigY, index=start_index;
   int xc=((gnEndX+gnOrigX)>>1), yc=((gnEndY+gnOrigY)>>1);
   double inc=((double)2.0)*M_PI/((double)sides), radius, rx, ry;
   double angle=(vertex_at_right ? (double)0.0 : inc/((double)2.0));

   if (vertex_at_right) {
      radius = (double)0.5;
      angle = (double)0.0;
   } else {
      angle = inc/((double)2.0);
      radius = (((sides%4)==0) ? (double)(cos(angle)/2.0) : (double)0.5);
      if (start_degree != 0) {
         angle = ((double)start_degree)*M_PI/((double)180.0);
      }
   }
   rx = radius*((double)w)*scale;
   ry = radius*((double)h)*scale;
   for (i=0; i < sides; i++, angle += inc, index += index_step) {
      double dw=rx*((double)cos(angle));
      double dh=ry*((double)sin(angle));

      gpVertices[index].x = xc + round(dw);
      gpVertices[index].y = yc + round(dh);
   }
   gpVertices[index].x = gpVertices[start_index].x;
   gpVertices[index].y = gpVertices[start_index].y;
   gnNumVs = sides+1;
}

static
void CalcBoxShapeVs()
{
   gpVertices[0].x = gnOrigX; gpVertices[0].y = gnOrigY;
   gpVertices[1].x = gnOrigX; gpVertices[1].y = gnEndY;
   gpVertices[2].x = gnEndX;  gpVertices[2].y = gnEndY;
   gpVertices[3].x = gnEndX;  gpVertices[3].y = gnOrigY;

   gpVertices[4].x = gpVertices[0].x;
   gpVertices[4].y = gpVertices[0].y;
}

static
void CalcParallelShapeVs()
{
   int h=gnEndY-gnOrigY, dx;
   double dval;

   dval = ((double)h) * TangentOfAngle();
   dx = (int)round(dval);
   gpVertices[0].x = gnOrigX+dx; gpVertices[0].y = gnOrigY;
   gpVertices[1].x = gnOrigX;    gpVertices[1].y = gnEndY;
   gpVertices[2].x = gnEndX-dx;  gpVertices[2].y = gnEndY;
   gpVertices[3].x = gnEndX;     gpVertices[3].y = gnOrigY;

   gpVertices[4].x = gpVertices[0].x;
   gpVertices[4].y = gpVertices[0].y;
}

static
void CalcTrapezoidShapeVs()
{
   int h=gnEndY-gnOrigY, dx;
   double dval;

   dval = ((double)h) * TangentOfAngle();
   dx = (int)round(dval);
   gpVertices[0].x = gnOrigX;    gpVertices[0].y = gnOrigY;
   gpVertices[1].x = gnOrigX+dx; gpVertices[1].y = gnEndY;
   gpVertices[2].x = gnEndX-dx;  gpVertices[2].y = gnEndY;
   gpVertices[3].x = gnEndX;     gpVertices[3].y = gnOrigY;

   gpVertices[4].x = gpVertices[0].x;
   gpVertices[4].y = gpVertices[0].y;
}

static
void CalcRhombusShapeVs()
{
   int cx=((gnOrigX+gnEndX)>>1), cy=((gnOrigY+gnEndY)>>1);

   gpVertices[0].x = cx;      gpVertices[0].y = gnOrigY;
   gpVertices[1].x = gnOrigX; gpVertices[1].y = cy;
   gpVertices[2].x = cx;      gpVertices[2].y = gnEndY;
   gpVertices[3].x = gnEndX;  gpVertices[3].y = cy;

   gpVertices[4].x = gpVertices[0].x;
   gpVertices[4].y = gpVertices[0].y;
}

static
void CalcRCBoxShapeVs()
{
   int w=gnEndX-gnOrigX, h=gnEndY-gnOrigY;

   if (abs(w) >= (rcbRadius<<1) && abs(h) >= (rcbRadius<<1)) {
      int dx=(w>0 ? rcbRadius : -rcbRadius), dy=(h>0 ? rcbRadius : -rcbRadius);

      gpVertices[0].x = gnOrigX;     gpVertices[0].y = gnOrigY+dy;
      gpVertices[1].x = gnOrigX;     gpVertices[1].y = gnEndY-dy;
      gpVertices[2].x = gnOrigX;     gpVertices[2].y = gnEndY;
      gpVertices[3].x = gnOrigX+dx;  gpVertices[3].y = gnEndY;
      gpVertices[4].x = gnEndX-dx;   gpVertices[4].y = gnEndY;
      gpVertices[5].x = gnEndX;      gpVertices[5].y = gnEndY;
      gpVertices[6].x = gnEndX;      gpVertices[6].y = gnEndY-dy;
      gpVertices[7].x = gnEndX;      gpVertices[7].y = gnOrigY+dy;
      gpVertices[8].x = gnEndX;      gpVertices[8].y = gnOrigY;
      gpVertices[9].x = gnEndX-dx;   gpVertices[9].y = gnOrigY;
      gpVertices[10].x = gnOrigX+dx; gpVertices[10].y = gnOrigY;
      gpVertices[11].x = gnOrigX;    gpVertices[11].y = gnOrigY;

      gpVertices[12].x = gpVertices[0].x;
      gpVertices[12].y = gpVertices[0].y;
      gnNumVs = 13;
      memset(gpnSmooth, 0, gnNumVs*sizeof(char));
      gpnSmooth[2] = gpnSmooth[5] = gpnSmooth[8] = gpnSmooth[11] = TRUE;
   } else {
      gpVertices[0].x = gnOrigX; gpVertices[0].y = gnOrigY;
      gpVertices[1].x = gnOrigX; gpVertices[1].y = gnEndY;
      gpVertices[2].x = gnEndX;  gpVertices[2].y = gnEndY;
      gpVertices[3].x = gnEndX;  gpVertices[3].y = gnOrigY;

      gpVertices[4].x = gpVertices[0].x;
      gpVertices[4].y = gpVertices[0].y;
      gnNumVs = 5;
      memset(gpnSmooth, 0, gnNumVs*sizeof(char));
   }
}

static
void CalcOctagonShapeVs()
{
   CalcRegularPolygonVs(8, FALSE, 0, (double)1.0, 0, 1);
}

static
void CalcCrossShapeVs()
{
   int w=gnEndX-gnOrigX, h=gnEndY-gnOrigY;
   int dx=(int)(w/3), dy=(int)(h/3);

   gpVertices[0].x = gnOrigX;     gpVertices[0].y = gnOrigY+dy;
   gpVertices[1].x = gnOrigX;     gpVertices[1].y = gnEndY-dy;
   gpVertices[2].x = gnOrigX+dx;  gpVertices[2].y = gnEndY-dy;
   gpVertices[3].x = gnOrigX+dx;  gpVertices[3].y = gnEndY;
   gpVertices[4].x = gnEndX-dx;   gpVertices[4].y = gnEndY;
   gpVertices[5].x = gnEndX-dx;   gpVertices[5].y = gnEndY-dy;
   gpVertices[6].x = gnEndX;      gpVertices[6].y = gnEndY-dy;
   gpVertices[7].x = gnEndX;      gpVertices[7].y = gnOrigY+dy;
   gpVertices[8].x = gnEndX-dx;   gpVertices[8].y = gnOrigY+dy;
   gpVertices[9].x = gnEndX-dx;   gpVertices[9].y = gnOrigY;
   gpVertices[10].x = gnOrigX+dx; gpVertices[10].y = gnOrigY;
   gpVertices[11].x = gnOrigX+dx; gpVertices[11].y = gnOrigY+dy;

   gpVertices[12].x = gpVertices[0].x;
   gpVertices[12].y = gpVertices[0].y;
}

static
void CalcOvalShapeVs()
{
   CalcBBox(gnOrigX, gnOrigY, gnEndX, gnEndY, &(gShapeBBox.ltx),
         &(gShapeBBox.lty), &(gShapeBBox.rbx), &(gShapeBBox.rby));
}

static
void CalcWordsShapeVs()
{
   int w=gnEndX-gnOrigX, h=gnEndY-gnOrigY;
   int min_h=(rcbRadius<<1)+rcbRadius;
   int min_w=min_h+(rcbRadius>>1);

   if (abs(w) >= min_w && abs(h) >= min_h) {
      int dx=(w>0 ? rcbRadius : -rcbRadius), dy=(h>0 ? rcbRadius : -rcbRadius);
      int dy2=(dy<<1);

      gpVertices[0].x = gnOrigX;     gpVertices[0].y = gnOrigY+dy;
      gpVertices[1].x = gnOrigX;     gpVertices[1].y = gnEndY-dy2;
      gpVertices[2].x = gnOrigX;     gpVertices[2].y = gnEndY-dy;
      gpVertices[3].x = gnOrigX+dx;  gpVertices[3].y = gnEndY-dy;
      gpVertices[4].x = gnOrigX+dx+(dx>>1);      gpVertices[4].y = gnEndY-dy;
      gpVertices[5].x = gnOrigX+dx+(dx>>1);      gpVertices[5].y = gnEndY;
      gpVertices[6].x = gnOrigX+(dx<<1)+(dx>>1); gpVertices[6].y = gnEndY-dy;
      gpVertices[7].x = gnEndX-dx;   gpVertices[7].y = gnEndY-dy;
      gpVertices[8].x = gnEndX;      gpVertices[8].y = gnEndY-dy;
      gpVertices[9].x = gnEndX;      gpVertices[9].y = gnEndY-dy2;
      gpVertices[10].x = gnEndX;     gpVertices[10].y = gnOrigY+dy;
      gpVertices[11].x = gnEndX;     gpVertices[11].y = gnOrigY;
      gpVertices[12].x = gnEndX-dx;  gpVertices[12].y = gnOrigY;
      gpVertices[13].x = gnOrigX+dx; gpVertices[13].y = gnOrigY;
      gpVertices[14].x = gnOrigX;    gpVertices[14].y = gnOrigY;

      gpVertices[15].x = gpVertices[0].x;
      gpVertices[15].y = gpVertices[0].y;
      gnNumVs = 16;
      memset(gpnSmooth, 0, gnNumVs*sizeof(char));
      gpnSmooth[2] = gpnSmooth[8] = gpnSmooth[11] = gpnSmooth[14] = TRUE;
   } else {
      gpVertices[0].x = gnOrigX; gpVertices[0].y = gnOrigY;
      gpVertices[1].x = gnOrigX; gpVertices[1].y = gnEndY;
      gpVertices[2].x = gnEndX; gpVertices[2].y = gnEndY;
      gpVertices[3].x = gnEndX; gpVertices[3].y = gnOrigY;

      gpVertices[4].x = gpVertices[0].x;
      gpVertices[4].y = gpVertices[0].y;
      gnNumVs = 5;
      memset(gpnSmooth, 0, gnNumVs*sizeof(char));
   }
}

static
void CalcHexagonShapeVs()
{
   CalcRegularPolygonVs(6, TRUE, 0, (double)1.0, 0, 1);
}

static
void CalcTriangleShapeVs()
{
   gpVertices[0].x = ((gnOrigX+gnEndX)>>1); gpVertices[0].y = gnOrigY;
   gpVertices[1].x = gnOrigX;               gpVertices[1].y = gnEndY;
   gpVertices[2].x = gnEndX;                gpVertices[2].y = gnEndY;

   gpVertices[3].x = gpVertices[0].x;
   gpVertices[3].y = gpVertices[0].y;
}

static
void CalcBlastShapeVs()
{
   CalcRegularPolygonVs(16, TRUE, 0, (double)1.0, 0, 2);
   CalcRegularPolygonVs(16, FALSE, 0, (double)0.66, 1, 2);
   gnNumVs = 33;
}

static
void CalcStarShapeVs()
{
   int i;
   IntPoint vs[5];

   CalcRegularPolygonVs(5, FALSE, -90, (double)1.0, 0, 2);
   CalcRegularPolygonVs(5, FALSE, 90, (double)0.375, 1, 2);
   for (i=0; i < 5; i++) {
      memcpy(&vs[i], &gpVertices[((i<<1)+7) % 10], sizeof(IntPoint));
   }
   for (i=0; i < 5; i++) {
      memcpy(&gpVertices[(i<<1)+1], &vs[i], sizeof(IntPoint));
   }
   gnNumVs = 11;
}

static
void CalcDiskShapeVs()
{
   CalcBBox(gnOrigX, gnOrigY, gnEndX, gnEndY, &(gShapeBBox.ltx),
         &(gShapeBBox.lty), &(gShapeBBox.rbx), &(gShapeBBox.rby));
}

static
void CalcRightArrowShapeVs()
{
   int w=gnEndX-gnOrigX, h=gnEndY-gnOrigY;
   int dx=(w>>1), dy=(h>>2);

   gpVertices[0].x = gnOrigX;    gpVertices[0].y = gnOrigY+dy;
   gpVertices[1].x = gnOrigX;    gpVertices[1].y = gnEndY-dy;
   gpVertices[2].x = gnOrigX+dx; gpVertices[2].y = gnEndY-dy;
   gpVertices[3].x = gnOrigX+dx; gpVertices[3].y = gnEndY;
   gpVertices[4].x = gnEndX;     gpVertices[4].y = gnOrigY+(h>>1);
   gpVertices[5].x = gnOrigX+dx; gpVertices[5].y = gnOrigY;
   gpVertices[6].x = gnOrigX+dx; gpVertices[6].y = gnOrigY+dy;

   gpVertices[7].x = gpVertices[0].x;
   gpVertices[7].y = gpVertices[0].y;
}

static
void CalcUpArrowShapeVs()
{
   int w=gnEndX-gnOrigX, h=gnEndY-gnOrigY;
   int dx=(w>>2), dy=(h>>1);

   gpVertices[0].x = gnOrigX;        gpVertices[0].y = gnEndY-dy;
   gpVertices[1].x = gnOrigX+dx;     gpVertices[1].y = gnEndY-dy;
   gpVertices[2].x = gnOrigX+dx;     gpVertices[2].y = gnEndY;
   gpVertices[3].x = gnEndX-dx;      gpVertices[3].y = gnEndY;
   gpVertices[4].x = gnEndX-dx;      gpVertices[4].y = gnEndY-dy;
   gpVertices[5].x = gnEndX;         gpVertices[5].y = gnEndY-dy;
   gpVertices[6].x = gnOrigX+(w>>1); gpVertices[6].y = gnOrigY;

   gpVertices[7].x = gpVertices[0].x;
   gpVertices[7].y = gpVertices[0].y;
}

static
void CalcFatRightArrowShapeVs()
{
   int w=gnEndX-gnOrigX, h=gnEndY-gnOrigY;
   int dx=(w>>1), dy=(h>>3);

   gpVertices[0].x = gnOrigX;    gpVertices[0].y = gnOrigY+dy;
   gpVertices[1].x = gnOrigX;    gpVertices[1].y = gnEndY-dy;
   gpVertices[2].x = gnOrigX+dx; gpVertices[2].y = gnEndY-dy;
   gpVertices[3].x = gnOrigX+dx; gpVertices[3].y = gnEndY;
   gpVertices[4].x = gnEndX;     gpVertices[4].y = gnOrigY+(h>>1);
   gpVertices[5].x = gnOrigX+dx; gpVertices[5].y = gnOrigY;
   gpVertices[6].x = gnOrigX+dx; gpVertices[6].y = gnOrigY+dy;

   gpVertices[7].x = gpVertices[0].x;
   gpVertices[7].y = gpVertices[0].y;
}

static
void CalcFatUpArrowShapeVs()
{
   int w=gnEndX-gnOrigX, h=gnEndY-gnOrigY;
   int dx=(w>>3), dy=(h>>1);

   gpVertices[0].x = gnOrigX;        gpVertices[0].y = gnEndY-dy;
   gpVertices[1].x = gnOrigX+dx;     gpVertices[1].y = gnEndY-dy;
   gpVertices[2].x = gnOrigX+dx;     gpVertices[2].y = gnEndY;
   gpVertices[3].x = gnEndX-dx;      gpVertices[3].y = gnEndY;
   gpVertices[4].x = gnEndX-dx;      gpVertices[4].y = gnEndY-dy;
   gpVertices[5].x = gnEndX;         gpVertices[5].y = gnEndY-dy;
   gpVertices[6].x = gnOrigX+(w>>1); gpVertices[6].y = gnOrigY;

   gpVertices[7].x = gpVertices[0].x;
   gpVertices[7].y = gpVertices[0].y;
}

static
void CalcRightTabShapeVs()
{
   int w=gnEndX-gnOrigX, h=gnEndY-gnOrigY;
   int dx=(int)(w/3), dy=(h>>1);

   gpVertices[0].x = gnOrigX;   gpVertices[0].y = gnOrigY;
   gpVertices[1].x = gnOrigX;   gpVertices[1].y = gnEndY;
   gpVertices[2].x = gnEndX-dx; gpVertices[2].y = gnEndY;
   gpVertices[3].x = gnEndX;    gpVertices[3].y = gnOrigY+dy;
   gpVertices[4].x = gnEndX-dx; gpVertices[4].y = gnOrigY;

   gpVertices[5].x = gpVertices[0].x;
   gpVertices[5].y = gpVertices[0].y;
}

static
void CalcUpTabShapeVs()
{
   int w=gnEndX-gnOrigX, h=gnEndY-gnOrigY;
   int dx=(w>>1), dy=(int)(h/3);

   gpVertices[0].x = gnOrigX;    gpVertices[0].y = gnOrigY+dy;
   gpVertices[1].x = gnOrigX;    gpVertices[1].y = gnEndY;
   gpVertices[2].x = gnEndX;     gpVertices[2].y = gnEndY;
   gpVertices[3].x = gnEndX;     gpVertices[3].y = gnOrigY+dy;
   gpVertices[4].x = gnOrigX+dx; gpVertices[4].y = gnOrigY;

   gpVertices[5].x = gpVertices[0].x;
   gpVertices[5].y = gpVertices[0].y;
}

/* ----------------------- HighLight Shape Functions ----------------------- */

static
int CalcCreateShapeVs(end_x, end_y)
   int end_x, end_y;
{
   gnEndX = end_x;
   gnEndY = end_y;
   if (gnEndX == gnOrigX || gnEndY == gnOrigY) return FALSE;

   switch (curShape) {
   case SHAPE_BOX: CalcBoxShapeVs(); break;
   case SHAPE_PARALLEL: CalcParallelShapeVs(); break;
   case SHAPE_TRAPEZOID: CalcTrapezoidShapeVs(); break;
   case SHAPE_RHOMBUS: CalcRhombusShapeVs(); break;
   case SHAPE_RCBOX: CalcRCBoxShapeVs(); break;
   case SHAPE_OCTAGON: CalcOctagonShapeVs(); break;
   case SHAPE_CROSS: CalcCrossShapeVs(); break;
   case SHAPE_OVAL: CalcOvalShapeVs(); break;
   case SHAPE_WORDS: CalcWordsShapeVs(); break;
   case SHAPE_HEXAGON: CalcHexagonShapeVs(); break;
   case SHAPE_TRIANGLE: CalcTriangleShapeVs(); break;
   case SHAPE_BLAST: CalcBlastShapeVs(); break;
   case SHAPE_STAR: CalcStarShapeVs(); break;
   case SHAPE_DISK: CalcDiskShapeVs(); break;
   case SHAPE_RIGHTARROW: CalcRightArrowShapeVs(); break;
   case SHAPE_UPARROW: CalcUpArrowShapeVs(); break;
   case SHAPE_FATRIGHTARROW: CalcFatRightArrowShapeVs(); break;
   case SHAPE_FATUPARROW: CalcFatUpArrowShapeVs(); break;
   case SHAPE_RIGHTTAB: CalcRightTabShapeVs(); break;
   case SHAPE_UPTAB: CalcUpTabShapeVs(); break;
   }
   return TRUE;
}

#define NO_GENERATE 0
#define GENERATE 1

static
void HighLightCreateShape(end_x, end_y, generate, pn_need_to_draw)
   int end_x, end_y, generate, *pn_need_to_draw;
{
   if (generate) {
      *pn_need_to_draw = CalcCreateShapeVs(end_x, end_y);

      if (*pn_need_to_draw && gpVertices != NULL) {
         int saved_draw_orig_x=drawOrigX, saved_draw_orig_y=drawOrigY;
         int saved_zoomed_in=zoomedIn, saved_zoom_scale=zoomScale;

         if (gpRubberVertices != NULL) free(gpRubberVertices);
         gnNumSplineVs = 0;
         drawOrigX = drawOrigY = 0;
         zoomedIn = FALSE;
         zoomScale = 0;

         if (gpnSmooth == NULL) {
            gpRubberVertices = MakePolygonVertex(drawOrigX, drawOrigY,
                  gnNumVs, gpVertices);
            gnNumSplineVs = gnNumVs;
         } else {
            gpRubberVertices = MakeMultiSplinePolygonVertex(LT_SPLINE,
                  &gnNumSplineVs, gpnSmooth, drawOrigX, drawOrigY, gnNumVs,
                  gpVertices);
         }
         if (gpRubberVertices == NULL) FailAllocMessage();
         drawOrigX = saved_draw_orig_x;
         drawOrigY = saved_draw_orig_y;
         zoomedIn = saved_zoomed_in;
         zoomScale = saved_zoom_scale;
      }
   }
   if (*pn_need_to_draw) {
      int w, h, diam_y, radius_y, side_h;

      switch (curShape) {
      case SHAPE_OVAL:
         XDrawArc(mainDisplay, drawWindow, revDefaultGC,
               gShapeBBox.ltx, gShapeBBox.lty,
               gShapeBBox.rbx-gShapeBBox.ltx, gShapeBBox.rby-gShapeBBox.lty,
               0, 360<<6);
         break;
      case SHAPE_DISK:
         w = gShapeBBox.rbx-gShapeBBox.ltx;
         h = gShapeBBox.rby-gShapeBBox.lty;
         radius_y = (int)(h/6);
         diam_y = (radius_y<<1);
         side_h = h-diam_y;
         XDrawArc(mainDisplay, drawWindow, revDefaultGC,
               gShapeBBox.ltx, gShapeBBox.lty, w, diam_y, 0, 360<<6);
         XDrawArc(mainDisplay, drawWindow, revDefaultGC,
               gShapeBBox.ltx, gShapeBBox.rby-diam_y, w, diam_y,
               180<<6, 180<<6);
         XDrawLine(mainDisplay, drawWindow, revDefaultGC,
               gShapeBBox.ltx, gShapeBBox.lty+radius_y,
               gShapeBBox.ltx, gShapeBBox.rby-radius_y);
         XDrawLine(mainDisplay, drawWindow, revDefaultGC,
               gShapeBBox.rbx, gShapeBBox.lty+radius_y,
               gShapeBBox.rbx, gShapeBBox.rby-radius_y);
         break;
      default:
         if (gpVertices != NULL) {
            XDrawLines(mainDisplay, drawWindow, revDefaultGC,
                  gpRubberVertices, gnNumSplineVs, CoordModeOrigin);
         }
         break;
      }
   }
}

/* ----------------------- Generate Object Functions ----------------------- */

static
struct AttrRec *GenerateInvisibleBox()
{
   int w, h, saved_text_just=textJust, attr_w, attr_h, attr_ltx, attr_lty;
   int saved_fill=objFill, saved_trans_pat=transPat;
   struct BBRec bbox;
   struct BoxRec *box_ptr;
   struct AttrRec *attr_ptr, *label_attr=NULL;

   textJust = JUST_C;
   memcpy(&bbox, &gShapeBBox, sizeof(struct BBRec));
   w = bbox.rbx-bbox.ltx;
   h = bbox.rby-bbox.lty;

   switch (curShape) {
   case SHAPE_WORDS:
      if (gnEndY >= gnOrigY) {
         bbox.rby -= rcbRadius;
      } else {
         bbox.lty += rcbRadius;
      }
      break;
   case SHAPE_TRIANGLE:
      if (gnEndY >= gnOrigY) {
         bbox.lty += (int)(h/3);
      } else {
         bbox.rby -= (int)(h/3);
      }
      break;
   case SHAPE_DISK:
      bbox.lty += (int)(h/3);
      break;
   case SHAPE_RIGHTARROW:
   case SHAPE_FATRIGHTARROW:
   case SHAPE_RIGHTTAB:
      if (gnEndX >= gnOrigX) {
         bbox.rbx -= (w>>2);
      } else {
         bbox.ltx += (w>>2);
      }
      break;
   case SHAPE_UPARROW:
   case SHAPE_FATUPARROW:
   case SHAPE_UPTAB:
      if (gnEndY >= gnOrigY) {
         bbox.lty += (h>>2);
      } else {
         bbox.rby -= (h>>2);
      }
      break;
   }
   if (bbox.rbx-bbox.ltx >= 12) {
      bbox.ltx += 4;
      bbox.rbx -= 4;
   }
   if (bbox.rby-bbox.lty >= 12) {
      bbox.lty += 4;
      bbox.rby -= 4;
   }
   CreateBoxObj(bbox.ltx, bbox.lty, bbox.rbx, bbox.rby, FALSE);
   box_ptr = topObj->detail.b;
   box_ptr->fill = NONEPAT;
   box_ptr->pen = NONEPAT;
   objFill = NONEPAT;
   transPat = FALSE;
   AddAttrByNameAndValue(topObj, "", "auto_center_attr");
   attr_ptr = topObj->fattr;
   if (attr_ptr != NULL) {
      attr_ptr->shown = FALSE;
      UpdAttr(attr_ptr);
      AdjObjBBox(attr_ptr->obj);
      attr_w = attr_ptr->obj->obbox.rbx-attr_ptr->obj->obbox.ltx;
      attr_h = attr_ptr->obj->bbox.rby-attr_ptr->obj->bbox.lty;
      attr_ltx = ((topObj->obbox.ltx+topObj->obbox.rbx)>>1)-(attr_w>>1);
      attr_lty = topObj->bbox.lty;
      MoveObj(attr_ptr->obj, attr_ltx-attr_ptr->obj->obbox.ltx,
            attr_lty-attr_ptr->obj->obbox.lty);
   }
   AddAttrByNameAndValue(topObj, "label=", "");
   transPat = saved_trans_pat;
   objFill = saved_fill;
   label_attr = FindAttrWithName(topObj, "label=", NULL);
   if (label_attr != NULL) {
      label_attr->nameshown = FALSE;
      label_attr->shown = TRUE;
      UpdAttr(label_attr);
      AdjObjBBox(label_attr->obj);
      attr_w = label_attr->obj->obbox.rbx-label_attr->obj->obbox.ltx;
      attr_h = label_attr->obj->bbox.rby-label_attr->obj->bbox.lty;
      attr_ltx = ((topObj->obbox.ltx+topObj->obbox.rbx)>>1)-(attr_w>>1);
      attr_lty = ((topObj->obbox.lty+topObj->obbox.rby)>>1)-(attr_h>>1);
      MoveObj(label_attr->obj, attr_ltx-label_attr->obj->obbox.ltx,
            attr_lty-label_attr->obj->obbox.lty);
   }
   AdjObjBBox(topObj);
   textJust = saved_text_just;
   return label_attr;
}

static
void GenerateShadow()
{
   struct ObjRec *obj_ptr;
   struct SelRec *tmp_top_sel=NULL, *tmp_bot_sel=NULL;

   SelAllObj(FALSE, FALSE);

   JustDupSelObj(&tmp_top_sel, &tmp_bot_sel);
   /*
    * At this point, the objects pointed by tmp_top_sel and
    *      tmp_bot_sel are dangling.
    */
   if (tmp_top_sel != tmp_bot_sel) {
      CreateGroupObj(tmp_top_sel->obj, tmp_bot_sel->obj);
   } else {
      obj_ptr = tmp_top_sel->obj;
      obj_ptr->next = topObj;
      obj_ptr->prev = NULL;
      if (topObj == NULL) {
         botObj = obj_ptr;
      } else {
         topObj->prev = obj_ptr;
      }
      topObj = obj_ptr;
   }
   RemoveAllSel();
   UpdSelBBox();

   /* move the new object to the bottom */
   obj_ptr = topObj;
   UnlinkObj(topObj);
   obj_ptr->next = NULL;
   obj_ptr->prev = botObj;
   if (botObj == NULL) {
      topObj = obj_ptr;
   } else {
      botObj->next = obj_ptr;
   }
   botObj = obj_ptr;

   ChangeObjTransPat(obj_ptr, NO_TRANSPAT_MODE);
   ChangeObjFill(obj_ptr, SOLIDPAT);
   ChangeObjPen(obj_ptr, SOLIDPAT);
   MoveObj(obj_ptr, shapeShadowDx, shapeShadowDy);
}

static
struct AttrRec *GenerateShape()
{
   struct ObjRec *saved_top_obj, *saved_bot_obj, *tmp_top_obj, *tmp_bot_obj;
   int w, h, diam_y, radius_y, side_h, saved_cur_spline, saved_line_style;
   struct AttrRec *label_attr=NULL;
   struct BBRec bbox;

   saved_top_obj = topObj;
   saved_bot_obj = botObj;
   curPage->top = curPage->bot = topObj = botObj = NULL;

   switch (curShape) {
   case SHAPE_OVAL:
      CreateOvalObj(&gShapeBBox, FALSE);
      break;
   case SHAPE_DISK:
      saved_cur_spline = curSpline;
      saved_line_style = lineStyle;

      curSpline = LT_STRAIGHT;
      lineStyle = LS_PLAIN;
      w = gShapeBBox.rbx-gShapeBBox.ltx;
      h = gShapeBBox.rby-gShapeBBox.lty;
      radius_y = (int)(h/6);
      diam_y = (radius_y<<1);
      side_h = h-diam_y;
      if (penPat != NONEPAT) {
         int saved_pen=penPat;

         penPat = NONEPAT;
         CreateBoxObj(gShapeBBox.ltx, gShapeBBox.lty+radius_y,
               gShapeBBox.rbx, gShapeBBox.rby-radius_y, FALSE);
         penPat = saved_pen;
      }
      bbox.ltx = gShapeBBox.ltx;
      bbox.lty = gShapeBBox.lty;
      bbox.rbx = bbox.ltx+w;
      bbox.rby = bbox.lty+diam_y;
      CreateOvalObj(&bbox, FALSE);
      CreateArcObj(gShapeBBox.ltx+(w>>1), gShapeBBox.rby-radius_y,
            gShapeBBox.ltx, gShapeBBox.rby-radius_y,
            gShapeBBox.ltx+w, gShapeBBox.rby-radius_y, ARC_CCW,
            gShapeBBox.ltx, gShapeBBox.rby-diam_y, w, diam_y, 180<<6, 180<<6,
            FALSE);
      ResetCreatePoly();
      AddPtToCreatePoly(gShapeBBox.ltx, gShapeBBox.lty+radius_y);
      AddPtToCreatePoly(gShapeBBox.ltx, gShapeBBox.rby-radius_y);
      CreatePolyObj(2, FALSE);
      ResetCreatePoly();
      AddPtToCreatePoly(gShapeBBox.rbx, gShapeBBox.lty+radius_y);
      AddPtToCreatePoly(gShapeBBox.rbx, gShapeBBox.rby-radius_y);
      CreatePolyObj(2, FALSE);

      curSpline = saved_cur_spline;
      lineStyle = saved_line_style;
      break;
   default:
      CalcBBox(gnOrigX, gnOrigY, gnEndX, gnEndY, &(gShapeBBox.ltx),
            &(gShapeBBox.lty), &(gShapeBBox.rbx), &(gShapeBBox.rby));
      if (gpVertices != NULL) {
         int i;

         saved_cur_spline = curSpline;
         curSpline = LT_STRAIGHT;
         ResetCreatePolygon();
         for (i=0; i < gnNumVs; i++) {
            AddPtToCreatePolygon(gpVertices[i].x, gpVertices[i].y);
         }
         CreatePolygonObj(gnNumVs, FALSE);
         if (gpnSmooth != NULL) {
            struct PolygonRec *polygon_ptr=topObj->detail.g;

            if (polygon_ptr->smooth == NULL) {
               polygon_ptr->smooth = (char*)malloc((gnNumVs+1)*sizeof(char));
               if (polygon_ptr->smooth == NULL) FailAllocMessage();
               memset(polygon_ptr->smooth, 0, (gnNumVs+1)*sizeof(char));
            }
            for (i=0; i < gnNumVs; i++) {
               polygon_ptr->smooth[i] = gpnSmooth[i];
            }
            AdjObjSplineVs(topObj);
            UpdPolyBBox(topObj, polygon_ptr->n, polygon_ptr->vlist);
            AdjObjBBox(topObj);
         }
         curSpline = saved_cur_spline;
      }
      break;
   }
   if (shapeShadowDx != 0 || shapeShadowDy != 0) {
      GenerateShadow();
   }
   label_attr = GenerateInvisibleBox();

   /*
    * Use the next two functions to set selLtX, etc. to be used
    *    in CreateGroupObj().
    */
   SelAllObj(FALSE, FALSE);
   RemoveAllSel();

   tmp_top_obj = topObj;
   tmp_bot_obj = botObj;
   curPage->top = topObj = saved_top_obj;
   curPage->bot = botObj = saved_bot_obj;
   CreateGroupObj(tmp_top_obj, tmp_bot_obj);
   return label_attr;
}

/* ----------------------- Event Loop ----------------------- */

#define START_SHOW 0
#define MOTION_SHOW 1
#define END_SHOW 2

static
void DoCursor(x, y, w, h, which)
   int x, y, w, h, which;
{
   char w_buf[80], h_buf[80], x_buf[80], y_buf[80];

   PixelToMeasurementUnit(w_buf, ABS_SIZE(abs(w)));
   PixelToMeasurementUnit(h_buf, ABS_SIZE(abs(h)));
   PixelToMeasurementUnit(x_buf, ABS_X(x));
   PixelToMeasurementUnit(y_buf, ABS_Y(y));
   sprintf(gszMsgBox, "w=%s\nh=%s\nx=%s\ny=%s", w_buf, h_buf, x_buf, y_buf);
   switch (which) {
   case START_SHOW: StartShowMeasureCursor(x, y, gszMsgBox, TRUE); break;
   case MOTION_SHOW: ShowMeasureCursor(x, y, gszMsgBox, TRUE); break;
   case END_SHOW: EndShowMeasureCursor(x, y, gszMsgBox, TRUE); break;
   }
}

static
void ContinueCreateShape(orig_x, orig_y)
   int orig_x, orig_y;
{
   int done=FALSE, something_created=FALSE, grid_x=orig_x, grid_y=orig_y;
   int need_to_draw=FALSE;
   struct AttrRec *label_attr=NULL;

   BeginIntervalRulers(orig_x, orig_y, orig_x, orig_y);
   DoCursor(orig_x, orig_y, 0, 0, START_SHOW);

   if (!debugNoPointerGrab) {
      XGrabPointer(mainDisplay, drawWindow, False,
            PointerMotionMask | ButtonReleaseMask | ButtonReleaseMask,
            GrabModeAsync, GrabModeAsync, None, handCursor, CurrentTime);
   }
   if (!StartCreateShape(orig_x, orig_y)) {
      Msg(TgLoadString(STID_CREATE_A_SHAPE_FAILED));
      return;
   }
   HighLightCreateShape(grid_x, grid_y, GENERATE, &need_to_draw);
   while (!done) {
      XEvent input, ev;

      XNextEvent(mainDisplay, &input);
      switch (input.type) {
      case Expose: ExposeEventHandler(&input, TRUE); break;
      case VisibilityNotify: ExposeEventHandler(&input, TRUE); break;
      case MotionNotify:
         HighLightCreateShape(grid_x, grid_y, NO_GENERATE, &need_to_draw);
         DoCursor(grid_x, grid_y, grid_x-orig_x, grid_y-orig_y, MOTION_SHOW);
         GridXY(input.xmotion.x, input.xmotion.y, &grid_x, &grid_y);
         if (input.xmotion.state & (ShiftMask | ControlMask)) {
            int w=grid_x-orig_x, h=grid_y-orig_y, pos_w=TRUE, pos_h=TRUE;

            if (w < 0) {
               w = (-w);
               pos_w = FALSE;
            }
            if (h < 0) {
               h = (-h);
               pos_h = FALSE;
            }
            if (w > h) {
               grid_x = (pos_w ? orig_x+h : orig_x-h);
            } else {
               grid_y = (pos_h ? orig_y+w : orig_y-w);
            }
         }
         DoCursor(grid_x, grid_y, grid_x-orig_x, grid_y-orig_y, MOTION_SHOW);
         HighLightCreateShape(grid_x, grid_y, GENERATE, &need_to_draw);
         DrawIntervalRulers(orig_x, orig_y, grid_x, grid_y, NULL);
         while (XCheckMaskEvent(mainDisplay, PointerMotionMask, &ev)) ;
         break;
      case KeyPress:
         if (KeyPressEventIsEscape(&input.xkey)) {
            done = TRUE;
         }
         break;
      case ButtonRelease:
         if (grid_x != orig_x && grid_y != orig_y) {
            something_created = TRUE;
         }
         done = TRUE;
         break;
      }
   }
   XUngrabPointer(mainDisplay, CurrentTime);
   XSync(mainDisplay, False);
   done = TRUE;
   DoCursor(grid_x, grid_y, grid_x-orig_x, grid_y-orig_y, END_SHOW);
   HighLightCreateShape(grid_x, grid_y, NO_GENERATE, &need_to_draw);
   EndIntervalRulers(grid_x, grid_y);

   if (something_created) {
      label_attr = GenerateShape();
      numRedrawBBox = 0;
      topObj->tmp_parent = NULL;
      DrawObj(drawWindow, topObj);
      RecordNewObjCmd();
      SetFileModified(TRUE);
      justDupped = FALSE;
   }
   EndCreateShape();
   if (something_created && label_attr != NULL) {
      EditTextInAttr(label_attr);
   }
}

static
void DoCreateShape()
{
   unsigned int button;
   int mouse_x=0, mouse_y=0;

   Msg(TgLoadCachedString(CSTID_DRAG_MOUSE_CREATE_SHAPE_DOTS));
   SetMouseStatus(GetShapeDesc(curShape), TgLoadCachedString(CSTID_CANCEL),
         TgLoadCachedString(CSTID_CANCEL));
   button = DrawWindowLoop(&mouse_x, &mouse_y, cornerCursor, TRUE);
   if (button == Button1) {
      int grid_x=0, grid_y=0;

      GridXY(mouse_x, mouse_y, &grid_x, &grid_y);
      ContinueCreateShape(grid_x, grid_y);
   } else {
      Msg(TgLoadCachedString(CSTID_CREATE_A_SHAPE_ABORTED));
   }
}

static
void CreateShape()
{
   int force_no_shadow=FALSE, saved_dx=shapeShadowDx, saved_dy=shapeShadowDy;

   MakeQuiescent();

   if ((objFill == NONEPAT || (objFill == BACKPAT && transPat)) &&
         (shapeShadowDx != 0 || shapeShadowDy != 0)) {
      sprintf(gszMsgBox, TgLoadString(objFill==NONEPAT ?
            STID_CREATE_SHADOW_SHAPE_NO_FILL :
            STID_CREATE_SHADOW_SHAPE_TRAN_FILL));
      switch (MsgBox(gszMsgBox, TOOL_NAME, YNC_MB)) {
      case MB_ID_YES: force_no_shadow = TRUE; break;
      case MB_ID_NO: break;
      default: return;
      }
   }
   if (force_no_shadow) {
      shapeShadowDx = shapeShadowDy = 0;
   }
   SaveStatusStrings();
   DoCreateShape();
   RestoreStatusStrings();
   if (force_no_shadow) {
      shapeShadowDx = saved_dx;
      shapeShadowDy = saved_dy;
   }
}

/* ----------------------- Init and Clean Up ----------------------- */

void CleanUpShape()
{
   CleanUpShapeDescMouseStatusStrings();
}

void InitShape()
{
   char *c_ptr;

   shapeShadowInResource = FALSE;
   shapeShadowDx = shapeShadowDy = 0;
   if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"ShapeShadowSpec")) != NULL) {
      if (ParseXYSpec(c_ptr, &shapeShadowDx, &shapeShadowDy)) {
         shapeShadowInResource = TRUE;
      }
   }
}

/* ----------------------- Menu Functions ----------------------- */

void ShapeSubMenu(nIndex)
   int nIndex;
{
   if (nIndex < 0 || nIndex >= MAXSHAPES) return;
   curShape = nIndex;
   CreateShape();
   curShape = (-1);
}

void RefreshShapeMenu(menu)
   TgMenu *menu;
{
}

TgMenu *CreateShapeMenu(parent_menu, x, y, menu_info, status_str_xlated)
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
         (MAXSHAPES+1)*sizeof(TgMenuItemInfo));
   if (stMenuInfo.items == NULL) FailAllocMessage();
   memset(stMenuInfo.items, 0, (MAXSHAPES+1)*sizeof(TgMenuItemInfo));
   for (item_info=stMenuInfo.items, i=0; i < MAXSHAPES; item_info++, i++) {
      item_info->menu_str = (char*)(Pixmap*)(&shapePixmap[i]);
      item_info->shortcut_str = NULL;
      sprintf(gszMsgBox, "%s", GetShapeDesc(i));
      item_info->status_str = UtilStrDup(gszMsgBox);
      if (item_info->status_str == NULL) FailAllocMessage();
      item_info->submenu_info = NULL;
      item_info->cmdid = CMDID_CREATESHAPE;
   }
   stMenuInfo.items[MAXSHAPES].cmdid = INVALID;

   /* the status_str has been translated in GetShapeDesc() */
   menu = TgCreateMenuFromMenuInfo(parent_menu, x, y, &stMenuInfo, TRUE);
   for (item_info=stMenuInfo.items, i=0; i < MAXSHAPES; item_info++, i++) {
      UtilFree(item_info->status_str);
   }
   memset(stMenuInfo.items, 0, (MAXSHAPES+1)*sizeof(TgMenuItemInfo));
   free(stMenuInfo.items);
   stMenuInfo.items = NULL;
   if (menu != NULL) {
      menu->track_menubar = TRUE;
      TgAdjustMenuGeometry(menu, choiceImageW, choiceImageH, 8);
   }
   return menu;
}

int ShapeMenu(X, Y, TrackMenubar)
   int X, Y, TrackMenubar;
{
   int rc=INVALID;
   TgMenu *menu=(shapeMenuInfo. create_proc)(NULL, X, Y, &shapeMenuInfo,
         INVALID);

   activeMenu = MENU_SHAPE;
   if (menu != NULL) {
      menu->track_menubar = TrackMenubar;

      rc = TgMenuLoop(menu);
      TgDestroyMenu(menu, TRUE);
   }
   return rc;
}

/* ----------------------- Public Functions ----------------------- */

void SetShapeShadow()
{
   char spec[MAXSTRING+1];

   *spec = '\0';
   sprintf(gszMsgBox, TgLoadString(STID_ENTER_XY_OFFSET_FOR_SHP_SHDW),
         shapeShadowDx, shapeShadowDy);
   Dialog(gszMsgBox, NULL, spec);
   if (ParseXYSpec(spec, &shapeShadowDx, &shapeShadowDy)) {
      sprintf(gszMsgBox, TgLoadString(STID_SHAPE_SHDW_XY_OFFSETS_SET_TO),
            shapeShadowDx, shapeShadowDy);
      Msg(gszMsgBox);
   }
}

