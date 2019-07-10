/* Compiled by KLIC compiler version 3.003 (Mon Mar 29 19:42:59 JST 1999) */
#include <klic/klichdr.h>
#include "atom.h"
#include "funct.h"

module module_keyed__sorted__set();
Const struct predicate predicate_keyed__sorted__set_xcreate_1 =
   { module_keyed__sorted__set, 0, 1 };
Const struct predicate predicate_keyed__sorted__set_xcreate_2 =
   { module_keyed__sorted__set, 1, 2 };
Const struct predicate predicate_keyed__sorted__set_xmain_3 =
   { module_keyed__sorted__set, 2, 3 };
Const struct predicate predicate_keyed__sorted__set_xmessage_3 =
   { module_keyed__sorted__set, 3, 3 };
Const struct predicate predicate_keyed__sorted__set_xmessage_2F3_246_5 =
   { module_keyed__sorted__set, 4, 5 };
Const struct predicate predicate_keyed__sorted__set_xmessage_2F3_245_5 =
   { module_keyed__sorted__set, 5, 5 };
Const struct predicate predicate_keyed__sorted__set_xmessage_2F3_244_5 =
   { module_keyed__sorted__set, 6, 5 };
Const struct predicate predicate_keyed__sorted__set_xmessage_2F3_243_6 =
   { module_keyed__sorted__set, 7, 6 };
Const struct predicate predicate_keyed__sorted__set_xmessage_2F3_242_7 =
   { module_keyed__sorted__set, 8, 7 };
Const struct predicate predicate_keyed__sorted__set_xmessage_2F3_241_4 =
   { module_keyed__sorted__set, 9, 4 };
Const struct predicate predicate_keyed__sorted__set_xmessage_2F3_240_4 =
   { module_keyed__sorted__set, 10, 4 };
Const struct predicate predicate_keyed__sorted__set_xconvert_2 =
   { module_keyed__sorted__set, 11, 2 };
extern Const struct predicate predicate_splay__tree_xempty_3;
extern Const struct predicate predicate_splay__tree_xempty_4;
extern Const struct predicate predicate_splay__tree_xput_4;
extern Const struct predicate predicate_splay__tree_xget_4;
extern Const struct predicate predicate_splay__tree_xget__all_3;
extern Const struct predicate predicate_splay__tree_xcreate_1;
extern Const struct predicate predicate_splay__tree_xcreate_2;
extern Const struct predicate predicate_splay__tree_xupdate_5;
extern Const struct predicate predicate_splay__tree_xget__max_4;
extern Const struct predicate predicate_splay__tree_xget__min_4;

