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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/spline.c,v 1.45 2011/05/16 16:21:59 william Exp $
 */

#define _INCLUDE_FROM_SPLINE_C_

#include "tgifdefs.h"

#include "dialog.e"
#include "file.e"
#include "msg.e"
#include "poly.e"
#include "polygon.e"
#include "ps.e"
#include "raster.e"
#include "rect.e"
#include "setup.e"
#include "spline.e"
#include "strtbl.e"

#define SUM_MINUS_2 (theSum-((double)2.0))
#define HALF ((double)0.5)

int intSplineTension=3;
int splineTol=9;
int tighterStructSplines=TRUE;

#define curveToFactor (((double)2.0)/((double)3.0))
#define compCurveToFactor (((double)1.0)/((double)3.0))

static double theSum=(double)6.0;

int ReadSplineTightness(buf)
   char *buf;
{
   int tighter=TRUE; 
   char *psz=NULL;

   if (importingFile) return TRUE;

   psz = FindChar((int)'(', buf);
   InitScan(psz, "\t\n, ");
   if (GETINT("spline_tightness", tighter, "tighter_splines") == INVALID) {
      return FALSE;
   }
   tighterStructSplines = tighter;

   return TRUE;
}

void CalcAutoRetractedArrowAttrBend(Style, X0, Y0, X2, Y2, X1, Y1)
   int Style, X0, Y0, X2, Y2, *X1, *Y1;
{
   double dx, dy, len, new_x, new_y;

   dx = (double)(X2 - X0);
   dy = (double)(Y2 - Y0);
   len = (double)(sqrt(((double)dx)*((double)dx)+((double)dy)*((double)dy)));
   if (Style == LS_RIGHT) {
      new_x = ((double)((X0+X2)>>1)) + dy/((double)8.0);
      new_y = ((double)((Y0+Y2)>>1)) - dx/((double)8.0);
   } else {
      new_x = ((double)((X0+X2)>>1)) - dy/((double)8.0);
      new_y = ((double)((Y0+Y2)>>1)) + dx/((double)8.0);
   }
   *X1 = (int)round(new_x);
   *Y1 = (int)round(new_y);
}

static
void MidPoint(X1, Y1, X2, Y2, pd_mx, pd_my)
   double X1, Y1, X2, Y2, *pd_mx, *pd_my;
{
   *pd_mx = HALF*X1+HALF*X2;
   *pd_my = HALF*Y1+HALF*Y2;
}

static
void SetAStructuredSplineTickMark(vs, num_pts, target_vs)
   IntPoint *vs, *target_vs;
   int num_pts;
{
   double x, y, x0, y0, x1, y1, x2, y2, x3, y3;
   double mx1, my1, mx2, my2, mx3, my3, mx12, my12, mx23, my23;

   switch (num_pts) {
   case 1:
      target_vs[0].x = vs[0].x;
      target_vs[0].y = vs[0].y;
      break;
   case 3:
      x0 = (double)(vs[0].x); y0 = (double)(vs[0].y);
      x1 = (double)(vs[1].x); y1 = (double)(vs[1].y);
      x2 = (double)(vs[2].x); y2 = (double)(vs[2].y);
      MidPoint(x0, y0, x1, y1, &mx1, &my1);
      MidPoint(x2, y2, x1, y1, &mx2, &my2);
      MidPoint(mx1, my1, mx2, my2, &x, &y);
      target_vs[0].x = (int)x;
      target_vs[0].y = (int)y;
      break;
   case 4:
      x0 = (double)(vs[0].x); y0 = (double)(vs[0].y);
      x1 = (double)(vs[1].x); y1 = (double)(vs[1].y);
      x2 = (double)(vs[2].x); y2 = (double)(vs[2].y);
      x3 = (double)(vs[3].x); y3 = (double)(vs[3].y);
      MidPoint(x0, y0, x1, y1, &mx1, &my1);
      MidPoint(x1, y1, x2, y2, &mx2, &my2);
      MidPoint(x2, y2, x3, y3, &mx3, &my3);
      MidPoint(mx1, my1, mx2, my2, &mx12, &my12);
      MidPoint(mx2, my2, mx3, my3, &mx23, &my23);
      MidPoint(mx12, my12, mx23, my23, &x, &y);
      target_vs[0].x = (int)x;
      target_vs[0].y = (int)y;
      break;
   }
}

IntPoint *GetStructuredSplinePolyTickMarkVs(pn_N, obj_ptr, poly_ptr,
      polygon_ptr)
   int *pn_N;
   struct ObjRec *obj_ptr;
   struct PolyRec *poly_ptr;
   struct PolygonRec *polygon_ptr;
{
   int i=0, j=0, n=0, num_hinge_vs=0, num_vs=0, index=0;
   int last_was_smooth=FALSE;
   IntPoint *vs=NULL, *vlist=NULL, tmp_vs[4], *xformed_vs=NULL;

   if (poly_ptr != NULL) {
      n = poly_ptr->n;
      vlist = poly_ptr->vlist;
   } else if (polygon_ptr != NULL) {
      n = polygon_ptr->n;
      vlist = polygon_ptr->vlist;
   }
   if (obj_ptr->ctm != NULL) {
      xformed_vs = (IntPoint*)malloc((n+1)*sizeof(IntPoint));
      if (xformed_vs == NULL) FailAllocMessage();
      memset(xformed_vs, 0, (n+1)*sizeof(IntPoint));
      for (i=0; i < n; i++) {
         int tmp_x=0, tmp_y=0;

         TransformPointThroughCTM(vlist[i].x-obj_ptr->x, vlist[i].y-obj_ptr->y,
               obj_ptr->ctm, &tmp_x, &tmp_y);
         xformed_vs[i].x = tmp_x + obj_ptr->x;
         xformed_vs[i].y = tmp_y + obj_ptr->y;
      }
   }
   num_hinge_vs = (n+2)/3;
#ifdef _TGIF_DBG /* debug, do not translate */
   TgAssert((n+2)%3 == 0,
         "invalid n in GetStructuredSplinePolyTickMarkVs()", NULL);
#endif /* _TGIF_DBG */
   num_vs = num_hinge_vs;
   for (i=0, j=0; i < num_hinge_vs; i++, j+=3) {
      if (i == 0) {
         if (vlist[0].x != vlist[1].x || vlist[0].y != vlist[1].y) {
            last_was_smooth = TRUE;
         }
      } else if (i == num_hinge_vs-1) {
         if (vlist[n-1].x != vlist[n-2].x || vlist[n-1].y != vlist[n-2].y) {
            num_vs++;
            last_was_smooth = TRUE;
         } else if (last_was_smooth) {
            num_vs++;
            last_was_smooth = FALSE;
         }
      } else {
         if (vlist[j-1].x != vlist[j].x || vlist[j-1].y != vlist[j].y) {
#ifdef _TGIF_DBG /* debug, do not translate */
            TgAssert(vlist[j+1].x != vlist[j].x || vlist[j+1].y != vlist[j].y,
                  "half smooth detected in GetStructuredSplinePolyTickMarkVs()",
                  NULL);
#endif /* _TGIF_DBG */
            num_vs++;
            last_was_smooth = TRUE;
         } else if (last_was_smooth) {
            num_vs++;
            last_was_smooth = FALSE;
         }
      }
   }
   vs = (IntPoint*)malloc((num_vs+1)*sizeof(IntPoint));
   if (vs == NULL) FailAllocMessage();
   memset(vs, 0, (num_vs+1)*sizeof(IntPoint));

   if (xformed_vs != NULL) {
      vs[0].x = xformed_vs[0].x;
      vs[0].y = xformed_vs[0].y;
   } else {
      vs[0].x = vlist[0].x;
      vs[0].y = vlist[0].y;
   }
   index = 1;
   last_was_smooth = FALSE;
   for (i=0, j=0; i < num_hinge_vs; i++, j+=3) {
      int num_pts_in_seg=0;

      if (i == 0) {
         if (xformed_vs != NULL) {
            tmp_vs[0].x = xformed_vs[0].x;
            tmp_vs[0].y = xformed_vs[0].y;
         } else {
            tmp_vs[0].x = vlist[0].x;
            tmp_vs[0].y = vlist[0].y;
         }
         if (vlist[0].x != vlist[1].x || vlist[0].y != vlist[1].y) {
            last_was_smooth = TRUE;
            if (xformed_vs != NULL) {
               tmp_vs[1].x = xformed_vs[1].x;
               tmp_vs[1].y = xformed_vs[1].y;
            } else {
               tmp_vs[1].x = vlist[1].x;
               tmp_vs[1].y = vlist[1].y;
            }
         }
      } else if (i == num_hinge_vs-1) {
         if (vlist[n-1].x != vlist[n-2].x || vlist[n-1].y != vlist[n-2].y) {
            if (last_was_smooth) {
               num_pts_in_seg = 4;
            } else {
               num_pts_in_seg = 3;
            }
            if (xformed_vs != NULL) {
               tmp_vs[num_pts_in_seg-2].x = xformed_vs[j-1].x;
               tmp_vs[num_pts_in_seg-2].y = xformed_vs[j-1].y;
               tmp_vs[num_pts_in_seg-1].x = xformed_vs[j].x;
               tmp_vs[num_pts_in_seg-1].y = xformed_vs[j].y;
            } else {
               tmp_vs[num_pts_in_seg-2].x = vlist[j-1].x;
               tmp_vs[num_pts_in_seg-2].y = vlist[j-1].y;
               tmp_vs[num_pts_in_seg-1].x = vlist[j].x;
               tmp_vs[num_pts_in_seg-1].y = vlist[j].y;
            }
            SetAStructuredSplineTickMark(tmp_vs, num_pts_in_seg, &vs[index++]);
         } else if (last_was_smooth) {
            if (xformed_vs != NULL) {
               tmp_vs[2].x = xformed_vs[j].x;
               tmp_vs[2].y = xformed_vs[j].y;
            } else {
               tmp_vs[2].x = vlist[j].x;
               tmp_vs[2].y = vlist[j].y;
            }
            SetAStructuredSplineTickMark(tmp_vs, 3, &vs[index++]);
         }
         if (poly_ptr != NULL) {
            if (xformed_vs != NULL) {
               SetAStructuredSplineTickMark(&xformed_vs[j], 1, &vs[index++]);
            } else {
               SetAStructuredSplineTickMark(&vlist[j], 1, &vs[index++]);
            }
         }
         /* all done */
      } else {
         if (vlist[j-1].x != vlist[j].x || vlist[j-1].y != vlist[j].y) {
            if (last_was_smooth) {
               num_pts_in_seg = 4;
            } else {
               num_pts_in_seg = 3;
            }
            if (xformed_vs != NULL) {
               tmp_vs[num_pts_in_seg-2].x = xformed_vs[j-1].x;
               tmp_vs[num_pts_in_seg-2].y = xformed_vs[j-1].y;
               tmp_vs[num_pts_in_seg-1].x = xformed_vs[j].x;
               tmp_vs[num_pts_in_seg-1].y = xformed_vs[j].y;
            } else {
               tmp_vs[num_pts_in_seg-2].x = vlist[j-1].x;
               tmp_vs[num_pts_in_seg-2].y = vlist[j-1].y;
               tmp_vs[num_pts_in_seg-1].x = vlist[j].x;
               tmp_vs[num_pts_in_seg-1].y = vlist[j].y;
            }
            SetAStructuredSplineTickMark(tmp_vs, num_pts_in_seg, &vs[index++]);

            last_was_smooth = TRUE;
            if (xformed_vs != NULL) {
               tmp_vs[1].x = xformed_vs[j+1].x;
               tmp_vs[1].y = xformed_vs[j+1].y;
            } else {
               tmp_vs[1].x = vlist[j+1].x;
               tmp_vs[1].y = vlist[j+1].y;
            }
         } else if (last_was_smooth) {
            if (xformed_vs != NULL) {
               tmp_vs[2].x = xformed_vs[j].x;
               tmp_vs[2].y = xformed_vs[j].y;
            } else {
               tmp_vs[2].x = vlist[j].x;
               tmp_vs[2].y = vlist[j].y;
            }
            SetAStructuredSplineTickMark(tmp_vs, 3, &vs[index++]);

            last_was_smooth = FALSE;
         }
         if (xformed_vs != NULL) {
            tmp_vs[0].x = xformed_vs[j].x;
            tmp_vs[0].y = xformed_vs[j].y;
            SetAStructuredSplineTickMark(&xformed_vs[j], 1, &vs[index++]);
         } else {
            tmp_vs[0].x = vlist[j].x;
            tmp_vs[0].y = vlist[j].y;
            SetAStructuredSplineTickMark(&vlist[j], 1, &vs[index++]);
         }
      }
   }
   if (xformed_vs != NULL) free(xformed_vs);
   *pn_N = num_vs;

