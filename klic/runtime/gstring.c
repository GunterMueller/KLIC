/* ---------------------------------------------------------- 
%   (C)1993,1994,1995 Institute for New Generation Computer Technology 
%       (Read COPYRIGHT for detailed information.) 
%   (C)1996, 1997, 1998, 1999 Japan Information Processing Development Center
%       (Read COPYRIGHT-JIPDEC for detailed information.)
----------------------------------------------------------- */

#include <stdio.h>
#include <ctype.h>
#include <klic/gdobject.h>
#include <klic/g_string.h>
#include "atom.h"
#include "funct.h"
#ifdef STRINGH
#include <string.h>
#else
#include <strings.h>
#endif

#ifdef SHM
#include <klic/shm.h>
#endif

#ifdef DIST
#include <klic/interpe.h>
#endif

#define GD_CLASS_NAME() byte__string
#define GD_OBJ_TYPE struct byte_string_object
#define GD_OBJ_SIZE(obj) (G_SIZE_IN_Q(GD_OBJ_TYPE))
#define ELEMSIZE 8

#include <klic/gd_macro.h>

#define STRING_OBJ(x)	((GD_OBJ_TYPE *)(G_FUNCTORP(x)))

/* shallowing */

static do_shallow(string)
     struct byte_string_object *string;
{
  q s, last, next;

  /* Go down to the shallwed version inverting pointers */
  last = STRING_SHALLOW_MARK;
  s = GD_OBJ(string);
  do {
    next = STRING_OBJ(s)->next;
    STRING_OBJ(s)->next = last;
    last = s;
    s = next;
  } while (s != STRING_SHALLOW_MARK);
  s = last;

  /* Update physical string elements tracing the inverted pointers */
  {
    long size = STRING_OBJ(s)->index;
    unsigned char *body = STRING_OBJ(s)->body;
    next = STRING_OBJ(s)->next;
    do {
      long index;
      index = STRING_OBJ(next)->index;
      STRING_OBJ(s)->index = index;
      STRING_OBJ(s)->body = (unsigned char *)((unsigned long)body[index]);
      body[index] = (unsigned char)(unsigned long)(STRING_OBJ(next)->body);
      s = next;
      next = STRING_OBJ(s)->next;
    } while (next != STRING_SHALLOW_MARK);
    STRING_OBJ(s)->index = size;
    STRING_OBJ(s)->body = body;
  }
}

#define Shallow(string) \
{ if (!IS_SHALLOW_STRING(string)) do_shallow(string); }

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
  if (IS_SHALLOW_STRING(GD_SELF)) {
    if (BCMP(GD_SELF->body, GD_OTHER->body, size) != 0) GD_GUNIFY_FAIL;
  } else {
    for (k=0; k<size; k++) {
      unsigned char c = GD_OTHER->body[k];
      Shallow(GD_SELF);
      if (GD_SELF->body[k] != c) GD_GUNIFY_FAIL;
      Shallow(GD_OTHER);
    }
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
  if (IS_SHALLOW_STRING(GD_SELF)) {
    if (BCMP(GD_SELF->body, GD_OTHER->body, size) != 0) GD_UNIFY_FAIL;
  } else {
    for (k=0; k<size; k++) {
      unsigned char c = GD_OTHER->body[k];
      Shallow(GD_SELF);
      if (GD_SELF->body[k] != c) GD_UNIFY_FAIL;
      Shallow(GD_OTHER);
    }
  }
  GD_RETURN;
}

#define ROUND_UP(size)	(((size)+sizeof(q)-1)/sizeof(q))

GDDEF_GC()
{
  G_STD_DECL;
  GD_OBJ_TYPE *newself;

  GDSET_NEWOBJ_IN_NEWGEN(newself);
  if (GD_SELF->iscnst) {
    fatal("GC method for constant string unexpected");
  }
  newself->iscnst = 0;
  if (IS_SHALLOW_STRING(GD_SELF)) {
    unsigned char *body = GD_SELF->body;
    unsigned char *newbody;
    unsigned long size = GD_SELF->index;
    unsigned long qsize = ROUND_UP(size);
    if ((g_allocp+qsize)>real_heaplimit) fatal("Not enough space collected");
    newbody = (unsigned char *)g_allocp;
    g_allocp += qsize;
    newself->next = STRING_SHALLOW_MARK;
    newself->index = size;
    newself->body = newbody;
    BCOPY(body,newbody,qsize*sizeof(q));
  } else {
    newself->index = GD_SELF->index;
    newself->body = GD_SELF->body;
    GD_COPY_KL1_TERM_TO_NEWGEN(GD_SELF->next, newself->next);
  }
  GD_RETURN_FROM_GC(newself);
}

