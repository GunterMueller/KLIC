/* ---------------------------------------------------------- 
%   (C)1993, 1994, 1995 Institute for New Generation Computer Technology 
%       (Read COPYRIGHT for detailed information.) 
%   (C)1996, 1997, 1998, 1999 Japan Information Processing Development Center
%       (Read COPYRIGHT-JIPDEC for detailed information.)
----------------------------------------------------------- */
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <klic/basic.h>
#include <klic/config.h>
#include "klic.h"
#ifdef USELOCKF
#include <fcntl.h>
#endif

extern void *malloc();

char *dbdir = 0;
char *initdbdir = 0;
int nocfiles = 0;
int nolink = 0;
int makelib = 0;
char *libmade;

struct atomrec {
  struct atomrec *next;
  int id;
  unsigned char *name;
} *atomroot;
int nextatom;

struct functrec {
  struct functrec *next;
  int id;
  struct atomrec *principal_functor;
  int arity;
} *functroot;
int nextfunct;

enum mod_or_class { Mod, Class };

struct modrec {
  struct modrec *next;
  unsigned char *name;		/* name of the module */
  enum mod_or_class morc;	/* whether itis a class or a module */
  struct predrec *defined;	/* predicates defined in the module */
				/* sorted by names alphabetically */
  struct refmodrec *refmod;	/* module referenced from the module */
				/* sorted by names alphabetically */
} *modroot, currentmod;

struct refmodrec {
  struct refmodrec *next;
  enum mod_or_class morc;
  unsigned char *name;
};

struct predrec {
  struct predrec *next;
  unsigned char *name;
};

int changes;

#ifdef __STDC__
static voidfn error_exit;
#endif
static void error_exit(format, arg1, arg2)
     char *format, *arg1, *arg2;
{
  (void)fprintf(stderr, format, arg1, arg2);
  putc('\n', stderr);
  exit(-1);
}

usage_error(command)
     char *command;
{
  error_exit("Usage: %s [-n] [-x dbdir] [-X initdbdir] file ...", command, 0);
}

static struct modrec *find_module(name, morc, root)
     unsigned char *name;
     enum mod_or_class morc;
     struct modrec *root;
{
  while (root != 0 &&
	 (root->morc != morc ||
	  strcmp((char *)name, (char *)root->name) != 0)) {
    root = root->next;
  }
  return root;
}

static struct atomrec *enter_atom(name)
     unsigned char *name;
{
  struct atomrec *a;
  int len = strlen((char *)name);
  for (a=atomroot; a!=0; a=a->next) {
    if (strcmp((char *)name,(char *)a->name) == 0) return a;
  }
  a = (struct atomrec *)malloc(sizeof(struct atomrec));
  a->name = (unsigned char *)strcpy((char *)malloc(len+1),(char *)name);
  a->next = atomroot;
  a->id = nextatom++;
  atomroot = a;
  changes = 1;
  return a;
}

static void enter_functor(name)
     unsigned char *name;
{
  struct functrec *f;
  unsigned char *p;
  struct atomrec *functor;
  int arity;

  for (p=name+strlen((char *)name)-1; *p!='_'; p--)
    ;
  *p = 0;
  arity = atoi(p+1);
  functor = enter_atom(name);
  for (f=functroot; f!=0; f=f->next) {
    if (f->principal_functor == functor &&
	f->arity == arity)
      return;
  }
  f = (struct functrec *)malloc(sizeof(struct functrec));
  f->principal_functor = functor;
  f->arity = arity;
  f->next = functroot;
  f->id = nextfunct++;
  functroot = f;
  changes = 1;
}

