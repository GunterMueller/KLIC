/* Compiled by KLIC compiler version 3.003 (Mon Mar 29 19:42:59 JST 1999) */
#include <klic/klichdr.h>
#include "atom.h"
#include "funct.h"

module module_main();
Const struct predicate predicate_main_xmain_0 =
   { module_main, 0, 0 };
Const struct predicate predicate_main_xkl1cmps_2 =
   { module_main, 1, 2 };
extern Const struct predicate predicate_unix_xargv_1;
extern Const struct predicate predicate_unix_xexit_1;
extern Const struct predicate predicate_klic__comp__kl1cmp_xklcmp_2;

module module_main(glbl, qp, allocp, toppred)
  struct global_variables *glbl;
  struct goalrec *qp;
  register q *allocp;
  Const struct predicate *toppred;
{
  q a0, a1;

  q *reasonp;
 module_top:
  switch_on_pred() {
    case_pred(0, main_0_top);
    last_case_pred(1, kl1cmps_2_top);
  }

 main_0_top: {
  q x0;
  qp = qp->next;
 main_0_clear_reason:
  reasonp = reasons;
 main_0_0:
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_main_xkl1cmps_2);
  allocp[2] = x0 = makeref(&allocp[2]);
  allocp[3] = makesym(atom_normal);
  allocp[4] = (q)(struct goalrec*)&allocp[0];
  allocp[5] = (q)(&predicate_unix_xargv_1);
  allocp[6] = x0;
  qp = (struct goalrec*)&allocp[4];
  allocp += 7;
  proceed();
 main_0_ext_interrupt:
  reasonp = 0l;
 main_0_interrupt:
  goto interrupt_0;
 }

 kl1cmps_2_top: {
  q x0, x1, x2, x3, x4;
  a0 = qp->args[0];
  a1 = qp->args[1];
  qp = qp->next;
 kl1cmps_2_clear_reason:
  reasonp = reasons;
 kl1cmps_2_0:
 kl1cmps_2_1:
  switch (ptagof(a0)) {
 case CONS:
 kl1cmps_2_2:
  switch (ptagof(a1)) {
 case ATOMIC:
  if (a1 != makesym(atom_normal)) goto kl1cmps_2_4;
  allocp[1] = (q)(&predicate_main_xkl1cmps_2);
  x0 = cdr_of(a0);
  allocp[2] = x0;
  allocp[3] = x1 = makeref(&allocp[3]);
  x2 = (q)qp;
  x3 = (q)(struct goalrec*)&allocp[0];
  enqueue_at_lower_priority_no_check(makeint(1L),x2,x3);
  allocp[4] = (q)qp;
  allocp[5] = (q)(&predicate_klic__comp__kl1cmp_xklcmp_2);
  x4 = car_of(a0);
  allocp[6] = x4;
  allocp[7] = x1;
  qp = (struct goalrec*)&allocp[4];
  allocp += 8;
  proceed();
 case VARREF:
  deref_and_jump(a1,kl1cmps_2_2);
  *reasonp++ =  a1;
  goto kl1cmps_2_4;
  };
  goto kl1cmps_2_4;
 case ATOMIC:
  if (a0 != NILATOM) goto kl1cmps_2_4;
 kl1cmps_2_3:
  switch (ptagof(a1)) {
 case ATOMIC:
  if (a1 != makesym(atom_normal)) goto kl1cmps_2_4;
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_unix_xexit_1);
  allocp[2] = makeint(0L);
  qp = (struct goalrec*)&allocp[0];
  allocp += 3;
  proceed();
 case VARREF:
  deref_and_jump(a1,kl1cmps_2_3);
  *reasonp++ =  a1;
  goto kl1cmps_2_4;
  };
  goto kl1cmps_2_4;
 case VARREF:
  deref_and_jump(a0,kl1cmps_2_1);
  *reasonp++ =  a0;
  goto kl1cmps_2_4;
  };
  goto kl1cmps_2_4;
 kl1cmps_2_4:
  otherwise(kl1cmps_2_interrupt);
 kl1cmps_2_5:
  if (isref(a1)) goto kl1cmps_2_6;
  gblt_wait(a1,kl1cmps_2_interrupt);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_unix_xexit_1);
  allocp[2] = makeint(-1L);
  qp = (struct goalrec*)&allocp[0];
  allocp += 3;
  proceed();
 kl1cmps_2_6:
  deref_and_jump(a1,kl1cmps_2_5);
  *reasonp++ =  a1;
  goto kl1cmps_2_interrupt;
 kl1cmps_2_ext_interrupt:
  reasonp = 0l;
 kl1cmps_2_interrupt:
  goto interrupt_2;
 }
 interrupt_2:
  allocp[3] = a1;
 interrupt_1:
  allocp[2] = a0;
 interrupt_0:
  allocp = interrupt_goal(allocp, toppred, reasonp);
 proceed_label:
  loop_within_module(module_main);
}
