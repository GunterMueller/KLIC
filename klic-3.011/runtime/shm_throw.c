/* ---------------------------------------------------------- 
%   (C)1994,1995 Institute for New Generation Computer Technology 
%       (Read COPYRIGHT for detailed information.) 
%   (C)1996, 1997, 1998, 1999 Japan Information Processing Development Center
%       (Read COPYRIGHT-JIPDEC for detailed information.)
----------------------------------------------------------- */

#include <klic/basic.h>
#include <klic/struct.h>
#include <klic/primitives.h>
#include <klic/unify.h>
#include <klic/functorstuffs.h>
#include <klic/susp.h>
#include <klic/gobj.h>
#include <klic/shm.h>
#include <signal.h>

q last_shm_var;
struct goalrec *trace_goal();

/* throw_goal_routine(qp,gp,Logical_process_number)
*/
struct goalrec* throw_goal_routine(N,qp,gp)
int N;
struct goalrec *qp,*gp;
{
  declare_globals;
  extern int trace_flag;
  struct goalrec *ng;
  int k;
  if ( is_shmconnect(N) ) {
    int num = phisc_pe(N);
    struct ex_goalrec *sh_goal;
    *cur_status = 2;

    ck_throw_new_galloc(num);

#ifdef DEBUGLIB
    if (trace_flag) {
      static long throw_trace_count = 0;
      gp = trace_goal(gp, (num+1)*100000+throw_trace_count,0);
      throw_trace_count++;
    }
#endif

    sh_goal = shm_goal_copy(gp,current_prio);
    shm_goal_stack(sh_goal,num);
    *cur_status = 1;
  } else {
    abend(" Not implemeted yet..\n");
  }
  return(qp);
}


/* shm_goal_copy(Local_goal_pointer,priority)
  copy a goal into shared memory
*/
struct ex_goalrec *shm_goal_copy(qp,prio)
struct goalrec *qp;
long prio;
{
    int i,j;
    int arity = qp->pred->arity;
    struct ex_goalrec *sgoal;
    struct goalrec *goal;
    sgoal = (struct ex_goalrec*)galloc(arity+4);
    sgoal->prio = prio;
    goal = &sgoal->goal;
    goal->next = 0;
    goal->pred = qp->pred;
    for(i=0;i<arity;i++) {
       shm_arg_copy(&qp->args[i],&goal->args[i]);
    }
    return(sgoal);
}

/* shm_goal_stack(Shared_goal_pointer,stack_position)
  stack a goal to shared other process's stack
*/
shm_goal_stack(goal,num)
struct ex_goalrec* goal;
int num;
{
    struct ex_goalrec* tqp;
    int key = p_key(num);

    goal->next = 0;
    klic_barrier();
    s_lock(key);

    tqp = *(top_shm_qp+num);
    tqp->next = goal;
    *(top_shm_qp+num) = goal;

    s_unlock(key);

    {
      struct global_variables* tglbl = get_otherPE_glbl(num);
      if (tglbl->current_prio0 < goal->prio ) {
         tglbl->par.aux.shm.queued0 = 1;
         tglbl->interrupt_off0 = 0;
         klic_barrier();
         tglbl->heaplimit0 = 0;
      } else {
         tglbl->par.aux.shm.queued0 = 1;
         if (tglbl->current_prio0 < goal->prio ) {
                 tglbl->interrupt_off0 = 0;
                 klic_barrier();
                 tglbl->heaplimit0 = 0;
         }
      }
      klic_barrier();
      tglbl->par.aux.shm.queued0 = 1;
    }
}

/* phisc_pe(Logical_process_number)
  covert a logical process number to a phisical shared process number
*/
int phisc_pe(NO)
int NO;
{
    declare_globals;
    if ( NO >= 0 && NO < total_node )
       return(NO);
    else
       abend("Illegal PE number !!!");
}

/* ***************  Copy Routines  ****************************** */

