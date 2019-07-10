/* Compiled by KLIC compiler version 3.003 (Mon Mar 29 19:42:59 JST 1999) */
#include <klic/klichdr.h>
#include "atom.h"
#include "funct.h"

module module_comparator();
Const struct predicate predicate_comparator_xsort_5 =
   { module_comparator, 0, 5 };

module module_comparator(glbl, qp, allocp, toppred)
  struct global_variables *glbl;
  struct goalrec *qp;
  register q *allocp;
  Const struct predicate *toppred;
{
  q a0, a1, a2, a3, a4;

  q *reasonp;
 module_top:
  a0 = qp->args[0];
  a1 = qp->args[1];
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  switch_on_pred() {
    last_case_pred(0, sort_5_top);
  }

 sort_5_top: {
  q x0, x1, x2, x3;
  qp = qp->next;
 sort_5_clear_reason:
  reasonp = reasons;
 sort_5_0:
 sort_5_1:
  if (isref(a0)) goto sort_5_5;
 sort_5_2:
  if (isref(a1)) goto sort_5_4;
  gblt_compare(a0,a1,x0,sort_5_interrupt);
  x1 = makeint(0L);
  gblt_less_or_eq(x0,x1,sort_5_3);
  x2 = makesym(atom_no);
  unify_value(a4, x2);
  unify_value(a2, a0);
  unify_value(a3, a1);
  proceed();
 sort_5_3:
  x2 = makeint(0L);
  gblt_greater(x0,x2,sort_5_interrupt);
  x3 = makesym(atom_yes);
  unify_value(a4, x3);
  unify_value(a2, a1);
  unify_value(a3, a0);
  proceed();
 sort_5_4:
  deref_and_jump(a1,sort_5_2);
  *reasonp++ =  a1;
  goto sort_5_interrupt;
 sort_5_5:
  deref_and_jump(a0,sort_5_1);
  *reasonp++ =  a0;
  goto sort_5_interrupt;
 sort_5_ext_interrupt:
  reasonp = 0l;
 sort_5_interrupt:
  goto interrupt_5;
 }
 interrupt_5:
  allocp[6] = a4;
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
  loop_within_module(module_comparator);
}