#ifdef DIST
q* decode_byte_string();
unsigned char *generic_string_body();

/* for byte string */
GDDEF_ENCODE()  
{
  G_STD_DECL;
  long size;
  int i;
  long *tmp_buf;

  Shallow(GD_SELF);

  PUT_BUFFER(buffer, decode_byte_string);
  PUT_BUFFER(buffer, GD_SELF->index);
  size = ROUND_UP(GD_SELF->index);
  
  tmp_buf = (long *)generic_string_body(GD_SELF);
  for(i = 0 ; i < size ; i++){
    PUT_BUFFER(buffer, tmp_buf[i]);  /* Should be carefule of endian */
  }
  return(GENERIC_SUCCEEDED);
}

#endif

#ifdef SHM
GDDEF_SHMCOPY()
{
  G_STD_DECL;
  GD_OBJ_TYPE *newself;

  newself = (GD_OBJ_TYPE *)galloc((sizeof(struct byte_string_object)));
  Shallow(GD_SELF);
  newself->method_table = GD_SELF->method_table;

  newself->iscnst = 1;
  {
    unsigned char *body = GD_SELF->body;
    unsigned char *newbody;
    long size = GD_SELF->index;
    long qsize = ROUND_UP(size);

    newbody = (unsigned char *)galloc(qsize);
    newself->next = STRING_SHALLOW_MARK;
    newself->index = size;
    newself->body = newbody;
    BCOPY(body,newbody,qsize*sizeof(q));
  }
  return(makefunctor(newself));
}
#endif

/* Generic method */

GDDEF_METHOD(string_2)
{
  G_STD_DECL;
  Shallow(GD_SELF);
  GD_UNIFY_VALUE(GD_ARGV[0],G_MAKEINT(GD_SELF->index));
  GD_UNIFY_VALUE(GD_ARGV[1],G_MAKEINT(ELEMSIZE));
  GD_RETURN;
}

GDDEF_METHOD(element_2)
{
  G_STD_DECL;
  long position;
  Shallow(GD_SELF);
  GDSET_INTARG_WITHIN_RANGE(position,GD_ARGV[0],0,(long)(GD_SELF->index));
  GD_UNIFY_VALUE(GD_ARGV[1],
		 G_MAKEINT((unsigned long)(GD_SELF->body[position])));
  GD_RETURN;
}

GDDEF_METHOD(size_1)
{
  G_STD_DECL;
  Shallow(GD_SELF);
  GD_UNIFY_VALUE(GD_ARGV[0],G_MAKEINT(GD_SELF->index));
  GD_RETURN;
}

GDDEF_METHOD(element__size_1)
{
  G_STD_DECL;
  Shallow(GD_SELF);
  GD_UNIFY_VALUE(GD_ARGV[0],G_MAKEINT(ELEMSIZE));
  GD_RETURN;
}

GDDEF_METHOD(set__element_3)
{
  G_STD_DECL;
  long position, newelem;
  GD_OBJ_TYPE *newstr;
  unsigned char *body;
  int iscnst;
  long size;

  Shallow(GD_SELF);
  size = GD_SELF->index;
  iscnst = GD_SELF->iscnst;
  body = GD_SELF->body;
  GDSET_INTARG_WITHIN_RANGE(position,GD_ARGV[0],0,size);
  GDSET_INTARG_WITHIN_RANGE(newelem,GD_ARGV[1],0,256);
  GDSET_NEWOBJ(newstr);
  if (!iscnst) {
    unsigned char olddata = body[position];

    GD_SELF->index = position;
    GD_SELF->body = (unsigned char *)(unsigned long)olddata;
    body[position] = newelem;
    GD_SELF->next = GD_OBJ(newstr);
    newstr->body = body;
  } else {
    unsigned char *newbody;
    unsigned long qsize = ROUND_UP(size);
    GD_ALLOC_AREA(newbody, (unsigned char *), qsize);
    BCOPY(body, newbody, qsize*sizeof(q));
    newbody[position] = newelem;
    newstr->body = newbody;
  }
  newstr->next = STRING_SHALLOW_MARK;
  newstr->index = size;
  newstr->iscnst = 0;
  GD_UNIFY_VALUE(GD_ARGV[2], GD_OBJ(newstr));
  GD_RETURN;
}

