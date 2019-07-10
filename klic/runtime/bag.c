/* Compiled by KLIC compiler version 3.003 (Mon Mar 29 19:42:59 JST 1999) */
#include <klic/klichdr.h>
#include "atom.h"
#include "funct.h"

module module_bag();
Const struct predicate predicate_bag_xcreate_1 =
   { module_bag, 0, 1 };
extern Const struct predicate predicate_sorted__bag_xcreate_1;

module module_bag(glbl, qp, allocp, toppred)
  struct global_variables *glbl;
  struct goalrec *qp;
  register q *allocp;
  Const struct predicate *toppred;
{
  q a0;

  q *reasonp;
 module_top:
  a0 = qp->args[0];
  switch_on_pred() {
    last_case_pred(0, create_1_top);
  }

 create_1_top: {
  qp = qp->next;
 create_1_clear_reason:
  reasonp = reasons;
 create_1_0:
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_sorted__bag_xcreate_1);
  allocp[2] = a0;
  qp = (struct goalrec*)&allocp[0];
  allocp += 3;
  proceed();
 create_1_ext_interrupt:
  reasonp = 0l;
 create_1_interrupt:
  goto interrupt_1;
 }
 interrupt_1:
  allocp[2] = a0;
 interrupt_0:
  allocp = interrupt_goal(allocp, toppred, reasonp);
 proceed_label:
  loop_within_module(module_bag);
}