   return vs;
}

IntPoint *MakeStructuredSplinePolyVertex(pn_N, ppszSmooth, NumVs, Vs)
   int *pn_N, NumVs; /* NumVs is poly_ptr->n */
   char **ppszSmooth;
   IntPoint *Vs; /* Vs is poly_ptr->vlist */
{
   int i=0, j=0, k=0, num_hinge_vs=(NumVs+2)/3, num_vs=0;
   IntPoint *vs=NULL;
   char *ssmooth=NULL;

#ifdef _TGIF_DBG /* debug, do not translate */
   TgAssert((NumVs+2)%3 == 0,
         "invalid NumVs in MakeStructuredSplineVertex()", NULL);
#endif /* _TGIF_DBG */
   num_vs = num_hinge_vs;
   for (i=0, j=0; i < num_hinge_vs; i++, j+=3) {
      if (i == 0) {
         if (Vs[0].x != Vs[1].x || Vs[0].y != Vs[1].y) {
            num_vs++;
         }
      } else if (i == num_hinge_vs-1) {
         if (Vs[NumVs-1].x != Vs[NumVs-2].x || Vs[NumVs-1].y != Vs[NumVs-2].y) {
            num_vs++;
         }
      } else {
         if (Vs[j-1].x != Vs[j].x || Vs[j-1].y != Vs[j].y) {
            num_vs++;
         }
         if (Vs[j+1].x != Vs[j].x || Vs[j+1].y != Vs[j].y) {
            num_vs++;
         }
      }
   }
   vs = (IntPoint*)malloc((num_vs+2)*sizeof(IntPoint));
   if (vs == NULL) FailAllocMessage();
   ssmooth = (char*)malloc((num_vs+2)*sizeof(char));
   if (ssmooth == NULL) FailAllocMessage();
   memset(ssmooth, 0, (num_vs+2)*sizeof(char));

   for (i=0, j=0, k=0; i < num_hinge_vs; i++, j+=3, k++) {
      if (i == 0) {
         vs[k].x = Vs[0].x;
         vs[k].y = Vs[0].y;
         ssmooth[k] = FALSE;
         if (Vs[0].x != Vs[1].x || Vs[0].y != Vs[1].y) {
            k++;
            vs[k].x = Vs[1].x;
            vs[k].y = Vs[1].y;
            ssmooth[k] = TRUE;
         }
      } else if (i == num_hinge_vs-1) {
         if (Vs[NumVs-1].x != Vs[NumVs-2].x || Vs[NumVs-1].y != Vs[NumVs-2].y) {
            vs[k].x = Vs[NumVs-2].x;
            vs[k].y = Vs[NumVs-2].y;
            ssmooth[k] = TRUE;
            k++;
         }
         vs[k].x = Vs[NumVs-1].x;
         vs[k].y = Vs[NumVs-1].y;
         ssmooth[k] = FALSE;
      } else {
         if (Vs[j-1].x != Vs[j].x || Vs[j-1].y != Vs[j].y) {
            vs[k].x = Vs[j-1].x;
            vs[k].y = Vs[j-1].y;
            ssmooth[k] = TRUE;
            k++;
         }
         vs[k].x = Vs[j].x;
         vs[k].y = Vs[j].y;
         ssmooth[k] = FALSE;
         if (Vs[j+1].x != Vs[j].x || Vs[j+1].y != Vs[j].y) {
            k++;
            vs[k].x = Vs[j+1].x;
            vs[k].y = Vs[j+1].y;
            ssmooth[k] = TRUE;
         }
      }
   }
#ifdef _TGIF_DBG /* debug, do not translate */
   TgAssert(k == num_vs,
         "k != num_vs in MakeStructuredSplineVertex()", NULL);
#endif /* _TGIF_DBG */
   *pn_N = num_vs;
   *ppszSmooth = ssmooth;

   return vs;
}

IntPoint *MakeStructuredSplinePolygonVertex(pn_N, ppszSmooth, NumVs, Vs)
   int *pn_N, NumVs;
   char **ppszSmooth;
   IntPoint *Vs;
{
   int i=0, j=0, k=0, num_hinge_vs=(NumVs+2)/3, num_vs=0;
   IntPoint *vs=NULL;
   char *ssmooth=NULL;

#ifdef _TGIF_DBG /* debug, do not translate */
   TgAssert((NumVs+2)%3 == 0,
         "invalid NumVs in MakeStructuredSplinePolygonVertex()", NULL);
   TgAssert(Vs[0].x == Vs[NumVs-1].x && Vs[0].y == Vs[NumVs-1].y,
         "first and last points not identical in MakeStructuredSplinePolygonVertex()",
         NULL);
#endif /* _TGIF_DBG */
   num_vs = num_hinge_vs;
   for (i=0, j=0; i < num_hinge_vs; i++, j+=3) {
      if (i == 0) {
         if (Vs[0].x != Vs[1].x || Vs[0].y != Vs[1].y) {
            num_vs++;
         }
      } else if (i == num_hinge_vs-1) {
         if (Vs[NumVs-1].x != Vs[NumVs-2].x || Vs[NumVs-1].y != Vs[NumVs-2].y) {
            num_vs++;
         }
      } else {
         if (Vs[j-1].x != Vs[j].x || Vs[j-1].y != Vs[j].y) {
            num_vs++;
         }
         if (Vs[j+1].x != Vs[j].x || Vs[j+1].y != Vs[j].y) {
            num_vs++;
         }
      }
   }
   vs = (IntPoint*)malloc((num_vs+2)*sizeof(IntPoint));
   if (vs == NULL) FailAllocMessage();
   ssmooth = (char*)malloc((num_vs+2)*sizeof(char));
   if (ssmooth == NULL) FailAllocMessage();
   memset(ssmooth, 0, (num_vs+2)*sizeof(char));

   for (i=0, j=0, k=0; i < num_hinge_vs; i++, j+=3, k++) {
      if (i == 0) {
         vs[k].x = Vs[0].x;
         vs[k].y = Vs[0].y;
         ssmooth[k] = FALSE;
         if (Vs[0].x != Vs[1].x || Vs[0].y != Vs[1].y) {
            k++;
            vs[k].x = Vs[1].x;
            vs[k].y = Vs[1].y;
            ssmooth[k] = TRUE;
         }
      } else if (i == num_hinge_vs-1) {
         if (Vs[NumVs-1].x != Vs[NumVs-2].x || Vs[NumVs-1].y != Vs[NumVs-2].y) {
            vs[k].x = Vs[NumVs-2].x;
            vs[k].y = Vs[NumVs-2].y;
            ssmooth[k] = TRUE;
            k++;
         }
         vs[k].x = Vs[NumVs-1].x;
         vs[k].y = Vs[NumVs-1].y;
         ssmooth[k] = FALSE;
      } else {
         if (Vs[j-1].x != Vs[j].x || Vs[j-1].y != Vs[j].y) {
            vs[k].x = Vs[j-1].x;
            vs[k].y = Vs[j-1].y;
            ssmooth[k] = TRUE;
            k++;
         }
         vs[k].x = Vs[j].x;
         vs[k].y = Vs[j].y;
         ssmooth[k] = FALSE;
         if (Vs[j+1].x != Vs[j].x || Vs[j+1].y != Vs[j].y) {
            k++;
            vs[k].x = Vs[j+1].x;
            vs[k].y = Vs[j+1].y;
            ssmooth[k] = TRUE;
         }
      }
   }
#ifdef _TGIF_DBG /* debug, do not translate */
   TgAssert(k == num_vs,
         "k != num_vs in MakeStructuredSplinePolygonVertex()", NULL);
#endif /* _TGIF_DBG */
   *pn_N = num_vs;
   *ppszSmooth = ssmooth;

   return vs;
}

static XPoint *splineVs=NULL;
static DoublePoint *splineDoubleVs=NULL;

static
int AddSplinePt(N, MaxN, X, Y)
   int *N, *MaxN, X, Y;
{
   if (*N == *MaxN) {
      splineVs = (XPoint*)realloc(splineVs, (*MaxN)*2*sizeof(XPoint)+1);
      if (splineVs == NULL) {
         return FailAllocMessage();
      }
      *MaxN = (*MaxN) * 2;
   }
   splineVs[*N].x = X;
   splineVs[*N].y = Y;
   (*N)++;
   return TRUE;
}

static
int AddDoubleSplinePt(N, MaxN, X, Y)
   int *N, *MaxN;
   double X, Y;
{
   if (*N == *MaxN) {
      splineDoubleVs = (DoublePoint*)realloc(splineDoubleVs,
            (*MaxN)*2*sizeof(DoublePoint)+1);
      if (splineDoubleVs == NULL) {
         return FailAllocMessage();
      }
      *MaxN = (*MaxN) << 1;
   }
   splineDoubleVs[*N].x = X;
   splineDoubleVs[*N].y = Y;
   (*N)++;
   return TRUE;
}

