/* ---------------------------------------------------------- 
%   (C)1993,1994,1995 Institute for New Generation Computer Technology 
%       (Read COPYRIGHT for detailed information.) 
%   (C)1996, 1997, 1998, 1999 Japan Information Processing Development Center
%       (Read COPYRIGHT-JIPDEC for detailed information.)
----------------------------------------------------------- */
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <errno.h>
#include <klic/config.h>
#include "klic.h"
#include "version.h"
#ifdef USELOCKF
#include <fcntl.h>
#endif

#define KL1_TO_C_COMPILER   KLIC_COMPILER

extern void *malloc();

enum filetype { kl1_file, prog_file, asm_file, obj_file, other_file };

struct lang_info {
  char *ext;
  char *comp_env;
  char *comp_default;
  char *comp;
  char *comp_opt_env;
  char *comp_opt_default;
  char *comp_opt;
  char *ld_env;
  char *ld_default;
  char *ld;
  char *ld_opt_env;
  char *ld_opt_default;
  char *ld_opt;
  int prio;
} lang_infos[] = {
  /* CAUTION ! The 1st definition must be for C.*/
  ".c",
  "KLIC_CC",
  CC,
  0,
  "KLIC_CC_OPTIONS",
  KLIC_CC_OPTIONS,
  0,
  "KLIC_LD",
  LD,
  0,
  "KLIC_LD_OPTIONS",
  KLIC_LD_OPTIONS,
  0,
  0,
#ifdef KLIC_FORT
  /* Fortran */
  ".f",
  "KLIC_FORT",
  KLIC_FORT,
  0,
  "KLIC_FORT_OPTIONS",
  KLIC_FORT_OPTIONS,
  0,
  "KLIC_FORT_LD",
  KLIC_FORT_LD,
  0,
  "KLIC_FORT_LD_OPTIONS",
  KLIC_FORT_LD_OPTIONS,
  0,
  1,
#endif
  /* Do not remove the following one line. It's a `end mark'' */
  0,
};

struct lang_info * const c_info = lang_infos;
struct lang_info * pref_info = lang_infos;

struct namerec {
  struct namerec *next;
  enum filetype type;
  char *name;
  struct lang_info *info;
};

static void
initialize_lang_info(void)
{
  extern char *option_value();
  struct lang_info *info = lang_infos;
  for(; info->ext; ++info) {
    info->comp = option_value(info->comp_env, info->comp_default);
    info->comp_opt = option_value(info->comp_opt_env, info->comp_opt_default);
    info->ld = option_value(info->ld_env, info->ld_default);
    info->ld_opt = option_value(info->ld_opt_env, info->ld_opt_default);
  }
}

static struct lang_info*
search_lang_info(suffix)
     char *suffix;
{
  int i;
  struct lang_info *info = lang_infos;
  for(; info->ext; ++info) {
    if(strcmp(info->ext, suffix) == 0) {
      return info;
    }
  }
  return 0;
}

static void reverse_names(names)
     struct namerec **names;
{
  struct namerec *one, *next;
  next = *names;
  one = 0;
  while (next != 0) {
    struct namerec *last = one;
    one = next;
    next = one->next;
    one->next = last;
  }
  *names = one;
}

static void error_exit(format, arg)
     char *format, *arg;
{
  (void)fprintf(stderr, format, arg);
  putc('\n', stderr);
  exit(-1);
}

static void sync_and_exit()
{
  int status;
  while (wait(&status) >= 0 || errno != ECHILD)
    ;
  exit(1);
}

static void usage_error(cmd)
     char *cmd;
{
  error_exit("Usage: %s -options files ...\nSpecify -h option for help", cmd);
}