module module_keyed__sorted__set(glbl, qp, allocp, toppred)
  struct global_variables *glbl;
  struct goalrec *qp;
  register q *allocp;
  Const struct predicate *toppred;
{
  declare_method_table_of(vector);
  static Const vector_structure_type vector_const_0 =
      declare_vector_constant(0,0);
  q a0, a1, a2, a3, a4, a5, a6;

  q *reasonp;
 module_top:
  a0 = qp->args[0];
  switch_on_pred() {
    case_pred(0, create_1_top);
    case_pred(1, create_2_top);
    case_pred(2, main_3_top);
    case_pred(3, message_3_top);
    case_pred(4, message_2F3_246_5_top);
    case_pred(5, message_2F3_245_5_top);
    case_pred(6, message_2F3_244_5_top);
    case_pred(7, message_2F3_243_6_top);
    case_pred(8, message_2F3_242_7_top);
    case_pred(9, message_2F3_241_4_top);
    case_pred(10, message_2F3_240_4_top);
    last_case_pred(11, convert_2_top);
  }

 create_1_top: {
  q x0;
  qp = qp->next;
 create_1_clear_reason:
  reasonp = reasons;
 create_1_0:
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_keyed__sorted__set_xmain_3);
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
  allocp[1] = (q)(&predicate_keyed__sorted__set_xmain_3);
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
  allocp[1] = (q)(&predicate_keyed__sorted__set_xmain_3);
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
  q x0, x1, x2, x3, x4, x5;
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
 case functor_empty_2:
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_splay__tree_xempty_4);
  x0 = arg(a0, 0);
  allocp[2] = x0;
  x1 = arg(a0, 1);
  allocp[3] = x1;
  allocp[4] = a1;
  allocp[5] = a2;
  qp = (struct goalrec*)&allocp[0];
  allocp += 6;
  proceed();
 case functor_put_3:
  allocp[0] = makesym(functor_get__if__any__and__put_3);
  x1 = arg(a0, 0);
  allocp[1] = x1;
  x2 = arg(a0, 2);
  allocp[2] = x2;
  x3 = arg(a0, 1);
  allocp[3] = x3;
  x0 = makefunctor(&allocp[0]);
  a0 = x0;
  allocp += 4;
  execute(message_3_0);
  goto message_3_ext_interrupt;
 case functor_get_2:
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_splay__tree_xget_4);
  x0 = arg(a0, 0);
  allocp[2] = x0;
  x1 = arg(a0, 1);
  allocp[3] = x1;
  allocp[4] = x2 = makeref(&allocp[4]);
  allocp[5] = a2;
  allocp[6] = (q)(struct goalrec*)&allocp[0];
  allocp[7] = (q)(&predicate_splay__tree_xempty_4);
  allocp[8] = x0;
  allocp[9] = makesym(atom_no);
  allocp[10] = a1;
  allocp[11] = x2;
  qp = (struct goalrec*)&allocp[6];
  allocp += 12;
  proceed();
 case functor_get__if__any_2:
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_keyed__sorted__set_xmessage_2F3_246_5);
  allocp[2] = x0 = makeref(&allocp[2]);
  x1 = arg(a0, 1);
  allocp[3] = x1;
  allocp[4] = a2;
  allocp[5] = x2 = makeref(&allocp[5]);
  x3 = arg(a0, 0);
  allocp[6] = x3;
  allocp[7] = (q)(struct goalrec*)&allocp[0];
  allocp[8] = (q)(&predicate_splay__tree_xempty_4);
  allocp[9] = x3;
  allocp[10] = x0;
  allocp[11] = a1;
  allocp[12] = x2;
  qp = (struct goalrec*)&allocp[7];
  allocp += 13;
  proceed();
 case functor_get__all_1:
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_keyed__sorted__set_xconvert_2);
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
 case functor_get__all_2:
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_keyed__sorted__set_xmessage_2F3_245_5);
  allocp[2] = x0 = makeref(&allocp[2]);
  x1 = arg(a0, 1);
  allocp[3] = x1;
  allocp[4] = a2;
  allocp[5] = x2 = makeref(&allocp[5]);
  x3 = arg(a0, 0);
  allocp[6] = x3;
  allocp[7] = (q)(struct goalrec*)&allocp[0];
  allocp[8] = (q)(&predicate_splay__tree_xempty_4);
  allocp[9] = x3;
  allocp[10] = x0;
  allocp[11] = a1;
  allocp[12] = x2;
  qp = (struct goalrec*)&allocp[7];
  allocp += 13;
  proceed();
 case functor_carbon__copy_1:
  unify(a2, a1);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_keyed__sorted__set_xconvert_2);
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
 case functor_carbon__copy_2:
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_keyed__sorted__set_xmessage_2F3_244_5);
  allocp[2] = x0 = makeref(&allocp[2]);
  x1 = arg(a0, 1);
  allocp[3] = x1;
  allocp[4] = a2;
  allocp[5] = x2 = makeref(&allocp[5]);
  x3 = arg(a0, 0);
  allocp[6] = x3;
  allocp[7] = (q)(struct goalrec*)&allocp[0];
  allocp[8] = (q)(&predicate_splay__tree_xempty_4);
  allocp[9] = x3;
  allocp[10] = x0;
  allocp[11] = a1;
  allocp[12] = x2;
  qp = (struct goalrec*)&allocp[7];
  allocp += 13;
  proceed();
 case functor_get__and__put_3:
  x1 = arg(a0, 1);
  allocp[0] = x1;
  x0 = create_vector(&allocp[0], 1, allocp);
  allocp[1] = makesym(functor_get__and__put__if__any_4);
  x3 = arg(a0, 0);
  allocp[2] = x3;
  allocp[3] = x0;
  x4 = arg(a0, 2);
  allocp[4] = x4;
  allocp[5] = x5 = makeref(&allocp[5]);
  x2 = makefunctor(&allocp[1]);
  a0 = x2;
  allocp += 6;
  execute(message_3_0);
  goto message_3_ext_interrupt;
 case functor_get__if__any__and__put_3:
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_keyed__sorted__set_xmessage_2F3_243_6);
  allocp[2] = x0 = makeref(&allocp[2]);
  x1 = arg(a0, 0);
  allocp[3] = x1;
  x2 = arg(a0, 2);
  allocp[4] = x2;
  allocp[5] = x3 = makeref(&allocp[5]);
  x4 = arg(a0, 1);
  allocp[6] = x4;
  allocp[7] = a2;
  allocp[8] = (q)(struct goalrec*)&allocp[0];
  allocp[9] = (q)(&predicate_splay__tree_xempty_4);
  allocp[10] = x1;
  allocp[11] = x0;
  allocp[12] = a1;
  allocp[13] = x3;
  qp = (struct goalrec*)&allocp[8];
  allocp += 14;
  proceed();
 case functor_get__and__put__if__any_4:
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_keyed__sorted__set_xmessage_2F3_242_7);
  allocp[2] = x0 = makeref(&allocp[2]);
  x1 = arg(a0, 1);
  allocp[3] = x1;
  x2 = arg(a0, 3);
  allocp[4] = x2;
  x3 = arg(a0, 2);
  allocp[5] = x3;
  allocp[6] = a2;
  allocp[7] = x4 = makeref(&allocp[7]);
  x5 = arg(a0, 0);
  allocp[8] = x5;
  allocp[9] = (q)(struct goalrec*)&allocp[0];
  allocp[10] = (q)(&predicate_splay__tree_xempty_4);
  allocp[11] = x5;
  allocp[12] = x0;
  allocp[13] = a1;
  allocp[14] = x4;
  qp = (struct goalrec*)&allocp[9];
  allocp += 15;
  proceed();
 case functor_get__max__if__any_1:
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_keyed__sorted__set_xmessage_2F3_241_4);
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
  allocp[1] = (q)(&predicate_keyed__sorted__set_xmessage_2F3_240_4);
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
  toppred = &predicate_keyed__sorted__set_xmessage_3;
  goto interrupt_3;
 }

 message_2F3_246_5_top: {
  q x0, x1;
  a1 = qp->args[1];
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  qp = qp->next;
 message_2F3_246_5_clear_reason:
  reasonp = reasons;
 message_2F3_246_5_0:
 message_2F3_246_5_1:
  switch (ptagof(a0)) {
 case ATOMIC:
  if (isint(a0)) goto message_2F3_246_5_interrupt;
  switch (symval(a0)) {
 case symval(makesym(atom_yes)):
  x0 = makefunctor(&vector_const_0);
  unify_value(a1, x0);
  unify(a2, a3);
  proceed();
 case symval(makesym(atom_no)):
  allocp[0] = x0 = makeref(&allocp[0]);
  allocp[1] = x0;
  x1 = create_vector(&allocp[1], 1, allocp);
  allocp += 2;
  unify_value(a1, x1);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_splay__tree_xget_4);
  allocp[2] = a4;
  allocp[3] = x0;
  allocp[4] = a3;
  allocp[5] = a2;
  qp = (struct goalrec*)&allocp[0];
  allocp += 6;
  proceed();
  };
  goto message_2F3_246_5_interrupt;
 case VARREF:
  deref_and_jump(a0,message_2F3_246_5_1);
  *reasonp++ =  a0;
  goto message_2F3_246_5_interrupt;
  };
  goto message_2F3_246_5_interrupt;
 message_2F3_246_5_ext_interrupt:
  reasonp = 0l;
 message_2F3_246_5_interrupt:
  goto interrupt_5;
 }

 message_2F3_245_5_top: {
  q x0, x1;
  a1 = qp->args[1];
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  qp = qp->next;
 message_2F3_245_5_clear_reason:
  reasonp = reasons;
 message_2F3_245_5_0:
 message_2F3_245_5_1:
  switch (ptagof(a0)) {
 case ATOMIC:
  if (isint(a0)) goto message_2F3_245_5_interrupt;
  switch (symval(a0)) {
 case symval(makesym(atom_yes)):
  x0 = NILATOM;
  unify_value(a1, x0);
  unify(a2, a3);
  proceed();
 case symval(makesym(atom_no)):
  allocp[0] = NILATOM;
  allocp[1] = x1 = makeref(&allocp[1]);
  x0 = makecons(&allocp[0]);
  allocp += 2;
  unify_value(a1, x0);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_splay__tree_xget_4);
  allocp[2] = a4;
  allocp[3] = x1;
  allocp[4] = a3;
  allocp[5] = a2;
  qp = (struct goalrec*)&allocp[0];
  allocp += 6;
  proceed();
  };
  goto message_2F3_245_5_interrupt;
 case VARREF:
  deref_and_jump(a0,message_2F3_245_5_1);
  *reasonp++ =  a0;
  goto message_2F3_245_5_interrupt;
  };
  goto message_2F3_245_5_interrupt;
 message_2F3_245_5_ext_interrupt:
  reasonp = 0l;
 message_2F3_245_5_interrupt:
  goto interrupt_5;
 }

 message_2F3_244_5_top: {
  q x0, x1;
  a1 = qp->args[1];
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  qp = qp->next;
 message_2F3_244_5_clear_reason:
  reasonp = reasons;
 message_2F3_244_5_0:
 message_2F3_244_5_1:
  switch (ptagof(a0)) {
 case ATOMIC:
  if (isint(a0)) goto message_2F3_244_5_interrupt;
  switch (symval(a0)) {
 case symval(makesym(atom_yes)):
  x0 = NILATOM;
  unify_value(a1, x0);
  unify(a2, a3);
  proceed();
 case symval(makesym(atom_no)):
  allocp[0] = NILATOM;
  allocp[1] = x1 = makeref(&allocp[1]);
  x0 = makecons(&allocp[0]);
  allocp += 2;
  unify_value(a1, x0);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_splay__tree_xupdate_5);
  allocp[2] = a4;
  allocp[3] = x1;
  allocp[4] = x1;
  allocp[5] = a3;
  allocp[6] = a2;
  qp = (struct goalrec*)&allocp[0];
  allocp += 7;
  proceed();
  };
  goto message_2F3_244_5_interrupt;
 case VARREF:
  deref_and_jump(a0,message_2F3_244_5_1);
  *reasonp++ =  a0;
  goto message_2F3_244_5_interrupt;
  };
  goto message_2F3_244_5_interrupt;
 message_2F3_244_5_ext_interrupt:
  reasonp = 0l;
 message_2F3_244_5_interrupt:
  goto interrupt_5;
 }

 message_2F3_243_6_top: {
  q x0, x1;
  a1 = qp->args[1];
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  qp = qp->next;
 message_2F3_243_6_clear_reason:
  reasonp = reasons;
 message_2F3_243_6_0:
 message_2F3_243_6_1:
  switch (ptagof(a0)) {
 case ATOMIC:
  if (isint(a0)) goto message_2F3_243_6_interrupt;
  switch (symval(a0)) {
 case symval(makesym(atom_yes)):
  x0 = makefunctor(&vector_const_0);
  unify_value(a4, x0);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_splay__tree_xput_4);
  allocp[2] = a1;
  allocp[3] = a2;
  allocp[4] = a3;
  allocp[5] = a5;
  qp = (struct goalrec*)&allocp[0];
  allocp += 6;
  proceed();
 case symval(makesym(atom_no)):
  allocp[0] = x0 = makeref(&allocp[0]);
  allocp[1] = x0;
  x1 = create_vector(&allocp[1], 1, allocp);
  allocp += 2;
  unify_value(a4, x1);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_splay__tree_xupdate_5);
  allocp[2] = a1;
  allocp[3] = x0;
  allocp[4] = a2;
  allocp[5] = a3;
  allocp[6] = a5;
  qp = (struct goalrec*)&allocp[0];
  allocp += 7;
  proceed();
  };
  goto message_2F3_243_6_interrupt;
 case VARREF:
  deref_and_jump(a0,message_2F3_243_6_1);
  *reasonp++ =  a0;
  goto message_2F3_243_6_interrupt;
  };
  goto message_2F3_243_6_interrupt;
 message_2F3_243_6_ext_interrupt:
  reasonp = 0l;
 message_2F3_243_6_interrupt:
  goto interrupt_6;
 }

 message_2F3_242_7_top: {
  q x0, x1, x2;
  a1 = qp->args[1];
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  a6 = qp->args[6];
  qp = qp->next;
 message_2F3_242_7_clear_reason:
  reasonp = reasons;
 message_2F3_242_7_0:
 message_2F3_242_7_1:
  switch (ptagof(a0)) {
 case ATOMIC:
  if (isint(a0)) goto message_2F3_242_7_interrupt;
  switch (symval(a0)) {
 case symval(makesym(atom_yes)):
  x0 = makefunctor(&vector_const_0);
  unify_value(a1, x0);
  allocp[0] = a3;
  x1 = create_vector(&allocp[0], 1, allocp);
  allocp += 1;
  unify_value(a2, x1);
  unify(a4, a5);
  proceed();
 case symval(makesym(atom_no)):
  x0 = makefunctor(&vector_const_0);
  unify_value(a2, x0);
  allocp[0] = x1 = makeref(&allocp[0]);
  allocp[1] = x1;
  x2 = create_vector(&allocp[1], 1, allocp);
  allocp += 2;
  unify_value(a1, x2);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_splay__tree_xupdate_5);
  allocp[2] = a6;
  allocp[3] = x1;
  allocp[4] = a3;
  allocp[5] = a5;
  allocp[6] = a4;
  qp = (struct goalrec*)&allocp[0];
  allocp += 7;
  proceed();
  };
  goto message_2F3_242_7_interrupt;
 case VARREF:
  deref_and_jump(a0,message_2F3_242_7_1);
  *reasonp++ =  a0;
  goto message_2F3_242_7_interrupt;
  };
  goto message_2F3_242_7_interrupt;
 message_2F3_242_7_ext_interrupt:
  reasonp = 0l;
 message_2F3_242_7_interrupt:
  goto interrupt_7;
 }

 message_2F3_241_4_top: {
  q x0, x1, x2;
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
  allocp[0] = x0 = makeref(&allocp[0]);
  allocp[1] = x1 = makeref(&allocp[1]);
  allocp[2] = x0;
  allocp[3] = x1;
  x2 = create_vector(&allocp[2], 2, allocp);
  allocp += 4;
  unify_value(a1, x2);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_splay__tree_xget__max_4);
  allocp[2] = x0;
  allocp[3] = x1;
  allocp[4] = a3;
  allocp[5] = a2;
  qp = (struct goalrec*)&allocp[0];
  allocp += 6;
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

 message_2F3_240_4_top: {
  q x0, x1, x2;
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
  allocp[0] = x0 = makeref(&allocp[0]);
  allocp[1] = x1 = makeref(&allocp[1]);
  allocp[2] = x0;
  allocp[3] = x1;
  x2 = create_vector(&allocp[2], 2, allocp);
  allocp += 4;
  unify_value(a1, x2);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_splay__tree_xget__min_4);
  allocp[2] = x0;
  allocp[3] = x1;
  allocp[4] = a3;
  allocp[5] = a2;
  qp = (struct goalrec*)&allocp[0];
  allocp += 6;
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

 convert_2_top: {
  q x0, x1, x2, x3, x4, x5;
  a1 = qp->args[1];
  qp = qp->next;
 convert_2_clear_reason:
  reasonp = reasons;
 convert_2_0:
 convert_2_1:
  switch (ptagof(a0)) {
 case CONS:
  x0 = car_of(a0);
 convert_2_2:
  switch (ptagof(x0)) {
 case FUNCTOR:
  if (functor_of(x0) != makesym(functor__3D_2)) goto convert_2_interrupt;
  x2 = arg(x0, 0);
  allocp[0] = x2;
  x3 = arg(x0, 1);
  allocp[1] = x3;
  x1 = create_vector(&allocp[0], 2, allocp);
  allocp[2] = x5 = makeref(&allocp[2]);
  allocp[3] = x1;
  x4 = makecons(&allocp[2]);
  allocp += 4;
  unify_value(a1, x4);
  a0 = cdr_of(a0);
  a1 = x5;
  execute(convert_2_0);
  goto convert_2_ext_interrupt;
 case VARREF:
  deref_and_jump(x0,convert_2_2);
  *reasonp++ =  x0;
  goto convert_2_interrupt;
  };
  goto convert_2_interrupt;
 case ATOMIC:
  if (a0 != NILATOM) goto convert_2_interrupt;
  x0 = NILATOM;
  unify_value(a1, x0);
  proceed();
 case VARREF:
  deref_and_jump(a0,convert_2_1);
  *reasonp++ =  a0;
  goto convert_2_interrupt;
  };
  goto convert_2_interrupt;
 convert_2_ext_interrupt:
  reasonp = 0l;
 convert_2_interrupt:
  goto interrupt_2;
 }
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
  loop_within_module(module_keyed__sorted__set);
}