static
void SetSplineVs(N, MaxN, X1, Y1, X2, Y2, X3, Y3, X4, Y4)
   int *N, *MaxN;
   double X1, Y1, X2, Y2, X3, Y3, X4, Y4;
   /* X1, Y1, X2, Y2, X3, Y3, X4, Y4 are screen offsets */
{
   if (tighterStructSplines) {
      double mx1, my1, mx2, my2, mx3, my3, mx12, my12, mx23, my23, mx4, my4;

      MidPoint(X1, Y1, X2, Y2, &mx1, &my1);
      MidPoint(X2, Y2, X3, Y3, &mx2, &my2);
      MidPoint(X3, Y3, X4, Y4, &mx3, &my3);

      MidPoint(mx1, my1, mx2, my2, &mx12, &my12);
      MidPoint(mx2, my2, mx3, my3, &mx23, &my23);

      MidPoint(mx12, my12, mx23, my23, &mx4, &my4);

      if (fabs(X1 - mx4) < splineTol && fabs(Y1 - my4) < splineTol) {
         AddSplinePt(N, MaxN, round(mx4), round(my4));
      } else {
         SetSplineVs(N, MaxN, X1, Y1, mx1, my1, mx12, my12, mx4, my4);
      }
      if (fabs(mx4 - X4) < splineTol && fabs(my4 - Y4) < splineTol) {
         AddSplinePt(N, MaxN, round(X4), round(Y4));
      } else {
         SetSplineVs(N, MaxN, mx4, my4, mx23, my23, mx3, my3, X4, Y4);
      }
   } else {
      double x, y;

      x = (X2 + X3) / 2.0;
      y = (Y2 + Y3) / 2.0;
      if (fabs(X1 - x) < splineTol && fabs(Y1 - y) < splineTol) {
         AddSplinePt(N, MaxN, round(x), round(y));
      } else {
         SetSplineVs(N, MaxN, X1, Y1, ((X1+X2)/2.0), ((Y1+Y2)/2.0),
               ((3.0*X2+X3)/4.0), ((3.0*Y2+Y3)/4.0), x, y);
      }
      if (fabs(x - X4) < splineTol && fabs(y - Y4) < splineTol) {
         AddSplinePt(N, MaxN, round(X4), round(Y4));
      } else {
         SetSplineVs(N, MaxN, x, y, ((X2+3.0*X3)/4.0), ((Y2+3.0*Y3)/4.0),
               ((X3+X4)/2.0), ((Y3+Y4)/2.0), X4, Y4);
      }
   }
}

static
void SetDoubleSplineVs(N, MaxN, X1, Y1, X2, Y2, X3, Y3, X4, Y4)
   int *N, *MaxN;
   double X1, Y1, X2, Y2, X3, Y3, X4, Y4;
   /* X1, Y1, X2, Y2, X3, Y3, X4, Y4 are screen offsets */
{
   if (tighterStructSplines) {
      double mx1, my1, mx2, my2, mx3, my3, mx12, my12, mx23, my23, mx4, my4;

      MidPoint(X1, Y1, X2, Y2, &mx1, &my1);
      MidPoint(X2, Y2, X3, Y3, &mx2, &my2);
      MidPoint(X3, Y3, X4, Y4, &mx3, &my3);

      MidPoint(mx1, my1, mx2, my2, &mx12, &my12);
      MidPoint(mx2, my2, mx3, my3, &mx23, &my23);

      MidPoint(mx12, my12, mx23, my23, &mx4, &my4);

      if (fabs(X1 - mx4) < splineTol && fabs(Y1 - my4) < splineTol) {
         AddDoubleSplinePt(N, MaxN, mx4, my4);
      } else {
         SetDoubleSplineVs(N, MaxN, X1, Y1, mx1, my1, mx12, my12, mx4, my4);
      }
      if (fabs(mx4 - X4) < splineTol && fabs(my4 - Y4) < splineTol) {
         AddDoubleSplinePt(N, MaxN, round(X4), round(Y4));
      } else {
         SetDoubleSplineVs(N, MaxN, mx4, my4, mx23, my23, mx3, my3, X4, Y4);
      }
   } else {
      double x, y;

      x = (X2 + X3) / 2.0;
      y = (Y2 + Y3) / 2.0;
      if (fabs(X1 - x) < splineTol && fabs(Y1 - y) < splineTol) {
         AddDoubleSplinePt(N, MaxN, x, y);
      } else {
         SetDoubleSplineVs(N, MaxN, X1, Y1, ((X1+X2)/2.0), ((Y1+Y2)/2.0),
               ((3.0*X2+X3)/4.0), ((3.0*Y2+Y3)/4.0), x, y);
      }
      if (fabs(x - X4) < splineTol && fabs(y - Y4) < splineTol) {
         AddDoubleSplinePt(N, MaxN, X4, Y4);
      } else {
         SetDoubleSplineVs(N, MaxN, x, y, ((X2+3.0*X3)/4.0), ((Y2+3.0*Y3)/4.0),
               ((X3+X4)/2.0), ((Y3+Y4)/2.0), X4, Y4);
      }
   }
}

XPoint *MakeSplinePolyVertex(Level, Curved, N, XOff, YOff, NumVs, Vs)
   int Level, Curved, *N, XOff, YOff, NumVs;
   IntPoint *Vs;
{
   double mx1, my1, mx2, my2, mx3, my3, mx4, my4;
   double x0, y0, x1, y1, x2, y2, x3, y3;
   int i, x_off, y_off, max_n, saved_tighter_splines=tighterStructSplines;

   if (Level == 0 && Curved != LT_STRUCT_SPLINE && saved_tighter_splines) {
      tighterStructSplines = FALSE;
   }
   x_off = (zoomedIn ? XOff : (XOff>>zoomScale)<<zoomScale);
   y_off = (zoomedIn ? YOff : (YOff>>zoomScale)<<zoomScale);

   splineVs = NULL;

   switch (NumVs) {
   case 0:
   case 1:
      break;
   case 2:
      splineVs = (XPoint*)malloc((NumVs+1)*sizeof(XPoint));
      if (splineVs == NULL) {
         FailAllocMessage();
         *N = 0;
         tighterStructSplines = saved_tighter_splines;
         return splineVs;
      }
      memset(splineVs, 0, (NumVs+1)*sizeof(XPoint));
      splineVs[0].x = ZOOMED_SIZE(Vs[0].x-x_off);
      splineVs[0].y = ZOOMED_SIZE(Vs[0].y-y_off);
      splineVs[1].x = ZOOMED_SIZE(Vs[1].x-x_off);
      splineVs[1].y = ZOOMED_SIZE(Vs[1].y-y_off);
      *N = 2;
      break;
   case 3:
      if (tighterStructSplines) {
         mx1 = ZOOMED_SIZE(Vs->x-x_off); my1 = ZOOMED_SIZE((Vs++)->y-y_off);
         x1 = ZOOMED_SIZE(Vs->x-x_off);  y1 = ZOOMED_SIZE((Vs++)->y-y_off);
         mx2 = (mx1+2.0*x1)/3.0;         my2 = (my1+2.0*y1)/3.0;
         mx4 = ZOOMED_SIZE(Vs->x-x_off); my4 = ZOOMED_SIZE(Vs->y-y_off);
         mx3 = (2.0*x1+mx4)/3.0;         my3 = (2.0*y1+my4)/3.0;
         max_n = 100;
         splineVs = (XPoint*)malloc((max_n+1)*sizeof(XPoint));
         if (splineVs == NULL) {
            FailAllocMessage();
            *N = 0;
            tighterStructSplines = saved_tighter_splines;
            return splineVs;
         }
         memset(splineVs, 0, (max_n+1)*sizeof(XPoint));
         splineVs[0].x = mx1;
         splineVs[0].y = my1;
         *N = 1;
         SetSplineVs(N, &max_n, mx1, my1, mx2, my2, mx3, my3, mx4, my4);
      } else {
         mx1 = ZOOMED_SIZE(Vs->x-x_off); my1 = ZOOMED_SIZE((Vs++)->y-y_off);
         x1 = ZOOMED_SIZE(Vs->x-x_off);  y1 = ZOOMED_SIZE((Vs++)->y-y_off);
         mx2 = (mx1+x1)/2.0;             my2 = (my1+y1)/2.0;
         mx4 = ZOOMED_SIZE(Vs->x-x_off); my4 = ZOOMED_SIZE(Vs->y-y_off);
         mx3 = (x1+mx4)/2.0;             my3 = (y1+my4)/2.0;
         max_n = 100;
         splineVs = (XPoint*)malloc((max_n+1)*sizeof(XPoint));
         if (splineVs == NULL) {
            FailAllocMessage();
            *N = 0;
            tighterStructSplines = saved_tighter_splines;
            return splineVs;
         }
         memset(splineVs, 0, (max_n+1)*sizeof(XPoint));
         splineVs[0].x = mx1;
         splineVs[0].y = my1;
         *N = 1;
         SetSplineVs(N, &max_n, mx1, my1, mx2, my2, mx3, my3, mx4, my4);
      }
      break;
   default:
      if (tighterStructSplines) {
         /* must only have 4 points */
         double mx1, my1, mx2, my2, mx3, my3, mx12, my12, mx23, my23, mx4, my4;

         x0 = ZOOMED_SIZE(Vs->x-x_off);  y0 = ZOOMED_SIZE((Vs++)->y-y_off);
         x1 = ZOOMED_SIZE(Vs->x-x_off);  y1 = ZOOMED_SIZE((Vs++)->y-y_off);
         x2 = ZOOMED_SIZE(Vs->x-x_off);  y2 = ZOOMED_SIZE((Vs++)->y-y_off);
         x3 = ZOOMED_SIZE(Vs->x-x_off);  y3 = ZOOMED_SIZE(Vs->y-y_off);

         MidPoint(x0, y0, x1, y1, &mx1, &my1);
         MidPoint(x1, y1, x2, y2, &mx2, &my2);
         MidPoint(x2, y2, x3, y3, &mx3, &my3);

         MidPoint(mx1, my1, mx2, my2, &mx12, &my12);
         MidPoint(mx2, my2, mx3, my3, &mx23, &my23);

         MidPoint(mx12, my12, mx23, my23, &mx4, &my4);

         max_n = 100;
         splineVs = (XPoint *)malloc((max_n+1)*sizeof(XPoint));
         if (splineVs == NULL) {
            FailAllocMessage();
            *N = 0;
            tighterStructSplines = saved_tighter_splines;
            return splineVs;
         }
         memset(splineVs, 0, (max_n+1)*sizeof(XPoint));
         splineVs[0].x = x0;
         splineVs[0].y = y0;
         *N = 1;
         SetSplineVs(N, &max_n, x0, y0, mx1, my1, mx12, my12, mx4, my4);
         SetSplineVs(N, &max_n, mx4, my4, mx23, my23, mx3, my3, x3, y3);
      } else {
         mx1 = ZOOMED_SIZE(Vs->x-x_off); my1 = ZOOMED_SIZE((Vs++)->y-y_off);
         x1 = ZOOMED_SIZE(Vs->x-x_off);  y1 = ZOOMED_SIZE((Vs++)->y-y_off);
         x2 = ZOOMED_SIZE(Vs->x-x_off);  y2 = ZOOMED_SIZE((Vs++)->y-y_off);
         mx2 = (mx1+x1)/2.0;             my2 = (my1+y1)/2.0;
         mx3 = (3.0*x1+x2)/4.0;          my3 = (3.0*y1+y2)/4.0;
         mx4 = (x1+x2)/2.0;              my4 = (y1+y2)/2.0;
         max_n = 100;
         splineVs = (XPoint *)malloc((max_n+1)*sizeof(XPoint));
         if (splineVs == NULL) {
            FailAllocMessage();
            *N = 0;
            tighterStructSplines = saved_tighter_splines;
            return splineVs;
         }
         memset(splineVs, 0, (max_n+1)*sizeof(XPoint));
         splineVs[0].x = mx1;
         splineVs[0].y = my1;
         *N = 1;
         SetSplineVs(N, &max_n, mx1, my1, mx2, my2, mx3, my3, mx4, my4);

         for (i=2; i < NumVs-2; i++, Vs++) {
            mx1 = mx4;                     my1 = my4;
            mx2 = (x1 + 3.0*x2) / 4.0;     my2 = (y1 + 3.0*y2) / 4.0;
            x1 = x2;                       y1 = y2;
            x2 = ZOOMED_SIZE(Vs->x-x_off); y2 = ZOOMED_SIZE(Vs->y-y_off);
            mx3 = (3.0*x1 + x2) / 4.0;     my3 = (3.0*y1 + y2) / 4.0;
            mx4 = (x1 + x2) / 2.0;         my4 = (y1 + y2) / 2.0;
            SetSplineVs(N, &max_n, mx1, my1, mx2, my2, mx3, my3, mx4, my4);
         }
         mx1 = mx4;                      my1 = my4;
         mx2 = (x1 + 3.0*x2) / 4.0;      my2 = (y1 + 3.0*y2) / 4.0;
         x1 = x2;                        y1 = y2;
         mx4 = ZOOMED_SIZE(Vs->x-x_off); my4 = ZOOMED_SIZE(Vs->y-y_off);
         mx3 = (x1 + mx4) / 2.0;         my3 = (y1 + my4) / 2.0;
         SetSplineVs(N, &max_n, mx1, my1, mx2, my2, mx3, my3, mx4, my4);
      }
      break;
   }
   tighterStructSplines = saved_tighter_splines;

   return splineVs;
}

