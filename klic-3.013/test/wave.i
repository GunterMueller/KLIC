# 1 "wave.c"
# 1 "<built-in>"
# 1 "<command-line>"
# 1 "/usr/include/stdc-predef.h" 1 3 4
# 1 "<command-line>" 2
# 1 "wave.c"

# 1 "/usr/local/include/klic/klichdr.h" 1 3







# 1 "/usr/local/include/klic/basic.h" 1 3







# 1 "/usr/local/include/klic/config.h" 1 3
# 9 "/usr/local/include/klic/basic.h" 2 3
# 36 "/usr/local/include/klic/basic.h" 3

# 36 "/usr/local/include/klic/basic.h" 3
typedef void voidfn ();

extern volatile voidfn fatal;
extern volatile voidfn fatalf;
extern volatile voidfn fatalp;
extern void debug_printf();
# 58 "/usr/local/include/klic/basic.h" 3
extern int klic_getc();
extern int klic_gets();
extern int klic_fread();

extern int klic_putc();
extern int klic_fwrite();
extern int klic_fflush();

extern void debug_fprintf();

extern void klic_fprintf();
# 9 "/usr/local/include/klic/klichdr.h" 2 3
# 1 "/usr/local/include/klic/struct.h" 1 3
# 14 "/usr/local/include/klic/struct.h" 3
# 1 "/usr/local/include/klic/param.h" 1 3
# 15 "/usr/local/include/klic/struct.h" 2 3
# 24 "/usr/local/include/klic/struct.h" 3
typedef struct gazonk *q;
# 92 "/usr/local/include/klic/struct.h" 3
struct functor {
  q functor;
  q args[1];
};




struct cons {
  q cdr, car;
};
# 117 "/usr/local/include/klic/struct.h" 3
typedef char * (*module)();

struct predicate {
  module (*func)();
  unsigned short int pred;
  unsigned short int arity;
  const struct arginfo *info;
};



struct arginfo {
  int info;

};



struct goalrec {
  struct goalrec *next;
  const struct predicate *pred;
  q args[6];
};




struct prioqrec {
  struct prioqrec *next;
  long prio;
  struct goalrec *q;
};
# 191 "/usr/local/include/klic/struct.h" 3
extern struct funcdesc {
  q name;
  unsigned long arity;
} functab[1];



struct ext_atom {
  char *name;
  q value;
};

struct ext_funct {
  char *name;
  long arity;
  q value;
};

struct ext_pred {
  char *module;
  char *name;
  long arity;
  q value;
};
# 275 "/usr/local/include/klic/struct.h" 3
extern struct global_variables {

  q *heapp0;
  q * volatile heaplimit0;
  struct goalrec *current_queue0;
  struct goalrec *resumed_goals0;
  unsigned long current_prio0;
  unsigned long top_prio0;

  struct prioqrec prioq0;
  q *heaptop0;
  q *heapbottom0;
  q *real_heaplimit0;
  unsigned long heapsize0, maxheapsize0, incrementsize0;
  unsigned long real_heapbytesize0;
  double maxactiveratio0;
  unsigned long this_more_space0;
  q *new_space_top0, *old_space_top0;
  unsigned long new_space_size0, old_space_size0;
  q **gcstack0;
  q **gcsp0;
  q **gcmax0;
  unsigned long gcstack_size0;
  volatile long interrupt_off0;
  struct goalrec *interrupt_qp0;

  struct {

    long my_num0;
    long num_pes0;
    union {

      struct {
 long queued0;
 struct ex_goalrec* ex_qp0;
 long currid0;
 long oldid0;
 long shm_htop0;
 long shm_hbyte0;
 long dummy[10];
      } shm;


    } aux;
  } par;

  char *program_name0;
  int command_argc0;
  char **command_argv0;
  q* (**gc_hook_table0)();
  int gc_hooktab_size0;
  int num_gc_hooks0;
  q* (**after_gc_hook_table0)();
  int after_gc_hooktab_size0;
  int num_after_gc_hooks0;
  unsigned long
    suspensions0, resumes0, copied_susp0, cum_susps0, cum_resumps0;
  struct suspended_goal_rec *suspended_goal_list0;
  const struct predicate *postmortem_pred0;
  q postmortem_args0;
  long generic_argc0;
  q generic_arg0[64];
  q reasons0[1024L];



} globals;

extern struct global_variables* My_glbl ;







extern long higher_priority_goal;





extern q suspension_reason;
extern q rest_of_stream;
# 10 "/usr/local/include/klic/klichdr.h" 2 3
# 1 "/usr/local/include/klic/primitives.h" 1 3







# 1 "/usr/local/include/klic/alloc.h" 1 3







q *klic_interrupt();
# 9 "/usr/local/include/klic/primitives.h" 2 3
# 1 "/usr/local/include/klic/control.h" 1 3
# 12 "/usr/local/include/klic/control.h" 3
extern q *interrupt_goal();
int check_stack_for_alternatively();
struct goalrec *enqueue_goal();
struct goalrec *enqueue_after_waiting();
struct goalrec *enqueue_throw_goal();
# 9 "/usr/local/include/klic/primitives.h" 2 3
# 11 "/usr/local/include/klic/klichdr.h" 2 3
# 1 "/usr/local/include/klic/unify.h" 1 3







extern q
  *do_unify(), *do_unify2(), *do_unify3(), *do_unify4(),
  *do_unify_value(), *do_unify_value2(),
  *do_unify_value3(), *do_unify_value4();
# 12 "/usr/local/include/klic/klichdr.h" 2 3
# 1 "/usr/local/include/klic/index.h" 1 3
# 13 "/usr/local/include/klic/klichdr.h" 2 3
# 1 "/usr/local/include/klic/gb.h" 1 3







extern q builtin_compare_terms();
extern q builtin_hash_term();
# 204 "/usr/local/include/klic/gb.h" 3
q eq_terms_body();
# 14 "/usr/local/include/klic/klichdr.h" 2 3
# 1 "/usr/local/include/klic/bb.h" 1 3
# 12 "/usr/local/include/klic/bb.h" 3
extern q bblt_add_3();
# 63 "/usr/local/include/klic/bb.h" 3
extern q bblt_sub_3();
# 114 "/usr/local/include/klic/bb.h" 3
extern q bblt_mult_3();
# 163 "/usr/local/include/klic/bb.h" 3
extern q bblt_div_3();
# 214 "/usr/local/include/klic/bb.h" 3
extern q bblt_mod_3();
# 265 "/usr/local/include/klic/bb.h" 3
extern q bblt_and_3();
# 316 "/usr/local/include/klic/bb.h" 3
extern q bblt_or_3();
# 367 "/usr/local/include/klic/bb.h" 3
extern q bblt_exclusive_or_3();
# 418 "/usr/local/include/klic/bb.h" 3
extern q bblt_rshift_3();
# 469 "/usr/local/include/klic/bb.h" 3
extern q bblt_lshift_3();
# 520 "/usr/local/include/klic/bb.h" 3
extern q bblt_complement_2();
# 548 "/usr/local/include/klic/bb.h" 3
extern q bblt_plus_2();
# 575 "/usr/local/include/klic/bb.h" 3
extern q bblt_minus_2();
# 602 "/usr/local/include/klic/bb.h" 3
extern q bblt_fix_2();
# 15 "/usr/local/include/klic/klichdr.h" 2 3
# 1 "/usr/local/include/klic/generic.h" 1 3






# 1 "/usr/local/include/klic/g_methtab.h" 1 3
# 19 "/usr/local/include/klic/g_methtab.h" 3
struct data_object {
  struct data_object_method_table *method_table;
};

struct data_object_method_table {
  q (*passive_unify)(
                                   );
  q *(*active_unify)(

                                                    );
  long (*print)(


                              );
  q *(*gc)( );
  long (*regist)( );
  long (*deallocate)( );
  q *(*close)( );
  q *(*body_generic)(


                       );
  q (*g_generic)(

                  );
  q (*compare)(
                                        );
  q (*hash)( );
  q (*encode)( );
  q (*shmcopy)( );
};
# 59 "/usr/local/include/klic/g_methtab.h" 3
struct consumer_object {
  struct consumer_object_method_table *method_table;
};

struct consumer_object_method_table {
  q *(*active_unify)(

                      );
  long (*print)(


                               );
  q *(*gc)( );
  long (*regist)( );
  long (*deallocate)( );
  q *(*close)( );
  q (*encode)( );
};





struct generator_object_method_table {
  q *(*active_unify)(

                );
  q (*generate)(
              );
  q (*suspend)( );
  long (*print) (


                               );
  q *(*gc)(

                 );
  long (*regist)( );
  long (*deallocate)( );
  q *(*close)( );
  q (*encode)( );
};

struct generator_object {
  struct generator_object_method_table *method_table;
};
# 8 "/usr/local/include/klic/generic.h" 2 3

extern q *gd_generic();
# 16 "/usr/local/include/klic/klichdr.h" 2 3
# 1 "/usr/local/include/klic/g_string.h" 1 3







struct byte_string_object {
  struct data_object_method_table *method_table;
  q next;
  unsigned int index:(8*sizeof(int)-2);
  int iscnst:1;
  int ismbdiff:1;





  unsigned char *body;
};
# 17 "/usr/local/include/klic/klichdr.h" 2 3
# 1 "/usr/local/include/klic/g_vector.h" 1 3







struct vector_object {
  struct data_object_method_table *method_table;
  q next;
  int index:(8*sizeof(int)-1);
  int iscnst:1;





  q *body;
};
# 35 "/usr/local/include/klic/g_vector.h" 3
extern q create_vector();
# 18 "/usr/local/include/klic/klichdr.h" 2 3
# 1 "/usr/local/include/klic/g_float.h" 1 3







struct float_object {
  struct data_object_method_table *method_table;
  double value;
};
# 19 "/usr/local/include/klic/klichdr.h" 2 3
# 1 "/usr/local/include/klic/gmodule.h" 1 3







struct module_object {
  struct data_object_method_table *method_table;
  q name;
};
# 21 "/usr/local/include/klic/gmodule.h" 3
struct predicate_object {
  struct data_object_method_table *method_table;
  const struct predicate *pdesc;
  q module_obj;
  q predicate_name;
};
# 19 "/usr/local/include/klic/klichdr.h" 2 3
# 3 "wave.c" 2
# 1 "atom.h" 1
# 1 "/usr/local/include/klic/atomstuffs.h" 1 3
# 9 "/usr/local/include/klic/atomstuffs.h" 3
extern unsigned char **atomname;
# 2 "atom.h" 2
# 4 "wave.c" 2
# 1 "funct.h" 1
# 1 "/usr/local/include/klic/functorstuffs.h" 1 3
# 9 "/usr/local/include/klic/functorstuffs.h" 3
extern unsigned long *arities;
extern unsigned long *functors;
# 2 "funct.h" 2
# 5 "wave.c" 2


# 6 "wave.c"
module module_main();

# 7 "wave.c" 3
const 
# 7 "wave.c"
     struct predicate predicate_main_xmain_0 =
   { module_main, 0, 0 };

# 9 "wave.c" 3
const 
# 9 "wave.c"
     struct predicate predicate_main_xgo6__6_0 =
   { module_main, 1, 0 };

# 11 "wave.c" 3
const 
# 11 "wave.c"
     struct predicate predicate_main_xgo6__2_0 =
   { module_main, 2, 0 };

# 13 "wave.c" 3
const 
# 13 "wave.c"
     struct predicate predicate_main_xgo10__5_0 =
   { module_main, 3, 0 };

# 15 "wave.c" 3
const 
# 15 "wave.c"
     struct predicate predicate_main_xgo11__5_0 =
   { module_main, 4, 0 };

# 17 "wave.c" 3
const 
# 17 "wave.c"
     struct predicate predicate_main_xgo12__5_0 =
   { module_main, 5, 0 };

# 19 "wave.c" 3
const 
# 19 "wave.c"
     struct predicate predicate_main_xgo9__6_0 =
   { module_main, 6, 0 };

# 21 "wave.c" 3
const 
# 21 "wave.c"
     struct predicate predicate_main_xgo10__6_0 =
   { module_main, 7, 0 };

# 23 "wave.c" 3
const 
# 23 "wave.c"
     struct predicate predicate_main_xgo11__6_0 =
   { module_main, 8, 0 };

# 25 "wave.c" 3
const 
# 25 "wave.c"
     struct predicate predicate_main_xgo_3 =
   { module_main, 9, 3 };

# 27 "wave.c" 3
const 
# 27 "wave.c"
     struct predicate predicate_main_xplanes_7 =
   { module_main, 10, 7 };

# 29 "wave.c" 3
const 
# 29 "wave.c"
     struct predicate predicate_main_xplane_10 =
   { module_main, 11, 10 };

# 31 "wave.c" 3
const 
# 31 "wave.c"
     struct predicate predicate_main_xcol_13 =
   { module_main, 12, 13 };

# 33 "wave.c" 3
const 
# 33 "wave.c"
     struct predicate predicate_main_xpt_16 =
   { module_main, 13, 16 };

# 35 "wave.c" 3
const 
# 35 "wave.c"
     struct predicate predicate_main_xfilter_8 =
   { module_main, 14, 8 };

# 37 "wave.c" 3
const 
# 37 "wave.c"
     struct predicate predicate_main_xroute_14 =
   { module_main, 15, 14 };

# 39 "wave.c" 3
const 
# 39 "wave.c"
     struct predicate predicate_main_xbfilter_5 =
   { module_main, 16, 5 };

# 41 "wave.c" 3
const 
# 41 "wave.c"
     struct predicate predicate_main_xnode_7 =
   { module_main, 17, 7 };

# 43 "wave.c" 3
const 
# 43 "wave.c"
     struct predicate predicate_main_xgetin_3 =
   { module_main, 18, 3 };

# 45 "wave.c" 3
const 
# 45 "wave.c"
     struct predicate predicate_main_xcompute_6 =
   { module_main, 19, 6 };

# 47 "wave.c" 3
const 
# 47 "wave.c"
     struct predicate predicate_main_xchecklist_2 =
   { module_main, 20, 2 };

# 49 "wave.c" 3
const 
# 49 "wave.c"
     struct predicate predicate_main_xshow_7 =
   { module_main, 21, 7 };

# 51 "wave.c" 3
const 
# 51 "wave.c"
     struct predicate predicate_main_xputout_3 =
   { module_main, 22, 3 };

# 53 "wave.c" 3
const 
# 53 "wave.c"
     struct predicate predicate_main_xstart_2 =
   { module_main, 23, 2 };

# 55 "wave.c" 3
const 
# 55 "wave.c"
     struct predicate predicate_main_xacknowledged_3 =
   { module_main, 24, 3 };

# 57 "wave.c" 3
const 
# 57 "wave.c"
     struct predicate predicate_main_xacknowledged_4 =
   { module_main, 25, 4 };

# 59 "wave.c" 3
const 
# 59 "wave.c"
     struct predicate predicate_main_xgen_3 =
   { module_main, 26, 3 };

# 61 "wave.c" 3
const 
# 61 "wave.c"
     struct predicate predicate_main_xgen1_2 =
   { module_main, 27, 2 };

# 63 "wave.c" 3
const 
# 63 "wave.c"
     struct predicate predicate_main_xend_1 =
   { module_main, 28, 1 };

# 65 "wave.c" 3
const 
# 65 "wave.c"
     struct predicate predicate_main_xend1_1 =
   { module_main, 29, 1 };

# 67 "wave.c" 3
extern struct data_object_method_table 
# 67 "wave.c"
vector_g_data_method_table;

module module_main(glbl, qp, allocp, toppred)
  struct global_variables *glbl;
  struct goalrec *qp;
  register q *allocp;
  
# 73 "wave.c" 3
 const 
# 73 "wave.c"
       struct predicate *toppred;
{
  q a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15;

  q *reasonp;
 module_top:
  
# 79 "wave.c" 3
 switch (toppred->pred) 
# 79 "wave.c"
                  {
    
# 80 "wave.c" 3
   case (
# 80 "wave.c"
   0
# 80 "wave.c" 3
   ): goto 
# 80 "wave.c"
   main_0_top
# 80 "wave.c" 3
   ;
# 80 "wave.c"
                           ;
    
# 81 "wave.c" 3
   case (
# 81 "wave.c"
   1
# 81 "wave.c" 3
   ): goto 
# 81 "wave.c"
   go6__6_0_top
# 81 "wave.c" 3
   ;
# 81 "wave.c"
                             ;
    
# 82 "wave.c" 3
   case (
# 82 "wave.c"
   2
# 82 "wave.c" 3
   ): goto 
# 82 "wave.c"
   go6__2_0_top
# 82 "wave.c" 3
   ;
# 82 "wave.c"
                             ;
    
# 83 "wave.c" 3
   case (
# 83 "wave.c"
   3
# 83 "wave.c" 3
   ): goto 
# 83 "wave.c"
   go10__5_0_top
# 83 "wave.c" 3
   ;
# 83 "wave.c"
                              ;
    
# 84 "wave.c" 3
   case (
# 84 "wave.c"
   4
# 84 "wave.c" 3
   ): goto 
# 84 "wave.c"
   go11__5_0_top
# 84 "wave.c" 3
   ;
# 84 "wave.c"
                              ;
    
# 85 "wave.c" 3
   case (
# 85 "wave.c"
   5
# 85 "wave.c" 3
   ): goto 
# 85 "wave.c"
   go12__5_0_top
# 85 "wave.c" 3
   ;
# 85 "wave.c"
                              ;
    
# 86 "wave.c" 3
   case (
# 86 "wave.c"
   6
# 86 "wave.c" 3
   ): goto 
# 86 "wave.c"
   go9__6_0_top
# 86 "wave.c" 3
   ;
# 86 "wave.c"
                             ;
    
# 87 "wave.c" 3
   case (
# 87 "wave.c"
   7
# 87 "wave.c" 3
   ): goto 
# 87 "wave.c"
   go10__6_0_top
# 87 "wave.c" 3
   ;
# 87 "wave.c"
                              ;
    
# 88 "wave.c" 3
   case (
# 88 "wave.c"
   8
# 88 "wave.c" 3
   ): goto 
# 88 "wave.c"
   go11__6_0_top
# 88 "wave.c" 3
   ;
# 88 "wave.c"
                              ;
    
# 89 "wave.c" 3
   case (
# 89 "wave.c"
   9
# 89 "wave.c" 3
   ): goto 
# 89 "wave.c"
   go_3_top
# 89 "wave.c" 3
   ;
# 89 "wave.c"
                         ;
    
# 90 "wave.c" 3
   case (
# 90 "wave.c"
   10
# 90 "wave.c" 3
   ): goto 
# 90 "wave.c"
   planes_7_top
# 90 "wave.c" 3
   ;
# 90 "wave.c"
                              ;
    
# 91 "wave.c" 3
   case (
# 91 "wave.c"
   11
# 91 "wave.c" 3
   ): goto 
# 91 "wave.c"
   plane_10_top
# 91 "wave.c" 3
   ;
# 91 "wave.c"
                              ;
    
# 92 "wave.c" 3
   case (
# 92 "wave.c"
   12
# 92 "wave.c" 3
   ): goto 
# 92 "wave.c"
   col_13_top
# 92 "wave.c" 3
   ;
# 92 "wave.c"
                            ;
    
# 93 "wave.c" 3
   case (
# 93 "wave.c"
   13
# 93 "wave.c" 3
   ): goto 
# 93 "wave.c"
   pt_16_top
# 93 "wave.c" 3
   ;
# 93 "wave.c"
                           ;
    
# 94 "wave.c" 3
   case (
# 94 "wave.c"
   14
# 94 "wave.c" 3
   ): goto 
# 94 "wave.c"
   filter_8_top
# 94 "wave.c" 3
   ;
# 94 "wave.c"
                              ;
    
# 95 "wave.c" 3
   case (
# 95 "wave.c"
   15
# 95 "wave.c" 3
   ): goto 
# 95 "wave.c"
   route_14_top
# 95 "wave.c" 3
   ;
# 95 "wave.c"
                              ;
    
# 96 "wave.c" 3
   case (
# 96 "wave.c"
   16
# 96 "wave.c" 3
   ): goto 
# 96 "wave.c"
   bfilter_5_top
# 96 "wave.c" 3
   ;
# 96 "wave.c"
                               ;
    
# 97 "wave.c" 3
   case (
# 97 "wave.c"
   17
# 97 "wave.c" 3
   ): goto 
# 97 "wave.c"
   node_7_top
# 97 "wave.c" 3
   ;
# 97 "wave.c"
                            ;
    
# 98 "wave.c" 3
   case (
# 98 "wave.c"
   18
# 98 "wave.c" 3
   ): goto 
# 98 "wave.c"
   getin_3_top
# 98 "wave.c" 3
   ;
# 98 "wave.c"
                             ;
    
# 99 "wave.c" 3
   case (
# 99 "wave.c"
   19
# 99 "wave.c" 3
   ): goto 
# 99 "wave.c"
   compute_6_top
# 99 "wave.c" 3
   ;
# 99 "wave.c"
                               ;
    
# 100 "wave.c" 3
   case (
# 100 "wave.c"
   20
# 100 "wave.c" 3
   ): goto 
# 100 "wave.c"
   checklist_2_top
# 100 "wave.c" 3
   ;
# 100 "wave.c"
                                 ;
    
# 101 "wave.c" 3
   case (
# 101 "wave.c"
   21
# 101 "wave.c" 3
   ): goto 
# 101 "wave.c"
   show_7_top
# 101 "wave.c" 3
   ;
# 101 "wave.c"
                            ;
    
# 102 "wave.c" 3
   case (
# 102 "wave.c"
   22
# 102 "wave.c" 3
   ): goto 
# 102 "wave.c"
   putout_3_top
# 102 "wave.c" 3
   ;
# 102 "wave.c"
                              ;
    
# 103 "wave.c" 3
   case (
# 103 "wave.c"
   23
# 103 "wave.c" 3
   ): goto 
# 103 "wave.c"
   start_2_top
# 103 "wave.c" 3
   ;
# 103 "wave.c"
                             ;
    
# 104 "wave.c" 3
   case (
# 104 "wave.c"
   24
# 104 "wave.c" 3
   ): goto 
# 104 "wave.c"
   acknowledged_3_top
# 104 "wave.c" 3
   ;
# 104 "wave.c"
                                    ;
    
# 105 "wave.c" 3
   case (
# 105 "wave.c"
   25
# 105 "wave.c" 3
   ): goto 
# 105 "wave.c"
   acknowledged_4_top
# 105 "wave.c" 3
   ;
# 105 "wave.c"
                                    ;
    
# 106 "wave.c" 3
   case (
# 106 "wave.c"
   26
# 106 "wave.c" 3
   ): goto 
# 106 "wave.c"
   gen_3_top
# 106 "wave.c" 3
   ;
# 106 "wave.c"
                           ;
    
# 107 "wave.c" 3
   case (
# 107 "wave.c"
   27
# 107 "wave.c" 3
   ): goto 
# 107 "wave.c"
   gen1_2_top
# 107 "wave.c" 3
   ;
# 107 "wave.c"
                            ;
    
# 108 "wave.c" 3
   case (
# 108 "wave.c"
   28
# 108 "wave.c" 3
   ): goto 
# 108 "wave.c"
   end_1_top
# 108 "wave.c" 3
   ;
# 108 "wave.c"
                           ;
    
# 109 "wave.c" 3
   default: goto 
# 109 "wave.c"
   end1_1_top
# 109 "wave.c" 3
   ;
# 109 "wave.c"
                                 ;
  }

 main_0_top: {
  qp = qp->next;
 main_0_clear_reason:
  reasonp = 
# 115 "wave.c" 3
           (glbl->reasons0)
# 115 "wave.c"
                  ;
 main_0_0:
  
# 117 "wave.c" 3
 { if (allocp < (glbl->heaplimit0)) goto 
# 117 "wave.c"
 go6__2_0_0
# 117 "wave.c" 3
 ;}
# 117 "wave.c"
                    ;
  goto go6__2_0_ext_interrupt;
 main_0_ext_interrupt:
  reasonp = 0l;
 main_0_interrupt:
  goto interrupt_0;
 }

 go6__6_0_top: {
  q x0;
  qp = qp->next;
 go6__6_0_clear_reason:
  reasonp = 
# 129 "wave.c" 3
           (glbl->reasons0)
# 129 "wave.c"
                  ;
 go6__6_0_0:
  a0 = 
# 131 "wave.c" 3
      ((q)(((long)(
# 131 "wave.c"
      6L
# 131 "wave.c" 3
      ) << 4) + (0x2 +0x0)))
# 131 "wave.c"
                 ;
  a1 = 
# 132 "wave.c" 3
      ((q)(((long)(
# 132 "wave.c"
      6L
# 132 "wave.c" 3
      ) << 4) + (0x2 +0x0)))
# 132 "wave.c"
                 ;
  allocp[0] = x0 = 
# 133 "wave.c" 3
                  (q) ((unsigned long)(
# 133 "wave.c"
                  &allocp[0]
# 133 "wave.c" 3
                  ) + 0x0)
# 133 "wave.c"
                                     ;
  a2 = x0;
  allocp += 1;
  
# 136 "wave.c" 3
 { if (allocp < (glbl->heaplimit0)) goto 
# 136 "wave.c"
 go_3_0
# 136 "wave.c" 3
 ;}
# 136 "wave.c"
                ;
  goto go_3_ext_interrupt;
 go6__6_0_ext_interrupt:
  reasonp = 0l;
 go6__6_0_interrupt:
  goto interrupt_0;
 }

 go6__2_0_top: {
  q x0;
  qp = qp->next;
 go6__2_0_clear_reason:
  reasonp = 
# 148 "wave.c" 3
           (glbl->reasons0)
# 148 "wave.c"
                  ;
 go6__2_0_0:
  a0 = 
# 150 "wave.c" 3
      ((q)(((long)(
# 150 "wave.c"
      6L
# 150 "wave.c" 3
      ) << 4) + (0x2 +0x0)))
# 150 "wave.c"
                 ;
  a1 = 
# 151 "wave.c" 3
      ((q)(((long)(
# 151 "wave.c"
      2L
# 151 "wave.c" 3
      ) << 4) + (0x2 +0x0)))
# 151 "wave.c"
                 ;
  allocp[0] = x0 = 
# 152 "wave.c" 3
                  (q) ((unsigned long)(
# 152 "wave.c"
                  &allocp[0]
# 152 "wave.c" 3
                  ) + 0x0)
# 152 "wave.c"
                                     ;
  a2 = x0;
  allocp += 1;
  
# 155 "wave.c" 3
 { if (allocp < (glbl->heaplimit0)) goto 
# 155 "wave.c"
 go_3_0
# 155 "wave.c" 3
 ;}
# 155 "wave.c"
                ;
  goto go_3_ext_interrupt;
 go6__2_0_ext_interrupt:
  reasonp = 0l;
 go6__2_0_interrupt:
  toppred = &predicate_main_xgo6__2_0;
  goto interrupt_0;
 }

 go10__5_0_top: {
  q x0;
  qp = qp->next;
 go10__5_0_clear_reason:
  reasonp = 
# 168 "wave.c" 3
           (glbl->reasons0)
# 168 "wave.c"
                  ;
 go10__5_0_0:
  a0 = 
# 170 "wave.c" 3
      ((q)(((long)(
# 170 "wave.c"
      10L
# 170 "wave.c" 3
      ) << 4) + (0x2 +0x0)))
# 170 "wave.c"
                  ;
  a1 = 
# 171 "wave.c" 3
      ((q)(((long)(
# 171 "wave.c"
      5L
# 171 "wave.c" 3
      ) << 4) + (0x2 +0x0)))
# 171 "wave.c"
                 ;
  allocp[0] = x0 = 
# 172 "wave.c" 3
                  (q) ((unsigned long)(
# 172 "wave.c"
                  &allocp[0]
# 172 "wave.c" 3
                  ) + 0x0)
# 172 "wave.c"
                                     ;
  a2 = x0;
  allocp += 1;
  
# 175 "wave.c" 3
 { if (allocp < (glbl->heaplimit0)) goto 
# 175 "wave.c"
 go_3_0
# 175 "wave.c" 3
 ;}
# 175 "wave.c"
                ;
  goto go_3_ext_interrupt;
 go10__5_0_ext_interrupt:
  reasonp = 0l;
 go10__5_0_interrupt:
  goto interrupt_0;
 }

 go11__5_0_top: {
  q x0;
  qp = qp->next;
 go11__5_0_clear_reason:
  reasonp = 
# 187 "wave.c" 3
           (glbl->reasons0)
# 187 "wave.c"
                  ;
 go11__5_0_0:
  a0 = 
# 189 "wave.c" 3
      ((q)(((long)(
# 189 "wave.c"
      11L
# 189 "wave.c" 3
      ) << 4) + (0x2 +0x0)))
# 189 "wave.c"
                  ;
  a1 = 
# 190 "wave.c" 3
      ((q)(((long)(
# 190 "wave.c"
      5L
# 190 "wave.c" 3
      ) << 4) + (0x2 +0x0)))
# 190 "wave.c"
                 ;
  allocp[0] = x0 = 
# 191 "wave.c" 3
                  (q) ((unsigned long)(
# 191 "wave.c"
                  &allocp[0]
# 191 "wave.c" 3
                  ) + 0x0)
# 191 "wave.c"
                                     ;
  a2 = x0;
  allocp += 1;
  
# 194 "wave.c" 3
 { if (allocp < (glbl->heaplimit0)) goto 
# 194 "wave.c"
 go_3_0
# 194 "wave.c" 3
 ;}
# 194 "wave.c"
                ;
  goto go_3_ext_interrupt;
 go11__5_0_ext_interrupt:
  reasonp = 0l;
 go11__5_0_interrupt:
  goto interrupt_0;
 }

 go12__5_0_top: {
  q x0;
  qp = qp->next;
 go12__5_0_clear_reason:
  reasonp = 
# 206 "wave.c" 3
           (glbl->reasons0)
# 206 "wave.c"
                  ;
 go12__5_0_0:
  a0 = 
# 208 "wave.c" 3
      ((q)(((long)(
# 208 "wave.c"
      12L
# 208 "wave.c" 3
      ) << 4) + (0x2 +0x0)))
# 208 "wave.c"
                  ;
  a1 = 
# 209 "wave.c" 3
      ((q)(((long)(
# 209 "wave.c"
      5L
# 209 "wave.c" 3
      ) << 4) + (0x2 +0x0)))
# 209 "wave.c"
                 ;
  allocp[0] = x0 = 
# 210 "wave.c" 3
                  (q) ((unsigned long)(
# 210 "wave.c"
                  &allocp[0]
# 210 "wave.c" 3
                  ) + 0x0)
# 210 "wave.c"
                                     ;
  a2 = x0;
  allocp += 1;
  
# 213 "wave.c" 3
 { if (allocp < (glbl->heaplimit0)) goto 
# 213 "wave.c"
 go_3_0
# 213 "wave.c" 3
 ;}
# 213 "wave.c"
                ;
  goto go_3_ext_interrupt;
 go12__5_0_ext_interrupt:
  reasonp = 0l;
 go12__5_0_interrupt:
  goto interrupt_0;
 }

 go9__6_0_top: {
  q x0;
  qp = qp->next;
 go9__6_0_clear_reason:
  reasonp = 
# 225 "wave.c" 3
           (glbl->reasons0)
# 225 "wave.c"
                  ;
 go9__6_0_0:
  a0 = 
# 227 "wave.c" 3
      ((q)(((long)(
# 227 "wave.c"
      9L
# 227 "wave.c" 3
      ) << 4) + (0x2 +0x0)))
# 227 "wave.c"
                 ;
  a1 = 
# 228 "wave.c" 3
      ((q)(((long)(
# 228 "wave.c"
      6L
# 228 "wave.c" 3
      ) << 4) + (0x2 +0x0)))
# 228 "wave.c"
                 ;
  allocp[0] = x0 = 
# 229 "wave.c" 3
                  (q) ((unsigned long)(
# 229 "wave.c"
                  &allocp[0]
# 229 "wave.c" 3
                  ) + 0x0)
# 229 "wave.c"
                                     ;
  a2 = x0;
  allocp += 1;
  
# 232 "wave.c" 3
 { if (allocp < (glbl->heaplimit0)) goto 
# 232 "wave.c"
 go_3_0
# 232 "wave.c" 3
 ;}
# 232 "wave.c"
                ;
  goto go_3_ext_interrupt;
 go9__6_0_ext_interrupt:
  reasonp = 0l;
 go9__6_0_interrupt:
  goto interrupt_0;
 }

 go10__6_0_top: {
  q x0;
  qp = qp->next;
 go10__6_0_clear_reason:
  reasonp = 
# 244 "wave.c" 3
           (glbl->reasons0)
# 244 "wave.c"
                  ;
 go10__6_0_0:
  a0 = 
# 246 "wave.c" 3
      ((q)(((long)(
# 246 "wave.c"
      10L
# 246 "wave.c" 3
      ) << 4) + (0x2 +0x0)))
# 246 "wave.c"
                  ;
  a1 = 
# 247 "wave.c" 3
      ((q)(((long)(
# 247 "wave.c"
      6L
# 247 "wave.c" 3
      ) << 4) + (0x2 +0x0)))
# 247 "wave.c"
                 ;
  allocp[0] = x0 = 
# 248 "wave.c" 3
                  (q) ((unsigned long)(
# 248 "wave.c"
                  &allocp[0]
# 248 "wave.c" 3
                  ) + 0x0)
# 248 "wave.c"
                                     ;
  a2 = x0;
  allocp += 1;
  
# 251 "wave.c" 3
 { if (allocp < (glbl->heaplimit0)) goto 
# 251 "wave.c"
 go_3_0
# 251 "wave.c" 3
 ;}
# 251 "wave.c"
                ;
  goto go_3_ext_interrupt;
 go10__6_0_ext_interrupt:
  reasonp = 0l;
 go10__6_0_interrupt:
  goto interrupt_0;
 }

 go11__6_0_top: {
  q x0;
  qp = qp->next;
 go11__6_0_clear_reason:
  reasonp = 
# 263 "wave.c" 3
           (glbl->reasons0)
# 263 "wave.c"
                  ;
 go11__6_0_0:
  a0 = 
# 265 "wave.c" 3
      ((q)(((long)(
# 265 "wave.c"
      11L
# 265 "wave.c" 3
      ) << 4) + (0x2 +0x0)))
# 265 "wave.c"
                  ;
  a1 = 
# 266 "wave.c" 3
      ((q)(((long)(
# 266 "wave.c"
      6L
# 266 "wave.c" 3
      ) << 4) + (0x2 +0x0)))
# 266 "wave.c"
                 ;
  allocp[0] = x0 = 
# 267 "wave.c" 3
                  (q) ((unsigned long)(
# 267 "wave.c"
                  &allocp[0]
# 267 "wave.c" 3
                  ) + 0x0)
# 267 "wave.c"
                                     ;
  a2 = x0;
  allocp += 1;
  
# 270 "wave.c" 3
 { if (allocp < (glbl->heaplimit0)) goto 
# 270 "wave.c"
 go_3_0
# 270 "wave.c" 3
 ;}
# 270 "wave.c"
                ;
  goto go_3_ext_interrupt;
 go11__6_0_ext_interrupt:
  reasonp = 0l;
 go11__6_0_interrupt:
  goto interrupt_0;
 }

 go_3_top: {
  q x0, x1, x2, x3, x4, x5, x6, x7, x8;
  a0 = qp->args[0];
  a1 = qp->args[1];
  a2 = qp->args[2];
  qp = qp->next;
 go_3_clear_reason:
  reasonp = 
# 285 "wave.c" 3
           (glbl->reasons0)
# 285 "wave.c"
                  ;
 go_3_0:
 go_3_1:
  if (!
# 288 "wave.c" 3
      (((unsigned long)(
# 288 "wave.c"
      a0
# 288 "wave.c" 3
      ) & 0xFL) == (0x2 +0x0))
# 288 "wave.c"
               ) goto go_3_4;
  x0 = 
# 289 "wave.c" 3
      ((q)(((long)(
# 289 "wave.c"
      5L
# 289 "wave.c" 3
      ) << 4) + (0x2 +0x0)))
# 289 "wave.c"
                 ;
  
# 290 "wave.c" 3
 if ((long)(
# 290 "wave.c"
 a0
# 290 "wave.c" 3
 ) <= (long)(
# 290 "wave.c"
 x0
# 290 "wave.c" 3
 )) goto 
# 290 "wave.c"
 go_3_5
# 290 "wave.c" 3
 ;
# 290 "wave.c"
                           ;
 go_3_2:
  if (!
# 292 "wave.c" 3
      (((unsigned long)(
# 292 "wave.c"
      a1
# 292 "wave.c" 3
      ) & 0xFL) == (0x2 +0x0))
# 292 "wave.c"
               ) goto go_3_3;
  x1 = 
# 293 "wave.c" 3
      ((q)(((long)(
# 293 "wave.c"
      0L
# 293 "wave.c" 3
      ) << 4) + (0x2 +0x0)))
# 293 "wave.c"
                 ;
  
# 294 "wave.c" 3
 if ((long)(
# 294 "wave.c"
 a1
# 294 "wave.c" 3
 ) <= (long)(
# 294 "wave.c"
 x1
# 294 "wave.c" 3
 )) goto 
# 294 "wave.c"
 go_3_5
# 294 "wave.c" 3
 ;
# 294 "wave.c"
                           ;
  x2 = 
# 295 "wave.c" 3
      ((q)(((long)(
# 295 "wave.c"
      2L
# 295 "wave.c" 3
      ) << 4) + (0x2 +0x0)))
# 295 "wave.c"
                 ;
  
# 296 "wave.c" 3
 { 
# 296 "wave.c"
 x3 
# 296 "wave.c" 3
 = (q)((unsigned long)(
# 296 "wave.c"
 a0
# 296 "wave.c" 3
 ) - ((unsigned long)(
# 296 "wave.c"
 x2
# 296 "wave.c" 3
 ) - (0x2 +0x0))); }
# 296 "wave.c"
                            ;
  x4 = 
# 297 "wave.c" 3
      ((q)(((unsigned long)(
# 297 "wave.c"
      83L+
# 297 "wave.c" 3
      2) << 4) + (0x2 +0x4)))
# 297 "wave.c"
                       ;
  
# 298 "wave.c" 3
 { if (!(((unsigned long)(
# 298 "wave.c"
 a2
# 298 "wave.c" 3
 ) & 0x3) == 0x0) || (*((q *)((unsigned long)(
# 298 "wave.c"
 a2
# 298 "wave.c" 3
 ) - 0x0))) != (
# 298 "wave.c"
 a2
# 298 "wave.c" 3
 )) { allocp = do_unify_value(allocp, (
# 298 "wave.c"
 a2
# 298 "wave.c" 3
 ), (
# 298 "wave.c"
 x4
# 298 "wave.c" 3
 )); } else { (*((q *)((unsigned long)(
# 298 "wave.c"
 a2
# 298 "wave.c" 3
 ) - 0x0))) = (
# 298 "wave.c"
 x4
# 298 "wave.c" 3
 ); } }
# 298 "wave.c"
                    ;
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_main_xstart_2);
  allocp[2] = x5 = 
# 301 "wave.c" 3
                  (q) ((unsigned long)(
# 301 "wave.c"
                  &allocp[2]
# 301 "wave.c" 3
                  ) + 0x0)
# 301 "wave.c"
                                     ;
  allocp[3] = x6 = 
# 302 "wave.c" 3
                  (q) ((unsigned long)(
# 302 "wave.c"
                  &allocp[3]
# 302 "wave.c" 3
                  ) + 0x0)
# 302 "wave.c"
                                     ;
  allocp[4] = (q)(struct goalrec*)&allocp[0];
  allocp[5] = (q)(&predicate_main_xgen_3);
  allocp[6] = x3;
  allocp[7] = a0;
  allocp[8] = x7 = 
# 307 "wave.c" 3
                  (q) ((unsigned long)(
# 307 "wave.c"
                  &allocp[8]
# 307 "wave.c" 3
                  ) + 0x0)
# 307 "wave.c"
                                     ;
  x8 = a0;
  a2 = x7;
  a3 = a1;
  a4 = 
# 311 "wave.c" 3
      ((q)(((unsigned long)(
# 311 "wave.c"
      310L+
# 311 "wave.c" 3
      2) << 4) + (0x2 +0x4)))
# 311 "wave.c"
                        ;
  a5 = x5;
  a6 = x6;
  a1 = x8;
  a0 = 
# 315 "wave.c" 3
      ((q)(((long)(
# 315 "wave.c"
      0L
# 315 "wave.c" 3
      ) << 4) + (0x2 +0x0)))
# 315 "wave.c"
                 ;
  qp = (struct goalrec*)&allocp[4];
  allocp += 9;
  
# 318 "wave.c" 3
 { if (allocp < (glbl->heaplimit0)) goto 
# 318 "wave.c"
 planes_7_0
# 318 "wave.c" 3
 ;}
# 318 "wave.c"
                    ;
  goto planes_7_ext_interrupt;
 go_3_3:
  if (!
# 321 "wave.c" 3
      (((unsigned long)(
# 321 "wave.c"
      a1
# 321 "wave.c" 3
      ) & 0x3) == 0x0)
# 321 "wave.c"
               ) goto go_3_5;
  
# 322 "wave.c" 3
 { q temp0 = (*((q *)((unsigned long)(
# 322 "wave.c"
 a1
# 322 "wave.c" 3
 ) - 0x0))); if(!(((unsigned long)(temp0) & 0x3) == 0x0)) { (
# 322 "wave.c"
 a1
# 322 "wave.c" 3
 ) = temp0; goto 
# 322 "wave.c"
 go_3_2
# 322 "wave.c" 3
 ; } }
# 322 "wave.c"
                          ;
  *reasonp++ = a1;
  goto go_3_5;
 go_3_4:
  if (!
# 326 "wave.c" 3
      (((unsigned long)(
# 326 "wave.c"
      a0
# 326 "wave.c" 3
      ) & 0x3) == 0x0)
# 326 "wave.c"
               ) goto go_3_5;
  
# 327 "wave.c" 3
 { q temp0 = (*((q *)((unsigned long)(
# 327 "wave.c"
 a0
# 327 "wave.c" 3
 ) - 0x0))); if(!(((unsigned long)(temp0) & 0x3) == 0x0)) { (
# 327 "wave.c"
 a0
# 327 "wave.c" 3
 ) = temp0; goto 
# 327 "wave.c"
 go_3_1
# 327 "wave.c" 3
 ; } }
# 327 "wave.c"
                          ;
  *reasonp++ = a0;
  goto go_3_5;
 go_3_5:
  
# 331 "wave.c" 3
 { if (reasonp != (glbl->reasons0)) goto 
# 331 "wave.c"
 go_3_interrupt
# 331 "wave.c" 3
 ; }
# 331 "wave.c"
                          ;
  x0 = 
# 332 "wave.c" 3
      ((q)(((unsigned long)(
# 332 "wave.c"
      84L+
# 332 "wave.c" 3
      2) << 4) + (0x2 +0x4)))
# 332 "wave.c"
                      ;
  
# 333 "wave.c" 3
 { if (!(((unsigned long)(
# 333 "wave.c"
 a2
# 333 "wave.c" 3
 ) & 0x3) == 0x0) || (*((q *)((unsigned long)(
# 333 "wave.c"
 a2
# 333 "wave.c" 3
 ) - 0x0))) != (
# 333 "wave.c"
 a2
# 333 "wave.c" 3
 )) { allocp = do_unify_value(allocp, (
# 333 "wave.c"
 a2
# 333 "wave.c" 3
 ), (
# 333 "wave.c"
 x0
# 333 "wave.c" 3
 )); } else { (*((q *)((unsigned long)(
# 333 "wave.c"
 a2
# 333 "wave.c" 3
 ) - 0x0))) = (
# 333 "wave.c"
 x0
# 333 "wave.c" 3
 ); } }
# 333 "wave.c"
                    ;
  
# 334 "wave.c" 3
 { goto proceed_label; }
# 334 "wave.c"
          ;
 go_3_ext_interrupt:
  reasonp = 0l;
 go_3_interrupt:
  toppred = &predicate_main_xgo_3;
  goto interrupt_3;
 }

 planes_7_top: {
  q x0, x1, x2, x3, x4;
  a0 = qp->args[0];
  a1 = qp->args[1];
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  a6 = qp->args[6];
  qp = qp->next;
 planes_7_clear_reason:
  reasonp = 
# 353 "wave.c" 3
           (glbl->reasons0)
# 353 "wave.c"
                  ;
 planes_7_0:
 planes_7_1:
  switch (
# 356 "wave.c" 3
         ((unsigned long)(
# 356 "wave.c"
         a0
# 356 "wave.c" 3
         ) & 0x3)
# 356 "wave.c"
                   ) {
 case 
# 357 "wave.c" 3
     0x2
# 357 "wave.c"
           :
  if (!
# 358 "wave.c" 3
      (((unsigned long)(
# 358 "wave.c"
      a0
# 358 "wave.c" 3
      ) & 0xFL) == (0x2 +0x0))
# 358 "wave.c"
               ) goto planes_7_5;
 planes_7_2:
 planes_7_3:
  if (!
# 361 "wave.c" 3
      (((unsigned long)(
# 361 "wave.c"
      a1
# 361 "wave.c" 3
      ) & 0xFL) == (0x2 +0x0))
# 361 "wave.c"
               ) goto planes_7_4;
  
# 362 "wave.c" 3
 if ((long)(
# 362 "wave.c"
 a0
# 362 "wave.c" 3
 ) >= (long)(
# 362 "wave.c"
 a1
# 362 "wave.c" 3
 )) goto 
# 362 "wave.c"
 planes_7_5
# 362 "wave.c" 3
 ;
# 362 "wave.c"
                            ;
  x0 = 
# 363 "wave.c" 3
      ((q)(((long)(
# 363 "wave.c"
      1L
# 363 "wave.c" 3
      ) << 4) + (0x2 +0x0)))
# 363 "wave.c"
                 ;
  
# 364 "wave.c" 3
 { 
# 364 "wave.c"
 x1 
# 364 "wave.c" 3
 = (q)((unsigned long)(
# 364 "wave.c"
 a0
# 364 "wave.c" 3
 ) + (unsigned long)(
# 364 "wave.c"
 x0
# 364 "wave.c" 3
 ) - (0x2 +0x0)); }
# 364 "wave.c"
                            ;
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_main_xplanes_7);
  allocp[2] = x1;
  allocp[3] = a1;
  allocp[4] = x2 = 
# 369 "wave.c" 3
                  (q) ((unsigned long)(
# 369 "wave.c"
                  &allocp[4]
# 369 "wave.c" 3
                  ) + 0x0)
# 369 "wave.c"
                                     ;
  allocp[5] = a3;
  allocp[6] = x3 = 
# 371 "wave.c" 3
                  (q) ((unsigned long)(
# 371 "wave.c"
                  &allocp[6]
# 371 "wave.c" 3
                  ) + 0x0)
# 371 "wave.c"
                                     ;
  allocp[7] = a5;
  allocp[8] = a6;
  allocp[9] = (q)(struct goalrec*)&allocp[0];
  allocp[10] = (q)(&predicate_main_xplane_10);
  allocp[11] = 
# 376 "wave.c" 3
              ((q)(((long)(
# 376 "wave.c"
              1L
# 376 "wave.c" 3
              ) << 4) + (0x2 +0x0)))
# 376 "wave.c"
                         ;
  allocp[12] = a0;
  allocp[13] = a1;
  allocp[14] = x4 = 
# 379 "wave.c" 3
                   (q) ((unsigned long)(
# 379 "wave.c"
                   &allocp[14]
# 379 "wave.c" 3
                   ) + 0x0)
# 379 "wave.c"
                                       ;
  allocp[15] = a2;
  allocp[16] = x2;
  allocp[17] = a3;
  allocp[18] = a4;
  allocp[19] = x3;
  allocp[20] = a6;
  a0 = a1;
  a1 = x4;
  qp = (struct goalrec*)&allocp[9];
  allocp += 21;
  
# 390 "wave.c" 3
 { if (allocp < (glbl->heaplimit0)) goto 
# 390 "wave.c"
 gen1_2_0
# 390 "wave.c" 3
 ;}
# 390 "wave.c"
                  ;
  goto gen1_2_ext_interrupt;
 planes_7_4:
  if (!
# 393 "wave.c" 3
      (((unsigned long)(
# 393 "wave.c"
      a1
# 393 "wave.c" 3
      ) & 0x3) == 0x0)
# 393 "wave.c"
               ) goto planes_7_5;
  
# 394 "wave.c" 3
 { q temp0 = (*((q *)((unsigned long)(
# 394 "wave.c"
 a1
# 394 "wave.c" 3
 ) - 0x0))); if(!(((unsigned long)(temp0) & 0x3) == 0x0)) { (
# 394 "wave.c"
 a1
# 394 "wave.c" 3
 ) = temp0; goto 
# 394 "wave.c"
 planes_7_3
# 394 "wave.c" 3
 ; } }
# 394 "wave.c"
                              ;
  *reasonp++ = a1;
  goto planes_7_5;
 case 
# 397 "wave.c" 3
     0x0
# 397 "wave.c"
           :
  
# 398 "wave.c" 3
 { q temp0 = (*((q *)((unsigned long)(
# 398 "wave.c"
 a0
# 398 "wave.c" 3
 ) - 0x0))); if(!(((unsigned long)(temp0) & 0x3) == 0x0)) { (
# 398 "wave.c"
 a0
# 398 "wave.c" 3
 ) = temp0; goto 
# 398 "wave.c"
 planes_7_1
# 398 "wave.c" 3
 ; } }
# 398 "wave.c"
                              ;
  *reasonp++ = a0;
  goto planes_7_interrupt;
  };
  goto planes_7_5;
 planes_7_5:
 planes_7_6:
  if (
# 405 "wave.c" 3
     (((unsigned long)(
# 405 "wave.c"
     a0
# 405 "wave.c" 3
     ) & 0x3) == 0x0)
# 405 "wave.c"
              ) goto planes_7_7;
  
# 406 "wave.c" 3
 if ((
# 406 "wave.c"
 a0
# 406 "wave.c" 3
 )!=(
# 406 "wave.c"
 a1
# 406 "wave.c" 3
 ) || (((unsigned long)(
# 406 "wave.c"
 a0
# 406 "wave.c" 3
 ) & 0x3) == 0x0)) { q retval; switch((long)(retval = eq_terms_body((
# 406 "wave.c"
 a0
# 406 "wave.c" 3
 ), (
# 406 "wave.c"
 a1
# 406 "wave.c" 3
 )))){ case 0: break; default: *reasonp++ = retval; case 1: goto 
# 406 "wave.c"
 planes_7_interrupt
# 406 "wave.c" 3
 ; } }
# 406 "wave.c"
                                         ;
  
# 407 "wave.c" 3
 { allocp = do_unify(allocp, (
# 407 "wave.c"
 a5
# 407 "wave.c" 3
 ), (
# 407 "wave.c"
 a4
# 407 "wave.c" 3
 )); }
# 407 "wave.c"
              ;
  a0 = a2;
  
# 409 "wave.c" 3
 { if (allocp < (glbl->heaplimit0)) goto 
# 409 "wave.c"
 end_1_clear_reason
# 409 "wave.c" 3
 ;}
# 409 "wave.c"
                            ;
  goto end_1_ext_interrupt;
 planes_7_7:
  
# 412 "wave.c" 3
 { q temp0 = (*((q *)((unsigned long)(
# 412 "wave.c"
 a0
# 412 "wave.c" 3
 ) - 0x0))); if(!(((unsigned long)(temp0) & 0x3) == 0x0)) { (
# 412 "wave.c"
 a0
# 412 "wave.c" 3
 ) = temp0; goto 
# 412 "wave.c"
 planes_7_6
# 412 "wave.c" 3
 ; } }
# 412 "wave.c"
                              ;
  *reasonp++ = a0;
  goto planes_7_interrupt;
 planes_7_ext_interrupt:
  reasonp = 0l;
 planes_7_interrupt:
  toppred = &predicate_main_xplanes_7;
  goto interrupt_7;
 }

 plane_10_top: {
  q x0, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13;
  a0 = qp->args[0];
  a1 = qp->args[1];
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  a6 = qp->args[6];
  a7 = qp->args[7];
  a8 = qp->args[8];
  a9 = qp->args[9];
  qp = qp->next;
 plane_10_clear_reason:
  reasonp = 
# 436 "wave.c" 3
           (glbl->reasons0)
# 436 "wave.c"
                  ;
 plane_10_0:
 plane_10_1:
  switch (
# 439 "wave.c" 3
         ((unsigned long)(
# 439 "wave.c"
         a4
# 439 "wave.c" 3
         ) & 0x3)
# 439 "wave.c"
                   ) {
 case 
# 440 "wave.c" 3
     0x1
# 440 "wave.c"
         :
  x0 = 
# 441 "wave.c" 3
      ((q)(((long)(
# 441 "wave.c"
      1L
# 441 "wave.c" 3
      ) << 4) + (0x2 +0x0)))
# 441 "wave.c"
                 ;
  
# 442 "wave.c" 3
 { if (!(((unsigned long)(
# 442 "wave.c"
 a0
# 442 "wave.c" 3
 ) & 0xFL) == (0x2 +0x0))) { 
# 442 "wave.c"
 x1 
# 442 "wave.c" 3
 = bblt_add_3(
# 442 "wave.c"
 a0
# 442 "wave.c" 3
 , 
# 442 "wave.c"
 x0
# 442 "wave.c" 3
 ); } else { { 
# 442 "wave.c"
 x1 
# 442 "wave.c" 3
 = (q)((unsigned long)(
# 442 "wave.c"
 a0
# 442 "wave.c" 3
 ) + (unsigned long)(
# 442 "wave.c"
 x0
# 442 "wave.c" 3
 ) - (0x2 +0x0)); }; } }
# 442 "wave.c"
                              ;
  allocp[0] = x3 = 
# 443 "wave.c" 3
                  (q) ((unsigned long)(
# 443 "wave.c"
                  &allocp[0]
# 443 "wave.c" 3
                  ) + 0x0)
# 443 "wave.c"
                                     ;
  allocp[1] = x4 = 
# 444 "wave.c" 3
                  (q) ((unsigned long)(
# 444 "wave.c"
                  &allocp[1]
# 444 "wave.c" 3
                  ) + 0x0)
# 444 "wave.c"
                                     ;
  x2 = 
# 445 "wave.c" 3
      ((q)((unsigned long)(
# 445 "wave.c"
      &allocp[0]
# 445 "wave.c" 3
      ) + 0x1))
# 445 "wave.c"
                          ;
  allocp += 2;
  
# 447 "wave.c" 3
 { if (!(((unsigned long)(
# 447 "wave.c"
 a5
# 447 "wave.c" 3
 ) & 0x3) == 0x0) || (*((q *)((unsigned long)(
# 447 "wave.c"
 a5
# 447 "wave.c" 3
 ) - 0x0))) != (
# 447 "wave.c"
 a5
# 447 "wave.c" 3
 )) { allocp = do_unify_value(allocp, (
# 447 "wave.c"
 a5
# 447 "wave.c" 3
 ), (
# 447 "wave.c"
 x2
# 447 "wave.c" 3
 )); } else { (*((q *)((unsigned long)(
# 447 "wave.c"
 a5
# 447 "wave.c" 3
 ) - 0x0))) = (
# 447 "wave.c"
 x2
# 447 "wave.c" 3
 ); } }
# 447 "wave.c"
                    ;
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_main_xplane_10);
  allocp[2] = x1;
  allocp[3] = a1;
  allocp[4] = a2;
  allocp[5] = x5 = 
# 453 "wave.c" 3
                  (q) ((unsigned long)(
# 453 "wave.c"
                  &allocp[5]
# 453 "wave.c" 3
                  ) + 0x0)
# 453 "wave.c"
                                     ;
  x6 = 
# 454 "wave.c" 3
      (((struct cons *)((unsigned long)(
# 454 "wave.c"
      a4
# 454 "wave.c" 3
      ) - 0x1))->cdr)
# 454 "wave.c"
                ;
  allocp[6] = x6;
  allocp[7] = x3;
  allocp[8] = a6;
  allocp[9] = x7 = 
# 458 "wave.c" 3
                  (q) ((unsigned long)(
# 458 "wave.c"
                  &allocp[9]
# 458 "wave.c" 3
                  ) + 0x0)
# 458 "wave.c"
                                     ;
  allocp[10] = a8;
  allocp[11] = a9;
  allocp[12] = x8 = 
# 461 "wave.c" 3
                   (q) ((unsigned long)(
# 461 "wave.c"
                   &allocp[12]
# 461 "wave.c" 3
                   ) + 0x0)
# 461 "wave.c"
                                       ;
  allocp[13] = 
# 462 "wave.c" 3
              ((q)(((unsigned long)(0) << 4) + (0x2 +0x4)))
# 462 "wave.c"
                     ;
  allocp[14] = x8;
  x9 = create_vector(&allocp[13], 2, allocp);
  x10 = a1;
  x11 = a2;
  a5 = a3;
  x12 = 
# 468 "wave.c" 3
       (((struct cons *)((unsigned long)(
# 468 "wave.c"
       a4
# 468 "wave.c" 3
       ) - 0x1))->car)
# 468 "wave.c"
                 ;
  a8 = x4;
  x13 = a6;
  a10 = a7;
  a11 = x7;
  a12 = a9;
  a9 = x13;
  a7 = x12;
  a6 = x5;
  a4 = x9;
  a3 = x11;
  a2 = x10;
  a1 = 
# 480 "wave.c" 3
      ((q)(((long)(
# 480 "wave.c"
      0L
# 480 "wave.c" 3
      ) << 4) + (0x2 +0x0)))
# 480 "wave.c"
                 ;
  qp = (struct goalrec*)&allocp[0];
  allocp += 15;
  
# 483 "wave.c" 3
 { if (allocp < (glbl->heaplimit0)) goto 
# 483 "wave.c"
 col_13_0
# 483 "wave.c" 3
 ;}
# 483 "wave.c"
                  ;
  goto col_13_ext_interrupt;
 case 
# 485 "wave.c" 3
     0x2
# 485 "wave.c"
           :
  if (a4 != 
# 486 "wave.c" 3
           ((q)(((unsigned long)(0) << 4) + (0x2 +0x4)))
# 486 "wave.c"
                  ) goto plane_10_interrupt;
  
# 487 "wave.c" 3
 { allocp = do_unify(allocp, (
# 487 "wave.c"
 a8
# 487 "wave.c" 3
 ), (
# 487 "wave.c"
 a7
# 487 "wave.c" 3
 )); }
# 487 "wave.c"
              ;
  x0 = 
# 488 "wave.c" 3
      ((q)(((unsigned long)(0) << 4) + (0x2 +0x4)))
# 488 "wave.c"
             ;
  
# 489 "wave.c" 3
 { if (!(((unsigned long)(
# 489 "wave.c"
 a5
# 489 "wave.c" 3
 ) & 0x3) == 0x0) || (*((q *)((unsigned long)(
# 489 "wave.c"
 a5
# 489 "wave.c" 3
 ) - 0x0))) != (
# 489 "wave.c"
 a5
# 489 "wave.c" 3
 )) { allocp = do_unify_value(allocp, (
# 489 "wave.c"
 a5
# 489 "wave.c" 3
 ), (
# 489 "wave.c"
 x0
# 489 "wave.c" 3
 )); } else { (*((q *)((unsigned long)(
# 489 "wave.c"
 a5
# 489 "wave.c" 3
 ) - 0x0))) = (
# 489 "wave.c"
 x0
# 489 "wave.c" 3
 ); } }
# 489 "wave.c"
                    ;
  a0 = a3;
  
# 491 "wave.c" 3
 { if (allocp < (glbl->heaplimit0)) goto 
# 491 "wave.c"
 end1_1_0
# 491 "wave.c" 3
 ;}
# 491 "wave.c"
                  ;
  goto end1_1_ext_interrupt;
 case 
# 493 "wave.c" 3
     0x0
# 493 "wave.c"
           :
  
# 494 "wave.c" 3
 { q temp0 = (*((q *)((unsigned long)(
# 494 "wave.c"
 a4
# 494 "wave.c" 3
 ) - 0x0))); if(!(((unsigned long)(temp0) & 0x3) == 0x0)) { (
# 494 "wave.c"
 a4
# 494 "wave.c" 3
 ) = temp0; goto 
# 494 "wave.c"
 plane_10_1
# 494 "wave.c" 3
 ; } }
# 494 "wave.c"
                              ;
  *reasonp++ = a4;
  goto plane_10_interrupt;
  };
  goto plane_10_interrupt;
 plane_10_ext_interrupt:
  reasonp = 0l;
 plane_10_interrupt:
  goto interrupt_10;
 }

 col_13_top: {
  q x0, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13, x14, x15, x16, x17, x18, x19, x20, x21, x22, x23, x24, x25, x26, x27;
  a0 = qp->args[0];
  a1 = qp->args[1];
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  a6 = qp->args[6];
  a7 = qp->args[7];
  a8 = qp->args[8];
  a9 = qp->args[9];
  a10 = qp->args[10];
  a11 = qp->args[11];
  a12 = qp->args[12];
  qp = qp->next;
 col_13_clear_reason:
  reasonp = 
# 522 "wave.c" 3
           (glbl->reasons0)
# 522 "wave.c"
                  ;
 col_13_0:
 col_13_1:
  switch (
# 525 "wave.c" 3
         ((unsigned long)(
# 525 "wave.c"
         a5
# 525 "wave.c" 3
         ) & 0x3)
# 525 "wave.c"
                   ) {
 case 
# 526 "wave.c" 3
     0x1
# 526 "wave.c"
         :
 col_13_2:
  switch (
# 528 "wave.c" 3
         ((unsigned long)(
# 528 "wave.c"
         a7
# 528 "wave.c" 3
         ) & 0x3)
# 528 "wave.c"
                   ) {
 case 
# 529 "wave.c" 3
     0x1
# 529 "wave.c"
         :
  x0 = 
# 530 "wave.c" 3
      ((q)(((long)(
# 530 "wave.c"
      1L
# 530 "wave.c" 3
      ) << 4) + (0x2 +0x0)))
# 530 "wave.c"
                 ;
  
# 531 "wave.c" 3
 { if (!(((unsigned long)(
# 531 "wave.c"
 a1
# 531 "wave.c" 3
 ) & 0xFL) == (0x2 +0x0))) { 
# 531 "wave.c"
 x1 
# 531 "wave.c" 3
 = bblt_add_3(
# 531 "wave.c"
 a1
# 531 "wave.c" 3
 , 
# 531 "wave.c"
 x0
# 531 "wave.c" 3
 ); } else { { 
# 531 "wave.c"
 x1 
# 531 "wave.c" 3
 = (q)((unsigned long)(
# 531 "wave.c"
 a1
# 531 "wave.c" 3
 ) + (unsigned long)(
# 531 "wave.c"
 x0
# 531 "wave.c" 3
 ) - (0x2 +0x0)); }; } }
# 531 "wave.c"
                              ;
  x2 = 
# 532 "wave.c" 3
      ((q)(((long)(
# 532 "wave.c"
      1L
# 532 "wave.c" 3
      ) << 4) + (0x2 +0x0)))
# 532 "wave.c"
                 ;
  
# 533 "wave.c" 3
 { if (!(((unsigned long)(
# 533 "wave.c"
 a3
# 533 "wave.c" 3
 ) & 0xFL) == (0x2 +0x0))) { 
# 533 "wave.c"
 x3 
# 533 "wave.c" 3
 = bblt_sub_3(
# 533 "wave.c"
 a3
# 533 "wave.c" 3
 , 
# 533 "wave.c"
 x2
# 533 "wave.c" 3
 ); } else { { 
# 533 "wave.c"
 x3 
# 533 "wave.c" 3
 = (q)((unsigned long)(
# 533 "wave.c"
 a3
# 533 "wave.c" 3
 ) - ((unsigned long)(
# 533 "wave.c"
 x2
# 533 "wave.c" 3
 ) - (0x2 +0x0))); }; } }
# 533 "wave.c"
                              ;
  x4 = 
# 534 "wave.c" 3
      ((q)(((long)(
# 534 "wave.c"
      2L
# 534 "wave.c" 3
      ) << 4) + (0x2 +0x0)))
# 534 "wave.c"
                 ;
  
# 535 "wave.c" 3
 { if (!(((unsigned long)(
# 535 "wave.c"
 a3
# 535 "wave.c" 3
 ) & 0xFL) == (0x2 +0x0))) { 
# 535 "wave.c"
 x5 
# 535 "wave.c" 3
 = bblt_sub_3(
# 535 "wave.c"
 a3
# 535 "wave.c" 3
 , 
# 535 "wave.c"
 x4
# 535 "wave.c" 3
 ); } else { { 
# 535 "wave.c"
 x5 
# 535 "wave.c" 3
 = (q)((unsigned long)(
# 535 "wave.c"
 a3
# 535 "wave.c" 3
 ) - ((unsigned long)(
# 535 "wave.c"
 x4
# 535 "wave.c" 3
 ) - (0x2 +0x0))); }; } }
# 535 "wave.c"
                              ;
  allocp[0] = x6 = 
# 536 "wave.c" 3
                  (q) ((unsigned long)(
# 536 "wave.c"
                  &allocp[0]
# 536 "wave.c" 3
                  ) + 0x0)
# 536 "wave.c"
                                     ;
  allocp[1] = x7 = 
# 537 "wave.c" 3
                  (q) ((unsigned long)(
# 537 "wave.c"
                  &allocp[1]
# 537 "wave.c" 3
                  ) + 0x0)
# 537 "wave.c"
                                     ;
  allocp[2] = x6;
  allocp[3] = x7;
  x8 = create_vector(&allocp[2], 2, allocp);
  allocp[4] = x10 = 
# 541 "wave.c" 3
                   (q) ((unsigned long)(
# 541 "wave.c"
                   &allocp[4]
# 541 "wave.c" 3
                   ) + 0x0)
# 541 "wave.c"
                                      ;
  allocp[5] = x8;
  x9 = 
# 543 "wave.c" 3
      ((q)((unsigned long)(
# 543 "wave.c"
      &allocp[4]
# 543 "wave.c" 3
      ) + 0x1))
# 543 "wave.c"
                          ;
  allocp += 6;
  
# 545 "wave.c" 3
 { if (!(((unsigned long)(
# 545 "wave.c"
 a6
# 545 "wave.c" 3
 ) & 0x3) == 0x0) || (*((q *)((unsigned long)(
# 545 "wave.c"
 a6
# 545 "wave.c" 3
 ) - 0x0))) != (
# 545 "wave.c"
 a6
# 545 "wave.c" 3
 )) { allocp = do_unify_value(allocp, (
# 545 "wave.c"
 a6
# 545 "wave.c" 3
 ), (
# 545 "wave.c"
 x9
# 545 "wave.c" 3
 )); } else { (*((q *)((unsigned long)(
# 545 "wave.c"
 a6
# 545 "wave.c" 3
 ) - 0x0))) = (
# 545 "wave.c"
 x9
# 545 "wave.c" 3
 ); } }
# 545 "wave.c"
                    ;
  allocp[0] = x11 = 
# 546 "wave.c" 3
                   (q) ((unsigned long)(
# 546 "wave.c"
                   &allocp[0]
# 546 "wave.c" 3
                   ) + 0x0)
# 546 "wave.c"
                                      ;
  allocp[1] = x12 = 
# 547 "wave.c" 3
                   (q) ((unsigned long)(
# 547 "wave.c"
                   &allocp[1]
# 547 "wave.c" 3
                   ) + 0x0)
# 547 "wave.c"
                                      ;
  allocp[2] = x11;
  allocp[3] = x12;
  x13 = create_vector(&allocp[2], 2, allocp);
  allocp[4] = x15 = 
# 551 "wave.c" 3
                   (q) ((unsigned long)(
# 551 "wave.c"
                   &allocp[4]
# 551 "wave.c" 3
                   ) + 0x0)
# 551 "wave.c"
                                      ;
  allocp[5] = x13;
  x14 = 
# 553 "wave.c" 3
       ((q)((unsigned long)(
# 553 "wave.c"
       &allocp[4]
# 553 "wave.c" 3
       ) + 0x1))
# 553 "wave.c"
                           ;
  allocp += 6;
  
# 555 "wave.c" 3
 { if (!(((unsigned long)(
# 555 "wave.c"
 a8
# 555 "wave.c" 3
 ) & 0x3) == 0x0) || (*((q *)((unsigned long)(
# 555 "wave.c"
 a8
# 555 "wave.c" 3
 ) - 0x0))) != (
# 555 "wave.c"
 a8
# 555 "wave.c" 3
 )) { allocp = do_unify_value(allocp, (
# 555 "wave.c"
 a8
# 555 "wave.c" 3
 ), (
# 555 "wave.c"
 x14
# 555 "wave.c" 3
 )); } else { (*((q *)((unsigned long)(
# 555 "wave.c"
 a8
# 555 "wave.c" 3
 ) - 0x0))) = (
# 555 "wave.c"
 x14
# 555 "wave.c" 3
 ); } }
# 555 "wave.c"
                     ;
  allocp[0] = x16 = 
# 556 "wave.c" 3
                   (q) ((unsigned long)(
# 556 "wave.c"
                   &allocp[0]
# 556 "wave.c" 3
                   ) + 0x0)
# 556 "wave.c"
                                      ;
  allocp[1] = x17 = 
# 557 "wave.c" 3
                   (q) ((unsigned long)(
# 557 "wave.c"
                   &allocp[1]
# 557 "wave.c" 3
                   ) + 0x0)
# 557 "wave.c"
                                      ;
  allocp[2] = x16;
  allocp[3] = x17;
  x18 = create_vector(&allocp[2], 2, allocp);
  allocp[4] = (q)qp;
  allocp[5] = (q)(&predicate_main_xcol_13);
  allocp[6] = a0;
  allocp[7] = x1;
  allocp[8] = a2;
  allocp[9] = a3;
  allocp[10] = x18;
  x19 = 
# 568 "wave.c" 3
       (((struct cons *)((unsigned long)(
# 568 "wave.c"
       a5
# 568 "wave.c" 3
       ) - 0x1))->cdr)
# 568 "wave.c"
                 ;
  allocp[11] = x19;
  allocp[12] = x10;
  x20 = 
# 571 "wave.c" 3
       (((struct cons *)((unsigned long)(
# 571 "wave.c"
       a7
# 571 "wave.c" 3
       ) - 0x1))->cdr)
# 571 "wave.c"
                 ;
  allocp[13] = x20;
  allocp[14] = x15;
  allocp[15] = a9;
  allocp[16] = x21 = 
# 575 "wave.c" 3
                    (q) ((unsigned long)(
# 575 "wave.c"
                    &allocp[16]
# 575 "wave.c" 3
                    ) + 0x0)
# 575 "wave.c"
                                        ;
  allocp[17] = a11;
  allocp[18] = a12;
  allocp[19] = x17;
  allocp[20] = x16;
  x22 = create_vector(&allocp[19], 2, allocp);
  allocp[21] = x7;
  allocp[22] = x6;
  x23 = create_vector(&allocp[21], 2, allocp);
  allocp[23] = x12;
  allocp[24] = x11;
  x24 = create_vector(&allocp[23], 2, allocp);
  a3 = x5;
  a6 = x23;
  x25 = a4;
  a8 = 
# 590 "wave.c" 3
      (((struct cons *)((unsigned long)(
# 590 "wave.c"
      a5
# 590 "wave.c" 3
      ) - 0x1))->car)
# 590 "wave.c"
                ;
  x26 = 
# 591 "wave.c" 3
       (((struct cons *)((unsigned long)(
# 591 "wave.c"
       a7
# 591 "wave.c" 3
       ) - 0x1))->car)
# 591 "wave.c"
                 ;
  a11 = 
# 592 "wave.c" 3
       ((q)(((long)(
# 592 "wave.c"
       1L
# 592 "wave.c" 3
       ) << 4) + (0x2 +0x0)))
# 592 "wave.c"
                  ;
  x27 = a9;
  a13 = a10;
  a14 = x21;
  a15 = a12;
  a12 = x27;
  a10 = x24;
  a9 = x26;
  a7 = x25;
  a5 = x22;
  a4 = x3;
  qp = (struct goalrec*)&allocp[4];
  allocp += 25;
  
# 605 "wave.c" 3
 { if (allocp < (glbl->heaplimit0)) goto 
# 605 "wave.c"
 pt_16_0
# 605 "wave.c" 3
 ;}
# 605 "wave.c"
                 ;
  goto pt_16_ext_interrupt;
 case 
# 607 "wave.c" 3
     0x0
# 607 "wave.c"
           :
  
# 608 "wave.c" 3
 { q temp0 = (*((q *)((unsigned long)(
# 608 "wave.c"
 a7
# 608 "wave.c" 3
 ) - 0x0))); if(!(((unsigned long)(temp0) & 0x3) == 0x0)) { (
# 608 "wave.c"
 a7
# 608 "wave.c" 3
 ) = temp0; goto 
# 608 "wave.c"
 col_13_2
# 608 "wave.c" 3
 ; } }
# 608 "wave.c"
                            ;
  *reasonp++ = a7;
  goto col_13_3;
  };
  goto col_13_3;
 case 
# 613 "wave.c" 3
     0x0
# 613 "wave.c"
           :
  
# 614 "wave.c" 3
 { q temp0 = (*((q *)((unsigned long)(
# 614 "wave.c"
 a5
# 614 "wave.c" 3
 ) - 0x0))); if(!(((unsigned long)(temp0) & 0x3) == 0x0)) { (
# 614 "wave.c"
 a5
# 614 "wave.c" 3
 ) = temp0; goto 
# 614 "wave.c"
 col_13_1
# 614 "wave.c" 3
 ; } }
# 614 "wave.c"
                            ;
  *reasonp++ = a5;
  goto col_13_3;
  };
  goto col_13_3;
 col_13_3:
 col_13_4:
  switch (
# 621 "wave.c" 3
         ((unsigned long)(
# 621 "wave.c"
         a4
# 621 "wave.c" 3
         ) & 0x3)
# 621 "wave.c"
                   ) {
 case 
# 622 "wave.c" 3
     0x3
# 622 "wave.c"
            :
 col_13_5:
  if (!
# 624 "wave.c" 3
      ((((unsigned long)((((struct functor *)((unsigned long)(
# 624 "wave.c"
      a4
# 624 "wave.c" 3
      ) - 0x3))->functor)) & 0x3) == 0x0))
# 624 "wave.c"
                ) goto col_13_interrupt;
  if (!
# 625 "wave.c" 3
      ((struct data_object_method_table *)((((struct functor *)((unsigned long)(
# 625 "wave.c"
      a4
# 625 "wave.c" 3
      ) - 0x3))->functor)) == &
# 625 "wave.c"
      vector_g_data_method_table
# 625 "wave.c" 3
      )
# 625 "wave.c"
                        ) goto col_13_interrupt;
  
# 626 "wave.c" 3
 { extern q size_of_vector(); 
# 626 "wave.c"
 x0 
# 626 "wave.c" 3
 = size_of_vector(
# 626 "wave.c"
 a4
# 626 "wave.c" 3
 ); }
# 626 "wave.c"
                                            ;
 col_13_6:
  if (x0 != 
# 628 "wave.c" 3
           ((q)(((long)(
# 628 "wave.c"
           2L
# 628 "wave.c" 3
           ) << 4) + (0x2 +0x0)))
# 628 "wave.c"
                      ) goto col_13_interrupt;
  x2 = 
# 629 "wave.c" 3
      ((q)(((long)(
# 629 "wave.c"
      0L
# 629 "wave.c" 3
      ) << 4) + (0x2 +0x0)))
# 629 "wave.c"
                 ;
  
# 630 "wave.c" 3
 { extern q element_of_vector(); 
# 630 "wave.c"
 x1 
# 630 "wave.c" 3
 = element_of_vector(
# 630 "wave.c"
 a4
# 630 "wave.c" 3
 ,
# 630 "wave.c"
 x2
# 630 "wave.c" 3
 ); if ((long)
# 630 "wave.c"
 x1 
# 630 "wave.c" 3
 == 0) goto 
# 630 "wave.c"
 col_13_interrupt
# 630 "wave.c" 3
 ; }
# 630 "wave.c"
                                                  ;
  x4 = 
# 631 "wave.c" 3
      ((q)(((long)(
# 631 "wave.c"
      1L
# 631 "wave.c" 3
      ) << 4) + (0x2 +0x0)))
# 631 "wave.c"
                 ;
  
# 632 "wave.c" 3
 { extern q element_of_vector(); 
# 632 "wave.c"
 x3 
# 632 "wave.c" 3
 = element_of_vector(
# 632 "wave.c"
 a4
# 632 "wave.c" 3
 ,
# 632 "wave.c"
 x4
# 632 "wave.c" 3
 ); if ((long)
# 632 "wave.c"
 x3 
# 632 "wave.c" 3
 == 0) goto 
# 632 "wave.c"
 col_13_interrupt
# 632 "wave.c" 3
 ; }
# 632 "wave.c"
                                                  ;
 col_13_7:
  switch (
# 634 "wave.c" 3
         ((unsigned long)(
# 634 "wave.c"
         a5
# 634 "wave.c" 3
         ) & 0x3)
# 634 "wave.c"
                   ) {
 case 
# 635 "wave.c" 3
     0x2
# 635 "wave.c"
           :
  if (a5 != 
# 636 "wave.c" 3
           ((q)(((unsigned long)(0) << 4) + (0x2 +0x4)))
# 636 "wave.c"
                  ) goto col_13_interrupt;
 col_13_8:
  switch (
# 638 "wave.c" 3
         ((unsigned long)(
# 638 "wave.c"
         a7
# 638 "wave.c" 3
         ) & 0x3)
# 638 "wave.c"
                   ) {
 case 
# 639 "wave.c" 3
     0x2
# 639 "wave.c"
           :
  if (a7 != 
# 640 "wave.c" 3
           ((q)(((unsigned long)(0) << 4) + (0x2 +0x4)))
# 640 "wave.c"
                  ) goto col_13_interrupt;
  
# 641 "wave.c" 3
 { allocp = do_unify(allocp, (
# 641 "wave.c"
 a11
# 641 "wave.c" 3
 ), (
# 641 "wave.c"
 a10
# 641 "wave.c" 3
 )); }
# 641 "wave.c"
                ;
  x5 = 
# 642 "wave.c" 3
      ((q)(((unsigned long)(0) << 4) + (0x2 +0x4)))
# 642 "wave.c"
             ;
  
# 643 "wave.c" 3
 { if (!(((unsigned long)(
# 643 "wave.c"
 x3
# 643 "wave.c" 3
 ) & 0x3) == 0x0) || (*((q *)((unsigned long)(
# 643 "wave.c"
 x3
# 643 "wave.c" 3
 ) - 0x0))) != (
# 643 "wave.c"
 x3
# 643 "wave.c" 3
 )) { allocp = do_unify_value(allocp, (
# 643 "wave.c"
 x3
# 643 "wave.c" 3
 ), (
# 643 "wave.c"
 x5
# 643 "wave.c" 3
 )); } else { (*((q *)((unsigned long)(
# 643 "wave.c"
 x3
# 643 "wave.c" 3
 ) - 0x0))) = (
# 643 "wave.c"
 x5
# 643 "wave.c" 3
 ); } }
# 643 "wave.c"
                    ;
  x6 = 
# 644 "wave.c" 3
      ((q)(((unsigned long)(0) << 4) + (0x2 +0x4)))
# 644 "wave.c"
             ;
  
# 645 "wave.c" 3
 { if (!(((unsigned long)(
# 645 "wave.c"
 a6
# 645 "wave.c" 3
 ) & 0x3) == 0x0) || (*((q *)((unsigned long)(
# 645 "wave.c"
 a6
# 645 "wave.c" 3
 ) - 0x0))) != (
# 645 "wave.c"
 a6
# 645 "wave.c" 3
 )) { allocp = do_unify_value(allocp, (
# 645 "wave.c"
 a6
# 645 "wave.c" 3
 ), (
# 645 "wave.c"
 x6
# 645 "wave.c" 3
 )); } else { (*((q *)((unsigned long)(
# 645 "wave.c"
 a6
# 645 "wave.c" 3
 ) - 0x0))) = (
# 645 "wave.c"
 x6
# 645 "wave.c" 3
 ); } }
# 645 "wave.c"
                    ;
  x7 = 
# 646 "wave.c" 3
      ((q)(((unsigned long)(0) << 4) + (0x2 +0x4)))
# 646 "wave.c"
             ;
  
# 647 "wave.c" 3
 { if (!(((unsigned long)(
# 647 "wave.c"
 a8
# 647 "wave.c" 3
 ) & 0x3) == 0x0) || (*((q *)((unsigned long)(
# 647 "wave.c"
 a8
# 647 "wave.c" 3
 ) - 0x0))) != (
# 647 "wave.c"
 a8
# 647 "wave.c" 3
 )) { allocp = do_unify_value(allocp, (
# 647 "wave.c"
 a8
# 647 "wave.c" 3
 ), (
# 647 "wave.c"
 x7
# 647 "wave.c" 3
 )); } else { (*((q *)((unsigned long)(
# 647 "wave.c"
 a8
# 647 "wave.c" 3
 ) - 0x0))) = (
# 647 "wave.c"
 x7
# 647 "wave.c" 3
 ); } }
# 647 "wave.c"
                    ;
  
# 648 "wave.c" 3
 { goto proceed_label; }
# 648 "wave.c"
          ;
 case 
# 649 "wave.c" 3
     0x0
# 649 "wave.c"
           :
  
# 650 "wave.c" 3
 { q temp0 = (*((q *)((unsigned long)(
# 650 "wave.c"
 a7
# 650 "wave.c" 3
 ) - 0x0))); if(!(((unsigned long)(temp0) & 0x3) == 0x0)) { (
# 650 "wave.c"
 a7
# 650 "wave.c" 3
 ) = temp0; goto 
# 650 "wave.c"
 col_13_8
# 650 "wave.c" 3
 ; } }
# 650 "wave.c"
                            ;
  *reasonp++ = a7;
  goto col_13_interrupt;
  };
  goto col_13_interrupt;
 case 
# 655 "wave.c" 3
     0x0
# 655 "wave.c"
           :
  
# 656 "wave.c" 3
 { q temp0 = (*((q *)((unsigned long)(
# 656 "wave.c"
 a5
# 656 "wave.c" 3
 ) - 0x0))); if(!(((unsigned long)(temp0) & 0x3) == 0x0)) { (
# 656 "wave.c"
 a5
# 656 "wave.c" 3
 ) = temp0; goto 
# 656 "wave.c"
 col_13_7
# 656 "wave.c" 3
 ; } }
# 656 "wave.c"
                            ;
  *reasonp++ = a5;
  goto col_13_interrupt;
  };
  goto col_13_interrupt;
 case 
# 661 "wave.c" 3
     0x0
# 661 "wave.c"
           :
  
# 662 "wave.c" 3
 { q temp0 = (*((q *)((unsigned long)(
# 662 "wave.c"
 a4
# 662 "wave.c" 3
 ) - 0x0))); if(!(((unsigned long)(temp0) & 0x3) == 0x0)) { (
# 662 "wave.c"
 a4
# 662 "wave.c" 3
 ) = temp0; goto 
# 662 "wave.c"
 col_13_4
# 662 "wave.c" 3
 ; } }
# 662 "wave.c"
                            ;
  *reasonp++ = a4;
  goto col_13_interrupt;
  };
  goto col_13_interrupt;
 col_13_ext_interrupt:
  reasonp = 0l;
 col_13_interrupt:
  toppred = &predicate_main_xcol_13;
  goto interrupt_13;
 }

 pt_16_top: {
  q x0, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13, x14, x15, x16, x17, x18, x19, x20, x21, x22, x23, x24, x25, x26, x27, x28, x29, x30, x31, x32, x33, x34, x35, x36, x37, x38, x39, x40, x41, x42, x43, x44, x45, x46, x47;
  a0 = qp->args[0];
  a1 = qp->args[1];
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  a6 = qp->args[6];
  a7 = qp->args[7];
  a8 = qp->args[8];
  a9 = qp->args[9];
  a10 = qp->args[10];
  a11 = qp->args[11];
  a12 = qp->args[12];
  a13 = qp->args[13];
  a14 = qp->args[14];
  a15 = qp->args[15];
  qp = qp->next;
 pt_16_clear_reason:
  reasonp = 
# 694 "wave.c" 3
           (glbl->reasons0)
# 694 "wave.c"
                  ;
 pt_16_0:
 pt_16_1:
  switch (
# 697 "wave.c" 3
         ((unsigned long)(
# 697 "wave.c"
         a5
# 697 "wave.c" 3
         ) & 0x3)
# 697 "wave.c"
                   ) {
 case 
# 698 "wave.c" 3
     0x3
# 698 "wave.c"
            :
 pt_16_2:
  if (!
# 700 "wave.c" 3
      ((((unsigned long)((((struct functor *)((unsigned long)(
# 700 "wave.c"
      a5
# 700 "wave.c" 3
      ) - 0x3))->functor)) & 0x3) == 0x0))
# 700 "wave.c"
                ) goto pt_16_27;
  if (!
# 701 "wave.c" 3
      ((struct data_object_method_table *)((((struct functor *)((unsigned long)(
# 701 "wave.c"
      a5
# 701 "wave.c" 3
      ) - 0x3))->functor)) == &
# 701 "wave.c"
      vector_g_data_method_table
# 701 "wave.c" 3
      )
# 701 "wave.c"
                        ) goto pt_16_27;
  
# 702 "wave.c" 3
 { extern q size_of_vector(); 
# 702 "wave.c"
 x0 
# 702 "wave.c" 3
 = size_of_vector(
# 702 "wave.c"
 a5
# 702 "wave.c" 3
 ); }
# 702 "wave.c"
                                    ;
 pt_16_3:
  if (x0 != 
# 704 "wave.c" 3
           ((q)(((long)(
# 704 "wave.c"
           2L
# 704 "wave.c" 3
           ) << 4) + (0x2 +0x0)))
# 704 "wave.c"
                      ) goto pt_16_27;
  x2 = 
# 705 "wave.c" 3
      ((q)(((long)(
# 705 "wave.c"
      0L
# 705 "wave.c" 3
      ) << 4) + (0x2 +0x0)))
# 705 "wave.c"
                 ;
  
# 706 "wave.c" 3
 { extern q element_of_vector(); 
# 706 "wave.c"
 x1 
# 706 "wave.c" 3
 = element_of_vector(
# 706 "wave.c"
 a5
# 706 "wave.c" 3
 ,
# 706 "wave.c"
 x2
# 706 "wave.c" 3
 ); if ((long)
# 706 "wave.c"
 x1 
# 706 "wave.c" 3
 == 0) goto 
# 706 "wave.c"
 pt_16_27
# 706 "wave.c" 3
 ; }
# 706 "wave.c"
                                          ;
  x4 = 
# 707 "wave.c" 3
      ((q)(((long)(
# 707 "wave.c"
      1L
# 707 "wave.c" 3
      ) << 4) + (0x2 +0x0)))
# 707 "wave.c"
                 ;
  
# 708 "wave.c" 3
 { extern q element_of_vector(); 
# 708 "wave.c"
 x3 
# 708 "wave.c" 3
 = element_of_vector(
# 708 "wave.c"
 a5
# 708 "wave.c" 3
 ,
# 708 "wave.c"
 x4
# 708 "wave.c" 3
 ); if ((long)
# 708 "wave.c"
 x3 
# 708 "wave.c" 3
 == 0) goto 
# 708 "wave.c"
 pt_16_27
# 708 "wave.c" 3
 ; }
# 708 "wave.c"
                                          ;
 pt_16_4:
  switch (
# 710 "wave.c" 3
         ((unsigned long)(
# 710 "wave.c"
         a6
# 710 "wave.c" 3
         ) & 0x3)
# 710 "wave.c"
                   ) {
 case 
# 711 "wave.c" 3
     0x3
# 711 "wave.c"
            :
 pt_16_5:
  if (!
# 713 "wave.c" 3
      ((((unsigned long)((((struct functor *)((unsigned long)(
# 713 "wave.c"
      a6
# 713 "wave.c" 3
      ) - 0x3))->functor)) & 0x3) == 0x0))
# 713 "wave.c"
                ) goto pt_16_27;
  if (!
# 714 "wave.c" 3
      ((struct data_object_method_table *)((((struct functor *)((unsigned long)(
# 714 "wave.c"
      a6
# 714 "wave.c" 3
      ) - 0x3))->functor)) == &
# 714 "wave.c"
      vector_g_data_method_table
# 714 "wave.c" 3
      )
# 714 "wave.c"
                        ) goto pt_16_27;
  
# 715 "wave.c" 3
 { extern q size_of_vector(); 
# 715 "wave.c"
 x5 
# 715 "wave.c" 3
 = size_of_vector(
# 715 "wave.c"
 a6
# 715 "wave.c" 3
 ); }
# 715 "wave.c"
                                    ;
 pt_16_6:
  if (x5 != 
# 717 "wave.c" 3
           ((q)(((long)(
# 717 "wave.c"
           2L
# 717 "wave.c" 3
           ) << 4) + (0x2 +0x0)))
# 717 "wave.c"
                      ) goto pt_16_27;
  x7 = 
# 718 "wave.c" 3
      ((q)(((long)(
# 718 "wave.c"
      0L
# 718 "wave.c" 3
      ) << 4) + (0x2 +0x0)))
# 718 "wave.c"
                 ;
  
# 719 "wave.c" 3
 { extern q element_of_vector(); 
# 719 "wave.c"
 x6 
# 719 "wave.c" 3
 = element_of_vector(
# 719 "wave.c"
 a6
# 719 "wave.c" 3
 ,
# 719 "wave.c"
 x7
# 719 "wave.c" 3
 ); if ((long)
# 719 "wave.c"
 x6 
# 719 "wave.c" 3
 == 0) goto 
# 719 "wave.c"
 pt_16_27
# 719 "wave.c" 3
 ; }
# 719 "wave.c"
                                          ;
  x9 = 
# 720 "wave.c" 3
      ((q)(((long)(
# 720 "wave.c"
      1L
# 720 "wave.c" 3
      ) << 4) + (0x2 +0x0)))
# 720 "wave.c"
                 ;
  
# 721 "wave.c" 3
 { extern q element_of_vector(); 
# 721 "wave.c"
 x8 
# 721 "wave.c" 3
 = element_of_vector(
# 721 "wave.c"
 a6
# 721 "wave.c" 3
 ,
# 721 "wave.c"
 x9
# 721 "wave.c" 3
 ); if ((long)
# 721 "wave.c"
 x8 
# 721 "wave.c" 3
 == 0) goto 
# 721 "wave.c"
 pt_16_27
# 721 "wave.c" 3
 ; }
# 721 "wave.c"
                                          ;
 pt_16_7:
  switch (
# 723 "wave.c" 3
         ((unsigned long)(
# 723 "wave.c"
         a7
# 723 "wave.c" 3
         ) & 0x3)
# 723 "wave.c"
                   ) {
 case 
# 724 "wave.c" 3
     0x3
# 724 "wave.c"
            :
 pt_16_8:
  if (!
# 726 "wave.c" 3
      ((((unsigned long)((((struct functor *)((unsigned long)(
# 726 "wave.c"
      a7
# 726 "wave.c" 3
      ) - 0x3))->functor)) & 0x3) == 0x0))
# 726 "wave.c"
                ) goto pt_16_27;
  if (!
# 727 "wave.c" 3
      ((struct data_object_method_table *)((((struct functor *)((unsigned long)(
# 727 "wave.c"
      a7
# 727 "wave.c" 3
      ) - 0x3))->functor)) == &
# 727 "wave.c"
      vector_g_data_method_table
# 727 "wave.c" 3
      )
# 727 "wave.c"
                        ) goto pt_16_27;
  
# 728 "wave.c" 3
 { extern q size_of_vector(); 
# 728 "wave.c"
 x10 
# 728 "wave.c" 3
 = size_of_vector(
# 728 "wave.c"
 a7
# 728 "wave.c" 3
 ); }
# 728 "wave.c"
                                     ;
 pt_16_9:
  if (x10 != 
# 730 "wave.c" 3
            ((q)(((long)(
# 730 "wave.c"
            2L
# 730 "wave.c" 3
            ) << 4) + (0x2 +0x0)))
# 730 "wave.c"
                       ) goto pt_16_27;
  x12 = 
# 731 "wave.c" 3
       ((q)(((long)(
# 731 "wave.c"
       0L
# 731 "wave.c" 3
       ) << 4) + (0x2 +0x0)))
# 731 "wave.c"
                  ;
  
# 732 "wave.c" 3
 { extern q element_of_vector(); 
# 732 "wave.c"
 x11 
# 732 "wave.c" 3
 = element_of_vector(
# 732 "wave.c"
 a7
# 732 "wave.c" 3
 ,
# 732 "wave.c"
 x12
# 732 "wave.c" 3
 ); if ((long)
# 732 "wave.c"
 x11 
# 732 "wave.c" 3
 == 0) goto 
# 732 "wave.c"
 pt_16_27
# 732 "wave.c" 3
 ; }
# 732 "wave.c"
                                            ;
  x14 = 
# 733 "wave.c" 3
       ((q)(((long)(
# 733 "wave.c"
       1L
# 733 "wave.c" 3
       ) << 4) + (0x2 +0x0)))
# 733 "wave.c"
                  ;
  
# 734 "wave.c" 3
 { extern q element_of_vector(); 
# 734 "wave.c"
 x13 
# 734 "wave.c" 3
 = element_of_vector(
# 734 "wave.c"
 a7
# 734 "wave.c" 3
 ,
# 734 "wave.c"
 x14
# 734 "wave.c" 3
 ); if ((long)
# 734 "wave.c"
 x13 
# 734 "wave.c" 3
 == 0) goto 
# 734 "wave.c"
 pt_16_27
# 734 "wave.c" 3
 ; }
# 734 "wave.c"
                                            ;
 pt_16_10:
  switch (
# 736 "wave.c" 3
         ((unsigned long)(
# 736 "wave.c"
         a8
# 736 "wave.c" 3
         ) & 0x3)
# 736 "wave.c"
                   ) {
 case 
# 737 "wave.c" 3
     0x3
# 737 "wave.c"
            :
 pt_16_11:
  if (!
# 739 "wave.c" 3
      ((((unsigned long)((((struct functor *)((unsigned long)(
# 739 "wave.c"
      a8
# 739 "wave.c" 3
      ) - 0x3))->functor)) & 0x3) == 0x0))
# 739 "wave.c"
                ) goto pt_16_27;
  if (!
# 740 "wave.c" 3
      ((struct data_object_method_table *)((((struct functor *)((unsigned long)(
# 740 "wave.c"
      a8
# 740 "wave.c" 3
      ) - 0x3))->functor)) == &
# 740 "wave.c"
      vector_g_data_method_table
# 740 "wave.c" 3
      )
# 740 "wave.c"
                        ) goto pt_16_27;
  
# 741 "wave.c" 3
 { extern q size_of_vector(); 
# 741 "wave.c"
 x15 
# 741 "wave.c" 3
 = size_of_vector(
# 741 "wave.c"
 a8
# 741 "wave.c" 3
 ); }
# 741 "wave.c"
                                     ;
 pt_16_12:
  if (x15 != 
# 743 "wave.c" 3
            ((q)(((long)(
# 743 "wave.c"
            2L
# 743 "wave.c" 3
            ) << 4) + (0x2 +0x0)))
# 743 "wave.c"
                       ) goto pt_16_27;
  x17 = 
# 744 "wave.c" 3
       ((q)(((long)(
# 744 "wave.c"
       0L
# 744 "wave.c" 3
       ) << 4) + (0x2 +0x0)))
# 744 "wave.c"
                  ;
  
# 745 "wave.c" 3
 { extern q element_of_vector(); 
# 745 "wave.c"
 x16 
# 745 "wave.c" 3
 = element_of_vector(
# 745 "wave.c"
 a8
# 745 "wave.c" 3
 ,
# 745 "wave.c"
 x17
# 745 "wave.c" 3
 ); if ((long)
# 745 "wave.c"
 x16 
# 745 "wave.c" 3
 == 0) goto 
# 745 "wave.c"
 pt_16_27
# 745 "wave.c" 3
 ; }
# 745 "wave.c"
                                            ;
  x19 = 
# 746 "wave.c" 3
       ((q)(((long)(
# 746 "wave.c"
       1L
# 746 "wave.c" 3
       ) << 4) + (0x2 +0x0)))
# 746 "wave.c"
                  ;
  
# 747 "wave.c" 3
 { extern q element_of_vector(); 
# 747 "wave.c"
 x18 
# 747 "wave.c" 3
 = element_of_vector(
# 747 "wave.c"
 a8
# 747 "wave.c" 3
 ,
# 747 "wave.c"
 x19
# 747 "wave.c" 3
 ); if ((long)
# 747 "wave.c"
 x18 
# 747 "wave.c" 3
 == 0) goto 
# 747 "wave.c"
 pt_16_27
# 747 "wave.c" 3
 ; }
# 747 "wave.c"
                                            ;
 pt_16_13:
  switch (
# 749 "wave.c" 3
         ((unsigned long)(
# 749 "wave.c"
         a9
# 749 "wave.c" 3
         ) & 0x3)
# 749 "wave.c"
                   ) {
 case 
# 750 "wave.c" 3
     0x3
# 750 "wave.c"
            :
 pt_16_14:
  if (!
# 752 "wave.c" 3
      ((((unsigned long)((((struct functor *)((unsigned long)(
# 752 "wave.c"
      a9
# 752 "wave.c" 3
      ) - 0x3))->functor)) & 0x3) == 0x0))
# 752 "wave.c"
                ) goto pt_16_27;
  if (!
# 753 "wave.c" 3
      ((struct data_object_method_table *)((((struct functor *)((unsigned long)(
# 753 "wave.c"
      a9
# 753 "wave.c" 3
      ) - 0x3))->functor)) == &
# 753 "wave.c"
      vector_g_data_method_table
# 753 "wave.c" 3
      )
# 753 "wave.c"
                        ) goto pt_16_27;
  
# 754 "wave.c" 3
 { extern q size_of_vector(); 
# 754 "wave.c"
 x20 
# 754 "wave.c" 3
 = size_of_vector(
# 754 "wave.c"
 a9
# 754 "wave.c" 3
 ); }
# 754 "wave.c"
                                     ;
 pt_16_15:
  if (x20 != 
# 756 "wave.c" 3
            ((q)(((long)(
# 756 "wave.c"
            2L
# 756 "wave.c" 3
            ) << 4) + (0x2 +0x0)))
# 756 "wave.c"
                       ) goto pt_16_27;
  x22 = 
# 757 "wave.c" 3
       ((q)(((long)(
# 757 "wave.c"
       0L
# 757 "wave.c" 3
       ) << 4) + (0x2 +0x0)))
# 757 "wave.c"
                  ;
  
# 758 "wave.c" 3
 { extern q element_of_vector(); 
# 758 "wave.c"
 x21 
# 758 "wave.c" 3
 = element_of_vector(
# 758 "wave.c"
 a9
# 758 "wave.c" 3
 ,
# 758 "wave.c"
 x22
# 758 "wave.c" 3
 ); if ((long)
# 758 "wave.c"
 x21 
# 758 "wave.c" 3
 == 0) goto 
# 758 "wave.c"
 pt_16_27
# 758 "wave.c" 3
 ; }
# 758 "wave.c"
                                            ;
  x24 = 
# 759 "wave.c" 3
       ((q)(((long)(
# 759 "wave.c"
       1L
# 759 "wave.c" 3
       ) << 4) + (0x2 +0x0)))
# 759 "wave.c"
                  ;
  
# 760 "wave.c" 3
 { extern q element_of_vector(); 
# 760 "wave.c"
 x23 
# 760 "wave.c" 3
 = element_of_vector(
# 760 "wave.c"
 a9
# 760 "wave.c" 3
 ,
# 760 "wave.c"
 x24
# 760 "wave.c" 3
 ); if ((long)
# 760 "wave.c"
 x23 
# 760 "wave.c" 3
 == 0) goto 
# 760 "wave.c"
 pt_16_27
# 760 "wave.c" 3
 ; }
# 760 "wave.c"
                                            ;
 pt_16_16:
  switch (
# 762 "wave.c" 3
         ((unsigned long)(
# 762 "wave.c"
         a10
# 762 "wave.c" 3
         ) & 0x3)
# 762 "wave.c"
                    ) {
 case 
# 763 "wave.c" 3
     0x3
# 763 "wave.c"
            :
 pt_16_17:
  if (!
# 765 "wave.c" 3
      ((((unsigned long)((((struct functor *)((unsigned long)(
# 765 "wave.c"
      a10
# 765 "wave.c" 3
      ) - 0x3))->functor)) & 0x3) == 0x0))
# 765 "wave.c"
                 ) goto pt_16_27;
  if (!
# 766 "wave.c" 3
      ((struct data_object_method_table *)((((struct functor *)((unsigned long)(
# 766 "wave.c"
      a10
# 766 "wave.c" 3
      ) - 0x3))->functor)) == &
# 766 "wave.c"
      vector_g_data_method_table
# 766 "wave.c" 3
      )
# 766 "wave.c"
                         ) goto pt_16_27;
  
# 767 "wave.c" 3
 { extern q size_of_vector(); 
# 767 "wave.c"
 x25 
# 767 "wave.c" 3
 = size_of_vector(
# 767 "wave.c"
 a10
# 767 "wave.c" 3
 ); }
# 767 "wave.c"
                                      ;
 pt_16_18:
  if (x25 != 
# 769 "wave.c" 3
            ((q)(((long)(
# 769 "wave.c"
            2L
# 769 "wave.c" 3
            ) << 4) + (0x2 +0x0)))
# 769 "wave.c"
                       ) goto pt_16_27;
  x27 = 
# 770 "wave.c" 3
       ((q)(((long)(
# 770 "wave.c"
       0L
# 770 "wave.c" 3
       ) << 4) + (0x2 +0x0)))
# 770 "wave.c"
                  ;
  
# 771 "wave.c" 3
 { extern q element_of_vector(); 
# 771 "wave.c"
 x26 
# 771 "wave.c" 3
 = element_of_vector(
# 771 "wave.c"
 a10
# 771 "wave.c" 3
 ,
# 771 "wave.c"
 x27
# 771 "wave.c" 3
 ); if ((long)
# 771 "wave.c"
 x26 
# 771 "wave.c" 3
 == 0) goto 
# 771 "wave.c"
 pt_16_27
# 771 "wave.c" 3
 ; }
# 771 "wave.c"
                                             ;
  x29 = 
# 772 "wave.c" 3
       ((q)(((long)(
# 772 "wave.c"
       1L
# 772 "wave.c" 3
       ) << 4) + (0x2 +0x0)))
# 772 "wave.c"
                  ;
  
# 773 "wave.c" 3
 { extern q element_of_vector(); 
# 773 "wave.c"
 x28 
# 773 "wave.c" 3
 = element_of_vector(
# 773 "wave.c"
 a10
# 773 "wave.c" 3
 ,
# 773 "wave.c"
 x29
# 773 "wave.c" 3
 ); if ((long)
# 773 "wave.c"
 x28 
# 773 "wave.c" 3
 == 0) goto 
# 773 "wave.c"
 pt_16_27
# 773 "wave.c" 3
 ; }
# 773 "wave.c"
                                             ;
 pt_16_19:
  if (!
# 775 "wave.c" 3
      (((unsigned long)(
# 775 "wave.c"
      a0
# 775 "wave.c" 3
      ) & 0xFL) == (0x2 +0x0))
# 775 "wave.c"
               ) goto pt_16_26;
  x30 = 
# 776 "wave.c" 3
       ((q)(((long)(
# 776 "wave.c"
       2L
# 776 "wave.c" 3
       ) << 4) + (0x2 +0x0)))
# 776 "wave.c"
                  ;
  
# 777 "wave.c" 3
 if ((long)(
# 777 "wave.c"
 x30
# 777 "wave.c" 3
 ) > (long)(
# 777 "wave.c"
 a0
# 777 "wave.c" 3
 )) goto 
# 777 "wave.c"
 pt_16_27
# 777 "wave.c" 3
 ;
# 777 "wave.c"
                                 ;
 pt_16_20:
  if (!
# 779 "wave.c" 3
      (((unsigned long)(
# 779 "wave.c"
      a3
# 779 "wave.c" 3
      ) & 0xFL) == (0x2 +0x0))
# 779 "wave.c"
               ) goto pt_16_25;
  
# 780 "wave.c" 3
 if ((long)(
# 780 "wave.c"
 a0
# 780 "wave.c" 3
 ) >= (long)(
# 780 "wave.c"
 a3
# 780 "wave.c" 3
 )) goto 
# 780 "wave.c"
 pt_16_27
# 780 "wave.c" 3
 ;
# 780 "wave.c"
                          ;
 pt_16_21:
  if (!
# 782 "wave.c" 3
      (((unsigned long)(
# 782 "wave.c"
      a1
# 782 "wave.c" 3
      ) & 0xFL) == (0x2 +0x0))
# 782 "wave.c"
               ) goto pt_16_24;
  x31 = 
# 783 "wave.c" 3
       ((q)(((long)(
# 783 "wave.c"
       1L
# 783 "wave.c" 3
       ) << 4) + (0x2 +0x0)))
# 783 "wave.c"
                  ;
  
# 784 "wave.c" 3
 if ((long)(
# 784 "wave.c"
 x31
# 784 "wave.c" 3
 ) > (long)(
# 784 "wave.c"
 a1
# 784 "wave.c" 3
 )) goto 
# 784 "wave.c"
 pt_16_27
# 784 "wave.c" 3
 ;
# 784 "wave.c"
                                 ;
  
# 785 "wave.c" 3
 if ((long)(
# 785 "wave.c"
 a1
# 785 "wave.c" 3
 ) > (long)(
# 785 "wave.c"
 a3
# 785 "wave.c" 3
 )) goto 
# 785 "wave.c"
 pt_16_27
# 785 "wave.c" 3
 ;
# 785 "wave.c"
                                ;
 pt_16_22:
  if (!
# 787 "wave.c" 3
      (((unsigned long)(
# 787 "wave.c"
      a2
# 787 "wave.c" 3
      ) & 0xFL) == (0x2 +0x0))
# 787 "wave.c"
               ) goto pt_16_23;
  x32 = 
# 788 "wave.c" 3
       ((q)(((long)(
# 788 "wave.c"
       1L
# 788 "wave.c" 3
       ) << 4) + (0x2 +0x0)))
# 788 "wave.c"
                  ;
  
# 789 "wave.c" 3
 if ((long)(
# 789 "wave.c"
 x32
# 789 "wave.c" 3
 ) > (long)(
# 789 "wave.c"
 a2
# 789 "wave.c" 3
 )) goto 
# 789 "wave.c"
 pt_16_27
# 789 "wave.c" 3
 ;
# 789 "wave.c"
                                 ;
  
# 790 "wave.c" 3
 if ((long)(
# 790 "wave.c"
 a2
# 790 "wave.c" 3
 ) > (long)(
# 790 "wave.c"
 a3
# 790 "wave.c" 3
 )) goto 
# 790 "wave.c"
 pt_16_27
# 790 "wave.c" 3
 ;
# 790 "wave.c"
                                ;
  
# 791 "wave.c" 3
 { allocp = do_unify(allocp, (
# 791 "wave.c"
 a14
# 791 "wave.c" 3
 ), (
# 791 "wave.c"
 a13
# 791 "wave.c" 3
 )); }
# 791 "wave.c"
                ;
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_main_xnode_7);
  allocp[2] = a0;
  allocp[3] = a1;
  allocp[4] = a2;
  allocp[5] = x33 = 
# 797 "wave.c" 3
                   (q) ((unsigned long)(
# 797 "wave.c"
                   &allocp[5]
# 797 "wave.c" 3
                   ) + 0x0)
# 797 "wave.c"
                                      ;
  allocp[6] = x34 = 
# 798 "wave.c" 3
                   (q) ((unsigned long)(
# 798 "wave.c"
                   &allocp[6]
# 798 "wave.c" 3
                   ) + 0x0)
# 798 "wave.c"
                                      ;
  allocp[7] = a11;
  allocp[8] = a12;
  allocp[9] = 
# 801 "wave.c" 3
             ((q)(((unsigned long)(
# 801 "wave.c"
             191L+
# 801 "wave.c" 3
             0) << 4) + (0x2 +0x4)))
# 801 "wave.c"
                                    ;
  allocp[10] = a11;
  x35 = 
# 803 "wave.c" 3
       ((q)((unsigned long)(
# 803 "wave.c"
       &allocp[9]
# 803 "wave.c" 3
       ) + 0x3))
# 803 "wave.c"
                              ;
  allocp[11] = (q)(struct goalrec*)&allocp[0];
  allocp[12] = (q)(&predicate_main_xfilter_8);
  allocp[13] = a15;
  allocp[14] = x26;
  allocp[15] = x23;
  allocp[16] = x35;
  allocp[17] = x36 = 
# 810 "wave.c" 3
                    (q) ((unsigned long)(
# 810 "wave.c"
                    &allocp[17]
# 810 "wave.c" 3
                    ) + 0x0)
# 810 "wave.c"
                                        ;
  allocp[18] = 
# 811 "wave.c" 3
              ((q)(((unsigned long)(0) << 4) + (0x2 +0x4)))
# 811 "wave.c"
                     ;
  allocp[19] = x37 = 
# 812 "wave.c" 3
                    (q) ((unsigned long)(
# 812 "wave.c"
                    &allocp[19]
# 812 "wave.c" 3
                    ) + 0x0)
# 812 "wave.c"
                                        ;
  allocp[20] = 
# 813 "wave.c" 3
              ((q)(((unsigned long)(0) << 4) + (0x2 +0x4)))
# 813 "wave.c"
                     ;
  allocp[21] = (q)(struct goalrec*)&allocp[11];
  allocp[22] = (q)(&predicate_main_xfilter_8);
  allocp[23] = a15;
  allocp[24] = x21;
  allocp[25] = x28;
  allocp[26] = 
# 819 "wave.c" 3
              ((q)(((unsigned long)(
# 819 "wave.c"
              82L+
# 819 "wave.c" 3
              2) << 4) + (0x2 +0x4)))
# 819 "wave.c"
                                ;
  allocp[27] = x38 = 
# 820 "wave.c" 3
                    (q) ((unsigned long)(
# 820 "wave.c"
                    &allocp[27]
# 820 "wave.c" 3
                    ) + 0x0)
# 820 "wave.c"
                                        ;
  allocp[28] = x36;
  allocp[29] = x39 = 
# 822 "wave.c" 3
                    (q) ((unsigned long)(
# 822 "wave.c"
                    &allocp[29]
# 822 "wave.c" 3
                    ) + 0x0)
# 822 "wave.c"
                                        ;
  allocp[30] = x37;
  allocp[31] = (q)(struct goalrec*)&allocp[21];
  allocp[32] = (q)(&predicate_main_xfilter_8);
  allocp[33] = a15;
  allocp[34] = x16;
  allocp[35] = x8;
  allocp[36] = 
# 829 "wave.c" 3
              ((q)(((unsigned long)(
# 829 "wave.c"
              82L+
# 829 "wave.c" 3
              2) << 4) + (0x2 +0x4)))
# 829 "wave.c"
                                ;
  allocp[37] = x40 = 
# 830 "wave.c" 3
                    (q) ((unsigned long)(
# 830 "wave.c"
                    &allocp[37]
# 830 "wave.c" 3
                    ) + 0x0)
# 830 "wave.c"
                                        ;
  allocp[38] = x38;
  allocp[39] = x41 = 
# 832 "wave.c" 3
                    (q) ((unsigned long)(
# 832 "wave.c"
                    &allocp[39]
# 832 "wave.c" 3
                    ) + 0x0)
# 832 "wave.c"
                                        ;
  allocp[40] = x39;
  allocp[41] = (q)(struct goalrec*)&allocp[31];
  allocp[42] = (q)(&predicate_main_xfilter_8);
  allocp[43] = a15;
  allocp[44] = x11;
  allocp[45] = x3;
  allocp[46] = 
# 839 "wave.c" 3
              ((q)(((unsigned long)(
# 839 "wave.c"
              82L+
# 839 "wave.c" 3
              2) << 4) + (0x2 +0x4)))
# 839 "wave.c"
                                ;
  allocp[47] = x42 = 
# 840 "wave.c" 3
                    (q) ((unsigned long)(
# 840 "wave.c"
                    &allocp[47]
# 840 "wave.c" 3
                    ) + 0x0)
# 840 "wave.c"
                                        ;
  allocp[48] = x40;
  allocp[49] = x43 = 
# 842 "wave.c" 3
                    (q) ((unsigned long)(
# 842 "wave.c"
                    &allocp[49]
# 842 "wave.c" 3
                    ) + 0x0)
# 842 "wave.c"
                                        ;
  allocp[50] = x41;
  allocp[51] = 
# 844 "wave.c" 3
              ((q)(((unsigned long)(
# 844 "wave.c"
              191L+
# 844 "wave.c" 3
              0) << 4) + (0x2 +0x4)))
# 844 "wave.c"
                                     ;
  allocp[52] = a11;
  x44 = 
# 846 "wave.c" 3
       ((q)((unsigned long)(
# 846 "wave.c"
       &allocp[51]
# 846 "wave.c" 3
       ) + 0x3))
# 846 "wave.c"
                               ;
  allocp[53] = (q)(struct goalrec*)&allocp[41];
  allocp[54] = (q)(&predicate_main_xfilter_8);
  allocp[55] = a15;
  allocp[56] = x6;
  allocp[57] = x18;
  allocp[58] = x44;
  allocp[59] = x45 = 
# 853 "wave.c" 3
                    (q) ((unsigned long)(
# 853 "wave.c"
                    &allocp[59]
# 853 "wave.c" 3
                    ) + 0x0)
# 853 "wave.c"
                                        ;
  allocp[60] = x42;
  allocp[61] = x46 = 
# 855 "wave.c" 3
                    (q) ((unsigned long)(
# 855 "wave.c"
                    &allocp[61]
# 855 "wave.c" 3
                    ) + 0x0)
# 855 "wave.c"
                                        ;
  allocp[62] = x43;
  allocp[63] = 
# 857 "wave.c" 3
              ((q)(((unsigned long)(
# 857 "wave.c"
              191L+
# 857 "wave.c" 3
              0) << 4) + (0x2 +0x4)))
# 857 "wave.c"
                                     ;
  allocp[64] = a11;
  x47 = 
# 859 "wave.c" 3
       ((q)((unsigned long)(
# 859 "wave.c"
       &allocp[63]
# 859 "wave.c" 3
       ) + 0x3))
# 859 "wave.c"
                               ;
  a0 = a15;
  a1 = x1;
  a2 = x13;
  a3 = x47;
  a4 = x33;
  a5 = x45;
  a6 = x34;
  a7 = x46;
  qp = (struct goalrec*)&allocp[53];
  allocp += 65;
  
# 870 "wave.c" 3
 { if (allocp < (glbl->heaplimit0)) goto 
# 870 "wave.c"
 filter_8_0
# 870 "wave.c" 3
 ;}
# 870 "wave.c"
                    ;
  goto filter_8_ext_interrupt;
 pt_16_23:
  if (!
# 873 "wave.c" 3
      (((unsigned long)(
# 873 "wave.c"
      a2
# 873 "wave.c" 3
      ) & 0x3) == 0x0)
# 873 "wave.c"
               ) goto pt_16_27;
  
# 874 "wave.c" 3
 { q temp0 = (*((q *)((unsigned long)(
# 874 "wave.c"
 a2
# 874 "wave.c" 3
 ) - 0x0))); if(!(((unsigned long)(temp0) & 0x3) == 0x0)) { (
# 874 "wave.c"
 a2
# 874 "wave.c" 3
 ) = temp0; goto 
# 874 "wave.c"
 pt_16_22
# 874 "wave.c" 3
 ; } }
# 874 "wave.c"
                            ;
  *reasonp++ = a2;
  goto pt_16_27;
 pt_16_24:
  if (!
# 878 "wave.c" 3
      (((unsigned long)(
# 878 "wave.c"
      a1
# 878 "wave.c" 3
      ) & 0x3) == 0x0)
# 878 "wave.c"
               ) goto pt_16_27;
  
# 879 "wave.c" 3
 { q temp0 = (*((q *)((unsigned long)(
# 879 "wave.c"
 a1
# 879 "wave.c" 3
 ) - 0x0))); if(!(((unsigned long)(temp0) & 0x3) == 0x0)) { (
# 879 "wave.c"
 a1
# 879 "wave.c" 3
 ) = temp0; goto 
# 879 "wave.c"
 pt_16_21
# 879 "wave.c" 3
 ; } }
# 879 "wave.c"
                            ;
  *reasonp++ = a1;
  goto pt_16_27;
 pt_16_25:
  if (!
# 883 "wave.c" 3
      (((unsigned long)(
# 883 "wave.c"
      a3
# 883 "wave.c" 3
      ) & 0x3) == 0x0)
# 883 "wave.c"
               ) goto pt_16_27;
  
# 884 "wave.c" 3
 { q temp0 = (*((q *)((unsigned long)(
# 884 "wave.c"
 a3
# 884 "wave.c" 3
 ) - 0x0))); if(!(((unsigned long)(temp0) & 0x3) == 0x0)) { (
# 884 "wave.c"
 a3
# 884 "wave.c" 3
 ) = temp0; goto 
# 884 "wave.c"
 pt_16_20
# 884 "wave.c" 3
 ; } }
# 884 "wave.c"
                            ;
  *reasonp++ = a3;
  goto pt_16_27;
 pt_16_26:
  if (!
# 888 "wave.c" 3
      (((unsigned long)(
# 888 "wave.c"
      a0
# 888 "wave.c" 3
      ) & 0x3) == 0x0)
# 888 "wave.c"
               ) goto pt_16_27;
  
# 889 "wave.c" 3
 { q temp0 = (*((q *)((unsigned long)(
# 889 "wave.c"
 a0
# 889 "wave.c" 3
 ) - 0x0))); if(!(((unsigned long)(temp0) & 0x3) == 0x0)) { (
# 889 "wave.c"
 a0
# 889 "wave.c" 3
 ) = temp0; goto 
# 889 "wave.c"
 pt_16_19
# 889 "wave.c" 3
 ; } }
# 889 "wave.c"
                            ;
  *reasonp++ = a0;
  goto pt_16_27;
 case 
# 892 "wave.c" 3
     0x0
# 892 "wave.c"
           :
  
# 893 "wave.c" 3
 { q temp0 = (*((q *)((unsigned long)(
# 893 "wave.c"
 a10
# 893 "wave.c" 3
 ) - 0x0))); if(!(((unsigned long)(temp0) & 0x3) == 0x0)) { (
# 893 "wave.c"
 a10
# 893 "wave.c" 3
 ) = temp0; goto 
# 893 "wave.c"
 pt_16_16
# 893 "wave.c" 3
 ; } }
# 893 "wave.c"
                             ;
  *reasonp++ = a10;
  goto pt_16_27;
  };
  goto pt_16_27;
 case 
# 898 "wave.c" 3
     0x0
# 898 "wave.c"
           :
  
# 899 "wave.c" 3
 { q temp0 = (*((q *)((unsigned long)(
# 899 "wave.c"
 a9
# 899 "wave.c" 3
 ) - 0x0))); if(!(((unsigned long)(temp0) & 0x3) == 0x0)) { (
# 899 "wave.c"
 a9
# 899 "wave.c" 3
 ) = temp0; goto 
# 899 "wave.c"
 pt_16_13
# 899 "wave.c" 3
 ; } }
# 899 "wave.c"
                            ;
  *reasonp++ = a9;
  goto pt_16_27;
  };
  goto pt_16_27;
 case 
# 904 "wave.c" 3
     0x0
# 904 "wave.c"
           :
  
# 905 "wave.c" 3
 { q temp0 = (*((q *)((unsigned long)(
# 905 "wave.c"
 a8
# 905 "wave.c" 3
 ) - 0x0))); if(!(((unsigned long)(temp0) & 0x3) == 0x0)) { (
# 905 "wave.c"
 a8
# 905 "wave.c" 3
 ) = temp0; goto 
# 905 "wave.c"
 pt_16_10
# 905 "wave.c" 3
 ; } }
# 905 "wave.c"
                            ;
  *reasonp++ = a8;
  goto pt_16_27;
  };
  goto pt_16_27;
 case 
# 910 "wave.c" 3
     0x0
# 910 "wave.c"
           :
  
# 911 "wave.c" 3
 { q temp0 = (*((q *)((unsigned long)(
# 911 "wave.c"
 a7
# 911 "wave.c" 3
 ) - 0x0))); if(!(((unsigned long)(temp0) & 0x3) == 0x0)) { (
# 911 "wave.c"
 a7
# 911 "wave.c" 3
 ) = temp0; goto 
# 911 "wave.c"
 pt_16_7
# 911 "wave.c" 3
 ; } }
# 911 "wave.c"
                           ;
  *reasonp++ = a7;
  goto pt_16_27;
  };
  goto pt_16_27;
 case 
# 916 "wave.c" 3
     0x0
# 916 "wave.c"
           :
  
# 917 "wave.c" 3
 { q temp0 = (*((q *)((unsigned long)(
# 917 "wave.c"
 a6
# 917 "wave.c" 3
 ) - 0x0))); if(!(((unsigned long)(temp0) & 0x3) == 0x0)) { (
# 917 "wave.c"
 a6
# 917 "wave.c" 3
 ) = temp0; goto 
# 917 "wave.c"
 pt_16_4
# 917 "wave.c" 3
 ; } }
# 917 "wave.c"
                           ;
  *reasonp++ = a6;
  goto pt_16_27;
  };
  goto pt_16_27;
 case 
# 922 "wave.c" 3
     0x0
# 922 "wave.c"
           :
  
# 923 "wave.c" 3
 { q temp0 = (*((q *)((unsigned long)(
# 923 "wave.c"
 a5
# 923 "wave.c" 3
 ) - 0x0))); if(!(((unsigned long)(temp0) & 0x3) == 0x0)) { (
# 923 "wave.c"
 a5
# 923 "wave.c" 3
 ) = temp0; goto 
# 923 "wave.c"
 pt_16_1
# 923 "wave.c" 3
 ; } }
# 923 "wave.c"
                           ;
  *reasonp++ = a5;
  goto pt_16_27;
  };
  goto pt_16_27;
 pt_16_27:
 pt_16_28:
  switch (
# 930 "wave.c" 3
         ((unsigned long)(
# 930 "wave.c"
         a0
# 930 "wave.c" 3
         ) & 0x3)
# 930 "wave.c"
                   ) {
 case 
# 931 "wave.c" 3
     0x2
# 931 "wave.c"
           :
 pt_16_29:
  if (a0 != 
# 933 "wave.c" 3
           ((q)(((long)(
# 933 "wave.c"
           1L
# 933 "wave.c" 3
           ) << 4) + (0x2 +0x0)))
# 933 "wave.c"
                      ) goto pt_16_54;
 pt_16_30:
  switch (
# 935 "wave.c" 3
         ((unsigned long)(
# 935 "wave.c"
         a5
# 935 "wave.c" 3
         ) & 0x3)
# 935 "wave.c"
                   ) {
 case 
# 936 "wave.c" 3
     0x3
# 936 "wave.c"
            :
 pt_16_31:
  if (!
# 938 "wave.c" 3
      ((((unsigned long)((((struct functor *)((unsigned long)(
# 938 "wave.c"
      a5
# 938 "wave.c" 3
      ) - 0x3))->functor)) & 0x3) == 0x0))
# 938 "wave.c"
                ) goto pt_16_54;
  if (!
# 939 "wave.c" 3
      ((struct data_object_method_table *)((((struct functor *)((unsigned long)(
# 939 "wave.c"
      a5
# 939 "wave.c" 3
      ) - 0x3))->functor)) == &
# 939 "wave.c"
      vector_g_data_method_table
# 939 "wave.c" 3
      )
# 939 "wave.c"
                        ) goto pt_16_54;
  
# 940 "wave.c" 3
 { extern q size_of_vector(); 
# 940 "wave.c"
 x0 
# 940 "wave.c" 3
 = size_of_vector(
# 940 "wave.c"
 a5
# 940 "wave.c" 3
 ); }
# 940 "wave.c"
                                    ;
 pt_16_32:
  if (x0 != 
# 942 "wave.c" 3
           ((q)(((long)(
# 942 "wave.c"
           2L
# 942 "wave.c" 3
           ) << 4) + (0x2 +0x0)))
# 942 "wave.c"
                      ) goto pt_16_54;
  x2 = 
# 943 "wave.c" 3
      ((q)(((long)(
# 943 "wave.c"
      0L
# 943 "wave.c" 3
      ) << 4) + (0x2 +0x0)))
# 943 "wave.c"
                 ;
  
# 944 "wave.c" 3
 { extern q element_of_vector(); 
# 944 "wave.c"
 x1 
# 944 "wave.c" 3
 = element_of_vector(
# 944 "wave.c"
 a5
# 944 "wave.c" 3
 ,
# 944 "wave.c"
 x2
# 944 "wave.c" 3
 ); if ((long)
# 944 "wave.c"
 x1 
# 944 "wave.c" 3
 == 0) goto 
# 944 "wave.c"
 pt_16_54
# 944 "wave.c" 3
 ; }
# 944 "wave.c"
                                          ;
  x4 = 
# 945 "wave.c" 3
      ((q)(((long)(
# 945 "wave.c"
      1L
# 945 "wave.c" 3
      ) << 4) + (0x2 +0x0)))
# 945 "wave.c"
                 ;
  
# 946 "wave.c" 3
 { extern q element_of_vector(); 
# 946 "wave.c"
 x3 
# 946 "wave.c" 3
 = element_of_vector(
# 946 "wave.c"
 a5
# 946 "wave.c" 3
 ,
# 946 "wave.c"
 x4
# 946 "wave.c" 3
 ); if ((long)
# 946 "wave.c"
 x3 
# 946 "wave.c" 3
 == 0) goto 
# 946 "wave.c"
 pt_16_54
# 946 "wave.c" 3
 ; }
# 946 "wave.c"
                                          ;
 pt_16_33:
  switch (
# 948 "wave.c" 3
         ((unsigned long)(
# 948 "wave.c"
         a6
# 948 "wave.c" 3
         ) & 0x3)
# 948 "wave.c"
                   ) {
 case 
# 949 "wave.c" 3
     0x3
# 949 "wave.c"
            :
 pt_16_34:
  if (!
# 951 "wave.c" 3
      ((((unsigned long)((((struct functor *)((unsigned long)(
# 951 "wave.c"
      a6
# 951 "wave.c" 3
      ) - 0x3))->functor)) & 0x3) == 0x0))
# 951 "wave.c"
                ) goto pt_16_54;
  if (!
# 952 "wave.c" 3
      ((struct data_object_method_table *)((((struct functor *)((unsigned long)(
# 952 "wave.c"
      a6
# 952 "wave.c" 3
      ) - 0x3))->functor)) == &
# 952 "wave.c"
      vector_g_data_method_table
# 952 "wave.c" 3
      )
# 952 "wave.c"
                        ) goto pt_16_54;
  
# 953 "wave.c" 3
 { extern q size_of_vector(); 
# 953 "wave.c"
 x5 
# 953 "wave.c" 3
 = size_of_vector(
# 953 "wave.c"
 a6
# 953 "wave.c" 3
 ); }
# 953 "wave.c"
                                    ;
 pt_16_35:
  if (x5 != 
# 955 "wave.c" 3
           ((q)(((long)(
# 955 "wave.c"
           2L
# 955 "wave.c" 3
           ) << 4) + (0x2 +0x0)))
# 955 "wave.c"
                      ) goto pt_16_54;
  x7 = 
# 956 "wave.c" 3
      ((q)(((long)(
# 956 "wave.c"
      0L
# 956 "wave.c" 3
      ) << 4) + (0x2 +0x0)))
# 956 "wave.c"
                 ;
  
# 957 "wave.c" 3
 { extern q element_of_vector(); 
# 957 "wave.c"
 x6 
# 957 "wave.c" 3
 = element_of_vector(
# 957 "wave.c"
 a6
# 957 "wave.c" 3
 ,
# 957 "wave.c"
 x7
# 957 "wave.c" 3
 ); if ((long)
# 957 "wave.c"
 x6 
# 957 "wave.c" 3
 == 0) goto 
# 957 "wave.c"
 pt_16_54
# 957 "wave.c" 3
 ; }
# 957 "wave.c"
                                          ;
  x9 = 
# 958 "wave.c" 3
      ((q)(((long)(
# 958 "wave.c"
      1L
# 958 "wave.c" 3
      ) << 4) + (0x2 +0x0)))
# 958 "wave.c"
                 ;
  
# 959 "wave.c" 3
 { extern q element_of_vector(); 
# 959 "wave.c"
 x8 
# 959 "wave.c" 3
 = element_of_vector(
# 959 "wave.c"
 a6
# 959 "wave.c" 3
 ,
# 959 "wave.c"
 x9
# 959 "wave.c" 3
 ); if ((long)
# 959 "wave.c"
 x8 
# 959 "wave.c" 3
 == 0) goto 
# 959 "wave.c"
 pt_16_54
# 959 "wave.c" 3
 ; }
# 959 "wave.c"
                                          ;
 pt_16_36:
  switch (
# 961 "wave.c" 3
         ((unsigned long)(
# 961 "wave.c"
         a7
# 961 "wave.c" 3
         ) & 0x3)
# 961 "wave.c"
                   ) {
 case 
# 962 "wave.c" 3
     0x3
# 962 "wave.c"
            :
 pt_16_37:
  if (!
# 964 "wave.c" 3
      ((((unsigned long)((((struct functor *)((unsigned long)(
# 964 "wave.c"
      a7
# 964 "wave.c" 3
      ) - 0x3))->functor)) & 0x3) == 0x0))
# 964 "wave.c"
                ) goto pt_16_54;
  if (!
# 965 "wave.c" 3
      ((struct data_object_method_table *)((((struct functor *)((unsigned long)(
# 965 "wave.c"
      a7
# 965 "wave.c" 3
      ) - 0x3))->functor)) == &
# 965 "wave.c"
      vector_g_data_method_table
# 965 "wave.c" 3
      )
# 965 "wave.c"
                        ) goto pt_16_54;
  
# 966 "wave.c" 3
 { extern q size_of_vector(); 
# 966 "wave.c"
 x10 
# 966 "wave.c" 3
 = size_of_vector(
# 966 "wave.c"
 a7
# 966 "wave.c" 3
 ); }
# 966 "wave.c"
                                     ;
 pt_16_38:
  if (x10 != 
# 968 "wave.c" 3
            ((q)(((long)(
# 968 "wave.c"
            2L
# 968 "wave.c" 3
            ) << 4) + (0x2 +0x0)))
# 968 "wave.c"
                       ) goto pt_16_54;
  x12 = 
# 969 "wave.c" 3
       ((q)(((long)(
# 969 "wave.c"
       0L
# 969 "wave.c" 3
       ) << 4) + (0x2 +0x0)))
# 969 "wave.c"
                  ;
  
# 970 "wave.c" 3
 { extern q element_of_vector(); 
# 970 "wave.c"
 x11 
# 970 "wave.c" 3
 = element_of_vector(
# 970 "wave.c"
 a7
# 970 "wave.c" 3
 ,
# 970 "wave.c"
 x12
# 970 "wave.c" 3
 ); if ((long)
# 970 "wave.c"
 x11 
# 970 "wave.c" 3
 == 0) goto 
# 970 "wave.c"
 pt_16_54
# 970 "wave.c" 3
 ; }
# 970 "wave.c"
                                            ;
  x14 = 
# 971 "wave.c" 3
       ((q)(((long)(
# 971 "wave.c"
       1L
# 971 "wave.c" 3
       ) << 4) + (0x2 +0x0)))
# 971 "wave.c"
                  ;
  
# 972 "wave.c" 3
 { extern q element_of_vector(); 
# 972 "wave.c"
 x13 
# 972 "wave.c" 3
 = element_of_vector(
# 972 "wave.c"
 a7
# 972 "wave.c" 3
 ,
# 972 "wave.c"
 x14
# 972 "wave.c" 3
 ); if ((long)
# 972 "wave.c"
 x13 
# 972 "wave.c" 3
 == 0) goto 
# 972 "wave.c"
 pt_16_54
# 972 "wave.c" 3
 ; }
# 972 "wave.c"
                                            ;
 pt_16_39:
  switch (
# 974 "wave.c" 3
         ((unsigned long)(
# 974 "wave.c"
         a8
# 974 "wave.c" 3
         ) & 0x3)
# 974 "wave.c"
                   ) {
 case 
# 975 "wave.c" 3
     0x3
# 975 "wave.c"
            :
 pt_16_40:
  if (!
# 977 "wave.c" 3
      ((((unsigned long)((((struct functor *)((unsigned long)(
# 977 "wave.c"
      a8
# 977 "wave.c" 3
      ) - 0x3))->functor)) & 0x3) == 0x0))
# 977 "wave.c"
                ) goto pt_16_54;
  if (!
# 978 "wave.c" 3
      ((struct data_object_method_table *)((((struct functor *)((unsigned long)(
# 978 "wave.c"
      a8
# 978 "wave.c" 3
      ) - 0x3))->functor)) == &
# 978 "wave.c"
      vector_g_data_method_table
# 978 "wave.c" 3
      )
# 978 "wave.c"
                        ) goto pt_16_54;
  
# 979 "wave.c" 3
 { extern q size_of_vector(); 
# 979 "wave.c"
 x15 
# 979 "wave.c" 3
 = size_of_vector(
# 979 "wave.c"
 a8
# 979 "wave.c" 3
 ); }
# 979 "wave.c"
                                     ;
 pt_16_41:
  if (x15 != 
# 981 "wave.c" 3
            ((q)(((long)(
# 981 "wave.c"
            2L
# 981 "wave.c" 3
            ) << 4) + (0x2 +0x0)))
# 981 "wave.c"
                       ) goto pt_16_54;
  x17 = 
# 982 "wave.c" 3
       ((q)(((long)(
# 982 "wave.c"
       0L
# 982 "wave.c" 3
       ) << 4) + (0x2 +0x0)))
# 982 "wave.c"
                  ;
  
# 983 "wave.c" 3
 { extern q element_of_vector(); 
# 983 "wave.c"
 x16 
# 983 "wave.c" 3
 = element_of_vector(
# 983 "wave.c"
 a8
# 983 "wave.c" 3
 ,
# 983 "wave.c"
 x17
# 983 "wave.c" 3
 ); if ((long)
# 983 "wave.c"
 x16 
# 983 "wave.c" 3
 == 0) goto 
# 983 "wave.c"
 pt_16_54
# 983 "wave.c" 3
 ; }
# 983 "wave.c"
                                            ;
  x19 = 
# 984 "wave.c" 3
       ((q)(((long)(
# 984 "wave.c"
       1L
# 984 "wave.c" 3
       ) << 4) + (0x2 +0x0)))
# 984 "wave.c"
                  ;
  
# 985 "wave.c" 3
 { extern q element_of_vector(); 
# 985 "wave.c"
 x18 
# 985 "wave.c" 3
 = element_of_vector(
# 985 "wave.c"
 a8
# 985 "wave.c" 3
 ,
# 985 "wave.c"
 x19
# 985 "wave.c" 3
 ); if ((long)
# 985 "wave.c"
 x18 
# 985 "wave.c" 3
 == 0) goto 
# 985 "wave.c"
 pt_16_54
# 985 "wave.c" 3
 ; }
# 985 "wave.c"
                                            ;
 pt_16_42:
  switch (
# 987 "wave.c" 3
         ((unsigned long)(
# 987 "wave.c"
         a9
# 987 "wave.c" 3
         ) & 0x3)
# 987 "wave.c"
                   ) {
 case 
# 988 "wave.c" 3
     0x3
# 988 "wave.c"
            :
 pt_16_43:
  if (!
# 990 "wave.c" 3
      ((((unsigned long)((((struct functor *)((unsigned long)(
# 990 "wave.c"
      a9
# 990 "wave.c" 3
      ) - 0x3))->functor)) & 0x3) == 0x0))
# 990 "wave.c"
                ) goto pt_16_54;
  if (!
# 991 "wave.c" 3
      ((struct data_object_method_table *)((((struct functor *)((unsigned long)(
# 991 "wave.c"
      a9
# 991 "wave.c" 3
      ) - 0x3))->functor)) == &
# 991 "wave.c"
      vector_g_data_method_table
# 991 "wave.c" 3
      )
# 991 "wave.c"
                        ) goto pt_16_54;
  
# 992 "wave.c" 3
 { extern q size_of_vector(); 
# 992 "wave.c"
 x20 
# 992 "wave.c" 3
 = size_of_vector(
# 992 "wave.c"
 a9
# 992 "wave.c" 3
 ); }
# 992 "wave.c"
                                     ;
 pt_16_44:
  if (x20 != 
# 994 "wave.c" 3
            ((q)(((long)(
# 994 "wave.c"
            2L
# 994 "wave.c" 3
            ) << 4) + (0x2 +0x0)))
# 994 "wave.c"
                       ) goto pt_16_54;
  x22 = 
# 995 "wave.c" 3
       ((q)(((long)(
# 995 "wave.c"
       0L
# 995 "wave.c" 3
       ) << 4) + (0x2 +0x0)))
# 995 "wave.c"
                  ;
  
# 996 "wave.c" 3
 { extern q element_of_vector(); 
# 996 "wave.c"
 x21 
# 996 "wave.c" 3
 = element_of_vector(
# 996 "wave.c"
 a9
# 996 "wave.c" 3
 ,
# 996 "wave.c"
 x22
# 996 "wave.c" 3
 ); if ((long)
# 996 "wave.c"
 x21 
# 996 "wave.c" 3
 == 0) goto 
# 996 "wave.c"
 pt_16_54
# 996 "wave.c" 3
 ; }
# 996 "wave.c"
                                            ;
  x24 = 
# 997 "wave.c" 3
       ((q)(((long)(
# 997 "wave.c"
       1L
# 997 "wave.c" 3
       ) << 4) + (0x2 +0x0)))
# 997 "wave.c"
                  ;
  
# 998 "wave.c" 3
 { extern q element_of_vector(); 
# 998 "wave.c"
 x23 
# 998 "wave.c" 3
 = element_of_vector(
# 998 "wave.c"
 a9
# 998 "wave.c" 3
 ,
# 998 "wave.c"
 x24
# 998 "wave.c" 3
 ); if ((long)
# 998 "wave.c"
 x23 
# 998 "wave.c" 3
 == 0) goto 
# 998 "wave.c"
 pt_16_54
# 998 "wave.c" 3
 ; }
# 998 "wave.c"
                                            ;
 pt_16_45:
  switch (
# 1000 "wave.c" 3
         ((unsigned long)(
# 1000 "wave.c"
         a10
# 1000 "wave.c" 3
         ) & 0x3)
# 1000 "wave.c"
                    ) {
 case 
# 1001 "wave.c" 3
     0x3
# 1001 "wave.c"
            :
 pt_16_46:
  if (!
# 1003 "wave.c" 3
      ((((unsigned long)((((struct functor *)((unsigned long)(
# 1003 "wave.c"
      a10
# 1003 "wave.c" 3
      ) - 0x3))->functor)) & 0x3) == 0x0))
# 1003 "wave.c"
                 ) goto pt_16_54;
  if (!
# 1004 "wave.c" 3
      ((struct data_object_method_table *)((((struct functor *)((unsigned long)(
# 1004 "wave.c"
      a10
# 1004 "wave.c" 3
      ) - 0x3))->functor)) == &
# 1004 "wave.c"
      vector_g_data_method_table
# 1004 "wave.c" 3
      )
# 1004 "wave.c"
                         ) goto pt_16_54;
  
# 1005 "wave.c" 3
 { extern q size_of_vector(); 
# 1005 "wave.c"
 x25 
# 1005 "wave.c" 3
 = size_of_vector(
# 1005 "wave.c"
 a10
# 1005 "wave.c" 3
 ); }
# 1005 "wave.c"
                                      ;
 pt_16_47:
  if (x25 != 
# 1007 "wave.c" 3
            ((q)(((long)(
# 1007 "wave.c"
            2L
# 1007 "wave.c" 3
            ) << 4) + (0x2 +0x0)))
# 1007 "wave.c"
                       ) goto pt_16_54;
  x27 = 
# 1008 "wave.c" 3
       ((q)(((long)(
# 1008 "wave.c"
       0L
# 1008 "wave.c" 3
       ) << 4) + (0x2 +0x0)))
# 1008 "wave.c"
                  ;
  
# 1009 "wave.c" 3
 { extern q element_of_vector(); 
# 1009 "wave.c"
 x26 
# 1009 "wave.c" 3
 = element_of_vector(
# 1009 "wave.c"
 a10
# 1009 "wave.c" 3
 ,
# 1009 "wave.c"
 x27
# 1009 "wave.c" 3
 ); if ((long)
# 1009 "wave.c"
 x26 
# 1009 "wave.c" 3
 == 0) goto 
# 1009 "wave.c"
 pt_16_54
# 1009 "wave.c" 3
 ; }
# 1009 "wave.c"
                                             ;
  x29 = 
# 1010 "wave.c" 3
       ((q)(((long)(
# 1010 "wave.c"
       1L
# 1010 "wave.c" 3
       ) << 4) + (0x2 +0x0)))
# 1010 "wave.c"
                  ;
  
# 1011 "wave.c" 3
 { extern q element_of_vector(); 
# 1011 "wave.c"
 x28 
# 1011 "wave.c" 3
 = element_of_vector(
# 1011 "wave.c"
 a10
# 1011 "wave.c" 3
 ,
# 1011 "wave.c"
 x29
# 1011 "wave.c" 3
 ); if ((long)
# 1011 "wave.c"
 x28 
# 1011 "wave.c" 3
 == 0) goto 
# 1011 "wave.c"
 pt_16_54
# 1011 "wave.c" 3
 ; }
# 1011 "wave.c"
                                             ;
 pt_16_48:
  if (!
# 1013 "wave.c" 3
      (((unsigned long)(
# 1013 "wave.c"
      a1
# 1013 "wave.c" 3
      ) & 0xFL) == (0x2 +0x0))
# 1013 "wave.c"
               ) goto pt_16_53;
  x30 = 
# 1014 "wave.c" 3
       ((q)(((long)(
# 1014 "wave.c"
       1L
# 1014 "wave.c" 3
       ) << 4) + (0x2 +0x0)))
# 1014 "wave.c"
                  ;
  
# 1015 "wave.c" 3
 if ((long)(
# 1015 "wave.c"
 x30
# 1015 "wave.c" 3
 ) > (long)(
# 1015 "wave.c"
 a1
# 1015 "wave.c" 3
 )) goto 
# 1015 "wave.c"
 pt_16_54
# 1015 "wave.c" 3
 ;
# 1015 "wave.c"
                                 ;
 pt_16_49:
  if (!
# 1017 "wave.c" 3
      (((unsigned long)(
# 1017 "wave.c"
      a3
# 1017 "wave.c" 3
      ) & 0xFL) == (0x2 +0x0))
# 1017 "wave.c"
               ) goto pt_16_52;
  
# 1018 "wave.c" 3
 if ((long)(
# 1018 "wave.c"
 a1
# 1018 "wave.c" 3
 ) > (long)(
# 1018 "wave.c"
 a3
# 1018 "wave.c" 3
 )) goto 
# 1018 "wave.c"
 pt_16_54
# 1018 "wave.c" 3
 ;
# 1018 "wave.c"
                                ;
 pt_16_50:
  if (!
# 1020 "wave.c" 3
      (((unsigned long)(
# 1020 "wave.c"
      a2
# 1020 "wave.c" 3
      ) & 0xFL) == (0x2 +0x0))
# 1020 "wave.c"
               ) goto pt_16_51;
  x31 = 
# 1021 "wave.c" 3
       ((q)(((long)(
# 1021 "wave.c"
       1L
# 1021 "wave.c" 3
       ) << 4) + (0x2 +0x0)))
# 1021 "wave.c"
                  ;
  
# 1022 "wave.c" 3
 if ((long)(
# 1022 "wave.c"
 x31
# 1022 "wave.c" 3
 ) > (long)(
# 1022 "wave.c"
 a2
# 1022 "wave.c" 3
 )) goto 
# 1022 "wave.c"
 pt_16_54
# 1022 "wave.c" 3
 ;
# 1022 "wave.c"
                                 ;
  
# 1023 "wave.c" 3
 if ((long)(
# 1023 "wave.c"
 a2
# 1023 "wave.c" 3
 ) > (long)(
# 1023 "wave.c"
 a3
# 1023 "wave.c" 3
 )) goto 
# 1023 "wave.c"
 pt_16_54
# 1023 "wave.c" 3
 ;
# 1023 "wave.c"
                                ;
  allocp[0] = 
# 1024 "wave.c" 3
             ((q)(((unsigned long)(
# 1024 "wave.c"
             190L+
# 1024 "wave.c" 3
             0) << 4) + (0x2 +0x4)))
# 1024 "wave.c"
                                    ;
  allocp[1] = 
# 1025 "wave.c" 3
             ((q)(((long)(
# 1025 "wave.c"
             0L
# 1025 "wave.c" 3
             ) << 4) + (0x2 +0x0)))
# 1025 "wave.c"
                        ;
  allocp[2] = x33 = 
# 1026 "wave.c" 3
                   (q) ((unsigned long)(
# 1026 "wave.c"
                   &allocp[2]
# 1026 "wave.c" 3
                   ) + 0x0)
# 1026 "wave.c"
                                      ;
  allocp[3] = x34 = 
# 1027 "wave.c" 3
                   (q) ((unsigned long)(
# 1027 "wave.c"
                   &allocp[3]
# 1027 "wave.c" 3
                   ) + 0x0)
# 1027 "wave.c"
                                      ;
  x32 = 
# 1028 "wave.c" 3
       ((q)((unsigned long)(
# 1028 "wave.c"
       &allocp[0]
# 1028 "wave.c" 3
       ) + 0x3))
# 1028 "wave.c"
                              ;
  allocp[4] = 
# 1029 "wave.c" 3
             ((q)(((unsigned long)(
# 1029 "wave.c"
             190L+
# 1029 "wave.c" 3
             0) << 4) + (0x2 +0x4)))
# 1029 "wave.c"
                                    ;
  allocp[5] = 
# 1030 "wave.c" 3
             ((q)(((long)(
# 1030 "wave.c"
             1L
# 1030 "wave.c" 3
             ) << 4) + (0x2 +0x0)))
# 1030 "wave.c"
                        ;
  allocp[6] = x36 = 
# 1031 "wave.c" 3
                   (q) ((unsigned long)(
# 1031 "wave.c"
                   &allocp[6]
# 1031 "wave.c" 3
                   ) + 0x0)
# 1031 "wave.c"
                                      ;
  allocp[7] = x37 = 
# 1032 "wave.c" 3
                   (q) ((unsigned long)(
# 1032 "wave.c"
                   &allocp[7]
# 1032 "wave.c" 3
                   ) + 0x0)
# 1032 "wave.c"
                                      ;
  x35 = 
# 1033 "wave.c" 3
       ((q)((unsigned long)(
# 1033 "wave.c"
       &allocp[4]
# 1033 "wave.c" 3
       ) + 0x3))
# 1033 "wave.c"
                              ;
  allocp[8] = x39 = 
# 1034 "wave.c" 3
                   (q) ((unsigned long)(
# 1034 "wave.c"
                   &allocp[8]
# 1034 "wave.c" 3
                   ) + 0x0)
# 1034 "wave.c"
                                      ;
  allocp[9] = x35;
  x38 = 
# 1036 "wave.c" 3
       ((q)((unsigned long)(
# 1036 "wave.c"
       &allocp[8]
# 1036 "wave.c" 3
       ) + 0x1))
# 1036 "wave.c"
                           ;
  allocp[10] = x38;
  allocp[11] = x32;
  x40 = 
# 1039 "wave.c" 3
       ((q)((unsigned long)(
# 1039 "wave.c"
       &allocp[10]
# 1039 "wave.c" 3
       ) + 0x1))
# 1039 "wave.c"
                            ;
  allocp += 12;
  
# 1041 "wave.c" 3
 { if (!(((unsigned long)(
# 1041 "wave.c"
 x8
# 1041 "wave.c" 3
 ) & 0x3) == 0x0) || (*((q *)((unsigned long)(
# 1041 "wave.c"
 x8
# 1041 "wave.c" 3
 ) - 0x0))) != (
# 1041 "wave.c"
 x8
# 1041 "wave.c" 3
 )) { allocp = do_unify_value(allocp, (
# 1041 "wave.c"
 x8
# 1041 "wave.c" 3
 ), (
# 1041 "wave.c"
 x40
# 1041 "wave.c" 3
 )); } else { (*((q *)((unsigned long)(
# 1041 "wave.c"
 x8
# 1041 "wave.c" 3
 ) - 0x0))) = (
# 1041 "wave.c"
 x40
# 1041 "wave.c" 3
 ); } }
# 1041 "wave.c"
                     ;
  allocp[0] = 
# 1042 "wave.c" 3
             ((q)(((unsigned long)(0) << 4) + (0x2 +0x4)))
# 1042 "wave.c"
                    ;
  allocp[1] = x36;
  x41 = 
# 1044 "wave.c" 3
       ((q)((unsigned long)(
# 1044 "wave.c"
       &allocp[0]
# 1044 "wave.c" 3
       ) + 0x1))
# 1044 "wave.c"
                           ;
  allocp[2] = x41;
  allocp[3] = x33;
  x42 = 
# 1047 "wave.c" 3
       ((q)((unsigned long)(
# 1047 "wave.c"
       &allocp[2]
# 1047 "wave.c" 3
       ) + 0x1))
# 1047 "wave.c"
                           ;
  allocp[4] = (q)qp;
  allocp[5] = (q)(&predicate_main_xnode_7);
  allocp[6] = 
# 1050 "wave.c" 3
             ((q)(((long)(
# 1050 "wave.c"
             1L
# 1050 "wave.c" 3
             ) << 4) + (0x2 +0x0)))
# 1050 "wave.c"
                        ;
  allocp[7] = a1;
  allocp[8] = a2;
  allocp[9] = x42;
  allocp[10] = x43 = 
# 1054 "wave.c" 3
                    (q) ((unsigned long)(
# 1054 "wave.c"
                    &allocp[10]
# 1054 "wave.c" 3
                    ) + 0x0)
# 1054 "wave.c"
                                        ;
  allocp[11] = a11;
  allocp[12] = a12;
  allocp[13] = (q)(struct goalrec*)&allocp[4];
  allocp[14] = (q)(&predicate_main_xroute_14);
  allocp[15] = a15;
  allocp[16] = x1;
  allocp[17] = x6;
  allocp[18] = x11;
  allocp[19] = x16;
  allocp[20] = x21;
  allocp[21] = x26;
  allocp[22] = x3;
  allocp[23] = x39;
  allocp[24] = x13;
  allocp[25] = x18;
  allocp[26] = x23;
  allocp[27] = x28;
  allocp[28] = x43;
  a0 = x34;
  a1 = x37;
  a2 = a13;
  a3 = a14;
  qp = (struct goalrec*)&allocp[13];
  allocp += 29;
  
# 1079 "wave.c" 3
 { if (allocp < (glbl->heaplimit0)) goto 
# 1079 "wave.c"
 acknowledged_4_clear_reason
# 1079 "wave.c" 3
 ;}
# 1079 "wave.c"
                                     ;
  goto acknowledged_4_ext_interrupt;
 pt_16_51:
  if (!
# 1082 "wave.c" 3
      (((unsigned long)(
# 1082 "wave.c"
      a2
# 1082 "wave.c" 3
      ) & 0x3) == 0x0)
# 1082 "wave.c"
               ) goto pt_16_54;
  
# 1083 "wave.c" 3
 { q temp0 = (*((q *)((unsigned long)(
# 1083 "wave.c"
 a2
# 1083 "wave.c" 3
 ) - 0x0))); if(!(((unsigned long)(temp0) & 0x3) == 0x0)) { (
# 1083 "wave.c"
 a2
# 1083 "wave.c" 3
 ) = temp0; goto 
# 1083 "wave.c"
 pt_16_50
# 1083 "wave.c" 3
 ; } }
# 1083 "wave.c"
                            ;
  *reasonp++ = a2;
  goto pt_16_54;
 pt_16_52:
  if (!
# 1087 "wave.c" 3
      (((unsigned long)(
# 1087 "wave.c"
      a3
# 1087 "wave.c" 3
      ) & 0x3) == 0x0)
# 1087 "wave.c"
               ) goto pt_16_54;
  
# 1088 "wave.c" 3
 { q temp0 = (*((q *)((unsigned long)(
# 1088 "wave.c"
 a3
# 1088 "wave.c" 3
 ) - 0x0))); if(!(((unsigned long)(temp0) & 0x3) == 0x0)) { (
# 1088 "wave.c"
 a3
# 1088 "wave.c" 3
 ) = temp0; goto 
# 1088 "wave.c"
 pt_16_49
# 1088 "wave.c" 3
 ; } }
# 1088 "wave.c"
                            ;
  *reasonp++ = a3;
  goto pt_16_54;
 pt_16_53:
  if (!
# 1092 "wave.c" 3
      (((unsigned long)(
# 1092 "wave.c"
      a1
# 1092 "wave.c" 3
      ) & 0x3) == 0x0)
# 1092 "wave.c"
               ) goto pt_16_54;
  
# 1093 "wave.c" 3
 { q temp0 = (*((q *)((unsigned long)(
# 1093 "wave.c"
 a1
# 1093 "wave.c" 3
 ) - 0x0))); if(!(((unsigned long)(temp0) & 0x3) == 0x0)) { (
# 1093 "wave.c"
 a1
# 1093 "wave.c" 3
 ) = temp0; goto 
# 1093 "wave.c"
 pt_16_48
# 1093 "wave.c" 3
 ; } }
# 1093 "wave.c"
                            ;
  *reasonp++ = a1;
  goto pt_16_54;
 case 
# 1096 "wave.c" 3
     0x0
# 1096 "wave.c"
           :
  
# 1097 "wave.c" 3
 { q temp0 = (*((q *)((unsigned long)(
# 1097 "wave.c"
 a10
# 1097 "wave.c" 3
 ) - 0x0))); if(!(((unsigned long)(temp0) & 0x3) == 0x0)) { (
# 1097 "wave.c"
 a10
# 1097 "wave.c" 3
 ) = temp0; goto 
# 1097 "wave.c"
 pt_16_45
# 1097 "wave.c" 3
 ; } }
# 1097 "wave.c"
                             ;
  *reasonp++ = a10;
  goto pt_16_54;
  };
  goto pt_16_54;
 case 
# 1102 "wave.c" 3
     0x0
# 1102 "wave.c"
           :
  
# 1103 "wave.c" 3
 { q temp0 = (*((q *)((unsigned long)(
# 1103 "wave.c"
 a9
# 1103 "wave.c" 3
 ) - 0x0))); if(!(((unsigned long)(temp0) & 0x3) == 0x0)) { (
# 1103 "wave.c"
 a9
# 1103 "wave.c" 3
 ) = temp0; goto 
# 1103 "wave.c"
 pt_16_42
# 1103 "wave.c" 3
 ; } }
# 1103 "wave.c"
                            ;
  *reasonp++ = a9;
  goto pt_16_54;
  };
  goto pt_16_54;
 case 
# 1108 "wave.c" 3
     0x0
# 1108 "wave.c"
           :
  
# 1109 "wave.c" 3
 { q temp0 = (*((q *)((unsigned long)(
# 1109 "wave.c"
 a8
# 1109 "wave.c" 3
 ) - 0x0))); if(!(((unsigned long)(temp0) & 0x3) == 0x0)) { (
# 1109 "wave.c"
 a8
# 1109 "wave.c" 3
 ) = temp0; goto 
# 1109 "wave.c"
 pt_16_39
# 1109 "wave.c" 3
 ; } }
# 1109 "wave.c"
                            ;
  *reasonp++ = a8;
  goto pt_16_54;
  };
  goto pt_16_54;
 case 
# 1114 "wave.c" 3
     0x0
# 1114 "wave.c"
           :
  
# 1115 "wave.c" 3
 { q temp0 = (*((q *)((unsigned long)(
# 1115 "wave.c"
 a7
# 1115 "wave.c" 3
 ) - 0x0))); if(!(((unsigned long)(temp0) & 0x3) == 0x0)) { (
# 1115 "wave.c"
 a7
# 1115 "wave.c" 3
 ) = temp0; goto 
# 1115 "wave.c"
 pt_16_36
# 1115 "wave.c" 3
 ; } }
# 1115 "wave.c"
                            ;
  *reasonp++ = a7;
  goto pt_16_54;
  };
  goto pt_16_54;
 case 
# 1120 "wave.c" 3
     0x0
# 1120 "wave.c"
           :
  
# 1121 "wave.c" 3
 { q temp0 = (*((q *)((unsigned long)(
# 1121 "wave.c"
 a6
# 1121 "wave.c" 3
 ) - 0x0))); if(!(((unsigned long)(temp0) & 0x3) == 0x0)) { (
# 1121 "wave.c"
 a6
# 1121 "wave.c" 3
 ) = temp0; goto 
# 1121 "wave.c"
 pt_16_33
# 1121 "wave.c" 3
 ; } }
# 1121 "wave.c"
                            ;
  *reasonp++ = a6;
  goto pt_16_54;
  };
  goto pt_16_54;
 case 
# 1126 "wave.c" 3
     0x0
# 1126 "wave.c"
           :
  
# 1127 "wave.c" 3
 { q temp0 = (*((q *)((unsigned long)(
# 1127 "wave.c"
 a5
# 1127 "wave.c" 3
 ) - 0x0))); if(!(((unsigned long)(temp0) & 0x3) == 0x0)) { (
# 1127 "wave.c"
 a5
# 1127 "wave.c" 3
 ) = temp0; goto 
# 1127 "wave.c"
 pt_16_30
# 1127 "wave.c" 3
 ; } }
# 1127 "wave.c"
                            ;
  *reasonp++ = a5;
  goto pt_16_54;
  };
  goto pt_16_54;
 case 
# 1132 "wave.c" 3
     0x0
# 1132 "wave.c"
           :
  
# 1133 "wave.c" 3
 { q temp0 = (*((q *)((unsigned long)(
# 1133 "wave.c"
 a0
# 1133 "wave.c" 3
 ) - 0x0))); if(!(((unsigned long)(temp0) & 0x3) == 0x0)) { (
# 1133 "wave.c"
 a0
# 1133 "wave.c" 3
 ) = temp0; goto 
# 1133 "wave.c"
 pt_16_28
# 1133 "wave.c" 3
 ; } }
# 1133 "wave.c"
                            ;
  *reasonp++ = a0;
  goto pt_16_81;
  };
  goto pt_16_54;
 pt_16_54:
 pt_16_55:
  if (
# 1140 "wave.c" 3
     (((unsigned long)(
# 1140 "wave.c"
     a0
# 1140 "wave.c" 3
     ) & 0x3) == 0x0)
# 1140 "wave.c"
              ) goto pt_16_80;
  
# 1141 "wave.c" 3
 if ((
# 1141 "wave.c"
 a0
# 1141 "wave.c" 3
 )!=(
# 1141 "wave.c"
 a3
# 1141 "wave.c" 3
 ) || (((unsigned long)(
# 1141 "wave.c"
 a0
# 1141 "wave.c" 3
 ) & 0x3) == 0x0)) { q retval; switch((long)(retval = eq_terms_body((
# 1141 "wave.c"
 a0
# 1141 "wave.c" 3
 ), (
# 1141 "wave.c"
 a3
# 1141 "wave.c" 3
 )))){ case 0: break; default: *reasonp++ = retval; case 1: goto 
# 1141 "wave.c"
 pt_16_81
# 1141 "wave.c" 3
 ; } }
# 1141 "wave.c"
                               ;
 pt_16_56:
  switch (
# 1143 "wave.c" 3
         ((unsigned long)(
# 1143 "wave.c"
         a5
# 1143 "wave.c" 3
         ) & 0x3)
# 1143 "wave.c"
                   ) {
 case 
# 1144 "wave.c" 3
     0x3
# 1144 "wave.c"
            :
 pt_16_57:
  if (!
# 1146 "wave.c" 3
      ((((unsigned long)((((struct functor *)((unsigned long)(
# 1146 "wave.c"
      a5
# 1146 "wave.c" 3
      ) - 0x3))->functor)) & 0x3) == 0x0))
# 1146 "wave.c"
                ) goto pt_16_81;
  if (!
# 1147 "wave.c" 3
      ((struct data_object_method_table *)((((struct functor *)((unsigned long)(
# 1147 "wave.c"
      a5
# 1147 "wave.c" 3
      ) - 0x3))->functor)) == &
# 1147 "wave.c"
      vector_g_data_method_table
# 1147 "wave.c" 3
      )
# 1147 "wave.c"
                        ) goto pt_16_81;
  
# 1148 "wave.c" 3
 { extern q size_of_vector(); 
# 1148 "wave.c"
 x0 
# 1148 "wave.c" 3
 = size_of_vector(
# 1148 "wave.c"
 a5
# 1148 "wave.c" 3
 ); }
# 1148 "wave.c"
                                    ;
 pt_16_58:
  if (x0 != 
# 1150 "wave.c" 3
           ((q)(((long)(
# 1150 "wave.c"
           2L
# 1150 "wave.c" 3
           ) << 4) + (0x2 +0x0)))
# 1150 "wave.c"
                      ) goto pt_16_81;
  x2 = 
# 1151 "wave.c" 3
      ((q)(((long)(
# 1151 "wave.c"
      0L
# 1151 "wave.c" 3
      ) << 4) + (0x2 +0x0)))
# 1151 "wave.c"
                 ;
  
# 1152 "wave.c" 3
 { extern q element_of_vector(); 
# 1152 "wave.c"
 x1 
# 1152 "wave.c" 3
 = element_of_vector(
# 1152 "wave.c"
 a5
# 1152 "wave.c" 3
 ,
# 1152 "wave.c"
 x2
# 1152 "wave.c" 3
 ); if ((long)
# 1152 "wave.c"
 x1 
# 1152 "wave.c" 3
 == 0) goto 
# 1152 "wave.c"
 pt_16_81
# 1152 "wave.c" 3
 ; }
# 1152 "wave.c"
                                          ;
  x4 = 
# 1153 "wave.c" 3
      ((q)(((long)(
# 1153 "wave.c"
      1L
# 1153 "wave.c" 3
      ) << 4) + (0x2 +0x0)))
# 1153 "wave.c"
                 ;
  
# 1154 "wave.c" 3
 { extern q element_of_vector(); 
# 1154 "wave.c"
 x3 
# 1154 "wave.c" 3
 = element_of_vector(
# 1154 "wave.c"
 a5
# 1154 "wave.c" 3
 ,
# 1154 "wave.c"
 x4
# 1154 "wave.c" 3
 ); if ((long)
# 1154 "wave.c"
 x3 
# 1154 "wave.c" 3
 == 0) goto 
# 1154 "wave.c"
 pt_16_81
# 1154 "wave.c" 3
 ; }
# 1154 "wave.c"
                                          ;
 pt_16_59:
  switch (
# 1156 "wave.c" 3
         ((unsigned long)(
# 1156 "wave.c"
         a6
# 1156 "wave.c" 3
         ) & 0x3)
# 1156 "wave.c"
                   ) {
 case 
# 1157 "wave.c" 3
     0x3
# 1157 "wave.c"
            :
 pt_16_60:
  if (!
# 1159 "wave.c" 3
      ((((unsigned long)((((struct functor *)((unsigned long)(
# 1159 "wave.c"
      a6
# 1159 "wave.c" 3
      ) - 0x3))->functor)) & 0x3) == 0x0))
# 1159 "wave.c"
                ) goto pt_16_81;
  if (!
# 1160 "wave.c" 3
      ((struct data_object_method_table *)((((struct functor *)((unsigned long)(
# 1160 "wave.c"
      a6
# 1160 "wave.c" 3
      ) - 0x3))->functor)) == &
# 1160 "wave.c"
      vector_g_data_method_table
# 1160 "wave.c" 3
      )
# 1160 "wave.c"
                        ) goto pt_16_81;
  
# 1161 "wave.c" 3
 { extern q size_of_vector(); 
# 1161 "wave.c"
 x5 
# 1161 "wave.c" 3
 = size_of_vector(
# 1161 "wave.c"
 a6
# 1161 "wave.c" 3
 ); }
# 1161 "wave.c"
                                    ;
 pt_16_61:
  if (x5 != 
# 1163 "wave.c" 3
           ((q)(((long)(
# 1163 "wave.c"
           2L
# 1163 "wave.c" 3
           ) << 4) + (0x2 +0x0)))
# 1163 "wave.c"
                      ) goto pt_16_81;
  x7 = 
# 1164 "wave.c" 3
      ((q)(((long)(
# 1164 "wave.c"
      0L
# 1164 "wave.c" 3
      ) << 4) + (0x2 +0x0)))
# 1164 "wave.c"
                 ;
  
# 1165 "wave.c" 3
 { extern q element_of_vector(); 
# 1165 "wave.c"
 x6 
# 1165 "wave.c" 3
 = element_of_vector(
# 1165 "wave.c"
 a6
# 1165 "wave.c" 3
 ,
# 1165 "wave.c"
 x7
# 1165 "wave.c" 3
 ); if ((long)
# 1165 "wave.c"
 x6 
# 1165 "wave.c" 3
 == 0) goto 
# 1165 "wave.c"
 pt_16_81
# 1165 "wave.c" 3
 ; }
# 1165 "wave.c"
                                          ;
  x9 = 
# 1166 "wave.c" 3
      ((q)(((long)(
# 1166 "wave.c"
      1L
# 1166 "wave.c" 3
      ) << 4) + (0x2 +0x0)))
# 1166 "wave.c"
                 ;
  
# 1167 "wave.c" 3
 { extern q element_of_vector(); 
# 1167 "wave.c"
 x8 
# 1167 "wave.c" 3
 = element_of_vector(
# 1167 "wave.c"
 a6
# 1167 "wave.c" 3
 ,
# 1167 "wave.c"
 x9
# 1167 "wave.c" 3
 ); if ((long)
# 1167 "wave.c"
 x8 
# 1167 "wave.c" 3
 == 0) goto 
# 1167 "wave.c"
 pt_16_81
# 1167 "wave.c" 3
 ; }
# 1167 "wave.c"
                                          ;
 pt_16_62:
  switch (
# 1169 "wave.c" 3
         ((unsigned long)(
# 1169 "wave.c"
         a7
# 1169 "wave.c" 3
         ) & 0x3)
# 1169 "wave.c"
                   ) {
 case 
# 1170 "wave.c" 3
     0x3
# 1170 "wave.c"
            :
 pt_16_63:
  if (!
# 1172 "wave.c" 3
      ((((unsigned long)((((struct functor *)((unsigned long)(
# 1172 "wave.c"
      a7
# 1172 "wave.c" 3
      ) - 0x3))->functor)) & 0x3) == 0x0))
# 1172 "wave.c"
                ) goto pt_16_81;
  if (!
# 1173 "wave.c" 3
      ((struct data_object_method_table *)((((struct functor *)((unsigned long)(
# 1173 "wave.c"
      a7
# 1173 "wave.c" 3
      ) - 0x3))->functor)) == &
# 1173 "wave.c"
      vector_g_data_method_table
# 1173 "wave.c" 3
      )
# 1173 "wave.c"
                        ) goto pt_16_81;
  
# 1174 "wave.c" 3
 { extern q size_of_vector(); 
# 1174 "wave.c"
 x10 
# 1174 "wave.c" 3
 = size_of_vector(
# 1174 "wave.c"
 a7
# 1174 "wave.c" 3
 ); }
# 1174 "wave.c"
                                     ;
 pt_16_64:
  if (x10 != 
# 1176 "wave.c" 3
            ((q)(((long)(
# 1176 "wave.c"
            2L
# 1176 "wave.c" 3
            ) << 4) + (0x2 +0x0)))
# 1176 "wave.c"
                       ) goto pt_16_81;
  x12 = 
# 1177 "wave.c" 3
       ((q)(((long)(
# 1177 "wave.c"
       0L
# 1177 "wave.c" 3
       ) << 4) + (0x2 +0x0)))
# 1177 "wave.c"
                  ;
  
# 1178 "wave.c" 3
 { extern q element_of_vector(); 
# 1178 "wave.c"
 x11 
# 1178 "wave.c" 3
 = element_of_vector(
# 1178 "wave.c"
 a7
# 1178 "wave.c" 3
 ,
# 1178 "wave.c"
 x12
# 1178 "wave.c" 3
 ); if ((long)
# 1178 "wave.c"
 x11 
# 1178 "wave.c" 3
 == 0) goto 
# 1178 "wave.c"
 pt_16_81
# 1178 "wave.c" 3
 ; }
# 1178 "wave.c"
                                            ;
  x14 = 
# 1179 "wave.c" 3
       ((q)(((long)(
# 1179 "wave.c"
       1L
# 1179 "wave.c" 3
       ) << 4) + (0x2 +0x0)))
# 1179 "wave.c"
                  ;
  
# 1180 "wave.c" 3
 { extern q element_of_vector(); 
# 1180 "wave.c"
 x13 
# 1180 "wave.c" 3
 = element_of_vector(
# 1180 "wave.c"
 a7
# 1180 "wave.c" 3
 ,
# 1180 "wave.c"
 x14
# 1180 "wave.c" 3
 ); if ((long)
# 1180 "wave.c"
 x13 
# 1180 "wave.c" 3
 == 0) goto 
# 1180 "wave.c"
 pt_16_81
# 1180 "wave.c" 3
 ; }
# 1180 "wave.c"
                                            ;
 pt_16_65:
  switch (
# 1182 "wave.c" 3
         ((unsigned long)(
# 1182 "wave.c"
         a8
# 1182 "wave.c" 3
         ) & 0x3)
# 1182 "wave.c"
                   ) {
 case 
# 1183 "wave.c" 3
     0x3
# 1183 "wave.c"
            :
 pt_16_66:
  if (!
# 1185 "wave.c" 3
      ((((unsigned long)((((struct functor *)((unsigned long)(
# 1185 "wave.c"
      a8
# 1185 "wave.c" 3
      ) - 0x3))->functor)) & 0x3) == 0x0))
# 1185 "wave.c"
                ) goto pt_16_81;
  if (!
# 1186 "wave.c" 3
      ((struct data_object_method_table *)((((struct functor *)((unsigned long)(
# 1186 "wave.c"
      a8
# 1186 "wave.c" 3
      ) - 0x3))->functor)) == &
# 1186 "wave.c"
      vector_g_data_method_table
# 1186 "wave.c" 3
      )
# 1186 "wave.c"
                        ) goto pt_16_81;
  
# 1187 "wave.c" 3
 { extern q size_of_vector(); 
# 1187 "wave.c"
 x15 
# 1187 "wave.c" 3
 = size_of_vector(
# 1187 "wave.c"
 a8
# 1187 "wave.c" 3
 ); }
# 1187 "wave.c"
                                     ;
 pt_16_67:
  if (x15 != 
# 1189 "wave.c" 3
            ((q)(((long)(
# 1189 "wave.c"
            2L
# 1189 "wave.c" 3
            ) << 4) + (0x2 +0x0)))
# 1189 "wave.c"
                       ) goto pt_16_81;
  x17 = 
# 1190 "wave.c" 3
       ((q)(((long)(
# 1190 "wave.c"
       0L
# 1190 "wave.c" 3
       ) << 4) + (0x2 +0x0)))
# 1190 "wave.c"
                  ;
  
# 1191 "wave.c" 3
 { extern q element_of_vector(); 
# 1191 "wave.c"
 x16 
# 1191 "wave.c" 3
 = element_of_vector(
# 1191 "wave.c"
 a8
# 1191 "wave.c" 3
 ,
# 1191 "wave.c"
 x17
# 1191 "wave.c" 3
 ); if ((long)
# 1191 "wave.c"
 x16 
# 1191 "wave.c" 3
 == 0) goto 
# 1191 "wave.c"
 pt_16_81
# 1191 "wave.c" 3
 ; }
# 1191 "wave.c"
                                            ;
  x19 = 
# 1192 "wave.c" 3
       ((q)(((long)(
# 1192 "wave.c"
       1L
# 1192 "wave.c" 3
       ) << 4) + (0x2 +0x0)))
# 1192 "wave.c"
                  ;
  
# 1193 "wave.c" 3
 { extern q element_of_vector(); 
# 1193 "wave.c"
 x18 
# 1193 "wave.c" 3
 = element_of_vector(
# 1193 "wave.c"
 a8
# 1193 "wave.c" 3
 ,
# 1193 "wave.c"
 x19
# 1193 "wave.c" 3
 ); if ((long)
# 1193 "wave.c"
 x18 
# 1193 "wave.c" 3
 == 0) goto 
# 1193 "wave.c"
 pt_16_81
# 1193 "wave.c" 3
 ; }
# 1193 "wave.c"
                                            ;
 pt_16_68:
  switch (
# 1195 "wave.c" 3
         ((unsigned long)(
# 1195 "wave.c"
         a9
# 1195 "wave.c" 3
         ) & 0x3)
# 1195 "wave.c"
                   ) {
 case 
# 1196 "wave.c" 3
     0x3
# 1196 "wave.c"
            :
 pt_16_69:
  if (!
# 1198 "wave.c" 3
      ((((unsigned long)((((struct functor *)((unsigned long)(
# 1198 "wave.c"
      a9
# 1198 "wave.c" 3
      ) - 0x3))->functor)) & 0x3) == 0x0))
# 1198 "wave.c"
                ) goto pt_16_81;
  if (!
# 1199 "wave.c" 3
      ((struct data_object_method_table *)((((struct functor *)((unsigned long)(
# 1199 "wave.c"
      a9
# 1199 "wave.c" 3
      ) - 0x3))->functor)) == &
# 1199 "wave.c"
      vector_g_data_method_table
# 1199 "wave.c" 3
      )
# 1199 "wave.c"
                        ) goto pt_16_81;
  
# 1200 "wave.c" 3
 { extern q size_of_vector(); 
# 1200 "wave.c"
 x20 
# 1200 "wave.c" 3
 = size_of_vector(
# 1200 "wave.c"
 a9
# 1200 "wave.c" 3
 ); }
# 1200 "wave.c"
                                     ;
 pt_16_70:
  if (x20 != 
# 1202 "wave.c" 3
            ((q)(((long)(
# 1202 "wave.c"
            2L
# 1202 "wave.c" 3
            ) << 4) + (0x2 +0x0)))
# 1202 "wave.c"
                       ) goto pt_16_81;
  x22 = 
# 1203 "wave.c" 3
       ((q)(((long)(
# 1203 "wave.c"
       0L
# 1203 "wave.c" 3
       ) << 4) + (0x2 +0x0)))
# 1203 "wave.c"
                  ;
  
# 1204 "wave.c" 3
 { extern q element_of_vector(); 
# 1204 "wave.c"
 x21 
# 1204 "wave.c" 3
 = element_of_vector(
# 1204 "wave.c"
 a9
# 1204 "wave.c" 3
 ,
# 1204 "wave.c"
 x22
# 1204 "wave.c" 3
 ); if ((long)
# 1204 "wave.c"
 x21 
# 1204 "wave.c" 3
 == 0) goto 
# 1204 "wave.c"
 pt_16_81
# 1204 "wave.c" 3
 ; }
# 1204 "wave.c"
                                            ;
  x24 = 
# 1205 "wave.c" 3
       ((q)(((long)(
# 1205 "wave.c"
       1L
# 1205 "wave.c" 3
       ) << 4) + (0x2 +0x0)))
# 1205 "wave.c"
                  ;
  
# 1206 "wave.c" 3
 { extern q element_of_vector(); 
# 1206 "wave.c"
 x23 
# 1206 "wave.c" 3
 = element_of_vector(
# 1206 "wave.c"
 a9
# 1206 "wave.c" 3
 ,
# 1206 "wave.c"
 x24
# 1206 "wave.c" 3
 ); if ((long)
# 1206 "wave.c"
 x23 
# 1206 "wave.c" 3
 == 0) goto 
# 1206 "wave.c"
 pt_16_81
# 1206 "wave.c" 3
 ; }
# 1206 "wave.c"
                                            ;
 pt_16_71:
  switch (
# 1208 "wave.c" 3
         ((unsigned long)(
# 1208 "wave.c"
         a10
# 1208 "wave.c" 3
         ) & 0x3)
# 1208 "wave.c"
                    ) {
 case 
# 1209 "wave.c" 3
     0x3
# 1209 "wave.c"
            :
 pt_16_72:
  if (!
# 1211 "wave.c" 3
      ((((unsigned long)((((struct functor *)((unsigned long)(
# 1211 "wave.c"
      a10
# 1211 "wave.c" 3
      ) - 0x3))->functor)) & 0x3) == 0x0))
# 1211 "wave.c"
                 ) goto pt_16_81;
  if (!
# 1212 "wave.c" 3
      ((struct data_object_method_table *)((((struct functor *)((unsigned long)(
# 1212 "wave.c"
      a10
# 1212 "wave.c" 3
      ) - 0x3))->functor)) == &
# 1212 "wave.c"
      vector_g_data_method_table
# 1212 "wave.c" 3
      )
# 1212 "wave.c"
                         ) goto pt_16_81;
  
# 1213 "wave.c" 3
 { extern q size_of_vector(); 
# 1213 "wave.c"
 x25 
# 1213 "wave.c" 3
 = size_of_vector(
# 1213 "wave.c"
 a10
# 1213 "wave.c" 3
 ); }
# 1213 "wave.c"
                                      ;
 pt_16_73:
  if (x25 != 
# 1215 "wave.c" 3
            ((q)(((long)(
# 1215 "wave.c"
            2L
# 1215 "wave.c" 3
            ) << 4) + (0x2 +0x0)))
# 1215 "wave.c"
                       ) goto pt_16_81;
  x27 = 
# 1216 "wave.c" 3
       ((q)(((long)(
# 1216 "wave.c"
       0L
# 1216 "wave.c" 3
       ) << 4) + (0x2 +0x0)))
# 1216 "wave.c"
                  ;
  
# 1217 "wave.c" 3
 { extern q element_of_vector(); 
# 1217 "wave.c"
 x26 
# 1217 "wave.c" 3
 = element_of_vector(
# 1217 "wave.c"
 a10
# 1217 "wave.c" 3
 ,
# 1217 "wave.c"
 x27
# 1217 "wave.c" 3
 ); if ((long)
# 1217 "wave.c"
 x26 
# 1217 "wave.c" 3
 == 0) goto 
# 1217 "wave.c"
 pt_16_81
# 1217 "wave.c" 3
 ; }
# 1217 "wave.c"
                                             ;
  x29 = 
# 1218 "wave.c" 3
       ((q)(((long)(
# 1218 "wave.c"
       1L
# 1218 "wave.c" 3
       ) << 4) + (0x2 +0x0)))
# 1218 "wave.c"
                  ;
  
# 1219 "wave.c" 3
 { extern q element_of_vector(); 
# 1219 "wave.c"
 x28 
# 1219 "wave.c" 3
 = element_of_vector(
# 1219 "wave.c"
 a10
# 1219 "wave.c" 3
 ,
# 1219 "wave.c"
 x29
# 1219 "wave.c" 3
 ); if ((long)
# 1219 "wave.c"
 x28 
# 1219 "wave.c" 3
 == 0) goto 
# 1219 "wave.c"
 pt_16_81
# 1219 "wave.c" 3
 ; }
# 1219 "wave.c"
                                             ;
 pt_16_74:
  if (!
# 1221 "wave.c" 3
      (((unsigned long)(
# 1221 "wave.c"
      a1
# 1221 "wave.c" 3
      ) & 0xFL) == (0x2 +0x0))
# 1221 "wave.c"
               ) goto pt_16_79;
  x30 = 
# 1222 "wave.c" 3
       ((q)(((long)(
# 1222 "wave.c"
       1L
# 1222 "wave.c" 3
       ) << 4) + (0x2 +0x0)))
# 1222 "wave.c"
                  ;
  
# 1223 "wave.c" 3
 if ((long)(
# 1223 "wave.c"
 x30
# 1223 "wave.c" 3
 ) > (long)(
# 1223 "wave.c"
 a1
# 1223 "wave.c" 3
 )) goto 
# 1223 "wave.c"
 pt_16_81
# 1223 "wave.c" 3
 ;
# 1223 "wave.c"
                                 ;
 pt_16_75:
  if (!
# 1225 "wave.c" 3
      (((unsigned long)(
# 1225 "wave.c"
      a0
# 1225 "wave.c" 3
      ) & 0xFL) == (0x2 +0x0))
# 1225 "wave.c"
               ) goto pt_16_78;
  
# 1226 "wave.c" 3
 if ((long)(
# 1226 "wave.c"
 a1
# 1226 "wave.c" 3
 ) > (long)(
# 1226 "wave.c"
 a0
# 1226 "wave.c" 3
 )) goto 
# 1226 "wave.c"
 pt_16_81
# 1226 "wave.c" 3
 ;
# 1226 "wave.c"
                                ;
 pt_16_76:
  if (!
# 1228 "wave.c" 3
      (((unsigned long)(
# 1228 "wave.c"
      a2
# 1228 "wave.c" 3
      ) & 0xFL) == (0x2 +0x0))
# 1228 "wave.c"
               ) goto pt_16_77;
  x31 = 
# 1229 "wave.c" 3
       ((q)(((long)(
# 1229 "wave.c"
       1L
# 1229 "wave.c" 3
       ) << 4) + (0x2 +0x0)))
# 1229 "wave.c"
                  ;
  
# 1230 "wave.c" 3
 if ((long)(
# 1230 "wave.c"
 x31
# 1230 "wave.c" 3
 ) > (long)(
# 1230 "wave.c"
 a2
# 1230 "wave.c" 3
 )) goto 
# 1230 "wave.c"
 pt_16_81
# 1230 "wave.c" 3
 ;
# 1230 "wave.c"
                                 ;
  
# 1231 "wave.c" 3
 if ((long)(
# 1231 "wave.c"
 a2
# 1231 "wave.c" 3
 ) > (long)(
# 1231 "wave.c"
 a0
# 1231 "wave.c" 3
 )) goto 
# 1231 "wave.c"
 pt_16_81
# 1231 "wave.c" 3
 ;
# 1231 "wave.c"
                                ;
  allocp[0] = 
# 1232 "wave.c" 3
             ((q)(((unsigned long)(
# 1232 "wave.c"
             190L+
# 1232 "wave.c" 3
             0) << 4) + (0x2 +0x4)))
# 1232 "wave.c"
                                    ;
  allocp[1] = 
# 1233 "wave.c" 3
             ((q)(((long)(
# 1233 "wave.c"
             0L
# 1233 "wave.c" 3
             ) << 4) + (0x2 +0x0)))
# 1233 "wave.c"
                        ;
  allocp[2] = x33 = 
# 1234 "wave.c" 3
                   (q) ((unsigned long)(
# 1234 "wave.c"
                   &allocp[2]
# 1234 "wave.c" 3
                   ) + 0x0)
# 1234 "wave.c"
                                      ;
  allocp[3] = x34 = 
# 1235 "wave.c" 3
                   (q) ((unsigned long)(
# 1235 "wave.c"
                   &allocp[3]
# 1235 "wave.c" 3
                   ) + 0x0)
# 1235 "wave.c"
                                      ;
  x32 = 
# 1236 "wave.c" 3
       ((q)((unsigned long)(
# 1236 "wave.c"
       &allocp[0]
# 1236 "wave.c" 3
       ) + 0x3))
# 1236 "wave.c"
                              ;
  allocp[4] = 
# 1237 "wave.c" 3
             ((q)(((unsigned long)(
# 1237 "wave.c"
             190L+
# 1237 "wave.c" 3
             0) << 4) + (0x2 +0x4)))
# 1237 "wave.c"
                                    ;
  allocp[5] = 
# 1238 "wave.c" 3
             ((q)(((long)(
# 1238 "wave.c"
             1L
# 1238 "wave.c" 3
             ) << 4) + (0x2 +0x0)))
# 1238 "wave.c"
                        ;
  allocp[6] = x36 = 
# 1239 "wave.c" 3
                   (q) ((unsigned long)(
# 1239 "wave.c"
                   &allocp[6]
# 1239 "wave.c" 3
                   ) + 0x0)
# 1239 "wave.c"
                                      ;
  allocp[7] = x37 = 
# 1240 "wave.c" 3
                   (q) ((unsigned long)(
# 1240 "wave.c"
                   &allocp[7]
# 1240 "wave.c" 3
                   ) + 0x0)
# 1240 "wave.c"
                                      ;
  x35 = 
# 1241 "wave.c" 3
       ((q)((unsigned long)(
# 1241 "wave.c"
       &allocp[4]
# 1241 "wave.c" 3
       ) + 0x3))
# 1241 "wave.c"
                              ;
  allocp[8] = x39 = 
# 1242 "wave.c" 3
                   (q) ((unsigned long)(
# 1242 "wave.c"
                   &allocp[8]
# 1242 "wave.c" 3
                   ) + 0x0)
# 1242 "wave.c"
                                      ;
  allocp[9] = x35;
  x38 = 
# 1244 "wave.c" 3
       ((q)((unsigned long)(
# 1244 "wave.c"
       &allocp[8]
# 1244 "wave.c" 3
       ) + 0x1))
# 1244 "wave.c"
                           ;
  allocp[10] = x38;
  allocp[11] = x32;
  x40 = 
# 1247 "wave.c" 3
       ((q)((unsigned long)(
# 1247 "wave.c"
       &allocp[10]
# 1247 "wave.c" 3
       ) + 0x1))
# 1247 "wave.c"
                            ;
  allocp += 12;
  
# 1249 "wave.c" 3
 { if (!(((unsigned long)(
# 1249 "wave.c"
 x18
# 1249 "wave.c" 3
 ) & 0x3) == 0x0) || (*((q *)((unsigned long)(
# 1249 "wave.c"
 x18
# 1249 "wave.c" 3
 ) - 0x0))) != (
# 1249 "wave.c"
 x18
# 1249 "wave.c" 3
 )) { allocp = do_unify_value(allocp, (
# 1249 "wave.c"
 x18
# 1249 "wave.c" 3
 ), (
# 1249 "wave.c"
 x40
# 1249 "wave.c" 3
 )); } else { (*((q *)((unsigned long)(
# 1249 "wave.c"
 x18
# 1249 "wave.c" 3
 ) - 0x0))) = (
# 1249 "wave.c"
 x40
# 1249 "wave.c" 3
 ); } }
# 1249 "wave.c"
                      ;
  allocp[0] = 
# 1250 "wave.c" 3
             ((q)(((unsigned long)(0) << 4) + (0x2 +0x4)))
# 1250 "wave.c"
                    ;
  allocp[1] = x36;
  x41 = 
# 1252 "wave.c" 3
       ((q)((unsigned long)(
# 1252 "wave.c"
       &allocp[0]
# 1252 "wave.c" 3
       ) + 0x1))
# 1252 "wave.c"
                           ;
  allocp[2] = x41;
  allocp[3] = x33;
  x42 = 
# 1255 "wave.c" 3
       ((q)((unsigned long)(
# 1255 "wave.c"
       &allocp[2]
# 1255 "wave.c" 3
       ) + 0x1))
# 1255 "wave.c"
                           ;
  allocp[4] = (q)qp;
  allocp[5] = (q)(&predicate_main_xnode_7);
  allocp[6] = a0;
  allocp[7] = a1;
  allocp[8] = a2;
  allocp[9] = x42;
  allocp[10] = x43 = 
# 1262 "wave.c" 3
                    (q) ((unsigned long)(
# 1262 "wave.c"
                    &allocp[10]
# 1262 "wave.c" 3
                    ) + 0x0)
# 1262 "wave.c"
                                        ;
  allocp[11] = a11;
  allocp[12] = a12;
  allocp[13] = (q)(struct goalrec*)&allocp[4];
  allocp[14] = (q)(&predicate_main_xroute_14);
  allocp[15] = a15;
  allocp[16] = x1;
  allocp[17] = x6;
  allocp[18] = x11;
  allocp[19] = x16;
  allocp[20] = x21;
  allocp[21] = x26;
  allocp[22] = x3;
  allocp[23] = x8;
  allocp[24] = x13;
  allocp[25] = x39;
  allocp[26] = x23;
  allocp[27] = x28;
  allocp[28] = x43;
  a0 = x34;
  a1 = x37;
  a2 = a13;
  a3 = a14;
  qp = (struct goalrec*)&allocp[13];
  allocp += 29;
  
# 1287 "wave.c" 3
 { if (allocp < (glbl->heaplimit0)) goto 
# 1287 "wave.c"
 acknowledged_4_clear_reason
# 1287 "wave.c" 3
 ;}
# 1287 "wave.c"
                                     ;
  goto acknowledged_4_ext_interrupt;
 pt_16_77:
  if (!
# 1290 "wave.c" 3
      (((unsigned long)(
# 1290 "wave.c"
      a2
# 1290 "wave.c" 3
      ) & 0x3) == 0x0)
# 1290 "wave.c"
               ) goto pt_16_81;
  
# 1291 "wave.c" 3
 { q temp0 = (*((q *)((unsigned long)(
# 1291 "wave.c"
 a2
# 1291 "wave.c" 3
 ) - 0x0))); if(!(((unsigned long)(temp0) & 0x3) == 0x0)) { (
# 1291 "wave.c"
 a2
# 1291 "wave.c" 3
 ) = temp0; goto 
# 1291 "wave.c"
 pt_16_76
# 1291 "wave.c" 3
 ; } }
# 1291 "wave.c"
                            ;
  *reasonp++ = a2;
  goto pt_16_81;
 pt_16_78:
  if (!
# 1295 "wave.c" 3
      (((unsigned long)(
# 1295 "wave.c"
      a0
# 1295 "wave.c" 3
      ) & 0x3) == 0x0)
# 1295 "wave.c"
               ) goto pt_16_81;
  
# 1296 "wave.c" 3
 { q temp0 = (*((q *)((unsigned long)(
# 1296 "wave.c"
 a0
# 1296 "wave.c" 3
 ) - 0x0))); if(!(((unsigned long)(temp0) & 0x3) == 0x0)) { (
# 1296 "wave.c"
 a0
# 1296 "wave.c" 3
 ) = temp0; goto 
# 1296 "wave.c"
 pt_16_75
# 1296 "wave.c" 3
 ; } }
# 1296 "wave.c"
                            ;
  *reasonp++ = a0;
  goto pt_16_81;
 pt_16_79:
  if (!
# 1300 "wave.c" 3
      (((unsigned long)(
# 1300 "wave.c"
      a1
# 1300 "wave.c" 3
      ) & 0x3) == 0x0)
# 1300 "wave.c"
               ) goto pt_16_81;
  
# 1301 "wave.c" 3
 { q temp0 = (*((q *)((unsigned long)(
# 1301 "wave.c"
 a1
# 1301 "wave.c" 3
 ) - 0x0))); if(!(((unsigned long)(temp0) & 0x3) == 0x0)) { (
# 1301 "wave.c"
 a1
# 1301 "wave.c" 3
 ) = temp0; goto 
# 1301 "wave.c"
 pt_16_74
# 1301 "wave.c" 3
 ; } }
# 1301 "wave.c"
                            ;
  *reasonp++ = a1;
  goto pt_16_81;
 case 
# 1304 "wave.c" 3
     0x0
# 1304 "wave.c"
           :
  
# 1305 "wave.c" 3
 { q temp0 = (*((q *)((unsigned long)(
# 1305 "wave.c"
 a10
# 1305 "wave.c" 3
 ) - 0x0))); if(!(((unsigned long)(temp0) & 0x3) == 0x0)) { (
# 1305 "wave.c"
 a10
# 1305 "wave.c" 3
 ) = temp0; goto 
# 1305 "wave.c"
 pt_16_71
# 1305 "wave.c" 3
 ; } }
# 1305 "wave.c"
                             ;
  *reasonp++ = a10;
  goto pt_16_81;
  };
  goto pt_16_81;
 case 
# 1310 "wave.c" 3
     0x0
# 1310 "wave.c"
           :
  
# 1311 "wave.c" 3
 { q temp0 = (*((q *)((unsigned long)(
# 1311 "wave.c"
 a9
# 1311 "wave.c" 3
 ) - 0x0))); if(!(((unsigned long)(temp0) & 0x3) == 0x0)) { (
# 1311 "wave.c"
 a9
# 1311 "wave.c" 3
 ) = temp0; goto 
# 1311 "wave.c"
 pt_16_68
# 1311 "wave.c" 3
 ; } }
# 1311 "wave.c"
                            ;
  *reasonp++ = a9;
  goto pt_16_81;
  };
  goto pt_16_81;
 case 
# 1316 "wave.c" 3
     0x0
# 1316 "wave.c"
           :
  
# 1317 "wave.c" 3
 { q temp0 = (*((q *)((unsigned long)(
# 1317 "wave.c"
 a8
# 1317 "wave.c" 3
 ) - 0x0))); if(!(((unsigned long)(temp0) & 0x3) == 0x0)) { (
# 1317 "wave.c"
 a8
# 1317 "wave.c" 3
 ) = temp0; goto 
# 1317 "wave.c"
 pt_16_65
# 1317 "wave.c" 3
 ; } }
# 1317 "wave.c"
                            ;
  *reasonp++ = a8;
  goto pt_16_81;
  };
  goto pt_16_81;
 case 
# 1322 "wave.c" 3
     0x0
# 1322 "wave.c"
           :
  
# 1323 "wave.c" 3
 { q temp0 = (*((q *)((unsigned long)(
# 1323 "wave.c"
 a7
# 1323 "wave.c" 3
 ) - 0x0))); if(!(((unsigned long)(temp0) & 0x3) == 0x0)) { (
# 1323 "wave.c"
 a7
# 1323 "wave.c" 3
 ) = temp0; goto 
# 1323 "wave.c"
 pt_16_62
# 1323 "wave.c" 3
 ; } }
# 1323 "wave.c"
                            ;
  *reasonp++ = a7;
  goto pt_16_81;
  };
  goto pt_16_81;
 case 
# 1328 "wave.c" 3
     0x0
# 1328 "wave.c"
           :
  
# 1329 "wave.c" 3
 { q temp0 = (*((q *)((unsigned long)(
# 1329 "wave.c"
 a6
# 1329 "wave.c" 3
 ) - 0x0))); if(!(((unsigned long)(temp0) & 0x3) == 0x0)) { (
# 1329 "wave.c"
 a6
# 1329 "wave.c" 3
 ) = temp0; goto 
# 1329 "wave.c"
 pt_16_59
# 1329 "wave.c" 3
 ; } }
# 1329 "wave.c"
                            ;
  *reasonp++ = a6;
  goto pt_16_81;
  };
  goto pt_16_81;
 case 
# 1334 "wave.c" 3
     0x0
# 1334 "wave.c"
           :
  
# 1335 "wave.c" 3
 { q temp0 = (*((q *)((unsigned long)(
# 1335 "wave.c"
 a5
# 1335 "wave.c" 3
 ) - 0x0))); if(!(((unsigned long)(temp0) & 0x3) == 0x0)) { (
# 1335 "wave.c"
 a5
# 1335 "wave.c" 3
 ) = temp0; goto 
# 1335 "wave.c"
 pt_16_56
# 1335 "wave.c" 3
 ; } }
# 1335 "wave.c"
                            ;
  *reasonp++ = a5;
  goto pt_16_81;
  };
  goto pt_16_81;
 pt_16_80:
  
# 1341 "wave.c" 3
 { q temp0 = (*((q *)((unsigned long)(
# 1341 "wave.c"
 a0
# 1341 "wave.c" 3
 ) - 0x0))); if(!(((unsigned long)(temp0) & 0x3) == 0x0)) { (
# 1341 "wave.c"
 a0
# 1341 "wave.c" 3
 ) = temp0; goto 
# 1341 "wave.c"
 pt_16_55
# 1341 "wave.c" 3
 ; } }
# 1341 "wave.c"
                            ;
  *reasonp++ = a0;
  goto pt_16_81;
 pt_16_81:
 pt_16_82:
  switch (
# 1346 "wave.c" 3
         ((unsigned long)(
# 1346 "wave.c"
         a2
# 1346 "wave.c" 3
         ) & 0x3)
# 1346 "wave.c"
                   ) {
 case 
# 1347 "wave.c" 3
     0x2
# 1347 "wave.c"
           :
 pt_16_83:
  if (a2 != 
# 1349 "wave.c" 3
           ((q)(((long)(
# 1349 "wave.c"
           0L
# 1349 "wave.c" 3
           ) << 4) + (0x2 +0x0)))
# 1349 "wave.c"
                      ) goto pt_16_108;
 pt_16_84:
  switch (
# 1351 "wave.c" 3
         ((unsigned long)(
# 1351 "wave.c"
         a5
# 1351 "wave.c" 3
         ) & 0x3)
# 1351 "wave.c"
                   ) {
 case 
# 1352 "wave.c" 3
     0x3
# 1352 "wave.c"
            :
 pt_16_85:
  if (!
# 1354 "wave.c" 3
      ((((unsigned long)((((struct functor *)((unsigned long)(
# 1354 "wave.c"
      a5
# 1354 "wave.c" 3
      ) - 0x3))->functor)) & 0x3) == 0x0))
# 1354 "wave.c"
                ) goto pt_16_108;
  if (!
# 1355 "wave.c" 3
      ((struct data_object_method_table *)((((struct functor *)((unsigned long)(
# 1355 "wave.c"
      a5
# 1355 "wave.c" 3
      ) - 0x3))->functor)) == &
# 1355 "wave.c"
      vector_g_data_method_table
# 1355 "wave.c" 3
      )
# 1355 "wave.c"
                        ) goto pt_16_108;
  
# 1356 "wave.c" 3
 { extern q size_of_vector(); 
# 1356 "wave.c"
 x0 
# 1356 "wave.c" 3
 = size_of_vector(
# 1356 "wave.c"
 a5
# 1356 "wave.c" 3
 ); }
# 1356 "wave.c"
                                     ;
 pt_16_86:
  if (x0 != 
# 1358 "wave.c" 3
           ((q)(((long)(
# 1358 "wave.c"
           2L
# 1358 "wave.c" 3
           ) << 4) + (0x2 +0x0)))
# 1358 "wave.c"
                      ) goto pt_16_108;
  x2 = 
# 1359 "wave.c" 3
      ((q)(((long)(
# 1359 "wave.c"
      0L
# 1359 "wave.c" 3
      ) << 4) + (0x2 +0x0)))
# 1359 "wave.c"
                 ;
  
# 1360 "wave.c" 3
 { extern q element_of_vector(); 
# 1360 "wave.c"
 x1 
# 1360 "wave.c" 3
 = element_of_vector(
# 1360 "wave.c"
 a5
# 1360 "wave.c" 3
 ,
# 1360 "wave.c"
 x2
# 1360 "wave.c" 3
 ); if ((long)
# 1360 "wave.c"
 x1 
# 1360 "wave.c" 3
 == 0) goto 
# 1360 "wave.c"
 pt_16_108
# 1360 "wave.c" 3
 ; }
# 1360 "wave.c"
                                           ;
  x4 = 
# 1361 "wave.c" 3
      ((q)(((long)(
# 1361 "wave.c"
      1L
# 1361 "wave.c" 3
      ) << 4) + (0x2 +0x0)))
# 1361 "wave.c"
                 ;
  
# 1362 "wave.c" 3
 { extern q element_of_vector(); 
# 1362 "wave.c"
 x3 
# 1362 "wave.c" 3
 = element_of_vector(
# 1362 "wave.c"
 a5
# 1362 "wave.c" 3
 ,
# 1362 "wave.c"
 x4
# 1362 "wave.c" 3
 ); if ((long)
# 1362 "wave.c"
 x3 
# 1362 "wave.c" 3
 == 0) goto 
# 1362 "wave.c"
 pt_16_108
# 1362 "wave.c" 3
 ; }
# 1362 "wave.c"
                                           ;
 pt_16_87:
  switch (
# 1364 "wave.c" 3
         ((unsigned long)(
# 1364 "wave.c"
         a6
# 1364 "wave.c" 3
         ) & 0x3)
# 1364 "wave.c"
                   ) {
 case 
# 1365 "wave.c" 3
     0x3
# 1365 "wave.c"
            :
 pt_16_88:
  if (!
# 1367 "wave.c" 3
      ((((unsigned long)((((struct functor *)((unsigned long)(
# 1367 "wave.c"
      a6
# 1367 "wave.c" 3
      ) - 0x3))->functor)) & 0x3) == 0x0))
# 1367 "wave.c"
                ) goto pt_16_108;
  if (!
# 1368 "wave.c" 3
      ((struct data_object_method_table *)((((struct functor *)((unsigned long)(
# 1368 "wave.c"
      a6
# 1368 "wave.c" 3
      ) - 0x3))->functor)) == &
# 1368 "wave.c"
      vector_g_data_method_table
# 1368 "wave.c" 3
      )
# 1368 "wave.c"
                        ) goto pt_16_108;
  
# 1369 "wave.c" 3
 { extern q size_of_vector(); 
# 1369 "wave.c"
 x5 
# 1369 "wave.c" 3
 = size_of_vector(
# 1369 "wave.c"
 a6
# 1369 "wave.c" 3
 ); }
# 1369 "wave.c"
                                     ;
 pt_16_89:
  if (x5 != 
# 1371 "wave.c" 3
           ((q)(((long)(
# 1371 "wave.c"
           2L
# 1371 "wave.c" 3
           ) << 4) + (0x2 +0x0)))
# 1371 "wave.c"
                      ) goto pt_16_108;
  x7 = 
# 1372 "wave.c" 3
      ((q)(((long)(
# 1372 "wave.c"
      0L
# 1372 "wave.c" 3
      ) << 4) + (0x2 +0x0)))
# 1372 "wave.c"
                 ;
  
# 1373 "wave.c" 3
 { extern q element_of_vector(); 
# 1373 "wave.c"
 x6 
# 1373 "wave.c" 3
 = element_of_vector(
# 1373 "wave.c"
 a6
# 1373 "wave.c" 3
 ,
# 1373 "wave.c"
 x7
# 1373 "wave.c" 3
 ); if ((long)
# 1373 "wave.c"
 x6 
# 1373 "wave.c" 3
 == 0) goto 
# 1373 "wave.c"
 pt_16_108
# 1373 "wave.c" 3
 ; }
# 1373 "wave.c"
                                           ;
  x9 = 
# 1374 "wave.c" 3
      ((q)(((long)(
# 1374 "wave.c"
      1L
# 1374 "wave.c" 3
      ) << 4) + (0x2 +0x0)))
# 1374 "wave.c"
                 ;
  
# 1375 "wave.c" 3
 { extern q element_of_vector(); 
# 1375 "wave.c"
 x8 
# 1375 "wave.c" 3
 = element_of_vector(
# 1375 "wave.c"
 a6
# 1375 "wave.c" 3
 ,
# 1375 "wave.c"
 x9
# 1375 "wave.c" 3
 ); if ((long)
# 1375 "wave.c"
 x8 
# 1375 "wave.c" 3
 == 0) goto 
# 1375 "wave.c"
 pt_16_108
# 1375 "wave.c" 3
 ; }
# 1375 "wave.c"
                                           ;
 pt_16_90:
  switch (
# 1377 "wave.c" 3
         ((unsigned long)(
# 1377 "wave.c"
         a7
# 1377 "wave.c" 3
         ) & 0x3)
# 1377 "wave.c"
                   ) {
 case 
# 1378 "wave.c" 3
     0x3
# 1378 "wave.c"
            :
 pt_16_91:
  if (!
# 1380 "wave.c" 3
      ((((unsigned long)((((struct functor *)((unsigned long)(
# 1380 "wave.c"
      a7
# 1380 "wave.c" 3
      ) - 0x3))->functor)) & 0x3) == 0x0))
# 1380 "wave.c"
                ) goto pt_16_108;
  if (!
# 1381 "wave.c" 3
      ((struct data_object_method_table *)((((struct functor *)((unsigned long)(
# 1381 "wave.c"
      a7
# 1381 "wave.c" 3
      ) - 0x3))->functor)) == &
# 1381 "wave.c"
      vector_g_data_method_table
# 1381 "wave.c" 3
      )
# 1381 "wave.c"
                        ) goto pt_16_108;
  
# 1382 "wave.c" 3
 { extern q size_of_vector(); 
# 1382 "wave.c"
 x10 
# 1382 "wave.c" 3
 = size_of_vector(
# 1382 "wave.c"
 a7
# 1382 "wave.c" 3
 ); }
# 1382 "wave.c"
                                      ;
 pt_16_92:
  if (x10 != 
# 1384 "wave.c" 3
            ((q)(((long)(
# 1384 "wave.c"
            2L
# 1384 "wave.c" 3
            ) << 4) + (0x2 +0x0)))
# 1384 "wave.c"
                       ) goto pt_16_108;
  x12 = 
# 1385 "wave.c" 3
       ((q)(((long)(
# 1385 "wave.c"
       0L
# 1385 "wave.c" 3
       ) << 4) + (0x2 +0x0)))
# 1385 "wave.c"
                  ;
  
# 1386 "wave.c" 3
 { extern q element_of_vector(); 
# 1386 "wave.c"
 x11 
# 1386 "wave.c" 3
 = element_of_vector(
# 1386 "wave.c"
 a7
# 1386 "wave.c" 3
 ,
# 1386 "wave.c"
 x12
# 1386 "wave.c" 3
 ); if ((long)
# 1386 "wave.c"
 x11 
# 1386 "wave.c" 3
 == 0) goto 
# 1386 "wave.c"
 pt_16_108
# 1386 "wave.c" 3
 ; }
# 1386 "wave.c"
                                             ;
  x14 = 
# 1387 "wave.c" 3
       ((q)(((long)(
# 1387 "wave.c"
       1L
# 1387 "wave.c" 3
       ) << 4) + (0x2 +0x0)))
# 1387 "wave.c"
                  ;
  
# 1388 "wave.c" 3
 { extern q element_of_vector(); 
# 1388 "wave.c"
 x13 
# 1388 "wave.c" 3
 = element_of_vector(
# 1388 "wave.c"
 a7
# 1388 "wave.c" 3
 ,
# 1388 "wave.c"
 x14
# 1388 "wave.c" 3
 ); if ((long)
# 1388 "wave.c"
 x13 
# 1388 "wave.c" 3
 == 0) goto 
# 1388 "wave.c"
 pt_16_108
# 1388 "wave.c" 3
 ; }
# 1388 "wave.c"
                                             ;
 pt_16_93:
  switch (
# 1390 "wave.c" 3
         ((unsigned long)(
# 1390 "wave.c"
         a8
# 1390 "wave.c" 3
         ) & 0x3)
# 1390 "wave.c"
                   ) {
 case 
# 1391 "wave.c" 3
     0x3
# 1391 "wave.c"
            :
 pt_16_94:
  if (!
# 1393 "wave.c" 3
      ((((unsigned long)((((struct functor *)((unsigned long)(
# 1393 "wave.c"
      a8
# 1393 "wave.c" 3
      ) - 0x3))->functor)) & 0x3) == 0x0))
# 1393 "wave.c"
                ) goto pt_16_108;
  if (!
# 1394 "wave.c" 3
      ((struct data_object_method_table *)((((struct functor *)((unsigned long)(
# 1394 "wave.c"
      a8
# 1394 "wave.c" 3
      ) - 0x3))->functor)) == &
# 1394 "wave.c"
      vector_g_data_method_table
# 1394 "wave.c" 3
      )
# 1394 "wave.c"
                        ) goto pt_16_108;
  
# 1395 "wave.c" 3
 { extern q size_of_vector(); 
# 1395 "wave.c"
 x15 
# 1395 "wave.c" 3
 = size_of_vector(
# 1395 "wave.c"
 a8
# 1395 "wave.c" 3
 ); }
# 1395 "wave.c"
                                      ;
 pt_16_95:
  if (x15 != 
# 1397 "wave.c" 3
            ((q)(((long)(
# 1397 "wave.c"
            2L
# 1397 "wave.c" 3
            ) << 4) + (0x2 +0x0)))
# 1397 "wave.c"
                       ) goto pt_16_108;
  x17 = 
# 1398 "wave.c" 3
       ((q)(((long)(
# 1398 "wave.c"
       0L
# 1398 "wave.c" 3
       ) << 4) + (0x2 +0x0)))
# 1398 "wave.c"
                  ;
  
# 1399 "wave.c" 3
 { extern q element_of_vector(); 
# 1399 "wave.c"
 x16 
# 1399 "wave.c" 3
 = element_of_vector(
# 1399 "wave.c"
 a8
# 1399 "wave.c" 3
 ,
# 1399 "wave.c"
 x17
# 1399 "wave.c" 3
 ); if ((long)
# 1399 "wave.c"
 x16 
# 1399 "wave.c" 3
 == 0) goto 
# 1399 "wave.c"
 pt_16_108
# 1399 "wave.c" 3
 ; }
# 1399 "wave.c"
                                             ;
  x19 = 
# 1400 "wave.c" 3
       ((q)(((long)(
# 1400 "wave.c"
       1L
# 1400 "wave.c" 3
       ) << 4) + (0x2 +0x0)))
# 1400 "wave.c"
                  ;
  
# 1401 "wave.c" 3
 { extern q element_of_vector(); 
# 1401 "wave.c"
 x18 
# 1401 "wave.c" 3
 = element_of_vector(
# 1401 "wave.c"
 a8
# 1401 "wave.c" 3
 ,
# 1401 "wave.c"
 x19
# 1401 "wave.c" 3
 ); if ((long)
# 1401 "wave.c"
 x18 
# 1401 "wave.c" 3
 == 0) goto 
# 1401 "wave.c"
 pt_16_108
# 1401 "wave.c" 3
 ; }
# 1401 "wave.c"
                                             ;
 pt_16_96:
  switch (
# 1403 "wave.c" 3
         ((unsigned long)(
# 1403 "wave.c"
         a9
# 1403 "wave.c" 3
         ) & 0x3)
# 1403 "wave.c"
                   ) {
 case 
# 1404 "wave.c" 3
     0x3
# 1404 "wave.c"
            :
 pt_16_97:
  if (!
# 1406 "wave.c" 3
      ((((unsigned long)((((struct functor *)((unsigned long)(
# 1406 "wave.c"
      a9
# 1406 "wave.c" 3
      ) - 0x3))->functor)) & 0x3) == 0x0))
# 1406 "wave.c"
                ) goto pt_16_108;
  if (!
# 1407 "wave.c" 3
      ((struct data_object_method_table *)((((struct functor *)((unsigned long)(
# 1407 "wave.c"
      a9
# 1407 "wave.c" 3
      ) - 0x3))->functor)) == &
# 1407 "wave.c"
      vector_g_data_method_table
# 1407 "wave.c" 3
      )
# 1407 "wave.c"
                        ) goto pt_16_108;
  
# 1408 "wave.c" 3
 { extern q size_of_vector(); 
# 1408 "wave.c"
 x20 
# 1408 "wave.c" 3
 = size_of_vector(
# 1408 "wave.c"
 a9
# 1408 "wave.c" 3
 ); }
# 1408 "wave.c"
                                      ;
 pt_16_98:
  if (x20 != 
# 1410 "wave.c" 3
            ((q)(((long)(
# 1410 "wave.c"
            2L
# 1410 "wave.c" 3
            ) << 4) + (0x2 +0x0)))
# 1410 "wave.c"
                       ) goto pt_16_108;
  x22 = 
# 1411 "wave.c" 3
       ((q)(((long)(
# 1411 "wave.c"
       0L
# 1411 "wave.c" 3
       ) << 4) + (0x2 +0x0)))
# 1411 "wave.c"
                  ;
  
# 1412 "wave.c" 3
 { extern q element_of_vector(); 
# 1412 "wave.c"
 x21 
# 1412 "wave.c" 3
 = element_of_vector(
# 1412 "wave.c"
 a9
# 1412 "wave.c" 3
 ,
# 1412 "wave.c"
 x22
# 1412 "wave.c" 3
 ); if ((long)
# 1412 "wave.c"
 x21 
# 1412 "wave.c" 3
 == 0) goto 
# 1412 "wave.c"
 pt_16_108
# 1412 "wave.c" 3
 ; }
# 1412 "wave.c"
                                             ;
  x24 = 
# 1413 "wave.c" 3
       ((q)(((long)(
# 1413 "wave.c"
       1L
# 1413 "wave.c" 3
       ) << 4) + (0x2 +0x0)))
# 1413 "wave.c"
                  ;
  
# 1414 "wave.c" 3
 { extern q element_of_vector(); 
# 1414 "wave.c"
 x23 
# 1414 "wave.c" 3
 = element_of_vector(
# 1414 "wave.c"
 a9
# 1414 "wave.c" 3
 ,
# 1414 "wave.c"
 x24
# 1414 "wave.c" 3
 ); if ((long)
# 1414 "wave.c"
 x23 
# 1414 "wave.c" 3
 == 0) goto 
# 1414 "wave.c"
 pt_16_108
# 1414 "wave.c" 3
 ; }
# 1414 "wave.c"
                                             ;
 pt_16_99:
  switch (
# 1416 "wave.c" 3
         ((unsigned long)(
# 1416 "wave.c"
         a10
# 1416 "wave.c" 3
         ) & 0x3)
# 1416 "wave.c"
                    ) {
 case 
# 1417 "wave.c" 3
     0x3
# 1417 "wave.c"
            :
 pt_16_100:
  if (!
# 1419 "wave.c" 3
      ((((unsigned long)((((struct functor *)((unsigned long)(
# 1419 "wave.c"
      a10
# 1419 "wave.c" 3
      ) - 0x3))->functor)) & 0x3) == 0x0))
# 1419 "wave.c"
                 ) goto pt_16_108;
  if (!
# 1420 "wave.c" 3
      ((struct data_object_method_table *)((((struct functor *)((unsigned long)(
# 1420 "wave.c"
      a10
# 1420 "wave.c" 3
      ) - 0x3))->functor)) == &
# 1420 "wave.c"
      vector_g_data_method_table
# 1420 "wave.c" 3
      )
# 1420 "wave.c"
                         ) goto pt_16_108;
  
# 1421 "wave.c" 3
 { extern q size_of_vector(); 
# 1421 "wave.c"
 x25 
# 1421 "wave.c" 3
 = size_of_vector(
# 1421 "wave.c"
 a10
# 1421 "wave.c" 3
 ); }
# 1421 "wave.c"
                                       ;
 pt_16_101:
  if (x25 != 
# 1423 "wave.c" 3
            ((q)(((long)(
# 1423 "wave.c"
            2L
# 1423 "wave.c" 3
            ) << 4) + (0x2 +0x0)))
# 1423 "wave.c"
                       ) goto pt_16_108;
  x27 = 
# 1424 "wave.c" 3
       ((q)(((long)(
# 1424 "wave.c"
       0L
# 1424 "wave.c" 3
       ) << 4) + (0x2 +0x0)))
# 1424 "wave.c"
                  ;
  
# 1425 "wave.c" 3
 { extern q element_of_vector(); 
# 1425 "wave.c"
 x26 
# 1425 "wave.c" 3
 = element_of_vector(
# 1425 "wave.c"
 a10
# 1425 "wave.c" 3
 ,
# 1425 "wave.c"
 x27
# 1425 "wave.c" 3
 ); if ((long)
# 1425 "wave.c"
 x26 
# 1425 "wave.c" 3
 == 0) goto 
# 1425 "wave.c"
 pt_16_108
# 1425 "wave.c" 3
 ; }
# 1425 "wave.c"
                                              ;
  x29 = 
# 1426 "wave.c" 3
       ((q)(((long)(
# 1426 "wave.c"
       1L
# 1426 "wave.c" 3
       ) << 4) + (0x2 +0x0)))
# 1426 "wave.c"
                  ;
  
# 1427 "wave.c" 3
 { extern q element_of_vector(); 
# 1427 "wave.c"
 x28 
# 1427 "wave.c" 3
 = element_of_vector(
# 1427 "wave.c"
 a10
# 1427 "wave.c" 3
 ,
# 1427 "wave.c"
 x29
# 1427 "wave.c" 3
 ); if ((long)
# 1427 "wave.c"
 x28 
# 1427 "wave.c" 3
 == 0) goto 
# 1427 "wave.c"
 pt_16_108
# 1427 "wave.c" 3
 ; }
# 1427 "wave.c"
                                              ;
 pt_16_102:
  if (!
# 1429 "wave.c" 3
      (((unsigned long)(
# 1429 "wave.c"
      a0
# 1429 "wave.c" 3
      ) & 0xFL) == (0x2 +0x0))
# 1429 "wave.c"
               ) goto pt_16_107;
  x30 = 
# 1430 "wave.c" 3
       ((q)(((long)(
# 1430 "wave.c"
       1L
# 1430 "wave.c" 3
       ) << 4) + (0x2 +0x0)))
# 1430 "wave.c"
                  ;
  
# 1431 "wave.c" 3
 if ((long)(
# 1431 "wave.c"
 x30
# 1431 "wave.c" 3
 ) > (long)(
# 1431 "wave.c"
 a0
# 1431 "wave.c" 3
 )) goto 
# 1431 "wave.c"
 pt_16_108
# 1431 "wave.c" 3
 ;
# 1431 "wave.c"
                                  ;
 pt_16_103:
  if (!
# 1433 "wave.c" 3
      (((unsigned long)(
# 1433 "wave.c"
      a3
# 1433 "wave.c" 3
      ) & 0xFL) == (0x2 +0x0))
# 1433 "wave.c"
               ) goto pt_16_106;
  
# 1434 "wave.c" 3
 if ((long)(
# 1434 "wave.c"
 a0
# 1434 "wave.c" 3
 ) > (long)(
# 1434 "wave.c"
 a3
# 1434 "wave.c" 3
 )) goto 
# 1434 "wave.c"
 pt_16_108
# 1434 "wave.c" 3
 ;
# 1434 "wave.c"
                                 ;
 pt_16_104:
  if (!
# 1436 "wave.c" 3
      (((unsigned long)(
# 1436 "wave.c"
      a1
# 1436 "wave.c" 3
      ) & 0xFL) == (0x2 +0x0))
# 1436 "wave.c"
               ) goto pt_16_105;
  x31 = 
# 1437 "wave.c" 3
       ((q)(((long)(
# 1437 "wave.c"
       1L
# 1437 "wave.c" 3
       ) << 4) + (0x2 +0x0)))
# 1437 "wave.c"
                  ;
  
# 1438 "wave.c" 3
 if ((long)(
# 1438 "wave.c"
 x31
# 1438 "wave.c" 3
 ) > (long)(
# 1438 "wave.c"
 a1
# 1438 "wave.c" 3
 )) goto 
# 1438 "wave.c"
 pt_16_108
# 1438 "wave.c" 3
 ;
# 1438 "wave.c"
                                  ;
  
# 1439 "wave.c" 3
 if ((long)(
# 1439 "wave.c"
 a1
# 1439 "wave.c" 3
 ) > (long)(
# 1439 "wave.c"
 a3
# 1439 "wave.c" 3
 )) goto 
# 1439 "wave.c"
 pt_16_108
# 1439 "wave.c" 3
 ;
# 1439 "wave.c"
                                 ;
  x32 = 
# 1440 "wave.c" 3
       ((q)(((long)(
# 1440 "wave.c"
       2L
# 1440 "wave.c" 3
       ) << 4) + (0x2 +0x0)))
# 1440 "wave.c"
                  ;
  
# 1441 "wave.c" 3
 { 
# 1441 "wave.c"
 x33 
# 1441 "wave.c" 3
 = (q)((unsigned long)(
# 1441 "wave.c"
 a3
# 1441 "wave.c" 3
 ) - ((unsigned long)(
# 1441 "wave.c"
 x32
# 1441 "wave.c" 3
 ) - (0x2 +0x0))); }
# 1441 "wave.c"
                              ;
  allocp[0] = 
# 1442 "wave.c" 3
             ((q)(((unsigned long)(
# 1442 "wave.c"
             190L+
# 1442 "wave.c" 3
             0) << 4) + (0x2 +0x4)))
# 1442 "wave.c"
                                    ;
  allocp[1] = x33;
  allocp[2] = x35 = 
# 1444 "wave.c" 3
                   (q) ((unsigned long)(
# 1444 "wave.c"
                   &allocp[2]
# 1444 "wave.c" 3
                   ) + 0x0)
# 1444 "wave.c"
                                      ;
  allocp[3] = x36 = 
# 1445 "wave.c" 3
                   (q) ((unsigned long)(
# 1445 "wave.c"
                   &allocp[3]
# 1445 "wave.c" 3
                   ) + 0x0)
# 1445 "wave.c"
                                      ;
  x34 = 
# 1446 "wave.c" 3
       ((q)((unsigned long)(
# 1446 "wave.c"
       &allocp[0]
# 1446 "wave.c" 3
       ) + 0x3))
# 1446 "wave.c"
                              ;
  allocp[4] = x38 = 
# 1447 "wave.c" 3
                   (q) ((unsigned long)(
# 1447 "wave.c"
                   &allocp[4]
# 1447 "wave.c" 3
                   ) + 0x0)
# 1447 "wave.c"
                                      ;
  allocp[5] = x34;
  x37 = 
# 1449 "wave.c" 3
       ((q)((unsigned long)(
# 1449 "wave.c"
       &allocp[4]
# 1449 "wave.c" 3
       ) + 0x1))
# 1449 "wave.c"
                           ;
  allocp += 6;
  
# 1451 "wave.c" 3
 { if (!(((unsigned long)(
# 1451 "wave.c"
 x28
# 1451 "wave.c" 3
 ) & 0x3) == 0x0) || (*((q *)((unsigned long)(
# 1451 "wave.c"
 x28
# 1451 "wave.c" 3
 ) - 0x0))) != (
# 1451 "wave.c"
 x28
# 1451 "wave.c" 3
 )) { allocp = do_unify_value(allocp, (
# 1451 "wave.c"
 x28
# 1451 "wave.c" 3
 ), (
# 1451 "wave.c"
 x37
# 1451 "wave.c" 3
 )); } else { (*((q *)((unsigned long)(
# 1451 "wave.c"
 x28
# 1451 "wave.c" 3
 ) - 0x0))) = (
# 1451 "wave.c"
 x37
# 1451 "wave.c" 3
 ); } }
# 1451 "wave.c"
                      ;
  allocp[0] = 
# 1452 "wave.c" 3
             ((q)(((unsigned long)(0) << 4) + (0x2 +0x4)))
# 1452 "wave.c"
                    ;
  allocp[1] = x35;
  x39 = 
# 1454 "wave.c" 3
       ((q)((unsigned long)(
# 1454 "wave.c"
       &allocp[0]
# 1454 "wave.c" 3
       ) + 0x1))
# 1454 "wave.c"
                           ;
  allocp[2] = (q)qp;
  allocp[3] = (q)(&predicate_main_xnode_7);
  allocp[4] = a0;
  allocp[5] = a1;
  allocp[6] = 
# 1459 "wave.c" 3
             ((q)(((long)(
# 1459 "wave.c"
             0L
# 1459 "wave.c" 3
             ) << 4) + (0x2 +0x0)))
# 1459 "wave.c"
                        ;
  allocp[7] = x39;
  allocp[8] = x40 = 
# 1461 "wave.c" 3
                   (q) ((unsigned long)(
# 1461 "wave.c"
                   &allocp[8]
# 1461 "wave.c" 3
                   ) + 0x0)
# 1461 "wave.c"
                                      ;
  allocp[9] = a11;
  allocp[10] = a12;
  allocp[11] = (q)(struct goalrec*)&allocp[2];
  allocp[12] = (q)(&predicate_main_xroute_14);
  allocp[13] = a15;
  allocp[14] = x1;
  allocp[15] = x6;
  allocp[16] = x11;
  allocp[17] = x16;
  allocp[18] = x21;
  allocp[19] = x26;
  allocp[20] = x3;
  allocp[21] = x8;
  allocp[22] = x13;
  allocp[23] = x18;
  allocp[24] = x23;
  allocp[25] = x38;
  allocp[26] = x40;
  a0 = x36;
  a1 = a13;
  a2 = a14;
  qp = (struct goalrec*)&allocp[11];
  allocp += 27;
  
# 1485 "wave.c" 3
 { if (allocp < (glbl->heaplimit0)) goto 
# 1485 "wave.c"
 acknowledged_3_clear_reason
# 1485 "wave.c" 3
 ;}
# 1485 "wave.c"
                                     ;
  goto acknowledged_3_ext_interrupt;
 pt_16_105:
  if (!
# 1488 "wave.c" 3
      (((unsigned long)(
# 1488 "wave.c"
      a1
# 1488 "wave.c" 3
      ) & 0x3) == 0x0)
# 1488 "wave.c"
               ) goto pt_16_108;
  
# 1489 "wave.c" 3
 { q temp0 = (*((q *)((unsigned long)(
# 1489 "wave.c"
 a1
# 1489 "wave.c" 3
 ) - 0x0))); if(!(((unsigned long)(temp0) & 0x3) == 0x0)) { (
# 1489 "wave.c"
 a1
# 1489 "wave.c" 3
 ) = temp0; goto 
# 1489 "wave.c"
 pt_16_104
# 1489 "wave.c" 3
 ; } }
# 1489 "wave.c"
                             ;
  *reasonp++ = a1;
  goto pt_16_108;
 pt_16_106:
  if (!
# 1493 "wave.c" 3
      (((unsigned long)(
# 1493 "wave.c"
      a3
# 1493 "wave.c" 3
      ) & 0x3) == 0x0)
# 1493 "wave.c"
               ) goto pt_16_108;
  
# 1494 "wave.c" 3
 { q temp0 = (*((q *)((unsigned long)(
# 1494 "wave.c"
 a3
# 1494 "wave.c" 3
 ) - 0x0))); if(!(((unsigned long)(temp0) & 0x3) == 0x0)) { (
# 1494 "wave.c"
 a3
# 1494 "wave.c" 3
 ) = temp0; goto 
# 1494 "wave.c"
 pt_16_103
# 1494 "wave.c" 3
 ; } }
# 1494 "wave.c"
                             ;
  *reasonp++ = a3;
  goto pt_16_108;
 pt_16_107:
  if (!
# 1498 "wave.c" 3
      (((unsigned long)(
# 1498 "wave.c"
      a0
# 1498 "wave.c" 3
      ) & 0x3) == 0x0)
# 1498 "wave.c"
               ) goto pt_16_108;
  
# 1499 "wave.c" 3
 { q temp0 = (*((q *)((unsigned long)(
# 1499 "wave.c"
 a0
# 1499 "wave.c" 3
 ) - 0x0))); if(!(((unsigned long)(temp0) & 0x3) == 0x0)) { (
# 1499 "wave.c"
 a0
# 1499 "wave.c" 3
 ) = temp0; goto 
# 1499 "wave.c"
 pt_16_102
# 1499 "wave.c" 3
 ; } }
# 1499 "wave.c"
                             ;
  *reasonp++ = a0;
  goto pt_16_108;
 case 
# 1502 "wave.c" 3
     0x0
# 1502 "wave.c"
           :
  
# 1503 "wave.c" 3
 { q temp0 = (*((q *)((unsigned long)(
# 1503 "wave.c"
 a10
# 1503 "wave.c" 3
 ) - 0x0))); if(!(((unsigned long)(temp0) & 0x3) == 0x0)) { (
# 1503 "wave.c"
 a10
# 1503 "wave.c" 3
 ) = temp0; goto 
# 1503 "wave.c"
 pt_16_99
# 1503 "wave.c" 3
 ; } }
# 1503 "wave.c"
                             ;
  *reasonp++ = a10;
  goto pt_16_108;
  };
  goto pt_16_108;
 case 
# 1508 "wave.c" 3
     0x0
# 1508 "wave.c"
           :
  
# 1509 "wave.c" 3
 { q temp0 = (*((q *)((unsigned long)(
# 1509 "wave.c"
 a9
# 1509 "wave.c" 3
 ) - 0x0))); if(!(((unsigned long)(temp0) & 0x3) == 0x0)) { (
# 1509 "wave.c"
 a9
# 1509 "wave.c" 3
 ) = temp0; goto 
# 1509 "wave.c"
 pt_16_96
# 1509 "wave.c" 3
 ; } }
# 1509 "wave.c"
                            ;
  *reasonp++ = a9;
  goto pt_16_108;
  };
  goto pt_16_108;
 case 
# 1514 "wave.c" 3
     0x0
# 1514 "wave.c"
           :
  
# 1515 "wave.c" 3
 { q temp0 = (*((q *)((unsigned long)(
# 1515 "wave.c"
 a8
# 1515 "wave.c" 3
 ) - 0x0))); if(!(((unsigned long)(temp0) & 0x3) == 0x0)) { (
# 1515 "wave.c"
 a8
# 1515 "wave.c" 3
 ) = temp0; goto 
# 1515 "wave.c"
 pt_16_93
# 1515 "wave.c" 3
 ; } }
# 1515 "wave.c"
                            ;
  *reasonp++ = a8;
  goto pt_16_108;
  };
  goto pt_16_108;
 case 
# 1520 "wave.c" 3
     0x0
# 1520 "wave.c"
           :
  
# 1521 "wave.c" 3
 { q temp0 = (*((q *)((unsigned long)(
# 1521 "wave.c"
 a7
# 1521 "wave.c" 3
 ) - 0x0))); if(!(((unsigned long)(temp0) & 0x3) == 0x0)) { (
# 1521 "wave.c"
 a7
# 1521 "wave.c" 3
 ) = temp0; goto 
# 1521 "wave.c"
 pt_16_90
# 1521 "wave.c" 3
 ; } }
# 1521 "wave.c"
                            ;
  *reasonp++ = a7;
  goto pt_16_108;
  };
  goto pt_16_108;
 case 
# 1526 "wave.c" 3
     0x0
# 1526 "wave.c"
           :
  
# 1527 "wave.c" 3
 { q temp0 = (*((q *)((unsigned long)(
# 1527 "wave.c"
 a6
# 1527 "wave.c" 3
 ) - 0x0))); if(!(((unsigned long)(temp0) & 0x3) == 0x0)) { (
# 1527 "wave.c"
 a6
# 1527 "wave.c" 3
 ) = temp0; goto 
# 1527 "wave.c"
 pt_16_87
# 1527 "wave.c" 3
 ; } }
# 1527 "wave.c"
                            ;
  *reasonp++ = a6;
  goto pt_16_108;
  };
  goto pt_16_108;
 case 
# 1532 "wave.c" 3
     0x0
# 1532 "wave.c"
           :
  
# 1533 "wave.c" 3
 { q temp0 = (*((q *)((unsigned long)(
# 1533 "wave.c"
 a5
# 1533 "wave.c" 3
 ) - 0x0))); if(!(((unsigned long)(temp0) & 0x3) == 0x0)) { (
# 1533 "wave.c"
 a5
# 1533 "wave.c" 3
 ) = temp0; goto 
# 1533 "wave.c"
 pt_16_84
# 1533 "wave.c" 3
 ; } }
# 1533 "wave.c"
                            ;
  *reasonp++ = a5;
  goto pt_16_108;
  };
  goto pt_16_108;
 case 
# 1538 "wave.c" 3
     0x0
# 1538 "wave.c"
           :
  
# 1539 "wave.c" 3
 { q temp0 = (*((q *)((unsigned long)(
# 1539 "wave.c"
 a2
# 1539 "wave.c" 3
 ) - 0x0))); if(!(((unsigned long)(temp0) & 0x3) == 0x0)) { (
# 1539 "wave.c"
 a2
# 1539 "wave.c" 3
 ) = temp0; goto 
# 1539 "wave.c"
 pt_16_82
# 1539 "wave.c" 3
 ; } }
# 1539 "wave.c"
                            ;
  *reasonp++ = a2;
  goto pt_16_135;
  };
  goto pt_16_108;
 pt_16_108:
 pt_16_109:
  if (
# 1546 "wave.c" 3
     (((unsigned long)(
# 1546 "wave.c"
     a2
# 1546 "wave.c" 3
     ) & 0x3) == 0x0)
# 1546 "wave.c"
              ) goto pt_16_134;
  
# 1547 "wave.c" 3
 if ((
# 1547 "wave.c"
 a2
# 1547 "wave.c" 3
 )!=(
# 1547 "wave.c"
 a4
# 1547 "wave.c" 3
 ) || (((unsigned long)(
# 1547 "wave.c"
 a2
# 1547 "wave.c" 3
 ) & 0x3) == 0x0)) { q retval; switch((long)(retval = eq_terms_body((
# 1547 "wave.c"
 a2
# 1547 "wave.c" 3
 ), (
# 1547 "wave.c"
 a4
# 1547 "wave.c" 3
 )))){ case 0: break; default: *reasonp++ = retval; case 1: goto 
# 1547 "wave.c"
 pt_16_135
# 1547 "wave.c" 3
 ; } }
# 1547 "wave.c"
                                ;
 pt_16_110:
  switch (
# 1549 "wave.c" 3
         ((unsigned long)(
# 1549 "wave.c"
         a5
# 1549 "wave.c" 3
         ) & 0x3)
# 1549 "wave.c"
                   ) {
 case 
# 1550 "wave.c" 3
     0x3
# 1550 "wave.c"
            :
 pt_16_111:
  if (!
# 1552 "wave.c" 3
      ((((unsigned long)((((struct functor *)((unsigned long)(
# 1552 "wave.c"
      a5
# 1552 "wave.c" 3
      ) - 0x3))->functor)) & 0x3) == 0x0))
# 1552 "wave.c"
                ) goto pt_16_135;
  if (!
# 1553 "wave.c" 3
      ((struct data_object_method_table *)((((struct functor *)((unsigned long)(
# 1553 "wave.c"
      a5
# 1553 "wave.c" 3
      ) - 0x3))->functor)) == &
# 1553 "wave.c"
      vector_g_data_method_table
# 1553 "wave.c" 3
      )
# 1553 "wave.c"
                        ) goto pt_16_135;
  
# 1554 "wave.c" 3
 { extern q size_of_vector(); 
# 1554 "wave.c"
 x0 
# 1554 "wave.c" 3
 = size_of_vector(
# 1554 "wave.c"
 a5
# 1554 "wave.c" 3
 ); }
# 1554 "wave.c"
                                     ;
 pt_16_112:
  if (x0 != 
# 1556 "wave.c" 3
           ((q)(((long)(
# 1556 "wave.c"
           2L
# 1556 "wave.c" 3
           ) << 4) + (0x2 +0x0)))
# 1556 "wave.c"
                      ) goto pt_16_135;
  x2 = 
# 1557 "wave.c" 3
      ((q)(((long)(
# 1557 "wave.c"
      0L
# 1557 "wave.c" 3
      ) << 4) + (0x2 +0x0)))
# 1557 "wave.c"
                 ;
  
# 1558 "wave.c" 3
 { extern q element_of_vector(); 
# 1558 "wave.c"
 x1 
# 1558 "wave.c" 3
 = element_of_vector(
# 1558 "wave.c"
 a5
# 1558 "wave.c" 3
 ,
# 1558 "wave.c"
 x2
# 1558 "wave.c" 3
 ); if ((long)
# 1558 "wave.c"
 x1 
# 1558 "wave.c" 3
 == 0) goto 
# 1558 "wave.c"
 pt_16_135
# 1558 "wave.c" 3
 ; }
# 1558 "wave.c"
                                           ;
  x4 = 
# 1559 "wave.c" 3
      ((q)(((long)(
# 1559 "wave.c"
      1L
# 1559 "wave.c" 3
      ) << 4) + (0x2 +0x0)))
# 1559 "wave.c"
                 ;
  
# 1560 "wave.c" 3
 { extern q element_of_vector(); 
# 1560 "wave.c"
 x3 
# 1560 "wave.c" 3
 = element_of_vector(
# 1560 "wave.c"
 a5
# 1560 "wave.c" 3
 ,
# 1560 "wave.c"
 x4
# 1560 "wave.c" 3
 ); if ((long)
# 1560 "wave.c"
 x3 
# 1560 "wave.c" 3
 == 0) goto 
# 1560 "wave.c"
 pt_16_135
# 1560 "wave.c" 3
 ; }
# 1560 "wave.c"
                                           ;
 pt_16_113:
  switch (
# 1562 "wave.c" 3
         ((unsigned long)(
# 1562 "wave.c"
         a6
# 1562 "wave.c" 3
         ) & 0x3)
# 1562 "wave.c"
                   ) {
 case 
# 1563 "wave.c" 3
     0x3
# 1563 "wave.c"
            :
 pt_16_114:
  if (!
# 1565 "wave.c" 3
      ((((unsigned long)((((struct functor *)((unsigned long)(
# 1565 "wave.c"
      a6
# 1565 "wave.c" 3
      ) - 0x3))->functor)) & 0x3) == 0x0))
# 1565 "wave.c"
                ) goto pt_16_135;
  if (!
# 1566 "wave.c" 3
      ((struct data_object_method_table *)((((struct functor *)((unsigned long)(
# 1566 "wave.c"
      a6
# 1566 "wave.c" 3
      ) - 0x3))->functor)) == &
# 1566 "wave.c"
      vector_g_data_method_table
# 1566 "wave.c" 3
      )
# 1566 "wave.c"
                        ) goto pt_16_135;
  
# 1567 "wave.c" 3
 { extern q size_of_vector(); 
# 1567 "wave.c"
 x5 
# 1567 "wave.c" 3
 = size_of_vector(
# 1567 "wave.c"
 a6
# 1567 "wave.c" 3
 ); }
# 1567 "wave.c"
                                     ;
 pt_16_115:
  if (x5 != 
# 1569 "wave.c" 3
           ((q)(((long)(
# 1569 "wave.c"
           2L
# 1569 "wave.c" 3
           ) << 4) + (0x2 +0x0)))
# 1569 "wave.c"
                      ) goto pt_16_135;
  x7 = 
# 1570 "wave.c" 3
      ((q)(((long)(
# 1570 "wave.c"
      0L
# 1570 "wave.c" 3
      ) << 4) + (0x2 +0x0)))
# 1570 "wave.c"
                 ;
  
# 1571 "wave.c" 3
 { extern q element_of_vector(); 
# 1571 "wave.c"
 x6 
# 1571 "wave.c" 3
 = element_of_vector(
# 1571 "wave.c"
 a6
# 1571 "wave.c" 3
 ,
# 1571 "wave.c"
 x7
# 1571 "wave.c" 3
 ); if ((long)
# 1571 "wave.c"
 x6 
# 1571 "wave.c" 3
 == 0) goto 
# 1571 "wave.c"
 pt_16_135
# 1571 "wave.c" 3
 ; }
# 1571 "wave.c"
                                           ;
  x9 = 
# 1572 "wave.c" 3
      ((q)(((long)(
# 1572 "wave.c"
      1L
# 1572 "wave.c" 3
      ) << 4) + (0x2 +0x0)))
# 1572 "wave.c"
                 ;
  
# 1573 "wave.c" 3
 { extern q element_of_vector(); 
# 1573 "wave.c"
 x8 
# 1573 "wave.c" 3
 = element_of_vector(
# 1573 "wave.c"
 a6
# 1573 "wave.c" 3
 ,
# 1573 "wave.c"
 x9
# 1573 "wave.c" 3
 ); if ((long)
# 1573 "wave.c"
 x8 
# 1573 "wave.c" 3
 == 0) goto 
# 1573 "wave.c"
 pt_16_135
# 1573 "wave.c" 3
 ; }
# 1573 "wave.c"
                                           ;
 pt_16_116:
  switch (
# 1575 "wave.c" 3
         ((unsigned long)(
# 1575 "wave.c"
         a7
# 1575 "wave.c" 3
         ) & 0x3)
# 1575 "wave.c"
                   ) {
 case 
# 1576 "wave.c" 3
     0x3
# 1576 "wave.c"
            :
 pt_16_117:
  if (!
# 1578 "wave.c" 3
      ((((unsigned long)((((struct functor *)((unsigned long)(
# 1578 "wave.c"
      a7
# 1578 "wave.c" 3
      ) - 0x3))->functor)) & 0x3) == 0x0))
# 1578 "wave.c"
                ) goto pt_16_135;
  if (!
# 1579 "wave.c" 3
      ((struct data_object_method_table *)((((struct functor *)((unsigned long)(
# 1579 "wave.c"
      a7
# 1579 "wave.c" 3
      ) - 0x3))->functor)) == &
# 1579 "wave.c"
      vector_g_data_method_table
# 1579 "wave.c" 3
      )
# 1579 "wave.c"
                        ) goto pt_16_135;
  
# 1580 "wave.c" 3
 { extern q size_of_vector(); 
# 1580 "wave.c"
 x10 
# 1580 "wave.c" 3
 = size_of_vector(
# 1580 "wave.c"
 a7
# 1580 "wave.c" 3
 ); }
# 1580 "wave.c"
                                      ;
 pt_16_118:
  if (x10 != 
# 1582 "wave.c" 3
            ((q)(((long)(
# 1582 "wave.c"
            2L
# 1582 "wave.c" 3
            ) << 4) + (0x2 +0x0)))
# 1582 "wave.c"
                       ) goto pt_16_135;
  x12 = 
# 1583 "wave.c" 3
       ((q)(((long)(
# 1583 "wave.c"
       0L
# 1583 "wave.c" 3
       ) << 4) + (0x2 +0x0)))
# 1583 "wave.c"
                  ;
  
# 1584 "wave.c" 3
 { extern q element_of_vector(); 
# 1584 "wave.c"
 x11 
# 1584 "wave.c" 3
 = element_of_vector(
# 1584 "wave.c"
 a7
# 1584 "wave.c" 3
 ,
# 1584 "wave.c"
 x12
# 1584 "wave.c" 3
 ); if ((long)
# 1584 "wave.c"
 x11 
# 1584 "wave.c" 3
 == 0) goto 
# 1584 "wave.c"
 pt_16_135
# 1584 "wave.c" 3
 ; }
# 1584 "wave.c"
                                             ;
  x14 = 
# 1585 "wave.c" 3
       ((q)(((long)(
# 1585 "wave.c"
       1L
# 1585 "wave.c" 3
       ) << 4) + (0x2 +0x0)))
# 1585 "wave.c"
                  ;
  
# 1586 "wave.c" 3
 { extern q element_of_vector(); 
# 1586 "wave.c"
 x13 
# 1586 "wave.c" 3
 = element_of_vector(
# 1586 "wave.c"
 a7
# 1586 "wave.c" 3
 ,
# 1586 "wave.c"
 x14
# 1586 "wave.c" 3
 ); if ((long)
# 1586 "wave.c"
 x13 
# 1586 "wave.c" 3
 == 0) goto 
# 1586 "wave.c"
 pt_16_135
# 1586 "wave.c" 3
 ; }
# 1586 "wave.c"
                                             ;
 pt_16_119:
  switch (
# 1588 "wave.c" 3
         ((unsigned long)(
# 1588 "wave.c"
         a8
# 1588 "wave.c" 3
         ) & 0x3)
# 1588 "wave.c"
                   ) {
 case 
# 1589 "wave.c" 3
     0x3
# 1589 "wave.c"
            :
 pt_16_120:
  if (!
# 1591 "wave.c" 3
      ((((unsigned long)((((struct functor *)((unsigned long)(
# 1591 "wave.c"
      a8
# 1591 "wave.c" 3
      ) - 0x3))->functor)) & 0x3) == 0x0))
# 1591 "wave.c"
                ) goto pt_16_135;
  if (!
# 1592 "wave.c" 3
      ((struct data_object_method_table *)((((struct functor *)((unsigned long)(
# 1592 "wave.c"
      a8
# 1592 "wave.c" 3
      ) - 0x3))->functor)) == &
# 1592 "wave.c"
      vector_g_data_method_table
# 1592 "wave.c" 3
      )
# 1592 "wave.c"
                        ) goto pt_16_135;
  
# 1593 "wave.c" 3
 { extern q size_of_vector(); 
# 1593 "wave.c"
 x15 
# 1593 "wave.c" 3
 = size_of_vector(
# 1593 "wave.c"
 a8
# 1593 "wave.c" 3
 ); }
# 1593 "wave.c"
                                      ;
 pt_16_121:
  if (x15 != 
# 1595 "wave.c" 3
            ((q)(((long)(
# 1595 "wave.c"
            2L
# 1595 "wave.c" 3
            ) << 4) + (0x2 +0x0)))
# 1595 "wave.c"
                       ) goto pt_16_135;
  x17 = 
# 1596 "wave.c" 3
       ((q)(((long)(
# 1596 "wave.c"
       0L
# 1596 "wave.c" 3
       ) << 4) + (0x2 +0x0)))
# 1596 "wave.c"
                  ;
  
# 1597 "wave.c" 3
 { extern q element_of_vector(); 
# 1597 "wave.c"
 x16 
# 1597 "wave.c" 3
 = element_of_vector(
# 1597 "wave.c"
 a8
# 1597 "wave.c" 3
 ,
# 1597 "wave.c"
 x17
# 1597 "wave.c" 3
 ); if ((long)
# 1597 "wave.c"
 x16 
# 1597 "wave.c" 3
 == 0) goto 
# 1597 "wave.c"
 pt_16_135
# 1597 "wave.c" 3
 ; }
# 1597 "wave.c"
                                             ;
  x19 = 
# 1598 "wave.c" 3
       ((q)(((long)(
# 1598 "wave.c"
       1L
# 1598 "wave.c" 3
       ) << 4) + (0x2 +0x0)))
# 1598 "wave.c"
                  ;
  
# 1599 "wave.c" 3
 { extern q element_of_vector(); 
# 1599 "wave.c"
 x18 
# 1599 "wave.c" 3
 = element_of_vector(
# 1599 "wave.c"
 a8
# 1599 "wave.c" 3
 ,
# 1599 "wave.c"
 x19
# 1599 "wave.c" 3
 ); if ((long)
# 1599 "wave.c"
 x18 
# 1599 "wave.c" 3
 == 0) goto 
# 1599 "wave.c"
 pt_16_135
# 1599 "wave.c" 3
 ; }
# 1599 "wave.c"
                                             ;
 pt_16_122:
  switch (
# 1601 "wave.c" 3
         ((unsigned long)(
# 1601 "wave.c"
         a9
# 1601 "wave.c" 3
         ) & 0x3)
# 1601 "wave.c"
                   ) {
 case 
# 1602 "wave.c" 3
     0x3
# 1602 "wave.c"
            :
 pt_16_123:
  if (!
# 1604 "wave.c" 3
      ((((unsigned long)((((struct functor *)((unsigned long)(
# 1604 "wave.c"
      a9
# 1604 "wave.c" 3
      ) - 0x3))->functor)) & 0x3) == 0x0))
# 1604 "wave.c"
                ) goto pt_16_135;
  if (!
# 1605 "wave.c" 3
      ((struct data_object_method_table *)((((struct functor *)((unsigned long)(
# 1605 "wave.c"
      a9
# 1605 "wave.c" 3
      ) - 0x3))->functor)) == &
# 1605 "wave.c"
      vector_g_data_method_table
# 1605 "wave.c" 3
      )
# 1605 "wave.c"
                        ) goto pt_16_135;
  
# 1606 "wave.c" 3
 { extern q size_of_vector(); 
# 1606 "wave.c"
 x20 
# 1606 "wave.c" 3
 = size_of_vector(
# 1606 "wave.c"
 a9
# 1606 "wave.c" 3
 ); }
# 1606 "wave.c"
                                      ;
 pt_16_124:
  if (x20 != 
# 1608 "wave.c" 3
            ((q)(((long)(
# 1608 "wave.c"
            2L
# 1608 "wave.c" 3
            ) << 4) + (0x2 +0x0)))
# 1608 "wave.c"
                       ) goto pt_16_135;
  x22 = 
# 1609 "wave.c" 3
       ((q)(((long)(
# 1609 "wave.c"
       0L
# 1609 "wave.c" 3
       ) << 4) + (0x2 +0x0)))
# 1609 "wave.c"
                  ;
  
# 1610 "wave.c" 3
 { extern q element_of_vector(); 
# 1610 "wave.c"
 x21 
# 1610 "wave.c" 3
 = element_of_vector(
# 1610 "wave.c"
 a9
# 1610 "wave.c" 3
 ,
# 1610 "wave.c"
 x22
# 1610 "wave.c" 3
 ); if ((long)
# 1610 "wave.c"
 x21 
# 1610 "wave.c" 3
 == 0) goto 
# 1610 "wave.c"
 pt_16_135
# 1610 "wave.c" 3
 ; }
# 1610 "wave.c"
                                             ;
  x24 = 
# 1611 "wave.c" 3
       ((q)(((long)(
# 1611 "wave.c"
       1L
# 1611 "wave.c" 3
       ) << 4) + (0x2 +0x0)))
# 1611 "wave.c"
                  ;
  
# 1612 "wave.c" 3
 { extern q element_of_vector(); 
# 1612 "wave.c"
 x23 
# 1612 "wave.c" 3
 = element_of_vector(
# 1612 "wave.c"
 a9
# 1612 "wave.c" 3
 ,
# 1612 "wave.c"
 x24
# 1612 "wave.c" 3
 ); if ((long)
# 1612 "wave.c"
 x23 
# 1612 "wave.c" 3
 == 0) goto 
# 1612 "wave.c"
 pt_16_135
# 1612 "wave.c" 3
 ; }
# 1612 "wave.c"
                                             ;
 pt_16_125:
  switch (
# 1614 "wave.c" 3
         ((unsigned long)(
# 1614 "wave.c"
         a10
# 1614 "wave.c" 3
         ) & 0x3)
# 1614 "wave.c"
                    ) {
 case 
# 1615 "wave.c" 3
     0x3
# 1615 "wave.c"
            :
 pt_16_126:
  if (!
# 1617 "wave.c" 3
      ((((unsigned long)((((struct functor *)((unsigned long)(
# 1617 "wave.c"
      a10
# 1617 "wave.c" 3
      ) - 0x3))->functor)) & 0x3) == 0x0))
# 1617 "wave.c"
                 ) goto pt_16_135;
  if (!
# 1618 "wave.c" 3
      ((struct data_object_method_table *)((((struct functor *)((unsigned long)(
# 1618 "wave.c"
      a10
# 1618 "wave.c" 3
      ) - 0x3))->functor)) == &
# 1618 "wave.c"
      vector_g_data_method_table
# 1618 "wave.c" 3
      )
# 1618 "wave.c"
                         ) goto pt_16_135;
  
# 1619 "wave.c" 3
 { extern q size_of_vector(); 
# 1619 "wave.c"
 x25 
# 1619 "wave.c" 3
 = size_of_vector(
# 1619 "wave.c"
 a10
# 1619 "wave.c" 3
 ); }
# 1619 "wave.c"
                                       ;
 pt_16_127:
  if (x25 != 
# 1621 "wave.c" 3
            ((q)(((long)(
# 1621 "wave.c"
            2L
# 1621 "wave.c" 3
            ) << 4) + (0x2 +0x0)))
# 1621 "wave.c"
                       ) goto pt_16_135;
  x27 = 
# 1622 "wave.c" 3
       ((q)(((long)(
# 1622 "wave.c"
       0L
# 1622 "wave.c" 3
       ) << 4) + (0x2 +0x0)))
# 1622 "wave.c"
                  ;
  
# 1623 "wave.c" 3
 { extern q element_of_vector(); 
# 1623 "wave.c"
 x26 
# 1623 "wave.c" 3
 = element_of_vector(
# 1623 "wave.c"
 a10
# 1623 "wave.c" 3
 ,
# 1623 "wave.c"
 x27
# 1623 "wave.c" 3
 ); if ((long)
# 1623 "wave.c"
 x26 
# 1623 "wave.c" 3
 == 0) goto 
# 1623 "wave.c"
 pt_16_135
# 1623 "wave.c" 3
 ; }
# 1623 "wave.c"
                                              ;
  x29 = 
# 1624 "wave.c" 3
       ((q)(((long)(
# 1624 "wave.c"
       1L
# 1624 "wave.c" 3
       ) << 4) + (0x2 +0x0)))
# 1624 "wave.c"
                  ;
  
# 1625 "wave.c" 3
 { extern q element_of_vector(); 
# 1625 "wave.c"
 x28 
# 1625 "wave.c" 3
 = element_of_vector(
# 1625 "wave.c"
 a10
# 1625 "wave.c" 3
 ,
# 1625 "wave.c"
 x29
# 1625 "wave.c" 3
 ); if ((long)
# 1625 "wave.c"
 x28 
# 1625 "wave.c" 3
 == 0) goto 
# 1625 "wave.c"
 pt_16_135
# 1625 "wave.c" 3
 ; }
# 1625 "wave.c"
                                              ;
 pt_16_128:
  if (!
# 1627 "wave.c" 3
      (((unsigned long)(
# 1627 "wave.c"
      a0
# 1627 "wave.c" 3
      ) & 0xFL) == (0x2 +0x0))
# 1627 "wave.c"
               ) goto pt_16_133;
  x30 = 
# 1628 "wave.c" 3
       ((q)(((long)(
# 1628 "wave.c"
       1L
# 1628 "wave.c" 3
       ) << 4) + (0x2 +0x0)))
# 1628 "wave.c"
                  ;
  
# 1629 "wave.c" 3
 if ((long)(
# 1629 "wave.c"
 x30
# 1629 "wave.c" 3
 ) > (long)(
# 1629 "wave.c"
 a0
# 1629 "wave.c" 3
 )) goto 
# 1629 "wave.c"
 pt_16_135
# 1629 "wave.c" 3
 ;
# 1629 "wave.c"
                                  ;
 pt_16_129:
  if (!
# 1631 "wave.c" 3
      (((unsigned long)(
# 1631 "wave.c"
      a3
# 1631 "wave.c" 3
      ) & 0xFL) == (0x2 +0x0))
# 1631 "wave.c"
               ) goto pt_16_132;
  
# 1632 "wave.c" 3
 if ((long)(
# 1632 "wave.c"
 a0
# 1632 "wave.c" 3
 ) > (long)(
# 1632 "wave.c"
 a3
# 1632 "wave.c" 3
 )) goto 
# 1632 "wave.c"
 pt_16_135
# 1632 "wave.c" 3
 ;
# 1632 "wave.c"
                                 ;
 pt_16_130:
  if (!
# 1634 "wave.c" 3
      (((unsigned long)(
# 1634 "wave.c"
      a1
# 1634 "wave.c" 3
      ) & 0xFL) == (0x2 +0x0))
# 1634 "wave.c"
               ) goto pt_16_131;
  x31 = 
# 1635 "wave.c" 3
       ((q)(((long)(
# 1635 "wave.c"
       1L
# 1635 "wave.c" 3
       ) << 4) + (0x2 +0x0)))
# 1635 "wave.c"
                  ;
  
# 1636 "wave.c" 3
 if ((long)(
# 1636 "wave.c"
 x31
# 1636 "wave.c" 3
 ) > (long)(
# 1636 "wave.c"
 a1
# 1636 "wave.c" 3
 )) goto 
# 1636 "wave.c"
 pt_16_135
# 1636 "wave.c" 3
 ;
# 1636 "wave.c"
                                  ;
  
# 1637 "wave.c" 3
 if ((long)(
# 1637 "wave.c"
 a1
# 1637 "wave.c" 3
 ) > (long)(
# 1637 "wave.c"
 a3
# 1637 "wave.c" 3
 )) goto 
# 1637 "wave.c"
 pt_16_135
# 1637 "wave.c" 3
 ;
# 1637 "wave.c"
                                 ;
  x32 = 
# 1638 "wave.c" 3
       ((q)(((long)(
# 1638 "wave.c"
       2L
# 1638 "wave.c" 3
       ) << 4) + (0x2 +0x0)))
# 1638 "wave.c"
                  ;
  
# 1639 "wave.c" 3
 { 
# 1639 "wave.c"
 x33 
# 1639 "wave.c" 3
 = (q)((unsigned long)(
# 1639 "wave.c"
 a3
# 1639 "wave.c" 3
 ) - ((unsigned long)(
# 1639 "wave.c"
 x32
# 1639 "wave.c" 3
 ) - (0x2 +0x0))); }
# 1639 "wave.c"
                              ;
  allocp[0] = 
# 1640 "wave.c" 3
             ((q)(((unsigned long)(
# 1640 "wave.c"
             190L+
# 1640 "wave.c" 3
             0) << 4) + (0x2 +0x4)))
# 1640 "wave.c"
                                    ;
  allocp[1] = x33;
  allocp[2] = x35 = 
# 1642 "wave.c" 3
                   (q) ((unsigned long)(
# 1642 "wave.c"
                   &allocp[2]
# 1642 "wave.c" 3
                   ) + 0x0)
# 1642 "wave.c"
                                      ;
  allocp[3] = x36 = 
# 1643 "wave.c" 3
                   (q) ((unsigned long)(
# 1643 "wave.c"
                   &allocp[3]
# 1643 "wave.c" 3
                   ) + 0x0)
# 1643 "wave.c"
                                      ;
  x34 = 
# 1644 "wave.c" 3
       ((q)((unsigned long)(
# 1644 "wave.c"
       &allocp[0]
# 1644 "wave.c" 3
       ) + 0x3))
# 1644 "wave.c"
                              ;
  allocp[4] = x38 = 
# 1645 "wave.c" 3
                   (q) ((unsigned long)(
# 1645 "wave.c"
                   &allocp[4]
# 1645 "wave.c" 3
                   ) + 0x0)
# 1645 "wave.c"
                                      ;
  allocp[5] = x34;
  x37 = 
# 1647 "wave.c" 3
       ((q)((unsigned long)(
# 1647 "wave.c"
       &allocp[4]
# 1647 "wave.c" 3
       ) + 0x1))
# 1647 "wave.c"
                           ;
  allocp += 6;
  
# 1649 "wave.c" 3
 { if (!(((unsigned long)(
# 1649 "wave.c"
 x23
# 1649 "wave.c" 3
 ) & 0x3) == 0x0) || (*((q *)((unsigned long)(
# 1649 "wave.c"
 x23
# 1649 "wave.c" 3
 ) - 0x0))) != (
# 1649 "wave.c"
 x23
# 1649 "wave.c" 3
 )) { allocp = do_unify_value(allocp, (
# 1649 "wave.c"
 x23
# 1649 "wave.c" 3
 ), (
# 1649 "wave.c"
 x37
# 1649 "wave.c" 3
 )); } else { (*((q *)((unsigned long)(
# 1649 "wave.c"
 x23
# 1649 "wave.c" 3
 ) - 0x0))) = (
# 1649 "wave.c"
 x37
# 1649 "wave.c" 3
 ); } }
# 1649 "wave.c"
                      ;
  allocp[0] = 
# 1650 "wave.c" 3
             ((q)(((unsigned long)(0) << 4) + (0x2 +0x4)))
# 1650 "wave.c"
                    ;
  allocp[1] = x35;
  x39 = 
# 1652 "wave.c" 3
       ((q)((unsigned long)(
# 1652 "wave.c"
       &allocp[0]
# 1652 "wave.c" 3
       ) + 0x1))
# 1652 "wave.c"
                           ;
  allocp[2] = (q)qp;
  allocp[3] = (q)(&predicate_main_xnode_7);
  allocp[4] = a0;
  allocp[5] = a1;
  allocp[6] = a2;
  allocp[7] = x39;
  allocp[8] = x40 = 
# 1659 "wave.c" 3
                   (q) ((unsigned long)(
# 1659 "wave.c"
                   &allocp[8]
# 1659 "wave.c" 3
                   ) + 0x0)
# 1659 "wave.c"
                                      ;
  allocp[9] = a11;
  allocp[10] = a12;
  allocp[11] = (q)(struct goalrec*)&allocp[2];
  allocp[12] = (q)(&predicate_main_xroute_14);
  allocp[13] = a15;
  allocp[14] = x1;
  allocp[15] = x6;
  allocp[16] = x11;
  allocp[17] = x16;
  allocp[18] = x21;
  allocp[19] = x26;
  allocp[20] = x3;
  allocp[21] = x8;
  allocp[22] = x13;
  allocp[23] = x18;
  allocp[24] = x38;
  allocp[25] = x28;
  allocp[26] = x40;
  a0 = x36;
  a1 = a13;
  a2 = a14;
  qp = (struct goalrec*)&allocp[11];
  allocp += 27;
  
# 1683 "wave.c" 3
 { if (allocp < (glbl->heaplimit0)) goto 
# 1683 "wave.c"
 acknowledged_3_clear_reason
# 1683 "wave.c" 3
 ;}
# 1683 "wave.c"
                                     ;
  goto acknowledged_3_ext_interrupt;
 pt_16_131:
  if (!
# 1686 "wave.c" 3
      (((unsigned long)(
# 1686 "wave.c"
      a1
# 1686 "wave.c" 3
      ) & 0x3) == 0x0)
# 1686 "wave.c"
               ) goto pt_16_135;
  
# 1687 "wave.c" 3
 { q temp0 = (*((q *)((unsigned long)(
# 1687 "wave.c"
 a1
# 1687 "wave.c" 3
 ) - 0x0))); if(!(((unsigned long)(temp0) & 0x3) == 0x0)) { (
# 1687 "wave.c"
 a1
# 1687 "wave.c" 3
 ) = temp0; goto 
# 1687 "wave.c"
 pt_16_130
# 1687 "wave.c" 3
 ; } }
# 1687 "wave.c"
                             ;
  *reasonp++ = a1;
  goto pt_16_135;
 pt_16_132:
  if (!
# 1691 "wave.c" 3
      (((unsigned long)(
# 1691 "wave.c"
      a3
# 1691 "wave.c" 3
      ) & 0x3) == 0x0)
# 1691 "wave.c"
               ) goto pt_16_135;
  
# 1692 "wave.c" 3
 { q temp0 = (*((q *)((unsigned long)(
# 1692 "wave.c"
 a3
# 1692 "wave.c" 3
 ) - 0x0))); if(!(((unsigned long)(temp0) & 0x3) == 0x0)) { (
# 1692 "wave.c"
 a3
# 1692 "wave.c" 3
 ) = temp0; goto 
# 1692 "wave.c"
 pt_16_129
# 1692 "wave.c" 3
 ; } }
# 1692 "wave.c"
                             ;
  *reasonp++ = a3;
  goto pt_16_135;
 pt_16_133:
  if (!
# 1696 "wave.c" 3
      (((unsigned long)(
# 1696 "wave.c"
      a0
# 1696 "wave.c" 3
      ) & 0x3) == 0x0)
# 1696 "wave.c"
               ) goto pt_16_135;
  
# 1697 "wave.c" 3
 { q temp0 = (*((q *)((unsigned long)(
# 1697 "wave.c"
 a0
# 1697 "wave.c" 3
 ) - 0x0))); if(!(((unsigned long)(temp0) & 0x3) == 0x0)) { (
# 1697 "wave.c"
 a0
# 1697 "wave.c" 3
 ) = temp0; goto 
# 1697 "wave.c"
 pt_16_128
# 1697 "wave.c" 3
 ; } }
# 1697 "wave.c"
                             ;
  *reasonp++ = a0;
  goto pt_16_135;
 case 
# 1700 "wave.c" 3
     0x0
# 1700 "wave.c"
           :
  
# 1701 "wave.c" 3
 { q temp0 = (*((q *)((unsigned long)(
# 1701 "wave.c"
 a10
# 1701 "wave.c" 3
 ) - 0x0))); if(!(((unsigned long)(temp0) & 0x3) == 0x0)) { (
# 1701 "wave.c"
 a10
# 1701 "wave.c" 3
 ) = temp0; goto 
# 1701 "wave.c"
 pt_16_125
# 1701 "wave.c" 3
 ; } }
# 1701 "wave.c"
                              ;
  *reasonp++ = a10;
  goto pt_16_135;
  };
  goto pt_16_135;
 case 
# 1706 "wave.c" 3
     0x0
# 1706 "wave.c"
           :
  
# 1707 "wave.c" 3
 { q temp0 = (*((q *)((unsigned long)(
# 1707 "wave.c"
 a9
# 1707 "wave.c" 3
 ) - 0x0))); if(!(((unsigned long)(temp0) & 0x3) == 0x0)) { (
# 1707 "wave.c"
 a9
# 1707 "wave.c" 3
 ) = temp0; goto 
# 1707 "wave.c"
 pt_16_122
# 1707 "wave.c" 3
 ; } }
# 1707 "wave.c"
                             ;
  *reasonp++ = a9;
  goto pt_16_135;
  };
  goto pt_16_135;
 case 
# 1712 "wave.c" 3
     0x0
# 1712 "wave.c"
           :
  
# 1713 "wave.c" 3
 { q temp0 = (*((q *)((unsigned long)(
# 1713 "wave.c"
 a8
# 1713 "wave.c" 3
 ) - 0x0))); if(!(((unsigned long)(temp0) & 0x3) == 0x0)) { (
# 1713 "wave.c"
 a8
# 1713 "wave.c" 3
 ) = temp0; goto 
# 1713 "wave.c"
 pt_16_119
# 1713 "wave.c" 3
 ; } }
# 1713 "wave.c"
                             ;
  *reasonp++ = a8;
  goto pt_16_135;
  };
  goto pt_16_135;
 case 
# 1718 "wave.c" 3
     0x0
# 1718 "wave.c"
           :
  
# 1719 "wave.c" 3
 { q temp0 = (*((q *)((unsigned long)(
# 1719 "wave.c"
 a7
# 1719 "wave.c" 3
 ) - 0x0))); if(!(((unsigned long)(temp0) & 0x3) == 0x0)) { (
# 1719 "wave.c"
 a7
# 1719 "wave.c" 3
 ) = temp0; goto 
# 1719 "wave.c"
 pt_16_116
# 1719 "wave.c" 3
 ; } }
# 1719 "wave.c"
                             ;
  *reasonp++ = a7;
  goto pt_16_135;
  };
  goto pt_16_135;
 case 
# 1724 "wave.c" 3
     0x0
# 1724 "wave.c"
           :
  
# 1725 "wave.c" 3
 { q temp0 = (*((q *)((unsigned long)(
# 1725 "wave.c"
 a6
# 1725 "wave.c" 3
 ) - 0x0))); if(!(((unsigned long)(temp0) & 0x3) == 0x0)) { (
# 1725 "wave.c"
 a6
# 1725 "wave.c" 3
 ) = temp0; goto 
# 1725 "wave.c"
 pt_16_113
# 1725 "wave.c" 3
 ; } }
# 1725 "wave.c"
                             ;
  *reasonp++ = a6;
  goto pt_16_135;
  };
  goto pt_16_135;
 case 
# 1730 "wave.c" 3
     0x0
# 1730 "wave.c"
           :
  
# 1731 "wave.c" 3
 { q temp0 = (*((q *)((unsigned long)(
# 1731 "wave.c"
 a5
# 1731 "wave.c" 3
 ) - 0x0))); if(!(((unsigned long)(temp0) & 0x3) == 0x0)) { (
# 1731 "wave.c"
 a5
# 1731 "wave.c" 3
 ) = temp0; goto 
# 1731 "wave.c"
 pt_16_110
# 1731 "wave.c" 3
 ; } }
# 1731 "wave.c"
                             ;
  *reasonp++ = a5;
  goto pt_16_135;
  };
  goto pt_16_135;
 pt_16_134:
  
# 1737 "wave.c" 3
 { q temp0 = (*((q *)((unsigned long)(
# 1737 "wave.c"
 a2
# 1737 "wave.c" 3
 ) - 0x0))); if(!(((unsigned long)(temp0) & 0x3) == 0x0)) { (
# 1737 "wave.c"
 a2
# 1737 "wave.c" 3
 ) = temp0; goto 
# 1737 "wave.c"
 pt_16_109
# 1737 "wave.c" 3
 ; } }
# 1737 "wave.c"
                             ;
  *reasonp++ = a2;
  goto pt_16_135;
 pt_16_135:
 pt_16_136:
  switch (
# 1742 "wave.c" 3
         ((unsigned long)(
# 1742 "wave.c"
         a1
# 1742 "wave.c" 3
         ) & 0x3)
# 1742 "wave.c"
                   ) {
 case 
# 1743 "wave.c" 3
     0x2
# 1743 "wave.c"
           :
 pt_16_137:
  if (a1 != 
# 1745 "wave.c" 3
           ((q)(((long)(
# 1745 "wave.c"
           0L
# 1745 "wave.c" 3
           ) << 4) + (0x2 +0x0)))
# 1745 "wave.c"
                      ) goto pt_16_164;
 pt_16_138:
  switch (
# 1747 "wave.c" 3
         ((unsigned long)(
# 1747 "wave.c"
         a5
# 1747 "wave.c" 3
         ) & 0x3)
# 1747 "wave.c"
                   ) {
 case 
# 1748 "wave.c" 3
     0x3
# 1748 "wave.c"
            :
 pt_16_139:
  if (!
# 1750 "wave.c" 3
      ((((unsigned long)((((struct functor *)((unsigned long)(
# 1750 "wave.c"
      a5
# 1750 "wave.c" 3
      ) - 0x3))->functor)) & 0x3) == 0x0))
# 1750 "wave.c"
                ) goto pt_16_164;
  if (!
# 1751 "wave.c" 3
      ((struct data_object_method_table *)((((struct functor *)((unsigned long)(
# 1751 "wave.c"
      a5
# 1751 "wave.c" 3
      ) - 0x3))->functor)) == &
# 1751 "wave.c"
      vector_g_data_method_table
# 1751 "wave.c" 3
      )
# 1751 "wave.c"
                        ) goto pt_16_164;
  
# 1752 "wave.c" 3
 { extern q size_of_vector(); 
# 1752 "wave.c"
 x0 
# 1752 "wave.c" 3
 = size_of_vector(
# 1752 "wave.c"
 a5
# 1752 "wave.c" 3
 ); }
# 1752 "wave.c"
                                     ;
 pt_16_140:
  if (x0 != 
# 1754 "wave.c" 3
           ((q)(((long)(
# 1754 "wave.c"
           2L
# 1754 "wave.c" 3
           ) << 4) + (0x2 +0x0)))
# 1754 "wave.c"
                      ) goto pt_16_164;
  x2 = 
# 1755 "wave.c" 3
      ((q)(((long)(
# 1755 "wave.c"
      0L
# 1755 "wave.c" 3
      ) << 4) + (0x2 +0x0)))
# 1755 "wave.c"
                 ;
  
# 1756 "wave.c" 3
 { extern q element_of_vector(); 
# 1756 "wave.c"
 x1 
# 1756 "wave.c" 3
 = element_of_vector(
# 1756 "wave.c"
 a5
# 1756 "wave.c" 3
 ,
# 1756 "wave.c"
 x2
# 1756 "wave.c" 3
 ); if ((long)
# 1756 "wave.c"
 x1 
# 1756 "wave.c" 3
 == 0) goto 
# 1756 "wave.c"
 pt_16_164
# 1756 "wave.c" 3
 ; }
# 1756 "wave.c"
                                           ;
  x4 = 
# 1757 "wave.c" 3
      ((q)(((long)(
# 1757 "wave.c"
      1L
# 1757 "wave.c" 3
      ) << 4) + (0x2 +0x0)))
# 1757 "wave.c"
                 ;
  
# 1758 "wave.c" 3
 { extern q element_of_vector(); 
# 1758 "wave.c"
 x3 
# 1758 "wave.c" 3
 = element_of_vector(
# 1758 "wave.c"
 a5
# 1758 "wave.c" 3
 ,
# 1758 "wave.c"
 x4
# 1758 "wave.c" 3
 ); if ((long)
# 1758 "wave.c"
 x3 
# 1758 "wave.c" 3
 == 0) goto 
# 1758 "wave.c"
 pt_16_164
# 1758 "wave.c" 3
 ; }
# 1758 "wave.c"
                                           ;
 pt_16_141:
  switch (
# 1760 "wave.c" 3
         ((unsigned long)(
# 1760 "wave.c"
         a6
# 1760 "wave.c" 3
         ) & 0x3)
# 1760 "wave.c"
                   ) {
 case 
# 1761 "wave.c" 3
     0x3
# 1761 "wave.c"
            :
 pt_16_142:
  if (!
# 1763 "wave.c" 3
      ((((unsigned long)((((struct functor *)((unsigned long)(
# 1763 "wave.c"
      a6
# 1763 "wave.c" 3
      ) - 0x3))->functor)) & 0x3) == 0x0))
# 1763 "wave.c"
                ) goto pt_16_164;
  if (!
# 1764 "wave.c" 3
      ((struct data_object_method_table *)((((struct functor *)((unsigned long)(
# 1764 "wave.c"
      a6
# 1764 "wave.c" 3
      ) - 0x3))->functor)) == &
# 1764 "wave.c"
      vector_g_data_method_table
# 1764 "wave.c" 3
      )
# 1764 "wave.c"
                        ) goto pt_16_164;
  
# 1765 "wave.c" 3
 { extern q size_of_vector(); 
# 1765 "wave.c"
 x5 
# 1765 "wave.c" 3
 = size_of_vector(
# 1765 "wave.c"
 a6
# 1765 "wave.c" 3
 ); }
# 1765 "wave.c"
                                     ;
 pt_16_143:
  if (x5 != 
# 1767 "wave.c" 3
           ((q)(((long)(
# 1767 "wave.c"
           2L
# 1767 "wave.c" 3
           ) << 4) + (0x2 +0x0)))
# 1767 "wave.c"
                      ) goto pt_16_164;
  x7 = 
# 1768 "wave.c" 3
      ((q)(((long)(
# 1768 "wave.c"
      0L
# 1768 "wave.c" 3
      ) << 4) + (0x2 +0x0)))
# 1768 "wave.c"
                 ;
  
# 1769 "wave.c" 3
 { extern q element_of_vector(); 
# 1769 "wave.c"
 x6 
# 1769 "wave.c" 3
 = element_of_vector(
# 1769 "wave.c"
 a6
# 1769 "wave.c" 3
 ,
# 1769 "wave.c"
 x7
# 1769 "wave.c" 3
 ); if ((long)
# 1769 "wave.c"
 x6 
# 1769 "wave.c" 3
 == 0) goto 
# 1769 "wave.c"
 pt_16_164
# 1769 "wave.c" 3
 ; }
# 1769 "wave.c"
                                           ;
  x9 = 
# 1770 "wave.c" 3
      ((q)(((long)(
# 1770 "wave.c"
      1L
# 1770 "wave.c" 3
      ) << 4) + (0x2 +0x0)))
# 1770 "wave.c"
                 ;
  
# 1771 "wave.c" 3
 { extern q element_of_vector(); 
# 1771 "wave.c"
 x8 
# 1771 "wave.c" 3
 = element_of_vector(
# 1771 "wave.c"
 a6
# 1771 "wave.c" 3
 ,
# 1771 "wave.c"
 x9
# 1771 "wave.c" 3
 ); if ((long)
# 1771 "wave.c"
 x8 
# 1771 "wave.c" 3
 == 0) goto 
# 1771 "wave.c"
 pt_16_164
# 1771 "wave.c" 3
 ; }
# 1771 "wave.c"
                                           ;
 pt_16_144:
  switch (
# 1773 "wave.c" 3
         ((unsigned long)(
# 1773 "wave.c"
         a7
# 1773 "wave.c" 3
         ) & 0x3)
# 1773 "wave.c"
                   ) {
 case 
# 1774 "wave.c" 3
     0x3
# 1774 "wave.c"
            :
 pt_16_145:
  if (!
# 1776 "wave.c" 3
      ((((unsigned long)((((struct functor *)((unsigned long)(
# 1776 "wave.c"
      a7
# 1776 "wave.c" 3
      ) - 0x3))->functor)) & 0x3) == 0x0))
# 1776 "wave.c"
                ) goto pt_16_164;
  if (!
# 1777 "wave.c" 3
      ((struct data_object_method_table *)((((struct functor *)((unsigned long)(
# 1777 "wave.c"
      a7
# 1777 "wave.c" 3
      ) - 0x3))->functor)) == &
# 1777 "wave.c"
      vector_g_data_method_table
# 1777 "wave.c" 3
      )
# 1777 "wave.c"
                        ) goto pt_16_164;
  
# 1778 "wave.c" 3
 { extern q size_of_vector(); 
# 1778 "wave.c"
 x10 
# 1778 "wave.c" 3
 = size_of_vector(
# 1778 "wave.c"
 a7
# 1778 "wave.c" 3
 ); }
# 1778 "wave.c"
                                      ;
 pt_16_146:
  if (x10 != 
# 1780 "wave.c" 3
            ((q)(((long)(
# 1780 "wave.c"
            2L
# 1780 "wave.c" 3
            ) << 4) + (0x2 +0x0)))
# 1780 "wave.c"
                       ) goto pt_16_164;
  x12 = 
# 1781 "wave.c" 3
       ((q)(((long)(
# 1781 "wave.c"
       0L
# 1781 "wave.c" 3
       ) << 4) + (0x2 +0x0)))
# 1781 "wave.c"
                  ;
  
# 1782 "wave.c" 3
 { extern q element_of_vector(); 
# 1782 "wave.c"
 x11 
# 1782 "wave.c" 3
 = element_of_vector(
# 1782 "wave.c"
 a7
# 1782 "wave.c" 3
 ,
# 1782 "wave.c"
 x12
# 1782 "wave.c" 3
 ); if ((long)
# 1782 "wave.c"
 x11 
# 1782 "wave.c" 3
 == 0) goto 
# 1782 "wave.c"
 pt_16_164
# 1782 "wave.c" 3
 ; }
# 1782 "wave.c"
                                             ;
  x14 = 
# 1783 "wave.c" 3
       ((q)(((long)(
# 1783 "wave.c"
       1L
# 1783 "wave.c" 3
       ) << 4) + (0x2 +0x0)))
# 1783 "wave.c"
                  ;
  
# 1784 "wave.c" 3
 { extern q element_of_vector(); 
# 1784 "wave.c"
 x13 
# 1784 "wave.c" 3
 = element_of_vector(
# 1784 "wave.c"
 a7
# 1784 "wave.c" 3
 ,
# 1784 "wave.c"
 x14
# 1784 "wave.c" 3
 ); if ((long)
# 1784 "wave.c"
 x13 
# 1784 "wave.c" 3
 == 0) goto 
# 1784 "wave.c"
 pt_16_164
# 1784 "wave.c" 3
 ; }
# 1784 "wave.c"
                                             ;
 pt_16_147:
  switch (
# 1786 "wave.c" 3
         ((unsigned long)(
# 1786 "wave.c"
         a8
# 1786 "wave.c" 3
         ) & 0x3)
# 1786 "wave.c"
                   ) {
 case 
# 1787 "wave.c" 3
     0x3
# 1787 "wave.c"
            :
 pt_16_148:
  if (!
# 1789 "wave.c" 3
      ((((unsigned long)((((struct functor *)((unsigned long)(
# 1789 "wave.c"
      a8
# 1789 "wave.c" 3
      ) - 0x3))->functor)) & 0x3) == 0x0))
# 1789 "wave.c"
                ) goto pt_16_164;
  if (!
# 1790 "wave.c" 3
      ((struct data_object_method_table *)((((struct functor *)((unsigned long)(
# 1790 "wave.c"
      a8
# 1790 "wave.c" 3
      ) - 0x3))->functor)) == &
# 1790 "wave.c"
      vector_g_data_method_table
# 1790 "wave.c" 3
      )
# 1790 "wave.c"
                        ) goto pt_16_164;
  
# 1791 "wave.c" 3
 { extern q size_of_vector(); 
# 1791 "wave.c"
 x15 
# 1791 "wave.c" 3
 = size_of_vector(
# 1791 "wave.c"
 a8
# 1791 "wave.c" 3
 ); }
# 1791 "wave.c"
                                      ;
 pt_16_149:
  if (x15 != 
# 1793 "wave.c" 3
            ((q)(((long)(
# 1793 "wave.c"
            2L
# 1793 "wave.c" 3
            ) << 4) + (0x2 +0x0)))
# 1793 "wave.c"
                       ) goto pt_16_164;
  x17 = 
# 1794 "wave.c" 3
       ((q)(((long)(
# 1794 "wave.c"
       0L
# 1794 "wave.c" 3
       ) << 4) + (0x2 +0x0)))
# 1794 "wave.c"
                  ;
  
# 1795 "wave.c" 3
 { extern q element_of_vector(); 
# 1795 "wave.c"
 x16 
# 1795 "wave.c" 3
 = element_of_vector(
# 1795 "wave.c"
 a8
# 1795 "wave.c" 3
 ,
# 1795 "wave.c"
 x17
# 1795 "wave.c" 3
 ); if ((long)
# 1795 "wave.c"
 x16 
# 1795 "wave.c" 3
 == 0) goto 
# 1795 "wave.c"
 pt_16_164
# 1795 "wave.c" 3
 ; }
# 1795 "wave.c"
                                             ;
  x19 = 
# 1796 "wave.c" 3
       ((q)(((long)(
# 1796 "wave.c"
       1L
# 1796 "wave.c" 3
       ) << 4) + (0x2 +0x0)))
# 1796 "wave.c"
                  ;
  
# 1797 "wave.c" 3
 { extern q element_of_vector(); 
# 1797 "wave.c"
 x18 
# 1797 "wave.c" 3
 = element_of_vector(
# 1797 "wave.c"
 a8
# 1797 "wave.c" 3
 ,
# 1797 "wave.c"
 x19
# 1797 "wave.c" 3
 ); if ((long)
# 1797 "wave.c"
 x18 
# 1797 "wave.c" 3
 == 0) goto 
# 1797 "wave.c"
 pt_16_164
# 1797 "wave.c" 3
 ; }
# 1797 "wave.c"
                                             ;
 pt_16_150:
  switch (
# 1799 "wave.c" 3
         ((unsigned long)(
# 1799 "wave.c"
         a9
# 1799 "wave.c" 3
         ) & 0x3)
# 1799 "wave.c"
                   ) {
 case 
# 1800 "wave.c" 3
     0x3
# 1800 "wave.c"
            :
 pt_16_151:
  if (!
# 1802 "wave.c" 3
      ((((unsigned long)((((struct functor *)((unsigned long)(
# 1802 "wave.c"
      a9
# 1802 "wave.c" 3
      ) - 0x3))->functor)) & 0x3) == 0x0))
# 1802 "wave.c"
                ) goto pt_16_164;
  if (!
# 1803 "wave.c" 3
      ((struct data_object_method_table *)((((struct functor *)((unsigned long)(
# 1803 "wave.c"
      a9
# 1803 "wave.c" 3
      ) - 0x3))->functor)) == &
# 1803 "wave.c"
      vector_g_data_method_table
# 1803 "wave.c" 3
      )
# 1803 "wave.c"
                        ) goto pt_16_164;
  
# 1804 "wave.c" 3
 { extern q size_of_vector(); 
# 1804 "wave.c"
 x20 
# 1804 "wave.c" 3
 = size_of_vector(
# 1804 "wave.c"
 a9
# 1804 "wave.c" 3
 ); }
# 1804 "wave.c"
                                      ;
 pt_16_152:
  if (x20 != 
# 1806 "wave.c" 3
            ((q)(((long)(
# 1806 "wave.c"
            2L
# 1806 "wave.c" 3
            ) << 4) + (0x2 +0x0)))
# 1806 "wave.c"
                       ) goto pt_16_164;
  x22 = 
# 1807 "wave.c" 3
       ((q)(((long)(
# 1807 "wave.c"
       0L
# 1807 "wave.c" 3
       ) << 4) + (0x2 +0x0)))
# 1807 "wave.c"
                  ;
  
# 1808 "wave.c" 3
 { extern q element_of_vector(); 
# 1808 "wave.c"
 x21 
# 1808 "wave.c" 3
 = element_of_vector(
# 1808 "wave.c"
 a9
# 1808 "wave.c" 3
 ,
# 1808 "wave.c"
 x22
# 1808 "wave.c" 3
 ); if ((long)
# 1808 "wave.c"
 x21 
# 1808 "wave.c" 3
 == 0) goto 
# 1808 "wave.c"
 pt_16_164
# 1808 "wave.c" 3
 ; }
# 1808 "wave.c"
                                             ;
  x24 = 
# 1809 "wave.c" 3
       ((q)(((long)(
# 1809 "wave.c"
       1L
# 1809 "wave.c" 3
       ) << 4) + (0x2 +0x0)))
# 1809 "wave.c"
                  ;
  
# 1810 "wave.c" 3
 { extern q element_of_vector(); 
# 1810 "wave.c"
 x23 
# 1810 "wave.c" 3
 = element_of_vector(
# 1810 "wave.c"
 a9
# 1810 "wave.c" 3
 ,
# 1810 "wave.c"
 x24
# 1810 "wave.c" 3
 ); if ((long)
# 1810 "wave.c"
 x23 
# 1810 "wave.c" 3
 == 0) goto 
# 1810 "wave.c"
 pt_16_164
# 1810 "wave.c" 3
 ; }
# 1810 "wave.c"
                                             ;
 pt_16_153:
  switch (
# 1812 "wave.c" 3
         ((unsigned long)(
# 1812 "wave.c"
         a10
# 1812 "wave.c" 3
         ) & 0x3)
# 1812 "wave.c"
                    ) {
 case 
# 1813 "wave.c" 3
     0x3
# 1813 "wave.c"
            :
 pt_16_154:
  if (!
# 1815 "wave.c" 3
      ((((unsigned long)((((struct functor *)((unsigned long)(
# 1815 "wave.c"
      a10
# 1815 "wave.c" 3
      ) - 0x3))->functor)) & 0x3) == 0x0))
# 1815 "wave.c"
                 ) goto pt_16_164;
  if (!
# 1816 "wave.c" 3
      ((struct data_object_method_table *)((((struct functor *)((unsigned long)(
# 1816 "wave.c"
      a10
# 1816 "wave.c" 3
      ) - 0x3))->functor)) == &
# 1816 "wave.c"
      vector_g_data_method_table
# 1816 "wave.c" 3
      )
# 1816 "wave.c"
                         ) goto pt_16_164;
  
# 1817 "wave.c" 3
 { extern q size_of_vector(); 
# 1817 "wave.c"
 x25 
# 1817 "wave.c" 3
 = size_of_vector(
# 1817 "wave.c"
 a10
# 1817 "wave.c" 3
 ); }
# 1817 "wave.c"
                                       ;
 pt_16_155:
  if (x25 != 
# 1819 "wave.c" 3
            ((q)(((long)(
# 1819 "wave.c"
            2L
# 1819 "wave.c" 3
            ) << 4) + (0x2 +0x0)))
# 1819 "wave.c"
                       ) goto pt_16_164;
  x27 = 
# 1820 "wave.c" 3
       ((q)(((long)(
# 1820 "wave.c"
       0L
# 1820 "wave.c" 3
       ) << 4) + (0x2 +0x0)))
# 1820 "wave.c"
                  ;
  
# 1821 "wave.c" 3
 { extern q element_of_vector(); 
# 1821 "wave.c"
 x26 
# 1821 "wave.c" 3
 = element_of_vector(
# 1821 "wave.c"
 a10
# 1821 "wave.c" 3
 ,
# 1821 "wave.c"
 x27
# 1821 "wave.c" 3
 ); if ((long)
# 1821 "wave.c"
 x26 
# 1821 "wave.c" 3
 == 0) goto 
# 1821 "wave.c"
 pt_16_164
# 1821 "wave.c" 3
 ; }
# 1821 "wave.c"
                                              ;
  x29 = 
# 1822 "wave.c" 3
       ((q)(((long)(
# 1822 "wave.c"
       1L
# 1822 "wave.c" 3
       ) << 4) + (0x2 +0x0)))
# 1822 "wave.c"
                  ;
  
# 1823 "wave.c" 3
 { extern q element_of_vector(); 
# 1823 "wave.c"
 x28 
# 1823 "wave.c" 3
 = element_of_vector(
# 1823 "wave.c"
 a10
# 1823 "wave.c" 3
 ,
# 1823 "wave.c"
 x29
# 1823 "wave.c" 3
 ); if ((long)
# 1823 "wave.c"
 x28 
# 1823 "wave.c" 3
 == 0) goto 
# 1823 "wave.c"
 pt_16_164
# 1823 "wave.c" 3
 ; }
# 1823 "wave.c"
                                              ;
 pt_16_156:
  if (!
# 1825 "wave.c" 3
      (((unsigned long)(
# 1825 "wave.c"
      a0
# 1825 "wave.c" 3
      ) & 0xFL) == (0x2 +0x0))
# 1825 "wave.c"
               ) goto pt_16_163;
  x30 = 
# 1826 "wave.c" 3
       ((q)(((long)(
# 1826 "wave.c"
       1L
# 1826 "wave.c" 3
       ) << 4) + (0x2 +0x0)))
# 1826 "wave.c"
                  ;
  
# 1827 "wave.c" 3
 if ((long)(
# 1827 "wave.c"
 x30
# 1827 "wave.c" 3
 ) > (long)(
# 1827 "wave.c"
 a0
# 1827 "wave.c" 3
 )) goto 
# 1827 "wave.c"
 pt_16_164
# 1827 "wave.c" 3
 ;
# 1827 "wave.c"
                                  ;
 pt_16_157:
  if (!
# 1829 "wave.c" 3
      (((unsigned long)(
# 1829 "wave.c"
      a3
# 1829 "wave.c" 3
      ) & 0xFL) == (0x2 +0x0))
# 1829 "wave.c"
               ) goto pt_16_162;
  
# 1830 "wave.c" 3
 if ((long)(
# 1830 "wave.c"
 a0
# 1830 "wave.c" 3
 ) > (long)(
# 1830 "wave.c"
 a3
# 1830 "wave.c" 3
 )) goto 
# 1830 "wave.c"
 pt_16_164
# 1830 "wave.c" 3
 ;
# 1830 "wave.c"
                                 ;
 pt_16_158:
  if (!
# 1832 "wave.c" 3
      (((unsigned long)(
# 1832 "wave.c"
      a2
# 1832 "wave.c" 3
      ) & 0xFL) == (0x2 +0x0))
# 1832 "wave.c"
               ) goto pt_16_161;
  x31 = 
# 1833 "wave.c" 3
       ((q)(((long)(
# 1833 "wave.c"
       0L
# 1833 "wave.c" 3
       ) << 4) + (0x2 +0x0)))
# 1833 "wave.c"
                  ;
  
# 1834 "wave.c" 3
 if ((long)(
# 1834 "wave.c"
 x31
# 1834 "wave.c" 3
 ) > (long)(
# 1834 "wave.c"
 a2
# 1834 "wave.c" 3
 )) goto 
# 1834 "wave.c"
 pt_16_164
# 1834 "wave.c" 3
 ;
# 1834 "wave.c"
                                  ;
 pt_16_159:
  if (!
# 1836 "wave.c" 3
      (((unsigned long)(
# 1836 "wave.c"
      a4
# 1836 "wave.c" 3
      ) & 0xFL) == (0x2 +0x0))
# 1836 "wave.c"
               ) goto pt_16_160;
  
# 1837 "wave.c" 3
 if ((long)(
# 1837 "wave.c"
 a2
# 1837 "wave.c" 3
 ) > (long)(
# 1837 "wave.c"
 a4
# 1837 "wave.c" 3
 )) goto 
# 1837 "wave.c"
 pt_16_164
# 1837 "wave.c" 3
 ;
# 1837 "wave.c"
                                 ;
  x32 = 
# 1838 "wave.c" 3
       ((q)(((long)(
# 1838 "wave.c"
       2L
# 1838 "wave.c" 3
       ) << 4) + (0x2 +0x0)))
# 1838 "wave.c"
                  ;
  
# 1839 "wave.c" 3
 { 
# 1839 "wave.c"
 x33 
# 1839 "wave.c" 3
 = (q)((unsigned long)(
# 1839 "wave.c"
 a3
# 1839 "wave.c" 3
 ) - ((unsigned long)(
# 1839 "wave.c"
 x32
# 1839 "wave.c" 3
 ) - (0x2 +0x0))); }
# 1839 "wave.c"
                              ;
  allocp[0] = 
# 1840 "wave.c" 3
             ((q)(((unsigned long)(
# 1840 "wave.c"
             190L+
# 1840 "wave.c" 3
             0) << 4) + (0x2 +0x4)))
# 1840 "wave.c"
                                    ;
  allocp[1] = x33;
  allocp[2] = x35 = 
# 1842 "wave.c" 3
                   (q) ((unsigned long)(
# 1842 "wave.c"
                   &allocp[2]
# 1842 "wave.c" 3
                   ) + 0x0)
# 1842 "wave.c"
                                      ;
  allocp[3] = x36 = 
# 1843 "wave.c" 3
                   (q) ((unsigned long)(
# 1843 "wave.c"
                   &allocp[3]
# 1843 "wave.c" 3
                   ) + 0x0)
# 1843 "wave.c"
                                      ;
  x34 = 
# 1844 "wave.c" 3
       ((q)((unsigned long)(
# 1844 "wave.c"
       &allocp[0]
# 1844 "wave.c" 3
       ) + 0x3))
# 1844 "wave.c"
                              ;
  allocp[4] = x38 = 
# 1845 "wave.c" 3
                   (q) ((unsigned long)(
# 1845 "wave.c"
                   &allocp[4]
# 1845 "wave.c" 3
                   ) + 0x0)
# 1845 "wave.c"
                                      ;
  allocp[5] = x34;
  x37 = 
# 1847 "wave.c" 3
       ((q)((unsigned long)(
# 1847 "wave.c"
       &allocp[4]
# 1847 "wave.c" 3
       ) + 0x1))
# 1847 "wave.c"
                           ;
  allocp += 6;
  
# 1849 "wave.c" 3
 { if (!(((unsigned long)(
# 1849 "wave.c"
 x3
# 1849 "wave.c" 3
 ) & 0x3) == 0x0) || (*((q *)((unsigned long)(
# 1849 "wave.c"
 x3
# 1849 "wave.c" 3
 ) - 0x0))) != (
# 1849 "wave.c"
 x3
# 1849 "wave.c" 3
 )) { allocp = do_unify_value(allocp, (
# 1849 "wave.c"
 x3
# 1849 "wave.c" 3
 ), (
# 1849 "wave.c"
 x37
# 1849 "wave.c" 3
 )); } else { (*((q *)((unsigned long)(
# 1849 "wave.c"
 x3
# 1849 "wave.c" 3
 ) - 0x0))) = (
# 1849 "wave.c"
 x37
# 1849 "wave.c" 3
 ); } }
# 1849 "wave.c"
                     ;
  allocp[0] = 
# 1850 "wave.c" 3
             ((q)(((unsigned long)(0) << 4) + (0x2 +0x4)))
# 1850 "wave.c"
                    ;
  allocp[1] = x35;
  x39 = 
# 1852 "wave.c" 3
       ((q)((unsigned long)(
# 1852 "wave.c"
       &allocp[0]
# 1852 "wave.c" 3
       ) + 0x1))
# 1852 "wave.c"
                           ;
  allocp[2] = (q)qp;
  allocp[3] = (q)(&predicate_main_xnode_7);
  allocp[4] = a0;
  allocp[5] = 
# 1856 "wave.c" 3
             ((q)(((long)(
# 1856 "wave.c"
             0L
# 1856 "wave.c" 3
             ) << 4) + (0x2 +0x0)))
# 1856 "wave.c"
                        ;
  allocp[6] = a2;
  allocp[7] = x39;
  allocp[8] = x40 = 
# 1859 "wave.c" 3
                   (q) ((unsigned long)(
# 1859 "wave.c"
                   &allocp[8]
# 1859 "wave.c" 3
                   ) + 0x0)
# 1859 "wave.c"
                                      ;
  allocp[9] = a11;
  allocp[10] = a12;
  allocp[11] = (q)(struct goalrec*)&allocp[2];
  allocp[12] = (q)(&predicate_main_xroute_14);
  allocp[13] = a15;
  allocp[14] = x1;
  allocp[15] = x6;
  allocp[16] = x11;
  allocp[17] = x16;
  allocp[18] = x21;
  allocp[19] = x26;
  allocp[20] = x38;
  allocp[21] = x8;
  allocp[22] = x13;
  allocp[23] = x18;
  allocp[24] = x23;
  allocp[25] = x28;
  allocp[26] = x40;
  a0 = x36;
  a1 = a13;
  a2 = a14;
  qp = (struct goalrec*)&allocp[11];
  allocp += 27;
  
# 1883 "wave.c" 3
 { if (allocp < (glbl->heaplimit0)) goto 
# 1883 "wave.c"
 acknowledged_3_clear_reason
# 1883 "wave.c" 3
 ;}
# 1883 "wave.c"
                                     ;
  goto acknowledged_3_ext_interrupt;
 pt_16_160:
  if (!
# 1886 "wave.c" 3
      (((unsigned long)(
# 1886 "wave.c"
      a4
# 1886 "wave.c" 3
      ) & 0x3) == 0x0)
# 1886 "wave.c"
               ) goto pt_16_164;
  
# 1887 "wave.c" 3
 { q temp0 = (*((q *)((unsigned long)(
# 1887 "wave.c"
 a4
# 1887 "wave.c" 3
 ) - 0x0))); if(!(((unsigned long)(temp0) & 0x3) == 0x0)) { (
# 1887 "wave.c"
 a4
# 1887 "wave.c" 3
 ) = temp0; goto 
# 1887 "wave.c"
 pt_16_159
# 1887 "wave.c" 3
 ; } }
# 1887 "wave.c"
                             ;
  *reasonp++ = a4;
  goto pt_16_164;
 pt_16_161:
  if (!
# 1891 "wave.c" 3
      (((unsigned long)(
# 1891 "wave.c"
      a2
# 1891 "wave.c" 3
      ) & 0x3) == 0x0)
# 1891 "wave.c"
               ) goto pt_16_164;
  
# 1892 "wave.c" 3
 { q temp0 = (*((q *)((unsigned long)(
# 1892 "wave.c"
 a2
# 1892 "wave.c" 3
 ) - 0x0))); if(!(((unsigned long)(temp0) & 0x3) == 0x0)) { (
# 1892 "wave.c"
 a2
# 1892 "wave.c" 3
 ) = temp0; goto 
# 1892 "wave.c"
 pt_16_158
# 1892 "wave.c" 3
 ; } }
# 1892 "wave.c"
                             ;
  *reasonp++ = a2;
  goto pt_16_164;
 pt_16_162:
  if (!
# 1896 "wave.c" 3
      (((unsigned long)(
# 1896 "wave.c"
      a3
# 1896 "wave.c" 3
      ) & 0x3) == 0x0)
# 1896 "wave.c"
               ) goto pt_16_164;
  
# 1897 "wave.c" 3
 { q temp0 = (*((q *)((unsigned long)(
# 1897 "wave.c"
 a3
# 1897 "wave.c" 3
 ) - 0x0))); if(!(((unsigned long)(temp0) & 0x3) == 0x0)) { (
# 1897 "wave.c"
 a3
# 1897 "wave.c" 3
 ) = temp0; goto 
# 1897 "wave.c"
 pt_16_157
# 1897 "wave.c" 3
 ; } }
# 1897 "wave.c"
                             ;
  *reasonp++ = a3;
  goto pt_16_164;
 pt_16_163:
  if (!
# 1901 "wave.c" 3
      (((unsigned long)(
# 1901 "wave.c"
      a0
# 1901 "wave.c" 3
      ) & 0x3) == 0x0)
# 1901 "wave.c"
               ) goto pt_16_164;
  
# 1902 "wave.c" 3
 { q temp0 = (*((q *)((unsigned long)(
# 1902 "wave.c"
 a0
# 1902 "wave.c" 3
 ) - 0x0))); if(!(((unsigned long)(temp0) & 0x3) == 0x0)) { (
# 1902 "wave.c"
 a0
# 1902 "wave.c" 3
 ) = temp0; goto 
# 1902 "wave.c"
 pt_16_156
# 1902 "wave.c" 3
 ; } }
# 1902 "wave.c"
                             ;
  *reasonp++ = a0;
  goto pt_16_164;
 case 
# 1905 "wave.c" 3
     0x0
# 1905 "wave.c"
           :
  
# 1906 "wave.c" 3
 { q temp0 = (*((q *)((unsigned long)(
# 1906 "wave.c"
 a10
# 1906 "wave.c" 3
 ) - 0x0))); if(!(((unsigned long)(temp0) & 0x3) == 0x0)) { (
# 1906 "wave.c"
 a10
# 1906 "wave.c" 3
 ) = temp0; goto 
# 1906 "wave.c"
 pt_16_153
# 1906 "wave.c" 3
 ; } }
# 1906 "wave.c"
                              ;
  *reasonp++ = a10;
  goto pt_16_164;
  };
  goto pt_16_164;
 case 
# 1911 "wave.c" 3
     0x0
# 1911 "wave.c"
           :
  
# 1912 "wave.c" 3
 { q temp0 = (*((q *)((unsigned long)(
# 1912 "wave.c"
 a9
# 1912 "wave.c" 3
 ) - 0x0))); if(!(((unsigned long)(temp0) & 0x3) == 0x0)) { (
# 1912 "wave.c"
 a9
# 1912 "wave.c" 3
 ) = temp0; goto 
# 1912 "wave.c"
 pt_16_150
# 1912 "wave.c" 3
 ; } }
# 1912 "wave.c"
                             ;
  *reasonp++ = a9;
  goto pt_16_164;
  };
  goto pt_16_164;
 case 
# 1917 "wave.c" 3
     0x0
# 1917 "wave.c"
           :
  
# 1918 "wave.c" 3
 { q temp0 = (*((q *)((unsigned long)(
# 1918 "wave.c"
 a8
# 1918 "wave.c" 3
 ) - 0x0))); if(!(((unsigned long)(temp0) & 0x3) == 0x0)) { (
# 1918 "wave.c"
 a8
# 1918 "wave.c" 3
 ) = temp0; goto 
# 1918 "wave.c"
 pt_16_147
# 1918 "wave.c" 3
 ; } }
# 1918 "wave.c"
                             ;
  *reasonp++ = a8;
  goto pt_16_164;
  };
  goto pt_16_164;
 case 
# 1923 "wave.c" 3
     0x0
# 1923 "wave.c"
           :
  
# 1924 "wave.c" 3
 { q temp0 = (*((q *)((unsigned long)(
# 1924 "wave.c"
 a7
# 1924 "wave.c" 3
 ) - 0x0))); if(!(((unsigned long)(temp0) & 0x3) == 0x0)) { (
# 1924 "wave.c"
 a7
# 1924 "wave.c" 3
 ) = temp0; goto 
# 1924 "wave.c"
 pt_16_144
# 1924 "wave.c" 3
 ; } }
# 1924 "wave.c"
                             ;
  *reasonp++ = a7;
  goto pt_16_164;
  };
  goto pt_16_164;
 case 
# 1929 "wave.c" 3
     0x0
# 1929 "wave.c"
           :
  
# 1930 "wave.c" 3
 { q temp0 = (*((q *)((unsigned long)(
# 1930 "wave.c"
 a6
# 1930 "wave.c" 3
 ) - 0x0))); if(!(((unsigned long)(temp0) & 0x3) == 0x0)) { (
# 1930 "wave.c"
 a6
# 1930 "wave.c" 3
 ) = temp0; goto 
# 1930 "wave.c"
 pt_16_141
# 1930 "wave.c" 3
 ; } }
# 1930 "wave.c"
                             ;
  *reasonp++ = a6;
  goto pt_16_164;
  };
  goto pt_16_164;
 case 
# 1935 "wave.c" 3
     0x0
# 1935 "wave.c"
           :
  
# 1936 "wave.c" 3
 { q temp0 = (*((q *)((unsigned long)(
# 1936 "wave.c"
 a5
# 1936 "wave.c" 3
 ) - 0x0))); if(!(((unsigned long)(temp0) & 0x3) == 0x0)) { (
# 1936 "wave.c"
 a5
# 1936 "wave.c" 3
 ) = temp0; goto 
# 1936 "wave.c"
 pt_16_138
# 1936 "wave.c" 3
 ; } }
# 1936 "wave.c"
                             ;
  *reasonp++ = a5;
  goto pt_16_164;
  };
  goto pt_16_164;
 case 
# 1941 "wave.c" 3
     0x0
# 1941 "wave.c"
           :
  
# 1942 "wave.c" 3
 { q temp0 = (*((q *)((unsigned long)(
# 1942 "wave.c"
 a1
# 1942 "wave.c" 3
 ) - 0x0))); if(!(((unsigned long)(temp0) & 0x3) == 0x0)) { (
# 1942 "wave.c"
 a1
# 1942 "wave.c" 3
 ) = temp0; goto 
# 1942 "wave.c"
 pt_16_136
# 1942 "wave.c" 3
 ; } }
# 1942 "wave.c"
                             ;
  *reasonp++ = a1;
  goto pt_16_interrupt;
  };
  goto pt_16_164;
 pt_16_164:
 pt_16_165:
  if (
# 1949 "wave.c" 3
     (((unsigned long)(
# 1949 "wave.c"
     a1
# 1949 "wave.c" 3
     ) & 0x3) == 0x0)
# 1949 "wave.c"
              ) goto pt_16_192;
  
# 1950 "wave.c" 3
 if ((
# 1950 "wave.c"
 a1
# 1950 "wave.c" 3
 )!=(
# 1950 "wave.c"
 a4
# 1950 "wave.c" 3
 ) || (((unsigned long)(
# 1950 "wave.c"
 a1
# 1950 "wave.c" 3
 ) & 0x3) == 0x0)) { q retval; switch((long)(retval = eq_terms_body((
# 1950 "wave.c"
 a1
# 1950 "wave.c" 3
 ), (
# 1950 "wave.c"
 a4
# 1950 "wave.c" 3
 )))){ case 0: break; default: *reasonp++ = retval; case 1: goto 
# 1950 "wave.c"
 pt_16_interrupt
# 1950 "wave.c" 3
 ; } }
# 1950 "wave.c"
                                      ;
 pt_16_166:
  switch (
# 1952 "wave.c" 3
         ((unsigned long)(
# 1952 "wave.c"
         a5
# 1952 "wave.c" 3
         ) & 0x3)
# 1952 "wave.c"
                   ) {
 case 
# 1953 "wave.c" 3
     0x3
# 1953 "wave.c"
            :
 pt_16_167:
  if (!
# 1955 "wave.c" 3
      ((((unsigned long)((((struct functor *)((unsigned long)(
# 1955 "wave.c"
      a5
# 1955 "wave.c" 3
      ) - 0x3))->functor)) & 0x3) == 0x0))
# 1955 "wave.c"
                ) goto pt_16_interrupt;
  if (!
# 1956 "wave.c" 3
      ((struct data_object_method_table *)((((struct functor *)((unsigned long)(
# 1956 "wave.c"
      a5
# 1956 "wave.c" 3
      ) - 0x3))->functor)) == &
# 1956 "wave.c"
      vector_g_data_method_table
# 1956 "wave.c" 3
      )
# 1956 "wave.c"
                        ) goto pt_16_interrupt;
  
# 1957 "wave.c" 3
 { extern q size_of_vector(); 
# 1957 "wave.c"
 x0 
# 1957 "wave.c" 3
 = size_of_vector(
# 1957 "wave.c"
 a5
# 1957 "wave.c" 3
 ); }
# 1957 "wave.c"
                                           ;
 pt_16_168:
  if (x0 != 
# 1959 "wave.c" 3
           ((q)(((long)(
# 1959 "wave.c"
           2L
# 1959 "wave.c" 3
           ) << 4) + (0x2 +0x0)))
# 1959 "wave.c"
                      ) goto pt_16_interrupt;
  x2 = 
# 1960 "wave.c" 3
      ((q)(((long)(
# 1960 "wave.c"
      0L
# 1960 "wave.c" 3
      ) << 4) + (0x2 +0x0)))
# 1960 "wave.c"
                 ;
  
# 1961 "wave.c" 3
 { extern q element_of_vector(); 
# 1961 "wave.c"
 x1 
# 1961 "wave.c" 3
 = element_of_vector(
# 1961 "wave.c"
 a5
# 1961 "wave.c" 3
 ,
# 1961 "wave.c"
 x2
# 1961 "wave.c" 3
 ); if ((long)
# 1961 "wave.c"
 x1 
# 1961 "wave.c" 3
 == 0) goto 
# 1961 "wave.c"
 pt_16_interrupt
# 1961 "wave.c" 3
 ; }
# 1961 "wave.c"
                                                 ;
  x4 = 
# 1962 "wave.c" 3
      ((q)(((long)(
# 1962 "wave.c"
      1L
# 1962 "wave.c" 3
      ) << 4) + (0x2 +0x0)))
# 1962 "wave.c"
                 ;
  
# 1963 "wave.c" 3
 { extern q element_of_vector(); 
# 1963 "wave.c"
 x3 
# 1963 "wave.c" 3
 = element_of_vector(
# 1963 "wave.c"
 a5
# 1963 "wave.c" 3
 ,
# 1963 "wave.c"
 x4
# 1963 "wave.c" 3
 ); if ((long)
# 1963 "wave.c"
 x3 
# 1963 "wave.c" 3
 == 0) goto 
# 1963 "wave.c"
 pt_16_interrupt
# 1963 "wave.c" 3
 ; }
# 1963 "wave.c"
                                                 ;
 pt_16_169:
  switch (
# 1965 "wave.c" 3
         ((unsigned long)(
# 1965 "wave.c"
         a6
# 1965 "wave.c" 3
         ) & 0x3)
# 1965 "wave.c"
                   ) {
 case 
# 1966 "wave.c" 3
     0x3
# 1966 "wave.c"
            :
 pt_16_170:
  if (!
# 1968 "wave.c" 3
      ((((unsigned long)((((struct functor *)((unsigned long)(
# 1968 "wave.c"
      a6
# 1968 "wave.c" 3
      ) - 0x3))->functor)) & 0x3) == 0x0))
# 1968 "wave.c"
                ) goto pt_16_interrupt;
  if (!
# 1969 "wave.c" 3
      ((struct data_object_method_table *)((((struct functor *)((unsigned long)(
# 1969 "wave.c"
      a6
# 1969 "wave.c" 3
      ) - 0x3))->functor)) == &
# 1969 "wave.c"
      vector_g_data_method_table
# 1969 "wave.c" 3
      )
# 1969 "wave.c"
                        ) goto pt_16_interrupt;
  
# 1970 "wave.c" 3
 { extern q size_of_vector(); 
# 1970 "wave.c"
 x5 
# 1970 "wave.c" 3
 = size_of_vector(
# 1970 "wave.c"
 a6
# 1970 "wave.c" 3
 ); }
# 1970 "wave.c"
                                           ;
 pt_16_171:
  if (x5 != 
# 1972 "wave.c" 3
           ((q)(((long)(
# 1972 "wave.c"
           2L
# 1972 "wave.c" 3
           ) << 4) + (0x2 +0x0)))
# 1972 "wave.c"
                      ) goto pt_16_interrupt;
  x7 = 
# 1973 "wave.c" 3
      ((q)(((long)(
# 1973 "wave.c"
      0L
# 1973 "wave.c" 3
      ) << 4) + (0x2 +0x0)))
# 1973 "wave.c"
                 ;
  
# 1974 "wave.c" 3
 { extern q element_of_vector(); 
# 1974 "wave.c"
 x6 
# 1974 "wave.c" 3
 = element_of_vector(
# 1974 "wave.c"
 a6
# 1974 "wave.c" 3
 ,
# 1974 "wave.c"
 x7
# 1974 "wave.c" 3
 ); if ((long)
# 1974 "wave.c"
 x6 
# 1974 "wave.c" 3
 == 0) goto 
# 1974 "wave.c"
 pt_16_interrupt
# 1974 "wave.c" 3
 ; }
# 1974 "wave.c"
                                                 ;
  x9 = 
# 1975 "wave.c" 3
      ((q)(((long)(
# 1975 "wave.c"
      1L
# 1975 "wave.c" 3
      ) << 4) + (0x2 +0x0)))
# 1975 "wave.c"
                 ;
  
# 1976 "wave.c" 3
 { extern q element_of_vector(); 
# 1976 "wave.c"
 x8 
# 1976 "wave.c" 3
 = element_of_vector(
# 1976 "wave.c"
 a6
# 1976 "wave.c" 3
 ,
# 1976 "wave.c"
 x9
# 1976 "wave.c" 3
 ); if ((long)
# 1976 "wave.c"
 x8 
# 1976 "wave.c" 3
 == 0) goto 
# 1976 "wave.c"
 pt_16_interrupt
# 1976 "wave.c" 3
 ; }
# 1976 "wave.c"
                                                 ;
 pt_16_172:
  switch (
# 1978 "wave.c" 3
         ((unsigned long)(
# 1978 "wave.c"
         a7
# 1978 "wave.c" 3
         ) & 0x3)
# 1978 "wave.c"
                   ) {
 case 
# 1979 "wave.c" 3
     0x3
# 1979 "wave.c"
            :
 pt_16_173:
  if (!
# 1981 "wave.c" 3
      ((((unsigned long)((((struct functor *)((unsigned long)(
# 1981 "wave.c"
      a7
# 1981 "wave.c" 3
      ) - 0x3))->functor)) & 0x3) == 0x0))
# 1981 "wave.c"
                ) goto pt_16_interrupt;
  if (!
# 1982 "wave.c" 3
      ((struct data_object_method_table *)((((struct functor *)((unsigned long)(
# 1982 "wave.c"
      a7
# 1982 "wave.c" 3
      ) - 0x3))->functor)) == &
# 1982 "wave.c"
      vector_g_data_method_table
# 1982 "wave.c" 3
      )
# 1982 "wave.c"
                        ) goto pt_16_interrupt;
  
# 1983 "wave.c" 3
 { extern q size_of_vector(); 
# 1983 "wave.c"
 x10 
# 1983 "wave.c" 3
 = size_of_vector(
# 1983 "wave.c"
 a7
# 1983 "wave.c" 3
 ); }
# 1983 "wave.c"
                                            ;
 pt_16_174:
  if (x10 != 
# 1985 "wave.c" 3
            ((q)(((long)(
# 1985 "wave.c"
            2L
# 1985 "wave.c" 3
            ) << 4) + (0x2 +0x0)))
# 1985 "wave.c"
                       ) goto pt_16_interrupt;
  x12 = 
# 1986 "wave.c" 3
       ((q)(((long)(
# 1986 "wave.c"
       0L
# 1986 "wave.c" 3
       ) << 4) + (0x2 +0x0)))
# 1986 "wave.c"
                  ;
  
# 1987 "wave.c" 3
 { extern q element_of_vector(); 
# 1987 "wave.c"
 x11 
# 1987 "wave.c" 3
 = element_of_vector(
# 1987 "wave.c"
 a7
# 1987 "wave.c" 3
 ,
# 1987 "wave.c"
 x12
# 1987 "wave.c" 3
 ); if ((long)
# 1987 "wave.c"
 x11 
# 1987 "wave.c" 3
 == 0) goto 
# 1987 "wave.c"
 pt_16_interrupt
# 1987 "wave.c" 3
 ; }
# 1987 "wave.c"
                                                   ;
  x14 = 
# 1988 "wave.c" 3
       ((q)(((long)(
# 1988 "wave.c"
       1L
# 1988 "wave.c" 3
       ) << 4) + (0x2 +0x0)))
# 1988 "wave.c"
                  ;
  
# 1989 "wave.c" 3
 { extern q element_of_vector(); 
# 1989 "wave.c"
 x13 
# 1989 "wave.c" 3
 = element_of_vector(
# 1989 "wave.c"
 a7
# 1989 "wave.c" 3
 ,
# 1989 "wave.c"
 x14
# 1989 "wave.c" 3
 ); if ((long)
# 1989 "wave.c"
 x13 
# 1989 "wave.c" 3
 == 0) goto 
# 1989 "wave.c"
 pt_16_interrupt
# 1989 "wave.c" 3
 ; }
# 1989 "wave.c"
                                                   ;
 pt_16_175:
  switch (
# 1991 "wave.c" 3
         ((unsigned long)(
# 1991 "wave.c"
         a8
# 1991 "wave.c" 3
         ) & 0x3)
# 1991 "wave.c"
                   ) {
 case 
# 1992 "wave.c" 3
     0x3
# 1992 "wave.c"
            :
 pt_16_176:
  if (!
# 1994 "wave.c" 3
      ((((unsigned long)((((struct functor *)((unsigned long)(
# 1994 "wave.c"
      a8
# 1994 "wave.c" 3
      ) - 0x3))->functor)) & 0x3) == 0x0))
# 1994 "wave.c"
                ) goto pt_16_interrupt;
  if (!
# 1995 "wave.c" 3
      ((struct data_object_method_table *)((((struct functor *)((unsigned long)(
# 1995 "wave.c"
      a8
# 1995 "wave.c" 3
      ) - 0x3))->functor)) == &
# 1995 "wave.c"
      vector_g_data_method_table
# 1995 "wave.c" 3
      )
# 1995 "wave.c"
                        ) goto pt_16_interrupt;
  
# 1996 "wave.c" 3
 { extern q size_of_vector(); 
# 1996 "wave.c"
 x15 
# 1996 "wave.c" 3
 = size_of_vector(
# 1996 "wave.c"
 a8
# 1996 "wave.c" 3
 ); }
# 1996 "wave.c"
                                            ;
 pt_16_177:
  if (x15 != 
# 1998 "wave.c" 3
            ((q)(((long)(
# 1998 "wave.c"
            2L
# 1998 "wave.c" 3
            ) << 4) + (0x2 +0x0)))
# 1998 "wave.c"
                       ) goto pt_16_interrupt;
  x17 = 
# 1999 "wave.c" 3
       ((q)(((long)(
# 1999 "wave.c"
       0L
# 1999 "wave.c" 3
       ) << 4) + (0x2 +0x0)))
# 1999 "wave.c"
                  ;
  
# 2000 "wave.c" 3
 { extern q element_of_vector(); 
# 2000 "wave.c"
 x16 
# 2000 "wave.c" 3
 = element_of_vector(
# 2000 "wave.c"
 a8
# 2000 "wave.c" 3
 ,
# 2000 "wave.c"
 x17
# 2000 "wave.c" 3
 ); if ((long)
# 2000 "wave.c"
 x16 
# 2000 "wave.c" 3
 == 0) goto 
# 2000 "wave.c"
 pt_16_interrupt
# 2000 "wave.c" 3
 ; }
# 2000 "wave.c"
                                                   ;
  x19 = 
# 2001 "wave.c" 3
       ((q)(((long)(
# 2001 "wave.c"
       1L
# 2001 "wave.c" 3
       ) << 4) + (0x2 +0x0)))
# 2001 "wave.c"
                  ;
  
# 2002 "wave.c" 3
 { extern q element_of_vector(); 
# 2002 "wave.c"
 x18 
# 2002 "wave.c" 3
 = element_of_vector(
# 2002 "wave.c"
 a8
# 2002 "wave.c" 3
 ,
# 2002 "wave.c"
 x19
# 2002 "wave.c" 3
 ); if ((long)
# 2002 "wave.c"
 x18 
# 2002 "wave.c" 3
 == 0) goto 
# 2002 "wave.c"
 pt_16_interrupt
# 2002 "wave.c" 3
 ; }
# 2002 "wave.c"
                                                   ;
 pt_16_178:
  switch (
# 2004 "wave.c" 3
         ((unsigned long)(
# 2004 "wave.c"
         a9
# 2004 "wave.c" 3
         ) & 0x3)
# 2004 "wave.c"
                   ) {
 case 
# 2005 "wave.c" 3
     0x3
# 2005 "wave.c"
            :
 pt_16_179:
  if (!
# 2007 "wave.c" 3
      ((((unsigned long)((((struct functor *)((unsigned long)(
# 2007 "wave.c"
      a9
# 2007 "wave.c" 3
      ) - 0x3))->functor)) & 0x3) == 0x0))
# 2007 "wave.c"
                ) goto pt_16_interrupt;
  if (!
# 2008 "wave.c" 3
      ((struct data_object_method_table *)((((struct functor *)((unsigned long)(
# 2008 "wave.c"
      a9
# 2008 "wave.c" 3
      ) - 0x3))->functor)) == &
# 2008 "wave.c"
      vector_g_data_method_table
# 2008 "wave.c" 3
      )
# 2008 "wave.c"
                        ) goto pt_16_interrupt;
  
# 2009 "wave.c" 3
 { extern q size_of_vector(); 
# 2009 "wave.c"
 x20 
# 2009 "wave.c" 3
 = size_of_vector(
# 2009 "wave.c"
 a9
# 2009 "wave.c" 3
 ); }
# 2009 "wave.c"
                                            ;
 pt_16_180:
  if (x20 != 
# 2011 "wave.c" 3
            ((q)(((long)(
# 2011 "wave.c"
            2L
# 2011 "wave.c" 3
            ) << 4) + (0x2 +0x0)))
# 2011 "wave.c"
                       ) goto pt_16_interrupt;
  x22 = 
# 2012 "wave.c" 3
       ((q)(((long)(
# 2012 "wave.c"
       0L
# 2012 "wave.c" 3
       ) << 4) + (0x2 +0x0)))
# 2012 "wave.c"
                  ;
  
# 2013 "wave.c" 3
 { extern q element_of_vector(); 
# 2013 "wave.c"
 x21 
# 2013 "wave.c" 3
 = element_of_vector(
# 2013 "wave.c"
 a9
# 2013 "wave.c" 3
 ,
# 2013 "wave.c"
 x22
# 2013 "wave.c" 3
 ); if ((long)
# 2013 "wave.c"
 x21 
# 2013 "wave.c" 3
 == 0) goto 
# 2013 "wave.c"
 pt_16_interrupt
# 2013 "wave.c" 3
 ; }
# 2013 "wave.c"
                                                   ;
  x24 = 
# 2014 "wave.c" 3
       ((q)(((long)(
# 2014 "wave.c"
       1L
# 2014 "wave.c" 3
       ) << 4) + (0x2 +0x0)))
# 2014 "wave.c"
                  ;
  
# 2015 "wave.c" 3
 { extern q element_of_vector(); 
# 2015 "wave.c"
 x23 
# 2015 "wave.c" 3
 = element_of_vector(
# 2015 "wave.c"
 a9
# 2015 "wave.c" 3
 ,
# 2015 "wave.c"
 x24
# 2015 "wave.c" 3
 ); if ((long)
# 2015 "wave.c"
 x23 
# 2015 "wave.c" 3
 == 0) goto 
# 2015 "wave.c"
 pt_16_interrupt
# 2015 "wave.c" 3
 ; }
# 2015 "wave.c"
                                                   ;
 pt_16_181:
  switch (
# 2017 "wave.c" 3
         ((unsigned long)(
# 2017 "wave.c"
         a10
# 2017 "wave.c" 3
         ) & 0x3)
# 2017 "wave.c"
                    ) {
 case 
# 2018 "wave.c" 3
     0x3
# 2018 "wave.c"
            :
 pt_16_182:
  if (!
# 2020 "wave.c" 3
      ((((unsigned long)((((struct functor *)((unsigned long)(
# 2020 "wave.c"
      a10
# 2020 "wave.c" 3
      ) - 0x3))->functor)) & 0x3) == 0x0))
# 2020 "wave.c"
                 ) goto pt_16_interrupt;
  if (!
# 2021 "wave.c" 3
      ((struct data_object_method_table *)((((struct functor *)((unsigned long)(
# 2021 "wave.c"
      a10
# 2021 "wave.c" 3
      ) - 0x3))->functor)) == &
# 2021 "wave.c"
      vector_g_data_method_table
# 2021 "wave.c" 3
      )
# 2021 "wave.c"
                         ) goto pt_16_interrupt;
  
# 2022 "wave.c" 3
 { extern q size_of_vector(); 
# 2022 "wave.c"
 x25 
# 2022 "wave.c" 3
 = size_of_vector(
# 2022 "wave.c"
 a10
# 2022 "wave.c" 3
 ); }
# 2022 "wave.c"
                                             ;
 pt_16_183:
  if (x25 != 
# 2024 "wave.c" 3
            ((q)(((long)(
# 2024 "wave.c"
            2L
# 2024 "wave.c" 3
            ) << 4) + (0x2 +0x0)))
# 2024 "wave.c"
                       ) goto pt_16_interrupt;
  x27 = 
# 2025 "wave.c" 3
       ((q)(((long)(
# 2025 "wave.c"
       0L
# 2025 "wave.c" 3
       ) << 4) + (0x2 +0x0)))
# 2025 "wave.c"
                  ;
  
# 2026 "wave.c" 3
 { extern q element_of_vector(); 
# 2026 "wave.c"
 x26 
# 2026 "wave.c" 3
 = element_of_vector(
# 2026 "wave.c"
 a10
# 2026 "wave.c" 3
 ,
# 2026 "wave.c"
 x27
# 2026 "wave.c" 3
 ); if ((long)
# 2026 "wave.c"
 x26 
# 2026 "wave.c" 3
 == 0) goto 
# 2026 "wave.c"
 pt_16_interrupt
# 2026 "wave.c" 3
 ; }
# 2026 "wave.c"
                                                    ;
  x29 = 
# 2027 "wave.c" 3
       ((q)(((long)(
# 2027 "wave.c"
       1L
# 2027 "wave.c" 3
       ) << 4) + (0x2 +0x0)))
# 2027 "wave.c"
                  ;
  
# 2028 "wave.c" 3
 { extern q element_of_vector(); 
# 2028 "wave.c"
 x28 
# 2028 "wave.c" 3
 = element_of_vector(
# 2028 "wave.c"
 a10
# 2028 "wave.c" 3
 ,
# 2028 "wave.c"
 x29
# 2028 "wave.c" 3
 ); if ((long)
# 2028 "wave.c"
 x28 
# 2028 "wave.c" 3
 == 0) goto 
# 2028 "wave.c"
 pt_16_interrupt
# 2028 "wave.c" 3
 ; }
# 2028 "wave.c"
                                                    ;
 pt_16_184:
  if (!
# 2030 "wave.c" 3
      (((unsigned long)(
# 2030 "wave.c"
      a0
# 2030 "wave.c" 3
      ) & 0xFL) == (0x2 +0x0))
# 2030 "wave.c"
               ) goto pt_16_191;
  x30 = 
# 2031 "wave.c" 3
       ((q)(((long)(
# 2031 "wave.c"
       1L
# 2031 "wave.c" 3
       ) << 4) + (0x2 +0x0)))
# 2031 "wave.c"
                  ;
  
# 2032 "wave.c" 3
 if ((long)(
# 2032 "wave.c"
 x30
# 2032 "wave.c" 3
 ) > (long)(
# 2032 "wave.c"
 a0
# 2032 "wave.c" 3
 )) goto 
# 2032 "wave.c"
 pt_16_interrupt
# 2032 "wave.c" 3
 ;
# 2032 "wave.c"
                                        ;
 pt_16_185:
  if (!
# 2034 "wave.c" 3
      (((unsigned long)(
# 2034 "wave.c"
      a3
# 2034 "wave.c" 3
      ) & 0xFL) == (0x2 +0x0))
# 2034 "wave.c"
               ) goto pt_16_190;
  
# 2035 "wave.c" 3
 if ((long)(
# 2035 "wave.c"
 a0
# 2035 "wave.c" 3
 ) > (long)(
# 2035 "wave.c"
 a3
# 2035 "wave.c" 3
 )) goto 
# 2035 "wave.c"
 pt_16_interrupt
# 2035 "wave.c" 3
 ;
# 2035 "wave.c"
                                       ;
 pt_16_186:
  if (!
# 2037 "wave.c" 3
      (((unsigned long)(
# 2037 "wave.c"
      a2
# 2037 "wave.c" 3
      ) & 0xFL) == (0x2 +0x0))
# 2037 "wave.c"
               ) goto pt_16_189;
  x31 = 
# 2038 "wave.c" 3
       ((q)(((long)(
# 2038 "wave.c"
       0L
# 2038 "wave.c" 3
       ) << 4) + (0x2 +0x0)))
# 2038 "wave.c"
                  ;
  
# 2039 "wave.c" 3
 if ((long)(
# 2039 "wave.c"
 x31
# 2039 "wave.c" 3
 ) > (long)(
# 2039 "wave.c"
 a2
# 2039 "wave.c" 3
 )) goto 
# 2039 "wave.c"
 pt_16_interrupt
# 2039 "wave.c" 3
 ;
# 2039 "wave.c"
                                        ;
 pt_16_187:
  if (!
# 2041 "wave.c" 3
      (((unsigned long)(
# 2041 "wave.c"
      a1
# 2041 "wave.c" 3
      ) & 0xFL) == (0x2 +0x0))
# 2041 "wave.c"
               ) goto pt_16_188;
  
# 2042 "wave.c" 3
 if ((long)(
# 2042 "wave.c"
 a2
# 2042 "wave.c" 3
 ) > (long)(
# 2042 "wave.c"
 a1
# 2042 "wave.c" 3
 )) goto 
# 2042 "wave.c"
 pt_16_interrupt
# 2042 "wave.c" 3
 ;
# 2042 "wave.c"
                                       ;
  x32 = 
# 2043 "wave.c" 3
       ((q)(((long)(
# 2043 "wave.c"
       2L
# 2043 "wave.c" 3
       ) << 4) + (0x2 +0x0)))
# 2043 "wave.c"
                  ;
  
# 2044 "wave.c" 3
 { 
# 2044 "wave.c"
 x33 
# 2044 "wave.c" 3
 = (q)((unsigned long)(
# 2044 "wave.c"
 a3
# 2044 "wave.c" 3
 ) - ((unsigned long)(
# 2044 "wave.c"
 x32
# 2044 "wave.c" 3
 ) - (0x2 +0x0))); }
# 2044 "wave.c"
                              ;
  allocp[0] = 
# 2045 "wave.c" 3
             ((q)(((unsigned long)(
# 2045 "wave.c"
             190L+
# 2045 "wave.c" 3
             0) << 4) + (0x2 +0x4)))
# 2045 "wave.c"
                                    ;
  allocp[1] = x33;
  allocp[2] = x35 = 
# 2047 "wave.c" 3
                   (q) ((unsigned long)(
# 2047 "wave.c"
                   &allocp[2]
# 2047 "wave.c" 3
                   ) + 0x0)
# 2047 "wave.c"
                                      ;
  allocp[3] = x36 = 
# 2048 "wave.c" 3
                   (q) ((unsigned long)(
# 2048 "wave.c"
                   &allocp[3]
# 2048 "wave.c" 3
                   ) + 0x0)
# 2048 "wave.c"
                                      ;
  x34 = 
# 2049 "wave.c" 3
       ((q)((unsigned long)(
# 2049 "wave.c"
       &allocp[0]
# 2049 "wave.c" 3
       ) + 0x3))
# 2049 "wave.c"
                              ;
  allocp[4] = x38 = 
# 2050 "wave.c" 3
                   (q) ((unsigned long)(
# 2050 "wave.c"
                   &allocp[4]
# 2050 "wave.c" 3
                   ) + 0x0)
# 2050 "wave.c"
                                      ;
  allocp[5] = x34;
  x37 = 
# 2052 "wave.c" 3
       ((q)((unsigned long)(
# 2052 "wave.c"
       &allocp[4]
# 2052 "wave.c" 3
       ) + 0x1))
# 2052 "wave.c"
                           ;
  allocp += 6;
  
# 2054 "wave.c" 3
 { if (!(((unsigned long)(
# 2054 "wave.c"
 x13
# 2054 "wave.c" 3
 ) & 0x3) == 0x0) || (*((q *)((unsigned long)(
# 2054 "wave.c"
 x13
# 2054 "wave.c" 3
 ) - 0x0))) != (
# 2054 "wave.c"
 x13
# 2054 "wave.c" 3
 )) { allocp = do_unify_value(allocp, (
# 2054 "wave.c"
 x13
# 2054 "wave.c" 3
 ), (
# 2054 "wave.c"
 x37
# 2054 "wave.c" 3
 )); } else { (*((q *)((unsigned long)(
# 2054 "wave.c"
 x13
# 2054 "wave.c" 3
 ) - 0x0))) = (
# 2054 "wave.c"
 x37
# 2054 "wave.c" 3
 ); } }
# 2054 "wave.c"
                      ;
  allocp[0] = 
# 2055 "wave.c" 3
             ((q)(((unsigned long)(0) << 4) + (0x2 +0x4)))
# 2055 "wave.c"
                    ;
  allocp[1] = x35;
  x39 = 
# 2057 "wave.c" 3
       ((q)((unsigned long)(
# 2057 "wave.c"
       &allocp[0]
# 2057 "wave.c" 3
       ) + 0x1))
# 2057 "wave.c"
                           ;
  allocp[2] = (q)qp;
  allocp[3] = (q)(&predicate_main_xnode_7);
  allocp[4] = a0;
  allocp[5] = a1;
  allocp[6] = a2;
  allocp[7] = x39;
  allocp[8] = x40 = 
# 2064 "wave.c" 3
                   (q) ((unsigned long)(
# 2064 "wave.c"
                   &allocp[8]
# 2064 "wave.c" 3
                   ) + 0x0)
# 2064 "wave.c"
                                      ;
  allocp[9] = a11;
  allocp[10] = a12;
  allocp[11] = (q)(struct goalrec*)&allocp[2];
  allocp[12] = (q)(&predicate_main_xroute_14);
  allocp[13] = a15;
  allocp[14] = x1;
  allocp[15] = x6;
  allocp[16] = x11;
  allocp[17] = x16;
  allocp[18] = x21;
  allocp[19] = x26;
  allocp[20] = x3;
  allocp[21] = x8;
  allocp[22] = x38;
  allocp[23] = x18;
  allocp[24] = x23;
  allocp[25] = x28;
  allocp[26] = x40;
  a0 = x36;
  a1 = a13;
  a2 = a14;
  qp = (struct goalrec*)&allocp[11];
  allocp += 27;
  
# 2088 "wave.c" 3
 { if (allocp < (glbl->heaplimit0)) goto 
# 2088 "wave.c"
 acknowledged_3_clear_reason
# 2088 "wave.c" 3
 ;}
# 2088 "wave.c"
                                     ;
  goto acknowledged_3_ext_interrupt;
 pt_16_188:
  if (!
# 2091 "wave.c" 3
      (((unsigned long)(
# 2091 "wave.c"
      a1
# 2091 "wave.c" 3
      ) & 0x3) == 0x0)
# 2091 "wave.c"
               ) goto pt_16_interrupt;
  
# 2092 "wave.c" 3
 { q temp0 = (*((q *)((unsigned long)(
# 2092 "wave.c"
 a1
# 2092 "wave.c" 3
 ) - 0x0))); if(!(((unsigned long)(temp0) & 0x3) == 0x0)) { (
# 2092 "wave.c"
 a1
# 2092 "wave.c" 3
 ) = temp0; goto 
# 2092 "wave.c"
 pt_16_187
# 2092 "wave.c" 3
 ; } }
# 2092 "wave.c"
                             ;
  *reasonp++ = a1;
  goto pt_16_interrupt;
 pt_16_189:
  if (!
# 2096 "wave.c" 3
      (((unsigned long)(
# 2096 "wave.c"
      a2
# 2096 "wave.c" 3
      ) & 0x3) == 0x0)
# 2096 "wave.c"
               ) goto pt_16_interrupt;
  
# 2097 "wave.c" 3
 { q temp0 = (*((q *)((unsigned long)(
# 2097 "wave.c"
 a2
# 2097 "wave.c" 3
 ) - 0x0))); if(!(((unsigned long)(temp0) & 0x3) == 0x0)) { (
# 2097 "wave.c"
 a2
# 2097 "wave.c" 3
 ) = temp0; goto 
# 2097 "wave.c"
 pt_16_186
# 2097 "wave.c" 3
 ; } }
# 2097 "wave.c"
                             ;
  *reasonp++ = a2;
  goto pt_16_interrupt;
 pt_16_190:
  if (!
# 2101 "wave.c" 3
      (((unsigned long)(
# 2101 "wave.c"
      a3
# 2101 "wave.c" 3
      ) & 0x3) == 0x0)
# 2101 "wave.c"
               ) goto pt_16_interrupt;
  
# 2102 "wave.c" 3
 { q temp0 = (*((q *)((unsigned long)(
# 2102 "wave.c"
 a3
# 2102 "wave.c" 3
 ) - 0x0))); if(!(((unsigned long)(temp0) & 0x3) == 0x0)) { (
# 2102 "wave.c"
 a3
# 2102 "wave.c" 3
 ) = temp0; goto 
# 2102 "wave.c"
 pt_16_185
# 2102 "wave.c" 3
 ; } }
# 2102 "wave.c"
                             ;
  *reasonp++ = a3;
  goto pt_16_interrupt;
 pt_16_191:
  if (!
# 2106 "wave.c" 3
      (((unsigned long)(
# 2106 "wave.c"
      a0
# 2106 "wave.c" 3
      ) & 0x3) == 0x0)
# 2106 "wave.c"
               ) goto pt_16_interrupt;
  
# 2107 "wave.c" 3
 { q temp0 = (*((q *)((unsigned long)(
# 2107 "wave.c"
 a0
# 2107 "wave.c" 3
 ) - 0x0))); if(!(((unsigned long)(temp0) & 0x3) == 0x0)) { (
# 2107 "wave.c"
 a0
# 2107 "wave.c" 3
 ) = temp0; goto 
# 2107 "wave.c"
 pt_16_184
# 2107 "wave.c" 3
 ; } }
# 2107 "wave.c"
                             ;
  *reasonp++ = a0;
  goto pt_16_interrupt;
 case 
# 2110 "wave.c" 3
     0x0
# 2110 "wave.c"
           :
  
# 2111 "wave.c" 3
 { q temp0 = (*((q *)((unsigned long)(
# 2111 "wave.c"
 a10
# 2111 "wave.c" 3
 ) - 0x0))); if(!(((unsigned long)(temp0) & 0x3) == 0x0)) { (
# 2111 "wave.c"
 a10
# 2111 "wave.c" 3
 ) = temp0; goto 
# 2111 "wave.c"
 pt_16_181
# 2111 "wave.c" 3
 ; } }
# 2111 "wave.c"
                              ;
  *reasonp++ = a10;
  goto pt_16_interrupt;
  };
  goto pt_16_interrupt;
 case 
# 2116 "wave.c" 3
     0x0
# 2116 "wave.c"
           :
  
# 2117 "wave.c" 3
 { q temp0 = (*((q *)((unsigned long)(
# 2117 "wave.c"
 a9
# 2117 "wave.c" 3
 ) - 0x0))); if(!(((unsigned long)(temp0) & 0x3) == 0x0)) { (
# 2117 "wave.c"
 a9
# 2117 "wave.c" 3
 ) = temp0; goto 
# 2117 "wave.c"
 pt_16_178
# 2117 "wave.c" 3
 ; } }
# 2117 "wave.c"
                             ;
  *reasonp++ = a9;
  goto pt_16_interrupt;
  };
  goto pt_16_interrupt;
 case 
# 2122 "wave.c" 3
     0x0
# 2122 "wave.c"
           :
  
# 2123 "wave.c" 3
 { q temp0 = (*((q *)((unsigned long)(
# 2123 "wave.c"
 a8
# 2123 "wave.c" 3
 ) - 0x0))); if(!(((unsigned long)(temp0) & 0x3) == 0x0)) { (
# 2123 "wave.c"
 a8
# 2123 "wave.c" 3
 ) = temp0; goto 
# 2123 "wave.c"
 pt_16_175
# 2123 "wave.c" 3
 ; } }
# 2123 "wave.c"
                             ;
  *reasonp++ = a8;
  goto pt_16_interrupt;
  };
  goto pt_16_interrupt;
 case 
# 2128 "wave.c" 3
     0x0
# 2128 "wave.c"
           :
  
# 2129 "wave.c" 3
 { q temp0 = (*((q *)((unsigned long)(
# 2129 "wave.c"
 a7
# 2129 "wave.c" 3
 ) - 0x0))); if(!(((unsigned long)(temp0) & 0x3) == 0x0)) { (
# 2129 "wave.c"
 a7
# 2129 "wave.c" 3
 ) = temp0; goto 
# 2129 "wave.c"
 pt_16_172
# 2129 "wave.c" 3
 ; } }
# 2129 "wave.c"
                             ;
  *reasonp++ = a7;
  goto pt_16_interrupt;
  };
  goto pt_16_interrupt;
 case 
# 2134 "wave.c" 3
     0x0
# 2134 "wave.c"
           :
  
# 2135 "wave.c" 3
 { q temp0 = (*((q *)((unsigned long)(
# 2135 "wave.c"
 a6
# 2135 "wave.c" 3
 ) - 0x0))); if(!(((unsigned long)(temp0) & 0x3) == 0x0)) { (
# 2135 "wave.c"
 a6
# 2135 "wave.c" 3
 ) = temp0; goto 
# 2135 "wave.c"
 pt_16_169
# 2135 "wave.c" 3
 ; } }
# 2135 "wave.c"
                             ;
  *reasonp++ = a6;
  goto pt_16_interrupt;
  };
  goto pt_16_interrupt;
 case 
# 2140 "wave.c" 3
     0x0
# 2140 "wave.c"
           :
  
# 2141 "wave.c" 3
 { q temp0 = (*((q *)((unsigned long)(
# 2141 "wave.c"
 a5
# 2141 "wave.c" 3
 ) - 0x0))); if(!(((unsigned long)(temp0) & 0x3) == 0x0)) { (
# 2141 "wave.c"
 a5
# 2141 "wave.c" 3
 ) = temp0; goto 
# 2141 "wave.c"
 pt_16_166
# 2141 "wave.c" 3
 ; } }
# 2141 "wave.c"
                             ;
  *reasonp++ = a5;
  goto pt_16_interrupt;
  };
  goto pt_16_interrupt;
 pt_16_192:
  
# 2147 "wave.c" 3
 { q temp0 = (*((q *)((unsigned long)(
# 2147 "wave.c"
 a1
# 2147 "wave.c" 3
 ) - 0x0))); if(!(((unsigned long)(temp0) & 0x3) == 0x0)) { (
# 2147 "wave.c"
 a1
# 2147 "wave.c" 3
 ) = temp0; goto 
# 2147 "wave.c"
 pt_16_165
# 2147 "wave.c" 3
 ; } }
# 2147 "wave.c"
                             ;
  *reasonp++ = a1;
  goto pt_16_interrupt;
 pt_16_ext_interrupt:
  reasonp = 0l;
 pt_16_interrupt:
  toppred = &predicate_main_xpt_16;
  goto interrupt_16;
 }

 filter_8_top: {
  q x0, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13, x14;
  a0 = qp->args[0];
  a1 = qp->args[1];
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  a6 = qp->args[6];
  a7 = qp->args[7];
  qp = qp->next;
 filter_8_clear_reason:
  reasonp = 
# 2169 "wave.c" 3
           (glbl->reasons0)
# 2169 "wave.c"
                  ;
 filter_8_0:
 filter_8_1:
  switch (
# 2172 "wave.c" 3
         ((unsigned long)(
# 2172 "wave.c"
         a1
# 2172 "wave.c" 3
         ) & 0x3)
# 2172 "wave.c"
                   ) {
 case 
# 2173 "wave.c" 3
     0x1
# 2173 "wave.c"
         :
  x0 = 
# 2174 "wave.c" 3
      (((struct cons *)((unsigned long)(
# 2174 "wave.c"
      a1
# 2174 "wave.c" 3
      ) - 0x1))->car)
# 2174 "wave.c"
                ;
 filter_8_2:
  switch (
# 2176 "wave.c" 3
         ((unsigned long)(
# 2176 "wave.c"
         x0
# 2176 "wave.c" 3
         ) & 0x3)
# 2176 "wave.c"
                   ) {
 case 
# 2177 "wave.c" 3
     0x3
# 2177 "wave.c"
            :
  if (
# 2178 "wave.c" 3
     (((struct functor *)((unsigned long)(
# 2178 "wave.c"
     x0
# 2178 "wave.c" 3
     ) - 0x3))->functor) 
# 2178 "wave.c"
                    != 
# 2178 "wave.c" 3
                       ((q)(((unsigned long)(
# 2178 "wave.c"
                       190L+
# 2178 "wave.c" 3
                       0) << 4) + (0x2 +0x4)))
# 2178 "wave.c"
                                              ) goto filter_8_6;
  x1 = 
# 2179 "wave.c" 3
      (((struct functor *)((unsigned long)(
# 2179 "wave.c"
      x0
# 2179 "wave.c" 3
      ) - 0x3))->args[
# 2179 "wave.c"
      0
# 2179 "wave.c" 3
      ])
# 2179 "wave.c"
                ;
 filter_8_3:
  if (!
# 2181 "wave.c" 3
      (((unsigned long)(
# 2181 "wave.c"
      x1
# 2181 "wave.c" 3
      ) & 0xFL) == (0x2 +0x0))
# 2181 "wave.c"
               ) goto filter_8_5;
  x2 = 
# 2182 "wave.c" 3
      ((q)(((long)(
# 2182 "wave.c"
      0L
# 2182 "wave.c" 3
      ) << 4) + (0x2 +0x0)))
# 2182 "wave.c"
                 ;
  
# 2183 "wave.c" 3
 if ((long)(
# 2183 "wave.c"
 x1
# 2183 "wave.c" 3
 ) <= (long)(
# 2183 "wave.c"
 x2
# 2183 "wave.c" 3
 )) goto 
# 2183 "wave.c"
 filter_8_4
# 2183 "wave.c" 3
 ;
# 2183 "wave.c"
                               ;
  x3 = 
# 2184 "wave.c" 3
      ((q)(((long)(
# 2184 "wave.c"
      1L
# 2184 "wave.c" 3
      ) << 4) + (0x2 +0x0)))
# 2184 "wave.c"
                 ;
  
# 2185 "wave.c" 3
 { 
# 2185 "wave.c"
 x4 
# 2185 "wave.c" 3
 = (q)((unsigned long)(
# 2185 "wave.c"
 x1
# 2185 "wave.c" 3
 ) - ((unsigned long)(
# 2185 "wave.c"
 x3
# 2185 "wave.c" 3
 ) - (0x2 +0x0))); }
# 2185 "wave.c"
                            ;
  allocp[0] = 
# 2186 "wave.c" 3
             ((q)(((unsigned long)(
# 2186 "wave.c"
             190L+
# 2186 "wave.c" 3
             0) << 4) + (0x2 +0x4)))
# 2186 "wave.c"
                                    ;
  allocp[1] = x4;
  x6 = 
# 2188 "wave.c" 3
      (((struct functor *)((unsigned long)(
# 2188 "wave.c"
      x0
# 2188 "wave.c" 3
      ) - 0x3))->args[
# 2188 "wave.c"
      1
# 2188 "wave.c" 3
      ])
# 2188 "wave.c"
                ;
  allocp[2] = x6;
  x7 = 
# 2190 "wave.c" 3
      (((struct functor *)((unsigned long)(
# 2190 "wave.c"
      x0
# 2190 "wave.c" 3
      ) - 0x3))->args[
# 2190 "wave.c"
      2
# 2190 "wave.c" 3
      ])
# 2190 "wave.c"
                ;
  allocp[3] = x7;
  x5 = 
# 2192 "wave.c" 3
      ((q)((unsigned long)(
# 2192 "wave.c"
      &allocp[0]
# 2192 "wave.c" 3
      ) + 0x3))
# 2192 "wave.c"
                             ;
  allocp[4] = x9 = 
# 2193 "wave.c" 3
                  (q) ((unsigned long)(
# 2193 "wave.c"
                  &allocp[4]
# 2193 "wave.c" 3
                  ) + 0x0)
# 2193 "wave.c"
                                     ;
  allocp[5] = x5;
  x8 = 
# 2195 "wave.c" 3
      ((q)((unsigned long)(
# 2195 "wave.c"
      &allocp[4]
# 2195 "wave.c" 3
      ) + 0x1))
# 2195 "wave.c"
                          ;
  allocp += 6;
  
# 2197 "wave.c" 3
 { if (!(((unsigned long)(
# 2197 "wave.c"
 a2
# 2197 "wave.c" 3
 ) & 0x3) == 0x0) || (*((q *)((unsigned long)(
# 2197 "wave.c"
 a2
# 2197 "wave.c" 3
 ) - 0x0))) != (
# 2197 "wave.c"
 a2
# 2197 "wave.c" 3
 )) { allocp = do_unify_value(allocp, (
# 2197 "wave.c"
 a2
# 2197 "wave.c" 3
 ), (
# 2197 "wave.c"
 x8
# 2197 "wave.c" 3
 )); } else { (*((q *)((unsigned long)(
# 2197 "wave.c"
 a2
# 2197 "wave.c" 3
 ) - 0x0))) = (
# 2197 "wave.c"
 x8
# 2197 "wave.c" 3
 ); } }
# 2197 "wave.c"
                    ;
  a1 = 
# 2198 "wave.c" 3
      (((struct cons *)((unsigned long)(
# 2198 "wave.c"
      a1
# 2198 "wave.c" 3
      ) - 0x1))->cdr)
# 2198 "wave.c"
                ;
  a2 = x9;
  
# 2200 "wave.c" 3
 { if (allocp < (glbl->heaplimit0)) goto 
# 2200 "wave.c"
 filter_8_0
# 2200 "wave.c" 3
 ;}
# 2200 "wave.c"
                    ;
  goto filter_8_ext_interrupt;
 filter_8_4:
  if (x1 != 
# 2203 "wave.c" 3
           ((q)(((long)(
# 2203 "wave.c"
           0L
# 2203 "wave.c" 3
           ) << 4) + (0x2 +0x0)))
# 2203 "wave.c"
                      ) goto filter_8_6;
  allocp[0] = x11 = 
# 2204 "wave.c" 3
                   (q) ((unsigned long)(
# 2204 "wave.c"
                   &allocp[0]
# 2204 "wave.c" 3
                   ) + 0x0)
# 2204 "wave.c"
                                      ;
  x12 = 
# 2205 "wave.c" 3
       (((struct functor *)((unsigned long)(
# 2205 "wave.c"
       x0
# 2205 "wave.c" 3
       ) - 0x3))->args[
# 2205 "wave.c"
       1
# 2205 "wave.c" 3
       ])
# 2205 "wave.c"
                 ;
  allocp[1] = x12;
  x10 = 
# 2207 "wave.c" 3
       ((q)((unsigned long)(
# 2207 "wave.c"
       &allocp[0]
# 2207 "wave.c" 3
       ) + 0x1))
# 2207 "wave.c"
                           ;
  allocp += 2;
  
# 2209 "wave.c" 3
 { if (!(((unsigned long)(
# 2209 "wave.c"
 a6
# 2209 "wave.c" 3
 ) & 0x3) == 0x0) || (*((q *)((unsigned long)(
# 2209 "wave.c"
 a6
# 2209 "wave.c" 3
 ) - 0x0))) != (
# 2209 "wave.c"
 a6
# 2209 "wave.c" 3
 )) { allocp = do_unify_value(allocp, (
# 2209 "wave.c"
 a6
# 2209 "wave.c" 3
 ), (
# 2209 "wave.c"
 x10
# 2209 "wave.c" 3
 )); } else { (*((q *)((unsigned long)(
# 2209 "wave.c"
 a6
# 2209 "wave.c" 3
 ) - 0x0))) = (
# 2209 "wave.c"
 x10
# 2209 "wave.c" 3
 ); } }
# 2209 "wave.c"
                     ;
  x13 = 
# 2210 "wave.c" 3
       (((struct functor *)((unsigned long)(
# 2210 "wave.c"
       x0
# 2210 "wave.c" 3
       ) - 0x3))->args[
# 2210 "wave.c"
       2
# 2210 "wave.c" 3
       ])
# 2210 "wave.c"
                 ;
  x14 = 
# 2211 "wave.c" 3
       ((q)(((unsigned long)(
# 2211 "wave.c"
       311L+
# 2211 "wave.c" 3
       2) << 4) + (0x2 +0x4)))
# 2211 "wave.c"
                        ;
  
# 2212 "wave.c" 3
 { if (!(((unsigned long)(
# 2212 "wave.c"
 x13
# 2212 "wave.c" 3
 ) & 0x3) == 0x0) || (*((q *)((unsigned long)(
# 2212 "wave.c"
 x13
# 2212 "wave.c" 3
 ) - 0x0))) != (
# 2212 "wave.c"
 x13
# 2212 "wave.c" 3
 )) { allocp = do_unify_value(allocp, (
# 2212 "wave.c"
 x13
# 2212 "wave.c" 3
 ), (
# 2212 "wave.c"
 x14
# 2212 "wave.c" 3
 )); } else { (*((q *)((unsigned long)(
# 2212 "wave.c"
 x13
# 2212 "wave.c" 3
 ) - 0x0))) = (
# 2212 "wave.c"
 x14
# 2212 "wave.c" 3
 ); } }
# 2212 "wave.c"
                      ;
  a1 = 
# 2213 "wave.c" 3
      (((struct cons *)((unsigned long)(
# 2213 "wave.c"
      a1
# 2213 "wave.c" 3
      ) - 0x1))->cdr)
# 2213 "wave.c"
                ;
  a6 = x11;
  
# 2215 "wave.c" 3
 { if (allocp < (glbl->heaplimit0)) goto 
# 2215 "wave.c"
 filter_8_0
# 2215 "wave.c" 3
 ;}
# 2215 "wave.c"
                    ;
  goto filter_8_ext_interrupt;
 filter_8_5:
  if (!
# 2218 "wave.c" 3
      (((unsigned long)(
# 2218 "wave.c"
      x1
# 2218 "wave.c" 3
      ) & 0x3) == 0x0)
# 2218 "wave.c"
               ) goto filter_8_6;
  
# 2219 "wave.c" 3
 { q temp0 = (*((q *)((unsigned long)(
# 2219 "wave.c"
 x1
# 2219 "wave.c" 3
 ) - 0x0))); if(!(((unsigned long)(temp0) & 0x3) == 0x0)) { (
# 2219 "wave.c"
 x1
# 2219 "wave.c" 3
 ) = temp0; goto 
# 2219 "wave.c"
 filter_8_3
# 2219 "wave.c" 3
 ; } }
# 2219 "wave.c"
                              ;
  *reasonp++ = x1;
  goto filter_8_6;
 case 
# 2222 "wave.c" 3
     0x0
# 2222 "wave.c"
           :
  
# 2223 "wave.c" 3
 { q temp0 = (*((q *)((unsigned long)(
# 2223 "wave.c"
 x0
# 2223 "wave.c" 3
 ) - 0x0))); if(!(((unsigned long)(temp0) & 0x3) == 0x0)) { (
# 2223 "wave.c"
 x0
# 2223 "wave.c" 3
 ) = temp0; goto 
# 2223 "wave.c"
 filter_8_2
# 2223 "wave.c" 3
 ; } }
# 2223 "wave.c"
                              ;
  *reasonp++ = x0;
  goto filter_8_6;
  };
  goto filter_8_6;
 case 
# 2228 "wave.c" 3
     0x0
# 2228 "wave.c"
           :
  
# 2229 "wave.c" 3
 { q temp0 = (*((q *)((unsigned long)(
# 2229 "wave.c"
 a1
# 2229 "wave.c" 3
 ) - 0x0))); if(!(((unsigned long)(temp0) & 0x3) == 0x0)) { (
# 2229 "wave.c"
 a1
# 2229 "wave.c" 3
 ) = temp0; goto 
# 2229 "wave.c"
 filter_8_1
# 2229 "wave.c" 3
 ; } }
# 2229 "wave.c"
                              ;
  *reasonp++ = a1;
  goto filter_8_6;
  };
  goto filter_8_6;
 filter_8_6:
 filter_8_7:
  switch (
# 2236 "wave.c" 3
         ((unsigned long)(
# 2236 "wave.c"
         a0
# 2236 "wave.c" 3
         ) & 0x3)
# 2236 "wave.c"
                   ) {
 case 
# 2237 "wave.c" 3
     0x2
# 2237 "wave.c"
           :
  if (a0 != 
# 2238 "wave.c" 3
           ((q)(((unsigned long)(
# 2238 "wave.c"
           309L+
# 2238 "wave.c" 3
           2) << 4) + (0x2 +0x4)))
# 2238 "wave.c"
                           ) goto filter_8_interrupt;
 filter_8_8:
  switch (
# 2240 "wave.c" 3
         ((unsigned long)(
# 2240 "wave.c"
         a3
# 2240 "wave.c" 3
         ) & 0x3)
# 2240 "wave.c"
                   ) {
 case 
# 2241 "wave.c" 3
     0x2
# 2241 "wave.c"
           :
  if (a3 != 
# 2242 "wave.c" 3
           ((q)(((unsigned long)(
# 2242 "wave.c"
           82L+
# 2242 "wave.c" 3
           2) << 4) + (0x2 +0x4)))
# 2242 "wave.c"
                             ) goto filter_8_interrupt;
  allocp[0] = a5;
  allocp[1] = a1;
  x0 = 
# 2245 "wave.c" 3
      ((q)((unsigned long)(
# 2245 "wave.c"
      &allocp[0]
# 2245 "wave.c" 3
      ) + 0x1))
# 2245 "wave.c"
                          ;
  allocp += 2;
  
# 2247 "wave.c" 3
 { if (!(((unsigned long)(
# 2247 "wave.c"
 a4
# 2247 "wave.c" 3
 ) & 0x3) == 0x0) || (*((q *)((unsigned long)(
# 2247 "wave.c"
 a4
# 2247 "wave.c" 3
 ) - 0x0))) != (
# 2247 "wave.c"
 a4
# 2247 "wave.c" 3
 )) { allocp = do_unify_value(allocp, (
# 2247 "wave.c"
 a4
# 2247 "wave.c" 3
 ), (
# 2247 "wave.c"
 x0
# 2247 "wave.c" 3
 )); } else { (*((q *)((unsigned long)(
# 2247 "wave.c"
 a4
# 2247 "wave.c" 3
 ) - 0x0))) = (
# 2247 "wave.c"
 x0
# 2247 "wave.c" 3
 ); } }
# 2247 "wave.c"
                    ;
  allocp[0] = a7;
  allocp[1] = a2;
  x1 = 
# 2250 "wave.c" 3
      ((q)((unsigned long)(
# 2250 "wave.c"
      &allocp[0]
# 2250 "wave.c" 3
      ) + 0x1))
# 2250 "wave.c"
                          ;
  allocp += 2;
  
# 2252 "wave.c" 3
 { if (!(((unsigned long)(
# 2252 "wave.c"
 a6
# 2252 "wave.c" 3
 ) & 0x3) == 0x0) || (*((q *)((unsigned long)(
# 2252 "wave.c"
 a6
# 2252 "wave.c" 3
 ) - 0x0))) != (
# 2252 "wave.c"
 a6
# 2252 "wave.c" 3
 )) { allocp = do_unify_value(allocp, (
# 2252 "wave.c"
 a6
# 2252 "wave.c" 3
 ), (
# 2252 "wave.c"
 x1
# 2252 "wave.c" 3
 )); } else { (*((q *)((unsigned long)(
# 2252 "wave.c"
 a6
# 2252 "wave.c" 3
 ) - 0x0))) = (
# 2252 "wave.c"
 x1
# 2252 "wave.c" 3
 ); } }
# 2252 "wave.c"
                    ;
  
# 2253 "wave.c" 3
 { goto proceed_label; }
# 2253 "wave.c"
          ;
 case 
# 2254 "wave.c" 3
     0x3
# 2254 "wave.c"
            :
  if (
# 2255 "wave.c" 3
     (((struct functor *)((unsigned long)(
# 2255 "wave.c"
     a3
# 2255 "wave.c" 3
     ) - 0x3))->functor) 
# 2255 "wave.c"
                    != 
# 2255 "wave.c" 3
                       ((q)(((unsigned long)(
# 2255 "wave.c"
                       191L+
# 2255 "wave.c" 3
                       0) << 4) + (0x2 +0x4)))
# 2255 "wave.c"
                                              ) goto filter_8_interrupt;
  allocp[0] = a5;
  allocp[1] = a1;
  x0 = 
# 2258 "wave.c" 3
      ((q)((unsigned long)(
# 2258 "wave.c"
      &allocp[0]
# 2258 "wave.c" 3
      ) + 0x1))
# 2258 "wave.c"
                          ;
  allocp += 2;
  
# 2260 "wave.c" 3
 { if (!(((unsigned long)(
# 2260 "wave.c"
 a4
# 2260 "wave.c" 3
 ) & 0x3) == 0x0) || (*((q *)((unsigned long)(
# 2260 "wave.c"
 a4
# 2260 "wave.c" 3
 ) - 0x0))) != (
# 2260 "wave.c"
 a4
# 2260 "wave.c" 3
 )) { allocp = do_unify_value(allocp, (
# 2260 "wave.c"
 a4
# 2260 "wave.c" 3
 ), (
# 2260 "wave.c"
 x0
# 2260 "wave.c" 3
 )); } else { (*((q *)((unsigned long)(
# 2260 "wave.c"
 a4
# 2260 "wave.c" 3
 ) - 0x0))) = (
# 2260 "wave.c"
 x0
# 2260 "wave.c" 3
 ); } }
# 2260 "wave.c"
                    ;
  allocp[0] = x2 = 
# 2261 "wave.c" 3
                  (q) ((unsigned long)(
# 2261 "wave.c"
                  &allocp[0]
# 2261 "wave.c" 3
                  ) + 0x0)
# 2261 "wave.c"
                                     ;
  x3 = 
# 2262 "wave.c" 3
      (((struct functor *)((unsigned long)(
# 2262 "wave.c"
      a3
# 2262 "wave.c" 3
      ) - 0x3))->args[
# 2262 "wave.c"
      0
# 2262 "wave.c" 3
      ])
# 2262 "wave.c"
                ;
  allocp[1] = x3;
  x1 = 
# 2264 "wave.c" 3
      ((q)((unsigned long)(
# 2264 "wave.c"
      &allocp[0]
# 2264 "wave.c" 3
      ) + 0x1))
# 2264 "wave.c"
                          ;
  allocp += 2;
  
# 2266 "wave.c" 3
 { if (!(((unsigned long)(
# 2266 "wave.c"
 a2
# 2266 "wave.c" 3
 ) & 0x3) == 0x0) || (*((q *)((unsigned long)(
# 2266 "wave.c"
 a2
# 2266 "wave.c" 3
 ) - 0x0))) != (
# 2266 "wave.c"
 a2
# 2266 "wave.c" 3
 )) { allocp = do_unify_value(allocp, (
# 2266 "wave.c"
 a2
# 2266 "wave.c" 3
 ), (
# 2266 "wave.c"
 x1
# 2266 "wave.c" 3
 )); } else { (*((q *)((unsigned long)(
# 2266 "wave.c"
 a2
# 2266 "wave.c" 3
 ) - 0x0))) = (
# 2266 "wave.c"
 x1
# 2266 "wave.c" 3
 ); } }
# 2266 "wave.c"
                    ;
  allocp[0] = a7;
  allocp[1] = x2;
  x4 = 
# 2269 "wave.c" 3
      ((q)((unsigned long)(
# 2269 "wave.c"
      &allocp[0]
# 2269 "wave.c" 3
      ) + 0x1))
# 2269 "wave.c"
                          ;
  allocp += 2;
  
# 2271 "wave.c" 3
 { if (!(((unsigned long)(
# 2271 "wave.c"
 a6
# 2271 "wave.c" 3
 ) & 0x3) == 0x0) || (*((q *)((unsigned long)(
# 2271 "wave.c"
 a6
# 2271 "wave.c" 3
 ) - 0x0))) != (
# 2271 "wave.c"
 a6
# 2271 "wave.c" 3
 )) { allocp = do_unify_value(allocp, (
# 2271 "wave.c"
 a6
# 2271 "wave.c" 3
 ), (
# 2271 "wave.c"
 x4
# 2271 "wave.c" 3
 )); } else { (*((q *)((unsigned long)(
# 2271 "wave.c"
 a6
# 2271 "wave.c" 3
 ) - 0x0))) = (
# 2271 "wave.c"
 x4
# 2271 "wave.c" 3
 ); } }
# 2271 "wave.c"
                    ;
  
# 2272 "wave.c" 3
 { goto proceed_label; }
# 2272 "wave.c"
          ;
 case 
# 2273 "wave.c" 3
     0x0
# 2273 "wave.c"
           :
  
# 2274 "wave.c" 3
 { q temp0 = (*((q *)((unsigned long)(
# 2274 "wave.c"
 a3
# 2274 "wave.c" 3
 ) - 0x0))); if(!(((unsigned long)(temp0) & 0x3) == 0x0)) { (
# 2274 "wave.c"
 a3
# 2274 "wave.c" 3
 ) = temp0; goto 
# 2274 "wave.c"
 filter_8_8
# 2274 "wave.c" 3
 ; } }
# 2274 "wave.c"
                              ;
  *reasonp++ = a3;
  goto filter_8_interrupt;
  };
  goto filter_8_interrupt;
 case 
# 2279 "wave.c" 3
     0x0
# 2279 "wave.c"
           :
  
# 2280 "wave.c" 3
 { q temp0 = (*((q *)((unsigned long)(
# 2280 "wave.c"
 a0
# 2280 "wave.c" 3
 ) - 0x0))); if(!(((unsigned long)(temp0) & 0x3) == 0x0)) { (
# 2280 "wave.c"
 a0
# 2280 "wave.c" 3
 ) = temp0; goto 
# 2280 "wave.c"
 filter_8_7
# 2280 "wave.c" 3
 ; } }
# 2280 "wave.c"
                              ;
  *reasonp++ = a0;
  goto filter_8_interrupt;
  };
  goto filter_8_interrupt;
 filter_8_ext_interrupt:
  reasonp = 0l;
 filter_8_interrupt:
  toppred = &predicate_main_xfilter_8;
  goto interrupt_8;
 }

 route_14_top: {
  q x0, x1, x2, x3, x4;
  a0 = qp->args[0];
  a1 = qp->args[1];
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  a6 = qp->args[6];
  a7 = qp->args[7];
  a8 = qp->args[8];
  a9 = qp->args[9];
  a10 = qp->args[10];
  a11 = qp->args[11];
  a12 = qp->args[12];
  a13 = qp->args[13];
  qp = qp->next;
 route_14_clear_reason:
  reasonp = 
# 2310 "wave.c" 3
           (glbl->reasons0)
# 2310 "wave.c"
                  ;
 route_14_0:
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_main_xbfilter_5);
  allocp[2] = a0;
  allocp[3] = a6;
  allocp[4] = a11;
  allocp[5] = x0 = 
# 2317 "wave.c" 3
                  (q) ((unsigned long)(
# 2317 "wave.c"
                  &allocp[5]
# 2317 "wave.c" 3
                  ) + 0x0)
# 2317 "wave.c"
                                     ;
  allocp[6] = 
# 2318 "wave.c" 3
             ((q)(((unsigned long)(0) << 4) + (0x2 +0x4)))
# 2318 "wave.c"
                    ;
  allocp[7] = (q)(struct goalrec*)&allocp[0];
  allocp[8] = (q)(&predicate_main_xbfilter_5);
  allocp[9] = a0;
  allocp[10] = a5;
  allocp[11] = a12;
  allocp[12] = x1 = 
# 2324 "wave.c" 3
                   (q) ((unsigned long)(
# 2324 "wave.c"
                   &allocp[12]
# 2324 "wave.c" 3
                   ) + 0x0)
# 2324 "wave.c"
                                       ;
  allocp[13] = x0;
  allocp[14] = (q)(struct goalrec*)&allocp[7];
  allocp[15] = (q)(&predicate_main_xbfilter_5);
  allocp[16] = a0;
  allocp[17] = a4;
  allocp[18] = a8;
  allocp[19] = x2 = 
# 2331 "wave.c" 3
                   (q) ((unsigned long)(
# 2331 "wave.c"
                   &allocp[19]
# 2331 "wave.c" 3
                   ) + 0x0)
# 2331 "wave.c"
                                       ;
  allocp[20] = x1;
  allocp[21] = (q)(struct goalrec*)&allocp[14];
  allocp[22] = (q)(&predicate_main_xbfilter_5);
  allocp[23] = a0;
  allocp[24] = a3;
  allocp[25] = a7;
  allocp[26] = x3 = 
# 2338 "wave.c" 3
                   (q) ((unsigned long)(
# 2338 "wave.c"
                   &allocp[26]
# 2338 "wave.c" 3
                   ) + 0x0)
# 2338 "wave.c"
                                       ;
  allocp[27] = x2;
  allocp[28] = (q)(struct goalrec*)&allocp[21];
  allocp[29] = (q)(&predicate_main_xbfilter_5);
  allocp[30] = a0;
  allocp[31] = a2;
  allocp[32] = a10;
  allocp[33] = x4 = 
# 2345 "wave.c" 3
                   (q) ((unsigned long)(
# 2345 "wave.c"
                   &allocp[33]
# 2345 "wave.c" 3
                   ) + 0x0)
# 2345 "wave.c"
                                       ;
  allocp[34] = x3;
  a2 = a9;
  a3 = a13;
  a4 = x4;
  qp = (struct goalrec*)&allocp[28];
  allocp += 35;
  
# 2352 "wave.c" 3
 { if (allocp < (glbl->heaplimit0)) goto 
# 2352 "wave.c"
 bfilter_5_0
# 2352 "wave.c" 3
 ;}
# 2352 "wave.c"
                     ;
  goto bfilter_5_ext_interrupt;
 route_14_ext_interrupt:
  reasonp = 0l;
 route_14_interrupt:
  goto interrupt_14;
 }

 bfilter_5_top: {
  q x0, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13, x14;
  a0 = qp->args[0];
  a1 = qp->args[1];
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  qp = qp->next;
 bfilter_5_clear_reason:
  reasonp = 
# 2369 "wave.c" 3
           (glbl->reasons0)
# 2369 "wave.c"
                  ;
 bfilter_5_0:
 bfilter_5_1:
  switch (
# 2372 "wave.c" 3
         ((unsigned long)(
# 2372 "wave.c"
         a1
# 2372 "wave.c" 3
         ) & 0x3)
# 2372 "wave.c"
                   ) {
 case 
# 2373 "wave.c" 3
     0x1
# 2373 "wave.c"
         :
  x0 = 
# 2374 "wave.c" 3
      (((struct cons *)((unsigned long)(
# 2374 "wave.c"
      a1
# 2374 "wave.c" 3
      ) - 0x1))->car)
# 2374 "wave.c"
                ;
 bfilter_5_2:
  switch (
# 2376 "wave.c" 3
         ((unsigned long)(
# 2376 "wave.c"
         x0
# 2376 "wave.c" 3
         ) & 0x3)
# 2376 "wave.c"
                   ) {
 case 
# 2377 "wave.c" 3
     0x3
# 2377 "wave.c"
            :
  if (
# 2378 "wave.c" 3
     (((struct functor *)((unsigned long)(
# 2378 "wave.c"
     x0
# 2378 "wave.c" 3
     ) - 0x3))->functor) 
# 2378 "wave.c"
                    != 
# 2378 "wave.c" 3
                       ((q)(((unsigned long)(
# 2378 "wave.c"
                       190L+
# 2378 "wave.c" 3
                       0) << 4) + (0x2 +0x4)))
# 2378 "wave.c"
                                              ) goto bfilter_5_6;
  x1 = 
# 2379 "wave.c" 3
      (((struct functor *)((unsigned long)(
# 2379 "wave.c"
      x0
# 2379 "wave.c" 3
      ) - 0x3))->args[
# 2379 "wave.c"
      0
# 2379 "wave.c" 3
      ])
# 2379 "wave.c"
                ;
 bfilter_5_3:
  if (!
# 2381 "wave.c" 3
      (((unsigned long)(
# 2381 "wave.c"
      x1
# 2381 "wave.c" 3
      ) & 0xFL) == (0x2 +0x0))
# 2381 "wave.c"
               ) goto bfilter_5_5;
  x2 = 
# 2382 "wave.c" 3
      ((q)(((long)(
# 2382 "wave.c"
      0L
# 2382 "wave.c" 3
      ) << 4) + (0x2 +0x0)))
# 2382 "wave.c"
                 ;
  
# 2383 "wave.c" 3
 if ((long)(
# 2383 "wave.c"
 x1
# 2383 "wave.c" 3
 ) <= (long)(
# 2383 "wave.c"
 x2
# 2383 "wave.c" 3
 )) goto 
# 2383 "wave.c"
 bfilter_5_4
# 2383 "wave.c" 3
 ;
# 2383 "wave.c"
                                ;
  x3 = 
# 2384 "wave.c" 3
      ((q)(((long)(
# 2384 "wave.c"
      1L
# 2384 "wave.c" 3
      ) << 4) + (0x2 +0x0)))
# 2384 "wave.c"
                 ;
  
# 2385 "wave.c" 3
 { 
# 2385 "wave.c"
 x4 
# 2385 "wave.c" 3
 = (q)((unsigned long)(
# 2385 "wave.c"
 x1
# 2385 "wave.c" 3
 ) - ((unsigned long)(
# 2385 "wave.c"
 x3
# 2385 "wave.c" 3
 ) - (0x2 +0x0))); }
# 2385 "wave.c"
                            ;
  allocp[0] = 
# 2386 "wave.c" 3
             ((q)(((unsigned long)(
# 2386 "wave.c"
             190L+
# 2386 "wave.c" 3
             0) << 4) + (0x2 +0x4)))
# 2386 "wave.c"
                                    ;
  allocp[1] = x4;
  x6 = 
# 2388 "wave.c" 3
      (((struct functor *)((unsigned long)(
# 2388 "wave.c"
      x0
# 2388 "wave.c" 3
      ) - 0x3))->args[
# 2388 "wave.c"
      1
# 2388 "wave.c" 3
      ])
# 2388 "wave.c"
                ;
  allocp[2] = x6;
  x7 = 
# 2390 "wave.c" 3
      (((struct functor *)((unsigned long)(
# 2390 "wave.c"
      x0
# 2390 "wave.c" 3
      ) - 0x3))->args[
# 2390 "wave.c"
      2
# 2390 "wave.c" 3
      ])
# 2390 "wave.c"
                ;
  allocp[3] = x7;
  x5 = 
# 2392 "wave.c" 3
      ((q)((unsigned long)(
# 2392 "wave.c"
      &allocp[0]
# 2392 "wave.c" 3
      ) + 0x3))
# 2392 "wave.c"
                             ;
  allocp[4] = x9 = 
# 2393 "wave.c" 3
                  (q) ((unsigned long)(
# 2393 "wave.c"
                  &allocp[4]
# 2393 "wave.c" 3
                  ) + 0x0)
# 2393 "wave.c"
                                     ;
  allocp[5] = x5;
  x8 = 
# 2395 "wave.c" 3
      ((q)((unsigned long)(
# 2395 "wave.c"
      &allocp[4]
# 2395 "wave.c" 3
      ) + 0x1))
# 2395 "wave.c"
                          ;
  allocp += 6;
  
# 2397 "wave.c" 3
 { if (!(((unsigned long)(
# 2397 "wave.c"
 a2
# 2397 "wave.c" 3
 ) & 0x3) == 0x0) || (*((q *)((unsigned long)(
# 2397 "wave.c"
 a2
# 2397 "wave.c" 3
 ) - 0x0))) != (
# 2397 "wave.c"
 a2
# 2397 "wave.c" 3
 )) { allocp = do_unify_value(allocp, (
# 2397 "wave.c"
 a2
# 2397 "wave.c" 3
 ), (
# 2397 "wave.c"
 x8
# 2397 "wave.c" 3
 )); } else { (*((q *)((unsigned long)(
# 2397 "wave.c"
 a2
# 2397 "wave.c" 3
 ) - 0x0))) = (
# 2397 "wave.c"
 x8
# 2397 "wave.c" 3
 ); } }
# 2397 "wave.c"
                    ;
  a1 = 
# 2398 "wave.c" 3
      (((struct cons *)((unsigned long)(
# 2398 "wave.c"
      a1
# 2398 "wave.c" 3
      ) - 0x1))->cdr)
# 2398 "wave.c"
                ;
  a2 = x9;
  
# 2400 "wave.c" 3
 { if (allocp < (glbl->heaplimit0)) goto 
# 2400 "wave.c"
 bfilter_5_0
# 2400 "wave.c" 3
 ;}
# 2400 "wave.c"
                     ;
  goto bfilter_5_ext_interrupt;
 bfilter_5_4:
  if (x1 != 
# 2403 "wave.c" 3
           ((q)(((long)(
# 2403 "wave.c"
           0L
# 2403 "wave.c" 3
           ) << 4) + (0x2 +0x0)))
# 2403 "wave.c"
                      ) goto bfilter_5_6;
  allocp[0] = x11 = 
# 2404 "wave.c" 3
                   (q) ((unsigned long)(
# 2404 "wave.c"
                   &allocp[0]
# 2404 "wave.c" 3
                   ) + 0x0)
# 2404 "wave.c"
                                      ;
  x12 = 
# 2405 "wave.c" 3
       (((struct functor *)((unsigned long)(
# 2405 "wave.c"
       x0
# 2405 "wave.c" 3
       ) - 0x3))->args[
# 2405 "wave.c"
       1
# 2405 "wave.c" 3
       ])
# 2405 "wave.c"
                 ;
  allocp[1] = x12;
  x10 = 
# 2407 "wave.c" 3
       ((q)((unsigned long)(
# 2407 "wave.c"
       &allocp[0]
# 2407 "wave.c" 3
       ) + 0x1))
# 2407 "wave.c"
                           ;
  allocp += 2;
  
# 2409 "wave.c" 3
 { if (!(((unsigned long)(
# 2409 "wave.c"
 a3
# 2409 "wave.c" 3
 ) & 0x3) == 0x0) || (*((q *)((unsigned long)(
# 2409 "wave.c"
 a3
# 2409 "wave.c" 3
 ) - 0x0))) != (
# 2409 "wave.c"
 a3
# 2409 "wave.c" 3
 )) { allocp = do_unify_value(allocp, (
# 2409 "wave.c"
 a3
# 2409 "wave.c" 3
 ), (
# 2409 "wave.c"
 x10
# 2409 "wave.c" 3
 )); } else { (*((q *)((unsigned long)(
# 2409 "wave.c"
 a3
# 2409 "wave.c" 3
 ) - 0x0))) = (
# 2409 "wave.c"
 x10
# 2409 "wave.c" 3
 ); } }
# 2409 "wave.c"
                     ;
  x13 = 
# 2410 "wave.c" 3
       (((struct functor *)((unsigned long)(
# 2410 "wave.c"
       x0
# 2410 "wave.c" 3
       ) - 0x3))->args[
# 2410 "wave.c"
       2
# 2410 "wave.c" 3
       ])
# 2410 "wave.c"
                 ;
  x14 = 
# 2411 "wave.c" 3
       ((q)(((unsigned long)(
# 2411 "wave.c"
       311L+
# 2411 "wave.c" 3
       2) << 4) + (0x2 +0x4)))
# 2411 "wave.c"
                        ;
  
# 2412 "wave.c" 3
 { if (!(((unsigned long)(
# 2412 "wave.c"
 x13
# 2412 "wave.c" 3
 ) & 0x3) == 0x0) || (*((q *)((unsigned long)(
# 2412 "wave.c"
 x13
# 2412 "wave.c" 3
 ) - 0x0))) != (
# 2412 "wave.c"
 x13
# 2412 "wave.c" 3
 )) { allocp = do_unify_value(allocp, (
# 2412 "wave.c"
 x13
# 2412 "wave.c" 3
 ), (
# 2412 "wave.c"
 x14
# 2412 "wave.c" 3
 )); } else { (*((q *)((unsigned long)(
# 2412 "wave.c"
 x13
# 2412 "wave.c" 3
 ) - 0x0))) = (
# 2412 "wave.c"
 x14
# 2412 "wave.c" 3
 ); } }
# 2412 "wave.c"
                      ;
  a1 = 
# 2413 "wave.c" 3
      (((struct cons *)((unsigned long)(
# 2413 "wave.c"
      a1
# 2413 "wave.c" 3
      ) - 0x1))->cdr)
# 2413 "wave.c"
                ;
  a3 = x11;
  
# 2415 "wave.c" 3
 { if (allocp < (glbl->heaplimit0)) goto 
# 2415 "wave.c"
 bfilter_5_0
# 2415 "wave.c" 3
 ;}
# 2415 "wave.c"
                     ;
  goto bfilter_5_ext_interrupt;
 bfilter_5_5:
  if (!
# 2418 "wave.c" 3
      (((unsigned long)(
# 2418 "wave.c"
      x1
# 2418 "wave.c" 3
      ) & 0x3) == 0x0)
# 2418 "wave.c"
               ) goto bfilter_5_6;
  
# 2419 "wave.c" 3
 { q temp0 = (*((q *)((unsigned long)(
# 2419 "wave.c"
 x1
# 2419 "wave.c" 3
 ) - 0x0))); if(!(((unsigned long)(temp0) & 0x3) == 0x0)) { (
# 2419 "wave.c"
 x1
# 2419 "wave.c" 3
 ) = temp0; goto 
# 2419 "wave.c"
 bfilter_5_3
# 2419 "wave.c" 3
 ; } }
# 2419 "wave.c"
                               ;
  *reasonp++ = x1;
  goto bfilter_5_6;
 case 
# 2422 "wave.c" 3
     0x0
# 2422 "wave.c"
           :
  
# 2423 "wave.c" 3
 { q temp0 = (*((q *)((unsigned long)(
# 2423 "wave.c"
 x0
# 2423 "wave.c" 3
 ) - 0x0))); if(!(((unsigned long)(temp0) & 0x3) == 0x0)) { (
# 2423 "wave.c"
 x0
# 2423 "wave.c" 3
 ) = temp0; goto 
# 2423 "wave.c"
 bfilter_5_2
# 2423 "wave.c" 3
 ; } }
# 2423 "wave.c"
                               ;
  *reasonp++ = x0;
  goto bfilter_5_6;
  };
  goto bfilter_5_6;
 case 
# 2428 "wave.c" 3
     0x0
# 2428 "wave.c"
           :
  
# 2429 "wave.c" 3
 { q temp0 = (*((q *)((unsigned long)(
# 2429 "wave.c"
 a1
# 2429 "wave.c" 3
 ) - 0x0))); if(!(((unsigned long)(temp0) & 0x3) == 0x0)) { (
# 2429 "wave.c"
 a1
# 2429 "wave.c" 3
 ) = temp0; goto 
# 2429 "wave.c"
 bfilter_5_1
# 2429 "wave.c" 3
 ; } }
# 2429 "wave.c"
                               ;
  *reasonp++ = a1;
  goto bfilter_5_6;
  };
  goto bfilter_5_6;
 bfilter_5_6:
 bfilter_5_7:
  switch (
# 2436 "wave.c" 3
         ((unsigned long)(
# 2436 "wave.c"
         a0
# 2436 "wave.c" 3
         ) & 0x3)
# 2436 "wave.c"
                   ) {
 case 
# 2437 "wave.c" 3
     0x2
# 2437 "wave.c"
           :
  if (a0 != 
# 2438 "wave.c" 3
           ((q)(((unsigned long)(
# 2438 "wave.c"
           309L+
# 2438 "wave.c" 3
           2) << 4) + (0x2 +0x4)))
# 2438 "wave.c"
                           ) goto bfilter_5_interrupt;
  
# 2439 "wave.c" 3
 { allocp = do_unify(allocp, (
# 2439 "wave.c"
 a3
# 2439 "wave.c" 3
 ), (
# 2439 "wave.c"
 a4
# 2439 "wave.c" 3
 )); }
# 2439 "wave.c"
              ;
  x0 = 
# 2440 "wave.c" 3
      ((q)(((unsigned long)(0) << 4) + (0x2 +0x4)))
# 2440 "wave.c"
             ;
  
# 2441 "wave.c" 3
 { if (!(((unsigned long)(
# 2441 "wave.c"
 a2
# 2441 "wave.c" 3
 ) & 0x3) == 0x0) || (*((q *)((unsigned long)(
# 2441 "wave.c"
 a2
# 2441 "wave.c" 3
 ) - 0x0))) != (
# 2441 "wave.c"
 a2
# 2441 "wave.c" 3
 )) { allocp = do_unify_value(allocp, (
# 2441 "wave.c"
 a2
# 2441 "wave.c" 3
 ), (
# 2441 "wave.c"
 x0
# 2441 "wave.c" 3
 )); } else { (*((q *)((unsigned long)(
# 2441 "wave.c"
 a2
# 2441 "wave.c" 3
 ) - 0x0))) = (
# 2441 "wave.c"
 x0
# 2441 "wave.c" 3
 ); } }
# 2441 "wave.c"
                    ;
  
# 2442 "wave.c" 3
 { goto proceed_label; }
# 2442 "wave.c"
          ;
 case 
# 2443 "wave.c" 3
     0x0
# 2443 "wave.c"
           :
  
# 2444 "wave.c" 3
 { q temp0 = (*((q *)((unsigned long)(
# 2444 "wave.c"
 a0
# 2444 "wave.c" 3
 ) - 0x0))); if(!(((unsigned long)(temp0) & 0x3) == 0x0)) { (
# 2444 "wave.c"
 a0
# 2444 "wave.c" 3
 ) = temp0; goto 
# 2444 "wave.c"
 bfilter_5_7
# 2444 "wave.c" 3
 ; } }
# 2444 "wave.c"
                               ;
  *reasonp++ = a0;
  goto bfilter_5_interrupt;
  };
  goto bfilter_5_interrupt;
 bfilter_5_ext_interrupt:
  reasonp = 0l;
 bfilter_5_interrupt:
  toppred = &predicate_main_xbfilter_5;
  goto interrupt_5;
 }

 node_7_top: {
  q x0, x1, x2, x3, x4, x5, x6;
  a0 = qp->args[0];
  a1 = qp->args[1];
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  a6 = qp->args[6];
  qp = qp->next;
 node_7_clear_reason:
  reasonp = 
# 2467 "wave.c" 3
           (glbl->reasons0)
# 2467 "wave.c"
                  ;
 node_7_0:
 node_7_1:
  if (!
# 2470 "wave.c" 3
      (((unsigned long)(
# 2470 "wave.c"
      a6
# 2470 "wave.c" 3
      ) & 0xFL) == (0x2 +0x0))
# 2470 "wave.c"
               ) goto node_7_3;
  x0 = 
# 2471 "wave.c" 3
      ((q)(((long)(
# 2471 "wave.c"
      0L
# 2471 "wave.c" 3
      ) << 4) + (0x2 +0x0)))
# 2471 "wave.c"
                 ;
  
# 2472 "wave.c" 3
 if ((long)(
# 2472 "wave.c"
 a6
# 2472 "wave.c" 3
 ) <= (long)(
# 2472 "wave.c"
 x0
# 2472 "wave.c" 3
 )) goto 
# 2472 "wave.c"
 node_7_2
# 2472 "wave.c" 3
 ;
# 2472 "wave.c"
                             ;
  x1 = 
# 2473 "wave.c" 3
      ((q)(((long)(
# 2473 "wave.c"
      1L
# 2473 "wave.c" 3
      ) << 4) + (0x2 +0x0)))
# 2473 "wave.c"
                 ;
  
# 2474 "wave.c" 3
 { 
# 2474 "wave.c"
 x2 
# 2474 "wave.c" 3
 = (q)((unsigned long)(
# 2474 "wave.c"
 a6
# 2474 "wave.c" 3
 ) - ((unsigned long)(
# 2474 "wave.c"
 x1
# 2474 "wave.c" 3
 ) - (0x2 +0x0))); }
# 2474 "wave.c"
                            ;
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_main_xnode_7);
  allocp[2] = a0;
  allocp[3] = a1;
  allocp[4] = a2;
  allocp[5] = x3 = 
# 2480 "wave.c" 3
                  (q) ((unsigned long)(
# 2480 "wave.c"
                  &allocp[5]
# 2480 "wave.c" 3
                  ) + 0x0)
# 2480 "wave.c"
                                     ;
  allocp[6] = x4 = 
# 2481 "wave.c" 3
                  (q) ((unsigned long)(
# 2481 "wave.c"
                  &allocp[6]
# 2481 "wave.c" 3
                  ) + 0x0)
# 2481 "wave.c"
                                     ;
  allocp[7] = x5 = 
# 2482 "wave.c" 3
                  (q) ((unsigned long)(
# 2482 "wave.c"
                  &allocp[7]
# 2482 "wave.c" 3
                  ) + 0x0)
# 2482 "wave.c"
                                     ;
  allocp[8] = x2;
  allocp[9] = (q)(struct goalrec*)&allocp[0];
  allocp[10] = (q)(&predicate_main_xputout_3);
  allocp[11] = x5;
  allocp[12] = a4;
  allocp[13] = x4;
  allocp[14] = (q)(struct goalrec*)&allocp[9];
  allocp[15] = (q)(&predicate_main_xcompute_6);
  allocp[16] = a0;
  allocp[17] = a1;
  allocp[18] = a2;
  allocp[19] = x6 = 
# 2494 "wave.c" 3
                   (q) ((unsigned long)(
# 2494 "wave.c"
                   &allocp[19]
# 2494 "wave.c" 3
                   ) + 0x0)
# 2494 "wave.c"
                                       ;
  allocp[20] = a5;
  allocp[21] = x5;
  a0 = a3;
  a1 = x6;
  a2 = x3;
  qp = (struct goalrec*)&allocp[14];
  allocp += 22;
  
# 2502 "wave.c" 3
 { if (allocp < (glbl->heaplimit0)) goto 
# 2502 "wave.c"
 getin_3_0
# 2502 "wave.c" 3
 ;}
# 2502 "wave.c"
                   ;
  goto getin_3_ext_interrupt;
 node_7_2:
  if (a6 != 
# 2505 "wave.c" 3
           ((q)(((long)(
# 2505 "wave.c"
           0L
# 2505 "wave.c" 3
           ) << 4) + (0x2 +0x0)))
# 2505 "wave.c"
                      ) goto node_7_interrupt;
  
# 2506 "wave.c" 3
 { goto proceed_label; }
# 2506 "wave.c"
          ;
 node_7_3:
  if (!
# 2508 "wave.c" 3
      (((unsigned long)(
# 2508 "wave.c"
      a6
# 2508 "wave.c" 3
      ) & 0x3) == 0x0)
# 2508 "wave.c"
               ) goto node_7_interrupt;
  
# 2509 "wave.c" 3
 { q temp0 = (*((q *)((unsigned long)(
# 2509 "wave.c"
 a6
# 2509 "wave.c" 3
 ) - 0x0))); if(!(((unsigned long)(temp0) & 0x3) == 0x0)) { (
# 2509 "wave.c"
 a6
# 2509 "wave.c" 3
 ) = temp0; goto 
# 2509 "wave.c"
 node_7_1
# 2509 "wave.c" 3
 ; } }
# 2509 "wave.c"
                            ;
  *reasonp++ = a6;
  goto node_7_interrupt;
 node_7_ext_interrupt:
  reasonp = 0l;
 node_7_interrupt:
  goto interrupt_7;
 }

 getin_3_top: {
  q x0, x1, x2, x3, x4, x5, x6;
  a0 = qp->args[0];
  a1 = qp->args[1];
  a2 = qp->args[2];
  qp = qp->next;
 getin_3_clear_reason:
  reasonp = 
# 2525 "wave.c" 3
           (glbl->reasons0)
# 2525 "wave.c"
                  ;
 getin_3_0:
 getin_3_1:
  switch (
# 2528 "wave.c" 3
         ((unsigned long)(
# 2528 "wave.c"
         a0
# 2528 "wave.c" 3
         ) & 0x3)
# 2528 "wave.c"
                   ) {
 case 
# 2529 "wave.c" 3
     0x1
# 2529 "wave.c"
         :
  x0 = 
# 2530 "wave.c" 3
      (((struct cons *)((unsigned long)(
# 2530 "wave.c"
      a0
# 2530 "wave.c" 3
      ) - 0x1))->car)
# 2530 "wave.c"
                ;
 getin_3_2:
  switch (
# 2532 "wave.c" 3
         ((unsigned long)(
# 2532 "wave.c"
         x0
# 2532 "wave.c" 3
         ) & 0x3)
# 2532 "wave.c"
                   ) {
 case 
# 2533 "wave.c" 3
     0x1
# 2533 "wave.c"
         :
  allocp[0] = x2 = 
# 2534 "wave.c" 3
                  (q) ((unsigned long)(
# 2534 "wave.c"
                  &allocp[0]
# 2534 "wave.c" 3
                  ) + 0x0)
# 2534 "wave.c"
                                     ;
  x3 = 
# 2535 "wave.c" 3
      (((struct cons *)((unsigned long)(
# 2535 "wave.c"
      x0
# 2535 "wave.c" 3
      ) - 0x1))->car)
# 2535 "wave.c"
                ;
  allocp[1] = x3;
  x1 = 
# 2537 "wave.c" 3
      ((q)((unsigned long)(
# 2537 "wave.c"
      &allocp[0]
# 2537 "wave.c" 3
      ) + 0x1))
# 2537 "wave.c"
                          ;
  allocp += 2;
  
# 2539 "wave.c" 3
 { if (!(((unsigned long)(
# 2539 "wave.c"
 a1
# 2539 "wave.c" 3
 ) & 0x3) == 0x0) || (*((q *)((unsigned long)(
# 2539 "wave.c"
 a1
# 2539 "wave.c" 3
 ) - 0x0))) != (
# 2539 "wave.c"
 a1
# 2539 "wave.c" 3
 )) { allocp = do_unify_value(allocp, (
# 2539 "wave.c"
 a1
# 2539 "wave.c" 3
 ), (
# 2539 "wave.c"
 x1
# 2539 "wave.c" 3
 )); } else { (*((q *)((unsigned long)(
# 2539 "wave.c"
 a1
# 2539 "wave.c" 3
 ) - 0x0))) = (
# 2539 "wave.c"
 x1
# 2539 "wave.c" 3
 ); } }
# 2539 "wave.c"
                    ;
  allocp[0] = x5 = 
# 2540 "wave.c" 3
                  (q) ((unsigned long)(
# 2540 "wave.c"
                  &allocp[0]
# 2540 "wave.c" 3
                  ) + 0x0)
# 2540 "wave.c"
                                     ;
  x6 = 
# 2541 "wave.c" 3
      (((struct cons *)((unsigned long)(
# 2541 "wave.c"
      x0
# 2541 "wave.c" 3
      ) - 0x1))->cdr)
# 2541 "wave.c"
                ;
  allocp[1] = x6;
  x4 = 
# 2543 "wave.c" 3
      ((q)((unsigned long)(
# 2543 "wave.c"
      &allocp[0]
# 2543 "wave.c" 3
      ) + 0x1))
# 2543 "wave.c"
                          ;
  allocp += 2;
  
# 2545 "wave.c" 3
 { if (!(((unsigned long)(
# 2545 "wave.c"
 a2
# 2545 "wave.c" 3
 ) & 0x3) == 0x0) || (*((q *)((unsigned long)(
# 2545 "wave.c"
 a2
# 2545 "wave.c" 3
 ) - 0x0))) != (
# 2545 "wave.c"
 a2
# 2545 "wave.c" 3
 )) { allocp = do_unify_value(allocp, (
# 2545 "wave.c"
 a2
# 2545 "wave.c" 3
 ), (
# 2545 "wave.c"
 x4
# 2545 "wave.c" 3
 )); } else { (*((q *)((unsigned long)(
# 2545 "wave.c"
 a2
# 2545 "wave.c" 3
 ) - 0x0))) = (
# 2545 "wave.c"
 x4
# 2545 "wave.c" 3
 ); } }
# 2545 "wave.c"
                    ;
  a0 = 
# 2546 "wave.c" 3
      (((struct cons *)((unsigned long)(
# 2546 "wave.c"
      a0
# 2546 "wave.c" 3
      ) - 0x1))->cdr)
# 2546 "wave.c"
                ;
  a1 = x2;
  a2 = x5;
  
# 2549 "wave.c" 3
 { if (allocp < (glbl->heaplimit0)) goto 
# 2549 "wave.c"
 getin_3_0
# 2549 "wave.c" 3
 ;}
# 2549 "wave.c"
                   ;
  goto getin_3_ext_interrupt;
 case 
# 2551 "wave.c" 3
     0x2
# 2551 "wave.c"
           :
  if (x0 != 
# 2552 "wave.c" 3
           ((q)(((unsigned long)(0) << 4) + (0x2 +0x4)))
# 2552 "wave.c"
                  ) goto getin_3_interrupt;
  a0 = 
# 2553 "wave.c" 3
      (((struct cons *)((unsigned long)(
# 2553 "wave.c"
      a0
# 2553 "wave.c" 3
      ) - 0x1))->cdr)
# 2553 "wave.c"
                ;
  
# 2554 "wave.c" 3
 { if (allocp < (glbl->heaplimit0)) goto 
# 2554 "wave.c"
 getin_3_0
# 2554 "wave.c" 3
 ;}
# 2554 "wave.c"
                   ;
  goto getin_3_ext_interrupt;
 case 
# 2556 "wave.c" 3
     0x0
# 2556 "wave.c"
           :
  
# 2557 "wave.c" 3
 { q temp0 = (*((q *)((unsigned long)(
# 2557 "wave.c"
 x0
# 2557 "wave.c" 3
 ) - 0x0))); if(!(((unsigned long)(temp0) & 0x3) == 0x0)) { (
# 2557 "wave.c"
 x0
# 2557 "wave.c" 3
 ) = temp0; goto 
# 2557 "wave.c"
 getin_3_2
# 2557 "wave.c" 3
 ; } }
# 2557 "wave.c"
                             ;
  *reasonp++ = x0;
  goto getin_3_interrupt;
  };
  goto getin_3_interrupt;
 case 
# 2562 "wave.c" 3
     0x2
# 2562 "wave.c"
           :
  if (a0 != 
# 2563 "wave.c" 3
           ((q)(((unsigned long)(0) << 4) + (0x2 +0x4)))
# 2563 "wave.c"
                  ) goto getin_3_interrupt;
  x0 = 
# 2564 "wave.c" 3
      ((q)(((unsigned long)(0) << 4) + (0x2 +0x4)))
# 2564 "wave.c"
             ;
  
# 2565 "wave.c" 3
 { if (!(((unsigned long)(
# 2565 "wave.c"
 a1
# 2565 "wave.c" 3
 ) & 0x3) == 0x0) || (*((q *)((unsigned long)(
# 2565 "wave.c"
 a1
# 2565 "wave.c" 3
 ) - 0x0))) != (
# 2565 "wave.c"
 a1
# 2565 "wave.c" 3
 )) { allocp = do_unify_value(allocp, (
# 2565 "wave.c"
 a1
# 2565 "wave.c" 3
 ), (
# 2565 "wave.c"
 x0
# 2565 "wave.c" 3
 )); } else { (*((q *)((unsigned long)(
# 2565 "wave.c"
 a1
# 2565 "wave.c" 3
 ) - 0x0))) = (
# 2565 "wave.c"
 x0
# 2565 "wave.c" 3
 ); } }
# 2565 "wave.c"
                    ;
  x1 = 
# 2566 "wave.c" 3
      ((q)(((unsigned long)(0) << 4) + (0x2 +0x4)))
# 2566 "wave.c"
             ;
  
# 2567 "wave.c" 3
 { if (!(((unsigned long)(
# 2567 "wave.c"
 a2
# 2567 "wave.c" 3
 ) & 0x3) == 0x0) || (*((q *)((unsigned long)(
# 2567 "wave.c"
 a2
# 2567 "wave.c" 3
 ) - 0x0))) != (
# 2567 "wave.c"
 a2
# 2567 "wave.c" 3
 )) { allocp = do_unify_value(allocp, (
# 2567 "wave.c"
 a2
# 2567 "wave.c" 3
 ), (
# 2567 "wave.c"
 x1
# 2567 "wave.c" 3
 )); } else { (*((q *)((unsigned long)(
# 2567 "wave.c"
 a2
# 2567 "wave.c" 3
 ) - 0x0))) = (
# 2567 "wave.c"
 x1
# 2567 "wave.c" 3
 ); } }
# 2567 "wave.c"
                    ;
  
# 2568 "wave.c" 3
 { goto proceed_label; }
# 2568 "wave.c"
          ;
 case 
# 2569 "wave.c" 3
     0x0
# 2569 "wave.c"
           :
  
# 2570 "wave.c" 3
 { q temp0 = (*((q *)((unsigned long)(
# 2570 "wave.c"
 a0
# 2570 "wave.c" 3
 ) - 0x0))); if(!(((unsigned long)(temp0) & 0x3) == 0x0)) { (
# 2570 "wave.c"
 a0
# 2570 "wave.c" 3
 ) = temp0; goto 
# 2570 "wave.c"
 getin_3_1
# 2570 "wave.c" 3
 ; } }
# 2570 "wave.c"
                             ;
  *reasonp++ = a0;
  goto getin_3_interrupt;
  };
  goto getin_3_interrupt;
 getin_3_ext_interrupt:
  reasonp = 0l;
 getin_3_interrupt:
  toppred = &predicate_main_xgetin_3;
  goto interrupt_3;
 }

 compute_6_top: {
  q x0;
  a0 = qp->args[0];
  a1 = qp->args[1];
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  qp = qp->next;
 compute_6_clear_reason:
  reasonp = 
# 2592 "wave.c" 3
           (glbl->reasons0)
# 2592 "wave.c"
                  ;
 compute_6_0:
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_main_xshow_7);
  allocp[2] = x0 = 
# 2596 "wave.c" 3
                  (q) ((unsigned long)(
# 2596 "wave.c"
                  &allocp[2]
# 2596 "wave.c" 3
                  ) + 0x0)
# 2596 "wave.c"
                                     ;
  allocp[3] = a0;
  allocp[4] = a1;
  allocp[5] = a2;
  allocp[6] = a3;
  allocp[7] = a4;
  allocp[8] = a5;
  a0 = a3;
  a1 = x0;
  qp = (struct goalrec*)&allocp[0];
  allocp += 9;
  
# 2607 "wave.c" 3
 { if (allocp < (glbl->heaplimit0)) goto 
# 2607 "wave.c"
 checklist_2_0
# 2607 "wave.c" 3
 ;}
# 2607 "wave.c"
                       ;
  goto checklist_2_ext_interrupt;
 compute_6_ext_interrupt:
  reasonp = 0l;
 compute_6_interrupt:
  goto interrupt_6;
 }

 checklist_2_top: {
  q x0;
  a0 = qp->args[0];
  a1 = qp->args[1];
  qp = qp->next;
 checklist_2_clear_reason:
  reasonp = 
# 2621 "wave.c" 3
           (glbl->reasons0)
# 2621 "wave.c"
                  ;
 checklist_2_0:
 checklist_2_1:
  switch (
# 2624 "wave.c" 3
         ((unsigned long)(
# 2624 "wave.c"
         a0
# 2624 "wave.c" 3
         ) & 0x3)
# 2624 "wave.c"
                   ) {
 case 
# 2625 "wave.c" 3
     0x1
# 2625 "wave.c"
         :
  x0 = 
# 2626 "wave.c" 3
      (((struct cons *)((unsigned long)(
# 2626 "wave.c"
      a0
# 2626 "wave.c" 3
      ) - 0x1))->car)
# 2626 "wave.c"
                ;
 checklist_2_2:
  if (!
# 2628 "wave.c" 3
      (((unsigned long)(
# 2628 "wave.c"
      x0
# 2628 "wave.c" 3
      ) & 0xFL) == (0x2 +0x0))
# 2628 "wave.c"
               ) goto checklist_2_3;
  ;
  a0 = 
# 2630 "wave.c" 3
      (((struct cons *)((unsigned long)(
# 2630 "wave.c"
      a0
# 2630 "wave.c" 3
      ) - 0x1))->cdr)
# 2630 "wave.c"
                ;
  
# 2631 "wave.c" 3
 { if (allocp < (glbl->heaplimit0)) goto 
# 2631 "wave.c"
 checklist_2_0
# 2631 "wave.c" 3
 ;}
# 2631 "wave.c"
                       ;
  goto checklist_2_ext_interrupt;
 checklist_2_3:
  if (!
# 2634 "wave.c" 3
      (((unsigned long)(
# 2634 "wave.c"
      x0
# 2634 "wave.c" 3
      ) & 0x3) == 0x0)
# 2634 "wave.c"
               ) goto checklist_2_interrupt;
  
# 2635 "wave.c" 3
 { q temp0 = (*((q *)((unsigned long)(
# 2635 "wave.c"
 x0
# 2635 "wave.c" 3
 ) - 0x0))); if(!(((unsigned long)(temp0) & 0x3) == 0x0)) { (
# 2635 "wave.c"
 x0
# 2635 "wave.c" 3
 ) = temp0; goto 
# 2635 "wave.c"
 checklist_2_2
# 2635 "wave.c" 3
 ; } }
# 2635 "wave.c"
                                 ;
  *reasonp++ = x0;
  goto checklist_2_interrupt;
 case 
# 2638 "wave.c" 3
     0x2
# 2638 "wave.c"
           :
  if (a0 != 
# 2639 "wave.c" 3
           ((q)(((unsigned long)(0) << 4) + (0x2 +0x4)))
# 2639 "wave.c"
                  ) goto checklist_2_interrupt;
  x0 = 
# 2640 "wave.c" 3
      ((q)(((unsigned long)(0) << 4) + (0x2 +0x4)))
# 2640 "wave.c"
             ;
  
# 2641 "wave.c" 3
 { if (!(((unsigned long)(
# 2641 "wave.c"
 a1
# 2641 "wave.c" 3
 ) & 0x3) == 0x0) || (*((q *)((unsigned long)(
# 2641 "wave.c"
 a1
# 2641 "wave.c" 3
 ) - 0x0))) != (
# 2641 "wave.c"
 a1
# 2641 "wave.c" 3
 )) { allocp = do_unify_value(allocp, (
# 2641 "wave.c"
 a1
# 2641 "wave.c" 3
 ), (
# 2641 "wave.c"
 x0
# 2641 "wave.c" 3
 )); } else { (*((q *)((unsigned long)(
# 2641 "wave.c"
 a1
# 2641 "wave.c" 3
 ) - 0x0))) = (
# 2641 "wave.c"
 x0
# 2641 "wave.c" 3
 ); } }
# 2641 "wave.c"
                    ;
  
# 2642 "wave.c" 3
 { goto proceed_label; }
# 2642 "wave.c"
          ;
 case 
# 2643 "wave.c" 3
     0x0
# 2643 "wave.c"
           :
  
# 2644 "wave.c" 3
 { q temp0 = (*((q *)((unsigned long)(
# 2644 "wave.c"
 a0
# 2644 "wave.c" 3
 ) - 0x0))); if(!(((unsigned long)(temp0) & 0x3) == 0x0)) { (
# 2644 "wave.c"
 a0
# 2644 "wave.c" 3
 ) = temp0; goto 
# 2644 "wave.c"
 checklist_2_1
# 2644 "wave.c" 3
 ; } }
# 2644 "wave.c"
                                 ;
  *reasonp++ = a0;
  goto checklist_2_interrupt;
  };
  goto checklist_2_interrupt;
 checklist_2_ext_interrupt:
  reasonp = 0l;
 checklist_2_interrupt:
  toppred = &predicate_main_xchecklist_2;
  goto interrupt_2;
 }

 show_7_top: {
  q x0, x1;
  a0 = qp->args[0];
  a1 = qp->args[1];
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  a5 = qp->args[5];
  a6 = qp->args[6];
  qp = qp->next;
 show_7_clear_reason:
  reasonp = 
# 2667 "wave.c" 3
           (glbl->reasons0)
# 2667 "wave.c"
                  ;
 show_7_0:
 show_7_1:
  if (
# 2670 "wave.c" 3
     (((unsigned long)(
# 2670 "wave.c"
     a0
# 2670 "wave.c" 3
     ) & 0x3) == 0x0)
# 2670 "wave.c"
              ) goto show_7_2;
  ;
  x0 = 
# 2672 "wave.c" 3
      ((q)(((long)(
# 2672 "wave.c"
      1L
# 2672 "wave.c" 3
      ) << 4) + (0x2 +0x0)))
# 2672 "wave.c"
                 ;
  
# 2673 "wave.c" 3
 { if (!(((unsigned long)(
# 2673 "wave.c"
 a5
# 2673 "wave.c" 3
 ) & 0xFL) == (0x2 +0x0))) { 
# 2673 "wave.c"
 x1 
# 2673 "wave.c" 3
 = bblt_add_3(
# 2673 "wave.c"
 a5
# 2673 "wave.c" 3
 , 
# 2673 "wave.c"
 x0
# 2673 "wave.c" 3
 ); } else { { 
# 2673 "wave.c"
 x1 
# 2673 "wave.c" 3
 = (q)((unsigned long)(
# 2673 "wave.c"
 a5
# 2673 "wave.c" 3
 ) + (unsigned long)(
# 2673 "wave.c"
 x0
# 2673 "wave.c" 3
 ) - (0x2 +0x0)); }; } }
# 2673 "wave.c"
                              ;
  
# 2674 "wave.c" 3
 { allocp = do_unify(allocp, (
# 2674 "wave.c"
 a6
# 2674 "wave.c" 3
 ), (
# 2674 "wave.c"
 x1
# 2674 "wave.c" 3
 )); }
# 2674 "wave.c"
              ;
  
# 2675 "wave.c" 3
 { goto proceed_label; }
# 2675 "wave.c"
          ;
 show_7_2:
  
# 2677 "wave.c" 3
 { q temp0 = (*((q *)((unsigned long)(
# 2677 "wave.c"
 a0
# 2677 "wave.c" 3
 ) - 0x0))); if(!(((unsigned long)(temp0) & 0x3) == 0x0)) { (
# 2677 "wave.c"
 a0
# 2677 "wave.c" 3
 ) = temp0; goto 
# 2677 "wave.c"
 show_7_1
# 2677 "wave.c" 3
 ; } }
# 2677 "wave.c"
                            ;
  *reasonp++ = a0;
  goto show_7_interrupt;
 show_7_ext_interrupt:
  reasonp = 0l;
 show_7_interrupt:
  goto interrupt_7;
 }

 putout_3_top: {
  q x0, x1, x2, x3, x4;
  a0 = qp->args[0];
  a1 = qp->args[1];
  a2 = qp->args[2];
  qp = qp->next;
 putout_3_clear_reason:
  reasonp = 
# 2693 "wave.c" 3
           (glbl->reasons0)
# 2693 "wave.c"
                  ;
 putout_3_0:
 putout_3_1:
  switch (
# 2696 "wave.c" 3
         ((unsigned long)(
# 2696 "wave.c"
         a1
# 2696 "wave.c" 3
         ) & 0x3)
# 2696 "wave.c"
                   ) {
 case 
# 2697 "wave.c" 3
     0x1
# 2697 "wave.c"
         :
  allocp[0] = x1 = 
# 2698 "wave.c" 3
                  (q) ((unsigned long)(
# 2698 "wave.c"
                  &allocp[0]
# 2698 "wave.c" 3
                  ) + 0x0)
# 2698 "wave.c"
                                     ;
  allocp[1] = a0;
  x0 = 
# 2700 "wave.c" 3
      ((q)((unsigned long)(
# 2700 "wave.c"
      &allocp[0]
# 2700 "wave.c" 3
      ) + 0x1))
# 2700 "wave.c"
                          ;
  x2 = 
# 2701 "wave.c" 3
      (((struct cons *)((unsigned long)(
# 2701 "wave.c"
      a1
# 2701 "wave.c" 3
      ) - 0x1))->car)
# 2701 "wave.c"
                ;
  allocp += 2;
  
# 2703 "wave.c" 3
 { if (!(((unsigned long)(
# 2703 "wave.c"
 x2
# 2703 "wave.c" 3
 ) & 0x3) == 0x0) || (*((q *)((unsigned long)(
# 2703 "wave.c"
 x2
# 2703 "wave.c" 3
 ) - 0x0))) != (
# 2703 "wave.c"
 x2
# 2703 "wave.c" 3
 )) { allocp = do_unify_value(allocp, (
# 2703 "wave.c"
 x2
# 2703 "wave.c" 3
 ), (
# 2703 "wave.c"
 x0
# 2703 "wave.c" 3
 )); } else { (*((q *)((unsigned long)(
# 2703 "wave.c"
 x2
# 2703 "wave.c" 3
 ) - 0x0))) = (
# 2703 "wave.c"
 x0
# 2703 "wave.c" 3
 ); } }
# 2703 "wave.c"
                    ;
  allocp[0] = x4 = 
# 2704 "wave.c" 3
                  (q) ((unsigned long)(
# 2704 "wave.c"
                  &allocp[0]
# 2704 "wave.c" 3
                  ) + 0x0)
# 2704 "wave.c"
                                     ;
  allocp[1] = x1;
  x3 = 
# 2706 "wave.c" 3
      ((q)((unsigned long)(
# 2706 "wave.c"
      &allocp[0]
# 2706 "wave.c" 3
      ) + 0x1))
# 2706 "wave.c"
                          ;
  allocp += 2;
  
# 2708 "wave.c" 3
 { if (!(((unsigned long)(
# 2708 "wave.c"
 a2
# 2708 "wave.c" 3
 ) & 0x3) == 0x0) || (*((q *)((unsigned long)(
# 2708 "wave.c"
 a2
# 2708 "wave.c" 3
 ) - 0x0))) != (
# 2708 "wave.c"
 a2
# 2708 "wave.c" 3
 )) { allocp = do_unify_value(allocp, (
# 2708 "wave.c"
 a2
# 2708 "wave.c" 3
 ), (
# 2708 "wave.c"
 x3
# 2708 "wave.c" 3
 )); } else { (*((q *)((unsigned long)(
# 2708 "wave.c"
 a2
# 2708 "wave.c" 3
 ) - 0x0))) = (
# 2708 "wave.c"
 x3
# 2708 "wave.c" 3
 ); } }
# 2708 "wave.c"
                    ;
  a1 = 
# 2709 "wave.c" 3
      (((struct cons *)((unsigned long)(
# 2709 "wave.c"
      a1
# 2709 "wave.c" 3
      ) - 0x1))->cdr)
# 2709 "wave.c"
                ;
  a2 = x4;
  
# 2711 "wave.c" 3
 { if (allocp < (glbl->heaplimit0)) goto 
# 2711 "wave.c"
 putout_3_0
# 2711 "wave.c" 3
 ;}
# 2711 "wave.c"
                    ;
  goto putout_3_ext_interrupt;
 case 
# 2713 "wave.c" 3
     0x2
# 2713 "wave.c"
           :
  if (a1 != 
# 2714 "wave.c" 3
           ((q)(((unsigned long)(0) << 4) + (0x2 +0x4)))
# 2714 "wave.c"
                  ) goto putout_3_interrupt;
  x0 = 
# 2715 "wave.c" 3
      ((q)(((unsigned long)(0) << 4) + (0x2 +0x4)))
# 2715 "wave.c"
             ;
  
# 2716 "wave.c" 3
 { if (!(((unsigned long)(
# 2716 "wave.c"
 a2
# 2716 "wave.c" 3
 ) & 0x3) == 0x0) || (*((q *)((unsigned long)(
# 2716 "wave.c"
 a2
# 2716 "wave.c" 3
 ) - 0x0))) != (
# 2716 "wave.c"
 a2
# 2716 "wave.c" 3
 )) { allocp = do_unify_value(allocp, (
# 2716 "wave.c"
 a2
# 2716 "wave.c" 3
 ), (
# 2716 "wave.c"
 x0
# 2716 "wave.c" 3
 )); } else { (*((q *)((unsigned long)(
# 2716 "wave.c"
 a2
# 2716 "wave.c" 3
 ) - 0x0))) = (
# 2716 "wave.c"
 x0
# 2716 "wave.c" 3
 ); } }
# 2716 "wave.c"
                    ;
  
# 2717 "wave.c" 3
 { goto proceed_label; }
# 2717 "wave.c"
          ;
 case 
# 2718 "wave.c" 3
     0x0
# 2718 "wave.c"
           :
  
# 2719 "wave.c" 3
 { q temp0 = (*((q *)((unsigned long)(
# 2719 "wave.c"
 a1
# 2719 "wave.c" 3
 ) - 0x0))); if(!(((unsigned long)(temp0) & 0x3) == 0x0)) { (
# 2719 "wave.c"
 a1
# 2719 "wave.c" 3
 ) = temp0; goto 
# 2719 "wave.c"
 putout_3_1
# 2719 "wave.c" 3
 ; } }
# 2719 "wave.c"
                              ;
  *reasonp++ = a1;
  goto putout_3_interrupt;
  };
  goto putout_3_interrupt;
 putout_3_ext_interrupt:
  reasonp = 0l;
 putout_3_interrupt:
  goto interrupt_3;
 }

 start_2_top: {
  q x0;
  a0 = qp->args[0];
  a1 = qp->args[1];
  qp = qp->next;
 start_2_clear_reason:
  reasonp = 
# 2736 "wave.c" 3
           (glbl->reasons0)
# 2736 "wave.c"
                  ;
 start_2_0:
 start_2_1:
  switch (
# 2739 "wave.c" 3
         ((unsigned long)(
# 2739 "wave.c"
         a0
# 2739 "wave.c" 3
         ) & 0x3)
# 2739 "wave.c"
                   ) {
 case 
# 2740 "wave.c" 3
     0x2
# 2740 "wave.c"
           :
  if (a0 != 
# 2741 "wave.c" 3
           ((q)(((unsigned long)(
# 2741 "wave.c"
           310L+
# 2741 "wave.c" 3
           2) << 4) + (0x2 +0x4)))
# 2741 "wave.c"
                             ) goto start_2_interrupt;
  x0 = 
# 2742 "wave.c" 3
      ((q)(((unsigned long)(
# 2742 "wave.c"
      309L+
# 2742 "wave.c" 3
      2) << 4) + (0x2 +0x4)))
# 2742 "wave.c"
                      ;
  
# 2743 "wave.c" 3
 { if (!(((unsigned long)(
# 2743 "wave.c"
 a1
# 2743 "wave.c" 3
 ) & 0x3) == 0x0) || (*((q *)((unsigned long)(
# 2743 "wave.c"
 a1
# 2743 "wave.c" 3
 ) - 0x0))) != (
# 2743 "wave.c"
 a1
# 2743 "wave.c" 3
 )) { allocp = do_unify_value(allocp, (
# 2743 "wave.c"
 a1
# 2743 "wave.c" 3
 ), (
# 2743 "wave.c"
 x0
# 2743 "wave.c" 3
 )); } else { (*((q *)((unsigned long)(
# 2743 "wave.c"
 a1
# 2743 "wave.c" 3
 ) - 0x0))) = (
# 2743 "wave.c"
 x0
# 2743 "wave.c" 3
 ); } }
# 2743 "wave.c"
                    ;
  
# 2744 "wave.c" 3
 { goto proceed_label; }
# 2744 "wave.c"
          ;
 case 
# 2745 "wave.c" 3
     0x0
# 2745 "wave.c"
           :
  
# 2746 "wave.c" 3
 { q temp0 = (*((q *)((unsigned long)(
# 2746 "wave.c"
 a0
# 2746 "wave.c" 3
 ) - 0x0))); if(!(((unsigned long)(temp0) & 0x3) == 0x0)) { (
# 2746 "wave.c"
 a0
# 2746 "wave.c" 3
 ) = temp0; goto 
# 2746 "wave.c"
 start_2_1
# 2746 "wave.c" 3
 ; } }
# 2746 "wave.c"
                             ;
  *reasonp++ = a0;
  goto start_2_interrupt;
  };
  goto start_2_interrupt;
 start_2_ext_interrupt:
  reasonp = 0l;
 start_2_interrupt:
  goto interrupt_2;
 }

 acknowledged_3_top: {
  a0 = qp->args[0];
  a1 = qp->args[1];
  a2 = qp->args[2];
  qp = qp->next;
 acknowledged_3_clear_reason:
  reasonp = 
# 2763 "wave.c" 3
           (glbl->reasons0)
# 2763 "wave.c"
                  ;
 acknowledged_3_0:
 acknowledged_3_1:
  switch (
# 2766 "wave.c" 3
         ((unsigned long)(
# 2766 "wave.c"
         a0
# 2766 "wave.c" 3
         ) & 0x3)
# 2766 "wave.c"
                   ) {
 case 
# 2767 "wave.c" 3
     0x2
# 2767 "wave.c"
           :
  if (a0 != 
# 2768 "wave.c" 3
           ((q)(((unsigned long)(
# 2768 "wave.c"
           311L+
# 2768 "wave.c" 3
           2) << 4) + (0x2 +0x4)))
# 2768 "wave.c"
                            ) goto acknowledged_3_interrupt;
  
# 2769 "wave.c" 3
 { allocp = do_unify(allocp, (
# 2769 "wave.c"
 a2
# 2769 "wave.c" 3
 ), (
# 2769 "wave.c"
 a1
# 2769 "wave.c" 3
 )); }
# 2769 "wave.c"
              ;
  
# 2770 "wave.c" 3
 { goto proceed_label; }
# 2770 "wave.c"
          ;
 case 
# 2771 "wave.c" 3
     0x0
# 2771 "wave.c"
           :
  
# 2772 "wave.c" 3
 { q temp0 = (*((q *)((unsigned long)(
# 2772 "wave.c"
 a0
# 2772 "wave.c" 3
 ) - 0x0))); if(!(((unsigned long)(temp0) & 0x3) == 0x0)) { (
# 2772 "wave.c"
 a0
# 2772 "wave.c" 3
 ) = temp0; goto 
# 2772 "wave.c"
 acknowledged_3_1
# 2772 "wave.c" 3
 ; } }
# 2772 "wave.c"
                                    ;
  *reasonp++ = a0;
  goto acknowledged_3_interrupt;
  };
  goto acknowledged_3_interrupt;
 acknowledged_3_ext_interrupt:
  reasonp = 0l;
 acknowledged_3_interrupt:
  toppred = &predicate_main_xacknowledged_3;
  goto interrupt_3;
 }

 acknowledged_4_top: {
  a0 = qp->args[0];
  a1 = qp->args[1];
  a2 = qp->args[2];
  a3 = qp->args[3];
  qp = qp->next;
 acknowledged_4_clear_reason:
  reasonp = 
# 2791 "wave.c" 3
           (glbl->reasons0)
# 2791 "wave.c"
                  ;
 acknowledged_4_0:
 acknowledged_4_1:
  switch (
# 2794 "wave.c" 3
         ((unsigned long)(
# 2794 "wave.c"
         a0
# 2794 "wave.c" 3
         ) & 0x3)
# 2794 "wave.c"
                   ) {
 case 
# 2795 "wave.c" 3
     0x2
# 2795 "wave.c"
           :
  if (a0 != 
# 2796 "wave.c" 3
           ((q)(((unsigned long)(
# 2796 "wave.c"
           311L+
# 2796 "wave.c" 3
           2) << 4) + (0x2 +0x4)))
# 2796 "wave.c"
                            ) goto acknowledged_4_interrupt;
 acknowledged_4_2:
  switch (
# 2798 "wave.c" 3
         ((unsigned long)(
# 2798 "wave.c"
         a1
# 2798 "wave.c" 3
         ) & 0x3)
# 2798 "wave.c"
                   ) {
 case 
# 2799 "wave.c" 3
     0x2
# 2799 "wave.c"
           :
  if (a1 != 
# 2800 "wave.c" 3
           ((q)(((unsigned long)(
# 2800 "wave.c"
           311L+
# 2800 "wave.c" 3
           2) << 4) + (0x2 +0x4)))
# 2800 "wave.c"
                            ) goto acknowledged_4_interrupt;
  
# 2801 "wave.c" 3
 { allocp = do_unify(allocp, (
# 2801 "wave.c"
 a3
# 2801 "wave.c" 3
 ), (
# 2801 "wave.c"
 a2
# 2801 "wave.c" 3
 )); }
# 2801 "wave.c"
              ;
  
# 2802 "wave.c" 3
 { goto proceed_label; }
# 2802 "wave.c"
          ;
 case 
# 2803 "wave.c" 3
     0x0
# 2803 "wave.c"
           :
  
# 2804 "wave.c" 3
 { q temp0 = (*((q *)((unsigned long)(
# 2804 "wave.c"
 a1
# 2804 "wave.c" 3
 ) - 0x0))); if(!(((unsigned long)(temp0) & 0x3) == 0x0)) { (
# 2804 "wave.c"
 a1
# 2804 "wave.c" 3
 ) = temp0; goto 
# 2804 "wave.c"
 acknowledged_4_2
# 2804 "wave.c" 3
 ; } }
# 2804 "wave.c"
                                    ;
  *reasonp++ = a1;
  goto acknowledged_4_interrupt;
  };
  goto acknowledged_4_interrupt;
 case 
# 2809 "wave.c" 3
     0x0
# 2809 "wave.c"
           :
  
# 2810 "wave.c" 3
 { q temp0 = (*((q *)((unsigned long)(
# 2810 "wave.c"
 a0
# 2810 "wave.c" 3
 ) - 0x0))); if(!(((unsigned long)(temp0) & 0x3) == 0x0)) { (
# 2810 "wave.c"
 a0
# 2810 "wave.c" 3
 ) = temp0; goto 
# 2810 "wave.c"
 acknowledged_4_1
# 2810 "wave.c" 3
 ; } }
# 2810 "wave.c"
                                    ;
  *reasonp++ = a0;
  goto acknowledged_4_interrupt;
  };
  goto acknowledged_4_interrupt;
 acknowledged_4_ext_interrupt:
  reasonp = 0l;
 acknowledged_4_interrupt:
  toppred = &predicate_main_xacknowledged_4;
  goto interrupt_4;
 }

 gen_3_top: {
  q x0, x1, x2, x3, x4, x5, x6;
  a0 = qp->args[0];
  a1 = qp->args[1];
  a2 = qp->args[2];
  qp = qp->next;
 gen_3_clear_reason:
  reasonp = 
# 2829 "wave.c" 3
           (glbl->reasons0)
# 2829 "wave.c"
                  ;
 gen_3_0:
 gen_3_1:
  if (!
# 2832 "wave.c" 3
      (((unsigned long)(
# 2832 "wave.c"
      a0
# 2832 "wave.c" 3
      ) & 0xFL) == (0x2 +0x0))
# 2832 "wave.c"
               ) goto gen_3_3;
  x0 = 
# 2833 "wave.c" 3
      ((q)(((long)(
# 2833 "wave.c"
      0L
# 2833 "wave.c" 3
      ) << 4) + (0x2 +0x0)))
# 2833 "wave.c"
                 ;
  
# 2834 "wave.c" 3
 if ((long)(
# 2834 "wave.c"
 a0
# 2834 "wave.c" 3
 ) <= (long)(
# 2834 "wave.c"
 x0
# 2834 "wave.c" 3
 )) goto 
# 2834 "wave.c"
 gen_3_2
# 2834 "wave.c" 3
 ;
# 2834 "wave.c"
                            ;
  x1 = 
# 2835 "wave.c" 3
      ((q)(((long)(
# 2835 "wave.c"
      1L
# 2835 "wave.c" 3
      ) << 4) + (0x2 +0x0)))
# 2835 "wave.c"
                 ;
  
# 2836 "wave.c" 3
 { 
# 2836 "wave.c"
 x2 
# 2836 "wave.c" 3
 = (q)((unsigned long)(
# 2836 "wave.c"
 a0
# 2836 "wave.c" 3
 ) - ((unsigned long)(
# 2836 "wave.c"
 x1
# 2836 "wave.c" 3
 ) - (0x2 +0x0))); }
# 2836 "wave.c"
                            ;
  allocp[0] = x4 = 
# 2837 "wave.c" 3
                  (q) ((unsigned long)(
# 2837 "wave.c"
                  &allocp[0]
# 2837 "wave.c" 3
                  ) + 0x0)
# 2837 "wave.c"
                                     ;
  allocp[1] = x5 = 
# 2838 "wave.c" 3
                  (q) ((unsigned long)(
# 2838 "wave.c"
                  &allocp[1]
# 2838 "wave.c" 3
                  ) + 0x0)
# 2838 "wave.c"
                                     ;
  x3 = 
# 2839 "wave.c" 3
      ((q)((unsigned long)(
# 2839 "wave.c"
      &allocp[0]
# 2839 "wave.c" 3
      ) + 0x1))
# 2839 "wave.c"
                          ;
  allocp += 2;
  
# 2841 "wave.c" 3
 { if (!(((unsigned long)(
# 2841 "wave.c"
 a2
# 2841 "wave.c" 3
 ) & 0x3) == 0x0) || (*((q *)((unsigned long)(
# 2841 "wave.c"
 a2
# 2841 "wave.c" 3
 ) - 0x0))) != (
# 2841 "wave.c"
 a2
# 2841 "wave.c" 3
 )) { allocp = do_unify_value(allocp, (
# 2841 "wave.c"
 a2
# 2841 "wave.c" 3
 ), (
# 2841 "wave.c"
 x3
# 2841 "wave.c" 3
 )); } else { (*((q *)((unsigned long)(
# 2841 "wave.c"
 a2
# 2841 "wave.c" 3
 ) - 0x0))) = (
# 2841 "wave.c"
 x3
# 2841 "wave.c" 3
 ); } }
# 2841 "wave.c"
                    ;
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_main_xgen_3);
  allocp[2] = x2;
  allocp[3] = a1;
  allocp[4] = x4;
  a0 = a1;
  a1 = x5;
  qp = (struct goalrec*)&allocp[0];
  allocp += 5;
  
# 2851 "wave.c" 3
 { if (allocp < (glbl->heaplimit0)) goto 
# 2851 "wave.c"
 gen1_2_0
# 2851 "wave.c" 3
 ;}
# 2851 "wave.c"
                  ;
  goto gen1_2_ext_interrupt;
 gen_3_2:
  if (a0 != 
# 2854 "wave.c" 3
           ((q)(((long)(
# 2854 "wave.c"
           0L
# 2854 "wave.c" 3
           ) << 4) + (0x2 +0x0)))
# 2854 "wave.c"
                      ) goto gen_3_interrupt;
  x6 = 
# 2855 "wave.c" 3
      ((q)(((unsigned long)(0) << 4) + (0x2 +0x4)))
# 2855 "wave.c"
             ;
  
# 2856 "wave.c" 3
 { if (!(((unsigned long)(
# 2856 "wave.c"
 a2
# 2856 "wave.c" 3
 ) & 0x3) == 0x0) || (*((q *)((unsigned long)(
# 2856 "wave.c"
 a2
# 2856 "wave.c" 3
 ) - 0x0))) != (
# 2856 "wave.c"
 a2
# 2856 "wave.c" 3
 )) { allocp = do_unify_value(allocp, (
# 2856 "wave.c"
 a2
# 2856 "wave.c" 3
 ), (
# 2856 "wave.c"
 x6
# 2856 "wave.c" 3
 )); } else { (*((q *)((unsigned long)(
# 2856 "wave.c"
 a2
# 2856 "wave.c" 3
 ) - 0x0))) = (
# 2856 "wave.c"
 x6
# 2856 "wave.c" 3
 ); } }
# 2856 "wave.c"
                    ;
  
# 2857 "wave.c" 3
 { goto proceed_label; }
# 2857 "wave.c"
          ;
 gen_3_3:
  if (!
# 2859 "wave.c" 3
      (((unsigned long)(
# 2859 "wave.c"
      a0
# 2859 "wave.c" 3
      ) & 0x3) == 0x0)
# 2859 "wave.c"
               ) goto gen_3_interrupt;
  
# 2860 "wave.c" 3
 { q temp0 = (*((q *)((unsigned long)(
# 2860 "wave.c"
 a0
# 2860 "wave.c" 3
 ) - 0x0))); if(!(((unsigned long)(temp0) & 0x3) == 0x0)) { (
# 2860 "wave.c"
 a0
# 2860 "wave.c" 3
 ) = temp0; goto 
# 2860 "wave.c"
 gen_3_1
# 2860 "wave.c" 3
 ; } }
# 2860 "wave.c"
                           ;
  *reasonp++ = a0;
  goto gen_3_interrupt;
 gen_3_ext_interrupt:
  reasonp = 0l;
 gen_3_interrupt:
  goto interrupt_3;
 }

 gen1_2_top: {
  q x0, x1, x2, x3, x4, x5, x6, x7;
  a0 = qp->args[0];
  a1 = qp->args[1];
  qp = qp->next;
 gen1_2_clear_reason:
  reasonp = 
# 2875 "wave.c" 3
           (glbl->reasons0)
# 2875 "wave.c"
                  ;
 gen1_2_0:
 gen1_2_1:
  if (!
# 2878 "wave.c" 3
      (((unsigned long)(
# 2878 "wave.c"
      a0
# 2878 "wave.c" 3
      ) & 0xFL) == (0x2 +0x0))
# 2878 "wave.c"
               ) goto gen1_2_3;
  x0 = 
# 2879 "wave.c" 3
      ((q)(((long)(
# 2879 "wave.c"
      0L
# 2879 "wave.c" 3
      ) << 4) + (0x2 +0x0)))
# 2879 "wave.c"
                 ;
  
# 2880 "wave.c" 3
 if ((long)(
# 2880 "wave.c"
 a0
# 2880 "wave.c" 3
 ) <= (long)(
# 2880 "wave.c"
 x0
# 2880 "wave.c" 3
 )) goto 
# 2880 "wave.c"
 gen1_2_2
# 2880 "wave.c" 3
 ;
# 2880 "wave.c"
                             ;
  x1 = 
# 2881 "wave.c" 3
      ((q)(((long)(
# 2881 "wave.c"
      1L
# 2881 "wave.c" 3
      ) << 4) + (0x2 +0x0)))
# 2881 "wave.c"
                 ;
  
# 2882 "wave.c" 3
 { 
# 2882 "wave.c"
 x2 
# 2882 "wave.c" 3
 = (q)((unsigned long)(
# 2882 "wave.c"
 a0
# 2882 "wave.c" 3
 ) - ((unsigned long)(
# 2882 "wave.c"
 x1
# 2882 "wave.c" 3
 ) - (0x2 +0x0))); }
# 2882 "wave.c"
                            ;
  allocp[0] = x3 = 
# 2883 "wave.c" 3
                  (q) ((unsigned long)(
# 2883 "wave.c"
                  &allocp[0]
# 2883 "wave.c" 3
                  ) + 0x0)
# 2883 "wave.c"
                                     ;
  allocp[1] = 
# 2884 "wave.c" 3
             ((q)(((unsigned long)(0) << 4) + (0x2 +0x4)))
# 2884 "wave.c"
                    ;
  allocp[2] = x3;
  x4 = create_vector(&allocp[1], 2, allocp);
  allocp[3] = x6 = 
# 2887 "wave.c" 3
                  (q) ((unsigned long)(
# 2887 "wave.c"
                  &allocp[3]
# 2887 "wave.c" 3
                  ) + 0x0)
# 2887 "wave.c"
                                     ;
  allocp[4] = x4;
  x5 = 
# 2889 "wave.c" 3
      ((q)((unsigned long)(
# 2889 "wave.c"
      &allocp[3]
# 2889 "wave.c" 3
      ) + 0x1))
# 2889 "wave.c"
                          ;
  allocp += 5;
  
# 2891 "wave.c" 3
 { if (!(((unsigned long)(
# 2891 "wave.c"
 a1
# 2891 "wave.c" 3
 ) & 0x3) == 0x0) || (*((q *)((unsigned long)(
# 2891 "wave.c"
 a1
# 2891 "wave.c" 3
 ) - 0x0))) != (
# 2891 "wave.c"
 a1
# 2891 "wave.c" 3
 )) { allocp = do_unify_value(allocp, (
# 2891 "wave.c"
 a1
# 2891 "wave.c" 3
 ), (
# 2891 "wave.c"
 x5
# 2891 "wave.c" 3
 )); } else { (*((q *)((unsigned long)(
# 2891 "wave.c"
 a1
# 2891 "wave.c" 3
 ) - 0x0))) = (
# 2891 "wave.c"
 x5
# 2891 "wave.c" 3
 ); } }
# 2891 "wave.c"
                    ;
  a0 = x2;
  a1 = x6;
  
# 2894 "wave.c" 3
 { if (allocp < (glbl->heaplimit0)) goto 
# 2894 "wave.c"
 gen1_2_0
# 2894 "wave.c" 3
 ;}
# 2894 "wave.c"
                  ;
  goto gen1_2_ext_interrupt;
 gen1_2_2:
  if (a0 != 
# 2897 "wave.c" 3
           ((q)(((long)(
# 2897 "wave.c"
           0L
# 2897 "wave.c" 3
           ) << 4) + (0x2 +0x0)))
# 2897 "wave.c"
                      ) goto gen1_2_interrupt;
  x7 = 
# 2898 "wave.c" 3
      ((q)(((unsigned long)(0) << 4) + (0x2 +0x4)))
# 2898 "wave.c"
             ;
  
# 2899 "wave.c" 3
 { if (!(((unsigned long)(
# 2899 "wave.c"
 a1
# 2899 "wave.c" 3
 ) & 0x3) == 0x0) || (*((q *)((unsigned long)(
# 2899 "wave.c"
 a1
# 2899 "wave.c" 3
 ) - 0x0))) != (
# 2899 "wave.c"
 a1
# 2899 "wave.c" 3
 )) { allocp = do_unify_value(allocp, (
# 2899 "wave.c"
 a1
# 2899 "wave.c" 3
 ), (
# 2899 "wave.c"
 x7
# 2899 "wave.c" 3
 )); } else { (*((q *)((unsigned long)(
# 2899 "wave.c"
 a1
# 2899 "wave.c" 3
 ) - 0x0))) = (
# 2899 "wave.c"
 x7
# 2899 "wave.c" 3
 ); } }
# 2899 "wave.c"
                    ;
  
# 2900 "wave.c" 3
 { goto proceed_label; }
# 2900 "wave.c"
          ;
 gen1_2_3:
  if (!
# 2902 "wave.c" 3
      (((unsigned long)(
# 2902 "wave.c"
      a0
# 2902 "wave.c" 3
      ) & 0x3) == 0x0)
# 2902 "wave.c"
               ) goto gen1_2_interrupt;
  
# 2903 "wave.c" 3
 { q temp0 = (*((q *)((unsigned long)(
# 2903 "wave.c"
 a0
# 2903 "wave.c" 3
 ) - 0x0))); if(!(((unsigned long)(temp0) & 0x3) == 0x0)) { (
# 2903 "wave.c"
 a0
# 2903 "wave.c" 3
 ) = temp0; goto 
# 2903 "wave.c"
 gen1_2_1
# 2903 "wave.c" 3
 ; } }
# 2903 "wave.c"
                            ;
  *reasonp++ = a0;
  goto gen1_2_interrupt;
 gen1_2_ext_interrupt:
  reasonp = 0l;
 gen1_2_interrupt:
  toppred = &predicate_main_xgen1_2;
  goto interrupt_2;
 }

 end_1_top: {
  q x0;
  a0 = qp->args[0];
  qp = qp->next;
 end_1_clear_reason:
  reasonp = 
# 2918 "wave.c" 3
           (glbl->reasons0)
# 2918 "wave.c"
                  ;
 end_1_0:
 end_1_1:
  switch (
# 2921 "wave.c" 3
         ((unsigned long)(
# 2921 "wave.c"
         a0
# 2921 "wave.c" 3
         ) & 0x3)
# 2921 "wave.c"
                   ) {
 case 
# 2922 "wave.c" 3
     0x1
# 2922 "wave.c"
         :
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_main_xend_1);
  x0 = 
# 2925 "wave.c" 3
      (((struct cons *)((unsigned long)(
# 2925 "wave.c"
      a0
# 2925 "wave.c" 3
      ) - 0x1))->cdr)
# 2925 "wave.c"
                ;
  allocp[2] = x0;
  a0 = 
# 2927 "wave.c" 3
      (((struct cons *)((unsigned long)(
# 2927 "wave.c"
      a0
# 2927 "wave.c" 3
      ) - 0x1))->car)
# 2927 "wave.c"
                ;
  qp = (struct goalrec*)&allocp[0];
  allocp += 3;
  
# 2930 "wave.c" 3
 { if (allocp < (glbl->heaplimit0)) goto 
# 2930 "wave.c"
 end1_1_0
# 2930 "wave.c" 3
 ;}
# 2930 "wave.c"
                  ;
  goto end1_1_ext_interrupt;
 case 
# 2932 "wave.c" 3
     0x2
# 2932 "wave.c"
           :
  if (a0 != 
# 2933 "wave.c" 3
           ((q)(((unsigned long)(0) << 4) + (0x2 +0x4)))
# 2933 "wave.c"
                  ) goto end_1_interrupt;
  
# 2934 "wave.c" 3
 { goto proceed_label; }
# 2934 "wave.c"
          ;
 case 
# 2935 "wave.c" 3
     0x0
# 2935 "wave.c"
           :
  
# 2936 "wave.c" 3
 { q temp0 = (*((q *)((unsigned long)(
# 2936 "wave.c"
 a0
# 2936 "wave.c" 3
 ) - 0x0))); if(!(((unsigned long)(temp0) & 0x3) == 0x0)) { (
# 2936 "wave.c"
 a0
# 2936 "wave.c" 3
 ) = temp0; goto 
# 2936 "wave.c"
 end_1_1
# 2936 "wave.c" 3
 ; } }
# 2936 "wave.c"
                           ;
  *reasonp++ = a0;
  goto end_1_interrupt;
  };
  goto end_1_interrupt;
 end_1_ext_interrupt:
  reasonp = 0l;
 end_1_interrupt:
  toppred = &predicate_main_xend_1;
  goto interrupt_1;
 }

 end1_1_top: {
  q x0, x1, x2, x3, x4, x5, x6;
  a0 = qp->args[0];
  qp = qp->next;
 end1_1_clear_reason:
  reasonp = 
# 2953 "wave.c" 3
           (glbl->reasons0)
# 2953 "wave.c"
                  ;
 end1_1_0:
 end1_1_1:
  switch (
# 2956 "wave.c" 3
         ((unsigned long)(
# 2956 "wave.c"
         a0
# 2956 "wave.c" 3
         ) & 0x3)
# 2956 "wave.c"
                   ) {
 case 
# 2957 "wave.c" 3
     0x1
# 2957 "wave.c"
         :
  x0 = 
# 2958 "wave.c" 3
      (((struct cons *)((unsigned long)(
# 2958 "wave.c"
      a0
# 2958 "wave.c" 3
      ) - 0x1))->car)
# 2958 "wave.c"
                ;
 end1_1_2:
  switch (
# 2960 "wave.c" 3
         ((unsigned long)(
# 2960 "wave.c"
         x0
# 2960 "wave.c" 3
         ) & 0x3)
# 2960 "wave.c"
                   ) {
 case 
# 2961 "wave.c" 3
     0x3
# 2961 "wave.c"
            :
 end1_1_3:
  if (!
# 2963 "wave.c" 3
      ((((unsigned long)((((struct functor *)((unsigned long)(
# 2963 "wave.c"
      x0
# 2963 "wave.c" 3
      ) - 0x3))->functor)) & 0x3) == 0x0))
# 2963 "wave.c"
                ) goto end1_1_interrupt;
  if (!
# 2964 "wave.c" 3
      ((struct data_object_method_table *)((((struct functor *)((unsigned long)(
# 2964 "wave.c"
      x0
# 2964 "wave.c" 3
      ) - 0x3))->functor)) == &
# 2964 "wave.c"
      vector_g_data_method_table
# 2964 "wave.c" 3
      )
# 2964 "wave.c"
                        ) goto end1_1_interrupt;
  
# 2965 "wave.c" 3
 { extern q size_of_vector(); 
# 2965 "wave.c"
 x1 
# 2965 "wave.c" 3
 = size_of_vector(
# 2965 "wave.c"
 x0
# 2965 "wave.c" 3
 ); }
# 2965 "wave.c"
                                            ;
 end1_1_4:
  if (x1 != 
# 2967 "wave.c" 3
           ((q)(((long)(
# 2967 "wave.c"
           2L
# 2967 "wave.c" 3
           ) << 4) + (0x2 +0x0)))
# 2967 "wave.c"
                      ) goto end1_1_interrupt;
  x3 = 
# 2968 "wave.c" 3
      ((q)(((long)(
# 2968 "wave.c"
      0L
# 2968 "wave.c" 3
      ) << 4) + (0x2 +0x0)))
# 2968 "wave.c"
                 ;
  
# 2969 "wave.c" 3
 { extern q element_of_vector(); 
# 2969 "wave.c"
 x2 
# 2969 "wave.c" 3
 = element_of_vector(
# 2969 "wave.c"
 x0
# 2969 "wave.c" 3
 ,
# 2969 "wave.c"
 x3
# 2969 "wave.c" 3
 ); if ((long)
# 2969 "wave.c"
 x2 
# 2969 "wave.c" 3
 == 0) goto 
# 2969 "wave.c"
 end1_1_interrupt
# 2969 "wave.c" 3
 ; }
# 2969 "wave.c"
                                                  ;
  x5 = 
# 2970 "wave.c" 3
      ((q)(((long)(
# 2970 "wave.c"
      1L
# 2970 "wave.c" 3
      ) << 4) + (0x2 +0x0)))
# 2970 "wave.c"
                 ;
  
# 2971 "wave.c" 3
 { extern q element_of_vector(); 
# 2971 "wave.c"
 x4 
# 2971 "wave.c" 3
 = element_of_vector(
# 2971 "wave.c"
 x0
# 2971 "wave.c" 3
 ,
# 2971 "wave.c"
 x5
# 2971 "wave.c" 3
 ); if ((long)
# 2971 "wave.c"
 x4 
# 2971 "wave.c" 3
 == 0) goto 
# 2971 "wave.c"
 end1_1_interrupt
# 2971 "wave.c" 3
 ; }
# 2971 "wave.c"
                                                  ;
  x6 = 
# 2972 "wave.c" 3
      ((q)(((unsigned long)(0) << 4) + (0x2 +0x4)))
# 2972 "wave.c"
             ;
  
# 2973 "wave.c" 3
 { if (!(((unsigned long)(
# 2973 "wave.c"
 x4
# 2973 "wave.c" 3
 ) & 0x3) == 0x0) || (*((q *)((unsigned long)(
# 2973 "wave.c"
 x4
# 2973 "wave.c" 3
 ) - 0x0))) != (
# 2973 "wave.c"
 x4
# 2973 "wave.c" 3
 )) { allocp = do_unify_value(allocp, (
# 2973 "wave.c"
 x4
# 2973 "wave.c" 3
 ), (
# 2973 "wave.c"
 x6
# 2973 "wave.c" 3
 )); } else { (*((q *)((unsigned long)(
# 2973 "wave.c"
 x4
# 2973 "wave.c" 3
 ) - 0x0))) = (
# 2973 "wave.c"
 x6
# 2973 "wave.c" 3
 ); } }
# 2973 "wave.c"
                    ;
  a0 = 
# 2974 "wave.c" 3
      (((struct cons *)((unsigned long)(
# 2974 "wave.c"
      a0
# 2974 "wave.c" 3
      ) - 0x1))->cdr)
# 2974 "wave.c"
                ;
  
# 2975 "wave.c" 3
 { if (allocp < (glbl->heaplimit0)) goto 
# 2975 "wave.c"
 end1_1_0
# 2975 "wave.c" 3
 ;}
# 2975 "wave.c"
                  ;
  goto end1_1_ext_interrupt;
 case 
# 2977 "wave.c" 3
     0x0
# 2977 "wave.c"
           :
  
# 2978 "wave.c" 3
 { q temp0 = (*((q *)((unsigned long)(
# 2978 "wave.c"
 x0
# 2978 "wave.c" 3
 ) - 0x0))); if(!(((unsigned long)(temp0) & 0x3) == 0x0)) { (
# 2978 "wave.c"
 x0
# 2978 "wave.c" 3
 ) = temp0; goto 
# 2978 "wave.c"
 end1_1_2
# 2978 "wave.c" 3
 ; } }
# 2978 "wave.c"
                            ;
  *reasonp++ = x0;
  goto end1_1_interrupt;
  };
  goto end1_1_interrupt;
 case 
# 2983 "wave.c" 3
     0x2
# 2983 "wave.c"
           :
  if (a0 != 
# 2984 "wave.c" 3
           ((q)(((unsigned long)(0) << 4) + (0x2 +0x4)))
# 2984 "wave.c"
                  ) goto end1_1_interrupt;
  
# 2985 "wave.c" 3
 { goto proceed_label; }
# 2985 "wave.c"
          ;
 case 
# 2986 "wave.c" 3
     0x0
# 2986 "wave.c"
           :
  
# 2987 "wave.c" 3
 { q temp0 = (*((q *)((unsigned long)(
# 2987 "wave.c"
 a0
# 2987 "wave.c" 3
 ) - 0x0))); if(!(((unsigned long)(temp0) & 0x3) == 0x0)) { (
# 2987 "wave.c"
 a0
# 2987 "wave.c" 3
 ) = temp0; goto 
# 2987 "wave.c"
 end1_1_1
# 2987 "wave.c" 3
 ; } }
# 2987 "wave.c"
                            ;
  *reasonp++ = a0;
  goto end1_1_interrupt;
  };
  goto end1_1_interrupt;
 end1_1_ext_interrupt:
  reasonp = 0l;
 end1_1_interrupt:
  toppred = &predicate_main_xend1_1;
  goto interrupt_1;
 }
 interrupt_16:
  allocp[17] = a15;
 interrupt_15:
  allocp[16] = a14;
 interrupt_14:
  allocp[15] = a13;
 interrupt_13:
  allocp[14] = a12;
 interrupt_12:
  allocp[13] = a11;
 interrupt_11:
  allocp[12] = a10;
 interrupt_10:
  allocp[11] = a9;
 interrupt_9:
  allocp[10] = a8;
 interrupt_8:
  allocp[9] = a7;
 interrupt_7:
  allocp[8] = a6;
 interrupt_6:
  allocp[7] = a5;
 interrupt_5:
  allocp[6] = a4;
 interrupt_4:
  allocp[5] = a3;
 interrupt_3:
  allocp[4] = a2;
 interrupt_2:
  allocp[3] = a1;
 interrupt_1:
  allocp[2] = a0;
 interrupt_0:
  allocp = interrupt_goal(allocp, toppred, reasonp);
 proceed_label:
  
# 3033 "wave.c" 3
 { module (*func)(); if (allocp >= (glbl->heaplimit0)) { allocp = klic_interrupt(allocp, qp); qp = (glbl->current_queue0); } if ((func = (toppred = qp->pred)->func) == (
# 3033 "wave.c"
 module_main
# 3033 "wave.c" 3
 )) goto module_top; (glbl->heapp0) = allocp; (glbl->current_queue0) = qp; return (module) func; }
# 3033 "wave.c"
                                ;
}
