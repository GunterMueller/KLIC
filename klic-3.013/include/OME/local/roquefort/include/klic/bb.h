/* ----------------------------------------------------------
%   (C)1993,1994,1995 Institute for New Generation Computer Technology
%       (Read COPYRIGHT for detailed information.)
%   (C)1996, 1997, 1998, 1999 Japan Information Processing Development Center
%       (Read COPYRIGHT-JIPDEC for detailed information.)
----------------------------------------------------------- */

/*
  Addition
*/

extern q bblt_add_3();

#ifdef COMPACTCODE

#define bblt_add(x, y, z)					\
{ z = bblt_add_3(x, y); }

#define bblt_add_no_check_x(x, y, z)				\
{ bblt_add(x, y, z); }

#define bblt_add_no_check_y(x, y, z)				\
{ bblt_add(x, y, z); }

#else

#define bblt_add(x, y, z)					\
{								\
  if (!isint(x) || !isint(y)) {					\
    z = bblt_add_3(x, y);					\
  } else {							\
    bblt_add_no_check(x, y, z);					\
  }								\
}

#define bblt_add_no_check_x(x, y, z)				\
{								\
  if (!isint(y)) {						\
    z = bblt_add_3(x, y);					\
  } else {							\
    bblt_add_no_check(x, y, z);					\
  }								\
}

#define bblt_add_no_check_y(x, y, z)				\
{								\
  if (!isint(x)) {						\
    z = bblt_add_3(x, y);					\
  } else {							\
    bblt_add_no_check(x, y, z);					\
  }								\
}

#endif

#define bblt_add_no_check(x, y, z)				\
{ z = (q)((unsigned long)(x) + (unsigned long)(y) - INT); }

/*
  Subtraction
*/

extern q bblt_sub_3();

#ifdef COMPACTCODE

#define bblt_sub(x, y, z)					\
{ z = bblt_sub_3(x, y); }

#define bblt_sub_no_check_x(x, y, z)				\
{ bblt_sub(x, y, z); }

#define bblt_sub_no_check_y(x, y, z)				\
{ bblt_sub(x, y, z); }

#else

#define bblt_sub(x, y, z)					\
{								\
  if (!isint(x) || !isint(y)) {					\
    z = bblt_sub_3(x, y);					\
  } else {							\
    bblt_sub_no_check(x, y, z);					\
  }								\
}

#define bblt_sub_no_check_x(x, y, z)				\
{								\
  if (!isint(y)) {						\
    z = bblt_sub_3(x, y);					\
  } else {							\
    bblt_sub_no_check(x, y, z);					\
  }								\
}

#define bblt_sub_no_check_y(x, y, z)				\
{								\
  if (!isint(x)) {						\
    z = bblt_sub_3(x, y);					\
  } else {							\
    bblt_sub_no_check(x, y, z);					\
  }								\
}

#endif

#define bblt_sub_no_check(x, y, z)				\
{ z = (q)((unsigned long)(x) - ((unsigned long)(y) - INT)); }

/*
  Multiplication
*/

extern q bblt_mult_3();

#ifdef COMPACTCODE

#define bblt_mult(x, y, z)					\
{ z = bblt_mult_3(x, y); }
#define bblt_mult_no_check_x(x, y, z)				\
{ bblt_mult(x, y, z); }
#define bblt_mult_no_check_y(x, y, z)				\
{ bblt_mult(x, y, z); }

#else

#define bblt_mult(x, y, z)					\
{								\
  if (!isint(x) || !isint(y)) {					\
    z = bblt_mult_3(x, y);					\
  } else {							\
    bblt_mult_no_check(x, y, z);				\
  }								\
}

#define bblt_mult_no_check_x(x, y, z)				\
{								\
  if (!isint(y)) {						\
    z = bblt_mult_3(x, y);					\
  } else {							\
    bblt_mult_no_check(x, y, z);				\
  }								\
}