static void update_module_info()
{
  /* Compares the newly read module info in currenmod
     with the original info in klic.db (if existent),
     and updates the info if any changes are found */
  struct modrec *m;

  for (m=modroot; m!=0; m=m->next) {
    if (m->morc == currentmod.morc &&
	strcmp((char *)m->name, (char *)currentmod.name) == 0) {
      struct refmodrec *rmo, *rmn;
      for (rmo=m->refmod, rmn=currentmod.refmod;
	   rmo != 0 && rmn != 0 &&
	   strcmp((char *)rmo->name, (char *)rmn->name) == 0;
	   rmo=rmo->next, rmn=rmn->next)
	;
      if (rmo==0 || rmn == 0) {
	struct predrec *po, *pn;
	for (po=m->defined, pn=currentmod.defined;
	     po !=0 && pn !=0 &&
	     strcmp((char *)po->name, (char *)pn->name) == 0;
	     po=po->next, pn=pn->next)
	  ;
	if (po==0 && pn==0) {
	  return;
	}
      }
      m->refmod = currentmod.refmod;
      m->defined = currentmod.defined;
      return;
    }
  }
  m = (struct modrec *)malloc(sizeof(struct modrec));
  *m = currentmod;
  m->next = modroot;
  modroot = m;
  return;
}

static void enter_module(name, morc)
     unsigned char *name;
     enum mod_or_class morc;
{
  if (currentmod.name != 0) {
    /* not the first module to be entered */
    update_module_info();
  }
  currentmod.name = (unsigned char *)
    strcpy((char *)malloc(strlen((char *)name)+1),(char *)name);
  currentmod.morc = morc;
  currentmod.defined = 0;
  currentmod.refmod = 0;
}

static void enter_predicate(name)
     unsigned char *name;
{
  struct predrec *p, **r;

  for (p=currentmod.defined; p!=0; p=p->next) {
    int res = strcmp((char *)name, (char *)p->name);
    if (res == 0) return;
    if (res > 0) break;
  }
  p = (struct predrec *)malloc(sizeof(struct predrec));
  p->name = (unsigned char *)
    strcpy((char *)malloc(strlen((char *)name)+1),(char *)name);
  for (r = &currentmod.defined;
       *r != 0 && strcmp((char *)name, (char *)((*r)->name)) < 0;
       r = &(*r)->next)
    ;
  p->next = *r;
  *r = p;
}

static void enter_ref_module(name, morc)
     unsigned char *name;
     enum mod_or_class morc;
{
  struct refmodrec *m, **r;
  for (m=currentmod.refmod; m!=0; m=m->next) {
    if (morc > m->morc) break;
    if (morc == m->morc) {
      int res = strcmp((char *)name,(char *)m->name);
      if (res > 0) break;
      if (res == 0) return;
    }
  }
  m = (struct refmodrec *)malloc(sizeof(struct refmodrec));
  m->morc = morc;
  m->name = (unsigned char *)
    strcpy((char *)malloc(strlen((char *)name)+1),(char *)name);
  for (r = &currentmod.refmod;
       *r != 0 && morc < (*r)->morc &&
       strcmp((char *)name, (char *)((*r)->name)) < 0;
       r = &(*r)->next)
    ;
  m->next = *r;
  *r = m;
}

static void enter_one_line(buf, pathname)
     char *buf;
     char *pathname;
{
  if (buf[0] == '#') {
    return;			/* comment line */
  } else if (strncmp(buf,"atom_", 5) == 0) {
    (void) enter_atom(buf+5);
  } else if (strncmp(buf,"functor_",8)==0) {
    enter_functor(buf+8);
  } else if (strncmp(buf,"predicate_",10)==0) {
    enter_predicate(buf+10);
  } else if (strncmp(buf,"module_",7)==0) {
    enter_module(buf+7, Mod);
  } else if (strncmp(buf,"class_",6)==0) {
    enter_module(buf+6, Class);
  } else if (strncmp(buf,"ref_module_",11)==0) {
    enter_ref_module(buf+11, Mod);
  } else if (strncmp(buf,"ref_class_",10)==0) {
    enter_ref_module(buf+10, Class);
  } else {
    error_exit("Unrecognized line in file %s:\n%s",
	       pathname, buf);
  }
}

static void read_db_file(file, pathname)
     FILE *file;
     char *pathname;
{
  char buf[BUFSIZE];
  while (fgets(buf,BUFSIZE,file) != 0) {
    *strchr(buf,'\n') = 0;
    enter_one_line(buf, pathname);
  }
  (void)fclose(file);
  if (currentmod.name != 0) {
    /* not the first module to be entered */
    update_module_info();
    currentmod.name = 0;
  }
}

