#include <klic/basic.h>
#include <klic/predinfo.h>

extern Const struct predicate predicate_klic__comp__bb_xis__body__builtin_2;
static Const struct predinfo predinfo_klic__comp__bb_xis__body__builtin_2 = {
  (char *)"is_body_builtin",
  &predicate_klic__comp__bb_xis__body__builtin_2
};

extern Const struct predicate predicate_klic__comp__bb_xint__unary_2;
static Const struct predinfo predinfo_klic__comp__bb_xint__unary_2 = {
  (char *)"int_unary",
  &predicate_klic__comp__bb_xint__unary_2
};

extern Const struct predicate predicate_klic__comp__bb_xint__binary_2;
static Const struct predinfo predinfo_klic__comp__bb_xint__binary_2 = {
  (char *)"int_binary",
  &predicate_klic__comp__bb_xint__binary_2
};

extern Const struct predicate predicate_klic__comp__bb_xfloat__unary_2;
static Const struct predinfo predinfo_klic__comp__bb_xfloat__unary_2 = {
  (char *)"float_unary",
  &predicate_klic__comp__bb_xfloat__unary_2
};

extern Const struct predicate predicate_klic__comp__bb_xfloat__binary_2;
static Const struct predinfo predinfo_klic__comp__bb_xfloat__binary_2 = {
  (char *)"float_binary",
  &predicate_klic__comp__bb_xfloat__binary_2
};

static Const struct predinfo * Const preds_in_klic__comp__bb[] = {
  &predinfo_klic__comp__bb_xis__body__builtin_2,
  &predinfo_klic__comp__bb_xint__unary_2,
  &predinfo_klic__comp__bb_xint__binary_2,
  &predinfo_klic__comp__bb_xfloat__unary_2,
  &predinfo_klic__comp__bb_xfloat__binary_2,
  0
};
extern Const struct predicate predicate_klic__comp__gb_xis__guard__builtin_2;
static Const struct predinfo predinfo_klic__comp__gb_xis__guard__builtin_2 = {
  (char *)"is_guard_builtin",
  &predicate_klic__comp__gb_xis__guard__builtin_2
};

extern Const struct predicate predicate_klic__comp__gb_xint__unary_4;
static Const struct predinfo predinfo_klic__comp__gb_xint__unary_4 = {
  (char *)"int_unary",
  &predicate_klic__comp__gb_xint__unary_4
};

extern Const struct predicate predicate_klic__comp__gb_xint__compare_4;
static Const struct predinfo predinfo_klic__comp__gb_xint__compare_4 = {
  (char *)"int_compare",
  &predicate_klic__comp__gb_xint__compare_4
};

extern Const struct predicate predicate_klic__comp__gb_xint__binary_5;
static Const struct predinfo predinfo_klic__comp__gb_xint__binary_5 = {
  (char *)"int_binary",
  &predicate_klic__comp__gb_xint__binary_5
};

extern Const struct predicate predicate_klic__comp__gb_xbuiltin_4;
static Const struct predinfo predinfo_klic__comp__gb_xbuiltin_4 = {
  (char *)"builtin",
  &predicate_klic__comp__gb_xbuiltin_4
};

static Const struct predinfo * Const preds_in_klic__comp__gb[] = {
  &predinfo_klic__comp__gb_xis__guard__builtin_2,
  &predinfo_klic__comp__gb_xint__unary_4,
  &predinfo_klic__comp__gb_xint__compare_4,
  &predinfo_klic__comp__gb_xint__binary_5,
  &predinfo_klic__comp__gb_xbuiltin_4,
  0
};
extern Const struct predicate predicate_klic__comp__generate_xused__in__1_5;
static Const struct predinfo predinfo_klic__comp__generate_xused__in__1_5 = {
  (char *)"used_in_1",
  &predicate_klic__comp__generate_xused__in__1_5
};

extern Const struct predicate predicate_klic__comp__generate_xused__in_4;
static Const struct predinfo predinfo_klic__comp__generate_xused__in_4 = {
  (char *)"used_in",
  &predicate_klic__comp__generate_xused__in_4
};

extern Const struct predicate predicate_klic__comp__generate_xstrip__types_2;
static Const struct predinfo predinfo_klic__comp__generate_xstrip__types_2 = {
  (char *)"strip_types",
  &predicate_klic__comp__generate_xstrip__types_2
};

extern Const struct predicate predicate_klic__comp__generate_xstrip__type_2;
static Const struct predinfo predinfo_klic__comp__generate_xstrip__type_2 = {
  (char *)"strip_type",
  &predicate_klic__comp__generate_xstrip__type_2
};

extern Const struct predicate predicate_klic__comp__generate_xset__one__1_10;
static Const struct predinfo predinfo_klic__comp__generate_xset__one__1_10 = {
  (char *)"set_one_1",
  &predicate_klic__comp__generate_xset__one__1_10
};

extern Const struct predicate predicate_klic__comp__generate_xset__one_10;
static Const struct predinfo predinfo_klic__comp__generate_xset__one_10 = {
  (char *)"set_one",
  &predicate_klic__comp__generate_xset__one_10
};

extern Const struct predicate predicate_klic__comp__generate_xset__generic__args_10;
static Const struct predinfo predinfo_klic__comp__generate_xset__generic__args_10 = {
  (char *)"set_generic_args",
  &predicate_klic__comp__generate_xset__generic__args_10
};

extern Const struct predicate predicate_klic__comp__generate_xset__call__merge_3;
static Const struct predinfo predinfo_klic__comp__generate_xset__call__merge_3 = {
  (char *)"set_call_merge",
  &predicate_klic__comp__generate_xset__call__merge_3
};

extern Const struct predicate predicate_klic__comp__generate_xset__call_10;
static Const struct predinfo predinfo_klic__comp__generate_xset__call_10 = {
  (char *)"set_call",
  &predicate_klic__comp__generate_xset__call_10
};

extern Const struct predicate predicate_klic__comp__generate_xset__args_10;
static Const struct predinfo predinfo_klic__comp__generate_xset__args_10 = {
  (char *)"set_args",
  &predicate_klic__comp__generate_xset__args_10
};

extern Const struct predicate predicate_klic__comp__generate_xrassoc_3;
static Const struct predinfo predinfo_klic__comp__generate_xrassoc_3 = {
  (char *)"rassoc",
  &predicate_klic__comp__generate_xrassoc_3
};

extern Const struct predicate predicate_klic__comp__generate_xprio__code_9;
static Const struct predinfo predinfo_klic__comp__generate_xprio__code_9 = {
  (char *)"prio_code",
  &predicate_klic__comp__generate_xprio__code_9
};

extern Const struct predicate predicate_klic__comp__generate_xprep__vec__elems_10;
static Const struct predinfo predinfo_klic__comp__generate_xprep__vec__elems_10 = {
  (char *)"prep_vec_elems",
  &predicate_klic__comp__generate_xprep__vec__elems_10
};

extern Const struct predicate predicate_klic__comp__generate_xprep__poss__gb_10;
static Const struct predinfo predinfo_klic__comp__generate_xprep__poss__gb_10 = {
  (char *)"prep_poss_gb",
  &predicate_klic__comp__generate_xprep__poss__gb_10
};

extern Const struct predicate predicate_klic__comp__generate_xprep__poss_10;
static Const struct predinfo predinfo_klic__comp__generate_xprep__poss_10 = {
  (char *)"prep_poss",
  &predicate_klic__comp__generate_xprep__poss_10
};

extern Const struct predicate predicate_klic__comp__generate_xprep__pos__gb__1_10;
static Const struct predinfo predinfo_klic__comp__generate_xprep__pos__gb__1_10 = {
  (char *)"prep_pos_gb_1",
  &predicate_klic__comp__generate_xprep__pos__gb__1_10
};

extern Const struct predicate predicate_klic__comp__generate_xprep__pos__gb_10;
static Const struct predinfo predinfo_klic__comp__generate_xprep__pos__gb_10 = {
  (char *)"prep_pos_gb",
  &predicate_klic__comp__generate_xprep__pos__gb_10
};

extern Const struct predicate predicate_klic__comp__generate_xprep__pos__1_10;
static Const struct predinfo predinfo_klic__comp__generate_xprep__pos__1_10 = {
  (char *)"prep_pos_1",
  &predicate_klic__comp__generate_xprep__pos__1_10
};

extern Const struct predicate predicate_klic__comp__generate_xprep__pos_10;
static Const struct predinfo predinfo_klic__comp__generate_xprep__pos_10 = {
  (char *)"prep_pos",
  &predicate_klic__comp__generate_xprep__pos_10
};

extern Const struct predicate predicate_klic__comp__generate_xprep__call__arg_10;
static Const struct predinfo predinfo_klic__comp__generate_xprep__call__arg_10 = {
  (char *)"prep_call_arg",
  &predicate_klic__comp__generate_xprep__call__arg_10
};

extern Const struct predicate predicate_klic__comp__generate_xmove__one__1_10;
static Const struct predinfo predinfo_klic__comp__generate_xmove__one__1_10 = {
  (char *)"move_one_1",
  &predicate_klic__comp__generate_xmove__one__1_10
};

extern Const struct predicate predicate_klic__comp__generate_xmove__one_10;
static Const struct predinfo predinfo_klic__comp__generate_xmove__one_10 = {
  (char *)"move_one",
  &predicate_klic__comp__generate_xmove__one_10
};

extern Const struct predicate predicate_klic__comp__generate_xmove__args__1_12;
static Const struct predinfo predinfo_klic__comp__generate_xmove__args__1_12 = {
  (char *)"move_args_1",
  &predicate_klic__comp__generate_xmove__args__1_12
};

extern Const struct predicate predicate_klic__comp__generate_xmove__args_10;
static Const struct predinfo predinfo_klic__comp__generate_xmove__args_10 = {
  (char *)"move_args",
  &predicate_klic__comp__generate_xmove__args_10
};

extern Const struct predicate predicate_klic__comp__generate_xmake__label_2;
static Const struct predinfo predinfo_klic__comp__generate_xmake__label_2 = {
  (char *)"make_label",
  &predicate_klic__comp__generate_xmake__label_2
};

extern Const struct predicate predicate_klic__comp__generate_xload__reg__2_11;
static Const struct predinfo predinfo_klic__comp__generate_xload__reg__2_11 = {
  (char *)"load_reg_2",
  &predicate_klic__comp__generate_xload__reg__2_11
};

extern Const struct predicate predicate_klic__comp__generate_xload__reg__1_11;
static Const struct predinfo predinfo_klic__comp__generate_xload__reg__1_11 = {
  (char *)"load_reg_1",
  &predicate_klic__comp__generate_xload__reg__1_11
};

extern Const struct predicate predicate_klic__comp__generate_xload__reg_11;
static Const struct predinfo predinfo_klic__comp__generate_xload__reg_11 = {
  (char *)"load_reg",
  &predicate_klic__comp__generate_xload__reg_11
};

extern Const struct predicate predicate_klic__comp__generate_xload__pos_10;
static Const struct predinfo predinfo_klic__comp__generate_xload__pos_10 = {
  (char *)"load_pos",
  &predicate_klic__comp__generate_xload__pos_10
};

extern Const struct predicate predicate_klic__comp__generate_xload__inline__args_11;
static Const struct predinfo predinfo_klic__comp__generate_xload__inline__args_11 = {
  (char *)"load_inline_args",
  &predicate_klic__comp__generate_xload__inline__args_11
};

extern Const struct predicate predicate_klic__comp__generate_xload__guard__generic__out_11;
static Const struct predinfo predinfo_klic__comp__generate_xload__guard__generic__out_11 = {
  (char *)"load_guard_generic_out",
  &predicate_klic__comp__generate_xload__guard__generic__out_11
};

extern Const struct predicate predicate_klic__comp__generate_xgen__type__known_3;
static Const struct predinfo predinfo_klic__comp__generate_xgen__type__known_3 = {
  (char *)"gen_type_known",
  &predicate_klic__comp__generate_xgen__type__known_3
};

extern Const struct predicate predicate_klic__comp__generate_xgen__type__code_18;
static Const struct predinfo predinfo_klic__comp__generate_xgen__type__code_18 = {
  (char *)"gen_type_code",
  &predicate_klic__comp__generate_xgen__type__code_18
};

extern Const struct predicate predicate_klic__comp__generate_xgen__sw__instr_3;
static Const struct predinfo predinfo_klic__comp__generate_xgen__sw__instr_3 = {
  (char *)"gen_sw_instr",
  &predicate_klic__comp__generate_xgen__sw__instr_3
};

extern Const struct predicate predicate_klic__comp__generate_xgen__new__lab_5;
static Const struct predinfo predinfo_klic__comp__generate_xgen__new__lab_5 = {
  (char *)"gen_new_lab",
  &predicate_klic__comp__generate_xgen__new__lab_5
};

extern Const struct predicate predicate_klic__comp__generate_xgen__lab_5;
static Const struct predinfo predinfo_klic__comp__generate_xgen__lab_5 = {
  (char *)"gen_lab",
  &predicate_klic__comp__generate_xgen__lab_5
};

extern Const struct predicate predicate_klic__comp__generate_xgen__if__not_7;
static Const struct predinfo predinfo_klic__comp__generate_xgen__if__not_7 = {
  (char *)"gen_if_not",
  &predicate_klic__comp__generate_xgen__if__not_7
};

extern Const struct predicate predicate_klic__comp__generate_xgen__gobjs_15;
static Const struct predinfo predinfo_klic__comp__generate_xgen__gobjs_15 = {
  (char *)"gen_gobjs",
  &predicate_klic__comp__generate_xgen__gobjs_15
};

extern Const struct predicate predicate_klic__comp__generate_xgen__functs_15;
static Const struct predinfo predinfo_klic__comp__generate_xgen__functs_15 = {
  (char *)"gen_functs",
  &predicate_klic__comp__generate_xgen__functs_15
};

extern Const struct predicate predicate_klic__comp__generate_xgen__cond_12;
static Const struct predinfo predinfo_klic__comp__generate_xgen__cond_12 = {
  (char *)"gen_cond",
  &predicate_klic__comp__generate_xgen__cond_12
};

extern Const struct predicate predicate_klic__comp__generate_xgen__code__3__3_14;
static Const struct predinfo predinfo_klic__comp__generate_xgen__code__3__3_14 = {
  (char *)"gen_code_3_3",
  &predicate_klic__comp__generate_xgen__code__3__3_14
};

extern Const struct predicate predicate_klic__comp__generate_xgen__code__3__2_14;
static Const struct predinfo predinfo_klic__comp__generate_xgen__code__3__2_14 = {
  (char *)"gen_code_3_2",
  &predicate_klic__comp__generate_xgen__code__3__2_14
};

extern Const struct predicate predicate_klic__comp__generate_xgen__code__3__1_14;
static Const struct predinfo predinfo_klic__comp__generate_xgen__code__3__1_14 = {
  (char *)"gen_code_3_1",
  &predicate_klic__comp__generate_xgen__code__3__1_14
};

extern Const struct predicate predicate_klic__comp__generate_xgen__code__3_14;
static Const struct predinfo predinfo_klic__comp__generate_xgen__code__3_14 = {
  (char *)"gen_code_3",
  &predicate_klic__comp__generate_xgen__code__3_14
};

extern Const struct predicate predicate_klic__comp__generate_xgen__code__1_15;
static Const struct predinfo predinfo_klic__comp__generate_xgen__code__1_15 = {
  (char *)"gen_code_1",
  &predicate_klic__comp__generate_xgen__code__1_15
};

extern Const struct predicate predicate_klic__comp__generate_xgen__code_14;
static Const struct predinfo predinfo_klic__comp__generate_xgen__code_14 = {
  (char *)"gen_code",
  &predicate_klic__comp__generate_xgen__code_14
};

extern Const struct predicate predicate_klic__comp__generate_xgen__classes_15;
static Const struct predinfo predinfo_klic__comp__generate_xgen__classes_15 = {
  (char *)"gen_classes",
  &predicate_klic__comp__generate_xgen__classes_15
};

extern Const struct predicate predicate_klic__comp__generate_xgen__cases_16;
static Const struct predinfo predinfo_klic__comp__generate_xgen__cases_16 = {
  (char *)"gen_cases",
  &predicate_klic__comp__generate_xgen__cases_16
};

extern Const struct predicate predicate_klic__comp__generate_xgen__case__lab_5;
static Const struct predinfo predinfo_klic__comp__generate_xgen__case__lab_5 = {
  (char *)"gen_case_lab",
  &predicate_klic__comp__generate_xgen__case__lab_5
};

extern Const struct predicate predicate_klic__comp__generate_xgen__body__goal_13;
static Const struct predinfo predinfo_klic__comp__generate_xgen__body__goal_13 = {
  (char *)"gen_body_goal",
  &predicate_klic__comp__generate_xgen__body__goal_13
};

extern Const struct predicate predicate_klic__comp__generate_xgen__body_13;
static Const struct predinfo predinfo_klic__comp__generate_xgen__body_13 = {
  (char *)"gen_body",
  &predicate_klic__comp__generate_xgen__body_13
};

extern Const struct predicate predicate_klic__comp__generate_xgen__atomic__2_20;
static Const struct predinfo predinfo_klic__comp__generate_xgen__atomic__2_20 = {
  (char *)"gen_atomic_2",
  &predicate_klic__comp__generate_xgen__atomic__2_20
};

extern Const struct predicate predicate_klic__comp__generate_xgen__atomic_2F15_240_2F18_240_13;
static Const struct predinfo predinfo_klic__comp__generate_xgen__atomic_2F15_240_2F18_240_13 = {
  (char *)"gen_atomic/15$0/18$0",
  &predicate_klic__comp__generate_xgen__atomic_2F15_240_2F18_240_13
};

extern Const struct predicate predicate_klic__comp__generate_xgen__atomic_2F15_240_18;
static Const struct predinfo predinfo_klic__comp__generate_xgen__atomic_2F15_240_18 = {
  (char *)"gen_atomic/15$0",
  &predicate_klic__comp__generate_xgen__atomic_2F15_240_18
};

extern Const struct predicate predicate_klic__comp__generate_xgen__atomic_15;
static Const struct predinfo predinfo_klic__comp__generate_xgen__atomic_15 = {
  (char *)"gen_atomic",
  &predicate_klic__comp__generate_xgen__atomic_15
};

extern Const struct predicate predicate_klic__comp__generate_xgen__all__cases_14;
static Const struct predinfo predinfo_klic__comp__generate_xgen__all__cases_14 = {
  (char *)"gen_all_cases",
  &predicate_klic__comp__generate_xgen__all__cases_14
};

extern Const struct predicate predicate_klic__comp__generate_xfunctor__type_2;
static Const struct predinfo predinfo_klic__comp__generate_xfunctor__type_2 = {
  (char *)"functor_type",
  &predicate_klic__comp__generate_xfunctor__type_2
};

extern Const struct predicate predicate_klic__comp__generate_xenq__at__prio_12;
static Const struct predinfo predinfo_klic__comp__generate_xenq__at__prio_12 = {
  (char *)"enq_at_prio",
  &predicate_klic__comp__generate_xenq__at__prio_12
};

extern Const struct predicate predicate_klic__comp__generate_xcase__info_7;
static Const struct predinfo predinfo_klic__comp__generate_xcase__info_7 = {
  (char *)"case_info",
  &predicate_klic__comp__generate_xcase__info_7
};

extern Const struct predicate predicate_klic__comp__generate_xbody__unify__2_7;
static Const struct predinfo predinfo_klic__comp__generate_xbody__unify__2_7 = {
  (char *)"body_unify_2",
  &predicate_klic__comp__generate_xbody__unify__2_7
};

extern Const struct predicate predicate_klic__comp__generate_xbody__unify__1_11;
static Const struct predinfo predinfo_klic__comp__generate_xbody__unify__1_11 = {
  (char *)"body_unify_1",
  &predicate_klic__comp__generate_xbody__unify__1_11
};

extern Const struct predicate predicate_klic__comp__generate_xbody__unify_12;
static Const struct predinfo predinfo_klic__comp__generate_xbody__unify_12 = {
  (char *)"body_unify",
  &predicate_klic__comp__generate_xbody__unify_12
};

extern Const struct predicate predicate_klic__comp__generate_xbb__out__type_3;
static Const struct predinfo predinfo_klic__comp__generate_xbb__out__type_3 = {
  (char *)"bb_out_type",
  &predicate_klic__comp__generate_xbb__out__type_3
};

extern Const struct predicate predicate_klic__comp__generate_xbb__out_14;
static Const struct predinfo predinfo_klic__comp__generate_xbb__out_14 = {
  (char *)"bb_out",
  &predicate_klic__comp__generate_xbb__out_14
};

extern Const struct predicate predicate_klic__comp__generate_xbb__in__type_3;
static Const struct predinfo predinfo_klic__comp__generate_xbb__in__type_3 = {
  (char *)"bb_in_type",
  &predicate_klic__comp__generate_xbb__in__type_3
};

extern Const struct predicate predicate_klic__comp__generate_xbb__in__check_4;
static Const struct predinfo predinfo_klic__comp__generate_xbb__in__check_4 = {
  (char *)"bb_in_check",
  &predicate_klic__comp__generate_xbb__in__check_4
};

extern Const struct predicate predicate_klic__comp__generate_xbb__in_14;
static Const struct predinfo predinfo_klic__comp__generate_xbb__in_14 = {
  (char *)"bb_in",
  &predicate_klic__comp__generate_xbb__in_14
};

extern Const struct predicate predicate_klic__comp__generate_xbasic__type_2;
static Const struct predinfo predinfo_klic__comp__generate_xbasic__type_2 = {
  (char *)"basic_type",
  &predicate_klic__comp__generate_xbasic__type_2
};

extern Const struct predicate predicate_klic__comp__generate_xatomic__type_2;
static Const struct predinfo predinfo_klic__comp__generate_xatomic__type_2 = {
  (char *)"atomic_type",
  &predicate_klic__comp__generate_xatomic__type_2
};

extern Const struct predicate predicate_klic__comp__generate_xassocx_3;
static Const struct predinfo predinfo_klic__comp__generate_xassocx_3 = {
  (char *)"assocx",
  &predicate_klic__comp__generate_xassocx_3
};

extern Const struct predicate predicate_klic__comp__generate_xalloc__one__1_10;
static Const struct predinfo predinfo_klic__comp__generate_xalloc__one__1_10 = {
  (char *)"alloc_one_1",
  &predicate_klic__comp__generate_xalloc__one__1_10
};

extern Const struct predicate predicate_klic__comp__generate_xalloc__one_10;
static Const struct predinfo predinfo_klic__comp__generate_xalloc__one_10 = {
  (char *)"alloc_one",
  &predicate_klic__comp__generate_xalloc__one_10
};

extern Const struct predicate predicate_klic__comp__generate_xalloc__if__var_8;
static Const struct predinfo predinfo_klic__comp__generate_xalloc__if__var_8 = {
  (char *)"alloc_if_var",
  &predicate_klic__comp__generate_xalloc__if__var_8
};

extern Const struct predicate predicate_klic__comp__generate_xalloc__args_10;
static Const struct predinfo predinfo_klic__comp__generate_xalloc__args_10 = {
  (char *)"alloc_args",
  &predicate_klic__comp__generate_xalloc__args_10
};

extern Const struct predicate predicate_klic__comp__generate_xadd__inline__info_3;
static Const struct predinfo predinfo_klic__comp__generate_xadd__inline__info_3 = {
  (char *)"add_inline_info",
  &predicate_klic__comp__generate_xadd__inline__info_3
};

extern Const struct predicate predicate_klic__comp__generate_xadd__info_4;
static Const struct predinfo predinfo_klic__comp__generate_xadd__info_4 = {
  (char *)"add_info",
  &predicate_klic__comp__generate_xadd__info_4
};

static Const struct predinfo * Const preds_in_klic__comp__generate[] = {
  &predinfo_klic__comp__generate_xused__in__1_5,
  &predinfo_klic__comp__generate_xused__in_4,
  &predinfo_klic__comp__generate_xstrip__types_2,
  &predinfo_klic__comp__generate_xstrip__type_2,
  &predinfo_klic__comp__generate_xset__one__1_10,
  &predinfo_klic__comp__generate_xset__one_10,
  &predinfo_klic__comp__generate_xset__generic__args_10,
  &predinfo_klic__comp__generate_xset__call__merge_3,
  &predinfo_klic__comp__generate_xset__call_10,
  &predinfo_klic__comp__generate_xset__args_10,
  &predinfo_klic__comp__generate_xrassoc_3,
  &predinfo_klic__comp__generate_xprio__code_9,
  &predinfo_klic__comp__generate_xprep__vec__elems_10,
  &predinfo_klic__comp__generate_xprep__poss__gb_10,
  &predinfo_klic__comp__generate_xprep__poss_10,
  &predinfo_klic__comp__generate_xprep__pos__gb__1_10,
  &predinfo_klic__comp__generate_xprep__pos__gb_10,
  &predinfo_klic__comp__generate_xprep__pos__1_10,
  &predinfo_klic__comp__generate_xprep__pos_10,
  &predinfo_klic__comp__generate_xprep__call__arg_10,
  &predinfo_klic__comp__generate_xmove__one__1_10,
  &predinfo_klic__comp__generate_xmove__one_10,
  &predinfo_klic__comp__generate_xmove__args__1_12,
  &predinfo_klic__comp__generate_xmove__args_10,
  &predinfo_klic__comp__generate_xmake__label_2,
  &predinfo_klic__comp__generate_xload__reg__2_11,
  &predinfo_klic__comp__generate_xload__reg__1_11,
  &predinfo_klic__comp__generate_xload__reg_11,
  &predinfo_klic__comp__generate_xload__pos_10,
  &predinfo_klic__comp__generate_xload__inline__args_11,
  &predinfo_klic__comp__generate_xload__guard__generic__out_11,
  &predinfo_klic__comp__generate_xgen__type__known_3,
  &predinfo_klic__comp__generate_xgen__type__code_18,
  &predinfo_klic__comp__generate_xgen__sw__instr_3,
  &predinfo_klic__comp__generate_xgen__new__lab_5,
  &predinfo_klic__comp__generate_xgen__lab_5,
  &predinfo_klic__comp__generate_xgen__if__not_7,
  &predinfo_klic__comp__generate_xgen__gobjs_15,
  &predinfo_klic__comp__generate_xgen__functs_15,
  &predinfo_klic__comp__generate_xgen__cond_12,
  &predinfo_klic__comp__generate_xgen__code__3__3_14,
  &predinfo_klic__comp__generate_xgen__code__3__2_14,
  &predinfo_klic__comp__generate_xgen__code__3__1_14,
  &predinfo_klic__comp__generate_xgen__code__3_14,
  &predinfo_klic__comp__generate_xgen__code__1_15,
  &predinfo_klic__comp__generate_xgen__code_14,
  &predinfo_klic__comp__generate_xgen__classes_15,
  &predinfo_klic__comp__generate_xgen__cases_16,
  &predinfo_klic__comp__generate_xgen__case__lab_5,
  &predinfo_klic__comp__generate_xgen__body__goal_13,
  &predinfo_klic__comp__generate_xgen__body_13,
  &predinfo_klic__comp__generate_xgen__atomic__2_20,
  &predinfo_klic__comp__generate_xgen__atomic_2F15_240_2F18_240_13,
  &predinfo_klic__comp__generate_xgen__atomic_2F15_240_18,
  &predinfo_klic__comp__generate_xgen__atomic_15,
  &predinfo_klic__comp__generate_xgen__all__cases_14,
  &predinfo_klic__comp__generate_xfunctor__type_2,
  &predinfo_klic__comp__generate_xenq__at__prio_12,
  &predinfo_klic__comp__generate_xcase__info_7,
  &predinfo_klic__comp__generate_xbody__unify__2_7,
  &predinfo_klic__comp__generate_xbody__unify__1_11,
  &predinfo_klic__comp__generate_xbody__unify_12,
  &predinfo_klic__comp__generate_xbb__out__type_3,
  &predinfo_klic__comp__generate_xbb__out_14,
  &predinfo_klic__comp__generate_xbb__in__type_3,
  &predinfo_klic__comp__generate_xbb__in__check_4,
  &predinfo_klic__comp__generate_xbb__in_14,
  &predinfo_klic__comp__generate_xbasic__type_2,
  &predinfo_klic__comp__generate_xatomic__type_2,
  &predinfo_klic__comp__generate_xassocx_3,
  &predinfo_klic__comp__generate_xalloc__one__1_10,
  &predinfo_klic__comp__generate_xalloc__one_10,
  &predinfo_klic__comp__generate_xalloc__if__var_8,
  &predinfo_klic__comp__generate_xalloc__args_10,
  &predinfo_klic__comp__generate_xadd__inline__info_3,
  &predinfo_klic__comp__generate_xadd__info_4,
  0
};
extern Const struct predicate predicate_klic__comp__insert_xsubsumed__type_3;
static Const struct predinfo predinfo_klic__comp__insert_xsubsumed__type_3 = {
  (char *)"subsumed_type",
  &predicate_klic__comp__insert_xsubsumed__type_3
};

extern Const struct predicate predicate_klic__comp__insert_xsubsumed__2_3;
static Const struct predinfo predinfo_klic__comp__insert_xsubsumed__2_3 = {
  (char *)"subsumed_2",
  &predicate_klic__comp__insert_xsubsumed__2_3
};

extern Const struct predicate predicate_klic__comp__insert_xsubsumed__1_5;
static Const struct predinfo predinfo_klic__comp__insert_xsubsumed__1_5 = {
  (char *)"subsumed_1",
  &predicate_klic__comp__insert_xsubsumed__1_5
};

extern Const struct predicate predicate_klic__comp__insert_xsubsumed_4;
static Const struct predinfo predinfo_klic__comp__insert_xsubsumed_4 = {
  (char *)"subsumed",
  &predicate_klic__comp__insert_xsubsumed_4
};

extern Const struct predicate predicate_klic__comp__insert_xpattern__kind_2;
static Const struct predinfo predinfo_klic__comp__insert_xpattern__kind_2 = {
  (char *)"pattern_kind",
  &predicate_klic__comp__insert_xpattern__kind_2
};

