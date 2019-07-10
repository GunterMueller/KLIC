/* ---------------------------------------------------------- 
%   (C)1994,1995 Institute for New Generation Computer Technology 
%       (Read COPYRIGHT for detailed information.) 
%   (C)1996, 1997, 1998, 1999 Japan Information Processing Development Center
%       (Read COPYRIGHT-JIPDEC for detailed information.)
----------------------------------------------------------- */
#ifndef _KLIC_DISTPKT_H_
#define _KLIC_DISTPKT_H_

#include <klic/sighndl.h>

/* Maximum ratio of the output KL1 words created by one communication word. */
#define DECODE_MAX_OBJ_RATIO	(((6+3+1+5) + 2)/3)


/* Network Buffer Type */
typedef struct {
    Volatile long *buffer;		/* pointer to the actual buffer */
    Volatile long rd_index;		/* pointer used for reading */
    Volatile long wt_index;		/* pointer used for writing */
    Volatile long buf_size;		/* the physical size of buffer */
} combuf;

Extern combuf *kumon_inbuf;	/* for input packet */
Extern combuf *kumon_outbuf;	/* for output packet */

Extern combuf *pending_msg Init(0);

/* Buffer I/O primitives and its associates */
#define IS_BUFFER_EMPTY(inbuf)	((inbuf)->rd_index == (inbuf)->wt_index)
#define NODE_TO_BUFFER(node)	kumon_outbuf
#define GET_BUFFER(inbuf)	((inbuf)->buffer[(inbuf)->rd_index++])

#define PUT_BUFFER(outbuf, data) 				\
{								\
    (outbuf)->buffer[(outbuf)->wt_index] = (long)(data);	\
    (outbuf)->wt_index++;					\
    if ((outbuf)->wt_index >= (outbuf)->buf_size){		\
	expand_outbuf(outbuf);					\
    }								\
}
/* The followings are needed to optimize the buffer operations */
#define PUT2_BUFFER(outbuf, data, data1)			\
{								\
    if ((outbuf)->wt_index + 2 >= (outbuf)->buf_size)		\
	expand_outbuf(outbuf);					\
    (outbuf)->buffer[(outbuf)->wt_index] = (long)(data);	\
    (outbuf)->buffer[(outbuf)->wt_index+1] = (long)(data1);	\
    (outbuf)->wt_index += 2;					\
}

#define PUT3_BUFFER(outbuf, data, data1, data2)			\
{								\
    if ((outbuf)->wt_index + 3 >= (outbuf)->buf_size)		\
	expand_outbuf(outbuf);					\
    (outbuf)->buffer[(outbuf)->wt_index] = (long)(data);	\
    (outbuf)->buffer[(outbuf)->wt_index + 1] = (long)(data1);	\
    (outbuf)->buffer[(outbuf)->wt_index + 2] = (long)(data2);	\
    (outbuf)->wt_index += 3;					\
}

#define PUT4_BUFFER(outbuf, data, data1, data2, data3)		\
{								\
    if ((outbuf)->wt_index + 4 >= (outbuf)->buf_size)		\
	expand_outbuf(outbuf);					\
    (outbuf)->buffer[(outbuf)->wt_index] = (long)(data);	\
    (outbuf)->buffer[(outbuf)->wt_index + 1] = (long)(data1);	\
    (outbuf)->buffer[(outbuf)->wt_index + 2] = (long)(data2);	\
    (outbuf)->buffer[(outbuf)->wt_index + 3] = (long)(data3);	\
    (outbuf)->wt_index += 4;					\
}

struct recv_int_stat {
    long enter;
    long usefull;
    long useless;
};
#define INTERRUPT_RECEIVE_STAT0(SIG)	\
{						\
    extern struct recv_int_stat recv_int_stat[NSIG];	\
    recv_int_stat[SIG].enter++;			\
}
#define INTERRUPT_RECEIVE_STAT1(SIG,RECV)	\
{						\
    extern struct recv_int_stat recv_int_stat[NSIG];	\
    if (RECV) recv_int_stat[SIG].usefull++;	\
    else      recv_int_stat[SIG].useless++;	\
}
#define MACDEP_DIST_STATISTICS()		\
{						\
    extern char * print_interrupt_statistics();	\
    ptr = print_interrupt_statistics(ptr);	\
}

/* shared memory */

#ifdef SPARC
Extern void s_lock();
Extern void s_unlock();
Extern void store_barrier();
#elif defined(ATT3600)
Extern void s_lock();
Extern void s_unlock();
#define store_barrier() {;}
#elif defined(IRIX)
Extern void s_lock();
Extern void s_unlock();
#define store_barrier() {;}
#elif defined(SPPUX)
Extern void s_lock();
Extern void s_unlock();
#define store_barrier() {;}
#elif defined(INTEL)
Extern void s_lock();
Extern void s_unlock();
#define store_barrier() {;}
#endif


typedef struct {
  Volatile int lockwd;
  Volatile int syncwd;
  struct global_variables globals;
  struct klic_sgnl_flags klic_sgnl_flags;
  Volatile long buffer_size;
  Volatile long rd_index;
  Volatile long wt_index;
  char* top_ptr;
  char* bottom_ptr;
  char buffer[1];
} SHARED_BLOCK;

typedef struct {
  Volatile int lockwd;
  Volatile int syncwd;
} IO_LOCKWORD;

Extern SHARED_BLOCK **shmp;
Volatile Extern IO_LOCKWORD *io_lockwds;
Extern long shared_buffer_size Init(0x10000);
Extern int my_io_lock_flag;

#define Lock(pe)   (s_lock  (&shmp[pe]->lockwd))
#define Unlock(pe) (s_unlock(&shmp[pe]->lockwd))

#define LEFT_SHM_BUFF(pe) (shmp[pe]->buffer_size - shmp[pe]->wt_index)
#define SHM_MSG_SIZE(pe) (shmp[pe]->wt_index - shmp[pe]->rd_index)
#define SHM_WPTR(pe) (shmp[pe]->top_ptr + shmp[pe]->wt_index)
#define SHM_RPTR(pe) (shmp[pe]->top_ptr + shmp[pe]->rd_index)
#define SHM_WIDX(pe) (shmp[pe]->wt_index)
#define SHM_RIDX(pe) (shmp[pe]->rd_index)
#define ADD_SHM_WIDX(pe,n) (shmp[pe]->wt_index = shmp[pe]->wt_index + n)
#define CLEAR_SHM_RWIDX(pe) (shmp[pe]->wt_index = shmp[pe]->rd_index = 0)

#endif /* _KLIC_DISTPKT_H_ */
