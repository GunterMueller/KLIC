/* Compiled by KLIC compiler version 3.003 (Mon Mar 29 19:42:59 JST 1999) */
#include <klic/klichdr.h>
#include "atom.h"
#include "funct.h"
#include <ctype.h>

module module_klic__comp__write();
Const struct predicate predicate_klic__comp__write_xwrite__out_3 =
   { module_klic__comp__write, 0, 3 };
Const struct predicate predicate_klic__comp__write_xdeclare__const_5 =
   { module_klic__comp__write, 1, 5 };
Const struct predicate predicate_klic__comp__write_xdeclare__const_6 =
   { module_klic__comp__write, 2, 6 };
Const struct predicate predicate_klic__comp__write_xdeclare__const__list_6 =
   { module_klic__comp__write, 3, 6 };
Const struct predicate predicate_klic__comp__write_xwrite__const__elems_3 =
   { module_klic__comp__write, 4, 3 };
Const struct predicate predicate_klic__comp__write_xwrite__const__elem_3 =
   { module_klic__comp__write, 5, 3 };
Const struct predicate predicate_klic__comp__write_xwrite__string__body_4 =
   { module_klic__comp__write, 6, 4 };
Const struct predicate predicate_klic__comp__write_xwrite__character__constants_5 =
   { module_klic__comp__write, 7, 5 };
Const struct predicate predicate_klic__comp__write_xwrite__string__elements_5 =
   { module_klic__comp__write, 8, 5 };
Const struct predicate predicate_klic__comp__write_xwrite__string__element_3 =
   { module_klic__comp__write, 9, 3 };
Const struct predicate predicate_klic__comp__write_xwrite__pred__structs_4 =
   { module_klic__comp__write, 10, 4 };
Const struct predicate predicate_klic__comp__write_xdeclare__externals_3 =
   { module_klic__comp__write, 11, 3 };
Const struct predicate predicate_klic__comp__write_xdeclare__one__external_3 =
   { module_klic__comp__write, 12, 3 };
Const struct predicate predicate_klic__comp__write_xpred__min__max_5 =
   { module_klic__comp__write, 13, 5 };
Const struct predicate predicate_klic__comp__write_xdeclare__registers_4 =
   { module_klic__comp__write, 14, 4 };
Const struct predicate predicate_klic__comp__write_xwrite__comma__list_5 =
   { module_klic__comp__write, 15, 5 };
Const struct predicate predicate_klic__comp__write_xwrite__comma__list1_5 =
   { module_klic__comp__write, 16, 5 };
Const struct predicate predicate_klic__comp__write_xwrite__pred__dispatch_3 =
   { module_klic__comp__write, 17, 3 };
Const struct predicate predicate_klic__comp__write_xwrite__predicates_6 =
   { module_klic__comp__write, 18, 6 };
Const struct predicate predicate_klic__comp__write_xwrite__predicates__1_6 =
   { module_klic__comp__write, 19, 6 };
Const struct predicate predicate_klic__comp__write_xwrite__get__args_4 =
   { module_klic__comp__write, 20, 4 };
Const struct predicate predicate_klic__comp__write_xwrite__interrupt__call_4 =
   { module_klic__comp__write, 21, 4 };
Const struct predicate predicate_klic__comp__write_xwrite__header__file_3 =
   { module_klic__comp__write, 22, 3 };
Const struct predicate predicate_klic__comp__write_xwrite__ext__entries_3 =
   { module_klic__comp__write, 23, 3 };
Const struct predicate predicate_klic__comp__write_xwrite__ext__entry_3 =
   { module_klic__comp__write, 24, 3 };
Const struct predicate predicate_klic__comp__write_xwrite__defined__preds_4 =
   { module_klic__comp__write, 25, 4 };
Const struct predicate predicate_klic__comp__write_xwrite__atom_3 =
   { module_klic__comp__write, 26, 3 };
Const struct predicate predicate_klic__comp__write_xwrite__funct__name_4 =
   { module_klic__comp__write, 27, 4 };
Const struct predicate predicate_klic__comp__write_xwrite__pred__name_5 =
   { module_klic__comp__write, 28, 5 };
Const struct predicate predicate_klic__comp__write_xsort__info_2 =
   { module_klic__comp__write, 29, 2 };
Const struct predicate predicate_klic__comp__write_xsort__info_3 =
   { module_klic__comp__write, 30, 3 };
Const struct predicate predicate_klic__comp__write_xpartition_4 =
   { module_klic__comp__write, 31, 4 };
extern Const struct predicate predicate_integer__arithmetics_xadd_3;
extern Const struct predicate predicate_klic__comp__obj_xklicformat_3;
extern Const struct predicate predicate_klic__comp__obj_xklicformat_4;
extern Const struct predicate predicate_klic__comp__obj_xwrite__object_4;
extern Const struct predicate predicate_klic__comp__util_xvector__to__list_2;
extern Const struct predicate predicate_klic__comp__util_xuniv_2;
extern Const struct predicate predicate_klic__comp__util_xmin_3;
extern Const struct predicate predicate_klic__comp__util_xmax_3;
extern Const struct predicate predicate_klic__comp__util_xmember_3;
extern Const struct predicate predicate_klic__comp__util_xqsort_2;
extern Const struct predicate predicate_klic__comp__extern_xmake__ext__table_3;
declare_method_table_of(vector);

