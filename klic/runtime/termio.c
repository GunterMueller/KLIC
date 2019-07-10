/* Compiled by KLIC compiler version 3.003 (Thu May  3 23:26:46 JST 2001) */
#include <klic/klichdr.h>
#include "atom.h"
#include "funct.h"

module module_klicio();
Const struct predicate predicate_klicio_xklicio_1 =
   { module_klicio, 0, 1 };
Const struct predicate predicate_klicio_xklicio_2 =
   { module_klicio, 1, 2 };
Const struct predicate predicate_klicio_xsend__unix_3 =
   { module_klicio, 2, 3 };
Const struct predicate predicate_klicio_xclose__unix_1 =
   { module_klicio, 3, 1 };
Const struct predicate predicate_klicio_xtermin_2 =
   { module_klicio, 4, 2 };
Const struct predicate predicate_klicio_xtermout_2 =
   { module_klicio, 5, 2 };
Const struct predicate predicate_klicio_xtermupdt_2 =
   { module_klicio, 6, 2 };
Const struct predicate predicate_klicio_xbind_2 =
   { module_klicio, 7, 2 };
Const struct predicate predicate_klicio_xfork__io_2 =
   { module_klicio, 8, 2 };
Const struct predicate predicate_klicio_xbound__sock_2 =
   { module_klicio, 9, 2 };
Const struct predicate predicate_klicio_xin_3 =
   { module_klicio, 10, 3 };
Const struct predicate predicate_klicio_xout_3 =
   { module_klicio, 11, 3 };
Const struct predicate predicate_klicio_xupdt_3 =
   { module_klicio, 12, 3 };
Const struct predicate predicate_klicio_xaddop_5 =
   { module_klicio, 13, 5 };
Const struct predicate predicate_klicio_xaddop2_5 =
   { module_klicio, 14, 5 };
Const struct predicate predicate_klicio_xrmop_4 =
   { module_klicio, 15, 4 };
Const struct predicate predicate_klicio_xrmop2_4 =
   { module_klicio, 16, 4 };
Const struct predicate predicate_klicio_xdefault__operator__table_1 =
   { module_klicio, 17, 1 };
Const struct predicate predicate_klicio_xoutput__stream_2 =
   { module_klicio, 18, 2 };
Const struct predicate predicate_klicio_xstring__output_4 =
   { module_klicio, 19, 4 };
Const struct predicate predicate_klicio_xcompute__string__length_3 =
   { module_klicio, 20, 3 };
Const struct predicate predicate_klicio_xfill__string_4 =
   { module_klicio, 21, 4 };
Const struct predicate predicate_klicio_xset__string__elements_6 =
   { module_klicio, 22, 6 };
Const struct predicate predicate_klicio_xstring__input_5 =
   { module_klicio, 23, 5 };
Const struct predicate predicate_klicio_xupdate__lc_3 =
   { module_klicio, 24, 3 };
extern q string_g_new();
extern Const struct predicate predicate_unix_xunix_1;
extern Const struct predicate predicate_unparser_xunparse_4;
extern Const struct predicate predicate_unparser_xunwparse_4;
extern Const struct predicate predicate_integer__arithmetics_xadd_3;
extern Const struct predicate predicate_klicio_xdefault__operator__table_1;
extern Const struct predicate predicate_klicio_xout_3;
extern Const struct predicate predicate_klic__reader_xparse__one__term__reporting__errors_4;
extern Const struct predicate predicate_klic__reader_xwparse__one__term__reporting__errors_4;

