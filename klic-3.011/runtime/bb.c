/* ---------------------------------------------------------- 
%   (C)1993 Institute for New Generation Computer Technology 
%       (Read COPYRIGHT for detailed information.) 
%   (C)1996, 1997, 1998, 1999 Japan Information Processing Development Center
%       (Read COPYRIGHT-JIPDEC for detailed information.)
----------------------------------------------------------- */
#include <klic/basic.h>
#include <klic/struct.h>
#include <klic/primitives.h>
#include <klic/unify.h>
#include <klic/index.h>
#include <klic/generic.h>
#include <klic/gb.h>
#include <klic/bb.h>
#include <klic/g_float.h>

#define builtin_body_3(macro_body, pred, name)		\
     q x, y;						\
{							\
  extern struct predicate pred;				\
  q z;							\
 deref_x:						\
  if (isint(x)) {					\
  x_ok:							\
  deref_y:						\
    if (isint(y)) {					\
    ok:							\
      macro_body(x, y, z);				\
      return z;						\
    } else if (isref(y)) {				\
      q yy;						\
      yy = derefone(y);					\
      if (isint(yy)) {					\
	y = yy;						\
	goto ok;					\
      } else if (isref(yy)) {				\
	if (yy == y) {					\
	  goto suspend_y;				\
	} else {					\
	  q yyy = derefone(yy);				\
	  if (yyy == y) {				\
	    goto suspend_y;				\
	  } else {					\
	    y = yy;					\
	    goto deref_y;				\
	  }						\
	}						\
      }							\
    }							\
  } else if (isref(x)) {				\
    q xx;						\
    xx = derefone(x);					\
    if (isint(xx)) {					\
      x = xx;						\
      goto x_ok;					\
    } else if (isref(xx)) {				\
      if (xx == x) {					\
	goto suspend_x;					\
      } else {						\
	q xxx = derefone(xx);				\
	if (xxx == x) {					\
	  goto suspend_x;				\
	} else {					\
	  x = xx;					\
	  goto deref_x;					\
	}						\
      }							\
    }							\
  }							\
  builtin_3_type_error(x, y, &pred, name);		\
 suspend_x:						\
  return suspend_builtin_3(x, x, y, &pred);		\
 suspend_y:						\
  return suspend_builtin_3(y, x, y, &pred);		\
}

#define builtin_body_2(macro_body, pred, name)		\
     q x;						\
{							\
  extern struct predicate pred;				\
  q z;							\
 deref_x:						\
  if (isint(x)) {					\
  ok:							\
    macro_body(x, z);					\
    return z;						\
  } else if (isref(x)) {				\
    q xx;						\
    xx = derefone(x);					\
    if (isint(xx)) {					\
      x = xx;						\
      goto ok;						\
    } else if (isref(xx)) {				\
      if (xx == x) {					\
	goto suspend_x;					\
      } else {						\
	q xxx = derefone(xx);				\
	if (xxx == x) {					\
	  goto suspend_x;				\
	} else {					\
	  x = xx;					\
	  goto deref_x;					\
	}						\
      }							\
    }							\
  }							\
  builtin_2_type_error(x, &pred, name);			\
 suspend_x:						\
  return suspend_builtin_2(x, &pred);			\
}

static void enqueue_as_resumed(goal)
     struct goalrec *goal;
{
  declare_globals;
  struct goalrec *rsmg = resumed_goals;
  if (rsmg == 0) {
    rsmg = goal;
    goal->next = goal;
  } else {
    goal->next = rsmg->next;
    rsmg->next = goal;
  }
  resumed_goals = rsmg;
  heaplimit = 0;
}

static q suspend_builtin_3(v, x, y, pred)
     q v, x, y;
     Const struct predicate *pred;
{
  declare_globals;
  struct goalrec *goal;
  q z;
  heapalloc(goal, sizeof(struct goalrec)/sizeof(q)+3, (struct goalrec *));
  z = makeref(&goal->args[2]);
  goal->pred = pred;
  goal->args[0] = x;
  goal->args[1] = y;
  goal->args[2] = z;
  enqueue_as_resumed(goal);
  return z;
}