typedef struct MultiSplineRec {
   XPoint *vlist;
   int n;
} *MultiSplineRecPtr;

XPoint *MakeMultiSplinePolyVertex(Curved, N, Smooth, XOff, YOff, NumVs, Vs)
   int *N, XOff, YOff, NumVs;
   char *Smooth;
   IntPoint *Vs;
{
   register int i, j;
   int segments=1, has_smooth_point=FALSE, start_index, seg_index;
   int total=0;
   XPoint *xpptr=NULL;
   struct MultiSplineRec *msptr=NULL;

   if (Smooth == NULL) {
      return MakeSplinePolyVertex(0, Curved, N, XOff, YOff, NumVs, Vs);
   }
   if (Smooth[0] || Smooth[NumVs-1]) {
      FatalUnexpectedError(
            TgLoadCachedString(CSTID_CORRUPTED_POLY_MKMULTISPLINE),
            TgLoadCachedString(CSTID_FIX_ATTEMPTED));
      Smooth[0] = Smooth[NumVs-1] = FALSE;
   }
   for (i=1; i < NumVs-1; i++) {
      if (Smooth[i]) {
         has_smooth_point = TRUE;
      } else {
         segments++;
      }
   }
   if (!has_smooth_point) {
      *N = NumVs;
      return MakePolyVertex(XOff, YOff, NumVs, Vs);
   }
   if (segments == 1) {
      return MakeSplinePolyVertex(0, Curved, N, XOff, YOff, NumVs, Vs);
   }
   msptr = (struct MultiSplineRec *)malloc(segments *
         sizeof(struct MultiSplineRec));
   if (msptr == NULL) {
      FailAllocMessage();
      return NULL;
   }
   memset(msptr, 0, segments*sizeof(struct MultiSplineRec));

   start_index = 0;
   seg_index = 0;
   for (i=1; i <= NumVs-1; i++) {
      if (!Smooth[i]) {
         msptr[seg_index].vlist = MakeSplinePolyVertex(0, Curved,
               &msptr[seg_index].n, XOff, YOff, i-start_index+1,
               &Vs[start_index]);
         total += msptr[seg_index].n-1;
         seg_index++;
         start_index = i;
      }
   }
   if (total > 0) total++;
   splineVs = (XPoint *)malloc((total+2)*sizeof(XPoint));
   if (splineVs == NULL) FailAllocMessage();
   memset(splineVs, 0, (total+2)*sizeof(XPoint));
   xpptr = splineVs;
   for (i=0; i < segments; i++) {
      if (msptr[i].vlist != NULL) {
         for (j=0; j < msptr[i].n; j++) {
            xpptr->x = msptr[i].vlist[j].x;
            xpptr->y = msptr[i].vlist[j].y;
            xpptr++;
         }
         xpptr--;
         free(msptr[i].vlist);
      }
   }
   free(msptr);
   *N = total;
   return splineVs;
}

XPoint *MakeSplinePolygonVertex(Level, Curved, N, XOff, YOff, NumVs, Vs)
   int Level, Curved, *N, XOff, YOff, NumVs;
   IntPoint *Vs;
{
   double mx1, my1, mx2, my2, mx3, my3, mx4, my4, x1, y1, x2, y2;
   int i, x_off, y_off, max_n, saved_tighter_splines=tighterStructSplines;

#ifdef _TGIF_DBG /* debug, do not translate */
   TgAssert(Curved != LT_STRUCT_SPLINE,
         "Curved == LT_STRUCT_SPLINE in MakeSplinePolygonVertex()", NULL);
#endif /* _TGIF_DBG */

   if (Level == 0 && Curved != LT_STRUCT_SPLINE && saved_tighter_splines) {
      tighterStructSplines = FALSE;
   }
   x_off = (zoomedIn ? XOff : (XOff>>zoomScale)<<zoomScale);
   y_off = (zoomedIn ? YOff : (YOff>>zoomScale)<<zoomScale);

   splineVs = NULL;

   if (NumVs <= 3) {
      splineVs = (XPoint *)malloc(5*sizeof(XPoint));
      if (splineVs == NULL) {
         FailAllocMessage();
         *N = 0;
         tighterStructSplines = saved_tighter_splines;
         return splineVs;
      }
      memset(splineVs, 0, 5*sizeof(XPoint));
      splineVs[0].x = ZOOMED_SIZE(Vs[0].x-x_off);
      splineVs[0].y = ZOOMED_SIZE(Vs[0].y-y_off);
      splineVs[1].x = ZOOMED_SIZE(Vs[1].x-x_off);
      splineVs[1].y = ZOOMED_SIZE(Vs[1].y-y_off);
      *N = 2;
      tighterStructSplines = saved_tighter_splines;
      return splineVs;
   }
   Vs[NumVs].x = Vs[1].x; Vs[NumVs].y = Vs[1].y;
   x1 = ZOOMED_SIZE(Vs->x-x_off); y1 = ZOOMED_SIZE((Vs++)->y-y_off);
   x2 = ZOOMED_SIZE(Vs->x-x_off); y2 = ZOOMED_SIZE((Vs++)->y-y_off);
   mx4 = (x1 + x2) / 2.0;         my4 = (y1 + y2) / 2.0;

   max_n = 100;
   splineVs = (XPoint*)malloc((max_n+1)*sizeof(XPoint));
   if (splineVs == NULL) {
      FailAllocMessage();
      *N = 0;
      tighterStructSplines = saved_tighter_splines;
      return splineVs;
   }
   memset(splineVs, 0, (max_n+1)*sizeof(XPoint));
   splineVs[0].x = mx4;
   splineVs[0].y = my4;
   *N = 1;

   for (i=1; i < NumVs; i++, Vs++) {
      mx1 = mx4;                     my1 = my4;
      mx2 = (x1+3.0*x2)/4.0;         my2 = (y1+3.0*y2)/4.0;
      x1 = x2;                       y1 = y2;
      x2 = ZOOMED_SIZE(Vs->x-x_off); y2 = ZOOMED_SIZE(Vs->y-y_off);
      mx3 = (3.0*x1+x2)/4.0;         my3 = (3.0*y1+y2)/4.0;
      mx4 = (x1+x2)/2.0;             my4 = (y1+y2)/2.0;
      SetSplineVs(N, &max_n, mx1, my1, mx2, my2, mx3, my3, mx4, my4);
   }
   tighterStructSplines = saved_tighter_splines;

   return splineVs;
}

static
XPoint *DoMakeDoubleIntSplinePolygonVertex(N, XOff, YOff, NumVs, Vs)
   int *N, XOff, YOff, NumVs;
   DoublePoint *Vs;
{
   double mx1, my1, mx2, my2, mx3, my3, mx4, my4, x1, y1, x2, y2, x, y;
   double dx_off, dy_off;
   int i, max_n, x_off, y_off;

   x_off = (zoomedIn ? XOff : (XOff>>zoomScale)<<zoomScale);
   y_off = (zoomedIn ? YOff : (YOff>>zoomScale)<<zoomScale);

   dx_off = (double)x_off;
   dy_off = (double)y_off;

   splineVs = NULL;

   if (NumVs <= 3) {
      splineVs = (XPoint *)malloc(5*sizeof(XPoint));
      if (splineVs == NULL) {
         FailAllocMessage();
         *N = 0;
         return splineVs;
      }
      memset(splineVs, 0, 5*sizeof(XPoint));
      x = ZOOMED_DOUBLE_SIZE(Vs[0].x-dx_off);
      y = ZOOMED_DOUBLE_SIZE(Vs[0].y-dy_off);
      splineVs[0].x = round(x);
      splineVs[0].y = round(y);
      x = ZOOMED_DOUBLE_SIZE(Vs[1].x-dx_off);
      y = ZOOMED_DOUBLE_SIZE(Vs[1].y-dy_off);
      splineVs[1].x = round(x);
      splineVs[1].y = round(y);
      *N = 2;
      return splineVs;
   }

   Vs[NumVs].x = Vs[1].x; Vs[NumVs].y = Vs[1].y;
   x1 = ZOOMED_DOUBLE_SIZE(Vs->x-dx_off);
   y1 = ZOOMED_DOUBLE_SIZE((Vs++)->y-dy_off);
   x2 = ZOOMED_DOUBLE_SIZE(Vs->x-dx_off);
   y2 = ZOOMED_DOUBLE_SIZE((Vs++)->y-dy_off);
   mx4 = (x1 + x2) / 2.0;
   my4 = (y1 + y2) / 2.0;

   max_n = 100;
   splineDoubleVs = (DoublePoint*)malloc((max_n+1)*sizeof(DoublePoint));
   if (splineDoubleVs == NULL) {
      FailAllocMessage();
      *N = 0;
      return NULL;
   }
   memset(splineDoubleVs, 0, (max_n+1)*sizeof(DoublePoint));
   splineDoubleVs[0].x = mx4;
   splineDoubleVs[0].y = my4;
   *N = 1;

   for (i=1; i < NumVs; i++, Vs++) {
      mx1 = mx4;                             my1 = my4;
      mx2 = (x1+3.0*x2)/4.0;                 my2 = (y1+3.0*y2)/4.0;
      x1 = x2;                               y1 = y2;
      x2 = ZOOMED_DOUBLE_SIZE(Vs->x-dx_off); y2 = ZOOMED_DOUBLE_SIZE(Vs->y-dy_off);
      mx3 = (3.0*x1+x2)/4.0;                 my3 = (3.0*y1+y2)/4.0;
      mx4 = (x1+x2)/2.0;                     my4 = (y1+y2)/2.0;
      SetDoubleSplineVs(N, &max_n, mx1, my1, mx2, my2, mx3, my3, mx4, my4);
   }
   splineVs = (XPoint*)malloc(((*N)+2)*sizeof(XPoint));
   if (splineVs == NULL) {
      FailAllocMessage();
      free(splineDoubleVs);
      splineDoubleVs = NULL;
      *N = 0;
      return NULL;
   }
   memset(splineVs, 0, ((*N)+2)*sizeof(XPoint));
   for (i=0; i < *N; i++) {
      splineVs[i].x = round(splineDoubleVs[i].x);
      splineVs[i].y = round(splineDoubleVs[i].y);
   }
   free(splineDoubleVs);
   splineDoubleVs = NULL;
   return splineVs;
}

