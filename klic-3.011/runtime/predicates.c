#include <klic/basic.h>
#include <klic/predinfo.h>

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
  (char *)"generic",
  preds_in_generic,
  (char *)"unify_term_dcode",
  preds_in_unify__term__dcode,
  (char *)"integer_arithmetics",
  preds_in_integer__arithmetics,
  0, 0
};
