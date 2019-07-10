/* Compiled by KLIC compiler version 3.003 (Mon Mar 29 19:42:59 JST 1999) */
#include <klic/klichdr.h>
#include "atom.h"
#include "funct.h"

module module_klic__comp__message();
Const struct predicate predicate_klic__comp__message_xreport__error_2 =
   { module_klic__comp__message, 0, 2 };
Const struct predicate predicate_klic__comp__message_xreport__error_4 =
   { module_klic__comp__message, 1, 4 };
Const struct predicate predicate_klic__comp__message_xwarning_2 =
   { module_klic__comp__message, 2, 2 };
Const struct predicate predicate_klic__comp__message_xwarning_4 =
   { module_klic__comp__message, 3, 4 };
Const struct predicate predicate_klic__comp__message_xwrite__message_3 =
   { module_klic__comp__message, 4, 3 };
Const struct predicate predicate_klic__comp__message_xwrite__message_5 =
   { module_klic__comp__message, 5, 5 };
Const struct predicate predicate_klic__comp__message_xwrite__message1_7 =
   { module_klic__comp__message, 6, 7 };
Const struct predicate predicate_klic__comp__message_xwrite__message2_8 =
   { module_klic__comp__message, 7, 8 };
Const struct predicate predicate_klic__comp__message_xcompilation__error_0 =
   { module_klic__comp__message, 8, 0 };
Const struct predicate predicate_klic__comp__message_xcompilation__error_1 =
   { module_klic__comp__message, 9, 1 };
Const struct predicate predicate_klic__comp__message_xtell_2 =
   { module_klic__comp__message, 10, 2 };
Const struct predicate predicate_klic__comp__message_xtell1_3 =
   { module_klic__comp__message, 11, 3 };
Const struct predicate predicate_klic__comp__message_xsee_2 =
   { module_klic__comp__message, 12, 2 };
Const struct predicate predicate_klic__comp__message_xsee__2_3 =
   { module_klic__comp__message, 13, 3 };
extern Const struct predicate predicate_unix_xunix_1;
extern Const struct predicate predicate_unix_xexit_1;
extern Const struct predicate predicate_klic__comp__obj_xflush_2;
extern Const struct predicate predicate_klic__comp__obj_xflush_3;
extern Const struct predicate predicate_klic__comp__obj_xklicformat_4;
extern Const struct predicate predicate_klic__comp__obj_xklicformat__stderr_2;
extern Const struct predicate predicate_klicio_xklicio_1;