XPoint *MakeMultiSplinePolygonVertex(Curved, N, Smooth, XOff, YOff, NumVs, Vs)
   int Curved, *N, XOff, YOff, NumVs;
   char *Smooth;
   IntPoint *Vs;
{
   register int i, j;
   int num_smooth_points=0, num_hinge_points=0;
   int start_index, seg_index, tmp_index;
   int total=0, once_around=FALSE;
   XPoint *xpptr=NULL;
   IntPoint *tmp_vs=NULL;
   struct MultiSplineRec *msptr=NULL;

   if (Smooth == NULL) {
      return MakeSplinePolygonVertex(0, Curved, N, XOff, YOff, NumVs, Vs);
   }
   for (i=1; i < NumVs; i++) {
      if (Smooth[i]) {
         num_smooth_points++;
      } else {
         num_hinge_points++;
      }
   }
   if (num_smooth_points == 0) {
      *N = NumVs;
      return MakePolygonVertex(XOff, YOff, NumVs, Vs);
   }
   if (num_hinge_points == 0) {
      return MakeSplinePolygonVertex(0, Curved, N, XOff, YOff, NumVs, Vs);
   }
   msptr = (struct MultiSplineRec *)malloc(num_hinge_points *
         sizeof(struct MultiSplineRec));
   if (msptr == NULL) FailAllocMessage();
   memset(msptr, 0, num_hinge_points*sizeof(struct MultiSplineRec));

   for (i=0; i < NumVs; i++) {
      if (!Smooth[i]) {
         break;
      }
   }
   tmp_vs = (IntPoint*)malloc((NumVs+1)*sizeof(IntPoint));
   if (tmp_vs == NULL) FailAllocMessage();
   memset(tmp_vs, 0, (NumVs+1)*sizeof(IntPoint));
   start_index = i;
   seg_index = 0;

   tmp_vs[0].x = Vs[start_index].x;
   tmp_vs[0].y = Vs[start_index].y;
   tmp_index = 1;
   for (i=start_index+1; !(once_around && i==start_index+1); i++, tmp_index++) {
      tmp_vs[tmp_index].x = Vs[i].x;
      tmp_vs[tmp_index].y = Vs[i].y;
      if (!Smooth[i]) {
         msptr[seg_index].vlist = MakeSplinePolyVertex(0, Curved,
               &msptr[seg_index].n, XOff, YOff, tmp_index+1, tmp_vs);
         total += msptr[seg_index].n-1;
         seg_index++;
         start_index = (i==NumVs-1 ? 0 : i);
         tmp_vs[0].x = Vs[start_index].x;
         tmp_vs[0].y = Vs[start_index].y;
         tmp_index = 0;
      }
      if (i == NumVs-1) {
         i = 0;
         once_around = TRUE;
      }
   }
   if (tmp_vs != NULL) free(tmp_vs);
   if (total > 0) total++;
   splineVs = (XPoint*)malloc((total+2)*sizeof(XPoint));
   if (splineVs == NULL) FailAllocMessage();
   memset(splineVs, 0, (total+2)*sizeof(XPoint));
   xpptr = splineVs;
   for (i=0; i < num_hinge_points; i++) {
      if (msptr[i].vlist != NULL) {
         for (j=0; j < msptr[i].n; j++) {
            xpptr->x = msptr[i].vlist[j].x;
            xpptr->y = msptr[i].vlist[j].y;
            xpptr++;
         }
         xpptr--;
         free(msptr[i].vlist);
      }
   }
   free(msptr);
   *N = total;
   return splineVs;
}

struct MtxRec {
   double *x, *y, *dx, *dy;
   double **mtx;
} mtxInfo;

static
void OpenSetupMatrix(NumPts, Vs)
   int NumPts;
   IntPoint *Vs;
{
   register int i;

   mtxInfo.x = (double*)malloc(NumPts*sizeof(double));
   mtxInfo.y = (double*)malloc(NumPts*sizeof(double));
   mtxInfo.dx = (double*)malloc(NumPts*sizeof(double));
   mtxInfo.dy = (double*)malloc(NumPts*sizeof(double));
   if (mtxInfo.x == NULL || mtxInfo.y == NULL || mtxInfo.dx == NULL ||
         mtxInfo.dy == NULL) {
      FailAllocMessage();
   }
   for (i=0; i < NumPts; i++) {
      mtxInfo.x[i] = mtxInfo.dx[i] = ((double)(Vs[i].x))*((double)theSum);
      mtxInfo.y[i] = mtxInfo.dy[i] = ((double)(Vs[i].y))*((double)theSum);
   }
   mtxInfo.mtx = (double**)malloc(NumPts*sizeof(double*));
   if (mtxInfo.mtx == NULL) FailAllocMessage();
   memset(mtxInfo.mtx, 0, NumPts*sizeof(double*));
   for (i=0; i < NumPts; i++) {
      mtxInfo.mtx[i] = (double*)malloc(3*sizeof(double));
      if (mtxInfo.mtx[i] == NULL) FailAllocMessage();
      memset(mtxInfo.mtx[i], 0, 3*sizeof(double));
   }
   mtxInfo.mtx[0][0] = mtxInfo.mtx[NumPts-1][2] = (double)0.0;
   mtxInfo.mtx[0][1] = mtxInfo.mtx[NumPts-1][1] = (double)theSum;
   mtxInfo.mtx[0][2] = mtxInfo.mtx[NumPts-1][0] = (double)0.0;
   for (i=1; i < NumPts-1; i++) {
      mtxInfo.mtx[i][0] = (double)1.0;
      mtxInfo.mtx[i][1] = (double)SUM_MINUS_2;
      mtxInfo.mtx[i][2] = (double)1.0;
   }
}

static
void TriGaussian(NumPts)
   int NumPts;
{
   register int i;
   register double val;

   for (i=1; i<NumPts-1; i++) {
      val = (-mtxInfo.mtx[i-1][1]);
      mtxInfo.mtx[i][0] = (double)0.0;
      mtxInfo.mtx[i][1] = (mtxInfo.mtx[i][1]*val+mtxInfo.mtx[i-1][2])/theSum;
      mtxInfo.mtx[i][2] = val/theSum;
      mtxInfo.x[i] = (mtxInfo.x[i]*val+mtxInfo.x[i-1])/theSum;
      mtxInfo.y[i] = (mtxInfo.y[i]*val+mtxInfo.y[i-1])/theSum;
   }
   mtxInfo.x[i] = mtxInfo.x[i]/mtxInfo.mtx[i][1];
   mtxInfo.y[i] = mtxInfo.y[i]/mtxInfo.mtx[i][1];
   for (i--; i>=0; i--) {
      mtxInfo.x[i] = (mtxInfo.x[i]-mtxInfo.x[i+1]*mtxInfo.mtx[i][2]) /
            mtxInfo.mtx[i][1];
      mtxInfo.y[i] = (mtxInfo.y[i]-mtxInfo.y[i+1]*mtxInfo.mtx[i][2]) /
            mtxInfo.mtx[i][1];
   }
}

static
void FreeMtxInfo(NumPts)
   int NumPts;
{
   register int i;

   if (mtxInfo.x != NULL) free(mtxInfo.x);
   if (mtxInfo.y != NULL) free(mtxInfo.y);
   if (mtxInfo.dx != NULL) free(mtxInfo.dx);
   if (mtxInfo.dy != NULL) free(mtxInfo.dy);
   if (mtxInfo.mtx != NULL) {
      for (i=0; i < NumPts; i++) {
         if (mtxInfo.mtx[i] != NULL) free(mtxInfo.mtx[i]);
      }
      free(mtxInfo.mtx);
   }
   memset(&mtxInfo, 0, sizeof(struct MtxRec));
}

static
IntPoint *OpenControlPts(NumPts, N)
   int NumPts, *N;
{
   register int i;
   int index=0;
   double half=theSum/((double)2.0);
   double weight=half-((double)1.0);
   IntPoint *v;

   v = (IntPoint*)malloc((((NumPts-2)<<1)+2)*sizeof(IntPoint));
   if (v == NULL) FailAllocMessage();
   memset(v, 0, (((NumPts-2)<<1)+2)*sizeof(IntPoint));

   v[index].x = (int)(mtxInfo.x[0]);
   v[index].y = (int)(mtxInfo.y[0]);
   index++;
   v[index].x = (int)((mtxInfo.x[0]+weight*mtxInfo.x[1])/half);
   v[index].y = (int)((mtxInfo.y[0]+weight*mtxInfo.y[1])/half);
   index++;
   for (i=1; i<NumPts-2; i++) {
      v[index].x = (int)((weight*mtxInfo.x[i]+mtxInfo.x[i+1])/half);
      v[index].y = (int)((weight*mtxInfo.y[i]+mtxInfo.y[i+1])/half);
      index++;
      v[index].x = (int)((mtxInfo.x[i]+weight*mtxInfo.x[i+1])/half);
      v[index].y = (int)((mtxInfo.y[i]+weight*mtxInfo.y[i+1])/half);
      index++;
   }
   v[index].x = (int)((weight*mtxInfo.x[i]+mtxInfo.x[i+1])/half);
   v[index].y = (int)((weight*mtxInfo.y[i]+mtxInfo.y[i+1])/half);
   index++;
   v[index].x = (int)(mtxInfo.x[NumPts-1]);
   v[index].y = (int)(mtxInfo.y[NumPts-1]);
   index++;

   FreeMtxInfo(NumPts);

   *N = index;
   return v;
}

