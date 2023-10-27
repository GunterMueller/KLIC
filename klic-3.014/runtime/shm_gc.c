/* ---------------------------------------------------------- 
%   (C)1994,1995 Institute for New Generation Computer Technology 
%       (Read COPYRIGHT for detailed information.) 
%   (C)1996, 1997, 1998, 1999 Japan Information Processing Development Center
%       (Read COPYRIGHT-JIPDEC for detailed information.)
----------------------------------------------------------- */

#include <stdio.h>
#include <klic/basic.h>
#include <klic/struct.h>
#include <klic/timing.h>
#include <klic/primitives.h>
#include <klic/functorstuffs.h>
#include <klic/gobj.h>
#include <klic/susp.h>
#include <klic/shm.h>
#include <time.h>

/* ------- Optinal Flags --------------------------------- */
/* FLAGS                                                                */
/*      IAFWD   ... Implicit All Forwarding                             */
/*      ISFWD   ... Implicit Single Forwarding                          */
/*                                                                      */
/*      PAR_GC  ... Parallel GC (Parallel Copy)                         */
/*      SEQ_GC  ... Sequential GC (Sequential Copy)                     */
/*                                                                      */
/*      ASYNC_GC... Asynchronous GC (On-the-fly GC)                     */
/*      SYNC_GC ... Synchronous GC (Stop-and-GC)                        */

/*
#define TOPDOWN
/**/
#define BOTTOMUP
/**/

/*
#define ASYNC_GC   /* Asynchronous GC */
/**/
#define SYNC_GC    /* Synchronous GC */
/**/

/*
#define PAR_GC     /* Parallel GC */
/**/
#define SEQ_GC     /* Sequential GC */
/**/

/*
#define IAFWD      /* Implicit All Forwarding */
/**/
#define ISFWD      /* Implicit Single Forwarding */
/**/

/* atomic swap(ptr,io) */
/* *ptr <-> io */
#define atomic_swap(ptr,io) ( {\
     register long  v  asm ("g2") = (long)io;   \
     register long* s1 asm ("g3") = (long*)ptr; \
     asm volatile ("                            \
     swap [%%g3],%%g2;                          \
" : "=g" (v) : "g" (v), "g" (s1) );             \
  io = (q)v;                                    \
  } )

/*
/* BARRIERS                                                             */
/*  a_barrier ... to make sure mutators may not see uninitialized cells.*/
/*      Inserted between element initialization and assignment of       */
/*       new structure to scavenging target cell.                       */
/*      (necessary only in Asynchronous GC)                             */
/*  p_barrier ... to make sure other collectors may not copy            */
/*      nested objects in parallel with the current collector.          */
/*      Inserted between forwarding and scavenging of nested objects.   */
/*      This may help but does not ensure avoiding duplicate copying... */
/*      (necessary only in Asynchronous Parallel GC)                    */
/*      (In Synchronous Parallel GC, duplicate copying is avoided by    */
/*      copy locking.)                                                  */
/*                                                                      */
#ifdef ASYNC_GC
#define a_barrier()     klic_barrier()
#else
#define a_barrier()
#endif

#if defined(PAR_GC) && defined(ASYNC_GC)
#define p_barrier()     klic_barrier()
#else
#define p_barrier()
#endif

/* For debugging */
#define ASSERT(cond,mess)      
/* \
        if (!(cond))            \
          fatal((sprintf(msgstr,"** ASSERT ERROR: %s. **\n",mess),msgstr));
*/

char msgstr[128];

q** make_shm_larger_stack();

#define push_shm_stack(addr,sp,max)                     \
{                                                       \
   if ( (sp) == max )  {                                \
    (sp) = make_shm_larger_stack(sp);                   \
    max = shm_gcmax;                                    \
  }                                                     \
  *(sp)++ = (q*)(addr);                                 \
}

#define pop_gc_command()    (q)(*--sp)
#define push_gc_command(x)  push_shm_stack(x,sp,max)

