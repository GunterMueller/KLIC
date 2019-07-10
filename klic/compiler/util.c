/* Compiled by KLIC compiler version 3.003 (Mon Mar 29 19:42:59 JST 1999) */
#include <klic/klichdr.h>
#include "atom.h"
#include "funct.h"

module module_klic__comp__util();
Const struct predicate predicate_klic__comp__util_xappend_3 =
   { module_klic__comp__util, 0, 3 };
Const struct predicate predicate_klic__comp__util_xassoc_3 =
   { module_klic__comp__util, 1, 3 };
Const struct predicate predicate_klic__comp__util_xnth_3 =
   { module_klic__comp__util, 2, 3 };
Const struct predicate predicate_klic__comp__util_xmember_3 =
   { module_klic__comp__util, 3, 3 };
Const struct predicate predicate_klic__comp__util_xqsort_2 =
   { module_klic__comp__util, 4, 2 };
Const struct predicate predicate_klic__comp__util_xqsort_3 =
   { module_klic__comp__util, 5, 3 };
Const struct predicate predicate_klic__comp__util_xpartition_4 =
   { module_klic__comp__util, 6, 4 };
Const struct predicate predicate_klic__comp__util_xmax_3 =
   { module_klic__comp__util, 7, 3 };
Const struct predicate predicate_klic__comp__util_xmin_3 =
   { module_klic__comp__util, 8, 3 };
Const struct predicate predicate_klic__comp__util_xmax__vect_2 =
   { module_klic__comp__util, 9, 2 };
Const struct predicate predicate_klic__comp__util_xmax__vect_4 =
   { module_klic__comp__util, 10, 4 };
Const struct predicate predicate_klic__comp__util_xremove_4 =
   { module_klic__comp__util, 11, 4 };
Const struct predicate predicate_klic__comp__util_xlength_2 =
   { module_klic__comp__util, 12, 2 };
Const struct predicate predicate_klic__comp__util_xlength_3 =
   { module_klic__comp__util, 13, 3 };
Const struct predicate predicate_klic__comp__util_xvector__to__list_2 =
   { module_klic__comp__util, 14, 2 };
Const struct predicate predicate_klic__comp__util_xvector__to__list_4 =
   { module_klic__comp__util, 15, 4 };
Const struct predicate predicate_klic__comp__util_xuniv_2 =
   { module_klic__comp__util, 16, 2 };
Const struct predicate predicate_klic__comp__util_xuniv_3 =
   { module_klic__comp__util, 17, 3 };
Const struct predicate predicate_klic__comp__util_xuniv__funct__to__list_5 =
   { module_klic__comp__util, 18, 5 };
Const struct predicate predicate_klic__comp__util_xname_2 =
   { module_klic__comp__util, 19, 2 };
Const struct predicate predicate_klic__comp__util_xname__integer_2 =
   { module_klic__comp__util, 20, 2 };
Const struct predicate predicate_klic__comp__util_xname__integer_3 =
   { module_klic__comp__util, 21, 3 };
Const struct predicate predicate_klic__comp__util_xstring__to__char__list_2 =
   { module_klic__comp__util, 22, 2 };
Const struct predicate predicate_klic__comp__util_xstring__to__char__list_4 =
   { module_klic__comp__util, 23, 4 };
Const struct predicate predicate_klic__comp__util_xnumbervars_4 =
   { module_klic__comp__util, 24, 4 };
Const struct predicate predicate_klic__comp__util_xnumbervars_6 =
   { module_klic__comp__util, 25, 6 };
Const struct predicate predicate_klic__comp__util_xnumbervars__1_7 =
   { module_klic__comp__util, 26, 7 };
Const struct predicate predicate_klic__comp__util_xnumbervars__vector_7 =
   { module_klic__comp__util, 27, 7 };
Const struct predicate predicate_klic__comp__util_xnumbervars__funct_9 =
   { module_klic__comp__util, 28, 9 };
Const struct predicate predicate_klic__comp__util_xmake__atom_2 =
   { module_klic__comp__util, 29, 2 };
extern q string_g_new();
extern Const struct predicate predicate_functor__table_x_3D_2E_2E_2;
extern Const struct predicate predicate_atom__table_xget__atom__string_2;
extern Const struct predicate predicate_atom__table_xmake__atom_2;
declare_method_table_of(vector);
declare_method_table_of(float);

