/* ---------------------------------------------------------- 
%   (C)1994,1995 Institute for New Generation Computer Technology 
%       (Read COPYRIGHT for detailed information.) 
%   (C)1996, 1997, 1998, 1999 Japan Information Processing Development Center
%       (Read COPYRIGHT-JIPDEC for detailed information.)
----------------------------------------------------------- */
#include <stdio.h>

#include <klic/basic.h>
#include <klic/struct.h>
#include <klic/g_methtab.h>
#include <klic/g_basic.h>
#include <klic/gg_macro.h>
#include <klic/susp.h>
#include <klic/gobj.h>
#include <klic/shm.h>

#define GENHOOK
#define GENHOPT

#define GG_CLASS_NAME() shvar
#define GG_OBJ_TYPE struct Shvar

typedef struct Shvar {
  struct generator_object_method_table *method_table;
  Sinfo chain;
} Shm_var;

Sinfo* shm_copy_chain();
Sinfo* shm_merge_chain();

q* do_unify();
#define One_more do_unify(g_allocp,GG_SELF,GG_TERM)

GGDEF_UNIFY()
{
  G_STD_DECL;

  struct generator_object *addi;
  Shvar *gobjp;
  q pair = derefone(GG_SELF);
  if ( !isref(pair) || GG_SELF != derefone(pair) )   return One_more;
  addi = n_lock(GG_SELF,pair);
  if ( pair != derefone(GG_SELF) )  return One_more;

  gobjp = (Shvar*)untag_generator_susp(addi);
  GG_SWITCH_ON_TERM(cons,atomic,func,dobj,susp)
 atomic: {
  derefone(GG_SELF) = GG_TERM;
  if ( gobjp->chain ) {
    if ( is_genhook(gobjp->chain) ) {
      g_allocp = shm_ck_request(g_allocp,gobjp->chain);
    } else {
      g_allocp = shm_resume_goals(g_allocp,gobjp->chain);
    }
  }
  GG_RETURN;
 }
 cons:
 func:
 dobj: {
  q y;
#ifdef GENHOOK
  if ( !gobjp->chain && !is_shma(GG_TERM)
#ifdef GENHOPT
  && last_shm_var == GG_SELF
#endif
                       ) {
    gobjp = create_genhook(GG_TERM,GG_SELF);
    n_unlock(pair,tag_generator_susp(gobjp));
    GG_RETURN;
  } else {
#endif
    ck_new_galloc(GG_SELF);
    y = shm_copy(GG_TERM);
    klic_barrier();
    derefone(GG_SELF) = y;
    if ( gobjp->chain ) {
      if ( is_genhook(gobjp->chain) ) {
        g_allocp = shm_ck_request(g_allocp,gobjp->chain);
      } else {
        g_allocp = shm_resume_goals(g_allocp,gobjp->chain);
      }
    }
#ifdef GENHOOK
  }
#endif
  GG_RETURN;
 }

 susp: {
  q x = GG_SELF;
  q temp = pair;
  struct generator_object* xaddi = addi;
  Shvar* xobjp = gobjp;
  q y = GG_TERM;
  q ytemp;
  struct generator_object* yaddi;
  Shvar* yobjp;

 Re_try:

  ytemp = derefone(y);
  if ( !isref(ytemp) )        { n_unlock(temp,xaddi); return One_more;}
  if ( y != derefone(ytemp) ) { n_unlock(temp,xaddi); return One_more;}
  yaddi = generator_suspp(ytemp)->u.o;
/*  if ( is_generator_susp(yaddi) ) { */
  if ( (long)yaddi & 0x1 ) {
    struct generator_object_method_table* m;
    yobjp = (Shvar*)untag_generator_susp(yaddi);
    m = yobjp->method;
    if (m == SHM_VAR || m == SHM_BUSY) {
      if ( x == y )           { n_unlock(temp,xaddi); GG_RETURN;}
      if ( cmp_forward_ptr(x,y) ) { /* reverse order */
        q w;
        n_unlock(temp,xaddi);
        w = x; x = y; y = w;
        w = temp; temp = ytemp; ytemp = w;
	xaddi = n_lock(x,temp);
        if ( temp != derefone(x) )  { return One_more;}
        xobjp = (Shvar*)untag_generator_susp(xaddi);
      }

      if ( xobjp->chain ) {
        if ( is_genhook(xobjp->chain) ) { /* one is a generator hook. */
	  derefone(x) = y;
	  g_allocp = shm_ck_request(g_allocp,xobjp->chain);
        } else {              /* one is a normal hook */
	  yaddi = n_lock(y,ytemp);
	  if ( derefone(y) != ytemp )  { n_unlock(temp,xaddi); return One_more;}
	  yobjp = (Shvar*)untag_generator_susp(yaddi);
	  if ( yobjp->chain ) {
	    if ( is_genhook(yobjp->chain) ) { /* another is a generator hook. */
	      Sinfo* ychain = yobjp->chain;
	      ck_new_galloc(yobjp);
	      /* Don't forget keeping direction */
	      yobjp->chain = shm_copy_chain(xobjp->chain); 
	      n_unlock(ytemp,yaddi);
	      derefone(x) = y;
	      g_allocp = shm_ck_request(g_allocp,ychain);
	    } else { /* both are ordinary hooks */
	      ck_new_galloc(yobjp);
	      yobjp->chain = shm_merge_chain(xobjp->chain,yobjp->chain);
	      n_unlock(ytemp,yaddi);
	      derefone(x) = y;
	    }
	  } else {
	    ck_new_galloc(y);
	    if ( is_cur_or_forward_ptr(xobjp) ) {
	      n_unlock(ytemp,tag_generator_susp(xobjp));
	      derefone(x) = y;
	    } else {
	      yobjp = (Shvar*)galloc(sizeof(Shvar)/sizeof(q));
	      yobjp->method = SHM_VAR;
	      yobjp->chain = shm_copy_chain(xobjp->chain);
	      n_unlock(ytemp,tag_generator_susp(yobjp));
	      derefone(x) = y;
	    }
	  }
        }
      } else { /* x has no additional info. */
        derefone(x) = y;
      }
    } else { /* y is an other local generator object */
      /* assumes user defined object. Not Exref */
      if ( xobjp->chain ) { /* x has any info */
        struct generator_object *gobjy = untag_generator_susp(yaddi);
        q tmpy = generic_generate(gobjy,g_allocp);
        switch((long)tmpy) {
        case (long)makeref(0):
          GG_KL1_UNIFY(x,y);
          GG_RETURN;
        case (long)makecons(0):
          fatal("system bug!!!");
        default:
          g_allocp = heapp;
          derefone(y) = tmpy;
          return One_more;
        }
      } else { /* x is a simple variable */
        xobjp = create_genhook(tag_local(y),x);
        n_unlock(temp,tag_generator_susp(xobjp));
      }
    }
  } else { /* y is a consumer */
    if ( xobjp && is_genhook(xobjp->chain) ) { /* global generator hook */
      Shvar* tobj = shm_add_consumer(x,0,y,glbl);
      g_allocp = shm_ck_request(g_allocp,xobjp->chain);
      n_unlock(temp,tag_generator_susp(tobj));
    } else {
      Shvar* tobj = (Shvar*)shm_add_consumer(x,xobjp,y,glbl);
      n_unlock(temp,tag_generator_susp(tobj));
    }
  }
  GG_RETURN;
  }
}
/*
static q
generate(self, g_allocp)
struct integer_generator *self;
q *g_allocp;
*/
GGDEF_GENERATE()
{
  return( makecons(0) );
}

