/* ---------------------------------------------------------- 
%   (C)1993, 1995 Institute for New Generation Computer Technology 
%       (Read COPYRIGHT for detailed information.) 
%   (C)1996, 1997, 1998, 1999 Japan Information Processing Development Center
%       (Read COPYRIGHT-JIPDEC for detailed information.)
----------------------------------------------------------- */

#include <klic/config.h>

#ifdef MAIN
#define Extern
#define Init(x)		= x
#define Size(size)	size
#else
#define Extern		extern
#define Init(x)
#define Size(size)
#endif

#ifdef __STDC__
#define Const		const
#define Volatile	volatile
#else
#define Const
#define Volatile
#endif

#ifdef __GNUC__
#define Inline		__inline__
#define NoReturn	Volatile
#else
#define Inline
#define NoReturn
#endif

typedef void voidfn ();

#ifndef IN_DEBUG_C
extern NoReturn voidfn fatal;
extern NoReturn voidfn fatalf;
extern NoReturn voidfn fatalp;
extern void debug_printf();
#endif

/*
  PARALLEL flag is DIST || SHM.
*/
#ifdef DIST
#define PARALLEL
#endif
#ifdef SHM
#define PARALLEL
#endif

/*
  For handling I/O interrupts during I/O,
  we use our own version of getc &c
*/

extern int klic_getc();
extern int klic_fread();

extern int klic_putc();
extern int klic_fwrite();
extern int klic_fflush();

extern void debug_fprintf();

extern void klic_fprintf();

#define klic_local_fprintf fprintf
