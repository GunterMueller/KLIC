/* ---------------------------------------------------------- 
%   (C) 1995 Institute for New Generation Computer Technology 
%       (Read COPYRIGHT for detailed information.) 
%   (C)1996, 1997, 1998, 1999 Japan Information Processing Development Center
%       (Read COPYRIGHT-JIPDEC for detailed information.)
----------------------------------------------------------- */
#include <klic/basic.h>

#include <stdio.h>
#include <errno.h>

#ifdef STDDEFH
#include <stddef.h>
#else
#ifndef _SIZE_T
typedef unsigned int   size_t;
#define _SIZE_T
#endif
#endif

#ifdef DIST
#include <klic/distio.h>
#endif

Inline int debug_getc(f)
     FILE *f;
{
  while (1) {
    int result = getc(f);
    if (result!=EOF || feof(f) || errno != EINTR) {
      return result;
    }
  }
}

Inline int debug_fread(pt, size, items, f)
     void *pt;
     size_t size, items;
     FILE *f;
{
  return fread(pt, size, items, f);
}

char *debug_fgets(buf, length, f)
     char *buf;
     size_t length;
     FILE *f;
{
  while (1) {
    char *result = fgets(buf, length, f);
    if (result!=NULL || feof(f) || errno != EINTR) {
      return result;
    }
  }
}

#if 0
extern int debug_putc(c, f)
     int c;
     FILE *f;
{
  while (1) {
    int result = putc(c, f);
    if (result!=EOF || feof(f) || errno != EINTR) {
      return result;
    }
  }
}
#endif

Inline int debug_fputc(c, f)
int c;
FILE *f;
{
  while(1) {
    int result = fputc(c, f);
    if(result != EOF || feof(f) || errno != EINTR) {
      return result;
    }
  }
}

Inline int debug_fwrite(pt, size, items, f)
     Const void *pt;
     size_t size, items;
     FILE *f;
{
  return fwrite(pt, size, items, f);
}

Inline int debug_fflush(f)
     FILE *f;
{
  while (1) {
    int result = fflush(f);
    if (result!=EOF || errno != EINTR) {
      return result;
    }
  }
}

/*
  klic_XXXX redirect stdout/err
*/

int klic_getc(f)
FILE *f;
{
#ifdef DIST
  if(f == stdin) {
    return UserGetc();
  }
#endif
  return debug_getc(f);
}

int klic_fread(pt, size, items, f)
     void *pt;
     size_t size;
     size_t items;
     FILE *f;
{
#ifdef DIST
  /*
    This code flagment must be rewritten using fread...
    */
  if(f == stdin) {
    return Read(pt);
  }
#endif
  return debug_fread(pt, size, items, f);
}

char *klic_fgets(buf, len, f)
     char *buf;
     size_t len;
     FILE *f;
{
#ifdef DIST
  if(f == stdin)
    SystemRead(buf);
    return buf;
#endif
  return debug_fgets(buf, len, f);
}

int klic_putc(c, out)
int c;
FILE *out;
{
#ifdef DIST
  if(out == stdout)
    return UserPutc(c);
  else if(out == stderr)
    return SystemPutc(c);
#endif
  return debug_fputc(c, out);
}

int
klic_fwrite(pt, size, items, f)
     Const void *pt;
     size_t size, items;
     FILE *f;
{
#ifdef DIST
  if(f == stdout)
    return UserWrite(pt, size * items);
  else if(f == stderr)
    return SystemWrite(pt, size * items);
#endif
  return debug_fwrite(pt, size, items, f);
}

int
klic_fflush(f)
     FILE *f;
{
#ifdef DIST
  if(f == stdout)
    return UserFflush();
  else if(f == stderr)
    return SystemFflush();
#endif
  return debug_fflush(f);
}

