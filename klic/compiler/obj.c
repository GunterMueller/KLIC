/* Compiled by KLIC compiler version 3.003 (Mon Mar 29 19:42:59 JST 1999) */
#include <klic/klichdr.h>
#include "atom.h"
#include "funct.h"
#include <ctype.h>

module module_klic__comp__obj();
Const struct predicate predicate_klic__comp__obj_xwrite__object_4 =
   { module_klic__comp__obj, 0, 4 };
Const struct predicate predicate_klic__comp__obj_xwrite__object_5 =
   { module_klic__comp__obj, 1, 5 };
Const struct predicate predicate_klic__comp__obj_xwrite__object_2F5_240_8 =
   { module_klic__comp__obj, 2, 8 };
Const struct predicate predicate_klic__comp__obj_xwrite__object_2F5_240_2F8_240_10 =
   { module_klic__comp__obj, 3, 10 };
Const struct predicate predicate_klic__comp__obj_xwrite__one_6 =
   { module_klic__comp__obj, 4, 6 };
Const struct predicate predicate_klic__comp__obj_xwrite__one__move_4 =
   { module_klic__comp__obj, 5, 4 };
Const struct predicate predicate_klic__comp__obj_xwrite__one__execute_5 =
   { module_klic__comp__obj, 6, 5 };
Const struct predicate predicate_klic__comp__obj_xwrite__one__bblt_6 =
   { module_klic__comp__obj, 7, 6 };
Const struct predicate predicate_klic__comp__obj_xwrite__adjust_4 =
   { module_klic__comp__obj, 8, 4 };
Const struct predicate predicate_klic__comp__obj_xwrite__adjust__qp_3 =
   { module_klic__comp__obj, 9, 3 };
Const struct predicate predicate_klic__comp__obj_xwrite__adjust__allocp_3 =
   { module_klic__comp__obj, 10, 3 };
Const struct predicate predicate_klic__comp__obj_xklicformat__stderr_2 =
   { module_klic__comp__obj, 11, 2 };
Const struct predicate predicate_klic__comp__obj_xflush_2 =
   { module_klic__comp__obj, 12, 2 };
Const struct predicate predicate_klic__comp__obj_xflush_3 =
   { module_klic__comp__obj, 13, 3 };
Const struct predicate predicate_klic__comp__obj_xklicformat_3 =
   { module_klic__comp__obj, 14, 3 };
Const struct predicate predicate_klic__comp__obj_xklicformat_4 =
   { module_klic__comp__obj, 15, 4 };
Const struct predicate predicate_klic__comp__obj_xwait__and__out_4 =
   { module_klic__comp__obj, 16, 4 };
Const struct predicate predicate_klic__comp__obj_xklicformat_6 =
   { module_klic__comp__obj, 17, 6 };
Const struct predicate predicate_klic__comp__obj_xformat__special_5 =
   { module_klic__comp__obj, 18, 5 };
Const struct predicate predicate_klic__comp__obj_xformat__special__i_3 =
   { module_klic__comp__obj, 19, 3 };
Const struct predicate predicate_klic__comp__obj_xformat__special__w_3 =
   { module_klic__comp__obj, 20, 3 };
Const struct predicate predicate_klic__comp__obj_xformat__special__W_3 =
   { module_klic__comp__obj, 21, 3 };
Const struct predicate predicate_klic__comp__obj_xformat__special__D_4 =
   { module_klic__comp__obj, 22, 4 };
Const struct predicate predicate_klic__comp__obj_xformat__special__D__1_4 =
   { module_klic__comp__obj, 23, 4 };
Const struct predicate predicate_klic__comp__obj_xformat__special__L_3 =
   { module_klic__comp__obj, 24, 3 };
Const struct predicate predicate_klic__comp__obj_xwrite__params_3 =
   { module_klic__comp__obj, 25, 3 };
Const struct predicate predicate_klic__comp__obj_xwrite__param_3 =
   { module_klic__comp__obj, 26, 3 };
Const struct predicate predicate_klic__comp__obj_xwrite__params__blt_3 =
   { module_klic__comp__obj, 27, 3 };
Const struct predicate predicate_klic__comp__obj_xwrite__param__blt_3 =
   { module_klic__comp__obj, 28, 3 };
Const struct predicate predicate_klic__comp__obj_xwrite__int_3 =
   { module_klic__comp__obj, 29, 3 };
Const struct predicate predicate_klic__comp__obj_xwrite__elem__access_4 =
   { module_klic__comp__obj, 30, 4 };
Const struct predicate predicate_klic__comp__obj_xwrite__reg_3 =
   { module_klic__comp__obj, 31, 3 };
Const struct predicate predicate_klic__comp__obj_xwrite__reg__int_3 =
   { module_klic__comp__obj, 32, 3 };
Const struct predicate predicate_klic__comp__obj_xwrite__short_3 =
   { module_klic__comp__obj, 33, 3 };
Const struct predicate predicate_klic__comp__obj_xwrite__qp_3 =
   { module_klic__comp__obj, 34, 3 };
Const struct predicate predicate_klic__comp__obj_xwrite__inline_6 =
   { module_klic__comp__obj, 35, 6 };
Const struct predicate predicate_klic__comp__obj_xwrite__inline_8 =
   { module_klic__comp__obj, 36, 8 };
Const struct predicate predicate_klic__comp__obj_xinline__special_6 =
   { module_klic__comp__obj, 37, 6 };
Const struct predicate predicate_klic__comp__obj_xwrite__atom__name__string_3 =
   { module_klic__comp__obj, 38, 3 };
Const struct predicate predicate_klic__comp__obj_xwrite__atom__name__string_5 =
   { module_klic__comp__obj, 39, 5 };
Const struct predicate predicate_klic__comp__obj_xwrite__atom__name__char_3 =
   { module_klic__comp__obj, 40, 3 };
Const struct predicate predicate_klic__comp__obj_xhexchar_2 =
   { module_klic__comp__obj, 41, 2 };
extern Const struct predicate predicate_unparser_xunparse_4;
extern Const struct predicate predicate_unparser_xunwparse_4;
extern Const struct predicate predicate_integer__arithmetics_xadd_3;
extern Const struct predicate predicate_klic__comp__util_xappend_3;
extern Const struct predicate predicate_klic__comp__util_xlength_2;
extern Const struct predicate predicate_klic__comp__util_xnth_3;
extern Const struct predicate predicate_klic__comp__message_xwarning_2;
extern Const struct predicate predicate_klic__comp__message_xreport__error_2;
extern Const struct predicate predicate_klicio_xklicio_1;
extern Const struct predicate predicate_klic__comp__write_xwrite__atom_3;
extern Const struct predicate predicate_klic__comp__write_xwrite__funct__name_4;
extern Const struct predicate predicate_klic__comp__write_xwrite__pred__name_5;
extern Const struct predicate predicate_atom__table_xget__atom__string_2;

