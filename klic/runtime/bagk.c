/* Compiled by KLIC compiler version 3.003 (Mon Mar 29 19:42:59 JST 1999) */
#include <klic/klichdr.h>
#include "atom.h"
#include "funct.h"

module module_keyed__bag();
Const struct predicate predicate_keyed__bag_xcreate_3 =
   { module_keyed__bag, 0, 3 };
Const struct predicate predicate_keyed__bag_xcreate_2 =
   { module_keyed__bag, 1, 2 };
Const struct predicate predicate_keyed__bag_xcreate_1 =
   { module_keyed__bag, 2, 1 };
extern Const struct predicate predicate_keyed__sorted__bag_xcreate_1;

module module_keyed__bag(glbl, qp, allocp, toppred)
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
    case_pred(0, create_3_top);
    case_pred(1, create_2_top);
    last_case_pred(2, create_1_top);
  }

 create_3_top: {
  a1 = qp->args[1];
  a2 = qp->args[2];
  qp = qp->next;
 create_3_clear_reason:
  reasonp = reasons;
 create_3_0:
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_keyed__sorted__bag_xcreate_1);
  allocp[2] = a1;
  qp = (struct goalrec*)&allocp[0];
  allocp += 3;
  proceed();
 create_3_ext_interrupt:
  reasonp = 0l;
 create_3_interrupt:
  goto interrupt_3;
 }

 create_2_top: {
  a1 = qp->args[1];
  qp = qp->next;
 create_2_clear_reason:
  reasonp = reasons;
 create_2_0:
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_keyed__sorted__bag_xcreate_1);
  allocp[2] = a0;
  qp = (struct goalrec*)&allocp[0];
  allocp += 3;
  proceed();
 create_2_ext_interrupt:
  reasonp = 0l;
 create_2_interrupt:
  goto interrupt_2;
 }

 create_1_top: {
  qp = qp->next;
 create_1_clear_reason:
  reasonp = reasons;
 create_1_0:
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_keyed__sorted__bag_xcreate_1);
  allocp[2] = a0;
  qp = (struct goalrec*)&allocp[0];
  allocp += 3;
  proceed();
 create_1_ext_interrupt:
  reasonp = 0l;
 create_1_interrupt:
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
  loop_within_module(module_keyed__bag);
}