GDDEF_METHOD(split_3)
{
  G_STD_DECL;
  unsigned long size, lower_size;
  long split_point;
  unsigned char *upper_body, *lower_body;
  struct byte_string_object *upper, *lower;

  Shallow(GD_SELF);
  size = GD_SELF->index;
  GDSET_INTARG_WITHIN_RANGE(split_point,GD_ARGV[0],0,size+1);
  lower_size = size-split_point;
  GD_ALLOC_AREA(upper_body, (unsigned char *), ROUND_UP(split_point));
  GD_ALLOC_AREA(lower_body, (unsigned char *), ROUND_UP(lower_size));
  GD_ALLOC_AREA(upper, (GD_OBJ_TYPE *), GD_OBJ_SIZE(GD_SELF));
  GD_ALLOC_AREA(lower, (GD_OBJ_TYPE *), GD_OBJ_SIZE(GD_SELF));
  BCOPY(GD_SELF->body, upper_body, split_point);
  BCOPY(GD_SELF->body+split_point, lower_body, lower_size);
  upper->method_table = lower->method_table = GD_SELF->method_table;
  upper->next = lower->next = STRING_SHALLOW_MARK;
  upper->index = split_point;
  lower->index = lower_size;
  upper->iscnst = lower->iscnst = 0;
  upper->ismbdiff = lower->ismbdiff = 0;
  upper->body = upper_body;
  lower->body = lower_body;
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
  unsigned char *newbody;

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
  GD_ALLOC_AREA(newbody, (unsigned char *), ROUND_UP(newsize));
  GD_ALLOC_AREA(new, (GD_OBJ_TYPE *), GD_OBJ_SIZE(GD_SELF));
  Shallow(GD_SELF);
  BCOPY(GD_SELF->body, newbody, size1);
  Shallow(another);
  BCOPY(another->body, newbody+size1, size2);
  new->method_table = GD_SELF->method_table;
  new->next = STRING_SHALLOW_MARK;
  new->index = newsize;
  new->iscnst = 0;
  new->ismbdiff = 0;
  new->body = newbody;
  GD_UNIFY_VALUE(GD_ARGV[1],G_MAKEFUNCTOR(new));
  GD_RETURN;
}

GDDEF_METHOD(search__character_4)
{
  G_STD_DECL;
  long start, end, code;
  unsigned long k;

  Shallow(GD_SELF);
  GDSET_INTARG_WITHIN_RANGE(start,GD_ARGV[0],0, (long)(GD_SELF->index));
  GDSET_INTARG_WITHIN_RANGE(end,GD_ARGV[1],0, (long)(GD_SELF->index));
  GDSET_INTARG_WITHIN_RANGE(code,GD_ARGV[2],0,256L);
  if (start <= end) {
    for (k=start; k<=end; k++) {
      if (((unsigned long) GD_SELF->body[k]) == code) goto done;
    }
  } else {
    for (k=start; k>=end; k--) {
      if (((unsigned long) GD_SELF->body[k]) == code) goto done;
    }
  }
  GD_UNIFY_VALUE(GD_ARGV[3],G_MAKEINT(-1L));
  GD_RETURN;

  done:
    GD_UNIFY_VALUE(GD_ARGV[3],G_MAKEINT(k));
  GD_RETURN;
}