static void usage_help(cmd)
     char *cmd;
{
  fprintf(stderr, "Usage: %s -options files ...\n", cmd);
  fprintf(stderr, "Options available are:\n");
  fprintf(stderr, "  -c: no link; only compile to .o\n");
  fprintf(stderr, "  -C: no C compilation; only compile to .c\n");
  fprintf(stderr, "  -d: dry run; no actual compilation; implies -v\n");
  fprintf(stderr, "  -D dbmaker: specifies KLIC database manager program\n");
  fprintf(stderr, "  -Fld linker: force to set linker program to linker \n");
  fprintf(stderr, "  -Flo opt: force to set linker option to opt\n");
  fprintf(stderr, "  -g: add debug option for C compiler\n");
  fprintf(stderr, "  -K compiler: specifies KL1 to C compiler program\n");
  fprintf(stderr, "  -I incdir: add .h include directory for cc\n");
  fprintf(stderr, "  -L libdir: add library directory for linkage\n");
  fprintf(stderr, "  -l library: add library archive for linkage\n");
  fprintf(stderr, "  -o file: specifies executable file name\n");
  fprintf(stderr, "  -O[n]: specifies cc optimization\n");
  fprintf(stderr, "  -P n: parallel compilation (up to n processes)\n");
  fprintf(stderr, "  -p: pass -p option to cc\n");
  fprintf(stderr, "  -pg: pass -pg option to cc\n");
  fprintf(stderr, "  -pl: pass -p option to cc only on linkage\n");
  fprintf(stderr, "  -R: recompile everything ignoring file dates\n");
  fprintf(stderr, "  -S: generate assembly language code\n");
  fprintf(stderr, "  -v: verbose compilation\n");
  fprintf(stderr, "  -n: link with non-debugging runtime system\n");
  fprintf(stderr, "  -x dbdir: specifies KLIC database directory\n");
  fprintf(stderr, "  -X initdbdir: specifies database initiation directory\n");
}

static void access_error(file)
     char *file;
{
  error_exit("Can't access file %s", file);
}

static void get_stat(file, st)
     char *file;
     struct stat *st;
{
  if (stat(file, st) != 0) {
    access_error(file);
  }
}

/* Options */

char distklic = 0;  /***** added for klicdist *****/
char shmklic = 0;  /***** added for klicshm *****/

char *ofile = 0;
int optlevel = 0;
char *optflags;
char debug = 1;
char cdebug = 0;
char *cc_profile_flag = "";
char *link_profile_flag = "";
char verbose = 0;
char no_link = 0;
char no_cc = 0;
char make_asm = 0;
char dryrun = 0;
char do_recompile = 0;
char *dbdir = 0;
int parallelism = 0;
int forked = 0;
char *forced_linker = 0;
char *forced_linker_opt = 0;

char *initdbdir;
char *klic_compiler;
char *klic_dbmaker;
char *klic_libdir;
char *klic_incdir;
char *klic_cc;
char *klic_cc_options;

struct namerec *files = 0;
struct namerec *incdirs = 0;
struct namerec *libraries = 0;
struct namerec *libdirs = 0;

char buf[BUFSIZE];
time_t inittime;

static void exec_command(msg, info, garbage)
     char *msg;
     char *info;
     void (*garbage)();
{
  if (verbose) {
    (void)fputs(buf, stderr);
    (void)putc('\n', stderr);
  }
  if (!dryrun) {
    if (parallelism == 0) {
      if (system(buf) != 0) {
	(void)fprintf(stderr, msg, info);
	(void)putc('\n', stderr);
	garbage(info);
	exit(1);
      }
    } else {
      pid_t child;

      if (forked >= parallelism) {
	int status;
	while (1) {
	  pid_t wait_result = wait(&status);
	  if (wait_result < 0) {
	    if (errno != ECHILD) continue;
	    perror("Unexpected error in wait");
	    sync_and_exit();
	  } else if (status != 0) {
	    sync_and_exit();
	  } else {
	    break;
	  }
	}
	forked--;
      }

      if ((child = fork()) != 0) {
	forked++;
	return;
      } else if (system(buf) != 0) {
	(void)fprintf(stderr, msg, info);
	(void)putc('\n', stderr);
	garbage(info);
	exit(1);
      } else {
	exit(0);
      }
    }
  }
}

static void sync_with_subtasks()
{
  int status;
  while (wait(&status) >= 0) {
    if (status != 0) {
      sync_and_exit();
    }
    forked--;
  }
}

