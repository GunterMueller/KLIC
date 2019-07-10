/* ----------------------------------------------------------
%   (C)1993,1994,1995 Institute for New Generation Computer Technology
%       (Read COPYRIGHT for detailed information.)
%   (C)1996, 1997, 1998, 1999 Japan Information Processing Development Center
%       (Read COPYRIGHT-JIPDEC for detailed information.)
----------------------------------------------------------- */
#include <klic/basic.h>
#include <klic/struct.h>
#include <klic/primitives.h>
#include <klic/timing.h>
#include <stdio.h>
#include <klic/functorstuffs.h>
#include <klic/gobj.h>
#include <klic/susp.h>

#ifdef DIST
#include <klic/interpe.h>
#endif
#ifdef SHM
#include <klic/shm.h>
/* a following line is for debugging of reverse pointer problem. */
static int hirata_bug1;
  
#define push_shm_stack(addr,sp,max)                     \
{                                                       \
   if ( (sp) == max )  {                                \
    (sp) = make_shm_larger_stack(sp);                   \
    max = shm_gcmax;                                    \
  }                                                     \
  *(sp)++ = (q*)(addr);                                 \
}

#endif

extern char *malloc(), *malloc_check(), *realloc_check();
extern struct goalrec goal_queue_tail;

static Inline void flip_spaces()
{
  declare_globals;
  q *tempp = new_space_top;
  unsigned long temps = new_space_size;
  new_space_top = old_space_top;
  new_space_size = old_space_size;
  old_space_top = tempp;
  old_space_size = temps;
}

static struct goalrec *collect_garbage();

struct goalrec *klic_gc(qp)
     struct goalrec *qp;
{
  timerstruct before, after;
  declare_globals;
  static lastgc_dangerous = 0;
  static int make_heap_larger = 0; /* make heap larger in the next GC */
  q *new_new_space_top;
  unsigned long bytesize;
  int k;
/*  debug_printf("MAIN GC START\n"); */
 if (measure_gc) measure(before);
 gc_again:
  if (make_heap_larger) {
    q *new_old_space_top;
    heapsize *= 2;
    if (heapsize > maxheapsize) heapsize = maxheapsize;
    bytesize = (heapsize+incrementsize)*sizeof(q);
    new_old_space_top = (q *)malloc(bytesize);
    if (new_old_space_top != 0) {
      new_new_space_top = (q *)malloc(bytesize);
      if (new_new_space_top != 0) {
	free(old_space_top);
	old_space_top = new_old_space_top;
	old_space_size = bytesize;
      } else {
	free(new_old_space_top);
	if (lastgc_dangerous) {
	  fatal("Not enough space collected and can't make heap larger");
	}
      }
    }
  } else if (lastgc_dangerous) {
    fatalf("Maximum heap size specified (%u words) has been used up",
	   maxheapsize);
  }
  flip_spaces();
  copied_susp = 0;		/* for perpetual suspension detection */
  qp = collect_garbage(qp);
  if (make_heap_larger) {
    free(old_space_top);
    old_space_top = new_new_space_top;
    old_space_size = bytesize;
  }
  make_heap_larger =
    (heapp-new_space_top+this_more_space > heapsize*maxactiveratio &&
     heapsize < maxheapsize);
  if (lastgc_dangerous = (real_heaplimit < heapp+this_more_space)) {
    goto gc_again;
  }
  this_more_space = 0;
  gctimes++;
#ifdef SHM
    if ( F_shm_gc ) qp = shm_gc(qp);
#endif
  if (measure_gc) {
    measure(after);
#ifdef GETRUSAGE
    gcums += diff_usec(ru_utime)/1000;
    gcsms += diff_usec(ru_stime)/1000;
#else
    gcums += (int) tick2msec(field_diff(tms_utime));
    gcsms += (int) tick2msec(field_diff(tms_stime));
#endif
  }
  return qp;
}


q**
make_larger_stack(sp)
     q**sp;
{
  declare_globals;
  q **newstack;
  gcstack_size *= 2;
  newstack = (q**)realloc_check(gcstack, gcstack_size*sizeof(q*));
  sp = newstack+(sp-gcstack);
  gcstack = newstack;
  gcmax = newstack+gcstack_size;
  return sp;
}

