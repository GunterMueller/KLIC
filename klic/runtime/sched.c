/* ---------------------------------------------------------- 
%   (C)1993, 1994, 1995 Institute for New Generation Computer Technology 
%       (Read COPYRIGHT for detailed information.) 
%   (C)1996, 1997, 1998, 1999 Japan Information Processing Development Center
%       (Read COPYRIGHT-JIPDEC for detailed information.)
----------------------------------------------------------- */
#include <klic/basic.h>
#include <klic/struct.h>
#include <klic/alloc.h>

#include <stdio.h>
#ifdef DEBUGLIB
#include <klic/trace.h>
#endif
#ifdef SHM
#include <setjmp.h>
#include <klic/shm.h>
extern jmp_buf klic_topmost;
#endif

extern char *malloc_check();

#define new_prioqrec(pqr)			\
{						\
  struct prioqrec *temp;			\
  while ((temp = prioqrec_free) == 0) {		\
    prioqrec_free = more_prioqrec();		\
  }						\
  prioqrec_free = temp->next;			\
  (pqr) = temp;					\
}

#define free_prioqrec(pqr)			\
{						\
  pqr->next = prioqrec_free;			\
  prioqrec_free = pqr;				\
}

struct prioqrec *prioqrec_free = 0;

struct prioqrec *more_prioqrec()
{
  unsigned long k;
  extern char *calloc();
  struct prioqrec *bulk =
    (struct prioqrec *)
      calloc(PRIOQRECBULK, sizeof(struct prioqrec));
  for (k = PRIOQRECBULK-1; k != 0; k--) {
    bulk[k-1].next = &bulk[k];
  }
  bulk[PRIOQRECBULK-1].next = 0;
  return bulk;
}

module queue_empty();
extern module topsucceed();

Const struct predicate queue_empty_pred = { queue_empty, 0, 0 };
Const struct predicate topsucceed_pred = { topsucceed, 0, 0 };
#ifdef SHM
module mastersucceed();
Const struct predicate mastersucceed_pred = { mastersucceed, 0, 0 };
module childsucceed();
Const struct predicate childsucceed_pred = { childsucceed, 0, 0 };
#endif

struct goalrec
  goal_queue_tail = { 0, &queue_empty_pred, { 0 } };

static struct predicate wait_prio_preds[MAXSTDARGS+1];
module wait_prio_routine();

struct predicate wait_penum_preds[MAXSTDARGS+1]; /* for inter nodes */
module wait_penum_routine();

q *reinitiate_prioq(allocp)
     q *allocp;
{
  declare_globals;
  static struct prioqrec
    *pq,
    tail_sentinel = { 0, -1, 0 };

  new_prioqrec(pq);
  /*
    The tail of the lowest priority level is top level succeed goal.
  */
  pq->next = &tail_sentinel;
  pq->prio = 0;
#ifdef SHM
  if (my_node == MASTER) {
    allocp[0] = (q)&goal_queue_tail;
    allocp[1] = (q)&mastersucceed_pred;
    pq->q = (struct goalrec *)allocp;
  } else {
    allocp[0] = (q)&goal_queue_tail;
    allocp[1] = (q)&childsucceed_pred;
    pq->q = (struct goalrec *)allocp;
  }
  allocp += 2;
#else
  allocp[0] = (q)&goal_queue_tail;
  allocp[1] = (q)&topsucceed_pred;
  pq->q = (struct goalrec *)allocp;
  allocp += 2;
#endif
  prioq.next = pq;

  return allocp;
}

q *initiate_prioq(allocp)
     q *allocp;
{
  int k;
  /* Prepare for creating goals that wait for priority value */
  for (k=0; k<=MAXSTDARGS; k++) {
    wait_prio_preds[k].func = wait_prio_routine;
    wait_prio_preds[k].pred = k;
    wait_prio_preds[k].arity = k+3;
  }
  /* Prepare for creating goals that wait for node value */
  for (k=0; k<=MAXSTDARGS; k++) {
    wait_penum_preds[k].func = wait_penum_routine;
    wait_penum_preds[k].pred = k;
    wait_penum_preds[k].arity = k+2;
  }
  return reinitiate_prioq(allocp);
}

