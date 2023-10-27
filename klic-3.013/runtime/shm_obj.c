/* ---------------------------------------------------------- 
%   (C)1994,1995 Institute for New Generation Computer Technology 
%       (Read COPYRIGHT for detailed information.) 
%   (C)1996, 1997, 1998, 1999 Japan Information Processing Development Center
%       (Read COPYRIGHT-JIPDEC for detailed information.)
----------------------------------------------------------- */

#include <klic/basic.h>
#include <klic/struct.h>
#include <klic/primitives.h>
#include <klic/alloc.h>
#include <klic/functorstuffs.h>
#include <klic/index.h>
#include <klic/unify.h>
#include <klic/susp.h>
#include <klic/gg_macro.h>
#include <klic/shm.h>

/* unbound variable in shared memory (representating by a generic object)
        +-------------------+
        V                   |
   +---------+         +----+----+
   |  var ---+-------->|    *    |
   +---------+         +---------+
                     Additional Info.
                       +---------+

*/

/* definition of an initail generator object on text area */

extern struct generator_object_method_table GG_method_table0(shvar);
extern struct generator_object_method_table GG_method_table0(shbusy);
struct generator_object_method_table* SHM_VAR = 
  &GG_method_table0(shvar);
struct generator_object_method_table* SHM_BUSY =
  &GG_method_table0(shbusy);
Shvar xSHM_VAR = {&GG_method_table0(shvar),0};
struct generator_object* pSHM_VAR = (struct generator_object*)&xSHM_VAR;
Shvar xSHM_BUSY = {&GG_method_table0(shbusy),0};
struct generator_object* qSHM_BUSY = tag_generator_susp(&xSHM_BUSY);


/* address drive table */
#define ADDRBULK 2048
TADDRtbl ADDRtbl = { &ADDRtbl, &ADDRtbl, 0, 0 };
TADDRtbl *ADDRtbl_free = 0;

#define new_addr_rec(lrec)			\
{						\
  TADDRtbl *temp;				\
  while ((temp = ADDRtbl_free) == 0) {		\
    ADDRtbl_free = more_addr_rec();		\
  }						\
  ADDRtbl_free = temp->next;			\
  (lrec) = temp;				\
}

/* Resume a goal with the same priority as current */

#define resume_same_prio(gp) {        \
  declare_globals;                    \
  if (resumed_goals == 0) {           \
    resumed_goals = gp;               \
    gp->next = gp;                    \
  } else {                            \
    gp->next = resumed_goals->next;   \
    resumed_goals->next = gp;         \
  }                                   \
  heaplimit = 0;                      \
}


module shm_resume();
Const struct predicate shm_xresume_1 = { shm_resume, 0, 1 };
module shm_request();
Const struct predicate shm_xrequest_1 = { shm_request, 0, 1 };


TADDRtbl *more_addr_rec()
{
  int k;
  extern void *calloc();
  TADDRtbl *bulk = (TADDRtbl*)
      calloc(ADDRBULK, sizeof(TADDRtbl));
  for (k = ADDRBULK-1; k > 0; k--) {
    bulk[k-1].next = &bulk[k];
  }
  bulk[ADDRBULK-1].next = 0;
  return bulk;
}

/* d_shm_init
  initializes free chain of external trigger */
d_shm_init()
{
     TADDRtbl* bptr;
     new_addr_rec(bptr);
}



Sinfo* shm_copy_chain(xp)
Sinfo* xp;
{
    declare_globals;
    Sinfo* ret;
    Sinfo** retp = &ret;
    while ( xp && !is_cur_or_forward_ptr(xp) ) {
      Sinfo* sp = (Sinfo*)galloc(sizeof(Sinfo)/sizeof(q));
      *sp = *xp;
      *retp = sp;
      retp = (Sinfo**)sp;
      xp = xp->next;
    }
    *retp = xp;
    return(ret);
}


