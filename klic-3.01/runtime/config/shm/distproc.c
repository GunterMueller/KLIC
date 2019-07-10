/* ---------------------------------------------------------- 
%   (C)1994,1995 Institute for New Generation Computer Technology 
%       (Read COPYRIGHT for detailed information.) 
%   (C)1996, 1997, 1998, 1999 Japan Information Processing Development Center
%       (Read COPYRIGHT-JIPDEC for detailed information.)
----------------------------------------------------------- */
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <fcntl.h>
#ifdef SPPUX
#include <sys/cnx_mman.h>
#endif
#include <sys/mman.h>
#include <klic/basic.h>
#include <klic/struct.h>
#include <klic/primitives.h>
#include <klic/sighndl.h>
#include <klic/distio.h>
#include <klic/interpe.h>
#include <klic/rmon.h>
#include <signal.h>
#include <unistd.h>
#ifdef USEPROCBIND
#include <sys/processor.h>
#include <sys/procset.h>
#endif
#ifdef STRINGH
#include <string.h>
#else
#include <strings.h>
#endif

#include <klic/options.h>
#include <klic/distpkt.h>
#include <klic/distproc.h>

#define SYNCID_SPAWN 0x3193
/*
  char *expand_prog_name(buffer, progname)
  static void setup_send_args(opttable)
  static void recv_extract_args(opttable)
  static int io_spawn_children(num_child, opttable, prog_name, argc, argv)
  static int child_spawn_children(par_tid, opttable, prog_name, argc, argv)
  int spawn_children(num_child, opttable, prog_name, argc, argv)
  */

#define OPT_DEBUG(X) 

void
kill_child()
{
  kill(0, SIGINT);
}

void
child_sync(syncid)
int syncid;
{
}
void
io_wait_sync(syncid)
int syncid;
{
}
void
child_wait_sync(syncid)
int syncid;
{
}

void
io_sync(syncid)
int syncid;
{
}

static void
sigint_handler()
{
    klic_fprintf(stdout, "kill tasks in children_spawn()\n");
    kill_child();
    exit(1);
}


