/* Compiled by KLIC compiler version 3.003 (Mon Mar 29 19:42:59 JST 1999) */
#include <klic/klichdr.h>
#include "atom.h"
#include "funct.h"

module module_scanner();
Const struct predicate predicate_scanner_xscan_3 =
   { module_scanner, 0, 3 };
Const struct predicate predicate_scanner_xscan_6 =
   { module_scanner, 1, 6 };
Const struct predicate predicate_scanner_xscan_2F6_243_7 =
   { module_scanner, 2, 7 };
Const struct predicate predicate_scanner_xscan_2F6_243_2F7_240_7 =
   { module_scanner, 3, 7 };
Const struct predicate predicate_scanner_xscan_2F6_242_8 =
   { module_scanner, 4, 8 };
Const struct predicate predicate_scanner_xscan_2F6_241_8 =
   { module_scanner, 5, 8 };
Const struct predicate predicate_scanner_xscan_2F6_240_6 =
   { module_scanner, 6, 6 };
Const struct predicate predicate_scanner_xscan__after__atom_8 =
   { module_scanner, 7, 8 };
Const struct predicate predicate_scanner_xscan__comment__line_5 =
   { module_scanner, 8, 5 };
Const struct predicate predicate_scanner_xscan__comment_5 =
   { module_scanner, 9, 5 };
Const struct predicate predicate_scanner_xscan__comment_2F5_240_5 =
   { module_scanner, 10, 5 };
Const struct predicate predicate_scanner_xscan__rest_4 =
   { module_scanner, 11, 4 };
Const struct predicate predicate_scanner_xscan__name_7 =
   { module_scanner, 12, 7 };
Const struct predicate predicate_scanner_xscan__sname_7 =
   { module_scanner, 13, 7 };
Const struct predicate predicate_scanner_xscan__number_9 =
   { module_scanner, 14, 9 };
Const struct predicate predicate_scanner_xscan__number_2F9_241_8 =
   { module_scanner, 15, 8 };
Const struct predicate predicate_scanner_xscan__number_2F9_240_9 =
   { module_scanner, 16, 9 };
Const struct predicate predicate_scanner_xscan__fraction_9 =
   { module_scanner, 17, 9 };
Const struct predicate predicate_scanner_xscan__fraction_2F9_240_11 =
   { module_scanner, 18, 11 };
Const struct predicate predicate_scanner_xscan__exponent_9 =
   { module_scanner, 19, 9 };
Const struct predicate predicate_scanner_xscan__unsigned__exponent_9 =
   { module_scanner, 20, 9 };
Const struct predicate predicate_scanner_xchar__list__to__int_3 =
   { module_scanner, 21, 3 };
Const struct predicate predicate_scanner_xchar__list__to__float_3 =
   { module_scanner, 22, 3 };
Const struct predicate predicate_scanner_xscan__based__number_9 =
   { module_scanner, 23, 9 };
Const struct predicate predicate_scanner_xscan__based__number_2F9_242_9 =
   { module_scanner, 24, 9 };
Const struct predicate predicate_scanner_xscan__based__number_2F9_241_9 =
   { module_scanner, 25, 9 };
Const struct predicate predicate_scanner_xscan__based__number_2F9_240_9 =
   { module_scanner, 26, 9 };
Const struct predicate predicate_scanner_xlookup__var_8 =
   { module_scanner, 27, 8 };
Const struct predicate predicate_scanner_xfinalize__vars_1 =
   { module_scanner, 28, 1 };
Const struct predicate predicate_scanner_xfinalize__vars_2F1_240_2 =
   { module_scanner, 29, 2 };
Const struct predicate predicate_scanner_xscan__string_8 =
   { module_scanner, 30, 8 };
Const struct predicate predicate_scanner_xscan__string_2F8_240_8 =
   { module_scanner, 31, 8 };
Const struct predicate predicate_scanner_xescape__char_2 =
   { module_scanner, 32, 2 };
Const struct predicate predicate_scanner_xscan__escape_8 =
   { module_scanner, 33, 8 };
Const struct predicate predicate_scanner_xscan__octal__escape_9 =
   { module_scanner, 34, 9 };
Const struct predicate predicate_scanner_xscan__octal__escape__2_9 =
   { module_scanner, 35, 9 };
Const struct predicate predicate_scanner_xscan__octal__escape__2_2F9_240_9 =
   { module_scanner, 36, 9 };
Const struct predicate predicate_scanner_xscan__hex__escape_9 =
   { module_scanner, 37, 9 };
Const struct predicate predicate_scanner_xatom__chars_2 =
   { module_scanner, 38, 2 };
Const struct predicate predicate_scanner_xmake__new__string_2 =
   { module_scanner, 39, 2 };
Const struct predicate predicate_scanner_xget0_3 =
   { module_scanner, 40, 3 };
Const struct predicate predicate_scanner_xgetchar_4 =
   { module_scanner, 41, 4 };
Const struct predicate predicate_scanner_xungetc_3 =
   { module_scanner, 42, 3 };
Const struct predicate predicate_scanner_xchartype_2 =
   { module_scanner, 43, 2 };
extern q string_g_new();
extern q float_g_new();
extern Const struct predicate predicate_integer__arithmetics_xadd_3;
extern Const struct predicate predicate_atom__table_xmake__atom_2;

