/* ---------------------------------------------------------- 
%   Copyright 1995 Takashi Chikayama
%       (Read COPYRIGHT for detailed information.) 
----------------------------------------------------------- */

/*
  Stepper: See ggoal.c also which defines "goal objects"
*/

#include <klic/basic.h>
#include <klic/struct.h>
#include <klic/primitives.h>
#include <klic/goalobj.h>
#include <klic/generic.h>
#include "atom.h"
#include "funct.h"

/* Global flag for stepping */
int stepping_flag = 0;

/* Local places to hold stepping info */
static struct goalrec *step_trigger_goal;

static step_suspended;

static int num_queued;
static struct goalrec **queued_goals;
static long *queued_prio;
static int max_queued;

static int num_wokenup;
static struct goalrec **wokenup_goals;
static long *wokenup_prio;
static int max_wokenup;

static int num_thrown;
static struct goalrec **thrown_goals;
static long *thrown_node;
static long *thrown_prio;
static int max_thrown;

static int num_failed;
static struct goalrec **failed_goals;
static int max_failed;

/* Initiation */
static int stepping_initiated = 0;

#define MIN_ALLOC	      8

#define InitialAlloc(type)    \
  ((type *)malloc_check(MIN_ALLOC*sizeof(type)))

static void init_stepping()
{
  extern char *malloc_check();

  queued_goals = InitialAlloc(struct goalrec *);
  queued_prio = InitialAlloc(long);
  max_queued = MIN_ALLOC;
  wokenup_goals = InitialAlloc(struct goalrec *);
  wokenup_prio = InitialAlloc(long);
  max_wokenup = MIN_ALLOC;
  thrown_goals = InitialAlloc(struct goalrec *);
  thrown_node = InitialAlloc(long);
  thrown_prio = InitialAlloc(long);
  max_thrown = MIN_ALLOC;
  failed_goals = InitialAlloc(struct goalrec *);
  max_failed = MIN_ALLOC;
  stepping_initiated = 1;
}

/* Dummy predicate to trigger stepping */
static module module_step__trigger();
Const struct predicate step_trigger_pred = {
  module_step__trigger, 0, 5 };

module module_step__trigger(glbl, qp, allocp, toppred)
     struct global_variables *glbl;
     struct goalrec *qp;
     q *allocp;
     Const struct predicate *toppred;
{
  struct goal_object *obj = (struct goal_object *)functorp(qp->args[0]);
  struct goalrec *goal = obj->goal;
  Const struct predicate *stepped_pred = goal->pred;

  if (!stepping_initiated) {
    init_stepping();
  }
  goal->next = qp->next;
  stepping_flag = 1;
  heaplimit = 0;
  step_trigger_goal = qp;
  num_queued = 0;
  num_wokenup = 0;
  num_failed = 0;
  step_suspended = 0;
  return (*stepped_pred->func)(glbl, goal, allocp, stepped_pred);
}

static q *step_unify_results(allocp, children, wokenup, failure, susp)
     q *allocp;
     q children, wokenup, failure, susp;
{
  struct goalrec *trigger = step_trigger_goal;
  extern q *do_unify_value();
  allocp = do_unify_value(allocp, trigger->args[1], children);
  allocp = do_unify_value(allocp, trigger->args[2], wokenup);
  allocp = do_unify_value(allocp, trigger->args[3], failure);
  allocp = do_unify_value(allocp, trigger->args[4], susp);
  return allocp;
}

