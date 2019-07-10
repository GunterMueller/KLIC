/* Compiled by KLIC compiler version 3.003 (Mon Mar 29 19:42:59 JST 1999) */
#include <klic/klichdr.h>
#include "atom.h"
#include "funct.h"

module module_splay__tree();
Const struct predicate predicate_splay__tree_xcreate_1 =
   { module_splay__tree, 0, 1 };
Const struct predicate predicate_splay__tree_xcreate_2 =
   { module_splay__tree, 1, 2 };
Const struct predicate predicate_splay__tree_xdefault__comparator_1 =
   { module_splay__tree, 2, 1 };
Const struct predicate predicate_splay__tree_xrefer_5 =
   { module_splay__tree, 3, 5 };
Const struct predicate predicate_splay__tree_xempty_3 =
   { module_splay__tree, 4, 3 };
Const struct predicate predicate_splay__tree_xempty_2F3_240_3 =
   { module_splay__tree, 5, 3 };
Const struct predicate predicate_splay__tree_xempty_4 =
   { module_splay__tree, 6, 4 };
Const struct predicate predicate_splay__tree_xempty__sub_4 =
   { module_splay__tree, 7, 4 };
Const struct predicate predicate_splay__tree_xempty__sub_2F4_240_6 =
   { module_splay__tree, 8, 6 };
Const struct predicate predicate_splay__tree_xupdate_5 =
   { module_splay__tree, 9, 5 };
Const struct predicate predicate_splay__tree_xput_4 =
   { module_splay__tree, 10, 4 };
Const struct predicate predicate_splay__tree_xupdate_6 =
   { module_splay__tree, 11, 6 };
Const struct predicate predicate_splay__tree_xupdate_8 =
   { module_splay__tree, 12, 8 };
Const struct predicate predicate_splay__tree_xupdate_2F8_240_12 =
   { module_splay__tree, 13, 12 };
Const struct predicate predicate_splay__tree_xupdate__left_11 =
   { module_splay__tree, 14, 11 };
Const struct predicate predicate_splay__tree_xupdate__left_2F11_240_15 =
   { module_splay__tree, 15, 15 };
Const struct predicate predicate_splay__tree_xupdate__right_11 =
   { module_splay__tree, 16, 11 };
Const struct predicate predicate_splay__tree_xupdate__right_2F11_240_15 =
   { module_splay__tree, 17, 15 };
Const struct predicate predicate_splay__tree_xget_4 =
   { module_splay__tree, 18, 4 };
Const struct predicate predicate_splay__tree_xget_5 =
   { module_splay__tree, 19, 5 };
Const struct predicate predicate_splay__tree_xget_2F5_241_3 =
   { module_splay__tree, 20, 3 };
Const struct predicate predicate_splay__tree_xget_2F5_240_9 =
   { module_splay__tree, 21, 9 };
Const struct predicate predicate_splay__tree_xget__all_3 =
   { module_splay__tree, 22, 3 };
Const struct predicate predicate_splay__tree_xget__all__sub_3 =
   { module_splay__tree, 23, 3 };
Const struct predicate predicate_splay__tree_xget__max_4 =
   { module_splay__tree, 24, 4 };
Const struct predicate predicate_splay__tree_xget__max__sub_4 =
   { module_splay__tree, 25, 4 };
Const struct predicate predicate_splay__tree_xget__min_4 =
   { module_splay__tree, 26, 4 };
Const struct predicate predicate_splay__tree_xget__min__sub_4 =
   { module_splay__tree, 27, 4 };
Const struct predicate predicate_splay__tree_xreform__tree_3 =
   { module_splay__tree, 28, 3 };
extern Const struct predicate predicate_comparator_xsort_5;

