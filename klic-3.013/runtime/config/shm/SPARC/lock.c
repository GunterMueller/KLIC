void store_barrier()
{
  asm volatile ( "stbar;" : : );
}

void s_lock(lockwdp)
int *lockwdp;
{
    volatile register int* val asm ("g3") = lockwdp;
    asm volatile ( "        
0:                          
	mov -1,%%g2;        
	swap [%%g3],%%g2;   
	cmp %%g2,0;         
	be 3f;              
	nop;                
2:                          
	ld [%%g3],%%g2;     
	cmp %%g2,0;         
	be 0b;              
	nop;                
	b 2b;               
	nop;                
3:                          
    " :  : "g" (val) : "g2" );  
}

void s_unlock(lockwdp)
int *lockwdp;
{
    volatile register int* val asm ("g3") = lockwdp;
    asm volatile ( "        
	stbar;              
	st %%g0,[%%g3];     
    " :  : "g" (val) ) ;        
}
