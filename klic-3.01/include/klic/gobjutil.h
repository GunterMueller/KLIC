/* ---------------------------------------------------------- 
%   (C)1993,1994,1995 Institute for New Generation Computer Technology 
%       (Read COPYRIGHT for detailed information.) 
%   (C)1996, 1997, 1998, 1999 Japan Information Processing Development Center
%       (Read COPYRIGHT-JIPDEC for detailed information.)
----------------------------------------------------------- */
/**** header file for using data and functions defined in kernel ****/ 

/* temporary */

#define general_passive_unify(obj, anothor) \
  eq_terms_body(obj, anothor)

#define general_active_unify(obj, anothor, allocp) \
  do_shallow_unify(allocp, obj, anothor)

extern q general_gc( /* q *term, q* allocp */ );

/*
extern struct goalrec *general_active_unify_method();
*/

/*
  datas for general utility with mutable object
*/

/****************
  suspension_reason

  shows result of the processing for mutable object.

  0: normal;
  1: GC request;
  other: the process is suspended. pointer of the reason of the prosess.
******************/
extern q suspension_reason;

/************
  rest_of_stream

  indicates the next hooked term. This variable is meaningful
  just `suspension_reason' is 0.

  0: normal;
  other: the next hooked term
**************/
extern q rest_of_stream;

/* 
  this function has replaced by G_suspend
  make new hook for object for gmethod.c
q make_new_hook_for_gobj();
*/

/*
  this function has not already used
struct goalrec *call_generic_new();
*/

extern q method_result;

/*
  this variable for new method has not already used
struct generic *new_term;
*/
