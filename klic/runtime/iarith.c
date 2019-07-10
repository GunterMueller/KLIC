/* Compiled by KLIC compiler version 3.003 (Mon Mar 29 19:42:59 JST 1999) */
#include <klic/klichdr.h>
#include "atom.h"
#include "funct.h"

module module_integer__arithmetics();
Const struct predicate predicate_integer__arithmetics_xadd_3 =
   { module_integer__arithmetics, 0, 3 };
Const struct predicate predicate_integer__arithmetics_xsubtract_3 =
   { module_integer__arithmetics, 1, 3 };
Const struct predicate predicate_integer__arithmetics_xmultiply_3 =
   { module_integer__arithmetics, 2, 3 };
Const struct predicate predicate_integer__arithmetics_xdivide_3 =
   { module_integer__arithmetics, 3, 3 };
Const struct predicate predicate_integer__arithmetics_xmodulo_3 =
   { module_integer__arithmetics, 4, 3 };
Const struct predicate predicate_integer__arithmetics_xand_3 =
   { module_integer__arithmetics, 5, 3 };
Const struct predicate predicate_integer__arithmetics_xor_3 =
   { module_integer__arithmetics, 6, 3 };
Const struct predicate predicate_integer__arithmetics_xexclusive__or_3 =
   { module_integer__arithmetics, 7, 3 };
Const struct predicate predicate_integer__arithmetics_xshift__right_3 =
   { module_integer__arithmetics, 8, 3 };
Const struct predicate predicate_integer__arithmetics_xshift__left_3 =
   { module_integer__arithmetics, 9, 3 };
Const struct predicate predicate_integer__arithmetics_xcomplement_2 =
   { module_integer__arithmetics, 10, 2 };
Const struct predicate predicate_integer__arithmetics_xplus_2 =
   { module_integer__arithmetics, 11, 2 };
Const struct predicate predicate_integer__arithmetics_xminus_2 =
   { module_integer__arithmetics, 12, 2 };