module module_klicio(glbl, qp, allocp, toppred)
  struct global_variables *glbl;
  struct goalrec *qp;
  register q *allocp;
  Const struct predicate *toppred;
{
  static Const q funct_const_0[] = {
    makesym(functor_op_3),
    makesym(atom__5C_2B),
    makesym(atom_fy),
    makeint(900),
  };
  static Const q cons_const_1[] = {
    NILATOM,
    makefunctor(funct_const_0),
  };
  static Const q funct_const_2[] = {
    makesym(functor_op_3),
    makesym(atom__24_7E),
    makesym(atom_fy),
    makeint(300),
  };
  static Const q cons_const_3[] = {
    makecons(cons_const_1),
    makefunctor(funct_const_2),
  };
  static Const q funct_const_4[] = {
    makesym(functor_op_3),
    makesym(atom__7E),
    makesym(atom_fy),
    makeint(300),
  };
  static Const q cons_const_5[] = {
    makecons(cons_const_3),
    makefunctor(funct_const_4),
  };
  static Const q funct_const_6[] = {
    makesym(functor_op_3),
    makesym(atom_with__macro),
    makesym(atom_fx),
    makeint(1150),
  };
  static Const q cons_const_7[] = {
    makecons(cons_const_5),
    makefunctor(funct_const_6),
  };
  static Const q funct_const_8[] = {
    makesym(functor_op_3),
    makesym(atom_spy),
    makesym(atom_fy),
    makeint(900),
  };
  static Const q cons_const_9[] = {
    makecons(cons_const_7),
    makefunctor(funct_const_8),
  };
  static Const q funct_const_10[] = {
    makesym(functor_op_3),
    makesym(atom_public),
    makesym(atom_fx),
    makeint(1150),
  };
  static Const q cons_const_11[] = {
    makecons(cons_const_9),
    makefunctor(funct_const_10),
  };
  static Const q funct_const_12[] = {
    makesym(functor_op_3),
    makesym(atom_nospy),
    makesym(atom_fy),
    makeint(900),
  };
  static Const q cons_const_13[] = {
    makecons(cons_const_11),
    makefunctor(funct_const_12),
  };
  static Const q funct_const_14[] = {
    makesym(functor_op_3),
    makesym(atom_module),
    makesym(atom_fx),
    makeint(80),
  };
  static Const q cons_const_15[] = {
    makecons(cons_const_13),
    makefunctor(funct_const_14),
  };
  static Const q funct_const_16[] = {
    makesym(functor_op_3),
    makesym(atom_mode),
    makesym(atom_fx),
    makeint(1150),
  };
  static Const q cons_const_17[] = {
    makecons(cons_const_15),
    makefunctor(funct_const_16),
  };
  static Const q funct_const_18[] = {
    makesym(functor_op_3),
    makesym(atom_local__implicit),
    makesym(atom_fx),
    makeint(1150),
  };
  static Const q cons_const_19[] = {
    makecons(cons_const_17),
    makefunctor(funct_const_18),
  };
  static Const q funct_const_20[] = {
    makesym(functor_op_3),
    makesym(atom_implicit),
    makesym(atom_fx),
    makeint(1150),
  };
  static Const q cons_const_21[] = {
    makecons(cons_const_19),
    makefunctor(funct_const_20),
  };
  static Const q funct_const_22[] = {
    makesym(functor_op_3),
    makesym(atom__3F_2D),
    makesym(atom_fx),
    makeint(1200),
  };
  static Const q cons_const_23[] = {
    makecons(cons_const_21),
    makefunctor(funct_const_22),
  };
  static Const q funct_const_24[] = {
    makesym(functor_op_3),
    makesym(atom__3A_2D),
    makesym(atom_fx),
    makeint(1200),
  };
  static Const q cons_const_25[] = {
    makecons(cons_const_23),
    makefunctor(funct_const_24),
  };
  static Const q funct_const_26[] = {
    makesym(functor_op_3),
    makesym(atom__2D),
    makesym(atom_fx),
    makeint(500),
  };
  static Const q cons_const_27[] = {
    makecons(cons_const_25),
    makefunctor(funct_const_26),
  };
  static Const q funct_const_28[] = {
    makesym(functor_op_3),
    makesym(atom__2B),
    makesym(atom_fx),
    makeint(500),
  };
  static Const q cons_const_29[] = {
    makecons(cons_const_27),
    makefunctor(funct_const_28),
  };
  static Const q funct_const_30[] = {
    makesym(functor_op_3),
    makesym(atom__26),
    makesym(atom_fx),
    makeint(200),
  };
  static Const q cons_const_31[] = {
    makecons(cons_const_29),
    makefunctor(funct_const_30),
  };
  static Const q funct_const_32[] = {
    makesym(functor_op_3),
    makesym(atom__23),
    makesym(atom_fx),
    makeint(100),
  };
  static Const q cons_const_33[] = {
    makecons(cons_const_31),
    makefunctor(funct_const_32),
  };
  static Const q funct_const_34[] = {
    makesym(functor_op_3),
    makesym(atom_xor),
    makesym(atom_yfx),
    makeint(500),
  };
  static Const q cons_const_35[] = {
    NILATOM,
    makefunctor(funct_const_34),
  };
  static Const q funct_const_36[] = {
    makesym(functor_op_3),
    makesym(atom_mod),
    makesym(atom_xfx),
    makeint(300),
  };
  static Const q cons_const_37[] = {
    makecons(cons_const_35),
    makefunctor(funct_const_36),
  };
  static Const q funct_const_38[] = {
    makesym(functor_op_3),
    makesym(atom_is),
    makesym(atom_xfx),
    makeint(700),
  };
  static Const q cons_const_39[] = {
    makecons(cons_const_37),
    makefunctor(funct_const_38),
  };
  static Const q funct_const_40[] = {
    makesym(functor_op_3),
    makesym(atom__5E),
    makesym(atom_xfy),
    makeint(200),
  };
  static Const q cons_const_41[] = {
    makecons(cons_const_39),
    makefunctor(funct_const_40),
  };
  static Const q funct_const_42[] = {
    makesym(functor_op_3),
    makesym(atom__5C_3D_3D),
    makesym(atom_xfx),
    makeint(700),
  };
  static Const q cons_const_43[] = {
    makecons(cons_const_41),
    makefunctor(funct_const_42),
  };
  static Const q funct_const_44[] = {
    makesym(functor_op_3),
    makesym(atom__5C_3D),
    makesym(atom_xfx),
    makeint(700),
  };
  static Const q cons_const_45[] = {
    makecons(cons_const_43),
    makefunctor(funct_const_44),
  };
  static Const q funct_const_46[] = {
    makesym(functor_op_3),
    makesym(atom__5C_2F),
    makesym(atom_yfx),
    makeint(500),
  };
  static Const q cons_const_47[] = {
    makecons(cons_const_45),
    makefunctor(funct_const_46),
  };
  static Const q funct_const_48[] = {
    makesym(functor_op_3),
    makesym(atom__40_3E_3D),
    makesym(atom_xfx),
    makeint(700),
  };
  static Const q cons_const_49[] = {
    makecons(cons_const_47),
    makefunctor(funct_const_48),
  };
  static Const q funct_const_50[] = {
    makesym(functor_op_3),
    makesym(atom__40_3E),
    makesym(atom_xfx),
    makeint(700),
  };
  static Const q cons_const_51[] = {
    makecons(cons_const_49),
    makefunctor(funct_const_50),
  };
  static Const q funct_const_52[] = {
    makesym(functor_op_3),
    makesym(atom__40_3D_3C),
    makesym(atom_xfx),
    makeint(700),
  };
  static Const q cons_const_53[] = {
    makecons(cons_const_51),
    makefunctor(funct_const_52),
  };
  static Const q funct_const_54[] = {
    makesym(functor_op_3),
    makesym(atom__40_3C),
    makesym(atom_xfx),
    makeint(700),
  };
  static Const q cons_const_55[] = {
    makecons(cons_const_53),
    makefunctor(funct_const_54),
  };
  static Const q funct_const_56[] = {
    makesym(functor_op_3),
    makesym(atom__40),
    makesym(atom_xfy),
    makeint(700),
  };
  static Const q cons_const_57[] = {
    makecons(cons_const_55),
    makefunctor(funct_const_56),
  };
  static Const q funct_const_58[] = {
    makesym(functor_op_3),
    makesym(atom__3E_3E),
    makesym(atom_yfx),
    makeint(400),
  };
  static Const q cons_const_59[] = {
    makecons(cons_const_57),
    makefunctor(funct_const_58),
  };
  static Const q funct_const_60[] = {
    makesym(functor_op_3),
    makesym(atom__2F_3D),
    makesym(atom_xfx),
    makeint(700),
  };
  static Const q cons_const_61[] = {
    makecons(cons_const_59),
    makefunctor(funct_const_60),
  };
  static Const q funct_const_62[] = {
    makesym(functor_op_3),
    makesym(atom__2A_3D),
    makesym(atom_xfx),
    makeint(700),
  };
  static Const q cons_const_63[] = {
    makecons(cons_const_61),
    makefunctor(funct_const_62),
  };
  static Const q funct_const_64[] = {
    makesym(functor_op_3),
    makesym(atom__2D_3D),
    makesym(atom_xfx),
    makeint(700),
  };
  static Const q cons_const_65[] = {
    makecons(cons_const_63),
    makefunctor(funct_const_64),
  };
  static Const q funct_const_66[] = {
    makesym(functor_op_3),
    makesym(atom__2B_3D),
    makesym(atom_xfx),
    makeint(700),
  };
  static Const q cons_const_67[] = {
    makecons(cons_const_65),
    makefunctor(funct_const_66),
  };
  static Const q funct_const_68[] = {
    makesym(functor_op_3),
    makesym(atom__3E_3D),
    makesym(atom_xfx),
    makeint(700),
  };
  static Const q cons_const_69[] = {
    makecons(cons_const_67),
    makefunctor(funct_const_68),
  };
  static Const q funct_const_70[] = {
    makesym(functor_op_3),
    makesym(atom__3E),
    makesym(atom_xfx),
    makeint(700),
  };
  static Const q cons_const_71[] = {
    makecons(cons_const_69),
    makefunctor(funct_const_70),
  };
  static Const q funct_const_72[] = {
    makesym(functor_op_3),
    makesym(atom__3D_5C_3D),
    makesym(atom_xfx),
    makeint(700),
  };
  static Const q cons_const_73[] = {
    makecons(cons_const_71),
    makefunctor(funct_const_72),
  };
  static Const q funct_const_74[] = {
    makesym(functor_op_3),
    makesym(atom__3D_3E),
    makesym(atom_xfx),
    makeint(1090),
  };
  static Const q cons_const_75[] = {
    makecons(cons_const_73),
    makefunctor(funct_const_74),
  };
  static Const q funct_const_76[] = {
    makesym(functor_op_3),
    makesym(atom__3D_3D),
    makesym(atom_xfx),
    makeint(700),
  };
  static Const q cons_const_77[] = {
    makecons(cons_const_75),
    makefunctor(funct_const_76),
  };
  static Const q funct_const_78[] = {
    makesym(functor_op_3),
    makesym(atom__3D_3C),
    makesym(atom_xfx),
    makeint(700),
  };
  static Const q cons_const_79[] = {
    makecons(cons_const_77),
    makefunctor(funct_const_78),
  };
  static Const q funct_const_80[] = {
    makesym(functor_op_3),
    makesym(atom__3D_3A_3D),
    makesym(atom_xfx),
    makeint(700),
  };
  static Const q cons_const_81[] = {
    makecons(cons_const_79),
    makefunctor(funct_const_80),
  };
  static Const q funct_const_82[] = {
    makesym(functor_op_3),
    makesym(atom__3D_2E_2E),
    makesym(atom_xfx),
    makeint(700),
  };
  static Const q cons_const_83[] = {
    makecons(cons_const_81),
    makefunctor(funct_const_82),
  };
  static Const q funct_const_84[] = {
    makesym(functor_op_3),
    makesym(atom__3D),
    makesym(atom_xfx),
    makeint(700),
  };
  static Const q cons_const_85[] = {
    makecons(cons_const_83),
    makefunctor(funct_const_84),
  };
  static Const q funct_const_86[] = {
    makesym(functor_op_3),
    makesym(atom__3C_3D_3D),
    makesym(atom_xfx),
    makeint(700),
  };
  static Const q cons_const_87[] = {
    makecons(cons_const_85),
    makefunctor(funct_const_86),
  };
  static Const q funct_const_88[] = {
    makesym(functor_op_3),
    makesym(atom__3D_3E),
    makesym(atom_xfx),
    makeint(700),
  };
  static Const q cons_const_89[] = {
    makecons(cons_const_87),
    makefunctor(funct_const_88),
  };
  static Const q funct_const_90[] = {
    makesym(functor_op_3),
    makesym(atom__3C_3D),
    makesym(atom_xfx),
    makeint(700),
  };
  static Const q cons_const_91[] = {
    makecons(cons_const_89),
    makefunctor(funct_const_90),
  };
  static Const q funct_const_92[] = {
    makesym(functor_op_3),
    makesym(atom__3C_3C_3D),
    makesym(atom_xfx),
    makeint(700),
  };
  static Const q cons_const_93[] = {
    makecons(cons_const_91),
    makefunctor(funct_const_92),
  };
  static Const q funct_const_94[] = {
    makesym(functor_op_3),
    makesym(atom__3C_3C),
    makesym(atom_yfx),
    makeint(400),
  };
  static Const q cons_const_95[] = {
    makecons(cons_const_93),
    makefunctor(funct_const_94),
  };
  static Const q funct_const_96[] = {
    makesym(functor_op_3),
    makesym(atom__3C),
    makesym(atom_xfx),
    makeint(700),
  };
  static Const q cons_const_97[] = {
    makecons(cons_const_95),
    makefunctor(funct_const_96),
  };
  static Const q funct_const_98[] = {
    makesym(functor_op_3),
    makesym(atom__3B),
    makesym(atom_xfy),
    makeint(1100),
  };
  static Const q cons_const_99[] = {
    makecons(cons_const_97),
    makefunctor(funct_const_98),
  };
  static Const q funct_const_100[] = {
    makesym(functor_op_3),
    makesym(atom__3A_3D),
    makesym(atom_xfx),
    makeint(700),
  };
  static Const q cons_const_101[] = {
    makecons(cons_const_99),
    makefunctor(funct_const_100),
  };
  static Const q funct_const_102[] = {
    makesym(functor_op_3),
    makesym(atom__3A_3A),
    makesym(atom_xfx),
    makeint(90),
  };
  static Const q cons_const_103[] = {
    makecons(cons_const_101),
    makefunctor(funct_const_102),
  };
  static Const q funct_const_104[] = {
    makesym(functor_op_3),
    makesym(atom__3A_2D),
    makesym(atom_xfx),
    makeint(1200),
  };
  static Const q cons_const_105[] = {
    makecons(cons_const_103),
    makefunctor(funct_const_104),
  };
  static Const q funct_const_106[] = {
    makesym(functor_op_3),
    makesym(atom__3A),
    makesym(atom_xfy),
    makeint(800),
  };
  static Const q cons_const_107[] = {
    makecons(cons_const_105),
    makefunctor(funct_const_106),
  };
  static Const q funct_const_108[] = {
    makesym(functor_op_3),
    makesym(atom__2F_5C),
    makesym(atom_yfx),
    makeint(500),
  };
  static Const q cons_const_109[] = {
    makecons(cons_const_107),
    makefunctor(funct_const_108),
  };
  static Const q funct_const_110[] = {
    makesym(functor_op_3),
    makesym(atom__2F_2F),
    makesym(atom_yfx),
    makeint(400),
  };
  static Const q cons_const_111[] = {
    makecons(cons_const_109),
    makefunctor(funct_const_110),
  };
  static Const q funct_const_112[] = {
    makesym(functor_op_3),
    makesym(atom__2F),
    makesym(atom_yfx),
    makeint(400),
  };
  static Const q cons_const_113[] = {
    makecons(cons_const_111),
    makefunctor(funct_const_112),
  };
  static Const q funct_const_114[] = {
    makesym(functor_op_3),
    makesym(atom__2D_3E),
    makesym(atom_xfy),
    makeint(1050),
  };
  static Const q cons_const_115[] = {
    makecons(cons_const_113),
    makefunctor(funct_const_114),
  };
  static Const q funct_const_116[] = {
    makesym(functor_op_3),
    makesym(atom__2D_2D_3E),
    makesym(atom_xfx),
    makeint(1200),
  };
  static Const q cons_const_117[] = {
    makecons(cons_const_115),
    makefunctor(funct_const_116),
  };
  static Const q funct_const_118[] = {
    makesym(functor_op_3),
    makesym(atom__2D),
    makesym(atom_yfx),
    makeint(500),
  };
  static Const q cons_const_119[] = {
    makecons(cons_const_117),
    makefunctor(funct_const_118),
  };
  static Const q funct_const_120[] = {
    makesym(functor_op_3),
    makesym(atom__2C),
    makesym(atom_xfy),
    makeint(1000),
  };
  static Const q cons_const_121[] = {
    makecons(cons_const_119),
    makefunctor(funct_const_120),
  };
  static Const q funct_const_122[] = {
    makesym(functor_op_3),
    makesym(atom__2B),
    makesym(atom_yfx),
    makeint(500),
  };
  static Const q cons_const_123[] = {
    makecons(cons_const_121),
    makefunctor(funct_const_122),
  };
  static Const q funct_const_124[] = {
    makesym(functor_op_3),
    makesym(atom__2A_2A),
    makesym(atom_xfy),
    makeint(300),
  };
  static Const q cons_const_125[] = {
    makecons(cons_const_123),
    makefunctor(funct_const_124),
  };
  static Const q funct_const_126[] = {
    makesym(functor_op_3),
    makesym(atom__2A),
    makesym(atom_yfx),
    makeint(400),
  };
  static Const q cons_const_127[] = {
    makecons(cons_const_125),
    makefunctor(funct_const_126),
  };
  static Const q funct_const_128[] = {
    makesym(functor_op_3),
    makesym(atom__24_2F_3D),
    makesym(atom_xfx),
    makeint(700),
  };
  static Const q cons_const_129[] = {
    makecons(cons_const_127),
    makefunctor(funct_const_128),
  };
  static Const q funct_const_130[] = {
    makesym(functor_op_3),
    makesym(atom__24_2A_3D),
    makesym(atom_xfx),
    makeint(700),
  };
  static Const q cons_const_131[] = {
    makecons(cons_const_129),
    makefunctor(funct_const_130),
  };
  static Const q funct_const_132[] = {
    makesym(functor_op_3),
    makesym(atom__24_2D_3D),
    makesym(atom_xfx),
    makeint(700),
  };
  static Const q cons_const_133[] = {
    makecons(cons_const_131),
    makefunctor(funct_const_132),
  };
  static Const q funct_const_134[] = {
    makesym(functor_op_3),
    makesym(atom__24_2B_3D),
    makesym(atom_xfx),
    makeint(700),
  };
  static Const q cons_const_135[] = {
    makecons(cons_const_133),
    makefunctor(funct_const_134),
  };
  static Const q funct_const_136[] = {
    makesym(functor_op_3),
    makesym(atom__26_3E_3D),
    makesym(atom_xfx),
    makeint(700),
  };
  static Const q cons_const_137[] = {
    makecons(cons_const_135),
    makefunctor(funct_const_136),
  };
  static Const q funct_const_138[] = {
    makesym(functor_op_3),
    makesym(atom__26_3E),
    makesym(atom_xfx),
    makeint(700),
  };
  static Const q cons_const_139[] = {
    makecons(cons_const_137),
    makefunctor(funct_const_138),
  };
  static Const q funct_const_140[] = {
    makesym(functor_op_3),
    makesym(atom__26_3D_3C),
    makesym(atom_xfx),
    makeint(700),
  };
  static Const q cons_const_141[] = {
    makecons(cons_const_139),
    makefunctor(funct_const_140),
  };
  static Const q funct_const_142[] = {
    makesym(functor_op_3),
    makesym(atom__26_3C_3D),
    makesym(atom_xfx),
    makeint(700),
  };
  static Const q cons_const_143[] = {
    makecons(cons_const_141),
    makefunctor(funct_const_142),
  };
  static Const q funct_const_144[] = {
    makesym(functor_op_3),
    makesym(atom__26_3C),
    makesym(atom_xfx),
    makeint(700),
  };
  static Const q cons_const_145[] = {
    makecons(cons_const_143),
    makefunctor(funct_const_144),
  };
  static Const q funct_const_146[] = {
    makesym(functor_op_3),
    makesym(atom__24_3E_3D),
    makesym(atom_xfx),
    makeint(700),
  };
  static Const q cons_const_147[] = {
    makecons(cons_const_145),
    makefunctor(funct_const_146),
  };
  static Const q funct_const_148[] = {
    makesym(functor_op_3),
    makesym(atom__24_3E),
    makesym(atom_xfx),
    makeint(700),
  };
  static Const q cons_const_149[] = {
    makecons(cons_const_147),
    makefunctor(funct_const_148),
  };
  static Const q funct_const_150[] = {
    makesym(functor_op_3),
    makesym(atom__24_3D_5C_3D),
    makesym(atom_xfx),
    makeint(700),
  };
  static Const q cons_const_151[] = {
    makecons(cons_const_149),
    makefunctor(funct_const_150),
  };
  static Const q funct_const_152[] = {
    makesym(functor_op_3),
    makesym(atom__24_3D_3C),
    makesym(atom_xfx),
    makeint(700),
  };
  static Const q cons_const_153[] = {
    makecons(cons_const_151),
    makefunctor(funct_const_152),
  };
  static Const q funct_const_154[] = {
    makesym(functor_op_3),
    makesym(atom__24_3D_3A_3D),
    makesym(atom_xfx),
    makeint(700),
  };
  static Const q cons_const_155[] = {
    makecons(cons_const_153),
    makefunctor(funct_const_154),
  };
  static Const q funct_const_156[] = {
    makesym(functor_op_3),
    makesym(atom__24_3C_3D),
    makesym(atom_xfx),
    makeint(700),
  };
  static Const q cons_const_157[] = {
    makecons(cons_const_155),
    makefunctor(funct_const_156),
  };
  static Const q funct_const_158[] = {
    makesym(functor_op_3),
    makesym(atom__24_3C),
    makesym(atom_xfx),
    makeint(700),
  };
  static Const q cons_const_159[] = {
    makecons(cons_const_157),
    makefunctor(funct_const_158),
  };
  static Const q funct_const_160[] = {
    makesym(functor_op_3),
    makesym(atom__24_3A_3D),
    makesym(atom_xfx),
    makeint(700),
  };
  static Const q cons_const_161[] = {
    makecons(cons_const_159),
    makefunctor(funct_const_160),
  };
  static Const q funct_const_162[] = {
    makesym(functor_op_3),
    makesym(atom__24_24_3E_3D),
    makesym(atom_xfx),
    makeint(700),
  };
  static Const q cons_const_163[] = {
    makecons(cons_const_161),
    makefunctor(funct_const_162),
  };
  static Const q funct_const_164[] = {
    makesym(functor_op_3),
    makesym(atom__24_24_3E),
    makesym(atom_xfx),
    makeint(700),
  };
  static Const q cons_const_165[] = {
    makecons(cons_const_163),
    makefunctor(funct_const_164),
  };
  static Const q funct_const_166[] = {
    makesym(functor_op_3),
    makesym(atom__24_24_3D_5C_3D),
    makesym(atom_xfx),
    makeint(700),
  };
  static Const q cons_const_167[] = {
    makecons(cons_const_165),
    makefunctor(funct_const_166),
  };
  static Const q funct_const_168[] = {
    makesym(functor_op_3),
    makesym(atom__24_24_3D_3C),
    makesym(atom_xfx),
    makeint(700),
  };
  static Const q cons_const_169[] = {
    makecons(cons_const_167),
    makefunctor(funct_const_168),
  };
  static Const q funct_const_170[] = {
    makesym(functor_op_3),
    makesym(atom__24_24_3D_3A_3D),
    makesym(atom_xfx),
    makeint(700),
  };
  static Const q cons_const_171[] = {
    makecons(cons_const_169),
    makefunctor(funct_const_170),
  };
  static Const q funct_const_172[] = {
    makesym(functor_op_3),
    makesym(atom__24_24_3C_3D),
    makesym(atom_xfx),
    makeint(700),
  };
  static Const q cons_const_173[] = {
    makecons(cons_const_171),
    makefunctor(funct_const_172),
  };
  static Const q funct_const_174[] = {
    makesym(functor_op_3),
    makesym(atom__24_24_3C),
    makesym(atom_xfx),
    makeint(700),
  };
  static Const q cons_const_175[] = {
    makecons(cons_const_173),
    makefunctor(funct_const_174),
  };
  static Const q funct_const_176[] = {
    makesym(functor_op_3),
    makesym(atom__24_24_3A_3D),
    makesym(atom_xfx),
    makeint(700),
  };
  static Const q cons_const_177[] = {
    makecons(cons_const_175),
    makefunctor(funct_const_176),
  };
  static Const q funct_const_178[] = {
    makesym(functor_op_3),
    makesym(atom__23),
    makesym(atom_xfx),
    makeint(100),
  };
  static Const q cons_const_179[] = {
    makecons(cons_const_177),
    makefunctor(funct_const_178),
  };
  static Const q funct_const_180[] = {
    makesym(functor_op_3),
    makesym(atom__2D_2D),
    makesym(atom_xf),
    makeint(150),
  };
  static Const q cons_const_181[] = {
    NILATOM,
    makefunctor(funct_const_180),
  };
  static Const q funct_const_182[] = {
    makesym(functor_op_3),
    makesym(atom__2B_2B),
    makesym(atom_xf),
    makeint(150),
  };
  static Const q cons_const_183[] = {
    makecons(cons_const_181),
    makefunctor(funct_const_182),
  };
  static Const q funct_const_184[] = {
    makesym(functor_ops_3),
    makecons(cons_const_33),
    makecons(cons_const_179),
    makecons(cons_const_183),
  };
  static Const q funct_const_185[] = {
    makesym(functor_putc_1),
    makeint(10),
  };
  q a0, a1, a2, a3, a4, a5;

  q *reasonp;
 module_top:
  a0 = qp->args[0];
  switch_on_pred() {
    case_pred(0, klicio_1_top);
    case_pred(1, klicio_2_top);
    case_pred(2, send__unix_3_top);
    case_pred(3, close__unix_1_top);
    case_pred(4, termin_2_top);
    case_pred(5, termout_2_top);
    case_pred(6, termupdt_2_top);
    case_pred(7, bind_2_top);
    case_pred(8, fork__io_2_top);
    case_pred(9, bound__sock_2_top);
    case_pred(10, in_3_top);
    case_pred(11, out_3_top);
    case_pred(12, updt_3_top);
    case_pred(13, addop_5_top);
    case_pred(14, addop2_5_top);
    case_pred(15, rmop_4_top);
    case_pred(16, rmop2_4_top);
    case_pred(17, default__operator__table_1_top);
    case_pred(18, output__stream_2_top);
    case_pred(19, string__output_4_top);
    case_pred(20, compute__string__length_3_top);
    case_pred(21, fill__string_4_top);
    case_pred(22, set__string__elements_6_top);
    case_pred(23, string__input_5_top);
    last_case_pred(24, update__lc_3_top);
  }

 klicio_1_top: {
  qp = qp->next;
 klicio_1_clear_reason:
  reasonp = reasons;
 klicio_1_0:
  a1 = NILATOM;
  execute(klicio_2_0);
  goto klicio_2_ext_interrupt;
 klicio_1_ext_interrupt:
  reasonp = 0l;
 klicio_1_interrupt:
  goto interrupt_1;
 }

 klicio_2_top: {
  q x0, x1, x2, x3, x4, x5, x6, x7;
  a1 = qp->args[1];
  qp = qp->next;
 klicio_2_clear_reason:
  reasonp = reasons;
 klicio_2_0:
 klicio_2_1:
  switch (ptagof(a0)) {
 case CONS:
  x0 = car_of(a0);
 klicio_2_2:
  switch (ptagof(x0)) {
 case FUNCTOR:
  switch (symval(functor_of(x0))) {
 case functor_stdin_1:
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klicio_xklicio_2);
  x1 = cdr_of(a0);
  allocp[2] = x1;
  allocp[3] = x2 = makeref(&allocp[3]);
  allocp[4] = (q)(struct goalrec*)&allocp[0];
  allocp[5] = (q)(&predicate_klicio_xtermin_2);
  allocp[6] = x3 = makeref(&allocp[6]);
  x4 = arg(x0, 0);
  allocp[7] = x4;
  allocp[8] = makesym(functor_stdin_1);
  allocp[9] = x3;
  x5 = makefunctor(&allocp[8]);
  a0 = x5;
  a2 = x2;
  qp = (struct goalrec*)&allocp[4];
  allocp += 10;
  execute(send__unix_3_0);
  goto send__unix_3_ext_interrupt;
 case functor_stdout_1:
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klicio_xklicio_2);
  x1 = cdr_of(a0);
  allocp[2] = x1;
  allocp[3] = x2 = makeref(&allocp[3]);
  allocp[4] = (q)(struct goalrec*)&allocp[0];
  allocp[5] = (q)(&predicate_klicio_xtermout_2);
  allocp[6] = x3 = makeref(&allocp[6]);
  x4 = arg(x0, 0);
  allocp[7] = x4;
  allocp[8] = makesym(functor_stdout_1);
  allocp[9] = x3;
  x5 = makefunctor(&allocp[8]);
  a0 = x5;
  a2 = x2;
  qp = (struct goalrec*)&allocp[4];
  allocp += 10;
  execute(send__unix_3_0);
  goto send__unix_3_ext_interrupt;
 case functor_stderr_1:
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klicio_xklicio_2);
  x1 = cdr_of(a0);
  allocp[2] = x1;
  allocp[3] = x2 = makeref(&allocp[3]);
  allocp[4] = (q)(struct goalrec*)&allocp[0];
  allocp[5] = (q)(&predicate_klicio_xtermout_2);
  allocp[6] = x3 = makeref(&allocp[6]);
  x4 = arg(x0, 0);
  allocp[7] = x4;
  allocp[8] = makesym(functor_stderr_1);
  allocp[9] = x3;
  x5 = makefunctor(&allocp[8]);
  a0 = x5;
  a2 = x2;
  qp = (struct goalrec*)&allocp[4];
  allocp += 10;
  execute(send__unix_3_0);
  goto send__unix_3_ext_interrupt;
 case functor_string__output_2:
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klicio_xklicio_2);
  x1 = cdr_of(a0);
  allocp[2] = x1;
  allocp[3] = a1;
  allocp[4] = makesym(functor_normal_1);
  allocp[5] = x3 = makeref(&allocp[5]);
  x2 = makefunctor(&allocp[4]);
  allocp[6] = (q)(struct goalrec*)&allocp[0];
  allocp[7] = (q)(&predicate_klicio_xtermout_2);
  allocp[8] = x2;
  x4 = arg(x0, 0);
  allocp[9] = x4;
  a0 = x3;
  a1 = arg(x0, 1);
  allocp[10] = x5 = makeref(&allocp[10]);
  a2 = x5;
  a3 = x5;
  qp = (struct goalrec*)&allocp[6];
  allocp += 11;
  execute(string__output_4_0);
  goto string__output_4_ext_interrupt;
 case functor_string__input_2:
  x1 = arg(x0, 1);
 klicio_2_3:
  switch (ptagof(x1)) {
 case FUNCTOR:
 klicio_2_4:
  if (!isgobj(x1)) goto klicio_2_6;
 klicio_2_5:
  gblt_is_string(x1,klicio_2_6);
  gblt_size_of_string(x1,x2,klicio_2_6);
  gblt_element_size_of_string(x1,x3,klicio_2_6);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klicio_xklicio_2);
  x4 = cdr_of(a0);
  allocp[2] = x4;
  allocp[3] = a1;
  allocp[4] = makesym(functor_normal_1);
  allocp[5] = x6 = makeref(&allocp[5]);
  x5 = makefunctor(&allocp[4]);
  allocp[6] = (q)(struct goalrec*)&allocp[0];
  allocp[7] = (q)(&predicate_klicio_xtermin_2);
  allocp[8] = x5;
  x7 = arg(x0, 0);
  allocp[9] = x7;
  a0 = x6;
  a1 = x1;
  a2 = makeint(0L);
  a3 = x2;
  a4 = makeint(0L);
  qp = (struct goalrec*)&allocp[6];
  allocp += 10;
  execute(string__input_5_0);
  goto string__input_5_ext_interrupt;
 case VARREF:
  deref_and_jump(x1,klicio_2_3);
  *reasonp++ =  x1;
  goto klicio_2_6;
  };
  goto klicio_2_6;
 case functor_read__open_2:
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klicio_xklicio_2);
  x1 = cdr_of(a0);
  allocp[2] = x1;
  allocp[3] = x2 = makeref(&allocp[3]);
  allocp[4] = (q)(struct goalrec*)&allocp[0];
  allocp[5] = (q)(&predicate_klicio_xtermin_2);
  allocp[6] = x3 = makeref(&allocp[6]);
  x4 = arg(x0, 1);
  allocp[7] = x4;
  allocp[8] = makesym(functor_read__open_2);
  x6 = arg(x0, 0);
  allocp[9] = x6;
  allocp[10] = x3;
  x5 = makefunctor(&allocp[8]);
  a0 = x5;
  a2 = x2;
  qp = (struct goalrec*)&allocp[4];
  allocp += 11;
  execute(send__unix_3_0);
  goto send__unix_3_ext_interrupt;
 case functor_write__open_2:
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klicio_xklicio_2);
  x1 = cdr_of(a0);
  allocp[2] = x1;
  allocp[3] = x2 = makeref(&allocp[3]);
  allocp[4] = (q)(struct goalrec*)&allocp[0];
  allocp[5] = (q)(&predicate_klicio_xtermout_2);
  allocp[6] = x3 = makeref(&allocp[6]);
  x4 = arg(x0, 1);
  allocp[7] = x4;
  allocp[8] = makesym(functor_write__open_2);
  x6 = arg(x0, 0);
  allocp[9] = x6;
  allocp[10] = x3;
  x5 = makefunctor(&allocp[8]);
  a0 = x5;
  a2 = x2;
  qp = (struct goalrec*)&allocp[4];
  allocp += 11;
  execute(send__unix_3_0);
  goto send__unix_3_ext_interrupt;
 case functor_append__open_2:
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klicio_xklicio_2);
  x1 = cdr_of(a0);
  allocp[2] = x1;
  allocp[3] = x2 = makeref(&allocp[3]);
  allocp[4] = (q)(struct goalrec*)&allocp[0];
  allocp[5] = (q)(&predicate_klicio_xtermout_2);
  allocp[6] = x3 = makeref(&allocp[6]);
  x4 = arg(x0, 1);
  allocp[7] = x4;
  allocp[8] = makesym(functor_append__open_2);
  x6 = arg(x0, 0);
  allocp[9] = x6;
  allocp[10] = x3;
  x5 = makefunctor(&allocp[8]);
  a0 = x5;
  a2 = x2;
  qp = (struct goalrec*)&allocp[4];
  allocp += 11;
  execute(send__unix_3_0);
  goto send__unix_3_ext_interrupt;
 case functor_update__open_2:
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klicio_xklicio_2);
  x1 = cdr_of(a0);
  allocp[2] = x1;
  allocp[3] = x2 = makeref(&allocp[3]);
  allocp[4] = (q)(struct goalrec*)&allocp[0];
  allocp[5] = (q)(&predicate_klicio_xtermupdt_2);
  allocp[6] = x3 = makeref(&allocp[6]);
  x4 = arg(x0, 1);
  allocp[7] = x4;
  allocp[8] = makesym(functor_update__open_2);
  x6 = arg(x0, 0);
  allocp[9] = x6;
  allocp[10] = x3;
  x5 = makefunctor(&allocp[8]);
  a0 = x5;
  a2 = x2;
  qp = (struct goalrec*)&allocp[4];
  allocp += 11;
  execute(send__unix_3_0);
  goto send__unix_3_ext_interrupt;
 case functor_connect_2:
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klicio_xklicio_2);
  x1 = cdr_of(a0);
  allocp[2] = x1;
  allocp[3] = x2 = makeref(&allocp[3]);
  allocp[4] = (q)(struct goalrec*)&allocp[0];
  allocp[5] = (q)(&predicate_klicio_xtermupdt_2);
  allocp[6] = x3 = makeref(&allocp[6]);
  x4 = arg(x0, 1);
  allocp[7] = x4;
  allocp[8] = makesym(functor_connect_2);
  x6 = arg(x0, 0);
  allocp[9] = x6;
  allocp[10] = x3;
  x5 = makefunctor(&allocp[8]);
  a0 = x5;
  a2 = x2;
  qp = (struct goalrec*)&allocp[4];
  allocp += 11;
  execute(send__unix_3_0);
  goto send__unix_3_ext_interrupt;
 case functor_connect2_2:
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klicio_xklicio_2);
  x1 = cdr_of(a0);
  allocp[2] = x1;
  allocp[3] = x2 = makeref(&allocp[3]);
  allocp[4] = (q)(struct goalrec*)&allocp[0];
  allocp[5] = (q)(&predicate_klicio_xtermupdt_2);
  allocp[6] = x3 = makeref(&allocp[6]);
  x4 = arg(x0, 1);
  allocp[7] = x4;
  allocp[8] = makesym(functor_connect2_2);
  x6 = arg(x0, 0);
  allocp[9] = x6;
  allocp[10] = x3;
  x5 = makefunctor(&allocp[8]);
  a0 = x5;
  a2 = x2;
  qp = (struct goalrec*)&allocp[4];
  allocp += 11;
  execute(send__unix_3_0);
  goto send__unix_3_ext_interrupt;
 case functor_fork__with__pipes_1:
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klicio_xklicio_2);
  x1 = cdr_of(a0);
  allocp[2] = x1;
  allocp[3] = x2 = makeref(&allocp[3]);
  allocp[4] = (q)(struct goalrec*)&allocp[0];
  allocp[5] = (q)(&predicate_klicio_xfork__io_2);
  allocp[6] = x3 = makeref(&allocp[6]);
  x4 = arg(x0, 0);
  allocp[7] = x4;
  allocp[8] = makesym(functor_fork__with__pipes_1);
  allocp[9] = x3;
  x5 = makefunctor(&allocp[8]);
  a0 = x5;
  a2 = x2;
  qp = (struct goalrec*)&allocp[4];
  allocp += 10;
  execute(send__unix_3_0);
  goto send__unix_3_ext_interrupt;
 case functor_bind_2:
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klicio_xklicio_2);
  x1 = cdr_of(a0);
  allocp[2] = x1;
  allocp[3] = x2 = makeref(&allocp[3]);
  allocp[4] = (q)(struct goalrec*)&allocp[0];
  allocp[5] = (q)(&predicate_klicio_xbind_2);
  allocp[6] = x3 = makeref(&allocp[6]);
  x4 = arg(x0, 1);
  allocp[7] = x4;
  allocp[8] = makesym(functor_bind_2);
  x6 = arg(x0, 0);
  allocp[9] = x6;
  allocp[10] = x3;
  x5 = makefunctor(&allocp[8]);
  a0 = x5;
  a2 = x2;
  qp = (struct goalrec*)&allocp[4];
  allocp += 11;
  execute(send__unix_3_0);
  goto send__unix_3_ext_interrupt;
  };
  goto klicio_2_6;
 case VARREF:
  deref_and_jump(x0,klicio_2_2);
  *reasonp++ =  x0;
  goto klicio_2_6;
  };
  goto klicio_2_6;
 case ATOMIC:
  if (a0 != NILATOM) goto klicio_2_6;
  a0 = a1;
  execute(close__unix_1_0);
  goto close__unix_1_ext_interrupt;
 case VARREF:
  deref_and_jump(a0,klicio_2_1);
  *reasonp++ =  a0;
  goto klicio_2_6;
  };
  goto klicio_2_6;
 klicio_2_6:
  otherwise(klicio_2_interrupt);
 klicio_2_7:
  switch (ptagof(a0)) {
 case CONS:
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klicio_xklicio_2);
  x0 = cdr_of(a0);
  allocp[2] = x0;
  allocp[3] = x1 = makeref(&allocp[3]);
  a0 = car_of(a0);
  a2 = x1;
  qp = (struct goalrec*)&allocp[0];
  allocp += 4;
  execute(send__unix_3_0);
  goto send__unix_3_ext_interrupt;
 case VARREF:
  deref_and_jump(a0,klicio_2_7);
  *reasonp++ =  a0;
  goto klicio_2_interrupt;
  };
  goto klicio_2_interrupt;
 klicio_2_ext_interrupt:
  reasonp = 0l;
 klicio_2_interrupt:
  toppred = &predicate_klicio_xklicio_2;
  goto interrupt_2;
 }

 send__unix_3_top: {
  q x0, x1, x2, x3;
  a1 = qp->args[1];
  a2 = qp->args[2];
  qp = qp->next;
 send__unix_3_clear_reason:
  reasonp = reasons;
 send__unix_3_0:
 send__unix_3_1:
  switch (ptagof(a1)) {
 case ATOMIC:
  if (a1 != NILATOM) goto send__unix_3_interrupt;
  allocp[0] = makesym(functor_unix_1);
  allocp[1] = x1 = makeref(&allocp[1]);
  x0 = makefunctor(&allocp[0]);
  allocp += 2;
  unify_value(a2, x0);
  allocp[0] = x1;
  allocp[1] = a0;
  x2 = makecons(&allocp[0]);
  allocp[2] = (q)qp;
  allocp[3] = (q)(&predicate_unix_xunix_1);
  allocp[4] = x2;
  qp = (struct goalrec*)&allocp[2];
  allocp += 5;
  proceed();
 case FUNCTOR:
  if (functor_of(a1) != makesym(functor_unix_1)) goto send__unix_3_interrupt;
  allocp[0] = x1 = makeref(&allocp[0]);
  allocp[1] = a0;
  x0 = makecons(&allocp[0]);
  x2 = arg(a1, 0);
  allocp += 2;
  unify_value(x2, x0);
  allocp[0] = makesym(functor_unix_1);
  allocp[1] = x1;
  x3 = makefunctor(&allocp[0]);
  allocp += 2;
  unify_value(a2, x3);
  proceed();
 case VARREF:
  deref_and_jump(a1,send__unix_3_1);
  *reasonp++ =  a1;
  goto send__unix_3_interrupt;
  };
  goto send__unix_3_interrupt;
 send__unix_3_ext_interrupt:
  reasonp = 0l;
 send__unix_3_interrupt:
  toppred = &predicate_klicio_xsend__unix_3;
  goto interrupt_3;
 }

 close__unix_1_top: {
  q x0, x1;
  qp = qp->next;
 close__unix_1_clear_reason:
  reasonp = reasons;
 close__unix_1_0:
 close__unix_1_1:
  switch (ptagof(a0)) {
 case ATOMIC:
  if (a0 != NILATOM) goto close__unix_1_interrupt;
  proceed();
 case FUNCTOR:
  if (functor_of(a0) != makesym(functor_unix_1)) goto close__unix_1_interrupt;
  x0 = arg(a0, 0);
  x1 = NILATOM;
  unify_value(x0, x1);
  proceed();
 case VARREF:
  deref_and_jump(a0,close__unix_1_1);
  *reasonp++ =  a0;
  goto close__unix_1_interrupt;
  };
  goto close__unix_1_interrupt;
 close__unix_1_ext_interrupt:
  reasonp = 0l;
 close__unix_1_interrupt:
  toppred = &predicate_klicio_xclose__unix_1;
  goto interrupt_1;
 }

 termin_2_top: {
  q x0, x1, x2, x3, x4;
  a1 = qp->args[1];
  qp = qp->next;
 termin_2_clear_reason:
  reasonp = reasons;
 termin_2_0:
 termin_2_1:
  switch (ptagof(a0)) {
 case FUNCTOR:
  if (functor_of(a0) != makesym(functor_normal_1)) goto termin_2_2;
  allocp[0] = makesym(functor_normal_1);
  allocp[1] = x1 = makeref(&allocp[1]);
  x0 = makefunctor(&allocp[0]);
  allocp += 2;
  unify_value(a1, x0);
  allocp[0] = makesym(functor_s_2);
  allocp[1] = x3 = makeref(&allocp[1]);
  allocp[2] = makeint(0L);
  x2 = makefunctor(&allocp[0]);
  allocp[3] = (q)qp;
  allocp[4] = (q)(&predicate_klicio_xin_3);
  allocp[5] = x1;
  x4 = arg(a0, 0);
  allocp[6] = x4;
  allocp[7] = x2;
  a0 = x3;
  qp = (struct goalrec*)&allocp[3];
  allocp += 8;
  execute(default__operator__table_1_0);
  goto default__operator__table_1_ext_interrupt;
 case VARREF:
  deref_and_jump(a0,termin_2_1);
  *reasonp++ =  a0;
  goto termin_2_2;
  };
  goto termin_2_2;
 termin_2_2:
  otherwise(termin_2_interrupt);
  unify(a1, a0);
  proceed();
 termin_2_ext_interrupt:
  reasonp = 0l;
 termin_2_interrupt:
  goto interrupt_2;
 }

 termout_2_top: {
  q x0, x1, x2, x3, x4;
  a1 = qp->args[1];
  qp = qp->next;
 termout_2_clear_reason:
  reasonp = reasons;
 termout_2_0:
 termout_2_1:
  switch (ptagof(a0)) {
 case FUNCTOR:
  if (functor_of(a0) != makesym(functor_normal_1)) goto termout_2_2;
  allocp[0] = makesym(functor_normal_1);
  allocp[1] = x1 = makeref(&allocp[1]);
  x0 = makefunctor(&allocp[0]);
  allocp += 2;
  unify_value(a1, x0);
  allocp[0] = makesym(functor_s_2);
  allocp[1] = x3 = makeref(&allocp[1]);
  allocp[2] = makeint(0L);
  x2 = makefunctor(&allocp[0]);
  allocp[3] = (q)qp;
  allocp[4] = (q)(&predicate_klicio_xout_3);
  allocp[5] = x1;
  x4 = arg(a0, 0);
  allocp[6] = x4;
  allocp[7] = x2;
  a0 = x3;
  qp = (struct goalrec*)&allocp[3];
  allocp += 8;
  execute(default__operator__table_1_0);
  goto default__operator__table_1_ext_interrupt;
 case VARREF:
  deref_and_jump(a0,termout_2_1);
  *reasonp++ =  a0;
  goto termout_2_2;
  };
  goto termout_2_2;
 termout_2_2:
  otherwise(termout_2_interrupt);
  unify(a1, a0);
  proceed();
 termout_2_ext_interrupt:
  reasonp = 0l;
 termout_2_interrupt:
  toppred = &predicate_klicio_xtermout_2;
  goto interrupt_2;
 }

 termupdt_2_top: {
  q x0, x1, x2, x3, x4, x5, x6, x7;
  a1 = qp->args[1];
  qp = qp->next;
 termupdt_2_clear_reason:
  reasonp = reasons;
 termupdt_2_0:
 termupdt_2_1:
  switch (ptagof(a0)) {
 case FUNCTOR:
  switch (symval(functor_of(a0))) {
 case functor_normal_1:
  allocp[0] = makesym(functor_normal_1);
  allocp[1] = x1 = makeref(&allocp[1]);
  x0 = makefunctor(&allocp[0]);
  allocp += 2;
  unify_value(a1, x0);
  allocp[0] = makesym(functor_s_2);
  allocp[1] = x3 = makeref(&allocp[1]);
  allocp[2] = makeint(0L);
  x2 = makefunctor(&allocp[0]);
  allocp[3] = (q)qp;
  allocp[4] = (q)(&predicate_klicio_xupdt_3);
  allocp[5] = x1;
  x4 = arg(a0, 0);
  allocp[6] = x4;
  allocp[7] = x2;
  a0 = x3;
  qp = (struct goalrec*)&allocp[3];
  allocp += 8;
  execute(default__operator__table_1_0);
  goto default__operator__table_1_ext_interrupt;
 case functor_normal_2:
  allocp[0] = makesym(functor_normal_2);
  allocp[1] = x1 = makeref(&allocp[1]);
  allocp[2] = x2 = makeref(&allocp[2]);
  x0 = makefunctor(&allocp[0]);
  allocp += 3;
  unify_value(a1, x0);
  allocp[0] = makesym(functor_s_2);
  allocp[1] = x4 = makeref(&allocp[1]);
  allocp[2] = makeint(0L);
  x3 = makefunctor(&allocp[0]);
  allocp[3] = (q)qp;
  allocp[4] = (q)(&predicate_klicio_xupdt_3);
  allocp[5] = x2;
  x5 = arg(a0, 1);
  allocp[6] = x5;
  allocp[7] = x3;
  allocp[8] = makesym(functor_s_2);
  allocp[9] = x4;
  allocp[10] = makeint(0L);
  x6 = makefunctor(&allocp[8]);
  allocp[11] = (q)(struct goalrec*)&allocp[3];
  allocp[12] = (q)(&predicate_klicio_xupdt_3);
  allocp[13] = x1;
  x7 = arg(a0, 0);
  allocp[14] = x7;
  allocp[15] = x6;
  a0 = x4;
  qp = (struct goalrec*)&allocp[11];
  allocp += 16;
  execute(default__operator__table_1_0);
  goto default__operator__table_1_ext_interrupt;
  };
  goto termupdt_2_2;
 case VARREF:
  deref_and_jump(a0,termupdt_2_1);
  *reasonp++ =  a0;
  goto termupdt_2_2;
  };
  goto termupdt_2_2;
 termupdt_2_2:
  otherwise(termupdt_2_interrupt);
  unify(a1, a0);
  proceed();
 termupdt_2_ext_interrupt:
  reasonp = 0l;
 termupdt_2_interrupt:
  toppred = &predicate_klicio_xtermupdt_2;
  goto interrupt_2;
 }

 bind_2_top: {
  q x0, x1;
  a1 = qp->args[1];
  qp = qp->next;
 bind_2_clear_reason:
  reasonp = reasons;
 bind_2_0:
 bind_2_1:
  switch (ptagof(a0)) {
 case FUNCTOR:
  if (functor_of(a0) != makesym(functor_normal_1)) goto bind_2_2;
  allocp[0] = makesym(functor_normal_1);
  allocp[1] = x1 = makeref(&allocp[1]);
  x0 = makefunctor(&allocp[0]);
  allocp += 2;
  unify_value(a1, x0);
  a1 = arg(a0, 0);
  a0 = x1;
  execute(bound__sock_2_0);
  goto bound__sock_2_ext_interrupt;
 case VARREF:
  deref_and_jump(a0,bind_2_1);
  *reasonp++ =  a0;
  goto bind_2_2;
  };
  goto bind_2_2;
 bind_2_2:
  otherwise(bind_2_interrupt);
  unify(a1, a0);
  proceed();
 bind_2_ext_interrupt:
  reasonp = 0l;
 bind_2_interrupt:
  goto interrupt_2;
 }

 fork__io_2_top: {
  q x0, x1, x2, x3, x4, x5, x6, x7, x8, x9;
  a1 = qp->args[1];
  qp = qp->next;
 fork__io_2_clear_reason:
  reasonp = reasons;
 fork__io_2_0:
 fork__io_2_1:
  switch (ptagof(a0)) {
 case FUNCTOR:
  switch (symval(functor_of(a0))) {
 case functor_parent_3:
  allocp[0] = makesym(functor_parent_3);
  x1 = arg(a0, 0);
  allocp[1] = x1;
  allocp[2] = x2 = makeref(&allocp[2]);
  allocp[3] = x3 = makeref(&allocp[3]);
  x0 = makefunctor(&allocp[0]);
  allocp += 4;
  unify_value(a1, x0);
  allocp[0] = makesym(functor_normal_1);
  x5 = arg(a0, 1);
  allocp[1] = x5;
  x4 = makefunctor(&allocp[0]);
  allocp[2] = makesym(functor_normal_1);
  allocp[3] = x2;
  x6 = makefunctor(&allocp[2]);
  allocp[4] = (q)qp;
  allocp[5] = (q)(&predicate_klicio_xtermin_2);
  allocp[6] = x4;
  allocp[7] = x6;
  allocp[8] = makesym(functor_normal_1);
  x8 = arg(a0, 2);
  allocp[9] = x8;
  x7 = makefunctor(&allocp[8]);
  allocp[10] = makesym(functor_normal_1);
  allocp[11] = x3;
  x9 = makefunctor(&allocp[10]);
  a0 = x7;
  a1 = x9;
  qp = (struct goalrec*)&allocp[4];
  allocp += 12;
  execute(termout_2_0);
  goto termout_2_ext_interrupt;
 case functor_child_2:
  allocp[0] = makesym(functor_child_2);
  allocp[1] = x1 = makeref(&allocp[1]);
  allocp[2] = x2 = makeref(&allocp[2]);
  x0 = makefunctor(&allocp[0]);
  allocp += 3;
  unify_value(a1, x0);
  allocp[0] = makesym(functor_normal_1);
  x4 = arg(a0, 0);
  allocp[1] = x4;
  x3 = makefunctor(&allocp[0]);
  allocp[2] = makesym(functor_normal_1);
  allocp[3] = x1;
  x5 = makefunctor(&allocp[2]);
  allocp[4] = (q)qp;
  allocp[5] = (q)(&predicate_klicio_xtermin_2);
  allocp[6] = x3;
  allocp[7] = x5;
  allocp[8] = makesym(functor_normal_1);
  x7 = arg(a0, 1);
  allocp[9] = x7;
  x6 = makefunctor(&allocp[8]);
  allocp[10] = makesym(functor_normal_1);
  allocp[11] = x2;
  x8 = makefunctor(&allocp[10]);
  a0 = x6;
  a1 = x8;
  qp = (struct goalrec*)&allocp[4];
  allocp += 12;
  execute(termout_2_0);
  goto termout_2_ext_interrupt;
  };
  goto fork__io_2_interrupt;
 case VARREF:
  deref_and_jump(a0,fork__io_2_1);
  *reasonp++ =  a0;
  goto fork__io_2_interrupt;
  };
  goto fork__io_2_interrupt;
 fork__io_2_ext_interrupt:
  reasonp = 0l;
 fork__io_2_interrupt:
  goto interrupt_2;
 }

 bound__sock_2_top: {
  q x0, x1, x2, x3, x4, x5;
  a1 = qp->args[1];
  qp = qp->next;
 bound__sock_2_clear_reason:
  reasonp = reasons;
 bound__sock_2_0:
 bound__sock_2_1:
  switch (ptagof(a0)) {
 case CONS:
  x0 = car_of(a0);
 bound__sock_2_2:
  switch (ptagof(x0)) {
 case FUNCTOR:
  switch (symval(functor_of(x0))) {
 case functor_accept_1:
  allocp[0] = makesym(functor_accept_1);
  allocp[1] = x2 = makeref(&allocp[1]);
  x1 = makefunctor(&allocp[0]);
  allocp[2] = x4 = makeref(&allocp[2]);
  allocp[3] = x1;
  x3 = makecons(&allocp[2]);
  allocp += 4;
  unify_value(a1, x3);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klicio_xbound__sock_2);
  x5 = cdr_of(a0);
  allocp[2] = x5;
  allocp[3] = x4;
  a0 = x2;
  a1 = arg(x0, 0);
  qp = (struct goalrec*)&allocp[0];
  allocp += 4;
  execute(termupdt_2_0);
  goto termupdt_2_ext_interrupt;
 case functor_accept2_1:
  allocp[0] = makesym(functor_accept2_1);
  allocp[1] = x2 = makeref(&allocp[1]);
  x1 = makefunctor(&allocp[0]);
  allocp[2] = x4 = makeref(&allocp[2]);
  allocp[3] = x1;
  x3 = makecons(&allocp[2]);
  allocp += 4;
  unify_value(a1, x3);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klicio_xbound__sock_2);
  x5 = cdr_of(a0);
  allocp[2] = x5;
  allocp[3] = x4;
  a0 = x2;
  a1 = arg(x0, 0);
  qp = (struct goalrec*)&allocp[0];
  allocp += 4;
  execute(termupdt_2_0);
  goto termupdt_2_ext_interrupt;
  };
  goto bound__sock_2_interrupt;
 case VARREF:
  deref_and_jump(x0,bound__sock_2_2);
  *reasonp++ =  x0;
  goto bound__sock_2_interrupt;
  };
  goto bound__sock_2_interrupt;
 case ATOMIC:
  if (a0 != NILATOM) goto bound__sock_2_interrupt;
  x0 = NILATOM;
  unify_value(a1, x0);
  proceed();
 case VARREF:
  deref_and_jump(a0,bound__sock_2_1);
  *reasonp++ =  a0;
  goto bound__sock_2_interrupt;
  };
  goto bound__sock_2_interrupt;
 bound__sock_2_ext_interrupt:
  reasonp = 0l;
 bound__sock_2_interrupt:
  toppred = &predicate_klicio_xbound__sock_2;
  goto interrupt_2;
 }

 in_3_top: {
  q x0, x1, x2, x3, x4, x5;
  a1 = qp->args[1];
  a2 = qp->args[2];
  qp = qp->next;
 in_3_clear_reason:
  reasonp = reasons;
 in_3_0:
 in_3_1:
  switch (ptagof(a0)) {
 case CONS:
  x0 = car_of(a0);
 in_3_2:
  switch (ptagof(x0)) {
 case FUNCTOR:
  switch (symval(functor_of(x0))) {
 case functor_gett_1:
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klicio_xin_3);
  x1 = cdr_of(a0);
  allocp[2] = x1;
  allocp[3] = x2 = makeref(&allocp[3]);
  allocp[4] = a2;
  allocp[5] = (q)(struct goalrec*)&allocp[0];
  allocp[6] = (q)(&predicate_klic__reader_xparse__one__term__reporting__errors_4);
  allocp[7] = a1;
  allocp[8] = a2;
  x3 = arg(x0, 0);
  allocp[9] = x3;
  allocp[10] = x2;
  qp = (struct goalrec*)&allocp[5];
  allocp += 11;
  proceed();
 case functor_getwt_1:
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klicio_xin_3);
  x1 = cdr_of(a0);
  allocp[2] = x1;
  allocp[3] = x2 = makeref(&allocp[3]);
  allocp[4] = a2;
  allocp[5] = (q)(struct goalrec*)&allocp[0];
  allocp[6] = (q)(&predicate_klic__reader_xwparse__one__term__reporting__errors_4);
  allocp[7] = a1;
  allocp[8] = a2;
  x3 = arg(x0, 0);
  allocp[9] = x3;
  allocp[10] = x2;
  qp = (struct goalrec*)&allocp[5];
  allocp += 11;
  proceed();
 case functor_addop_3:
 in_3_3:
  switch (ptagof(a2)) {
 case FUNCTOR:
  if (functor_of(a2) != makesym(functor_s_2)) goto in_3_6;
  allocp[0] = makesym(functor_s_2);
  allocp[1] = x2 = makeref(&allocp[1]);
  x3 = arg(a2, 1);
  allocp[2] = x3;
  x1 = makefunctor(&allocp[0]);
  allocp[3] = (q)qp;
  allocp[4] = (q)(&predicate_klicio_xin_3);
  x4 = cdr_of(a0);
  allocp[5] = x4;
  allocp[6] = a1;
  allocp[7] = x1;
  a0 = arg(x0, 1);
  a1 = arg(x0, 0);
  x5 = arg(x0, 2);
  a3 = arg(a2, 0);
  a4 = x2;
  a2 = x5;
  qp = (struct goalrec*)&allocp[3];
  allocp += 8;
  execute(addop_5_0);
  goto addop_5_ext_interrupt;
 case VARREF:
  deref_and_jump(a2,in_3_3);
  *reasonp++ =  a2;
  goto in_3_6;
  };
  goto in_3_6;
 case functor_rmop_2:
 in_3_4:
  switch (ptagof(a2)) {
 case FUNCTOR:
  if (functor_of(a2) != makesym(functor_s_2)) goto in_3_6;
  allocp[0] = makesym(functor_s_2);
  allocp[1] = x2 = makeref(&allocp[1]);
  x3 = arg(a2, 1);
  allocp[2] = x3;
  x1 = makefunctor(&allocp[0]);
  allocp[3] = (q)qp;
  allocp[4] = (q)(&predicate_klicio_xin_3);
  x4 = cdr_of(a0);
  allocp[5] = x4;
  allocp[6] = a1;
  allocp[7] = x1;
  a0 = arg(x0, 1);
  a1 = arg(x0, 0);
  a2 = arg(a2, 0);
  a3 = x2;
  qp = (struct goalrec*)&allocp[3];
  allocp += 8;
  execute(rmop_4_0);
  goto rmop_4_ext_interrupt;
 case VARREF:
  deref_and_jump(a2,in_3_4);
  *reasonp++ =  a2;
  goto in_3_6;
  };
  goto in_3_6;
 case functor_on__error_1:
 in_3_5:
  switch (ptagof(a2)) {
 case FUNCTOR:
  if (functor_of(a2) != makesym(functor_s_2)) goto in_3_6;
  allocp[0] = makesym(functor_s_2);
  x2 = arg(a2, 0);
  allocp[1] = x2;
  x3 = arg(x0, 0);
  allocp[2] = x3;
  x1 = makefunctor(&allocp[0]);
  a0 = cdr_of(a0);
  a2 = x1;
  allocp += 3;
  execute(in_3_0);
  goto in_3_ext_interrupt;
 case VARREF:
  deref_and_jump(a2,in_3_5);
  *reasonp++ =  a2;
  goto in_3_6;
  };
  goto in_3_6;
  };
  goto in_3_6;
 case VARREF:
  deref_and_jump(x0,in_3_2);
  *reasonp++ =  x0;
  goto in_3_6;
  };
  goto in_3_6;
 case ATOMIC:
  if (a0 != NILATOM) goto in_3_6;
  x0 = NILATOM;
  unify_value(a1, x0);
  proceed();
 case VARREF:
  deref_and_jump(a0,in_3_1);
  *reasonp++ =  a0;
  goto in_3_6;
  };
  goto in_3_6;
 in_3_6:
  otherwise(in_3_interrupt);
 in_3_7:
  switch (ptagof(a0)) {
 case CONS:
  allocp[0] = x1 = makeref(&allocp[0]);
  x2 = car_of(a0);
  allocp[1] = x2;
  x0 = makecons(&allocp[0]);
  allocp += 2;
  unify_value(a1, x0);
  a0 = cdr_of(a0);
  a1 = x1;
  execute(in_3_0);
  goto in_3_ext_interrupt;
 case VARREF:
  deref_and_jump(a0,in_3_7);
  *reasonp++ =  a0;
  goto in_3_interrupt;
  };
  goto in_3_interrupt;
 in_3_ext_interrupt:
  reasonp = 0l;
 in_3_interrupt:
  goto interrupt_3;
 }

 out_3_top: {
  q x0, x1, x2, x3, x4, x5;
  a1 = qp->args[1];
  a2 = qp->args[2];
  qp = qp->next;
 out_3_clear_reason:
  reasonp = reasons;
 out_3_0:
 out_3_1:
  switch (ptagof(a0)) {
 case CONS:
  x0 = car_of(a0);
 out_3_2:
  switch (ptagof(x0)) {
 case ATOMIC:
  if (x0 != makesym(atom_nl)) goto out_3_6;
  allocp[0] = x2 = makeref(&allocp[0]);
  allocp[1] = makefunctor(funct_const_185);
  x1 = makecons(&allocp[0]);
  allocp += 2;
  unify_value(a1, x1);
  a0 = cdr_of(a0);
  a1 = x2;
  execute(out_3_0);
  goto out_3_ext_interrupt;
 case FUNCTOR:
  switch (symval(functor_of(x0))) {
 case functor_putt_1:
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klicio_xout_3);
  x1 = cdr_of(a0);
  allocp[2] = x1;
  allocp[3] = x2 = makeref(&allocp[3]);
  allocp[4] = a2;
  allocp[5] = (q)(struct goalrec*)&allocp[0];
  allocp[6] = (q)(&predicate_unparser_xunparse_4);
  x3 = arg(x0, 0);
  allocp[7] = x3;
  allocp[8] = a2;
  allocp[9] = a1;
  allocp[10] = x2;
  qp = (struct goalrec*)&allocp[5];
  allocp += 11;
  proceed();
 case functor_puttq_1:
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klicio_xout_3);
  x1 = cdr_of(a0);
  allocp[2] = x1;
  allocp[3] = x2 = makeref(&allocp[3]);
  allocp[4] = a2;
  allocp[5] = (q)(struct goalrec*)&allocp[0];
  allocp[6] = (q)(&predicate_unparser_xunparse_4);
  x3 = arg(x0, 0);
  allocp[7] = x3;
  allocp[8] = a2;
  allocp[9] = a1;
  allocp[10] = x2;
  qp = (struct goalrec*)&allocp[5];
  allocp += 11;
  proceed();
 case functor_putwt_1:
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klicio_xout_3);
  x1 = cdr_of(a0);
  allocp[2] = x1;
  allocp[3] = x2 = makeref(&allocp[3]);
  allocp[4] = a2;
  allocp[5] = (q)(struct goalrec*)&allocp[0];
  allocp[6] = (q)(&predicate_unparser_xunwparse_4);
  x3 = arg(x0, 0);
  allocp[7] = x3;
  allocp[8] = a2;
  allocp[9] = a1;
  allocp[10] = x2;
  qp = (struct goalrec*)&allocp[5];
  allocp += 11;
  proceed();
 case functor_putwtq_1:
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klicio_xout_3);
  x1 = cdr_of(a0);
  allocp[2] = x1;
  allocp[3] = x2 = makeref(&allocp[3]);
  allocp[4] = a2;
  allocp[5] = (q)(struct goalrec*)&allocp[0];
  allocp[6] = (q)(&predicate_unparser_xunwparse_4);
  x3 = arg(x0, 0);
  allocp[7] = x3;
  allocp[8] = a2;
  allocp[9] = a1;
  allocp[10] = x2;
  qp = (struct goalrec*)&allocp[5];
  allocp += 11;
  proceed();
 case functor_addop_3:
 out_3_3:
  switch (ptagof(a2)) {
 case FUNCTOR:
  if (functor_of(a2) != makesym(functor_s_2)) goto out_3_6;
  allocp[0] = makesym(functor_s_2);
  allocp[1] = x2 = makeref(&allocp[1]);
  x3 = arg(a2, 1);
  allocp[2] = x3;
  x1 = makefunctor(&allocp[0]);
  allocp[3] = (q)qp;
  allocp[4] = (q)(&predicate_klicio_xout_3);
  x4 = cdr_of(a0);
  allocp[5] = x4;
  allocp[6] = a1;
  allocp[7] = x1;
  a0 = arg(x0, 1);
  a1 = arg(x0, 0);
  x5 = arg(x0, 2);
  a3 = arg(a2, 0);
  a4 = x2;
  a2 = x5;
  qp = (struct goalrec*)&allocp[3];
  allocp += 8;
  execute(addop_5_0);
  goto addop_5_ext_interrupt;
 case VARREF:
  deref_and_jump(a2,out_3_3);
  *reasonp++ =  a2;
  goto out_3_6;
  };
  goto out_3_6;
 case functor_rmop_2:
 out_3_4:
  switch (ptagof(a2)) {
 case FUNCTOR:
  if (functor_of(a2) != makesym(functor_s_2)) goto out_3_6;
  allocp[0] = makesym(functor_s_2);
  allocp[1] = x2 = makeref(&allocp[1]);
  x3 = arg(a2, 1);
  allocp[2] = x3;
  x1 = makefunctor(&allocp[0]);
  allocp[3] = (q)qp;
  allocp[4] = (q)(&predicate_klicio_xout_3);
  x4 = cdr_of(a0);
  allocp[5] = x4;
  allocp[6] = a1;
  allocp[7] = x1;
  a0 = arg(x0, 1);
  a1 = arg(x0, 0);
  a2 = arg(a2, 0);
  a3 = x2;
  qp = (struct goalrec*)&allocp[3];
  allocp += 8;
  execute(rmop_4_0);
  goto rmop_4_ext_interrupt;
 case VARREF:
  deref_and_jump(a2,out_3_4);
  *reasonp++ =  a2;
  goto out_3_6;
  };
  goto out_3_6;
 case functor_on__error_1:
 out_3_5:
  switch (ptagof(a2)) {
 case FUNCTOR:
  if (functor_of(a2) != makesym(functor_s_2)) goto out_3_6;
  allocp[0] = makesym(functor_s_2);
  x2 = arg(a2, 0);
  allocp[1] = x2;
  x3 = arg(x0, 0);
  allocp[2] = x3;
  x1 = makefunctor(&allocp[0]);
  a0 = cdr_of(a0);
  a2 = x1;
  allocp += 3;
  execute(out_3_0);
  goto out_3_ext_interrupt;
 case VARREF:
  deref_and_jump(a2,out_3_5);
  *reasonp++ =  a2;
  goto out_3_6;
  };
  goto out_3_6;
  };
  goto out_3_6;
 case VARREF:
  deref_and_jump(x0,out_3_2);
  *reasonp++ =  x0;
  goto out_3_6;
  };
  goto out_3_6;
 case ATOMIC:
  if (a0 != NILATOM) goto out_3_6;
  x0 = NILATOM;
  unify_value(a1, x0);
  proceed();
 case VARREF:
  deref_and_jump(a0,out_3_1);
  *reasonp++ =  a0;
  goto out_3_6;
  };
  goto out_3_6;
 out_3_6:
  otherwise(out_3_interrupt);
 out_3_7:
  switch (ptagof(a0)) {
 case CONS:
 out_3_8:
  switch (ptagof(a2)) {
 case FUNCTOR:
  if (functor_of(a2) != makesym(functor_s_2)) goto out_3_interrupt;
  allocp[0] = x1 = makeref(&allocp[0]);
  x2 = car_of(a0);
  allocp[1] = x2;
  x0 = makecons(&allocp[0]);
  allocp += 2;
  unify_value(a1, x0);
  allocp[0] = makesym(functor_s_2);
  x4 = arg(a2, 0);
  allocp[1] = x4;
  x5 = arg(a2, 1);
  allocp[2] = x5;
  x3 = makefunctor(&allocp[0]);
  a0 = cdr_of(a0);
  a1 = x1;
  a2 = x3;
  allocp += 3;
  execute(out_3_0);
  goto out_3_ext_interrupt;
 case VARREF:
  deref_and_jump(a2,out_3_8);
  *reasonp++ =  a2;
  goto out_3_interrupt;
  };
  goto out_3_interrupt;
 case VARREF:
  deref_and_jump(a0,out_3_7);
  *reasonp++ =  a0;
  goto out_3_interrupt;
  };
  goto out_3_interrupt;
 out_3_ext_interrupt:
  reasonp = 0l;
 out_3_interrupt:
  goto interrupt_3;
 }

 updt_3_top: {
  q x0, x1, x2, x3, x4, x5;
  a1 = qp->args[1];
  a2 = qp->args[2];
  qp = qp->next;
 updt_3_clear_reason:
  reasonp = reasons;
 updt_3_0:
 updt_3_1:
  switch (ptagof(a0)) {
 case CONS:
  x0 = car_of(a0);
 updt_3_2:
  switch (ptagof(x0)) {
 case ATOMIC:
  if (x0 != makesym(atom_nl)) goto updt_3_6;
  allocp[0] = x2 = makeref(&allocp[0]);
  allocp[1] = makefunctor(funct_const_185);
  x1 = makecons(&allocp[0]);
  allocp += 2;
  unify_value(a1, x1);
  a0 = cdr_of(a0);
  a1 = x2;
  execute(updt_3_0);
  goto updt_3_ext_interrupt;
 case FUNCTOR:
  switch (symval(functor_of(x0))) {
 case functor_gett_1:
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klicio_xupdt_3);
  x1 = cdr_of(a0);
  allocp[2] = x1;
  allocp[3] = x2 = makeref(&allocp[3]);
  allocp[4] = a2;
  allocp[5] = (q)(struct goalrec*)&allocp[0];
  allocp[6] = (q)(&predicate_klic__reader_xparse__one__term__reporting__errors_4);
  allocp[7] = a1;
  allocp[8] = a2;
  x3 = arg(x0, 0);
  allocp[9] = x3;
  allocp[10] = x2;
  qp = (struct goalrec*)&allocp[5];
  allocp += 11;
  proceed();
 case functor_getwt_1:
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klicio_xupdt_3);
  x1 = cdr_of(a0);
  allocp[2] = x1;
  allocp[3] = x2 = makeref(&allocp[3]);
  allocp[4] = a2;
  allocp[5] = (q)(struct goalrec*)&allocp[0];
  allocp[6] = (q)(&predicate_klic__reader_xwparse__one__term__reporting__errors_4);
  allocp[7] = a1;
  allocp[8] = a2;
  x3 = arg(x0, 0);
  allocp[9] = x3;
  allocp[10] = x2;
  qp = (struct goalrec*)&allocp[5];
  allocp += 11;
  proceed();
 case functor_putt_1:
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klicio_xupdt_3);
  x1 = cdr_of(a0);
  allocp[2] = x1;
  allocp[3] = x2 = makeref(&allocp[3]);
  allocp[4] = a2;
  allocp[5] = (q)(struct goalrec*)&allocp[0];
  allocp[6] = (q)(&predicate_unparser_xunparse_4);
  x3 = arg(x0, 0);
  allocp[7] = x3;
  allocp[8] = a2;
  allocp[9] = a1;
  allocp[10] = x2;
  qp = (struct goalrec*)&allocp[5];
  allocp += 11;
  proceed();
 case functor_puttq_1:
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klicio_xupdt_3);
  x1 = cdr_of(a0);
  allocp[2] = x1;
  allocp[3] = x2 = makeref(&allocp[3]);
  allocp[4] = a2;
  allocp[5] = (q)(struct goalrec*)&allocp[0];
  allocp[6] = (q)(&predicate_unparser_xunparse_4);
  x3 = arg(x0, 0);
  allocp[7] = x3;
  allocp[8] = a2;
  allocp[9] = a1;
  allocp[10] = x2;
  qp = (struct goalrec*)&allocp[5];
  allocp += 11;
  proceed();
 case functor_putwt_1:
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klicio_xupdt_3);
  x1 = cdr_of(a0);
  allocp[2] = x1;
  allocp[3] = x2 = makeref(&allocp[3]);
  allocp[4] = a2;
  allocp[5] = (q)(struct goalrec*)&allocp[0];
  allocp[6] = (q)(&predicate_unparser_xunwparse_4);
  x3 = arg(x0, 0);
  allocp[7] = x3;
  allocp[8] = a2;
  allocp[9] = a1;
  allocp[10] = x2;
  qp = (struct goalrec*)&allocp[5];
  allocp += 11;
  proceed();
 case functor_putwtq_1:
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klicio_xupdt_3);
  x1 = cdr_of(a0);
  allocp[2] = x1;
  allocp[3] = x2 = makeref(&allocp[3]);
  allocp[4] = a2;
  allocp[5] = (q)(struct goalrec*)&allocp[0];
  allocp[6] = (q)(&predicate_unparser_xunwparse_4);
  x3 = arg(x0, 0);
  allocp[7] = x3;
  allocp[8] = a2;
  allocp[9] = a1;
  allocp[10] = x2;
  qp = (struct goalrec*)&allocp[5];
  allocp += 11;
  proceed();
 case functor_addop_3:
 updt_3_3:
  switch (ptagof(a2)) {
 case FUNCTOR:
  if (functor_of(a2) != makesym(functor_s_2)) goto updt_3_6;
  allocp[0] = makesym(functor_s_2);
  allocp[1] = x2 = makeref(&allocp[1]);
  x3 = arg(a2, 1);
  allocp[2] = x3;
  x1 = makefunctor(&allocp[0]);
  allocp[3] = (q)qp;
  allocp[4] = (q)(&predicate_klicio_xupdt_3);
  x4 = cdr_of(a0);
  allocp[5] = x4;
  allocp[6] = a1;
  allocp[7] = x1;
  a0 = arg(x0, 1);
  a1 = arg(x0, 0);
  x5 = arg(x0, 2);
  a3 = arg(a2, 0);
  a4 = x2;
  a2 = x5;
  qp = (struct goalrec*)&allocp[3];
  allocp += 8;
  execute(addop_5_0);
  goto addop_5_ext_interrupt;
 case VARREF:
  deref_and_jump(a2,updt_3_3);
  *reasonp++ =  a2;
  goto updt_3_6;
  };
  goto updt_3_6;
 case functor_rmop_2:
 updt_3_4:
  switch (ptagof(a2)) {
 case FUNCTOR:
  if (functor_of(a2) != makesym(functor_s_2)) goto updt_3_6;
  allocp[0] = makesym(functor_s_2);
  allocp[1] = x2 = makeref(&allocp[1]);
  x3 = arg(a2, 1);
  allocp[2] = x3;
  x1 = makefunctor(&allocp[0]);
  allocp[3] = (q)qp;
  allocp[4] = (q)(&predicate_klicio_xupdt_3);
  x4 = cdr_of(a0);
  allocp[5] = x4;
  allocp[6] = a1;
  allocp[7] = x1;
  a0 = arg(x0, 1);
  a1 = arg(x0, 0);
  a2 = arg(a2, 0);
  a3 = x2;
  qp = (struct goalrec*)&allocp[3];
  allocp += 8;
  execute(rmop_4_0);
  goto rmop_4_ext_interrupt;
 case VARREF:
  deref_and_jump(a2,updt_3_4);
  *reasonp++ =  a2;
  goto updt_3_6;
  };
  goto updt_3_6;
 case functor_on__error_1:
 updt_3_5:
  switch (ptagof(a2)) {
 case FUNCTOR:
  if (functor_of(a2) != makesym(functor_s_2)) goto updt_3_6;
  allocp[0] = makesym(functor_s_2);
  x2 = arg(a2, 0);
  allocp[1] = x2;
  x3 = arg(x0, 0);
  allocp[2] = x3;
  x1 = makefunctor(&allocp[0]);
  a0 = cdr_of(a0);
  a2 = x1;
  allocp += 3;
  execute(updt_3_0);
  goto updt_3_ext_interrupt;
 case VARREF:
  deref_and_jump(a2,updt_3_5);
  *reasonp++ =  a2;
  goto updt_3_6;
  };
  goto updt_3_6;
  };
  goto updt_3_6;
 case VARREF:
  deref_and_jump(x0,updt_3_2);
  *reasonp++ =  x0;
  goto updt_3_6;
  };
  goto updt_3_6;
 case ATOMIC:
  if (a0 != NILATOM) goto updt_3_6;
  x0 = NILATOM;
  unify_value(a1, x0);
  proceed();
 case VARREF:
  deref_and_jump(a0,updt_3_1);
  *reasonp++ =  a0;
  goto updt_3_6;
  };
  goto updt_3_6;
 updt_3_6:
  otherwise(updt_3_interrupt);
 updt_3_7:
  switch (ptagof(a0)) {
 case CONS:
 updt_3_8:
  switch (ptagof(a2)) {
 case FUNCTOR:
  if (functor_of(a2) != makesym(functor_s_2)) goto updt_3_interrupt;
  allocp[0] = x1 = makeref(&allocp[0]);
  x2 = car_of(a0);
  allocp[1] = x2;
  x0 = makecons(&allocp[0]);
  allocp += 2;
  unify_value(a1, x0);
  allocp[0] = makesym(functor_s_2);
  x4 = arg(a2, 0);
  allocp[1] = x4;
  x5 = arg(a2, 1);
  allocp[2] = x5;
  x3 = makefunctor(&allocp[0]);
  a0 = cdr_of(a0);
  a1 = x1;
  a2 = x3;
  allocp += 3;
  execute(updt_3_0);
  goto updt_3_ext_interrupt;
 case VARREF:
  deref_and_jump(a2,updt_3_8);
  *reasonp++ =  a2;
  goto updt_3_interrupt;
  };
  goto updt_3_interrupt;
 case VARREF:
  deref_and_jump(a0,updt_3_7);
  *reasonp++ =  a0;
  goto updt_3_interrupt;
  };
  goto updt_3_interrupt;
 updt_3_ext_interrupt:
  reasonp = 0l;
 updt_3_interrupt:
  goto interrupt_3;
 }

 addop_5_top: {
  q x0, x1, x2, x3, x4;
  a1 = qp->args[1];
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  qp = qp->next;
 addop_5_clear_reason:
  reasonp = reasons;
 addop_5_0:
 addop_5_1:
  switch (ptagof(a3)) {
 case FUNCTOR:
  if (functor_of(a3) != makesym(functor_ops_3)) goto addop_5_interrupt;
 addop_5_2:
  switch (ptagof(a0)) {
 case ATOMIC:
  if (isint(a0)) goto addop_5_interrupt;
  switch (symval(a0)) {
 case symval(makesym(atom_fx)):
  allocp[0] = makesym(functor_ops_3);
  allocp[1] = x1 = makeref(&allocp[1]);
  x2 = arg(a3, 1);
  allocp[2] = x2;
  x3 = arg(a3, 2);
  allocp[3] = x3;
  x0 = makefunctor(&allocp[0]);
  allocp += 4;
  unify_value(a4, x0);
  x4 = a1;
  a1 = a0;
  a3 = arg(a3, 0);
  a4 = x1;
  a0 = x4;
  execute(addop2_5_0);
  goto addop2_5_ext_interrupt;
 case symval(makesym(atom_fy)):
  allocp[0] = makesym(functor_ops_3);
  allocp[1] = x1 = makeref(&allocp[1]);
  x2 = arg(a3, 1);
  allocp[2] = x2;
  x3 = arg(a3, 2);
  allocp[3] = x3;
  x0 = makefunctor(&allocp[0]);
  allocp += 4;
  unify_value(a4, x0);
  x4 = a1;
  a1 = a0;
  a3 = arg(a3, 0);
  a4 = x1;
  a0 = x4;
  execute(addop2_5_0);
  goto addop2_5_ext_interrupt;
 case symval(makesym(atom_xfx)):
  allocp[0] = makesym(functor_ops_3);
  x1 = arg(a3, 0);
  allocp[1] = x1;
  allocp[2] = x2 = makeref(&allocp[2]);
  x3 = arg(a3, 2);
  allocp[3] = x3;
  x0 = makefunctor(&allocp[0]);
  allocp += 4;
  unify_value(a4, x0);
  x4 = a1;
  a1 = a0;
  a3 = arg(a3, 1);
  a4 = x2;
  a0 = x4;
  execute(addop2_5_0);
  goto addop2_5_ext_interrupt;
 case symval(makesym(atom_xfy)):
  allocp[0] = makesym(functor_ops_3);
  x1 = arg(a3, 0);
  allocp[1] = x1;
  allocp[2] = x2 = makeref(&allocp[2]);
  x3 = arg(a3, 2);
  allocp[3] = x3;
  x0 = makefunctor(&allocp[0]);
  allocp += 4;
  unify_value(a4, x0);
  x4 = a1;
  a1 = a0;
  a3 = arg(a3, 1);
  a4 = x2;
  a0 = x4;
  execute(addop2_5_0);
  goto addop2_5_ext_interrupt;
 case symval(makesym(atom_yfx)):
  allocp[0] = makesym(functor_ops_3);
  x1 = arg(a3, 0);
  allocp[1] = x1;
  allocp[2] = x2 = makeref(&allocp[2]);
  x3 = arg(a3, 2);
  allocp[3] = x3;
  x0 = makefunctor(&allocp[0]);
  allocp += 4;
  unify_value(a4, x0);
  x4 = a1;
  a1 = a0;
  a3 = arg(a3, 1);
  a4 = x2;
  a0 = x4;
  execute(addop2_5_0);
  goto addop2_5_ext_interrupt;
 case symval(makesym(atom_xf)):
  allocp[0] = makesym(functor_ops_3);
  x1 = arg(a3, 0);
  allocp[1] = x1;
  x2 = arg(a3, 1);
  allocp[2] = x2;
  allocp[3] = x3 = makeref(&allocp[3]);
  x0 = makefunctor(&allocp[0]);
  allocp += 4;
  unify_value(a4, x0);
  x4 = a1;
  a1 = a0;
  a3 = arg(a3, 2);
  a4 = x3;
  a0 = x4;
  execute(addop2_5_0);
  goto addop2_5_ext_interrupt;
 case symval(makesym(atom_yf)):
  allocp[0] = makesym(functor_ops_3);
  x1 = arg(a3, 0);
  allocp[1] = x1;
  x2 = arg(a3, 1);
  allocp[2] = x2;
  allocp[3] = x3 = makeref(&allocp[3]);
  x0 = makefunctor(&allocp[0]);
  allocp += 4;
  unify_value(a4, x0);
  x4 = a1;
  a1 = a0;
  a3 = arg(a3, 2);
  a4 = x3;
  a0 = x4;
  execute(addop2_5_0);
  goto addop2_5_ext_interrupt;
  };
  goto addop_5_interrupt;
 case VARREF:
  deref_and_jump(a0,addop_5_2);
  *reasonp++ =  a0;
  goto addop_5_interrupt;
  };
  goto addop_5_interrupt;
 case VARREF:
  deref_and_jump(a3,addop_5_1);
  *reasonp++ =  a3;
  goto addop_5_interrupt;
  };
  goto addop_5_interrupt;
 addop_5_ext_interrupt:
  reasonp = 0l;
 addop_5_interrupt:
  toppred = &predicate_klicio_xaddop_5;
  goto interrupt_5;
 }

 addop2_5_top: {
  q x0, x1, x2, x3, x4;
  a1 = qp->args[1];
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  qp = qp->next;
 addop2_5_clear_reason:
  reasonp = reasons;
 addop2_5_0:
 addop2_5_1:
  switch (ptagof(a3)) {
 case CONS:
  x0 = car_of(a3);
 addop2_5_2:
  switch (ptagof(x0)) {
 case FUNCTOR:
  if (functor_of(x0) != makesym(functor_op_3)) goto addop2_5_7;
  x1 = arg(x0, 0);
 addop2_5_3:
  if (isref(x1)) goto addop2_5_6;
 addop2_5_4:
  if (isref(x1)) goto addop2_5_5;
  if_not_equal(x1, a0, addop2_5_7);
  allocp[0] = makesym(functor_op_3);
  allocp[1] = a0;
  allocp[2] = a1;
  allocp[3] = a2;
  x2 = makefunctor(&allocp[0]);
  x4 = cdr_of(a3);
  allocp[4] = x4;
  allocp[5] = x2;
  x3 = makecons(&allocp[4]);
  allocp += 6;
  unify_value(a4, x3);
  proceed();
 addop2_5_5:
  deref_and_jump(x1,addop2_5_4);
  *reasonp++ =  x1;
  goto addop2_5_7;
 addop2_5_6:
  deref_and_jump(x1,addop2_5_3);
  *reasonp++ =  x1;
  goto addop2_5_7;
 case VARREF:
  deref_and_jump(x0,addop2_5_2);
  *reasonp++ =  x0;
  goto addop2_5_7;
  };
  goto addop2_5_7;
 case ATOMIC:
  if (a3 != NILATOM) goto addop2_5_7;
  allocp[0] = makesym(functor_op_3);
  allocp[1] = a0;
  allocp[2] = a1;
  allocp[3] = a2;
  x0 = makefunctor(&allocp[0]);
  allocp[4] = NILATOM;
  allocp[5] = x0;
  x1 = makecons(&allocp[4]);
  allocp += 6;
  unify_value(a4, x1);
  proceed();
 case VARREF:
  deref_and_jump(a3,addop2_5_1);
  *reasonp++ =  a3;
  goto addop2_5_7;
  };
  goto addop2_5_7;
 addop2_5_7:
  otherwise(addop2_5_interrupt);
 addop2_5_8:
  switch (ptagof(a3)) {
 case CONS:
  allocp[0] = x1 = makeref(&allocp[0]);
  x2 = car_of(a3);
  allocp[1] = x2;
  x0 = makecons(&allocp[0]);
  allocp += 2;
  unify_value(a4, x0);
  a3 = cdr_of(a3);
  a4 = x1;
  execute(addop2_5_0);
  goto addop2_5_ext_interrupt;
 case VARREF:
  deref_and_jump(a3,addop2_5_8);
  *reasonp++ =  a3;
  goto addop2_5_interrupt;
  };
  goto addop2_5_interrupt;
 addop2_5_ext_interrupt:
  reasonp = 0l;
 addop2_5_interrupt:
  toppred = &predicate_klicio_xaddop2_5;
  goto interrupt_5;
 }

 rmop_4_top: {
  q x0, x1, x2, x3, x4;
  a1 = qp->args[1];
  a2 = qp->args[2];
  a3 = qp->args[3];
  qp = qp->next;
 rmop_4_clear_reason:
  reasonp = reasons;
 rmop_4_0:
 rmop_4_1:
  switch (ptagof(a2)) {
 case FUNCTOR:
  if (functor_of(a2) != makesym(functor_ops_3)) goto rmop_4_interrupt;
 rmop_4_2:
  switch (ptagof(a0)) {
 case ATOMIC:
  if (isint(a0)) goto rmop_4_interrupt;
  switch (symval(a0)) {
 case symval(makesym(atom_fx)):
  allocp[0] = makesym(functor_ops_3);
  allocp[1] = x1 = makeref(&allocp[1]);
  x2 = arg(a2, 1);
  allocp[2] = x2;
  x3 = arg(a2, 2);
  allocp[3] = x3;
  x0 = makefunctor(&allocp[0]);
  allocp += 4;
  unify_value(a3, x0);
  x4 = a1;
  a1 = a0;
  a2 = arg(a2, 0);
  a3 = x1;
  a0 = x4;
  execute(rmop2_4_0);
  goto rmop2_4_ext_interrupt;
 case symval(makesym(atom_fy)):
  allocp[0] = makesym(functor_ops_3);
  allocp[1] = x1 = makeref(&allocp[1]);
  x2 = arg(a2, 1);
  allocp[2] = x2;
  x3 = arg(a2, 2);
  allocp[3] = x3;
  x0 = makefunctor(&allocp[0]);
  allocp += 4;
  unify_value(a3, x0);
  x4 = a1;
  a1 = a0;
  a2 = arg(a2, 0);
  a3 = x1;
  a0 = x4;
  execute(rmop2_4_0);
  goto rmop2_4_ext_interrupt;
 case symval(makesym(atom_xfx)):
  allocp[0] = makesym(functor_ops_3);
  x1 = arg(a2, 0);
  allocp[1] = x1;
  allocp[2] = x2 = makeref(&allocp[2]);
  x3 = arg(a2, 2);
  allocp[3] = x3;
  x0 = makefunctor(&allocp[0]);
  allocp += 4;
  unify_value(a3, x0);
  x4 = a1;
  a1 = a0;
  a2 = arg(a2, 1);
  a3 = x2;
  a0 = x4;
  execute(rmop2_4_0);
  goto rmop2_4_ext_interrupt;
 case symval(makesym(atom_xfy)):
  allocp[0] = makesym(functor_ops_3);
  x1 = arg(a2, 0);
  allocp[1] = x1;
  allocp[2] = x2 = makeref(&allocp[2]);
  x3 = arg(a2, 2);
  allocp[3] = x3;
  x0 = makefunctor(&allocp[0]);
  allocp += 4;
  unify_value(a3, x0);
  x4 = a1;
  a1 = a0;
  a2 = arg(a2, 1);
  a3 = x2;
  a0 = x4;
  execute(rmop2_4_0);
  goto rmop2_4_ext_interrupt;
 case symval(makesym(atom_yfx)):
  allocp[0] = makesym(functor_ops_3);
  x1 = arg(a2, 0);
  allocp[1] = x1;
  allocp[2] = x2 = makeref(&allocp[2]);
  x3 = arg(a2, 2);
  allocp[3] = x3;
  x0 = makefunctor(&allocp[0]);
  allocp += 4;
  unify_value(a3, x0);
  x4 = a1;
  a1 = a0;
  a2 = arg(a2, 1);
  a3 = x2;
  a0 = x4;
  execute(rmop2_4_0);
  goto rmop2_4_ext_interrupt;
 case symval(makesym(atom_xf)):
  allocp[0] = makesym(functor_ops_3);
  x1 = arg(a2, 0);
  allocp[1] = x1;
  x2 = arg(a2, 1);
  allocp[2] = x2;
  allocp[3] = x3 = makeref(&allocp[3]);
  x0 = makefunctor(&allocp[0]);
  allocp += 4;
  unify_value(a3, x0);
  x4 = a1;
  a1 = a0;
  a2 = arg(a2, 2);
  a3 = x3;
  a0 = x4;
  execute(rmop2_4_0);
  goto rmop2_4_ext_interrupt;
 case symval(makesym(atom_yf)):
  allocp[0] = makesym(functor_ops_3);
  x1 = arg(a2, 0);
  allocp[1] = x1;
  x2 = arg(a2, 1);
  allocp[2] = x2;
  allocp[3] = x3 = makeref(&allocp[3]);
  x0 = makefunctor(&allocp[0]);
  allocp += 4;
  unify_value(a3, x0);
  x4 = a1;
  a1 = a0;
  a2 = arg(a2, 2);
  a3 = x3;
  a0 = x4;
  execute(rmop2_4_0);
  goto rmop2_4_ext_interrupt;
  };
  goto rmop_4_interrupt;
 case VARREF:
  deref_and_jump(a0,rmop_4_2);
  *reasonp++ =  a0;
  goto rmop_4_interrupt;
  };
  goto rmop_4_interrupt;
 case VARREF:
  deref_and_jump(a2,rmop_4_1);
  *reasonp++ =  a2;
  goto rmop_4_interrupt;
  };
  goto rmop_4_interrupt;
 rmop_4_ext_interrupt:
  reasonp = 0l;
 rmop_4_interrupt:
  toppred = &predicate_klicio_xrmop_4;
  goto interrupt_4;
 }

 rmop2_4_top: {
  q x0, x1, x2;
  a1 = qp->args[1];
  a2 = qp->args[2];
  a3 = qp->args[3];
  qp = qp->next;
 rmop2_4_clear_reason:
  reasonp = reasons;
 rmop2_4_0:
 rmop2_4_1:
  switch (ptagof(a2)) {
 case CONS:
  x0 = car_of(a2);
 rmop2_4_2:
  switch (ptagof(x0)) {
 case FUNCTOR:
  if (functor_of(x0) != makesym(functor_op_3)) goto rmop2_4_7;
  x1 = arg(x0, 0);
 rmop2_4_3:
  if (isref(x1)) goto rmop2_4_6;
 rmop2_4_4:
  if (isref(x1)) goto rmop2_4_5;
  if_not_equal(x1, a0, rmop2_4_7);
  x2 = cdr_of(a2);
  unify(a3, x2);
  proceed();
 rmop2_4_5:
  deref_and_jump(x1,rmop2_4_4);
  *reasonp++ =  x1;
  goto rmop2_4_7;
 rmop2_4_6:
  deref_and_jump(x1,rmop2_4_3);
  *reasonp++ =  x1;
  goto rmop2_4_7;
 case VARREF:
  deref_and_jump(x0,rmop2_4_2);
  *reasonp++ =  x0;
  goto rmop2_4_7;
  };
  goto rmop2_4_7;
 case ATOMIC:
  if (a2 != NILATOM) goto rmop2_4_7;
  x0 = NILATOM;
  unify_value(a3, x0);
  proceed();
 case VARREF:
  deref_and_jump(a2,rmop2_4_1);
  *reasonp++ =  a2;
  goto rmop2_4_7;
  };
  goto rmop2_4_7;
 rmop2_4_7:
  otherwise(rmop2_4_interrupt);
 rmop2_4_8:
  switch (ptagof(a2)) {
 case CONS:
  allocp[0] = x1 = makeref(&allocp[0]);
  x2 = car_of(a2);
  allocp[1] = x2;
  x0 = makecons(&allocp[0]);
  allocp += 2;
  unify_value(a3, x0);
  a2 = cdr_of(a2);
  a3 = x1;
  execute(rmop2_4_0);
  goto rmop2_4_ext_interrupt;
 case VARREF:
  deref_and_jump(a2,rmop2_4_8);
  *reasonp++ =  a2;
  goto rmop2_4_interrupt;
  };
  goto rmop2_4_interrupt;
 rmop2_4_ext_interrupt:
  reasonp = 0l;
 rmop2_4_interrupt:
  toppred = &predicate_klicio_xrmop2_4;
  goto interrupt_4;
 }

 default__operator__table_1_top: {
  q x0;
  qp = qp->next;
 default__operator__table_1_clear_reason:
  reasonp = reasons;
 default__operator__table_1_0:
  x0 = makefunctor(funct_const_184);
  unify_value(a0, x0);
  proceed();
 default__operator__table_1_ext_interrupt:
  reasonp = 0l;
 default__operator__table_1_interrupt:
  toppred = &predicate_klicio_xdefault__operator__table_1;
  goto interrupt_1;
 }

 output__stream_2_top: {
  q x0, x1, x2, x3;
  a1 = qp->args[1];
  qp = qp->next;
 output__stream_2_clear_reason:
  reasonp = reasons;
 output__stream_2_0:
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klicio_xstring__output_4);
  allocp[2] = x0 = makeref(&allocp[2]);
  allocp[3] = a1;
  allocp[4] = x1 = makeref(&allocp[4]);
  allocp[5] = x1;
  allocp[6] = makesym(functor_s_2);
  allocp[7] = x3 = makeref(&allocp[7]);
  allocp[8] = makeint(0L);
  x2 = makefunctor(&allocp[6]);
  allocp[9] = (q)(struct goalrec*)&allocp[0];
  allocp[10] = (q)(&predicate_klicio_xout_3);
  allocp[11] = a0;
  allocp[12] = x0;
  allocp[13] = x2;
  allocp[14] = (q)(struct goalrec*)&allocp[9];
  allocp[15] = (q)(&predicate_klicio_xdefault__operator__table_1);
  allocp[16] = x3;
  qp = (struct goalrec*)&allocp[14];
  allocp += 17;
  proceed();
 output__stream_2_ext_interrupt:
  reasonp = 0l;
 output__stream_2_interrupt:
  goto interrupt_2;
 }

 string__output_4_top: {
  q x0, x1, x2, x3, x4, x5, x6;
  a1 = qp->args[1];
  a2 = qp->args[2];
  a3 = qp->args[3];
  qp = qp->next;
 string__output_4_clear_reason:
  reasonp = reasons;
 string__output_4_0:
 string__output_4_1:
  switch (ptagof(a0)) {
 case CONS:
  x0 = car_of(a0);
 string__output_4_2:
  switch (ptagof(x0)) {
 case ATOMIC:
  if (!isint(x0)) goto string__output_4_interrupt;
 string__output_4_3:
  gblt_integer(x0,string__output_4_interrupt);
  allocp[0] = x2 = makeref(&allocp[0]);
  allocp[1] = x0;
  x1 = makecons(&allocp[0]);
  allocp += 2;
  unify_value(a3, x1);
  a0 = cdr_of(a0);
  a3 = x2;
  execute(string__output_4_0);
  goto string__output_4_ext_interrupt;
 case FUNCTOR:
  switch (symval(functor_of(x0))) {
 case functor_putc_1:
  allocp[0] = x2 = makeref(&allocp[0]);
  x3 = arg(x0, 0);
  allocp[1] = x3;
  x1 = makecons(&allocp[0]);
  allocp += 2;
  unify_value(a3, x1);
  a0 = cdr_of(a0);
  a3 = x2;
  execute(string__output_4_0);
  goto string__output_4_ext_interrupt;
 case functor_fwrite_1:
  x1 = arg(x0, 0);
 string__output_4_4:
  switch (ptagof(x1)) {
 case FUNCTOR:
 string__output_4_5:
  if (!isgobj(x1)) goto string__output_4_interrupt;
 string__output_4_6:
  gblt_is_string(x1,string__output_4_interrupt);
  gblt_size_of_string(x1,x2,string__output_4_interrupt);
  gblt_element_size_of_string(x1,x3,string__output_4_interrupt);
  allocp[0] = x5 = makeref(&allocp[0]);
  allocp[1] = x1;
  x4 = makecons(&allocp[0]);
  allocp += 2;
  unify_value(a3, x4);
  a0 = cdr_of(a0);
  a3 = x5;
  execute(string__output_4_0);
  goto string__output_4_ext_interrupt;
 case VARREF:
  deref_and_jump(x1,string__output_4_4);
  *reasonp++ =  x1;
  goto string__output_4_interrupt;
  };
  goto string__output_4_interrupt;
 case functor_fwrite_2:
  x1 = arg(x0, 0);
 string__output_4_7:
  switch (ptagof(x1)) {
 case FUNCTOR:
 string__output_4_8:
  if (!isgobj(x1)) goto string__output_4_interrupt;
 string__output_4_9:
  gblt_is_string(x1,string__output_4_interrupt);
  gblt_size_of_string(x1,x2,string__output_4_interrupt);
  gblt_element_size_of_string(x1,x3,string__output_4_interrupt);
  x4 = arg(x0, 1);
  unify_value(x4, x2);
  allocp[0] = x6 = makeref(&allocp[0]);
  allocp[1] = x1;
  x5 = makecons(&allocp[0]);
  allocp += 2;
  unify_value(a3, x5);
  a0 = cdr_of(a0);
  a3 = x6;
  execute(string__output_4_0);
  goto string__output_4_ext_interrupt;
 case VARREF:
  deref_and_jump(x1,string__output_4_7);
  *reasonp++ =  x1;
  goto string__output_4_interrupt;
  };
  goto string__output_4_interrupt;
  };
  goto string__output_4_interrupt;
 case VARREF:
  deref_and_jump(x0,string__output_4_2);
  *reasonp++ =  x0;
  goto string__output_4_interrupt;
  };
  goto string__output_4_interrupt;
 case ATOMIC:
  if (a0 != NILATOM) goto string__output_4_interrupt;
  x0 = NILATOM;
  unify_value(a3, x0);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klicio_xfill__string_4);
  allocp[2] = a2;
  allocp[3] = x1 = makeref(&allocp[3]);
  allocp[4] = a1;
  allocp[5] = makeint(0L);
  allocp[6] = x2 = makeref(&allocp[6]);
  generic_arg[0] = x2;
  x3 = makeint(8L);
  generic_arg[1] = x3;
  qp = (struct goalrec*)&allocp[0];
  new_generic(string_g_new, 2, x4, 7);
  unify(x1, x4);
  a0 = a2;
  a1 = makeint(0L);
  a2 = x2;
  execute(compute__string__length_3_0);
  goto compute__string__length_3_ext_interrupt;
 case VARREF:
  deref_and_jump(a0,string__output_4_1);
  *reasonp++ =  a0;
  goto string__output_4_interrupt;
  };
  goto string__output_4_interrupt;
 string__output_4_ext_interrupt:
  reasonp = 0l;
 string__output_4_interrupt:
  toppred = &predicate_klicio_xstring__output_4;
  goto interrupt_4;
 }

 compute__string__length_3_top: {
  q x0, x1, x2, x3;
  a1 = qp->args[1];
  a2 = qp->args[2];
  qp = qp->next;
 compute__string__length_3_clear_reason:
  reasonp = reasons;
 compute__string__length_3_0:
 compute__string__length_3_1:
  switch (ptagof(a0)) {
 case CONS:
  x0 = car_of(a0);
 compute__string__length_3_2:
  switch (ptagof(x0)) {
 case ATOMIC:
  if (!isint(x0)) goto compute__string__length_3_interrupt;
 compute__string__length_3_3:
  gblt_integer(x0,compute__string__length_3_interrupt);
  x1 = makeint(1L);
  bblt_add_no_check_y(a1,x1,x2);
  a0 = cdr_of(a0);
  a1 = x2;
  execute(compute__string__length_3_0);
  goto compute__string__length_3_ext_interrupt;
 case FUNCTOR:
 compute__string__length_3_4:
  if (!isgobj(x0)) goto compute__string__length_3_interrupt;
 compute__string__length_3_5:
  gblt_is_string(x0,compute__string__length_3_interrupt);
  gblt_size_of_string(x0,x1,compute__string__length_3_interrupt);
  gblt_element_size_of_string(x0,x2,compute__string__length_3_interrupt);
  bblt_add_no_check_y(a1,x1,x3);
  a0 = cdr_of(a0);
  a1 = x3;
  execute(compute__string__length_3_0);
  goto compute__string__length_3_ext_interrupt;
 case VARREF:
  deref_and_jump(x0,compute__string__length_3_2);
  *reasonp++ =  x0;
  goto compute__string__length_3_interrupt;
  };
  goto compute__string__length_3_interrupt;
 case ATOMIC:
  if (a0 != NILATOM) goto compute__string__length_3_interrupt;
  unify(a2, a1);
  proceed();
 case VARREF:
  deref_and_jump(a0,compute__string__length_3_1);
  *reasonp++ =  a0;
  goto compute__string__length_3_interrupt;
  };
  goto compute__string__length_3_interrupt;
 compute__string__length_3_ext_interrupt:
  reasonp = 0l;
 compute__string__length_3_interrupt:
  toppred = &predicate_klicio_xcompute__string__length_3;
  goto interrupt_3;
 }

 fill__string_4_top: {
  q x0, x1, x2, x3, x4, x5;
  a1 = qp->args[1];
  a2 = qp->args[2];
  a3 = qp->args[3];
  qp = qp->next;
 fill__string_4_clear_reason:
  reasonp = reasons;
 fill__string_4_0:
 fill__string_4_1:
  switch (ptagof(a0)) {
 case CONS:
  x0 = car_of(a0);
 fill__string_4_2:
  switch (ptagof(x0)) {
 case ATOMIC:
  if (!isint(x0)) goto fill__string_4_interrupt;
 fill__string_4_3:
  gblt_integer(x0,fill__string_4_interrupt);
  generic_arg[0] = a3;
  generic_arg[1] = x0;
  allocp[0] = x1 = makeref(&allocp[0]);
  generic_arg[2] = x1;
  call_generic(a1, functor_set__element_3, 1);
  x2 = makeint(1L);
  bblt_add_no_check_y(a3,x2,x3);
  a0 = cdr_of(a0);
  a1 = x1;
  a3 = x3;
  execute(fill__string_4_0);
  goto fill__string_4_ext_interrupt;
 case FUNCTOR:
 fill__string_4_4:
  if (!isgobj(x0)) goto fill__string_4_interrupt;
 fill__string_4_5:
  gblt_is_string(x0,fill__string_4_interrupt);
  gblt_size_of_string(x0,x1,fill__string_4_interrupt);
  gblt_element_size_of_string(x0,x2,fill__string_4_interrupt);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klicio_xfill__string_4);
  x3 = cdr_of(a0);
  allocp[2] = x3;
  allocp[3] = x4 = makeref(&allocp[3]);
  allocp[4] = a2;
  allocp[5] = x5 = makeref(&allocp[5]);
  allocp[6] = (q)(struct goalrec*)&allocp[0];
  allocp[7] = (q)(&predicate_integer__arithmetics_xadd_3);
  allocp[8] = a3;
  allocp[9] = x1;
  allocp[10] = x5;
  a0 = a1;
  a1 = a3;
  a2 = makeint(0L);
  a3 = x1;
  a4 = x0;
  a5 = x4;
  qp = (struct goalrec*)&allocp[6];
  allocp += 11;
  execute(set__string__elements_6_0);
  goto set__string__elements_6_ext_interrupt;
 case VARREF:
  deref_and_jump(x0,fill__string_4_2);
  *reasonp++ =  x0;
  goto fill__string_4_interrupt;
  };
  goto fill__string_4_interrupt;
 case ATOMIC:
  if (a0 != NILATOM) goto fill__string_4_interrupt;
  unify(a2, a1);
  proceed();
 case VARREF:
  deref_and_jump(a0,fill__string_4_1);
  *reasonp++ =  a0;
  goto fill__string_4_interrupt;
  };
  goto fill__string_4_interrupt;
 fill__string_4_ext_interrupt:
  reasonp = 0l;
 fill__string_4_interrupt:
  goto interrupt_4;
 }

 set__string__elements_6_top: {
  q x0, x1, x2, x3, x4, x5;
  a1 = qp->args[1];
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  qp = qp->next;
 set__string__elements_6_clear_reason:
  reasonp = reasons;
 set__string__elements_6_0:
 set__string__elements_6_1:
  if (!isint(a2)) goto set__string__elements_6_8;
 set__string__elements_6_2:
  if (!isint(a3)) goto set__string__elements_6_7;
  gblt_greater_or_eq(a2,a3,set__string__elements_6_3);
  unify(a5, a0);
  proceed();
 set__string__elements_6_3:
  gblt_less(a2,a3,set__string__elements_6_interrupt);
 set__string__elements_6_4:
  switch (ptagof(a4)) {
 case FUNCTOR:
 set__string__elements_6_5:
  if (!isgobj(a4)) goto set__string__elements_6_interrupt;
 set__string__elements_6_6:
  gblt_is_string(a4,set__string__elements_6_interrupt);
  gblt_element_of_string(a4,a2,x0,set__string__elements_6_interrupt);
  generic_arg[0] = a1;
  generic_arg[1] = x0;
  allocp[0] = x1 = makeref(&allocp[0]);
  generic_arg[2] = x1;
  call_generic(a0, functor_set__element_3, 1);
  x2 = makeint(1L);
  bblt_add_no_check_y(a1,x2,x3);
  x4 = makeint(1L);
  bblt_add_no_check(a2,x4,x5);
  a0 = x1;
  a1 = x3;
  a2 = x5;
  execute(set__string__elements_6_0);
  goto set__string__elements_6_ext_interrupt;
 case VARREF:
  deref_and_jump(a4,set__string__elements_6_4);
  *reasonp++ =  a4;
  goto set__string__elements_6_interrupt;
  };
  goto set__string__elements_6_interrupt;
 set__string__elements_6_7:
  if (!isref(a3)) goto set__string__elements_6_interrupt;
  deref_and_jump(a3,set__string__elements_6_2);
  *reasonp++ =  a3;
  goto set__string__elements_6_interrupt;
 set__string__elements_6_8:
  if (!isref(a2)) goto set__string__elements_6_interrupt;
  deref_and_jump(a2,set__string__elements_6_1);
  *reasonp++ =  a2;
  goto set__string__elements_6_interrupt;
 set__string__elements_6_ext_interrupt:
  reasonp = 0l;
 set__string__elements_6_interrupt:
  toppred = &predicate_klicio_xset__string__elements_6;
  goto interrupt_6;
 }

 string__input_5_top: {
  q x0, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10;
  a1 = qp->args[1];
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  qp = qp->next;
 string__input_5_clear_reason:
  reasonp = reasons;
 string__input_5_0:
 string__input_5_1:
  switch (ptagof(a0)) {
 case CONS:
  x0 = car_of(a0);
 string__input_5_2:
  switch (ptagof(x0)) {
 case FUNCTOR:
  switch (symval(functor_of(x0))) {
 case functor_linecount_1:
  x1 = arg(x0, 0);
  unify(x1, a4);
  a0 = cdr_of(a0);
  execute(string__input_5_0);
  goto string__input_5_ext_interrupt;
 case functor_getc_1:
 string__input_5_3:
  if (!isint(a2)) goto string__input_5_10;
 string__input_5_4:
  if (!isint(a3)) goto string__input_5_9;
  gblt_greater_or_eq(a2,a3,string__input_5_5);
  x1 = arg(x0, 0);
  x2 = makeint(-1L);
  unify_value(x1, x2);
  a0 = cdr_of(a0);
  execute(string__input_5_0);
  goto string__input_5_ext_interrupt;
 string__input_5_5:
  gblt_less(a2,a3,string__input_5_interrupt);
 string__input_5_6:
  switch (ptagof(a1)) {
 case FUNCTOR:
 string__input_5_7:
  if (!isgobj(a1)) goto string__input_5_interrupt;
 string__input_5_8:
  gblt_is_string(a1,string__input_5_interrupt);
  gblt_element_of_string(a1,a2,x1,string__input_5_interrupt);
  x3 = makeint(1L);
  gblt_add(a2,x3,x2,string__input_5_interrupt);
  x4 = arg(x0, 0);
  unify_value(x4, x1);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_klicio_xstring__input_5);
  x5 = cdr_of(a0);
  allocp[2] = x5;
  allocp[3] = a1;
  allocp[4] = x2;
  allocp[5] = a3;
  allocp[6] = a4;
  a0 = x4;
  a1 = a4;
  allocp[7] = x6 = makeref(&allocp[7]);
  a2 = x6;
  qp = (struct goalrec*)&allocp[0];
  allocp += 8;
  execute(update__lc_3_0);
  goto update__lc_3_ext_interrupt;
 case VARREF:
  deref_and_jump(a1,string__input_5_6);
  *reasonp++ =  a1;
  goto string__input_5_interrupt;
  };
  goto string__input_5_interrupt;
 string__input_5_9:
  if (!isref(a3)) goto string__input_5_interrupt;
  deref_and_jump(a3,string__input_5_4);
  *reasonp++ =  a3;
  goto string__input_5_interrupt;
 string__input_5_10:
  if (!isref(a2)) goto string__input_5_interrupt;
  deref_and_jump(a2,string__input_5_3);
  *reasonp++ =  a2;
  goto string__input_5_interrupt;
 case functor_ungetc_1:
  x1 = arg(x0, 0);
 string__input_5_11:
  if (!isint(x1)) goto string__input_5_16;
  x2 = makeint(-1L);
  gblt_eq(x1,x2,string__input_5_12);
  a0 = cdr_of(a0);
  execute(string__input_5_0);
  goto string__input_5_ext_interrupt;
 string__input_5_12:
  x3 = makeint(-1L);
  gblt_not_eq(x1,x3,string__input_5_interrupt);
 string__input_5_13:
  if (!isint(a2)) goto string__input_5_15;
  x4 = makeint(0L);
  gblt_greater(a2,x4,string__input_5_interrupt);
  x6 = makeint(1L);
  gblt_subtract(a2,x6,x5,string__input_5_interrupt);
  x7 = makeint(10L);
  gblt_not_eq(x1,x7,string__input_5_14);
  a0 = cdr_of(a0);
  a2 = x5;
  execute(string__input_5_0);
  goto string__input_5_ext_interrupt;
 string__input_5_14:
  x8 = makeint(10L);
  gblt_eq(x1,x8,string__input_5_interrupt);
  x9 = makeint(1L);
  bblt_sub_no_check_y(a4,x9,x10);
  a0 = cdr_of(a0);
  a2 = x5;
  a4 = x10;
  execute(string__input_5_0);
  goto string__input_5_ext_interrupt;
 string__input_5_15:
  if (!isref(a2)) goto string__input_5_interrupt;
  deref_and_jump(a2,string__input_5_13);
  *reasonp++ =  a2;
  goto string__input_5_interrupt;
 string__input_5_16:
  if (!isref(x1)) goto string__input_5_interrupt;
  deref_and_jump(x1,string__input_5_11);
  *reasonp++ =  x1;
  goto string__input_5_interrupt;
  };
  goto string__input_5_interrupt;
 case VARREF:
  deref_and_jump(x0,string__input_5_2);
  *reasonp++ =  x0;
  goto string__input_5_interrupt;
  };
  goto string__input_5_interrupt;
 case ATOMIC:
  if (a0 != NILATOM) goto string__input_5_interrupt;
  proceed();
 case VARREF:
  deref_and_jump(a0,string__input_5_1);
  *reasonp++ =  a0;
  goto string__input_5_interrupt;
  };
  goto string__input_5_interrupt;
 string__input_5_ext_interrupt:
  reasonp = 0l;
 string__input_5_interrupt:
  toppred = &predicate_klicio_xstring__input_5;
  goto interrupt_5;
 }

 update__lc_3_top: {
  q x0, x1, x2;
  a1 = qp->args[1];
  a2 = qp->args[2];
  qp = qp->next;
 update__lc_3_clear_reason:
  reasonp = reasons;
 update__lc_3_0:
 update__lc_3_1:
  if (!isint(a0)) goto update__lc_3_3;
  x0 = makeint(10L);
  gblt_eq(a0,x0,update__lc_3_2);
  x1 = makeint(1L);
  bblt_add_no_check_y(a1,x1,x2);
  unify(a2, x2);
  proceed();
 update__lc_3_2:
  x1 = makeint(10L);
  gblt_not_eq(a0,x1,update__lc_3_interrupt);
  unify(a2, a1);
  proceed();
 update__lc_3_3:
  if (!isref(a0)) goto update__lc_3_interrupt;
  deref_and_jump(a0,update__lc_3_1);
  *reasonp++ =  a0;
  goto update__lc_3_interrupt;
 update__lc_3_ext_interrupt:
  reasonp = 0l;
 update__lc_3_interrupt:
  toppred = &predicate_klicio_xupdate__lc_3;
  goto interrupt_3;
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
  loop_within_module(module_klicio);
}
