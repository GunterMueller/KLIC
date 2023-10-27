/* ----------------------------------------------------------
%   (C)1993,1994,1995 Institute for New Generation Computer Technology
%       (Read COPYRIGHT for detailed information.)
%   (C)1996, 1997, 1998, 1999 Japan Information Processing Development Center
%       (Read COPYRIGHT-JIPDEC for detailed information.)
----------------------------------------------------------- */

#include <math.h>
#include <stdio.h>
#include <klic/gdobject.h>
#include <klic/gd_macro.h>

#ifdef DIST
#include <klic/interpe.h>
#endif

#include "atom.h"
#include "funct.h"

#ifdef STRINGH
#include <string.h>
#else
#include <strings.h>
#endif

#define GD_CLASS_NAME() float
#define GD_OBJ_TYPE struct float_object
#define GD_OBJ_SIZE(obj) (G_SIZE_IN_Q(GD_OBJ_TYPE))

GD_USE_CLASS(byte__string);

GD_OBJ_TYPE {
  struct data_object_method_table *method_table;
  double value;
};

#define ALIGN() \
if (sizeof(double) != sizeof(long)) { \
  g_allocp = (q*)(((unsigned long)g_allocp + (sizeof(double) - 1)) \
		  & (-sizeof(double))); \
}

#ifdef SHM
#define ALIGN_SHM() \
if (sizeof(double) != sizeof(long)) { \
  gallocp = (q*)(((unsigned long)gallocp + (sizeof(double) - 1)) \
		  & (-sizeof(double))); \
}

#include <klic/shm.h>
GDDEF_SHMCOPY()
{
  G_STD_DECL;
  GD_OBJ_TYPE *newself;

  for (;;) {
    ALIGN_SHM();
    newself = (GD_OBJ_TYPE *)gallocp;
    gallocp = (q*)((unsigned long)gallocp + sizeof(struct float_object));
    if (gallocp <= glimit) break;
    gallocp = shm_galloc(0);
  }
  newself->method_table = GD_SELF->method_table;
  newself->value = GD_SELF->value;
  return(makefunctor(newself));
}
#define GDUSE_MY_SHMCOPY
#endif

/* basic method definitions */

GDDEF_GUNIFY()
{
  G_STD_DECL;
  if (GD_SELF->method_table != GD_OTHER->method_table ||
      GD_SELF->value != GD_OTHER->value)
    GD_GUNIFY_FAIL;
  GD_GSUCCEED;
}

GDDEF_UNIFY()
{
  G_STD_DECL;

  if (GD_SELF->method_table != GD_OTHER->method_table ||
      GD_SELF->value != GD_OTHER->value)
    GD_UNIFY_FAIL;
  GD_RETURN;
}

GDDEF_GC()
{
  G_STD_DECL;
  GD_OBJ_TYPE *newself;

  ALIGN();
  GDSET_NEWOBJ_IN_NEWGEN(newself);
  newself->value = GD_SELF->value;
  GD_RETURN_FROM_GC(newself);
}

/* Generic method */

GDDEF_METHOD(print_1)
{
  G_STD_DECL;
  extern q convert_c_string_to_klic_string();
  double value = GD_SELF->value;
  q str;
  char buf[100];
  (void)sprintf(buf, "%1.16g", value);
  if (STRCHR(buf, '.') == 0 &&
      strcmp(buf, "Infinity") && strcmp(buf, "-Infinity") &&
      strcmp(buf, "NaN")) {
    char *exponent = STRCHR(buf, 'e');
    if (exponent != 0) {
      char save[100];
      (void)strcpy(save, exponent);
      (void)strcpy(exponent, ".0");
      (void)strcpy(exponent+2, save);
    } else {
      (void)strcat(buf, ".0");
    }
  }
  str = convert_c_string_to_klic_string(buf,g_allocp);
  g_allocp = heapp;
  GD_UNIFY_VALUE(GD_ARGV[0], str);
  GD_RETURN;
}

GDDEF_METHOD(int_1)
{
  G_STD_DECL;
  double value = GD_SELF->value;
  q result = makeint((long)value);
  GD_UNIFY_VALUE(GD_ARGV[0], result);
  GD_RETURN;
}

static double unary_plus(x)
     double x;
{
  return x;
}

static double unary_minus(x)
     double x;
{
  return(-x);
}

