/* ---------------------------------------------------------- 
%   (C)1993,1994,1995 Institute for New Generation Computer Technology 
%       (Read COPYRIGHT for detailed information.) 
%   (C)1996, 1997, 1998, 1999 Japan Information Processing Development Center
%       (Read COPYRIGHT-JIPDEC for detailed information.)
----------------------------------------------------------- */
#include <klic/basic.h>
#include <klic/struct.h>
#include <klic/primitives.h>
#include <klic/schedule.h>
#include <stdio.h>
#include <klic/gb.h>
#include <klic/gobj.h>
#include <klic/susp.h>

q *do_unify(), *do_unify_value();
extern struct predicate predicate_unify__term__dcode_xunify_2;
extern struct predicate predicate_unify__term__dcode_xunify__goal_2;

#define enqueue_unify_terms(x, y) \
{ \
  struct goalrec *gp = (struct goalrec *)allocp; \
  gp->next = (struct goalrec*)makeint(current_prio); \
  gp->pred = &predicate_unify__term__dcode_xunify_2; \
  gp->args[0] = x; \
  gp->args[1] = y; \
  allocp += 4; \
  resume_same_prio(gp); \
}

#define enqueue_unify_goal(x, y) \
{ \
  struct goalrec *gp = (struct goalrec *)allocp; \
  gp->next = (struct goalrec*)makeint(current_prio); \
  gp->pred = &predicate_unify__term__dcode_xunify__goal_2; \
  gp->args[0] = (x); \
  gp->args[1] = (y); \
  allocp += 4; \
  resume_same_prio(gp); \
}

/* Resume a goal with the same priority as current */

void resume_same_prio(gp)
     struct goalrec *gp;
{
  declare_globals;
  if (resumed_goals == 0) {
    resumed_goals = gp;
    gp->next = gp;
  } else {
    gp->next = resumed_goals->next;
    resumed_goals->next = gp;
  }
  heaplimit = 0;
}

/*
  resume the goals which has been hooked to variable `x'.
  by the unification with `y'
*/

q *resume_goals(allocp, x, y)
     q * allocp;
     q x;
     q y;
{
  declare_globals;

  /* Variable x with suspended goals is instantiated here */
  /* x points suspension record directly. */

  struct susprec *susprecord = suspp(x);

  if(is_generator_susp(susprecord->u)) {
    /* generator_object */
    struct generator_susp *gsusp = generator_suspp(susprecord);
    struct generator_object *gobj = untag_generator_susp(gsusp->u.o);
    q *tempallocp;
    q backpt = gsusp->backpt;

    if((tempallocp = method_table_of(gobj)->
	active_unify(gsusp->backpt, y, allocp))) {
      /* Unify method was succeeded */
      allocp = tempallocp;
/*
      derefone(backpt) = y; 
*/
    } else {
      /* failed */
      q tmp = generic_generate(gobj, allocp);

      switch((long)tmp) {
      case (long)makeref(0):
	/* x is a reference to suspension record. thus
	   derefone is needed for enqueue */
	enqueue_unify_goal(derefone(x), y);
	break;
      case (long)makecons(0):
	fatal("illegal situation in generator unification");
      default:
	allocp = heapp;
	derefone(gsusp->backpt) =  tmp;
	if(isref(tmp) && tmp == derefone(tmp)) {
	  derefone(tmp) = y;
	} else {
	  allocp = do_unify(allocp, tmp, y);
	}
      }
    }
  } else {
    struct hook *top = susprecord->u.first_hook.next;
    struct hook *loopp = top;
#ifdef UNIFYDEBUG
    void dump_suspension_structure();
    klic_fprintf(stdout, "\nResume with ");
    print(y);
    klic_putc('\n', stdout);
    dump_suspension_structure(top);
#endif
  
    derefone(susprecord->backpt) = y;
  
    do {
      union goal_or_consumer u;
      u = loopp->u;
      if (u.l != 0) {
	if (is_consumer_hook(u)) {
	  struct consumer_object *obj = untag_consumer_hook(u.o);
	  q tmpval;
	  long keepp = loopp->u.l;
	  loopp->u.l = 0;
	  generic_active_unify(obj, y, allocp);
	  switch ((long)method_result) {
	  case (long)GENERIC_FAILED:
	    fatal("Unification failure on a consumer object");
	  case (long)GENERIC_SUCCEEDED:
	    if(!rest_of_stream) goto consumer_terminate;
/*	    y = rest_of_stream;*/
	    tmpval = rest_of_stream;
	    break;
	  case (long)GENERIC_GCREQUEST:
	    /* This case, make the hook chain which contains
	       rest of consumer objects and goals. */
	    {
	      q newvar = makeref(allocp);
	      allocp++;
	      susprecord->u.first_hook.next = loopp;
	      loopp->u.l = keepp;
	      susprecord->backpt = newvar;
	      derefone(newvar) = (q)susprecord;
	      enqueue_unify_goal(y, newvar);
	    }
	    /* exit the loop */
	    return(allocp);
	  default:
/*	    y = suspension_reason;*/
	    tmpval = method_result;
	  }
	  {
	    struct susprec *susp;
	    q newvar;
	    makenewsusp(newvar,susp,allocp);
	    susp->u.first_hook.u.o = tag_consumer_hook(obj);
	    if (derefone(tmpval) == tmpval) {
	      derefone(tmpval) = newvar;
	    } else {
	      enqueue_unify_goal(tmpval, newvar);
	    }
	  }
	consumer_terminate:;
	} else if (isint(u.g->next)) {
	  long gp = intval(u.g->next);
#ifdef DEBUGLIB
	  extern int stepping_flag, trace_flag;
	  if (stepping_flag) {
	    extern void step_wokenup();
	    u.g->next = 0;	/* to nullify deadlock check */
	    step_wokenup(u.g, gp);
	  } else
#endif
	    {
	      if (gp != current_prio) {
		(void) enqueue_goal(0, gp, u.g, glbl);
	      } else {
		resume_same_prio(u.g);
	      }
#ifdef DEBUGLIB
	      if(trace_flag) trace_resumption(u.g);
#endif
	    }
	  resumes++;
	}
      }
      loopp = loopp->next;
    } while (loopp != top);
  }
  return allocp;
}

