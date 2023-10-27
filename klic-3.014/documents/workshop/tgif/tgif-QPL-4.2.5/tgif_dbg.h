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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/tgif_dbg.h,v 1.6 2011/05/16 16:22:00 william Exp $
 */

#ifndef _TGIF_DBG_H_

#ifdef _TGIF_DBG

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/cursorfont.h>
#include <X11/keysym.h>

extern void tgif_dbg_add_to_log ARGS_DECL((char*));
extern void tgif_dbg_enable ARGS_DECL((int));

extern void got_trigger ARGS_DECL((int));
extern void tgif_track ARGS_DECL((int));
extern void tgif_dbg_dump_count ARGS_DECL((char*));

extern int  gnAllocTrigger;
extern int  gnFreeTrigger;

extern void *tgif_malloc ARGS_DECL((size_t));
extern void *tgif_realloc ARGS_DECL((void*, size_t));
extern void tgif_free ARGS_DECL((void*));

extern size_t tgif_strlen ARGS_DECL((char*));

extern GC Tgif_XCreateGC ARGS_DECL((Display*, Drawable, unsigned long,
       XGCValues*));
extern void Tgif_XFreeGC ARGS_DECL((Display*, GC));

extern XImage *Tgif_XCreateImage ARGS_DECL((Display*, Visual*, unsigned int,
       int, int, char*, unsigned int, unsigned int, int, int));
extern XImage *Tgif_XGetImage ARGS_DECL((Display*, Drawable, int, int,
       unsigned int, unsigned int, unsigned long, int));
extern void Tgif_XDestroyImage ARGS_DECL((XImage*));

extern Cursor Tgif_XCreateFontCursor ARGS_DECL((Display*, unsigned int));
extern Cursor Tgif_XCreatePixmapCursor ARGS_DECL((Display*, Pixmap, Pixmap,
       XColor*, XColor*, unsigned int, unsigned int));
extern void Tgif_XFreeCursor ARGS_DECL((Display*, Cursor));

extern Pixmap Tgif_XCreatePixmap ARGS_DECL((Display*, Drawable, unsigned int,
       unsigned int, unsigned int));
extern Pixmap Tgif_XCreateBitmapFromData ARGS_DECL((Display*, Drawable,
       _Xconst char*, unsigned int, unsigned int));
extern int Tgif_XReadBitmapFile ARGS_DECL((Display*, Drawable,
       _Xconst char*, unsigned int *, unsigned int *, Pixmap*, int*, int*));
extern void Tgif_XFreePixmap ARGS_DECL((Display*, Pixmap));

extern char *Tgif_XFetchBytes ARGS_DECL((Display*, int*));
extern Status Tgif_XQueryTree ARGS_DECL((Display*, Window, Window*, Window*,
       Window**, unsigned int*));
extern char *Tgif_XGetAtomName ARGS_DECL((Display*, Atom));
extern Colormap *Tgif_XListInstalledColormaps ARGS_DECL((Display*, Window,
       int*));
extern void Tgif_XFree ARGS_DECL((void*));

/* -------------------- defines -------------------- */

#ifndef PRTGIF_NO_TGIF_DBG

#ifdef malloc
#undef malloc
#endif /* malloc */
#ifdef realloc
#undef realloc
#endif /* realloc */
#ifdef free
#undef free
#endif /* free */

#ifdef strlen
#undef strlen
#endif /* strlen */

#define malloc tgif_malloc
#define realloc tgif_realloc
#define free tgif_free

#define strlen tgif_strlen

#define XCreateGC Tgif_XCreateGC
#define XFreeGC Tgif_XFreeGC

#define XCreateImage Tgif_XCreateImage
#define XGetImage Tgif_XGetImage
#undef XDestroyImage
#define XDestroyImage Tgif_XDestroyImage

#define XCreateFontCursor Tgif_XCreateFontCursor
#define XCreatePixmapCursor Tgif_XCreatePixmapCursor
#define XFreeCursor Tgif_XFreeCursor

#define XCreatePixmap Tgif_XCreatePixmap
#define XCreateBitmapFromData Tgif_XCreateBitmapFromData
#define XReadBitmapFile Tgif_XReadBitmapFile
#define XFreePixmap Tgif_XFreePixmap

#define XFetchBytes Tgif_XFetchBytes
#define XQueryTree Tgif_XQueryTree
#define XGetAtomName Tgif_XGetAtomName
#define XListInstalledColormaps Tgif_XListInstalledColormaps
#define XFree Tgif_XFree

#endif /* ~PRTGIF_NO_TGIF_DBG */

#endif /* _TGIF_DBG */

#endif /* ~_TGIF_DBG_H_ */
