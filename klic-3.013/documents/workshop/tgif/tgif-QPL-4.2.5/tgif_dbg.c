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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/tgif_dbg.c,v 1.8 2011/05/16 16:22:00 william Exp $
 */
#define _INCLUDE_FROM_TGIF_DBG_C_

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/cursorfont.h>
#include <X11/keysym.h>

#ifdef _TGIF_DBG /* debug, do not translate */

#undef _Xconst
#define _Xconst

static char gszTraceFile[]="TGIF_DBG.LOG";

static FILE *gpTraceFile=NULL;

int gnAllocCount=0, gnFreeCount=0;
int gnCreateGCCount=0, gnFreeGCCount=0;
int gnCreateImageCount=0, gnDestroyImageCount=0;
int gnCreateCursorCount=0, gnFreeCursorCount=0;
int gnCreatePixmapCount=0, gnFreePixmapCount=0;
int gnCreateStringCount=0, gnFreeStringCount=0;

void tgif_dbg_add_to_log(pszString)
   char *pszString;
{
   if (gpTraceFile != NULL) {
      fprintf(gpTraceFile, "%%  %s\n", (pszString==NULL) ? "" : pszString);
   }
}

void tgif_dbg_enable(nEnable)
   int nEnable;
{
   static FILE *st_fp=NULL;

   if (nEnable) {
      gpTraceFile = st_fp;
   } else {
      st_fp = gpTraceFile;
      gpTraceFile = NULL;
   }
}

/* --------------- utility functions --------------- */

void got_trigger(nOne)
   int nOne;
{
   if (nOne) { }
}

void tgif_track(nBegin)
   int nBegin;
{
   switch (nBegin) {
   case 2:
      if (gpTraceFile != NULL && gpTraceFile != stderr) fclose(gpTraceFile);
      gpTraceFile = stderr;
      break;
   case 1:
      if (gpTraceFile != NULL) fclose(gpTraceFile);
      gpTraceFile = fopen(gszTraceFile, "w");
      if (gpTraceFile == NULL) {
         fprintf(stderr, "Cannot open '%s' for write.\n", gszTraceFile);
      } else {
         fprintf(stderr, "Set gnAllocTrigger for alloc().\n");
         fprintf(stderr, "Set gnFreeTrigger for free().\n");
      }
      break;
   case 0:
      if (gpTraceFile != NULL) {
         if (gpTraceFile != stderr) fclose(gpTraceFile);
         gpTraceFile = NULL;
      }
      break;
   }
   gnAllocCount = gnFreeCount = 0;
   gnCreateGCCount = gnFreeGCCount = 0;
   gnCreateImageCount = gnDestroyImageCount = 0;
   gnCreateCursorCount = gnFreeCursorCount = 0;
   gnCreatePixmapCount = gnFreePixmapCount = 0;
   gnCreateStringCount = gnFreeStringCount = 0;
}

void tgif_dbg_dump_count(msg)
   char *msg;
{
   if (gpTraceFile != NULL) {
      fprintf(gpTraceFile,
            "%% %s Next gnAllocCount is %1d and next gnFreeCount is %1d\n",
            msg, gnAllocCount+1, gnFreeCount+1);
   }
}

/* --------------- malloc, realloc, free --------------- */

int gnAllocTrigger=0, gnFreeTrigger=0;

void *tgif_malloc(nSize)
   size_t nSize;
{
   char *pVoid=malloc(nSize);

   if (gpTraceFile != NULL) {
      if (++gnAllocCount == gnAllocTrigger) {
         got_trigger(1);
      }
      fprintf(gpTraceFile, "a: %6d 0x%08x	%d\n",
            gnAllocCount, (unsigned int)pVoid, (unsigned int)(nSize));
   }
   return (void*)pVoid;
}