static Inline q*
generator_unify(gsx, sy, allocp)
     struct generator_susp *gsx;
     struct susprec *sy;
     q *allocp;
/*
   sx is a suspension structure
   and y is hook or generator
*/
{
  declare_globals;
  q *tmpallocp;

  /* At first, try unify method */
  tmpallocp = (method_table_of(untag_generator_susp(gsx->u.o))->
	       active_unify(gsx->backpt, sy->backpt, allocp));
  if(tmpallocp != (q*)0) {
    /* succeeded */
    allocp = tmpallocp;
/*
    derefone(gsx->backpt) = sy->backpt;
*/
  } else {
    /* unify of x is failed */
    if(is_generator_susp(sy->u)) {
      struct generator_susp *gsy = generator_suspp(sy);
      q *tempallocp =  
	method_table_of(untag_generator_susp(gsy->u.o))->
	  active_unify(gsy->backpt, gsx->backpt, allocp);
      if(tempallocp != (q*)0) {
	allocp = tempallocp;
/*
	derefone(gsy->backpt) = gsx->backpt;
*/
      } else {
	/* x and y are both generator,
	   but both failed. */
	struct generator_object *gobjx = untag_generator_susp(gsx->u.o);
	q tmpx = generic_generate(gobjx, allocp);
	switch((long)tmpx) {
	case (long)makeref(0): {
	  /* failed */
	  struct generator_object *gobjy = untag_generator_susp(gsy->u.o);
	  q tmpy = generic_generate(gobjy, allocp);
	  switch((long)tmpy) {
	  case (long)makeref(0):
	    enqueue_unify_goal(gsx->backpt, sy->backpt);
	    break;
	  case (long)makecons(0):
	    fatal("illegal situation at the generator unification");
	  default:
	    allocp = heapp;
	    derefone(gsy->backpt) = tmpy;
	    if(isref(tmpy) && tmpy == derefone(tmpy)) {
	      derefone(tmpy) = gsx->backpt;
	    } else {
	      allocp = do_unify(allocp, tmpy, gsx->backpt);
	    }
	  }
	  break;
	}
	case (long)makecons(0):
	  fatal("illegal situation at the generator unification");
	default:
	  allocp = heapp;
	  gsx->backpt = tmpx;
	  if(isref(tmpx) && tmpx == derefone(tmpx)) {
	    derefone(sy->backpt) = tmpx;
	  } else {
	    return do_unify(allocp, tmpx, sy->backpt);
	  }
	}
      }
    } else {
      /* The unify method for x is failed and
	 y is hook */
      struct generator_object *gobjx = untag_generator_susp(gsx->u.o);
      q tmpx = generic_generate(gobjx, allocp);
      switch((long)tmpx) {
      case (long)makeref(0): /* GC request */
	enqueue_unify_goal(gsx->backpt, sy->backpt);
	break;
      case (long)makecons(0): /* illegal */
	fatal("illegal situation at the generator unification");
      default:
        allocp = heapp;
	derefone(gsx->backpt) = tmpx;
	if(isref(tmpx) && tmpx == derefone(tmpx)) {
	  derefone(tmpx) = sy->backpt;
	} else {
	  return do_unify(allocp, tmpx, sy->backpt);
	}
      }
    }
  }
  return allocp;
}

