/* ----------------------------------------------------------
%   (C)1993,1994 Institute for New Generation Computer Technology
%       (Read COPYRIGHT for detailed information.)
%   (C)1996, 1997, 1998, 1999 Japan Information Processing Development Center
%       (Read COPYRIGHT-JIPDEC for detailed information.)
----------------------------------------------------------- */

#include <stdio.h>
#include <klic/gcobject.h>
#include <klic/g_pointer.h>
#include <klic/g_string.h>
#include <klic/gd_macro.h>
#ifdef DIST
#include <klic/distio.h>
#endif
#include "atom.h"
#include "funct.h"

struct file_io_object {
  struct consumer_object_method_table *method_table;
  int linecount;
  q inname, outname;
  q stream; /* saved input stream for suspension by some other reasons */
  FILE *infile, *outfile;
};

#define GC_CLASS_NAME() file__io
#define GC_OBJ_TYPE struct file_io_object
#define GC_OBJ_SIZE(obj)  G_SIZE_IN_Q(GC_OBJ_TYPE)

GD_USE_CLASS(byte__string);
GD_USE_CLASS(pointer);

#include <klic/gc_macro.h>

/* basic method definitions */

static long do_flush(obj)
     struct file_io_object *obj;
{
#ifdef DIST
  if (obj->infile == stdin ||
      (obj->outfile == stdout || obj->outfile == stderr)) {
    return UserFflush();
  } else
#endif
    {
      return klic_fflush(obj->outfile);
    }
}

#define CheckInput()				    \
{						    \
  if (GC_SELF->infile == 0) goto message_error;	    \
  if (GC_SELF->outfile != 0) do_flush(GC_SELF);	    \
}

#define CheckOutput()				    \
{						    \
  if (GC_SELF->outfile == 0) goto message_error;    \
}

