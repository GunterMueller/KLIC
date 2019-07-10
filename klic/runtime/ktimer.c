/* ---------------------------------------------------------- 
%   (C)1995 Institute for New Generation Computer Technology 
%       (Read COPYRIGHT for detailed information.) 
%   (C)1996, 1997, 1998, 1999 Japan Information Processing Development Center
%       (Read COPYRIGHT-JIPDEC for detailed information.)
----------------------------------------------------------- */

#include <klic/basic.h>
#ifdef USETIMER
#include <sys/time.h>
#include <klic/struct.h>
#include <klic/timer.h>
#include <klic/sighndl.h>

extern char *malloc_check();

void klic_timer_interrupt_handler()
{
  declare_globals;
  interrupt_off = 0;
  heaplimit = 0;
  signal_done = 0;
  signal_flags[SIGALRM] = 1;
}

struct timer_reservation_rec {
  struct timeval at;
  struct timeval interval;
  struct timer_reservation_rec *next;
  int (*handler)();
  q data;
};
static struct timer_reservation_rec *timer_reservations;
static struct timer_reservation_rec *timer_rsv_free;

static int process_timed_instantiation(allocp, data)
     q *allocp;
     q data;
{
  declare_globals;
  extern q *do_unify_value();

  heapp = do_unify_value(allocp, data, NILATOM);
  return 0;
}

static int process_simple_timer_handler(allocp, data)
     q *allocp;
     q data;
{
  declare_globals;

  return ((int (*)())((unsigned long)data-ATOMIC))(allocp, SIGALRM);
}

int
process_timer_interrupt(allocp, sig)
     q* allocp;
     long sig;
{
  declare_globals;
  struct timeval now;

  klic_gettod(&now);
  while (timer_reservations != 0 &&
	 NotLaterThan(timer_reservations->at, now)) {
    int result =
      timer_reservations-> handler(allocp, timer_reservations->data);
    if (result != 0) return result;
    allocp = heapp;
    if (TimeIsZero(timer_reservations->interval)) {
      struct timer_reservation_rec *tmp = timer_reservations->next;
      timer_reservations->next = timer_rsv_free;
      timer_rsv_free = timer_reservations;
      timer_reservations = tmp;
    } else {
      struct timer_reservation_rec **tmp, *trec;
      trec = timer_reservations;
      TimeAdd(trec->at, trec->interval, trec->at);
      timer_reservations = timer_reservations->next;
      for (tmp = &timer_reservations;
	   *tmp != 0 && NotLaterThan((*tmp)->at, trec->at);
	   tmp = &(*tmp)->next)
	;
      trec->next = *tmp;
      *tmp = trec;
    }
  }
  if (timer_reservations != 0) {
    call_at_specified_time(timer_reservations->at.tv_sec,
			   timer_reservations->at.tv_usec,
			   klic_timer_interrupt_handler);
  }
  heapp = allocp;
  return 0;
}


static q* gc_timer_data(allocp, ntop, otop, nsize, osize)
     q *allocp, *ntop, *otop;
     unsigned long nsize, osize;
{
  struct timer_reservation_rec *trec;
  extern q *copy_one_term();
  for (trec = timer_reservations;
       trec != 0;
       trec = trec->next) {
    allocp =
      copy_one_term(&trec->data, allocp, ntop, otop, nsize, osize);
  }
  return allocp;
}

void init_klic_timer_handling()
{
  timer_reservations = 0;
  timer_rsv_free = 0;
  register_gc_hook(gc_timer_data);
  add_slit_check_handler(SIGALRM, process_timer_interrupt);
}

static struct timer_reservation_rec *
alloc_timer_reservation_rec()
{
  struct timer_reservation_rec *newrec;
  if (timer_rsv_free == 0) {
    newrec = (struct timer_reservation_rec *)
      malloc_check(sizeof(struct timer_reservation_rec));
  } else {
    newrec = timer_rsv_free;
    timer_rsv_free = timer_rsv_free->next;
  }
  return newrec;
}

q *set_simple_interval_timer_handler(allocp, sec, usec, func)
     q *allocp;
     long sec, usec;
     int (*func)();
{
  declare_globals;
  struct timeval now;
  struct timer_reservation_rec *newrec, **tmp;

  klic_gettod(&now);
  newrec = alloc_timer_reservation_rec();
  newrec->at = now;
  TimeSet(newrec->interval, sec, usec);
  TimeAdd(newrec->at, newrec->interval, newrec->at);
  newrec->handler = process_simple_timer_handler;
  newrec->data = makeatomic(func);
  for (tmp = &timer_reservations;
       *tmp != 0 && NotLaterThan((*tmp)->at, newrec->at);
       tmp = &(*tmp)->next)
    ;
  newrec->next = *tmp;
  *tmp = newrec;
  process_timer_interrupt(allocp, SIGALRM);
  return heapp;
}  

q *reserve_klic_timer_interrupt(allocp, sec, usec, vrbl)
     q *allocp;
     long sec, usec;
     q vrbl;
{
  declare_globals;
  struct timer_reservation_rec *newrec, **tmp;

  newrec = alloc_timer_reservation_rec();
  TimeSet(newrec->at, sec, usec);
  TimeSet(newrec->interval, 0, 0);
  newrec->handler = process_timed_instantiation;
  newrec->data = vrbl;
  for (tmp = &timer_reservations;
       *tmp != 0 && NotLaterThan((*tmp)->at, newrec->at);
       tmp = &(*tmp)->next)
    ;
  newrec->next = *tmp;
  *tmp = newrec;
  process_timer_interrupt(allocp, SIGALRM);
  return heapp;
}
#endif