XPoint *MakeIntSplinePolyVertex(N, CntrlN, CntrlVs, XOff, YOff, NumVs, Vs)
   int *N, *CntrlN, XOff, YOff, NumVs;
   IntPoint **CntrlVs, *Vs;
{
   int x_off, y_off;

   x_off = (zoomedIn ? XOff : (XOff>>zoomScale)<<zoomScale);
   y_off = (zoomedIn ? YOff : (YOff>>zoomScale)<<zoomScale);

   splineVs = NULL;

   switch (NumVs) {
   case 0:
   case 1:
      break;
   case 2:
      *CntrlVs = (IntPoint*)malloc((NumVs+1)*sizeof(IntPoint));
      if (*CntrlVs == NULL) FailAllocMessage();
      memset(*CntrlVs, 0, (NumVs+1)*sizeof(IntPoint));
      splineVs = (XPoint*)malloc((NumVs+1)*sizeof(XPoint));
      if (splineVs == NULL) {
         FailAllocMessage();
         *N = 0;
         return splineVs;
      }
      memset(splineVs, 0, (NumVs+1)*sizeof(XPoint));
      splineVs[0].x = ZOOMED_SIZE(Vs[0].x-x_off);
      splineVs[0].y = ZOOMED_SIZE(Vs[0].y-y_off);
      splineVs[1].x = ZOOMED_SIZE(Vs[1].x-x_off);
      splineVs[1].y = ZOOMED_SIZE(Vs[1].y-y_off);
      (*CntrlVs)[0].x = Vs[0].x;
      (*CntrlVs)[0].y = Vs[0].y;
      (*CntrlVs)[1].x = Vs[1].x;
      (*CntrlVs)[1].y = Vs[1].y;
      *N = *CntrlN = 2;
      break;
   default:
      OpenSetupMatrix(NumVs, Vs);
      TriGaussian(NumVs);
      *CntrlVs = OpenControlPts(NumVs, CntrlN);

      return MakeSplinePolyVertex(0, LT_INTSPLINE, N, XOff, YOff, *CntrlN,
            *CntrlVs);
   }
   return splineVs;
}

static
void ClosedSetupMatrix(NumPts, Vs)
   int NumPts;
   IntPoint *Vs;
{
   register int i;
   register double val;

   mtxInfo.x = (double*)malloc(NumPts*sizeof(double));
   mtxInfo.y = (double*)malloc(NumPts*sizeof(double));
   mtxInfo.dx = (double*)malloc(NumPts*sizeof(double));
   mtxInfo.dy = (double*)malloc(NumPts*sizeof(double));
   if (mtxInfo.x == NULL || mtxInfo.y == NULL || mtxInfo.dx == NULL ||
         mtxInfo.dy == NULL) {
      FailAllocMessage();
   }
   for (i=0; i < NumPts; i++) {
      mtxInfo.x[i] = mtxInfo.dx[i] = ((double)(Vs[i].x))*((double)theSum);
      mtxInfo.y[i] = mtxInfo.dy[i] = ((double)(Vs[i].y))*((double)theSum);
   }
   /* the first NumPts-2 rows have an extra column */
   mtxInfo.mtx = (double**)malloc(NumPts*sizeof(double*));
   if (mtxInfo.mtx == NULL) FailAllocMessage();
   memset(mtxInfo.mtx, 0, NumPts*sizeof(double*));
   for (i=0; i < NumPts; i++) {
      mtxInfo.mtx[i] = (double*)malloc(4*sizeof(double));
      if (mtxInfo.mtx[i] == NULL) FailAllocMessage();
      memset(mtxInfo.mtx[i], 0, 4*sizeof(double));
   }
   mtxInfo.mtx[0][0] = mtxInfo.mtx[NumPts-1][2] = (double)1.0;
   mtxInfo.mtx[0][1] = mtxInfo.mtx[NumPts-1][1] = (double)SUM_MINUS_2;
   mtxInfo.mtx[0][2] = mtxInfo.mtx[NumPts-1][0] = (double)1.0;
   mtxInfo.mtx[0][3] = (double)1.0;
   /* use mtx[NumPts-1][3] as mtx[NumPts-1][i] where i is moving to right */
   mtxInfo.mtx[NumPts-1][3] = (double)0.0;
   for (i=1; i < NumPts-1; i++) {
      mtxInfo.mtx[i][0] = (double)1.0;
      mtxInfo.mtx[i][1] = (double)SUM_MINUS_2;
      mtxInfo.mtx[i][2] = (double)1.0;
      mtxInfo.mtx[i][3] = (double)0.0;
   }
   val = (-mtxInfo.mtx[0][1]);
   if (NumPts == 3) {
      mtxInfo.mtx[NumPts-1][0] =
            (mtxInfo.mtx[NumPts-1][0]*val+mtxInfo.mtx[0][2])/theSum;
   } else {
      mtxInfo.mtx[NumPts-1][0] = mtxInfo.mtx[NumPts-1][0]*val/theSum;
   }
   mtxInfo.mtx[NumPts-1][1] =
         (mtxInfo.mtx[NumPts-1][1]*val+mtxInfo.mtx[0][3])/theSum;
   /* use mtx[NumPts-1][3] as mtx[NumPts-1][i] where i moves to right */
   mtxInfo.mtx[NumPts-1][3] = mtxInfo.mtx[0][2]/theSum;
   mtxInfo.x[NumPts-1] = (mtxInfo.x[NumPts-1]*val+mtxInfo.x[0])/theSum;
   mtxInfo.y[NumPts-1] = (mtxInfo.y[NumPts-1]*val+mtxInfo.y[0])/theSum;
}

static
void DoubleClosedSetupMatrix(NumPts, Vs)
   int NumPts;
   DoublePoint *Vs;
{
   register int i;
   register double val;

   mtxInfo.x = (double*)malloc(NumPts*sizeof(double));
   mtxInfo.y = (double*)malloc(NumPts*sizeof(double));
   mtxInfo.dx = (double*)malloc(NumPts*sizeof(double));
   mtxInfo.dy = (double*)malloc(NumPts*sizeof(double));
   if (mtxInfo.x == NULL || mtxInfo.y == NULL || mtxInfo.dx == NULL ||
         mtxInfo.dy == NULL) {
      FailAllocMessage();
   }
   for (i=0; i < NumPts; i++) {
      mtxInfo.x[i] = mtxInfo.dx[i] = ((double)(Vs[i].x))*((double)theSum);
      mtxInfo.y[i] = mtxInfo.dy[i] = ((double)(Vs[i].y))*((double)theSum);
   }
   /* the first NumPts-2 rows have an extra column */
   mtxInfo.mtx = (double**)malloc(NumPts*sizeof(double*));
   if (mtxInfo.mtx == NULL) FailAllocMessage();
   memset(mtxInfo.mtx, 0, NumPts*sizeof(double*));
   for (i=0; i < NumPts; i++) {
      mtxInfo.mtx[i] = (double*)malloc(4*sizeof(double));
      if (mtxInfo.mtx[i] == NULL) FailAllocMessage();
      memset(mtxInfo.mtx[i], 0, 4*sizeof(double));
   }
   mtxInfo.mtx[0][0] = mtxInfo.mtx[NumPts-1][2] = (double)1.0;
   mtxInfo.mtx[0][1] = mtxInfo.mtx[NumPts-1][1] = (double)SUM_MINUS_2;
   mtxInfo.mtx[0][2] = mtxInfo.mtx[NumPts-1][0] = (double)1.0;
   mtxInfo.mtx[0][3] = (double)1.0;
   /* use mtx[NumPts-1][3] as mtx[NumPts-1][i] where i is moving to right */
   mtxInfo.mtx[NumPts-1][3] = (double)0.0;
   for (i=1; i < NumPts-1; i++) {
      mtxInfo.mtx[i][0] = (double)1.0;
      mtxInfo.mtx[i][1] = (double)SUM_MINUS_2;
      mtxInfo.mtx[i][2] = (double)1.0;
      mtxInfo.mtx[i][3] = (double)0.0;
   }
   val = (-mtxInfo.mtx[0][1]);
   if (NumPts == 3) {
      mtxInfo.mtx[NumPts-1][0] =
            (mtxInfo.mtx[NumPts-1][0]*val+mtxInfo.mtx[0][2])/theSum;
   } else {
      mtxInfo.mtx[NumPts-1][0] = mtxInfo.mtx[NumPts-1][0]*val/theSum;
   }
   mtxInfo.mtx[NumPts-1][1] =
         (mtxInfo.mtx[NumPts-1][1]*val+mtxInfo.mtx[0][3])/theSum;
   /* use mtx[NumPts-1][3] as mtx[NumPts-1][i] where i moves to right */
   mtxInfo.mtx[NumPts-1][3] = mtxInfo.mtx[0][2]/theSum;
   mtxInfo.x[NumPts-1] = (mtxInfo.x[NumPts-1]*val+mtxInfo.x[0])/theSum;
   mtxInfo.y[NumPts-1] = (mtxInfo.y[NumPts-1]*val+mtxInfo.y[0])/theSum;
}

static int gaussIteration=0;

#ifdef NOT_DEFINED
#ifdef _TGIF_DBG
static
void DebugClosedMatrix(NumPts)
   int NumPts;
{
   int j, i;

   /* mtxInfo.mtx[0][3] is actually mtxInfo.mtx[0][NumPts-1]           */
   /* mtxInfo.mtx[NumPts-1][3] is actually mtxInfo.mtx[NumPts-1][i+1]  */
   /*       where i is the current iteration in gaussian elimincation; */
   /*       before gaussian() is called, i is 0                        */
   printf("|  %+8.2f  %+8.2f  ", mtxInfo.mtx[0][1], mtxInfo.mtx[0][2]);
   for (j=2; j<NumPts-1; j++) printf("          ");
   printf("%+8.2f  |  (%+8.2f,%+8.2f)\n", mtxInfo.mtx[0][3],
         mtxInfo.x[0], mtxInfo.y[0]);
   for (i=1; i<NumPts-1; i++) {
      if (i == NumPts-2) {
         printf("|  ");
         for (j=1; j<i; j++) printf("          ");
         printf("%+8.2f  %+8.2f  %+8.2f  ", mtxInfo.mtx[i][0],
               mtxInfo.mtx[i][1], mtxInfo.mtx[i][2]);
         for (j=i+2; j<NumPts; j++) printf("          ");
         printf("|  (%+8.2f,%+8.2f)\n", mtxInfo.x[i], mtxInfo.y[i]);
      } else {
         printf("|  ");
         for (j=1; j<i; j++) printf("          ");
         printf("%+8.2f  %+8.2f  %+8.2f  ", mtxInfo.mtx[i][0],
               mtxInfo.mtx[i][1], mtxInfo.mtx[i][2]);
         for (j=i+2; j<NumPts-1; j++) printf("          ");
         printf("%+8.2f  |  (%+8.2f,%+8.2f)\n", mtxInfo.mtx[i][3],
               mtxInfo.x[i], mtxInfo.y[i]);
      }
   }
   printf("|  ");
   if (gaussIteration+2 < i) {
      for (j=1; j<gaussIteration+2; j++) printf("          ");
      printf("%+8.2f  ", mtxInfo.mtx[i][3]);
      for (j=gaussIteration+3; j<i; j++) printf("          ");
   } else {
      for (j=1; j<i; j++) printf("          ");
   }
   printf("%+8.2f  %+8.2f  ", mtxInfo.mtx[i][0], mtxInfo.mtx[i][1]);
   printf("|  (%+8.2f,%+8.2f)\n", mtxInfo.x[i], mtxInfo.y[i]);
}
#endif /* _TGIF_DBG */
#endif /* NOT_DEFINED */

