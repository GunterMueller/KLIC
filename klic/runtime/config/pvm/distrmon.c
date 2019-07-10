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

#include <klic/distio.h>
#include <klic/interpe.h>
#include <klic/rmon.h>

#if defined(VPP) || defined(AP3000)
#include <sys/socket.h>
#endif

/*
  Shoen outputs profile information to rmon server using PVM
  */

#if defined(VPP) || defined(AP3000)
static FILE *rmonfile;
#endif

void
output_profile_information()
{
    declare_globals;
    int  i;
    int  bufindex = 0;
#if defined(VPP) || defined(AP3000)
    int req_size, n;
    char *rmon_buf = (char *) send_pbuf;
#else
    int  info;
#endif

    send_pbuf[bufindex++] = SHOEN_OUTPUT_PROF_TAG;
    send_pbuf[bufindex++] = total_node;
    for(i = 0; i < total_node ; i++){
	send_pbuf[bufindex++] = shoen_profile[i].stimes;
      	send_pbuf[bufindex++] = shoen_profile[i].itimes;
      	send_pbuf[bufindex++] = shoen_profile[i].gctimes;
      	send_pbuf[bufindex++] = shoen_profile[i].mstimes;
      	send_pbuf[bufindex++] = shoen_profile[i].mrtimes;
    }
#if defined(VPP) || defined(AP3000)
    req_size = bufindex * sizeof(long);

    while ((n = fwrite((char *)rmon_buf,1,req_size,rmonfile)) < req_size) {
      if (n <= 0 ) {
	fclose(rmonfile);
	fatal("Cannot send to 'pg-server'!!");
      }
      req_size -= n;
      rmon_buf += n;
    }
    fflush(rmonfile);
#else
    info = pvm_initsend(PvmDataDefault);
    if (info < 0)
      fatal("Error in pvm_initsend\n");
    info = pvm_pklong(send_pbuf, bufindex, 1);
    if (info < 0)
      fatal("Error in pvm_pklong\n");
    info = pvm_send(rmon_tid,3);
    if (info < 0)
      fatal("Error in pvm_send\n");
#endif
}

void
profiling_ctr_tag(tag)
     long tag;
{
#if defined(VPP) || defined(AP3000)
    declare_globals;
    int req_size = sizeof(long), n;
    union { long v; char b[sizeof(long)]; } tagb;
    char *buf = (char *)&tagb;

    tagb.v = tag;

    while ((n = fwrite(buf,1,req_size,rmonfile)) < req_size) {
      if (n <= 0) {
	fclose(rmonfile);
	fatal("Cannot send to 'pg-server'!!");
      }
      req_size -= n;  buf += n;
    }
    fflush(rmonfile);
#else
    int info;
    info = pvm_initsend(PvmDataDefault);
    if (info < 0)
      fatal("Error in pvm_initsend\n");
    info = pvm_pklong(&tag, 1, 1);
    if (info < 0)
      fatal("Error in pvm_pklong\n");
    info = pvm_send(rmon_tid,3);
    if (info < 0)
      fatal("Error in pvm_send\n");
#endif
}

void
start_profiling()
{
#if defined(VPP) || defined(AP3000)
  declare_globals;
  int family = PF_UNIX, n, max, req_size, i, j, sock;
  struct sockaddr addr;
  char *path;

  max = total_node;
  path = rmonnode;

  addr.sa_family = family;
  strcpy(addr.sa_data,"/tmp/");
  strcat(addr.sa_data,path);

  sock = socket(family,SOCK_STREAM,0);
  if (sock < 0) {
    fatal("Cannot make a socket!!");
  }
  if (connect(sock, &addr, sizeof(struct sockaddr)) < 0) {
    fatal("Cannot be connected with 'pg-server'!!");
  }

  rmonfile  = fdopen(sock,"w+");
#endif
    profiling_ctr_tag(SHOEN_START_TAG);
}

void
terminate_profiling()
{
#if defined(VPP) || defined(AP3000)
    declare_globals;
#endif
    profiling_ctr_tag(SHOEN_TERMINATE_TAG);
#if defined(VPP) || defined(AP3000)
    fclose(rmonfile);
#endif
}


/*
  Initiate profile data structure in SHOEN NODE
*/

void
init_rmonitor()
{
  declare_globals;
  if(IS_SHOEN_NODE(my_node)){
      if(rmonnode){
#ifndef VPP
#ifndef AP3000
	  sscanf(rmonnode,"%x",&rmon_tid);
#endif
#endif
	  
	  start_profiling();

	  shoen_profile = 
	    (struct profile_rec *)malloc(total_node*sizeof(struct profile_rec));	  
	  if(shoen_profile == NULL)
	    fatal("malloc failed in init_rmonitor");
	  
	  send_pbuf = 
	    (long *)malloc(total_node*sizeof(struct profile_rec)+2);
	  if(send_pbuf == NULL)
	    fatal("malloc failed in init_rmonitor");
      }
  }
}
