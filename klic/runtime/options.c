/* ---------------------------------------------------------- 
%   (C) 1995 Institute for New Generation Computer Technology 
%       (Read COPYRIGHT for detailed information.) 
%   (C)1996, 1997, 1998, 1999 Japan Information Processing Development Center
%       (Read COPYRIGHT-JIPDEC for detailed information.)
----------------------------------------------------------- */
#include <string.h>
#include <klic/basic.h>
#include <klic/options.h>
static char *
parse_size(str, retp)
long *retp;
char *str;
{
    long n;
    char c[2];
    switch (sscanf(str, "%ld%1s", &n, c)) {
     case 1:
	*retp = n;
	return 0;
     case 2:
	switch (tolower(c[0])) {
	 case 'k':
	    *retp = n << 10;
	    return 0;
	 case 'm':
	    *retp = n << 20;
	    return 0;
	 case 'g':
	    *retp = n << 30;
	    return 0;
	}
    }
    return str;
}
char *
parse_opts(opttbl, acp, avp)
Const struct opttable *opttbl;
int *acp;
char ***avp;
{
    char **argv = *avp;
    char *errp;
    int argc = *acp;
    Const struct opttable *tblp;
    int argc_adv;
    int no_set = 0;		/* -$ option suppress setting variables */
    double atof();
    
    for (argc_adv = 1; argc_adv < argc; ++argc_adv) {
	char *opttop;
	if (argv[argc_adv][0] != '-')
	  break;
	opttop = &argv[argc_adv][1];
	if (opttop[0] == '-' && opttop[1] == '\0') { /* options after -- */
	    ++argc_adv;
	    break;
	} else if (opttop[0] == '$' && opttop[1] == '\0') { /* -$ */
	    no_set = 1;
	    continue;
	}
     parse_succesive_arg:
	if (opttop[0] == '\0')
	  goto proc_next_arg;
	
	for (tblp = opttbl; tblp->opt_str; tblp++) {
	    char *candid_ptr = tblp->opt_str;
	    int optind;
	    
	    if (candid_ptr[0] != opttop[0])
	      continue;
	    optind = strlen(candid_ptr);
	    if (strncmp(opttop+1, candid_ptr+1, optind-1))
	      goto try_next_candidate;
	    
	    else {
		/* We think a option found.
		   Check the descriptor for the argument */
		enum argtype type = ARG_ORIGINAL_TYPE(tblp->opt_type);
		
		if (type == ARG_RESET || type == ARG_SET) {	/* on/off */
		    if (!no_set)
		      tblp->opt_dest->i = (type==ARG_SET);
		    opttop += optind;
		    goto parse_succesive_arg;
		    
		} else if (type == ARG_STRING) {	/* string output */
		    /* string option must be mached exactly */
		    if (opttop[optind] != '\0')
		      goto try_next_candidate;
		    else if (argc_adv + 1 >= argc)
		      /* no argument followed? */
		      goto try_next_candidate;
		    argc_adv++;
		    if (!no_set)
		      tblp->opt_dest->s = argv[argc_adv];
		    goto proc_next_arg;
		    
		} else {			/* float or int case */
		    char *opts_arg;
		    if (opttop[optind] != '\0') /* param follows */
		      opts_arg = &opttop[optind];
		    else if (argc_adv + 1 >= argc) /*no param followed?*/
		      goto try_next_candidate;
		    else {	/* parm is the next arg */
			argc_adv++;
			opts_arg = argv[argc_adv];
		    }
		    
		    if (!no_set) {
			if (type == ARG_SIZE) {		/* size_spec */
			    errp = parse_size(opts_arg, &(tblp->opt_dest->l));
			    if (errp)
			      return errp;
			} else if (type == ARG_DOUBLE) { 	/* floating */
			    if (sscanf(opts_arg,
				       "%le", &(tblp->opt_dest->d)) != 1)
			      return opts_arg;
			} else {	/* this must not happen!! */
#if 0
			    fprintf(stderr, "argtab spec internal error\n");
			    abort();
#endif
			}
		    }
		    goto proc_next_arg;
		}
	    }
	 try_next_candidate:;
	}
	return opttop;
     proc_next_arg:;
    }
    *avp = argv+argc_adv;
    *acp = argc-argc_adv;
    return 0;
}