static
void Gaussian(NumPts)
   int NumPts;
{
   register int i;
   register double val;

   gaussIteration = 0;
#ifdef _TGIF_DBG
   /* DebugClosedMatrix(NumPts); */
#endif /* _TGIF_DBG */
   for (i=1; i<NumPts-1; i++) {
      val = (-mtxInfo.mtx[i-1][1]);
      mtxInfo.mtx[i][0] = (double)0.0;
      mtxInfo.mtx[i][1] = (mtxInfo.mtx[i][1]*val+mtxInfo.mtx[i-1][2])/theSum;
      if (i == NumPts-2) {
         mtxInfo.mtx[i][2] = (mtxInfo.mtx[i][2]*val+mtxInfo.mtx[i-1][3])/theSum;
      } else {
         mtxInfo.mtx[i][2] = val/theSum;
      }
      mtxInfo.x[i] = (mtxInfo.x[i]*val+mtxInfo.x[i-1])/theSum;
      mtxInfo.y[i] = (mtxInfo.y[i]*val+mtxInfo.y[i-1])/theSum;
      if (i != NumPts-2) {
         mtxInfo.mtx[i][3] = mtxInfo.mtx[i-1][3]/theSum;;
      }
      val = (-mtxInfo.mtx[i][1])/mtxInfo.mtx[NumPts-1][3];
      if (i < NumPts-2) {
         if (i < NumPts-3) {
            mtxInfo.mtx[NumPts-1][0] = mtxInfo.mtx[NumPts-1][0]*val/theSum;
         } else {
            mtxInfo.mtx[NumPts-1][0] =
                  (mtxInfo.mtx[NumPts-1][0]*val+mtxInfo.mtx[i][2])/theSum;
         }
         mtxInfo.mtx[NumPts-1][1] =
               (mtxInfo.mtx[NumPts-1][1]*val+mtxInfo.mtx[i][3])/theSum;
         mtxInfo.mtx[NumPts-1][3] = mtxInfo.mtx[i][2]/theSum;
         mtxInfo.x[NumPts-1] = (mtxInfo.x[NumPts-1]*val+mtxInfo.x[i])/theSum;
         mtxInfo.y[NumPts-1] = (mtxInfo.y[NumPts-1]*val+mtxInfo.y[i])/theSum;
      }
      gaussIteration++;
#ifdef _TGIF_DBG
      /* DebugClosedMatrix(NumPts); */
#endif /* _TGIF_DBG */
   }
   val = (-mtxInfo.mtx[i-1][1])/mtxInfo.mtx[i][0];
   mtxInfo.mtx[i][0] = (double)0.0;
   mtxInfo.mtx[i][1] = (mtxInfo.mtx[i][1]*val+mtxInfo.mtx[i-1][2])/theSum;
   mtxInfo.x[i] = (mtxInfo.x[i]*val+mtxInfo.x[i-1])/theSum;
   mtxInfo.y[i] = (mtxInfo.y[i]*val+mtxInfo.y[i-1])/theSum;

   mtxInfo.x[i] = mtxInfo.x[i]/mtxInfo.mtx[i][1];
   mtxInfo.y[i] = mtxInfo.y[i]/mtxInfo.mtx[i][1];
   for (i--; i>=0; i--) {
      if (i == NumPts-2) {
         mtxInfo.x[i] = (mtxInfo.x[i]-mtxInfo.x[i+1]*mtxInfo.mtx[i][2]) /
               mtxInfo.mtx[i][1];
         mtxInfo.y[i] = (mtxInfo.y[i]-mtxInfo.y[i+1]*mtxInfo.mtx[i][2]) /
               mtxInfo.mtx[i][1];
      } else {
         mtxInfo.x[i] = (mtxInfo.x[i]-mtxInfo.x[i+1]*mtxInfo.mtx[i][2] -
               mtxInfo.x[NumPts-1]*mtxInfo.mtx[i][3])/mtxInfo.mtx[i][1];
         mtxInfo.y[i] = (mtxInfo.y[i]-mtxInfo.y[i+1]*mtxInfo.mtx[i][2] -
               mtxInfo.y[NumPts-1]*mtxInfo.mtx[i][3])/mtxInfo.mtx[i][1];
      }
   }
#ifdef _TGIF_DBG
   /* DebugClosedMatrix(NumPts); */
#endif /* _TGIF_DBG */
}

static
IntPoint *ClosedControlPts(NumPts, N)
   int NumPts, *N;
{
   register int i;
   int index=0;
   double half=theSum/((double)2.0);
   double weight=half-((double)1.0);
   IntPoint *v;

   v = (IntPoint*)malloc(((NumPts<<1)+2)*sizeof(IntPoint));
   if (v == NULL) FailAllocMessage();
   memset(v, 0, ((NumPts<<1)+2)*sizeof(IntPoint));
   for (i=0; i<NumPts; i++) {
      v[index].x = (int)((weight*mtxInfo.x[i]+mtxInfo.x[(i+1) % NumPts])/half);
      v[index].y = (int)((weight*mtxInfo.y[i]+mtxInfo.y[(i+1) % NumPts])/half);
      index++;
      v[index].x = (int)((mtxInfo.x[i]+weight*mtxInfo.x[(i+1) % NumPts])/half);
      v[index].y = (int)((mtxInfo.y[i]+weight*mtxInfo.y[(i+1) % NumPts])/half);
      index++;
   }
   v[index].x = (int)((weight*mtxInfo.x[0]+mtxInfo.x[1])/half);
   v[index].y = (int)((weight*mtxInfo.y[0]+mtxInfo.y[1])/half);
   index++;

   FreeMtxInfo(NumPts);

   *N = index;
   return v;
}

static
DoublePoint *DoubleClosedControlPts(NumPts, N)
   int NumPts, *N;
{
   register int i;
   int index=0;
   double half=theSum/((double)2.0);
   double weight=half-((double)1.0);
   DoublePoint *v;

   v = (DoublePoint*)malloc(((NumPts<<1)+2)*sizeof(DoublePoint));
   if (v == NULL) FailAllocMessage();
   memset(v, 0, ((NumPts<<1)+2)*sizeof(DoublePoint));
   for (i=0; i<NumPts; i++) {
      v[index].x = ((weight*mtxInfo.x[i]+mtxInfo.x[(i+1) % NumPts])/half);
      v[index].y = ((weight*mtxInfo.y[i]+mtxInfo.y[(i+1) % NumPts])/half);
      index++;
      v[index].x = ((mtxInfo.x[i]+weight*mtxInfo.x[(i+1) % NumPts])/half);
      v[index].y = ((mtxInfo.y[i]+weight*mtxInfo.y[(i+1) % NumPts])/half);
      index++;
   }
   v[index].x = ((weight*mtxInfo.x[0]+mtxInfo.x[1])/half);
   v[index].y = ((weight*mtxInfo.y[0]+mtxInfo.y[1])/half);
   index++;

   FreeMtxInfo(NumPts);

   *N = index;
   return v;
}

XPoint *MakeIntSplinePolygonVertex(N, CntrlN, CntrlVs, XOff, YOff, NumVs, Vs)
   int *N, *CntrlN, XOff, YOff, NumVs;
   IntPoint **CntrlVs, *Vs;
{
   int x_off, y_off;

   x_off = (zoomedIn ? XOff : (XOff>>zoomScale)<<zoomScale);
   y_off = (zoomedIn ? YOff : (YOff>>zoomScale)<<zoomScale);

   splineVs = NULL;

   if (NumVs <= 3) {
      splineVs = (XPoint *)malloc(5*sizeof(XPoint));
      if (splineVs == NULL) {
         FailAllocMessage();
         *N = 0;
         return (splineVs);
      }
      memset(splineVs, 0, 5*sizeof(XPoint));
      splineVs[0].x = ZOOMED_SIZE(Vs[0].x-x_off);
      splineVs[0].y = ZOOMED_SIZE(Vs[0].y-y_off);
      splineVs[1].x = ZOOMED_SIZE(Vs[1].x-x_off);
      splineVs[1].y = ZOOMED_SIZE(Vs[1].y-y_off);
      *N = *CntrlN = 2;
      return splineVs;
   }
   gaussIteration = 0;
   NumVs--; /* drop the duplicated end point */
   ClosedSetupMatrix(NumVs, Vs);
   Gaussian(NumVs);
   *CntrlVs = ClosedControlPts(NumVs, CntrlN);

   return MakeSplinePolygonVertex(0, LT_INTSPLINE, N, XOff, YOff, *CntrlN,
         *CntrlVs);
}

XPoint *MakeDoubleIntSplinePolygonVertex(N, CntrlN, CntrlVs, XOff, YOff,
      NumVs, Vs)
   int *N, *CntrlN, XOff, YOff, NumVs;
   DoublePoint **CntrlVs, *Vs;
{
   int x_off=0, y_off=0;
   int saved_tighter_splines=tighterStructSplines;
   XPoint *pxp=NULL;

   x_off = (zoomedIn ? XOff : (XOff>>zoomScale)<<zoomScale);
   y_off = (zoomedIn ? YOff : (YOff>>zoomScale)<<zoomScale);

   splineVs = NULL;

   if (NumVs <= 3) {
      double x, y;

      splineVs = (XPoint *)malloc(5*sizeof(XPoint));
      if (splineVs == NULL) {
         FailAllocMessage();
         *N = 0;
         return splineVs;
      }
      memset(splineVs, 0, 5*sizeof(XPoint));
      x = ZOOMED_DOUBLE_SIZE(Vs[0].x-((double)x_off));
      y = ZOOMED_DOUBLE_SIZE(Vs[0].y-((double)y_off));
      splineVs[0].x = round(x);
      splineVs[0].y = round(y);
      x = ZOOMED_DOUBLE_SIZE(Vs[1].x-((double)x_off));
      y = ZOOMED_DOUBLE_SIZE(Vs[1].y-((double)y_off));
      splineVs[1].x = round(x);
      splineVs[1].y = round(y);
      *N = *CntrlN = 2;
      return splineVs;
   }
   gaussIteration = 0;
   NumVs--; /* drop the duplicated end point */
   DoubleClosedSetupMatrix(NumVs, Vs);
   Gaussian(NumVs);
   *CntrlVs = DoubleClosedControlPts(NumVs, CntrlN);

   if (saved_tighter_splines) {
      tighterStructSplines = FALSE;
   }
   pxp = DoMakeDoubleIntSplinePolygonVertex(N, XOff, YOff, *CntrlN, *CntrlVs);
   tighterStructSplines = saved_tighter_splines;

   return pxp;
}

