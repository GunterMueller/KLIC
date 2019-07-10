/* ---------------------------------------------------------- 
%   (C)1994, 1995 Institute for New Generation Computer Technology 
%       (Read COPYRIGHT for detailed information.) 
%   (C)1996, 1997, 1998, 1999 Japan Information Processing Development Center
%       (Read COPYRIGHT-JIPDEC for detailed information.)
----------------------------------------------------------- */
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

static void send_request_wtc();
void add_node_wtc();
long get_wtc();
combuf *backup_combuf();
static int requesting_wtc = 0;

/*
  Basic routine for sendig/suspending/receiving message
  SEND MESSAGE WITHOUT WTC CHK	void send_message_without_wtc(node, sendbuf)
  SEND MESSAGE			void send_message(node, sendbuf)
  MESSAGE SUSPEND		void message_suspend(dummy_wtc, node, outbuf)
  RECEIVE MESSAGE		int receive_message (allocp, sig)
  
  Wtc feed/suply message handling.
  supply wtc			static void send_supply_wtc(node)
  request wtc			static void send_request_wtc()
  RETURN WTC			void send_return_wtc(wtc)
  
  Management of the node's wtc
  GET WTC			long get_wtc(request, request_type)
  ADD NODE WTC			void add_node_wtc(wtc)
  
  Profiler support routines
  static q* decode_send_profile(inbuf, allocp)
  static void encode_send_profile(buffer)
  int send_profile(allocp, sig)
  int send_profile_signal(allocp, sig)

  */

/*
  SEND MESSAGE WITHOUT WTC CHK
  SEND MESSAGE
  */

void send_packet();

void
send_message_without_wtc_chk(node, sendbuf)
long node;
combuf *sendbuf;
{
    SEND_ON();
    send_packet(node, sendbuf);
    SEND_OFF();
}

void
send_message(node, sendbuf)
long node;
combuf *sendbuf;
{
    SEND_ON();
    send_packet(node, sendbuf);
    SEND_OFF();

#ifdef REQUEST_WTC_THRESHOLD
    /* The following code is optional for enhancing the performance? */
    if (!IS_SHOEN_NODE(my_node)) {
	if (node_wtc < REQUEST_WTC_THRESHOLD && !requesting_wtc) {
	    requesting_wtc = 1;
	    if (node_wtc < MIN_WTC) {
		ioprintf("Pre check, Invalid WTC in node%d\n",my_node);
		fatal("invalid WTC");
	    }
	    send_request_wtc();
	}
    }
#endif
}

/*
  MESSAGE SUSPEND
  */

void
message_suspend(wtc_needed, node, outbuf)
long wtc_needed;
long node;
combuf *outbuf;
{
    declare_globals;
    long *msg_body;
    struct susp_msg_rec *susp_msg;
    
    INT_CL_DEBUG_X(ioprintf("%d:message_suspend()\n", my_node));
    INT_CL_PROBE_X(netstat.msg_suspend_cnt++);
    
    susp_msg = (struct susp_msg_rec *)malloc(sizeof(struct susp_msg_rec));
    if(!susp_msg) {
	fatal("malloc failed in initiating message suspend record");
    }
    
    susp_msg->node = node;
    susp_msg->wtc_needed = wtc_needed;
    susp_msg->outbuf = backup_combuf(outbuf);
    susp_msg->next = susp_msg_list;
    susp_msg_list = susp_msg;
    
    if (IS_SHOEN_NODE(my_node))
      fatal("shoen should not suspend message\n");
    else {
	if (!requesting_wtc) {
	    requesting_wtc = 1;
	    send_request_wtc();
	}
    }
}
/*
  RECEIVE MESSAGE
  */
combuf *receive_packet();

void check_decode_stack();