#define bblt_mult_no_check_y(x, y, z)				\
{								\
  if (!isint(x)) {						\
    z = bblt_mult_3(x, y);					\
  } else {							\
    bblt_mult_no_check(x, y, z);				\
  }								\
}

#endif

#define bblt_mult_no_check(x, y, z)				\
{ z = (q)(((long)(x) - INT) * (intval(y)) + INT); }

/*
  Division
*/

extern q bblt_div_3();

#ifdef COMPACTCODE

#define bblt_div(x, y, z)					\
{ z = bblt_div_3(x, y); }

#define bblt_div_no_check_x(x, y, z)				\
{ bblt_div(x, y, z); }

#define bblt_div_no_check_y(x, y, z)				\
{ bblt_div(x, y, z); }

#else

#define bblt_div(x, y, z)					\
{								\
  if (!isint(x) || !isint(y)) {					\
    z = bblt_div_3(x, y);					\
  } else {							\
    bblt_div_no_check(x, y, z);					\
  }								\
}

#define bblt_div_no_check_x(x, y, z)				\
{								\
  if (!isint(y)) {						\
    z = bblt_div_3(x, y);					\
  } else {							\
    bblt_div_no_check(x, y, z);					\
  }								\
}

#define bblt_div_no_check_y(x, y, z)				\
{								\
  if (!isint(x)) {						\
    z = bblt_div_3(x, y);					\
  } else {							\
    bblt_div_no_check(x, y, z);					\
  }								\
}

#endif

#define bblt_div_no_check(x, y, z)				\
{ z = makeint(((long)(x) - INT)/((long)(y) - INT)); }

/*
  Modulo
*/

extern q bblt_mod_3();

#ifdef COMPACTCODE

#define bblt_mod(x, y, z)					\
{ z = bblt_mod_3(x, y); }

#define bblt_mod_no_check_x(x, y, z)				\
{ bblt_mod(x, y, z); }

#define bblt_mod_no_check_y(x, y, z)				\
{ bblt_mod(x, y, z); }

#else

#define bblt_mod(x, y, z)					\
{								\
  if (!isint(x) || !isint(y)) {					\
    z = bblt_mod_3(x, y);					\
  } else {							\
    bblt_mod_no_check(x, y, z);					\
  }								\
}

#define bblt_mod_no_check_x(x, y, z)				\
{								\
  if (!isint(y)) {						\
    z = bblt_mod_3(x, y);					\
  } else {							\
    bblt_mod_no_check(x, y, z);					\
  }								\
}

#define bblt_mod_no_check_y(x, y, z)				\
{								\
  if (!isint(x)) {						\
    z = bblt_mod_3(x, y);					\
  } else {							\
    bblt_mod_no_check(x, y, z);					\
  }								\
}

#endif

#define bblt_mod_no_check(x, y, z)				\
{ z = (q)(((long)(x) - INT) % ((long)(y) - INT) + INT); }

/*
  Bitwise Logical And
*/

extern q bblt_and_3();

#ifdef COMPACTCODE

#define bblt_and(x, y, z)					\
{ z = bblt_and_3(x, y); }

#define bblt_and_no_check_x(x, y, z)				\
{ bblt_and(x, y, z); }

#define bblt_and_no_check_y(x, y, z)				\
{ bblt_and(x, y, z); }

#else

#define bblt_and(x, y, z)					\
{								\
  if (!isint(x) || !isint(y)) {					\
    z = bblt_and_3(x, y);					\
  } else {							\
    bblt_and_no_check(x, y, z);					\
  }								\
}

#define bblt_and_no_check_x(x, y, z)				\
{								\
  if (!isint(y)) {						\
    z = bblt_and_3(x, y);					\
  } else {							\
    bblt_and_no_check(x, y, z);					\
  }								\
}