module module_klic__comp__util(glbl, qp, allocp, toppred)
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
  q a0, a1, a2, a3, a4, a5, a6, a7, a8;

  q *reasonp;
 module_top:
  a0 = qp->args[0];
  a1 = qp->args[1];
  switch_on_pred() {
    case_pred(0, append_3_top);
    case_pred(1, assoc_3_top);
    case_pred(2, nth_3_top);
    case_pred(3, member_3_top);
    case_pred(4, qsort_2_top);
    case_pred(5, qsort_3_top);
    case_pred(6, partition_4_top);
    case_pred(7, max_3_top);
    case_pred(8, min_3_top);
    case_pred(9, max__vect_2_top);
    case_pred(10, max__vect_4_top);
    case_pred(11, remove_4_top);
    case_pred(12, length_2_top);
    case_pred(13, length_3_top);
    case_pred(14, vector__to__list_2_top);
    case_pred(15, vector__to__list_4_top);
    case_pred(16, univ_2_top);
    case_pred(17, univ_3_top);
    case_pred(18, univ__funct__to__list_5_top);
    case_pred(19, name_2_top);
    case_pred(20, name__integer_2_top);
    case_pred(21, name__integer_3_top);
    case_pred(22, string__to__char__list_2_top);
    case_pred(23, string__to__char__list_4_top);
    case_pred(24, numbervars_4_top);
    case_pred(25, numbervars_6_top);
    case_pred(26, numbervars__1_7_top);
    case_pred(27, numbervars__vector_7_top);
    case_pred(28, numbervars__funct_9_top);
    last_case_pred(29, make__atom_2_top);
  }

 append_3_top: {
  q x0, x1, x2;
  a2 = qp->args[2];
  qp = qp->next;
 append_3_clear_reason:
  reasonp = reasons;
 append_3_0:
 append_3_1:
  switch (ptagof(a0)) {
 case CONS:
  allocp[0] = x1 = makeref(&allocp[0]);
  x2 = car_of(a0);
  allocp[1] = x2;
  x0 = makecons(&allocp[0]);
  allocp += 2;
  unify_value(a2, x0);
  a0 = cdr_of(a0);
  a2 = x1;
  execute(append_3_0);
  goto append_3_ext_interrupt;
 case ATOMIC:
  if (a0 != NILATOM) goto append_3_interrupt;
  unify(a1, a2);
  proceed();
 case VARREF:
  deref_and_jump(a0,append_3_1);
  *reasonp++ =  a0;
  goto append_3_interrupt;
  };
  goto append_3_interrupt;
 append_3_ext_interrupt:
  reasonp = 0l;
 append_3_interrupt:
  goto interrupt_3;
 }

 assoc_3_top: {
  q x0, x1, x2, x3;
  a2 = qp->args[2];
  qp = qp->next;
 assoc_3_clear_reason:
  reasonp = reasons;
 assoc_3_0:
 assoc_3_1:
  switch (ptagof(a0)) {
 case CONS:
  x0 = car_of(a0);
 assoc_3_2:
  switch (ptagof(x0)) {
 case FUNCTOR:
  if (functor_of(x0) != makesym(functor__3D_2)) goto assoc_3_7;
  x1 = arg(x0, 0);
 assoc_3_3:
  if (isref(x1)) goto assoc_3_6;
 assoc_3_4:
  if (isref(x1)) goto assoc_3_5;
  if_not_equal(x1, a1, assoc_3_7);
  allocp[0] = makesym(functor_yes_1);
  x3 = arg(x0, 1);
  allocp[1] = x3;
  x2 = makefunctor(&allocp[0]);
  allocp += 2;
  unify_value(a2, x2);
  proceed();
 assoc_3_5:
  deref_and_jump(x1,assoc_3_4);
  *reasonp++ =  x1;
  goto assoc_3_7;
 assoc_3_6:
  deref_and_jump(x1,assoc_3_3);
  *reasonp++ =  x1;
  goto assoc_3_7;
 case VARREF:
  deref_and_jump(x0,assoc_3_2);
  *reasonp++ =  x0;
  goto assoc_3_7;
  };
  goto assoc_3_7;
 case ATOMIC:
  if (a0 != NILATOM) goto assoc_3_7;
  x0 = makesym(atom_no);
  unify_value(a2, x0);
  proceed();
 case VARREF:
  deref_and_jump(a0,assoc_3_1);
  *reasonp++ =  a0;
  goto assoc_3_7;
  };
  goto assoc_3_7;
 assoc_3_7:
  otherwise(assoc_3_interrupt);
 assoc_3_8:
  switch (ptagof(a0)) {
 case CONS:
  a0 = cdr_of(a0);
  execute(assoc_3_0);
  goto assoc_3_ext_interrupt;
 case VARREF:
  deref_and_jump(a0,assoc_3_8);
  *reasonp++ =  a0;
  goto assoc_3_interrupt;
  };
  goto assoc_3_interrupt;
 assoc_3_ext_interrupt:
  reasonp = 0l;
 assoc_3_interrupt:
  toppred = &predicate_klic__comp__util_xassoc_3;
  goto interrupt_3;
 }

 nth_3_top: {
  q x0, x1, x2, x3;
  a2 = qp->args[2];
  qp = qp->next;
 nth_3_clear_reason:
  reasonp = reasons;
 nth_3_0:
 nth_3_1:
  switch (ptagof(a1)) {
 case CONS:
 nth_3_2:
  if (!isint(a0)) goto nth_3_4;
  x0 = makeint(0L);
  gblt_less_or_eq(a0,x0,nth_3_3);
  x1 = car_of(a1);
  unify(a2, x1);
  proceed();
 nth_3_3:
  x1 = makeint(0L);
  gblt_greater(a0,x1,nth_3_interrupt);
  x2 = makeint(1L);
  bblt_sub_no_check(a0,x2,x3);
  a0 = x3;
  a1 = cdr_of(a1);
  execute(nth_3_0);
  goto nth_3_ext_interrupt;
 nth_3_4:
  if (!isref(a0)) goto nth_3_interrupt;
  deref_and_jump(a0,nth_3_2);
  *reasonp++ =  a0;
  goto nth_3_interrupt;
 case VARREF:
  deref_and_jump(a1,nth_3_1);
  *reasonp++ =  a1;
  goto nth_3_interrupt;
  };
  goto nth_3_interrupt;
 nth_3_ext_interrupt:
  reasonp = 0l;
 nth_3_interrupt:
  goto interrupt_3;
 }

 member_3_top: {
  q x0, x1;
  a2 = qp->args[2];
  qp = qp->next;
 member_3_clear_reason:
  reasonp = reasons;
 member_3_0:
 member_3_1:
  switch (ptagof(a0)) {
 case CONS:
  x0 = car_of(a0);
 member_3_2:
  if (isref(x0)) goto member_3_5;
 member_3_3:
  if (isref(x0)) goto member_3_4;
  if_not_equal(x0, a1, member_3_6);
  x1 = makesym(atom_yes);
  unify_value(a2, x1);
  proceed();
 member_3_4:
  deref_and_jump(x0,member_3_3);
  *reasonp++ =  x0;
  goto member_3_6;
 member_3_5:
  deref_and_jump(x0,member_3_2);
  *reasonp++ =  x0;
  goto member_3_6;
 case ATOMIC:
  if (a0 != NILATOM) goto member_3_6;
  x0 = makesym(atom_no);
  unify_value(a2, x0);
  proceed();
 case VARREF:
  deref_and_jump(a0,member_3_1);
  *reasonp++ =  a0;
  goto member_3_6;
  };
  goto member_3_6;
 member_3_6:
  otherwise(member_3_interrupt);
 member_3_7:
  switch (ptagof(a0)) {
 case CONS:
  a0 = cdr_of(a0);
  execute(member_3_0);
  goto member_3_ext_interrupt;
 case VARREF:
  deref_and_jump(a0,member_3_7);
  *reasonp++ =  a0;
  goto member_3_interrupt;
  };
  goto member_3_interrupt;
 member_3_ext_interrupt:
  reasonp = 0l;
 member_3_interrupt:
  goto interrupt_3;
 }

 qsort_2_top: {
  qp = qp->next;
 qsort_2_clear_reason:
  reasonp = reasons;
 qsort_2_0:
  a2 = NILATOM;
  execute(qsort_3_0);
  goto qsort_3_ext_interrupt;
 qsort_2_ext_interrupt:
  reasonp = 0l;
 qsort_2_interrupt:
  goto interrupt_2;
 }

 qsort_3_top: {
  q x0, x1, x2, x3, x4, x5;
  a2 = qp->args[2];
  qp = qp->next;
 qsort_3_clear_reason:
  reasonp = reasons;
 qsort_3_0:
 qsort_3_1:
  switch (ptagof(a0)) {
 case CONS:
  allocp[0] = x1 = makeref(&allocp[0]);
  x2 = car_of(a0);
  allocp[1] = x2;
  x0 = makecons(&allocp[0]);
  allocp[2] = (q)qp;
  allocp[3] = (q)(&predicate_klic__comp__util_xqsort_3);
  allocp[4] = x3 = makeref(&allocp[4]);
  allocp[5] = a1;
  allocp[6] = x0;
  allocp[7] = (q)(struct goalrec*)&allocp[2];
  allocp[8] = (q)(&predicate_klic__comp__util_xqsort_3);
  allocp[9] = x4 = makeref(&allocp[9]);
  allocp[10] = x1;
  allocp[11] = a2;
  x5 = cdr_of(a0);
  a1 = car_of(a0);
  a2 = x3;
  a3 = x4;
  a0 = x5;
  qp = (struct goalrec*)&allocp[7];
  allocp += 12;
  execute(partition_4_0);
  goto partition_4_ext_interrupt;
 case ATOMIC:
  if (a0 != NILATOM) goto qsort_3_interrupt;
  unify(a1, a2);
  proceed();
 case VARREF:
  deref_and_jump(a0,qsort_3_1);
  *reasonp++ =  a0;
  goto qsort_3_interrupt;
  };
  goto qsort_3_interrupt;
 qsort_3_ext_interrupt:
  reasonp = 0l;
 qsort_3_interrupt:
  toppred = &predicate_klic__comp__util_xqsort_3;
  goto interrupt_3;
 }

 partition_4_top: {
  q x0, x1, x2, x3, x4, x5;
  a2 = qp->args[2];
  a3 = qp->args[3];
  qp = qp->next;
 partition_4_clear_reason:
  reasonp = reasons;
 partition_4_0:
 partition_4_1:
  switch (ptagof(a0)) {
 case CONS:
  x0 = car_of(a0);
 partition_4_2:
  if (isref(x0)) goto partition_4_9;
 partition_4_3:
  if (isref(x0)) goto partition_4_4;
  if_not_equal(x0, a1, partition_4_5);
  a0 = cdr_of(a0);
  execute(partition_4_clear_reason);
  goto partition_4_ext_interrupt;
 partition_4_4:
  deref_and_jump(x0,partition_4_3);
  *reasonp++ =  x0;
  goto partition_4_5;
 partition_4_5:
 partition_4_6:
  if (isref(a1)) goto partition_4_8;
  gblt_compare(x0,a1,x1,partition_4_interrupt);
  x2 = makeint(0L);
  gblt_less(x1,x2,partition_4_7);
  allocp[0] = x4 = makeref(&allocp[0]);
  allocp[1] = x0;
  x3 = makecons(&allocp[0]);
  allocp += 2;
  unify_value(a2, x3);
  a0 = cdr_of(a0);
  a2 = x4;
  execute(partition_4_clear_reason);
  goto partition_4_ext_interrupt;
 partition_4_7:
  x3 = makeint(0L);
  gblt_greater(x1,x3,partition_4_interrupt);
  allocp[0] = x5 = makeref(&allocp[0]);
  allocp[1] = x0;
  x4 = makecons(&allocp[0]);
  allocp += 2;
  unify_value(a3, x4);
  a0 = cdr_of(a0);
  a3 = x5;
  execute(partition_4_clear_reason);
  goto partition_4_ext_interrupt;
 partition_4_8:
  deref_and_jump(a1,partition_4_6);
  *reasonp++ =  a1;
  goto partition_4_interrupt;
 partition_4_9:
  deref_and_jump(x0,partition_4_2);
  *reasonp++ =  x0;
  goto partition_4_interrupt;
 case ATOMIC:
  if (a0 != NILATOM) goto partition_4_interrupt;
  x0 = NILATOM;
  unify_value(a2, x0);
  x1 = NILATOM;
  unify_value(a3, x1);
  proceed();
 case VARREF:
  deref_and_jump(a0,partition_4_1);
  *reasonp++ =  a0;
  goto partition_4_interrupt;
  };
  goto partition_4_interrupt;
 partition_4_ext_interrupt:
  reasonp = 0l;
 partition_4_interrupt:
  toppred = &predicate_klic__comp__util_xpartition_4;
  goto interrupt_4;
 }

 max_3_top: {
  a2 = qp->args[2];
  qp = qp->next;
 max_3_clear_reason:
  reasonp = reasons;
 max_3_0:
 max_3_1:
  if (!isint(a0)) goto max_3_5;
 max_3_2:
  if (!isint(a1)) goto max_3_4;
  gblt_greater_or_eq(a0,a1,max_3_3);
  unify_value(a2, a0);
  proceed();
 max_3_3:
  gblt_less_or_eq(a0,a1,max_3_interrupt);
  unify_value(a2, a1);
  proceed();
 max_3_4:
  if (!isref(a1)) goto max_3_interrupt;
  deref_and_jump(a1,max_3_2);
  *reasonp++ =  a1;
  goto max_3_interrupt;
 max_3_5:
  if (!isref(a0)) goto max_3_interrupt;
  deref_and_jump(a0,max_3_1);
  *reasonp++ =  a0;
  goto max_3_interrupt;
 max_3_ext_interrupt:
  reasonp = 0l;
 max_3_interrupt:
  toppred = &predicate_klic__comp__util_xmax_3;
  goto interrupt_3;
 }

 min_3_top: {
  a2 = qp->args[2];
  qp = qp->next;
 min_3_clear_reason:
  reasonp = reasons;
 min_3_0:
 min_3_1:
  if (!isint(a0)) goto min_3_5;
 min_3_2:
  if (!isint(a1)) goto min_3_4;
  gblt_less_or_eq(a0,a1,min_3_3);
  unify_value(a2, a0);
  proceed();
 min_3_3:
  gblt_greater_or_eq(a0,a1,min_3_interrupt);
  unify_value(a2, a1);
  proceed();
 min_3_4:
  if (!isref(a1)) goto min_3_interrupt;
  deref_and_jump(a1,min_3_2);
  *reasonp++ =  a1;
  goto min_3_interrupt;
 min_3_5:
  if (!isref(a0)) goto min_3_interrupt;
  deref_and_jump(a0,min_3_1);
  *reasonp++ =  a0;
  goto min_3_interrupt;
 min_3_ext_interrupt:
  reasonp = 0l;
 min_3_interrupt:
  goto interrupt_3;
 }

 max__vect_2_top: {
  q x0, x1;
  qp = qp->next;
 max__vect_2_clear_reason:
  reasonp = reasons;
 max__vect_2_0:
 max__vect_2_1:
  switch (ptagof(a0)) {
 case FUNCTOR:
 max__vect_2_2:
  if (!isgobj(a0)) goto max__vect_2_interrupt;
  if (!isclass(a0,vector)) goto max__vect_2_interrupt;
  gblt_size_of_vector(a0,x0,max__vect_2_interrupt);
  x1 = a0;
  a2 = makeint(0L);
  a3 = a1;
  a1 = x1;
  a0 = x0;
  execute(max__vect_4_0);
  goto max__vect_4_ext_interrupt;
 case VARREF:
  deref_and_jump(a0,max__vect_2_1);
  *reasonp++ =  a0;
  goto max__vect_2_interrupt;
  };
  goto max__vect_2_interrupt;
 max__vect_2_ext_interrupt:
  reasonp = 0l;
 max__vect_2_interrupt:
  goto interrupt_2;
 }

 max__vect_4_top: {
  q x0, x1, x2, x3, x4, x5;
  a2 = qp->args[2];
  a3 = qp->args[3];
  qp = qp->next;
 max__vect_4_clear_reason:
  reasonp = reasons;
 max__vect_4_0:
 max__vect_4_1:
  if (!isint(a0)) goto max__vect_4_5;
  x0 = makeint(0L);
  gblt_less_or_eq(a0,x0,max__vect_4_2);
  unify(a3, a2);
  proceed();
 max__vect_4_2:
  x1 = makeint(0L);
  gblt_greater(a0,x1,max__vect_4_interrupt);
  x3 = makeint(1L);
  gblt_subtract(a0,x3,x2,max__vect_4_interrupt);
 max__vect_4_3:
  switch (ptagof(a1)) {
 case FUNCTOR:
 max__vect_4_4:
  if (!isgobj(a1)) goto max__vect_4_interrupt;
  if (!isclass(a1,vector)) goto max__vect_4_interrupt;
  gblt_element_of_vector(a1,x2,x4,max__vect_4_interrupt);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__util_xmax__vect_4);
  allocp[2] = x2;
  allocp[3] = a1;
  allocp[4] = x5 = makeref(&allocp[4]);
  allocp[5] = a3;
  a0 = x4;
  a1 = a2;
  a2 = x5;
  qp = (struct goalrec*)&allocp[0];
  allocp += 6;
  execute(max_3_0);
  goto max_3_ext_interrupt;
 case VARREF:
  deref_and_jump(a1,max__vect_4_3);
  *reasonp++ =  a1;
  goto max__vect_4_interrupt;
  };
  goto max__vect_4_interrupt;
 max__vect_4_5:
  if (!isref(a0)) goto max__vect_4_interrupt;
  deref_and_jump(a0,max__vect_4_1);
  *reasonp++ =  a0;
  goto max__vect_4_interrupt;
 max__vect_4_ext_interrupt:
  reasonp = 0l;
 max__vect_4_interrupt:
  toppred = &predicate_klic__comp__util_xmax__vect_4;
  goto interrupt_4;
 }

 remove_4_top: {
  q x0, x1, x2;
  a2 = qp->args[2];
  a3 = qp->args[3];
  qp = qp->next;
 remove_4_clear_reason:
  reasonp = reasons;
 remove_4_0:
 remove_4_1:
  switch (ptagof(a0)) {
 case CONS:
  x0 = car_of(a0);
 remove_4_2:
  if (isref(x0)) goto remove_4_5;
 remove_4_3:
  if (isref(x0)) goto remove_4_4;
  if_not_equal(x0, a1, remove_4_6);
  a0 = cdr_of(a0);
  a1 = x0;
  execute(remove_4_clear_reason);
  goto remove_4_ext_interrupt;
 remove_4_4:
  deref_and_jump(x0,remove_4_3);
  *reasonp++ =  x0;
  goto remove_4_6;
 remove_4_5:
  deref_and_jump(x0,remove_4_2);
  *reasonp++ =  x0;
  goto remove_4_6;
 case ATOMIC:
  if (a0 != NILATOM) goto remove_4_6;
  unify(a2, a3);
  proceed();
 case VARREF:
  deref_and_jump(a0,remove_4_1);
  *reasonp++ =  a0;
  goto remove_4_6;
  };
  goto remove_4_6;
 remove_4_6:
  otherwise(remove_4_interrupt);
 remove_4_7:
  switch (ptagof(a0)) {
 case CONS:
  allocp[0] = x1 = makeref(&allocp[0]);
  x2 = car_of(a0);
  allocp[1] = x2;
  x0 = makecons(&allocp[0]);
  allocp += 2;
  unify_value(a2, x0);
  a0 = cdr_of(a0);
  a2 = x1;
  execute(remove_4_0);
  goto remove_4_ext_interrupt;
 case VARREF:
  deref_and_jump(a0,remove_4_7);
  *reasonp++ =  a0;
  goto remove_4_interrupt;
  };
  goto remove_4_interrupt;
 remove_4_ext_interrupt:
  reasonp = 0l;
 remove_4_interrupt:
  goto interrupt_4;
 }

 length_2_top: {
  qp = qp->next;
 length_2_clear_reason:
  reasonp = reasons;
 length_2_0:
  a2 = makeint(0L);
  execute(length_3_0);
  goto length_3_ext_interrupt;
 length_2_ext_interrupt:
  reasonp = 0l;
 length_2_interrupt:
  goto interrupt_2;
 }

 length_3_top: {
  q x0, x1;
  a2 = qp->args[2];
  qp = qp->next;
 length_3_clear_reason:
  reasonp = reasons;
 length_3_0:
 length_3_1:
  switch (ptagof(a0)) {
 case CONS:
  x0 = makeint(1L);
  bblt_add_no_check_y(a2,x0,x1);
  a0 = cdr_of(a0);
  a2 = x1;
  execute(length_3_0);
  goto length_3_ext_interrupt;
 case ATOMIC:
  if (a0 != NILATOM) goto length_3_interrupt;
  unify(a1, a2);
  proceed();
 case VARREF:
  deref_and_jump(a0,length_3_1);
  *reasonp++ =  a0;
  goto length_3_interrupt;
  };
  goto length_3_interrupt;
 length_3_ext_interrupt:
  reasonp = 0l;
 length_3_interrupt:
  toppred = &predicate_klic__comp__util_xlength_3;
  goto interrupt_3;
 }

 vector__to__list_2_top: {
  q x0;
  qp = qp->next;
 vector__to__list_2_clear_reason:
  reasonp = reasons;
 vector__to__list_2_0:
 vector__to__list_2_1:
  switch (ptagof(a0)) {
 case FUNCTOR:
 vector__to__list_2_2:
  if (!isgobj(a0)) goto vector__to__list_2_interrupt;
  if (!isclass(a0,vector)) goto vector__to__list_2_interrupt;
  gblt_size_of_vector(a0,x0,vector__to__list_2_interrupt);
  a2 = NILATOM;
  a3 = x0;
  execute(vector__to__list_4_0);
  goto vector__to__list_4_ext_interrupt;
 case VARREF:
  deref_and_jump(a0,vector__to__list_2_1);
  *reasonp++ =  a0;
  goto vector__to__list_2_interrupt;
  };
  goto vector__to__list_2_interrupt;
 vector__to__list_2_ext_interrupt:
  reasonp = 0l;
 vector__to__list_2_interrupt:
  goto interrupt_2;
 }

 vector__to__list_4_top: {
  q x0, x1, x2, x3, x4, x5;
  a2 = qp->args[2];
  a3 = qp->args[3];
  qp = qp->next;
 vector__to__list_4_clear_reason:
  reasonp = reasons;
 vector__to__list_4_0:
 vector__to__list_4_1:
  if (!isint(a3)) goto vector__to__list_4_5;
  x0 = makeint(0L);
  gblt_less_or_eq(a3,x0,vector__to__list_4_2);
  unify(a1, a2);
  proceed();
 vector__to__list_4_2:
  x1 = makeint(0L);
  gblt_greater(a3,x1,vector__to__list_4_interrupt);
  x3 = makeint(1L);
  gblt_subtract(a3,x3,x2,vector__to__list_4_interrupt);
 vector__to__list_4_3:
  switch (ptagof(a0)) {
 case FUNCTOR:
 vector__to__list_4_4:
  if (!isgobj(a0)) goto vector__to__list_4_interrupt;
  if (!isclass(a0,vector)) goto vector__to__list_4_interrupt;
  gblt_element_of_vector(a0,x2,x4,vector__to__list_4_interrupt);
  allocp[0] = a2;
  allocp[1] = x4;
  x5 = makecons(&allocp[0]);
  a2 = x5;
  a3 = x2;
  allocp += 2;
  execute(vector__to__list_4_0);
  goto vector__to__list_4_ext_interrupt;
 case VARREF:
  deref_and_jump(a0,vector__to__list_4_3);
  *reasonp++ =  a0;
  goto vector__to__list_4_interrupt;
  };
  goto vector__to__list_4_interrupt;
 vector__to__list_4_5:
  if (!isref(a3)) goto vector__to__list_4_interrupt;
  deref_and_jump(a3,vector__to__list_4_1);
  *reasonp++ =  a3;
  goto vector__to__list_4_interrupt;
 vector__to__list_4_ext_interrupt:
  reasonp = 0l;
 vector__to__list_4_interrupt:
  toppred = &predicate_klic__comp__util_xvector__to__list_4;
  goto interrupt_4;
 }

 univ_2_top: {
  qp = qp->next;
 univ_2_clear_reason:
  reasonp = reasons;
 univ_2_0:
  a2 = NILATOM;
  execute(univ_3_0);
  goto univ_3_ext_interrupt;
 univ_2_ext_interrupt:
  reasonp = 0l;
 univ_2_interrupt:
  goto interrupt_2;
 }

 univ_3_top: {
  q x0, x1, x2, x3;
  a2 = qp->args[2];
  qp = qp->next;
 univ_3_clear_reason:
  reasonp = reasons;
 univ_3_0:
 univ_3_1:
  if (isref(a0)) goto univ_3_2;
  gblt_pfunctor(a0,x0,univ_3_interrupt);
  gblt_arity(a0,x1,univ_3_interrupt);
  allocp[0] = x3 = makeref(&allocp[0]);
  allocp[1] = x0;
  x2 = makecons(&allocp[0]);
  allocp += 2;
  unify_value(a1, x2);
  a1 = x3;
  a3 = x1;
  a4 = a2;
  a2 = makeint(1L);
  execute(univ__funct__to__list_5_0);
  goto univ__funct__to__list_5_ext_interrupt;
 univ_3_2:
  deref_and_jump(a0,univ_3_1);
  *reasonp++ =  a0;
  goto univ_3_interrupt;
 univ_3_ext_interrupt:
  reasonp = 0l;
 univ_3_interrupt:
  toppred = &predicate_klic__comp__util_xuniv_3;
  goto interrupt_3;
 }

 univ__funct__to__list_5_top: {
  q x0, x1, x2, x3, x4;
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  qp = qp->next;
 univ__funct__to__list_5_clear_reason:
  reasonp = reasons;
 univ__funct__to__list_5_0:
 univ__funct__to__list_5_1:
  if (!isint(a2)) goto univ__funct__to__list_5_7;
 univ__funct__to__list_5_2:
  if (!isint(a3)) goto univ__funct__to__list_5_6;
  gblt_greater(a2,a3,univ__funct__to__list_5_3);
  unify(a1, a4);
  proceed();
 univ__funct__to__list_5_3:
  gblt_less_or_eq(a2,a3,univ__funct__to__list_5_interrupt);
 univ__funct__to__list_5_4:
  if (isref(a0)) goto univ__funct__to__list_5_5;
  gblt_arg(a2,a0,x0,univ__funct__to__list_5_interrupt);
  allocp[0] = x2 = makeref(&allocp[0]);
  allocp[1] = x0;
  x1 = makecons(&allocp[0]);
  allocp += 2;
  unify_value(a1, x1);
  x3 = makeint(1L);
  bblt_add_no_check(a2,x3,x4);
  a1 = x2;
  a2 = x4;
  execute(univ__funct__to__list_5_0);
  goto univ__funct__to__list_5_ext_interrupt;
 univ__funct__to__list_5_5:
  deref_and_jump(a0,univ__funct__to__list_5_4);
  *reasonp++ =  a0;
  goto univ__funct__to__list_5_interrupt;
 univ__funct__to__list_5_6:
  if (!isref(a3)) goto univ__funct__to__list_5_interrupt;
  deref_and_jump(a3,univ__funct__to__list_5_2);
  *reasonp++ =  a3;
  goto univ__funct__to__list_5_interrupt;
 univ__funct__to__list_5_7:
  if (!isref(a2)) goto univ__funct__to__list_5_interrupt;
  deref_and_jump(a2,univ__funct__to__list_5_1);
  *reasonp++ =  a2;
  goto univ__funct__to__list_5_interrupt;
 univ__funct__to__list_5_ext_interrupt:
  reasonp = 0l;
 univ__funct__to__list_5_interrupt:
  toppred = &predicate_klic__comp__util_xuniv__funct__to__list_5;
  goto interrupt_5;
 }

 name_2_top: {
  q x0;
  qp = qp->next;
 name_2_clear_reason:
  reasonp = reasons;
 name_2_0:
 name_2_1:
  switch (ptagof(a0)) {
 case ATOMIC:
  if (isint(a0)) goto name_2_3;
 name_2_2:
  gblt_atom(a0,name_2_interrupt);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__util_xstring__to__char__list_2);
  allocp[2] = x0 = makeref(&allocp[2]);
  allocp[3] = a1;
  allocp[4] = (q)(struct goalrec*)&allocp[0];
  allocp[5] = (q)(&predicate_atom__table_xget__atom__string_2);
  allocp[6] = a0;
  allocp[7] = x0;
  qp = (struct goalrec*)&allocp[4];
  allocp += 8;
  proceed();
 name_2_3:
  gblt_integer(a0,name_2_interrupt);
  execute(name__integer_2_0);
  goto name__integer_2_ext_interrupt;
 case VARREF:
  deref_and_jump(a0,name_2_1);
  *reasonp++ =  a0;
  goto name_2_interrupt;
  };
  goto name_2_interrupt;
 name_2_ext_interrupt:
  reasonp = 0l;
 name_2_interrupt:
  goto interrupt_2;
 }

 name__integer_2_top: {
  q x0, x1, x2, x3, x4;
  qp = qp->next;
 name__integer_2_clear_reason:
  reasonp = reasons;
 name__integer_2_0:
 name__integer_2_1:
  if (!isint(a0)) goto name__integer_2_3;
  x0 = makeint(0L);
  gblt_greater_or_eq(a0,x0,name__integer_2_2);
  a2 = NILATOM;
  execute(name__integer_3_0);
  goto name__integer_3_ext_interrupt;
 name__integer_2_2:
  x1 = makeint(0L);
  gblt_less(a0,x1,name__integer_2_interrupt);
  allocp[0] = x3 = makeref(&allocp[0]);
  allocp[1] = makeint(45L);
  x2 = makecons(&allocp[0]);
  allocp += 2;
  unify_value(a1, x2);
  bblt_minus_no_check(a0,x4);
  a0 = x4;
  a1 = x3;
  a2 = NILATOM;
  execute(name__integer_3_0);
  goto name__integer_3_ext_interrupt;
 name__integer_2_3:
  if (!isref(a0)) goto name__integer_2_interrupt;
  deref_and_jump(a0,name__integer_2_1);
  *reasonp++ =  a0;
  goto name__integer_2_interrupt;
 name__integer_2_ext_interrupt:
  reasonp = 0l;
 name__integer_2_interrupt:
  toppred = &predicate_klic__comp__util_xname__integer_2;
  goto interrupt_2;
 }

 name__integer_3_top: {
  q x0, x1, x2, x3, x4, x5, x6, x7, x8;
  a2 = qp->args[2];
  qp = qp->next;
 name__integer_3_clear_reason:
  reasonp = reasons;
 name__integer_3_0:
 name__integer_3_1:
  if (!isint(a0)) goto name__integer_3_3;
  x0 = makeint(10L);
  gblt_less(a0,x0,name__integer_3_2);
  x1 = makeint(48L);
  bblt_add_no_check(x1,a0,x2);
  allocp[0] = a2;
  allocp[1] = x2;
  x3 = makecons(&allocp[0]);
  allocp += 2;
  unify_value(a1, x3);
  proceed();
 name__integer_3_2:
  x1 = makeint(10L);
  gblt_greater_or_eq(a0,x1,name__integer_3_interrupt);
  x2 = makeint(10L);
  bblt_div_no_check(a0,x2,x3);
  x4 = makeint(10L);
  bblt_mod_no_check(a0,x4,x5);
  x6 = makeint(48L);
  bblt_add_no_check(x5,x6,x7);
  allocp[0] = a2;
  allocp[1] = x7;
  x8 = makecons(&allocp[0]);
  a0 = x3;
  a2 = x8;
  allocp += 2;
  execute(name__integer_3_0);
  goto name__integer_3_ext_interrupt;
 name__integer_3_3:
  if (!isref(a0)) goto name__integer_3_interrupt;
  deref_and_jump(a0,name__integer_3_1);
  *reasonp++ =  a0;
  goto name__integer_3_interrupt;
 name__integer_3_ext_interrupt:
  reasonp = 0l;
 name__integer_3_interrupt:
  toppred = &predicate_klic__comp__util_xname__integer_3;
  goto interrupt_3;
 }

 string__to__char__list_2_top: {
  q x0, x1;
  qp = qp->next;
 string__to__char__list_2_clear_reason:
  reasonp = reasons;
 string__to__char__list_2_0:
 string__to__char__list_2_1:
  switch (ptagof(a0)) {
 case FUNCTOR:
 string__to__char__list_2_2:
  if (!isgobj(a0)) goto string__to__char__list_2_interrupt;
 string__to__char__list_2_3:
  gblt_is_string(a0,string__to__char__list_2_interrupt);
  gblt_size_of_string(a0,x0,string__to__char__list_2_interrupt);
  gblt_element_size_of_string(a0,x1,string__to__char__list_2_interrupt);
 string__to__char__list_2_4:
  if (x1 != makeint(8L)) goto string__to__char__list_2_interrupt;
  a2 = NILATOM;
  a3 = x0;
  execute(string__to__char__list_4_0);
  goto string__to__char__list_4_ext_interrupt;
 case VARREF:
  deref_and_jump(a0,string__to__char__list_2_1);
  *reasonp++ =  a0;
  goto string__to__char__list_2_interrupt;
  };
  goto string__to__char__list_2_interrupt;
 string__to__char__list_2_ext_interrupt:
  reasonp = 0l;
 string__to__char__list_2_interrupt:
  goto interrupt_2;
 }

 string__to__char__list_4_top: {
  q x0, x1, x2, x3, x4, x5;
  a2 = qp->args[2];
  a3 = qp->args[3];
  qp = qp->next;
 string__to__char__list_4_clear_reason:
  reasonp = reasons;
 string__to__char__list_4_0:
 string__to__char__list_4_1:
  if (!isint(a3)) goto string__to__char__list_4_3;
  x0 = makeint(0L);
  gblt_less_or_eq(a3,x0,string__to__char__list_4_2);
  unify(a1, a2);
  proceed();
 string__to__char__list_4_2:
  x1 = makeint(0L);
  gblt_greater(a3,x1,string__to__char__list_4_interrupt);
  x2 = makeint(1L);
  bblt_sub_no_check(a3,x2,x3);
  generic_arg[0] = x3;
  allocp[0] = x4 = makeref(&allocp[0]);
  generic_arg[1] = x4;
  call_generic(a0, functor_element_2, 1);
  allocp[0] = a2;
  allocp[1] = x4;
  x5 = makecons(&allocp[0]);
  a2 = x5;
  a3 = x3;
  allocp += 2;
  execute(string__to__char__list_4_0);
  goto string__to__char__list_4_ext_interrupt;
 string__to__char__list_4_3:
  if (!isref(a3)) goto string__to__char__list_4_interrupt;
  deref_and_jump(a3,string__to__char__list_4_1);
  *reasonp++ =  a3;
  goto string__to__char__list_4_interrupt;
 string__to__char__list_4_ext_interrupt:
  reasonp = 0l;
 string__to__char__list_4_interrupt:
  toppred = &predicate_klic__comp__util_xstring__to__char__list_4;
  goto interrupt_4;
 }

 numbervars_4_top: {
  q x0;
  a2 = qp->args[2];
  a3 = qp->args[3];
  qp = qp->next;
 numbervars_4_clear_reason:
  reasonp = reasons;
 numbervars_4_0:
  a4 = NILATOM;
  allocp[0] = x0 = makeref(&allocp[0]);
  a5 = x0;
  allocp += 1;
  execute(numbervars_6_0);
  goto numbervars_6_ext_interrupt;
 numbervars_4_ext_interrupt:
  reasonp = 0l;
 numbervars_4_interrupt:
  goto interrupt_4;
 }

 numbervars_6_top: {
  q x0, x1, x2, x3, x4, x5;
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  qp = qp->next;
 numbervars_6_clear_reason:
  reasonp = reasons;
 numbervars_6_0:
 numbervars_6_1:
  switch (ptagof(a0)) {
 case FUNCTOR:
  if (functor_of(a0) != makesym(functor_variable_1)) goto numbervars_6_6;
  x0 = arg(a0, 0);
 numbervars_6_2:
  if (isref(x0)) goto numbervars_6_5;
 numbervars_6_3:
  if (isref(x0)) goto numbervars_6_4;
  x1 = makefunctor(&string_const_0);
  if_not_equal(x0, x1, numbervars_6_6);
  unify(a5, a4);
  allocp[0] = makesym(functor_variable_1);
  allocp[1] = a2;
  x2 = makefunctor(&allocp[0]);
  allocp += 2;
  unify_value(a1, x2);
  x3 = makeint(1L);
  bblt_add_no_check_y(a2,x3,x4);
  unify(a3, x4);
  proceed();
 numbervars_6_4:
  deref_and_jump(x0,numbervars_6_3);
  *reasonp++ =  x0;
  goto numbervars_6_6;
 numbervars_6_5:
  deref_and_jump(x0,numbervars_6_2);
  *reasonp++ =  x0;
  goto numbervars_6_6;
 case VARREF:
  deref_and_jump(a0,numbervars_6_1);
  *reasonp++ =  a0;
  goto numbervars_6_6;
  };
  goto numbervars_6_6;
 numbervars_6_6:
  otherwise(numbervars_6_interrupt);
 numbervars_6_7:
  switch (ptagof(a0)) {
 case FUNCTOR:
  switch (symval(functor_of(a0))) {
 case functor_variable_1:
  x0 = arg(a0, 0);
 numbervars_6_8:
  switch (ptagof(x0)) {
 case FUNCTOR:
 numbervars_6_9:
  if (!isgobj(x0)) goto numbervars_6_13;
 numbervars_6_10:
  gblt_is_string(x0,numbervars_6_13);
  gblt_size_of_string(x0,x1,numbervars_6_13);
  gblt_element_size_of_string(x0,x2,numbervars_6_13);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__util_xnumbervars__1_7);
  allocp[2] = x3 = makeref(&allocp[2]);
  allocp[3] = a1;
  allocp[4] = x0;
  allocp[5] = a2;
  allocp[6] = a3;
  allocp[7] = a4;
  allocp[8] = a5;
  a0 = a4;
  a1 = x0;
  a2 = x3;
  qp = (struct goalrec*)&allocp[0];
  allocp += 9;
  execute(assoc_3_0);
  goto assoc_3_ext_interrupt;
 case VARREF:
  deref_and_jump(x0,numbervars_6_8);
  *reasonp++ =  x0;
  goto numbervars_6_13;
  };
  goto numbervars_6_13;
 case functor_floating__point_1:
  x0 = arg(a0, 0);
 numbervars_6_11:
  switch (ptagof(x0)) {
 case FUNCTOR:
 numbervars_6_12:
  if (!isgobj(x0)) goto numbervars_6_13;
  if (!isclass(x0,float)) goto numbervars_6_13;
  gblt_float(x0,numbervars_6_13);
  unify2(a5, a4, a3, a2);
  allocp[0] = makesym(functor_float_1);
  allocp[1] = x0;
  x1 = makefunctor(&allocp[0]);
  allocp += 2;
  unify_value(a1, x1);
  proceed();
 case VARREF:
  deref_and_jump(x0,numbervars_6_11);
  *reasonp++ =  x0;
  goto numbervars_6_13;
  };
  goto numbervars_6_13;
  };
  goto numbervars_6_13;
 case VARREF:
  deref_and_jump(a0,numbervars_6_7);
  *reasonp++ =  a0;
  goto numbervars_6_13;
  };
  goto numbervars_6_13;
 numbervars_6_13:
  otherwise(numbervars_6_interrupt);
 numbervars_6_14:
  switch (ptagof(a0)) {
 case FUNCTOR:
 numbervars_6_15:
  if (!isgobj(a0)) goto numbervars_6_16;
  if (!isclass(a0,vector)) goto numbervars_6_16;
  gblt_size_of_vector(a0,x0,numbervars_6_16);
  x1 = a0;
  x2 = a1;
  x3 = a2;
  x4 = a3;
  x5 = a4;
  a6 = a5;
  a5 = x5;
  a4 = x4;
  a3 = x3;
  a2 = x2;
  a1 = x1;
  a0 = x0;
  execute(numbervars__vector_7_0);
  goto numbervars__vector_7_ext_interrupt;
 case VARREF:
  deref_and_jump(a0,numbervars_6_14);
  *reasonp++ =  a0;
  goto numbervars_6_16;
  };
  goto numbervars_6_16;
 numbervars_6_16:
  otherwise(numbervars_6_interrupt);
 numbervars_6_17:
  if (isref(a0)) goto numbervars_6_18;
  gblt_pfunctor(a0,x0,numbervars_6_interrupt);
  gblt_arity(a0,x1,numbervars_6_interrupt);
  x2 = a0;
  x3 = a1;
  x4 = a2;
  a6 = a3;
  a7 = a4;
  a8 = a5;
  a5 = x4;
  a4 = x3;
  a3 = NILATOM;
  a2 = x0;
  a1 = x2;
  a0 = x1;
  execute(numbervars__funct_9_0);
  goto numbervars__funct_9_ext_interrupt;
 numbervars_6_18:
  deref_and_jump(a0,numbervars_6_17);
  *reasonp++ =  a0;
  goto numbervars_6_interrupt;
 numbervars_6_ext_interrupt:
  reasonp = 0l;
 numbervars_6_interrupt:
  toppred = &predicate_klic__comp__util_xnumbervars_6;
  goto interrupt_6;
 }

 numbervars__1_7_top: {
  q x0, x1, x2, x3, x4;
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  a6 = qp->args[6];
  qp = qp->next;
 numbervars__1_7_clear_reason:
  reasonp = reasons;
 numbervars__1_7_0:
 numbervars__1_7_1:
  switch (ptagof(a0)) {
 case ATOMIC:
  if (a0 != makesym(atom_no)) goto numbervars__1_7_interrupt;
  allocp[0] = makesym(functor_variable_1);
  allocp[1] = a3;
  x0 = makefunctor(&allocp[0]);
  allocp += 2;
  unify_value(a1, x0);
  allocp[0] = makesym(functor__3D_2);
  allocp[1] = a2;
  allocp[2] = a3;
  x1 = makefunctor(&allocp[0]);
  allocp[3] = a5;
  allocp[4] = x1;
  x2 = makecons(&allocp[3]);
  allocp += 5;
  unify_value(a6, x2);
  x3 = makeint(1L);
  bblt_add_no_check_y(a3,x3,x4);
  unify(a4, x4);
  proceed();
 case FUNCTOR:
  if (functor_of(a0) != makesym(functor_yes_1)) goto numbervars__1_7_interrupt;
  unify2(a6, a5, a4, a3);
  allocp[0] = makesym(functor_variable_1);
  x1 = arg(a0, 0);
  allocp[1] = x1;
  x0 = makefunctor(&allocp[0]);
  allocp += 2;
  unify_value(a1, x0);
  proceed();
 case VARREF:
  deref_and_jump(a0,numbervars__1_7_1);
  *reasonp++ =  a0;
  goto numbervars__1_7_interrupt;
  };
  goto numbervars__1_7_interrupt;
 numbervars__1_7_ext_interrupt:
  reasonp = 0l;
 numbervars__1_7_interrupt:
  goto interrupt_7;
 }

 numbervars__vector_7_top: {
  q x0, x1, x2, x3, x4, x5, x6, x7, x8;
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  a6 = qp->args[6];
  qp = qp->next;
 numbervars__vector_7_clear_reason:
  reasonp = reasons;
 numbervars__vector_7_0:
 numbervars__vector_7_1:
  if (!isint(a0)) goto numbervars__vector_7_3;
  x0 = makeint(0L);
  gblt_less_or_eq(a0,x0,numbervars__vector_7_2);
  unify3(a6, a5, a4, a3, a2, a1);
  proceed();
 numbervars__vector_7_2:
  x1 = makeint(0L);
  gblt_greater(a0,x1,numbervars__vector_7_interrupt);
  x3 = makeint(1L);
  gblt_subtract(a0,x3,x2,numbervars__vector_7_interrupt);
  generic_arg[0] = x2;
  allocp[0] = x4 = makeref(&allocp[0]);
  generic_arg[1] = x4;
  allocp[1] = x5 = makeref(&allocp[1]);
  generic_arg[2] = x5;
  allocp[2] = x6 = makeref(&allocp[2]);
  generic_arg[3] = x6;
  call_generic(a1, functor_set__element_4, 3);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__util_xnumbervars__vector_7);
  allocp[2] = x2;
  allocp[3] = x6;
  allocp[4] = a2;
  allocp[5] = x7 = makeref(&allocp[5]);
  allocp[6] = a4;
  allocp[7] = x8 = makeref(&allocp[7]);
  allocp[8] = a6;
  a0 = x4;
  a1 = x5;
  a2 = a3;
  a3 = x7;
  a4 = a5;
  a5 = x8;
  qp = (struct goalrec*)&allocp[0];
  allocp += 9;
  execute(numbervars_6_0);
  goto numbervars_6_ext_interrupt;
 numbervars__vector_7_3:
  if (!isref(a0)) goto numbervars__vector_7_interrupt;
  deref_and_jump(a0,numbervars__vector_7_1);
  *reasonp++ =  a0;
  goto numbervars__vector_7_interrupt;
 numbervars__vector_7_ext_interrupt:
  reasonp = 0l;
 numbervars__vector_7_interrupt:
  toppred = &predicate_klic__comp__util_xnumbervars__vector_7;
  goto interrupt_7;
 }

 numbervars__funct_9_top: {
  q x0, x1, x2, x3, x4, x5, x6, x7, x8;
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  a6 = qp->args[6];
  a7 = qp->args[7];
  a8 = qp->args[8];
  qp = qp->next;
 numbervars__funct_9_clear_reason:
  reasonp = reasons;
 numbervars__funct_9_0:
 numbervars__funct_9_1:
  if (!isint(a0)) goto numbervars__funct_9_5;
  x0 = makeint(0L);
  gblt_less_or_eq(a0,x0,numbervars__funct_9_2);
  unify2(a8, a7, a6, a5);
  allocp[0] = a3;
  allocp[1] = a2;
  x1 = makecons(&allocp[0]);
  allocp[2] = (q)qp;
  allocp[3] = (q)(&predicate_functor__table_x_3D_2E_2E_2);
  allocp[4] = a4;
  allocp[5] = x1;
  qp = (struct goalrec*)&allocp[2];
  allocp += 6;
  proceed();
 numbervars__funct_9_2:
  x1 = makeint(0L);
  gblt_greater_or_eq(a0,x1,numbervars__funct_9_interrupt);
 numbervars__funct_9_3:
  if (isref(a1)) goto numbervars__funct_9_4;
  gblt_arg(a0,a1,x2,numbervars__funct_9_interrupt);
  x3 = makeint(1L);
  bblt_sub_no_check(a0,x3,x4);
  allocp[0] = a3;
  allocp[1] = x6 = makeref(&allocp[1]);
  x5 = makecons(&allocp[0]);
  allocp[2] = (q)qp;
  allocp[3] = (q)(&predicate_klic__comp__util_xnumbervars__funct_9);
  allocp[4] = x4;
  allocp[5] = a1;
  allocp[6] = a2;
  allocp[7] = x5;
  allocp[8] = a4;
  allocp[9] = x7 = makeref(&allocp[9]);
  allocp[10] = a6;
  allocp[11] = x8 = makeref(&allocp[11]);
  allocp[12] = a8;
  a0 = x2;
  a1 = x6;
  a2 = a5;
  a3 = x7;
  a4 = a7;
  a5 = x8;
  qp = (struct goalrec*)&allocp[2];
  allocp += 13;
  execute(numbervars_6_0);
  goto numbervars_6_ext_interrupt;
 numbervars__funct_9_4:
  deref_and_jump(a1,numbervars__funct_9_3);
  *reasonp++ =  a1;
  goto numbervars__funct_9_interrupt;
 numbervars__funct_9_5:
  if (!isref(a0)) goto numbervars__funct_9_interrupt;
  deref_and_jump(a0,numbervars__funct_9_1);
  *reasonp++ =  a0;
  goto numbervars__funct_9_interrupt;
 numbervars__funct_9_ext_interrupt:
  reasonp = 0l;
 numbervars__funct_9_interrupt:
  toppred = &predicate_klic__comp__util_xnumbervars__funct_9;
  goto interrupt_9;
 }

 make__atom_2_top: {
  q x0, x1;
  qp = qp->next;
 make__atom_2_clear_reason:
  reasonp = reasons;
 make__atom_2_0:
  generic_arg[0] = a0;
  x0 = makeint(8L);
  generic_arg[1] = x0;
  new_generic(string_g_new, 2, x1, 0);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_atom__table_xmake__atom_2);
  allocp[2] = x1;
  allocp[3] = a1;
  qp = (struct goalrec*)&allocp[0];
  allocp += 4;
  proceed();
 make__atom_2_ext_interrupt:
  reasonp = 0l;
 make__atom_2_interrupt:
  goto interrupt_2;
 }
 interrupt_9:
  allocp[10] = a8;
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
  loop_within_module(module_klic__comp__util);
}
