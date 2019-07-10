.text
	.align 4
	.global _store_barrier
	.proc	020
_store_barrier:
	!#PROLOGUE# 0
	save %sp,-104,%sp
	!#PROLOGUE# 1
L1:
	ret
	restore
	.align 4
	.global _s_lock
	.proc	020
_s_lock:
	!#PROLOGUE# 0
	save %sp,-104,%sp
	!#PROLOGUE# 1
	st %i0,[%fp+68]
	ld [%fp+68],%g3
	        
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
    
L2:
	ret
	restore
	.align 4
	.global _s_unlock
	.proc	020
_s_unlock:
	!#PROLOGUE# 0
	save %sp,-104,%sp
	!#PROLOGUE# 1
	st %i0,[%fp+68]
	ld [%fp+68],%g3
	        
	st %g0,[%g3];     
    
L3:
	ret
	restore