struct goalrec *
enqueue_goal(qp, prio, gp, glbl)
     struct goalrec *qp;
     long prio;
     struct goalrec *gp;
     struct global_variables *glbl;
{
  if (prio<0) prio = 0;
  if (current_prio == prio) {
    gp->next = qp;
    qp = gp;
  } else {
#ifdef DEBUGLIB
    extern int trace_flag;
    extern struct enqueue_trace_rec *trace_enqueued_goals;
    if (trace_flag) {
      struct enqueue_trace_rec *tr;
      tr = (struct enqueue_trace_rec *)
	malloc_check(sizeof(struct enqueue_trace_rec));
      tr->next = trace_enqueued_goals;
      tr->g = gp;
      tr->prio = prio;
      trace_enqueued_goals = tr;
    } else {
#endif
      struct prioqrec *pq = &prioq;
      while (pq->next->prio > prio) {
	pq = pq->next;
      }
      if (pq->next->prio == prio) {
	/* there already are some active goals with the same priority */
	pq = pq->next;
	gp->next = pq->q;
	pq->q = gp;
      } else {
	/* there are no active goals with the same priority */
	/* must allocate a new prioqrec entry */
	struct prioqrec *newpq;
	new_prioqrec(newpq);
	newpq->next = pq->next;
	pq->next = newpq;
	newpq->prio = prio;
	newpq->q = gp;
	gp->next = &goal_queue_tail;
      }
      /* interrupt for higher priority goal */
      if (current_prio < prio){
	higher_priority_goal = 1;
	heaplimit = 0;
      }
#ifdef DEBUGLIB
    }
#endif
  }
  return qp;
}

static void
priority_type_error(gp)
     struct goalrec *gp;
{
  fatal("Non-integer priority specified");
}

struct goalrec *
enqueue_after_waiting(qp, prio, gp, allocp, is_relative)
     struct goalrec *qp;
     q prio;
     struct goalrec *gp;
     q *allocp;
     int is_relative;
{
  declare_globals;
  struct goalrec *ng;
  int k;
 again:
  if (isint(prio)) {
    return
      enqueue_goal(qp,
		   (is_relative ? current_prio-intval(prio) : intval(prio)),
		   gp, glbl);
  }
  if (!isref(prio)) priority_type_error(qp);
  {
    q value = derefone(prio);
    if (value != prio && (!isref(value) || derefone(value) != prio)) {
      prio = value;
      goto again;
    }
  }
  heapalloc(ng, gp->pred->arity+(2+3), (struct goalrec *));
  for (k=0; k!=gp->pred->arity; k++) {
    ng->args[k] = gp->args[k];
  }
  ng->args[k] = makecons(gp->pred);
  ng->args[k+1] = prio;
  ng->args[k+2] = (is_relative ? makeint(1) : makeint(0));
  ng->pred = &wait_prio_preds[gp->pred->arity];
  {
    struct goalrec *rsmg = resumed_goals;
    if (rsmg == 0) {
      rsmg = ng;
      ng->next = ng;
    } else {
      ng->next = rsmg->next;
      rsmg->next = ng;
    }
    resumed_goals = rsmg;
  }
  heaplimit = 0;
  return qp;
}

module wait_prio_routine(glbl, qp, allocp, toppred)
  struct global_variables *glbl;
  struct goalrec *qp;
  q *allocp;
  Const struct predicate *toppred;
{
  int arity = toppred->pred;
  q prio = qp->args[arity+1];
  int is_relative = intval(qp->args[arity+2]);

  qp->pred = (struct predicate *)consp(qp->args[arity]);
  qp = enqueue_after_waiting(qp->next, prio, qp, allocp, is_relative);
  heapp = allocp;
  current_queue = qp;
  return (module)qp->pred->func;
}

#ifdef DIST
struct goalrec *send_throw_goal();
#endif
#ifdef SHM
struct goalrec *throw_goal_routine();
#endif

