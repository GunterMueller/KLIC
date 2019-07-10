/* Compiled by KLIC compiler version 3.003 (Mon Mar 29 19:42:59 JST 1999) */
#include <klic/klichdr.h>
#include "atom.h"
#include "funct.h"

module module_klic__comp__insert();
Const struct predicate predicate_klic__comp__insert_xmake__index__tree_5 =
   { module_klic__comp__insert, 0, 5 };
Const struct predicate predicate_klic__comp__insert_xmake__index__tree__1_7 =
   { module_klic__comp__insert, 1, 7 };
Const struct predicate predicate_klic__comp__insert_xins__index_6 =
   { module_klic__comp__insert, 2, 6 };
Const struct predicate predicate_klic__comp__insert_xins__index__check__and__stop_7 =
   { module_klic__comp__insert, 3, 7 };
Const struct predicate predicate_klic__comp__insert_xins__index__check__and__go_7 =
   { module_klic__comp__insert, 4, 7 };
Const struct predicate predicate_klic__comp__insert_xins__index__1_6 =
   { module_klic__comp__insert, 5, 6 };
Const struct predicate predicate_klic__comp__insert_xins__pattern_9 =
   { module_klic__comp__insert, 6, 9 };
Const struct predicate predicate_klic__comp__insert_xins__value__node_12 =
   { module_klic__comp__insert, 7, 12 };
Const struct predicate predicate_klic__comp__insert_xpattern__kind_2 =
   { module_klic__comp__insert, 8, 2 };
Const struct predicate predicate_klic__comp__insert_xins__eq_7 =
   { module_klic__comp__insert, 9, 7 };
Const struct predicate predicate_klic__comp__insert_xins__atomic__node_6 =
   { module_klic__comp__insert, 10, 6 };
Const struct predicate predicate_klic__comp__insert_xins__funct__node_6 =
   { module_klic__comp__insert, 11, 6 };
Const struct predicate predicate_klic__comp__insert_xins__merge__case_5 =
   { module_klic__comp__insert, 12, 5 };
Const struct predicate predicate_klic__comp__insert_xins__gb_6 =
   { module_klic__comp__insert, 13, 6 };
Const struct predicate predicate_klic__comp__insert_xsubsumed_4 =
   { module_klic__comp__insert, 14, 4 };
Const struct predicate predicate_klic__comp__insert_xsubsumed__1_5 =
   { module_klic__comp__insert, 15, 5 };
Const struct predicate predicate_klic__comp__insert_xsubsumed__2_3 =
   { module_klic__comp__insert, 16, 3 };
Const struct predicate predicate_klic__comp__insert_xsubsumed__type_3 =
   { module_klic__comp__insert, 17, 3 };
Const struct predicate predicate_klic__comp__insert_xins__redundant_1 =
   { module_klic__comp__insert, 18, 1 };
extern Const struct predicate predicate_klic__comp__message_xwarning_2;
extern Const struct predicate predicate_klic__comp__util_xassoc_3;