#define push_VALUE(val)     push_gc_command(val)

#define push_UPDATE(addr)   push_gc_command(makeatomic(addr))

/*              == GC STACK COMMANDS ==                                 */
/*                                                                      */
/*  SCAVENGE(x) (identified by VARREF tag)                              */
/*    q *x ... pointer to (q) in new local or new shared heap           */
/*    action: copy *x to new shared heap and update *x                  */
/*                                                                      */
#define push_SCAVENGE(addr) push_gc_command(makeref(addr))

#define within_new_space(X) (\
  (unsigned long)((long)(X) - (long)(PLNE_ptr[currid]->top_addr) ) < SHM_SIZE )

/* Global varibles */
int shm_gc_times = 0;
int measure_shmgc = 1;
int F_shm_gc;
q** shm_gcstack;
q** shm_gcmax;
q** shm_sp;
int shm_gcstack_size;

extern struct ex_goalrec** top_shm_next;
extern struct ex_goalrec** shm_qp;

extern struct goalrec goal_queue_tail;

/* checks shared-memory GC. called by "intrpt" routine */
int ck_shm_gc()
{
  declare_globals;
  if ( oldid != currid ) {
    F_shm_gc = 1;
    return(1);
  } else if ( PLNE_ptr[currid]->caddr > PLNE_ptr[currid]->limit1 ) {
    F_shm_gc = 1;
    return(1);
  } else {
    return(0);
  }
}



#define chase_queue(qp, sp, max)   (\
 {\
  struct goalrec top;                                            \
  struct goalrec* before = &top;                                 \
  top.next = qp;                                                 \
  for (; qp!=&goal_queue_tail; before=qp,qp=qp->next) {          \
    if ( is_shma(qp) && !within_new_space(qp) ) {                \
        int i,arity=qp->pred->arity;                             \
        struct goalrec* newg = (struct goalrec*)galloc(arity+2); \
        newg->next = qp->next;                                   \
        newg->pred = qp->pred;                                   \
        for(i=0;i<arity;i++) {                                   \
	  if ( !isatomic( newg->args[i] = qp->args[i] ) ) {      \
            push_SCAVENGE(&(newg->args[i]));                     \
	  }                                                      \
        }                                                        \
        before->next = qp = newg;                                \
    }                                                            \
  };                                                             \
 top.next;} )

#define chase_ex_queue(exqp,sp,max) {              \
   struct ex_goalrec* eqp;                         \
   struct ex_goalrec* before =                     \
      (struct ex_goalrec*)(top_shm_next+my_node);  \
   before->next = exqp->next;                      \
   exqp = before;                                  \
   eqp = before->next;                             \
   for(; eqp; before=eqp,eqp=eqp->next) {          \
      if ( !within_new_space(eqp) ) {              \
         int i,arity=eqp->goal.pred->arity;        \
         struct ex_goalrec* newg = (struct ex_goalrec*)galloc(arity+4); \
                                                   \
         newg->next = eqp->next;                   \
         newg->prio = eqp->prio;                   \
         newg->goal.pred = eqp->goal.pred;         \
         for(i=0;i<arity;i++) {                    \
           if ( !isatomic( newg->goal.args[i] = eqp->goal.args[i] ) ) {\
             push_SCAVENGE(&(newg->goal.args[i])); \
	   }                                       \
         }                                         \
         before->next = eqp = newg;                \
      }                                            \
   }                                               \
   *shm_qp = before;                               \
}

struct goalrec* shm_gc(qp)
struct goalrec* qp;
{
   declare_globals;
   struct prioqrec *pq = prioq.next;

   struct timeval before, after;
   int i,j;
   q** sp = shm_sp;
   q** max = shm_gcmax;
   int qkey;
   int key = gc_key();
   *cur_status = 4;

/*   printf("The %dth process Shared memory GC start!!\n",my_node); */
   shm_gc_times++;
#ifdef TOPDOWN
   if (measure_shmgc) {  gettimeofday(&before); }
#endif