#ifdef SHM
q** make_shm_larger_stack(sp)
     q**sp;
{
  declare_globals;
  q **newstack;
  shm_gcstack_size *= 2;
  newstack = (q**)realloc_check(shm_gcstack, shm_gcstack_size*sizeof(q*));
  sp = newstack+(sp-shm_gcstack);
  shm_gcstack = newstack;
  shm_gcmax = newstack+shm_gcstack_size;
  return sp;
}
#endif

#define within_new_space(x)	\
( (unsigned long)(x) - (unsigned long)ntop < nsize)

#define within_old_space(x)	\
( (unsigned long)(x) - (unsigned long)otop < osize)

#define push_gc_stack(addr, sp, max)			\
{							\
  if ((sp) == max) {					\
    (sp) = make_larger_stack(sp);			\
    max = gcmax;					\
  }							\
  *(sp)++ = (addr);					\
}

#ifdef SHM
#define reserve_copy(from, to, sp, max)                 \
if (from == makeref(&from)) {                           \
  to = from = makeref(&to);                             \
} else {                                                \
  to = from;                                            \
  if (!isatomic(from)) {                                \
    if ( is_shma(from) ) {                              \
       push_shm_stack(&to,shm_sp,shm_gcmax);            \
    } else if ( within_old_space(from )) {              \
       from = makeref(&to);                             \
       push_gc_stack(&to, sp, max);                     \
    }                                                   \
  }                                                     \
}
#else
#define reserve_copy(from, to, sp, max)			\
if (from == makeref(&from)) {				\
  to = from = makeref(&to);				\
} else {						\
  to = from;						\
  if (!isatomic(from) && within_old_space(from)) {	\
    from = makeref(&to);				\
    push_gc_stack(&to, sp, max);			\
  }							\
}
#endif

#define copy_one_goal(goal, sp, max, susp)		\
{							\
  struct goalrec *og=(goal);				\
  int n = (og)->pred->arity;				\
  struct goalrec *ng = (struct goalrec *)hp;		\
  hp += n + 2;						\
  ng->next = og->next;					\
  og->next = ng;					\
  ng->pred = og->pred;					\
  og->pred = 0;						\
  while (--n >= 0) {					\
    reserve_copy(og->args[n], ng->args[n], (sp), (max)); \
  }							\
  (goal) = ng;						\
  if (susp) copied_susp++;				\
}