static q suspend_builtin_2(v, pred)
     q v;
     Const struct predicate *pred;
{
  declare_globals;
  struct goalrec *goal = 0;
  q z;
  heapalloc(goal, sizeof(struct goalrec)/sizeof(q)+2, (struct goalrec *));
  z = makeref(&goal->args[1]);
  goal->pred = pred;
  goal->args[0] = v;
  goal->args[1] = z;
  enqueue_as_resumed(goal);
  return z;
}

static void builtin_3_type_error(x, y, pred, name)
     q x, y;
     struct predicate *pred;
     char *name;
{
  fatalf("Argument type error in builtin predicate: %s\n", name);
}

static void builtin_2_type_error(x, pred, name)
     q x;
     struct predicate *pred;
     char *name;
{
  fatalf("Argument type error in builtin predicate %s", name);
}

q bblt_add_3(x, y)
builtin_body_3(bblt_add_no_check, predicate_integer__arithmetics_xadd_3,
	       "add/3")

q bblt_sub_3(x, y)
builtin_body_3(bblt_sub_no_check, predicate_integer__arithmetics_xsubtract_3,
	       "subtract/3")

q bblt_mult_3(x, y)
builtin_body_3(bblt_mult_no_check, predicate_integer__arithmetics_xmultiply_3,
	       "multiply/3")

q bblt_div_3(x, y)
builtin_body_3(bblt_div_no_check, predicate_integer__arithmetics_xdivide_3,
	       "divide/3")

q bblt_mod_3(x, y)
builtin_body_3(bblt_mod_no_check, predicate_integer__arithmetics_xmodulo_3,
	       "modulo/3")

q bblt_and_3(x, y)
builtin_body_3(bblt_and_no_check, predicate_integer__arithmetics_xand_3,
	       "and/3")

q bblt_or_3(x, y)
builtin_body_3(bblt_or_no_check, predicate_integer__arithmetics_xor_3,
	       "or/3")

q bblt_exclusive_or_3(x, y)
builtin_body_3(bblt_exclusive_or_no_check,
	       predicate_integer__arithmetics_xexclusive__or_3,
	       "exlclusive_or/3")

q bblt_rshift_3(x, y)
builtin_body_3(bblt_rshift_no_check,
	       predicate_integer__arithmetics_xshift__right_3,
	       "shift_right/3")

q bblt_lshift_3(x, y)
builtin_body_3(bblt_lshift_no_check,
	       predicate_integer__arithmetics_xshift__left_3,
	       "shift_left/3")

q bblt_plus_2(x)
builtin_body_2(bblt_plus_no_check,
	       predicate_integer__arithmetics_xplus_2,
	       "plus/2")

q bblt_minus_2(x)
builtin_body_2(bblt_minus_no_check,
	       predicate_integer__arithmetics_xminus_2,
	       "minus/2")

q bblt_complement_2(x)
builtin_body_2(bblt_complement_no_check,
	       predicate_integer__arithmetics_xcomplement_2,
	       "complement/2")

q bblt_fix_2(x)
     q x;
{
  extern struct predicate
    predicate_floating__arithmetics_xfloating__point__to__integer_2;
  q z;
 deref_x:
  if (isref(x)) {
    q xx;
    xx = derefone(x);
    if (isref(xx)) {
      q xxx = derefone(xx);
      if (xxx == x) {
	goto suspend_x;
      } else {
	x = xx;
	goto deref_x;
      }
    } else {
      x = xx;
    }
  }
  jump_if_not_in_class(x, float, type_error);
  z = makeint((long)float_value(x));
  return z;

 type_error:
  builtin_2_type_error(x,
    &predicate_floating__arithmetics_xfloating__point__to__integer_2,
		       "fix/2");
 suspend_x:
  return suspend_builtin_2(x,
    &predicate_floating__arithmetics_xfloating__point__to__integer_2);
}