static char *make_path(name)
     char *name;
{
  char *path;
  if (dbdir != 0) {
    path = (char *)malloc(strlen(dbdir)+strlen(name)+2);
    (void)sprintf(path, "%s/%s", dbdir, name);
  } else {
    path = name;
  }
  return path;
}

/* Cleaning after commmand failure */
static void delete_file(file, prefix, suffix)
     char *file, *prefix, *suffix;
{
  buf[0] = 0;
  (void)sprintf(buf, "%s%s%s%s%s",
		(prefix ? prefix : ""), (prefix ? "/" : ""),
		file,
		(suffix ? "." : ""), (suffix ? suffix : ""));
  if (unlink(buf) == 0) (void)fprintf(stderr, "%s removed\n", buf);
}

static void failed_kl1c(file)
     char *file;
{
  delete_file(file, 0, "c");
  delete_file(file, 0, "ext");
}

static void failed_makedb(dbdir)
     char *dbdir;
{
  delete_file("atom", 0, "h");
  delete_file("funct", 0, "h");
  delete_file("atom", dbdir, "c");
  delete_file("funct", dbdir, "c");
}

static void noop(dummy)
     char *dummy;
{}

static void guess_linker()
{
  /* Decide what linker to use */
  struct namerec *f = files;
  int prio = -1;

  for(; f != 0; f = f->next) {
    if(f->type == prog_file) {
      if(prio < f->info->prio) {
	pref_info = f->info;
	prio = f->info->prio;
      }
    }
  }
}

static void kl1_to_c()
{
  struct namerec *f = files;

  while (f != 0) {
    if (f->type == kl1_file) {
      int recompile = 0;
      struct stat kl1_stat, c_stat, ext_stat;
      (void)sprintf(buf, "%s.kl1", f->name);
      get_stat(buf, &kl1_stat);
      (void)sprintf(buf, "%s.c", f->name);
      if (do_recompile ||
	  stat(buf, &c_stat) != 0 ||
	  kl1_stat.st_mtime > c_stat.st_mtime) {
	recompile = 1;
      } else {
	(void)sprintf(buf, "%s.kl1", f->name);
	if (do_recompile ||
	    stat(buf, &ext_stat) != 0 ||
	    kl1_stat.st_mtime > ext_stat.st_mtime) {
	  recompile = 1;
	}
      }
      if (recompile) {
	(void)sprintf(buf, "%s %s.kl1 </dev/null", klic_compiler, f->name);
	exec_command("KL1 to C translation failed for %s",
		     f->name, failed_kl1c);
      }
      f->type = prog_file;
      f->info = c_info;
    }
    f = f->next;
  }
}

make_database()
{
  struct namerec *f;
  struct stat initstat;
  int makedb = 0;
  (void)sprintf(buf, "%s%sklicdb.init",
		(initdbdir ? initdbdir : ""),
		(initdbdir ? "/" : ""));
  get_stat(buf, &initstat);
  inittime = initstat.st_mtime;
  (void)sprintf(buf, "%s %s%s%s%s%s%s",
		klic_dbmaker,
		(no_cc ? " -n" : ""), (no_link ? " -c" : ""),
		(initdbdir ? " -X " : ""), (initdbdir ? initdbdir : ""),
		(dbdir ? " -x " : ""), (dbdir ? dbdir : ""));
  for (f = files; f != 0; f = f->next) {
    char xbuf[BUFSIZE];
    (void)sprintf(xbuf, "%s.ext", f->name);
    if (access(xbuf, F_OK) == 0) {
      makedb = 1;
      (void)strcat(buf, " ");
      (void)strcat(buf, xbuf);
    }
  }
  if (makedb || !no_cc) {
    exec_command("Atom/functor database merging failed",
		 dbdir, failed_makedb);
  }
}