int
receive_message (allocp, sig)
q *allocp;
int sig;
{
    declare_globals;
/*    static combuf *pending_msg = 0;*/
    
    if (allocp + this_more_space >= real_heaplimit) {
	heapp = allocp;
	return 1;
    }
    
#ifdef INTERRUPT_RECEIVE_STAT0
    INT_CL_PROBE_X(INTERRUPT_RECEIVE_STAT0(sig));
#endif
    for (;;) {
	combuf *inbuf;
	if (pending_msg) {
	    inbuf = pending_msg;
	    pending_msg = 0;
	    INT_CL_DEBUG_X(ioprintf("%d:pending message processed\n", my_node));
	} else {
	    long heap_needed;

	    RECV_ON();
	    inbuf = receive_packet(&heap_needed);
	    RECV_OFF();

#ifdef INTERRUPT_RECEIVE_STAT1
	    INT_CL_PROBE_X(INTERRUPT_RECEIVE_STAT1(sig, inbuf));
#endif
	    if (!inbuf) {
		/* no message arrived */

		check_decode_stack();

		heapp = allocp;
		return 0;
	    }
/*	    if (allocp + heap_needed + this_more_space >= real_heaplimit) {
		INT_CL_DEBUG_X(ioprintf("%d:message pending\n", my_node));
		pending_msg = inbuf;
		this_more_space += heap_needed;

		check_decode_stack();

		heapp = allocp;
		return 1;
	    }*/
	}
	while (!IS_BUFFER_EMPTY(inbuf)) {
	    module decoder;
	    decoder = (module)GET_BUFFER(inbuf);
	    allocp = (q *)decoder(inbuf, allocp);
	    if(allocp + this_more_space >= real_heaplimit){
/*		fprintf(stderr, "Node %d, decode pending \n", my_node);*/
		pending_msg = inbuf;

		heapp = allocp;
		return 1;
	    }

	}
	if (allocp + this_more_space >= real_heaplimit) {
	    /* Heap over in msg_handler
	       this should not happen */
	    /*heapp = allocp; return 1;*/
	    fprintf(stderr, "Node %d, heap over in msg_handler allocp = %x tms = %x rhl = %x \n", my_node, allocp, this_more_space, real_heaplimit);	    

	    heapp = allocp;
	    return 1;

/*	    fatal("Heap over in msg_handler: 0x%lx+0x%lx >= 0x%lx\n",
		  (long)allocp, (long)this_more_space, (long)real_heaplimit);*/
	}
    }
}

/*
  SUPPLY WTC
  */
static
int message_resume()
{
    declare_globals;
    struct susp_msg_rec *top_susp_msg;
    
    INT_CL_DEBUG_X(ioprintf("%d:message_resume()\n", my_node));
    INT_CL_PROBE_X(netstat.msg_resume_cnt++);
    
    while (susp_msg_list) {
	top_susp_msg = susp_msg_list;
	
	if (!get_wtc(top_susp_msg->wtc_needed, get_wtc_full_request)) {
	    INT_CL_DEBUG_X(ioprintf("%d:message_suspend again\n", my_node));
	    return 0;		/* node_wtc lower again */
	}
	send_message(top_susp_msg->node, top_susp_msg->outbuf);
	
	susp_msg_list = top_susp_msg->next;
	free_combuf(top_susp_msg->outbuf);
	free(top_susp_msg);
    }
    return 1;			/* successfully send all suspended messages */
}

static q*
decode_supply_wtc(inbuf, allocp)
combuf *inbuf;
q *allocp;
{
    declare_globals;
    
    INT_CL_DEBUG_X(ioprintf("%d:decode_supply_wtc\n", my_node));
    INT_CL_PROBE_X(netstat.recv_supply_wtc_cnt++);
    
    if(!requesting_wtc){
	fatal("Not requesting wtc now");
    }
    
    /*  node_wtc = node_wtc + SUPPLY_WTC_UNIT;*/
    add_node_wtc(SUPPLY_WTC_UNIT);
    
    requesting_wtc = 0;
    if (!message_resume()) {
	if (requesting_wtc)
	  ioprintf("%d: requesting_wtc is already 1\n", my_node);
	requesting_wtc = 1;
	send_request_wtc();
    }
    return(allocp);
}

static void
encode_supply_wtc(buffer)
combuf *buffer;
{
    declare_globals;
    INT_CL_PROBE_X(netstat.send_supply_wtc_cnt++);
    
    if (!IS_SHOEN_NODE(my_node)) {
	fatal("FP node try to send supply wtc");
    }
    shoen_wtc -= SUPPLY_WTC_UNIT;
    PUT_BUFFER(buffer, decode_supply_wtc);
}

