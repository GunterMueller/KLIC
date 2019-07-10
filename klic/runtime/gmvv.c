/* ---------------------------------------------------------- 
%   (C)1993,1994,1995 Institute for New Generation Computer Technology 
%       (Read COPYRIGHT for detailed information.) 
%   (C)1996, 1997, 1998, 1999 Japan Information Processing Development Center
%       (Read COPYRIGHT-JIPDEC for detailed information.)
----------------------------------------------------------- */

/*
  Vector Objects
*/

#include <stdio.h>
#include <klic/gdobject.h>
#include <klic/g_vector.h>

#ifdef DIST
#include <klic/interpe.h>
#endif
#ifdef SHM
#include <klic/shm.h>
#endif

#include "atom.h"
#include "funct.h"

#define GD_CLASS_NAME()		vector
#define GD_OBJ_TYPE		struct vector_object
#define GD_OBJ_SIZE(obj)	(G_SIZE_IN_Q(GD_OBJ_TYPE))

#include <klic/gd_macro.h>

#define VECTOR_OBJ(x)	((GD_OBJ_TYPE *)(G_FUNCTORP(x)))

/* shallowing */

static do_shallow(vector)
     struct vector_object *vector;
{
  q v, last, next;

  /* Go down to the shallwed version inverting pointers */
  last = VECTOR_SHALLOW_MARK;
  v = GD_OBJ(vector);
  do {
    next = VECTOR_OBJ(v)->next;
    VECTOR_OBJ(v)->next = last;
    last = v;
    v = next;
  } while (v != VECTOR_SHALLOW_MARK);
  v = last;

  /* Update physical vector elements tracing the inverted pointers */
  {
    long size = VECTOR_OBJ(v)->index;
    q *body = VECTOR_OBJ(v)->body;
    next = VECTOR_OBJ(v)->next;
    do {
      long index;
      index = VECTOR_OBJ(next)->index;
      VECTOR_OBJ(v)->index = index;
      VECTOR_OBJ(v)->body = (q *)body[index];
      body[index] = (q)(VECTOR_OBJ(next)->body);
      v = next;
      next = VECTOR_OBJ(v)->next;
    } while (next != VECTOR_SHALLOW_MARK);
    VECTOR_OBJ(v)->index = size;
    VECTOR_OBJ(v)->body = body;
  }
}

#define Shallow(vector) \
{ if (!IS_SHALLOW_VECTOR(vector)) do_shallow(vector); }

/* basic method definitions */

GDDEF_GUNIFY()
{
  G_STD_DECL;
  long size, k;
  if (GD_SELF->method_table != GD_OTHER->method_table) GD_GUNIFY_FAIL;
  Shallow(GD_SELF);
  size = GD_SELF->index;
  Shallow(GD_OTHER);
  if (GD_OTHER->index != size) GD_GUNIFY_FAIL;
  for (k=0; k<size; k++) {
    q retval;
    q x = GD_OTHER->body[k];
    Shallow(GD_SELF);
    retval = GD_GUNIFY(GD_SELF->body[k], x);
    switch ((long)retval) {
    case GD_GSUCCESS:
      break;
    case GD_GFAILURE:
      GD_GUNIFY_FAIL;
      break;
    default:
      GD_GRETURN(retval);
    }
    Shallow(GD_OTHER);
  }
  GD_GSUCCEED;
}

GDDEF_UNIFY()
{
  G_STD_DECL;
  long size, k;

  if (GD_SELF->method_table != GD_OTHER->method_table) GD_UNIFY_FAIL;
  Shallow(GD_SELF);
  size = GD_SELF->index;
  Shallow(GD_OTHER);
  if (GD_OTHER->index != size) GD_UNIFY_FAIL;
  for (k=0; k<size; k++) {
    q x = GD_OTHER->body[k];
    Shallow(GD_SELF);
    GD_UNIFY(GD_SELF->body[k], x);
    Shallow(GD_OTHER);
  }
  GD_RETURN;
}

