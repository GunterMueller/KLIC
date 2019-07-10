/* Compiled by KLIC compiler version 3.003 (Mon Mar 29 19:42:59 JST 1999) */
#include <klic/klichdr.h>
#include "atom.h"
#include "funct.h"

module module_klic__comp__gb();
Const struct predicate predicate_klic__comp__gb_xis__guard__builtin_2 =
   { module_klic__comp__gb, 0, 2 };
Const struct predicate predicate_klic__comp__gb_xint__compare_4 =
   { module_klic__comp__gb, 1, 4 };
Const struct predicate predicate_klic__comp__gb_xint__binary_5 =
   { module_klic__comp__gb, 2, 5 };
Const struct predicate predicate_klic__comp__gb_xint__unary_4 =
   { module_klic__comp__gb, 3, 4 };
Const struct predicate predicate_klic__comp__gb_xbuiltin_4 =
   { module_klic__comp__gb, 4, 4 };

module module_klic__comp__gb(glbl, qp, allocp, toppred)
  struct global_variables *glbl;
  struct goalrec *qp;
  register q *allocp;
  Const struct predicate *toppred;
{
  static Const q funct_const_0[] = {
    makesym(functor_object_1),
    makesym(atom_vector),
  };
  static Const q funct_const_1[] = {
    makesym(functor_object_1),
    makesym(atom_float),
  };
  q a0, a1, a2, a3, a4;

  q *reasonp;
 module_top:
  a0 = qp->args[0];
  a1 = qp->args[1];
  switch_on_pred() {
    case_pred(0, is__guard__builtin_2_top);
    case_pred(1, int__compare_4_top);
    case_pred(2, int__binary_5_top);
    case_pred(3, int__unary_4_top);
    last_case_pred(4, builtin_4_top);
  }

 is__guard__builtin_2_top: {
  q x0, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13, x14, x15, x16, x17, x18, x19;
  qp = qp->next;
 is__guard__builtin_2_clear_reason:
  reasonp = reasons;
 is__guard__builtin_2_0:
 is__guard__builtin_2_1:
  switch (ptagof(a0)) {
 case FUNCTOR:
  switch (symval(functor_of(a0))) {
 case functor_hash_2:
  allocp[0] = makesym(functor__3A_2);
  x1 = arg(a0, 0);
  allocp[1] = x1;
  allocp[2] = makesym(atom_bound);
  x0 = makefunctor(&allocp[0]);
  allocp[3] = NILATOM;
  allocp[4] = x0;
  x2 = makecons(&allocp[3]);
  allocp[5] = makesym(functor__3A_2);
  x4 = arg(a0, 1);
  allocp[6] = x4;
  allocp[7] = makesym(atom_int);
  x3 = makefunctor(&allocp[5]);
  a0 = x2;
  a2 = makesym(atom_gblt__hash);
  a3 = a1;
  a1 = x3;
  allocp += 8;
  execute(builtin_4_0);
  goto builtin_4_ext_interrupt;
 case functor_hash_3:
  allocp[0] = makesym(functor__3A_2);
  x1 = arg(a0, 0);
  allocp[1] = x1;
  allocp[2] = makesym(atom_bound);
  x0 = makefunctor(&allocp[0]);
  allocp[3] = makesym(functor__3A_2);
  x3 = arg(a0, 1);
  allocp[4] = x3;
  allocp[5] = makesym(atom_int);
  x2 = makefunctor(&allocp[3]);
  allocp[6] = NILATOM;
  allocp[7] = x2;
  x4 = makecons(&allocp[6]);
  allocp[8] = x4;
  allocp[9] = x0;
  x5 = makecons(&allocp[8]);
  allocp[10] = makesym(functor__3A_2);
  x7 = arg(a0, 2);
  allocp[11] = x7;
  allocp[12] = makesym(atom_int);
  x6 = makefunctor(&allocp[10]);
  a0 = x5;
  a2 = makesym(atom_gblt__deep__hash);
  a3 = a1;
  a1 = x6;
  allocp += 13;
  execute(builtin_4_0);
  goto builtin_4_ext_interrupt;
 case functor_compare_3:
  allocp[0] = makesym(functor__3A_2);
  x1 = arg(a0, 0);
  allocp[1] = x1;
  allocp[2] = makesym(atom_bound);
  x0 = makefunctor(&allocp[0]);
  allocp[3] = makesym(functor__3A_2);
  x3 = arg(a0, 1);
  allocp[4] = x3;
  allocp[5] = makesym(atom_bound);
  x2 = makefunctor(&allocp[3]);
  allocp[6] = NILATOM;
  allocp[7] = x2;
  x4 = makecons(&allocp[6]);
  allocp[8] = x4;
  allocp[9] = x0;
  x5 = makecons(&allocp[8]);
  allocp[10] = makesym(functor__3A_2);
  x7 = arg(a0, 2);
  allocp[11] = x7;
  allocp[12] = makesym(atom_int);
  x6 = makefunctor(&allocp[10]);
  a0 = x5;
  a2 = makesym(atom_gblt__compare);
  a3 = a1;
  a1 = x6;
  allocp += 13;
  execute(builtin_4_0);
  goto builtin_4_ext_interrupt;
 case functor_current__priority_1:
  allocp[0] = makesym(functor__3A_2);
  x1 = arg(a0, 0);
  allocp[1] = x1;
  allocp[2] = makesym(atom_int);
  x0 = makefunctor(&allocp[0]);
  a0 = NILATOM;
  a2 = makesym(atom_gblt__current__prio);
  a3 = a1;
  a1 = x0;
  allocp += 3;
  execute(builtin_4_0);
  goto builtin_4_ext_interrupt;
 case functor_wait_1:
  allocp[0] = makesym(functor__3A_2);
  x1 = arg(a0, 0);
  allocp[1] = x1;
  allocp[2] = makesym(atom_bound);
  x0 = makefunctor(&allocp[0]);
  allocp[3] = NILATOM;
  allocp[4] = x0;
  x2 = makecons(&allocp[3]);
  a0 = x2;
  a2 = makesym(atom_gblt__wait);
  a3 = a1;
  a1 = makesym(atom_none);
  allocp += 5;
  execute(builtin_4_0);
  goto builtin_4_ext_interrupt;
 case functor_list_1:
  allocp[0] = makesym(functor__3A_2);
  x1 = arg(a0, 0);
  allocp[1] = x1;
  allocp[2] = makesym(atom_list);
  x0 = makefunctor(&allocp[0]);
  allocp[3] = NILATOM;
  allocp[4] = x0;
  x2 = makecons(&allocp[3]);
  a0 = x2;
  a2 = makesym(atom_gblt__list);
  a3 = a1;
  a1 = makesym(atom_none);
  allocp += 5;
  execute(builtin_4_0);
  goto builtin_4_ext_interrupt;
 case functor_atom_1:
  allocp[0] = makesym(functor__3A_2);
  x1 = arg(a0, 0);
  allocp[1] = x1;
  allocp[2] = makesym(atom_atom);
  x0 = makefunctor(&allocp[0]);
  allocp[3] = NILATOM;
  allocp[4] = x0;
  x2 = makecons(&allocp[3]);
  a0 = x2;
  a2 = makesym(atom_gblt__atom);
  a3 = a1;
  a1 = makesym(atom_none);
  allocp += 5;
  execute(builtin_4_0);
  goto builtin_4_ext_interrupt;
 case functor_integer_1:
  allocp[0] = makesym(functor__3A_2);
  x1 = arg(a0, 0);
  allocp[1] = x1;
  allocp[2] = makesym(atom_int);
  x0 = makefunctor(&allocp[0]);
  allocp[3] = NILATOM;
  allocp[4] = x0;
  x2 = makecons(&allocp[3]);
  a0 = x2;
  a2 = makesym(atom_gblt__integer);
  a3 = a1;
  a1 = makesym(atom_none);
  allocp += 5;
  execute(builtin_4_0);
  goto builtin_4_ext_interrupt;
 case functor_atomic_1:
  allocp[0] = makesym(functor__3A_2);
  x1 = arg(a0, 0);
  allocp[1] = x1;
  allocp[2] = makesym(atom_atomic);
  x0 = makefunctor(&allocp[0]);
  allocp[3] = NILATOM;
  allocp[4] = x0;
  x2 = makecons(&allocp[3]);
  a0 = x2;
  a2 = makesym(atom_gblt__atomic);
  a3 = a1;
  a1 = makesym(atom_none);
  allocp += 5;
  execute(builtin_4_0);
  goto builtin_4_ext_interrupt;
 case functor_float_1:
  allocp[0] = makesym(functor__3A_2);
  x1 = arg(a0, 0);
  allocp[1] = x1;
  allocp[2] = makefunctor(funct_const_1);
  x0 = makefunctor(&allocp[0]);
  allocp[3] = NILATOM;
  allocp[4] = x0;
  x2 = makecons(&allocp[3]);
  a0 = x2;
  a2 = makesym(atom_gblt__float);
  a3 = a1;
  a1 = makesym(atom_none);
  allocp += 5;
  execute(builtin_4_0);
  goto builtin_4_ext_interrupt;
 case functor_vector_2:
  allocp[0] = makesym(functor__3A_2);
  x1 = arg(a0, 0);
  allocp[1] = x1;
  allocp[2] = makefunctor(funct_const_0);
  x0 = makefunctor(&allocp[0]);
  allocp[3] = NILATOM;
  allocp[4] = x0;
  x2 = makecons(&allocp[3]);
  allocp[5] = makesym(functor__3A_2);
  x4 = arg(a0, 1);
  allocp[6] = x4;
  allocp[7] = makesym(atom_int);
  x3 = makefunctor(&allocp[5]);
  a0 = x2;
  a2 = makesym(atom_gblt__size__of__vector);
  a3 = a1;
  a1 = x3;
  allocp += 8;
  execute(builtin_4_0);
  goto builtin_4_ext_interrupt;
 case functor_vector__element_3:
  allocp[0] = makesym(functor__3A_2);
  x1 = arg(a0, 0);
  allocp[1] = x1;
  allocp[2] = makefunctor(funct_const_0);
  x0 = makefunctor(&allocp[0]);
  allocp[3] = makesym(functor__3A_2);
  x3 = arg(a0, 1);
  allocp[4] = x3;
  allocp[5] = makesym(atom_int);
  x2 = makefunctor(&allocp[3]);
  allocp[6] = NILATOM;
  allocp[7] = x2;
  x4 = makecons(&allocp[6]);
  allocp[8] = x4;
  allocp[9] = x0;
  x5 = makecons(&allocp[8]);
  allocp[10] = makesym(functor__3A_2);
  x7 = arg(a0, 2);
  allocp[11] = x7;
  allocp[12] = makesym(atom_any);
  x6 = makefunctor(&allocp[10]);
  a0 = x5;
  a2 = makesym(atom_gblt__element__of__vector);
  a3 = a1;
  a1 = x6;
  allocp += 13;
  execute(builtin_4_0);
  goto builtin_4_ext_interrupt;
 case functor_string_3:
  allocp[0] = makesym(functor__3A_2);
  x1 = arg(a0, 0);
  allocp[1] = x1;
  allocp[2] = makesym(atom_object);
  x0 = makefunctor(&allocp[0]);
  allocp[3] = NILATOM;
  allocp[4] = x0;
  x2 = makecons(&allocp[3]);
  allocp[5] = makesym(functor__24_3);
  allocp[6] = x2;
  allocp[7] = makesym(atom_none);
  allocp[8] = makesym(atom_gblt__is__string);
  x3 = makefunctor(&allocp[5]);
  allocp[9] = makesym(functor__3A_2);
  x5 = arg(a0, 0);
  allocp[10] = x5;
  allocp[11] = makesym(atom_object);
  x4 = makefunctor(&allocp[9]);
  allocp[12] = NILATOM;
  allocp[13] = x4;
  x6 = makecons(&allocp[12]);
  allocp[14] = makesym(functor__3A_2);
  x8 = arg(a0, 1);
  allocp[15] = x8;
  allocp[16] = makesym(atom_int);
  x7 = makefunctor(&allocp[14]);
  allocp[17] = makesym(functor__24_3);
  allocp[18] = x6;
  allocp[19] = x7;
  allocp[20] = makesym(atom_gblt__size__of__string);
  x9 = makefunctor(&allocp[17]);
  allocp[21] = makesym(functor__3A_2);
  x11 = arg(a0, 0);
  allocp[22] = x11;
  allocp[23] = makesym(atom_object);
  x10 = makefunctor(&allocp[21]);
  allocp[24] = NILATOM;
  allocp[25] = x10;
  x12 = makecons(&allocp[24]);
  allocp[26] = makesym(functor__3A_2);
  x14 = arg(a0, 2);
  allocp[27] = x14;
  allocp[28] = makesym(atom_int);
  x13 = makefunctor(&allocp[26]);
  allocp[29] = makesym(functor__24_3);
  allocp[30] = x12;
  allocp[31] = x13;
  allocp[32] = makesym(atom_gblt__element__size__of__string);
  x15 = makefunctor(&allocp[29]);
  allocp[33] = NILATOM;
  allocp[34] = x15;
  x16 = makecons(&allocp[33]);
  allocp[35] = x16;
  allocp[36] = x9;
  x17 = makecons(&allocp[35]);
  allocp[37] = x17;
  allocp[38] = x3;
  x18 = makecons(&allocp[37]);
  allocp[39] = makesym(functor_yes_1);
  allocp[40] = x18;
  x19 = makefunctor(&allocp[39]);
  allocp += 41;
  unify_value(a1, x19);
  proceed();
 case functor_string__element_3:
  allocp[0] = makesym(functor__3A_2);
  x1 = arg(a0, 0);
  allocp[1] = x1;
  allocp[2] = makesym(atom_object);
  x0 = makefunctor(&allocp[0]);
  allocp[3] = NILATOM;
  allocp[4] = x0;
  x2 = makecons(&allocp[3]);
  allocp[5] = makesym(functor__24_3);
  allocp[6] = x2;
  allocp[7] = makesym(atom_none);
  allocp[8] = makesym(atom_gblt__is__string);
  x3 = makefunctor(&allocp[5]);
  allocp[9] = makesym(functor__3A_2);
  x5 = arg(a0, 0);
  allocp[10] = x5;
  allocp[11] = makesym(atom_object);
  x4 = makefunctor(&allocp[9]);
  allocp[12] = makesym(functor__3A_2);
  x7 = arg(a0, 1);
  allocp[13] = x7;
  allocp[14] = makesym(atom_int);
  x6 = makefunctor(&allocp[12]);
  allocp[15] = NILATOM;
  allocp[16] = x6;
  x8 = makecons(&allocp[15]);
  allocp[17] = x8;
  allocp[18] = x4;
  x9 = makecons(&allocp[17]);
  allocp[19] = makesym(functor__3A_2);
  x11 = arg(a0, 2);
  allocp[20] = x11;
  allocp[21] = makesym(atom_int);
  x10 = makefunctor(&allocp[19]);
  allocp[22] = makesym(functor__24_3);
  allocp[23] = x9;
  allocp[24] = x10;
  allocp[25] = makesym(atom_gblt__element__of__string);
  x12 = makefunctor(&allocp[22]);
  allocp[26] = NILATOM;
  allocp[27] = x12;
  x13 = makecons(&allocp[26]);
  allocp[28] = x13;
  allocp[29] = x3;
  x14 = makecons(&allocp[28]);
  allocp[30] = makesym(functor_yes_1);
  allocp[31] = x14;
  x15 = makefunctor(&allocp[30]);
  allocp += 32;
  unify_value(a1, x15);
  proceed();
 case functor_functor_3:
  allocp[0] = makesym(functor__3A_2);
  x1 = arg(a0, 0);
  allocp[1] = x1;
  allocp[2] = makesym(atom_bound);
  x0 = makefunctor(&allocp[0]);
  allocp[3] = NILATOM;
  allocp[4] = x0;
  x2 = makecons(&allocp[3]);
  allocp[5] = makesym(functor__3A_2);
  x4 = arg(a0, 1);
  allocp[6] = x4;
  allocp[7] = makesym(atom_atom);
  x3 = makefunctor(&allocp[5]);
  allocp[8] = makesym(functor__24_3);
  allocp[9] = x2;
  allocp[10] = x3;
  allocp[11] = makesym(atom_gblt__pfunctor);
  x5 = makefunctor(&allocp[8]);
  allocp[12] = makesym(functor__3A_2);
  x7 = arg(a0, 0);
  allocp[13] = x7;
  allocp[14] = makesym(atom_bound);
  x6 = makefunctor(&allocp[12]);
  allocp[15] = NILATOM;
  allocp[16] = x6;
  x8 = makecons(&allocp[15]);
  allocp[17] = makesym(functor__3A_2);
  x10 = arg(a0, 2);
  allocp[18] = x10;
  allocp[19] = makesym(atom_int);
  x9 = makefunctor(&allocp[17]);
  allocp[20] = makesym(functor__24_3);
  allocp[21] = x8;
  allocp[22] = x9;
  allocp[23] = makesym(atom_gblt__arity);
  x11 = makefunctor(&allocp[20]);
  allocp[24] = NILATOM;
  allocp[25] = x11;
  x12 = makecons(&allocp[24]);
  allocp[26] = x12;
  allocp[27] = x5;
  x13 = makecons(&allocp[26]);
  allocp[28] = makesym(functor_yes_1);
  allocp[29] = x13;
  x14 = makefunctor(&allocp[28]);
  allocp += 30;
  unify_value(a1, x14);
  proceed();
 case functor_arg_3:
  allocp[0] = makesym(functor__3A_2);
  x1 = arg(a0, 0);
  allocp[1] = x1;
  allocp[2] = makesym(atom_int);
  x0 = makefunctor(&allocp[0]);
  allocp[3] = makesym(functor__3A_2);
  x3 = arg(a0, 1);
  allocp[4] = x3;
  allocp[5] = makesym(atom_bound);
  x2 = makefunctor(&allocp[3]);
  allocp[6] = NILATOM;
  allocp[7] = x2;
  x4 = makecons(&allocp[6]);
  allocp[8] = x4;
  allocp[9] = x0;
  x5 = makecons(&allocp[8]);
  allocp[10] = makesym(functor__3A_2);
  x7 = arg(a0, 2);
  allocp[11] = x7;
  allocp[12] = makesym(atom_any);
  x6 = makefunctor(&allocp[10]);
  a0 = x5;
  a2 = makesym(atom_gblt__arg);
  a3 = a1;
  a1 = x6;
  allocp += 13;
  execute(builtin_4_0);
  goto builtin_4_ext_interrupt;
 case functor_less__than_2:
  x0 = arg(a0, 0);
  x1 = arg(a0, 1);
  a2 = makesym(atom_gblt__less);
  a3 = a1;
  a1 = x1;
  a0 = x0;
  execute(int__compare_4_0);
  goto int__compare_4_ext_interrupt;
 case functor_not__less__than_2:
  x0 = arg(a0, 0);
  x1 = arg(a0, 1);
  a2 = makesym(atom_gblt__greater__or__eq);
  a3 = a1;
  a1 = x1;
  a0 = x0;
  execute(int__compare_4_0);
  goto int__compare_4_ext_interrupt;
 case functor_greater__than_2:
  x0 = arg(a0, 0);
  x1 = arg(a0, 1);
  a2 = makesym(atom_gblt__greater);
  a3 = a1;
  a1 = x1;
  a0 = x0;
  execute(int__compare_4_0);
  goto int__compare_4_ext_interrupt;
 case functor_not__greater__than_2:
  x0 = arg(a0, 0);
  x1 = arg(a0, 1);
  a2 = makesym(atom_gblt__less__or__eq);
  a3 = a1;
  a1 = x1;
  a0 = x0;
  execute(int__compare_4_0);
  goto int__compare_4_ext_interrupt;
 case functor_equal_2:
  x0 = arg(a0, 0);
  x1 = arg(a0, 1);
  a2 = makesym(atom_gblt__eq);
  a3 = a1;
  a1 = x1;
  a0 = x0;
  execute(int__compare_4_0);
  goto int__compare_4_ext_interrupt;
 case functor_not__equal_2:
  x0 = arg(a0, 0);
  x1 = arg(a0, 1);
  a2 = makesym(atom_gblt__not__eq);
  a3 = a1;
  a1 = x1;
  a0 = x0;
  execute(int__compare_4_0);
  goto int__compare_4_ext_interrupt;
 case functor_diff_2:
  allocp[0] = makesym(functor__3A_2);
  x1 = arg(a0, 0);
  allocp[1] = x1;
  allocp[2] = makesym(atom_bound);
  x0 = makefunctor(&allocp[0]);
  allocp[3] = makesym(functor__3A_2);
  x3 = arg(a0, 1);
  allocp[4] = x3;
  allocp[5] = makesym(atom_bound);
  x2 = makefunctor(&allocp[3]);
  allocp[6] = NILATOM;
  allocp[7] = x2;
  x4 = makecons(&allocp[6]);
  allocp[8] = x4;
  allocp[9] = x0;
  x5 = makecons(&allocp[8]);
  a0 = x5;
  a2 = makesym(atom_gblt__diff);
  a3 = a1;
  a1 = makesym(atom_none);
  allocp += 10;
  execute(builtin_4_0);
  goto builtin_4_ext_interrupt;
 case functor_and_3:
  x0 = arg(a0, 0);
  x1 = arg(a0, 1);
  a2 = arg(a0, 2);
  a3 = makesym(atom_gblt__and);
  a4 = a1;
  a1 = x1;
  a0 = x0;
  execute(int__binary_5_0);
  goto int__binary_5_ext_interrupt;
 case functor_or_3:
  x0 = arg(a0, 0);
  x1 = arg(a0, 1);
  a2 = arg(a0, 2);
  a3 = makesym(atom_gblt__or);
  a4 = a1;
  a1 = x1;
  a0 = x0;
  execute(int__binary_5_0);
  goto int__binary_5_ext_interrupt;
 case functor_exclusive__or_3:
  x0 = arg(a0, 0);
  x1 = arg(a0, 1);
  a2 = arg(a0, 2);
  a3 = makesym(atom_gblt__exclusive__or);
  a4 = a1;
  a1 = x1;
  a0 = x0;
  execute(int__binary_5_0);
  goto int__binary_5_ext_interrupt;
 case functor_shift__right_3:
  x0 = arg(a0, 0);
  x1 = arg(a0, 1);
  a2 = arg(a0, 2);
  a3 = makesym(atom_gblt__rshift);
  a4 = a1;
  a1 = x1;
  a0 = x0;
  execute(int__binary_5_0);
  goto int__binary_5_ext_interrupt;
 case functor_shift__left_3:
  x0 = arg(a0, 0);
  x1 = arg(a0, 1);
  a2 = arg(a0, 2);
  a3 = makesym(atom_gblt__lshift);
  a4 = a1;
  a1 = x1;
  a0 = x0;
  execute(int__binary_5_0);
  goto int__binary_5_ext_interrupt;
 case functor_add_3:
  x0 = arg(a0, 0);
  x1 = arg(a0, 1);
  a2 = arg(a0, 2);
  a3 = makesym(atom_gblt__add);
  a4 = a1;
  a1 = x1;
  a0 = x0;
  execute(int__binary_5_0);
  goto int__binary_5_ext_interrupt;
 case functor_subtract_3:
  x0 = arg(a0, 0);
  x1 = arg(a0, 1);
  a2 = arg(a0, 2);
  a3 = makesym(atom_gblt__subtract);
  a4 = a1;
  a1 = x1;
  a0 = x0;
  execute(int__binary_5_0);
  goto int__binary_5_ext_interrupt;
 case functor_multiply_3:
  x0 = arg(a0, 0);
  x1 = arg(a0, 1);
  a2 = arg(a0, 2);
  a3 = makesym(atom_gblt__multiply);
  a4 = a1;
  a1 = x1;
  a0 = x0;
  execute(int__binary_5_0);
  goto int__binary_5_ext_interrupt;
 case functor_divide_3:
  x0 = arg(a0, 0);
  x1 = arg(a0, 1);
  a2 = arg(a0, 2);
  a3 = makesym(atom_gblt__divide);
  a4 = a1;
  a1 = x1;
  a0 = x0;
  execute(int__binary_5_0);
  goto int__binary_5_ext_interrupt;
 case functor_modulo_3:
  x0 = arg(a0, 0);
  x1 = arg(a0, 1);
  a2 = arg(a0, 2);
  a3 = makesym(atom_gblt__modulo);
  a4 = a1;
  a1 = x1;
  a0 = x0;
  execute(int__binary_5_0);
  goto int__binary_5_ext_interrupt;
 case functor_complement_2:
  x0 = arg(a0, 0);
  x1 = arg(a0, 1);
  a2 = makesym(atom_gblt__complement);
  a3 = a1;
  a1 = x1;
  a0 = x0;
  execute(int__unary_4_0);
  goto int__unary_4_ext_interrupt;
 case functor_plus_2:
  x0 = arg(a0, 0);
  x1 = arg(a0, 1);
  a2 = makesym(atom_gblt__plus);
  a3 = a1;
  a1 = x1;
  a0 = x0;
  execute(int__unary_4_0);
  goto int__unary_4_ext_interrupt;
 case functor_minus_2:
  x0 = arg(a0, 0);
  x1 = arg(a0, 1);
  a2 = makesym(atom_gblt__minus);
  a3 = a1;
  a1 = x1;
  a0 = x0;
  execute(int__unary_4_0);
  goto int__unary_4_ext_interrupt;
 case functor_floating__point__to__integer_2:
  allocp[0] = makesym(functor__3A_2);
  x1 = arg(a0, 0);
  allocp[1] = x1;
  allocp[2] = makefunctor(funct_const_1);
  x0 = makefunctor(&allocp[0]);
  allocp[3] = NILATOM;
  allocp[4] = x0;
  x2 = makecons(&allocp[3]);
  allocp[5] = makesym(functor__3A_2);
  x4 = arg(a0, 1);
  allocp[6] = x4;
  allocp[7] = makesym(atom_int);
  x3 = makefunctor(&allocp[5]);
  a0 = x2;
  a2 = makesym(atom_gblt__fix);
  a3 = a1;
  a1 = x3;
  allocp += 8;
  execute(builtin_4_0);
  goto builtin_4_ext_interrupt;
 case functor_tag__and__value_3:
  allocp[0] = makesym(functor__3A_2);
  x1 = arg(a0, 0);
  allocp[1] = x1;
  allocp[2] = makesym(atom_bound);
  x0 = makefunctor(&allocp[0]);
  allocp[3] = NILATOM;
  allocp[4] = x0;
  x2 = makecons(&allocp[3]);
  allocp[5] = makesym(functor__3A_2);
  x4 = arg(a0, 1);
  allocp[6] = x4;
  allocp[7] = makesym(atom_int);
  x3 = makefunctor(&allocp[5]);
  allocp[8] = makesym(functor__24_3);
  allocp[9] = x2;
  allocp[10] = x3;
  allocp[11] = makesym(atom_gblt__tag);
  x5 = makefunctor(&allocp[8]);
  allocp[12] = makesym(functor__3A_2);
  x7 = arg(a0, 0);
  allocp[13] = x7;
  allocp[14] = makesym(atom_bound);
  x6 = makefunctor(&allocp[12]);
  allocp[15] = NILATOM;
  allocp[16] = x6;
  x8 = makecons(&allocp[15]);
  allocp[17] = makesym(functor__3A_2);
  x10 = arg(a0, 2);
  allocp[18] = x10;
  allocp[19] = makesym(atom_int);
  x9 = makefunctor(&allocp[17]);
  allocp[20] = makesym(functor__24_3);
  allocp[21] = x8;
  allocp[22] = x9;
  allocp[23] = makesym(atom_gblt__value);
  x11 = makefunctor(&allocp[20]);
  allocp[24] = NILATOM;
  allocp[25] = x11;
  x12 = makecons(&allocp[24]);
  allocp[26] = x12;
  allocp[27] = x5;
  x13 = makecons(&allocp[26]);
  allocp[28] = makesym(functor_yes_1);
  allocp[29] = x13;
  x14 = makefunctor(&allocp[28]);
  allocp += 30;
  unify_value(a1, x14);
  proceed();
 case functor_display__console_1:
  allocp[0] = makesym(functor__3A_2);
  x1 = arg(a0, 0);
  allocp[1] = x1;
  allocp[2] = makesym(atom_bound);
  x0 = makefunctor(&allocp[0]);
  allocp[3] = NILATOM;
  allocp[4] = x0;
  x2 = makecons(&allocp[3]);
  a0 = x2;
  a2 = makesym(atom_gblt__display__console);
  a3 = a1;
  a1 = makesym(atom_none);
  allocp += 5;
  execute(builtin_4_0);
  goto builtin_4_ext_interrupt;
 case functor_put__console_1:
  allocp[0] = makesym(functor__3A_2);
  x1 = arg(a0, 0);
  allocp[1] = x1;
  allocp[2] = makesym(atom_bound);
  x0 = makefunctor(&allocp[0]);
  allocp[3] = NILATOM;
  allocp[4] = x0;
  x2 = makecons(&allocp[3]);
  a0 = x2;
  a2 = makesym(atom_gblt__display__console);
  a3 = a1;
  a1 = makesym(atom_none);
  allocp += 5;
  execute(builtin_4_0);
  goto builtin_4_ext_interrupt;
  };
  goto is__guard__builtin_2_2;
 case VARREF:
  deref_and_jump(a0,is__guard__builtin_2_1);
  *reasonp++ =  a0;
  goto is__guard__builtin_2_2;
  };
  goto is__guard__builtin_2_2;
 is__guard__builtin_2_2:
  otherwise(is__guard__builtin_2_interrupt);
  allocp[0] = makesym(functor_no_1);
  allocp[1] = a0;
  x0 = makefunctor(&allocp[0]);
  allocp += 2;
  unify_value(a1, x0);
  proceed();
 is__guard__builtin_2_ext_interrupt:
  reasonp = 0l;
 is__guard__builtin_2_interrupt:
  goto interrupt_2;
 }

 int__compare_4_top: {
  q x0, x1, x2, x3, x4, x5, x6;
  a2 = qp->args[2];
  a3 = qp->args[3];
  qp = qp->next;
 int__compare_4_clear_reason:
  reasonp = reasons;
 int__compare_4_0:
  allocp[0] = makesym(functor__3A_2);
  allocp[1] = a0;
  allocp[2] = makesym(atom_int);
  x0 = makefunctor(&allocp[0]);
  allocp[3] = makesym(functor__3A_2);
  allocp[4] = a1;
  allocp[5] = makesym(atom_int);
  x1 = makefunctor(&allocp[3]);
  allocp[6] = NILATOM;
  allocp[7] = x1;
  x2 = makecons(&allocp[6]);
  allocp[8] = x2;
  allocp[9] = x0;
  x3 = makecons(&allocp[8]);
  allocp[10] = makesym(functor__24_3);
  allocp[11] = x3;
  allocp[12] = makesym(atom_none);
  allocp[13] = a2;
  x4 = makefunctor(&allocp[10]);
  allocp[14] = NILATOM;
  allocp[15] = x4;
  x5 = makecons(&allocp[14]);
  allocp[16] = makesym(functor_yes_1);
  allocp[17] = x5;
  x6 = makefunctor(&allocp[16]);
  allocp += 18;
  unify_value(a3, x6);
  proceed();
 int__compare_4_ext_interrupt:
  reasonp = 0l;
 int__compare_4_interrupt:
  toppred = &predicate_klic__comp__gb_xint__compare_4;
  goto interrupt_4;
 }

 int__binary_5_top: {
  q x0, x1, x2, x3, x4, x5, x6, x7;
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  qp = qp->next;
 int__binary_5_clear_reason:
  reasonp = reasons;
 int__binary_5_0:
  allocp[0] = makesym(functor__3A_2);
  allocp[1] = a0;
  allocp[2] = makesym(atom_int);
  x0 = makefunctor(&allocp[0]);
  allocp[3] = makesym(functor__3A_2);
  allocp[4] = a1;
  allocp[5] = makesym(atom_int);
  x1 = makefunctor(&allocp[3]);
  allocp[6] = NILATOM;
  allocp[7] = x1;
  x2 = makecons(&allocp[6]);
  allocp[8] = x2;
  allocp[9] = x0;
  x3 = makecons(&allocp[8]);
  allocp[10] = makesym(functor__3A_2);
  allocp[11] = a2;
  allocp[12] = makesym(atom_int);
  x4 = makefunctor(&allocp[10]);
  allocp[13] = makesym(functor__24_3);
  allocp[14] = x3;
  allocp[15] = x4;
  allocp[16] = a3;
  x5 = makefunctor(&allocp[13]);
  allocp[17] = NILATOM;
  allocp[18] = x5;
  x6 = makecons(&allocp[17]);
  allocp[19] = makesym(functor_yes_1);
  allocp[20] = x6;
  x7 = makefunctor(&allocp[19]);
  allocp += 21;
  unify_value(a4, x7);
  proceed();
 int__binary_5_ext_interrupt:
  reasonp = 0l;
 int__binary_5_interrupt:
  toppred = &predicate_klic__comp__gb_xint__binary_5;
  goto interrupt_5;
 }

 int__unary_4_top: {
  q x0, x1, x2, x3, x4, x5;
  a2 = qp->args[2];
  a3 = qp->args[3];
  qp = qp->next;
 int__unary_4_clear_reason:
  reasonp = reasons;
 int__unary_4_0:
  allocp[0] = makesym(functor__3A_2);
  allocp[1] = a0;
  allocp[2] = makesym(atom_int);
  x0 = makefunctor(&allocp[0]);
  allocp[3] = NILATOM;
  allocp[4] = x0;
  x1 = makecons(&allocp[3]);
  allocp[5] = makesym(functor__3A_2);
  allocp[6] = a1;
  allocp[7] = makesym(atom_int);
  x2 = makefunctor(&allocp[5]);
  allocp[8] = makesym(functor__24_3);
  allocp[9] = x1;
  allocp[10] = x2;
  allocp[11] = a2;
  x3 = makefunctor(&allocp[8]);
  allocp[12] = NILATOM;
  allocp[13] = x3;
  x4 = makecons(&allocp[12]);
  allocp[14] = makesym(functor_yes_1);
  allocp[15] = x4;
  x5 = makefunctor(&allocp[14]);
  allocp += 16;
  unify_value(a3, x5);
  proceed();
 int__unary_4_ext_interrupt:
  reasonp = 0l;
 int__unary_4_interrupt:
  toppred = &predicate_klic__comp__gb_xint__unary_4;
  goto interrupt_4;
 }

 builtin_4_top: {
  q x0, x1, x2;
  a2 = qp->args[2];
  a3 = qp->args[3];
  qp = qp->next;
 builtin_4_clear_reason:
  reasonp = reasons;
 builtin_4_0:
  allocp[0] = makesym(functor__24_3);
  allocp[1] = a0;
  allocp[2] = a1;
  allocp[3] = a2;
  x0 = makefunctor(&allocp[0]);
  allocp[4] = NILATOM;
  allocp[5] = x0;
  x1 = makecons(&allocp[4]);
  allocp[6] = makesym(functor_yes_1);
  allocp[7] = x1;
  x2 = makefunctor(&allocp[6]);
  allocp += 8;
  unify_value(a3, x2);
  proceed();
 builtin_4_ext_interrupt:
  reasonp = 0l;
 builtin_4_interrupt:
  toppred = &predicate_klic__comp__gb_xbuiltin_4;
  goto interrupt_4;
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
  loop_within_module(module_klic__comp__gb);
}
