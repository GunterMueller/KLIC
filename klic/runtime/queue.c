/* Compiled by KLIC compiler version 3.003 (Mon Mar 29 19:42:59 JST 1999) */
#include <klic/klichdr.h>
#include "atom.h"
#include "funct.h"

module module_queue();
Const struct predicate predicate_queue_xcreate_1 =
   { module_queue, 0, 1 };
Const struct predicate predicate_queue_xmain_5 =
   { module_queue, 1, 5 };
Const struct predicate predicate_queue_xmessage_7 =
   { module_queue, 2, 7 };
Const struct predicate predicate_queue_xdlist_3 =
   { module_queue, 3, 3 };
Const struct predicate predicate_queue_xget__all_4 =
   { module_queue, 4, 4 };

module module_queue(glbl, qp, allocp, toppred)
  struct global_variables *glbl;
  struct goalrec *qp;
  register q *allocp;
  Const struct predicate *toppred;
{
  declare_method_table_of(vector);
  static Const vector_structure_type vector_const_0 =
      declare_vector_constant(0,0);
  q a0, a1, a2, a3, a4, a5, a6;

  q *reasonp;
 module_top:
  a0 = qp->args[0];
  switch_on_pred() {
    case_pred(0, create_1_top);
    case_pred(1, main_5_top);
    case_pred(2, message_7_top);
    case_pred(3, dlist_3_top);
    last_case_pred(4, get__all_4_top);
  }

 create_1_top: {
  q x0;
  qp = qp->next;
 create_1_clear_reason:
  reasonp = reasons;
 create_1_0:
  allocp[0] = x0 = makeref(&allocp[0]);
  a1 = NILATOM;
  a2 = makeint(0L);
  a3 = x0;
  a4 = x0;
  allocp += 1;
  execute(main_5_0);
  goto main_5_ext_interrupt;
 create_1_ext_interrupt:
  reasonp = 0l;
 create_1_interrupt:
  goto interrupt_1;
 }

 main_5_top: {
  q x0, x1, x2, x3;
  a1 = qp->args[1];
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  qp = qp->next;
 main_5_clear_reason:
  reasonp = reasons;
 main_5_0:
 main_5_1:
  switch (ptagof(a0)) {
 case CONS:
  x0 = car_of(a0);
 main_5_2:
  switch (ptagof(x0)) {
 case FUNCTOR:
  if (functor_of(x0) != makesym(functor_do_1)) goto main_5_4;
  allocp[0] = a1;
  x2 = cdr_of(a0);
  allocp[1] = x2;
  x1 = makecons(&allocp[0]);
  a0 = arg(x0, 0);
  a1 = x1;
  allocp += 2;
  execute(main_5_0);
  goto main_5_ext_interrupt;
 case VARREF:
  deref_and_jump(x0,main_5_2);
  *reasonp++ =  x0;
  goto main_5_4;
  };
  goto main_5_4;
 case ATOMIC:
  if (a0 != NILATOM) goto main_5_4;
 main_5_3:
  switch (ptagof(a1)) {
 case CONS:
  a0 = car_of(a1);
  a1 = cdr_of(a1);
  execute(main_5_0);
  goto main_5_ext_interrupt;
 case ATOMIC:
  if (a1 != NILATOM) goto main_5_4;
  x0 = NILATOM;
  unify_value(a4, x0);
  proceed();
 case VARREF:
  deref_and_jump(a1,main_5_3);
  *reasonp++ =  a1;
  goto main_5_4;
  };
  goto main_5_4;
 case VARREF:
  deref_and_jump(a0,main_5_1);
  *reasonp++ =  a0;
  goto main_5_4;
  };
  goto main_5_4;
 main_5_4:
  otherwise(main_5_interrupt);
 main_5_5:
  switch (ptagof(a0)) {
 case CONS:
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_queue_xmain_5);
  x0 = cdr_of(a0);
  allocp[2] = x0;
  allocp[3] = a1;
  allocp[4] = x1 = makeref(&allocp[4]);
  allocp[5] = x2 = makeref(&allocp[5]);
  allocp[6] = x3 = makeref(&allocp[6]);
  a0 = car_of(a0);
  a1 = a2;
  a2 = x1;
  a5 = a4;
  a6 = x3;
  a4 = x2;
  qp = (struct goalrec*)&allocp[0];
  allocp += 7;
  execute(message_7_0);
  goto message_7_ext_interrupt;
 case VARREF:
  deref_and_jump(a0,main_5_5);
  *reasonp++ =  a0;
  goto main_5_interrupt;
  };
  goto main_5_interrupt;
 main_5_ext_interrupt:
  reasonp = 0l;
 main_5_interrupt:
  toppred = &predicate_queue_xmain_5;
  goto interrupt_5;
 }

 message_7_top: {
  q x0, x1, x2, x3, x4, x5;
  a1 = qp->args[1];
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  a6 = qp->args[6];
  qp = qp->next;
 message_7_clear_reason:
  reasonp = reasons;
 message_7_0:
 message_7_1:
  switch (ptagof(a0)) {
 case FUNCTOR:
  switch (symval(functor_of(a0))) {
 case functor_empty_1:
 message_7_2:
  if (!isint(a1)) goto message_7_4;
  x0 = makeint(0L);
  gblt_eq(a1,x0,message_7_3);
  unify2(a6, a5, a4, a3);
  unify_value(a2, a1);
  x1 = arg(a0, 0);
  x2 = makesym(atom_yes);
  unify_value(x1, x2);
  proceed();
 message_7_3:
  x1 = makeint(0L);
  gblt_not_eq(a1,x1,message_7_interrupt);
  unify2(a6, a5, a4, a3);
  unify_value(a2, a1);
  x2 = arg(a0, 0);
  x3 = makesym(atom_no);
  unify_value(x2, x3);
  proceed();
 message_7_4:
  if (!isref(a1)) goto message_7_interrupt;
  deref_and_jump(a1,message_7_2);
  *reasonp++ =  a1;
  goto message_7_interrupt;
 case functor_put_1:
  unify(a4, a3);
  x0 = makeint(1L);
  bblt_add_no_check_y(a1,x0,x1);
  unify(a2, x1);
  a0 = arg(a0, 0);
  a1 = a5;
  a2 = a6;
  execute(dlist_3_0);
  goto dlist_3_ext_interrupt;
 case functor_get_1:
 message_7_5:
  if (!isint(a1)) goto message_7_6;
  x0 = makeint(0L);
  gblt_greater(a1,x0,message_7_interrupt);
  unify(a6, a5);
  x1 = makeint(1L);
  bblt_sub_no_check(a1,x1,x2);
  unify_value(a2, x2);
  a0 = arg(a0, 0);
  a1 = a3;
  a2 = a4;
  execute(dlist_3_0);
  goto dlist_3_ext_interrupt;
 message_7_6:
  if (!isref(a1)) goto message_7_interrupt;
  deref_and_jump(a1,message_7_5);
  *reasonp++ =  a1;
  goto message_7_interrupt;
 case functor_get__if__any_1:
 message_7_7:
  if (!isint(a1)) goto message_7_9;
  x0 = makeint(0L);
  gblt_eq(a1,x0,message_7_8);
  unify2(a6, a5, a4, a3);
  unify_value(a2, a1);
  x1 = arg(a0, 0);
  x2 = makefunctor(&vector_const_0);
  unify_value(x1, x2);
  proceed();
 message_7_8:
  x1 = makeint(0L);
  gblt_not_eq(a1,x1,message_7_interrupt);
  allocp[0] = x2 = makeref(&allocp[0]);
  allocp[1] = x2;
  x3 = create_vector(&allocp[1], 1, allocp);
  x4 = arg(a0, 0);
  allocp += 2;
  unify_value(x4, x3);
  allocp[0] = makesym(functor_get_1);
  allocp[1] = x2;
  x5 = makefunctor(&allocp[0]);
  a0 = x5;
  allocp += 2;
  execute(message_7_0);
  goto message_7_ext_interrupt;
 message_7_9:
  if (!isref(a1)) goto message_7_interrupt;
  deref_and_jump(a1,message_7_7);
  *reasonp++ =  a1;
  goto message_7_interrupt;
 case functor_get__all_1:
  unify(a6, a5);
  x0 = makeint(0L);
  unify_value(a2, x0);
  x1 = a1;
  a1 = a3;
  a2 = a4;
  a3 = arg(a0, 0);
  a0 = x1;
  execute(get__all_4_0);
  goto get__all_4_ext_interrupt;
 case functor_carbon__copy_1:
  unify3(a6, a5, a4, a3, a2, a1);
  x0 = a1;
  a1 = a3;
  allocp[0] = x1 = makeref(&allocp[0]);
  a2 = x1;
  a3 = arg(a0, 0);
  a0 = x0;
  allocp += 1;
  execute(get__all_4_0);
  goto get__all_4_ext_interrupt;
  };
  goto message_7_interrupt;
 case VARREF:
  deref_and_jump(a0,message_7_1);
  *reasonp++ =  a0;
  goto message_7_interrupt;
  };
  goto message_7_interrupt;
 message_7_ext_interrupt:
  reasonp = 0l;
 message_7_interrupt:
  toppred = &predicate_queue_xmessage_7;
  goto interrupt_7;
 }

 dlist_3_top: {
  q x0;
  a1 = qp->args[1];
  a2 = qp->args[2];
  qp = qp->next;
 dlist_3_clear_reason:
  reasonp = reasons;
 dlist_3_0:
  allocp[0] = a2;
  allocp[1] = a0;
  x0 = makecons(&allocp[0]);
  allocp += 2;
  unify_value(a1, x0);
  proceed();
 dlist_3_ext_interrupt:
  reasonp = 0l;
 dlist_3_interrupt:
  toppred = &predicate_queue_xdlist_3;
  goto interrupt_3;
 }

 get__all_4_top: {
  q x0, x1, x2, x3, x4, x5;
  a1 = qp->args[1];
  a2 = qp->args[2];
  a3 = qp->args[3];
  qp = qp->next;
 get__all_4_clear_reason:
  reasonp = reasons;
 get__all_4_0:
 get__all_4_1:
  if (!isint(a0)) goto get__all_4_2;
  x0 = makeint(0L);
  gblt_eq(a0,x0,get__all_4_3);
  unify(a1, a2);
  x1 = NILATOM;
  unify_value(a3, x1);
  proceed();
 get__all_4_2:
  if (!isref(a0)) goto get__all_4_3;
  deref_and_jump(a0,get__all_4_1);
  *reasonp++ =  a0;
  goto get__all_4_3;
 get__all_4_3:
 get__all_4_4:
  switch (ptagof(a1)) {
 case CONS:
 get__all_4_5:
  if (!isint(a0)) goto get__all_4_6;
  x0 = makeint(0L);
  gblt_not_eq(a0,x0,get__all_4_interrupt);
  allocp[0] = x2 = makeref(&allocp[0]);
  x3 = car_of(a1);
  allocp[1] = x3;
  x1 = makecons(&allocp[0]);
  allocp += 2;
  unify_value(a3, x1);
  x4 = makeint(1L);
  bblt_sub_no_check(a0,x4,x5);
  a0 = x5;
  a1 = cdr_of(a1);
  a3 = x2;
  execute(get__all_4_clear_reason);
  goto get__all_4_ext_interrupt;
 get__all_4_6:
  if (!isref(a0)) goto get__all_4_interrupt;
  deref_and_jump(a0,get__all_4_5);
  *reasonp++ =  a0;
  goto get__all_4_interrupt;
 case VARREF:
  deref_and_jump(a1,get__all_4_4);
  *reasonp++ =  a1;
  goto get__all_4_interrupt;
  };
  goto get__all_4_interrupt;
 get__all_4_ext_interrupt:
  reasonp = 0l;
 get__all_4_interrupt:
  toppred = &predicate_queue_xget__all_4;
  goto interrupt_4;
 }
 interrupt_7:
  allocp[8] = a6;
 interrupt_6:
  allocp[7] = a5;
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
  loop_within_module(module_queue);
}