static void
compile_c_file(name, inf, masm)
     char *name;
     struct lang_info *inf;
     int masm;
{
  int recompile;
  char suffix = (masm ? 's' : 'o');
  char *bufp;
  struct stat c_stat, o_stat;

  (void)sprintf(buf, "%s%s", name, inf->ext);
  get_stat(buf, &c_stat);
  (void)sprintf(buf, "%s.%c", name, suffix);
  recompile =
    (do_recompile ||
     stat(buf, &o_stat) != 0 ||
     c_stat.st_mtime >= o_stat.st_mtime ||
     inittime >= o_stat.st_mtime);
  if (recompile) {
    struct namerec *inc;
    (void)
      sprintf(buf, "%s -%c %s%s%s %s -o %s.%c",
	      inf->comp, (masm ? 'S' : 'c'), optflags,
	      (cdebug ? " -g" : ""), cc_profile_flag,
	      (inf->comp_opt ? inf->comp_opt : ""),
	      name, suffix);
    bufp = buf+strlen(buf);
    for (inc = incdirs; inc != 0; inc = inc->next) {
      (void)sprintf(bufp, " -I%s", inc->name);
      bufp += strlen(bufp);
    }
    /*** for Distributed KLIC system ***/
    if (distklic) {
      DIST_COMPILER_FLAG(name, inf->ext);
    } else if ( shmklic ) {
      /*
	(void)sprintf(bufp, " -DSHM -I%s -I. %s%s", klic_incdir, name, inf->ext);
      */
      (void)sprintf(bufp, " -DSHM %s -I. %s%s", klic_incdir, name, inf->ext);
    } else {
      /*
	(void)sprintf(bufp, " -I%s -I. %s%s", klic_incdir, name, inf->ext);
      */
      (void)sprintf(bufp, " %s -I. %s%s", klic_incdir, name, inf->ext);
    }
    {
      char tmpbuf[256];
      (void)sprintf(tmpbuf, "%s%s", name, inf->ext);
      exec_command("Compilation failed for file %s",
		   tmpbuf, inf->ext, noop);
    }
  }
}

static void
compile_s_file(name, masm)
 char *name;
 int masm; 
{
 if (!masm) {
   struct stat s_stat, o_stat;
   (void)sprintf(buf, "%s.s", name);
   get_stat(buf, &s_stat);
   (void)sprintf(buf, "%s.o", name);
   if (do_recompile ||
       stat(buf, &o_stat) != 0 ||
       s_stat.st_mtime >= o_stat.st_mtime) {
     /*
       (void)sprintf(buf, "%s -c %s%s -I%s %s -o %s.o %s.s",
                     klic_cc, optflags,
                     (cdebug ? " -g" : ""),
                     klic_incdir,
		     (klic_cc_options ? klic_cc_options : ""),
		     name, name);
     */
     (void)sprintf(buf, "%s -c %s%s %s %s -o %s.o %s.s",
		   klic_cc, optflags,
		   (cdebug ? " -g" : ""),
		   klic_incdir,
		   (klic_cc_options ? klic_cc_options : ""),
		   name, name);
     
     exec_command("Assembly failed for file %s.s",
		  name, noop);
   }
 }
}

static void
c_to_o(void)
{
  struct namerec *f;
  int status;

  for (f = files; f != 0; f = f->next) {
    if(f->type == prog_file) {
      char *tmpname = f->name;
      struct lang_info *tmpinfo = f->info;
      compile_c_file(tmpname, tmpinfo, make_asm);
    } else if (f->type == asm_file && !make_asm) {
      compile_s_file(f->name, make_asm);
    }
    f->type = (make_asm ? asm_file : obj_file);
  }
}

make_atom_o()
{
  compile_c_file(make_path("atom"), c_info, 0);
}

make_funct_o()
{
  compile_c_file(make_path("funct"), c_info, 0);
}

make_pred_o()
{
  compile_c_file(make_path("predicates"), c_info, 0);
}

