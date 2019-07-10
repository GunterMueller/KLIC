/* ---------------------------------------------------------- 
%   (C)1994,1995 Institute for New Generation Computer Technology 
%       (Read COPYRIGHT for detailed information.) 
%   (C)1996, 1997, 1998, 1999 Japan Information Processing Development Center
%       (Read COPYRIGHT-JIPDEC for detailed information.)
----------------------------------------------------------- */
/* PVM dependent inter-pe communication library */
/* $Id: distpkt.c,v 1.1.1.1 2000/07/28 02:01:42 igarashi Exp $ */
#include <klic/basic.h>
#include <klic/struct.h>
#include <klic/primitives.h>
#include <klic/unify.h>
#include <klic/timing.h>

#include <stdio.h>
#include <signal.h>

#include <klic/distio.h>
#include <klic/interpe.h>
#include <klic/rmon.h>

/*
  Communication buffer management
  ALLOCATE COMBUF		combuf *alloc_combuf(bufsize)
  FREE COMBUF			void *free_combuf(buf)
  BACKUP COMBUF			combuf *backup_combuf(buf)
  realloc combuf		static combuf *realloc_combuf(inbuf, bufsize)
  EXPAND OUTBUF			void expand_outbuf(outbuf)

  Communication initialize
  INIT COMMUNICATION		void init_comm()
  IO INIT COMMUNICATION		void io_init_comm()
  IO INIT AFTER INTERRUPT SETUP	void init_dist_signal_setup() 

  Raw packet send/receive
  RECEIVE PACKET		combuf *receive_packet(heap_neededp)
  SEND_PACKET			void send_packet(node, sendbuf)
*/

#define INITIAL_COMBUF_SIZE	0x10000	/* for inbuf and outbuf */

/* Communication buffer management functions */
combuf *
alloc_combuf(bufsize)
long bufsize;
{
    combuf *buf = (combuf *)malloc(sizeof(combuf));
    if (!buf)
      fatal("malloc failed in alloc_combuf (1)");
    buf->buf_size = bufsize;
    buf->buffer = (long *)malloc(bufsize * sizeof(long));
    buf->rd_index = 0;		/* pointer for read */
    buf->wt_index = 0;		/* pointer for write */
    if (buf->buffer == NULL)
      fatal("malloc failed in alloc_combuf (2)");
    return buf;
}

static combuf *
realloc_combuf(inbuf, bufsize)
combuf *inbuf;
long bufsize;
{
    inbuf->buffer = (long *)realloc((char *)inbuf->buffer,
				    bufsize * sizeof(long));
    if (inbuf->buffer == NULL)
      return 0;
    inbuf->buf_size = bufsize;
    return inbuf;
}

void *
free_combuf(buf)
combuf *buf;
{
    free(buf->buffer);
    free(buf);
}
/* backup_combuf() is called only within message_suspend() */
combuf *
backup_combuf(buf)
combuf *buf;
{
    combuf *retbuf;

    /* backup the contents of BUF into newly created combuf  */
    retbuf = alloc_combuf(buf->wt_index);
    BCOPY((char *)buf->buffer, (char *)retbuf->buffer,
	  sizeof(long)* buf->wt_index);
    retbuf->wt_index = buf->wt_index;
    retbuf->rd_index = buf->rd_index;
    buf->wt_index = 0;		/* Clear the write buffer */
    return retbuf;
}

/* expand_outbuf() is called only within the macro PUT_BUFFER() */
void
expand_outbuf(outbuf)
combuf *outbuf;
{
    /* always double the size */
    /* expand_outbuf() must expand the buffer at least 4-words.
       Otherwise, PUT4_BUFFER() in distpkt.h will crash.        */ 
    declare_globals;       
    long bufsize = outbuf->buf_size * 2;
    outbuf = realloc_combuf(outbuf, bufsize);
    if (!outbuf)
      fatal("realloc failed in expanding outbuf");
    ioprintf("PE %d outbuf expanded to %d\n", my_node, bufsize);
}

void
init_comm()
{
    /* initialize out buffer */
    kumon_outbuf = alloc_combuf(INITIAL_COMBUF_SIZE);
    /* initialize in buffer */
    kumon_inbuf = alloc_combuf(INITIAL_COMBUF_SIZE);
    /* allow faster PVM message passing */
    pvm_setopt(PvmRoute, PvmAllowDirect);
}
void
io_init_comm()
{}