module module_splay__tree(glbl, qp, allocp, toppred)
  struct global_variables *glbl;
  struct goalrec *qp;
  register q *allocp;
  Const struct predicate *toppred;
{
  declare_method_table_of(module);
  extern module module_comparator();
  static Const module_structure_type mod_const_0 =
    declare_module_constant(module_comparator, makesym(atom_comparator));
  declare_method_table_of(predicate);
  static Const predicate_structure_type pred_const_1 =
    declare_pred_constant(predicate_comparator_xsort_5, mod_const_0, makesym(atom_sort));
  q a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14;

  q *reasonp;
 module_top:
  a0 = qp->args[0];
  switch_on_pred() {
    case_pred(0, create_1_top);
    case_pred(1, create_2_top);
    case_pred(2, default__comparator_1_top);
    case_pred(3, refer_5_top);
    case_pred(4, empty_3_top);
    case_pred(5, empty_2F3_240_3_top);
    case_pred(6, empty_4_top);
    case_pred(7, empty__sub_4_top);
    case_pred(8, empty__sub_2F4_240_6_top);
    case_pred(9, update_5_top);
    case_pred(10, put_4_top);
    case_pred(11, update_6_top);
    case_pred(12, update_8_top);
    case_pred(13, update_2F8_240_12_top);
    case_pred(14, update__left_11_top);
    case_pred(15, update__left_2F11_240_15_top);
    case_pred(16, update__right_11_top);
    case_pred(17, update__right_2F11_240_15_top);
    case_pred(18, get_4_top);
    case_pred(19, get_5_top);
    case_pred(20, get_2F5_241_3_top);
    case_pred(21, get_2F5_240_9_top);
    case_pred(22, get__all_3_top);
    case_pred(23, get__all__sub_3_top);
    case_pred(24, get__max_4_top);
    case_pred(25, get__max__sub_4_top);
    case_pred(26, get__min_4_top);
    case_pred(27, get__min__sub_4_top);
    last_case_pred(28, reform__tree_3_top);
  }

 create_1_top: {
  q x0, x1;
  qp = qp->next;
 create_1_clear_reason:
  reasonp = reasons;
 create_1_0:
  allocp[0] = makesym(functor_pool_2);
  allocp[1] = x1 = makeref(&allocp[1]);
  allocp[2] = NILATOM;
  x0 = makefunctor(&allocp[0]);
  allocp += 3;
  unify_value(a0, x0);
  a0 = x1;
  execute(default__comparator_1_0);
  goto default__comparator_1_ext_interrupt;
 create_1_ext_interrupt:
  reasonp = 0l;
 create_1_interrupt:
  goto interrupt_1;
 }

 create_2_top: {
  q x0;
  a1 = qp->args[1];
  qp = qp->next;
 create_2_clear_reason:
  reasonp = reasons;
 create_2_0:
  allocp[0] = makesym(functor_pool_2);
  allocp[1] = a0;
  allocp[2] = NILATOM;
  x0 = makefunctor(&allocp[0]);
  allocp += 3;
  unify_value(a1, x0);
  proceed();
 create_2_ext_interrupt:
  reasonp = 0l;
 create_2_interrupt:
  goto interrupt_2;
 }

 default__comparator_1_top: {
  q x0;
  qp = qp->next;
 default__comparator_1_clear_reason:
  reasonp = reasons;
 default__comparator_1_0:
  x0 = makepred(pred_const_1);
  unify_value(a0, x0);
  proceed();
 default__comparator_1_ext_interrupt:
  reasonp = 0l;
 default__comparator_1_interrupt:
  toppred = &predicate_splay__tree_xdefault__comparator_1;
  goto interrupt_1;
 }

 refer_5_top: {
  a1 = qp->args[1];
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  qp = qp->next;
 refer_5_clear_reason:
  reasonp = reasons;
 refer_5_0:
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_comparator_xsort_5);
  allocp[2] = a0;
  allocp[3] = a1;
  allocp[4] = a2;
  allocp[5] = a3;
  allocp[6] = a4;
  qp = (struct goalrec*)&allocp[0];
  allocp += 7;
  proceed();
 refer_5_ext_interrupt:
  reasonp = 0l;
 refer_5_interrupt:
  goto interrupt_5;
 }

 empty_3_top: {
  q x0;
  a1 = qp->args[1];
  a2 = qp->args[2];
  qp = qp->next;
 empty_3_clear_reason:
  reasonp = reasons;
 empty_3_0:
  x0 = a1;
  a1 = a0;
  a0 = x0;
  execute(empty_2F3_240_3_0);
  goto empty_2F3_240_3_ext_interrupt;
 empty_3_ext_interrupt:
  reasonp = 0l;
 empty_3_interrupt:
  goto interrupt_3;
 }

 empty_2F3_240_3_top: {
  q x0, x1;
  a1 = qp->args[1];
  a2 = qp->args[2];
  qp = qp->next;
 empty_2F3_240_3_clear_reason:
  reasonp = reasons;
 empty_2F3_240_3_0:
 empty_2F3_240_3_1:
  switch (ptagof(a0)) {
 case FUNCTOR:
  if (functor_of(a0) != makesym(functor_pool_2)) goto empty_2F3_240_3_3;
  x0 = arg(a0, 1);
 empty_2F3_240_3_2:
  switch (ptagof(x0)) {
 case ATOMIC:
  if (x0 != NILATOM) goto empty_2F3_240_3_3;
  x1 = makesym(atom_yes);
  unify_value(a1, x1);
  unify_value(a2, a0);
  proceed();
 case VARREF:
  deref_and_jump(x0,empty_2F3_240_3_2);
  *reasonp++ =  x0;
  goto empty_2F3_240_3_3;
  };
  goto empty_2F3_240_3_3;
 case VARREF:
  deref_and_jump(a0,empty_2F3_240_3_1);
  *reasonp++ =  a0;
  goto empty_2F3_240_3_3;
  };
  goto empty_2F3_240_3_3;
 empty_2F3_240_3_3:
  otherwise(empty_2F3_240_3_interrupt);
  x0 = makesym(atom_no);
  unify_value(a1, x0);
  unify(a2, a0);
  proceed();
 empty_2F3_240_3_ext_interrupt:
  reasonp = 0l;
 empty_2F3_240_3_interrupt:
  toppred = &predicate_splay__tree_xempty_2F3_240_3;
  goto interrupt_3;
 }

 empty_4_top: {
  q x0, x1, x2, x3, x4, x5;
  a1 = qp->args[1];
  a2 = qp->args[2];
  a3 = qp->args[3];
  qp = qp->next;
 empty_4_clear_reason:
  reasonp = reasons;
 empty_4_0:
 empty_4_1:
  switch (ptagof(a2)) {
 case FUNCTOR:
  if (functor_of(a2) != makesym(functor_pool_2)) goto empty_4_interrupt;
  allocp[0] = makesym(functor_pool_2);
  x1 = arg(a2, 0);
  allocp[1] = x1;
  x2 = arg(a2, 1);
  allocp[2] = x2;
  x0 = makefunctor(&allocp[0]);
  allocp += 3;
  unify_value(a3, x0);
  x3 = arg(a2, 1);
  x4 = a0;
  x5 = a1;
  a3 = arg(a2, 0);
  a2 = x5;
  a1 = x4;
  a0 = x3;
  execute(empty__sub_4_0);
  goto empty__sub_4_ext_interrupt;
 case VARREF:
  deref_and_jump(a2,empty_4_1);
  *reasonp++ =  a2;
  goto empty_4_interrupt;
  };
  goto empty_4_interrupt;
 empty_4_ext_interrupt:
  reasonp = 0l;
 empty_4_interrupt:
  goto interrupt_4;
 }

 empty__sub_4_top: {
  q x0, x1, x2, x3, x4, x5, x6;
  a1 = qp->args[1];
  a2 = qp->args[2];
  a3 = qp->args[3];
  qp = qp->next;
 empty__sub_4_clear_reason:
  reasonp = reasons;
 empty__sub_4_0:
 empty__sub_4_1:
  switch (ptagof(a0)) {
 case ATOMIC:
  if (a0 != NILATOM) goto empty__sub_4_6;
  x0 = makesym(atom_yes);
  unify_value(a2, x0);
  proceed();
 case FUNCTOR:
  if (functor_of(a0) != makesym(functor_n_4)) goto empty__sub_4_6;
  x0 = arg(a0, 1);
 empty__sub_4_2:
  if (isref(x0)) goto empty__sub_4_5;
 empty__sub_4_3:
  if (isref(x0)) goto empty__sub_4_4;
  if_not_equal(x0, a1, empty__sub_4_6);
  x1 = makesym(atom_no);
  unify_value(a2, x1);
  proceed();
 empty__sub_4_4:
  deref_and_jump(x0,empty__sub_4_3);
  *reasonp++ =  x0;
  goto empty__sub_4_6;
 empty__sub_4_5:
  deref_and_jump(x0,empty__sub_4_2);
  *reasonp++ =  x0;
  goto empty__sub_4_6;
 case VARREF:
  deref_and_jump(a0,empty__sub_4_1);
  *reasonp++ =  a0;
  goto empty__sub_4_6;
  };
  goto empty__sub_4_6;
 empty__sub_4_6:
  otherwise(empty__sub_4_interrupt);
 empty__sub_4_7:
  switch (ptagof(a0)) {
 case FUNCTOR:
  if (functor_of(a0) != makesym(functor_n_4)) goto empty__sub_4_interrupt;
  generic_arg[0] = a1;
  x0 = arg(a0, 1);
  generic_arg[1] = x0;
  allocp[0] = x1 = makeref(&allocp[0]);
  generic_arg[2] = x1;
  allocp[1] = x2 = makeref(&allocp[1]);
  generic_arg[3] = x2;
  allocp[2] = x3 = makeref(&allocp[2]);
  generic_arg[4] = x3;
  call_generic(a3, functor_call_5, 3);
  x4 = arg(a0, 0);
  x5 = a1;
  x6 = a2;
  a4 = a3;
  a5 = arg(a0, 3);
  a3 = x6;
  a2 = x5;
  a1 = x4;
  a0 = x3;
  execute(empty__sub_2F4_240_6_0);
  goto empty__sub_2F4_240_6_ext_interrupt;
 case VARREF:
  deref_and_jump(a0,empty__sub_4_7);
  *reasonp++ =  a0;
  goto empty__sub_4_interrupt;
  };
  goto empty__sub_4_interrupt;
 empty__sub_4_ext_interrupt:
  reasonp = 0l;
 empty__sub_4_interrupt:
  toppred = &predicate_splay__tree_xempty__sub_4;
  goto interrupt_4;
 }

 empty__sub_2F4_240_6_top: {
  a1 = qp->args[1];
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  qp = qp->next;
 empty__sub_2F4_240_6_clear_reason:
  reasonp = reasons;
 empty__sub_2F4_240_6_0:
 empty__sub_2F4_240_6_1:
  switch (ptagof(a0)) {
 case ATOMIC:
  if (isint(a0)) goto empty__sub_2F4_240_6_interrupt;
  switch (symval(a0)) {
 case symval(makesym(atom_no)):
  a0 = a1;
  a1 = a2;
  a2 = a3;
  a3 = a4;
  execute(empty__sub_4_0);
  goto empty__sub_4_ext_interrupt;
 case symval(makesym(atom_yes)):
  a0 = a5;
  a1 = a2;
  a2 = a3;
  a3 = a4;
  execute(empty__sub_4_0);
  goto empty__sub_4_ext_interrupt;
  };
  goto empty__sub_2F4_240_6_interrupt;
 case VARREF:
  deref_and_jump(a0,empty__sub_2F4_240_6_1);
  *reasonp++ =  a0;
  goto empty__sub_2F4_240_6_interrupt;
  };
  goto empty__sub_2F4_240_6_interrupt;
 empty__sub_2F4_240_6_ext_interrupt:
  reasonp = 0l;
 empty__sub_2F4_240_6_interrupt:
  toppred = &predicate_splay__tree_xempty__sub_2F4_240_6;
  goto interrupt_6;
 }

 update_5_top: {
  q x0, x1, x2, x3, x4, x5, x6;
  a1 = qp->args[1];
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  qp = qp->next;
 update_5_clear_reason:
  reasonp = reasons;
 update_5_0:
 update_5_1:
  switch (ptagof(a3)) {
 case FUNCTOR:
  if (functor_of(a3) != makesym(functor_pool_2)) goto update_5_interrupt;
  allocp[0] = makesym(functor_pool_2);
  x1 = arg(a3, 0);
  allocp[1] = x1;
  allocp[2] = x2 = makeref(&allocp[2]);
  x0 = makefunctor(&allocp[0]);
  allocp += 3;
  unify_value(a4, x0);
  x3 = arg(a3, 1);
  x4 = a0;
  x5 = a1;
  x6 = a2;
  a4 = x2;
  a5 = arg(a3, 0);
  a3 = x6;
  a2 = x5;
  a1 = x4;
  a0 = x3;
  execute(update_6_0);
  goto update_6_ext_interrupt;
 case VARREF:
  deref_and_jump(a3,update_5_1);
  *reasonp++ =  a3;
  goto update_5_interrupt;
  };
  goto update_5_interrupt;
 update_5_ext_interrupt:
  reasonp = 0l;
 update_5_interrupt:
  goto interrupt_5;
 }

 put_4_top: {
  q x0, x1, x2, x3, x4, x5, x6;
  a1 = qp->args[1];
  a2 = qp->args[2];
  a3 = qp->args[3];
  qp = qp->next;
 put_4_clear_reason:
  reasonp = reasons;
 put_4_0:
 put_4_1:
  switch (ptagof(a2)) {
 case FUNCTOR:
  if (functor_of(a2) != makesym(functor_pool_2)) goto put_4_interrupt;
  allocp[0] = makesym(functor_pool_2);
  x1 = arg(a2, 0);
  allocp[1] = x1;
  allocp[2] = x2 = makeref(&allocp[2]);
  x0 = makefunctor(&allocp[0]);
  allocp += 3;
  unify_value(a3, x0);
  x3 = arg(a2, 1);
  x4 = a0;
  allocp[0] = x6 = makeref(&allocp[0]);
  x5 = x6;
  a3 = a1;
  a4 = x2;
  a5 = arg(a2, 0);
  a2 = x5;
  a1 = x4;
  a0 = x3;
  allocp += 1;
  execute(update_6_0);
  goto update_6_ext_interrupt;
 case VARREF:
  deref_and_jump(a2,put_4_1);
  *reasonp++ =  a2;
  goto put_4_interrupt;
  };
  goto put_4_interrupt;
 put_4_ext_interrupt:
  reasonp = 0l;
 put_4_interrupt:
  goto interrupt_4;
 }

 update_6_top: {
  q x0, x1, x2, x3;
  a1 = qp->args[1];
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  qp = qp->next;
 update_6_clear_reason:
  reasonp = reasons;
 update_6_0:
  allocp[0] = makesym(functor_n_4);
  allocp[1] = x1 = makeref(&allocp[1]);
  allocp[2] = a1;
  allocp[3] = x2 = makeref(&allocp[3]);
  allocp[4] = x3 = makeref(&allocp[4]);
  x0 = makefunctor(&allocp[0]);
  allocp += 5;
  unify_value(a4, x0);
  a4 = x1;
  a6 = x3;
  a7 = a5;
  a5 = x2;
  execute(update_8_0);
  goto update_8_ext_interrupt;
 update_6_ext_interrupt:
  reasonp = 0l;
 update_6_interrupt:
  toppred = &predicate_splay__tree_xupdate_6;
  goto interrupt_6;
 }

 update_8_top: {
  q x0, x1, x2, x3, x4, x5, x6, x7, x8, x9;
  a1 = qp->args[1];
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  a6 = qp->args[6];
  a7 = qp->args[7];
  qp = qp->next;
 update_8_clear_reason:
  reasonp = reasons;
 update_8_0:
 update_8_1:
  switch (ptagof(a0)) {
 case ATOMIC:
  if (a0 != NILATOM) goto update_8_6;
  x0 = NILATOM;
  unify_value(a4, x0);
  unify(a5, a3);
  x1 = NILATOM;
  unify_value(a6, x1);
  proceed();
 case FUNCTOR:
  if (functor_of(a0) != makesym(functor_n_4)) goto update_8_6;
  x0 = arg(a0, 1);
 update_8_2:
  if (isref(x0)) goto update_8_5;
 update_8_3:
  if (isref(x0)) goto update_8_4;
  if_not_equal(x0, a1, update_8_6);
  x1 = arg(a0, 2);
  unify2(a2, x1, a5, a3);
  x2 = arg(a0, 0);
  unify(a4, x2);
  x3 = arg(a0, 3);
  unify(a6, x3);
  proceed();
 update_8_4:
  deref_and_jump(x0,update_8_3);
  *reasonp++ =  x0;
  goto update_8_6;
 update_8_5:
  deref_and_jump(x0,update_8_2);
  *reasonp++ =  x0;
  goto update_8_6;
 case VARREF:
  deref_and_jump(a0,update_8_1);
  *reasonp++ =  a0;
  goto update_8_6;
  };
  goto update_8_6;
 update_8_6:
  otherwise(update_8_interrupt);
 update_8_7:
  switch (ptagof(a0)) {
 case FUNCTOR:
  if (functor_of(a0) != makesym(functor_n_4)) goto update_8_interrupt;
  generic_arg[0] = a1;
  x0 = arg(a0, 1);
  generic_arg[1] = x0;
  allocp[0] = x1 = makeref(&allocp[0]);
  generic_arg[2] = x1;
  allocp[1] = x2 = makeref(&allocp[1]);
  generic_arg[3] = x2;
  allocp[2] = x3 = makeref(&allocp[2]);
  generic_arg[4] = x3;
  call_generic(a7, functor_call_5, 3);
  x4 = arg(a0, 0);
  x5 = a1;
  x6 = a2;
  x7 = a3;
  x8 = arg(a0, 2);
  x9 = arg(a0, 3);
  a8 = a4;
  a9 = a5;
  a10 = a6;
  a11 = a7;
  a7 = x9;
  a6 = x8;
  a5 = x0;
  a4 = x7;
  a3 = x6;
  a2 = x5;
  a1 = x4;
  a0 = x3;
  execute(update_2F8_240_12_0);
  goto update_2F8_240_12_ext_interrupt;
 case VARREF:
  deref_and_jump(a0,update_8_7);
  *reasonp++ =  a0;
  goto update_8_interrupt;
  };
  goto update_8_interrupt;
 update_8_ext_interrupt:
  reasonp = 0l;
 update_8_interrupt:
  toppred = &predicate_splay__tree_xupdate_8;
  goto interrupt_8;
 }

 update_2F8_240_12_top: {
  q x0;
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
 update_2F8_240_12_clear_reason:
  reasonp = reasons;
 update_2F8_240_12_0:
 update_2F8_240_12_1:
  switch (ptagof(a0)) {
 case ATOMIC:
  if (isint(a0)) goto update_2F8_240_12_interrupt;
  switch (symval(a0)) {
 case symval(makesym(atom_no)):
  a0 = a1;
  a1 = a2;
  a2 = a3;
  a3 = a4;
  a4 = a5;
  a5 = a6;
  a6 = a7;
  a7 = a8;
  a8 = a9;
  a9 = a10;
  a10 = a11;
  execute(update__left_11_0);
  goto update__left_11_ext_interrupt;
 case symval(makesym(atom_yes)):
  a0 = a7;
  x0 = a2;
  a2 = a3;
  a3 = a4;
  a4 = a5;
  a5 = a6;
  a6 = a1;
  a7 = a8;
  a8 = a9;
  a9 = a10;
  a10 = a11;
  a1 = x0;
  execute(update__right_11_0);
  goto update__right_11_ext_interrupt;
  };
  goto update_2F8_240_12_interrupt;
 case VARREF:
  deref_and_jump(a0,update_2F8_240_12_1);
  *reasonp++ =  a0;
  goto update_2F8_240_12_interrupt;
  };
  goto update_2F8_240_12_interrupt;
 update_2F8_240_12_ext_interrupt:
  reasonp = 0l;
 update_2F8_240_12_interrupt:
  toppred = &predicate_splay__tree_xupdate_2F8_240_12;
  goto interrupt_12;
 }

 update__left_11_top: {
  q x0, x1, x2, x3, x4, x5, x6, x7, x8, x9;
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
  qp = qp->next;
 update__left_11_clear_reason:
  reasonp = reasons;
 update__left_11_0:
 update__left_11_1:
  switch (ptagof(a0)) {
 case ATOMIC:
  if (a0 != NILATOM) goto update__left_11_6;
  x0 = NILATOM;
  unify_value(a7, x0);
  unify(a8, a3);
  allocp[0] = makesym(functor_n_4);
  allocp[1] = NILATOM;
  allocp[2] = a4;
  allocp[3] = a5;
  allocp[4] = a6;
  x1 = makefunctor(&allocp[0]);
  allocp += 5;
  unify_value(a9, x1);
  proceed();
 case FUNCTOR:
  if (functor_of(a0) != makesym(functor_n_4)) goto update__left_11_6;
  x0 = arg(a0, 1);
 update__left_11_2:
  if (isref(x0)) goto update__left_11_5;
 update__left_11_3:
  if (isref(x0)) goto update__left_11_4;
  if_not_equal(x0, a1, update__left_11_6);
  x1 = arg(a0, 2);
  unify2(a2, x1, a8, a3);
  x2 = arg(a0, 0);
  unify(a7, x2);
  allocp[0] = makesym(functor_n_4);
  x4 = arg(a0, 3);
  allocp[1] = x4;
  allocp[2] = a4;
  allocp[3] = a5;
  allocp[4] = a6;
  x3 = makefunctor(&allocp[0]);
  allocp += 5;
  unify_value(a9, x3);
  proceed();
 update__left_11_4:
  deref_and_jump(x0,update__left_11_3);
  *reasonp++ =  x0;
  goto update__left_11_6;
 update__left_11_5:
  deref_and_jump(x0,update__left_11_2);
  *reasonp++ =  x0;
  goto update__left_11_6;
 case VARREF:
  deref_and_jump(a0,update__left_11_1);
  *reasonp++ =  a0;
  goto update__left_11_6;
  };
  goto update__left_11_6;
 update__left_11_6:
  otherwise(update__left_11_interrupt);
 update__left_11_7:
  switch (ptagof(a0)) {
 case FUNCTOR:
  if (functor_of(a0) != makesym(functor_n_4)) goto update__left_11_interrupt;
  x0 = arg(a0, 1);
  generic_arg[0] = x0;
  generic_arg[1] = a1;
  allocp[0] = x1 = makeref(&allocp[0]);
  generic_arg[2] = x1;
  allocp[1] = x2 = makeref(&allocp[1]);
  generic_arg[3] = x2;
  allocp[2] = x3 = makeref(&allocp[2]);
  generic_arg[4] = x3;
  call_generic(a10, functor_call_5, 3);
  x4 = arg(a0, 0);
  x5 = a1;
  x6 = a2;
  x7 = a3;
  x8 = a7;
  x9 = a8;
  a7 = a10;
  a8 = a9;
  a9 = x0;
  a10 = arg(a0, 2);
  a11 = arg(a0, 3);
  a12 = a4;
  a13 = a5;
  a14 = a6;
  a6 = x9;
  a5 = x8;
  a4 = x7;
  a3 = x6;
  a2 = x5;
  a1 = x4;
  a0 = x3;
  execute(update__left_2F11_240_15_0);
  goto update__left_2F11_240_15_ext_interrupt;
 case VARREF:
  deref_and_jump(a0,update__left_11_7);
  *reasonp++ =  a0;
  goto update__left_11_interrupt;
  };
  goto update__left_11_interrupt;
 update__left_11_ext_interrupt:
  reasonp = 0l;
 update__left_11_interrupt:
  toppred = &predicate_splay__tree_xupdate__left_11;
  goto interrupt_11;
 }

 update__left_2F11_240_15_top: {
  q x0, x1, x2, x3;
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
  a12 = qp->args[12];
  a13 = qp->args[13];
  a14 = qp->args[14];
  qp = qp->next;
 update__left_2F11_240_15_clear_reason:
  reasonp = reasons;
 update__left_2F11_240_15_0:
 update__left_2F11_240_15_1:
  switch (ptagof(a0)) {
 case ATOMIC:
  if (isint(a0)) goto update__left_2F11_240_15_interrupt;
  switch (symval(a0)) {
 case symval(makesym(atom_yes)):
  allocp[0] = makesym(functor_n_4);
  allocp[1] = a11;
  allocp[2] = a12;
  allocp[3] = a13;
  allocp[4] = a14;
  x0 = makefunctor(&allocp[0]);
  allocp[5] = makesym(functor_n_4);
  allocp[6] = x2 = makeref(&allocp[6]);
  allocp[7] = a9;
  allocp[8] = a10;
  allocp[9] = x0;
  x1 = makefunctor(&allocp[5]);
  allocp += 10;
  unify_value(a8, x1);
  a0 = a1;
  a1 = a2;
  a2 = a3;
  a3 = a4;
  a4 = a5;
  a5 = a6;
  a6 = x2;
  execute(update_8_0);
  goto update_8_ext_interrupt;
 case symval(makesym(atom_no)):
  allocp[0] = makesym(functor_n_4);
  allocp[1] = x1 = makeref(&allocp[1]);
  allocp[2] = a12;
  allocp[3] = a13;
  allocp[4] = a14;
  x0 = makefunctor(&allocp[0]);
  allocp += 5;
  unify_value(a8, x0);
  allocp[0] = makesym(functor_n_4);
  allocp[1] = a1;
  allocp[2] = a9;
  allocp[3] = a10;
  allocp[4] = x3 = makeref(&allocp[4]);
  x2 = makefunctor(&allocp[0]);
  allocp += 5;
  unify_value(a5, x2);
  a0 = a11;
  a1 = a2;
  a2 = a3;
  a3 = a4;
  a4 = x3;
  a5 = a6;
  a6 = x1;
  execute(update_8_0);
  goto update_8_ext_interrupt;
  };
  goto update__left_2F11_240_15_interrupt;
 case VARREF:
  deref_and_jump(a0,update__left_2F11_240_15_1);
  *reasonp++ =  a0;
  goto update__left_2F11_240_15_interrupt;
  };
  goto update__left_2F11_240_15_interrupt;
 update__left_2F11_240_15_ext_interrupt:
  reasonp = 0l;
 update__left_2F11_240_15_interrupt:
  toppred = &predicate_splay__tree_xupdate__left_2F11_240_15;
  goto interrupt_15;
 }

 update__right_11_top: {
  q x0, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10;
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
  qp = qp->next;
 update__right_11_clear_reason:
  reasonp = reasons;
 update__right_11_0:
 update__right_11_1:
  switch (ptagof(a0)) {
 case ATOMIC:
  if (a0 != NILATOM) goto update__right_11_6;
  allocp[0] = makesym(functor_n_4);
  allocp[1] = a6;
  allocp[2] = a4;
  allocp[3] = a5;
  allocp[4] = NILATOM;
  x0 = makefunctor(&allocp[0]);
  allocp += 5;
  unify_value(a7, x0);
  unify(a8, a3);
  x1 = NILATOM;
  unify_value(a9, x1);
  proceed();
 case FUNCTOR:
  if (functor_of(a0) != makesym(functor_n_4)) goto update__right_11_6;
  x0 = arg(a0, 1);
 update__right_11_2:
  if (isref(x0)) goto update__right_11_5;
 update__right_11_3:
  if (isref(x0)) goto update__right_11_4;
  if_not_equal(x0, a1, update__right_11_6);
  x1 = arg(a0, 2);
  unify2(a2, x1, a8, a3);
  allocp[0] = makesym(functor_n_4);
  allocp[1] = a6;
  allocp[2] = a4;
  allocp[3] = a5;
  x3 = arg(a0, 0);
  allocp[4] = x3;
  x2 = makefunctor(&allocp[0]);
  allocp += 5;
  unify_value(a7, x2);
  x4 = arg(a0, 3);
  unify(a9, x4);
  proceed();
 update__right_11_4:
  deref_and_jump(x0,update__right_11_3);
  *reasonp++ =  x0;
  goto update__right_11_6;
 update__right_11_5:
  deref_and_jump(x0,update__right_11_2);
  *reasonp++ =  x0;
  goto update__right_11_6;
 case VARREF:
  deref_and_jump(a0,update__right_11_1);
  *reasonp++ =  a0;
  goto update__right_11_6;
  };
  goto update__right_11_6;
 update__right_11_6:
  otherwise(update__right_11_interrupt);
 update__right_11_7:
  switch (ptagof(a0)) {
 case FUNCTOR:
  if (functor_of(a0) != makesym(functor_n_4)) goto update__right_11_interrupt;
  x0 = arg(a0, 1);
  generic_arg[0] = x0;
  generic_arg[1] = a1;
  allocp[0] = x1 = makeref(&allocp[0]);
  generic_arg[2] = x1;
  allocp[1] = x2 = makeref(&allocp[1]);
  generic_arg[3] = x2;
  allocp[2] = x3 = makeref(&allocp[2]);
  generic_arg[4] = x3;
  call_generic(a10, functor_call_5, 3);
  x4 = arg(a0, 0);
  x5 = a1;
  x6 = a2;
  x7 = a3;
  x8 = a8;
  x9 = a10;
  a8 = a6;
  x10 = a4;
  a10 = a5;
  a11 = a9;
  a12 = x0;
  a13 = arg(a0, 2);
  a14 = arg(a0, 3);
  a9 = x10;
  a6 = x9;
  a5 = x8;
  a4 = x7;
  a3 = x6;
  a2 = x5;
  a1 = x4;
  a0 = x3;
  execute(update__right_2F11_240_15_0);
  goto update__right_2F11_240_15_ext_interrupt;
 case VARREF:
  deref_and_jump(a0,update__right_11_7);
  *reasonp++ =  a0;
  goto update__right_11_interrupt;
  };
  goto update__right_11_interrupt;
 update__right_11_ext_interrupt:
  reasonp = 0l;
 update__right_11_interrupt:
  toppred = &predicate_splay__tree_xupdate__right_11;
  goto interrupt_11;
 }

 update__right_2F11_240_15_top: {
  q x0, x1, x2, x3;
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
  a12 = qp->args[12];
  a13 = qp->args[13];
  a14 = qp->args[14];
  qp = qp->next;
 update__right_2F11_240_15_clear_reason:
  reasonp = reasons;
 update__right_2F11_240_15_0:
 update__right_2F11_240_15_1:
  switch (ptagof(a0)) {
 case ATOMIC:
  if (isint(a0)) goto update__right_2F11_240_15_interrupt;
  switch (symval(a0)) {
 case symval(makesym(atom_yes)):
  allocp[0] = makesym(functor_n_4);
  allocp[1] = x1 = makeref(&allocp[1]);
  allocp[2] = a12;
  allocp[3] = a13;
  allocp[4] = a14;
  x0 = makefunctor(&allocp[0]);
  allocp += 5;
  unify_value(a11, x0);
  allocp[0] = makesym(functor_n_4);
  allocp[1] = a8;
  allocp[2] = a9;
  allocp[3] = a10;
  allocp[4] = x3 = makeref(&allocp[4]);
  x2 = makefunctor(&allocp[0]);
  allocp += 5;
  unify_value(a7, x2);
  a0 = a1;
  a1 = a2;
  a2 = a3;
  a3 = a4;
  a4 = x3;
  a7 = a6;
  a6 = x1;
  execute(update_8_0);
  goto update_8_ext_interrupt;
 case symval(makesym(atom_no)):
  allocp[0] = makesym(functor_n_4);
  allocp[1] = a8;
  allocp[2] = a9;
  allocp[3] = a10;
  allocp[4] = a1;
  x0 = makefunctor(&allocp[0]);
  allocp[5] = makesym(functor_n_4);
  allocp[6] = x0;
  allocp[7] = a12;
  allocp[8] = a13;
  allocp[9] = x2 = makeref(&allocp[9]);
  x1 = makefunctor(&allocp[5]);
  allocp += 10;
  unify_value(a7, x1);
  a0 = a14;
  a1 = a2;
  a2 = a3;
  a3 = a4;
  a4 = x2;
  x3 = a11;
  a7 = a6;
  a6 = x3;
  execute(update_8_0);
  goto update_8_ext_interrupt;
  };
  goto update__right_2F11_240_15_interrupt;
 case VARREF:
  deref_and_jump(a0,update__right_2F11_240_15_1);
  *reasonp++ =  a0;
  goto update__right_2F11_240_15_interrupt;
  };
  goto update__right_2F11_240_15_interrupt;
 update__right_2F11_240_15_ext_interrupt:
  reasonp = 0l;
 update__right_2F11_240_15_interrupt:
  toppred = &predicate_splay__tree_xupdate__right_2F11_240_15;
  goto interrupt_15;
 }

 get_4_top: {
  q x0, x1, x2, x3, x4, x5;
  a1 = qp->args[1];
  a2 = qp->args[2];
  a3 = qp->args[3];
  qp = qp->next;
 get_4_clear_reason:
  reasonp = reasons;
 get_4_0:
 get_4_1:
  switch (ptagof(a2)) {
 case FUNCTOR:
  if (functor_of(a2) != makesym(functor_pool_2)) goto get_4_interrupt;
  allocp[0] = makesym(functor_pool_2);
  x1 = arg(a2, 0);
  allocp[1] = x1;
  allocp[2] = x2 = makeref(&allocp[2]);
  x0 = makefunctor(&allocp[0]);
  allocp += 3;
  unify_value(a3, x0);
  x3 = arg(a2, 1);
  x4 = a0;
  x5 = a1;
  a3 = x2;
  a4 = arg(a2, 0);
  a2 = x5;
  a1 = x4;
  a0 = x3;
  execute(get_5_0);
  goto get_5_ext_interrupt;
 case VARREF:
  deref_and_jump(a2,get_4_1);
  *reasonp++ =  a2;
  goto get_4_interrupt;
  };
  goto get_4_interrupt;
 get_4_ext_interrupt:
  reasonp = 0l;
 get_4_interrupt:
  goto interrupt_4;
 }

 get_5_top: {
  q x0, x1, x2, x3, x4, x5;
  a1 = qp->args[1];
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  qp = qp->next;
 get_5_clear_reason:
  reasonp = reasons;
 get_5_0:
 get_5_1:
  switch (ptagof(a0)) {
 case ATOMIC:
  if (a0 != NILATOM) goto get_5_6;
  x0 = NILATOM;
  unify_value(a2, x0);
  x1 = NILATOM;
  unify_value(a3, x1);
  proceed();
 case FUNCTOR:
  if (functor_of(a0) != makesym(functor_n_4)) goto get_5_6;
  x0 = arg(a0, 1);
 get_5_2:
  if (isref(x0)) goto get_5_5;
 get_5_3:
  if (isref(x0)) goto get_5_4;
  if_not_equal(x0, a1, get_5_6);
  x1 = arg(a0, 2);
  unify(a2, x1);
  x2 = arg(a0, 0);
  a1 = a3;
  a2 = arg(a0, 3);
  a0 = x2;
  execute(get_2F5_241_3_clear_reason);
  goto get_2F5_241_3_ext_interrupt;
 get_5_4:
  deref_and_jump(x0,get_5_3);
  *reasonp++ =  x0;
  goto get_5_6;
 get_5_5:
  deref_and_jump(x0,get_5_2);
  *reasonp++ =  x0;
  goto get_5_6;
 case VARREF:
  deref_and_jump(a0,get_5_1);
  *reasonp++ =  a0;
  goto get_5_6;
  };
  goto get_5_6;
 get_5_6:
  otherwise(get_5_interrupt);
 get_5_7:
  switch (ptagof(a0)) {
 case FUNCTOR:
  if (functor_of(a0) != makesym(functor_n_4)) goto get_5_interrupt;
  generic_arg[0] = a1;
  x0 = arg(a0, 1);
  generic_arg[1] = x0;
  allocp[0] = x1 = makeref(&allocp[0]);
  generic_arg[2] = x1;
  allocp[1] = x2 = makeref(&allocp[1]);
  generic_arg[3] = x2;
  allocp[2] = x3 = makeref(&allocp[2]);
  generic_arg[4] = x3;
  call_generic(a4, functor_call_5, 3);
  x4 = a3;
  a3 = arg(a0, 2);
  x5 = arg(a0, 3);
  a5 = arg(a0, 0);
  a6 = a1;
  a7 = a2;
  a8 = a4;
  a4 = x5;
  a2 = x0;
  a1 = x4;
  a0 = x3;
  execute(get_2F5_240_9_0);
  goto get_2F5_240_9_ext_interrupt;
 case VARREF:
  deref_and_jump(a0,get_5_7);
  *reasonp++ =  a0;
  goto get_5_interrupt;
  };
  goto get_5_interrupt;
 get_5_ext_interrupt:
  reasonp = 0l;
 get_5_interrupt:
  toppred = &predicate_splay__tree_xget_5;
  goto interrupt_5;
 }

 get_2F5_241_3_top: {
  q x0, x1, x2, x3, x4;
  a1 = qp->args[1];
  a2 = qp->args[2];
  qp = qp->next;
 get_2F5_241_3_clear_reason:
  reasonp = reasons;
 get_2F5_241_3_0:
 get_2F5_241_3_1:
  switch (ptagof(a0)) {
 case ATOMIC:
  if (a0 != NILATOM) goto get_2F5_241_3_interrupt;
  unify(a1, a2);
  proceed();
 case FUNCTOR:
  if (functor_of(a0) != makesym(functor_n_4)) goto get_2F5_241_3_interrupt;
  allocp[0] = makesym(functor_n_4);
  x1 = arg(a0, 0);
  allocp[1] = x1;
  x2 = arg(a0, 1);
  allocp[2] = x2;
  x3 = arg(a0, 2);
  allocp[3] = x3;
  allocp[4] = x4 = makeref(&allocp[4]);
  x0 = makefunctor(&allocp[0]);
  allocp += 5;
  unify_value(a1, x0);
  a0 = arg(a0, 3);
  a1 = a2;
  a2 = x4;
  execute(reform__tree_3_0);
  goto reform__tree_3_ext_interrupt;
 case VARREF:
  deref_and_jump(a0,get_2F5_241_3_1);
  *reasonp++ =  a0;
  goto get_2F5_241_3_interrupt;
  };
  goto get_2F5_241_3_interrupt;
 get_2F5_241_3_ext_interrupt:
  reasonp = 0l;
 get_2F5_241_3_interrupt:
  toppred = &predicate_splay__tree_xget_2F5_241_3;
  goto interrupt_3;
 }

 get_2F5_240_9_top: {
  q x0, x1;
  a1 = qp->args[1];
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  a6 = qp->args[6];
  a7 = qp->args[7];
  a8 = qp->args[8];
  qp = qp->next;
 get_2F5_240_9_clear_reason:
  reasonp = reasons;
 get_2F5_240_9_0:
 get_2F5_240_9_1:
  switch (ptagof(a0)) {
 case ATOMIC:
  if (isint(a0)) goto get_2F5_240_9_interrupt;
  switch (symval(a0)) {
 case symval(makesym(atom_no)):
  allocp[0] = makesym(functor_n_4);
  allocp[1] = x1 = makeref(&allocp[1]);
  allocp[2] = a2;
  allocp[3] = a3;
  allocp[4] = a4;
  x0 = makefunctor(&allocp[0]);
  allocp += 5;
  unify_value(a1, x0);
  a0 = a5;
  a1 = a6;
  a2 = a7;
  a3 = x1;
  a4 = a8;
  execute(get_5_0);
  goto get_5_ext_interrupt;
 case symval(makesym(atom_yes)):
  allocp[0] = makesym(functor_n_4);
  allocp[1] = a5;
  allocp[2] = a2;
  allocp[3] = a3;
  allocp[4] = x1 = makeref(&allocp[4]);
  x0 = makefunctor(&allocp[0]);
  allocp += 5;
  unify_value(a1, x0);
  a0 = a4;
  a1 = a6;
  a2 = a7;
  a3 = x1;
  a4 = a8;
  execute(get_5_0);
  goto get_5_ext_interrupt;
  };
  goto get_2F5_240_9_interrupt;
 case VARREF:
  deref_and_jump(a0,get_2F5_240_9_1);
  *reasonp++ =  a0;
  goto get_2F5_240_9_interrupt;
  };
  goto get_2F5_240_9_interrupt;
 get_2F5_240_9_ext_interrupt:
  reasonp = 0l;
 get_2F5_240_9_interrupt:
  toppred = &predicate_splay__tree_xget_2F5_240_9;
  goto interrupt_9;
 }

 get__all_3_top: {
  q x0, x1, x2;
  a1 = qp->args[1];
  a2 = qp->args[2];
  qp = qp->next;
 get__all_3_clear_reason:
  reasonp = reasons;
 get__all_3_0:
 get__all_3_1:
  switch (ptagof(a1)) {
 case FUNCTOR:
  if (functor_of(a1) != makesym(functor_pool_2)) goto get__all_3_interrupt;
  allocp[0] = makesym(functor_pool_2);
  x1 = arg(a1, 0);
  allocp[1] = x1;
  allocp[2] = NILATOM;
  x0 = makefunctor(&allocp[0]);
  allocp += 3;
  unify_value(a2, x0);
  x2 = arg(a1, 1);
  a1 = a0;
  a2 = NILATOM;
  a0 = x2;
  execute(get__all__sub_3_0);
  goto get__all__sub_3_ext_interrupt;
 case VARREF:
  deref_and_jump(a1,get__all_3_1);
  *reasonp++ =  a1;
  goto get__all_3_interrupt;
  };
  goto get__all_3_interrupt;
 get__all_3_ext_interrupt:
  reasonp = 0l;
 get__all_3_interrupt:
  goto interrupt_3;
 }

 get__all__sub_3_top: {
  q x0, x1, x2, x3, x4, x5;
  a1 = qp->args[1];
  a2 = qp->args[2];
  qp = qp->next;
 get__all__sub_3_clear_reason:
  reasonp = reasons;
 get__all__sub_3_0:
 get__all__sub_3_1:
  switch (ptagof(a0)) {
 case ATOMIC:
  if (a0 != NILATOM) goto get__all__sub_3_interrupt;
  unify(a1, a2);
  proceed();
 case FUNCTOR:
  if (functor_of(a0) != makesym(functor_n_4)) goto get__all__sub_3_interrupt;
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_splay__tree_xget__all__sub_3);
  x0 = arg(a0, 3);
  allocp[2] = x0;
  allocp[3] = x1 = makeref(&allocp[3]);
  allocp[4] = a2;
  allocp[5] = makesym(functor__3D_2);
  x3 = arg(a0, 1);
  allocp[6] = x3;
  x4 = arg(a0, 2);
  allocp[7] = x4;
  x2 = makefunctor(&allocp[5]);
  allocp[8] = x1;
  allocp[9] = x2;
  x5 = makecons(&allocp[8]);
  a0 = arg(a0, 0);
  a2 = x5;
  qp = (struct goalrec*)&allocp[0];
  allocp += 10;
  execute(get__all__sub_3_0);
  goto get__all__sub_3_ext_interrupt;
 case VARREF:
  deref_and_jump(a0,get__all__sub_3_1);
  *reasonp++ =  a0;
  goto get__all__sub_3_interrupt;
  };
  goto get__all__sub_3_interrupt;
 get__all__sub_3_ext_interrupt:
  reasonp = 0l;
 get__all__sub_3_interrupt:
  toppred = &predicate_splay__tree_xget__all__sub_3;
  goto interrupt_3;
 }

 get__max_4_top: {
  q x0, x1, x2, x3, x4;
  a1 = qp->args[1];
  a2 = qp->args[2];
  a3 = qp->args[3];
  qp = qp->next;
 get__max_4_clear_reason:
  reasonp = reasons;
 get__max_4_0:
 get__max_4_1:
  switch (ptagof(a2)) {
 case FUNCTOR:
  if (functor_of(a2) != makesym(functor_pool_2)) goto get__max_4_interrupt;
  allocp[0] = makesym(functor_pool_2);
  x1 = arg(a2, 0);
  allocp[1] = x1;
  allocp[2] = x2 = makeref(&allocp[2]);
  x0 = makefunctor(&allocp[0]);
  allocp += 3;
  unify_value(a3, x0);
  x3 = arg(a2, 1);
  x4 = a0;
  a2 = a1;
  a3 = x2;
  a1 = x4;
  a0 = x3;
  execute(get__max__sub_4_0);
  goto get__max__sub_4_ext_interrupt;
 case VARREF:
  deref_and_jump(a2,get__max_4_1);
  *reasonp++ =  a2;
  goto get__max_4_interrupt;
  };
  goto get__max_4_interrupt;
 get__max_4_ext_interrupt:
  reasonp = 0l;
 get__max_4_interrupt:
  goto interrupt_4;
 }

 get__max__sub_4_top: {
  q x0, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10;
  a1 = qp->args[1];
  a2 = qp->args[2];
  a3 = qp->args[3];
  qp = qp->next;
 get__max__sub_4_clear_reason:
  reasonp = reasons;
 get__max__sub_4_0:
 get__max__sub_4_1:
  switch (ptagof(a0)) {
 case FUNCTOR:
  if (functor_of(a0) != makesym(functor_n_4)) goto get__max__sub_4_interrupt;
  x0 = arg(a0, 3);
 get__max__sub_4_2:
  switch (ptagof(x0)) {
 case ATOMIC:
  if (x0 != NILATOM) goto get__max__sub_4_interrupt;
  x1 = arg(a0, 1);
  unify(a1, x1);
  x2 = arg(a0, 2);
  unify(a2, x2);
  x3 = arg(a0, 0);
  unify(a3, x3);
  proceed();
 case FUNCTOR:
  if (functor_of(x0) != makesym(functor_n_4)) goto get__max__sub_4_interrupt;
  allocp[0] = makesym(functor_n_4);
  x2 = arg(a0, 0);
  allocp[1] = x2;
  x3 = arg(a0, 1);
  allocp[2] = x3;
  x4 = arg(a0, 2);
  allocp[3] = x4;
  allocp[4] = x5 = makeref(&allocp[4]);
  x1 = makefunctor(&allocp[0]);
  allocp += 5;
  unify_value(a3, x1);
  allocp[0] = makesym(functor_n_4);
  x7 = arg(x0, 0);
  allocp[1] = x7;
  x8 = arg(x0, 1);
  allocp[2] = x8;
  x9 = arg(x0, 2);
  allocp[3] = x9;
  x10 = arg(x0, 3);
  allocp[4] = x10;
  x6 = makefunctor(&allocp[0]);
  a0 = x6;
  a3 = x5;
  allocp += 5;
  execute(get__max__sub_4_0);
  goto get__max__sub_4_ext_interrupt;
 case VARREF:
  deref_and_jump(x0,get__max__sub_4_2);
  *reasonp++ =  x0;
  goto get__max__sub_4_interrupt;
  };
  goto get__max__sub_4_interrupt;
 case VARREF:
  deref_and_jump(a0,get__max__sub_4_1);
  *reasonp++ =  a0;
  goto get__max__sub_4_interrupt;
  };
  goto get__max__sub_4_interrupt;
 get__max__sub_4_ext_interrupt:
  reasonp = 0l;
 get__max__sub_4_interrupt:
  toppred = &predicate_splay__tree_xget__max__sub_4;
  goto interrupt_4;
 }

 get__min_4_top: {
  q x0, x1, x2, x3, x4;
  a1 = qp->args[1];
  a2 = qp->args[2];
  a3 = qp->args[3];
  qp = qp->next;
 get__min_4_clear_reason:
  reasonp = reasons;
 get__min_4_0:
 get__min_4_1:
  switch (ptagof(a2)) {
 case FUNCTOR:
  if (functor_of(a2) != makesym(functor_pool_2)) goto get__min_4_interrupt;
  allocp[0] = makesym(functor_pool_2);
  x1 = arg(a2, 0);
  allocp[1] = x1;
  allocp[2] = x2 = makeref(&allocp[2]);
  x0 = makefunctor(&allocp[0]);
  allocp += 3;
  unify_value(a3, x0);
  x3 = arg(a2, 1);
  x4 = a0;
  a2 = a1;
  a3 = x2;
  a1 = x4;
  a0 = x3;
  execute(get__min__sub_4_0);
  goto get__min__sub_4_ext_interrupt;
 case VARREF:
  deref_and_jump(a2,get__min_4_1);
  *reasonp++ =  a2;
  goto get__min_4_interrupt;
  };
  goto get__min_4_interrupt;
 get__min_4_ext_interrupt:
  reasonp = 0l;
 get__min_4_interrupt:
  goto interrupt_4;
 }

 get__min__sub_4_top: {
  q x0, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10;
  a1 = qp->args[1];
  a2 = qp->args[2];
  a3 = qp->args[3];
  qp = qp->next;
 get__min__sub_4_clear_reason:
  reasonp = reasons;
 get__min__sub_4_0:
 get__min__sub_4_1:
  switch (ptagof(a0)) {
 case FUNCTOR:
  if (functor_of(a0) != makesym(functor_n_4)) goto get__min__sub_4_interrupt;
  x0 = arg(a0, 0);
 get__min__sub_4_2:
  switch (ptagof(x0)) {
 case ATOMIC:
  if (x0 != NILATOM) goto get__min__sub_4_interrupt;
  x1 = arg(a0, 1);
  unify(a1, x1);
  x2 = arg(a0, 2);
  unify(a2, x2);
  x3 = arg(a0, 3);
  unify(a3, x3);
  proceed();
 case FUNCTOR:
  if (functor_of(x0) != makesym(functor_n_4)) goto get__min__sub_4_interrupt;
  allocp[0] = makesym(functor_n_4);
  allocp[1] = x2 = makeref(&allocp[1]);
  x3 = arg(a0, 1);
  allocp[2] = x3;
  x4 = arg(a0, 2);
  allocp[3] = x4;
  x5 = arg(a0, 3);
  allocp[4] = x5;
  x1 = makefunctor(&allocp[0]);
  allocp += 5;
  unify_value(a3, x1);
  allocp[0] = makesym(functor_n_4);
  x7 = arg(x0, 0);
  allocp[1] = x7;
  x8 = arg(x0, 1);
  allocp[2] = x8;
  x9 = arg(x0, 2);
  allocp[3] = x9;
  x10 = arg(x0, 3);
  allocp[4] = x10;
  x6 = makefunctor(&allocp[0]);
  a0 = x6;
  a3 = x2;
  allocp += 5;
  execute(get__min__sub_4_0);
  goto get__min__sub_4_ext_interrupt;
 case VARREF:
  deref_and_jump(x0,get__min__sub_4_2);
  *reasonp++ =  x0;
  goto get__min__sub_4_interrupt;
  };
  goto get__min__sub_4_interrupt;
 case VARREF:
  deref_and_jump(a0,get__min__sub_4_1);
  *reasonp++ =  a0;
  goto get__min__sub_4_interrupt;
  };
  goto get__min__sub_4_interrupt;
 get__min__sub_4_ext_interrupt:
  reasonp = 0l;
 get__min__sub_4_interrupt:
  toppred = &predicate_splay__tree_xget__min__sub_4;
  goto interrupt_4;
 }

 reform__tree_3_top: {
  q x0, x1, x2, x3, x4;
  a1 = qp->args[1];
  a2 = qp->args[2];
  qp = qp->next;
 reform__tree_3_clear_reason:
  reasonp = reasons;
 reform__tree_3_0:
 reform__tree_3_1:
  switch (ptagof(a0)) {
 case ATOMIC:
  if (a0 != NILATOM) goto reform__tree_3_interrupt;
  unify(a2, a1);
  proceed();
 case FUNCTOR:
  if (functor_of(a0) != makesym(functor_n_4)) goto reform__tree_3_interrupt;
  allocp[0] = makesym(functor_n_4);
  x1 = arg(a0, 0);
  allocp[1] = x1;
  x2 = arg(a0, 1);
  allocp[2] = x2;
  x3 = arg(a0, 2);
  allocp[3] = x3;
  allocp[4] = x4 = makeref(&allocp[4]);
  x0 = makefunctor(&allocp[0]);
  allocp += 5;
  unify_value(a2, x0);
  a0 = arg(a0, 3);
  a2 = x4;
  execute(reform__tree_3_0);
  goto reform__tree_3_ext_interrupt;
 case VARREF:
  deref_and_jump(a0,reform__tree_3_1);
  *reasonp++ =  a0;
  goto reform__tree_3_interrupt;
  };
  goto reform__tree_3_interrupt;
 reform__tree_3_ext_interrupt:
  reasonp = 0l;
 reform__tree_3_interrupt:
  toppred = &predicate_splay__tree_xreform__tree_3;
  goto interrupt_3;
 }
 interrupt_15:
  allocp[16] = a14;
 interrupt_14:
  allocp[15] = a13;
 interrupt_13:
  allocp[14] = a12;
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
  loop_within_module(module_splay__tree);
}