extern Const struct predicate predicate_klic__comp__insert_xmake__index__tree__1_7;
static Const struct predinfo predinfo_klic__comp__insert_xmake__index__tree__1_7 = {
  (char *)"make_index_tree_1",
  &predicate_klic__comp__insert_xmake__index__tree__1_7
};

extern Const struct predicate predicate_klic__comp__insert_xmake__index__tree_5;
static Const struct predinfo predinfo_klic__comp__insert_xmake__index__tree_5 = {
  (char *)"make_index_tree",
  &predicate_klic__comp__insert_xmake__index__tree_5
};

extern Const struct predicate predicate_klic__comp__insert_xins__value__node_12;
static Const struct predinfo predinfo_klic__comp__insert_xins__value__node_12 = {
  (char *)"ins_value_node",
  &predicate_klic__comp__insert_xins__value__node_12
};

extern Const struct predicate predicate_klic__comp__insert_xins__redundant_1;
static Const struct predinfo predinfo_klic__comp__insert_xins__redundant_1 = {
  (char *)"ins_redundant",
  &predicate_klic__comp__insert_xins__redundant_1
};

extern Const struct predicate predicate_klic__comp__insert_xins__pattern_9;
static Const struct predinfo predinfo_klic__comp__insert_xins__pattern_9 = {
  (char *)"ins_pattern",
  &predicate_klic__comp__insert_xins__pattern_9
};

extern Const struct predicate predicate_klic__comp__insert_xins__merge__case_5;
static Const struct predinfo predinfo_klic__comp__insert_xins__merge__case_5 = {
  (char *)"ins_merge_case",
  &predicate_klic__comp__insert_xins__merge__case_5
};

extern Const struct predicate predicate_klic__comp__insert_xins__index__check__and__stop_7;
static Const struct predinfo predinfo_klic__comp__insert_xins__index__check__and__stop_7 = {
  (char *)"ins_index_check_and_stop",
  &predicate_klic__comp__insert_xins__index__check__and__stop_7
};

extern Const struct predicate predicate_klic__comp__insert_xins__index__check__and__go_7;
static Const struct predinfo predinfo_klic__comp__insert_xins__index__check__and__go_7 = {
  (char *)"ins_index_check_and_go",
  &predicate_klic__comp__insert_xins__index__check__and__go_7
};

extern Const struct predicate predicate_klic__comp__insert_xins__index__1_6;
static Const struct predinfo predinfo_klic__comp__insert_xins__index__1_6 = {
  (char *)"ins_index_1",
  &predicate_klic__comp__insert_xins__index__1_6
};

extern Const struct predicate predicate_klic__comp__insert_xins__index_6;
static Const struct predinfo predinfo_klic__comp__insert_xins__index_6 = {
  (char *)"ins_index",
  &predicate_klic__comp__insert_xins__index_6
};

extern Const struct predicate predicate_klic__comp__insert_xins__gb_6;
static Const struct predinfo predinfo_klic__comp__insert_xins__gb_6 = {
  (char *)"ins_gb",
  &predicate_klic__comp__insert_xins__gb_6
};

extern Const struct predicate predicate_klic__comp__insert_xins__funct__node_6;
static Const struct predinfo predinfo_klic__comp__insert_xins__funct__node_6 = {
  (char *)"ins_funct_node",
  &predicate_klic__comp__insert_xins__funct__node_6
};

extern Const struct predicate predicate_klic__comp__insert_xins__eq_7;
static Const struct predinfo predinfo_klic__comp__insert_xins__eq_7 = {
  (char *)"ins_eq",
  &predicate_klic__comp__insert_xins__eq_7
};

extern Const struct predicate predicate_klic__comp__insert_xins__atomic__node_6;
static Const struct predinfo predinfo_klic__comp__insert_xins__atomic__node_6 = {
  (char *)"ins_atomic_node",
  &predicate_klic__comp__insert_xins__atomic__node_6
};

static Const struct predinfo * Const preds_in_klic__comp__insert[] = {
  &predinfo_klic__comp__insert_xsubsumed__type_3,
  &predinfo_klic__comp__insert_xsubsumed__2_3,
  &predinfo_klic__comp__insert_xsubsumed__1_5,
  &predinfo_klic__comp__insert_xsubsumed_4,
  &predinfo_klic__comp__insert_xpattern__kind_2,
  &predinfo_klic__comp__insert_xmake__index__tree__1_7,
  &predinfo_klic__comp__insert_xmake__index__tree_5,
  &predinfo_klic__comp__insert_xins__value__node_12,
  &predinfo_klic__comp__insert_xins__redundant_1,
  &predinfo_klic__comp__insert_xins__pattern_9,
  &predinfo_klic__comp__insert_xins__merge__case_5,
  &predinfo_klic__comp__insert_xins__index__check__and__stop_7,
  &predinfo_klic__comp__insert_xins__index__check__and__go_7,
  &predinfo_klic__comp__insert_xins__index__1_6,
  &predinfo_klic__comp__insert_xins__index_6,
  &predinfo_klic__comp__insert_xins__gb_6,
  &predinfo_klic__comp__insert_xins__funct__node_6,
  &predinfo_klic__comp__insert_xins__eq_7,
  &predinfo_klic__comp__insert_xins__atomic__node_6,
  0
};
extern Const struct predicate predicate_klic__comp__normalize_xyes__const_5;
static Const struct predinfo predinfo_klic__comp__normalize_xyes__const_5 = {
  (char *)"yes_const",
  &predicate_klic__comp__normalize_xyes__const_5
};

extern Const struct predicate predicate_klic__comp__normalize_xseparate__cond__1_9;
static Const struct predinfo predinfo_klic__comp__normalize_xseparate__cond__1_9 = {
  (char *)"separate_cond_1",
  &predicate_klic__comp__normalize_xseparate__cond__1_9
};

extern Const struct predicate predicate_klic__comp__normalize_xseparate__cond_5;
static Const struct predinfo predinfo_klic__comp__normalize_xseparate__cond_5 = {
  (char *)"separate_cond",
  &predicate_klic__comp__normalize_xseparate__cond_5
};

extern Const struct predicate predicate_klic__comp__normalize_xnorm__top__goal__unify_8;
static Const struct predinfo predinfo_klic__comp__normalize_xnorm__top__goal__unify_8 = {
  (char *)"norm_top_goal_unify",
  &predicate_klic__comp__normalize_xnorm__top__goal__unify_8
};

extern Const struct predicate predicate_klic__comp__normalize_xnorm__top__goal__builtin_7;
static Const struct predinfo predinfo_klic__comp__normalize_xnorm__top__goal__builtin_7 = {
  (char *)"norm_top_goal_builtin",
  &predicate_klic__comp__normalize_xnorm__top__goal__builtin_7
};

extern Const struct predicate predicate_klic__comp__normalize_xnorm__top__goal_7;
static Const struct predinfo predinfo_klic__comp__normalize_xnorm__top__goal_7 = {
  (char *)"norm_top_goal",
  &predicate_klic__comp__normalize_xnorm__top__goal_7
};

extern Const struct predicate predicate_klic__comp__normalize_xnorm__tail__goal__unify_8;
static Const struct predinfo predinfo_klic__comp__normalize_xnorm__tail__goal__unify_8 = {
  (char *)"norm_tail_goal_unify",
  &predicate_klic__comp__normalize_xnorm__tail__goal__unify_8
};

extern Const struct predicate predicate_klic__comp__normalize_xnorm__tail__goal__generic_9;
static Const struct predinfo predinfo_klic__comp__normalize_xnorm__tail__goal__generic_9 = {
  (char *)"norm_tail_goal_generic",
  &predicate_klic__comp__normalize_xnorm__tail__goal__generic_9
};

extern Const struct predicate predicate_klic__comp__normalize_xnorm__tail__goal__builtin_8;
static Const struct predinfo predinfo_klic__comp__normalize_xnorm__tail__goal__builtin_8 = {
  (char *)"norm_tail_goal_builtin",
  &predicate_klic__comp__normalize_xnorm__tail__goal__builtin_8
};

extern Const struct predicate predicate_klic__comp__normalize_xnorm__tail__goal_7;
static Const struct predinfo predinfo_klic__comp__normalize_xnorm__tail__goal_7 = {
  (char *)"norm_tail_goal",
  &predicate_klic__comp__normalize_xnorm__tail__goal_7
};

extern Const struct predicate predicate_klic__comp__normalize_xnorm__pragma__priority_8;
static Const struct predinfo predinfo_klic__comp__normalize_xnorm__pragma__priority_8 = {
  (char *)"norm_pragma_priority",
  &predicate_klic__comp__normalize_xnorm__pragma__priority_8
};

extern Const struct predicate predicate_klic__comp__normalize_xnorm__pragma__pragma__arg_8;
static Const struct predinfo predinfo_klic__comp__normalize_xnorm__pragma__pragma__arg_8 = {
  (char *)"norm_pragma_pragma_arg",
  &predicate_klic__comp__normalize_xnorm__pragma__pragma__arg_8
};

extern Const struct predicate predicate_klic__comp__normalize_xnorm__pragma__node_8;
static Const struct predinfo predinfo_klic__comp__normalize_xnorm__pragma__node_8 = {
  (char *)"norm_pragma_node",
  &predicate_klic__comp__normalize_xnorm__pragma__node_8
};

extern Const struct predicate predicate_klic__comp__normalize_xnorm__pragma__lower__priority_8;
static Const struct predinfo predinfo_klic__comp__normalize_xnorm__pragma__lower__priority_8 = {
  (char *)"norm_pragma_lower_priority",
  &predicate_klic__comp__normalize_xnorm__pragma__lower__priority_8
};

extern Const struct predicate predicate_klic__comp__normalize_xnorm__pragma__1_8;
static Const struct predinfo predinfo_klic__comp__normalize_xnorm__pragma__1_8 = {
  (char *)"norm_pragma_1",
  &predicate_klic__comp__normalize_xnorm__pragma__1_8
};

extern Const struct predicate predicate_klic__comp__normalize_xnorm__pragma__0_9;
static Const struct predinfo predinfo_klic__comp__normalize_xnorm__pragma__0_9 = {
  (char *)"norm_pragma_0",
  &predicate_klic__comp__normalize_xnorm__pragma__0_9
};

extern Const struct predicate predicate_klic__comp__normalize_xnorm__pragma_8;
static Const struct predinfo predinfo_klic__comp__normalize_xnorm__pragma_8 = {
  (char *)"norm_pragma",
  &predicate_klic__comp__normalize_xnorm__pragma_8
};

extern Const struct predicate predicate_klic__comp__normalize_xnorm__inline__arg__1_7;
static Const struct predinfo predinfo_klic__comp__normalize_xnorm__inline__arg__1_7 = {
  (char *)"norm_inline_arg_1",
  &predicate_klic__comp__normalize_xnorm__inline__arg__1_7
};

extern Const struct predicate predicate_klic__comp__normalize_xnorm__inline__arg_11;
static Const struct predinfo predinfo_klic__comp__normalize_xnorm__inline__arg_11 = {
  (char *)"norm_inline_arg",
  &predicate_klic__comp__normalize_xnorm__inline__arg_11
};

extern Const struct predicate predicate_klic__comp__normalize_xnorm__inline_2F12_240_13;
static Const struct predinfo predinfo_klic__comp__normalize_xnorm__inline_2F12_240_13 = {
  (char *)"norm_inline/12$0",
  &predicate_klic__comp__normalize_xnorm__inline_2F12_240_13
};

extern Const struct predicate predicate_klic__comp__normalize_xnorm__inline_12;
static Const struct predinfo predinfo_klic__comp__normalize_xnorm__inline_12 = {
  (char *)"norm_inline",
  &predicate_klic__comp__normalize_xnorm__inline_12
};

extern Const struct predicate predicate_klic__comp__normalize_xnorm__head_5;
static Const struct predinfo predinfo_klic__comp__normalize_xnorm__head_5 = {
  (char *)"norm_head",
  &predicate_klic__comp__normalize_xnorm__head_5
};

extern Const struct predicate predicate_klic__comp__normalize_xnorm__guard__unify__new_6;
static Const struct predinfo predinfo_klic__comp__normalize_xnorm__guard__unify__new_6 = {
  (char *)"norm_guard_unify_new",
  &predicate_klic__comp__normalize_xnorm__guard__unify__new_6
};

extern Const struct predicate predicate_klic__comp__normalize_xnorm__guard__unify__new_2F6_240_7;
static Const struct predinfo predinfo_klic__comp__normalize_xnorm__guard__unify__new_2F6_240_7 = {
  (char *)"norm_guard_unify_new/6$0",
  &predicate_klic__comp__normalize_xnorm__guard__unify__new_2F6_240_7
};

extern Const struct predicate predicate_klic__comp__normalize_xnorm__guard__unify__args_10;
static Const struct predinfo predinfo_klic__comp__normalize_xnorm__guard__unify__args_10 = {
  (char *)"norm_guard_unify_args",
  &predicate_klic__comp__normalize_xnorm__guard__unify__args_10
};

extern Const struct predicate predicate_klic__comp__normalize_xnorm__guard__unify_9;
static Const struct predinfo predinfo_klic__comp__normalize_xnorm__guard__unify_9 = {
  (char *)"norm_guard_unify",
  &predicate_klic__comp__normalize_xnorm__guard__unify_9
};

extern Const struct predicate predicate_klic__comp__normalize_xnorm__guard__unify_2F9_241_2F10_240_9;
static Const struct predinfo predinfo_klic__comp__normalize_xnorm__guard__unify_2F9_241_2F10_240_9 = {
  (char *)"norm_guard_unify/9$1/10$0",
  &predicate_klic__comp__normalize_xnorm__guard__unify_2F9_241_2F10_240_9
};

extern Const struct predicate predicate_klic__comp__normalize_xnorm__guard__unify_2F9_241_10;
static Const struct predinfo predinfo_klic__comp__normalize_xnorm__guard__unify_2F9_241_10 = {
  (char *)"norm_guard_unify/9$1",
  &predicate_klic__comp__normalize_xnorm__guard__unify_2F9_241_10
};

extern Const struct predicate predicate_klic__comp__normalize_xnorm__guard__unify_2F9_240_10;
static Const struct predinfo predinfo_klic__comp__normalize_xnorm__guard__unify_2F9_240_10 = {
  (char *)"norm_guard_unify/9$0",
  &predicate_klic__comp__normalize_xnorm__guard__unify_2F9_240_10
};

extern Const struct predicate predicate_klic__comp__normalize_xnorm__guard__term__1_7;
static Const struct predinfo predinfo_klic__comp__normalize_xnorm__guard__term__1_7 = {
  (char *)"norm_guard_term_1",
  &predicate_klic__comp__normalize_xnorm__guard__term__1_7
};

extern Const struct predicate predicate_klic__comp__normalize_xnorm__guard__term_6;
static Const struct predinfo predinfo_klic__comp__normalize_xnorm__guard__term_6 = {
  (char *)"norm_guard_term",
  &predicate_klic__comp__normalize_xnorm__guard__term_6
};

extern Const struct predicate predicate_klic__comp__normalize_xnorm__guard__goal_8;
static Const struct predinfo predinfo_klic__comp__normalize_xnorm__guard__goal_8 = {
  (char *)"norm_guard_goal",
  &predicate_klic__comp__normalize_xnorm__guard__goal_8
};

extern Const struct predicate predicate_klic__comp__normalize_xnorm__guard__goal_2F8_241_8;
static Const struct predinfo predinfo_klic__comp__normalize_xnorm__guard__goal_2F8_241_8 = {
  (char *)"norm_guard_goal/8$1",
  &predicate_klic__comp__normalize_xnorm__guard__goal_2F8_241_8
};

extern Const struct predicate predicate_klic__comp__normalize_xnorm__guard__goal_2F8_240_8;
static Const struct predinfo predinfo_klic__comp__normalize_xnorm__guard__goal_2F8_240_8 = {
  (char *)"norm_guard_goal/8$0",
  &predicate_klic__comp__normalize_xnorm__guard__goal_2F8_240_8
};

extern Const struct predicate predicate_klic__comp__normalize_xnorm__guard__generic__3_13;
static Const struct predinfo predinfo_klic__comp__normalize_xnorm__guard__generic__3_13 = {
  (char *)"norm_guard_generic_3",
  &predicate_klic__comp__normalize_xnorm__guard__generic__3_13
};

extern Const struct predicate predicate_klic__comp__normalize_xnorm__guard__generic__2_14;
static Const struct predinfo predinfo_klic__comp__normalize_xnorm__guard__generic__2_14 = {
  (char *)"norm_guard_generic_2",
  &predicate_klic__comp__normalize_xnorm__guard__generic__2_14
};

extern Const struct predicate predicate_klic__comp__normalize_xnorm__guard__generic__1_14;
static Const struct predinfo predinfo_klic__comp__normalize_xnorm__guard__generic__1_14 = {
  (char *)"norm_guard_generic_1",
  &predicate_klic__comp__normalize_xnorm__guard__generic__1_14
};

extern Const struct predicate predicate_klic__comp__normalize_xnorm__guard__generic_8;
static Const struct predinfo predinfo_klic__comp__normalize_xnorm__guard__generic_8 = {
  (char *)"norm_guard_generic",
  &predicate_klic__comp__normalize_xnorm__guard__generic_8
};

extern Const struct predicate predicate_klic__comp__normalize_xnorm__guard__builts__1_12;
static Const struct predinfo predinfo_klic__comp__normalize_xnorm__guard__builts__1_12 = {
  (char *)"norm_guard_builts_1",
  &predicate_klic__comp__normalize_xnorm__guard__builts__1_12
};

extern Const struct predicate predicate_klic__comp__normalize_xnorm__guard__builts_8;
static Const struct predinfo predinfo_klic__comp__normalize_xnorm__guard__builts_8 = {
  (char *)"norm_guard_builts",
  &predicate_klic__comp__normalize_xnorm__guard__builts_8
};

extern Const struct predicate predicate_klic__comp__normalize_xnorm__guard_2F10_241_11;
static Const struct predinfo predinfo_klic__comp__normalize_xnorm__guard_2F10_241_11 = {
  (char *)"norm_guard/10$1",
  &predicate_klic__comp__normalize_xnorm__guard_2F10_241_11
};

extern Const struct predicate predicate_klic__comp__normalize_xnorm__guard_2F10_240_11;
static Const struct predinfo predinfo_klic__comp__normalize_xnorm__guard_2F10_240_11 = {
  (char *)"norm_guard/10$0",
  &predicate_klic__comp__normalize_xnorm__guard_2F10_240_11
};

extern Const struct predicate predicate_klic__comp__normalize_xnorm__guard_10;
static Const struct predinfo predinfo_klic__comp__normalize_xnorm__guard_10 = {
  (char *)"norm_guard",
  &predicate_klic__comp__normalize_xnorm__guard_10
};

extern Const struct predicate predicate_klic__comp__normalize_xnorm__gg__outs_9;
static Const struct predinfo predinfo_klic__comp__normalize_xnorm__gg__outs_9 = {
  (char *)"norm_gg_outs",
  &predicate_klic__comp__normalize_xnorm__gg__outs_9
};

extern Const struct predicate predicate_klic__comp__normalize_xnorm__gg__ins__1_9;
static Const struct predinfo predinfo_klic__comp__normalize_xnorm__gg__ins__1_9 = {
  (char *)"norm_gg_ins_1",
  &predicate_klic__comp__normalize_xnorm__gg__ins__1_9
};

extern Const struct predicate predicate_klic__comp__normalize_xnorm__gg__ins_8;
static Const struct predinfo predinfo_klic__comp__normalize_xnorm__gg__ins_8 = {
  (char *)"norm_gg_ins",
  &predicate_klic__comp__normalize_xnorm__gg__ins_8
};

extern Const struct predicate predicate_klic__comp__normalize_xnorm__gg__in__out_7;
static Const struct predinfo predinfo_klic__comp__normalize_xnorm__gg__in__out_7 = {
  (char *)"norm_gg_in_out",
  &predicate_klic__comp__normalize_xnorm__gg__in__out_7
};

extern Const struct predicate predicate_klic__comp__normalize_xnorm__gg__in__2_8;
static Const struct predinfo predinfo_klic__comp__normalize_xnorm__gg__in__2_8 = {
  (char *)"norm_gg_in_2",
  &predicate_klic__comp__normalize_xnorm__gg__in__2_8
};

extern Const struct predicate predicate_klic__comp__normalize_xnorm__gg__in__1_10;
static Const struct predinfo predinfo_klic__comp__normalize_xnorm__gg__in__1_10 = {
  (char *)"norm_gg_in_1",
  &predicate_klic__comp__normalize_xnorm__gg__in__1_10
};

extern Const struct predicate predicate_klic__comp__normalize_xnorm__gg__in_9;
static Const struct predinfo predinfo_klic__comp__normalize_xnorm__gg__in_9 = {
  (char *)"norm_gg_in",
  &predicate_klic__comp__normalize_xnorm__gg__in_9
};

extern Const struct predicate predicate_klic__comp__normalize_xnorm__gg__count__out_4;
static Const struct predinfo predinfo_klic__comp__normalize_xnorm__gg__count__out_4 = {
  (char *)"norm_gg_count_out",
  &predicate_klic__comp__normalize_xnorm__gg__count__out_4
};

extern Const struct predicate predicate_klic__comp__normalize_xnorm__gg__2_6;
static Const struct predinfo predinfo_klic__comp__normalize_xnorm__gg__2_6 = {
  (char *)"norm_gg_2",
  &predicate_klic__comp__normalize_xnorm__gg__2_6
};

extern Const struct predicate predicate_klic__comp__normalize_xnorm__gg__1_8;
static Const struct predinfo predinfo_klic__comp__normalize_xnorm__gg__1_8 = {
  (char *)"norm_gg_1",
  &predicate_klic__comp__normalize_xnorm__gg__1_8
};

extern Const struct predicate predicate_klic__comp__normalize_xnorm__gg_5;
static Const struct predinfo predinfo_klic__comp__normalize_xnorm__gg_5 = {
  (char *)"norm_gg",
  &predicate_klic__comp__normalize_xnorm__gg_5
};

extern Const struct predicate predicate_klic__comp__normalize_xnorm__gblt__args__1_9;
static Const struct predinfo predinfo_klic__comp__normalize_xnorm__gblt__args__1_9 = {
  (char *)"norm_gblt_args_1",
  &predicate_klic__comp__normalize_xnorm__gblt__args__1_9
};

extern Const struct predicate predicate_klic__comp__normalize_xnorm__gblt__args_8;
static Const struct predinfo predinfo_klic__comp__normalize_xnorm__gblt__args_8 = {
  (char *)"norm_gblt_args",
  &predicate_klic__comp__normalize_xnorm__gblt__args_8
};

extern Const struct predicate predicate_klic__comp__normalize_xnorm__gblt__arg__2_8;
static Const struct predinfo predinfo_klic__comp__normalize_xnorm__gblt__arg__2_8 = {
  (char *)"norm_gblt_arg_2",
  &predicate_klic__comp__normalize_xnorm__gblt__arg__2_8
};

extern Const struct predicate predicate_klic__comp__normalize_xnorm__gblt__arg__1_10;
static Const struct predinfo predinfo_klic__comp__normalize_xnorm__gblt__arg__1_10 = {
  (char *)"norm_gblt_arg_1",
  &predicate_klic__comp__normalize_xnorm__gblt__arg__1_10
};

extern Const struct predicate predicate_klic__comp__normalize_xnorm__gblt__arg_9;
static Const struct predinfo predinfo_klic__comp__normalize_xnorm__gblt__arg_9 = {
  (char *)"norm_gblt_arg",
  &predicate_klic__comp__normalize_xnorm__gblt__arg_9
};

extern Const struct predicate predicate_klic__comp__normalize_xnorm__body__tail__1_8;
static Const struct predinfo predinfo_klic__comp__normalize_xnorm__body__tail__1_8 = {
  (char *)"norm_body_tail_1",
  &predicate_klic__comp__normalize_xnorm__body__tail__1_8
};

extern Const struct predicate predicate_klic__comp__normalize_xnorm__body__tail_7;
static Const struct predinfo predinfo_klic__comp__normalize_xnorm__body__tail_7 = {
  (char *)"norm_body_tail",
  &predicate_klic__comp__normalize_xnorm__body__tail_7
};

extern Const struct predicate predicate_klic__comp__normalize_xnorm__body__args__1_8;
static Const struct predinfo predinfo_klic__comp__normalize_xnorm__body__args__1_8 = {
  (char *)"norm_body_args_1",
  &predicate_klic__comp__normalize_xnorm__body__args__1_8
};

extern Const struct predicate predicate_klic__comp__normalize_xnorm__body__args_9;
static Const struct predinfo predinfo_klic__comp__normalize_xnorm__body__args_9 = {
  (char *)"norm_body_args",
  &predicate_klic__comp__normalize_xnorm__body__args_9
};

extern Const struct predicate predicate_klic__comp__normalize_xnorm__body__args_7;
static Const struct predinfo predinfo_klic__comp__normalize_xnorm__body__args_7 = {
  (char *)"norm_body_args",
  &predicate_klic__comp__normalize_xnorm__body__args_7
};

extern Const struct predicate predicate_klic__comp__normalize_xnorm__body__arg__3__2_6;
static Const struct predinfo predinfo_klic__comp__normalize_xnorm__body__arg__3__2_6 = {
  (char *)"norm_body_arg_3_2",
  &predicate_klic__comp__normalize_xnorm__body__arg__3__2_6
};

extern Const struct predicate predicate_klic__comp__normalize_xnorm__body__arg__3__1_10;
static Const struct predinfo predinfo_klic__comp__normalize_xnorm__body__arg__3__1_10 = {
  (char *)"norm_body_arg_3_1",
  &predicate_klic__comp__normalize_xnorm__body__arg__3__1_10
};

extern Const struct predicate predicate_klic__comp__normalize_xnorm__body__arg__3_8;
static Const struct predinfo predinfo_klic__comp__normalize_xnorm__body__arg__3_8 = {
  (char *)"norm_body_arg_3",
  &predicate_klic__comp__normalize_xnorm__body__arg__3_8
};

extern Const struct predicate predicate_klic__comp__normalize_xnorm__body__arg__2_7;
static Const struct predinfo predinfo_klic__comp__normalize_xnorm__body__arg__2_7 = {
  (char *)"norm_body_arg_2",
  &predicate_klic__comp__normalize_xnorm__body__arg__2_7
};

extern Const struct predicate predicate_klic__comp__normalize_xnorm__body__arg__1_8;
static Const struct predinfo predinfo_klic__comp__normalize_xnorm__body__arg__1_8 = {
  (char *)"norm_body_arg_1",
  &predicate_klic__comp__normalize_xnorm__body__arg__1_8
};

extern Const struct predicate predicate_klic__comp__normalize_xnorm__body__arg_7;
static Const struct predinfo predinfo_klic__comp__normalize_xnorm__body__arg_7 = {
  (char *)"norm_body_arg",
  &predicate_klic__comp__normalize_xnorm__body__arg_7
};

extern Const struct predicate predicate_klic__comp__normalize_xnorm__body__1_9;
static Const struct predinfo predinfo_klic__comp__normalize_xnorm__body__1_9 = {
  (char *)"norm_body_1",
  &predicate_klic__comp__normalize_xnorm__body__1_9
};

extern Const struct predicate predicate_klic__comp__normalize_xnorm__body_6;
static Const struct predinfo predinfo_klic__comp__normalize_xnorm__body_6 = {
  (char *)"norm_body",
  &predicate_klic__comp__normalize_xnorm__body_6
};

extern Const struct predicate predicate_klic__comp__normalize_xnorm__args_8;
static Const struct predinfo predinfo_klic__comp__normalize_xnorm__args_8 = {
  (char *)"norm_args",
  &predicate_klic__comp__normalize_xnorm__args_8
};

extern Const struct predicate predicate_klic__comp__normalize_xmake__arg__assoc_3;
static Const struct predinfo predinfo_klic__comp__normalize_xmake__arg__assoc_3 = {
  (char *)"make_arg_assoc",
  &predicate_klic__comp__normalize_xmake__arg__assoc_3
};

extern Const struct predicate predicate_klic__comp__normalize_xis__const__args_3;
static Const struct predinfo predinfo_klic__comp__normalize_xis__const__args_3 = {
  (char *)"is_const_args",
  &predicate_klic__comp__normalize_xis__const__args_3
};

extern Const struct predicate predicate_klic__comp__normalize_xis__const__1_6;
static Const struct predinfo predinfo_klic__comp__normalize_xis__const__1_6 = {
  (char *)"is_const_1",
  &predicate_klic__comp__normalize_xis__const__1_6
};

extern Const struct predicate predicate_klic__comp__normalize_xis__const_5;
static Const struct predinfo predinfo_klic__comp__normalize_xis__const_5 = {
  (char *)"is_const",
  &predicate_klic__comp__normalize_xis__const_5
};

extern Const struct predicate predicate_klic__comp__normalize_xinline__arg__type_2;
static Const struct predinfo predinfo_klic__comp__normalize_xinline__arg__type_2 = {
  (char *)"inline_arg_type",
  &predicate_klic__comp__normalize_xinline__arg__type_2
};

extern Const struct predicate predicate_klic__comp__normalize_xgenerate_8;
static Const struct predinfo predinfo_klic__comp__normalize_xgenerate_8 = {
  (char *)"generate",
  &predicate_klic__comp__normalize_xgenerate_8
};

extern Const struct predicate predicate_klic__comp__normalize_xcheck__gblt__input_5;
static Const struct predinfo predinfo_klic__comp__normalize_xcheck__gblt__input_5 = {
  (char *)"check_gblt_input",
  &predicate_klic__comp__normalize_xcheck__gblt__input_5
};