static void read_ext_file(pathname)
     char *pathname;
{
  FILE *file;

  file = fopen(pathname, "r");
  if (file==0) error_exit("Cannot open file %s", pathname, 0);
  read_db_file(file, pathname);
}

static char *make_path(dir, name)
     char *dir;
     char *name;
{
  char *path;
  if (dir != 0) {
    path = (char *)malloc(strlen(dir)+strlen(name)+2);
    (void)sprintf(path, "%s/%s", dir, name);
  } else {
    path = name;
  }
  return path;
}

static FILE *open_cwd_out(name)
     char *name;
{
  FILE *file;
  if ((file=fopen(name, "w")) == 0) {
    error_exit("Cannot open file %s", name, 0);
  }
  return file;
}

static FILE *open_db_out(name)
     char *name;
{
  FILE *file;
  char *path = make_path(dbdir, name);
  if ((file=fopen(path, "w")) == 0) {
    error_exit("Cannot open file %s", path, 0);
  }
  return file;
}

static void reverse_atomrecs()
{
  struct atomrec
    *next = atomroot,
    *x = 0;
  while (next != 0) {
    struct atomrec *last = x;
    x = next;
    next = x->next;
    x->next = last;
  }
  atomroot = x;
}

static void reverse_functrecs()
{
  struct functrec
    *next = functroot,
    *x = 0;
  while (next != 0) {
    struct functrec *last = x;
    x = next;
    next = x->next;
    x->next = last;
  }
  functroot = x;
}

static int hexvalue(c, name)
     int c;
     unsigned char *name;
{
  if ('0' <= c && c <= '9') return c - '0';
  if ('A' <= c && c <= 'F') return c - 'A' + 10;
  if ('a' <= c && c <= 'f') return c - 'a' + 10;
  error_exit("Invalid atom name: %s", name, 0);
}

static void real_atom_name(name,q)
     unsigned char *name, *q;
{
  unsigned char *p = name;
  while (*p != 0) {
    if (*p == '_') {
      int c = *++p;
      if (c == '_') {
	*q = '_';
      } else {
	int n = (hexvalue(c,name) << 4);
	c = *++p;
	*q = n+hexvalue(c,name);
      }
    } else {
      *q = *p;
    }
    p++; q++;
  }
  *q = 0;
}

static void real_pred_name(name,q)
     unsigned char *name, *q;
{
  unsigned char *p = name;
  unsigned char *e;
  while (*p != 0 && (*p != '_' || *(p+1) != 'x'))
    p++;
  p += 2;
  e = p + strlen((char *)p);
  while (*(--e) != '_')
    ;
  while (p < e) {
    if (*p == '_') {
      int c = *++p;
      if (c == '_') {
	*q = '_';
      } else {
	int n = (hexvalue(c,name) << 4);
	c = *++p;
	*q = n+hexvalue(c,name);
      }
    } else {
      *q = *p;
    }
    p++; q++;
  }
  *q = 0;
}

static void print_c_string(file, str)
     FILE *file;
     unsigned char *str;
{
  int c;
  (void)fprintf(file, "(unsigned char *)");
  (void)putc('\"', file);
  while ((c=*str) != 0) {
    if (c=='\\' || c=='"') (void)putc('\\', file);
    (void)putc(c, file);
    str++;
  }
  (void)putc('\"', file);
}

#define TEMPDBNAME ".klic.db"

