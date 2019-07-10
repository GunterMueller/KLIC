/* Compiled by KLIC compiler version 3.003 (Mon Mar 29 19:42:59 JST 1999) */
#include <klic/klichdr.h>
#include "atom.h"
#include "funct.h"

module module_sorted__bag();
Const struct predicate predicate_sorted__bag_xcreate_1 =
   { module_sorted__bag, 0, 1 };
Const struct predicate predicate_sorted__bag_xcreate_2 =
   { module_sorted__bag, 1, 2 };
Const struct predicate predicate_sorted__bag_xmain_3 =
   { module_sorted__bag, 2, 3 };
Const struct predicate predicate_sorted__bag_xmessage_3 =
   { module_sorted__bag, 3, 3 };
Const struct predicate predicate_sorted__bag_xmessage_2F3_243_4 =
   { module_sorted__bag, 4, 4 };
Const struct predicate predicate_sorted__bag_xmessage_2F3_242_4 =
   { module_sorted__bag, 5, 4 };
Const struct predicate predicate_sorted__bag_xmessage_2F3_242_2F4_240_4 =
   { module_sorted__bag, 6, 4 };
Const struct predicate predicate_sorted__bag_xmessage_2F3_241_4 =
   { module_sorted__bag, 7, 4 };
Const struct predicate predicate_sorted__bag_xmessage_2F3_241_2F4_240_4 =
   { module_sorted__bag, 8, 4 };
Const struct predicate predicate_sorted__bag_xmessage_2F3_240_4 =
   { module_sorted__bag, 9, 4 };
Const struct predicate predicate_sorted__bag_xmessage_2F3_240_2F4_240_4 =
   { module_sorted__bag, 10, 4 };
Const struct predicate predicate_sorted__bag_xflatten_2 =
   { module_sorted__bag, 11, 2 };
Const struct predicate predicate_sorted__bag_xflatten_4 =
   { module_sorted__bag, 12, 4 };
Const struct predicate predicate_sorted__bag_xappend_3 =
   { module_sorted__bag, 13, 3 };
Const struct predicate predicate_sorted__bag_xlast__elem_3 =
   { module_sorted__bag, 14, 3 };
extern Const struct predicate predicate_splay__tree_xempty_3;
extern Const struct predicate predicate_splay__tree_xempty_4;
extern Const struct predicate predicate_splay__tree_xput_4;
extern Const struct predicate predicate_splay__tree_xget__all_3;
extern Const struct predicate predicate_splay__tree_xcreate_1;
extern Const struct predicate predicate_splay__tree_xcreate_2;
extern Const struct predicate predicate_splay__tree_xupdate_5;
extern Const struct predicate predicate_splay__tree_xget__max_4;
extern Const struct predicate predicate_splay__tree_xget__min_4;