/*  Generic Method Table */
GDDEF_GENERIC()
{
  G_STD_DECL;
  double self, result;
  unsigned long result_index;
  GD_OBJ_TYPE *newobj;
  double (*func)();

  self = GD_SELF->value;
  GD_SWITCH_ON_METHOD {
    GD_METHOD_CASE(print_1);
    GD_METHOD_CASE(int_1);
    break;
  default:
    ALIGN();
    GDSET_NEWOBJ(newobj);
    GD_SWITCH_ON_ARITY {
    case 1: {
      result_index = 0;
      GD_SWITCH_ON_METHOD {
	GD_METHOD_CASE_DIRECT(sin_1):
	func = sin; goto apply_1;
	GD_METHOD_CASE_DIRECT(cos_1):
	func = cos; goto apply_1;
	GD_METHOD_CASE_DIRECT(tan_1):
	func = tan; goto apply_1;
	GD_METHOD_CASE_DIRECT(asin_1):
	func = asin; goto apply_1;
	GD_METHOD_CASE_DIRECT(acos_1):
	func = acos; goto apply_1;
	GD_METHOD_CASE_DIRECT(atan_1):
	func = atan; goto apply_1;
	GD_METHOD_CASE_DIRECT(sinh_1):
	func = sinh; goto apply_1;
	GD_METHOD_CASE_DIRECT(cosh_1):
	func = cosh; goto apply_1;
	GD_METHOD_CASE_DIRECT(tanh_1):
	func = tanh; goto apply_1;
	/*
	  GD_METHOD_CASE_DIRECT(asinh_1):
	  func = asinh; goto apply_1;
	  GD_METHOD_CASE_DIRECT(acosh_1):
	  func = acosh; goto apply_1;
	  GD_METHOD_CASE_DIRECT(atanh_1):
	  func = atanh; goto apply_1;
	  */
	GD_METHOD_CASE_DIRECT(exp_1):
	func = exp; goto apply_1;
	GD_METHOD_CASE_DIRECT(log_1):
	func = log; goto apply_1;
	GD_METHOD_CASE_DIRECT(sqrt_1):
	func = sqrt; goto apply_1;
	GD_METHOD_CASE_DIRECT(ceil_1):
	func = ceil; goto apply_1;
	GD_METHOD_CASE_DIRECT(floor_1):
	func = floor; goto apply_1;
	/*
	  GD_METHOD_CASE_DIRECT(round_1):
	  func = rint; goto apply_1;
	  */
	GD_METHOD_CASE_DIRECT(plus_1):
	func = unary_plus; goto apply_1; 
	GD_METHOD_CASE_DIRECT(minus_1):
	func = unary_minus; goto apply_1; 
	GD_METHOD_CASE_DEFAULT;
      }
    }
    case 2: {
      q another = GD_ARGV[0];
      double another_value;
      GD_DEREF(another);
      result_index = 1;
      if (!isfunctor(another) ||
	  ((GD_OBJ_TYPE*)functorp(another))->method_table !=
	  GD_SELF->method_table) {
	debug_printf("### \"%k\" given to floating point method %F ###\n",
		     another, g_method_functor);
	GD_FAIL("Illegal argument in floating point object method.");
      }
      another_value = ((GD_OBJ_TYPE *)functorp(another))->value;
      GD_SWITCH_ON_METHOD {
	GD_METHOD_CASE_DIRECT(add_2):
	result = self + another_value;
	goto apply_2;
	GD_METHOD_CASE_DIRECT(subtract_2):
	result = self - another_value;
	goto apply_2;
	GD_METHOD_CASE_DIRECT(multiply_2):
	result = self * another_value;
	goto apply_2;
	GD_METHOD_CASE_DIRECT(divide_2):
	result = self / another_value;
	goto apply_2;
	GD_METHOD_CASE_DIRECT(pow_2):
	result = pow(self, another_value);
	goto apply_2;
	GD_METHOD_CASE_DEFAULT;
      }
      break;
    }
      GD_METHOD_CASE_DEFAULT;
    }
  }
  GD_RETURN;

 apply_1:
  result = func(self);
 apply_2:
  newobj->value = result;
  GD_UNIFY(GD_ARGV[result_index], makefunctor(newobj));
  GD_RETURN;
}

/* guard generic methods */

#define COMPARE_METHOD(comparison)				\
{								\
  G_STD_DECL;							\
  q otherq = GD_ARGV[0];					\
  GD_OBJ_TYPE *other;						\
  double self, theother;					\
								\
  if (!G_ISGOBJ(otherq)) GD_GFAIL;				\
  other = (GD_OBJ_TYPE *)G_FUNCTORP(otherq);			\
  if (other->method_table != GD_SELF->method_table) GD_GFAIL;	\
  self = GD_SELF->value;					\
  theother = other->value;					\
  if (comparison) GD_GSUCCEED;					\
  GD_GFAIL;							\
}

GDDEF_GMETHOD(less__than_1)
{ COMPARE_METHOD((self < theother)); }

GDDEF_GMETHOD(not__greater__than_1)
{ COMPARE_METHOD((self <= theother)); }

GDDEF_GMETHOD(not__less__than_1)
{ COMPARE_METHOD((self >= theother)); }

