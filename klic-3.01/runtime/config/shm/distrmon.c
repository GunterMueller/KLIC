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
#include <stdlib.h>
#include <signal.h>

#include <klic/sighndl.h>
#include <klic/distio.h>
#include <klic/interpe.h>
#include <klic/rmon.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <sys/socket.h>

/*
  Shoen outputs profile information to rmon server using Unix domain sockets
  */

static FILE *rmonfile;

void
output_profile_information()
{
    declare_globals;
    int  i;
    int  bufindex = 0;
    int  req_size, n;
    char *rmon_buf = (char *) send_pbuf;

    send_pbuf[bufindex++] = SHOEN_OUTPUT_PROF_TAG;
    send_pbuf[bufindex++] = total_node;
    for(i = 0; i < total_node ; i++){
	send_pbuf[bufindex++] = shoen_profile[i].stimes;
      	send_pbuf[bufindex++] = shoen_profile[i].itimes;
      	send_pbuf[bufindex++] = shoen_profile[i].gctimes;
      	send_pbuf[bufindex++] = shoen_profile[i].mstimes;
      	send_pbuf[bufindex++] = shoen_profile[i].mrtimes;
    }

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
}

void
profiling_ctr_tag(tag)
     long tag;
{
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
}

void
start_profiling()
{
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
    fatal("Cannot connect with 'pg-server'!!");
  }

  rmonfile  = fdopen(sock,"w+");

  profiling_ctr_tag(SHOEN_START_TAG);
}

void
terminate_profiling()
{
    declare_globals;
    profiling_ctr_tag(SHOEN_TERMINATE_TAG);
    fclose(rmonfile);
}

void
init_rmonitor()
{
  declare_globals;
  if(IS_SHOEN_NODE(my_node)){
      if(rmonnode){
	  
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
