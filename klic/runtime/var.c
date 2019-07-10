/* Compiled by KLIC compiler version 3.003 (Mon Mar 29 19:42:59 JST 1999) */
#include <klic/klichdr.h>
#include "atom.h"
#include "funct.h"

module module_variable();
Const struct predicate predicate_variable_xwrap_2 =
   { module_variable, 0, 2 };
Const struct predicate predicate_variable_xwrap_2F2_240_3 =
   { module_variable, 1, 3 };
Const struct predicate predicate_variable_xwrap__bound_2 =
   { module_variable, 2, 2 };
Const struct predicate predicate_variable_xwrap__vec__elems_4 =
   { module_variable, 3, 4 };
Const struct predicate predicate_variable_xwrap__funct__elems_6 =
   { module_variable, 4, 6 };
Const struct predicate predicate_variable_xunwrap_2 =
   { module_variable, 5, 2 };
Const struct predicate predicate_variable_xunwrap_3 =
   { module_variable, 6, 3 };
Const struct predicate predicate_variable_xunwrap_4 =
   { module_variable, 7, 4 };
Const struct predicate predicate_variable_xunwrap__functor_8 =
   { module_variable, 8, 8 };
Const struct predicate predicate_variable_xunwrap__vector_6 =
   { module_variable, 9, 6 };
Const struct predicate predicate_variable_xget__variable_4 =
   { module_variable, 10, 4 };
Const struct predicate predicate_variable_xget__variable2_4 =
   { module_variable, 11, 4 };
Const struct predicate predicate_variable_xunbound_2 =
   { module_variable, 12, 2 };
extern Const struct predicate predicate_variable_xunbound_2;
extern Const struct predicate predicate_integer__arithmetics_xadd_3;
extern Const struct predicate predicate_functor__table_x_3D_2E_2E_2;
declare_method_table_of(vector);

