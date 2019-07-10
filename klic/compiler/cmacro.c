/* Compiled by KLIC compiler version 3.003 (Mon Mar 29 19:42:59 JST 1999) */
#include <klic/klichdr.h>
#include "atom.h"
#include "funct.h"

module module_klic__comp__cmacro();
Const struct predicate predicate_klic__comp__cmacro_xexpand_2 =
   { module_klic__comp__cmacro, 0, 2 };
Const struct predicate predicate_klic__comp__cmacro_xexpand_4 =
   { module_klic__comp__cmacro, 1, 4 };
Const struct predicate predicate_klic__comp__cmacro_xinsDefs_3 =
   { module_klic__comp__cmacro, 2, 3 };
Const struct predicate predicate_klic__comp__cmacro_xreplaceClause_4 =
   { module_klic__comp__cmacro, 3, 4 };
Const struct predicate predicate_klic__comp__cmacro_xreplaceGoals_5 =
   { module_klic__comp__cmacro, 4, 5 };
Const struct predicate predicate_klic__comp__cmacro_xreplaceHead_5 =
   { module_klic__comp__cmacro, 5, 5 };
Const struct predicate predicate_klic__comp__cmacro_xreplaceGoals_7 =
   { module_klic__comp__cmacro, 6, 7 };
Const struct predicate predicate_klic__comp__cmacro_xreplace_5 =
   { module_klic__comp__cmacro, 7, 5 };
Const struct predicate predicate_klic__comp__cmacro_xreplace_6 =
   { module_klic__comp__cmacro, 8, 6 };
Const struct predicate predicate_klic__comp__cmacro_xreplace_7 =
   { module_klic__comp__cmacro, 9, 7 };
Const struct predicate predicate_klic__comp__cmacro_xreconstGoals_2 =
   { module_klic__comp__cmacro, 10, 2 };
extern Const struct predicate predicate_integer__arithmetics_xadd_3;
extern Const struct predicate predicate_functor__table_xarg_3;
extern Const struct predicate predicate_keyed__set_xcreate_1;
extern Const struct predicate predicate_klic__comp__message_xreport__error_2;

