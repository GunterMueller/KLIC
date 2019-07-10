/* Compiled by KLIC compiler version 3.003 (Mon Mar 29 19:42:59 JST 1999) */
#include <klic/klichdr.h>
#include "atom.h"
#include "funct.h"
#include <klic/gobj.h>

module module_unify__term__dcode();
Const struct predicate predicate_unify__term__dcode_xunify_2 =
   { module_unify__term__dcode, 0, 2 };
Const struct predicate predicate_unify__term__dcode_xunify__pf_2 =
   { module_unify__term__dcode, 1, 2 };
Const struct predicate predicate_unify__term__dcode_xunify__args_3 =
   { module_unify__term__dcode, 2, 3 };
Const struct predicate predicate_unify__term__dcode_xunify__goal_2 =
   { module_unify__term__dcode, 3, 2 };

module module_unify__term__dcode(glbl, qp, allocp, toppred)
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
    case_pred(0, unify_2_top);
    case_pred(1, unify__pf_2_top);
    case_pred(2, unify__args_3_top);
    last_case_pred(3, unify__goal_2_top);
  }

 unify_2_top: {
  q x0, x1, x2, x3;
  qp = qp->next;
 unify_2_clear_reason:
  reasonp = reasons;
 unify_2_0:
 unify_2_1:
  if (isref(a0)) goto unify_2_4;
  gblt_pfunctor(a0,x0,unify_2_interrupt);
  gblt_arity(a0,x1,unify_2_interrupt);
 unify_2_2:
  if (isref(a1)) goto unify_2_3;
  gblt_pfunctor(a1,x2,unify_2_interrupt);
  gblt_arity(a1,x3,unify_2_interrupt);
  gblt_eq(x1,x3,unify_2_interrupt);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_unify__term__dcode_xunify__args_3);
  allocp[2] = x1;
  allocp[3] = a0;
  allocp[4] = a1;
  a0 = x0;
  a1 = x2;
  qp = (struct goalrec*)&allocp[0];
  allocp += 5;
  execute(unify__pf_2_0);
  goto unify__pf_2_ext_interrupt;
 unify_2_3:
  deref_and_jump(a1,unify_2_2);
  *reasonp++ =  a1;
  goto unify_2_interrupt;
 unify_2_4:
  deref_and_jump(a0,unify_2_1);
  *reasonp++ =  a0;
  goto unify_2_interrupt;
 unify_2_ext_interrupt:
  reasonp = 0l;
 unify_2_interrupt:
  goto interrupt_2;
 }

 unify__pf_2_top: {
  qp = qp->next;
 unify__pf_2_clear_reason:
  reasonp = reasons;
 unify__pf_2_0:
 unify__pf_2_1:
  if (isref(a0)) goto unify__pf_2_4;
 unify__pf_2_2:
  if (isref(a1)) goto unify__pf_2_3;
  
         if (isatomic(a0)) {
           if (a0 != a1) goto unify__pf_2_interrupt;
         } else if (isatomic(a1)) {
	   goto unify__pf_2_interrupt;
         } else {
            generic_active_unify(data_objectp(a0),
			         data_objectp(a1),allocp);
         }
  proceed();
 unify__pf_2_3:
  deref_and_jump(a1,unify__pf_2_2);
  *reasonp++ =  a1;
  goto unify__pf_2_interrupt;
 unify__pf_2_4:
  deref_and_jump(a0,unify__pf_2_1);
  *reasonp++ =  a0;
  goto unify__pf_2_interrupt;
 unify__pf_2_ext_interrupt:
  reasonp = 0l;
 unify__pf_2_interrupt:
  toppred = &predicate_unify__term__dcode_xunify__pf_2;
  goto interrupt_2;
 }

 unify__args_3_top: {
  q x0, x1, x2, x3, x4;
  a2 = qp->args[2];
  qp = qp->next;
 unify__args_3_clear_reason:
  reasonp = reasons;
 unify__args_3_0:
 unify__args_3_1:
  if (!isint(a0)) goto unify__args_3_7;
  x0 = makeint(0L);
  gblt_greater(a0,x0,unify__args_3_6);
 unify__args_3_2:
  if (isref(a1)) goto unify__args_3_5;
  gblt_arg(a0,a1,x1,unify__args_3_6);
 unify__args_3_3:
  if (isref(a2)) goto unify__args_3_4;
  gblt_arg(a0,a2,x2,unify__args_3_6);
  unify(x1, x2);
  x3 = makeint(1L);
  bblt_sub_no_check(a0,x3,x4);
  a0 = x4;
  execute(unify__args_3_0);
  goto unify__args_3_ext_interrupt;
 unify__args_3_4:
  deref_and_jump(a2,unify__args_3_3);
  *reasonp++ =  a2;
  goto unify__args_3_6;
 unify__args_3_5:
  deref_and_jump(a1,unify__args_3_2);
  *reasonp++ =  a1;
  goto unify__args_3_6;
 unify__args_3_6:
  if (a0 != makeint(0L)) goto unify__args_3_interrupt;
  proceed();
 unify__args_3_7:
  if (!isref(a0)) goto unify__args_3_interrupt;
  deref_and_jump(a0,unify__args_3_1);
  *reasonp++ =  a0;
  goto unify__args_3_interrupt;
 unify__args_3_ext_interrupt:
  reasonp = 0l;
 unify__args_3_interrupt:
  goto interrupt_3;
 }

 unify__goal_2_top: {
  qp = qp->next;
 unify__goal_2_clear_reason:
  reasonp = reasons;
 unify__goal_2_0:
  unify(a0, a1);
  proceed();
 unify__goal_2_ext_interrupt:
  reasonp = 0l;
 unify__goal_2_interrupt:
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
  loop_within_module(module_unify__term__dcode);
}
