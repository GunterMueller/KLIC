/* Compiled by KLIC compiler version 3.003 (Mon Mar 29 19:42:59 JST 1999) */
#include <klic/klichdr.h>
#include "atom.h"
#include "funct.h"

module module_floating__arithmetics();
Const struct predicate predicate_floating__arithmetics_xfloating__point__add_3 =
   { module_floating__arithmetics, 0, 3 };
Const struct predicate predicate_floating__arithmetics_xfloating__point__subtract_3 =
   { module_floating__arithmetics, 1, 3 };
Const struct predicate predicate_floating__arithmetics_xfloating__point__multiply_3 =
   { module_floating__arithmetics, 2, 3 };
Const struct predicate predicate_floating__arithmetics_xfloating__point__divide_3 =
   { module_floating__arithmetics, 3, 3 };
Const struct predicate predicate_floating__arithmetics_xfloating__point__power_3 =
   { module_floating__arithmetics, 4, 3 };
Const struct predicate predicate_floating__arithmetics_xfloating__point__sin_2 =
   { module_floating__arithmetics, 5, 2 };
Const struct predicate predicate_floating__arithmetics_xfloating__point__cos_2 =
   { module_floating__arithmetics, 6, 2 };
Const struct predicate predicate_floating__arithmetics_xfloating__point__tan_2 =
   { module_floating__arithmetics, 7, 2 };
Const struct predicate predicate_floating__arithmetics_xfloating__point__asin_2 =
   { module_floating__arithmetics, 8, 2 };
Const struct predicate predicate_floating__arithmetics_xfloating__point__acos_2 =
   { module_floating__arithmetics, 9, 2 };
Const struct predicate predicate_floating__arithmetics_xfloating__point__atan_2 =
   { module_floating__arithmetics, 10, 2 };
Const struct predicate predicate_floating__arithmetics_xfloating__point__sinh_2 =
   { module_floating__arithmetics, 11, 2 };
Const struct predicate predicate_floating__arithmetics_xfloating__point__cosh_2 =
   { module_floating__arithmetics, 12, 2 };
Const struct predicate predicate_floating__arithmetics_xfloating__point__tanh_2 =
   { module_floating__arithmetics, 13, 2 };
Const struct predicate predicate_floating__arithmetics_xfloating__point__exp_2 =
   { module_floating__arithmetics, 14, 2 };
Const struct predicate predicate_floating__arithmetics_xfloating__point__log_2 =
   { module_floating__arithmetics, 15, 2 };
Const struct predicate predicate_floating__arithmetics_xfloating__point__sqrt_2 =
   { module_floating__arithmetics, 16, 2 };
Const struct predicate predicate_floating__arithmetics_xfloating__point__ceil_2 =
   { module_floating__arithmetics, 17, 2 };
Const struct predicate predicate_floating__arithmetics_xfloating__point__floor_2 =
   { module_floating__arithmetics, 18, 2 };
Const struct predicate predicate_floating__arithmetics_xfloating__point__to__integer_2 =
   { module_floating__arithmetics, 19, 2 };

