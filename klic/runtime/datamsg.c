/* ---------------------------------------------------------- 
%   (C)1994,1995 Institute for New Generation Computer Technology 
%       (Read COPYRIGHT for detailed information.) 
%   (C)1996, 1997, 1998, 1999 Japan Information Processing Development Center
%       (Read COPYRIGHT-JIPDEC for detailed information.)
----------------------------------------------------------- */
#include <klic/basic.h>
#include <klic/struct.h>
#include <klic/primitives.h>
#include <klic/unify.h>
#include <klic/functorstuffs.h> 
#include <klic/timing.h>

#include <klic/gb.h>
#include <klic/gobj.h>
#include <klic/susp.h>
#include <klic/functorstuffs.h>
#include <klic/atomstuffs.h>

#include <stdio.h>

#include <klic/interpe.h>

#include <klic/generic.h> 
#include <klic/index.h> 
#include <klic/alloc.h>

#include <klic/distio.h>

extern Const long initial_atoms;
extern Const long initial_functors;
q search_exptbl();
extern q exref_g_new();
/* 
  public entry point
  DATA ENCODER			void encode_data(buffer, data, depth)

  specially defined as extern for reference from exref generic object;
  EXREF DECODER			q* decode_exref(inbuf, allocp)
  decode atomic or const	static q* decode_atomic_or_const(inbuf, allocp)
  decode list			static q* decode_list(inbuf, allocp)
  decode funct			static q* decode_funct(inbuf, allocp)
  */

void push_decode_stack();
q pop_decode_stack();

/* decoding data */
/*
  LEVEL0 OBJECTS encoding
  e.g. const, atomic and ref
  */
static q* decode_atomic_or_const(inbuf, allocp)
     combuf *inbuf;
     q *allocp;
{
  declare_globals;

  INT_CL_DEBUG_X(ioprintf("%d:decode_const_or_atomic\n", my_node));

  decode_data = (q)GET_BUFFER(inbuf);
  push_decode_stack(decode_data);
  return(allocp);
}

q* decode_exref(inbuf, allocp)
     combuf *inbuf;
     q *allocp;
{
  declare_globals;
  long node;

  INT_CL_DEBUG_X(ioprintf("%d:decode_exref\n", my_node));

  node = GET_BUFFER(inbuf);
  if(node == my_node) {
      /* If the data is data exported by myself */
    long index = GET_BUFFER(inbuf);
    long wec   = GET_BUFFER(inbuf);

    decode_data = (q)search_exptbl(index, wec);
    push_decode_stack(decode_data);
    return(allocp);

  } else {
      /* If the data is others data */
    generic_arg[0] = makeint(node);  /* PE number */
    generic_arg[1] = makeint(GET_BUFFER(inbuf));  /* exp_table index */
    generic_arg[2] = makeint(GET_BUFFER(inbuf));  /* WEC */

    new_generic(exref_g_new, 3L, decode_data, 0); 
    push_decode_stack(decode_data);
    return(allocp);
  }
}
/*
  decode  STRUCTURED VALUE
  e.g. lists and functors
 */
static q* decode_list(inbuf, allocp)
     combuf *inbuf;
     q *allocp;
{
  declare_globals;
  q list;
  module decoder;

  INT_CL_DEBUG_X(ioprintf("%d:decode_list\n", my_node));

 loop:
  heapalloc(list, sizeof(struct cons)/sizeof(q), makecons);

  cdr_of(list) = pop_decode_stack();
  car_of(list) = pop_decode_stack();
  push_decode_stack(list);
  return(allocp);
}

static q* decode_funct(inbuf, allocp)
     combuf *inbuf;
     q *allocp;
{
  declare_globals;
  q funct, p_funct;
  module decoder;
  int i;

  INT_CL_DEBUG_X(ioprintf("%d:decode_funct\n", my_node));

  p_funct = (q)GET_BUFFER(inbuf);


  if (( real_heaplimit - allocp ) < ( arityof(p_funct) + 1 )) {
    inbuf->rd_index -= 2;
    heaplimit = 0;
    this_more_space += arityof(p_funct)+1;
    return (allocp);
  }

  heapalloc(funct, arityof(p_funct)+1, makefunctor);

  functor_of(funct) = p_funct;
  for(i=arityof(p_funct) ; i > 0; i--){
    arg(funct, i-1) = pop_decode_stack();
  }
  push_decode_stack(funct);
  return(allocp);
}