static q *
copy_terms(hp, ntop, otop, nsize, osize, sp, max)
     q *hp;
     q *ntop, *otop;
     unsigned long nsize, osize;
     q **sp, **max;
{
  declare_globals;
  while (sp > gcstack) {
    q *addr = *--sp;
    q obj = *addr;
  loop:
#ifdef SHM
          if ( is_shma(obj) ) {
            *addr = obj;
            if ( ptagof(obj) != ATOMIC ) {
              push_shm_stack(addr,shm_sp,shm_gcmax);
            }
            continue;
          }
#endif
    switch (ptagof(obj)) {
    case ATOMIC:
      *addr = obj;
      break;
    case VARREF:
      {
	q value;
      deref:
	value = derefone(obj);
	switch (ptagof(value)) {
	case VARREF:
	  if (derefone(value) == obj) {
	    if (value == obj) {
	      if (within_new_space(addr)) {
		*addr = derefone(obj) = makeref(addr);
	      } else {
		*addr = derefone(obj) = *hp = makeref(hp);
		hp++;
	      }
	    } else {
	      struct susprec *s;
/*
		if(value > obj) {
		  s = suspp(value);
		} else {
		  s = suspp(obj);
		}
*/   s = suspp(value);
	      if (is_generator_susp(s->u)) {
		struct generator_susp *gsusp = generator_suspp(s);
		q newvar = *addr = derefone(obj) = makeref(hp);
		hp++;
		{
		  struct generator_susp *newgsusp = generator_suspp(hp);
		  hp += sizeof(struct generator_susp) / sizeof(q);
		  derefone(newvar) = makeref(newgsusp);
		  newgsusp->backpt = makeref(newvar);
		  {
		    struct generator_object *oldobj =
		      untag_generator_susp(gsusp->u.o);
		    q newplace = (q)(oldobj->method_table);
		    if(!isstruct(newplace)) {
		      /* not yet copied */
		      struct generator_object *newobj
			= (struct generator_object *)
			  generic_gc(oldobj, hp, sp);
		      sp = gcsp;
		      hp = heapp;
		      oldobj->method_table = 
			(struct generator_object_method_table *)
			  makefunctor(newobj);
		      newgsusp->u.o = tag_generator_susp(newobj);
		    } else {
		      /* already copied */
		      newgsusp->u.o =
			tag_generator_susp(functorp(newplace));
		    }
		  }
		}
	      } else {
		struct hook *second_hook = s->u.first_hook.next;
		struct hook *h = second_hook;
		struct hook dummy;
		struct hook *last = &dummy;
		union goal_or_consumer lastu;
		q newvar;
		/* make a new variable, anyway */
		newvar = *addr = derefone(obj) = *hp = makeref(hp);
		hp++;
		lastu.l = 0;
		do {
		  union goal_or_consumer u;
		  u = h->u;
		  if (u.l != 0) {
		    union goal_or_consumer nu;
		    nu.l = 0;
		    if (!is_consumer_hook(u)) {
		      /* suspended goal */
		      if (u.g->pred == 0) {
			nu.g = u.g->next;
		      } else if (!isref(u.g->next)) {
			nu.g = u.g;
/*  			debug_printf("COPY_ONE_GOAL %p %p h:%p %p %p\n",nu.g, nu.g->pred,h,h->next,second_hook); */
			copy_one_goal(nu.g, sp, max, 1);
		      } else {
			goto not_a_valid_hook;
		      }
		    } else {
		      /* consumer object */
		      q newplace = (q)(untag_consumer_hook(u.o)->method_table);
		      if (isstruct(newplace)) {
			nu.o = tag_consumer_hook(functorp(newplace));
		      } else {
			struct consumer_object *newobj =
			  (struct consumer_object *)
			    generic_gc(untag_consumer_hook(u.o), hp, sp);
			sp = gcsp;
			hp = heapp;
			untag_consumer_hook(u.o)->method_table =
			  (struct consumer_object_method_table *)
			    makefunctor(newobj);
			nu.o = tag_consumer_hook(newobj);
		      }
		    }
		    if (lastu.l != 0) {
		      struct hook *nh = (struct hook *)hp;
		      hp += sizeof(struct hook)/sizeof(q);
		      nh->u = lastu;
		      last->next = nh;
		      last = nh;
		    }
		    lastu = nu;
		  }
		not_a_valid_hook:
		  h = h->next;
		} while (h != second_hook);
		if (lastu.l != 0) {
		  struct susprec *ns = (struct susprec *)hp;
		  hp += sizeof(struct susprec)/sizeof(q);
		  last->next = &ns->u.first_hook;
		  ns->backpt = newvar;
		  ns->u.first_hook.next = dummy.next;
		  ns->u.first_hook.u = lastu;
		  derefone(newvar) = (q)ns;
		}
	      }
	    }
	  } else if (within_old_space(value)) {
	    obj = value;
	    goto deref;
	  } else {
	    *addr = value;
#ifdef SHM
	    if ( is_shma(value) ) push_shm_stack(addr,shm_sp,shm_gcmax);
#endif
	  }
	  break;
	case CONS:
	  if (within_new_space(value)) {
	    *addr = makeref(&cdr_of(value));
	  } else {
	    obj = value;
	    goto cons_case;
	  }
	  break;
	case ATOMIC:
	  *addr = value;
	  break;
	default: /* FUNCTOR */
	  obj = value;
	  goto functor_case;
	}
      }
      break;
    case CONS:
    cons_case:
      if (within_old_space(obj)) {
	q cdr = cdr_of(obj);
	if (!isstruct(cdr) || !within_new_space(cdr)) {
	  q newcons = makecons(hp);
	  hp += 2;
	  reserve_copy(car_of(obj), car_of(newcons), sp, max);
	  *addr = cdr_of(obj) = newcons;
	  if (isatomic(cdr)) {
	    cdr_of(newcons) = cdr;
	  } else {
	    if (cdr == makeref(&cdr_of(obj))) {
	      cdr_of(newcons) = makeref(&cdr_of(newcons));
	    } else {
	      addr = &cdr_of(newcons);
	      obj = cdr;
	      goto loop;
	    }
	  }
	} else {
	  *addr = cdr;
	}
      } else {
	*addr = obj;
      }
      break;
    default: /* FUNCTOR */
    functor_case:
      if (within_old_space(obj)) {
	q f = functor_of(obj);
	if(!isstruct(f)){
	  if(isref(f)) {
	    struct data_object *oldobj
	      = (struct data_object *)functorp(obj);
	    q *newobj;
	    newobj = generic_gc(oldobj, hp, sp);
	    sp = gcsp;
	    hp = heapp;
	    *addr = functor_of(obj) = makefunctor(newobj);
	  } else {
	    q newfunct = makefunctor(hp);
	    int k = arityof(f);
	    hp += k+1;
	    *addr = functor_of(obj) = newfunct;
	    functor_of(newfunct) = f;
	    do {
	      k--;
	      reserve_copy(arg(obj,k), arg(newfunct,k), sp, max);
	    } while (k > 0);
	  }
	} else {
	  *addr = f;
	}
      } else {
	*addr = obj;
      }
      break;
    }
  }
  gcsp = sp;
  return hp;
}

