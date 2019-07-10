#include <klic/basic.h>
char * Const init_atomname[] = {
	(char *)"call",
	(char *)"element",
	(char *)"size",
	(char *)"set_element",
	(char *)"split",
	(char *)"join",
	(char *)"vector",
	(char *)"string",
	(char *)"element_size",
	(char *)"search_character",
	(char *)"less_than",
	(char *)"not_less_than",
	(char *)"estring",
	(char *)"print",
	(char *)"int",
	(char *)"sin",
	(char *)"cos",
	(char *)"tan",
	(char *)"asin",
	(char *)"acos",
	(char *)"atan",
	(char *)"sinh",
	(char *)"cosh",
	(char *)"tanh",
	(char *)"asinh",
	(char *)"acosh",
	(char *)"atanh",
	(char *)"exp",
	(char *)"log",
	(char *)"sqrt",
	(char *)"ceil",
	(char *)"floor",
	(char *)"round",
	(char *)"plus",
	(char *)"minus",
	(char *)"add",
	(char *)"subtract",
	(char *)"multiply",
	(char *)"divide",
	(char *)"pow",
	(char *)"not_greater_than",
	(char *)"greater_than",
	(char *)"equal",
	(char *)"not_equal",
	(char *)"float",
	(char *)"module",
	(char *)"name",
	(char *)"arity",
	(char *)"apply",
	(char *)"predicate",
	(char *)"defined",
	(char *)"reduce",
	(char *)"step",
	(char *)"arg",
	(char *)"setarg",
	(char *)"id",
	(char *)"set_predicate",
	(char *)"normal",
	(char *)"get_atom",
	(char *)"get_atom_name",
	(char *)"intern",
	(char *)"power",
	(char *)"eof",
	(char *)"end_of_file",
	(char *)"[",
	(char *)"{",
	(char *)" (",
	(char *)"(",
	(char *)"-",
	(char *)"]",
	(char *)"never",
	(char *)"}",
	(char *)")",
	(char *)"|",
	(char *)",",
	(char *)"fx",
	(char *)"fy",
	(char *)"xfx",
	(char *)"xfy",
	(char *)"yfx",
	(char *)"xf",
	(char *)"yf",
	(char *)"none",
	(char *)"yes",
	(char *)"no",
	(char *)"abnormal",
	(char *)"variable",
	(char *)"error",
	(char *)"number",
	(char *)"integer",
	(char *)"floating_point",
	(char *)"prefix",
	(char *)"infix",
	(char *)"ops",
	(char *)"postfix",
	(char *)"op",
	(char *)"atom",
	(char *)"list",
	(char *)"functor",
	(char *)"!",
	(char *)";",
	(char *)"void",
	(char *)"escaped_newline",
	(char *)"hexadecimal",
	(char *)"getc",
	(char *)"vtab",
	(char *)"=",
	(char *)"var",
	(char *)"octal",
	(char *)"ungetc",
	(char *)"stderr",
	(char *)"linecount",
	(char *)"putc",
	(char *)"fwrite",
	(char *)"fflush",
	(char *)"s",
	(char *)"open",
	(char *)"unknown",
	(char *)"nl",
	(char *)"&",
	(char *)"?-",
	(char *)"implicit",
	(char *)"local_implicit",
	(char *)"mode",
	(char *)"nospy",
	(char *)"public",
	(char *)"spy",
	(char *)"with_macro",
	(char *)"~",
	(char *)"$~",
	(char *)"\\+",
	(char *)"#",
	(char *)"$$:=",
	(char *)"$$<",
	(char *)"$$<=",
	(char *)"$$=:=",
	(char *)"$$=<",
	(char *)"$$=\\=",
	(char *)"$$>",
	(char *)"$$>=",
	(char *)"$:=",
	(char *)"$<",
	(char *)"$<=",
	(char *)"$=:=",
	(char *)"$=<",
	(char *)"$=\\=",
	(char *)"$>",
	(char *)"$>=",
	(char *)"&<",
	(char *)"&<=",
	(char *)"&=<",
	(char *)"&>",
	(char *)"&>=",
	(char *)"$+=",
	(char *)"$-=",
	(char *)"$*=",
	(char *)"$/=",
	(char *)"*",
	(char *)"**",
	(char *)"+",
	(char *)"-->",
	(char *)"->",
	(char *)"/",
	(char *)"//",
	(char *)"/\\",
	(char *)":",
	(char *)":-",
	(char *)"::",
	(char *)":=",
	(char *)"<",
	(char *)"<<",
	(char *)"<<=",
	(char *)"<=",
	(char *)"<==",
	(char *)"=..",
	(char *)"=:=",
	(char *)"=<",
	(char *)"==",
	(char *)"=>",
	(char *)"=\\=",
	(char *)">",
	(char *)">=",
	(char *)"+=",
	(char *)"-=",
	(char *)"*=",
	(char *)"/=",
	(char *)">>",
	(char *)"@",
	(char *)"@<",
	(char *)"@=<",
	(char *)"@>",
	(char *)"@>=",
	(char *)"\\/",
	(char *)"\\=",
	(char *)"\\==",
	(char *)"^",
	(char *)"is",
	(char *)"mod",
	(char *)"xor",
	(char *)"++",
	(char *)"--",
	(char *)"stdin",
	(char *)"stdout",
	(char *)"read_open",
	(char *)"write_open",
	(char *)"append_open",
	(char *)"update_open",
	(char *)"unix",
	(char *)"string_output",
	(char *)"string_input",
	(char *)"gett",
	(char *)"getwt",
	(char *)"putt",
	(char *)"puttq",
	(char *)"putwt",
	(char *)"putwtq",
	(char *)"addop",
	(char *)"rmop",
	(char *)"on_error",
	(char *)"connect",
	(char *)"bind",
	(char *)"accept",
	(char *)"connect2",
	(char *)"accept2",
	(char *)"sym",
	(char *)"unparser",
	(char *)"unparse",
	(char *)"unwparse",
	(char *)"qtd",
	(char *)"alf",
	(char *)"do",
	(char *)"empty",
	(char *)"put",
	(char *)"get",
	(char *)"get_if_any",
	(char *)"get_all",
	(char *)"carbon_copy",
	(char *)"get_max_if_any",
	(char *)"get_min_if_any",
	(char *)"get_if_any_and_put",
	(char *)"get_and_put_if_any",
	(char *)"get_and_put",
	(char *)"comparator",
	(char *)"sort",
	(char *)"pool",
	(char *)"n",
	(char *)"time",
	(char *)"signal_stream",
	(char *)"pipe",
	(char *)"system",
	(char *)"cd",
	(char *)"unlink",
	(char *)"mktemp",
	(char *)"access",
	(char *)"chmod",
	(char *)"umask",
	(char *)"getenv",
	(char *)"putenv",
	(char *)"kill",
	(char *)"fork",
	(char *)"fork_with_pipes",
	(char *)"inet",
	(char *)"fread",
	(char *)"feof",
	(char *)"sync",
	(char *)"parent",
	(char *)"child",
	(char *)"fseek",
	(char *)"ftell",
	(char *)"fclose",
	(char *)"stdio",
	(char *)"flush",
	(char *)"puts",
};
Const unsigned long initial_atoms_atom_c = 273;
char **atomname_atom_c = (char **)init_atomname;