static Const struct predinfo * Const preds_in_klic__comp__normalize[] = {
  &predinfo_klic__comp__normalize_xyes__const_5,
  &predinfo_klic__comp__normalize_xseparate__cond__1_9,
  &predinfo_klic__comp__normalize_xseparate__cond_5,
  &predinfo_klic__comp__normalize_xnorm__top__goal__unify_8,
  &predinfo_klic__comp__normalize_xnorm__top__goal__builtin_7,
  &predinfo_klic__comp__normalize_xnorm__top__goal_7,
  &predinfo_klic__comp__normalize_xnorm__tail__goal__unify_8,
  &predinfo_klic__comp__normalize_xnorm__tail__goal__generic_9,
  &predinfo_klic__comp__normalize_xnorm__tail__goal__builtin_8,
  &predinfo_klic__comp__normalize_xnorm__tail__goal_7,
  &predinfo_klic__comp__normalize_xnorm__pragma__priority_8,
  &predinfo_klic__comp__normalize_xnorm__pragma__pragma__arg_8,
  &predinfo_klic__comp__normalize_xnorm__pragma__node_8,
  &predinfo_klic__comp__normalize_xnorm__pragma__lower__priority_8,
  &predinfo_klic__comp__normalize_xnorm__pragma__1_8,
  &predinfo_klic__comp__normalize_xnorm__pragma__0_9,
  &predinfo_klic__comp__normalize_xnorm__pragma_8,
  &predinfo_klic__comp__normalize_xnorm__inline__arg__1_7,
  &predinfo_klic__comp__normalize_xnorm__inline__arg_11,
  &predinfo_klic__comp__normalize_xnorm__inline_2F12_240_13,
  &predinfo_klic__comp__normalize_xnorm__inline_12,
  &predinfo_klic__comp__normalize_xnorm__head_5,
  &predinfo_klic__comp__normalize_xnorm__guard__unify__new_6,
  &predinfo_klic__comp__normalize_xnorm__guard__unify__new_2F6_240_7,
  &predinfo_klic__comp__normalize_xnorm__guard__unify__args_10,
  &predinfo_klic__comp__normalize_xnorm__guard__unify_9,
  &predinfo_klic__comp__normalize_xnorm__guard__unify_2F9_241_2F10_240_9,
  &predinfo_klic__comp__normalize_xnorm__guard__unify_2F9_241_10,
  &predinfo_klic__comp__normalize_xnorm__guard__unify_2F9_240_10,
  &predinfo_klic__comp__normalize_xnorm__guard__term__1_7,
  &predinfo_klic__comp__normalize_xnorm__guard__term_6,
  &predinfo_klic__comp__normalize_xnorm__guard__goal_8,
  &predinfo_klic__comp__normalize_xnorm__guard__goal_2F8_241_8,
  &predinfo_klic__comp__normalize_xnorm__guard__goal_2F8_240_8,
  &predinfo_klic__comp__normalize_xnorm__guard__generic__3_13,
  &predinfo_klic__comp__normalize_xnorm__guard__generic__2_14,
  &predinfo_klic__comp__normalize_xnorm__guard__generic__1_14,
  &predinfo_klic__comp__normalize_xnorm__guard__generic_8,
  &predinfo_klic__comp__normalize_xnorm__guard__builts__1_12,
  &predinfo_klic__comp__normalize_xnorm__guard__builts_8,
  &predinfo_klic__comp__normalize_xnorm__guard_2F10_241_11,
  &predinfo_klic__comp__normalize_xnorm__guard_2F10_240_11,
  &predinfo_klic__comp__normalize_xnorm__guard_10,
  &predinfo_klic__comp__normalize_xnorm__gg__outs_9,
  &predinfo_klic__comp__normalize_xnorm__gg__ins__1_9,
  &predinfo_klic__comp__normalize_xnorm__gg__ins_8,
  &predinfo_klic__comp__normalize_xnorm__gg__in__out_7,
  &predinfo_klic__comp__normalize_xnorm__gg__in__2_8,
  &predinfo_klic__comp__normalize_xnorm__gg__in__1_10,
  &predinfo_klic__comp__normalize_xnorm__gg__in_9,
  &predinfo_klic__comp__normalize_xnorm__gg__count__out_4,
  &predinfo_klic__comp__normalize_xnorm__gg__2_6,
  &predinfo_klic__comp__normalize_xnorm__gg__1_8,
  &predinfo_klic__comp__normalize_xnorm__gg_5,
  &predinfo_klic__comp__normalize_xnorm__gblt__args__1_9,
  &predinfo_klic__comp__normalize_xnorm__gblt__args_8,
  &predinfo_klic__comp__normalize_xnorm__gblt__arg__2_8,
  &predinfo_klic__comp__normalize_xnorm__gblt__arg__1_10,
  &predinfo_klic__comp__normalize_xnorm__gblt__arg_9,
  &predinfo_klic__comp__normalize_xnorm__body__tail__1_8,
  &predinfo_klic__comp__normalize_xnorm__body__tail_7,
  &predinfo_klic__comp__normalize_xnorm__body__args__1_8,
  &predinfo_klic__comp__normalize_xnorm__body__args_9,
  &predinfo_klic__comp__normalize_xnorm__body__args_7,
  &predinfo_klic__comp__normalize_xnorm__body__arg__3__2_6,
  &predinfo_klic__comp__normalize_xnorm__body__arg__3__1_10,
  &predinfo_klic__comp__normalize_xnorm__body__arg__3_8,
  &predinfo_klic__comp__normalize_xnorm__body__arg__2_7,
  &predinfo_klic__comp__normalize_xnorm__body__arg__1_8,
  &predinfo_klic__comp__normalize_xnorm__body__arg_7,
  &predinfo_klic__comp__normalize_xnorm__body__1_9,
  &predinfo_klic__comp__normalize_xnorm__body_6,
  &predinfo_klic__comp__normalize_xnorm__args_8,
  &predinfo_klic__comp__normalize_xmake__arg__assoc_3,
  &predinfo_klic__comp__normalize_xis__const__args_3,
  &predinfo_klic__comp__normalize_xis__const__1_6,
  &predinfo_klic__comp__normalize_xis__const_5,
  &predinfo_klic__comp__normalize_xinline__arg__type_2,
  &predinfo_klic__comp__normalize_xgenerate_8,
  &predinfo_klic__comp__normalize_xcheck__gblt__input_5,
  0
};
extern Const struct predicate predicate_klic__comp__util_xvector__to__list_4;
static Const struct predinfo predinfo_klic__comp__util_xvector__to__list_4 = {
  (char *)"vector_to_list",
  &predicate_klic__comp__util_xvector__to__list_4
};

extern Const struct predicate predicate_klic__comp__util_xvector__to__list_2;
static Const struct predinfo predinfo_klic__comp__util_xvector__to__list_2 = {
  (char *)"vector_to_list",
  &predicate_klic__comp__util_xvector__to__list_2
};

extern Const struct predicate predicate_klic__comp__util_xuniv__funct__to__list_5;
static Const struct predinfo predinfo_klic__comp__util_xuniv__funct__to__list_5 = {
  (char *)"univ_funct_to_list",
  &predicate_klic__comp__util_xuniv__funct__to__list_5
};

extern Const struct predicate predicate_klic__comp__util_xuniv_3;
static Const struct predinfo predinfo_klic__comp__util_xuniv_3 = {
  (char *)"univ",
  &predicate_klic__comp__util_xuniv_3
};

extern Const struct predicate predicate_klic__comp__util_xuniv_2;
static Const struct predinfo predinfo_klic__comp__util_xuniv_2 = {
  (char *)"univ",
  &predicate_klic__comp__util_xuniv_2
};

extern Const struct predicate predicate_klic__comp__util_xstring__to__char__list_4;
static Const struct predinfo predinfo_klic__comp__util_xstring__to__char__list_4 = {
  (char *)"string_to_char_list",
  &predicate_klic__comp__util_xstring__to__char__list_4
};

extern Const struct predicate predicate_klic__comp__util_xstring__to__char__list_2;
static Const struct predinfo predinfo_klic__comp__util_xstring__to__char__list_2 = {
  (char *)"string_to_char_list",
  &predicate_klic__comp__util_xstring__to__char__list_2
};

extern Const struct predicate predicate_klic__comp__util_xremove_4;
static Const struct predinfo predinfo_klic__comp__util_xremove_4 = {
  (char *)"remove",
  &predicate_klic__comp__util_xremove_4
};

extern Const struct predicate predicate_klic__comp__util_xqsort_3;
static Const struct predinfo predinfo_klic__comp__util_xqsort_3 = {
  (char *)"qsort",
  &predicate_klic__comp__util_xqsort_3
};

extern Const struct predicate predicate_klic__comp__util_xqsort_2;
static Const struct predinfo predinfo_klic__comp__util_xqsort_2 = {
  (char *)"qsort",
  &predicate_klic__comp__util_xqsort_2
};

extern Const struct predicate predicate_klic__comp__util_xpartition_4;
static Const struct predinfo predinfo_klic__comp__util_xpartition_4 = {
  (char *)"partition",
  &predicate_klic__comp__util_xpartition_4
};

extern Const struct predicate predicate_klic__comp__util_xnumbervars__vector_7;
static Const struct predinfo predinfo_klic__comp__util_xnumbervars__vector_7 = {
  (char *)"numbervars_vector",
  &predicate_klic__comp__util_xnumbervars__vector_7
};

extern Const struct predicate predicate_klic__comp__util_xnumbervars__funct_9;
static Const struct predinfo predinfo_klic__comp__util_xnumbervars__funct_9 = {
  (char *)"numbervars_funct",
  &predicate_klic__comp__util_xnumbervars__funct_9
};

extern Const struct predicate predicate_klic__comp__util_xnumbervars__1_7;
static Const struct predinfo predinfo_klic__comp__util_xnumbervars__1_7 = {
  (char *)"numbervars_1",
  &predicate_klic__comp__util_xnumbervars__1_7
};

extern Const struct predicate predicate_klic__comp__util_xnumbervars_6;
static Const struct predinfo predinfo_klic__comp__util_xnumbervars_6 = {
  (char *)"numbervars",
  &predicate_klic__comp__util_xnumbervars_6
};

extern Const struct predicate predicate_klic__comp__util_xnumbervars_4;
static Const struct predinfo predinfo_klic__comp__util_xnumbervars_4 = {
  (char *)"numbervars",
  &predicate_klic__comp__util_xnumbervars_4
};

extern Const struct predicate predicate_klic__comp__util_xnth_3;
static Const struct predinfo predinfo_klic__comp__util_xnth_3 = {
  (char *)"nth",
  &predicate_klic__comp__util_xnth_3
};

extern Const struct predicate predicate_klic__comp__util_xname__integer_3;
static Const struct predinfo predinfo_klic__comp__util_xname__integer_3 = {
  (char *)"name_integer",
  &predicate_klic__comp__util_xname__integer_3
};

extern Const struct predicate predicate_klic__comp__util_xname__integer_2;
static Const struct predinfo predinfo_klic__comp__util_xname__integer_2 = {
  (char *)"name_integer",
  &predicate_klic__comp__util_xname__integer_2
};

extern Const struct predicate predicate_klic__comp__util_xname_2;
static Const struct predinfo predinfo_klic__comp__util_xname_2 = {
  (char *)"name",
  &predicate_klic__comp__util_xname_2
};

extern Const struct predicate predicate_klic__comp__util_xmin_3;
static Const struct predinfo predinfo_klic__comp__util_xmin_3 = {
  (char *)"min",
  &predicate_klic__comp__util_xmin_3
};

extern Const struct predicate predicate_klic__comp__util_xmember_3;
static Const struct predinfo predinfo_klic__comp__util_xmember_3 = {
  (char *)"member",
  &predicate_klic__comp__util_xmember_3
};

extern Const struct predicate predicate_klic__comp__util_xmax__vect_4;
static Const struct predinfo predinfo_klic__comp__util_xmax__vect_4 = {
  (char *)"max_vect",
  &predicate_klic__comp__util_xmax__vect_4
};

extern Const struct predicate predicate_klic__comp__util_xmax__vect_2;
static Const struct predinfo predinfo_klic__comp__util_xmax__vect_2 = {
  (char *)"max_vect",
  &predicate_klic__comp__util_xmax__vect_2
};

extern Const struct predicate predicate_klic__comp__util_xmax_3;
static Const struct predinfo predinfo_klic__comp__util_xmax_3 = {
  (char *)"max",
  &predicate_klic__comp__util_xmax_3
};

extern Const struct predicate predicate_klic__comp__util_xmake__atom_2;
static Const struct predinfo predinfo_klic__comp__util_xmake__atom_2 = {
  (char *)"make_atom",
  &predicate_klic__comp__util_xmake__atom_2
};

extern Const struct predicate predicate_klic__comp__util_xlength_3;
static Const struct predinfo predinfo_klic__comp__util_xlength_3 = {
  (char *)"length",
  &predicate_klic__comp__util_xlength_3
};

extern Const struct predicate predicate_klic__comp__util_xlength_2;
static Const struct predinfo predinfo_klic__comp__util_xlength_2 = {
  (char *)"length",
  &predicate_klic__comp__util_xlength_2
};

extern Const struct predicate predicate_klic__comp__util_xassoc_3;
static Const struct predinfo predinfo_klic__comp__util_xassoc_3 = {
  (char *)"assoc",
  &predicate_klic__comp__util_xassoc_3
};

extern Const struct predicate predicate_klic__comp__util_xappend_3;
static Const struct predinfo predinfo_klic__comp__util_xappend_3 = {
  (char *)"append",
  &predicate_klic__comp__util_xappend_3
};

static Const struct predinfo * Const preds_in_klic__comp__util[] = {
  &predinfo_klic__comp__util_xvector__to__list_4,
  &predinfo_klic__comp__util_xvector__to__list_2,
  &predinfo_klic__comp__util_xuniv__funct__to__list_5,
  &predinfo_klic__comp__util_xuniv_3,
  &predinfo_klic__comp__util_xuniv_2,
  &predinfo_klic__comp__util_xstring__to__char__list_4,
  &predinfo_klic__comp__util_xstring__to__char__list_2,
  &predinfo_klic__comp__util_xremove_4,
  &predinfo_klic__comp__util_xqsort_3,
  &predinfo_klic__comp__util_xqsort_2,
  &predinfo_klic__comp__util_xpartition_4,
  &predinfo_klic__comp__util_xnumbervars__vector_7,
  &predinfo_klic__comp__util_xnumbervars__funct_9,
  &predinfo_klic__comp__util_xnumbervars__1_7,
  &predinfo_klic__comp__util_xnumbervars_6,
  &predinfo_klic__comp__util_xnumbervars_4,
  &predinfo_klic__comp__util_xnth_3,
  &predinfo_klic__comp__util_xname__integer_3,
  &predinfo_klic__comp__util_xname__integer_2,
  &predinfo_klic__comp__util_xname_2,
  &predinfo_klic__comp__util_xmin_3,
  &predinfo_klic__comp__util_xmember_3,
  &predinfo_klic__comp__util_xmax__vect_4,
  &predinfo_klic__comp__util_xmax__vect_2,
  &predinfo_klic__comp__util_xmax_3,
  &predinfo_klic__comp__util_xmake__atom_2,
  &predinfo_klic__comp__util_xlength_3,
  &predinfo_klic__comp__util_xlength_2,
  &predinfo_klic__comp__util_xassoc_3,
  &predinfo_klic__comp__util_xappend_3,
  0
};
extern Const struct predicate predicate_comparator_xsort_5;
static Const struct predinfo predinfo_comparator_xsort_5 = {
  (char *)"sort",
  &predicate_comparator_xsort_5
};

static Const struct predinfo * Const preds_in_comparator[] = {
  &predinfo_comparator_xsort_5,
  0
};
extern Const struct predicate predicate_splay__tree_xupdate__right_2F11_240_15;
static Const struct predinfo predinfo_splay__tree_xupdate__right_2F11_240_15 = {
  (char *)"update_right/11$0",
  &predicate_splay__tree_xupdate__right_2F11_240_15
};

extern Const struct predicate predicate_splay__tree_xupdate__right_11;
static Const struct predinfo predinfo_splay__tree_xupdate__right_11 = {
  (char *)"update_right",
  &predicate_splay__tree_xupdate__right_11
};

extern Const struct predicate predicate_splay__tree_xupdate__left_2F11_240_15;
static Const struct predinfo predinfo_splay__tree_xupdate__left_2F11_240_15 = {
  (char *)"update_left/11$0",
  &predicate_splay__tree_xupdate__left_2F11_240_15
};

extern Const struct predicate predicate_splay__tree_xupdate__left_11;
static Const struct predinfo predinfo_splay__tree_xupdate__left_11 = {
  (char *)"update_left",
  &predicate_splay__tree_xupdate__left_11
};

extern Const struct predicate predicate_splay__tree_xupdate_8;
static Const struct predinfo predinfo_splay__tree_xupdate_8 = {
  (char *)"update",
  &predicate_splay__tree_xupdate_8
};

extern Const struct predicate predicate_splay__tree_xupdate_6;
static Const struct predinfo predinfo_splay__tree_xupdate_6 = {
  (char *)"update",
  &predicate_splay__tree_xupdate_6
};

extern Const struct predicate predicate_splay__tree_xupdate_5;
static Const struct predinfo predinfo_splay__tree_xupdate_5 = {
  (char *)"update",
  &predicate_splay__tree_xupdate_5
};

extern Const struct predicate predicate_splay__tree_xupdate_2F8_240_12;
static Const struct predinfo predinfo_splay__tree_xupdate_2F8_240_12 = {
  (char *)"update/8$0",
  &predicate_splay__tree_xupdate_2F8_240_12
};

extern Const struct predicate predicate_splay__tree_xreform__tree_3;
static Const struct predinfo predinfo_splay__tree_xreform__tree_3 = {
  (char *)"reform_tree",
  &predicate_splay__tree_xreform__tree_3
};

extern Const struct predicate predicate_splay__tree_xrefer_5;
static Const struct predinfo predinfo_splay__tree_xrefer_5 = {
  (char *)"refer",
  &predicate_splay__tree_xrefer_5
};

extern Const struct predicate predicate_splay__tree_xput_4;
static Const struct predinfo predinfo_splay__tree_xput_4 = {
  (char *)"put",
  &predicate_splay__tree_xput_4
};

extern Const struct predicate predicate_splay__tree_xget__min__sub_4;
static Const struct predinfo predinfo_splay__tree_xget__min__sub_4 = {
  (char *)"get_min_sub",
  &predicate_splay__tree_xget__min__sub_4
};

extern Const struct predicate predicate_splay__tree_xget__min_4;
static Const struct predinfo predinfo_splay__tree_xget__min_4 = {
  (char *)"get_min",
  &predicate_splay__tree_xget__min_4
};

extern Const struct predicate predicate_splay__tree_xget__max__sub_4;
static Const struct predinfo predinfo_splay__tree_xget__max__sub_4 = {
  (char *)"get_max_sub",
  &predicate_splay__tree_xget__max__sub_4
};

extern Const struct predicate predicate_splay__tree_xget__max_4;
static Const struct predinfo predinfo_splay__tree_xget__max_4 = {
  (char *)"get_max",
  &predicate_splay__tree_xget__max_4
};

extern Const struct predicate predicate_splay__tree_xget__all__sub_3;
static Const struct predinfo predinfo_splay__tree_xget__all__sub_3 = {
  (char *)"get_all_sub",
  &predicate_splay__tree_xget__all__sub_3
};

extern Const struct predicate predicate_splay__tree_xget__all_3;
static Const struct predinfo predinfo_splay__tree_xget__all_3 = {
  (char *)"get_all",
  &predicate_splay__tree_xget__all_3
};

extern Const struct predicate predicate_splay__tree_xget_5;
static Const struct predinfo predinfo_splay__tree_xget_5 = {
  (char *)"get",
  &predicate_splay__tree_xget_5
};

extern Const struct predicate predicate_splay__tree_xget_4;
static Const struct predinfo predinfo_splay__tree_xget_4 = {
  (char *)"get",
  &predicate_splay__tree_xget_4
};

extern Const struct predicate predicate_splay__tree_xget_2F5_241_3;
static Const struct predinfo predinfo_splay__tree_xget_2F5_241_3 = {
  (char *)"get/5$1",
  &predicate_splay__tree_xget_2F5_241_3
};

extern Const struct predicate predicate_splay__tree_xget_2F5_240_9;
static Const struct predinfo predinfo_splay__tree_xget_2F5_240_9 = {
  (char *)"get/5$0",
  &predicate_splay__tree_xget_2F5_240_9
};

extern Const struct predicate predicate_splay__tree_xempty__sub_4;
static Const struct predinfo predinfo_splay__tree_xempty__sub_4 = {
  (char *)"empty_sub",
  &predicate_splay__tree_xempty__sub_4
};

extern Const struct predicate predicate_splay__tree_xempty__sub_2F4_240_6;
static Const struct predinfo predinfo_splay__tree_xempty__sub_2F4_240_6 = {
  (char *)"empty_sub/4$0",
  &predicate_splay__tree_xempty__sub_2F4_240_6
};

extern Const struct predicate predicate_splay__tree_xempty_4;
static Const struct predinfo predinfo_splay__tree_xempty_4 = {
  (char *)"empty",
  &predicate_splay__tree_xempty_4
};

extern Const struct predicate predicate_splay__tree_xempty_3;
static Const struct predinfo predinfo_splay__tree_xempty_3 = {
  (char *)"empty",
  &predicate_splay__tree_xempty_3
};

extern Const struct predicate predicate_splay__tree_xempty_2F3_240_3;
static Const struct predinfo predinfo_splay__tree_xempty_2F3_240_3 = {
  (char *)"empty/3$0",
  &predicate_splay__tree_xempty_2F3_240_3
};

extern Const struct predicate predicate_splay__tree_xdefault__comparator_1;
static Const struct predinfo predinfo_splay__tree_xdefault__comparator_1 = {
  (char *)"default_comparator",
  &predicate_splay__tree_xdefault__comparator_1
};

extern Const struct predicate predicate_splay__tree_xcreate_2;
static Const struct predinfo predinfo_splay__tree_xcreate_2 = {
  (char *)"create",
  &predicate_splay__tree_xcreate_2
};

extern Const struct predicate predicate_splay__tree_xcreate_1;
static Const struct predinfo predinfo_splay__tree_xcreate_1 = {
  (char *)"create",
  &predicate_splay__tree_xcreate_1
};

static Const struct predinfo * Const preds_in_splay__tree[] = {
  &predinfo_splay__tree_xupdate__right_2F11_240_15,
  &predinfo_splay__tree_xupdate__right_11,
  &predinfo_splay__tree_xupdate__left_2F11_240_15,
  &predinfo_splay__tree_xupdate__left_11,
  &predinfo_splay__tree_xupdate_8,
  &predinfo_splay__tree_xupdate_6,
  &predinfo_splay__tree_xupdate_5,
  &predinfo_splay__tree_xupdate_2F8_240_12,
  &predinfo_splay__tree_xreform__tree_3,
  &predinfo_splay__tree_xrefer_5,
  &predinfo_splay__tree_xput_4,
  &predinfo_splay__tree_xget__min__sub_4,
  &predinfo_splay__tree_xget__min_4,
  &predinfo_splay__tree_xget__max__sub_4,
  &predinfo_splay__tree_xget__max_4,
  &predinfo_splay__tree_xget__all__sub_3,
  &predinfo_splay__tree_xget__all_3,
  &predinfo_splay__tree_xget_5,
  &predinfo_splay__tree_xget_4,
  &predinfo_splay__tree_xget_2F5_241_3,
  &predinfo_splay__tree_xget_2F5_240_9,
  &predinfo_splay__tree_xempty__sub_4,
  &predinfo_splay__tree_xempty__sub_2F4_240_6,
  &predinfo_splay__tree_xempty_4,
  &predinfo_splay__tree_xempty_3,
  &predinfo_splay__tree_xempty_2F3_240_3,
  &predinfo_splay__tree_xdefault__comparator_1,
  &predinfo_splay__tree_xcreate_2,
  &predinfo_splay__tree_xcreate_1,
  0
};
extern Const struct predicate predicate_keyed__sorted__set_xmessage_3;
static Const struct predinfo predinfo_keyed__sorted__set_xmessage_3 = {
  (char *)"message",
  &predicate_keyed__sorted__set_xmessage_3
};

extern Const struct predicate predicate_keyed__sorted__set_xmessage_2F3_246_5;
static Const struct predinfo predinfo_keyed__sorted__set_xmessage_2F3_246_5 = {
  (char *)"message/3$6",
  &predicate_keyed__sorted__set_xmessage_2F3_246_5
};

extern Const struct predicate predicate_keyed__sorted__set_xmessage_2F3_245_5;
static Const struct predinfo predinfo_keyed__sorted__set_xmessage_2F3_245_5 = {
  (char *)"message/3$5",
  &predicate_keyed__sorted__set_xmessage_2F3_245_5
};

extern Const struct predicate predicate_keyed__sorted__set_xmessage_2F3_244_5;
static Const struct predinfo predinfo_keyed__sorted__set_xmessage_2F3_244_5 = {
  (char *)"message/3$4",
  &predicate_keyed__sorted__set_xmessage_2F3_244_5
};

extern Const struct predicate predicate_keyed__sorted__set_xmessage_2F3_243_6;
static Const struct predinfo predinfo_keyed__sorted__set_xmessage_2F3_243_6 = {
  (char *)"message/3$3",
  &predicate_keyed__sorted__set_xmessage_2F3_243_6
};

extern Const struct predicate predicate_keyed__sorted__set_xmessage_2F3_242_7;
static Const struct predinfo predinfo_keyed__sorted__set_xmessage_2F3_242_7 = {
  (char *)"message/3$2",
  &predicate_keyed__sorted__set_xmessage_2F3_242_7
};

extern Const struct predicate predicate_keyed__sorted__set_xmessage_2F3_241_4;
static Const struct predinfo predinfo_keyed__sorted__set_xmessage_2F3_241_4 = {
  (char *)"message/3$1",
  &predicate_keyed__sorted__set_xmessage_2F3_241_4
};

extern Const struct predicate predicate_keyed__sorted__set_xmessage_2F3_240_4;
static Const struct predinfo predinfo_keyed__sorted__set_xmessage_2F3_240_4 = {
  (char *)"message/3$0",
  &predicate_keyed__sorted__set_xmessage_2F3_240_4
};

extern Const struct predicate predicate_keyed__sorted__set_xmain_3;
static Const struct predinfo predinfo_keyed__sorted__set_xmain_3 = {
  (char *)"main",
  &predicate_keyed__sorted__set_xmain_3
};

extern Const struct predicate predicate_keyed__sorted__set_xcreate_2;
static Const struct predinfo predinfo_keyed__sorted__set_xcreate_2 = {
  (char *)"create",
  &predicate_keyed__sorted__set_xcreate_2
};

extern Const struct predicate predicate_keyed__sorted__set_xcreate_1;
static Const struct predinfo predinfo_keyed__sorted__set_xcreate_1 = {
  (char *)"create",
  &predicate_keyed__sorted__set_xcreate_1
};

extern Const struct predicate predicate_keyed__sorted__set_xconvert_2;
static Const struct predinfo predinfo_keyed__sorted__set_xconvert_2 = {
  (char *)"convert",
  &predicate_keyed__sorted__set_xconvert_2
};

static Const struct predinfo * Const preds_in_keyed__sorted__set[] = {
  &predinfo_keyed__sorted__set_xmessage_3,
  &predinfo_keyed__sorted__set_xmessage_2F3_246_5,
  &predinfo_keyed__sorted__set_xmessage_2F3_245_5,
  &predinfo_keyed__sorted__set_xmessage_2F3_244_5,
  &predinfo_keyed__sorted__set_xmessage_2F3_243_6,
  &predinfo_keyed__sorted__set_xmessage_2F3_242_7,
  &predinfo_keyed__sorted__set_xmessage_2F3_241_4,
  &predinfo_keyed__sorted__set_xmessage_2F3_240_4,
  &predinfo_keyed__sorted__set_xmain_3,
  &predinfo_keyed__sorted__set_xcreate_2,
  &predinfo_keyed__sorted__set_xcreate_1,
  &predinfo_keyed__sorted__set_xconvert_2,
  0
};
extern Const struct predicate predicate_klic__comp__extern_xmake__ext__table_3;
static Const struct predinfo predinfo_klic__comp__extern_xmake__ext__table_3 = {
  (char *)"make_ext_table",
  &predicate_klic__comp__extern_xmake__ext__table_3
};

extern Const struct predicate predicate_klic__comp__extern_xext__struct__vector_5;
static Const struct predinfo predinfo_klic__comp__extern_xext__struct__vector_5 = {
  (char *)"ext_struct_vector",
  &predicate_klic__comp__extern_xext__struct__vector_5
};

extern Const struct predicate predicate_klic__comp__extern_xext__struct__funct_5;
static Const struct predinfo predinfo_klic__comp__extern_xext__struct__funct_5 = {
  (char *)"ext_struct_funct",
  &predicate_klic__comp__extern_xext__struct__funct_5
};

extern Const struct predicate predicate_klic__comp__extern_xext__struct_3;
static Const struct predinfo predinfo_klic__comp__extern_xext__struct_3 = {
  (char *)"ext_struct",
  &predicate_klic__comp__extern_xext__struct_3
};

static Const struct predinfo * Const preds_in_klic__comp__extern[] = {
  &predinfo_klic__comp__extern_xmake__ext__table_3,
  &predinfo_klic__comp__extern_xext__struct__vector_5,
  &predinfo_klic__comp__extern_xext__struct__funct_5,
  &predinfo_klic__comp__extern_xext__struct_3,
  0
};
extern Const struct predicate predicate_klic__comp__write_xwrite__string__elements_5;
static Const struct predinfo predinfo_klic__comp__write_xwrite__string__elements_5 = {
  (char *)"write_string_elements",
  &predicate_klic__comp__write_xwrite__string__elements_5
};

extern Const struct predicate predicate_klic__comp__write_xwrite__string__element_3;
static Const struct predinfo predinfo_klic__comp__write_xwrite__string__element_3 = {
  (char *)"write_string_element",
  &predicate_klic__comp__write_xwrite__string__element_3
};

extern Const struct predicate predicate_klic__comp__write_xwrite__string__body_4;
static Const struct predinfo predinfo_klic__comp__write_xwrite__string__body_4 = {
  (char *)"write_string_body",
  &predicate_klic__comp__write_xwrite__string__body_4
};