void
init_dist_signal_setup()
{
    declare_globals;
    int add_signal_handler();
    void call_at_time_intervals();
    int receive_message();
    int send_profile();
    int send_profile_trigger();
    void count_profile();

    q *set_simple_interval_timer_handler();

    add_signal_handler(SIGUSR1, receive_message);
#ifdef USETIMER
    if(rmonnode){
	if (IS_SHOEN_NODE(my_node)){
	    struct itimerval interval;
	    long isec, usec;
	    heapp = set_simple_interval_timer_handler(heapp, 
						      PROF_GATHER_INTERVAL_SEC,
						      0, send_profile_trigger);
	}else {	
	    call_at_time_intervals(0, PROF_SAMPLE_INTERVAL_uSEC, 
				   count_profile);
/*	    add_signal_handler(SIGUSR2, send_profile);*/
	}
    }

    if (!noitimer) {
	struct itimerval interval;
	long isec, usec;
	if (interval_usec == 0)
	  interval_usec = 10000;
	isec = interval_usec / 1000000;
	usec = interval_usec % 1000000;
	heapp = set_simple_interval_timer_handler(heapp, isec, usec,
						  receive_message);
    }
#endif
}

/* Raw packet send/receive functions */
combuf *
receive_packet(heap_neededp)
long *heap_neededp;	/* this variable will be obsoleted in the future. */
{
    declare_globals;
    long last_index;
    int buf_id;
    int bufsize;
    int info;
    combuf *inbuf = kumon_inbuf;
    buf_id = pvm_nrecv(-1, -1);
    if (!buf_id)
      return 0;		/* no message arrived */
    {
	int msgtag, tid;
	info = pvm_bufinfo(buf_id, &bufsize, &msgtag, &tid);
	if(info < 0)
	  fatal("Error in pvm_bufinfo\n");
    }
    last_index = bufsize / sizeof(long);
    if (last_index > inbuf->buf_size)
      if (realloc_combuf(inbuf, last_index)) {
	  ioprintf("PE %d inbuf expanded to %d\n", my_node, bufsize);
      } else {
	  fatal("realloc failed in expanding inbuf");
      }
    info = pvm_upklong(inbuf->buffer, last_index, 1);
    if (info < 0)
      fatal("Error in pvm_upk\n");
    inbuf->wt_index = last_index;
    inbuf->rd_index = 0;  /* clear rd_index */
    INT_CL_PROBE_X(netstat.recv_data_siz += bufsize);
    *heap_neededp = last_index * DECODE_MAX_OBJ_RATIO;
    return inbuf;
}

void
send_packet(node, sendbuf)
long node;
combuf *sendbuf;
{
    int info;

    info = pvm_initsend(PvmDataRaw);
    if (info < 0)
      fatal("Error in pvm_initsend\n");
    info = pvm_pklong(sendbuf->buffer, sendbuf->wt_index, 1);
    if (info < 0)
      fatal("Error in pvm_pklong\n");
    info = pvm_send(Tid_of(node),3);
    if (info < 0)
      fatal("Error in pvm_send\n");
    INT_CL_PROBE_X(netstat.send_data_siz += sendbuf->wt_index * sizeof(long));

    sendbuf->wt_index = 0;
    if (!no_sendsig) {
	info = pvm_sendsig(Tid_of(node), SIGUSR1);
	if (info != 0) {
	    ioprintf("PVM_SENDSIG_ERROR sig_info = %d\n",info);
	}
    }
}
void
close_network()
{
    pvm_exit();
}
struct recv_int_stat recv_int_stat[NSIG];
char *
print_interrupt_statistics(ptr)
char *ptr;
{
    sprintf(ptr, " sigalrm=(%d+%d)/%d, sigio=(%d+%d)/%d, sigusr1=(%d+%d)/%d\n",
	    recv_int_stat[SIGALRM].usefull,
	    recv_int_stat[SIGALRM].useless,
	    recv_int_stat[SIGALRM].enter,
	    recv_int_stat[SIGIO].usefull,
	    recv_int_stat[SIGIO].useless,
	    recv_int_stat[SIGIO].enter,
	    recv_int_stat[SIGUSR1].usefull,
	    recv_int_stat[SIGUSR1].useless,
	    recv_int_stat[SIGUSR1].enter);
    return strlen(ptr)+ptr;
}