   enter_shm_gc();
/* chases my external pool */
   qkey = p_key(my_node);
   s_lock(qkey);
   chase_ex_queue(ex_qp,sp,max);
   s_unlock(qkey);

/* chase qp */
   for(; pq->prio >= 0; pq = pq->next) {
     pq->q = chase_queue(pq->q, sp,max);
   }
   qp = chase_queue(qp, sp,max);

   shm_copy_terms(sp,max);

   exit_shm_gc();

#ifdef TOPDOWN
   if (measure_shmgc) {
     gettimeofday(&after);
     gcums += (after.tv_sec - before.tv_sec)*1000000+(after.tv_usec-before.tv_usec);
   }
#endif
   *cur_status = 3;
   return(qp);
}

#ifdef TOPDOWN
/*----------------------------------------------------------------------*/
/* TOP DOWN TERM COPY		1994/09/20 N.Ichiyoshi			*/
/*----------------------------------------------------------------------*/

/*		== FORWARDING SCHEMES ==				*/
/*									*/
/*  -- Before Copying and Forwarding --					*/
/*									*/
/*	    (in old space)						*/
/*									*/
/*		+-------+						*/
/*		|  FH	| (functor header)				*/
/*		+-------+						*/
/*		|  X0	| (0-th argument)				*/
/*		+-------+						*/
/*		|  X1	| (1-st argument)				*/
/*		+-------+						*/
/*		   ...							*/
/*		+-------+						*/
/*		|  Xn	| (n-th argument, n = arify)			*/
/*		+-------+
/*									*/
/*  -- After Surface-Level Copying and Forwarding --			*/
/*									*/
/*		*** Implicit All Forwarding (IAFWD) ***			*/
/*									*/
/*	     (in old space)	      (in new space)			*/
/*									*/
/*		+-------+		+-------+			*/
/*		|  FH	|		|  FH	|			*/
/*		+-------+		+-------+			*/
/*		| REF +--------------->	|  X0	|			*/
/*		+-------+		+-------+			*/
/*		| REF +--------------->	|  X1	|			*/
/*		+-------+		+-------+			*/
/*		   ...			   ...				*/
/*		+-------+		+-------+			*/
/*		| REF +--------------->	|  Xn	|			*/
/*		+-------+		+-------+			*/
/*									*/
/*		*** Implicit Single Forwarding (ISFWD) ***		*/
/*									*/
/*	     (in old space)	      (in new space)			*/
/*									*/
/*		+-------+		+-------+			*/
/*		|  FH	|		|  FH	|			*/
/*		+-------+		+-------+			*/
/*		| REF +--------------->	|  X0	|			*/
/*		+-------+		+-------+			*/
/*		| PX1	|		|  X1	|			*/
/*		+-------+		+-------+			*/
/*		   ...			   ...				*/
/*		+-------+		+-------+			*/
/*		| PXn	|		|  Xn	|			*/
/*		+-------+		+-------+			*/
/*									*/
/*	PXi = Xi, 		       if Xi is atomic,			*/
/*	      REF to new i-th element, if Xi is not atomic		*/
/*									*/
/*  FORWARDING POINTER CELLS
/*   - For CONS record, cdr contains forwarding pointer.		*/
/*   - For normal FUNCTOR record, args[0] contains forwarding pointer.	*/
/*									*/
/*  ASSUMPTIONS								*/
/*   (1) The mutator process must not overwrite active cells in		*/
/*	 shared heap other than unbound shared variables.		*/
/*	 (In particular, it is assumed that there is no destructive	*/
/*	 update of structure elements.)					*/
/*   (2) Under ISF, the mutator must not put REF pointer to new space	*/
/*	 in a forwarding pointer cell.					*/
/*   (3) There is no functor with arity = 0.				*/
/*									*/