extern Const struct predicate predicate_klic__comp__write_xwrite__predicates__1_6;
static Const struct predinfo predinfo_klic__comp__write_xwrite__predicates__1_6 = {
  (char *)"write_predicates_1",
  &predicate_klic__comp__write_xwrite__predicates__1_6
};

extern Const struct predicate predicate_klic__comp__write_xwrite__predicates_6;
static Const struct predinfo predinfo_klic__comp__write_xwrite__predicates_6 = {
  (char *)"write_predicates",
  &predicate_klic__comp__write_xwrite__predicates_6
};

extern Const struct predicate predicate_klic__comp__write_xwrite__pred__structs_4;
static Const struct predinfo predinfo_klic__comp__write_xwrite__pred__structs_4 = {
  (char *)"write_pred_structs",
  &predicate_klic__comp__write_xwrite__pred__structs_4
};

extern Const struct predicate predicate_klic__comp__write_xwrite__pred__name_5;
static Const struct predinfo predinfo_klic__comp__write_xwrite__pred__name_5 = {
  (char *)"write_pred_name",
  &predicate_klic__comp__write_xwrite__pred__name_5
};

extern Const struct predicate predicate_klic__comp__write_xwrite__pred__dispatch_3;
static Const struct predinfo predinfo_klic__comp__write_xwrite__pred__dispatch_3 = {
  (char *)"write_pred_dispatch",
  &predicate_klic__comp__write_xwrite__pred__dispatch_3
};

extern Const struct predicate predicate_klic__comp__write_xwrite__out_3;
static Const struct predinfo predinfo_klic__comp__write_xwrite__out_3 = {
  (char *)"write_out",
  &predicate_klic__comp__write_xwrite__out_3
};

extern Const struct predicate predicate_klic__comp__write_xwrite__interrupt__call_4;
static Const struct predinfo predinfo_klic__comp__write_xwrite__interrupt__call_4 = {
  (char *)"write_interrupt_call",
  &predicate_klic__comp__write_xwrite__interrupt__call_4
};

extern Const struct predicate predicate_klic__comp__write_xwrite__header__file_3;
static Const struct predinfo predinfo_klic__comp__write_xwrite__header__file_3 = {
  (char *)"write_header_file",
  &predicate_klic__comp__write_xwrite__header__file_3
};

extern Const struct predicate predicate_klic__comp__write_xwrite__get__args_4;
static Const struct predinfo predinfo_klic__comp__write_xwrite__get__args_4 = {
  (char *)"write_get_args",
  &predicate_klic__comp__write_xwrite__get__args_4
};

extern Const struct predicate predicate_klic__comp__write_xwrite__funct__name_4;
static Const struct predinfo predinfo_klic__comp__write_xwrite__funct__name_4 = {
  (char *)"write_funct_name",
  &predicate_klic__comp__write_xwrite__funct__name_4
};

extern Const struct predicate predicate_klic__comp__write_xwrite__ext__entry_3;
static Const struct predinfo predinfo_klic__comp__write_xwrite__ext__entry_3 = {
  (char *)"write_ext_entry",
  &predicate_klic__comp__write_xwrite__ext__entry_3
};

extern Const struct predicate predicate_klic__comp__write_xwrite__ext__entries_3;
static Const struct predinfo predinfo_klic__comp__write_xwrite__ext__entries_3 = {
  (char *)"write_ext_entries",
  &predicate_klic__comp__write_xwrite__ext__entries_3
};

extern Const struct predicate predicate_klic__comp__write_xwrite__defined__preds_4;
static Const struct predinfo predinfo_klic__comp__write_xwrite__defined__preds_4 = {
  (char *)"write_defined_preds",
  &predicate_klic__comp__write_xwrite__defined__preds_4
};

extern Const struct predicate predicate_klic__comp__write_xwrite__const__elems_3;
static Const struct predinfo predinfo_klic__comp__write_xwrite__const__elems_3 = {
  (char *)"write_const_elems",
  &predicate_klic__comp__write_xwrite__const__elems_3
};

extern Const struct predicate predicate_klic__comp__write_xwrite__const__elem_3;
static Const struct predinfo predinfo_klic__comp__write_xwrite__const__elem_3 = {
  (char *)"write_const_elem",
  &predicate_klic__comp__write_xwrite__const__elem_3
};

extern Const struct predicate predicate_klic__comp__write_xwrite__comma__list_5;
static Const struct predinfo predinfo_klic__comp__write_xwrite__comma__list_5 = {
  (char *)"write_comma_list",
  &predicate_klic__comp__write_xwrite__comma__list_5
};

extern Const struct predicate predicate_klic__comp__write_xwrite__comma__list1_5;
static Const struct predinfo predinfo_klic__comp__write_xwrite__comma__list1_5 = {
  (char *)"write_comma_list1",
  &predicate_klic__comp__write_xwrite__comma__list1_5
};

extern Const struct predicate predicate_klic__comp__write_xwrite__character__constants_5;
static Const struct predinfo predinfo_klic__comp__write_xwrite__character__constants_5 = {
  (char *)"write_character_constants",
  &predicate_klic__comp__write_xwrite__character__constants_5
};

extern Const struct predicate predicate_klic__comp__write_xwrite__atom_3;
static Const struct predinfo predinfo_klic__comp__write_xwrite__atom_3 = {
  (char *)"write_atom",
  &predicate_klic__comp__write_xwrite__atom_3
};

extern Const struct predicate predicate_klic__comp__write_xsort__info_3;
static Const struct predinfo predinfo_klic__comp__write_xsort__info_3 = {
  (char *)"sort_info",
  &predicate_klic__comp__write_xsort__info_3
};

extern Const struct predicate predicate_klic__comp__write_xsort__info_2;
static Const struct predinfo predinfo_klic__comp__write_xsort__info_2 = {
  (char *)"sort_info",
  &predicate_klic__comp__write_xsort__info_2
};

extern Const struct predicate predicate_klic__comp__write_xpred__min__max_5;
static Const struct predinfo predinfo_klic__comp__write_xpred__min__max_5 = {
  (char *)"pred_min_max",
  &predicate_klic__comp__write_xpred__min__max_5
};

extern Const struct predicate predicate_klic__comp__write_xpartition_4;
static Const struct predinfo predinfo_klic__comp__write_xpartition_4 = {
  (char *)"partition",
  &predicate_klic__comp__write_xpartition_4
};

extern Const struct predicate predicate_klic__comp__write_xdeclare__registers_4;
static Const struct predinfo predinfo_klic__comp__write_xdeclare__registers_4 = {
  (char *)"declare_registers",
  &predicate_klic__comp__write_xdeclare__registers_4
};

extern Const struct predicate predicate_klic__comp__write_xdeclare__one__external_3;
static Const struct predinfo predinfo_klic__comp__write_xdeclare__one__external_3 = {
  (char *)"declare_one_external",
  &predicate_klic__comp__write_xdeclare__one__external_3
};

extern Const struct predicate predicate_klic__comp__write_xdeclare__externals_3;
static Const struct predinfo predinfo_klic__comp__write_xdeclare__externals_3 = {
  (char *)"declare_externals",
  &predicate_klic__comp__write_xdeclare__externals_3
};

extern Const struct predicate predicate_klic__comp__write_xdeclare__const__list_6;
static Const struct predinfo predinfo_klic__comp__write_xdeclare__const__list_6 = {
  (char *)"declare_const_list",
  &predicate_klic__comp__write_xdeclare__const__list_6
};

extern Const struct predicate predicate_klic__comp__write_xdeclare__const_6;
static Const struct predinfo predinfo_klic__comp__write_xdeclare__const_6 = {
  (char *)"declare_const",
  &predicate_klic__comp__write_xdeclare__const_6
};

extern Const struct predicate predicate_klic__comp__write_xdeclare__const_5;
static Const struct predinfo predinfo_klic__comp__write_xdeclare__const_5 = {
  (char *)"declare_const",
  &predicate_klic__comp__write_xdeclare__const_5
};

static Const struct predinfo * Const preds_in_klic__comp__write[] = {
  &predinfo_klic__comp__write_xwrite__string__elements_5,
  &predinfo_klic__comp__write_xwrite__string__element_3,
  &predinfo_klic__comp__write_xwrite__string__body_4,
  &predinfo_klic__comp__write_xwrite__predicates__1_6,
  &predinfo_klic__comp__write_xwrite__predicates_6,
  &predinfo_klic__comp__write_xwrite__pred__structs_4,
  &predinfo_klic__comp__write_xwrite__pred__name_5,
  &predinfo_klic__comp__write_xwrite__pred__dispatch_3,
  &predinfo_klic__comp__write_xwrite__out_3,
  &predinfo_klic__comp__write_xwrite__interrupt__call_4,
  &predinfo_klic__comp__write_xwrite__header__file_3,
  &predinfo_klic__comp__write_xwrite__get__args_4,
  &predinfo_klic__comp__write_xwrite__funct__name_4,
  &predinfo_klic__comp__write_xwrite__ext__entry_3,
  &predinfo_klic__comp__write_xwrite__ext__entries_3,
  &predinfo_klic__comp__write_xwrite__defined__preds_4,
  &predinfo_klic__comp__write_xwrite__const__elems_3,
  &predinfo_klic__comp__write_xwrite__const__elem_3,
  &predinfo_klic__comp__write_xwrite__comma__list_5,
  &predinfo_klic__comp__write_xwrite__comma__list1_5,
  &predinfo_klic__comp__write_xwrite__character__constants_5,
  &predinfo_klic__comp__write_xwrite__atom_3,
  &predinfo_klic__comp__write_xsort__info_3,
  &predinfo_klic__comp__write_xsort__info_2,
  &predinfo_klic__comp__write_xpred__min__max_5,
  &predinfo_klic__comp__write_xpartition_4,
  &predinfo_klic__comp__write_xdeclare__registers_4,
  &predinfo_klic__comp__write_xdeclare__one__external_3,
  &predinfo_klic__comp__write_xdeclare__externals_3,
  &predinfo_klic__comp__write_xdeclare__const__list_6,
  &predinfo_klic__comp__write_xdeclare__const_6,
  &predinfo_klic__comp__write_xdeclare__const_5,
  0
};
extern Const struct predicate predicate_klic__comp__obj_xwrite__short_3;
static Const struct predinfo predinfo_klic__comp__obj_xwrite__short_3 = {
  (char *)"write_short",
  &predicate_klic__comp__obj_xwrite__short_3
};

extern Const struct predicate predicate_klic__comp__obj_xwrite__reg__int_3;
static Const struct predinfo predinfo_klic__comp__obj_xwrite__reg__int_3 = {
  (char *)"write_reg_int",
  &predicate_klic__comp__obj_xwrite__reg__int_3
};

extern Const struct predicate predicate_klic__comp__obj_xwrite__reg_3;
static Const struct predinfo predinfo_klic__comp__obj_xwrite__reg_3 = {
  (char *)"write_reg",
  &predicate_klic__comp__obj_xwrite__reg_3
};

extern Const struct predicate predicate_klic__comp__obj_xwrite__qp_3;
static Const struct predinfo predinfo_klic__comp__obj_xwrite__qp_3 = {
  (char *)"write_qp",
  &predicate_klic__comp__obj_xwrite__qp_3
};

extern Const struct predicate predicate_klic__comp__obj_xwrite__params__blt_3;
static Const struct predinfo predinfo_klic__comp__obj_xwrite__params__blt_3 = {
  (char *)"write_params_blt",
  &predicate_klic__comp__obj_xwrite__params__blt_3
};

extern Const struct predicate predicate_klic__comp__obj_xwrite__params_3;
static Const struct predinfo predinfo_klic__comp__obj_xwrite__params_3 = {
  (char *)"write_params",
  &predicate_klic__comp__obj_xwrite__params_3
};

extern Const struct predicate predicate_klic__comp__obj_xwrite__param__blt_3;
static Const struct predinfo predinfo_klic__comp__obj_xwrite__param__blt_3 = {
  (char *)"write_param_blt",
  &predicate_klic__comp__obj_xwrite__param__blt_3
};

extern Const struct predicate predicate_klic__comp__obj_xwrite__param_3;
static Const struct predinfo predinfo_klic__comp__obj_xwrite__param_3 = {
  (char *)"write_param",
  &predicate_klic__comp__obj_xwrite__param_3
};

extern Const struct predicate predicate_klic__comp__obj_xwrite__one__move_4;
static Const struct predinfo predinfo_klic__comp__obj_xwrite__one__move_4 = {
  (char *)"write_one_move",
  &predicate_klic__comp__obj_xwrite__one__move_4
};

extern Const struct predicate predicate_klic__comp__obj_xwrite__one__execute_5;
static Const struct predinfo predinfo_klic__comp__obj_xwrite__one__execute_5 = {
  (char *)"write_one_execute",
  &predicate_klic__comp__obj_xwrite__one__execute_5
};

extern Const struct predicate predicate_klic__comp__obj_xwrite__one__bblt_6;
static Const struct predinfo predinfo_klic__comp__obj_xwrite__one__bblt_6 = {
  (char *)"write_one_bblt",
  &predicate_klic__comp__obj_xwrite__one__bblt_6
};

extern Const struct predicate predicate_klic__comp__obj_xwrite__one_6;
static Const struct predinfo predinfo_klic__comp__obj_xwrite__one_6 = {
  (char *)"write_one",
  &predicate_klic__comp__obj_xwrite__one_6
};

extern Const struct predicate predicate_klic__comp__obj_xwrite__object_5;
static Const struct predinfo predinfo_klic__comp__obj_xwrite__object_5 = {
  (char *)"write_object",
  &predicate_klic__comp__obj_xwrite__object_5
};

extern Const struct predicate predicate_klic__comp__obj_xwrite__object_4;
static Const struct predinfo predinfo_klic__comp__obj_xwrite__object_4 = {
  (char *)"write_object",
  &predicate_klic__comp__obj_xwrite__object_4
};

extern Const struct predicate predicate_klic__comp__obj_xwrite__object_2F5_240_8;
static Const struct predinfo predinfo_klic__comp__obj_xwrite__object_2F5_240_8 = {
  (char *)"write_object/5$0",
  &predicate_klic__comp__obj_xwrite__object_2F5_240_8
};

extern Const struct predicate predicate_klic__comp__obj_xwrite__object_2F5_240_2F8_240_10;
static Const struct predinfo predinfo_klic__comp__obj_xwrite__object_2F5_240_2F8_240_10 = {
  (char *)"write_object/5$0/8$0",
  &predicate_klic__comp__obj_xwrite__object_2F5_240_2F8_240_10
};

extern Const struct predicate predicate_klic__comp__obj_xwrite__int_3;
static Const struct predinfo predinfo_klic__comp__obj_xwrite__int_3 = {
  (char *)"write_int",
  &predicate_klic__comp__obj_xwrite__int_3
};

extern Const struct predicate predicate_klic__comp__obj_xwrite__inline_8;
static Const struct predinfo predinfo_klic__comp__obj_xwrite__inline_8 = {
  (char *)"write_inline",
  &predicate_klic__comp__obj_xwrite__inline_8
};

extern Const struct predicate predicate_klic__comp__obj_xwrite__inline_6;
static Const struct predinfo predinfo_klic__comp__obj_xwrite__inline_6 = {
  (char *)"write_inline",
  &predicate_klic__comp__obj_xwrite__inline_6
};

extern Const struct predicate predicate_klic__comp__obj_xwrite__elem__access_4;
static Const struct predinfo predinfo_klic__comp__obj_xwrite__elem__access_4 = {
  (char *)"write_elem_access",
  &predicate_klic__comp__obj_xwrite__elem__access_4
};

extern Const struct predicate predicate_klic__comp__obj_xwrite__atom__name__string_5;
static Const struct predinfo predinfo_klic__comp__obj_xwrite__atom__name__string_5 = {
  (char *)"write_atom_name_string",
  &predicate_klic__comp__obj_xwrite__atom__name__string_5
};

extern Const struct predicate predicate_klic__comp__obj_xwrite__atom__name__string_3;
static Const struct predinfo predinfo_klic__comp__obj_xwrite__atom__name__string_3 = {
  (char *)"write_atom_name_string",
  &predicate_klic__comp__obj_xwrite__atom__name__string_3
};

extern Const struct predicate predicate_klic__comp__obj_xwrite__atom__name__char_3;
static Const struct predinfo predinfo_klic__comp__obj_xwrite__atom__name__char_3 = {
  (char *)"write_atom_name_char",
  &predicate_klic__comp__obj_xwrite__atom__name__char_3
};

extern Const struct predicate predicate_klic__comp__obj_xwrite__adjust__qp_3;
static Const struct predinfo predinfo_klic__comp__obj_xwrite__adjust__qp_3 = {
  (char *)"write_adjust_qp",
  &predicate_klic__comp__obj_xwrite__adjust__qp_3
};

extern Const struct predicate predicate_klic__comp__obj_xwrite__adjust__allocp_3;
static Const struct predinfo predinfo_klic__comp__obj_xwrite__adjust__allocp_3 = {
  (char *)"write_adjust_allocp",
  &predicate_klic__comp__obj_xwrite__adjust__allocp_3
};

extern Const struct predicate predicate_klic__comp__obj_xwrite__adjust_4;
static Const struct predinfo predinfo_klic__comp__obj_xwrite__adjust_4 = {
  (char *)"write_adjust",
  &predicate_klic__comp__obj_xwrite__adjust_4
};

extern Const struct predicate predicate_klic__comp__obj_xwait__and__out_4;
static Const struct predinfo predinfo_klic__comp__obj_xwait__and__out_4 = {
  (char *)"wait_and_out",
  &predicate_klic__comp__obj_xwait__and__out_4
};

extern Const struct predicate predicate_klic__comp__obj_xklicformat__stderr_2;
static Const struct predinfo predinfo_klic__comp__obj_xklicformat__stderr_2 = {
  (char *)"klicformat_stderr",
  &predicate_klic__comp__obj_xklicformat__stderr_2
};

extern Const struct predicate predicate_klic__comp__obj_xklicformat_6;
static Const struct predinfo predinfo_klic__comp__obj_xklicformat_6 = {
  (char *)"klicformat",
  &predicate_klic__comp__obj_xklicformat_6
};

extern Const struct predicate predicate_klic__comp__obj_xklicformat_4;
static Const struct predinfo predinfo_klic__comp__obj_xklicformat_4 = {
  (char *)"klicformat",
  &predicate_klic__comp__obj_xklicformat_4
};

extern Const struct predicate predicate_klic__comp__obj_xklicformat_3;
static Const struct predinfo predinfo_klic__comp__obj_xklicformat_3 = {
  (char *)"klicformat",
  &predicate_klic__comp__obj_xklicformat_3
};

extern Const struct predicate predicate_klic__comp__obj_xinline__special_6;
static Const struct predinfo predinfo_klic__comp__obj_xinline__special_6 = {
  (char *)"inline_special",
  &predicate_klic__comp__obj_xinline__special_6
};

extern Const struct predicate predicate_klic__comp__obj_xhexchar_2;
static Const struct predinfo predinfo_klic__comp__obj_xhexchar_2 = {
  (char *)"hexchar",
  &predicate_klic__comp__obj_xhexchar_2
};

extern Const struct predicate predicate_klic__comp__obj_xformat__special__w_3;
static Const struct predinfo predinfo_klic__comp__obj_xformat__special__w_3 = {
  (char *)"format_special_w",
  &predicate_klic__comp__obj_xformat__special__w_3
};

extern Const struct predicate predicate_klic__comp__obj_xformat__special__i_3;
static Const struct predinfo predinfo_klic__comp__obj_xformat__special__i_3 = {
  (char *)"format_special_i",
  &predicate_klic__comp__obj_xformat__special__i_3
};

extern Const struct predicate predicate_klic__comp__obj_xformat__special__W_3;
static Const struct predinfo predinfo_klic__comp__obj_xformat__special__W_3 = {
  (char *)"format_special_W",
  &predicate_klic__comp__obj_xformat__special__W_3
};

extern Const struct predicate predicate_klic__comp__obj_xformat__special__L_3;
static Const struct predinfo predinfo_klic__comp__obj_xformat__special__L_3 = {
  (char *)"format_special_L",
  &predicate_klic__comp__obj_xformat__special__L_3
};

extern Const struct predicate predicate_klic__comp__obj_xformat__special__D__1_4;
static Const struct predinfo predinfo_klic__comp__obj_xformat__special__D__1_4 = {
  (char *)"format_special_D_1",
  &predicate_klic__comp__obj_xformat__special__D__1_4
};

extern Const struct predicate predicate_klic__comp__obj_xformat__special__D_4;
static Const struct predinfo predinfo_klic__comp__obj_xformat__special__D_4 = {
  (char *)"format_special_D",
  &predicate_klic__comp__obj_xformat__special__D_4
};

extern Const struct predicate predicate_klic__comp__obj_xformat__special_5;
static Const struct predinfo predinfo_klic__comp__obj_xformat__special_5 = {
  (char *)"format_special",
  &predicate_klic__comp__obj_xformat__special_5
};

extern Const struct predicate predicate_klic__comp__obj_xflush_3;
static Const struct predinfo predinfo_klic__comp__obj_xflush_3 = {
  (char *)"flush",
  &predicate_klic__comp__obj_xflush_3
};

extern Const struct predicate predicate_klic__comp__obj_xflush_2;
static Const struct predinfo predinfo_klic__comp__obj_xflush_2 = {
  (char *)"flush",
  &predicate_klic__comp__obj_xflush_2
};

static Const struct predinfo * Const preds_in_klic__comp__obj[] = {
  &predinfo_klic__comp__obj_xwrite__short_3,
  &predinfo_klic__comp__obj_xwrite__reg__int_3,
  &predinfo_klic__comp__obj_xwrite__reg_3,
  &predinfo_klic__comp__obj_xwrite__qp_3,
  &predinfo_klic__comp__obj_xwrite__params__blt_3,
  &predinfo_klic__comp__obj_xwrite__params_3,
  &predinfo_klic__comp__obj_xwrite__param__blt_3,
  &predinfo_klic__comp__obj_xwrite__param_3,
  &predinfo_klic__comp__obj_xwrite__one__move_4,
  &predinfo_klic__comp__obj_xwrite__one__execute_5,
  &predinfo_klic__comp__obj_xwrite__one__bblt_6,
  &predinfo_klic__comp__obj_xwrite__one_6,
  &predinfo_klic__comp__obj_xwrite__object_5,
  &predinfo_klic__comp__obj_xwrite__object_4,
  &predinfo_klic__comp__obj_xwrite__object_2F5_240_8,
  &predinfo_klic__comp__obj_xwrite__object_2F5_240_2F8_240_10,
  &predinfo_klic__comp__obj_xwrite__int_3,
  &predinfo_klic__comp__obj_xwrite__inline_8,
  &predinfo_klic__comp__obj_xwrite__inline_6,
  &predinfo_klic__comp__obj_xwrite__elem__access_4,
  &predinfo_klic__comp__obj_xwrite__atom__name__string_5,
  &predinfo_klic__comp__obj_xwrite__atom__name__string_3,
  &predinfo_klic__comp__obj_xwrite__atom__name__char_3,
  &predinfo_klic__comp__obj_xwrite__adjust__qp_3,
  &predinfo_klic__comp__obj_xwrite__adjust__allocp_3,
  &predinfo_klic__comp__obj_xwrite__adjust_4,
  &predinfo_klic__comp__obj_xwait__and__out_4,
  &predinfo_klic__comp__obj_xklicformat__stderr_2,
  &predinfo_klic__comp__obj_xklicformat_6,
  &predinfo_klic__comp__obj_xklicformat_4,
  &predinfo_klic__comp__obj_xklicformat_3,
  &predinfo_klic__comp__obj_xinline__special_6,
  &predinfo_klic__comp__obj_xhexchar_2,
  &predinfo_klic__comp__obj_xformat__special__w_3,
  &predinfo_klic__comp__obj_xformat__special__i_3,
  &predinfo_klic__comp__obj_xformat__special__W_3,
  &predinfo_klic__comp__obj_xformat__special__L_3,
  &predinfo_klic__comp__obj_xformat__special__D__1_4,
  &predinfo_klic__comp__obj_xformat__special__D_4,
  &predinfo_klic__comp__obj_xformat__special_5,
  &predinfo_klic__comp__obj_xflush_3,
  &predinfo_klic__comp__obj_xflush_2,
  0
};
extern Const struct predicate predicate_klic__comp__version_xcompiler__version_1;
static Const struct predinfo predinfo_klic__comp__version_xcompiler__version_1 = {
  (char *)"compiler_version",
  &predicate_klic__comp__version_xcompiler__version_1
};

extern Const struct predicate predicate_klic__comp__version_xcompiler__date_1;
static Const struct predinfo predinfo_klic__comp__version_xcompiler__date_1 = {
  (char *)"compiler_date",
  &predicate_klic__comp__version_xcompiler__date_1
};

static Const struct predinfo * Const preds_in_klic__comp__version[] = {
  &predinfo_klic__comp__version_xcompiler__version_1,
  &predinfo_klic__comp__version_xcompiler__date_1,
  0
};
extern Const struct predicate predicate_klic__comp__macro_xupdate__info__1_9;
static Const struct predinfo predinfo_klic__comp__macro_xupdate__info__1_9 = {
  (char *)"update_info_1",
  &predicate_klic__comp__macro_xupdate__info__1_9
};

extern Const struct predicate predicate_klic__comp__macro_xupdate__info_6;
static Const struct predinfo predinfo_klic__comp__macro_xupdate__info_6 = {
  (char *)"update_info",
  &predicate_klic__comp__macro_xupdate__info_6
};

extern Const struct predicate predicate_klic__comp__macro_xreplace__vars__elems_3;
static Const struct predinfo predinfo_klic__comp__macro_xreplace__vars__elems_3 = {
  (char *)"replace_vars_elems",
  &predicate_klic__comp__macro_xreplace__vars__elems_3
};

extern Const struct predicate predicate_klic__comp__macro_xreplace__vars__1_3;
static Const struct predinfo predinfo_klic__comp__macro_xreplace__vars__1_3 = {
  (char *)"replace_vars_1",
  &predicate_klic__comp__macro_xreplace__vars__1_3
};

extern Const struct predicate predicate_klic__comp__macro_xreplace__vars_3;
static Const struct predinfo predinfo_klic__comp__macro_xreplace__vars_3 = {
  (char *)"replace_vars",
  &predicate_klic__comp__macro_xreplace__vars_3
};

extern Const struct predicate predicate_klic__comp__macro_xmerge__vinfo_5;
static Const struct predinfo predinfo_klic__comp__macro_xmerge__vinfo_5 = {
  (char *)"merge_vinfo",
  &predicate_klic__comp__macro_xmerge__vinfo_5
};

extern Const struct predicate predicate_klic__comp__macro_xmake__expanded__call_2;
static Const struct predinfo predinfo_klic__comp__macro_xmake__expanded__call_2 = {
  (char *)"make_expanded_call",
  &predicate_klic__comp__macro_xmake__expanded__call_2
};

extern Const struct predicate predicate_klic__comp__macro_xmacro__vector__elems_10;
static Const struct predinfo predinfo_klic__comp__macro_xmacro__vector__elems_10 = {
  (char *)"macro_vector_elems",
  &predicate_klic__comp__macro_xmacro__vector__elems_10
};

extern Const struct predicate predicate_klic__comp__macro_xmacro__vec__elems_8;
static Const struct predinfo predinfo_klic__comp__macro_xmacro__vec__elems_8 = {
  (char *)"macro_vec_elems",
  &predicate_klic__comp__macro_xmacro__vec__elems_8
};

extern Const struct predicate predicate_klic__comp__macro_xmacro__goal__functor__cmp_9;
static Const struct predinfo predinfo_klic__comp__macro_xmacro__goal__functor__cmp_9 = {
  (char *)"macro_goal_functor_cmp",
  &predicate_klic__comp__macro_xmacro__goal__functor__cmp_9
};

extern Const struct predicate predicate_klic__comp__macro_xmacro__goal__functor_8;
static Const struct predinfo predinfo_klic__comp__macro_xmacro__goal__functor_8 = {
  (char *)"macro_goal_functor",
  &predicate_klic__comp__macro_xmacro__goal__functor_8
};

extern Const struct predicate predicate_klic__comp__macro_xmacro__goal_8;
static Const struct predinfo predinfo_klic__comp__macro_xmacro__goal_8 = {
  (char *)"macro_goal",
  &predicate_klic__comp__macro_xmacro__goal_8
};

extern Const struct predicate predicate_klic__comp__macro_xmacro__float__op_2;
static Const struct predinfo predinfo_klic__comp__macro_xmacro__float__op_2 = {
  (char *)"macro_float_op",
  &predicate_klic__comp__macro_xmacro__float__op_2
};

extern Const struct predicate predicate_klic__comp__macro_xmacro__float__exp__1_7;
static Const struct predinfo predinfo_klic__comp__macro_xmacro__float__exp__1_7 = {
  (char *)"macro_float_exp_1",
  &predicate_klic__comp__macro_xmacro__float__exp__1_7
};

extern Const struct predicate predicate_klic__comp__macro_xmacro__float__exp_6;
static Const struct predinfo predinfo_klic__comp__macro_xmacro__float__exp_6 = {
  (char *)"macro_float_exp",
  &predicate_klic__comp__macro_xmacro__float__exp_6
};

extern Const struct predicate predicate_klic__comp__macro_xmacro__float__args_7;
static Const struct predinfo predinfo_klic__comp__macro_xmacro__float__args_7 = {
  (char *)"macro_float_args",
  &predicate_klic__comp__macro_xmacro__float__args_7
};

extern Const struct predicate predicate_klic__comp__macro_xmacro__exp__args_7;
static Const struct predinfo predinfo_klic__comp__macro_xmacro__exp__args_7 = {
  (char *)"macro_exp_args",
  &predicate_klic__comp__macro_xmacro__exp__args_7
};

extern Const struct predicate predicate_klic__comp__macro_xmacro__exp__1_7;
static Const struct predinfo predinfo_klic__comp__macro_xmacro__exp__1_7 = {
  (char *)"macro_exp_1",
  &predicate_klic__comp__macro_xmacro__exp__1_7
};

