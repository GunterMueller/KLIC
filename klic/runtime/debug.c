/* ---------------------------------------------------------- 
%   (C)1993,1994,1995 Institute for New Generation Computer Technology 
%       (Read COPYRIGHT for detailed information.) 
%   (C)1996, 1997, 1998, 1999 Japan Information Processing Development Center
%       (Read COPYRIGHT-JIPDEC for detailed information.)
----------------------------------------------------------- */
#include <stdio.h>
#include <setjmp.h>
#include <ctype.h>
#include <errno.h>
#define IN_DEBUG_C
#include <klic/basic.h>
#include <klic/struct.h>
#include <klic/primitives.h>
#include <klic/unify.h>
#include <klic/index.h>
#include <klic/atomstuffs.h>
#include <klic/functorstuffs.h>

#ifdef DIST
#include <klic/distio.h>
#include <klic/interpe.h>
#endif

#ifdef SHM
#include <klic/shm.h>
#endif

/*
  Function name convention:

  klic_fprintf, klic_printf, ...: for system tracer, user, i.e.
  redirect stdout, stderr to the IO process.
  debug_fprintf, debug_printf, ...: debugging aids, i.e.
  DO NOT redirect stdout, stderr.

  iosXXXX: equivalent to klic_fprintf(stderr, ...)
  ioXXXX: equivalent to klic_fprintf(stdout, ...)
  ioeXXXX: equivalent to klic_fprintf(stderr, ...)
  */

extern struct predicate topsucceed_pred;
extern jmp_buf klic_topmost;

void fatal(message)
     char *message;
{
  declare_globals;

  klic_fprintf(stderr, "Fatal Error: %s\n", message);

#ifdef DIST
  /* klic_fprintf(stderr, "Node %d Going to dump core\n", my_node); */

  /*  ERROR_STOP; */

  DetachIO();
  EmergencyStop();
#endif

  if (postmortem_pred !=0 && postmortem_args !=0) {
    longjmp(klic_topmost, 1);
  }
#ifdef DIST
  Close_net();
#endif
#ifdef SHM
  abend("");
#endif
  exit(-1);
}

klic_exit(N)
int N;
{
#ifdef DIST
  kill_child();
#endif
#ifdef SHM
  abend("");
#endif
  exit(N);
}

void fatalf(format, a0, a1, a2, a3, a4, a5, a6, a7)
     char *format;
     long a0, a1, a2, a3, a4, a5, a6, a7;
{
  char buf[4096];
  (void) sprintf(buf, format, a0, a1, a2, a3, a4, a5, a6, a7);
  fatal(buf);
}

void fatalp(where, format, a0, a1, a2, a3, a4, a5, a6, a7)
     char *where, *format;
     long a0, a1, a2, a3, a4, a5, a6, a7;
{
#ifdef DIST
  char buf[4096];
  char *bufp;
#ifdef DECL_SYS_ERRLIST
  extern char *sys_errlist();
#endif
  (void) sprintf(buf, "%s: %s\n", where, sys_errlist[errno]);
  bufp = buf + strlen(buf);
  (void) sprintf(bufp, format, a0, a1, a2, a3, a4, a5, a6, a7);
  fatal(buf);
#else
  perror(where);
  fatalf(format, a0, a1, a2, a3, a4, a5, a6, a7);
#endif
}

#ifdef DIST
/* inner functions for DISTRIBUTED version */
void 
iofprintf_inner(out, format, args)
FILE *out;
char *format;
va_list args;
{
  char io_buf[2048];
  declare_globals;
  vsprintf(io_buf, format, args);
  
  if ( Node_type() ) {
    if(out == stdout)
      Print(io_buf);
    else if(out == stderr)
      EPrint(io_buf);
    else
      fputs(io_buf, out);
  } else {
    fputs(io_buf, out );
  }
  va_end(args);
}

void
#ifdef USE_STDARG
iofprintf(FILE *out, char *format, ...)
#else
iofprintf(out, format, va_alist)
FILE *out;
char *format;
va_dcl
#endif
{
    va_list args;
    char io_buf[2048];
    declare_globals;

    VA_START(args, format);
    iofprintf_inner(out, format, args);
    va_end(args);
}


void
#ifdef USE_STDARG
ioprintf(char *format, ...)
#else
ioprintf(format, va_alist)
char *format;
va_dcl
#endif
{
    va_list args;
    char io_buf[2048];
    declare_globals;

    VA_START(args, format);
    iofprintf_inner(stdout, format, args);
    va_end(args);
}

void
#ifdef USE_STDARG
ioeprintf(char *format, ...)
#else
ioeprintf(format, va_alist)
char *format;
va_dcl
#endif
{
    va_list args;
    char io_buf[2048];
    declare_globals;

    VA_START(args, format);
    iofprintf_inner(stderr, format, args);
    va_end(args);
}