/* send_suply_wtc is only called from decode_request_wtc() in shoen node. */
static void
send_supply_wtc(node)
long node;
{
    declare_globals;
    combuf *buffer = NODE_TO_BUFFER(node);
    
    INT_CL_DEBUG_X(ioprintf("%d:send_supply_wtc\n", my_node));
    
    encode_supply_wtc(buffer);
    send_message_without_wtc_chk(node, buffer);
} 

/*
  REQUEST WTC
  */
static q*
decode_request_wtc(inbuf, allocp)
combuf *inbuf;
q *allocp;
{
    declare_globals;
    long node = GET_BUFFER(inbuf);
    
    INT_CL_DEBUG_X(ioprintf("%d:decode_request_wtc(%ld)\n", my_node, node));
    INT_CL_PROBE_X(netstat.recv_request_wtc_cnt++);
    shoen_wtc += 1;		/* wtc for request_wtc */
    send_supply_wtc(node);
    
    return(allocp);
}

static void
encode_request_wtc(buffer)
combuf *buffer;
{
    declare_globals;
    INT_CL_PROBE_X(netstat.send_request_wtc_cnt++);
    
    PUT2_BUFFER(buffer, decode_request_wtc, my_node);
    node_wtc -= 1;		/* wtc for request_wtc */
    
    if(node_wtc < 1){
	ioprintf("Request WTC, Invalid WTC in Node %d\n", my_node);
	fatal("Invalid WTC");
    }
}
static void
send_request_wtc()
{
    declare_globals;
    combuf *buffer = NODE_TO_BUFFER(SHOEN_NODE);
    
    INT_CL_DEBUG_X(ioprintf("%d:send_request_wtc\n", my_node));
    
    encode_request_wtc(buffer);
    send_message_without_wtc_chk(SHOEN_NODE, buffer);
}

/*
  RETURN WTC
  */
static q*
decode_return_wtc(inbuf, allocp)
combuf *inbuf;
q *allocp;
{
    declare_globals;
    long ret_wtc;
    
    INT_CL_PROBE_X(netstat.recv_return_wtc_cnt++);
    ret_wtc = GET_BUFFER(inbuf);
    INT_CL_DEBUG_X(ioprintf("%d:decode_return_wtc(%d)\n", my_node, ret_wtc));
    shoen_wtc += ret_wtc;
    
    return(allocp);
}
static void
encode_return_wtc(buffer, wtc)
combuf *buffer;
long wtc;
{
    declare_globals;
    INT_CL_PROBE_X(netstat.send_return_wtc_cnt++);
    
    PUT2_BUFFER(buffer, decode_return_wtc, wtc);
    node_wtc -= wtc;
}

void
send_return_wtc(wtc)
long wtc;
{
    declare_globals;
    combuf *buffer = NODE_TO_BUFFER(SHOEN_NODE);
    
    INT_CL_DEBUG_X(ioprintf("%d:send_return_wtc(%ld)\n", my_node, wtc));
    
    encode_return_wtc(buffer, wtc);
    
    /* Cannot use send_message routine, 
       because it may send request_wtc message after sending the message. */
    
    send_message_without_wtc_chk(SHOEN_NODE, buffer);
}

/*
  GET WTC
  This function doesn't send/receive any packet.
  */
long
get_wtc(request, request_type)
long request;
enum get_wtc_type request_type;
{
    declare_globals;
    long ret_request;
    if (request_type == get_wtc_force_request) {
	if (node_wtc < request) {
	    ioprintf("Invalid wtc force request,in Node %d, req_wtc=%d, node_wtc=%d\n",
		     my_node, requesting_wtc, node_wtc);
	    fatal("Invalid WTC");
	} else
	  ret_request = request;
    } else {			/* ! get_wtc_force_request */
	if (node_wtc < MIN_WTC) {
	    if (requesting_wtc && node_wtc + 1 >= MIN_WTC)
	      return 0;
	    ioprintf("Invalid WTC in Node %d, req_wtc=%d, node_wtc=%d\n",
		     my_node, requesting_wtc, node_wtc);
	    fatal("Invalid WTC");
	}
	
	if (node_wtc >= request * 2 || request_type == get_wtc_full_request)
	  ret_request = request;
	else {
	    ret_request = node_wtc >> 1;
	    if (ret_request < MIN_WTC)
	      return 0;		/* at least MIN_WTC wtc should return */
	    /* for throw goal to idle node */
	}
	if (node_wtc - ret_request < MIN_WTC)
	  return 0;		/*If node_wtc will become lower than MIN_WTC.*/
    }
    INT_CL_DEBUG_X(iosprintf("%d: wtc %d-%d\n",
			     my_node, node_wtc, ret_request));
    node_wtc -= ret_request;
    return ret_request;
}
/*
  ADD NODE WTC
  It may return surplus WTC to shoen.
  */