module module_klic__comp__obj(glbl, qp, allocp, toppred)
  struct global_variables *glbl;
  struct goalrec *qp;
  register q *allocp;
  Const struct predicate *toppred;
{
  static Const unsigned char strconst_body_0[4] =
    "  ";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_0 =
      declare_string_constant_8(strconst_body_0,2);
  static Const unsigned char strconst_body_1[16] =
    {' ',' ','a','l','l','o','c','p',' ','+','=',' ','~','d',';','\n',};
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_1 =
      declare_string_constant_8(strconst_body_1,16);
  static Const unsigned char strconst_body_2[28] =
    "  allocp[~d] = (q)(&~p);\n";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_2 =
      declare_string_constant_8(strconst_body_2,25);
  static Const unsigned char strconst_body_3[24] =
    "  allocp[~d] = (q)~Q;\n";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_3 =
      declare_string_constant_8(strconst_body_3,22);
  static Const unsigned char strconst_body_4[28] =
    {' ',' ','a','l','l','o','c','p','[','~','d',']',' ','=',' ','m','a','k','e','s','y','m','(','~','f',')',';','\n',};
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_4 =
      declare_string_constant_8(strconst_body_4,28);
  static Const unsigned char strconst_body_5[20] =
    "  allocp[~d] = ~e;\n";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_5 =
      declare_string_constant_8(strconst_body_5,19);
  static Const unsigned char strconst_body_6[20] =
    "  allocp[~d] = ~k;\n";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_6 =
      declare_string_constant_8(strconst_body_6,19);
  static Const unsigned char strconst_body_7[44] =
    "  allocp[~d] = ~r = makeref(&allocp[~d]);\n";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_7 =
      declare_string_constant_8(strconst_body_7,42);
  static Const unsigned char strconst_body_8[20] =
    "  allocp[~d] = ~r;\n";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_8 =
      declare_string_constant_8(strconst_body_8,19);
  static Const unsigned char strconst_body_9[20] =
    "  alternative(~l);\n";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_9 =
      declare_string_constant_8(strconst_body_9,19);
  static Const unsigned char strconst_body_10[28] =
    {' ',' ','c','a','l','l','_','g','e','n','e','r','i','c','(','~','k',',',' ','~','f',',',' ','~','d',')',';','\n',};
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_10 =
      declare_string_constant_8(strconst_body_10,28);
  static Const unsigned char strconst_body_11[56] =
    {' ',' ','d','e','r','e','f','_','a','n','d','_','j','u','m','p','(','~','r',',','~','l',')',';','\n',' ',' ','*','r','e','a','s','o','n','p','+','+',' ','=',' ',' ','~','r',';','\n',' ',' ','g','o','t','o',' ','~','l',';','\n',};
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_11 =
      declare_string_constant_8(strconst_body_11,56);
  static Const unsigned char strconst_body_12[40] =
    "  enqueue_at_lower_priority(~k,~r,~r);\n";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_12 =
      declare_string_constant_8(strconst_body_12,39);
  static Const unsigned char strconst_body_13[48] =
    {' ',' ','e','n','q','u','e','u','e','_','a','t','_','l','o','w','e','r','_','p','r','i','o','r','i','t','y','_','n','o','_','c','h','e','c','k','(','~','k',',','~','r',',','~','r',')',';','\n',};
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_13 =
      declare_string_constant_8(strconst_body_13,48);
  static Const unsigned char strconst_body_14[36] =
    "  enqueue_at_priority(~k,~r,~r);\n";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_14 =
      declare_string_constant_8(strconst_body_14,33);
  static Const unsigned char strconst_body_15[44] =
    "  enqueue_at_priority_no_check(~k,~r,~r);\n";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_15 =
      declare_string_constant_8(strconst_body_15,42);
  static Const unsigned char strconst_body_16[48] =
    {' ',' ','e','x','e','c','u','t','e','(','~','q','_','~','d','_','0',')',';','\n',' ',' ','g','o','t','o',' ','~','q','_','~','d','_','e','x','t','_','i','n','t','e','r','r','u','p','t',';','\n',};
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_16 =
      declare_string_constant_8(strconst_body_16,48);
  static Const unsigned char strconst_body_17[60] =
    "  execute(~q_~d_clear_reason);\n  goto ~q_~d_ext_interrupt;\n";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_17 =
      declare_string_constant_8(strconst_body_17,59);
  static Const unsigned char strconst_body_18[24] =
    {' ',' ','g','e','n','e','r','i','c','_','a','r','g','[','~','d',']',' ','=',' ','~','k',';','\n',};
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_18 =
      declare_string_constant_8(strconst_body_18,24);
  static Const unsigned char strconst_body_19[12] =
    "  goto ~l;\n";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_19 =
      declare_string_constant_8(strconst_body_19,11);
  static Const unsigned char strconst_body_20[36] =
    "  guard_generic(~k, ~f, ~d, ~l);\n";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_20 =
      declare_string_constant_8(strconst_body_20,33);
  static Const unsigned char strconst_body_21[32] =
    {' ',' ','i','f',' ','(','!','i','s','c','l','a','s','s','(','~','r',',','~','q',')',')',' ','g','o','t','o',' ','~','l',';','\n',};
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_21 =
      declare_string_constant_8(strconst_body_21,32);
  static Const unsigned char strconst_body_22[28] =
    {' ',' ','i','f',' ','(','!','i','s','c','o','n','s','(','~','r',')',')',' ','g','o','t','o',' ','~','l',';','\n',};
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_22 =
      declare_string_constant_8(strconst_body_22,28);
  static Const unsigned char strconst_body_23[32] =
    "  if (!isfunctor(~r)) goto ~l;\n";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_23 =
      declare_string_constant_8(strconst_body_23,31);
  static Const unsigned char strconst_body_24[28] =
    {' ',' ','i','f',' ','(','!','i','s','g','o','b','j','(','~','r',')',')',' ','g','o','t','o',' ','~','l',';','\n',};
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_24 =
      declare_string_constant_8(strconst_body_24,28);
  static Const unsigned char strconst_body_25[28] =
    "  if (!isint(~r)) goto ~l;\n";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_25 =
      declare_string_constant_8(strconst_body_25,27);
  static Const unsigned char strconst_body_26[28] =
    "  if (!isref(~r)) goto ~l;\n";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_26 =
      declare_string_constant_8(strconst_body_26,27);
  static Const unsigned char strconst_body_27[48] =
    "  if (functor_of(~r) != makesym(~f)) goto ~l;\n";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_27 =
      declare_string_constant_8(strconst_body_27,46);
  static Const unsigned char strconst_body_28[28] =
    "  if (iscons(~r)) goto ~l;\n";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_28 =
      declare_string_constant_8(strconst_body_28,27);
  static Const unsigned char strconst_body_29[32] =
    "  if (isfunctor(~r)) goto ~l;\n";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_29 =
      declare_string_constant_8(strconst_body_29,30);
  static Const unsigned char strconst_body_30[28] =
    "  if (isint(~r)) goto ~l;\n";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_30 =
      declare_string_constant_8(strconst_body_30,26);
  static Const unsigned char strconst_body_31[28] =
    "  if (isref(~r)) goto ~l;\n";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_31 =
      declare_string_constant_8(strconst_body_31,26);
  static Const unsigned char strconst_body_32[28] =
    "  if (~r != ~k) goto ~l;\n";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_32 =
      declare_string_constant_8(strconst_body_32,25);
  static Const unsigned char strconst_body_33[32] =
    {' ',' ','i','f','_','e','q','u','a','l','(','~','r',',',' ','~','r',',',' ','~','l',',',' ','~','l',',',' ','~','l',')',';','\n',};
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_33 =
      declare_string_constant_8(strconst_body_33,32);
  static Const unsigned char strconst_body_34[28] =
    {' ',' ','i','f','_','n','o','t','_','e','q','u','a','l','(','~','r',',',' ','~','r',',',' ','~','l',')',';','\n',};
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_34 =
      declare_string_constant_8(strconst_body_34,28);
  static Const unsigned char strconst_body_35[40] =
    "  new_generic(~q_g_new, ~d, ~r, ~d);\n";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_35 =
      declare_string_constant_8(strconst_body_35,37);
  static Const unsigned char strconst_body_36[20] =
    "  otherwise(~l);\n";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_36 =
      declare_string_constant_8(strconst_body_36,17);
  static Const unsigned char strconst_body_37[16] =
    "  proceed();\n";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_37 =
      declare_string_constant_8(strconst_body_37,13);
  static Const unsigned char strconst_body_38[12] =
    "  qp = ~Q;\n";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_38 =
      declare_string_constant_8(strconst_body_38,11);
  static Const unsigned char strconst_body_39[32] =
    "  switch ((unsigned long)~r) {\n";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_39 =
      declare_string_constant_8(strconst_body_39,31);
  static Const unsigned char strconst_body_40[24] =
    {' ',' ','s','w','i','t','c','h',' ','(','p','t','a','g','o','f','(','~','r',')',')',' ','{','\n',};
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_40 =
      declare_string_constant_8(strconst_body_40,24);
  static Const unsigned char strconst_body_41[36] =
    {' ',' ','s','w','i','t','c','h',' ','(','s','y','m','v','a','l','(','f','u','n','c','t','o','r','_','o','f','(','~','r',')',')',')',' ','{','\n',};
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_41 =
      declare_string_constant_8(strconst_body_41,36);
  static Const unsigned char strconst_body_42[24] =
    {' ',' ','s','w','i','t','c','h',' ','(','s','y','m','v','a','l','(','~','r',')',')',' ','{','\n',};
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_42 =
      declare_string_constant_8(strconst_body_42,24);
  static Const unsigned char strconst_body_43[24] =
    {' ',' ','t','h','r','o','w','_','g','o','a','l','(','~','r',',','~','r',',','~','r',')',';','\n',};
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_43 =
      declare_string_constant_8(strconst_body_43,24);
  static Const unsigned char strconst_body_44[20] =
    "  unify(~k, ~k);\n";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_44 =
      declare_string_constant_8(strconst_body_44,17);
  static Const unsigned char strconst_body_45[28] =
    "  unify2(~k, ~k, ~k, ~k);\n";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_45 =
      declare_string_constant_8(strconst_body_45,26);
  static Const unsigned char strconst_body_46[36] =
    "  unify3(~k, ~k, ~k, ~k, ~k, ~k);\n";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_46 =
      declare_string_constant_8(strconst_body_46,34);
  static Const unsigned char strconst_body_47[44] =
    "  unify4(~k, ~k, ~k, ~k, ~k, ~k, ~k, ~k);\n";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_47 =
      declare_string_constant_8(strconst_body_47,42);
  static Const unsigned char strconst_body_48[24] =
    "  unify_value(~k, ~k);\n";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_48 =
      declare_string_constant_8(strconst_body_48,23);
  static Const unsigned char strconst_body_49[8] =
    "  };\n";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_49 =
      declare_string_constant_8(strconst_body_49,5);
  static Const unsigned char strconst_body_50[16] =
    "  ~r = (q)~Q;\n";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_50 =
      declare_string_constant_8(strconst_body_50,14);
  static Const unsigned char strconst_body_51[48] =
    "  ~r = create_vector(&allocp[~d], ~d, allocp);\n";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_51 =
      declare_string_constant_8(strconst_body_51,47);
  static Const unsigned char strconst_body_52[24] =
    {' ',' ','~','r',' ','=',' ','g','e','n','e','r','i','c','_','a','r','g','[','~','d',']',';','\n',};
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_52 =
      declare_string_constant_8(strconst_body_52,24);
  static Const unsigned char strconst_body_53[32] =
    "  ~r = makecons(&allocp[~d]);\n";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_53 =
      declare_string_constant_8(strconst_body_53,30);
  static Const unsigned char strconst_body_54[36] =
    "  ~r = makefunctor(&allocp[~d]);\n";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_54 =
      declare_string_constant_8(strconst_body_54,33);
  static Const unsigned char strconst_body_55[12] =
    "  ~r = ~e;\n";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_55 =
      declare_string_constant_8(strconst_body_55,11);
  static Const unsigned char strconst_body_56[12] =
    "  ~r = ~k;\n";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_56 =
      declare_string_constant_8(strconst_body_56,11);
  static Const unsigned char strconst_body_57[12] =
    "  ~r = ~r;\n";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_57 =
      declare_string_constant_8(strconst_body_57,11);
  static Const unsigned char strconst_body_58[12] =
    "  ~w(~,);\n";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_58 =
      declare_string_constant_8(strconst_body_58,10);
  static Const unsigned char strconst_body_59[16] =
    {' ',' ','~','w','(','~',',',',','~','k',',','~','l',')',';','\n',};
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_59 =
      declare_string_constant_8(strconst_body_59,16);
  static Const unsigned char strconst_body_60[16] =
    "  ~w(~,,~l);\n";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_60 =
      declare_string_constant_8(strconst_body_60,13);
  static Const unsigned char strconst_body_61[20] =
    "  ~w_no_check(~,);\n";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_61 =
      declare_string_constant_8(strconst_body_61,19);
  static Const unsigned char strconst_body_62[24] =
    "  ~w_no_check_x(~,);\n";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_62 =
      declare_string_constant_8(strconst_body_62,21);
  static Const unsigned char strconst_body_63[24] =
    "  ~w_no_check_y(~,);\n";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_63 =
      declare_string_constant_8(strconst_body_63,21);
  static Const unsigned char strconst_body_64[28] =
    " case (unsigned long) ~a:\n";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_64 =
      declare_string_constant_8(strconst_body_64,26);
  static Const unsigned char strconst_body_65[20] =
    " case symval(~a):\n";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_65 =
      declare_string_constant_8(strconst_body_65,18);
  static Const unsigned char strconst_body_66[12] =
    " case ~f:\n";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_66 =
      declare_string_constant_8(strconst_body_66,10);
  static Const unsigned char strconst_body_67[12] =
    " case ~s:\n";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_67 =
      declare_string_constant_8(strconst_body_67,10);
  static Const unsigned char strconst_body_68[12] =
    " default:\n";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_68 =
      declare_string_constant_8(strconst_body_68,10);
  static Const unsigned char strconst_body_69[8] =
    " ~l:\n";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_69 =
      declare_string_constant_8(strconst_body_69,5);
  static Const unsigned char strconst_body_70[28] =
    {'(','s','t','r','u','c','t',' ','g','o','a','l','r','e','c','*',')','&','a','l','l','o','c','p','[','~','d',']',};
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_70 =
      declare_string_constant_8(strconst_body_70,28);
  static Const unsigned char strconst_body_71[4] =
    ")";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_71 =
      declare_string_constant_8(strconst_body_71,1);
  static Const unsigned char strconst_body_72[4] =
    ", ";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_72 =
      declare_string_constant_8(strconst_body_72,2);
  static Const unsigned char strconst_body_73[8] =
    ", ~d)";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_73 =
      declare_string_constant_8(strconst_body_73,5);
  static Const unsigned char strconst_body_74[32] =
    "Failed to write out object: ~w";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_74 =
      declare_string_constant_8(strconst_body_74,30);
  static Const unsigned char strconst_body_75[12] =
    "HALFWORD(";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_75 =
      declare_string_constant_8(strconst_body_75,9);
  static Const unsigned char strconst_body_76[12] =
    {'H','A','L','F','W','O','R','D','(','~','d',')',};
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_76 =
      declare_string_constant_8(strconst_body_76,12);
  static Const unsigned char strconst_body_77[16] =
    "KLICLABELDIFF(";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_77 =
      declare_string_constant_8(strconst_body_77,14);
  static Const unsigned char strconst_body_78[16] =
    "LABELDIFF(~d, ";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_78 =
      declare_string_constant_8(strconst_body_78,14);
  static Const unsigned char strconst_body_79[12] =
    "arg(~r, ~d)";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_79 =
      declare_string_constant_8(strconst_body_79,11);
  static Const unsigned char strconst_body_80[12] =
    "car_of(~r)";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_80 =
      declare_string_constant_8(strconst_body_80,10);
  static Const unsigned char strconst_body_81[12] =
    "cdr_of(~r)";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_81 =
      declare_string_constant_8(strconst_body_81,10);
  static Const unsigned char strconst_body_82[12] =
    {'k','l','i','c','l','a','b','e','l','_','~','f',};
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_82 =
      declare_string_constant_8(strconst_body_82,12);
  static Const unsigned char strconst_body_83[16] =
    {'k','l','i','c','l','a','b','e','l','_','~','f','_','_','~','d',};
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_83 =
      declare_string_constant_8(strconst_body_83,16);
  static Const unsigned char strconst_body_84[16] =
    {'k','l','i','c','l','a','b','e','l','_','~','f','_','_','~','q',};
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_84 =
      declare_string_constant_8(strconst_body_84,16);
  static Const unsigned char strconst_body_85[24] =
    "makecons(cons_const_~d)";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_85 =
      declare_string_constant_8(strconst_body_85,23);
  static Const unsigned char strconst_body_86[28] =
    "makefloat(float_const_~d)";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_86 =
      declare_string_constant_8(strconst_body_86,25);
  static Const unsigned char strconst_body_87[32] =
    "makefunctor(&string_const_~d)";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_87 =
      declare_string_constant_8(strconst_body_87,29);
  static Const unsigned char strconst_body_88[32] =
    "makefunctor(&vector_const_~d)";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_88 =
      declare_string_constant_8(strconst_body_88,29);
  static Const unsigned char strconst_body_89[28] =
    "makefunctor(funct_const_~d)";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_89 =
      declare_string_constant_8(strconst_body_89,27);
  static Const unsigned char strconst_body_90[12] =
    {'m','a','k','e','i','n','t','(','~','d','L',')',};
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_90 =
      declare_string_constant_8(strconst_body_90,12);
  static Const unsigned char strconst_body_91[24] =
    "makepred(pred_const_~d)";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_91 =
      declare_string_constant_8(strconst_body_91,23);
  static Const unsigned char strconst_body_92[4] =
    "qp";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_92 =
      declare_string_constant_8(strconst_body_92,2);
  static Const unsigned char strconst_body_93[24] =
    "unknown format char ~c";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_93 =
      declare_string_constant_8(strconst_body_93,22);
  static Const unsigned char strconst_body_94[4] =
    "~l";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_94 =
      declare_string_constant_8(strconst_body_94,2);
  static Const unsigned char strconst_body_95[8] =
    {'~','q','_','~','d','_','~','w',};
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_95 =
      declare_string_constant_8(strconst_body_95,8);
  static Const unsigned char strconst_body_96[4] =
    "~r";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_96 =
      declare_string_constant_8(strconst_body_96,2);
  static Const unsigned char strconst_body_97[8] =
    "~w_~d";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_97 =
      declare_string_constant_8(strconst_body_97,5);
  static Const unsigned char strconst_body_98[8] =
    {'~','w','_','~','w','_','~','d',};
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_98 =
      declare_string_constant_8(strconst_body_98,8);
  static Const q cons_const_99[] = {
    NILATOM,
    makesym(atom_interrupt),
  };
  static Const q cons_const_100[] = {
    NILATOM,
    makesym(atom_clear__reason),
  };
  static Const q funct_const_101[] = {
    makesym(functor_ops_3),
    NILATOM,
    NILATOM,
    NILATOM,
  };
  q a0, a1, a2, a3, a4, a5, a6, a7, a8, a9;

  q *reasonp;
 module_top:
  a0 = qp->args[0];
  a1 = qp->args[1];
  switch_on_pred() {
    case_pred(0, write__object_4_top);
    case_pred(1, write__object_5_top);
    case_pred(2, write__object_2F5_240_8_top);
    case_pred(3, write__object_2F5_240_2F8_240_10_top);
    case_pred(4, write__one_6_top);
    case_pred(5, write__one__move_4_top);
    case_pred(6, write__one__execute_5_top);
    case_pred(7, write__one__bblt_6_top);
    case_pred(8, write__adjust_4_top);
    case_pred(9, write__adjust__qp_3_top);
    case_pred(10, write__adjust__allocp_3_top);
    case_pred(11, klicformat__stderr_2_top);
    case_pred(12, flush_2_top);
    case_pred(13, flush_3_top);
    case_pred(14, klicformat_3_top);
    case_pred(15, klicformat_4_top);
    case_pred(16, wait__and__out_4_top);
    case_pred(17, klicformat_6_top);
    case_pred(18, format__special_5_top);
    case_pred(19, format__special__i_3_top);
    case_pred(20, format__special__w_3_top);
    case_pred(21, format__special__W_3_top);
    case_pred(22, format__special__D_4_top);
    case_pred(23, format__special__D__1_4_top);
    case_pred(24, format__special__L_3_top);
    case_pred(25, write__params_3_top);
    case_pred(26, write__param_3_top);
    case_pred(27, write__params__blt_3_top);
    case_pred(28, write__param__blt_3_top);
    case_pred(29, write__int_3_top);
    case_pred(30, write__elem__access_4_top);
    case_pred(31, write__reg_3_top);
    case_pred(32, write__reg__int_3_top);
    case_pred(33, write__short_3_top);
    case_pred(34, write__qp_3_top);
    case_pred(35, write__inline_6_top);
    case_pred(36, write__inline_8_top);
    case_pred(37, inline__special_6_top);
    case_pred(38, write__atom__name__string_3_top);
    case_pred(39, write__atom__name__string_5_top);
    case_pred(40, write__atom__name__char_3_top);
    last_case_pred(41, hexchar_2_top);
  }

 write__object_4_top: {
  q x0;
  a2 = qp->args[2];
  a3 = qp->args[3];
  qp = qp->next;
 write__object_4_clear_reason:
  reasonp = reasons;
 write__object_4_0:
  x0 = a2;
  a4 = a3;
  a3 = x0;
  a2 = makeint(0L);
  execute(write__object_5_0);
  goto write__object_5_ext_interrupt;
 write__object_4_ext_interrupt:
  reasonp = 0l;
 write__object_4_interrupt:
  goto interrupt_4;
 }

 write__object_5_top: {
  q x0, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10;
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  qp = qp->next;
 write__object_5_clear_reason:
  reasonp = reasons;
 write__object_5_0:
 write__object_5_1:
  switch (ptagof(a0)) {
 case CONS:
  x0 = car_of(a0);
 write__object_5_2:
  switch (ptagof(x0)) {
 case FUNCTOR:
  if (functor_of(x0) != makesym(functor_unify_3)) goto write__object_5_5;
  x1 = cdr_of(a0);
 write__object_5_3:
  switch (ptagof(x1)) {
 case CONS:
  x2 = car_of(x1);
 write__object_5_4:
  switch (ptagof(x2)) {
 case FUNCTOR:
  if (functor_of(x2) != makesym(functor_unify_3)) goto write__object_5_5;
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__obj_xwrite__object_5);
  allocp[2] = x3 = makeref(&allocp[2]);
  allocp[3] = a1;
  allocp[4] = makeint(0L);
  allocp[5] = x4 = makeref(&allocp[5]);
  allocp[6] = a4;
  allocp[7] = (q)(struct goalrec*)&allocp[0];
  allocp[8] = (q)(&predicate_klic__comp__obj_xwrite__object_2F5_240_8);
  x5 = cdr_of(x1);
  allocp[9] = x5;
  x6 = arg(x0, 0);
  allocp[10] = x6;
  x7 = arg(x0, 1);
  allocp[11] = x7;
  x8 = arg(x2, 0);
  allocp[12] = x8;
  x9 = arg(x2, 1);
  allocp[13] = x9;
  allocp[14] = x10 = makeref(&allocp[14]);
  allocp[15] = x3;
  allocp[16] = x4;
  a0 = arg(x0, 2);
  a1 = a2;
  a2 = a3;
  a3 = x10;
  qp = (struct goalrec*)&allocp[7];
  allocp += 17;
  execute(write__adjust_4_0);
  goto write__adjust_4_ext_interrupt;
 case VARREF:
  deref_and_jump(x2,write__object_5_4);
  *reasonp++ =  x2;
  goto write__object_5_5;
  };
  goto write__object_5_5;
 case VARREF:
  deref_and_jump(x1,write__object_5_3);
  *reasonp++ =  x1;
  goto write__object_5_5;
  };
  goto write__object_5_5;
 case VARREF:
  deref_and_jump(x0,write__object_5_2);
  *reasonp++ =  x0;
  goto write__object_5_5;
  };
  goto write__object_5_5;
 case ATOMIC:
  if (a0 != NILATOM) goto write__object_5_5;
  unify(a4, a3);
  proceed();
 case VARREF:
  deref_and_jump(a0,write__object_5_1);
  *reasonp++ =  a0;
  goto write__object_5_5;
  };
  goto write__object_5_5;
 write__object_5_5:
  otherwise(write__object_5_interrupt);
 write__object_5_6:
  switch (ptagof(a0)) {
 case CONS:
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__obj_xwrite__object_5);
  x0 = cdr_of(a0);
  allocp[2] = x0;
  allocp[3] = a1;
  allocp[4] = x1 = makeref(&allocp[4]);
  allocp[5] = x2 = makeref(&allocp[5]);
  allocp[6] = a4;
  a0 = car_of(a0);
  a4 = a3;
  a5 = x2;
  a3 = x1;
  qp = (struct goalrec*)&allocp[0];
  allocp += 7;
  execute(write__one_6_0);
  goto write__one_6_ext_interrupt;
 case VARREF:
  deref_and_jump(a0,write__object_5_6);
  *reasonp++ =  a0;
  goto write__object_5_interrupt;
  };
  goto write__object_5_interrupt;
 write__object_5_ext_interrupt:
  reasonp = 0l;
 write__object_5_interrupt:
  toppred = &predicate_klic__comp__obj_xwrite__object_5;
  goto interrupt_5;
 }

 write__object_2F5_240_8_top: {
  q x0, x1, x2, x3;
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  a6 = qp->args[6];
  a7 = qp->args[7];
  qp = qp->next;
 write__object_2F5_240_8_clear_reason:
  reasonp = reasons;
 write__object_2F5_240_8_0:
 write__object_2F5_240_8_1:
  switch (ptagof(a0)) {
 case CONS:
  x0 = car_of(a0);
 write__object_2F5_240_8_2:
  switch (ptagof(x0)) {
 case FUNCTOR:
  if (functor_of(x0) != makesym(functor_unify_3)) goto write__object_2F5_240_8_3;
  a0 = cdr_of(a0);
  x1 = arg(x0, 0);
  x2 = arg(x0, 1);
  x3 = a5;
  a8 = a6;
  a9 = a7;
  a7 = x3;
  a6 = x2;
  a5 = x1;
  execute(write__object_2F5_240_2F8_240_10_0);
  goto write__object_2F5_240_2F8_240_10_ext_interrupt;
 case VARREF:
  deref_and_jump(x0,write__object_2F5_240_8_2);
  *reasonp++ =  x0;
  goto write__object_2F5_240_8_3;
  };
  goto write__object_2F5_240_8_3;
 case VARREF:
  deref_and_jump(a0,write__object_2F5_240_8_1);
  *reasonp++ =  a0;
  goto write__object_2F5_240_8_3;
  };
  goto write__object_2F5_240_8_3;
 write__object_2F5_240_8_3:
  otherwise(write__object_2F5_240_8_interrupt);
  unify(a6, a0);
  allocp[0] = NILATOM;
  allocp[1] = a4;
  x0 = makecons(&allocp[0]);
  allocp[2] = x0;
  allocp[3] = a3;
  x1 = makecons(&allocp[2]);
  allocp[4] = x1;
  allocp[5] = a2;
  x2 = makecons(&allocp[4]);
  allocp[6] = x2;
  allocp[7] = a1;
  x3 = makecons(&allocp[6]);
  a0 = makefunctor(&string_const_45);
  a1 = x3;
  a2 = a5;
  a3 = a7;
  allocp += 8;
  execute(klicformat_4_0);
  goto klicformat_4_ext_interrupt;
 write__object_2F5_240_8_ext_interrupt:
  reasonp = 0l;
 write__object_2F5_240_8_interrupt:
  goto interrupt_8;
 }

 write__object_2F5_240_2F8_240_10_top: {
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
 write__object_2F5_240_2F8_240_10_clear_reason:
  reasonp = reasons;
 write__object_2F5_240_2F8_240_10_0:
 write__object_2F5_240_2F8_240_10_1:
  switch (ptagof(a0)) {
 case CONS:
  x0 = car_of(a0);
 write__object_2F5_240_2F8_240_10_2:
  switch (ptagof(x0)) {
 case FUNCTOR:
  if (functor_of(x0) != makesym(functor_unify_3)) goto write__object_2F5_240_2F8_240_10_3;
  x1 = cdr_of(a0);
  unify(a8, x1);
  allocp[0] = NILATOM;
  x3 = arg(x0, 1);
  allocp[1] = x3;
  x2 = makecons(&allocp[0]);
  allocp[2] = x2;
  x5 = arg(x0, 0);
  allocp[3] = x5;
  x4 = makecons(&allocp[2]);
  allocp[4] = x4;
  allocp[5] = a6;
  x6 = makecons(&allocp[4]);
  allocp[6] = x6;
  allocp[7] = a5;
  x7 = makecons(&allocp[6]);
  allocp[8] = x7;
  allocp[9] = a4;
  x8 = makecons(&allocp[8]);
  allocp[10] = x8;
  allocp[11] = a3;
  x9 = makecons(&allocp[10]);
  allocp[12] = x9;
  allocp[13] = a2;
  x10 = makecons(&allocp[12]);
  allocp[14] = x10;
  allocp[15] = a1;
  x11 = makecons(&allocp[14]);
  a0 = makefunctor(&string_const_47);
  a1 = x11;
  a2 = a7;
  a3 = a9;
  allocp += 16;
  execute(klicformat_4_0);
  goto klicformat_4_ext_interrupt;
 case VARREF:
  deref_and_jump(x0,write__object_2F5_240_2F8_240_10_2);
  *reasonp++ =  x0;
  goto write__object_2F5_240_2F8_240_10_3;
  };
  goto write__object_2F5_240_2F8_240_10_3;
 case VARREF:
  deref_and_jump(a0,write__object_2F5_240_2F8_240_10_1);
  *reasonp++ =  a0;
  goto write__object_2F5_240_2F8_240_10_3;
  };
  goto write__object_2F5_240_2F8_240_10_3;
 write__object_2F5_240_2F8_240_10_3:
  otherwise(write__object_2F5_240_2F8_240_10_interrupt);
  unify(a8, a0);
  allocp[0] = NILATOM;
  allocp[1] = a6;
  x0 = makecons(&allocp[0]);
  allocp[2] = x0;
  allocp[3] = a5;
  x1 = makecons(&allocp[2]);
  allocp[4] = x1;
  allocp[5] = a4;
  x2 = makecons(&allocp[4]);
  allocp[6] = x2;
  allocp[7] = a3;
  x3 = makecons(&allocp[6]);
  allocp[8] = x3;
  allocp[9] = a2;
  x4 = makecons(&allocp[8]);
  allocp[10] = x4;
  allocp[11] = a1;
  x5 = makecons(&allocp[10]);
  a0 = makefunctor(&string_const_46);
  a1 = x5;
  a2 = a7;
  a3 = a9;
  allocp += 12;
  execute(klicformat_4_0);
  goto klicformat_4_ext_interrupt;
 write__object_2F5_240_2F8_240_10_ext_interrupt:
  reasonp = 0l;
 write__object_2F5_240_2F8_240_10_interrupt:
  toppred = &predicate_klic__comp__obj_xwrite__object_2F5_240_2F8_240_10;
  goto interrupt_10;
 }

 write__one_6_top: {
  q x0, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12;
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  qp = qp->next;
 write__one_6_clear_reason:
  reasonp = reasons;
 write__one_6_0:
 write__one_6_1:
  switch (ptagof(a0)) {
 case ATOMIC:
  if (isint(a0)) goto write__one_6_3;
  switch (symval(a0)) {
 case symval(makesym(atom_case__default)):
  unify(a3, a2);
  a0 = makefunctor(&string_const_68);
  a1 = a4;
  a2 = a5;
  execute(klicformat_3_0);
  goto klicformat_3_ext_interrupt;
 case symval(makesym(atom_end__sw)):
  unify(a3, a2);
  a0 = makefunctor(&string_const_49);
  a1 = a4;
  a2 = a5;
  execute(klicformat_3_0);
  goto klicformat_3_ext_interrupt;
 case symval(makesym(atom_otherwise)):
  unify(a3, a2);
  allocp[0] = makecons(cons_const_99);
  allocp[1] = a1;
  x0 = makecons(&allocp[0]);
  a0 = makefunctor(&string_const_36);
  a1 = x0;
  a2 = a4;
  a3 = a5;
  allocp += 2;
  execute(klicformat_4_0);
  goto klicformat_4_ext_interrupt;
 case symval(makesym(atom_alternatively)):
  unify(a3, a2);
  allocp[0] = makecons(cons_const_100);
  allocp[1] = a1;
  x0 = makecons(&allocp[0]);
  a0 = makefunctor(&string_const_9);
  a1 = x0;
  a2 = a4;
  a3 = a5;
  allocp += 2;
  execute(klicformat_4_0);
  goto klicformat_4_ext_interrupt;
  };
  goto write__one_6_3;
 case FUNCTOR:
  switch (symval(functor_of(a0))) {
 case functor_label_1:
  unify(a3, a2);
  allocp[0] = NILATOM;
  x1 = arg(a0, 0);
  allocp[1] = x1;
  x0 = makecons(&allocp[0]);
  allocp[2] = x0;
  allocp[3] = a1;
  x2 = makecons(&allocp[2]);
  a0 = makefunctor(&string_const_69);
  a1 = x2;
  a2 = a4;
  a3 = a5;
  allocp += 4;
  execute(klicformat_4_0);
  goto klicformat_4_ext_interrupt;
 case functor_move_2:
  unify(a3, a2);
  x0 = arg(a0, 0);
  a1 = arg(a0, 1);
  a2 = a4;
  a3 = a5;
  a0 = x0;
  execute(write__one__move_4_0);
  goto write__one__move_4_ext_interrupt;
 case functor_get__elem_3:
  unify(a3, a2);
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
  a0 = makefunctor(&string_const_55);
  a1 = x4;
  a2 = a4;
  a3 = a5;
  allocp += 6;
  execute(klicformat_4_0);
  goto klicformat_4_ext_interrupt;
 case functor_deref_3:
  unify(a3, a2);
  allocp[0] = NILATOM;
  x1 = arg(a0, 2);
  allocp[1] = x1;
  x0 = makecons(&allocp[0]);
  allocp[2] = x0;
  allocp[3] = a1;
  x2 = makecons(&allocp[2]);
  allocp[4] = x2;
  x4 = arg(a0, 0);
  allocp[5] = x4;
  x3 = makecons(&allocp[4]);
  allocp[6] = x3;
  x6 = arg(a0, 1);
  allocp[7] = x6;
  x5 = makecons(&allocp[6]);
  allocp[8] = x5;
  allocp[9] = a1;
  x7 = makecons(&allocp[8]);
  allocp[10] = x7;
  x9 = arg(a0, 0);
  allocp[11] = x9;
  x8 = makecons(&allocp[10]);
  a0 = makefunctor(&string_const_11);
  a1 = x8;
  a2 = a4;
  a3 = a5;
  allocp += 12;
  execute(klicformat_4_0);
  goto klicformat_4_ext_interrupt;
 case functor_goto_1:
  unify(a3, a2);
  allocp[0] = NILATOM;
  x1 = arg(a0, 0);
  allocp[1] = x1;
  x0 = makecons(&allocp[0]);
  allocp[2] = x0;
  allocp[3] = a1;
  x2 = makecons(&allocp[2]);
  a0 = makefunctor(&string_const_19);
  a1 = x2;
  a2 = a4;
  a3 = a5;
  allocp += 4;
  execute(klicformat_4_0);
  goto klicformat_4_ext_interrupt;
 case functor_sw__tag_1:
  unify(a3, a2);
  allocp[0] = NILATOM;
  x1 = arg(a0, 0);
  allocp[1] = x1;
  x0 = makecons(&allocp[0]);
  a0 = makefunctor(&string_const_40);
  a1 = x0;
  a2 = a4;
  a3 = a5;
  allocp += 2;
  execute(klicformat_4_0);
  goto klicformat_4_ext_interrupt;
 case functor_sw__sym_1:
  unify(a3, a2);
  allocp[0] = NILATOM;
  x1 = arg(a0, 0);
  allocp[1] = x1;
  x0 = makecons(&allocp[0]);
  a0 = makefunctor(&string_const_42);
  a1 = x0;
  a2 = a4;
  a3 = a5;
  allocp += 2;
  execute(klicformat_4_0);
  goto klicformat_4_ext_interrupt;
 case functor_sw__int_1:
  unify(a3, a2);
  allocp[0] = NILATOM;
  x1 = arg(a0, 0);
  allocp[1] = x1;
  x0 = makecons(&allocp[0]);
  a0 = makefunctor(&string_const_39);
  a1 = x0;
  a2 = a4;
  a3 = a5;
  allocp += 2;
  execute(klicformat_4_0);
  goto klicformat_4_ext_interrupt;
 case functor_sw__funct_1:
  unify(a3, a2);
  allocp[0] = NILATOM;
  x1 = arg(a0, 0);
  allocp[1] = x1;
  x0 = makecons(&allocp[0]);
  a0 = makefunctor(&string_const_41);
  a1 = x0;
  a2 = a4;
  a3 = a5;
  allocp += 2;
  execute(klicformat_4_0);
  goto klicformat_4_ext_interrupt;
 case functor_case__label_1:
  unify(a3, a2);
  allocp[0] = NILATOM;
  x1 = arg(a0, 0);
  allocp[1] = x1;
  x0 = makecons(&allocp[0]);
  a0 = makefunctor(&string_const_67);
  a1 = x0;
  a2 = a4;
  a3 = a5;
  allocp += 2;
  execute(klicformat_4_0);
  goto klicformat_4_ext_interrupt;
 case functor_case__sym_1:
  unify(a3, a2);
  allocp[0] = NILATOM;
  x1 = arg(a0, 0);
  allocp[1] = x1;
  x0 = makecons(&allocp[0]);
  a0 = makefunctor(&string_const_65);
  a1 = x0;
  a2 = a4;
  a3 = a5;
  allocp += 2;
  execute(klicformat_4_0);
  goto klicformat_4_ext_interrupt;
 case functor_case__int_1:
  unify(a3, a2);
  allocp[0] = NILATOM;
  x1 = arg(a0, 0);
  allocp[1] = x1;
  x0 = makecons(&allocp[0]);
  a0 = makefunctor(&string_const_64);
  a1 = x0;
  a2 = a4;
  a3 = a5;
  allocp += 2;
  execute(klicformat_4_0);
  goto klicformat_4_ext_interrupt;
 case functor_case__funct_1:
  unify(a3, a2);
  allocp[0] = NILATOM;
  x1 = arg(a0, 0);
  allocp[1] = x1;
  x0 = makecons(&allocp[0]);
  a0 = makefunctor(&string_const_66);
  a1 = x0;
  a2 = a4;
  a3 = a5;
  allocp += 2;
  execute(klicformat_4_0);
  goto klicformat_4_ext_interrupt;
 case functor_eq_5:
  unify(a3, a2);
  allocp[0] = NILATOM;
  x1 = arg(a0, 4);
  allocp[1] = x1;
  x0 = makecons(&allocp[0]);
  allocp[2] = x0;
  allocp[3] = a1;
  x2 = makecons(&allocp[2]);
  allocp[4] = x2;
  x4 = arg(a0, 3);
  allocp[5] = x4;
  x3 = makecons(&allocp[4]);
  allocp[6] = x3;
  allocp[7] = a1;
  x5 = makecons(&allocp[6]);
  allocp[8] = x5;
  x7 = arg(a0, 2);
  allocp[9] = x7;
  x6 = makecons(&allocp[8]);
  allocp[10] = x6;
  allocp[11] = a1;
  x8 = makecons(&allocp[10]);
  allocp[12] = x8;
  x10 = arg(a0, 1);
  allocp[13] = x10;
  x9 = makecons(&allocp[12]);
  allocp[14] = x9;
  x12 = arg(a0, 0);
  allocp[15] = x12;
  x11 = makecons(&allocp[14]);
  a0 = makefunctor(&string_const_33);
  a1 = x11;
  a2 = a4;
  a3 = a5;
  allocp += 16;
  execute(klicformat_4_0);
  goto klicformat_4_ext_interrupt;
 case functor_if__not__eq_3:
  unify(a3, a2);
  allocp[0] = NILATOM;
  x1 = arg(a0, 2);
  allocp[1] = x1;
  x0 = makecons(&allocp[0]);
  allocp[2] = x0;
  allocp[3] = a1;
  x2 = makecons(&allocp[2]);
  allocp[4] = x2;
  x4 = arg(a0, 1);
  allocp[5] = x4;
  x3 = makecons(&allocp[4]);
  allocp[6] = x3;
  x6 = arg(a0, 0);
  allocp[7] = x6;
  x5 = makecons(&allocp[6]);
  a0 = makefunctor(&string_const_34);
  a1 = x5;
  a2 = a4;
  a3 = a5;
  allocp += 8;
  execute(klicformat_4_0);
  goto klicformat_4_ext_interrupt;
 case functor_if__int_2:
  unify(a3, a2);
  allocp[0] = NILATOM;
  x1 = arg(a0, 1);
  allocp[1] = x1;
  x0 = makecons(&allocp[0]);
  allocp[2] = x0;
  allocp[3] = a1;
  x2 = makecons(&allocp[2]);
  allocp[4] = x2;
  x4 = arg(a0, 0);
  allocp[5] = x4;
  x3 = makecons(&allocp[4]);
  a0 = makefunctor(&string_const_30);
  a1 = x3;
  a2 = a4;
  a3 = a5;
  allocp += 6;
  execute(klicformat_4_0);
  goto klicformat_4_ext_interrupt;
 case functor_if__not__int_2:
  unify(a3, a2);
  allocp[0] = NILATOM;
  x1 = arg(a0, 1);
  allocp[1] = x1;
  x0 = makecons(&allocp[0]);
  allocp[2] = x0;
  allocp[3] = a1;
  x2 = makecons(&allocp[2]);
  allocp[4] = x2;
  x4 = arg(a0, 0);
  allocp[5] = x4;
  x3 = makecons(&allocp[4]);
  a0 = makefunctor(&string_const_25);
  a1 = x3;
  a2 = a4;
  a3 = a5;
  allocp += 6;
  execute(klicformat_4_0);
  goto klicformat_4_ext_interrupt;
 case functor_if__list_2:
  unify(a3, a2);
  allocp[0] = NILATOM;
  x1 = arg(a0, 1);
  allocp[1] = x1;
  x0 = makecons(&allocp[0]);
  allocp[2] = x0;
  allocp[3] = a1;
  x2 = makecons(&allocp[2]);
  allocp[4] = x2;
  x4 = arg(a0, 0);
  allocp[5] = x4;
  x3 = makecons(&allocp[4]);
  a0 = makefunctor(&string_const_28);
  a1 = x3;
  a2 = a4;
  a3 = a5;
  allocp += 6;
  execute(klicformat_4_0);
  goto klicformat_4_ext_interrupt;
 case functor_if__not__list_2:
  unify(a3, a2);
  allocp[0] = NILATOM;
  x1 = arg(a0, 1);
  allocp[1] = x1;
  x0 = makecons(&allocp[0]);
  allocp[2] = x0;
  allocp[3] = a1;
  x2 = makecons(&allocp[2]);
  allocp[4] = x2;
  x4 = arg(a0, 0);
  allocp[5] = x4;
  x3 = makecons(&allocp[4]);
  a0 = makefunctor(&string_const_22);
  a1 = x3;
  a2 = a4;
  a3 = a5;
  allocp += 6;
  execute(klicformat_4_0);
  goto klicformat_4_ext_interrupt;
 case functor_if__funct_2:
  unify(a3, a2);
  allocp[0] = NILATOM;
  x1 = arg(a0, 1);
  allocp[1] = x1;
  x0 = makecons(&allocp[0]);
  allocp[2] = x0;
  allocp[3] = a1;
  x2 = makecons(&allocp[2]);
  allocp[4] = x2;
  x4 = arg(a0, 0);
  allocp[5] = x4;
  x3 = makecons(&allocp[4]);
  a0 = makefunctor(&string_const_29);
  a1 = x3;
  a2 = a4;
  a3 = a5;
  allocp += 6;
  execute(klicformat_4_0);
  goto klicformat_4_ext_interrupt;
 case functor_if__not__funct_2:
  unify(a3, a2);
  allocp[0] = NILATOM;
  x1 = arg(a0, 1);
  allocp[1] = x1;
  x0 = makecons(&allocp[0]);
  allocp[2] = x0;
  allocp[3] = a1;
  x2 = makecons(&allocp[2]);
  allocp[4] = x2;
  x4 = arg(a0, 0);
  allocp[5] = x4;
  x3 = makecons(&allocp[4]);
  a0 = makefunctor(&string_const_23);
  a1 = x3;
  a2 = a4;
  a3 = a5;
  allocp += 6;
  execute(klicformat_4_0);
  goto klicformat_4_ext_interrupt;
 case functor_if__not__gobj_2:
  unify(a3, a2);
  allocp[0] = NILATOM;
  x1 = arg(a0, 1);
  allocp[1] = x1;
  x0 = makecons(&allocp[0]);
  allocp[2] = x0;
  allocp[3] = a1;
  x2 = makecons(&allocp[2]);
  allocp[4] = x2;
  x4 = arg(a0, 0);
  allocp[5] = x4;
  x3 = makecons(&allocp[4]);
  a0 = makefunctor(&string_const_24);
  a1 = x3;
  a2 = a4;
  a3 = a5;
  allocp += 6;
  execute(klicformat_4_0);
  goto klicformat_4_ext_interrupt;
 case functor_if__not__class_3:
  unify(a3, a2);
  allocp[0] = NILATOM;
  x1 = arg(a0, 2);
  allocp[1] = x1;
  x0 = makecons(&allocp[0]);
  allocp[2] = x0;
  allocp[3] = a1;
  x2 = makecons(&allocp[2]);
  allocp[4] = x2;
  x4 = arg(a0, 1);
  allocp[5] = x4;
  x3 = makecons(&allocp[4]);
  allocp[6] = x3;
  x6 = arg(a0, 0);
  allocp[7] = x6;
  x5 = makecons(&allocp[6]);
  a0 = makefunctor(&string_const_21);
  a1 = x5;
  a2 = a4;
  a3 = a5;
  allocp += 8;
  execute(klicformat_4_0);
  goto klicformat_4_ext_interrupt;
 case functor_if__ref_2:
  unify(a3, a2);
  allocp[0] = NILATOM;
  x1 = arg(a0, 1);
  allocp[1] = x1;
  x0 = makecons(&allocp[0]);
  allocp[2] = x0;
  allocp[3] = a1;
  x2 = makecons(&allocp[2]);
  allocp[4] = x2;
  x4 = arg(a0, 0);
  allocp[5] = x4;
  x3 = makecons(&allocp[4]);
  a0 = makefunctor(&string_const_31);
  a1 = x3;
  a2 = a4;
  a3 = a5;
  allocp += 6;
  execute(klicformat_4_0);
  goto klicformat_4_ext_interrupt;
 case functor_if__not__ref_2:
  unify(a3, a2);
  allocp[0] = NILATOM;
  x1 = arg(a0, 1);
  allocp[1] = x1;
  x0 = makecons(&allocp[0]);
  allocp[2] = x0;
  allocp[3] = a1;
  x2 = makecons(&allocp[2]);
  allocp[4] = x2;
  x4 = arg(a0, 0);
  allocp[5] = x4;
  x3 = makecons(&allocp[4]);
  a0 = makefunctor(&string_const_26);
  a1 = x3;
  a2 = a4;
  a3 = a5;
  allocp += 6;
  execute(klicformat_4_0);
  goto klicformat_4_ext_interrupt;
 case functor_if__funct__not_3:
  unify(a3, a2);
  allocp[0] = NILATOM;
  x1 = arg(a0, 2);
  allocp[1] = x1;
  x0 = makecons(&allocp[0]);
  allocp[2] = x0;
  allocp[3] = a1;
  x2 = makecons(&allocp[2]);
  allocp[4] = x2;
  x4 = arg(a0, 1);
  allocp[5] = x4;
  x3 = makecons(&allocp[4]);
  allocp[6] = x3;
  x6 = arg(a0, 0);
  allocp[7] = x6;
  x5 = makecons(&allocp[6]);
  a0 = makefunctor(&string_const_27);
  a1 = x5;
  a2 = a4;
  a3 = a5;
  allocp += 8;
  execute(klicformat_4_0);
  goto klicformat_4_ext_interrupt;
 case functor_if__not_3:
  unify(a3, a2);
  allocp[0] = NILATOM;
  x1 = arg(a0, 2);
  allocp[1] = x1;
  x0 = makecons(&allocp[0]);
  allocp[2] = x0;
  allocp[3] = a1;
  x2 = makecons(&allocp[2]);
  allocp[4] = x2;
  x4 = arg(a0, 1);
  allocp[5] = x4;
  x3 = makecons(&allocp[4]);
  allocp[6] = x3;
  x6 = arg(a0, 0);
  allocp[7] = x6;
  x5 = makecons(&allocp[6]);
  a0 = makefunctor(&string_const_32);
  a1 = x5;
  a2 = a4;
  a3 = a5;
  allocp += 8;
  execute(klicformat_4_0);
  goto klicformat_4_ext_interrupt;
 case functor_inline_3:
  unify(a3, a2);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__obj_xwrite__inline_6);
  x0 = arg(a0, 0);
  allocp[2] = x0;
  x1 = arg(a0, 1);
  allocp[3] = x1;
  allocp[4] = a1;
  x2 = arg(a0, 2);
  allocp[5] = x2;
  allocp[6] = x3 = makeref(&allocp[6]);
  allocp[7] = a5;
  a0 = makefunctor(&string_const_0);
  a1 = a4;
  a2 = x3;
  qp = (struct goalrec*)&allocp[0];
  allocp += 8;
  execute(klicformat_3_0);
  goto klicformat_3_ext_interrupt;
 case functor_load__const_2:
  unify(a3, a2);
  allocp[0] = NILATOM;
  x1 = arg(a0, 1);
  allocp[1] = x1;
  x0 = makecons(&allocp[0]);
  allocp[2] = x0;
  x3 = arg(a0, 0);
  allocp[3] = x3;
  x2 = makecons(&allocp[2]);
  a0 = makefunctor(&string_const_56);
  a1 = x2;
  a2 = a4;
  a3 = a5;
  allocp += 4;
  execute(klicformat_4_0);
  goto klicformat_4_ext_interrupt;
 case functor_load__newvar_1:
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_integer__arithmetics_xadd_3);
  allocp[2] = a2;
  allocp[3] = makeint(1L);
  allocp[4] = a3;
  allocp[5] = NILATOM;
  allocp[6] = a2;
  x0 = makecons(&allocp[5]);
  allocp[7] = x0;
  x2 = arg(a0, 0);
  allocp[8] = x2;
  x1 = makecons(&allocp[7]);
  allocp[9] = x1;
  allocp[10] = a2;
  x3 = makecons(&allocp[9]);
  a0 = makefunctor(&string_const_7);
  a1 = x3;
  a2 = a4;
  a3 = a5;
  qp = (struct goalrec*)&allocp[0];
  allocp += 11;
  execute(klicformat_4_0);
  goto klicformat_4_ext_interrupt;
 case functor_make__cons_1:
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_integer__arithmetics_xadd_3);
  allocp[2] = a2;
  allocp[3] = makeint(2L);
  allocp[4] = a3;
  allocp[5] = NILATOM;
  allocp[6] = a2;
  x0 = makecons(&allocp[5]);
  allocp[7] = x0;
  x2 = arg(a0, 0);
  allocp[8] = x2;
  x1 = makecons(&allocp[7]);
  a0 = makefunctor(&string_const_53);
  a1 = x1;
  a2 = a4;
  a3 = a5;
  qp = (struct goalrec*)&allocp[0];
  allocp += 9;
  execute(klicformat_4_0);
  goto klicformat_4_ext_interrupt;
 case functor_make__functor_2:
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_integer__arithmetics_xadd_3);
  allocp[2] = a2;
  x0 = arg(a0, 1);
  allocp[3] = x0;
  allocp[4] = a3;
  allocp[5] = NILATOM;
  allocp[6] = a2;
  x1 = makecons(&allocp[5]);
  allocp[7] = x1;
  x3 = arg(a0, 0);
  allocp[8] = x3;
  x2 = makecons(&allocp[7]);
  a0 = makefunctor(&string_const_54);
  a1 = x2;
  a2 = a4;
  a3 = a5;
  qp = (struct goalrec*)&allocp[0];
  allocp += 9;
  execute(klicformat_4_0);
  goto klicformat_4_ext_interrupt;
 case functor_make__vector_2:
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_integer__arithmetics_xadd_3);
  allocp[2] = a2;
  x0 = arg(a0, 1);
  allocp[3] = x0;
  allocp[4] = a3;
  allocp[5] = NILATOM;
  allocp[6] = x0;
  x1 = makecons(&allocp[5]);
  allocp[7] = x1;
  allocp[8] = a2;
  x2 = makecons(&allocp[7]);
  allocp[9] = x2;
  x4 = arg(a0, 0);
  allocp[10] = x4;
  x3 = makecons(&allocp[9]);
  a0 = makefunctor(&string_const_51);
  a1 = x3;
  a2 = a4;
  a3 = a5;
  qp = (struct goalrec*)&allocp[0];
  allocp += 11;
  execute(klicformat_4_0);
  goto klicformat_4_ext_interrupt;
 case functor_alloc__functor__id_2:
  unify(a3, a2);
  allocp[0] = makesym(functor__2F_2);
  x1 = arg(a0, 0);
  allocp[1] = x1;
  x2 = arg(a0, 1);
  allocp[2] = x2;
  x0 = makefunctor(&allocp[0]);
  allocp[3] = NILATOM;
  allocp[4] = x0;
  x3 = makecons(&allocp[3]);
  allocp[5] = x3;
  allocp[6] = a2;
  x4 = makecons(&allocp[5]);
  a0 = makefunctor(&string_const_4);
  a1 = x4;
  a2 = a4;
  a3 = a5;
  allocp += 7;
  execute(klicformat_4_0);
  goto klicformat_4_ext_interrupt;
 case functor_alloc__value_2:
  unify(a3, a2);
  x0 = arg(a0, 1);
  bblt_add(a2,x0,x1);
  allocp[0] = NILATOM;
  x3 = arg(a0, 0);
  allocp[1] = x3;
  x2 = makecons(&allocp[0]);
  allocp[2] = x2;
  allocp[3] = x1;
  x4 = makecons(&allocp[2]);
  a0 = makefunctor(&string_const_8);
  a1 = x4;
  a2 = a4;
  a3 = a5;
  allocp += 4;
  execute(klicformat_4_0);
  goto klicformat_4_ext_interrupt;
 case functor_alloc__var_2:
  unify(a3, a2);
  x0 = arg(a0, 1);
  bblt_add(a2,x0,x1);
  allocp[0] = NILATOM;
  allocp[1] = x1;
  x2 = makecons(&allocp[0]);
  allocp[2] = x2;
  x4 = arg(a0, 0);
  allocp[3] = x4;
  x3 = makecons(&allocp[2]);
  allocp[4] = x3;
  allocp[5] = x1;
  x5 = makecons(&allocp[4]);
  a0 = makefunctor(&string_const_7);
  a1 = x5;
  a2 = a4;
  a3 = a5;
  allocp += 6;
  execute(klicformat_4_0);
  goto klicformat_4_ext_interrupt;
 case functor_alloc__elem_3:
  unify(a3, a2);
  x0 = arg(a0, 2);
  bblt_add(a2,x0,x1);
  allocp[0] = NILATOM;
  x3 = arg(a0, 1);
  allocp[1] = x3;
  x2 = makecons(&allocp[0]);
  allocp[2] = x2;
  x5 = arg(a0, 0);
  allocp[3] = x5;
  x4 = makecons(&allocp[2]);
  allocp[4] = x4;
  allocp[5] = x1;
  x6 = makecons(&allocp[4]);
  a0 = makefunctor(&string_const_5);
  a1 = x6;
  a2 = a4;
  a3 = a5;
  allocp += 6;
  execute(klicformat_4_0);
  goto klicformat_4_ext_interrupt;
 case functor_alloc__const_2:
  unify(a3, a2);
  x0 = arg(a0, 1);
  bblt_add(a2,x0,x1);
  allocp[0] = NILATOM;
  x3 = arg(a0, 0);
  allocp[1] = x3;
  x2 = makecons(&allocp[0]);
  allocp[2] = x2;
  allocp[3] = x1;
  x4 = makecons(&allocp[2]);
  a0 = makefunctor(&string_const_6);
  a1 = x4;
  a2 = a4;
  a3 = a5;
  allocp += 4;
  execute(klicformat_4_0);
  goto klicformat_4_ext_interrupt;
 case functor_make__space_1:
  unify(a5, a4);
  x0 = arg(a0, 0);
  bblt_add(a2,x0,x1);
  unify(a3, x1);
  proceed();
 case functor_unify_3:
  allocp[0] = NILATOM;
  x1 = arg(a0, 1);
  allocp[1] = x1;
  x0 = makecons(&allocp[0]);
  allocp[2] = x0;
  x3 = arg(a0, 0);
  allocp[3] = x3;
  x2 = makecons(&allocp[2]);
  allocp[4] = (q)qp;
  allocp[5] = (q)(&predicate_klic__comp__obj_xklicformat_4);
  allocp[6] = makefunctor(&string_const_44);
  allocp[7] = x2;
  allocp[8] = x4 = makeref(&allocp[8]);
  allocp[9] = a5;
  x5 = makeint(0L);
  qp = (struct goalrec*)&allocp[4];
  allocp += 10;
  unify_value(a3, x5);
  a0 = arg(a0, 2);
  a1 = a2;
  a2 = a4;
  a3 = x4;
  execute(write__adjust_4_0);
  goto write__adjust_4_ext_interrupt;
 case functor_unify__value_3:
  allocp[0] = NILATOM;
  x1 = arg(a0, 1);
  allocp[1] = x1;
  x0 = makecons(&allocp[0]);
  allocp[2] = x0;
  x3 = arg(a0, 0);
  allocp[3] = x3;
  x2 = makecons(&allocp[2]);
  allocp[4] = (q)qp;
  allocp[5] = (q)(&predicate_klic__comp__obj_xklicformat_4);
  allocp[6] = makefunctor(&string_const_48);
  allocp[7] = x2;
  allocp[8] = x4 = makeref(&allocp[8]);
  allocp[9] = a5;
  x5 = makeint(0L);
  qp = (struct goalrec*)&allocp[4];
  allocp += 10;
  unify_value(a3, x5);
  a0 = arg(a0, 2);
  a1 = a2;
  a2 = a4;
  a3 = x4;
  execute(write__adjust_4_0);
  goto write__adjust_4_ext_interrupt;
 case functor_execute_4:
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__obj_xwrite__one__execute_5);
  x0 = arg(a0, 0);
  allocp[2] = x0;
  x1 = arg(a0, 1);
  allocp[3] = x1;
  x2 = arg(a0, 2);
  allocp[4] = x2;
  allocp[5] = x3 = makeref(&allocp[5]);
  allocp[6] = a5;
  x4 = makeint(0L);
  qp = (struct goalrec*)&allocp[0];
  allocp += 7;
  unify_value(a3, x4);
  a0 = arg(a0, 3);
  a1 = a2;
  a2 = a4;
  a3 = x3;
  execute(write__adjust_4_0);
  goto write__adjust_4_ext_interrupt;
 case functor_proceed_1:
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__obj_xklicformat_3);
  allocp[2] = makefunctor(&string_const_37);
  allocp[3] = x0 = makeref(&allocp[3]);
  allocp[4] = a5;
  x1 = makeint(0L);
  qp = (struct goalrec*)&allocp[0];
  allocp += 5;
  unify_value(a3, x1);
  a0 = arg(a0, 0);
  a1 = a2;
  a2 = a4;
  a3 = x0;
  execute(write__adjust_4_0);
  goto write__adjust_4_ext_interrupt;
 case functor_set__pred_2:
 write__one_6_2:
  switch (ptagof(a1)) {
 case FUNCTOR:
  if (functor_of(a1) != makesym(functor_info_3)) goto write__one_6_3;
  unify(a3, a2);
  x0 = makeint(1L);
  bblt_add_no_check_y(a2,x0,x1);
  allocp[0] = NILATOM;
  x3 = arg(a0, 1);
  allocp[1] = x3;
  x2 = makecons(&allocp[0]);
  allocp[2] = x2;
  x5 = arg(a0, 0);
  allocp[3] = x5;
  x4 = makecons(&allocp[2]);
  allocp[4] = x4;
  x7 = arg(a1, 0);
  allocp[5] = x7;
  x6 = makecons(&allocp[4]);
  allocp[6] = x6;
  allocp[7] = x1;
  x8 = makecons(&allocp[6]);
  a0 = makefunctor(&string_const_2);
  a1 = x8;
  a2 = a4;
  a3 = a5;
  allocp += 8;
  execute(klicformat_4_0);
  goto klicformat_4_ext_interrupt;
 case VARREF:
  deref_and_jump(a1,write__one_6_2);
  *reasonp++ =  a1;
  goto write__one_6_3;
  };
  goto write__one_6_3;
 case functor_set__ext__pred_3:
  unify(a3, a2);
  x0 = makeint(1L);
  bblt_add_no_check_y(a2,x0,x1);
  allocp[0] = NILATOM;
  x3 = arg(a0, 2);
  allocp[1] = x3;
  x2 = makecons(&allocp[0]);
  allocp[2] = x2;
  x5 = arg(a0, 1);
  allocp[3] = x5;
  x4 = makecons(&allocp[2]);
  allocp[4] = x4;
  x7 = arg(a0, 0);
  allocp[5] = x7;
  x6 = makecons(&allocp[4]);
  allocp[6] = x6;
  allocp[7] = x1;
  x8 = makecons(&allocp[6]);
  a0 = makefunctor(&string_const_2);
  a1 = x8;
  a2 = a4;
  a3 = a5;
  allocp += 8;
  execute(klicformat_4_0);
  goto klicformat_4_ext_interrupt;
 case functor_heappos_1:
  unify2(a5, a4, a3, a2);
  x0 = arg(a0, 0);
  unify(x0, a2);
  proceed();
 case functor_push__goal_2:
  unify(a3, a2);
  x0 = arg(a0, 1);
  unify(x0, a2);
  allocp[0] = NILATOM;
  x2 = arg(a0, 0);
  allocp[1] = x2;
  x1 = makecons(&allocp[0]);
  allocp[2] = x1;
  allocp[3] = a2;
  x3 = makecons(&allocp[2]);
  a0 = makefunctor(&string_const_3);
  a1 = x3;
  a2 = a4;
  a3 = a5;
  allocp += 4;
  execute(klicformat_4_0);
  goto klicformat_4_ext_interrupt;
 case functor_throw__goal_3:
  unify(a3, a2);
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
  a0 = makefunctor(&string_const_43);
  a1 = x4;
  a2 = a4;
  a3 = a5;
  allocp += 6;
  execute(klicformat_4_0);
  goto klicformat_4_ext_interrupt;
 case functor_enq__at__prio_3:
  unify(a3, a2);
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
  a0 = makefunctor(&string_const_14);
  a1 = x4;
  a2 = a4;
  a3 = a5;
  allocp += 6;
  execute(klicformat_4_0);
  goto klicformat_4_ext_interrupt;
 case functor_enq__at__lower__prio_3:
  unify(a3, a2);
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
  a0 = makefunctor(&string_const_12);
  a1 = x4;
  a2 = a4;
  a3 = a5;
  allocp += 6;
  execute(klicformat_4_0);
  goto klicformat_4_ext_interrupt;
 case functor_enq__at__prio__no__check_3:
  unify(a3, a2);
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
  a0 = makefunctor(&string_const_15);
  a1 = x4;
  a2 = a4;
  a3 = a5;
  allocp += 6;
  execute(klicformat_4_0);
  goto klicformat_4_ext_interrupt;
 case functor_enq__at__lower__prio__no__check_3:
  unify(a3, a2);
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
  a0 = makefunctor(&string_const_13);
  a1 = x4;
  a2 = a4;
  a3 = a5;
  allocp += 6;
  execute(klicformat_4_0);
  goto klicformat_4_ext_interrupt;
 case functor_set__qp_2:
  unify(a3, a2);
  allocp[0] = NILATOM;
  x1 = arg(a0, 1);
  allocp[1] = x1;
  x0 = makecons(&allocp[0]);
  allocp[2] = x0;
  x3 = arg(a0, 0);
  allocp[3] = x3;
  x2 = makecons(&allocp[2]);
  a0 = makefunctor(&string_const_50);
  a1 = x2;
  a2 = a4;
  a3 = a5;
  allocp += 4;
  execute(klicformat_4_0);
  goto klicformat_4_ext_interrupt;
 case functor_set__value_2:
  unify(a3, a2);
  x0 = arg(a0, 0);
  bblt_add(x0,a2,x1);
  x2 = makeint(2L);
  bblt_add_no_check_y(x1,x2,x3);
  allocp[0] = NILATOM;
  x5 = arg(a0, 1);
  allocp[1] = x5;
  x4 = makecons(&allocp[0]);
  allocp[2] = x4;
  allocp[3] = x3;
  x6 = makecons(&allocp[2]);
  a0 = makefunctor(&string_const_8);
  a1 = x6;
  a2 = a4;
  a3 = a5;
  allocp += 4;
  execute(klicformat_4_0);
  goto klicformat_4_ext_interrupt;
 case functor_set__newvar_2:
  unify(a3, a2);
  x0 = arg(a0, 0);
  bblt_add(x0,a2,x1);
  x2 = makeint(2L);
  bblt_add_no_check_y(x1,x2,x3);
  allocp[0] = NILATOM;
  allocp[1] = x3;
  x4 = makecons(&allocp[0]);
  allocp[2] = x4;
  x6 = arg(a0, 1);
  allocp[3] = x6;
  x5 = makecons(&allocp[2]);
  allocp[4] = x5;
  allocp[5] = x3;
  x7 = makecons(&allocp[4]);
  a0 = makefunctor(&string_const_7);
  a1 = x7;
  a2 = a4;
  a3 = a5;
  allocp += 6;
  execute(klicformat_4_0);
  goto klicformat_4_ext_interrupt;
 case functor_set__elem_3:
  unify(a3, a2);
  x0 = arg(a0, 0);
  bblt_add(x0,a2,x1);
  x2 = makeint(2L);
  bblt_add_no_check_y(x1,x2,x3);
  allocp[0] = NILATOM;
  x5 = arg(a0, 2);
  allocp[1] = x5;
  x4 = makecons(&allocp[0]);
  allocp[2] = x4;
  x7 = arg(a0, 1);
  allocp[3] = x7;
  x6 = makecons(&allocp[2]);
  allocp[4] = x6;
  allocp[5] = x3;
  x8 = makecons(&allocp[4]);
  a0 = makefunctor(&string_const_5);
  a1 = x8;
  a2 = a4;
  a3 = a5;
  allocp += 6;
  execute(klicformat_4_0);
  goto klicformat_4_ext_interrupt;
 case functor_set__const_2:
  unify(a3, a2);
  x0 = arg(a0, 0);
  bblt_add(x0,a2,x1);
  x2 = makeint(2L);
  bblt_add_no_check_y(x1,x2,x3);
  allocp[0] = NILATOM;
  x5 = arg(a0, 1);
  allocp[1] = x5;
  x4 = makecons(&allocp[0]);
  allocp[2] = x4;
  allocp[3] = x3;
  x6 = makecons(&allocp[2]);
  a0 = makefunctor(&string_const_6);
  a1 = x6;
  a2 = a4;
  a3 = a5;
  allocp += 4;
  execute(klicformat_4_0);
  goto klicformat_4_ext_interrupt;
 case functor_bblt_4:
  unify(a3, a2);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__obj_xwrite__one__bblt_6);
  x0 = arg(a0, 0);
  allocp[2] = x0;
  allocp[3] = x1 = makeref(&allocp[3]);
  x2 = arg(a0, 1);
  allocp[4] = x2;
  allocp[5] = x3 = makeref(&allocp[5]);
  allocp[6] = a4;
  allocp[7] = a5;
  allocp[8] = (q)(struct goalrec*)&allocp[0];
  allocp[9] = (q)(&predicate_klic__comp__util_xlength_2);
  x4 = arg(a0, 2);
  allocp[10] = x4;
  allocp[11] = x1;
  allocp[12] = (q)(struct goalrec*)&allocp[8];
  allocp[13] = (q)(&predicate_klic__comp__util_xappend_3);
  allocp[14] = x4;
  x5 = arg(a0, 3);
  allocp[15] = x5;
  allocp[16] = x3;
  qp = (struct goalrec*)&allocp[12];
  allocp += 17;
  proceed();
 case functor_gblt_5:
  unify(a3, a2);
  allocp[0] = NILATOM;
  x1 = arg(a0, 4);
  allocp[1] = x1;
  x0 = makecons(&allocp[0]);
  allocp[2] = x0;
  allocp[3] = a1;
  x2 = makecons(&allocp[2]);
  allocp[4] = x2;
  x4 = arg(a0, 3);
  allocp[5] = x4;
  x3 = makecons(&allocp[4]);
  allocp[6] = x3;
  x6 = arg(a0, 2);
  allocp[7] = x6;
  x5 = makecons(&allocp[6]);
  allocp[8] = x5;
  x8 = arg(a0, 0);
  allocp[9] = x8;
  x7 = makecons(&allocp[8]);
  a0 = makefunctor(&string_const_59);
  a1 = x7;
  a2 = a4;
  a3 = a5;
  allocp += 10;
  execute(klicformat_4_0);
  goto klicformat_4_ext_interrupt;
 case functor_gblt__pred_4:
  unify(a3, a2);
  allocp[0] = NILATOM;
  x1 = arg(a0, 3);
  allocp[1] = x1;
  x0 = makecons(&allocp[0]);
  allocp[2] = x0;
  allocp[3] = a1;
  x2 = makecons(&allocp[2]);
  allocp[4] = x2;
  x4 = arg(a0, 2);
  allocp[5] = x4;
  x3 = makecons(&allocp[4]);
  allocp[6] = x3;
  x6 = arg(a0, 0);
  allocp[7] = x6;
  x5 = makecons(&allocp[6]);
  a0 = makefunctor(&string_const_60);
  a1 = x5;
  a2 = a4;
  a3 = a5;
  allocp += 8;
  execute(klicformat_4_0);
  goto klicformat_4_ext_interrupt;
 case functor_equiv_2:
  unify(a3, a2);
  allocp[0] = NILATOM;
  x1 = arg(a0, 0);
  allocp[1] = x1;
  x0 = makecons(&allocp[0]);
  allocp[2] = x0;
  x3 = arg(a0, 1);
  allocp[3] = x3;
  x2 = makecons(&allocp[2]);
  a0 = makefunctor(&string_const_57);
  a1 = x2;
  a2 = a4;
  a3 = a5;
  allocp += 4;
  execute(klicformat_4_0);
  goto klicformat_4_ext_interrupt;
 case functor_new__generic_4:
  x0 = makeint(0L);
  unify_value(a3, x0);
  allocp[0] = NILATOM;
  allocp[1] = a2;
  x1 = makecons(&allocp[0]);
  allocp[2] = x1;
  x3 = arg(a0, 2);
  allocp[3] = x3;
  x2 = makecons(&allocp[2]);
  allocp[4] = x2;
  x5 = arg(a0, 1);
  allocp[5] = x5;
  x4 = makecons(&allocp[4]);
  allocp[6] = x4;
  x7 = arg(a0, 0);
  allocp[7] = x7;
  x6 = makecons(&allocp[6]);
  allocp[8] = (q)qp;
  allocp[9] = (q)(&predicate_klic__comp__obj_xklicformat_4);
  allocp[10] = makefunctor(&string_const_35);
  allocp[11] = x6;
  allocp[12] = x8 = makeref(&allocp[12]);
  allocp[13] = a5;
  a0 = arg(a0, 3);
  a1 = a4;
  a2 = x8;
  qp = (struct goalrec*)&allocp[8];
  allocp += 14;
  execute(write__adjust__qp_3_0);
  goto write__adjust__qp_3_ext_interrupt;
 case functor_call__generic_3:
  x0 = makeint(0L);
  unify_value(a3, x0);
  allocp[0] = NILATOM;
  allocp[1] = a2;
  x1 = makecons(&allocp[0]);
  allocp[2] = x1;
  x3 = arg(a0, 1);
  allocp[3] = x3;
  x2 = makecons(&allocp[2]);
  allocp[4] = x2;
  x5 = arg(a0, 0);
  allocp[5] = x5;
  x4 = makecons(&allocp[4]);
  allocp[6] = (q)qp;
  allocp[7] = (q)(&predicate_klic__comp__obj_xklicformat_4);
  allocp[8] = makefunctor(&string_const_10);
  allocp[9] = x4;
  allocp[10] = x6 = makeref(&allocp[10]);
  allocp[11] = a5;
  a0 = arg(a0, 2);
  a1 = a4;
  a2 = x6;
  qp = (struct goalrec*)&allocp[6];
  allocp += 12;
  execute(write__adjust__qp_3_0);
  goto write__adjust__qp_3_ext_interrupt;
 case functor_guard__generic_4:
  unify(a3, a2);
  allocp[0] = NILATOM;
  x1 = arg(a0, 3);
  allocp[1] = x1;
  x0 = makecons(&allocp[0]);
  allocp[2] = x0;
  allocp[3] = a1;
  x2 = makecons(&allocp[2]);
  allocp[4] = x2;
  x4 = arg(a0, 2);
  allocp[5] = x4;
  x3 = makecons(&allocp[4]);
  allocp[6] = x3;
  x6 = arg(a0, 1);
  allocp[7] = x6;
  x5 = makecons(&allocp[6]);
  allocp[8] = x5;
  x8 = arg(a0, 0);
  allocp[9] = x8;
  x7 = makecons(&allocp[8]);
  a0 = makefunctor(&string_const_20);
  a1 = x7;
  a2 = a4;
  a3 = a5;
  allocp += 10;
  execute(klicformat_4_0);
  goto klicformat_4_ext_interrupt;
 case functor_store__generic__arg_2:
  unify(a3, a2);
  allocp[0] = NILATOM;
  x1 = arg(a0, 0);
  allocp[1] = x1;
  x0 = makecons(&allocp[0]);
  allocp[2] = x0;
  x3 = arg(a0, 1);
  allocp[3] = x3;
  x2 = makecons(&allocp[2]);
  a0 = makefunctor(&string_const_18);
  a1 = x2;
  a2 = a4;
  a3 = a5;
  allocp += 4;
  execute(klicformat_4_0);
  goto klicformat_4_ext_interrupt;
 case functor_load__generic__arg_2:
  unify(a3, a2);
  allocp[0] = NILATOM;
  x1 = arg(a0, 1);
  allocp[1] = x1;
  x0 = makecons(&allocp[0]);
  allocp[2] = x0;
  x3 = arg(a0, 0);
  allocp[3] = x3;
  x2 = makecons(&allocp[2]);
  a0 = makefunctor(&string_const_52);
  a1 = x2;
  a2 = a4;
  a3 = a5;
  allocp += 4;
  execute(klicformat_4_0);
  goto klicformat_4_ext_interrupt;
  };
  goto write__one_6_3;
 case VARREF:
  deref_and_jump(a0,write__one_6_1);
  *reasonp++ =  a0;
  goto write__one_6_3;
  };
  goto write__one_6_3;
 write__one_6_3:
  otherwise(write__one_6_interrupt);
  unify2(a5, a4, a3, a2);
  allocp[0] = NILATOM;
  allocp[1] = a0;
  x0 = makecons(&allocp[0]);
  allocp[2] = (q)qp;
  allocp[3] = (q)(&predicate_klic__comp__message_xwarning_2);
  allocp[4] = makefunctor(&string_const_74);
  allocp[5] = x0;
  qp = (struct goalrec*)&allocp[2];
  allocp += 6;
  proceed();
 write__one_6_ext_interrupt:
  reasonp = 0l;
 write__one_6_interrupt:
  toppred = &predicate_klic__comp__obj_xwrite__one_6;
  goto interrupt_6;
 }

 write__one__move_4_top: {
  q x0, x1;
  a2 = qp->args[2];
  a3 = qp->args[3];
  qp = qp->next;
 write__one__move_4_clear_reason:
  reasonp = reasons;
 write__one__move_4_0:
 write__one__move_4_1:
  if (isref(a0)) goto write__one__move_4_4;
 write__one__move_4_2:
  if (isref(a0)) goto write__one__move_4_3;
  if_not_equal(a0, a1, write__one__move_4_5);
  unify(a3, a2);
  proceed();
 write__one__move_4_3:
  deref_and_jump(a0,write__one__move_4_2);
  *reasonp++ =  a0;
  goto write__one__move_4_5;
 write__one__move_4_4:
  deref_and_jump(a0,write__one__move_4_1);
  *reasonp++ =  a0;
  goto write__one__move_4_5;
 write__one__move_4_5:
  otherwise(write__one__move_4_interrupt);
  allocp[0] = NILATOM;
  allocp[1] = a1;
  x0 = makecons(&allocp[0]);
  allocp[2] = x0;
  allocp[3] = a0;
  x1 = makecons(&allocp[2]);
  a0 = makefunctor(&string_const_57);
  a1 = x1;
  allocp += 4;
  execute(klicformat_4_0);
  goto klicformat_4_ext_interrupt;
 write__one__move_4_ext_interrupt:
  reasonp = 0l;
 write__one__move_4_interrupt:
  toppred = &predicate_klic__comp__obj_xwrite__one__move_4;
  goto interrupt_4;
 }

 write__one__execute_5_top: {
  q x0, x1, x2, x3, x4, x5;
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  qp = qp->next;
 write__one__execute_5_clear_reason:
  reasonp = reasons;
 write__one__execute_5_0:
 write__one__execute_5_1:
  if (!isint(a2)) goto write__one__execute_5_3;
  x0 = makeint(0L);
  gblt_eq(a2,x0,write__one__execute_5_2);
  allocp[0] = NILATOM;
  allocp[1] = a1;
  x1 = makecons(&allocp[0]);
  allocp[2] = x1;
  allocp[3] = a0;
  x2 = makecons(&allocp[2]);
  allocp[4] = x2;
  allocp[5] = a1;
  x3 = makecons(&allocp[4]);
  allocp[6] = x3;
  allocp[7] = a0;
  x4 = makecons(&allocp[6]);
  a0 = makefunctor(&string_const_16);
  a1 = x4;
  a2 = a3;
  a3 = a4;
  allocp += 8;
  execute(klicformat_4_0);
  goto klicformat_4_ext_interrupt;
 write__one__execute_5_2:
  x1 = makeint(0L);
  gblt_not_eq(a2,x1,write__one__execute_5_interrupt);
  allocp[0] = NILATOM;
  allocp[1] = a1;
  x2 = makecons(&allocp[0]);
  allocp[2] = x2;
  allocp[3] = a0;
  x3 = makecons(&allocp[2]);
  allocp[4] = x3;
  allocp[5] = a1;
  x4 = makecons(&allocp[4]);
  allocp[6] = x4;
  allocp[7] = a0;
  x5 = makecons(&allocp[6]);
  a0 = makefunctor(&string_const_17);
  a1 = x5;
  a2 = a3;
  a3 = a4;
  allocp += 8;
  execute(klicformat_4_0);
  goto klicformat_4_ext_interrupt;
 write__one__execute_5_3:
  if (!isref(a2)) goto write__one__execute_5_interrupt;
  deref_and_jump(a2,write__one__execute_5_1);
  *reasonp++ =  a2;
  goto write__one__execute_5_interrupt;
 write__one__execute_5_ext_interrupt:
  reasonp = 0l;
 write__one__execute_5_interrupt:
  goto interrupt_5;
 }

 write__one__bblt_6_top: {
  q x0, x1, x2, x3;
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  qp = qp->next;
 write__one__bblt_6_clear_reason:
  reasonp = reasons;
 write__one__bblt_6_0:
 write__one__bblt_6_1:
  switch (ptagof(a0)) {
 case CONS:
  x0 = car_of(a0);
 write__one__bblt_6_2:
  switch (ptagof(x0)) {
 case ATOMIC:
 write__one__bblt_6_3:
  switch ((unsigned long)x0) {
 case (unsigned long) makeint(1L):
  x1 = cdr_of(a0);
 write__one__bblt_6_4:
  switch (ptagof(x1)) {
 case ATOMIC:
  if (x1 != NILATOM) goto write__one__bblt_6_8;
 write__one__bblt_6_5:
  switch (ptagof(a1)) {
 case ATOMIC:
 write__one__bblt_6_6:
  switch ((unsigned long)a1) {
 case (unsigned long) makeint(1L):
  allocp[0] = NILATOM;
  allocp[1] = a3;
  x2 = makecons(&allocp[0]);
  allocp[2] = x2;
  allocp[3] = a2;
  x3 = makecons(&allocp[2]);
  a0 = makefunctor(&string_const_58);
  a1 = x3;
  a2 = a4;
  a3 = a5;
  allocp += 4;
  execute(klicformat_4_0);
  goto klicformat_4_ext_interrupt;
 case (unsigned long) makeint(2L):
  allocp[0] = NILATOM;
  allocp[1] = a3;
  x2 = makecons(&allocp[0]);
  allocp[2] = x2;
  allocp[3] = a2;
  x3 = makecons(&allocp[2]);
  a0 = makefunctor(&string_const_63);
  a1 = x3;
  a2 = a4;
  a3 = a5;
  allocp += 4;
  execute(klicformat_4_0);
  goto klicformat_4_ext_interrupt;
  };
  goto write__one__bblt_6_8;
 case VARREF:
  deref_and_jump(a1,write__one__bblt_6_5);
  *reasonp++ =  a1;
  goto write__one__bblt_6_8;
  };
  goto write__one__bblt_6_8;
 case VARREF:
  deref_and_jump(x1,write__one__bblt_6_4);
  *reasonp++ =  x1;
  goto write__one__bblt_6_8;
  };
  goto write__one__bblt_6_8;
 case (unsigned long) makeint(2L):
  x1 = cdr_of(a0);
 write__one__bblt_6_7:
  switch (ptagof(x1)) {
 case ATOMIC:
  if (x1 != NILATOM) goto write__one__bblt_6_8;
  allocp[0] = NILATOM;
  allocp[1] = a3;
  x2 = makecons(&allocp[0]);
  allocp[2] = x2;
  allocp[3] = a2;
  x3 = makecons(&allocp[2]);
  a0 = makefunctor(&string_const_62);
  a1 = x3;
  a2 = a4;
  a3 = a5;
  allocp += 4;
  execute(klicformat_4_0);
  goto klicformat_4_ext_interrupt;
 case VARREF:
  deref_and_jump(x1,write__one__bblt_6_7);
  *reasonp++ =  x1;
  goto write__one__bblt_6_8;
  };
  goto write__one__bblt_6_8;
  };
  goto write__one__bblt_6_8;
 case VARREF:
  deref_and_jump(x0,write__one__bblt_6_2);
  *reasonp++ =  x0;
  goto write__one__bblt_6_8;
  };
  goto write__one__bblt_6_8;
 case ATOMIC:
  if (a0 != NILATOM) goto write__one__bblt_6_8;
  allocp[0] = NILATOM;
  allocp[1] = a3;
  x0 = makecons(&allocp[0]);
  allocp[2] = x0;
  allocp[3] = a2;
  x1 = makecons(&allocp[2]);
  a0 = makefunctor(&string_const_61);
  a1 = x1;
  a2 = a4;
  a3 = a5;
  allocp += 4;
  execute(klicformat_4_0);
  goto klicformat_4_ext_interrupt;
 case VARREF:
  deref_and_jump(a0,write__one__bblt_6_1);
  *reasonp++ =  a0;
  goto write__one__bblt_6_8;
  };
  goto write__one__bblt_6_8;
 write__one__bblt_6_8:
  otherwise(write__one__bblt_6_interrupt);
  allocp[0] = NILATOM;
  allocp[1] = a3;
  x0 = makecons(&allocp[0]);
  allocp[2] = x0;
  allocp[3] = a2;
  x1 = makecons(&allocp[2]);
  a0 = makefunctor(&string_const_58);
  a1 = x1;
  a2 = a4;
  a3 = a5;
  allocp += 4;
  execute(klicformat_4_0);
  goto klicformat_4_ext_interrupt;
 write__one__bblt_6_ext_interrupt:
  reasonp = 0l;
 write__one__bblt_6_interrupt:
  goto interrupt_6;
 }

 write__adjust_4_top: {
  q x0;
  a2 = qp->args[2];
  a3 = qp->args[3];
  qp = qp->next;
 write__adjust_4_clear_reason:
  reasonp = reasons;
 write__adjust_4_0:
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__obj_xwrite__adjust__allocp_3);
  allocp[2] = a1;
  allocp[3] = x0 = makeref(&allocp[3]);
  allocp[4] = a3;
  a1 = a2;
  a2 = x0;
  qp = (struct goalrec*)&allocp[0];
  allocp += 5;
  execute(write__adjust__qp_3_0);
  goto write__adjust__qp_3_ext_interrupt;
 write__adjust_4_ext_interrupt:
  reasonp = 0l;
 write__adjust_4_interrupt:
  toppred = &predicate_klic__comp__obj_xwrite__adjust_4;
  goto interrupt_4;
 }

 write__adjust__qp_3_top: {
  q x0, x1, x2;
  a2 = qp->args[2];
  qp = qp->next;
 write__adjust__qp_3_clear_reason:
  reasonp = reasons;
 write__adjust__qp_3_0:
 write__adjust__qp_3_1:
  switch (ptagof(a0)) {
 case ATOMIC:
  if (a0 != makesym(atom_qp)) goto write__adjust__qp_3_2;
  unify(a2, a1);
  proceed();
 case VARREF:
  deref_and_jump(a0,write__adjust__qp_3_1);
  *reasonp++ =  a0;
  goto write__adjust__qp_3_interrupt;
  };
  goto write__adjust__qp_3_2;
 write__adjust__qp_3_2:
  x0 = makesym(atom_qp);
  gblt_diff(a0,x0,write__adjust__qp_3_interrupt);
  allocp[0] = NILATOM;
  allocp[1] = a0;
  x1 = makecons(&allocp[0]);
  a0 = makefunctor(&string_const_38);
  x2 = a1;
  a3 = a2;
  a2 = x2;
  a1 = x1;
  allocp += 2;
  execute(klicformat_4_0);
  goto klicformat_4_ext_interrupt;
 write__adjust__qp_3_ext_interrupt:
  reasonp = 0l;
 write__adjust__qp_3_interrupt:
  toppred = &predicate_klic__comp__obj_xwrite__adjust__qp_3;
  goto interrupt_3;
 }

 write__adjust__allocp_3_top: {
  q x0, x1, x2, x3;
  a2 = qp->args[2];
  qp = qp->next;
 write__adjust__allocp_3_clear_reason:
  reasonp = reasons;
 write__adjust__allocp_3_0:
 write__adjust__allocp_3_1:
  if (!isint(a0)) goto write__adjust__allocp_3_3;
  x0 = makeint(0L);
  gblt_eq(a0,x0,write__adjust__allocp_3_2);
  unify(a2, a1);
  proceed();
 write__adjust__allocp_3_2:
  x1 = makeint(0L);
  gblt_not_eq(a0,x1,write__adjust__allocp_3_interrupt);
  allocp[0] = NILATOM;
  allocp[1] = a0;
  x2 = makecons(&allocp[0]);
  a0 = makefunctor(&string_const_1);
  x3 = a1;
  a3 = a2;
  a2 = x3;
  a1 = x2;
  allocp += 2;
  execute(klicformat_4_0);
  goto klicformat_4_ext_interrupt;
 write__adjust__allocp_3_3:
  if (!isref(a0)) goto write__adjust__allocp_3_interrupt;
  deref_and_jump(a0,write__adjust__allocp_3_1);
  *reasonp++ =  a0;
  goto write__adjust__allocp_3_interrupt;
 write__adjust__allocp_3_ext_interrupt:
  reasonp = 0l;
 write__adjust__allocp_3_interrupt:
  goto interrupt_3;
 }

 klicformat__stderr_2_top: {
  q x0, x1, x2, x3;
  qp = qp->next;
 klicformat__stderr_2_clear_reason:
  reasonp = reasons;
 klicformat__stderr_2_0:
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__obj_xklicformat_4);
  allocp[2] = a0;
  allocp[3] = a1;
  allocp[4] = x0 = makeref(&allocp[4]);
  allocp[5] = NILATOM;
  allocp[6] = makesym(functor_normal_1);
  allocp[7] = x0;
  x1 = makefunctor(&allocp[6]);
  allocp[8] = makesym(functor_stderr_1);
  allocp[9] = x1;
  x2 = makefunctor(&allocp[8]);
  allocp[10] = NILATOM;
  allocp[11] = x2;
  x3 = makecons(&allocp[10]);
  allocp[12] = (q)(struct goalrec*)&allocp[0];
  allocp[13] = (q)(&predicate_klicio_xklicio_1);
  allocp[14] = x3;
  qp = (struct goalrec*)&allocp[12];
  allocp += 15;
  proceed();
 klicformat__stderr_2_ext_interrupt:
  reasonp = 0l;
 klicformat__stderr_2_interrupt:
  goto interrupt_2;
 }

 flush_2_top: {
  q x0, x1, x2, x3, x4;
  qp = qp->next;
 flush_2_clear_reason:
  reasonp = reasons;
 flush_2_0:
 flush_2_1:
  switch (ptagof(a0)) {
 case ATOMIC:
  if (a0 != makesym(atom_user__error)) goto flush_2_interrupt;
  allocp[0] = makesym(functor_fflush_1);
  allocp[1] = a1;
  x0 = makefunctor(&allocp[0]);
  allocp[2] = NILATOM;
  allocp[3] = x0;
  x1 = makecons(&allocp[2]);
  allocp[4] = makesym(functor_normal_1);
  allocp[5] = x1;
  x2 = makefunctor(&allocp[4]);
  allocp[6] = makesym(functor_stderr_1);
  allocp[7] = x2;
  x3 = makefunctor(&allocp[6]);
  allocp[8] = NILATOM;
  allocp[9] = x3;
  x4 = makecons(&allocp[8]);
  allocp[10] = (q)qp;
  allocp[11] = (q)(&predicate_klicio_xklicio_1);
  allocp[12] = x4;
  qp = (struct goalrec*)&allocp[10];
  allocp += 13;
  proceed();
 case VARREF:
  deref_and_jump(a0,flush_2_1);
  *reasonp++ =  a0;
  goto flush_2_interrupt;
  };
  goto flush_2_interrupt;
 flush_2_ext_interrupt:
  reasonp = 0l;
 flush_2_interrupt:
  goto interrupt_2;
 }

 flush_3_top: {
  q x0, x1;
  a2 = qp->args[2];
  qp = qp->next;
 flush_3_clear_reason:
  reasonp = reasons;
 flush_3_0:
  allocp[0] = makesym(functor_fflush_1);
  allocp[1] = a0;
  x0 = makefunctor(&allocp[0]);
  allocp[2] = a2;
  allocp[3] = x0;
  x1 = makecons(&allocp[2]);
  allocp += 4;
  unify_value(a1, x1);
  proceed();
 flush_3_ext_interrupt:
  reasonp = 0l;
 flush_3_interrupt:
  goto interrupt_3;
 }

 klicformat_3_top: {
  q x0, x1;
  a2 = qp->args[2];
  qp = qp->next;
 klicformat_3_clear_reason:
  reasonp = reasons;
 klicformat_3_0:
  allocp[0] = makesym(functor_fwrite_1);
  allocp[1] = a0;
  x0 = makefunctor(&allocp[0]);
  allocp[2] = a2;
  allocp[3] = x0;
  x1 = makecons(&allocp[2]);
  allocp += 4;
  unify_value(a1, x1);
  proceed();
 klicformat_3_ext_interrupt:
  reasonp = 0l;
 klicformat_3_interrupt:
  toppred = &predicate_klic__comp__obj_xklicformat_3;
  goto interrupt_3;
 }

 klicformat_4_top: {
  q x0, x1, x2, x3;
  a2 = qp->args[2];
  a3 = qp->args[3];
  qp = qp->next;
 klicformat_4_clear_reason:
  reasonp = reasons;
 klicformat_4_0:
 klicformat_4_1:
  switch (ptagof(a0)) {
 case FUNCTOR:
 klicformat_4_2:
  if (!isgobj(a0)) goto klicformat_4_interrupt;
 klicformat_4_3:
  gblt_is_string(a0,klicformat_4_interrupt);
  gblt_size_of_string(a0,x0,klicformat_4_interrupt);
  gblt_element_size_of_string(a0,x1,klicformat_4_interrupt);
 klicformat_4_4:
  if (x1 != makeint(8L)) goto klicformat_4_interrupt;
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__obj_xwait__and__out_4);
  allocp[2] = x2 = makeref(&allocp[2]);
  allocp[3] = x3 = makeref(&allocp[3]);
  allocp[4] = a2;
  allocp[5] = a3;
  a2 = a0;
  a3 = a1;
  a4 = x2;
  a5 = x3;
  a1 = x0;
  a0 = makeint(0L);
  qp = (struct goalrec*)&allocp[0];
  allocp += 6;
  execute(klicformat_6_0);
  goto klicformat_6_ext_interrupt;
 case VARREF:
  deref_and_jump(a0,klicformat_4_1);
  *reasonp++ =  a0;
  goto klicformat_4_interrupt;
  };
  goto klicformat_4_interrupt;
 klicformat_4_ext_interrupt:
  reasonp = 0l;
 klicformat_4_interrupt:
  toppred = &predicate_klic__comp__obj_xklicformat_4;
  goto interrupt_4;
 }

 wait__and__out_4_top: {
  a2 = qp->args[2];
  a3 = qp->args[3];
  qp = qp->next;
 wait__and__out_4_clear_reason:
  reasonp = reasons;
 wait__and__out_4_0:
  unify2(a2, a0, a3, a1);
  proceed();
 wait__and__out_4_ext_interrupt:
  reasonp = 0l;
 wait__and__out_4_interrupt:
  goto interrupt_4;
 }

 klicformat_6_top: {
  q x0, x1, x2, x3, x4, x5, x6, x7, x8, x9;
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  qp = qp->next;
 klicformat_6_clear_reason:
  reasonp = reasons;
 klicformat_6_0:
 klicformat_6_1:
  if (!isint(a0)) goto klicformat_6_9;
 klicformat_6_2:
  if (!isint(a1)) goto klicformat_6_8;
  gblt_greater_or_eq(a0,a1,klicformat_6_3);
  unify(a5, a4);
  proceed();
 klicformat_6_3:
  gblt_less(a0,a1,klicformat_6_interrupt);
 klicformat_6_4:
  switch (ptagof(a2)) {
 case FUNCTOR:
 klicformat_6_5:
  if (!isgobj(a2)) goto klicformat_6_interrupt;
 klicformat_6_6:
  gblt_is_string(a2,klicformat_6_interrupt);
  gblt_element_of_string(a2,a0,x0,klicformat_6_interrupt);
  x2 = makeint(1L);
  gblt_add(a0,x2,x1,klicformat_6_interrupt);
  x3 = makeint(126L);
  gblt_not_eq(x0,x3,klicformat_6_7);
  allocp[0] = x5 = makeref(&allocp[0]);
  allocp[1] = x0;
  x4 = makecons(&allocp[0]);
  allocp += 2;
  unify_value(a4, x4);
  a0 = x1;
  a4 = x5;
  execute(klicformat_6_0);
  goto klicformat_6_ext_interrupt;
 klicformat_6_7:
  x4 = makeint(126L);
  gblt_eq(x0,x4,klicformat_6_interrupt);
  gblt_is_string(a2,klicformat_6_interrupt);
  gblt_element_of_string(a2,x1,x5,klicformat_6_interrupt);
  x6 = makeint(1L);
  bblt_add_no_check(x1,x6,x7);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__obj_xklicformat_6);
  allocp[2] = x7;
  allocp[3] = a1;
  allocp[4] = a2;
  allocp[5] = x8 = makeref(&allocp[5]);
  allocp[6] = x9 = makeref(&allocp[6]);
  allocp[7] = a5;
  a0 = x5;
  a1 = a3;
  a2 = x8;
  a3 = a4;
  a4 = x9;
  qp = (struct goalrec*)&allocp[0];
  allocp += 8;
  execute(format__special_5_0);
  goto format__special_5_ext_interrupt;
 case VARREF:
  deref_and_jump(a2,klicformat_6_4);
  *reasonp++ =  a2;
  goto klicformat_6_interrupt;
  };
  goto klicformat_6_interrupt;
 klicformat_6_8:
  if (!isref(a1)) goto klicformat_6_interrupt;
  deref_and_jump(a1,klicformat_6_2);
  *reasonp++ =  a1;
  goto klicformat_6_interrupt;
 klicformat_6_9:
  if (!isref(a0)) goto klicformat_6_interrupt;
  deref_and_jump(a0,klicformat_6_1);
  *reasonp++ =  a0;
  goto klicformat_6_interrupt;
 klicformat_6_ext_interrupt:
  reasonp = 0l;
 klicformat_6_interrupt:
  toppred = &predicate_klic__comp__obj_xklicformat_6;
  goto interrupt_6;
 }

 format__special_5_top: {
  q x0, x1, x2, x3, x4, x5, x6, x7, x8;
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  qp = qp->next;
 format__special_5_clear_reason:
  reasonp = reasons;
 format__special_5_0:
 format__special_5_1:
  switch (ptagof(a0)) {
 case ATOMIC:
 format__special_5_2:
  switch ((unsigned long)a0) {
 case (unsigned long) makeint(97L):
 format__special_5_3:
  switch (ptagof(a1)) {
 case CONS:
  x0 = cdr_of(a1);
  unify(a2, x0);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__write_xwrite__atom_3);
  x1 = car_of(a1);
  allocp[2] = x1;
  allocp[3] = a3;
  allocp[4] = a4;
  qp = (struct goalrec*)&allocp[0];
  allocp += 5;
  proceed();
 case VARREF:
  deref_and_jump(a1,format__special_5_3);
  *reasonp++ =  a1;
  goto format__special_5_31;
  };
  goto format__special_5_31;
 case (unsigned long) makeint(99L):
 format__special_5_4:
  switch (ptagof(a1)) {
 case CONS:
  x0 = cdr_of(a1);
  unify(a2, x0);
  allocp[0] = a4;
  x2 = car_of(a1);
  allocp[1] = x2;
  x1 = makecons(&allocp[0]);
  allocp += 2;
  unify_value(a3, x1);
  proceed();
 case VARREF:
  deref_and_jump(a1,format__special_5_4);
  *reasonp++ =  a1;
  goto format__special_5_31;
  };
  goto format__special_5_31;
 case (unsigned long) makeint(100L):
 format__special_5_5:
  switch (ptagof(a1)) {
 case CONS:
  x0 = cdr_of(a1);
  unify(a2, x0);
  a0 = car_of(a1);
  a1 = a3;
  a2 = a4;
  execute(write__int_3_0);
  goto write__int_3_ext_interrupt;
 case VARREF:
  deref_and_jump(a1,format__special_5_5);
  *reasonp++ =  a1;
  goto format__special_5_31;
  };
  goto format__special_5_31;
 case (unsigned long) makeint(68L):
 format__special_5_6:
  switch (ptagof(a1)) {
 case CONS:
  x0 = car_of(a1);
 format__special_5_7:
  switch (ptagof(x0)) {
 case FUNCTOR:
  if (functor_of(x0) != makesym(functor__2D_2)) goto format__special_5_31;
  x1 = cdr_of(a1);
  unify(a2, x1);
  a0 = arg(x0, 0);
  a1 = arg(x0, 1);
  a2 = a3;
  a3 = a4;
  execute(format__special__D_4_0);
  goto format__special__D_4_ext_interrupt;
 case VARREF:
  deref_and_jump(x0,format__special_5_7);
  *reasonp++ =  x0;
  goto format__special_5_31;
  };
  goto format__special_5_31;
 case VARREF:
  deref_and_jump(a1,format__special_5_6);
  *reasonp++ =  a1;
  goto format__special_5_31;
  };
  goto format__special_5_31;
 case (unsigned long) makeint(101L):
 format__special_5_8:
  switch (ptagof(a1)) {
 case CONS:
  x0 = cdr_of(a1);
 format__special_5_9:
  switch (ptagof(x0)) {
 case CONS:
  x1 = cdr_of(x0);
  unify(a2, x1);
  a0 = car_of(a1);
  a1 = car_of(x0);
  a2 = a3;
  a3 = a4;
  execute(write__elem__access_4_0);
  goto write__elem__access_4_ext_interrupt;
 case VARREF:
  deref_and_jump(x0,format__special_5_9);
  *reasonp++ =  x0;
  goto format__special_5_31;
  };
  goto format__special_5_31;
 case VARREF:
  deref_and_jump(a1,format__special_5_8);
  *reasonp++ =  a1;
  goto format__special_5_31;
  };
  goto format__special_5_31;
 case (unsigned long) makeint(102L):
 format__special_5_10:
  switch (ptagof(a1)) {
 case CONS:
  x0 = car_of(a1);
 format__special_5_11:
  switch (ptagof(x0)) {
 case FUNCTOR:
  if (functor_of(x0) != makesym(functor__2F_2)) goto format__special_5_31;
  x1 = cdr_of(a1);
  unify(a2, x1);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__write_xwrite__funct__name_4);
  x2 = arg(x0, 0);
  allocp[2] = x2;
  x3 = arg(x0, 1);
  allocp[3] = x3;
  allocp[4] = a3;
  allocp[5] = a4;
  qp = (struct goalrec*)&allocp[0];
  allocp += 6;
  proceed();
 case VARREF:
  deref_and_jump(x0,format__special_5_11);
  *reasonp++ =  x0;
  goto format__special_5_31;
  };
  goto format__special_5_31;
 case VARREF:
  deref_and_jump(a1,format__special_5_10);
  *reasonp++ =  a1;
  goto format__special_5_31;
  };
  goto format__special_5_31;
 case (unsigned long) makeint(103L):
 format__special_5_12:
  switch (ptagof(a1)) {
 case CONS:
  x0 = cdr_of(a1);
  unify(a2, x0);
  allocp[0] = x1 = makeref(&allocp[0]);
  generic_arg[0] = x1;
  x2 = car_of(a1);
  call_generic(x2, functor_print_1, 1);
  allocp[0] = makesym(functor_fwrite_1);
  allocp[1] = x1;
  x3 = makefunctor(&allocp[0]);
  allocp[2] = a4;
  allocp[3] = x3;
  x4 = makecons(&allocp[2]);
  allocp += 4;
  unify_value(a3, x4);
  proceed();
 case VARREF:
  deref_and_jump(a1,format__special_5_12);
  *reasonp++ =  a1;
  goto format__special_5_31;
  };
  goto format__special_5_31;
 case (unsigned long) makeint(105L):
 format__special_5_13:
  switch (ptagof(a1)) {
 case CONS:
  x0 = cdr_of(a1);
  unify(x0, a2);
  a0 = car_of(a1);
  a1 = a3;
  a2 = a4;
  execute(format__special__i_3_0);
  goto format__special__i_3_ext_interrupt;
 case VARREF:
  deref_and_jump(a1,format__special_5_13);
  *reasonp++ =  a1;
  goto format__special_5_31;
  };
  goto format__special_5_31;
 case (unsigned long) makeint(107L):
 format__special_5_14:
  switch (ptagof(a1)) {
 case CONS:
  x0 = cdr_of(a1);
  unify(a2, x0);
  a0 = car_of(a1);
  a1 = a3;
  a2 = a4;
  execute(write__param_3_0);
  goto write__param_3_ext_interrupt;
 case VARREF:
  deref_and_jump(a1,format__special_5_14);
  *reasonp++ =  a1;
  goto format__special_5_31;
  };
  goto format__special_5_31;
 case (unsigned long) makeint(108L):
 format__special_5_15:
  switch (ptagof(a1)) {
 case CONS:
  x0 = car_of(a1);
 format__special_5_16:
  switch (ptagof(x0)) {
 case FUNCTOR:
  if (functor_of(x0) != makesym(functor_info_3)) goto format__special_5_31;
  x1 = cdr_of(a1);
 format__special_5_17:
  switch (ptagof(x1)) {
 case CONS:
  x2 = cdr_of(x1);
  unify(a2, x2);
  allocp[0] = NILATOM;
  x4 = car_of(x1);
  allocp[1] = x4;
  x3 = makecons(&allocp[0]);
  allocp[2] = x3;
  x6 = arg(x0, 2);
  allocp[3] = x6;
  x5 = makecons(&allocp[2]);
  allocp[4] = x5;
  x8 = arg(x0, 1);
  allocp[5] = x8;
  x7 = makecons(&allocp[4]);
  a0 = makefunctor(&string_const_95);
  a1 = x7;
  a2 = a3;
  a3 = a4;
  allocp += 6;
  execute(klicformat_4_0);
  goto klicformat_4_ext_interrupt;
 case VARREF:
  deref_and_jump(x1,format__special_5_17);
  *reasonp++ =  x1;
  goto format__special_5_31;
  };
  goto format__special_5_31;
 case VARREF:
  deref_and_jump(x0,format__special_5_16);
  *reasonp++ =  x0;
  goto format__special_5_31;
  };
  goto format__special_5_31;
 case VARREF:
  deref_and_jump(a1,format__special_5_15);
  *reasonp++ =  a1;
  goto format__special_5_31;
  };
  goto format__special_5_31;
 case (unsigned long) makeint(76L):
 format__special_5_18:
  switch (ptagof(a1)) {
 case CONS:
  x0 = cdr_of(a1);
  unify(a2, x0);
  a0 = car_of(a1);
  a1 = a3;
  a2 = a4;
  execute(format__special__L_3_0);
  goto format__special__L_3_ext_interrupt;
 case VARREF:
  deref_and_jump(a1,format__special_5_18);
  *reasonp++ =  a1;
  goto format__special_5_31;
  };
  goto format__special_5_31;
 case (unsigned long) makeint(110L):
  unify(a2, a1);
  allocp[0] = a4;
  allocp[1] = makeint(10L);
  x0 = makecons(&allocp[0]);
  allocp += 2;
  unify_value(a3, x0);
  proceed();
 case (unsigned long) makeint(112L):
 format__special_5_19:
  switch (ptagof(a1)) {
 case CONS:
  x0 = cdr_of(a1);
 format__special_5_20:
  switch (ptagof(x0)) {
 case CONS:
  x1 = cdr_of(x0);
 format__special_5_21:
  switch (ptagof(x1)) {
 case CONS:
  x2 = cdr_of(x1);
  unify(a2, x2);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__write_xwrite__pred__name_5);
  x3 = car_of(a1);
  allocp[2] = x3;
  x4 = car_of(x0);
  allocp[3] = x4;
  x5 = car_of(x1);
  allocp[4] = x5;
  allocp[5] = a3;
  allocp[6] = a4;
  qp = (struct goalrec*)&allocp[0];
  allocp += 7;
  proceed();
 case VARREF:
  deref_and_jump(x1,format__special_5_21);
  *reasonp++ =  x1;
  goto format__special_5_31;
  };
  goto format__special_5_31;
 case VARREF:
  deref_and_jump(x0,format__special_5_20);
  *reasonp++ =  x0;
  goto format__special_5_31;
  };
  goto format__special_5_31;
 case VARREF:
  deref_and_jump(a1,format__special_5_19);
  *reasonp++ =  a1;
  goto format__special_5_31;
  };
  goto format__special_5_31;
 case (unsigned long) makeint(113L):
 format__special_5_22:
  switch (ptagof(a1)) {
 case CONS:
  x0 = cdr_of(a1);
  unify(a2, x0);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__obj_xwrite__atom__name__string_3);
  allocp[2] = x1 = makeref(&allocp[2]);
  allocp[3] = a3;
  allocp[4] = a4;
  allocp[5] = (q)(struct goalrec*)&allocp[0];
  allocp[6] = (q)(&predicate_atom__table_xget__atom__string_2);
  x2 = car_of(a1);
  allocp[7] = x2;
  allocp[8] = x1;
  qp = (struct goalrec*)&allocp[5];
  allocp += 9;
  proceed();
 case VARREF:
  deref_and_jump(a1,format__special_5_22);
  *reasonp++ =  a1;
  goto format__special_5_31;
  };
  goto format__special_5_31;
 case (unsigned long) makeint(114L):
 format__special_5_23:
  switch (ptagof(a1)) {
 case CONS:
  x0 = cdr_of(a1);
  unify(a2, x0);
  a0 = car_of(a1);
  a1 = a3;
  a2 = a4;
  execute(write__reg_3_0);
  goto write__reg_3_ext_interrupt;
 case VARREF:
  deref_and_jump(a1,format__special_5_23);
  *reasonp++ =  a1;
  goto format__special_5_31;
  };
  goto format__special_5_31;
 case (unsigned long) makeint(82L):
 format__special_5_24:
  switch (ptagof(a1)) {
 case CONS:
  x0 = cdr_of(a1);
  unify(a2, x0);
  a0 = car_of(a1);
  a1 = a3;
  a2 = a4;
  execute(write__reg__int_3_0);
  goto write__reg__int_3_ext_interrupt;
 case VARREF:
  deref_and_jump(a1,format__special_5_24);
  *reasonp++ =  a1;
  goto format__special_5_31;
  };
  goto format__special_5_31;
 case (unsigned long) makeint(115L):
 format__special_5_25:
  switch (ptagof(a1)) {
 case CONS:
  x0 = cdr_of(a1);
  unify(a2, x0);
  allocp[0] = makesym(functor_fwrite_1);
  x2 = car_of(a1);
  allocp[1] = x2;
  x1 = makefunctor(&allocp[0]);
  allocp[2] = a4;
  allocp[3] = x1;
  x3 = makecons(&allocp[2]);
  allocp += 4;
  unify_value(a3, x3);
  proceed();
 case VARREF:
  deref_and_jump(a1,format__special_5_25);
  *reasonp++ =  a1;
  goto format__special_5_31;
  };
  goto format__special_5_31;
 case (unsigned long) makeint(83L):
 format__special_5_26:
  switch (ptagof(a1)) {
 case CONS:
  x0 = cdr_of(a1);
  unify(a2, x0);
  a0 = car_of(a1);
  a1 = a3;
  a2 = a4;
  execute(write__short_3_0);
  goto write__short_3_ext_interrupt;
 case VARREF:
  deref_and_jump(a1,format__special_5_26);
  *reasonp++ =  a1;
  goto format__special_5_31;
  };
  goto format__special_5_31;
 case (unsigned long) makeint(119L):
 format__special_5_27:
  switch (ptagof(a1)) {
 case CONS:
  x0 = cdr_of(a1);
  unify(a2, x0);
  a0 = car_of(a1);
  a1 = a3;
  a2 = a4;
  execute(format__special__w_3_0);
  goto format__special__w_3_ext_interrupt;
 case VARREF:
  deref_and_jump(a1,format__special_5_27);
  *reasonp++ =  a1;
  goto format__special_5_31;
  };
  goto format__special_5_31;
 case (unsigned long) makeint(87L):
 format__special_5_28:
  switch (ptagof(a1)) {
 case CONS:
  x0 = cdr_of(a1);
  unify(a2, x0);
  a0 = car_of(a1);
  a1 = a3;
  a2 = a4;
  execute(format__special__W_3_0);
  goto format__special__W_3_ext_interrupt;
 case VARREF:
  deref_and_jump(a1,format__special_5_28);
  *reasonp++ =  a1;
  goto format__special_5_31;
  };
  goto format__special_5_31;
 case (unsigned long) makeint(44L):
 format__special_5_29:
  switch (ptagof(a1)) {
 case CONS:
  x0 = cdr_of(a1);
  unify(a2, x0);
  a0 = car_of(a1);
  a1 = a3;
  a2 = a4;
  execute(write__params__blt_3_0);
  goto write__params__blt_3_ext_interrupt;
 case VARREF:
  deref_and_jump(a1,format__special_5_29);
  *reasonp++ =  a1;
  goto format__special_5_31;
  };
  goto format__special_5_31;
 case (unsigned long) makeint(81L):
 format__special_5_30:
  switch (ptagof(a1)) {
 case CONS:
  x0 = cdr_of(a1);
  unify(a2, x0);
  a0 = car_of(a1);
  a1 = a3;
  a2 = a4;
  execute(write__qp_3_0);
  goto write__qp_3_ext_interrupt;
 case VARREF:
  deref_and_jump(a1,format__special_5_30);
  *reasonp++ =  a1;
  goto format__special_5_31;
  };
  goto format__special_5_31;
 case (unsigned long) makeint(126L):
  unify(a2, a1);
  allocp[0] = a4;
  allocp[1] = makeint(126L);
  x0 = makecons(&allocp[0]);
  allocp += 2;
  unify_value(a3, x0);
  proceed();
  };
  goto format__special_5_31;
 case VARREF:
  deref_and_jump(a0,format__special_5_1);
  *reasonp++ =  a0;
  goto format__special_5_31;
  };
  goto format__special_5_31;
 format__special_5_31:
  otherwise(format__special_5_interrupt);
  unify2(a4, a3, a2, a1);
  allocp[0] = NILATOM;
  allocp[1] = a0;
  x0 = makecons(&allocp[0]);
  allocp[2] = (q)qp;
  allocp[3] = (q)(&predicate_klic__comp__message_xreport__error_2);
  allocp[4] = makefunctor(&string_const_93);
  allocp[5] = x0;
  qp = (struct goalrec*)&allocp[2];
  allocp += 6;
  proceed();
 format__special_5_ext_interrupt:
  reasonp = 0l;
 format__special_5_interrupt:
  toppred = &predicate_klic__comp__obj_xformat__special_5;
  goto interrupt_5;
 }

 format__special__i_3_top: {
  q x0, x1, x2, x3, x4, x5, x6, x7;
  a2 = qp->args[2];
  qp = qp->next;
 format__special__i_3_clear_reason:
  reasonp = reasons;
 format__special__i_3_0:
 format__special__i_3_1:
  switch (ptagof(a0)) {
 case ATOMIC:
  if (isint(a0)) goto format__special__i_3_4;
 format__special__i_3_2:
  gblt_atom(a0,format__special__i_3_4);
  execute(format__special__w_3_0);
  goto format__special__w_3_ext_interrupt;
 case FUNCTOR:
  if (functor_of(a0) != makesym(functor__2F_2)) goto format__special__i_3_4;
  x0 = arg(a0, 0);
 format__special__i_3_3:
  switch (ptagof(x0)) {
 case FUNCTOR:
  if (functor_of(x0) != makesym(functor__2F_2)) goto format__special__i_3_4;
  allocp[0] = NILATOM;
  x2 = arg(a0, 1);
  allocp[1] = x2;
  x1 = makecons(&allocp[0]);
  allocp[2] = x1;
  x4 = arg(x0, 1);
  allocp[3] = x4;
  x3 = makecons(&allocp[2]);
  allocp[4] = x3;
  x6 = arg(x0, 0);
  allocp[5] = x6;
  x5 = makecons(&allocp[4]);
  a0 = makefunctor(&string_const_98);
  x7 = a1;
  a3 = a2;
  a2 = x7;
  a1 = x5;
  allocp += 6;
  execute(klicformat_4_0);
  goto klicformat_4_ext_interrupt;
 case VARREF:
  deref_and_jump(x0,format__special__i_3_3);
  *reasonp++ =  x0;
  goto format__special__i_3_4;
  };
  goto format__special__i_3_4;
 case VARREF:
  deref_and_jump(a0,format__special__i_3_1);
  *reasonp++ =  a0;
  goto format__special__i_3_4;
  };
  goto format__special__i_3_4;
 format__special__i_3_4:
  otherwise(format__special__i_3_interrupt);
 format__special__i_3_5:
  switch (ptagof(a0)) {
 case FUNCTOR:
  if (functor_of(a0) != makesym(functor__2F_2)) goto format__special__i_3_interrupt;
  allocp[0] = NILATOM;
  x1 = arg(a0, 1);
  allocp[1] = x1;
  x0 = makecons(&allocp[0]);
  allocp[2] = x0;
  x3 = arg(a0, 0);
  allocp[3] = x3;
  x2 = makecons(&allocp[2]);
  a0 = makefunctor(&string_const_97);
  x4 = a1;
  a3 = a2;
  a2 = x4;
  a1 = x2;
  allocp += 4;
  execute(klicformat_4_0);
  goto klicformat_4_ext_interrupt;
 case VARREF:
  deref_and_jump(a0,format__special__i_3_5);
  *reasonp++ =  a0;
  goto format__special__i_3_interrupt;
  };
  goto format__special__i_3_interrupt;
 format__special__i_3_ext_interrupt:
  reasonp = 0l;
 format__special__i_3_interrupt:
  toppred = &predicate_klic__comp__obj_xformat__special__i_3;
  goto interrupt_3;
 }

 format__special__w_3_top: {
  q x0, x1, x2;
  a2 = qp->args[2];
  qp = qp->next;
 format__special__w_3_clear_reason:
  reasonp = reasons;
 format__special__w_3_0:
 format__special__w_3_1:
  switch (ptagof(a0)) {
 case ATOMIC:
  if (isint(a0)) goto format__special__w_3_3;
 format__special__w_3_2:
  gblt_atom(a0,format__special__w_3_3);
  allocp[0] = makesym(functor_fwrite_1);
  allocp[1] = x1 = makeref(&allocp[1]);
  x0 = makefunctor(&allocp[0]);
  allocp[2] = a2;
  allocp[3] = x0;
  x2 = makecons(&allocp[2]);
  allocp += 4;
  unify_value(a1, x2);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_atom__table_xget__atom__string_2);
  allocp[2] = a0;
  allocp[3] = x1;
  qp = (struct goalrec*)&allocp[0];
  allocp += 4;
  proceed();
 case VARREF:
  deref_and_jump(a0,format__special__w_3_1);
  *reasonp++ =  a0;
  goto format__special__w_3_3;
  };
  goto format__special__w_3_3;
 format__special__w_3_3:
  otherwise(format__special__w_3_interrupt);
  allocp[0] = makesym(functor_s_2);
  allocp[1] = makefunctor(funct_const_101);
  allocp[2] = x1 = makeref(&allocp[2]);
  x0 = makefunctor(&allocp[0]);
  allocp[3] = (q)qp;
  allocp[4] = (q)(&predicate_unparser_xunparse_4);
  allocp[5] = a0;
  allocp[6] = x0;
  allocp[7] = a1;
  allocp[8] = a2;
  qp = (struct goalrec*)&allocp[3];
  allocp += 9;
  proceed();
 format__special__w_3_ext_interrupt:
  reasonp = 0l;
 format__special__w_3_interrupt:
  toppred = &predicate_klic__comp__obj_xformat__special__w_3;
  goto interrupt_3;
 }

 format__special__W_3_top: {
  q x0, x1;
  a2 = qp->args[2];
  qp = qp->next;
 format__special__W_3_clear_reason:
  reasonp = reasons;
 format__special__W_3_0:
  allocp[0] = makesym(functor_s_2);
  allocp[1] = makefunctor(funct_const_101);
  allocp[2] = x1 = makeref(&allocp[2]);
  x0 = makefunctor(&allocp[0]);
  allocp[3] = (q)qp;
  allocp[4] = (q)(&predicate_unparser_xunwparse_4);
  allocp[5] = a0;
  allocp[6] = x0;
  allocp[7] = a1;
  allocp[8] = a2;
  qp = (struct goalrec*)&allocp[3];
  allocp += 9;
  proceed();
 format__special__W_3_ext_interrupt:
  reasonp = 0l;
 format__special__W_3_interrupt:
  toppred = &predicate_klic__comp__obj_xformat__special__W_3;
  goto interrupt_3;
 }

 format__special__D_4_top: {
  q x0, x1;
  a2 = qp->args[2];
  a3 = qp->args[3];
  qp = qp->next;
 format__special__D_4_clear_reason:
  reasonp = reasons;
 format__special__D_4_0:
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__obj_xklicformat_3);
  allocp[2] = makefunctor(&string_const_71);
  allocp[3] = x0 = makeref(&allocp[3]);
  allocp[4] = a3;
  allocp[5] = (q)(struct goalrec*)&allocp[0];
  allocp[6] = (q)(&predicate_klic__comp__obj_xformat__special__D__1_4);
  allocp[7] = a0;
  allocp[8] = a1;
  allocp[9] = x1 = makeref(&allocp[9]);
  allocp[10] = x0;
  a0 = makefunctor(&string_const_75);
  a1 = a2;
  a2 = x1;
  qp = (struct goalrec*)&allocp[5];
  allocp += 11;
  execute(klicformat_3_0);
  goto klicformat_3_ext_interrupt;
 format__special__D_4_ext_interrupt:
  reasonp = 0l;
 format__special__D_4_interrupt:
  toppred = &predicate_klic__comp__obj_xformat__special__D_4;
  goto interrupt_4;
 }

 format__special__D__1_4_top: {
  q x0, x1, x2, x3, x4, x5, x6, x7, x8, x9;
  a2 = qp->args[2];
  a3 = qp->args[3];
  qp = qp->next;
 format__special__D__1_4_clear_reason:
  reasonp = reasons;
 format__special__D__1_4_0:
 format__special__D__1_4_1:
  switch (ptagof(a0)) {
 case ATOMIC:
  if (!isint(a0)) goto format__special__D__1_4_7;
 format__special__D__1_4_2:
  gblt_integer(a0,format__special__D__1_4_7);
 format__special__D__1_4_3:
  if (!isint(a1)) goto format__special__D__1_4_4;
  gblt_integer(a1,format__special__D__1_4_7);
  bblt_sub_no_check(a1,a0,x0);
  a0 = x0;
  a1 = a2;
  a2 = a3;
  execute(write__int_3_0);
  goto write__int_3_ext_interrupt;
 format__special__D__1_4_4:
  if (!isref(a1)) goto format__special__D__1_4_7;
  deref_and_jump(a1,format__special__D__1_4_3);
  *reasonp++ =  a1;
  goto format__special__D__1_4_7;
 case FUNCTOR:
  if (functor_of(a0) != makesym(functor__2F_2)) goto format__special__D__1_4_7;
 format__special__D__1_4_5:
  switch (ptagof(a1)) {
 case ATOMIC:
  if (!isint(a1)) goto format__special__D__1_4_7;
 format__special__D__1_4_6:
  gblt_integer(a1,format__special__D__1_4_7);
  allocp[0] = NILATOM;
  allocp[1] = a1;
  x0 = makecons(&allocp[0]);
  allocp[2] = (q)qp;
  allocp[3] = (q)(&predicate_klic__comp__obj_xklicformat_4);
  allocp[4] = makefunctor(&string_const_73);
  allocp[5] = x0;
  allocp[6] = x1 = makeref(&allocp[6]);
  allocp[7] = a3;
  allocp[8] = makesym(functor__2F_2);
  x3 = arg(a0, 0);
  allocp[9] = x3;
  x4 = arg(a0, 1);
  allocp[10] = x4;
  x2 = makefunctor(&allocp[8]);
  allocp[11] = (q)(struct goalrec*)&allocp[2];
  allocp[12] = (q)(&predicate_klic__comp__obj_xformat__special__L_3);
  allocp[13] = x2;
  allocp[14] = x5 = makeref(&allocp[14]);
  allocp[15] = x1;
  a0 = makefunctor(&string_const_77);
  a1 = a2;
  a2 = x5;
  qp = (struct goalrec*)&allocp[11];
  allocp += 16;
  execute(klicformat_3_0);
  goto klicformat_3_ext_interrupt;
 case FUNCTOR:
  if (functor_of(a1) != makesym(functor__2F_2)) goto format__special__D__1_4_7;
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__obj_xklicformat_3);
  allocp[2] = makefunctor(&string_const_71);
  allocp[3] = x0 = makeref(&allocp[3]);
  allocp[4] = a3;
  allocp[5] = makesym(functor__2F_2);
  x2 = arg(a1, 0);
  allocp[6] = x2;
  x3 = arg(a1, 1);
  allocp[7] = x3;
  x1 = makefunctor(&allocp[5]);
  allocp[8] = (q)(struct goalrec*)&allocp[0];
  allocp[9] = (q)(&predicate_klic__comp__obj_xformat__special__L_3);
  allocp[10] = x1;
  allocp[11] = x4 = makeref(&allocp[11]);
  allocp[12] = x0;
  allocp[13] = (q)(struct goalrec*)&allocp[8];
  allocp[14] = (q)(&predicate_klic__comp__obj_xklicformat_3);
  allocp[15] = makefunctor(&string_const_72);
  allocp[16] = x5 = makeref(&allocp[16]);
  allocp[17] = x4;
  allocp[18] = makesym(functor__2F_2);
  x7 = arg(a0, 0);
  allocp[19] = x7;
  x8 = arg(a0, 1);
  allocp[20] = x8;
  x6 = makefunctor(&allocp[18]);
  allocp[21] = (q)(struct goalrec*)&allocp[13];
  allocp[22] = (q)(&predicate_klic__comp__obj_xformat__special__L_3);
  allocp[23] = x6;
  allocp[24] = x9 = makeref(&allocp[24]);
  allocp[25] = x5;
  a0 = makefunctor(&string_const_77);
  a1 = a2;
  a2 = x9;
  qp = (struct goalrec*)&allocp[21];
  allocp += 26;
  execute(klicformat_3_0);
  goto klicformat_3_ext_interrupt;
 case VARREF:
  deref_and_jump(a1,format__special__D__1_4_5);
  *reasonp++ =  a1;
  goto format__special__D__1_4_7;
  };
  goto format__special__D__1_4_7;
 case VARREF:
  deref_and_jump(a0,format__special__D__1_4_1);
  *reasonp++ =  a0;
  goto format__special__D__1_4_7;
  };
  goto format__special__D__1_4_7;
 format__special__D__1_4_7:
 format__special__D__1_4_8:
  switch (ptagof(a1)) {
 case FUNCTOR:
  if (functor_of(a1) != makesym(functor__2F_2)) goto format__special__D__1_4_interrupt;
 format__special__D__1_4_9:
  if (!isint(a0)) goto format__special__D__1_4_10;
  gblt_integer(a0,format__special__D__1_4_interrupt);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__obj_xklicformat_3);
  allocp[2] = makefunctor(&string_const_71);
  allocp[3] = x0 = makeref(&allocp[3]);
  allocp[4] = a3;
  allocp[5] = makesym(functor__2F_2);
  x2 = arg(a1, 0);
  allocp[6] = x2;
  x3 = arg(a1, 1);
  allocp[7] = x3;
  x1 = makefunctor(&allocp[5]);
  allocp[8] = (q)(struct goalrec*)&allocp[0];
  allocp[9] = (q)(&predicate_klic__comp__obj_xformat__special__L_3);
  allocp[10] = x1;
  allocp[11] = x4 = makeref(&allocp[11]);
  allocp[12] = x0;
  allocp[13] = NILATOM;
  allocp[14] = a0;
  x5 = makecons(&allocp[13]);
  a0 = makefunctor(&string_const_78);
  a1 = x5;
  a3 = x4;
  qp = (struct goalrec*)&allocp[8];
  allocp += 15;
  execute(klicformat_4_clear_reason);
  goto klicformat_4_ext_interrupt;
 format__special__D__1_4_10:
  if (!isref(a0)) goto format__special__D__1_4_interrupt;
  deref_and_jump(a0,format__special__D__1_4_9);
  *reasonp++ =  a0;
  goto format__special__D__1_4_interrupt;
 case VARREF:
  deref_and_jump(a1,format__special__D__1_4_8);
  *reasonp++ =  a1;
  goto format__special__D__1_4_interrupt;
  };
  goto format__special__D__1_4_interrupt;
 format__special__D__1_4_ext_interrupt:
  reasonp = 0l;
 format__special__D__1_4_interrupt:
  goto interrupt_4;
 }

 format__special__L_3_top: {
  q x0, x1, x2, x3, x4, x5, x6, x7;
  a2 = qp->args[2];
  qp = qp->next;
 format__special__L_3_clear_reason:
  reasonp = reasons;
 format__special__L_3_0:
 format__special__L_3_1:
  switch (ptagof(a0)) {
 case FUNCTOR:
  if (functor_of(a0) != makesym(functor__2F_2)) goto format__special__L_3_interrupt;
  x0 = arg(a0, 0);
 format__special__L_3_2:
  switch (ptagof(x0)) {
 case ATOMIC:
  if (isint(x0)) goto format__special__L_3_interrupt;
 format__special__L_3_3:
  gblt_atom(x0,format__special__L_3_interrupt);
  x1 = arg(a0, 1);
 format__special__L_3_4:
  if (!isint(x1)) goto format__special__L_3_5;
  gblt_integer(x1,format__special__L_3_interrupt);
  allocp[0] = makesym(functor__2F_2);
  allocp[1] = x0;
  allocp[2] = x1;
  x2 = makefunctor(&allocp[0]);
  allocp[3] = NILATOM;
  allocp[4] = x2;
  x3 = makecons(&allocp[3]);
  a0 = makefunctor(&string_const_82);
  x4 = a1;
  a3 = a2;
  a2 = x4;
  a1 = x3;
  allocp += 5;
  execute(klicformat_4_0);
  goto klicformat_4_ext_interrupt;
 format__special__L_3_5:
  if (!isref(x1)) goto format__special__L_3_interrupt;
  deref_and_jump(x1,format__special__L_3_4);
  *reasonp++ =  x1;
  goto format__special__L_3_interrupt;
 case FUNCTOR:
  if (functor_of(x0) != makesym(functor__2F_2)) goto format__special__L_3_interrupt;
  x1 = arg(a0, 1);
 format__special__L_3_6:
  switch (ptagof(x1)) {
 case ATOMIC:
  if (isint(x1)) goto format__special__L_3_8;
 format__special__L_3_7:
  gblt_atom(x1,format__special__L_3_interrupt);
  allocp[0] = makesym(functor__2F_2);
  x3 = arg(x0, 0);
  allocp[1] = x3;
  x4 = arg(x0, 1);
  allocp[2] = x4;
  x2 = makefunctor(&allocp[0]);
  allocp[3] = NILATOM;
  allocp[4] = x1;
  x5 = makecons(&allocp[3]);
  allocp[5] = x5;
  allocp[6] = x2;
  x6 = makecons(&allocp[5]);
  a0 = makefunctor(&string_const_84);
  x7 = a1;
  a3 = a2;
  a2 = x7;
  a1 = x6;
  allocp += 7;
  execute(klicformat_4_0);
  goto klicformat_4_ext_interrupt;
 format__special__L_3_8:
  gblt_integer(x1,format__special__L_3_interrupt);
  allocp[0] = makesym(functor__2F_2);
  x3 = arg(x0, 0);
  allocp[1] = x3;
  x4 = arg(x0, 1);
  allocp[2] = x4;
  x2 = makefunctor(&allocp[0]);
  allocp[3] = NILATOM;
  allocp[4] = x1;
  x5 = makecons(&allocp[3]);
  allocp[5] = x5;
  allocp[6] = x2;
  x6 = makecons(&allocp[5]);
  a0 = makefunctor(&string_const_83);
  x7 = a1;
  a3 = a2;
  a2 = x7;
  a1 = x6;
  allocp += 7;
  execute(klicformat_4_0);
  goto klicformat_4_ext_interrupt;
 case VARREF:
  deref_and_jump(x1,format__special__L_3_6);
  *reasonp++ =  x1;
  goto format__special__L_3_interrupt;
  };
  goto format__special__L_3_interrupt;
 case VARREF:
  deref_and_jump(x0,format__special__L_3_2);
  *reasonp++ =  x0;
  goto format__special__L_3_interrupt;
  };
  goto format__special__L_3_interrupt;
 case VARREF:
  deref_and_jump(a0,format__special__L_3_1);
  *reasonp++ =  a0;
  goto format__special__L_3_interrupt;
  };
  goto format__special__L_3_interrupt;
 format__special__L_3_ext_interrupt:
  reasonp = 0l;
 format__special__L_3_interrupt:
  toppred = &predicate_klic__comp__obj_xformat__special__L_3;
  goto interrupt_3;
 }

 write__params_3_top: {
  q x0, x1, x2, x3;
  a2 = qp->args[2];
  qp = qp->next;
 write__params_3_clear_reason:
  reasonp = reasons;
 write__params_3_0:
 write__params_3_1:
  switch (ptagof(a0)) {
 case CONS:
  x0 = cdr_of(a0);
 write__params_3_2:
  switch (ptagof(x0)) {
 case ATOMIC:
  if (x0 != NILATOM) goto write__params_3_3;
  a0 = car_of(a0);
  execute(write__param_3_0);
  goto write__param_3_ext_interrupt;
 case VARREF:
  deref_and_jump(x0,write__params_3_2);
  *reasonp++ =  x0;
  goto write__params_3_interrupt;
  };
  goto write__params_3_3;
 write__params_3_3:
  x1 = NILATOM;
  gblt_diff(x0,x1,write__params_3_interrupt);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__obj_xwrite__params_3);
  allocp[2] = x0;
  allocp[3] = x2 = makeref(&allocp[3]);
  allocp[4] = a2;
  allocp[5] = x2;
  allocp[6] = makeint(44L);
  x3 = makecons(&allocp[5]);
  a0 = car_of(a0);
  a2 = x3;
  qp = (struct goalrec*)&allocp[0];
  allocp += 7;
  execute(write__param_3_0);
  goto write__param_3_ext_interrupt;
 case ATOMIC:
  if (a0 != NILATOM) goto write__params_3_interrupt;
  unify(a2, a1);
  proceed();
 case VARREF:
  deref_and_jump(a0,write__params_3_1);
  *reasonp++ =  a0;
  goto write__params_3_interrupt;
  };
  goto write__params_3_interrupt;
 write__params_3_ext_interrupt:
  reasonp = 0l;
 write__params_3_interrupt:
  goto interrupt_3;
 }

 write__param_3_top: {
  q x0, x1, x2, x3;
  a2 = qp->args[2];
  qp = qp->next;
 write__param_3_clear_reason:
  reasonp = reasons;
 write__param_3_0:
 write__param_3_1:
  switch (ptagof(a0)) {
 case FUNCTOR:
  switch (symval(functor_of(a0))) {
 case functor_int_1:
  allocp[0] = NILATOM;
  x1 = arg(a0, 0);
  allocp[1] = x1;
  x0 = makecons(&allocp[0]);
  a0 = makefunctor(&string_const_90);
  x2 = a1;
  a3 = a2;
  a2 = x2;
  a1 = x0;
  allocp += 2;
  execute(klicformat_4_0);
  goto klicformat_4_ext_interrupt;
 case functor_atom_1:
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__write_xwrite__atom_3);
  x0 = arg(a0, 0);
  allocp[2] = x0;
  allocp[3] = a1;
  allocp[4] = a2;
  qp = (struct goalrec*)&allocp[0];
  allocp += 5;
  proceed();
 case functor_const_1:
  x0 = arg(a0, 0);
 write__param_3_2:
  switch (ptagof(x0)) {
 case FUNCTOR:
  switch (symval(functor_of(x0))) {
 case functor_float_1:
  allocp[0] = NILATOM;
  x2 = arg(x0, 0);
  allocp[1] = x2;
  x1 = makecons(&allocp[0]);
  a0 = makefunctor(&string_const_86);
  x3 = a1;
  a3 = a2;
  a2 = x3;
  a1 = x1;
  allocp += 2;
  execute(klicformat_4_0);
  goto klicformat_4_ext_interrupt;
 case functor_predicate_1:
  allocp[0] = NILATOM;
  x2 = arg(x0, 0);
  allocp[1] = x2;
  x1 = makecons(&allocp[0]);
  a0 = makefunctor(&string_const_91);
  x3 = a1;
  a3 = a2;
  a2 = x3;
  a1 = x1;
  allocp += 2;
  execute(klicformat_4_0);
  goto klicformat_4_ext_interrupt;
 case functor_list_1:
  allocp[0] = NILATOM;
  x2 = arg(x0, 0);
  allocp[1] = x2;
  x1 = makecons(&allocp[0]);
  a0 = makefunctor(&string_const_85);
  x3 = a1;
  a3 = a2;
  a2 = x3;
  a1 = x1;
  allocp += 2;
  execute(klicformat_4_0);
  goto klicformat_4_ext_interrupt;
 case functor_functor_1:
  allocp[0] = NILATOM;
  x2 = arg(x0, 0);
  allocp[1] = x2;
  x1 = makecons(&allocp[0]);
  a0 = makefunctor(&string_const_89);
  x3 = a1;
  a3 = a2;
  a2 = x3;
  a1 = x1;
  allocp += 2;
  execute(klicformat_4_0);
  goto klicformat_4_ext_interrupt;
 case functor_string_1:
  allocp[0] = NILATOM;
  x2 = arg(x0, 0);
  allocp[1] = x2;
  x1 = makecons(&allocp[0]);
  a0 = makefunctor(&string_const_87);
  x3 = a1;
  a3 = a2;
  a2 = x3;
  a1 = x1;
  allocp += 2;
  execute(klicformat_4_0);
  goto klicformat_4_ext_interrupt;
 case functor_vector_1:
  allocp[0] = NILATOM;
  x2 = arg(x0, 0);
  allocp[1] = x2;
  x1 = makecons(&allocp[0]);
  a0 = makefunctor(&string_const_88);
  x3 = a1;
  a3 = a2;
  a2 = x3;
  a1 = x1;
  allocp += 2;
  execute(klicformat_4_0);
  goto klicformat_4_ext_interrupt;
  };
  goto write__param_3_interrupt;
 case VARREF:
  deref_and_jump(x0,write__param_3_2);
  *reasonp++ =  x0;
  goto write__param_3_interrupt;
  };
  goto write__param_3_interrupt;
 case functor_r_1:
  a0 = arg(a0, 0);
  execute(write__reg_3_0);
  goto write__reg_3_ext_interrupt;
 case functor_arg_2:
  x0 = arg(a0, 0);
  x1 = arg(a0, 1);
  x2 = a1;
  a3 = a2;
  a2 = x2;
  a1 = x1;
  a0 = x0;
  execute(write__elem__access_4_0);
  goto write__elem__access_4_ext_interrupt;
  };
  goto write__param_3_interrupt;
 case VARREF:
  deref_and_jump(a0,write__param_3_1);
  *reasonp++ =  a0;
  goto write__param_3_interrupt;
  };
  goto write__param_3_interrupt;
 write__param_3_ext_interrupt:
  reasonp = 0l;
 write__param_3_interrupt:
  toppred = &predicate_klic__comp__obj_xwrite__param_3;
  goto interrupt_3;
 }

 write__params__blt_3_top: {
  q x0, x1, x2, x3;
  a2 = qp->args[2];
  qp = qp->next;
 write__params__blt_3_clear_reason:
  reasonp = reasons;
 write__params__blt_3_0:
 write__params__blt_3_1:
  switch (ptagof(a0)) {
 case CONS:
  x0 = cdr_of(a0);
 write__params__blt_3_2:
  switch (ptagof(x0)) {
 case ATOMIC:
  if (x0 != NILATOM) goto write__params__blt_3_3;
  a0 = car_of(a0);
  execute(write__param_3_0);
  goto write__param_3_ext_interrupt;
 case VARREF:
  deref_and_jump(x0,write__params__blt_3_2);
  *reasonp++ =  x0;
  goto write__params__blt_3_interrupt;
  };
  goto write__params__blt_3_3;
 write__params__blt_3_3:
  x1 = NILATOM;
  gblt_diff(x0,x1,write__params__blt_3_interrupt);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__obj_xwrite__params_3);
  allocp[2] = x0;
  allocp[3] = x2 = makeref(&allocp[3]);
  allocp[4] = a2;
  allocp[5] = x2;
  allocp[6] = makeint(44L);
  x3 = makecons(&allocp[5]);
  a0 = car_of(a0);
  a2 = x3;
  qp = (struct goalrec*)&allocp[0];
  allocp += 7;
  execute(write__param__blt_3_0);
  goto write__param__blt_3_ext_interrupt;
 case ATOMIC:
  if (a0 != NILATOM) goto write__params__blt_3_interrupt;
  unify(a2, a1);
  proceed();
 case VARREF:
  deref_and_jump(a0,write__params__blt_3_1);
  *reasonp++ =  a0;
  goto write__params__blt_3_interrupt;
  };
  goto write__params__blt_3_interrupt;
 write__params__blt_3_ext_interrupt:
  reasonp = 0l;
 write__params__blt_3_interrupt:
  toppred = &predicate_klic__comp__obj_xwrite__params__blt_3;
  goto interrupt_3;
 }

 write__param__blt_3_top: {
  a2 = qp->args[2];
  qp = qp->next;
 write__param__blt_3_clear_reason:
  reasonp = reasons;
 write__param__blt_3_0:
 write__param__blt_3_1:
  switch (ptagof(a0)) {
 case FUNCTOR:
  if (functor_of(a0) != makesym(functor_r_1)) goto write__param__blt_3_interrupt;
  a0 = arg(a0, 0);
  execute(write__reg_3_0);
  goto write__reg_3_ext_interrupt;
 case VARREF:
  deref_and_jump(a0,write__param__blt_3_1);
  *reasonp++ =  a0;
  goto write__param__blt_3_interrupt;
  };
  goto write__param__blt_3_interrupt;
 write__param__blt_3_ext_interrupt:
  reasonp = 0l;
 write__param__blt_3_interrupt:
  toppred = &predicate_klic__comp__obj_xwrite__param__blt_3;
  goto interrupt_3;
 }

 write__int_3_top: {
  q x0, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13, x14, x15;
  a2 = qp->args[2];
  qp = qp->next;
 write__int_3_clear_reason:
  reasonp = reasons;
 write__int_3_0:
 write__int_3_1:
  if (!isint(a0)) goto write__int_3_5;
  x0 = makeint(0L);
  gblt_greater_or_eq(a0,x0,write__int_3_2);
  x1 = makeint(10L);
  gblt_less(a0,x1,write__int_3_2);
  x2 = makeint(48L);
  bblt_add_no_check(x2,a0,x3);
  allocp[0] = a2;
  allocp[1] = x3;
  x4 = makecons(&allocp[0]);
  allocp += 2;
  unify_value(a1, x4);
  proceed();
 write__int_3_2:
  x1 = makeint(10L);
  gblt_greater_or_eq(a0,x1,write__int_3_3);
  x2 = makeint(10L);
  bblt_mod_no_check(a0,x2,x3);
  x4 = makeint(48L);
  bblt_add_no_check(x4,x3,x5);
  x6 = makeint(10L);
  bblt_div_no_check(a0,x6,x7);
  allocp[0] = a2;
  allocp[1] = x5;
  x8 = makecons(&allocp[0]);
  a0 = x7;
  a2 = x8;
  allocp += 2;
  execute(write__int_3_0);
  goto write__int_3_ext_interrupt;
 write__int_3_3:
  x2 = makeint(0L);
  gblt_less(a0,x2,write__int_3_interrupt);
  gblt_minus(a0,x3,write__int_3_interrupt);
  gblt_eq(a0,x3,write__int_3_4);
  allocp[0] = x5 = makeref(&allocp[0]);
  allocp[1] = makeint(45L);
  x4 = makecons(&allocp[0]);
  allocp += 2;
  unify_value(a1, x4);
  x6 = makeint(10L);
  bblt_div_no_check(a0,x6,x7);
  bblt_minus_no_check(x7,x8);
  x9 = makeint(10L);
  bblt_mult_no_check(x8,x9,x10);
  bblt_add_no_check(a0,x10,x11);
  bblt_minus_no_check(x11,x12);
  x13 = makeint(48L);
  bblt_add_no_check(x13,x12,x14);
  allocp[0] = a2;
  allocp[1] = x14;
  x15 = makecons(&allocp[0]);
  a0 = x8;
  a1 = x5;
  a2 = x15;
  allocp += 2;
  execute(write__int_3_0);
  goto write__int_3_ext_interrupt;
 write__int_3_4:
  gblt_not_eq(a0,x3,write__int_3_interrupt);
  allocp[0] = x5 = makeref(&allocp[0]);
  allocp[1] = makeint(45L);
  x4 = makecons(&allocp[0]);
  allocp += 2;
  unify_value(a1, x4);
  a0 = x3;
  a1 = x5;
  execute(write__int_3_0);
  goto write__int_3_ext_interrupt;
 write__int_3_5:
  if (!isref(a0)) goto write__int_3_interrupt;
  deref_and_jump(a0,write__int_3_1);
  *reasonp++ =  a0;
  goto write__int_3_interrupt;
 write__int_3_ext_interrupt:
  reasonp = 0l;
 write__int_3_interrupt:
  toppred = &predicate_klic__comp__obj_xwrite__int_3;
  goto interrupt_3;
 }

 write__elem__access_4_top: {
  q x0, x1;
  a2 = qp->args[2];
  a3 = qp->args[3];
  qp = qp->next;
 write__elem__access_4_clear_reason:
  reasonp = reasons;
 write__elem__access_4_0:
 write__elem__access_4_1:
  switch (ptagof(a1)) {
 case ATOMIC:
  if (isint(a1)) goto write__elem__access_4_2;
  switch (symval(a1)) {
 case symval(makesym(atom_car)):
  allocp[0] = NILATOM;
  allocp[1] = a0;
  x0 = makecons(&allocp[0]);
  a0 = makefunctor(&string_const_80);
  a1 = x0;
  allocp += 2;
  execute(klicformat_4_0);
  goto klicformat_4_ext_interrupt;
 case symval(makesym(atom_cdr)):
  allocp[0] = NILATOM;
  allocp[1] = a0;
  x0 = makecons(&allocp[0]);
  a0 = makefunctor(&string_const_81);
  a1 = x0;
  allocp += 2;
  execute(klicformat_4_0);
  goto klicformat_4_ext_interrupt;
  };
  goto write__elem__access_4_interrupt;
 write__elem__access_4_2:
  gblt_integer(a1,write__elem__access_4_interrupt);
  allocp[0] = NILATOM;
  allocp[1] = a1;
  x0 = makecons(&allocp[0]);
  allocp[2] = x0;
  allocp[3] = a0;
  x1 = makecons(&allocp[2]);
  a0 = makefunctor(&string_const_79);
  a1 = x1;
  allocp += 4;
  execute(klicformat_4_0);
  goto klicformat_4_ext_interrupt;
 case VARREF:
  deref_and_jump(a1,write__elem__access_4_1);
  *reasonp++ =  a1;
  goto write__elem__access_4_interrupt;
  };
  goto write__elem__access_4_interrupt;
 write__elem__access_4_ext_interrupt:
  reasonp = 0l;
 write__elem__access_4_interrupt:
  toppred = &predicate_klic__comp__obj_xwrite__elem__access_4;
  goto interrupt_4;
 }

 write__reg_3_top: {
  q x0, x1;
  a2 = qp->args[2];
  qp = qp->next;
 write__reg_3_clear_reason:
  reasonp = reasons;
 write__reg_3_0:
 write__reg_3_1:
  switch (ptagof(a0)) {
 case FUNCTOR:
  switch (symval(functor_of(a0))) {
 case functor_a_1:
  allocp[0] = x1 = makeref(&allocp[0]);
  allocp[1] = makeint(97L);
  x0 = makecons(&allocp[0]);
  allocp += 2;
  unify_value(a1, x0);
  a0 = arg(a0, 0);
  a1 = x1;
  execute(write__int_3_0);
  goto write__int_3_ext_interrupt;
 case functor_x_1:
  allocp[0] = x1 = makeref(&allocp[0]);
  allocp[1] = makeint(120L);
  x0 = makecons(&allocp[0]);
  allocp += 2;
  unify_value(a1, x0);
  a0 = arg(a0, 0);
  a1 = x1;
  execute(write__int_3_0);
  goto write__int_3_ext_interrupt;
 case functor_r_1:
  a0 = arg(a0, 0);
  execute(write__reg_3_0);
  goto write__reg_3_ext_interrupt;
  };
  goto write__reg_3_interrupt;
 case VARREF:
  deref_and_jump(a0,write__reg_3_1);
  *reasonp++ =  a0;
  goto write__reg_3_interrupt;
  };
  goto write__reg_3_interrupt;
 write__reg_3_ext_interrupt:
  reasonp = 0l;
 write__reg_3_interrupt:
  toppred = &predicate_klic__comp__obj_xwrite__reg_3;
  goto interrupt_3;
 }

 write__reg__int_3_top: {
  q x0, x1, x2;
  a2 = qp->args[2];
  qp = qp->next;
 write__reg__int_3_clear_reason:
  reasonp = reasons;
 write__reg__int_3_0:
 write__reg__int_3_1:
  switch (ptagof(a0)) {
 case FUNCTOR:
  switch (symval(functor_of(a0))) {
 case functor_a_1:
  a0 = arg(a0, 0);
  execute(write__int_3_0);
  goto write__int_3_ext_interrupt;
 case functor_x_1:
  x0 = arg(a0, 0);
  x1 = makeint(128L);
  bblt_add_no_check_y(x0,x1,x2);
  a0 = x2;
  execute(write__int_3_0);
  goto write__int_3_ext_interrupt;
 case functor_r_1:
  a0 = arg(a0, 0);
  execute(write__reg__int_3_0);
  goto write__reg__int_3_ext_interrupt;
  };
  goto write__reg__int_3_interrupt;
 case VARREF:
  deref_and_jump(a0,write__reg__int_3_1);
  *reasonp++ =  a0;
  goto write__reg__int_3_interrupt;
  };
  goto write__reg__int_3_interrupt;
 write__reg__int_3_ext_interrupt:
  reasonp = 0l;
 write__reg__int_3_interrupt:
  toppred = &predicate_klic__comp__obj_xwrite__reg__int_3;
  goto interrupt_3;
 }

 write__short_3_top: {
  q x0, x1;
  a2 = qp->args[2];
  qp = qp->next;
 write__short_3_clear_reason:
  reasonp = reasons;
 write__short_3_0:
 write__short_3_1:
  if (isref(a0)) goto write__short_3_2;
  gblt_wait(a0,write__short_3_interrupt);
  allocp[0] = NILATOM;
  allocp[1] = a0;
  x0 = makecons(&allocp[0]);
  a0 = makefunctor(&string_const_76);
  x1 = a1;
  a3 = a2;
  a2 = x1;
  a1 = x0;
  allocp += 2;
  execute(klicformat_4_0);
  goto klicformat_4_ext_interrupt;
 write__short_3_2:
  deref_and_jump(a0,write__short_3_1);
  *reasonp++ =  a0;
  goto write__short_3_interrupt;
 write__short_3_ext_interrupt:
  reasonp = 0l;
 write__short_3_interrupt:
  toppred = &predicate_klic__comp__obj_xwrite__short_3;
  goto interrupt_3;
 }

 write__qp_3_top: {
  q x0, x1;
  a2 = qp->args[2];
  qp = qp->next;
 write__qp_3_clear_reason:
  reasonp = reasons;
 write__qp_3_0:
 write__qp_3_1:
  switch (ptagof(a0)) {
 case ATOMIC:
  if (isint(a0)) goto write__qp_3_2;
  if (a0 != makesym(atom_qp)) goto write__qp_3_interrupt;
  a0 = makefunctor(&string_const_92);
  execute(klicformat_3_0);
  goto klicformat_3_ext_interrupt;
 write__qp_3_2:
  gblt_integer(a0,write__qp_3_interrupt);
  allocp[0] = NILATOM;
  allocp[1] = a0;
  x0 = makecons(&allocp[0]);
  a0 = makefunctor(&string_const_70);
  x1 = a1;
  a3 = a2;
  a2 = x1;
  a1 = x0;
  allocp += 2;
  execute(klicformat_4_0);
  goto klicformat_4_ext_interrupt;
 case VARREF:
  deref_and_jump(a0,write__qp_3_1);
  *reasonp++ =  a0;
  goto write__qp_3_interrupt;
  };
  goto write__qp_3_interrupt;
 write__qp_3_ext_interrupt:
  reasonp = 0l;
 write__qp_3_interrupt:
  toppred = &predicate_klic__comp__obj_xwrite__qp_3;
  goto interrupt_3;
 }

 write__inline_6_top: {
  q x0, x1, x2, x3, x4, x5;
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  qp = qp->next;
 write__inline_6_clear_reason:
  reasonp = reasons;
 write__inline_6_0:
 write__inline_6_1:
  switch (ptagof(a0)) {
 case FUNCTOR:
  if (functor_of(a0) != makesym(functor_string_1)) goto write__inline_6_interrupt;
  x0 = arg(a0, 0);
 write__inline_6_2:
  switch (ptagof(x0)) {
 case FUNCTOR:
 write__inline_6_3:
  if (!isgobj(x0)) goto write__inline_6_interrupt;
 write__inline_6_4:
  gblt_is_string(x0,write__inline_6_interrupt);
  gblt_size_of_string(x0,x1,write__inline_6_interrupt);
  gblt_element_size_of_string(x0,x2,write__inline_6_interrupt);
 write__inline_6_5:
  if (x2 != makeint(8L)) goto write__inline_6_interrupt;
  a0 = makeint(0L);
  x3 = a1;
  x4 = a2;
  x5 = a3;
  a6 = a4;
  a7 = a5;
  a5 = x5;
  a4 = x4;
  a3 = x3;
  a2 = x0;
  a1 = x1;
  execute(write__inline_8_0);
  goto write__inline_8_ext_interrupt;
 case VARREF:
  deref_and_jump(x0,write__inline_6_2);
  *reasonp++ =  x0;
  goto write__inline_6_interrupt;
  };
  goto write__inline_6_interrupt;
 case VARREF:
  deref_and_jump(a0,write__inline_6_1);
  *reasonp++ =  a0;
  goto write__inline_6_interrupt;
  };
  goto write__inline_6_interrupt;
 write__inline_6_ext_interrupt:
  reasonp = 0l;
 write__inline_6_interrupt:
  goto interrupt_6;
 }

 write__inline_8_top: {
  q x0, x1, x2, x3, x4, x5, x6, x7, x8;
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  a6 = qp->args[6];
  a7 = qp->args[7];
  qp = qp->next;
 write__inline_8_clear_reason:
  reasonp = reasons;
 write__inline_8_0:
 write__inline_8_1:
  if (!isint(a0)) goto write__inline_8_9;
 write__inline_8_2:
  if (!isint(a1)) goto write__inline_8_8;
  gblt_greater_or_eq(a0,a1,write__inline_8_3);
  allocp[0] = a7;
  allocp[1] = makeint(10L);
  x0 = makecons(&allocp[0]);
  allocp += 2;
  unify_value(a6, x0);
  proceed();
 write__inline_8_3:
  gblt_less(a0,a1,write__inline_8_interrupt);
 write__inline_8_4:
  switch (ptagof(a2)) {
 case FUNCTOR:
 write__inline_8_5:
  if (!isgobj(a2)) goto write__inline_8_interrupt;
 write__inline_8_6:
  gblt_is_string(a2,write__inline_8_interrupt);
  gblt_element_of_string(a2,a0,x0,write__inline_8_interrupt);
  x2 = makeint(1L);
  gblt_add(a0,x2,x1,write__inline_8_interrupt);
  x3 = makeint(37L);
  gblt_not_eq(x0,x3,write__inline_8_7);
  allocp[0] = x5 = makeref(&allocp[0]);
  allocp[1] = x0;
  x4 = makecons(&allocp[0]);
  allocp += 2;
  unify_value(a6, x4);
  a0 = x1;
  a6 = x5;
  execute(write__inline_8_0);
  goto write__inline_8_ext_interrupt;
 write__inline_8_7:
  x4 = makeint(37L);
  gblt_eq(x0,x4,write__inline_8_interrupt);
  gblt_is_string(a2,write__inline_8_interrupt);
  gblt_element_of_string(a2,x1,x5,write__inline_8_interrupt);
  x6 = makeint(1L);
  bblt_add_no_check(x1,x6,x7);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__obj_xwrite__inline_8);
  allocp[2] = x7;
  allocp[3] = a1;
  allocp[4] = a2;
  allocp[5] = a3;
  allocp[6] = a4;
  allocp[7] = a5;
  allocp[8] = x8 = makeref(&allocp[8]);
  allocp[9] = a7;
  a0 = x5;
  a1 = a3;
  a2 = a4;
  a3 = a5;
  a4 = a6;
  a5 = x8;
  qp = (struct goalrec*)&allocp[0];
  allocp += 10;
  execute(inline__special_6_0);
  goto inline__special_6_ext_interrupt;
 case VARREF:
  deref_and_jump(a2,write__inline_8_4);
  *reasonp++ =  a2;
  goto write__inline_8_interrupt;
  };
  goto write__inline_8_interrupt;
 write__inline_8_8:
  if (!isref(a1)) goto write__inline_8_interrupt;
  deref_and_jump(a1,write__inline_8_2);
  *reasonp++ =  a1;
  goto write__inline_8_interrupt;
 write__inline_8_9:
  if (!isref(a0)) goto write__inline_8_interrupt;
  deref_and_jump(a0,write__inline_8_1);
  *reasonp++ =  a0;
  goto write__inline_8_interrupt;
 write__inline_8_ext_interrupt:
  reasonp = 0l;
 write__inline_8_interrupt:
  toppred = &predicate_klic__comp__obj_xwrite__inline_8;
  goto interrupt_8;
 }

 inline__special_6_top: {
  q x0, x1, x2, x3, x4, x5, x6, x7;
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  qp = qp->next;
 inline__special_6_clear_reason:
  reasonp = reasons;
 inline__special_6_0:
 inline__special_6_1:
  if (!isint(a0)) goto inline__special_6_4;
  x0 = makeint(37L);
  gblt_eq(a0,x0,inline__special_6_2);
  allocp[0] = a5;
  allocp[1] = a0;
  x1 = makecons(&allocp[0]);
  allocp += 2;
  unify_value(a4, x1);
  proceed();
 inline__special_6_2:
  x1 = makeint(102L);
  gblt_eq(a0,x1,inline__special_6_3);
  allocp[0] = NILATOM;
  allocp[1] = a3;
  x2 = makecons(&allocp[0]);
  allocp[2] = x2;
  allocp[3] = a2;
  x3 = makecons(&allocp[2]);
  a0 = makefunctor(&string_const_94);
  a1 = x3;
  a2 = a4;
  a3 = a5;
  allocp += 4;
  execute(klicformat_4_0);
  goto klicformat_4_ext_interrupt;
 inline__special_6_3:
  x2 = makeint(48L);
  gblt_less_or_eq(x2,a0,inline__special_6_5);
  x3 = makeint(57L);
  gblt_less_or_eq(a0,x3,inline__special_6_5);
  x4 = makeint(48L);
  bblt_sub_no_check(a0,x4,x5);
  allocp[0] = NILATOM;
  allocp[1] = x7 = makeref(&allocp[1]);
  x6 = makecons(&allocp[0]);
  allocp[2] = (q)qp;
  allocp[3] = (q)(&predicate_klic__comp__obj_xklicformat_4);
  allocp[4] = makefunctor(&string_const_96);
  allocp[5] = x6;
  allocp[6] = a4;
  allocp[7] = a5;
  allocp[8] = (q)(struct goalrec*)&allocp[2];
  allocp[9] = (q)(&predicate_klic__comp__util_xnth_3);
  allocp[10] = x5;
  allocp[11] = a1;
  allocp[12] = x7;
  qp = (struct goalrec*)&allocp[8];
  allocp += 13;
  proceed();
 inline__special_6_4:
  if (!isref(a0)) goto inline__special_6_5;
  deref_and_jump(a0,inline__special_6_1);
  *reasonp++ =  a0;
  goto inline__special_6_5;
 inline__special_6_5:
  otherwise(inline__special_6_interrupt);
  allocp[0] = x1 = makeref(&allocp[0]);
  allocp[1] = makeint(37L);
  x0 = makecons(&allocp[0]);
  allocp += 2;
  unify_value(a4, x0);
  allocp[0] = a5;
  allocp[1] = a0;
  x2 = makecons(&allocp[0]);
  allocp += 2;
  unify_value(x1, x2);
  proceed();
 inline__special_6_ext_interrupt:
  reasonp = 0l;
 inline__special_6_interrupt:
  toppred = &predicate_klic__comp__obj_xinline__special_6;
  goto interrupt_6;
 }

 write__atom__name__string_3_top: {
  q x0, x1, x2;
  a2 = qp->args[2];
  qp = qp->next;
 write__atom__name__string_3_clear_reason:
  reasonp = reasons;
 write__atom__name__string_3_0:
 write__atom__name__string_3_1:
  switch (ptagof(a0)) {
 case FUNCTOR:
 write__atom__name__string_3_2:
  if (!isgobj(a0)) goto write__atom__name__string_3_interrupt;
 write__atom__name__string_3_3:
  gblt_is_string(a0,write__atom__name__string_3_interrupt);
  gblt_size_of_string(a0,x0,write__atom__name__string_3_interrupt);
  gblt_element_size_of_string(a0,x1,write__atom__name__string_3_interrupt);
  x2 = a0;
  a3 = a1;
  a4 = a2;
  a2 = x2;
  a1 = x0;
  a0 = makeint(0L);
  execute(write__atom__name__string_5_0);
  goto write__atom__name__string_5_ext_interrupt;
 case VARREF:
  deref_and_jump(a0,write__atom__name__string_3_1);
  *reasonp++ =  a0;
  goto write__atom__name__string_3_interrupt;
  };
  goto write__atom__name__string_3_interrupt;
 write__atom__name__string_3_ext_interrupt:
  reasonp = 0l;
 write__atom__name__string_3_interrupt:
  goto interrupt_3;
 }

 write__atom__name__string_5_top: {
  q x0, x1, x2, x3;
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  qp = qp->next;
 write__atom__name__string_5_clear_reason:
  reasonp = reasons;
 write__atom__name__string_5_0:
 write__atom__name__string_5_1:
  if (!isint(a0)) goto write__atom__name__string_5_8;
 write__atom__name__string_5_2:
  if (!isint(a1)) goto write__atom__name__string_5_7;
  gblt_greater_or_eq(a0,a1,write__atom__name__string_5_3);
  unify(a4, a3);
  proceed();
 write__atom__name__string_5_3:
  gblt_less(a0,a1,write__atom__name__string_5_interrupt);
  x1 = makeint(1L);
  gblt_add(a0,x1,x0,write__atom__name__string_5_interrupt);
 write__atom__name__string_5_4:
  switch (ptagof(a2)) {
 case FUNCTOR:
 write__atom__name__string_5_5:
  if (!isgobj(a2)) goto write__atom__name__string_5_interrupt;
 write__atom__name__string_5_6:
  gblt_is_string(a2,write__atom__name__string_5_interrupt);
  gblt_element_of_string(a2,a0,x2,write__atom__name__string_5_interrupt);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__obj_xwrite__atom__name__string_5);
  allocp[2] = x0;
  allocp[3] = a1;
  allocp[4] = a2;
  allocp[5] = x3 = makeref(&allocp[5]);
  allocp[6] = a4;
  a0 = x2;
  a1 = a3;
  a2 = x3;
  qp = (struct goalrec*)&allocp[0];
  allocp += 7;
  execute(write__atom__name__char_3_0);
  goto write__atom__name__char_3_ext_interrupt;
 case VARREF:
  deref_and_jump(a2,write__atom__name__string_5_4);
  *reasonp++ =  a2;
  goto write__atom__name__string_5_interrupt;
  };
  goto write__atom__name__string_5_interrupt;
 write__atom__name__string_5_7:
  if (!isref(a1)) goto write__atom__name__string_5_interrupt;
  deref_and_jump(a1,write__atom__name__string_5_2);
  *reasonp++ =  a1;
  goto write__atom__name__string_5_interrupt;
 write__atom__name__string_5_8:
  if (!isref(a0)) goto write__atom__name__string_5_interrupt;
  deref_and_jump(a0,write__atom__name__string_5_1);
  *reasonp++ =  a0;
  goto write__atom__name__string_5_interrupt;
 write__atom__name__string_5_ext_interrupt:
  reasonp = 0l;
 write__atom__name__string_5_interrupt:
  toppred = &predicate_klic__comp__obj_xwrite__atom__name__string_5;
  goto interrupt_5;
 }

 write__atom__name__char_3_top: {
  q x0, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11;
  a2 = qp->args[2];
  qp = qp->next;
 write__atom__name__char_3_clear_reason:
  reasonp = reasons;
 write__atom__name__char_3_0:
 write__atom__name__char_3_1:
  if (!isint(a0)) goto write__atom__name__char_3_2;
  
{
  long c = intval(a0);
  if (!isalnum(c)) goto write__atom__name__char_3_3;
}
  allocp[0] = a2;
  allocp[1] = a0;
  x0 = makecons(&allocp[0]);
  allocp += 2;
  unify_value(a1, x0);
  proceed();
 write__atom__name__char_3_2:
  if (!isref(a0)) goto write__atom__name__char_3_3;
  deref_and_jump(a0,write__atom__name__char_3_1);
  *reasonp++ =  a0;
  goto write__atom__name__char_3_3;
 write__atom__name__char_3_3:
  otherwise(write__atom__name__char_3_interrupt);
 write__atom__name__char_3_4:
  if (!isint(a0)) goto write__atom__name__char_3_6;
  x0 = makeint(95L);
  gblt_eq(a0,x0,write__atom__name__char_3_5);
  allocp[0] = x2 = makeref(&allocp[0]);
  allocp[1] = a0;
  x1 = makecons(&allocp[0]);
  allocp += 2;
  unify_value(a1, x1);
  allocp[0] = a2;
  allocp[1] = a0;
  x3 = makecons(&allocp[0]);
  allocp += 2;
  unify_value(x2, x3);
  proceed();
 write__atom__name__char_3_5:
  x1 = makeint(95L);
  gblt_not_eq(a0,x1,write__atom__name__char_3_interrupt);
  allocp[0] = x3 = makeref(&allocp[0]);
  allocp[1] = makeint(95L);
  x2 = makecons(&allocp[0]);
  allocp += 2;
  unify_value(a1, x2);
  x4 = makeint(4L);
  bblt_rshift_no_check(a0,x4,x5);
  x6 = makeint(15L);
  bblt_and_no_check(a0,x6,x7);
  allocp[0] = a2;
  allocp[1] = x9 = makeref(&allocp[1]);
  x8 = makecons(&allocp[0]);
  allocp[2] = x8;
  allocp[3] = x11 = makeref(&allocp[3]);
  x10 = makecons(&allocp[2]);
  allocp += 4;
  unify_value(x3, x10);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__comp__obj_xhexchar_2);
  allocp[2] = x7;
  allocp[3] = x9;
  a0 = x5;
  a1 = x11;
  qp = (struct goalrec*)&allocp[0];
  allocp += 4;
  execute(hexchar_2_0);
  goto hexchar_2_ext_interrupt;
 write__atom__name__char_3_6:
  if (!isref(a0)) goto write__atom__name__char_3_interrupt;
  deref_and_jump(a0,write__atom__name__char_3_4);
  *reasonp++ =  a0;
  goto write__atom__name__char_3_interrupt;
 write__atom__name__char_3_ext_interrupt:
  reasonp = 0l;
 write__atom__name__char_3_interrupt:
  toppred = &predicate_klic__comp__obj_xwrite__atom__name__char_3;
  goto interrupt_3;
 }

 hexchar_2_top: {
  q x0, x1, x2, x3, x4, x5;
  qp = qp->next;
 hexchar_2_clear_reason:
  reasonp = reasons;
 hexchar_2_0:
 hexchar_2_1:
  if (!isint(a0)) goto hexchar_2_3;
  x0 = makeint(10L);
  gblt_less(a0,x0,hexchar_2_2);
  x1 = makeint(48L);
  bblt_add_no_check(x1,a0,x2);
  unify_value(a1, x2);
  proceed();
 hexchar_2_2:
  x1 = makeint(10L);
  gblt_greater_or_eq(a0,x1,hexchar_2_interrupt);
  x2 = makeint(65L);
  x3 = makeint(10L);
  bblt_sub_no_check(x2,x3,x4);
  bblt_add_no_check(x4,a0,x5);
  unify_value(a1, x5);
  proceed();
 hexchar_2_3:
  if (!isref(a0)) goto hexchar_2_interrupt;
  deref_and_jump(a0,hexchar_2_1);
  *reasonp++ =  a0;
  goto hexchar_2_interrupt;
 hexchar_2_ext_interrupt:
  reasonp = 0l;
 hexchar_2_interrupt:
  toppred = &predicate_klic__comp__obj_xhexchar_2;
  goto interrupt_2;
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
  loop_within_module(module_klic__comp__obj);
}
