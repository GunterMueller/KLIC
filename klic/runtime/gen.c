/* Compiled by KLIC compiler version 3.003 (Mon Mar 29 19:42:59 JST 1999) */
#include <klic/klichdr.h>
#include "atom.h"
#include "funct.h"
#include <klic/gobj.h>
#include <klic/g_pointer.h>
#include <klic/g_termarray.h>
typedef q * (*gd_func)();

module module_generic();
Const struct predicate predicate_generic_xnew_3 =
   { module_generic, 0, 3 };
Const struct predicate predicate_generic_xgeneric_2 =
   { module_generic, 1, 2 };

module module_generic(glbl, qp, allocp, toppred)
  struct global_variables *glbl;
  struct goalrec *qp;
  register q *allocp;
  Const struct predicate *toppred;
{
  q a0, a1, a2;

  q *reasonp;
 module_top:
  a0 = qp->args[0];
  a1 = qp->args[1];
  switch_on_pred() {
    case_pred(0, new_3_top);
    last_case_pred(1, generic_2_top);
  }

 new_3_top: {
  q x0;
  a2 = qp->args[2];
  qp = qp->next;
 new_3_clear_reason:
  reasonp = reasons;
 new_3_0:
 new_3_1:
  switch (ptagof(a1)) {
 case FUNCTOR:
 new_3_2:
 new_3_3:
  switch (ptagof(a2)) {
 case FUNCTOR:
 new_3_4:
  
{
  unsigned long argc;
  q *argv;
  gd_func func = (gd_func)(((struct pointer_object *)
                           (data_objectp(a1)))->pointer);
  argc = ((struct termarray_object *)(data_objectp(a2)))->nterm;
  argv = ((struct termarray_object *)(data_objectp(a2)))->terms;
  x0   = (q)func(argc,argv,allocp);
  allocp = heapp;
}
  unify(x0, a0);
  proceed();
 case VARREF:
  deref_and_jump(a2,new_3_3);
  *reasonp++ =  a2;
  goto new_3_interrupt;
  };
  goto new_3_interrupt;
 case VARREF:
  deref_and_jump(a1,new_3_1);
  *reasonp++ =  a1;
  goto new_3_interrupt;
  };
  goto new_3_interrupt;
 new_3_ext_interrupt:
  reasonp = 0l;
 new_3_interrupt:
  goto interrupt_3;
 }

 generic_2_top: {
  qp = qp->next;
 generic_2_clear_reason:
  reasonp = reasons;
 generic_2_0:
 generic_2_1:
  switch (ptagof(a0)) {
 case ATOMIC:
  if (isint(a0)) goto generic_2_interrupt;
 generic_2_2:
 generic_2_3:
  switch (ptagof(a1)) {
 case FUNCTOR:
 generic_2_4:
  if (!isgobj(a1)) goto generic_2_interrupt;
 generic_2_5:
  {
  extern unsigned long enter_functor();
  unsigned long funct = enter_functor(symval(a0), 0);
  allocp = gd_generic(allocp, a1, funct, 0);
}
  proceed();
 case VARREF:
  deref_and_jump(a1,generic_2_3);
  *reasonp++ =  a1;
  goto generic_2_interrupt;
  };
  goto generic_2_interrupt;
 case FUNCTOR:
 generic_2_6:
 generic_2_7:
  switch (ptagof(a1)) {
 case FUNCTOR:
 generic_2_8:
  if (!isgobj(a1)) goto generic_2_interrupt;
 generic_2_9:
  
  allocp = gd_generic(allocp,a1,symval((functorp(a0))->functor),
                     &((functorp(a0))->args[0]));

  proceed();
 case VARREF:
  deref_and_jump(a1,generic_2_7);
  *reasonp++ =  a1;
  goto generic_2_interrupt;
  };
  goto generic_2_interrupt;
 case VARREF:
  deref_and_jump(a0,generic_2_1);
  *reasonp++ =  a0;
  goto generic_2_interrupt;
  };
  goto generic_2_interrupt;
 generic_2_ext_interrupt:
  reasonp = 0l;
 generic_2_interrupt:
  goto interrupt_2;
 }
 interrupt_3:
  allocp[4] = a2;
 interrupt_2:
  allocp[3] = a1;
 interrupt_1:
  allocp[2] = a0;
 interrupt_0:
  allocp = interrupt_goal(allocp, toppred, reasonp);
 proceed_label:
  loop_within_module(module_generic);
}
