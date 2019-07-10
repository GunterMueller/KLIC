/* ---------------------------------------------------------- 
%   (C)1995 Institute for New Generation Computer Technology 
%       (Read COPYRIGHT for detailed information.) 
%   (C)1996, 1997, 1998, 1999 Japan Information Processing Development Center
%       (Read COPYRIGHT-JIPDEC for detailed information.)
----------------------------------------------------------- */
struct klic_timer_wait_rec {
  struct klic_timer_wait_rec *next;
  struct timeval on_at;
  struct timeval interval;
  void (*func)();
};

#define NotLaterThan(x, y) 				\
((x).tv_sec < (y).tv_sec ||				\
 ((x).tv_sec == (y).tv_sec && (x).tv_usec <= (y).tv_usec))

#define Million 1000000

#define TimeSet(x, s, u)				\
{ (x).tv_sec = (s); (x).tv_usec = (u); }

#define TimeIsZero(x)					\
((x).tv_sec == 0 && (x).tv_usec == 0)

#define TimeSub(x, y, z)				\
if ((x).tv_usec-(y).tv_usec >= 0) {			\
  (z).tv_sec = (x).tv_sec - (y).tv_sec;			\
  (z).tv_usec = (x).tv_usec - (y).tv_usec;		\
} else {						\
  (z).tv_sec = (x).tv_sec - (y).tv_sec - 1;		\
  (z).tv_usec = Million + (x).tv_usec - (y).tv_usec;	\
}

#define TimeAdd(x, y, z)				\
if ((x).tv_usec+(y).tv_usec < Million) {		\
  (z).tv_sec = (x).tv_sec + (y).tv_sec;			\
  (z).tv_usec = (x).tv_usec + (y).tv_usec;		\
} else {						\
  (z).tv_sec = (x).tv_sec + (y).tv_sec + 1;		\
  (z).tv_usec = (x).tv_usec + (y).tv_usec - Million;	\
}

extern void
  klic_gettod(),
  call_at_specified_time(),
  call_after_specified_interval(),
  call_at_time_intervals(),
  init_virtualized_timer();