/*		== OFFSET VALUES (in number of q cells) == 		*/
/*									*/
/*  CDROFFSET	    ... offset of CDR from top of CONS record (=0)	*/
/*  FIRSTELTOFFSET  ... offset of arg[0] from top of FUNCTOR record (=1)*/
/*  CARCDROFFSET    ... offset of CAR from CDR (=1)			*/
/*									*/
/* N.B. In gcc, these are compiled as constants under -O option.	*/
/*									*/
struct cons dummycons;
struct functor dummyfunc;
#define CDROFFSET	((q*)&(dummycons.cdr)- (q*)&dummycons)
#define CARCDROFFSET	((q*)&(dummycons.car)- (q*)&(dummycons.cdr))
#define FIRSTELTOFFSET	((q*)&(dummyfunc.args[0])- (q*)&dummyfunc)

#define qsizeof_cons	    (sizeof(struct cons)/sizeof(q))	   /* 2 */
#define qsizeof_functor(n)  (sizeof(struct functor)/sizeof(q)+n-1) /* n+1 */

/*			== COPY LOCK ==					*/
/*									*/
/*  In synchronous parallel GC, objects are locked in order to avoid	*/
/*  duplicate copying.							*/
/*									*/
#if defined(SYNC_GC) && defined(PAR_GC)
#define COPYING_MARK	makeref(sizeof(q))  /* impossible as normal object */
#else
#define lock_for_copying(fwdptraddr,offset,taggingfunc)
#endif