q *do_unify(allocp, x, y)
     q * allocp;
     q x, y;
{
#ifdef UNIFYDEBUG
  klic_fprintf(stdout, "Unify with ");
  print(x);
  klic_fprintf(stdout, ",");
  print(y);
  klic_fprintf(stdout, "\n");
#endif
  if (isref(x)) {
    q temp = derefone(x);
  deref_x:
    if (x != temp){
      if (isref(temp)) {
	q temp1 = derefone(temp);
	if (temp1 == x) {
	  while (isref(y)) {
	    q ytemp = derefone(y);
	    if (y == ytemp) {
	      /* Suspension records must be referenced through REF. */
	      /* Thus, doing "derefone(y) = temp;" here is buggy. */
	      derefone(y) = x;
	      return allocp;
	    } else {
	      if (isref(ytemp) && derefone(ytemp) == y) {
		y = ytemp;
		x =  temp;
		if (x != y) {
		  /* merge two hook chains */
		  struct susprec *sx = (struct susprec *)x;
		  struct susprec *sy = (struct susprec *)y;
		  if(is_generator_susp(sx->u)) {
		    return generator_unify(generator_suspp(sx), sy, allocp);
		  } else if(is_generator_susp(sy->u)) {
		    return generator_unify(generator_suspp(sy), sx, allocp);
		  } else {
		    /* Both x and y are not generator */
		    /* None of two is generator, then merge ... */
		    struct hook *second_of_x = sx->u.first_hook.next;
		    /* connect sx and topy */
		    sx->u.first_hook.next = sy->u.first_hook.next;
		    sy->u.first_hook.next = second_of_x;
		    derefone(sy->backpt) = sx->backpt;
		  }
		}
		return allocp;
	      }
	    }
	    y = ytemp;
	  }
	  /* x is hook variable and y points a real object */
	  return resume_goals(allocp, temp, y);
	} else {
	  x = temp;
	  temp = temp1;
	  goto deref_x;
	}
      } else {
	x = temp;
      }
    } else {
      /* dereference y */
      while (isref(y)) {
	temp = derefone(y);
	if (temp == y || (isref(temp) && derefone(temp) == y)) break;
	y = temp;
      }
      derefone(x) = y;		/* this also handles x==y cases */
      return allocp;
    }
  }

  /* x is bound */
  while (isref(y)) {
    q temp = derefone(y);
    if (temp == y) { /* y is undef cell */
      derefone(y) = x;
      return allocp;
    } else {
      if(isref(temp) && derefone(temp) == y) {
	return resume_goals(allocp, temp, x);
      }
    }
    y = temp;
  }

  /* Both x and y are bound */
  if (x != y) {
    declare_globals;
    enqueue_unify_terms(x, y);
  } 
  return allocp;
}

q *do_shallow_unify(allocp, x, y)
     q * allocp;
     q x, y;
{
  /*
    Unifier that does never call unification recursively.
    To realize this, we make goals not only for recursive unifications
    but also for unification with hooked variables, to avoid the
    unifier to be called recursively from generic objects.
  */
  declare_globals;
  if (isref(x)) {
    q temp = derefone(x);
  deref_x:
    if (x != temp){
      if (isref(temp)) {
	q temp1 = derefone(temp);
	if (temp1 == x) {
	  while (isref(y)) {
	    q ytemp = derefone(y);
	    if (y == ytemp) {
	      /* Suspension records must be referenced through REF. */
	      /* Thus, doing "derefone(y) = temp;" here is buggy. */
	      derefone(y) = x;
	      return allocp;
	    } else {
	      if (isref(ytemp) && derefone(ytemp) == y) {
		enqueue_unify_goal(x, y);
		return allocp;
	      }
	    }
	    y = ytemp;
	  }
	  /* x is hook variable and y points a real object */
	  enqueue_unify_goal(x, y);
	  return allocp;
	} else {
	  x = temp;
	  temp = temp1;
	  goto deref_x;
	}
      } else {
	x = temp;
      }
    } else {
      /* dereference y */
      while (isref(y)) {
	temp = derefone(y);
	if (temp == y || (isref(temp) && derefone(temp) == y)) break;
	y = temp;
      }
      derefone(x) = y;		/* this also handles x==y cases */
      return allocp;
    }
  }

  /* x is bound */
  while (isref(y)) {
    q temp = derefone(y);
    if (temp == y) { /* y is undef cell */
      derefone(y) = x;
      return allocp;
    } else {
      if(isref(temp) && derefone(temp) == y) {
	enqueue_unify_goal(x, y);
	return allocp;
      }
    }
    y = temp;
  }

  /* Both x and y are bound */
  if (x != y) {
    enqueue_unify_goal(x, y);
  } 
  return allocp;
}