module module_floating__arithmetics(glbl, qp, allocp, toppred)
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
    case_pred(0, floating__point__add_3_top);
    case_pred(1, floating__point__subtract_3_top);
    case_pred(2, floating__point__multiply_3_top);
    case_pred(3, floating__point__divide_3_top);
    case_pred(4, floating__point__power_3_top);
    case_pred(5, floating__point__sin_2_top);
    case_pred(6, floating__point__cos_2_top);
    case_pred(7, floating__point__tan_2_top);
    case_pred(8, floating__point__asin_2_top);
    case_pred(9, floating__point__acos_2_top);
    case_pred(10, floating__point__atan_2_top);
    case_pred(11, floating__point__sinh_2_top);
    case_pred(12, floating__point__cosh_2_top);
    case_pred(13, floating__point__tanh_2_top);
    case_pred(14, floating__point__exp_2_top);
    case_pred(15, floating__point__log_2_top);
    case_pred(16, floating__point__sqrt_2_top);
    case_pred(17, floating__point__ceil_2_top);
    case_pred(18, floating__point__floor_2_top);
    last_case_pred(19, floating__point__to__integer_2_top);
  }

 floating__point__add_3_top: {
  a2 = qp->args[2];
  qp = qp->next;
 floating__point__add_3_clear_reason:
  reasonp = reasons;
 floating__point__add_3_0:
  generic_arg[0] = a1;
  generic_arg[1] = a2;
  call_generic(a0, functor_add_2, 0);
  proceed();
 floating__point__add_3_ext_interrupt:
  reasonp = 0l;
 floating__point__add_3_interrupt:
  goto interrupt_3;
 }

 floating__point__subtract_3_top: {
  a2 = qp->args[2];
  qp = qp->next;
 floating__point__subtract_3_clear_reason:
  reasonp = reasons;
 floating__point__subtract_3_0:
  generic_arg[0] = a1;
  generic_arg[1] = a2;
  call_generic(a0, functor_subtract_2, 0);
  proceed();
 floating__point__subtract_3_ext_interrupt:
  reasonp = 0l;
 floating__point__subtract_3_interrupt:
  goto interrupt_3;
 }

 floating__point__multiply_3_top: {
  a2 = qp->args[2];
  qp = qp->next;
 floating__point__multiply_3_clear_reason:
  reasonp = reasons;
 floating__point__multiply_3_0:
  generic_arg[0] = a1;
  generic_arg[1] = a2;
  call_generic(a0, functor_multiply_2, 0);
  proceed();
 floating__point__multiply_3_ext_interrupt:
  reasonp = 0l;
 floating__point__multiply_3_interrupt:
  goto interrupt_3;
 }

 floating__point__divide_3_top: {
  a2 = qp->args[2];
  qp = qp->next;
 floating__point__divide_3_clear_reason:
  reasonp = reasons;
 floating__point__divide_3_0:
  generic_arg[0] = a1;
  generic_arg[1] = a2;
  call_generic(a0, functor_divide_2, 0);
  proceed();
 floating__point__divide_3_ext_interrupt:
  reasonp = 0l;
 floating__point__divide_3_interrupt:
  goto interrupt_3;
 }

 floating__point__power_3_top: {
  a2 = qp->args[2];
  qp = qp->next;
 floating__point__power_3_clear_reason:
  reasonp = reasons;
 floating__point__power_3_0:
  generic_arg[0] = a1;
  generic_arg[1] = a2;
  call_generic(a0, functor_power_2, 0);
  proceed();
 floating__point__power_3_ext_interrupt:
  reasonp = 0l;
 floating__point__power_3_interrupt:
  goto interrupt_3;
 }

 floating__point__sin_2_top: {
  qp = qp->next;
 floating__point__sin_2_clear_reason:
  reasonp = reasons;
 floating__point__sin_2_0:
  generic_arg[0] = a1;
  call_generic(a0, functor_sin_1, 0);
  proceed();
 floating__point__sin_2_ext_interrupt:
  reasonp = 0l;
 floating__point__sin_2_interrupt:
  goto interrupt_2;
 }

 floating__point__cos_2_top: {
  qp = qp->next;
 floating__point__cos_2_clear_reason:
  reasonp = reasons;
 floating__point__cos_2_0:
  generic_arg[0] = a1;
  call_generic(a0, functor_cos_1, 0);
  proceed();
 floating__point__cos_2_ext_interrupt:
  reasonp = 0l;
 floating__point__cos_2_interrupt:
  goto interrupt_2;
 }

 floating__point__tan_2_top: {
  qp = qp->next;
 floating__point__tan_2_clear_reason:
  reasonp = reasons;
 floating__point__tan_2_0:
  generic_arg[0] = a1;
  call_generic(a0, functor_tan_1, 0);
  proceed();
 floating__point__tan_2_ext_interrupt:
  reasonp = 0l;
 floating__point__tan_2_interrupt:
  goto interrupt_2;
 }

 floating__point__asin_2_top: {
  qp = qp->next;
 floating__point__asin_2_clear_reason:
  reasonp = reasons;
 floating__point__asin_2_0:
  generic_arg[0] = a1;
  call_generic(a0, functor_asin_1, 0);
  proceed();
 floating__point__asin_2_ext_interrupt:
  reasonp = 0l;
 floating__point__asin_2_interrupt:
  goto interrupt_2;
 }

 floating__point__acos_2_top: {
  qp = qp->next;
 floating__point__acos_2_clear_reason:
  reasonp = reasons;
 floating__point__acos_2_0:
  generic_arg[0] = a1;
  call_generic(a0, functor_acos_1, 0);
  proceed();
 floating__point__acos_2_ext_interrupt:
  reasonp = 0l;
 floating__point__acos_2_interrupt:
  goto interrupt_2;
 }

 floating__point__atan_2_top: {
  qp = qp->next;
 floating__point__atan_2_clear_reason:
  reasonp = reasons;
 floating__point__atan_2_0:
  generic_arg[0] = a1;
  call_generic(a0, functor_atan_1, 0);
  proceed();
 floating__point__atan_2_ext_interrupt:
  reasonp = 0l;
 floating__point__atan_2_interrupt:
  goto interrupt_2;
 }

 floating__point__sinh_2_top: {
  qp = qp->next;
 floating__point__sinh_2_clear_reason:
  reasonp = reasons;
 floating__point__sinh_2_0:
  generic_arg[0] = a1;
  call_generic(a0, functor_sinh_1, 0);
  proceed();
 floating__point__sinh_2_ext_interrupt:
  reasonp = 0l;
 floating__point__sinh_2_interrupt:
  goto interrupt_2;
 }

 floating__point__cosh_2_top: {
  qp = qp->next;
 floating__point__cosh_2_clear_reason:
  reasonp = reasons;
 floating__point__cosh_2_0:
  generic_arg[0] = a1;
  call_generic(a0, functor_cosh_1, 0);
  proceed();
 floating__point__cosh_2_ext_interrupt:
  reasonp = 0l;
 floating__point__cosh_2_interrupt:
  goto interrupt_2;
 }

 floating__point__tanh_2_top: {
  qp = qp->next;
 floating__point__tanh_2_clear_reason:
  reasonp = reasons;
 floating__point__tanh_2_0:
  generic_arg[0] = a1;
  call_generic(a0, functor_tanh_1, 0);
  proceed();
 floating__point__tanh_2_ext_interrupt:
  reasonp = 0l;
 floating__point__tanh_2_interrupt:
  goto interrupt_2;
 }

 floating__point__exp_2_top: {
  qp = qp->next;
 floating__point__exp_2_clear_reason:
  reasonp = reasons;
 floating__point__exp_2_0:
  generic_arg[0] = a1;
  call_generic(a0, functor_exp_1, 0);
  proceed();
 floating__point__exp_2_ext_interrupt:
  reasonp = 0l;
 floating__point__exp_2_interrupt:
  goto interrupt_2;
 }

 floating__point__log_2_top: {
  qp = qp->next;
 floating__point__log_2_clear_reason:
  reasonp = reasons;
 floating__point__log_2_0:
  generic_arg[0] = a1;
  call_generic(a0, functor_log_1, 0);
  proceed();
 floating__point__log_2_ext_interrupt:
  reasonp = 0l;
 floating__point__log_2_interrupt:
  goto interrupt_2;
 }

 floating__point__sqrt_2_top: {
  qp = qp->next;
 floating__point__sqrt_2_clear_reason:
  reasonp = reasons;
 floating__point__sqrt_2_0:
  generic_arg[0] = a1;
  call_generic(a0, functor_sqrt_1, 0);
  proceed();
 floating__point__sqrt_2_ext_interrupt:
  reasonp = 0l;
 floating__point__sqrt_2_interrupt:
  goto interrupt_2;
 }

 floating__point__ceil_2_top: {
  qp = qp->next;
 floating__point__ceil_2_clear_reason:
  reasonp = reasons;
 floating__point__ceil_2_0:
  generic_arg[0] = a1;
  call_generic(a0, functor_ceil_1, 0);
  proceed();
 floating__point__ceil_2_ext_interrupt:
  reasonp = 0l;
 floating__point__ceil_2_interrupt:
  goto interrupt_2;
 }

 floating__point__floor_2_top: {
  qp = qp->next;
 floating__point__floor_2_clear_reason:
  reasonp = reasons;
 floating__point__floor_2_0:
  generic_arg[0] = a1;
  call_generic(a0, functor_floor_1, 0);
  proceed();
 floating__point__floor_2_ext_interrupt:
  reasonp = 0l;
 floating__point__floor_2_interrupt:
  goto interrupt_2;
 }

 floating__point__to__integer_2_top: {
  qp = qp->next;
 floating__point__to__integer_2_clear_reason:
  reasonp = reasons;
 floating__point__to__integer_2_0:
  generic_arg[0] = a1;
  call_generic(a0, functor_int_1, 0);
  proceed();
 floating__point__to__integer_2_ext_interrupt:
  reasonp = 0l;
 floating__point__to__integer_2_interrupt:
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
  loop_within_module(module_floating__arithmetics);
}
