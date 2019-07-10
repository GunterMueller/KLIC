##---------------------------------------------------------- 
##   (C)1993,1994,1995 Institute for New Generation Computer Technology 
##       (Read COPYRIGHT for detailed information.) 
##-----------------------------------------------------------
while (<>) {
    /warning: possible pointer alignment problem/ && next;
    /warning: glbl unused in function/ && next;
    /_ext_interrupt unused in function module_/ && next;
    /_clear_reason unused in function module_/ && next;
    /_\d+_interrupt unused in function module_/ && next;
    /: warning: argument g_method_functor unused in function/ && next;
    /: warning: argument G[GCD]_SELF unused in function / && next;
    /: warning: argument GD_ARGV unused in function / && next;
    /: warning: argument GD_LEVEL unused in function / && next;
    /: warning: argument GC_TERM unused in function / && next;
    /: warning: argument GD_OTHER unused in function / && next;
    /: warning: argument g_myself unused in function / && next;
    /: warning: g_myself unused in function / && next;
    /: warning: argument g_length unused in function / && next;
    /: warning: argument g_depth unused in function / && next;
    /fatal[fp]: variable \# of args./ && next;
    /tracer_error: variable \# of args./ && next;
    /debug_printf: variable \# of args./ && next;
    /: warning: [\w_]+_\d+_\d+ unused in function module_/ && next;
    /: warning: interrupt_\d+ unused in function module_/ && next;
    /: warning: x\d+ set but not used in function module_/ && next;
    / value declared inconsistently	/ && next;
    print;
}