#define bblt_and_no_check_y(x, y, z)				\
{								\
  if (!isint(x)) {						\
    z = bblt_and_3(x, y);					\
  } else {							\
    bblt_and_no_check(x, y, z);					\
  }								\
}

#endif

#define bblt_and_no_check(x, y, z)				\
{ z = (q)((long)(x) & (long)(y)); }

/*
  Bitwise Logical Or
*/

extern q bblt_or_3();

#ifdef COMPACTCODE

#define bblt_or(x, y, z)					\
{ z = bblt_or_3(x, y); }

#define bblt_or_no_check_x(x, y, z)				\
{ bblt_or(x, y, z); }

#define bblt_or_no_check_y(x, y, z)				\
{ bblt_or(x, y, z); }

#else

#define bblt_or(x, y, z)					\
{								\
  if (!isint(x) || !isint(y)) {					\
    z = bblt_or_3(x, y);					\
  } else {							\
    bblt_or_no_check(x, y, z);					\
  }								\
}

#define bblt_or_no_check_x(x, y, z)				\
{								\
  if (!isint(y)) {						\
    z = bblt_or_3(x, y);					\
  } else {							\
    bblt_or_no_check(x, y, z);					\
  }								\
}

#define bblt_or_no_check_y(x, y, z)				\
{								\
  if (!isint(x)) {						\
    z = bblt_or_3(x, y);					\
  } else {							\
    bblt_or_no_check(x, y, z);					\
  }								\
}

#endif

#define bblt_or_no_check(x, y, z)				\
{ z = (q)((long)(x) | (long)(y)); }

/*
  Bitwise Logical Exclusive Or
*/

extern q bblt_exclusive_or_3();

#ifdef COMPACTCODE

#define bblt_exclusive_or(x, y, z)				\
{ z = bblt_exclusive_or_3(x, y); }

#define bblt_exclusive_or_no_check_x(x, y, z)			\
{ bblt_exclusive_or(x, y, z); }

#define bblt_exclusive_or_no_check_y(x, y, z)			\
{ bblt_exclusive_or(x, y, z); }

#else

#define bblt_exclusive_or(x, y, z)				\
{								\
  if (!isint(x) || !isint(y)) {					\
    z = bblt_exclusive_or_3(x, y);				\
  } else {							\
    bblt_exclusive_or_no_check(x, y, z);			\
  }								\
}

#define bblt_exclusive_or_no_check_x(x, y, z)			\
{								\
  if (!isint(y)) {						\
    z = bblt_exclusive_or_3(x, y);				\
  } else {							\
    bblt_exclusive_or_no_check(x, y, z);			\
  }								\
}

#define bblt_exclusive_or_no_check_y(x, y, z)			\
{								\
  if (!isint(x)) {						\
    z = bblt_exclusive_or_3(x, y);				\
  } else {							\
    bblt_exclusive_or_no_check(x, y, z);			\
  }								\
}

#endif

#define bblt_exclusive_or_no_check(x, y, z)			\
{ z = (q)(((long)(x) ^ (long)(y)) + INT); }

/*
  Right Shift
*/

extern q bblt_rshift_3();

#ifdef COMPACTCODE

#define bblt_rshift(x, y, z)					\
{ z = bblt_rshift_3(x, y); }

#define bblt_rshift_no_check_x(x, y, z)				\
{ bblt_rshift(x, y, z); }

#define bblt_rshift_no_check_y(x, y, z)				\
{ bblt_rshift(x, y, z); }

#else

#define bblt_rshift(x, y, z)					\
{								\
  if (!isint(x) || !isint(y)) {					\
    z = bblt_rshift_3(x, y);					\
  } else {							\
    bblt_rshift_no_check(x, y, z);				\
  }								\
}

#define bblt_rshift_no_check_x(x, y, z)				\
{								\
  if (!isint(y)) {						\
    z = bblt_rshift_3(x, y);					\
  } else {							\
    bblt_rshift_no_check(x, y, z);				\
  }								\
}

