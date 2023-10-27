/* ---------------------------------------------------------- 
%   (C)1993 Institute for New Generation Computer Technology 
%       (Read COPYRIGHT for detailed information.) 
%   (C)1996, 1997, 1998, 1999 Japan Information Processing Development Center
%       (Read COPYRIGHT-JIPDEC for detailed information.)
----------------------------------------------------------- */

/* functor staffs */
extern unsigned long *arities;
extern unsigned long *functors;
#define FUNCTORNUMBERBASE 0
#define functoratom(f) functors[symval(f) - FUNCTORNUMBERBASE]
#define arityof(f) arities[symval(f) - FUNCTORNUMBERBASE]

