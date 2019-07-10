/* ---------------------------------------------------------- 
%   (C)1993,1994,1995 Institute for New Generation Computer Technology 
%       (Read COPYRIGHT for detailed information.) 
%   (C)1996, 1997, 1998, 1999 Japan Information Processing Development Center
%       (Read COPYRIGHT-JIPDEC for detailed information.)
----------------------------------------------------------- */

/* Reasons of interruption */
#define EXTEVENT	makeint(0)
#define FAILURE		makeint(1)

extern q *interrupt_goal();
int check_stack_for_alternatively();
struct goalrec *enqueue_goal();
struct goalrec *enqueue_after_waiting();
struct goalrec *enqueue_throw_goal();

#define enqueue_at_priority(p,q0,q)			\
{							\
  qp = enqueue_after_waiting(q0, (p), q, allocp, 0);	\
}

#define enqueue_at_priority_no_check(x,q0,q)		\
{							\
  long prio = intval(x);				\
  qp = enqueue_goal(q0, prio, q, glbl);			\
}

#define enqueue_at_lower_priority(p,q0,q)		\
{							\
  qp = enqueue_after_waiting(q0, (p), q, allocp, 1);	\
}

#define enqueue_at_lower_priority_no_check(x,q0,q)	\
{							\
  long prio = current_prio-intval(x);			\
  qp = enqueue_goal(q0, prio, q, glbl);			\
}

#define switch_on_pred()	switch (toppred->pred)

#define case_pred(p, label)	case (p): goto label;

#define last_case_pred(p, label) default: goto label;

#if ( defined(DIST) && defined(USE_REDUCTION_COUNT) )
#define loop_within_module(f)				\
{ \
  module (*func)(); \
  if (++reduction_count > REDUCTION_COUNTS) {heaplimit = 0; reduction_count = 0;}\
  if (allocp >= heaplimit) { \
    allocp = klic_interrupt(allocp, qp); \
    qp = current_queue; \
  } \
  if ((func = (toppred = qp->pred)->func) == (f)) \
    goto module_top; \
  heapp = allocp; \
  current_queue = qp; \
  return (module) func; \
}
#else
#define loop_within_module(f)				\
{ \
  module (*func)(); \
  if (allocp >= heaplimit) { \
    allocp = klic_interrupt(allocp, qp); \
    qp = current_queue; \
  } \
  if ((func = (toppred = qp->pred)->func) == (f)) \
    goto module_top; \
  heapp = allocp; \
  current_queue = qp; \
  return (module) func; \
}
#endif

#if ( defined(DIST) && defined(USE_REDUCTION_COUNT) )
#define execute(label)\
{\
  if (++reduction_count > 500) {heaplimit = 0; reduction_count = 0;}\
  if (allocp < heaplimit) goto label;\
}
#else
#define execute(label)\
{\
  if (allocp < heaplimit) goto label;\
}
#endif

#define proceed()		\
{				\
  goto proceed_label;		\
}

#define throw_goal(node, oldqp, goal) \
{ \
  qp = enqueue_throw_goal((node), (goal), (oldqp), allocp);\
}
