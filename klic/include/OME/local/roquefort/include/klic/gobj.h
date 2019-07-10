/* ---------------------------------------------------------- 
%   (C)1993,1994,1995 Institute for New Generation Computer Technology 
%       (Read COPYRIGHT for detailed information.) 
%   (C)1996, 1997, 1998, 1999 Japan Information Processing Development Center
%       (Read COPYRIGHT-JIPDEC for detailed information.)
----------------------------------------------------------- */

#include <klic/g_methtab.h>

/*
  macros which call methods
*/

#define method_table_of(obj) ((obj)->method_table)
#define generic_passive_unify(obj, anothor) \
  (method_table_of(obj)->passive_unify(obj, anothor))
#define generic_active_unify(obj, anothor, allocp) \
{ \
  allocp = (method_table_of(obj)->active_unify(obj, anothor, allocp)); \
}
#define generic_print(obj, stream, depth, length) \
  (method_table_of(obj)->print(obj, stream, depth, length))
#define generic_gc(obj, allocp, sp) \
  (method_table_of(obj)->gc(obj, allocp, sp))
#define generic_regist(obj) \
  (method_table_of(obj)->regist(obj))
#define generic_deallocate(obj) \
  (method_table_of(obj)->deallocate(obj))
#define generic_close(obj, allocp) \
  (method_table_of(obj)->close(obj))

#define generic_generate(obj, allocp) \
  (method_table_of(obj)->generate(obj, allocp))

#define generic_suspend(obj, ref, goal) \
  (method_table_of(obj)->suspend(ref, goal))

/*** for Distributed KLIC system ***/
#define generic_encode(obj, pe, depth) \
  (method_table_of(obj)->encode(obj, pe, depth))

/*** for Shared-memory KLIC system ***/
#define generic_shmcopy(obj) \
  (method_table_of(obj)->shmcopy(obj))

#define generic_new(class, n, args) \
  (((struct data_object_method_table *)(class))->new((n), (args)))

/**** header file for using data and functions defined in kernel ****/ 

/* temporary */

#define general_passive_unify(obj, anothor) \
  eq_terms_body(obj, anothor)

#define general_active_unify(obj, anothor, allocp) \
  allocp = do_shallow_unify(allocp, obj, anothor)

/* extern q general_gc(); */

/*
extern struct goalrec *general_active_unify_method();
*/

/*
  datas for general utility with consumer object
*/

/****************
  suspension_reason

  shows result of the processing for consumer object.

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
