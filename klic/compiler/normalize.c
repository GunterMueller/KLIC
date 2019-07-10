/* Compiled by KLIC compiler version 3.003 (Mon Mar 29 19:42:59 JST 1999) */
#include <klic/klichdr.h>
#include "atom.h"
#include "funct.h"

module module_klic__comp__normalize();
Const struct predicate predicate_klic__comp__normalize_xgenerate_8 =
   { module_klic__comp__normalize, 0, 8 };
Const struct predicate predicate_klic__comp__normalize_xmake__arg__assoc_3 =
   { module_klic__comp__normalize, 1, 3 };
Const struct predicate predicate_klic__comp__normalize_xseparate__cond_5 =
   { module_klic__comp__normalize, 2, 5 };
Const struct predicate predicate_klic__comp__normalize_xseparate__cond__1_9 =
   { module_klic__comp__normalize, 3, 9 };
Const struct predicate predicate_klic__comp__normalize_xnorm__head_5 =
   { module_klic__comp__normalize, 4, 5 };
Const struct predicate predicate_klic__comp__normalize_xnorm__args_8 =
   { module_klic__comp__normalize, 5, 8 };
Const struct predicate predicate_klic__comp__normalize_xnorm__guard__term_6 =
   { module_klic__comp__normalize, 6, 6 };
Const struct predicate predicate_klic__comp__normalize_xnorm__guard__term__1_7 =
   { module_klic__comp__normalize, 7, 7 };
Const struct predicate predicate_klic__comp__normalize_xnorm__guard_10 =
   { module_klic__comp__normalize, 8, 10 };
Const struct predicate predicate_klic__comp__normalize_xnorm__guard_2F10_241_11 =
   { module_klic__comp__normalize, 9, 11 };
Const struct predicate predicate_klic__comp__normalize_xnorm__guard_2F10_240_11 =
   { module_klic__comp__normalize, 10, 11 };
Const struct predicate predicate_klic__comp__normalize_xnorm__guard__goal_8 =
   { module_klic__comp__normalize, 11, 8 };
Const struct predicate predicate_klic__comp__normalize_xnorm__guard__goal_2F8_241_8 =
   { module_klic__comp__normalize, 12, 8 };
Const struct predicate predicate_klic__comp__normalize_xnorm__guard__goal_2F8_240_8 =
   { module_klic__comp__normalize, 13, 8 };
Const struct predicate predicate_klic__comp__normalize_xnorm__inline_12 =
   { module_klic__comp__normalize, 14, 12 };
Const struct predicate predicate_klic__comp__normalize_xnorm__inline_2F12_240_13 =
   { module_klic__comp__normalize, 15, 13 };
Const struct predicate predicate_klic__comp__normalize_xnorm__inline__arg_11 =
   { module_klic__comp__normalize, 16, 11 };
Const struct predicate predicate_klic__comp__normalize_xnorm__inline__arg__1_7 =
   { module_klic__comp__normalize, 17, 7 };
Const struct predicate predicate_klic__comp__normalize_xinline__arg__type_2 =
   { module_klic__comp__normalize, 18, 2 };
Const struct predicate predicate_klic__comp__normalize_xnorm__guard__unify_9 =
   { module_klic__comp__normalize, 19, 9 };
Const struct predicate predicate_klic__comp__normalize_xnorm__guard__unify_2F9_241_10 =
   { module_klic__comp__normalize, 20, 10 };
Const struct predicate predicate_klic__comp__normalize_xnorm__guard__unify_2F9_241_2F10_240_9 =
   { module_klic__comp__normalize, 21, 9 };
Const struct predicate predicate_klic__comp__normalize_xnorm__guard__unify_2F9_240_10 =
   { module_klic__comp__normalize, 22, 10 };
Const struct predicate predicate_klic__comp__normalize_xnorm__guard__unify__new_6 =
   { module_klic__comp__normalize, 23, 6 };
Const struct predicate predicate_klic__comp__normalize_xnorm__guard__unify__new_2F6_240_7 =
   { module_klic__comp__normalize, 24, 7 };
Const struct predicate predicate_klic__comp__normalize_xnorm__guard__unify__args_10 =
   { module_klic__comp__normalize, 25, 10 };
Const struct predicate predicate_klic__comp__normalize_xnorm__guard__generic_8 =
   { module_klic__comp__normalize, 26, 8 };
Const struct predicate predicate_klic__comp__normalize_xnorm__guard__generic__1_14 =
   { module_klic__comp__normalize, 27, 14 };
Const struct predicate predicate_klic__comp__normalize_xnorm__guard__generic__2_14 =
   { module_klic__comp__normalize, 28, 14 };
Const struct predicate predicate_klic__comp__normalize_xnorm__guard__generic__3_13 =
   { module_klic__comp__normalize, 29, 13 };
Const struct predicate predicate_klic__comp__normalize_xnorm__gg__in__out_7 =
   { module_klic__comp__normalize, 30, 7 };
Const struct predicate predicate_klic__comp__normalize_xnorm__gg__count__out_4 =
   { module_klic__comp__normalize, 31, 4 };
Const struct predicate predicate_klic__comp__normalize_xnorm__gg_5 =
   { module_klic__comp__normalize, 32, 5 };
Const struct predicate predicate_klic__comp__normalize_xnorm__gg__1_8 =
   { module_klic__comp__normalize, 33, 8 };
Const struct predicate predicate_klic__comp__normalize_xnorm__gg__2_6 =
   { module_klic__comp__normalize, 34, 6 };
Const struct predicate predicate_klic__comp__normalize_xnorm__gg__ins_8 =
   { module_klic__comp__normalize, 35, 8 };
Const struct predicate predicate_klic__comp__normalize_xnorm__gg__ins__1_9 =
   { module_klic__comp__normalize, 36, 9 };
Const struct predicate predicate_klic__comp__normalize_xnorm__gg__in_9 =
   { module_klic__comp__normalize, 37, 9 };
Const struct predicate predicate_klic__comp__normalize_xnorm__gg__in__1_10 =
   { module_klic__comp__normalize, 38, 10 };
Const struct predicate predicate_klic__comp__normalize_xnorm__gg__in__2_8 =
   { module_klic__comp__normalize, 39, 8 };
Const struct predicate predicate_klic__comp__normalize_xnorm__gg__outs_9 =
   { module_klic__comp__normalize, 40, 9 };
Const struct predicate predicate_klic__comp__normalize_xnorm__guard__builts_8 =
   { module_klic__comp__normalize, 41, 8 };
Const struct predicate predicate_klic__comp__normalize_xnorm__guard__builts__1_12 =
   { module_klic__comp__normalize, 42, 12 };
Const struct predicate predicate_klic__comp__normalize_xnorm__gblt__args_8 =
   { module_klic__comp__normalize, 43, 8 };
Const struct predicate predicate_klic__comp__normalize_xnorm__gblt__args__1_9 =
   { module_klic__comp__normalize, 44, 9 };
Const struct predicate predicate_klic__comp__normalize_xnorm__gblt__arg_9 =
   { module_klic__comp__normalize, 45, 9 };
Const struct predicate predicate_klic__comp__normalize_xnorm__gblt__arg__1_10 =
   { module_klic__comp__normalize, 46, 10 };
Const struct predicate predicate_klic__comp__normalize_xnorm__gblt__arg__2_8 =
   { module_klic__comp__normalize, 47, 8 };
Const struct predicate predicate_klic__comp__normalize_xcheck__gblt__input_5 =
   { module_klic__comp__normalize, 48, 5 };
Const struct predicate predicate_klic__comp__normalize_xnorm__body_6 =
   { module_klic__comp__normalize, 49, 6 };
Const struct predicate predicate_klic__comp__normalize_xnorm__body__1_9 =
   { module_klic__comp__normalize, 50, 9 };
Const struct predicate predicate_klic__comp__normalize_xnorm__body__tail_7 =
   { module_klic__comp__normalize, 51, 7 };
Const struct predicate predicate_klic__comp__normalize_xnorm__body__tail__1_8 =
   { module_klic__comp__normalize, 52, 8 };
Const struct predicate predicate_klic__comp__normalize_xnorm__top__goal_7 =
   { module_klic__comp__normalize, 53, 7 };
Const struct predicate predicate_klic__comp__normalize_xnorm__top__goal__unify_8 =
   { module_klic__comp__normalize, 54, 8 };
Const struct predicate predicate_klic__comp__normalize_xnorm__top__goal__builtin_7 =
   { module_klic__comp__normalize, 55, 7 };
Const struct predicate predicate_klic__comp__normalize_xnorm__tail__goal_7 =
   { module_klic__comp__normalize, 56, 7 };
Const struct predicate predicate_klic__comp__normalize_xnorm__tail__goal__unify_8 =
   { module_klic__comp__normalize, 57, 8 };
Const struct predicate predicate_klic__comp__normalize_xnorm__tail__goal__generic_9 =
   { module_klic__comp__normalize, 58, 9 };
Const struct predicate predicate_klic__comp__normalize_xnorm__tail__goal__builtin_8 =
   { module_klic__comp__normalize, 59, 8 };
Const struct predicate predicate_klic__comp__normalize_xnorm__pragma_8 =
   { module_klic__comp__normalize, 60, 8 };
Const struct predicate predicate_klic__comp__normalize_xnorm__pragma__0_9 =
   { module_klic__comp__normalize, 61, 9 };
Const struct predicate predicate_klic__comp__normalize_xnorm__pragma__1_8 =
   { module_klic__comp__normalize, 62, 8 };
Const struct predicate predicate_klic__comp__normalize_xnorm__pragma__priority_8 =
   { module_klic__comp__normalize, 63, 8 };
Const struct predicate predicate_klic__comp__normalize_xnorm__pragma__lower__priority_8 =
   { module_klic__comp__normalize, 64, 8 };
Const struct predicate predicate_klic__comp__normalize_xnorm__pragma__node_8 =
   { module_klic__comp__normalize, 65, 8 };
Const struct predicate predicate_klic__comp__normalize_xnorm__pragma__pragma__arg_8 =
   { module_klic__comp__normalize, 66, 8 };
Const struct predicate predicate_klic__comp__normalize_xnorm__body__args_9 =
   { module_klic__comp__normalize, 67, 9 };
Const struct predicate predicate_klic__comp__normalize_xnorm__body__args_7 =
   { module_klic__comp__normalize, 68, 7 };
Const struct predicate predicate_klic__comp__normalize_xnorm__body__args__1_8 =
   { module_klic__comp__normalize, 69, 8 };
Const struct predicate predicate_klic__comp__normalize_xnorm__body__arg_7 =
   { module_klic__comp__normalize, 70, 7 };
Const struct predicate predicate_klic__comp__normalize_xnorm__body__arg__1_8 =
   { module_klic__comp__normalize, 71, 8 };
Const struct predicate predicate_klic__comp__normalize_xnorm__body__arg__2_7 =
   { module_klic__comp__normalize, 72, 7 };
Const struct predicate predicate_klic__comp__normalize_xnorm__body__arg__3_8 =
   { module_klic__comp__normalize, 73, 8 };
Const struct predicate predicate_klic__comp__normalize_xnorm__body__arg__3__1_10 =
   { module_klic__comp__normalize, 74, 10 };
Const struct predicate predicate_klic__comp__normalize_xnorm__body__arg__3__2_6 =
   { module_klic__comp__normalize, 75, 6 };
Const struct predicate predicate_klic__comp__normalize_xis__const_5 =
   { module_klic__comp__normalize, 76, 5 };
Const struct predicate predicate_klic__comp__normalize_xis__const__1_6 =
   { module_klic__comp__normalize, 77, 6 };
Const struct predicate predicate_klic__comp__normalize_xis__const__args_3 =
   { module_klic__comp__normalize, 78, 3 };
Const struct predicate predicate_klic__comp__normalize_xyes__const_5 =
   { module_klic__comp__normalize, 79, 5 };
extern Const struct predicate predicate_integer__arithmetics_xadd_3;
extern Const struct predicate predicate_functor__table_xarg_3;
extern Const struct predicate predicate_functor__table_x_3D_2E_2E_2;
extern Const struct predicate predicate_klic__comp__insert_xmake__index__tree_5;
extern Const struct predicate predicate_klic__comp__insert_xsubsumed__type_3;
extern Const struct predicate predicate_klic__comp__generate_xgen__code_14;
extern Const struct predicate predicate_klic__comp__message_xwarning_4;
extern Const struct predicate predicate_klic__comp__message_xreport__error_4;
extern Const struct predicate predicate_klic__comp__util_xassoc_3;
extern Const struct predicate predicate_klic__comp__util_xappend_3;
extern Const struct predicate predicate_klic__comp__util_xuniv_2;
extern Const struct predicate predicate_klic__comp__util_xvector__to__list_2;
extern Const struct predicate predicate_klic__comp__util_xlength_2;
extern Const struct predicate predicate_klic__comp__gb_xis__guard__builtin_2;
extern Const struct predicate predicate_klic__comp__bb_xis__body__builtin_2;
declare_method_table_of(vector);