/*  Generic Method Table */
GDDEF_GENERIC()
{
  G_STD_DECL;

  GD_SWITCH_ON_METHOD{
    GD_METHOD_CASE(string_2);
    GD_METHOD_CASE(element_2);
    GD_METHOD_CASE(size_1);
    GD_METHOD_CASE(element__size_1);
    GD_METHOD_CASE(set__element_3);
    GD_METHOD_CASE(split_3);
    GD_METHOD_CASE(join_2);
    GD_METHOD_CASE(search__character_4);
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
  GDSET_GINTARG_WITHIN_RANGE(position,GD_ARGV[0],0, (long)(GD_SELF->index));
  GD_ARGV[1] = G_MAKEINT((long)(GD_SELF->body[position]));
  GD_GSUCCEED;
}

GDDEF_GMETHOD(string_2)
{
  G_STD_DECL;

  Shallow(GD_SELF);
  GD_ARGV[0] = G_MAKEINT(GD_SELF->index);
  GD_ARGV[1] = G_MAKEINT(ELEMSIZE);
  GD_GSUCCEED;
}

static int compare_two_strings(s1, s2)
     GD_OBJ_TYPE *s1, *s2;
{
  long size1, size2, minsize, k;
  Shallow(s1);
  size1 = s1->index;
  Shallow(s2);
  size2 = s2->index;
  minsize = ((size1 < size2) ? size1 : size2);
  if (!IS_SHALLOW_STRING(s1)) {
    /* s1 and s2 are different versions of the same string */
    for (k=0; k<minsize; k++) {
      unsigned char c;
      Shallow(s1);
      c = s1->body[k];
      Shallow(s2);
      if (c != s2->body[k]) {
	return ((c < s2->body[k]) ? -(k+1) : k+1);
      }
    }
  } else {
    for (k=0; k<minsize; k++) {
      if (s1->body[k] != s2->body[k]) {
	return ((s1->body[k] < s2->body[k]) ? -(k+1) : k+1);
      }
    }
  }
  if (size1 != size2) {
    return ((size1<size2) ? -(size1+1) : size2+1);
  } else {
    return 0;
  }
}

GDDEF_GMETHOD(less__than_1)
{
  G_STD_DECL;
  q otherq = GD_ARGV[0];
  GD_OBJ_TYPE *other;
  int cmp;

  if (!G_ISGOBJ(otherq)) GD_GFAIL;
  other = (GD_OBJ_TYPE *)G_FUNCTORP(otherq);
  if (other->method_table != GD_SELF->method_table) GD_GFAIL;
  cmp = compare_two_strings(GD_SELF, other);
  if (cmp < 0) GD_GSUCCEED; else GD_GFAIL;
}

GDDEF_GMETHOD(not__less__than_1)
{
  G_STD_DECL;
  q otherq = GD_ARGV[0];
  GD_OBJ_TYPE *other;
  int cmp;

  if (!G_ISGOBJ(otherq)) GD_GFAIL;
  other = (GD_OBJ_TYPE *)G_FUNCTORP(otherq);
  if (other->method_table != GD_SELF->method_table) GD_GFAIL;
  cmp = compare_two_strings(GD_SELF, other);
  if (cmp >= 0) GD_GSUCCEED; else GD_GFAIL;
}

GDDEF_GMETHOD(estring_3)
{
  G_STD_DECL;
  long size, k;
  q tmp;

  Shallow(GD_SELF);
  size = GD_SELF->index;
  if (G_INTVAL(GD_ARGV[0])!= size) GD_GFAIL;
  if (G_INTVAL(GD_ARGV[1])!= ELEMSIZE) GD_GFAIL;
  tmp = GD_ARGV[2];
  for(k=0; k<size; k++, tmp = G_CDR_OF(tmp)) {
    if(G_INTVAL(G_CAR_OF(tmp))!= GD_SELF->body[k]) GD_GFAIL;
  }
  GD_GSUCCEED;
}

GDDEF_GGENERIC()
{
  G_STD_DECL;

  GD_SWITCH_ON_GMETHOD {
    GD_GMETHOD_CASE(element_2);
    GD_GMETHOD_CASE(string_2);
    GD_GMETHOD_CASE(less__than_1);
    GD_GMETHOD_CASE(not__less__than_1);
    GD_GMETHOD_CASE(estring_3);
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
  GD_PUTC('"');
  for (k = 0; k < limit; k++) {
    int c = GD_SELF->body[k];
    if (!isgraph(c) && c!=' ') {
      GD_PUTC('\\');
      GD_PUTC('0'+((c>>6)&3));
      GD_PUTC('0'+((c>>3)&7));
      GD_PUTC('0'+(c&7));
    } else {
      if (c=='\\' || c=='"') GD_PUTC('\\');
      GD_PUTC(c);
    }
  }
  if (limit != size) GD_PRINT("..");
  GD_PUTC('"');
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
    unsigned char elem;
    Shallow(GD_SELF);
    elem = GD_SELF->body[k];
    Shallow(GD_OTHER);
    if (elem != GD_OTHER->body[k]) {
      return G_MAKEINT(elem > GD_OTHER->body[k] ? k+1 : -(k+1));
    }
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
  Shallow(GD_SELF);
  size = GD_SELF->index;
  if (size == 0) {
    return G_MAKEINT(0);
  } else {
    return
      G_MAKEINT(0x813 * GD_SELF->body[0] +
		0x425 * GD_SELF->body[size>>1] +
		0x3c9 * GD_SELF->body[size-1]);
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

/*  new_string function */
GDDEF_NEW()
{
  GD_STD_DECL_FOR_NEW;
  GD_OBJ_TYPE *newstr;
  unsigned char *body;
  long size;
  q init;

  if (GD_ARGC!=1) GD_ERROR_IN_NEW("Too few or too many arguments");
  init = GD_ARGV[0];
  GD_DEREF_FOR_NEW(init);
  GDSET_NEWOBJ_FOR_NEW(newstr, G_SIZE_IN_Q(GD_OBJ_TYPE));
  if (G_ISINT(init)) {
    unsigned long qsize;
    size = G_INTVAL(init);
    if (size < 0) GD_ERROR_IN_NEW("Negative size specified");
    qsize = ROUND_UP(size);
    GD_ALLOC_AREA_FOR_NEW(body, (unsigned char *), qsize);
    BZERO(body, qsize*sizeof(q));
  } else if (init==NILATOM || G_ISCONS(init)) {
    unsigned long qsize, k;
    for (size=0; ; size++) {
      q elem;
      if (init == NILATOM) break;
      elem = G_CAR_OF(init);
      GD_DEREF_FOR_NEW(elem);
      if (!G_ISINT(elem) || G_INTVAL(elem) < 0 || 256 <= G_INTVAL(elem)) {
	GD_ERROR_IN_NEW("Illegal parameter");
      }
      init = G_CDR_OF(init);
      GD_DEREF_FOR_NEW(init);
      if (init!=NILATOM && !G_ISCONS(init))
	GD_ERROR_IN_NEW("Illegal parameter");
    }
    qsize = ROUND_UP(size);
    init = GD_ARGV[0];
    GD_ALLOC_AREA_FOR_NEW(body, (unsigned char *), qsize);
    for (k=0; k<size; k++) {
      q elem;
      GD_DEREF_FOR_NEW(init);
      elem = G_CAR_OF(init);
      GD_DEREF_FOR_NEW(elem);
      body[k] = G_INTVAL(elem);
      init = G_CDR_OF(init);
    }
  } else {
    GD_ERROR_IN_NEW("Illegal parameter");
  }
  newstr->next = STRING_SHALLOW_MARK;
  newstr->iscnst = 0;
  newstr->index = size;
  newstr->body = body;
  GD_RETURN_FROM_NEW(newstr);
}

#ifdef DIST
/*q *decode_byte_string(inbuf, g_allocp)
     combuf *inbuf;
     q *g_allocp;
{
  G_STD_DECL;
  GD_OBJ_TYPE *newstring;
  unsigned char *body;
  unsigned long size;
  long *tmp_buf;
  int i;

/ *  printf("Decode string\n");* /

/ *  G_HEAPALLOC_WITH_CHECK(newstring, G_SIZE_IN_Q(GD_OBJ_TYPE), 
			  (GD_OBJ_TYPE *), g_allocp, res);* /
  G_HEAPALLOC(newstring, G_SIZE_IN_Q(GD_OBJ_TYPE), (GD_OBJ_TYPE *));

/ *  if(GENERIC_GCREQUEST == res){ 
    fatal("GC while decoding string is not supported yet(1)");
  }* /
  newstring->method_table = &GD_method_table;
  newstring->index = (long)GET_BUFFER(inbuf);

  size = ROUND_UP(newstring->index);

/ *  G_HEAPALLOC_WITH_CHECK((q *)body, size, (q*), g_allocp, res); * /
  G_HEAPALLOC(body, size, (unsigned char *));

/ *  if(GD_GCREQUEST == res){ 
    fatal("GC while decoding string is not supported yet(2)");
  }* /

  tmp_buf = (long *)body;
  for(i = 0 ; i < size ; i++){
    tmp_buf[i] = GET_BUFFER(inbuf);
  }
  
  newstring->next    = STRING_SHALLOW_MARK;
  newstring->iscnst  = 0;
  newstring->body  = body;

  decode_data = (q)makefunctor(newstring);
  return(g_allocp);
}
*/
void push_decode_stack();

q *decode_byte_string(inbuf, g_allocp)
     combuf *inbuf;
     q *g_allocp;
{
  G_STD_DECL;
  GD_OBJ_TYPE *newstring;
  unsigned char *body;
  unsigned long size;
  long *tmp_buf;
  int i;
  q res;

  G_HEAPALLOC_WITH_CHECK(newstring, G_SIZE_IN_Q(GD_OBJ_TYPE), 
			  (GD_OBJ_TYPE *), g_allocp, res);
/*  G_HEAPALLOC(newstring, G_SIZE_IN_Q(GD_OBJ_TYPE), (GD_OBJ_TYPE *)); */

  if(GENERIC_GCREQUEST == res){ 
      this_more_space += G_SIZE_IN_Q(GD_OBJ_TYPE);
      
      inbuf->rd_index--;

/*      fprintf(stderr, "Node %d Heap shortage in decoding string(1)",my_node);*/
      return(g_allocp);
  }
  newstring->method_table = &GD_method_table;
  newstring->index = (long)GET_BUFFER(inbuf);

  size = ROUND_UP(newstring->index);

  {
    q *tmpbody;
    G_HEAPALLOC_WITH_CHECK(tmpbody, size, (q*), g_allocp, res); 
    body = (unsigned char *) tmpbody;
  }
/*  G_HEAPALLOC(body, size, (unsigned char *));*/

  if(GD_GCREQUEST == res){ 
      this_more_space += size;
      
      inbuf->rd_index -= 2;

/*      fprintf(stderr, "Node %d Heap shortage in decoding string(2)",my_node);*/
      return(g_allocp);
  }

  tmp_buf = (long *)body;
  for(i = 0 ; i < size ; i++){
    tmp_buf[i] = GET_BUFFER(inbuf);
  }
  
  newstring->next    = STRING_SHALLOW_MARK;
  newstring->iscnst  = 0;
  newstring->body  = body;

/*  decode_data = (q)makefunctor(newstring);*/
  push_decode_stack((q)makefunctor(newstring));
  return(g_allocp);
}

#endif

unsigned char *generic_string_body(str)
GD_OBJ_TYPE *str;
{
  Shallow(str);
  return (str->body);
}

unsigned long generic_string_size(str)
GD_OBJ_TYPE *str;
{
  Shallow(str);
  return (str->index);
}

q gd_new_string(size,g_allocp)
     long size;
     q *g_allocp;
{
  q argv[1];
  argv[0] = makeint(size);
  return byte__string_g_new(1,argv,g_allocp);
}

q gd_list_to_string(list,g_allocp)
     q list;
     q *g_allocp;
{
  q argv[2];
  argv[0] = list;
  return byte__string_g_new(1,argv,g_allocp);
}

q convert_c_string_to_klic_string(cstr,g_allocp)
     char *cstr;
     q *g_allocp;
{
  q argv[1];
  q str;
  long len = strlen(cstr);
  argv[0] = makeint(len);
  str = byte__string_g_new(1,argv,g_allocp);
  if (!G_ISREF(str)) {
    BCOPY(cstr, ((struct byte_string_object *)functorp(str))->body, len);
  }
  return str;
}

q convert_binary_c_string_to_klic_string(cstr,len,g_allocp)
     char *cstr;
     long len;
     q *g_allocp;
{
  q argv[1];
  q str;
  argv[0] = makeint(len);
  str = byte__string_g_new(1,argv,g_allocp);
  if (!G_ISREF(str)) {
    BCOPY(cstr, ((struct byte_string_object *)functorp(str))->body, len);
  }
  return str;
}

char *convert_klic_string_to_c_string(s)
     q s;
{
  extern char *malloc_check();
  struct byte_string_object *str =
    (struct byte_string_object *)functorp(s);
  char *cstr;
  Shallow(str);
  cstr = (char *)malloc_check(str->index+1);
  BCOPY((char *)str->body, cstr, str->index);
  cstr[str->index] = '\0';
  return cstr;
}

/* Interface with builtin */

q size_of_string(s)
     q s;
{
  Shallow(STRING_OBJ(s));
  return G_MAKEINT(STRING_OBJ(s)->index);
}

q element_of_string(s, k)
     q s, k;
{
  Shallow(STRING_OBJ(s));
  if (G_INTVAL(k) < 0 || (long)(STRING_OBJ(s)->index) <= G_INTVAL(k)) {
    return (q) 0;
  } else {
    return G_MAKEINT(STRING_OBJ(s)->body[G_INTVAL(k)]);
  }
}

#undef GD_CLASS_NAME
#define GD_CLASS_NAME() string

GDDEF_NEW()
{
  GD_STD_DECL_FOR_NEW;
  long elemsize;

  if (GD_ARGC!=2) GD_ERROR_IN_NEW("Too few or too many arguments");
  GDSET_INTARG_FOR_NEW(elemsize, GD_ARGV[1]);
  if (elemsize != 8) GD_ERROR_IN_NEW("Only byte strings are supported now");
  return byte__string_g_new(1,GD_ARGV,g_allocp);
}