void *tgif_realloc(pVoid, nSize)
   void *pVoid;
   size_t nSize;
{
   char *pszVoid;

   if (gpTraceFile != NULL) {
      if (++gnFreeCount == gnFreeTrigger) {
         got_trigger(1);
      }
      fprintf(gpTraceFile, "f: %6d 0x%08x\n",
            gnFreeCount, (unsigned int)pVoid);
   }
   pszVoid = realloc(pVoid, nSize);
   if (gpTraceFile != NULL) {
      if (++gnAllocCount == gnAllocTrigger) {
         got_trigger(1);
      }
      fprintf(gpTraceFile, "a: %6d 0x%08x	%d\n",
            gnAllocCount, (unsigned int)pszVoid, (unsigned int)(nSize));
   }
   return (void*)pszVoid;
}

void tgif_free(pVoid)
   void *pVoid;
{
   if (gpTraceFile != NULL) {
      if (++gnFreeCount == gnFreeTrigger) {
         got_trigger(1);
      }
      fprintf(gpTraceFile, "f: %6d 0x%08x\n",
            gnFreeCount, (unsigned int)pVoid);
   }
   if (pVoid == NULL) {
      /* debug, do not translate */
      fprintf(stderr, "ERROR: free(NULL) called!\n");
      return;
   }
   free(pVoid);
}

size_t tgif_strlen(psz)
   char *psz;
{
   if (psz == NULL) {
      /* debug, do not translate */
      fprintf(stderr, "ERROR: strlen(NULL) called!\n");
   }
   return strlen(psz);
}

/* --------------- XCreateGC, XFreeGC --------------- */

int gnCreateGCTrigger=0, gnFreeGCTrigger=0;

GC Tgif_XCreateGC(display, d, valuemask, values)
   Display *display;
   Drawable d;
   unsigned long valuemask;
   XGCValues *values;
{
   GC gc=XCreateGC(display, d, valuemask, values);

   if (gpTraceFile != NULL) {
      if (++gnCreateGCCount == gnCreateGCTrigger) {
         got_trigger(1);
      }
      fprintf(gpTraceFile, "g: %6d 0x%08x	%ld\n",
            gnCreateGCCount, (unsigned int)gc, (long)valuemask);
   }
   return gc;
}

void Tgif_XFreeGC(display, gc)
   Display *display;
   GC gc;
{
   if (gpTraceFile != NULL) {
      if (++gnFreeGCCount == gnFreeGCTrigger) {
         got_trigger(1);
      }
      fprintf(gpTraceFile, "h: %6d 0x%08x\n",
            gnFreeGCCount, (unsigned int)gc);
   }
   XFreeGC(display, gc);
}

/* --------------- XCreateImage, XDestroyImage --------------- */

int gnCreateImageTrigger=0, gnDestroyImageTrigger=0;

XImage *Tgif_XCreateImage(display, visual, depth, format, offset, data,
      width, height, bitmap_pad, bytes_per_line)
   Display *display;
   Visual *visual;
   unsigned int depth;
   int format;
   int offset;
   char *data;
   unsigned int width;
   unsigned int height;
   int bitmap_pad;
   int bytes_per_line;
{
   XImage *ximage=XCreateImage(display, visual, depth, format, offset, data,
         width, height, bitmap_pad, bytes_per_line);

   if (gpTraceFile != NULL) {
      if (++gnCreateImageCount == gnCreateImageTrigger) {
         got_trigger(1);
      }
      fprintf(gpTraceFile, "i: %6d 0x%08x	%d\n",
            gnCreateImageCount, (unsigned int)ximage,
            (unsigned int)(width*height));
   }
   return ximage;
}

XImage *Tgif_XGetImage(display, d, x, y, width, height, plane_mask, format)
   Display *display;
   Drawable d;
   int x;
   int y;
   unsigned int width;
   unsigned int height;
   unsigned long plane_mask;
   int format;
{
   XImage *ximage=XGetImage(display, d, x, y, width, height, plane_mask,
         format);

   if (gpTraceFile != NULL) {
      if (++gnCreateImageCount == gnCreateImageTrigger) {
         got_trigger(1);
      }
      fprintf(gpTraceFile, "i: %6d 0x%08x	%d\n",
            gnCreateImageCount, (unsigned int)ximage,
            (unsigned int)(width*height));
   }
   return ximage;
}