GDDEF_GC()
{
  G_STD_DECL;
  GD_OBJ_TYPE *newself;

  if (GD_SELF->iscnst) {
    fatal("GC method for constant vector unexpected");
  }
  GDSET_NEWOBJ_IN_NEWGEN(newself);
  newself->iscnst = 0;
  if (IS_SHALLOW_VECTOR(GD_SELF)) {
    /* Shallow version */
    q *body = GD_SELF->body;
    long size = GD_SELF->index;
    q *newbody;
    long k;
    if ((g_allocp+size)>real_heaplimit) fatal("Not enough space collected");
    newbody = g_allocp;
    g_allocp += size;
    newself->next = VECTOR_SHALLOW_MARK;
    newself->index = size;
    newself->body = newbody;
    for (k=0; k<size; k++) {
/*
      q elem = body[k];

      if (G_ISREF(elem)) {
	q* newword = g_allocp++;
	derefone(newword) = elem;
	newbody[k] = makeref(newword);
	if((g_sp) == gcmax) {
	  (g_sp) = make_larger_stack(g_sp);
	}
	*(g_sp++) = newword;
      } else {
	GD_COPY_KL1_TERM_TO_NEWGEN(body[k], newbody[k]);
      }
*/
      GD_COPY_KL1_TERM_TO_NEWGEN(body[k], newbody[k]);
    }
  } else {
    /* Deep version */
    newself->index = GD_SELF->index;
    GD_COPY_KL1_TERM_TO_NEWGEN(GD_SELF->next, newself->next);
    /*
      The following macro is manually expanded inline here
      to avoid type conflict problem.

      GD_COPY_KL1_TERM_TO_NEWGEN(GD_SELF->body, newself->body);
    */
    newself->body = GD_SELF->body;
    if ((g_sp) == gcmax) {
      (g_sp) = make_larger_stack(g_sp);
    }
    *(g_sp++) = (q *)&(newself->body);
    /* End of manual expansion */
  }
  GD_RETURN_FROM_GC(newself);
}

#ifdef DIST
q element_of_vector();

q *decode_vector();
void encode_data();


GDDEF_ENCODE()
{
  G_STD_DECL;
  int i;
  q   elem;
  long size;

  Shallow(GD_SELF);
  size = GD_SELF->index;
  depth--;
  for(i = 0; i<size; i++){
    elem = element_of_vector(makefunctor(GD_SELF), makeint(i));
    if (elem == 0){
      fatal("Error in element_of_vector");
    }
    encode_data(buffer, elem, depth);
  }
  PUT_BUFFER(buffer, decode_vector);
  PUT_BUFFER(buffer, size);

  return(GENERIC_SUCCEEDED);
}

/*
GDDEF_ENCODE()
{
  G_STD_DECL;
  int i;
  q   elem;
  long size;

  Shallow(GD_SELF);
  size = GD_SELF->index;

  PUT_BUFFER(buffer, decode_vector);
  PUT_BUFFER(buffer, size);
  for(i = 0; i<size; i++){
    elem = element_of_vector(makefunctor(GD_SELF), makeint(i));
    if (elem == 0){
      fatal("Error in element_of_vector");
    }
    encode_data(buffer, elem, eager_transfer_level);
  }
  return(GENERIC_SUCCEEDED);
}
*/
#endif


#ifdef SHM
GDDEF_SHMCOPY()
{
  G_STD_DECL;
  GD_OBJ_TYPE *newself;

  newself = (GD_OBJ_TYPE *)galloc((sizeof(struct vector_object)));
  Shallow(GD_SELF);
  {
    /* Shallow version */
    q *body = GD_SELF->body;
    long size = GD_SELF->index;
    q *newbody;
    long k;
    newbody = (q*)galloc(size);
    newself->method_table = GD_SELF->method_table;
    newself->next = VECTOR_SHALLOW_MARK;
    newself->index = size;
    newself->iscnst = 1;
    newself->body = newbody;
    for (k=0; k<size; k++) {
        shm_arg_copy(&body[k], &newbody[k]);
      }
  }
  return(makefunctor(newself));
}
#endif

/* Generic method */

GDDEF_METHOD(element_2)
{
  G_STD_DECL;
  int position;
  Shallow(GD_SELF);
  GDSET_INTARG_WITHIN_RANGE(position,GD_ARGV[0],0,GD_SELF->index);
  GD_UNIFY(GD_ARGV[1], GD_SELF->body[position]);
  GD_RETURN;
}