module module_klic__comp__insert(glbl, qp, allocp, toppred)
  struct global_variables *glbl;
  struct goalrec *qp;
  register q *allocp;
  Const struct predicate *toppred;
{
  static Const unsigned char strconst_body_0[24] =
    "Clause deleted: ~w/~d";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_0 =
      declare_string_constant_8(strconst_body_0,21);
  static Const unsigned char strconst_body_1[16] =
    {'R','e','d','u','n','d','a','n','t',' ','c','l','a','u','s','e',};
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_1 =
      declare_string_constant_8(strconst_body_1,16);
  static Const q funct_const_2[] = {
    makesym(functor_a_5),
    NILATOM,
    makesym(atom_e),
    NILATOM,
    makesym(atom_e),
    makesym(atom_e),
  };
  static Const q funct_const_3[] = {
    makesym(functor_f_3),
    NILATOM,
    makesym(atom_e),
    makesym(atom_e),
  };
  q a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11;

  q *reasonp;
 module_top:
  a0 = qp->args[0];
  switch_on_pred() {
    case_pred(0, make__index__tree_5_top);
    case_pred(1, make__index__tree__1_7_top);
    case_pred(2, ins__index_6_top);
    case_pred(3, ins__index__check__and__stop_7_top);
    case_pred(4, ins__index__check__and__go_7_top);
    case_pred(5, ins__index__1_6_top);
    case_pred(6, ins__pattern_9_top);
    case_pred(7, ins__value__node_12_top);
    case_pred(8, pattern__kind_2_top);
    case_pred(9, ins__eq_7_top);
    case_pred(10, ins__atomic__node_6_top);
    case_pred(11, ins__funct__node_6_top);
    case_pred(12, ins__merge__case_5_top);
    case_pred(13, ins__gb_6_top);
    case_pred(14, subsumed_4_top);
    case_pred(15, subsumed__1_5_top);
    case_pred(16, subsumed__2_3_top);
    case_pred(17, subsumed__type_3_top);
    last_case_pred(18, ins__redundant_1_top);
  }

 make__index__tree_5_top: {
  q x0, x1, x2, x3, x4;
  a1 = qp->args[1];
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  qp = qp->next;
 make__index__tree_5_clear_reason:
  reasonp = reasons;
 make__index__tree_5_0:
 make__index__tree_5_1:
  switch (ptagof(a0)) {
 case CONS:
  x0 = car_of(a0);
 make__index__tree_5_2:
  switch (ptagof(x0)) {
 case ATOMIC:
  if (isint(x0)) goto make__index__tree_5_interrupt;
  switch (symval(x0)) {
 case symval(makesym(atom_otherwise)):
  allocp[0] = makesym(functor_other_2);
  allocp[1] = a2;
  allocp[2] = x2 = makeref(&allocp[2]);
  x1 = makefunctor(&allocp[0]);
  allocp += 3;
  unify_value(a3, x1);
  a0 = cdr_of(a0);
  a2 = makesym(atom_e);
  a3 = x2;
  execute(make__index__tree_5_0);
  goto make__index__tree_5_ext_interrupt;
 case symval(makesym(atom_alternatively)):
  allocp[0] = makesym(functor_alter_2);
  allocp[1] = a2;
  allocp[2] = x2 = makeref(&allocp[2]);
  x1 = makefunctor(&allocp[0]);
  allocp += 3;
  unify_value(a3, x1);
  a0 = cdr_of(a0);
  a2 = makesym(atom_e);
  a3 = x2;
  execute(make__index__tree_5_0);
  goto make__index__tree_5_ext_interrupt;
  };
  goto make__index__tree_5_interrupt;
 case FUNCTOR:
  if (functor_of(x0) != makesym(functor__2D_2)) goto make__index__tree_5_interrupt;
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__insert_xmake__index__tree__1_7);
  allocp[2] = x1 = makeref(&allocp[2]);
  x2 = cdr_of(a0);
  allocp[3] = x2;
  allocp[4] = a1;
  allocp[5] = a2;
  allocp[6] = x3 = makeref(&allocp[6]);
  allocp[7] = a3;
  allocp[8] = a4;
  a0 = arg(x0, 0);
  x4 = a2;
  a2 = a1;
  a3 = arg(x0, 1);
  a4 = x3;
  a5 = x1;
  a1 = x4;
  qp = (struct goalrec*)&allocp[0];
  allocp += 9;
  execute(ins__index_6_0);
  goto ins__index_6_ext_interrupt;
 case VARREF:
  deref_and_jump(x0,make__index__tree_5_2);
  *reasonp++ =  x0;
  goto make__index__tree_5_interrupt;
  };
  goto make__index__tree_5_interrupt;
 case ATOMIC:
  if (a0 != NILATOM) goto make__index__tree_5_interrupt;
  unify(a3, a2);
  proceed();
 case VARREF:
  deref_and_jump(a0,make__index__tree_5_1);
  *reasonp++ =  a0;
  goto make__index__tree_5_interrupt;
  };
  goto make__index__tree_5_interrupt;
 make__index__tree_5_ext_interrupt:
  reasonp = 0l;
 make__index__tree_5_interrupt:
  toppred = &predicate_klic__comp__insert_xmake__index__tree_5;
  goto interrupt_5;
 }

 make__index__tree__1_7_top: {
  q x0, x1, x2, x3;
  a1 = qp->args[1];
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  a6 = qp->args[6];
  qp = qp->next;
 make__index__tree__1_7_clear_reason:
  reasonp = reasons;
 make__index__tree__1_7_0:
 make__index__tree__1_7_1:
  switch (ptagof(a0)) {
 case ATOMIC:
  if (isint(a0)) goto make__index__tree__1_7_interrupt;
  switch (symval(a0)) {
 case symval(makesym(atom_normal)):
  a0 = a1;
  a1 = a2;
  a2 = a4;
  a3 = a5;
  a4 = a6;
  execute(make__index__tree_5_0);
  goto make__index__tree_5_ext_interrupt;
 case symval(makesym(atom_abnormal)):
 make__index__tree__1_7_2:
  switch (ptagof(a6)) {
 case FUNCTOR:
  if (functor_of(a6) != makesym(functor__2F_2)) goto make__index__tree__1_7_interrupt;
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__insert_xmake__index__tree_5);
  allocp[2] = a1;
  allocp[3] = a2;
  allocp[4] = a3;
  allocp[5] = a5;
  allocp[6] = a6;
  allocp[7] = NILATOM;
  x1 = arg(a6, 1);
  allocp[8] = x1;
  x0 = makecons(&allocp[7]);
  allocp[9] = x0;
  x3 = arg(a6, 0);
  allocp[10] = x3;
  x2 = makecons(&allocp[9]);
  allocp[11] = (q)(struct goalrec*)&allocp[0];
  allocp[12] = (q)(&predicate_klic__comp__message_xwarning_2);
  allocp[13] = makefunctor(&string_const_0);
  allocp[14] = x2;
  qp = (struct goalrec*)&allocp[11];
  allocp += 15;
  proceed();
 case VARREF:
  deref_and_jump(a6,make__index__tree__1_7_2);
  *reasonp++ =  a6;
  goto make__index__tree__1_7_interrupt;
  };
  goto make__index__tree__1_7_interrupt;
  };
  goto make__index__tree__1_7_interrupt;
 case VARREF:
  deref_and_jump(a0,make__index__tree__1_7_1);
  *reasonp++ =  a0;
  goto make__index__tree__1_7_interrupt;
  };
  goto make__index__tree__1_7_interrupt;
 make__index__tree__1_7_ext_interrupt:
  reasonp = 0l;
 make__index__tree__1_7_interrupt:
  goto interrupt_7;
 }

 ins__index_6_top: {
  q x0, x1, x2, x3, x4, x5, x6;
  a1 = qp->args[1];
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  qp = qp->next;
 ins__index_6_clear_reason:
  reasonp = reasons;
 ins__index_6_0:
 ins__index_6_1:
  switch (ptagof(a0)) {
 case CONS:
  x0 = car_of(a0);
 ins__index_6_2:
  switch (ptagof(x0)) {
 case FUNCTOR:
  switch (symval(functor_of(x0))) {
 case functor__3B_2:
  x1 = cdr_of(a0);
 ins__index_6_3:
  switch (ptagof(x1)) {
 case ATOMIC:
  if (x1 != NILATOM) goto ins__index_6_6;
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__insert_xins__index__check__and__stop_7);
  x2 = arg(x0, 1);
  allocp[2] = x2;
  allocp[3] = x3 = makeref(&allocp[3]);
  allocp[4] = a2;
  allocp[5] = a3;
  allocp[6] = a4;
  allocp[7] = x4 = makeref(&allocp[7]);
  allocp[8] = a5;
  a0 = arg(x0, 0);
  a4 = x3;
  a5 = x4;
  qp = (struct goalrec*)&allocp[0];
  allocp += 9;
  execute(ins__index_6_0);
  goto ins__index_6_ext_interrupt;
 case VARREF:
  deref_and_jump(x1,ins__index_6_3);
  *reasonp++ =  x1;
  goto ins__index_6_6;
  };
  goto ins__index_6_6;
 case functor__3A_2:
  allocp[0] = makesym(functor__3A_2);
  x2 = arg(x0, 0);
  allocp[1] = x2;
  x3 = arg(x0, 1);
  allocp[2] = x3;
  x1 = makefunctor(&allocp[0]);
  x5 = cdr_of(a0);
  allocp[3] = x5;
  allocp[4] = x1;
  x4 = makecons(&allocp[3]);
  allocp[5] = (q)qp;
  allocp[6] = (q)(&predicate_klic__comp__insert_xins__index__check__and__go_7);
  allocp[7] = x4;
  allocp[8] = a1;
  allocp[9] = a2;
  allocp[10] = a3;
  allocp[11] = a4;
  allocp[12] = x6 = makeref(&allocp[12]);
  allocp[13] = a5;
  a0 = arg(x0, 1);
  a1 = arg(x0, 0);
  a3 = x6;
  qp = (struct goalrec*)&allocp[5];
  allocp += 14;
  execute(subsumed_4_0);
  goto subsumed_4_ext_interrupt;
  };
  goto ins__index_6_6;
 case VARREF:
  deref_and_jump(x0,ins__index_6_2);
  *reasonp++ =  x0;
  goto ins__index_6_6;
  };
  goto ins__index_6_6;
 case ATOMIC:
  if (a0 != NILATOM) goto ins__index_6_6;
 ins__index_6_4:
  switch (ptagof(a1)) {
 case ATOMIC:
  if (a1 != makesym(atom_e)) goto ins__index_6_5;
  x0 = makesym(atom_normal);
  unify_value(a5, x0);
  allocp[0] = makesym(functor_l_1);
  allocp[1] = a3;
  x1 = makefunctor(&allocp[0]);
  allocp += 2;
  unify_value(a4, x1);
  proceed();
 case VARREF:
  deref_and_jump(a1,ins__index_6_4);
  *reasonp++ =  a1;
  goto ins__index_6_6;
  };
  goto ins__index_6_5;
 ins__index_6_5:
  x0 = makesym(atom_e);
  gblt_diff(a1,x0,ins__index_6_6);
  allocp[0] = makesym(functor_l_1);
  allocp[1] = a3;
  x1 = makefunctor(&allocp[0]);
  allocp += 2;
  unify_value(a4, x1);
  a0 = a5;
  execute(ins__redundant_1_0);
  goto ins__redundant_1_ext_interrupt;
 case VARREF:
  deref_and_jump(a0,ins__index_6_1);
  *reasonp++ =  a0;
  goto ins__index_6_6;
  };
  goto ins__index_6_6;
 ins__index_6_6:
  otherwise(ins__index_6_interrupt);
  execute(ins__index__1_6_0);
  goto ins__index__1_6_ext_interrupt;
 ins__index_6_ext_interrupt:
  reasonp = 0l;
 ins__index_6_interrupt:
  toppred = &predicate_klic__comp__insert_xins__index_6;
  goto interrupt_6;
 }

 ins__index__check__and__stop_7_top: {
  a1 = qp->args[1];
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  a6 = qp->args[6];
  qp = qp->next;
 ins__index__check__and__stop_7_clear_reason:
  reasonp = reasons;
 ins__index__check__and__stop_7_0:
 ins__index__check__and__stop_7_1:
  switch (ptagof(a5)) {
 case ATOMIC:
  if (isint(a5)) goto ins__index__check__and__stop_7_interrupt;
  switch (symval(a5)) {
 case symval(makesym(atom_normal)):
  a5 = a6;
  execute(ins__index_6_0);
  goto ins__index_6_ext_interrupt;
 case symval(makesym(atom_abnormal)):
  unify(a4, a1);
  unify_value(a6, a5);
  proceed();
  };
  goto ins__index__check__and__stop_7_interrupt;
 case VARREF:
  deref_and_jump(a5,ins__index__check__and__stop_7_1);
  *reasonp++ =  a5;
  goto ins__index__check__and__stop_7_interrupt;
  };
  goto ins__index__check__and__stop_7_interrupt;
 ins__index__check__and__stop_7_ext_interrupt:
  reasonp = 0l;
 ins__index__check__and__stop_7_interrupt:
  goto interrupt_7;
 }

 ins__index__check__and__go_7_top: {
  a1 = qp->args[1];
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  a6 = qp->args[6];
  qp = qp->next;
 ins__index__check__and__go_7_clear_reason:
  reasonp = reasons;
 ins__index__check__and__go_7_0:
 ins__index__check__and__go_7_1:
  switch (ptagof(a5)) {
 case ATOMIC:
  if (isint(a5)) goto ins__index__check__and__go_7_interrupt;
  switch (symval(a5)) {
 case symval(makesym(atom_normal)):
 ins__index__check__and__go_7_2:
  switch (ptagof(a0)) {
 case CONS:
  a0 = cdr_of(a0);
  a5 = a6;
  execute(ins__index_6_0);
  goto ins__index_6_ext_interrupt;
 case VARREF:
  deref_and_jump(a0,ins__index__check__and__go_7_2);
  *reasonp++ =  a0;
  goto ins__index__check__and__go_7_interrupt;
  };
  goto ins__index__check__and__go_7_interrupt;
 case symval(makesym(atom_abnormal)):
  a5 = a6;
  execute(ins__index__1_6_0);
  goto ins__index__1_6_ext_interrupt;
  };
  goto ins__index__check__and__go_7_interrupt;
 case VARREF:
  deref_and_jump(a5,ins__index__check__and__go_7_1);
  *reasonp++ =  a5;
  goto ins__index__check__and__go_7_interrupt;
  };
  goto ins__index__check__and__go_7_interrupt;
 ins__index__check__and__go_7_ext_interrupt:
  reasonp = 0l;
 ins__index__check__and__go_7_interrupt:
  goto interrupt_7;
 }

 ins__index__1_6_top: {
  q x0, x1, x2, x3, x4, x5, x6, x7, x8, x9;
  a1 = qp->args[1];
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  qp = qp->next;
 ins__index__1_6_clear_reason:
  reasonp = reasons;
 ins__index__1_6_0:
 ins__index__1_6_1:
  switch (ptagof(a0)) {
 case CONS:
  x0 = car_of(a0);
 ins__index__1_6_2:
  switch (ptagof(x0)) {
 case FUNCTOR:
  switch (symval(functor_of(x0))) {
 case functor_gp_1:
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__insert_xins__index__check__and__stop_7);
  x1 = cdr_of(a0);
  allocp[2] = x1;
  allocp[3] = x2 = makeref(&allocp[3]);
  allocp[4] = a2;
  allocp[5] = a3;
  allocp[6] = x3 = makeref(&allocp[6]);
  allocp[7] = x4 = makeref(&allocp[7]);
  allocp[8] = a5;
  a0 = a1;
  a1 = x0;
  a2 = a4;
  a3 = x2;
  a4 = x3;
  a5 = x4;
  qp = (struct goalrec*)&allocp[0];
  allocp += 9;
  execute(ins__gb_6_0);
  goto ins__gb_6_ext_interrupt;
 case functor_gg_2:
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__insert_xins__index__check__and__stop_7);
  x1 = cdr_of(a0);
  allocp[2] = x1;
  allocp[3] = x2 = makeref(&allocp[3]);
  allocp[4] = a2;
  allocp[5] = a3;
  allocp[6] = x3 = makeref(&allocp[6]);
  allocp[7] = x4 = makeref(&allocp[7]);
  allocp[8] = a5;
  a0 = a1;
  a1 = x0;
  a2 = a4;
  a3 = x2;
  a4 = x3;
  a5 = x4;
  qp = (struct goalrec*)&allocp[0];
  allocp += 9;
  execute(ins__gb_6_0);
  goto ins__gb_6_ext_interrupt;
 case functor_il_3:
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__insert_xins__index__check__and__stop_7);
  x1 = cdr_of(a0);
  allocp[2] = x1;
  allocp[3] = x2 = makeref(&allocp[3]);
  allocp[4] = a2;
  allocp[5] = a3;
  allocp[6] = x3 = makeref(&allocp[6]);
  allocp[7] = x4 = makeref(&allocp[7]);
  allocp[8] = a5;
  a0 = a1;
  a1 = x0;
  a2 = a4;
  a3 = x2;
  a4 = x3;
  a5 = x4;
  qp = (struct goalrec*)&allocp[0];
  allocp += 9;
  execute(ins__gb_6_0);
  goto ins__gb_6_ext_interrupt;
 case functor_gb_2:
  allocp[0] = makesym(functor_gb_1);
  x2 = arg(x0, 0);
  allocp[1] = x2;
  x1 = makefunctor(&allocp[0]);
  allocp[2] = makesym(functor__3D_2);
  allocp[3] = x1;
  x4 = arg(x0, 1);
  allocp[4] = x4;
  x3 = makefunctor(&allocp[2]);
  allocp[5] = a2;
  allocp[6] = x3;
  x5 = makecons(&allocp[5]);
  allocp[7] = (q)qp;
  allocp[8] = (q)(&predicate_klic__comp__insert_xins__index__check__and__stop_7);
  x6 = cdr_of(a0);
  allocp[9] = x6;
  allocp[10] = x7 = makeref(&allocp[10]);
  allocp[11] = x5;
  allocp[12] = a3;
  allocp[13] = x8 = makeref(&allocp[13]);
  allocp[14] = x9 = makeref(&allocp[14]);
  allocp[15] = a5;
  a0 = a1;
  a1 = x0;
  a2 = a4;
  a3 = x7;
  a4 = x8;
  a5 = x9;
  qp = (struct goalrec*)&allocp[7];
  allocp += 16;
  execute(ins__gb_6_0);
  goto ins__gb_6_ext_interrupt;
 case functor__3A_2:
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__insert_xins__index__check__and__stop_7);
  x1 = cdr_of(a0);
  allocp[2] = x1;
  allocp[3] = x2 = makeref(&allocp[3]);
  allocp[4] = x3 = makeref(&allocp[4]);
  allocp[5] = a3;
  allocp[6] = x4 = makeref(&allocp[6]);
  allocp[7] = x5 = makeref(&allocp[7]);
  allocp[8] = a5;
  a0 = a1;
  a1 = arg(x0, 1);
  x6 = arg(x0, 0);
  a3 = a4;
  a4 = a2;
  a5 = x3;
  a6 = x2;
  a7 = x4;
  a8 = x5;
  a2 = x6;
  qp = (struct goalrec*)&allocp[0];
  allocp += 9;
  execute(ins__pattern_9_0);
  goto ins__pattern_9_ext_interrupt;
  };
  goto ins__index__1_6_interrupt;
 case VARREF:
  deref_and_jump(x0,ins__index__1_6_2);
  *reasonp++ =  x0;
  goto ins__index__1_6_interrupt;
  };
  goto ins__index__1_6_interrupt;
 case VARREF:
  deref_and_jump(a0,ins__index__1_6_1);
  *reasonp++ =  a0;
  goto ins__index__1_6_interrupt;
  };
  goto ins__index__1_6_interrupt;
 ins__index__1_6_ext_interrupt:
  reasonp = 0l;
 ins__index__1_6_interrupt:
  toppred = &predicate_klic__comp__insert_xins__index__1_6;
  goto interrupt_6;
 }

 ins__pattern_9_top: {
  q x0, x1, x2, x3, x4, x5, x6, x7, x8;
  a1 = qp->args[1];
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  a6 = qp->args[6];
  a7 = qp->args[7];
  a8 = qp->args[8];
  qp = qp->next;
 ins__pattern_9_clear_reason:
  reasonp = reasons;
 ins__pattern_9_0:
 ins__pattern_9_1:
  switch (ptagof(a0)) {
 case ATOMIC:
  if (a0 != makesym(atom_e)) goto ins__pattern_9_9;
  allocp[0] = makesym(functor__3D_2);
  allocp[1] = a2;
  allocp[2] = a1;
  x0 = makefunctor(&allocp[0]);
  allocp[3] = a4;
  allocp[4] = x0;
  x1 = makecons(&allocp[3]);
  allocp += 5;
  unify_value(a5, x1);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__insert_xins__value__node_12);
  allocp[2] = x2 = makeref(&allocp[2]);
  allocp[3] = a1;
  allocp[4] = a2;
  allocp[5] = makesym(atom_e);
  allocp[6] = makesym(atom_e);
  allocp[7] = makesym(atom_e);
  allocp[8] = makesym(atom_e);
  allocp[9] = makesym(atom_e);
  allocp[10] = a3;
  allocp[11] = a6;
  allocp[12] = a7;
  allocp[13] = a8;
  a0 = a1;
  a1 = x2;
  qp = (struct goalrec*)&allocp[0];
  allocp += 14;
  execute(pattern__kind_2_0);
  goto pattern__kind_2_ext_interrupt;
 case FUNCTOR:
  switch (symval(functor_of(a0))) {
 case functor_v_6:
  x0 = arg(a0, 0);
 ins__pattern_9_2:
  if (isref(x0)) goto ins__pattern_9_8;
 ins__pattern_9_3:
  if (isref(x0)) goto ins__pattern_9_7;
  if_not_equal(x0, a2, ins__pattern_9_4);
  allocp[0] = makesym(functor__3D_2);
  allocp[1] = x0;
  allocp[2] = a1;
  x1 = makefunctor(&allocp[0]);
  allocp[3] = a4;
  allocp[4] = x1;
  x2 = makecons(&allocp[3]);
  allocp += 5;
  unify_value(a5, x2);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__insert_xins__value__node_12);
  allocp[2] = x3 = makeref(&allocp[2]);
  allocp[3] = a1;
  allocp[4] = x0;
  x4 = arg(a0, 1);
  allocp[5] = x4;
  x5 = arg(a0, 2);
  allocp[6] = x5;
  x6 = arg(a0, 3);
  allocp[7] = x6;
  x7 = arg(a0, 4);
  allocp[8] = x7;
  x8 = arg(a0, 5);
  allocp[9] = x8;
  allocp[10] = a3;
  allocp[11] = a6;
  allocp[12] = a7;
  allocp[13] = a8;
  a0 = a1;
  a1 = x3;
  qp = (struct goalrec*)&allocp[0];
  allocp += 14;
  execute(pattern__kind_2_clear_reason);
  goto pattern__kind_2_ext_interrupt;
 ins__pattern_9_4:
 ins__pattern_9_5:
  if (isref(x0)) goto ins__pattern_9_6;
  if_not_equal(x0, a1, ins__pattern_9_9);
  allocp[0] = makesym(functor__3D_2);
  allocp[1] = x0;
  allocp[2] = a2;
  x1 = makefunctor(&allocp[0]);
  allocp[3] = a4;
  allocp[4] = x1;
  x2 = makecons(&allocp[3]);
  allocp += 5;
  unify_value(a5, x2);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__insert_xins__value__node_12);
  allocp[2] = x3 = makeref(&allocp[2]);
  allocp[3] = a2;
  allocp[4] = x0;
  x4 = arg(a0, 1);
  allocp[5] = x4;
  x5 = arg(a0, 2);
  allocp[6] = x5;
  x6 = arg(a0, 3);
  allocp[7] = x6;
  x7 = arg(a0, 4);
  allocp[8] = x7;
  x8 = arg(a0, 5);
  allocp[9] = x8;
  allocp[10] = a3;
  allocp[11] = a6;
  allocp[12] = a7;
  allocp[13] = a8;
  a0 = a2;
  a1 = x3;
  qp = (struct goalrec*)&allocp[0];
  allocp += 14;
  execute(pattern__kind_2_clear_reason);
  goto pattern__kind_2_ext_interrupt;
 ins__pattern_9_6:
  deref_and_jump(x0,ins__pattern_9_5);
  *reasonp++ =  x0;
  goto ins__pattern_9_9;
 ins__pattern_9_7:
  deref_and_jump(x0,ins__pattern_9_3);
  *reasonp++ =  x0;
  goto ins__pattern_9_9;
 ins__pattern_9_8:
  deref_and_jump(x0,ins__pattern_9_2);
  *reasonp++ =  x0;
  goto ins__pattern_9_9;
 case functor_x_3:
  allocp[0] = makesym(functor_x_3);
  x1 = arg(a0, 0);
  allocp[1] = x1;
  x2 = arg(a0, 1);
  allocp[2] = x2;
  allocp[3] = x3 = makeref(&allocp[3]);
  x0 = makefunctor(&allocp[0]);
  allocp += 4;
  unify_value(a3, x0);
  a0 = arg(a0, 2);
  a3 = x3;
  execute(ins__pattern_9_0);
  goto ins__pattern_9_ext_interrupt;
 case functor_l_1:
  a0 = a8;
  execute(ins__redundant_1_0);
  goto ins__redundant_1_ext_interrupt;
  };
  goto ins__pattern_9_9;
 case VARREF:
  deref_and_jump(a0,ins__pattern_9_1);
  *reasonp++ =  a0;
  goto ins__pattern_9_9;
  };
  goto ins__pattern_9_9;
 ins__pattern_9_9:
  otherwise(ins__pattern_9_interrupt);
 ins__pattern_9_10:
  switch (ptagof(a0)) {
 case FUNCTOR:
  if (functor_of(a0) != makesym(functor_v_6)) goto ins__pattern_9_interrupt;
  allocp[0] = makesym(functor_v_6);
  x1 = arg(a0, 0);
  allocp[1] = x1;
  x2 = arg(a0, 1);
  allocp[2] = x2;
  x3 = arg(a0, 2);
  allocp[3] = x3;
  x4 = arg(a0, 3);
  allocp[4] = x4;
  x5 = arg(a0, 4);
  allocp[5] = x5;
  allocp[6] = x6 = makeref(&allocp[6]);
  x0 = makefunctor(&allocp[0]);
  allocp += 7;
  unify_value(a3, x0);
  a0 = arg(a0, 5);
  a3 = x6;
  execute(ins__pattern_9_0);
  goto ins__pattern_9_ext_interrupt;
 case VARREF:
  deref_and_jump(a0,ins__pattern_9_10);
  *reasonp++ =  a0;
  goto ins__pattern_9_interrupt;
  };
  goto ins__pattern_9_interrupt;
 ins__pattern_9_ext_interrupt:
  reasonp = 0l;
 ins__pattern_9_interrupt:
  toppred = &predicate_klic__comp__insert_xins__pattern_9;
  goto interrupt_9;
 }

 ins__value__node_12_top: {
  q x0, x1, x2;
  a1 = qp->args[1];
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  a6 = qp->args[6];
  a7 = qp->args[7];
  a8 = qp->args[8];
  a9 = qp->args[9];
  a10 = qp->args[10];
  a11 = qp->args[11];
  qp = qp->next;
 ins__value__node_12_clear_reason:
  reasonp = reasons;
 ins__value__node_12_0:
 ins__value__node_12_1:
  switch (ptagof(a0)) {
 case ATOMIC:
  if (isint(a0)) goto ins__value__node_12_interrupt;
  switch (symval(a0)) {
 case symval(makesym(atom_list)):
  allocp[0] = makesym(functor_v_6);
  allocp[1] = a2;
  allocp[2] = a3;
  allocp[3] = a10;
  allocp[4] = a5;
  allocp[5] = a6;
  allocp[6] = a7;
  x0 = makefunctor(&allocp[0]);
  allocp += 7;
  unify_value(a8, x0);
  unify(a9, a4);
  x1 = makesym(atom_normal);
  unify_value(a11, x1);
  proceed();
 case symval(makesym(atom_bound)):
  allocp[0] = makesym(functor_v_6);
  allocp[1] = a2;
  allocp[2] = a3;
  allocp[3] = a4;
  allocp[4] = a5;
  allocp[5] = a10;
  allocp[6] = a7;
  x0 = makefunctor(&allocp[0]);
  allocp += 7;
  unify_value(a8, x0);
  unify(a9, a6);
  x1 = makesym(atom_normal);
  unify_value(a11, x1);
  proceed();
 case symval(makesym(atom_atomic)):
  allocp[0] = makesym(functor_v_6);
  allocp[1] = a2;
  allocp[2] = x1 = makeref(&allocp[2]);
  allocp[3] = a4;
  allocp[4] = a5;
  allocp[5] = a6;
  allocp[6] = a7;
  x0 = makefunctor(&allocp[0]);
  allocp += 7;
  unify_value(a8, x0);
  a0 = a3;
  a2 = x1;
  a3 = a9;
  a4 = a10;
  a5 = a11;
  execute(ins__atomic__node_6_0);
  goto ins__atomic__node_6_ext_interrupt;
 case symval(makesym(atom_functor)):
  allocp[0] = makesym(functor_v_6);
  allocp[1] = a2;
  allocp[2] = a3;
  allocp[3] = a4;
  allocp[4] = x1 = makeref(&allocp[4]);
  allocp[5] = a6;
  allocp[6] = a7;
  x0 = makefunctor(&allocp[0]);
  allocp += 7;
  unify_value(a8, x0);
  a0 = a5;
  a2 = x1;
  a3 = a9;
  a4 = a10;
  a5 = a11;
  execute(ins__funct__node_6_0);
  goto ins__funct__node_6_ext_interrupt;
 case symval(makesym(atom_other)):
  allocp[0] = makesym(functor_v_6);
  allocp[1] = a2;
  allocp[2] = a3;
  allocp[3] = a4;
  allocp[4] = a5;
  allocp[5] = x1 = makeref(&allocp[5]);
  allocp[6] = a7;
  x0 = makefunctor(&allocp[0]);
  allocp += 7;
  unify_value(a8, x0);
  a0 = a6;
  x2 = a2;
  a2 = a1;
  a3 = x1;
  a4 = a9;
  a5 = a10;
  a6 = a11;
  a1 = x2;
  execute(ins__eq_7_0);
  goto ins__eq_7_ext_interrupt;
  };
  goto ins__value__node_12_interrupt;
 case VARREF:
  deref_and_jump(a0,ins__value__node_12_1);
  *reasonp++ =  a0;
  goto ins__value__node_12_interrupt;
  };
  goto ins__value__node_12_interrupt;
 ins__value__node_12_ext_interrupt:
  reasonp = 0l;
 ins__value__node_12_interrupt:
  goto interrupt_12;
 }

 pattern__kind_2_top: {
  q x0;
  a1 = qp->args[1];
  qp = qp->next;
 pattern__kind_2_clear_reason:
  reasonp = reasons;
 pattern__kind_2_0:
 pattern__kind_2_1:
  switch (ptagof(a0)) {
 case ATOMIC:
  if (isint(a0)) goto pattern__kind_2_2;
  switch (symval(a0)) {
 case symval(makesym(atom_list)):
  x0 = makesym(atom_list);
  unify_value(a1, x0);
  proceed();
 case symval(makesym(atom_bound)):
  x0 = makesym(atom_bound);
  unify_value(a1, x0);
  proceed();
 case symval(makesym(atom_atom)):
  x0 = makesym(atom_atomic);
  unify_value(a1, x0);
  proceed();
 case symval(makesym(atom_int)):
  x0 = makesym(atom_atomic);
  unify_value(a1, x0);
  proceed();
 case symval(makesym(atom_atomic)):
  x0 = makesym(atom_atomic);
  unify_value(a1, x0);
  proceed();
 case symval(makesym(atom_functor)):
  x0 = makesym(atom_functor);
  unify_value(a1, x0);
  proceed();
 case symval(makesym(atom_object)):
  x0 = makesym(atom_functor);
  unify_value(a1, x0);
  proceed();
  };
  goto pattern__kind_2_2;
 case FUNCTOR:
  switch (symval(functor_of(a0))) {
 case functor_atom_1:
  x0 = makesym(atom_atomic);
  unify_value(a1, x0);
  proceed();
 case functor_int_1:
  x0 = makesym(atom_atomic);
  unify_value(a1, x0);
  proceed();
 case functor_functor_2:
  x0 = makesym(atom_functor);
  unify_value(a1, x0);
  proceed();
 case functor_object_1:
  x0 = makesym(atom_functor);
  unify_value(a1, x0);
  proceed();
  };
  goto pattern__kind_2_2;
 case VARREF:
  deref_and_jump(a0,pattern__kind_2_1);
  *reasonp++ =  a0;
  goto pattern__kind_2_2;
  };
  goto pattern__kind_2_2;
 pattern__kind_2_2:
  otherwise(pattern__kind_2_interrupt);
  x0 = makesym(atom_other);
  unify_value(a1, x0);
  proceed();
 pattern__kind_2_ext_interrupt:
  reasonp = 0l;
 pattern__kind_2_interrupt:
  toppred = &predicate_klic__comp__insert_xpattern__kind_2;
  goto interrupt_2;
 }

 ins__eq_7_top: {
  q x0, x1, x2, x3, x4, x5, x6;
  a1 = qp->args[1];
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  a6 = qp->args[6];
  qp = qp->next;
 ins__eq_7_clear_reason:
  reasonp = reasons;
 ins__eq_7_0:
 ins__eq_7_1:
  switch (ptagof(a0)) {
 case ATOMIC:
  if (a0 != makesym(atom_e)) goto ins__eq_7_6;
  allocp[0] = makesym(functor_eq_2);
  allocp[1] = a1;
  allocp[2] = a2;
  x0 = makefunctor(&allocp[0]);
  allocp[3] = makesym(functor_x_3);
  allocp[4] = x0;
  allocp[5] = a5;
  allocp[6] = makesym(atom_e);
  x1 = makefunctor(&allocp[3]);
  allocp[7] = makesym(functor_v_6);
  allocp[8] = a1;
  allocp[9] = makesym(atom_e);
  allocp[10] = makesym(atom_e);
  allocp[11] = makesym(atom_e);
  allocp[12] = x1;
  allocp[13] = makesym(atom_e);
  x2 = makefunctor(&allocp[7]);
  allocp += 14;
  unify_value(a3, x2);
  x3 = makesym(atom_e);
  unify_value(a4, x3);
  x4 = makesym(atom_normal);
  unify_value(a6, x4);
  proceed();
 case FUNCTOR:
  switch (symval(functor_of(a0))) {
 case functor_v_6:
  x0 = arg(a0, 0);
 ins__eq_7_2:
  if (isref(x0)) goto ins__eq_7_5;
 ins__eq_7_3:
  if (isref(x0)) goto ins__eq_7_4;
  if_not_equal(x0, a1, ins__eq_7_6);
  allocp[0] = makesym(functor_v_6);
  allocp[1] = x0;
  x2 = arg(a0, 1);
  allocp[2] = x2;
  x3 = arg(a0, 2);
  allocp[3] = x3;
  x4 = arg(a0, 3);
  allocp[4] = x4;
  allocp[5] = x5 = makeref(&allocp[5]);
  x6 = arg(a0, 5);
  allocp[6] = x6;
  x1 = makefunctor(&allocp[0]);
  allocp += 7;
  unify_value(a3, x1);
  a0 = arg(a0, 4);
  a1 = x0;
  a3 = x5;
  execute(ins__eq_7_clear_reason);
  goto ins__eq_7_ext_interrupt;
 ins__eq_7_4:
  deref_and_jump(x0,ins__eq_7_3);
  *reasonp++ =  x0;
  goto ins__eq_7_6;
 ins__eq_7_5:
  deref_and_jump(x0,ins__eq_7_2);
  *reasonp++ =  x0;
  goto ins__eq_7_6;
 case functor_x_3:
  allocp[0] = makesym(functor_x_3);
  x1 = arg(a0, 0);
  allocp[1] = x1;
  x2 = arg(a0, 1);
  allocp[2] = x2;
  allocp[3] = x3 = makeref(&allocp[3]);
  x0 = makefunctor(&allocp[0]);
  allocp += 4;
  unify_value(a3, x0);
  a0 = arg(a0, 2);
  a3 = x3;
  execute(ins__eq_7_0);
  goto ins__eq_7_ext_interrupt;
 case functor_l_1:
  a0 = a6;
  execute(ins__redundant_1_0);
  goto ins__redundant_1_ext_interrupt;
  };
  goto ins__eq_7_6;
 case VARREF:
  deref_and_jump(a0,ins__eq_7_1);
  *reasonp++ =  a0;
  goto ins__eq_7_6;
  };
  goto ins__eq_7_6;
 ins__eq_7_6:
  otherwise(ins__eq_7_interrupt);
 ins__eq_7_7:
  switch (ptagof(a0)) {
 case FUNCTOR:
  if (functor_of(a0) != makesym(functor_v_6)) goto ins__eq_7_interrupt;
  allocp[0] = makesym(functor_v_6);
  x1 = arg(a0, 0);
  allocp[1] = x1;
  x2 = arg(a0, 1);
  allocp[2] = x2;
  x3 = arg(a0, 2);
  allocp[3] = x3;
  x4 = arg(a0, 3);
  allocp[4] = x4;
  x5 = arg(a0, 4);
  allocp[5] = x5;
  allocp[6] = x6 = makeref(&allocp[6]);
  x0 = makefunctor(&allocp[0]);
  allocp += 7;
  unify_value(a3, x0);
  a0 = arg(a0, 5);
  a3 = x6;
  execute(ins__eq_7_0);
  goto ins__eq_7_ext_interrupt;
 case VARREF:
  deref_and_jump(a0,ins__eq_7_7);
  *reasonp++ =  a0;
  goto ins__eq_7_interrupt;
  };
  goto ins__eq_7_interrupt;
 ins__eq_7_ext_interrupt:
  reasonp = 0l;
 ins__eq_7_interrupt:
  toppred = &predicate_klic__comp__insert_xins__eq_7;
  goto interrupt_7;
 }

 ins__atomic__node_6_top: {
  q x0, x1, x2, x3, x4, x5, x6;
  a1 = qp->args[1];
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  qp = qp->next;
 ins__atomic__node_6_clear_reason:
  reasonp = reasons;
 ins__atomic__node_6_0:
 ins__atomic__node_6_1:
  switch (ptagof(a0)) {
 case ATOMIC:
  if (a0 != makesym(atom_e)) goto ins__atomic__node_6_3;
  a0 = makefunctor(funct_const_2);
  execute(ins__atomic__node_6_0);
  goto ins__atomic__node_6_ext_interrupt;
 case FUNCTOR:
  if (functor_of(a0) != makesym(functor_a_5)) goto ins__atomic__node_6_3;
 ins__atomic__node_6_2:
  switch (ptagof(a1)) {
 case ATOMIC:
  if (isint(a1)) goto ins__atomic__node_6_3;
  switch (symval(a1)) {
 case symval(makesym(atom_atomic)):
  allocp[0] = makesym(functor_a_5);
  x1 = arg(a0, 0);
  allocp[1] = x1;
  x2 = arg(a0, 1);
  allocp[2] = x2;
  x3 = arg(a0, 2);
  allocp[3] = x3;
  x4 = arg(a0, 3);
  allocp[4] = x4;
  allocp[5] = a4;
  x0 = makefunctor(&allocp[0]);
  allocp += 6;
  unify_value(a2, x0);
  x5 = arg(a0, 4);
  unify(a3, x5);
  x6 = makesym(atom_normal);
  unify_value(a5, x6);
  proceed();
 case symval(makesym(atom_atom)):
  allocp[0] = makesym(functor_a_5);
  x1 = arg(a0, 0);
  allocp[1] = x1;
  allocp[2] = a4;
  x2 = arg(a0, 2);
  allocp[3] = x2;
  x3 = arg(a0, 3);
  allocp[4] = x3;
  x4 = arg(a0, 4);
  allocp[5] = x4;
  x0 = makefunctor(&allocp[0]);
  allocp += 6;
  unify_value(a2, x0);
  x5 = arg(a0, 1);
  unify(a3, x5);
  x6 = makesym(atom_normal);
  unify_value(a5, x6);
  proceed();
 case symval(makesym(atom_int)):
  allocp[0] = makesym(functor_a_5);
  x1 = arg(a0, 0);
  allocp[1] = x1;
  x2 = arg(a0, 1);
  allocp[2] = x2;
  x3 = arg(a0, 2);
  allocp[3] = x3;
  allocp[4] = a4;
  x4 = arg(a0, 4);
  allocp[5] = x4;
  x0 = makefunctor(&allocp[0]);
  allocp += 6;
  unify_value(a2, x0);
  x5 = arg(a0, 3);
  unify(a3, x5);
  x6 = makesym(atom_normal);
  unify_value(a5, x6);
  proceed();
  };
  goto ins__atomic__node_6_3;
 case FUNCTOR:
  switch (symval(functor_of(a1))) {
 case functor_atom_1:
  allocp[0] = makesym(functor_a_5);
  allocp[1] = x1 = makeref(&allocp[1]);
  x2 = arg(a0, 1);
  allocp[2] = x2;
  x3 = arg(a0, 2);
  allocp[3] = x3;
  x4 = arg(a0, 3);
  allocp[4] = x4;
  x5 = arg(a0, 4);
  allocp[5] = x5;
  x0 = makefunctor(&allocp[0]);
  allocp += 6;
  unify_value(a2, x0);
  x6 = makesym(atom_normal);
  unify_value(a5, x6);
  a0 = arg(a0, 0);
  a1 = arg(a1, 0);
  a2 = x1;
  execute(ins__merge__case_5_0);
  goto ins__merge__case_5_ext_interrupt;
 case functor_int_1:
  allocp[0] = makesym(functor_a_5);
  x1 = arg(a0, 0);
  allocp[1] = x1;
  x2 = arg(a0, 1);
  allocp[2] = x2;
  allocp[3] = x3 = makeref(&allocp[3]);
  x4 = arg(a0, 3);
  allocp[4] = x4;
  x5 = arg(a0, 4);
  allocp[5] = x5;
  x0 = makefunctor(&allocp[0]);
  allocp += 6;
  unify_value(a2, x0);
  x6 = makesym(atom_normal);
  unify_value(a5, x6);
  a0 = arg(a0, 2);
  a1 = arg(a1, 0);
  a2 = x3;
  execute(ins__merge__case_5_0);
  goto ins__merge__case_5_ext_interrupt;
  };
  goto ins__atomic__node_6_3;
 case VARREF:
  deref_and_jump(a1,ins__atomic__node_6_2);
  *reasonp++ =  a1;
  goto ins__atomic__node_6_3;
  };
  goto ins__atomic__node_6_3;
 case VARREF:
  deref_and_jump(a0,ins__atomic__node_6_1);
  *reasonp++ =  a0;
  goto ins__atomic__node_6_3;
  };
  goto ins__atomic__node_6_3;
 ins__atomic__node_6_3:
  otherwise(ins__atomic__node_6_interrupt);
  x0 = makesym(atom_abnormal);
  unify_value(a5, x0);
  proceed();
 ins__atomic__node_6_ext_interrupt:
  reasonp = 0l;
 ins__atomic__node_6_interrupt:
  toppred = &predicate_klic__comp__insert_xins__atomic__node_6;
  goto interrupt_6;
 }

 ins__funct__node_6_top: {
  q x0, x1, x2, x3, x4, x5, x6, x7;
  a1 = qp->args[1];
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  qp = qp->next;
 ins__funct__node_6_clear_reason:
  reasonp = reasons;
 ins__funct__node_6_0:
 ins__funct__node_6_1:
  switch (ptagof(a0)) {
 case ATOMIC:
  if (a0 != makesym(atom_e)) goto ins__funct__node_6_7;
  a0 = makefunctor(funct_const_3);
  execute(ins__funct__node_6_0);
  goto ins__funct__node_6_ext_interrupt;
 case FUNCTOR:
  if (functor_of(a0) != makesym(functor_f_3)) goto ins__funct__node_6_7;
 ins__funct__node_6_2:
  switch (ptagof(a1)) {
 case ATOMIC:
  if (a1 != makesym(atom_functor)) goto ins__funct__node_6_3;
  x0 = arg(a0, 2);
  unify(a3, x0);
  allocp[0] = makesym(functor_f_3);
  x2 = arg(a0, 0);
  allocp[1] = x2;
  x3 = arg(a0, 1);
  allocp[2] = x3;
  allocp[3] = a4;
  x1 = makefunctor(&allocp[0]);
  allocp += 4;
  unify_value(a2, x1);
  x4 = makesym(atom_normal);
  unify_value(a5, x4);
  proceed();
 case FUNCTOR:
  if (functor_of(a1) != makesym(functor_functor_2)) goto ins__funct__node_6_3;
  allocp[0] = makesym(functor_f_3);
  allocp[1] = x1 = makeref(&allocp[1]);
  x2 = arg(a0, 1);
  allocp[2] = x2;
  x3 = arg(a0, 2);
  allocp[3] = x3;
  x0 = makefunctor(&allocp[0]);
  allocp += 4;
  unify_value(a2, x0);
  x4 = makesym(atom_normal);
  unify_value(a5, x4);
  allocp[0] = makesym(functor__2F_2);
  x6 = arg(a1, 0);
  allocp[1] = x6;
  x7 = arg(a1, 1);
  allocp[2] = x7;
  x5 = makefunctor(&allocp[0]);
  a0 = arg(a0, 0);
  a1 = x5;
  a2 = x1;
  allocp += 3;
  execute(ins__merge__case_5_0);
  goto ins__merge__case_5_ext_interrupt;
 case VARREF:
  deref_and_jump(a1,ins__funct__node_6_2);
  *reasonp++ =  a1;
  goto ins__funct__node_6_3;
  };
  goto ins__funct__node_6_3;
 ins__funct__node_6_3:
  x0 = arg(a0, 1);
 ins__funct__node_6_4:
  switch (ptagof(x0)) {
 case ATOMIC:
  if (x0 != makesym(atom_e)) goto ins__funct__node_6_7;
 ins__funct__node_6_5:
  switch (ptagof(a1)) {
 case ATOMIC:
  if (a1 != makesym(atom_object)) goto ins__funct__node_6_7;
  x1 = makesym(atom_e);
  unify_value(a3, x1);
  allocp[0] = makesym(functor_o_2);
  allocp[1] = NILATOM;
  allocp[2] = a4;
  x2 = makefunctor(&allocp[0]);
  allocp[3] = makesym(functor_f_3);
  x4 = arg(a0, 0);
  allocp[4] = x4;
  allocp[5] = x2;
  x5 = arg(a0, 2);
  allocp[6] = x5;
  x3 = makefunctor(&allocp[3]);
  allocp += 7;
  unify_value(a2, x3);
  x6 = makesym(atom_normal);
  unify_value(a5, x6);
  proceed();
 case FUNCTOR:
  if (functor_of(a1) != makesym(functor_object_1)) goto ins__funct__node_6_7;
  allocp[0] = makesym(functor_o_2);
  allocp[1] = x2 = makeref(&allocp[1]);
  allocp[2] = makesym(atom_e);
  x1 = makefunctor(&allocp[0]);
  allocp[3] = makesym(functor_f_3);
  x4 = arg(a0, 0);
  allocp[4] = x4;
  allocp[5] = x1;
  x5 = arg(a0, 2);
  allocp[6] = x5;
  x3 = makefunctor(&allocp[3]);
  allocp += 7;
  unify_value(a2, x3);
  x6 = makesym(atom_normal);
  unify_value(a5, x6);
  a0 = NILATOM;
  a1 = arg(a1, 0);
  a2 = x2;
  execute(ins__merge__case_5_clear_reason);
  goto ins__merge__case_5_ext_interrupt;
 case VARREF:
  deref_and_jump(a1,ins__funct__node_6_5);
  *reasonp++ =  a1;
  goto ins__funct__node_6_7;
  };
  goto ins__funct__node_6_7;
 case FUNCTOR:
  if (functor_of(x0) != makesym(functor_o_2)) goto ins__funct__node_6_7;
 ins__funct__node_6_6:
  switch (ptagof(a1)) {
 case ATOMIC:
  if (a1 != makesym(atom_object)) goto ins__funct__node_6_7;
  x1 = arg(x0, 1);
  unify(a3, x1);
  allocp[0] = makesym(functor_o_2);
  x3 = arg(x0, 0);
  allocp[1] = x3;
  allocp[2] = a4;
  x2 = makefunctor(&allocp[0]);
  allocp[3] = makesym(functor_f_3);
  x5 = arg(a0, 0);
  allocp[4] = x5;
  allocp[5] = x2;
  x6 = arg(a0, 2);
  allocp[6] = x6;
  x4 = makefunctor(&allocp[3]);
  allocp += 7;
  unify_value(a2, x4);
  x7 = makesym(atom_normal);
  unify_value(a5, x7);
  proceed();
 case FUNCTOR:
  if (functor_of(a1) != makesym(functor_object_1)) goto ins__funct__node_6_7;
  allocp[0] = makesym(functor_o_2);
  allocp[1] = x2 = makeref(&allocp[1]);
  x3 = arg(x0, 1);
  allocp[2] = x3;
  x1 = makefunctor(&allocp[0]);
  allocp[3] = makesym(functor_f_3);
  x5 = arg(a0, 0);
  allocp[4] = x5;
  allocp[5] = x1;
  x6 = arg(a0, 2);
  allocp[6] = x6;
  x4 = makefunctor(&allocp[3]);
  allocp += 7;
  unify_value(a2, x4);
  x7 = makesym(atom_normal);
  unify_value(a5, x7);
  a0 = arg(x0, 0);
  a1 = arg(a1, 0);
  a2 = x2;
  execute(ins__merge__case_5_clear_reason);
  goto ins__merge__case_5_ext_interrupt;
 case VARREF:
  deref_and_jump(a1,ins__funct__node_6_6);
  *reasonp++ =  a1;
  goto ins__funct__node_6_7;
  };
  goto ins__funct__node_6_7;
 case VARREF:
  deref_and_jump(x0,ins__funct__node_6_4);
  *reasonp++ =  x0;
  goto ins__funct__node_6_7;
  };
  goto ins__funct__node_6_7;
 case VARREF:
  deref_and_jump(a0,ins__funct__node_6_1);
  *reasonp++ =  a0;
  goto ins__funct__node_6_7;
  };
  goto ins__funct__node_6_7;
 ins__funct__node_6_7:
  otherwise(ins__funct__node_6_interrupt);
  x0 = makesym(atom_abnormal);
  unify_value(a5, x0);
  proceed();
 ins__funct__node_6_ext_interrupt:
  reasonp = 0l;
 ins__funct__node_6_interrupt:
  toppred = &predicate_klic__comp__insert_xins__funct__node_6;
  goto interrupt_6;
 }

 ins__merge__case_5_top: {
  q x0, x1, x2, x3, x4, x5;
  a1 = qp->args[1];
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  qp = qp->next;
 ins__merge__case_5_clear_reason:
  reasonp = reasons;
 ins__merge__case_5_0:
 ins__merge__case_5_1:
  switch (ptagof(a0)) {
 case CONS:
  x0 = car_of(a0);
 ins__merge__case_5_2:
  switch (ptagof(x0)) {
 case FUNCTOR:
  if (functor_of(x0) != makesym(functor__2D_2)) goto ins__merge__case_5_7;
  x1 = arg(x0, 0);
 ins__merge__case_5_3:
  if (isref(x1)) goto ins__merge__case_5_6;
 ins__merge__case_5_4:
  if (isref(x1)) goto ins__merge__case_5_5;
  if_not_equal(x1, a1, ins__merge__case_5_7);
  allocp[0] = makesym(functor__2D_2);
  allocp[1] = x1;
  allocp[2] = a4;
  x2 = makefunctor(&allocp[0]);
  x4 = cdr_of(a0);
  allocp[3] = x4;
  allocp[4] = x2;
  x3 = makecons(&allocp[3]);
  allocp += 5;
  unify_value(a2, x3);
  x5 = arg(x0, 1);
  unify(a3, x5);
  proceed();
 ins__merge__case_5_5:
  deref_and_jump(x1,ins__merge__case_5_4);
  *reasonp++ =  x1;
  goto ins__merge__case_5_7;
 ins__merge__case_5_6:
  deref_and_jump(x1,ins__merge__case_5_3);
  *reasonp++ =  x1;
  goto ins__merge__case_5_7;
 case VARREF:
  deref_and_jump(x0,ins__merge__case_5_2);
  *reasonp++ =  x0;
  goto ins__merge__case_5_7;
  };
  goto ins__merge__case_5_7;
 case ATOMIC:
  if (a0 != NILATOM) goto ins__merge__case_5_7;
  allocp[0] = makesym(functor__2D_2);
  allocp[1] = a1;
  allocp[2] = a4;
  x0 = makefunctor(&allocp[0]);
  allocp[3] = NILATOM;
  allocp[4] = x0;
  x1 = makecons(&allocp[3]);
  allocp += 5;
  unify_value(a2, x1);
  x2 = makesym(atom_e);
  unify_value(a3, x2);
  proceed();
 case VARREF:
  deref_and_jump(a0,ins__merge__case_5_1);
  *reasonp++ =  a0;
  goto ins__merge__case_5_7;
  };
  goto ins__merge__case_5_7;
 ins__merge__case_5_7:
  otherwise(ins__merge__case_5_interrupt);
 ins__merge__case_5_8:
  switch (ptagof(a0)) {
 case CONS:
  x0 = car_of(a0);
 ins__merge__case_5_9:
  switch (ptagof(x0)) {
 case FUNCTOR:
  if (functor_of(x0) != makesym(functor__2D_2)) goto ins__merge__case_5_interrupt;
  allocp[0] = makesym(functor__2D_2);
  x2 = arg(x0, 0);
  allocp[1] = x2;
  x3 = arg(x0, 1);
  allocp[2] = x3;
  x1 = makefunctor(&allocp[0]);
  allocp[3] = x5 = makeref(&allocp[3]);
  allocp[4] = x1;
  x4 = makecons(&allocp[3]);
  allocp += 5;
  unify_value(a2, x4);
  a0 = cdr_of(a0);
  a2 = x5;
  execute(ins__merge__case_5_0);
  goto ins__merge__case_5_ext_interrupt;
 case VARREF:
  deref_and_jump(x0,ins__merge__case_5_9);
  *reasonp++ =  x0;
  goto ins__merge__case_5_interrupt;
  };
  goto ins__merge__case_5_interrupt;
 case VARREF:
  deref_and_jump(a0,ins__merge__case_5_8);
  *reasonp++ =  a0;
  goto ins__merge__case_5_interrupt;
  };
  goto ins__merge__case_5_interrupt;
 ins__merge__case_5_ext_interrupt:
  reasonp = 0l;
 ins__merge__case_5_interrupt:
  toppred = &predicate_klic__comp__insert_xins__merge__case_5;
  goto interrupt_5;
 }

 ins__gb_6_top: {
  q x0, x1, x2, x3, x4, x5, x6;
  a1 = qp->args[1];
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  qp = qp->next;
 ins__gb_6_clear_reason:
  reasonp = reasons;
 ins__gb_6_0:
 ins__gb_6_1:
  switch (ptagof(a0)) {
 case ATOMIC:
  if (a0 != makesym(atom_e)) goto ins__gb_6_6;
  allocp[0] = makesym(functor_x_3);
  allocp[1] = a1;
  allocp[2] = a4;
  allocp[3] = makesym(atom_e);
  x0 = makefunctor(&allocp[0]);
  allocp += 4;
  unify_value(a2, x0);
  x1 = makesym(atom_e);
  unify_value(a3, x1);
  x2 = makesym(atom_normal);
  unify_value(a5, x2);
  proceed();
 case FUNCTOR:
  switch (symval(functor_of(a0))) {
 case functor_v_6:
  allocp[0] = makesym(functor_v_6);
  x1 = arg(a0, 0);
  allocp[1] = x1;
  x2 = arg(a0, 1);
  allocp[2] = x2;
  x3 = arg(a0, 2);
  allocp[3] = x3;
  x4 = arg(a0, 3);
  allocp[4] = x4;
  x5 = arg(a0, 4);
  allocp[5] = x5;
  allocp[6] = x6 = makeref(&allocp[6]);
  x0 = makefunctor(&allocp[0]);
  allocp += 7;
  unify_value(a2, x0);
  a0 = arg(a0, 5);
  a2 = x6;
  execute(ins__gb_6_0);
  goto ins__gb_6_ext_interrupt;
 case functor_x_3:
  x0 = arg(a0, 0);
 ins__gb_6_2:
  if (isref(x0)) goto ins__gb_6_5;
 ins__gb_6_3:
  if (isref(x0)) goto ins__gb_6_4;
  if_not_equal(x0, a1, ins__gb_6_6);
  x1 = arg(a0, 1);
  unify(a3, x1);
  allocp[0] = makesym(functor_x_3);
  allocp[1] = x0;
  allocp[2] = a4;
  x3 = arg(a0, 2);
  allocp[3] = x3;
  x2 = makefunctor(&allocp[0]);
  allocp += 4;
  unify_value(a2, x2);
  x4 = makesym(atom_normal);
  unify_value(a5, x4);
  proceed();
 ins__gb_6_4:
  deref_and_jump(x0,ins__gb_6_3);
  *reasonp++ =  x0;
  goto ins__gb_6_6;
 ins__gb_6_5:
  deref_and_jump(x0,ins__gb_6_2);
  *reasonp++ =  x0;
  goto ins__gb_6_6;
 case functor_l_1:
  a0 = a5;
  execute(ins__redundant_1_0);
  goto ins__redundant_1_ext_interrupt;
  };
  goto ins__gb_6_6;
 case VARREF:
  deref_and_jump(a0,ins__gb_6_1);
  *reasonp++ =  a0;
  goto ins__gb_6_6;
  };
  goto ins__gb_6_6;
 ins__gb_6_6:
  otherwise(ins__gb_6_interrupt);
 ins__gb_6_7:
  switch (ptagof(a0)) {
 case FUNCTOR:
  if (functor_of(a0) != makesym(functor_x_3)) goto ins__gb_6_interrupt;
  allocp[0] = makesym(functor_x_3);
  x1 = arg(a0, 0);
  allocp[1] = x1;
  x2 = arg(a0, 1);
  allocp[2] = x2;
  allocp[3] = x3 = makeref(&allocp[3]);
  x0 = makefunctor(&allocp[0]);
  allocp += 4;
  unify_value(a2, x0);
  a0 = arg(a0, 2);
  a2 = x3;
  execute(ins__gb_6_0);
  goto ins__gb_6_ext_interrupt;
 case VARREF:
  deref_and_jump(a0,ins__gb_6_7);
  *reasonp++ =  a0;
  goto ins__gb_6_interrupt;
  };
  goto ins__gb_6_interrupt;
 ins__gb_6_ext_interrupt:
  reasonp = 0l;
 ins__gb_6_interrupt:
  toppred = &predicate_klic__comp__insert_xins__gb_6;
  goto interrupt_6;
 }

 subsumed_4_top: {
  q x0, x1;
  a1 = qp->args[1];
  a2 = qp->args[2];
  a3 = qp->args[3];
  qp = qp->next;
 subsumed_4_clear_reason:
  reasonp = reasons;
 subsumed_4_0:
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__insert_xsubsumed__1_5);
  allocp[2] = a0;
  allocp[3] = a1;
  allocp[4] = a2;
  allocp[5] = x0 = makeref(&allocp[5]);
  allocp[6] = a3;
  x1 = a1;
  a1 = a0;
  a2 = x0;
  a0 = x1;
  qp = (struct goalrec*)&allocp[0];
  allocp += 7;
  execute(subsumed__type_3_0);
  goto subsumed__type_3_ext_interrupt;
 subsumed_4_ext_interrupt:
  reasonp = 0l;
 subsumed_4_interrupt:
  toppred = &predicate_klic__comp__insert_xsubsumed_4;
  goto interrupt_4;
 }

 subsumed__1_5_top: {
  q x0;
  a1 = qp->args[1];
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  qp = qp->next;
 subsumed__1_5_clear_reason:
  reasonp = reasons;
 subsumed__1_5_0:
 subsumed__1_5_1:
  switch (ptagof(a3)) {
 case ATOMIC:
  if (isint(a3)) goto subsumed__1_5_interrupt;
  switch (symval(a3)) {
 case symval(makesym(atom_normal)):
  x0 = makesym(atom_normal);
  unify_value(a4, x0);
  proceed();
 case symval(makesym(atom_abnormal)):
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__insert_xsubsumed__2_3);
  allocp[2] = x0 = makeref(&allocp[2]);
  allocp[3] = a0;
  allocp[4] = a4;
  allocp[5] = (q)(struct goalrec*)&allocp[0];
  allocp[6] = (q)(&predicate_klic__comp__util_xassoc_3);
  allocp[7] = a2;
  allocp[8] = a1;
  allocp[9] = x0;
  qp = (struct goalrec*)&allocp[5];
  allocp += 10;
  proceed();
  };
  goto subsumed__1_5_interrupt;
 case VARREF:
  deref_and_jump(a3,subsumed__1_5_1);
  *reasonp++ =  a3;
  goto subsumed__1_5_interrupt;
  };
  goto subsumed__1_5_interrupt;
 subsumed__1_5_ext_interrupt:
  reasonp = 0l;
 subsumed__1_5_interrupt:
  goto interrupt_5;
 }

 subsumed__2_3_top: {
  q x0;
  a1 = qp->args[1];
  a2 = qp->args[2];
  qp = qp->next;
 subsumed__2_3_clear_reason:
  reasonp = reasons;
 subsumed__2_3_0:
 subsumed__2_3_1:
  switch (ptagof(a0)) {
 case ATOMIC:
  if (a0 != makesym(atom_no)) goto subsumed__2_3_interrupt;
  x0 = makesym(atom_abnormal);
  unify_value(a2, x0);
  proceed();
 case FUNCTOR:
  if (functor_of(a0) != makesym(functor_yes_1)) goto subsumed__2_3_interrupt;
  a0 = arg(a0, 0);
  execute(subsumed__type_3_0);
  goto subsumed__type_3_ext_interrupt;
 case VARREF:
  deref_and_jump(a0,subsumed__2_3_1);
  *reasonp++ =  a0;
  goto subsumed__2_3_interrupt;
  };
  goto subsumed__2_3_interrupt;
 subsumed__2_3_ext_interrupt:
  reasonp = 0l;
 subsumed__2_3_interrupt:
  goto interrupt_3;
 }

 subsumed__type_3_top: {
  q x0;
  a1 = qp->args[1];
  a2 = qp->args[2];
  qp = qp->next;
 subsumed__type_3_clear_reason:
  reasonp = reasons;
 subsumed__type_3_0:
 subsumed__type_3_1:
  switch (ptagof(a0)) {
 case ATOMIC:
  if (isint(a0)) goto subsumed__type_3_9;
  switch (symval(a0)) {
 case symval(makesym(atom_atom)):
 subsumed__type_3_2:
  switch (ptagof(a1)) {
 case ATOMIC:
  if (isint(a1)) goto subsumed__type_3_9;
  switch (symval(a1)) {
 case symval(makesym(atom_atomic)):
  x0 = makesym(atom_normal);
  unify_value(a2, x0);
  proceed();
 case symval(makesym(atom_bound)):
  x0 = makesym(atom_normal);
  unify_value(a2, x0);
  proceed();
  };
  goto subsumed__type_3_9;
 case VARREF:
  deref_and_jump(a1,subsumed__type_3_2);
  *reasonp++ =  a1;
  goto subsumed__type_3_9;
  };
  goto subsumed__type_3_9;
 case symval(makesym(atom_int)):
 subsumed__type_3_3:
  switch (ptagof(a1)) {
 case ATOMIC:
  if (isint(a1)) goto subsumed__type_3_9;
  switch (symval(a1)) {
 case symval(makesym(atom_atomic)):
  x0 = makesym(atom_normal);
  unify_value(a2, x0);
  proceed();
 case symval(makesym(atom_bound)):
  x0 = makesym(atom_normal);
  unify_value(a2, x0);
  proceed();
  };
  goto subsumed__type_3_9;
 case VARREF:
  deref_and_jump(a1,subsumed__type_3_3);
  *reasonp++ =  a1;
  goto subsumed__type_3_9;
  };
  goto subsumed__type_3_9;
 case symval(makesym(atom_float)):
 subsumed__type_3_4:
  switch (ptagof(a1)) {
 case ATOMIC:
  if (isint(a1)) goto subsumed__type_3_9;
  switch (symval(a1)) {
 case symval(makesym(atom_bound)):
  x0 = makesym(atom_normal);
  unify_value(a2, x0);
  proceed();
 case symval(makesym(atom_object)):
  x0 = makesym(atom_normal);
  unify_value(a2, x0);
  proceed();
  };
  goto subsumed__type_3_9;
 case VARREF:
  deref_and_jump(a1,subsumed__type_3_4);
  *reasonp++ =  a1;
  goto subsumed__type_3_9;
  };
  goto subsumed__type_3_9;
 case symval(makesym(atom_atomic)):
 subsumed__type_3_5:
  switch (ptagof(a1)) {
 case ATOMIC:
  if (a1 != makesym(atom_bound)) goto subsumed__type_3_9;
  x0 = makesym(atom_normal);
  unify_value(a2, x0);
  proceed();
 case VARREF:
  deref_and_jump(a1,subsumed__type_3_5);
  *reasonp++ =  a1;
  goto subsumed__type_3_9;
  };
  goto subsumed__type_3_9;
 case symval(makesym(atom_functor)):
 subsumed__type_3_6:
  switch (ptagof(a1)) {
 case ATOMIC:
  if (a1 != makesym(atom_bound)) goto subsumed__type_3_9;
  x0 = makesym(atom_normal);
  unify_value(a2, x0);
  proceed();
 case VARREF:
  deref_and_jump(a1,subsumed__type_3_6);
  *reasonp++ =  a1;
  goto subsumed__type_3_9;
  };
  goto subsumed__type_3_9;
 case symval(makesym(atom_string)):
 subsumed__type_3_7:
  switch (ptagof(a1)) {
 case ATOMIC:
  if (isint(a1)) goto subsumed__type_3_9;
  switch (symval(a1)) {
 case symval(makesym(atom_bound)):
  x0 = makesym(atom_normal);
  unify_value(a2, x0);
  proceed();
 case symval(makesym(atom_object)):
  x0 = makesym(atom_normal);
  unify_value(a2, x0);
  proceed();
  };
  goto subsumed__type_3_9;
 case VARREF:
  deref_and_jump(a1,subsumed__type_3_7);
  *reasonp++ =  a1;
  goto subsumed__type_3_9;
  };
  goto subsumed__type_3_9;
 case symval(makesym(atom_vector)):
 subsumed__type_3_8:
  switch (ptagof(a1)) {
 case ATOMIC:
  if (isint(a1)) goto subsumed__type_3_9;
  switch (symval(a1)) {
 case symval(makesym(atom_bound)):
  x0 = makesym(atom_normal);
  unify_value(a2, x0);
  proceed();
 case symval(makesym(atom_object)):
  x0 = makesym(atom_normal);
  unify_value(a2, x0);
  proceed();
  };
  goto subsumed__type_3_9;
 case VARREF:
  deref_and_jump(a1,subsumed__type_3_8);
  *reasonp++ =  a1;
  goto subsumed__type_3_9;
  };
  goto subsumed__type_3_9;
  };
  goto subsumed__type_3_9;
 case FUNCTOR:
  switch (symval(functor_of(a0))) {
 case functor_atom_1:
  a0 = makesym(atom_atom);
  execute(subsumed__type_3_0);
  goto subsumed__type_3_ext_interrupt;
 case functor_int_1:
  a0 = makesym(atom_int);
  execute(subsumed__type_3_0);
  goto subsumed__type_3_ext_interrupt;
 case functor_float_1:
  a0 = makesym(atom_float);
  execute(subsumed__type_3_0);
  goto subsumed__type_3_ext_interrupt;
 case functor_list_1:
  a0 = makesym(atom_list);
  execute(subsumed__type_3_0);
  goto subsumed__type_3_ext_interrupt;
 case functor_functor_1:
  a0 = makesym(atom_functor);
  execute(subsumed__type_3_0);
  goto subsumed__type_3_ext_interrupt;
 case functor_functor_2:
  a0 = makesym(atom_functor);
  execute(subsumed__type_3_0);
  goto subsumed__type_3_ext_interrupt;
 case functor_string_1:
  a0 = makesym(atom_string);
  execute(subsumed__type_3_0);
  goto subsumed__type_3_ext_interrupt;
 case functor_vector_1:
  a0 = makesym(atom_vector);
  execute(subsumed__type_3_0);
  goto subsumed__type_3_ext_interrupt;
  };
  goto subsumed__type_3_9;
 case VARREF:
  deref_and_jump(a0,subsumed__type_3_1);
  *reasonp++ =  a0;
  goto subsumed__type_3_12;
  };
  goto subsumed__type_3_9;
 subsumed__type_3_9:
 subsumed__type_3_10:
  if (isref(a0)) goto subsumed__type_3_11;
  if_not_equal(a0, a1, subsumed__type_3_12);
  x0 = makesym(atom_normal);
  unify_value(a2, x0);
  proceed();
 subsumed__type_3_11:
  deref_and_jump(a0,subsumed__type_3_10);
  *reasonp++ =  a0;
  goto subsumed__type_3_12;
 subsumed__type_3_12:
 subsumed__type_3_13:
  switch (ptagof(a1)) {
 case ATOMIC:
  if (a1 != makesym(atom_any)) goto subsumed__type_3_14;
  x0 = makesym(atom_normal);
  unify_value(a2, x0);
  proceed();
 case VARREF:
  deref_and_jump(a1,subsumed__type_3_13);
  *reasonp++ =  a1;
  goto subsumed__type_3_14;
  };
  goto subsumed__type_3_14;
 subsumed__type_3_14:
  otherwise(subsumed__type_3_interrupt);
  x0 = makesym(atom_abnormal);
  unify_value(a2, x0);
  proceed();
 subsumed__type_3_ext_interrupt:
  reasonp = 0l;
 subsumed__type_3_interrupt:
  toppred = &predicate_klic__comp__insert_xsubsumed__type_3;
  goto interrupt_3;
 }

 ins__redundant_1_top: {
  q x0;
  qp = qp->next;
 ins__redundant_1_clear_reason:
  reasonp = reasons;
 ins__redundant_1_0:
  x0 = makesym(atom_abnormal);
  unify_value(a0, x0);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__message_xwarning_2);
  allocp[2] = makefunctor(&string_const_1);
  allocp[3] = NILATOM;
  qp = (struct goalrec*)&allocp[0];
  allocp += 4;
  proceed();
 ins__redundant_1_ext_interrupt:
  reasonp = 0l;
 ins__redundant_1_interrupt:
  toppred = &predicate_klic__comp__insert_xins__redundant_1;
  goto interrupt_1;
 }
 interrupt_12:
  allocp[13] = a11;
 interrupt_11:
  allocp[12] = a10;
 interrupt_10:
  allocp[11] = a9;
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
  loop_within_module(module_klic__comp__insert);
}