void Tgif_XDestroyImage(ximage)
   XImage *ximage;
{
   if (gpTraceFile != NULL) {
      if (++gnDestroyImageCount == gnDestroyImageTrigger) {
         got_trigger(1);
      }
      fprintf(gpTraceFile, "j: %6d 0x%08x\n",
            gnDestroyImageCount, (unsigned int)ximage);
   }
   (*(ximage->f.destroy_image))(ximage);
}

/* --------------- XCreateFontCursor, XFreeGC --------------- */

int gnCreateCursorTrigger=0, gnFreeCursorTrigger=0;

Cursor Tgif_XCreateFontCursor(display, shape)
   Display *display;
   unsigned int shape;
{
   Cursor cursor=XCreateFontCursor(display, shape);

   if (gpTraceFile != NULL) {
      if (++gnCreateCursorCount == gnCreateCursorTrigger) {
         got_trigger(1);
      }
      fprintf(gpTraceFile, "c: %6d 0x%08x	%d\n",
            gnCreateCursorCount, (unsigned int)cursor, shape);
   }
   return cursor;
}

Cursor Tgif_XCreatePixmapCursor(display, source, mask, foreground_color,
      background_color, x, y)
   Display *display;
   Pixmap source;
   Pixmap mask;
   XColor *foreground_color;
   XColor *background_color;
   unsigned int x;
   unsigned int y;
{
   Cursor cursor=XCreatePixmapCursor(display, source, mask, foreground_color,
         background_color, x, y);

   if (gpTraceFile != NULL) {
      if (++gnCreateCursorCount == gnCreateCursorTrigger) {
         got_trigger(1);
      }
      fprintf(gpTraceFile, "c: %6d 0x%08x	%d\n",
            gnCreateCursorCount, (unsigned int)cursor, (unsigned int)source);
   }
   return cursor;
}

void Tgif_XFreeCursor(display, cursor)
   Display *display;
   Cursor cursor;
{
   if (gpTraceFile != NULL) {
      if (++gnFreeCursorCount == gnFreeCursorTrigger) {
         got_trigger(1);
      }
      fprintf(gpTraceFile, "d: %6d 0x%08x\n",
            gnFreeCursorCount, (unsigned int)cursor);
   }
   XFreeCursor(display, cursor);
}

/* --------------- XCreatePixmap, XFreeGC --------------- */

int gnCreatePixmapTrigger=0, gnFreePixmapTrigger=0;

Pixmap Tgif_XCreatePixmap(display, d, width, height, depth)
   Display *display;
   Drawable d;
   unsigned int width;
   unsigned int height;
   unsigned int depth;
{
   Pixmap pixmap=XCreatePixmap(display, d, width, height, depth);

   if (gpTraceFile != NULL) {
      if (++gnCreatePixmapCount == gnCreatePixmapTrigger) {
         got_trigger(1);
      }
      fprintf(gpTraceFile, "p: %6d 0x%08x	%d\n",
            gnCreatePixmapCount, (unsigned int)pixmap, width*height*(depth>>3));
   }
   return pixmap;
}

Pixmap Tgif_XCreateBitmapFromData(display, d, data, width, height)
   Display *display;
   Drawable d;
   _Xconst char *data;
   unsigned int width;
   unsigned int height;
{
   Pixmap pixmap=XCreateBitmapFromData(display, d, data, width, height);

   if (gpTraceFile != NULL) {
      if (++gnCreatePixmapCount == gnCreatePixmapTrigger) {
         got_trigger(1);
      }
      fprintf(gpTraceFile, "p: %6d 0x%08x	%d\n",
            gnCreatePixmapCount, (unsigned int)pixmap, width*height);
   }
   return pixmap;
}