module module_klic__comp__cmacro(glbl, qp, allocp, toppred)
  struct global_variables *glbl;
  struct goalrec *qp;
  register q *allocp;
  Const struct predicate *toppred;
{
  static Const unsigned char strconst_body_0[36] =
    "Unrecognized macro definition: ~w";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_0 =
      declare_string_constant_8(strconst_body_0,33);
  static Const q funct_const_1[] = {
    makesym(functor_atom_1),
    makesym(atom_true),
  };
  static Const q funct_const_2[] = {
    makesym(functor_atom_1),
    makesym(atom_true),
  };
  static Const q funct_const_3[] = {
    makesym(functor_atom_1),
    makesym(atom_true),
  };
  static Const q funct_const_4[] = {
    makesym(functor__7C_2),
    makefunctor(funct_const_2),
    makefunctor(funct_const_3),
  };
  static Const q funct_const_5[] = {
    makesym(functor_functor_1),
    makefunctor(funct_const_4),
  };
  q a0, a1, a2, a3, a4, a5, a6;

  q *reasonp;
 module_top:
  a0 = qp->args[0];
  a1 = qp->args[1];
  switch_on_pred() {
    case_pred(0, expand_2_top);
    case_pred(1, expand_4_top);
    case_pred(2, insDefs_3_top);
    case_pred(3, replaceClause_4_top);
    case_pred(4, replaceGoals_5_top);
    case_pred(5, replaceHead_5_top);
    case_pred(6, replaceGoals_7_top);
    case_pred(7, replace_5_top);
    case_pred(8, replace_6_top);
    case_pred(9, replace_7_top);
    last_case_pred(10, reconstGoals_2_top);
  }

 expand_2_top: {
  q x0;
  qp = qp->next;
 expand_2_clear_reason:
  reasonp = reasons;
 expand_2_0:
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__cmacro_xexpand_4);
  allocp[2] = a0;
  allocp[3] = a1;
  allocp[4] = x0 = makeref(&allocp[4]);
  allocp[5] = NILATOM;
  allocp[6] = (q)(struct goalrec*)&allocp[0];
  allocp[7] = (q)(&predicate_keyed__set_xcreate_1);
  allocp[8] = x0;
  qp = (struct goalrec*)&allocp[6];
  allocp += 9;
  proceed();
 expand_2_ext_interrupt:
  reasonp = 0l;
 expand_2_interrupt:
  goto interrupt_2;
 }

 expand_4_top: {
  q x0, x1, x2, x3, x4, x5;
  a2 = qp->args[2];
  a3 = qp->args[3];
  qp = qp->next;
 expand_4_clear_reason:
  reasonp = reasons;
 expand_4_0:
 expand_4_1:
  switch (ptagof(a0)) {
 case CONS:
  x0 = car_of(a0);
 expand_4_2:
  switch (ptagof(x0)) {
 case FUNCTOR:
  if (functor_of(x0) != makesym(functor_functor_1)) goto expand_4_6;
  x1 = arg(x0, 0);
 expand_4_3:
  switch (ptagof(x1)) {
 case FUNCTOR:
  if (functor_of(x1) != makesym(functor__3A_2D_1)) goto expand_4_6;
  x2 = arg(x1, 0);
 expand_4_4:
  switch (ptagof(x2)) {
 case FUNCTOR:
  if (functor_of(x2) != makesym(functor_functor_1)) goto expand_4_6;
  x3 = arg(x2, 0);
 expand_4_5:
  switch (ptagof(x3)) {
 case FUNCTOR:
  if (functor_of(x3) != makesym(functor_with_1)) goto expand_4_6;
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__cmacro_xexpand_4);
  x4 = cdr_of(a0);
  allocp[2] = x4;
  allocp[3] = a1;
  allocp[4] = x5 = makeref(&allocp[4]);
  allocp[5] = a3;
  a0 = arg(x3, 0);
  a1 = a2;
  a2 = x5;
  qp = (struct goalrec*)&allocp[0];
  allocp += 6;
  execute(insDefs_3_0);
  goto insDefs_3_ext_interrupt;
 case VARREF:
  deref_and_jump(x3,expand_4_5);
  *reasonp++ =  x3;
  goto expand_4_6;
  };
  goto expand_4_6;
 case VARREF:
  deref_and_jump(x2,expand_4_4);
  *reasonp++ =  x2;
  goto expand_4_6;
  };
  goto expand_4_6;
 case VARREF:
  deref_and_jump(x1,expand_4_3);
  *reasonp++ =  x1;
  goto expand_4_6;
  };
  goto expand_4_6;
 case VARREF:
  deref_and_jump(x0,expand_4_2);
  *reasonp++ =  x0;
  goto expand_4_6;
  };
  goto expand_4_6;
 case ATOMIC:
  if (a0 != NILATOM) goto expand_4_6;
  unify(a3, a2);
  x0 = NILATOM;
  unify_value(a1, x0);
  proceed();
 case VARREF:
  deref_and_jump(a0,expand_4_1);
  *reasonp++ =  a0;
  goto expand_4_6;
  };
  goto expand_4_6;
 expand_4_6:
  otherwise(expand_4_interrupt);
 expand_4_7:
  switch (ptagof(a0)) {
 case CONS:
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__cmacro_xexpand_4);
  x0 = cdr_of(a0);
  allocp[2] = x0;
  allocp[3] = x1 = makeref(&allocp[3]);
  allocp[4] = x2 = makeref(&allocp[4]);
  allocp[5] = a3;
  allocp[6] = x1;
  allocp[7] = x4 = makeref(&allocp[7]);
  x3 = makecons(&allocp[6]);
  qp = (struct goalrec*)&allocp[0];
  allocp += 8;
  unify_value(a1, x3);
  a0 = car_of(a0);
  a1 = x4;
  a3 = x2;
  execute(replaceClause_4_0);
  goto replaceClause_4_ext_interrupt;
 case VARREF:
  deref_and_jump(a0,expand_4_7);
  *reasonp++ =  a0;
  goto expand_4_interrupt;
  };
  goto expand_4_interrupt;
 expand_4_ext_interrupt:
  reasonp = 0l;
 expand_4_interrupt:
  goto interrupt_4;
 }

 insDefs_3_top: {
  q x0, x1, x2, x3, x4, x5, x6, x7, x8, x9;
  a2 = qp->args[2];
  qp = qp->next;
 insDefs_3_clear_reason:
  reasonp = reasons;
 insDefs_3_0:
 insDefs_3_1:
  switch (ptagof(a0)) {
 case FUNCTOR:
  if (functor_of(a0) != makesym(functor_functor_1)) goto insDefs_3_7;
  x0 = arg(a0, 0);
 insDefs_3_2:
  switch (ptagof(x0)) {
 case FUNCTOR:
  switch (symval(functor_of(x0))) {
 case functor__2C_2:
  x1 = arg(x0, 0);
 insDefs_3_3:
  switch (ptagof(x1)) {
 case FUNCTOR:
  if (functor_of(x1) != makesym(functor_functor_1)) goto insDefs_3_7;
  x2 = arg(x1, 0);
 insDefs_3_4:
  switch (ptagof(x2)) {
 case FUNCTOR:
  if (functor_of(x2) != makesym(functor__3D_2)) goto insDefs_3_7;
  x3 = arg(x2, 0);
 insDefs_3_5:
  switch (ptagof(x3)) {
 case FUNCTOR:
  if (functor_of(x3) != makesym(functor_variable_1)) goto insDefs_3_7;
  allocp[0] = makesym(functor_put_3);
  x5 = arg(x3, 0);
  allocp[1] = x5;
  x6 = arg(x2, 1);
  allocp[2] = x6;
  allocp[3] = x7 = makeref(&allocp[3]);
  x4 = makefunctor(&allocp[0]);
  allocp[4] = x9 = makeref(&allocp[4]);
  allocp[5] = x4;
  x8 = makecons(&allocp[4]);
  allocp += 6;
  unify_value(a1, x8);
  a0 = arg(x0, 1);
  a1 = x9;
  execute(insDefs_3_0);
  goto insDefs_3_ext_interrupt;
 case VARREF:
  deref_and_jump(x3,insDefs_3_5);
  *reasonp++ =  x3;
  goto insDefs_3_7;
  };
  goto insDefs_3_7;
 case VARREF:
  deref_and_jump(x2,insDefs_3_4);
  *reasonp++ =  x2;
  goto insDefs_3_7;
  };
  goto insDefs_3_7;
 case VARREF:
  deref_and_jump(x1,insDefs_3_3);
  *reasonp++ =  x1;
  goto insDefs_3_7;
  };
  goto insDefs_3_7;
 case functor__3D_2:
  x1 = arg(x0, 0);
 insDefs_3_6:
  switch (ptagof(x1)) {
 case FUNCTOR:
  if (functor_of(x1) != makesym(functor_variable_1)) goto insDefs_3_7;
  allocp[0] = makesym(functor_put_3);
  x3 = arg(x1, 0);
  allocp[1] = x3;
  x4 = arg(x0, 1);
  allocp[2] = x4;
  allocp[3] = x5 = makeref(&allocp[3]);
  x2 = makefunctor(&allocp[0]);
  allocp[4] = a2;
  allocp[5] = x2;
  x6 = makecons(&allocp[4]);
  allocp += 6;
  unify_value(a1, x6);
  proceed();
 case VARREF:
  deref_and_jump(x1,insDefs_3_6);
  *reasonp++ =  x1;
  goto insDefs_3_7;
  };
  goto insDefs_3_7;
  };
  goto insDefs_3_7;
 case VARREF:
  deref_and_jump(x0,insDefs_3_2);
  *reasonp++ =  x0;
  goto insDefs_3_7;
  };
  goto insDefs_3_7;
 case VARREF:
  deref_and_jump(a0,insDefs_3_1);
  *reasonp++ =  a0;
  goto insDefs_3_7;
  };
  goto insDefs_3_7;
 insDefs_3_7:
  otherwise(insDefs_3_interrupt);
 insDefs_3_8:
  switch (ptagof(a0)) {
 case FUNCTOR:
  if (functor_of(a0) != makesym(functor_functor_1)) goto insDefs_3_10;
  x0 = arg(a0, 0);
 insDefs_3_9:
  switch (ptagof(x0)) {
 case FUNCTOR:
  if (functor_of(x0) != makesym(functor__2C_2)) goto insDefs_3_10;
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__cmacro_xinsDefs_3);
  x1 = arg(x0, 1);
  allocp[2] = x1;
  allocp[3] = a1;
  allocp[4] = a2;
  allocp[5] = NILATOM;
  x3 = arg(x0, 0);
  allocp[6] = x3;
  x2 = makecons(&allocp[5]);
  allocp[7] = (q)(struct goalrec*)&allocp[0];
  allocp[8] = (q)(&predicate_klic__comp__message_xreport__error_2);
  allocp[9] = makefunctor(&string_const_0);
  allocp[10] = x2;
  qp = (struct goalrec*)&allocp[7];
  allocp += 11;
  proceed();
 case VARREF:
  deref_and_jump(x0,insDefs_3_9);
  *reasonp++ =  x0;
  goto insDefs_3_10;
  };
  goto insDefs_3_10;
 case VARREF:
  deref_and_jump(a0,insDefs_3_8);
  *reasonp++ =  a0;
  goto insDefs_3_10;
  };
  goto insDefs_3_10;
 insDefs_3_10:
  otherwise(insDefs_3_interrupt);
  unify(a2, a1);
  allocp[0] = NILATOM;
  allocp[1] = a0;
  x0 = makecons(&allocp[0]);
  allocp[2] = (q)qp;
  allocp[3] = (q)(&predicate_klic__comp__message_xreport__error_2);
  allocp[4] = makefunctor(&string_const_0);
  allocp[5] = x0;
  qp = (struct goalrec*)&allocp[2];
  allocp += 6;
  proceed();
 insDefs_3_ext_interrupt:
  reasonp = 0l;
 insDefs_3_interrupt:
  toppred = &predicate_klic__comp__cmacro_xinsDefs_3;
  goto interrupt_3;
 }

 replaceClause_4_top: {
  q x0, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13, x14, x15, x16;
  a2 = qp->args[2];
  a3 = qp->args[3];
  qp = qp->next;
 replaceClause_4_clear_reason:
  reasonp = reasons;
 replaceClause_4_0:
 replaceClause_4_1:
  switch (ptagof(a0)) {
 case FUNCTOR:
  if (functor_of(a0) != makesym(functor_functor_1)) goto replaceClause_4_5;
  x0 = arg(a0, 0);
 replaceClause_4_2:
  switch (ptagof(x0)) {
 case FUNCTOR:
  switch (symval(functor_of(x0))) {
 case functor__3A_2D_1:
  unify(a3, a2);
  allocp[0] = makesym(functor__3A_2D_1);
  x2 = arg(x0, 0);
  allocp[1] = x2;
  x1 = makefunctor(&allocp[0]);
  allocp[2] = makesym(functor_functor_1);
  allocp[3] = x1;
  x3 = makefunctor(&allocp[2]);
  allocp += 4;
  unify_value(a1, x3);
  proceed();
 case functor__3A_2D_2:
  x1 = arg(x0, 1);
 replaceClause_4_3:
  switch (ptagof(x1)) {
 case FUNCTOR:
  if (functor_of(x1) != makesym(functor_functor_1)) goto replaceClause_4_5;
  x2 = arg(x1, 0);
 replaceClause_4_4:
  switch (ptagof(x2)) {
 case FUNCTOR:
  if (functor_of(x2) != makesym(functor__7C_2)) goto replaceClause_4_5;
  allocp[0] = makesym(functor__7C_2);
  allocp[1] = x4 = makeref(&allocp[1]);
  allocp[2] = x5 = makeref(&allocp[2]);
  x3 = makefunctor(&allocp[0]);
  allocp[3] = makesym(functor_functor_1);
  allocp[4] = x3;
  x6 = makefunctor(&allocp[3]);
  allocp[5] = makesym(functor__3A_2D_2);
  allocp[6] = x8 = makeref(&allocp[6]);
  allocp[7] = x6;
  x7 = makefunctor(&allocp[5]);
  allocp[8] = makesym(functor_functor_1);
  allocp[9] = x7;
  x9 = makefunctor(&allocp[8]);
  allocp += 10;
  unify_value(a1, x9);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__cmacro_xreconstGoals_2);
  allocp[2] = x10 = makeref(&allocp[2]);
  allocp[3] = x5;
  allocp[4] = (q)(struct goalrec*)&allocp[0];
  allocp[5] = (q)(&predicate_klic__comp__cmacro_xreconstGoals_2);
  allocp[6] = x11 = makeref(&allocp[6]);
  allocp[7] = x4;
  allocp[8] = (q)(struct goalrec*)&allocp[4];
  allocp[9] = (q)(&predicate_klic__comp__cmacro_xreplaceGoals_5);
  x12 = arg(x2, 1);
  allocp[10] = x12;
  allocp[11] = NILATOM;
  allocp[12] = x10;
  allocp[13] = x13 = makeref(&allocp[13]);
  allocp[14] = a3;
  allocp[15] = (q)(struct goalrec*)&allocp[8];
  allocp[16] = (q)(&predicate_klic__comp__cmacro_xreplaceGoals_5);
  x14 = arg(x2, 0);
  allocp[17] = x14;
  allocp[18] = x15 = makeref(&allocp[18]);
  allocp[19] = x11;
  allocp[20] = x16 = makeref(&allocp[20]);
  allocp[21] = x13;
  a0 = arg(x0, 0);
  a1 = x8;
  a3 = a2;
  a4 = x16;
  a2 = x15;
  qp = (struct goalrec*)&allocp[15];
  allocp += 22;
  execute(replaceHead_5_0);
  goto replaceHead_5_ext_interrupt;
 case VARREF:
  deref_and_jump(x2,replaceClause_4_4);
  *reasonp++ =  x2;
  goto replaceClause_4_5;
  };
  goto replaceClause_4_5;
 case VARREF:
  deref_and_jump(x1,replaceClause_4_3);
  *reasonp++ =  x1;
  goto replaceClause_4_5;
  };
  goto replaceClause_4_5;
  };
  goto replaceClause_4_5;
 case VARREF:
  deref_and_jump(x0,replaceClause_4_2);
  *reasonp++ =  x0;
  goto replaceClause_4_5;
  };
  goto replaceClause_4_5;
 case VARREF:
  deref_and_jump(a0,replaceClause_4_1);
  *reasonp++ =  a0;
  goto replaceClause_4_5;
  };
  goto replaceClause_4_5;
 replaceClause_4_5:
  otherwise(replaceClause_4_interrupt);
 replaceClause_4_6:
  switch (ptagof(a0)) {
 case FUNCTOR:
  if (functor_of(a0) != makesym(functor_functor_1)) goto replaceClause_4_8;
  x0 = arg(a0, 0);
 replaceClause_4_7:
  switch (ptagof(x0)) {
 case FUNCTOR:
  if (functor_of(x0) != makesym(functor__3A_2D_2)) goto replaceClause_4_8;
  allocp[0] = makesym(functor__7C_2);
  allocp[1] = makefunctor(funct_const_1);
  x2 = arg(x0, 1);
  allocp[2] = x2;
  x1 = makefunctor(&allocp[0]);
  allocp[3] = makesym(functor_functor_1);
  allocp[4] = x1;
  x3 = makefunctor(&allocp[3]);
  allocp[5] = makesym(functor__3A_2D_2);
  x5 = arg(x0, 0);
  allocp[6] = x5;
  allocp[7] = x3;
  x4 = makefunctor(&allocp[5]);
  allocp[8] = makesym(functor_functor_1);
  allocp[9] = x4;
  x6 = makefunctor(&allocp[8]);
  a0 = x6;
  allocp += 10;
  execute(replaceClause_4_0);
  goto replaceClause_4_ext_interrupt;
 case VARREF:
  deref_and_jump(x0,replaceClause_4_7);
  *reasonp++ =  x0;
  goto replaceClause_4_8;
  };
  goto replaceClause_4_8;
 case VARREF:
  deref_and_jump(a0,replaceClause_4_6);
  *reasonp++ =  a0;
  goto replaceClause_4_8;
  };
  goto replaceClause_4_8;
 replaceClause_4_8:
  otherwise(replaceClause_4_interrupt);
 replaceClause_4_9:
  switch (ptagof(a0)) {
 case FUNCTOR:
  if (functor_of(a0) != makesym(functor_functor_1)) goto replaceClause_4_10;
  allocp[0] = makesym(functor_functor_1);
  x1 = arg(a0, 0);
  allocp[1] = x1;
  x0 = makefunctor(&allocp[0]);
  allocp[2] = makesym(functor__3A_2D_2);
  allocp[3] = x0;
  allocp[4] = makefunctor(funct_const_5);
  x2 = makefunctor(&allocp[2]);
  allocp[5] = makesym(functor_functor_1);
  allocp[6] = x2;
  x3 = makefunctor(&allocp[5]);
  a0 = x3;
  allocp += 7;
  execute(replaceClause_4_0);
  goto replaceClause_4_ext_interrupt;
 case VARREF:
  deref_and_jump(a0,replaceClause_4_9);
  *reasonp++ =  a0;
  goto replaceClause_4_10;
  };
  goto replaceClause_4_10;
 replaceClause_4_10:
  otherwise(replaceClause_4_interrupt);
  unify2(a3, a2, a0, a1);
  proceed();
 replaceClause_4_ext_interrupt:
  reasonp = 0l;
 replaceClause_4_interrupt:
  toppred = &predicate_klic__comp__cmacro_xreplaceClause_4;
  goto interrupt_4;
 }

 replaceGoals_5_top: {
  q x0, x1, x2, x3;
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  qp = qp->next;
 replaceGoals_5_clear_reason:
  reasonp = reasons;
 replaceGoals_5_0:
  x0 = a2;
  allocp[0] = x1 = makeref(&allocp[0]);
  a2 = x1;
  x2 = x1;
  x3 = a1;
  a5 = a3;
  a6 = a4;
  a4 = x3;
  a3 = x2;
  a1 = x0;
  allocp += 1;
  execute(replaceGoals_7_0);
  goto replaceGoals_7_ext_interrupt;
 replaceGoals_5_ext_interrupt:
  reasonp = 0l;
 replaceGoals_5_interrupt:
  goto interrupt_5;
 }

 replaceHead_5_top: {
  q x0, x1, x2, x3, x4;
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  qp = qp->next;
 replaceHead_5_clear_reason:
  reasonp = reasons;
 replaceHead_5_0:
 replaceHead_5_1:
  switch (ptagof(a0)) {
 case FUNCTOR:
  if (functor_of(a0) != makesym(functor_functor_1)) goto replaceHead_5_4;
  x0 = arg(a0, 0);
 replaceHead_5_2:
  if (isref(x0)) goto replaceHead_5_3;
  gblt_pfunctor(x0,x1,replaceHead_5_4);
  gblt_arity(x0,x2,replaceHead_5_4);
  allocp[0] = makesym(functor_functor_1);
  allocp[1] = x0;
  x3 = makefunctor(&allocp[0]);
  allocp += 2;
  unify_value(a1, x3);
  a0 = makeint(1L);
  a1 = x2;
  x4 = a2;
  a5 = a3;
  a6 = a4;
  a4 = NILATOM;
  a3 = x4;
  a2 = x0;
  execute(replace_7_0);
  goto replace_7_ext_interrupt;
 replaceHead_5_3:
  deref_and_jump(x0,replaceHead_5_2);
  *reasonp++ =  x0;
  goto replaceHead_5_4;
 case VARREF:
  deref_and_jump(a0,replaceHead_5_1);
  *reasonp++ =  a0;
  goto replaceHead_5_4;
  };
  goto replaceHead_5_4;
 replaceHead_5_4:
  otherwise(replaceHead_5_interrupt);
  unify2(a4, a3, a0, a1);
  x0 = NILATOM;
  unify_value(a2, x0);
  proceed();
 replaceHead_5_ext_interrupt:
  reasonp = 0l;
 replaceHead_5_interrupt:
  toppred = &predicate_klic__comp__cmacro_xreplaceHead_5;
  goto interrupt_5;
 }

 replaceGoals_7_top: {
  q x0, x1, x2, x3, x4, x5, x6;
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  a6 = qp->args[6];
  qp = qp->next;
 replaceGoals_7_clear_reason:
  reasonp = reasons;
 replaceGoals_7_0:
 replaceGoals_7_1:
  switch (ptagof(a0)) {
 case FUNCTOR:
  if (functor_of(a0) != makesym(functor_functor_1)) goto replaceGoals_7_3;
  x0 = arg(a0, 0);
 replaceGoals_7_2:
  switch (ptagof(x0)) {
 case FUNCTOR:
  if (functor_of(x0) != makesym(functor__2C_2)) goto replaceGoals_7_3;
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__cmacro_xreplaceGoals_7);
  x1 = arg(x0, 1);
  allocp[2] = x1;
  allocp[3] = x2 = makeref(&allocp[3]);
  allocp[4] = a2;
  allocp[5] = x3 = makeref(&allocp[5]);
  allocp[6] = a4;
  allocp[7] = x4 = makeref(&allocp[7]);
  allocp[8] = a6;
  allocp[9] = x2;
  x6 = arg(x0, 0);
  allocp[10] = x6;
  x5 = makecons(&allocp[9]);
  qp = (struct goalrec*)&allocp[0];
  allocp += 11;
  unify_value(a1, x5);
  a0 = arg(x0, 0);
  a1 = a3;
  a2 = x3;
  a3 = a5;
  a4 = x4;
  execute(replace_5_0);
  goto replace_5_ext_interrupt;
 case VARREF:
  deref_and_jump(x0,replaceGoals_7_2);
  *reasonp++ =  x0;
  goto replaceGoals_7_3;
  };
  goto replaceGoals_7_3;
 case VARREF:
  deref_and_jump(a0,replaceGoals_7_1);
  *reasonp++ =  a0;
  goto replaceGoals_7_3;
  };
  goto replaceGoals_7_3;
 replaceGoals_7_3:
  otherwise(replaceGoals_7_interrupt);
  allocp[0] = a2;
  allocp[1] = a0;
  x0 = makecons(&allocp[0]);
  allocp += 2;
  unify_value(a1, x0);
  a1 = a3;
  a2 = a4;
  a3 = a5;
  a4 = a6;
  execute(replace_5_0);
  goto replace_5_ext_interrupt;
 replaceGoals_7_ext_interrupt:
  reasonp = 0l;
 replaceGoals_7_interrupt:
  toppred = &predicate_klic__comp__cmacro_xreplaceGoals_7;
  goto interrupt_7;
 }

 replace_5_top: {
  q x0, x1, x2, x3, x4, x5, x6;
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  qp = qp->next;
 replace_5_clear_reason:
  reasonp = reasons;
 replace_5_0:
 replace_5_1:
  switch (ptagof(a0)) {
 case FUNCTOR:
  switch (symval(functor_of(a0))) {
 case functor_functor_1:
  x0 = arg(a0, 0);
 replace_5_2:
  if (isref(x0)) goto replace_5_3;
  gblt_pfunctor(x0,x1,replace_5_5);
  gblt_arity(x0,x2,replace_5_5);
  a0 = makeint(1L);
  x3 = a1;
  x4 = a2;
  a5 = a3;
  a6 = a4;
  a4 = x4;
  a3 = x3;
  a2 = x0;
  a1 = x2;
  execute(replace_7_0);
  goto replace_7_ext_interrupt;
 replace_5_3:
  deref_and_jump(x0,replace_5_2);
  *reasonp++ =  x0;
  goto replace_5_5;
 case functor_list_1:
  x0 = arg(a0, 0);
 replace_5_4:
  switch (ptagof(x0)) {
 case CONS:
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__cmacro_xreplace_5);
  x1 = cdr_of(x0);
  allocp[2] = x1;
  allocp[3] = x2 = makeref(&allocp[3]);
  allocp[4] = a2;
  allocp[5] = x3 = makeref(&allocp[5]);
  allocp[6] = a4;
  a0 = car_of(x0);
  a2 = x2;
  a4 = x3;
  qp = (struct goalrec*)&allocp[0];
  allocp += 7;
  execute(replace_5_0);
  goto replace_5_ext_interrupt;
 case VARREF:
  deref_and_jump(x0,replace_5_4);
  *reasonp++ =  x0;
  goto replace_5_5;
  };
  goto replace_5_5;
 case functor_variable_1:
  allocp[0] = makesym(functor_empty_2);
  x1 = arg(a0, 0);
  allocp[1] = x1;
  allocp[2] = x2 = makeref(&allocp[2]);
  x0 = makefunctor(&allocp[0]);
  allocp[3] = x4 = makeref(&allocp[3]);
  allocp[4] = x0;
  x3 = makecons(&allocp[3]);
  allocp += 5;
  unify_value(a3, x3);
  x5 = arg(a0, 0);
  x6 = a1;
  a3 = a2;
  a5 = a4;
  a4 = x4;
  a2 = x6;
  a1 = x5;
  a0 = x2;
  execute(replace_6_0);
  goto replace_6_ext_interrupt;
  };
  goto replace_5_5;
 case VARREF:
  deref_and_jump(a0,replace_5_1);
  *reasonp++ =  a0;
  goto replace_5_5;
  };
  goto replace_5_5;
 replace_5_5:
  otherwise(replace_5_interrupt);
  unify2(a4, a3, a1, a2);
  proceed();
 replace_5_ext_interrupt:
  reasonp = 0l;
 replace_5_interrupt:
  toppred = &predicate_klic__comp__cmacro_xreplace_5;
  goto interrupt_5;
 }

 replace_6_top: {
  q x0, x1, x2, x3, x4, x5, x6;
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  qp = qp->next;
 replace_6_clear_reason:
  reasonp = reasons;
 replace_6_0:
 replace_6_1:
  switch (ptagof(a0)) {
 case ATOMIC:
  if (isint(a0)) goto replace_6_interrupt;
  switch (symval(a0)) {
 case symval(makesym(atom_yes)):
  unify2(a5, a4, a2, a3);
  proceed();
 case symval(makesym(atom_no)):
  allocp[0] = makesym(functor_get__and__put_3);
  allocp[1] = a1;
  allocp[2] = x1 = makeref(&allocp[2]);
  allocp[3] = x1;
  x0 = makefunctor(&allocp[0]);
  allocp[4] = a5;
  allocp[5] = x0;
  x2 = makecons(&allocp[4]);
  allocp += 6;
  unify_value(a4, x2);
  allocp[0] = makesym(functor_variable_1);
  allocp[1] = a1;
  x3 = makefunctor(&allocp[0]);
  allocp[2] = makesym(functor__3D_2);
  allocp[3] = x3;
  allocp[4] = x1;
  x4 = makefunctor(&allocp[2]);
  allocp[5] = makesym(functor_functor_1);
  allocp[6] = x4;
  x5 = makefunctor(&allocp[5]);
  allocp[7] = a3;
  allocp[8] = x5;
  x6 = makecons(&allocp[7]);
  allocp += 9;
  unify_value(a2, x6);
  proceed();
  };
  goto replace_6_interrupt;
 case VARREF:
  deref_and_jump(a0,replace_6_1);
  *reasonp++ =  a0;
  goto replace_6_interrupt;
  };
  goto replace_6_interrupt;
 replace_6_ext_interrupt:
  reasonp = 0l;
 replace_6_interrupt:
  toppred = &predicate_klic__comp__cmacro_xreplace_6;
  goto interrupt_6;
 }

 replace_7_top: {
  q x0, x1, x2, x3;
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  a6 = qp->args[6];
  qp = qp->next;
 replace_7_clear_reason:
  reasonp = reasons;
 replace_7_0:
 replace_7_1:
  if (!isint(a0)) goto replace_7_5;
 replace_7_2:
  if (!isint(a1)) goto replace_7_4;
  gblt_greater(a0,a1,replace_7_3);
  unify2(a6, a5, a3, a4);
  proceed();
 replace_7_3:
  gblt_less_or_eq(a0,a1,replace_7_interrupt);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__cmacro_xreplace_7);
  allocp[2] = x0 = makeref(&allocp[2]);
  allocp[3] = a1;
  allocp[4] = a2;
  allocp[5] = x1 = makeref(&allocp[5]);
  allocp[6] = a4;
  allocp[7] = x2 = makeref(&allocp[7]);
  allocp[8] = a6;
  allocp[9] = (q)(struct goalrec*)&allocp[0];
  allocp[10] = (q)(&predicate_integer__arithmetics_xadd_3);
  allocp[11] = a0;
  allocp[12] = makeint(1L);
  allocp[13] = x0;
  allocp[14] = (q)(struct goalrec*)&allocp[9];
  allocp[15] = (q)(&predicate_klic__comp__cmacro_xreplace_5);
  allocp[16] = x3 = makeref(&allocp[16]);
  allocp[17] = a3;
  allocp[18] = x1;
  allocp[19] = a5;
  allocp[20] = x2;
  allocp[21] = (q)(struct goalrec*)&allocp[14];
  allocp[22] = (q)(&predicate_functor__table_xarg_3);
  allocp[23] = a0;
  allocp[24] = a2;
  allocp[25] = x3;
  qp = (struct goalrec*)&allocp[21];
  allocp += 26;
  proceed();
 replace_7_4:
  if (!isref(a1)) goto replace_7_interrupt;
  deref_and_jump(a1,replace_7_2);
  *reasonp++ =  a1;
  goto replace_7_interrupt;
 replace_7_5:
  if (!isref(a0)) goto replace_7_interrupt;
  deref_and_jump(a0,replace_7_1);
  *reasonp++ =  a0;
  goto replace_7_interrupt;
 replace_7_ext_interrupt:
  reasonp = 0l;
 replace_7_interrupt:
  toppred = &predicate_klic__comp__cmacro_xreplace_7;
  goto interrupt_7;
 }

 reconstGoals_2_top: {
  q x0, x1, x2, x3;
  qp = qp->next;
 reconstGoals_2_clear_reason:
  reasonp = reasons;
 reconstGoals_2_0:
 reconstGoals_2_1:
  switch (ptagof(a0)) {
 case CONS:
  x0 = cdr_of(a0);
 reconstGoals_2_2:
  switch (ptagof(x0)) {
 case ATOMIC:
  if (x0 != NILATOM) goto reconstGoals_2_3;
  x1 = car_of(a0);
  unify(a1, x1);
  proceed();
 case VARREF:
  deref_and_jump(x0,reconstGoals_2_2);
  *reasonp++ =  x0;
  goto reconstGoals_2_3;
  };
  goto reconstGoals_2_3;
 case ATOMIC:
  if (a0 != NILATOM) goto reconstGoals_2_3;
  x0 = makefunctor(funct_const_1);
  unify_value(a1, x0);
  proceed();
 case VARREF:
  deref_and_jump(a0,reconstGoals_2_1);
  *reasonp++ =  a0;
  goto reconstGoals_2_3;
  };
  goto reconstGoals_2_3;
 reconstGoals_2_3:
  otherwise(reconstGoals_2_interrupt);
 reconstGoals_2_4:
  switch (ptagof(a0)) {
 case CONS:
  allocp[0] = makesym(functor__2C_2);
  x1 = car_of(a0);
  allocp[1] = x1;
  allocp[2] = x2 = makeref(&allocp[2]);
  x0 = makefunctor(&allocp[0]);
  allocp[3] = makesym(functor_functor_1);
  allocp[4] = x0;
  x3 = makefunctor(&allocp[3]);
  allocp += 5;
  unify_value(a1, x3);
  a0 = cdr_of(a0);
  a1 = x2;
  execute(reconstGoals_2_0);
  goto reconstGoals_2_ext_interrupt;
 case VARREF:
  deref_and_jump(a0,reconstGoals_2_4);
  *reasonp++ =  a0;
  goto reconstGoals_2_interrupt;
  };
  goto reconstGoals_2_interrupt;
 reconstGoals_2_ext_interrupt:
  reasonp = 0l;
 reconstGoals_2_interrupt:
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
  loop_within_module(module_klic__comp__cmacro);
}