struct goalrec *copy_one_queue(qp, hp, ntop, otop, nsize, osize)
     struct goalrec *qp;
     q *hp;
     q *ntop, *otop;
     unsigned long nsize, osize;
{
  declare_globals;
  struct goalrec *last, *next;

  /* Copy queue in reverse order */
  /* By this, variables will have better chance to be allocated */
  /* within the goal records or structures that'll be read after */
  /* their instantiation */

  /* First, reverse the goal queue */
  for (last=0; qp!=&goal_queue_tail; last=qp, qp=next) {
    next = qp->next;
    qp->next = last;
  }
  /* Then copy and rearrange the goal queue */
  qp = last;
  last = &goal_queue_tail;
  for (; qp != 0; qp=next) {
    next=qp->next;
#ifdef SHM
    if ( is_shma(qp) ) {
        qp->next = last;
        last = qp;
        continue;
    }
#endif
    copy_one_goal(qp, gcsp, gcmax, 0);
    hp = copy_terms(hp, ntop, otop, nsize, osize, gcsp, gcmax);
    qp->next = last;
    last = qp;
  }
  heapp = hp;
  return last;
}

static struct goalrec *collect_garbage(qp)
     struct goalrec *qp;
{
  declare_globals;
  int k;
  q *allocp, *ntop, *otop;
  unsigned long nsize, osize;
  struct prioqrec *pq = prioq.next;

#ifdef DIST
  if (node_wtc == 0) {
    fatal("invalid WTC in gc");
  }
#endif

  if (gctimes==0) {
    /* allocate GC stack on first GC */
    gcstack_size = GCSTACKSIZE;
    gcstack = (q**)malloc_check(gcstack_size*sizeof(q*));
    gcsp = gcstack;
    gcmax = gcstack+gcstack_size;
#ifdef SHM
    shm_gcstack_size = GCSTACKSIZE;
    shm_gcstack = (q**)malloc(shm_gcstack_size*sizeof(q*));
    shm_sp = shm_gcstack;
    shm_gcmax = shm_gcstack+shm_gcstack_size;
  } else {
    shm_sp = shm_gcstack;
#endif
  }

#ifdef SHM
  /* copy into Shared-memory Generator hook data */
  {
    TADDRtbl* sptr = &ADDRtbl;
    TADDRtbl* nptr;
    sptr = &ADDRtbl;
    for(sptr=sptr->next;sptr!=&ADDRtbl;sptr=nptr) {
      nptr = sptr->next;
      switch(ptagof(sptr->localA)) {
      case CONS:
      case FUNCTOR: { /* generator hook */
	struct generator_object* addi;
	Shvar* objp;
	q temp;
  Re_try:
	temp = derefone(sptr->globalA);
	if ( !isref(temp) ) { break; }
	if ( derefone(temp) != (q)sptr->globalA ) {
	  sptr->globalA = (q*)temp; goto Re_try;
	}
	addi = n_lock(((q)sptr->globalA),temp);
	if ( derefone(sptr->globalA) != temp ) { goto Re_try; }
	objp = (Shvar*)untag_generator_susp(addi);
	if ( is_genhook(objp->chain)) {
	  q tempg;
	  shm_arg_copy(&sptr->localA,&tempg);
	  klic_barrier();
	  *(sptr->globalA) = tempg;
	  free_local_tbl(sptr);
	} else {
	  n_unlock(temp,addi);
	}
      }
      }
    }
  }
#endif

