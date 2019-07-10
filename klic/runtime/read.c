/* Compiled by KLIC compiler version 3.003 (Mon Mar 29 19:42:59 JST 1999) */
#include <klic/klichdr.h>
#include "atom.h"
#include "funct.h"

module module_klic__reader();
Const struct predicate predicate_klic__reader_xparse__one__term__reporting__errors_4 =
   { module_klic__reader, 0, 4 };
Const struct predicate predicate_klic__reader_xparse__one__term__reporting__errors_2F4_240_8 =
   { module_klic__reader, 1, 8 };
Const struct predicate predicate_klic__reader_xwparse__one__term__reporting__errors_4 =
   { module_klic__reader, 2, 4 };
Const struct predicate predicate_klic__reader_xwparse__one__term__reporting__errors_2F4_240_8 =
   { module_klic__reader, 3, 8 };
Const struct predicate predicate_klic__reader_xreport__syntax__error_7 =
   { module_klic__reader, 4, 7 };
Const struct predicate predicate_klic__reader_xreport__syntax__error_2F7_240_2 =
   { module_klic__reader, 5, 2 };
Const struct predicate predicate_klic__reader_xwrite__tokens_4 =
   { module_klic__reader, 6, 4 };
Const struct predicate predicate_klic__reader_xwrite__one__token_3 =
   { module_klic__reader, 7, 3 };
Const struct predicate predicate_klic__reader_xblank_2 =
   { module_klic__reader, 8, 2 };
Const struct predicate predicate_klic__reader_xwrite__char_3 =
   { module_klic__reader, 9, 3 };
Const struct predicate predicate_klic__reader_xwrite__atom_3 =
   { module_klic__reader, 10, 3 };
Const struct predicate predicate_klic__reader_xwrite__string_3 =
   { module_klic__reader, 11, 3 };
Const struct predicate predicate_klic__reader_xwrite__number_3 =
   { module_klic__reader, 12, 3 };
Const struct predicate predicate_klic__reader_xwrite__decimal_3 =
   { module_klic__reader, 13, 3 };
Const struct predicate predicate_klic__reader_xlength_2 =
   { module_klic__reader, 14, 2 };
Const struct predicate predicate_klic__reader_xlength_3 =
   { module_klic__reader, 15, 3 };
extern Const struct predicate predicate_unix_xunix_1;
extern Const struct predicate predicate_unix_xexit_1;
extern Const struct predicate predicate_integer__arithmetics_xadd_3;
extern Const struct predicate predicate_integer__arithmetics_xsubtract_3;
extern Const struct predicate predicate_integer__arithmetics_xmodulo_3;
extern Const struct predicate predicate_scanner_xscan_3;
extern Const struct predicate predicate_parser_xparse_3;
extern Const struct predicate predicate_parser_xwparse_3;
extern Const struct predicate predicate_atom__table_xget__atom__string_2;

