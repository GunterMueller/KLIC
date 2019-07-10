/* Compiled by KLIC compiler version 3.003 (Mon Mar 29 19:42:59 JST 1999) */
#include <klic/klichdr.h>
#include "atom.h"
#include "funct.h"

#include <klic/gd_macro.h>

GD_USE_CLASS(predicate);

static q* gc_postmortem_args(allocp, ntop, otop, nsize, osize)
     q *allocp, *ntop, *otop;
     unsigned int nsize, osize;
{
  declare_globals;
  extern q *copy_one_term();
  if (postmortem_args != 0) {
    return copy_one_term(&postmortem_args, allocp,
			 ntop, otop, nsize, osize);
  } else {
    return allocp;
  }
}


module module_system__control();
Const struct predicate predicate_system__control_xpostmortem_3 =
   { module_system__control, 0, 3 };
Const struct predicate predicate_system__control_xregister__postmortem_3 =
   { module_system__control, 1, 3 };
Const struct predicate predicate_system__control_xgc_2 =
   { module_system__control, 2, 2 };
Const struct predicate predicate_system__control_xafter__gc_1 =
   { module_system__control, 3, 1 };
extern q module_g_new();
extern q predicate_g_new();
declare_method_table_of(predicate);

module module_system__control(glbl, qp, allocp, toppred)
  struct global_variables *glbl;
  struct goalrec *qp;
  register q *allocp;
  Const struct predicate *toppred;
{
  q a0, a1, a2;

  q *reasonp;
 module_top:
  a0 = qp->args[0];
  switch_on_pred() {
    case_pred(0, postmortem_3_top);
    case_pred(1, register__postmortem_3_top);
    case_pred(2, gc_2_top);
    last_case_pred(3, after__gc_1_top);
  }

 postmortem_3_top: {
  q x0, x1, x2, x3;
  a1 = qp->args[1];
  a2 = qp->args[2];
  qp = qp->next;
 postmortem_3_clear_reason:
  reasonp = reasons;
 postmortem_3_0:
 postmortem_3_1:
  if (isref(a1)) goto postmortem_3_2;
  gblt_pfunctor(a1,x0,postmortem_3_interrupt);
  gblt_arity(a1,x1,postmortem_3_interrupt);
  generic_arg[0] = a0;
  new_generic(module_g_new, 1, x2, 0);
  generic_arg[0] = x2;
  generic_arg[1] = x0;
  generic_arg[2] = x1;
  new_generic(predicate_g_new, 3, x3, 0);
  a0 = x3;
  execute(register__postmortem_3_0);
  goto register__postmortem_3_ext_interrupt;
 postmortem_3_2:
  deref_and_jump(a1,postmortem_3_1);
  *reasonp++ =  a1;
  goto postmortem_3_interrupt;
 postmortem_3_ext_interrupt:
  reasonp = 0l;
 postmortem_3_interrupt:
  goto interrupt_3;
 }

 register__postmortem_3_top: {
  q x0;
  a1 = qp->args[1];
  a2 = qp->args[2];
  qp = qp->next;
 register__postmortem_3_clear_reason:
  reasonp = reasons;
 register__postmortem_3_0:
 register__postmortem_3_1:
  switch (ptagof(a0)) {
 case FUNCTOR:
 register__postmortem_3_2:
  if (!isgobj(a0)) goto register__postmortem_3_interrupt;
  if (!isclass(a0,predicate)) goto register__postmortem_3_interrupt;
  
{
  postmortem_pred = ((struct predicate_object *)functorp(a0))->pdesc;
  if (postmortem_args ==0) register_gc_hook(gc_postmortem_args);
  postmortem_args = a1;
}
  x0 = NILATOM;
  unify_value(a2, x0);
  proceed();
 case VARREF:
  deref_and_jump(a0,register__postmortem_3_1);
  *reasonp++ =  a0;
  goto register__postmortem_3_interrupt;
  };
  goto register__postmortem_3_interrupt;
 register__postmortem_3_ext_interrupt:
  reasonp = 0l;
 register__postmortem_3_interrupt:
  toppred = &predicate_system__control_xregister__postmortem_3;
  goto interrupt_3;
 }

 gc_2_top: {
  q x0;
  a1 = qp->args[1];
  qp = qp->next;
 gc_2_clear_reason:
  reasonp = reasons;
 gc_2_0:
  
{
  x0 = makeint(allocp-new_space_top);
  allocp = real_heaplimit;
  heaplimit = 0;
}
  unify_value(a0, x0);
  a0 = a1;
  execute(after__gc_1_clear_reason);
  goto after__gc_1_ext_interrupt;
 gc_2_ext_interrupt:
  reasonp = 0l;
 gc_2_interrupt:
  goto interrupt_2;
 }

 after__gc_1_top: {
  q x0;
  qp = qp->next;
 after__gc_1_clear_reason:
  reasonp = reasons;
 after__gc_1_0:
  
{
  x0 = makeint(allocp-new_space_top);
}
  unify_value(a0, x0);
  proceed();
 after__gc_1_ext_interrupt:
  reasonp = 0l;
 after__gc_1_interrupt:
  toppred = &predicate_system__control_xafter__gc_1;
  goto interrupt_1;
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
  loop_within_module(module_system__control);
}