GCDEF_UNIFY()
{
  G_STD_DECL;
  q newvar;
  q reason;

  if (GC_SELF->stream != 0) {
    GC_TERM = GC_SELF->stream;
    GC_SELF->stream = 0;
  }
 top:
  if (G_ISCONS(GC_TERM)) {
    q message;
    GCSET_MESSAGE(message);
    if (G_ISINT(message)) {
      long c = G_INTVAL(message);
#ifdef DIST
      if (GC_SELF->outfile == stdout) {
	  UserPutc(c);
      } else if (GC_SELF->outfile == stderr) {
	  UserEPutc(c);
      } else
#endif
	{
	    if (klic_putc(c, GC_SELF->outfile) == EOF) {
		GC_FAIL("putc failed");
	    }
	}
    } else if (G_ISFUNCTOR(message)) {
      switch (G_SYMVAL(G_FUNCTOR_OF(message))) {
	/*** Input Messages ***/
      case functor_getc_1: {
	long c;
	CheckInput();
#ifdef DIST
	if (GC_SELF->infile == stdin) {
          c = UserGetc();
	} else
#endif
	  {
	    c = klic_getc(GC_SELF->infile);
	  }
	if (c=='\n') { GC_SELF->linecount++; }
	GC_UNIFY_VALUE(G_ARG(message,0), G_MAKEINT(c));
	break;
      }
      case functor_ungetc_1: {
	long c;
	CheckInput();
	GCSET_MESSAGE_INT_ARG(c, message, 0);
	if (c=='\n') { GC_SELF->linecount--; }
#ifdef DIST
	if (GC_SELF->infile == stdin) {
	  UserUngetc(c);
	} else
#endif
	  {
	      ungetc(c, GC_SELF->infile);
	  }
	break;
      }
      case functor_fread_2: {
	extern char *malloc_check();
	extern q convert_binary_c_string_to_klic_string();
	long n, k;
	char *buf;
	q string;
	long space_needed;
	CheckInput();
	GCSET_MESSAGE_INT_ARG(n, message, 0);
	if (n<0) goto message_error;
	space_needed = sizeof(struct byte_string_object)+n+sizeof(q);
	if ((char *)g_allocp+space_needed >= (char *)real_heaplimit) {
	  this_more_space += (space_needed+sizeof(q)-1)/sizeof(q);
	  goto gc_request;
	}
	buf = (char *)malloc_check(n);
#ifdef DIST
      if (GC_SELF->infile == stdin) {
	  size_t strlen();
          Read(buf); 
	  n = strlen(buf);
       } else
#endif
	 {
	     n = klic_fread(buf, 1, n, GC_SELF->infile);
	 }

	for (k=0; k<n; k++) {
	  if (buf[k] == '\n') { GC_SELF->linecount++; }
	}
	string = convert_binary_c_string_to_klic_string(buf, n, g_allocp);
	if (G_ISREF(string)) {
	  GC_FAIL("internal error: string allocation for fread");
	}
	g_allocp = heapp;
	free(buf);
	GC_UNIFY_VALUE(G_ARG(message,1), string);
	break;
      }
      case functor_linecount_1: {
	CheckInput();
	GC_UNIFY_VALUE(G_ARG(message,0), G_MAKEINT(GC_SELF->linecount));
	break;
      }
	/*** Output Messages ***/
      case functor_putc_1: {
	long c;
	CheckOutput();
	GCSET_MESSAGE_INT_ARG(c, message, 0);
#ifdef DIST
	if (GC_SELF->outfile == stdout) { UserPutc((int)c); }
	else if (GC_SELF->outfile == stderr) { UserEPutc((int)c); }
	else 
#endif
	  {
	      if (klic_putc(c, GC_SELF->outfile) == EOF) {
		  GC_FAIL("putc failed");
	      }
	  }
	break;
      }
      case functor_fwrite_2: {
	extern unsigned char *generic_string_body();
	struct byte_string_object *str;
	int size;
	int written;
	CheckOutput();
	GCSET_MESSAGE_STR_ARG(str, message, 0);
	size = generic_string_size(str);
#ifdef DIST
      if (GC_SELF->outfile == stdout) {
	  UserWrite(generic_string_body(str), size);
	  written = size;
      } else if (GC_SELF->outfile == stderr) {
	  UserEWrite(generic_string_body(str), size);
	  written = size;
      } else
#endif
	{
	    written = klic_fwrite(generic_string_body(str),
				  1, size, GC_SELF->outfile);
	}
	GC_UNIFY_VALUE(G_ARG(message,1), G_MAKEINT(written));
	break;
      }
      case functor_fwrite_1: {
	extern unsigned char *generic_string_body();
	struct byte_string_object *str;
	char *strbody;
	int size;
	int written = 0;
	CheckOutput();
	GCSET_MESSAGE_STR_ARG(str, message, 0);
	strbody = (char *)generic_string_body(str);
	size = generic_string_size(str);
#ifdef DIST
	if (GC_SELF->outfile == stdout) {
	    UserWrite(strbody, size);
	} else if (GC_SELF->outfile == stderr) {
	    UserEWrite(strbody, size);
	} else
#endif
	  {
	      while (size > 0) {
		  written = klic_fwrite(strbody+written, 1,
					size, GC_SELF->outfile);
		  size -= written;
	      }
	  }
	break;
      }
	/*** Common Messages ***/
      case functor_feof_1: {
	long iseof;
#ifdef DIST
	if (GC_SELF->infile == stdin ||
	    GC_SELF->outfile == stdout ||
	    GC_SELF->outfile == stderr) { iseof = 0; }
	else 
#endif
	  {
	    if (GC_SELF->infile != 0) {
	      iseof = feof(GC_SELF->infile);
	    } else {
	      iseof = feof(GC_SELF->outfile);
	    }
	  }
	GC_UNIFY_VALUE(G_ARG(message,0), G_MAKEINT(iseof));
	break;
      }
      case functor_fseek_3: {
	long offset, whence, result;
	GCSET_MESSAGE_INT_ARG(offset, message, 0);
	GCSET_MESSAGE_INT_ARG(whence, message, 1);
#ifdef DIST
      if (GC_SELF->infile == stdin ||
	  (GC_SELF->outfile == stdout || GC_SELF->outfile == stderr)) {
	result = 0;
      } else
#endif
	{
	  FILE *file =
	    (GC_SELF->infile == 0 ? GC_SELF->outfile : GC_SELF->infile);
	  result = fseek(file, offset, whence);
	}
	GC_UNIFY_VALUE(G_ARG(message,2), G_MAKEINT(result));
	break;
      }
      case functor_ftell_1: {
	long result;
#ifdef DIST
	if (GC_SELF->infile == stdin ||
	    (GC_SELF->outfile == stdout || GC_SELF->outfile == stderr)) {
	    result = 0;
	} else
#endif
	  {
	    FILE *file =
	      (GC_SELF->infile == 0 ? GC_SELF->outfile : GC_SELF->infile);
	    result = ftell(file);
	  }
	GC_UNIFY_VALUE(G_ARG(message,0), G_MAKEINT(result));
	break;
      }
      case functor_fflush_1: {
	long result = do_flush(GC_SELF);
	GC_UNIFY_VALUE(G_ARG(message,0), G_MAKEINT(result));
	break;
      }
      case functor_fclose_1: {
	long result;
#ifdef DIST
      if (GC_SELF->infile == stdin ||
	  GC_SELF->outfile == stdout || GC_SELF->outfile == stderr)
	result = 0;
      else
#endif
	{
	  if (GC_SELF->outfile != 0 &&
	      GC_SELF->outfile != stdout &&
	      GC_SELF->outfile != stderr) {
	    result = fclose(GC_SELF->outfile);
	    GC_SELF->outfile = 0;
	  } else {
	    result = 0;
	  }
	  if (result == 0) {
	    if (GC_SELF->infile != 0 &&
		GC_SELF->infile != stdin) {
	      result = fclose(GC_SELF->infile);
	      GC_SELF->infile = 0;
	    }
	  }
	  GC_UNIFY_VALUE(G_ARG(message,0), G_MAKEINT(result));
	  break;
	}
      }
      case functor_sync_1: {
	GC_UNIFY_VALUE(G_ARG(message,0), G_MAKEINT(0));
	break;
      }
      default: goto message_error;
      }
    } else {
      goto message_error;
    }
    GC_TERM = G_CDR_OF(GC_TERM);
    goto top;

  message_error:
    {
      extern unsigned char *generic_string_body();
      char *inname, *outname;
      inname = (char *)generic_string_body(G_FUNCTORP(GC_SELF->inname));
      if (inname == 0) inname = "";
      outname = (char *)generic_string_body(G_FUNCTORP(GC_SELF->outname));
      if (outname == 0) outname = "";
      debug_printf("Illegal message %k to I/O stream for %s%s%s\n",
		   message,
		   inname,
		   ((*inname !=0 && *outname !=0 &&
		     strcmp(inname,outname) != 0) ? "/" : ""),
		   outname);
      GC_FAIL("message error");
    }
  } else if (GC_TERM==NILATOM) {
#ifdef DIST
    if (GC_SELF->outfile == stdout) {
      UserFflush();
    } else if (GC_SELF->outfile == stderr) {
      UserEFflush();
    } else if (GC_SELF->infile == stdin) {
    } else
#endif
      {
	if (GC_SELF->infile != 0 && GC_SELF->infile != stdin) {
	  fclose(GC_SELF->infile);
	}
	if (GC_SELF->outfile != 0 &&
	    GC_SELF->outfile != stdout &&
	    GC_SELF->outfile != stderr) {
	  fclose(GC_SELF->outfile);
	}
      }
    GC_TERMINATE;
  } else if (G_ISREF(GC_TERM)) {
    q temp = G_DEREFONE(GC_TERM);
    if (G_ISREF(temp) &&
	(temp == GC_TERM || G_DEREFONE(temp) == GC_TERM)) {
      reason = GC_TERM;
      GC_SELF->stream = 0;
      goto suspend;
    } else {
      GC_TERM = temp;
      goto top;
    }
  }

 gc_request:
  G_MAKE_VAR(newvar);
  GC_KL1_UNIFY(GC_TERM,newvar);
  GC_RETURN_WITH_HOOK(newvar);

 suspend:
  GC_RETURN_WITH_HOOK(reason);
}