static write_db_files(inittime)
     time_t inittime;
{
  FILE *atomh, *atomc, *functh, *functc, *newdb;
  struct atomrec *a;
  struct functrec *f;

  newdb = open_db_out(TEMPDBNAME);
  (void)fprintf(newdb, "#KLIC DB for system @%lu\n",
		(unsigned long)inittime);

  reverse_atomrecs();
  if (!nocfiles) {
    atomh = open_cwd_out("atom.h");
    atomc = open_db_out("atom.c");
    (void)fprintf(atomh, "#include <klic/atomstuffs.h>\n");
    (void)fprintf(atomc, "#include <klic/basic.h>\n");
    (void)fprintf(atomc, "unsigned char * Const init_atomname[] = {\n");
  }
  for (a=atomroot; a!=0; a=a->next) {
    unsigned char realname[BUFSIZE];
    real_atom_name(a->name,realname);
    if (!nocfiles) {
      (void)fprintf(atomh, "#define atom_%s %dL+ATOMNUMBERBASE\n",
		    a->name, a->id);
      (void)putc('\t', atomc);
      print_c_string(atomc, realname);
      (void)putc(',', atomc);
      (void)putc('\n', atomc);
    }
    (void)fprintf(newdb, "atom_%s\n", a->name);
  }
  if (!nocfiles) {
    (void)fprintf(atomc, "};\n");
    (void)fprintf(atomc,
		  "Const unsigned long initial_atoms = %d;\n", nextatom);
    (void)fprintf(atomc, "unsigned char **atomname = (unsigned char **)init_atomname;\n");
    (void)fclose(atomh);
    (void)fclose(atomc);
  }

  reverse_functrecs();
  if (!nocfiles) {
    functh = open_cwd_out("funct.h");
    functc = open_db_out("funct.c");
    (void)fprintf(functh, "#include <klic/functorstuffs.h>\n\n");
    (void)fprintf(functc, "#include <klic/basic.h>\n");
    (void)fprintf(functc, "#include <klic/atomstuffs.h>\n\n");
    (void)fprintf(functc, "Const unsigned long init_functors[] = {\n");
  }
  for (f=functroot; f!=0; f=f->next) {
    if (!nocfiles) {
      (void)fprintf(functh, "#define functor_%s_%d\t%dL+FUNCTORNUMBERBASE\n",
		    f->principal_functor->name, f->arity,
		    f->id);
      (void)fprintf(functc, "\t%dL+ATOMNUMBERBASE,\n",
		    f->principal_functor->id);
    }
    (void)fprintf(newdb, "functor_%s_%d\n",
		  f->principal_functor->name, f->arity);
  }
  (void)fclose(newdb);


  if (!nocfiles) {
    (void)fprintf(functc, "};\n\nConst unsigned long init_arities[] = {\n");
    for (f=functroot; f!=0; f=f->next) {
      (void)fprintf(functc, "\t%dL,\n", f->arity);
    }
    (void)fprintf(functc, "};\n");
    (void)fprintf(functc,
		  "Const unsigned long initial_functors = %d;\n",
		  nextfunct);
    (void)fprintf(functc,
		  "unsigned long *functors = (unsigned long *)init_functors;\n");
    (void)fprintf(functc,
		  "unsigned long *arities = (unsigned long *)init_arities;\n");
    (void)fclose(functh);
    (void)fclose(functc);
  }

  {
    char *newpath = make_path(dbdir, TEMPDBNAME);
    char *dbpath = make_path(dbdir, DBFILENAME);
    if (rename(newpath, dbpath)) {
      error_exit("Can't rename %s to %s", newpath, dbpath);
    }
  }
}

