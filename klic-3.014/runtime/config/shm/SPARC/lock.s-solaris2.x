	.file	"lock.gcc.c"
.section	".text"
	.align 4
	.global store_barrier
	.type	 store_barrier,#function
	.proc	020
store_barrier:
	!#PROLOGUE# 0
	!#PROLOGUE# 1
	stbar;
	retl
	nop
.LLfe1:
	.size	 store_barrier,.LLfe1-store_barrier
	.align 4
	.global s_lock
	.type	 s_lock,#function
	.proc	020
s_lock:
	!#PROLOGUE# 0
	!#PROLOGUE# 1
	mov %o0,%g3
	        
0:                          
	mov -1,%g2;        
	swap [%g3],%g2;   
	cmp %g2,0;         
	be 3f;              
	nop;                
2:                          
	ld [%g3],%g2;     
	cmp %g2,0;         
	be 0b;              
	nop;                
	b 2b;               
	nop;                
3:                          
    
	retl
	nop
.LLfe2:
	.size	 s_lock,.LLfe2-s_lock
	.align 4
	.global s_unlock
	.type	 s_unlock,#function
	.proc	020
s_unlock:
	!#PROLOGUE# 0
	!#PROLOGUE# 1
	mov %o0,%g3
	        
	stbar;              
	st %g0,[%g3];     
    
	retl
	nop
.LLfe3:
	.size	 s_unlock,.LLfe3-s_unlock
	.ident	"GCC: (GNU) 2.6.0"