void DumpCurvedPolyPoints(FP, Curved, NumPts, V, Indent)
   FILE *FP;
   int Curved, NumPts, Indent;
   register IntPoint *V;
{
   register int j, i;
   double x1, y1, x2, y2;
   double mx1, my1, mx2, my2, mx3, my3, mx4, my4;
   int saved_tighter_splines=tighterStructSplines;

   if (Curved != LT_STRUCT_SPLINE && saved_tighter_splines) {
      tighterStructSplines = FALSE;
   }
   switch (NumPts) {
   case 0:
   case 1:
   case 2:
      break;
   case 3:
      mx1 = V->x; my1 = (V++)->y;
      x1 = V->x; y1 = (V++)->y;
      x2 = V->x; y2 = (V++)->y;
      mx2 = compCurveToFactor*mx1 + curveToFactor*x1;
      my2 = compCurveToFactor*my1 + curveToFactor*y1;
      mx3 = curveToFactor*x1 + compCurveToFactor*x2;
      my3 = curveToFactor*y1 + compCurveToFactor*y2;
      for (j = 0; j < Indent; j++) fprintf(FP, " ");
      fprintf(FP, "%.2f %.2f %.2f %.2f\n", mx2, my2, mx3, my3);
      break;
   default:
      if (tighterStructSplines) {
#ifdef _TGIF_DBG /* debug, do not translate */
         TgAssert(NumPts <= 4, "NumPts > 4 in DumpCurvedPolyPoints()", NULL);
#endif /* _TGIF_DBG */
         mx1 = V->x; my1 = (V++)->y;
         x1 = V->x;  y1 = (V++)->y;
         x2 = V->x;  y2 = (V++)->y;
         mx4 = V->x; my4 = V->y;
         for (j = 0; j < Indent; j++) fprintf(FP, " ");
         fprintf(FP, "%.2f %.2f %.2f %.2f\n", x1, y1, x2, y2);
      } else {
         mx1 = V->x; my1 = (V++)->y;
         x1 = V->x; y1 = (V++)->y;
         x2 = V->x; y2 = (V++)->y;
         mx2 = (mx1 + 2.0*x1) / 3.0; my2 = (my1 + 2.0*y1) / 3.0;
         mx3 = (5.0*x1 + x2) / 6.0; my3 = (5.0*y1 + y2) / 6.0;
         mx4 = (x1 + x2) / 2.0; my4 = (y1 + y2) / 2.0;
         for (j = 0; j < Indent; j++) fprintf(FP, " ");
         fprintf(FP, "%.2f %.2f %.2f %.2f %.2f %.2f %s\n",
               mx2, my2, mx3, my3, mx4, my4, gPsCmd[PS_CURVETO]);

         for (i=2; i < NumPts-2; i++, V++) {
            mx2 = (x1 + 5.0*x2) / 6.0; my2 = (y1 + 5.0*y2) / 6.0;
            x1 = x2; y1 = y2;
#ifdef stellar
            mx3 = (5.0*x1 + V->x) / 6.0; my3 = (5.0*y1 + V->y) / 6.0;
            mx4 = (x1 + V->x) / 2.0; my4 = (y1 + V->y) / 2.0;
#else
            x2 = V->x; y2 = V->y;
            mx3 = (5.0*x1 + x2) / 6.0; my3 = (5.0*y1 + y2) / 6.0;
            mx4 = (x1 + x2) / 2.0; my4 = (y1 + y2) / 2.0;
#endif
            for (j = 0; j < Indent; j++) fprintf(FP, " ");
            fprintf(FP, "%.2f %.2f %.2f %.2f %.2f %.2f %s\n",
                  mx2, my2, mx3, my3, mx4, my4, gPsCmd[PS_CURVETO]);
#ifdef stellar
            x2 = V->x; y2 = V->y;
#endif
         }
         mx2 = (x1 + 5.0*x2) / 6.0; my2 = (y1 + 5.0*y2) / 6.0;
         x1 = x2; y1 = y2;
         mx3 = (2.0*x1 + V->x) / 3.0; my3 = (2.0*y1 + V->y) / 3.0;
         for (j = 0; j < Indent; j++) fprintf(FP, " ");
         fprintf(FP, "%.2f %.2f %.2f %.2f\n", mx2, my2, mx3, my3);
      }
      break;
   }
   tighterStructSplines = saved_tighter_splines;
}

void DumpCurvedPolygonPoints(FP, Curved, NumPts, V, Indent)
   FILE *FP;
   int Curved, NumPts, Indent;
   register IntPoint *V;
{
   register int j;
   double mx2, my2, mx3, my3, mx4, my4, x1, y1, x2, y2;
   int i;
   int saved_tighter_splines=tighterStructSplines;

   if (Curved != LT_STRUCT_SPLINE && saved_tighter_splines) {
      tighterStructSplines = FALSE;
   }
   V[NumPts].x = V[1].x; V[NumPts].y = V[1].y;
   x1 = V->x;             y1 = (V++)->y;
   x2 = V->x;             y2 = (V++)->y;
   mx4 = (x1 + x2) / 2.0; my4 = (y1 + y2) / 2.0;
   for (j = 0; j < Indent; j++) fprintf(FP, " ");
   fprintf(FP, "%.2f %.2f %s\n", mx4, my4, gPsCmd[PS_MOVETO]);

   for (i=1; i < NumPts; i++, V++) {
      mx2 = (x1+5.0*x2)/6.0;   my2 = (y1+5.0*y2)/6.0;
      x1 = x2;                 y1 = y2;
#ifdef stellar
      mx3 = (5.0*x1+V->x)/6.0; my3 = (5.0*y1+V->y)/6.0;
      mx4 = (x1+V->x)/2.0;     my4 = (y1+V->y)/2.0;
#else
      x2 = V->x;               y2 = V->y;
      mx3 = (5.0*x1+x2)/6.0;   my3 = (5.0*y1+y2)/6.0;
      mx4 = (x1+x2)/2.0;       my4 = (y1+y2)/2.0;
#endif
      for (j = 0; j < Indent; j++) fprintf(FP, " ");
      fprintf(FP, "%.2f %.2f %.2f %.2f %.2f %.2f %s\n",
            mx2, my2, mx3, my3, mx4, my4, gPsCmd[PS_CURVETO]);
#ifdef stellar
      x2 = V->x;               y2 = V->y;
#endif
   }
   tighterStructSplines = saved_tighter_splines;
}

void DumpMultiCurvedPolyPoints(FP, Smooth, Style, Curved, NumPts, V, Indent)
   FILE *FP;
   char *Smooth;
   int Style, Curved, NumPts, Indent;
   register IntPoint *V;
{
   register int i, j;
   int segments=1, has_smooth_point=FALSE, start_index;

   if (Curved == LT_INTSPLINE || Smooth == NULL) {
      DumpCurvedPolyPoints(FP, Curved, NumPts, V, Indent);
      return;
   }
   if (Smooth[0] || Smooth[NumPts-1]) {
      FatalUnexpectedError(
            TgLoadCachedString(CSTID_CORRUPTED_POLY_DMPMULTICURVE),
            TgLoadCachedString(CSTID_FIX_ATTEMPTED));
      Smooth[0] = Smooth[NumPts-1] = FALSE;
   }
   for (i=1; i < NumPts-1; i++) {
      if (Smooth[i]) {
         has_smooth_point = TRUE;
      } else {
         segments++;
      }
   }
   if (!has_smooth_point) {
      /* simple polyline */
      if (Style & LS_RIGHT) {
         DumpPoints(FP, NumPts-1, V, Indent);
      } else {
         DumpPoints(FP, NumPts, V, Indent);
      }
      return;
   }
   if (segments == 1) {
      /* simple spline */
      if (Style & LS_RIGHT) {
         if (NumPts != 2) {
            DumpCurvedPolyPoints(FP, Curved, NumPts, V, Indent);
         } else {
            DumpPoints(FP, NumPts-1, V, Indent);
         }
      } else if (NumPts != 2) {
         DumpCurvedPolyPoints(FP, Curved, NumPts, V, Indent);
         for (i=0; i < Indent; i++) fprintf(FP, " ");
         fprintf(FP, "%1d %1d %s\n", V[NumPts-1].x, V[NumPts-1].y,
               gPsCmd[PS_CURVETO]);
      } else {
         DumpPoints(FP, NumPts, V, Indent);
      }
      return;
   }
   start_index = 0;
   for (i=1; i <= NumPts-1; i++) {
      if (!Smooth[i]) {
         int num_tmp_vs=i-start_index+1;

         if (num_tmp_vs == 2) {
            if (!(i == NumPts-1 && (Style & LS_RIGHT))) {
               DumpPoints(FP, num_tmp_vs, &V[start_index], Indent);
            }
         } else {
            DumpCurvedPolyPoints(FP, Curved, num_tmp_vs, &V[start_index],
                  Indent);
            if (!(i == NumPts-1 && (Style & LS_RIGHT))) {
               for (j=0; j < Indent; j++) fprintf(FP, " ");
               fprintf(FP, "%1d %1d %s\n", V[i].x, V[i].y, gPsCmd[PS_CURVETO]);
            }
         }
         start_index = i;
      }
   }
}

void DumpMultiCurvedPolygonPoints(FP, Smooth, Curved, NumPts, V, Indent)
   FILE *FP;
   char *Smooth;
   int Curved, NumPts, Indent;
   register IntPoint *V;
{
   register int i, j;
   int num_smooth_points=0, num_hinge_points=0, tmp_index;
   int start_index, once_around=FALSE;
   IntPoint *tmp_vs=NULL;

   if (Curved == LT_INTSPLINE || Smooth == NULL) {
      DumpCurvedPolygonPoints(FP, Curved, NumPts, V, Indent);
      return;
   }
   for (i=1; i < NumPts; i++) {
      if (Smooth[i]) {
         num_smooth_points++;
      } else {
         num_hinge_points++;
      }
   }
   if (num_smooth_points == 0) {
      /* simple polygon */
      for (j=0; j < Indent; j++) fprintf(FP, " ");
      fprintf(FP, "%1d %1d %s\n", V[0].x, V[0].y, gPsCmd[PS_MOVETO]);
      DumpPoints(FP, NumPts-1, V, Indent);
      return;
   }
   if (num_hinge_points == 0) {
      DumpCurvedPolygonPoints(FP, Curved, NumPts, V, Indent);
      return;
   }
   tmp_vs = (IntPoint*)malloc((NumPts+1)*sizeof(IntPoint));
   if (tmp_vs == NULL) FailAllocMessage();
   memset(tmp_vs, 0, (NumPts+1)*sizeof(IntPoint));

   for (i=0; i < NumPts; i++) {
      if (!Smooth[i]) {
         break;
      }
   }
   for (j=0; j < Indent; j++) fprintf(FP, " ");
   fprintf(FP, "%1d %1d %s\n", V[i].x, V[i].y, gPsCmd[PS_MOVETO]);
   start_index = i;
   tmp_vs[0].x = V[start_index].x;
   tmp_vs[0].y = V[start_index].y;
   tmp_index = 1;
   for (i=start_index+1; !(once_around && i==start_index+1); i++, tmp_index++) {
      tmp_vs[tmp_index].x = V[i].x;
      tmp_vs[tmp_index].y = V[i].y;
      if (!Smooth[i]) {
         if (tmp_index == 1) {
            DumpPoints(FP, tmp_index+1, tmp_vs, Indent);
         } else {
            DumpCurvedPolyPoints(FP, Curved, tmp_index+1, tmp_vs, Indent);
            for (j=0; j < Indent; j++) fprintf(FP, " ");
            fprintf(FP, "%1d %1d %s\n", V[i].x, V[i].y, gPsCmd[PS_CURVETO]);
         }
         start_index = (i==NumPts-1 ? 0 : i);
         tmp_vs[0].x = V[start_index].x;
         tmp_vs[0].y = V[start_index].y;
         tmp_index = 0;
      }
      if (i == NumPts-1) {
         i = 0;
         once_around = TRUE;
      }
   }
   if (tmp_vs != NULL) free(tmp_vs);
}