GDDEF_METHOD(size_1)
{
  G_STD_DECL;
  Shallow(GD_SELF);
  GD_UNIFY_VALUE(GD_ARGV[0], G_MAKEINT(GD_SELF->index));
  GD_RETURN;
}

GDDEF_METHOD(set__element_3)
{
  G_STD_DECL;
  long position;
  long size;
  GD_OBJ_TYPE *newvect;
  q *body;
  int iscnst;

  Shallow(GD_SELF);
  size = GD_SELF->index;
  iscnst = GD_SELF->iscnst;
  body = GD_SELF->body;
  GDSET_INTARG_WITHIN_RANGE(position,GD_ARGV[0],0,size);
  GDSET_NEWOBJ(newvect);
  if (!iscnst) {
    q olddata = body[position];

    GD_SELF->index = position;
    GD_SELF->body = (q *)olddata;
    body[position] = GD_ARGV[1];
    GD_SELF->next = GD_OBJ(newvect);
    newvect->body = body;
  } else {
    long k;
    q *newbody;
    GD_ALLOC_AREA(newbody,(q*),size);
    for (k=0; k<size; k++) newbody[k] = body[k];
    newbody[position] = GD_ARGV[1];
    newvect->body = newbody;
  }
  newvect->next = VECTOR_SHALLOW_MARK;
  newvect->index = size;
  newvect->iscnst = 0;
  GD_UNIFY_VALUE(GD_ARGV[2], GD_OBJ(newvect));
  GD_RETURN;
}

GDDEF_METHOD(set__element_4)
{
  G_STD_DECL;
  long position;
  long size;
  GD_OBJ_TYPE *newvect;
  q *body;
  int iscnst;
  q olddata;

  Shallow(GD_SELF);
  size = GD_SELF->index;
  body = GD_SELF->body;
  iscnst = GD_SELF->iscnst;

  GDSET_INTARG_WITHIN_RANGE(position,GD_ARGV[0],0,size);
  GDSET_NEWOBJ(newvect);
  olddata = body[position];
  GD_UNIFY(GD_ARGV[1], olddata);
  if (!iscnst) {
    GD_SELF->index = position;
    GD_SELF->body = (q *)olddata;
    body[position] = GD_ARGV[2];
    GD_SELF->next = GD_OBJ(newvect);
    newvect->body = body;
  } else {
    long k;
    q *newbody;
    GD_ALLOC_AREA(newbody,(q*),size);
    for (k=0; k<size; k++) newbody[k] = body[k];
    newbody[position] = GD_ARGV[2];
    newvect->body = newbody;
  }
  newvect->next = VECTOR_SHALLOW_MARK;
  newvect->index = size;
  newvect->iscnst = 0;
  GD_UNIFY_VALUE(GD_ARGV[3], GD_OBJ(newvect));
  GD_RETURN;
}

GDDEF_METHOD(split_3)
{
  G_STD_DECL;
  long split_point;
  unsigned long size, lower_size;
  q *upper_body, *lower_body;
  struct vector_object *upper, *lower;

  Shallow(GD_SELF);
  size = GD_SELF->index;
  GDSET_INTARG_WITHIN_RANGE(split_point,GD_ARGV[0],0,size+1);
  lower_size = size-split_point;
  GD_ALLOC_AREA(upper_body, (q *), split_point);
  GD_ALLOC_AREA(lower_body, (q *), lower_size);
  GD_ALLOC_AREA(upper, (GD_OBJ_TYPE *), GD_OBJ_SIZE(GD_SELF));
  GD_ALLOC_AREA(lower, (GD_OBJ_TYPE *), GD_OBJ_SIZE(GD_SELF));
  BCOPY((char *)GD_SELF->body, (char *)upper_body, split_point*sizeof(q));
  BCOPY((char *)(GD_SELF->body+split_point), (char *)lower_body,
	lower_size*sizeof(q));
  upper->method_table = lower->method_table = GD_SELF->method_table;
  upper->next = lower->next = VECTOR_SHALLOW_MARK;
  upper->index = split_point;
  lower->index = lower_size;
  upper->body = upper_body;
  lower->body = lower_body;
  upper->iscnst = lower->iscnst = 0;
  GD_UNIFY_VALUE(GD_ARGV[1],G_MAKEFUNCTOR(upper));
  GD_UNIFY_VALUE(GD_ARGV[2],G_MAKEFUNCTOR(lower));
  GD_RETURN;
}