struct goalrec *
enqueue_throw_goal(penum, gp, qp, allocp)
     q penum;
     struct goalrec *gp;
     struct goalrec *qp;
     q *allocp;
{
  declare_globals;
  struct goalrec *ng;
  int k;
 again:
  if (isint(penum)) {
    int pn = intval(penum);
    if ( pn != my_node ) {
#ifdef DIST
      return send_throw_goal(intval(penum), gp, qp);
#else
# ifdef SHM
      return throw_goal_routine(intval(penum), qp, gp);
# else
      return enqueue_goal(qp, current_prio, gp, glbl);
# endif
#endif
    } else return enqueue_goal(qp, current_prio, gp, glbl);
  }
  if (!isref(penum)) fatal("inter cluster pragma error");
  {
    q value = derefone(penum);
    if (value != penum && (!isref(value) || derefone(value) != penum)) {
      penum = value;
      goto again;
    }
  }
  heapalloc(ng, gp->pred->arity+(2+2), (struct goalrec *));
  for (k=0; k!=gp->pred->arity; k++) {
    ng->args[k] = gp->args[k];
  }
  ng->args[k] = makecons(gp->pred);
  ng->args[k+1] = penum;
  ng->pred = &wait_penum_preds[gp->pred->arity];
  {
    struct goalrec *rsmg = resumed_goals;
    if (rsmg == 0) {
      rsmg = ng;
      ng->next = ng;
    } else {
      ng->next = rsmg->next;
      rsmg->next = ng;
    }
    resumed_goals = rsmg;
  }
  heaplimit = 0;
  return qp;

}

module
wait_penum_routine(glbl, qp, allocp, toppred)
  struct global_variables *glbl;
  struct goalrec *qp;
  q *allocp;
  Const struct predicate *toppred;
{
  int arity = toppred->pred;
  q penum = qp->args[arity+1];

  qp->pred = (struct predicate *)consp(qp->args[arity]);
  qp = enqueue_throw_goal(penum, qp, qp->next, allocp);
  heapp = allocp;
  current_queue = qp;
  return (module)qp->pred->func;
}

struct goalrec *
get_top_priority_queue()
{
  declare_globals;
  struct goalrec *newqp;
  struct prioqrec *newprioq;

  newqp = prioq.next->q;
  current_prio = prioq.next->prio;
  newprioq = prioq.next->next;
  free_prioqrec(prioq.next);
  prioq.next = newprioq;
  return newqp;
}

void
put_priority_queue(qp, prio)
     struct goalrec *qp;
     long prio;
{
  declare_globals;
  struct prioqrec *pq = &prioq;

  while(pq->next->prio >= prio) {
    pq = pq->next;
  }
  if (pq->next->prio == prio) {
    /* there are some active goals with the same priority */
    pq = pq->next;
    qp->next = pq->q;
    pq->q = qp;
  } else {
    /* there a no goals with the same priority */
    struct prioqrec *newpq;
    new_prioqrec(newpq);
    newpq->next = pq->next;
    pq->next = newpq;
    newpq->prio = prio;
    newpq->q = qp;
  }
}

#ifdef SHM
void
put_priority_goal(qp, prio)
     struct goalrec *qp;
     long prio;
{
  declare_globals;
  struct prioqrec *pq = &prioq;

  while(pq->next->prio > prio) {
    pq = pq->next;
  }
  if (pq->next->prio == prio) {
    /* there are some active goals with the same priority */
    pq = pq->next;
    qp->next = pq->q;
    pq->q = qp;
  } else {
    /* there a no goals with the same priority */
    struct prioqrec *newpq;
    new_prioqrec(newpq);
    newpq->next = pq->next;
    pq->next = newpq;
    newpq->prio = prio;
    newpq->q = qp;
    qp->next = &goal_queue_tail;
  }
}
#endif

/*
  queue_empty:

  Dummy module to be called when the queue of the current priority
  becomes empty.  The call is automatic by having a dummy goal
  "goal_queue_tail" as the common tail of queues, instead of NULL.
*/
/* temp */
extern struct ex_goalrec** shm_qp;

module queue_empty(glbl, qp, allocp, fg, toppred)
     struct global_variables *glbl;
     struct goalrec *qp;
     struct goalrec *fg;
     q *allocp;
     Const struct predicate *toppred;
{
#ifdef SHM
  if ( ext_queued ) {
     ext_queued = 0;
     qp = get_invoked_goals(&goal_queue_tail);
     heapp = allocp;
     current_queue = qp;
     return (module)qp->pred->func;
  }
#endif
  qp = get_top_priority_queue();
  heapp = allocp;
  current_queue = qp;
  return (module)qp->pred->func;
}

#ifdef SHM
struct goalrec *get_invoked_goals(qp)
struct goalrec *qp;
{
    declare_globals;
    int key = p_key(my_node);