shm_copy_terms(sp,max)
q** sp;
q** max;
{
  declare_globals;
  q *objp, obj;
  while (sp > shm_gcstack) {
    (q)objp = pop_gc_command();
    ASSERT( isref(objp), "Shared heap topdown GC command error.");
    objp = refp(objp);	/* objp points to scavenging target cell */
    obj = *objp;	/* obj is content of scavenging target cell */

  TRO_LOOP:

    ASSERT( !is_shma(objp) || within_new_space(objp), "objp in old space");
    ASSERT( obj == *objp, "obj != *objp");

    switch ( ptagof(obj) ) {
    case CONS: {
      struct cons *old, *new;
      old = consp(obj);
					/* in new space ? */
      if ( within_new_space(old) ) continue;
					/* already copied ? */
#ifdef ISFWD
      if ( isref(old->cdr) && within_new_space(refp(old->cdr)) ) {
	*objp = makecons(refp(old->cdr)-CDROFFSET);
	continue;
      }
#endif
#ifdef IAFWD
      if ( isref(old->cdr) && within_new_space(refp(old->cdr)) &&
	   (old->car == makeref(refp(old->cdr)+CARCDROFFSET)) ) {
	*objp = makecons(refp(old->cdr)-CDROFFSET);
	continue;
      }
#endif
#if defined(SYNC_GC) && defined(PAR_GC)
      {
	q x = COPYING_MARK;
	volatile q* fpp = &(old->cdr);
	int offset = CDROFFSET;
	atomic_swap(fpp,x);
	if ( x == COPYING_MARK ) {
	  /* being copied by somebody else */
	  while ( x == COPYING_MARK ) x = *fpp;
	  ASSERT( isref(x) && within_new_space(refp(x)), "bad fwd");
	  *objp = makecons(refp(x)-offset);
	  continue;
	}
					/* allocation */
	new = (struct cons*)galloc(qsizeof_cons);
	new->cdr = x;			/* initialization of fwdptr position */
	new->car = old->car;
      }
#else
					/* allocation */
      new = (struct cons*)galloc(qsizeof_cons);
					/* initialization */
      new->cdr = old->cdr;
      new->car = old->car;
#endif
					/* update of scavenging target */
      a_barrier();
      *objp = makecons(new);
					/* forwarding */
      old->cdr = makeref(&(new->cdr));
#ifdef IAFWD
      old->car = makeref(&(new->car));
#endif
					/* nested scavenging */
      p_barrier();
      if ( isatomic(new->car) ) {
	   /* N.B. In IAFWD, new->cdr is already REF to new->car. */
	objp = &(new->cdr);
	obj = *objp;
      } else {
	push_SCAVENGE(&(new->cdr));
#ifdef ISFWD
	old->car = makeref(&(new->car));
#endif
	objp = &(new->car);
	obj = *objp;
      }
      goto TRO_LOOP;
    }
    case ATOMIC: {
      continue;
    }
    case VARREF: {
      q temp;
      if ( within_new_space(refp(obj)) ) continue;
      temp = derefone(obj);
      if ( isref(temp) && obj == derefone(temp) ) {	/* double loop */
        q svar;
        struct generator_susp* vcell;
        struct generator_object* addi;
        Shvar* gvar;
        addi = n_lock(obj,temp);
        if ( derefone(obj) != temp ) {
          obj = derefone(obj);
          *objp = obj; goto TRO_LOOP;
        }
        svar = (q)galloc((sizeof(struct generator_susp)/sizeof(q))+1);
        vcell = (struct generator_susp*)(((q*)svar)+1);
        vcell->backpt = (q)svar;
        derefone(svar) = (q)vcell;
        gvar = (Shvar*)untag_generator_susp(addi);
        if ( !gvar->chain ) {
           vcell->u.o = tag_generator_susp(pSHM_VAR);
        } else if ( within_new_space(gvar) ) {
           vcell->u.o = tag_generator_susp(gvar);
        } else {
           Sinfo* dptr;
           Sinfo* sptr;
           Shvar* svar = (Shvar*)galloc((sizeof(Shvar)/sizeof(q)));
           vcell->u.o = tag_generator_susp(svar);
           svar->method = SHM_VAR;
           dptr = (Sinfo*)&(svar->chain);
           sptr = gvar->chain;
           if ( !is_genhook(sptr) ) {
             while(sptr && !within_new_space(sptr)) {
               dptr->next = (Sinfo*)galloc((sizeof(Sinfo)/sizeof(q)));
               dptr = dptr->next;
               dptr->PE_num = sptr->PE_num;
               dptr->prio = sptr->prio;
               dptr->indp = sptr->indp;
               sptr = sptr->next;
             }
             dptr->next = sptr;
           } else {
             if ( !within_new_space(sptr) ) {
               Sinfo* addr = (Sinfo*)galloc((sizeof(Sinfo)/sizeof(q)));
               sptr = untag_genhook(sptr);
               dptr->next = tag_genhook(addr);
               dptr = addr;
               dptr->next = 0;
               dptr->PE_num = sptr->PE_num;
               dptr->prio = sptr->prio;
               dptr->indp = sptr->indp;
             } else {
               dptr->next = sptr;
             }
           }
         }
	a_barrier();
	derefone(obj) = makeref(svar);
	*objp = makeref(svar);
	continue;
      } else { /* obj is an invisible pointer */	  
	*objp = obj = temp;
	goto TRO_LOOP;
      }
    }
    case FUNCTOR: {
      int i,arity;
      q f;
      struct functor *old, *new;
      old = functorp(obj);
					/* in new space ? */
      if ( within_new_space(old) ) continue;
      f = functor_of(obj);
      if ( isref(f) ) {
	/*--- GENERIC OBJECT (data object) ---*/
	struct data_object *dobj = (struct data_object *)old;
	obj = generic_shmcopy(dobj);
	a_barrier();
	*objp = obj;
	continue;
      }
      /*--- NORMAL FUNCTOR ---*/
					/* already copied ? */
#ifdef ISFWD
      /* arity > 0 is assumed */
      if ( isref(old->args[0]) && within_new_space(refp(old->args[0])) ) {
	new = (struct functor*)(refp(old->args[0])-FIRSTELTOFFSET);
	*objp = makefunctor(new);
	continue;
      }
      arity = arityof(f);
#endif
#ifdef IAFWD
      arity = arityof(f);
      if ( isref(old->args[0]) && within_new_space(refp(old->args[0])) ) {
	q* neweltp = refp(old->args[0])+1;
	int copied = 1;
	for (i=1; i<arity; i++,neweltp++) {
	  if ( old->args[i] != makeref(neweltp) ) {
	    copied = 0;
	    break; /* exit for statement */
	  }
	  /* N.B. If arity is extremely large, neweltp might overflow.	*/
	  /*   But in that case, makeref(neweltp) becomes makeref(0),	*/
	  /*   which cannot be equal to old->args[i].			*/
	}
	if ( copied ) {
	  new = (struct functor*)(refp(old->args[0])-FIRSTELTOFFSET);
	  *objp = makefunctor(new);
	  continue;
	}
      } /* end if */
#endif
#if defined(SYNC_GC) && defined(PAR_GC)
      {
	q x = COPYING_MARK;
	volatile q* fpp = &(old->args[0]);
	int offset = FIRSTELTOFFSET;
	atomic_swap(fpp,x);
	if ( x == COPYING_MARK ) {
	  /* being copied by somebody else */
	  while ( x == COPYING_MARK ) x = *fpp;
	  ASSERT( isref(x) && within_new_space(refp(x)), "bad fwd");
	  *objp = makefunctor(refp(x)-offset);
	  continue;
	}
					/* allocation */
	new = (struct functor*)galloc(qsizeof_functor(arity));
					/* initialization */
	new->functor = f;
	new->args[0] = x;		/* initialization of fwdptr position */
	for(i=1; i<arity; i++) {
	  new->args[i] = old->args[i];
	}
      }
#else
					/* allocation */
      new = (struct functor*)galloc(qsizeof_functor(arity));
					/* initialization */
      new->functor = f;
      for(i=0; i<arity; i++) {
	new->args[i] = old->args[i];
      }
#endif
					/* update of scavenging target */
      a_barrier();
      *objp = makefunctor(new);
					/* forwarding */
      old->args[0] = makeref(&(new->args[0]));
#ifdef IAFWD
      for(i=1; i<arity; i++) {
	old->args[i] = makeref(&(new->args[i]));
      }
#endif
					/* nested scavenging */
      p_barrier();
      for (i=arity-1; i>0; i--) {
	if ( ! isatomic(new->args[i]) ) {
#ifdef ISFWD
	  old->args[i] = makeref(&(new->args[i]));
	
#endif
	  push_SCAVENGE(&(new->args[i]));
	}
      }
      objp = &(new->args[0]);
      obj = *objp;
      goto TRO_LOOP;
    } /* case FUNCTOR */
    default: {
      ASSERT(0,"Impossible case");
    }
    } /* switch statement */
  } /* while (sp > shm_gcstack) */
}
#endif /* TOPDOWN */
#ifdef BOTTOMUP
shm_copy_terms(sp,max)
q** sp;
q** max;
{
   declare_globals;
   q obj;
   while(sp > shm_gcstack) {
     q *objp = *--sp;
      switch ( ptagof(objp) ) {
      case ATOMIC: { /* in case of UPDATE */
         q* conta = (q*)((long)(objp)-ATOMIC);
         klic_barrier();
         *conta = obj;
         break;
      }
      case CONS:
      case FUNCTOR: {
         obj = (q)objp;
         break;
      }
      default: {
         obj = *objp;

      ENTRY:
         switch(ptagof(obj)) {
         case CONS: {
           struct cons* new;
	   if ( !is_shma(obj) ) break; /* assume text-area */
           if ( within_new_space(obj) ) break;
           new = (struct cons*)galloc(2);
           *objp = makecons(new);
	   push_VALUE(makecons(new));
           if ( !isatomic( new->cdr = cdr_of(obj) ) ) {
	     push_UPDATE(&cdr_of(obj));
	     push_SCAVENGE(&new->cdr);
	   }
	   if ( isatomic( new->car = car_of(obj) ) ) break;
           push_UPDATE(&car_of(obj));
           objp = &new->car;
           obj = *objp;
           goto ENTRY;
         }
         case ATOMIC: { break; }
         case VARREF: {
           q temp;
           if ( within_new_space(obj) ) { break; }
           temp = derefone(obj);
           if ( isref(temp) && obj == derefone(temp) ) {
             q svar;
             struct generator_susp* vcell;
             struct generator_object* addi;
             Shvar* gvar;
             addi = n_lock(obj,temp);
             if ( derefone(obj) != temp ) {
               obj = derefone(obj);
               *objp = obj; goto ENTRY;
             }
             svar = (q)galloc((sizeof(struct generator_susp)/sizeof(q))+1);
             vcell = (struct generator_susp*)(((q*)svar)+1);
             vcell->backpt = (q)svar;
             derefone(svar) = (q)vcell;
             gvar = (Shvar*)untag_generator_susp(addi);
             if ( !gvar->chain ) {
               vcell->u.o = tag_generator_susp(pSHM_VAR);
             } else if ( within_new_space(gvar) ) {
               vcell->u.o = tag_generator_susp(gvar);
             } else {
               Sinfo* dptr;
               Sinfo* sptr;
               Shvar* svar = (Shvar*)galloc((sizeof(Shvar)/sizeof(q)));
               vcell->u.o = tag_generator_susp(svar);
               svar->method = SHM_VAR;
               dptr = (Sinfo*)&(svar->chain);
               sptr = gvar->chain;
               if ( !is_genhook(sptr) ) {
                 while(sptr && !within_new_space(sptr)) {
                   dptr->next = (Sinfo*)galloc((sizeof(Sinfo)/sizeof(q)));
                   dptr = dptr->next;
                   dptr->PE_num = sptr->PE_num;
                   dptr->prio = sptr->prio;
                   dptr->indp = sptr->indp;
                   sptr = sptr->next;
                 }
                 dptr->next = sptr;
               } else {
                 if ( !within_new_space(sptr) ) {
                   Sinfo* addr = (Sinfo*)galloc((sizeof(Sinfo)/sizeof(q)));
                   sptr = untag_genhook(sptr);
                   dptr->next = tag_genhook(addr);
                   dptr = addr;
                   dptr->next = 0;
                   dptr->PE_num = sptr->PE_num;
                   dptr->prio = sptr->prio;
                   dptr->indp = sptr->indp;
                 } else {
                   dptr->next = sptr;
                 }
               }
             }
             klic_barrier();
             derefone(obj) = (q)svar;
             *objp = obj = (q)svar;
             break;
           }

           { /* in case of an invisible pointer */
              *objp = obj = temp;
              goto ENTRY;
           }
         }
         default: { /* functor */
           int i,arity,aritym1;
           struct functor* new;
           q f;
	   if ( !is_shma(obj) ) break; /* assume text-area */
           if ( within_new_space(obj) ) break;
           f = functor_of(obj);
           if ( isref(f) ) {
              struct data_object *dobj
               = (struct data_object *)functorp(obj);
              obj = generic_shmcopy(dobj);
              klic_barrier();
              *objp = obj;
              break;
           }
           arity = arityof(f);
           aritym1 = arity-1;
           new = (struct functor*)galloc(arity+1);
           new->functor = f;
           *objp = makefunctor(new);
           push_VALUE(makefunctor(new));
           for(i=0;i<aritym1;i++) {
             if ( !isatomic( new->args[i] = arg(obj,i) ) ) {
	       push_UPDATE(&arg(obj,i));
	       push_SCAVENGE(&(new->args[i]));
	     }
           }
	   if ( !isatomic( new->args[aritym1] = arg(obj,aritym1) ) ) {
	     push_UPDATE(&arg(obj,aritym1));
	     objp = &(new->args[aritym1]);
	     obj = *objp;
	     goto ENTRY;
	   }
         }
         }/* switch(ptogof(obj)) */
      }
      }/* switch(ptagof(objp)) */
   }/* while( sp...) */
}
#endif /* BOTTOMUP */


