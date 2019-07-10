/* ---------------------------------------------------------- 
%   (C)1993, 1994 Institute for New Generation Computer Technology 
%       (Read COPYRIGHT for detailed information.) 
%   (C)1996, 1997, 1998, 1999 Japan Information Processing Development Center
%       (Read COPYRIGHT-JIPDEC for detailed information.)
----------------------------------------------------------- */
#include <sys/types.h>

#define  field_diff(field)	(after.field - before.field)

#ifdef GETRUSAGE

#include <sys/time.h>
#include <sys/resource.h>

#define  diff_usec(field)\
(field_diff(field.tv_sec) * 1000000 + field_diff(field.tv_usec))

#define	 timerstruct	struct rusage
#define	 measure(x)	getrusage(RUSAGE_SELF, &(x))

#else 

#include <sys/times.h>
#include <sys/param.h>

#ifndef HZ
#define HZ 60
#endif

#define	 tick2msec(n)	((n)*1000.0/HZ)

#define	 timerstruct	struct tms
#define	 measure(x)	times(&x)

#endif

Extern int gctimes Init(0);
Extern int gcums Init(0);
Extern int gcsms Init(0);
Extern int measure_gc Init(0);