extern Const struct predicate predicate_klic__comp__macro_xmacro__exp_6;
static Const struct predinfo predinfo_klic__comp__macro_xmacro__exp_6 = {
  (char *)"macro_exp",
  &predicate_klic__comp__macro_xmacro__exp_6
};

extern Const struct predicate predicate_klic__comp__macro_xmacro__arith__op_2;
static Const struct predinfo predinfo_klic__comp__macro_xmacro__arith__op_2 = {
  (char *)"macro_arith_op",
  &predicate_klic__comp__macro_xmacro__arith__op_2
};

extern Const struct predicate predicate_klic__comp__macro_xmacro__args_9;
static Const struct predinfo predinfo_klic__comp__macro_xmacro__args_9 = {
  (char *)"macro_args",
  &predicate_klic__comp__macro_xmacro__args_9
};

extern Const struct predicate predicate_klic__comp__macro_xmacro__arg__n_8;
static Const struct predinfo predinfo_klic__comp__macro_xmacro__arg__n_8 = {
  (char *)"macro_arg_n",
  &predicate_klic__comp__macro_xmacro__arg__n_8
};

extern Const struct predicate predicate_klic__comp__macro_xmacro__arg__functor__sharp_10;
static Const struct predinfo predinfo_klic__comp__macro_xmacro__arg__functor__sharp_10 = {
  (char *)"macro_arg_functor_sharp",
  &predicate_klic__comp__macro_xmacro__arg__functor__sharp_10
};

extern Const struct predicate predicate_klic__comp__macro_xmacro__arg__functor_9;
static Const struct predinfo predinfo_klic__comp__macro_xmacro__arg__functor_9 = {
  (char *)"macro_arg_functor",
  &predicate_klic__comp__macro_xmacro__arg__functor_9
};

extern Const struct predicate predicate_klic__comp__macro_xmacro__arg_9;
static Const struct predinfo predinfo_klic__comp__macro_xmacro__arg_9 = {
  (char *)"macro_arg",
  &predicate_klic__comp__macro_xmacro__arg_9
};

extern Const struct predicate predicate_klic__comp__macro_xmacro__N__10_4;
static Const struct predinfo predinfo_klic__comp__macro_xmacro__N__10_4 = {
  (char *)"macro_N_10",
  &predicate_klic__comp__macro_xmacro__N__10_4
};

extern Const struct predicate predicate_klic__comp__macro_xmacro__1_4;
static Const struct predinfo predinfo_klic__comp__macro_xmacro__1_4 = {
  (char *)"macro_1",
  &predicate_klic__comp__macro_xmacro__1_4
};

extern Const struct predicate predicate_klic__comp__macro_xmacro_3;
static Const struct predinfo predinfo_klic__comp__macro_xmacro_3 = {
  (char *)"macro",
  &predicate_klic__comp__macro_xmacro_3
};

extern Const struct predicate predicate_klic__comp__macro_xmacro_2;
static Const struct predinfo predinfo_klic__comp__macro_xmacro_2 = {
  (char *)"macro",
  &predicate_klic__comp__macro_xmacro_2
};

extern Const struct predicate predicate_klic__comp__macro_xlinearize__guard_3;
static Const struct predinfo predinfo_klic__comp__macro_xlinearize__guard_3 = {
  (char *)"linearize_guard",
  &predicate_klic__comp__macro_xlinearize__guard_3
};

extern Const struct predicate predicate_klic__comp__macro_xinitialize__info__1_10;
static Const struct predinfo predinfo_klic__comp__macro_xinitialize__info__1_10 = {
  (char *)"initialize_info_1",
  &predicate_klic__comp__macro_xinitialize__info__1_10
};

extern Const struct predicate predicate_klic__comp__macro_xinitialize__info_7;
static Const struct predinfo predinfo_klic__comp__macro_xinitialize__info_7 = {
  (char *)"initialize_info",
  &predicate_klic__comp__macro_xinitialize__info_7
};

extern Const struct predicate predicate_klic__comp__macro_xfindvars__elems_3;
static Const struct predinfo predinfo_klic__comp__macro_xfindvars__elems_3 = {
  (char *)"findvars_elems",
  &predicate_klic__comp__macro_xfindvars__elems_3
};

extern Const struct predicate predicate_klic__comp__macro_xfindvars_3;
static Const struct predinfo predinfo_klic__comp__macro_xfindvars_3 = {
  (char *)"findvars",
  &predicate_klic__comp__macro_xfindvars_3
};

extern Const struct predicate predicate_klic__comp__macro_xfind__interface__vars__1_6;
static Const struct predinfo predinfo_klic__comp__macro_xfind__interface__vars__1_6 = {
  (char *)"find_interface_vars_1",
  &predicate_klic__comp__macro_xfind__interface__vars__1_6
};

extern Const struct predicate predicate_klic__comp__macro_xfind__interface__vars_3;
static Const struct predinfo predinfo_klic__comp__macro_xfind__interface__vars_3 = {
  (char *)"find_interface_vars",
  &predicate_klic__comp__macro_xfind__interface__vars_3
};

extern Const struct predicate predicate_klic__comp__macro_xfinalize__vars_4;
static Const struct predinfo predinfo_klic__comp__macro_xfinalize__vars_4 = {
  (char *)"finalize_vars",
  &predicate_klic__comp__macro_xfinalize__vars_4
};

extern Const struct predicate predicate_klic__comp__macro_xexpand__goal_3;
static Const struct predinfo predinfo_klic__comp__macro_xexpand__goal_3 = {
  (char *)"expand_goal",
  &predicate_klic__comp__macro_xexpand__goal_3
};

extern Const struct predicate predicate_klic__comp__macro_xexp__one__clause_5;
static Const struct predinfo predinfo_klic__comp__macro_xexp__one__clause_5 = {
  (char *)"exp_one_clause",
  &predicate_klic__comp__macro_xexp__one__clause_5
};

extern Const struct predicate predicate_klic__comp__macro_xexp__head__and__guard__top_6;
static Const struct predinfo predinfo_klic__comp__macro_xexp__head__and__guard__top_6 = {
  (char *)"exp_head_and_guard_top",
  &predicate_klic__comp__macro_xexp__head__and__guard__top_6
};

extern Const struct predicate predicate_klic__comp__macro_xexp__head_6;
static Const struct predinfo predinfo_klic__comp__macro_xexp__head_6 = {
  (char *)"exp_head",
  &predicate_klic__comp__macro_xexp__head_6
};

extern Const struct predicate predicate_klic__comp__macro_xexp__guard_5;
static Const struct predinfo predinfo_klic__comp__macro_xexp__guard_5 = {
  (char *)"exp_guard",
  &predicate_klic__comp__macro_xexp__guard_5
};

extern Const struct predicate predicate_klic__comp__macro_xexp__body__or_6;
static Const struct predinfo predinfo_klic__comp__macro_xexp__body__or_6 = {
  (char *)"exp_body_or",
  &predicate_klic__comp__macro_xexp__body__or_6
};

extern Const struct predicate predicate_klic__comp__macro_xexp__body_9;
static Const struct predinfo predinfo_klic__comp__macro_xexp__body_9 = {
  (char *)"exp_body",
  &predicate_klic__comp__macro_xexp__body_9
};

extern Const struct predicate predicate_klic__comp__macro_xdlor_8;
static Const struct predinfo predinfo_klic__comp__macro_xdlor_8 = {
  (char *)"dlor",
  &predicate_klic__comp__macro_xdlor_8
};

extern Const struct predicate predicate_klic__comp__macro_xdlgoals_7;
static Const struct predinfo predinfo_klic__comp__macro_xdlgoals_7 = {
  (char *)"dlgoals",
  &predicate_klic__comp__macro_xdlgoals_7
};

extern Const struct predicate predicate_klic__comp__macro_xdlgoal__atom_3;
static Const struct predinfo predinfo_klic__comp__macro_xdlgoal__atom_3 = {
  (char *)"dlgoal_atom",
  &predicate_klic__comp__macro_xdlgoal__atom_3
};

extern Const struct predicate predicate_klic__comp__macro_xdlgoal_7;
static Const struct predinfo predinfo_klic__comp__macro_xdlgoal_7 = {
  (char *)"dlgoal",
  &predicate_klic__comp__macro_xdlgoal_7
};

extern Const struct predicate predicate_klic__comp__macro_xdlclause__1_9;
static Const struct predinfo predinfo_klic__comp__macro_xdlclause__1_9 = {
  (char *)"dlclause_1",
  &predicate_klic__comp__macro_xdlclause__1_9
};

extern Const struct predicate predicate_klic__comp__macro_xdlclause_9;
static Const struct predinfo predinfo_klic__comp__macro_xdlclause_9 = {
  (char *)"dlclause",
  &predicate_klic__comp__macro_xdlclause_9
};

extern Const struct predicate predicate_klic__comp__macro_xdlappend__args_3;
static Const struct predinfo predinfo_klic__comp__macro_xdlappend__args_3 = {
  (char *)"dlappend_args",
  &predicate_klic__comp__macro_xdlappend__args_3
};

extern Const struct predicate predicate_klic__comp__macro_xdlappend__args_2F3_240_4;
static Const struct predinfo predinfo_klic__comp__macro_xdlappend__args_2F3_240_4 = {
  (char *)"dlappend_args/3$0",
  &predicate_klic__comp__macro_xdlappend__args_2F3_240_4
};

extern Const struct predicate predicate_klic__comp__macro_xdl__update_12;
static Const struct predinfo predinfo_klic__comp__macro_xdl__update_12 = {
  (char *)"dl_update",
  &predicate_klic__comp__macro_xdl__update_12
};

extern Const struct predicate predicate_klic__comp__macro_xdl__compute_9;
static Const struct predinfo predinfo_klic__comp__macro_xdl__compute_9 = {
  (char *)"dl_compute",
  &predicate_klic__comp__macro_xdl__compute_9
};

extern Const struct predicate predicate_klic__comp__macro_xdl__common_9;
static Const struct predinfo predinfo_klic__comp__macro_xdl__common_9 = {
  (char *)"dl_common",
  &predicate_klic__comp__macro_xdl__common_9
};

extern Const struct predicate predicate_klic__comp__macro_xcountvars_3;
static Const struct predinfo predinfo_klic__comp__macro_xcountvars_3 = {
  (char *)"countvars",
  &predicate_klic__comp__macro_xcountvars_3
};

extern Const struct predicate predicate_klic__comp__macro_xcountvars_2;
static Const struct predinfo predinfo_klic__comp__macro_xcountvars_2 = {
  (char *)"countvars",
  &predicate_klic__comp__macro_xcountvars_2
};

extern Const struct predicate predicate_klic__comp__macro_xcountsame_5;
static Const struct predinfo predinfo_klic__comp__macro_xcountsame_5 = {
  (char *)"countsame",
  &predicate_klic__comp__macro_xcountsame_5
};

extern Const struct predicate predicate_klic__comp__macro_xcompar__macro_3;
static Const struct predinfo predinfo_klic__comp__macro_xcompar__macro_3 = {
  (char *)"compar_macro",
  &predicate_klic__comp__macro_xcompar__macro_3
};

extern Const struct predicate predicate_klic__comp__macro_xbody__or__one_6;
static Const struct predinfo predinfo_klic__comp__macro_xbody__or__one_6 = {
  (char *)"body_or_one",
  &predicate_klic__comp__macro_xbody__or__one_6
};

extern Const struct predicate predicate_klic__comp__macro_xbody__or__clauses_6;
static Const struct predinfo predinfo_klic__comp__macro_xbody__or__clauses_6 = {
  (char *)"body_or_clauses",
  &predicate_klic__comp__macro_xbody__or__clauses_6
};

static Const struct predinfo * Const preds_in_klic__comp__macro[] = {
  &predinfo_klic__comp__macro_xupdate__info__1_9,
  &predinfo_klic__comp__macro_xupdate__info_6,
  &predinfo_klic__comp__macro_xreplace__vars__elems_3,
  &predinfo_klic__comp__macro_xreplace__vars__1_3,
  &predinfo_klic__comp__macro_xreplace__vars_3,
  &predinfo_klic__comp__macro_xmerge__vinfo_5,
  &predinfo_klic__comp__macro_xmake__expanded__call_2,
  &predinfo_klic__comp__macro_xmacro__vector__elems_10,
  &predinfo_klic__comp__macro_xmacro__vec__elems_8,
  &predinfo_klic__comp__macro_xmacro__goal__functor__cmp_9,
  &predinfo_klic__comp__macro_xmacro__goal__functor_8,
  &predinfo_klic__comp__macro_xmacro__goal_8,
  &predinfo_klic__comp__macro_xmacro__float__op_2,
  &predinfo_klic__comp__macro_xmacro__float__exp__1_7,
  &predinfo_klic__comp__macro_xmacro__float__exp_6,
  &predinfo_klic__comp__macro_xmacro__float__args_7,
  &predinfo_klic__comp__macro_xmacro__exp__args_7,
  &predinfo_klic__comp__macro_xmacro__exp__1_7,
  &predinfo_klic__comp__macro_xmacro__exp_6,
  &predinfo_klic__comp__macro_xmacro__arith__op_2,
  &predinfo_klic__comp__macro_xmacro__args_9,
  &predinfo_klic__comp__macro_xmacro__arg__n_8,
  &predinfo_klic__comp__macro_xmacro__arg__functor__sharp_10,
  &predinfo_klic__comp__macro_xmacro__arg__functor_9,
  &predinfo_klic__comp__macro_xmacro__arg_9,
  &predinfo_klic__comp__macro_xmacro__N__10_4,
  &predinfo_klic__comp__macro_xmacro__1_4,
  &predinfo_klic__comp__macro_xmacro_3,
  &predinfo_klic__comp__macro_xmacro_2,
  &predinfo_klic__comp__macro_xlinearize__guard_3,
  &predinfo_klic__comp__macro_xinitialize__info__1_10,
  &predinfo_klic__comp__macro_xinitialize__info_7,
  &predinfo_klic__comp__macro_xfindvars__elems_3,
  &predinfo_klic__comp__macro_xfindvars_3,
  &predinfo_klic__comp__macro_xfind__interface__vars__1_6,
  &predinfo_klic__comp__macro_xfind__interface__vars_3,
  &predinfo_klic__comp__macro_xfinalize__vars_4,
  &predinfo_klic__comp__macro_xexpand__goal_3,
  &predinfo_klic__comp__macro_xexp__one__clause_5,
  &predinfo_klic__comp__macro_xexp__head__and__guard__top_6,
  &predinfo_klic__comp__macro_xexp__head_6,
  &predinfo_klic__comp__macro_xexp__guard_5,
  &predinfo_klic__comp__macro_xexp__body__or_6,
  &predinfo_klic__comp__macro_xexp__body_9,
  &predinfo_klic__comp__macro_xdlor_8,
  &predinfo_klic__comp__macro_xdlgoals_7,
  &predinfo_klic__comp__macro_xdlgoal__atom_3,
  &predinfo_klic__comp__macro_xdlgoal_7,
  &predinfo_klic__comp__macro_xdlclause__1_9,
  &predinfo_klic__comp__macro_xdlclause_9,
  &predinfo_klic__comp__macro_xdlappend__args_3,
  &predinfo_klic__comp__macro_xdlappend__args_2F3_240_4,
  &predinfo_klic__comp__macro_xdl__update_12,
  &predinfo_klic__comp__macro_xdl__compute_9,
  &predinfo_klic__comp__macro_xdl__common_9,
  &predinfo_klic__comp__macro_xcountvars_3,
  &predinfo_klic__comp__macro_xcountvars_2,
  &predinfo_klic__comp__macro_xcountsame_5,
  &predinfo_klic__comp__macro_xcompar__macro_3,
  &predinfo_klic__comp__macro_xbody__or__one_6,
  &predinfo_klic__comp__macro_xbody__or__clauses_6,
  0
};
extern Const struct predicate predicate_klic__comp__cmacro_xreplace_7;
static Const struct predinfo predinfo_klic__comp__cmacro_xreplace_7 = {
  (char *)"replace",
  &predicate_klic__comp__cmacro_xreplace_7
};

extern Const struct predicate predicate_klic__comp__cmacro_xreplace_6;
static Const struct predinfo predinfo_klic__comp__cmacro_xreplace_6 = {
  (char *)"replace",
  &predicate_klic__comp__cmacro_xreplace_6
};

extern Const struct predicate predicate_klic__comp__cmacro_xreplace_5;
static Const struct predinfo predinfo_klic__comp__cmacro_xreplace_5 = {
  (char *)"replace",
  &predicate_klic__comp__cmacro_xreplace_5
};

extern Const struct predicate predicate_klic__comp__cmacro_xreplaceHead_5;
static Const struct predinfo predinfo_klic__comp__cmacro_xreplaceHead_5 = {
  (char *)"replaceHead",
  &predicate_klic__comp__cmacro_xreplaceHead_5
};

extern Const struct predicate predicate_klic__comp__cmacro_xreplaceGoals_7;
static Const struct predinfo predinfo_klic__comp__cmacro_xreplaceGoals_7 = {
  (char *)"replaceGoals",
  &predicate_klic__comp__cmacro_xreplaceGoals_7
};

extern Const struct predicate predicate_klic__comp__cmacro_xreplaceGoals_5;
static Const struct predinfo predinfo_klic__comp__cmacro_xreplaceGoals_5 = {
  (char *)"replaceGoals",
  &predicate_klic__comp__cmacro_xreplaceGoals_5
};

extern Const struct predicate predicate_klic__comp__cmacro_xreplaceClause_4;
static Const struct predinfo predinfo_klic__comp__cmacro_xreplaceClause_4 = {
  (char *)"replaceClause",
  &predicate_klic__comp__cmacro_xreplaceClause_4
};

extern Const struct predicate predicate_klic__comp__cmacro_xreconstGoals_2;
static Const struct predinfo predinfo_klic__comp__cmacro_xreconstGoals_2 = {
  (char *)"reconstGoals",
  &predicate_klic__comp__cmacro_xreconstGoals_2
};

extern Const struct predicate predicate_klic__comp__cmacro_xinsDefs_3;
static Const struct predinfo predinfo_klic__comp__cmacro_xinsDefs_3 = {
  (char *)"insDefs",
  &predicate_klic__comp__cmacro_xinsDefs_3
};

extern Const struct predicate predicate_klic__comp__cmacro_xexpand_4;
static Const struct predinfo predinfo_klic__comp__cmacro_xexpand_4 = {
  (char *)"expand",
  &predicate_klic__comp__cmacro_xexpand_4
};

extern Const struct predicate predicate_klic__comp__cmacro_xexpand_2;
static Const struct predinfo predinfo_klic__comp__cmacro_xexpand_2 = {
  (char *)"expand",
  &predicate_klic__comp__cmacro_xexpand_2
};

static Const struct predinfo * Const preds_in_klic__comp__cmacro[] = {
  &predinfo_klic__comp__cmacro_xreplace_7,
  &predinfo_klic__comp__cmacro_xreplace_6,
  &predinfo_klic__comp__cmacro_xreplace_5,
  &predinfo_klic__comp__cmacro_xreplaceHead_5,
  &predinfo_klic__comp__cmacro_xreplaceGoals_7,
  &predinfo_klic__comp__cmacro_xreplaceGoals_5,
  &predinfo_klic__comp__cmacro_xreplaceClause_4,
  &predinfo_klic__comp__cmacro_xreconstGoals_2,
  &predinfo_klic__comp__cmacro_xinsDefs_3,
  &predinfo_klic__comp__cmacro_xexpand_4,
  &predinfo_klic__comp__cmacro_xexpand_2,
  0
};
extern Const struct predicate predicate_keyed__set_xcreate_3;
static Const struct predinfo predinfo_keyed__set_xcreate_3 = {
  (char *)"create",
  &predicate_keyed__set_xcreate_3
};

extern Const struct predicate predicate_keyed__set_xcreate_2;
static Const struct predinfo predinfo_keyed__set_xcreate_2 = {
  (char *)"create",
  &predicate_keyed__set_xcreate_2
};

extern Const struct predicate predicate_keyed__set_xcreate_1;
static Const struct predinfo predinfo_keyed__set_xcreate_1 = {
  (char *)"create",
  &predicate_keyed__set_xcreate_1
};

static Const struct predinfo * Const preds_in_keyed__set[] = {
  &predinfo_keyed__set_xcreate_3,
  &predinfo_keyed__set_xcreate_2,
  &predinfo_keyed__set_xcreate_1,
  0
};
extern Const struct predicate predicate_unparser_xunwparse__cons_5;
static Const struct predinfo predinfo_unparser_xunwparse__cons_5 = {
  (char *)"unwparse_cons",
  &predicate_unparser_xunwparse__cons_5
};

extern Const struct predicate predicate_unparser_xunwparse__cdr_5;
static Const struct predinfo predinfo_unparser_xunwparse__cdr_5 = {
  (char *)"unwparse_cdr",
  &predicate_unparser_xunwparse__cdr_5
};

extern Const struct predicate predicate_unparser_xunwparse_6;
static Const struct predinfo predinfo_unparser_xunwparse_6 = {
  (char *)"unwparse",
  &predicate_unparser_xunwparse_6
};

extern Const struct predicate predicate_unparser_xunwparse_4;
static Const struct predinfo predinfo_unparser_xunwparse_4 = {
  (char *)"unwparse",
  &predicate_unparser_xunwparse_4
};

extern Const struct predicate predicate_unparser_xunscan__after__op_4;
static Const struct predinfo predinfo_unparser_xunscan__after__op_4 = {
  (char *)"unscan_after_op",
  &predicate_unparser_xunscan__after__op_4
};

extern Const struct predicate predicate_unparser_xunscan_4;
static Const struct predinfo predinfo_unparser_xunscan_4 = {
  (char *)"unscan",
  &predicate_unparser_xunscan_4
};

extern Const struct predicate predicate_unparser_xunparse__with__op_10;
static Const struct predinfo predinfo_unparser_xunparse__with__op_10 = {
  (char *)"unparse_with_op",
  &predicate_unparser_xunparse__with__op_10
};

extern Const struct predicate predicate_unparser_xunparse__vector_5;
static Const struct predinfo predinfo_unparser_xunparse__vector_5 = {
  (char *)"unparse_vector",
  &predicate_unparser_xunparse__vector_5
};

extern Const struct predicate predicate_unparser_xunparse__vecelems_7;
static Const struct predinfo predinfo_unparser_xunparse__vecelems_7 = {
  (char *)"unparse_vecelems",
  &predicate_unparser_xunparse__vecelems_7
};

extern Const struct predicate predicate_unparser_xunparse__string_3;
static Const struct predinfo predinfo_unparser_xunparse__string_3 = {
  (char *)"unparse_string",
  &predicate_unparser_xunparse__string_3
};

extern Const struct predicate predicate_unparser_xunparse__int_3;
static Const struct predinfo predinfo_unparser_xunparse__int_3 = {
  (char *)"unparse_int",
  &predicate_unparser_xunparse__int_3
};

extern Const struct predicate predicate_unparser_xunparse__general_6;
static Const struct predinfo predinfo_unparser_xunparse__general_6 = {
  (char *)"unparse_general",
  &predicate_unparser_xunparse__general_6
};

extern Const struct predicate predicate_unparser_xunparse__float_3;
static Const struct predinfo predinfo_unparser_xunparse__float_3 = {
  (char *)"unparse_float",
  &predicate_unparser_xunparse__float_3
};

extern Const struct predicate predicate_unparser_xunparse__elems_7;
static Const struct predinfo predinfo_unparser_xunparse__elems_7 = {
  (char *)"unparse_elems",
  &predicate_unparser_xunparse__elems_7
};

extern Const struct predicate predicate_unparser_xunparse__cons_5;
static Const struct predinfo predinfo_unparser_xunparse__cons_5 = {
  (char *)"unparse_cons",
  &predicate_unparser_xunparse__cons_5
};

extern Const struct predicate predicate_unparser_xunparse__comma__list__elem_5;
static Const struct predinfo predinfo_unparser_xunparse__comma__list__elem_5 = {
  (char *)"unparse_comma_list_elem",
  &predicate_unparser_xunparse__comma__list__elem_5
};

extern Const struct predicate predicate_unparser_xunparse__cdr_5;
static Const struct predinfo predinfo_unparser_xunparse__cdr_5 = {
  (char *)"unparse_cdr",
  &predicate_unparser_xunparse__cdr_5
};

extern Const struct predicate predicate_unparser_xunparse__bin__op_3;
static Const struct predinfo predinfo_unparser_xunparse__bin__op_3 = {
  (char *)"unparse_bin_op",
  &predicate_unparser_xunparse__bin__op_3
};

extern Const struct predicate predicate_unparser_xunparse__atom_3;
static Const struct predinfo predinfo_unparser_xunparse__atom_3 = {
  (char *)"unparse_atom",
  &predicate_unparser_xunparse__atom_3
};

extern Const struct predicate predicate_unparser_xunparse_6;
static Const struct predinfo predinfo_unparser_xunparse_6 = {
  (char *)"unparse",
  &predicate_unparser_xunparse_6
};

extern Const struct predicate predicate_unparser_xunparse_4;
static Const struct predinfo predinfo_unparser_xunparse_4 = {
  (char *)"unparse",
  &predicate_unparser_xunparse_4
};

extern Const struct predicate predicate_unparser_xstr__elem__chars_3;
static Const struct predinfo predinfo_unparser_xstr__elem__chars_3 = {
  (char *)"str_elem_chars",
  &predicate_unparser_xstr__elem__chars_3
};

extern Const struct predicate predicate_unparser_xstr__chars_5;
static Const struct predinfo predinfo_unparser_xstr__chars_5 = {
  (char *)"str_chars",
  &predicate_unparser_xstr__chars_5
};

extern Const struct predicate predicate_unparser_xquoted__atom__chars_4;
static Const struct predinfo predinfo_unparser_xquoted__atom__chars_4 = {
  (char *)"quoted_atom_chars",
  &predicate_unparser_xquoted__atom__chars_4
};

extern Const struct predicate predicate_unparser_xquoted__atom__chars_2F4_240_3;
static Const struct predinfo predinfo_unparser_xquoted__atom__chars_2F4_240_3 = {
  (char *)"quoted_atom_chars/4$0",
  &predicate_unparser_xquoted__atom__chars_2F4_240_3
};

extern Const struct predicate predicate_unparser_xmax__op_4;
static Const struct predinfo predinfo_unparser_xmax__op_4 = {
  (char *)"max_op",
  &predicate_unparser_xmax__op_4
};

extern Const struct predicate predicate_unparser_xmax_3;
static Const struct predinfo predinfo_unparser_xmax_3 = {
  (char *)"max",
  &predicate_unparser_xmax_3
};

extern Const struct predicate predicate_unparser_xmake__atom__string_4;
static Const struct predinfo predinfo_unparser_xmake__atom__string_4 = {
  (char *)"make_atom_string",
  &predicate_unparser_xmake__atom__string_4
};

extern Const struct predicate predicate_unparser_xjudge__quoted_6;
static Const struct predinfo predinfo_unparser_xjudge__quoted_6 = {
  (char *)"judge_quoted",
  &predicate_unparser_xjudge__quoted_6
};

extern Const struct predicate predicate_unparser_xint__chars_3;
static Const struct predinfo predinfo_unparser_xint__chars_3 = {
  (char *)"int_chars",
  &predicate_unparser_xint__chars_3
};

extern Const struct predicate predicate_unparser_xins__if__greater__than_5;
static Const struct predinfo predinfo_unparser_xins__if__greater__than_5 = {
  (char *)"ins_if_greater_than",
  &predicate_unparser_xins__if__greater__than_5
};

extern Const struct predicate predicate_unparser_xins__backslash_3;
static Const struct predinfo predinfo_unparser_xins__backslash_3 = {
  (char *)"ins_backslash",
  &predicate_unparser_xins__backslash_3
};

extern Const struct predicate predicate_unparser_xfirst__char__kind_2;
static Const struct predinfo predinfo_unparser_xfirst__char__kind_2 = {
  (char *)"first_char_kind",
  &predicate_unparser_xfirst__char__kind_2
};

extern Const struct predicate predicate_unparser_xfind__operator_3;
static Const struct predinfo predinfo_unparser_xfind__operator_3 = {
  (char *)"find_operator",
  &predicate_unparser_xfind__operator_3
};

extern Const struct predicate predicate_unparser_xdelimit_4;
static Const struct predinfo predinfo_unparser_xdelimit_4 = {
  (char *)"delimit",
  &predicate_unparser_xdelimit_4
};

extern Const struct predicate predicate_unparser_xchartype_2F2_240_2;
static Const struct predinfo predinfo_unparser_xchartype_2F2_240_2 = {
  (char *)"chartype/2$0",
  &predicate_unparser_xchartype_2F2_240_2
};

extern Const struct predicate predicate_unparser_xchartype_2;
static Const struct predinfo predinfo_unparser_xchartype_2 = {
  (char *)"chartype",
  &predicate_unparser_xchartype_2
};

extern Const struct predicate predicate_unparser_xchar__kind_2;
static Const struct predinfo predinfo_unparser_xchar__kind_2 = {
  (char *)"char_kind",
  &predicate_unparser_xchar__kind_2
};

extern Const struct predicate predicate_unparser_xcall__unparser_6;
static Const struct predinfo predinfo_unparser_xcall__unparser_6 = {
  (char *)"call_unparser",
  &predicate_unparser_xcall__unparser_6
};

extern Const struct predicate predicate_unparser_xatom__kind_3;
static Const struct predinfo predinfo_unparser_xatom__kind_3 = {
  (char *)"atom_kind",
  &predicate_unparser_xatom__kind_3
};

extern Const struct predicate predicate_unparser_xatom__kind_2F3_240_4;
static Const struct predinfo predinfo_unparser_xatom__kind_2F3_240_4 = {
  (char *)"atom_kind/3$0",
  &predicate_unparser_xatom__kind_2F3_240_4
};

