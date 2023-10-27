/* ---------------------------------------------------------- 
%   (C)1993,1994,1995 Institute for New Generation Computer Technology 
%       (Read COPYRIGHT for detailed information.) 
%   (C)1996, 1997, 1998, 1999 Japan Information Processing Development Center
%       (Read COPYRIGHT-JIPDEC for detailed information.)
----------------------------------------------------------- */
#ifdef DIST
#define TRACE_ATTACHIO()	AttachIO()
#define TRACE_DETACHIO()	DetachIO()
#else
# ifdef SHM
#define TRACE_ATTACHIO() \
  {if (!shm_trace_lock) {s_lock(tr_key()); shm_trace_lock=1;}}
#define TRACE_DETACHIO() \
  { shm_trace_lock=0; s_unlock(tr_key()); }
# else
#define TRACE_ATTACHIO()
#define TRACE_DETACHIO()
# endif
#endif
