/* Compiled by KLIC compiler version 3.003 (Mon Mar 29 19:42:59 JST 1999) */
#include <klic/klichdr.h>
#include "atom.h"
#include "funct.h"

module module_unparser();
Const struct predicate predicate_unparser_xunparse_4 =
   { module_unparser, 0, 4 };
Const struct predicate predicate_unparser_xunparse_6 =
   { module_unparser, 1, 6 };
Const struct predicate predicate_unparser_xunwparse_4 =
   { module_unparser, 2, 4 };
Const struct predicate predicate_unparser_xunwparse_6 =
   { module_unparser, 3, 6 };
Const struct predicate predicate_unparser_xunparse__int_3 =
   { module_unparser, 4, 3 };
Const struct predicate predicate_unparser_xint__chars_3 =
   { module_unparser, 5, 3 };
Const struct predicate predicate_unparser_xunparse__float_3 =
   { module_unparser, 6, 3 };
Const struct predicate predicate_unparser_xunparse__cons_5 =
   { module_unparser, 7, 5 };
Const struct predicate predicate_unparser_xunparse__cdr_5 =
   { module_unparser, 8, 5 };
Const struct predicate predicate_unparser_xunwparse__cons_5 =
   { module_unparser, 9, 5 };
Const struct predicate predicate_unparser_xunwparse__cdr_5 =
   { module_unparser, 10, 5 };
Const struct predicate predicate_unparser_xunparse__vector_5 =
   { module_unparser, 11, 5 };
Const struct predicate predicate_unparser_xunparse__vecelems_7 =
   { module_unparser, 12, 7 };
Const struct predicate predicate_unparser_xunparse__string_3 =
   { module_unparser, 13, 3 };
Const struct predicate predicate_unparser_xstr__chars_5 =
   { module_unparser, 14, 5 };
Const struct predicate predicate_unparser_xstr__elem__chars_3 =
   { module_unparser, 15, 3 };
Const struct predicate predicate_unparser_xins__backslash_3 =
   { module_unparser, 16, 3 };
Const struct predicate predicate_unparser_xunparse__general_6 =
   { module_unparser, 17, 6 };
Const struct predicate predicate_unparser_xins__if__greater__than_5 =
   { module_unparser, 18, 5 };
Const struct predicate predicate_unparser_xunparse__with__op_10 =
   { module_unparser, 19, 10 };
Const struct predicate predicate_unparser_xunparse__elems_7 =
   { module_unparser, 20, 7 };
Const struct predicate predicate_unparser_xunparse__atom_3 =
   { module_unparser, 21, 3 };
Const struct predicate predicate_unparser_xatom__kind_3 =
   { module_unparser, 22, 3 };
Const struct predicate predicate_unparser_xatom__kind_2F3_240_4 =
   { module_unparser, 23, 4 };
Const struct predicate predicate_unparser_xfirst__char__kind_2 =
   { module_unparser, 24, 2 };
Const struct predicate predicate_unparser_xjudge__quoted_6 =
   { module_unparser, 25, 6 };
Const struct predicate predicate_unparser_xchar__kind_2 =
   { module_unparser, 26, 2 };
Const struct predicate predicate_unparser_xmake__atom__string_4 =
   { module_unparser, 27, 4 };
Const struct predicate predicate_unparser_xquoted__atom__chars_4 =
   { module_unparser, 28, 4 };
Const struct predicate predicate_unparser_xquoted__atom__chars_2F4_240_3 =
   { module_unparser, 29, 3 };
Const struct predicate predicate_unparser_xfind__operator_3 =
   { module_unparser, 30, 3 };
Const struct predicate predicate_unparser_xmax__op_4 =
   { module_unparser, 31, 4 };
Const struct predicate predicate_unparser_xmax_3 =
   { module_unparser, 32, 3 };
Const struct predicate predicate_unparser_xunparse__bin__op_3 =
   { module_unparser, 33, 3 };
Const struct predicate predicate_unparser_xunparse__comma__list__elem_5 =
   { module_unparser, 34, 5 };
Const struct predicate predicate_unparser_xcall__unparser_6 =
   { module_unparser, 35, 6 };
Const struct predicate predicate_unparser_xunscan_4 =
   { module_unparser, 36, 4 };
Const struct predicate predicate_unparser_xunscan__after__op_4 =
   { module_unparser, 37, 4 };
Const struct predicate predicate_unparser_xchartype_2 =
   { module_unparser, 38, 2 };
Const struct predicate predicate_unparser_xchartype_2F2_240_2 =
   { module_unparser, 39, 2 };
Const struct predicate predicate_unparser_xdelimit_4 =
   { module_unparser, 40, 4 };
extern q string_g_new();
extern Const struct predicate predicate_unparser_xunparse_6;
extern Const struct predicate predicate_unparser_xunwparse_6;
extern Const struct predicate predicate_generic_xgeneric_2;
extern Const struct predicate predicate_functor__table_x_3D_2E_2E_2;
extern Const struct predicate predicate_atom__table_xget__atom__string_2;
extern Const struct predicate predicate_scanner_xchartype_2;
declare_method_table_of(vector);