Sinfo* shm_merge_chain(xp,yp)
Sinfo* xp;
Sinfo* yp;
{
    declare_globals;
    Sinfo* ret;
    Sinfo** retp = &ret;
    while ( xp ) {
      if ( !is_cur_or_forward_ptr(xp) ) { /* xp is old */
        Sinfo* sp = (Sinfo*)galloc(sizeof(Sinfo)/sizeof(q));
        *sp = *xp;
        *retp = sp;
        retp = (Sinfo**)sp;
      } else if ( !is_cur_space_ptr(xp) ) {      /* xp is new */
        while (yp) {
          if ( !is_cur_or_forward_ptr(yp) ) { /* yp is old */
            Sinfo* sp = (Sinfo*)galloc(sizeof(Sinfo)/sizeof(q));
            *sp = *yp;
            *retp = sp;
            retp = (Sinfo**)sp;
          } else if ( !is_cur_space_ptr(yp) ) { /* yp is new */
            while (yp) {
	      *retp = yp;
              retp = (Sinfo**)yp;
              yp = yp->next;
            }
            *retp = xp;
	    return(ret);
          } else {          /* yp is cur */
            *retp = yp;
            retp = (Sinfo**)yp;
          }
          yp = yp->next;
        }
        *retp = yp;
      } else {                             /* xp is cur */
	*retp = xp;
	retp = (Sinfo**)xp;
      }
      xp = xp->next;
    }
    /* xp is no more new space */
    while (yp) {
      if ( !is_cur_or_forward_ptr(yp) ) { /* yp is old */
	Sinfo* sp = (Sinfo*)galloc(sizeof(Sinfo)/sizeof(q));
	*sp = *yp;
	*retp = sp;
	retp = (Sinfo**)sp;
      } else break;
      yp = yp->next;
    }
    *retp = yp;
    return(ret);
}


/* shm_add_consumer(shrec,consumer_ref)
  adds a shared memory variable
*/
Shvar* shm_add_consumer(shv,shobj,cvar,glbl)
q shv;
Shvar* shobj;
q cvar;
struct global_variables* glbl;
{
	q mid;
	Sinfo* sptr;
	ck_new_galloc(shv);
	if ( shobj && shobj->chain ) {
	  Sinfo* shrec = shobj->chain;
	  Sinfo* bp = (Sinfo*)&(shobj->chain);
	  ck_new_galloc(shrec);
	  while ( !is_cur_or_forward_ptr(shrec) ) {
	    sptr = (Sinfo*)galloc(sizeof(Sinfo)/sizeof(q));
	    *sptr = *shrec;
	    bp->next = sptr;
	    bp = sptr;
	    shrec = sptr->next;
	  }
	  sptr = (Sinfo*)galloc(sizeof(Sinfo)/sizeof(q));
	  bp->next = sptr;
	  sptr->next = shrec;
	  sptr->PE_num = my_node;
	  sptr->prio = current_prio;
	  mid = derefone(cvar);
	  derefone(cvar) = shv;
	  derefone(mid) = 0;
	  sptr->indp = create_local_tbl(tag_local(mid),shv);
	  return(shobj);
	} else {
	  /* add first */
	  Shvar *vr = (Shvar*)galloc((sizeof(Shvar)+sizeof(Sinfo))/sizeof(q));
	  sptr = (Sinfo*)(vr+1);
	  vr->method = SHM_VAR;
	  vr->chain = sptr;
	  sptr->next = 0;
	  sptr->PE_num = my_node;
	  sptr->prio = current_prio;
	  mid = derefone(cvar);
	  derefone(cvar) = shv;
	  derefone(mid) = 0;
	  sptr->indp = create_local_tbl(tag_local(mid),shv);
	  return(vr);
	}
}


Shvar* create_genhook(la,ga)
q* la;
q* ga;
{
     declare_globals;
     Shvar* varp;
     Sinfo* sptr;
     ck_new_galloc(ga);
     varp = (Shvar*)galloc((sizeof(Shvar)+sizeof(Sinfo))/sizeof(q));
     sptr = (Sinfo*)(varp+1);
     varp->method = SHM_VAR;
     varp->chain = tag_genhook(sptr);
     sptr->PE_num = my_node;
     sptr->next = 0;
     sptr->prio = HIGHESTPRIO-1;
     sptr->indp = create_local_tbl(la,ga);
     return( varp );
}