  allocp = ntop = heaptop = new_space_top;
  otop = old_space_top;
  real_heapbytesize = nsize = new_space_size;
  osize = old_space_size;
  real_heaplimit = allocp+heapsize;
  if (interrupt_off) heaplimit = real_heaplimit;
  else heaplimit = 0;
  heapbottom = real_heaplimit+incrementsize;

  for (k=0; k<num_gc_hooks; k++) {
    allocp = gc_hook_table[k](allocp, ntop, otop, nsize, osize);
  }

  for(; pq->prio >= 0; pq = pq->next) {
    pq->q = copy_one_queue(pq->q, allocp, ntop, otop, nsize, osize);
    allocp = heapp;
  }
  qp = copy_one_queue(qp, allocp, ntop, otop, nsize, osize);
#ifdef SHM
  {
    TADDRtbl* sptr = &ADDRtbl;
    TADDRtbl* nptr;
    q *hp = heapp;
    sptr = &ADDRtbl;
    for(sptr=sptr->next;sptr!=&ADDRtbl;sptr=nptr) {
      nptr = sptr->next;
      switch(ptagof(sptr->localA)) {
      case CONS:
      case FUNCTOR: { /* generator hook but anybody reqested */
        push_gc_stack((q*)&sptr->localA,gcsp,gcmax);
        hp = copy_terms(hp, ntop, otop, nsize, osize, gcsp, gcmax);
        push_shm_stack(&sptr->globalA,shm_sp,shm_gcmax);
        break;
      }
      case ATOMIC: { /* genarator object (distributed interface) */
	q wk = (q)untag_local(sptr->localA);
	if ( !derefone(wk) ) { /* consumer */
	  q top = (q)&(sptr->localA);
	  derefone(wk) = top;
	  derefone(top) = wk;
	  push_gc_stack(&top,gcsp,gcmax);
/* patch for debugging of hirata problem */
	  hirata_bug1 = 1;
	  hp = copy_terms(hp, ntop, otop, nsize, osize, gcsp, gcmax);
	  hirata_bug1 = 0;
	  wk = derefone(top);
	  if ( wk != top ) {
	    sptr->localA = (q*)tag_local(wk);
	    derefone(wk) = 0;
            push_shm_stack(&sptr->globalA,shm_sp,shm_gcmax);
          } else goto REM_HOOK;
	  break;
	} else { /* generator */
	  push_gc_stack(&wk,gcsp,gcmax);
	  hp = copy_terms(hp, ntop, otop, nsize, osize, gcsp, gcmax);
	  sptr->localA = (q*)tag_local(wk);
          push_shm_stack(&sptr->globalA,shm_sp,shm_gcmax);
	  break;
	}
      }
      default: { /* normal goal */
	struct goalrec* wqp = (struct goalrec*)sptr->localA;
	if ( !wqp ) { /* skip */
	} else if ( !wqp->pred ) {
	  sptr->localA = (q*)wqp->next;
	} else if ( isint(wqp->next) ) {
	  copy_one_goal(wqp, gcsp, gcmax,1);
	  sptr->localA = (q*)wqp;
	  hp = copy_terms(hp, ntop, otop, nsize, osize, gcsp, gcmax);
	  if ( isref(sptr->globalA) ) {
	    q ww;
	    while (1) {
	      ww = derefone(sptr->globalA);
	      if ( isref(ww) ) {
		if ( (q)sptr->globalA == derefone(ww) ) break;
		else {
		  sptr->globalA = (q*)ww;
		}
	      } else break;
	    }
	  }
	  push_shm_stack(&sptr->globalA,shm_sp,shm_gcmax);
	} else {
	REM_HOOK:
	  sptr->localA = 0;
          /* removes a hook record */
	  {
	    struct generator_object* addi;
	    q sv;
	    Shvar* objp;
	    Sinfo *hk,*bsi,*si;
	    q var = (q)sptr->globalA;
	    sv = derefone(var);
	    if ( !isref(sv) || derefone(sv) != var )  { goto REM_skip; }
	    addi = n_lock(var,sv);
	    if ( derefone(var) != sv ) goto REM_skip;
	    objp = (Shvar*)untag_generator_susp(addi);
	    hk = objp->chain;
	    if ( hk->indp == sptr ) {
	      objp->chain = hk->next;
		  free_local_tbl(sptr);
	    } else {
	      for(bsi=hk,si=bsi->next;(si);bsi=si,si=si->next) {
		if ( si->indp == sptr ) {
		  bsi->next = si->next;
		  free_local_tbl(sptr);
		  break; }
	      }
	    }
	    n_unlock(sv,addi);
	  REM_skip:
	    ;
	  }
	}
      }
      }
    }
    heapp = hp;
  }
#endif
  for (k=0; k<num_before_pcheck_hooks; k++) {
    heapp = before_pcheck_hook_table[k](heapp, ntop, otop, nsize, osize);
  }
#ifdef DEBUGLIB
  {
    struct suspended_goal_rec *sgl = suspended_goal_list;
    struct suspended_goal_rec **sgl_tail = &suspended_goal_list;
    struct goalrec *dead_goal = 0;
    q *hp = heapp;
    if (copied_susp != suspensions-resumes) {
      klic_fprintf(stderr,
		   "%d perpetually suspending goals found\n",
		   suspensions-resumes-copied_susp);
    }
    /* First, we will copy the surface of suspended goal list.
       This is needed to distinguish resumed goals and
       goals copied while copying other goals in this list */
    while (sgl != 0) {
      if (sgl->goal->pred == 0 ||
	  /* already copied */
	  !isref(sgl->goal->next)
	  /* or not resumed yet */
	  ) {
	struct suspended_goal_rec *newsgr =
	  (struct suspended_goal_rec *)hp;
	hp += sizeof(struct suspended_goal_rec)/sizeof(q);
	*sgl_tail = newsgr;
	newsgr->goal = sgl->goal;
	sgl_tail = &newsgr->next;
      }
      sgl = sgl->next;
    }
    *sgl_tail = 0;
    /* Next, we will copy the suspended goals */
    sgl = suspended_goal_list;
    while (sgl != 0) {
      if (sgl->goal->pred != 0) {
	/* not copied yet */
	copy_one_goal(sgl->goal, gcsp, gcmax, 0);
	hp = copy_terms(hp, ntop, otop, nsize, osize, gcsp, gcmax);
	dead_goal = sgl->goal;
      } else {
	sgl->goal = sgl->goal->next;
      }
      sgl = sgl->next;
    }
    heapp = hp;
    if (dead_goal != 0) {
      /* we have to make the dead goal look like a normal ready queue */
      extern Const struct predicate queue_empty_pred;
      ((struct goalrec *)hp)->pred = &queue_empty_pred;
      dead_goal->next = (struct goalrec *)hp;
      hp += sizeof(struct goalrec)/sizeof(q);
      trace_deadlock(dead_goal);
    }
  }
#endif

  if (copied_susp != suspensions-resumes) {
    fatal("Perpetually suspending goal(s) found during GC");
  }

  for (k=0; k<num_after_gc_hooks; k++) {
    heapp = after_gc_hook_table[k](heapp);
  }

  return qp;
}

/*
  interface routine for copying one term
*/

q* copy_one_term(term, allocp, ntop, otop, nsize, osize)
     q *term;
     q *allocp, *ntop, *otop;
     unsigned long nsize, osize;
{
  declare_globals;
  push_gc_stack(term, gcsp, gcmax);
  return
    copy_terms(allocp, ntop, otop, nsize, osize, gcsp, gcmax);
}

/*
  for generic object
*/
q
general_gc(term, allocp, sp)
     q *term;
     q *allocp;
     q **sp;
{
  declare_globals;

  push_gc_stack(term, sp, gcmax);
  heapp = copy_terms(allocp, new_space_top, old_space_top,
		     new_space_size, old_space_size,
		     sp, gcmax);
  return *term;
}
