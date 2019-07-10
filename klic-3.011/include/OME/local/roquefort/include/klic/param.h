/* ---------------------------------------------------------- 
%   (C)1993 Institute for New Generation Computer Technology 
%       (Read COPYRIGHT for detailed information.) 
%   (C)1996, 1997, 1998, 1999 Japan Information Processing Development Center
%       (Read COPYRIGHT-JIPDEC for detailed information.)
----------------------------------------------------------- */

#define HEAPSIZE	24576L	/* default heap size (in words) */
#define INCREMENTSIZE	1024L	/* reserved heap size (in words) */
#define MAXSUSPENSION	1024L	/* how many suspension reasons can exist */
#define MAXSTDARGS	64	/* maximum number of arguments handled by
				   tracer or suspension for priority value */
#define MAXGENERICARGS	64	/* maximum number of arguments for
				   generic methods*/
#define MAXKLICINT	((1L<<(sizeof(long)*8-ATAGBITS-1))-1)
				/* maximum KLIC integer */
#define MINKLICINT	(-(1L<<(sizeof(long)*8-ATAGBITS-1)))
				/* minimum KLIC integer */
#define HIGHESTPRIO	MAXKLICINT
				/* highest priority */
#define PRIOQRECBULK	64	/* how many priority queue records to */
				/* allocate at a time */
#define GCSTACKSIZE	1024L	/* initial GC stack size */
