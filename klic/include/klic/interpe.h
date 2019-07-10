/* ---------------------------------------------------------- 
%   (C) 1994,1995 Institute for New Generation Computer Technology 
%       (Read COPYRIGHT for detailed information.) 
%   (C)1996, 1997, 1998, 1999 Japan Information Processing Development Center
%       (Read COPYRIGHT-JIPDEC for detailed information.)
----------------------------------------------------------- */
#include <malloc.h>
#ifdef __alpha
/* dec malloc.h does not supply function return value decralations */
void *malloc();
void *calloc();
void *realloc();
#endif
#include <string.h>

#include <klic/distpkt.h>

/********** for debugging output control */
/*#define INT_CL_DEBUG*/
#define INT_CL_PROBE

#ifdef	INT_CL_DEBUG
#  define INT_CL_DEBUG_X(X) X
#else
#  define INT_CL_DEBUG_X(X)
#endif

#ifdef	INT_CL_PROBE
#  define INT_CL_PROBE_X(X) X
#else
#  define INT_CL_PROBE_X(X)
#endif

/*********** for bsd libraries */
#ifdef USEBCOPY
#  define BCOPY(from,to,len)	bcopy(from,to,len)
#else
#  define BCOPY(from,to,len)	memcpy(to,from,len)
#endif
#ifdef USEBZERO
#  define BZERO(from,len)		bzero(from,len)
#else
#  define BZERO(from,len)		memset(from,0,len)
#endif

/*********** Some convenient macros **************/
/*Extern long total_node, my_node;*/
#define MASTER_NODE		0L
#define SHOEN_NODE		total_node
#define IS_MASTER_NODE(node)	((node) == MASTER_NODE)
#define IS_SHOEN_NODE(node)	((node) == SHOEN_NODE)

/* Constants */
#define EXPORT_UNIT_WEC		0x10000
#define MIN_WEC			0x2

#define EXP_TABLE_INC_SIZE	0x1000
#define IMP_TABLE_INC_SIZE	0x1000

#define EXP_TABLE_HASH_SIZE	0x1

#define END_IMP_FREELIST	(-1)
#define END_IMP_TABLE		(-1)

#define IMPREC_NOT_COPIED	12345
#define IMPREC_COPIED		54321
#define IMPREC_RELEASED		98765

/* Temporary. for PVM-TCP bug */
#ifndef TRANSFER_CNT
#define TRANSFER_CNT            0x1000L
#endif

/************* WTC related definition ***************/
Extern long shoen_wtc;
Extern long node_wtc;

#define SUPPLY_WTC_UNIT		0x1000000L
#define THROW_WTC_UNIT		0x1000L
#define UNIFY_WTC_UNIT		0x10L


/* if defined, optional eager requesting is activated. */
/*#define REQUEST_WTC_THRESHOLD	0x3L*/

/*#define CONTROL_WTC_UNIT	0x1L	/*This value implicitly assumed. */

#define NODE_WTC_LIMIT		0x10000000L
#define RETURN_WTC_UNIT		0x08000000L

#define MIN_WTC			2L /* one for wtc_request,
				      another for node existence */
#define READ_WTC_UNIT		10L
#define ANSWER_WTC_UNIT		10L

enum get_wtc_type {
    get_wtc_normal_request,
    get_wtc_full_request,
    get_wtc_force_request
};
/************* Export Table *********************/
struct exp_entry{
    long index;
    long wec;
    q data;
    struct exp_entry *next;
};

Extern struct exp_entry *exp_table;
Extern long current_exp_size;
Extern long active_exp_entry;

#define UNUSED_EXPREC 0xf0f0f0ff /* This value is curious!! */
#define UNUSED_IMPREC 0xf0f0f0ff /* it too. */

/* Export Hash Table */
Extern struct exp_entry *exp_hash_table[EXP_TABLE_HASH_SIZE];

Extern struct exp_entry *top_of_exp_freelist;

/************** Import Table ********************/

struct imp_entry{
    long next;
    q object;
};

Extern struct imp_entry *imp_table;
Extern long current_imp_size;

Extern struct imp_entry top_of_imp_freelist;

Extern long active_imp_rec;
Extern long free_imp_rec;

Extern long receive_answer_flag;
Extern long answer_return_exp_index;
Extern q decode_data;		/* Un natural way of value passing. */

/* Options set by program arguments */
Extern long eager_transfer_level	Init(0);
Extern int network_statistics_flag	Init(0);
Extern int msg_busywait_flag		Init(0);

/************* message suspend record **************/

struct susp_msg_rec {
    struct susp_msg_rec *next;
    long node;
    long wtc_needed;
    combuf *outbuf;
};
Extern struct susp_msg_rec *susp_msg_list;

/************* Message decoder stack ****************/

#define INIT_DECODE_STACK_SIZE 0x1000

Extern q *decode_stack;
Extern q *decode_stack_ptr;
Extern q *decode_stack_limit;
Extern int decode_stack_size;

/************* Network Statistics Measurement *******/

Extern struct netstat {
    long start_sec;		/* only SHOEN uses this field */
    long start_usec;		/* only SHOEN uses this field */

    long send_throw_cnt;
    long send_read_cnt;
    long send_unify_cnt;
    long send_answer_cnt;
    long send_release_cnt;
    long send_request_wtc_cnt;
    long send_supply_wtc_cnt;
    long send_return_wtc_cnt;
    
    long recv_throw_cnt;
    long recv_read_cnt;
    long recv_unify_cnt;
    long recv_release_cnt;
    long recv_answer_cnt;
    long recv_request_wtc_cnt;
    long recv_supply_wtc_cnt;
    long recv_return_wtc_cnt;

    long msg_suspend_cnt;
    long msg_resume_cnt;
    
    long send_data_siz;
    long recv_data_siz;

} netstat;

#define ERROR_STOP for(;;)