int r_spawn_children(num_child, opttable, prog_name, argc, argv)
     long num_child; 
     Const struct opttable *opttable;
     char *prog_name;
     int argc;
     char **argv;
{
  int my_penum = num_child;
  long one_block = sizeof(SHARED_BLOCK)+shared_buffer_size-1;

  if (num_child <= 0)  return num_child;

  /*  allocate_shared_memory(); */
  {
    char tmpfile[100];
    int i, fds, pid = getpid();

    shmp = ( SHARED_BLOCK** ) calloc(num_child, sizeof(SHARED_BLOCK*));
    if (shmp == ( SHARED_BLOCK** ) NULL) {
      klic_fprintf(stderr, "can't memory allocate: spawn_children");
      exit(1);
    }
    tmpfile[0] = (char)'\0';
    sprintf(tmpfile, "/tmp/klic%d.shm", pid);

    fds = open(tmpfile, (O_CREAT|O_RDWR), 0644);
    if (fds < 0) {
      klic_fprintf(stderr, "can't open file /tmp/klic%d.shm\n", pid);
      exit(1);
    }
    lseek(fds, sizeof(IO_LOCKWORD)-1, SEEK_SET);
    write(fds, tmpfile, 1);
    io_lockwds = (IO_LOCKWORD*)mmap(0, sizeof(IO_LOCKWORD)
				    , (PROT_READ|PROT_WRITE)
				    , (MAP_FILE|MAP_VARIABLE|MAP_SHARED)
				    , fds, 0);
    if (io_lockwds == (IO_LOCKWORD*)MAP_FAILED) {
      klic_fprintf(stderr, "can't mmap(io_lockwds)\n");
      exit(1);
    }
    close(fds);
    unlink(tmpfile);

/*    IO_Unlock; */
/*    io_lockwds->lockwd = 0; */
    IO_Unlock;
    io_lockwds->syncwd = 1;

    for (i=0; i<num_child; i++) {
      fds = open(tmpfile, (O_CREAT|O_RDWR), 0644);
      if (fds < 0) {
	klic_fprintf(stderr, "can't open file /tmp/klic...\n");
	exit(1);
      }
      lseek(fds, one_block-1, SEEK_SET);
      write(fds, tmpfile, 1);
      shmp[i] = (SHARED_BLOCK*)mmap(0, one_block
				    , (PROT_READ|PROT_WRITE)
				    , (MAP_FILE|MAP_VARIABLE|MAP_SHARED)
				    , fds, 0);
      if (shmp[i] == (SHARED_BLOCK*)MAP_FAILED) {
	klic_fprintf(stderr, "can't mmap(block)\n");
	exit(1);
      }

/*      Unlock(i); */
/*      shmp[i]->lockwd = 0; */
      Unlock(i);
      shmp[i]->syncwd = 1;
      shmp[i]->buffer_size = shared_buffer_size;
      shmp[i]->rd_index = 0;
      shmp[i]->wt_index = 0;
      shmp[i]->top_ptr = shmp[i]->buffer;
      shmp[i]->bottom_ptr = shmp[i]->buffer + shared_buffer_size - 1;
      shmp[i]->buffer[shared_buffer_size-1] = (char)'z';
      close(fds);
      unlink(tmpfile);
    }
  }

  /*  fork_PEs();               */
  {
    int i, pid;
    char *p;

    if (network_statistics_flag) {
      fprintf(stderr, "shoen node\n");
      fprintf(stderr, "\tNode %ld: pid = %d\n", num_child-1, getpid());
      fprintf(stderr, "reduction node\n");
    }

    /*  set session ID - make a process group */
    (void) setsid();

    for (i=1; i<num_child; i++) {
      pid = fork();
      if (pid == 0) { /* child -  working node - */
	my_penum = i;
	p = shmp[my_penum-1]->buffer;
	if (shmp[my_penum-1]->buffer[shared_buffer_size-1] != 'z') {
	  klic_fprintf(stderr, "mmap fail\n");
	}
	break;
      } else if (pid > 0) { /* parent - shoen node - */
	if (network_statistics_flag) {
	  fprintf(stderr, "\tNode %d: pid = %d\n", i - 1, pid);
	}
      } else { /* failure */
        klic_fprintf(stderr, "fork fail");
        exit(1);
      }
    }
  }

  CLEAR_SHM_RWIDX(my_penum-1);
  /*  cp_globals();             */
  BCOPY(&globals, &shmp[my_penum-1]->globals, sizeof(struct global_variables));
  My_glbl = &shmp[my_penum-1]->globals;
  /*  cp_signals();             */
  BCOPY((const void *)klic_sgnl_flags, &shmp[my_penum-1]->klic_sgnl_flags, sizeof(struct klic_sgnl_flags));
  klic_sgnl_flags = &shmp[my_penum-1]->klic_sgnl_flags;

  /*  barrier           */
  store_barrier();
#ifdef SPPUX
  {
    int i;

    for (i=0; i<num_child; i++)
      shmp[i]->lockwd = shmp[i]->lockwd;
    io_lockwds->lockwd = io_lockwds->lockwd;
  }
#endif
  /* barrier_sync */
  if (my_penum == num_child) {
    int i;
    int sync;
    do {
      sync = 0;
      for (i=1; i<num_child; i++) {
	sync |= shmp[i-1]->syncwd;
      }
    } while (sync);
    io_lockwds->syncwd = 0;
  } else {
    shmp[my_penum-1]->syncwd = 0;
    while (io_lockwds->syncwd) ;
  }

  return my_penum;
}

void
count_profile()
{
    perfmon_counter.intr_count++;
    if (perfmon_state.idle_on) {
	perfmon_counter.idle_count++;
	return;
    }
    if (perfmon_state.ms_on) {
	perfmon_counter.ms_count++;
	return;
    }
    if (perfmon_state.mr_on) {
	perfmon_counter.mr_count++;
	return;
    }
    if (perfmon_state.gc_on) {
	perfmon_counter.gc_count++;
	return;
    }
}

#if 0
myexit(char *s, int *l, int *c)
{
    FILE *f;
    char buf[10];
    sprintf(buf,"/tmp/%d",getpid());
    f=fopen(buf,"w");
    klic_fprintf(f, "exit(pid=%d,my_node=%d) %s:%d\n",
		 getpid(), my_node,  s, l);
    fclose(f);
    exit(c);
}
#endif
