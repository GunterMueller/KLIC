/* Compiled by KLIC compiler version 3.004 (Fri Jan  31 15:00:00 JST 2003) */
#include <klic/klichdr.h>
#include "atom.h"
#include "funct.h"

module module_klic__comp__version();
Const struct predicate predicate_klic__comp__version_xcompiler__version_1 =
   { module_klic__comp__version, 0, 1 };
Const struct predicate predicate_klic__comp__version_xcompiler__date_1 =
   { module_klic__comp__version, 1, 1 };

module module_klic__comp__version(glbl, qp, allocp, toppred)
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
    case_pred(0, compiler__version_1_top);
    last_case_pred(1, compiler__date_1_top);
  }

 compiler__version_1_top: {
  q x0;
  qp = qp->next;
 compiler__version_1_clear_reason:
  reasonp = reasons;
 compiler__version_1_0:
  x0 = makesym(atom_version_203_2E005);
  unify_value(a0, x0);
  proceed();
 compiler__version_1_ext_interrupt:
  reasonp = 0l;
 compiler__version_1_interrupt:
  goto interrupt_1;
 }

 compiler__date_1_top: {
  q x0;
  qp = qp->next;
 compiler__date_1_clear_reason:
  reasonp = reasons;
 compiler__date_1_0:
  x0 = makesym(atom_Thu_20Feb_20_2013_2017_3A11_3A01_20JST_202003);
  unify_value(a0, x0);
  proceed();
 compiler__date_1_ext_interrupt:
  reasonp = 0l;
 compiler__date_1_interrupt:
  goto interrupt_1;
 }
 interrupt_1:
  allocp[2] = a0;
 interrupt_0:
  allocp = interrupt_goal(allocp, toppred, reasonp);
 proceed_label:
  loop_within_module(module_klic__comp__version);
}