void
#ifdef USE_STDARG
iosprintf(char *format, ...)
#else
iosprintf(format, va_alist)
char *format;
va_dcl
#endif
{
    va_list args;
    char io_buf[2048];
    declare_globals;

    VA_START(args, format);
    iofprintf_inner(stderr, format, args);
    va_end(args);
}

static char *putbuf_fordist(redirect, bufp, buf, out)
int redirect;
char *bufp;
char *buf;
FILE *out;
{
  if(bufp != buf) {
    *bufp = '\0';
    if(redirect) {
      if(Node_type()) {
	/* If not IO node */
	if(out == stdout)
	  Print(buf);
	else if (out == stderr)
	  SystemPrint(buf);
	else
	  fputs(buf, out);
      } else {
	/* if IO node */
	fputs(buf, out);
      }
    } else {
      fputs(buf, out);
    }
    bufp = buf;
  }
  return bufp;
}
#endif  /* DIST */

#ifdef DECL_FPRINTF
#ifdef __STDC__
extern int fprintf(FILE *out, char *format, ...);
#else
extern int fprintf();
#endif
#endif

static Inline void 
klic_inner_fprintf(redirect, out, format,
		   a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, aa, ab)
     int redirect; /* if redirect != 0 -> redirect stdout/err */
     FILE *out;
     char *format;
     long a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, aa, ab;
{
  char *f;
#ifdef DIST
  char buf[4096];
  char *bufp = buf;
  declare_globals;
#endif

#ifdef __STDC__ 
  void (*fprintf_func)(FILE *out, char *format, ...);
#else
  void (*fprintf_func)();
#endif

  long args[12], *argp;
  args[0] = a0; args[1] = a1; args[2] = a2; args[3] = a3;
  args[4] = a4; args[5] = a5; args[6] = a6; args[7] = a7;
  args[8] = a8; args[9] = a9; args[10] = aa; args[11] = ab;
  argp = &args[0];

#ifdef DIST
  if(redirect) {
    fprintf_func = iofprintf;
  } else {
#endif
#ifdef __STDC__
    fprintf_func = (void (*)(FILE *, char *, ...))(fprintf);
#else
    fprintf_func = (void (*)())(fprintf);
#endif
#ifdef DIST
  }
#endif

  for (f = format; *f != '\0'; f++) {
    if (*f == '%') {
#ifdef DIST
      bufp = putbuf_fordist(redirect, bufp, buf, out);
#endif
      f++;
      switch (*f) {
      case 'k':
#ifdef DEBUGLIB
	fprint_partially(out, (q)*argp, 3, 10);
#else
	fprintf_func(out, "???");
#endif
	argp++;
	break;
      case 'F':
#ifdef DEBUGLIB
	(*fprintf_func)(out, "%s/%d",
			atomname[functors[*argp-FUNCTORNUMBERBASE]-
			      ATOMNUMBERBASE],
			arities[*argp++ -FUNCTORNUMBERBASE]);
#else
	(*fprintf_func)(out, "???/%d",
			arities[*argp++ -FUNCTORNUMBERBASE]);
#endif
	break;
      default:
	{
	  int k;
	  char fmtbuf[1024];
	  fmtbuf[0] = '%';
	  k=1;
	  while ((!isalpha(*f) || *f == 'l') && *f != 0) {
	    if (k>=1022) {
	      fatal("Too complicated format string for debug_printf");
	    }
	    fmtbuf[k++] = *f++;
	  }
	  fmtbuf[k++] = *f;
	  fmtbuf[k] = 0;
	  (*fprintf_func)(out, fmtbuf, *argp++);
	}
      }
    } else {
#ifdef DIST
      *bufp++ = *f;
#else
      klic_putc(*f, out);
#endif
    }
  }
#ifdef DIST
  bufp = putbuf_fordist(redirect, bufp, buf, out);
#endif
}

void
klic_fprintf(out, format,
		  a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, aa, ab)
     FILE *out;
     char *format;
     long a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, aa, ab;
{
  klic_inner_fprintf(1, out, format,
		     a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, aa, ab);
}

klic_printf(format,
	    a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, aa, ab)
     char *format;
     long a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, aa, ab;
{
  klic_inner_fprintf(1, stdout, format,
		     a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, aa, ab);
}

void
debug_fprintf(out, format,
		  a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, aa, ab)
     FILE *out;
     char *format;
     long a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, aa, ab;
{
  klic_inner_fprintf(0, out, format,
		     a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, aa, ab);
}


void debug_printf(format,
		  a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, aa, ab)
     char *format;
     long a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, aa, ab;
{
  debug_fprintf(stderr, format,
		a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, aa, ab);
}