module module_integer__arithmetics(glbl, qp, allocp, toppred)
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
    case_pred(0, add_3_top);
    case_pred(1, subtract_3_top);
    case_pred(2, multiply_3_top);
    case_pred(3, divide_3_top);
    case_pred(4, modulo_3_top);
    case_pred(5, and_3_top);
    case_pred(6, or_3_top);
    case_pred(7, exclusive__or_3_top);
    case_pred(8, shift__right_3_top);
    case_pred(9, shift__left_3_top);
    case_pred(10, complement_2_top);
    case_pred(11, plus_2_top);
    last_case_pred(12, minus_2_top);
  }

 add_3_top: {
  q x0;
  a2 = qp->args[2];
  qp = qp->next;
 add_3_clear_reason:
  reasonp = reasons;
 add_3_0:
 add_3_1:
  if (!isint(a0)) goto add_3_4;
 add_3_2:
  if (!isint(a1)) goto add_3_3;
  gblt_add(a0,a1,x0,add_3_interrupt);
  unify_value(a2, x0);
  proceed();
 add_3_3:
  if (!isref(a1)) goto add_3_interrupt;
  deref_and_jump(a1,add_3_2);
  *reasonp++ =  a1;
  goto add_3_interrupt;
 add_3_4:
  if (!isref(a0)) goto add_3_interrupt;
  deref_and_jump(a0,add_3_1);
  *reasonp++ =  a0;
  goto add_3_interrupt;
 add_3_ext_interrupt:
  reasonp = 0l;
 add_3_interrupt:
  goto interrupt_3;
 }

 subtract_3_top: {
  q x0;
  a2 = qp->args[2];
  qp = qp->next;
 subtract_3_clear_reason:
  reasonp = reasons;
 subtract_3_0:
 subtract_3_1:
  if (!isint(a0)) goto subtract_3_4;
 subtract_3_2:
  if (!isint(a1)) goto subtract_3_3;
  gblt_subtract(a0,a1,x0,subtract_3_interrupt);
  unify_value(a2, x0);
  proceed();
 subtract_3_3:
  if (!isref(a1)) goto subtract_3_interrupt;
  deref_and_jump(a1,subtract_3_2);
  *reasonp++ =  a1;
  goto subtract_3_interrupt;
 subtract_3_4:
  if (!isref(a0)) goto subtract_3_interrupt;
  deref_and_jump(a0,subtract_3_1);
  *reasonp++ =  a0;
  goto subtract_3_interrupt;
 subtract_3_ext_interrupt:
  reasonp = 0l;
 subtract_3_interrupt:
  goto interrupt_3;
 }

 multiply_3_top: {
  q x0;
  a2 = qp->args[2];
  qp = qp->next;
 multiply_3_clear_reason:
  reasonp = reasons;
 multiply_3_0:
 multiply_3_1:
  if (!isint(a0)) goto multiply_3_4;
 multiply_3_2:
  if (!isint(a1)) goto multiply_3_3;
  gblt_multiply(a0,a1,x0,multiply_3_interrupt);
  unify_value(a2, x0);
  proceed();
 multiply_3_3:
  if (!isref(a1)) goto multiply_3_interrupt;
  deref_and_jump(a1,multiply_3_2);
  *reasonp++ =  a1;
  goto multiply_3_interrupt;
 multiply_3_4:
  if (!isref(a0)) goto multiply_3_interrupt;
  deref_and_jump(a0,multiply_3_1);
  *reasonp++ =  a0;
  goto multiply_3_interrupt;
 multiply_3_ext_interrupt:
  reasonp = 0l;
 multiply_3_interrupt:
  goto interrupt_3;
 }

 divide_3_top: {
  q x0;
  a2 = qp->args[2];
  qp = qp->next;
 divide_3_clear_reason:
  reasonp = reasons;
 divide_3_0:
 divide_3_1:
  if (!isint(a0)) goto divide_3_4;
 divide_3_2:
  if (!isint(a1)) goto divide_3_3;
  gblt_divide(a0,a1,x0,divide_3_interrupt);
  unify_value(a2, x0);
  proceed();
 divide_3_3:
  if (!isref(a1)) goto divide_3_interrupt;
  deref_and_jump(a1,divide_3_2);
  *reasonp++ =  a1;
  goto divide_3_interrupt;
 divide_3_4:
  if (!isref(a0)) goto divide_3_interrupt;
  deref_and_jump(a0,divide_3_1);
  *reasonp++ =  a0;
  goto divide_3_interrupt;
 divide_3_ext_interrupt:
  reasonp = 0l;
 divide_3_interrupt:
  goto interrupt_3;
 }

 modulo_3_top: {
  q x0;
  a2 = qp->args[2];
  qp = qp->next;
 modulo_3_clear_reason:
  reasonp = reasons;
 modulo_3_0:
 modulo_3_1:
  if (!isint(a0)) goto modulo_3_4;
 modulo_3_2:
  if (!isint(a1)) goto modulo_3_3;
  gblt_modulo(a0,a1,x0,modulo_3_interrupt);
  unify_value(a2, x0);
  proceed();
 modulo_3_3:
  if (!isref(a1)) goto modulo_3_interrupt;
  deref_and_jump(a1,modulo_3_2);
  *reasonp++ =  a1;
  goto modulo_3_interrupt;
 modulo_3_4:
  if (!isref(a0)) goto modulo_3_interrupt;
  deref_and_jump(a0,modulo_3_1);
  *reasonp++ =  a0;
  goto modulo_3_interrupt;
 modulo_3_ext_interrupt:
  reasonp = 0l;
 modulo_3_interrupt:
  goto interrupt_3;
 }

 and_3_top: {
  q x0;
  a2 = qp->args[2];
  qp = qp->next;
 and_3_clear_reason:
  reasonp = reasons;
 and_3_0:
 and_3_1:
  if (!isint(a0)) goto and_3_4;
 and_3_2:
  if (!isint(a1)) goto and_3_3;
  gblt_and(a0,a1,x0,and_3_interrupt);
  unify_value(a2, x0);
  proceed();
 and_3_3:
  if (!isref(a1)) goto and_3_interrupt;
  deref_and_jump(a1,and_3_2);
  *reasonp++ =  a1;
  goto and_3_interrupt;
 and_3_4:
  if (!isref(a0)) goto and_3_interrupt;
  deref_and_jump(a0,and_3_1);
  *reasonp++ =  a0;
  goto and_3_interrupt;
 and_3_ext_interrupt:
  reasonp = 0l;
 and_3_interrupt:
  goto interrupt_3;
 }

 or_3_top: {
  q x0;
  a2 = qp->args[2];
  qp = qp->next;
 or_3_clear_reason:
  reasonp = reasons;
 or_3_0:
 or_3_1:
  if (!isint(a0)) goto or_3_4;
 or_3_2:
  if (!isint(a1)) goto or_3_3;
  gblt_or(a0,a1,x0,or_3_interrupt);
  unify_value(a2, x0);
  proceed();
 or_3_3:
  if (!isref(a1)) goto or_3_interrupt;
  deref_and_jump(a1,or_3_2);
  *reasonp++ =  a1;
  goto or_3_interrupt;
 or_3_4:
  if (!isref(a0)) goto or_3_interrupt;
  deref_and_jump(a0,or_3_1);
  *reasonp++ =  a0;
  goto or_3_interrupt;
 or_3_ext_interrupt:
  reasonp = 0l;
 or_3_interrupt:
  goto interrupt_3;
 }

 exclusive__or_3_top: {
  q x0;
  a2 = qp->args[2];
  qp = qp->next;
 exclusive__or_3_clear_reason:
  reasonp = reasons;
 exclusive__or_3_0:
 exclusive__or_3_1:
  if (!isint(a0)) goto exclusive__or_3_4;
 exclusive__or_3_2:
  if (!isint(a1)) goto exclusive__or_3_3;
  gblt_exclusive_or(a0,a1,x0,exclusive__or_3_interrupt);
  unify_value(a2, x0);
  proceed();
 exclusive__or_3_3:
  if (!isref(a1)) goto exclusive__or_3_interrupt;
  deref_and_jump(a1,exclusive__or_3_2);
  *reasonp++ =  a1;
  goto exclusive__or_3_interrupt;
 exclusive__or_3_4:
  if (!isref(a0)) goto exclusive__or_3_interrupt;
  deref_and_jump(a0,exclusive__or_3_1);
  *reasonp++ =  a0;
  goto exclusive__or_3_interrupt;
 exclusive__or_3_ext_interrupt:
  reasonp = 0l;
 exclusive__or_3_interrupt:
  goto interrupt_3;
 }

 shift__right_3_top: {
  q x0;
  a2 = qp->args[2];
  qp = qp->next;
 shift__right_3_clear_reason:
  reasonp = reasons;
 shift__right_3_0:
 shift__right_3_1:
  if (!isint(a0)) goto shift__right_3_4;
 shift__right_3_2:
  if (!isint(a1)) goto shift__right_3_3;
  gblt_rshift(a0,a1,x0,shift__right_3_interrupt);
  unify_value(a2, x0);
  proceed();
 shift__right_3_3:
  if (!isref(a1)) goto shift__right_3_interrupt;
  deref_and_jump(a1,shift__right_3_2);
  *reasonp++ =  a1;
  goto shift__right_3_interrupt;
 shift__right_3_4:
  if (!isref(a0)) goto shift__right_3_interrupt;
  deref_and_jump(a0,shift__right_3_1);
  *reasonp++ =  a0;
  goto shift__right_3_interrupt;
 shift__right_3_ext_interrupt:
  reasonp = 0l;
 shift__right_3_interrupt:
  goto interrupt_3;
 }

 shift__left_3_top: {
  q x0;
  a2 = qp->args[2];
  qp = qp->next;
 shift__left_3_clear_reason:
  reasonp = reasons;
 shift__left_3_0:
 shift__left_3_1:
  if (!isint(a0)) goto shift__left_3_4;
 shift__left_3_2:
  if (!isint(a1)) goto shift__left_3_3;
  gblt_lshift(a0,a1,x0,shift__left_3_interrupt);
  unify_value(a2, x0);
  proceed();
 shift__left_3_3:
  if (!isref(a1)) goto shift__left_3_interrupt;
  deref_and_jump(a1,shift__left_3_2);
  *reasonp++ =  a1;
  goto shift__left_3_interrupt;
 shift__left_3_4:
  if (!isref(a0)) goto shift__left_3_interrupt;
  deref_and_jump(a0,shift__left_3_1);
  *reasonp++ =  a0;
  goto shift__left_3_interrupt;
 shift__left_3_ext_interrupt:
  reasonp = 0l;
 shift__left_3_interrupt:
  goto interrupt_3;
 }

 complement_2_top: {
  q x0;
  qp = qp->next;
 complement_2_clear_reason:
  reasonp = reasons;
 complement_2_0:
 complement_2_1:
  if (!isint(a0)) goto complement_2_2;
  gblt_complement(a0,x0,complement_2_interrupt);
  unify_value(a1, x0);
  proceed();
 complement_2_2:
  if (!isref(a0)) goto complement_2_interrupt;
  deref_and_jump(a0,complement_2_1);
  *reasonp++ =  a0;
  goto complement_2_interrupt;
 complement_2_ext_interrupt:
  reasonp = 0l;
 complement_2_interrupt:
  goto interrupt_2;
 }

 plus_2_top: {
  q x0;
  qp = qp->next;
 plus_2_clear_reason:
  reasonp = reasons;
 plus_2_0:
 plus_2_1:
  if (!isint(a0)) goto plus_2_2;
  gblt_plus(a0,x0,plus_2_interrupt);
  unify_value(a1, x0);
  proceed();
 plus_2_2:
  if (!isref(a0)) goto plus_2_interrupt;
  deref_and_jump(a0,plus_2_1);
  *reasonp++ =  a0;
  goto plus_2_interrupt;
 plus_2_ext_interrupt:
  reasonp = 0l;
 plus_2_interrupt:
  goto interrupt_2;
 }

 minus_2_top: {
  q x0;
  qp = qp->next;
 minus_2_clear_reason:
  reasonp = reasons;
 minus_2_0:
 minus_2_1:
  if (!isint(a0)) goto minus_2_2;
  gblt_minus(a0,x0,minus_2_interrupt);
  unify_value(a1, x0);
  proceed();
 minus_2_2:
  if (!isref(a0)) goto minus_2_interrupt;
  deref_and_jump(a0,minus_2_1);
  *reasonp++ =  a0;
  goto minus_2_interrupt;
 minus_2_ext_interrupt:
  reasonp = 0l;
 minus_2_interrupt:
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
  loop_within_module(module_integer__arithmetics);
}