static Const struct predinfo * Const preds_in_unparser[] = {
  &predinfo_unparser_xunwparse__cons_5,
  &predinfo_unparser_xunwparse__cdr_5,
  &predinfo_unparser_xunwparse_6,
  &predinfo_unparser_xunwparse_4,
  &predinfo_unparser_xunscan__after__op_4,
  &predinfo_unparser_xunscan_4,
  &predinfo_unparser_xunparse__with__op_10,
  &predinfo_unparser_xunparse__vector_5,
  &predinfo_unparser_xunparse__vecelems_7,
  &predinfo_unparser_xunparse__string_3,
  &predinfo_unparser_xunparse__int_3,
  &predinfo_unparser_xunparse__general_6,
  &predinfo_unparser_xunparse__float_3,
  &predinfo_unparser_xunparse__elems_7,
  &predinfo_unparser_xunparse__cons_5,
  &predinfo_unparser_xunparse__comma__list__elem_5,
  &predinfo_unparser_xunparse__cdr_5,
  &predinfo_unparser_xunparse__bin__op_3,
  &predinfo_unparser_xunparse__atom_3,
  &predinfo_unparser_xunparse_6,
  &predinfo_unparser_xunparse_4,
  &predinfo_unparser_xstr__elem__chars_3,
  &predinfo_unparser_xstr__chars_5,
  &predinfo_unparser_xquoted__atom__chars_4,
  &predinfo_unparser_xquoted__atom__chars_2F4_240_3,
  &predinfo_unparser_xmax__op_4,
  &predinfo_unparser_xmax_3,
  &predinfo_unparser_xmake__atom__string_4,
  &predinfo_unparser_xjudge__quoted_6,
  &predinfo_unparser_xint__chars_3,
  &predinfo_unparser_xins__if__greater__than_5,
  &predinfo_unparser_xins__backslash_3,
  &predinfo_unparser_xfirst__char__kind_2,
  &predinfo_unparser_xfind__operator_3,
  &predinfo_unparser_xdelimit_4,
  &predinfo_unparser_xchartype_2F2_240_2,
  &predinfo_unparser_xchartype_2,
  &predinfo_unparser_xchar__kind_2,
  &predinfo_unparser_xcall__unparser_6,
  &predinfo_unparser_xatom__kind_3,
  &predinfo_unparser_xatom__kind_2F3_240_4,
  0
};
extern Const struct predicate predicate_scanner_xungetc_3;
static Const struct predinfo predinfo_scanner_xungetc_3 = {
  (char *)"ungetc",
  &predicate_scanner_xungetc_3
};

extern Const struct predicate predicate_scanner_xscan__unsigned__exponent_9;
static Const struct predinfo predinfo_scanner_xscan__unsigned__exponent_9 = {
  (char *)"scan_unsigned_exponent",
  &predicate_scanner_xscan__unsigned__exponent_9
};

extern Const struct predicate predicate_scanner_xscan__string_8;
static Const struct predinfo predinfo_scanner_xscan__string_8 = {
  (char *)"scan_string",
  &predicate_scanner_xscan__string_8
};

extern Const struct predicate predicate_scanner_xscan__string_2F8_240_8;
static Const struct predinfo predinfo_scanner_xscan__string_2F8_240_8 = {
  (char *)"scan_string/8$0",
  &predicate_scanner_xscan__string_2F8_240_8
};

extern Const struct predicate predicate_scanner_xscan__sname_7;
static Const struct predinfo predinfo_scanner_xscan__sname_7 = {
  (char *)"scan_sname",
  &predicate_scanner_xscan__sname_7
};

extern Const struct predicate predicate_scanner_xscan__rest_4;
static Const struct predinfo predinfo_scanner_xscan__rest_4 = {
  (char *)"scan_rest",
  &predicate_scanner_xscan__rest_4
};

extern Const struct predicate predicate_scanner_xscan__octal__escape__2_9;
static Const struct predinfo predinfo_scanner_xscan__octal__escape__2_9 = {
  (char *)"scan_octal_escape_2",
  &predicate_scanner_xscan__octal__escape__2_9
};

extern Const struct predicate predicate_scanner_xscan__octal__escape__2_2F9_240_9;
static Const struct predinfo predinfo_scanner_xscan__octal__escape__2_2F9_240_9 = {
  (char *)"scan_octal_escape_2/9$0",
  &predicate_scanner_xscan__octal__escape__2_2F9_240_9
};

extern Const struct predicate predicate_scanner_xscan__octal__escape_9;
static Const struct predinfo predinfo_scanner_xscan__octal__escape_9 = {
  (char *)"scan_octal_escape",
  &predicate_scanner_xscan__octal__escape_9
};

extern Const struct predicate predicate_scanner_xscan__number_9;
static Const struct predinfo predinfo_scanner_xscan__number_9 = {
  (char *)"scan_number",
  &predicate_scanner_xscan__number_9
};

extern Const struct predicate predicate_scanner_xscan__number_2F9_241_8;
static Const struct predinfo predinfo_scanner_xscan__number_2F9_241_8 = {
  (char *)"scan_number/9$1",
  &predicate_scanner_xscan__number_2F9_241_8
};

extern Const struct predicate predicate_scanner_xscan__number_2F9_240_9;
static Const struct predinfo predinfo_scanner_xscan__number_2F9_240_9 = {
  (char *)"scan_number/9$0",
  &predicate_scanner_xscan__number_2F9_240_9
};

extern Const struct predicate predicate_scanner_xscan__name_7;
static Const struct predinfo predinfo_scanner_xscan__name_7 = {
  (char *)"scan_name",
  &predicate_scanner_xscan__name_7
};

extern Const struct predicate predicate_scanner_xscan__hex__escape_9;
static Const struct predinfo predinfo_scanner_xscan__hex__escape_9 = {
  (char *)"scan_hex_escape",
  &predicate_scanner_xscan__hex__escape_9
};

extern Const struct predicate predicate_scanner_xscan__fraction_9;
static Const struct predinfo predinfo_scanner_xscan__fraction_9 = {
  (char *)"scan_fraction",
  &predicate_scanner_xscan__fraction_9
};

extern Const struct predicate predicate_scanner_xscan__fraction_2F9_240_11;
static Const struct predinfo predinfo_scanner_xscan__fraction_2F9_240_11 = {
  (char *)"scan_fraction/9$0",
  &predicate_scanner_xscan__fraction_2F9_240_11
};

extern Const struct predicate predicate_scanner_xscan__exponent_9;
static Const struct predinfo predinfo_scanner_xscan__exponent_9 = {
  (char *)"scan_exponent",
  &predicate_scanner_xscan__exponent_9
};

extern Const struct predicate predicate_scanner_xscan__escape_8;
static Const struct predinfo predinfo_scanner_xscan__escape_8 = {
  (char *)"scan_escape",
  &predicate_scanner_xscan__escape_8
};

extern Const struct predicate predicate_scanner_xscan__comment__line_5;
static Const struct predinfo predinfo_scanner_xscan__comment__line_5 = {
  (char *)"scan_comment_line",
  &predicate_scanner_xscan__comment__line_5
};

extern Const struct predicate predicate_scanner_xscan__comment_5;
static Const struct predinfo predinfo_scanner_xscan__comment_5 = {
  (char *)"scan_comment",
  &predicate_scanner_xscan__comment_5
};

extern Const struct predicate predicate_scanner_xscan__comment_2F5_240_5;
static Const struct predinfo predinfo_scanner_xscan__comment_2F5_240_5 = {
  (char *)"scan_comment/5$0",
  &predicate_scanner_xscan__comment_2F5_240_5
};

extern Const struct predicate predicate_scanner_xscan__based__number_9;
static Const struct predinfo predinfo_scanner_xscan__based__number_9 = {
  (char *)"scan_based_number",
  &predicate_scanner_xscan__based__number_9
};

extern Const struct predicate predicate_scanner_xscan__based__number_2F9_242_9;
static Const struct predinfo predinfo_scanner_xscan__based__number_2F9_242_9 = {
  (char *)"scan_based_number/9$2",
  &predicate_scanner_xscan__based__number_2F9_242_9
};

extern Const struct predicate predicate_scanner_xscan__based__number_2F9_241_9;
static Const struct predinfo predinfo_scanner_xscan__based__number_2F9_241_9 = {
  (char *)"scan_based_number/9$1",
  &predicate_scanner_xscan__based__number_2F9_241_9
};

extern Const struct predicate predicate_scanner_xscan__based__number_2F9_240_9;
static Const struct predinfo predinfo_scanner_xscan__based__number_2F9_240_9 = {
  (char *)"scan_based_number/9$0",
  &predicate_scanner_xscan__based__number_2F9_240_9
};

extern Const struct predicate predicate_scanner_xscan__after__atom_8;
static Const struct predinfo predinfo_scanner_xscan__after__atom_8 = {
  (char *)"scan_after_atom",
  &predicate_scanner_xscan__after__atom_8
};

extern Const struct predicate predicate_scanner_xscan_6;
static Const struct predinfo predinfo_scanner_xscan_6 = {
  (char *)"scan",
  &predicate_scanner_xscan_6
};

extern Const struct predicate predicate_scanner_xscan_3;
static Const struct predinfo predinfo_scanner_xscan_3 = {
  (char *)"scan",
  &predicate_scanner_xscan_3
};

extern Const struct predicate predicate_scanner_xscan_2F6_243_7;
static Const struct predinfo predinfo_scanner_xscan_2F6_243_7 = {
  (char *)"scan/6$3",
  &predicate_scanner_xscan_2F6_243_7
};

extern Const struct predicate predicate_scanner_xscan_2F6_243_2F7_240_7;
static Const struct predinfo predinfo_scanner_xscan_2F6_243_2F7_240_7 = {
  (char *)"scan/6$3/7$0",
  &predicate_scanner_xscan_2F6_243_2F7_240_7
};

extern Const struct predicate predicate_scanner_xscan_2F6_242_8;
static Const struct predinfo predinfo_scanner_xscan_2F6_242_8 = {
  (char *)"scan/6$2",
  &predicate_scanner_xscan_2F6_242_8
};

extern Const struct predicate predicate_scanner_xscan_2F6_241_8;
static Const struct predinfo predinfo_scanner_xscan_2F6_241_8 = {
  (char *)"scan/6$1",
  &predicate_scanner_xscan_2F6_241_8
};

extern Const struct predicate predicate_scanner_xscan_2F6_240_6;
static Const struct predinfo predinfo_scanner_xscan_2F6_240_6 = {
  (char *)"scan/6$0",
  &predicate_scanner_xscan_2F6_240_6
};

extern Const struct predicate predicate_scanner_xmake__new__string_2;
static Const struct predinfo predinfo_scanner_xmake__new__string_2 = {
  (char *)"make_new_string",
  &predicate_scanner_xmake__new__string_2
};

extern Const struct predicate predicate_scanner_xlookup__var_8;
static Const struct predinfo predinfo_scanner_xlookup__var_8 = {
  (char *)"lookup_var",
  &predicate_scanner_xlookup__var_8
};

extern Const struct predicate predicate_scanner_xgetchar_4;
static Const struct predinfo predinfo_scanner_xgetchar_4 = {
  (char *)"getchar",
  &predicate_scanner_xgetchar_4
};

extern Const struct predicate predicate_scanner_xget0_3;
static Const struct predinfo predinfo_scanner_xget0_3 = {
  (char *)"get0",
  &predicate_scanner_xget0_3
};

extern Const struct predicate predicate_scanner_xfinalize__vars_2F1_240_2;
static Const struct predinfo predinfo_scanner_xfinalize__vars_2F1_240_2 = {
  (char *)"finalize_vars/1$0",
  &predicate_scanner_xfinalize__vars_2F1_240_2
};

extern Const struct predicate predicate_scanner_xfinalize__vars_1;
static Const struct predinfo predinfo_scanner_xfinalize__vars_1 = {
  (char *)"finalize_vars",
  &predicate_scanner_xfinalize__vars_1
};

extern Const struct predicate predicate_scanner_xescape__char_2;
static Const struct predinfo predinfo_scanner_xescape__char_2 = {
  (char *)"escape_char",
  &predicate_scanner_xescape__char_2
};

extern Const struct predicate predicate_scanner_xchartype_2;
static Const struct predinfo predinfo_scanner_xchartype_2 = {
  (char *)"chartype",
  &predicate_scanner_xchartype_2
};

extern Const struct predicate predicate_scanner_xchar__list__to__int_3;
static Const struct predinfo predinfo_scanner_xchar__list__to__int_3 = {
  (char *)"char_list_to_int",
  &predicate_scanner_xchar__list__to__int_3
};

extern Const struct predicate predicate_scanner_xchar__list__to__float_3;
static Const struct predinfo predinfo_scanner_xchar__list__to__float_3 = {
  (char *)"char_list_to_float",
  &predicate_scanner_xchar__list__to__float_3
};

extern Const struct predicate predicate_scanner_xatom__chars_2;
static Const struct predinfo predinfo_scanner_xatom__chars_2 = {
  (char *)"atom_chars",
  &predicate_scanner_xatom__chars_2
};

static Const struct predinfo * Const preds_in_scanner[] = {
  &predinfo_scanner_xungetc_3,
  &predinfo_scanner_xscan__unsigned__exponent_9,
  &predinfo_scanner_xscan__string_8,
  &predinfo_scanner_xscan__string_2F8_240_8,
  &predinfo_scanner_xscan__sname_7,
  &predinfo_scanner_xscan__rest_4,
  &predinfo_scanner_xscan__octal__escape__2_9,
  &predinfo_scanner_xscan__octal__escape__2_2F9_240_9,
  &predinfo_scanner_xscan__octal__escape_9,
  &predinfo_scanner_xscan__number_9,
  &predinfo_scanner_xscan__number_2F9_241_8,
  &predinfo_scanner_xscan__number_2F9_240_9,
  &predinfo_scanner_xscan__name_7,
  &predinfo_scanner_xscan__hex__escape_9,
  &predinfo_scanner_xscan__fraction_9,
  &predinfo_scanner_xscan__fraction_2F9_240_11,
  &predinfo_scanner_xscan__exponent_9,
  &predinfo_scanner_xscan__escape_8,
  &predinfo_scanner_xscan__comment__line_5,
  &predinfo_scanner_xscan__comment_5,
  &predinfo_scanner_xscan__comment_2F5_240_5,
  &predinfo_scanner_xscan__based__number_9,
  &predinfo_scanner_xscan__based__number_2F9_242_9,
  &predinfo_scanner_xscan__based__number_2F9_241_9,
  &predinfo_scanner_xscan__based__number_2F9_240_9,
  &predinfo_scanner_xscan__after__atom_8,
  &predinfo_scanner_xscan_6,
  &predinfo_scanner_xscan_3,
  &predinfo_scanner_xscan_2F6_243_7,
  &predinfo_scanner_xscan_2F6_243_2F7_240_7,
  &predinfo_scanner_xscan_2F6_242_8,
  &predinfo_scanner_xscan_2F6_241_8,
  &predinfo_scanner_xscan_2F6_240_6,
  &predinfo_scanner_xmake__new__string_2,
  &predinfo_scanner_xlookup__var_8,
  &predinfo_scanner_xgetchar_4,
  &predinfo_scanner_xget0_3,
  &predinfo_scanner_xfinalize__vars_2F1_240_2,
  &predinfo_scanner_xfinalize__vars_1,
  &predinfo_scanner_xescape__char_2,
  &predinfo_scanner_xchartype_2,
  &predinfo_scanner_xchar__list__to__int_3,
  &predinfo_scanner_xchar__list__to__float_3,
  &predinfo_scanner_xatom__chars_2,
  0
};
extern Const struct predicate predicate_variable_xwrap__vec__elems_4;
static Const struct predinfo predinfo_variable_xwrap__vec__elems_4 = {
  (char *)"wrap_vec_elems",
  &predicate_variable_xwrap__vec__elems_4
};

extern Const struct predicate predicate_variable_xwrap__funct__elems_6;
static Const struct predinfo predinfo_variable_xwrap__funct__elems_6 = {
  (char *)"wrap_funct_elems",
  &predicate_variable_xwrap__funct__elems_6
};

extern Const struct predicate predicate_variable_xwrap__bound_2;
static Const struct predinfo predinfo_variable_xwrap__bound_2 = {
  (char *)"wrap_bound",
  &predicate_variable_xwrap__bound_2
};

extern Const struct predicate predicate_variable_xwrap_2F2_240_3;
static Const struct predinfo predinfo_variable_xwrap_2F2_240_3 = {
  (char *)"wrap/2$0",
  &predicate_variable_xwrap_2F2_240_3
};

extern Const struct predicate predicate_variable_xwrap_2;
static Const struct predinfo predinfo_variable_xwrap_2 = {
  (char *)"wrap",
  &predicate_variable_xwrap_2
};

extern Const struct predicate predicate_variable_xunwrap__vector_6;
static Const struct predinfo predinfo_variable_xunwrap__vector_6 = {
  (char *)"unwrap_vector",
  &predicate_variable_xunwrap__vector_6
};

extern Const struct predicate predicate_variable_xunwrap__functor_8;
static Const struct predinfo predinfo_variable_xunwrap__functor_8 = {
  (char *)"unwrap_functor",
  &predicate_variable_xunwrap__functor_8
};

extern Const struct predicate predicate_variable_xunwrap_4;
static Const struct predinfo predinfo_variable_xunwrap_4 = {
  (char *)"unwrap",
  &predicate_variable_xunwrap_4
};

extern Const struct predicate predicate_variable_xunwrap_3;
static Const struct predinfo predinfo_variable_xunwrap_3 = {
  (char *)"unwrap",
  &predicate_variable_xunwrap_3
};

extern Const struct predicate predicate_variable_xunwrap_2;
static Const struct predinfo predinfo_variable_xunwrap_2 = {
  (char *)"unwrap",
  &predicate_variable_xunwrap_2
};

extern Const struct predicate predicate_variable_xunbound_2;
static Const struct predinfo predinfo_variable_xunbound_2 = {
  (char *)"unbound",
  &predicate_variable_xunbound_2
};

extern Const struct predicate predicate_variable_xget__variable_4;
static Const struct predinfo predinfo_variable_xget__variable_4 = {
  (char *)"get_variable",
  &predicate_variable_xget__variable_4
};

extern Const struct predicate predicate_variable_xget__variable2_4;
static Const struct predinfo predinfo_variable_xget__variable2_4 = {
  (char *)"get_variable2",
  &predicate_variable_xget__variable2_4
};

static Const struct predinfo * Const preds_in_variable[] = {
  &predinfo_variable_xwrap__vec__elems_4,
  &predinfo_variable_xwrap__funct__elems_6,
  &predinfo_variable_xwrap__bound_2,
  &predinfo_variable_xwrap_2F2_240_3,
  &predinfo_variable_xwrap_2,
  &predinfo_variable_xunwrap__vector_6,
  &predinfo_variable_xunwrap__functor_8,
  &predinfo_variable_xunwrap_4,
  &predinfo_variable_xunwrap_3,
  &predinfo_variable_xunwrap_2,
  &predinfo_variable_xunbound_2,
  &predinfo_variable_xget__variable_4,
  &predinfo_variable_xget__variable2_4,
  0
};
extern Const struct predicate predicate_parser_xwrap__list_2;
static Const struct predinfo predinfo_parser_xwrap__list_2 = {
  (char *)"wrap_list",
  &predicate_parser_xwrap__list_2
};

extern Const struct predicate predicate_parser_xwrap__functor_2;
static Const struct predinfo predinfo_parser_xwrap__functor_2 = {
  (char *)"wrap_functor",
  &predicate_parser_xwrap__functor_2
};

extern Const struct predicate predicate_parser_xwrap__functor1_2;
static Const struct predinfo predinfo_parser_xwrap__functor1_2 = {
  (char *)"wrap_functor1",
  &predicate_parser_xwrap__functor1_2
};

extern Const struct predicate predicate_parser_xwparse_5;
static Const struct predinfo predinfo_parser_xwparse_5 = {
  (char *)"wparse",
  &predicate_parser_xwparse_5
};

extern Const struct predicate predicate_parser_xwparse_4;
static Const struct predinfo predinfo_parser_xwparse_4 = {
  (char *)"wparse",
  &predicate_parser_xwparse_4
};

extern Const struct predicate predicate_parser_xwparse_3;
static Const struct predinfo predinfo_parser_xwparse_3 = {
  (char *)"wparse",
  &predicate_parser_xwparse_3
};

extern Const struct predicate predicate_parser_xwparse_2F5_244_5;
static Const struct predinfo predinfo_parser_xwparse_2F5_244_5 = {
  (char *)"wparse/5$4",
  &predicate_parser_xwparse_2F5_244_5
};

extern Const struct predicate predicate_parser_xwparse_2F5_244_2F5_242_2;
static Const struct predinfo predinfo_parser_xwparse_2F5_244_2F5_242_2 = {
  (char *)"wparse/5$4/5$2",
  &predicate_parser_xwparse_2F5_244_2F5_242_2
};

extern Const struct predicate predicate_parser_xwparse_2F5_244_2F5_241_5;
static Const struct predinfo predinfo_parser_xwparse_2F5_244_2F5_241_5 = {
  (char *)"wparse/5$4/5$1",
  &predicate_parser_xwparse_2F5_244_2F5_241_5
};

extern Const struct predicate predicate_parser_xwparse_2F5_244_2F5_240_6;
static Const struct predinfo predinfo_parser_xwparse_2F5_244_2F5_240_6 = {
  (char *)"wparse/5$4/5$0",
  &predicate_parser_xwparse_2F5_244_2F5_240_6
};

extern Const struct predicate predicate_parser_xwparse_2F5_244_2F5_240_2F6_240_6;
static Const struct predinfo predinfo_parser_xwparse_2F5_244_2F5_240_2F6_240_6 = {
  (char *)"wparse/5$4/5$0/6$0",
  &predicate_parser_xwparse_2F5_244_2F5_240_2F6_240_6
};

extern Const struct predicate predicate_parser_xwparse_2F5_243_5;
static Const struct predinfo predinfo_parser_xwparse_2F5_243_5 = {
  (char *)"wparse/5$3",
  &predicate_parser_xwparse_2F5_243_5
};

extern Const struct predicate predicate_parser_xwparse_2F5_242_4;
static Const struct predinfo predinfo_parser_xwparse_2F5_242_4 = {
  (char *)"wparse/5$2",
  &predicate_parser_xwparse_2F5_242_4
};

extern Const struct predicate predicate_parser_xwparse_2F5_242_2F4_240_4;
static Const struct predinfo predinfo_parser_xwparse_2F5_242_2F4_240_4 = {
  (char *)"wparse/5$2/4$0",
  &predicate_parser_xwparse_2F5_242_2F4_240_4
};

extern Const struct predicate predicate_parser_xwparse_2F5_241_4;
static Const struct predinfo predinfo_parser_xwparse_2F5_241_4 = {
  (char *)"wparse/5$1",
  &predicate_parser_xwparse_2F5_241_4
};

extern Const struct predicate predicate_parser_xwparse_2F5_241_2F4_240_4;
static Const struct predinfo predinfo_parser_xwparse_2F5_241_2F4_240_4 = {
  (char *)"wparse/5$1/4$0",
  &predicate_parser_xwparse_2F5_241_2F4_240_4
};

extern Const struct predicate predicate_parser_xwparse_2F5_240_4;
static Const struct predinfo predinfo_parser_xwparse_2F5_240_4 = {
  (char *)"wparse/5$0",
  &predicate_parser_xwparse_2F5_240_4
};

extern Const struct predicate predicate_parser_xwparse_2F5_240_2F4_240_5;
static Const struct predinfo predinfo_parser_xwparse_2F5_240_2F4_240_5 = {
  (char *)"wparse/5$0/4$0",
  &predicate_parser_xwparse_2F5_240_2F4_240_5
};

extern Const struct predicate predicate_parser_xwait__parse_2F2_240_2;
static Const struct predinfo predinfo_parser_xwait__parse_2F2_240_2 = {
  (char *)"wait_parse/2$0",
  &predicate_parser_xwait__parse_2F2_240_2
};

extern Const struct predicate predicate_parser_xwait__parse_2;
static Const struct predinfo predinfo_parser_xwait__parse_2 = {
  (char *)"wait_parse",
  &predicate_parser_xwait__parse_2
};

extern Const struct predicate predicate_parser_xunwrap_2;
static Const struct predinfo predinfo_parser_xunwrap_2 = {
  (char *)"unwrap",
  &predicate_parser_xunwrap_2
};

extern Const struct predicate predicate_parser_xselect__better_3;
static Const struct predinfo predinfo_parser_xselect__better_3 = {
  (char *)"select_better",
  &predicate_parser_xselect__better_3
};

extern Const struct predicate predicate_parser_xselect__better_2F3_241_4;
static Const struct predinfo predinfo_parser_xselect__better_2F3_241_4 = {
  (char *)"select_better/3$1",
  &predicate_parser_xselect__better_2F3_241_4
};

extern Const struct predicate predicate_parser_xselect__better_2F3_240_4;
static Const struct predinfo predinfo_parser_xselect__better_2F3_240_4 = {
  (char *)"select_better/3$0",
  &predicate_parser_xselect__better_2F3_240_4
};

extern Const struct predicate predicate_parser_xprefixop_4;
static Const struct predinfo predinfo_parser_xprefixop_4 = {
  (char *)"prefixop",
  &predicate_parser_xprefixop_4
};

extern Const struct predicate predicate_parser_xprefixop_2F4_240_4;
static Const struct predinfo predinfo_parser_xprefixop_2F4_240_4 = {
  (char *)"prefixop/4$0",
  &predicate_parser_xprefixop_2F4_240_4
};

extern Const struct predicate predicate_parser_xpostfixop_5;
static Const struct predinfo predinfo_parser_xpostfixop_5 = {
  (char *)"postfixop",
  &predicate_parser_xpostfixop_5
};

extern Const struct predicate predicate_parser_xpostfixop_2F5_240_5;
static Const struct predinfo predinfo_parser_xpostfixop_2F5_240_5 = {
  (char *)"postfixop/5$0",
  &predicate_parser_xpostfixop_2F5_240_5
};

extern Const struct predicate predicate_parser_xparse__comma__list_8;
static Const struct predinfo predinfo_parser_xparse__comma__list_8 = {
  (char *)"parse_comma_list",
  &predicate_parser_xparse__comma__list_8
};

extern Const struct predicate predicate_parser_xparse__comma__list_6;
static Const struct predinfo predinfo_parser_xparse__comma__list_6 = {
  (char *)"parse_comma_list",
  &predicate_parser_xparse__comma__list_6
};

extern Const struct predicate predicate_parser_xparse__comma__list_2F8_240_8;
static Const struct predinfo predinfo_parser_xparse__comma__list_2F8_240_8 = {
  (char *)"parse_comma_list/8$0",
  &predicate_parser_xparse__comma__list_2F8_240_8
};

extern Const struct predicate predicate_parser_xparse__comma__list_2F8_240_2F8_240_8;
static Const struct predinfo predinfo_parser_xparse__comma__list_2F8_240_2F8_240_8 = {
  (char *)"parse_comma_list/8$0/8$0",
  &predicate_parser_xparse__comma__list_2F8_240_2F8_240_8
};

extern Const struct predicate predicate_parser_xparse__comma__list_2F8_240_2F8_240_2F8_240_5;
static Const struct predinfo predinfo_parser_xparse__comma__list_2F8_240_2F8_240_2F8_240_5 = {
  (char *)"parse_comma_list/8$0/8$0/8$0",
  &predicate_parser_xparse__comma__list_2F8_240_2F8_240_2F8_240_5
};

extern Const struct predicate predicate_parser_xparse__comma__list_2F8_240_2F8_240_2F8_240_2F5_240_4;
static Const struct predinfo predinfo_parser_xparse__comma__list_2F8_240_2F8_240_2F8_240_2F5_240_4 = {
  (char *)"parse_comma_list/8$0/8$0/8$0/5$0",
  &predicate_parser_xparse__comma__list_2F8_240_2F8_240_2F8_240_2F5_240_4
};

extern Const struct predicate predicate_parser_xparse_3;
static Const struct predinfo predinfo_parser_xparse_3 = {
  (char *)"parse",
  &predicate_parser_xparse_3
};

extern Const struct predicate predicate_parser_xlookup__op_4;
static Const struct predinfo predinfo_parser_xlookup__op_4 = {
  (char *)"lookup_op",
  &predicate_parser_xlookup__op_4
};

extern Const struct predicate predicate_parser_xlonger_3;
static Const struct predinfo predinfo_parser_xlonger_3 = {
  (char *)"longer",
  &predicate_parser_xlonger_3
};

extern Const struct predicate predicate_parser_xlist__to__comma__list_2;
static Const struct predinfo predinfo_parser_xlist__to__comma__list_2 = {
  (char *)"list_to_comma_list",
  &predicate_parser_xlist__to__comma__list_2
};

extern Const struct predicate predicate_parser_xlast__check_2;
static Const struct predinfo predinfo_parser_xlast__check_2 = {
  (char *)"last_check",
  &predicate_parser_xlast__check_2
};

extern Const struct predicate predicate_parser_xinfixop_5;
static Const struct predinfo predinfo_parser_xinfixop_5 = {
  (char *)"infixop",
  &predicate_parser_xinfixop_5
};

extern Const struct predicate predicate_parser_xinfixop_2F5_240_5;
static Const struct predinfo predinfo_parser_xinfixop_2F5_240_5 = {
  (char *)"infixop/5$0",
  &predicate_parser_xinfixop_2F5_240_5
};

extern Const struct predicate predicate_parser_xafter__term_6;
static Const struct predinfo predinfo_parser_xafter__term_6 = {
  (char *)"after_term",
  &predicate_parser_xafter__term_6
};

extern Const struct predicate predicate_parser_xafter__term_2F6_242_8;
static Const struct predinfo predinfo_parser_xafter__term_2F6_242_8 = {
  (char *)"after_term/6$2",
  &predicate_parser_xafter__term_2F6_242_8
};

extern Const struct predicate predicate_parser_xafter__term_2F6_242_2F8_241_7;
static Const struct predinfo predinfo_parser_xafter__term_2F6_242_2F8_241_7 = {
  (char *)"after_term/6$2/8$1",
  &predicate_parser_xafter__term_2F6_242_2F8_241_7
};

extern Const struct predicate predicate_parser_xafter__term_2F6_242_2F8_240_9;
static Const struct predinfo predinfo_parser_xafter__term_2F6_242_2F8_240_9 = {
  (char *)"after_term/6$2/8$0",
  &predicate_parser_xafter__term_2F6_242_2F8_240_9
};