module module_scanner(glbl, qp, allocp, toppred)
  struct global_variables *glbl;
  struct goalrec *qp;
  register q *allocp;
  Const struct predicate *toppred;
{
  static Const q vectconst_body_0[] = {
    makeint(21),
    makeint(20),
    makeint(20),
    makeint(20),
    makeint(20),
    makeint(20),
    makeint(20),
    makeint(20),
    makeint(20),
    makeint(20),
    makeint(20),
    makeint(20),
    makeint(20),
    makeint(20),
    makeint(20),
    makeint(20),
    makeint(20),
    makeint(20),
    makeint(20),
    makeint(20),
    makeint(20),
    makeint(20),
    makeint(20),
    makeint(20),
    makeint(20),
    makeint(20),
    makeint(20),
    makeint(20),
    makeint(20),
    makeint(20),
    makeint(20),
    makeint(20),
    makeint(20),
    makeint(20),
    makeint(7),
    makeint(5),
    makeint(4),
    makeint(4),
    makeint(17),
    makeint(4),
    makeint(6),
    makeint(11),
    makeint(12),
    makeint(4),
    makeint(4),
    makeint(8),
    makeint(4),
    makeint(4),
    makeint(4),
    makeint(2),
    makeint(2),
    makeint(2),
    makeint(2),
    makeint(2),
    makeint(2),
    makeint(2),
    makeint(2),
    makeint(2),
    makeint(2),
    makeint(4),
    makeint(9),
    makeint(4),
    makeint(4),
    makeint(4),
    makeint(4),
    makeint(4),
    makeint(1),
    makeint(1),
    makeint(1),
    makeint(1),
    makeint(1),
    makeint(1),
    makeint(1),
    makeint(1),
    makeint(1),
    makeint(1),
    makeint(1),
    makeint(1),
    makeint(1),
    makeint(1),
    makeint(1),
    makeint(1),
    makeint(1),
    makeint(1),
    makeint(1),
    makeint(1),
    makeint(1),
    makeint(1),
    makeint(1),
    makeint(1),
    makeint(1),
    makeint(1),
    makeint(13),
    makeint(4),
    makeint(14),
    makeint(4),
    makeint(3),
    makeint(4),
    makeint(0),
    makeint(0),
    makeint(0),
    makeint(0),
    makeint(0),
    makeint(0),
    makeint(0),
    makeint(0),
    makeint(0),
    makeint(0),
    makeint(0),
    makeint(0),
    makeint(0),
    makeint(0),
    makeint(0),
    makeint(0),
    makeint(0),
    makeint(0),
    makeint(0),
    makeint(0),
    makeint(0),
    makeint(0),
    makeint(0),
    makeint(0),
    makeint(0),
    makeint(0),
    makeint(15),
    makeint(10),
    makeint(16),
    makeint(4),
    makeint(20),
    makeint(0),
    makeint(0),
    makeint(0),
    makeint(0),
    makeint(0),
    makeint(0),
    makeint(0),
    makeint(0),
    makeint(0),
    makeint(0),
    makeint(0),
    makeint(0),
    makeint(0),
    makeint(0),
    makeint(0),
    makeint(0),
    makeint(0),
    makeint(0),
    makeint(0),
    makeint(0),
    makeint(0),
    makeint(0),
    makeint(0),
    makeint(0),
    makeint(0),
    makeint(0),
    makeint(0),
    makeint(0),
    makeint(0),
    makeint(0),
    makeint(0),
    makeint(0),
    makeint(0),
    makeint(0),
    makeint(0),
    makeint(0),
    makeint(0),
    makeint(0),
    makeint(0),
    makeint(0),
    makeint(0),
    makeint(0),
    makeint(0),
    makeint(0),
    makeint(0),
    makeint(0),
    makeint(0),
    makeint(0),
    makeint(0),
    makeint(0),
    makeint(0),
    makeint(0),
    makeint(0),
    makeint(0),
    makeint(0),
    makeint(0),
    makeint(0),
    makeint(0),
    makeint(0),
    makeint(0),
    makeint(0),
    makeint(0),
    makeint(0),
    makeint(0),
    makeint(0),
    makeint(0),
    makeint(0),
    makeint(0),
    makeint(0),
    makeint(0),
    makeint(0),
    makeint(0),
    makeint(0),
    makeint(0),
    makeint(0),
    makeint(0),
    makeint(0),
    makeint(0),
    makeint(0),
    makeint(0),
    makeint(0),
    makeint(0),
    makeint(0),
    makeint(0),
    makeint(0),
    makeint(0),
    makeint(0),
    makeint(0),
    makeint(0),
    makeint(0),
    makeint(0),
    makeint(0),
    makeint(0),
    makeint(0),
    makeint(0),
    makeint(0),
    makeint(0),
    makeint(0),
    makeint(0),
    makeint(0),
    makeint(0),
    makeint(0),
    makeint(0),
    makeint(0),
    makeint(0),
    makeint(0),
    makeint(0),
    makeint(0),
    makeint(0),
    makeint(0),
    makeint(0),
    makeint(0),
    makeint(0),
    makeint(0),
    makeint(0),
    makeint(0),
    makeint(0),
    makeint(0),
    makeint(0),
    makeint(0),
    makeint(0),
    makeint(0),
    makeint(0),
    makeint(0),
    makeint(0),
    makeint(0),
    makeint(0),
    makeint(0),
  };
  declare_method_table_of(vector);
  static Const vector_structure_type vector_const_0 =
      declare_vector_constant(vectconst_body_0,257);
  static Const unsigned char strconst_body_1[60] =
    "end of file within a string literal or a quoted atom name";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_1 =
      declare_string_constant_8(strconst_body_1,57);
  static Const unsigned char strconst_body_2[48] =
    {'t','o','o',' ','l','a','r','g','e',' ','e','l','e','m','e','n','t',' ','i','n',' ','h','e','x','a','d','e','c','i','m','a','l',' ','e','s','c','a','p','e',' ','s','e','q','u','e','n','c','e',};
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_2 =
      declare_string_constant_8(strconst_body_2,48);
  static Const unsigned char strconst_body_3[44] =
    "too large element in octal escape sequence";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_3 =
      declare_string_constant_8(strconst_body_3,42);
  static Const q cons_const_4[] = {
    NILATOM,
    makeint(32),
  };
  static Const q cons_const_5[] = {
    NILATOM,
    makesym(atom_eof),
  };
  static Const q funct_const_6[] = {
    makesym(functor_vtab_2),
    makeint(0),
    NILATOM,
  };
  q a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10;

  q *reasonp;
 module_top:
  a0 = qp->args[0];
  switch_on_pred() {
    case_pred(0, scan_3_top);
    case_pred(1, scan_6_top);
    case_pred(2, scan_2F6_243_7_top);
    case_pred(3, scan_2F6_243_2F7_240_7_top);
    case_pred(4, scan_2F6_242_8_top);
    case_pred(5, scan_2F6_241_8_top);
    case_pred(6, scan_2F6_240_6_top);
    case_pred(7, scan__after__atom_8_top);
    case_pred(8, scan__comment__line_5_top);
    case_pred(9, scan__comment_5_top);
    case_pred(10, scan__comment_2F5_240_5_top);
    case_pred(11, scan__rest_4_top);
    case_pred(12, scan__name_7_top);
    case_pred(13, scan__sname_7_top);
    case_pred(14, scan__number_9_top);
    case_pred(15, scan__number_2F9_241_8_top);
    case_pred(16, scan__number_2F9_240_9_top);
    case_pred(17, scan__fraction_9_top);
    case_pred(18, scan__fraction_2F9_240_11_top);
    case_pred(19, scan__exponent_9_top);
    case_pred(20, scan__unsigned__exponent_9_top);
    case_pred(21, char__list__to__int_3_top);
    case_pred(22, char__list__to__float_3_top);
    case_pred(23, scan__based__number_9_top);
    case_pred(24, scan__based__number_2F9_242_9_top);
    case_pred(25, scan__based__number_2F9_241_9_top);
    case_pred(26, scan__based__number_2F9_240_9_top);
    case_pred(27, lookup__var_8_top);
    case_pred(28, finalize__vars_1_top);
    case_pred(29, finalize__vars_2F1_240_2_top);
    case_pred(30, scan__string_8_top);
    case_pred(31, scan__string_2F8_240_8_top);
    case_pred(32, escape__char_2_top);
    case_pred(33, scan__escape_8_top);
    case_pred(34, scan__octal__escape_9_top);
    case_pred(35, scan__octal__escape__2_9_top);
    case_pred(36, scan__octal__escape__2_2F9_240_9_top);
    case_pred(37, scan__hex__escape_9_top);
    case_pred(38, atom__chars_2_top);
    case_pred(39, make__new__string_2_top);
    case_pred(40, get0_3_top);
    case_pred(41, getchar_4_top);
    case_pred(42, ungetc_3_top);
    last_case_pred(43, chartype_2_top);
  }

 scan_3_top: {
  q x0, x1, x2;
  a1 = qp->args[1];
  a2 = qp->args[2];
  qp = qp->next;
 scan_3_clear_reason:
  reasonp = reasons;
 scan_3_0:
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_scanner_xscan_6);
  allocp[2] = x0 = makeref(&allocp[2]);
  allocp[3] = x1 = makeref(&allocp[3]);
  allocp[4] = makefunctor(funct_const_6);
  allocp[5] = a0;
  allocp[6] = x2 = makeref(&allocp[6]);
  allocp[7] = a2;
  a0 = x1;
  a2 = a1;
  a3 = x2;
  a1 = x0;
  qp = (struct goalrec*)&allocp[0];
  allocp += 8;
  execute(getchar_4_0);
  goto getchar_4_ext_interrupt;
 scan_3_ext_interrupt:
  reasonp = 0l;
 scan_3_interrupt:
  goto interrupt_3;
 }

 scan_6_top: {
  q x0, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13, x14, x15, x16, x17, x18, x19;
  a1 = qp->args[1];
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  qp = qp->next;
 scan_6_clear_reason:
  reasonp = reasons;
 scan_6_0:
 scan_6_1:
  switch (ptagof(a0)) {
 case ATOMIC:
 scan_6_2:
  switch ((unsigned long)a0) {
 case (unsigned long) makeint(0L):
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_scanner_xscan__after__atom_8);
  allocp[2] = x0 = makeref(&allocp[2]);
  allocp[3] = x1 = makeref(&allocp[3]);
  allocp[4] = x2 = makeref(&allocp[4]);
  allocp[5] = a2;
  allocp[6] = a3;
  allocp[7] = x3 = makeref(&allocp[7]);
  allocp[8] = x4 = makeref(&allocp[8]);
  allocp[9] = a5;
  allocp[10] = x6 = makeref(&allocp[10]);
  allocp[11] = a1;
  x5 = makecons(&allocp[10]);
  allocp[12] = (q)(struct goalrec*)&allocp[0];
  allocp[13] = (q)(&predicate_scanner_xatom__chars_2);
  allocp[14] = x2;
  allocp[15] = x5;
  allocp[16] = (q)(struct goalrec*)&allocp[12];
  allocp[17] = (q)(&predicate_scanner_xscan__name_7);
  allocp[18] = x7 = makeref(&allocp[18]);
  allocp[19] = x8 = makeref(&allocp[19]);
  allocp[20] = x6;
  allocp[21] = x0;
  allocp[22] = x1;
  allocp[23] = x9 = makeref(&allocp[23]);
  allocp[24] = x4;
  a0 = x8;
  a1 = x7;
  a2 = a4;
  a3 = x9;
  qp = (struct goalrec*)&allocp[16];
  allocp += 25;
  execute(getchar_4_0);
  goto getchar_4_ext_interrupt;
 case (unsigned long) makeint(1L):
 scan_6_3:
  switch (ptagof(a2)) {
 case FUNCTOR:
  if (functor_of(a2) != makesym(functor_vtab_2)) goto scan_6_interrupt;
  allocp[0] = makesym(functor_vtab_2);
  allocp[1] = x1 = makeref(&allocp[1]);
  allocp[2] = x2 = makeref(&allocp[2]);
  x0 = makefunctor(&allocp[0]);
  allocp[3] = (q)qp;
  allocp[4] = (q)(&predicate_scanner_xscan_6);
  allocp[5] = x3 = makeref(&allocp[5]);
  allocp[6] = x4 = makeref(&allocp[6]);
  allocp[7] = x0;
  allocp[8] = x5 = makeref(&allocp[8]);
  allocp[9] = x6 = makeref(&allocp[9]);
  allocp[10] = a5;
  allocp[11] = makesym(functor_variable_4);
  allocp[12] = x8 = makeref(&allocp[12]);
  allocp[13] = x9 = makeref(&allocp[13]);
  allocp[14] = x10 = makeref(&allocp[14]);
  allocp[15] = x11 = makeref(&allocp[15]);
  x7 = makefunctor(&allocp[11]);
  allocp[16] = x5;
  allocp[17] = x7;
  x12 = makecons(&allocp[16]);
  qp = (struct goalrec*)&allocp[3];
  allocp += 18;
  unify_value(a3, x12);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_scanner_xlookup__var_8);
  x13 = arg(a2, 1);
  allocp[2] = x13;
  x14 = arg(a2, 0);
  allocp[3] = x14;
  allocp[4] = x10;
  allocp[5] = x9;
  allocp[6] = x8;
  allocp[7] = x11;
  allocp[8] = x2;
  allocp[9] = x1;
  allocp[10] = x16 = makeref(&allocp[10]);
  allocp[11] = a1;
  x15 = makecons(&allocp[10]);
  allocp[12] = (q)(struct goalrec*)&allocp[0];
  allocp[13] = (q)(&predicate_scanner_xmake__new__string_2);
  allocp[14] = x10;
  allocp[15] = x15;
  allocp[16] = (q)(struct goalrec*)&allocp[12];
  allocp[17] = (q)(&predicate_scanner_xscan__name_7);
  allocp[18] = x17 = makeref(&allocp[18]);
  allocp[19] = x18 = makeref(&allocp[19]);
  allocp[20] = x16;
  allocp[21] = x3;
  allocp[22] = x4;
  allocp[23] = x19 = makeref(&allocp[23]);
  allocp[24] = x6;
  a0 = x18;
  a1 = x17;
  a2 = a4;
  a3 = x19;
  qp = (struct goalrec*)&allocp[16];
  allocp += 25;
  execute(getchar_4_0);
  goto getchar_4_ext_interrupt;
 case VARREF:
  deref_and_jump(a2,scan_6_3);
  *reasonp++ =  a2;
  goto scan_6_interrupt;
  };
  goto scan_6_interrupt;
 case (unsigned long) makeint(2L):
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_scanner_xscan_6);
  allocp[2] = x0 = makeref(&allocp[2]);
  allocp[3] = x1 = makeref(&allocp[3]);
  allocp[4] = a2;
  allocp[5] = x2 = makeref(&allocp[5]);
  allocp[6] = x3 = makeref(&allocp[6]);
  allocp[7] = a5;
  allocp[8] = makesym(functor_number_1);
  allocp[9] = x5 = makeref(&allocp[9]);
  x4 = makefunctor(&allocp[8]);
  allocp[10] = x2;
  allocp[11] = x4;
  x6 = makecons(&allocp[10]);
  qp = (struct goalrec*)&allocp[0];
  allocp += 12;
  unify_value(a3, x6);
  a0 = makeint(2L);
  a2 = x5;
  a3 = x1;
  allocp[0] = x7 = makeref(&allocp[0]);
  a5 = x7;
  a6 = x7;
  a7 = a4;
  a8 = x3;
  a4 = x0;
  allocp += 1;
  execute(scan__number_9_0);
  goto scan__number_9_ext_interrupt;
 case (unsigned long) makeint(3L):
 scan_6_4:
  switch (ptagof(a2)) {
 case FUNCTOR:
  if (functor_of(a2) != makesym(functor_vtab_2)) goto scan_6_interrupt;
  allocp[0] = makesym(functor_vtab_2);
  allocp[1] = x1 = makeref(&allocp[1]);
  x2 = arg(a2, 1);
  allocp[2] = x2;
  x0 = makefunctor(&allocp[0]);
  allocp[3] = (q)qp;
  allocp[4] = (q)(&predicate_scanner_xscan_6);
  allocp[5] = x3 = makeref(&allocp[5]);
  allocp[6] = x4 = makeref(&allocp[6]);
  allocp[7] = x0;
  allocp[8] = x5 = makeref(&allocp[8]);
  allocp[9] = x6 = makeref(&allocp[9]);
  allocp[10] = a5;
  allocp[11] = (q)(struct goalrec*)&allocp[3];
  allocp[12] = (q)(&predicate_integer__arithmetics_xadd_3);
  x7 = arg(a2, 0);
  allocp[13] = x7;
  allocp[14] = makeint(1L);
  allocp[15] = x1;
  allocp[16] = makesym(functor_variable_4);
  allocp[17] = x9 = makeref(&allocp[17]);
  allocp[18] = x7;
  allocp[19] = x10 = makeref(&allocp[19]);
  allocp[20] = makesym(atom_void);
  x8 = makefunctor(&allocp[16]);
  allocp[21] = x5;
  allocp[22] = x8;
  x11 = makecons(&allocp[21]);
  qp = (struct goalrec*)&allocp[11];
  allocp += 23;
  unify_value(a3, x11);
  allocp[0] = x13 = makeref(&allocp[0]);
  allocp[1] = a1;
  x12 = makecons(&allocp[0]);
  allocp[2] = (q)qp;
  allocp[3] = (q)(&predicate_scanner_xmake__new__string_2);
  allocp[4] = x10;
  allocp[5] = x12;
  allocp[6] = (q)(struct goalrec*)&allocp[2];
  allocp[7] = (q)(&predicate_scanner_xscan__name_7);
  allocp[8] = x14 = makeref(&allocp[8]);
  allocp[9] = x15 = makeref(&allocp[9]);
  allocp[10] = x13;
  allocp[11] = x3;
  allocp[12] = x4;
  allocp[13] = x16 = makeref(&allocp[13]);
  allocp[14] = x6;
  a0 = x15;
  a1 = x14;
  a2 = a4;
  a3 = x16;
  qp = (struct goalrec*)&allocp[6];
  allocp += 15;
  execute(getchar_4_0);
  goto getchar_4_ext_interrupt;
 case VARREF:
  deref_and_jump(a2,scan_6_4);
  *reasonp++ =  a2;
  goto scan_6_interrupt;
  };
  goto scan_6_interrupt;
 case (unsigned long) makeint(4L):
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_scanner_xscan_2F6_243_7);
  allocp[2] = a1;
  allocp[3] = x0 = makeref(&allocp[3]);
  allocp[4] = a2;
  allocp[5] = a3;
  allocp[6] = a5;
  allocp[7] = x1 = makeref(&allocp[7]);
  allocp[8] = x2 = makeref(&allocp[8]);
  a0 = x2;
  a1 = x0;
  a2 = a4;
  a3 = x1;
  qp = (struct goalrec*)&allocp[0];
  allocp += 9;
  execute(getchar_4_0);
  goto getchar_4_ext_interrupt;
 case (unsigned long) makeint(5L):
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_scanner_xscan_2F6_242_8);
  allocp[2] = x0 = makeref(&allocp[2]);
  allocp[3] = x1 = makeref(&allocp[3]);
  allocp[4] = a3;
  allocp[5] = x2 = makeref(&allocp[5]);
  allocp[6] = x3 = makeref(&allocp[6]);
  allocp[7] = a2;
  allocp[8] = x4 = makeref(&allocp[8]);
  allocp[9] = a5;
  allocp[10] = (q)(struct goalrec*)&allocp[0];
  allocp[11] = (q)(&predicate_scanner_xscan__string_8);
  allocp[12] = x5 = makeref(&allocp[12]);
  allocp[13] = a1;
  allocp[14] = x1;
  allocp[15] = x3;
  allocp[16] = x2;
  allocp[17] = x0;
  allocp[18] = x6 = makeref(&allocp[18]);
  allocp[19] = x4;
  a0 = x5;
  a1 = a4;
  a2 = x6;
  qp = (struct goalrec*)&allocp[10];
  allocp += 20;
  execute(get0_3_0);
  goto get0_3_ext_interrupt;
 case (unsigned long) makeint(6L):
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_scanner_xscan_2F6_241_8);
  allocp[2] = x0 = makeref(&allocp[2]);
  allocp[3] = x1 = makeref(&allocp[3]);
  allocp[4] = x2 = makeref(&allocp[4]);
  allocp[5] = x3 = makeref(&allocp[5]);
  allocp[6] = a2;
  allocp[7] = a3;
  allocp[8] = x4 = makeref(&allocp[8]);
  allocp[9] = a5;
  allocp[10] = (q)(struct goalrec*)&allocp[0];
  allocp[11] = (q)(&predicate_scanner_xscan__string_8);
  allocp[12] = x5 = makeref(&allocp[12]);
  allocp[13] = a1;
  allocp[14] = x1;
  allocp[15] = x3;
  allocp[16] = x2;
  allocp[17] = x0;
  allocp[18] = x6 = makeref(&allocp[18]);
  allocp[19] = x4;
  a0 = x5;
  a1 = a4;
  a2 = x6;
  qp = (struct goalrec*)&allocp[10];
  allocp += 20;
  execute(get0_3_0);
  goto get0_3_ext_interrupt;
 case (unsigned long) makeint(7L):
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_scanner_xscan__after__atom_8);
  allocp[2] = x0 = makeref(&allocp[2]);
  allocp[3] = x1 = makeref(&allocp[3]);
  allocp[4] = makesym(atom__21);
  allocp[5] = a2;
  allocp[6] = a3;
  allocp[7] = x2 = makeref(&allocp[7]);
  allocp[8] = x3 = makeref(&allocp[8]);
  allocp[9] = a5;
  a0 = x1;
  a1 = x0;
  a2 = a4;
  a3 = x3;
  qp = (struct goalrec*)&allocp[0];
  allocp += 10;
  execute(getchar_4_0);
  goto getchar_4_ext_interrupt;
 case (unsigned long) makeint(9L):
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_scanner_xscan__after__atom_8);
  allocp[2] = x0 = makeref(&allocp[2]);
  allocp[3] = x1 = makeref(&allocp[3]);
  allocp[4] = makesym(atom__3B);
  allocp[5] = a2;
  allocp[6] = a3;
  allocp[7] = x2 = makeref(&allocp[7]);
  allocp[8] = x3 = makeref(&allocp[8]);
  allocp[9] = a5;
  a0 = x1;
  a1 = x0;
  a2 = a4;
  a3 = x3;
  qp = (struct goalrec*)&allocp[0];
  allocp += 10;
  execute(getchar_4_0);
  goto getchar_4_ext_interrupt;
 case (unsigned long) makeint(8L):
  allocp[0] = x1 = makeref(&allocp[0]);
  allocp[1] = makesym(atom__2C);
  x0 = makecons(&allocp[0]);
  allocp += 2;
  unify_value(a3, x0);
  a0 = a2;
  a1 = x1;
  a2 = a4;
  a3 = a5;
  execute(scan__rest_4_0);
  goto scan__rest_4_ext_interrupt;
 case (unsigned long) makeint(10L):
  allocp[0] = x1 = makeref(&allocp[0]);
  allocp[1] = makesym(atom__7C);
  x0 = makecons(&allocp[0]);
  allocp += 2;
  unify_value(a3, x0);
  a0 = a2;
  a1 = x1;
  a2 = a4;
  a3 = a5;
  execute(scan__rest_4_0);
  goto scan__rest_4_ext_interrupt;
 case (unsigned long) makeint(11L):
  allocp[0] = x1 = makeref(&allocp[0]);
  allocp[1] = makesym(atom__20_28);
  x0 = makecons(&allocp[0]);
  allocp += 2;
  unify_value(a3, x0);
  a0 = a2;
  a1 = x1;
  a2 = a4;
  a3 = a5;
  execute(scan__rest_4_0);
  goto scan__rest_4_ext_interrupt;
 case (unsigned long) makeint(12L):
  allocp[0] = x1 = makeref(&allocp[0]);
  allocp[1] = makesym(atom__29);
  x0 = makecons(&allocp[0]);
  allocp += 2;
  unify_value(a3, x0);
  a0 = a2;
  a1 = x1;
  a2 = a4;
  a3 = a5;
  execute(scan__rest_4_0);
  goto scan__rest_4_ext_interrupt;
 case (unsigned long) makeint(13L):
  allocp[0] = x1 = makeref(&allocp[0]);
  allocp[1] = makesym(atom__5B);
  x0 = makecons(&allocp[0]);
  allocp += 2;
  unify_value(a3, x0);
  a0 = a2;
  a1 = x1;
  a2 = a4;
  a3 = a5;
  execute(scan__rest_4_0);
  goto scan__rest_4_ext_interrupt;
 case (unsigned long) makeint(14L):
  allocp[0] = x1 = makeref(&allocp[0]);
  allocp[1] = makesym(atom__5D);
  x0 = makecons(&allocp[0]);
  allocp += 2;
  unify_value(a3, x0);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_scanner_xscan_2F6_240_6);
  allocp[2] = x2 = makeref(&allocp[2]);
  allocp[3] = x1;
  allocp[4] = a2;
  allocp[5] = x3 = makeref(&allocp[5]);
  allocp[6] = a5;
  allocp[7] = x4 = makeref(&allocp[7]);
  a0 = x4;
  a1 = x2;
  a2 = a4;
  a3 = x3;
  qp = (struct goalrec*)&allocp[0];
  allocp += 8;
  execute(getchar_4_0);
  goto getchar_4_ext_interrupt;
 case (unsigned long) makeint(15L):
  allocp[0] = x1 = makeref(&allocp[0]);
  allocp[1] = makesym(atom__7B);
  x0 = makecons(&allocp[0]);
  allocp += 2;
  unify_value(a3, x0);
  a0 = a2;
  a1 = x1;
  a2 = a4;
  a3 = a5;
  execute(scan__rest_4_0);
  goto scan__rest_4_ext_interrupt;
 case (unsigned long) makeint(16L):
  allocp[0] = x1 = makeref(&allocp[0]);
  allocp[1] = makesym(atom__7D);
  x0 = makecons(&allocp[0]);
  allocp += 2;
  unify_value(a3, x0);
  a0 = a2;
  a1 = x1;
  a2 = a4;
  a3 = a5;
  execute(scan__rest_4_0);
  goto scan__rest_4_ext_interrupt;
 case (unsigned long) makeint(17L):
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_scanner_xscan__comment__line_5);
  allocp[2] = x0 = makeref(&allocp[2]);
  allocp[3] = a2;
  allocp[4] = a3;
  allocp[5] = x1 = makeref(&allocp[5]);
  allocp[6] = a5;
  a0 = x0;
  a1 = a4;
  a2 = x1;
  qp = (struct goalrec*)&allocp[0];
  allocp += 7;
  execute(get0_3_0);
  goto get0_3_ext_interrupt;
 case (unsigned long) makeint(20L):
  a0 = a2;
  a1 = a3;
  a2 = a4;
  a3 = a5;
  execute(scan__rest_4_0);
  goto scan__rest_4_ext_interrupt;
 case (unsigned long) makeint(21L):
 scan_6_5:
  switch (ptagof(a2)) {
 case FUNCTOR:
  if (functor_of(a2) != makesym(functor_vtab_2)) goto scan_6_interrupt;
  unify(a5, a4);
  x0 = makecons(cons_const_5);
  unify_value(a3, x0);
  a0 = arg(a2, 1);
  execute(finalize__vars_1_0);
  goto finalize__vars_1_ext_interrupt;
 case VARREF:
  deref_and_jump(a2,scan_6_5);
  *reasonp++ =  a2;
  goto scan_6_interrupt;
  };
  goto scan_6_interrupt;
  };
  goto scan_6_interrupt;
 case VARREF:
  deref_and_jump(a0,scan_6_1);
  *reasonp++ =  a0;
  goto scan_6_interrupt;
  };
  goto scan_6_interrupt;
 scan_6_ext_interrupt:
  reasonp = 0l;
 scan_6_interrupt:
  toppred = &predicate_scanner_xscan_6;
  goto interrupt_6;
 }

 scan_2F6_243_7_top: {
  q x0, x1, x2, x3, x4, x5, x6;
  a1 = qp->args[1];
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  a6 = qp->args[6];
  qp = qp->next;
 scan_2F6_243_7_clear_reason:
  reasonp = reasons;
 scan_2F6_243_7_0:
 scan_2F6_243_7_1:
  if (!isint(a0)) goto scan_2F6_243_7_6;
  x0 = makeint(46L);
  gblt_eq(a0,x0,scan_2F6_243_7_5);
 scan_2F6_243_7_2:
  if (!isint(a1)) goto scan_2F6_243_7_4;
  x1 = makeint(20L);
  gblt_greater_or_eq(a1,x1,scan_2F6_243_7_5);
 scan_2F6_243_7_3:
  switch (ptagof(a2)) {
 case FUNCTOR:
  if (functor_of(a2) != makesym(functor_vtab_2)) goto scan_2F6_243_7_5;
  x2 = NILATOM;
  unify_value(a3, x2);
  unify(a4, a5);
  a0 = arg(a2, 1);
  execute(finalize__vars_1_0);
  goto finalize__vars_1_ext_interrupt;
 case VARREF:
  deref_and_jump(a2,scan_2F6_243_7_3);
  *reasonp++ =  a2;
  goto scan_2F6_243_7_5;
  };
  goto scan_2F6_243_7_5;
 scan_2F6_243_7_4:
  if (!isref(a1)) goto scan_2F6_243_7_5;
  deref_and_jump(a1,scan_2F6_243_7_2);
  *reasonp++ =  a1;
  goto scan_2F6_243_7_5;
 scan_2F6_243_7_5:
  x1 = makeint(47L);
  gblt_eq(a0,x1,scan_2F6_243_7_7);
  x2 = a6;
  x3 = a5;
  a5 = a1;
  a6 = a0;
  a1 = x3;
  a0 = x2;
  execute(scan_2F6_243_2F7_240_7_0);
  goto scan_2F6_243_2F7_240_7_ext_interrupt;
 scan_2F6_243_7_6:
  if (!isref(a0)) goto scan_2F6_243_7_7;
  deref_and_jump(a0,scan_2F6_243_7_1);
  *reasonp++ =  a0;
  goto scan_2F6_243_7_7;
 scan_2F6_243_7_7:
  otherwise(scan_2F6_243_7_interrupt);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_scanner_xscan__after__atom_8);
  allocp[2] = x0 = makeref(&allocp[2]);
  allocp[3] = x1 = makeref(&allocp[3]);
  allocp[4] = x2 = makeref(&allocp[4]);
  allocp[5] = a2;
  allocp[6] = a3;
  allocp[7] = x3 = makeref(&allocp[7]);
  allocp[8] = x4 = makeref(&allocp[8]);
  allocp[9] = a4;
  allocp[10] = x6 = makeref(&allocp[10]);
  allocp[11] = a0;
  x5 = makecons(&allocp[10]);
  allocp[12] = (q)(struct goalrec*)&allocp[0];
  allocp[13] = (q)(&predicate_scanner_xatom__chars_2);
  allocp[14] = x2;
  allocp[15] = x5;
  a0 = a1;
  a1 = a6;
  a2 = x6;
  a3 = x0;
  a4 = x1;
  a6 = x4;
  qp = (struct goalrec*)&allocp[12];
  allocp += 16;
  execute(scan__sname_7_0);
  goto scan__sname_7_ext_interrupt;
 scan_2F6_243_7_ext_interrupt:
  reasonp = 0l;
 scan_2F6_243_7_interrupt:
  goto interrupt_7;
 }

 scan_2F6_243_2F7_240_7_top: {
  q x0, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10;
  a1 = qp->args[1];
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  a6 = qp->args[6];
  qp = qp->next;
 scan_2F6_243_2F7_240_7_clear_reason:
  reasonp = reasons;
 scan_2F6_243_2F7_240_7_0:
 scan_2F6_243_2F7_240_7_1:
  if (!isint(a0)) goto scan_2F6_243_2F7_240_7_3;
  x0 = makeint(42L);
  gblt_eq(a0,x0,scan_2F6_243_2F7_240_7_2);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_scanner_xscan_6);
  allocp[2] = x1 = makeref(&allocp[2]);
  allocp[3] = x2 = makeref(&allocp[3]);
  allocp[4] = a2;
  allocp[5] = a3;
  allocp[6] = x3 = makeref(&allocp[6]);
  allocp[7] = a4;
  allocp[8] = (q)(struct goalrec*)&allocp[0];
  allocp[9] = (q)(&predicate_scanner_xscan__comment_5);
  allocp[10] = x4 = makeref(&allocp[10]);
  allocp[11] = x1;
  allocp[12] = x2;
  allocp[13] = x5 = makeref(&allocp[13]);
  allocp[14] = x3;
  a0 = x4;
  a2 = x5;
  qp = (struct goalrec*)&allocp[8];
  allocp += 15;
  execute(get0_3_0);
  goto get0_3_ext_interrupt;
 scan_2F6_243_2F7_240_7_2:
  x1 = makeint(42L);
  gblt_not_eq(a0,x1,scan_2F6_243_2F7_240_7_interrupt);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_scanner_xscan__after__atom_8);
  allocp[2] = x2 = makeref(&allocp[2]);
  allocp[3] = x3 = makeref(&allocp[3]);
  allocp[4] = x4 = makeref(&allocp[4]);
  allocp[5] = a2;
  allocp[6] = a3;
  allocp[7] = x5 = makeref(&allocp[7]);
  allocp[8] = x6 = makeref(&allocp[8]);
  allocp[9] = a4;
  allocp[10] = x8 = makeref(&allocp[10]);
  allocp[11] = a6;
  x7 = makecons(&allocp[10]);
  allocp[12] = (q)(struct goalrec*)&allocp[0];
  allocp[13] = (q)(&predicate_scanner_xatom__chars_2);
  allocp[14] = x4;
  allocp[15] = x7;
  x9 = a5;
  x10 = a0;
  a2 = x8;
  a3 = x2;
  a4 = x3;
  a5 = a1;
  a6 = x6;
  a1 = x10;
  a0 = x9;
  qp = (struct goalrec*)&allocp[12];
  allocp += 16;
  execute(scan__sname_7_0);
  goto scan__sname_7_ext_interrupt;
 scan_2F6_243_2F7_240_7_3:
  if (!isref(a0)) goto scan_2F6_243_2F7_240_7_interrupt;
  deref_and_jump(a0,scan_2F6_243_2F7_240_7_1);
  *reasonp++ =  a0;
  goto scan_2F6_243_2F7_240_7_interrupt;
 scan_2F6_243_2F7_240_7_ext_interrupt:
  reasonp = 0l;
 scan_2F6_243_2F7_240_7_interrupt:
  toppred = &predicate_scanner_xscan_2F6_243_2F7_240_7;
  goto interrupt_7;
 }

 scan_2F6_242_8_top: {
  q x0, x1, x2, x3;
  a1 = qp->args[1];
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  a6 = qp->args[6];
  a7 = qp->args[7];
  qp = qp->next;
 scan_2F6_242_8_clear_reason:
  reasonp = reasons;
 scan_2F6_242_8_0:
 scan_2F6_242_8_1:
  switch (ptagof(a0)) {
 case ATOMIC:
  if (a0 != NILATOM) goto scan_2F6_242_8_2;
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_scanner_xscan_6);
  allocp[2] = a3;
  allocp[3] = a4;
  allocp[4] = a5;
  allocp[5] = x0 = makeref(&allocp[5]);
  allocp[6] = a6;
  allocp[7] = a7;
  allocp[8] = makesym(functor_string_1);
  allocp[9] = x2 = makeref(&allocp[9]);
  x1 = makefunctor(&allocp[8]);
  allocp[10] = x0;
  allocp[11] = x1;
  x3 = makecons(&allocp[10]);
  qp = (struct goalrec*)&allocp[0];
  allocp += 12;
  unify_value(a2, x3);
  a0 = x2;
  execute(make__new__string_2_0);
  goto make__new__string_2_ext_interrupt;
 case VARREF:
  deref_and_jump(a0,scan_2F6_242_8_1);
  *reasonp++ =  a0;
  goto scan_2F6_242_8_2;
  };
  goto scan_2F6_242_8_2;
 scan_2F6_242_8_2:
  otherwise(scan_2F6_242_8_interrupt);
  allocp[0] = makesym(functor_error_1);
  allocp[1] = a0;
  x0 = makefunctor(&allocp[0]);
  allocp[2] = x2 = makeref(&allocp[2]);
  allocp[3] = x0;
  x1 = makecons(&allocp[2]);
  allocp += 4;
  unify_value(a2, x1);
  a0 = a3;
  a1 = a4;
  a2 = a5;
  a3 = x2;
  a4 = a6;
  a5 = a7;
  execute(scan_6_0);
  goto scan_6_ext_interrupt;
 scan_2F6_242_8_ext_interrupt:
  reasonp = 0l;
 scan_2F6_242_8_interrupt:
  goto interrupt_8;
 }

 scan_2F6_241_8_top: {
  q x0, x1, x2;
  a1 = qp->args[1];
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  a6 = qp->args[6];
  a7 = qp->args[7];
  qp = qp->next;
 scan_2F6_241_8_clear_reason:
  reasonp = reasons;
 scan_2F6_241_8_0:
 scan_2F6_241_8_1:
  switch (ptagof(a0)) {
 case ATOMIC:
  if (a0 != NILATOM) goto scan_2F6_241_8_2;
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_scanner_xscan__after__atom_8);
  allocp[2] = a2;
  allocp[3] = a3;
  allocp[4] = x0 = makeref(&allocp[4]);
  allocp[5] = a4;
  allocp[6] = a5;
  allocp[7] = x1 = makeref(&allocp[7]);
  allocp[8] = a6;
  allocp[9] = a7;
  a0 = x0;
  qp = (struct goalrec*)&allocp[0];
  allocp += 10;
  execute(atom__chars_2_0);
  goto atom__chars_2_ext_interrupt;
 case VARREF:
  deref_and_jump(a0,scan_2F6_241_8_1);
  *reasonp++ =  a0;
  goto scan_2F6_241_8_2;
  };
  goto scan_2F6_241_8_2;
 scan_2F6_241_8_2:
  otherwise(scan_2F6_241_8_interrupt);
  allocp[0] = makesym(functor_error_1);
  allocp[1] = a0;
  x0 = makefunctor(&allocp[0]);
  allocp[2] = x2 = makeref(&allocp[2]);
  allocp[3] = x0;
  x1 = makecons(&allocp[2]);
  allocp += 4;
  unify_value(a5, x1);
  a0 = a2;
  a1 = a3;
  a2 = a4;
  a3 = x2;
  a4 = a6;
  a5 = a7;
  execute(scan_6_0);
  goto scan_6_ext_interrupt;
 scan_2F6_241_8_ext_interrupt:
  reasonp = 0l;
 scan_2F6_241_8_interrupt:
  goto interrupt_8;
 }

 scan_2F6_240_6_top: {
  q x0, x1, x2;
  a1 = qp->args[1];
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  qp = qp->next;
 scan_2F6_240_6_clear_reason:
  reasonp = reasons;
 scan_2F6_240_6_0:
 scan_2F6_240_6_1:
  switch (ptagof(a0)) {
 case ATOMIC:
 scan_2F6_240_6_2:
  if (a0 != makeint(11L)) goto scan_2F6_240_6_3;
  allocp[0] = x1 = makeref(&allocp[0]);
  allocp[1] = makesym(atom__28);
  x0 = makecons(&allocp[0]);
  allocp += 2;
  unify_value(a1, x0);
  a0 = a2;
  a1 = x1;
  a2 = a3;
  a3 = a4;
  execute(scan__rest_4_0);
  goto scan__rest_4_ext_interrupt;
 case VARREF:
  deref_and_jump(a0,scan_2F6_240_6_1);
  *reasonp++ =  a0;
  goto scan_2F6_240_6_3;
  };
  goto scan_2F6_240_6_3;
 scan_2F6_240_6_3:
  otherwise(scan_2F6_240_6_interrupt);
  x0 = a5;
  x1 = a1;
  x2 = a3;
  a5 = a4;
  a4 = x2;
  a3 = x1;
  a1 = x0;
  execute(scan_6_0);
  goto scan_6_ext_interrupt;
 scan_2F6_240_6_ext_interrupt:
  reasonp = 0l;
 scan_2F6_240_6_interrupt:
  goto interrupt_6;
 }

 scan__after__atom_8_top: {
  q x0, x1, x2, x3;
  a1 = qp->args[1];
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  a6 = qp->args[6];
  a7 = qp->args[7];
  qp = qp->next;
 scan__after__atom_8_clear_reason:
  reasonp = reasons;
 scan__after__atom_8_0:
 scan__after__atom_8_1:
  if (!isint(a0)) goto scan__after__atom_8_3;
  x0 = makeint(11L);
  gblt_eq(a0,x0,scan__after__atom_8_2);
  allocp[0] = makesym(functor_atom_1);
  allocp[1] = a2;
  x1 = makefunctor(&allocp[0]);
  allocp[2] = a5;
  allocp[3] = makesym(atom__28);
  x2 = makecons(&allocp[2]);
  allocp[4] = x2;
  allocp[5] = x1;
  x3 = makecons(&allocp[4]);
  allocp += 6;
  unify_value(a4, x3);
  a0 = a3;
  a1 = a5;
  a2 = a6;
  a3 = a7;
  execute(scan__rest_4_0);
  goto scan__rest_4_ext_interrupt;
 scan__after__atom_8_2:
  x1 = makeint(11L);
  gblt_not_eq(a0,x1,scan__after__atom_8_interrupt);
  allocp[0] = makesym(functor_atom_1);
  allocp[1] = a2;
  x2 = makefunctor(&allocp[0]);
  allocp[2] = a5;
  allocp[3] = x2;
  x3 = makecons(&allocp[2]);
  allocp += 4;
  unify_value(a4, x3);
  a2 = a3;
  a3 = a5;
  a4 = a6;
  a5 = a7;
  execute(scan_6_0);
  goto scan_6_ext_interrupt;
 scan__after__atom_8_3:
  if (!isref(a0)) goto scan__after__atom_8_interrupt;
  deref_and_jump(a0,scan__after__atom_8_1);
  *reasonp++ =  a0;
  goto scan__after__atom_8_interrupt;
 scan__after__atom_8_ext_interrupt:
  reasonp = 0l;
 scan__after__atom_8_interrupt:
  goto interrupt_8;
 }

 scan__comment__line_5_top: {
  q x0, x1, x2, x3, x4, x5, x6;
  a1 = qp->args[1];
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  qp = qp->next;
 scan__comment__line_5_clear_reason:
  reasonp = reasons;
 scan__comment__line_5_0:
 scan__comment__line_5_1:
  if (!isint(a0)) goto scan__comment__line_5_4;
  x0 = makeint(0L);
  gblt_less(a0,x0,scan__comment__line_5_2);
  x1 = a0;
  x2 = a1;
  x3 = a2;
  x4 = a3;
  a5 = a4;
  a4 = x4;
  a3 = x3;
  a2 = x2;
  a1 = x1;
  a0 = makeint(21L);
  execute(scan_6_0);
  goto scan_6_ext_interrupt;
 scan__comment__line_5_2:
  x1 = makeint(0L);
  gblt_greater_or_eq(a0,x1,scan__comment__line_5_interrupt);
  x2 = makeint(10L);
  gblt_eq(a0,x2,scan__comment__line_5_3);
  a0 = a1;
  a1 = a2;
  a2 = a3;
  a3 = a4;
  execute(scan__rest_4_0);
  goto scan__rest_4_ext_interrupt;
 scan__comment__line_5_3:
  x3 = makeint(10L);
  gblt_not_eq(a0,x3,scan__comment__line_5_interrupt);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_scanner_xscan__comment__line_5);
  allocp[2] = x4 = makeref(&allocp[2]);
  allocp[3] = a1;
  allocp[4] = a2;
  allocp[5] = x5 = makeref(&allocp[5]);
  allocp[6] = a4;
  a0 = x4;
  allocp[7] = x6 = makeref(&allocp[7]);
  a1 = x6;
  a2 = a3;
  a3 = x5;
  qp = (struct goalrec*)&allocp[0];
  allocp += 8;
  execute(getchar_4_0);
  goto getchar_4_ext_interrupt;
 scan__comment__line_5_4:
  if (!isref(a0)) goto scan__comment__line_5_interrupt;
  deref_and_jump(a0,scan__comment__line_5_1);
  *reasonp++ =  a0;
  goto scan__comment__line_5_interrupt;
 scan__comment__line_5_ext_interrupt:
  reasonp = 0l;
 scan__comment__line_5_interrupt:
  goto interrupt_5;
 }

 scan__comment_5_top: {
  q x0, x1, x2, x3;
  a1 = qp->args[1];
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  qp = qp->next;
 scan__comment_5_clear_reason:
  reasonp = reasons;
 scan__comment_5_0:
 scan__comment_5_1:
  if (!isint(a0)) goto scan__comment_5_3;
  x0 = makeint(42L);
  gblt_eq(a0,x0,scan__comment_5_2);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_scanner_xscan__comment_2F5_240_5);
  allocp[2] = x1 = makeref(&allocp[2]);
  allocp[3] = a2;
  allocp[4] = a1;
  allocp[5] = x2 = makeref(&allocp[5]);
  allocp[6] = a4;
  a0 = x1;
  a1 = a3;
  a2 = x2;
  qp = (struct goalrec*)&allocp[0];
  allocp += 7;
  execute(get0_3_0);
  goto get0_3_ext_interrupt;
 scan__comment_5_2:
  x1 = makeint(42L);
  gblt_not_eq(a0,x1,scan__comment_5_interrupt);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_scanner_xscan__comment_5);
  allocp[2] = x2 = makeref(&allocp[2]);
  allocp[3] = a1;
  allocp[4] = a2;
  allocp[5] = x3 = makeref(&allocp[5]);
  allocp[6] = a4;
  a0 = x2;
  a1 = a3;
  a2 = x3;
  qp = (struct goalrec*)&allocp[0];
  allocp += 7;
  execute(get0_3_0);
  goto get0_3_ext_interrupt;
 scan__comment_5_3:
  if (!isref(a0)) goto scan__comment_5_interrupt;
  deref_and_jump(a0,scan__comment_5_1);
  *reasonp++ =  a0;
  goto scan__comment_5_interrupt;
 scan__comment_5_ext_interrupt:
  reasonp = 0l;
 scan__comment_5_interrupt:
  toppred = &predicate_scanner_xscan__comment_5;
  goto interrupt_5;
 }

 scan__comment_2F5_240_5_top: {
  q x0, x1, x2;
  a1 = qp->args[1];
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  qp = qp->next;
 scan__comment_2F5_240_5_clear_reason:
  reasonp = reasons;
 scan__comment_2F5_240_5_0:
 scan__comment_2F5_240_5_1:
  if (!isint(a0)) goto scan__comment_2F5_240_5_3;
  x0 = makeint(47L);
  gblt_eq(a0,x0,scan__comment_2F5_240_5_2);
  a0 = a1;
  a1 = a2;
  a2 = a3;
  a3 = a4;
  execute(getchar_4_0);
  goto getchar_4_ext_interrupt;
 scan__comment_2F5_240_5_2:
  x1 = makeint(47L);
  gblt_not_eq(a0,x1,scan__comment_2F5_240_5_interrupt);
  x2 = a2;
  a2 = a1;
  a1 = x2;
  execute(scan__comment_5_0);
  goto scan__comment_5_ext_interrupt;
 scan__comment_2F5_240_5_3:
  if (!isref(a0)) goto scan__comment_2F5_240_5_interrupt;
  deref_and_jump(a0,scan__comment_2F5_240_5_1);
  *reasonp++ =  a0;
  goto scan__comment_2F5_240_5_interrupt;
 scan__comment_2F5_240_5_ext_interrupt:
  reasonp = 0l;
 scan__comment_2F5_240_5_interrupt:
  goto interrupt_5;
 }

 scan__rest_4_top: {
  q x0, x1, x2;
  a1 = qp->args[1];
  a2 = qp->args[2];
  a3 = qp->args[3];
  qp = qp->next;
 scan__rest_4_clear_reason:
  reasonp = reasons;
 scan__rest_4_0:
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_scanner_xscan_6);
  allocp[2] = x0 = makeref(&allocp[2]);
  allocp[3] = x1 = makeref(&allocp[3]);
  allocp[4] = a0;
  allocp[5] = a1;
  allocp[6] = x2 = makeref(&allocp[6]);
  allocp[7] = a3;
  a0 = x1;
  a1 = x0;
  a3 = x2;
  qp = (struct goalrec*)&allocp[0];
  allocp += 8;
  execute(getchar_4_0);
  goto getchar_4_ext_interrupt;
 scan__rest_4_ext_interrupt:
  reasonp = 0l;
 scan__rest_4_interrupt:
  toppred = &predicate_scanner_xscan__rest_4;
  goto interrupt_4;
 }

 scan__name_7_top: {
  q x0, x1, x2, x3, x4, x5;
  a1 = qp->args[1];
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  a6 = qp->args[6];
  qp = qp->next;
 scan__name_7_clear_reason:
  reasonp = reasons;
 scan__name_7_0:
 scan__name_7_1:
  if (!isint(a0)) goto scan__name_7_3;
  x0 = makeint(3L);
  gblt_less_or_eq(a0,x0,scan__name_7_2);
  allocp[0] = x2 = makeref(&allocp[0]);
  allocp[1] = a1;
  x1 = makecons(&allocp[0]);
  allocp += 2;
  unify_value(a2, x1);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_scanner_xscan__name_7);
  allocp[2] = x3 = makeref(&allocp[2]);
  allocp[3] = x4 = makeref(&allocp[3]);
  allocp[4] = x2;
  allocp[5] = a3;
  allocp[6] = a4;
  allocp[7] = x5 = makeref(&allocp[7]);
  allocp[8] = a6;
  a0 = x4;
  a1 = x3;
  a2 = a5;
  a3 = x5;
  qp = (struct goalrec*)&allocp[0];
  allocp += 9;
  execute(getchar_4_0);
  goto getchar_4_ext_interrupt;
 scan__name_7_2:
  x1 = makeint(3L);
  gblt_greater(a0,x1,scan__name_7_interrupt);
  unify(a6, a5);
  x2 = NILATOM;
  unify_value(a2, x2);
  unify(a4, a1);
  unify_value(a3, a0);
  proceed();
 scan__name_7_3:
  if (!isref(a0)) goto scan__name_7_interrupt;
  deref_and_jump(a0,scan__name_7_1);
  *reasonp++ =  a0;
  goto scan__name_7_interrupt;
 scan__name_7_ext_interrupt:
  reasonp = 0l;
 scan__name_7_interrupt:
  goto interrupt_7;
 }

 scan__sname_7_top: {
  q x0, x1, x2, x3, x4, x5;
  a1 = qp->args[1];
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  a6 = qp->args[6];
  qp = qp->next;
 scan__sname_7_clear_reason:
  reasonp = reasons;
 scan__sname_7_0:
 scan__sname_7_1:
  if (!isint(a0)) goto scan__sname_7_3;
  x0 = makeint(4L);
  gblt_eq(a0,x0,scan__sname_7_2);
  allocp[0] = x2 = makeref(&allocp[0]);
  allocp[1] = a1;
  x1 = makecons(&allocp[0]);
  allocp += 2;
  unify_value(a2, x1);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_scanner_xscan__sname_7);
  allocp[2] = x3 = makeref(&allocp[2]);
  allocp[3] = x4 = makeref(&allocp[3]);
  allocp[4] = x2;
  allocp[5] = a3;
  allocp[6] = a4;
  allocp[7] = x5 = makeref(&allocp[7]);
  allocp[8] = a6;
  a0 = x4;
  a1 = x3;
  a2 = a5;
  a3 = x5;
  qp = (struct goalrec*)&allocp[0];
  allocp += 9;
  execute(getchar_4_0);
  goto getchar_4_ext_interrupt;
 scan__sname_7_2:
  x1 = makeint(4L);
  gblt_not_eq(a0,x1,scan__sname_7_interrupt);
  unify(a6, a5);
  x2 = NILATOM;
  unify_value(a2, x2);
  unify(a4, a1);
  unify_value(a3, a0);
  proceed();
 scan__sname_7_3:
  if (!isref(a0)) goto scan__sname_7_interrupt;
  deref_and_jump(a0,scan__sname_7_1);
  *reasonp++ =  a0;
  goto scan__sname_7_interrupt;
 scan__sname_7_ext_interrupt:
  reasonp = 0l;
 scan__sname_7_interrupt:
  toppred = &predicate_scanner_xscan__sname_7;
  goto interrupt_7;
 }

 scan__number_9_top: {
  q x0, x1, x2, x3, x4;
  a1 = qp->args[1];
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  a6 = qp->args[6];
  a7 = qp->args[7];
  a8 = qp->args[8];
  qp = qp->next;
 scan__number_9_clear_reason:
  reasonp = reasons;
 scan__number_9_0:
 scan__number_9_1:
  switch (ptagof(a0)) {
 case ATOMIC:
 scan__number_9_2:
  switch ((unsigned long)a0) {
 case (unsigned long) makeint(2L):
  allocp[0] = x1 = makeref(&allocp[0]);
  allocp[1] = a1;
  x0 = makecons(&allocp[0]);
  allocp += 2;
  unify_value(a6, x0);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_scanner_xscan__number_9);
  allocp[2] = x2 = makeref(&allocp[2]);
  allocp[3] = x3 = makeref(&allocp[3]);
  allocp[4] = a2;
  allocp[5] = a3;
  allocp[6] = a4;
  allocp[7] = a5;
  allocp[8] = x1;
  allocp[9] = x4 = makeref(&allocp[9]);
  allocp[10] = a8;
  a0 = x3;
  a1 = x2;
  a2 = a7;
  a3 = x4;
  qp = (struct goalrec*)&allocp[0];
  allocp += 11;
  execute(getchar_4_0);
  goto getchar_4_ext_interrupt;
 case (unsigned long) makeint(6L):
  x0 = NILATOM;
  unify_value(a6, x0);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_scanner_xscan__number_2F9_241_8);
  allocp[2] = x1 = makeref(&allocp[2]);
  allocp[3] = a2;
  allocp[4] = x2 = makeref(&allocp[4]);
  allocp[5] = a3;
  allocp[6] = a4;
  allocp[7] = x3 = makeref(&allocp[7]);
  allocp[8] = a8;
  allocp[9] = x4 = makeref(&allocp[9]);
  allocp[10] = (q)(struct goalrec*)&allocp[0];
  allocp[11] = (q)(&predicate_scanner_xgetchar_4);
  allocp[12] = x2;
  allocp[13] = x4;
  allocp[14] = a7;
  allocp[15] = x3;
  a0 = a5;
  a1 = makeint(0L);
  a2 = x1;
  qp = (struct goalrec*)&allocp[10];
  allocp += 16;
  execute(char__list__to__int_3_0);
  goto char__list__to__int_3_ext_interrupt;
  };
  goto scan__number_9_3;
 case VARREF:
  deref_and_jump(a0,scan__number_9_1);
  *reasonp++ =  a0;
  goto scan__number_9_3;
  };
  goto scan__number_9_3;
 scan__number_9_3:
 scan__number_9_4:
  switch (ptagof(a1)) {
 case ATOMIC:
 scan__number_9_5:
  if (a1 != makeint(46L)) goto scan__number_9_6;
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_scanner_xscan__number_2F9_240_9);
  allocp[2] = x0 = makeref(&allocp[2]);
  allocp[3] = a6;
  allocp[4] = x1 = makeref(&allocp[4]);
  allocp[5] = a2;
  allocp[6] = a3;
  allocp[7] = a4;
  allocp[8] = a5;
  allocp[9] = x2 = makeref(&allocp[9]);
  allocp[10] = a8;
  a0 = x1;
  a1 = x0;
  a2 = a7;
  a3 = x2;
  qp = (struct goalrec*)&allocp[0];
  allocp += 11;
  execute(getchar_4_clear_reason);
  goto getchar_4_ext_interrupt;
 case VARREF:
  deref_and_jump(a1,scan__number_9_4);
  *reasonp++ =  a1;
  goto scan__number_9_6;
  };
  goto scan__number_9_6;
 scan__number_9_6:
  otherwise(scan__number_9_interrupt);
  unify3(a8, a7, a4, a0, a3, a1);
  x0 = NILATOM;
  unify_value(a6, x0);
  a0 = a5;
  a1 = makeint(0L);
  execute(char__list__to__int_3_0);
  goto char__list__to__int_3_ext_interrupt;
 scan__number_9_ext_interrupt:
  reasonp = 0l;
 scan__number_9_interrupt:
  toppred = &predicate_scanner_xscan__number_9;
  goto interrupt_9;
 }

 scan__number_2F9_241_8_top: {
  q x0, x1, x2, x3, x4, x5, x6;
  a1 = qp->args[1];
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  a6 = qp->args[6];
  a7 = qp->args[7];
  qp = qp->next;
 scan__number_2F9_241_8_clear_reason:
  reasonp = reasons;
 scan__number_2F9_241_8_0:
 scan__number_2F9_241_8_1:
  if (!isint(a0)) goto scan__number_2F9_241_8_3;
  x0 = makeint(0L);
  gblt_eq(a0,x0,scan__number_2F9_241_8_2);
  unify(a1, a2);
  a0 = a3;
  a1 = a4;
  a2 = a5;
  a3 = a6;
  execute(getchar_4_0);
  goto getchar_4_ext_interrupt;
 scan__number_2F9_241_8_2:
  x1 = makeint(0L);
  gblt_not_eq(a0,x1,scan__number_2F9_241_8_interrupt);
  x2 = a7;
  x3 = a2;
  a2 = a0;
  x4 = a1;
  x5 = a3;
  x6 = a4;
  a7 = a5;
  a8 = a6;
  a6 = x6;
  a5 = x5;
  a4 = x4;
  a3 = makeint(0L);
  a1 = x3;
  a0 = x2;
  execute(scan__based__number_9_0);
  goto scan__based__number_9_ext_interrupt;
 scan__number_2F9_241_8_3:
  if (!isref(a0)) goto scan__number_2F9_241_8_interrupt;
  deref_and_jump(a0,scan__number_2F9_241_8_1);
  *reasonp++ =  a0;
  goto scan__number_2F9_241_8_interrupt;
 scan__number_2F9_241_8_ext_interrupt:
  reasonp = 0l;
 scan__number_2F9_241_8_interrupt:
  goto interrupt_8;
 }

 scan__number_2F9_240_9_top: {
  q x0, x1, x2, x3, x4;
  a1 = qp->args[1];
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  a6 = qp->args[6];
  a7 = qp->args[7];
  a8 = qp->args[8];
  qp = qp->next;
 scan__number_2F9_240_9_clear_reason:
  reasonp = reasons;
 scan__number_2F9_240_9_0:
 scan__number_2F9_240_9_1:
  if (!isint(a0)) goto scan__number_2F9_240_9_3;
  x0 = makeint(2L);
  gblt_eq(a0,x0,scan__number_2F9_240_9_2);
  allocp[0] = x2 = makeref(&allocp[0]);
  allocp[1] = makeint(46L);
  x1 = makecons(&allocp[0]);
  allocp += 2;
  unify_value(a1, x1);
  a1 = a2;
  a2 = a3;
  a3 = a4;
  a4 = a5;
  a5 = a6;
  a6 = x2;
  execute(scan__fraction_9_0);
  goto scan__fraction_9_ext_interrupt;
 scan__number_2F9_240_9_2:
  x1 = makeint(2L);
  gblt_not_eq(a0,x1,scan__number_2F9_240_9_interrupt);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_scanner_xchar__list__to__int_3);
  allocp[2] = a6;
  allocp[3] = makeint(0L);
  allocp[4] = a3;
  x2 = NILATOM;
  qp = (struct goalrec*)&allocp[0];
  allocp += 5;
  unify_value(a1, x2);
  x3 = makeint(46L);
  unify_value(a4, x3);
  x4 = makeint(4L);
  unify_value(a5, x4);
  a0 = a2;
  a1 = a7;
  a2 = a8;
  execute(ungetc_3_0);
  goto ungetc_3_ext_interrupt;
 scan__number_2F9_240_9_3:
  if (!isref(a0)) goto scan__number_2F9_240_9_interrupt;
  deref_and_jump(a0,scan__number_2F9_240_9_1);
  *reasonp++ =  a0;
  goto scan__number_2F9_240_9_interrupt;
 scan__number_2F9_240_9_ext_interrupt:
  reasonp = 0l;
 scan__number_2F9_240_9_interrupt:
  goto interrupt_9;
 }

 scan__fraction_9_top: {
  q x0, x1, x2, x3, x4;
  a1 = qp->args[1];
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  a6 = qp->args[6];
  a7 = qp->args[7];
  a8 = qp->args[8];
  qp = qp->next;
 scan__fraction_9_clear_reason:
  reasonp = reasons;
 scan__fraction_9_0:
 scan__fraction_9_1:
  switch (ptagof(a0)) {
 case ATOMIC:
 scan__fraction_9_2:
  if (a0 != makeint(2L)) goto scan__fraction_9_3;
  allocp[0] = x1 = makeref(&allocp[0]);
  allocp[1] = a1;
  x0 = makecons(&allocp[0]);
  allocp += 2;
  unify_value(a6, x0);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_scanner_xscan__fraction_9);
  allocp[2] = x2 = makeref(&allocp[2]);
  allocp[3] = x3 = makeref(&allocp[3]);
  allocp[4] = a2;
  allocp[5] = a3;
  allocp[6] = a4;
  allocp[7] = a5;
  allocp[8] = x1;
  allocp[9] = x4 = makeref(&allocp[9]);
  allocp[10] = a8;
  a0 = x3;
  a1 = x2;
  a2 = a7;
  a3 = x4;
  qp = (struct goalrec*)&allocp[0];
  allocp += 11;
  execute(getchar_4_0);
  goto getchar_4_ext_interrupt;
 case VARREF:
  deref_and_jump(a0,scan__fraction_9_1);
  *reasonp++ =  a0;
  goto scan__fraction_9_3;
  };
  goto scan__fraction_9_3;
 scan__fraction_9_3:
 scan__fraction_9_4:
  switch (ptagof(a1)) {
 case ATOMIC:
 scan__fraction_9_5:
  switch ((unsigned long)a1) {
 case (unsigned long) makeint(101L):
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_scanner_xscan__fraction_2F9_240_11);
  allocp[2] = x0 = makeref(&allocp[2]);
  allocp[3] = x1 = makeref(&allocp[3]);
  allocp[4] = a6;
  allocp[5] = a1;
  allocp[6] = a2;
  allocp[7] = a3;
  allocp[8] = a4;
  allocp[9] = a5;
  allocp[10] = x2 = makeref(&allocp[10]);
  allocp[11] = a8;
  allocp[12] = a0;
  a0 = x1;
  a1 = x0;
  a2 = a7;
  a3 = x2;
  qp = (struct goalrec*)&allocp[0];
  allocp += 13;
  execute(getchar_4_clear_reason);
  goto getchar_4_ext_interrupt;
 case (unsigned long) makeint(69L):
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_scanner_xscan__fraction_2F9_240_11);
  allocp[2] = x0 = makeref(&allocp[2]);
  allocp[3] = x1 = makeref(&allocp[3]);
  allocp[4] = a6;
  allocp[5] = a1;
  allocp[6] = a2;
  allocp[7] = a3;
  allocp[8] = a4;
  allocp[9] = a5;
  allocp[10] = x2 = makeref(&allocp[10]);
  allocp[11] = a8;
  allocp[12] = a0;
  a0 = x1;
  a1 = x0;
  a2 = a7;
  a3 = x2;
  qp = (struct goalrec*)&allocp[0];
  allocp += 13;
  execute(getchar_4_clear_reason);
  goto getchar_4_ext_interrupt;
  };
  goto scan__fraction_9_6;
 case VARREF:
  deref_and_jump(a1,scan__fraction_9_4);
  *reasonp++ =  a1;
  goto scan__fraction_9_6;
  };
  goto scan__fraction_9_6;
 scan__fraction_9_6:
  otherwise(scan__fraction_9_interrupt);
  unify3(a8, a7, a4, a0, a3, a1);
  a0 = a5;
  a1 = a6;
  execute(char__list__to__float_3_0);
  goto char__list__to__float_3_ext_interrupt;
 scan__fraction_9_ext_interrupt:
  reasonp = 0l;
 scan__fraction_9_interrupt:
  toppred = &predicate_scanner_xscan__fraction_9;
  goto interrupt_9;
 }

 scan__fraction_2F9_240_11_top: {
  q x0, x1;
  a1 = qp->args[1];
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
 scan__fraction_2F9_240_11_clear_reason:
  reasonp = reasons;
 scan__fraction_2F9_240_11_0:
 scan__fraction_2F9_240_11_1:
  switch (ptagof(a0)) {
 case ATOMIC:
 scan__fraction_2F9_240_11_2:
  if (a0 != makeint(2L)) goto scan__fraction_2F9_240_11_3;
  allocp[0] = x1 = makeref(&allocp[0]);
  allocp[1] = a3;
  x0 = makecons(&allocp[0]);
  allocp += 2;
  unify_value(a2, x0);
  a2 = a4;
  a3 = a5;
  a4 = a6;
  a5 = a7;
  a6 = x1;
  a7 = a8;
  a8 = a9;
  execute(scan__exponent_9_0);
  goto scan__exponent_9_ext_interrupt;
 case VARREF:
  deref_and_jump(a0,scan__fraction_2F9_240_11_1);
  *reasonp++ =  a0;
  goto scan__fraction_2F9_240_11_3;
  };
  goto scan__fraction_2F9_240_11_3;
 scan__fraction_2F9_240_11_3:
 scan__fraction_2F9_240_11_4:
  switch (ptagof(a1)) {
 case ATOMIC:
 scan__fraction_2F9_240_11_5:
  switch ((unsigned long)a1) {
 case (unsigned long) makeint(45L):
  allocp[0] = x1 = makeref(&allocp[0]);
  allocp[1] = a3;
  x0 = makecons(&allocp[0]);
  allocp += 2;
  unify_value(a2, x0);
  a2 = a4;
  a3 = a5;
  a4 = a6;
  a5 = a7;
  a6 = x1;
  a7 = a8;
  a8 = a9;
  execute(scan__exponent_9_clear_reason);
  goto scan__exponent_9_ext_interrupt;
 case (unsigned long) makeint(43L):
  allocp[0] = x1 = makeref(&allocp[0]);
  allocp[1] = a3;
  x0 = makecons(&allocp[0]);
  allocp += 2;
  unify_value(a2, x0);
  a2 = a4;
  a3 = a5;
  a4 = a6;
  a5 = a7;
  a6 = x1;
  a7 = a8;
  a8 = a9;
  execute(scan__exponent_9_clear_reason);
  goto scan__exponent_9_ext_interrupt;
  };
  goto scan__fraction_2F9_240_11_6;
 case VARREF:
  deref_and_jump(a1,scan__fraction_2F9_240_11_4);
  *reasonp++ =  a1;
  goto scan__fraction_2F9_240_11_6;
  };
  goto scan__fraction_2F9_240_11_6;
 scan__fraction_2F9_240_11_6:
  otherwise(scan__fraction_2F9_240_11_interrupt);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_scanner_xchar__list__to__float_3);
  allocp[2] = a7;
  allocp[3] = a2;
  allocp[4] = a4;
  qp = (struct goalrec*)&allocp[0];
  allocp += 5;
  unify2(a5, a3, a6, a10);
  a0 = a1;
  a1 = a8;
  a2 = a9;
  execute(ungetc_3_0);
  goto ungetc_3_ext_interrupt;
 scan__fraction_2F9_240_11_ext_interrupt:
  reasonp = 0l;
 scan__fraction_2F9_240_11_interrupt:
  goto interrupt_11;
 }

 scan__exponent_9_top: {
  q x0, x1, x2, x3, x4;
  a1 = qp->args[1];
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  a6 = qp->args[6];
  a7 = qp->args[7];
  a8 = qp->args[8];
  qp = qp->next;
 scan__exponent_9_clear_reason:
  reasonp = reasons;
 scan__exponent_9_0:
 scan__exponent_9_1:
  switch (ptagof(a1)) {
 case ATOMIC:
 scan__exponent_9_2:
  switch ((unsigned long)a1) {
 case (unsigned long) makeint(45L):
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_scanner_xscan__unsigned__exponent_9);
  allocp[2] = x0 = makeref(&allocp[2]);
  allocp[3] = x1 = makeref(&allocp[3]);
  allocp[4] = a2;
  allocp[5] = a3;
  allocp[6] = a4;
  allocp[7] = a5;
  allocp[8] = x2 = makeref(&allocp[8]);
  allocp[9] = x3 = makeref(&allocp[9]);
  allocp[10] = a8;
  allocp[11] = x2;
  allocp[12] = a1;
  x4 = makecons(&allocp[11]);
  qp = (struct goalrec*)&allocp[0];
  allocp += 13;
  unify_value(a6, x4);
  a0 = x1;
  a1 = x0;
  a2 = a7;
  a3 = x3;
  execute(getchar_4_0);
  goto getchar_4_ext_interrupt;
 case (unsigned long) makeint(43L):
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_scanner_xscan__unsigned__exponent_9);
  allocp[2] = x0 = makeref(&allocp[2]);
  allocp[3] = x1 = makeref(&allocp[3]);
  allocp[4] = a2;
  allocp[5] = a3;
  allocp[6] = a4;
  allocp[7] = a5;
  allocp[8] = x2 = makeref(&allocp[8]);
  allocp[9] = x3 = makeref(&allocp[9]);
  allocp[10] = a8;
  allocp[11] = x2;
  allocp[12] = a1;
  x4 = makecons(&allocp[11]);
  qp = (struct goalrec*)&allocp[0];
  allocp += 13;
  unify_value(a6, x4);
  a0 = x1;
  a1 = x0;
  a2 = a7;
  a3 = x3;
  execute(getchar_4_0);
  goto getchar_4_ext_interrupt;
  };
  goto scan__exponent_9_3;
 case VARREF:
  deref_and_jump(a1,scan__exponent_9_1);
  *reasonp++ =  a1;
  goto scan__exponent_9_3;
  };
  goto scan__exponent_9_3;
 scan__exponent_9_3:
  otherwise(scan__exponent_9_interrupt);
  execute(scan__unsigned__exponent_9_0);
  goto scan__unsigned__exponent_9_ext_interrupt;
 scan__exponent_9_ext_interrupt:
  reasonp = 0l;
 scan__exponent_9_interrupt:
  toppred = &predicate_scanner_xscan__exponent_9;
  goto interrupt_9;
 }

 scan__unsigned__exponent_9_top: {
  q x0, x1, x2, x3, x4, x5;
  a1 = qp->args[1];
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  a6 = qp->args[6];
  a7 = qp->args[7];
  a8 = qp->args[8];
  qp = qp->next;
 scan__unsigned__exponent_9_clear_reason:
  reasonp = reasons;
 scan__unsigned__exponent_9_0:
 scan__unsigned__exponent_9_1:
  if (!isint(a0)) goto scan__unsigned__exponent_9_3;
  x0 = makeint(2L);
  gblt_eq(a0,x0,scan__unsigned__exponent_9_2);
  allocp[0] = x2 = makeref(&allocp[0]);
  allocp[1] = a1;
  x1 = makecons(&allocp[0]);
  allocp += 2;
  unify_value(a6, x1);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_scanner_xscan__unsigned__exponent_9);
  allocp[2] = x3 = makeref(&allocp[2]);
  allocp[3] = x4 = makeref(&allocp[3]);
  allocp[4] = a2;
  allocp[5] = a3;
  allocp[6] = a4;
  allocp[7] = a5;
  allocp[8] = x2;
  allocp[9] = x5 = makeref(&allocp[9]);
  allocp[10] = a8;
  a0 = x4;
  a1 = x3;
  a2 = a7;
  a3 = x5;
  qp = (struct goalrec*)&allocp[0];
  allocp += 11;
  execute(getchar_4_0);
  goto getchar_4_ext_interrupt;
 scan__unsigned__exponent_9_2:
  x1 = makeint(2L);
  gblt_not_eq(a0,x1,scan__unsigned__exponent_9_interrupt);
  unify(a8, a7);
  unify_value(a4, a0);
  unify(a3, a1);
  a0 = a5;
  a1 = a6;
  execute(char__list__to__float_3_0);
  goto char__list__to__float_3_ext_interrupt;
 scan__unsigned__exponent_9_3:
  if (!isref(a0)) goto scan__unsigned__exponent_9_interrupt;
  deref_and_jump(a0,scan__unsigned__exponent_9_1);
  *reasonp++ =  a0;
  goto scan__unsigned__exponent_9_interrupt;
 scan__unsigned__exponent_9_ext_interrupt:
  reasonp = 0l;
 scan__unsigned__exponent_9_interrupt:
  toppred = &predicate_scanner_xscan__unsigned__exponent_9;
  goto interrupt_9;
 }

 char__list__to__int_3_top: {
  q x0, x1, x2, x3, x4, x5;
  a1 = qp->args[1];
  a2 = qp->args[2];
  qp = qp->next;
 char__list__to__int_3_clear_reason:
  reasonp = reasons;
 char__list__to__int_3_0:
 char__list__to__int_3_1:
  switch (ptagof(a0)) {
 case CONS:
  x0 = makeint(10L);
  bblt_mult_no_check_x(x0,a1,x1);
  x2 = car_of(a0);
  x3 = makeint(48L);
  bblt_sub_no_check_y(x2,x3,x4);
  bblt_add(x1,x4,x5);
  a0 = cdr_of(a0);
  a1 = x5;
  execute(char__list__to__int_3_0);
  goto char__list__to__int_3_ext_interrupt;
 case ATOMIC:
  if (a0 != NILATOM) goto char__list__to__int_3_interrupt;
  unify(a2, a1);
  proceed();
 case VARREF:
  deref_and_jump(a0,char__list__to__int_3_1);
  *reasonp++ =  a0;
  goto char__list__to__int_3_interrupt;
  };
  goto char__list__to__int_3_interrupt;
 char__list__to__int_3_ext_interrupt:
  reasonp = 0l;
 char__list__to__int_3_interrupt:
  toppred = &predicate_scanner_xchar__list__to__int_3;
  goto interrupt_3;
 }

 char__list__to__float_3_top: {
  q x0, x1, x2, x3;
  a1 = qp->args[1];
  a2 = qp->args[2];
  qp = qp->next;
 char__list__to__float_3_clear_reason:
  reasonp = reasons;
 char__list__to__float_3_0:
  x0 = makecons(cons_const_4);
  unify_value(a1, x0);
  generic_arg[0] = a0;
  x1 = makeint(8L);
  generic_arg[1] = x1;
  new_generic(string_g_new, 2, x2, 0);
  generic_arg[0] = x2;
  new_generic(float_g_new, 1, x3, 0);
  unify(a2, x3);
  proceed();
 char__list__to__float_3_ext_interrupt:
  reasonp = 0l;
 char__list__to__float_3_interrupt:
  toppred = &predicate_scanner_xchar__list__to__float_3;
  goto interrupt_3;
 }

 scan__based__number_9_top: {
  q x0, x1, x2, x3, x4;
  a1 = qp->args[1];
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  a6 = qp->args[6];
  a7 = qp->args[7];
  a8 = qp->args[8];
  qp = qp->next;
 scan__based__number_9_clear_reason:
  reasonp = reasons;
 scan__based__number_9_0:
 scan__based__number_9_1:
  switch (ptagof(a0)) {
 case ATOMIC:
 scan__based__number_9_2:
  switch ((unsigned long)a0) {
 case (unsigned long) makeint(2L):
  x0 = makeint(48L);
  bblt_sub_no_check_y(a1,x0,x1);
  a0 = x1;
  x2 = a2;
  a2 = a3;
  a3 = a7;
  a7 = a8;
  a8 = a1;
  a1 = x2;
  execute(scan__based__number_2F9_242_9_0);
  goto scan__based__number_2F9_242_9_ext_interrupt;
 case (unsigned long) makeint(0L):
  x0 = makeint(97L);
  bblt_sub_no_check_y(a1,x0,x1);
  x2 = makeint(10L);
  bblt_add_no_check_y(x1,x2,x3);
  a0 = x3;
  x4 = a2;
  a2 = a3;
  a3 = a7;
  a7 = a8;
  a8 = a1;
  a1 = x4;
  execute(scan__based__number_2F9_241_9_0);
  goto scan__based__number_2F9_241_9_ext_interrupt;
 case (unsigned long) makeint(1L):
  x0 = makeint(65L);
  bblt_sub_no_check_y(a1,x0,x1);
  x2 = makeint(10L);
  bblt_add_no_check_y(x1,x2,x3);
  a0 = x3;
  x4 = a2;
  a2 = a3;
  a3 = a7;
  a7 = a8;
  a8 = a1;
  a1 = x4;
  execute(scan__based__number_2F9_240_9_0);
  goto scan__based__number_2F9_240_9_ext_interrupt;
  };
  goto scan__based__number_9_3;
 case VARREF:
  deref_and_jump(a0,scan__based__number_9_1);
  *reasonp++ =  a0;
  goto scan__based__number_9_3;
  };
  goto scan__based__number_9_3;
 scan__based__number_9_3:
  otherwise(scan__based__number_9_interrupt);
  unify4(a8, a7, a4, a3, a5, a1, a6, a0);
  proceed();
 scan__based__number_9_ext_interrupt:
  reasonp = 0l;
 scan__based__number_9_interrupt:
  toppred = &predicate_scanner_xscan__based__number_9;
  goto interrupt_9;
 }

 scan__based__number_2F9_242_9_top: {
  q x0, x1, x2, x3, x4;
  a1 = qp->args[1];
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  a6 = qp->args[6];
  a7 = qp->args[7];
  a8 = qp->args[8];
  qp = qp->next;
 scan__based__number_2F9_242_9_clear_reason:
  reasonp = reasons;
 scan__based__number_2F9_242_9_0:
 scan__based__number_2F9_242_9_1:
  if (!isint(a0)) goto scan__based__number_2F9_242_9_5;
 scan__based__number_2F9_242_9_2:
  if (!isint(a1)) goto scan__based__number_2F9_242_9_4;
  gblt_less(a0,a1,scan__based__number_2F9_242_9_3);
  bblt_mult_no_check_x(a1,a2,x0);
  bblt_add_no_check_y(x0,a0,x1);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_scanner_xscan__based__number_9);
  allocp[2] = x2 = makeref(&allocp[2]);
  allocp[3] = x3 = makeref(&allocp[3]);
  allocp[4] = a1;
  allocp[5] = x1;
  allocp[6] = a4;
  allocp[7] = a5;
  allocp[8] = a6;
  allocp[9] = x4 = makeref(&allocp[9]);
  allocp[10] = a7;
  a0 = x3;
  a1 = x2;
  a2 = a3;
  a3 = x4;
  qp = (struct goalrec*)&allocp[0];
  allocp += 11;
  execute(getchar_4_0);
  goto getchar_4_ext_interrupt;
 scan__based__number_2F9_242_9_3:
  gblt_greater_or_eq(a0,a1,scan__based__number_2F9_242_9_interrupt);
  unify2(a4, a2, a5, a8);
  x0 = makeint(2L);
  unify_value(a6, x0);
  unify(a7, a3);
  proceed();
 scan__based__number_2F9_242_9_4:
  if (!isref(a1)) goto scan__based__number_2F9_242_9_interrupt;
  deref_and_jump(a1,scan__based__number_2F9_242_9_2);
  *reasonp++ =  a1;
  goto scan__based__number_2F9_242_9_interrupt;
 scan__based__number_2F9_242_9_5:
  if (!isref(a0)) goto scan__based__number_2F9_242_9_interrupt;
  deref_and_jump(a0,scan__based__number_2F9_242_9_1);
  *reasonp++ =  a0;
  goto scan__based__number_2F9_242_9_interrupt;
 scan__based__number_2F9_242_9_ext_interrupt:
  reasonp = 0l;
 scan__based__number_2F9_242_9_interrupt:
  toppred = &predicate_scanner_xscan__based__number_2F9_242_9;
  goto interrupt_9;
 }

 scan__based__number_2F9_241_9_top: {
  q x0, x1, x2, x3, x4;
  a1 = qp->args[1];
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  a6 = qp->args[6];
  a7 = qp->args[7];
  a8 = qp->args[8];
  qp = qp->next;
 scan__based__number_2F9_241_9_clear_reason:
  reasonp = reasons;
 scan__based__number_2F9_241_9_0:
 scan__based__number_2F9_241_9_1:
  if (!isint(a0)) goto scan__based__number_2F9_241_9_5;
 scan__based__number_2F9_241_9_2:
  if (!isint(a1)) goto scan__based__number_2F9_241_9_4;
  gblt_less(a0,a1,scan__based__number_2F9_241_9_3);
  bblt_mult_no_check_x(a1,a2,x0);
  bblt_add_no_check_y(x0,a0,x1);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_scanner_xscan__based__number_9);
  allocp[2] = x2 = makeref(&allocp[2]);
  allocp[3] = x3 = makeref(&allocp[3]);
  allocp[4] = a1;
  allocp[5] = x1;
  allocp[6] = a4;
  allocp[7] = a5;
  allocp[8] = a6;
  allocp[9] = x4 = makeref(&allocp[9]);
  allocp[10] = a7;
  a0 = x3;
  a1 = x2;
  a2 = a3;
  a3 = x4;
  qp = (struct goalrec*)&allocp[0];
  allocp += 11;
  execute(getchar_4_0);
  goto getchar_4_ext_interrupt;
 scan__based__number_2F9_241_9_3:
  gblt_greater_or_eq(a0,a1,scan__based__number_2F9_241_9_interrupt);
  unify2(a4, a2, a5, a8);
  x0 = makeint(0L);
  unify_value(a6, x0);
  unify(a7, a3);
  proceed();
 scan__based__number_2F9_241_9_4:
  if (!isref(a1)) goto scan__based__number_2F9_241_9_interrupt;
  deref_and_jump(a1,scan__based__number_2F9_241_9_2);
  *reasonp++ =  a1;
  goto scan__based__number_2F9_241_9_interrupt;
 scan__based__number_2F9_241_9_5:
  if (!isref(a0)) goto scan__based__number_2F9_241_9_interrupt;
  deref_and_jump(a0,scan__based__number_2F9_241_9_1);
  *reasonp++ =  a0;
  goto scan__based__number_2F9_241_9_interrupt;
 scan__based__number_2F9_241_9_ext_interrupt:
  reasonp = 0l;
 scan__based__number_2F9_241_9_interrupt:
  toppred = &predicate_scanner_xscan__based__number_2F9_241_9;
  goto interrupt_9;
 }

 scan__based__number_2F9_240_9_top: {
  q x0, x1, x2, x3, x4;
  a1 = qp->args[1];
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  a6 = qp->args[6];
  a7 = qp->args[7];
  a8 = qp->args[8];
  qp = qp->next;
 scan__based__number_2F9_240_9_clear_reason:
  reasonp = reasons;
 scan__based__number_2F9_240_9_0:
 scan__based__number_2F9_240_9_1:
  if (!isint(a0)) goto scan__based__number_2F9_240_9_5;
 scan__based__number_2F9_240_9_2:
  if (!isint(a1)) goto scan__based__number_2F9_240_9_4;
  gblt_less(a0,a1,scan__based__number_2F9_240_9_3);
  bblt_mult_no_check_x(a1,a2,x0);
  bblt_add_no_check_y(x0,a0,x1);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_scanner_xscan__based__number_9);
  allocp[2] = x2 = makeref(&allocp[2]);
  allocp[3] = x3 = makeref(&allocp[3]);
  allocp[4] = a1;
  allocp[5] = x1;
  allocp[6] = a4;
  allocp[7] = a5;
  allocp[8] = a6;
  allocp[9] = x4 = makeref(&allocp[9]);
  allocp[10] = a7;
  a0 = x3;
  a1 = x2;
  a2 = a3;
  a3 = x4;
  qp = (struct goalrec*)&allocp[0];
  allocp += 11;
  execute(getchar_4_0);
  goto getchar_4_ext_interrupt;
 scan__based__number_2F9_240_9_3:
  gblt_greater_or_eq(a0,a1,scan__based__number_2F9_240_9_interrupt);
  unify2(a4, a2, a5, a8);
  x0 = makeint(1L);
  unify_value(a6, x0);
  unify(a7, a3);
  proceed();
 scan__based__number_2F9_240_9_4:
  if (!isref(a1)) goto scan__based__number_2F9_240_9_interrupt;
  deref_and_jump(a1,scan__based__number_2F9_240_9_2);
  *reasonp++ =  a1;
  goto scan__based__number_2F9_240_9_interrupt;
 scan__based__number_2F9_240_9_5:
  if (!isref(a0)) goto scan__based__number_2F9_240_9_interrupt;
  deref_and_jump(a0,scan__based__number_2F9_240_9_1);
  *reasonp++ =  a0;
  goto scan__based__number_2F9_240_9_interrupt;
 scan__based__number_2F9_240_9_ext_interrupt:
  reasonp = 0l;
 scan__based__number_2F9_240_9_interrupt:
  toppred = &predicate_scanner_xscan__based__number_2F9_240_9;
  goto interrupt_9;
 }

 lookup__var_8_top: {
  q x0, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12;
  a1 = qp->args[1];
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  a6 = qp->args[6];
  a7 = qp->args[7];
  qp = qp->next;
 lookup__var_8_clear_reason:
  reasonp = reasons;
 lookup__var_8_0:
 lookup__var_8_1:
  switch (ptagof(a0)) {
 case CONS:
  x0 = car_of(a0);
 lookup__var_8_2:
  switch (ptagof(x0)) {
 case FUNCTOR:
  if (functor_of(x0) != makesym(functor__3D_2)) goto lookup__var_8_8;
  x1 = arg(x0, 1);
 lookup__var_8_3:
  switch (ptagof(x1)) {
 case FUNCTOR:
  if (functor_of(x1) != makesym(functor_var_4)) goto lookup__var_8_8;
  x2 = arg(x0, 0);
 lookup__var_8_4:
  if (isref(x2)) goto lookup__var_8_7;
 lookup__var_8_5:
  if (isref(x2)) goto lookup__var_8_6;
  if_not_equal(x2, a2, lookup__var_8_8);
  x3 = arg(x1, 0);
  unify(a4, x3);
  x4 = arg(x1, 1);
  unify(a3, x4);
  x5 = arg(x1, 2);
  unify(a5, x5);
  x6 = arg(x1, 3);
  x7 = makeint(1L);
  bblt_add_no_check_y(x6,x7,x8);
  allocp[0] = makesym(functor_var_4);
  allocp[1] = x3;
  allocp[2] = x4;
  allocp[3] = x5;
  allocp[4] = x8;
  x9 = makefunctor(&allocp[0]);
  allocp[5] = makesym(functor__3D_2);
  allocp[6] = x2;
  allocp[7] = x9;
  x10 = makefunctor(&allocp[5]);
  x12 = cdr_of(a0);
  allocp[8] = x12;
  allocp[9] = x10;
  x11 = makecons(&allocp[8]);
  allocp += 10;
  unify_value(a6, x11);
  unify(a7, a1);
  proceed();
 lookup__var_8_6:
  deref_and_jump(x2,lookup__var_8_5);
  *reasonp++ =  x2;
  goto lookup__var_8_8;
 lookup__var_8_7:
  deref_and_jump(x2,lookup__var_8_4);
  *reasonp++ =  x2;
  goto lookup__var_8_8;
 case VARREF:
  deref_and_jump(x1,lookup__var_8_3);
  *reasonp++ =  x1;
  goto lookup__var_8_8;
  };
  goto lookup__var_8_8;
 case VARREF:
  deref_and_jump(x0,lookup__var_8_2);
  *reasonp++ =  x0;
  goto lookup__var_8_8;
  };
  goto lookup__var_8_8;
 case ATOMIC:
  if (a0 != NILATOM) goto lookup__var_8_8;
  unify(a3, a1);
  allocp[0] = makesym(functor_var_4);
  allocp[1] = a4;
  allocp[2] = a1;
  allocp[3] = a5;
  allocp[4] = makeint(1L);
  x0 = makefunctor(&allocp[0]);
  allocp[5] = makesym(functor__3D_2);
  allocp[6] = a2;
  allocp[7] = x0;
  x1 = makefunctor(&allocp[5]);
  allocp[8] = NILATOM;
  allocp[9] = x1;
  x2 = makecons(&allocp[8]);
  allocp += 10;
  unify_value(a6, x2);
  x3 = makeint(1L);
  bblt_add_no_check_y(a1,x3,x4);
  unify(a7, x4);
  proceed();
 case VARREF:
  deref_and_jump(a0,lookup__var_8_1);
  *reasonp++ =  a0;
  goto lookup__var_8_8;
  };
  goto lookup__var_8_8;
 lookup__var_8_8:
  otherwise(lookup__var_8_interrupt);
 lookup__var_8_9:
  switch (ptagof(a0)) {
 case CONS:
  x0 = car_of(a0);
 lookup__var_8_10:
  switch (ptagof(x0)) {
 case FUNCTOR:
  if (functor_of(x0) != makesym(functor__3D_2)) goto lookup__var_8_interrupt;
  allocp[0] = x2 = makeref(&allocp[0]);
  allocp[1] = x0;
  x1 = makecons(&allocp[0]);
  allocp += 2;
  unify_value(a6, x1);
  a0 = cdr_of(a0);
  a6 = x2;
  execute(lookup__var_8_0);
  goto lookup__var_8_ext_interrupt;
 case VARREF:
  deref_and_jump(x0,lookup__var_8_10);
  *reasonp++ =  x0;
  goto lookup__var_8_interrupt;
  };
  goto lookup__var_8_interrupt;
 case VARREF:
  deref_and_jump(a0,lookup__var_8_9);
  *reasonp++ =  a0;
  goto lookup__var_8_interrupt;
  };
  goto lookup__var_8_interrupt;
 lookup__var_8_ext_interrupt:
  reasonp = 0l;
 lookup__var_8_interrupt:
  goto interrupt_8;
 }

 finalize__vars_1_top: {
  q x0, x1, x2;
  qp = qp->next;
 finalize__vars_1_clear_reason:
  reasonp = reasons;
 finalize__vars_1_0:
 finalize__vars_1_1:
  switch (ptagof(a0)) {
 case CONS:
  x0 = car_of(a0);
 finalize__vars_1_2:
  switch (ptagof(x0)) {
 case FUNCTOR:
  if (functor_of(x0) != makesym(functor__3D_2)) goto finalize__vars_1_interrupt;
  x1 = arg(x0, 1);
 finalize__vars_1_3:
  switch (ptagof(x1)) {
 case FUNCTOR:
  if (functor_of(x1) != makesym(functor_var_4)) goto finalize__vars_1_interrupt;
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_scanner_xfinalize__vars_1);
  x2 = cdr_of(a0);
  allocp[2] = x2;
  a0 = arg(x1, 3);
  a1 = arg(x1, 2);
  qp = (struct goalrec*)&allocp[0];
  allocp += 3;
  execute(finalize__vars_2F1_240_2_0);
  goto finalize__vars_2F1_240_2_ext_interrupt;
 case VARREF:
  deref_and_jump(x1,finalize__vars_1_3);
  *reasonp++ =  x1;
  goto finalize__vars_1_interrupt;
  };
  goto finalize__vars_1_interrupt;
 case VARREF:
  deref_and_jump(x0,finalize__vars_1_2);
  *reasonp++ =  x0;
  goto finalize__vars_1_interrupt;
  };
  goto finalize__vars_1_interrupt;
 case ATOMIC:
  if (a0 != NILATOM) goto finalize__vars_1_interrupt;
  proceed();
 case VARREF:
  deref_and_jump(a0,finalize__vars_1_1);
  *reasonp++ =  a0;
  goto finalize__vars_1_interrupt;
  };
  goto finalize__vars_1_interrupt;
 finalize__vars_1_ext_interrupt:
  reasonp = 0l;
 finalize__vars_1_interrupt:
  toppred = &predicate_scanner_xfinalize__vars_1;
  goto interrupt_1;
 }

 finalize__vars_2F1_240_2_top: {
  q x0, x1, x2;
  a1 = qp->args[1];
  qp = qp->next;
 finalize__vars_2F1_240_2_clear_reason:
  reasonp = reasons;
 finalize__vars_2F1_240_2_0:
 finalize__vars_2F1_240_2_1:
  if (!isint(a0)) goto finalize__vars_2F1_240_2_3;
  x0 = makeint(1L);
  gblt_greater(a0,x0,finalize__vars_2F1_240_2_2);
  x1 = makesym(atom_normal);
  unify_value(a1, x1);
  proceed();
 finalize__vars_2F1_240_2_2:
  if (a0 != makeint(1L)) goto finalize__vars_2F1_240_2_interrupt;
  x2 = makesym(atom_void);
  unify_value(a1, x2);
  proceed();
 finalize__vars_2F1_240_2_3:
  if (!isref(a0)) goto finalize__vars_2F1_240_2_interrupt;
  deref_and_jump(a0,finalize__vars_2F1_240_2_1);
  *reasonp++ =  a0;
  goto finalize__vars_2F1_240_2_interrupt;
 finalize__vars_2F1_240_2_ext_interrupt:
  reasonp = 0l;
 finalize__vars_2F1_240_2_interrupt:
  toppred = &predicate_scanner_xfinalize__vars_2F1_240_2;
  goto interrupt_2;
 }

 scan__string_8_top: {
  q x0, x1, x2, x3;
  a1 = qp->args[1];
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  a6 = qp->args[6];
  a7 = qp->args[7];
  qp = qp->next;
 scan__string_8_clear_reason:
  reasonp = reasons;
 scan__string_8_0:
 scan__string_8_1:
  switch (ptagof(a0)) {
 case ATOMIC:
 scan__string_8_2:
  switch ((unsigned long)a0) {
 case (unsigned long) makeint(92L):
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_scanner_xscan__escape_8);
  allocp[2] = x0 = makeref(&allocp[2]);
  allocp[3] = a1;
  allocp[4] = a2;
  allocp[5] = a3;
  allocp[6] = a4;
  allocp[7] = a5;
  allocp[8] = x1 = makeref(&allocp[8]);
  allocp[9] = a7;
  allocp[10] = (q)(struct goalrec*)&allocp[0];
  allocp[11] = (q)(&predicate_scanner_xescape__char_2);
  allocp[12] = x2 = makeref(&allocp[12]);
  allocp[13] = x0;
  a0 = x2;
  a1 = a6;
  a2 = x1;
  qp = (struct goalrec*)&allocp[10];
  allocp += 14;
  execute(get0_3_0);
  goto get0_3_ext_interrupt;
 case (unsigned long) makeint(-1L):
  unify(a7, a6);
  x0 = makefunctor(&string_const_1);
  unify_value(a5, x0);
  x1 = NILATOM;
  unify_value(a2, x1);
  x2 = makeint(-1L);
  unify_value(a3, x2);
  x3 = makeint(21L);
  unify_value(a4, x3);
  proceed();
  };
  goto scan__string_8_3;
 case VARREF:
  deref_and_jump(a0,scan__string_8_1);
  *reasonp++ =  a0;
  goto scan__string_8_6;
  };
  goto scan__string_8_3;
 scan__string_8_3:
 scan__string_8_4:
  if (isref(a0)) goto scan__string_8_5;
  if_not_equal(a0, a1, scan__string_8_6);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_scanner_xscan__string_2F8_240_8);
  allocp[2] = x0 = makeref(&allocp[2]);
  allocp[3] = a0;
  allocp[4] = a2;
  allocp[5] = x1 = makeref(&allocp[5]);
  allocp[6] = a3;
  allocp[7] = a4;
  allocp[8] = a5;
  allocp[9] = a7;
  a0 = x0;
  a1 = a6;
  a2 = x1;
  qp = (struct goalrec*)&allocp[0];
  allocp += 10;
  execute(get0_3_clear_reason);
  goto get0_3_ext_interrupt;
 scan__string_8_5:
  deref_and_jump(a0,scan__string_8_4);
  *reasonp++ =  a0;
  goto scan__string_8_6;
 scan__string_8_6:
  otherwise(scan__string_8_interrupt);
  allocp[0] = x1 = makeref(&allocp[0]);
  allocp[1] = a0;
  x0 = makecons(&allocp[0]);
  allocp += 2;
  unify_value(a2, x0);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_scanner_xscan__string_8);
  allocp[2] = x2 = makeref(&allocp[2]);
  allocp[3] = a1;
  allocp[4] = x1;
  allocp[5] = a3;
  allocp[6] = a4;
  allocp[7] = a5;
  allocp[8] = x3 = makeref(&allocp[8]);
  allocp[9] = a7;
  a0 = x2;
  a1 = a6;
  a2 = x3;
  qp = (struct goalrec*)&allocp[0];
  allocp += 10;
  execute(get0_3_0);
  goto get0_3_ext_interrupt;
 scan__string_8_ext_interrupt:
  reasonp = 0l;
 scan__string_8_interrupt:
  toppred = &predicate_scanner_xscan__string_8;
  goto interrupt_8;
 }

 scan__string_2F8_240_8_top: {
  q x0, x1, x2, x3;
  a1 = qp->args[1];
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  a6 = qp->args[6];
  a7 = qp->args[7];
  qp = qp->next;
 scan__string_2F8_240_8_clear_reason:
  reasonp = reasons;
 scan__string_2F8_240_8_0:
 scan__string_2F8_240_8_1:
  if (!isint(a0)) goto scan__string_2F8_240_8_5;
 scan__string_2F8_240_8_2:
  if (!isint(a1)) goto scan__string_2F8_240_8_4;
  gblt_eq(a0,a1,scan__string_2F8_240_8_3);
  allocp[0] = x1 = makeref(&allocp[0]);
  allocp[1] = a1;
  x0 = makecons(&allocp[0]);
  allocp += 2;
  unify_value(a2, x0);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_scanner_xscan__string_8);
  allocp[2] = x2 = makeref(&allocp[2]);
  allocp[3] = a1;
  allocp[4] = x1;
  allocp[5] = a4;
  allocp[6] = a5;
  allocp[7] = a6;
  allocp[8] = x3 = makeref(&allocp[8]);
  allocp[9] = a7;
  a0 = x2;
  a1 = a3;
  a2 = x3;
  qp = (struct goalrec*)&allocp[0];
  allocp += 10;
  execute(get0_3_0);
  goto get0_3_ext_interrupt;
 scan__string_2F8_240_8_3:
  gblt_not_eq(a0,a1,scan__string_2F8_240_8_interrupt);
  x0 = NILATOM;
  unify_value(a2, x0);
  x1 = NILATOM;
  unify_value(a6, x1);
  unify_value(a4, a0);
  unify(a7, a3);
  a1 = a5;
  execute(chartype_2_0);
  goto chartype_2_ext_interrupt;
 scan__string_2F8_240_8_4:
  if (!isref(a1)) goto scan__string_2F8_240_8_interrupt;
  deref_and_jump(a1,scan__string_2F8_240_8_2);
  *reasonp++ =  a1;
  goto scan__string_2F8_240_8_interrupt;
 scan__string_2F8_240_8_5:
  if (!isref(a0)) goto scan__string_2F8_240_8_interrupt;
  deref_and_jump(a0,scan__string_2F8_240_8_1);
  *reasonp++ =  a0;
  goto scan__string_2F8_240_8_interrupt;
 scan__string_2F8_240_8_ext_interrupt:
  reasonp = 0l;
 scan__string_2F8_240_8_interrupt:
  goto interrupt_8;
 }

 escape__char_2_top: {
  q x0, x1, x2, x3, x4, x5;
  a1 = qp->args[1];
  qp = qp->next;
 escape__char_2_clear_reason:
  reasonp = reasons;
 escape__char_2_0:
 escape__char_2_1:
  if (!isint(a0)) goto escape__char_2_3;
  x0 = makeint(48L);
  gblt_less_or_eq(x0,a0,escape__char_2_2);
  x1 = makeint(56L);
  gblt_less(a0,x1,escape__char_2_2);
  x2 = makeint(48L);
  bblt_sub_no_check(a0,x2,x3);
  allocp[0] = makesym(functor_octal_1);
  allocp[1] = x3;
  x4 = makefunctor(&allocp[0]);
  allocp += 2;
  unify_value(a1, x4);
  proceed();
 escape__char_2_2:
  switch ((unsigned long)a0) {
 case (unsigned long) makeint(97L):
  x5 = makeint(7L);
  unify_value(a1, x5);
  proceed();
 case (unsigned long) makeint(98L):
  x5 = makeint(8L);
  unify_value(a1, x5);
  proceed();
 case (unsigned long) makeint(116L):
  x5 = makeint(9L);
  unify_value(a1, x5);
  proceed();
 case (unsigned long) makeint(110L):
  x5 = makeint(10L);
  unify_value(a1, x5);
  proceed();
 case (unsigned long) makeint(118L):
  x5 = makeint(11L);
  unify_value(a1, x5);
  proceed();
 case (unsigned long) makeint(102L):
  x5 = makeint(12L);
  unify_value(a1, x5);
  proceed();
 case (unsigned long) makeint(114L):
  x5 = makeint(13L);
  unify_value(a1, x5);
  proceed();
 case (unsigned long) makeint(39L):
  x5 = makeint(39L);
  unify_value(a1, x5);
  proceed();
 case (unsigned long) makeint(34L):
  x5 = makeint(34L);
  unify_value(a1, x5);
  proceed();
 case (unsigned long) makeint(63L):
  x5 = makeint(63L);
  unify_value(a1, x5);
  proceed();
 case (unsigned long) makeint(92L):
  x5 = makeint(92L);
  unify_value(a1, x5);
  proceed();
 case (unsigned long) makeint(120L):
  x5 = makesym(atom_hexadecimal);
  unify_value(a1, x5);
  proceed();
 case (unsigned long) makeint(10L):
  x5 = makesym(atom_escaped__newline);
  unify_value(a1, x5);
  proceed();
  };
  goto escape__char_2_4;
 escape__char_2_3:
  if (!isref(a0)) goto escape__char_2_4;
  deref_and_jump(a0,escape__char_2_1);
  *reasonp++ =  a0;
  goto escape__char_2_4;
 escape__char_2_4:
  otherwise(escape__char_2_interrupt);
  unify(a1, a0);
  proceed();
 escape__char_2_ext_interrupt:
  reasonp = 0l;
 escape__char_2_interrupt:
  goto interrupt_2;
 }

 scan__escape_8_top: {
  q x0, x1, x2, x3;
  a1 = qp->args[1];
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  a6 = qp->args[6];
  a7 = qp->args[7];
  qp = qp->next;
 scan__escape_8_clear_reason:
  reasonp = reasons;
 scan__escape_8_0:
 scan__escape_8_1:
  switch (ptagof(a0)) {
 case ATOMIC:
  if (isint(a0)) goto scan__escape_8_2;
  switch (symval(a0)) {
 case symval(makesym(atom_escaped__newline)):
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_scanner_xscan__string_8);
  allocp[2] = x0 = makeref(&allocp[2]);
  allocp[3] = a1;
  allocp[4] = a2;
  allocp[5] = a3;
  allocp[6] = a4;
  allocp[7] = a5;
  allocp[8] = x1 = makeref(&allocp[8]);
  allocp[9] = a7;
  a0 = x0;
  a1 = a6;
  a2 = x1;
  qp = (struct goalrec*)&allocp[0];
  allocp += 10;
  execute(get0_3_0);
  goto get0_3_ext_interrupt;
 case symval(makesym(atom_hexadecimal)):
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_scanner_xscan__hex__escape_9);
  allocp[2] = x0 = makeref(&allocp[2]);
  allocp[3] = makeint(0L);
  allocp[4] = a1;
  allocp[5] = a2;
  allocp[6] = a3;
  allocp[7] = a4;
  allocp[8] = a5;
  allocp[9] = x1 = makeref(&allocp[9]);
  allocp[10] = a7;
  a0 = x0;
  a1 = a6;
  a2 = x1;
  qp = (struct goalrec*)&allocp[0];
  allocp += 11;
  execute(get0_3_0);
  goto get0_3_ext_interrupt;
  };
  goto scan__escape_8_interrupt;
 scan__escape_8_2:
  gblt_integer(a0,scan__escape_8_interrupt);
  allocp[0] = x1 = makeref(&allocp[0]);
  allocp[1] = a0;
  x0 = makecons(&allocp[0]);
  allocp += 2;
  unify_value(a2, x0);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_scanner_xscan__string_8);
  allocp[2] = x2 = makeref(&allocp[2]);
  allocp[3] = a1;
  allocp[4] = x1;
  allocp[5] = a3;
  allocp[6] = a4;
  allocp[7] = a5;
  allocp[8] = x3 = makeref(&allocp[8]);
  allocp[9] = a7;
  a0 = x2;
  a1 = a6;
  a2 = x3;
  qp = (struct goalrec*)&allocp[0];
  allocp += 10;
  execute(get0_3_0);
  goto get0_3_ext_interrupt;
 case FUNCTOR:
  if (functor_of(a0) != makesym(functor_octal_1)) goto scan__escape_8_interrupt;
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_scanner_xscan__octal__escape_9);
  allocp[2] = x0 = makeref(&allocp[2]);
  x1 = arg(a0, 0);
  allocp[3] = x1;
  allocp[4] = a1;
  allocp[5] = a2;
  allocp[6] = a3;
  allocp[7] = a4;
  allocp[8] = a5;
  allocp[9] = x2 = makeref(&allocp[9]);
  allocp[10] = a7;
  a0 = x0;
  a1 = a6;
  a2 = x2;
  qp = (struct goalrec*)&allocp[0];
  allocp += 11;
  execute(get0_3_0);
  goto get0_3_ext_interrupt;
 case VARREF:
  deref_and_jump(a0,scan__escape_8_1);
  *reasonp++ =  a0;
  goto scan__escape_8_interrupt;
  };
  goto scan__escape_8_interrupt;
 scan__escape_8_ext_interrupt:
  reasonp = 0l;
 scan__escape_8_interrupt:
  goto interrupt_8;
 }

 scan__octal__escape_9_top: {
  q x0, x1, x2, x3, x4, x5, x6, x7, x8;
  a1 = qp->args[1];
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  a6 = qp->args[6];
  a7 = qp->args[7];
  a8 = qp->args[8];
  qp = qp->next;
 scan__octal__escape_9_clear_reason:
  reasonp = reasons;
 scan__octal__escape_9_0:
 scan__octal__escape_9_1:
  if (!isint(a0)) goto scan__octal__escape_9_2;
  x0 = makeint(48L);
  gblt_less_or_eq(x0,a0,scan__octal__escape_9_3);
  x1 = makeint(56L);
  gblt_less(a0,x1,scan__octal__escape_9_3);
  x2 = makeint(8L);
  bblt_mult_no_check_y(a1,x2,x3);
  bblt_add_no_check_y(x3,a0,x4);
  x5 = makeint(48L);
  bblt_sub_no_check_y(x4,x5,x6);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_scanner_xscan__octal__escape__2_9);
  allocp[2] = x7 = makeref(&allocp[2]);
  allocp[3] = x6;
  allocp[4] = a2;
  allocp[5] = a3;
  allocp[6] = a4;
  allocp[7] = a5;
  allocp[8] = a6;
  allocp[9] = x8 = makeref(&allocp[9]);
  allocp[10] = a8;
  a0 = x7;
  a1 = a7;
  a2 = x8;
  qp = (struct goalrec*)&allocp[0];
  allocp += 11;
  execute(get0_3_0);
  goto get0_3_ext_interrupt;
 scan__octal__escape_9_2:
  if (!isref(a0)) goto scan__octal__escape_9_3;
  deref_and_jump(a0,scan__octal__escape_9_1);
  *reasonp++ =  a0;
  goto scan__octal__escape_9_3;
 scan__octal__escape_9_3:
  otherwise(scan__octal__escape_9_interrupt);
  allocp[0] = x1 = makeref(&allocp[0]);
  allocp[1] = a1;
  x0 = makecons(&allocp[0]);
  allocp += 2;
  unify_value(a3, x0);
  a1 = a2;
  a2 = x1;
  a3 = a4;
  a4 = a5;
  a5 = a6;
  a6 = a7;
  a7 = a8;
  execute(scan__string_8_0);
  goto scan__string_8_ext_interrupt;
 scan__octal__escape_9_ext_interrupt:
  reasonp = 0l;
 scan__octal__escape_9_interrupt:
  goto interrupt_9;
 }

 scan__octal__escape__2_9_top: {
  q x0, x1, x2, x3, x4, x5, x6, x7;
  a1 = qp->args[1];
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  a6 = qp->args[6];
  a7 = qp->args[7];
  a8 = qp->args[8];
  qp = qp->next;
 scan__octal__escape__2_9_clear_reason:
  reasonp = reasons;
 scan__octal__escape__2_9_0:
 scan__octal__escape__2_9_1:
  if (!isint(a0)) goto scan__octal__escape__2_9_2;
  x0 = makeint(48L);
  gblt_less_or_eq(x0,a0,scan__octal__escape__2_9_3);
  x1 = makeint(56L);
  gblt_less(a0,x1,scan__octal__escape__2_9_3);
  x2 = makeint(8L);
  bblt_mult_no_check_y(a1,x2,x3);
  bblt_add_no_check_y(x3,a0,x4);
  x5 = makeint(48L);
  bblt_sub_no_check_y(x4,x5,x6);
  a1 = a3;
  x7 = a7;
  a3 = a2;
  a7 = a8;
  a8 = a0;
  a2 = x7;
  a0 = x6;
  execute(scan__octal__escape__2_2F9_240_9_0);
  goto scan__octal__escape__2_2F9_240_9_ext_interrupt;
 scan__octal__escape__2_9_2:
  if (!isref(a0)) goto scan__octal__escape__2_9_3;
  deref_and_jump(a0,scan__octal__escape__2_9_1);
  *reasonp++ =  a0;
  goto scan__octal__escape__2_9_3;
 scan__octal__escape__2_9_3:
  otherwise(scan__octal__escape__2_9_interrupt);
  allocp[0] = x1 = makeref(&allocp[0]);
  allocp[1] = a1;
  x0 = makecons(&allocp[0]);
  allocp += 2;
  unify_value(a3, x0);
  a1 = a2;
  a2 = x1;
  a3 = a4;
  a4 = a5;
  a5 = a6;
  a6 = a7;
  a7 = a8;
  execute(scan__string_8_0);
  goto scan__string_8_ext_interrupt;
 scan__octal__escape__2_9_ext_interrupt:
  reasonp = 0l;
 scan__octal__escape__2_9_interrupt:
  goto interrupt_9;
 }

 scan__octal__escape__2_2F9_240_9_top: {
  q x0, x1, x2, x3, x4;
  a1 = qp->args[1];
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  a6 = qp->args[6];
  a7 = qp->args[7];
  a8 = qp->args[8];
  qp = qp->next;
 scan__octal__escape__2_2F9_240_9_clear_reason:
  reasonp = reasons;
 scan__octal__escape__2_2F9_240_9_0:
 scan__octal__escape__2_2F9_240_9_1:
  if (!isint(a0)) goto scan__octal__escape__2_2F9_240_9_2;
  x0 = makeint(256L);
  gblt_less(a0,x0,scan__octal__escape__2_2F9_240_9_3);
  allocp[0] = x2 = makeref(&allocp[0]);
  allocp[1] = a0;
  x1 = makecons(&allocp[0]);
  allocp += 2;
  unify_value(a1, x1);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_scanner_xscan__string_8);
  allocp[2] = x3 = makeref(&allocp[2]);
  allocp[3] = a3;
  allocp[4] = x2;
  allocp[5] = a4;
  allocp[6] = a5;
  allocp[7] = a6;
  allocp[8] = x4 = makeref(&allocp[8]);
  allocp[9] = a7;
  a0 = x3;
  a1 = a2;
  a2 = x4;
  qp = (struct goalrec*)&allocp[0];
  allocp += 10;
  execute(get0_3_0);
  goto get0_3_ext_interrupt;
 scan__octal__escape__2_2F9_240_9_2:
  if (!isref(a0)) goto scan__octal__escape__2_2F9_240_9_3;
  deref_and_jump(a0,scan__octal__escape__2_2F9_240_9_1);
  *reasonp++ =  a0;
  goto scan__octal__escape__2_2F9_240_9_3;
 scan__octal__escape__2_2F9_240_9_3:
  otherwise(scan__octal__escape__2_2F9_240_9_interrupt);
  x0 = makefunctor(&string_const_3);
  unify_value(a6, x0);
  a0 = a8;
  x1 = a3;
  x2 = a1;
  a3 = a4;
  a4 = a5;
  allocp[0] = x3 = makeref(&allocp[0]);
  a5 = x3;
  a6 = a2;
  a2 = x2;
  a1 = x1;
  allocp += 1;
  execute(scan__string_8_0);
  goto scan__string_8_ext_interrupt;
 scan__octal__escape__2_2F9_240_9_ext_interrupt:
  reasonp = 0l;
 scan__octal__escape__2_2F9_240_9_interrupt:
  toppred = &predicate_scanner_xscan__octal__escape__2_2F9_240_9;
  goto interrupt_9;
 }

 scan__hex__escape_9_top: {
  q x0, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10;
  a1 = qp->args[1];
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  a6 = qp->args[6];
  a7 = qp->args[7];
  a8 = qp->args[8];
  qp = qp->next;
 scan__hex__escape_9_clear_reason:
  reasonp = reasons;
 scan__hex__escape_9_0:
 scan__hex__escape_9_1:
  if (!isint(a0)) goto scan__hex__escape_9_4;
  x0 = makeint(48L);
  gblt_less_or_eq(x0,a0,scan__hex__escape_9_2);
  x1 = makeint(57L);
  gblt_less_or_eq(a0,x1,scan__hex__escape_9_2);
  x2 = makeint(16L);
  bblt_mult_no_check_x(x2,a1,x3);
  bblt_add_no_check_y(x3,a0,x4);
  x5 = makeint(48L);
  bblt_sub_no_check_y(x4,x5,x6);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_scanner_xscan__hex__escape_9);
  allocp[2] = x7 = makeref(&allocp[2]);
  allocp[3] = x6;
  allocp[4] = a2;
  allocp[5] = a3;
  allocp[6] = a4;
  allocp[7] = a5;
  allocp[8] = a6;
  allocp[9] = x8 = makeref(&allocp[9]);
  allocp[10] = a8;
  a0 = x7;
  a1 = a7;
  a2 = x8;
  qp = (struct goalrec*)&allocp[0];
  allocp += 11;
  execute(get0_3_0);
  goto get0_3_ext_interrupt;
 scan__hex__escape_9_2:
  x1 = makeint(97L);
  gblt_less_or_eq(x1,a0,scan__hex__escape_9_3);
  x2 = makeint(102L);
  gblt_less_or_eq(a0,x2,scan__hex__escape_9_3);
  x3 = makeint(16L);
  bblt_mult_no_check_x(x3,a1,x4);
  bblt_add_no_check_y(x4,a0,x5);
  x6 = makeint(97L);
  bblt_sub_no_check_y(x5,x6,x7);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_scanner_xscan__hex__escape_9);
  allocp[2] = x8 = makeref(&allocp[2]);
  allocp[3] = x7;
  allocp[4] = a2;
  allocp[5] = a3;
  allocp[6] = a4;
  allocp[7] = a5;
  allocp[8] = a6;
  allocp[9] = x9 = makeref(&allocp[9]);
  allocp[10] = a8;
  a0 = x8;
  a1 = a7;
  a2 = x9;
  qp = (struct goalrec*)&allocp[0];
  allocp += 11;
  execute(get0_3_0);
  goto get0_3_ext_interrupt;
 scan__hex__escape_9_3:
  x2 = makeint(65L);
  gblt_less_or_eq(x2,a0,scan__hex__escape_9_5);
  x3 = makeint(70L);
  gblt_less_or_eq(a0,x3,scan__hex__escape_9_5);
  x4 = makeint(16L);
  bblt_mult_no_check_x(x4,a1,x5);
  bblt_add_no_check_y(x5,a0,x6);
  x7 = makeint(65L);
  bblt_sub_no_check_y(x6,x7,x8);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_scanner_xscan__hex__escape_9);
  allocp[2] = x9 = makeref(&allocp[2]);
  allocp[3] = x8;
  allocp[4] = a2;
  allocp[5] = a3;
  allocp[6] = a4;
  allocp[7] = a5;
  allocp[8] = a6;
  allocp[9] = x10 = makeref(&allocp[9]);
  allocp[10] = a8;
  a0 = x9;
  a1 = a7;
  a2 = x10;
  qp = (struct goalrec*)&allocp[0];
  allocp += 11;
  execute(get0_3_0);
  goto get0_3_ext_interrupt;
 scan__hex__escape_9_4:
  if (!isref(a0)) goto scan__hex__escape_9_5;
  deref_and_jump(a0,scan__hex__escape_9_1);
  *reasonp++ =  a0;
  goto scan__hex__escape_9_5;
 scan__hex__escape_9_5:
  otherwise(scan__hex__escape_9_interrupt);
 scan__hex__escape_9_6:
  if (!isint(a1)) goto scan__hex__escape_9_8;
  x0 = makeint(256L);
  gblt_greater_or_eq(a1,x0,scan__hex__escape_9_7);
  x1 = makefunctor(&string_const_2);
  unify_value(a6, x1);
  a1 = a2;
  a2 = a3;
  a3 = a4;
  a4 = a5;
  allocp[0] = x2 = makeref(&allocp[0]);
  a5 = x2;
  a6 = a7;
  a7 = a8;
  allocp += 1;
  execute(scan__string_8_0);
  goto scan__string_8_ext_interrupt;
 scan__hex__escape_9_7:
  x1 = makeint(256L);
  gblt_less(a1,x1,scan__hex__escape_9_interrupt);
  allocp[0] = x3 = makeref(&allocp[0]);
  allocp[1] = a1;
  x2 = makecons(&allocp[0]);
  allocp += 2;
  unify_value(a3, x2);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_scanner_xscan__string_8);
  allocp[2] = x4 = makeref(&allocp[2]);
  allocp[3] = a2;
  allocp[4] = a3;
  allocp[5] = a4;
  allocp[6] = a5;
  allocp[7] = a6;
  allocp[8] = x5 = makeref(&allocp[8]);
  allocp[9] = a8;
  a0 = x4;
  a1 = a7;
  a2 = x5;
  qp = (struct goalrec*)&allocp[0];
  allocp += 10;
  execute(get0_3_0);
  goto get0_3_ext_interrupt;
 scan__hex__escape_9_8:
  if (!isref(a1)) goto scan__hex__escape_9_interrupt;
  deref_and_jump(a1,scan__hex__escape_9_6);
  *reasonp++ =  a1;
  goto scan__hex__escape_9_interrupt;
 scan__hex__escape_9_ext_interrupt:
  reasonp = 0l;
 scan__hex__escape_9_interrupt:
  goto interrupt_9;
 }

 atom__chars_2_top: {
  q x0, x1;
  a1 = qp->args[1];
  qp = qp->next;
 atom__chars_2_clear_reason:
  reasonp = reasons;
 atom__chars_2_0:
  generic_arg[0] = a1;
  x0 = makeint(8L);
  generic_arg[1] = x0;
  new_generic(string_g_new, 2, x1, 0);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_atom__table_xmake__atom_2);
  allocp[2] = x1;
  allocp[3] = a0;
  qp = (struct goalrec*)&allocp[0];
  allocp += 4;
  proceed();
 atom__chars_2_ext_interrupt:
  reasonp = 0l;
 atom__chars_2_interrupt:
  toppred = &predicate_scanner_xatom__chars_2;
  goto interrupt_2;
 }

 make__new__string_2_top: {
  q x0, x1;
  a1 = qp->args[1];
  qp = qp->next;
 make__new__string_2_clear_reason:
  reasonp = reasons;
 make__new__string_2_0:
  generic_arg[0] = a1;
  x0 = makeint(8L);
  generic_arg[1] = x0;
  new_generic(string_g_new, 2, x1, 0);
  unify(a0, x1);
  proceed();
 make__new__string_2_ext_interrupt:
  reasonp = 0l;
 make__new__string_2_interrupt:
  toppred = &predicate_scanner_xmake__new__string_2;
  goto interrupt_2;
 }

 get0_3_top: {
  q x0, x1;
  a1 = qp->args[1];
  a2 = qp->args[2];
  qp = qp->next;
 get0_3_clear_reason:
  reasonp = reasons;
 get0_3_0:
  allocp[0] = makesym(functor_getc_1);
  allocp[1] = a0;
  x0 = makefunctor(&allocp[0]);
  allocp[2] = a2;
  allocp[3] = x0;
  x1 = makecons(&allocp[2]);
  allocp += 4;
  unify_value(a1, x1);
  proceed();
 get0_3_ext_interrupt:
  reasonp = 0l;
 get0_3_interrupt:
  toppred = &predicate_scanner_xget0_3;
  goto interrupt_3;
 }

 getchar_4_top: {
  a1 = qp->args[1];
  a2 = qp->args[2];
  a3 = qp->args[3];
  qp = qp->next;
 getchar_4_clear_reason:
  reasonp = reasons;
 getchar_4_0:
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_scanner_xchartype_2);
  allocp[2] = a0;
  allocp[3] = a1;
  a1 = a2;
  a2 = a3;
  qp = (struct goalrec*)&allocp[0];
  allocp += 4;
  execute(get0_3_0);
  goto get0_3_ext_interrupt;
 getchar_4_ext_interrupt:
  reasonp = 0l;
 getchar_4_interrupt:
  toppred = &predicate_scanner_xgetchar_4;
  goto interrupt_4;
 }

 ungetc_3_top: {
  q x0, x1;
  a1 = qp->args[1];
  a2 = qp->args[2];
  qp = qp->next;
 ungetc_3_clear_reason:
  reasonp = reasons;
 ungetc_3_0:
  allocp[0] = makesym(functor_ungetc_1);
  allocp[1] = a0;
  x0 = makefunctor(&allocp[0]);
  allocp[2] = a2;
  allocp[3] = x0;
  x1 = makecons(&allocp[2]);
  allocp += 4;
  unify_value(a1, x1);
  proceed();
 ungetc_3_ext_interrupt:
  reasonp = 0l;
 ungetc_3_interrupt:
  toppred = &predicate_scanner_xungetc_3;
  goto interrupt_3;
 }

 chartype_2_top: {
  q x0, x1, x2;
  a1 = qp->args[1];
  qp = qp->next;
 chartype_2_clear_reason:
  reasonp = reasons;
 chartype_2_0:
 chartype_2_1:
  if (!isint(a0)) goto chartype_2_2;
  gblt_integer(a0,chartype_2_interrupt);
  x0 = makefunctor(&vector_const_0);
  x1 = makeint(1L);
  bblt_add_no_check(a0,x1,x2);
  generic_arg[0] = x2;
  generic_arg[1] = a1;
  call_generic(x0, functor_element_2, 0);
  proceed();
 chartype_2_2:
  if (!isref(a0)) goto chartype_2_interrupt;
  deref_and_jump(a0,chartype_2_1);
  *reasonp++ =  a0;
  goto chartype_2_interrupt;
 chartype_2_ext_interrupt:
  reasonp = 0l;
 chartype_2_interrupt:
  toppred = &predicate_scanner_xchartype_2;
  goto interrupt_2;
 }
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
  loop_within_module(module_scanner);
}
