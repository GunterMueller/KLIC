/* Compiled by KLIC compiler version 3.003 (Mon Mar 29 19:42:59 JST 1999) */
#include <klic/klichdr.h>
#include "atom.h"
#include "funct.h"

module module_stack();
Const struct predicate predicate_stack_xcreate_1 =
   { module_stack, 0, 1 };
Const struct predicate predicate_stack_xmain_3 =
   { module_stack, 1, 3 };
Const struct predicate predicate_stack_xmessage_3 =
   { module_stack, 2, 3 };

module module_stack(glbl, qp, allocp, toppred)
  struct global_variables *glbl;
  struct goalrec *qp;
  register q *allocp;
  Const struct predicate *toppred;
{
  declare_method_table_of(vector);
  static Const vector_structure_type vector_const_0 =
      declare_vector_constant(0,0);
  q a0, a1, a2;

  q *reasonp;
 module_top:
  a0 = qp->args[0];
  switch_on_pred() {
    case_pred(0, create_1_top);
    case_pred(1, main_3_top);
    last_case_pred(2, message_3_top);
  }

 create_1_top: {
  qp = qp->next;
 create_1_clear_reason:
  reasonp = reasons;
 create_1_0:
  a1 = NILATOM;
  a2 = NILATOM;
  execute(main_3_0);
  goto main_3_ext_interrupt;
 create_1_ext_interrupt:
  reasonp = 0l;
 create_1_interrupt:
  goto interrupt_1;
 }

 main_3_top: {
  q x0, x1, x2;
  a1 = qp->args[1];
  a2 = qp->args[2];
  qp = qp->next;
 main_3_clear_reason:
  reasonp = reasons;
 main_3_0:
 main_3_1:
  switch (ptagof(a0)) {
 case CONS:
  x0 = car_of(a0);
 main_3_2:
  switch (ptagof(x0)) {
 case FUNCTOR:
  if (functor_of(x0) != makesym(functor_do_1)) goto main_3_4;
  allocp[0] = a1;
  x2 = cdr_of(a0);
  allocp[1] = x2;
  x1 = makecons(&allocp[0]);
  a0 = arg(x0, 0);
  a1 = x1;
  allocp += 2;
  execute(main_3_0);
  goto main_3_ext_interrupt;
 case VARREF:
  deref_and_jump(x0,main_3_2);
  *reasonp++ =  x0;
  goto main_3_4;
  };
  goto main_3_4;
 case ATOMIC:
  if (a0 != NILATOM) goto main_3_4;
 main_3_3:
  switch (ptagof(a1)) {
 case CONS:
  a0 = car_of(a1);
  a1 = cdr_of(a1);
  execute(main_3_0);
  goto main_3_ext_interrupt;
 case ATOMIC:
  if (a1 != NILATOM) goto main_3_4;
  proceed();
 case VARREF:
  deref_and_jump(a1,main_3_3);
  *reasonp++ =  a1;
  goto main_3_4;
  };
  goto main_3_4;
 case VARREF:
  deref_and_jump(a0,main_3_1);
  *reasonp++ =  a0;
  goto main_3_4;
  };
  goto main_3_4;
 main_3_4:
  otherwise(main_3_interrupt);
 main_3_5:
  switch (ptagof(a0)) {
 case CONS:
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_stack_xmain_3);
  x0 = cdr_of(a0);
  allocp[2] = x0;
  allocp[3] = a1;
  allocp[4] = x1 = makeref(&allocp[4]);
  a0 = car_of(a0);
  a1 = a2;
  a2 = x1;
  qp = (struct goalrec*)&allocp[0];
  allocp += 5;
  execute(message_3_0);
  goto message_3_ext_interrupt;
 case VARREF:
  deref_and_jump(a0,main_3_5);
  *reasonp++ =  a0;
  goto main_3_interrupt;
  };
  goto main_3_interrupt;
 main_3_ext_interrupt:
  reasonp = 0l;
 main_3_interrupt:
  toppred = &predicate_stack_xmain_3;
  goto interrupt_3;
 }

 message_3_top: {
  q x0, x1, x2, x3, x4;
  a1 = qp->args[1];
  a2 = qp->args[2];
  qp = qp->next;
 message_3_clear_reason:
  reasonp = reasons;
 message_3_0:
 message_3_1:
  switch (ptagof(a0)) {
 case FUNCTOR:
  switch (symval(functor_of(a0))) {
 case functor_empty_1:
 message_3_2:
  switch (ptagof(a1)) {
 case CONS:
  x0 = arg(a0, 0);
  x1 = makesym(atom_no);
  unify_value(x0, x1);
  x3 = cdr_of(a1);
  allocp[0] = x3;
  x4 = car_of(a1);
  allocp[1] = x4;
  x2 = makecons(&allocp[0]);
  allocp += 2;
  unify_value(a2, x2);
  proceed();
 case ATOMIC:
  if (a1 != NILATOM) goto message_3_interrupt;
  x0 = arg(a0, 0);
  x1 = makesym(atom_yes);
  unify_value(x0, x1);
  x2 = NILATOM;
  unify_value(a2, x2);
  proceed();
 case VARREF:
  deref_and_jump(a1,message_3_2);
  *reasonp++ =  a1;
  goto message_3_interrupt;
  };
  goto message_3_interrupt;
 case functor_put_1:
  allocp[0] = a1;
  x1 = arg(a0, 0);
  allocp[1] = x1;
  x0 = makecons(&allocp[0]);
  allocp += 2;
  unify_value(a2, x0);
  proceed();
 case functor_get_1:
 message_3_3:
  switch (ptagof(a1)) {
 case CONS:
  x0 = arg(a0, 0);
  x1 = car_of(a1);
  unify(x0, x1);
  x2 = cdr_of(a1);
  unify(a2, x2);
  proceed();
 case VARREF:
  deref_and_jump(a1,message_3_3);
  *reasonp++ =  a1;
  goto message_3_interrupt;
  };
  goto message_3_interrupt;
 case functor_get__if__any_1:
 message_3_4:
  switch (ptagof(a1)) {
 case CONS:
  x1 = car_of(a1);
  allocp[0] = x1;
  x0 = create_vector(&allocp[0], 1, allocp);
  x2 = arg(a0, 0);
  allocp += 1;
  unify_value(x2, x0);
  x3 = cdr_of(a1);
  unify(a2, x3);
  proceed();
 case ATOMIC:
  if (a1 != NILATOM) goto message_3_interrupt;
  x0 = arg(a0, 0);
  x1 = makefunctor(&vector_const_0);
  unify_value(x0, x1);
  x2 = NILATOM;
  unify_value(a2, x2);
  proceed();
 case VARREF:
  deref_and_jump(a1,message_3_4);
  *reasonp++ =  a1;
  goto message_3_interrupt;
  };
  goto message_3_interrupt;
 case functor_get__all_1:
  x0 = arg(a0, 0);
  unify(x0, a1);
  x1 = NILATOM;
  unify_value(a2, x1);
  proceed();
 case functor_carbon__copy_1:
  x0 = arg(a0, 0);
  unify2(x0, a1, a2, a1);
  proceed();
  };
  goto message_3_interrupt;
 case VARREF:
  deref_and_jump(a0,message_3_1);
  *reasonp++ =  a0;
  goto message_3_interrupt;
  };
  goto message_3_interrupt;
 message_3_ext_interrupt:
  reasonp = 0l;
 message_3_interrupt:
  toppred = &predicate_stack_xmessage_3;
  goto interrupt_3;
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
  loop_within_module(module_stack);
}