module module_klic__comp__normalize(glbl, qp, allocp, toppred)
  struct global_variables *glbl;
  struct goalrec *qp;
  register q *allocp;
  Const struct predicate *toppred;
{
  static Const unsigned char strconst_body_0[20] =
    "Clause deleted: ~w";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_0 =
      declare_string_constant_8(strconst_body_0,18);
  static Const unsigned char strconst_body_1[36] =
    "Guard builtin arg type mismatch: ~w";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_1 =
      declare_string_constant_8(strconst_body_1,35);
  static Const unsigned char strconst_body_2[40] =
    "Ignored pragma for a generic call: ~w";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_2 =
      declare_string_constant_8(strconst_body_2,37);
  static Const unsigned char strconst_body_3[32] =
    "Illegal argument to inline: ~w";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_3 =
      declare_string_constant_8(strconst_body_3,30);
  static Const unsigned char strconst_body_4[24] =
    "Illegal body goal: ~w";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_4 =
      declare_string_constant_8(strconst_body_4,21);
  static Const unsigned char strconst_body_5[40] =
    "Illegal pragma: ~w attached to goal: ~w";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_5 =
      declare_string_constant_8(strconst_body_5,39);
  static Const unsigned char strconst_body_6[32] =
    "Never proceeding guard builtin";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_6 =
      declare_string_constant_8(strconst_body_6,30);
  static Const unsigned char strconst_body_7[36] =
    "Never proceeding guard generic call";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_7 =
      declare_string_constant_8(strconst_body_7,35);
  static Const unsigned char strconst_body_8[60] =
    "No clauses follow \"alternatively\"; \"alternatively\" ignored";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_8 =
      declare_string_constant_8(strconst_body_8,58);
  static Const unsigned char strconst_body_9[52] =
    "No clauses follow \"otherwise\"; \"otherwise\" ignored";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_9 =
      declare_string_constant_8(strconst_body_9,50);
  static Const unsigned char strconst_body_10[44] =
    {'N','o','n','-','c','o','n','s','t',' ','s','t','r','u','c','t','u','r','e',' ','i','n',' ','g','u','a','r','d',' ','b','u','i','l','t','i','n',' ','a','r','g',':',' ','~','w',};
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_10 =
      declare_string_constant_8(strconst_body_10,44);
  static Const unsigned char strconst_body_11[52] =
    "Non-const structure in guard generic call arg: ~w";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_11 =
      declare_string_constant_8(strconst_body_11,49);
  static Const unsigned char strconst_body_12[52] =
    "Non-first variable occurrence as inline output: ~w";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_12 =
      declare_string_constant_8(strconst_body_12,50);
  static Const unsigned char strconst_body_13[40] =
    {'N','o','n','-','o','b','j','e','c','t',' ','g','i','v','e','n',' ','a','s',' ','g','e','n','e','r','i','c',' ','c','a','l','l',' ','a','r','g',':',' ','~','w',};
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_13 =
      declare_string_constant_8(strconst_body_13,40);
  static Const unsigned char strconst_body_14[48] =
    "Structure allocation not allowed in guard: ~w";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_14 =
      declare_string_constant_8(strconst_body_14,45);
  static Const unsigned char strconst_body_15[32] =
    "Unification always fails: ~w=~w";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_15 =
      declare_string_constant_8(strconst_body_15,31);
  static Const unsigned char strconst_body_16[28] =
    "Unrecognized guard goal: ~w";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_16 =
      declare_string_constant_8(strconst_body_16,27);
  static Const q funct_const_17[] = {
    makesym(functor_atom_1),
    NILATOM,
  };
  static Const q funct_const_18[] = {
    makesym(functor_integer_1),
    makeint(1),
  };
  static Const q funct_const_19[] = {
    makesym(functor_lower__priority_1),
    makefunctor(funct_const_18),
  };
  static Const q funct_const_20[] = {
    makesym(functor_functor_1),
    makefunctor(funct_const_19),
  };
  q a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13;

  q *reasonp;
 module_top:
  a0 = qp->args[0];
  a1 = qp->args[1];
  switch_on_pred() {
    case_pred(0, generate_8_top);
    case_pred(1, make__arg__assoc_3_top);
    case_pred(2, separate__cond_5_top);
    case_pred(3, separate__cond__1_9_top);
    case_pred(4, norm__head_5_top);
    case_pred(5, norm__args_8_top);
    case_pred(6, norm__guard__term_6_top);
    case_pred(7, norm__guard__term__1_7_top);
    case_pred(8, norm__guard_10_top);
    case_pred(9, norm__guard_2F10_241_11_top);
    case_pred(10, norm__guard_2F10_240_11_top);
    case_pred(11, norm__guard__goal_8_top);
    case_pred(12, norm__guard__goal_2F8_241_8_top);
    case_pred(13, norm__guard__goal_2F8_240_8_top);
    case_pred(14, norm__inline_12_top);
    case_pred(15, norm__inline_2F12_240_13_top);
    case_pred(16, norm__inline__arg_11_top);
    case_pred(17, norm__inline__arg__1_7_top);
    case_pred(18, inline__arg__type_2_top);
    case_pred(19, norm__guard__unify_9_top);
    case_pred(20, norm__guard__unify_2F9_241_10_top);
    case_pred(21, norm__guard__unify_2F9_241_2F10_240_9_top);
    case_pred(22, norm__guard__unify_2F9_240_10_top);
    case_pred(23, norm__guard__unify__new_6_top);
    case_pred(24, norm__guard__unify__new_2F6_240_7_top);
    case_pred(25, norm__guard__unify__args_10_top);
    case_pred(26, norm__guard__generic_8_top);
    case_pred(27, norm__guard__generic__1_14_top);
    case_pred(28, norm__guard__generic__2_14_top);
    case_pred(29, norm__guard__generic__3_13_top);
    case_pred(30, norm__gg__in__out_7_top);
    case_pred(31, norm__gg__count__out_4_top);
    case_pred(32, norm__gg_5_top);
    case_pred(33, norm__gg__1_8_top);
    case_pred(34, norm__gg__2_6_top);
    case_pred(35, norm__gg__ins_8_top);
    case_pred(36, norm__gg__ins__1_9_top);
    case_pred(37, norm__gg__in_9_top);
    case_pred(38, norm__gg__in__1_10_top);
    case_pred(39, norm__gg__in__2_8_top);
    case_pred(40, norm__gg__outs_9_top);
    case_pred(41, norm__guard__builts_8_top);
    case_pred(42, norm__guard__builts__1_12_top);
    case_pred(43, norm__gblt__args_8_top);
    case_pred(44, norm__gblt__args__1_9_top);
    case_pred(45, norm__gblt__arg_9_top);
    case_pred(46, norm__gblt__arg__1_10_top);
    case_pred(47, norm__gblt__arg__2_8_top);
    case_pred(48, check__gblt__input_5_top);
    case_pred(49, norm__body_6_top);
    case_pred(50, norm__body__1_9_top);
    case_pred(51, norm__body__tail_7_top);
    case_pred(52, norm__body__tail__1_8_top);
    case_pred(53, norm__top__goal_7_top);
    case_pred(54, norm__top__goal__unify_8_top);
    case_pred(55, norm__top__goal__builtin_7_top);
    case_pred(56, norm__tail__goal_7_top);
    case_pred(57, norm__tail__goal__unify_8_top);
    case_pred(58, norm__tail__goal__generic_9_top);
    case_pred(59, norm__tail__goal__builtin_8_top);
    case_pred(60, norm__pragma_8_top);
    case_pred(61, norm__pragma__0_9_top);
    case_pred(62, norm__pragma__1_8_top);
    case_pred(63, norm__pragma__priority_8_top);
    case_pred(64, norm__pragma__lower__priority_8_top);
    case_pred(65, norm__pragma__node_8_top);
    case_pred(66, norm__pragma__pragma__arg_8_top);
    case_pred(67, norm__body__args_9_top);
    case_pred(68, norm__body__args_7_top);
    case_pred(69, norm__body__args__1_8_top);
    case_pred(70, norm__body__arg_7_top);
    case_pred(71, norm__body__arg__1_8_top);
    case_pred(72, norm__body__arg__2_7_top);
    case_pred(73, norm__body__arg__3_8_top);
    case_pred(74, norm__body__arg__3__1_10_top);
    case_pred(75, norm__body__arg__3__2_6_top);
    case_pred(76, is__const_5_top);
    case_pred(77, is__const__1_6_top);
    case_pred(78, is__const__args_3_top);
    last_case_pred(79, yes__const_5_top);
  }

 generate_8_top: {
  q x0, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10;
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  a6 = qp->args[6];
  a7 = qp->args[7];
  qp = qp->next;
 generate_8_clear_reason:
  reasonp = reasons;
 generate_8_0:
 generate_8_1:
  switch (ptagof(a0)) {
 case FUNCTOR:
  if (functor_of(a0) != makesym(functor__2F_2)) goto generate_8_interrupt;
  allocp[0] = makesym(functor_lab_1);
  allocp[1] = x1 = makeref(&allocp[1]);
  x0 = makefunctor(&allocp[0]);
  allocp[2] = (q)qp;
  allocp[3] = (q)(&predicate_klic__comp__generate_xgen__code_14);
  allocp[4] = x2 = makeref(&allocp[4]);
  allocp[5] = makesym(atom_interrupt);
  allocp[6] = x0;
  allocp[7] = x3 = makeref(&allocp[7]);
  allocp[8] = makeint(0L);
  allocp[9] = a2;
  allocp[10] = makeint(0L);
  allocp[11] = x4 = makeref(&allocp[11]);
  allocp[12] = makeint(0L);
  allocp[13] = x5 = makeref(&allocp[13]);
  allocp[14] = a3;
  allocp[15] = a4;
  allocp[16] = a5;
  allocp[17] = NILATOM;
  allocp[18] = (q)(struct goalrec*)&allocp[2];
  allocp[19] = (q)(&predicate_klic__comp__normalize_xmake__arg__assoc_3);
  allocp[20] = makeint(0L);
  x6 = arg(a0, 1);
  allocp[21] = x6;
  allocp[22] = x3;
  allocp[23] = makesym(functor__2F_2);
  x8 = arg(a0, 0);
  allocp[24] = x8;
  allocp[25] = x6;
  x7 = makefunctor(&allocp[23]);
  allocp[26] = (q)(struct goalrec*)&allocp[18];
  allocp[27] = (q)(&predicate_klic__comp__insert_xmake__index__tree_5);
  allocp[28] = x9 = makeref(&allocp[28]);
  allocp[29] = NILATOM;
  allocp[30] = makesym(atom_e);
  allocp[31] = x2;
  allocp[32] = x7;
  a0 = a1;
  a1 = x9;
  allocp[33] = x10 = makeref(&allocp[33]);
  a2 = x10;
  a3 = a6;
  a4 = a7;
  qp = (struct goalrec*)&allocp[26];
  allocp += 34;
  execute(separate__cond_5_0);
  goto separate__cond_5_ext_interrupt;
 case VARREF:
  deref_and_jump(a0,generate_8_1);
  *reasonp++ =  a0;
  goto generate_8_interrupt;
  };
  goto generate_8_interrupt;
 generate_8_ext_interrupt:
  reasonp = 0l;
 generate_8_interrupt:
  goto interrupt_8;
 }

 make__arg__assoc_3_top: {
  q x0, x1, x2, x3, x4, x5, x6;
  a2 = qp->args[2];
  qp = qp->next;
 make__arg__assoc_3_clear_reason:
  reasonp = reasons;
 make__arg__assoc_3_0:
 make__arg__assoc_3_1:
  if (!isint(a0)) goto make__arg__assoc_3_5;
 make__arg__assoc_3_2:
  if (!isint(a1)) goto make__arg__assoc_3_4;
  gblt_greater_or_eq(a0,a1,make__arg__assoc_3_3);
  x0 = NILATOM;
  unify_value(a2, x0);
  proceed();
 make__arg__assoc_3_3:
  gblt_less(a0,a1,make__arg__assoc_3_interrupt);
  x0 = makeint(1L);
  bblt_add_no_check(a0,x0,x1);
  allocp[0] = makesym(functor_arg_2);
  allocp[1] = makesym(atom_head);
  allocp[2] = a0;
  x2 = makefunctor(&allocp[0]);
  allocp[3] = makesym(functor_a_1);
  allocp[4] = a0;
  x3 = makefunctor(&allocp[3]);
  allocp[5] = makesym(functor__24_3);
  allocp[6] = x2;
  allocp[7] = x3;
  allocp[8] = makesym(atom_any);
  x4 = makefunctor(&allocp[5]);
  allocp[9] = x6 = makeref(&allocp[9]);
  allocp[10] = x4;
  x5 = makecons(&allocp[9]);
  allocp += 11;
  unify_value(a2, x5);
  a0 = x1;
  a2 = x6;
  execute(make__arg__assoc_3_0);
  goto make__arg__assoc_3_ext_interrupt;
 make__arg__assoc_3_4:
  if (!isref(a1)) goto make__arg__assoc_3_interrupt;
  deref_and_jump(a1,make__arg__assoc_3_2);
  *reasonp++ =  a1;
  goto make__arg__assoc_3_interrupt;
 make__arg__assoc_3_5:
  if (!isref(a0)) goto make__arg__assoc_3_interrupt;
  deref_and_jump(a0,make__arg__assoc_3_1);
  *reasonp++ =  a0;
  goto make__arg__assoc_3_interrupt;
 make__arg__assoc_3_ext_interrupt:
  reasonp = 0l;
 make__arg__assoc_3_interrupt:
  goto interrupt_3;
 }

 separate__cond_5_top: {
  q x0, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11;
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  qp = qp->next;
 separate__cond_5_clear_reason:
  reasonp = reasons;
 separate__cond_5_0:
 separate__cond_5_1:
  switch (ptagof(a0)) {
 case CONS:
  x0 = car_of(a0);
 separate__cond_5_2:
  switch (ptagof(x0)) {
 case ATOMIC:
  if (isint(x0)) goto separate__cond_5_interrupt;
  switch (symval(x0)) {
 case symval(makesym(atom_otherwise)):
  x1 = cdr_of(a0);
 separate__cond_5_3:
  switch (ptagof(x1)) {
 case ATOMIC:
  if (x1 != NILATOM) goto separate__cond_5_4;
  x2 = makesym(atom_normal);
  unify_value(a2, x2);
  x3 = NILATOM;
  unify_value(a1, x3);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__message_xwarning_4);
  allocp[2] = makefunctor(&string_const_9);
  allocp[3] = NILATOM;
  allocp[4] = a3;
  allocp[5] = a4;
  qp = (struct goalrec*)&allocp[0];
  allocp += 6;
  proceed();
 case VARREF:
  deref_and_jump(x1,separate__cond_5_3);
  *reasonp++ =  x1;
  goto separate__cond_5_interrupt;
  };
  goto separate__cond_5_4;
 separate__cond_5_4:
  x2 = NILATOM;
  gblt_diff(x1,x2,separate__cond_5_interrupt);
  allocp[0] = x4 = makeref(&allocp[0]);
  allocp[1] = makesym(atom_otherwise);
  x3 = makecons(&allocp[0]);
  allocp += 2;
  unify_value(a1, x3);
  a0 = x1;
  a1 = x4;
  execute(separate__cond_5_0);
  goto separate__cond_5_ext_interrupt;
 case symval(makesym(atom_alternatively)):
  x1 = cdr_of(a0);
 separate__cond_5_5:
  switch (ptagof(x1)) {
 case ATOMIC:
  if (x1 != NILATOM) goto separate__cond_5_6;
  x2 = makesym(atom_normal);
  unify_value(a2, x2);
  x3 = NILATOM;
  unify_value(a1, x3);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__message_xwarning_4);
  allocp[2] = makefunctor(&string_const_8);
  allocp[3] = NILATOM;
  allocp[4] = a3;
  allocp[5] = a4;
  qp = (struct goalrec*)&allocp[0];
  allocp += 6;
  proceed();
 case VARREF:
  deref_and_jump(x1,separate__cond_5_5);
  *reasonp++ =  x1;
  goto separate__cond_5_interrupt;
  };
  goto separate__cond_5_6;
 separate__cond_5_6:
  x2 = NILATOM;
  gblt_diff(x1,x2,separate__cond_5_interrupt);
  allocp[0] = x4 = makeref(&allocp[0]);
  allocp[1] = makesym(atom_alternatively);
  x3 = makecons(&allocp[0]);
  allocp += 2;
  unify_value(a1, x3);
  a0 = x1;
  a1 = x4;
  execute(separate__cond_5_0);
  goto separate__cond_5_ext_interrupt;
  };
  goto separate__cond_5_interrupt;
 case FUNCTOR:
  if (functor_of(x0) != makesym(functor_clause_4)) goto separate__cond_5_interrupt;
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__normalize_xseparate__cond__1_9);
  x1 = cdr_of(a0);
  allocp[2] = x1;
  allocp[3] = x2 = makeref(&allocp[3]);
  allocp[4] = a2;
  allocp[5] = a1;
  allocp[6] = x3 = makeref(&allocp[6]);
  x4 = arg(x0, 0);
  allocp[7] = x4;
  allocp[8] = x5 = makeref(&allocp[8]);
  allocp[9] = x6 = makeref(&allocp[9]);
  allocp[10] = a4;
  allocp[11] = (q)(struct goalrec*)&allocp[0];
  allocp[12] = (q)(&predicate_klic__comp__normalize_xnorm__guard_10);
  x7 = arg(x0, 2);
  allocp[13] = x7;
  allocp[14] = x8 = makeref(&allocp[14]);
  x9 = arg(x0, 3);
  allocp[15] = x9;
  allocp[16] = x10 = makeref(&allocp[16]);
  allocp[17] = x3;
  allocp[18] = x2;
  allocp[19] = x11 = makeref(&allocp[19]);
  allocp[20] = x5;
  allocp[21] = a3;
  allocp[22] = x6;
  a0 = arg(x0, 1);
  a1 = NILATOM;
  a2 = x8;
  a3 = x10;
  a4 = x11;
  qp = (struct goalrec*)&allocp[11];
  allocp += 23;
  execute(norm__head_5_0);
  goto norm__head_5_ext_interrupt;
 case VARREF:
  deref_and_jump(x0,separate__cond_5_2);
  *reasonp++ =  x0;
  goto separate__cond_5_interrupt;
  };
  goto separate__cond_5_interrupt;
 case ATOMIC:
  if (a0 != NILATOM) goto separate__cond_5_interrupt;
  unify(a4, a3);
  x0 = NILATOM;
  unify_value(a1, x0);
  x1 = makesym(atom_normal);
  unify_value(a2, x1);
  proceed();
 case VARREF:
  deref_and_jump(a0,separate__cond_5_1);
  *reasonp++ =  a0;
  goto separate__cond_5_interrupt;
  };
  goto separate__cond_5_interrupt;
 separate__cond_5_ext_interrupt:
  reasonp = 0l;
 separate__cond_5_interrupt:
  toppred = &predicate_klic__comp__normalize_xseparate__cond_5;
  goto interrupt_5;
 }

 separate__cond__1_9_top: {
  q x0, x1, x2;
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  a6 = qp->args[6];
  a7 = qp->args[7];
  a8 = qp->args[8];
  qp = qp->next;
 separate__cond__1_9_clear_reason:
  reasonp = reasons;
 separate__cond__1_9_0:
 separate__cond__1_9_1:
  switch (ptagof(a6)) {
 case ATOMIC:
  if (a6 != makesym(atom_normal)) goto separate__cond__1_9_2;
  unify(a3, a4);
  a3 = a7;
  a4 = a8;
  execute(separate__cond_5_0);
  goto separate__cond_5_ext_interrupt;
 case VARREF:
  deref_and_jump(a6,separate__cond__1_9_1);
  *reasonp++ =  a6;
  goto separate__cond__1_9_interrupt;
  };
  goto separate__cond__1_9_2;
 separate__cond__1_9_2:
  x0 = makesym(atom_normal);
  gblt_diff(a6,x0,separate__cond__1_9_interrupt);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__normalize_xseparate__cond_5);
  allocp[2] = a0;
  allocp[3] = a3;
  allocp[4] = a2;
  allocp[5] = x1 = makeref(&allocp[5]);
  allocp[6] = a8;
  allocp[7] = NILATOM;
  allocp[8] = a5;
  x2 = makecons(&allocp[7]);
  allocp[9] = (q)(struct goalrec*)&allocp[0];
  allocp[10] = (q)(&predicate_klic__comp__message_xwarning_4);
  allocp[11] = makefunctor(&string_const_0);
  allocp[12] = x2;
  allocp[13] = a7;
  allocp[14] = x1;
  qp = (struct goalrec*)&allocp[9];
  allocp += 15;
  proceed();
 separate__cond__1_9_ext_interrupt:
  reasonp = 0l;
 separate__cond__1_9_interrupt:
  goto interrupt_9;
 }

 norm__head_5_top: {
  q x0, x1, x2, x3;
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  qp = qp->next;
 norm__head_5_clear_reason:
  reasonp = reasons;
 norm__head_5_0:
 norm__head_5_1:
  switch (ptagof(a0)) {
 case FUNCTOR:
  switch (symval(functor_of(a0))) {
 case functor_functor_1:
  x0 = arg(a0, 0);
 norm__head_5_2:
  if (isref(x0)) goto norm__head_5_3;
  gblt_pfunctor(x0,x1,norm__head_5_interrupt);
  gblt_arity(x0,x2,norm__head_5_interrupt);
  a0 = makeint(0L);
  x3 = a1;
  a5 = a2;
  a6 = a3;
  a7 = a4;
  a4 = x3;
  a3 = makesym(atom_head);
  a2 = x0;
  a1 = x2;
  execute(norm__args_8_0);
  goto norm__args_8_ext_interrupt;
 norm__head_5_3:
  deref_and_jump(x0,norm__head_5_2);
  *reasonp++ =  x0;
  goto norm__head_5_interrupt;
 case functor_atom_1:
  unify2(a4, a3, a2, a1);
  proceed();
  };
  goto norm__head_5_interrupt;
 case VARREF:
  deref_and_jump(a0,norm__head_5_1);
  *reasonp++ =  a0;
  goto norm__head_5_interrupt;
  };
  goto norm__head_5_interrupt;
 norm__head_5_ext_interrupt:
  reasonp = 0l;
 norm__head_5_interrupt:
  toppred = &predicate_klic__comp__normalize_xnorm__head_5;
  goto interrupt_5;
 }

 norm__args_8_top: {
  q x0, x1, x2, x3, x4, x5;
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  a6 = qp->args[6];
  a7 = qp->args[7];
  qp = qp->next;
 norm__args_8_clear_reason:
  reasonp = reasons;
 norm__args_8_0:
 norm__args_8_1:
  if (!isint(a0)) goto norm__args_8_7;
 norm__args_8_2:
  if (!isint(a1)) goto norm__args_8_6;
  gblt_greater_or_eq(a0,a1,norm__args_8_3);
  unify2(a7, a6, a5, a4);
  proceed();
 norm__args_8_3:
  gblt_less(a0,a1,norm__args_8_interrupt);
  x1 = makeint(1L);
  gblt_add(a0,x1,x0,norm__args_8_interrupt);
 norm__args_8_4:
  if (isref(a2)) goto norm__args_8_5;
  gblt_arg(x0,a2,x2,norm__args_8_interrupt);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__normalize_xnorm__args_8);
  allocp[2] = x0;
  allocp[3] = a1;
  allocp[4] = a2;
  allocp[5] = a3;
  allocp[6] = x3 = makeref(&allocp[6]);
  allocp[7] = a5;
  allocp[8] = x4 = makeref(&allocp[8]);
  allocp[9] = a7;
  allocp[10] = makesym(functor_arg_2);
  allocp[11] = a3;
  allocp[12] = a0;
  x5 = makefunctor(&allocp[10]);
  a0 = x2;
  a1 = x5;
  a2 = a4;
  a3 = x3;
  a4 = a6;
  a5 = x4;
  qp = (struct goalrec*)&allocp[0];
  allocp += 13;
  execute(norm__guard__term_6_0);
  goto norm__guard__term_6_ext_interrupt;
 norm__args_8_5:
  deref_and_jump(a2,norm__args_8_4);
  *reasonp++ =  a2;
  goto norm__args_8_interrupt;
 norm__args_8_6:
  if (!isref(a1)) goto norm__args_8_interrupt;
  deref_and_jump(a1,norm__args_8_2);
  *reasonp++ =  a1;
  goto norm__args_8_interrupt;
 norm__args_8_7:
  if (!isref(a0)) goto norm__args_8_interrupt;
  deref_and_jump(a0,norm__args_8_1);
  *reasonp++ =  a0;
  goto norm__args_8_interrupt;
 norm__args_8_ext_interrupt:
  reasonp = 0l;
 norm__args_8_interrupt:
  toppred = &predicate_klic__comp__normalize_xnorm__args_8;
  goto interrupt_8;
 }

 norm__guard__term_6_top: {
  q x0, x1, x2, x3, x4, x5, x6, x7, x8;
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  qp = qp->next;
 norm__guard__term_6_clear_reason:
  reasonp = reasons;
 norm__guard__term_6_0:
 norm__guard__term_6_1:
  switch (ptagof(a0)) {
 case FUNCTOR:
  switch (symval(functor_of(a0))) {
 case functor_variable_1:
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__normalize_xnorm__guard__term__1_7);
  allocp[2] = x0 = makeref(&allocp[2]);
  x1 = arg(a0, 0);
  allocp[3] = x1;
  allocp[4] = a1;
  allocp[5] = a2;
  allocp[6] = a3;
  allocp[7] = a4;
  allocp[8] = a5;
  allocp[9] = (q)(struct goalrec*)&allocp[0];
  allocp[10] = (q)(&predicate_klic__comp__util_xassoc_3);
  allocp[11] = a2;
  allocp[12] = x1;
  allocp[13] = x0;
  qp = (struct goalrec*)&allocp[9];
  allocp += 14;
  proceed();
 case functor_integer_1:
  unify(a3, a2);
  allocp[0] = makesym(functor_int_1);
  x1 = arg(a0, 0);
  allocp[1] = x1;
  x0 = makefunctor(&allocp[0]);
  allocp[2] = makesym(functor__3A_2);
  allocp[3] = a1;
  allocp[4] = x0;
  x2 = makefunctor(&allocp[2]);
  allocp[5] = a5;
  allocp[6] = x2;
  x3 = makecons(&allocp[5]);
  allocp += 7;
  unify_value(a4, x3);
  proceed();
 case functor_atom_1:
  unify(a3, a2);
  allocp[0] = makesym(functor__3A_2);
  allocp[1] = a1;
  allocp[2] = a0;
  x0 = makefunctor(&allocp[0]);
  allocp[3] = a5;
  allocp[4] = x0;
  x1 = makecons(&allocp[3]);
  allocp += 5;
  unify_value(a4, x1);
  proceed();
 case functor_float_1:
  unify(a3, a2);
  allocp[0] = makesym(functor__3A_2);
  allocp[1] = a1;
  allocp[2] = a0;
  x0 = makefunctor(&allocp[0]);
  allocp[3] = a5;
  allocp[4] = x0;
  x1 = makecons(&allocp[3]);
  allocp += 5;
  unify_value(a4, x1);
  proceed();
 case functor_predicate_3:
  unify(a3, a2);
  allocp[0] = makesym(functor__3A_2);
  allocp[1] = a1;
  allocp[2] = a0;
  x0 = makefunctor(&allocp[0]);
  allocp[3] = a5;
  allocp[4] = x0;
  x1 = makecons(&allocp[3]);
  allocp += 5;
  unify_value(a4, x1);
  proceed();
 case functor_string_1:
  unify(a3, a2);
  allocp[0] = makesym(functor__3A_2);
  allocp[1] = a1;
  allocp[2] = a0;
  x0 = makefunctor(&allocp[0]);
  allocp[3] = a5;
  allocp[4] = x0;
  x1 = makecons(&allocp[3]);
  allocp += 5;
  unify_value(a4, x1);
  proceed();
 case functor_list_1:
  x0 = arg(a0, 0);
 norm__guard__term_6_2:
  switch (ptagof(x0)) {
 case CONS:
  allocp[0] = makesym(functor__3A_2);
  allocp[1] = a1;
  allocp[2] = makesym(atom_list);
  x1 = makefunctor(&allocp[0]);
  allocp[3] = x3 = makeref(&allocp[3]);
  allocp[4] = x1;
  x2 = makecons(&allocp[3]);
  allocp += 5;
  unify_value(a4, x2);
  allocp[0] = makesym(functor_arg_2);
  allocp[1] = a1;
  allocp[2] = makesym(atom_cdr);
  x4 = makefunctor(&allocp[0]);
  allocp[3] = (q)qp;
  allocp[4] = (q)(&predicate_klic__comp__normalize_xnorm__guard__term_6);
  x5 = cdr_of(x0);
  allocp[5] = x5;
  allocp[6] = x4;
  allocp[7] = x6 = makeref(&allocp[7]);
  allocp[8] = a3;
  allocp[9] = x7 = makeref(&allocp[9]);
  allocp[10] = a5;
  allocp[11] = makesym(functor_arg_2);
  allocp[12] = a1;
  allocp[13] = makesym(atom_car);
  x8 = makefunctor(&allocp[11]);
  a0 = car_of(x0);
  a1 = x8;
  a3 = x6;
  a4 = x3;
  a5 = x7;
  qp = (struct goalrec*)&allocp[3];
  allocp += 14;
  execute(norm__guard__term_6_0);
  goto norm__guard__term_6_ext_interrupt;
 case VARREF:
  deref_and_jump(x0,norm__guard__term_6_2);
  *reasonp++ =  x0;
  goto norm__guard__term_6_interrupt;
  };
  goto norm__guard__term_6_interrupt;
 case functor_functor_1:
  x0 = arg(a0, 0);
 norm__guard__term_6_3:
  if (isref(x0)) goto norm__guard__term_6_4;
  gblt_pfunctor(x0,x1,norm__guard__term_6_interrupt);
  gblt_arity(x0,x2,norm__guard__term_6_interrupt);
  allocp[0] = makesym(functor_functor_2);
  allocp[1] = x1;
  allocp[2] = x2;
  x3 = makefunctor(&allocp[0]);
  allocp[3] = makesym(functor__3A_2);
  allocp[4] = a1;
  allocp[5] = x3;
  x4 = makefunctor(&allocp[3]);
  allocp[6] = x6 = makeref(&allocp[6]);
  allocp[7] = x4;
  x5 = makecons(&allocp[6]);
  allocp += 8;
  unify_value(a4, x5);
  a0 = makeint(0L);
  x7 = a1;
  a4 = a2;
  x8 = a3;
  a6 = x6;
  a7 = a5;
  a5 = x8;
  a3 = x7;
  a2 = x0;
  a1 = x2;
  execute(norm__args_8_0);
  goto norm__args_8_ext_interrupt;
 norm__guard__term_6_4:
  deref_and_jump(x0,norm__guard__term_6_3);
  *reasonp++ =  x0;
  goto norm__guard__term_6_interrupt;
  };
  goto norm__guard__term_6_interrupt;
 case VARREF:
  deref_and_jump(a0,norm__guard__term_6_1);
  *reasonp++ =  a0;
  goto norm__guard__term_6_interrupt;
  };
  goto norm__guard__term_6_interrupt;
 norm__guard__term_6_ext_interrupt:
  reasonp = 0l;
 norm__guard__term_6_interrupt:
  toppred = &predicate_klic__comp__normalize_xnorm__guard__term_6;
  goto interrupt_6;
 }

 norm__guard__term__1_7_top: {
  q x0, x1, x2;
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  a6 = qp->args[6];
  qp = qp->next;
 norm__guard__term__1_7_clear_reason:
  reasonp = reasons;
 norm__guard__term__1_7_0:
 norm__guard__term__1_7_1:
  switch (ptagof(a0)) {
 case FUNCTOR:
  if (functor_of(a0) != makesym(functor_yes_1)) goto norm__guard__term__1_7_3;
  x0 = arg(a0, 0);
 norm__guard__term__1_7_2:
  switch (ptagof(x0)) {
 case FUNCTOR:
  if (functor_of(x0) != makesym(functor_rename_1)) goto norm__guard__term__1_7_3;
  allocp[0] = makesym(functor_variable_1);
  x2 = arg(x0, 0);
  allocp[1] = x2;
  x1 = makefunctor(&allocp[0]);
  a0 = x1;
  a1 = a2;
  a2 = a3;
  a3 = a4;
  a4 = a5;
  a5 = a6;
  allocp += 2;
  execute(norm__guard__term_6_0);
  goto norm__guard__term_6_ext_interrupt;
 case VARREF:
  deref_and_jump(x0,norm__guard__term__1_7_2);
  *reasonp++ =  x0;
  goto norm__guard__term__1_7_3;
  };
  goto norm__guard__term__1_7_3;
 case VARREF:
  deref_and_jump(a0,norm__guard__term__1_7_1);
  *reasonp++ =  a0;
  goto norm__guard__term__1_7_3;
  };
  goto norm__guard__term__1_7_3;
 norm__guard__term__1_7_3:
  otherwise(norm__guard__term__1_7_interrupt);
 norm__guard__term__1_7_4:
  switch (ptagof(a0)) {
 case ATOMIC:
  if (a0 != makesym(atom_no)) goto norm__guard__term__1_7_interrupt;
  unify(a6, a5);
  allocp[0] = makesym(functor__3D_2);
  allocp[1] = a1;
  allocp[2] = a2;
  x0 = makefunctor(&allocp[0]);
  allocp[3] = a3;
  allocp[4] = x0;
  x1 = makecons(&allocp[3]);
  allocp += 5;
  unify_value(a4, x1);
  proceed();
 case FUNCTOR:
  if (functor_of(a0) != makesym(functor_yes_1)) goto norm__guard__term__1_7_interrupt;
  unify(a4, a3);
  allocp[0] = makesym(functor__3A_2);
  allocp[1] = a2;
  x1 = arg(a0, 0);
  allocp[2] = x1;
  x0 = makefunctor(&allocp[0]);
  allocp[3] = a6;
  allocp[4] = x0;
  x2 = makecons(&allocp[3]);
  allocp += 5;
  unify_value(a5, x2);
  proceed();
 case VARREF:
  deref_and_jump(a0,norm__guard__term__1_7_4);
  *reasonp++ =  a0;
  goto norm__guard__term__1_7_interrupt;
  };
  goto norm__guard__term__1_7_interrupt;
 norm__guard__term__1_7_ext_interrupt:
  reasonp = 0l;
 norm__guard__term__1_7_interrupt:
  goto interrupt_7;
 }

 norm__guard_10_top: {
  q x0, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11;
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  a6 = qp->args[6];
  a7 = qp->args[7];
  a8 = qp->args[8];
  a9 = qp->args[9];
  qp = qp->next;
 norm__guard_10_clear_reason:
  reasonp = reasons;
 norm__guard_10_0:
 norm__guard_10_1:
  switch (ptagof(a0)) {
 case CONS:
  x0 = car_of(a0);
 norm__guard_10_2:
  switch (ptagof(x0)) {
 case FUNCTOR:
  if (functor_of(x0) != makesym(functor_functor_1)) goto norm__guard_10_4;
  x1 = arg(x0, 0);
 norm__guard_10_3:
  switch (ptagof(x1)) {
 case FUNCTOR:
  if (functor_of(x1) != makesym(functor__3B_2)) goto norm__guard_10_4;
  x2 = NILATOM;
  unify_value(a6, x2);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__normalize_xnorm__guard_2F10_241_11);
  allocp[2] = x3 = makeref(&allocp[2]);
  x4 = arg(x1, 1);
  allocp[3] = x4;
  x5 = cdr_of(a0);
  allocp[4] = x5;
  allocp[5] = a3;
  allocp[6] = a1;
  allocp[7] = a2;
  allocp[8] = x6 = makeref(&allocp[8]);
  allocp[9] = a5;
  allocp[10] = a7;
  allocp[11] = x7 = makeref(&allocp[11]);
  allocp[12] = a9;
  allocp[13] = (q)(struct goalrec*)&allocp[0];
  allocp[14] = (q)(&predicate_klic__comp__normalize_xnorm__guard_10);
  allocp[15] = x8 = makeref(&allocp[15]);
  allocp[16] = a1;
  allocp[17] = a2;
  allocp[18] = x9 = makeref(&allocp[18]);
  allocp[19] = a4;
  allocp[20] = x6;
  allocp[21] = x10 = makeref(&allocp[21]);
  allocp[22] = x3;
  allocp[23] = a8;
  allocp[24] = x7;
  allocp[25] = (q)(struct goalrec*)&allocp[13];
  allocp[26] = (q)(&predicate_klic__comp__util_xappend_3);
  allocp[27] = a3;
  allocp[28] = x10;
  allocp[29] = x9;
  allocp[30] = (q)(struct goalrec*)&allocp[25];
  allocp[31] = (q)(&predicate_klic__comp__util_xappend_3);
  x11 = arg(x1, 0);
  allocp[32] = x11;
  allocp[33] = x5;
  allocp[34] = x8;
  qp = (struct goalrec*)&allocp[30];
  allocp += 35;
  proceed();
 case VARREF:
  deref_and_jump(x1,norm__guard_10_3);
  *reasonp++ =  x1;
  goto norm__guard_10_4;
  };
  goto norm__guard_10_4;
 case VARREF:
  deref_and_jump(x0,norm__guard_10_2);
  *reasonp++ =  x0;
  goto norm__guard_10_4;
  };
  goto norm__guard_10_4;
 case VARREF:
  deref_and_jump(a0,norm__guard_10_1);
  *reasonp++ =  a0;
  goto norm__guard_10_4;
  };
  goto norm__guard_10_4;
 norm__guard_10_4:
  otherwise(norm__guard_10_interrupt);
 norm__guard_10_5:
  switch (ptagof(a0)) {
 case CONS:
  x0 = car_of(a0);
 norm__guard_10_6:
  switch (ptagof(x0)) {
 case FUNCTOR:
  switch (symval(functor_of(x0))) {
 case functor_functor_1:
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__normalize_xnorm__guard_2F10_240_11);
  allocp[2] = x1 = makeref(&allocp[2]);
  x2 = cdr_of(a0);
  allocp[3] = x2;
  allocp[4] = x3 = makeref(&allocp[4]);
  allocp[5] = a2;
  allocp[6] = a3;
  allocp[7] = a4;
  allocp[8] = a5;
  allocp[9] = x4 = makeref(&allocp[9]);
  allocp[10] = a7;
  allocp[11] = x5 = makeref(&allocp[11]);
  allocp[12] = a9;
  a0 = arg(x0, 0);
  a2 = a1;
  a3 = x3;
  a4 = a6;
  a5 = x4;
  a6 = a8;
  a7 = x5;
  a1 = x1;
  qp = (struct goalrec*)&allocp[0];
  allocp += 13;
  execute(norm__guard__goal_8_0);
  goto norm__guard__goal_8_ext_interrupt;
 case functor_atom_1:
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__normalize_xnorm__guard_2F10_240_11);
  allocp[2] = x1 = makeref(&allocp[2]);
  x2 = cdr_of(a0);
  allocp[3] = x2;
  allocp[4] = x3 = makeref(&allocp[4]);
  allocp[5] = a2;
  allocp[6] = a3;
  allocp[7] = a4;
  allocp[8] = a5;
  allocp[9] = x4 = makeref(&allocp[9]);
  allocp[10] = a7;
  allocp[11] = x5 = makeref(&allocp[11]);
  allocp[12] = a9;
  a0 = arg(x0, 0);
  a2 = a1;
  a3 = x3;
  a4 = a6;
  a5 = x4;
  a6 = a8;
  a7 = x5;
  a1 = x1;
  qp = (struct goalrec*)&allocp[0];
  allocp += 13;
  execute(norm__guard__goal_8_0);
  goto norm__guard__goal_8_ext_interrupt;
  };
  goto norm__guard_10_interrupt;
 case VARREF:
  deref_and_jump(x0,norm__guard_10_6);
  *reasonp++ =  x0;
  goto norm__guard_10_interrupt;
  };
  goto norm__guard_10_interrupt;
 case ATOMIC:
  if (a0 != NILATOM) goto norm__guard_10_interrupt;
  allocp[0] = makesym(functor__2D_2);
  allocp[1] = a3;
  allocp[2] = x1 = makeref(&allocp[2]);
  x0 = makefunctor(&allocp[0]);
  allocp[3] = a5;
  allocp[4] = x0;
  x2 = makecons(&allocp[3]);
  allocp += 5;
  unify_value(a4, x2);
  x3 = NILATOM;
  unify_value(a6, x3);
  a0 = a2;
  a2 = a1;
  a3 = a7;
  a4 = a8;
  a5 = a9;
  a1 = x1;
  execute(norm__body_6_0);
  goto norm__body_6_ext_interrupt;
 case VARREF:
  deref_and_jump(a0,norm__guard_10_5);
  *reasonp++ =  a0;
  goto norm__guard_10_interrupt;
  };
  goto norm__guard_10_interrupt;
 norm__guard_10_ext_interrupt:
  reasonp = 0l;
 norm__guard_10_interrupt:
  toppred = &predicate_klic__comp__normalize_xnorm__guard_10;
  goto interrupt_10;
 }

 norm__guard_2F10_241_11_top: {
  q x0, x1, x2;
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
 norm__guard_2F10_241_11_clear_reason:
  reasonp = reasons;
 norm__guard_2F10_241_11_0:
 norm__guard_2F10_241_11_1:
  switch (ptagof(a0)) {
 case ATOMIC:
  if (a0 != makesym(atom_normal)) goto norm__guard_2F10_241_11_2;
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__normalize_xnorm__guard_10);
  allocp[2] = x0 = makeref(&allocp[2]);
  allocp[3] = a4;
  allocp[4] = a5;
  allocp[5] = x1 = makeref(&allocp[5]);
  allocp[6] = a6;
  allocp[7] = a7;
  allocp[8] = x2 = makeref(&allocp[8]);
  allocp[9] = a8;
  allocp[10] = a9;
  allocp[11] = a10;
  allocp[12] = (q)(struct goalrec*)&allocp[0];
  allocp[13] = (q)(&predicate_klic__comp__util_xappend_3);
  allocp[14] = a3;
  allocp[15] = x2;
  allocp[16] = x1;
  allocp[17] = (q)(struct goalrec*)&allocp[12];
  allocp[18] = (q)(&predicate_klic__comp__util_xappend_3);
  allocp[19] = a1;
  allocp[20] = a2;
  allocp[21] = x0;
  qp = (struct goalrec*)&allocp[17];
  allocp += 22;
  proceed();
 case VARREF:
  deref_and_jump(a0,norm__guard_2F10_241_11_1);
  *reasonp++ =  a0;
  goto norm__guard_2F10_241_11_interrupt;
  };
  goto norm__guard_2F10_241_11_2;
 norm__guard_2F10_241_11_2:
  x0 = makesym(atom_normal);
  gblt_diff(a0,x0,norm__guard_2F10_241_11_interrupt);
  unify_value(a8, a0);
  unify(a10, a9);
  proceed();
 norm__guard_2F10_241_11_ext_interrupt:
  reasonp = 0l;
 norm__guard_2F10_241_11_interrupt:
  goto interrupt_11;
 }

 norm__guard_2F10_240_11_top: {
  q x0;
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
 norm__guard_2F10_240_11_clear_reason:
  reasonp = reasons;
 norm__guard_2F10_240_11_0:
 norm__guard_2F10_240_11_1:
  switch (ptagof(a0)) {
 case ATOMIC:
  if (a0 != makesym(atom_normal)) goto norm__guard_2F10_240_11_2;
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
  execute(norm__guard_10_0);
  goto norm__guard_10_ext_interrupt;
 case VARREF:
  deref_and_jump(a0,norm__guard_2F10_240_11_1);
  *reasonp++ =  a0;
  goto norm__guard_2F10_240_11_interrupt;
  };
  goto norm__guard_2F10_240_11_2;
 norm__guard_2F10_240_11_2:
  x0 = makesym(atom_normal);
  gblt_diff(a0,x0,norm__guard_2F10_240_11_interrupt);
  unify_value(a8, a0);
  unify(a10, a9);
  proceed();
 norm__guard_2F10_240_11_ext_interrupt:
  reasonp = 0l;
 norm__guard_2F10_240_11_interrupt:
  goto interrupt_11;
 }

 norm__guard__goal_8_top: {
  q x0, x1, x2, x3, x4, x5, x6, x7;
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  a6 = qp->args[6];
  a7 = qp->args[7];
  qp = qp->next;
 norm__guard__goal_8_clear_reason:
  reasonp = reasons;
 norm__guard__goal_8_0:
 norm__guard__goal_8_1:
  switch (ptagof(a0)) {
 case FUNCTOR:
  switch (symval(functor_of(a0))) {
 case functor__3D_2:
  x0 = arg(a0, 0);
  x1 = arg(a0, 1);
  x2 = a1;
  x3 = a2;
  x4 = a3;
  x5 = a4;
  x6 = a5;
  x7 = a6;
  a8 = a7;
  a7 = x7;
  a6 = x6;
  a5 = x5;
  a4 = x4;
  a3 = x3;
  a2 = x2;
  a1 = x1;
  a0 = x0;
  execute(norm__guard__unify_9_0);
  goto norm__guard__unify_9_ext_interrupt;
 case functor__3A_2:
  x0 = arg(a0, 0);
 norm__guard__goal_8_2:
  switch (ptagof(x0)) {
 case FUNCTOR:
  if (functor_of(x0) != makesym(functor_atom_1)) goto norm__guard__goal_8_4;
  x1 = arg(x0, 0);
 norm__guard__goal_8_3:
  switch (ptagof(x1)) {
 case ATOMIC:
  if (isint(x1)) goto norm__guard__goal_8_4;
  switch (symval(x1)) {
 case symval(makesym(atom_generic)):
  a0 = arg(a0, 1);
  x2 = a2;
  a2 = a3;
  a3 = a1;
  a1 = x2;
  execute(norm__guard__generic_8_0);
  goto norm__guard__generic_8_ext_interrupt;
 case symval(makesym(atom_inline)):
  a0 = arg(a0, 1);
  x2 = a4;
  a4 = a6;
  x3 = a3;
  a6 = a5;
  a5 = x3;
  a3 = x2;
  execute(norm__guard__goal_2F8_241_8_0);
  goto norm__guard__goal_2F8_241_8_ext_interrupt;
  };
  goto norm__guard__goal_8_4;
 case VARREF:
  deref_and_jump(x1,norm__guard__goal_8_3);
  *reasonp++ =  x1;
  goto norm__guard__goal_8_4;
  };
  goto norm__guard__goal_8_4;
 case VARREF:
  deref_and_jump(x0,norm__guard__goal_8_2);
  *reasonp++ =  x0;
  goto norm__guard__goal_8_4;
  };
  goto norm__guard__goal_8_4;
  };
  goto norm__guard__goal_8_4;
 case VARREF:
  deref_and_jump(a0,norm__guard__goal_8_1);
  *reasonp++ =  a0;
  goto norm__guard__goal_8_4;
  };
  goto norm__guard__goal_8_4;
 norm__guard__goal_8_4:
  otherwise(norm__guard__goal_8_interrupt);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__normalize_xnorm__guard__goal_2F8_240_8);
  allocp[2] = x0 = makeref(&allocp[2]);
  allocp[3] = a2;
  allocp[4] = a1;
  allocp[5] = a4;
  allocp[6] = a6;
  allocp[7] = a3;
  allocp[8] = a5;
  allocp[9] = a7;
  allocp[10] = (q)(struct goalrec*)&allocp[0];
  allocp[11] = (q)(&predicate_klic__comp__gb_xis__guard__builtin_2);
  allocp[12] = a0;
  allocp[13] = x0;
  qp = (struct goalrec*)&allocp[10];
  allocp += 14;
  proceed();
 norm__guard__goal_8_ext_interrupt:
  reasonp = 0l;
 norm__guard__goal_8_interrupt:
  toppred = &predicate_klic__comp__normalize_xnorm__guard__goal_8;
  goto interrupt_8;
 }

 norm__guard__goal_2F8_241_8_top: {
  q x0, x1, x2, x3, x4, x5, x6, x7;
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  a6 = qp->args[6];
  a7 = qp->args[7];
  qp = qp->next;
 norm__guard__goal_2F8_241_8_clear_reason:
  reasonp = reasons;
 norm__guard__goal_2F8_241_8_0:
 norm__guard__goal_2F8_241_8_1:
  switch (ptagof(a0)) {
 case FUNCTOR:
  if (functor_of(a0) != makesym(functor_functor_1)) goto norm__guard__goal_2F8_241_8_3;
  x0 = arg(a0, 0);
 norm__guard__goal_2F8_241_8_2:
  switch (ptagof(x0)) {
 case FUNCTOR:
  if (functor_of(x0) != makesym(functor__3A_2)) goto norm__guard__goal_2F8_241_8_3;
  a0 = arg(x0, 1);
  allocp[0] = x2 = makeref(&allocp[0]);
  x1 = x2;
  x3 = x2;
  x4 = arg(x0, 0);
  x5 = a1;
  x6 = a2;
  x7 = a5;
  a8 = a3;
  a9 = a6;
  a10 = a4;
  a11 = a7;
  a7 = x7;
  a6 = x6;
  a5 = x5;
  a4 = x4;
  a3 = NILATOM;
  a2 = x3;
  a1 = x1;
  allocp += 1;
  execute(norm__inline_12_0);
  goto norm__inline_12_ext_interrupt;
 case VARREF:
  deref_and_jump(x0,norm__guard__goal_2F8_241_8_2);
  *reasonp++ =  x0;
  goto norm__guard__goal_2F8_241_8_3;
  };
  goto norm__guard__goal_2F8_241_8_3;
 case VARREF:
  deref_and_jump(a0,norm__guard__goal_2F8_241_8_1);
  *reasonp++ =  a0;
  goto norm__guard__goal_2F8_241_8_3;
  };
  goto norm__guard__goal_2F8_241_8_3;
 norm__guard__goal_2F8_241_8_3:
  otherwise(norm__guard__goal_2F8_241_8_interrupt);
  allocp[0] = x1 = makeref(&allocp[0]);
  x0 = x1;
  x2 = x1;
  x3 = a0;
  x4 = a1;
  x5 = a2;
  x6 = a5;
  a8 = a3;
  a9 = a6;
  a10 = a4;
  a11 = a7;
  a7 = x6;
  a6 = x5;
  a5 = x4;
  a4 = x3;
  a3 = NILATOM;
  a2 = x2;
  a1 = x0;
  a0 = makefunctor(funct_const_17);
  allocp += 1;
  execute(norm__inline_12_0);
  goto norm__inline_12_ext_interrupt;
 norm__guard__goal_2F8_241_8_ext_interrupt:
  reasonp = 0l;
 norm__guard__goal_2F8_241_8_interrupt:
  toppred = &predicate_klic__comp__normalize_xnorm__guard__goal_2F8_241_8;
  goto interrupt_8;
 }

 norm__guard__goal_2F8_240_8_top: {
  q x0, x1, x2;
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  a6 = qp->args[6];
  a7 = qp->args[7];
  qp = qp->next;
 norm__guard__goal_2F8_240_8_clear_reason:
  reasonp = reasons;
 norm__guard__goal_2F8_240_8_0:
 norm__guard__goal_2F8_240_8_1:
  switch (ptagof(a0)) {
 case FUNCTOR:
  switch (symval(functor_of(a0))) {
 case functor_yes_1:
  a0 = arg(a0, 0);
  x0 = a5;
  x1 = a2;
  x2 = a3;
  a5 = a6;
  a6 = a4;
  a4 = x2;
  a3 = x1;
  a2 = x0;
  execute(norm__guard__builts_8_0);
  goto norm__guard__builts_8_ext_interrupt;
 case functor_no_1:
  unify2(a6, a3, a5, a1);
  allocp[0] = NILATOM;
  x1 = arg(a0, 0);
  allocp[1] = x1;
  x0 = makecons(&allocp[0]);
  allocp[2] = (q)qp;
  allocp[3] = (q)(&predicate_klic__comp__message_xreport__error_4);
  allocp[4] = makefunctor(&string_const_16);
  allocp[5] = x0;
  allocp[6] = a4;
  allocp[7] = a7;
  qp = (struct goalrec*)&allocp[2];
  allocp += 8;
  proceed();
  };
  goto norm__guard__goal_2F8_240_8_interrupt;
 case VARREF:
  deref_and_jump(a0,norm__guard__goal_2F8_240_8_1);
  *reasonp++ =  a0;
  goto norm__guard__goal_2F8_240_8_interrupt;
  };
  goto norm__guard__goal_2F8_240_8_interrupt;
 norm__guard__goal_2F8_240_8_ext_interrupt:
  reasonp = 0l;
 norm__guard__goal_2F8_240_8_interrupt:
  goto interrupt_8;
 }

 norm__inline_12_top: {
  q x0, x1, x2, x3, x4, x5, x6, x7, x8, x9;
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
 norm__inline_12_clear_reason:
  reasonp = reasons;
 norm__inline_12_0:
 norm__inline_12_1:
  switch (ptagof(a0)) {
 case FUNCTOR:
  switch (symval(functor_of(a0))) {
 case functor_atom_1:
  x0 = arg(a0, 0);
 norm__inline_12_2:
  switch (ptagof(x0)) {
 case ATOMIC:
  if (x0 != NILATOM) goto norm__inline_12_interrupt;
  unify2(a11, a10, a7, a6);
  x1 = NILATOM;
  unify_value(a1, x1);
  allocp[0] = makesym(functor_il_3);
  allocp[1] = a4;
  allocp[2] = a2;
  allocp[3] = a3;
  x2 = makefunctor(&allocp[0]);
  allocp[4] = a9;
  allocp[5] = x2;
  x3 = makecons(&allocp[4]);
  allocp += 6;
  unify_value(a8, x3);
  x4 = makesym(atom_normal);
  unify_value(a5, x4);
  proceed();
 case VARREF:
  deref_and_jump(x0,norm__inline_12_2);
  *reasonp++ =  x0;
  goto norm__inline_12_interrupt;
  };
  goto norm__inline_12_interrupt;
 case functor_list_1:
  x0 = arg(a0, 0);
 norm__inline_12_3:
  switch (ptagof(x0)) {
 case CONS:
  allocp[0] = x2 = makeref(&allocp[0]);
  allocp[1] = x3 = makeref(&allocp[1]);
  x1 = makecons(&allocp[0]);
  allocp += 2;
  unify_value(a1, x1);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__normalize_xnorm__inline_2F12_240_13);
  allocp[2] = x4 = makeref(&allocp[2]);
  x5 = cdr_of(x0);
  allocp[3] = x5;
  allocp[4] = x2;
  allocp[5] = a2;
  allocp[6] = x6 = makeref(&allocp[6]);
  allocp[7] = a4;
  allocp[8] = a5;
  allocp[9] = x7 = makeref(&allocp[9]);
  allocp[10] = x8 = makeref(&allocp[10]);
  allocp[11] = x9 = makeref(&allocp[11]);
  allocp[12] = a7;
  allocp[13] = a9;
  allocp[14] = a11;
  a0 = car_of(x0);
  a1 = x3;
  a2 = x4;
  a4 = x6;
  a5 = a6;
  a6 = x7;
  a7 = a8;
  a8 = x8;
  a9 = a10;
  a10 = x9;
  qp = (struct goalrec*)&allocp[0];
  allocp += 15;
  execute(norm__inline__arg_11_0);
  goto norm__inline__arg_11_ext_interrupt;
 case VARREF:
  deref_and_jump(x0,norm__inline_12_3);
  *reasonp++ =  x0;
  goto norm__inline_12_interrupt;
  };
  goto norm__inline_12_interrupt;
  };
  goto norm__inline_12_interrupt;
 case VARREF:
  deref_and_jump(a0,norm__inline_12_1);
  *reasonp++ =  a0;
  goto norm__inline_12_interrupt;
  };
  goto norm__inline_12_interrupt;
 norm__inline_12_ext_interrupt:
  reasonp = 0l;
 norm__inline_12_interrupt:
  toppred = &predicate_klic__comp__normalize_xnorm__inline_12;
  goto interrupt_12;
 }

 norm__inline_2F12_240_13_top: {
  q x0;
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
  qp = qp->next;
 norm__inline_2F12_240_13_clear_reason:
  reasonp = reasons;
 norm__inline_2F12_240_13_0:
 norm__inline_2F12_240_13_1:
  switch (ptagof(a0)) {
 case ATOMIC:
  if (a0 != makesym(atom_normal)) goto norm__inline_2F12_240_13_2;
  a0 = a1;
  a1 = a2;
  a2 = a3;
  a3 = a4;
  a4 = a5;
  a5 = a6;
  a6 = a7;
  a7 = a10;
  x0 = a11;
  a10 = a9;
  a11 = a12;
  a9 = x0;
  execute(norm__inline_12_0);
  goto norm__inline_12_ext_interrupt;
 case VARREF:
  deref_and_jump(a0,norm__inline_2F12_240_13_1);
  *reasonp++ =  a0;
  goto norm__inline_2F12_240_13_interrupt;
  };
  goto norm__inline_2F12_240_13_2;
 norm__inline_2F12_240_13_2:
  x0 = makesym(atom_normal);
  gblt_diff(a0,x0,norm__inline_2F12_240_13_interrupt);
  unify_value(a6, a0);
  unify3(a10, a7, a11, a8, a12, a9);
  proceed();
 norm__inline_2F12_240_13_ext_interrupt:
  reasonp = 0l;
 norm__inline_2F12_240_13_interrupt:
  goto interrupt_13;
 }

 norm__inline__arg_11_top: {
  q x0, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13, x14, x15, x16;
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
 norm__inline__arg_11_clear_reason:
  reasonp = reasons;
 norm__inline__arg_11_0:
 norm__inline__arg_11_1:
  switch (ptagof(a0)) {
 case FUNCTOR:
  if (functor_of(a0) != makesym(functor_functor_1)) goto norm__inline__arg_11_5;
  x0 = arg(a0, 0);
 norm__inline__arg_11_2:
  switch (ptagof(x0)) {
 case FUNCTOR:
  switch (symval(functor_of(x0))) {
 case functor__2B_2:
  x1 = arg(x0, 0);
 norm__inline__arg_11_3:
  switch (ptagof(x1)) {
 case FUNCTOR:
  if (functor_of(x1) != makesym(functor_variable_1)) goto norm__inline__arg_11_5;
  allocp[0] = makesym(functor__2B_1);
  allocp[1] = x3 = makeref(&allocp[1]);
  x2 = makefunctor(&allocp[0]);
  allocp += 2;
  unify_value(a1, x2);
  unify2(a4, a3, a6, a5);
  allocp[0] = makesym(functor_variable_1);
  x5 = arg(x1, 0);
  allocp[1] = x5;
  x4 = makefunctor(&allocp[0]);
  allocp[2] = (q)qp;
  allocp[3] = (q)(&predicate_klic__comp__normalize_xnorm__gblt__arg_9);
  allocp[4] = x4;
  allocp[5] = x6 = makeref(&allocp[5]);
  allocp[6] = x3;
  allocp[7] = a6;
  allocp[8] = a2;
  allocp[9] = a7;
  allocp[10] = a8;
  allocp[11] = a9;
  allocp[12] = a10;
  a0 = arg(x0, 1);
  a1 = x6;
  qp = (struct goalrec*)&allocp[2];
  allocp += 13;
  execute(inline__arg__type_2_0);
  goto inline__arg__type_2_ext_interrupt;
 case VARREF:
  deref_and_jump(x1,norm__inline__arg_11_3);
  *reasonp++ =  x1;
  goto norm__inline__arg_11_5;
  };
  goto norm__inline__arg_11_5;
 case functor__2D_2:
  x1 = arg(x0, 0);
 norm__inline__arg_11_4:
  switch (ptagof(x1)) {
 case FUNCTOR:
  if (functor_of(x1) != makesym(functor_variable_1)) goto norm__inline__arg_11_5;
  allocp[0] = makesym(functor_var_1);
  x3 = arg(x1, 0);
  allocp[1] = x3;
  x2 = makefunctor(&allocp[0]);
  allocp[2] = makesym(functor__2D_1);
  allocp[3] = x2;
  x4 = makefunctor(&allocp[2]);
  allocp += 4;
  unify_value(a1, x4);
  allocp[0] = makesym(functor__3A_2);
  x6 = arg(x1, 0);
  allocp[1] = x6;
  allocp[2] = x7 = makeref(&allocp[2]);
  x5 = makefunctor(&allocp[0]);
  allocp[3] = a3;
  allocp[4] = x5;
  x8 = makecons(&allocp[3]);
  allocp += 5;
  unify_value(a4, x8);
  allocp[0] = makesym(functor_var_1);
  x10 = arg(x1, 0);
  allocp[1] = x10;
  x9 = makefunctor(&allocp[0]);
  allocp[2] = makesym(functor__3D_2);
  x12 = arg(x1, 0);
  allocp[3] = x12;
  allocp[4] = x9;
  x11 = makefunctor(&allocp[2]);
  allocp[5] = a5;
  allocp[6] = x11;
  x13 = makecons(&allocp[5]);
  allocp += 7;
  unify_value(a6, x13);
  unify(a8, a7);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__normalize_xnorm__inline__arg__1_7);
  allocp[2] = x14 = makeref(&allocp[2]);
  allocp[3] = a5;
  x15 = arg(x1, 0);
  allocp[4] = x15;
  allocp[5] = x7;
  allocp[6] = a2;
  allocp[7] = a9;
  allocp[8] = a10;
  allocp[9] = (q)(struct goalrec*)&allocp[0];
  allocp[10] = (q)(&predicate_klic__comp__normalize_xinline__arg__type_2);
  x16 = arg(x0, 1);
  allocp[11] = x16;
  allocp[12] = x7;
  allocp[13] = (q)(struct goalrec*)&allocp[9];
  allocp[14] = (q)(&predicate_klic__comp__util_xassoc_3);
  allocp[15] = a5;
  allocp[16] = x15;
  allocp[17] = x14;
  qp = (struct goalrec*)&allocp[13];
  allocp += 18;
  proceed();
 case VARREF:
  deref_and_jump(x1,norm__inline__arg_11_4);
  *reasonp++ =  x1;
  goto norm__inline__arg_11_5;
  };
  goto norm__inline__arg_11_5;
  };
  goto norm__inline__arg_11_5;
 case VARREF:
  deref_and_jump(x0,norm__inline__arg_11_2);
  *reasonp++ =  x0;
  goto norm__inline__arg_11_5;
  };
  goto norm__inline__arg_11_5;
 case VARREF:
  deref_and_jump(a0,norm__inline__arg_11_1);
  *reasonp++ =  a0;
  goto norm__inline__arg_11_5;
  };
  goto norm__inline__arg_11_5;
 norm__inline__arg_11_5:
  otherwise(norm__inline__arg_11_interrupt);
  unify2(a8, a7, a4, a3);
  x0 = makesym(atom_abnormal);
  unify_value(a2, x0);
  allocp[0] = NILATOM;
  allocp[1] = a0;
  x1 = makecons(&allocp[0]);
  allocp[2] = (q)qp;
  allocp[3] = (q)(&predicate_klic__comp__message_xwarning_4);
  allocp[4] = makefunctor(&string_const_3);
  allocp[5] = x1;
  allocp[6] = a9;
  allocp[7] = a10;
  qp = (struct goalrec*)&allocp[2];
  allocp += 8;
  proceed();
 norm__inline__arg_11_ext_interrupt:
  reasonp = 0l;
 norm__inline__arg_11_interrupt:
  toppred = &predicate_klic__comp__normalize_xnorm__inline__arg_11;
  goto interrupt_11;
 }

 norm__inline__arg__1_7_top: {
  q x0, x1, x2, x3;
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  a6 = qp->args[6];
  qp = qp->next;
 norm__inline__arg__1_7_clear_reason:
  reasonp = reasons;
 norm__inline__arg__1_7_0:
 norm__inline__arg__1_7_1:
  switch (ptagof(a0)) {
 case ATOMIC:
  if (a0 != makesym(atom_no)) goto norm__inline__arg__1_7_interrupt;
  unify(a6, a5);
  x0 = makesym(atom_normal);
  unify_value(a4, x0);
  proceed();
 case FUNCTOR:
  if (functor_of(a0) != makesym(functor_yes_1)) goto norm__inline__arg__1_7_interrupt;
  allocp[0] = makesym(functor_variable_1);
  allocp[1] = a2;
  x0 = makefunctor(&allocp[0]);
  allocp[2] = makesym(functor__2D_2);
  allocp[3] = x0;
  allocp[4] = a3;
  x1 = makefunctor(&allocp[2]);
  x2 = makesym(atom_abnormal);
  allocp += 5;
  unify_value(a4, x2);
  allocp[0] = NILATOM;
  allocp[1] = x1;
  x3 = makecons(&allocp[0]);
  allocp[2] = (q)qp;
  allocp[3] = (q)(&predicate_klic__comp__message_xwarning_4);
  allocp[4] = makefunctor(&string_const_12);
  allocp[5] = x3;
  allocp[6] = a5;
  allocp[7] = a6;
  qp = (struct goalrec*)&allocp[2];
  allocp += 8;
  proceed();
 case VARREF:
  deref_and_jump(a0,norm__inline__arg__1_7_1);
  *reasonp++ =  a0;
  goto norm__inline__arg__1_7_interrupt;
  };
  goto norm__inline__arg__1_7_interrupt;
 norm__inline__arg__1_7_ext_interrupt:
  reasonp = 0l;
 norm__inline__arg__1_7_interrupt:
  goto interrupt_7;
 }

 inline__arg__type_2_top: {
  q x0, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11;
  qp = qp->next;
 inline__arg__type_2_clear_reason:
  reasonp = reasons;
 inline__arg__type_2_0:
 inline__arg__type_2_1:
  switch (ptagof(a0)) {
 case FUNCTOR:
  switch (symval(functor_of(a0))) {
 case functor_atom_1:
  x0 = arg(a0, 0);
  unify(a1, x0);
  proceed();
 case functor_functor_1:
  x0 = arg(a0, 0);
 inline__arg__type_2_2:
  if (isref(x0)) goto inline__arg__type_2_4;
  gblt_pfunctor(x0,x1,inline__arg__type_2_interrupt);
  gblt_arity(x0,x2,inline__arg__type_2_interrupt);
 inline__arg__type_2_3:
  switch ((unsigned long)x2) {
 case (unsigned long) makeint(1L):
  x4 = makeint(1L);
  gblt_arg(x4,x0,x3,inline__arg__type_2_interrupt);
  allocp[0] = NILATOM;
  allocp[1] = x6 = makeref(&allocp[1]);
  x5 = makecons(&allocp[0]);
  allocp[2] = x5;
  allocp[3] = x1;
  x7 = makecons(&allocp[2]);
  allocp[4] = (q)qp;
  allocp[5] = (q)(&predicate_functor__table_x_3D_2E_2E_2);
  allocp[6] = a1;
  allocp[7] = x7;
  a0 = x3;
  a1 = x6;
  qp = (struct goalrec*)&allocp[4];
  allocp += 8;
  execute(inline__arg__type_2_0);
  goto inline__arg__type_2_ext_interrupt;
 case (unsigned long) makeint(2L):
  x4 = makeint(1L);
  gblt_arg(x4,x0,x3,inline__arg__type_2_interrupt);
  x6 = makeint(1L);
  gblt_arg(x6,x0,x5,inline__arg__type_2_interrupt);
  allocp[0] = NILATOM;
  allocp[1] = x8 = makeref(&allocp[1]);
  x7 = makecons(&allocp[0]);
  allocp[2] = x7;
  allocp[3] = x10 = makeref(&allocp[3]);
  x9 = makecons(&allocp[2]);
  allocp[4] = x9;
  allocp[5] = x1;
  x11 = makecons(&allocp[4]);
  allocp[6] = (q)qp;
  allocp[7] = (q)(&predicate_functor__table_x_3D_2E_2E_2);
  allocp[8] = a1;
  allocp[9] = x11;
  allocp[10] = (q)(struct goalrec*)&allocp[6];
  allocp[11] = (q)(&predicate_klic__comp__normalize_xinline__arg__type_2);
  allocp[12] = x5;
  allocp[13] = x8;
  a0 = x5;
  a1 = x10;
  qp = (struct goalrec*)&allocp[10];
  allocp += 14;
  execute(inline__arg__type_2_0);
  goto inline__arg__type_2_ext_interrupt;
  };
  goto inline__arg__type_2_interrupt;
 inline__arg__type_2_4:
  deref_and_jump(x0,inline__arg__type_2_2);
  *reasonp++ =  x0;
  goto inline__arg__type_2_interrupt;
  };
  goto inline__arg__type_2_interrupt;
 case VARREF:
  deref_and_jump(a0,inline__arg__type_2_1);
  *reasonp++ =  a0;
  goto inline__arg__type_2_interrupt;
  };
  goto inline__arg__type_2_interrupt;
 inline__arg__type_2_ext_interrupt:
  reasonp = 0l;
 inline__arg__type_2_interrupt:
  toppred = &predicate_klic__comp__normalize_xinline__arg__type_2;
  goto interrupt_2;
 }

 norm__guard__unify_9_top: {
  q x0, x1, x2, x3, x4, x5, x6, x7;
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  a6 = qp->args[6];
  a7 = qp->args[7];
  a8 = qp->args[8];
  qp = qp->next;
 norm__guard__unify_9_clear_reason:
  reasonp = reasons;
 norm__guard__unify_9_0:
 norm__guard__unify_9_1:
  if (isref(a0)) goto norm__guard__unify_9_4;
 norm__guard__unify_9_2:
  if (isref(a0)) goto norm__guard__unify_9_3;
  if_not_equal(a0, a1, norm__guard__unify_9_5);
  unify3(a8, a7, a6, a5, a4, a3);
  x0 = makesym(atom_normal);
  unify_value(a2, x0);
  proceed();
 norm__guard__unify_9_3:
  deref_and_jump(a0,norm__guard__unify_9_2);
  *reasonp++ =  a0;
  goto norm__guard__unify_9_5;
 norm__guard__unify_9_4:
  deref_and_jump(a0,norm__guard__unify_9_1);
  *reasonp++ =  a0;
  goto norm__guard__unify_9_5;
 norm__guard__unify_9_5:
  otherwise(norm__guard__unify_9_interrupt);
 norm__guard__unify_9_6:
  switch (ptagof(a0)) {
 case FUNCTOR:
  switch (symval(functor_of(a0))) {
 case functor_variable_1:
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__normalize_xnorm__guard__unify_2F9_241_10);
  allocp[2] = x0 = makeref(&allocp[2]);
  allocp[3] = a1;
  allocp[4] = a2;
  allocp[5] = a3;
  allocp[6] = a5;
  allocp[7] = a7;
  allocp[8] = a4;
  allocp[9] = a6;
  allocp[10] = a8;
  x1 = arg(a0, 0);
  allocp[11] = x1;
  allocp[12] = (q)(struct goalrec*)&allocp[0];
  allocp[13] = (q)(&predicate_klic__comp__util_xassoc_3);
  allocp[14] = a3;
  allocp[15] = x1;
  allocp[16] = x0;
  qp = (struct goalrec*)&allocp[12];
  allocp += 17;
  proceed();
 case functor_list_1:
  x0 = arg(a0, 0);
 norm__guard__unify_9_7:
  switch (ptagof(x0)) {
 case CONS:
 norm__guard__unify_9_8:
  switch (ptagof(a1)) {
 case FUNCTOR:
  if (functor_of(a1) != makesym(functor_list_1)) goto norm__guard__unify_9_32;
  x1 = arg(a1, 0);
 norm__guard__unify_9_9:
  switch (ptagof(x1)) {
 case CONS:
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__normalize_xnorm__guard__unify_2F9_240_10);
  allocp[2] = x2 = makeref(&allocp[2]);
  x3 = cdr_of(x0);
  allocp[3] = x3;
  x4 = cdr_of(x1);
  allocp[4] = x4;
  allocp[5] = a2;
  allocp[6] = x5 = makeref(&allocp[6]);
  allocp[7] = x6 = makeref(&allocp[7]);
  allocp[8] = x7 = makeref(&allocp[8]);
  allocp[9] = a4;
  allocp[10] = a6;
  allocp[11] = a8;
  a0 = car_of(x0);
  a1 = car_of(x1);
  a2 = x2;
  a4 = x5;
  a6 = x6;
  a8 = x7;
  qp = (struct goalrec*)&allocp[0];
  allocp += 12;
  execute(norm__guard__unify_9_0);
  goto norm__guard__unify_9_ext_interrupt;
 case VARREF:
  deref_and_jump(x1,norm__guard__unify_9_9);
  *reasonp++ =  x1;
  goto norm__guard__unify_9_32;
  };
  goto norm__guard__unify_9_32;
 case VARREF:
  deref_and_jump(a1,norm__guard__unify_9_8);
  *reasonp++ =  a1;
  goto norm__guard__unify_9_32;
  };
  goto norm__guard__unify_9_32;
 case VARREF:
  deref_and_jump(x0,norm__guard__unify_9_7);
  *reasonp++ =  x0;
  goto norm__guard__unify_9_32;
  };
  goto norm__guard__unify_9_32;
 case functor_functor_1:
 norm__guard__unify_9_10:
  switch (ptagof(a1)) {
 case FUNCTOR:
  if (functor_of(a1) != makesym(functor_functor_1)) goto norm__guard__unify_9_32;
  x0 = arg(a0, 0);
 norm__guard__unify_9_11:
  if (isref(x0)) goto norm__guard__unify_9_22;
  gblt_pfunctor(x0,x1,norm__guard__unify_9_32);
  gblt_arity(x0,x2,norm__guard__unify_9_32);
  x3 = arg(a1, 0);
 norm__guard__unify_9_12:
  if (isref(x3)) goto norm__guard__unify_9_21;
  gblt_pfunctor(x3,x4,norm__guard__unify_9_32);
 norm__guard__unify_9_13:
  if (isref(x4)) goto norm__guard__unify_9_20;
 norm__guard__unify_9_14:
  if (isref(x4)) goto norm__guard__unify_9_19;
  if_not_equal(x4, x1, norm__guard__unify_9_32);
  gblt_arity(x3,x5,norm__guard__unify_9_32);
 norm__guard__unify_9_15:
  if (isref(x5)) goto norm__guard__unify_9_18;
 norm__guard__unify_9_16:
  if (isref(x5)) goto norm__guard__unify_9_17;
  if_not_equal(x5, x2, norm__guard__unify_9_32);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__normalize_xnorm__guard__unify__args_10);
  allocp[2] = x6 = makeref(&allocp[2]);
  allocp[3] = x7 = makeref(&allocp[3]);
  allocp[4] = a3;
  allocp[5] = a4;
  allocp[6] = makesym(atom_normal);
  allocp[7] = a2;
  allocp[8] = a5;
  allocp[9] = a6;
  allocp[10] = a7;
  allocp[11] = a8;
  allocp[12] = (q)(struct goalrec*)&allocp[0];
  allocp[13] = (q)(&predicate_klic__comp__util_xuniv_2);
  allocp[14] = x3;
  allocp[15] = x7;
  allocp[16] = (q)(struct goalrec*)&allocp[12];
  allocp[17] = (q)(&predicate_klic__comp__util_xuniv_2);
  allocp[18] = x0;
  allocp[19] = x6;
  qp = (struct goalrec*)&allocp[16];
  allocp += 20;
  proceed();
 norm__guard__unify_9_17:
  deref_and_jump(x5,norm__guard__unify_9_16);
  *reasonp++ =  x5;
  goto norm__guard__unify_9_32;
 norm__guard__unify_9_18:
  deref_and_jump(x5,norm__guard__unify_9_15);
  *reasonp++ =  x5;
  goto norm__guard__unify_9_32;
 norm__guard__unify_9_19:
  deref_and_jump(x4,norm__guard__unify_9_14);
  *reasonp++ =  x4;
  goto norm__guard__unify_9_32;
 norm__guard__unify_9_20:
  deref_and_jump(x4,norm__guard__unify_9_13);
  *reasonp++ =  x4;
  goto norm__guard__unify_9_32;
 norm__guard__unify_9_21:
  deref_and_jump(x3,norm__guard__unify_9_12);
  *reasonp++ =  x3;
  goto norm__guard__unify_9_32;
 norm__guard__unify_9_22:
  deref_and_jump(x0,norm__guard__unify_9_11);
  *reasonp++ =  x0;
  goto norm__guard__unify_9_32;
 case VARREF:
  deref_and_jump(a1,norm__guard__unify_9_10);
  *reasonp++ =  a1;
  goto norm__guard__unify_9_32;
  };
  goto norm__guard__unify_9_32;
 case functor_vector_1:
 norm__guard__unify_9_23:
  switch (ptagof(a1)) {
 case FUNCTOR:
  if (functor_of(a1) != makesym(functor_vector_1)) goto norm__guard__unify_9_32;
  x0 = arg(a0, 0);
 norm__guard__unify_9_24:
  switch (ptagof(x0)) {
 case FUNCTOR:
 norm__guard__unify_9_25:
  if (!isgobj(x0)) goto norm__guard__unify_9_32;
  if (!isclass(x0,vector)) goto norm__guard__unify_9_32;
  gblt_size_of_vector(x0,x1,norm__guard__unify_9_32);
  x2 = arg(a1, 0);
 norm__guard__unify_9_26:
  switch (ptagof(x2)) {
 case FUNCTOR:
 norm__guard__unify_9_27:
  if (!isgobj(x2)) goto norm__guard__unify_9_32;
  if (!isclass(x2,vector)) goto norm__guard__unify_9_32;
  gblt_size_of_vector(x2,x3,norm__guard__unify_9_32);
 norm__guard__unify_9_28:
  if (isref(x3)) goto norm__guard__unify_9_31;
 norm__guard__unify_9_29:
  if (isref(x3)) goto norm__guard__unify_9_30;
  if_not_equal(x3, x1, norm__guard__unify_9_32);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__normalize_xnorm__guard__unify__args_10);
  allocp[2] = x4 = makeref(&allocp[2]);
  allocp[3] = x5 = makeref(&allocp[3]);
  allocp[4] = a3;
  allocp[5] = a4;
  allocp[6] = makesym(atom_normal);
  allocp[7] = a2;
  allocp[8] = a5;
  allocp[9] = a6;
  allocp[10] = a7;
  allocp[11] = a8;
  allocp[12] = (q)(struct goalrec*)&allocp[0];
  allocp[13] = (q)(&predicate_klic__comp__util_xvector__to__list_2);
  allocp[14] = x2;
  allocp[15] = x5;
  allocp[16] = (q)(struct goalrec*)&allocp[12];
  allocp[17] = (q)(&predicate_klic__comp__util_xvector__to__list_2);
  allocp[18] = x0;
  allocp[19] = x4;
  qp = (struct goalrec*)&allocp[16];
  allocp += 20;
  proceed();
 norm__guard__unify_9_30:
  deref_and_jump(x3,norm__guard__unify_9_29);
  *reasonp++ =  x3;
  goto norm__guard__unify_9_32;
 norm__guard__unify_9_31:
  deref_and_jump(x3,norm__guard__unify_9_28);
  *reasonp++ =  x3;
  goto norm__guard__unify_9_32;
 case VARREF:
  deref_and_jump(x2,norm__guard__unify_9_26);
  *reasonp++ =  x2;
  goto norm__guard__unify_9_32;
  };
  goto norm__guard__unify_9_32;
 case VARREF:
  deref_and_jump(x0,norm__guard__unify_9_24);
  *reasonp++ =  x0;
  goto norm__guard__unify_9_32;
  };
  goto norm__guard__unify_9_32;
 case VARREF:
  deref_and_jump(a1,norm__guard__unify_9_23);
  *reasonp++ =  a1;
  goto norm__guard__unify_9_32;
  };
  goto norm__guard__unify_9_32;
  };
  goto norm__guard__unify_9_32;
 case VARREF:
  deref_and_jump(a0,norm__guard__unify_9_6);
  *reasonp++ =  a0;
  goto norm__guard__unify_9_32;
  };
  goto norm__guard__unify_9_32;
 norm__guard__unify_9_32:
  otherwise(norm__guard__unify_9_interrupt);
 norm__guard__unify_9_33:
  switch (ptagof(a1)) {
 case FUNCTOR:
  if (functor_of(a1) != makesym(functor_variable_1)) goto norm__guard__unify_9_34;
  allocp[0] = makesym(functor_variable_1);
  x1 = arg(a1, 0);
  allocp[1] = x1;
  x0 = makefunctor(&allocp[0]);
  a1 = a0;
  a0 = x0;
  allocp += 2;
  execute(norm__guard__unify_9_0);
  goto norm__guard__unify_9_ext_interrupt;
 case VARREF:
  deref_and_jump(a1,norm__guard__unify_9_33);
  *reasonp++ =  a1;
  goto norm__guard__unify_9_34;
  };
  goto norm__guard__unify_9_34;
 norm__guard__unify_9_34:
  otherwise(norm__guard__unify_9_interrupt);
  unify2(a6, a5, a4, a3);
  x0 = makesym(atom_abnormal);
  unify_value(a2, x0);
  allocp[0] = NILATOM;
  allocp[1] = a1;
  x1 = makecons(&allocp[0]);
  allocp[2] = x1;
  allocp[3] = a0;
  x2 = makecons(&allocp[2]);
  allocp[4] = (q)qp;
  allocp[5] = (q)(&predicate_klic__comp__message_xwarning_4);
  allocp[6] = makefunctor(&string_const_15);
  allocp[7] = x2;
  allocp[8] = a7;
  allocp[9] = a8;
  qp = (struct goalrec*)&allocp[4];
  allocp += 10;
  proceed();
 norm__guard__unify_9_ext_interrupt:
  reasonp = 0l;
 norm__guard__unify_9_interrupt:
  toppred = &predicate_klic__comp__normalize_xnorm__guard__unify_9;
  goto interrupt_9;
 }

 norm__guard__unify_2F9_241_10_top: {
  q x0;
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  a6 = qp->args[6];
  a7 = qp->args[7];
  a8 = qp->args[8];
  a9 = qp->args[9];
  qp = qp->next;
 norm__guard__unify_2F9_241_10_clear_reason:
  reasonp = reasons;
 norm__guard__unify_2F9_241_10_0:
 norm__guard__unify_2F9_241_10_1:
  switch (ptagof(a0)) {
 case ATOMIC:
  if (a0 != makesym(atom_no)) goto norm__guard__unify_2F9_241_10_interrupt;
  x0 = makesym(atom_normal);
  unify_value(a2, x0);
  unify(a7, a4);
  a0 = a1;
  a1 = a9;
  a2 = a3;
  a3 = a6;
  a4 = a5;
  a5 = a8;
  execute(norm__guard__unify__new_6_0);
  goto norm__guard__unify__new_6_ext_interrupt;
 case FUNCTOR:
  if (functor_of(a0) != makesym(functor_yes_1)) goto norm__guard__unify_2F9_241_10_interrupt;
  a0 = arg(a0, 0);
  execute(norm__guard__unify_2F9_241_2F10_240_9_0);
  goto norm__guard__unify_2F9_241_2F10_240_9_ext_interrupt;
 case VARREF:
  deref_and_jump(a0,norm__guard__unify_2F9_241_10_1);
  *reasonp++ =  a0;
  goto norm__guard__unify_2F9_241_10_interrupt;
  };
  goto norm__guard__unify_2F9_241_10_interrupt;
 norm__guard__unify_2F9_241_10_ext_interrupt:
  reasonp = 0l;
 norm__guard__unify_2F9_241_10_interrupt:
  goto interrupt_10;
 }

 norm__guard__unify_2F9_241_2F10_240_9_top: {
  q x0, x1, x2, x3;
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  a6 = qp->args[6];
  a7 = qp->args[7];
  a8 = qp->args[8];
  qp = qp->next;
 norm__guard__unify_2F9_241_2F10_240_9_clear_reason:
  reasonp = reasons;
 norm__guard__unify_2F9_241_2F10_240_9_0:
 norm__guard__unify_2F9_241_2F10_240_9_1:
  switch (ptagof(a0)) {
 case FUNCTOR:
  if (functor_of(a0) != makesym(functor_rename_1)) goto norm__guard__unify_2F9_241_2F10_240_9_2;
  allocp[0] = makesym(functor_variable_1);
  x1 = arg(a0, 0);
  allocp[1] = x1;
  x0 = makefunctor(&allocp[0]);
  a0 = x0;
  x2 = a6;
  x3 = a4;
  a6 = a7;
  a7 = a5;
  a5 = x3;
  a4 = x2;
  allocp += 2;
  execute(norm__guard__unify_9_0);
  goto norm__guard__unify_9_ext_interrupt;
 case VARREF:
  deref_and_jump(a0,norm__guard__unify_2F9_241_2F10_240_9_1);
  *reasonp++ =  a0;
  goto norm__guard__unify_2F9_241_2F10_240_9_2;
  };
  goto norm__guard__unify_2F9_241_2F10_240_9_2;
 norm__guard__unify_2F9_241_2F10_240_9_2:
  otherwise(norm__guard__unify_2F9_241_2F10_240_9_interrupt);
  x0 = makesym(atom_normal);
  unify_value(a2, x0);
  unify(a8, a5);
  x1 = a1;
  a1 = a0;
  a2 = a3;
  a3 = a6;
  a5 = a7;
  a0 = x1;
  execute(norm__guard__term_6_0);
  goto norm__guard__term_6_ext_interrupt;
 norm__guard__unify_2F9_241_2F10_240_9_ext_interrupt:
  reasonp = 0l;
 norm__guard__unify_2F9_241_2F10_240_9_interrupt:
  toppred = &predicate_klic__comp__normalize_xnorm__guard__unify_2F9_241_2F10_240_9;
  goto interrupt_9;
 }

 norm__guard__unify_2F9_240_10_top: {
  q x0;
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  a6 = qp->args[6];
  a7 = qp->args[7];
  a8 = qp->args[8];
  a9 = qp->args[9];
  qp = qp->next;
 norm__guard__unify_2F9_240_10_clear_reason:
  reasonp = reasons;
 norm__guard__unify_2F9_240_10_0:
 norm__guard__unify_2F9_240_10_1:
  switch (ptagof(a0)) {
 case ATOMIC:
  if (a0 != makesym(atom_normal)) goto norm__guard__unify_2F9_240_10_2;
  a0 = a1;
  a1 = a2;
  a2 = a3;
  a3 = a4;
  a4 = a7;
  x0 = a8;
  a7 = a6;
  a8 = a9;
  a6 = x0;
  execute(norm__guard__unify_9_0);
  goto norm__guard__unify_9_ext_interrupt;
 case VARREF:
  deref_and_jump(a0,norm__guard__unify_2F9_240_10_1);
  *reasonp++ =  a0;
  goto norm__guard__unify_2F9_240_10_interrupt;
  };
  goto norm__guard__unify_2F9_240_10_2;
 norm__guard__unify_2F9_240_10_2:
  x0 = makesym(atom_normal);
  gblt_diff(a0,x0,norm__guard__unify_2F9_240_10_interrupt);
  unify_value(a3, a0);
  unify3(a7, a4, a8, a5, a9, a6);
  proceed();
 norm__guard__unify_2F9_240_10_ext_interrupt:
  reasonp = 0l;
 norm__guard__unify_2F9_240_10_interrupt:
  goto interrupt_10;
 }

 norm__guard__unify__new_6_top: {
  q x0, x1, x2, x3;
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  qp = qp->next;
 norm__guard__unify__new_6_clear_reason:
  reasonp = reasons;
 norm__guard__unify__new_6_0:
 norm__guard__unify__new_6_1:
  switch (ptagof(a0)) {
 case FUNCTOR:
  switch (symval(functor_of(a0))) {
 case functor_variable_1:
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__normalize_xnorm__guard__unify__new_2F6_240_7);
  allocp[2] = x0 = makeref(&allocp[2]);
  allocp[3] = a3;
  allocp[4] = a1;
  allocp[5] = a2;
  allocp[6] = a5;
  allocp[7] = a4;
  x1 = arg(a0, 0);
  allocp[8] = x1;
  allocp[9] = (q)(struct goalrec*)&allocp[0];
  allocp[10] = (q)(&predicate_klic__comp__util_xassoc_3);
  allocp[11] = a2;
  allocp[12] = x1;
  allocp[13] = x0;
  qp = (struct goalrec*)&allocp[9];
  allocp += 14;
  proceed();
 case functor_atom_1:
  unify(a5, a4);
  allocp[0] = makesym(functor__3D_2);
  allocp[1] = a1;
  allocp[2] = a0;
  x0 = makefunctor(&allocp[0]);
  allocp[3] = a2;
  allocp[4] = x0;
  x1 = makecons(&allocp[3]);
  allocp += 5;
  unify_value(a3, x1);
  proceed();
 case functor_float_1:
  unify(a5, a4);
  allocp[0] = makesym(functor__3D_2);
  allocp[1] = a1;
  allocp[2] = a0;
  x0 = makefunctor(&allocp[0]);
  allocp[3] = a2;
  allocp[4] = x0;
  x1 = makecons(&allocp[3]);
  allocp += 5;
  unify_value(a3, x1);
  proceed();
 case functor_predicate_3:
  unify(a5, a4);
  allocp[0] = makesym(functor__3D_2);
  allocp[1] = a1;
  allocp[2] = a0;
  x0 = makefunctor(&allocp[0]);
  allocp[3] = a2;
  allocp[4] = x0;
  x1 = makecons(&allocp[3]);
  allocp += 5;
  unify_value(a3, x1);
  proceed();
 case functor_integer_1:
  unify(a5, a4);
  allocp[0] = makesym(functor_int_1);
  x1 = arg(a0, 0);
  allocp[1] = x1;
  x0 = makefunctor(&allocp[0]);
  allocp[2] = makesym(functor__3D_2);
  allocp[3] = a1;
  allocp[4] = x0;
  x2 = makefunctor(&allocp[2]);
  allocp[5] = a2;
  allocp[6] = x2;
  x3 = makecons(&allocp[5]);
  allocp += 7;
  unify_value(a3, x3);
  proceed();
  };
  goto norm__guard__unify__new_6_2;
 case VARREF:
  deref_and_jump(a0,norm__guard__unify__new_6_1);
  *reasonp++ =  a0;
  goto norm__guard__unify__new_6_2;
  };
  goto norm__guard__unify__new_6_2;
 norm__guard__unify__new_6_2:
  otherwise(norm__guard__unify__new_6_interrupt);
  unify(a3, a2);
  allocp[0] = NILATOM;
  allocp[1] = a0;
  x0 = makecons(&allocp[0]);
  allocp[2] = (q)qp;
  allocp[3] = (q)(&predicate_klic__comp__message_xreport__error_4);
  allocp[4] = makefunctor(&string_const_14);
  allocp[5] = x0;
  allocp[6] = a4;
  allocp[7] = a5;
  qp = (struct goalrec*)&allocp[2];
  allocp += 8;
  proceed();
 norm__guard__unify__new_6_ext_interrupt:
  reasonp = 0l;
 norm__guard__unify__new_6_interrupt:
  toppred = &predicate_klic__comp__normalize_xnorm__guard__unify__new_6;
  goto interrupt_6;
 }

 norm__guard__unify__new_2F6_240_7_top: {
  q x0, x1, x2;
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  a6 = qp->args[6];
  qp = qp->next;
 norm__guard__unify__new_2F6_240_7_clear_reason:
  reasonp = reasons;
 norm__guard__unify__new_2F6_240_7_0:
 norm__guard__unify__new_2F6_240_7_1:
  switch (ptagof(a0)) {
 case ATOMIC:
  if (a0 != makesym(atom_no)) goto norm__guard__unify__new_2F6_240_7_interrupt;
  allocp[0] = makesym(functor_rename_1);
  allocp[1] = a6;
  x0 = makefunctor(&allocp[0]);
  allocp[2] = makesym(functor__3D_2);
  allocp[3] = a2;
  allocp[4] = x0;
  x1 = makefunctor(&allocp[2]);
  allocp[5] = a3;
  allocp[6] = x1;
  x2 = makecons(&allocp[5]);
  allocp += 7;
  unify_value(a1, x2);
  unify(a4, a5);
  proceed();
 case FUNCTOR:
  if (functor_of(a0) != makesym(functor_yes_1)) goto norm__guard__unify__new_2F6_240_7_interrupt;
  allocp[0] = makesym(functor__3D_2);
  allocp[1] = a2;
  x1 = arg(a0, 0);
  allocp[2] = x1;
  x0 = makefunctor(&allocp[0]);
  allocp[3] = a3;
  allocp[4] = x0;
  x2 = makecons(&allocp[3]);
  allocp += 5;
  unify_value(a1, x2);
  unify(a4, a5);
  proceed();
 case VARREF:
  deref_and_jump(a0,norm__guard__unify__new_2F6_240_7_1);
  *reasonp++ =  a0;
  goto norm__guard__unify__new_2F6_240_7_interrupt;
  };
  goto norm__guard__unify__new_2F6_240_7_interrupt;
 norm__guard__unify__new_2F6_240_7_ext_interrupt:
  reasonp = 0l;
 norm__guard__unify__new_2F6_240_7_interrupt:
  goto interrupt_7;
 }

 norm__guard__unify__args_10_top: {
  q x0, x1, x2, x3, x4, x5;
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  a6 = qp->args[6];
  a7 = qp->args[7];
  a8 = qp->args[8];
  a9 = qp->args[9];
  qp = qp->next;
 norm__guard__unify__args_10_clear_reason:
  reasonp = reasons;
 norm__guard__unify__args_10_0:
 norm__guard__unify__args_10_1:
  switch (ptagof(a0)) {
 case CONS:
 norm__guard__unify__args_10_2:
  switch (ptagof(a1)) {
 case CONS:
 norm__guard__unify__args_10_3:
  switch (ptagof(a4)) {
 case ATOMIC:
  if (a4 != makesym(atom_normal)) goto norm__guard__unify__args_10_4;
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__normalize_xnorm__guard__unify__args_10);
  x0 = cdr_of(a0);
  allocp[2] = x0;
  x1 = cdr_of(a1);
  allocp[3] = x1;
  allocp[4] = x2 = makeref(&allocp[4]);
  allocp[5] = a3;
  allocp[6] = x3 = makeref(&allocp[6]);
  allocp[7] = a5;
  allocp[8] = x4 = makeref(&allocp[8]);
  allocp[9] = a7;
  allocp[10] = x5 = makeref(&allocp[10]);
  allocp[11] = a9;
  a0 = car_of(a0);
  a1 = car_of(a1);
  a3 = a2;
  a4 = x2;
  a5 = a6;
  a6 = x4;
  a7 = a8;
  a8 = x5;
  a2 = x3;
  qp = (struct goalrec*)&allocp[0];
  allocp += 12;
  execute(norm__guard__unify_9_0);
  goto norm__guard__unify_9_ext_interrupt;
 case VARREF:
  deref_and_jump(a4,norm__guard__unify__args_10_3);
  *reasonp++ =  a4;
  goto norm__guard__unify__args_10_interrupt;
  };
  goto norm__guard__unify__args_10_4;
 norm__guard__unify__args_10_4:
  x0 = makesym(atom_normal);
  gblt_diff(a4,x0,norm__guard__unify__args_10_interrupt);
  unify3(a9, a8, a7, a6, a3, a2);
  unify_value(a5, a4);
  proceed();
 case VARREF:
  deref_and_jump(a1,norm__guard__unify__args_10_2);
  *reasonp++ =  a1;
  goto norm__guard__unify__args_10_interrupt;
  };
  goto norm__guard__unify__args_10_interrupt;
 case ATOMIC:
  if (a0 != NILATOM) goto norm__guard__unify__args_10_interrupt;
  unify4(a9, a8, a7, a6, a5, a4, a3, a2);
  proceed();
 case VARREF:
  deref_and_jump(a0,norm__guard__unify__args_10_1);
  *reasonp++ =  a0;
  goto norm__guard__unify__args_10_interrupt;
  };
  goto norm__guard__unify__args_10_interrupt;
 norm__guard__unify__args_10_ext_interrupt:
  reasonp = 0l;
 norm__guard__unify__args_10_interrupt:
  goto interrupt_10;
 }

 norm__guard__generic_8_top: {
  q x0, x1, x2, x3, x4, x5;
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  a6 = qp->args[6];
  a7 = qp->args[7];
  qp = qp->next;
 norm__guard__generic_8_clear_reason:
  reasonp = reasons;
 norm__guard__generic_8_0:
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__normalize_xnorm__guard__generic__1_14);
  allocp[2] = a0;
  allocp[3] = a1;
  allocp[4] = a2;
  allocp[5] = a3;
  allocp[6] = x0 = makeref(&allocp[6]);
  allocp[7] = x1 = makeref(&allocp[7]);
  allocp[8] = x2 = makeref(&allocp[8]);
  allocp[9] = x3 = makeref(&allocp[9]);
  allocp[10] = x4 = makeref(&allocp[10]);
  allocp[11] = x5 = makeref(&allocp[11]);
  allocp[12] = a4;
  allocp[13] = a5;
  allocp[14] = a6;
  allocp[15] = a7;
  a1 = x0;
  a2 = x1;
  a3 = x2;
  a4 = x3;
  a5 = x4;
  a6 = x5;
  qp = (struct goalrec*)&allocp[0];
  allocp += 16;
  execute(norm__gg__in__out_7_0);
  goto norm__gg__in__out_7_ext_interrupt;
 norm__guard__generic_8_ext_interrupt:
  reasonp = 0l;
 norm__guard__generic_8_interrupt:
  toppred = &predicate_klic__comp__normalize_xnorm__guard__generic_8;
  goto interrupt_8;
 }

 norm__guard__generic__1_14_top: {
  q x0, x1, x2, x3, x4;
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
  qp = qp->next;
 norm__guard__generic__1_14_clear_reason:
  reasonp = reasons;
 norm__guard__generic__1_14_0:
 norm__guard__generic__1_14_1:
  switch (ptagof(a9)) {
 case ATOMIC:
  if (a9 != makesym(atom_normal)) goto norm__guard__generic__1_14_2;
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__normalize_xnorm__guard__generic__2_14);
  allocp[2] = a6;
  allocp[3] = x0 = makeref(&allocp[3]);
  allocp[4] = a1;
  allocp[5] = a2;
  allocp[6] = a3;
  allocp[7] = a4;
  allocp[8] = x1 = makeref(&allocp[8]);
  allocp[9] = a7;
  allocp[10] = a8;
  allocp[11] = x2 = makeref(&allocp[11]);
  allocp[12] = x3 = makeref(&allocp[12]);
  allocp[13] = a11;
  allocp[14] = x4 = makeref(&allocp[14]);
  allocp[15] = a13;
  a0 = a5;
  a2 = x1;
  a3 = a1;
  a4 = x2;
  a5 = a10;
  a6 = x3;
  a7 = a12;
  a8 = x4;
  a1 = makesym(atom_object);
  qp = (struct goalrec*)&allocp[0];
  allocp += 16;
  execute(norm__gg__in_9_0);
  goto norm__gg__in_9_ext_interrupt;
 case VARREF:
  deref_and_jump(a9,norm__guard__generic__1_14_1);
  *reasonp++ =  a9;
  goto norm__guard__generic__1_14_interrupt;
  };
  goto norm__guard__generic__1_14_2;
 norm__guard__generic__1_14_2:
  x0 = makesym(atom_normal);
  gblt_diff(a9,x0,norm__guard__generic__1_14_interrupt);
  unify2(a13, a12, a11, a10);
  unify_value(a3, a9);
  proceed();
 norm__guard__generic__1_14_ext_interrupt:
  reasonp = 0l;
 norm__guard__generic__1_14_interrupt:
  goto interrupt_14;
 }

 norm__guard__generic__2_14_top: {
  q x0, x1, x2;
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
  qp = qp->next;
 norm__guard__generic__2_14_clear_reason:
  reasonp = reasons;
 norm__guard__generic__2_14_0:
 norm__guard__generic__2_14_1:
  switch (ptagof(a9)) {
 case ATOMIC:
  if (a9 != makesym(atom_normal)) goto norm__guard__generic__2_14_2;
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__normalize_xnorm__guard__generic__3_13);
  allocp[2] = x0 = makeref(&allocp[2]);
  allocp[3] = a1;
  allocp[4] = a2;
  allocp[5] = a3;
  allocp[6] = a4;
  allocp[7] = a5;
  allocp[8] = a6;
  allocp[9] = a7;
  allocp[10] = a8;
  allocp[11] = x1 = makeref(&allocp[11]);
  allocp[12] = a11;
  allocp[13] = x2 = makeref(&allocp[13]);
  allocp[14] = a13;
  a3 = x0;
  a4 = a10;
  a5 = x1;
  a6 = a12;
  a7 = x2;
  qp = (struct goalrec*)&allocp[0];
  allocp += 15;
  execute(norm__gg__ins_8_0);
  goto norm__gg__ins_8_ext_interrupt;
 case VARREF:
  deref_and_jump(a9,norm__guard__generic__2_14_1);
  *reasonp++ =  a9;
  goto norm__guard__generic__2_14_interrupt;
  };
  goto norm__guard__generic__2_14_2;
 norm__guard__generic__2_14_2:
  x0 = makesym(atom_normal);
  gblt_diff(a9,x0,norm__guard__generic__2_14_interrupt);
  unify2(a13, a12, a11, a10);
  unify_value(a4, a9);
  proceed();
 norm__guard__generic__2_14_ext_interrupt:
  reasonp = 0l;
 norm__guard__generic__2_14_interrupt:
  goto interrupt_14;
 }

 norm__guard__generic__3_13_top: {
  q x0, x1, x2, x3, x4, x5;
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
  qp = qp->next;
 norm__guard__generic__3_13_clear_reason:
  reasonp = reasons;
 norm__guard__generic__3_13_0:
 norm__guard__generic__3_13_1:
  switch (ptagof(a0)) {
 case ATOMIC:
  if (a0 != makesym(atom_normal)) goto norm__guard__generic__3_13_2;
  allocp[0] = makesym(functor_gg_3);
  allocp[1] = a5;
  allocp[2] = a6;
  allocp[3] = a1;
  x0 = makefunctor(&allocp[0]);
  allocp[4] = makesym(functor_gg_2);
  allocp[5] = x0;
  allocp[6] = a7;
  x1 = makefunctor(&allocp[4]);
  allocp[7] = x3 = makeref(&allocp[7]);
  allocp[8] = x1;
  x2 = makecons(&allocp[7]);
  allocp += 9;
  unify_value(a9, x2);
  x4 = makesym(atom_normal);
  unify_value(a4, x4);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__normalize_xnorm__gg__outs_9);
  allocp[2] = a8;
  allocp[3] = x5 = makeref(&allocp[3]);
  allocp[4] = x0;
  allocp[5] = a2;
  allocp[6] = a3;
  allocp[7] = x3;
  allocp[8] = a10;
  allocp[9] = a11;
  allocp[10] = a12;
  allocp[11] = (q)(struct goalrec*)&allocp[0];
  allocp[12] = (q)(&predicate_klic__comp__util_xlength_2);
  allocp[13] = a1;
  allocp[14] = x5;
  qp = (struct goalrec*)&allocp[11];
  allocp += 15;
  proceed();
 case VARREF:
  deref_and_jump(a0,norm__guard__generic__3_13_1);
  *reasonp++ =  a0;
  goto norm__guard__generic__3_13_interrupt;
  };
  goto norm__guard__generic__3_13_2;
 norm__guard__generic__3_13_2:
  x0 = makesym(atom_normal);
  gblt_diff(a0,x0,norm__guard__generic__3_13_interrupt);
  unify2(a12, a11, a10, a9);
  unify_value(a4, a0);
  proceed();
 norm__guard__generic__3_13_ext_interrupt:
  reasonp = 0l;
 norm__guard__generic__3_13_interrupt:
  goto interrupt_13;
 }

 norm__gg__in__out_7_top: {
  q x0, x1;
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  a6 = qp->args[6];
  qp = qp->next;
 norm__gg__in__out_7_clear_reason:
  reasonp = reasons;
 norm__gg__in__out_7_0:
 norm__gg__in__out_7_1:
  switch (ptagof(a0)) {
 case FUNCTOR:
  if (functor_of(a0) != makesym(functor_functor_1)) goto norm__gg__in__out_7_3;
  x0 = arg(a0, 0);
 norm__gg__in__out_7_2:
  switch (ptagof(x0)) {
 case FUNCTOR:
  if (functor_of(x0) != makesym(functor__3A_2)) goto norm__gg__in__out_7_3;
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__normalize_xnorm__gg_5);
  x1 = arg(x0, 0);
  allocp[2] = x1;
  allocp[3] = a1;
  allocp[4] = a2;
  allocp[5] = a3;
  allocp[6] = a6;
  a0 = arg(x0, 1);
  a1 = a5;
  a2 = makeint(0L);
  a3 = a4;
  qp = (struct goalrec*)&allocp[0];
  allocp += 7;
  execute(norm__gg__count__out_4_0);
  goto norm__gg__count__out_4_ext_interrupt;
 case VARREF:
  deref_and_jump(x0,norm__gg__in__out_7_2);
  *reasonp++ =  x0;
  goto norm__gg__in__out_7_3;
  };
  goto norm__gg__in__out_7_3;
 case VARREF:
  deref_and_jump(a0,norm__gg__in__out_7_1);
  *reasonp++ =  a0;
  goto norm__gg__in__out_7_3;
  };
  goto norm__gg__in__out_7_3;
 norm__gg__in__out_7_3:
  otherwise(norm__gg__in__out_7_interrupt);
  x0 = makeint(0L);
  unify_value(a4, x0);
  x1 = NILATOM;
  unify_value(a5, x1);
  a4 = a6;
  execute(norm__gg_5_0);
  goto norm__gg_5_ext_interrupt;
 norm__gg__in__out_7_ext_interrupt:
  reasonp = 0l;
 norm__gg__in__out_7_interrupt:
  toppred = &predicate_klic__comp__normalize_xnorm__gg__in__out_7;
  goto interrupt_7;
 }

 norm__gg__count__out_4_top: {
  q x0, x1, x2, x3, x4, x5;
  a2 = qp->args[2];
  a3 = qp->args[3];
  qp = qp->next;
 norm__gg__count__out_4_clear_reason:
  reasonp = reasons;
 norm__gg__count__out_4_0:
 norm__gg__count__out_4_1:
  switch (ptagof(a0)) {
 case FUNCTOR:
  if (functor_of(a0) != makesym(functor_functor_1)) goto norm__gg__count__out_4_3;
  x0 = arg(a0, 0);
 norm__gg__count__out_4_2:
  switch (ptagof(x0)) {
 case FUNCTOR:
  if (functor_of(x0) != makesym(functor__3A_2)) goto norm__gg__count__out_4_3;
  allocp[0] = x2 = makeref(&allocp[0]);
  x3 = arg(x0, 0);
  allocp[1] = x3;
  x1 = makecons(&allocp[0]);
  allocp += 2;
  unify_value(a1, x1);
  x4 = makeint(1L);
  bblt_add_no_check_y(a2,x4,x5);
  a0 = arg(x0, 1);
  a1 = x2;
  a2 = x5;
  execute(norm__gg__count__out_4_0);
  goto norm__gg__count__out_4_ext_interrupt;
 case VARREF:
  deref_and_jump(x0,norm__gg__count__out_4_2);
  *reasonp++ =  x0;
  goto norm__gg__count__out_4_3;
  };
  goto norm__gg__count__out_4_3;
 case VARREF:
  deref_and_jump(a0,norm__gg__count__out_4_1);
  *reasonp++ =  a0;
  goto norm__gg__count__out_4_3;
  };
  goto norm__gg__count__out_4_3;
 norm__gg__count__out_4_3:
  otherwise(norm__gg__count__out_4_interrupt);
  allocp[0] = NILATOM;
  allocp[1] = a0;
  x0 = makecons(&allocp[0]);
  allocp += 2;
  unify_value(a1, x0);
  x1 = makeint(1L);
  bblt_add_no_check_y(a2,x1,x2);
  unify(a3, x2);
  proceed();
 norm__gg__count__out_4_ext_interrupt:
  reasonp = 0l;
 norm__gg__count__out_4_interrupt:
  toppred = &predicate_klic__comp__normalize_xnorm__gg__count__out_4;
  goto interrupt_4;
 }

 norm__gg_5_top: {
  q x0, x1, x2, x3, x4, x5;
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  qp = qp->next;
 norm__gg_5_clear_reason:
  reasonp = reasons;
 norm__gg_5_0:
 norm__gg_5_1:
  switch (ptagof(a0)) {
 case FUNCTOR:
  if (functor_of(a0) != makesym(functor_functor_1)) goto norm__gg_5_4;
  x0 = arg(a0, 0);
 norm__gg_5_2:
  if (isref(x0)) goto norm__gg_5_3;
  gblt_pfunctor(x0,x1,norm__gg_5_4);
  gblt_arity(x0,x2,norm__gg_5_4);
  allocp[0] = makesym(functor__2F_2);
  allocp[1] = x4 = makeref(&allocp[1]);
  allocp[2] = x5 = makeref(&allocp[2]);
  x3 = makefunctor(&allocp[0]);
  allocp += 3;
  unify_value(a1, x3);
  a0 = x4;
  a1 = x5;
  a5 = x1;
  a6 = x2;
  a7 = a4;
  a4 = x0;
  execute(norm__gg__1_8_0);
  goto norm__gg__1_8_ext_interrupt;
 norm__gg_5_3:
  deref_and_jump(x0,norm__gg_5_2);
  *reasonp++ =  x0;
  goto norm__gg_5_4;
 case VARREF:
  deref_and_jump(a0,norm__gg_5_1);
  *reasonp++ =  a0;
  goto norm__gg_5_4;
  };
  goto norm__gg_5_4;
 norm__gg_5_4:
  otherwise(norm__gg_5_interrupt);
  x0 = makesym(atom_abnormal);
  unify_value(a4, x0);
  proceed();
 norm__gg_5_ext_interrupt:
  reasonp = 0l;
 norm__gg_5_interrupt:
  toppred = &predicate_klic__comp__normalize_xnorm__gg_5;
  goto interrupt_5;
 }

 norm__gg__1_8_top: {
  q x0, x1, x2, x3;
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  a6 = qp->args[6];
  a7 = qp->args[7];
  qp = qp->next;
 norm__gg__1_8_clear_reason:
  reasonp = reasons;
 norm__gg__1_8_0:
 norm__gg__1_8_1:
  switch (ptagof(a5)) {
 case ATOMIC:
  if (a5 != makesym(atom_generic)) goto norm__gg__1_8_4;
 norm__gg__1_8_2:
  switch (ptagof(a6)) {
 case ATOMIC:
 norm__gg__1_8_3:
  if (a6 != makeint(2L)) goto norm__gg__1_8_4;
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__normalize_xnorm__gg__2_6);
  allocp[2] = a0;
  allocp[3] = a1;
  allocp[4] = a2;
  allocp[5] = a3;
  allocp[6] = x0 = makeref(&allocp[6]);
  allocp[7] = a7;
  allocp[8] = (q)(struct goalrec*)&allocp[0];
  allocp[9] = (q)(&predicate_functor__table_xarg_3);
  allocp[10] = makeint(2L);
  allocp[11] = a4;
  allocp[12] = x0;
  allocp[13] = (q)(struct goalrec*)&allocp[8];
  allocp[14] = (q)(&predicate_functor__table_xarg_3);
  allocp[15] = makeint(1L);
  allocp[16] = a4;
  allocp[17] = a2;
  qp = (struct goalrec*)&allocp[13];
  allocp += 18;
  proceed();
 case VARREF:
  deref_and_jump(a6,norm__gg__1_8_2);
  *reasonp++ =  a6;
  goto norm__gg__1_8_4;
  };
  goto norm__gg__1_8_4;
 case VARREF:
  deref_and_jump(a5,norm__gg__1_8_1);
  *reasonp++ =  a5;
  goto norm__gg__1_8_4;
  };
  goto norm__gg__1_8_4;
 norm__gg__1_8_4:
  otherwise(norm__gg__1_8_interrupt);
  unify2(a0, a5, a1, a6);
  x0 = makesym(atom_normal);
  unify_value(a7, x0);
  allocp[0] = a3;
  allocp[1] = a2;
  x1 = makecons(&allocp[0]);
  allocp[2] = x1;
  allocp[3] = x3 = makeref(&allocp[3]);
  x2 = makecons(&allocp[2]);
  allocp[4] = (q)qp;
  allocp[5] = (q)(&predicate_klic__comp__util_xuniv_2);
  allocp[6] = a4;
  allocp[7] = x2;
  qp = (struct goalrec*)&allocp[4];
  allocp += 8;
  proceed();
 norm__gg__1_8_ext_interrupt:
  reasonp = 0l;
 norm__gg__1_8_interrupt:
  toppred = &predicate_klic__comp__normalize_xnorm__gg__1_8;
  goto interrupt_8;
 }

 norm__gg__2_6_top: {
  q x0, x1, x2, x3, x4, x5;
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  qp = qp->next;
 norm__gg__2_6_clear_reason:
  reasonp = reasons;
 norm__gg__2_6_0:
 norm__gg__2_6_1:
  switch (ptagof(a4)) {
 case FUNCTOR:
  if (functor_of(a4) != makesym(functor_functor_1)) goto norm__gg__2_6_4;
  x0 = arg(a4, 0);
 norm__gg__2_6_2:
  if (isref(x0)) goto norm__gg__2_6_3;
  gblt_pfunctor(x0,x1,norm__gg__2_6_4);
  gblt_arity(x0,x2,norm__gg__2_6_4);
  unify_value(a0, x1);
  unify_value(a1, x2);
  x3 = makesym(atom_normal);
  unify_value(a5, x3);
  allocp[0] = a3;
  allocp[1] = x5 = makeref(&allocp[1]);
  x4 = makecons(&allocp[0]);
  allocp[2] = (q)qp;
  allocp[3] = (q)(&predicate_klic__comp__util_xuniv_2);
  allocp[4] = x0;
  allocp[5] = x4;
  qp = (struct goalrec*)&allocp[2];
  allocp += 6;
  proceed();
 norm__gg__2_6_3:
  deref_and_jump(x0,norm__gg__2_6_2);
  *reasonp++ =  x0;
  goto norm__gg__2_6_4;
 case VARREF:
  deref_and_jump(a4,norm__gg__2_6_1);
  *reasonp++ =  a4;
  goto norm__gg__2_6_4;
  };
  goto norm__gg__2_6_4;
 norm__gg__2_6_4:
  otherwise(norm__gg__2_6_interrupt);
  x0 = makesym(atom_abnormal);
  unify_value(a5, x0);
  proceed();
 norm__gg__2_6_ext_interrupt:
  reasonp = 0l;
 norm__gg__2_6_interrupt:
  goto interrupt_6;
 }

 norm__gg__ins_8_top: {
  q x0, x1, x2, x3, x4, x5, x6;
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  a6 = qp->args[6];
  a7 = qp->args[7];
  qp = qp->next;
 norm__gg__ins_8_clear_reason:
  reasonp = reasons;
 norm__gg__ins_8_0:
 norm__gg__ins_8_1:
  switch (ptagof(a0)) {
 case CONS:
  allocp[0] = x1 = makeref(&allocp[0]);
  allocp[1] = x2 = makeref(&allocp[1]);
  x0 = makecons(&allocp[0]);
  allocp += 2;
  unify_value(a1, x0);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__normalize_xnorm__gg__ins__1_9);
  x3 = cdr_of(a0);
  allocp[2] = x3;
  allocp[3] = x1;
  allocp[4] = a2;
  allocp[5] = a3;
  allocp[6] = x4 = makeref(&allocp[6]);
  allocp[7] = x5 = makeref(&allocp[7]);
  allocp[8] = a5;
  allocp[9] = x6 = makeref(&allocp[9]);
  allocp[10] = a7;
  a0 = car_of(a0);
  a1 = makesym(atom_bound);
  a3 = a2;
  a5 = a4;
  a7 = a6;
  a8 = x6;
  a6 = x5;
  a4 = x4;
  a2 = x2;
  qp = (struct goalrec*)&allocp[0];
  allocp += 11;
  execute(norm__gg__in_9_0);
  goto norm__gg__in_9_ext_interrupt;
 case ATOMIC:
  if (a0 != NILATOM) goto norm__gg__ins_8_interrupt;
  unify2(a7, a6, a5, a4);
  x0 = NILATOM;
  unify_value(a1, x0);
  x1 = makesym(atom_normal);
  unify_value(a3, x1);
  proceed();
 case VARREF:
  deref_and_jump(a0,norm__gg__ins_8_1);
  *reasonp++ =  a0;
  goto norm__gg__ins_8_interrupt;
  };
  goto norm__gg__ins_8_interrupt;
 norm__gg__ins_8_ext_interrupt:
  reasonp = 0l;
 norm__gg__ins_8_interrupt:
  toppred = &predicate_klic__comp__normalize_xnorm__gg__ins_8;
  goto interrupt_8;
 }

 norm__gg__ins__1_9_top: {
  q x0;
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  a6 = qp->args[6];
  a7 = qp->args[7];
  a8 = qp->args[8];
  qp = qp->next;
 norm__gg__ins__1_9_clear_reason:
  reasonp = reasons;
 norm__gg__ins__1_9_0:
 norm__gg__ins__1_9_1:
  switch (ptagof(a4)) {
 case ATOMIC:
  if (a4 != makesym(atom_normal)) goto norm__gg__ins__1_9_2;
  a4 = a5;
  a5 = a6;
  a6 = a7;
  a7 = a8;
  execute(norm__gg__ins_8_0);
  goto norm__gg__ins_8_ext_interrupt;
 case VARREF:
  deref_and_jump(a4,norm__gg__ins__1_9_1);
  *reasonp++ =  a4;
  goto norm__gg__ins__1_9_interrupt;
  };
  goto norm__gg__ins__1_9_2;
 norm__gg__ins__1_9_2:
  x0 = makesym(atom_normal);
  gblt_diff(a4,x0,norm__gg__ins__1_9_interrupt);
  unify2(a8, a7, a6, a5);
  unify_value(a3, a4);
  proceed();
 norm__gg__ins__1_9_ext_interrupt:
  reasonp = 0l;
 norm__gg__ins__1_9_interrupt:
  goto interrupt_9;
 }

 norm__gg__in_9_top: {
  q x0, x1;
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  a6 = qp->args[6];
  a7 = qp->args[7];
  a8 = qp->args[8];
  qp = qp->next;
 norm__gg__in_9_clear_reason:
  reasonp = reasons;
 norm__gg__in_9_0:
 norm__gg__in_9_1:
  switch (ptagof(a0)) {
 case FUNCTOR:
  if (functor_of(a0) != makesym(functor_variable_1)) goto norm__gg__in_9_2;
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__normalize_xnorm__gg__in__1_10);
  x0 = arg(a0, 0);
  allocp[2] = x0;
  allocp[3] = a1;
  allocp[4] = a2;
  allocp[5] = a3;
  allocp[6] = a4;
  allocp[7] = x1 = makeref(&allocp[7]);
  allocp[8] = a5;
  allocp[9] = a6;
  allocp[10] = a7;
  allocp[11] = a8;
  allocp[12] = (q)(struct goalrec*)&allocp[0];
  allocp[13] = (q)(&predicate_klic__comp__util_xassoc_3);
  allocp[14] = a3;
  allocp[15] = x0;
  allocp[16] = x1;
  qp = (struct goalrec*)&allocp[12];
  allocp += 17;
  proceed();
 case VARREF:
  deref_and_jump(a0,norm__gg__in_9_1);
  *reasonp++ =  a0;
  goto norm__gg__in_9_2;
  };
  goto norm__gg__in_9_2;
 norm__gg__in_9_2:
  otherwise(norm__gg__in_9_interrupt);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__normalize_xnorm__gg__in__2_8);
  allocp[2] = a0;
  allocp[3] = a1;
  allocp[4] = a4;
  allocp[5] = x0 = makeref(&allocp[5]);
  allocp[6] = a5;
  allocp[7] = a6;
  allocp[8] = a7;
  allocp[9] = a8;
  a1 = a2;
  allocp[10] = x1 = makeref(&allocp[10]);
  a2 = x1;
  a3 = makesym(atom_yes);
  a4 = x0;
  qp = (struct goalrec*)&allocp[0];
  allocp += 11;
  execute(is__const_5_0);
  goto is__const_5_ext_interrupt;
 norm__gg__in_9_ext_interrupt:
  reasonp = 0l;
 norm__gg__in_9_interrupt:
  toppred = &predicate_klic__comp__normalize_xnorm__gg__in_9;
  goto interrupt_9;
 }

 norm__gg__in__1_10_top: {
  q x0, x1, x2, x3;
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  a6 = qp->args[6];
  a7 = qp->args[7];
  a8 = qp->args[8];
  a9 = qp->args[9];
  qp = qp->next;
 norm__gg__in__1_10_clear_reason:
  reasonp = reasons;
 norm__gg__in__1_10_0:
 norm__gg__in__1_10_1:
  switch (ptagof(a5)) {
 case ATOMIC:
  if (a5 != makesym(atom_no)) goto norm__gg__in__1_10_3;
  unify(a7, a6);
  x0 = makesym(atom_abnormal);
  unify_value(a4, x0);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__message_xwarning_4);
  allocp[2] = makefunctor(&string_const_7);
  allocp[3] = NILATOM;
  allocp[4] = a8;
  allocp[5] = a9;
  qp = (struct goalrec*)&allocp[0];
  allocp += 6;
  proceed();
 case FUNCTOR:
  if (functor_of(a5) != makesym(functor_yes_1)) goto norm__gg__in__1_10_3;
  x0 = arg(a5, 0);
 norm__gg__in__1_10_2:
  switch (ptagof(x0)) {
 case FUNCTOR:
  if (functor_of(x0) != makesym(functor_rename_1)) goto norm__gg__in__1_10_3;
  allocp[0] = makesym(functor_variable_1);
  x2 = arg(x0, 0);
  allocp[1] = x2;
  x1 = makefunctor(&allocp[0]);
  a0 = x1;
  a5 = a6;
  a6 = a7;
  a7 = a8;
  a8 = a9;
  allocp += 2;
  execute(norm__gg__in_9_0);
  goto norm__gg__in_9_ext_interrupt;
 case VARREF:
  deref_and_jump(x0,norm__gg__in__1_10_2);
  *reasonp++ =  x0;
  goto norm__gg__in__1_10_3;
  };
  goto norm__gg__in__1_10_3;
 case VARREF:
  deref_and_jump(a5,norm__gg__in__1_10_1);
  *reasonp++ =  a5;
  goto norm__gg__in__1_10_3;
  };
  goto norm__gg__in__1_10_3;
 norm__gg__in__1_10_3:
  otherwise(norm__gg__in__1_10_interrupt);
 norm__gg__in__1_10_4:
  switch (ptagof(a5)) {
 case FUNCTOR:
  if (functor_of(a5) != makesym(functor_yes_1)) goto norm__gg__in__1_10_interrupt;
  unify(a9, a8);
  x0 = arg(a5, 0);
  unify(a2, x0);
  allocp[0] = makesym(functor__3A_2);
  allocp[1] = x0;
  allocp[2] = a1;
  x1 = makefunctor(&allocp[0]);
  allocp[3] = a7;
  allocp[4] = x1;
  x2 = makecons(&allocp[3]);
  allocp += 5;
  unify_value(a6, x2);
  x3 = makesym(atom_normal);
  unify_value(a4, x3);
  proceed();
 case VARREF:
  deref_and_jump(a5,norm__gg__in__1_10_4);
  *reasonp++ =  a5;
  goto norm__gg__in__1_10_interrupt;
  };
  goto norm__gg__in__1_10_interrupt;
 norm__gg__in__1_10_ext_interrupt:
  reasonp = 0l;
 norm__gg__in__1_10_interrupt:
  goto interrupt_10;
 }

 norm__gg__in__2_8_top: {
  q x0, x1;
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  a6 = qp->args[6];
  a7 = qp->args[7];
  qp = qp->next;
 norm__gg__in__2_8_clear_reason:
  reasonp = reasons;
 norm__gg__in__2_8_0:
 norm__gg__in__2_8_1:
  switch (ptagof(a1)) {
 case ATOMIC:
  if (a1 != makesym(atom_object)) goto norm__gg__in__2_8_4;
 norm__gg__in__2_8_2:
  switch (ptagof(a3)) {
 case ATOMIC:
  if (a3 != makesym(atom_yes)) goto norm__gg__in__2_8_4;
  unify(a5, a4);
  x0 = makesym(atom_normal);
  unify_value(a2, x0);
  allocp[0] = NILATOM;
  allocp[1] = a0;
  x1 = makecons(&allocp[0]);
  allocp[2] = (q)qp;
  allocp[3] = (q)(&predicate_klic__comp__message_xwarning_4);
  allocp[4] = makefunctor(&string_const_13);
  allocp[5] = x1;
  allocp[6] = a6;
  allocp[7] = a7;
  qp = (struct goalrec*)&allocp[2];
  allocp += 8;
  proceed();
 case VARREF:
  deref_and_jump(a3,norm__gg__in__2_8_2);
  *reasonp++ =  a3;
  goto norm__gg__in__2_8_4;
  };
  goto norm__gg__in__2_8_4;
 case FUNCTOR:
  if (functor_of(a1) != makesym(functor_object_1)) goto norm__gg__in__2_8_4;
 norm__gg__in__2_8_3:
  switch (ptagof(a3)) {
 case ATOMIC:
  if (a3 != makesym(atom_yes)) goto norm__gg__in__2_8_4;
  unify(a5, a4);
  x0 = makesym(atom_normal);
  unify_value(a2, x0);
  allocp[0] = NILATOM;
  allocp[1] = a0;
  x1 = makecons(&allocp[0]);
  allocp[2] = (q)qp;
  allocp[3] = (q)(&predicate_klic__comp__message_xwarning_4);
  allocp[4] = makefunctor(&string_const_13);
  allocp[5] = x1;
  allocp[6] = a6;
  allocp[7] = a7;
  qp = (struct goalrec*)&allocp[2];
  allocp += 8;
  proceed();
 case VARREF:
  deref_and_jump(a3,norm__gg__in__2_8_3);
  *reasonp++ =  a3;
  goto norm__gg__in__2_8_4;
  };
  goto norm__gg__in__2_8_4;
 case VARREF:
  deref_and_jump(a1,norm__gg__in__2_8_1);
  *reasonp++ =  a1;
  goto norm__gg__in__2_8_4;
  };
  goto norm__gg__in__2_8_4;
 norm__gg__in__2_8_4:
 norm__gg__in__2_8_5:
  switch (ptagof(a3)) {
 case ATOMIC:
  if (a3 != makesym(atom_no)) goto norm__gg__in__2_8_6;
  unify(a5, a4);
  x0 = makesym(atom_abnormal);
  unify_value(a2, x0);
  allocp[0] = NILATOM;
  allocp[1] = a0;
  x1 = makecons(&allocp[0]);
  allocp[2] = (q)qp;
  allocp[3] = (q)(&predicate_klic__comp__message_xwarning_4);
  allocp[4] = makefunctor(&string_const_11);
  allocp[5] = x1;
  allocp[6] = a6;
  allocp[7] = a7;
  qp = (struct goalrec*)&allocp[2];
  allocp += 8;
  proceed();
 case VARREF:
  deref_and_jump(a3,norm__gg__in__2_8_5);
  *reasonp++ =  a3;
  goto norm__gg__in__2_8_6;
  };
  goto norm__gg__in__2_8_6;
 norm__gg__in__2_8_6:
  otherwise(norm__gg__in__2_8_interrupt);
 norm__gg__in__2_8_7:
  switch (ptagof(a3)) {
 case ATOMIC:
  if (a3 != makesym(atom_yes)) goto norm__gg__in__2_8_interrupt;
  unify2(a7, a6, a5, a4);
  x0 = makesym(atom_normal);
  unify_value(a2, x0);
  proceed();
 case VARREF:
  deref_and_jump(a3,norm__gg__in__2_8_7);
  *reasonp++ =  a3;
  goto norm__gg__in__2_8_interrupt;
  };
  goto norm__gg__in__2_8_interrupt;
 norm__gg__in__2_8_ext_interrupt:
  reasonp = 0l;
 norm__gg__in__2_8_interrupt:
  goto interrupt_8;
 }

 norm__gg__outs_9_top: {
  q x0, x1, x2, x3, x4;
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  a6 = qp->args[6];
  a7 = qp->args[7];
  a8 = qp->args[8];
  qp = qp->next;
 norm__gg__outs_9_clear_reason:
  reasonp = reasons;
 norm__gg__outs_9_0:
 norm__gg__outs_9_1:
  switch (ptagof(a0)) {
 case CONS:
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__normalize_xnorm__gg__outs_9);
  x0 = cdr_of(a0);
  allocp[2] = x0;
  allocp[3] = x1 = makeref(&allocp[3]);
  allocp[4] = a2;
  allocp[5] = x2 = makeref(&allocp[5]);
  allocp[6] = a4;
  allocp[7] = x3 = makeref(&allocp[7]);
  allocp[8] = a6;
  allocp[9] = a7;
  allocp[10] = a8;
  allocp[11] = (q)(struct goalrec*)&allocp[0];
  allocp[12] = (q)(&predicate_integer__arithmetics_xadd_3);
  allocp[13] = a1;
  allocp[14] = makeint(1L);
  allocp[15] = x1;
  allocp[16] = makesym(functor_gg_2);
  allocp[17] = a2;
  allocp[18] = a1;
  x4 = makefunctor(&allocp[16]);
  a0 = car_of(a0);
  a1 = x4;
  a2 = a3;
  a3 = x2;
  a4 = a5;
  a5 = x3;
  qp = (struct goalrec*)&allocp[11];
  allocp += 19;
  execute(norm__guard__term_6_0);
  goto norm__guard__term_6_ext_interrupt;
 case ATOMIC:
  if (a0 != NILATOM) goto norm__gg__outs_9_interrupt;
  unify3(a4, a3, a6, a5, a8, a7);
  proceed();
 case VARREF:
  deref_and_jump(a0,norm__gg__outs_9_1);
  *reasonp++ =  a0;
  goto norm__gg__outs_9_interrupt;
  };
  goto norm__gg__outs_9_interrupt;
 norm__gg__outs_9_ext_interrupt:
  reasonp = 0l;
 norm__gg__outs_9_interrupt:
  goto interrupt_9;
 }

 norm__guard__builts_8_top: {
  q x0, x1, x2, x3, x4, x5, x6, x7;
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  a6 = qp->args[6];
  a7 = qp->args[7];
  qp = qp->next;
 norm__guard__builts_8_clear_reason:
  reasonp = reasons;
 norm__guard__builts_8_0:
 norm__guard__builts_8_1:
  switch (ptagof(a0)) {
 case CONS:
  x0 = car_of(a0);
 norm__guard__builts_8_2:
  switch (ptagof(x0)) {
 case FUNCTOR:
  if (functor_of(x0) != makesym(functor__24_3)) goto norm__guard__builts_8_interrupt;
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__normalize_xnorm__guard__builts__1_12);
  x1 = cdr_of(a0);
  allocp[2] = x1;
  allocp[3] = a1;
  allocp[4] = a2;
  allocp[5] = a3;
  x2 = arg(x0, 1);
  allocp[6] = x2;
  x3 = arg(x0, 2);
  allocp[7] = x3;
  allocp[8] = x4 = makeref(&allocp[8]);
  allocp[9] = x5 = makeref(&allocp[9]);
  allocp[10] = x6 = makeref(&allocp[10]);
  allocp[11] = a5;
  allocp[12] = x7 = makeref(&allocp[12]);
  allocp[13] = a7;
  a0 = arg(x0, 0);
  a2 = a1;
  a3 = x5;
  a5 = x6;
  a7 = x7;
  a1 = x4;
  qp = (struct goalrec*)&allocp[0];
  allocp += 14;
  execute(norm__gblt__args_8_0);
  goto norm__gblt__args_8_ext_interrupt;
 case VARREF:
  deref_and_jump(x0,norm__guard__builts_8_2);
  *reasonp++ =  x0;
  goto norm__guard__builts_8_interrupt;
  };
  goto norm__guard__builts_8_interrupt;
 case ATOMIC:
  if (a0 != NILATOM) goto norm__guard__builts_8_interrupt;
  unify3(a7, a6, a5, a4, a2, a1);
  x0 = makesym(atom_normal);
  unify_value(a3, x0);
  proceed();
 case VARREF:
  deref_and_jump(a0,norm__guard__builts_8_1);
  *reasonp++ =  a0;
  goto norm__guard__builts_8_interrupt;
  };
  goto norm__guard__builts_8_interrupt;
 norm__guard__builts_8_ext_interrupt:
  reasonp = 0l;
 norm__guard__builts_8_interrupt:
  toppred = &predicate_klic__comp__normalize_xnorm__guard__builts_8;
  goto interrupt_8;
 }

 norm__guard__builts__1_12_top: {
  q x0, x1, x2, x3, x4, x5, x6, x7, x8, x9;
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
 norm__guard__builts__1_12_clear_reason:
  reasonp = reasons;
 norm__guard__builts__1_12_0:
 norm__guard__builts__1_12_1:
  switch (ptagof(a7)) {
 case ATOMIC:
  if (a7 != makesym(atom_normal)) goto norm__guard__builts__1_12_3;
 norm__guard__builts__1_12_2:
  switch (ptagof(a4)) {
 case ATOMIC:
  if (a4 != makesym(atom_none)) goto norm__guard__builts__1_12_3;
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__normalize_xnorm__guard__builts_8);
  allocp[2] = a0;
  allocp[3] = a1;
  allocp[4] = a2;
  allocp[5] = a3;
  allocp[6] = x0 = makeref(&allocp[6]);
  allocp[7] = a9;
  allocp[8] = a10;
  allocp[9] = a11;
  allocp[10] = makesym(functor_gp_1);
  allocp[11] = x2 = makeref(&allocp[11]);
  x1 = makefunctor(&allocp[10]);
  allocp[12] = x0;
  allocp[13] = x1;
  x3 = makecons(&allocp[12]);
  qp = (struct goalrec*)&allocp[0];
  allocp += 14;
  unify_value(a8, x3);
  allocp[0] = a6;
  allocp[1] = a5;
  x4 = makecons(&allocp[0]);
  allocp[2] = (q)qp;
  allocp[3] = (q)(&predicate_functor__table_x_3D_2E_2E_2);
  allocp[4] = x2;
  allocp[5] = x4;
  qp = (struct goalrec*)&allocp[2];
  allocp += 6;
  proceed();
 case FUNCTOR:
  if (functor_of(a4) != makesym(functor__3A_2)) goto norm__guard__builts__1_12_3;
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__normalize_xnorm__guard__builts_8);
  allocp[2] = a0;
  allocp[3] = x0 = makeref(&allocp[3]);
  allocp[4] = a2;
  allocp[5] = a3;
  allocp[6] = x1 = makeref(&allocp[6]);
  allocp[7] = a9;
  allocp[8] = a10;
  allocp[9] = a11;
  allocp[10] = makesym(functor_gb_1);
  allocp[11] = x3 = makeref(&allocp[11]);
  x2 = makefunctor(&allocp[10]);
  allocp[12] = (q)(struct goalrec*)&allocp[0];
  allocp[13] = (q)(&predicate_klic__comp__normalize_xnorm__guard__term_6);
  x4 = arg(a4, 0);
  allocp[14] = x4;
  allocp[15] = x2;
  allocp[16] = a1;
  allocp[17] = x0;
  allocp[18] = x5 = makeref(&allocp[18]);
  allocp[19] = x1;
  allocp[20] = makesym(functor_gb_2);
  allocp[21] = x3;
  x7 = arg(a4, 1);
  allocp[22] = x7;
  x6 = makefunctor(&allocp[20]);
  allocp[23] = x5;
  allocp[24] = x6;
  x8 = makecons(&allocp[23]);
  qp = (struct goalrec*)&allocp[12];
  allocp += 25;
  unify_value(a8, x8);
  allocp[0] = a6;
  allocp[1] = a5;
  x9 = makecons(&allocp[0]);
  allocp[2] = (q)qp;
  allocp[3] = (q)(&predicate_functor__table_x_3D_2E_2E_2);
  allocp[4] = x3;
  allocp[5] = x9;
  qp = (struct goalrec*)&allocp[2];
  allocp += 6;
  proceed();
 case VARREF:
  deref_and_jump(a4,norm__guard__builts__1_12_2);
  *reasonp++ =  a4;
  goto norm__guard__builts__1_12_3;
  };
  goto norm__guard__builts__1_12_3;
 case VARREF:
  deref_and_jump(a7,norm__guard__builts__1_12_1);
  *reasonp++ =  a7;
  goto norm__guard__builts__1_12_interrupt;
  };
  goto norm__guard__builts__1_12_3;
 norm__guard__builts__1_12_3:
  x0 = makesym(atom_normal);
  gblt_diff(a7,x0,norm__guard__builts__1_12_interrupt);
  unify2(a11, a10, a9, a8);
  unify_value(a3, a7);
  proceed();
 norm__guard__builts__1_12_ext_interrupt:
  reasonp = 0l;
 norm__guard__builts__1_12_interrupt:
  goto interrupt_12;
 }

 norm__gblt__args_8_top: {
  q x0, x1, x2, x3, x4, x5, x6, x7;
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  a6 = qp->args[6];
  a7 = qp->args[7];
  qp = qp->next;
 norm__gblt__args_8_clear_reason:
  reasonp = reasons;
 norm__gblt__args_8_0:
 norm__gblt__args_8_1:
  switch (ptagof(a0)) {
 case CONS:
  x0 = car_of(a0);
 norm__gblt__args_8_2:
  switch (ptagof(x0)) {
 case FUNCTOR:
  if (functor_of(x0) != makesym(functor__3A_2)) goto norm__gblt__args_8_interrupt;
  allocp[0] = x2 = makeref(&allocp[0]);
  allocp[1] = x3 = makeref(&allocp[1]);
  x1 = makecons(&allocp[0]);
  allocp += 2;
  unify_value(a1, x1);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__normalize_xnorm__gblt__args__1_9);
  x4 = cdr_of(a0);
  allocp[2] = x4;
  allocp[3] = x2;
  allocp[4] = a2;
  allocp[5] = a3;
  allocp[6] = x5 = makeref(&allocp[6]);
  allocp[7] = x6 = makeref(&allocp[7]);
  allocp[8] = a5;
  allocp[9] = x7 = makeref(&allocp[9]);
  allocp[10] = a7;
  a0 = arg(x0, 0);
  a1 = arg(x0, 1);
  a3 = a2;
  a5 = a4;
  a7 = a6;
  a8 = x7;
  a6 = x6;
  a4 = x5;
  a2 = x3;
  qp = (struct goalrec*)&allocp[0];
  allocp += 11;
  execute(norm__gblt__arg_9_0);
  goto norm__gblt__arg_9_ext_interrupt;
 case VARREF:
  deref_and_jump(x0,norm__gblt__args_8_2);
  *reasonp++ =  x0;
  goto norm__gblt__args_8_interrupt;
  };
  goto norm__gblt__args_8_interrupt;
 case ATOMIC:
  if (a0 != NILATOM) goto norm__gblt__args_8_interrupt;
  unify2(a7, a6, a5, a4);
  x0 = NILATOM;
  unify_value(a1, x0);
  x1 = makesym(atom_normal);
  unify_value(a3, x1);
  proceed();
 case VARREF:
  deref_and_jump(a0,norm__gblt__args_8_1);
  *reasonp++ =  a0;
  goto norm__gblt__args_8_interrupt;
  };
  goto norm__gblt__args_8_interrupt;
 norm__gblt__args_8_ext_interrupt:
  reasonp = 0l;
 norm__gblt__args_8_interrupt:
  toppred = &predicate_klic__comp__normalize_xnorm__gblt__args_8;
  goto interrupt_8;
 }

 norm__gblt__args__1_9_top: {
  q x0;
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  a6 = qp->args[6];
  a7 = qp->args[7];
  a8 = qp->args[8];
  qp = qp->next;
 norm__gblt__args__1_9_clear_reason:
  reasonp = reasons;
 norm__gblt__args__1_9_0:
 norm__gblt__args__1_9_1:
  switch (ptagof(a4)) {
 case ATOMIC:
  if (a4 != makesym(atom_normal)) goto norm__gblt__args__1_9_2;
  a4 = a5;
  a5 = a6;
  a6 = a7;
  a7 = a8;
  execute(norm__gblt__args_8_0);
  goto norm__gblt__args_8_ext_interrupt;
 case VARREF:
  deref_and_jump(a4,norm__gblt__args__1_9_1);
  *reasonp++ =  a4;
  goto norm__gblt__args__1_9_interrupt;
  };
  goto norm__gblt__args__1_9_2;
 norm__gblt__args__1_9_2:
  x0 = makesym(atom_normal);
  gblt_diff(a4,x0,norm__gblt__args__1_9_interrupt);
  unify2(a8, a7, a6, a5);
  unify_value(a3, a4);
  proceed();
 norm__gblt__args__1_9_ext_interrupt:
  reasonp = 0l;
 norm__gblt__args__1_9_interrupt:
  goto interrupt_9;
 }

 norm__gblt__arg_9_top: {
  q x0, x1;
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  a6 = qp->args[6];
  a7 = qp->args[7];
  a8 = qp->args[8];
  qp = qp->next;
 norm__gblt__arg_9_clear_reason:
  reasonp = reasons;
 norm__gblt__arg_9_0:
 norm__gblt__arg_9_1:
  switch (ptagof(a0)) {
 case FUNCTOR:
  if (functor_of(a0) != makesym(functor_variable_1)) goto norm__gblt__arg_9_2;
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__normalize_xnorm__gblt__arg__1_10);
  x0 = arg(a0, 0);
  allocp[2] = x0;
  allocp[3] = a1;
  allocp[4] = a2;
  allocp[5] = a3;
  allocp[6] = a4;
  allocp[7] = x1 = makeref(&allocp[7]);
  allocp[8] = a5;
  allocp[9] = a6;
  allocp[10] = a7;
  allocp[11] = a8;
  allocp[12] = (q)(struct goalrec*)&allocp[0];
  allocp[13] = (q)(&predicate_klic__comp__util_xassoc_3);
  allocp[14] = a3;
  allocp[15] = x0;
  allocp[16] = x1;
  qp = (struct goalrec*)&allocp[12];
  allocp += 17;
  proceed();
 case VARREF:
  deref_and_jump(a0,norm__gblt__arg_9_1);
  *reasonp++ =  a0;
  goto norm__gblt__arg_9_2;
  };
  goto norm__gblt__arg_9_2;
 norm__gblt__arg_9_2:
  otherwise(norm__gblt__arg_9_interrupt);
  unify(a6, a5);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__normalize_xnorm__gblt__arg__2_8);
  allocp[2] = x0 = makeref(&allocp[2]);
  allocp[3] = a1;
  allocp[4] = a2;
  allocp[5] = x1 = makeref(&allocp[5]);
  allocp[6] = a4;
  allocp[7] = a0;
  allocp[8] = a7;
  allocp[9] = a8;
  a1 = a2;
  a2 = x1;
  a3 = makesym(atom_yes);
  a4 = x0;
  qp = (struct goalrec*)&allocp[0];
  allocp += 10;
  execute(is__const_5_0);
  goto is__const_5_ext_interrupt;
 norm__gblt__arg_9_ext_interrupt:
  reasonp = 0l;
 norm__gblt__arg_9_interrupt:
  toppred = &predicate_klic__comp__normalize_xnorm__gblt__arg_9;
  goto interrupt_9;
 }

 norm__gblt__arg__1_10_top: {
  q x0, x1, x2, x3;
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  a6 = qp->args[6];
  a7 = qp->args[7];
  a8 = qp->args[8];
  a9 = qp->args[9];
  qp = qp->next;
 norm__gblt__arg__1_10_clear_reason:
  reasonp = reasons;
 norm__gblt__arg__1_10_0:
 norm__gblt__arg__1_10_1:
  switch (ptagof(a5)) {
 case FUNCTOR:
  if (functor_of(a5) != makesym(functor_yes_1)) goto norm__gblt__arg__1_10_3;
  x0 = arg(a5, 0);
 norm__gblt__arg__1_10_2:
  switch (ptagof(x0)) {
 case FUNCTOR:
  if (functor_of(x0) != makesym(functor_rename_1)) goto norm__gblt__arg__1_10_3;
  allocp[0] = makesym(functor_variable_1);
  x2 = arg(x0, 0);
  allocp[1] = x2;
  x1 = makefunctor(&allocp[0]);
  a0 = x1;
  a5 = a6;
  a6 = a7;
  a7 = a8;
  a8 = a9;
  allocp += 2;
  execute(norm__gblt__arg_9_0);
  goto norm__gblt__arg_9_ext_interrupt;
 case VARREF:
  deref_and_jump(x0,norm__gblt__arg__1_10_2);
  *reasonp++ =  x0;
  goto norm__gblt__arg__1_10_3;
  };
  goto norm__gblt__arg__1_10_3;
 case VARREF:
  deref_and_jump(a5,norm__gblt__arg__1_10_1);
  *reasonp++ =  a5;
  goto norm__gblt__arg__1_10_3;
  };
  goto norm__gblt__arg__1_10_3;
 norm__gblt__arg__1_10_3:
  otherwise(norm__gblt__arg__1_10_interrupt);
 norm__gblt__arg__1_10_4:
  switch (ptagof(a5)) {
 case ATOMIC:
  if (a5 != makesym(atom_no)) goto norm__gblt__arg__1_10_interrupt;
  unify(a7, a6);
  x0 = makesym(atom_abnormal);
  unify_value(a4, x0);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__message_xwarning_4);
  allocp[2] = makefunctor(&string_const_6);
  allocp[3] = NILATOM;
  allocp[4] = a8;
  allocp[5] = a9;
  qp = (struct goalrec*)&allocp[0];
  allocp += 6;
  proceed();
 case FUNCTOR:
  if (functor_of(a5) != makesym(functor_yes_1)) goto norm__gblt__arg__1_10_interrupt;
  unify(a9, a8);
  x0 = arg(a5, 0);
  unify(a2, x0);
  allocp[0] = makesym(functor__3A_2);
  allocp[1] = x0;
  allocp[2] = a1;
  x1 = makefunctor(&allocp[0]);
  allocp[3] = a7;
  allocp[4] = x1;
  x2 = makecons(&allocp[3]);
  allocp += 5;
  unify_value(a6, x2);
  x3 = makesym(atom_normal);
  unify_value(a4, x3);
  proceed();
 case VARREF:
  deref_and_jump(a5,norm__gblt__arg__1_10_4);
  *reasonp++ =  a5;
  goto norm__gblt__arg__1_10_interrupt;
  };
  goto norm__gblt__arg__1_10_interrupt;
 norm__gblt__arg__1_10_ext_interrupt:
  reasonp = 0l;
 norm__gblt__arg__1_10_interrupt:
  goto interrupt_10;
 }

 norm__gblt__arg__2_8_top: {
  q x0, x1;
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  a6 = qp->args[6];
  a7 = qp->args[7];
  qp = qp->next;
 norm__gblt__arg__2_8_clear_reason:
  reasonp = reasons;
 norm__gblt__arg__2_8_0:
 norm__gblt__arg__2_8_1:
  switch (ptagof(a0)) {
 case ATOMIC:
  if (isint(a0)) goto norm__gblt__arg__2_8_interrupt;
  switch (symval(a0)) {
 case symval(makesym(atom_yes)):
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__normalize_xcheck__gblt__input_5);
  allocp[2] = x0 = makeref(&allocp[2]);
  allocp[3] = a4;
  allocp[4] = a5;
  allocp[5] = a6;
  allocp[6] = a7;
  allocp[7] = (q)(struct goalrec*)&allocp[0];
  allocp[8] = (q)(&predicate_klic__comp__insert_xsubsumed__type_3);
  allocp[9] = a3;
  allocp[10] = a1;
  allocp[11] = x0;
  qp = (struct goalrec*)&allocp[7];
  allocp += 12;
  proceed();
 case symval(makesym(atom_no)):
  x0 = makesym(atom_abnormal);
  unify_value(a4, x0);
  allocp[0] = NILATOM;
  allocp[1] = a5;
  x1 = makecons(&allocp[0]);
  allocp[2] = (q)qp;
  allocp[3] = (q)(&predicate_klic__comp__message_xwarning_4);
  allocp[4] = makefunctor(&string_const_10);
  allocp[5] = x1;
  allocp[6] = a6;
  allocp[7] = a7;
  qp = (struct goalrec*)&allocp[2];
  allocp += 8;
  proceed();
  };
  goto norm__gblt__arg__2_8_interrupt;
 case VARREF:
  deref_and_jump(a0,norm__gblt__arg__2_8_1);
  *reasonp++ =  a0;
  goto norm__gblt__arg__2_8_interrupt;
  };
  goto norm__gblt__arg__2_8_interrupt;
 norm__gblt__arg__2_8_ext_interrupt:
  reasonp = 0l;
 norm__gblt__arg__2_8_interrupt:
  goto interrupt_8;
 }

 check__gblt__input_5_top: {
  q x0, x1;
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  qp = qp->next;
 check__gblt__input_5_clear_reason:
  reasonp = reasons;
 check__gblt__input_5_0:
 check__gblt__input_5_1:
  switch (ptagof(a0)) {
 case ATOMIC:
  if (a0 != makesym(atom_normal)) goto check__gblt__input_5_2;
  unify(a4, a3);
  unify_value(a1, a0);
  proceed();
 case VARREF:
  deref_and_jump(a0,check__gblt__input_5_1);
  *reasonp++ =  a0;
  goto check__gblt__input_5_interrupt;
  };
  goto check__gblt__input_5_2;
 check__gblt__input_5_2:
  x0 = makesym(atom_normal);
  gblt_diff(a0,x0,check__gblt__input_5_interrupt);
  unify_value(a1, a0);
  allocp[0] = NILATOM;
  allocp[1] = a2;
  x1 = makecons(&allocp[0]);
  allocp[2] = (q)qp;
  allocp[3] = (q)(&predicate_klic__comp__message_xwarning_4);
  allocp[4] = makefunctor(&string_const_1);
  allocp[5] = x1;
  allocp[6] = a3;
  allocp[7] = a4;
  qp = (struct goalrec*)&allocp[2];
  allocp += 8;
  proceed();
 check__gblt__input_5_ext_interrupt:
  reasonp = 0l;
 check__gblt__input_5_interrupt:
  goto interrupt_5;
 }

 norm__body_6_top: {
  q x0, x1, x2, x3, x4, x5, x6;
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  qp = qp->next;
 norm__body_6_clear_reason:
  reasonp = reasons;
 norm__body_6_0:
 norm__body_6_1:
  switch (ptagof(a0)) {
 case CONS:
  x0 = car_of(a0);
 norm__body_6_2:
  switch (ptagof(x0)) {
 case FUNCTOR:
  if (functor_of(x0) != makesym(functor_functor_1)) goto norm__body_6_3;
  x2 = cdr_of(a0);
  allocp[0] = x2;
  allocp[1] = x0;
  x1 = makecons(&allocp[0]);
  allocp[2] = (q)qp;
  allocp[3] = (q)(&predicate_klic__comp__normalize_xnorm__body__1_9);
  allocp[4] = x1;
  allocp[5] = a1;
  allocp[6] = a2;
  allocp[7] = x3 = makeref(&allocp[7]);
  allocp[8] = a3;
  allocp[9] = x4 = makeref(&allocp[9]);
  allocp[10] = x5 = makeref(&allocp[10]);
  allocp[11] = x6 = makeref(&allocp[11]);
  allocp[12] = a5;
  a0 = arg(x0, 0);
  a1 = x4;
  a3 = x3;
  a5 = a4;
  a6 = x6;
  a4 = x5;
  qp = (struct goalrec*)&allocp[2];
  allocp += 13;
  execute(norm__top__goal_7_0);
  goto norm__top__goal_7_ext_interrupt;
 case VARREF:
  deref_and_jump(x0,norm__body_6_2);
  *reasonp++ =  x0;
  goto norm__body_6_3;
  };
  goto norm__body_6_3;
 case ATOMIC:
  if (a0 != NILATOM) goto norm__body_6_3;
  unify(a5, a4);
  x0 = NILATOM;
  unify_value(a1, x0);
  x1 = makesym(atom_normal);
  unify_value(a3, x1);
  proceed();
 case VARREF:
  deref_and_jump(a0,norm__body_6_1);
  *reasonp++ =  a0;
  goto norm__body_6_3;
  };
  goto norm__body_6_3;
 norm__body_6_3:
  otherwise(norm__body_6_interrupt);
  x0 = a2;
  x1 = a3;
  x2 = a4;
  a6 = a5;
  a5 = x2;
  a4 = x1;
  a3 = x0;
  a2 = NILATOM;
  execute(norm__body__tail_7_0);
  goto norm__body__tail_7_ext_interrupt;
 norm__body_6_ext_interrupt:
  reasonp = 0l;
 norm__body_6_interrupt:
  toppred = &predicate_klic__comp__normalize_xnorm__body_6;
  goto interrupt_6;
 }

 norm__body__1_9_top: {
  q x0, x1;
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  a6 = qp->args[6];
  a7 = qp->args[7];
  a8 = qp->args[8];
  qp = qp->next;
 norm__body__1_9_clear_reason:
  reasonp = reasons;
 norm__body__1_9_0:
 norm__body__1_9_1:
  switch (ptagof(a0)) {
 case CONS:
 norm__body__1_9_2:
  switch (ptagof(a6)) {
 case ATOMIC:
  if (a6 != makesym(atom_normal)) goto norm__body__1_9_3;
  allocp[0] = x1 = makeref(&allocp[0]);
  allocp[1] = a5;
  x0 = makecons(&allocp[0]);
  allocp += 2;
  unify_value(a1, x0);
  a0 = cdr_of(a0);
  a1 = x1;
  a2 = a3;
  a3 = a4;
  a4 = a7;
  a5 = a8;
  execute(norm__body_6_0);
  goto norm__body_6_ext_interrupt;
 case VARREF:
  deref_and_jump(a6,norm__body__1_9_2);
  *reasonp++ =  a6;
  goto norm__body__1_9_3;
  };
  goto norm__body__1_9_3;
 case VARREF:
  deref_and_jump(a0,norm__body__1_9_1);
  *reasonp++ =  a0;
  goto norm__body__1_9_3;
  };
  goto norm__body__1_9_3;
 norm__body__1_9_3:
 norm__body__1_9_4:
  if (isref(a6)) goto norm__body__1_9_5;
  x0 = makesym(atom_normal);
  gblt_diff(a6,x0,norm__body__1_9_interrupt);
  a3 = a2;
  a5 = a7;
  a6 = a8;
  a2 = NILATOM;
  execute(norm__body__tail_7_clear_reason);
  goto norm__body__tail_7_ext_interrupt;
 norm__body__1_9_5:
  deref_and_jump(a6,norm__body__1_9_4);
  *reasonp++ =  a6;
  goto norm__body__1_9_interrupt;
 norm__body__1_9_ext_interrupt:
  reasonp = 0l;
 norm__body__1_9_interrupt:
  goto interrupt_9;
 }

 norm__body__tail_7_top: {
  q x0, x1, x2, x3, x4, x5;
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  a6 = qp->args[6];
  qp = qp->next;
 norm__body__tail_7_clear_reason:
  reasonp = reasons;
 norm__body__tail_7_0:
 norm__body__tail_7_1:
  switch (ptagof(a0)) {
 case CONS:
  allocp[0] = a2;
  allocp[1] = x1 = makeref(&allocp[1]);
  x0 = makecons(&allocp[0]);
  allocp[2] = (q)qp;
  allocp[3] = (q)(&predicate_klic__comp__normalize_xnorm__body__tail__1_8);
  x2 = cdr_of(a0);
  allocp[4] = x2;
  allocp[5] = a1;
  allocp[6] = x0;
  allocp[7] = x3 = makeref(&allocp[7]);
  allocp[8] = a4;
  allocp[9] = x4 = makeref(&allocp[9]);
  allocp[10] = x5 = makeref(&allocp[10]);
  allocp[11] = a6;
  a0 = car_of(a0);
  a1 = x1;
  a2 = a3;
  a3 = x3;
  a4 = x4;
  a6 = x5;
  qp = (struct goalrec*)&allocp[2];
  allocp += 12;
  execute(norm__tail__goal_7_0);
  goto norm__tail__goal_7_ext_interrupt;
 case ATOMIC:
  if (a0 != NILATOM) goto norm__body__tail_7_interrupt;
  unify2(a6, a5, a1, a2);
  x0 = makesym(atom_normal);
  unify_value(a4, x0);
  proceed();
 case VARREF:
  deref_and_jump(a0,norm__body__tail_7_1);
  *reasonp++ =  a0;
  goto norm__body__tail_7_interrupt;
  };
  goto norm__body__tail_7_interrupt;
 norm__body__tail_7_ext_interrupt:
  reasonp = 0l;
 norm__body__tail_7_interrupt:
  toppred = &predicate_klic__comp__normalize_xnorm__body__tail_7;
  goto interrupt_7;
 }

 norm__body__tail__1_8_top: {
  q x0;
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  a6 = qp->args[6];
  a7 = qp->args[7];
  qp = qp->next;
 norm__body__tail__1_8_clear_reason:
  reasonp = reasons;
 norm__body__tail__1_8_0:
 norm__body__tail__1_8_1:
  switch (ptagof(a5)) {
 case ATOMIC:
  if (a5 != makesym(atom_normal)) goto norm__body__tail__1_8_2;
  a5 = a6;
  a6 = a7;
  execute(norm__body__tail_7_0);
  goto norm__body__tail_7_ext_interrupt;
 case VARREF:
  deref_and_jump(a5,norm__body__tail__1_8_1);
  *reasonp++ =  a5;
  goto norm__body__tail__1_8_interrupt;
  };
  goto norm__body__tail__1_8_2;
 norm__body__tail__1_8_2:
  x0 = makesym(atom_normal);
  gblt_diff(a5,x0,norm__body__tail__1_8_interrupt);
  unify(a7, a6);
  unify_value(a4, a5);
  proceed();
 norm__body__tail__1_8_ext_interrupt:
  reasonp = 0l;
 norm__body__tail__1_8_interrupt:
  goto interrupt_8;
 }

 norm__top__goal_7_top: {
  q x0, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12;
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  a6 = qp->args[6];
  qp = qp->next;
 norm__top__goal_7_clear_reason:
  reasonp = reasons;
 norm__top__goal_7_0:
 norm__top__goal_7_1:
  switch (ptagof(a0)) {
 case FUNCTOR:
  switch (symval(functor_of(a0))) {
 case functor__3D_2:
  allocp[0] = makesym(functor__3D_2);
  allocp[1] = x1 = makeref(&allocp[1]);
  allocp[2] = x2 = makeref(&allocp[2]);
  x0 = makefunctor(&allocp[0]);
  allocp += 3;
  unify_value(a1, x0);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__normalize_xnorm__top__goal__unify_8);
  x3 = arg(a0, 1);
  allocp[2] = x3;
  allocp[3] = x2;
  allocp[4] = x4 = makeref(&allocp[4]);
  allocp[5] = a3;
  allocp[6] = a4;
  allocp[7] = x5 = makeref(&allocp[7]);
  allocp[8] = x6 = makeref(&allocp[8]);
  allocp[9] = a6;
  a0 = arg(a0, 0);
  a1 = x1;
  a3 = x4;
  a4 = x5;
  a6 = x6;
  qp = (struct goalrec*)&allocp[0];
  allocp += 10;
  execute(norm__body__arg_7_0);
  goto norm__body__arg_7_ext_interrupt;
 case functor__3A_2:
  x0 = arg(a0, 1);
 norm__top__goal_7_2:
  switch (ptagof(x0)) {
 case FUNCTOR:
  if (functor_of(x0) != makesym(functor_functor_1)) goto norm__top__goal_7_6;
  x1 = arg(x0, 0);
 norm__top__goal_7_3:
  switch (ptagof(x1)) {
 case FUNCTOR:
  if (functor_of(x1) != makesym(functor__40_2)) goto norm__top__goal_7_6;
  x2 = arg(a0, 0);
 norm__top__goal_7_4:
  switch (ptagof(x2)) {
 case FUNCTOR:
  if (functor_of(x2) != makesym(functor_atom_1)) goto norm__top__goal_7_6;
  x3 = arg(x2, 0);
 norm__top__goal_7_5:
  switch (ptagof(x3)) {
 case ATOMIC:
  if (x3 != makesym(atom_generic)) goto norm__top__goal_7_6;
  allocp[0] = makesym(functor__3A_2);
  allocp[1] = x2;
  x5 = arg(x1, 0);
  allocp[2] = x5;
  x4 = makefunctor(&allocp[0]);
  allocp[3] = makesym(functor_functor_1);
  allocp[4] = x4;
  x6 = makefunctor(&allocp[3]);
  allocp[5] = (q)qp;
  allocp[6] = (q)(&predicate_klic__comp__normalize_xnorm__top__goal_7);
  allocp[7] = x6;
  allocp[8] = a1;
  allocp[9] = a2;
  allocp[10] = a3;
  allocp[11] = a4;
  allocp[12] = x7 = makeref(&allocp[12]);
  allocp[13] = a6;
  allocp[14] = makesym(functor__40_2);
  x9 = arg(x1, 0);
  allocp[15] = x9;
  x10 = arg(x1, 1);
  allocp[16] = x10;
  x8 = makefunctor(&allocp[14]);
  allocp[17] = makesym(functor__3A_2);
  allocp[18] = makesym(atom_generic);
  allocp[19] = x8;
  x11 = makefunctor(&allocp[17]);
  allocp[20] = NILATOM;
  allocp[21] = x11;
  x12 = makecons(&allocp[20]);
  allocp[22] = (q)(struct goalrec*)&allocp[5];
  allocp[23] = (q)(&predicate_klic__comp__message_xwarning_4);
  allocp[24] = makefunctor(&string_const_2);
  allocp[25] = x12;
  allocp[26] = a5;
  allocp[27] = x7;
  qp = (struct goalrec*)&allocp[22];
  allocp += 28;
  proceed();
 case VARREF:
  deref_and_jump(x3,norm__top__goal_7_5);
  *reasonp++ =  x3;
  goto norm__top__goal_7_6;
  };
  goto norm__top__goal_7_6;
 case VARREF:
  deref_and_jump(x2,norm__top__goal_7_4);
  *reasonp++ =  x2;
  goto norm__top__goal_7_6;
  };
  goto norm__top__goal_7_6;
 case VARREF:
  deref_and_jump(x1,norm__top__goal_7_3);
  *reasonp++ =  x1;
  goto norm__top__goal_7_6;
  };
  goto norm__top__goal_7_6;
 case VARREF:
  deref_and_jump(x0,norm__top__goal_7_2);
  *reasonp++ =  x0;
  goto norm__top__goal_7_6;
  };
  goto norm__top__goal_7_6;
  };
  goto norm__top__goal_7_6;
 case VARREF:
  deref_and_jump(a0,norm__top__goal_7_1);
  *reasonp++ =  a0;
  goto norm__top__goal_7_6;
  };
  goto norm__top__goal_7_6;
 norm__top__goal_7_6:
  otherwise(norm__top__goal_7_interrupt);
 norm__top__goal_7_7:
  switch (ptagof(a0)) {
 case FUNCTOR:
  if (functor_of(a0) != makesym(functor__3A_2)) goto norm__top__goal_7_10;
  x0 = arg(a0, 0);
 norm__top__goal_7_8:
  switch (ptagof(x0)) {
 case FUNCTOR:
  if (functor_of(x0) != makesym(functor_atom_1)) goto norm__top__goal_7_10;
  x1 = arg(x0, 0);
 norm__top__goal_7_9:
  switch (ptagof(x1)) {
 case ATOMIC:
  if (x1 != makesym(atom_generic)) goto norm__top__goal_7_10;
  allocp[0] = makesym(functor_gcall_3);
  allocp[1] = x3 = makeref(&allocp[1]);
  allocp[2] = x4 = makeref(&allocp[2]);
  allocp[3] = x5 = makeref(&allocp[3]);
  x2 = makefunctor(&allocp[0]);
  allocp += 4;
  unify_value(a1, x2);
  a0 = arg(a0, 1);
  a1 = x3;
  x6 = a2;
  x7 = a3;
  x8 = a4;
  a7 = a5;
  a8 = a6;
  a6 = x8;
  a5 = x7;
  a4 = x6;
  a3 = x5;
  a2 = x4;
  execute(norm__body__args_9_0);
  goto norm__body__args_9_ext_interrupt;
 case VARREF:
  deref_and_jump(x1,norm__top__goal_7_9);
  *reasonp++ =  x1;
  goto norm__top__goal_7_10;
  };
  goto norm__top__goal_7_10;
 case VARREF:
  deref_and_jump(x0,norm__top__goal_7_8);
  *reasonp++ =  x0;
  goto norm__top__goal_7_10;
  };
  goto norm__top__goal_7_10;
 case VARREF:
  deref_and_jump(a0,norm__top__goal_7_7);
  *reasonp++ =  a0;
  goto norm__top__goal_7_10;
  };
  goto norm__top__goal_7_10;
 norm__top__goal_7_10:
  otherwise(norm__top__goal_7_interrupt);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__normalize_xnorm__top__goal__builtin_7);
  allocp[2] = x0 = makeref(&allocp[2]);
  allocp[3] = a1;
  allocp[4] = a2;
  allocp[5] = a3;
  allocp[6] = a4;
  allocp[7] = a5;
  allocp[8] = a6;
  allocp[9] = (q)(struct goalrec*)&allocp[0];
  allocp[10] = (q)(&predicate_klic__comp__bb_xis__body__builtin_2);
  allocp[11] = a0;
  allocp[12] = x0;
  qp = (struct goalrec*)&allocp[9];
  allocp += 13;
  proceed();
 norm__top__goal_7_11:
  otherwise(norm__top__goal_7_interrupt);
  unify(a6, a5);
  x0 = makesym(atom_abnormal);
  unify_value(a4, x0);
  proceed();
 norm__top__goal_7_ext_interrupt:
  reasonp = 0l;
 norm__top__goal_7_interrupt:
  toppred = &predicate_klic__comp__normalize_xnorm__top__goal_7;
  goto interrupt_7;
 }

 norm__top__goal__unify_8_top: {
  q x0;
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  a6 = qp->args[6];
  a7 = qp->args[7];
  qp = qp->next;
 norm__top__goal__unify_8_clear_reason:
  reasonp = reasons;
 norm__top__goal__unify_8_0:
 norm__top__goal__unify_8_1:
  switch (ptagof(a5)) {
 case ATOMIC:
  if (a5 != makesym(atom_normal)) goto norm__top__goal__unify_8_2;
  a5 = a6;
  a6 = a7;
  execute(norm__body__arg_7_0);
  goto norm__body__arg_7_ext_interrupt;
 case VARREF:
  deref_and_jump(a5,norm__top__goal__unify_8_1);
  *reasonp++ =  a5;
  goto norm__top__goal__unify_8_interrupt;
  };
  goto norm__top__goal__unify_8_2;
 norm__top__goal__unify_8_2:
  x0 = makesym(atom_normal);
  gblt_diff(a5,x0,norm__top__goal__unify_8_interrupt);
  unify(a7, a6);
  unify_value(a4, a5);
  proceed();
 norm__top__goal__unify_8_ext_interrupt:
  reasonp = 0l;
 norm__top__goal__unify_8_interrupt:
  goto interrupt_8;
 }

 norm__top__goal__builtin_7_top: {
  q x0, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10;
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  a6 = qp->args[6];
  qp = qp->next;
 norm__top__goal__builtin_7_clear_reason:
  reasonp = reasons;
 norm__top__goal__builtin_7_0:
 norm__top__goal__builtin_7_1:
  switch (ptagof(a0)) {
 case ATOMIC:
  if (a0 != makesym(atom_no)) goto norm__top__goal__builtin_7_interrupt;
  unify(a6, a5);
  x0 = makesym(atom_abnormal);
  unify_value(a4, x0);
  proceed();
 case FUNCTOR:
  if (functor_of(a0) != makesym(functor_yes_4)) goto norm__top__goal__builtin_7_interrupt;
  allocp[0] = makesym(functor_builtin_5);
  allocp[1] = x1 = makeref(&allocp[1]);
  allocp[2] = x2 = makeref(&allocp[2]);
  allocp[3] = x3 = makeref(&allocp[3]);
  x4 = arg(a0, 2);
  allocp[4] = x4;
  x5 = arg(a0, 3);
  allocp[5] = x5;
  x0 = makefunctor(&allocp[0]);
  allocp += 6;
  unify_value(a1, x0);
  allocp[0] = makesym(functor_functor_1);
  x7 = arg(a0, 0);
  allocp[1] = x7;
  x6 = makefunctor(&allocp[0]);
  a0 = x6;
  a1 = x1;
  x8 = a2;
  x9 = a3;
  x10 = a4;
  a7 = a5;
  a8 = a6;
  a6 = x10;
  a5 = x9;
  a4 = x8;
  a3 = x3;
  a2 = x2;
  allocp += 2;
  execute(norm__body__args_9_0);
  goto norm__body__args_9_ext_interrupt;
 case VARREF:
  deref_and_jump(a0,norm__top__goal__builtin_7_1);
  *reasonp++ =  a0;
  goto norm__top__goal__builtin_7_interrupt;
  };
  goto norm__top__goal__builtin_7_interrupt;
 norm__top__goal__builtin_7_ext_interrupt:
  reasonp = 0l;
 norm__top__goal__builtin_7_interrupt:
  goto interrupt_7;
 }

 norm__tail__goal_7_top: {
  q x0, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13;
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  a6 = qp->args[6];
  qp = qp->next;
 norm__tail__goal_7_clear_reason:
  reasonp = reasons;
 norm__tail__goal_7_0:
 norm__tail__goal_7_1:
  switch (ptagof(a0)) {
 case FUNCTOR:
  switch (symval(functor_of(a0))) {
 case functor_functor_1:
  x0 = arg(a0, 0);
 norm__tail__goal_7_2:
  switch (ptagof(x0)) {
 case FUNCTOR:
  switch (symval(functor_of(x0))) {
 case functor__3D_2:
  allocp[0] = makesym(functor__3D_2);
  allocp[1] = x2 = makeref(&allocp[1]);
  allocp[2] = x3 = makeref(&allocp[2]);
  x1 = makefunctor(&allocp[0]);
  allocp += 3;
  unify_value(a1, x1);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__normalize_xnorm__tail__goal__unify_8);
  x4 = arg(x0, 1);
  allocp[2] = x4;
  allocp[3] = x3;
  allocp[4] = x5 = makeref(&allocp[4]);
  allocp[5] = a3;
  allocp[6] = a4;
  allocp[7] = x6 = makeref(&allocp[7]);
  allocp[8] = x7 = makeref(&allocp[8]);
  allocp[9] = a6;
  a0 = arg(x0, 0);
  a1 = x2;
  a3 = x5;
  a4 = x6;
  a6 = x7;
  qp = (struct goalrec*)&allocp[0];
  allocp += 10;
  execute(norm__body__arg_7_0);
  goto norm__body__arg_7_ext_interrupt;
 case functor__3A_2:
  x1 = arg(x0, 1);
 norm__tail__goal_7_3:
  switch (ptagof(x1)) {
 case FUNCTOR:
  if (functor_of(x1) != makesym(functor_functor_1)) goto norm__tail__goal_7_8;
  x2 = arg(x1, 0);
 norm__tail__goal_7_4:
  switch (ptagof(x2)) {
 case FUNCTOR:
  if (functor_of(x2) != makesym(functor__40_2)) goto norm__tail__goal_7_8;
  x3 = arg(x0, 0);
 norm__tail__goal_7_5:
  switch (ptagof(x3)) {
 case FUNCTOR:
  if (functor_of(x3) != makesym(functor_atom_1)) goto norm__tail__goal_7_8;
  x4 = arg(x3, 0);
 norm__tail__goal_7_6:
  switch (ptagof(x4)) {
 case ATOMIC:
  if (x4 != makesym(atom_generic)) goto norm__tail__goal_7_8;
  allocp[0] = makesym(functor__3A_2);
  allocp[1] = x3;
  x6 = arg(x2, 0);
  allocp[2] = x6;
  x5 = makefunctor(&allocp[0]);
  allocp[3] = makesym(functor_functor_1);
  allocp[4] = x5;
  x7 = makefunctor(&allocp[3]);
  allocp[5] = (q)qp;
  allocp[6] = (q)(&predicate_klic__comp__normalize_xnorm__tail__goal_7);
  allocp[7] = x7;
  allocp[8] = a1;
  allocp[9] = a2;
  allocp[10] = a3;
  allocp[11] = a4;
  allocp[12] = x8 = makeref(&allocp[12]);
  allocp[13] = a6;
  allocp[14] = makesym(functor__40_2);
  x10 = arg(x2, 0);
  allocp[15] = x10;
  x11 = arg(x2, 1);
  allocp[16] = x11;
  x9 = makefunctor(&allocp[14]);
  allocp[17] = makesym(functor__3A_2);
  allocp[18] = makesym(atom_generic);
  allocp[19] = x9;
  x12 = makefunctor(&allocp[17]);
  allocp[20] = NILATOM;
  allocp[21] = x12;
  x13 = makecons(&allocp[20]);
  allocp[22] = (q)(struct goalrec*)&allocp[5];
  allocp[23] = (q)(&predicate_klic__comp__message_xwarning_4);
  allocp[24] = makefunctor(&string_const_2);
  allocp[25] = x13;
  allocp[26] = a5;
  allocp[27] = x8;
  qp = (struct goalrec*)&allocp[22];
  allocp += 28;
  proceed();
 case VARREF:
  deref_and_jump(x4,norm__tail__goal_7_6);
  *reasonp++ =  x4;
  goto norm__tail__goal_7_8;
  };
  goto norm__tail__goal_7_8;
 case VARREF:
  deref_and_jump(x3,norm__tail__goal_7_5);
  *reasonp++ =  x3;
  goto norm__tail__goal_7_8;
  };
  goto norm__tail__goal_7_8;
 case VARREF:
  deref_and_jump(x2,norm__tail__goal_7_4);
  *reasonp++ =  x2;
  goto norm__tail__goal_7_8;
  };
  goto norm__tail__goal_7_8;
 case VARREF:
  deref_and_jump(x1,norm__tail__goal_7_3);
  *reasonp++ =  x1;
  goto norm__tail__goal_7_8;
  };
  goto norm__tail__goal_7_8;
  };
  goto norm__tail__goal_7_8;
 case VARREF:
  deref_and_jump(x0,norm__tail__goal_7_2);
  *reasonp++ =  x0;
  goto norm__tail__goal_7_8;
  };
  goto norm__tail__goal_7_8;
 case functor__24macroexpanded_2:
  x0 = arg(a0, 1);
 norm__tail__goal_7_7:
  switch (ptagof(x0)) {
 case ATOMIC:
  if (x0 != NILATOM) goto norm__tail__goal_7_8;
  allocp[0] = makesym(functor_atom_1);
  x2 = arg(a0, 0);
  allocp[1] = x2;
  x1 = makefunctor(&allocp[0]);
  a0 = x1;
  allocp += 2;
  execute(norm__tail__goal_7_0);
  goto norm__tail__goal_7_ext_interrupt;
 case VARREF:
  deref_and_jump(x0,norm__tail__goal_7_7);
  *reasonp++ =  x0;
  goto norm__tail__goal_7_8;
  };
  goto norm__tail__goal_7_8;
  };
  goto norm__tail__goal_7_8;
 case VARREF:
  deref_and_jump(a0,norm__tail__goal_7_1);
  *reasonp++ =  a0;
  goto norm__tail__goal_7_8;
  };
  goto norm__tail__goal_7_8;
 norm__tail__goal_7_8:
  otherwise(norm__tail__goal_7_interrupt);
 norm__tail__goal_7_9:
  switch (ptagof(a0)) {
 case FUNCTOR:
  switch (symval(functor_of(a0))) {
 case functor__24macroexpanded_2:
  allocp[0] = makesym(functor_functor_1);
  allocp[1] = x1 = makeref(&allocp[1]);
  x0 = makefunctor(&allocp[0]);
  allocp[2] = (q)qp;
  allocp[3] = (q)(&predicate_klic__comp__normalize_xnorm__tail__goal_7);
  allocp[4] = x0;
  allocp[5] = a1;
  allocp[6] = a2;
  allocp[7] = a3;
  allocp[8] = a4;
  allocp[9] = a5;
  allocp[10] = a6;
  x3 = arg(a0, 1);
  allocp[11] = x3;
  x4 = arg(a0, 0);
  allocp[12] = x4;
  x2 = makecons(&allocp[11]);
  allocp[13] = (q)(struct goalrec*)&allocp[2];
  allocp[14] = (q)(&predicate_functor__table_x_3D_2E_2E_2);
  allocp[15] = x1;
  allocp[16] = x2;
  qp = (struct goalrec*)&allocp[13];
  allocp += 17;
  proceed();
 case functor_functor_1:
  x0 = arg(a0, 0);
 norm__tail__goal_7_10:
  switch (ptagof(x0)) {
 case FUNCTOR:
  if (functor_of(x0) != makesym(functor__3A_2)) goto norm__tail__goal_7_14;
  x1 = arg(x0, 0);
 norm__tail__goal_7_11:
  switch (ptagof(x1)) {
 case FUNCTOR:
  if (functor_of(x1) != makesym(functor_atom_1)) goto norm__tail__goal_7_14;
  x2 = arg(x1, 0);
 norm__tail__goal_7_12:
  switch (ptagof(x2)) {
 case ATOMIC:
  if (x2 != makesym(atom_generic)) goto norm__tail__goal_7_14;
  x3 = arg(x0, 1);
 norm__tail__goal_7_13:
  switch (ptagof(x3)) {
 case FUNCTOR:
  switch (symval(functor_of(x3))) {
 case functor_functor_1:
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__normalize_xnorm__tail__goal__generic_9);
  allocp[2] = x3;
  allocp[3] = a1;
  allocp[4] = a2;
  allocp[5] = a3;
  allocp[6] = a4;
  allocp[7] = x4 = makeref(&allocp[7]);
  allocp[8] = x5 = makeref(&allocp[8]);
  allocp[9] = a5;
  allocp[10] = a6;
  allocp[11] = x5;
  allocp[12] = x4;
  x6 = makecons(&allocp[11]);
  allocp[13] = (q)(struct goalrec*)&allocp[0];
  allocp[14] = (q)(&predicate_klic__comp__util_xuniv_2);
  x7 = arg(x3, 0);
  allocp[15] = x7;
  allocp[16] = x6;
  qp = (struct goalrec*)&allocp[13];
  allocp += 17;
  proceed();
 case functor_atom_1:
  a0 = x3;
  x4 = arg(x3, 0);
  a7 = a5;
  a8 = a6;
  a6 = NILATOM;
  a5 = x4;
  execute(norm__tail__goal__generic_9_0);
  goto norm__tail__goal__generic_9_ext_interrupt;
  };
  goto norm__tail__goal_7_14;
 case VARREF:
  deref_and_jump(x3,norm__tail__goal_7_13);
  *reasonp++ =  x3;
  goto norm__tail__goal_7_14;
  };
  goto norm__tail__goal_7_14;
 case VARREF:
  deref_and_jump(x2,norm__tail__goal_7_12);
  *reasonp++ =  x2;
  goto norm__tail__goal_7_14;
  };
  goto norm__tail__goal_7_14;
 case VARREF:
  deref_and_jump(x1,norm__tail__goal_7_11);
  *reasonp++ =  x1;
  goto norm__tail__goal_7_14;
  };
  goto norm__tail__goal_7_14;
 case VARREF:
  deref_and_jump(x0,norm__tail__goal_7_10);
  *reasonp++ =  x0;
  goto norm__tail__goal_7_14;
  };
  goto norm__tail__goal_7_14;
  };
  goto norm__tail__goal_7_14;
 case VARREF:
  deref_and_jump(a0,norm__tail__goal_7_9);
  *reasonp++ =  a0;
  goto norm__tail__goal_7_14;
  };
  goto norm__tail__goal_7_14;
 norm__tail__goal_7_14:
  otherwise(norm__tail__goal_7_interrupt);
 norm__tail__goal_7_15:
  switch (ptagof(a0)) {
 case FUNCTOR:
  if (functor_of(a0) != makesym(functor_functor_1)) goto norm__tail__goal_7_20;
  x0 = arg(a0, 0);
 norm__tail__goal_7_16:
  switch (ptagof(x0)) {
 case FUNCTOR:
  if (functor_of(x0) != makesym(functor__3A_2)) goto norm__tail__goal_7_20;
  x1 = arg(x0, 0);
 norm__tail__goal_7_17:
  switch (ptagof(x1)) {
 case FUNCTOR:
  if (functor_of(x1) != makesym(functor_atom_1)) goto norm__tail__goal_7_20;
  x2 = arg(x0, 1);
 norm__tail__goal_7_18:
  switch (ptagof(x2)) {
 case FUNCTOR:
  if (functor_of(x2) != makesym(functor_functor_1)) goto norm__tail__goal_7_20;
  x3 = arg(x2, 0);
 norm__tail__goal_7_19:
  switch (ptagof(x3)) {
 case FUNCTOR:
  if (functor_of(x3) != makesym(functor__40_2)) goto norm__tail__goal_7_20;
  allocp[0] = makesym(functor__3A_2);
  allocp[1] = x1;
  x5 = arg(x3, 0);
  allocp[2] = x5;
  x4 = makefunctor(&allocp[0]);
  allocp[3] = makesym(functor_functor_1);
  allocp[4] = x4;
  x6 = makefunctor(&allocp[3]);
  a0 = x6;
  x7 = arg(x3, 1);
  x8 = a4;
  x9 = a5;
  a7 = a6;
  a6 = x9;
  a5 = x8;
  a4 = x7;
  allocp += 5;
  execute(norm__pragma_8_0);
  goto norm__pragma_8_ext_interrupt;
 case VARREF:
  deref_and_jump(x3,norm__tail__goal_7_19);
  *reasonp++ =  x3;
  goto norm__tail__goal_7_20;
  };
  goto norm__tail__goal_7_20;
 case VARREF:
  deref_and_jump(x2,norm__tail__goal_7_18);
  *reasonp++ =  x2;
  goto norm__tail__goal_7_20;
  };
  goto norm__tail__goal_7_20;
 case VARREF:
  deref_and_jump(x1,norm__tail__goal_7_17);
  *reasonp++ =  x1;
  goto norm__tail__goal_7_20;
  };
  goto norm__tail__goal_7_20;
 case VARREF:
  deref_and_jump(x0,norm__tail__goal_7_16);
  *reasonp++ =  x0;
  goto norm__tail__goal_7_20;
  };
  goto norm__tail__goal_7_20;
 case VARREF:
  deref_and_jump(a0,norm__tail__goal_7_15);
  *reasonp++ =  a0;
  goto norm__tail__goal_7_20;
  };
  goto norm__tail__goal_7_20;
 norm__tail__goal_7_20:
  otherwise(norm__tail__goal_7_interrupt);
 norm__tail__goal_7_21:
  switch (ptagof(a0)) {
 case FUNCTOR:
  if (functor_of(a0) != makesym(functor_functor_1)) goto norm__tail__goal_7_24;
  x0 = arg(a0, 0);
 norm__tail__goal_7_22:
  switch (ptagof(x0)) {
 case FUNCTOR:
  switch (symval(functor_of(x0))) {
 case functor__3A_2:
  x1 = arg(x0, 0);
 norm__tail__goal_7_23:
  switch (ptagof(x1)) {
 case FUNCTOR:
  if (functor_of(x1) != makesym(functor_atom_1)) goto norm__tail__goal_7_24;
  allocp[0] = makesym(functor_xcall_4);
  x3 = arg(x1, 0);
  allocp[1] = x3;
  allocp[2] = x4 = makeref(&allocp[2]);
  allocp[3] = x5 = makeref(&allocp[3]);
  allocp[4] = x6 = makeref(&allocp[4]);
  x2 = makefunctor(&allocp[0]);
  allocp += 5;
  unify_value(a1, x2);
  a0 = arg(x0, 1);
  a1 = x4;
  x7 = a2;
  x8 = a3;
  x9 = a4;
  a7 = a5;
  a8 = a6;
  a6 = x9;
  a5 = x8;
  a4 = x7;
  a3 = x6;
  a2 = x5;
  execute(norm__body__args_9_0);
  goto norm__body__args_9_ext_interrupt;
 case VARREF:
  deref_and_jump(x1,norm__tail__goal_7_23);
  *reasonp++ =  x1;
  goto norm__tail__goal_7_24;
  };
  goto norm__tail__goal_7_24;
 case functor__40_2:
  a0 = arg(x0, 0);
  x1 = arg(x0, 1);
  x2 = a4;
  x3 = a5;
  a7 = a6;
  a6 = x3;
  a5 = x2;
  a4 = x1;
  execute(norm__pragma_8_0);
  goto norm__pragma_8_ext_interrupt;
  };
  goto norm__tail__goal_7_24;
 case VARREF:
  deref_and_jump(x0,norm__tail__goal_7_22);
  *reasonp++ =  x0;
  goto norm__tail__goal_7_24;
  };
  goto norm__tail__goal_7_24;
 case VARREF:
  deref_and_jump(a0,norm__tail__goal_7_21);
  *reasonp++ =  a0;
  goto norm__tail__goal_7_24;
  };
  goto norm__tail__goal_7_24;
 norm__tail__goal_7_24:
  otherwise(norm__tail__goal_7_interrupt);
 norm__tail__goal_7_25:
  switch (ptagof(a0)) {
 case FUNCTOR:
  if (functor_of(a0) != makesym(functor_functor_1)) goto norm__tail__goal_7_26;
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__normalize_xnorm__tail__goal__builtin_8);
  allocp[2] = x0 = makeref(&allocp[2]);
  allocp[3] = a0;
  allocp[4] = a1;
  allocp[5] = a2;
  allocp[6] = a3;
  allocp[7] = a4;
  allocp[8] = a5;
  allocp[9] = a6;
  allocp[10] = (q)(struct goalrec*)&allocp[0];
  allocp[11] = (q)(&predicate_klic__comp__bb_xis__body__builtin_2);
  x1 = arg(a0, 0);
  allocp[12] = x1;
  allocp[13] = x0;
  qp = (struct goalrec*)&allocp[10];
  allocp += 14;
  proceed();
 case VARREF:
  deref_and_jump(a0,norm__tail__goal_7_25);
  *reasonp++ =  a0;
  goto norm__tail__goal_7_26;
  };
  goto norm__tail__goal_7_26;
 norm__tail__goal_7_26:
  otherwise(norm__tail__goal_7_interrupt);
  allocp[0] = makesym(functor_call_3);
  allocp[1] = x1 = makeref(&allocp[1]);
  allocp[2] = x2 = makeref(&allocp[2]);
  allocp[3] = x3 = makeref(&allocp[3]);
  x0 = makefunctor(&allocp[0]);
  allocp += 4;
  unify_value(a1, x0);
  a1 = x1;
  x4 = a2;
  x5 = a3;
  x6 = a4;
  a7 = a5;
  a8 = a6;
  a6 = x6;
  a5 = x5;
  a4 = x4;
  a3 = x3;
  a2 = x2;
  execute(norm__body__args_9_0);
  goto norm__body__args_9_ext_interrupt;
 norm__tail__goal_7_ext_interrupt:
  reasonp = 0l;
 norm__tail__goal_7_interrupt:
  toppred = &predicate_klic__comp__normalize_xnorm__tail__goal_7;
  goto interrupt_7;
 }

 norm__tail__goal__unify_8_top: {
  q x0;
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  a6 = qp->args[6];
  a7 = qp->args[7];
  qp = qp->next;
 norm__tail__goal__unify_8_clear_reason:
  reasonp = reasons;
 norm__tail__goal__unify_8_0:
 norm__tail__goal__unify_8_1:
  switch (ptagof(a5)) {
 case ATOMIC:
  if (a5 != makesym(atom_normal)) goto norm__tail__goal__unify_8_2;
  a5 = a6;
  a6 = a7;
  execute(norm__body__arg_7_0);
  goto norm__body__arg_7_ext_interrupt;
 case VARREF:
  deref_and_jump(a5,norm__tail__goal__unify_8_1);
  *reasonp++ =  a5;
  goto norm__tail__goal__unify_8_interrupt;
  };
  goto norm__tail__goal__unify_8_2;
 norm__tail__goal__unify_8_2:
  x0 = makesym(atom_normal);
  gblt_diff(a5,x0,norm__tail__goal__unify_8_interrupt);
  unify(a7, a6);
  unify_value(a4, a5);
  proceed();
 norm__tail__goal__unify_8_ext_interrupt:
  reasonp = 0l;
 norm__tail__goal__unify_8_interrupt:
  goto interrupt_8;
 }

 norm__tail__goal__generic_9_top: {
  q x0, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12;
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  a6 = qp->args[6];
  a7 = qp->args[7];
  a8 = qp->args[8];
  qp = qp->next;
 norm__tail__goal__generic_9_clear_reason:
  reasonp = reasons;
 norm__tail__goal__generic_9_0:
 norm__tail__goal__generic_9_1:
  switch (ptagof(a5)) {
 case ATOMIC:
  if (isint(a5)) goto norm__tail__goal__generic_9_5;
  switch (symval(a5)) {
 case symval(makesym(atom_new)):
  allocp[0] = makesym(functor_gcall_3);
  allocp[1] = x1 = makeref(&allocp[1]);
  allocp[2] = x2 = makeref(&allocp[2]);
  allocp[3] = x3 = makeref(&allocp[3]);
  x0 = makefunctor(&allocp[0]);
  allocp += 4;
  unify_value(a1, x0);
  a1 = x1;
  x4 = a2;
  a5 = a3;
  a6 = a4;
  a4 = x4;
  a3 = x3;
  a2 = x2;
  execute(norm__body__args_9_0);
  goto norm__body__args_9_ext_interrupt;
 case symval(makesym(atom_generic)):
 norm__tail__goal__generic_9_2:
  switch (ptagof(a6)) {
 case CONS:
  x0 = cdr_of(a6);
 norm__tail__goal__generic_9_3:
  switch (ptagof(x0)) {
 case CONS:
  x1 = cdr_of(x0);
 norm__tail__goal__generic_9_4:
  switch (ptagof(x1)) {
 case ATOMIC:
  if (x1 != NILATOM) goto norm__tail__goal__generic_9_5;
  allocp[0] = NILATOM;
  allocp[1] = x3 = makeref(&allocp[1]);
  x2 = makecons(&allocp[0]);
  allocp[2] = x2;
  allocp[3] = x5 = makeref(&allocp[3]);
  x4 = makecons(&allocp[2]);
  allocp[4] = makesym(functor_xcall_4);
  allocp[5] = makesym(atom_generic);
  allocp[6] = makesym(atom_generic);
  allocp[7] = makeint(2L);
  allocp[8] = x4;
  x6 = makefunctor(&allocp[4]);
  allocp += 9;
  unify_value(a1, x6);
  allocp[0] = NILATOM;
  allocp[1] = x5;
  x7 = makecons(&allocp[0]);
  allocp[2] = x7;
  allocp[3] = x3;
  x8 = makecons(&allocp[2]);
  allocp[4] = x9 = makeref(&allocp[4]);
  a1 = x9;
  allocp[5] = x11 = makeref(&allocp[5]);
  x10 = x11;
  x12 = a2;
  a5 = a3;
  a6 = a4;
  a4 = x12;
  a3 = x8;
  a2 = x10;
  allocp += 6;
  execute(norm__body__args_9_0);
  goto norm__body__args_9_ext_interrupt;
 case VARREF:
  deref_and_jump(x1,norm__tail__goal__generic_9_4);
  *reasonp++ =  x1;
  goto norm__tail__goal__generic_9_5;
  };
  goto norm__tail__goal__generic_9_5;
 case VARREF:
  deref_and_jump(x0,norm__tail__goal__generic_9_3);
  *reasonp++ =  x0;
  goto norm__tail__goal__generic_9_5;
  };
  goto norm__tail__goal__generic_9_5;
 case VARREF:
  deref_and_jump(a6,norm__tail__goal__generic_9_2);
  *reasonp++ =  a6;
  goto norm__tail__goal__generic_9_5;
  };
  goto norm__tail__goal__generic_9_5;
  };
  goto norm__tail__goal__generic_9_5;
 case VARREF:
  deref_and_jump(a5,norm__tail__goal__generic_9_1);
  *reasonp++ =  a5;
  goto norm__tail__goal__generic_9_5;
  };
  goto norm__tail__goal__generic_9_5;
 norm__tail__goal__generic_9_5:
  otherwise(norm__tail__goal__generic_9_interrupt);
 norm__tail__goal__generic_9_6:
  switch (ptagof(a6)) {
 case CONS:
  x0 = cdr_of(a6);
 norm__tail__goal__generic_9_7:
  switch (ptagof(x0)) {
 case ATOMIC:
  if (x0 != NILATOM) goto norm__tail__goal__generic_9_8;
  allocp[0] = makesym(functor_xcall_4);
  allocp[1] = makesym(atom_generic);
  allocp[2] = makesym(atom_generic);
  allocp[3] = makeint(2L);
  allocp[4] = x2 = makeref(&allocp[4]);
  x1 = makefunctor(&allocp[0]);
  allocp += 5;
  unify_value(a1, x1);
  allocp[0] = makesym(functor_atom_1);
  allocp[1] = a5;
  x3 = makefunctor(&allocp[0]);
  allocp[2] = makesym(functor__24_2);
  allocp[3] = x3;
  x5 = car_of(a6);
  allocp[4] = x5;
  x4 = makefunctor(&allocp[2]);
  allocp[5] = makesym(functor_functor_1);
  allocp[6] = x4;
  x6 = makefunctor(&allocp[5]);
  a0 = x6;
  allocp[7] = x7 = makeref(&allocp[7]);
  a1 = x7;
  allocp[8] = x9 = makeref(&allocp[8]);
  x8 = x9;
  x10 = a2;
  a5 = a3;
  a6 = a4;
  a4 = x10;
  a3 = x2;
  a2 = x8;
  allocp += 9;
  execute(norm__body__args_9_0);
  goto norm__body__args_9_ext_interrupt;
 case VARREF:
  deref_and_jump(x0,norm__tail__goal__generic_9_7);
  *reasonp++ =  x0;
  goto norm__tail__goal__generic_9_8;
  };
  goto norm__tail__goal__generic_9_8;
 case VARREF:
  deref_and_jump(a6,norm__tail__goal__generic_9_6);
  *reasonp++ =  a6;
  goto norm__tail__goal__generic_9_8;
  };
  goto norm__tail__goal__generic_9_8;
 norm__tail__goal__generic_9_8:
  otherwise(norm__tail__goal__generic_9_interrupt);
 norm__tail__goal__generic_9_9:
  switch (ptagof(a6)) {
 case CONS:
  allocp[0] = makesym(functor_xcall_4);
  allocp[1] = makesym(atom_generic);
  allocp[2] = makesym(atom_generic);
  allocp[3] = makeint(2L);
  allocp[4] = x1 = makeref(&allocp[4]);
  x0 = makefunctor(&allocp[0]);
  allocp += 5;
  unify_value(a1, x0);
  allocp[0] = makesym(functor_functor_1);
  allocp[1] = x3 = makeref(&allocp[1]);
  x2 = makefunctor(&allocp[0]);
  allocp[2] = makesym(functor__24_2);
  allocp[3] = x2;
  x5 = car_of(a6);
  allocp[4] = x5;
  x4 = makefunctor(&allocp[2]);
  allocp[5] = makesym(functor_functor_1);
  allocp[6] = x4;
  x6 = makefunctor(&allocp[5]);
  allocp[7] = (q)qp;
  allocp[8] = (q)(&predicate_klic__comp__normalize_xnorm__body__args_9);
  allocp[9] = x6;
  allocp[10] = x7 = makeref(&allocp[10]);
  allocp[11] = x8 = makeref(&allocp[11]);
  allocp[12] = x1;
  allocp[13] = a2;
  allocp[14] = a3;
  allocp[15] = a4;
  allocp[16] = a7;
  allocp[17] = a8;
  x10 = cdr_of(a6);
  allocp[18] = x10;
  allocp[19] = a5;
  x9 = makecons(&allocp[18]);
  allocp[20] = (q)(struct goalrec*)&allocp[7];
  allocp[21] = (q)(&predicate_functor__table_x_3D_2E_2E_2);
  allocp[22] = x3;
  allocp[23] = x9;
  qp = (struct goalrec*)&allocp[20];
  allocp += 24;
  proceed();
 case VARREF:
  deref_and_jump(a6,norm__tail__goal__generic_9_9);
  *reasonp++ =  a6;
  goto norm__tail__goal__generic_9_interrupt;
  };
  goto norm__tail__goal__generic_9_interrupt;
 norm__tail__goal__generic_9_ext_interrupt:
  reasonp = 0l;
 norm__tail__goal__generic_9_interrupt:
  toppred = &predicate_klic__comp__normalize_xnorm__tail__goal__generic_9;
  goto interrupt_9;
 }

 norm__tail__goal__builtin_8_top: {
  q x0, x1, x2, x3, x4, x5, x6, x7, x8;
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  a6 = qp->args[6];
  a7 = qp->args[7];
  qp = qp->next;
 norm__tail__goal__builtin_8_clear_reason:
  reasonp = reasons;
 norm__tail__goal__builtin_8_0:
 norm__tail__goal__builtin_8_1:
  switch (ptagof(a0)) {
 case ATOMIC:
  if (a0 != makesym(atom_no)) goto norm__tail__goal__builtin_8_interrupt;
  allocp[0] = makesym(functor_call_3);
  allocp[1] = x1 = makeref(&allocp[1]);
  allocp[2] = x2 = makeref(&allocp[2]);
  allocp[3] = x3 = makeref(&allocp[3]);
  x0 = makefunctor(&allocp[0]);
  allocp += 4;
  unify_value(a2, x0);
  a0 = a1;
  a1 = x1;
  a2 = x2;
  x4 = a3;
  x5 = a4;
  x6 = a5;
  x7 = a6;
  a8 = a7;
  a7 = x7;
  a6 = x6;
  a5 = x5;
  a4 = x4;
  a3 = x3;
  execute(norm__body__args_9_0);
  goto norm__body__args_9_ext_interrupt;
 case FUNCTOR:
  if (functor_of(a0) != makesym(functor_yes_4)) goto norm__tail__goal__builtin_8_interrupt;
  allocp[0] = makesym(functor_xcall_4);
  x1 = arg(a0, 1);
  allocp[1] = x1;
  allocp[2] = x2 = makeref(&allocp[2]);
  allocp[3] = x3 = makeref(&allocp[3]);
  allocp[4] = x4 = makeref(&allocp[4]);
  x0 = makefunctor(&allocp[0]);
  allocp += 5;
  unify_value(a2, x0);
  a0 = a1;
  a1 = x2;
  a2 = x3;
  x5 = a3;
  x6 = a4;
  x7 = a5;
  x8 = a6;
  a8 = a7;
  a7 = x8;
  a6 = x7;
  a5 = x6;
  a4 = x5;
  a3 = x4;
  execute(norm__body__args_9_0);
  goto norm__body__args_9_ext_interrupt;
 case VARREF:
  deref_and_jump(a0,norm__tail__goal__builtin_8_1);
  *reasonp++ =  a0;
  goto norm__tail__goal__builtin_8_interrupt;
  };
  goto norm__tail__goal__builtin_8_interrupt;
 norm__tail__goal__builtin_8_ext_interrupt:
  reasonp = 0l;
 norm__tail__goal__builtin_8_interrupt:
  goto interrupt_8;
 }

 norm__pragma_8_top: {
  q x0, x1;
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  a6 = qp->args[6];
  a7 = qp->args[7];
  qp = qp->next;
 norm__pragma_8_clear_reason:
  reasonp = reasons;
 norm__pragma_8_0:
 norm__pragma_8_1:
  switch (ptagof(a0)) {
 case FUNCTOR:
  switch (symval(functor_of(a0))) {
 case functor_functor_1:
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__normalize_xnorm__pragma__0_9);
  allocp[2] = x0 = makeref(&allocp[2]);
  allocp[3] = a0;
  allocp[4] = a1;
  allocp[5] = a2;
  allocp[6] = a3;
  allocp[7] = a4;
  allocp[8] = a5;
  allocp[9] = a6;
  allocp[10] = a7;
  allocp[11] = (q)(struct goalrec*)&allocp[0];
  allocp[12] = (q)(&predicate_klic__comp__bb_xis__body__builtin_2);
  x1 = arg(a0, 0);
  allocp[13] = x1;
  allocp[14] = x0;
  qp = (struct goalrec*)&allocp[11];
  allocp += 15;
  proceed();
 case functor_atom_1:
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__normalize_xnorm__pragma__0_9);
  allocp[2] = x0 = makeref(&allocp[2]);
  allocp[3] = a0;
  allocp[4] = a1;
  allocp[5] = a2;
  allocp[6] = a3;
  allocp[7] = a4;
  allocp[8] = a5;
  allocp[9] = a6;
  allocp[10] = a7;
  allocp[11] = (q)(struct goalrec*)&allocp[0];
  allocp[12] = (q)(&predicate_klic__comp__bb_xis__body__builtin_2);
  x1 = arg(a0, 0);
  allocp[13] = x1;
  allocp[14] = x0;
  qp = (struct goalrec*)&allocp[11];
  allocp += 15;
  proceed();
  };
  goto norm__pragma_8_interrupt;
 case VARREF:
  deref_and_jump(a0,norm__pragma_8_1);
  *reasonp++ =  a0;
  goto norm__pragma_8_interrupt;
  };
  goto norm__pragma_8_interrupt;
 norm__pragma_8_ext_interrupt:
  reasonp = 0l;
 norm__pragma_8_interrupt:
  toppred = &predicate_klic__comp__normalize_xnorm__pragma_8;
  goto interrupt_8;
 }

 norm__pragma__0_9_top: {
  q x0, x1, x2, x3;
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  a6 = qp->args[6];
  a7 = qp->args[7];
  a8 = qp->args[8];
  qp = qp->next;
 norm__pragma__0_9_clear_reason:
  reasonp = reasons;
 norm__pragma__0_9_0:
 norm__pragma__0_9_1:
  switch (ptagof(a0)) {
 case ATOMIC:
  if (a0 != makesym(atom_no)) goto norm__pragma__0_9_interrupt;
  a0 = a1;
  a1 = a2;
  a2 = a3;
  a3 = a4;
  a4 = a5;
  a5 = a6;
  a6 = a7;
  a7 = a8;
  execute(norm__pragma__1_8_0);
  goto norm__pragma__1_8_ext_interrupt;
 case FUNCTOR:
  if (functor_of(a0) != makesym(functor_yes_4)) goto norm__pragma__0_9_interrupt;
  allocp[0] = makesym(functor_atom_1);
  x1 = arg(a0, 1);
  allocp[1] = x1;
  x0 = makefunctor(&allocp[0]);
  allocp[2] = makesym(functor__3A_2);
  allocp[3] = x0;
  allocp[4] = a1;
  x2 = makefunctor(&allocp[2]);
  allocp[5] = makesym(functor_functor_1);
  allocp[6] = x2;
  x3 = makefunctor(&allocp[5]);
  a0 = x3;
  a1 = a2;
  a2 = a3;
  a3 = a4;
  a4 = a5;
  a5 = a6;
  a6 = a7;
  a7 = a8;
  allocp += 7;
  execute(norm__pragma__1_8_0);
  goto norm__pragma__1_8_ext_interrupt;
 case VARREF:
  deref_and_jump(a0,norm__pragma__0_9_1);
  *reasonp++ =  a0;
  goto norm__pragma__0_9_interrupt;
  };
  goto norm__pragma__0_9_interrupt;
 norm__pragma__0_9_ext_interrupt:
  reasonp = 0l;
 norm__pragma__0_9_interrupt:
  goto interrupt_9;
 }

 norm__pragma__1_8_top: {
  q x0, x1;
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  a6 = qp->args[6];
  a7 = qp->args[7];
  qp = qp->next;
 norm__pragma__1_8_clear_reason:
  reasonp = reasons;
 norm__pragma__1_8_0:
 norm__pragma__1_8_1:
  switch (ptagof(a4)) {
 case FUNCTOR:
  switch (symval(functor_of(a4))) {
 case functor_atom_1:
  x0 = arg(a4, 0);
 norm__pragma__1_8_2:
  switch (ptagof(x0)) {
 case ATOMIC:
  if (x0 != makesym(atom_lower__priority)) goto norm__pragma__1_8_4;
  a4 = makefunctor(funct_const_20);
  execute(norm__pragma__1_8_0);
  goto norm__pragma__1_8_ext_interrupt;
 case VARREF:
  deref_and_jump(x0,norm__pragma__1_8_2);
  *reasonp++ =  x0;
  goto norm__pragma__1_8_4;
  };
  goto norm__pragma__1_8_4;
 case functor_functor_1:
  x0 = arg(a4, 0);
 norm__pragma__1_8_3:
  switch (ptagof(x0)) {
 case FUNCTOR:
  switch (symval(functor_of(x0))) {
 case functor_priority_1:
  a4 = arg(x0, 0);
  execute(norm__pragma__priority_8_0);
  goto norm__pragma__priority_8_ext_interrupt;
 case functor_lower__priority_1:
  a4 = arg(x0, 0);
  execute(norm__pragma__lower__priority_8_0);
  goto norm__pragma__lower__priority_8_ext_interrupt;
 case functor_node_1:
  a4 = arg(x0, 0);
  execute(norm__pragma__node_8_0);
  goto norm__pragma__node_8_ext_interrupt;
  };
  goto norm__pragma__1_8_4;
 case VARREF:
  deref_and_jump(x0,norm__pragma__1_8_3);
  *reasonp++ =  x0;
  goto norm__pragma__1_8_4;
  };
  goto norm__pragma__1_8_4;
  };
  goto norm__pragma__1_8_4;
 case VARREF:
  deref_and_jump(a4,norm__pragma__1_8_1);
  *reasonp++ =  a4;
  goto norm__pragma__1_8_4;
  };
  goto norm__pragma__1_8_4;
 norm__pragma__1_8_4:
  otherwise(norm__pragma__1_8_interrupt);
  allocp[0] = NILATOM;
  allocp[1] = a0;
  x0 = makecons(&allocp[0]);
  allocp[2] = x0;
  allocp[3] = a4;
  x1 = makecons(&allocp[2]);
  allocp[4] = (q)qp;
  allocp[5] = (q)(&predicate_klic__comp__message_xreport__error_4);
  allocp[6] = makefunctor(&string_const_5);
  allocp[7] = x1;
  allocp[8] = a6;
  allocp[9] = a7;
  qp = (struct goalrec*)&allocp[4];
  allocp += 10;
  proceed();
 norm__pragma__1_8_ext_interrupt:
  reasonp = 0l;
 norm__pragma__1_8_interrupt:
  toppred = &predicate_klic__comp__normalize_xnorm__pragma__1_8;
  goto interrupt_8;
 }

 norm__pragma__priority_8_top: {
  q x0, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10;
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  a6 = qp->args[6];
  a7 = qp->args[7];
  qp = qp->next;
 norm__pragma__priority_8_clear_reason:
  reasonp = reasons;
 norm__pragma__priority_8_0:
 norm__pragma__priority_8_1:
  switch (ptagof(a0)) {
 case FUNCTOR:
  switch (symval(functor_of(a0))) {
 case functor_functor_1:
  x0 = arg(a0, 0);
 norm__pragma__priority_8_2:
  switch (ptagof(x0)) {
 case FUNCTOR:
  if (functor_of(x0) != makesym(functor__3A_2)) goto norm__pragma__priority_8_4;
  x1 = arg(x0, 0);
 norm__pragma__priority_8_3:
  switch (ptagof(x1)) {
 case FUNCTOR:
  if (functor_of(x1) != makesym(functor_atom_1)) goto norm__pragma__priority_8_4;
  allocp[0] = makesym(functor_xpcall_5);
  allocp[1] = x3 = makeref(&allocp[1]);
  x4 = arg(x1, 0);
  allocp[2] = x4;
  allocp[3] = x5 = makeref(&allocp[3]);
  allocp[4] = x6 = makeref(&allocp[4]);
  allocp[5] = x7 = makeref(&allocp[5]);
  x2 = makefunctor(&allocp[0]);
  allocp += 6;
  unify_value(a1, x2);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__normalize_xnorm__pragma__pragma__arg_8);
  allocp[2] = a4;
  allocp[3] = x3;
  allocp[4] = x8 = makeref(&allocp[4]);
  allocp[5] = a3;
  allocp[6] = a5;
  allocp[7] = x9 = makeref(&allocp[7]);
  allocp[8] = x10 = makeref(&allocp[8]);
  allocp[9] = a7;
  a0 = arg(x0, 1);
  a1 = x5;
  a3 = x7;
  a4 = a2;
  a5 = x8;
  a7 = a6;
  a8 = x10;
  a6 = x9;
  a2 = x6;
  qp = (struct goalrec*)&allocp[0];
  allocp += 10;
  execute(norm__body__args_9_0);
  goto norm__body__args_9_ext_interrupt;
 case VARREF:
  deref_and_jump(x1,norm__pragma__priority_8_3);
  *reasonp++ =  x1;
  goto norm__pragma__priority_8_4;
  };
  goto norm__pragma__priority_8_4;
 case VARREF:
  deref_and_jump(x0,norm__pragma__priority_8_2);
  *reasonp++ =  x0;
  goto norm__pragma__priority_8_4;
  };
  goto norm__pragma__priority_8_4;
 case functor_atom_1:
  allocp[0] = makesym(functor_pcall_4);
  allocp[1] = x1 = makeref(&allocp[1]);
  x2 = arg(a0, 0);
  allocp[2] = x2;
  allocp[3] = makeint(0L);
  allocp[4] = NILATOM;
  x0 = makefunctor(&allocp[0]);
  allocp += 5;
  unify_value(a1, x0);
  a0 = a4;
  a1 = x1;
  a4 = a5;
  a5 = makesym(atom_normal);
  execute(norm__pragma__pragma__arg_8_0);
  goto norm__pragma__pragma__arg_8_ext_interrupt;
  };
  goto norm__pragma__priority_8_4;
 case VARREF:
  deref_and_jump(a0,norm__pragma__priority_8_1);
  *reasonp++ =  a0;
  goto norm__pragma__priority_8_4;
  };
  goto norm__pragma__priority_8_4;
 norm__pragma__priority_8_4:
  otherwise(norm__pragma__priority_8_interrupt);
 norm__pragma__priority_8_5:
  switch (ptagof(a0)) {
 case FUNCTOR:
  if (functor_of(a0) != makesym(functor_functor_1)) goto norm__pragma__priority_8_interrupt;
  allocp[0] = makesym(functor_pcall_4);
  allocp[1] = x1 = makeref(&allocp[1]);
  allocp[2] = x2 = makeref(&allocp[2]);
  allocp[3] = x3 = makeref(&allocp[3]);
  allocp[4] = x4 = makeref(&allocp[4]);
  x0 = makefunctor(&allocp[0]);
  allocp += 5;
  unify_value(a1, x0);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__normalize_xnorm__pragma__pragma__arg_8);
  allocp[2] = a4;
  allocp[3] = x1;
  allocp[4] = x5 = makeref(&allocp[4]);
  allocp[5] = a3;
  allocp[6] = a5;
  allocp[7] = x6 = makeref(&allocp[7]);
  allocp[8] = x7 = makeref(&allocp[8]);
  allocp[9] = a7;
  allocp[10] = makesym(functor_functor_1);
  x9 = arg(a0, 0);
  allocp[11] = x9;
  x8 = makefunctor(&allocp[10]);
  a0 = x8;
  a1 = x2;
  a3 = x4;
  a4 = a2;
  a5 = x5;
  a7 = a6;
  a8 = x7;
  a6 = x6;
  a2 = x3;
  qp = (struct goalrec*)&allocp[0];
  allocp += 12;
  execute(norm__body__args_9_0);
  goto norm__body__args_9_ext_interrupt;
 case VARREF:
  deref_and_jump(a0,norm__pragma__priority_8_5);
  *reasonp++ =  a0;
  goto norm__pragma__priority_8_interrupt;
  };
  goto norm__pragma__priority_8_interrupt;
 norm__pragma__priority_8_ext_interrupt:
  reasonp = 0l;
 norm__pragma__priority_8_interrupt:
  toppred = &predicate_klic__comp__normalize_xnorm__pragma__priority_8;
  goto interrupt_8;
 }

 norm__pragma__lower__priority_8_top: {
  q x0, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10;
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  a6 = qp->args[6];
  a7 = qp->args[7];
  qp = qp->next;
 norm__pragma__lower__priority_8_clear_reason:
  reasonp = reasons;
 norm__pragma__lower__priority_8_0:
 norm__pragma__lower__priority_8_1:
  switch (ptagof(a0)) {
 case FUNCTOR:
  switch (symval(functor_of(a0))) {
 case functor_functor_1:
  x0 = arg(a0, 0);
 norm__pragma__lower__priority_8_2:
  switch (ptagof(x0)) {
 case FUNCTOR:
  if (functor_of(x0) != makesym(functor__3A_2)) goto norm__pragma__lower__priority_8_4;
  x1 = arg(x0, 0);
 norm__pragma__lower__priority_8_3:
  switch (ptagof(x1)) {
 case FUNCTOR:
  if (functor_of(x1) != makesym(functor_atom_1)) goto norm__pragma__lower__priority_8_4;
  allocp[0] = makesym(functor_xlcall_5);
  allocp[1] = x3 = makeref(&allocp[1]);
  x4 = arg(x1, 0);
  allocp[2] = x4;
  allocp[3] = x5 = makeref(&allocp[3]);
  allocp[4] = x6 = makeref(&allocp[4]);
  allocp[5] = x7 = makeref(&allocp[5]);
  x2 = makefunctor(&allocp[0]);
  allocp += 6;
  unify_value(a1, x2);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__normalize_xnorm__pragma__pragma__arg_8);
  allocp[2] = a4;
  allocp[3] = x3;
  allocp[4] = x8 = makeref(&allocp[4]);
  allocp[5] = a3;
  allocp[6] = a5;
  allocp[7] = x9 = makeref(&allocp[7]);
  allocp[8] = x10 = makeref(&allocp[8]);
  allocp[9] = a7;
  a0 = arg(x0, 1);
  a1 = x5;
  a3 = x7;
  a4 = a2;
  a5 = x8;
  a7 = a6;
  a8 = x10;
  a6 = x9;
  a2 = x6;
  qp = (struct goalrec*)&allocp[0];
  allocp += 10;
  execute(norm__body__args_9_0);
  goto norm__body__args_9_ext_interrupt;
 case VARREF:
  deref_and_jump(x1,norm__pragma__lower__priority_8_3);
  *reasonp++ =  x1;
  goto norm__pragma__lower__priority_8_4;
  };
  goto norm__pragma__lower__priority_8_4;
 case VARREF:
  deref_and_jump(x0,norm__pragma__lower__priority_8_2);
  *reasonp++ =  x0;
  goto norm__pragma__lower__priority_8_4;
  };
  goto norm__pragma__lower__priority_8_4;
 case functor_atom_1:
  allocp[0] = makesym(functor_lcall_4);
  allocp[1] = x1 = makeref(&allocp[1]);
  x2 = arg(a0, 0);
  allocp[2] = x2;
  allocp[3] = makeint(0L);
  allocp[4] = NILATOM;
  x0 = makefunctor(&allocp[0]);
  allocp += 5;
  unify_value(a1, x0);
  a0 = a4;
  a1 = x1;
  a4 = a5;
  a5 = makesym(atom_normal);
  execute(norm__pragma__pragma__arg_8_0);
  goto norm__pragma__pragma__arg_8_ext_interrupt;
  };
  goto norm__pragma__lower__priority_8_4;
 case VARREF:
  deref_and_jump(a0,norm__pragma__lower__priority_8_1);
  *reasonp++ =  a0;
  goto norm__pragma__lower__priority_8_4;
  };
  goto norm__pragma__lower__priority_8_4;
 norm__pragma__lower__priority_8_4:
  otherwise(norm__pragma__lower__priority_8_interrupt);
 norm__pragma__lower__priority_8_5:
  switch (ptagof(a0)) {
 case FUNCTOR:
  if (functor_of(a0) != makesym(functor_functor_1)) goto norm__pragma__lower__priority_8_interrupt;
  allocp[0] = makesym(functor_lcall_4);
  allocp[1] = x1 = makeref(&allocp[1]);
  allocp[2] = x2 = makeref(&allocp[2]);
  allocp[3] = x3 = makeref(&allocp[3]);
  allocp[4] = x4 = makeref(&allocp[4]);
  x0 = makefunctor(&allocp[0]);
  allocp += 5;
  unify_value(a1, x0);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__normalize_xnorm__pragma__pragma__arg_8);
  allocp[2] = a4;
  allocp[3] = x1;
  allocp[4] = x5 = makeref(&allocp[4]);
  allocp[5] = a3;
  allocp[6] = a5;
  allocp[7] = x6 = makeref(&allocp[7]);
  allocp[8] = x7 = makeref(&allocp[8]);
  allocp[9] = a7;
  a1 = x2;
  a3 = x4;
  a4 = a2;
  a5 = x5;
  a7 = a6;
  a8 = x7;
  a6 = x6;
  a2 = x3;
  qp = (struct goalrec*)&allocp[0];
  allocp += 10;
  execute(norm__body__args_9_0);
  goto norm__body__args_9_ext_interrupt;
 case VARREF:
  deref_and_jump(a0,norm__pragma__lower__priority_8_5);
  *reasonp++ =  a0;
  goto norm__pragma__lower__priority_8_interrupt;
  };
  goto norm__pragma__lower__priority_8_interrupt;
 norm__pragma__lower__priority_8_ext_interrupt:
  reasonp = 0l;
 norm__pragma__lower__priority_8_interrupt:
  toppred = &predicate_klic__comp__normalize_xnorm__pragma__lower__priority_8;
  goto interrupt_8;
 }

 norm__pragma__node_8_top: {
  q x0, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10;
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  a6 = qp->args[6];
  a7 = qp->args[7];
  qp = qp->next;
 norm__pragma__node_8_clear_reason:
  reasonp = reasons;
 norm__pragma__node_8_0:
 norm__pragma__node_8_1:
  switch (ptagof(a0)) {
 case FUNCTOR:
  switch (symval(functor_of(a0))) {
 case functor_functor_1:
  x0 = arg(a0, 0);
 norm__pragma__node_8_2:
  switch (ptagof(x0)) {
 case FUNCTOR:
  if (functor_of(x0) != makesym(functor__3A_2)) goto norm__pragma__node_8_4;
  x1 = arg(x0, 0);
 norm__pragma__node_8_3:
  switch (ptagof(x1)) {
 case FUNCTOR:
  if (functor_of(x1) != makesym(functor_atom_1)) goto norm__pragma__node_8_4;
  allocp[0] = makesym(functor_xthrow_5);
  allocp[1] = x3 = makeref(&allocp[1]);
  x4 = arg(x1, 0);
  allocp[2] = x4;
  allocp[3] = x5 = makeref(&allocp[3]);
  allocp[4] = x6 = makeref(&allocp[4]);
  allocp[5] = x7 = makeref(&allocp[5]);
  x2 = makefunctor(&allocp[0]);
  allocp += 6;
  unify_value(a1, x2);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__normalize_xnorm__pragma__pragma__arg_8);
  allocp[2] = a4;
  allocp[3] = x3;
  allocp[4] = x8 = makeref(&allocp[4]);
  allocp[5] = a3;
  allocp[6] = a5;
  allocp[7] = x9 = makeref(&allocp[7]);
  allocp[8] = x10 = makeref(&allocp[8]);
  allocp[9] = a7;
  a0 = arg(x0, 1);
  a1 = x5;
  a3 = x7;
  a4 = a2;
  a5 = x8;
  a7 = a6;
  a8 = x10;
  a6 = x9;
  a2 = x6;
  qp = (struct goalrec*)&allocp[0];
  allocp += 10;
  execute(norm__body__args_9_0);
  goto norm__body__args_9_ext_interrupt;
 case VARREF:
  deref_and_jump(x1,norm__pragma__node_8_3);
  *reasonp++ =  x1;
  goto norm__pragma__node_8_4;
  };
  goto norm__pragma__node_8_4;
 case VARREF:
  deref_and_jump(x0,norm__pragma__node_8_2);
  *reasonp++ =  x0;
  goto norm__pragma__node_8_4;
  };
  goto norm__pragma__node_8_4;
 case functor_atom_1:
  allocp[0] = makesym(functor_throw_4);
  allocp[1] = x1 = makeref(&allocp[1]);
  x2 = arg(a0, 0);
  allocp[2] = x2;
  allocp[3] = makeint(0L);
  allocp[4] = NILATOM;
  x0 = makefunctor(&allocp[0]);
  allocp += 5;
  unify_value(a1, x0);
  a0 = a4;
  a1 = x1;
  a4 = a5;
  a5 = makesym(atom_normal);
  execute(norm__pragma__pragma__arg_8_0);
  goto norm__pragma__pragma__arg_8_ext_interrupt;
  };
  goto norm__pragma__node_8_4;
 case VARREF:
  deref_and_jump(a0,norm__pragma__node_8_1);
  *reasonp++ =  a0;
  goto norm__pragma__node_8_4;
  };
  goto norm__pragma__node_8_4;
 norm__pragma__node_8_4:
  otherwise(norm__pragma__node_8_interrupt);
 norm__pragma__node_8_5:
  switch (ptagof(a0)) {
 case FUNCTOR:
  if (functor_of(a0) != makesym(functor_functor_1)) goto norm__pragma__node_8_interrupt;
  allocp[0] = makesym(functor_throw_4);
  allocp[1] = x1 = makeref(&allocp[1]);
  allocp[2] = x2 = makeref(&allocp[2]);
  allocp[3] = x3 = makeref(&allocp[3]);
  allocp[4] = x4 = makeref(&allocp[4]);
  x0 = makefunctor(&allocp[0]);
  allocp += 5;
  unify_value(a1, x0);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__normalize_xnorm__pragma__pragma__arg_8);
  allocp[2] = a4;
  allocp[3] = x1;
  allocp[4] = x5 = makeref(&allocp[4]);
  allocp[5] = a3;
  allocp[6] = a5;
  allocp[7] = x6 = makeref(&allocp[7]);
  allocp[8] = x7 = makeref(&allocp[8]);
  allocp[9] = a7;
  a1 = x2;
  a3 = x4;
  a4 = a2;
  a5 = x5;
  a7 = a6;
  a8 = x7;
  a6 = x6;
  a2 = x3;
  qp = (struct goalrec*)&allocp[0];
  allocp += 10;
  execute(norm__body__args_9_0);
  goto norm__body__args_9_ext_interrupt;
 case VARREF:
  deref_and_jump(a0,norm__pragma__node_8_5);
  *reasonp++ =  a0;
  goto norm__pragma__node_8_interrupt;
  };
  goto norm__pragma__node_8_interrupt;
 norm__pragma__node_8_ext_interrupt:
  reasonp = 0l;
 norm__pragma__node_8_interrupt:
  toppred = &predicate_klic__comp__normalize_xnorm__pragma__node_8;
  goto interrupt_8;
 }

 norm__pragma__pragma__arg_8_top: {
  q x0;
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  a6 = qp->args[6];
  a7 = qp->args[7];
  qp = qp->next;
 norm__pragma__pragma__arg_8_clear_reason:
  reasonp = reasons;
 norm__pragma__pragma__arg_8_0:
 norm__pragma__pragma__arg_8_1:
  switch (ptagof(a5)) {
 case ATOMIC:
  if (a5 != makesym(atom_normal)) goto norm__pragma__pragma__arg_8_2;
  a5 = a6;
  a6 = a7;
  execute(norm__body__arg_7_0);
  goto norm__body__arg_7_ext_interrupt;
 case VARREF:
  deref_and_jump(a5,norm__pragma__pragma__arg_8_1);
  *reasonp++ =  a5;
  goto norm__pragma__pragma__arg_8_interrupt;
  };
  goto norm__pragma__pragma__arg_8_2;
 norm__pragma__pragma__arg_8_2:
  x0 = makesym(atom_normal);
  gblt_diff(a5,x0,norm__pragma__pragma__arg_8_interrupt);
  unify(a7, a6);
  unify_value(a4, a5);
  proceed();
 norm__pragma__pragma__arg_8_ext_interrupt:
  reasonp = 0l;
 norm__pragma__pragma__arg_8_interrupt:
  toppred = &predicate_klic__comp__normalize_xnorm__pragma__pragma__arg_8;
  goto interrupt_8;
 }

 norm__body__args_9_top: {
  q x0, x1, x2, x3, x4, x5;
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  a6 = qp->args[6];
  a7 = qp->args[7];
  a8 = qp->args[8];
  qp = qp->next;
 norm__body__args_9_clear_reason:
  reasonp = reasons;
 norm__body__args_9_0:
 norm__body__args_9_1:
  switch (ptagof(a0)) {
 case FUNCTOR:
  switch (symval(functor_of(a0))) {
 case functor_functor_1:
  x0 = arg(a0, 0);
 norm__body__args_9_2:
  if (isref(x0)) goto norm__body__args_9_3;
  gblt_pfunctor(x0,x1,norm__body__args_9_4);
  gblt_arity(x0,x2,norm__body__args_9_4);
  unify_value(a1, x1);
  unify_value(a2, x2);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__normalize_xnorm__body__args_7);
  allocp[2] = x3 = makeref(&allocp[2]);
  allocp[3] = a3;
  allocp[4] = a4;
  allocp[5] = a5;
  allocp[6] = a6;
  allocp[7] = a7;
  allocp[8] = a8;
  allocp[9] = x3;
  allocp[10] = x5 = makeref(&allocp[10]);
  x4 = makecons(&allocp[9]);
  allocp[11] = (q)(struct goalrec*)&allocp[0];
  allocp[12] = (q)(&predicate_klic__comp__util_xuniv_2);
  allocp[13] = x0;
  allocp[14] = x4;
  qp = (struct goalrec*)&allocp[11];
  allocp += 15;
  proceed();
 norm__body__args_9_3:
  deref_and_jump(x0,norm__body__args_9_2);
  *reasonp++ =  x0;
  goto norm__body__args_9_4;
 case functor_atom_1:
  unify(a8, a7);
  x0 = arg(a0, 0);
  unify(a1, x0);
  x1 = makeint(0L);
  unify_value(a2, x1);
  x2 = NILATOM;
  unify_value(a3, x2);
  unify(a5, a4);
  x3 = makesym(atom_normal);
  unify_value(a6, x3);
  proceed();
  };
  goto norm__body__args_9_4;
 case VARREF:
  deref_and_jump(a0,norm__body__args_9_1);
  *reasonp++ =  a0;
  goto norm__body__args_9_4;
  };
  goto norm__body__args_9_4;
 norm__body__args_9_4:
  otherwise(norm__body__args_9_interrupt);
  x0 = makesym(atom_abnormal);
  unify_value(a6, x0);
  allocp[0] = NILATOM;
  allocp[1] = a0;
  x1 = makecons(&allocp[0]);
  allocp[2] = (q)qp;
  allocp[3] = (q)(&predicate_klic__comp__message_xreport__error_4);
  allocp[4] = makefunctor(&string_const_4);
  allocp[5] = x1;
  allocp[6] = a7;
  allocp[7] = a8;
  qp = (struct goalrec*)&allocp[2];
  allocp += 8;
  proceed();
 norm__body__args_9_ext_interrupt:
  reasonp = 0l;
 norm__body__args_9_interrupt:
  toppred = &predicate_klic__comp__normalize_xnorm__body__args_9;
  goto interrupt_9;
 }

 norm__body__args_7_top: {
  q x0, x1, x2, x3, x4, x5, x6;
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  a6 = qp->args[6];
  qp = qp->next;
 norm__body__args_7_clear_reason:
  reasonp = reasons;
 norm__body__args_7_0:
 norm__body__args_7_1:
  switch (ptagof(a0)) {
 case CONS:
  allocp[0] = x1 = makeref(&allocp[0]);
  allocp[1] = x2 = makeref(&allocp[1]);
  x0 = makecons(&allocp[0]);
  allocp += 2;
  unify_value(a1, x0);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__normalize_xnorm__body__args__1_8);
  x3 = cdr_of(a0);
  allocp[2] = x3;
  allocp[3] = x1;
  allocp[4] = x4 = makeref(&allocp[4]);
  allocp[5] = a3;
  allocp[6] = a4;
  allocp[7] = x5 = makeref(&allocp[7]);
  allocp[8] = x6 = makeref(&allocp[8]);
  allocp[9] = a6;
  a0 = car_of(a0);
  a1 = x2;
  a3 = x4;
  a4 = x5;
  a6 = x6;
  qp = (struct goalrec*)&allocp[0];
  allocp += 10;
  execute(norm__body__arg_7_0);
  goto norm__body__arg_7_ext_interrupt;
 case ATOMIC:
  if (a0 != NILATOM) goto norm__body__args_7_interrupt;
  unify(a6, a5);
  x0 = NILATOM;
  unify_value(a1, x0);
  unify(a3, a2);
  x1 = makesym(atom_normal);
  unify_value(a4, x1);
  proceed();
 case VARREF:
  deref_and_jump(a0,norm__body__args_7_1);
  *reasonp++ =  a0;
  goto norm__body__args_7_interrupt;
  };
  goto norm__body__args_7_interrupt;
 norm__body__args_7_ext_interrupt:
  reasonp = 0l;
 norm__body__args_7_interrupt:
  toppred = &predicate_klic__comp__normalize_xnorm__body__args_7;
  goto interrupt_7;
 }

 norm__body__args__1_8_top: {
  q x0;
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  a6 = qp->args[6];
  a7 = qp->args[7];
  qp = qp->next;
 norm__body__args__1_8_clear_reason:
  reasonp = reasons;
 norm__body__args__1_8_0:
 norm__body__args__1_8_1:
  switch (ptagof(a5)) {
 case ATOMIC:
  if (a5 != makesym(atom_normal)) goto norm__body__args__1_8_2;
  a5 = a6;
  a6 = a7;
  execute(norm__body__args_7_0);
  goto norm__body__args_7_ext_interrupt;
 case VARREF:
  deref_and_jump(a5,norm__body__args__1_8_1);
  *reasonp++ =  a5;
  goto norm__body__args__1_8_interrupt;
  };
  goto norm__body__args__1_8_2;
 norm__body__args__1_8_2:
  x0 = makesym(atom_normal);
  gblt_diff(a5,x0,norm__body__args__1_8_interrupt);
  unify(a7, a6);
  unify_value(a4, a5);
  proceed();
 norm__body__args__1_8_ext_interrupt:
  reasonp = 0l;
 norm__body__args__1_8_interrupt:
  goto interrupt_8;
 }

 norm__body__arg_7_top: {
  q x0, x1;
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  a6 = qp->args[6];
  qp = qp->next;
 norm__body__arg_7_clear_reason:
  reasonp = reasons;
 norm__body__arg_7_0:
 norm__body__arg_7_1:
  switch (ptagof(a0)) {
 case FUNCTOR:
  if (functor_of(a0) != makesym(functor_variable_1)) goto norm__body__arg_7_2;
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__normalize_xnorm__body__arg__1_8);
  x0 = arg(a0, 0);
  allocp[2] = x0;
  allocp[3] = a1;
  allocp[4] = a2;
  allocp[5] = a3;
  allocp[6] = a4;
  allocp[7] = x1 = makeref(&allocp[7]);
  allocp[8] = a5;
  allocp[9] = a6;
  allocp[10] = (q)(struct goalrec*)&allocp[0];
  allocp[11] = (q)(&predicate_klic__comp__util_xassoc_3);
  allocp[12] = a2;
  allocp[13] = x0;
  allocp[14] = x1;
  qp = (struct goalrec*)&allocp[10];
  allocp += 15;
  proceed();
 case VARREF:
  deref_and_jump(a0,norm__body__arg_7_1);
  *reasonp++ =  a0;
  goto norm__body__arg_7_2;
  };
  goto norm__body__arg_7_2;
 norm__body__arg_7_2:
  otherwise(norm__body__arg_7_interrupt);
  execute(norm__body__arg__2_7_0);
  goto norm__body__arg__2_7_ext_interrupt;
 norm__body__arg_7_ext_interrupt:
  reasonp = 0l;
 norm__body__arg_7_interrupt:
  toppred = &predicate_klic__comp__normalize_xnorm__body__arg_7;
  goto interrupt_7;
 }

 norm__body__arg__1_8_top: {
  q x0, x1, x2, x3, x4;
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  a6 = qp->args[6];
  a7 = qp->args[7];
  qp = qp->next;
 norm__body__arg__1_8_clear_reason:
  reasonp = reasons;
 norm__body__arg__1_8_0:
 norm__body__arg__1_8_1:
  switch (ptagof(a5)) {
 case ATOMIC:
  if (a5 != makesym(atom_no)) goto norm__body__arg__1_8_3;
  unify(a7, a6);
  allocp[0] = makesym(functor_var_1);
  allocp[1] = a0;
  x0 = makefunctor(&allocp[0]);
  allocp += 2;
  unify_value(a1, x0);
  allocp[0] = makesym(functor_var_1);
  allocp[1] = a0;
  x1 = makefunctor(&allocp[0]);
  allocp[2] = makesym(functor__3D_2);
  allocp[3] = a0;
  allocp[4] = x1;
  x2 = makefunctor(&allocp[2]);
  allocp[5] = a2;
  allocp[6] = x2;
  x3 = makecons(&allocp[5]);
  allocp += 7;
  unify_value(a3, x3);
  x4 = makesym(atom_normal);
  unify_value(a4, x4);
  proceed();
 case FUNCTOR:
  if (functor_of(a5) != makesym(functor_yes_1)) goto norm__body__arg__1_8_3;
  x0 = arg(a5, 0);
 norm__body__arg__1_8_2:
  switch (ptagof(x0)) {
 case FUNCTOR:
  if (functor_of(x0) != makesym(functor_rename_1)) goto norm__body__arg__1_8_3;
  allocp[0] = makesym(functor_variable_1);
  x2 = arg(x0, 0);
  allocp[1] = x2;
  x1 = makefunctor(&allocp[0]);
  a0 = x1;
  a5 = a6;
  a6 = a7;
  allocp += 2;
  execute(norm__body__arg_7_0);
  goto norm__body__arg_7_ext_interrupt;
 case VARREF:
  deref_and_jump(x0,norm__body__arg__1_8_2);
  *reasonp++ =  x0;
  goto norm__body__arg__1_8_3;
  };
  goto norm__body__arg__1_8_3;
 case VARREF:
  deref_and_jump(a5,norm__body__arg__1_8_1);
  *reasonp++ =  a5;
  goto norm__body__arg__1_8_3;
  };
  goto norm__body__arg__1_8_3;
 norm__body__arg__1_8_3:
  otherwise(norm__body__arg__1_8_interrupt);
 norm__body__arg__1_8_4:
  switch (ptagof(a5)) {
 case FUNCTOR:
  if (functor_of(a5) != makesym(functor_yes_1)) goto norm__body__arg__1_8_interrupt;
  unify(a7, a6);
  x0 = arg(a5, 0);
  unify2(a1, x0, a3, a2);
  x1 = makesym(atom_normal);
  unify_value(a4, x1);
  proceed();
 case VARREF:
  deref_and_jump(a5,norm__body__arg__1_8_4);
  *reasonp++ =  a5;
  goto norm__body__arg__1_8_interrupt;
  };
  goto norm__body__arg__1_8_interrupt;
 norm__body__arg__1_8_ext_interrupt:
  reasonp = 0l;
 norm__body__arg__1_8_interrupt:
  goto interrupt_8;
 }

 norm__body__arg__2_7_top: {
  q x0, x1, x2, x3, x4, x5, x6, x7;
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  a6 = qp->args[6];
  qp = qp->next;
 norm__body__arg__2_7_clear_reason:
  reasonp = reasons;
 norm__body__arg__2_7_0:
 norm__body__arg__2_7_1:
  switch (ptagof(a0)) {
 case FUNCTOR:
  if (functor_of(a0) != makesym(functor_variable_1)) goto norm__body__arg__2_7_2;
  unify(a6, a5);
  allocp[0] = makesym(functor_var_1);
  x1 = arg(a0, 0);
  allocp[1] = x1;
  x0 = makefunctor(&allocp[0]);
  allocp += 2;
  unify_value(a1, x0);
  allocp[0] = makesym(functor_var_1);
  x3 = arg(a0, 0);
  allocp[1] = x3;
  x2 = makefunctor(&allocp[0]);
  allocp[2] = makesym(functor__3D_2);
  x5 = arg(a0, 0);
  allocp[3] = x5;
  allocp[4] = x2;
  x4 = makefunctor(&allocp[2]);
  allocp[5] = a2;
  allocp[6] = x4;
  x6 = makecons(&allocp[5]);
  allocp += 7;
  unify_value(a3, x6);
  x7 = makesym(atom_normal);
  unify_value(a4, x7);
  proceed();
 case VARREF:
  deref_and_jump(a0,norm__body__arg__2_7_1);
  *reasonp++ =  a0;
  goto norm__body__arg__2_7_2;
  };
  goto norm__body__arg__2_7_2;
 norm__body__arg__2_7_2:
  otherwise(norm__body__arg__2_7_interrupt);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__normalize_xnorm__body__arg__3_8);
  allocp[2] = a0;
  allocp[3] = a1;
  allocp[4] = a2;
  allocp[5] = a3;
  allocp[6] = a4;
  allocp[7] = x0 = makeref(&allocp[7]);
  allocp[8] = a5;
  allocp[9] = a6;
  allocp[10] = x1 = makeref(&allocp[10]);
  a2 = x1;
  a3 = makesym(atom_yes);
  a4 = x0;
  qp = (struct goalrec*)&allocp[0];
  allocp += 11;
  execute(is__const_5_0);
  goto is__const_5_ext_interrupt;
 norm__body__arg__2_7_ext_interrupt:
  reasonp = 0l;
 norm__body__arg__2_7_interrupt:
  toppred = &predicate_klic__comp__normalize_xnorm__body__arg__2_7;
  goto interrupt_7;
 }

 norm__body__arg__3_8_top: {
  q x0, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10;
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  a6 = qp->args[6];
  a7 = qp->args[7];
  qp = qp->next;
 norm__body__arg__3_8_clear_reason:
  reasonp = reasons;
 norm__body__arg__3_8_0:
 norm__body__arg__3_8_1:
  switch (ptagof(a5)) {
 case ATOMIC:
  if (isint(a5)) goto norm__body__arg__3_8_interrupt;
  switch (symval(a5)) {
 case symval(makesym(atom_yes)):
  unify2(a7, a6, a3, a2);
  x0 = makesym(atom_normal);
  unify_value(a4, x0);
  proceed();
 case symval(makesym(atom_no)):
 norm__body__arg__3_8_2:
  switch (ptagof(a0)) {
 case FUNCTOR:
  switch (symval(functor_of(a0))) {
 case functor_list_1:
  x0 = arg(a0, 0);
 norm__body__arg__3_8_3:
  switch (ptagof(x0)) {
 case CONS:
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__normalize_xnorm__body__arg__3__1_10);
  x1 = cdr_of(x0);
  allocp[2] = x1;
  allocp[3] = x2 = makeref(&allocp[3]);
  allocp[4] = x3 = makeref(&allocp[4]);
  allocp[5] = x4 = makeref(&allocp[5]);
  allocp[6] = a3;
  allocp[7] = a4;
  allocp[8] = a1;
  allocp[9] = x5 = makeref(&allocp[9]);
  allocp[10] = x6 = makeref(&allocp[10]);
  allocp[11] = a7;
  a0 = car_of(x0);
  a1 = x2;
  a3 = x4;
  a4 = x5;
  a5 = a6;
  a6 = x6;
  qp = (struct goalrec*)&allocp[0];
  allocp += 12;
  execute(norm__body__arg_7_0);
  goto norm__body__arg_7_ext_interrupt;
 case VARREF:
  deref_and_jump(x0,norm__body__arg__3_8_3);
  *reasonp++ =  x0;
  goto norm__body__arg__3_8_interrupt;
  };
  goto norm__body__arg__3_8_interrupt;
 case functor_functor_1:
  x0 = arg(a0, 0);
 norm__body__arg__3_8_4:
  if (isref(x0)) goto norm__body__arg__3_8_5;
  gblt_pfunctor(x0,x1,norm__body__arg__3_8_interrupt);
  gblt_arity(x0,x2,norm__body__arg__3_8_interrupt);
  allocp[0] = makesym(functor__2F_2);
  allocp[1] = x1;
  allocp[2] = x2;
  x3 = makefunctor(&allocp[0]);
  allocp[3] = makesym(functor_mkfunct_2);
  allocp[4] = x3;
  allocp[5] = x5 = makeref(&allocp[5]);
  x4 = makefunctor(&allocp[3]);
  allocp[6] = (q)qp;
  allocp[7] = (q)(&predicate_klic__comp__normalize_xnorm__body__arg__3__2_6);
  allocp[8] = a1;
  allocp[9] = x4;
  allocp[10] = a4;
  allocp[11] = x6 = makeref(&allocp[11]);
  allocp[12] = x7 = makeref(&allocp[12]);
  allocp[13] = a7;
  allocp[14] = (q)(struct goalrec*)&allocp[6];
  allocp[15] = (q)(&predicate_klic__comp__normalize_xnorm__body__args_7);
  allocp[16] = x8 = makeref(&allocp[16]);
  allocp[17] = x5;
  allocp[18] = a2;
  allocp[19] = a3;
  allocp[20] = x6;
  allocp[21] = a6;
  allocp[22] = x7;
  allocp[23] = x8;
  allocp[24] = x10 = makeref(&allocp[24]);
  x9 = makecons(&allocp[23]);
  allocp[25] = (q)(struct goalrec*)&allocp[14];
  allocp[26] = (q)(&predicate_klic__comp__util_xuniv_2);
  allocp[27] = x0;
  allocp[28] = x9;
  qp = (struct goalrec*)&allocp[25];
  allocp += 29;
  proceed();
 norm__body__arg__3_8_5:
  deref_and_jump(x0,norm__body__arg__3_8_4);
  *reasonp++ =  x0;
  goto norm__body__arg__3_8_interrupt;
 case functor_vector_1:
  x0 = arg(a0, 0);
 norm__body__arg__3_8_6:
  switch (ptagof(x0)) {
 case FUNCTOR:
 norm__body__arg__3_8_7:
  if (!isgobj(x0)) goto norm__body__arg__3_8_interrupt;
  if (!isclass(x0,vector)) goto norm__body__arg__3_8_interrupt;
  gblt_size_of_vector(x0,x1,norm__body__arg__3_8_interrupt);
  allocp[0] = makesym(functor_mkvect_2);
  allocp[1] = x1;
  allocp[2] = x3 = makeref(&allocp[2]);
  x2 = makefunctor(&allocp[0]);
  allocp[3] = (q)qp;
  allocp[4] = (q)(&predicate_klic__comp__normalize_xnorm__body__arg__3__2_6);
  allocp[5] = a1;
  allocp[6] = x2;
  allocp[7] = a4;
  allocp[8] = x4 = makeref(&allocp[8]);
  allocp[9] = x5 = makeref(&allocp[9]);
  allocp[10] = a7;
  allocp[11] = (q)(struct goalrec*)&allocp[3];
  allocp[12] = (q)(&predicate_klic__comp__normalize_xnorm__body__args_7);
  allocp[13] = x6 = makeref(&allocp[13]);
  allocp[14] = x3;
  allocp[15] = a2;
  allocp[16] = a3;
  allocp[17] = x4;
  allocp[18] = a6;
  allocp[19] = x5;
  allocp[20] = (q)(struct goalrec*)&allocp[11];
  allocp[21] = (q)(&predicate_klic__comp__util_xvector__to__list_2);
  allocp[22] = x0;
  allocp[23] = x6;
  qp = (struct goalrec*)&allocp[20];
  allocp += 24;
  proceed();
 case VARREF:
  deref_and_jump(x0,norm__body__arg__3_8_6);
  *reasonp++ =  x0;
  goto norm__body__arg__3_8_interrupt;
  };
  goto norm__body__arg__3_8_interrupt;
  };
  goto norm__body__arg__3_8_interrupt;
 case VARREF:
  deref_and_jump(a0,norm__body__arg__3_8_2);
  *reasonp++ =  a0;
  goto norm__body__arg__3_8_interrupt;
  };
  goto norm__body__arg__3_8_interrupt;
  };
  goto norm__body__arg__3_8_interrupt;
 case VARREF:
  deref_and_jump(a5,norm__body__arg__3_8_1);
  *reasonp++ =  a5;
  goto norm__body__arg__3_8_interrupt;
  };
  goto norm__body__arg__3_8_interrupt;
 norm__body__arg__3_8_ext_interrupt:
  reasonp = 0l;
 norm__body__arg__3_8_interrupt:
  goto interrupt_8;
 }

 norm__body__arg__3__1_10_top: {
  q x0, x1, x2;
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  a6 = qp->args[6];
  a7 = qp->args[7];
  a8 = qp->args[8];
  a9 = qp->args[9];
  qp = qp->next;
 norm__body__arg__3__1_10_clear_reason:
  reasonp = reasons;
 norm__body__arg__3__1_10_0:
 norm__body__arg__3__1_10_1:
  switch (ptagof(a7)) {
 case ATOMIC:
  if (a7 != makesym(atom_normal)) goto norm__body__arg__3__1_10_2;
  allocp[0] = makesym(functor_cons_2);
  allocp[1] = a1;
  allocp[2] = a2;
  x0 = makefunctor(&allocp[0]);
  allocp[3] = (q)qp;
  allocp[4] = (q)(&predicate_klic__comp__normalize_xnorm__body__arg__3__2_6);
  allocp[5] = a6;
  allocp[6] = x0;
  allocp[7] = a5;
  allocp[8] = x1 = makeref(&allocp[8]);
  allocp[9] = x2 = makeref(&allocp[9]);
  allocp[10] = a9;
  a1 = a2;
  a2 = a3;
  a3 = a4;
  a4 = x1;
  a5 = a8;
  a6 = x2;
  qp = (struct goalrec*)&allocp[3];
  allocp += 11;
  execute(norm__body__arg_7_0);
  goto norm__body__arg_7_ext_interrupt;
 case VARREF:
  deref_and_jump(a7,norm__body__arg__3__1_10_1);
  *reasonp++ =  a7;
  goto norm__body__arg__3__1_10_interrupt;
  };
  goto norm__body__arg__3__1_10_2;
 norm__body__arg__3__1_10_2:
  x0 = makesym(atom_normal);
  gblt_diff(a7,x0,norm__body__arg__3__1_10_interrupt);
  unify(a9, a8);
  unify_value(a5, a7);
  proceed();
 norm__body__arg__3__1_10_ext_interrupt:
  reasonp = 0l;
 norm__body__arg__3__1_10_interrupt:
  goto interrupt_10;
 }

 norm__body__arg__3__2_6_top: {
  q x0;
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  qp = qp->next;
 norm__body__arg__3__2_6_clear_reason:
  reasonp = reasons;
 norm__body__arg__3__2_6_0:
 norm__body__arg__3__2_6_1:
  switch (ptagof(a3)) {
 case ATOMIC:
  if (a3 != makesym(atom_normal)) goto norm__body__arg__3__2_6_2;
  unify2(a5, a4, a0, a1);
  unify_value(a2, a3);
  proceed();
 case VARREF:
  deref_and_jump(a3,norm__body__arg__3__2_6_1);
  *reasonp++ =  a3;
  goto norm__body__arg__3__2_6_interrupt;
  };
  goto norm__body__arg__3__2_6_2;
 norm__body__arg__3__2_6_2:
  x0 = makesym(atom_normal);
  gblt_diff(a3,x0,norm__body__arg__3__2_6_interrupt);
  unify(a5, a4);
  unify_value(a2, a3);
  proceed();
 norm__body__arg__3__2_6_ext_interrupt:
  reasonp = 0l;
 norm__body__arg__3__2_6_interrupt:
  goto interrupt_6;
 }

 is__const_5_top: {
  q x0, x1, x2, x3, x4, x5, x6, x7;
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  qp = qp->next;
 is__const_5_clear_reason:
  reasonp = reasons;
 is__const_5_0:
 is__const_5_1:
  switch (ptagof(a3)) {
 case ATOMIC:
  if (a3 != makesym(atom_yes)) goto is__const_5_4;
 is__const_5_2:
  switch (ptagof(a0)) {
 case FUNCTOR:
  switch (symval(functor_of(a0))) {
 case functor_atom_1:
  a3 = makesym(atom_atom);
  execute(yes__const_5_0);
  goto yes__const_5_ext_interrupt;
 case functor_integer_1:
  allocp[0] = makesym(functor_int_1);
  x1 = arg(a0, 0);
  allocp[1] = x1;
  x0 = makefunctor(&allocp[0]);
  a0 = x0;
  a3 = makesym(atom_int);
  allocp += 2;
  execute(yes__const_5_0);
  goto yes__const_5_ext_interrupt;
 case functor_float_1:
  a3 = makesym(atom_float);
  execute(yes__const_5_0);
  goto yes__const_5_ext_interrupt;
 case functor_predicate_3:
  a3 = makesym(atom_predicate);
  execute(yes__const_5_0);
  goto yes__const_5_ext_interrupt;
 case functor_list_1:
  x0 = arg(a0, 0);
 is__const_5_3:
  switch (ptagof(x0)) {
 case CONS:
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__normalize_xis__const__1_6);
  allocp[2] = a0;
  allocp[3] = a1;
  allocp[4] = a2;
  allocp[5] = x1 = makeref(&allocp[5]);
  allocp[6] = a4;
  allocp[7] = makesym(atom_list);
  allocp[8] = (q)(struct goalrec*)&allocp[0];
  allocp[9] = (q)(&predicate_klic__comp__normalize_xis__const_5);
  x2 = cdr_of(x0);
  allocp[10] = x2;
  allocp[11] = x3 = makeref(&allocp[11]);
  allocp[12] = x4 = makeref(&allocp[12]);
  allocp[13] = x5 = makeref(&allocp[13]);
  allocp[14] = x1;
  a0 = car_of(x0);
  allocp[15] = x6 = makeref(&allocp[15]);
  a1 = x6;
  allocp[16] = x7 = makeref(&allocp[16]);
  a2 = x7;
  a3 = makesym(atom_yes);
  a4 = x5;
  qp = (struct goalrec*)&allocp[8];
  allocp += 17;
  execute(is__const_5_0);
  goto is__const_5_ext_interrupt;
 case VARREF:
  deref_and_jump(x0,is__const_5_3);
  *reasonp++ =  x0;
  goto is__const_5_4;
  };
  goto is__const_5_4;
 case functor_functor_1:
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__normalize_xis__const__1_6);
  allocp[2] = a0;
  allocp[3] = a1;
  allocp[4] = a2;
  allocp[5] = x0 = makeref(&allocp[5]);
  allocp[6] = a4;
  allocp[7] = makesym(atom_functor);
  allocp[8] = (q)(struct goalrec*)&allocp[0];
  allocp[9] = (q)(&predicate_klic__comp__normalize_xis__const__args_3);
  allocp[10] = makesym(atom_yes);
  allocp[11] = x1 = makeref(&allocp[11]);
  allocp[12] = x0;
  allocp[13] = x1;
  allocp[14] = x3 = makeref(&allocp[14]);
  x2 = makecons(&allocp[13]);
  allocp[15] = (q)(struct goalrec*)&allocp[8];
  allocp[16] = (q)(&predicate_klic__comp__util_xuniv_2);
  x4 = arg(a0, 0);
  allocp[17] = x4;
  allocp[18] = x2;
  qp = (struct goalrec*)&allocp[15];
  allocp += 19;
  proceed();
 case functor_string_1:
  a3 = makesym(atom_string);
  execute(yes__const_5_0);
  goto yes__const_5_ext_interrupt;
 case functor_vector_1:
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__normalize_xis__const__1_6);
  allocp[2] = a0;
  allocp[3] = a1;
  allocp[4] = a2;
  allocp[5] = x0 = makeref(&allocp[5]);
  allocp[6] = a4;
  allocp[7] = makesym(atom_vector);
  allocp[8] = (q)(struct goalrec*)&allocp[0];
  allocp[9] = (q)(&predicate_klic__comp__normalize_xis__const__args_3);
  allocp[10] = makesym(atom_yes);
  allocp[11] = x1 = makeref(&allocp[11]);
  allocp[12] = x0;
  allocp[13] = (q)(struct goalrec*)&allocp[8];
  allocp[14] = (q)(&predicate_klic__comp__util_xvector__to__list_2);
  x2 = arg(a0, 0);
  allocp[15] = x2;
  allocp[16] = x1;
  qp = (struct goalrec*)&allocp[13];
  allocp += 17;
  proceed();
  };
  goto is__const_5_4;
 case VARREF:
  deref_and_jump(a0,is__const_5_2);
  *reasonp++ =  a0;
  goto is__const_5_4;
  };
  goto is__const_5_4;
 case VARREF:
  deref_and_jump(a3,is__const_5_1);
  *reasonp++ =  a3;
  goto is__const_5_4;
  };
  goto is__const_5_4;
 is__const_5_4:
  otherwise(is__const_5_interrupt);
  x0 = makesym(atom_no);
  unify_value(a4, x0);
  proceed();
 is__const_5_ext_interrupt:
  reasonp = 0l;
 is__const_5_interrupt:
  toppred = &predicate_klic__comp__normalize_xis__const_5;
  goto interrupt_5;
 }

 is__const__1_6_top: {
  q x0;
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  qp = qp->next;
 is__const__1_6_clear_reason:
  reasonp = reasons;
 is__const__1_6_0:
 is__const__1_6_1:
  switch (ptagof(a3)) {
 case ATOMIC:
  if (isint(a3)) goto is__const__1_6_interrupt;
  switch (symval(a3)) {
 case symval(makesym(atom_no)):
  x0 = makesym(atom_no);
  unify_value(a4, x0);
  proceed();
 case symval(makesym(atom_yes)):
  a3 = a5;
  execute(yes__const_5_0);
  goto yes__const_5_ext_interrupt;
  };
  goto is__const__1_6_interrupt;
 case VARREF:
  deref_and_jump(a3,is__const__1_6_1);
  *reasonp++ =  a3;
  goto is__const__1_6_interrupt;
  };
  goto is__const__1_6_interrupt;
 is__const__1_6_ext_interrupt:
  reasonp = 0l;
 is__const__1_6_interrupt:
  goto interrupt_6;
 }

 is__const__args_3_top: {
  q x0, x1, x2, x3, x4;
  a2 = qp->args[2];
  qp = qp->next;
 is__const__args_3_clear_reason:
  reasonp = reasons;
 is__const__args_3_0:
 is__const__args_3_1:
  switch (ptagof(a0)) {
 case ATOMIC:
  if (isint(a0)) goto is__const__args_3_interrupt;
  switch (symval(a0)) {
 case symval(makesym(atom_no)):
  unify_value(a2, a0);
  proceed();
 case symval(makesym(atom_yes)):
 is__const__args_3_2:
  switch (ptagof(a1)) {
 case CONS:
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__normalize_xis__const__args_3);
  allocp[2] = x0 = makeref(&allocp[2]);
  x1 = cdr_of(a1);
  allocp[3] = x1;
  allocp[4] = a2;
  x2 = car_of(a1);
  allocp[5] = x3 = makeref(&allocp[5]);
  a1 = x3;
  allocp[6] = x4 = makeref(&allocp[6]);
  a2 = x4;
  a3 = a0;
  a4 = x0;
  a0 = x2;
  qp = (struct goalrec*)&allocp[0];
  allocp += 7;
  execute(is__const_5_0);
  goto is__const_5_ext_interrupt;
 case ATOMIC:
  if (a1 != NILATOM) goto is__const__args_3_interrupt;
  unify_value(a2, a0);
  proceed();
 case VARREF:
  deref_and_jump(a1,is__const__args_3_2);
  *reasonp++ =  a1;
  goto is__const__args_3_interrupt;
  };
  goto is__const__args_3_interrupt;
  };
  goto is__const__args_3_interrupt;
 case VARREF:
  deref_and_jump(a0,is__const__args_3_1);
  *reasonp++ =  a0;
  goto is__const__args_3_interrupt;
  };
  goto is__const__args_3_interrupt;
 is__const__args_3_ext_interrupt:
  reasonp = 0l;
 is__const__args_3_interrupt:
  goto interrupt_3;
 }

 yes__const_5_top: {
  q x0;
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  qp = qp->next;
 yes__const_5_clear_reason:
  reasonp = reasons;
 yes__const_5_0:
  unify2(a1, a0, a2, a3);
  x0 = makesym(atom_yes);
  unify_value(a4, x0);
  proceed();
 yes__const_5_ext_interrupt:
  reasonp = 0l;
 yes__const_5_interrupt:
  toppred = &predicate_klic__comp__normalize_xyes__const_5;
  goto interrupt_5;
 }
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
  loop_within_module(module_klic__comp__normalize);
}
