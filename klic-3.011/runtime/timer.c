/* ---------------------------------------------------------- 
%   (C)1995 Institute for New Generation Computer Technology 
%       (Read COPYRIGHT for detailed information.) 
%   (C)1996, 1997, 1998, 1999 Japan Information Processing Development Center
%       (Read COPYRIGHT-JIPDEC for detailed information.)
----------------------------------------------------------- */

#include <klic/basic.h>
#ifdef USETIMER
#include <signal.h>
#include <sys/time.h>
#include <klic/timer.h>

extern char *malloc_check();

static struct klic_timer_wait_rec * Volatile
  timer_wait_root = 0;
static struct klic_timer_wait_rec * Volatile
  timer_wait_free = 0;

static Inline void
insert_timer_queue(newtwr)
     struct klic_timer_wait_rec *newtwr;
{
  struct klic_timer_wait_rec * Volatile *twr;
  for (twr = &timer_wait_root;
       *twr != 0 && NotLaterThan((*twr)->on_at, newtwr->on_at);
       twr = &(*twr)->next) {
  }
  newtwr->next = *twr;
  *twr = newtwr;
}

static void restart_timer(now)
     struct timeval *now;
{
  struct itimerval itmp;
  struct timeval margin;
  TimeSub(timer_wait_root->on_at, *now, itmp.it_value);
  TimeSet(itmp.it_interval, 0, 0);
  if (setitimer(ITIMER_REAL, &itmp, 0) != 0) {
    fatalp("setitimer", "Error in restarting timer");
  }
}

void klic_gettod(now)
     struct timeval *now;
{
  if (gettimeofday(now, 0) != 0) {
    fatalp("gettimeofday", "Error in getting date and time");
  }
}

static void
process_time_expired_events()
{
  struct timeval now;

  klic_gettod(&now);
  while (timer_wait_root != 0 &&
	 NotLaterThan(timer_wait_root->on_at, now)) {
    struct klic_timer_wait_rec *tmp;
    timer_wait_root->func();
    tmp = timer_wait_root;
    timer_wait_root = tmp->next;
    if (TimeIsZero(tmp->interval)) {
      tmp->next = timer_wait_free;
      timer_wait_free = tmp;
    } else {
      TimeAdd(tmp->on_at, tmp->interval, tmp->on_at);
      insert_timer_queue(tmp);
    }
  }
  if (timer_wait_root != 0) {
    restart_timer(&now);
  }
}

static void timer_expiration_handler()
{
  process_time_expired_events();
}

static void
schedule_timer_interrupt(on_at, interval, func)
     struct timeval *on_at, *interval;
     void (*func)();
{
  struct timeval now;
  struct timeval at;

  sigset_t original, blocking;
  struct klic_timer_wait_rec *tmp;

  at = *on_at;
  sigemptyset(&blocking);
  sigaddset(&blocking, SIGALRM);
  if (sigprocmask(SIG_BLOCK, &blocking, &original) != 0) {
    fatalp("sigprocmask", "Error in setting signal mask");
  }
  if (timer_wait_free == 0) {
    tmp = (struct klic_timer_wait_rec *)
      malloc_check(sizeof(struct klic_timer_wait_rec));
  } else {
    tmp = timer_wait_free;
    timer_wait_free = timer_wait_free->next;
  }
  tmp->on_at = at;
  tmp->func = func;
  tmp->interval = *interval;

  insert_timer_queue(tmp);
  process_time_expired_events();
  if (sigprocmask(SIG_SETMASK, &original, (sigset_t *)0) != 0) {
    fatalp("sigprocmask", "Error in restoring signal mask");
  }
}

void
call_at_specified_time(sec, usec, func)
     long sec, usec;
     void (*func)();
{
  struct timeval at, interval;
  TimeSet(at, sec, usec);
  TimeSet(interval, 0, 0);
  schedule_timer_interrupt(&at, &interval, func);
}

void
call_after_specified_interval(sec, usec, func)
     long sec, usec;
     void (*func)();
{
  struct timeval now, at, interval;
  klic_gettod(&now);
  TimeSet(at, sec, usec);
  TimeAdd(at, now, at);
  TimeSet(interval, 0, 0);
  schedule_timer_interrupt(&at, &interval, func);
}

void
call_at_time_intervals(sec, usec, func)
     long sec, usec;
     void (*func)();
{
  struct timeval now, at, interval;
  klic_gettod(&now);
  TimeSet(at, sec, usec);
  TimeAdd(at, now, at);
  TimeSet(interval, sec, usec);
  schedule_timer_interrupt(&at, &interval, func);
}

void
init_virtualized_timer()
{
  struct sigaction act;

  act.sa_handler = timer_expiration_handler;  
  sigemptyset(&act.sa_mask);
  sigaddset(&act.sa_mask, SIGALRM);
  act.sa_flags = 0;
  if (sigaction(SIGALRM, &act, (struct sigaction *)0) != 0) {
    fatalp("sigation", "Error in initiating handler for SIGALRM");
  }
}
#endif