GGDEF_SUSPEND()
{
  G_STD_DECL;
  struct generator_object* addi;
  q temp = derefone(GG_SELF);
  if ( !isref(temp) )   return( makecons(0) );
  if ( GG_SELF != derefone(temp) )  return( makecons(0) );
  addi = n_lock(GG_SELF,temp);
  if ( derefone(GG_SELF) != temp )  return( makecons(0) );
  ck_new_galloc(GG_SELF);
  /* creates shared memory hook chain */
  {
    Sinfo* sptr;
    Shvar* gvar = (Shvar*)untag_generator_susp(addi);
    Sinfo* gp = gvar->chain;
    if ( !gp ) { /* First hook */
      Shvar* gptr = (Shvar*)galloc(
         (sizeof(Shvar)+sizeof(Sinfo))/sizeof(q) );
      sptr = (Sinfo*)(gptr+1);
      gptr->method = SHM_VAR;
      gptr->chain = sptr;
      sptr->next = 0;
      sptr->PE_num = my_node;
      sptr->prio = current_prio;
      sptr->indp = create_local_tbl(GG_GOAL,GG_SELF);
      n_unlock(temp,tag_generator_susp(gptr));
      return( makeref(0) );
    }
    /* already any hooks */
    if ( is_genhook(gp) ) { /* shared generator hook */
      Sinfo* sptr = (Sinfo*)untag_generator_susp(gp);
      int pe_num = sptr->PE_num;
      long prio = sptr->prio;
      TADDRtbl* ind = sptr->indp;
      if ( pe_num == my_node ) {
	q temp;
	q* tp = ind->globalA;
	if ( !isatomic(ind->localA) ) {
	  free_local_tbl(ind);
	  shm_arg_copy(&ind->localA,&temp);
	  klic_barrier();
	  *tp = temp;
	  return( makecons(0) );
	}
      }
      /* reuses Sinfo */
      gvar->method = SHM_VAR;
      gvar->chain = sptr;
      sptr->PE_num = my_node;
      sptr->prio = current_prio;
      sptr->indp = create_local_tbl(GG_GOAL,GG_SELF);
      shm_request_queueing(pe_num,prio,ind);
      n_unlock(temp,addi);
      return( makeref(0) );
    } else { /* ordinary hook */
      Sinfo* bgp = (Sinfo*)&(gvar->chain);
      ck_new_galloc(gvar);
      while( gp && !is_forward_space_ptr(gp) ) {
	if ( gp->PE_num == my_node ) {
	  if ( (struct goalrec*)(gp->indp->localA) == GG_GOAL ) {
	    n_unlock(temp,addi);
	    return( makeref(0) );
	  } else {
	    break;
	  }
	}
	bgp = gp;
	gp = gp->next;
      }
      sptr = (Sinfo*)galloc(sizeof(Sinfo)/sizeof(q));
      bgp->next = sptr;
      sptr->next = gp;
      sptr->PE_num = my_node;
      sptr->prio = current_prio;
      sptr->indp = create_local_tbl(GG_GOAL,GG_SELF);
      n_unlock(temp,addi);
      return( makeref(0) );
    }
  }
}

GGDEF_PRINT()
{
  fprintf(g_fp, "<INTEGER GENERATOR %x>", GG_SELF);
  return(0);
}

GGDEF_GC()
{
  fatal("bug!!!");
}

#define GGUSE_MY_UNIFY
#define GGUSE_MY_SUSPEND
#define GGUSE_MY_GENERATE
#define GGUSE_MY_GC
#define GGUSE_MY_PRINT

#include <klic/gg_methtab.h>

GGDEF_NEW()
{
}