/* resume waiting process
*/
q* shm_resume_goals(allocp,hook)
q* allocp;
Sinfo *hook;
{
   declare_globals;

   while(hook) {
      if ( hook->PE_num == my_node ) {
         TADDRtbl *sptr = hook->indp;
         q term = (q)sptr->localA;
	 q shm_term = (q)sptr->globalA;

         free_local_tbl(sptr);

         if ( isatomic(term) ) {
           q wp = untag_local(term);
           q top = (q)&sptr->localA;
           derefone(top) = wp;
           derefone(wp) = top;
	   allocp = do_unify(allocp,top,shm_term);
	 } else {
	   struct goalrec* goal = (struct goalrec*)term;
	   if ( goal && isatomic(goal->next) ) {
	     long prio = intval(goal->next);
	     if ( current_prio != prio ) {
	       (void) enqueue_goal(0,prio,goal,glbl);
	     } else {
	       resume_same_prio(goal);
	     }
	     resumes++;
	   }
	 }

      } else {
         resume_queueing(hook->PE_num,hook->indp,hook->prio);
      }
      hook = hook->next;
   }
   return(allocp);
}

/* queueing other PE's resume routine
*/
resume_queueing(Pe_num,suspg,prio)
int Pe_num;
TADDRtbl *suspg;
long prio;
{
   struct ex_goalrec* tp;
   struct goalrec *goal;

   tp = (struct ex_goalrec*)galloc(5);
   tp->prio = prio;
   goal = &tp->goal;
   goal->next = 0;
   goal->pred = &shm_xresume_1;
   goal->args[0] = (q)((unsigned long)suspg+ATOMIC);

   shm_goal_stack(tp,Pe_num);
}


q* shm_ck_request(allocp,chain)
q* allocp;
Sinfo* chain;
{
  declare_globals;
  Sinfo* sptr = untag_genhook(chain);
  int penum = sptr->PE_num;
  TADDRtbl* indp = sptr->indp;
  if ( penum == my_node ) {
    q temp;
    free_local_tbl(indp);
    shm_arg_copy(&indp->localA,&temp);
    allocp = do_unify(allocp,indp->globalA,temp);
  } else {
    shm_request_queueing(penum,current_prio,indp);
  }
  return(allocp);
}
  

shm_request_queueing(peno,prio,indp)
int peno;
long prio;
TADDRtbl* indp;
{
   struct ex_goalrec* tp;
   struct goalrec *goal;

   tp = (struct ex_goalrec*)galloc(5);
   tp->prio = prio;
   goal = &tp->goal;
   goal->next = 0;
   goal->pred = &shm_xrequest_1;
   goal->args[0] = (q)((unsigned long)(indp)+ATOMIC);

   shm_goal_stack(tp,peno);
}


TADDRtbl* create_local_tbl(lcl,gbl)
q* lcl;
q* gbl;
{
    TADDRtbl* sl;
    new_addr_rec(sl);

    sl->next = ADDRtbl.next;
    ADDRtbl.next = sl;
    (sl->next)->prev = sl;
    sl->prev = &ADDRtbl;
    sl->localA = lcl;
    sl->globalA = gbl;

    return(sl);
}

module shm_resume(glbl,qp,allocp,toppred)
struct global_variables *glbl;
struct goalrec *qp;
q *allocp;
Const struct predicate *toppred;
{
    TADDRtbl *tp = (TADDRtbl*)((unsigned long)(qp->args[0])-ATOMIC);
    free_local_tbl(tp);
    if ( isatomic(tp->localA)) {  /* consumer */
      q x = (q)untag_local(tp->localA);
      q top = (q)&(tp->localA);
      derefone(top) = x;
      derefone(x) = top;
      qp = qp->next;
      allocp = do_unify(allocp,top,tp->globalA);
    } else {
      struct goalrec* gp = (struct goalrec*)(tp->localA);
      if ( gp && isatomic(gp->next) ) {
	gp->next = qp->next;
	qp = gp;
	{ resumes++; }
      } else {
	qp = qp->next;
      }
    }
    heapp = allocp;
    current_queue = qp;
    return((module)(qp->pred)->func);
}


module shm_request(glbl,qp,allocp,toppred)
struct global_variables *glbl;
struct goalrec *qp;
q *allocp;
Const struct predicate *toppred;
{
    TADDRtbl *tp = (TADDRtbl*)((unsigned long)(qp->args[0])-ATOMIC);
    q var = (q)tp->globalA;
    free_local_tbl(tp);
    qp = qp->next;

    if ( isatomic(tp->localA) ) { /* local generator object */
      q x = untag_local(tp->localA);
      allocp = do_unify(allocp,x,var);
    } else { /* shared-memory generator hook */
      allocp = do_unify(allocp,tp->localA,var);
    }
    heapp = allocp;
    current_queue = qp;
    return((module)(qp->pred)->func);
}