int Tgif_XReadBitmapFile(display, d, filename, width_return, height_return,
      bitmap_return, x_hot_return, y_hot_return)
   Display *display;
   Drawable d;
   _Xconst char *filename;
   unsigned int *width_return;
   unsigned int *height_return;
   Pixmap *bitmap_return;
   int *x_hot_return, *y_hot_return;
{
   int rc=XReadBitmapFile(display, d, filename, width_return, height_return,
         bitmap_return, x_hot_return, y_hot_return);

   if (gpTraceFile != NULL) {
      if (++gnCreatePixmapCount == gnCreatePixmapTrigger) {
         got_trigger(1);
      }
      fprintf(gpTraceFile, "p: %6d 0x%08x	%d\n",
            gnCreatePixmapCount,
            (bitmap_return==NULL || rc != BitmapSuccess) ? 0 :
            (unsigned int)(*bitmap_return),
            (width_return==NULL || height_return==NULL) ? 0 :
            (int)((*width_return)*(*height_return)));
   }
   return rc;
}

void Tgif_XFreePixmap(display, pixmap)
   Display *display;
   Pixmap pixmap;
{
   if (gpTraceFile != NULL) {
      if (++gnFreePixmapCount == gnFreePixmapTrigger) {
         got_trigger(1);
      }
      fprintf(gpTraceFile, "q: %6d 0x%08x\n",
            gnFreePixmapCount, (unsigned int)pixmap);
   }
   XFreePixmap(display, pixmap);
}

/* ----------- XFetchBytes, XQueryTree, XGetAtomName, XFreeString ----------- */

int gnCreateStringTrigger=0, gnFreeStringTrigger=0;

char *Tgif_XFetchBytes(display, nbytes_return)
   Display *display;
   int *nbytes_return;
{
   char *pszString=XFetchBytes(display, nbytes_return);

   if (gpTraceFile != NULL) {
      if (++gnCreateStringCount == gnCreateStringTrigger) {
         got_trigger(1);
      }
      fprintf(gpTraceFile, "s: %6d 0x%08x	%d\n",
            gnCreateStringCount, (unsigned int)pszString,
            nbytes_return == NULL ? 0 : (*nbytes_return));
   }
   return pszString;
}

Status Tgif_XQueryTree(display, w, root_return, parent_return, children_return,
      nchildren_return)
   Display *display;
   Window w;
   Window *root_return;
   Window *parent_return;
   Window **children_return;
   unsigned int *nchildren_return;
{
   Status status=XQueryTree(display, w, root_return, parent_return,
         children_return, nchildren_return);

   if (gpTraceFile != NULL && children_return != NULL &&
         (*children_return) != NULL) {
      if (++gnCreateStringCount == gnCreateStringTrigger) {
         got_trigger(1);
      }
      fprintf(gpTraceFile, "s: %6d 0x%08x	%d\n",
            gnCreateStringCount, (unsigned int)(*children_return),
            (unsigned int)w);
   }
   return status;
}

char *Tgif_XGetAtomName(display, atom)
   Display *display;
   Atom atom;
{
   char *pszString=XGetAtomName(display, atom);

   if (gpTraceFile != NULL) {
      if (++gnCreateStringCount == gnCreateStringTrigger) {
         got_trigger(1);
      }
      fprintf(gpTraceFile, "s: %6d 0x%08x	%d\n",
            gnCreateStringCount, (unsigned int)pszString, (unsigned int)atom);
   }
   return pszString;
}

Colormap *Tgif_XListInstalledColormaps(display, w, num_return)
   Display *display;
   Window w;
   int *num_return;
{
   Colormap *pColormap=XListInstalledColormaps(display, w, num_return);

   if (gpTraceFile != NULL) {
      if (++gnCreateStringCount == gnCreateStringTrigger) {
         got_trigger(1);
      }
      fprintf(gpTraceFile, "s: %6d 0x%08x	%d\n",
            gnCreateStringCount, (unsigned int)pColormap,
            (unsigned int)sizeof(Colormap*));
   }
   return pColormap;
}

void Tgif_XFree(pVoid)
   void *pVoid;
{
   if (gpTraceFile != NULL) {
      if (++gnFreeStringCount == gnFreeStringTrigger) {
         got_trigger(1);
      }
      fprintf(gpTraceFile, "t: %6d 0x%08x\n",
            gnFreeStringCount, (unsigned int)pVoid);
   }
   XFree(pVoid);
}

#endif /* _TGIF_DBG */