void
add_node_wtc(wtc)
long wtc;
{
    declare_globals;
    INT_CL_DEBUG_X(iosprintf("%d: wtc %d+%d\n", my_node, node_wtc, wtc));
    node_wtc += wtc;  
    if(node_wtc > NODE_WTC_LIMIT){
	node_wtc -= RETURN_WTC_UNIT;
	send_return_wtc(RETURN_WTC_UNIT);
    }
}

/*
  Initiate message decoder stack
*/

void
initiate_decode_stack()
{
    declare_globals;
    char *dstack_alloc;
    decode_stack_size = 0;

    dstack_alloc = (char *) malloc(INIT_DECODE_STACK_SIZE*sizeof(q));
    if(dstack_alloc == NULL){
	fatal("malloc failed in initiate_decode_stack");
    }
    decode_stack = (q *) dstack_alloc;
    decode_stack_ptr = (q *) dstack_alloc;
    decode_stack_size = INIT_DECODE_STACK_SIZE;
    decode_stack_limit = (q *) ( dstack_alloc + decode_stack_size );
/*    fprintf(stderr, "Node %d, ds = %x dsp =%x\n", 
	    my_node, decode_stack, decode_stack_ptr);*/
}

void push_decode_stack(data)
     q data;
{
    declare_globals;
    char *dstack_alloc;
    int dstack_current_offset;

    *(decode_stack_ptr)++ = data;
    if(decode_stack_ptr == decode_stack_limit){
        dstack_alloc = realloc((char *)decode_stack,
               (decode_stack_size+INIT_DECODE_STACK_SIZE)*sizeof(q));
        if(dstack_alloc == NULL){
            fatal("realloc failed in push_decode_stack");
        }
        dstack_current_offset = (int)(decode_stack_ptr - decode_stack);
        decode_stack_size += INIT_DECODE_STACK_SIZE;
        decode_stack = (q*)dstack_alloc;
        decode_stack_ptr =(q*)(decode_stack+dstack_current_offset);
        decode_stack_limit = (q*)(dstack_alloc+decode_stack_size);

/*      fprintf(stderr, "Node %d, ds = %x dsp =%x dsl =%x\n",
                my_node, decode_stack, decode_stack_ptr,decode_stack_limit);*/
    }
}

q pop_decode_stack()
{
    q data;
    decode_stack_ptr--;
    data = *(decode_stack_ptr);

    if(decode_stack_ptr < decode_stack){
	fatal("Error in pop_decode_stack");
    }

    return(data);
}

void 
check_decode_stack()
{
    if(decode_stack_ptr != decode_stack){
	fatal("Error in check_decode_stack");
    }
}

q general_gc();

q* gc_decode_stack(allocp, ntop, otop, nsize, osize)
     q *allocp, *ntop, *otop;
     unsigned long nsize, osize;
{
    declare_globals;
    q *decode_stack_tmp;
    q newdata;

/*    fprintf(stderr, "Node %d gc_decode_stack in\n", my_node);*/

    decode_stack_tmp = decode_stack;
    while(decode_stack_tmp != decode_stack_ptr){
	newdata = general_gc(decode_stack_tmp, allocp, gcsp);
	*decode_stack_tmp = newdata;
	allocp = heapp;
	decode_stack_tmp++;
	
/*	fprintf(stderr, "Node %d gc_decode_stack\n", my_node);*/

    }
    return(allocp);
}