linkage()
{
  struct namerec *f;
  char *bufp;
  char *ld;
  char *ldopt;

  if(forced_linker) {
    ld = forced_linker;
  } else {
    ld = pref_info->ld;
  }

  if(forced_linker_opt) {
    ldopt = forced_linker_opt;
  } else {
    ldopt = pref_info->ld_opt;
  }

  /* We will link the program anyway, as we can't tell whether the */
  /* existing executable was linked with the tracing library or not */

  (void)sprintf(buf, "%s%s%s%s %s %s %s %s",
		ld,
		(ofile ? " -o " : ""), (ofile ? ofile : ""),
		link_profile_flag,
		(ldopt ? ldopt : ""),
		make_path("atom.o"),
		make_path("funct.o"),
		( debug ? make_path("predicates.o") : ""));
  bufp = buf+strlen(buf);
  for (f = files; f != 0; f = f->next) {
    (void)sprintf(bufp, " %s.o", f->name);
    bufp += strlen(bufp);
  }
  for (f = libdirs; f != 0; f = f->next) {
    (void)sprintf(bufp, " -L%s", f->name);
    bufp += strlen(bufp);
  }
  (void)sprintf(bufp, " -L%s", klic_libdir);
  bufp += strlen(bufp);
  {
    char *tptr;
    if ( distklic ) {
      tptr = LIBRARIES_D;
    } else if ( shmklic ) {
      tptr = LIBRARIES_S;
    } else if ( debug ) {
      tptr = LIBRARIES_T;
    } else {
      tptr = LIBRARIES;
    }
    (void)sprintf(bufp, " %s", tptr);
  }
  bufp += strlen(bufp);
  for (f = libraries; f != 0; f = f->next) {
    (void)sprintf(bufp, " -l%s", f->name);
    bufp += strlen(bufp);
  }
  if (distklic) {
    DIST_LINKAGE_FLAG();
  }
  exec_command("Linkage failed", 0, noop);
}

#define Optarg() \
( argv[optind][charind+1] != 0 ? \
  argv[optind]+charind+1 : \
  (optind++, argv[optind] ))

#define Nextarg() \
( argv[++optind] )

