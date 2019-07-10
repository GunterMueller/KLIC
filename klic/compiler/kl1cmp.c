/* Compiled by KLIC compiler version 3.003 (Mon Mar 29 19:42:59 JST 1999) */
#include <klic/klichdr.h>
#include "atom.h"
#include "funct.h"

module module_klic__comp__kl1cmp();
Const struct predicate predicate_klic__comp__kl1cmp_xklcmp_2 =
   { module_klic__comp__kl1cmp, 0, 2 };
Const struct predicate predicate_klic__comp__kl1cmp_xklcmp_2F2_240_4 =
   { module_klic__comp__kl1cmp, 1, 4 };
Const struct predicate predicate_klic__comp__kl1cmp_xklcmp1_4 =
   { module_klic__comp__kl1cmp, 2, 4 };
Const struct predicate predicate_klic__comp__kl1cmp_xcompile_4 =
   { module_klic__comp__kl1cmp, 3, 4 };
Const struct predicate predicate_klic__comp__kl1cmp_xcompile1_4 =
   { module_klic__comp__kl1cmp, 4, 4 };
Const struct predicate predicate_klic__comp__kl1cmp_xclose__streams_1 =
   { module_klic__comp__kl1cmp, 5, 1 };
Const struct predicate predicate_klic__comp__kl1cmp_xread__in_5 =
   { module_klic__comp__kl1cmp, 6, 5 };
Const struct predicate predicate_klic__comp__kl1cmp_xread__inline_7 =
   { module_klic__comp__kl1cmp, 7, 7 };
Const struct predicate predicate_klic__comp__kl1cmp_xread__all_6 =
   { module_klic__comp__kl1cmp, 8, 6 };
Const struct predicate predicate_klic__comp__kl1cmp_xread__all__1_6 =
   { module_klic__comp__kl1cmp, 9, 6 };
Const struct predicate predicate_klic__comp__kl1cmp_xread__one__module_7 =
   { module_klic__comp__kl1cmp, 10, 7 };
Const struct predicate predicate_klic__comp__kl1cmp_xcompile2_6 =
   { module_klic__comp__kl1cmp, 11, 6 };
Const struct predicate predicate_klic__comp__kl1cmp_xwrite__file__header_2 =
   { module_klic__comp__kl1cmp, 12, 2 };
Const struct predicate predicate_klic__comp__kl1cmp_xwrite__inline__header_3 =
   { module_klic__comp__kl1cmp, 13, 3 };
Const struct predicate predicate_klic__comp__kl1cmp_xcompile__modules_6 =
   { module_klic__comp__kl1cmp, 14, 6 };
Const struct predicate predicate_klic__comp__kl1cmp_xwait__output__end_3 =
   { module_klic__comp__kl1cmp, 15, 3 };
Const struct predicate predicate_klic__comp__kl1cmp_xanalyze_2 =
   { module_klic__comp__kl1cmp, 16, 2 };
Const struct predicate predicate_klic__comp__kl1cmp_xanalyze1_6 =
   { module_klic__comp__kl1cmp, 17, 6 };
Const struct predicate predicate_klic__comp__kl1cmp_xanalyze__preds_6 =
   { module_klic__comp__kl1cmp, 18, 6 };
Const struct predicate predicate_klic__comp__kl1cmp_xget__clause__name_3 =
   { module_klic__comp__kl1cmp, 19, 3 };
Const struct predicate predicate_klic__comp__kl1cmp_xcollect__same_8 =
   { module_klic__comp__kl1cmp, 20, 8 };
Const struct predicate predicate_klic__comp__kl1cmp_xcollect__same__1_10 =
   { module_klic__comp__kl1cmp, 21, 10 };
Const struct predicate predicate_klic__comp__kl1cmp_xrename__macro__preds_5 =
   { module_klic__comp__kl1cmp, 22, 5 };
Const struct predicate predicate_klic__comp__kl1cmp_xrename__macro__clauses_3 =
   { module_klic__comp__kl1cmp, 23, 3 };
Const struct predicate predicate_klic__comp__kl1cmp_xshift__vector_2 =
   { module_klic__comp__kl1cmp, 24, 2 };
Const struct predicate predicate_klic__comp__kl1cmp_xshift__vector_3 =
   { module_klic__comp__kl1cmp, 25, 3 };
Const struct predicate predicate_klic__comp__kl1cmp_xshift__vector_5 =
   { module_klic__comp__kl1cmp, 26, 5 };
Const struct predicate predicate_klic__comp__kl1cmp_xhandle__exception_1 =
   { module_klic__comp__kl1cmp, 27, 1 };
Const struct predicate predicate_klic__comp__kl1cmp_xhandle__exception_3 =
   { module_klic__comp__kl1cmp, 28, 3 };
Const struct predicate predicate_klic__comp__kl1cmp_xcomplain__tokens_6 =
   { module_klic__comp__kl1cmp, 29, 6 };
Const struct predicate predicate_klic__comp__kl1cmp_xcomplain__tokens_3 =
   { module_klic__comp__kl1cmp, 30, 3 };
Const struct predicate predicate_klic__comp__kl1cmp_xcomplain__token_3 =
   { module_klic__comp__kl1cmp, 31, 3 };
Const struct predicate predicate_klic__comp__kl1cmp_xcomplain__string_3 =
   { module_klic__comp__kl1cmp, 32, 3 };
Const struct predicate predicate_klic__comp__kl1cmp_xcomplain__words_3 =
   { module_klic__comp__kl1cmp, 33, 3 };
extern Const struct predicate predicate_integer__arithmetics_xadd_3;
extern Const struct predicate predicate_functor__table_xsetarg_5;
extern Const struct predicate predicate_functor__table_xnew__functor_3;
extern Const struct predicate predicate_klic__comp__message_xreport__error_2;
extern Const struct predicate predicate_klic__comp__message_xsee_2;
extern Const struct predicate predicate_klic__comp__message_xtell_2;
extern Const struct predicate predicate_klic__comp__message_xcompilation__error_1;
extern Const struct predicate predicate_keyed__set_xcreate_1;
extern Const struct predicate predicate_klic__comp__cmacro_xinsDefs_3;
extern Const struct predicate predicate_klic__comp__cmacro_xreplaceClause_4;
extern Const struct predicate predicate_klic__comp__macro_xmacro_2;
extern Const struct predicate predicate_klic__comp__version_xcompiler__version_1;
extern Const struct predicate predicate_klic__comp__version_xcompiler__date_1;
extern Const struct predicate predicate_klic__comp__obj_xflush_3;
extern Const struct predicate predicate_klic__comp__obj_xklicformat_3;
extern Const struct predicate predicate_klic__comp__obj_xklicformat_4;
extern Const struct predicate predicate_klic__comp__obj_xwrite__inline_6;
extern Const struct predicate predicate_klic__comp__write_xwrite__out_3;
extern Const struct predicate predicate_klic__comp__write_xwrite__header__file_3;
extern Const struct predicate predicate_keyed__sorted__set_xcreate_1;
extern Const struct predicate predicate_klic__comp__util_xname_2;
extern Const struct predicate predicate_klic__comp__util_xappend_3;
extern Const struct predicate predicate_klic__comp__util_xremove_4;
extern Const struct predicate predicate_klic__comp__util_xmake__atom_2;
extern Const struct predicate predicate_klic__comp__util_xstring__to__char__list_2;
extern Const struct predicate predicate_klic__comp__normalize_xgenerate_8;
declare_method_table_of(vector);