module module_klic__comp__write(glbl, qp, allocp, toppred)
  struct global_variables *glbl;
  struct goalrec *qp;
  register q *allocp;
  Const struct predicate *toppred;
{
  static Const unsigned char strconst_body_0[28] =
    {'\n',' ',' ','q',' ','*','r','e','a','s','o','n','p',';','\n',' ','m','o','d','u','l','e','_','t','o','p',':','\n',};
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_0 =
      declare_string_constant_8(strconst_body_0,28);
  static Const unsigned char strconst_body_1[32] =
    "    case_pred(~d, ~q_~d_top);\n";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_1 =
      declare_string_constant_8(strconst_body_1,30);
  static Const unsigned char strconst_body_2[36] =
    "    last_case_pred(~d, ~q_~d_top);\n";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_2 =
      declare_string_constant_8(strconst_body_2,35);
  static Const unsigned char strconst_body_3[32] =
    "    makecons(cons_const_~d),\n";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_3 =
      declare_string_constant_8(strconst_body_3,29);
  static Const unsigned char strconst_body_4[32] =
    "    makefloat(float_const_~d),\n";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_4 =
      declare_string_constant_8(strconst_body_4,31);
  static Const unsigned char strconst_body_5[36] =
    "    makefunctor(&string_const_~d),\n";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_5 =
      declare_string_constant_8(strconst_body_5,35);
  static Const unsigned char strconst_body_6[36] =
    "    makefunctor(&vector_const_~d),\n";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_6 =
      declare_string_constant_8(strconst_body_6,35);
  static Const unsigned char strconst_body_7[36] =
    "    makefunctor(funct_const_~d),\n";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_7 =
      declare_string_constant_8(strconst_body_7,33);
  static Const unsigned char strconst_body_8[20] =
    "    makeint(~d),\n";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_8 =
      declare_string_constant_8(strconst_body_8,17);
  static Const unsigned char strconst_body_9[28] =
    {' ',' ',' ',' ','m','a','k','e','p','r','e','d','(','m','o','d','_','c','o','n','s','t','_','~','d',')',',','\n',};
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_9 =
      declare_string_constant_8(strconst_body_9,28);
  static Const unsigned char strconst_body_10[32] =
    "    makepred(pred_const_~d),\n";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_10 =
      declare_string_constant_8(strconst_body_10,29);
  static Const unsigned char strconst_body_11[8] =
    {' ',' ',' ',' ','~','a',',','\n',};
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_11 =
      declare_string_constant_8(strconst_body_11,8);
  static Const unsigned char strconst_body_12[24] =
    "  a~d = qp->args[~d];\n";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_12 =
      declare_string_constant_8(strconst_body_12,22);
  static Const unsigned char strconst_body_13[140] =
    "  declare_method_table_of(byte__string);\n  static Const string_structure_type_~d string_const_~d =\n      declare_string_constant_~d(0,0);\n";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_13 =
      declare_string_constant_8(strconst_body_13,138);
  static Const unsigned char strconst_body_14[156] =
    "  declare_method_table_of(byte__string);\n  static Const string_structure_type_~d string_const_~d =\n      declare_string_constant_~d(strconst_body_~d,~d);\n";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_14 =
      declare_string_constant_8(strconst_body_14,154);
  static Const unsigned char strconst_body_15[120] =
    "  declare_method_table_of(float);\n  static Const float_structure_type float_const_~d =\n    declare_float_constant(~g);\n";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_15 =
      declare_string_constant_8(strconst_body_15,119);
  static Const unsigned char strconst_body_16[160] =
    {' ',' ','d','e','c','l','a','r','e','_','m','e','t','h','o','d','_','t','a','b','l','e','_','o','f','(','m','o','d','u','l','e',')',';','\n',' ',' ','e','x','t','e','r','n',' ','m','o','d','u','l','e',' ','m','o','d','u','l','e','_','~','q','(',')',';','\n',' ',' ','s','t','a','t','i','c',' ','C','o','n','s','t',' ','m','o','d','u','l','e','_','s','t','r','u','c','t','u','r','e','_','t','y','p','e',' ','m','o','d','_','c','o','n','s','t','_','~','d',' ','=','\n',' ',' ',' ',' ','d','e','c','l','a','r','e','_','m','o','d','u','l','e','_','c','o','n','s','t','a','n','t','(','m','o','d','u','l','e','_','~','q',',',' ','~','a',')',';','\n',};
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_16 =
      declare_string_constant_8(strconst_body_16,160);
  static Const unsigned char strconst_body_17[144] =
    "  declare_method_table_of(predicate);\n  static Const predicate_structure_type pred_const_~d =\n    declare_pred_constant(~p, mod_const_~d, ~a);\n";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_17 =
      declare_string_constant_8(strconst_body_17,143);
  static Const unsigned char strconst_body_18[128] =
    "  declare_method_table_of(vector);\n  static Const vector_structure_type vector_const_~d =\n      declare_vector_constant(0,0);\n";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_18 =
      declare_string_constant_8(strconst_body_18,126);
  static Const unsigned char strconst_body_19[144] =
    "  declare_method_table_of(vector);\n  static Const vector_structure_type vector_const_~d =\n      declare_vector_constant(vectconst_body_~d,~d);\n";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_19 =
      declare_string_constant_8(strconst_body_19,143);
  static Const unsigned char strconst_body_20[24] =
    {' ',' ','g','o','t','o',' ','i','n','t','e','r','r','u','p','t','_','~','d',';','\n',' ','}','\n',};
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_20 =
      declare_string_constant_8(strconst_body_20,24);
  static Const unsigned char strconst_body_21[4] =
    {' ',' ','q',' ',};
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_21 =
      declare_string_constant_8(strconst_body_21,4);
  static Const unsigned char strconst_body_22[60] =
    "  qp = qp->next;\n ~q_~d_clear_reason:\n  reasonp = reasons;\n";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_22 =
      declare_string_constant_8(strconst_body_22,59);
  static Const unsigned char strconst_body_23[40] =
    "  static Const q cons_const_~d[] = {\n";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_23 =
      declare_string_constant_8(strconst_body_23,37);
  static Const unsigned char strconst_body_24[56] =
    "  static Const q funct_const_~d[] = {\n    makesym(~f),\n";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_24 =
      declare_string_constant_8(strconst_body_24,55);
  static Const unsigned char strconst_body_25[44] =
    "  static Const q vectconst_body_~d[] = {\n";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_25 =
      declare_string_constant_8(strconst_body_25,41);
  static Const unsigned char strconst_body_26[56] =
    {' ',' ','s','t','a','t','i','c',' ','C','o','n','s','t',' ','u','n','s','i','g','n','e','d',' ','c','h','a','r',' ','s','t','r','c','o','n','s','t','_','b','o','d','y','_','~','d','[','~','d',']',' ','=','\n',' ',' ',' ',' ',};
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_26 =
      declare_string_constant_8(strconst_body_26,56);
  static Const unsigned char strconst_body_27[24] =
    "  switch_on_pred() {\n";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_27 =
      declare_string_constant_8(strconst_body_27,21);
  static Const unsigned char strconst_body_28[20] =
    "  toppred = &~p;\n";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_28 =
      declare_string_constant_8(strconst_body_28,17);
  static Const unsigned char strconst_body_29[4] =
    {' ',' ','}','\n',};
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_29 =
      declare_string_constant_8(strconst_body_29,4);
  static Const unsigned char strconst_body_30[8] =
    "  };\n";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_30 =
      declare_string_constant_8(strconst_body_30,5);
  static Const unsigned char strconst_body_31[68] =
    " interrupt_0:\n  allocp = interrupt_goal(allocp, toppred, reasonp);\n";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_31 =
      declare_string_constant_8(strconst_body_31,67);
  static Const unsigned char strconst_body_32[36] =
    " interrupt_~d:\n  allocp[~d] = a~d;\n";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_32 =
      declare_string_constant_8(strconst_body_32,35);
  static Const unsigned char strconst_body_33[52] =
    " proceed_label:\n  loop_within_module(module_~q);\n}\n";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_33 =
      declare_string_constant_8(strconst_body_33,51);
  static Const unsigned char strconst_body_34[56] =
    {' ','~','q','_','~','d','_','e','x','t','_','i','n','t','e','r','r','u','p','t',':','\n',' ',' ','r','e','a','s','o','n','p',' ','=',' ','0','l',';','\n',' ','~','q','_','~','d','_','i','n','t','e','r','r','u','p','t',':','\n',};
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_34 =
      declare_string_constant_8(strconst_body_34,56);
  static Const unsigned char strconst_body_35[16] =
    " ~q_~d_top: {\n";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_35 =
      declare_string_constant_8(strconst_body_35,14);
  static Const unsigned char strconst_body_36[56] =
    "Const struct predicate ~p =\n   { module_~q, ~d, ~d };\n";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_36 =
      declare_string_constant_8(strconst_body_36,54);
  static Const unsigned char strconst_body_37[4] =
    "a";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_37 =
      declare_string_constant_8(strconst_body_37,1);
  static Const unsigned char strconst_body_38[8] =
    {'a','t','o','m','_','~','q','\n',};
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_38 =
      declare_string_constant_8(strconst_body_38,8);
  static Const unsigned char strconst_body_39[32] =
    "declare_method_table_of(~q);\n";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_39 =
      declare_string_constant_8(strconst_body_39,29);
  static Const unsigned char strconst_body_40[36] =
    "extern Const struct predicate ~p;\n";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_40 =
      declare_string_constant_8(strconst_body_40,34);
  static Const unsigned char strconst_body_41[24] =
    "extern q ~q_g_new();\n";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_41 =
      declare_string_constant_8(strconst_body_41,21);
  static Const unsigned char strconst_body_42[16] =
    "functor_~q_~d";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_42 =
      declare_string_constant_8(strconst_body_42,13);
  static Const unsigned char strconst_body_43[12] =
    {'m','a','k','e','i','n','t','(','~','d','L',')',};
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_43 =
      declare_string_constant_8(strconst_body_43,12);
  static Const unsigned char strconst_body_44[16] =
    {'m','a','k','e','s','y','m','(','a','t','o','m','_','~','q',')',};
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_44 =
      declare_string_constant_8(strconst_body_44,16);
  static Const unsigned char strconst_body_45[20] =
    {'m','o','d','u','l','e',' ','m','o','d','u','l','e','_','~','q','(',')',';','\n',};
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_45 =
      declare_string_constant_8(strconst_body_45,20);
  static Const unsigned char strconst_body_46[160] =
    "module module_~q(glbl, qp, allocp, toppred)\n  struct global_variables *glbl;\n  struct goalrec *qp;\n  register q *allocp;\n  Const struct predicate *toppred;\n{\n";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_46 =
      declare_string_constant_8(strconst_body_46,158);
  static Const unsigned char strconst_body_47[12] =
    "module_~q\n";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_47 =
      declare_string_constant_8(strconst_body_47,10);
  static Const unsigned char strconst_body_48[20] =
    "predicate_~q_x~q_~d";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_48 =
      declare_string_constant_8(strconst_body_48,19);
  static Const unsigned char strconst_body_49[16] =
    "ref_class_~q\n";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_49 =
      declare_string_constant_8(strconst_body_49,13);
  static Const unsigned char strconst_body_50[16] =
    "ref_module_~q\n";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_50 =
      declare_string_constant_8(strconst_body_50,14);
  static Const unsigned char strconst_body_51[4] =
    "x";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_51 =
      declare_string_constant_8(strconst_body_51,1);
  static Const unsigned char strconst_body_52[4] =
    "~p\n";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_52 =
      declare_string_constant_8(strconst_body_52,3);
  static Const unsigned char strconst_body_53[4] =
    "~s";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_53 =
      declare_string_constant_8(strconst_body_53,2);
  static Const unsigned char strconst_body_54[4] =
    {'~','s','~','d',};
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_54 =
      declare_string_constant_8(strconst_body_54,4);
  static Const unsigned char strconst_body_55[8] =
    "~s~d, ";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_55 =
      declare_string_constant_8(strconst_body_55,6);
  static Const unsigned char strconst_body_56[4] =
    "\n";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_56 =
      declare_string_constant_8(strconst_body_56,1);
  static Const q funct_const_57[] = {
    makesym(functor_fwrite_1),
    makefunctor(&string_const_56),
  };
  static Const unsigned char strconst_body_58[4] =
    " ";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_58 =
      declare_string_constant_8(strconst_body_58,1);
  static Const q funct_const_59[] = {
    makesym(functor_fwrite_1),
    makefunctor(&string_const_58),
  };
  static Const unsigned char strconst_body_60[8] =
    "  };\n";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_60 =
      declare_string_constant_8(strconst_body_60,5);
  static Const q funct_const_61[] = {
    makesym(functor_fwrite_1),
    makefunctor(&string_const_60),
  };
  static Const unsigned char strconst_body_62[4] =
    "\"";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_62 =
      declare_string_constant_8(strconst_body_62,1);
  static Const q funct_const_63[] = {
    makesym(functor_fwrite_1),
    makefunctor(&string_const_62),
  };
  static Const unsigned char strconst_body_64[4] =
    "\'";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_64 =
      declare_string_constant_8(strconst_body_64,1);
  static Const q funct_const_65[] = {
    makesym(functor_fwrite_1),
    makefunctor(&string_const_64),
  };
  static Const unsigned char strconst_body_66[4] =
    "\',";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_66 =
      declare_string_constant_8(strconst_body_66,2);
  static Const q funct_const_67[] = {
    makesym(functor_fwrite_1),
    makefunctor(&string_const_66),
  };
  static Const unsigned char strconst_body_68[4] =
    ";\n";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_68 =
      declare_string_constant_8(strconst_body_68,2);
  static Const q funct_const_69[] = {
    makesym(functor_fwrite_1),
    makefunctor(&string_const_68),
  };
  static Const unsigned char strconst_body_70[8] =
    "NILATOM";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_70 =
      declare_string_constant_8(strconst_body_70,7);
  static Const q funct_const_71[] = {
    makesym(functor_fwrite_1),
    makefunctor(&string_const_70),
  };
  static Const unsigned char strconst_body_72[12] =
    "PERIODATOM";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_72 =
      declare_string_constant_8(strconst_body_72,10);
  static Const q funct_const_73[] = {
    makesym(functor_fwrite_1),
    makefunctor(&string_const_72),
  };
  static Const unsigned char strconst_body_74[4] =
    "\\\"";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_74 =
      declare_string_constant_8(strconst_body_74,2);
  static Const q funct_const_75[] = {
    makesym(functor_fwrite_1),
    makefunctor(&string_const_74),
  };
  static Const unsigned char strconst_body_76[4] =
    "\\\'";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_76 =
      declare_string_constant_8(strconst_body_76,2);
  static Const q funct_const_77[] = {
    makesym(functor_fwrite_1),
    makefunctor(&string_const_76),
  };
  static Const unsigned char strconst_body_78[4] =
    "\\\\";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_78 =
      declare_string_constant_8(strconst_body_78,2);
  static Const q funct_const_79[] = {
    makesym(functor_fwrite_1),
    makefunctor(&string_const_78),
  };
  static Const unsigned char strconst_body_80[4] =
    "\\n";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_80 =
      declare_string_constant_8(strconst_body_80,2);
  static Const q funct_const_81[] = {
    makesym(functor_fwrite_1),
    makefunctor(&string_const_80),
  };
  static Const unsigned char strconst_body_82[4] =
    "\\t";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_82 =
      declare_string_constant_8(strconst_body_82,2);
  static Const q funct_const_83[] = {
    makesym(functor_fwrite_1),
    makefunctor(&string_const_82),
  };
  static Const unsigned char strconst_body_84[4] =
    "{";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_84 =
      declare_string_constant_8(strconst_body_84,1);
  static Const q funct_const_85[] = {
    makesym(functor_fwrite_1),
    makefunctor(&string_const_84),
  };
  static Const unsigned char strconst_body_86[4] =
    "}";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_86 =
      declare_string_constant_8(strconst_body_86,1);
  static Const q funct_const_87[] = {
    makesym(functor_fwrite_1),
    makefunctor(&string_const_86),
  };
  q a0, a1, a2, a3, a4, a5;

  q *reasonp;
 module_top:
  a0 = qp->args[0];
  a1 = qp->args[1];
  switch_on_pred() {
    case_pred(0, write__out_3_top);
    case_pred(1, declare__const_5_top);
    case_pred(2, declare__const_6_top);
    case_pred(3, declare__const__list_6_top);
    case_pred(4, write__const__elems_3_top);
    case_pred(5, write__const__elem_3_top);
    case_pred(6, write__string__body_4_top);
    case_pred(7, write__character__constants_5_top);
    case_pred(8, write__string__elements_5_top);
    case_pred(9, write__string__element_3_top);
    case_pred(10, write__pred__structs_4_top);
    case_pred(11, declare__externals_3_top);
    case_pred(12, declare__one__external_3_top);
    case_pred(13, pred__min__max_5_top);
    case_pred(14, declare__registers_4_top);
    case_pred(15, write__comma__list_5_top);
    case_pred(16, write__comma__list1_5_top);
    case_pred(17, write__pred__dispatch_3_top);
    case_pred(18, write__predicates_6_top);
    case_pred(19, write__predicates__1_6_top);
    case_pred(20, write__get__args_4_top);
    case_pred(21, write__interrupt__call_4_top);
    case_pred(22, write__header__file_3_top);
    case_pred(23, write__ext__entries_3_top);
    case_pred(24, write__ext__entry_3_top);
    case_pred(25, write__defined__preds_4_top);
    case_pred(26, write__atom_3_top);
    case_pred(27, write__funct__name_4_top);
    case_pred(28, write__pred__name_5_top);
    case_pred(29, sort__info_2_top);
    case_pred(30, sort__info_3_top);
    last_case_pred(31, partition_4_top);
  }

 write__out_3_top: {
  q x0, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13, x14, x15, x16, x17, x18, x19, x20, x21, x22, x23, x24;
  a2 = qp->args[2];
  qp = qp->next;
 write__out_3_clear_reason:
  reasonp = reasons;
 write__out_3_0:
 write__out_3_1:
  switch (ptagof(a0)) {
 case FUNCTOR:
  if (functor_of(a0) != makesym(functor_module_3)) goto write__out_3_interrupt;
  allocp[0] = NILATOM;
  x1 = arg(a0, 0);
  allocp[1] = x1;
  x0 = makecons(&allocp[0]);
  allocp[2] = (q)qp;
  allocp[3] = (q)(&predicate_klic__comp__obj_xklicformat_4);
  allocp[4] = makefunctor(&string_const_33);
  allocp[5] = x0;
  allocp[6] = x2 = makeref(&allocp[6]);
  allocp[7] = a2;
  allocp[8] = (q)(struct goalrec*)&allocp[2];
  allocp[9] = (q)(&predicate_klic__comp__write_xwrite__interrupt__call_4);
  allocp[10] = x3 = makeref(&allocp[10]);
  allocp[11] = x3;
  allocp[12] = x4 = makeref(&allocp[12]);
  allocp[13] = x2;
  allocp[14] = (q)(struct goalrec*)&allocp[8];
  allocp[15] = (q)(&predicate_klic__comp__write_xwrite__predicates_6);
  x5 = arg(a0, 1);
  allocp[16] = x5;
  x6 = arg(a0, 0);
  allocp[17] = x6;
  allocp[18] = x7 = makeref(&allocp[18]);
  x8 = arg(a0, 2);
  allocp[19] = x8;
  allocp[20] = x9 = makeref(&allocp[20]);
  allocp[21] = x4;
  allocp[22] = (q)(struct goalrec*)&allocp[14];
  allocp[23] = (q)(&predicate_klic__comp__obj_xklicformat_3);
  allocp[24] = makefunctor(&string_const_29);
  allocp[25] = x10 = makeref(&allocp[25]);
  allocp[26] = x9;
  allocp[27] = (q)(struct goalrec*)&allocp[22];
  allocp[28] = (q)(&predicate_klic__comp__write_xwrite__pred__dispatch_3);
  allocp[29] = x5;
  allocp[30] = x11 = makeref(&allocp[30]);
  allocp[31] = x10;
  allocp[32] = (q)(struct goalrec*)&allocp[27];
  allocp[33] = (q)(&predicate_klic__comp__obj_xklicformat_3);
  allocp[34] = makefunctor(&string_const_27);
  allocp[35] = x12 = makeref(&allocp[35]);
  allocp[36] = x11;
  allocp[37] = (q)(struct goalrec*)&allocp[32];
  allocp[38] = (q)(&predicate_klic__comp__write_xwrite__get__args_4);
  allocp[39] = makeint(0L);
  allocp[40] = x7;
  allocp[41] = x13 = makeref(&allocp[41]);
  allocp[42] = x12;
  allocp[43] = (q)(struct goalrec*)&allocp[37];
  allocp[44] = (q)(&predicate_klic__comp__obj_xklicformat_3);
  allocp[45] = makefunctor(&string_const_0);
  allocp[46] = x14 = makeref(&allocp[46]);
  allocp[47] = x13;
  allocp[48] = (q)(struct goalrec*)&allocp[43];
  allocp[49] = (q)(&predicate_klic__comp__write_xdeclare__registers_4);
  allocp[50] = x3;
  allocp[51] = makefunctor(&string_const_37);
  allocp[52] = x15 = makeref(&allocp[52]);
  allocp[53] = x14;
  allocp[54] = (q)(struct goalrec*)&allocp[48];
  allocp[55] = (q)(&predicate_klic__comp__write_xpred__min__max_5);
  allocp[56] = x5;
  allocp[57] = makeint(99999L);
  allocp[58] = x7;
  allocp[59] = makeint(0L);
  allocp[60] = x3;
  allocp[61] = (q)(struct goalrec*)&allocp[54];
  allocp[62] = (q)(&predicate_klic__comp__write_xdeclare__const_5);
  allocp[63] = x16 = makeref(&allocp[63]);
  allocp[64] = makeint(0L);
  allocp[65] = x17 = makeref(&allocp[65]);
  allocp[66] = x18 = makeref(&allocp[66]);
  allocp[67] = x15;
  allocp[68] = NILATOM;
  allocp[69] = x6;
  x19 = makecons(&allocp[68]);
  allocp[70] = (q)(struct goalrec*)&allocp[61];
  allocp[71] = (q)(&predicate_klic__comp__obj_xklicformat_4);
  allocp[72] = makefunctor(&string_const_46);
  allocp[73] = x19;
  allocp[74] = x20 = makeref(&allocp[74]);
  allocp[75] = x18;
  allocp[76] = x20;
  allocp[77] = makeint(10L);
  x21 = makecons(&allocp[76]);
  allocp[78] = (q)(struct goalrec*)&allocp[70];
  allocp[79] = (q)(&predicate_klic__comp__write_xdeclare__externals_3);
  allocp[80] = x16;
  allocp[81] = x22 = makeref(&allocp[81]);
  allocp[82] = x21;
  allocp[83] = (q)(struct goalrec*)&allocp[78];
  allocp[84] = (q)(&predicate_klic__comp__write_xsort__info_2);
  allocp[85] = x8;
  allocp[86] = x16;
  allocp[87] = (q)(struct goalrec*)&allocp[83];
  allocp[88] = (q)(&predicate_klic__comp__write_xwrite__pred__structs_4);
  allocp[89] = x5;
  allocp[90] = x6;
  allocp[91] = x23 = makeref(&allocp[91]);
  allocp[92] = x22;
  allocp[93] = NILATOM;
  allocp[94] = x6;
  x24 = makecons(&allocp[93]);
  allocp[95] = (q)(struct goalrec*)&allocp[87];
  allocp[96] = (q)(&predicate_klic__comp__obj_xklicformat_4);
  allocp[97] = makefunctor(&string_const_45);
  allocp[98] = x24;
  allocp[99] = a1;
  allocp[100] = x23;
  qp = (struct goalrec*)&allocp[95];
  allocp += 101;
  proceed();
 case VARREF:
  deref_and_jump(a0,write__out_3_1);
  *reasonp++ =  a0;
  goto write__out_3_interrupt;
  };
  goto write__out_3_interrupt;
 write__out_3_ext_interrupt:
  reasonp = 0l;
 write__out_3_interrupt:
  goto interrupt_3;
 }

 declare__const_5_top: {
  q x0, x1, x2, x3, x4;
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  qp = qp->next;
 declare__const_5_clear_reason:
  reasonp = reasons;
 declare__const_5_0:
 declare__const_5_1:
  switch (ptagof(a0)) {
 case CONS:
  x0 = car_of(a0);
 declare__const_5_2:
  switch (ptagof(x0)) {
 case FUNCTOR:
  if (functor_of(x0) != makesym(functor_const_2)) goto declare__const_5_3;
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__write_xdeclare__const_5);
  x1 = cdr_of(a0);
  allocp[2] = x1;
  allocp[3] = x2 = makeref(&allocp[3]);
  allocp[4] = a2;
  allocp[5] = x3 = makeref(&allocp[5]);
  allocp[6] = a4;
  a0 = arg(x0, 1);
  x4 = arg(x0, 0);
  a2 = a1;
  a4 = a3;
  a5 = x3;
  a3 = x2;
  a1 = x4;
  qp = (struct goalrec*)&allocp[0];
  allocp += 7;
  execute(declare__const_6_0);
  goto declare__const_6_ext_interrupt;
 case VARREF:
  deref_and_jump(x0,declare__const_5_2);
  *reasonp++ =  x0;
  goto declare__const_5_3;
  };
  goto declare__const_5_3;
 case ATOMIC:
  if (a0 != NILATOM) goto declare__const_5_3;
  unify2(a4, a3, a2, a1);
  proceed();
 case VARREF:
  deref_and_jump(a0,declare__const_5_1);
  *reasonp++ =  a0;
  goto declare__const_5_3;
  };
  goto declare__const_5_3;
 declare__const_5_3:
  otherwise(declare__const_5_interrupt);
 declare__const_5_4:
  switch (ptagof(a0)) {
 case CONS:
  a0 = cdr_of(a0);
  execute(declare__const_5_0);
  goto declare__const_5_ext_interrupt;
 case VARREF:
  deref_and_jump(a0,declare__const_5_4);
  *reasonp++ =  a0;
  goto declare__const_5_interrupt;
  };
  goto declare__const_5_interrupt;
 declare__const_5_ext_interrupt:
  reasonp = 0l;
 declare__const_5_interrupt:
  goto interrupt_5;
 }

 declare__const_6_top: {
  q x0, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13, x14, x15, x16, x17, x18, x19, x20, x21;
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  qp = qp->next;
 declare__const_6_clear_reason:
  reasonp = reasons;
 declare__const_6_0:
 declare__const_6_1:
  switch (ptagof(a0)) {
 case FUNCTOR:
  switch (symval(functor_of(a0))) {
 case functor_string_1:
  x0 = arg(a0, 0);
 declare__const_6_2:
  switch (ptagof(x0)) {
 case FUNCTOR:
 declare__const_6_3:
  if (!isgobj(x0)) goto declare__const_6_12;
 declare__const_6_4:
  gblt_is_string(x0,declare__const_6_12);
  gblt_size_of_string(x0,x1,declare__const_6_12);
  gblt_element_size_of_string(x0,x2,declare__const_6_12);
  x3 = makeint(0L);
  gblt_less_or_eq(x1,x3,declare__const_6_5);
  allocp[0] = makesym(functor_string_1);
  allocp[1] = a2;
  x4 = makefunctor(&allocp[0]);
  allocp += 2;
  unify_value(a1, x4);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_integer__arithmetics_xadd_3);
  allocp[2] = a2;
  allocp[3] = makeint(1L);
  allocp[4] = a3;
  allocp[5] = NILATOM;
  allocp[6] = x2;
  x5 = makecons(&allocp[5]);
  allocp[7] = x5;
  allocp[8] = a2;
  x6 = makecons(&allocp[7]);
  allocp[9] = x6;
  allocp[10] = x2;
  x7 = makecons(&allocp[9]);
  allocp[11] = (q)(struct goalrec*)&allocp[0];
  allocp[12] = (q)(&predicate_klic__comp__obj_xklicformat_4);
  allocp[13] = makefunctor(&string_const_13);
  allocp[14] = x7;
  allocp[15] = a4;
  allocp[16] = a5;
  qp = (struct goalrec*)&allocp[11];
  allocp += 17;
  proceed();
 declare__const_6_5:
  x4 = makeint(0L);
  gblt_greater(x1,x4,declare__const_6_12);
  allocp[0] = makesym(functor_string_1);
  allocp[1] = a2;
  x5 = makefunctor(&allocp[0]);
  allocp += 2;
  unify_value(a1, x5);
  x6 = makeint(3L);
  bblt_add_no_check(x1,x6,x7);
  x8 = makeint(4L);
  bblt_div_no_check(x7,x8,x9);
  x10 = makeint(4L);
  bblt_mult_no_check(x9,x10,x11);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_integer__arithmetics_xadd_3);
  allocp[2] = a2;
  allocp[3] = makeint(1L);
  allocp[4] = a3;
  allocp[5] = NILATOM;
  allocp[6] = x1;
  x12 = makecons(&allocp[5]);
  allocp[7] = x12;
  allocp[8] = a2;
  x13 = makecons(&allocp[7]);
  allocp[9] = x13;
  allocp[10] = x2;
  x14 = makecons(&allocp[9]);
  allocp[11] = x14;
  allocp[12] = a2;
  x15 = makecons(&allocp[11]);
  allocp[13] = x15;
  allocp[14] = x2;
  x16 = makecons(&allocp[13]);
  allocp[15] = (q)(struct goalrec*)&allocp[0];
  allocp[16] = (q)(&predicate_klic__comp__obj_xklicformat_4);
  allocp[17] = makefunctor(&string_const_14);
  allocp[18] = x16;
  allocp[19] = x17 = makeref(&allocp[19]);
  allocp[20] = a5;
  allocp[21] = x17;
  allocp[22] = makefunctor(funct_const_69);
  x18 = makecons(&allocp[21]);
  allocp[23] = (q)(struct goalrec*)&allocp[15];
  allocp[24] = (q)(&predicate_klic__comp__write_xwrite__string__body_4);
  allocp[25] = x1;
  allocp[26] = x0;
  allocp[27] = x19 = makeref(&allocp[27]);
  allocp[28] = x18;
  allocp[29] = NILATOM;
  allocp[30] = x11;
  x20 = makecons(&allocp[29]);
  allocp[31] = x20;
  allocp[32] = a2;
  x21 = makecons(&allocp[31]);
  allocp[33] = (q)(struct goalrec*)&allocp[23];
  allocp[34] = (q)(&predicate_klic__comp__obj_xklicformat_4);
  allocp[35] = makefunctor(&string_const_26);
  allocp[36] = x21;
  allocp[37] = a4;
  allocp[38] = x19;
  qp = (struct goalrec*)&allocp[33];
  allocp += 39;
  proceed();
 case VARREF:
  deref_and_jump(x0,declare__const_6_2);
  *reasonp++ =  x0;
  goto declare__const_6_12;
  };
  goto declare__const_6_12;
 case functor_vector_1:
  x0 = arg(a0, 0);
 declare__const_6_6:
  switch (ptagof(x0)) {
 case FUNCTOR:
 declare__const_6_7:
  if (!isgobj(x0)) goto declare__const_6_12;
  if (!isclass(x0,vector)) goto declare__const_6_12;
  gblt_size_of_vector(x0,x1,declare__const_6_12);
  x2 = makeint(0L);
  gblt_less_or_eq(x1,x2,declare__const_6_8);
  allocp[0] = makesym(functor_vector_1);
  allocp[1] = a2;
  x3 = makefunctor(&allocp[0]);
  allocp += 2;
  unify_value(a1, x3);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_integer__arithmetics_xadd_3);
  allocp[2] = a2;
  allocp[3] = makeint(1L);
  allocp[4] = a3;
  allocp[5] = NILATOM;
  allocp[6] = a2;
  x4 = makecons(&allocp[5]);
  allocp[7] = (q)(struct goalrec*)&allocp[0];
  allocp[8] = (q)(&predicate_klic__comp__obj_xklicformat_4);
  allocp[9] = makefunctor(&string_const_18);
  allocp[10] = x4;
  allocp[11] = a4;
  allocp[12] = a5;
  qp = (struct goalrec*)&allocp[7];
  allocp += 13;
  proceed();
 declare__const_6_8:
  x3 = makeint(0L);
  gblt_greater(x1,x3,declare__const_6_12);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_integer__arithmetics_xadd_3);
  allocp[2] = x4 = makeref(&allocp[2]);
  allocp[3] = makeint(1L);
  allocp[4] = a3;
  allocp[5] = NILATOM;
  allocp[6] = x1;
  x5 = makecons(&allocp[5]);
  allocp[7] = x5;
  allocp[8] = x4;
  x6 = makecons(&allocp[7]);
  allocp[9] = x6;
  allocp[10] = x4;
  x7 = makecons(&allocp[9]);
  allocp[11] = (q)(struct goalrec*)&allocp[0];
  allocp[12] = (q)(&predicate_klic__comp__obj_xklicformat_4);
  allocp[13] = makefunctor(&string_const_19);
  allocp[14] = x7;
  allocp[15] = x8 = makeref(&allocp[15]);
  allocp[16] = a5;
  allocp[17] = x8;
  allocp[18] = makefunctor(funct_const_61);
  x9 = makecons(&allocp[17]);
  allocp[19] = (q)(struct goalrec*)&allocp[11];
  allocp[20] = (q)(&predicate_klic__comp__write_xwrite__const__elems_3);
  allocp[21] = x10 = makeref(&allocp[21]);
  allocp[22] = x11 = makeref(&allocp[22]);
  allocp[23] = x9;
  allocp[24] = NILATOM;
  allocp[25] = x4;
  x12 = makecons(&allocp[24]);
  allocp[26] = (q)(struct goalrec*)&allocp[19];
  allocp[27] = (q)(&predicate_klic__comp__obj_xklicformat_4);
  allocp[28] = makefunctor(&string_const_25);
  allocp[29] = x12;
  allocp[30] = x13 = makeref(&allocp[30]);
  allocp[31] = x11;
  allocp[32] = makesym(functor_vector_1);
  allocp[33] = x4;
  x14 = makefunctor(&allocp[32]);
  qp = (struct goalrec*)&allocp[26];
  allocp += 34;
  unify_value(a1, x14);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__write_xdeclare__const__list_6);
  allocp[2] = x15 = makeref(&allocp[2]);
  allocp[3] = x10;
  allocp[4] = a2;
  allocp[5] = x4;
  allocp[6] = a4;
  allocp[7] = x13;
  allocp[8] = (q)(struct goalrec*)&allocp[0];
  allocp[9] = (q)(&predicate_klic__comp__util_xvector__to__list_2);
  allocp[10] = x0;
  allocp[11] = x15;
  qp = (struct goalrec*)&allocp[8];
  allocp += 12;
  proceed();
 case VARREF:
  deref_and_jump(x0,declare__const_6_6);
  *reasonp++ =  x0;
  goto declare__const_6_12;
  };
  goto declare__const_6_12;
 case functor_float_1:
  allocp[0] = makesym(functor_float_1);
  allocp[1] = a2;
  x0 = makefunctor(&allocp[0]);
  allocp += 2;
  unify_value(a1, x0);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_integer__arithmetics_xadd_3);
  allocp[2] = a2;
  allocp[3] = makeint(1L);
  allocp[4] = a3;
  allocp[5] = NILATOM;
  x2 = arg(a0, 0);
  allocp[6] = x2;
  x1 = makecons(&allocp[5]);
  allocp[7] = x1;
  allocp[8] = a2;
  x3 = makecons(&allocp[7]);
  allocp[9] = (q)(struct goalrec*)&allocp[0];
  allocp[10] = (q)(&predicate_klic__comp__obj_xklicformat_4);
  allocp[11] = makefunctor(&string_const_15);
  allocp[12] = x3;
  allocp[13] = a4;
  allocp[14] = a5;
  qp = (struct goalrec*)&allocp[9];
  allocp += 15;
  proceed();
 case functor_module_1:
  allocp[0] = makesym(functor_module_1);
  allocp[1] = a2;
  x0 = makefunctor(&allocp[0]);
  allocp += 2;
  unify_value(a1, x0);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_integer__arithmetics_xadd_3);
  allocp[2] = a2;
  allocp[3] = makeint(1L);
  allocp[4] = a3;
  allocp[5] = NILATOM;
  x2 = arg(a0, 0);
  allocp[6] = x2;
  x1 = makecons(&allocp[5]);
  allocp[7] = x1;
  x4 = arg(a0, 0);
  allocp[8] = x4;
  x3 = makecons(&allocp[7]);
  allocp[9] = x3;
  allocp[10] = a2;
  x5 = makecons(&allocp[9]);
  allocp[11] = x5;
  x7 = arg(a0, 0);
  allocp[12] = x7;
  x6 = makecons(&allocp[11]);
  allocp[13] = (q)(struct goalrec*)&allocp[0];
  allocp[14] = (q)(&predicate_klic__comp__obj_xklicformat_4);
  allocp[15] = makefunctor(&string_const_16);
  allocp[16] = x6;
  allocp[17] = a4;
  allocp[18] = a5;
  qp = (struct goalrec*)&allocp[13];
  allocp += 19;
  proceed();
 case functor_predicate_3:
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_integer__arithmetics_xadd_3);
  allocp[2] = x0 = makeref(&allocp[2]);
  allocp[3] = makeint(1L);
  allocp[4] = a3;
  allocp[5] = NILATOM;
  x2 = arg(a0, 1);
  allocp[6] = x2;
  x1 = makecons(&allocp[5]);
  allocp[7] = x1;
  allocp[8] = x4 = makeref(&allocp[8]);
  x3 = makecons(&allocp[7]);
  allocp[9] = x3;
  x6 = arg(a0, 2);
  allocp[10] = x6;
  x5 = makecons(&allocp[9]);
  allocp[11] = x5;
  x8 = arg(a0, 1);
  allocp[12] = x8;
  x7 = makecons(&allocp[11]);
  allocp[13] = x7;
  x10 = arg(a0, 0);
  allocp[14] = x10;
  x9 = makecons(&allocp[13]);
  allocp[15] = x9;
  allocp[16] = x0;
  x11 = makecons(&allocp[15]);
  allocp[17] = (q)(struct goalrec*)&allocp[0];
  allocp[18] = (q)(&predicate_klic__comp__obj_xklicformat_4);
  allocp[19] = makefunctor(&string_const_17);
  allocp[20] = x11;
  allocp[21] = x12 = makeref(&allocp[21]);
  allocp[22] = a5;
  allocp[23] = makesym(functor_predicate_1);
  allocp[24] = x0;
  x13 = makefunctor(&allocp[23]);
  qp = (struct goalrec*)&allocp[17];
  allocp += 25;
  unify_value(a1, x13);
  allocp[0] = makesym(functor_module_1);
  x15 = arg(a0, 0);
  allocp[1] = x15;
  x14 = makefunctor(&allocp[0]);
  allocp[2] = makesym(functor_module_1);
  allocp[3] = x4;
  x16 = makefunctor(&allocp[2]);
  a0 = x14;
  a1 = x16;
  a3 = x0;
  a5 = x12;
  allocp += 4;
  execute(declare__const_6_0);
  goto declare__const_6_ext_interrupt;
 case functor_list_1:
  x0 = arg(a0, 0);
 declare__const_6_9:
  switch (ptagof(x0)) {
 case CONS:
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_integer__arithmetics_xadd_3);
  allocp[2] = x1 = makeref(&allocp[2]);
  allocp[3] = makeint(1L);
  allocp[4] = a3;
  allocp[5] = (q)(struct goalrec*)&allocp[0];
  allocp[6] = (q)(&predicate_klic__comp__obj_xklicformat_3);
  allocp[7] = makefunctor(&string_const_30);
  allocp[8] = x2 = makeref(&allocp[8]);
  allocp[9] = a5;
  allocp[10] = NILATOM;
  allocp[11] = x4 = makeref(&allocp[11]);
  x3 = makecons(&allocp[10]);
  allocp[12] = x3;
  allocp[13] = x6 = makeref(&allocp[13]);
  x5 = makecons(&allocp[12]);
  allocp[14] = (q)(struct goalrec*)&allocp[5];
  allocp[15] = (q)(&predicate_klic__comp__write_xwrite__const__elems_3);
  allocp[16] = x5;
  allocp[17] = x7 = makeref(&allocp[17]);
  allocp[18] = x2;
  allocp[19] = NILATOM;
  allocp[20] = x1;
  x8 = makecons(&allocp[19]);
  allocp[21] = (q)(struct goalrec*)&allocp[14];
  allocp[22] = (q)(&predicate_klic__comp__obj_xklicformat_4);
  allocp[23] = makefunctor(&string_const_23);
  allocp[24] = x8;
  allocp[25] = x9 = makeref(&allocp[25]);
  allocp[26] = x7;
  allocp[27] = makesym(functor_list_1);
  allocp[28] = x1;
  x10 = makefunctor(&allocp[27]);
  qp = (struct goalrec*)&allocp[21];
  allocp += 29;
  unify_value(a1, x10);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__write_xdeclare__const_6);
  x11 = car_of(x0);
  allocp[2] = x11;
  allocp[3] = x4;
  allocp[4] = x12 = makeref(&allocp[4]);
  allocp[5] = x1;
  allocp[6] = x13 = makeref(&allocp[6]);
  allocp[7] = x9;
  a0 = cdr_of(x0);
  a1 = x6;
  a3 = x12;
  a5 = x13;
  qp = (struct goalrec*)&allocp[0];
  allocp += 8;
  execute(declare__const_6_0);
  goto declare__const_6_ext_interrupt;
 case VARREF:
  deref_and_jump(x0,declare__const_6_9);
  *reasonp++ =  x0;
  goto declare__const_6_12;
  };
  goto declare__const_6_12;
 case functor_functor_1:
  x0 = arg(a0, 0);
 declare__const_6_10:
  if (isref(x0)) goto declare__const_6_11;
  gblt_pfunctor(x0,x1,declare__const_6_12);
  gblt_arity(x0,x2,declare__const_6_12);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_integer__arithmetics_xadd_3);
  allocp[2] = x3 = makeref(&allocp[2]);
  allocp[3] = makeint(1L);
  allocp[4] = a3;
  allocp[5] = (q)(struct goalrec*)&allocp[0];
  allocp[6] = (q)(&predicate_klic__comp__obj_xklicformat_3);
  allocp[7] = makefunctor(&string_const_30);
  allocp[8] = x4 = makeref(&allocp[8]);
  allocp[9] = a5;
  allocp[10] = (q)(struct goalrec*)&allocp[5];
  allocp[11] = (q)(&predicate_klic__comp__write_xwrite__const__elems_3);
  allocp[12] = x5 = makeref(&allocp[12]);
  allocp[13] = x6 = makeref(&allocp[13]);
  allocp[14] = x4;
  allocp[15] = makesym(functor__2F_2);
  allocp[16] = x1;
  allocp[17] = x2;
  x7 = makefunctor(&allocp[15]);
  allocp[18] = NILATOM;
  allocp[19] = x7;
  x8 = makecons(&allocp[18]);
  allocp[20] = x8;
  allocp[21] = x3;
  x9 = makecons(&allocp[20]);
  allocp[22] = (q)(struct goalrec*)&allocp[10];
  allocp[23] = (q)(&predicate_klic__comp__obj_xklicformat_4);
  allocp[24] = makefunctor(&string_const_24);
  allocp[25] = x9;
  allocp[26] = x10 = makeref(&allocp[26]);
  allocp[27] = x6;
  allocp[28] = makesym(functor_functor_1);
  allocp[29] = x3;
  x11 = makefunctor(&allocp[28]);
  qp = (struct goalrec*)&allocp[22];
  allocp += 30;
  unify_value(a1, x11);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__write_xdeclare__const__list_6);
  allocp[2] = x12 = makeref(&allocp[2]);
  allocp[3] = x5;
  allocp[4] = a2;
  allocp[5] = x3;
  allocp[6] = a4;
  allocp[7] = x10;
  allocp[8] = x12;
  allocp[9] = x1;
  x13 = makecons(&allocp[8]);
  allocp[10] = (q)(struct goalrec*)&allocp[0];
  allocp[11] = (q)(&predicate_klic__comp__util_xuniv_2);
  allocp[12] = x0;
  allocp[13] = x13;
  qp = (struct goalrec*)&allocp[10];
  allocp += 14;
  proceed();
 declare__const_6_11:
  deref_and_jump(x0,declare__const_6_10);
  *reasonp++ =  x0;
  goto declare__const_6_12;
  };
  goto declare__const_6_12;
 case VARREF:
  deref_and_jump(a0,declare__const_6_1);
  *reasonp++ =  a0;
  goto declare__const_6_12;
  };
  goto declare__const_6_12;
 declare__const_6_12:
  otherwise(declare__const_6_interrupt);
  unify3(a5, a4, a3, a2, a1, a0);
  proceed();
 declare__const_6_ext_interrupt:
  reasonp = 0l;
 declare__const_6_interrupt:
  toppred = &predicate_klic__comp__write_xdeclare__const_6;
  goto interrupt_6;
 }

 declare__const__list_6_top: {
  q x0, x1, x2, x3, x4, x5;
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  qp = qp->next;
 declare__const__list_6_clear_reason:
  reasonp = reasons;
 declare__const__list_6_0:
 declare__const__list_6_1:
  switch (ptagof(a0)) {
 case CONS:
  allocp[0] = x1 = makeref(&allocp[0]);
  allocp[1] = x2 = makeref(&allocp[1]);
  x0 = makecons(&allocp[0]);
  allocp += 2;
  unify_value(a1, x0);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__write_xdeclare__const__list_6);
  x3 = cdr_of(a0);
  allocp[2] = x3;
  allocp[3] = x1;
  allocp[4] = x4 = makeref(&allocp[4]);
  allocp[5] = a3;
  allocp[6] = x5 = makeref(&allocp[6]);
  allocp[7] = a5;
  a0 = car_of(a0);
  a1 = x2;
  a3 = x4;
  a5 = x5;
  qp = (struct goalrec*)&allocp[0];
  allocp += 8;
  execute(declare__const_6_0);
  goto declare__const_6_ext_interrupt;
 case ATOMIC:
  if (a0 != NILATOM) goto declare__const__list_6_interrupt;
  unify2(a5, a4, a3, a2);
  x0 = NILATOM;
  unify_value(a1, x0);
  proceed();
 case VARREF:
  deref_and_jump(a0,declare__const__list_6_1);
  *reasonp++ =  a0;
  goto declare__const__list_6_interrupt;
  };
  goto declare__const__list_6_interrupt;
 declare__const__list_6_ext_interrupt:
  reasonp = 0l;
 declare__const__list_6_interrupt:
  goto interrupt_6;
 }

 write__const__elems_3_top: {
  q x0, x1;
  a2 = qp->args[2];
  qp = qp->next;
 write__const__elems_3_clear_reason:
  reasonp = reasons;
 write__const__elems_3_0:
 write__const__elems_3_1:
  switch (ptagof(a0)) {
 case CONS:
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__write_xwrite__const__elems_3);
  x0 = cdr_of(a0);
  allocp[2] = x0;
  allocp[3] = x1 = makeref(&allocp[3]);
  allocp[4] = a2;
  a0 = car_of(a0);
  a2 = x1;
  qp = (struct goalrec*)&allocp[0];
  allocp += 5;
  execute(write__const__elem_3_0);
  goto write__const__elem_3_ext_interrupt;
 case ATOMIC:
  if (a0 != NILATOM) goto write__const__elems_3_interrupt;
  unify(a2, a1);
  proceed();
 case VARREF:
  deref_and_jump(a0,write__const__elems_3_1);
  *reasonp++ =  a0;
  goto write__const__elems_3_interrupt;
  };
  goto write__const__elems_3_interrupt;
 write__const__elems_3_ext_interrupt:
  reasonp = 0l;
 write__const__elems_3_interrupt:
  goto interrupt_3;
 }

 write__const__elem_3_top: {
  q x0, x1;
  a2 = qp->args[2];
  qp = qp->next;
 write__const__elem_3_clear_reason:
  reasonp = reasons;
 write__const__elem_3_0:
 write__const__elem_3_1:
  switch (ptagof(a0)) {
 case FUNCTOR:
  switch (symval(functor_of(a0))) {
 case functor_atom_1:
  allocp[0] = NILATOM;
  x1 = arg(a0, 0);
  allocp[1] = x1;
  x0 = makecons(&allocp[0]);
  allocp[2] = (q)qp;
  allocp[3] = (q)(&predicate_klic__comp__obj_xklicformat_4);
  allocp[4] = makefunctor(&string_const_11);
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
  allocp[4] = makefunctor(&string_const_8);
  allocp[5] = x0;
  allocp[6] = a1;
  allocp[7] = a2;
  qp = (struct goalrec*)&allocp[2];
  allocp += 8;
  proceed();
 case functor_float_1:
  allocp[0] = NILATOM;
  x1 = arg(a0, 0);
  allocp[1] = x1;
  x0 = makecons(&allocp[0]);
  allocp[2] = (q)qp;
  allocp[3] = (q)(&predicate_klic__comp__obj_xklicformat_4);
  allocp[4] = makefunctor(&string_const_4);
  allocp[5] = x0;
  allocp[6] = a1;
  allocp[7] = a2;
  qp = (struct goalrec*)&allocp[2];
  allocp += 8;
  proceed();
 case functor_predicate_1:
  allocp[0] = NILATOM;
  x1 = arg(a0, 0);
  allocp[1] = x1;
  x0 = makecons(&allocp[0]);
  allocp[2] = (q)qp;
  allocp[3] = (q)(&predicate_klic__comp__obj_xklicformat_4);
  allocp[4] = makefunctor(&string_const_10);
  allocp[5] = x0;
  allocp[6] = a1;
  allocp[7] = a2;
  qp = (struct goalrec*)&allocp[2];
  allocp += 8;
  proceed();
 case functor_module_1:
  allocp[0] = NILATOM;
  x1 = arg(a0, 0);
  allocp[1] = x1;
  x0 = makecons(&allocp[0]);
  allocp[2] = (q)qp;
  allocp[3] = (q)(&predicate_klic__comp__obj_xklicformat_4);
  allocp[4] = makefunctor(&string_const_9);
  allocp[5] = x0;
  allocp[6] = a1;
  allocp[7] = a2;
  qp = (struct goalrec*)&allocp[2];
  allocp += 8;
  proceed();
 case functor_list_1:
  allocp[0] = NILATOM;
  x1 = arg(a0, 0);
  allocp[1] = x1;
  x0 = makecons(&allocp[0]);
  allocp[2] = (q)qp;
  allocp[3] = (q)(&predicate_klic__comp__obj_xklicformat_4);
  allocp[4] = makefunctor(&string_const_3);
  allocp[5] = x0;
  allocp[6] = a1;
  allocp[7] = a2;
  qp = (struct goalrec*)&allocp[2];
  allocp += 8;
  proceed();
 case functor_functor_1:
  allocp[0] = NILATOM;
  x1 = arg(a0, 0);
  allocp[1] = x1;
  x0 = makecons(&allocp[0]);
  allocp[2] = (q)qp;
  allocp[3] = (q)(&predicate_klic__comp__obj_xklicformat_4);
  allocp[4] = makefunctor(&string_const_7);
  allocp[5] = x0;
  allocp[6] = a1;
  allocp[7] = a2;
  qp = (struct goalrec*)&allocp[2];
  allocp += 8;
  proceed();
 case functor_string_1:
  allocp[0] = NILATOM;
  x1 = arg(a0, 0);
  allocp[1] = x1;
  x0 = makecons(&allocp[0]);
  allocp[2] = (q)qp;
  allocp[3] = (q)(&predicate_klic__comp__obj_xklicformat_4);
  allocp[4] = makefunctor(&string_const_5);
  allocp[5] = x0;
  allocp[6] = a1;
  allocp[7] = a2;
  qp = (struct goalrec*)&allocp[2];
  allocp += 8;
  proceed();
 case functor_vector_1:
  allocp[0] = NILATOM;
  x1 = arg(a0, 0);
  allocp[1] = x1;
  x0 = makecons(&allocp[0]);
  allocp[2] = (q)qp;
  allocp[3] = (q)(&predicate_klic__comp__obj_xklicformat_4);
  allocp[4] = makefunctor(&string_const_6);
  allocp[5] = x0;
  allocp[6] = a1;
  allocp[7] = a2;
  qp = (struct goalrec*)&allocp[2];
  allocp += 8;
  proceed();
  };
  goto write__const__elem_3_interrupt;
 case VARREF:
  deref_and_jump(a0,write__const__elem_3_1);
  *reasonp++ =  a0;
  goto write__const__elem_3_interrupt;
  };
  goto write__const__elem_3_interrupt;
 write__const__elem_3_ext_interrupt:
  reasonp = 0l;
 write__const__elem_3_interrupt:
  toppred = &predicate_klic__comp__write_xwrite__const__elem_3;
  goto interrupt_3;
 }

 write__string__body_4_top: {
  q x0, x1, x2, x3, x4, x5, x6, x7;
  a2 = qp->args[2];
  a3 = qp->args[3];
  qp = qp->next;
 write__string__body_4_clear_reason:
  reasonp = reasons;
 write__string__body_4_0:
 write__string__body_4_1:
  if (!isint(a0)) goto write__string__body_4_3;
  x1 = makeint(4L);
  gblt_modulo(a0,x1,x0,write__string__body_4_interrupt);
  x2 = makeint(0L);
  gblt_not_eq(x0,x2,write__string__body_4_2);
  allocp[0] = x4 = makeref(&allocp[0]);
  allocp[1] = makefunctor(funct_const_63);
  x3 = makecons(&allocp[0]);
  allocp += 2;
  unify_value(a2, x3);
  allocp[0] = a3;
  allocp[1] = makefunctor(funct_const_63);
  x5 = makecons(&allocp[0]);
  x6 = a0;
  a2 = a1;
  a3 = x4;
  a4 = x5;
  a1 = x6;
  a0 = makeint(0L);
  allocp += 2;
  execute(write__string__elements_5_0);
  goto write__string__elements_5_ext_interrupt;
 write__string__body_4_2:
  x3 = makeint(0L);
  gblt_eq(x0,x3,write__string__body_4_interrupt);
  allocp[0] = x5 = makeref(&allocp[0]);
  allocp[1] = makefunctor(funct_const_85);
  x4 = makecons(&allocp[0]);
  allocp += 2;
  unify_value(a2, x4);
  allocp[0] = a3;
  allocp[1] = makefunctor(funct_const_87);
  x6 = makecons(&allocp[0]);
  x7 = a0;
  a2 = a1;
  a3 = x5;
  a4 = x6;
  a1 = x7;
  a0 = makeint(0L);
  allocp += 2;
  execute(write__character__constants_5_0);
  goto write__character__constants_5_ext_interrupt;
 write__string__body_4_3:
  if (!isref(a0)) goto write__string__body_4_interrupt;
  deref_and_jump(a0,write__string__body_4_1);
  *reasonp++ =  a0;
  goto write__string__body_4_interrupt;
 write__string__body_4_ext_interrupt:
  reasonp = 0l;
 write__string__body_4_interrupt:
  goto interrupt_4;
 }

 write__character__constants_5_top: {
  q x0, x1, x2, x3, x4, x5, x6;
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  qp = qp->next;
 write__character__constants_5_clear_reason:
  reasonp = reasons;
 write__character__constants_5_0:
 write__character__constants_5_1:
  if (!isint(a0)) goto write__character__constants_5_8;
 write__character__constants_5_2:
  if (!isint(a1)) goto write__character__constants_5_7;
  gblt_greater_or_eq(a0,a1,write__character__constants_5_3);
  unify(a4, a3);
  proceed();
 write__character__constants_5_3:
  gblt_less(a0,a1,write__character__constants_5_interrupt);
 write__character__constants_5_4:
  switch (ptagof(a2)) {
 case FUNCTOR:
 write__character__constants_5_5:
  if (!isgobj(a2)) goto write__character__constants_5_interrupt;
 write__character__constants_5_6:
  gblt_is_string(a2,write__character__constants_5_interrupt);
  gblt_element_of_string(a2,a0,x0,write__character__constants_5_interrupt);
  x2 = makeint(1L);
  gblt_add(a0,x2,x1,write__character__constants_5_interrupt);
  allocp[0] = x4 = makeref(&allocp[0]);
  allocp[1] = makefunctor(funct_const_65);
  x3 = makecons(&allocp[0]);
  allocp += 2;
  unify_value(a3, x3);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__write_xwrite__character__constants_5);
  allocp[2] = x1;
  allocp[3] = a1;
  allocp[4] = a2;
  allocp[5] = x5 = makeref(&allocp[5]);
  allocp[6] = a4;
  allocp[7] = x5;
  allocp[8] = makefunctor(funct_const_67);
  x6 = makecons(&allocp[7]);
  a0 = x0;
  a1 = x4;
  a2 = x6;
  qp = (struct goalrec*)&allocp[0];
  allocp += 9;
  execute(write__string__element_3_0);
  goto write__string__element_3_ext_interrupt;
 case VARREF:
  deref_and_jump(a2,write__character__constants_5_4);
  *reasonp++ =  a2;
  goto write__character__constants_5_interrupt;
  };
  goto write__character__constants_5_interrupt;
 write__character__constants_5_7:
  if (!isref(a1)) goto write__character__constants_5_interrupt;
  deref_and_jump(a1,write__character__constants_5_2);
  *reasonp++ =  a1;
  goto write__character__constants_5_interrupt;
 write__character__constants_5_8:
  if (!isref(a0)) goto write__character__constants_5_interrupt;
  deref_and_jump(a0,write__character__constants_5_1);
  *reasonp++ =  a0;
  goto write__character__constants_5_interrupt;
 write__character__constants_5_ext_interrupt:
  reasonp = 0l;
 write__character__constants_5_interrupt:
  toppred = &predicate_klic__comp__write_xwrite__character__constants_5;
  goto interrupt_5;
 }

 write__string__elements_5_top: {
  q x0, x1, x2, x3;
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  qp = qp->next;
 write__string__elements_5_clear_reason:
  reasonp = reasons;
 write__string__elements_5_0:
 write__string__elements_5_1:
  if (!isint(a0)) goto write__string__elements_5_8;
 write__string__elements_5_2:
  if (!isint(a1)) goto write__string__elements_5_7;
  gblt_greater_or_eq(a0,a1,write__string__elements_5_3);
  unify(a4, a3);
  proceed();
 write__string__elements_5_3:
  gblt_less(a0,a1,write__string__elements_5_interrupt);
 write__string__elements_5_4:
  switch (ptagof(a2)) {
 case FUNCTOR:
 write__string__elements_5_5:
  if (!isgobj(a2)) goto write__string__elements_5_interrupt;
 write__string__elements_5_6:
  gblt_is_string(a2,write__string__elements_5_interrupt);
  gblt_element_of_string(a2,a0,x0,write__string__elements_5_interrupt);
  x2 = makeint(1L);
  gblt_add(a0,x2,x1,write__string__elements_5_interrupt);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__write_xwrite__string__elements_5);
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
  execute(write__string__element_3_0);
  goto write__string__element_3_ext_interrupt;
 case VARREF:
  deref_and_jump(a2,write__string__elements_5_4);
  *reasonp++ =  a2;
  goto write__string__elements_5_interrupt;
  };
  goto write__string__elements_5_interrupt;
 write__string__elements_5_7:
  if (!isref(a1)) goto write__string__elements_5_interrupt;
  deref_and_jump(a1,write__string__elements_5_2);
  *reasonp++ =  a1;
  goto write__string__elements_5_interrupt;
 write__string__elements_5_8:
  if (!isref(a0)) goto write__string__elements_5_interrupt;
  deref_and_jump(a0,write__string__elements_5_1);
  *reasonp++ =  a0;
  goto write__string__elements_5_interrupt;
 write__string__elements_5_ext_interrupt:
  reasonp = 0l;
 write__string__elements_5_interrupt:
  toppred = &predicate_klic__comp__write_xwrite__string__elements_5;
  goto interrupt_5;
 }

 write__string__element_3_top: {
  q x0, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13, x14, x15, x16, x17, x18;
  a2 = qp->args[2];
  qp = qp->next;
 write__string__element_3_clear_reason:
  reasonp = reasons;
 write__string__element_3_0:
 write__string__element_3_1:
  switch (ptagof(a0)) {
 case ATOMIC:
 write__string__element_3_2:
  switch ((unsigned long)a0) {
 case (unsigned long) makeint(34L):
  allocp[0] = a2;
  allocp[1] = makefunctor(funct_const_75);
  x0 = makecons(&allocp[0]);
  allocp += 2;
  unify_value(a1, x0);
  proceed();
 case (unsigned long) makeint(39L):
  allocp[0] = a2;
  allocp[1] = makefunctor(funct_const_77);
  x0 = makecons(&allocp[0]);
  allocp += 2;
  unify_value(a1, x0);
  proceed();
 case (unsigned long) makeint(92L):
  allocp[0] = a2;
  allocp[1] = makefunctor(funct_const_79);
  x0 = makecons(&allocp[0]);
  allocp += 2;
  unify_value(a1, x0);
  proceed();
 case (unsigned long) makeint(10L):
  allocp[0] = a2;
  allocp[1] = makefunctor(funct_const_81);
  x0 = makecons(&allocp[0]);
  allocp += 2;
  unify_value(a1, x0);
  proceed();
 case (unsigned long) makeint(9L):
  allocp[0] = a2;
  allocp[1] = makefunctor(funct_const_83);
  x0 = makecons(&allocp[0]);
  allocp += 2;
  unify_value(a1, x0);
  proceed();
 case (unsigned long) makeint(32L):
  allocp[0] = a2;
  allocp[1] = makefunctor(funct_const_59);
  x0 = makecons(&allocp[0]);
  allocp += 2;
  unify_value(a1, x0);
  proceed();
  };
  goto write__string__element_3_3;
 case VARREF:
  deref_and_jump(a0,write__string__element_3_1);
  *reasonp++ =  a0;
  goto write__string__element_3_3;
  };
  goto write__string__element_3_3;
 write__string__element_3_3:
  otherwise(write__string__element_3_interrupt);
 write__string__element_3_4:
  if (!isint(a0)) goto write__string__element_3_5;
  
{
   long c = intval(a0);
   if (!isgraph(c)) goto write__string__element_3_6;
}
  allocp[0] = a2;
  allocp[1] = a0;
  x0 = makecons(&allocp[0]);
  allocp += 2;
  unify_value(a1, x0);
  proceed();
 write__string__element_3_5:
  if (!isref(a0)) goto write__string__element_3_6;
  deref_and_jump(a0,write__string__element_3_4);
  *reasonp++ =  a0;
  goto write__string__element_3_6;
 write__string__element_3_6:
  otherwise(write__string__element_3_interrupt);
 write__string__element_3_7:
  if (!isint(a0)) goto write__string__element_3_8;
  x1 = makeint(6L);
  gblt_rshift(a0,x1,x0,write__string__element_3_interrupt);
  x3 = makeint(48L);
  gblt_add(x0,x3,x2,write__string__element_3_interrupt);
  x5 = makeint(3L);
  gblt_rshift(a0,x5,x4,write__string__element_3_interrupt);
  x7 = makeint(7L);
  gblt_and(x4,x7,x6,write__string__element_3_interrupt);
  x9 = makeint(48L);
  gblt_add(x6,x9,x8,write__string__element_3_interrupt);
  x11 = makeint(7L);
  gblt_and(a0,x11,x10,write__string__element_3_interrupt);
  x13 = makeint(48L);
  gblt_add(x10,x13,x12,write__string__element_3_interrupt);
  allocp[0] = x15 = makeref(&allocp[0]);
  allocp[1] = x12;
  x14 = makecons(&allocp[0]);
  allocp[2] = x14;
  allocp[3] = x8;
  x16 = makecons(&allocp[2]);
  allocp[4] = x16;
  allocp[5] = x2;
  x17 = makecons(&allocp[4]);
  allocp[6] = x17;
  allocp[7] = makeint(92L);
  x18 = makecons(&allocp[6]);
  allocp += 8;
  unify_value(a1, x18);
  unify(a2, x15);
  proceed();
 write__string__element_3_8:
  if (!isref(a0)) goto write__string__element_3_interrupt;
  deref_and_jump(a0,write__string__element_3_7);
  *reasonp++ =  a0;
  goto write__string__element_3_interrupt;
 write__string__element_3_ext_interrupt:
  reasonp = 0l;
 write__string__element_3_interrupt:
  toppred = &predicate_klic__comp__write_xwrite__string__element_3;
  goto interrupt_3;
 }

 write__pred__structs_4_top: {
  q x0, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12;
  a2 = qp->args[2];
  a3 = qp->args[3];
  qp = qp->next;
 write__pred__structs_4_clear_reason:
  reasonp = reasons;
 write__pred__structs_4_0:
 write__pred__structs_4_1:
  switch (ptagof(a0)) {
 case CONS:
  x0 = car_of(a0);
 write__pred__structs_4_2:
  switch (ptagof(x0)) {
 case FUNCTOR:
  if (functor_of(x0) != makesym(functor_pred_5)) goto write__pred__structs_4_interrupt;
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__write_xwrite__pred__structs_4);
  x1 = cdr_of(a0);
  allocp[2] = x1;
  allocp[3] = a1;
  allocp[4] = x2 = makeref(&allocp[4]);
  allocp[5] = a3;
  allocp[6] = NILATOM;
  x4 = arg(x0, 1);
  allocp[7] = x4;
  x3 = makecons(&allocp[6]);
  allocp[8] = x3;
  x6 = arg(x0, 2);
  allocp[9] = x6;
  x5 = makecons(&allocp[8]);
  allocp[10] = x5;
  allocp[11] = a1;
  x7 = makecons(&allocp[10]);
  allocp[12] = x7;
  x9 = arg(x0, 1);
  allocp[13] = x9;
  x8 = makecons(&allocp[12]);
  allocp[14] = x8;
  x11 = arg(x0, 0);
  allocp[15] = x11;
  x10 = makecons(&allocp[14]);
  allocp[16] = x10;
  allocp[17] = a1;
  x12 = makecons(&allocp[16]);
  allocp[18] = (q)(struct goalrec*)&allocp[0];
  allocp[19] = (q)(&predicate_klic__comp__obj_xklicformat_4);
  allocp[20] = makefunctor(&string_const_36);
  allocp[21] = x12;
  allocp[22] = a2;
  allocp[23] = x2;
  qp = (struct goalrec*)&allocp[18];
  allocp += 24;
  proceed();
 case VARREF:
  deref_and_jump(x0,write__pred__structs_4_2);
  *reasonp++ =  x0;
  goto write__pred__structs_4_interrupt;
  };
  goto write__pred__structs_4_interrupt;
 case ATOMIC:
  if (a0 != NILATOM) goto write__pred__structs_4_interrupt;
  unify(a3, a2);
  proceed();
 case VARREF:
  deref_and_jump(a0,write__pred__structs_4_1);
  *reasonp++ =  a0;
  goto write__pred__structs_4_interrupt;
  };
  goto write__pred__structs_4_interrupt;
 write__pred__structs_4_ext_interrupt:
  reasonp = 0l;
 write__pred__structs_4_interrupt:
  goto interrupt_4;
 }

 declare__externals_3_top: {
  q x0, x1;
  a2 = qp->args[2];
  qp = qp->next;
 declare__externals_3_clear_reason:
  reasonp = reasons;
 declare__externals_3_0:
 declare__externals_3_1:
  switch (ptagof(a0)) {
 case CONS:
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__write_xdeclare__externals_3);
  x0 = cdr_of(a0);
  allocp[2] = x0;
  allocp[3] = x1 = makeref(&allocp[3]);
  allocp[4] = a2;
  a0 = car_of(a0);
  a2 = x1;
  qp = (struct goalrec*)&allocp[0];
  allocp += 5;
  execute(declare__one__external_3_0);
  goto declare__one__external_3_ext_interrupt;
 case ATOMIC:
  if (a0 != NILATOM) goto declare__externals_3_interrupt;
  unify(a2, a1);
  proceed();
 case VARREF:
  deref_and_jump(a0,declare__externals_3_1);
  *reasonp++ =  a0;
  goto declare__externals_3_interrupt;
  };
  goto declare__externals_3_interrupt;
 declare__externals_3_ext_interrupt:
  reasonp = 0l;
 declare__externals_3_interrupt:
  goto interrupt_3;
 }

 declare__one__external_3_top: {
  q x0, x1, x2, x3, x4, x5;
  a2 = qp->args[2];
  qp = qp->next;
 declare__one__external_3_clear_reason:
  reasonp = reasons;
 declare__one__external_3_0:
 declare__one__external_3_1:
  switch (ptagof(a0)) {
 case FUNCTOR:
  switch (symval(functor_of(a0))) {
 case functor_ext_3:
  allocp[0] = NILATOM;
  x1 = arg(a0, 2);
  allocp[1] = x1;
  x0 = makecons(&allocp[0]);
  allocp[2] = x0;
  x3 = arg(a0, 1);
  allocp[3] = x3;
  x2 = makecons(&allocp[2]);
  allocp[4] = x2;
  x5 = arg(a0, 0);
  allocp[5] = x5;
  x4 = makecons(&allocp[4]);
  allocp[6] = (q)qp;
  allocp[7] = (q)(&predicate_klic__comp__obj_xklicformat_4);
  allocp[8] = makefunctor(&string_const_40);
  allocp[9] = x4;
  allocp[10] = a1;
  allocp[11] = a2;
  qp = (struct goalrec*)&allocp[6];
  allocp += 12;
  proceed();
 case functor_gnew_1:
  allocp[0] = NILATOM;
  x1 = arg(a0, 0);
  allocp[1] = x1;
  x0 = makecons(&allocp[0]);
  allocp[2] = (q)qp;
  allocp[3] = (q)(&predicate_klic__comp__obj_xklicformat_4);
  allocp[4] = makefunctor(&string_const_41);
  allocp[5] = x0;
  allocp[6] = a1;
  allocp[7] = a2;
  qp = (struct goalrec*)&allocp[2];
  allocp += 8;
  proceed();
 case functor_guse_1:
  allocp[0] = NILATOM;
  x1 = arg(a0, 0);
  allocp[1] = x1;
  x0 = makecons(&allocp[0]);
  allocp[2] = (q)qp;
  allocp[3] = (q)(&predicate_klic__comp__obj_xklicformat_4);
  allocp[4] = makefunctor(&string_const_39);
  allocp[5] = x0;
  allocp[6] = a1;
  allocp[7] = a2;
  qp = (struct goalrec*)&allocp[2];
  allocp += 8;
  proceed();
  };
  goto declare__one__external_3_2;
 case VARREF:
  deref_and_jump(a0,declare__one__external_3_1);
  *reasonp++ =  a0;
  goto declare__one__external_3_2;
  };
  goto declare__one__external_3_2;
 declare__one__external_3_2:
  otherwise(declare__one__external_3_interrupt);
  unify(a2, a1);
  proceed();
 declare__one__external_3_ext_interrupt:
  reasonp = 0l;
 declare__one__external_3_interrupt:
  toppred = &predicate_klic__comp__write_xdeclare__one__external_3;
  goto interrupt_3;
 }

 pred__min__max_5_top: {
  q x0, x1, x2, x3, x4;
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  qp = qp->next;
 pred__min__max_5_clear_reason:
  reasonp = reasons;
 pred__min__max_5_0:
 pred__min__max_5_1:
  switch (ptagof(a0)) {
 case CONS:
  x0 = car_of(a0);
 pred__min__max_5_2:
  switch (ptagof(x0)) {
 case FUNCTOR:
  if (functor_of(x0) != makesym(functor_pred_5)) goto pred__min__max_5_interrupt;
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__write_xpred__min__max_5);
  x1 = cdr_of(a0);
  allocp[2] = x1;
  allocp[3] = x2 = makeref(&allocp[3]);
  allocp[4] = a2;
  allocp[5] = x3 = makeref(&allocp[5]);
  allocp[6] = a4;
  allocp[7] = (q)(struct goalrec*)&allocp[0];
  allocp[8] = (q)(&predicate_klic__comp__util_xmax_3);
  x4 = arg(x0, 1);
  allocp[9] = x4;
  allocp[10] = a3;
  allocp[11] = x3;
  allocp[12] = (q)(struct goalrec*)&allocp[7];
  allocp[13] = (q)(&predicate_klic__comp__util_xmin_3);
  allocp[14] = x4;
  allocp[15] = a1;
  allocp[16] = x2;
  qp = (struct goalrec*)&allocp[12];
  allocp += 17;
  proceed();
 case VARREF:
  deref_and_jump(x0,pred__min__max_5_2);
  *reasonp++ =  x0;
  goto pred__min__max_5_interrupt;
  };
  goto pred__min__max_5_interrupt;
 case ATOMIC:
  if (a0 != NILATOM) goto pred__min__max_5_interrupt;
  unify2(a2, a1, a4, a3);
  proceed();
 case VARREF:
  deref_and_jump(a0,pred__min__max_5_1);
  *reasonp++ =  a0;
  goto pred__min__max_5_interrupt;
  };
  goto pred__min__max_5_interrupt;
 pred__min__max_5_ext_interrupt:
  reasonp = 0l;
 pred__min__max_5_interrupt:
  goto interrupt_5;
 }

 declare__registers_4_top: {
  q x0, x1, x2;
  a2 = qp->args[2];
  a3 = qp->args[3];
  qp = qp->next;
 declare__registers_4_clear_reason:
  reasonp = reasons;
 declare__registers_4_0:
 declare__registers_4_1:
  if (!isint(a0)) goto declare__registers_4_3;
  x0 = makeint(0L);
  gblt_less_or_eq(a0,x0,declare__registers_4_2);
  unify(a3, a2);
  proceed();
 declare__registers_4_2:
  x1 = makeint(0L);
  gblt_greater(a0,x1,declare__registers_4_interrupt);
  allocp[0] = a3;
  allocp[1] = makefunctor(funct_const_69);
  x2 = makecons(&allocp[0]);
  a3 = a2;
  a4 = x2;
  a2 = makefunctor(&string_const_21);
  allocp += 2;
  execute(write__comma__list_5_0);
  goto write__comma__list_5_ext_interrupt;
 declare__registers_4_3:
  if (!isref(a0)) goto declare__registers_4_interrupt;
  deref_and_jump(a0,declare__registers_4_1);
  *reasonp++ =  a0;
  goto declare__registers_4_interrupt;
 declare__registers_4_ext_interrupt:
  reasonp = 0l;
 declare__registers_4_interrupt:
  goto interrupt_4;
 }

 write__comma__list_5_top: {
  q x0, x1, x2, x3, x4, x5, x6, x7, x8;
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  qp = qp->next;
 write__comma__list_5_clear_reason:
  reasonp = reasons;
 write__comma__list_5_0:
 write__comma__list_5_1:
  if (!isint(a0)) goto write__comma__list_5_3;
  x0 = makeint(0L);
  gblt_less_or_eq(a0,x0,write__comma__list_5_2);
  unify(a4, a3);
  proceed();
 write__comma__list_5_2:
  x1 = makeint(0L);
  gblt_greater(a0,x1,write__comma__list_5_interrupt);
  x2 = makeint(1L);
  bblt_sub_no_check(a0,x2,x3);
  allocp[0] = NILATOM;
  allocp[1] = x3;
  x4 = makecons(&allocp[0]);
  allocp[2] = x4;
  allocp[3] = a1;
  x5 = makecons(&allocp[2]);
  allocp[4] = (q)qp;
  allocp[5] = (q)(&predicate_klic__comp__obj_xklicformat_4);
  allocp[6] = makefunctor(&string_const_54);
  allocp[7] = x5;
  allocp[8] = x6 = makeref(&allocp[8]);
  allocp[9] = a4;
  allocp[10] = (q)(struct goalrec*)&allocp[4];
  allocp[11] = (q)(&predicate_klic__comp__write_xwrite__comma__list1_5);
  allocp[12] = makeint(0L);
  allocp[13] = x3;
  allocp[14] = a1;
  allocp[15] = x7 = makeref(&allocp[15]);
  allocp[16] = x6;
  allocp[17] = NILATOM;
  allocp[18] = a2;
  x8 = makecons(&allocp[17]);
  allocp[19] = (q)(struct goalrec*)&allocp[10];
  allocp[20] = (q)(&predicate_klic__comp__obj_xklicformat_4);
  allocp[21] = makefunctor(&string_const_53);
  allocp[22] = x8;
  allocp[23] = a3;
  allocp[24] = x7;
  qp = (struct goalrec*)&allocp[19];
  allocp += 25;
  proceed();
 write__comma__list_5_3:
  if (!isref(a0)) goto write__comma__list_5_interrupt;
  deref_and_jump(a0,write__comma__list_5_1);
  *reasonp++ =  a0;
  goto write__comma__list_5_interrupt;
 write__comma__list_5_ext_interrupt:
  reasonp = 0l;
 write__comma__list_5_interrupt:
  toppred = &predicate_klic__comp__write_xwrite__comma__list_5;
  goto interrupt_5;
 }

 write__comma__list1_5_top: {
  q x0, x1, x2, x3, x4;
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  qp = qp->next;
 write__comma__list1_5_clear_reason:
  reasonp = reasons;
 write__comma__list1_5_0:
 write__comma__list1_5_1:
  if (!isint(a0)) goto write__comma__list1_5_5;
 write__comma__list1_5_2:
  if (!isint(a1)) goto write__comma__list1_5_4;
  gblt_greater_or_eq(a0,a1,write__comma__list1_5_3);
  unify(a4, a3);
  proceed();
 write__comma__list1_5_3:
  gblt_less(a0,a1,write__comma__list1_5_interrupt);
  x0 = makeint(1L);
  bblt_add_no_check(a0,x0,x1);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__write_xwrite__comma__list1_5);
  allocp[2] = x1;
  allocp[3] = a1;
  allocp[4] = a2;
  allocp[5] = x2 = makeref(&allocp[5]);
  allocp[6] = a4;
  allocp[7] = NILATOM;
  allocp[8] = a0;
  x3 = makecons(&allocp[7]);
  allocp[9] = x3;
  allocp[10] = a2;
  x4 = makecons(&allocp[9]);
  allocp[11] = (q)(struct goalrec*)&allocp[0];
  allocp[12] = (q)(&predicate_klic__comp__obj_xklicformat_4);
  allocp[13] = makefunctor(&string_const_55);
  allocp[14] = x4;
  allocp[15] = a3;
  allocp[16] = x2;
  qp = (struct goalrec*)&allocp[11];
  allocp += 17;
  proceed();
 write__comma__list1_5_4:
  if (!isref(a1)) goto write__comma__list1_5_interrupt;
  deref_and_jump(a1,write__comma__list1_5_2);
  *reasonp++ =  a1;
  goto write__comma__list1_5_interrupt;
 write__comma__list1_5_5:
  if (!isref(a0)) goto write__comma__list1_5_interrupt;
  deref_and_jump(a0,write__comma__list1_5_1);
  *reasonp++ =  a0;
  goto write__comma__list1_5_interrupt;
 write__comma__list1_5_ext_interrupt:
  reasonp = 0l;
 write__comma__list1_5_interrupt:
  goto interrupt_5;
 }

 write__pred__dispatch_3_top: {
  q x0, x1, x2, x3, x4, x5, x6, x7, x8, x9;
  a2 = qp->args[2];
  qp = qp->next;
 write__pred__dispatch_3_clear_reason:
  reasonp = reasons;
 write__pred__dispatch_3_0:
 write__pred__dispatch_3_1:
  switch (ptagof(a0)) {
 case CONS:
  x0 = car_of(a0);
 write__pred__dispatch_3_2:
  switch (ptagof(x0)) {
 case FUNCTOR:
  if (functor_of(x0) != makesym(functor_pred_5)) goto write__pred__dispatch_3_interrupt;
  x1 = cdr_of(a0);
 write__pred__dispatch_3_3:
  switch (ptagof(x1)) {
 case ATOMIC:
  if (x1 != NILATOM) goto write__pred__dispatch_3_4;
  allocp[0] = NILATOM;
  x3 = arg(x0, 1);
  allocp[1] = x3;
  x2 = makecons(&allocp[0]);
  allocp[2] = x2;
  x5 = arg(x0, 0);
  allocp[3] = x5;
  x4 = makecons(&allocp[2]);
  allocp[4] = x4;
  x7 = arg(x0, 2);
  allocp[5] = x7;
  x6 = makecons(&allocp[4]);
  allocp[6] = (q)qp;
  allocp[7] = (q)(&predicate_klic__comp__obj_xklicformat_4);
  allocp[8] = makefunctor(&string_const_2);
  allocp[9] = x6;
  allocp[10] = a1;
  allocp[11] = a2;
  qp = (struct goalrec*)&allocp[6];
  allocp += 12;
  proceed();
 case VARREF:
  deref_and_jump(x1,write__pred__dispatch_3_3);
  *reasonp++ =  x1;
  goto write__pred__dispatch_3_interrupt;
  };
  goto write__pred__dispatch_3_4;
 write__pred__dispatch_3_4:
  x2 = NILATOM;
  gblt_diff(x1,x2,write__pred__dispatch_3_interrupt);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__write_xwrite__pred__dispatch_3);
  allocp[2] = x1;
  allocp[3] = x3 = makeref(&allocp[3]);
  allocp[4] = a2;
  allocp[5] = NILATOM;
  x5 = arg(x0, 1);
  allocp[6] = x5;
  x4 = makecons(&allocp[5]);
  allocp[7] = x4;
  x7 = arg(x0, 0);
  allocp[8] = x7;
  x6 = makecons(&allocp[7]);
  allocp[9] = x6;
  x9 = arg(x0, 2);
  allocp[10] = x9;
  x8 = makecons(&allocp[9]);
  allocp[11] = (q)(struct goalrec*)&allocp[0];
  allocp[12] = (q)(&predicate_klic__comp__obj_xklicformat_4);
  allocp[13] = makefunctor(&string_const_1);
  allocp[14] = x8;
  allocp[15] = a1;
  allocp[16] = x3;
  qp = (struct goalrec*)&allocp[11];
  allocp += 17;
  proceed();
 case VARREF:
  deref_and_jump(x0,write__pred__dispatch_3_2);
  *reasonp++ =  x0;
  goto write__pred__dispatch_3_interrupt;
  };
  goto write__pred__dispatch_3_interrupt;
 case VARREF:
  deref_and_jump(a0,write__pred__dispatch_3_1);
  *reasonp++ =  a0;
  goto write__pred__dispatch_3_interrupt;
  };
  goto write__pred__dispatch_3_interrupt;
 write__pred__dispatch_3_ext_interrupt:
  reasonp = 0l;
 write__pred__dispatch_3_interrupt:
  goto interrupt_3;
 }

 write__predicates_6_top: {
  q x0, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13, x14, x15, x16, x17, x18, x19, x20, x21, x22, x23, x24, x25, x26, x27, x28, x29;
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  qp = qp->next;
 write__predicates_6_clear_reason:
  reasonp = reasons;
 write__predicates_6_0:
 write__predicates_6_1:
  switch (ptagof(a0)) {
 case CONS:
  x0 = car_of(a0);
 write__predicates_6_2:
  switch (ptagof(x0)) {
 case FUNCTOR:
  if (functor_of(x0) != makesym(functor_pred_5)) goto write__predicates_6_interrupt;
  allocp[0] = x2 = makeref(&allocp[0]);
  allocp[1] = makefunctor(funct_const_57);
  x1 = makecons(&allocp[0]);
  allocp += 2;
  unify_value(a4, x1);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__write_xwrite__predicates_6);
  x3 = cdr_of(a0);
  allocp[2] = x3;
  allocp[3] = a1;
  allocp[4] = a2;
  allocp[5] = a3;
  allocp[6] = x4 = makeref(&allocp[6]);
  allocp[7] = a5;
  allocp[8] = NILATOM;
  x6 = arg(x0, 1);
  allocp[9] = x6;
  x5 = makecons(&allocp[8]);
  allocp[10] = (q)(struct goalrec*)&allocp[0];
  allocp[11] = (q)(&predicate_klic__comp__obj_xklicformat_4);
  allocp[12] = makefunctor(&string_const_20);
  allocp[13] = x5;
  allocp[14] = x7 = makeref(&allocp[14]);
  allocp[15] = x4;
  allocp[16] = (q)(struct goalrec*)&allocp[10];
  allocp[17] = (q)(&predicate_klic__comp__write_xwrite__predicates__1_6);
  allocp[18] = x8 = makeref(&allocp[18]);
  allocp[19] = a1;
  x9 = arg(x0, 0);
  allocp[20] = x9;
  x10 = arg(x0, 1);
  allocp[21] = x10;
  allocp[22] = x11 = makeref(&allocp[22]);
  allocp[23] = x7;
  allocp[24] = makesym(functor__2F_2);
  allocp[25] = x9;
  allocp[26] = x10;
  x12 = makefunctor(&allocp[24]);
  allocp[27] = makesym(functor_exec_1);
  allocp[28] = x12;
  x13 = makefunctor(&allocp[27]);
  allocp[29] = (q)(struct goalrec*)&allocp[16];
  allocp[30] = (q)(&predicate_klic__comp__util_xmember_3);
  allocp[31] = a3;
  allocp[32] = x13;
  allocp[33] = x8;
  allocp[34] = NILATOM;
  allocp[35] = x10;
  x14 = makecons(&allocp[34]);
  allocp[36] = x14;
  allocp[37] = x9;
  x15 = makecons(&allocp[36]);
  allocp[38] = x15;
  allocp[39] = x10;
  x16 = makecons(&allocp[38]);
  allocp[40] = x16;
  allocp[41] = x9;
  x17 = makecons(&allocp[40]);
  allocp[42] = (q)(struct goalrec*)&allocp[29];
  allocp[43] = (q)(&predicate_klic__comp__obj_xklicformat_4);
  allocp[44] = makefunctor(&string_const_34);
  allocp[45] = x17;
  allocp[46] = x18 = makeref(&allocp[46]);
  allocp[47] = x11;
  allocp[48] = makesym(functor_info_3);
  allocp[49] = a1;
  allocp[50] = x9;
  allocp[51] = x10;
  x19 = makefunctor(&allocp[48]);
  allocp[52] = (q)(struct goalrec*)&allocp[42];
  allocp[53] = (q)(&predicate_klic__comp__obj_xwrite__object_4);
  x20 = arg(x0, 4);
  allocp[54] = x20;
  allocp[55] = x19;
  allocp[56] = x21 = makeref(&allocp[56]);
  allocp[57] = x18;
  allocp[58] = NILATOM;
  allocp[59] = x10;
  x22 = makecons(&allocp[58]);
  allocp[60] = x22;
  allocp[61] = x9;
  x23 = makecons(&allocp[60]);
  allocp[62] = (q)(struct goalrec*)&allocp[52];
  allocp[63] = (q)(&predicate_klic__comp__obj_xklicformat_4);
  allocp[64] = makefunctor(&string_const_22);
  allocp[65] = x23;
  allocp[66] = x24 = makeref(&allocp[66]);
  allocp[67] = x21;
  allocp[68] = (q)(struct goalrec*)&allocp[62];
  allocp[69] = (q)(&predicate_klic__comp__write_xwrite__get__args_4);
  allocp[70] = a2;
  allocp[71] = x10;
  allocp[72] = x25 = makeref(&allocp[72]);
  allocp[73] = x24;
  allocp[74] = (q)(struct goalrec*)&allocp[68];
  allocp[75] = (q)(&predicate_klic__comp__write_xdeclare__registers_4);
  x26 = arg(x0, 3);
  allocp[76] = x26;
  allocp[77] = makefunctor(&string_const_51);
  allocp[78] = x27 = makeref(&allocp[78]);
  allocp[79] = x25;
  allocp[80] = NILATOM;
  allocp[81] = x10;
  x28 = makecons(&allocp[80]);
  allocp[82] = x28;
  allocp[83] = x9;
  x29 = makecons(&allocp[82]);
  allocp[84] = (q)(struct goalrec*)&allocp[74];
  allocp[85] = (q)(&predicate_klic__comp__obj_xklicformat_4);
  allocp[86] = makefunctor(&string_const_35);
  allocp[87] = x29;
  allocp[88] = x2;
  allocp[89] = x27;
  qp = (struct goalrec*)&allocp[84];
  allocp += 90;
  proceed();
 case VARREF:
  deref_and_jump(x0,write__predicates_6_2);
  *reasonp++ =  x0;
  goto write__predicates_6_interrupt;
  };
  goto write__predicates_6_interrupt;
 case ATOMIC:
  if (a0 != NILATOM) goto write__predicates_6_interrupt;
  unify(a5, a4);
  proceed();
 case VARREF:
  deref_and_jump(a0,write__predicates_6_1);
  *reasonp++ =  a0;
  goto write__predicates_6_interrupt;
  };
  goto write__predicates_6_interrupt;
 write__predicates_6_ext_interrupt:
  reasonp = 0l;
 write__predicates_6_interrupt:
  goto interrupt_6;
 }

 write__predicates__1_6_top: {
  q x0, x1, x2;
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  qp = qp->next;
 write__predicates__1_6_clear_reason:
  reasonp = reasons;
 write__predicates__1_6_0:
 write__predicates__1_6_1:
  switch (ptagof(a0)) {
 case ATOMIC:
  if (isint(a0)) goto write__predicates__1_6_interrupt;
  switch (symval(a0)) {
 case symval(makesym(atom_yes)):
  allocp[0] = NILATOM;
  allocp[1] = a3;
  x0 = makecons(&allocp[0]);
  allocp[2] = x0;
  allocp[3] = a2;
  x1 = makecons(&allocp[2]);
  allocp[4] = x1;
  allocp[5] = a1;
  x2 = makecons(&allocp[4]);
  allocp[6] = (q)qp;
  allocp[7] = (q)(&predicate_klic__comp__obj_xklicformat_4);
  allocp[8] = makefunctor(&string_const_28);
  allocp[9] = x2;
  allocp[10] = a4;
  allocp[11] = a5;
  qp = (struct goalrec*)&allocp[6];
  allocp += 12;
  proceed();
 case symval(makesym(atom_no)):
  unify(a5, a4);
  proceed();
  };
  goto write__predicates__1_6_interrupt;
 case VARREF:
  deref_and_jump(a0,write__predicates__1_6_1);
  *reasonp++ =  a0;
  goto write__predicates__1_6_interrupt;
  };
  goto write__predicates__1_6_interrupt;
 write__predicates__1_6_ext_interrupt:
  reasonp = 0l;
 write__predicates__1_6_interrupt:
  goto interrupt_6;
 }

 write__get__args_4_top: {
  q x0, x1, x2, x3;
  a2 = qp->args[2];
  a3 = qp->args[3];
  qp = qp->next;
 write__get__args_4_clear_reason:
  reasonp = reasons;
 write__get__args_4_0:
 write__get__args_4_1:
  if (!isint(a0)) goto write__get__args_4_5;
 write__get__args_4_2:
  if (!isint(a1)) goto write__get__args_4_4;
  gblt_eq(a0,a1,write__get__args_4_3);
  unify(a3, a2);
  proceed();
 write__get__args_4_3:
  gblt_less(a0,a1,write__get__args_4_interrupt);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__write_xwrite__get__args_4);
  allocp[2] = x0 = makeref(&allocp[2]);
  allocp[3] = a1;
  allocp[4] = x1 = makeref(&allocp[4]);
  allocp[5] = a3;
  allocp[6] = (q)(struct goalrec*)&allocp[0];
  allocp[7] = (q)(&predicate_integer__arithmetics_xadd_3);
  allocp[8] = a0;
  allocp[9] = makeint(1L);
  allocp[10] = x0;
  allocp[11] = NILATOM;
  allocp[12] = a0;
  x2 = makecons(&allocp[11]);
  allocp[13] = x2;
  allocp[14] = a0;
  x3 = makecons(&allocp[13]);
  allocp[15] = (q)(struct goalrec*)&allocp[6];
  allocp[16] = (q)(&predicate_klic__comp__obj_xklicformat_4);
  allocp[17] = makefunctor(&string_const_12);
  allocp[18] = x3;
  allocp[19] = a2;
  allocp[20] = x1;
  qp = (struct goalrec*)&allocp[15];
  allocp += 21;
  proceed();
 write__get__args_4_4:
  if (!isref(a1)) goto write__get__args_4_interrupt;
  deref_and_jump(a1,write__get__args_4_2);
  *reasonp++ =  a1;
  goto write__get__args_4_interrupt;
 write__get__args_4_5:
  if (!isref(a0)) goto write__get__args_4_interrupt;
  deref_and_jump(a0,write__get__args_4_1);
  *reasonp++ =  a0;
  goto write__get__args_4_interrupt;
 write__get__args_4_ext_interrupt:
  reasonp = 0l;
 write__get__args_4_interrupt:
  goto interrupt_4;
 }

 write__interrupt__call_4_top: {
  q x0, x1, x2, x3, x4, x5, x6, x7, x8, x9;
  a2 = qp->args[2];
  a3 = qp->args[3];
  qp = qp->next;
 write__interrupt__call_4_clear_reason:
  reasonp = reasons;
 write__interrupt__call_4_0:
 write__interrupt__call_4_1:
  if (!isint(a0)) goto write__interrupt__call_4_3;
  x0 = makeint(0L);
  gblt_less_or_eq(a0,x0,write__interrupt__call_4_2);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__obj_xklicformat_3);
  allocp[2] = makefunctor(&string_const_31);
  allocp[3] = a2;
  allocp[4] = a3;
  qp = (struct goalrec*)&allocp[0];
  allocp += 5;
  proceed();
 write__interrupt__call_4_2:
  x1 = makeint(0L);
  gblt_greater(a0,x1,write__interrupt__call_4_interrupt);
  x2 = makeint(1L);
  bblt_sub_no_check(a0,x2,x3);
  x4 = makeint(2L);
  bblt_add_no_check(x3,x4,x5);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__write_xwrite__interrupt__call_4);
  allocp[2] = x3;
  allocp[3] = a1;
  allocp[4] = x6 = makeref(&allocp[4]);
  allocp[5] = a3;
  allocp[6] = NILATOM;
  allocp[7] = x3;
  x7 = makecons(&allocp[6]);
  allocp[8] = x7;
  allocp[9] = x5;
  x8 = makecons(&allocp[8]);
  allocp[10] = x8;
  allocp[11] = a0;
  x9 = makecons(&allocp[10]);
  allocp[12] = (q)(struct goalrec*)&allocp[0];
  allocp[13] = (q)(&predicate_klic__comp__obj_xklicformat_4);
  allocp[14] = makefunctor(&string_const_32);
  allocp[15] = x9;
  allocp[16] = a2;
  allocp[17] = x6;
  qp = (struct goalrec*)&allocp[12];
  allocp += 18;
  proceed();
 write__interrupt__call_4_3:
  if (!isref(a0)) goto write__interrupt__call_4_interrupt;
  deref_and_jump(a0,write__interrupt__call_4_1);
  *reasonp++ =  a0;
  goto write__interrupt__call_4_interrupt;
 write__interrupt__call_4_ext_interrupt:
  reasonp = 0l;
 write__interrupt__call_4_interrupt:
  goto interrupt_4;
 }

 write__header__file_3_top: {
  q x0, x1, x2, x3, x4, x5, x6, x7;
  a2 = qp->args[2];
  qp = qp->next;
 write__header__file_3_clear_reason:
  reasonp = reasons;
 write__header__file_3_0:
 write__header__file_3_1:
  switch (ptagof(a0)) {
 case FUNCTOR:
  if (functor_of(a0) != makesym(functor_module_3)) goto write__header__file_3_interrupt;
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__write_xwrite__ext__entries_3);
  allocp[2] = x0 = makeref(&allocp[2]);
  allocp[3] = x1 = makeref(&allocp[3]);
  allocp[4] = a2;
  allocp[5] = (q)(struct goalrec*)&allocp[0];
  allocp[6] = (q)(&predicate_klic__comp__util_xqsort_2);
  allocp[7] = x2 = makeref(&allocp[7]);
  allocp[8] = x0;
  allocp[9] = (q)(struct goalrec*)&allocp[5];
  allocp[10] = (q)(&predicate_klic__comp__extern_xmake__ext__table_3);
  x3 = arg(a0, 2);
  allocp[11] = x3;
  allocp[12] = x2;
  allocp[13] = NILATOM;
  allocp[14] = (q)(struct goalrec*)&allocp[9];
  allocp[15] = (q)(&predicate_klic__comp__write_xwrite__defined__preds_4);
  x4 = arg(a0, 1);
  allocp[16] = x4;
  x5 = arg(a0, 0);
  allocp[17] = x5;
  allocp[18] = x6 = makeref(&allocp[18]);
  allocp[19] = x1;
  allocp[20] = NILATOM;
  allocp[21] = x5;
  x7 = makecons(&allocp[20]);
  allocp[22] = (q)(struct goalrec*)&allocp[14];
  allocp[23] = (q)(&predicate_klic__comp__obj_xklicformat_4);
  allocp[24] = makefunctor(&string_const_47);
  allocp[25] = x7;
  allocp[26] = a1;
  allocp[27] = x6;
  qp = (struct goalrec*)&allocp[22];
  allocp += 28;
  proceed();
 case VARREF:
  deref_and_jump(a0,write__header__file_3_1);
  *reasonp++ =  a0;
  goto write__header__file_3_interrupt;
  };
  goto write__header__file_3_interrupt;
 write__header__file_3_ext_interrupt:
  reasonp = 0l;
 write__header__file_3_interrupt:
  goto interrupt_3;
 }

 write__ext__entries_3_top: {
  q x0, x1;
  a2 = qp->args[2];
  qp = qp->next;
 write__ext__entries_3_clear_reason:
  reasonp = reasons;
 write__ext__entries_3_0:
 write__ext__entries_3_1:
  switch (ptagof(a0)) {
 case CONS:
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__write_xwrite__ext__entries_3);
  x0 = cdr_of(a0);
  allocp[2] = x0;
  allocp[3] = x1 = makeref(&allocp[3]);
  allocp[4] = a2;
  a0 = car_of(a0);
  a2 = x1;
  qp = (struct goalrec*)&allocp[0];
  allocp += 5;
  execute(write__ext__entry_3_0);
  goto write__ext__entry_3_ext_interrupt;
 case ATOMIC:
  if (a0 != NILATOM) goto write__ext__entries_3_interrupt;
  unify(a2, a1);
  proceed();
 case VARREF:
  deref_and_jump(a0,write__ext__entries_3_1);
  *reasonp++ =  a0;
  goto write__ext__entries_3_interrupt;
  };
  goto write__ext__entries_3_interrupt;
 write__ext__entries_3_ext_interrupt:
  reasonp = 0l;
 write__ext__entries_3_interrupt:
  goto interrupt_3;
 }

 write__ext__entry_3_top: {
  q x0, x1, x2;
  a2 = qp->args[2];
  qp = qp->next;
 write__ext__entry_3_clear_reason:
  reasonp = reasons;
 write__ext__entry_3_0:
 write__ext__entry_3_1:
  switch (ptagof(a0)) {
 case FUNCTOR:
  switch (symval(functor_of(a0))) {
 case functor_atom_1:
  allocp[0] = NILATOM;
  x1 = arg(a0, 0);
  allocp[1] = x1;
  x0 = makecons(&allocp[0]);
  allocp[2] = (q)qp;
  allocp[3] = (q)(&predicate_klic__comp__obj_xklicformat_4);
  allocp[4] = makefunctor(&string_const_38);
  allocp[5] = x0;
  allocp[6] = a1;
  allocp[7] = a2;
  qp = (struct goalrec*)&allocp[2];
  allocp += 8;
  proceed();
 case functor_funct_1:
  x0 = arg(a0, 0);
 write__ext__entry_3_2:
  switch (ptagof(x0)) {
 case FUNCTOR:
  if (functor_of(x0) != makesym(functor__2F_2)) goto write__ext__entry_3_interrupt;
  allocp[0] = a2;
  allocp[1] = makeint(10L);
  x1 = makecons(&allocp[0]);
  a0 = arg(x0, 0);
  x2 = arg(x0, 1);
  a2 = a1;
  a3 = x1;
  a1 = x2;
  allocp += 2;
  execute(write__funct__name_4_0);
  goto write__funct__name_4_ext_interrupt;
 case VARREF:
  deref_and_jump(x0,write__ext__entry_3_2);
  *reasonp++ =  x0;
  goto write__ext__entry_3_interrupt;
  };
  goto write__ext__entry_3_interrupt;
 case functor_module_1:
  allocp[0] = NILATOM;
  x1 = arg(a0, 0);
  allocp[1] = x1;
  x0 = makecons(&allocp[0]);
  allocp[2] = (q)qp;
  allocp[3] = (q)(&predicate_klic__comp__obj_xklicformat_4);
  allocp[4] = makefunctor(&string_const_50);
  allocp[5] = x0;
  allocp[6] = a1;
  allocp[7] = a2;
  qp = (struct goalrec*)&allocp[2];
  allocp += 8;
  proceed();
 case functor_class_1:
  allocp[0] = NILATOM;
  x1 = arg(a0, 0);
  allocp[1] = x1;
  x0 = makecons(&allocp[0]);
  allocp[2] = (q)qp;
  allocp[3] = (q)(&predicate_klic__comp__obj_xklicformat_4);
  allocp[4] = makefunctor(&string_const_49);
  allocp[5] = x0;
  allocp[6] = a1;
  allocp[7] = a2;
  qp = (struct goalrec*)&allocp[2];
  allocp += 8;
  proceed();
  };
  goto write__ext__entry_3_interrupt;
 case VARREF:
  deref_and_jump(a0,write__ext__entry_3_1);
  *reasonp++ =  a0;
  goto write__ext__entry_3_interrupt;
  };
  goto write__ext__entry_3_interrupt;
 write__ext__entry_3_ext_interrupt:
  reasonp = 0l;
 write__ext__entry_3_interrupt:
  toppred = &predicate_klic__comp__write_xwrite__ext__entry_3;
  goto interrupt_3;
 }

 write__defined__preds_4_top: {
  q x0, x1, x2, x3, x4, x5, x6, x7;
  a2 = qp->args[2];
  a3 = qp->args[3];
  qp = qp->next;
 write__defined__preds_4_clear_reason:
  reasonp = reasons;
 write__defined__preds_4_0:
 write__defined__preds_4_1:
  switch (ptagof(a0)) {
 case CONS:
  x0 = car_of(a0);
 write__defined__preds_4_2:
  switch (ptagof(x0)) {
 case FUNCTOR:
  if (functor_of(x0) != makesym(functor_pred_5)) goto write__defined__preds_4_interrupt;
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__write_xwrite__defined__preds_4);
  x1 = cdr_of(a0);
  allocp[2] = x1;
  allocp[3] = a1;
  allocp[4] = x2 = makeref(&allocp[4]);
  allocp[5] = a3;
  allocp[6] = NILATOM;
  x4 = arg(x0, 1);
  allocp[7] = x4;
  x3 = makecons(&allocp[6]);
  allocp[8] = x3;
  x6 = arg(x0, 0);
  allocp[9] = x6;
  x5 = makecons(&allocp[8]);
  allocp[10] = x5;
  allocp[11] = a1;
  x7 = makecons(&allocp[10]);
  allocp[12] = (q)(struct goalrec*)&allocp[0];
  allocp[13] = (q)(&predicate_klic__comp__obj_xklicformat_4);
  allocp[14] = makefunctor(&string_const_52);
  allocp[15] = x7;
  allocp[16] = a2;
  allocp[17] = x2;
  qp = (struct goalrec*)&allocp[12];
  allocp += 18;
  proceed();
 case VARREF:
  deref_and_jump(x0,write__defined__preds_4_2);
  *reasonp++ =  x0;
  goto write__defined__preds_4_interrupt;
  };
  goto write__defined__preds_4_interrupt;
 case ATOMIC:
  if (a0 != NILATOM) goto write__defined__preds_4_interrupt;
  unify(a3, a2);
  proceed();
 case VARREF:
  deref_and_jump(a0,write__defined__preds_4_1);
  *reasonp++ =  a0;
  goto write__defined__preds_4_interrupt;
  };
  goto write__defined__preds_4_interrupt;
 write__defined__preds_4_ext_interrupt:
  reasonp = 0l;
 write__defined__preds_4_interrupt:
  goto interrupt_4;
 }

 write__atom_3_top: {
  q x0;
  a2 = qp->args[2];
  qp = qp->next;
 write__atom_3_clear_reason:
  reasonp = reasons;
 write__atom_3_0:
 write__atom_3_1:
  switch (ptagof(a0)) {
 case ATOMIC:
  if (isint(a0)) goto write__atom_3_2;
  switch (symval(a0)) {
 case symval(NILATOM):
  allocp[0] = a2;
  allocp[1] = makefunctor(funct_const_71);
  x0 = makecons(&allocp[0]);
  allocp += 2;
  unify_value(a1, x0);
  proceed();
 case symval(PERIODATOM):
  allocp[0] = a2;
  allocp[1] = makefunctor(funct_const_73);
  x0 = makecons(&allocp[0]);
  allocp += 2;
  unify_value(a1, x0);
  proceed();
  };
  goto write__atom_3_3;
 write__atom_3_2:
  gblt_integer(a0,write__atom_3_3);
  allocp[0] = NILATOM;
  allocp[1] = a0;
  x0 = makecons(&allocp[0]);
  allocp[2] = (q)qp;
  allocp[3] = (q)(&predicate_klic__comp__obj_xklicformat_4);
  allocp[4] = makefunctor(&string_const_43);
  allocp[5] = x0;
  allocp[6] = a1;
  allocp[7] = a2;
  qp = (struct goalrec*)&allocp[2];
  allocp += 8;
  proceed();
 case VARREF:
  deref_and_jump(a0,write__atom_3_1);
  *reasonp++ =  a0;
  goto write__atom_3_3;
  };
  goto write__atom_3_3;
 write__atom_3_3:
  otherwise(write__atom_3_interrupt);
  allocp[0] = NILATOM;
  allocp[1] = a0;
  x0 = makecons(&allocp[0]);
  allocp[2] = (q)qp;
  allocp[3] = (q)(&predicate_klic__comp__obj_xklicformat_4);
  allocp[4] = makefunctor(&string_const_44);
  allocp[5] = x0;
  allocp[6] = a1;
  allocp[7] = a2;
  qp = (struct goalrec*)&allocp[2];
  allocp += 8;
  proceed();
 write__atom_3_ext_interrupt:
  reasonp = 0l;
 write__atom_3_interrupt:
  goto interrupt_3;
 }

 write__funct__name_4_top: {
  q x0, x1;
  a2 = qp->args[2];
  a3 = qp->args[3];
  qp = qp->next;
 write__funct__name_4_clear_reason:
  reasonp = reasons;
 write__funct__name_4_0:
  allocp[0] = NILATOM;
  allocp[1] = a1;
  x0 = makecons(&allocp[0]);
  allocp[2] = x0;
  allocp[3] = a0;
  x1 = makecons(&allocp[2]);
  allocp[4] = (q)qp;
  allocp[5] = (q)(&predicate_klic__comp__obj_xklicformat_4);
  allocp[6] = makefunctor(&string_const_42);
  allocp[7] = x1;
  allocp[8] = a2;
  allocp[9] = a3;
  qp = (struct goalrec*)&allocp[4];
  allocp += 10;
  proceed();
 write__funct__name_4_ext_interrupt:
  reasonp = 0l;
 write__funct__name_4_interrupt:
  toppred = &predicate_klic__comp__write_xwrite__funct__name_4;
  goto interrupt_4;
 }

 write__pred__name_5_top: {
  q x0, x1, x2;
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  qp = qp->next;
 write__pred__name_5_clear_reason:
  reasonp = reasons;
 write__pred__name_5_0:
  allocp[0] = NILATOM;
  allocp[1] = a2;
  x0 = makecons(&allocp[0]);
  allocp[2] = x0;
  allocp[3] = a1;
  x1 = makecons(&allocp[2]);
  allocp[4] = x1;
  allocp[5] = a0;
  x2 = makecons(&allocp[4]);
  allocp[6] = (q)qp;
  allocp[7] = (q)(&predicate_klic__comp__obj_xklicformat_4);
  allocp[8] = makefunctor(&string_const_48);
  allocp[9] = x2;
  allocp[10] = a3;
  allocp[11] = a4;
  qp = (struct goalrec*)&allocp[6];
  allocp += 12;
  proceed();
 write__pred__name_5_ext_interrupt:
  reasonp = 0l;
 write__pred__name_5_interrupt:
  goto interrupt_5;
 }

 sort__info_2_top: {
  qp = qp->next;
 sort__info_2_clear_reason:
  reasonp = reasons;
 sort__info_2_0:
  a2 = NILATOM;
  execute(sort__info_3_0);
  goto sort__info_3_ext_interrupt;
 sort__info_2_ext_interrupt:
  reasonp = 0l;
 sort__info_2_interrupt:
  goto interrupt_2;
 }

 sort__info_3_top: {
  q x0, x1, x2, x3, x4, x5;
  a2 = qp->args[2];
  qp = qp->next;
 sort__info_3_clear_reason:
  reasonp = reasons;
 sort__info_3_0:
 sort__info_3_1:
  switch (ptagof(a0)) {
 case CONS:
  allocp[0] = x1 = makeref(&allocp[0]);
  x2 = car_of(a0);
  allocp[1] = x2;
  x0 = makecons(&allocp[0]);
  allocp[2] = (q)qp;
  allocp[3] = (q)(&predicate_klic__comp__write_xsort__info_3);
  allocp[4] = x3 = makeref(&allocp[4]);
  allocp[5] = a1;
  allocp[6] = x0;
  allocp[7] = (q)(struct goalrec*)&allocp[2];
  allocp[8] = (q)(&predicate_klic__comp__write_xsort__info_3);
  allocp[9] = x4 = makeref(&allocp[9]);
  allocp[10] = x1;
  allocp[11] = a2;
  x5 = cdr_of(a0);
  a1 = car_of(a0);
  a2 = x3;
  a3 = x4;
  a0 = x5;
  qp = (struct goalrec*)&allocp[7];
  allocp += 12;
  execute(partition_4_0);
  goto partition_4_ext_interrupt;
 case ATOMIC:
  if (a0 != NILATOM) goto sort__info_3_interrupt;
  unify(a1, a2);
  proceed();
 case VARREF:
  deref_and_jump(a0,sort__info_3_1);
  *reasonp++ =  a0;
  goto sort__info_3_interrupt;
  };
  goto sort__info_3_interrupt;
 sort__info_3_ext_interrupt:
  reasonp = 0l;
 sort__info_3_interrupt:
  toppred = &predicate_klic__comp__write_xsort__info_3;
  goto interrupt_3;
 }

 partition_4_top: {
  q x0, x1, x2, x3, x4, x5, x6, x7;
  a2 = qp->args[2];
  a3 = qp->args[3];
  qp = qp->next;
 partition_4_clear_reason:
  reasonp = reasons;
 partition_4_0:
 partition_4_1:
  switch (ptagof(a0)) {
 case CONS:
  x0 = car_of(a0);
 partition_4_2:
  switch (ptagof(x0)) {
 case FUNCTOR:
  if (functor_of(x0) != makesym(functor_const_2)) goto partition_4_12;
 partition_4_3:
  switch (ptagof(a1)) {
 case FUNCTOR:
  if (functor_of(a1) != makesym(functor_const_2)) goto partition_4_12;
  x1 = arg(x0, 1);
 partition_4_4:
  if (isref(x1)) goto partition_4_11;
 partition_4_5:
  if (isref(x1)) goto partition_4_6;
  x2 = arg(a1, 1);
  if_not_equal(x1, x2, partition_4_7);
  x3 = arg(x0, 0);
  x4 = arg(a1, 0);
  unify(x3, x4);
  a0 = cdr_of(a0);
  execute(partition_4_clear_reason);
  goto partition_4_ext_interrupt;
 partition_4_6:
  deref_and_jump(x1,partition_4_5);
  *reasonp++ =  x1;
  goto partition_4_7;
 partition_4_7:
  x2 = arg(a1, 1);
 partition_4_8:
  if (isref(x2)) goto partition_4_10;
  gblt_compare(x1,x2,x3,partition_4_12);
  x4 = makeint(0L);
  gblt_less(x3,x4,partition_4_9);
  allocp[0] = x6 = makeref(&allocp[0]);
  allocp[1] = x0;
  x5 = makecons(&allocp[0]);
  allocp += 2;
  unify_value(a2, x5);
  a0 = cdr_of(a0);
  a2 = x6;
  execute(partition_4_clear_reason);
  goto partition_4_ext_interrupt;
 partition_4_9:
  x5 = makeint(0L);
  gblt_greater(x3,x5,partition_4_12);
  allocp[0] = x7 = makeref(&allocp[0]);
  allocp[1] = x0;
  x6 = makecons(&allocp[0]);
  allocp += 2;
  unify_value(a3, x6);
  a0 = cdr_of(a0);
  a3 = x7;
  execute(partition_4_clear_reason);
  goto partition_4_ext_interrupt;
 partition_4_10:
  deref_and_jump(x2,partition_4_8);
  *reasonp++ =  x2;
  goto partition_4_12;
 partition_4_11:
  deref_and_jump(x1,partition_4_4);
  *reasonp++ =  x1;
  goto partition_4_12;
 case VARREF:
  deref_and_jump(a1,partition_4_3);
  *reasonp++ =  a1;
  goto partition_4_12;
  };
  goto partition_4_12;
 case VARREF:
  deref_and_jump(x0,partition_4_2);
  *reasonp++ =  x0;
  goto partition_4_interrupt;
  };
  goto partition_4_12;
 partition_4_12:
 partition_4_13:
  if (isref(x0)) goto partition_4_14;
  if_not_equal(x0, a1, partition_4_15);
  a0 = cdr_of(a0);
  execute(partition_4_clear_reason);
  goto partition_4_ext_interrupt;
 partition_4_14:
  deref_and_jump(x0,partition_4_13);
  *reasonp++ =  x0;
  goto partition_4_15;
 partition_4_15:
 partition_4_16:
  if (isref(a1)) goto partition_4_18;
  gblt_compare(x0,a1,x1,partition_4_interrupt);
  x2 = makeint(0L);
  gblt_less(x1,x2,partition_4_17);
  allocp[0] = x4 = makeref(&allocp[0]);
  allocp[1] = x0;
  x3 = makecons(&allocp[0]);
  allocp += 2;
  unify_value(a2, x3);
  a0 = cdr_of(a0);
  a2 = x4;
  execute(partition_4_clear_reason);
  goto partition_4_ext_interrupt;
 partition_4_17:
  x3 = makeint(0L);
  gblt_greater(x1,x3,partition_4_interrupt);
  allocp[0] = x5 = makeref(&allocp[0]);
  allocp[1] = x0;
  x4 = makecons(&allocp[0]);
  allocp += 2;
  unify_value(a3, x4);
  a0 = cdr_of(a0);
  a3 = x5;
  execute(partition_4_clear_reason);
  goto partition_4_ext_interrupt;
 partition_4_18:
  deref_and_jump(a1,partition_4_16);
  *reasonp++ =  a1;
  goto partition_4_interrupt;
 case ATOMIC:
  if (a0 != NILATOM) goto partition_4_interrupt;
  x0 = NILATOM;
  unify_value(a2, x0);
  x1 = NILATOM;
  unify_value(a3, x1);
  proceed();
 case VARREF:
  deref_and_jump(a0,partition_4_1);
  *reasonp++ =  a0;
  goto partition_4_interrupt;
  };
  goto partition_4_interrupt;
 partition_4_ext_interrupt:
  reasonp = 0l;
 partition_4_interrupt:
  toppred = &predicate_klic__comp__write_xpartition_4;
  goto interrupt_4;
 }
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
  loop_within_module(module_klic__comp__write);
}