#define bblt_rshift_no_check_y(x, y, z)				\
{								\
  if (!isint(x)) {						\
    z = bblt_rshift_3(x, y);					\
  } else {							\
    bblt_rshift_no_check(x, y, z);				\
  }								\
}

#endif

#define bblt_rshift_no_check(x, y, z)				\
{ z = makeint(intval(x) >> intval(y)); }

/*
  Left Shift
*/

extern q bblt_lshift_3();

#ifdef COMPACTCODE

#define bblt_lshift(x, y, z)					\
{ z = bblt_lshift_3(x, y); }

#define bblt_lshift_no_check_x(x, y, z)				\
{ bblt_lshift(x, y, z); }

#define bblt_lshift_no_check_y(x, y, z)				\
{ bblt_lshift(x, y, z); }

#else

#define bblt_lshift(x, y, z)					\
{								\
  if (!isint(x) || !isint(y)) {					\
    z = bblt_lshift_3(x, y);					\
  } else {							\
    bblt_lshift_no_check(x, y, z);				\
  }								\
}

#define bblt_lshift_no_check_x(x, y, z)				\
{								\
  if (!isint(y)) {						\
    z = bblt_lshift_3(x, y);					\
  } else {							\
    bblt_lshift_no_check(x, y, z);				\
  }								\
}

#define bblt_lshift_no_check_y(x, y, z)				\
{								\
  if (!isint(x)) {						\
    z = bblt_lshift_3(x, y);					\
  } else {							\
    bblt_lshift_no_check(x, y, z);				\
  }								\
}

#endif

#define bblt_lshift_no_check(x, y, z)				\
{ z = (q)((((long)(x) - INT) << intval(y)) + INT); }

/*
  Bitwise Complement
*/

extern q bblt_complement_2();

#ifdef COMPACTCODE

#define bblt_complement(x, z)					\
{ z = bblt_complement_2(x); }

#else

#define bblt_complement(x, z)					\
{								\
  if (!isint(x)) {						\
    z = bblt_complement_2(x);					\
  } else {							\
    bblt_complement_no_check(x, z);				\
  }								\
}

#endif

#define bblt_complement_no_check(x, z)				\
  { z = (q)((long)(x) ^ ((-1l) << ATAGBITS)); }

/*
  Unary Plus
*/

extern q bblt_plus_2();

#ifdef COMPACTCODE

#define bblt_plus(x, z)						\
{ z = bblt_plus_2(x); }

#else

#define bblt_plus(x, z)						\
{								\
  if (!isint(x)) {						\
    z = bblt_plus_2(x);						\
  } else {							\
    bblt_plus_no_check(x, z);					\
  }								\
}

#endif

#define bblt_plus_no_check(x, z)				\
{ z = (x); }

/*
  Unary Minus
*/

extern q bblt_minus_2();

#ifdef COMPACTCODE

#define bblt_minus(x, z)					\
{ z = bblt_minus_2(x); }

#else

#define bblt_minus(x, z)					\
{								\
  if (!isint(x)) {						\
    z = bblt_minus_2(x);					\
  } else {							\
    bblt_minus_no_check(x, z);					\
  }								\
}

#endif

#define bblt_minus_no_check(x, z)				\
{ z = (q)(2*INT-(long)(x)); }

/*
  Floating Point
*/

extern q bblt_fix_2();

#define bblt_fix(x, z)						\
{								\
  extern struct data_object_method_table TableName(float);	\
  if (!isfunctor(x) || !isgobj(x) ||				\
      functor_of(x) != (q)&TableName(float)) {			\
    z = bblt_fix_2(x);						\
  } else {							\
    z = makeint((long)(float_value(x)));			\
  }								\
}


/*
  For Distributed KLIC System
*/

#define bblt_current_node_no_check(x, y) \
{  x = makeint(my_node);  y = makeint(total_node); }
