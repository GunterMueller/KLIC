/* ---------------------------------------------------------- 
%   (C) 1994,1995 Institute for New Generation Computer Technology 
%       (Read COPYRIGHT for detailed information.) 
%   (C)1996, 1997, 1998, 1999 Japan Information Processing Development Center
%       (Read COPYRIGHT-JIPDEC for detailed information.)
----------------------------------------------------------- */

#include <stdlib.h>

char *
option_value(optname, dflt)
     char *optname;
     char *dflt;
{
  char *envval = getenv(optname);
  return (envval==0 ? dflt : envval);
}