write_predicate_file()
{
  FILE *predc;
  struct modrec *m;
  if (!nolink) {
    predc = open_db_out("predicates.c");
    (void)fprintf(predc, "#include <klic/basic.h>\n");
    (void)fprintf(predc, "#include <klic/predinfo.h>\n");
    (void)fprintf(predc, "\n");
  }

  if (makelib) {
    char libnamebuf[1024];
    FILE *libdb;
    sprintf(libnamebuf, "lib%s.db", libmade);
    libdb = open_db_out(libnamebuf);
    for (m=modroot; m!=0; m=m->next) {
      struct predrec *p;
      struct refmodrec *r;
      if (m->morc == Mod) {
	(void)fprintf(libdb, "module_%s\n", m->name);
      } else {
	(void)fprintf(libdb, "class_%s\n", m->name);
      }
      for (p=m->defined; p!=0; p=p->next) {
	(void)fprintf(libdb, "predicate_%s\n", p->name);
      }
      for (r=m->refmod; r!=0; r=r->next) {
	if (r->morc == Mod) {
	  (void)fprintf(libdb, "ref_module_%s\n", r->name);
	} else {
	  (void)fprintf(libdb, "ref_class_%s\n", r->name);
	}
      }
    }
    (void)fclose(libdb);
  }
  if (!nocfiles) {
    struct modrec *linkedmod = 0;
    struct searched_mod {
      unsigned char *name;
      enum mod_or_class morc;
    };
#define Push(nam, mc) { smdp->name = nam; smdp->morc = mc; smdp++; }
    struct searched_mod searchmod[4096];
    struct searched_mod *smdp = searchmod;
    Push((unsigned char *)"main", Mod);
    Push((unsigned char *)"generic", Mod);
    Push((unsigned char *)"unify__term__dcode", Mod);
    Push((unsigned char *)"integer__arithmetics", Mod);
    while (smdp != searchmod) {
      struct modrec *searched;
      struct refmodrec *refmods;
      --smdp;
      searched = find_module(smdp->name, smdp->morc, modroot);
      if (searched != 0) {
	struct modrec *newlinked;
	newlinked = (struct modrec *)malloc(sizeof(struct modrec));
	*newlinked = *searched;
	newlinked->next = linkedmod;
	linkedmod = newlinked;
	for (refmods = searched->refmod;
	     refmods != 0;
	     refmods = refmods->next) {
	  if (find_module(refmods->name, refmods->morc, linkedmod) == 0) {
	    struct searched_mod *pp;
	    Push(refmods->name, refmods->morc);
	    for (pp=searchmod;
		 pp->morc != refmods->morc ||
		 strcmp((char *)(pp->name), (char *)refmods->name) != 0;
		 ++pp)
	      ;
	    if (pp != smdp-1) smdp--;
	  }
	}
      }
    }
    if (!nolink) {
      for (m=linkedmod; m!=0; m=m->next) {
	if (m->morc == Mod) {
	  struct predrec *p;
	  for (p=m->defined; p!=0; p=p->next) {
	    (void)fprintf(predc,
			  "extern Const struct predicate predicate_%s;\n",
			  p->name);
	    (void)fprintf(predc,
			  "static Const struct predinfo predinfo_%s = {\n",
			  p->name);
	    (void)fprintf(predc, "  ");
	    {
	      unsigned char realname[BUFSIZE];
	      real_pred_name(p->name,realname);
	      (void)print_c_string(predc, realname);
	    }
	    (void)fprintf(predc, ",\n");
	    (void)fprintf(predc, "  &predicate_%s\n", p->name);
	    (void)fprintf(predc, "};\n");
	    (void)fprintf(predc, "\n");
	  }
	  (void)
	    fprintf(predc,
		    "static Const struct predinfo * Const preds_in_%s[] = {\n",
		    m->name);
	  for (p=m->defined; p!=0; p=p->next) {
	    (void)fprintf(predc, "  &predinfo_%s,\n", p->name);
	  }
	  (void)fprintf(predc, "  0\n");
	  (void)fprintf(predc, "};\n");
	}
      }
      (void)fprintf(predc, "Const struct modinfo defined_modules[] = {\n");
      for (m=linkedmod; m!=0; m=m->next) {
	if (m->morc == Mod) {
	  (void)fprintf(predc, "  ");
	  {
	    unsigned char realname[BUFSIZE];
	    real_atom_name(m->name, realname);
	    (void)print_c_string(predc, realname);
	  }
	  (void)fprintf(predc, ",\n");
	  (void)fprintf(predc, "  preds_in_%s,\n", m->name);
	}
      }
      (void)fprintf(predc, "  0, 0\n");
      (void)fprintf(predc, "};\n");
      (void)fclose(predc);
    }
  }
}

from_same_installation(klicdb, inittime)
     FILE *klicdb;
     time_t inittime;
{
  int c;
  time_t dbtime;
  if (fgetc(klicdb) != '#') return 0;
  do {
    c = fgetc(klicdb);
    if (c == '\n' || c == EOF) {
      return 0;
    }
  } while (c != '@');
  {
    unsigned long temp;
    if (fscanf(klicdb, "%lu", &temp) != 1) return 0;
    dbtime = (time_t)temp;
  }
  if (dbtime != inittime) return 0;
  rewind(klicdb);
  return 1;
}

