/* ---------------------------------------------------------- 
%   (C)1993,1994,1995 Institute for New Generation Computer Technology
%       (Read COPYRIGHT for detailed information.) 
%   (C)1996, 1997, 1998, 1999 Japan Information Processing Development Center
%       (Read COPYRIGHT-JIPDEC for detailed information.)
----------------------------------------------------------- */
#include <stdio.h>
#include <klic/basic.h>
#include <klic/struct.h>
#include <klic/alloc.h>
#include <klic/control.h>
#include <klic/susp.h>
#include <klic/generic.h>
#include <klic/gobj.h>

#ifdef DEBUGLIB
extern int count_suspension;
#endif

static void do_fail(goal, reasonp)
     struct goalrec *goal;
     q *reasonp;
{
#ifdef DEBUGLIB
  extern int stepping_flag;
  goal->next = 0;
  if (stepping_flag) {
    step_failure(goal);
    return;
  }
  trace_failure(goal);
#endif
  fatal("Reduction Failure");
}

#ifdef DEBUGLIB
static Inline q *record_susp(allocp, goal)
     q *allocp;
     struct goalrec *goal;
{
  declare_globals;
  struct suspended_goal_rec *sm;
  void record_suspended_pred();
  heapalloc(sm, sizeof(struct suspended_goal_rec)/sizeof(q),
	    (struct suspended_goal_rec *));
  sm->goal = goal;
  sm->next = suspended_goal_list;
  suspended_goal_list = sm;
  if (count_suspension) {
    record_suspended_pred(goal);
  }
  return allocp;
}
#endif

q *suspend_goal(allocp, goal, reason, spontaneous)
     q *allocp;
     struct goalrec *goal;
     q reason;
     int spontaneous;
{
  declare_globals;
  int suspended = 0;

  goal->next = (struct goalrec *)makeint(current_prio);
  if (derefone(reason) == reason) {
    /* no goals suspended yet on this variable */
    struct susprec *susp;
    q newvar;
    allocnewsusp(newvar,susp);
    susp->u.first_hook.u.g = goal;
    derefone(reason) = newvar;
    suspended = 1;
  } else {
    /* some goals already has suspended */
    struct susprec *susp;
    struct hook *newhook;
    susp = suspp(derefone(reason));  /* for shm implementation */
#ifdef SHM
    /*
       On shared memory implementation,
       this `susp' may be instanciated by one of the other processors.
       Thus, check again.
       */
    if ( !isref(susp) || reason != derefone(susp) ) {
      resume_same_prio(goal);
      return(allocp);
    }
#endif
    /* generator object ? */
    if (!is_generator_susp(susp->u)) {
      /* then allocate new hook */
      allochook(&susp->u.first_hook,newhook);
      newhook->u.g = goal;
      suspended = 1;
    } else {
      struct generator_susp *gsusp = generator_suspp(susp);
      q ref = gsusp->backpt;
      struct generator_object *gobj = untag_generator_susp(gsusp->u.o);
      q tmp = generic_generate(gobj, allocp);
      if (tmp != (q)0) {
	if ( tmp == makecons(0) ) {
	  q retval = generic_suspend(gobj, ref, goal);
	  if(retval != makeref(0)) {
	    resume_same_prio(goal);
	  } else {
	    suspended = 1;
	  }
	} else {
	  allocp = heapp;
 	  derefone(ref) = tmp;
	  resume_same_prio(goal);
        }
      } else {
	/* This case, the instanciation by generator object must
	   postponed. Thus:
	   1. ``goal'' is hooked on new variable ``newvar''.
	   2. enqueue unification goal between ``newvar'' and ``ref'' */
	struct susprec *susp;
	q newvar;
	struct goalrec *newgp = (struct goalrec *)allocp;
	extern struct predicate predicate_unify__term__dcode_xunify__goal_2;

	suspended = 1;

	allocnewsusp(newvar,susp);
	susp->u.first_hook.u.g = goal;
	newgp->next = (struct goalrec *)makeint(current_prio);
	newgp->pred = &predicate_unify__term__dcode_xunify__goal_2;
	newgp->args[0] = reason;
	newgp->args[1] = newvar;
	allocp += 4;

	resume_same_prio(newgp);
      }
    }
  }
  if (suspended) {
#ifdef DEBUGLIB
    allocp = record_susp(allocp, goal);
#endif
    suspensions++;
  }

  return(allocp);
}