module module_sorted__bag(glbl, qp, allocp, toppred)
  struct global_variables *glbl;
  struct goalrec *qp;
  register q *allocp;
  Const struct predicate *toppred;
{
  declare_method_table_of(vector);
  static Const vector_structure_type vector_const_0 =
      declare_vector_constant(0,0);
  q a0, a1, a2, a3;

  q *reasonp;
 module_top:
  a0 = qp->args[0];
  switch_on_pred() {
    case_pred(0, create_1_top);
    case_pred(1, create_2_top);
    case_pred(2, main_3_top);
    case_pred(3, message_3_top);
    case_pred(4, message_2F3_243_4_top);
    case_pred(5, message_2F3_242_4_top);
    case_pred(6, message_2F3_242_2F4_240_4_top);
    case_pred(7, message_2F3_241_4_top);
    case_pred(8, message_2F3_241_2F4_240_4_top);
    case_pred(9, message_2F3_240_4_top);
    case_pred(10, message_2F3_240_2F4_240_4_top);
    case_pred(11, flatten_2_top);
    case_pred(12, flatten_4_top);
    case_pred(13, append_3_top);
    last_case_pred(14, last__elem_3_top);
  }

 create_1_top: {
  q x0;
  qp = qp->next;
 create_1_clear_reason:
  reasonp = reasons;
 create_1_0:
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_sorted__bag_xmain_3);
  allocp[2] = a0;
  allocp[3] = NILATOM;
  allocp[4] = x0 = makeref(&allocp[4]);
  allocp[5] = (q)(struct goalrec*)&allocp[0];
  allocp[6] = (q)(&predicate_splay__tree_xcreate_1);
  allocp[7] = x0;
  qp = (struct goalrec*)&allocp[5];
  allocp += 8;
  proceed();
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
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_sorted__bag_xmain_3);
  allocp[2] = a1;
  allocp[3] = NILATOM;
  allocp[4] = x0 = makeref(&allocp[4]);
  allocp[5] = (q)(struct goalrec*)&allocp[0];
  allocp[6] = (q)(&predicate_splay__tree_xcreate_2);
  allocp[7] = a0;
  allocp[8] = x0;
  qp = (struct goalrec*)&allocp[5];
  allocp += 9;
  proceed();
 create_2_ext_interrupt:
  reasonp = 0l;
 create_2_interrupt:
  goto interrupt_2;
 }

 main_3_top: {
  q x0, x1, x2;
  a1 = qp->args[1];
  a2 = qp->args[2];
  qp = qp->next;
 main_3_clear_reason:
  reasonp = reasons;
 main_3_0:
 main_3_1:
  switch (ptagof(a0)) {
 case CONS:
  x0 = car_of(a0);
 main_3_2:
  switch (ptagof(x0)) {
 case FUNCTOR:
  if (functor_of(x0) != makesym(functor_do_1)) goto main_3_4;
  allocp[0] = a1;
  x2 = cdr_of(a0);
  allocp[1] = x2;
  x1 = makecons(&allocp[0]);
  a0 = arg(x0, 0);
  a1 = x1;
  allocp += 2;
  execute(main_3_0);
  goto main_3_ext_interrupt;
 case VARREF:
  deref_and_jump(x0,main_3_2);
  *reasonp++ =  x0;
  goto main_3_4;
  };
  goto main_3_4;
 case ATOMIC:
  if (a0 != NILATOM) goto main_3_4;
 main_3_3:
  switch (ptagof(a1)) {
 case CONS:
  a0 = car_of(a1);
  a1 = cdr_of(a1);
  execute(main_3_0);
  goto main_3_ext_interrupt;
 case ATOMIC:
  if (a1 != NILATOM) goto main_3_4;
  proceed();
 case VARREF:
  deref_and_jump(a1,main_3_3);
  *reasonp++ =  a1;
  goto main_3_4;
  };
  goto main_3_4;
 case VARREF:
  deref_and_jump(a0,main_3_1);
  *reasonp++ =  a0;
  goto main_3_4;
  };
  goto main_3_4;
 main_3_4:
  otherwise(main_3_interrupt);
 main_3_5:
  switch (ptagof(a0)) {
 case CONS:
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_sorted__bag_xmain_3);
  x0 = cdr_of(a0);
  allocp[2] = x0;
  allocp[3] = a1;
  allocp[4] = x1 = makeref(&allocp[4]);
  a0 = car_of(a0);
  a1 = a2;
  a2 = x1;
  qp = (struct goalrec*)&allocp[0];
  allocp += 5;
  execute(message_3_0);
  goto message_3_ext_interrupt;
 case VARREF:
  deref_and_jump(a0,main_3_5);
  *reasonp++ =  a0;
  goto main_3_interrupt;
  };
  goto main_3_interrupt;
 main_3_ext_interrupt:
  reasonp = 0l;
 main_3_interrupt:
  goto interrupt_3;
 }

 message_3_top: {
  q x0, x1, x2;
  a1 = qp->args[1];
  a2 = qp->args[2];
  qp = qp->next;
 message_3_clear_reason:
  reasonp = reasons;
 message_3_0:
 message_3_1:
  switch (ptagof(a0)) {
 case FUNCTOR:
  switch (symval(functor_of(a0))) {
 case functor_empty_1:
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_splay__tree_xempty_3);
  x0 = arg(a0, 0);
  allocp[2] = x0;
  allocp[3] = a1;
  allocp[4] = a2;
  qp = (struct goalrec*)&allocp[0];
  allocp += 5;
  proceed();
 case functor_put_1:
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_sorted__bag_xmessage_2F3_243_4);
  allocp[2] = x0 = makeref(&allocp[2]);
  x1 = arg(a0, 0);
  allocp[3] = x1;
  allocp[4] = x2 = makeref(&allocp[4]);
  allocp[5] = a2;
  allocp[6] = (q)(struct goalrec*)&allocp[0];
  allocp[7] = (q)(&predicate_splay__tree_xempty_4);
  allocp[8] = x1;
  allocp[9] = x0;
  allocp[10] = a1;
  allocp[11] = x2;
  qp = (struct goalrec*)&allocp[6];
  allocp += 12;
  proceed();
 case functor_get_1:
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_sorted__bag_xmessage_2F3_242_4);
  allocp[2] = x0 = makeref(&allocp[2]);
  allocp[3] = x1 = makeref(&allocp[3]);
  x2 = arg(a0, 0);
  allocp[4] = x2;
  allocp[5] = a2;
  allocp[6] = (q)(struct goalrec*)&allocp[0];
  allocp[7] = (q)(&predicate_splay__tree_xempty_3);
  allocp[8] = x0;
  allocp[9] = a1;
  allocp[10] = x1;
  qp = (struct goalrec*)&allocp[6];
  allocp += 11;
  proceed();
 case functor_get__if__any_1:
  allocp[0] = makesym(functor_get__min__if__any_1);
  x1 = arg(a0, 0);
  allocp[1] = x1;
  x0 = makefunctor(&allocp[0]);
  a0 = x0;
  allocp += 2;
  execute(message_3_0);
  goto message_3_ext_interrupt;
 case functor_get__all_1:
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_sorted__bag_xflatten_2);
  allocp[2] = x0 = makeref(&allocp[2]);
  x1 = arg(a0, 0);
  allocp[3] = x1;
  allocp[4] = (q)(struct goalrec*)&allocp[0];
  allocp[5] = (q)(&predicate_splay__tree_xget__all_3);
  allocp[6] = x0;
  allocp[7] = a1;
  allocp[8] = a2;
  qp = (struct goalrec*)&allocp[4];
  allocp += 9;
  proceed();
 case functor_carbon__copy_1:
  unify(a2, a1);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_sorted__bag_xflatten_2);
  allocp[2] = x0 = makeref(&allocp[2]);
  x1 = arg(a0, 0);
  allocp[3] = x1;
  allocp[4] = (q)(struct goalrec*)&allocp[0];
  allocp[5] = (q)(&predicate_splay__tree_xget__all_3);
  allocp[6] = x0;
  allocp[7] = a1;
  allocp[8] = x2 = makeref(&allocp[8]);
  qp = (struct goalrec*)&allocp[4];
  allocp += 9;
  proceed();
 case functor_get__max__if__any_1:
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_sorted__bag_xmessage_2F3_241_4);
  allocp[2] = x0 = makeref(&allocp[2]);
  x1 = arg(a0, 0);
  allocp[3] = x1;
  allocp[4] = a2;
  allocp[5] = x2 = makeref(&allocp[5]);
  allocp[6] = (q)(struct goalrec*)&allocp[0];
  allocp[7] = (q)(&predicate_splay__tree_xempty_3);
  allocp[8] = x0;
  allocp[9] = a1;
  allocp[10] = x2;
  qp = (struct goalrec*)&allocp[6];
  allocp += 11;
  proceed();
 case functor_get__min__if__any_1:
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_sorted__bag_xmessage_2F3_240_4);
  allocp[2] = x0 = makeref(&allocp[2]);
  x1 = arg(a0, 0);
  allocp[3] = x1;
  allocp[4] = a2;
  allocp[5] = x2 = makeref(&allocp[5]);
  allocp[6] = (q)(struct goalrec*)&allocp[0];
  allocp[7] = (q)(&predicate_splay__tree_xempty_3);
  allocp[8] = x0;
  allocp[9] = a1;
  allocp[10] = x2;
  qp = (struct goalrec*)&allocp[6];
  allocp += 11;
  proceed();
  };
  goto message_3_interrupt;
 case VARREF:
  deref_and_jump(a0,message_3_1);
  *reasonp++ =  a0;
  goto message_3_interrupt;
  };
  goto message_3_interrupt;
 message_3_ext_interrupt:
  reasonp = 0l;
 message_3_interrupt:
  toppred = &predicate_sorted__bag_xmessage_3;
  goto interrupt_3;
 }

 message_2F3_243_4_top: {
  q x0, x1, x2;
  a1 = qp->args[1];
  a2 = qp->args[2];
  a3 = qp->args[3];
  qp = qp->next;
 message_2F3_243_4_clear_reason:
  reasonp = reasons;
 message_2F3_243_4_0:
 message_2F3_243_4_1:
  switch (ptagof(a0)) {
 case ATOMIC:
  if (isint(a0)) goto message_2F3_243_4_interrupt;
  switch (symval(a0)) {
 case symval(makesym(atom_yes)):
  allocp[0] = NILATOM;
  allocp[1] = a1;
  x0 = makecons(&allocp[0]);
  allocp[2] = (q)qp;
  allocp[3] = (q)(&predicate_splay__tree_xput_4);
  allocp[4] = a1;
  allocp[5] = x0;
  allocp[6] = a2;
  allocp[7] = a3;
  qp = (struct goalrec*)&allocp[2];
  allocp += 8;
  proceed();
 case symval(makesym(atom_no)):
  allocp[0] = NILATOM;
  allocp[1] = a1;
  x0 = makecons(&allocp[0]);
  allocp[2] = (q)qp;
  allocp[3] = (q)(&predicate_sorted__bag_xappend_3);
  allocp[4] = x1 = makeref(&allocp[4]);
  allocp[5] = x0;
  allocp[6] = x2 = makeref(&allocp[6]);
  allocp[7] = (q)(struct goalrec*)&allocp[2];
  allocp[8] = (q)(&predicate_splay__tree_xupdate_5);
  allocp[9] = a1;
  allocp[10] = x1;
  allocp[11] = x2;
  allocp[12] = a2;
  allocp[13] = a3;
  qp = (struct goalrec*)&allocp[7];
  allocp += 14;
  proceed();
  };
  goto message_2F3_243_4_interrupt;
 case VARREF:
  deref_and_jump(a0,message_2F3_243_4_1);
  *reasonp++ =  a0;
  goto message_2F3_243_4_interrupt;
  };
  goto message_2F3_243_4_interrupt;
 message_2F3_243_4_ext_interrupt:
  reasonp = 0l;
 message_2F3_243_4_interrupt:
  goto interrupt_4;
 }

 message_2F3_242_4_top: {
  q x0, x1, x2, x3;
  a1 = qp->args[1];
  a2 = qp->args[2];
  a3 = qp->args[3];
  qp = qp->next;
 message_2F3_242_4_clear_reason:
  reasonp = reasons;
 message_2F3_242_4_0:
 message_2F3_242_4_1:
  switch (ptagof(a0)) {
 case ATOMIC:
  if (a0 != makesym(atom_no)) goto message_2F3_242_4_interrupt;
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_sorted__bag_xmessage_2F3_242_2F4_240_4);
  allocp[2] = x0 = makeref(&allocp[2]);
  allocp[3] = a3;
  allocp[4] = x1 = makeref(&allocp[4]);
  allocp[5] = x2 = makeref(&allocp[5]);
  allocp[6] = (q)(struct goalrec*)&allocp[0];
  allocp[7] = (q)(&predicate_sorted__bag_xlast__elem_3);
  allocp[8] = x3 = makeref(&allocp[8]);
  allocp[9] = x0;
  allocp[10] = a2;
  allocp[11] = (q)(struct goalrec*)&allocp[6];
  allocp[12] = (q)(&predicate_splay__tree_xget__max_4);
  allocp[13] = x2;
  allocp[14] = x3;
  allocp[15] = a1;
  allocp[16] = x1;
  qp = (struct goalrec*)&allocp[11];
  allocp += 17;
  proceed();
 case VARREF:
  deref_and_jump(a0,message_2F3_242_4_1);
  *reasonp++ =  a0;
  goto message_2F3_242_4_interrupt;
  };
  goto message_2F3_242_4_interrupt;
 message_2F3_242_4_ext_interrupt:
  reasonp = 0l;
 message_2F3_242_4_interrupt:
  goto interrupt_4;
 }

 message_2F3_242_2F4_240_4_top: {
  a1 = qp->args[1];
  a2 = qp->args[2];
  a3 = qp->args[3];
  qp = qp->next;
 message_2F3_242_2F4_240_4_clear_reason:
  reasonp = reasons;
 message_2F3_242_2F4_240_4_0:
 message_2F3_242_2F4_240_4_1:
  switch (ptagof(a0)) {
 case ATOMIC:
  if (a0 != NILATOM) goto message_2F3_242_2F4_240_4_2;
  unify(a1, a2);
  proceed();
 case VARREF:
  deref_and_jump(a0,message_2F3_242_2F4_240_4_1);
  *reasonp++ =  a0;
  goto message_2F3_242_2F4_240_4_2;
  };
  goto message_2F3_242_2F4_240_4_2;
 message_2F3_242_2F4_240_4_2:
  otherwise(message_2F3_242_2F4_240_4_interrupt);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_splay__tree_xput_4);
  allocp[2] = a3;
  allocp[3] = a0;
  allocp[4] = a2;
  allocp[5] = a1;
  qp = (struct goalrec*)&allocp[0];
  allocp += 6;
  proceed();
 message_2F3_242_2F4_240_4_ext_interrupt:
  reasonp = 0l;
 message_2F3_242_2F4_240_4_interrupt:
  goto interrupt_4;
 }

 message_2F3_241_4_top: {
  q x0, x1, x2, x3, x4, x5;
  a1 = qp->args[1];
  a2 = qp->args[2];
  a3 = qp->args[3];
  qp = qp->next;
 message_2F3_241_4_clear_reason:
  reasonp = reasons;
 message_2F3_241_4_0:
 message_2F3_241_4_1:
  switch (ptagof(a0)) {
 case ATOMIC:
  if (isint(a0)) goto message_2F3_241_4_interrupt;
  switch (symval(a0)) {
 case symval(makesym(atom_yes)):
  x0 = makefunctor(&vector_const_0);
  unify_value(a1, x0);
  unify(a2, a3);
  proceed();
 case symval(makesym(atom_no)):
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_sorted__bag_xmessage_2F3_241_2F4_240_4);
  allocp[2] = x0 = makeref(&allocp[2]);
  allocp[3] = a2;
  allocp[4] = x1 = makeref(&allocp[4]);
  allocp[5] = x2 = makeref(&allocp[5]);
  allocp[6] = x3 = makeref(&allocp[6]);
  allocp[7] = x3;
  x4 = create_vector(&allocp[7], 1, allocp);
  qp = (struct goalrec*)&allocp[0];
  allocp += 8;
  unify_value(a1, x4);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_sorted__bag_xlast__elem_3);
  allocp[2] = x5 = makeref(&allocp[2]);
  allocp[3] = x0;
  allocp[4] = x3;
  allocp[5] = (q)(struct goalrec*)&allocp[0];
  allocp[6] = (q)(&predicate_splay__tree_xget__max_4);
  allocp[7] = x2;
  allocp[8] = x5;
  allocp[9] = a3;
  allocp[10] = x1;
  qp = (struct goalrec*)&allocp[5];
  allocp += 11;
  proceed();
  };
  goto message_2F3_241_4_interrupt;
 case VARREF:
  deref_and_jump(a0,message_2F3_241_4_1);
  *reasonp++ =  a0;
  goto message_2F3_241_4_interrupt;
  };
  goto message_2F3_241_4_interrupt;
 message_2F3_241_4_ext_interrupt:
  reasonp = 0l;
 message_2F3_241_4_interrupt:
  goto interrupt_4;
 }

 message_2F3_241_2F4_240_4_top: {
  a1 = qp->args[1];
  a2 = qp->args[2];
  a3 = qp->args[3];
  qp = qp->next;
 message_2F3_241_2F4_240_4_clear_reason:
  reasonp = reasons;
 message_2F3_241_2F4_240_4_0:
 message_2F3_241_2F4_240_4_1:
  switch (ptagof(a0)) {
 case ATOMIC:
  if (a0 != NILATOM) goto message_2F3_241_2F4_240_4_2;
  unify(a1, a2);
  proceed();
 case VARREF:
  deref_and_jump(a0,message_2F3_241_2F4_240_4_1);
  *reasonp++ =  a0;
  goto message_2F3_241_2F4_240_4_2;
  };
  goto message_2F3_241_2F4_240_4_2;
 message_2F3_241_2F4_240_4_2:
  otherwise(message_2F3_241_2F4_240_4_interrupt);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_splay__tree_xput_4);
  allocp[2] = a3;
  allocp[3] = a0;
  allocp[4] = a2;
  allocp[5] = a1;
  qp = (struct goalrec*)&allocp[0];
  allocp += 6;
  proceed();
 message_2F3_241_2F4_240_4_ext_interrupt:
  reasonp = 0l;
 message_2F3_241_2F4_240_4_interrupt:
  goto interrupt_4;
 }

 message_2F3_240_4_top: {
  q x0, x1, x2, x3, x4, x5;
  a1 = qp->args[1];
  a2 = qp->args[2];
  a3 = qp->args[3];
  qp = qp->next;
 message_2F3_240_4_clear_reason:
  reasonp = reasons;
 message_2F3_240_4_0:
 message_2F3_240_4_1:
  switch (ptagof(a0)) {
 case ATOMIC:
  if (isint(a0)) goto message_2F3_240_4_interrupt;
  switch (symval(a0)) {
 case symval(makesym(atom_yes)):
  x0 = makefunctor(&vector_const_0);
  unify_value(a1, x0);
  unify(a2, a3);
  proceed();
 case symval(makesym(atom_no)):
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_sorted__bag_xmessage_2F3_240_2F4_240_4);
  allocp[2] = x0 = makeref(&allocp[2]);
  allocp[3] = a2;
  allocp[4] = x1 = makeref(&allocp[4]);
  allocp[5] = x2 = makeref(&allocp[5]);
  allocp[6] = x3 = makeref(&allocp[6]);
  allocp[7] = x3;
  x4 = create_vector(&allocp[7], 1, allocp);
  qp = (struct goalrec*)&allocp[0];
  allocp += 8;
  unify_value(a1, x4);
  allocp[0] = x0;
  allocp[1] = x3;
  x5 = makecons(&allocp[0]);
  allocp[2] = (q)qp;
  allocp[3] = (q)(&predicate_splay__tree_xget__min_4);
  allocp[4] = x2;
  allocp[5] = x5;
  allocp[6] = a3;
  allocp[7] = x1;
  qp = (struct goalrec*)&allocp[2];
  allocp += 8;
  proceed();
  };
  goto message_2F3_240_4_interrupt;
 case VARREF:
  deref_and_jump(a0,message_2F3_240_4_1);
  *reasonp++ =  a0;
  goto message_2F3_240_4_interrupt;
  };
  goto message_2F3_240_4_interrupt;
 message_2F3_240_4_ext_interrupt:
  reasonp = 0l;
 message_2F3_240_4_interrupt:
  goto interrupt_4;
 }

 message_2F3_240_2F4_240_4_top: {
  a1 = qp->args[1];
  a2 = qp->args[2];
  a3 = qp->args[3];
  qp = qp->next;
 message_2F3_240_2F4_240_4_clear_reason:
  reasonp = reasons;
 message_2F3_240_2F4_240_4_0:
 message_2F3_240_2F4_240_4_1:
  switch (ptagof(a0)) {
 case ATOMIC:
  if (a0 != NILATOM) goto message_2F3_240_2F4_240_4_2;
  unify(a1, a2);
  proceed();
 case VARREF:
  deref_and_jump(a0,message_2F3_240_2F4_240_4_1);
  *reasonp++ =  a0;
  goto message_2F3_240_2F4_240_4_2;
  };
  goto message_2F3_240_2F4_240_4_2;
 message_2F3_240_2F4_240_4_2:
  otherwise(message_2F3_240_2F4_240_4_interrupt);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_splay__tree_xput_4);
  allocp[2] = a3;
  allocp[3] = a0;
  allocp[4] = a2;
  allocp[5] = a1;
  qp = (struct goalrec*)&allocp[0];
  allocp += 6;
  proceed();
 message_2F3_240_2F4_240_4_ext_interrupt:
  reasonp = 0l;
 message_2F3_240_2F4_240_4_interrupt:
  goto interrupt_4;
 }

 flatten_2_top: {
  q x0, x1, x2, x3;
  a1 = qp->args[1];
  qp = qp->next;
 flatten_2_clear_reason:
  reasonp = reasons;
 flatten_2_0:
 flatten_2_1:
  switch (ptagof(a0)) {
 case CONS:
  x0 = car_of(a0);
 flatten_2_2:
  switch (ptagof(x0)) {
 case FUNCTOR:
  if (functor_of(x0) != makesym(functor__3D_2)) goto flatten_2_interrupt;
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_sorted__bag_xflatten_2);
  x1 = cdr_of(a0);
  allocp[2] = x1;
  allocp[3] = x2 = makeref(&allocp[3]);
  a0 = arg(x0, 1);
  x3 = arg(x0, 0);
  a2 = a1;
  a3 = x2;
  a1 = x3;
  qp = (struct goalrec*)&allocp[0];
  allocp += 4;
  execute(flatten_4_0);
  goto flatten_4_ext_interrupt;
 case VARREF:
  deref_and_jump(x0,flatten_2_2);
  *reasonp++ =  x0;
  goto flatten_2_interrupt;
  };
  goto flatten_2_interrupt;
 case ATOMIC:
  if (a0 != NILATOM) goto flatten_2_interrupt;
  x0 = NILATOM;
  unify_value(a1, x0);
  proceed();
 case VARREF:
  deref_and_jump(a0,flatten_2_1);
  *reasonp++ =  a0;
  goto flatten_2_interrupt;
  };
  goto flatten_2_interrupt;
 flatten_2_ext_interrupt:
  reasonp = 0l;
 flatten_2_interrupt:
  goto interrupt_2;
 }

 flatten_4_top: {
  q x0, x1, x2;
  a1 = qp->args[1];
  a2 = qp->args[2];
  a3 = qp->args[3];
  qp = qp->next;
 flatten_4_clear_reason:
  reasonp = reasons;
 flatten_4_0:
 flatten_4_1:
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
  execute(flatten_4_0);
  goto flatten_4_ext_interrupt;
 case ATOMIC:
  if (a0 != NILATOM) goto flatten_4_interrupt;
  unify(a2, a3);
  proceed();
 case VARREF:
  deref_and_jump(a0,flatten_4_1);
  *reasonp++ =  a0;
  goto flatten_4_interrupt;
  };
  goto flatten_4_interrupt;
 flatten_4_ext_interrupt:
  reasonp = 0l;
 flatten_4_interrupt:
  toppred = &predicate_sorted__bag_xflatten_4;
  goto interrupt_4;
 }

 append_3_top: {
  q x0, x1, x2;
  a1 = qp->args[1];
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

 last__elem_3_top: {
  q x0, x1, x2;
  a1 = qp->args[1];
  a2 = qp->args[2];
  qp = qp->next;
 last__elem_3_clear_reason:
  reasonp = reasons;
 last__elem_3_0:
 last__elem_3_1:
  switch (ptagof(a0)) {
 case CONS:
  x0 = cdr_of(a0);
 last__elem_3_2:
  switch (ptagof(x0)) {
 case ATOMIC:
  if (x0 != NILATOM) goto last__elem_3_3;
  x1 = NILATOM;
  unify_value(a1, x1);
  x2 = car_of(a0);
  unify(a2, x2);
  proceed();
 case VARREF:
  deref_and_jump(x0,last__elem_3_2);
  *reasonp++ =  x0;
  goto last__elem_3_3;
  };
  goto last__elem_3_3;
 case VARREF:
  deref_and_jump(a0,last__elem_3_1);
  *reasonp++ =  a0;
  goto last__elem_3_3;
  };
  goto last__elem_3_3;
 last__elem_3_3:
  otherwise(last__elem_3_interrupt);
 last__elem_3_4:
  switch (ptagof(a0)) {
 case CONS:
  allocp[0] = x1 = makeref(&allocp[0]);
  x2 = car_of(a0);
  allocp[1] = x2;
  x0 = makecons(&allocp[0]);
  allocp += 2;
  unify_value(a1, x0);
  a0 = cdr_of(a0);
  a1 = x1;
  execute(last__elem_3_0);
  goto last__elem_3_ext_interrupt;
 case VARREF:
  deref_and_jump(a0,last__elem_3_4);
  *reasonp++ =  a0;
  goto last__elem_3_interrupt;
  };
  goto last__elem_3_interrupt;
 last__elem_3_ext_interrupt:
  reasonp = 0l;
 last__elem_3_interrupt:
  goto interrupt_3;
 }
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
  loop_within_module(module_sorted__bag);
}