GDDEF_METHOD(join_2)
{
  G_STD_DECL;
  unsigned long size1, size2, newsize;
  q anotherq = GD_ARGV[0];
  GD_OBJ_TYPE *another, *new;
  q *newbody;

  Shallow(GD_SELF);
  size1 = GD_SELF->index;
  GD_DEREF(anotherq);
  another = (GD_OBJ_TYPE*)G_FUNCTORP(anotherq);
  if (!G_ISFUNCTOR(anotherq) ||
      another->method_table != GD_SELF->method_table) {
    debug_printf("### %k ###\n", another);
    GD_FAIL("Illegal argument to string join");
  }
  Shallow(another);
  size2 = another->index;
  newsize = size1+size2;
  GD_ALLOC_AREA(newbody, (q *), newsize);
  GD_ALLOC_AREA(new, (GD_OBJ_TYPE *), GD_OBJ_SIZE(GD_SELF));
  Shallow(GD_SELF);
  BCOPY((char *)GD_SELF->body, (char *)newbody, size1*sizeof(q));
  Shallow(another);
  BCOPY((char *)another->body, (char *)(newbody+size1), size2*sizeof(q));
  new->method_table = GD_SELF->method_table;
  new->next = VECTOR_SHALLOW_MARK;
  new->index = newsize;
  new->body = newbody;
  new->iscnst = 0;
  GD_UNIFY_VALUE(GD_ARGV[1],G_MAKEFUNCTOR(new));
  GD_RETURN;
}

/*  Generic Method Table */
GDDEF_GENERIC()
{
  G_STD_DECL;

  GD_SWITCH_ON_METHOD{
    GD_METHOD_CASE(element_2);
    GD_METHOD_CASE(size_1);
    GD_METHOD_CASE(set__element_3);
    GD_METHOD_CASE(set__element_4);
    GD_METHOD_CASE(split_3);
    GD_METHOD_CASE(join_2);
    GD_METHOD_CASE_DEFAULT;
  }
  GD_RETURN;
}

/* guard generic methods */

GDDEF_GMETHOD(element_2)
{
  G_STD_DECL;
  long position;

  Shallow(GD_SELF);
  GDSET_GINTARG_WITHIN_RANGE(position,GD_ARGV[0],0,GD_SELF->index);
  GD_ARGV[1] = GD_SELF->body[position];
  GD_GSUCCEED;
}

GDDEF_GMETHOD(vector_1)
{
  G_STD_DECL;
  Shallow(GD_SELF);
  GD_ARGV[0] = G_MAKEINT(GD_SELF->index);
  GD_GSUCCEED;
}

GDDEF_GGENERIC()
{
  G_STD_DECL;
  GD_SWITCH_ON_GMETHOD {
    GD_GMETHOD_CASE(element_2);
    GD_GMETHOD_CASE(vector_1);
    GD_GMETHOD_CASE_DEFAULT;
  }
}

GDDEF_PRINT()
{
  G_STD_DECL;
  unsigned long size, limit, k;
  Shallow(GD_SELF);
  size = GD_SELF->index;
  limit = (size > g_length ? g_length : size);
  GD_PRINT("{");
  if (size != 0) GD_PRINT_KL1_TERM(GD_SELF->body[0]);
  for (k=1; k<limit; k++) {
    GD_PRINT(",");
    if (size>0) GD_PRINT_KL1_TERM(GD_SELF->body[k]);
  }
  if (size != limit) GD_PRINT(",..");
  GD_PRINT("}");
  GD_RETURN_FROM_PRINT;
}

GDDEF_COMPARE()
{
  G_STD_DECL;
  unsigned long size1, size2, limit, k;
  Shallow(GD_SELF);
  size1 = GD_SELF->index;
  Shallow(GD_OTHER);
  size2 = GD_OTHER->index;
  limit = (size1 <= size2 ? size1 : size2);
  for (k = 0; k < limit; k++) {
    q elem, tmp;
    Shallow(GD_SELF);
    elem = GD_SELF->body[k];
    Shallow(GD_OTHER);
    tmp = builtin_compare_terms(elem, GD_OTHER->body[k]);
    if (!isint(tmp)) return tmp;
    if (tmp != G_MAKEINT(0)) return tmp;
  }
  if (size1 != size2) {
    return G_MAKEINT(size1 >= size2 ? size1+1 : -(size2+1));
  } else {
    return G_MAKEINT(0);
  }
}

