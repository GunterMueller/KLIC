/* ---------------------------------------------------------- 
%   (C)1995 Institute for New Generation Computer Technology 
%       (Read COPYRIGHT for detailed information.) 
%   (C)1996, 1997, 1998, 1999 Japan Information Processing Development Center
%       (Read COPYRIGHT-JIPDEC for detailed information.)
----------------------------------------------------------- */

#include <klic/basic.h>
#include <klic/struct.h>
#include <klic/primitives.h>
#include <klic/timing.h>

#include <stdio.h>
#include <signal.h>

#include <klic/interpe.h>
#include <klic/rmon.h>

/* 
  SEND_PROFILE (from WORKERs to SHOEN)
  */

void output_profile_information();

static q*
decode_send_profile(inbuf, allocp)
combuf *inbuf;
q *allocp;
{
    declare_globals;
    long node;
    node = GET_BUFFER(inbuf);

    shoen_profile[node].stimes = GET_BUFFER(inbuf);
    shoen_profile[node].itimes = GET_BUFFER(inbuf);
    shoen_profile[node].gctimes = GET_BUFFER(inbuf);
    shoen_profile[node].mstimes = GET_BUFFER(inbuf);
    shoen_profile[node].mrtimes = GET_BUFFER(inbuf);
    gathered_count++;

    if(gathered_count == total_node){
	output_profile_information(); /* This routine is machine dependent */
	gathered_count = 0;
	gather_prof_ready = 1;
    }

    return(allocp);
}

static void
encode_send_profile(buffer)
combuf *buffer;
{
    declare_globals;
/*    iosprintf("Node %d: encode_send_profile it_c = %d, id_c = %d gc_c = %d, ms_c = %d, mr_c = %d\n",
		 my_node,intr_count,perfmon_counter.idle_count,perfmon_counter.gc_count,perfmon_counter.ms_count,perfmon_counter.mr_count);*/

    PUT_BUFFER(buffer, decode_send_profile);
    PUT_BUFFER(buffer, my_node);
    PUT_BUFFER(buffer, perfmon_counter.intr_count);
    PUT_BUFFER(buffer, perfmon_counter.idle_count);
    PUT_BUFFER(buffer, perfmon_counter.gc_count);
    PUT_BUFFER(buffer, perfmon_counter.ms_count);
    PUT_BUFFER(buffer, perfmon_counter.mr_count);

    perfmon_counter.intr_count = 0;
    perfmon_counter.idle_count = 0;
    perfmon_counter.gc_count = 0;
    perfmon_counter.ms_count = 0;
    perfmon_counter.mr_count = 0;
}

void
send_profile()
{
    declare_globals;
    combuf *buffer = NODE_TO_BUFFER(SHOEN_NODE);
    
    INT_CL_DEBUG_X(ioprintf("%d:send_message\n", my_node));
    
    encode_send_profile(buffer);
    
    /* Cannot use send_message routine, 
       because it may send request_wtc message after sending the message. */
    
    send_message_without_wtc_chk(SHOEN_NODE, buffer);

}


/* 
  SEND_PROFILE_TRIGGER (from WORKERs to SHOEN)
  */

static q*
decode_profile_trigger(inbuf, allocp)
combuf *inbuf;
q *allocp;
{
    send_profile();
    return(allocp);
}

static
void encode_profile_trigger(buffer)
combuf *buffer;
{
    PUT_BUFFER(buffer, decode_profile_trigger);
}

void
send_profile_trigger_message(node)
     long node;
{
    combuf *buffer = NODE_TO_BUFFER(node);
    encode_profile_trigger(buffer);
    send_message_without_wtc_chk(node, buffer);
}

int
send_profile_trigger(allocp, sig)
q *allocp;
int sig;
{
    declare_globals;
    long i;

    if (gather_prof_ready) {
	for (i = 0; i < total_node ; i++){
	    send_profile_trigger_message(i);
	}
	gather_prof_ready = 0;
    }

    heapp = allocp;
    return 0;
}

