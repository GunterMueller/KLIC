/* Compiled by KLIC compiler version 3.003 (Mon Mar 29 19:42:59 JST 1999) */
#include <klic/klichdr.h>
#include "atom.h"
#include "funct.h"

module module_parser();
Const struct predicate predicate_parser_xparse_3 =
   { module_parser, 0, 3 };
Const struct predicate predicate_parser_xwait__parse_2 =
   { module_parser, 1, 2 };
Const struct predicate predicate_parser_xwait__parse_2F2_240_2 =
   { module_parser, 2, 2 };
Const struct predicate predicate_parser_xwparse_3 =
   { module_parser, 3, 3 };
Const struct predicate predicate_parser_xlast__check_2 =
   { module_parser, 4, 2 };
Const struct predicate predicate_parser_xwparse_4 =
   { module_parser, 5, 4 };
Const struct predicate predicate_parser_xwparse_5 =
   { module_parser, 6, 5 };
Const struct predicate predicate_parser_xwparse_2F5_244_5 =
   { module_parser, 7, 5 };
Const struct predicate predicate_parser_xwparse_2F5_244_2F5_242_2 =
   { module_parser, 8, 2 };
Const struct predicate predicate_parser_xwparse_2F5_244_2F5_241_5 =
   { module_parser, 9, 5 };
Const struct predicate predicate_parser_xwparse_2F5_244_2F5_240_6 =
   { module_parser, 10, 6 };
Const struct predicate predicate_parser_xwparse_2F5_244_2F5_240_2F6_240_6 =
   { module_parser, 11, 6 };
Const struct predicate predicate_parser_xwparse_2F5_243_5 =
   { module_parser, 12, 5 };
Const struct predicate predicate_parser_xwparse_2F5_242_4 =
   { module_parser, 13, 4 };
Const struct predicate predicate_parser_xwparse_2F5_242_2F4_240_4 =
   { module_parser, 14, 4 };
Const struct predicate predicate_parser_xwparse_2F5_241_4 =
   { module_parser, 15, 4 };
Const struct predicate predicate_parser_xwparse_2F5_241_2F4_240_4 =
   { module_parser, 16, 4 };
Const struct predicate predicate_parser_xwparse_2F5_240_4 =
   { module_parser, 17, 4 };
Const struct predicate predicate_parser_xwparse_2F5_240_2F4_240_5 =
   { module_parser, 18, 5 };
Const struct predicate predicate_parser_xafter__term_6 =
   { module_parser, 19, 6 };
Const struct predicate predicate_parser_xafter__term_2F6_242_8 =
   { module_parser, 20, 8 };
Const struct predicate predicate_parser_xafter__term_2F6_242_2F8_241_7 =
   { module_parser, 21, 7 };
Const struct predicate predicate_parser_xafter__term_2F6_242_2F8_240_9 =
   { module_parser, 22, 9 };
Const struct predicate predicate_parser_xafter__term_2F6_242_2F8_240_2F9_240_7 =
   { module_parser, 23, 7 };
Const struct predicate predicate_parser_xafter__term_2F6_241_5 =
   { module_parser, 24, 5 };
Const struct predicate predicate_parser_xafter__term_2F6_240_5 =
   { module_parser, 25, 5 };
Const struct predicate predicate_parser_xparse__comma__list_6 =
   { module_parser, 26, 6 };
Const struct predicate predicate_parser_xparse__comma__list_8 =
   { module_parser, 27, 8 };
Const struct predicate predicate_parser_xparse__comma__list_2F8_240_8 =
   { module_parser, 28, 8 };
Const struct predicate predicate_parser_xparse__comma__list_2F8_240_2F8_240_8 =
   { module_parser, 29, 8 };
Const struct predicate predicate_parser_xparse__comma__list_2F8_240_2F8_240_2F8_240_5 =
   { module_parser, 30, 5 };
Const struct predicate predicate_parser_xparse__comma__list_2F8_240_2F8_240_2F8_240_2F5_240_4 =
   { module_parser, 31, 4 };
Const struct predicate predicate_parser_xlist__to__comma__list_2 =
   { module_parser, 32, 2 };
Const struct predicate predicate_parser_xselect__better_3 =
   { module_parser, 33, 3 };
Const struct predicate predicate_parser_xselect__better_2F3_241_4 =
   { module_parser, 34, 4 };
Const struct predicate predicate_parser_xselect__better_2F3_240_4 =
   { module_parser, 35, 4 };
Const struct predicate predicate_parser_xprefixop_4 =
   { module_parser, 36, 4 };
Const struct predicate predicate_parser_xprefixop_2F4_240_4 =
   { module_parser, 37, 4 };
Const struct predicate predicate_parser_xinfixop_5 =
   { module_parser, 38, 5 };
Const struct predicate predicate_parser_xinfixop_2F5_240_5 =
   { module_parser, 39, 5 };
Const struct predicate predicate_parser_xpostfixop_5 =
   { module_parser, 40, 5 };
Const struct predicate predicate_parser_xpostfixop_2F5_240_5 =
   { module_parser, 41, 5 };
Const struct predicate predicate_parser_xlookup__op_4 =
   { module_parser, 42, 4 };
Const struct predicate predicate_parser_xlonger_3 =
   { module_parser, 43, 3 };
Const struct predicate predicate_parser_xwrap__list_2 =
   { module_parser, 44, 2 };
Const struct predicate predicate_parser_xwrap__functor_2 =
   { module_parser, 45, 2 };
Const struct predicate predicate_parser_xwrap__functor1_2 =
   { module_parser, 46, 2 };
Const struct predicate predicate_parser_xunwrap_2 =
   { module_parser, 47, 2 };
extern q vector_g_new();
extern q float_g_new();
extern Const struct predicate predicate_variable_xunwrap_2;
extern Const struct predicate predicate_functor__table_x_3D_2E_2E_2;