GDDEF_GMETHOD(greater__than_1)
{ COMPARE_METHOD((self > theother)); }

GDDEF_GMETHOD(equal_1)
{ COMPARE_METHOD((self == theother)); }

GDDEF_GMETHOD(not__equal_1)
{ COMPARE_METHOD((self != theother)); }

GDDEF_GMETHOD(float_0)
{
  G_STD_DECL;
  GD_GSUCCEED;
}

GDDEF_GGENERIC()
{
  G_STD_DECL;
  GD_SWITCH_ON_GMETHOD {
    GD_GMETHOD_CASE(less__than_1);
    GD_GMETHOD_CASE(not__greater__than_1);
    GD_GMETHOD_CASE(not__less__than_1);
    GD_GMETHOD_CASE(greater__than_1);
    GD_GMETHOD_CASE(equal_1);
    GD_GMETHOD_CASE(not__equal_1);
    GD_GMETHOD_CASE(float_0);
    GD_GMETHOD_CASE_DEFAULT;
  }
}

GDDEF_PRINT()
{
  G_STD_DECL;
  char b[100];
  sprintf(b, "%f", GD_SELF->value);
  klic_fprintf(g_fp, "%s", b);
  GD_RETURN_FROM_PRINT;
}

GDDEF_COMPARE()
{
  G_STD_DECL;
  if (GD_SELF->value > GD_OTHER->value) {
    return G_MAKEINT(1);
  } else if (GD_SELF->value < GD_OTHER->value) {
    return G_MAKEINT(-1);
  } else {
    return G_MAKEINT(0);
  }
}

GDDEF_HASH()
{
  G_STD_DECL;
  return G_MAKEINT((long)GD_SELF->value);
}
#ifdef DIST
q *decode_float();

GDDEF_ENCODE()
{
  G_STD_DECL;

  PUT_BUFFER(buffer, decode_float);
  if ( sizeof(double) == sizeof(long) ) {
    PUT_BUFFER(buffer, (long) GD_SELF->value );
  } else {
    union d2l {
      double d;
      long l[2];
    } dl;
    
    dl.d = GD_SELF->value;
    PUT_BUFFER(buffer, dl.l[0]); 
    PUT_BUFFER(buffer, dl.l[1]); 
  }
  return(GENERIC_SUCCEEDED);
}

#endif

#define GDUSE_MY_GUNIFY
#define GDUSE_MY_UNIFY
#define GDUSE_MY_PRINT
#define GDUSE_MY_GC
#ifdef DIST
#define GDUSE_MY_ENCODE
#endif
#define GDUSE_MY_GENERIC
#define GDUSE_MY_GGENERIC
#define GDUSE_MY_COMPARE
#define GDUSE_MY_HASH

/* define the method table structure of the vector */

#include <klic/gd_methtab.h>

/*  new_float function */
GDDEF_NEW()
{
  GD_STD_DECL_FOR_NEW;
  q init = GD_ARGV[0];
  GD_OBJ_TYPE *newobj;

  GD_DEREF_FOR_NEW(init);
  ALIGN();
  GDSET_NEWOBJ_FOR_NEW(newobj,G_SIZE_IN_Q(GD_OBJ_TYPE));
  if (G_ISINT(init)) {
    newobj->value = (double)G_INTVAL(init);
  } else if (G_ISGOBJ(init) &&
	     (struct data_object_method_table *)G_FUNCTOR_OF(init)
	     == &byte__string_g_data_method_table) {
    extern unsigned char *generic_string_body();
    double atof();
    newobj->value = atof((char *)generic_string_body(G_FUNCTORP(init)));
  } else {
    debug_printf("### %k ###\n", init);
    GD_FAIL("Illegal initial value for floating point objects");
  }
  GD_RETURN_FROM_NEW(newobj);
}

#ifdef DIST

void push_decode_stack();

q *decode_float(inbuf, g_allocp)
     combuf *inbuf;
     q *g_allocp;
{
  G_STD_DECL;
  GD_OBJ_TYPE *newobj;

  ALIGN();
  G_HEAPALLOC(newobj, G_SIZE_IN_Q(GD_OBJ_TYPE), (GD_OBJ_TYPE *));

  newobj->method_table = &GD_method_table;
  if ( sizeof(long) == sizeof(double) ) {
    newobj->value = (double) GET_BUFFER(inbuf);
  } else {
    union l2d {
      long l[2];
      double d;
    } ld;
    
    ld.l[0] = (long) GET_BUFFER(inbuf);
    ld.l[1] = (long) GET_BUFFER(inbuf);
    newobj->value = ld.d;
  }
  push_decode_stack((q)makefunctor(newobj));
  return (g_allocp);
}
#endif