main(argc,argv)
     int argc;
     char **argv;
{
  extern char *option_value();
  int optind;
  int optc;
  int c;

#ifdef SETLINEBUF
  setlinebuf(stderr);
#endif

/*
  get environment variable values
*/

  klic_libdir = option_value("KLIC_LIBRARY", KLICLIB);
  initdbdir = option_value("KLIC_DBINIT", klic_libdir);
  klic_compiler = option_value("KLIC_COMPILER", KLIC_COMPILER);
  klic_dbmaker = option_value("KLIC_DBMAKER", KLIC_DBMAKER);
  klic_incdir = option_value("KLIC_INCLUDE", KLICINCLUDE0);

  /* C is a special language */
  klic_cc = option_value("KLIC_CC", CC);
  klic_cc_options = option_value("KLIC_CC_OPTIONS", KLIC_CC_OPTIONS);

  /* and other options ... */
  initialize_lang_info();

/*
  process command line options
*/
  for (optind=1;
       optind<argc && argv[optind][0]=='-';
       optind++) {
    int charind;
    for (charind = 1;
	 argv[optind][charind] != 0;
	 charind++) {
      switch (c=argv[optind][charind]) {
      case 'c': no_link = 1; break;
      case 'C': no_cc = 1; break;
      case 'd': 
	if (strcmp(argv[optind],"-dp")==0) {
	  distklic = 1; goto nextarg;
	} else {
	  dryrun = 1; verbose = 1; break;
	}
      case 'D': klic_dbmaker = Optarg(); goto nextarg;
      case 'g': cdebug = 1; break;
      case 'F':
	if(strcmp(argv[optind], "-Fld") == 0) {
	  forced_linker = Nextarg(); goto nextarg;
	} else if (strcmp(argv[optind], "-Flo") == 0) {
	  forced_linker_opt = Nextarg(); goto nextarg;
	}
      case 'K': klic_compiler = Optarg(); goto nextarg;
      case 'h': usage_help(argv[0]); exit(0);
      case 'I':
      case 'L':
      case 'l': {
	struct namerec *newname =
	  (struct namerec *)malloc(sizeof(struct namerec));
	newname->name = Optarg();
	newname->type = other_file;
	switch (c) {
	case 'I':
	  newname->next = incdirs;
	  incdirs = newname;
	  break;
	case 'L':
	  newname->next = libdirs;
	  libdirs = newname;
	  break;
	case 'l':
	  newname->next = libraries;
	  libraries = newname;
	  break;
	}
	goto nextarg;
      }
      case 'o': ofile = Optarg(); goto nextarg;
      case 'O':
	if (isdigit(argv[optind][charind+1])) {
	  optlevel = atoi(argv[optind]+charind+1);
	  goto nextarg;
	} else {
	  optlevel = -1;
	  break;
	}
      case 'p':
	switch (argv[optind][charind+1]) {
	case '\0':
	  cc_profile_flag = " -p";
	  link_profile_flag = " -p";
	  break;
	case 'l':
	  if (argv[optind][charind+2] != '\0') usage_error(argv[0]);
	  cc_profile_flag = "";
	  link_profile_flag = " -p";
	  break;
	case 'g':
	  if (argv[optind][charind+2] != '\0') usage_error(argv[0]);
	  cc_profile_flag = " -pg";
	  link_profile_flag = " -pg";
	  break;
	default: usage_error(argv[0]);
	}
	goto nextarg;
      case 'P':
	parallelism = atoi(Optarg()); goto nextarg;
      case 'R': do_recompile = 1; break;
      case 'S': make_asm = 1; break;
      case 's':
	if (strcmp(argv[optind],"-shm")==0) {
	  shmklic = 1; goto nextarg;
	}
      case 'v': verbose = 1; break;
      case 'n': debug = 0; break;
      case 't':
	fprintf(stderr, "This version links debugging library by default\n");
	fprintf(stderr, "To avoid linking debugging library, specify -n\n");
	break;
      case 'X': initdbdir = Optarg(); goto nextarg;
      case 'x': dbdir = Optarg(); goto nextarg;
      default: usage_error(argv[0]);
      }
    }
  nextarg:;
  }

  if (verbose) {
    fprintf(stderr, "KLIC compiler driver version %s (%s)\n",
	    KLIC_VERSION, KLIC_DATE);
  }
  if (optlevel==0) {
    optflags = "";
  } else {
    if (optlevel<1) {
      (void)sprintf(buf, " -O %s", UOPTFLAGS);
    } else {
      (void)sprintf(buf, " -O%d %s", optlevel, UOPTFLAGS);
    }
    optflags = strcpy((char *)malloc(strlen(buf)+1), buf);
  }
  for (optc = optind; optc < argc; optc++) {
    char *suffix;
    struct namerec *newname =
      (struct namerec *)malloc(sizeof(struct namerec));
    suffix = strrchr(argv[optc], '.');
    if (suffix == 0) {
      error_exit("Can't process file: %s", argv[optc]);
    } else {
      int len = suffix-argv[optc];
      struct lang_info *info;
      newname->name =
	strncpy((char *)malloc(len+1), argv[optc], len);
      newname->name[len] = 0;
      if (strcmp(suffix, ".kl1") == 0) {
	newname->type = kl1_file;
      } else if (strcmp(suffix, ".a") == 0) {
	newname->type = asm_file;
      } else if (strcmp(suffix, ".o") == 0) {
	newname->type = obj_file;
      } else if ((info = search_lang_info(suffix)) != 0) {
	newname->type = prog_file;
	newname->info = info;
      } else {
	error_exit("Can't process file: %s", argv[optc]);
      }
    }
    newname->next = files;
    files = newname;
  }

  reverse_names(&files);
  reverse_names(&libraries);
  reverse_names(&libdirs);


  guess_linker();
  kl1_to_c();
  if (!no_cc) {
    sync_with_subtasks();
    make_database();
    sync_with_subtasks();
    c_to_o();
    if (!make_asm) {
#ifdef USELOCKF
      int fd;
      char fdbuf[BUFSIZE];
      strcpy(fdbuf,make_path(DBFILENAME));
      strcat(fdbuf,".lock");
      lockf(fd=open(fdbuf,O_RDONLY),F_LOCK,1);
#endif
      if (!no_link) {
	make_atom_o();
	make_funct_o();
	if (debug) make_pred_o();
	sync_with_subtasks();
	linkage();
      }
      sync_with_subtasks();
#ifdef USELOCKF
      close(fd);
      lockf(fd,F_ULOCK,1);
#endif
    }
  }
  sync_with_subtasks();
  return 0;
}