extern Const struct predicate predicate_parser_xafter__term_2F6_242_2F8_240_2F9_240_7;
static Const struct predinfo predinfo_parser_xafter__term_2F6_242_2F8_240_2F9_240_7 = {
  (char *)"after_term/6$2/8$0/9$0",
  &predicate_parser_xafter__term_2F6_242_2F8_240_2F9_240_7
};

extern Const struct predicate predicate_parser_xafter__term_2F6_241_5;
static Const struct predinfo predinfo_parser_xafter__term_2F6_241_5 = {
  (char *)"after_term/6$1",
  &predicate_parser_xafter__term_2F6_241_5
};

extern Const struct predicate predicate_parser_xafter__term_2F6_240_5;
static Const struct predinfo predinfo_parser_xafter__term_2F6_240_5 = {
  (char *)"after_term/6$0",
  &predicate_parser_xafter__term_2F6_240_5
};

static Const struct predinfo * Const preds_in_parser[] = {
  &predinfo_parser_xwrap__list_2,
  &predinfo_parser_xwrap__functor_2,
  &predinfo_parser_xwrap__functor1_2,
  &predinfo_parser_xwparse_5,
  &predinfo_parser_xwparse_4,
  &predinfo_parser_xwparse_3,
  &predinfo_parser_xwparse_2F5_244_5,
  &predinfo_parser_xwparse_2F5_244_2F5_242_2,
  &predinfo_parser_xwparse_2F5_244_2F5_241_5,
  &predinfo_parser_xwparse_2F5_244_2F5_240_6,
  &predinfo_parser_xwparse_2F5_244_2F5_240_2F6_240_6,
  &predinfo_parser_xwparse_2F5_243_5,
  &predinfo_parser_xwparse_2F5_242_4,
  &predinfo_parser_xwparse_2F5_242_2F4_240_4,
  &predinfo_parser_xwparse_2F5_241_4,
  &predinfo_parser_xwparse_2F5_241_2F4_240_4,
  &predinfo_parser_xwparse_2F5_240_4,
  &predinfo_parser_xwparse_2F5_240_2F4_240_5,
  &predinfo_parser_xwait__parse_2F2_240_2,
  &predinfo_parser_xwait__parse_2,
  &predinfo_parser_xunwrap_2,
  &predinfo_parser_xselect__better_3,
  &predinfo_parser_xselect__better_2F3_241_4,
  &predinfo_parser_xselect__better_2F3_240_4,
  &predinfo_parser_xprefixop_4,
  &predinfo_parser_xprefixop_2F4_240_4,
  &predinfo_parser_xpostfixop_5,
  &predinfo_parser_xpostfixop_2F5_240_5,
  &predinfo_parser_xparse__comma__list_8,
  &predinfo_parser_xparse__comma__list_6,
  &predinfo_parser_xparse__comma__list_2F8_240_8,
  &predinfo_parser_xparse__comma__list_2F8_240_2F8_240_8,
  &predinfo_parser_xparse__comma__list_2F8_240_2F8_240_2F8_240_5,
  &predinfo_parser_xparse__comma__list_2F8_240_2F8_240_2F8_240_2F5_240_4,
  &predinfo_parser_xparse_3,
  &predinfo_parser_xlookup__op_4,
  &predinfo_parser_xlonger_3,
  &predinfo_parser_xlist__to__comma__list_2,
  &predinfo_parser_xlast__check_2,
  &predinfo_parser_xinfixop_5,
  &predinfo_parser_xinfixop_2F5_240_5,
  &predinfo_parser_xafter__term_6,
  &predinfo_parser_xafter__term_2F6_242_8,
  &predinfo_parser_xafter__term_2F6_242_2F8_241_7,
  &predinfo_parser_xafter__term_2F6_242_2F8_240_9,
  &predinfo_parser_xafter__term_2F6_242_2F8_240_2F9_240_7,
  &predinfo_parser_xafter__term_2F6_241_5,
  &predinfo_parser_xafter__term_2F6_240_5,
  0
};
extern Const struct predicate predicate_atom__table_xset__atom__string_5;
static Const struct predinfo predinfo_atom__table_xset__atom__string_5 = {
  (char *)"set_atom_string",
  &predicate_atom__table_xset__atom__string_5
};

extern Const struct predicate predicate_atom__table_xmake__atom_2;
static Const struct predinfo predinfo_atom__table_xmake__atom_2 = {
  (char *)"make_atom",
  &predicate_atom__table_xmake__atom_2
};

extern Const struct predicate predicate_atom__table_xintern_2;
static Const struct predinfo predinfo_atom__table_xintern_2 = {
  (char *)"intern",
  &predicate_atom__table_xintern_2
};

extern Const struct predicate predicate_atom__table_xget__atom__string_2;
static Const struct predinfo predinfo_atom__table_xget__atom__string_2 = {
  (char *)"get_atom_string",
  &predicate_atom__table_xget__atom__string_2
};

extern Const struct predicate predicate_atom__table_xget__atom__name_2;
static Const struct predinfo predinfo_atom__table_xget__atom__name_2 = {
  (char *)"get_atom_name",
  &predicate_atom__table_xget__atom__name_2
};

extern Const struct predicate predicate_atom__table_xcache__table_1;
static Const struct predinfo predinfo_atom__table_xcache__table_1 = {
  (char *)"cache_table",
  &predicate_atom__table_xcache__table_1
};

extern Const struct predicate predicate_atom__table_xatom__number_2;
static Const struct predinfo predinfo_atom__table_xatom__number_2 = {
  (char *)"atom_number",
  &predicate_atom__table_xatom__number_2
};

static Const struct predinfo * Const preds_in_atom__table[] = {
  &predinfo_atom__table_xset__atom__string_5,
  &predinfo_atom__table_xmake__atom_2,
  &predinfo_atom__table_xintern_2,
  &predinfo_atom__table_xget__atom__string_2,
  &predinfo_atom__table_xget__atom__name_2,
  &predinfo_atom__table_xcache__table_1,
  &predinfo_atom__table_xatom__number_2,
  0
};
extern Const struct predicate predicate_klic__reader_xwrite__tokens_4;
static Const struct predinfo predinfo_klic__reader_xwrite__tokens_4 = {
  (char *)"write_tokens",
  &predicate_klic__reader_xwrite__tokens_4
};

extern Const struct predicate predicate_klic__reader_xwrite__string_3;
static Const struct predinfo predinfo_klic__reader_xwrite__string_3 = {
  (char *)"write_string",
  &predicate_klic__reader_xwrite__string_3
};

extern Const struct predicate predicate_klic__reader_xwrite__one__token_3;
static Const struct predinfo predinfo_klic__reader_xwrite__one__token_3 = {
  (char *)"write_one_token",
  &predicate_klic__reader_xwrite__one__token_3
};

extern Const struct predicate predicate_klic__reader_xwrite__number_3;
static Const struct predinfo predinfo_klic__reader_xwrite__number_3 = {
  (char *)"write_number",
  &predicate_klic__reader_xwrite__number_3
};

extern Const struct predicate predicate_klic__reader_xwrite__decimal_3;
static Const struct predinfo predinfo_klic__reader_xwrite__decimal_3 = {
  (char *)"write_decimal",
  &predicate_klic__reader_xwrite__decimal_3
};

extern Const struct predicate predicate_klic__reader_xwrite__char_3;
static Const struct predinfo predinfo_klic__reader_xwrite__char_3 = {
  (char *)"write_char",
  &predicate_klic__reader_xwrite__char_3
};

extern Const struct predicate predicate_klic__reader_xwrite__atom_3;
static Const struct predinfo predinfo_klic__reader_xwrite__atom_3 = {
  (char *)"write_atom",
  &predicate_klic__reader_xwrite__atom_3
};

extern Const struct predicate predicate_klic__reader_xwparse__one__term__reporting__errors_4;
static Const struct predinfo predinfo_klic__reader_xwparse__one__term__reporting__errors_4 = {
  (char *)"wparse_one_term_reporting_errors",
  &predicate_klic__reader_xwparse__one__term__reporting__errors_4
};

extern Const struct predicate predicate_klic__reader_xwparse__one__term__reporting__errors_2F4_240_8;
static Const struct predinfo predinfo_klic__reader_xwparse__one__term__reporting__errors_2F4_240_8 = {
  (char *)"wparse_one_term_reporting_errors/4$0",
  &predicate_klic__reader_xwparse__one__term__reporting__errors_2F4_240_8
};

extern Const struct predicate predicate_klic__reader_xreport__syntax__error_7;
static Const struct predinfo predinfo_klic__reader_xreport__syntax__error_7 = {
  (char *)"report_syntax_error",
  &predicate_klic__reader_xreport__syntax__error_7
};

extern Const struct predicate predicate_klic__reader_xreport__syntax__error_2F7_240_2;
static Const struct predinfo predinfo_klic__reader_xreport__syntax__error_2F7_240_2 = {
  (char *)"report_syntax_error/7$0",
  &predicate_klic__reader_xreport__syntax__error_2F7_240_2
};

extern Const struct predicate predicate_klic__reader_xparse__one__term__reporting__errors_4;
static Const struct predinfo predinfo_klic__reader_xparse__one__term__reporting__errors_4 = {
  (char *)"parse_one_term_reporting_errors",
  &predicate_klic__reader_xparse__one__term__reporting__errors_4
};

extern Const struct predicate predicate_klic__reader_xparse__one__term__reporting__errors_2F4_240_8;
static Const struct predinfo predinfo_klic__reader_xparse__one__term__reporting__errors_2F4_240_8 = {
  (char *)"parse_one_term_reporting_errors/4$0",
  &predicate_klic__reader_xparse__one__term__reporting__errors_2F4_240_8
};

extern Const struct predicate predicate_klic__reader_xlength_3;
static Const struct predinfo predinfo_klic__reader_xlength_3 = {
  (char *)"length",
  &predicate_klic__reader_xlength_3
};

extern Const struct predicate predicate_klic__reader_xlength_2;
static Const struct predinfo predinfo_klic__reader_xlength_2 = {
  (char *)"length",
  &predicate_klic__reader_xlength_2
};

extern Const struct predicate predicate_klic__reader_xblank_2;
static Const struct predinfo predinfo_klic__reader_xblank_2 = {
  (char *)"blank",
  &predicate_klic__reader_xblank_2
};

static Const struct predinfo * Const preds_in_klic__reader[] = {
  &predinfo_klic__reader_xwrite__tokens_4,
  &predinfo_klic__reader_xwrite__string_3,
  &predinfo_klic__reader_xwrite__one__token_3,
  &predinfo_klic__reader_xwrite__number_3,
  &predinfo_klic__reader_xwrite__decimal_3,
  &predinfo_klic__reader_xwrite__char_3,
  &predinfo_klic__reader_xwrite__atom_3,
  &predinfo_klic__reader_xwparse__one__term__reporting__errors_4,
  &predinfo_klic__reader_xwparse__one__term__reporting__errors_2F4_240_8,
  &predinfo_klic__reader_xreport__syntax__error_7,
  &predinfo_klic__reader_xreport__syntax__error_2F7_240_2,
  &predinfo_klic__reader_xparse__one__term__reporting__errors_4,
  &predinfo_klic__reader_xparse__one__term__reporting__errors_2F4_240_8,
  &predinfo_klic__reader_xlength_3,
  &predinfo_klic__reader_xlength_2,
  &predinfo_klic__reader_xblank_2,
  0
};
extern Const struct predicate predicate_klicio_xupdt_3;
static Const struct predinfo predinfo_klicio_xupdt_3 = {
  (char *)"updt",
  &predicate_klicio_xupdt_3
};

extern Const struct predicate predicate_klicio_xupdate__lc_3;
static Const struct predinfo predinfo_klicio_xupdate__lc_3 = {
  (char *)"update_lc",
  &predicate_klicio_xupdate__lc_3
};

extern Const struct predicate predicate_klicio_xtermupdt_2;
static Const struct predinfo predinfo_klicio_xtermupdt_2 = {
  (char *)"termupdt",
  &predicate_klicio_xtermupdt_2
};

extern Const struct predicate predicate_klicio_xtermout_2;
static Const struct predinfo predinfo_klicio_xtermout_2 = {
  (char *)"termout",
  &predicate_klicio_xtermout_2
};

extern Const struct predicate predicate_klicio_xtermin_2;
static Const struct predinfo predinfo_klicio_xtermin_2 = {
  (char *)"termin",
  &predicate_klicio_xtermin_2
};

extern Const struct predicate predicate_klicio_xstring__output_4;
static Const struct predinfo predinfo_klicio_xstring__output_4 = {
  (char *)"string_output",
  &predicate_klicio_xstring__output_4
};

extern Const struct predicate predicate_klicio_xstring__input_5;
static Const struct predinfo predinfo_klicio_xstring__input_5 = {
  (char *)"string_input",
  &predicate_klicio_xstring__input_5
};

extern Const struct predicate predicate_klicio_xset__string__elements_6;
static Const struct predinfo predinfo_klicio_xset__string__elements_6 = {
  (char *)"set_string_elements",
  &predicate_klicio_xset__string__elements_6
};

extern Const struct predicate predicate_klicio_xsend__unix_3;
static Const struct predinfo predinfo_klicio_xsend__unix_3 = {
  (char *)"send_unix",
  &predicate_klicio_xsend__unix_3
};

extern Const struct predicate predicate_klicio_xrmop_4;
static Const struct predinfo predinfo_klicio_xrmop_4 = {
  (char *)"rmop",
  &predicate_klicio_xrmop_4
};

extern Const struct predicate predicate_klicio_xrmop2_4;
static Const struct predinfo predinfo_klicio_xrmop2_4 = {
  (char *)"rmop2",
  &predicate_klicio_xrmop2_4
};

extern Const struct predicate predicate_klicio_xoutput__stream_2;
static Const struct predinfo predinfo_klicio_xoutput__stream_2 = {
  (char *)"output_stream",
  &predicate_klicio_xoutput__stream_2
};

extern Const struct predicate predicate_klicio_xout_3;
static Const struct predinfo predinfo_klicio_xout_3 = {
  (char *)"out",
  &predicate_klicio_xout_3
};

extern Const struct predicate predicate_klicio_xklicio_2;
static Const struct predinfo predinfo_klicio_xklicio_2 = {
  (char *)"klicio",
  &predicate_klicio_xklicio_2
};

extern Const struct predicate predicate_klicio_xklicio_1;
static Const struct predinfo predinfo_klicio_xklicio_1 = {
  (char *)"klicio",
  &predicate_klicio_xklicio_1
};

extern Const struct predicate predicate_klicio_xin_3;
static Const struct predinfo predinfo_klicio_xin_3 = {
  (char *)"in",
  &predicate_klicio_xin_3
};

extern Const struct predicate predicate_klicio_xfill__string_4;
static Const struct predinfo predinfo_klicio_xfill__string_4 = {
  (char *)"fill_string",
  &predicate_klicio_xfill__string_4
};

extern Const struct predicate predicate_klicio_xdefault__operator__table_1;
static Const struct predinfo predinfo_klicio_xdefault__operator__table_1 = {
  (char *)"default_operator_table",
  &predicate_klicio_xdefault__operator__table_1
};

extern Const struct predicate predicate_klicio_xcompute__string__length_3;
static Const struct predinfo predinfo_klicio_xcompute__string__length_3 = {
  (char *)"compute_string_length",
  &predicate_klicio_xcompute__string__length_3
};

extern Const struct predicate predicate_klicio_xclose__unix_1;
static Const struct predinfo predinfo_klicio_xclose__unix_1 = {
  (char *)"close_unix",
  &predicate_klicio_xclose__unix_1
};

extern Const struct predicate predicate_klicio_xbound__sock_2;
static Const struct predinfo predinfo_klicio_xbound__sock_2 = {
  (char *)"bound_sock",
  &predicate_klicio_xbound__sock_2
};

extern Const struct predicate predicate_klicio_xbind_2;
static Const struct predinfo predinfo_klicio_xbind_2 = {
  (char *)"bind",
  &predicate_klicio_xbind_2
};

extern Const struct predicate predicate_klicio_xaddop_5;
static Const struct predinfo predinfo_klicio_xaddop_5 = {
  (char *)"addop",
  &predicate_klicio_xaddop_5
};

extern Const struct predicate predicate_klicio_xaddop2_5;
static Const struct predinfo predinfo_klicio_xaddop2_5 = {
  (char *)"addop2",
  &predicate_klicio_xaddop2_5
};

static Const struct predinfo * Const preds_in_klicio[] = {
  &predinfo_klicio_xupdt_3,
  &predinfo_klicio_xupdate__lc_3,
  &predinfo_klicio_xtermupdt_2,
  &predinfo_klicio_xtermout_2,
  &predinfo_klicio_xtermin_2,
  &predinfo_klicio_xstring__output_4,
  &predinfo_klicio_xstring__input_5,
  &predinfo_klicio_xset__string__elements_6,
  &predinfo_klicio_xsend__unix_3,
  &predinfo_klicio_xrmop_4,
  &predinfo_klicio_xrmop2_4,
  &predinfo_klicio_xoutput__stream_2,
  &predinfo_klicio_xout_3,
  &predinfo_klicio_xklicio_2,
  &predinfo_klicio_xklicio_1,
  &predinfo_klicio_xin_3,
  &predinfo_klicio_xfill__string_4,
  &predinfo_klicio_xdefault__operator__table_1,
  &predinfo_klicio_xcompute__string__length_3,
  &predinfo_klicio_xclose__unix_1,
  &predinfo_klicio_xbound__sock_2,
  &predinfo_klicio_xbind_2,
  &predinfo_klicio_xaddop_5,
  &predinfo_klicio_xaddop2_5,
  0
};
extern Const struct predicate predicate_klic__comp__message_xwrite__message_5;
static Const struct predinfo predinfo_klic__comp__message_xwrite__message_5 = {
  (char *)"write_message",
  &predicate_klic__comp__message_xwrite__message_5
};

extern Const struct predicate predicate_klic__comp__message_xwrite__message_3;
static Const struct predinfo predinfo_klic__comp__message_xwrite__message_3 = {
  (char *)"write_message",
  &predicate_klic__comp__message_xwrite__message_3
};

extern Const struct predicate predicate_klic__comp__message_xwrite__message2_8;
static Const struct predinfo predinfo_klic__comp__message_xwrite__message2_8 = {
  (char *)"write_message2",
  &predicate_klic__comp__message_xwrite__message2_8
};

extern Const struct predicate predicate_klic__comp__message_xwrite__message1_7;
static Const struct predinfo predinfo_klic__comp__message_xwrite__message1_7 = {
  (char *)"write_message1",
  &predicate_klic__comp__message_xwrite__message1_7
};

extern Const struct predicate predicate_klic__comp__message_xwarning_4;
static Const struct predinfo predinfo_klic__comp__message_xwarning_4 = {
  (char *)"warning",
  &predicate_klic__comp__message_xwarning_4
};

extern Const struct predicate predicate_klic__comp__message_xwarning_2;
static Const struct predinfo predinfo_klic__comp__message_xwarning_2 = {
  (char *)"warning",
  &predicate_klic__comp__message_xwarning_2
};

extern Const struct predicate predicate_klic__comp__message_xtell_2;
static Const struct predinfo predinfo_klic__comp__message_xtell_2 = {
  (char *)"tell",
  &predicate_klic__comp__message_xtell_2
};

extern Const struct predicate predicate_klic__comp__message_xtell1_3;
static Const struct predinfo predinfo_klic__comp__message_xtell1_3 = {
  (char *)"tell1",
  &predicate_klic__comp__message_xtell1_3
};

extern Const struct predicate predicate_klic__comp__message_xsee__2_3;
static Const struct predinfo predinfo_klic__comp__message_xsee__2_3 = {
  (char *)"see_2",
  &predicate_klic__comp__message_xsee__2_3
};

extern Const struct predicate predicate_klic__comp__message_xsee_2;
static Const struct predinfo predinfo_klic__comp__message_xsee_2 = {
  (char *)"see",
  &predicate_klic__comp__message_xsee_2
};

extern Const struct predicate predicate_klic__comp__message_xreport__error_4;
static Const struct predinfo predinfo_klic__comp__message_xreport__error_4 = {
  (char *)"report_error",
  &predicate_klic__comp__message_xreport__error_4
};

extern Const struct predicate predicate_klic__comp__message_xreport__error_2;
static Const struct predinfo predinfo_klic__comp__message_xreport__error_2 = {
  (char *)"report_error",
  &predicate_klic__comp__message_xreport__error_2
};

extern Const struct predicate predicate_klic__comp__message_xcompilation__error_1;
static Const struct predinfo predinfo_klic__comp__message_xcompilation__error_1 = {
  (char *)"compilation_error",
  &predicate_klic__comp__message_xcompilation__error_1
};

extern Const struct predicate predicate_klic__comp__message_xcompilation__error_0;
static Const struct predinfo predinfo_klic__comp__message_xcompilation__error_0 = {
  (char *)"compilation_error",
  &predicate_klic__comp__message_xcompilation__error_0
};

static Const struct predinfo * Const preds_in_klic__comp__message[] = {
  &predinfo_klic__comp__message_xwrite__message_5,
  &predinfo_klic__comp__message_xwrite__message_3,
  &predinfo_klic__comp__message_xwrite__message2_8,
  &predinfo_klic__comp__message_xwrite__message1_7,
  &predinfo_klic__comp__message_xwarning_4,
  &predinfo_klic__comp__message_xwarning_2,
  &predinfo_klic__comp__message_xtell_2,
  &predinfo_klic__comp__message_xtell1_3,
  &predinfo_klic__comp__message_xsee__2_3,
  &predinfo_klic__comp__message_xsee_2,
  &predinfo_klic__comp__message_xreport__error_4,
  &predinfo_klic__comp__message_xreport__error_2,
  &predinfo_klic__comp__message_xcompilation__error_1,
  &predinfo_klic__comp__message_xcompilation__error_0,
  0
};
extern Const struct predicate predicate_functor__table_xsetarg_5;
static Const struct predinfo predinfo_functor__table_xsetarg_5 = {
  (char *)"setarg",
  &predicate_functor__table_xsetarg_5
};

extern Const struct predicate predicate_functor__table_xsetarg_4;
static Const struct predinfo predinfo_functor__table_xsetarg_4 = {
  (char *)"setarg",
  &predicate_functor__table_xsetarg_4
};

extern Const struct predicate predicate_functor__table_xnew__functor_3;
static Const struct predinfo predinfo_functor__table_xnew__functor_3 = {
  (char *)"new_functor",
  &predicate_functor__table_xnew__functor_3
};

extern Const struct predicate predicate_functor__table_xlength_3;
static Const struct predinfo predinfo_functor__table_xlength_3 = {
  (char *)"length",
  &predicate_functor__table_xlength_3
};

extern Const struct predicate predicate_functor__table_xfunctor_4;
static Const struct predinfo predinfo_functor__table_xfunctor_4 = {
  (char *)"functor",
  &predicate_functor__table_xfunctor_4
};

extern Const struct predicate predicate_functor__table_xfunctor_3;
static Const struct predinfo predinfo_functor__table_xfunctor_3 = {
  (char *)"functor",
  &predicate_functor__table_xfunctor_3
};

extern Const struct predicate predicate_functor__table_xfill__args_4;
static Const struct predinfo predinfo_functor__table_xfill__args_4 = {
  (char *)"fill_args",
  &predicate_functor__table_xfill__args_4
};

extern Const struct predicate predicate_functor__table_xarg_4;
static Const struct predinfo predinfo_functor__table_xarg_4 = {
  (char *)"arg",
  &predicate_functor__table_xarg_4
};

extern Const struct predicate predicate_functor__table_xarg_3;
static Const struct predinfo predinfo_functor__table_xarg_3 = {
  (char *)"arg",
  &predicate_functor__table_xarg_3
};

extern Const struct predicate predicate_functor__table_x_3D_2E_2E_2;
static Const struct predinfo predinfo_functor__table_x_3D_2E_2E_2 = {
  (char *)"=..",
  &predicate_functor__table_x_3D_2E_2E_2
};

static Const struct predinfo * Const preds_in_functor__table[] = {
  &predinfo_functor__table_xsetarg_5,
  &predinfo_functor__table_xsetarg_4,
  &predinfo_functor__table_xnew__functor_3,
  &predinfo_functor__table_xlength_3,
  &predinfo_functor__table_xfunctor_4,
  &predinfo_functor__table_xfunctor_3,
  &predinfo_functor__table_xfill__args_4,
  &predinfo_functor__table_xarg_4,
  &predinfo_functor__table_xarg_3,
  &predinfo_functor__table_x_3D_2E_2E_2,
  0
};
extern Const struct predicate predicate_klic__comp__kl1cmp_xwrite__inline__header_3;
static Const struct predinfo predinfo_klic__comp__kl1cmp_xwrite__inline__header_3 = {
  (char *)"write_inline_header",
  &predicate_klic__comp__kl1cmp_xwrite__inline__header_3
};

extern Const struct predicate predicate_klic__comp__kl1cmp_xwrite__file__header_2;
static Const struct predinfo predinfo_klic__comp__kl1cmp_xwrite__file__header_2 = {
  (char *)"write_file_header",
  &predicate_klic__comp__kl1cmp_xwrite__file__header_2
};

extern Const struct predicate predicate_klic__comp__kl1cmp_xwait__output__end_3;
static Const struct predinfo predinfo_klic__comp__kl1cmp_xwait__output__end_3 = {
  (char *)"wait_output_end",
  &predicate_klic__comp__kl1cmp_xwait__output__end_3
};

extern Const struct predicate predicate_klic__comp__kl1cmp_xshift__vector_5;
static Const struct predinfo predinfo_klic__comp__kl1cmp_xshift__vector_5 = {
  (char *)"shift_vector",
  &predicate_klic__comp__kl1cmp_xshift__vector_5
};

extern Const struct predicate predicate_klic__comp__kl1cmp_xshift__vector_3;
static Const struct predinfo predinfo_klic__comp__kl1cmp_xshift__vector_3 = {
  (char *)"shift_vector",
  &predicate_klic__comp__kl1cmp_xshift__vector_3
};

extern Const struct predicate predicate_klic__comp__kl1cmp_xshift__vector_2;
static Const struct predinfo predinfo_klic__comp__kl1cmp_xshift__vector_2 = {
  (char *)"shift_vector",
  &predicate_klic__comp__kl1cmp_xshift__vector_2
};

extern Const struct predicate predicate_klic__comp__kl1cmp_xrename__macro__preds_5;
static Const struct predinfo predinfo_klic__comp__kl1cmp_xrename__macro__preds_5 = {
  (char *)"rename_macro_preds",
  &predicate_klic__comp__kl1cmp_xrename__macro__preds_5
};

extern Const struct predicate predicate_klic__comp__kl1cmp_xrename__macro__clauses_3;
static Const struct predinfo predinfo_klic__comp__kl1cmp_xrename__macro__clauses_3 = {
  (char *)"rename_macro_clauses",
  &predicate_klic__comp__kl1cmp_xrename__macro__clauses_3
};

extern Const struct predicate predicate_klic__comp__kl1cmp_xread__one__module_7;
static Const struct predinfo predinfo_klic__comp__kl1cmp_xread__one__module_7 = {
  (char *)"read_one_module",
  &predicate_klic__comp__kl1cmp_xread__one__module_7
};

extern Const struct predicate predicate_klic__comp__kl1cmp_xread__inline_7;
static Const struct predinfo predinfo_klic__comp__kl1cmp_xread__inline_7 = {
  (char *)"read_inline",
  &predicate_klic__comp__kl1cmp_xread__inline_7
};

extern Const struct predicate predicate_klic__comp__kl1cmp_xread__in_5;
static Const struct predinfo predinfo_klic__comp__kl1cmp_xread__in_5 = {
  (char *)"read_in",
  &predicate_klic__comp__kl1cmp_xread__in_5
};

extern Const struct predicate predicate_klic__comp__kl1cmp_xread__all__1_6;
static Const struct predinfo predinfo_klic__comp__kl1cmp_xread__all__1_6 = {
  (char *)"read_all_1",
  &predicate_klic__comp__kl1cmp_xread__all__1_6
};

extern Const struct predicate predicate_klic__comp__kl1cmp_xread__all_6;
static Const struct predinfo predinfo_klic__comp__kl1cmp_xread__all_6 = {
  (char *)"read_all",
  &predicate_klic__comp__kl1cmp_xread__all_6
};

extern Const struct predicate predicate_klic__comp__kl1cmp_xklcmp_2F2_240_4;
static Const struct predinfo predinfo_klic__comp__kl1cmp_xklcmp_2F2_240_4 = {
  (char *)"klcmp/2$0",
  &predicate_klic__comp__kl1cmp_xklcmp_2F2_240_4
};

extern Const struct predicate predicate_klic__comp__kl1cmp_xklcmp_2;
static Const struct predinfo predinfo_klic__comp__kl1cmp_xklcmp_2 = {
  (char *)"klcmp",
  &predicate_klic__comp__kl1cmp_xklcmp_2
};

extern Const struct predicate predicate_klic__comp__kl1cmp_xklcmp1_4;
static Const struct predinfo predinfo_klic__comp__kl1cmp_xklcmp1_4 = {
  (char *)"klcmp1",
  &predicate_klic__comp__kl1cmp_xklcmp1_4
};

extern Const struct predicate predicate_klic__comp__kl1cmp_xhandle__exception_3;
static Const struct predinfo predinfo_klic__comp__kl1cmp_xhandle__exception_3 = {
  (char *)"handle_exception",
  &predicate_klic__comp__kl1cmp_xhandle__exception_3
};

extern Const struct predicate predicate_klic__comp__kl1cmp_xhandle__exception_1;
static Const struct predinfo predinfo_klic__comp__kl1cmp_xhandle__exception_1 = {
  (char *)"handle_exception",
  &predicate_klic__comp__kl1cmp_xhandle__exception_1
};