module module_klic__comp__kl1cmp(glbl, qp, allocp, toppred)
  struct global_variables *glbl;
  struct goalrec *qp;
  register q *allocp;
  Const struct predicate *toppred;
{
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_0 =
      declare_string_constant_8(0,0);
  static Const unsigned char strconst_body_1[16] =
    "\n*** Here ***\n";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_1 =
      declare_string_constant_8(strconst_body_1,14);
  static Const unsigned char strconst_body_2[40] =
    "!! Error !!\nTL = ~w\nReason = ~wPs = \n";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_2 =
      declare_string_constant_8(strconst_body_2,37);
  static Const unsigned char strconst_body_3[4] =
    ".\n";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_3 =
      declare_string_constant_8(strconst_body_3,2);
  static Const unsigned char strconst_body_4[4] =
    ".c";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_4 =
      declare_string_constant_8(strconst_body_4,2);
  static Const unsigned char strconst_body_5[4] =
    {'.','e','x','t',};
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_5 =
      declare_string_constant_8(strconst_body_5,4);
  static Const unsigned char strconst_body_6[4] =
    {'.','k','l','1',};
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_6 =
      declare_string_constant_8(strconst_body_6,4);
  static Const unsigned char strconst_body_7[104] =
    "/* Compiled by KLIC compiler ~w (~w) */\n#include <klic/klichdr.h>\n#include \"atom.h\"\n#include \"funct.h\"\n";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_7 =
      declare_string_constant_8(strconst_body_7,103);
  static Const unsigned char strconst_body_8[40] =
    "File name extension of ~s is not kl1\n";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_8 =
      declare_string_constant_8(strconst_body_8,37);
  static Const unsigned char strconst_body_9[28] =
    "Internal error in analyze ";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_9 =
      declare_string_constant_8(strconst_body_9,26);
  static Const unsigned char strconst_body_10[28] =
    "Internal error in analyze1 ";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_10 =
      declare_string_constant_8(strconst_body_10,27);
  static Const unsigned char strconst_body_11[32] =
    "Module declaration is not found";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_11 =
      declare_string_constant_8(strconst_body_11,31);
  static Const unsigned char strconst_body_12[32] =
    "Module name is not an atom :~w";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_12 =
      declare_string_constant_8(strconst_body_12,30);
  static Const unsigned char strconst_body_13[16] =
    "Syntax error: ";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_13 =
      declare_string_constant_8(strconst_body_13,14);
  static Const unsigned char strconst_body_14[4] =
    "\\\"";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_14 =
      declare_string_constant_8(strconst_body_14,2);
  static Const unsigned char strconst_body_15[4] =
    "~c";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_15 =
      declare_string_constant_8(strconst_body_15,2);
  static Const unsigned char strconst_body_16[4] =
    "~s";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_16 =
      declare_string_constant_8(strconst_body_16,2);
  static Const unsigned char strconst_body_17[4] =
    "~w";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_17 =
      declare_string_constant_8(strconst_body_17,2);
  static Const unsigned char strconst_body_18[4] =
    "~w\n";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_18 =
      declare_string_constant_8(strconst_body_18,3);
  static Const unsigned char strconst_body_19[4] =
    "~w(";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_19 =
      declare_string_constant_8(strconst_body_19,3);
  static Const unsigned char strconst_body_20[4] =
    {'~','w','~','c',};
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_20 =
      declare_string_constant_8(strconst_body_20,4);
  static Const q cons_const_21[] = {
    NILATOM,
    makeint(32),
  };
  static Const q cons_const_22[] = {
    NILATOM,
    makesym(atom_found),
  };
  static Const q cons_const_23[] = {
    makecons(cons_const_22),
    makesym(atom_was),
  };
  static Const q cons_const_24[] = {
    makecons(cons_const_23),
    makesym(atom_token),
  };
  static Const q cons_const_25[] = {
    makecons(cons_const_24),
    makesym(atom_ambiguous),
  };
  q a0, a1, a2, a3, a4, a5, a6, a7, a8, a9;

  q *reasonp;
 module_top:
  a0 = qp->args[0];
  switch_on_pred() {
    case_pred(0, klcmp_2_top);
    case_pred(1, klcmp_2F2_240_4_top);
    case_pred(2, klcmp1_4_top);
    case_pred(3, compile_4_top);
    case_pred(4, compile1_4_top);
    case_pred(5, close__streams_1_top);
    case_pred(6, read__in_5_top);
    case_pred(7, read__inline_7_top);
    case_pred(8, read__all_6_top);
    case_pred(9, read__all__1_6_top);
    case_pred(10, read__one__module_7_top);
    case_pred(11, compile2_6_top);
    case_pred(12, write__file__header_2_top);
    case_pred(13, write__inline__header_3_top);
    case_pred(14, compile__modules_6_top);
    case_pred(15, wait__output__end_3_top);
    case_pred(16, analyze_2_top);
    case_pred(17, analyze1_6_top);
    case_pred(18, analyze__preds_6_top);
    case_pred(19, get__clause__name_3_top);
    case_pred(20, collect__same_8_top);
    case_pred(21, collect__same__1_10_top);
    case_pred(22, rename__macro__preds_5_top);
    case_pred(23, rename__macro__clauses_3_top);
    case_pred(24, shift__vector_2_top);
    case_pred(25, shift__vector_3_top);
    case_pred(26, shift__vector_5_top);
    case_pred(27, handle__exception_1_top);
    case_pred(28, handle__exception_3_top);
    case_pred(29, complain__tokens_6_top);
    case_pred(30, complain__tokens_3_top);
    case_pred(31, complain__token_3_top);
    case_pred(32, complain__string_3_top);
    last_case_pred(33, complain__words_3_top);
  }

 klcmp_2_top: {
  q x0, x1, x2, x3, x4, x5, x6, x7, x8;
  a1 = qp->args[1];
  qp = qp->next;
 klcmp_2_clear_reason:
  reasonp = reasons;
 klcmp_2_0:
 klcmp_2_1:
  switch (ptagof(a0)) {
 case FUNCTOR:
 klcmp_2_2:
  if (!isgobj(a0)) goto klcmp_2_4;
 klcmp_2_3:
  gblt_is_string(a0,klcmp_2_4);
  gblt_size_of_string(a0,x0,klcmp_2_4);
  gblt_element_size_of_string(a0,x1,klcmp_2_4);
  x2 = makeint(1L);
  bblt_sub_no_check(x0,x2,x3);
  generic_arg[0] = x3;
  x4 = makeint(1L);
  generic_arg[1] = x4;
  x5 = makeint(46L);
  generic_arg[2] = x5;
  allocp[0] = x6 = makeref(&allocp[0]);
  generic_arg[3] = x6;
  call_generic(a0, functor_search__character_4, 1);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__kl1cmp_xklcmp1_4);
  allocp[2] = a0;
  allocp[3] = x7 = makeref(&allocp[3]);
  allocp[4] = x8 = makeref(&allocp[4]);
  allocp[5] = a1;
  a1 = a0;
  a2 = x7;
  a3 = x8;
  a0 = x6;
  qp = (struct goalrec*)&allocp[0];
  allocp += 6;
  execute(klcmp_2F2_240_4_0);
  goto klcmp_2F2_240_4_ext_interrupt;
 case VARREF:
  deref_and_jump(a0,klcmp_2_1);
  *reasonp++ =  a0;
  goto klcmp_2_4;
  };
  goto klcmp_2_4;
 klcmp_2_4:
  otherwise(klcmp_2_interrupt);
  allocp[0] = makesym(functor_abnormal_2);
  allocp[1] = makesym(atom_inputfile);
  allocp[2] = a0;
  x0 = makefunctor(&allocp[0]);
  allocp += 3;
  unify_value(a1, x0);
  proceed();
 klcmp_2_ext_interrupt:
  reasonp = 0l;
 klcmp_2_interrupt:
  goto interrupt_2;
 }

 klcmp_2F2_240_4_top: {
  q x0, x1, x2;
  a1 = qp->args[1];
  a2 = qp->args[2];
  a3 = qp->args[3];
  qp = qp->next;
 klcmp_2F2_240_4_clear_reason:
  reasonp = reasons;
 klcmp_2F2_240_4_0:
 klcmp_2F2_240_4_1:
  if (!isint(a0)) goto klcmp_2F2_240_4_3;
  x0 = makeint(0L);
  gblt_greater(a0,x0,klcmp_2F2_240_4_2);
  generic_arg[0] = a0;
  generic_arg[1] = a2;
  generic_arg[2] = a3;
  call_generic(a1, functor_split_3, 0);
  proceed();
 klcmp_2F2_240_4_2:
  x1 = makeint(0L);
  gblt_less_or_eq(a0,x1,klcmp_2F2_240_4_interrupt);
  unify(a2, a1);
  x2 = makefunctor(&string_const_0);
  unify_value(a3, x2);
  proceed();
 klcmp_2F2_240_4_3:
  if (!isref(a0)) goto klcmp_2F2_240_4_interrupt;
  deref_and_jump(a0,klcmp_2F2_240_4_1);
  *reasonp++ =  a0;
  goto klcmp_2F2_240_4_interrupt;
 klcmp_2F2_240_4_ext_interrupt:
  reasonp = 0l;
 klcmp_2F2_240_4_interrupt:
  toppred = &predicate_klic__comp__kl1cmp_xklcmp_2F2_240_4;
  goto interrupt_4;
 }

 klcmp1_4_top: {
  q x0, x1, x2, x3, x4;
  a1 = qp->args[1];
  a2 = qp->args[2];
  a3 = qp->args[3];
  qp = qp->next;
 klcmp1_4_clear_reason:
  reasonp = reasons;
 klcmp1_4_0:
 klcmp1_4_1:
  if (isref(a2)) goto klcmp1_4_4;
 klcmp1_4_2:
  if (isref(a2)) goto klcmp1_4_3;
  x0 = makefunctor(&string_const_6);
  if_not_equal(a2, x0, klcmp1_4_5);
  x1 = makefunctor(&string_const_4);
  generic_arg[0] = x1;
  allocp[0] = x2 = makeref(&allocp[0]);
  generic_arg[1] = x2;
  call_generic(a1, functor_join_2, 1);
  x3 = makefunctor(&string_const_5);
  generic_arg[0] = x3;
  allocp[0] = x4 = makeref(&allocp[0]);
  generic_arg[1] = x4;
  call_generic(a1, functor_join_2, 1);
  a1 = x2;
  a2 = x4;
  execute(compile_4_clear_reason);
  goto compile_4_ext_interrupt;
 klcmp1_4_3:
  deref_and_jump(a2,klcmp1_4_2);
  *reasonp++ =  a2;
  goto klcmp1_4_5;
 klcmp1_4_4:
  deref_and_jump(a2,klcmp1_4_1);
  *reasonp++ =  a2;
  goto klcmp1_4_5;
 klcmp1_4_5:
  otherwise(klcmp1_4_interrupt);
  allocp[0] = makesym(functor_abnormal_2);
  allocp[1] = makesym(atom_inputfile);
  allocp[2] = a0;
  x0 = makefunctor(&allocp[0]);
  allocp += 3;
  unify_value(a3, x0);
  allocp[0] = NILATOM;
  allocp[1] = a0;
  x1 = makecons(&allocp[0]);
  allocp[2] = (q)qp;
  allocp[3] = (q)(&predicate_klic__comp__message_xreport__error_2);
  allocp[4] = makefunctor(&string_const_8);
  allocp[5] = x1;
  qp = (struct goalrec*)&allocp[2];
  allocp += 6;
  proceed();
 klcmp1_4_ext_interrupt:
  reasonp = 0l;
 klcmp1_4_interrupt:
  goto interrupt_4;
 }

 compile_4_top: {
  q x0, x1, x2;
  a1 = qp->args[1];
  a2 = qp->args[2];
  a3 = qp->args[3];
  qp = qp->next;
 compile_4_clear_reason:
  reasonp = reasons;
 compile_4_0:
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__kl1cmp_xcompile1_4);
  allocp[2] = x0 = makeref(&allocp[2]);
  allocp[3] = x1 = makeref(&allocp[3]);
  allocp[4] = x2 = makeref(&allocp[4]);
  allocp[5] = a3;
  allocp[6] = (q)(struct goalrec*)&allocp[0];
  allocp[7] = (q)(&predicate_klic__comp__message_xtell_2);
  allocp[8] = a2;
  allocp[9] = x2;
  allocp[10] = (q)(struct goalrec*)&allocp[6];
  allocp[11] = (q)(&predicate_klic__comp__message_xtell_2);
  allocp[12] = a1;
  allocp[13] = x1;
  allocp[14] = (q)(struct goalrec*)&allocp[10];
  allocp[15] = (q)(&predicate_klic__comp__message_xsee_2);
  allocp[16] = a0;
  allocp[17] = x0;
  qp = (struct goalrec*)&allocp[14];
  allocp += 18;
  proceed();
 compile_4_ext_interrupt:
  reasonp = 0l;
 compile_4_interrupt:
  toppred = &predicate_klic__comp__kl1cmp_xcompile_4;
  goto interrupt_4;
 }

 compile1_4_top: {
  q x0, x1, x2, x3, x4, x5;
  a1 = qp->args[1];
  a2 = qp->args[2];
  a3 = qp->args[3];
  qp = qp->next;
 compile1_4_clear_reason:
  reasonp = reasons;
 compile1_4_0:
 compile1_4_1:
  switch (ptagof(a0)) {
 case FUNCTOR:
  if (functor_of(a0) != makesym(functor_normal_1)) goto compile1_4_4;
 compile1_4_2:
  switch (ptagof(a1)) {
 case FUNCTOR:
  if (functor_of(a1) != makesym(functor_normal_1)) goto compile1_4_4;
 compile1_4_3:
  switch (ptagof(a2)) {
 case FUNCTOR:
  if (functor_of(a2) != makesym(functor_normal_1)) goto compile1_4_4;
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__kl1cmp_xcompile2_6);
  allocp[2] = x0 = makeref(&allocp[2]);
  allocp[3] = a3;
  x1 = arg(a1, 0);
  allocp[4] = x1;
  allocp[5] = NILATOM;
  x2 = arg(a2, 0);
  allocp[6] = x2;
  allocp[7] = NILATOM;
  allocp[8] = (q)(struct goalrec*)&allocp[0];
  allocp[9] = (q)(&predicate_klic__comp__kl1cmp_xread__in_5);
  allocp[10] = x0;
  x3 = arg(a0, 0);
  allocp[11] = x3;
  allocp[12] = NILATOM;
  allocp[13] = x4 = makeref(&allocp[13]);
  allocp[14] = NILATOM;
  allocp[15] = (q)(struct goalrec*)&allocp[8];
  allocp[16] = (q)(&predicate_keyed__set_xcreate_1);
  allocp[17] = x4;
  qp = (struct goalrec*)&allocp[15];
  allocp += 18;
  proceed();
 case VARREF:
  deref_and_jump(a2,compile1_4_3);
  *reasonp++ =  a2;
  goto compile1_4_4;
  };
  goto compile1_4_4;
 case VARREF:
  deref_and_jump(a1,compile1_4_2);
  *reasonp++ =  a1;
  goto compile1_4_4;
  };
  goto compile1_4_4;
 case VARREF:
  deref_and_jump(a0,compile1_4_1);
  *reasonp++ =  a0;
  goto compile1_4_4;
  };
  goto compile1_4_4;
 compile1_4_4:
  otherwise(compile1_4_interrupt);
  allocp[0] = makesym(functor__2C_2);
  allocp[1] = a1;
  allocp[2] = a2;
  x0 = makefunctor(&allocp[0]);
  allocp[3] = makesym(functor__2C_2);
  allocp[4] = a0;
  allocp[5] = x0;
  x1 = makefunctor(&allocp[3]);
  allocp[6] = makesym(functor_abnormal_2);
  allocp[7] = makesym(atom_fileopen);
  allocp[8] = x1;
  x2 = makefunctor(&allocp[6]);
  allocp += 9;
  unify_value(a3, x2);
  allocp[0] = NILATOM;
  allocp[1] = a2;
  x3 = makecons(&allocp[0]);
  allocp[2] = x3;
  allocp[3] = a1;
  x4 = makecons(&allocp[2]);
  allocp[4] = x4;
  allocp[5] = a0;
  x5 = makecons(&allocp[4]);
  a0 = x5;
  allocp += 6;
  execute(close__streams_1_0);
  goto close__streams_1_ext_interrupt;
 compile1_4_ext_interrupt:
  reasonp = 0l;
 compile1_4_interrupt:
  goto interrupt_4;
 }

 close__streams_1_top: {
  q x0, x1, x2;
  qp = qp->next;
 close__streams_1_clear_reason:
  reasonp = reasons;
 close__streams_1_0:
 close__streams_1_1:
  switch (ptagof(a0)) {
 case CONS:
  x0 = car_of(a0);
 close__streams_1_2:
  switch (ptagof(x0)) {
 case FUNCTOR:
  if (functor_of(x0) != makesym(functor_normal_1)) goto close__streams_1_3;
  x1 = arg(x0, 0);
  x2 = NILATOM;
  unify_value(x1, x2);
  a0 = cdr_of(a0);
  execute(close__streams_1_0);
  goto close__streams_1_ext_interrupt;
 case VARREF:
  deref_and_jump(x0,close__streams_1_2);
  *reasonp++ =  x0;
  goto close__streams_1_3;
  };
  goto close__streams_1_3;
 case ATOMIC:
  if (a0 != NILATOM) goto close__streams_1_3;
  proceed();
 case VARREF:
  deref_and_jump(a0,close__streams_1_1);
  *reasonp++ =  a0;
  goto close__streams_1_3;
  };
  goto close__streams_1_3;
 close__streams_1_3:
  otherwise(close__streams_1_interrupt);
 close__streams_1_4:
  switch (ptagof(a0)) {
 case CONS:
  a0 = cdr_of(a0);
  execute(close__streams_1_0);
  goto close__streams_1_ext_interrupt;
 case VARREF:
  deref_and_jump(a0,close__streams_1_4);
  *reasonp++ =  a0;
  goto close__streams_1_interrupt;
  };
  goto close__streams_1_interrupt;
 close__streams_1_ext_interrupt:
  reasonp = 0l;
 close__streams_1_interrupt:
  toppred = &predicate_klic__comp__kl1cmp_xclose__streams_1;
  goto interrupt_1;
 }

 read__in_5_top: {
  q x0, x1, x2, x3, x4, x5, x6, x7;
  a1 = qp->args[1];
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  qp = qp->next;
 read__in_5_clear_reason:
  reasonp = reasons;
 read__in_5_0:
  allocp[0] = makesym(functor_normal_2);
  allocp[1] = x1 = makeref(&allocp[1]);
  allocp[2] = x2 = makeref(&allocp[2]);
  x0 = makefunctor(&allocp[0]);
  allocp += 3;
  unify_value(a0, x0);
  allocp[0] = makesym(functor_getwt_1);
  allocp[1] = x4 = makeref(&allocp[1]);
  x3 = makefunctor(&allocp[0]);
  allocp[2] = x6 = makeref(&allocp[2]);
  allocp[3] = x3;
  x5 = makecons(&allocp[2]);
  allocp += 4;
  unify_value(a1, x5);
  a0 = x4;
  a1 = x2;
  x7 = a2;
  a5 = a3;
  a6 = a4;
  a4 = x7;
  a3 = x6;
  a2 = x1;
  execute(read__inline_7_0);
  goto read__inline_7_ext_interrupt;
 read__in_5_ext_interrupt:
  reasonp = 0l;
 read__in_5_interrupt:
  goto interrupt_5;
 }

 read__inline_7_top: {
  q x0, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12;
  a1 = qp->args[1];
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  a6 = qp->args[6];
  qp = qp->next;
 read__inline_7_clear_reason:
  reasonp = reasons;
 read__inline_7_0:
 read__inline_7_1:
  switch (ptagof(a0)) {
 case FUNCTOR:
  if (functor_of(a0) != makesym(functor_normal_1)) goto read__inline_7_8;
  x0 = arg(a0, 0);
 read__inline_7_2:
  switch (ptagof(x0)) {
 case FUNCTOR:
  if (functor_of(x0) != makesym(functor_functor_1)) goto read__inline_7_8;
  x1 = arg(x0, 0);
 read__inline_7_3:
  switch (ptagof(x1)) {
 case FUNCTOR:
  if (functor_of(x1) != makesym(functor__3A_2D_1)) goto read__inline_7_8;
  x2 = arg(x1, 0);
 read__inline_7_4:
  switch (ptagof(x2)) {
 case FUNCTOR:
  if (functor_of(x2) != makesym(functor_functor_1)) goto read__inline_7_8;
  x3 = arg(x2, 0);
 read__inline_7_5:
  switch (ptagof(x3)) {
 case FUNCTOR:
  if (functor_of(x3) != makesym(functor__3A_2)) goto read__inline_7_8;
  x4 = arg(x3, 0);
 read__inline_7_6:
  switch (ptagof(x4)) {
 case FUNCTOR:
  if (functor_of(x4) != makesym(functor_atom_1)) goto read__inline_7_8;
  x5 = arg(x4, 0);
 read__inline_7_7:
  switch (ptagof(x5)) {
 case ATOMIC:
  if (x5 != makesym(atom_inline)) goto read__inline_7_8;
  allocp[0] = x7 = makeref(&allocp[0]);
  x8 = arg(x3, 1);
  allocp[1] = x8;
  x6 = makecons(&allocp[0]);
  allocp += 2;
  unify_value(a1, x6);
  allocp[0] = makesym(functor_getwt_1);
  allocp[1] = x10 = makeref(&allocp[1]);
  x9 = makefunctor(&allocp[0]);
  allocp[2] = x12 = makeref(&allocp[2]);
  allocp[3] = x9;
  x11 = makecons(&allocp[2]);
  allocp += 4;
  unify_value(a3, x11);
  a0 = x10;
  a1 = x7;
  a3 = x12;
  execute(read__inline_7_0);
  goto read__inline_7_ext_interrupt;
 case VARREF:
  deref_and_jump(x5,read__inline_7_7);
  *reasonp++ =  x5;
  goto read__inline_7_8;
  };
  goto read__inline_7_8;
 case VARREF:
  deref_and_jump(x4,read__inline_7_6);
  *reasonp++ =  x4;
  goto read__inline_7_8;
  };
  goto read__inline_7_8;
 case VARREF:
  deref_and_jump(x3,read__inline_7_5);
  *reasonp++ =  x3;
  goto read__inline_7_8;
  };
  goto read__inline_7_8;
 case VARREF:
  deref_and_jump(x2,read__inline_7_4);
  *reasonp++ =  x2;
  goto read__inline_7_8;
  };
  goto read__inline_7_8;
 case VARREF:
  deref_and_jump(x1,read__inline_7_3);
  *reasonp++ =  x1;
  goto read__inline_7_8;
  };
  goto read__inline_7_8;
 case VARREF:
  deref_and_jump(x0,read__inline_7_2);
  *reasonp++ =  x0;
  goto read__inline_7_8;
  };
  goto read__inline_7_8;
 case VARREF:
  deref_and_jump(a0,read__inline_7_1);
  *reasonp++ =  a0;
  goto read__inline_7_8;
  };
  goto read__inline_7_8;
 read__inline_7_8:
  otherwise(read__inline_7_interrupt);
 read__inline_7_9:
  switch (ptagof(a0)) {
 case FUNCTOR:
  if (functor_of(a0) != makesym(functor_normal_1)) goto read__inline_7_10;
  x0 = NILATOM;
  unify_value(a1, x0);
  a0 = arg(a0, 0);
  a1 = a2;
  a2 = a3;
  a3 = a4;
  a4 = a5;
  a5 = a6;
  execute(read__all_6_0);
  goto read__all_6_ext_interrupt;
 case VARREF:
  deref_and_jump(a0,read__inline_7_9);
  *reasonp++ =  a0;
  goto read__inline_7_10;
  };
  goto read__inline_7_10;
 read__inline_7_10:
  otherwise(read__inline_7_interrupt);
  unify2(a6, a5, a4, a3);
  execute(handle__exception_1_0);
  goto handle__exception_1_ext_interrupt;
 read__inline_7_ext_interrupt:
  reasonp = 0l;
 read__inline_7_interrupt:
  toppred = &predicate_klic__comp__kl1cmp_xread__inline_7;
  goto interrupt_7;
 }

 read__all_6_top: {
  q x0, x1, x2;
  a1 = qp->args[1];
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  qp = qp->next;
 read__all_6_clear_reason:
  reasonp = reasons;
 read__all_6_0:
 read__all_6_1:
  switch (ptagof(a0)) {
 case ATOMIC:
  if (isint(a0)) goto read__all_6_5;
  switch (symval(a0)) {
 case symval(makesym(atom_end__of__file)):
  unify2(a5, a4, a3, a2);
  x0 = NILATOM;
  unify_value(a1, x0);
  proceed();
 case symval(makesym(atom_empty)):
  unify2(a5, a4, a3, a2);
  x0 = NILATOM;
  unify_value(a1, x0);
  proceed();
  };
  goto read__all_6_5;
 case FUNCTOR:
  if (functor_of(a0) != makesym(functor_functor_1)) goto read__all_6_5;
  x0 = arg(a0, 0);
 read__all_6_2:
  switch (ptagof(x0)) {
 case FUNCTOR:
  if (functor_of(x0) != makesym(functor__3A_2D_1)) goto read__all_6_5;
  x1 = arg(x0, 0);
 read__all_6_3:
  switch (ptagof(x1)) {
 case FUNCTOR:
  if (functor_of(x1) != makesym(functor_functor_1)) goto read__all_6_5;
  x2 = arg(x1, 0);
 read__all_6_4:
  switch (ptagof(x2)) {
 case FUNCTOR:
  if (functor_of(x2) != makesym(functor_module_1)) goto read__all_6_5;
  a0 = arg(x2, 0);
  execute(read__all__1_6_0);
  goto read__all__1_6_ext_interrupt;
 case VARREF:
  deref_and_jump(x2,read__all_6_4);
  *reasonp++ =  x2;
  goto read__all_6_5;
  };
  goto read__all_6_5;
 case VARREF:
  deref_and_jump(x1,read__all_6_3);
  *reasonp++ =  x1;
  goto read__all_6_5;
  };
  goto read__all_6_5;
 case VARREF:
  deref_and_jump(x0,read__all_6_2);
  *reasonp++ =  x0;
  goto read__all_6_5;
  };
  goto read__all_6_5;
 case VARREF:
  deref_and_jump(a0,read__all_6_1);
  *reasonp++ =  a0;
  goto read__all_6_5;
  };
  goto read__all_6_5;
 read__all_6_5:
  otherwise(read__all_6_interrupt);
  unify2(a5, a4, a3, a2);
  x0 = NILATOM;
  unify_value(a1, x0);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__message_xreport__error_2);
  allocp[2] = makefunctor(&string_const_11);
  allocp[3] = NILATOM;
  qp = (struct goalrec*)&allocp[0];
  allocp += 4;
  proceed();
 read__all_6_ext_interrupt:
  reasonp = 0l;
 read__all_6_interrupt:
  toppred = &predicate_klic__comp__kl1cmp_xread__all_6;
  goto interrupt_6;
 }

 read__all__1_6_top: {
  q x0, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11;
  a1 = qp->args[1];
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  qp = qp->next;
 read__all__1_6_clear_reason:
  reasonp = reasons;
 read__all__1_6_0:
 read__all__1_6_1:
  switch (ptagof(a0)) {
 case FUNCTOR:
  if (functor_of(a0) != makesym(functor_atom_1)) goto read__all__1_6_2;
  allocp[0] = makesym(functor_module_2);
  x1 = arg(a0, 0);
  allocp[1] = x1;
  allocp[2] = x2 = makeref(&allocp[2]);
  x0 = makefunctor(&allocp[0]);
  allocp[3] = x4 = makeref(&allocp[3]);
  allocp[4] = x0;
  x3 = makecons(&allocp[3]);
  allocp += 5;
  unify_value(a1, x3);
  allocp[0] = makesym(functor_getwt_1);
  allocp[1] = x6 = makeref(&allocp[1]);
  x5 = makefunctor(&allocp[0]);
  allocp[2] = x8 = makeref(&allocp[2]);
  allocp[3] = x5;
  x7 = makecons(&allocp[2]);
  allocp += 4;
  unify_value(a2, x7);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__kl1cmp_xread__all_6);
  allocp[2] = x9 = makeref(&allocp[2]);
  allocp[3] = x4;
  allocp[4] = x10 = makeref(&allocp[4]);
  allocp[5] = a3;
  allocp[6] = x11 = makeref(&allocp[6]);
  allocp[7] = a5;
  a0 = x6;
  a1 = x2;
  a2 = x9;
  a3 = x8;
  a5 = a4;
  a6 = x11;
  a4 = x10;
  qp = (struct goalrec*)&allocp[0];
  allocp += 8;
  execute(read__one__module_7_0);
  goto read__one__module_7_ext_interrupt;
 case VARREF:
  deref_and_jump(a0,read__all__1_6_1);
  *reasonp++ =  a0;
  goto read__all__1_6_2;
  };
  goto read__all__1_6_2;
 read__all__1_6_2:
  otherwise(read__all__1_6_interrupt);
  unify2(a5, a4, a3, a2);
  x0 = NILATOM;
  unify_value(a1, x0);
  allocp[0] = NILATOM;
  allocp[1] = a0;
  x1 = makecons(&allocp[0]);
  allocp[2] = (q)qp;
  allocp[3] = (q)(&predicate_klic__comp__message_xreport__error_2);
  allocp[4] = makefunctor(&string_const_12);
  allocp[5] = x1;
  qp = (struct goalrec*)&allocp[2];
  allocp += 6;
  proceed();
 read__all__1_6_ext_interrupt:
  reasonp = 0l;
 read__all__1_6_interrupt:
  toppred = &predicate_klic__comp__kl1cmp_xread__all__1_6;
  goto interrupt_6;
 }

 read__one__module_7_top: {
  q x0, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11;
  a1 = qp->args[1];
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  a6 = qp->args[6];
  qp = qp->next;
 read__one__module_7_clear_reason:
  reasonp = reasons;
 read__one__module_7_0:
 read__one__module_7_1:
  switch (ptagof(a0)) {
 case FUNCTOR:
  if (functor_of(a0) != makesym(functor_normal_1)) goto read__one__module_7_7;
  x0 = arg(a0, 0);
 read__one__module_7_2:
  switch (ptagof(x0)) {
 case ATOMIC:
  if (isint(x0)) goto read__one__module_7_7;
  switch (symval(x0)) {
 case symval(makesym(atom_end__of__file)):
  unify2(a6, a5, a4, a3);
  x1 = NILATOM;
  unify_value(a1, x1);
  x2 = makesym(atom_end__of__file);
  unify_value(a2, x2);
  proceed();
 case symval(makesym(atom_empty)):
  unify2(a6, a5, a4, a3);
  x1 = NILATOM;
  unify_value(a1, x1);
  x2 = makesym(atom_end__of__file);
  unify_value(a2, x2);
  proceed();
  };
  goto read__one__module_7_7;
 case FUNCTOR:
  if (functor_of(x0) != makesym(functor_functor_1)) goto read__one__module_7_7;
  x1 = arg(x0, 0);
 read__one__module_7_3:
  switch (ptagof(x1)) {
 case FUNCTOR:
  if (functor_of(x1) != makesym(functor__3A_2D_1)) goto read__one__module_7_7;
  x2 = arg(x1, 0);
 read__one__module_7_4:
  switch (ptagof(x2)) {
 case FUNCTOR:
  if (functor_of(x2) != makesym(functor_functor_1)) goto read__one__module_7_7;
  x3 = arg(x2, 0);
 read__one__module_7_5:
  switch (ptagof(x3)) {
 case FUNCTOR:
  switch (symval(functor_of(x3))) {
 case functor_module_1:
  x4 = arg(x3, 0);
 read__one__module_7_6:
  switch (ptagof(x4)) {
 case FUNCTOR:
  if (functor_of(x4) != makesym(functor_atom_1)) goto read__one__module_7_7;
  unify2(a6, a5, a4, a3);
  x5 = NILATOM;
  unify_value(a1, x5);
  allocp[0] = makesym(functor_atom_1);
  x7 = arg(x4, 0);
  allocp[1] = x7;
  x6 = makefunctor(&allocp[0]);
  allocp[2] = makesym(functor_module_1);
  allocp[3] = x6;
  x8 = makefunctor(&allocp[2]);
  allocp[4] = makesym(functor_functor_1);
  allocp[5] = x8;
  x9 = makefunctor(&allocp[4]);
  allocp[6] = makesym(functor__3A_2D_1);
  allocp[7] = x9;
  x10 = makefunctor(&allocp[6]);
  allocp[8] = makesym(functor_functor_1);
  allocp[9] = x10;
  x11 = makefunctor(&allocp[8]);
  allocp += 10;
  unify_value(a2, x11);
  proceed();
 case VARREF:
  deref_and_jump(x4,read__one__module_7_6);
  *reasonp++ =  x4;
  goto read__one__module_7_7;
  };
  goto read__one__module_7_7;
 case functor_with_1:
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__kl1cmp_xread__one__module_7);
  allocp[2] = x4 = makeref(&allocp[2]);
  allocp[3] = a1;
  allocp[4] = a2;
  allocp[5] = x5 = makeref(&allocp[5]);
  allocp[6] = a4;
  allocp[7] = x6 = makeref(&allocp[7]);
  allocp[8] = a6;
  allocp[9] = makesym(functor_getwt_1);
  allocp[10] = x4;
  x7 = makefunctor(&allocp[9]);
  allocp[11] = x5;
  allocp[12] = x7;
  x8 = makecons(&allocp[11]);
  qp = (struct goalrec*)&allocp[0];
  allocp += 13;
  unify_value(a3, x8);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__cmacro_xinsDefs_3);
  x9 = arg(x3, 0);
  allocp[2] = x9;
  allocp[3] = a5;
  allocp[4] = x6;
  qp = (struct goalrec*)&allocp[0];
  allocp += 5;
  proceed();
  };
  goto read__one__module_7_7;
 case VARREF:
  deref_and_jump(x3,read__one__module_7_5);
  *reasonp++ =  x3;
  goto read__one__module_7_7;
  };
  goto read__one__module_7_7;
 case VARREF:
  deref_and_jump(x2,read__one__module_7_4);
  *reasonp++ =  x2;
  goto read__one__module_7_7;
  };
  goto read__one__module_7_7;
 case VARREF:
  deref_and_jump(x1,read__one__module_7_3);
  *reasonp++ =  x1;
  goto read__one__module_7_7;
  };
  goto read__one__module_7_7;
 case VARREF:
  deref_and_jump(x0,read__one__module_7_2);
  *reasonp++ =  x0;
  goto read__one__module_7_7;
  };
  goto read__one__module_7_7;
 case VARREF:
  deref_and_jump(a0,read__one__module_7_1);
  *reasonp++ =  a0;
  goto read__one__module_7_7;
  };
  goto read__one__module_7_7;
 read__one__module_7_7:
  otherwise(read__one__module_7_interrupt);
 read__one__module_7_8:
  switch (ptagof(a0)) {
 case FUNCTOR:
  if (functor_of(a0) != makesym(functor_normal_1)) goto read__one__module_7_11;
  x0 = arg(a0, 0);
 read__one__module_7_9:
  switch (ptagof(x0)) {
 case FUNCTOR:
  if (functor_of(x0) != makesym(functor_functor_1)) goto read__one__module_7_11;
  x1 = arg(x0, 0);
 read__one__module_7_10:
  switch (ptagof(x1)) {
 case FUNCTOR:
  if (functor_of(x1) != makesym(functor__3A_2D_1)) goto read__one__module_7_11;
  allocp[0] = makesym(functor_getwt_1);
  allocp[1] = x3 = makeref(&allocp[1]);
  x2 = makefunctor(&allocp[0]);
  allocp[2] = x5 = makeref(&allocp[2]);
  allocp[3] = x2;
  x4 = makecons(&allocp[2]);
  allocp += 4;
  unify_value(a3, x4);
  a0 = x3;
  a3 = x5;
  execute(read__one__module_7_0);
  goto read__one__module_7_ext_interrupt;
 case VARREF:
  deref_and_jump(x1,read__one__module_7_10);
  *reasonp++ =  x1;
  goto read__one__module_7_11;
  };
  goto read__one__module_7_11;
 case VARREF:
  deref_and_jump(x0,read__one__module_7_9);
  *reasonp++ =  x0;
  goto read__one__module_7_11;
  };
  goto read__one__module_7_11;
 case VARREF:
  deref_and_jump(a0,read__one__module_7_8);
  *reasonp++ =  a0;
  goto read__one__module_7_11;
  };
  goto read__one__module_7_11;
 read__one__module_7_11:
  otherwise(read__one__module_7_interrupt);
 read__one__module_7_12:
  switch (ptagof(a0)) {
 case FUNCTOR:
  if (functor_of(a0) != makesym(functor_normal_1)) goto read__one__module_7_13;
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__kl1cmp_xread__one__module_7);
  allocp[2] = x0 = makeref(&allocp[2]);
  allocp[3] = x1 = makeref(&allocp[3]);
  allocp[4] = a2;
  allocp[5] = x2 = makeref(&allocp[5]);
  allocp[6] = a4;
  allocp[7] = x3 = makeref(&allocp[7]);
  allocp[8] = a6;
  allocp[9] = makesym(functor_getwt_1);
  allocp[10] = x0;
  x4 = makefunctor(&allocp[9]);
  allocp[11] = x2;
  allocp[12] = x4;
  x5 = makecons(&allocp[11]);
  qp = (struct goalrec*)&allocp[0];
  allocp += 13;
  unify_value(a3, x5);
  allocp[0] = x1;
  allocp[1] = x7 = makeref(&allocp[1]);
  x6 = makecons(&allocp[0]);
  allocp += 2;
  unify_value(a1, x6);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__macro_xmacro_2);
  allocp[2] = x8 = makeref(&allocp[2]);
  allocp[3] = x7;
  allocp[4] = (q)(struct goalrec*)&allocp[0];
  allocp[5] = (q)(&predicate_klic__comp__cmacro_xreplaceClause_4);
  x9 = arg(a0, 0);
  allocp[6] = x9;
  allocp[7] = x8;
  allocp[8] = a5;
  allocp[9] = x3;
  qp = (struct goalrec*)&allocp[4];
  allocp += 10;
  proceed();
 case VARREF:
  deref_and_jump(a0,read__one__module_7_12);
  *reasonp++ =  a0;
  goto read__one__module_7_13;
  };
  goto read__one__module_7_13;
 read__one__module_7_13:
  otherwise(read__one__module_7_interrupt);
  unify2(a6, a5, a4, a3);
  execute(handle__exception_1_0);
  goto handle__exception_1_ext_interrupt;
 read__one__module_7_ext_interrupt:
  reasonp = 0l;
 read__one__module_7_interrupt:
  toppred = &predicate_klic__comp__kl1cmp_xread__one__module_7;
  goto interrupt_7;
 }

 compile2_6_top: {
  q x0, x1, x2, x3, x4;
  a1 = qp->args[1];
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  qp = qp->next;
 compile2_6_clear_reason:
  reasonp = reasons;
 compile2_6_0:
 compile2_6_1:
  switch (ptagof(a0)) {
 case FUNCTOR:
  if (functor_of(a0) != makesym(functor_normal_2)) goto compile2_6_interrupt;
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__kl1cmp_xcompile__modules_6);
  x0 = arg(a0, 0);
  allocp[2] = x0;
  allocp[3] = a1;
  allocp[4] = x1 = makeref(&allocp[4]);
  allocp[5] = a3;
  allocp[6] = a4;
  allocp[7] = a5;
  allocp[8] = x1;
  allocp[9] = makeint(10L);
  x2 = makecons(&allocp[8]);
  allocp[10] = (q)(struct goalrec*)&allocp[0];
  allocp[11] = (q)(&predicate_klic__comp__kl1cmp_xwrite__inline__header_3);
  x3 = arg(a0, 1);
  allocp[12] = x3;
  allocp[13] = x4 = makeref(&allocp[13]);
  allocp[14] = x2;
  a0 = a2;
  a1 = x4;
  qp = (struct goalrec*)&allocp[10];
  allocp += 15;
  execute(write__file__header_2_0);
  goto write__file__header_2_ext_interrupt;
 case VARREF:
  deref_and_jump(a0,compile2_6_1);
  *reasonp++ =  a0;
  goto compile2_6_interrupt;
  };
  goto compile2_6_interrupt;
 compile2_6_ext_interrupt:
  reasonp = 0l;
 compile2_6_interrupt:
  goto interrupt_6;
 }

 write__file__header_2_top: {
  q x0, x1, x2, x3;
  a1 = qp->args[1];
  qp = qp->next;
 write__file__header_2_clear_reason:
  reasonp = reasons;
 write__file__header_2_0:
  allocp[0] = NILATOM;
  allocp[1] = x1 = makeref(&allocp[1]);
  x0 = makecons(&allocp[0]);
  allocp[2] = x0;
  allocp[3] = x3 = makeref(&allocp[3]);
  x2 = makecons(&allocp[2]);
  allocp[4] = (q)qp;
  allocp[5] = (q)(&predicate_klic__comp__obj_xklicformat_4);
  allocp[6] = makefunctor(&string_const_7);
  allocp[7] = x2;
  allocp[8] = a0;
  allocp[9] = a1;
  allocp[10] = (q)(struct goalrec*)&allocp[4];
  allocp[11] = (q)(&predicate_klic__comp__version_xcompiler__date_1);
  allocp[12] = x1;
  allocp[13] = (q)(struct goalrec*)&allocp[10];
  allocp[14] = (q)(&predicate_klic__comp__version_xcompiler__version_1);
  allocp[15] = x3;
  qp = (struct goalrec*)&allocp[13];
  allocp += 16;
  proceed();
 write__file__header_2_ext_interrupt:
  reasonp = 0l;
 write__file__header_2_interrupt:
  toppred = &predicate_klic__comp__kl1cmp_xwrite__file__header_2;
  goto interrupt_2;
 }

 write__inline__header_3_top: {
  q x0, x1, x2;
  a1 = qp->args[1];
  a2 = qp->args[2];
  qp = qp->next;
 write__inline__header_3_clear_reason:
  reasonp = reasons;
 write__inline__header_3_0:
 write__inline__header_3_1:
  switch (ptagof(a0)) {
 case CONS:
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__kl1cmp_xwrite__inline__header_3);
  x0 = cdr_of(a0);
  allocp[2] = x0;
  allocp[3] = x1 = makeref(&allocp[3]);
  allocp[4] = a2;
  allocp[5] = (q)(struct goalrec*)&allocp[0];
  allocp[6] = (q)(&predicate_klic__comp__obj_xwrite__inline_6);
  x2 = car_of(a0);
  allocp[7] = x2;
  allocp[8] = NILATOM;
  allocp[9] = NILATOM;
  allocp[10] = NILATOM;
  allocp[11] = a1;
  allocp[12] = x1;
  qp = (struct goalrec*)&allocp[5];
  allocp += 13;
  proceed();
 case ATOMIC:
  if (a0 != NILATOM) goto write__inline__header_3_interrupt;
  unify(a2, a1);
  proceed();
 case VARREF:
  deref_and_jump(a0,write__inline__header_3_1);
  *reasonp++ =  a0;
  goto write__inline__header_3_interrupt;
  };
  goto write__inline__header_3_interrupt;
 write__inline__header_3_ext_interrupt:
  reasonp = 0l;
 write__inline__header_3_interrupt:
  goto interrupt_3;
 }

 compile__modules_6_top: {
  q x0, x1, x2, x3;
  a1 = qp->args[1];
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  qp = qp->next;
 compile__modules_6_clear_reason:
  reasonp = reasons;
 compile__modules_6_0:
 compile__modules_6_1:
  switch (ptagof(a0)) {
 case CONS:
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__kl1cmp_xcompile__modules_6);
  x0 = cdr_of(a0);
  allocp[2] = x0;
  allocp[3] = a1;
  allocp[4] = x1 = makeref(&allocp[4]);
  allocp[5] = a3;
  allocp[6] = x2 = makeref(&allocp[6]);
  allocp[7] = a5;
  allocp[8] = (q)(struct goalrec*)&allocp[0];
  allocp[9] = (q)(&predicate_klic__comp__write_xwrite__header__file_3);
  allocp[10] = x3 = makeref(&allocp[10]);
  allocp[11] = a4;
  allocp[12] = x2;
  allocp[13] = (q)(struct goalrec*)&allocp[8];
  allocp[14] = (q)(&predicate_klic__comp__write_xwrite__out_3);
  allocp[15] = x3;
  allocp[16] = a2;
  allocp[17] = x1;
  a0 = car_of(a0);
  a1 = x3;
  qp = (struct goalrec*)&allocp[13];
  allocp += 18;
  execute(analyze_2_0);
  goto analyze_2_ext_interrupt;
 case ATOMIC:
  if (a0 != NILATOM) goto compile__modules_6_interrupt;
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__kl1cmp_xwait__output__end_3);
  allocp[2] = x0 = makeref(&allocp[2]);
  allocp[3] = x1 = makeref(&allocp[3]);
  allocp[4] = a1;
  allocp[5] = (q)(struct goalrec*)&allocp[0];
  allocp[6] = (q)(&predicate_klic__comp__obj_xflush_3);
  allocp[7] = x1;
  allocp[8] = a4;
  allocp[9] = a5;
  allocp[10] = (q)(struct goalrec*)&allocp[5];
  allocp[11] = (q)(&predicate_klic__comp__obj_xflush_3);
  allocp[12] = x0;
  allocp[13] = a2;
  allocp[14] = a3;
  qp = (struct goalrec*)&allocp[10];
  allocp += 15;
  proceed();
 case VARREF:
  deref_and_jump(a0,compile__modules_6_1);
  *reasonp++ =  a0;
  goto compile__modules_6_interrupt;
  };
  goto compile__modules_6_interrupt;
 compile__modules_6_ext_interrupt:
  reasonp = 0l;
 compile__modules_6_interrupt:
  goto interrupt_6;
 }

 wait__output__end_3_top: {
  q x0, x1;
  a1 = qp->args[1];
  a2 = qp->args[2];
  qp = qp->next;
 wait__output__end_3_clear_reason:
  reasonp = reasons;
 wait__output__end_3_0:
 wait__output__end_3_1:
  switch (ptagof(a0)) {
 case ATOMIC:
 wait__output__end_3_2:
  if (a0 != makeint(0L)) goto wait__output__end_3_5;
 wait__output__end_3_3:
  switch (ptagof(a1)) {
 case ATOMIC:
 wait__output__end_3_4:
  if (a1 != makeint(0L)) goto wait__output__end_3_5;
  x0 = makesym(atom_normal);
  unify_value(a2, x0);
  proceed();
 case VARREF:
  deref_and_jump(a1,wait__output__end_3_3);
  *reasonp++ =  a1;
  goto wait__output__end_3_5;
  };
  goto wait__output__end_3_5;
 case VARREF:
  deref_and_jump(a0,wait__output__end_3_1);
  *reasonp++ =  a0;
  goto wait__output__end_3_5;
  };
  goto wait__output__end_3_5;
 wait__output__end_3_5:
  otherwise(wait__output__end_3_interrupt);
  allocp[0] = makesym(functor__2C_2);
  allocp[1] = a0;
  allocp[2] = a1;
  x0 = makefunctor(&allocp[0]);
  allocp[3] = makesym(functor_abnormal_2);
  allocp[4] = makesym(atom_fileoutput);
  allocp[5] = x0;
  x1 = makefunctor(&allocp[3]);
  allocp += 6;
  unify_value(a2, x1);
  proceed();
 wait__output__end_3_ext_interrupt:
  reasonp = 0l;
 wait__output__end_3_interrupt:
  goto interrupt_3;
 }

 analyze_2_top: {
  q x0, x1, x2, x3, x4, x5, x6, x7, x8, x9;
  a1 = qp->args[1];
  qp = qp->next;
 analyze_2_clear_reason:
  reasonp = reasons;
 analyze_2_0:
 analyze_2_1:
  switch (ptagof(a0)) {
 case FUNCTOR:
  if (functor_of(a0) != makesym(functor_module_2)) goto analyze_2_2;
  allocp[0] = makesym(functor_module_3);
  x1 = arg(a0, 0);
  allocp[1] = x1;
  allocp[2] = x2 = makeref(&allocp[2]);
  allocp[3] = x3 = makeref(&allocp[3]);
  x0 = makefunctor(&allocp[0]);
  allocp += 4;
  unify_value(a1, x0);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__kl1cmp_xanalyze1_6);
  x4 = arg(a0, 1);
  allocp[2] = x4;
  x5 = arg(a0, 0);
  allocp[3] = x5;
  allocp[4] = x2;
  allocp[5] = x3;
  allocp[6] = x6 = makeref(&allocp[6]);
  allocp[7] = x7 = makeref(&allocp[7]);
  allocp[8] = makesym(functor_empty_1);
  allocp[9] = x6;
  x8 = makefunctor(&allocp[8]);
  allocp[10] = x7;
  allocp[11] = x8;
  x9 = makecons(&allocp[10]);
  allocp[12] = (q)(struct goalrec*)&allocp[0];
  allocp[13] = (q)(&predicate_keyed__sorted__set_xcreate_1);
  allocp[14] = x9;
  qp = (struct goalrec*)&allocp[12];
  allocp += 15;
  proceed();
 case VARREF:
  deref_and_jump(a0,analyze_2_1);
  *reasonp++ =  a0;
  goto analyze_2_2;
  };
  goto analyze_2_2;
 analyze_2_2:
  otherwise(analyze_2_interrupt);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__message_xreport__error_2);
  allocp[2] = makefunctor(&string_const_9);
  allocp[3] = NILATOM;
  qp = (struct goalrec*)&allocp[0];
  allocp += 4;
  proceed();
 analyze_2_ext_interrupt:
  reasonp = 0l;
 analyze_2_interrupt:
  toppred = &predicate_klic__comp__kl1cmp_xanalyze_2;
  goto interrupt_2;
 }

 analyze1_6_top: {
  q x0, x1, x2, x3;
  a1 = qp->args[1];
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  qp = qp->next;
 analyze1_6_clear_reason:
  reasonp = reasons;
 analyze1_6_0:
 analyze1_6_1:
  switch (ptagof(a4)) {
 case ATOMIC:
  if (a4 != makesym(atom_yes)) goto analyze1_6_2;
  allocp[0] = makesym(functor_put_3);
  allocp[1] = makesym(atom_compiling__module);
  allocp[2] = a1;
  allocp[3] = x1 = makeref(&allocp[3]);
  x0 = makefunctor(&allocp[0]);
  allocp[4] = x3 = makeref(&allocp[4]);
  allocp[5] = x0;
  x2 = makecons(&allocp[4]);
  allocp += 6;
  unify_value(a5, x2);
  a1 = makeint(0L);
  a4 = NILATOM;
  a5 = x3;
  execute(analyze__preds_6_0);
  goto analyze__preds_6_ext_interrupt;
 case VARREF:
  deref_and_jump(a4,analyze1_6_1);
  *reasonp++ =  a4;
  goto analyze1_6_2;
  };
  goto analyze1_6_2;
 analyze1_6_2:
  otherwise(analyze1_6_interrupt);
  x0 = NILATOM;
  unify_value(a5, x0);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__message_xreport__error_2);
  allocp[2] = makefunctor(&string_const_10);
  allocp[3] = NILATOM;
  qp = (struct goalrec*)&allocp[0];
  allocp += 4;
  proceed();
 analyze1_6_ext_interrupt:
  reasonp = 0l;
 analyze1_6_interrupt:
  goto interrupt_6;
 }

 analyze__preds_6_top: {
  q x0, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13, x14, x15, x16, x17, x18, x19, x20, x21, x22, x23, x24, x25, x26, x27, x28, x29, x30, x31, x32, x33, x34, x35, x36, x37;
  a1 = qp->args[1];
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  qp = qp->next;
 analyze__preds_6_clear_reason:
  reasonp = reasons;
 analyze__preds_6_0:
 analyze__preds_6_1:
  switch (ptagof(a0)) {
 case CONS:
  x0 = car_of(a0);
 analyze__preds_6_2:
  switch (ptagof(x0)) {
 case FUNCTOR:
  if (functor_of(x0) != makesym(functor_clause_4)) goto analyze__preds_6_interrupt;
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__kl1cmp_xanalyze__preds_6);
  allocp[2] = x1 = makeref(&allocp[2]);
  allocp[3] = x2 = makeref(&allocp[3]);
  allocp[4] = x3 = makeref(&allocp[4]);
  allocp[5] = x4 = makeref(&allocp[5]);
  allocp[6] = a4;
  allocp[7] = x5 = makeref(&allocp[7]);
  allocp[8] = (q)(struct goalrec*)&allocp[0];
  allocp[9] = (q)(&predicate_integer__arithmetics_xadd_3);
  allocp[10] = a1;
  allocp[11] = makeint(1L);
  allocp[12] = x2;
  allocp[13] = makesym(functor__2F_2);
  allocp[14] = x7 = makeref(&allocp[14]);
  allocp[15] = x8 = makeref(&allocp[15]);
  x6 = makefunctor(&allocp[13]);
  allocp[16] = makesym(functor_exec_1);
  allocp[17] = x6;
  x9 = makefunctor(&allocp[16]);
  allocp[18] = (q)(struct goalrec*)&allocp[8];
  allocp[19] = (q)(&predicate_klic__comp__util_xremove_4);
  allocp[20] = x10 = makeref(&allocp[20]);
  allocp[21] = x9;
  allocp[22] = a3;
  allocp[23] = x4;
  allocp[24] = makesym(functor__2F_2);
  allocp[25] = x7;
  allocp[26] = x8;
  x11 = makefunctor(&allocp[24]);
  allocp[27] = (q)(struct goalrec*)&allocp[18];
  allocp[28] = (q)(&predicate_klic__comp__normalize_xgenerate_8);
  allocp[29] = x11;
  allocp[30] = x12 = makeref(&allocp[30]);
  allocp[31] = x13 = makeref(&allocp[31]);
  allocp[32] = x10;
  allocp[33] = NILATOM;
  allocp[34] = x14 = makeref(&allocp[34]);
  allocp[35] = x15 = makeref(&allocp[35]);
  allocp[36] = x5;
  allocp[37] = (q)(struct goalrec*)&allocp[27];
  allocp[38] = (q)(&predicate_klic__comp__kl1cmp_xrename__macro__preds_5);
  allocp[39] = x16 = makeref(&allocp[39]);
  allocp[40] = x17 = makeref(&allocp[40]);
  allocp[41] = makeint(0L);
  allocp[42] = x18 = makeref(&allocp[42]);
  allocp[43] = x1;
  allocp[44] = x20 = makeref(&allocp[44]);
  allocp[45] = makeint(47L);
  x19 = makecons(&allocp[44]);
  allocp[46] = (q)(struct goalrec*)&allocp[37];
  allocp[47] = (q)(&predicate_klic__comp__util_xappend_3);
  allocp[48] = x21 = makeref(&allocp[48]);
  allocp[49] = x19;
  allocp[50] = x17;
  allocp[51] = (q)(struct goalrec*)&allocp[46];
  allocp[52] = (q)(&predicate_klic__comp__util_xname_2);
  allocp[53] = x8;
  allocp[54] = x20;
  allocp[55] = (q)(struct goalrec*)&allocp[51];
  allocp[56] = (q)(&predicate_klic__comp__util_xname_2);
  allocp[57] = x7;
  allocp[58] = x21;
  allocp[59] = (q)(struct goalrec*)&allocp[55];
  allocp[60] = (q)(&predicate_klic__comp__kl1cmp_xcollect__same_8);
  x22 = cdr_of(a0);
  allocp[61] = x22;
  allocp[62] = x18;
  allocp[63] = x7;
  allocp[64] = x8;
  allocp[65] = makeint(1L);
  allocp[66] = x23 = makeref(&allocp[66]);
  x24 = arg(x0, 3);
  allocp[67] = x24;
  allocp[68] = x16;
  allocp[69] = makesym(functor__2F_2);
  allocp[70] = x7;
  allocp[71] = x8;
  x25 = makefunctor(&allocp[69]);
  allocp[72] = makesym(functor__2F_2);
  allocp[73] = x25;
  allocp[74] = makeint(0L);
  x26 = makefunctor(&allocp[72]);
  allocp[75] = makesym(functor_clause_4);
  allocp[76] = x26;
  x28 = arg(x0, 0);
  allocp[77] = x28;
  x29 = arg(x0, 1);
  allocp[78] = x29;
  x30 = arg(x0, 2);
  allocp[79] = x30;
  x27 = makefunctor(&allocp[75]);
  allocp[80] = x23;
  allocp[81] = x27;
  x31 = makecons(&allocp[80]);
  qp = (struct goalrec*)&allocp[59];
  allocp += 82;
  unify_value(x12, x31);
  allocp[0] = makesym(functor_pred_5);
  allocp[1] = x7;
  allocp[2] = x8;
  allocp[3] = a1;
  allocp[4] = x13;
  allocp[5] = x14;
  x32 = makefunctor(&allocp[0]);
  allocp[6] = x3;
  allocp[7] = x32;
  x33 = makecons(&allocp[6]);
  allocp += 8;
  unify_value(a2, x33);
  allocp[0] = makesym(functor__2F_2);
  allocp[1] = x7;
  allocp[2] = x8;
  x34 = makefunctor(&allocp[0]);
  allocp[3] = makesym(functor_put_3);
  allocp[4] = makesym(atom_compiling__predicate);
  allocp[5] = x34;
  allocp[6] = x36 = makeref(&allocp[6]);
  x35 = makefunctor(&allocp[3]);
  allocp[7] = x15;
  allocp[8] = x35;
  x37 = makecons(&allocp[7]);
  allocp += 9;
  unify_value(a5, x37);
  a0 = arg(x0, 0);
  a1 = x7;
  a2 = x8;
  execute(get__clause__name_3_0);
  goto get__clause__name_3_ext_interrupt;
 case VARREF:
  deref_and_jump(x0,analyze__preds_6_2);
  *reasonp++ =  x0;
  goto analyze__preds_6_interrupt;
  };
  goto analyze__preds_6_interrupt;
 case ATOMIC:
  if (a0 != NILATOM) goto analyze__preds_6_interrupt;
  x0 = NILATOM;
  unify_value(a2, x0);
  unify(a4, a3);
  x1 = NILATOM;
  unify_value(a5, x1);
  proceed();
 case VARREF:
  deref_and_jump(a0,analyze__preds_6_1);
  *reasonp++ =  a0;
  goto analyze__preds_6_interrupt;
  };
  goto analyze__preds_6_interrupt;
 analyze__preds_6_ext_interrupt:
  reasonp = 0l;
 analyze__preds_6_interrupt:
  toppred = &predicate_klic__comp__kl1cmp_xanalyze__preds_6;
  goto interrupt_6;
 }

 get__clause__name_3_top: {
  q x0, x1, x2;
  a1 = qp->args[1];
  a2 = qp->args[2];
  qp = qp->next;
 get__clause__name_3_clear_reason:
  reasonp = reasons;
 get__clause__name_3_0:
 get__clause__name_3_1:
  switch (ptagof(a0)) {
 case FUNCTOR:
  switch (symval(functor_of(a0))) {
 case functor_functor_1:
  x0 = arg(a0, 0);
 get__clause__name_3_2:
  if (isref(x0)) goto get__clause__name_3_3;
  gblt_pfunctor(x0,x1,get__clause__name_3_interrupt);
  gblt_arity(x0,x2,get__clause__name_3_interrupt);
  unify_value(a1, x1);
  unify_value(a2, x2);
  proceed();
 get__clause__name_3_3:
  deref_and_jump(x0,get__clause__name_3_2);
  *reasonp++ =  x0;
  goto get__clause__name_3_interrupt;
 case functor_atom_1:
  x0 = arg(a0, 0);
  unify(a1, x0);
  x1 = makeint(0L);
  unify_value(a2, x1);
  proceed();
  };
  goto get__clause__name_3_interrupt;
 case VARREF:
  deref_and_jump(a0,get__clause__name_3_1);
  *reasonp++ =  a0;
  goto get__clause__name_3_interrupt;
  };
  goto get__clause__name_3_interrupt;
 get__clause__name_3_ext_interrupt:
  reasonp = 0l;
 get__clause__name_3_interrupt:
  toppred = &predicate_klic__comp__kl1cmp_xget__clause__name_3;
  goto interrupt_3;
 }

 collect__same_8_top: {
  q x0, x1, x2, x3, x4;
  a1 = qp->args[1];
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  a6 = qp->args[6];
  a7 = qp->args[7];
  qp = qp->next;
 collect__same_8_clear_reason:
  reasonp = reasons;
 collect__same_8_0:
 collect__same_8_1:
  switch (ptagof(a0)) {
 case CONS:
  x0 = car_of(a0);
 collect__same_8_2:
  switch (ptagof(x0)) {
 case ATOMIC:
  if (isint(x0)) goto collect__same_8_3;
  switch (symval(x0)) {
 case symval(makesym(atom_otherwise)):
  allocp[0] = x2 = makeref(&allocp[0]);
  allocp[1] = makesym(atom_otherwise);
  x1 = makecons(&allocp[0]);
  allocp += 2;
  unify_value(a5, x1);
  a0 = cdr_of(a0);
  a5 = x2;
  execute(collect__same_8_0);
  goto collect__same_8_ext_interrupt;
 case symval(makesym(atom_alternatively)):
  allocp[0] = x2 = makeref(&allocp[0]);
  allocp[1] = makesym(atom_alternatively);
  x1 = makecons(&allocp[0]);
  allocp += 2;
  unify_value(a5, x1);
  a0 = cdr_of(a0);
  a5 = x2;
  execute(collect__same_8_0);
  goto collect__same_8_ext_interrupt;
  };
  goto collect__same_8_3;
 case FUNCTOR:
  if (functor_of(x0) != makesym(functor_clause_4)) goto collect__same_8_3;
  x2 = cdr_of(a0);
  allocp[0] = x2;
  allocp[1] = x0;
  x1 = makecons(&allocp[0]);
  allocp[2] = (q)qp;
  allocp[3] = (q)(&predicate_klic__comp__kl1cmp_xcollect__same__1_10);
  allocp[4] = x1;
  allocp[5] = a1;
  allocp[6] = a2;
  allocp[7] = a3;
  allocp[8] = a4;
  allocp[9] = a5;
  allocp[10] = a6;
  allocp[11] = a7;
  allocp[12] = x3 = makeref(&allocp[12]);
  allocp[13] = x4 = makeref(&allocp[13]);
  a0 = arg(x0, 0);
  a1 = x3;
  a2 = x4;
  qp = (struct goalrec*)&allocp[2];
  allocp += 14;
  execute(get__clause__name_3_0);
  goto get__clause__name_3_ext_interrupt;
 case VARREF:
  deref_and_jump(x0,collect__same_8_2);
  *reasonp++ =  x0;
  goto collect__same_8_3;
  };
  goto collect__same_8_3;
 case VARREF:
  deref_and_jump(a0,collect__same_8_1);
  *reasonp++ =  a0;
  goto collect__same_8_3;
  };
  goto collect__same_8_3;
 collect__same_8_3:
  otherwise(collect__same_8_interrupt);
  unify(a1, a0);
  x0 = NILATOM;
  unify_value(a5, x0);
  unify(a7, a6);
  proceed();
 collect__same_8_ext_interrupt:
  reasonp = 0l;
 collect__same_8_interrupt:
  goto interrupt_8;
 }

 collect__same__1_10_top: {
  q x0, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13;
  a1 = qp->args[1];
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  a6 = qp->args[6];
  a7 = qp->args[7];
  a8 = qp->args[8];
  a9 = qp->args[9];
  qp = qp->next;
 collect__same__1_10_clear_reason:
  reasonp = reasons;
 collect__same__1_10_0:
 collect__same__1_10_1:
  switch (ptagof(a0)) {
 case CONS:
  x0 = car_of(a0);
 collect__same__1_10_2:
  switch (ptagof(x0)) {
 case FUNCTOR:
  if (functor_of(x0) != makesym(functor_clause_4)) goto collect__same__1_10_11;
 collect__same__1_10_3:
  if (isref(a2)) goto collect__same__1_10_10;
 collect__same__1_10_4:
  if (isref(a2)) goto collect__same__1_10_9;
  if_not_equal(a2, a8, collect__same__1_10_11);
 collect__same__1_10_5:
  if (isref(a3)) goto collect__same__1_10_8;
 collect__same__1_10_6:
  if (isref(a3)) goto collect__same__1_10_7;
  if_not_equal(a3, a9, collect__same__1_10_11);
  allocp[0] = makesym(functor__2F_2);
  allocp[1] = a2;
  allocp[2] = a3;
  x1 = makefunctor(&allocp[0]);
  allocp[3] = makesym(functor__2F_2);
  allocp[4] = x1;
  allocp[5] = a4;
  x2 = makefunctor(&allocp[3]);
  allocp[6] = makesym(functor_clause_4);
  allocp[7] = x2;
  x4 = arg(x0, 0);
  allocp[8] = x4;
  x5 = arg(x0, 1);
  allocp[9] = x5;
  x6 = arg(x0, 2);
  allocp[10] = x6;
  x3 = makefunctor(&allocp[6]);
  allocp[11] = x8 = makeref(&allocp[11]);
  allocp[12] = x3;
  x7 = makecons(&allocp[11]);
  allocp += 13;
  unify_value(a5, x7);
  x9 = makeint(1L);
  bblt_add_no_check_y(a4,x9,x10);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__kl1cmp_xcollect__same_8);
  x11 = cdr_of(a0);
  allocp[2] = x11;
  allocp[3] = a1;
  allocp[4] = a2;
  allocp[5] = a3;
  allocp[6] = x10;
  allocp[7] = x8;
  allocp[8] = x12 = makeref(&allocp[8]);
  allocp[9] = a7;
  allocp[10] = (q)(struct goalrec*)&allocp[0];
  allocp[11] = (q)(&predicate_klic__comp__util_xappend_3);
  x13 = arg(x0, 3);
  allocp[12] = x13;
  allocp[13] = a6;
  allocp[14] = x12;
  qp = (struct goalrec*)&allocp[10];
  allocp += 15;
  proceed();
 collect__same__1_10_7:
  deref_and_jump(a3,collect__same__1_10_6);
  *reasonp++ =  a3;
  goto collect__same__1_10_11;
 collect__same__1_10_8:
  deref_and_jump(a3,collect__same__1_10_5);
  *reasonp++ =  a3;
  goto collect__same__1_10_11;
 collect__same__1_10_9:
  deref_and_jump(a2,collect__same__1_10_4);
  *reasonp++ =  a2;
  goto collect__same__1_10_11;
 collect__same__1_10_10:
  deref_and_jump(a2,collect__same__1_10_3);
  *reasonp++ =  a2;
  goto collect__same__1_10_11;
 case VARREF:
  deref_and_jump(x0,collect__same__1_10_2);
  *reasonp++ =  x0;
  goto collect__same__1_10_11;
  };
  goto collect__same__1_10_11;
 case VARREF:
  deref_and_jump(a0,collect__same__1_10_1);
  *reasonp++ =  a0;
  goto collect__same__1_10_11;
  };
  goto collect__same__1_10_11;
 collect__same__1_10_11:
  otherwise(collect__same__1_10_interrupt);
  unify(a1, a0);
  x0 = NILATOM;
  unify_value(a5, x0);
  unify(a7, a6);
  proceed();
 collect__same__1_10_ext_interrupt:
  reasonp = 0l;
 collect__same__1_10_interrupt:
  goto interrupt_10;
 }

 rename__macro__preds_5_top: {
  q x0, x1, x2, x3, x4, x5, x6, x7, x8;
  a1 = qp->args[1];
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  qp = qp->next;
 rename__macro__preds_5_clear_reason:
  reasonp = reasons;
 rename__macro__preds_5_0:
 rename__macro__preds_5_1:
  switch (ptagof(a0)) {
 case CONS:
  x0 = car_of(a0);
 rename__macro__preds_5_2:
  switch (ptagof(x0)) {
 case FUNCTOR:
  if (functor_of(x0) != makesym(functor_expanded_2)) goto rename__macro__preds_5_interrupt;
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__kl1cmp_xrename__macro__preds_5);
  x1 = cdr_of(a0);
  allocp[2] = x1;
  allocp[3] = a1;
  allocp[4] = x2 = makeref(&allocp[4]);
  allocp[5] = x3 = makeref(&allocp[5]);
  allocp[6] = a4;
  allocp[7] = (q)(struct goalrec*)&allocp[0];
  allocp[8] = (q)(&predicate_integer__arithmetics_xadd_3);
  allocp[9] = a2;
  allocp[10] = makeint(1L);
  allocp[11] = x2;
  allocp[12] = (q)(struct goalrec*)&allocp[7];
  allocp[13] = (q)(&predicate_klic__comp__kl1cmp_xrename__macro__clauses_3);
  x4 = arg(x0, 0);
  allocp[14] = x4;
  allocp[15] = x3;
  allocp[16] = a3;
  allocp[17] = (q)(struct goalrec*)&allocp[12];
  allocp[18] = (q)(&predicate_klic__comp__util_xmake__atom_2);
  allocp[19] = x5 = makeref(&allocp[19]);
  x6 = arg(x0, 1);
  allocp[20] = x6;
  allocp[21] = x8 = makeref(&allocp[21]);
  allocp[22] = makeint(36L);
  x7 = makecons(&allocp[21]);
  allocp[23] = (q)(struct goalrec*)&allocp[17];
  allocp[24] = (q)(&predicate_klic__comp__util_xappend_3);
  allocp[25] = a1;
  allocp[26] = x7;
  allocp[27] = x5;
  allocp[28] = (q)(struct goalrec*)&allocp[23];
  allocp[29] = (q)(&predicate_klic__comp__util_xname_2);
  allocp[30] = a2;
  allocp[31] = x8;
  qp = (struct goalrec*)&allocp[28];
  allocp += 32;
  proceed();
 case VARREF:
  deref_and_jump(x0,rename__macro__preds_5_2);
  *reasonp++ =  x0;
  goto rename__macro__preds_5_interrupt;
  };
  goto rename__macro__preds_5_interrupt;
 case ATOMIC:
  if (a0 != NILATOM) goto rename__macro__preds_5_interrupt;
  unify(a4, a3);
  proceed();
 case VARREF:
  deref_and_jump(a0,rename__macro__preds_5_1);
  *reasonp++ =  a0;
  goto rename__macro__preds_5_interrupt;
  };
  goto rename__macro__preds_5_interrupt;
 rename__macro__preds_5_ext_interrupt:
  reasonp = 0l;
 rename__macro__preds_5_interrupt:
  goto interrupt_5;
 }

 rename__macro__clauses_3_top: {
  q x0, x1, x2, x3, x4, x5, x6, x7, x8;
  a1 = qp->args[1];
  a2 = qp->args[2];
  qp = qp->next;
 rename__macro__clauses_3_clear_reason:
  reasonp = reasons;
 rename__macro__clauses_3_0:
 rename__macro__clauses_3_1:
  switch (ptagof(a0)) {
 case CONS:
  x0 = car_of(a0);
 rename__macro__clauses_3_2:
  switch (ptagof(x0)) {
 case ATOMIC:
  if (isint(x0)) goto rename__macro__clauses_3_interrupt;
  switch (symval(x0)) {
 case symval(makesym(atom_otherwise)):
  allocp[0] = x2 = makeref(&allocp[0]);
  allocp[1] = makesym(atom_otherwise);
  x1 = makecons(&allocp[0]);
  allocp += 2;
  unify_value(a1, x1);
  a0 = cdr_of(a0);
  a1 = x2;
  execute(rename__macro__clauses_3_0);
  goto rename__macro__clauses_3_ext_interrupt;
 case symval(makesym(atom_alternatively)):
  allocp[0] = x2 = makeref(&allocp[0]);
  allocp[1] = makesym(atom_alternatively);
  x1 = makecons(&allocp[0]);
  allocp += 2;
  unify_value(a1, x1);
  a0 = cdr_of(a0);
  a1 = x2;
  execute(rename__macro__clauses_3_0);
  goto rename__macro__clauses_3_ext_interrupt;
  };
  goto rename__macro__clauses_3_interrupt;
 case FUNCTOR:
  if (functor_of(x0) != makesym(functor_clause_4)) goto rename__macro__clauses_3_interrupt;
  allocp[0] = makesym(functor_clause_4);
  allocp[1] = x2 = makeref(&allocp[1]);
  x3 = arg(x0, 1);
  allocp[2] = x3;
  x4 = arg(x0, 2);
  allocp[3] = x4;
  x5 = arg(x0, 3);
  allocp[4] = x5;
  x1 = makefunctor(&allocp[0]);
  allocp[5] = x7 = makeref(&allocp[5]);
  allocp[6] = x1;
  x6 = makecons(&allocp[5]);
  allocp += 7;
  unify_value(a1, x6);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__kl1cmp_xrename__macro__clauses_3);
  x8 = cdr_of(a0);
  allocp[2] = x8;
  allocp[3] = x7;
  allocp[4] = a2;
  a0 = arg(x0, 0);
  a1 = x2;
  qp = (struct goalrec*)&allocp[0];
  allocp += 5;
  execute(shift__vector_2_0);
  goto shift__vector_2_ext_interrupt;
 case VARREF:
  deref_and_jump(x0,rename__macro__clauses_3_2);
  *reasonp++ =  x0;
  goto rename__macro__clauses_3_interrupt;
  };
  goto rename__macro__clauses_3_interrupt;
 case ATOMIC:
  if (a0 != NILATOM) goto rename__macro__clauses_3_interrupt;
  unify(a1, a2);
  proceed();
 case VARREF:
  deref_and_jump(a0,rename__macro__clauses_3_1);
  *reasonp++ =  a0;
  goto rename__macro__clauses_3_interrupt;
  };
  goto rename__macro__clauses_3_interrupt;
 rename__macro__clauses_3_ext_interrupt:
  reasonp = 0l;
 rename__macro__clauses_3_interrupt:
  goto interrupt_3;
 }

 shift__vector_2_top: {
  q x0, x1, x2, x3, x4;
  a1 = qp->args[1];
  qp = qp->next;
 shift__vector_2_clear_reason:
  reasonp = reasons;
 shift__vector_2_0:
 shift__vector_2_1:
  if (isref(a0)) goto shift__vector_2_2;
  gblt_pfunctor(a0,x0,shift__vector_2_interrupt);
  gblt_arity(a0,x1,shift__vector_2_interrupt);
  x2 = makeint(1L);
  bblt_sub_no_check(x1,x2,x3);
  x4 = a0;
  a2 = a1;
  a1 = x4;
  a0 = x3;
  execute(shift__vector_3_0);
  goto shift__vector_3_ext_interrupt;
 shift__vector_2_2:
  deref_and_jump(a0,shift__vector_2_1);
  *reasonp++ =  a0;
  goto shift__vector_2_interrupt;
 shift__vector_2_ext_interrupt:
  reasonp = 0l;
 shift__vector_2_interrupt:
  toppred = &predicate_klic__comp__kl1cmp_xshift__vector_2;
  goto interrupt_2;
 }

 shift__vector_3_top: {
  q x0, x1, x2, x3, x4;
  a1 = qp->args[1];
  a2 = qp->args[2];
  qp = qp->next;
 shift__vector_3_clear_reason:
  reasonp = reasons;
 shift__vector_3_0:
 shift__vector_3_1:
  if (!isint(a0)) goto shift__vector_3_7;
  x0 = makeint(0L);
  gblt_greater(a0,x0,shift__vector_3_4);
 shift__vector_3_2:
  if (isref(a1)) goto shift__vector_3_3;
  x2 = makeint(1L);
  gblt_arg(x2,a1,x1,shift__vector_3_4);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__kl1cmp_xshift__vector_5);
  allocp[2] = makeint(1L);
  allocp[3] = a0;
  allocp[4] = a1;
  allocp[5] = x3 = makeref(&allocp[5]);
  allocp[6] = a2;
  allocp[7] = (q)(struct goalrec*)&allocp[0];
  allocp[8] = (q)(&predicate_functor__table_xnew__functor_3);
  allocp[9] = x3;
  allocp[10] = x1;
  allocp[11] = a0;
  qp = (struct goalrec*)&allocp[7];
  allocp += 12;
  proceed();
 shift__vector_3_3:
  deref_and_jump(a1,shift__vector_3_2);
  *reasonp++ =  a1;
  goto shift__vector_3_4;
 shift__vector_3_4:
  x1 = makeint(0L);
  gblt_eq(a0,x1,shift__vector_3_interrupt);
 shift__vector_3_5:
  if (isref(a1)) goto shift__vector_3_6;
  x3 = makeint(1L);
  gblt_arg(x3,a1,x2,shift__vector_3_interrupt);
  allocp[0] = makesym(functor_atom_1);
  allocp[1] = x2;
  x4 = makefunctor(&allocp[0]);
  allocp += 2;
  unify_value(a2, x4);
  proceed();
 shift__vector_3_6:
  deref_and_jump(a1,shift__vector_3_5);
  *reasonp++ =  a1;
  goto shift__vector_3_interrupt;
 shift__vector_3_7:
  if (!isref(a0)) goto shift__vector_3_interrupt;
  deref_and_jump(a0,shift__vector_3_1);
  *reasonp++ =  a0;
  goto shift__vector_3_interrupt;
 shift__vector_3_ext_interrupt:
  reasonp = 0l;
 shift__vector_3_interrupt:
  toppred = &predicate_klic__comp__kl1cmp_xshift__vector_3;
  goto interrupt_3;
 }

 shift__vector_5_top: {
  q x0, x1, x2, x3, x4, x5, x6;
  a1 = qp->args[1];
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  qp = qp->next;
 shift__vector_5_clear_reason:
  reasonp = reasons;
 shift__vector_5_0:
 shift__vector_5_1:
  if (!isint(a1)) goto shift__vector_5_6;
 shift__vector_5_2:
  if (!isint(a0)) goto shift__vector_5_5;
  gblt_greater_or_eq(a1,a0,shift__vector_5_7);
  x1 = makeint(1L);
  gblt_subtract(a0,x1,x0,shift__vector_5_7);
  x3 = makeint(1L);
  gblt_add(a0,x3,x2,shift__vector_5_7);
 shift__vector_5_3:
  if (isref(a2)) goto shift__vector_5_4;
  gblt_arg(x2,a2,x4,shift__vector_5_7);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__kl1cmp_xshift__vector_5);
  allocp[2] = x2;
  allocp[3] = a1;
  allocp[4] = a2;
  allocp[5] = x5 = makeref(&allocp[5]);
  allocp[6] = a4;
  allocp[7] = (q)(struct goalrec*)&allocp[0];
  allocp[8] = (q)(&predicate_functor__table_xsetarg_5);
  allocp[9] = a0;
  allocp[10] = a3;
  allocp[11] = x6 = makeref(&allocp[11]);
  allocp[12] = x4;
  allocp[13] = x5;
  qp = (struct goalrec*)&allocp[7];
  allocp += 14;
  proceed();
 shift__vector_5_4:
  deref_and_jump(a2,shift__vector_5_3);
  *reasonp++ =  a2;
  goto shift__vector_5_7;
 shift__vector_5_5:
  if (!isref(a0)) goto shift__vector_5_7;
  deref_and_jump(a0,shift__vector_5_2);
  *reasonp++ =  a0;
  goto shift__vector_5_7;
 shift__vector_5_6:
  if (!isref(a1)) goto shift__vector_5_7;
  deref_and_jump(a1,shift__vector_5_1);
  *reasonp++ =  a1;
  goto shift__vector_5_7;
 shift__vector_5_7:
  otherwise(shift__vector_5_interrupt);
  allocp[0] = makesym(functor_functor_1);
  allocp[1] = a3;
  x0 = makefunctor(&allocp[0]);
  allocp += 2;
  unify_value(a4, x0);
  proceed();
 shift__vector_5_ext_interrupt:
  reasonp = 0l;
 shift__vector_5_interrupt:
  goto interrupt_5;
 }

 handle__exception_1_top: {
  q x0, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13, x14, x15;
  qp = qp->next;
 handle__exception_1_clear_reason:
  reasonp = reasons;
 handle__exception_1_0:
 handle__exception_1_1:
  switch (ptagof(a0)) {
 case FUNCTOR:
  if (functor_of(a0) != makesym(functor_abnormal_1)) goto handle__exception_1_6;
  x0 = arg(a0, 0);
 handle__exception_1_2:
  switch (ptagof(x0)) {
 case FUNCTOR:
  switch (symval(functor_of(x0))) {
 case functor_error_2:
  a0 = arg(x0, 0);
  a1 = arg(x0, 1);
  a2 = NILATOM;
  execute(handle__exception_3_0);
  goto handle__exception_3_ext_interrupt;
 case functor_ambiguous_2:
  a0 = arg(x0, 0);
  a1 = arg(x0, 1);
  a2 = makecons(cons_const_25);
  execute(handle__exception_3_0);
  goto handle__exception_3_ext_interrupt;
 case functor_error_1:
  x1 = arg(x0, 0);
 handle__exception_1_3:
  switch (ptagof(x1)) {
 case FUNCTOR:
 handle__exception_1_4:
  if (!isgobj(x1)) goto handle__exception_1_6;
  if (!isclass(x1,vector)) goto handle__exception_1_6;
  gblt_size_of_vector(x1,x2,handle__exception_1_6);
 handle__exception_1_5:
  if (x2 != makeint(3L)) goto handle__exception_1_6;
  x4 = makeint(0L);
  gblt_element_of_vector(x1,x4,x3,handle__exception_1_6);
  x6 = makeint(1L);
  gblt_element_of_vector(x1,x6,x5,handle__exception_1_6);
  x8 = makeint(2L);
  gblt_element_of_vector(x1,x8,x7,handle__exception_1_6);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__message_xcompilation__error_1);
  allocp[2] = x9 = makeref(&allocp[2]);
  x10 = NILATOM;
  allocp[3] = (q)(struct goalrec*)&allocp[0];
  allocp[4] = (q)(&predicate_klic__comp__obj_xflush_3);
  allocp[5] = x9;
  allocp[6] = x11 = makeref(&allocp[6]);
  allocp[7] = x10;
  allocp[8] = NILATOM;
  allocp[9] = x7;
  x12 = makecons(&allocp[8]);
  allocp[10] = x12;
  allocp[11] = x5;
  x13 = makecons(&allocp[10]);
  allocp[12] = x13;
  allocp[13] = x3;
  x14 = makecons(&allocp[12]);
  allocp[14] = (q)(struct goalrec*)&allocp[3];
  allocp[15] = (q)(&predicate_klic__comp__obj_xklicformat_4);
  allocp[16] = makefunctor(&string_const_2);
  allocp[17] = x14;
  allocp[18] = x15 = makeref(&allocp[18]);
  allocp[19] = x11;
  allocp[20] = (q)(struct goalrec*)&allocp[14];
  allocp[21] = (q)(&predicate_klic__comp__message_xtell_2);
  allocp[22] = makesym(atom_user__error);
  allocp[23] = x15;
  qp = (struct goalrec*)&allocp[20];
  allocp += 24;
  proceed();
 case VARREF:
  deref_and_jump(x1,handle__exception_1_3);
  *reasonp++ =  x1;
  goto handle__exception_1_6;
  };
  goto handle__exception_1_6;
  };
  goto handle__exception_1_6;
 case VARREF:
  deref_and_jump(x0,handle__exception_1_2);
  *reasonp++ =  x0;
  goto handle__exception_1_6;
  };
  goto handle__exception_1_6;
 case VARREF:
  deref_and_jump(a0,handle__exception_1_1);
  *reasonp++ =  a0;
  goto handle__exception_1_6;
  };
  goto handle__exception_1_6;
 handle__exception_1_6:
  otherwise(handle__exception_1_interrupt);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__message_xcompilation__error_1);
  allocp[2] = x0 = makeref(&allocp[2]);
  allocp[3] = (q)(struct goalrec*)&allocp[0];
  allocp[4] = (q)(&predicate_klic__comp__obj_xflush_3);
  allocp[5] = x0;
  allocp[6] = x1 = makeref(&allocp[6]);
  allocp[7] = NILATOM;
  allocp[8] = NILATOM;
  allocp[9] = a0;
  x2 = makecons(&allocp[8]);
  allocp[10] = (q)(struct goalrec*)&allocp[3];
  allocp[11] = (q)(&predicate_klic__comp__obj_xklicformat_4);
  allocp[12] = makefunctor(&string_const_18);
  allocp[13] = x2;
  allocp[14] = x3 = makeref(&allocp[14]);
  allocp[15] = x1;
  allocp[16] = (q)(struct goalrec*)&allocp[10];
  allocp[17] = (q)(&predicate_klic__comp__message_xtell_2);
  allocp[18] = makesym(atom_user__error);
  allocp[19] = x3;
  qp = (struct goalrec*)&allocp[16];
  allocp += 20;
  proceed();
 handle__exception_1_ext_interrupt:
  reasonp = 0l;
 handle__exception_1_interrupt:
  toppred = &predicate_klic__comp__kl1cmp_xhandle__exception_1;
  goto interrupt_1;
 }

 handle__exception_3_top: {
  q x0, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10;
  a1 = qp->args[1];
  a2 = qp->args[2];
  qp = qp->next;
 handle__exception_3_clear_reason:
  reasonp = reasons;
 handle__exception_3_0:
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__message_xcompilation__error_1);
  allocp[2] = x0 = makeref(&allocp[2]);
  x1 = NILATOM;
  allocp[3] = (q)(struct goalrec*)&allocp[0];
  allocp[4] = (q)(&predicate_klic__comp__obj_xflush_3);
  allocp[5] = x0;
  allocp[6] = x2 = makeref(&allocp[6]);
  allocp[7] = x1;
  allocp[8] = x2;
  allocp[9] = makeint(10L);
  x3 = makecons(&allocp[8]);
  allocp[10] = (q)(struct goalrec*)&allocp[3];
  allocp[11] = (q)(&predicate_klic__comp__kl1cmp_xcomplain__tokens_3);
  allocp[12] = x4 = makeref(&allocp[12]);
  allocp[13] = x5 = makeref(&allocp[13]);
  allocp[14] = x3;
  allocp[15] = (q)(struct goalrec*)&allocp[10];
  allocp[16] = (q)(&predicate_klic__comp__obj_xklicformat_3);
  allocp[17] = makefunctor(&string_const_1);
  allocp[18] = x6 = makeref(&allocp[18]);
  allocp[19] = x5;
  allocp[20] = (q)(struct goalrec*)&allocp[15];
  allocp[21] = (q)(&predicate_klic__comp__kl1cmp_xcomplain__tokens_6);
  allocp[22] = makeint(0L);
  allocp[23] = a0;
  allocp[24] = a1;
  allocp[25] = x4;
  allocp[26] = x7 = makeref(&allocp[26]);
  allocp[27] = x6;
  allocp[28] = x7;
  allocp[29] = makeint(10L);
  x8 = makecons(&allocp[28]);
  allocp[30] = (q)(struct goalrec*)&allocp[20];
  allocp[31] = (q)(&predicate_klic__comp__kl1cmp_xcomplain__words_3);
  allocp[32] = a2;
  allocp[33] = x9 = makeref(&allocp[33]);
  allocp[34] = x8;
  allocp[35] = (q)(struct goalrec*)&allocp[30];
  allocp[36] = (q)(&predicate_klic__comp__obj_xklicformat_3);
  allocp[37] = makefunctor(&string_const_13);
  allocp[38] = x10 = makeref(&allocp[38]);
  allocp[39] = x9;
  allocp[40] = (q)(struct goalrec*)&allocp[35];
  allocp[41] = (q)(&predicate_klic__comp__message_xtell_2);
  allocp[42] = makesym(atom_user__error);
  allocp[43] = x10;
  qp = (struct goalrec*)&allocp[40];
  allocp += 44;
  proceed();
 handle__exception_3_ext_interrupt:
  reasonp = 0l;
 handle__exception_3_interrupt:
  toppred = &predicate_klic__comp__kl1cmp_xhandle__exception_3;
  goto interrupt_3;
 }

 complain__tokens_6_top: {
  q x0, x1, x2;
  a1 = qp->args[1];
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  qp = qp->next;
 complain__tokens_6_clear_reason:
  reasonp = reasons;
 complain__tokens_6_0:
 complain__tokens_6_1:
  if (isref(a0)) goto complain__tokens_6_4;
 complain__tokens_6_2:
  if (isref(a0)) goto complain__tokens_6_3;
  if_not_equal(a0, a1, complain__tokens_6_5);
  unify2(a3, a2, a5, a4);
  proceed();
 complain__tokens_6_3:
  deref_and_jump(a0,complain__tokens_6_2);
  *reasonp++ =  a0;
  goto complain__tokens_6_5;
 complain__tokens_6_4:
  deref_and_jump(a0,complain__tokens_6_1);
  *reasonp++ =  a0;
  goto complain__tokens_6_5;
 complain__tokens_6_5:
  otherwise(complain__tokens_6_interrupt);
 complain__tokens_6_6:
  switch (ptagof(a2)) {
 case CONS:
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__kl1cmp_xcomplain__tokens_6);
  allocp[2] = x0 = makeref(&allocp[2]);
  allocp[3] = a1;
  x1 = cdr_of(a2);
  allocp[4] = x1;
  allocp[5] = a3;
  allocp[6] = x2 = makeref(&allocp[6]);
  allocp[7] = a5;
  allocp[8] = (q)(struct goalrec*)&allocp[0];
  allocp[9] = (q)(&predicate_integer__arithmetics_xadd_3);
  allocp[10] = a0;
  allocp[11] = makeint(1L);
  allocp[12] = x0;
  a0 = car_of(a2);
  a1 = a4;
  a2 = x2;
  qp = (struct goalrec*)&allocp[8];
  allocp += 13;
  execute(complain__token_3_0);
  goto complain__token_3_ext_interrupt;
 case VARREF:
  deref_and_jump(a2,complain__tokens_6_6);
  *reasonp++ =  a2;
  goto complain__tokens_6_interrupt;
  };
  goto complain__tokens_6_interrupt;
 complain__tokens_6_ext_interrupt:
  reasonp = 0l;
 complain__tokens_6_interrupt:
  goto interrupt_6;
 }

 complain__tokens_3_top: {
  q x0, x1;
  a1 = qp->args[1];
  a2 = qp->args[2];
  qp = qp->next;
 complain__tokens_3_clear_reason:
  reasonp = reasons;
 complain__tokens_3_0:
 complain__tokens_3_1:
  switch (ptagof(a0)) {
 case CONS:
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__kl1cmp_xcomplain__tokens_3);
  x0 = cdr_of(a0);
  allocp[2] = x0;
  allocp[3] = x1 = makeref(&allocp[3]);
  allocp[4] = a2;
  a0 = car_of(a0);
  a2 = x1;
  qp = (struct goalrec*)&allocp[0];
  allocp += 5;
  execute(complain__token_3_0);
  goto complain__token_3_ext_interrupt;
 case ATOMIC:
  if (a0 != NILATOM) goto complain__tokens_3_interrupt;
  unify(a2, a1);
  proceed();
 case VARREF:
  deref_and_jump(a0,complain__tokens_3_1);
  *reasonp++ =  a0;
  goto complain__tokens_3_interrupt;
  };
  goto complain__tokens_3_interrupt;
 complain__tokens_3_ext_interrupt:
  reasonp = 0l;
 complain__tokens_3_interrupt:
  goto interrupt_3;
 }

 complain__token_3_top: {
  q x0, x1;
  a1 = qp->args[1];
  a2 = qp->args[2];
  qp = qp->next;
 complain__token_3_clear_reason:
  reasonp = reasons;
 complain__token_3_0:
 complain__token_3_1:
  switch (ptagof(a0)) {
 case ATOMIC:
  if (a0 != makesym(atom_end)) goto complain__token_3_2;
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__obj_xklicformat_3);
  allocp[2] = makefunctor(&string_const_3);
  allocp[3] = a1;
  allocp[4] = a2;
  qp = (struct goalrec*)&allocp[0];
  allocp += 5;
  proceed();
 case FUNCTOR:
  switch (symval(functor_of(a0))) {
 case functor_atom_1:
  allocp[0] = NILATOM;
  x1 = arg(a0, 0);
  allocp[1] = x1;
  x0 = makecons(&allocp[0]);
  allocp[2] = (q)qp;
  allocp[3] = (q)(&predicate_klic__comp__obj_xklicformat_4);
  allocp[4] = makefunctor(&string_const_17);
  allocp[5] = x0;
  allocp[6] = a1;
  allocp[7] = a2;
  qp = (struct goalrec*)&allocp[2];
  allocp += 8;
  proceed();
 case functor_integer_1:
  allocp[0] = NILATOM;
  x1 = arg(a0, 0);
  allocp[1] = x1;
  x0 = makecons(&allocp[0]);
  allocp[2] = (q)qp;
  allocp[3] = (q)(&predicate_klic__comp__obj_xklicformat_4);
  allocp[4] = makefunctor(&string_const_17);
  allocp[5] = x0;
  allocp[6] = a1;
  allocp[7] = a2;
  qp = (struct goalrec*)&allocp[2];
  allocp += 8;
  proceed();
 case functor_string_1:
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__kl1cmp_xcomplain__string_3);
  allocp[2] = x0 = makeref(&allocp[2]);
  allocp[3] = a1;
  allocp[4] = a2;
  allocp[5] = (q)(struct goalrec*)&allocp[0];
  allocp[6] = (q)(&predicate_klic__comp__util_xstring__to__char__list_2);
  x1 = arg(a0, 0);
  allocp[7] = x1;
  allocp[8] = x0;
  qp = (struct goalrec*)&allocp[5];
  allocp += 9;
  proceed();
 case functor__24VAR_2:
  allocp[0] = NILATOM;
  x1 = arg(a0, 1);
  allocp[1] = x1;
  x0 = makecons(&allocp[0]);
  allocp[2] = (q)qp;
  allocp[3] = (q)(&predicate_klic__comp__obj_xklicformat_4);
  allocp[4] = makefunctor(&string_const_16);
  allocp[5] = x0;
  allocp[6] = a1;
  allocp[7] = a2;
  qp = (struct goalrec*)&allocp[2];
  allocp += 8;
  proceed();
 case functor_open_1:
  allocp[0] = NILATOM;
  x1 = arg(a0, 0);
  allocp[1] = x1;
  x0 = makecons(&allocp[0]);
  allocp[2] = (q)qp;
  allocp[3] = (q)(&predicate_klic__comp__obj_xklicformat_4);
  allocp[4] = makefunctor(&string_const_19);
  allocp[5] = x0;
  allocp[6] = a1;
  allocp[7] = a2;
  qp = (struct goalrec*)&allocp[2];
  allocp += 8;
  proceed();
  };
  goto complain__token_3_2;
 case VARREF:
  deref_and_jump(a0,complain__token_3_1);
  *reasonp++ =  a0;
  goto complain__token_3_2;
  };
  goto complain__token_3_2;
 complain__token_3_2:
  otherwise(complain__token_3_interrupt);
  allocp[0] = NILATOM;
  allocp[1] = a0;
  x0 = makecons(&allocp[0]);
  allocp[2] = (q)qp;
  allocp[3] = (q)(&predicate_klic__comp__obj_xklicformat_4);
  allocp[4] = makefunctor(&string_const_17);
  allocp[5] = x0;
  allocp[6] = a1;
  allocp[7] = a2;
  qp = (struct goalrec*)&allocp[2];
  allocp += 8;
  proceed();
 complain__token_3_ext_interrupt:
  reasonp = 0l;
 complain__token_3_interrupt:
  toppred = &predicate_klic__comp__kl1cmp_xcomplain__token_3;
  goto interrupt_3;
 }

 complain__string_3_top: {
  q x0, x1, x2, x3;
  a1 = qp->args[1];
  a2 = qp->args[2];
  qp = qp->next;
 complain__string_3_clear_reason:
  reasonp = reasons;
 complain__string_3_0:
 complain__string_3_1:
  switch (ptagof(a0)) {
 case CONS:
  x0 = car_of(a0);
 complain__string_3_2:
  switch (ptagof(x0)) {
 case ATOMIC:
 complain__string_3_3:
  if (x0 != makeint(34L)) goto complain__string_3_4;
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__kl1cmp_xcomplain__string_3);
  x1 = cdr_of(a0);
  allocp[2] = x1;
  allocp[3] = x2 = makeref(&allocp[3]);
  allocp[4] = a2;
  allocp[5] = (q)(struct goalrec*)&allocp[0];
  allocp[6] = (q)(&predicate_klic__comp__obj_xklicformat_3);
  allocp[7] = makefunctor(&string_const_14);
  allocp[8] = a1;
  allocp[9] = x2;
  qp = (struct goalrec*)&allocp[5];
  allocp += 10;
  proceed();
 case VARREF:
  deref_and_jump(x0,complain__string_3_2);
  *reasonp++ =  x0;
  goto complain__string_3_4;
  };
  goto complain__string_3_4;
 case ATOMIC:
  if (a0 != NILATOM) goto complain__string_3_4;
  unify(a2, a1);
  proceed();
 case VARREF:
  deref_and_jump(a0,complain__string_3_1);
  *reasonp++ =  a0;
  goto complain__string_3_4;
  };
  goto complain__string_3_4;
 complain__string_3_4:
  otherwise(complain__string_3_interrupt);
 complain__string_3_5:
  switch (ptagof(a0)) {
 case CONS:
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__kl1cmp_xcomplain__string_3);
  x0 = cdr_of(a0);
  allocp[2] = x0;
  allocp[3] = x1 = makeref(&allocp[3]);
  allocp[4] = a2;
  allocp[5] = NILATOM;
  x3 = car_of(a0);
  allocp[6] = x3;
  x2 = makecons(&allocp[5]);
  allocp[7] = (q)(struct goalrec*)&allocp[0];
  allocp[8] = (q)(&predicate_klic__comp__obj_xklicformat_4);
  allocp[9] = makefunctor(&string_const_15);
  allocp[10] = x2;
  allocp[11] = a1;
  allocp[12] = x1;
  qp = (struct goalrec*)&allocp[7];
  allocp += 13;
  proceed();
 case VARREF:
  deref_and_jump(a0,complain__string_3_5);
  *reasonp++ =  a0;
  goto complain__string_3_interrupt;
  };
  goto complain__string_3_interrupt;
 complain__string_3_ext_interrupt:
  reasonp = 0l;
 complain__string_3_interrupt:
  goto interrupt_3;
 }

 complain__words_3_top: {
  q x0, x1, x2, x3;
  a1 = qp->args[1];
  a2 = qp->args[2];
  qp = qp->next;
 complain__words_3_clear_reason:
  reasonp = reasons;
 complain__words_3_0:
 complain__words_3_1:
  switch (ptagof(a0)) {
 case CONS:
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__kl1cmp_xcomplain__words_3);
  x0 = cdr_of(a0);
  allocp[2] = x0;
  allocp[3] = x1 = makeref(&allocp[3]);
  allocp[4] = a2;
  allocp[5] = makecons(cons_const_21);
  x3 = car_of(a0);
  allocp[6] = x3;
  x2 = makecons(&allocp[5]);
  allocp[7] = (q)(struct goalrec*)&allocp[0];
  allocp[8] = (q)(&predicate_klic__comp__obj_xklicformat_4);
  allocp[9] = makefunctor(&string_const_20);
  allocp[10] = x2;
  allocp[11] = a1;
  allocp[12] = x1;
  qp = (struct goalrec*)&allocp[7];
  allocp += 13;
  proceed();
 case ATOMIC:
  if (a0 != NILATOM) goto complain__words_3_interrupt;
  unify(a2, a1);
  proceed();
 case VARREF:
  deref_and_jump(a0,complain__words_3_1);
  *reasonp++ =  a0;
  goto complain__words_3_interrupt;
  };
  goto complain__words_3_interrupt;
 complain__words_3_ext_interrupt:
  reasonp = 0l;
 complain__words_3_interrupt:
  goto interrupt_3;
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
  loop_within_module(module_klic__comp__kl1cmp);
}
