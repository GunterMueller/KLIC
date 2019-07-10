/* ----------------------------------------------------------
%   (C)1993,1994,1995 Institute for New Generation Computer Technology
%       (Read COPYRIGHT for detailed information.)
%   (C)1996, 1997, 1998, 1999 Japan Information Processing Development Center
%       (Read COPYRIGHT-JIPDEC for detailed information.)
----------------------------------------------------------- */
#include <klic/basic.h>
#include <klic/struct.h>
#include <klic/sighndl.h>
#include <klic/primitives.h>
#include <klic/schedule.h>
#ifdef DIST
#include <klic/rmon.h>
#else
#define GC_ON()
#define GC_OFF()
#endif
#ifdef SHM
#include <klic/shm.h>
#endif

int sigint_interrupt = 0;

static struct goalrec *enqueue_resumed_goals(qp)
     struct goalrec *qp;
{
  declare_globals;
  struct goalrec *rsmg = resumed_goals;
  if (rsmg != 0) {
    struct goalrec *newqp = rsmg->next;
    rsmg->next = qp;
    qp = newqp;
    resumed_goals = 0;
    if (interrupt_off) heaplimit = real_heaplimit;
  }
  return qp;
}

q *klic_interrupt(allocp, qp)
     q *allocp;
     struct goalrec *qp;
{
  declare_globals;
  long retry;
  extern q *klic_signal_handler();
#ifdef DEBUGLIB
  extern int trace_flag;
  extern struct goalrec *trace_after();
#endif

  qp = enqueue_resumed_goals(qp);

#ifdef DEBUGLIB
  {
    extern int trace_flag, stepping_flag;
    extern struct goalrec *trace_after(), *step_after();
    if (stepping_flag) {
      qp = step_after(qp, allocp);
      allocp = heapp;
    } else if (trace_flag) {
      qp = trace_after(qp, allocp);
      allocp = heapp;
    }
  }
#endif

  do {
    retry = 0;

    /* signal handling */
#ifdef USESIG
    if (!signal_done) {
      allocp = klic_signal_handler(allocp);
      retry = (allocp + this_more_space >= real_heaplimit);
      /* some goals may have been resumed by the signal handler */
      qp = enqueue_resumed_goals(qp);
    }
#endif
#if defined(DIST) && defined(USE_REDUCTION_COUNT)
    {
	int again = receive_message(allocp, 0);
	if (again == 0)
	    heaplimit = real_heaplimit;
	allocp = heapp;
	retry = (allocp + this_more_space >= real_heaplimit);
     /* some goals may have been resumed by the message reception */
     qp = enqueue_resumed_goals(qp);
    }
#endif
    if (sigint_interrupt) {
      fatal("Execution aborted by SIGINT");
    }

#ifdef SHM
    while( (Volatile int)ext_queued ) {
      *cur_status = 1;
      ext_queued = 0;
      klic_barrier();
      qp = get_invoked_goals(qp);
      interrupt_off = -1;
      heaplimit = real_heaplimit;
      retry |= (allocp + this_more_space >= heaplimit);
    }
#endif

    /* priority support */
    if (higher_priority_goal) {
      higher_priority_goal = 0;
      put_priority_queue(qp, current_prio);
      qp = get_top_priority_queue();
      if (interrupt_off) heaplimit = real_heaplimit;
    }

#ifdef SHM
    if (ck_shm_gc() || allocp + this_more_space >= real_heaplimit) {
      extern struct goalrec *klic_gc();
      *cur_status = 3;
    /* ck_shm_gc will call another GC */
#else
    if (allocp + this_more_space >= real_heaplimit) {
      extern struct goalrec *klic_gc();
#endif
      GC_ON();
      qp = klic_gc(qp);
#ifdef SHM
      *cur_status = 1;
#endif
      GC_OFF();

      allocp = heapp;
    }
  } while (retry);

  current_queue = qp;
  return allocp;
}