get_new_plane()
{
   declare_globals;
   int i,id,j;
   s_lock(m_key());
   if ( PLNE_ptr[currid]->direct == -1 ) {
      id = (currid+1) % N_PLNE;
      for(j=0;j<total_node;j++) {
	if (PLNE_ptr[id]->proc[j]) {
	  abend("Shared Memory overflow !!!");
        }
      }

      /* initialize table */
      PLNE_ptr[id]->caddr = PLNE_ptr[id]->top_addr;
      PLNE_ptr[id]->demand = 0;
      PLNE_ptr[id]->direct = -1;
      klic_barrier();
      PLNE_ptr[currid]->direct = id;
   } else {
     id = PLNE_ptr[currid]->direct;
   }
   s_unlock(m_key());

   currid = id;
   PLNE_ptr[currid]->proc[my_node] = 1;
   gallocp = glimit = (q*)1;
}

enter_shm_gc()
{
  declare_globals;
#ifdef ASYNC_GC
   if ( currid == oldid) {
     PLNE_ptr[currid]->proc[my_node] = 3;
     if ( PLNE_ptr[currid]->direct == -1 ) {
       get_new_plane();
     } else {
       currid = PLNE_ptr[currid]->direct;
       PLNE_ptr[currid]->proc[my_node] = 2;
       gallocp = glimit = (q*)1;
     }
   } else {
     PLNE_ptr[oldid]->proc[my_node] = 10;
   }
   {
     int j,next;
     next = (currid+1) % N_PLNE;
     for(j=0;j<total_node;j++) {
       if ( PLNE_ptr[next]->proc[j] ) {
	 struct global_variables* tglbl = top_shm_glbl+j;
	 tglbl->interrupt_off0 = 0;
	 klic_barrier();
	 tglbl->heaplimit0 = 0;
       }
       {
	 int waitC=1000000;
	 while (1) {
           if ( !(PLNE_ptr[next]->proc[j]) ) break;
	   if ( !(waitC--) ) abend("Shared-memory overflow!!!");
	 }
       }
     }
     oldid = currid;
   }
#endif
#ifdef SYNC_GC
  {
    int i,j;
    if ( !PLNE_ptr[currid]->demand ) {
      s_lock(m_key());
      if ( !PLNE_ptr[currid]->demand ) {
	PLNE_ptr[currid]->demand = 1;
	for(j=0;j<total_node;j++) {
	  if ( PLNE_ptr[currid]->proc[j] ) {
	    struct global_variables* tglbl = top_shm_glbl+j;
	    tglbl->interrupt_off0 = 0;
	    klic_barrier();
	    tglbl->heaplimit0 = 0;
	  }
	}
	PLNE_ptr[1-currid]->caddr = PLNE_ptr[1-currid]->top_addr;
	PLNE_ptr[1-currid]->demand = 0;
      }
      s_unlock(m_key());
    }
    PLNE_ptr[currid]->proc[my_node] = 0;
    for(i=0;i<total_node;i++) while( PLNE_ptr[currid]->proc[i] ) {}
    currid = 1 - currid;
    gallocp = glimit = (q*)1;
    
  }
#endif
#ifdef SEQ_GC
  s_lock(gc_key());
#endif
}

exit_shm_gc()
{
  declare_globals;
#ifdef SEQ_GC
  s_unlock(gc_key());
#endif
#ifdef SYNC_GC
  {
    int i;
    PLNE_ptr[currid]->proc[my_node] = 1;
    for(i=0;i<total_node;i++) while( !PLNE_ptr[currid]->proc[i] ) {}
  }
#endif
#ifdef ASYNC_GC
  {
    int i;
    for(i=0;i<N_PLNE;i++) {
      if ( i != currid && i != oldid )  PLNE_ptr[i]->proc[my_node] = 0;
    }
    PLNE_ptr[currid]->proc[my_node] = (-1);
  }
#endif
  F_shm_gc = 0;
}