GDDEF_HASH()
{
  G_STD_DECL;
  unsigned long size;
  extern q hash_kl1_term();
  Shallow(GD_SELF);
  size = GD_SELF->index;
  if (GD_LEVEL == 0 || size == 0) {
    return G_MAKEINT((long)GD_SELF->method_table + 0xb5d*size);
  } else {
    q h1, h2, h3;
    GD_LEVEL--;
    h1 = hash_kl1_term(GD_SELF->body[0], GD_LEVEL);
    if (!isint(h1)) return h1;
    h2 = hash_kl1_term(GD_SELF->body[size>>1], GD_LEVEL);
    if (!isint(h1)) return h2;
    h3 = hash_kl1_term(GD_SELF->body[size-1], GD_LEVEL);
    if (!isint(h1)) return h3;
    return
      G_MAKEINT(0x813*intval(h1) + 0x425*intval(h2) + 0x3c9*intval(h3) +
		(long)GD_SELF->method_table + 0xb5d*size);
  }
}

#define GDUSE_MY_GUNIFY
#define GDUSE_MY_UNIFY
#define GDUSE_MY_PRINT
#define GDUSE_MY_GC
#ifdef DIST
#define GDUSE_MY_ENCODE
#endif
#ifdef SHM
#define GDUSE_MY_SHMCOPY
#endif
#define GDUSE_MY_GENERIC
#define GDUSE_MY_GGENERIC
#define GDUSE_MY_COMPARE
#define GDUSE_MY_HASH

/* define the method table structure of the vector */

#include <klic/gd_methtab.h>

/*  new_vector function */
GDDEF_NEW()
{
  GD_STD_DECL_FOR_NEW;
  GD_OBJ_TYPE *newvect;
  q *body;
  long size, k;
  q init;

  if (GD_ARGC!=1) GD_ERROR_IN_NEW("Too few or too many arguments");
  init = GD_ARGV[0];
  GD_DEREF_FOR_NEW(init);
  GDSET_NEWOBJ_FOR_NEW(newvect, G_SIZE_IN_Q(GD_OBJ_TYPE));
  if (G_ISINT(init)) {
    size = G_INTVAL(init);
    if (size < 0) GD_ERROR_IN_NEW("Negative size specified");
    GD_ALLOC_AREA_FOR_NEW(body, (q*), size);
    for (k=0; k<size; k++) body[k] = G_MAKEINT(0L);
  } else if (init==NILATOM || G_ISCONS(init)) {
    for (size=0; ; size++) {
      if (init == NILATOM) break;
      init = G_CDR_OF(init);
      GD_DEREF_FOR_NEW(init);
      if (init!=NILATOM && !G_ISCONS(init))
	GD_ERROR_IN_NEW("Illegal parameter");
    }
    init = GD_ARGV[0];
    GD_ALLOC_AREA_FOR_NEW(body, (q*), size);
    for (k=0; k<size; k++) {
      GD_DEREF_FOR_NEW(init);
      body[k] = G_CAR_OF(init);
      init = G_CDR_OF(init);
    }
  } else {
    GD_ERROR_IN_NEW("Illegal parameter");
  }
  newvect->next = VECTOR_SHALLOW_MARK;
  newvect->iscnst = 0;
  newvect->index = size;
  newvect->body = body;
  GD_RETURN_FROM_NEW(newvect);
}

/* Interface with builtin predicates */

q create_vector(body, size, g_allocp)
     q *body;
     long size;
     q *g_allocp;
{
  G_STD_DECL;
  struct vector_object *newvect;
  heapalloc(newvect, G_SIZE_IN_Q(GD_OBJ_TYPE), (GD_OBJ_TYPE *));
  newvect->method_table = &GD_method_table;
  newvect->next = VECTOR_SHALLOW_MARK;
  newvect->iscnst = 0;
  newvect->index = size;
  newvect->body = body;
  return G_MAKEFUNCTOR(newvect);
}

q size_of_vector(v)
     q v;
{
  Shallow(VECTOR_OBJ(v));
  return G_MAKEINT(VECTOR_OBJ(v)->index);
}