#define Optarg() \
( argv[optind][charind+1] != 0 ? \
  argv[optind]+charind+1 : \
  (optind++, argv[optind] ))

static void unlink_file(name)
     char *name;
{
  int err = unlink(name);
  if (err == 0) return;
  if (errno != ENOENT) {
    error_exit("Can't clear file: %s", name);
  }
}

main(argc,argv)
     int argc;
     char **argv;
{
  int optind;
  char *dbpath, *initpath, *libdbpath;
  FILE *klicdb, *libdb;
  int n;
  int c;
  struct stat initstat, libdbstat;
#ifdef USELOCKF
  int fd;
  char fdbuf[BUFSIZE];
#endif
  for (optind = 1;
       optind<argc && argv[optind][0]=='-';
       optind++) {
    int charind;
    for (charind = 1;
	 argv[optind][charind] != 0;
	 charind++) {
      switch (c = argv[optind][charind]) {
      case 'X': initdbdir = Optarg(); goto nextarg;
      case 'x': dbdir = Optarg(); goto nextarg;
      case 'n': nocfiles = 1; nolink = 1; break;
      case 'c': nolink = 1; break;
      case 'l': makelib = 1; libmade = Optarg(); goto nextarg;
      default: usage_error(argv[0]);
      }
    }
  nextarg:;
  }

  if (initdbdir == 0) {
    fprintf(stderr, "Initial KLIC database directory not specified\n");
    usage_error(argv[0]);
  }

  atomroot = 0; nextatom = 0;
  functroot = 0; nextfunct = 0;
  currentmod.name = 0;

  initpath = make_path(initdbdir, "klicdb.init");
  libdbpath = make_path(initdbdir, LIBDBNAME);
  dbpath = make_path(dbdir, DBFILENAME);

  if (stat(initpath, &initstat) != 0) {
    error_exit("Can't access file: %s", initpath);
  }
  if (!makelib && stat(libdbpath, &libdbstat) != 0) {
    error_exit("Can't access file: %s", libdbpath);
  }

#ifdef USELOCKF
  strcpy(fdbuf, dbpath);
  strcat(fdbuf, ".lock");
  lockf(fd=open(fdbuf,O_RDWR), F_LOCK, 1);
#endif

  if (!makelib) {
    libdb = fopen(libdbpath, "r");
    read_db_file(libdb, libdbpath);
    changes = 0;
  }
  klicdb = fopen(dbpath, "r");
  if (klicdb != 0 &&
      from_same_installation(klicdb, initstat.st_mtime)) {
    read_db_file(klicdb, dbpath);
    changes = 0;
  } else {
    FILE *klicinit = fopen(initpath, "r");
    if (klicinit == 0) {
      error_exit("Can't open initial database file: %s", initpath, 0);
    }
    unlink_file("atom.h");
    unlink_file(make_path(dbdir, "atom.c"));
    unlink_file("funct.h");
    unlink_file(make_path(dbdir, "funct.c"));
    unlink_file(make_path(dbdir, "predicates.c"));
    read_db_file(klicinit, initpath);
    changes = 1;
  }

  for (n=optind; n<argc; n++) {
    read_ext_file(argv[n]);
  }

  {
    struct stat buf;
    if (changes ||
	stat(make_path(dbdir, "klic.db"), &buf) != 0 ||
	(!nocfiles &&
	 (stat("atom.h", &buf) != 0 ||
	  stat("funct.h", &buf) != 0 ||
	  stat(make_path(dbdir, "atom.c"), &buf) != 0 ||
	  stat(make_path(dbdir, "funct.c"), &buf) != 0))) {
      write_db_files(initstat.st_mtime);
    }
  }
  write_predicate_file();

#ifdef USELOCKF
  close(fd);
  lockf(fd,F_ULOCK,1);
#endif
  return 0;
}