struct goalrec *step_after(qp, allocp)
     struct goalrec *qp;
     q *allocp;
{
  declare_globals;
  if (!step_suspended) {
    q children_reversed = NILATOM;
    q children, wokenup, failed;
    struct goalrec *queue_before = step_trigger_goal->next;
    int j, k;
    extern q make_goal_object();

    /*** Handling Newly Queued Goals ***/
    /* sort goals enqueued with different priorities,
     in descending order of priorities */
    for (j = num_queued-1; j > 1; j--) {
      for (k = 0; k < j-1; k++) {
	if (queued_prio[k] <= queued_prio[k+1]) {
	  struct goalrec *goal = queued_goals[k];
	  long prio = queued_prio[k];
	  queued_goals[k] = queued_goals[k+1];
	  queued_prio[k] = queued_prio[k+1];
	  queued_goals[k+1] = goal;
	  queued_prio[k+1] = prio;
	}
      }
    }
    /* make goal list in reverse order */
    /* first, those queued with higher priority */
    for (k = 0; k < num_queued && queued_prio[k] > current_prio; k++) {
      q gobj = make_goal_object(queued_goals[k], allocp);
      allocp = heapp;
      allocp[0] = gobj;
      allocp[1] = makeint(queued_prio[k]);
      allocp[2] = children_reversed;
      allocp[3] = makecons(&allocp[0]);
      children_reversed = makecons(&allocp[2]);
      allocp += 4;
    }
    /* then, inspect the current priority queue */
    if (qp != queue_before) {
      do {
	q gobj = make_goal_object(qp, allocp);
	allocp = heapp;
	allocp[0] = children_reversed;
	allocp[1] = gobj;
	children_reversed = makecons(&allocp[0]);
	allocp += 2;
	qp = qp->next;
      } while (qp != queue_before);
    }
    /* finally, those with lower priority */
    for (; k < num_queued; k++) {
      q gobj = make_goal_object(queued_goals[k], allocp);
      allocp = heapp;
      allocp[0] = gobj;
      allocp[1] = makeint(queued_prio[k]);
      allocp[2] = children_reversed;
      allocp[3] = makecons(&allocp[0]);
      children_reversed = makecons(&allocp[2]);
      allocp += 4;
    }
    /* now, reverse the list */
    children = NILATOM;
    while (children_reversed != NILATOM) {
      q temp = cdr_of(children_reversed);
      cdr_of(children_reversed) = children;
      children = children_reversed;
      children_reversed = temp;
    }

    /*** Handling Woken-up Goals ***/
    /* sort woken-up goals, in ascending order of priority */
    for (j = num_wokenup-1; j > 1; j--) {
      for (k = 0; k < j-1; k++) {
	if (wokenup_prio[k] > wokenup_prio[k+1]) {
	  struct goalrec *goal = wokenup_goals[k];
	  long prio = wokenup_prio[k];
	  wokenup_goals[k] = wokenup_goals[k+1];
	  wokenup_prio[k] = wokenup_prio[k+1];
	  wokenup_goals[k+1] = goal;
	  wokenup_prio[k+1] = prio;
	}
      }
    }
    /* make a list of woken-up goals */
    wokenup = NILATOM;
    for (k = 0; k < num_wokenup; k++) {
      q gobj = make_goal_object(wokenup_goals[k], allocp);
      allocp = heapp;
      if (wokenup_prio[k] != current_prio) {
	allocp[0] = gobj;
	allocp[1] = makeint(queued_prio[k]);
	gobj = makecons(&allocp[0]);
	allocp += 2;
      }
      allocp[0] = wokenup;
      allocp[1] = gobj;
      wokenup = makecons(&allocp[0]);
      allocp += 2;
    }

    /*** Handling Failed Goals */
    failed = NILATOM;
    for (k = 0; k < num_failed; k++) {
      q gobj = make_goal_object(failed_goals[k], allocp);
      allocp = heapp;
      allocp[0] = failed;
      allocp[1] = gobj;
      failed = makecons(&allocp[0]);
      allocp += 2;
    }

    /*** Finally, Unify the Results ***/
    allocp =
      step_unify_results(allocp, children, wokenup, failed, NILATOM);
  }
  heapp = allocp;
  stepping_flag = 0;
  return qp;
}

q *step_susp(allocp, goal, reasonp, spontaneous)
     q *allocp;
     struct goalrec *goal;
     q *reasonp;
     int spontaneous;
{
  declare_globals;
  q reason_list;
  q children;

  allocp[0] = NILATOM;
  allocp[1] = step_trigger_goal->args[0];
  children = makecons(allocp);
  allocp += 2;

  reason_list = NILATOM;
  while (--reasonp >= reasons) {
    allocp[0] = reason_list;
    allocp[1] = *reasonp;
    reason_list = makecons(allocp);
    allocp += 2;
  }
  step_suspended = 1;
  allocp = step_unify_results(allocp, children, NILATOM, NILATOM, reason_list);
  return allocp;
}

void step_enqueue(goal, prio)
     struct goalrec *goal;
     long prio;
{
  if (num_queued >= max_queued) {
    extern char *realloc_check();
    int newmax = max_queued * 2;
    queued_goals = (struct goalrec **)
      realloc_check(queued_goals, newmax*sizeof(struct goalrec *));
    queued_prio = (long *)
      realloc_check(queued_prio, newmax*sizeof(long));
    max_queued = newmax;
  }
  queued_goals[num_queued] = goal;
  queued_prio[num_queued] = prio;
  num_queued++;
}

void step_wokenup(goal, prio)
     struct goalrec *goal;
     long prio;
{
  if (num_wokenup >= max_wokenup) {
    extern char *realloc_check();
    int newmax = max_wokenup * 2;
    wokenup_goals = (struct goalrec **)
      realloc_check(wokenup_goals, newmax*sizeof(struct goalrec *));
    wokenup_prio = (long *)
      realloc_check(wokenup_prio, newmax*sizeof(long));
    max_wokenup = newmax;
  }
  wokenup_goals[num_wokenup] = goal;
  wokenup_prio[num_wokenup] = prio;
  num_wokenup++;
}

void step_thrown(goal, node, prio)
     struct goalrec *goal;
     long node, prio;
{
  if (num_thrown >= max_thrown) {
    extern char *realloc_check();
    int newmax = max_thrown * 2;
    thrown_goals = (struct goalrec **)
      realloc_check(thrown_goals, newmax*sizeof(struct goalrec *));
    thrown_node = (long *)
      realloc_check(thrown_node, newmax*sizeof(long));
    thrown_prio = (long *)
      realloc_check(thrown_prio, newmax*sizeof(long));
    max_thrown = newmax;
  }
  thrown_goals[num_thrown] = goal;
  thrown_node[num_thrown] = node;
  thrown_prio[num_thrown] = prio;
  num_wokenup++;
}

void step_failure(goal)
     struct goalrec *goal;
{
  if (num_failed >= max_failed) {
    extern char *realloc_check();
    int newmax = max_failed * 2;
    failed_goals = (struct goalrec **)
      realloc_check(failed_goals, newmax*sizeof(struct goalrec *));
    max_failed = newmax;
  }
  failed_goals[num_failed] = goal;
  num_failed++;
}
