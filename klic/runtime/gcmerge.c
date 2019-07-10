/* Compiled by KLIC compiler version 3.003 (Mon Mar 29 19:42:59 JST 1999) */
#include <klic/klichdr.h>
#include "atom.h"
#include "funct.h"

module module_gcmerge();
Const struct predicate predicate_gcmerge_xin_4 =
   { module_gcmerge, 0, 4 };

module module_gcmerge(glbl, qp, allocp, toppred)
  struct global_variables *glbl;
  struct goalrec *qp;
  register q *allocp;
  Const struct predicate *toppred;
{
  q a0, a1, a2, a3;

  q *reasonp;
 module_top:
  a0 = qp->args[0];
  a1 = qp->args[1];
  a2 = qp->args[2];
  a3 = qp->args[3];
  switch_on_pred() {
    last_case_pred(0, in_4_top);
  }

 in_4_top: {
  q x0, x1, x2, x3, x4;
  qp = qp->next;
 in_4_clear_reason:
  reasonp = reasons;
 in_4_0:
 in_4_1:
  if (!isint(a2)) goto in_4_5;
  x1 = makeint(1L);
  gblt_add(a2,x1,x0,in_4_interrupt);
 in_4_2:
  if (!isint(a3)) goto in_4_4;
  gblt_less(x0,a3,in_4_3);
  generic_arg[0] = a2;
  allocp[0] = x2 = makeref(&allocp[0]);
  generic_arg[1] = x2;
  call_generic(a1, functor_element_2, 1);
  allocp[0] = x3 = makeref(&allocp[0]);
  allocp[1] = x2;
  allocp[2] = x3;
  x4 = create_vector(&allocp[1], 2, allocp);
  allocp += 3;
  unify_value(a0, x4);
  a0 = x3;
  a2 = x0;
  execute(in_4_0);
  goto in_4_ext_interrupt;
 in_4_3:
  gblt_eq(x0,a3,in_4_interrupt);
  generic_arg[0] = a2;
  generic_arg[1] = a0;
  call_generic(a1, functor_element_2, 0);
  proceed();
 in_4_4:
  if (!isref(a3)) goto in_4_interrupt;
  deref_and_jump(a3,in_4_2);
  *reasonp++ =  a3;
  goto in_4_interrupt;
 in_4_5:
  if (!isref(a2)) goto in_4_interrupt;
  deref_and_jump(a2,in_4_1);
  *reasonp++ =  a2;
  goto in_4_interrupt;
 in_4_ext_interrupt:
  reasonp = 0l;
 in_4_interrupt:
  goto interrupt_4;
 }
 interrupt_4:
  allocp[5] = a3;
 interrupt_3:
  allocp[4] = a2;
 interrupt_2:
  allocp[3] = a1;
 interrupt_1:
  allocp[2] = a0;
 interrupt_0:
  allocp = interrupt_goal(allocp, toppred, reasonp);
 proceed_label:
  loop_within_module(module_gcmerge);
}