#define ck_arg_copy(x,y) {\
   if ( isatomic(x) ) y = x;\
   else if ( isref(x) && x == (q)&(x) ) {\
     q sv;\
     makeshvar(sv);\
     y = x = last_shm_var = sv;\
   } else {\
     shm_arg_copy(&x,&y);\
   }\
}

#define ck_shm_arg_copy(x,y) {\
   if ( isatomic(x) ) y = x;\
   else {\
     shm_arg_copy(&x,&y);\
   }\
}

/* q shm_copy(SrcAddr)
*/

q shm_copy(src)
q src;
{
  declare_globals;
   q cnp;
   q retval;
 ENTRY:
   switch(ptagof(src)) {
    case CONS:
     if ( is_locala(src) ) {
       cnp = (q)galloc(2);
       retval = makecons(cnp);
       ck_arg_copy(car_of(src),car_of(retval));
       ck_arg_copy(cdr_of(src),cdr_of(retval));
       return( retval );
     } else if ( is_shma(src) ) {
       if ( is_cur_or_forward_ptr(src) )  return(src);
       /* shared-memory to shared-memory copy */
       cnp = (q)galloc(2);
       retval = makecons(cnp);
       ck_shm_arg_copy(car_of(src),car_of(retval));
       ck_shm_arg_copy(cdr_of(src),cdr_of(retval));
       return( retval );
     } else { /* assume text-area */
       return(src);
     }
    case ATOMIC:
      return(src);
    case VARREF: {
        q temp = derefone(src);
        if ( !isref(temp) ) { src = temp; goto ENTRY; }
        if ( src == temp ) {
           makeshvar(temp);
           return( derefone(src) = temp );
        } else {
           q temp1 = derefone(temp);
           if ( !isref(temp1) ) { src = temp; goto ENTRY; }
           if ( src == temp1 ) {
               declare_globals;
               if ( is_shma(src) ) { /* shared memory */
                 if ( is_cur_or_forward_ptr(src) ) {
                   return( src );
                 } else { /* Shared-memory to shared-memory movement */
                   q svar;
                   struct generator_susp* vcell;
                   struct generator_object* addi;
                   Shvar* gvar;
                   addi = n_lock(src,temp);
                   if ( derefone(src) != temp ) goto ENTRY;
                   svar = (q)galloc((sizeof(struct generator_susp)/sizeof(q))+1);
                   vcell = (struct generator_susp*)(((q*)svar)+1);
                   vcell->backpt = (q)svar;
                   derefone(svar) = (q)vcell;
                   gvar = (Shvar*)untag_generator_susp(addi);
                   if ( !gvar->chain ) {
                     vcell->u.o = tag_generator_susp(pSHM_VAR);
                     klic_barrier();
                     derefone(src) = svar;
                     return(svar);
                   }
                   if ( is_cur_or_forward_ptr(gvar) ) {
                     vcell->u.o = tag_generator_susp(gvar);
                     klic_barrier();
                     derefone(src) = svar;
                     return(svar);
                   } else {
                     Sinfo* dptr;
                     Sinfo* sptr;
                     Shvar* svar = (Shvar*)galloc((sizeof(Shvar)/sizeof(q)));
                     vcell->u.o = tag_generator_susp(svar);
                     svar->method = SHM_VAR;
                     dptr = (Sinfo*)&(svar->chain);
                     sptr = gvar->chain;
                     if ( !is_genhook(sptr) ) {
                       while(sptr && !is_cur_or_forward_ptr(sptr)) {
                         dptr->next = (Sinfo*)galloc((sizeof(Sinfo)/sizeof(q)));
                         dptr = dptr->next;
                         dptr->PE_num = sptr->PE_num;
                         dptr->prio = sptr->prio;
                         dptr->indp = sptr->indp;
                         sptr = sptr->next;
                       }
                       dptr->next = sptr;
                     } else {
                       if ( !is_cur_or_forward_ptr(sptr) ) {
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
                   derefone(src) = svar;
                   return( svar );
                 }
               } else {              /* local memory */
                 Shvar* hobj;
                 struct susprec *sp = (struct susprec *)temp;
                 makeshvar(retval);
                 if ( is_generator_susp(sp->u) ) {
                   hobj = create_genhook(tag_local(src),retval);
                 } else {
                   hobj = shm_add_consumer(retval,0,src,glbl);
                 }
                 *(((struct generator_object**)retval+2)) = tag_generator_susp(hobj);
                 return( retval );
               }
           }
           src = temp;
           goto ENTRY;
        }
      }
    default:
      {  q f = functor_of(src);
         int i,arity;

	 if ( is_locala(src) ) {
	   if ( isref(f) ) {
	     struct data_object *obj
	       = (struct data_object *)functorp(src);
	     /* abend("Copied data object into shared memory!!!"); */
	     return( generic_shmcopy(obj) );
	   }
	   arity = arityof(f);
	   cnp = (q)galloc(arity+1);
	   retval = makefunctor(cnp);
	   ((struct functor*)cnp)->functor = functorp(src)->functor;
	   for(i=0;i<arity;i++) {
	     ck_arg_copy(functorp(src)->args[i],((struct functor*)cnp)->args[i]);
	   }
	   return( retval );
	 } else if ( is_shma(src) ) {
           if ( is_cur_or_forward_ptr(src) )  return( src );
           /* Shared-memory to shared-memory movement */
           if ( isref(f) ) {
             struct data_object *obj
                = (struct data_object *)functorp(src);
              /* abend("Copied data object into shared memory!!!"); */
             return( generic_shmcopy(obj) );
           }
           arity = arityof(f);
           cnp = (q)galloc(arity+1);
           retval = makefunctor(cnp);
           ((struct functor*)cnp)->functor = functorp(src)->functor;
           for(i=0;i<arity;i++) {
             ck_shm_arg_copy(functorp(src)->args[i],((struct functor*)cnp)->args[i]);
           }
           return( retval );
         } else { /* assume text-area */
	   return( src );
	 }
       }
   }
}

/* shm_arg_copy(SrcAddr,DistAddr)
  copy data into shared memory recursively
*/
shm_arg_copy(srcp,distp)
q* srcp;
q* distp;
{
  declare_globals;
  q src,cnp;
  int i;

  ENTRY:
    src = *srcp;
    switch(ptagof(src)) {
    case CONS:
      if ( is_locala(src) ) {
	cnp = (q)galloc(2);
	*distp = makecons(cnp);
	shm_arg_copy(&car_of(src),&(((struct cons*)cnp)->car));
	distp = &(((struct cons*)cnp)->cdr);
	srcp = &cdr_of(src);
	goto ENTRY;
      } else if ( is_shma(src) ) {
        if ( is_cur_or_forward_ptr(src) ) { *distp = src; return; }
        cnp = (q)galloc(2);
        shm_arg_copy(&car_of(src),&(((struct cons*)cnp)->car));
        shm_arg_copy(&cdr_of(src),&(((struct cons*)cnp)->cdr));
        klic_barrier();
        *distp = *srcp = makecons(cnp); /* patch */ return;
      } else { /* assume text-area */
	*distp = src; return;
      }
    case ATOMIC:
      *distp = src;
      return;
    case VARREF: {
        q temp = derefone(src);
        if ( !isref(temp) ) { srcp = (q*)src; goto ENTRY; }
        if ( src == temp ) {
           makeshvar(temp);
           *distp = derefone(src) = temp;
           return;
        } else {
           q temp1 = derefone(temp);
           if ( !isref(temp1) ) { srcp = (q*)src; goto ENTRY; }
           if ( src == temp1 ) {
               declare_globals;
               if ( is_shma(src) ) { /* a shared variable */
                 if ( is_cur_or_forward_ptr(src) ) {
                   *distp = src; return;
                 } else {
                   q svar;
                   struct generator_susp* vcell;
                   struct generator_object* addi;
                   Shvar* gvar;
                   addi = n_lock(src,temp);
                   if ( derefone(src) != temp ) goto ENTRY;
                   svar = (q)galloc((sizeof(struct generator_susp)/sizeof(q))+1);
                   vcell = (struct generator_susp*)(((q*)svar)+1);
                   vcell->backpt = (q)svar;
                   derefone(svar) = (q)vcell;
                   gvar = (Shvar*)untag_generator_susp(addi);
                   if ( !gvar->chain ) {
                     vcell->u.o = tag_generator_susp(pSHM_VAR);
                     klic_barrier();
                     derefone(src) = svar;
                     *distp = svar;
                     return;
                   }
                   if ( is_cur_or_forward_ptr(gvar) ) {
                     vcell->u.o = tag_generator_susp(gvar);
                     klic_barrier();
                     derefone(src) = svar;
                     *distp = svar;
                     return;
                   } else {
                     Sinfo* dptr;
                     Sinfo* sptr;
                     Shvar* svar = (Shvar*)galloc((sizeof(Shvar)/sizeof(q)));
                     vcell->u.o = tag_generator_susp(svar);
                     svar->method = SHM_VAR;
                     dptr = (Sinfo*)&(svar->chain);
                     sptr = gvar->chain;
                     if ( !is_genhook(sptr) ) {
                       while(sptr && !is_cur_or_forward_ptr(sptr)) {
                         dptr->next = (Sinfo*)galloc((sizeof(Sinfo)/sizeof(q)));
                         dptr = dptr->next;
                         dptr->PE_num = sptr->PE_num;
                         dptr->prio = sptr->prio;
                         dptr->indp = sptr->indp;
                         sptr = sptr->next;
                       }
                       dptr->next = sptr;
                     } else {
                       if ( !is_cur_or_forward_ptr(sptr) ) {
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
                   derefone(src) = svar;
                   *distp = svar;
                   return;
                 }
               } else {  /* local memory */
                 Shvar* hobj;
                 struct susprec* sp = (struct susprec*)temp;
                 makeshvar(temp1);
                 if ( is_generator_susp(sp->u) ) {
                   hobj = create_genhook(tag_local(src),temp1);
                 } else {
                   hobj = shm_add_consumer(temp1,0,src,glbl);
                 }
                 *(((struct generator_object**)temp1)+2) = tag_generator_susp(hobj);
                 klic_barrier();
                 *distp = temp1;
                 return;
               }
           }
           srcp = (q*)src;
           goto ENTRY;
        }
      }
    default:
      {  q f = functor_of(src);
         int arity;

	 if ( is_locala(src) ) {
	   if ( isref(f) ) {
	     struct data_object *obj
	       = (struct data_object *)functorp(src);
	     /* abend("Copied data object into shared memory!!!"); */
	     *distp = generic_shmcopy(obj);
	     return;
	   }
	   arity = arityof(f);
	   cnp = (q)galloc(arity+1);
	   *distp = makefunctor(cnp);
	   ((struct functor*)cnp)->functor = functorp(src)->functor;
	   for(i=0;i<arity;i++) {
	     shm_arg_copy(&functorp(src)->args[i],&((struct functor*)cnp)->args[i]);
	   }
	   return;
	 } else if ( is_shma(src) ) { /* Shared to shrared copy */
           if ( is_cur_or_forward_ptr(src) ) { *distp = src; return; }
           if ( isref(f) ) {
             struct data_object *obj
                = (struct data_object *)functorp(src);
             /* abend("Copied data object into shared memory!!!"); */
             q temp = generic_shmcopy(obj);
	     klic_barrier();
             *distp = temp;
             return;
           }
           arity = arityof(f);
           cnp = (q)galloc(arity+1);
           ((struct functor*)cnp)->functor = functorp(src)->functor;
/* First arg is special for Topdown GC */
           for(i=0;i<arity;i++) {
             shm_arg_copy(&functorp(src)->args[i],&((struct functor*)cnp)->args[i]);
           }
	   klic_barrier();
	   *distp = *srcp = makefunctor(cnp);
           return;
	 } else { /* assume text-area */
	   *distp = src; return;
	 }
       }
    }
}

