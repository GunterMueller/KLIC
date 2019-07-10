/* Compiled by KLIC compiler version 3.003 (Mon Mar 29 19:42:59 JST 1999) */
#include <klic/klichdr.h>
#include "atom.h"
#include "funct.h"

module module_klic__comp__bb();
Const struct predicate predicate_klic__comp__bb_xis__body__builtin_2 =
   { module_klic__comp__bb, 0, 2 };
Const struct predicate predicate_klic__comp__bb_xint__binary_2 =
   { module_klic__comp__bb, 1, 2 };
Const struct predicate predicate_klic__comp__bb_xint__unary_2 =
   { module_klic__comp__bb, 2, 2 };
Const struct predicate predicate_klic__comp__bb_xfloat__binary_2 =
   { module_klic__comp__bb, 3, 2 };
Const struct predicate predicate_klic__comp__bb_xfloat__unary_2 =
   { module_klic__comp__bb, 4, 2 };

module module_klic__comp__bb(glbl, qp, allocp, toppred)
  struct global_variables *glbl;
  struct goalrec *qp;
  register q *allocp;
  Const struct predicate *toppred;
{
  static Const q cons_const_0[] = {
    NILATOM,
    makesym(atom_int),
  };
  static Const q cons_const_1[] = {
    NILATOM,
    makesym(atom_int),
  };
  static Const q cons_const_2[] = {
    makecons(cons_const_1),
    makesym(atom_int),
  };
  static Const q funct_const_3[] = {
    makesym(functor_object_1),
    makesym(atom_float),
  };
  static Const q cons_const_4[] = {
    NILATOM,
    makefunctor(funct_const_3),
  };
  static Const q funct_const_5[] = {
    makesym(functor_object_1),
    makesym(atom_float),
  };
  static Const q cons_const_6[] = {
    NILATOM,
    makefunctor(funct_const_5),
  };
  static Const q funct_const_7[] = {
    makesym(functor_object_1),
    makesym(atom_float),
  };
  static Const q cons_const_8[] = {
    makecons(cons_const_6),
    makefunctor(funct_const_7),
  };
  q a0, a1;

  q *reasonp;
 module_top:
  a0 = qp->args[0];
  a1 = qp->args[1];
  switch_on_pred() {
    case_pred(0, is__body__builtin_2_top);
    case_pred(1, int__binary_2_top);
    case_pred(2, int__unary_2_top);
    case_pred(3, float__binary_2_top);
    last_case_pred(4, float__unary_2_top);
  }

 is__body__builtin_2_top: {
  q x0, x1, x2, x3;
  qp = qp->next;
 is__body__builtin_2_clear_reason:
  reasonp = reasons;
 is__body__builtin_2_0:
 is__body__builtin_2_1:
  switch (ptagof(a0)) {
 case FUNCTOR:
  switch (symval(functor_of(a0))) {
 case functor_add_3:
  allocp[0] = makesym(functor_bblt__add_3);
  x1 = arg(a0, 0);
  allocp[1] = x1;
  x2 = arg(a0, 1);
  allocp[2] = x2;
  x3 = arg(a0, 2);
  allocp[3] = x3;
  x0 = makefunctor(&allocp[0]);
  a0 = x0;
  allocp += 4;
  execute(int__binary_2_0);
  goto int__binary_2_ext_interrupt;
 case functor_subtract_3:
  allocp[0] = makesym(functor_bblt__sub_3);
  x1 = arg(a0, 0);
  allocp[1] = x1;
  x2 = arg(a0, 1);
  allocp[2] = x2;
  x3 = arg(a0, 2);
  allocp[3] = x3;
  x0 = makefunctor(&allocp[0]);
  a0 = x0;
  allocp += 4;
  execute(int__binary_2_0);
  goto int__binary_2_ext_interrupt;
 case functor_multiply_3:
  allocp[0] = makesym(functor_bblt__mult_3);
  x1 = arg(a0, 0);
  allocp[1] = x1;
  x2 = arg(a0, 1);
  allocp[2] = x2;
  x3 = arg(a0, 2);
  allocp[3] = x3;
  x0 = makefunctor(&allocp[0]);
  a0 = x0;
  allocp += 4;
  execute(int__binary_2_0);
  goto int__binary_2_ext_interrupt;
 case functor_divide_3:
  allocp[0] = makesym(functor_bblt__div_3);
  x1 = arg(a0, 0);
  allocp[1] = x1;
  x2 = arg(a0, 1);
  allocp[2] = x2;
  x3 = arg(a0, 2);
  allocp[3] = x3;
  x0 = makefunctor(&allocp[0]);
  a0 = x0;
  allocp += 4;
  execute(int__binary_2_0);
  goto int__binary_2_ext_interrupt;
 case functor_modulo_3:
  allocp[0] = makesym(functor_bblt__mod_3);
  x1 = arg(a0, 0);
  allocp[1] = x1;
  x2 = arg(a0, 1);
  allocp[2] = x2;
  x3 = arg(a0, 2);
  allocp[3] = x3;
  x0 = makefunctor(&allocp[0]);
  a0 = x0;
  allocp += 4;
  execute(int__binary_2_0);
  goto int__binary_2_ext_interrupt;
 case functor_and_3:
  allocp[0] = makesym(functor_bblt__and_3);
  x1 = arg(a0, 0);
  allocp[1] = x1;
  x2 = arg(a0, 1);
  allocp[2] = x2;
  x3 = arg(a0, 2);
  allocp[3] = x3;
  x0 = makefunctor(&allocp[0]);
  a0 = x0;
  allocp += 4;
  execute(int__binary_2_0);
  goto int__binary_2_ext_interrupt;
 case functor_or_3:
  allocp[0] = makesym(functor_bblt__or_3);
  x1 = arg(a0, 0);
  allocp[1] = x1;
  x2 = arg(a0, 1);
  allocp[2] = x2;
  x3 = arg(a0, 2);
  allocp[3] = x3;
  x0 = makefunctor(&allocp[0]);
  a0 = x0;
  allocp += 4;
  execute(int__binary_2_0);
  goto int__binary_2_ext_interrupt;
 case functor_exclusive__or_3:
  allocp[0] = makesym(functor_bblt__exclusive__or_3);
  x1 = arg(a0, 0);
  allocp[1] = x1;
  x2 = arg(a0, 1);
  allocp[2] = x2;
  x3 = arg(a0, 2);
  allocp[3] = x3;
  x0 = makefunctor(&allocp[0]);
  a0 = x0;
  allocp += 4;
  execute(int__binary_2_0);
  goto int__binary_2_ext_interrupt;
 case functor_shift__left_3:
  allocp[0] = makesym(functor_bblt__lshift_3);
  x1 = arg(a0, 0);
  allocp[1] = x1;
  x2 = arg(a0, 1);
  allocp[2] = x2;
  x3 = arg(a0, 2);
  allocp[3] = x3;
  x0 = makefunctor(&allocp[0]);
  a0 = x0;
  allocp += 4;
  execute(int__binary_2_0);
  goto int__binary_2_ext_interrupt;
 case functor_shift__right_3:
  allocp[0] = makesym(functor_bblt__rshift_3);
  x1 = arg(a0, 0);
  allocp[1] = x1;
  x2 = arg(a0, 1);
  allocp[2] = x2;
  x3 = arg(a0, 2);
  allocp[3] = x3;
  x0 = makefunctor(&allocp[0]);
  a0 = x0;
  allocp += 4;
  execute(int__binary_2_0);
  goto int__binary_2_ext_interrupt;
 case functor_complement_2:
  allocp[0] = makesym(functor_bblt__complement_2);
  x1 = arg(a0, 0);
  allocp[1] = x1;
  x2 = arg(a0, 1);
  allocp[2] = x2;
  x0 = makefunctor(&allocp[0]);
  a0 = x0;
  allocp += 3;
  execute(int__unary_2_0);
  goto int__unary_2_ext_interrupt;
 case functor_plus_2:
  allocp[0] = makesym(functor_bblt__plus_2);
  x1 = arg(a0, 0);
  allocp[1] = x1;
  x2 = arg(a0, 1);
  allocp[2] = x2;
  x0 = makefunctor(&allocp[0]);
  a0 = x0;
  allocp += 3;
  execute(int__unary_2_0);
  goto int__unary_2_ext_interrupt;
 case functor_minus_2:
  allocp[0] = makesym(functor_bblt__minus_2);
  x1 = arg(a0, 0);
  allocp[1] = x1;
  x2 = arg(a0, 1);
  allocp[2] = x2;
  x0 = makefunctor(&allocp[0]);
  a0 = x0;
  allocp += 3;
  execute(int__unary_2_0);
  goto int__unary_2_ext_interrupt;
 case functor_floating__point__add_3:
  allocp[0] = makesym(functor_bblt__float__add_3);
  x1 = arg(a0, 0);
  allocp[1] = x1;
  x2 = arg(a0, 1);
  allocp[2] = x2;
  x3 = arg(a0, 2);
  allocp[3] = x3;
  x0 = makefunctor(&allocp[0]);
  a0 = x0;
  allocp += 4;
  execute(float__binary_2_0);
  goto float__binary_2_ext_interrupt;
 case functor_floating__point__subtract_3:
  allocp[0] = makesym(functor_bblt__float__sub_3);
  x1 = arg(a0, 0);
  allocp[1] = x1;
  x2 = arg(a0, 1);
  allocp[2] = x2;
  x3 = arg(a0, 2);
  allocp[3] = x3;
  x0 = makefunctor(&allocp[0]);
  a0 = x0;
  allocp += 4;
  execute(float__binary_2_0);
  goto float__binary_2_ext_interrupt;
 case functor_floating__point__multiply_3:
  allocp[0] = makesym(functor_bblt__float__mult_3);
  x1 = arg(a0, 0);
  allocp[1] = x1;
  x2 = arg(a0, 1);
  allocp[2] = x2;
  x3 = arg(a0, 2);
  allocp[3] = x3;
  x0 = makefunctor(&allocp[0]);
  a0 = x0;
  allocp += 4;
  execute(float__binary_2_0);
  goto float__binary_2_ext_interrupt;
 case functor_floating__point__divide_3:
  allocp[0] = makesym(functor_bblt__float__div_3);
  x1 = arg(a0, 0);
  allocp[1] = x1;
  x2 = arg(a0, 1);
  allocp[2] = x2;
  x3 = arg(a0, 2);
  allocp[3] = x3;
  x0 = makefunctor(&allocp[0]);
  a0 = x0;
  allocp += 4;
  execute(float__binary_2_0);
  goto float__binary_2_ext_interrupt;
 case functor_floating__point__power_3:
  allocp[0] = makesym(functor_bblt__float__pow_3);
  x1 = arg(a0, 0);
  allocp[1] = x1;
  x2 = arg(a0, 1);
  allocp[2] = x2;
  x3 = arg(a0, 2);
  allocp[3] = x3;
  x0 = makefunctor(&allocp[0]);
  a0 = x0;
  allocp += 4;
  execute(float__binary_2_0);
  goto float__binary_2_ext_interrupt;
 case functor_floating__point__sin_2:
  allocp[0] = makesym(functor_bblt__float__sin_2);
  x1 = arg(a0, 0);
  allocp[1] = x1;
  x2 = arg(a0, 1);
  allocp[2] = x2;
  x0 = makefunctor(&allocp[0]);
  a0 = x0;
  allocp += 3;
  execute(float__unary_2_0);
  goto float__unary_2_ext_interrupt;
 case functor_floating__point__cos_2:
  allocp[0] = makesym(functor_bblt__float__cos_2);
  x1 = arg(a0, 0);
  allocp[1] = x1;
  x2 = arg(a0, 1);
  allocp[2] = x2;
  x0 = makefunctor(&allocp[0]);
  a0 = x0;
  allocp += 3;
  execute(float__unary_2_0);
  goto float__unary_2_ext_interrupt;
 case functor_floating__point__tan_2:
  allocp[0] = makesym(functor_bblt__float__tan_2);
  x1 = arg(a0, 0);
  allocp[1] = x1;
  x2 = arg(a0, 1);
  allocp[2] = x2;
  x0 = makefunctor(&allocp[0]);
  a0 = x0;
  allocp += 3;
  execute(float__unary_2_0);
  goto float__unary_2_ext_interrupt;
 case functor_floating__point__asin_2:
  allocp[0] = makesym(functor_bblt__float__asin_2);
  x1 = arg(a0, 0);
  allocp[1] = x1;
  x2 = arg(a0, 1);
  allocp[2] = x2;
  x0 = makefunctor(&allocp[0]);
  a0 = x0;
  allocp += 3;
  execute(float__unary_2_0);
  goto float__unary_2_ext_interrupt;
 case functor_floating__point__acos_2:
  allocp[0] = makesym(functor_bblt__float__acos_2);
  x1 = arg(a0, 0);
  allocp[1] = x1;
  x2 = arg(a0, 1);
  allocp[2] = x2;
  x0 = makefunctor(&allocp[0]);
  a0 = x0;
  allocp += 3;
  execute(float__unary_2_0);
  goto float__unary_2_ext_interrupt;
 case functor_floating__point__atan_2:
  allocp[0] = makesym(functor_bblt__float__atan_2);
  x1 = arg(a0, 0);
  allocp[1] = x1;
  x2 = arg(a0, 1);
  allocp[2] = x2;
  x0 = makefunctor(&allocp[0]);
  a0 = x0;
  allocp += 3;
  execute(float__unary_2_0);
  goto float__unary_2_ext_interrupt;
 case functor_floating__point__sinh_2:
  allocp[0] = makesym(functor_bblt__float__sinh_2);
  x1 = arg(a0, 0);
  allocp[1] = x1;
  x2 = arg(a0, 1);
  allocp[2] = x2;
  x0 = makefunctor(&allocp[0]);
  a0 = x0;
  allocp += 3;
  execute(float__unary_2_0);
  goto float__unary_2_ext_interrupt;
 case functor_floating__point__cosh_2:
  allocp[0] = makesym(functor_bblt__float__cosh_2);
  x1 = arg(a0, 0);
  allocp[1] = x1;
  x2 = arg(a0, 1);
  allocp[2] = x2;
  x0 = makefunctor(&allocp[0]);
  a0 = x0;
  allocp += 3;
  execute(float__unary_2_0);
  goto float__unary_2_ext_interrupt;
 case functor_floating__point__tanh_2:
  allocp[0] = makesym(functor_bblt__float__tanh_2);
  x1 = arg(a0, 0);
  allocp[1] = x1;
  x2 = arg(a0, 1);
  allocp[2] = x2;
  x0 = makefunctor(&allocp[0]);
  a0 = x0;
  allocp += 3;
  execute(float__unary_2_0);
  goto float__unary_2_ext_interrupt;
 case functor_floating__point__exp_2:
  allocp[0] = makesym(functor_bblt__float__exp_2);
  x1 = arg(a0, 0);
  allocp[1] = x1;
  x2 = arg(a0, 1);
  allocp[2] = x2;
  x0 = makefunctor(&allocp[0]);
  a0 = x0;
  allocp += 3;
  execute(float__unary_2_0);
  goto float__unary_2_ext_interrupt;
 case functor_floating__point__log_2:
  allocp[0] = makesym(functor_bblt__float__log_2);
  x1 = arg(a0, 0);
  allocp[1] = x1;
  x2 = arg(a0, 1);
  allocp[2] = x2;
  x0 = makefunctor(&allocp[0]);
  a0 = x0;
  allocp += 3;
  execute(float__unary_2_0);
  goto float__unary_2_ext_interrupt;
 case functor_floating__point__sqrt_2:
  allocp[0] = makesym(functor_bblt__float__sqrt_2);
  x1 = arg(a0, 0);
  allocp[1] = x1;
  x2 = arg(a0, 1);
  allocp[2] = x2;
  x0 = makefunctor(&allocp[0]);
  a0 = x0;
  allocp += 3;
  execute(float__unary_2_0);
  goto float__unary_2_ext_interrupt;
 case functor_floating__point__ceil_2:
  allocp[0] = makesym(functor_bblt__float__ceil_2);
  x1 = arg(a0, 0);
  allocp[1] = x1;
  x2 = arg(a0, 1);
  allocp[2] = x2;
  x0 = makefunctor(&allocp[0]);
  a0 = x0;
  allocp += 3;
  execute(float__unary_2_0);
  goto float__unary_2_ext_interrupt;
 case functor_floating__point__floor_2:
  allocp[0] = makesym(functor_bblt__float__floor_2);
  x1 = arg(a0, 0);
  allocp[1] = x1;
  x2 = arg(a0, 1);
  allocp[2] = x2;
  x0 = makefunctor(&allocp[0]);
  a0 = x0;
  allocp += 3;
  execute(float__unary_2_0);
  goto float__unary_2_ext_interrupt;
 case functor_floating__point__to__integer_2:
  allocp[0] = makesym(functor_bblt__fix_2);
  x1 = arg(a0, 0);
  allocp[1] = x1;
  x2 = arg(a0, 1);
  allocp[2] = x2;
  x0 = makefunctor(&allocp[0]);
  allocp[3] = makesym(functor_yes_4);
  allocp[4] = x0;
  allocp[5] = makesym(atom_floating__arithmetics);
  allocp[6] = makecons(cons_const_4);
  allocp[7] = makecons(cons_const_0);
  x3 = makefunctor(&allocp[3]);
  allocp += 8;
  unify_value(a1, x3);
  proceed();
 case functor_current__node_2:
  allocp[0] = makesym(functor_bblt__current__node_2);
  x1 = arg(a0, 0);
  allocp[1] = x1;
  x2 = arg(a0, 1);
  allocp[2] = x2;
  x0 = makefunctor(&allocp[0]);
  allocp[3] = makesym(functor_yes_4);
  allocp[4] = x0;
  allocp[5] = makesym(atom_builtin);
  allocp[6] = NILATOM;
  allocp[7] = makecons(cons_const_2);
  x3 = makefunctor(&allocp[3]);
  allocp += 8;
  unify_value(a1, x3);
  proceed();
  };
  goto is__body__builtin_2_2;
 case VARREF:
  deref_and_jump(a0,is__body__builtin_2_1);
  *reasonp++ =  a0;
  goto is__body__builtin_2_2;
  };
  goto is__body__builtin_2_2;
 is__body__builtin_2_2:
  otherwise(is__body__builtin_2_interrupt);
  x0 = makesym(atom_no);
  unify_value(a1, x0);
  proceed();
 is__body__builtin_2_ext_interrupt:
  reasonp = 0l;
 is__body__builtin_2_interrupt:
  goto interrupt_2;
 }

 int__binary_2_top: {
  q x0;
  qp = qp->next;
 int__binary_2_clear_reason:
  reasonp = reasons;
 int__binary_2_0:
  allocp[0] = makesym(functor_yes_4);
  allocp[1] = a0;
  allocp[2] = makesym(atom_integer__arithmetics);
  allocp[3] = makecons(cons_const_2);
  allocp[4] = makecons(cons_const_0);
  x0 = makefunctor(&allocp[0]);
  allocp += 5;
  unify_value(a1, x0);
  proceed();
 int__binary_2_ext_interrupt:
  reasonp = 0l;
 int__binary_2_interrupt:
  toppred = &predicate_klic__comp__bb_xint__binary_2;
  goto interrupt_2;
 }

 int__unary_2_top: {
  q x0;
  qp = qp->next;
 int__unary_2_clear_reason:
  reasonp = reasons;
 int__unary_2_0:
  allocp[0] = makesym(functor_yes_4);
  allocp[1] = a0;
  allocp[2] = makesym(atom_integer__arithmetics);
  allocp[3] = makecons(cons_const_0);
  allocp[4] = makecons(cons_const_0);
  x0 = makefunctor(&allocp[0]);
  allocp += 5;
  unify_value(a1, x0);
  proceed();
 int__unary_2_ext_interrupt:
  reasonp = 0l;
 int__unary_2_interrupt:
  toppred = &predicate_klic__comp__bb_xint__unary_2;
  goto interrupt_2;
 }

 float__binary_2_top: {
  q x0;
  qp = qp->next;
 float__binary_2_clear_reason:
  reasonp = reasons;
 float__binary_2_0:
  allocp[0] = makesym(functor_yes_4);
  allocp[1] = a0;
  allocp[2] = makesym(atom_floating__arithmetics);
  allocp[3] = makecons(cons_const_8);
  allocp[4] = makecons(cons_const_4);
  x0 = makefunctor(&allocp[0]);
  allocp += 5;
  unify_value(a1, x0);
  proceed();
 float__binary_2_ext_interrupt:
  reasonp = 0l;
 float__binary_2_interrupt:
  toppred = &predicate_klic__comp__bb_xfloat__binary_2;
  goto interrupt_2;
 }

 float__unary_2_top: {
  q x0;
  qp = qp->next;
 float__unary_2_clear_reason:
  reasonp = reasons;
 float__unary_2_0:
  allocp[0] = makesym(functor_yes_4);
  allocp[1] = a0;
  allocp[2] = makesym(atom_floating__arithmetics);
  allocp[3] = makecons(cons_const_4);
  allocp[4] = makecons(cons_const_4);
  x0 = makefunctor(&allocp[0]);
  allocp += 5;
  unify_value(a1, x0);
  proceed();
 float__unary_2_ext_interrupt:
  reasonp = 0l;
 float__unary_2_interrupt:
  toppred = &predicate_klic__comp__bb_xfloat__unary_2;
  goto interrupt_2;
 }
 interrupt_2:
  allocp[3] = a1;
 interrupt_1:
  allocp[2] = a0;
 interrupt_0:
  allocp = interrupt_goal(allocp, toppred, reasonp);
 proceed_label:
  loop_within_module(module_klic__comp__bb);
}