module module_parser(glbl, qp, allocp, toppred)
  struct global_variables *glbl;
  struct goalrec *qp;
  register q *allocp;
  Const struct predicate *toppred;
{
  static Const unsigned char strconst_body_0[28] =
    "close parenthesis expected";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_0 =
      declare_string_constant_8(strconst_body_0,26);
  static Const unsigned char strconst_body_1[36] =
    "comma or close parenthesis expected";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_1 =
      declare_string_constant_8(strconst_body_1,35);
  static Const unsigned char strconst_body_2[20] =
    "operator expected";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_2 =
      declare_string_constant_8(strconst_body_2,17);
  static Const unsigned char strconst_body_3[24] =
    "term expected to start";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_3 =
      declare_string_constant_8(strconst_body_3,22);
  static Const q funct_const_4[] = {
    makesym(functor_normal_1),
    makesym(atom_end__of__file),
  };
  static Const unsigned char strconst_body_5[20] =
    "expression expected";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_5 =
      declare_string_constant_8(strconst_body_5,19);
  static Const q funct_const_6[] = {
    makesym(functor_abnormal_2),
    makefunctor(&string_const_5),
    NILATOM,
  };
  static Const q funct_const_7[] = {
    makesym(functor_atom_1),
    NILATOM,
  };
  static Const q funct_const_8[] = {
    makesym(functor_atom_1),
    makesym(atom__7C),
  };
  q a0, a1, a2, a3, a4, a5, a6, a7, a8;

  q *reasonp;
 module_top:
  a0 = qp->args[0];
  a1 = qp->args[1];
  switch_on_pred() {
    case_pred(0, parse_3_top);
    case_pred(1, wait__parse_2_top);
    case_pred(2, wait__parse_2F2_240_2_top);
    case_pred(3, wparse_3_top);
    case_pred(4, last__check_2_top);
    case_pred(5, wparse_4_top);
    case_pred(6, wparse_5_top);
    case_pred(7, wparse_2F5_244_5_top);
    case_pred(8, wparse_2F5_244_2F5_242_2_top);
    case_pred(9, wparse_2F5_244_2F5_241_5_top);
    case_pred(10, wparse_2F5_244_2F5_240_6_top);
    case_pred(11, wparse_2F5_244_2F5_240_2F6_240_6_top);
    case_pred(12, wparse_2F5_243_5_top);
    case_pred(13, wparse_2F5_242_4_top);
    case_pred(14, wparse_2F5_242_2F4_240_4_top);
    case_pred(15, wparse_2F5_241_4_top);
    case_pred(16, wparse_2F5_241_2F4_240_4_top);
    case_pred(17, wparse_2F5_240_4_top);
    case_pred(18, wparse_2F5_240_2F4_240_5_top);
    case_pred(19, after__term_6_top);
    case_pred(20, after__term_2F6_242_8_top);
    case_pred(21, after__term_2F6_242_2F8_241_7_top);
    case_pred(22, after__term_2F6_242_2F8_240_9_top);
    case_pred(23, after__term_2F6_242_2F8_240_2F9_240_7_top);
    case_pred(24, after__term_2F6_241_5_top);
    case_pred(25, after__term_2F6_240_5_top);
    case_pred(26, parse__comma__list_6_top);
    case_pred(27, parse__comma__list_8_top);
    case_pred(28, parse__comma__list_2F8_240_8_top);
    case_pred(29, parse__comma__list_2F8_240_2F8_240_8_top);
    case_pred(30, parse__comma__list_2F8_240_2F8_240_2F8_240_5_top);
    case_pred(31, parse__comma__list_2F8_240_2F8_240_2F8_240_2F5_240_4_top);
    case_pred(32, list__to__comma__list_2_top);
    case_pred(33, select__better_3_top);
    case_pred(34, select__better_2F3_241_4_top);
    case_pred(35, select__better_2F3_240_4_top);
    case_pred(36, prefixop_4_top);
    case_pred(37, prefixop_2F4_240_4_top);
    case_pred(38, infixop_5_top);
    case_pred(39, infixop_2F5_240_5_top);
    case_pred(40, postfixop_5_top);
    case_pred(41, postfixop_2F5_240_5_top);
    case_pred(42, lookup__op_4_top);
    case_pred(43, longer_3_top);
    case_pred(44, wrap__list_2_top);
    case_pred(45, wrap__functor_2_top);
    case_pred(46, wrap__functor1_2_top);
    last_case_pred(47, unwrap_2_top);
  }

 parse_3_top: {
  q x0;
  a2 = qp->args[2];
  qp = qp->next;
 parse_3_clear_reason:
  reasonp = reasons;
 parse_3_0:
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_parser_xwait__parse_2);
  allocp[2] = x0 = makeref(&allocp[2]);
  allocp[3] = a2;
  a2 = x0;
  qp = (struct goalrec*)&allocp[0];
  allocp += 4;
  execute(wparse_3_0);
  goto wparse_3_ext_interrupt;
 parse_3_ext_interrupt:
  reasonp = 0l;
 parse_3_interrupt:
  goto interrupt_3;
 }

 wait__parse_2_top: {
  q x0, x1;
  qp = qp->next;
 wait__parse_2_clear_reason:
  reasonp = reasons;
 wait__parse_2_0:
 wait__parse_2_1:
  switch (ptagof(a0)) {
 case FUNCTOR:
  if (functor_of(a0) != makesym(functor_normal_1)) goto wait__parse_2_2;
  allocp[0] = makesym(functor_normal_1);
  allocp[1] = x1 = makeref(&allocp[1]);
  x0 = makefunctor(&allocp[0]);
  allocp += 2;
  unify_value(a1, x0);
  a0 = arg(a0, 0);
  a1 = x1;
  execute(wait__parse_2F2_240_2_0);
  goto wait__parse_2F2_240_2_ext_interrupt;
 case VARREF:
  deref_and_jump(a0,wait__parse_2_1);
  *reasonp++ =  a0;
  goto wait__parse_2_2;
  };
  goto wait__parse_2_2;
 wait__parse_2_2:
  otherwise(wait__parse_2_interrupt);
  unify(a1, a0);
  proceed();
 wait__parse_2_ext_interrupt:
  reasonp = 0l;
 wait__parse_2_interrupt:
  goto interrupt_2;
 }

 wait__parse_2F2_240_2_top: {
  q x0;
  qp = qp->next;
 wait__parse_2F2_240_2_clear_reason:
  reasonp = reasons;
 wait__parse_2F2_240_2_0:
 wait__parse_2F2_240_2_1:
  switch (ptagof(a0)) {
 case ATOMIC:
  if (a0 != makesym(atom_end__of__file)) goto wait__parse_2F2_240_2_2;
  x0 = makesym(atom_end__of__file);
  unify_value(a1, x0);
  proceed();
 case VARREF:
  deref_and_jump(a0,wait__parse_2F2_240_2_1);
  *reasonp++ =  a0;
  goto wait__parse_2F2_240_2_interrupt;
  };
  goto wait__parse_2F2_240_2_2;
 wait__parse_2F2_240_2_2:
  x0 = makesym(atom_end__of__file);
  gblt_diff(a0,x0,wait__parse_2F2_240_2_interrupt);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_variable_xunwrap_2);
  allocp[2] = a0;
  allocp[3] = a1;
  qp = (struct goalrec*)&allocp[0];
  allocp += 4;
  proceed();
 wait__parse_2F2_240_2_ext_interrupt:
  reasonp = 0l;
 wait__parse_2F2_240_2_interrupt:
  toppred = &predicate_parser_xwait__parse_2F2_240_2;
  goto interrupt_2;
 }

 wparse_3_top: {
  q x0, x1, x2;
  a2 = qp->args[2];
  qp = qp->next;
 wparse_3_clear_reason:
  reasonp = reasons;
 wparse_3_0:
 wparse_3_1:
  switch (ptagof(a0)) {
 case CONS:
  x0 = car_of(a0);
 wparse_3_2:
  switch (ptagof(x0)) {
 case ATOMIC:
  if (x0 != makesym(atom_eof)) goto wparse_3_4;
  x1 = cdr_of(a0);
 wparse_3_3:
  switch (ptagof(x1)) {
 case ATOMIC:
  if (x1 != NILATOM) goto wparse_3_4;
  x2 = makefunctor(funct_const_4);
  unify_value(a2, x2);
  proceed();
 case VARREF:
  deref_and_jump(x1,wparse_3_3);
  *reasonp++ =  x1;
  goto wparse_3_4;
  };
  goto wparse_3_4;
 case VARREF:
  deref_and_jump(x0,wparse_3_2);
  *reasonp++ =  x0;
  goto wparse_3_4;
  };
  goto wparse_3_4;
 case VARREF:
  deref_and_jump(a0,wparse_3_1);
  *reasonp++ =  a0;
  goto wparse_3_4;
  };
  goto wparse_3_4;
 wparse_3_4:
  otherwise(wparse_3_interrupt);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_parser_xlast__check_2);
  allocp[2] = x0 = makeref(&allocp[2]);
  allocp[3] = a2;
  a2 = a1;
  a3 = x0;
  a1 = makeint(1200L);
  qp = (struct goalrec*)&allocp[0];
  allocp += 4;
  execute(wparse_4_0);
  goto wparse_4_ext_interrupt;
 wparse_3_ext_interrupt:
  reasonp = 0l;
 wparse_3_interrupt:
  toppred = &predicate_parser_xwparse_3;
  goto interrupt_3;
 }

 last__check_2_top: {
  q x0, x1, x2, x3, x4;
  qp = qp->next;
 last__check_2_clear_reason:
  reasonp = reasons;
 last__check_2_0:
 last__check_2_1:
  switch (ptagof(a0)) {
 case FUNCTOR:
  switch (symval(functor_of(a0))) {
 case functor_normal_2:
  x0 = arg(a0, 1);
 last__check_2_2:
  switch (ptagof(x0)) {
 case CONS:
  x2 = cdr_of(x0);
  allocp[0] = x2;
  x3 = car_of(x0);
  allocp[1] = x3;
  x1 = makecons(&allocp[0]);
  allocp[2] = makesym(functor_abnormal_2);
  allocp[3] = makefunctor(&string_const_2);
  allocp[4] = x1;
  x4 = makefunctor(&allocp[2]);
  allocp += 5;
  unify_value(a1, x4);
  proceed();
 case ATOMIC:
  if (x0 != NILATOM) goto last__check_2_interrupt;
  allocp[0] = makesym(functor_normal_1);
  x2 = arg(a0, 0);
  allocp[1] = x2;
  x1 = makefunctor(&allocp[0]);
  allocp += 2;
  unify_value(a1, x1);
  proceed();
 case VARREF:
  deref_and_jump(x0,last__check_2_2);
  *reasonp++ =  x0;
  goto last__check_2_interrupt;
  };
  goto last__check_2_interrupt;
 case functor_abnormal_2:
  allocp[0] = makesym(functor_abnormal_2);
  x1 = arg(a0, 0);
  allocp[1] = x1;
  x2 = arg(a0, 1);
  allocp[2] = x2;
  x0 = makefunctor(&allocp[0]);
  allocp += 3;
  unify_value(a1, x0);
  proceed();
  };
  goto last__check_2_interrupt;
 case VARREF:
  deref_and_jump(a0,last__check_2_1);
  *reasonp++ =  a0;
  goto last__check_2_interrupt;
  };
  goto last__check_2_interrupt;
 last__check_2_ext_interrupt:
  reasonp = 0l;
 last__check_2_interrupt:
  goto interrupt_2;
 }

 wparse_4_top: {
  q x0, x1, x2, x3;
  a2 = qp->args[2];
  a3 = qp->args[3];
  qp = qp->next;
 wparse_4_clear_reason:
  reasonp = reasons;
 wparse_4_0:
 wparse_4_1:
  switch (ptagof(a0)) {
 case CONS:
  x0 = car_of(a0);
  x1 = cdr_of(a0);
  x2 = a1;
  x3 = a2;
  a4 = a3;
  a3 = x3;
  a2 = x2;
  a1 = x1;
  a0 = x0;
  execute(wparse_5_0);
  goto wparse_5_ext_interrupt;
 case ATOMIC:
  if (a0 != NILATOM) goto wparse_4_interrupt;
  x0 = makefunctor(funct_const_6);
  unify_value(a3, x0);
  proceed();
 case VARREF:
  deref_and_jump(a0,wparse_4_1);
  *reasonp++ =  a0;
  goto wparse_4_interrupt;
  };
  goto wparse_4_interrupt;
 wparse_4_ext_interrupt:
  reasonp = 0l;
 wparse_4_interrupt:
  toppred = &predicate_parser_xwparse_4;
  goto interrupt_4;
 }

 wparse_5_top: {
  q x0, x1, x2, x3;
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  qp = qp->next;
 wparse_5_clear_reason:
  reasonp = reasons;
 wparse_5_0:
 wparse_5_1:
  switch (ptagof(a0)) {
 case ATOMIC:
  if (isint(a0)) goto wparse_5_6;
  switch (symval(a0)) {
 case symval(makesym(atom__7C)):
  a0 = makefunctor(funct_const_8);
  execute(wparse_5_0);
  goto wparse_5_ext_interrupt;
 case symval(makesym(atom__5B)):
  a0 = a1;
  a1 = a2;
  a2 = a3;
  a3 = a4;
  execute(wparse_2F5_242_4_0);
  goto wparse_2F5_242_4_ext_interrupt;
 case symval(makesym(atom__7B)):
  a0 = a1;
  a1 = a2;
  a2 = a3;
  a3 = a4;
  execute(wparse_2F5_241_4_0);
  goto wparse_2F5_241_4_ext_interrupt;
 case symval(makesym(atom__28)):
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_parser_xwparse_2F5_240_4);
  allocp[2] = x0 = makeref(&allocp[2]);
  allocp[3] = a2;
  allocp[4] = a3;
  allocp[5] = a4;
  a0 = a1;
  a1 = makeint(1200L);
  a2 = a3;
  a3 = x0;
  qp = (struct goalrec*)&allocp[0];
  allocp += 6;
  execute(wparse_4_0);
  goto wparse_4_ext_interrupt;
 case symval(makesym(atom__20_28)):
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_parser_xwparse_2F5_240_4);
  allocp[2] = x0 = makeref(&allocp[2]);
  allocp[3] = a2;
  allocp[4] = a3;
  allocp[5] = a4;
  a0 = a1;
  a1 = makeint(1200L);
  a2 = a3;
  a3 = x0;
  qp = (struct goalrec*)&allocp[0];
  allocp += 6;
  execute(wparse_4_0);
  goto wparse_4_ext_interrupt;
  };
  goto wparse_5_6;
 case FUNCTOR:
  switch (symval(functor_of(a0))) {
 case functor_variable_4:
  allocp[0] = makesym(functor_variable_1);
  x1 = arg(a0, 2);
  allocp[1] = x1;
  x0 = makefunctor(&allocp[0]);
  a0 = a1;
  a1 = makeint(0L);
  x2 = a2;
  x3 = a3;
  a5 = a4;
  a4 = x3;
  a3 = x2;
  a2 = x0;
  allocp += 2;
  execute(after__term_6_0);
  goto after__term_6_ext_interrupt;
 case functor_atom_1:
  x0 = a1;
  a1 = arg(a0, 0);
  a0 = x0;
  execute(wparse_2F5_244_5_0);
  goto wparse_2F5_244_5_ext_interrupt;
 case functor_number_1:
  x0 = arg(a0, 0);
 wparse_5_2:
  switch (ptagof(x0)) {
 case ATOMIC:
  if (!isint(x0)) goto wparse_5_6;
 wparse_5_3:
  gblt_integer(x0,wparse_5_6);
  allocp[0] = makesym(functor_integer_1);
  allocp[1] = x0;
  x1 = makefunctor(&allocp[0]);
  a0 = a1;
  a1 = makeint(0L);
  x2 = a2;
  x3 = a3;
  a5 = a4;
  a4 = x3;
  a3 = x2;
  a2 = x1;
  allocp += 2;
  execute(after__term_6_0);
  goto after__term_6_ext_interrupt;
 case FUNCTOR:
 wparse_5_4:
  if (!isgobj(x0)) goto wparse_5_6;
 wparse_5_5:
  guard_generic(x0, functor_float_0, 0, wparse_5_6);
  allocp[0] = makesym(functor_floating__point_1);
  allocp[1] = x0;
  x1 = makefunctor(&allocp[0]);
  a0 = a1;
  a1 = makeint(0L);
  x2 = a2;
  x3 = a3;
  a5 = a4;
  a4 = x3;
  a3 = x2;
  a2 = x1;
  allocp += 2;
  execute(after__term_6_0);
  goto after__term_6_ext_interrupt;
 case VARREF:
  deref_and_jump(x0,wparse_5_2);
  *reasonp++ =  x0;
  goto wparse_5_6;
  };
  goto wparse_5_6;
 case functor_string_1:
  x0 = a1;
  a1 = arg(a0, 0);
  a0 = x0;
  execute(wparse_2F5_243_5_0);
  goto wparse_2F5_243_5_ext_interrupt;
 case functor_error_1:
  allocp[0] = makesym(functor_abnormal_2);
  x1 = arg(a0, 0);
  allocp[1] = x1;
  allocp[2] = a1;
  x0 = makefunctor(&allocp[0]);
  allocp += 3;
  unify_value(a4, x0);
  proceed();
  };
  goto wparse_5_6;
 case VARREF:
  deref_and_jump(a0,wparse_5_1);
  *reasonp++ =  a0;
  goto wparse_5_6;
  };
  goto wparse_5_6;
 wparse_5_6:
  otherwise(wparse_5_interrupt);
  allocp[0] = a1;
  allocp[1] = a0;
  x0 = makecons(&allocp[0]);
  allocp[2] = makesym(functor_abnormal_2);
  allocp[3] = makefunctor(&string_const_3);
  allocp[4] = x0;
  x1 = makefunctor(&allocp[2]);
  allocp += 5;
  unify_value(a4, x1);
  proceed();
 wparse_5_ext_interrupt:
  reasonp = 0l;
 wparse_5_interrupt:
  toppred = &predicate_parser_xwparse_5;
  goto interrupt_5;
 }

 wparse_2F5_244_5_top: {
  q x0, x1, x2;
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  qp = qp->next;
 wparse_2F5_244_5_clear_reason:
  reasonp = reasons;
 wparse_2F5_244_5_0:
 wparse_2F5_244_5_1:
  switch (ptagof(a0)) {
 case CONS:
  x0 = car_of(a0);
 wparse_2F5_244_5_2:
  switch (ptagof(x0)) {
 case ATOMIC:
  if (x0 != makesym(atom__28)) goto wparse_2F5_244_5_4;
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_parser_xwparse_2F5_244_2F5_241_5);
  allocp[2] = x1 = makeref(&allocp[2]);
  allocp[3] = a1;
  allocp[4] = a2;
  allocp[5] = a3;
  allocp[6] = a4;
  a0 = cdr_of(a0);
  a1 = a3;
  a2 = makeint(999L);
  a3 = makesym(atom_never);
  a4 = makesym(atom__29);
  a5 = x1;
  qp = (struct goalrec*)&allocp[0];
  allocp += 7;
  execute(parse__comma__list_6_0);
  goto parse__comma__list_6_ext_interrupt;
 case FUNCTOR:
  if (functor_of(x0) != makesym(functor_number_1)) goto wparse_2F5_244_5_4;
 wparse_2F5_244_5_3:
  switch (ptagof(a1)) {
 case ATOMIC:
  if (a1 != makesym(atom__2D)) goto wparse_2F5_244_5_4;
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_parser_xafter__term_6);
  x1 = cdr_of(a0);
  allocp[2] = x1;
  allocp[3] = makeint(0L);
  allocp[4] = x2 = makeref(&allocp[4]);
  allocp[5] = a2;
  allocp[6] = a3;
  allocp[7] = a4;
  a0 = arg(x0, 0);
  a1 = x2;
  qp = (struct goalrec*)&allocp[0];
  allocp += 8;
  execute(wparse_2F5_244_2F5_242_2_0);
  goto wparse_2F5_244_2F5_242_2_ext_interrupt;
 case VARREF:
  deref_and_jump(a1,wparse_2F5_244_5_3);
  *reasonp++ =  a1;
  goto wparse_2F5_244_5_4;
  };
  goto wparse_2F5_244_5_4;
 case VARREF:
  deref_and_jump(x0,wparse_2F5_244_5_2);
  *reasonp++ =  x0;
  goto wparse_2F5_244_5_4;
  };
  goto wparse_2F5_244_5_4;
 case VARREF:
  deref_and_jump(a0,wparse_2F5_244_5_1);
  *reasonp++ =  a0;
  goto wparse_2F5_244_5_4;
  };
  goto wparse_2F5_244_5_4;
 wparse_2F5_244_5_4:
  otherwise(wparse_2F5_244_5_interrupt);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_parser_xwparse_2F5_244_2F5_240_6);
  allocp[2] = x0 = makeref(&allocp[2]);
  allocp[3] = a0;
  allocp[4] = a3;
  allocp[5] = a1;
  allocp[6] = a2;
  allocp[7] = a4;
  a0 = a1;
  a1 = a3;
  a3 = x0;
  qp = (struct goalrec*)&allocp[0];
  allocp += 8;
  execute(prefixop_4_0);
  goto prefixop_4_ext_interrupt;
 wparse_2F5_244_5_ext_interrupt:
  reasonp = 0l;
 wparse_2F5_244_5_interrupt:
  toppred = &predicate_parser_xwparse_2F5_244_5;
  goto interrupt_5;
 }

 wparse_2F5_244_2F5_242_2_top: {
  q x0, x1, x2, x3;
  qp = qp->next;
 wparse_2F5_244_2F5_242_2_clear_reason:
  reasonp = reasons;
 wparse_2F5_244_2F5_242_2_0:
 wparse_2F5_244_2F5_242_2_1:
  if (!isint(a0)) goto wparse_2F5_244_2F5_242_2_2;
  gblt_integer(a0,wparse_2F5_244_2F5_242_2_3);
  bblt_minus_no_check(a0,x0);
  allocp[0] = makesym(functor_integer_1);
  allocp[1] = x0;
  x1 = makefunctor(&allocp[0]);
  allocp += 2;
  unify_value(a1, x1);
  proceed();
 wparse_2F5_244_2F5_242_2_2:
  if (!isref(a0)) goto wparse_2F5_244_2F5_242_2_3;
  deref_and_jump(a0,wparse_2F5_244_2F5_242_2_1);
  *reasonp++ =  a0;
  goto wparse_2F5_244_2F5_242_2_3;
 wparse_2F5_244_2F5_242_2_3:
  otherwise(wparse_2F5_244_2F5_242_2_interrupt);
  x0 = makeint(0L);
  generic_arg[0] = x0;
  new_generic(float_g_new, 1, x1, 0);
  generic_arg[0] = a0;
  allocp[0] = x2 = makeref(&allocp[0]);
  generic_arg[1] = x2;
  call_generic(x1, functor_subtract_2, 1);
  allocp[0] = makesym(functor_floating__point_1);
  allocp[1] = x2;
  x3 = makefunctor(&allocp[0]);
  allocp += 2;
  unify_value(a1, x3);
  proceed();
 wparse_2F5_244_2F5_242_2_ext_interrupt:
  reasonp = 0l;
 wparse_2F5_244_2F5_242_2_interrupt:
  toppred = &predicate_parser_xwparse_2F5_244_2F5_242_2;
  goto interrupt_2;
 }

 wparse_2F5_244_2F5_241_5_top: {
  q x0, x1, x2, x3;
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  qp = qp->next;
 wparse_2F5_244_2F5_241_5_clear_reason:
  reasonp = reasons;
 wparse_2F5_244_2F5_241_5_0:
 wparse_2F5_244_2F5_241_5_1:
  switch (ptagof(a0)) {
 case FUNCTOR:
  if (functor_of(a0) != makesym(functor_normal_2)) goto wparse_2F5_244_2F5_241_5_2;
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_parser_xafter__term_6);
  x0 = arg(a0, 1);
  allocp[2] = x0;
  allocp[3] = makeint(0L);
  allocp[4] = x1 = makeref(&allocp[4]);
  allocp[5] = a2;
  allocp[6] = a3;
  allocp[7] = a4;
  x3 = arg(a0, 0);
  allocp[8] = x3;
  allocp[9] = a1;
  x2 = makecons(&allocp[8]);
  a0 = x2;
  a1 = x1;
  qp = (struct goalrec*)&allocp[0];
  allocp += 10;
  execute(wrap__functor_2_0);
  goto wrap__functor_2_ext_interrupt;
 case VARREF:
  deref_and_jump(a0,wparse_2F5_244_2F5_241_5_1);
  *reasonp++ =  a0;
  goto wparse_2F5_244_2F5_241_5_2;
  };
  goto wparse_2F5_244_2F5_241_5_2;
 wparse_2F5_244_2F5_241_5_2:
  otherwise(wparse_2F5_244_2F5_241_5_interrupt);
  unify(a4, a0);
  proceed();
 wparse_2F5_244_2F5_241_5_ext_interrupt:
  reasonp = 0l;
 wparse_2F5_244_2F5_241_5_interrupt:
  goto interrupt_5;
 }

 wparse_2F5_244_2F5_240_6_top: {
  q x0, x1, x2, x3, x4, x5;
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  qp = qp->next;
 wparse_2F5_244_2F5_240_6_clear_reason:
  reasonp = reasons;
 wparse_2F5_244_2F5_240_6_0:
 wparse_2F5_244_2F5_240_6_1:
  switch (ptagof(a0)) {
 case ATOMIC:
  if (a0 != makesym(atom_none)) goto wparse_2F5_244_2F5_240_6_interrupt;
  allocp[0] = makesym(functor_atom_1);
  allocp[1] = a3;
  x0 = makefunctor(&allocp[0]);
  a0 = a1;
  a1 = makeint(0L);
  a3 = a4;
  a4 = a2;
  a2 = x0;
  allocp += 2;
  execute(after__term_6_0);
  goto after__term_6_ext_interrupt;
 case FUNCTOR:
  if (functor_of(a0) != makesym(functor_prefix_2)) goto wparse_2F5_244_2F5_240_6_interrupt;
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_parser_xselect__better_3);
  allocp[2] = x0 = makeref(&allocp[2]);
  allocp[3] = x1 = makeref(&allocp[3]);
  allocp[4] = a5;
  allocp[5] = makesym(functor_atom_1);
  allocp[6] = a3;
  x2 = makefunctor(&allocp[5]);
  allocp[7] = (q)(struct goalrec*)&allocp[0];
  allocp[8] = (q)(&predicate_parser_xafter__term_6);
  allocp[9] = a1;
  allocp[10] = makeint(0L);
  allocp[11] = x2;
  allocp[12] = a4;
  allocp[13] = a2;
  allocp[14] = x1;
  allocp[15] = (q)(struct goalrec*)&allocp[7];
  allocp[16] = (q)(&predicate_parser_xwparse_2F5_244_2F5_240_2F6_240_6);
  allocp[17] = x3 = makeref(&allocp[17]);
  allocp[18] = a3;
  x4 = arg(a0, 0);
  allocp[19] = x4;
  allocp[20] = a4;
  allocp[21] = a2;
  allocp[22] = x0;
  x5 = a1;
  a1 = arg(a0, 1);
  a3 = x3;
  a0 = x5;
  qp = (struct goalrec*)&allocp[15];
  allocp += 23;
  execute(wparse_4_0);
  goto wparse_4_ext_interrupt;
 case VARREF:
  deref_and_jump(a0,wparse_2F5_244_2F5_240_6_1);
  *reasonp++ =  a0;
  goto wparse_2F5_244_2F5_240_6_interrupt;
  };
  goto wparse_2F5_244_2F5_240_6_interrupt;
 wparse_2F5_244_2F5_240_6_ext_interrupt:
  reasonp = 0l;
 wparse_2F5_244_2F5_240_6_interrupt:
  goto interrupt_6;
 }

 wparse_2F5_244_2F5_240_2F6_240_6_top: {
  q x0, x1, x2, x3, x4;
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  qp = qp->next;
 wparse_2F5_244_2F5_240_2F6_240_6_clear_reason:
  reasonp = reasons;
 wparse_2F5_244_2F5_240_2F6_240_6_0:
 wparse_2F5_244_2F5_240_2F6_240_6_1:
  switch (ptagof(a0)) {
 case FUNCTOR:
  if (functor_of(a0) != makesym(functor_normal_2)) goto wparse_2F5_244_2F5_240_2F6_240_6_2;
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_parser_xafter__term_6);
  x0 = arg(a0, 1);
  allocp[2] = x0;
  allocp[3] = a2;
  allocp[4] = x1 = makeref(&allocp[4]);
  allocp[5] = a3;
  allocp[6] = a4;
  allocp[7] = a5;
  allocp[8] = NILATOM;
  x3 = arg(a0, 0);
  allocp[9] = x3;
  x2 = makecons(&allocp[8]);
  allocp[10] = x2;
  allocp[11] = a1;
  x4 = makecons(&allocp[10]);
  a0 = x4;
  a1 = x1;
  qp = (struct goalrec*)&allocp[0];
  allocp += 12;
  execute(wrap__functor_2_0);
  goto wrap__functor_2_ext_interrupt;
 case VARREF:
  deref_and_jump(a0,wparse_2F5_244_2F5_240_2F6_240_6_1);
  *reasonp++ =  a0;
  goto wparse_2F5_244_2F5_240_2F6_240_6_2;
  };
  goto wparse_2F5_244_2F5_240_2F6_240_6_2;
 wparse_2F5_244_2F5_240_2F6_240_6_2:
  otherwise(wparse_2F5_244_2F5_240_2F6_240_6_interrupt);
  unify(a5, a0);
  proceed();
 wparse_2F5_244_2F5_240_2F6_240_6_ext_interrupt:
  reasonp = 0l;
 wparse_2F5_244_2F5_240_2F6_240_6_interrupt:
  goto interrupt_6;
 }

 wparse_2F5_243_5_top: {
  q x0, x1, x2, x3;
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  qp = qp->next;
 wparse_2F5_243_5_clear_reason:
  reasonp = reasons;
 wparse_2F5_243_5_0:
 wparse_2F5_243_5_1:
  switch (ptagof(a0)) {
 case CONS:
  x0 = car_of(a0);
 wparse_2F5_243_5_2:
  switch (ptagof(x0)) {
 case FUNCTOR:
  if (functor_of(x0) != makesym(functor_string_1)) goto wparse_2F5_243_5_3;
  x1 = arg(x0, 0);
  generic_arg[0] = x1;
  allocp[0] = x2 = makeref(&allocp[0]);
  generic_arg[1] = x2;
  call_generic(a1, functor_join_2, 1);
  allocp[0] = makesym(functor_string_1);
  allocp[1] = x2;
  x3 = makefunctor(&allocp[0]);
  a1 = cdr_of(a0);
  a0 = x3;
  allocp += 2;
  execute(wparse_5_0);
  goto wparse_5_ext_interrupt;
 case VARREF:
  deref_and_jump(x0,wparse_2F5_243_5_2);
  *reasonp++ =  x0;
  goto wparse_2F5_243_5_3;
  };
  goto wparse_2F5_243_5_3;
 case VARREF:
  deref_and_jump(a0,wparse_2F5_243_5_1);
  *reasonp++ =  a0;
  goto wparse_2F5_243_5_3;
  };
  goto wparse_2F5_243_5_3;
 wparse_2F5_243_5_3:
  otherwise(wparse_2F5_243_5_interrupt);
  allocp[0] = makesym(functor_string_1);
  allocp[1] = a1;
  x0 = makefunctor(&allocp[0]);
  a1 = makeint(0L);
  x1 = a2;
  x2 = a3;
  a5 = a4;
  a4 = x2;
  a3 = x1;
  a2 = x0;
  allocp += 2;
  execute(after__term_6_0);
  goto after__term_6_ext_interrupt;
 wparse_2F5_243_5_ext_interrupt:
  reasonp = 0l;
 wparse_2F5_243_5_interrupt:
  toppred = &predicate_parser_xwparse_2F5_243_5;
  goto interrupt_5;
 }

 wparse_2F5_242_4_top: {
  q x0, x1, x2, x3;
  a2 = qp->args[2];
  a3 = qp->args[3];
  qp = qp->next;
 wparse_2F5_242_4_clear_reason:
  reasonp = reasons;
 wparse_2F5_242_4_0:
 wparse_2F5_242_4_1:
  switch (ptagof(a0)) {
 case CONS:
  x0 = car_of(a0);
 wparse_2F5_242_4_2:
  switch (ptagof(x0)) {
 case ATOMIC:
  if (x0 != makesym(atom__5D)) goto wparse_2F5_242_4_3;
  x1 = cdr_of(a0);
  x2 = a1;
  x3 = a2;
  a4 = a3;
  a3 = x3;
  a2 = x2;
  a1 = x1;
  a0 = makefunctor(funct_const_7);
  execute(wparse_5_0);
  goto wparse_5_ext_interrupt;
 case VARREF:
  deref_and_jump(x0,wparse_2F5_242_4_2);
  *reasonp++ =  x0;
  goto wparse_2F5_242_4_3;
  };
  goto wparse_2F5_242_4_3;
 case VARREF:
  deref_and_jump(a0,wparse_2F5_242_4_1);
  *reasonp++ =  a0;
  goto wparse_2F5_242_4_3;
  };
  goto wparse_2F5_242_4_3;
 wparse_2F5_242_4_3:
  otherwise(wparse_2F5_242_4_interrupt);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_parser_xwparse_2F5_242_2F4_240_4);
  allocp[2] = x0 = makeref(&allocp[2]);
  allocp[3] = a1;
  allocp[4] = a2;
  allocp[5] = a3;
  a1 = a2;
  a2 = makeint(999L);
  a3 = makesym(atom__7C);
  a4 = makesym(atom__5D);
  a5 = x0;
  qp = (struct goalrec*)&allocp[0];
  allocp += 6;
  execute(parse__comma__list_6_0);
  goto parse__comma__list_6_ext_interrupt;
 wparse_2F5_242_4_ext_interrupt:
  reasonp = 0l;
 wparse_2F5_242_4_interrupt:
  toppred = &predicate_parser_xwparse_2F5_242_4;
  goto interrupt_4;
 }

 wparse_2F5_242_2F4_240_4_top: {
  q x0, x1;
  a2 = qp->args[2];
  a3 = qp->args[3];
  qp = qp->next;
 wparse_2F5_242_2F4_240_4_clear_reason:
  reasonp = reasons;
 wparse_2F5_242_2F4_240_4_0:
 wparse_2F5_242_2F4_240_4_1:
  switch (ptagof(a0)) {
 case FUNCTOR:
  if (functor_of(a0) != makesym(functor_normal_2)) goto wparse_2F5_242_2F4_240_4_2;
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_parser_xafter__term_6);
  x0 = arg(a0, 1);
  allocp[2] = x0;
  allocp[3] = makeint(0L);
  allocp[4] = x1 = makeref(&allocp[4]);
  allocp[5] = a1;
  allocp[6] = a2;
  allocp[7] = a3;
  a0 = arg(a0, 0);
  a1 = x1;
  qp = (struct goalrec*)&allocp[0];
  allocp += 8;
  execute(wrap__list_2_0);
  goto wrap__list_2_ext_interrupt;
 case VARREF:
  deref_and_jump(a0,wparse_2F5_242_2F4_240_4_1);
  *reasonp++ =  a0;
  goto wparse_2F5_242_2F4_240_4_2;
  };
  goto wparse_2F5_242_2F4_240_4_2;
 wparse_2F5_242_2F4_240_4_2:
  otherwise(wparse_2F5_242_2F4_240_4_interrupt);
  unify(a3, a0);
  proceed();
 wparse_2F5_242_2F4_240_4_ext_interrupt:
  reasonp = 0l;
 wparse_2F5_242_2F4_240_4_interrupt:
  goto interrupt_4;
 }

 wparse_2F5_241_4_top: {
  q x0, x1, x2, x3, x4;
  a2 = qp->args[2];
  a3 = qp->args[3];
  qp = qp->next;
 wparse_2F5_241_4_clear_reason:
  reasonp = reasons;
 wparse_2F5_241_4_0:
 wparse_2F5_241_4_1:
  switch (ptagof(a0)) {
 case CONS:
  x0 = car_of(a0);
 wparse_2F5_241_4_2:
  switch (ptagof(x0)) {
 case ATOMIC:
  if (x0 != makesym(atom__7D)) goto wparse_2F5_241_4_3;
  x1 = makeint(0L);
  generic_arg[0] = x1;
  new_generic(vector_g_new, 1, x2, 0);
  allocp[0] = makesym(functor_vector_1);
  allocp[1] = x2;
  x3 = makefunctor(&allocp[0]);
  a0 = cdr_of(a0);
  x4 = a1;
  a4 = a2;
  a5 = a3;
  a3 = x4;
  a2 = x3;
  a1 = makeint(0L);
  allocp += 2;
  execute(after__term_6_0);
  goto after__term_6_ext_interrupt;
 case VARREF:
  deref_and_jump(x0,wparse_2F5_241_4_2);
  *reasonp++ =  x0;
  goto wparse_2F5_241_4_3;
  };
  goto wparse_2F5_241_4_3;
 case VARREF:
  deref_and_jump(a0,wparse_2F5_241_4_1);
  *reasonp++ =  a0;
  goto wparse_2F5_241_4_3;
  };
  goto wparse_2F5_241_4_3;
 wparse_2F5_241_4_3:
  otherwise(wparse_2F5_241_4_interrupt);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_parser_xwparse_2F5_241_2F4_240_4);
  allocp[2] = x0 = makeref(&allocp[2]);
  allocp[3] = a1;
  allocp[4] = a2;
  allocp[5] = a3;
  a1 = a2;
  a2 = makeint(999L);
  a3 = makesym(atom_never);
  a4 = makesym(atom__7D);
  a5 = x0;
  qp = (struct goalrec*)&allocp[0];
  allocp += 6;
  execute(parse__comma__list_6_0);
  goto parse__comma__list_6_ext_interrupt;
 wparse_2F5_241_4_ext_interrupt:
  reasonp = 0l;
 wparse_2F5_241_4_interrupt:
  toppred = &predicate_parser_xwparse_2F5_241_4;
  goto interrupt_4;
 }

 wparse_2F5_241_2F4_240_4_top: {
  q x0, x1, x2, x3;
  a2 = qp->args[2];
  a3 = qp->args[3];
  qp = qp->next;
 wparse_2F5_241_2F4_240_4_clear_reason:
  reasonp = reasons;
 wparse_2F5_241_2F4_240_4_0:
 wparse_2F5_241_2F4_240_4_1:
  switch (ptagof(a0)) {
 case FUNCTOR:
  if (functor_of(a0) != makesym(functor_normal_2)) goto wparse_2F5_241_2F4_240_4_2;
  x0 = arg(a0, 0);
  generic_arg[0] = x0;
  new_generic(vector_g_new, 1, x1, 0);
  allocp[0] = makesym(functor_vector_1);
  allocp[1] = x1;
  x2 = makefunctor(&allocp[0]);
  a0 = arg(a0, 1);
  x3 = a1;
  a4 = a2;
  a5 = a3;
  a3 = x3;
  a2 = x2;
  a1 = makeint(0L);
  allocp += 2;
  execute(after__term_6_0);
  goto after__term_6_ext_interrupt;
 case VARREF:
  deref_and_jump(a0,wparse_2F5_241_2F4_240_4_1);
  *reasonp++ =  a0;
  goto wparse_2F5_241_2F4_240_4_2;
  };
  goto wparse_2F5_241_2F4_240_4_2;
 wparse_2F5_241_2F4_240_4_2:
  otherwise(wparse_2F5_241_2F4_240_4_interrupt);
  unify(a3, a0);
  proceed();
 wparse_2F5_241_2F4_240_4_ext_interrupt:
  reasonp = 0l;
 wparse_2F5_241_2F4_240_4_interrupt:
  goto interrupt_4;
 }

 wparse_2F5_240_4_top: {
  q x0, x1, x2, x3;
  a2 = qp->args[2];
  a3 = qp->args[3];
  qp = qp->next;
 wparse_2F5_240_4_clear_reason:
  reasonp = reasons;
 wparse_2F5_240_4_0:
 wparse_2F5_240_4_1:
  switch (ptagof(a0)) {
 case FUNCTOR:
  if (functor_of(a0) != makesym(functor_normal_2)) goto wparse_2F5_240_4_2;
  x0 = arg(a0, 1);
  x1 = arg(a0, 0);
  x2 = a1;
  x3 = a2;
  a4 = a3;
  a3 = x3;
  a2 = x2;
  a1 = x1;
  a0 = x0;
  execute(wparse_2F5_240_2F4_240_5_0);
  goto wparse_2F5_240_2F4_240_5_ext_interrupt;
 case VARREF:
  deref_and_jump(a0,wparse_2F5_240_4_1);
  *reasonp++ =  a0;
  goto wparse_2F5_240_4_2;
  };
  goto wparse_2F5_240_4_2;
 wparse_2F5_240_4_2:
  otherwise(wparse_2F5_240_4_interrupt);
  unify(a3, a0);
  proceed();
 wparse_2F5_240_4_ext_interrupt:
  reasonp = 0l;
 wparse_2F5_240_4_interrupt:
  goto interrupt_4;
 }

 wparse_2F5_240_2F4_240_5_top: {
  q x0, x1, x2, x3;
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  qp = qp->next;
 wparse_2F5_240_2F4_240_5_clear_reason:
  reasonp = reasons;
 wparse_2F5_240_2F4_240_5_0:
 wparse_2F5_240_2F4_240_5_1:
  switch (ptagof(a0)) {
 case CONS:
  x0 = car_of(a0);
 wparse_2F5_240_2F4_240_5_2:
  switch (ptagof(x0)) {
 case ATOMIC:
  if (x0 != makesym(atom__29)) goto wparse_2F5_240_2F4_240_5_3;
  a0 = cdr_of(a0);
  x1 = a1;
  x2 = a2;
  x3 = a3;
  a5 = a4;
  a4 = x3;
  a3 = x2;
  a2 = x1;
  a1 = makeint(0L);
  execute(after__term_6_0);
  goto after__term_6_ext_interrupt;
 case VARREF:
  deref_and_jump(x0,wparse_2F5_240_2F4_240_5_2);
  *reasonp++ =  x0;
  goto wparse_2F5_240_2F4_240_5_3;
  };
  goto wparse_2F5_240_2F4_240_5_3;
 case VARREF:
  deref_and_jump(a0,wparse_2F5_240_2F4_240_5_1);
  *reasonp++ =  a0;
  goto wparse_2F5_240_2F4_240_5_3;
  };
  goto wparse_2F5_240_2F4_240_5_3;
 wparse_2F5_240_2F4_240_5_3:
  otherwise(wparse_2F5_240_2F4_240_5_interrupt);
  allocp[0] = makesym(functor_abnormal_2);
  allocp[1] = makefunctor(&string_const_0);
  allocp[2] = a0;
  x0 = makefunctor(&allocp[0]);
  allocp += 3;
  unify_value(a4, x0);
  proceed();
 wparse_2F5_240_2F4_240_5_ext_interrupt:
  reasonp = 0l;
 wparse_2F5_240_2F4_240_5_interrupt:
  toppred = &predicate_parser_xwparse_2F5_240_2F4_240_5;
  goto interrupt_5;
 }

 after__term_6_top: {
  q x0, x1, x2, x3, x4, x5;
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  qp = qp->next;
 after__term_6_clear_reason:
  reasonp = reasons;
 after__term_6_0:
 after__term_6_1:
  switch (ptagof(a0)) {
 case CONS:
  x0 = car_of(a0);
 after__term_6_2:
  switch (ptagof(x0)) {
 case ATOMIC:
  if (isint(x0)) goto after__term_6_11;
  switch (symval(x0)) {
 case symval(makesym(atom__2C)):
 after__term_6_3:
  if (!isint(a1)) goto after__term_6_6;
  x1 = makeint(1000L);
  gblt_less(a1,x1,after__term_6_11);
 after__term_6_4:
  if (!isint(a3)) goto after__term_6_5;
  x2 = makeint(1000L);
  gblt_greater_or_eq(a3,x2,after__term_6_11);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_parser_xafter__term_2F6_241_5);
  allocp[2] = x3 = makeref(&allocp[2]);
  allocp[3] = a2;
  allocp[4] = a3;
  allocp[5] = a4;
  allocp[6] = a5;
  a0 = cdr_of(a0);
  a1 = makeint(1000L);
  a2 = a4;
  a3 = x3;
  qp = (struct goalrec*)&allocp[0];
  allocp += 7;
  execute(wparse_4_0);
  goto wparse_4_ext_interrupt;
 after__term_6_5:
  if (!isref(a3)) goto after__term_6_11;
  deref_and_jump(a3,after__term_6_4);
  *reasonp++ =  a3;
  goto after__term_6_11;
 after__term_6_6:
  if (!isref(a1)) goto after__term_6_11;
  deref_and_jump(a1,after__term_6_3);
  *reasonp++ =  a1;
  goto after__term_6_11;
 case symval(makesym(atom__7C)):
 after__term_6_7:
  if (!isint(a1)) goto after__term_6_10;
  x1 = makeint(1100L);
  gblt_less(a1,x1,after__term_6_11);
 after__term_6_8:
  if (!isint(a3)) goto after__term_6_9;
  x2 = makeint(1100L);
  gblt_greater_or_eq(a3,x2,after__term_6_11);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_parser_xafter__term_2F6_240_5);
  allocp[2] = x3 = makeref(&allocp[2]);
  allocp[3] = a2;
  allocp[4] = a3;
  allocp[5] = a4;
  allocp[6] = a5;
  a0 = cdr_of(a0);
  a1 = makeint(1100L);
  a2 = a4;
  a3 = x3;
  qp = (struct goalrec*)&allocp[0];
  allocp += 7;
  execute(wparse_4_0);
  goto wparse_4_ext_interrupt;
 after__term_6_9:
  if (!isref(a3)) goto after__term_6_11;
  deref_and_jump(a3,after__term_6_8);
  *reasonp++ =  a3;
  goto after__term_6_11;
 after__term_6_10:
  if (!isref(a1)) goto after__term_6_11;
  deref_and_jump(a1,after__term_6_7);
  *reasonp++ =  a1;
  goto after__term_6_11;
  };
  goto after__term_6_11;
 case FUNCTOR:
  if (functor_of(x0) != makesym(functor_atom_1)) goto after__term_6_11;
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_parser_xafter__term_2F6_242_8);
  allocp[2] = x1 = makeref(&allocp[2]);
  allocp[3] = x2 = makeref(&allocp[3]);
  allocp[4] = a5;
  allocp[5] = a2;
  x3 = arg(x0, 0);
  allocp[6] = x3;
  x4 = cdr_of(a0);
  allocp[7] = x4;
  allocp[8] = a3;
  allocp[9] = a4;
  allocp[10] = (q)(struct goalrec*)&allocp[0];
  allocp[11] = (q)(&predicate_parser_xpostfixop_5);
  allocp[12] = x3;
  allocp[13] = a4;
  allocp[14] = a1;
  allocp[15] = a3;
  allocp[16] = x2;
  a0 = x3;
  x5 = a4;
  a2 = a1;
  a4 = x1;
  a1 = x5;
  qp = (struct goalrec*)&allocp[10];
  allocp += 17;
  execute(infixop_5_0);
  goto infixop_5_ext_interrupt;
 case VARREF:
  deref_and_jump(x0,after__term_6_2);
  *reasonp++ =  x0;
  goto after__term_6_11;
  };
  goto after__term_6_11;
 case VARREF:
  deref_and_jump(a0,after__term_6_1);
  *reasonp++ =  a0;
  goto after__term_6_11;
  };
  goto after__term_6_11;
 after__term_6_11:
  otherwise(after__term_6_interrupt);
  allocp[0] = makesym(functor_normal_2);
  allocp[1] = a2;
  allocp[2] = a0;
  x0 = makefunctor(&allocp[0]);
  allocp += 3;
  unify_value(a5, x0);
  proceed();
 after__term_6_ext_interrupt:
  reasonp = 0l;
 after__term_6_interrupt:
  toppred = &predicate_parser_xafter__term_6;
  goto interrupt_6;
 }

 after__term_2F6_242_8_top: {
  q x0, x1, x2;
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  a6 = qp->args[6];
  a7 = qp->args[7];
  qp = qp->next;
 after__term_2F6_242_8_clear_reason:
  reasonp = reasons;
 after__term_2F6_242_8_0:
 after__term_2F6_242_8_1:
  switch (ptagof(a0)) {
 case ATOMIC:
  if (a0 != makesym(atom_none)) goto after__term_2F6_242_8_interrupt;
  a0 = a1;
  a1 = a2;
  a2 = a3;
  a3 = a4;
  a4 = a5;
  a5 = a6;
  a6 = a7;
  execute(after__term_2F6_242_2F8_241_7_0);
  goto after__term_2F6_242_2F8_241_7_ext_interrupt;
 case FUNCTOR:
  if (functor_of(a0) != makesym(functor_infix_2)) goto after__term_2F6_242_8_interrupt;
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_parser_xafter__term_2F6_242_2F8_240_9);
  allocp[2] = a1;
  allocp[3] = x0 = makeref(&allocp[3]);
  allocp[4] = a4;
  allocp[5] = a3;
  x1 = arg(a0, 0);
  allocp[6] = x1;
  allocp[7] = a6;
  allocp[8] = a7;
  allocp[9] = a2;
  allocp[10] = a5;
  x2 = a5;
  a1 = arg(a0, 1);
  a2 = a7;
  a3 = x0;
  a0 = x2;
  qp = (struct goalrec*)&allocp[0];
  allocp += 11;
  execute(wparse_4_0);
  goto wparse_4_ext_interrupt;
 case VARREF:
  deref_and_jump(a0,after__term_2F6_242_8_1);
  *reasonp++ =  a0;
  goto after__term_2F6_242_8_interrupt;
  };
  goto after__term_2F6_242_8_interrupt;
 after__term_2F6_242_8_ext_interrupt:
  reasonp = 0l;
 after__term_2F6_242_8_interrupt:
  goto interrupt_8;
 }

 after__term_2F6_242_2F8_241_7_top: {
  q x0, x1, x2, x3;
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  a6 = qp->args[6];
  qp = qp->next;
 after__term_2F6_242_2F8_241_7_clear_reason:
  reasonp = reasons;
 after__term_2F6_242_2F8_241_7_0:
 after__term_2F6_242_2F8_241_7_1:
  switch (ptagof(a0)) {
 case ATOMIC:
  if (a0 != makesym(atom_none)) goto after__term_2F6_242_2F8_241_7_interrupt;
  allocp[0] = makesym(functor_atom_1);
  allocp[1] = a3;
  x0 = makefunctor(&allocp[0]);
  allocp[2] = a4;
  allocp[3] = x0;
  x1 = makecons(&allocp[2]);
  allocp[4] = makesym(functor_normal_2);
  allocp[5] = a2;
  allocp[6] = x1;
  x2 = makefunctor(&allocp[4]);
  allocp += 7;
  unify_value(a1, x2);
  proceed();
 case FUNCTOR:
  if (functor_of(a0) != makesym(functor_postfix_1)) goto after__term_2F6_242_2F8_241_7_interrupt;
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_parser_xafter__term_6);
  allocp[2] = a4;
  x0 = arg(a0, 0);
  allocp[3] = x0;
  allocp[4] = x1 = makeref(&allocp[4]);
  allocp[5] = a5;
  allocp[6] = a6;
  allocp[7] = a1;
  allocp[8] = NILATOM;
  allocp[9] = a2;
  x2 = makecons(&allocp[8]);
  allocp[10] = x2;
  allocp[11] = a3;
  x3 = makecons(&allocp[10]);
  a0 = x3;
  a1 = x1;
  qp = (struct goalrec*)&allocp[0];
  allocp += 12;
  execute(wrap__functor_2_0);
  goto wrap__functor_2_ext_interrupt;
 case VARREF:
  deref_and_jump(a0,after__term_2F6_242_2F8_241_7_1);
  *reasonp++ =  a0;
  goto after__term_2F6_242_2F8_241_7_interrupt;
  };
  goto after__term_2F6_242_2F8_241_7_interrupt;
 after__term_2F6_242_2F8_241_7_ext_interrupt:
  reasonp = 0l;
 after__term_2F6_242_2F8_241_7_interrupt:
  toppred = &predicate_parser_xafter__term_2F6_242_2F8_241_7;
  goto interrupt_7;
 }

 after__term_2F6_242_2F8_240_9_top: {
  q x0, x1, x2, x3, x4;
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  a6 = qp->args[6];
  a7 = qp->args[7];
  a8 = qp->args[8];
  qp = qp->next;
 after__term_2F6_242_2F8_240_9_clear_reason:
  reasonp = reasons;
 after__term_2F6_242_2F8_240_9_0:
 after__term_2F6_242_2F8_240_9_1:
  switch (ptagof(a0)) {
 case ATOMIC:
  if (a0 != makesym(atom_none)) goto after__term_2F6_242_2F8_240_9_interrupt;
  a0 = a1;
  a1 = a2;
  a2 = a3;
  a3 = a4;
  a4 = a5;
  a5 = a6;
  a6 = a7;
  execute(after__term_2F6_242_2F8_240_2F9_240_7_0);
  goto after__term_2F6_242_2F8_240_2F9_240_7_ext_interrupt;
 case FUNCTOR:
  if (functor_of(a0) != makesym(functor_postfix_1)) goto after__term_2F6_242_2F8_240_9_interrupt;
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_parser_xselect__better_3);
  allocp[2] = a1;
  allocp[3] = x0 = makeref(&allocp[3]);
  allocp[4] = a7;
  allocp[5] = (q)(struct goalrec*)&allocp[0];
  allocp[6] = (q)(&predicate_parser_xafter__term_6);
  allocp[7] = a8;
  x1 = arg(a0, 0);
  allocp[8] = x1;
  allocp[9] = x2 = makeref(&allocp[9]);
  allocp[10] = a5;
  allocp[11] = a6;
  allocp[12] = x0;
  allocp[13] = NILATOM;
  allocp[14] = a3;
  x3 = makecons(&allocp[13]);
  allocp[15] = x3;
  allocp[16] = a2;
  x4 = makecons(&allocp[15]);
  a0 = x4;
  a1 = x2;
  qp = (struct goalrec*)&allocp[5];
  allocp += 17;
  execute(wrap__functor_2_0);
  goto wrap__functor_2_ext_interrupt;
 case VARREF:
  deref_and_jump(a0,after__term_2F6_242_2F8_240_9_1);
  *reasonp++ =  a0;
  goto after__term_2F6_242_2F8_240_9_interrupt;
  };
  goto after__term_2F6_242_2F8_240_9_interrupt;
 after__term_2F6_242_2F8_240_9_ext_interrupt:
  reasonp = 0l;
 after__term_2F6_242_2F8_240_9_interrupt:
  goto interrupt_9;
 }

 after__term_2F6_242_2F8_240_2F9_240_7_top: {
  q x0, x1, x2, x3, x4, x5;
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  a6 = qp->args[6];
  qp = qp->next;
 after__term_2F6_242_2F8_240_2F9_240_7_clear_reason:
  reasonp = reasons;
 after__term_2F6_242_2F8_240_2F9_240_7_0:
 after__term_2F6_242_2F8_240_2F9_240_7_1:
  switch (ptagof(a0)) {
 case FUNCTOR:
  if (functor_of(a0) != makesym(functor_normal_2)) goto after__term_2F6_242_2F8_240_2F9_240_7_2;
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_parser_xafter__term_6);
  x0 = arg(a0, 1);
  allocp[2] = x0;
  allocp[3] = a3;
  allocp[4] = x1 = makeref(&allocp[4]);
  allocp[5] = a4;
  allocp[6] = a5;
  allocp[7] = a6;
  allocp[8] = NILATOM;
  x3 = arg(a0, 0);
  allocp[9] = x3;
  x2 = makecons(&allocp[8]);
  allocp[10] = x2;
  allocp[11] = a2;
  x4 = makecons(&allocp[10]);
  allocp[12] = x4;
  allocp[13] = a1;
  x5 = makecons(&allocp[12]);
  a0 = x5;
  a1 = x1;
  qp = (struct goalrec*)&allocp[0];
  allocp += 14;
  execute(wrap__functor_2_0);
  goto wrap__functor_2_ext_interrupt;
 case VARREF:
  deref_and_jump(a0,after__term_2F6_242_2F8_240_2F9_240_7_1);
  *reasonp++ =  a0;
  goto after__term_2F6_242_2F8_240_2F9_240_7_2;
  };
  goto after__term_2F6_242_2F8_240_2F9_240_7_2;
 after__term_2F6_242_2F8_240_2F9_240_7_2:
  otherwise(after__term_2F6_242_2F8_240_2F9_240_7_interrupt);
  unify(a6, a0);
  proceed();
 after__term_2F6_242_2F8_240_2F9_240_7_ext_interrupt:
  reasonp = 0l;
 after__term_2F6_242_2F8_240_2F9_240_7_interrupt:
  toppred = &predicate_parser_xafter__term_2F6_242_2F8_240_2F9_240_7;
  goto interrupt_7;
 }

 after__term_2F6_241_5_top: {
  q x0, x1, x2, x3, x4;
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  qp = qp->next;
 after__term_2F6_241_5_clear_reason:
  reasonp = reasons;
 after__term_2F6_241_5_0:
 after__term_2F6_241_5_1:
  switch (ptagof(a0)) {
 case FUNCTOR:
  if (functor_of(a0) != makesym(functor_normal_2)) goto after__term_2F6_241_5_2;
  allocp[0] = makesym(functor__2C_2);
  allocp[1] = a1;
  x1 = arg(a0, 0);
  allocp[2] = x1;
  x0 = makefunctor(&allocp[0]);
  allocp[3] = makesym(functor_functor_1);
  allocp[4] = x0;
  x2 = makefunctor(&allocp[3]);
  a0 = arg(a0, 1);
  a1 = makeint(999L);
  x3 = a2;
  x4 = a3;
  a5 = a4;
  a4 = x4;
  a3 = x3;
  a2 = x2;
  allocp += 5;
  execute(after__term_6_0);
  goto after__term_6_ext_interrupt;
 case VARREF:
  deref_and_jump(a0,after__term_2F6_241_5_1);
  *reasonp++ =  a0;
  goto after__term_2F6_241_5_2;
  };
  goto after__term_2F6_241_5_2;
 after__term_2F6_241_5_2:
  otherwise(after__term_2F6_241_5_interrupt);
  unify(a4, a0);
  proceed();
 after__term_2F6_241_5_ext_interrupt:
  reasonp = 0l;
 after__term_2F6_241_5_interrupt:
  goto interrupt_5;
 }

 after__term_2F6_240_5_top: {
  q x0, x1, x2, x3, x4;
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  qp = qp->next;
 after__term_2F6_240_5_clear_reason:
  reasonp = reasons;
 after__term_2F6_240_5_0:
 after__term_2F6_240_5_1:
  switch (ptagof(a0)) {
 case FUNCTOR:
  if (functor_of(a0) != makesym(functor_normal_2)) goto after__term_2F6_240_5_2;
  allocp[0] = makesym(functor__7C_2);
  allocp[1] = a1;
  x1 = arg(a0, 0);
  allocp[2] = x1;
  x0 = makefunctor(&allocp[0]);
  allocp[3] = makesym(functor_functor_1);
  allocp[4] = x0;
  x2 = makefunctor(&allocp[3]);
  a0 = arg(a0, 1);
  a1 = makeint(1099L);
  x3 = a2;
  x4 = a3;
  a5 = a4;
  a4 = x4;
  a3 = x3;
  a2 = x2;
  allocp += 5;
  execute(after__term_6_0);
  goto after__term_6_ext_interrupt;
 case VARREF:
  deref_and_jump(a0,after__term_2F6_240_5_1);
  *reasonp++ =  a0;
  goto after__term_2F6_240_5_2;
  };
  goto after__term_2F6_240_5_2;
 after__term_2F6_240_5_2:
  otherwise(after__term_2F6_240_5_interrupt);
  unify(a4, a0);
  proceed();
 after__term_2F6_240_5_ext_interrupt:
  reasonp = 0l;
 after__term_2F6_240_5_interrupt:
  goto interrupt_5;
 }

 parse__comma__list_6_top: {
  q x0, x1;
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  qp = qp->next;
 parse__comma__list_6_clear_reason:
  reasonp = reasons;
 parse__comma__list_6_0:
  allocp[0] = x1 = makeref(&allocp[0]);
  x0 = x1;
  a6 = x1;
  a7 = a5;
  a5 = x0;
  allocp += 1;
  execute(parse__comma__list_8_0);
  goto parse__comma__list_8_ext_interrupt;
 parse__comma__list_6_ext_interrupt:
  reasonp = 0l;
 parse__comma__list_6_interrupt:
  toppred = &predicate_parser_xparse__comma__list_6;
  goto interrupt_6;
 }

 parse__comma__list_8_top: {
  q x0, x1;
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  a6 = qp->args[6];
  a7 = qp->args[7];
  qp = qp->next;
 parse__comma__list_8_clear_reason:
  reasonp = reasons;
 parse__comma__list_8_0:
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_parser_xparse__comma__list_2F8_240_8);
  allocp[2] = x0 = makeref(&allocp[2]);
  allocp[3] = a6;
  allocp[4] = a1;
  allocp[5] = a2;
  allocp[6] = a3;
  allocp[7] = a4;
  allocp[8] = a5;
  allocp[9] = a7;
  x1 = a2;
  a2 = a1;
  a3 = x0;
  a1 = x1;
  qp = (struct goalrec*)&allocp[0];
  allocp += 10;
  execute(wparse_4_0);
  goto wparse_4_ext_interrupt;
 parse__comma__list_8_ext_interrupt:
  reasonp = 0l;
 parse__comma__list_8_interrupt:
  toppred = &predicate_parser_xparse__comma__list_8;
  goto interrupt_8;
 }

 parse__comma__list_2F8_240_8_top: {
  q x0, x1, x2;
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  a6 = qp->args[6];
  a7 = qp->args[7];
  qp = qp->next;
 parse__comma__list_2F8_240_8_clear_reason:
  reasonp = reasons;
 parse__comma__list_2F8_240_8_0:
 parse__comma__list_2F8_240_8_1:
  switch (ptagof(a0)) {
 case FUNCTOR:
  if (functor_of(a0) != makesym(functor_normal_2)) goto parse__comma__list_2F8_240_8_2;
  allocp[0] = x1 = makeref(&allocp[0]);
  x2 = arg(a0, 0);
  allocp[1] = x2;
  x0 = makecons(&allocp[0]);
  allocp += 2;
  unify_value(a1, x0);
  a0 = arg(a0, 1);
  a1 = a2;
  a2 = a3;
  a3 = a4;
  a4 = a5;
  a5 = a6;
  a6 = x1;
  execute(parse__comma__list_2F8_240_2F8_240_8_0);
  goto parse__comma__list_2F8_240_2F8_240_8_ext_interrupt;
 case VARREF:
  deref_and_jump(a0,parse__comma__list_2F8_240_8_1);
  *reasonp++ =  a0;
  goto parse__comma__list_2F8_240_8_2;
  };
  goto parse__comma__list_2F8_240_8_2;
 parse__comma__list_2F8_240_8_2:
  otherwise(parse__comma__list_2F8_240_8_interrupt);
  unify(a7, a0);
  proceed();
 parse__comma__list_2F8_240_8_ext_interrupt:
  reasonp = 0l;
 parse__comma__list_2F8_240_8_interrupt:
  goto interrupt_8;
 }

 parse__comma__list_2F8_240_2F8_240_8_top: {
  q x0, x1, x2, x3;
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  a6 = qp->args[6];
  a7 = qp->args[7];
  qp = qp->next;
 parse__comma__list_2F8_240_2F8_240_8_clear_reason:
  reasonp = reasons;
 parse__comma__list_2F8_240_2F8_240_8_0:
 parse__comma__list_2F8_240_2F8_240_8_1:
  switch (ptagof(a0)) {
 case CONS:
  x0 = car_of(a0);
 parse__comma__list_2F8_240_2F8_240_8_2:
  switch (ptagof(x0)) {
 case ATOMIC:
  if (x0 != makesym(atom__2C)) goto parse__comma__list_2F8_240_2F8_240_8_3;
  a0 = cdr_of(a0);
  execute(parse__comma__list_8_0);
  goto parse__comma__list_8_ext_interrupt;
 case VARREF:
  deref_and_jump(x0,parse__comma__list_2F8_240_2F8_240_8_2);
  *reasonp++ =  x0;
  goto parse__comma__list_2F8_240_2F8_240_8_9;
  };
  goto parse__comma__list_2F8_240_2F8_240_8_3;
 parse__comma__list_2F8_240_2F8_240_8_3:
 parse__comma__list_2F8_240_2F8_240_8_4:
  if (isref(x0)) goto parse__comma__list_2F8_240_2F8_240_8_8;
  if_not_equal(x0, a4, parse__comma__list_2F8_240_2F8_240_8_5);
  x1 = NILATOM;
  unify_value(a6, x1);
  allocp[0] = makesym(functor_normal_2);
  allocp[1] = a5;
  x3 = cdr_of(a0);
  allocp[2] = x3;
  x2 = makefunctor(&allocp[0]);
  allocp += 3;
  unify_value(a7, x2);
  proceed();
 parse__comma__list_2F8_240_2F8_240_8_5:
 parse__comma__list_2F8_240_2F8_240_8_6:
  if (isref(x0)) goto parse__comma__list_2F8_240_2F8_240_8_7;
  if_not_equal(x0, a3, parse__comma__list_2F8_240_2F8_240_8_9);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_parser_xparse__comma__list_2F8_240_2F8_240_2F8_240_5);
  allocp[2] = x1 = makeref(&allocp[2]);
  allocp[3] = a6;
  allocp[4] = a4;
  allocp[5] = a7;
  allocp[6] = a5;
  a0 = cdr_of(a0);
  x2 = a2;
  a2 = a1;
  a3 = x1;
  a1 = x2;
  qp = (struct goalrec*)&allocp[0];
  allocp += 7;
  execute(wparse_4_clear_reason);
  goto wparse_4_ext_interrupt;
 parse__comma__list_2F8_240_2F8_240_8_7:
  deref_and_jump(x0,parse__comma__list_2F8_240_2F8_240_8_6);
  *reasonp++ =  x0;
  goto parse__comma__list_2F8_240_2F8_240_8_9;
 parse__comma__list_2F8_240_2F8_240_8_8:
  deref_and_jump(x0,parse__comma__list_2F8_240_2F8_240_8_4);
  *reasonp++ =  x0;
  goto parse__comma__list_2F8_240_2F8_240_8_9;
 case VARREF:
  deref_and_jump(a0,parse__comma__list_2F8_240_2F8_240_8_1);
  *reasonp++ =  a0;
  goto parse__comma__list_2F8_240_2F8_240_8_9;
  };
  goto parse__comma__list_2F8_240_2F8_240_8_9;
 parse__comma__list_2F8_240_2F8_240_8_9:
  otherwise(parse__comma__list_2F8_240_2F8_240_8_interrupt);
  allocp[0] = makesym(functor_abnormal_2);
  allocp[1] = makefunctor(&string_const_1);
  allocp[2] = a0;
  x0 = makefunctor(&allocp[0]);
  allocp += 3;
  unify_value(a7, x0);
  proceed();
 parse__comma__list_2F8_240_2F8_240_8_ext_interrupt:
  reasonp = 0l;
 parse__comma__list_2F8_240_2F8_240_8_interrupt:
  toppred = &predicate_parser_xparse__comma__list_2F8_240_2F8_240_8;
  goto interrupt_8;
 }

 parse__comma__list_2F8_240_2F8_240_2F8_240_5_top: {
  q x0;
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  qp = qp->next;
 parse__comma__list_2F8_240_2F8_240_2F8_240_5_clear_reason:
  reasonp = reasons;
 parse__comma__list_2F8_240_2F8_240_2F8_240_5_0:
 parse__comma__list_2F8_240_2F8_240_2F8_240_5_1:
  switch (ptagof(a0)) {
 case FUNCTOR:
  if (functor_of(a0) != makesym(functor_normal_2)) goto parse__comma__list_2F8_240_2F8_240_2F8_240_5_2;
  x0 = arg(a0, 0);
  unify(a1, x0);
  a0 = arg(a0, 1);
  a1 = a2;
  a2 = a3;
  a3 = a4;
  execute(parse__comma__list_2F8_240_2F8_240_2F8_240_2F5_240_4_0);
  goto parse__comma__list_2F8_240_2F8_240_2F8_240_2F5_240_4_ext_interrupt;
 case VARREF:
  deref_and_jump(a0,parse__comma__list_2F8_240_2F8_240_2F8_240_5_1);
  *reasonp++ =  a0;
  goto parse__comma__list_2F8_240_2F8_240_2F8_240_5_2;
  };
  goto parse__comma__list_2F8_240_2F8_240_2F8_240_5_2;
 parse__comma__list_2F8_240_2F8_240_2F8_240_5_2:
  otherwise(parse__comma__list_2F8_240_2F8_240_2F8_240_5_interrupt);
  unify(a3, a0);
  proceed();
 parse__comma__list_2F8_240_2F8_240_2F8_240_5_ext_interrupt:
  reasonp = 0l;
 parse__comma__list_2F8_240_2F8_240_2F8_240_5_interrupt:
  goto interrupt_5;
 }

 parse__comma__list_2F8_240_2F8_240_2F8_240_2F5_240_4_top: {
  q x0, x1, x2;
  a2 = qp->args[2];
  a3 = qp->args[3];
  qp = qp->next;
 parse__comma__list_2F8_240_2F8_240_2F8_240_2F5_240_4_clear_reason:
  reasonp = reasons;
 parse__comma__list_2F8_240_2F8_240_2F8_240_2F5_240_4_0:
 parse__comma__list_2F8_240_2F8_240_2F8_240_2F5_240_4_1:
  switch (ptagof(a0)) {
 case CONS:
  x0 = car_of(a0);
 parse__comma__list_2F8_240_2F8_240_2F8_240_2F5_240_4_2:
  if (isref(x0)) goto parse__comma__list_2F8_240_2F8_240_2F8_240_2F5_240_4_5;
 parse__comma__list_2F8_240_2F8_240_2F8_240_2F5_240_4_3:
  if (isref(x0)) goto parse__comma__list_2F8_240_2F8_240_2F8_240_2F5_240_4_4;
  if_not_equal(x0, a1, parse__comma__list_2F8_240_2F8_240_2F8_240_2F5_240_4_6);
  allocp[0] = makesym(functor_normal_2);
  allocp[1] = a3;
  x2 = cdr_of(a0);
  allocp[2] = x2;
  x1 = makefunctor(&allocp[0]);
  allocp += 3;
  unify_value(a2, x1);
  proceed();
 parse__comma__list_2F8_240_2F8_240_2F8_240_2F5_240_4_4:
  deref_and_jump(x0,parse__comma__list_2F8_240_2F8_240_2F8_240_2F5_240_4_3);
  *reasonp++ =  x0;
  goto parse__comma__list_2F8_240_2F8_240_2F8_240_2F5_240_4_6;
 parse__comma__list_2F8_240_2F8_240_2F8_240_2F5_240_4_5:
  deref_and_jump(x0,parse__comma__list_2F8_240_2F8_240_2F8_240_2F5_240_4_2);
  *reasonp++ =  x0;
  goto parse__comma__list_2F8_240_2F8_240_2F8_240_2F5_240_4_6;
 case VARREF:
  deref_and_jump(a0,parse__comma__list_2F8_240_2F8_240_2F8_240_2F5_240_4_1);
  *reasonp++ =  a0;
  goto parse__comma__list_2F8_240_2F8_240_2F8_240_2F5_240_4_6;
  };
  goto parse__comma__list_2F8_240_2F8_240_2F8_240_2F5_240_4_6;
 parse__comma__list_2F8_240_2F8_240_2F8_240_2F5_240_4_6:
  otherwise(parse__comma__list_2F8_240_2F8_240_2F8_240_2F5_240_4_interrupt);
  allocp[0] = makesym(functor_abnormal_2);
  allocp[1] = makefunctor(&string_const_0);
  allocp[2] = a0;
  x0 = makefunctor(&allocp[0]);
  allocp += 3;
  unify_value(a2, x0);
  proceed();
 parse__comma__list_2F8_240_2F8_240_2F8_240_2F5_240_4_ext_interrupt:
  reasonp = 0l;
 parse__comma__list_2F8_240_2F8_240_2F8_240_2F5_240_4_interrupt:
  toppred = &predicate_parser_xparse__comma__list_2F8_240_2F8_240_2F8_240_2F5_240_4;
  goto interrupt_4;
 }

 list__to__comma__list_2_top: {
  q x0, x1, x2, x3, x4;
  qp = qp->next;
 list__to__comma__list_2_clear_reason:
  reasonp = reasons;
 list__to__comma__list_2_0:
 list__to__comma__list_2_1:
  switch (ptagof(a0)) {
 case CONS:
  x0 = cdr_of(a0);
 list__to__comma__list_2_2:
  switch (ptagof(x0)) {
 case ATOMIC:
  if (x0 != NILATOM) goto list__to__comma__list_2_3;
  x1 = car_of(a0);
  unify(a1, x1);
  proceed();
 case VARREF:
  deref_and_jump(x0,list__to__comma__list_2_2);
  *reasonp++ =  x0;
  goto list__to__comma__list_2_interrupt;
  };
  goto list__to__comma__list_2_3;
 list__to__comma__list_2_3:
  x1 = NILATOM;
  gblt_diff(x0,x1,list__to__comma__list_2_interrupt);
  allocp[0] = makesym(functor__2C_2);
  x3 = car_of(a0);
  allocp[1] = x3;
  allocp[2] = x4 = makeref(&allocp[2]);
  x2 = makefunctor(&allocp[0]);
  allocp += 3;
  unify_value(a1, x2);
  a0 = x0;
  a1 = x4;
  execute(list__to__comma__list_2_0);
  goto list__to__comma__list_2_ext_interrupt;
 case VARREF:
  deref_and_jump(a0,list__to__comma__list_2_1);
  *reasonp++ =  a0;
  goto list__to__comma__list_2_interrupt;
  };
  goto list__to__comma__list_2_interrupt;
 list__to__comma__list_2_ext_interrupt:
  reasonp = 0l;
 list__to__comma__list_2_interrupt:
  goto interrupt_2;
 }

 select__better_3_top: {
  q x0;
  a2 = qp->args[2];
  qp = qp->next;
 select__better_3_clear_reason:
  reasonp = reasons;
 select__better_3_0:
 select__better_3_1:
  switch (ptagof(a0)) {
 case FUNCTOR:
  switch (symval(functor_of(a0))) {
 case functor_normal_2:
 select__better_3_2:
  switch (ptagof(a1)) {
 case FUNCTOR:
  switch (symval(functor_of(a1))) {
 case functor_normal_2:
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_parser_xselect__better_2F3_241_4);
  allocp[2] = x0 = makeref(&allocp[2]);
  allocp[3] = a2;
  allocp[4] = a0;
  allocp[5] = a1;
  a0 = arg(a0, 1);
  a1 = arg(a1, 1);
  a2 = x0;
  qp = (struct goalrec*)&allocp[0];
  allocp += 6;
  execute(longer_3_0);
  goto longer_3_ext_interrupt;
 case functor_abnormal_2:
  unify_value(a2, a0);
  proceed();
  };
  goto select__better_3_interrupt;
 case VARREF:
  deref_and_jump(a1,select__better_3_2);
  *reasonp++ =  a1;
  goto select__better_3_interrupt;
  };
  goto select__better_3_interrupt;
 case functor_abnormal_2:
 select__better_3_3:
  switch (ptagof(a1)) {
 case FUNCTOR:
  switch (symval(functor_of(a1))) {
 case functor_normal_2:
  unify_value(a2, a1);
  proceed();
 case functor_abnormal_2:
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_parser_xselect__better_2F3_240_4);
  allocp[2] = x0 = makeref(&allocp[2]);
  allocp[3] = a2;
  allocp[4] = a0;
  allocp[5] = a1;
  a0 = arg(a0, 1);
  a1 = arg(a1, 1);
  a2 = x0;
  qp = (struct goalrec*)&allocp[0];
  allocp += 6;
  execute(longer_3_0);
  goto longer_3_ext_interrupt;
  };
  goto select__better_3_interrupt;
 case VARREF:
  deref_and_jump(a1,select__better_3_3);
  *reasonp++ =  a1;
  goto select__better_3_interrupt;
  };
  goto select__better_3_interrupt;
  };
  goto select__better_3_interrupt;
 case VARREF:
  deref_and_jump(a0,select__better_3_1);
  *reasonp++ =  a0;
  goto select__better_3_interrupt;
  };
  goto select__better_3_interrupt;
 select__better_3_ext_interrupt:
  reasonp = 0l;
 select__better_3_interrupt:
  goto interrupt_3;
 }

 select__better_2F3_241_4_top: {
  a2 = qp->args[2];
  a3 = qp->args[3];
  qp = qp->next;
 select__better_2F3_241_4_clear_reason:
  reasonp = reasons;
 select__better_2F3_241_4_0:
 select__better_2F3_241_4_1:
  switch (ptagof(a0)) {
 case ATOMIC:
  if (isint(a0)) goto select__better_2F3_241_4_interrupt;
  switch (symval(a0)) {
 case symval(makesym(atom_no)):
  unify(a1, a2);
  proceed();
 case symval(makesym(atom_yes)):
  unify(a1, a3);
  proceed();
  };
  goto select__better_2F3_241_4_interrupt;
 case VARREF:
  deref_and_jump(a0,select__better_2F3_241_4_1);
  *reasonp++ =  a0;
  goto select__better_2F3_241_4_interrupt;
  };
  goto select__better_2F3_241_4_interrupt;
 select__better_2F3_241_4_ext_interrupt:
  reasonp = 0l;
 select__better_2F3_241_4_interrupt:
  goto interrupt_4;
 }

 select__better_2F3_240_4_top: {
  a2 = qp->args[2];
  a3 = qp->args[3];
  qp = qp->next;
 select__better_2F3_240_4_clear_reason:
  reasonp = reasons;
 select__better_2F3_240_4_0:
 select__better_2F3_240_4_1:
  switch (ptagof(a0)) {
 case ATOMIC:
  if (isint(a0)) goto select__better_2F3_240_4_interrupt;
  switch (symval(a0)) {
 case symval(makesym(atom_no)):
  unify(a1, a2);
  proceed();
 case symval(makesym(atom_yes)):
  unify(a1, a3);
  proceed();
  };
  goto select__better_2F3_240_4_interrupt;
 case VARREF:
  deref_and_jump(a0,select__better_2F3_240_4_1);
  *reasonp++ =  a0;
  goto select__better_2F3_240_4_interrupt;
  };
  goto select__better_2F3_240_4_interrupt;
 select__better_2F3_240_4_ext_interrupt:
  reasonp = 0l;
 select__better_2F3_240_4_interrupt:
  goto interrupt_4;
 }

 prefixop_4_top: {
  q x0, x1, x2;
  a2 = qp->args[2];
  a3 = qp->args[3];
  qp = qp->next;
 prefixop_4_clear_reason:
  reasonp = reasons;
 prefixop_4_0:
 prefixop_4_1:
  switch (ptagof(a1)) {
 case FUNCTOR:
  if (functor_of(a1) != makesym(functor_ops_3)) goto prefixop_4_interrupt;
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_parser_xprefixop_2F4_240_4);
  allocp[2] = x0 = makeref(&allocp[2]);
  allocp[3] = a2;
  allocp[4] = a3;
  allocp[5] = x1 = makeref(&allocp[5]);
  x2 = arg(a1, 0);
  a1 = a0;
  a2 = x1;
  a3 = x0;
  a0 = x2;
  qp = (struct goalrec*)&allocp[0];
  allocp += 6;
  execute(lookup__op_4_0);
  goto lookup__op_4_ext_interrupt;
 case VARREF:
  deref_and_jump(a1,prefixop_4_1);
  *reasonp++ =  a1;
  goto prefixop_4_interrupt;
  };
  goto prefixop_4_interrupt;
 prefixop_4_ext_interrupt:
  reasonp = 0l;
 prefixop_4_interrupt:
  toppred = &predicate_parser_xprefixop_4;
  goto interrupt_4;
 }

 prefixop_2F4_240_4_top: {
  q x0, x1, x2;
  a2 = qp->args[2];
  a3 = qp->args[3];
  qp = qp->next;
 prefixop_2F4_240_4_clear_reason:
  reasonp = reasons;
 prefixop_2F4_240_4_0:
 prefixop_2F4_240_4_1:
  if (!isint(a0)) goto prefixop_2F4_240_4_4;
 prefixop_2F4_240_4_2:
  if (!isint(a1)) goto prefixop_2F4_240_4_3;
  gblt_greater(a0,a1,prefixop_2F4_240_4_5);
  x0 = makesym(atom_none);
  unify_value(a2, x0);
  proceed();
 prefixop_2F4_240_4_3:
  if (!isref(a1)) goto prefixop_2F4_240_4_5;
  deref_and_jump(a1,prefixop_2F4_240_4_2);
  *reasonp++ =  a1;
  goto prefixop_2F4_240_4_5;
 prefixop_2F4_240_4_4:
  if (!isref(a0)) goto prefixop_2F4_240_4_5;
  deref_and_jump(a0,prefixop_2F4_240_4_1);
  *reasonp++ =  a0;
  goto prefixop_2F4_240_4_5;
 prefixop_2F4_240_4_5:
  otherwise(prefixop_2F4_240_4_interrupt);
 prefixop_2F4_240_4_6:
  switch (ptagof(a3)) {
 case ATOMIC:
  if (isint(a3)) goto prefixop_2F4_240_4_interrupt;
  switch (symval(a3)) {
 case symval(makesym(atom_fx)):
  x0 = makeint(1L);
  bblt_sub_no_check_y(a0,x0,x1);
  allocp[0] = makesym(functor_prefix_2);
  allocp[1] = a0;
  allocp[2] = x1;
  x2 = makefunctor(&allocp[0]);
  allocp += 3;
  unify_value(a2, x2);
  proceed();
 case symval(makesym(atom_fy)):
  allocp[0] = makesym(functor_prefix_2);
  allocp[1] = a0;
  allocp[2] = a0;
  x0 = makefunctor(&allocp[0]);
  allocp += 3;
  unify_value(a2, x0);
  proceed();
  };
  goto prefixop_2F4_240_4_interrupt;
 case VARREF:
  deref_and_jump(a3,prefixop_2F4_240_4_6);
  *reasonp++ =  a3;
  goto prefixop_2F4_240_4_interrupt;
  };
  goto prefixop_2F4_240_4_interrupt;
 prefixop_2F4_240_4_ext_interrupt:
  reasonp = 0l;
 prefixop_2F4_240_4_interrupt:
  goto interrupt_4;
 }

 infixop_5_top: {
  q x0, x1, x2;
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  qp = qp->next;
 infixop_5_clear_reason:
  reasonp = reasons;
 infixop_5_0:
 infixop_5_1:
  switch (ptagof(a1)) {
 case FUNCTOR:
  if (functor_of(a1) != makesym(functor_ops_3)) goto infixop_5_interrupt;
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_parser_xinfixop_2F5_240_5);
  allocp[2] = x0 = makeref(&allocp[2]);
  allocp[3] = x1 = makeref(&allocp[3]);
  allocp[4] = a2;
  allocp[5] = a3;
  allocp[6] = a4;
  x2 = arg(a1, 1);
  a1 = a0;
  a2 = x0;
  a3 = x1;
  a0 = x2;
  qp = (struct goalrec*)&allocp[0];
  allocp += 7;
  execute(lookup__op_4_0);
  goto lookup__op_4_ext_interrupt;
 case VARREF:
  deref_and_jump(a1,infixop_5_1);
  *reasonp++ =  a1;
  goto infixop_5_interrupt;
  };
  goto infixop_5_interrupt;
 infixop_5_ext_interrupt:
  reasonp = 0l;
 infixop_5_interrupt:
  toppred = &predicate_parser_xinfixop_5;
  goto interrupt_5;
 }

 infixop_2F5_240_5_top: {
  q x0, x1, x2;
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  qp = qp->next;
 infixop_2F5_240_5_clear_reason:
  reasonp = reasons;
 infixop_2F5_240_5_0:
 infixop_2F5_240_5_1:
  switch (ptagof(a0)) {
 case ATOMIC:
  if (isint(a0)) goto infixop_2F5_240_5_20;
  switch (symval(a0)) {
 case symval(makesym(atom_xfx)):
 infixop_2F5_240_5_2:
  if (!isint(a1)) goto infixop_2F5_240_5_7;
 infixop_2F5_240_5_3:
  if (!isint(a2)) goto infixop_2F5_240_5_6;
  gblt_greater(a1,a2,infixop_2F5_240_5_20);
 infixop_2F5_240_5_4:
  if (!isint(a3)) goto infixop_2F5_240_5_5;
  gblt_less_or_eq(a1,a3,infixop_2F5_240_5_20);
  x0 = makeint(1L);
  bblt_sub_no_check(a1,x0,x1);
  allocp[0] = makesym(functor_infix_2);
  allocp[1] = a1;
  allocp[2] = x1;
  x2 = makefunctor(&allocp[0]);
  allocp += 3;
  unify_value(a4, x2);
  proceed();
 infixop_2F5_240_5_5:
  if (!isref(a3)) goto infixop_2F5_240_5_20;
  deref_and_jump(a3,infixop_2F5_240_5_4);
  *reasonp++ =  a3;
  goto infixop_2F5_240_5_20;
 infixop_2F5_240_5_6:
  if (!isref(a2)) goto infixop_2F5_240_5_20;
  deref_and_jump(a2,infixop_2F5_240_5_3);
  *reasonp++ =  a2;
  goto infixop_2F5_240_5_20;
 infixop_2F5_240_5_7:
  if (!isref(a1)) goto infixop_2F5_240_5_20;
  deref_and_jump(a1,infixop_2F5_240_5_2);
  *reasonp++ =  a1;
  goto infixop_2F5_240_5_20;
 case symval(makesym(atom_xfy)):
 infixop_2F5_240_5_8:
  if (!isint(a1)) goto infixop_2F5_240_5_13;
 infixop_2F5_240_5_9:
  if (!isint(a2)) goto infixop_2F5_240_5_12;
  gblt_greater(a1,a2,infixop_2F5_240_5_20);
 infixop_2F5_240_5_10:
  if (!isint(a3)) goto infixop_2F5_240_5_11;
  gblt_less_or_eq(a1,a3,infixop_2F5_240_5_20);
  allocp[0] = makesym(functor_infix_2);
  allocp[1] = a1;
  allocp[2] = a1;
  x0 = makefunctor(&allocp[0]);
  allocp += 3;
  unify_value(a4, x0);
  proceed();
 infixop_2F5_240_5_11:
  if (!isref(a3)) goto infixop_2F5_240_5_20;
  deref_and_jump(a3,infixop_2F5_240_5_10);
  *reasonp++ =  a3;
  goto infixop_2F5_240_5_20;
 infixop_2F5_240_5_12:
  if (!isref(a2)) goto infixop_2F5_240_5_20;
  deref_and_jump(a2,infixop_2F5_240_5_9);
  *reasonp++ =  a2;
  goto infixop_2F5_240_5_20;
 infixop_2F5_240_5_13:
  if (!isref(a1)) goto infixop_2F5_240_5_20;
  deref_and_jump(a1,infixop_2F5_240_5_8);
  *reasonp++ =  a1;
  goto infixop_2F5_240_5_20;
 case symval(makesym(atom_yfx)):
 infixop_2F5_240_5_14:
  if (!isint(a1)) goto infixop_2F5_240_5_19;
 infixop_2F5_240_5_15:
  if (!isint(a2)) goto infixop_2F5_240_5_18;
  gblt_greater_or_eq(a1,a2,infixop_2F5_240_5_20);
 infixop_2F5_240_5_16:
  if (!isint(a3)) goto infixop_2F5_240_5_17;
  gblt_less(a1,a3,infixop_2F5_240_5_20);
  x0 = makeint(1L);
  bblt_sub_no_check(a1,x0,x1);
  allocp[0] = makesym(functor_infix_2);
  allocp[1] = a1;
  allocp[2] = x1;
  x2 = makefunctor(&allocp[0]);
  allocp += 3;
  unify_value(a4, x2);
  proceed();
 infixop_2F5_240_5_17:
  if (!isref(a3)) goto infixop_2F5_240_5_20;
  deref_and_jump(a3,infixop_2F5_240_5_16);
  *reasonp++ =  a3;
  goto infixop_2F5_240_5_20;
 infixop_2F5_240_5_18:
  if (!isref(a2)) goto infixop_2F5_240_5_20;
  deref_and_jump(a2,infixop_2F5_240_5_15);
  *reasonp++ =  a2;
  goto infixop_2F5_240_5_20;
 infixop_2F5_240_5_19:
  if (!isref(a1)) goto infixop_2F5_240_5_20;
  deref_and_jump(a1,infixop_2F5_240_5_14);
  *reasonp++ =  a1;
  goto infixop_2F5_240_5_20;
  };
  goto infixop_2F5_240_5_20;
 case VARREF:
  deref_and_jump(a0,infixop_2F5_240_5_1);
  *reasonp++ =  a0;
  goto infixop_2F5_240_5_20;
  };
  goto infixop_2F5_240_5_20;
 infixop_2F5_240_5_20:
  otherwise(infixop_2F5_240_5_interrupt);
  x0 = makesym(atom_none);
  unify_value(a4, x0);
  proceed();
 infixop_2F5_240_5_ext_interrupt:
  reasonp = 0l;
 infixop_2F5_240_5_interrupt:
  goto interrupt_5;
 }

 postfixop_5_top: {
  q x0, x1, x2;
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  qp = qp->next;
 postfixop_5_clear_reason:
  reasonp = reasons;
 postfixop_5_0:
 postfixop_5_1:
  switch (ptagof(a1)) {
 case FUNCTOR:
  if (functor_of(a1) != makesym(functor_ops_3)) goto postfixop_5_interrupt;
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_parser_xpostfixop_2F5_240_5);
  allocp[2] = x0 = makeref(&allocp[2]);
  allocp[3] = a3;
  allocp[4] = a4;
  allocp[5] = x1 = makeref(&allocp[5]);
  allocp[6] = a2;
  x2 = arg(a1, 2);
  a1 = a0;
  a2 = x1;
  a3 = x0;
  a0 = x2;
  qp = (struct goalrec*)&allocp[0];
  allocp += 7;
  execute(lookup__op_4_0);
  goto lookup__op_4_ext_interrupt;
 case VARREF:
  deref_and_jump(a1,postfixop_5_1);
  *reasonp++ =  a1;
  goto postfixop_5_interrupt;
  };
  goto postfixop_5_interrupt;
 postfixop_5_ext_interrupt:
  reasonp = 0l;
 postfixop_5_interrupt:
  goto interrupt_5;
 }

 postfixop_2F5_240_5_top: {
  q x0;
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  qp = qp->next;
 postfixop_2F5_240_5_clear_reason:
  reasonp = reasons;
 postfixop_2F5_240_5_0:
 postfixop_2F5_240_5_1:
  if (!isint(a0)) goto postfixop_2F5_240_5_4;
 postfixop_2F5_240_5_2:
  if (!isint(a1)) goto postfixop_2F5_240_5_3;
  gblt_greater(a0,a1,postfixop_2F5_240_5_5);
  x0 = makesym(atom_none);
  unify_value(a2, x0);
  proceed();
 postfixop_2F5_240_5_3:
  if (!isref(a1)) goto postfixop_2F5_240_5_5;
  deref_and_jump(a1,postfixop_2F5_240_5_2);
  *reasonp++ =  a1;
  goto postfixop_2F5_240_5_5;
 postfixop_2F5_240_5_4:
  if (!isref(a0)) goto postfixop_2F5_240_5_5;
  deref_and_jump(a0,postfixop_2F5_240_5_1);
  *reasonp++ =  a0;
  goto postfixop_2F5_240_5_5;
 postfixop_2F5_240_5_5:
  otherwise(postfixop_2F5_240_5_interrupt);
 postfixop_2F5_240_5_6:
  switch (ptagof(a3)) {
 case ATOMIC:
  if (isint(a3)) goto postfixop_2F5_240_5_interrupt;
  switch (symval(a3)) {
 case symval(makesym(atom_xf)):
 postfixop_2F5_240_5_7:
  if (!isint(a0)) goto postfixop_2F5_240_5_10;
 postfixop_2F5_240_5_8:
  if (!isint(a4)) goto postfixop_2F5_240_5_9;
  gblt_greater(a0,a4,postfixop_2F5_240_5_interrupt);
  allocp[0] = makesym(functor_postfix_1);
  allocp[1] = a0;
  x0 = makefunctor(&allocp[0]);
  allocp += 2;
  unify_value(a2, x0);
  proceed();
 postfixop_2F5_240_5_9:
  if (!isref(a4)) goto postfixop_2F5_240_5_interrupt;
  deref_and_jump(a4,postfixop_2F5_240_5_8);
  *reasonp++ =  a4;
  goto postfixop_2F5_240_5_interrupt;
 postfixop_2F5_240_5_10:
  if (!isref(a0)) goto postfixop_2F5_240_5_interrupt;
  deref_and_jump(a0,postfixop_2F5_240_5_7);
  *reasonp++ =  a0;
  goto postfixop_2F5_240_5_interrupt;
 case symval(makesym(atom_yf)):
 postfixop_2F5_240_5_11:
  if (!isint(a0)) goto postfixop_2F5_240_5_14;
 postfixop_2F5_240_5_12:
  if (!isint(a4)) goto postfixop_2F5_240_5_13;
  gblt_greater_or_eq(a0,a4,postfixop_2F5_240_5_interrupt);
  allocp[0] = makesym(functor_postfix_1);
  allocp[1] = a0;
  x0 = makefunctor(&allocp[0]);
  allocp += 2;
  unify_value(a2, x0);
  proceed();
 postfixop_2F5_240_5_13:
  if (!isref(a4)) goto postfixop_2F5_240_5_interrupt;
  deref_and_jump(a4,postfixop_2F5_240_5_12);
  *reasonp++ =  a4;
  goto postfixop_2F5_240_5_interrupt;
 postfixop_2F5_240_5_14:
  if (!isref(a0)) goto postfixop_2F5_240_5_interrupt;
  deref_and_jump(a0,postfixop_2F5_240_5_11);
  *reasonp++ =  a0;
  goto postfixop_2F5_240_5_interrupt;
  };
  goto postfixop_2F5_240_5_interrupt;
 case VARREF:
  deref_and_jump(a3,postfixop_2F5_240_5_6);
  *reasonp++ =  a3;
  goto postfixop_2F5_240_5_interrupt;
  };
  goto postfixop_2F5_240_5_interrupt;
 postfixop_2F5_240_5_ext_interrupt:
  reasonp = 0l;
 postfixop_2F5_240_5_interrupt:
  goto interrupt_5;
 }

 lookup__op_4_top: {
  q x0, x1, x2, x3;
  a2 = qp->args[2];
  a3 = qp->args[3];
  qp = qp->next;
 lookup__op_4_clear_reason:
  reasonp = reasons;
 lookup__op_4_0:
 lookup__op_4_1:
  switch (ptagof(a0)) {
 case CONS:
  x0 = car_of(a0);
 lookup__op_4_2:
  switch (ptagof(x0)) {
 case FUNCTOR:
  if (functor_of(x0) != makesym(functor_op_3)) goto lookup__op_4_interrupt;
  x1 = arg(x0, 0);
 lookup__op_4_3:
  if (isref(x1)) goto lookup__op_4_9;
 lookup__op_4_4:
  if (isref(x1)) goto lookup__op_4_5;
  if_not_equal(x1, a1, lookup__op_4_6);
  x2 = arg(x0, 1);
  unify(a2, x2);
  x3 = arg(x0, 2);
  unify(a3, x3);
  proceed();
 lookup__op_4_5:
  deref_and_jump(x1,lookup__op_4_4);
  *reasonp++ =  x1;
  goto lookup__op_4_6;
 lookup__op_4_6:
 lookup__op_4_7:
  if (isref(a1)) goto lookup__op_4_8;
  gblt_diff(x1,a1,lookup__op_4_interrupt);
  a0 = cdr_of(a0);
  execute(lookup__op_4_clear_reason);
  goto lookup__op_4_ext_interrupt;
 lookup__op_4_8:
  deref_and_jump(a1,lookup__op_4_7);
  *reasonp++ =  a1;
  goto lookup__op_4_interrupt;
 lookup__op_4_9:
  deref_and_jump(x1,lookup__op_4_3);
  *reasonp++ =  x1;
  goto lookup__op_4_interrupt;
 case VARREF:
  deref_and_jump(x0,lookup__op_4_2);
  *reasonp++ =  x0;
  goto lookup__op_4_interrupt;
  };
  goto lookup__op_4_interrupt;
 case ATOMIC:
  if (a0 != NILATOM) goto lookup__op_4_interrupt;
  x0 = makesym(atom_none);
  unify_value(a2, x0);
  x1 = makeint(9999L);
  unify_value(a3, x1);
  proceed();
 case VARREF:
  deref_and_jump(a0,lookup__op_4_1);
  *reasonp++ =  a0;
  goto lookup__op_4_interrupt;
  };
  goto lookup__op_4_interrupt;
 lookup__op_4_ext_interrupt:
  reasonp = 0l;
 lookup__op_4_interrupt:
  toppred = &predicate_parser_xlookup__op_4;
  goto interrupt_4;
 }

 longer_3_top: {
  q x0;
  a2 = qp->args[2];
  qp = qp->next;
 longer_3_clear_reason:
  reasonp = reasons;
 longer_3_0:
 longer_3_1:
  switch (ptagof(a0)) {
 case CONS:
 longer_3_2:
  switch (ptagof(a1)) {
 case CONS:
  a0 = cdr_of(a0);
  a1 = cdr_of(a1);
  execute(longer_3_0);
  goto longer_3_ext_interrupt;
 case ATOMIC:
  if (a1 != NILATOM) goto longer_3_interrupt;
  x0 = makesym(atom_yes);
  unify_value(a2, x0);
  proceed();
 case VARREF:
  deref_and_jump(a1,longer_3_2);
  *reasonp++ =  a1;
  goto longer_3_interrupt;
  };
  goto longer_3_interrupt;
 case ATOMIC:
  if (a0 != NILATOM) goto longer_3_interrupt;
  x0 = makesym(atom_no);
  unify_value(a2, x0);
  proceed();
 case VARREF:
  deref_and_jump(a0,longer_3_1);
  *reasonp++ =  a0;
  goto longer_3_interrupt;
  };
  goto longer_3_interrupt;
 longer_3_ext_interrupt:
  reasonp = 0l;
 longer_3_interrupt:
  toppred = &predicate_parser_xlonger_3;
  goto interrupt_3;
 }

 wrap__list_2_top: {
  q x0, x1, x2, x3;
  qp = qp->next;
 wrap__list_2_clear_reason:
  reasonp = reasons;
 wrap__list_2_0:
 wrap__list_2_1:
  switch (ptagof(a0)) {
 case CONS:
  allocp[0] = x1 = makeref(&allocp[0]);
  x2 = car_of(a0);
  allocp[1] = x2;
  x0 = makecons(&allocp[0]);
  allocp[2] = makesym(functor_list_1);
  allocp[3] = x0;
  x3 = makefunctor(&allocp[2]);
  allocp += 4;
  unify_value(a1, x3);
  a0 = cdr_of(a0);
  a1 = x1;
  execute(wrap__list_2_0);
  goto wrap__list_2_ext_interrupt;
 case ATOMIC:
  if (a0 != NILATOM) goto wrap__list_2_2;
  x0 = makefunctor(funct_const_7);
  unify_value(a1, x0);
  proceed();
 case VARREF:
  deref_and_jump(a0,wrap__list_2_1);
  *reasonp++ =  a0;
  goto wrap__list_2_2;
  };
  goto wrap__list_2_2;
 wrap__list_2_2:
  otherwise(wrap__list_2_interrupt);
  unify(a1, a0);
  proceed();
 wrap__list_2_ext_interrupt:
  reasonp = 0l;
 wrap__list_2_interrupt:
  toppred = &predicate_parser_xwrap__list_2;
  goto interrupt_2;
 }

 wrap__functor_2_top: {
  q x0;
  qp = qp->next;
 wrap__functor_2_clear_reason:
  reasonp = reasons;
 wrap__functor_2_0:
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_parser_xwrap__functor1_2);
  allocp[2] = x0 = makeref(&allocp[2]);
  allocp[3] = a1;
  allocp[4] = (q)(struct goalrec*)&allocp[0];
  allocp[5] = (q)(&predicate_functor__table_x_3D_2E_2E_2);
  allocp[6] = x0;
  allocp[7] = a0;
  qp = (struct goalrec*)&allocp[4];
  allocp += 8;
  proceed();
 wrap__functor_2_ext_interrupt:
  reasonp = 0l;
 wrap__functor_2_interrupt:
  toppred = &predicate_parser_xwrap__functor_2;
  goto interrupt_2;
 }

 wrap__functor1_2_top: {
  q x0;
  qp = qp->next;
 wrap__functor1_2_clear_reason:
  reasonp = reasons;
 wrap__functor1_2_0:
 wrap__functor1_2_1:
  switch (ptagof(a0)) {
 case CONS:
  allocp[0] = makesym(functor_list_1);
  allocp[1] = a0;
  x0 = makefunctor(&allocp[0]);
  allocp += 2;
  unify_value(a1, x0);
  proceed();
 case VARREF:
  deref_and_jump(a0,wrap__functor1_2_1);
  *reasonp++ =  a0;
  goto wrap__functor1_2_2;
  };
  goto wrap__functor1_2_2;
 wrap__functor1_2_2:
  otherwise(wrap__functor1_2_interrupt);
  allocp[0] = makesym(functor_functor_1);
  allocp[1] = a0;
  x0 = makefunctor(&allocp[0]);
  allocp += 2;
  unify_value(a1, x0);
  proceed();
 wrap__functor1_2_ext_interrupt:
  reasonp = 0l;
 wrap__functor1_2_interrupt:
  goto interrupt_2;
 }

 unwrap_2_top: {
  qp = qp->next;
 unwrap_2_clear_reason:
  reasonp = reasons;
 unwrap_2_0:
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_variable_xunwrap_2);
  allocp[2] = a0;
  allocp[3] = a1;
  qp = (struct goalrec*)&allocp[0];
  allocp += 4;
  proceed();
 unwrap_2_ext_interrupt:
  reasonp = 0l;
 unwrap_2_interrupt:
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
  loop_within_module(module_parser);
}