module module_unparser(glbl, qp, allocp, toppred)
  struct global_variables *glbl;
  struct goalrec *qp;
  register q *allocp;
  Const struct predicate *toppred;
{
  static Const unsigned char strconst_body_0[4] =
    "\'.\'";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_0 =
      declare_string_constant_8(strconst_body_0,3);
  static Const unsigned char strconst_body_1[4] =
    "[]";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_1 =
      declare_string_constant_8(strconst_body_1,2);
  static Const q cons_const_2[] = {
    NILATOM,
    makeint(34),
  };
  static Const q cons_const_3[] = {
    NILATOM,
    makeint(39),
  };
  static Const unsigned char strconst_body_4[4] =
    " (";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_4 =
      declare_string_constant_8(strconst_body_4,2);
  static Const q funct_const_5[] = {
    makesym(functor_fwrite_1),
    makefunctor(&string_const_4),
  };
  static Const q funct_const_6[] = {
    makesym(functor_xfy_1),
    makeint(1100),
  };
  static Const q funct_const_7[] = {
    makesym(functor_none_1),
    makeint(0),
  };
  declare_method_table_of(module);
  extern module module_unparser();
  static Const module_structure_type mod_const_8 =
    declare_module_constant(module_unparser, makesym(atom_unparser));
  declare_method_table_of(predicate);
  static Const predicate_structure_type pred_const_9 =
    declare_pred_constant(predicate_unparser_xunparse_6, mod_const_8, makesym(atom_unparse));
  declare_method_table_of(module);
  extern module module_unparser();
  static Const module_structure_type mod_const_10 =
    declare_module_constant(module_unparser, makesym(atom_unparser));
  declare_method_table_of(predicate);
  static Const predicate_structure_type pred_const_11 =
    declare_pred_constant(predicate_unparser_xunwparse_6, mod_const_10, makesym(atom_unwparse));
  q a0, a1, a2, a3, a4, a5, a6, a7, a8, a9;

  q *reasonp;
 module_top:
  a0 = qp->args[0];
  a1 = qp->args[1];
  switch_on_pred() {
    case_pred(0, unparse_4_top);
    case_pred(1, unparse_6_top);
    case_pred(2, unwparse_4_top);
    case_pred(3, unwparse_6_top);
    case_pred(4, unparse__int_3_top);
    case_pred(5, int__chars_3_top);
    case_pred(6, unparse__float_3_top);
    case_pred(7, unparse__cons_5_top);
    case_pred(8, unparse__cdr_5_top);
    case_pred(9, unwparse__cons_5_top);
    case_pred(10, unwparse__cdr_5_top);
    case_pred(11, unparse__vector_5_top);
    case_pred(12, unparse__vecelems_7_top);
    case_pred(13, unparse__string_3_top);
    case_pred(14, str__chars_5_top);
    case_pred(15, str__elem__chars_3_top);
    case_pred(16, ins__backslash_3_top);
    case_pred(17, unparse__general_6_top);
    case_pred(18, ins__if__greater__than_5_top);
    case_pred(19, unparse__with__op_10_top);
    case_pred(20, unparse__elems_7_top);
    case_pred(21, unparse__atom_3_top);
    case_pred(22, atom__kind_3_top);
    case_pred(23, atom__kind_2F3_240_4_top);
    case_pred(24, first__char__kind_2_top);
    case_pred(25, judge__quoted_6_top);
    case_pred(26, char__kind_2_top);
    case_pred(27, make__atom__string_4_top);
    case_pred(28, quoted__atom__chars_4_top);
    case_pred(29, quoted__atom__chars_2F4_240_3_top);
    case_pred(30, find__operator_3_top);
    case_pred(31, max__op_4_top);
    case_pred(32, max_3_top);
    case_pred(33, unparse__bin__op_3_top);
    case_pred(34, unparse__comma__list__elem_5_top);
    case_pred(35, call__unparser_6_top);
    case_pred(36, unscan_4_top);
    case_pred(37, unscan__after__op_4_top);
    case_pred(38, chartype_2_top);
    case_pred(39, chartype_2F2_240_2_top);
    last_case_pred(40, delimit_4_top);
  }

 unparse_4_top: {
  q x0;
  a2 = qp->args[2];
  a3 = qp->args[3];
  qp = qp->next;
 unparse_4_clear_reason:
  reasonp = reasons;
 unparse_4_0:
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_unparser_xunscan_4);
  allocp[2] = x0 = makeref(&allocp[2]);
  allocp[3] = makeint(32L);
  allocp[4] = a2;
  allocp[5] = a3;
  a2 = makeint(1200L);
  a3 = makepred(pred_const_9);
  a4 = x0;
  a5 = NILATOM;
  qp = (struct goalrec*)&allocp[0];
  allocp += 6;
  execute(unparse_6_0);
  goto unparse_6_ext_interrupt;
 unparse_4_ext_interrupt:
  reasonp = 0l;
 unparse_4_interrupt:
  goto interrupt_4;
 }

 unparse_6_top: {
  q x0, x1, x2;
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  qp = qp->next;
 unparse_6_clear_reason:
  reasonp = reasons;
 unparse_6_0:
 unparse_6_1:
  switch (ptagof(a0)) {
 case CONS:
  a2 = a3;
  a3 = a4;
  a4 = a5;
  execute(unparse__cons_5_0);
  goto unparse__cons_5_ext_interrupt;
 case ATOMIC:
  if (!isint(a0)) goto unparse_6_6;
 unparse_6_2:
  gblt_integer(a0,unparse_6_6);
  a1 = a4;
  a2 = a5;
  execute(unparse__int_3_0);
  goto unparse__int_3_ext_interrupt;
 case FUNCTOR:
 unparse_6_3:
  if (!isgobj(a0)) goto unparse_6_6;
  if (!isclass(a0,vector)) goto unparse_6_4;
  gblt_size_of_vector(a0,x0,unparse_6_4);
  a2 = a3;
  a3 = a4;
  a4 = a5;
  execute(unparse__vector_5_0);
  goto unparse__vector_5_ext_interrupt;
 unparse_6_4:
  guard_generic(a0, functor_float_0, 0, unparse_6_5);
  a1 = a4;
  a2 = a5;
  execute(unparse__float_3_0);
  goto unparse__float_3_ext_interrupt;
 unparse_6_5:
  gblt_is_string(a0,unparse_6_6);
  gblt_size_of_string(a0,x1,unparse_6_6);
  gblt_element_size_of_string(a0,x2,unparse_6_6);
  a1 = a4;
  a2 = a5;
  execute(unparse__string_3_0);
  goto unparse__string_3_ext_interrupt;
 case VARREF:
  deref_and_jump(a0,unparse_6_1);
  *reasonp++ =  a0;
  goto unparse_6_6;
  };
  goto unparse_6_6;
 unparse_6_6:
  otherwise(unparse_6_interrupt);
  execute(unparse__general_6_0);
  goto unparse__general_6_ext_interrupt;
 unparse_6_ext_interrupt:
  reasonp = 0l;
 unparse_6_interrupt:
  toppred = &predicate_unparser_xunparse_6;
  goto interrupt_6;
 }

 unwparse_4_top: {
  q x0;
  a2 = qp->args[2];
  a3 = qp->args[3];
  qp = qp->next;
 unwparse_4_clear_reason:
  reasonp = reasons;
 unwparse_4_0:
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_unparser_xunscan_4);
  allocp[2] = x0 = makeref(&allocp[2]);
  allocp[3] = makeint(32L);
  allocp[4] = a2;
  allocp[5] = a3;
  a2 = makeint(1200L);
  a3 = makepred(pred_const_11);
  a4 = x0;
  a5 = NILATOM;
  qp = (struct goalrec*)&allocp[0];
  allocp += 6;
  execute(unwparse_6_0);
  goto unwparse_6_ext_interrupt;
 unwparse_4_ext_interrupt:
  reasonp = 0l;
 unwparse_4_interrupt:
  goto interrupt_4;
 }

 unwparse_6_top: {
  q x0, x1, x2, x3, x4, x5, x6;
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  qp = qp->next;
 unwparse_6_clear_reason:
  reasonp = reasons;
 unwparse_6_0:
 unwparse_6_1:
  switch (ptagof(a0)) {
 case FUNCTOR:
  switch (symval(functor_of(a0))) {
 case functor_integer_1:
  a0 = arg(a0, 0);
  a1 = a4;
  a2 = a5;
  execute(unparse__int_3_0);
  goto unparse__int_3_ext_interrupt;
 case functor_floating__point_1:
  a0 = arg(a0, 0);
  a1 = a4;
  a2 = a5;
  execute(unparse__float_3_0);
  goto unparse__float_3_ext_interrupt;
 case functor_list_1:
  a0 = arg(a0, 0);
  a2 = a3;
  a3 = a4;
  a4 = a5;
  execute(unwparse__cons_5_0);
  goto unwparse__cons_5_ext_interrupt;
 case functor_vector_1:
  a0 = arg(a0, 0);
  a2 = a3;
  a3 = a4;
  a4 = a5;
  execute(unparse__vector_5_0);
  goto unparse__vector_5_ext_interrupt;
 case functor_string_1:
  a0 = arg(a0, 0);
  a1 = a4;
  a2 = a5;
  execute(unparse__string_3_0);
  goto unparse__string_3_ext_interrupt;
 case functor_variable_1:
  x0 = arg(a0, 0);
 unwparse_6_2:
  switch (ptagof(x0)) {
 case ATOMIC:
  if (!isint(x0)) goto unwparse_6_interrupt;
 unwparse_6_3:
  gblt_integer(x0,unwparse_6_interrupt);
  allocp[0] = a5;
  allocp[1] = x2 = makeref(&allocp[1]);
  x1 = makecons(&allocp[0]);
  allocp += 2;
  unify_value(a4, x1);
  allocp[0] = x4 = makeref(&allocp[0]);
  allocp[1] = makeint(95L);
  x3 = makecons(&allocp[0]);
  generic_arg[0] = x3;
  x5 = makeint(8L);
  generic_arg[1] = x5;
  new_generic(string_g_new, 2, x6, 2);
  unify(x2, x6);
  a0 = x0;
  a1 = x4;
  a2 = NILATOM;
  execute(int__chars_3_0);
  goto int__chars_3_ext_interrupt;
 case FUNCTOR:
 unwparse_6_4:
  if (!isgobj(x0)) goto unwparse_6_interrupt;
 unwparse_6_5:
  gblt_is_string(x0,unwparse_6_interrupt);
  gblt_size_of_string(x0,x1,unwparse_6_interrupt);
  gblt_element_size_of_string(x0,x2,unwparse_6_interrupt);
  allocp[0] = a5;
  allocp[1] = x0;
  x3 = makecons(&allocp[0]);
  allocp += 2;
  unify_value(a4, x3);
  proceed();
 case VARREF:
  deref_and_jump(x0,unwparse_6_2);
  *reasonp++ =  x0;
  goto unwparse_6_interrupt;
  };
  goto unwparse_6_interrupt;
 case functor_functor_1:
  a0 = arg(a0, 0);
  execute(unparse__general_6_0);
  goto unparse__general_6_ext_interrupt;
 case functor_atom_1:
  a0 = arg(a0, 0);
  execute(unparse__general_6_0);
  goto unparse__general_6_ext_interrupt;
  };
  goto unwparse_6_interrupt;
 case VARREF:
  deref_and_jump(a0,unwparse_6_1);
  *reasonp++ =  a0;
  goto unwparse_6_interrupt;
  };
  goto unwparse_6_interrupt;
 unwparse_6_ext_interrupt:
  reasonp = 0l;
 unwparse_6_interrupt:
  toppred = &predicate_unparser_xunwparse_6;
  goto interrupt_6;
 }

 unparse__int_3_top: {
  q x0, x1, x2, x3, x4;
  a2 = qp->args[2];
  qp = qp->next;
 unparse__int_3_clear_reason:
  reasonp = reasons;
 unparse__int_3_0:
  allocp[0] = a2;
  allocp[1] = x1 = makeref(&allocp[1]);
  x0 = makecons(&allocp[0]);
  allocp += 2;
  unify_value(a1, x0);
  allocp[0] = x2 = makeref(&allocp[0]);
  generic_arg[0] = x2;
  x3 = makeint(8L);
  generic_arg[1] = x3;
  new_generic(string_g_new, 2, x4, 1);
  unify(x1, x4);
  a1 = x2;
  a2 = NILATOM;
  execute(int__chars_3_0);
  goto int__chars_3_ext_interrupt;
 unparse__int_3_ext_interrupt:
  reasonp = 0l;
 unparse__int_3_interrupt:
  toppred = &predicate_unparser_xunparse__int_3;
  goto interrupt_3;
 }

 int__chars_3_top: {
  q x0, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13, x14;
  a2 = qp->args[2];
  qp = qp->next;
 int__chars_3_clear_reason:
  reasonp = reasons;
 int__chars_3_0:
 int__chars_3_1:
  if (!isint(a0)) goto int__chars_3_5;
  x0 = makeint(0L);
  gblt_greater_or_eq(a0,x0,int__chars_3_3);
  x1 = makeint(10L);
  gblt_less(a0,x1,int__chars_3_2);
  x2 = makeint(48L);
  bblt_add_no_check(x2,a0,x3);
  allocp[0] = a2;
  allocp[1] = x3;
  x4 = makecons(&allocp[0]);
  allocp += 2;
  unify_value(a1, x4);
  proceed();
 int__chars_3_2:
  x2 = makeint(10L);
  gblt_greater_or_eq(a0,x2,int__chars_3_3);
  x3 = makeint(10L);
  bblt_mod_no_check(a0,x3,x4);
  x5 = makeint(48L);
  bblt_add_no_check(x5,x4,x6);
  x7 = makeint(10L);
  bblt_div_no_check(a0,x7,x8);
  allocp[0] = a2;
  allocp[1] = x6;
  x9 = makecons(&allocp[0]);
  a0 = x8;
  a2 = x9;
  allocp += 2;
  execute(int__chars_3_0);
  goto int__chars_3_ext_interrupt;
 int__chars_3_3:
  x1 = makeint(0L);
  gblt_less(a0,x1,int__chars_3_interrupt);
  gblt_minus(a0,x2,int__chars_3_interrupt);
  gblt_eq(a0,x2,int__chars_3_4);
  allocp[0] = x4 = makeref(&allocp[0]);
  allocp[1] = makeint(45L);
  x3 = makecons(&allocp[0]);
  allocp += 2;
  unify_value(a1, x3);
  x5 = makeint(10L);
  bblt_div_no_check(a0,x5,x6);
  bblt_minus_no_check(x6,x7);
  x8 = makeint(10L);
  bblt_mult_no_check(x7,x8,x9);
  bblt_add_no_check(a0,x9,x10);
  bblt_minus_no_check(x10,x11);
  x12 = makeint(48L);
  bblt_add_no_check(x12,x11,x13);
  allocp[0] = a2;
  allocp[1] = x13;
  x14 = makecons(&allocp[0]);
  a0 = x7;
  a1 = x4;
  a2 = x14;
  allocp += 2;
  execute(int__chars_3_0);
  goto int__chars_3_ext_interrupt;
 int__chars_3_4:
  gblt_not_eq(a0,x2,int__chars_3_interrupt);
  allocp[0] = x4 = makeref(&allocp[0]);
  allocp[1] = makeint(45L);
  x3 = makecons(&allocp[0]);
  allocp += 2;
  unify_value(a1, x3);
  bblt_minus_no_check(a0,x5);
  a0 = x5;
  a1 = x4;
  execute(int__chars_3_0);
  goto int__chars_3_ext_interrupt;
 int__chars_3_5:
  if (!isref(a0)) goto int__chars_3_interrupt;
  deref_and_jump(a0,int__chars_3_1);
  *reasonp++ =  a0;
  goto int__chars_3_interrupt;
 int__chars_3_ext_interrupt:
  reasonp = 0l;
 int__chars_3_interrupt:
  toppred = &predicate_unparser_xint__chars_3;
  goto interrupt_3;
 }

 unparse__float_3_top: {
  q x0, x1;
  a2 = qp->args[2];
  qp = qp->next;
 unparse__float_3_clear_reason:
  reasonp = reasons;
 unparse__float_3_0:
  allocp[0] = x0 = makeref(&allocp[0]);
  generic_arg[0] = x0;
  call_generic(a0, functor_print_1, 1);
  allocp[0] = a2;
  allocp[1] = x0;
  x1 = makecons(&allocp[0]);
  allocp += 2;
  unify_value(a1, x1);
  proceed();
 unparse__float_3_ext_interrupt:
  reasonp = 0l;
 unparse__float_3_interrupt:
  toppred = &predicate_unparser_xunparse__float_3;
  goto interrupt_3;
 }

 unparse__cons_5_top: {
  q x0, x1, x2, x3, x4;
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  qp = qp->next;
 unparse__cons_5_clear_reason:
  reasonp = reasons;
 unparse__cons_5_0:
 unparse__cons_5_1:
  switch (ptagof(a0)) {
 case CONS:
  allocp[0] = x1 = makeref(&allocp[0]);
  allocp[1] = makeint(91L);
  x0 = makecons(&allocp[0]);
  allocp += 2;
  unify_value(a3, x0);
  allocp[0] = a4;
  allocp[1] = makeint(93L);
  x2 = makecons(&allocp[0]);
  allocp[2] = (q)qp;
  allocp[3] = (q)(&predicate_unparser_xunparse__cdr_5);
  x3 = cdr_of(a0);
  allocp[4] = x3;
  allocp[5] = a1;
  allocp[6] = a2;
  allocp[7] = x4 = makeref(&allocp[7]);
  allocp[8] = x2;
  a0 = car_of(a0);
  a3 = x1;
  a4 = x4;
  qp = (struct goalrec*)&allocp[2];
  allocp += 9;
  execute(unparse__comma__list__elem_5_0);
  goto unparse__comma__list__elem_5_ext_interrupt;
 case VARREF:
  deref_and_jump(a0,unparse__cons_5_1);
  *reasonp++ =  a0;
  goto unparse__cons_5_interrupt;
  };
  goto unparse__cons_5_interrupt;
 unparse__cons_5_ext_interrupt:
  reasonp = 0l;
 unparse__cons_5_interrupt:
  toppred = &predicate_unparser_xunparse__cons_5;
  goto interrupt_5;
 }

 unparse__cdr_5_top: {
  q x0, x1, x2, x3;
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  qp = qp->next;
 unparse__cdr_5_clear_reason:
  reasonp = reasons;
 unparse__cdr_5_0:
 unparse__cdr_5_1:
  switch (ptagof(a0)) {
 case CONS:
  allocp[0] = x1 = makeref(&allocp[0]);
  allocp[1] = makeint(44L);
  x0 = makecons(&allocp[0]);
  allocp += 2;
  unify_value(a3, x0);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_unparser_xunparse__cdr_5);
  x2 = cdr_of(a0);
  allocp[2] = x2;
  allocp[3] = a1;
  allocp[4] = a2;
  allocp[5] = x3 = makeref(&allocp[5]);
  allocp[6] = a4;
  a0 = car_of(a0);
  a3 = x1;
  a4 = x3;
  qp = (struct goalrec*)&allocp[0];
  allocp += 7;
  execute(unparse__comma__list__elem_5_0);
  goto unparse__comma__list__elem_5_ext_interrupt;
 case ATOMIC:
  if (a0 != NILATOM) goto unparse__cdr_5_2;
  unify(a4, a3);
  proceed();
 case VARREF:
  deref_and_jump(a0,unparse__cdr_5_1);
  *reasonp++ =  a0;
  goto unparse__cdr_5_2;
  };
  goto unparse__cdr_5_2;
 unparse__cdr_5_2:
  otherwise(unparse__cdr_5_interrupt);
  allocp[0] = x1 = makeref(&allocp[0]);
  allocp[1] = makeint(124L);
  x0 = makecons(&allocp[0]);
  allocp += 2;
  unify_value(a3, x0);
  a3 = x1;
  execute(unparse__comma__list__elem_5_0);
  goto unparse__comma__list__elem_5_ext_interrupt;
 unparse__cdr_5_ext_interrupt:
  reasonp = 0l;
 unparse__cdr_5_interrupt:
  goto interrupt_5;
 }

 unwparse__cons_5_top: {
  q x0, x1, x2, x3, x4;
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  qp = qp->next;
 unwparse__cons_5_clear_reason:
  reasonp = reasons;
 unwparse__cons_5_0:
 unwparse__cons_5_1:
  switch (ptagof(a0)) {
 case CONS:
  allocp[0] = x1 = makeref(&allocp[0]);
  allocp[1] = makeint(91L);
  x0 = makecons(&allocp[0]);
  allocp += 2;
  unify_value(a3, x0);
  allocp[0] = a4;
  allocp[1] = makeint(93L);
  x2 = makecons(&allocp[0]);
  allocp[2] = (q)qp;
  allocp[3] = (q)(&predicate_unparser_xunwparse__cdr_5);
  x3 = cdr_of(a0);
  allocp[4] = x3;
  allocp[5] = a1;
  allocp[6] = a2;
  allocp[7] = x4 = makeref(&allocp[7]);
  allocp[8] = x2;
  a0 = car_of(a0);
  a3 = x1;
  a4 = x4;
  qp = (struct goalrec*)&allocp[2];
  allocp += 9;
  execute(unparse__comma__list__elem_5_0);
  goto unparse__comma__list__elem_5_ext_interrupt;
 case VARREF:
  deref_and_jump(a0,unwparse__cons_5_1);
  *reasonp++ =  a0;
  goto unwparse__cons_5_interrupt;
  };
  goto unwparse__cons_5_interrupt;
 unwparse__cons_5_ext_interrupt:
  reasonp = 0l;
 unwparse__cons_5_interrupt:
  toppred = &predicate_unparser_xunwparse__cons_5;
  goto interrupt_5;
 }

 unwparse__cdr_5_top: {
  q x0, x1, x2, x3, x4;
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  qp = qp->next;
 unwparse__cdr_5_clear_reason:
  reasonp = reasons;
 unwparse__cdr_5_0:
 unwparse__cdr_5_1:
  switch (ptagof(a0)) {
 case FUNCTOR:
  switch (symval(functor_of(a0))) {
 case functor_list_1:
  x0 = arg(a0, 0);
 unwparse__cdr_5_2:
  switch (ptagof(x0)) {
 case CONS:
  allocp[0] = x2 = makeref(&allocp[0]);
  allocp[1] = makeint(44L);
  x1 = makecons(&allocp[0]);
  allocp += 2;
  unify_value(a3, x1);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_unparser_xunwparse__cdr_5);
  x3 = cdr_of(x0);
  allocp[2] = x3;
  allocp[3] = a1;
  allocp[4] = a2;
  allocp[5] = x4 = makeref(&allocp[5]);
  allocp[6] = a4;
  a0 = car_of(x0);
  a3 = x2;
  a4 = x4;
  qp = (struct goalrec*)&allocp[0];
  allocp += 7;
  execute(unparse__comma__list__elem_5_0);
  goto unparse__comma__list__elem_5_ext_interrupt;
 case VARREF:
  deref_and_jump(x0,unwparse__cdr_5_2);
  *reasonp++ =  x0;
  goto unwparse__cdr_5_4;
  };
  goto unwparse__cdr_5_4;
 case functor_atom_1:
  x0 = arg(a0, 0);
 unwparse__cdr_5_3:
  switch (ptagof(x0)) {
 case ATOMIC:
  if (x0 != NILATOM) goto unwparse__cdr_5_4;
  unify(a4, a3);
  proceed();
 case VARREF:
  deref_and_jump(x0,unwparse__cdr_5_3);
  *reasonp++ =  x0;
  goto unwparse__cdr_5_4;
  };
  goto unwparse__cdr_5_4;
  };
  goto unwparse__cdr_5_4;
 case VARREF:
  deref_and_jump(a0,unwparse__cdr_5_1);
  *reasonp++ =  a0;
  goto unwparse__cdr_5_4;
  };
  goto unwparse__cdr_5_4;
 unwparse__cdr_5_4:
  otherwise(unwparse__cdr_5_interrupt);
  allocp[0] = x1 = makeref(&allocp[0]);
  allocp[1] = makeint(124L);
  x0 = makecons(&allocp[0]);
  allocp += 2;
  unify_value(a3, x0);
  a3 = x1;
  execute(unparse__comma__list__elem_5_0);
  goto unparse__comma__list__elem_5_ext_interrupt;
 unwparse__cdr_5_ext_interrupt:
  reasonp = 0l;
 unwparse__cdr_5_interrupt:
  goto interrupt_5;
 }

 unparse__vector_5_top: {
  q x0, x1, x2, x3, x4, x5, x6;
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  qp = qp->next;
 unparse__vector_5_clear_reason:
  reasonp = reasons;
 unparse__vector_5_0:
 unparse__vector_5_1:
  switch (ptagof(a0)) {
 case FUNCTOR:
 unparse__vector_5_2:
  if (!isgobj(a0)) goto unparse__vector_5_interrupt;
  if (!isclass(a0,vector)) goto unparse__vector_5_interrupt;
  gblt_size_of_vector(a0,x0,unparse__vector_5_interrupt);
  x1 = makeint(1L);
  bblt_sub_no_check(x0,x1,x2);
  allocp[0] = x4 = makeref(&allocp[0]);
  allocp[1] = makeint(123L);
  x3 = makecons(&allocp[0]);
  allocp += 2;
  unify_value(a3, x3);
  allocp[0] = a4;
  allocp[1] = makeint(125L);
  x5 = makecons(&allocp[0]);
  x6 = a0;
  a3 = a1;
  a4 = a2;
  a5 = x4;
  a6 = x5;
  a2 = x6;
  a1 = x2;
  a0 = makeint(0L);
  allocp += 2;
  execute(unparse__vecelems_7_0);
  goto unparse__vecelems_7_ext_interrupt;
 case VARREF:
  deref_and_jump(a0,unparse__vector_5_1);
  *reasonp++ =  a0;
  goto unparse__vector_5_interrupt;
  };
  goto unparse__vector_5_interrupt;
 unparse__vector_5_ext_interrupt:
  reasonp = 0l;
 unparse__vector_5_interrupt:
  toppred = &predicate_unparser_xunparse__vector_5;
  goto interrupt_5;
 }

 unparse__vecelems_7_top: {
  q x0, x1, x2, x3, x4;
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  a6 = qp->args[6];
  qp = qp->next;
 unparse__vecelems_7_clear_reason:
  reasonp = reasons;
 unparse__vecelems_7_0:
 unparse__vecelems_7_1:
  if (!isint(a1)) goto unparse__vecelems_7_4;
 unparse__vecelems_7_2:
  if (!isint(a0)) goto unparse__vecelems_7_3;
  gblt_less(a1,a0,unparse__vecelems_7_5);
  unify(a6, a5);
  proceed();
 unparse__vecelems_7_3:
  if (!isref(a0)) goto unparse__vecelems_7_5;
  deref_and_jump(a0,unparse__vecelems_7_2);
  *reasonp++ =  a0;
  goto unparse__vecelems_7_5;
 unparse__vecelems_7_4:
  if (!isref(a1)) goto unparse__vecelems_7_5;
  deref_and_jump(a1,unparse__vecelems_7_1);
  *reasonp++ =  a1;
  goto unparse__vecelems_7_5;
 unparse__vecelems_7_5:
 unparse__vecelems_7_6:
  switch (ptagof(a2)) {
 case FUNCTOR:
 unparse__vecelems_7_7:
  if (!isgobj(a2)) goto unparse__vecelems_7_interrupt;
  if (!isclass(a2,vector)) goto unparse__vecelems_7_interrupt;
 unparse__vecelems_7_8:
  if (!isint(a0)) goto unparse__vecelems_7_12;
  gblt_element_of_vector(a2,a0,x0,unparse__vecelems_7_interrupt);
 unparse__vecelems_7_9:
  if (!isint(a1)) goto unparse__vecelems_7_11;
  gblt_eq(a1,a0,unparse__vecelems_7_10);
  a0 = x0;
  a1 = a3;
  a2 = a4;
  a3 = a5;
  a4 = a6;
  execute(unparse__comma__list__elem_5_clear_reason);
  goto unparse__comma__list__elem_5_ext_interrupt;
 unparse__vecelems_7_10:
  gblt_greater(a1,a0,unparse__vecelems_7_interrupt);
  x1 = makeint(1L);
  bblt_add_no_check(a0,x1,x2);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_unparser_xunparse__vecelems_7);
  allocp[2] = x2;
  allocp[3] = a1;
  allocp[4] = a2;
  allocp[5] = a3;
  allocp[6] = a4;
  allocp[7] = x3 = makeref(&allocp[7]);
  allocp[8] = a6;
  allocp[9] = x3;
  allocp[10] = makeint(44L);
  x4 = makecons(&allocp[9]);
  a0 = x0;
  a1 = a3;
  a2 = a4;
  a3 = a5;
  a4 = x4;
  qp = (struct goalrec*)&allocp[0];
  allocp += 11;
  execute(unparse__comma__list__elem_5_clear_reason);
  goto unparse__comma__list__elem_5_ext_interrupt;
 unparse__vecelems_7_11:
  if (!isref(a1)) goto unparse__vecelems_7_interrupt;
  deref_and_jump(a1,unparse__vecelems_7_9);
  *reasonp++ =  a1;
  goto unparse__vecelems_7_interrupt;
 unparse__vecelems_7_12:
  if (!isref(a0)) goto unparse__vecelems_7_interrupt;
  deref_and_jump(a0,unparse__vecelems_7_8);
  *reasonp++ =  a0;
  goto unparse__vecelems_7_interrupt;
 case VARREF:
  deref_and_jump(a2,unparse__vecelems_7_6);
  *reasonp++ =  a2;
  goto unparse__vecelems_7_interrupt;
  };
  goto unparse__vecelems_7_interrupt;
 unparse__vecelems_7_ext_interrupt:
  reasonp = 0l;
 unparse__vecelems_7_interrupt:
  toppred = &predicate_unparser_xunparse__vecelems_7;
  goto interrupt_7;
 }

 unparse__string_3_top: {
  q x0, x1, x2, x3, x4, x5, x6, x7;
  a2 = qp->args[2];
  qp = qp->next;
 unparse__string_3_clear_reason:
  reasonp = reasons;
 unparse__string_3_0:
 unparse__string_3_1:
  switch (ptagof(a0)) {
 case FUNCTOR:
 unparse__string_3_2:
  if (!isgobj(a0)) goto unparse__string_3_interrupt;
 unparse__string_3_3:
  gblt_is_string(a0,unparse__string_3_interrupt);
  gblt_size_of_string(a0,x0,unparse__string_3_interrupt);
  gblt_element_size_of_string(a0,x1,unparse__string_3_interrupt);
  allocp[0] = a2;
  allocp[1] = x3 = makeref(&allocp[1]);
  x2 = makecons(&allocp[0]);
  allocp += 2;
  unify_value(a1, x2);
  allocp[0] = x5 = makeref(&allocp[0]);
  allocp[1] = makeint(34L);
  x4 = makecons(&allocp[0]);
  generic_arg[0] = x4;
  x6 = makeint(8L);
  generic_arg[1] = x6;
  new_generic(string_g_new, 2, x7, 2);
  unify(x3, x7);
  a1 = x0;
  a2 = a0;
  a3 = x5;
  a4 = makecons(cons_const_2);
  a0 = makeint(0L);
  execute(str__chars_5_0);
  goto str__chars_5_ext_interrupt;
 case VARREF:
  deref_and_jump(a0,unparse__string_3_1);
  *reasonp++ =  a0;
  goto unparse__string_3_interrupt;
  };
  goto unparse__string_3_interrupt;
 unparse__string_3_ext_interrupt:
  reasonp = 0l;
 unparse__string_3_interrupt:
  toppred = &predicate_unparser_xunparse__string_3;
  goto interrupt_3;
 }

 str__chars_5_top: {
  q x0, x1, x2, x3;
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  qp = qp->next;
 str__chars_5_clear_reason:
  reasonp = reasons;
 str__chars_5_0:
 str__chars_5_1:
  if (!isint(a0)) goto str__chars_5_8;
 str__chars_5_2:
  if (!isint(a1)) goto str__chars_5_7;
  gblt_eq(a0,a1,str__chars_5_3);
  unify(a4, a3);
  proceed();
 str__chars_5_3:
  gblt_not_eq(a0,a1,str__chars_5_interrupt);
 str__chars_5_4:
  switch (ptagof(a2)) {
 case FUNCTOR:
 str__chars_5_5:
  if (!isgobj(a2)) goto str__chars_5_interrupt;
 str__chars_5_6:
  gblt_is_string(a2,str__chars_5_interrupt);
  gblt_element_of_string(a2,a0,x0,str__chars_5_interrupt);
  x2 = makeint(1L);
  gblt_add(a0,x2,x1,str__chars_5_interrupt);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_unparser_xstr__chars_5);
  allocp[2] = x1;
  allocp[3] = a1;
  allocp[4] = a2;
  allocp[5] = x3 = makeref(&allocp[5]);
  allocp[6] = a4;
  a0 = x0;
  a1 = a3;
  a2 = x3;
  qp = (struct goalrec*)&allocp[0];
  allocp += 7;
  execute(str__elem__chars_3_0);
  goto str__elem__chars_3_ext_interrupt;
 case VARREF:
  deref_and_jump(a2,str__chars_5_4);
  *reasonp++ =  a2;
  goto str__chars_5_interrupt;
  };
  goto str__chars_5_interrupt;
 str__chars_5_7:
  if (!isref(a1)) goto str__chars_5_interrupt;
  deref_and_jump(a1,str__chars_5_2);
  *reasonp++ =  a1;
  goto str__chars_5_interrupt;
 str__chars_5_8:
  if (!isref(a0)) goto str__chars_5_interrupt;
  deref_and_jump(a0,str__chars_5_1);
  *reasonp++ =  a0;
  goto str__chars_5_interrupt;
 str__chars_5_ext_interrupt:
  reasonp = 0l;
 str__chars_5_interrupt:
  toppred = &predicate_unparser_xstr__chars_5;
  goto interrupt_5;
 }

 str__elem__chars_3_top: {
  q x0, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13, x14, x15, x16, x17, x18, x19, x20, x21, x22;
  a2 = qp->args[2];
  qp = qp->next;
 str__elem__chars_3_clear_reason:
  reasonp = reasons;
 str__elem__chars_3_0:
 str__elem__chars_3_1:
  if (!isint(a0)) goto str__elem__chars_3_4;
  x0 = makeint(32L);
  gblt_greater_or_eq(a0,x0,str__elem__chars_3_2);
  allocp[0] = a2;
  allocp[1] = a0;
  x1 = makecons(&allocp[0]);
  a2 = x1;
  allocp += 2;
  execute(ins__backslash_3_0);
  goto ins__backslash_3_ext_interrupt;
 str__elem__chars_3_2:
  x1 = makeint(32L);
  gblt_less(a0,x1,str__elem__chars_3_interrupt);
  x2 = makeint(10L);
  gblt_eq(a0,x2,str__elem__chars_3_3);
  allocp[0] = x4 = makeref(&allocp[0]);
  allocp[1] = makeint(110L);
  x3 = makecons(&allocp[0]);
  allocp[2] = x3;
  allocp[3] = makeint(92L);
  x5 = makecons(&allocp[2]);
  allocp += 4;
  unify_value(a1, x5);
  unify(a2, x4);
  proceed();
 str__elem__chars_3_3:
  x3 = makeint(10L);
  gblt_not_eq(a0,x3,str__elem__chars_3_interrupt);
  x4 = makeint(64L);
  bblt_div_no_check(a0,x4,x5);
  x6 = makeint(48L);
  bblt_add_no_check(x6,x5,x7);
  x8 = makeint(8L);
  bblt_div_no_check(a0,x8,x9);
  x10 = makeint(7L);
  bblt_and_no_check(x9,x10,x11);
  x12 = makeint(48L);
  bblt_add_no_check(x12,x11,x13);
  x14 = makeint(7L);
  bblt_and_no_check(a0,x14,x15);
  x16 = makeint(48L);
  bblt_add_no_check(x16,x15,x17);
  allocp[0] = x19 = makeref(&allocp[0]);
  allocp[1] = x17;
  x18 = makecons(&allocp[0]);
  allocp[2] = x18;
  allocp[3] = x13;
  x20 = makecons(&allocp[2]);
  allocp[4] = x20;
  allocp[5] = x7;
  x21 = makecons(&allocp[4]);
  allocp[6] = x21;
  allocp[7] = makeint(92L);
  x22 = makecons(&allocp[6]);
  allocp += 8;
  unify_value(a1, x22);
  unify(a2, x19);
  proceed();
 str__elem__chars_3_4:
  if (!isref(a0)) goto str__elem__chars_3_interrupt;
  deref_and_jump(a0,str__elem__chars_3_1);
  *reasonp++ =  a0;
  goto str__elem__chars_3_interrupt;
 str__elem__chars_3_ext_interrupt:
  reasonp = 0l;
 str__elem__chars_3_interrupt:
  toppred = &predicate_unparser_xstr__elem__chars_3;
  goto interrupt_3;
 }

 ins__backslash_3_top: {
  q x0, x1;
  a2 = qp->args[2];
  qp = qp->next;
 ins__backslash_3_clear_reason:
  reasonp = reasons;
 ins__backslash_3_0:
 ins__backslash_3_1:
  if (!isint(a0)) goto ins__backslash_3_2;
  x0 = makeint(34L);
  gblt_not_eq(a0,x0,ins__backslash_3_3);
  x1 = makeint(92L);
  gblt_not_eq(a0,x1,ins__backslash_3_3);
  unify(a2, a1);
  proceed();
 ins__backslash_3_2:
  if (!isref(a0)) goto ins__backslash_3_3;
  deref_and_jump(a0,ins__backslash_3_1);
  *reasonp++ =  a0;
  goto ins__backslash_3_3;
 ins__backslash_3_3:
  otherwise(ins__backslash_3_interrupt);
  allocp[0] = a2;
  allocp[1] = makeint(92L);
  x0 = makecons(&allocp[0]);
  allocp += 2;
  unify_value(a1, x0);
  proceed();
 ins__backslash_3_ext_interrupt:
  reasonp = 0l;
 ins__backslash_3_interrupt:
  toppred = &predicate_unparser_xins__backslash_3;
  goto interrupt_3;
 }

 unparse__general_6_top: {
  q x0, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10;
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  qp = qp->next;
 unparse__general_6_clear_reason:
  reasonp = reasons;
 unparse__general_6_0:
 unparse__general_6_1:
  if (isref(a0)) goto unparse__general_6_4;
  gblt_pfunctor(a0,x0,unparse__general_6_interrupt);
  gblt_arity(a0,x1,unparse__general_6_interrupt);
 unparse__general_6_2:
  switch (ptagof(a1)) {
 case FUNCTOR:
  if (functor_of(a1) != makesym(functor_s_2)) goto unparse__general_6_interrupt;
  x2 = arg(a1, 0);
 unparse__general_6_3:
  switch (ptagof(x2)) {
 case FUNCTOR:
  if (functor_of(x2) != makesym(functor_ops_3)) goto unparse__general_6_interrupt;
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_unparser_xins__if__greater__than_5);
  allocp[2] = x3 = makeref(&allocp[2]);
  allocp[3] = a2;
  allocp[4] = makeint(41L);
  allocp[5] = x4 = makeref(&allocp[5]);
  allocp[6] = a5;
  allocp[7] = (q)(struct goalrec*)&allocp[0];
  allocp[8] = (q)(&predicate_unparser_xunparse__with__op_10);
  allocp[9] = x1;
  allocp[10] = x0;
  allocp[11] = a0;
  allocp[12] = a1;
  allocp[13] = x5 = makeref(&allocp[13]);
  allocp[14] = x6 = makeref(&allocp[14]);
  allocp[15] = x7 = makeref(&allocp[15]);
  allocp[16] = a3;
  allocp[17] = x8 = makeref(&allocp[17]);
  allocp[18] = x4;
  allocp[19] = (q)(struct goalrec*)&allocp[7];
  allocp[20] = (q)(&predicate_unparser_xins__if__greater__than_5);
  allocp[21] = x3;
  allocp[22] = a2;
  allocp[23] = makeint(40L);
  allocp[24] = a4;
  allocp[25] = x8;
  allocp[26] = (q)(struct goalrec*)&allocp[19];
  allocp[27] = (q)(&predicate_unparser_xmax__op_4);
  allocp[28] = x5;
  allocp[29] = x6;
  allocp[30] = x7;
  allocp[31] = x3;
  allocp[32] = (q)(struct goalrec*)&allocp[26];
  allocp[33] = (q)(&predicate_unparser_xfind__operator_3);
  x9 = arg(x2, 2);
  allocp[34] = x9;
  allocp[35] = x0;
  allocp[36] = x7;
  allocp[37] = (q)(struct goalrec*)&allocp[32];
  allocp[38] = (q)(&predicate_unparser_xfind__operator_3);
  x10 = arg(x2, 1);
  allocp[39] = x10;
  allocp[40] = x0;
  allocp[41] = x6;
  a0 = arg(x2, 0);
  a1 = x0;
  a2 = x5;
  qp = (struct goalrec*)&allocp[37];
  allocp += 42;
  execute(find__operator_3_0);
  goto find__operator_3_ext_interrupt;
 case VARREF:
  deref_and_jump(x2,unparse__general_6_3);
  *reasonp++ =  x2;
  goto unparse__general_6_interrupt;
  };
  goto unparse__general_6_interrupt;
 case VARREF:
  deref_and_jump(a1,unparse__general_6_2);
  *reasonp++ =  a1;
  goto unparse__general_6_interrupt;
  };
  goto unparse__general_6_interrupt;
 unparse__general_6_4:
  deref_and_jump(a0,unparse__general_6_1);
  *reasonp++ =  a0;
  goto unparse__general_6_interrupt;
 unparse__general_6_ext_interrupt:
  reasonp = 0l;
 unparse__general_6_interrupt:
  toppred = &predicate_unparser_xunparse__general_6;
  goto interrupt_6;
 }

 ins__if__greater__than_5_top: {
  q x0;
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  qp = qp->next;
 ins__if__greater__than_5_clear_reason:
  reasonp = reasons;
 ins__if__greater__than_5_0:
 ins__if__greater__than_5_1:
  if (!isint(a0)) goto ins__if__greater__than_5_5;
 ins__if__greater__than_5_2:
  if (!isint(a1)) goto ins__if__greater__than_5_4;
  gblt_greater(a0,a1,ins__if__greater__than_5_3);
  allocp[0] = a4;
  allocp[1] = a2;
  x0 = makecons(&allocp[0]);
  allocp += 2;
  unify_value(a3, x0);
  proceed();
 ins__if__greater__than_5_3:
  gblt_less_or_eq(a0,a1,ins__if__greater__than_5_interrupt);
  unify(a4, a3);
  proceed();
 ins__if__greater__than_5_4:
  if (!isref(a1)) goto ins__if__greater__than_5_interrupt;
  deref_and_jump(a1,ins__if__greater__than_5_2);
  *reasonp++ =  a1;
  goto ins__if__greater__than_5_interrupt;
 ins__if__greater__than_5_5:
  if (!isref(a0)) goto ins__if__greater__than_5_interrupt;
  deref_and_jump(a0,ins__if__greater__than_5_1);
  *reasonp++ =  a0;
  goto ins__if__greater__than_5_interrupt;
 ins__if__greater__than_5_ext_interrupt:
  reasonp = 0l;
 ins__if__greater__than_5_interrupt:
  goto interrupt_5;
 }

 unparse__with__op_10_top: {
  q x0, x1, x2, x3, x4, x5, x6, x7, x8;
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  a6 = qp->args[6];
  a7 = qp->args[7];
  a8 = qp->args[8];
  a9 = qp->args[9];
  qp = qp->next;
 unparse__with__op_10_clear_reason:
  reasonp = reasons;
 unparse__with__op_10_0:
 unparse__with__op_10_1:
  switch (ptagof(a0)) {
 case ATOMIC:
 unparse__with__op_10_2:
  switch ((unsigned long)a0) {
 case (unsigned long) makeint(0L):
  a0 = a1;
  a1 = a8;
  a2 = a9;
  execute(unparse__atom_3_0);
  goto unparse__atom_3_ext_interrupt;
 case (unsigned long) makeint(1L):
 unparse__with__op_10_3:
  if (isref(a2)) goto unparse__with__op_10_7;
  x1 = makeint(1L);
  gblt_arg(x1,a2,x0,unparse__with__op_10_11);
 unparse__with__op_10_4:
  switch (ptagof(a4)) {
 case FUNCTOR:
  switch (symval(functor_of(a4))) {
 case functor_fx_1:
  x2 = arg(a4, 0);
  x3 = makeint(1L);
  bblt_sub_no_check_y(x2,x3,x4);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_unparser_xcall__unparser_6);
  allocp[2] = a7;
  allocp[3] = x0;
  allocp[4] = a3;
  allocp[5] = x4;
  allocp[6] = x5 = makeref(&allocp[6]);
  allocp[7] = a9;
  allocp[8] = x5;
  allocp[9] = makeint(0L);
  x6 = makecons(&allocp[8]);
  a0 = a1;
  a1 = a8;
  a2 = x6;
  qp = (struct goalrec*)&allocp[0];
  allocp += 10;
  execute(unparse__atom_3_0);
  goto unparse__atom_3_ext_interrupt;
 case functor_fy_1:
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_unparser_xcall__unparser_6);
  allocp[2] = a7;
  allocp[3] = x0;
  allocp[4] = a3;
  x2 = arg(a4, 0);
  allocp[5] = x2;
  allocp[6] = x3 = makeref(&allocp[6]);
  allocp[7] = a9;
  allocp[8] = x3;
  allocp[9] = makeint(0L);
  x4 = makecons(&allocp[8]);
  a0 = a1;
  a1 = a8;
  a2 = x4;
  qp = (struct goalrec*)&allocp[0];
  allocp += 10;
  execute(unparse__atom_3_0);
  goto unparse__atom_3_ext_interrupt;
  };
  goto unparse__with__op_10_5;
 case VARREF:
  deref_and_jump(a4,unparse__with__op_10_4);
  *reasonp++ =  a4;
  goto unparse__with__op_10_5;
  };
  goto unparse__with__op_10_5;
 unparse__with__op_10_5:
 unparse__with__op_10_6:
  switch (ptagof(a6)) {
 case FUNCTOR:
  switch (symval(functor_of(a6))) {
 case functor_xf_1:
  x2 = arg(a6, 0);
  x3 = makeint(1L);
  bblt_sub_no_check_y(x2,x3,x4);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_unparser_xunparse__atom_3);
  allocp[2] = a1;
  allocp[3] = x5 = makeref(&allocp[3]);
  allocp[4] = a9;
  a0 = a7;
  a1 = x0;
  a2 = a3;
  a3 = x4;
  a4 = a8;
  a5 = x5;
  qp = (struct goalrec*)&allocp[0];
  allocp += 5;
  execute(call__unparser_6_clear_reason);
  goto call__unparser_6_ext_interrupt;
 case functor_yf_1:
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_unparser_xunparse__atom_3);
  allocp[2] = a1;
  allocp[3] = x2 = makeref(&allocp[3]);
  allocp[4] = a9;
  a0 = a7;
  a1 = x0;
  a2 = a3;
  a3 = arg(a6, 0);
  a4 = a8;
  a5 = x2;
  qp = (struct goalrec*)&allocp[0];
  allocp += 5;
  execute(call__unparser_6_clear_reason);
  goto call__unparser_6_ext_interrupt;
  };
  goto unparse__with__op_10_11;
 case VARREF:
  deref_and_jump(a6,unparse__with__op_10_6);
  *reasonp++ =  a6;
  goto unparse__with__op_10_11;
  };
  goto unparse__with__op_10_11;
 unparse__with__op_10_7:
  deref_and_jump(a2,unparse__with__op_10_3);
  *reasonp++ =  a2;
  goto unparse__with__op_10_11;
 case (unsigned long) makeint(2L):
 unparse__with__op_10_8:
  if (isref(a2)) goto unparse__with__op_10_10;
  x1 = makeint(1L);
  gblt_arg(x1,a2,x0,unparse__with__op_10_11);
  x3 = makeint(2L);
  gblt_arg(x3,a2,x2,unparse__with__op_10_11);
 unparse__with__op_10_9:
  switch (ptagof(a5)) {
 case FUNCTOR:
  switch (symval(functor_of(a5))) {
 case functor_xfx_1:
  x4 = arg(a5, 0);
  x5 = makeint(1L);
  bblt_sub_no_check_y(x4,x5,x6);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_unparser_xcall__unparser_6);
  allocp[2] = a7;
  allocp[3] = x2;
  allocp[4] = a3;
  allocp[5] = x6;
  allocp[6] = x7 = makeref(&allocp[6]);
  allocp[7] = a9;
  allocp[8] = (q)(struct goalrec*)&allocp[0];
  allocp[9] = (q)(&predicate_unparser_xunparse__bin__op_3);
  allocp[10] = a1;
  allocp[11] = x8 = makeref(&allocp[11]);
  allocp[12] = x7;
  a0 = a7;
  a1 = x0;
  a2 = a3;
  a3 = x6;
  a4 = a8;
  a5 = x8;
  qp = (struct goalrec*)&allocp[8];
  allocp += 13;
  execute(call__unparser_6_0);
  goto call__unparser_6_ext_interrupt;
 case functor_xfy_1:
  x4 = arg(a5, 0);
  x5 = makeint(1L);
  bblt_sub_no_check_y(x4,x5,x6);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_unparser_xcall__unparser_6);
  allocp[2] = a7;
  allocp[3] = x2;
  allocp[4] = a3;
  allocp[5] = x4;
  allocp[6] = x7 = makeref(&allocp[6]);
  allocp[7] = a9;
  allocp[8] = (q)(struct goalrec*)&allocp[0];
  allocp[9] = (q)(&predicate_unparser_xunparse__bin__op_3);
  allocp[10] = a1;
  allocp[11] = x8 = makeref(&allocp[11]);
  allocp[12] = x7;
  a0 = a7;
  a1 = x0;
  a2 = a3;
  a3 = x6;
  a4 = a8;
  a5 = x8;
  qp = (struct goalrec*)&allocp[8];
  allocp += 13;
  execute(call__unparser_6_0);
  goto call__unparser_6_ext_interrupt;
 case functor_yfx_1:
  x4 = arg(a5, 0);
  x5 = makeint(1L);
  bblt_sub_no_check_y(x4,x5,x6);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_unparser_xcall__unparser_6);
  allocp[2] = a7;
  allocp[3] = x2;
  allocp[4] = a3;
  allocp[5] = x6;
  allocp[6] = x7 = makeref(&allocp[6]);
  allocp[7] = a9;
  allocp[8] = (q)(struct goalrec*)&allocp[0];
  allocp[9] = (q)(&predicate_unparser_xunparse__bin__op_3);
  allocp[10] = a1;
  allocp[11] = x8 = makeref(&allocp[11]);
  allocp[12] = x7;
  a0 = a7;
  a1 = x0;
  a2 = a3;
  a3 = x4;
  a4 = a8;
  a5 = x8;
  qp = (struct goalrec*)&allocp[8];
  allocp += 13;
  execute(call__unparser_6_0);
  goto call__unparser_6_ext_interrupt;
  };
  goto unparse__with__op_10_11;
 case VARREF:
  deref_and_jump(a5,unparse__with__op_10_9);
  *reasonp++ =  a5;
  goto unparse__with__op_10_11;
  };
  goto unparse__with__op_10_11;
 unparse__with__op_10_10:
  deref_and_jump(a2,unparse__with__op_10_8);
  *reasonp++ =  a2;
  goto unparse__with__op_10_11;
  };
  goto unparse__with__op_10_11;
 case VARREF:
  deref_and_jump(a0,unparse__with__op_10_1);
  *reasonp++ =  a0;
  goto unparse__with__op_10_11;
  };
  goto unparse__with__op_10_11;
 unparse__with__op_10_11:
  otherwise(unparse__with__op_10_interrupt);
  allocp[0] = a9;
  allocp[1] = makeint(41L);
  x0 = makecons(&allocp[0]);
  allocp[2] = (q)qp;
  allocp[3] = (q)(&predicate_unparser_xunparse__elems_7);
  allocp[4] = makeint(1L);
  allocp[5] = a0;
  allocp[6] = a2;
  allocp[7] = a3;
  allocp[8] = a7;
  allocp[9] = x1 = makeref(&allocp[9]);
  allocp[10] = x0;
  allocp[11] = x1;
  allocp[12] = makeint(40L);
  x2 = makecons(&allocp[11]);
  a0 = a1;
  a1 = a8;
  a2 = x2;
  qp = (struct goalrec*)&allocp[2];
  allocp += 13;
  execute(unparse__atom_3_0);
  goto unparse__atom_3_ext_interrupt;
 unparse__with__op_10_ext_interrupt:
  reasonp = 0l;
 unparse__with__op_10_interrupt:
  goto interrupt_10;
 }

 unparse__elems_7_top: {
  q x0, x1, x2, x3, x4;
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  a6 = qp->args[6];
  qp = qp->next;
 unparse__elems_7_clear_reason:
  reasonp = reasons;
 unparse__elems_7_0:
 unparse__elems_7_1:
  if (!isint(a0)) goto unparse__elems_7_7;
 unparse__elems_7_2:
  if (isref(a2)) goto unparse__elems_7_6;
  gblt_arg(a0,a2,x0,unparse__elems_7_interrupt);
 unparse__elems_7_3:
  if (!isint(a1)) goto unparse__elems_7_5;
  gblt_eq(a0,a1,unparse__elems_7_4);
  a0 = x0;
  a1 = a3;
  a2 = a4;
  a3 = a5;
  a4 = a6;
  execute(unparse__comma__list__elem_5_0);
  goto unparse__comma__list__elem_5_ext_interrupt;
 unparse__elems_7_4:
  gblt_not_eq(a0,a1,unparse__elems_7_interrupt);
  x1 = makeint(1L);
  bblt_add_no_check(a0,x1,x2);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_unparser_xunparse__elems_7);
  allocp[2] = x2;
  allocp[3] = a1;
  allocp[4] = a2;
  allocp[5] = a3;
  allocp[6] = a4;
  allocp[7] = x3 = makeref(&allocp[7]);
  allocp[8] = a6;
  allocp[9] = x3;
  allocp[10] = makeint(44L);
  x4 = makecons(&allocp[9]);
  a0 = x0;
  a1 = a3;
  a2 = a4;
  a3 = a5;
  a4 = x4;
  qp = (struct goalrec*)&allocp[0];
  allocp += 11;
  execute(unparse__comma__list__elem_5_0);
  goto unparse__comma__list__elem_5_ext_interrupt;
 unparse__elems_7_5:
  if (!isref(a1)) goto unparse__elems_7_interrupt;
  deref_and_jump(a1,unparse__elems_7_3);
  *reasonp++ =  a1;
  goto unparse__elems_7_interrupt;
 unparse__elems_7_6:
  deref_and_jump(a2,unparse__elems_7_2);
  *reasonp++ =  a2;
  goto unparse__elems_7_interrupt;
 unparse__elems_7_7:
  if (!isref(a0)) goto unparse__elems_7_interrupt;
  deref_and_jump(a0,unparse__elems_7_1);
  *reasonp++ =  a0;
  goto unparse__elems_7_interrupt;
 unparse__elems_7_ext_interrupt:
  reasonp = 0l;
 unparse__elems_7_interrupt:
  goto interrupt_7;
 }

 unparse__atom_3_top: {
  q x0, x1, x2, x3;
  a2 = qp->args[2];
  qp = qp->next;
 unparse__atom_3_clear_reason:
  reasonp = reasons;
 unparse__atom_3_0:
 unparse__atom_3_1:
  switch (ptagof(a0)) {
 case ATOMIC:
  if (isint(a0)) goto unparse__atom_3_2;
  switch (symval(a0)) {
 case symval(NILATOM):
  allocp[0] = a2;
  allocp[1] = makefunctor(&string_const_1);
  x0 = makecons(&allocp[0]);
  allocp += 2;
  unify_value(a1, x0);
  proceed();
 case symval(makesym(atom__21)):
  allocp[0] = a2;
  allocp[1] = makeint(33L);
  x0 = makecons(&allocp[0]);
  allocp += 2;
  unify_value(a1, x0);
  proceed();
 case symval(PERIODATOM):
  allocp[0] = a2;
  allocp[1] = makefunctor(&string_const_0);
  x0 = makecons(&allocp[0]);
  allocp += 2;
  unify_value(a1, x0);
  proceed();
  };
  goto unparse__atom_3_2;
 case VARREF:
  deref_and_jump(a0,unparse__atom_3_1);
  *reasonp++ =  a0;
  goto unparse__atom_3_2;
  };
  goto unparse__atom_3_2;
 unparse__atom_3_2:
  otherwise(unparse__atom_3_interrupt);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_unparser_xmake__atom__string_4);
  allocp[2] = x0 = makeref(&allocp[2]);
  allocp[3] = x1 = makeref(&allocp[3]);
  allocp[4] = a1;
  allocp[5] = a2;
  allocp[6] = (q)(struct goalrec*)&allocp[0];
  allocp[7] = (q)(&predicate_unparser_xatom__kind_3);
  allocp[8] = x1;
  allocp[9] = x2 = makeref(&allocp[9]);
  allocp[10] = x0;
  allocp[11] = makesym(functor_size_1);
  allocp[12] = x2;
  x3 = makefunctor(&allocp[11]);
  allocp[13] = (q)(struct goalrec*)&allocp[6];
  allocp[14] = (q)(&predicate_generic_xgeneric_2);
  allocp[15] = x3;
  allocp[16] = x1;
  allocp[17] = (q)(struct goalrec*)&allocp[13];
  allocp[18] = (q)(&predicate_atom__table_xget__atom__string_2);
  allocp[19] = a0;
  allocp[20] = x1;
  qp = (struct goalrec*)&allocp[17];
  allocp += 21;
  proceed();
 unparse__atom_3_ext_interrupt:
  reasonp = 0l;
 unparse__atom_3_interrupt:
  toppred = &predicate_unparser_xunparse__atom_3;
  goto interrupt_3;
 }

 atom__kind_3_top: {
  q x0, x1, x2, x3, x4, x5;
  a2 = qp->args[2];
  qp = qp->next;
 atom__kind_3_clear_reason:
  reasonp = reasons;
 atom__kind_3_0:
 atom__kind_3_1:
  if (!isint(a1)) goto atom__kind_3_6;
  x0 = makeint(0L);
  gblt_greater(a1,x0,atom__kind_3_5);
 atom__kind_3_2:
  switch (ptagof(a0)) {
 case FUNCTOR:
 atom__kind_3_3:
  if (!isgobj(a0)) goto atom__kind_3_5;
 atom__kind_3_4:
  gblt_is_string(a0,atom__kind_3_5);
  x2 = makeint(0L);
  gblt_element_of_string(a0,x2,x1,atom__kind_3_5);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_unparser_xatom__kind_2F3_240_4);
  allocp[2] = x3 = makeref(&allocp[2]);
  allocp[3] = a2;
  allocp[4] = a1;
  allocp[5] = a0;
  allocp[6] = (q)(struct goalrec*)&allocp[0];
  allocp[7] = (q)(&predicate_unparser_xfirst__char__kind_2);
  allocp[8] = x4 = makeref(&allocp[8]);
  allocp[9] = x3;
  allocp[10] = (q)(struct goalrec*)&allocp[6];
  allocp[11] = (q)(&predicate_scanner_xchartype_2);
  allocp[12] = x1;
  allocp[13] = x4;
  qp = (struct goalrec*)&allocp[10];
  allocp += 14;
  proceed();
 case VARREF:
  deref_and_jump(a0,atom__kind_3_2);
  *reasonp++ =  a0;
  goto atom__kind_3_5;
  };
  goto atom__kind_3_5;
 atom__kind_3_5:
  if (a1 != makeint(0L)) goto atom__kind_3_interrupt;
  x5 = makesym(atom_qtd);
  unify_value(a2, x5);
  proceed();
 atom__kind_3_6:
  if (!isref(a1)) goto atom__kind_3_interrupt;
  deref_and_jump(a1,atom__kind_3_1);
  *reasonp++ =  a1;
  goto atom__kind_3_interrupt;
 atom__kind_3_ext_interrupt:
  reasonp = 0l;
 atom__kind_3_interrupt:
  goto interrupt_3;
 }

 atom__kind_2F3_240_4_top: {
  q x0, x1, x2;
  a2 = qp->args[2];
  a3 = qp->args[3];
  qp = qp->next;
 atom__kind_2F3_240_4_clear_reason:
  reasonp = reasons;
 atom__kind_2F3_240_4_0:
 atom__kind_2F3_240_4_1:
  switch (ptagof(a0)) {
 case ATOMIC:
  if (a0 != makesym(atom_qtd)) goto atom__kind_2F3_240_4_2;
  x0 = makesym(atom_qtd);
  unify_value(a1, x0);
  proceed();
 case VARREF:
  deref_and_jump(a0,atom__kind_2F3_240_4_1);
  *reasonp++ =  a0;
  goto atom__kind_2F3_240_4_2;
  };
  goto atom__kind_2F3_240_4_2;
 atom__kind_2F3_240_4_2:
  otherwise(atom__kind_2F3_240_4_interrupt);
  x0 = a0;
  x1 = a0;
  x2 = a2;
  a4 = a3;
  a5 = a1;
  a3 = x2;
  a2 = makeint(1L);
  a1 = x1;
  a0 = x0;
  execute(judge__quoted_6_0);
  goto judge__quoted_6_ext_interrupt;
 atom__kind_2F3_240_4_ext_interrupt:
  reasonp = 0l;
 atom__kind_2F3_240_4_interrupt:
  goto interrupt_4;
 }

 first__char__kind_2_top: {
  q x0;
  qp = qp->next;
 first__char__kind_2_clear_reason:
  reasonp = reasons;
 first__char__kind_2_0:
 first__char__kind_2_1:
  switch (ptagof(a0)) {
 case ATOMIC:
 first__char__kind_2_2:
  switch ((unsigned long)a0) {
 case (unsigned long) makeint(0L):
  x0 = makesym(atom_alf);
  unify_value(a1, x0);
  proceed();
 case (unsigned long) makeint(4L):
  x0 = makesym(atom_sym);
  unify_value(a1, x0);
  proceed();
  };
  goto first__char__kind_2_3;
 case VARREF:
  deref_and_jump(a0,first__char__kind_2_1);
  *reasonp++ =  a0;
  goto first__char__kind_2_3;
  };
  goto first__char__kind_2_3;
 first__char__kind_2_3:
  otherwise(first__char__kind_2_interrupt);
  x0 = makesym(atom_qtd);
  unify_value(a1, x0);
  proceed();
 first__char__kind_2_ext_interrupt:
  reasonp = 0l;
 first__char__kind_2_interrupt:
  goto interrupt_2;
 }

 judge__quoted_6_top: {
  q x0, x1, x2, x3, x4;
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  qp = qp->next;
 judge__quoted_6_clear_reason:
  reasonp = reasons;
 judge__quoted_6_0:
 judge__quoted_6_1:
  if (isref(a0)) goto judge__quoted_6_21;
 judge__quoted_6_2:
  if (isref(a1)) goto judge__quoted_6_3;
  gblt_diff(a0,a1,judge__quoted_6_4);
  x0 = makesym(atom_qtd);
  unify_value(a5, x0);
  proceed();
 judge__quoted_6_3:
  deref_and_jump(a1,judge__quoted_6_2);
  *reasonp++ =  a1;
  goto judge__quoted_6_4;
 judge__quoted_6_4:
 judge__quoted_6_5:
  if (isref(a0)) goto judge__quoted_6_20;
  if_not_equal(a0, a1, judge__quoted_6_10);
 judge__quoted_6_6:
  if (!isint(a2)) goto judge__quoted_6_9;
 judge__quoted_6_7:
  if (!isint(a3)) goto judge__quoted_6_8;
  gblt_eq(a2,a3,judge__quoted_6_10);
  unify_value(a5, a0);
  proceed();
 judge__quoted_6_8:
  if (!isref(a3)) goto judge__quoted_6_10;
  deref_and_jump(a3,judge__quoted_6_7);
  *reasonp++ =  a3;
  goto judge__quoted_6_10;
 judge__quoted_6_9:
  if (!isref(a2)) goto judge__quoted_6_10;
  deref_and_jump(a2,judge__quoted_6_6);
  *reasonp++ =  a2;
  goto judge__quoted_6_10;
 judge__quoted_6_10:
 judge__quoted_6_11:
  if (isref(a0)) goto judge__quoted_6_19;
  if_not_equal(a0, a1, judge__quoted_6_interrupt);
 judge__quoted_6_12:
  if (!isint(a2)) goto judge__quoted_6_18;
 judge__quoted_6_13:
  if (!isint(a3)) goto judge__quoted_6_17;
  gblt_less(a2,a3,judge__quoted_6_interrupt);
 judge__quoted_6_14:
  switch (ptagof(a4)) {
 case FUNCTOR:
 judge__quoted_6_15:
  if (!isgobj(a4)) goto judge__quoted_6_interrupt;
 judge__quoted_6_16:
  gblt_is_string(a4,judge__quoted_6_interrupt);
  gblt_element_of_string(a4,a2,x0,judge__quoted_6_interrupt);
  x1 = makeint(1L);
  bblt_add_no_check(a2,x1,x2);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_unparser_xjudge__quoted_6);
  allocp[2] = a0;
  allocp[3] = x3 = makeref(&allocp[3]);
  allocp[4] = x2;
  allocp[5] = a3;
  allocp[6] = a4;
  allocp[7] = a5;
  allocp[8] = (q)(struct goalrec*)&allocp[0];
  allocp[9] = (q)(&predicate_unparser_xchar__kind_2);
  allocp[10] = x4 = makeref(&allocp[10]);
  allocp[11] = x3;
  allocp[12] = (q)(struct goalrec*)&allocp[8];
  allocp[13] = (q)(&predicate_scanner_xchartype_2);
  allocp[14] = x0;
  allocp[15] = x4;
  qp = (struct goalrec*)&allocp[12];
  allocp += 16;
  proceed();
 case VARREF:
  deref_and_jump(a4,judge__quoted_6_14);
  *reasonp++ =  a4;
  goto judge__quoted_6_interrupt;
  };
  goto judge__quoted_6_interrupt;
 judge__quoted_6_17:
  if (!isref(a3)) goto judge__quoted_6_interrupt;
  deref_and_jump(a3,judge__quoted_6_13);
  *reasonp++ =  a3;
  goto judge__quoted_6_interrupt;
 judge__quoted_6_18:
  if (!isref(a2)) goto judge__quoted_6_interrupt;
  deref_and_jump(a2,judge__quoted_6_12);
  *reasonp++ =  a2;
  goto judge__quoted_6_interrupt;
 judge__quoted_6_19:
  deref_and_jump(a0,judge__quoted_6_11);
  *reasonp++ =  a0;
  goto judge__quoted_6_interrupt;
 judge__quoted_6_20:
  deref_and_jump(a0,judge__quoted_6_5);
  *reasonp++ =  a0;
  goto judge__quoted_6_interrupt;
 judge__quoted_6_21:
  deref_and_jump(a0,judge__quoted_6_1);
  *reasonp++ =  a0;
  goto judge__quoted_6_interrupt;
 judge__quoted_6_ext_interrupt:
  reasonp = 0l;
 judge__quoted_6_interrupt:
  toppred = &predicate_unparser_xjudge__quoted_6;
  goto interrupt_6;
 }

 char__kind_2_top: {
  q x0, x1, x2, x3;
  qp = qp->next;
 char__kind_2_clear_reason:
  reasonp = reasons;
 char__kind_2_0:
 char__kind_2_1:
  if (!isint(a0)) goto char__kind_2_4;
  x0 = makeint(4L);
  gblt_less(a0,x0,char__kind_2_2);
  x1 = makesym(atom_alf);
  unify_value(a1, x1);
  proceed();
 char__kind_2_2:
  x1 = makeint(4L);
  gblt_eq(a0,x1,char__kind_2_3);
  x2 = makesym(atom_sym);
  unify_value(a1, x2);
  proceed();
 char__kind_2_3:
  x2 = makeint(4L);
  gblt_greater(a0,x2,char__kind_2_interrupt);
  x3 = makesym(atom_qtd);
  unify_value(a1, x3);
  proceed();
 char__kind_2_4:
  if (!isref(a0)) goto char__kind_2_interrupt;
  deref_and_jump(a0,char__kind_2_1);
  *reasonp++ =  a0;
  goto char__kind_2_interrupt;
 char__kind_2_ext_interrupt:
  reasonp = 0l;
 char__kind_2_interrupt:
  goto interrupt_2;
 }

 make__atom__string_4_top: {
  q x0, x1, x2, x3, x4, x5, x6, x7;
  a2 = qp->args[2];
  a3 = qp->args[3];
  qp = qp->next;
 make__atom__string_4_clear_reason:
  reasonp = reasons;
 make__atom__string_4_0:
 make__atom__string_4_1:
  switch (ptagof(a0)) {
 case ATOMIC:
  if (isint(a0)) goto make__atom__string_4_interrupt;
  switch (symval(a0)) {
 case symval(makesym(atom_alf)):
  allocp[0] = a3;
  allocp[1] = a1;
  x0 = makecons(&allocp[0]);
  allocp += 2;
  unify_value(a2, x0);
  proceed();
 case symval(makesym(atom_sym)):
  allocp[0] = a3;
  allocp[1] = a1;
  x0 = makecons(&allocp[0]);
  allocp += 2;
  unify_value(a2, x0);
  proceed();
 case symval(makesym(atom_qtd)):
 make__atom__string_4_2:
  switch (ptagof(a1)) {
 case FUNCTOR:
 make__atom__string_4_3:
  if (!isgobj(a1)) goto make__atom__string_4_interrupt;
 make__atom__string_4_4:
  gblt_is_string(a1,make__atom__string_4_interrupt);
  gblt_size_of_string(a1,x0,make__atom__string_4_interrupt);
  gblt_element_size_of_string(a1,x1,make__atom__string_4_interrupt);
  allocp[0] = a3;
  allocp[1] = x3 = makeref(&allocp[1]);
  x2 = makecons(&allocp[0]);
  allocp += 2;
  unify_value(a2, x2);
  allocp[0] = x5 = makeref(&allocp[0]);
  allocp[1] = makeint(39L);
  x4 = makecons(&allocp[0]);
  generic_arg[0] = x4;
  x6 = makeint(8L);
  generic_arg[1] = x6;
  new_generic(string_g_new, 2, x7, 2);
  unify(x3, x7);
  a0 = makeint(0L);
  a2 = a1;
  a3 = x5;
  a1 = x0;
  execute(quoted__atom__chars_4_0);
  goto quoted__atom__chars_4_ext_interrupt;
 case VARREF:
  deref_and_jump(a1,make__atom__string_4_2);
  *reasonp++ =  a1;
  goto make__atom__string_4_interrupt;
  };
  goto make__atom__string_4_interrupt;
  };
  goto make__atom__string_4_interrupt;
 case VARREF:
  deref_and_jump(a0,make__atom__string_4_1);
  *reasonp++ =  a0;
  goto make__atom__string_4_interrupt;
  };
  goto make__atom__string_4_interrupt;
 make__atom__string_4_ext_interrupt:
  reasonp = 0l;
 make__atom__string_4_interrupt:
  goto interrupt_4;
 }

 quoted__atom__chars_4_top: {
  q x0, x1, x2, x3, x4;
  a2 = qp->args[2];
  a3 = qp->args[3];
  qp = qp->next;
 quoted__atom__chars_4_clear_reason:
  reasonp = reasons;
 quoted__atom__chars_4_0:
 quoted__atom__chars_4_1:
  if (!isint(a0)) goto quoted__atom__chars_4_8;
 quoted__atom__chars_4_2:
  if (!isint(a1)) goto quoted__atom__chars_4_7;
  gblt_eq(a0,a1,quoted__atom__chars_4_3);
  x0 = makecons(cons_const_3);
  unify_value(a3, x0);
  proceed();
 quoted__atom__chars_4_3:
  gblt_less(a0,a1,quoted__atom__chars_4_interrupt);
 quoted__atom__chars_4_4:
  switch (ptagof(a2)) {
 case FUNCTOR:
 quoted__atom__chars_4_5:
  if (!isgobj(a2)) goto quoted__atom__chars_4_interrupt;
 quoted__atom__chars_4_6:
  gblt_is_string(a2,quoted__atom__chars_4_interrupt);
  gblt_element_of_string(a2,a0,x0,quoted__atom__chars_4_interrupt);
  x1 = makeint(1L);
  bblt_add_no_check(a0,x1,x2);
  allocp[0] = x4 = makeref(&allocp[0]);
  allocp[1] = x0;
  x3 = makecons(&allocp[0]);
  allocp[2] = (q)qp;
  allocp[3] = (q)(&predicate_unparser_xquoted__atom__chars_4);
  allocp[4] = x2;
  allocp[5] = a1;
  allocp[6] = a2;
  allocp[7] = x4;
  a0 = x0;
  a1 = a3;
  a2 = x3;
  qp = (struct goalrec*)&allocp[2];
  allocp += 8;
  execute(quoted__atom__chars_2F4_240_3_0);
  goto quoted__atom__chars_2F4_240_3_ext_interrupt;
 case VARREF:
  deref_and_jump(a2,quoted__atom__chars_4_4);
  *reasonp++ =  a2;
  goto quoted__atom__chars_4_interrupt;
  };
  goto quoted__atom__chars_4_interrupt;
 quoted__atom__chars_4_7:
  if (!isref(a1)) goto quoted__atom__chars_4_interrupt;
  deref_and_jump(a1,quoted__atom__chars_4_2);
  *reasonp++ =  a1;
  goto quoted__atom__chars_4_interrupt;
 quoted__atom__chars_4_8:
  if (!isref(a0)) goto quoted__atom__chars_4_interrupt;
  deref_and_jump(a0,quoted__atom__chars_4_1);
  *reasonp++ =  a0;
  goto quoted__atom__chars_4_interrupt;
 quoted__atom__chars_4_ext_interrupt:
  reasonp = 0l;
 quoted__atom__chars_4_interrupt:
  toppred = &predicate_unparser_xquoted__atom__chars_4;
  goto interrupt_4;
 }

 quoted__atom__chars_2F4_240_3_top: {
  q x0, x1;
  a2 = qp->args[2];
  qp = qp->next;
 quoted__atom__chars_2F4_240_3_clear_reason:
  reasonp = reasons;
 quoted__atom__chars_2F4_240_3_0:
 quoted__atom__chars_2F4_240_3_1:
  if (!isint(a0)) goto quoted__atom__chars_2F4_240_3_2;
  x0 = makeint(39L);
  gblt_not_eq(a0,x0,quoted__atom__chars_2F4_240_3_3);
  x1 = makeint(92L);
  gblt_not_eq(a0,x1,quoted__atom__chars_2F4_240_3_3);
  unify(a1, a2);
  proceed();
 quoted__atom__chars_2F4_240_3_2:
  if (!isref(a0)) goto quoted__atom__chars_2F4_240_3_3;
  deref_and_jump(a0,quoted__atom__chars_2F4_240_3_1);
  *reasonp++ =  a0;
  goto quoted__atom__chars_2F4_240_3_3;
 quoted__atom__chars_2F4_240_3_3:
  otherwise(quoted__atom__chars_2F4_240_3_interrupt);
  allocp[0] = a2;
  allocp[1] = a0;
  x0 = makecons(&allocp[0]);
  allocp += 2;
  unify_value(a1, x0);
  proceed();
 quoted__atom__chars_2F4_240_3_ext_interrupt:
  reasonp = 0l;
 quoted__atom__chars_2F4_240_3_interrupt:
  toppred = &predicate_unparser_xquoted__atom__chars_2F4_240_3;
  goto interrupt_3;
 }

 find__operator_3_top: {
  q x0, x1, x2, x3, x4, x5;
  a2 = qp->args[2];
  qp = qp->next;
 find__operator_3_clear_reason:
  reasonp = reasons;
 find__operator_3_0:
 find__operator_3_1:
  switch (ptagof(a1)) {
 case ATOMIC:
  if (a1 != makesym(atom__7C)) goto find__operator_3_2;
  x0 = makefunctor(funct_const_6);
  unify_value(a2, x0);
  proceed();
 case VARREF:
  deref_and_jump(a1,find__operator_3_1);
  *reasonp++ =  a1;
  goto find__operator_3_2;
  };
  goto find__operator_3_2;
 find__operator_3_2:
 find__operator_3_3:
  switch (ptagof(a0)) {
 case CONS:
  x0 = car_of(a0);
 find__operator_3_4:
  switch (ptagof(x0)) {
 case FUNCTOR:
  if (functor_of(x0) != makesym(functor_op_3)) goto find__operator_3_9;
  x1 = arg(x0, 0);
 find__operator_3_5:
  if (isref(x1)) goto find__operator_3_8;
 find__operator_3_6:
  if (isref(x1)) goto find__operator_3_7;
  if_not_equal(x1, a1, find__operator_3_9);
  allocp[0] = NILATOM;
  x3 = arg(x0, 2);
  allocp[1] = x3;
  x2 = makecons(&allocp[0]);
  allocp[2] = x2;
  x5 = arg(x0, 1);
  allocp[3] = x5;
  x4 = makecons(&allocp[2]);
  allocp[4] = (q)qp;
  allocp[5] = (q)(&predicate_functor__table_x_3D_2E_2E_2);
  allocp[6] = a2;
  allocp[7] = x4;
  qp = (struct goalrec*)&allocp[4];
  allocp += 8;
  proceed();
 find__operator_3_7:
  deref_and_jump(x1,find__operator_3_6);
  *reasonp++ =  x1;
  goto find__operator_3_9;
 find__operator_3_8:
  deref_and_jump(x1,find__operator_3_5);
  *reasonp++ =  x1;
  goto find__operator_3_9;
 case VARREF:
  deref_and_jump(x0,find__operator_3_4);
  *reasonp++ =  x0;
  goto find__operator_3_9;
  };
  goto find__operator_3_9;
 case ATOMIC:
  if (a0 != NILATOM) goto find__operator_3_9;
  x0 = makefunctor(funct_const_7);
  unify_value(a2, x0);
  proceed();
 case VARREF:
  deref_and_jump(a0,find__operator_3_3);
  *reasonp++ =  a0;
  goto find__operator_3_9;
  };
  goto find__operator_3_9;
 find__operator_3_9:
  otherwise(find__operator_3_interrupt);
 find__operator_3_10:
  switch (ptagof(a0)) {
 case CONS:
  a0 = cdr_of(a0);
  execute(find__operator_3_0);
  goto find__operator_3_ext_interrupt;
 case VARREF:
  deref_and_jump(a0,find__operator_3_10);
  *reasonp++ =  a0;
  goto find__operator_3_interrupt;
  };
  goto find__operator_3_interrupt;
 find__operator_3_ext_interrupt:
  reasonp = 0l;
 find__operator_3_interrupt:
  toppred = &predicate_unparser_xfind__operator_3;
  goto interrupt_3;
 }

 max__op_4_top: {
  q x0, x1, x2, x3, x4, x5, x6;
  a2 = qp->args[2];
  a3 = qp->args[3];
  qp = qp->next;
 max__op_4_clear_reason:
  reasonp = reasons;
 max__op_4_0:
 max__op_4_1:
  if (isref(a0)) goto max__op_4_6;
  x1 = makeint(1L);
  gblt_arg(x1,a0,x0,max__op_4_interrupt);
 max__op_4_2:
  if (isref(a1)) goto max__op_4_5;
  x3 = makeint(1L);
  gblt_arg(x3,a1,x2,max__op_4_interrupt);
 max__op_4_3:
  if (isref(a2)) goto max__op_4_4;
  x5 = makeint(1L);
  gblt_arg(x5,a2,x4,max__op_4_interrupt);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_unparser_xmax_3);
  allocp[2] = x6 = makeref(&allocp[2]);
  allocp[3] = x4;
  allocp[4] = a3;
  a0 = x0;
  a1 = x2;
  a2 = x6;
  qp = (struct goalrec*)&allocp[0];
  allocp += 5;
  execute(max_3_0);
  goto max_3_ext_interrupt;
 max__op_4_4:
  deref_and_jump(a2,max__op_4_3);
  *reasonp++ =  a2;
  goto max__op_4_interrupt;
 max__op_4_5:
  deref_and_jump(a1,max__op_4_2);
  *reasonp++ =  a1;
  goto max__op_4_interrupt;
 max__op_4_6:
  deref_and_jump(a0,max__op_4_1);
  *reasonp++ =  a0;
  goto max__op_4_interrupt;
 max__op_4_ext_interrupt:
  reasonp = 0l;
 max__op_4_interrupt:
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
  gblt_less(a0,a1,max_3_interrupt);
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
  toppred = &predicate_unparser_xmax_3;
  goto interrupt_3;
 }

 unparse__bin__op_3_top: {
  q x0;
  a2 = qp->args[2];
  qp = qp->next;
 unparse__bin__op_3_clear_reason:
  reasonp = reasons;
 unparse__bin__op_3_0:
 unparse__bin__op_3_1:
  switch (ptagof(a0)) {
 case ATOMIC:
  if (isint(a0)) goto unparse__bin__op_3_2;
  switch (symval(a0)) {
 case symval(makesym(atom__2C)):
  allocp[0] = a2;
  allocp[1] = makeint(44L);
  x0 = makecons(&allocp[0]);
  allocp += 2;
  unify_value(a1, x0);
  proceed();
 case symval(makesym(atom__3B)):
  allocp[0] = a2;
  allocp[1] = makeint(59L);
  x0 = makecons(&allocp[0]);
  allocp += 2;
  unify_value(a1, x0);
  proceed();
 case symval(makesym(atom__7C)):
  allocp[0] = a2;
  allocp[1] = makeint(124L);
  x0 = makecons(&allocp[0]);
  allocp += 2;
  unify_value(a1, x0);
  proceed();
  };
  goto unparse__bin__op_3_2;
 case VARREF:
  deref_and_jump(a0,unparse__bin__op_3_1);
  *reasonp++ =  a0;
  goto unparse__bin__op_3_2;
  };
  goto unparse__bin__op_3_2;
 unparse__bin__op_3_2:
  otherwise(unparse__bin__op_3_interrupt);
  execute(unparse__atom_3_0);
  goto unparse__atom_3_ext_interrupt;
 unparse__bin__op_3_ext_interrupt:
  reasonp = 0l;
 unparse__bin__op_3_interrupt:
  goto interrupt_3;
 }

 unparse__comma__list__elem_5_top: {
  q x0, x1, x2;
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  qp = qp->next;
 unparse__comma__list__elem_5_clear_reason:
  reasonp = reasons;
 unparse__comma__list__elem_5_0:
  x0 = a2;
  x1 = a0;
  a2 = a1;
  x2 = a3;
  a5 = a4;
  a4 = x2;
  a3 = makeint(999L);
  a1 = x1;
  a0 = x0;
  execute(call__unparser_6_0);
  goto call__unparser_6_ext_interrupt;
 unparse__comma__list__elem_5_ext_interrupt:
  reasonp = 0l;
 unparse__comma__list__elem_5_interrupt:
  toppred = &predicate_unparser_xunparse__comma__list__elem_5;
  goto interrupt_5;
 }

 call__unparser_6_top: {
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  qp = qp->next;
 call__unparser_6_clear_reason:
  reasonp = reasons;
 call__unparser_6_0:
  generic_arg[0] = a1;
  generic_arg[1] = a2;
  generic_arg[2] = a3;
  generic_arg[3] = a0;
  generic_arg[4] = a4;
  generic_arg[5] = a5;
  call_generic(a0, functor_call_6, 0);
  proceed();
 call__unparser_6_ext_interrupt:
  reasonp = 0l;
 call__unparser_6_interrupt:
  toppred = &predicate_unparser_xcall__unparser_6;
  goto interrupt_6;
 }

 unscan_4_top: {
  q x0, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13;
  a2 = qp->args[2];
  a3 = qp->args[3];
  qp = qp->next;
 unscan_4_clear_reason:
  reasonp = reasons;
 unscan_4_0:
 unscan_4_1:
  switch (ptagof(a0)) {
 case CONS:
  x0 = car_of(a0);
 unscan_4_2:
  switch (ptagof(x0)) {
 case ATOMIC:
  if (!isint(x0)) goto unscan_4_interrupt;
 unscan_4_3:
  x1 = makeint(0L);
  gblt_eq(x0,x1,unscan_4_4);
  a0 = cdr_of(a0);
  execute(unscan__after__op_4_0);
  goto unscan__after__op_4_ext_interrupt;
 unscan_4_4:
  x2 = makeint(0L);
  gblt_not_eq(x0,x2,unscan_4_interrupt);
  allocp[0] = x4 = makeref(&allocp[0]);
  allocp[1] = x0;
  x3 = makecons(&allocp[0]);
  allocp += 2;
  unify_value(a2, x3);
  a0 = cdr_of(a0);
  a1 = x0;
  a2 = x4;
  execute(unscan_4_0);
  goto unscan_4_ext_interrupt;
 case FUNCTOR:
 unscan_4_5:
  if (!isgobj(x0)) goto unscan_4_interrupt;
 unscan_4_6:
  gblt_is_string(x0,unscan_4_interrupt);
  gblt_size_of_string(x0,x1,unscan_4_interrupt);
  gblt_element_size_of_string(x0,x2,unscan_4_interrupt);
  gblt_is_string(x0,unscan_4_interrupt);
  x4 = makeint(0L);
  gblt_element_of_string(x0,x4,x3,unscan_4_interrupt);
  x6 = makeint(1L);
  gblt_subtract(x1,x6,x5,unscan_4_interrupt);
  gblt_is_string(x0,unscan_4_interrupt);
  gblt_element_of_string(x0,x5,x7,unscan_4_interrupt);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_unparser_xunscan_4);
  x8 = cdr_of(a0);
  allocp[2] = x8;
  allocp[3] = x7;
  allocp[4] = x9 = makeref(&allocp[4]);
  allocp[5] = a3;
  allocp[6] = makesym(functor_fwrite_1);
  allocp[7] = x0;
  x10 = makefunctor(&allocp[6]);
  allocp[8] = x9;
  allocp[9] = x10;
  x11 = makecons(&allocp[8]);
  allocp[10] = (q)(struct goalrec*)&allocp[0];
  allocp[11] = (q)(&predicate_unparser_xdelimit_4);
  allocp[12] = x12 = makeref(&allocp[12]);
  allocp[13] = x13 = makeref(&allocp[13]);
  allocp[14] = a2;
  allocp[15] = x11;
  allocp[16] = (q)(struct goalrec*)&allocp[10];
  allocp[17] = (q)(&predicate_unparser_xchartype_2);
  allocp[18] = x3;
  allocp[19] = x13;
  a0 = a1;
  a1 = x12;
  qp = (struct goalrec*)&allocp[16];
  allocp += 20;
  execute(chartype_2_0);
  goto chartype_2_ext_interrupt;
 case VARREF:
  deref_and_jump(x0,unscan_4_2);
  *reasonp++ =  x0;
  goto unscan_4_interrupt;
  };
  goto unscan_4_interrupt;
 case ATOMIC:
  if (a0 != NILATOM) goto unscan_4_interrupt;
  unify(a3, a2);
  proceed();
 case VARREF:
  deref_and_jump(a0,unscan_4_1);
  *reasonp++ =  a0;
  goto unscan_4_interrupt;
  };
  goto unscan_4_interrupt;
 unscan_4_ext_interrupt:
  reasonp = 0l;
 unscan_4_interrupt:
  toppred = &predicate_unparser_xunscan_4;
  goto interrupt_4;
 }

 unscan__after__op_4_top: {
  q x0, x1, x2;
  a2 = qp->args[2];
  a3 = qp->args[3];
  qp = qp->next;
 unscan__after__op_4_clear_reason:
  reasonp = reasons;
 unscan__after__op_4_0:
 unscan__after__op_4_1:
  switch (ptagof(a0)) {
 case CONS:
  x0 = car_of(a0);
 unscan__after__op_4_2:
  switch (ptagof(x0)) {
 case ATOMIC:
 unscan__after__op_4_3:
  if (x0 != makeint(40L)) goto unscan__after__op_4_4;
  allocp[0] = x2 = makeref(&allocp[0]);
  allocp[1] = makefunctor(funct_const_5);
  x1 = makecons(&allocp[0]);
  allocp += 2;
  unify_value(a2, x1);
  a0 = cdr_of(a0);
  a1 = makeint(40L);
  a2 = x2;
  execute(unscan_4_0);
  goto unscan_4_ext_interrupt;
 case VARREF:
  deref_and_jump(x0,unscan__after__op_4_2);
  *reasonp++ =  x0;
  goto unscan__after__op_4_4;
  };
  goto unscan__after__op_4_4;
 case VARREF:
  deref_and_jump(a0,unscan__after__op_4_1);
  *reasonp++ =  a0;
  goto unscan__after__op_4_4;
  };
  goto unscan__after__op_4_4;
 unscan__after__op_4_4:
  otherwise(unscan__after__op_4_interrupt);
  execute(unscan_4_0);
  goto unscan_4_ext_interrupt;
 unscan__after__op_4_ext_interrupt:
  reasonp = 0l;
 unscan__after__op_4_interrupt:
  toppred = &predicate_unparser_xunscan__after__op_4;
  goto interrupt_4;
 }

 chartype_2_top: {
  q x0;
  qp = qp->next;
 chartype_2_clear_reason:
  reasonp = reasons;
 chartype_2_0:
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_unparser_xchartype_2F2_240_2);
  allocp[2] = x0 = makeref(&allocp[2]);
  allocp[3] = a1;
  allocp[4] = (q)(struct goalrec*)&allocp[0];
  allocp[5] = (q)(&predicate_scanner_xchartype_2);
  allocp[6] = a0;
  allocp[7] = x0;
  qp = (struct goalrec*)&allocp[4];
  allocp += 8;
  proceed();
 chartype_2_ext_interrupt:
  reasonp = 0l;
 chartype_2_interrupt:
  toppred = &predicate_unparser_xchartype_2;
  goto interrupt_2;
 }

 chartype_2F2_240_2_top: {
  q x0, x1, x2, x3;
  qp = qp->next;
 chartype_2F2_240_2_clear_reason:
  reasonp = reasons;
 chartype_2F2_240_2_0:
 chartype_2F2_240_2_1:
  if (!isint(a0)) goto chartype_2F2_240_2_4;
  x0 = makeint(3L);
  gblt_less_or_eq(a0,x0,chartype_2F2_240_2_2);
  x1 = makeint(0L);
  unify_value(a1, x1);
  proceed();
 chartype_2F2_240_2_2:
  x1 = makeint(4L);
  gblt_eq(a0,x1,chartype_2F2_240_2_3);
  x2 = makeint(1L);
  unify_value(a1, x2);
  proceed();
 chartype_2F2_240_2_3:
  x2 = makeint(6L);
  gblt_eq(a0,x2,chartype_2F2_240_2_5);
  x3 = makeint(2L);
  unify_value(a1, x3);
  proceed();
 chartype_2F2_240_2_4:
  if (!isref(a0)) goto chartype_2F2_240_2_5;
  deref_and_jump(a0,chartype_2F2_240_2_1);
  *reasonp++ =  a0;
  goto chartype_2F2_240_2_5;
 chartype_2F2_240_2_5:
  otherwise(chartype_2F2_240_2_interrupt);
  x0 = makeint(-1L);
  unify_value(a1, x0);
  proceed();
 chartype_2F2_240_2_ext_interrupt:
  reasonp = 0l;
 chartype_2F2_240_2_interrupt:
  goto interrupt_2;
 }

 delimit_4_top: {
  q x0, x1;
  a2 = qp->args[2];
  a3 = qp->args[3];
  qp = qp->next;
 delimit_4_clear_reason:
  reasonp = reasons;
 delimit_4_0:
 delimit_4_1:
  if (isref(a0)) goto delimit_4_6;
 delimit_4_2:
  if (isref(a0)) goto delimit_4_5;
  if_not_equal(a0, a1, delimit_4_7);
 delimit_4_3:
  if (!isint(a0)) goto delimit_4_4;
  x0 = makeint(0L);
  gblt_greater_or_eq(a0,x0,delimit_4_7);
  allocp[0] = a3;
  allocp[1] = makeint(32L);
  x1 = makecons(&allocp[0]);
  allocp += 2;
  unify_value(a2, x1);
  proceed();
 delimit_4_4:
  if (!isref(a0)) goto delimit_4_7;
  deref_and_jump(a0,delimit_4_3);
  *reasonp++ =  a0;
  goto delimit_4_7;
 delimit_4_5:
  deref_and_jump(a0,delimit_4_2);
  *reasonp++ =  a0;
  goto delimit_4_7;
 delimit_4_6:
  deref_and_jump(a0,delimit_4_1);
  *reasonp++ =  a0;
  goto delimit_4_7;
 delimit_4_7:
  otherwise(delimit_4_interrupt);
  unify(a3, a2);
  proceed();
 delimit_4_ext_interrupt:
  reasonp = 0l;
 delimit_4_interrupt:
  goto interrupt_4;
 }
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
  loop_within_module(module_unparser);
}