extern Const struct predicate predicate_klic__comp__kl1cmp_xget__clause__name_3;
static Const struct predinfo predinfo_klic__comp__kl1cmp_xget__clause__name_3 = {
  (char *)"get_clause_name",
  &predicate_klic__comp__kl1cmp_xget__clause__name_3
};

extern Const struct predicate predicate_klic__comp__kl1cmp_xcomplain__words_3;
static Const struct predinfo predinfo_klic__comp__kl1cmp_xcomplain__words_3 = {
  (char *)"complain_words",
  &predicate_klic__comp__kl1cmp_xcomplain__words_3
};

extern Const struct predicate predicate_klic__comp__kl1cmp_xcomplain__tokens_6;
static Const struct predinfo predinfo_klic__comp__kl1cmp_xcomplain__tokens_6 = {
  (char *)"complain_tokens",
  &predicate_klic__comp__kl1cmp_xcomplain__tokens_6
};

extern Const struct predicate predicate_klic__comp__kl1cmp_xcomplain__tokens_3;
static Const struct predinfo predinfo_klic__comp__kl1cmp_xcomplain__tokens_3 = {
  (char *)"complain_tokens",
  &predicate_klic__comp__kl1cmp_xcomplain__tokens_3
};

extern Const struct predicate predicate_klic__comp__kl1cmp_xcomplain__token_3;
static Const struct predinfo predinfo_klic__comp__kl1cmp_xcomplain__token_3 = {
  (char *)"complain_token",
  &predicate_klic__comp__kl1cmp_xcomplain__token_3
};

extern Const struct predicate predicate_klic__comp__kl1cmp_xcomplain__string_3;
static Const struct predinfo predinfo_klic__comp__kl1cmp_xcomplain__string_3 = {
  (char *)"complain_string",
  &predicate_klic__comp__kl1cmp_xcomplain__string_3
};

extern Const struct predicate predicate_klic__comp__kl1cmp_xcompile__modules_6;
static Const struct predinfo predinfo_klic__comp__kl1cmp_xcompile__modules_6 = {
  (char *)"compile_modules",
  &predicate_klic__comp__kl1cmp_xcompile__modules_6
};

extern Const struct predicate predicate_klic__comp__kl1cmp_xcompile_4;
static Const struct predinfo predinfo_klic__comp__kl1cmp_xcompile_4 = {
  (char *)"compile",
  &predicate_klic__comp__kl1cmp_xcompile_4
};

extern Const struct predicate predicate_klic__comp__kl1cmp_xcompile2_6;
static Const struct predinfo predinfo_klic__comp__kl1cmp_xcompile2_6 = {
  (char *)"compile2",
  &predicate_klic__comp__kl1cmp_xcompile2_6
};

extern Const struct predicate predicate_klic__comp__kl1cmp_xcompile1_4;
static Const struct predinfo predinfo_klic__comp__kl1cmp_xcompile1_4 = {
  (char *)"compile1",
  &predicate_klic__comp__kl1cmp_xcompile1_4
};

extern Const struct predicate predicate_klic__comp__kl1cmp_xcollect__same__1_10;
static Const struct predinfo predinfo_klic__comp__kl1cmp_xcollect__same__1_10 = {
  (char *)"collect_same_1",
  &predicate_klic__comp__kl1cmp_xcollect__same__1_10
};

extern Const struct predicate predicate_klic__comp__kl1cmp_xcollect__same_8;
static Const struct predinfo predinfo_klic__comp__kl1cmp_xcollect__same_8 = {
  (char *)"collect_same",
  &predicate_klic__comp__kl1cmp_xcollect__same_8
};

extern Const struct predicate predicate_klic__comp__kl1cmp_xclose__streams_1;
static Const struct predinfo predinfo_klic__comp__kl1cmp_xclose__streams_1 = {
  (char *)"close_streams",
  &predicate_klic__comp__kl1cmp_xclose__streams_1
};

extern Const struct predicate predicate_klic__comp__kl1cmp_xanalyze__preds_6;
static Const struct predinfo predinfo_klic__comp__kl1cmp_xanalyze__preds_6 = {
  (char *)"analyze_preds",
  &predicate_klic__comp__kl1cmp_xanalyze__preds_6
};

extern Const struct predicate predicate_klic__comp__kl1cmp_xanalyze_2;
static Const struct predinfo predinfo_klic__comp__kl1cmp_xanalyze_2 = {
  (char *)"analyze",
  &predicate_klic__comp__kl1cmp_xanalyze_2
};

extern Const struct predicate predicate_klic__comp__kl1cmp_xanalyze1_6;
static Const struct predinfo predinfo_klic__comp__kl1cmp_xanalyze1_6 = {
  (char *)"analyze1",
  &predicate_klic__comp__kl1cmp_xanalyze1_6
};

static Const struct predinfo * Const preds_in_klic__comp__kl1cmp[] = {
  &predinfo_klic__comp__kl1cmp_xwrite__inline__header_3,
  &predinfo_klic__comp__kl1cmp_xwrite__file__header_2,
  &predinfo_klic__comp__kl1cmp_xwait__output__end_3,
  &predinfo_klic__comp__kl1cmp_xshift__vector_5,
  &predinfo_klic__comp__kl1cmp_xshift__vector_3,
  &predinfo_klic__comp__kl1cmp_xshift__vector_2,
  &predinfo_klic__comp__kl1cmp_xrename__macro__preds_5,
  &predinfo_klic__comp__kl1cmp_xrename__macro__clauses_3,
  &predinfo_klic__comp__kl1cmp_xread__one__module_7,
  &predinfo_klic__comp__kl1cmp_xread__inline_7,
  &predinfo_klic__comp__kl1cmp_xread__in_5,
  &predinfo_klic__comp__kl1cmp_xread__all__1_6,
  &predinfo_klic__comp__kl1cmp_xread__all_6,
  &predinfo_klic__comp__kl1cmp_xklcmp_2F2_240_4,
  &predinfo_klic__comp__kl1cmp_xklcmp_2,
  &predinfo_klic__comp__kl1cmp_xklcmp1_4,
  &predinfo_klic__comp__kl1cmp_xhandle__exception_3,
  &predinfo_klic__comp__kl1cmp_xhandle__exception_1,
  &predinfo_klic__comp__kl1cmp_xget__clause__name_3,
  &predinfo_klic__comp__kl1cmp_xcomplain__words_3,
  &predinfo_klic__comp__kl1cmp_xcomplain__tokens_6,
  &predinfo_klic__comp__kl1cmp_xcomplain__tokens_3,
  &predinfo_klic__comp__kl1cmp_xcomplain__token_3,
  &predinfo_klic__comp__kl1cmp_xcomplain__string_3,
  &predinfo_klic__comp__kl1cmp_xcompile__modules_6,
  &predinfo_klic__comp__kl1cmp_xcompile_4,
  &predinfo_klic__comp__kl1cmp_xcompile2_6,
  &predinfo_klic__comp__kl1cmp_xcompile1_4,
  &predinfo_klic__comp__kl1cmp_xcollect__same__1_10,
  &predinfo_klic__comp__kl1cmp_xcollect__same_8,
  &predinfo_klic__comp__kl1cmp_xclose__streams_1,
  &predinfo_klic__comp__kl1cmp_xanalyze__preds_6,
  &predinfo_klic__comp__kl1cmp_xanalyze_2,
  &predinfo_klic__comp__kl1cmp_xanalyze1_6,
  0
};
extern Const struct predicate predicate_timer_xtimer__loop_5;
static Const struct predinfo predinfo_timer_xtimer__loop_5 = {
  (char *)"timer_loop",
  &predicate_timer_xtimer__loop_5
};

extern Const struct predicate predicate_timer_xsub_3;
static Const struct predinfo predinfo_timer_xsub_3 = {
  (char *)"sub",
  &predicate_timer_xsub_3
};

extern Const struct predicate predicate_timer_xinstantiate__every_3;
static Const struct predinfo predinfo_timer_xinstantiate__every_3 = {
  (char *)"instantiate_every",
  &predicate_timer_xinstantiate__every_3
};

extern Const struct predicate predicate_timer_xinstantiate__at_2;
static Const struct predinfo predinfo_timer_xinstantiate__at_2 = {
  (char *)"instantiate_at",
  &predicate_timer_xinstantiate__at_2
};

extern Const struct predicate predicate_timer_xinstantiate__after_3;
static Const struct predinfo predinfo_timer_xinstantiate__after_3 = {
  (char *)"instantiate_after",
  &predicate_timer_xinstantiate__after_3
};

extern Const struct predicate predicate_timer_xinstantiate__after_2;
static Const struct predinfo predinfo_timer_xinstantiate__after_2 = {
  (char *)"instantiate_after",
  &predicate_timer_xinstantiate__after_2
};

extern Const struct predicate predicate_timer_xget__time__of__day_1;
static Const struct predinfo predinfo_timer_xget__time__of__day_1 = {
  (char *)"get_time_of_day",
  &predicate_timer_xget__time__of__day_1
};

extern Const struct predicate predicate_timer_xcompare_3;
static Const struct predinfo predinfo_timer_xcompare_3 = {
  (char *)"compare",
  &predicate_timer_xcompare_3
};

extern Const struct predicate predicate_timer_xadd_3;
static Const struct predinfo predinfo_timer_xadd_3 = {
  (char *)"add",
  &predicate_timer_xadd_3
};

static Const struct predinfo * Const preds_in_timer[] = {
  &predinfo_timer_xtimer__loop_5,
  &predinfo_timer_xsub_3,
  &predinfo_timer_xinstantiate__every_3,
  &predinfo_timer_xinstantiate__at_2,
  &predinfo_timer_xinstantiate__after_3,
  &predinfo_timer_xinstantiate__after_2,
  &predinfo_timer_xget__time__of__day_1,
  &predinfo_timer_xcompare_3,
  &predinfo_timer_xadd_3,
  0
};
extern Const struct predicate predicate_unix_xwrite__open_2;
static Const struct predinfo predinfo_unix_xwrite__open_2 = {
  (char *)"write_open",
  &predicate_unix_xwrite__open_2
};

extern Const struct predicate predicate_unix_xupdate__open_2;
static Const struct predinfo predinfo_unix_xupdate__open_2 = {
  (char *)"update_open",
  &predicate_unix_xupdate__open_2
};

extern Const struct predicate predicate_unix_xunlink_2;
static Const struct predinfo predinfo_unix_xunlink_2 = {
  (char *)"unlink",
  &predicate_unix_xunlink_2
};

extern Const struct predicate predicate_unix_xunix_1;
static Const struct predinfo predinfo_unix_xunix_1 = {
  (char *)"unix",
  &predicate_unix_xunix_1
};

extern Const struct predicate predicate_unix_xumask_2;
static Const struct predinfo predinfo_unix_xumask_2 = {
  (char *)"umask",
  &predicate_unix_xumask_2
};

extern Const struct predicate predicate_unix_xumask_1;
static Const struct predinfo predinfo_unix_xumask_1 = {
  (char *)"umask",
  &predicate_unix_xumask_1
};

extern Const struct predicate predicate_unix_xtimes_4;
static Const struct predinfo predinfo_unix_xtimes_4 = {
  (char *)"times",
  &predicate_unix_xtimes_4
};

extern Const struct predicate predicate_unix_xsystem_2;
static Const struct predinfo predinfo_unix_xsystem_2 = {
  (char *)"system",
  &predicate_unix_xsystem_2
};

extern Const struct predicate predicate_unix_xstdout_1;
static Const struct predinfo predinfo_unix_xstdout_1 = {
  (char *)"stdout",
  &predicate_unix_xstdout_1
};

extern Const struct predicate predicate_unix_xstdio_1;
static Const struct predinfo predinfo_unix_xstdio_1 = {
  (char *)"stdio",
  &predicate_unix_xstdio_1
};

extern Const struct predicate predicate_unix_xstdin_1;
static Const struct predinfo predinfo_unix_xstdin_1 = {
  (char *)"stdin",
  &predicate_unix_xstdin_1
};

extern Const struct predicate predicate_unix_xstderr_1;
static Const struct predinfo predinfo_unix_xstderr_1 = {
  (char *)"stderr",
  &predicate_unix_xstderr_1
};

extern Const struct predicate predicate_unix_xsignal__stream_3;
static Const struct predinfo predinfo_unix_xsignal__stream_3 = {
  (char *)"signal_stream",
  &predicate_unix_xsignal__stream_3
};

extern Const struct predicate predicate_unix_xsignal__stream_2;
static Const struct predinfo predinfo_unix_xsignal__stream_2 = {
  (char *)"signal_stream",
  &predicate_unix_xsignal__stream_2
};

extern Const struct predicate predicate_unix_xread__open_2;
static Const struct predinfo predinfo_unix_xread__open_2 = {
  (char *)"read_open",
  &predicate_unix_xread__open_2
};

extern Const struct predicate predicate_unix_xputenv_2;
static Const struct predinfo predinfo_unix_xputenv_2 = {
  (char *)"putenv",
  &predicate_unix_xputenv_2
};

extern Const struct predicate predicate_unix_xpipe_1;
static Const struct predinfo predinfo_unix_xpipe_1 = {
  (char *)"pipe",
  &predicate_unix_xpipe_1
};

extern Const struct predicate predicate_unix_xnet__convert_3;
static Const struct predinfo predinfo_unix_xnet__convert_3 = {
  (char *)"net_convert",
  &predicate_unix_xnet__convert_3
};

extern Const struct predicate predicate_unix_xmktemp_2;
static Const struct predinfo predinfo_unix_xmktemp_2 = {
  (char *)"mktemp",
  &predicate_unix_xmktemp_2
};

extern Const struct predicate predicate_unix_xmake__argv__list_3;
static Const struct predinfo predinfo_unix_xmake__argv__list_3 = {
  (char *)"make_argv_list",
  &predicate_unix_xmake__argv__list_3
};

extern Const struct predicate predicate_unix_xkill_3;
static Const struct predinfo predinfo_unix_xkill_3 = {
  (char *)"kill",
  &predicate_unix_xkill_3
};

extern Const struct predicate predicate_unix_xgetenv_2;
static Const struct predinfo predinfo_unix_xgetenv_2 = {
  (char *)"getenv",
  &predicate_unix_xgetenv_2
};

extern Const struct predicate predicate_unix_xfork__with__pipes_2F1_240_6;
static Const struct predinfo predinfo_unix_xfork__with__pipes_2F1_240_6 = {
  (char *)"fork_with_pipes/1$0",
  &predicate_unix_xfork__with__pipes_2F1_240_6
};

extern Const struct predicate predicate_unix_xfork__with__pipes_1;
static Const struct predinfo predinfo_unix_xfork__with__pipes_1 = {
  (char *)"fork_with_pipes",
  &predicate_unix_xfork__with__pipes_1
};

extern Const struct predicate predicate_unix_xfork_1;
static Const struct predinfo predinfo_unix_xfork_1 = {
  (char *)"fork",
  &predicate_unix_xfork_1
};

extern Const struct predicate predicate_unix_xexit_1;
static Const struct predinfo predinfo_unix_xexit_1 = {
  (char *)"exit",
  &predicate_unix_xexit_1
};

extern Const struct predicate predicate_unix_xdummy_0;
static Const struct predinfo predinfo_unix_xdummy_0 = {
  (char *)"dummy",
  &predicate_unix_xdummy_0
};

extern Const struct predicate predicate_unix_xcont_3;
static Const struct predinfo predinfo_unix_xcont_3 = {
  (char *)"cont",
  &predicate_unix_xcont_3
};

extern Const struct predicate predicate_unix_xconnect__sub_5;
static Const struct predinfo predinfo_unix_xconnect__sub_5 = {
  (char *)"connect_sub",
  &predicate_unix_xconnect__sub_5
};

extern Const struct predicate predicate_unix_xconnect__sub_4;
static Const struct predinfo predinfo_unix_xconnect__sub_4 = {
  (char *)"connect_sub",
  &predicate_unix_xconnect__sub_4
};

extern Const struct predicate predicate_unix_xconnect_4;
static Const struct predinfo predinfo_unix_xconnect_4 = {
  (char *)"connect",
  &predicate_unix_xconnect_4
};

extern Const struct predicate predicate_unix_xconnect_2;
static Const struct predinfo predinfo_unix_xconnect_2 = {
  (char *)"connect",
  &predicate_unix_xconnect_2
};

extern Const struct predicate predicate_unix_xconnect2__sub_5;
static Const struct predinfo predinfo_unix_xconnect2__sub_5 = {
  (char *)"connect2_sub",
  &predicate_unix_xconnect2__sub_5
};

extern Const struct predicate predicate_unix_xconnect2__sub_4;
static Const struct predinfo predinfo_unix_xconnect2__sub_4 = {
  (char *)"connect2_sub",
  &predicate_unix_xconnect2__sub_4
};

extern Const struct predicate predicate_unix_xconnect2_4;
static Const struct predinfo predinfo_unix_xconnect2_4 = {
  (char *)"connect2",
  &predicate_unix_xconnect2_4
};

extern Const struct predicate predicate_unix_xconnect2_2;
static Const struct predinfo predinfo_unix_xconnect2_2 = {
  (char *)"connect2",
  &predicate_unix_xconnect2_2
};

extern Const struct predicate predicate_unix_xchmod_3;
static Const struct predinfo predinfo_unix_xchmod_3 = {
  (char *)"chmod",
  &predicate_unix_xchmod_3
};

extern Const struct predicate predicate_unix_xcd_2;
static Const struct predinfo predinfo_unix_xcd_2 = {
  (char *)"cd",
  &predicate_unix_xcd_2
};

extern Const struct predicate predicate_unix_xbound__sock_4;
static Const struct predinfo predinfo_unix_xbound__sock_4 = {
  (char *)"bound_sock",
  &predicate_unix_xbound__sock_4
};

extern Const struct predicate predicate_unix_xbind_4;
static Const struct predinfo predinfo_unix_xbind_4 = {
  (char *)"bind",
  &predicate_unix_xbind_4
};

extern Const struct predicate predicate_unix_xbind_2;
static Const struct predinfo predinfo_unix_xbind_2 = {
  (char *)"bind",
  &predicate_unix_xbind_2
};

extern Const struct predicate predicate_unix_xasync__output_4;
static Const struct predinfo predinfo_unix_xasync__output_4 = {
  (char *)"async_output",
  &predicate_unix_xasync__output_4
};

extern Const struct predicate predicate_unix_xasync__io_5;
static Const struct predinfo predinfo_unix_xasync__io_5 = {
  (char *)"async_io",
  &predicate_unix_xasync__io_5
};

extern Const struct predicate predicate_unix_xasync__input_4;
static Const struct predinfo predinfo_unix_xasync__input_4 = {
  (char *)"async_input",
  &predicate_unix_xasync__input_4
};

extern Const struct predicate predicate_unix_xargv_1;
static Const struct predinfo predinfo_unix_xargv_1 = {
  (char *)"argv",
  &predicate_unix_xargv_1
};

extern Const struct predicate predicate_unix_xargc_1;
static Const struct predinfo predinfo_unix_xargc_1 = {
  (char *)"argc",
  &predicate_unix_xargc_1
};

extern Const struct predicate predicate_unix_xappend__open_2;
static Const struct predinfo predinfo_unix_xappend__open_2 = {
  (char *)"append_open",
  &predicate_unix_xappend__open_2
};

extern Const struct predicate predicate_unix_xaccess_3;
static Const struct predinfo predinfo_unix_xaccess_3 = {
  (char *)"access",
  &predicate_unix_xaccess_3
};

static Const struct predinfo * Const preds_in_unix[] = {
  &predinfo_unix_xwrite__open_2,
  &predinfo_unix_xupdate__open_2,
  &predinfo_unix_xunlink_2,
  &predinfo_unix_xunix_1,
  &predinfo_unix_xumask_2,
  &predinfo_unix_xumask_1,
  &predinfo_unix_xtimes_4,
  &predinfo_unix_xsystem_2,
  &predinfo_unix_xstdout_1,
  &predinfo_unix_xstdio_1,
  &predinfo_unix_xstdin_1,
  &predinfo_unix_xstderr_1,
  &predinfo_unix_xsignal__stream_3,
  &predinfo_unix_xsignal__stream_2,
  &predinfo_unix_xread__open_2,
  &predinfo_unix_xputenv_2,
  &predinfo_unix_xpipe_1,
  &predinfo_unix_xnet__convert_3,
  &predinfo_unix_xmktemp_2,
  &predinfo_unix_xmake__argv__list_3,
  &predinfo_unix_xkill_3,
  &predinfo_unix_xgetenv_2,
  &predinfo_unix_xfork__with__pipes_2F1_240_6,
  &predinfo_unix_xfork__with__pipes_1,
  &predinfo_unix_xfork_1,
  &predinfo_unix_xexit_1,
  &predinfo_unix_xdummy_0,
  &predinfo_unix_xcont_3,
  &predinfo_unix_xconnect__sub_5,
  &predinfo_unix_xconnect__sub_4,
  &predinfo_unix_xconnect_4,
  &predinfo_unix_xconnect_2,
  &predinfo_unix_xconnect2__sub_5,
  &predinfo_unix_xconnect2__sub_4,
  &predinfo_unix_xconnect2_4,
  &predinfo_unix_xconnect2_2,
  &predinfo_unix_xchmod_3,
  &predinfo_unix_xcd_2,
  &predinfo_unix_xbound__sock_4,
  &predinfo_unix_xbind_4,
  &predinfo_unix_xbind_2,
  &predinfo_unix_xasync__output_4,
  &predinfo_unix_xasync__io_5,
  &predinfo_unix_xasync__input_4,
  &predinfo_unix_xargv_1,
  &predinfo_unix_xargc_1,
  &predinfo_unix_xappend__open_2,
  &predinfo_unix_xaccess_3,
  0
};
extern Const struct predicate predicate_main_xmain_0;
static Const struct predinfo predinfo_main_xmain_0 = {
  (char *)"main",
  &predicate_main_xmain_0
};

extern Const struct predicate predicate_main_xkl1cmps_2;
static Const struct predinfo predinfo_main_xkl1cmps_2 = {
  (char *)"kl1cmps",
  &predicate_main_xkl1cmps_2
};

static Const struct predinfo * Const preds_in_main[] = {
  &predinfo_main_xmain_0,
  &predinfo_main_xkl1cmps_2,
  0
};
extern Const struct predicate predicate_generic_xnew_3;
static Const struct predinfo predinfo_generic_xnew_3 = {
  (char *)"new",
  &predicate_generic_xnew_3
};

extern Const struct predicate predicate_generic_xgeneric_2;
static Const struct predinfo predinfo_generic_xgeneric_2 = {
  (char *)"generic",
  &predicate_generic_xgeneric_2
};

static Const struct predinfo * Const preds_in_generic[] = {
  &predinfo_generic_xnew_3,
  &predinfo_generic_xgeneric_2,
  0
};
extern Const struct predicate predicate_unify__term__dcode_xunify__pf_2;
static Const struct predinfo predinfo_unify__term__dcode_xunify__pf_2 = {
  (char *)"unify_pf",
  &predicate_unify__term__dcode_xunify__pf_2
};

extern Const struct predicate predicate_unify__term__dcode_xunify__goal_2;
static Const struct predinfo predinfo_unify__term__dcode_xunify__goal_2 = {
  (char *)"unify_goal",
  &predicate_unify__term__dcode_xunify__goal_2
};

extern Const struct predicate predicate_unify__term__dcode_xunify__args_3;
static Const struct predinfo predinfo_unify__term__dcode_xunify__args_3 = {
  (char *)"unify_args",
  &predicate_unify__term__dcode_xunify__args_3
};

extern Const struct predicate predicate_unify__term__dcode_xunify_2;
static Const struct predinfo predinfo_unify__term__dcode_xunify_2 = {
  (char *)"unify",
  &predicate_unify__term__dcode_xunify_2
};

static Const struct predinfo * Const preds_in_unify__term__dcode[] = {
  &predinfo_unify__term__dcode_xunify__pf_2,
  &predinfo_unify__term__dcode_xunify__goal_2,
  &predinfo_unify__term__dcode_xunify__args_3,
  &predinfo_unify__term__dcode_xunify_2,
  0
};
extern Const struct predicate predicate_integer__arithmetics_xsubtract_3;
static Const struct predinfo predinfo_integer__arithmetics_xsubtract_3 = {
  (char *)"subtract",
  &predicate_integer__arithmetics_xsubtract_3
};

extern Const struct predicate predicate_integer__arithmetics_xshift__right_3;
static Const struct predinfo predinfo_integer__arithmetics_xshift__right_3 = {
  (char *)"shift_right",
  &predicate_integer__arithmetics_xshift__right_3
};

extern Const struct predicate predicate_integer__arithmetics_xshift__left_3;
static Const struct predinfo predinfo_integer__arithmetics_xshift__left_3 = {
  (char *)"shift_left",
  &predicate_integer__arithmetics_xshift__left_3
};

extern Const struct predicate predicate_integer__arithmetics_xplus_2;
static Const struct predinfo predinfo_integer__arithmetics_xplus_2 = {
  (char *)"plus",
  &predicate_integer__arithmetics_xplus_2
};

extern Const struct predicate predicate_integer__arithmetics_xor_3;
static Const struct predinfo predinfo_integer__arithmetics_xor_3 = {
  (char *)"or",
  &predicate_integer__arithmetics_xor_3
};

extern Const struct predicate predicate_integer__arithmetics_xmultiply_3;
static Const struct predinfo predinfo_integer__arithmetics_xmultiply_3 = {
  (char *)"multiply",
  &predicate_integer__arithmetics_xmultiply_3
};

extern Const struct predicate predicate_integer__arithmetics_xmodulo_3;
static Const struct predinfo predinfo_integer__arithmetics_xmodulo_3 = {
  (char *)"modulo",
  &predicate_integer__arithmetics_xmodulo_3
};

extern Const struct predicate predicate_integer__arithmetics_xminus_2;
static Const struct predinfo predinfo_integer__arithmetics_xminus_2 = {
  (char *)"minus",
  &predicate_integer__arithmetics_xminus_2
};

extern Const struct predicate predicate_integer__arithmetics_xexclusive__or_3;
static Const struct predinfo predinfo_integer__arithmetics_xexclusive__or_3 = {
  (char *)"exclusive_or",
  &predicate_integer__arithmetics_xexclusive__or_3
};

extern Const struct predicate predicate_integer__arithmetics_xdivide_3;
static Const struct predinfo predinfo_integer__arithmetics_xdivide_3 = {
  (char *)"divide",
  &predicate_integer__arithmetics_xdivide_3
};

extern Const struct predicate predicate_integer__arithmetics_xcomplement_2;
static Const struct predinfo predinfo_integer__arithmetics_xcomplement_2 = {
  (char *)"complement",
  &predicate_integer__arithmetics_xcomplement_2
};

extern Const struct predicate predicate_integer__arithmetics_xand_3;
static Const struct predinfo predinfo_integer__arithmetics_xand_3 = {
  (char *)"and",
  &predicate_integer__arithmetics_xand_3
};

extern Const struct predicate predicate_integer__arithmetics_xadd_3;
static Const struct predinfo predinfo_integer__arithmetics_xadd_3 = {
  (char *)"add",
  &predicate_integer__arithmetics_xadd_3
};

static Const struct predinfo * Const preds_in_integer__arithmetics[] = {
  &predinfo_integer__arithmetics_xsubtract_3,
  &predinfo_integer__arithmetics_xshift__right_3,
  &predinfo_integer__arithmetics_xshift__left_3,
  &predinfo_integer__arithmetics_xplus_2,
  &predinfo_integer__arithmetics_xor_3,
  &predinfo_integer__arithmetics_xmultiply_3,
  &predinfo_integer__arithmetics_xmodulo_3,
  &predinfo_integer__arithmetics_xminus_2,
  &predinfo_integer__arithmetics_xexclusive__or_3,
  &predinfo_integer__arithmetics_xdivide_3,
  &predinfo_integer__arithmetics_xcomplement_2,
  &predinfo_integer__arithmetics_xand_3,
  &predinfo_integer__arithmetics_xadd_3,
  0
};
Const struct modinfo defined_modules_predicates_c[] = {
  (char *)"klic_comp_bb",
  preds_in_klic__comp__bb,
  (char *)"klic_comp_gb",
  preds_in_klic__comp__gb,
  (char *)"klic_comp_generate",
  preds_in_klic__comp__generate,
  (char *)"klic_comp_insert",
  preds_in_klic__comp__insert,
  (char *)"klic_comp_normalize",
  preds_in_klic__comp__normalize,
  (char *)"klic_comp_util",
  preds_in_klic__comp__util,
  (char *)"comparator",
  preds_in_comparator,
  (char *)"splay_tree",
  preds_in_splay__tree,
  (char *)"keyed_sorted_set",
  preds_in_keyed__sorted__set,
  (char *)"klic_comp_extern",
  preds_in_klic__comp__extern,
  (char *)"klic_comp_write",
  preds_in_klic__comp__write,
  (char *)"klic_comp_obj",
  preds_in_klic__comp__obj,
  (char *)"klic_comp_version",
  preds_in_klic__comp__version,
  (char *)"klic_comp_macro",
  preds_in_klic__comp__macro,
  (char *)"klic_comp_cmacro",
  preds_in_klic__comp__cmacro,
  (char *)"keyed_set",
  preds_in_keyed__set,
  (char *)"unparser",
  preds_in_unparser,
  (char *)"scanner",
  preds_in_scanner,
  (char *)"variable",
  preds_in_variable,
  (char *)"parser",
  preds_in_parser,
  (char *)"atom_table",
  preds_in_atom__table,
  (char *)"klic_reader",
  preds_in_klic__reader,
  (char *)"klicio",
  preds_in_klicio,
  (char *)"klic_comp_message",
  preds_in_klic__comp__message,
  (char *)"functor_table",
  preds_in_functor__table,
  (char *)"klic_comp_kl1cmp",
  preds_in_klic__comp__kl1cmp,
  (char *)"timer",
  preds_in_timer,
  (char *)"unix",
  preds_in_unix,
  (char *)"main",
  preds_in_main,
  (char *)"generic",
  preds_in_generic,
  (char *)"unify_term_dcode",
  preds_in_unify__term__dcode,
  (char *)"integer_arithmetics",
  preds_in_integer__arithmetics,
  0, 0
};