q *do_unify2(allocp, x, y, z, w)
     q *allocp;
     q x, y, z, w;
{
  allocp = do_unify(allocp, x, y);
  return do_unify(allocp, z, w);
}

q *do_unify3(allocp, x, y, z, w, s, t)
     q *allocp;
     q x, y, z, w, s, t;
{
  allocp = do_unify(allocp, x, y);
  allocp = do_unify(allocp, z, w);
  return do_unify(allocp, s, t);
}

q *do_unify4(allocp, x, y, z, w, s, t, u, v)
     q *allocp;
     q x, y, z, w, s, t, u, v;
{
  allocp = do_unify(allocp, x, y);
  allocp = do_unify(allocp, z, w);
  allocp = do_unify(allocp, s, t);
  return do_unify(allocp, u, v);
}

/*
  do_unify_value(allocp, x, y)
	"do_unify" for when "y" is known to be instantiated.
*/
q *do_unify_value(allocp, x, y)
     q * allocp;
     q x, y;
{
  if (isref(x)) {
    q temp = derefone(x);
    if (x == temp) {
      derefone(x) = y;
      return allocp;
    } else if (isref(temp)) {
      q temp1;
    again:
      temp1 = derefone(temp);
      if (isref(temp1)) {
	if (temp1 == temp) {
	  derefone(temp) = y;
	  return allocp;
	} else if (temp1 == x) {
	  return resume_goals(allocp, temp, y);
	} else {
	  x = temp;
	  temp = temp1;
	  goto again;
	}
      } else {
	return do_unify(allocp, temp1, y);
      }
    } else {
      return do_unify(allocp, temp, y);
    }
  }
  return do_unify(allocp, x, y);
}

q *do_shallow_unify_value(allocp, x, y)
     q * allocp;
     q x, y;
{
  if (isref(x)) {
    q temp = derefone(x);
    if (x == temp) {
      derefone(x) = y;
      return allocp;
    } else if (isref(temp)) {
      q temp1;
    again:
      temp1 = derefone(temp);
      if (isref(temp1)) {
	if (temp1 == temp) {
	  derefone(temp) = y;
	  return allocp;
	} else if (temp1 == x) {
	  return resume_goals(allocp, temp, y);
	} else {
	  x = temp;
	  temp = temp1;
	  goto again;
	}
      } else {
	return do_shallow_unify(allocp, temp1, y);
      }
    } else {
      return do_shallow_unify(allocp, temp, y);
    }
  }
  return do_shallow_unify(allocp, x, y);
}

q *do_unify_value2(allocp, x, y, z, w)
     q *allocp;
     q x, y, z, w;
{
  allocp = do_unify_value(allocp, x, y);
  return do_unify_value(allocp, z, w);
}

q *do_unify_value3(allocp, x, y, z, w, s, t)
     q *allocp;
     q x, y, z, w, s, t;
{
  allocp = do_unify_value(allocp, x, y);
  allocp = do_unify_value(allocp, z, w);
  return do_unify_value(allocp, s, t);
}

q *do_unify_value4(allocp, x, y, z, w, s, t, u, v)
     q *allocp;
     q x, y, z, w, s, t, u, v;
{
  allocp = do_unify_value(allocp, x, y);
  allocp = do_unify_value(allocp, z, w);
  allocp = do_unify_value(allocp, s, t);
  return do_unify_value(allocp, u, v);
}

#ifdef UNIFYDEBUG
#define dumpgoal(g) \
{ \
  if(g){ \
    klic_fprintf(stdout, "\tgoal:\t"); \
    klic_fprintf(stdout, \
		 "%x:%d/%d\n", g->pred->func, g->pred->pred, g->pred->arity); \
  } else { \
    klic_fprintf(stdout, "Illegal goal !: %x\n", g); \
  } \
}

void
dump_suspension_structure(x)
q *x;
{
  q *i = x;
  klic_fprintf(stdout, "Dump suspension structure : %x\n", x);
  do{
    switch(ptagof(*i)){
     case SSUSP:
      {
	struct shook *s = shookp(i);
	struct goalrec *g = s->goals;
	klic_fprintf(stdout, "Single suspension record : %x\n", s);
	dumpgoal(g);
      }
      break;
     case MSUSP:
      {
	struct mhook *m = mhookp(i);
	struct goalrec *g = m->goals;
	klic_fprintf(stdout, "Multiple suspension record : %x\n", m);
	dumpgoal(g);
	klic_fprintf(stdout, "back pointer:\t");
	klic_fprintf(stdout, "%x\n", m->back);
	klic_fprintf(stdout, "related record:\t%x\n", m->next);
      }
      break;
     default:
      klic_fprintf(stdout, "Merger\n");
    }
    i = (q*)getnexthook(i);
  }while(getnexthook(i) != x);
}
#endif