module module_klic__comp__message(glbl, qp, allocp, toppred)
  struct global_variables *glbl;
  struct goalrec *qp;
  register q *allocp;
  Const struct predicate *toppred;
{
  static Const unsigned char strconst_body_0[20] =
    "Cannot open ~s!!\n";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_0 =
      declare_string_constant_8(strconst_body_0,17);
  static Const unsigned char strconst_body_1[8] =
    "In ~w:";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_1 =
      declare_string_constant_8(strconst_body_1,6);
  static Const unsigned char strconst_body_2[24] =
    "~s does not exist !!\n";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_2 =
      declare_string_constant_8(strconst_body_2,21);
  static Const unsigned char strconst_body_3[4] =
    {'~','w',':',' ',};
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_3 =
      declare_string_constant_8(strconst_body_3,4);
  static Const q funct_const_4[] = {
    makesym(functor_on__error_1),
    makeint(-1),
  };
  q a0, a1, a2, a3, a4, a5, a6, a7;

  q *reasonp;
 module_top:
  switch_on_pred() {
    case_pred(0, report__error_2_top);
    case_pred(1, report__error_4_top);
    case_pred(2, warning_2_top);
    case_pred(3, warning_4_top);
    case_pred(4, write__message_3_top);
    case_pred(5, write__message_5_top);
    case_pred(6, write__message1_7_top);
    case_pred(7, write__message2_8_top);
    case_pred(8, compilation__error_0_top);
    case_pred(9, compilation__error_1_top);
    case_pred(10, tell_2_top);
    case_pred(11, tell1_3_top);
    case_pred(12, see_2_top);
    last_case_pred(13, see__2_3_top);
  }

 report__error_2_top: {
  q x0;
  a0 = qp->args[0];
  a1 = qp->args[1];
  qp = qp->next;
 report__error_2_clear_reason:
  reasonp = reasons;
 report__error_2_0:
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__message_xcompilation__error_1);
  allocp[2] = x0 = makeref(&allocp[2]);
  a2 = x0;
  qp = (struct goalrec*)&allocp[0];
  allocp += 3;
  execute(write__message_3_0);
  goto write__message_3_ext_interrupt;
 report__error_2_ext_interrupt:
  reasonp = 0l;
 report__error_2_interrupt:
  goto interrupt_2;
 }

 report__error_4_top: {
  q x0, x1;
  a0 = qp->args[0];
  a1 = qp->args[1];
  a2 = qp->args[2];
  a3 = qp->args[3];
  qp = qp->next;
 report__error_4_clear_reason:
  reasonp = reasons;
 report__error_4_0:
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__message_xcompilation__error_1);
  allocp[2] = x0 = makeref(&allocp[2]);
  x1 = a2;
  a4 = a3;
  a3 = x1;
  a2 = x0;
  qp = (struct goalrec*)&allocp[0];
  allocp += 3;
  execute(write__message_5_0);
  goto write__message_5_ext_interrupt;
 report__error_4_ext_interrupt:
  reasonp = 0l;
 report__error_4_interrupt:
  goto interrupt_4;
 }

 warning_2_top: {
  q x0;
  a0 = qp->args[0];
  a1 = qp->args[1];
  qp = qp->next;
 warning_2_clear_reason:
  reasonp = reasons;
 warning_2_0:
  allocp[0] = x0 = makeref(&allocp[0]);
  a2 = x0;
  allocp += 1;
  execute(write__message_3_0);
  goto write__message_3_ext_interrupt;
 warning_2_ext_interrupt:
  reasonp = 0l;
 warning_2_interrupt:
  goto interrupt_2;
 }

 warning_4_top: {
  q x0, x1, x2;
  a0 = qp->args[0];
  a1 = qp->args[1];
  a2 = qp->args[2];
  a3 = qp->args[3];
  qp = qp->next;
 warning_4_clear_reason:
  reasonp = reasons;
 warning_4_0:
  allocp[0] = x1 = makeref(&allocp[0]);
  x0 = x1;
  x2 = a2;
  a4 = a3;
  a3 = x2;
  a2 = x0;
  allocp += 1;
  execute(write__message_5_0);
  goto write__message_5_ext_interrupt;
 warning_4_ext_interrupt:
  reasonp = 0l;
 warning_4_interrupt:
  goto interrupt_4;
 }

 write__message_3_top: {
  q x0, x1, x2;
  a0 = qp->args[0];
  a1 = qp->args[1];
  a2 = qp->args[2];
  qp = qp->next;
 write__message_3_clear_reason:
  reasonp = reasons;
 write__message_3_0:
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__obj_xflush_3);
  allocp[2] = a2;
  allocp[3] = x0 = makeref(&allocp[3]);
  allocp[4] = NILATOM;
  allocp[5] = x0;
  allocp[6] = makeint(10L);
  x1 = makecons(&allocp[5]);
  allocp[7] = (q)(struct goalrec*)&allocp[0];
  allocp[8] = (q)(&predicate_klic__comp__obj_xklicformat_4);
  allocp[9] = a0;
  allocp[10] = a1;
  allocp[11] = x2 = makeref(&allocp[11]);
  allocp[12] = x1;
  a0 = makesym(atom_user__error);
  a1 = x2;
  qp = (struct goalrec*)&allocp[7];
  allocp += 13;
  execute(tell_2_0);
  goto tell_2_ext_interrupt;
 write__message_3_ext_interrupt:
  reasonp = 0l;
 write__message_3_interrupt:
  toppred = &predicate_klic__comp__message_xwrite__message_3;
  goto interrupt_3;
 }

 write__message_5_top: {
  q x0, x1, x2, x3, x4, x5, x6;
  a0 = qp->args[0];
  a1 = qp->args[1];
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  qp = qp->next;
 write__message_5_clear_reason:
  reasonp = reasons;
 write__message_5_0:
  allocp[0] = makesym(functor_empty_2);
  allocp[1] = makesym(atom_compiling__module);
  allocp[2] = x1 = makeref(&allocp[2]);
  x0 = makefunctor(&allocp[0]);
  allocp[3] = makesym(functor_empty_2);
  allocp[4] = makesym(atom_compiling__predicate);
  allocp[5] = x3 = makeref(&allocp[5]);
  x2 = makefunctor(&allocp[3]);
  allocp[6] = x5 = makeref(&allocp[6]);
  allocp[7] = x2;
  x4 = makecons(&allocp[6]);
  allocp[8] = x4;
  allocp[9] = x0;
  x6 = makecons(&allocp[8]);
  allocp += 10;
  unify_value(a3, x6);
  a3 = x1;
  a5 = x5;
  a6 = a4;
  a4 = x3;
  execute(write__message1_7_0);
  goto write__message1_7_ext_interrupt;
 write__message_5_ext_interrupt:
  reasonp = 0l;
 write__message_5_interrupt:
  toppred = &predicate_klic__comp__message_xwrite__message_5;
  goto interrupt_5;
 }

 write__message1_7_top: {
  q x0, x1, x2, x3, x4, x5, x6, x7, x8, x9;
  a0 = qp->args[0];
  a1 = qp->args[1];
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  a6 = qp->args[6];
  qp = qp->next;
 write__message1_7_clear_reason:
  reasonp = reasons;
 write__message1_7_0:
 write__message1_7_1:
  switch (ptagof(a3)) {
 case ATOMIC:
  if (isint(a3)) goto write__message1_7_interrupt;
  switch (symval(a3)) {
 case symval(makesym(atom_yes)):
  allocp[0] = makesym(functor_get_2);
  allocp[1] = makesym(atom_compiling__module);
  allocp[2] = x1 = makeref(&allocp[2]);
  x0 = makefunctor(&allocp[0]);
  allocp[3] = makesym(functor_put_3);
  allocp[4] = makesym(atom_compiling__module);
  allocp[5] = x1;
  allocp[6] = x3 = makeref(&allocp[6]);
  x2 = makefunctor(&allocp[3]);
  allocp[7] = x5 = makeref(&allocp[7]);
  allocp[8] = x2;
  x4 = makecons(&allocp[7]);
  allocp[9] = x4;
  allocp[10] = x0;
  x6 = makecons(&allocp[9]);
  allocp += 11;
  unify_value(a5, x6);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__message_xwrite__message2_8);
  allocp[2] = a0;
  allocp[3] = a1;
  allocp[4] = a2;
  allocp[5] = a4;
  allocp[6] = x5;
  allocp[7] = a6;
  allocp[8] = x7 = makeref(&allocp[8]);
  allocp[9] = NILATOM;
  allocp[10] = NILATOM;
  allocp[11] = x1;
  x8 = makecons(&allocp[10]);
  allocp[12] = (q)(struct goalrec*)&allocp[0];
  allocp[13] = (q)(&predicate_klic__comp__obj_xklicformat_4);
  allocp[14] = makefunctor(&string_const_1);
  allocp[15] = x8;
  allocp[16] = x9 = makeref(&allocp[16]);
  allocp[17] = x7;
  a0 = makesym(atom_user__error);
  a1 = x9;
  qp = (struct goalrec*)&allocp[12];
  allocp += 18;
  execute(tell_2_0);
  goto tell_2_ext_interrupt;
 case symval(makesym(atom_no)):
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__message_xwrite__message2_8);
  allocp[2] = a0;
  allocp[3] = a1;
  allocp[4] = a2;
  allocp[5] = a4;
  allocp[6] = a5;
  allocp[7] = a6;
  allocp[8] = x0 = makeref(&allocp[8]);
  allocp[9] = NILATOM;
  a0 = makesym(atom_user__error);
  a1 = x0;
  qp = (struct goalrec*)&allocp[0];
  allocp += 10;
  execute(tell_2_0);
  goto tell_2_ext_interrupt;
  };
  goto write__message1_7_interrupt;
 case VARREF:
  deref_and_jump(a3,write__message1_7_1);
  *reasonp++ =  a3;
  goto write__message1_7_interrupt;
  };
  goto write__message1_7_interrupt;
 write__message1_7_ext_interrupt:
  reasonp = 0l;
 write__message1_7_interrupt:
  toppred = &predicate_klic__comp__message_xwrite__message1_7;
  goto interrupt_7;
 }

 write__message2_8_top: {
  q x0, x1, x2, x3, x4, x5, x6, x7, x8, x9;
  a0 = qp->args[0];
  a1 = qp->args[1];
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  a6 = qp->args[6];
  a7 = qp->args[7];
  qp = qp->next;
 write__message2_8_clear_reason:
  reasonp = reasons;
 write__message2_8_0:
 write__message2_8_1:
  switch (ptagof(a3)) {
 case ATOMIC:
  if (isint(a3)) goto write__message2_8_interrupt;
  switch (symval(a3)) {
 case symval(makesym(atom_yes)):
  allocp[0] = makesym(functor_get_2);
  allocp[1] = makesym(atom_compiling__predicate);
  allocp[2] = x1 = makeref(&allocp[2]);
  x0 = makefunctor(&allocp[0]);
  allocp[3] = makesym(functor_put_3);
  allocp[4] = makesym(atom_compiling__predicate);
  allocp[5] = x1;
  allocp[6] = x3 = makeref(&allocp[6]);
  x2 = makefunctor(&allocp[3]);
  allocp[7] = a5;
  allocp[8] = x2;
  x4 = makecons(&allocp[7]);
  allocp[9] = x4;
  allocp[10] = x0;
  x5 = makecons(&allocp[9]);
  allocp += 11;
  unify_value(a4, x5);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__obj_xflush_3);
  allocp[2] = a2;
  allocp[3] = x6 = makeref(&allocp[3]);
  allocp[4] = a7;
  allocp[5] = x6;
  allocp[6] = makeint(10L);
  x7 = makecons(&allocp[5]);
  allocp[7] = (q)(struct goalrec*)&allocp[0];
  allocp[8] = (q)(&predicate_klic__comp__obj_xklicformat_4);
  allocp[9] = a0;
  allocp[10] = a1;
  allocp[11] = x8 = makeref(&allocp[11]);
  allocp[12] = x7;
  allocp[13] = NILATOM;
  allocp[14] = x1;
  x9 = makecons(&allocp[13]);
  allocp[15] = (q)(struct goalrec*)&allocp[7];
  allocp[16] = (q)(&predicate_klic__comp__obj_xklicformat_4);
  allocp[17] = makefunctor(&string_const_3);
  allocp[18] = x9;
  allocp[19] = a6;
  allocp[20] = x8;
  qp = (struct goalrec*)&allocp[15];
  allocp += 21;
  proceed();
 case symval(makesym(atom_no)):
  unify(a4, a5);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__obj_xflush_3);
  allocp[2] = a2;
  allocp[3] = x0 = makeref(&allocp[3]);
  allocp[4] = a7;
  allocp[5] = x0;
  allocp[6] = makeint(10L);
  x1 = makecons(&allocp[5]);
  allocp[7] = (q)(struct goalrec*)&allocp[0];
  allocp[8] = (q)(&predicate_klic__comp__obj_xklicformat_4);
  allocp[9] = a0;
  allocp[10] = a1;
  allocp[11] = a6;
  allocp[12] = x1;
  qp = (struct goalrec*)&allocp[7];
  allocp += 13;
  proceed();
  };
  goto write__message2_8_interrupt;
 case VARREF:
  deref_and_jump(a3,write__message2_8_1);
  *reasonp++ =  a3;
  goto write__message2_8_interrupt;
  };
  goto write__message2_8_interrupt;
 write__message2_8_ext_interrupt:
  reasonp = 0l;
 write__message2_8_interrupt:
  goto interrupt_8;
 }

 compilation__error_0_top: {
  q x0;
  qp = qp->next;
 compilation__error_0_clear_reason:
  reasonp = reasons;
 compilation__error_0_0:
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__message_xcompilation__error_1);
  allocp[2] = x0 = makeref(&allocp[2]);
  allocp[3] = (q)(struct goalrec*)&allocp[0];
  allocp[4] = (q)(&predicate_klic__comp__obj_xflush_2);
  allocp[5] = makesym(atom_user__error);
  allocp[6] = x0;
  qp = (struct goalrec*)&allocp[3];
  allocp += 7;
  proceed();
 compilation__error_0_ext_interrupt:
  reasonp = 0l;
 compilation__error_0_interrupt:
  goto interrupt_0;
 }

 compilation__error_1_top: {
  a0 = qp->args[0];
  qp = qp->next;
 compilation__error_1_clear_reason:
  reasonp = reasons;
 compilation__error_1_0:
 compilation__error_1_1:
  if (isref(a0)) goto compilation__error_1_2;
  gblt_wait(a0,compilation__error_1_interrupt);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_unix_xexit_1);
  allocp[2] = makeint(-1L);
  qp = (struct goalrec*)&allocp[0];
  allocp += 3;
  proceed();
 compilation__error_1_2:
  deref_and_jump(a0,compilation__error_1_1);
  *reasonp++ =  a0;
  goto compilation__error_1_interrupt;
 compilation__error_1_ext_interrupt:
  reasonp = 0l;
 compilation__error_1_interrupt:
  goto interrupt_1;
 }

 tell_2_top: {
  q x0, x1, x2;
  a0 = qp->args[0];
  a1 = qp->args[1];
  qp = qp->next;
 tell_2_clear_reason:
  reasonp = reasons;
 tell_2_0:
 tell_2_1:
  switch (ptagof(a0)) {
 case ATOMIC:
  if (a0 != makesym(atom_user__error)) goto tell_2_2;
  allocp[0] = makesym(functor_normal_1);
  allocp[1] = a1;
  x0 = makefunctor(&allocp[0]);
  allocp[2] = makesym(functor_stderr_1);
  allocp[3] = x0;
  x1 = makefunctor(&allocp[2]);
  allocp[4] = NILATOM;
  allocp[5] = x1;
  x2 = makecons(&allocp[4]);
  allocp[6] = (q)qp;
  allocp[7] = (q)(&predicate_unix_xunix_1);
  allocp[8] = x2;
  qp = (struct goalrec*)&allocp[6];
  allocp += 9;
  proceed();
 case VARREF:
  deref_and_jump(a0,tell_2_1);
  *reasonp++ =  a0;
  goto tell_2_2;
  };
  goto tell_2_2;
 tell_2_2:
  otherwise(tell_2_interrupt);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__message_xtell1_3);
  allocp[2] = a0;
  allocp[3] = x0 = makeref(&allocp[3]);
  allocp[4] = a1;
  allocp[5] = makesym(functor_write__open_2);
  allocp[6] = a0;
  allocp[7] = x0;
  x1 = makefunctor(&allocp[5]);
  allocp[8] = NILATOM;
  allocp[9] = x1;
  x2 = makecons(&allocp[8]);
  allocp[10] = (q)(struct goalrec*)&allocp[0];
  allocp[11] = (q)(&predicate_unix_xunix_1);
  allocp[12] = x2;
  qp = (struct goalrec*)&allocp[10];
  allocp += 13;
  proceed();
 tell_2_ext_interrupt:
  reasonp = 0l;
 tell_2_interrupt:
  toppred = &predicate_klic__comp__message_xtell_2;
  goto interrupt_2;
 }

 tell1_3_top: {
  q x0, x1;
  a0 = qp->args[0];
  a1 = qp->args[1];
  a2 = qp->args[2];
  qp = qp->next;
 tell1_3_clear_reason:
  reasonp = reasons;
 tell1_3_0:
 tell1_3_1:
  switch (ptagof(a1)) {
 case FUNCTOR:
  if (functor_of(a1) != makesym(functor_normal_1)) goto tell1_3_2;
  allocp[0] = makesym(functor_normal_1);
  x1 = arg(a1, 0);
  allocp[1] = x1;
  x0 = makefunctor(&allocp[0]);
  allocp += 2;
  unify_value(a2, x0);
  proceed();
 case VARREF:
  deref_and_jump(a1,tell1_3_1);
  *reasonp++ =  a1;
  goto tell1_3_2;
  };
  goto tell1_3_2;
 tell1_3_2:
  otherwise(tell1_3_interrupt);
  unify(a2, a1);
  allocp[0] = NILATOM;
  allocp[1] = a0;
  x0 = makecons(&allocp[0]);
  allocp[2] = (q)qp;
  allocp[3] = (q)(&predicate_klic__comp__obj_xklicformat__stderr_2);
  allocp[4] = makefunctor(&string_const_0);
  allocp[5] = x0;
  qp = (struct goalrec*)&allocp[2];
  allocp += 6;
  proceed();
 tell1_3_ext_interrupt:
  reasonp = 0l;
 tell1_3_interrupt:
  goto interrupt_3;
 }

 see_2_top: {
  q x0, x1, x2;
  a0 = qp->args[0];
  a1 = qp->args[1];
  qp = qp->next;
 see_2_clear_reason:
  reasonp = reasons;
 see_2_0:
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__message_xsee__2_3);
  allocp[2] = a0;
  allocp[3] = x0 = makeref(&allocp[3]);
  allocp[4] = a1;
  allocp[5] = makesym(functor_read__open_2);
  allocp[6] = a0;
  allocp[7] = x0;
  x1 = makefunctor(&allocp[5]);
  allocp[8] = NILATOM;
  allocp[9] = x1;
  x2 = makecons(&allocp[8]);
  allocp[10] = (q)(struct goalrec*)&allocp[0];
  allocp[11] = (q)(&predicate_klicio_xklicio_1);
  allocp[12] = x2;
  qp = (struct goalrec*)&allocp[10];
  allocp += 13;
  proceed();
 see_2_ext_interrupt:
  reasonp = 0l;
 see_2_interrupt:
  goto interrupt_2;
 }

 see__2_3_top: {
  q x0, x1, x2, x3;
  a0 = qp->args[0];
  a1 = qp->args[1];
  a2 = qp->args[2];
  qp = qp->next;
 see__2_3_clear_reason:
  reasonp = reasons;
 see__2_3_0:
 see__2_3_1:
  switch (ptagof(a1)) {
 case FUNCTOR:
  if (functor_of(a1) != makesym(functor_normal_1)) goto see__2_3_2;
  allocp[0] = x1 = makeref(&allocp[0]);
  allocp[1] = makefunctor(funct_const_4);
  x0 = makecons(&allocp[0]);
  x2 = arg(a1, 0);
  allocp += 2;
  unify_value(x2, x0);
  allocp[0] = makesym(functor_normal_1);
  allocp[1] = x1;
  x3 = makefunctor(&allocp[0]);
  allocp += 2;
  unify_value(a2, x3);
  proceed();
 case VARREF:
  deref_and_jump(a1,see__2_3_1);
  *reasonp++ =  a1;
  goto see__2_3_2;
  };
  goto see__2_3_2;
 see__2_3_2:
  otherwise(see__2_3_interrupt);
  unify(a2, a1);
  allocp[0] = NILATOM;
  allocp[1] = a0;
  x0 = makecons(&allocp[0]);
  allocp[2] = (q)qp;
  allocp[3] = (q)(&predicate_klic__comp__obj_xklicformat__stderr_2);
  allocp[4] = makefunctor(&string_const_2);
  allocp[5] = x0;
  qp = (struct goalrec*)&allocp[2];
  allocp += 6;
  proceed();
 see__2_3_ext_interrupt:
  reasonp = 0l;
 see__2_3_interrupt:
  goto interrupt_3;
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
  loop_within_module(module_klic__comp__message);
}