/*
  encode  STRUCTURED VALUE
  if depth is negative, entire data structures are encoded.
  if depth is zero, only one word structures are encoded.
  */

#define   END_ENCODE_DATA()\
{\
    int i;\
    for(i = 0 ; i < cdrlist ; i++){\
	PUT_BUFFER(buffer, decode_list);\
    }\
}

void encode_data(buffer, data, depth)
combuf *buffer;
q data;
long depth;
{
  int i,depth_org;
  declare_globals;
  int cdrlist = 0;
/*  depth_org = depth;*/
  
 again:
  switch (ptagof(data)){
    case ATOMIC:
      if (issym(data) &&
	  symval(data) >= initial_atoms+ATOMNUMBERBASE) {
	ioeprintf("initial_atoms %d\n",initial_atoms);
	ioeprintf("symval(data) %d\n",symval(data));
	fatal("cannot send a newly created atom in this version.");
      } else {
	  goto Atomic_case;
      } 
      break;
    case CONS:
      if ((q*)data < heaptop ||
	  (q*)data >= heaptop+heapsize+incrementsize) {
	  goto Const_case;
      } else if (depth == 0) {
	  goto Make_exref;
      } else {
	  if (depth > 0)
	    depth--;
	  encode_data(buffer, car_of(data), depth);
          data = cdr_of(data);
	  cdrlist++;
          goto again;

/*	  encode_data(buffer, cdr_of(data), depth);	      
	  PUT_BUFFER(buffer, decode_list);*/
      }
      break;
    case FUNCTOR:
      if ((q*)data < heaptop ||
	  (q*)data >= heaptop+heapsize+incrementsize) {
	  goto Const_case;
      } else {
	q f = functor_of(data);
	if (isref(f)) { /* data_object */
	  if(depth == 0){
	    goto Make_exref;
	  } else {
	    if ( generic_encode((struct data_object *)functorp(data),
                                                        buffer, depth) ) {
	      fatal("Encode method is not defined in Data Object");
	    }
	  }
	} else {      /* functor */
	  if((symval(f) - FUNCTORNUMBERBASE) >= initial_functors){
	    ioeprintf("initial_functors %d\nsymval(data) %d\n",
		    initial_functors, symval(f));
	    fatal("cannot send a newly created functor in this version.");
	  } else if (depth == 0) {
	      goto Make_exref;
	  } else {
	    int arity1 = arityof(functor_of(data));
	    if (depth > 0)
	      depth--;
	    for (i = 0; i < arity1; i++) {
		encode_data(buffer, arg(data, i), depth);
	    }
	    PUT2_BUFFER(buffer, decode_funct, functor_of(data));  
	  }
	}
      }
      break;	   
      case VARREF:
      {
	q temp = derefone(data);
      deref_x:
	if (data != temp){
	  if (isref(temp)) {
	    q temp1 = derefone(temp);
	    if (temp1 == data) {
	      struct susprec *sdata = (struct susprec *)temp;
	      if(is_generator_susp(sdata->u)){
		/* generator */
		struct generator_susp *genobj = generator_suspp(sdata);

		if(generic_encode(untag_generator_susp(genobj->u.o), buffer, depth)){
		  goto Make_exref;
		}

	      } else {
		/* consumer, goal */
		goto Make_exref;
	      }
	    } else {
		/* REF chain continued */
	      data = temp;
	      temp = temp1;
	      goto deref_x;
	    }
	  } else {
	    /* Already instantiated */
	    data = temp;
	    goto again;
	  }
	} else {
	  /* Simple variable */
	  goto Make_exref;
	}
      }
      break;
    }
  END_ENCODE_DATA();
  return;
 Make_exref:
  {
      long index = regist_exptbl(data);
      PUT4_BUFFER(buffer, decode_exref, my_node, index, EXPORT_UNIT_WEC);
      END_ENCODE_DATA();
      return;
  }
 Atomic_case:
 Const_case:
  PUT2_BUFFER(buffer, decode_atomic_or_const, data);
  END_ENCODE_DATA();
  return;
}
