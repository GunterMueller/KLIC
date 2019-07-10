/* ---------------------------------------------------------- 
%   (C)1994,1995 Institute for New Generation Computer Technology 
%       (Read COPYRIGHT for detailed information.) 
%   (C)1996, 1997, 1998, 1999 Japan Information Processing Development Center
%       (Read COPYRIGHT-JIPDEC for detailed information.)
----------------------------------------------------------- */
/* PVM-TCP dependent inter-pe communication library */
/* $Id: distpkt.h,v 1.1.1.1 2000/07/28 02:01:42 igarashi Exp $ */
#include <pvm3.h>
#include <sys/param.h>
#include <netdb.h>

#ifndef MAXHOSTNAMELEN
#define MAXHOSTNAMELEN 256
#endif

/* Maximum ratio of the output KL1 words created by one communication word. */
#define DECODE_MAX_OBJ_RATIO	(((6+3+1+5) + 2)/3)


/* Network Buffer Type */
typedef struct {
    long *buffer;		/* pointer to the actual buffer */
    long rd_index;		/* pointer used for reading */
    long wt_index;		/* pointer used for writing */
    long buf_size;		/* the physical size of buffer */
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

struct port_table {
    char *node_name;
    int port;
    int fd;
};

struct port_table *port_tbl; 

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
