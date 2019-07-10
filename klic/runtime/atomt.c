/* Compiled by KLIC compiler version 3.003 (Mon Mar 29 19:42:59 JST 1999) */
#include <klic/klichdr.h>
#include "atom.h"
#include "funct.h"

#ifdef STRINGH
#include <string.h>
#else
#include <strings.h>
#endif

extern unsigned long enter_atom_string();


module module_atom__table();
Const struct predicate predicate_atom__table_xmake__atom_2 =
   { module_atom__table, 0, 2 };
Const struct predicate predicate_atom__table_xatom__number_2 =
   { module_atom__table, 1, 2 };
Const struct predicate predicate_atom__table_xget__atom__string_2 =
   { module_atom__table, 2, 2 };
Const struct predicate predicate_atom__table_xset__atom__string_5 =
   { module_atom__table, 3, 5 };
Const struct predicate predicate_atom__table_xintern_2 =
   { module_atom__table, 4, 2 };
Const struct predicate predicate_atom__table_xget__atom__name_2 =
   { module_atom__table, 5, 2 };
Const struct predicate predicate_atom__table_xcache__table_1 =
   { module_atom__table, 6, 1 };
extern q string_g_new();

module module_atom__table(glbl, qp, allocp, toppred)
  struct global_variables *glbl;
  struct goalrec *qp;
  register q *allocp;
  Const struct predicate *toppred;
{
  static Const unsigned char strconst_body_0[4] =
    ".";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_0 =
      declare_string_constant_8(strconst_body_0,1);
  static Const unsigned char strconst_body_1[4] =
    "[]";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_1 =
      declare_string_constant_8(strconst_body_1,2);
  q a0, a1, a2, a3, a4;

  q *reasonp;
 module_top:
  a0 = qp->args[0];
  switch_on_pred() {
    case_pred(0, make__atom_2_top);
    case_pred(1, atom__number_2_top);
    case_pred(2, get__atom__string_2_top);
    case_pred(3, set__atom__string_5_top);
    case_pred(4, intern_2_top);
    case_pred(5, get__atom__name_2_top);
    last_case_pred(6, cache__table_1_top);
  }

 make__atom_2_top: {
  q x0, x1, x2;
  a1 = qp->args[1];
  qp = qp->next;
 make__atom_2_clear_reason:
  reasonp = reasons;
 make__atom_2_0:
 make__atom_2_1:
  switch (ptagof(a0)) {
 case FUNCTOR:
 make__atom_2_2:
  if (!isgobj(a0)) goto make__atom_2_interrupt;
 make__atom_2_3:
  gblt_is_string(a0,make__atom_2_interrupt);
  gblt_size_of_string(a0,x0,make__atom_2_interrupt);
  gblt_element_size_of_string(a0,x1,make__atom_2_interrupt);
 make__atom_2_4:
  if (x1 != makeint(8L)) goto make__atom_2_interrupt;
  x2 = makesym(enter_atom_string(a0));
  unify_value(a1, x2);
  proceed();
 case VARREF:
  deref_and_jump(a0,make__atom_2_1);
  *reasonp++ =  a0;
  goto make__atom_2_interrupt;
  };
  goto make__atom_2_interrupt;
 make__atom_2_ext_interrupt:
  reasonp = 0l;
 make__atom_2_interrupt:
  toppred = &predicate_atom__table_xmake__atom_2;
  goto interrupt_2;
 }

 atom__number_2_top: {
  q x0;
  a1 = qp->args[1];
  qp = qp->next;
 atom__number_2_clear_reason:
  reasonp = reasons;
 atom__number_2_0:
 atom__number_2_1:
  switch (ptagof(a0)) {
 case ATOMIC:
  if (isint(a0)) goto atom__number_2_interrupt;
 atom__number_2_2:
  x0 = makeint(symval(a0));
  unify_value(a1, x0);
  proceed();
 case VARREF:
  deref_and_jump(a0,atom__number_2_1);
  *reasonp++ =  a0;
  goto atom__number_2_interrupt;
  };
  goto atom__number_2_interrupt;
 atom__number_2_ext_interrupt:
  reasonp = 0l;
 atom__number_2_interrupt:
  goto interrupt_2;
 }

 get__atom__string_2_top: {
  q x0, x1, x2;
  a1 = qp->args[1];
  qp = qp->next;
 get__atom__string_2_clear_reason:
  reasonp = reasons;
 get__atom__string_2_0:
 get__atom__string_2_1:
  switch (ptagof(a0)) {
 case ATOMIC:
  if (isint(a0)) goto get__atom__string_2_2;
  switch (symval(a0)) {
 case symval(NILATOM):
  x0 = makefunctor(&string_const_1);
  unify_value(a1, x0);
  proceed();
 case symval(PERIODATOM):
  x0 = makefunctor(&string_const_0);
  unify_value(a1, x0);
  proceed();
  };
  goto get__atom__string_2_2;
 case VARREF:
  deref_and_jump(a0,get__atom__string_2_1);
  *reasonp++ =  a0;
  goto get__atom__string_2_2;
  };
  goto get__atom__string_2_2;
 get__atom__string_2_2:
  otherwise(get__atom__string_2_interrupt);
 get__atom__string_2_3:
  switch (ptagof(a0)) {
 case ATOMIC:
  if (isint(a0)) goto get__atom__string_2_interrupt;
 get__atom__string_2_4:
  gblt_atom(a0,get__atom__string_2_interrupt);
  x0 = makeint(strlen((char *)namestringof(a0)));
  generic_arg[0] = x0;
  x1 = makeint(8L);
  generic_arg[1] = x1;
  new_generic(string_g_new, 2, x2, 0);
  a2 = a0;
  a3 = x2;
  a4 = a1;
  a1 = x0;
  a0 = makeint(0L);
  execute(set__atom__string_5_clear_reason);
  goto set__atom__string_5_ext_interrupt;
 case VARREF:
  deref_and_jump(a0,get__atom__string_2_3);
  *reasonp++ =  a0;
  goto get__atom__string_2_interrupt;
  };
  goto get__atom__string_2_interrupt;
 get__atom__string_2_ext_interrupt:
  reasonp = 0l;
 get__atom__string_2_interrupt:
  toppred = &predicate_atom__table_xget__atom__string_2;
  goto interrupt_2;
 }

 set__atom__string_5_top: {
  q x0, x1, x2, x3;
  a1 = qp->args[1];
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  qp = qp->next;
 set__atom__string_5_clear_reason:
  reasonp = reasons;
 set__atom__string_5_0:
 set__atom__string_5_1:
  switch (ptagof(a0)) {
 case ATOMIC:
  if (!isint(a0)) goto set__atom__string_5_7;
 set__atom__string_5_2:
 set__atom__string_5_3:
  if (!isint(a1)) goto set__atom__string_5_6;
  gblt_less(a0,a1,set__atom__string_5_7);
 set__atom__string_5_4:
  switch (ptagof(a2)) {
 case ATOMIC:
  if (isint(a2)) goto set__atom__string_5_7;
 set__atom__string_5_5:
  x0 = makeint((namestringof(a2))[intval(a0)]);
  generic_arg[0] = a0;
  generic_arg[1] = x0;
  allocp[0] = x1 = makeref(&allocp[0]);
  generic_arg[2] = x1;
  call_generic(a3, functor_set__element_3, 1);
  x2 = makeint(1L);
  bblt_add_no_check(a0,x2,x3);
  a0 = x3;
  a3 = x1;
  execute(set__atom__string_5_clear_reason);
  goto set__atom__string_5_ext_interrupt;
 case VARREF:
  deref_and_jump(a2,set__atom__string_5_4);
  *reasonp++ =  a2;
  goto set__atom__string_5_7;
  };
  goto set__atom__string_5_7;
 set__atom__string_5_6:
  if (!isref(a1)) goto set__atom__string_5_7;
  deref_and_jump(a1,set__atom__string_5_3);
  *reasonp++ =  a1;
  goto set__atom__string_5_7;
 case VARREF:
  deref_and_jump(a0,set__atom__string_5_1);
  *reasonp++ =  a0;
  goto set__atom__string_5_interrupt;
  };
  goto set__atom__string_5_7;
 set__atom__string_5_7:
 set__atom__string_5_8:
  if (isref(a0)) goto set__atom__string_5_9;
  if_not_equal(a0, a1, set__atom__string_5_interrupt);
  unify(a4, a3);
  proceed();
 set__atom__string_5_9:
  deref_and_jump(a0,set__atom__string_5_8);
  *reasonp++ =  a0;
  goto set__atom__string_5_interrupt;
 set__atom__string_5_ext_interrupt:
  reasonp = 0l;
 set__atom__string_5_interrupt:
  toppred = &predicate_atom__table_xset__atom__string_5;
  goto interrupt_5;
 }

 intern_2_top: {
  q x0, x1, x2, x3, x4;
  a1 = qp->args[1];
  qp = qp->next;
 intern_2_clear_reason:
  reasonp = reasons;
 intern_2_0:
 intern_2_1:
  switch (ptagof(a0)) {
 case FUNCTOR:
 intern_2_2:
  if (!isgobj(a0)) goto intern_2_interrupt;
 intern_2_3:
  gblt_is_string(a0,intern_2_interrupt);
  gblt_size_of_string(a0,x0,intern_2_interrupt);
  gblt_element_size_of_string(a0,x1,intern_2_interrupt);
  x2 = makeint(0L);
  gblt_greater(x0,x2,intern_2_interrupt);
  allocp[0] = makesym(functor_normal_1);
  allocp[1] = x4 = makeref(&allocp[1]);
  x3 = makefunctor(&allocp[0]);
  allocp += 2;
  unify_value(a1, x3);
  a1 = x4;
  execute(make__atom_2_0);
  goto make__atom_2_ext_interrupt;
 case VARREF:
  deref_and_jump(a0,intern_2_1);
  *reasonp++ =  a0;
  goto intern_2_interrupt;
  };
  goto intern_2_interrupt;
 intern_2_ext_interrupt:
  reasonp = 0l;
 intern_2_interrupt:
  toppred = &predicate_atom__table_xintern_2;
  goto interrupt_2;
 }

 get__atom__name_2_top: {
  q x0, x1;
  a1 = qp->args[1];
  qp = qp->next;
 get__atom__name_2_clear_reason:
  reasonp = reasons;
 get__atom__name_2_0:
  allocp[0] = makesym(functor_normal_1);
  allocp[1] = x1 = makeref(&allocp[1]);
  x0 = makefunctor(&allocp[0]);
  allocp += 2;
  unify_value(a1, x0);
  a1 = x1;
  execute(get__atom__string_2_0);
  goto get__atom__string_2_ext_interrupt;
 get__atom__name_2_ext_interrupt:
  reasonp = 0l;
 get__atom__name_2_interrupt:
  toppred = &predicate_atom__table_xget__atom__name_2;
  goto interrupt_2;
 }

 cache__table_1_top: {
  q x0, x1, x2, x3, x4, x5;
  qp = qp->next;
 cache__table_1_clear_reason:
  reasonp = reasons;
 cache__table_1_0:
 cache__table_1_1:
  switch (ptagof(a0)) {
 case CONS:
  x0 = car_of(a0);
 cache__table_1_2:
  switch (ptagof(x0)) {
 case FUNCTOR:
  switch (symval(functor_of(x0))) {
 case functor_get__atom_2:
  x1 = arg(x0, 0);
 cache__table_1_3:
  switch (ptagof(x1)) {
 case FUNCTOR:
 cache__table_1_4:
  if (!isgobj(x1)) goto cache__table_1_interrupt;
 cache__table_1_5:
  gblt_is_string(x1,cache__table_1_interrupt);
  gblt_size_of_string(x1,x2,cache__table_1_interrupt);
  gblt_element_size_of_string(x1,x3,cache__table_1_interrupt);
 cache__table_1_6:
  if (x3 != makeint(8L)) goto cache__table_1_interrupt;
  x4 = makeint(0L);
  gblt_greater(x2,x4,cache__table_1_interrupt);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_atom__table_xcache__table_1);
  x5 = cdr_of(a0);
  allocp[2] = x5;
  a0 = x1;
  a1 = arg(x0, 1);
  qp = (struct goalrec*)&allocp[0];
  allocp += 3;
  execute(intern_2_0);
  goto intern_2_ext_interrupt;
 case VARREF:
  deref_and_jump(x1,cache__table_1_3);
  *reasonp++ =  x1;
  goto cache__table_1_interrupt;
  };
  goto cache__table_1_interrupt;
 case functor_get__atom__name_2:
  x1 = arg(x0, 0);
 cache__table_1_7:
  switch (ptagof(x1)) {
 case ATOMIC:
  if (isint(x1)) goto cache__table_1_interrupt;
 cache__table_1_8:
  gblt_atom(x1,cache__table_1_interrupt);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_atom__table_xcache__table_1);
  x2 = cdr_of(a0);
  allocp[2] = x2;
  a0 = x1;
  a1 = arg(x0, 1);
  qp = (struct goalrec*)&allocp[0];
  allocp += 3;
  execute(get__atom__name_2_0);
  goto get__atom__name_2_ext_interrupt;
 case VARREF:
  deref_and_jump(x1,cache__table_1_7);
  *reasonp++ =  x1;
  goto cache__table_1_interrupt;
  };
  goto cache__table_1_interrupt;
 case functor_intern_2:
  x1 = arg(x0, 0);
 cache__table_1_9:
  switch (ptagof(x1)) {
 case FUNCTOR:
 cache__table_1_10:
  if (!isgobj(x1)) goto cache__table_1_interrupt;
 cache__table_1_11:
  gblt_is_string(x1,cache__table_1_interrupt);
  gblt_size_of_string(x1,x2,cache__table_1_interrupt);
  gblt_element_size_of_string(x1,x3,cache__table_1_interrupt);
 cache__table_1_12:
  if (x3 != makeint(8L)) goto cache__table_1_interrupt;
  x4 = makeint(0L);
  gblt_greater(x2,x4,cache__table_1_interrupt);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_atom__table_xcache__table_1);
  x5 = cdr_of(a0);
  allocp[2] = x5;
  a0 = x1;
  a1 = arg(x0, 1);
  qp = (struct goalrec*)&allocp[0];
  allocp += 3;
  execute(intern_2_0);
  goto intern_2_ext_interrupt;
 case VARREF:
  deref_and_jump(x1,cache__table_1_9);
  *reasonp++ =  x1;
  goto cache__table_1_interrupt;
  };
  goto cache__table_1_interrupt;
  };
  goto cache__table_1_interrupt;
 case VARREF:
  deref_and_jump(x0,cache__table_1_2);
  *reasonp++ =  x0;
  goto cache__table_1_interrupt;
  };
  goto cache__table_1_interrupt;
 case ATOMIC:
  if (a0 != NILATOM) goto cache__table_1_interrupt;
  proceed();
 case VARREF:
  deref_and_jump(a0,cache__table_1_1);
  *reasonp++ =  a0;
  goto cache__table_1_interrupt;
  };
  goto cache__table_1_interrupt;
 cache__table_1_ext_interrupt:
  reasonp = 0l;
 cache__table_1_interrupt:
  goto interrupt_1;
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
  loop_within_module(module_atom__table);
}
