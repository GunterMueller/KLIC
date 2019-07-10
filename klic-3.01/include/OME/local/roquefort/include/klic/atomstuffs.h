/* ---------------------------------------------------------- 
%   (C)1993,1994 Institute for New Generation Computer Technology 
%       (Read COPYRIGHT for detailed information.) 
%   (C)1996, 1997, 1998, 1999 Japan Information Processing Development Center
%       (Read COPYRIGHT-JIPDEC for detailed information.)
----------------------------------------------------------- */

/* atomnumber staffs */
extern Unsigned char **atomname;
#define ATOMNUMBERBASE 2
#define functoratomname(f) atomname[functoratom(f) - ATOMNUMBERBASE]
#define namestringof(a) atomname[symval(a) - ATOMNUMBERBASE]
