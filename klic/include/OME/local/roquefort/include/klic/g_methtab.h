/* ---------------------------------------------------------- 
%   (C)1993,1994,1995 Institute for New Generation Computer Technology 
%       (Read COPYRIGHT for detailed information.) 
%   (C)1996, 1997, 1998, 1999 Japan Information Processing Development Center
%       (Read COPYRIGHT-JIPDEC for detailed information.)
----------------------------------------------------------- */

#ifndef G_METHOD_TABLE_H
#define G_METHOD_TABLE_H

#define GENERIC_SUCCEEDED (q)0
#define GENERIC_FAILED    (q)1
#define GENERIC_GCREQUEST (q)2

/*
  Data Object
*/

struct data_object {
  struct data_object_method_table *method_table;
};

struct data_object_method_table {
  q (*passive_unify)( /* struct data_object *one,
			 struct data_object *another */ );
  q *(*active_unify)( /* struct data_object *one,
              with method_result      struct data_object *another,
                                      q * allocp */ );
  long (*print)( /* struct iimutable_object * obj,
		    FILE *out_stream,
		    unsigned long depth,
		    unsigned long length */ );
  q *(*gc)( /* struct data_object *obj, q *allocp , q **sp */ );
  long (*regist)( /* struct data_object *obj */ );
  long (*deallocate)( /* struct data_object *obj */ );
  q *(*close)( /* struct data_object *obj, */ );
  q *(*body_generic)( /* struct data_object *obj,
				      q method_functor,
				      q *argv,
				      q *allocp */ );
  q (*g_generic)( /* struct data_object *obj,
		     q method_functor,
		     q *args */ );
  q (*compare)( /* struct data_object *obj,
		   struct data_object *another_obj */ );
  q (*hash)( /* struct data_object *obj, unsigned long level */ );
  q (*encode)(/* struct data_object *obj, long node */);
  q (*shmcopy)(/* struct data_object *obj */);
};

#define data_objectp(obj) \
     ((struct data_object *)(((unsigned long)(obj)) - FUNCTOR))


/*
  Consumer Object
*/

struct consumer_object {
  struct consumer_object_method_table *method_table;
};

struct consumer_object_method_table {
  q *(*active_unify)(/* struct consumer_object *self,
		with method_result   q other,
				     q *allocp */ );
  long (*print)(/* struct consumer_object *obj,
		  FILE *out_stream,
		  unsigned long max_depth,
		  unsigned long max_length */);
  q *(*gc)(/* struct consumer_object *obj, q *allocp, q **sp */);
  long (*regist)(/* struct consumer_object *obj */ );
  long (*deallocate)( /* struct consumer_object *obj */ );
  q *(*close)( /* struct consumer_object *obj */ );
  q (*encode)(/* struct consumer_object *obj, long node */);
};

/*
  Generator Object
*/

struct generator_object_method_table {
  q *(*active_unify)(/* q self,
			q other,
			q  *allocp */);
  q (*generate)(/* struct generator_object *self,
		q *allocp */);
  q (*suspend)(/* q reference, struct goalrec *goal */);
  long (*print)/* print what ? */ (/* struct generator_object *self,
		  FILE *out_stream,
		  unsigned long max_depth,
		  unsigned long max_length */);
  q *(*gc)( /* struct generator_object *self,
	       q *allocp,
	       q **sp */);
  long (*regist)(/* struct generator_object *self */);
  long (*deallocate)(/* struct generator_object *self */);
  q *(*close)(/* struct generator_object *self */);
  q (*encode)(/* struct generator_object *self, long node */);
};
		    
struct generator_object {
  struct generator_object_method_table *method_table;
};

#endif