module module_klic__reader(glbl, qp, allocp, toppred)
  struct global_variables *glbl;
  struct goalrec *qp;
  register q *allocp;
  Const struct predicate *toppred;
{
  static Const q funct_const_0[] = {
    makesym(functor_putc_1),
    makeint(10),
  };
  static Const unsigned char strconst_body_1[12] =
    {'\n','*','*',' ','h','e','r','e',' ','*','*','\n',};
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_1 =
      declare_string_constant_8(strconst_body_1,12);
  static Const q funct_const_2[] = {
    makesym(functor_fwrite_1),
    makefunctor(&string_const_1),
  };
  static Const unsigned char strconst_body_3[4] =
    {' ','*','*','\n',};
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_3 =
      declare_string_constant_8(strconst_body_3,4);
  static Const q funct_const_4[] = {
    makesym(functor_fwrite_1),
    makefunctor(&string_const_3),
  };
  static Const unsigned char strconst_body_5[4] =
    "-";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_5 =
      declare_string_constant_8(strconst_body_5,1);
  static Const q funct_const_6[] = {
    makesym(functor_fwrite_1),
    makefunctor(&string_const_5),
  };
  static Const unsigned char strconst_body_7[8] =
    "<float>";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_7 =
      declare_string_constant_8(strconst_body_7,7);
  static Const q funct_const_8[] = {
    makesym(functor_fwrite_1),
    makefunctor(&string_const_7),
  };
  static Const unsigned char strconst_body_9[24] =
    {'{','S','Y','N','T','A','X',' ','E','R','R','O','R',':',' ','i','n',' ','l','i','n','e','s',' ',};
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_9 =
      declare_string_constant_8(strconst_body_9,24);
  static Const q funct_const_10[] = {
    makesym(functor_fwrite_1),
    makefunctor(&string_const_9),
  };
  static Const unsigned char strconst_body_11[8] =
    "}\n** ";
  declare_method_table_of(byte__string);
  static Const string_structure_type_8 string_const_11 =
      declare_string_constant_8(strconst_body_11,5);
  static Const q funct_const_12[] = {
    makesym(functor_fwrite_1),
    makefunctor(&string_const_11),
  };
  q a0, a1, a2, a3, a4, a5, a6, a7;

  q *reasonp;
 module_top:
  a0 = qp->args[0];
  a1 = qp->args[1];
  switch_on_pred() {
    case_pred(0, parse__one__term__reporting__errors_4_top);
    case_pred(1, parse__one__term__reporting__errors_2F4_240_8_top);
    case_pred(2, wparse__one__term__reporting__errors_4_top);
    case_pred(3, wparse__one__term__reporting__errors_2F4_240_8_top);
    case_pred(4, report__syntax__error_7_top);
    case_pred(5, report__syntax__error_2F7_240_2_top);
    case_pred(6, write__tokens_4_top);
    case_pred(7, write__one__token_3_top);
    case_pred(8, blank_2_top);
    case_pred(9, write__char_3_top);
    case_pred(10, write__atom_3_top);
    case_pred(11, write__string_3_top);
    case_pred(12, write__number_3_top);
    case_pred(13, write__decimal_3_top);
    case_pred(14, length_2_top);
    last_case_pred(15, length_3_top);
  }

 parse__one__term__reporting__errors_4_top: {
  q x0, x1, x2, x3, x4, x5, x6, x7, x8;
  a2 = qp->args[2];
  a3 = qp->args[3];
  qp = qp->next;
 parse__one__term__reporting__errors_4_clear_reason:
  reasonp = reasons;
 parse__one__term__reporting__errors_4_0:
 parse__one__term__reporting__errors_4_1:
  switch (ptagof(a1)) {
 case FUNCTOR:
  if (functor_of(a1) != makesym(functor_s_2)) goto parse__one__term__reporting__errors_4_interrupt;
  allocp[0] = makesym(functor_linecount_1);
  allocp[1] = x1 = makeref(&allocp[1]);
  x0 = makefunctor(&allocp[0]);
  allocp[2] = x3 = makeref(&allocp[2]);
  allocp[3] = x0;
  x2 = makecons(&allocp[2]);
  allocp += 4;
  unify_value(a0, x2);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__reader_xparse__one__term__reporting__errors_2F4_240_8);
  allocp[2] = x4 = makeref(&allocp[2]);
  allocp[3] = a2;
  allocp[4] = a3;
  allocp[5] = x5 = makeref(&allocp[5]);
  allocp[6] = x1;
  allocp[7] = x6 = makeref(&allocp[7]);
  x7 = arg(a1, 1);
  allocp[8] = x7;
  x8 = arg(a1, 0);
  allocp[9] = x8;
  allocp[10] = (q)(struct goalrec*)&allocp[0];
  allocp[11] = (q)(&predicate_parser_xparse_3);
  allocp[12] = x6;
  allocp[13] = x8;
  allocp[14] = x4;
  allocp[15] = (q)(struct goalrec*)&allocp[10];
  allocp[16] = (q)(&predicate_scanner_xscan_3);
  allocp[17] = x6;
  allocp[18] = x3;
  allocp[19] = x5;
  qp = (struct goalrec*)&allocp[15];
  allocp += 20;
  proceed();
 case VARREF:
  deref_and_jump(a1,parse__one__term__reporting__errors_4_1);
  *reasonp++ =  a1;
  goto parse__one__term__reporting__errors_4_interrupt;
  };
  goto parse__one__term__reporting__errors_4_interrupt;
 parse__one__term__reporting__errors_4_ext_interrupt:
  reasonp = 0l;
 parse__one__term__reporting__errors_4_interrupt:
  goto interrupt_4;
 }

 parse__one__term__reporting__errors_2F4_240_8_top: {
  q x0, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10;
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  a6 = qp->args[6];
  a7 = qp->args[7];
  qp = qp->next;
 parse__one__term__reporting__errors_2F4_240_8_clear_reason:
  reasonp = reasons;
 parse__one__term__reporting__errors_2F4_240_8_0:
 parse__one__term__reporting__errors_2F4_240_8_1:
  switch (ptagof(a0)) {
 case FUNCTOR:
  switch (symval(functor_of(a0))) {
 case functor_normal_1:
  x0 = arg(a0, 0);
  unify2(a1, x0, a2, a3);
  proceed();
 case functor_abnormal_2:
  allocp[0] = makesym(functor_s_2);
  allocp[1] = a7;
  allocp[2] = a6;
  x0 = makefunctor(&allocp[0]);
  allocp[3] = (q)qp;
  allocp[4] = (q)(&predicate_klic__reader_xparse__one__term__reporting__errors_4);
  allocp[5] = x1 = makeref(&allocp[5]);
  allocp[6] = x0;
  allocp[7] = a1;
  allocp[8] = a2;
  allocp[9] = (q)(struct goalrec*)&allocp[3];
  allocp[10] = (q)(&predicate_klic__reader_xreport__syntax__error_7);
  x2 = arg(a0, 0);
  allocp[11] = x2;
  allocp[12] = a4;
  allocp[13] = x3 = makeref(&allocp[13]);
  allocp[14] = a5;
  x4 = arg(a0, 1);
  allocp[15] = x4;
  allocp[16] = x5 = makeref(&allocp[16]);
  allocp[17] = a6;
  allocp[18] = makesym(functor_linecount_1);
  allocp[19] = x3;
  x6 = makefunctor(&allocp[18]);
  allocp[20] = x1;
  allocp[21] = x6;
  x7 = makecons(&allocp[20]);
  qp = (struct goalrec*)&allocp[9];
  allocp += 22;
  unify_value(a3, x7);
  allocp[0] = makesym(functor_normal_1);
  allocp[1] = x5;
  x8 = makefunctor(&allocp[0]);
  allocp[2] = makesym(functor_stderr_1);
  allocp[3] = x8;
  x9 = makefunctor(&allocp[2]);
  allocp[4] = NILATOM;
  allocp[5] = x9;
  x10 = makecons(&allocp[4]);
  allocp[6] = (q)qp;
  allocp[7] = (q)(&predicate_unix_xunix_1);
  allocp[8] = x10;
  qp = (struct goalrec*)&allocp[6];
  allocp += 9;
  proceed();
  };
  goto parse__one__term__reporting__errors_2F4_240_8_interrupt;
 case VARREF:
  deref_and_jump(a0,parse__one__term__reporting__errors_2F4_240_8_1);
  *reasonp++ =  a0;
  goto parse__one__term__reporting__errors_2F4_240_8_interrupt;
  };
  goto parse__one__term__reporting__errors_2F4_240_8_interrupt;
 parse__one__term__reporting__errors_2F4_240_8_ext_interrupt:
  reasonp = 0l;
 parse__one__term__reporting__errors_2F4_240_8_interrupt:
  goto interrupt_8;
 }

 wparse__one__term__reporting__errors_4_top: {
  q x0, x1, x2, x3, x4, x5, x6, x7, x8;
  a2 = qp->args[2];
  a3 = qp->args[3];
  qp = qp->next;
 wparse__one__term__reporting__errors_4_clear_reason:
  reasonp = reasons;
 wparse__one__term__reporting__errors_4_0:
 wparse__one__term__reporting__errors_4_1:
  switch (ptagof(a1)) {
 case FUNCTOR:
  if (functor_of(a1) != makesym(functor_s_2)) goto wparse__one__term__reporting__errors_4_interrupt;
  allocp[0] = makesym(functor_linecount_1);
  allocp[1] = x1 = makeref(&allocp[1]);
  x0 = makefunctor(&allocp[0]);
  allocp[2] = x3 = makeref(&allocp[2]);
  allocp[3] = x0;
  x2 = makecons(&allocp[2]);
  allocp += 4;
  unify_value(a0, x2);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__reader_xwparse__one__term__reporting__errors_2F4_240_8);
  allocp[2] = x4 = makeref(&allocp[2]);
  allocp[3] = a2;
  allocp[4] = a3;
  allocp[5] = x5 = makeref(&allocp[5]);
  allocp[6] = x1;
  allocp[7] = x6 = makeref(&allocp[7]);
  x7 = arg(a1, 1);
  allocp[8] = x7;
  x8 = arg(a1, 0);
  allocp[9] = x8;
  allocp[10] = (q)(struct goalrec*)&allocp[0];
  allocp[11] = (q)(&predicate_parser_xwparse_3);
  allocp[12] = x6;
  allocp[13] = x8;
  allocp[14] = x4;
  allocp[15] = (q)(struct goalrec*)&allocp[10];
  allocp[16] = (q)(&predicate_scanner_xscan_3);
  allocp[17] = x6;
  allocp[18] = x3;
  allocp[19] = x5;
  qp = (struct goalrec*)&allocp[15];
  allocp += 20;
  proceed();
 case VARREF:
  deref_and_jump(a1,wparse__one__term__reporting__errors_4_1);
  *reasonp++ =  a1;
  goto wparse__one__term__reporting__errors_4_interrupt;
  };
  goto wparse__one__term__reporting__errors_4_interrupt;
 wparse__one__term__reporting__errors_4_ext_interrupt:
  reasonp = 0l;
 wparse__one__term__reporting__errors_4_interrupt:
  goto interrupt_4;
 }

 wparse__one__term__reporting__errors_2F4_240_8_top: {
  q x0, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10;
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  a6 = qp->args[6];
  a7 = qp->args[7];
  qp = qp->next;
 wparse__one__term__reporting__errors_2F4_240_8_clear_reason:
  reasonp = reasons;
 wparse__one__term__reporting__errors_2F4_240_8_0:
 wparse__one__term__reporting__errors_2F4_240_8_1:
  switch (ptagof(a0)) {
 case FUNCTOR:
  switch (symval(functor_of(a0))) {
 case functor_normal_1:
  allocp[0] = makesym(functor_normal_1);
  x1 = arg(a0, 0);
  allocp[1] = x1;
  x0 = makefunctor(&allocp[0]);
  allocp += 2;
  unify_value(a1, x0);
  unify(a2, a3);
  proceed();
 case functor_abnormal_2:
  allocp[0] = makesym(functor_s_2);
  allocp[1] = a7;
  allocp[2] = a6;
  x0 = makefunctor(&allocp[0]);
  allocp[3] = (q)qp;
  allocp[4] = (q)(&predicate_klic__reader_xwparse__one__term__reporting__errors_4);
  allocp[5] = x1 = makeref(&allocp[5]);
  allocp[6] = x0;
  allocp[7] = a1;
  allocp[8] = a2;
  allocp[9] = (q)(struct goalrec*)&allocp[3];
  allocp[10] = (q)(&predicate_klic__reader_xreport__syntax__error_7);
  x2 = arg(a0, 0);
  allocp[11] = x2;
  allocp[12] = a4;
  allocp[13] = x3 = makeref(&allocp[13]);
  allocp[14] = a5;
  x4 = arg(a0, 1);
  allocp[15] = x4;
  allocp[16] = x5 = makeref(&allocp[16]);
  allocp[17] = a6;
  allocp[18] = makesym(functor_linecount_1);
  allocp[19] = x3;
  x6 = makefunctor(&allocp[18]);
  allocp[20] = x1;
  allocp[21] = x6;
  x7 = makecons(&allocp[20]);
  qp = (struct goalrec*)&allocp[9];
  allocp += 22;
  unify_value(a3, x7);
  allocp[0] = makesym(functor_normal_1);
  allocp[1] = x5;
  x8 = makefunctor(&allocp[0]);
  allocp[2] = makesym(functor_stderr_1);
  allocp[3] = x8;
  x9 = makefunctor(&allocp[2]);
  allocp[4] = NILATOM;
  allocp[5] = x9;
  x10 = makecons(&allocp[4]);
  allocp[6] = (q)qp;
  allocp[7] = (q)(&predicate_unix_xunix_1);
  allocp[8] = x10;
  qp = (struct goalrec*)&allocp[6];
  allocp += 9;
  proceed();
  };
  goto wparse__one__term__reporting__errors_2F4_240_8_interrupt;
 case VARREF:
  deref_and_jump(a0,wparse__one__term__reporting__errors_2F4_240_8_1);
  *reasonp++ =  a0;
  goto wparse__one__term__reporting__errors_2F4_240_8_interrupt;
  };
  goto wparse__one__term__reporting__errors_2F4_240_8_interrupt;
 wparse__one__term__reporting__errors_2F4_240_8_ext_interrupt:
  reasonp = 0l;
 wparse__one__term__reporting__errors_2F4_240_8_interrupt:
  goto interrupt_8;
 }

 report__syntax__error_7_top: {
  q x0, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13, x14, x15, x16, x17, x18, x19, x20, x21;
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  a6 = qp->args[6];
  qp = qp->next;
 report__syntax__error_7_clear_reason:
  reasonp = reasons;
 report__syntax__error_7_0:
 report__syntax__error_7_1:
  if (!isint(a6)) goto report__syntax__error_7_2;
  gblt_integer(a6,report__syntax__error_7_interrupt);
  allocp[0] = x1 = makeref(&allocp[0]);
  allocp[1] = makefunctor(funct_const_10);
  x0 = makecons(&allocp[0]);
  allocp += 2;
  unify_value(a5, x0);
  x2 = makeint(1L);
  bblt_add_no_check_y(a1,x2,x3);
  x4 = makeint(1L);
  bblt_add_no_check_y(a2,x4,x5);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__reader_xreport__syntax__error_2F7_240_2);
  allocp[2] = a6;
  allocp[3] = x6 = makeref(&allocp[3]);
  allocp[4] = makesym(functor_fflush_1);
  allocp[5] = x6;
  x7 = makefunctor(&allocp[4]);
  allocp[6] = NILATOM;
  allocp[7] = x7;
  x8 = makecons(&allocp[6]);
  allocp[8] = x8;
  allocp[9] = makefunctor(funct_const_0);
  x9 = makecons(&allocp[8]);
  allocp[10] = (q)(struct goalrec*)&allocp[0];
  allocp[11] = (q)(&predicate_klic__reader_xwrite__tokens_4);
  allocp[12] = x10 = makeref(&allocp[12]);
  allocp[13] = a4;
  allocp[14] = x11 = makeref(&allocp[14]);
  allocp[15] = x9;
  allocp[16] = x11;
  allocp[17] = makefunctor(funct_const_2);
  x12 = makecons(&allocp[16]);
  allocp[18] = (q)(struct goalrec*)&allocp[10];
  allocp[19] = (q)(&predicate_klic__reader_xwrite__tokens_4);
  allocp[20] = x13 = makeref(&allocp[20]);
  allocp[21] = a3;
  allocp[22] = x14 = makeref(&allocp[22]);
  allocp[23] = x12;
  allocp[24] = (q)(struct goalrec*)&allocp[18];
  allocp[25] = (q)(&predicate_integer__arithmetics_xsubtract_3);
  allocp[26] = x15 = makeref(&allocp[26]);
  allocp[27] = x10;
  allocp[28] = x13;
  allocp[29] = (q)(struct goalrec*)&allocp[24];
  allocp[30] = (q)(&predicate_klic__reader_xlength_2);
  allocp[31] = a4;
  allocp[32] = x10;
  allocp[33] = (q)(struct goalrec*)&allocp[29];
  allocp[34] = (q)(&predicate_klic__reader_xlength_2);
  allocp[35] = a3;
  allocp[36] = x15;
  allocp[37] = makesym(functor_fwrite_1);
  allocp[38] = a0;
  x16 = makefunctor(&allocp[37]);
  allocp[39] = x14;
  allocp[40] = makefunctor(funct_const_4);
  x17 = makecons(&allocp[39]);
  allocp[41] = x17;
  allocp[42] = x16;
  x18 = makecons(&allocp[41]);
  allocp[43] = x18;
  allocp[44] = makefunctor(funct_const_12);
  x19 = makecons(&allocp[43]);
  allocp[45] = (q)(struct goalrec*)&allocp[33];
  allocp[46] = (q)(&predicate_klic__reader_xwrite__number_3);
  allocp[47] = x5;
  allocp[48] = x20 = makeref(&allocp[48]);
  allocp[49] = x19;
  allocp[50] = x20;
  allocp[51] = makefunctor(funct_const_6);
  x21 = makecons(&allocp[50]);
  a0 = x3;
  a1 = x1;
  a2 = x21;
  qp = (struct goalrec*)&allocp[45];
  allocp += 52;
  execute(write__number_3_0);
  goto write__number_3_ext_interrupt;
 report__syntax__error_7_2:
  if (!isref(a6)) goto report__syntax__error_7_interrupt;
  deref_and_jump(a6,report__syntax__error_7_1);
  *reasonp++ =  a6;
  goto report__syntax__error_7_interrupt;
 report__syntax__error_7_ext_interrupt:
  reasonp = 0l;
 report__syntax__error_7_interrupt:
  goto interrupt_7;
 }

 report__syntax__error_2F7_240_2_top: {
  q x0, x1;
  qp = qp->next;
 report__syntax__error_2F7_240_2_clear_reason:
  reasonp = reasons;
 report__syntax__error_2F7_240_2_0:
 report__syntax__error_2F7_240_2_1:
  if (!isint(a0)) goto report__syntax__error_2F7_240_2_5;
  x0 = makeint(0L);
  gblt_less(a0,x0,report__syntax__error_2F7_240_2_4);
 report__syntax__error_2F7_240_2_2:
  if (isref(a1)) goto report__syntax__error_2F7_240_2_3;
  gblt_wait(a1,report__syntax__error_2F7_240_2_4);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_unix_xexit_1);
  allocp[2] = a0;
  qp = (struct goalrec*)&allocp[0];
  allocp += 3;
  proceed();
 report__syntax__error_2F7_240_2_3:
  deref_and_jump(a1,report__syntax__error_2F7_240_2_2);
  *reasonp++ =  a1;
  goto report__syntax__error_2F7_240_2_4;
 report__syntax__error_2F7_240_2_4:
  x1 = makeint(0L);
  gblt_eq(a0,x1,report__syntax__error_2F7_240_2_interrupt);
  proceed();
 report__syntax__error_2F7_240_2_5:
  if (!isref(a0)) goto report__syntax__error_2F7_240_2_interrupt;
  deref_and_jump(a0,report__syntax__error_2F7_240_2_1);
  *reasonp++ =  a0;
  goto report__syntax__error_2F7_240_2_interrupt;
 report__syntax__error_2F7_240_2_ext_interrupt:
  reasonp = 0l;
 report__syntax__error_2F7_240_2_interrupt:
  goto interrupt_2;
 }

 write__tokens_4_top: {
  q x0, x1, x2, x3;
  a2 = qp->args[2];
  a3 = qp->args[3];
  qp = qp->next;
 write__tokens_4_clear_reason:
  reasonp = reasons;
 write__tokens_4_0:
 write__tokens_4_1:
  switch (ptagof(a0)) {
 case ATOMIC:
 write__tokens_4_2:
  if (a0 != makeint(0L)) goto write__tokens_4_3;
  unify(a3, a2);
  proceed();
 case VARREF:
  deref_and_jump(a0,write__tokens_4_1);
  *reasonp++ =  a0;
  goto write__tokens_4_3;
  };
  goto write__tokens_4_3;
 write__tokens_4_3:
 write__tokens_4_4:
  switch (ptagof(a1)) {
 case CONS:
 write__tokens_4_5:
  if (!isint(a0)) goto write__tokens_4_6;
  x0 = makeint(0L);
  gblt_greater(a0,x0,write__tokens_4_interrupt);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__reader_xwrite__tokens_4);
  allocp[2] = x1 = makeref(&allocp[2]);
  x2 = cdr_of(a1);
  allocp[3] = x2;
  allocp[4] = x3 = makeref(&allocp[4]);
  allocp[5] = a3;
  allocp[6] = (q)(struct goalrec*)&allocp[0];
  allocp[7] = (q)(&predicate_integer__arithmetics_xsubtract_3);
  allocp[8] = a0;
  allocp[9] = makeint(1L);
  allocp[10] = x1;
  a0 = car_of(a1);
  a1 = a2;
  a2 = x3;
  qp = (struct goalrec*)&allocp[6];
  allocp += 11;
  execute(write__one__token_3_clear_reason);
  goto write__one__token_3_ext_interrupt;
 write__tokens_4_6:
  if (!isref(a0)) goto write__tokens_4_interrupt;
  deref_and_jump(a0,write__tokens_4_5);
  *reasonp++ =  a0;
  goto write__tokens_4_interrupt;
 case VARREF:
  deref_and_jump(a1,write__tokens_4_4);
  *reasonp++ =  a1;
  goto write__tokens_4_interrupt;
  };
  goto write__tokens_4_interrupt;
 write__tokens_4_ext_interrupt:
  reasonp = 0l;
 write__tokens_4_interrupt:
  goto interrupt_4;
 }

 write__one__token_3_top: {
  q x0, x1, x2, x3;
  a2 = qp->args[2];
  qp = qp->next;
 write__one__token_3_clear_reason:
  reasonp = reasons;
 write__one__token_3_0:
 write__one__token_3_1:
  switch (ptagof(a0)) {
 case ATOMIC:
  if (a0 != makesym(atom_eof)) goto write__one__token_3_2;
  unify(a2, a1);
  proceed();
 case FUNCTOR:
  switch (symval(functor_of(a0))) {
 case functor_variable_4:
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__reader_xblank_2);
  allocp[2] = x0 = makeref(&allocp[2]);
  allocp[3] = a2;
  a0 = arg(a0, 2);
  a2 = x0;
  qp = (struct goalrec*)&allocp[0];
  allocp += 4;
  execute(write__string_3_0);
  goto write__string_3_ext_interrupt;
 case functor_number_1:
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__reader_xblank_2);
  allocp[2] = x0 = makeref(&allocp[2]);
  allocp[3] = a2;
  a0 = arg(a0, 0);
  a2 = x0;
  qp = (struct goalrec*)&allocp[0];
  allocp += 4;
  execute(write__number_3_0);
  goto write__number_3_ext_interrupt;
 case functor_open_1:
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__reader_xblank_2);
  allocp[2] = x0 = makeref(&allocp[2]);
  allocp[3] = a2;
  allocp[4] = (q)(struct goalrec*)&allocp[0];
  allocp[5] = (q)(&predicate_klic__reader_xwrite__char_3);
  allocp[6] = makeint(40L);
  allocp[7] = x1 = makeref(&allocp[7]);
  allocp[8] = x0;
  a0 = arg(a0, 0);
  a2 = x1;
  qp = (struct goalrec*)&allocp[4];
  allocp += 9;
  execute(write__atom_3_0);
  goto write__atom_3_ext_interrupt;
 case functor_atom_1:
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__reader_xblank_2);
  allocp[2] = x0 = makeref(&allocp[2]);
  allocp[3] = a2;
  a0 = arg(a0, 0);
  a2 = x0;
  qp = (struct goalrec*)&allocp[0];
  allocp += 4;
  execute(write__atom_3_0);
  goto write__atom_3_ext_interrupt;
 case functor_string_1:
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__reader_xblank_2);
  allocp[2] = x0 = makeref(&allocp[2]);
  allocp[3] = a2;
  allocp[4] = (q)(struct goalrec*)&allocp[0];
  allocp[5] = (q)(&predicate_klic__reader_xwrite__char_3);
  allocp[6] = makeint(34L);
  allocp[7] = x1 = makeref(&allocp[7]);
  allocp[8] = x0;
  allocp[9] = (q)(struct goalrec*)&allocp[4];
  allocp[10] = (q)(&predicate_klic__reader_xwrite__string_3);
  x2 = arg(a0, 0);
  allocp[11] = x2;
  allocp[12] = x3 = makeref(&allocp[12]);
  allocp[13] = x1;
  a0 = makeint(34L);
  a2 = x3;
  qp = (struct goalrec*)&allocp[9];
  allocp += 14;
  execute(write__char_3_0);
  goto write__char_3_ext_interrupt;
 case functor_error_1:
  unify(a2, a1);
  proceed();
  };
  goto write__one__token_3_2;
 case VARREF:
  deref_and_jump(a0,write__one__token_3_1);
  *reasonp++ =  a0;
  goto write__one__token_3_2;
  };
  goto write__one__token_3_2;
 write__one__token_3_2:
  otherwise(write__one__token_3_interrupt);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__reader_xblank_2);
  allocp[2] = x0 = makeref(&allocp[2]);
  allocp[3] = a2;
  a2 = x0;
  qp = (struct goalrec*)&allocp[0];
  allocp += 4;
  execute(write__atom_3_0);
  goto write__atom_3_ext_interrupt;
 write__one__token_3_ext_interrupt:
  reasonp = 0l;
 write__one__token_3_interrupt:
  toppred = &predicate_klic__reader_xwrite__one__token_3;
  goto interrupt_3;
 }

 blank_2_top: {
  q x0;
  qp = qp->next;
 blank_2_clear_reason:
  reasonp = reasons;
 blank_2_0:
  x0 = a0;
  a2 = a1;
  a1 = x0;
  a0 = makeint(32L);
  execute(write__char_3_0);
  goto write__char_3_ext_interrupt;
 blank_2_ext_interrupt:
  reasonp = 0l;
 blank_2_interrupt:
  goto interrupt_2;
 }

 write__char_3_top: {
  q x0, x1;
  a2 = qp->args[2];
  qp = qp->next;
 write__char_3_clear_reason:
  reasonp = reasons;
 write__char_3_0:
  allocp[0] = makesym(functor_putc_1);
  allocp[1] = a0;
  x0 = makefunctor(&allocp[0]);
  allocp[2] = a2;
  allocp[3] = x0;
  x1 = makecons(&allocp[2]);
  allocp += 4;
  unify_value(a1, x1);
  proceed();
 write__char_3_ext_interrupt:
  reasonp = 0l;
 write__char_3_interrupt:
  toppred = &predicate_klic__reader_xwrite__char_3;
  goto interrupt_3;
 }

 write__atom_3_top: {
  q x0, x1, x2;
  a2 = qp->args[2];
  qp = qp->next;
 write__atom_3_clear_reason:
  reasonp = reasons;
 write__atom_3_0:
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
 write__atom_3_ext_interrupt:
  reasonp = 0l;
 write__atom_3_interrupt:
  toppred = &predicate_klic__reader_xwrite__atom_3;
  goto interrupt_3;
 }

 write__string_3_top: {
  q x0, x1;
  a2 = qp->args[2];
  qp = qp->next;
 write__string_3_clear_reason:
  reasonp = reasons;
 write__string_3_0:
  allocp[0] = makesym(functor_fwrite_1);
  allocp[1] = a0;
  x0 = makefunctor(&allocp[0]);
  allocp[2] = a2;
  allocp[3] = x0;
  x1 = makecons(&allocp[2]);
  allocp += 4;
  unify_value(a1, x1);
  proceed();
 write__string_3_ext_interrupt:
  reasonp = 0l;
 write__string_3_interrupt:
  toppred = &predicate_klic__reader_xwrite__string_3;
  goto interrupt_3;
 }

 write__number_3_top: {
  q x0;
  a2 = qp->args[2];
  qp = qp->next;
 write__number_3_clear_reason:
  reasonp = reasons;
 write__number_3_0:
 write__number_3_1:
  switch (ptagof(a0)) {
 case ATOMIC:
  if (!isint(a0)) goto write__number_3_interrupt;
 write__number_3_2:
  if (a0 != makeint(0L)) goto write__number_3_3;
  a0 = makeint(48L);
  execute(write__char_3_0);
  goto write__char_3_ext_interrupt;
 write__number_3_3:
  x0 = makeint(0L);
  gblt_greater(a0,x0,write__number_3_interrupt);
  execute(write__decimal_3_0);
  goto write__decimal_3_ext_interrupt;
 case FUNCTOR:
 write__number_3_4:
  if (!isgobj(a0)) goto write__number_3_interrupt;
 write__number_3_5:
  guard_generic(a0, functor_float_0, 0, write__number_3_interrupt);
  allocp[0] = a2;
  allocp[1] = makefunctor(funct_const_8);
  x0 = makecons(&allocp[0]);
  allocp += 2;
  unify_value(a1, x0);
  proceed();
 case VARREF:
  deref_and_jump(a0,write__number_3_1);
  *reasonp++ =  a0;
  goto write__number_3_interrupt;
  };
  goto write__number_3_interrupt;
 write__number_3_ext_interrupt:
  reasonp = 0l;
 write__number_3_interrupt:
  toppred = &predicate_klic__reader_xwrite__number_3;
  goto interrupt_3;
 }

 write__decimal_3_top: {
  q x0, x1, x2, x3, x4, x5;
  a2 = qp->args[2];
  qp = qp->next;
 write__decimal_3_clear_reason:
  reasonp = reasons;
 write__decimal_3_0:
 write__decimal_3_1:
  if (!isint(a0)) goto write__decimal_3_3;
  x0 = makeint(0L);
  gblt_greater(a0,x0,write__decimal_3_2);
  x1 = makeint(10L);
  bblt_div_no_check(a0,x1,x2);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klic__reader_xwrite__char_3);
  allocp[2] = x3 = makeref(&allocp[2]);
  allocp[3] = x4 = makeref(&allocp[3]);
  allocp[4] = a2;
  allocp[5] = (q)(struct goalrec*)&allocp[0];
  allocp[6] = (q)(&predicate_integer__arithmetics_xadd_3);
  allocp[7] = x5 = makeref(&allocp[7]);
  allocp[8] = makeint(48L);
  allocp[9] = x3;
  allocp[10] = (q)(struct goalrec*)&allocp[5];
  allocp[11] = (q)(&predicate_integer__arithmetics_xmodulo_3);
  allocp[12] = a0;
  allocp[13] = makeint(10L);
  allocp[14] = x5;
  a0 = x2;
  a2 = x4;
  qp = (struct goalrec*)&allocp[10];
  allocp += 15;
  execute(write__decimal_3_0);
  goto write__decimal_3_ext_interrupt;
 write__decimal_3_2:
  if (a0 != makeint(0L)) goto write__decimal_3_interrupt;
  unify(a2, a1);
  proceed();
 write__decimal_3_3:
  if (!isref(a0)) goto write__decimal_3_interrupt;
  deref_and_jump(a0,write__decimal_3_1);
  *reasonp++ =  a0;
  goto write__decimal_3_interrupt;
 write__decimal_3_ext_interrupt:
  reasonp = 0l;
 write__decimal_3_interrupt:
  toppred = &predicate_klic__reader_xwrite__decimal_3;
  goto interrupt_3;
 }

 length_2_top: {
  qp = qp->next;
 length_2_clear_reason:
  reasonp = reasons;
 length_2_0:
  a2 = a1;
  a1 = makeint(0L);
  execute(length_3_0);
  goto length_3_ext_interrupt;
 length_2_ext_interrupt:
  reasonp = 0l;
 length_2_interrupt:
  goto interrupt_2;
 }

 length_3_top: {
  q x0, x1;
  a2 = qp->args[2];
  qp = qp->next;
 length_3_clear_reason:
  reasonp = reasons;
 length_3_0:
 length_3_1:
  switch (ptagof(a0)) {
 case CONS:
  x0 = makeint(1L);
  bblt_add_no_check_y(a1,x0,x1);
  a0 = cdr_of(a0);
  a1 = x1;
  execute(length_3_0);
  goto length_3_ext_interrupt;
 case ATOMIC:
  if (a0 != NILATOM) goto length_3_interrupt;
  unify(a2, a1);
  proceed();
 case VARREF:
  deref_and_jump(a0,length_3_1);
  *reasonp++ =  a0;
  goto length_3_interrupt;
  };
  goto length_3_interrupt;
 length_3_ext_interrupt:
  reasonp = 0l;
 length_3_interrupt:
  toppred = &predicate_klic__reader_xlength_3;
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
  loop_within_module(module_klic__reader);
}