module module_variable(glbl, qp, allocp, toppred)
  struct global_variables *glbl;
  struct goalrec *qp;
  register q *allocp;
  Const struct predicate *toppred;
{
  static Const unsigned char strconst_body_0[4] =
    "_";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_0 =
      declare_string_constant_8(strconst_body_0,1);
  static Const unsigned char strconst_body_1[4] =
    "_";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_1 =
      declare_string_constant_8(strconst_body_1,1);
  static Const q funct_const_2[] = {
    makesym(functor_variable_1),
    makefunctor(&string_const_1),
  };
  q a0, a1, a2, a3, a4, a5, a6, a7;

  q *reasonp;
 module_top:
  a0 = qp->args[0];
  a1 = qp->args[1];
  switch_on_pred() {
    case_pred(0, wrap_2_top);
    case_pred(1, wrap_2F2_240_3_top);
    case_pred(2, wrap__bound_2_top);
    case_pred(3, wrap__vec__elems_4_top);
    case_pred(4, wrap__funct__elems_6_top);
    case_pred(5, unwrap_2_top);
    case_pred(6, unwrap_3_top);
    case_pred(7, unwrap_4_top);
    case_pred(8, unwrap__functor_8_top);
    case_pred(9, unwrap__vector_6_top);
    case_pred(10, get__variable_4_top);
    case_pred(11, get__variable2_4_top);
    last_case_pred(12, unbound_2_top);
  }

 wrap_2_top: {
  q x0;
  qp = qp->next;
 wrap_2_clear_reason:
  reasonp = reasons;
 wrap_2_0:
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_variable_xwrap_2F2_240_3);
  allocp[2] = x0 = makeref(&allocp[2]);
  allocp[3] = a0;
  allocp[4] = a1;
  allocp[5] = (q)(struct goalrec*)&allocp[0];
  allocp[6] = (q)(&predicate_variable_xunbound_2);
  allocp[7] = a0;
  allocp[8] = x0;
  qp = (struct goalrec*)&allocp[5];
  allocp += 9;
  proceed();
 wrap_2_ext_interrupt:
  reasonp = 0l;
 wrap_2_interrupt:
  toppred = &predicate_variable_xwrap_2;
  goto interrupt_2;
 }

 wrap_2F2_240_3_top: {
  q x0, x1, x2, x3, x4, x5, x6, x7;
  a2 = qp->args[2];
  qp = qp->next;
 wrap_2F2_240_3_clear_reason:
  reasonp = reasons;
 wrap_2F2_240_3_0:
 wrap_2F2_240_3_1:
  switch (ptagof(a0)) {
 case FUNCTOR:
 wrap_2F2_240_3_2:
  if (!isgobj(a0)) goto wrap_2F2_240_3_interrupt;
  if (!isclass(a0,vector)) goto wrap_2F2_240_3_interrupt;
  gblt_size_of_vector(a0,x0,wrap_2F2_240_3_interrupt);
 wrap_2F2_240_3_3:
  switch ((unsigned long)x0) {
 case (unsigned long) makeint(1L):
  x2 = makeint(0L);
  gblt_element_of_vector(a0,x2,x1,wrap_2F2_240_3_interrupt);
  a0 = a1;
  a1 = a2;
  execute(wrap__bound_2_0);
  goto wrap__bound_2_ext_interrupt;
 case (unsigned long) makeint(3L):
  x2 = makeint(0L);
  gblt_element_of_vector(a0,x2,x1,wrap_2F2_240_3_interrupt);
  x4 = makeint(1L);
  gblt_element_of_vector(a0,x4,x3,wrap_2F2_240_3_interrupt);
  x6 = makeint(2L);
  gblt_element_of_vector(a0,x6,x5,wrap_2F2_240_3_interrupt);
  x7 = makefunctor(funct_const_2);
  unify_value(a2, x7);
  proceed();
  };
  goto wrap_2F2_240_3_interrupt;
 case VARREF:
  deref_and_jump(a0,wrap_2F2_240_3_1);
  *reasonp++ =  a0;
  goto wrap_2F2_240_3_interrupt;
  };
  goto wrap_2F2_240_3_interrupt;
 wrap_2F2_240_3_ext_interrupt:
  reasonp = 0l;
 wrap_2F2_240_3_interrupt:
  goto interrupt_3;
 }

 wrap__bound_2_top: {
  q x0, x1, x2, x3, x4, x5, x6;
  qp = qp->next;
 wrap__bound_2_clear_reason:
  reasonp = reasons;
 wrap__bound_2_0:
 wrap__bound_2_1:
  switch (ptagof(a0)) {
 case CONS:
  allocp[0] = x1 = makeref(&allocp[0]);
  allocp[1] = x2 = makeref(&allocp[1]);
  x0 = makecons(&allocp[0]);
  allocp[2] = makesym(functor_list_1);
  allocp[3] = x0;
  x3 = makefunctor(&allocp[2]);
  allocp += 4;
  unify_value(a1, x3);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_variable_xwrap_2);
  x4 = cdr_of(a0);
  allocp[2] = x4;
  allocp[3] = x1;
  a0 = car_of(a0);
  a1 = x2;
  qp = (struct goalrec*)&allocp[0];
  allocp += 4;
  execute(wrap_2_0);
  goto wrap_2_ext_interrupt;
 case ATOMIC:
  if (isint(a0)) goto wrap__bound_2_3;
 wrap__bound_2_2:
  gblt_atom(a0,wrap__bound_2_7);
  allocp[0] = makesym(functor_atom_1);
  allocp[1] = a0;
  x0 = makefunctor(&allocp[0]);
  allocp += 2;
  unify_value(a1, x0);
  proceed();
 wrap__bound_2_3:
  gblt_integer(a0,wrap__bound_2_7);
  allocp[0] = makesym(functor_integer_1);
  allocp[1] = a0;
  x0 = makefunctor(&allocp[0]);
  allocp += 2;
  unify_value(a1, x0);
  proceed();
 case FUNCTOR:
 wrap__bound_2_4:
  if (!isgobj(a0)) goto wrap__bound_2_7;
  if (!isclass(a0,vector)) goto wrap__bound_2_5;
  gblt_size_of_vector(a0,x0,wrap__bound_2_5);
  allocp[0] = makesym(functor_vector_1);
  allocp[1] = x2 = makeref(&allocp[1]);
  x1 = makefunctor(&allocp[0]);
  allocp += 2;
  unify_value(a1, x1);
  a1 = x0;
  a2 = a0;
  a3 = x2;
  a0 = makeint(0L);
  execute(wrap__vec__elems_4_0);
  goto wrap__vec__elems_4_ext_interrupt;
 wrap__bound_2_5:
  guard_generic(a0, functor_float_0, 0, wrap__bound_2_6);
  allocp[0] = makesym(functor_floating__point_1);
  allocp[1] = a0;
  x3 = makefunctor(&allocp[0]);
  allocp += 2;
  unify_value(a1, x3);
  proceed();
 wrap__bound_2_6:
  gblt_is_string(a0,wrap__bound_2_7);
  gblt_size_of_string(a0,x3,wrap__bound_2_7);
  gblt_element_size_of_string(a0,x4,wrap__bound_2_7);
  allocp[0] = makesym(functor_string_1);
  allocp[1] = a0;
  x5 = makefunctor(&allocp[0]);
  allocp += 2;
  unify_value(a1, x5);
  proceed();
 case VARREF:
  deref_and_jump(a0,wrap__bound_2_1);
  *reasonp++ =  a0;
  goto wrap__bound_2_8;
  };
  goto wrap__bound_2_7;
 wrap__bound_2_7:
  gblt_pfunctor(a0,x0,wrap__bound_2_8);
  gblt_arity(a0,x1,wrap__bound_2_8);
  x2 = makeint(0L);
  gblt_greater(x1,x2,wrap__bound_2_8);
  allocp[0] = makesym(functor_functor_1);
  allocp[1] = x4 = makeref(&allocp[1]);
  x3 = makefunctor(&allocp[0]);
  allocp += 2;
  unify_value(a1, x3);
  allocp[0] = x6 = makeref(&allocp[0]);
  allocp[1] = x0;
  x5 = makecons(&allocp[0]);
  a1 = x1;
  a2 = a0;
  a3 = x5;
  a4 = x6;
  a5 = x4;
  a0 = makeint(0L);
  allocp += 2;
  execute(wrap__funct__elems_6_0);
  goto wrap__funct__elems_6_ext_interrupt;
 wrap__bound_2_8:
  otherwise(wrap__bound_2_interrupt);
  allocp[0] = makesym(functor_unknown_1);
  allocp[1] = a0;
  x0 = makefunctor(&allocp[0]);
  allocp += 2;
  unify_value(a1, x0);
  proceed();
 wrap__bound_2_ext_interrupt:
  reasonp = 0l;
 wrap__bound_2_interrupt:
  toppred = &predicate_variable_xwrap__bound_2;
  goto interrupt_2;
 }

 wrap__vec__elems_4_top: {
  q x0, x1, x2, x3;
  a2 = qp->args[2];
  a3 = qp->args[3];
  qp = qp->next;
 wrap__vec__elems_4_clear_reason:
  reasonp = reasons;
 wrap__vec__elems_4_0:
 wrap__vec__elems_4_1:
  if (!isint(a0)) goto wrap__vec__elems_4_5;
 wrap__vec__elems_4_2:
  if (!isint(a1)) goto wrap__vec__elems_4_4;
  gblt_eq(a0,a1,wrap__vec__elems_4_3);
  unify(a3, a2);
  proceed();
 wrap__vec__elems_4_3:
  gblt_less(a0,a1,wrap__vec__elems_4_interrupt);
  generic_arg[0] = a0;
  allocp[0] = x0 = makeref(&allocp[0]);
  generic_arg[1] = x0;
  allocp[1] = x1 = makeref(&allocp[1]);
  generic_arg[2] = x1;
  allocp[2] = x2 = makeref(&allocp[2]);
  generic_arg[3] = x2;
  call_generic(a2, functor_set__element_4, 3);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_variable_xwrap__vec__elems_4);
  allocp[2] = x3 = makeref(&allocp[2]);
  allocp[3] = a1;
  allocp[4] = x2;
  allocp[5] = a3;
  allocp[6] = (q)(struct goalrec*)&allocp[0];
  allocp[7] = (q)(&predicate_integer__arithmetics_xadd_3);
  allocp[8] = a0;
  allocp[9] = makeint(1L);
  allocp[10] = x3;
  a0 = x0;
  a1 = x1;
  qp = (struct goalrec*)&allocp[6];
  allocp += 11;
  execute(wrap_2_0);
  goto wrap_2_ext_interrupt;
 wrap__vec__elems_4_4:
  if (!isref(a1)) goto wrap__vec__elems_4_interrupt;
  deref_and_jump(a1,wrap__vec__elems_4_2);
  *reasonp++ =  a1;
  goto wrap__vec__elems_4_interrupt;
 wrap__vec__elems_4_5:
  if (!isref(a0)) goto wrap__vec__elems_4_interrupt;
  deref_and_jump(a0,wrap__vec__elems_4_1);
  *reasonp++ =  a0;
  goto wrap__vec__elems_4_interrupt;
 wrap__vec__elems_4_ext_interrupt:
  reasonp = 0l;
 wrap__vec__elems_4_interrupt:
  toppred = &predicate_variable_xwrap__vec__elems_4;
  goto interrupt_4;
 }

 wrap__funct__elems_6_top: {
  q x0, x1, x2, x3, x4, x5;
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  qp = qp->next;
 wrap__funct__elems_6_clear_reason:
  reasonp = reasons;
 wrap__funct__elems_6_0:
 wrap__funct__elems_6_1:
  switch (ptagof(a0)) {
 case ATOMIC:
  if (!isint(a0)) goto wrap__funct__elems_6_5;
 wrap__funct__elems_6_2:
  x1 = makeint(1L);
  gblt_add(a0,x1,x0,wrap__funct__elems_6_5);
 wrap__funct__elems_6_3:
  if (isref(a2)) goto wrap__funct__elems_6_4;
  gblt_arg(x0,a2,x2,wrap__funct__elems_6_5);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_variable_xwrap__funct__elems_6);
  allocp[2] = x0;
  allocp[3] = a1;
  allocp[4] = a2;
  allocp[5] = a3;
  allocp[6] = x3 = makeref(&allocp[6]);
  allocp[7] = a5;
  allocp[8] = x3;
  allocp[9] = x5 = makeref(&allocp[9]);
  x4 = makecons(&allocp[8]);
  qp = (struct goalrec*)&allocp[0];
  allocp += 10;
  unify_value(a4, x4);
  a0 = x2;
  a1 = x5;
  execute(wrap_2_0);
  goto wrap_2_ext_interrupt;
 wrap__funct__elems_6_4:
  deref_and_jump(a2,wrap__funct__elems_6_3);
  *reasonp++ =  a2;
  goto wrap__funct__elems_6_5;
 case VARREF:
  deref_and_jump(a0,wrap__funct__elems_6_1);
  *reasonp++ =  a0;
  goto wrap__funct__elems_6_interrupt;
  };
  goto wrap__funct__elems_6_5;
 wrap__funct__elems_6_5:
 wrap__funct__elems_6_6:
  if (isref(a0)) goto wrap__funct__elems_6_7;
  if_not_equal(a0, a1, wrap__funct__elems_6_interrupt);
  x0 = NILATOM;
  unify_value(a4, x0);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_functor__table_x_3D_2E_2E_2);
  allocp[2] = a5;
  allocp[3] = a3;
  qp = (struct goalrec*)&allocp[0];
  allocp += 4;
  proceed();
 wrap__funct__elems_6_7:
  deref_and_jump(a0,wrap__funct__elems_6_6);
  *reasonp++ =  a0;
  goto wrap__funct__elems_6_interrupt;
 wrap__funct__elems_6_ext_interrupt:
  reasonp = 0l;
 wrap__funct__elems_6_interrupt:
  toppred = &predicate_variable_xwrap__funct__elems_6;
  goto interrupt_6;
 }

 unwrap_2_top: {
  q x0;
  qp = qp->next;
 unwrap_2_clear_reason:
  reasonp = reasons;
 unwrap_2_0:
  a2 = NILATOM;
  allocp[0] = x0 = makeref(&allocp[0]);
  a3 = x0;
  allocp += 1;
  execute(unwrap_4_0);
  goto unwrap_4_ext_interrupt;
 unwrap_2_ext_interrupt:
  reasonp = 0l;
 unwrap_2_interrupt:
  goto interrupt_2;
 }

 unwrap_3_top: {
  a2 = qp->args[2];
  qp = qp->next;
 unwrap_3_clear_reason:
  reasonp = reasons;
 unwrap_3_0:
  a3 = a2;
  a2 = NILATOM;
  execute(unwrap_4_0);
  goto unwrap_4_ext_interrupt;
 unwrap_3_ext_interrupt:
  reasonp = 0l;
 unwrap_3_interrupt:
  goto interrupt_3;
 }

 unwrap_4_top: {
  q x0, x1, x2, x3, x4, x5;
  a2 = qp->args[2];
  a3 = qp->args[3];
  qp = qp->next;
 unwrap_4_clear_reason:
  reasonp = reasons;
 unwrap_4_0:
 unwrap_4_1:
  switch (ptagof(a0)) {
 case FUNCTOR:
  switch (symval(functor_of(a0))) {
 case functor_variable_1:
  a0 = arg(a0, 0);
  execute(get__variable_4_0);
  goto get__variable_4_ext_interrupt;
 case functor_atom_1:
  unify(a3, a2);
  x0 = arg(a0, 0);
  unify(a1, x0);
  proceed();
 case functor_integer_1:
  unify(a3, a2);
  x0 = arg(a0, 0);
  unify(a1, x0);
  proceed();
 case functor_floating__point_1:
  unify(a3, a2);
  x0 = arg(a0, 0);
  unify(a1, x0);
  proceed();
 case functor_list_1:
  x0 = arg(a0, 0);
 unwrap_4_2:
  switch (ptagof(x0)) {
 case CONS:
  allocp[0] = x2 = makeref(&allocp[0]);
  allocp[1] = x3 = makeref(&allocp[1]);
  x1 = makecons(&allocp[0]);
  allocp += 2;
  unify_value(a1, x1);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_variable_xunwrap_4);
  x4 = cdr_of(x0);
  allocp[2] = x4;
  allocp[3] = x2;
  allocp[4] = x5 = makeref(&allocp[4]);
  allocp[5] = a3;
  a0 = car_of(x0);
  a1 = x3;
  a3 = x5;
  qp = (struct goalrec*)&allocp[0];
  allocp += 6;
  execute(unwrap_4_0);
  goto unwrap_4_ext_interrupt;
 case VARREF:
  deref_and_jump(x0,unwrap_4_2);
  *reasonp++ =  x0;
  goto unwrap_4_interrupt;
  };
  goto unwrap_4_interrupt;
 case functor_functor_1:
  x0 = arg(a0, 0);
 unwrap_4_3:
  if (isref(x0)) goto unwrap_4_4;
  gblt_pfunctor(x0,x1,unwrap_4_interrupt);
  gblt_arity(x0,x2,unwrap_4_interrupt);
  allocp[0] = x4 = makeref(&allocp[0]);
  allocp[1] = x1;
  x3 = makecons(&allocp[0]);
  a0 = makeint(0L);
  a4 = x4;
  a5 = a1;
  a6 = a2;
  a7 = a3;
  a3 = x3;
  a2 = x0;
  a1 = x2;
  allocp += 2;
  execute(unwrap__functor_8_0);
  goto unwrap__functor_8_ext_interrupt;
 unwrap_4_4:
  deref_and_jump(x0,unwrap_4_3);
  *reasonp++ =  x0;
  goto unwrap_4_interrupt;
 case functor_vector_1:
  x0 = arg(a0, 0);
 unwrap_4_5:
  switch (ptagof(x0)) {
 case FUNCTOR:
 unwrap_4_6:
  if (!isgobj(x0)) goto unwrap_4_interrupt;
  if (!isclass(x0,vector)) goto unwrap_4_interrupt;
  gblt_size_of_vector(x0,x1,unwrap_4_interrupt);
  a0 = makeint(0L);
  x2 = a1;
  a4 = a2;
  a5 = a3;
  a3 = x2;
  a2 = x0;
  a1 = x1;
  execute(unwrap__vector_6_0);
  goto unwrap__vector_6_ext_interrupt;
 case VARREF:
  deref_and_jump(x0,unwrap_4_5);
  *reasonp++ =  x0;
  goto unwrap_4_interrupt;
  };
  goto unwrap_4_interrupt;
 case functor_string_1:
  unify(a3, a2);
  x0 = arg(a0, 0);
  unify(a1, x0);
  proceed();
 case functor_unknown_1:
  unify(a3, a2);
  x0 = arg(a0, 0);
  unify(a1, x0);
  proceed();
  };
  goto unwrap_4_interrupt;
 case VARREF:
  deref_and_jump(a0,unwrap_4_1);
  *reasonp++ =  a0;
  goto unwrap_4_interrupt;
  };
  goto unwrap_4_interrupt;
 unwrap_4_ext_interrupt:
  reasonp = 0l;
 unwrap_4_interrupt:
  toppred = &predicate_variable_xunwrap_4;
  goto interrupt_4;
 }

 unwrap__functor_8_top: {
  q x0, x1, x2, x3, x4, x5, x6;
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  a6 = qp->args[6];
  a7 = qp->args[7];
  qp = qp->next;
 unwrap__functor_8_clear_reason:
  reasonp = reasons;
 unwrap__functor_8_0:
 unwrap__functor_8_1:
  switch (ptagof(a0)) {
 case ATOMIC:
  if (!isint(a0)) goto unwrap__functor_8_7;
 unwrap__functor_8_2:
 unwrap__functor_8_3:
  if (!isint(a1)) goto unwrap__functor_8_6;
  gblt_less(a0,a1,unwrap__functor_8_7);
  x1 = makeint(1L);
  gblt_add(a0,x1,x0,unwrap__functor_8_7);
 unwrap__functor_8_4:
  if (isref(a2)) goto unwrap__functor_8_5;
  gblt_arg(x0,a2,x2,unwrap__functor_8_7);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_variable_xunwrap__functor_8);
  allocp[2] = x0;
  allocp[3] = a1;
  allocp[4] = a2;
  allocp[5] = a3;
  allocp[6] = x3 = makeref(&allocp[6]);
  allocp[7] = a5;
  allocp[8] = x4 = makeref(&allocp[8]);
  allocp[9] = a7;
  allocp[10] = x3;
  allocp[11] = x6 = makeref(&allocp[11]);
  x5 = makecons(&allocp[10]);
  qp = (struct goalrec*)&allocp[0];
  allocp += 12;
  unify_value(a4, x5);
  a0 = x2;
  a1 = x6;
  a2 = a6;
  a3 = x4;
  execute(unwrap_4_0);
  goto unwrap_4_ext_interrupt;
 unwrap__functor_8_5:
  deref_and_jump(a2,unwrap__functor_8_4);
  *reasonp++ =  a2;
  goto unwrap__functor_8_7;
 unwrap__functor_8_6:
  if (!isref(a1)) goto unwrap__functor_8_7;
  deref_and_jump(a1,unwrap__functor_8_3);
  *reasonp++ =  a1;
  goto unwrap__functor_8_7;
 case VARREF:
  deref_and_jump(a0,unwrap__functor_8_1);
  *reasonp++ =  a0;
  goto unwrap__functor_8_interrupt;
  };
  goto unwrap__functor_8_7;
 unwrap__functor_8_7:
 unwrap__functor_8_8:
  if (isref(a0)) goto unwrap__functor_8_9;
  if_not_equal(a0, a1, unwrap__functor_8_interrupt);
  x0 = NILATOM;
  unify_value(a4, x0);
  unify(a7, a6);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_functor__table_x_3D_2E_2E_2);
  allocp[2] = a5;
  allocp[3] = a3;
  qp = (struct goalrec*)&allocp[0];
  allocp += 4;
  proceed();
 unwrap__functor_8_9:
  deref_and_jump(a0,unwrap__functor_8_8);
  *reasonp++ =  a0;
  goto unwrap__functor_8_interrupt;
 unwrap__functor_8_ext_interrupt:
  reasonp = 0l;
 unwrap__functor_8_interrupt:
  toppred = &predicate_variable_xunwrap__functor_8;
  goto interrupt_8;
 }

 unwrap__vector_6_top: {
  q x0, x1, x2, x3, x4, x5;
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  qp = qp->next;
 unwrap__vector_6_clear_reason:
  reasonp = reasons;
 unwrap__vector_6_0:
 unwrap__vector_6_1:
  switch (ptagof(a0)) {
 case ATOMIC:
  if (!isint(a0)) goto unwrap__vector_6_5;
 unwrap__vector_6_2:
 unwrap__vector_6_3:
  if (!isint(a1)) goto unwrap__vector_6_4;
  gblt_not_eq(a0,a1,unwrap__vector_6_5);
  x0 = makeint(1L);
  bblt_add_no_check(a0,x0,x1);
  generic_arg[0] = a0;
  allocp[0] = x2 = makeref(&allocp[0]);
  generic_arg[1] = x2;
  allocp[1] = x3 = makeref(&allocp[1]);
  generic_arg[2] = x3;
  allocp[2] = x4 = makeref(&allocp[2]);
  generic_arg[3] = x4;
  call_generic(a2, functor_set__element_4, 3);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_variable_xunwrap__vector_6);
  allocp[2] = x1;
  allocp[3] = a1;
  allocp[4] = x4;
  allocp[5] = a3;
  allocp[6] = x5 = makeref(&allocp[6]);
  allocp[7] = a5;
  a0 = x2;
  a1 = x3;
  a2 = a4;
  a3 = x5;
  qp = (struct goalrec*)&allocp[0];
  allocp += 8;
  execute(unwrap_4_0);
  goto unwrap_4_ext_interrupt;
 unwrap__vector_6_4:
  if (!isref(a1)) goto unwrap__vector_6_5;
  deref_and_jump(a1,unwrap__vector_6_3);
  *reasonp++ =  a1;
  goto unwrap__vector_6_5;
 case VARREF:
  deref_and_jump(a0,unwrap__vector_6_1);
  *reasonp++ =  a0;
  goto unwrap__vector_6_interrupt;
  };
  goto unwrap__vector_6_5;
 unwrap__vector_6_5:
 unwrap__vector_6_6:
  if (isref(a0)) goto unwrap__vector_6_7;
  if_not_equal(a0, a1, unwrap__vector_6_interrupt);
  unify2(a3, a2, a5, a4);
  proceed();
 unwrap__vector_6_7:
  deref_and_jump(a0,unwrap__vector_6_6);
  *reasonp++ =  a0;
  goto unwrap__vector_6_interrupt;
 unwrap__vector_6_ext_interrupt:
  reasonp = 0l;
 unwrap__vector_6_interrupt:
  toppred = &predicate_variable_xunwrap__vector_6;
  goto interrupt_6;
 }

 get__variable_4_top: {
  q x0;
  a2 = qp->args[2];
  a3 = qp->args[3];
  qp = qp->next;
 get__variable_4_clear_reason:
  reasonp = reasons;
 get__variable_4_0:
 get__variable_4_1:
  if (isref(a0)) goto get__variable_4_4;
 get__variable_4_2:
  if (isref(a0)) goto get__variable_4_3;
  x0 = makefunctor(&string_const_0);
  if_not_equal(a0, x0, get__variable_4_5);
  unify(a3, a2);
  proceed();
 get__variable_4_3:
  deref_and_jump(a0,get__variable_4_2);
  *reasonp++ =  a0;
  goto get__variable_4_5;
 get__variable_4_4:
  deref_and_jump(a0,get__variable_4_1);
  *reasonp++ =  a0;
  goto get__variable_4_5;
 get__variable_4_5:
  otherwise(get__variable_4_interrupt);
  execute(get__variable2_4_0);
  goto get__variable2_4_ext_interrupt;
 get__variable_4_ext_interrupt:
  reasonp = 0l;
 get__variable_4_interrupt:
  toppred = &predicate_variable_xget__variable_4;
  goto interrupt_4;
 }

 get__variable2_4_top: {
  q x0, x1, x2, x3, x4, x5, x6;
  a2 = qp->args[2];
  a3 = qp->args[3];
  qp = qp->next;
 get__variable2_4_clear_reason:
  reasonp = reasons;
 get__variable2_4_0:
 get__variable2_4_1:
  switch (ptagof(a2)) {
 case CONS:
  x0 = car_of(a2);
 get__variable2_4_2:
  switch (ptagof(x0)) {
 case FUNCTOR:
  if (functor_of(x0) != makesym(functor_var_2)) goto get__variable2_4_7;
  x1 = arg(x0, 0);
 get__variable2_4_3:
  if (isref(x1)) goto get__variable2_4_6;
 get__variable2_4_4:
  if (isref(x1)) goto get__variable2_4_5;
  if_not_equal(x1, a0, get__variable2_4_7);
  allocp[0] = makesym(functor_var_2);
  allocp[1] = a0;
  x3 = arg(x0, 1);
  allocp[2] = x3;
  x2 = makefunctor(&allocp[0]);
  x5 = cdr_of(a2);
  allocp[3] = x5;
  allocp[4] = x2;
  x4 = makecons(&allocp[3]);
  allocp += 5;
  unify_value(a3, x4);
  x6 = arg(x0, 1);
  unify(a1, x6);
  proceed();
 get__variable2_4_5:
  deref_and_jump(x1,get__variable2_4_4);
  *reasonp++ =  x1;
  goto get__variable2_4_7;
 get__variable2_4_6:
  deref_and_jump(x1,get__variable2_4_3);
  *reasonp++ =  x1;
  goto get__variable2_4_7;
 case VARREF:
  deref_and_jump(x0,get__variable2_4_2);
  *reasonp++ =  x0;
  goto get__variable2_4_7;
  };
  goto get__variable2_4_7;
 case ATOMIC:
  if (a2 != NILATOM) goto get__variable2_4_7;
  allocp[0] = makesym(functor_var_2);
  allocp[1] = a0;
  allocp[2] = a1;
  x0 = makefunctor(&allocp[0]);
  allocp[3] = NILATOM;
  allocp[4] = x0;
  x1 = makecons(&allocp[3]);
  allocp += 5;
  unify_value(a3, x1);
  proceed();
 case VARREF:
  deref_and_jump(a2,get__variable2_4_1);
  *reasonp++ =  a2;
  goto get__variable2_4_7;
  };
  goto get__variable2_4_7;
 get__variable2_4_7:
  otherwise(get__variable2_4_interrupt);
 get__variable2_4_8:
  switch (ptagof(a2)) {
 case CONS:
  allocp[0] = x1 = makeref(&allocp[0]);
  x2 = car_of(a2);
  allocp[1] = x2;
  x0 = makecons(&allocp[0]);
  allocp += 2;
  unify_value(a3, x0);
  a2 = cdr_of(a2);
  a3 = x1;
  execute(get__variable2_4_0);
  goto get__variable2_4_ext_interrupt;
 case VARREF:
  deref_and_jump(a2,get__variable2_4_8);
  *reasonp++ =  a2;
  goto get__variable2_4_interrupt;
  };
  goto get__variable2_4_interrupt;
 get__variable2_4_ext_interrupt:
  reasonp = 0l;
 get__variable2_4_interrupt:
  toppred = &predicate_variable_xget__variable2_4;
  goto interrupt_4;
 }

 unbound_2_top: {
  q x0, x1;
  qp = qp->next;
 unbound_2_clear_reason:
  reasonp = reasons;
 unbound_2_0:
  
{
  q tmp = a0;
  while (isref(tmp)) {
    q tmp1 = derefone(tmp);
    if (tmp==tmp1 || (isref(tmp1) && tmp==derefone(tmp1))) {
      goto really_unbound;
    }
    tmp = tmp1;
  }
  goto unbound_2_1;
 really_unbound:
  x0 = makeint((q*)tmp-new_space_top);
}
  allocp[0] = makeint(0L);
  allocp[1] = x0;
  allocp[2] = a0;
  x1 = create_vector(&allocp[0], 3, allocp);
  allocp += 3;
  unify_value(a1, x1);
  proceed();
 unbound_2_1:
  otherwise(unbound_2_interrupt);
  allocp[0] = a0;
  x0 = create_vector(&allocp[0], 1, allocp);
  allocp += 1;
  unify_value(a1, x0);
  proceed();
 unbound_2_ext_interrupt:
  reasonp = 0l;
 unbound_2_interrupt:
  goto interrupt_2;
 }
 interrupt_8:
  allocp[9] = a7;
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
  loop_within_module(module_variable);
}