GCDEF_GC()
{
  G_STD_DECL;
  GC_OBJ_TYPE *newself;

  GCSET_NEWOBJ_IN_NEWGEN(newself);
  *newself = *GC_SELF;
  if (GC_SELF->stream != 0) {
    G_COPY_KL1_TERM_TO_NEWGEN(GC_SELF->stream,newself->stream);
  } else {
    newself->stream = 0;
  }
  G_COPY_KL1_TERM_TO_NEWGEN(GC_SELF->inname, newself->inname);
  G_COPY_KL1_TERM_TO_NEWGEN(GC_SELF->outname, newself->outname);
  GC_RETURN_FROM_GC(newself);
}

GCDEF_PRINT()
{
  G_STD_DECL;
  GC_PRINT("$$FILE I/O$");
  GC_RETURN_FROM_PRINT;
}

#define GCUSE_MY_UNIFY
#define GCUSE_MY_PRINT
#define GCUSE_MY_GC

/* define the method table structure of the merger */
#include <klic/gc_methtab.h>

GCDEF_NEW()
{
  GC_STD_DECL_FOR_NEW;
  GC_OBJ_TYPE *newobj;
  q infile, outfile, inpath, outpath;
  q var;
  struct pointer_object *inptr, *outptr;

  /*
    Arguments are:
      0: Input file object
      1: Its name
      2: Output file object
      3: Its name
  */
  if (GC_ARGC != 4) GC_ERROR_IN_NEW("Arity mismatch");

  infile = GC_ARGV[0];
  GC_DEREF_FOR_NEW(infile);
  if (infile != NILATOM &&
      (!G_ISGOBJ(infile) ||
       (struct data_object_method_table *)G_FUNCTOR_OF(infile) !=
       &pointer_g_data_method_table)) {
    GC_FAIL("First argument for file creation is not a pointer object");
  }
  inpath = GC_ARGV[1];
  GC_DEREF_FOR_NEW(inpath);

  outfile = GC_ARGV[2];
  GC_DEREF_FOR_NEW(outfile);
  if (outfile != NILATOM &&
      (!G_ISGOBJ(outfile) ||
       (struct data_object_method_table *)G_FUNCTOR_OF(outfile) !=
       &pointer_g_data_method_table)) {
    GC_FAIL("Third argument for file creation is not a pointer object");
  }
  outpath = GC_ARGV[3];
  GC_DEREF_FOR_NEW(outpath);

  GCSET_NEWOBJ_FOR_NEW(newobj,GC_OBJ_SIZE(newobj));
  if (infile == NILATOM) {
    newobj->infile = 0;
  } else {
    newobj->infile = (FILE *)
      ((struct pointer_object *)G_FUNCTORP(infile))->pointer;
  }
  if (outfile == NILATOM) {
    newobj->outfile = 0;
  } else {
    newobj->outfile = (FILE *)
      ((struct pointer_object *)G_FUNCTORP(outfile))->pointer;
  }
  newobj->linecount = 0;
  newobj->inname = inpath;
  newobj->outname = outpath;
  newobj->stream = 0;
  var = GC_MAKE_HOOK_VAR(newobj);
  GC_RETURN_FROM_NEW(var);
}