q element_of_vector(v, k)
     q v, k;
{
  Shallow(VECTOR_OBJ(v));
  if (G_INTVAL(k) < 0 || VECTOR_OBJ(v)->index <= G_INTVAL(k)) {
    return (q) 0;
  } else {
    return VECTOR_OBJ(v)->body[G_INTVAL(k)];
  }
}

#ifdef DIST


void push_decode_stack();
q    pop_decode_stack();

q *decode_vector(inbuf, g_allocp)
     combuf *inbuf;
     q *g_allocp;
{
  G_STD_DECL;
  GD_OBJ_TYPE *newvect;
  q *body;
  long size, k;
  q res;
  module decoder;

/*  GDSET_NEWOBJ_FOR_NEW(newvect ,G_SIZE_IN_Q(GD_OBJ_TYPE));*/
  G_HEAPALLOC_WITH_CHECK(newvect, G_SIZE_IN_Q(GD_OBJ_TYPE),
                          (GD_OBJ_TYPE *), g_allocp, res);
/*  G_HEAPALLOC(newvect, G_SIZE_IN_Q(GD_OBJ_TYPE), (GD_OBJ_TYPE *));*/

  if(GENERIC_GCREQUEST == res){
      this_more_space += G_SIZE_IN_Q(GD_OBJ_TYPE);
      
      inbuf->rd_index--;

/*      fprintf(stderr, "Node %d Heap shortage in decoding vector(1)",my_node);*/
      return(g_allocp);
  }
  newvect->method_table = &GD_method_table;
  newvect-> index = size = (long)GET_BUFFER(inbuf);

/*  GD_ALLOC_AREA_FOR_NEW(body, (q*), size);*/
  G_HEAPALLOC_WITH_CHECK(body, size, (q*), g_allocp, res); 
/*  G_HEAPALLOC(body, size, (q*));*/
  if(GD_GCREQUEST == res){
      this_more_space += size;
      
      inbuf->rd_index -= 2;

/*      fprintf(stderr, "Node %d Heap shortage in decoding vector(2)",my_node);*/
      return(g_allocp);
  }

  for(k = size; k > 0 ; k--){
    body[k-1] = pop_decode_stack();
  }

  newvect->next   = VECTOR_SHALLOW_MARK;
  newvect->iscnst = 0;
  newvect->body = body;

  push_decode_stack((q)makefunctor(newvect));
  return(g_allocp);
}

/*q *decode_vector(inbuf, g_allocp)
     combuf *inbuf;
     q *g_allocp;
{
  G_STD_DECL;
  GD_OBJ_TYPE *newvect;
  q *body;
  long size, k;
  module decoder;

/ *  EPrint("Decode vector\n");* /

/ *  GDSET_NEWOBJ_FOR_NEW(newvect ,G_SIZE_IN_Q(GD_OBJ_TYPE));* /
/ *  G_HEAPALLOC_WITH_CHECK(newvect, G_SIZE_IN_Q(GD_OBJ_TYPE),
                          (GD_OBJ_TYPE *), g_allocp, res);* /
  G_HEAPALLOC(newvect, G_SIZE_IN_Q(GD_OBJ_TYPE), (GD_OBJ_TYPE *));

/ *  if(GENERIC_GCREQUEST == res){
    fatal("GC while decoding vector is not supported yet(1)");
    }* /
  newvect->method_table = &GD_method_table;
  newvect-> index = size = (long)GET_BUFFER(inbuf);

/ *  GD_ALLOC_AREA_FOR_NEW(body, (q*), size);* /
/ *  G_HEAPALLOC_WITH_CHECK(body, size, (q*), g_allocp, res); * /
  G_HEAPALLOC(body, size, (q*));
/ *  if(GD_GCREQUEST == res){
    fatal("GC while decoding vector is not supported yet(2)");
    }* /

  for(k = 0; k<size ; k++){
    decoder = (module)GET_BUFFER(inbuf);
    g_allocp = (q *)decoder(inbuf, g_allocp);
    body[k] = decode_data;
  }

  newvect->next   = VECTOR_SHALLOW_MARK;
  newvect->iscnst = 0;
  newvect->body = body;

  decode_data = (q)makefunctor(newvect);
  return(g_allocp);
}
*/
#endif
