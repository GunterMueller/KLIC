/* ---------------------------------------------------------- 
%   (C)1993,1994 Institute for New Generation Computer Technology 
%       (Read COPYRIGHT for detailed information.) 
%   (C)1996, 1997, 1998, 1999 Japan Information Processing Development Center
%       (Read COPYRIGHT-JIPDEC for detailed information.)
----------------------------------------------------------- */
#include <sys/types.h>
#include <sys/times.h>
#include <klic/basic.h>

long blt_times()
{
  struct tms times_buf;
  if (times(&times_buf) < 0) {
    fatal("times failed");
  }
  return times_buf.tms_utime + times_buf.tms_stime;
}