    struct ex_goalrec *sqp, *eqp;

    s_lock(key);
    if ( !(sqp = ex_qp->next) ) { s_unlock(key); return(qp); }
    eqp = sqp;
    while ( eqp->next )  { eqp = eqp->next; }
    ex_qp = eqp;
    s_unlock(key);

    put_priority_queue(qp,current_prio);
    eqp = sqp;
    while(1) {
      put_priority_goal(&eqp->goal,eqp->prio);
      if ( eqp == ex_qp )  break;
      eqp = eqp->next;
    }

    return( get_top_priority_queue() );
  }


module mastersucceed(glbl, qp, allocp, toppred)
struct global_variables *glbl;
struct goalrec *qp;
q *allocp;
Const struct predicate *toppred;
{
    int first,i;

/*    printf("Wait master.\n");*/

  WLoop:
    if ( !(ex_qp->next) ) {
         heapp = klic_interrupt(allocp,qp);
         if ( current_queue != qp ) {
             *cur_status = 1;
              qp = current_queue;
             {
               module func = (module)(toppred = qp->pred)->func;
               return func;
             }
           }

        {
           struct global_variables* tglbl;
           for (i=1;i<total_node;i++) {
             tglbl = get_otherPE_glbl(i);
             if ( top_shm_qp[i] != tglbl->par.aux.shm.ex_qp0 ) goto WLoop;
             if ( pe_status[i] ) goto WLoop;
           }
           if ( ex_qp->next ) goto WLoop;
           for (i=0;i<total_node;i++) { s_lock(p_key(i)); }
	   *cur_status = 0;
           for (i=0;i<total_node;i++) {
             tglbl = get_otherPE_glbl(i);
             if ( ( top_shm_qp[i] == tglbl->par.aux.shm.ex_qp0 ) &&
		 !pe_status[i] )  continue;
             for (i=0;i<total_node;i++) { s_unlock(p_key(i)); }
             goto WLoop;
           }
	   for (i=0;i<total_node;i++) { /* One more */
             tglbl = get_otherPE_glbl(i);
             if ( ( top_shm_qp[i] == tglbl->par.aux.shm.ex_qp0 ) &&
		 !pe_status[i] )  continue;
             for (i=0;i<total_node;i++) { s_unlock(p_key(i)); }
             goto WLoop;
	   }
	   for (i=total_node-1;i>=0;i--) { /* One more */
             tglbl = get_otherPE_glbl(i);
             if ( ( top_shm_qp[i] == tglbl->par.aux.shm.ex_qp0 ) &&
		 !pe_status[i] ) continue;
             for (i=0;i<total_node;i++) { s_unlock(p_key(i)); }
             goto WLoop;
	   }
        }

	for (i=0;i<total_node;i++) { s_unlock(p_key(i)); }

        if ( suspensions - resumes ) {
            fatalf(" %d perpetual suspending goals.",(suspensions-resumes));
          }
         {
          struct ex_goalrec *eqp;
          struct goalrec *qp;

          for (i=1;i<total_node;i++) {
             eqp = (struct ex_goalrec*)galloc(4);
             eqp->prio = HIGHESTPRIO-1;
             qp = &eqp->goal;
             qp->next = 0;
             qp->pred = &topsucceed_pred;
             shm_goal_stack(eqp,i);
           }
          longjmp(klic_topmost, 0);
        }
       }
    qp = get_invoked_goals(qp);
    {
      module func = (module)(toppred = qp->pred)->func;
      heapp = allocp;
      current_queue = qp;
      return func;
    }
  }


module childsucceed(glbl, qp, allocp, toppred)
struct global_variables *glbl;
struct goalrec *qp;
q *allocp;
Const struct predicate *toppred;
{

    while ( !(ex_qp->next) ) {
       heapp = klic_interrupt(allocp,qp);
       if ( current_queue != qp ) {
           *cur_status = 1;
           qp = current_queue;
           {
             module func = (module)(toppred = qp->pred)->func;
             return func;
           }
         }
       *cur_status = 0;
     }

    *cur_status = 1;
    klic_barrier();

    qp = get_invoked_goals(qp);
/*    dump_queue(qp); */

    {
      module func = (module)(toppred = qp->pred)->func;
      heapp = allocp;
      current_queue = qp;
      return func;
    }
  }
#endif
