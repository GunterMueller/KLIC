/* Compiled by KLIC compiler version 3.003 (Mon Mar 29 19:42:59 JST 1999) */
#include <klic/klichdr.h>
#include "atom.h"
#include "funct.h"

module module_klic__comp__extern();
Const struct predicate predicate_klic__comp__extern_xmake__ext__table_3 =
   { module_klic__comp__extern, 0, 3 };
Const struct predicate predicate_klic__comp__extern_xext__struct_3 =
   { module_klic__comp__extern, 1, 3 };
Const struct predicate predicate_klic__comp__extern_xext__struct__funct_5 =
   { module_klic__comp__extern, 2, 5 };
Const struct predicate predicate_klic__comp__extern_xext__struct__vector_5 =
   { module_klic__comp__extern, 3, 5 };
declare_method_table_of(vector);

module module_klic__comp__extern(glbl, qp, allocp, toppred)
  struct global_variables *glbl;
  struct goalrec *qp;
  register q *allocp;
  Const struct predicate *toppred;
{
  static Const q funct_const_0[] = {
    makesym(functor_class_1),
    makesym(atom_vector),
  };
  static Const q funct_const_1[] = {
    makesym(functor_class_1),
    makesym(atom_float),
  };
  static Const q funct_const_2[] = {
    makesym(functor_class_1),
    makesym(atom_predicate),
  };
  static Const q funct_const_3[] = {
    makesym(functor_class_1),
    makesym(atom_byte__string),
  };
  q a0, a1, a2, a3, a4;

  q *reasonp;
 module_top:
  a0 = qp->args[0];
  a1 = qp->args[1];
  a2 = qp->args[2];
  switch_on_pred() {
    case_pred(0, make__ext__table_3_top);
    case_pred(1, ext__struct_3_top);
    case_pred(2, ext__struct__funct_5_top);
    last_case_pred(3, ext__struct__vector_5_top);
  }

 make__ext__table_3_top: {
  q x0, x1;
  qp = qp->next;
 make__ext__table_3_clear_reason:
  reasonp = reasons;
 make__ext__table_3_0:
 make__ext__table_3_1:
  switch (ptagof(a0)) {
 case CONS:
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__extern_xmake__ext__table_3);
  x0 = cdr_of(a0);
  allocp[2] = x0;
  allocp[3] = x1 = makeref(&allocp[3]);
  allocp[4] = a2;
  a0 = car_of(a0);
  a2 = x1;
  qp = (struct goalrec*)&allocp[0];
  allocp += 5;
  execute(ext__struct_3_0);
  goto ext__struct_3_ext_interrupt;
 case ATOMIC:
  if (a0 != NILATOM) goto make__ext__table_3_interrupt;
  unify(a2, a1);
  proceed();
 case VARREF:
  deref_and_jump(a0,make__ext__table_3_1);
  *reasonp++ =  a0;
  goto make__ext__table_3_interrupt;
  };
  goto make__ext__table_3_interrupt;
 make__ext__table_3_ext_interrupt:
  reasonp = 0l;
 make__ext__table_3_interrupt:
  goto interrupt_3;
 }

 ext__struct_3_top: {
  q x0, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12;
  qp = qp->next;
 ext__struct_3_clear_reason:
  reasonp = reasons;
 ext__struct_3_0:
 ext__struct_3_1:
  switch (ptagof(a0)) {
 case FUNCTOR:
  switch (symval(functor_of(a0))) {
 case functor_funct_1:
  allocp[0] = makesym(functor_funct_1);
  x1 = arg(a0, 0);
  allocp[1] = x1;
  x0 = makefunctor(&allocp[0]);
  allocp[2] = a2;
  allocp[3] = x0;
  x2 = makecons(&allocp[2]);
  allocp += 4;
  unify_value(a1, x2);
  proceed();
 case functor_ext_3:
  allocp[0] = makesym(functor_module_1);
  x1 = arg(a0, 0);
  allocp[1] = x1;
  x0 = makefunctor(&allocp[0]);
  allocp[2] = a2;
  allocp[3] = x0;
  x2 = makecons(&allocp[2]);
  allocp += 4;
  unify_value(a1, x2);
  proceed();
 case functor_gnew_1:
  allocp[0] = makesym(functor_class_1);
  x1 = arg(a0, 0);
  allocp[1] = x1;
  x0 = makefunctor(&allocp[0]);
  allocp[2] = a2;
  allocp[3] = x0;
  x2 = makecons(&allocp[2]);
  allocp += 4;
  unify_value(a1, x2);
  proceed();
 case functor_const_2:
  a0 = arg(a0, 1);
  execute(ext__struct_3_0);
  goto ext__struct_3_ext_interrupt;
 case functor_atom_1:
  x0 = arg(a0, 0);
 ext__struct_3_2:
  switch (ptagof(x0)) {
 case ATOMIC:
  if (isint(x0)) goto ext__struct_3_3;
  switch (symval(x0)) {
 case symval(NILATOM):
  unify(a2, a1);
  proceed();
 case symval(PERIODATOM):
  unify(a2, a1);
  proceed();
  };
  goto ext__struct_3_3;
 case VARREF:
  deref_and_jump(x0,ext__struct_3_2);
  *reasonp++ =  x0;
  goto ext__struct_3_9;
  };
  goto ext__struct_3_3;
 ext__struct_3_3:
  x1 = NILATOM;
  gblt_diff(x0,x1,ext__struct_3_9);
  x2 = PERIODATOM;
  gblt_diff(x0,x2,ext__struct_3_9);
  allocp[0] = makesym(functor_atom_1);
  allocp[1] = x0;
  x3 = makefunctor(&allocp[0]);
  allocp[2] = a2;
  allocp[3] = x3;
  x4 = makecons(&allocp[2]);
  allocp += 4;
  unify_value(a1, x4);
  proceed();
 case functor_integer_1:
  unify(a2, a1);
  proceed();
 case functor_float_1:
  allocp[0] = a2;
  allocp[1] = makefunctor(funct_const_1);
  x0 = makecons(&allocp[0]);
  allocp += 2;
  unify_value(a1, x0);
  proceed();
 case functor_predicate_3:
  allocp[0] = x1 = makeref(&allocp[0]);
  allocp[1] = makefunctor(funct_const_2);
  x0 = makecons(&allocp[0]);
  allocp += 2;
  unify_value(a1, x0);
  allocp[0] = makesym(functor_module_1);
  x3 = arg(a0, 0);
  allocp[1] = x3;
  x2 = makefunctor(&allocp[0]);
  allocp[2] = x5 = makeref(&allocp[2]);
  allocp[3] = x2;
  x4 = makecons(&allocp[2]);
  allocp += 4;
  unify_value(x1, x4);
  allocp[0] = makesym(functor_atom_1);
  x7 = arg(a0, 0);
  allocp[1] = x7;
  x6 = makefunctor(&allocp[0]);
  allocp[2] = x9 = makeref(&allocp[2]);
  allocp[3] = x6;
  x8 = makecons(&allocp[2]);
  allocp += 4;
  unify_value(x5, x8);
  allocp[0] = makesym(functor_atom_1);
  x11 = arg(a0, 1);
  allocp[1] = x11;
  x10 = makefunctor(&allocp[0]);
  allocp[2] = a2;
  allocp[3] = x10;
  x12 = makecons(&allocp[2]);
  allocp += 4;
  unify_value(x9, x12);
  proceed();
 case functor_string_1:
  allocp[0] = a2;
  allocp[1] = makefunctor(funct_const_3);
  x0 = makecons(&allocp[0]);
  allocp += 2;
  unify_value(a1, x0);
  proceed();
 case functor_list_1:
  x0 = arg(a0, 0);
 ext__struct_3_4:
  switch (ptagof(x0)) {
 case CONS:
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__extern_xext__struct_3);
  x1 = cdr_of(x0);
  allocp[2] = x1;
  allocp[3] = x2 = makeref(&allocp[3]);
  allocp[4] = a2;
  a0 = car_of(x0);
  a2 = x2;
  qp = (struct goalrec*)&allocp[0];
  allocp += 5;
  execute(ext__struct_3_0);
  goto ext__struct_3_ext_interrupt;
 case VARREF:
  deref_and_jump(x0,ext__struct_3_4);
  *reasonp++ =  x0;
  goto ext__struct_3_9;
  };
  goto ext__struct_3_9;
 case functor_functor_1:
  x0 = arg(a0, 0);
 ext__struct_3_5:
  if (isref(x0)) goto ext__struct_3_6;
  gblt_pfunctor(x0,x1,ext__struct_3_9);
  gblt_arity(x0,x2,ext__struct_3_9);
  allocp[0] = makesym(functor__2F_2);
  allocp[1] = x1;
  allocp[2] = x2;
  x3 = makefunctor(&allocp[0]);
  allocp[3] = makesym(functor_funct_1);
  allocp[4] = x3;
  x4 = makefunctor(&allocp[3]);
  allocp[5] = x6 = makeref(&allocp[5]);
  allocp[6] = x4;
  x5 = makecons(&allocp[5]);
  allocp += 7;
  unify_value(a1, x5);
  a0 = makeint(0L);
  a1 = x2;
  a3 = x6;
  a4 = a2;
  a2 = x0;
  execute(ext__struct__funct_5_0);
  goto ext__struct__funct_5_ext_interrupt;
 ext__struct_3_6:
  deref_and_jump(x0,ext__struct_3_5);
  *reasonp++ =  x0;
  goto ext__struct_3_9;
 case functor_vector_1:
  x0 = arg(a0, 0);
 ext__struct_3_7:
  switch (ptagof(x0)) {
 case FUNCTOR:
 ext__struct_3_8:
  if (!isgobj(x0)) goto ext__struct_3_9;
  if (!isclass(x0,vector)) goto ext__struct_3_9;
  gblt_size_of_vector(x0,x1,ext__struct_3_9);
  allocp[0] = x3 = makeref(&allocp[0]);
  allocp[1] = makefunctor(funct_const_0);
  x2 = makecons(&allocp[0]);
  allocp += 2;
  unify_value(a1, x2);
  a0 = makeint(0L);
  a1 = x1;
  a3 = x3;
  a4 = a2;
  a2 = x0;
  execute(ext__struct__vector_5_0);
  goto ext__struct__vector_5_ext_interrupt;
 case VARREF:
  deref_and_jump(x0,ext__struct_3_7);
  *reasonp++ =  x0;
  goto ext__struct_3_9;
  };
  goto ext__struct_3_9;
  };
  goto ext__struct_3_9;
 case VARREF:
  deref_and_jump(a0,ext__struct_3_1);
  *reasonp++ =  a0;
  goto ext__struct_3_9;
  };
  goto ext__struct_3_9;
 ext__struct_3_9:
  otherwise(ext__struct_3_interrupt);
  unify(a2, a1);
  proceed();
 ext__struct_3_ext_interrupt:
  reasonp = 0l;
 ext__struct_3_interrupt:
  toppred = &predicate_klic__comp__extern_xext__struct_3;
  goto interrupt_3;
 }

 ext__struct__funct_5_top: {
  q x0, x1, x2, x3;
  a3 = qp->args[3];
  a4 = qp->args[4];
  qp = qp->next;
 ext__struct__funct_5_clear_reason:
  reasonp = reasons;
 ext__struct__funct_5_0:
 ext__struct__funct_5_1:
  if (!isint(a0)) goto ext__struct__funct_5_7;
 ext__struct__funct_5_2:
  if (!isint(a1)) goto ext__struct__funct_5_6;
  gblt_greater_or_eq(a0,a1,ext__struct__funct_5_3);
  unify(a4, a3);
  proceed();
 ext__struct__funct_5_3:
  gblt_less(a0,a1,ext__struct__funct_5_interrupt);
  x1 = makeint(1L);
  gblt_add(a0,x1,x0,ext__struct__funct_5_interrupt);
 ext__struct__funct_5_4:
  if (isref(a2)) goto ext__struct__funct_5_5;
  gblt_arg(x0,a2,x2,ext__struct__funct_5_interrupt);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__extern_xext__struct__funct_5);
  allocp[2] = x0;
  allocp[3] = a1;
  allocp[4] = a2;
  allocp[5] = x3 = makeref(&allocp[5]);
  allocp[6] = a4;
  a0 = x2;
  a1 = a3;
  a2 = x3;
  qp = (struct goalrec*)&allocp[0];
  allocp += 7;
  execute(ext__struct_3_0);
  goto ext__struct_3_ext_interrupt;
 ext__struct__funct_5_5:
  deref_and_jump(a2,ext__struct__funct_5_4);
  *reasonp++ =  a2;
  goto ext__struct__funct_5_interrupt;
 ext__struct__funct_5_6:
  if (!isref(a1)) goto ext__struct__funct_5_interrupt;
  deref_and_jump(a1,ext__struct__funct_5_2);
  *reasonp++ =  a1;
  goto ext__struct__funct_5_interrupt;
 ext__struct__funct_5_7:
  if (!isref(a0)) goto ext__struct__funct_5_interrupt;
  deref_and_jump(a0,ext__struct__funct_5_1);
  *reasonp++ =  a0;
  goto ext__struct__funct_5_interrupt;
 ext__struct__funct_5_ext_interrupt:
  reasonp = 0l;
 ext__struct__funct_5_interrupt:
  toppred = &predicate_klic__comp__extern_xext__struct__funct_5;
  goto interrupt_5;
 }

 ext__struct__vector_5_top: {
  q x0, x1, x2, x3;
  a3 = qp->args[3];
  a4 = qp->args[4];
  qp = qp->next;
 ext__struct__vector_5_clear_reason:
  reasonp = reasons;
 ext__struct__vector_5_0:
 ext__struct__vector_5_1:
  if (!isint(a0)) goto ext__struct__vector_5_7;
 ext__struct__vector_5_2:
  if (!isint(a1)) goto ext__struct__vector_5_6;
  gblt_greater_or_eq(a0,a1,ext__struct__vector_5_3);
  unify(a4, a3);
  proceed();
 ext__struct__vector_5_3:
  gblt_less(a0,a1,ext__struct__vector_5_interrupt);
  x1 = makeint(1L);
  gblt_add(a0,x1,x0,ext__struct__vector_5_interrupt);
 ext__struct__vector_5_4:
  switch (ptagof(a2)) {
 case FUNCTOR:
 ext__struct__vector_5_5:
  if (!isgobj(a2)) goto ext__struct__vector_5_interrupt;
  if (!isclass(a2,vector)) goto ext__struct__vector_5_interrupt;
  gblt_element_of_vector(a2,a0,x2,ext__struct__vector_5_interrupt);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__extern_xext__struct__vector_5);
  allocp[2] = x0;
  allocp[3] = a1;
  allocp[4] = a2;
  allocp[5] = x3 = makeref(&allocp[5]);
  allocp[6] = a4;
  a0 = x2;
  a1 = a3;
  a2 = x3;
  qp = (struct goalrec*)&allocp[0];
  allocp += 7;
  execute(ext__struct_3_0);
  goto ext__struct_3_ext_interrupt;
 case VARREF:
  deref_and_jump(a2,ext__struct__vector_5_4);
  *reasonp++ =  a2;
  goto ext__struct__vector_5_interrupt;
  };
  goto ext__struct__vector_5_interrupt;
 ext__struct__vector_5_6:
  if (!isref(a1)) goto ext__struct__vector_5_interrupt;
  deref_and_jump(a1,ext__struct__vector_5_2);
  *reasonp++ =  a1;
  goto ext__struct__vector_5_interrupt;
 ext__struct__vector_5_7:
  if (!isref(a0)) goto ext__struct__vector_5_interrupt;
  deref_and_jump(a0,ext__struct__vector_5_1);
  *reasonp++ =  a0;
  goto ext__struct__vector_5_interrupt;
 ext__struct__vector_5_ext_interrupt:
  reasonp = 0l;
 ext__struct__vector_5_interrupt:
  toppred = &predicate_klic__comp__extern_xext__struct__vector_5;
  goto interrupt_5;
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
  loop_within_module(module_klic__comp__extern);
}