q *interrupt_goal(allocp, pred, reasonp)
     q *allocp;
     struct predicate *pred;
     q *reasonp;
{
  declare_globals;
  struct goalrec *goal = (struct goalrec *)allocp;
  allocp += pred->arity + 2;
  goal->pred = pred;

  if (reasonp == 0) {
    /* Interrupt by some external event, such as: */
    /*   - A higher priority goal got ready for execution */
    /*   - Garbage collection required */
    /* In such cases, the interrupted goal is pushed down to the queue. */
    struct goalrec *rsmg = resumed_goals;
    if (rsmg == 0) {
      goal->next = goal;
    } else {
      goal->next = rsmg->next;
      rsmg->next = goal;
    }
    resumed_goals = goal;
    return allocp;
  } else if (reasonp == reasons) {
    do_fail(goal, reasons[0]);
  } else {
    /* First, try dereference */
    /* Compiled out code won't dereference more than one level */
    q *rp;
    for (rp=reasons; rp<reasonp; rp++) {
      q v = *rp, iv = v, ov;
      do {
	q tmp;
	tmp = derefone(v);
	if (!isref(tmp)) {
	  derefone(iv) = tmp;
	  resume_same_prio(goal);
	  return allocp;
	}
	ov = v;
	v = tmp;
      } while (derefone(v) != ov);
    }
    /* OK, the goal should really be suspended */
#ifdef DEBUGLIB
    {
      extern int stepping_flag, trace_flag;
      q *step_susp();
      struct goalrec *trace_susp();
      if (reasonp > reasons) {
	if (stepping_flag) {
	  allocp = step_susp(allocp, goal, reasonp, 0);
	  return allocp;
	} else if (trace_flag) {
	  goal = trace_susp(goal, reasonp, 0);
	}
      }
    }
#endif

    goal->next = (struct goalrec *)makeint(current_prio);
    {
      int redo_request = 0;
      do {
	q tmp, tmp1;
/*
	for (tmp = *(--reasonp), tmp1 = derefone(tmp);
	     derefone(tmp1) != tmp;
	     tmp = tmp1, tmp1 = derefone(tmp))
	  ;
*/
	tmp = *(--reasonp);
	while (1) {
	  tmp1 = derefone(tmp);
	  if ( !isref(tmp1) ) {
		resume_same_prio(goal);
		return allocp;
	  }
	  if (derefone(tmp1) == tmp ) break;
	  tmp = tmp1;
	}

	if (tmp == tmp1) {
	  /* no goals suspended yet on this variable */
	  struct susprec *susp;
	  q newvar;
	  makenewsusp(newvar,susp,allocp);
	  susp->u.first_hook.u.g = goal;
	  derefone(tmp) = newvar;
	} else {
	  /* some goals already has suspended */
	  struct susprec *susp = suspp(tmp1);
	  /* generator object ? */
	  if(!is_generator_susp(susp->u)) {
	    if (susp->u.first_hook.next->u.g == goal) {
	      /* If the second hook is for the same goal, do nothing.
		 This includes the case where there's only one hook,
		 because of the loop structure of the hook chain. */
	    } else {
	      struct hook *newhook;
	      addhook(&susp->u.first_hook,newhook,allocp);
	      newhook->u.g = goal;
	    }
	  } else {
	    /* generator object */
	    struct generator_susp *gsusp = generator_suspp(susp);
	    q ref = gsusp->backpt;
	    struct generator_object *gobj = untag_generator_susp(gsusp->u.o);
	    q tmp = generic_generate(gobj, allocp);

	    if(tmp == makecons(0)) {
	      q retval = generic_suspend(gobj, ref, goal);
	      if(retval != makeref(0)) {
		redo_request = 1;
	      }
	    } else {
	      redo_request = 1;
	      if(tmp != makeref(0)) {
		allocp = heapp; 
		derefone(ref) = tmp;
	      } /* else GC request */
	    }
	  }
	}
      } while (reasonp > reasons);
      if (redo_request) {
	resume_same_prio(goal);
      } else {
#ifdef DEBUGLIB
	allocp = record_susp(allocp, goal);
#endif
	suspensions++;
      }
    }
    return allocp;
  }
}

int check_stack_for_alternatively(allocp, reasonp)
     q *allocp;
     q *reasonp;
{
  declare_globals;

  while (reasonp != reasons) {
    /* suspension stack is not empty */
    q tmp, tmp1;
    q var = *(--reasonp);
    q value;
    struct susprec *susp;
    tmp = var;
    if (isref(tmp)) {
      while (1) {
        tmp1 = derefone(tmp);
        if (tmp == tmp1) goto fin;
        if (!isref(tmp1)) {
          value = tmp1;
          goto ok;
        }
        if (tmp == derefone(tmp1)) break;
        tmp = tmp1;
      }
      susp = suspp(tmp1);
      /* generator object ? */
      if (is_generator_susp(susp->u)) {
        struct generator_susp *gsusp = generator_suspp(susp);
        q ref = gsusp->backpt;
        q tmp = generic_generate(untag_generator_susp(gsusp->u.o),
                                 allocp);
        allocp = heapp;
        if (tmp == 0) goto fin;
        derefone(ref) = tmp;
        value = tmp;
        goto ok;
      }
    } else {
      value = tmp;
    ok:
      derefone(var) = value;
      heapp = allocp;
      return 1;
    }
  }
 fin:
  heapp = allocp;
  return 0;
}
