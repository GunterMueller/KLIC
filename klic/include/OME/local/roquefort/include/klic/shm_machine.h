/* ---------------------------------------------------------- 
%   (C)1994,1995 Institute for New Generation Computer Technology 
%       (Read COPYRIGHT for detailed information.) 
%   (C)1996, 1997, 1998, 1999 Japan Information Processing Development Center
%       (Read COPYRIGHT-JIPDEC for detailed information.)
----------------------------------------------------------- */

  /* lock-unlock */
#ifdef SPARC

#define n_lock(shvp,sadp) ( { \
    register int* s1 asm ("g4") = (int*)shvp; \
    register int* s2 asm ("g3") = (int*)sadp; \
    register struct generator_object* befp asm ("g2");          \
    register struct generator_object* genp asm ("g5") = qSHM_BUSY; \
    asm ( "                   \
0:                            \
        mov %%g5,%%g2;        \
        swap [%%g3 + 4],%%g2; \
        cmp %%g2,%%g5;        \
        bne 4f;               \
        nop;                  \
2:                            \
        ld [%%g4],%%g2;       \
        cmp %%g3,%%g2;        \
        bne 3f;               \
        nop;                  \
        ld [%%g3 + 4],%%g2;   \
        cmp %%g2,%%g5;        \
        bne 0b;               \
        nop;                  \
        b 2b;                 \
        nop;                  \
3:                            \
        mov %%g5,%%g2;        \
4:                            \
" : "=g" (befp), "=g" (s1) : "g" (s1), "g" (s2), "g" (genp)  );    \
  shvp = (q)s1;               \
  befp;                       \
  } )

#define n_unlock(sadp,genh) {     \
    register int* n1 asm ("g3") = (int*)sadp; \
    register struct generator_object* n2 asm ("g4") = genh; \
    asm volatile ( "         \
        stbar;               \
        st %%g4,[%%g3 + 4];  \
" :  : "g" (n1), "g" (n2) ) ;\
}

#define s_lock(key) { \
    register int* val asm ("g3") = lockp[key]; \
    asm volatile ( "         \
0:                           \
        mov -1,%%g2;         \
        swap [%%g3],%%g2;    \
        cmp %%g2,0;          \
        be 3f;               \
        nop;                 \
2:                           \
        ld [%%g3],%%g2;      \
        cmp %%g2,0;          \
        be 0b;               \
        nop;                 \
        b 2b;                \
        nop;                 \
3:                           \
" :  : "g" (val) : "g2" );   \
}

#define s_unlock(key) {      \
    register int* val asm ("g3") = lockp[key]; \
    asm volatile ( "         \
        stbar;               \
        st %%g0,[%%g3];      \
" :  : "g" (val) ) ;         \
}

#define klic_barrier() {  asm volatile ( "stbar;" : : ); }

#endif

#ifdef ALPHA

#define n_lock(shvp,sadp) ( { \
    register long* s1 asm ("$4") = (long*)shvp; \
    register long* s2 asm ("$3") = (long*)sadp; \
    register struct generator_object* genp asm ("$5") = qSHM_BUSY; \
    register struct generator_object* befp asm ("$2");           \
    asm volatile ( "          \
1:                            \
        ldq_l $2,8($3);       \
        cmpeq $2,$5,$1;       \
        bne   $1,2f;          \
        bis   $5,$5,$1;       \
        stq_c $1,8($3);       \
        bne   $1,3f;          \
2:                            \
        ldq   $2,8($3);       \
        cmpeq $2,$5,$1;       \
        beq   $1,1b;          \
        ldq   $1,($4);        \
        cmpeq $3,$1,$1;       \
        beq   $1,3f;          \
        br    2b;             \
3:                            \
        mb;                   \
" : "=g" (befp), "=g" (s1) : "g" (s1), "g" (s2), "g" (genp) : "$1" );    \
  shvp = (q)s1;               \
  befp;                       \
  } )

#define n_unlock(sadp,genh) {     \
    register long* n1 asm ("$3") = (long*)sadp; \
    register struct generator_object* n2 asm ("$4") = genh; \
    asm volatile ( "         \
        mb;                  \
        stq $4,8($3);        \
" :  : "g" (n1), "g" (n2) ) ;\
}

#define s_lock(key) { \
    register int* val asm ("$3") = lockp[key]; \
    asm volatile ( "        \
1:                          \
        ldl_l $1,0($3);     \
        blbs  $1,2f;        \
        or    $1,1,$2;      \
        stl_c $2,0($3);     \
        beq   $2,2f;        \
        br    3f;           \
2:                          \
        ldl   $1,0($3);     \
        blbc  $1,1b;        \
        br    2b;           \
3:                          \
" :  : "g" (val) : "$1","$2" ); \
}

#define s_unlock(key) {     \
    register int* val asm ("$3") = lockp[key]; \
    asm volatile ( "        \
        mb;                 \
        stl $31,0($3);      \
" :  : "g" (val) ) ;        \
}


#define klic_barrier() { asm volatile ( "mb;" : : ); }

#endif

#ifdef INTEL
#define n_lock(shvp, sadp) ({ \
    register int* s1 asm ("%edx") =  (int *) shvp;  \
    register int* s2 asm ("%ecx") =  (int *) sadp;  \
    register struct generator_object* befp asm ("%ebx"); \
    register struct generator_object* genp asm ("%eax") = qSHM_BUSY; \
    asm ( "                     \
0:                              \
        movl  %%eax,%%ebx;      \
        xchgl 4(%%ecx),%%ebx;   \
        cmpl  %%ebx,%%eax;      \
        jne   4f;               \
2:                              \
        movl  (%%edx),%%ebx;    \
        cmpl  %%ecx,%%ebx;      \
        jne   3f;               \
        movl  4(%%ecx),%%ebx;   \
        cmpl  %%ebx,%%eax;      \
        jne   0b;               \
        jmp   2b;               \
3:                              \
        movl  %%eax,%%ebx;      \
4:                              \
" : "=g" (befp), "=g" (s1) : "g" (s1), "g" (s2), "g" (genp)  );    \
  shvp = (q)s1;               \
  befp;                       \
  } )

#define n_unlock(sadp,genh) {     \
    register int* n1 asm ("%ecx") = (int*)sadp; \
    register struct generator_object* n2 asm ("%edx") = genh; \
    asm volatile ( "         \
        movl %%edx,4(%%ecx);  \
" :  : "g" (n1), "g" (n2) ) ;\
}

#define s_lock(key) { \
    register int* val asm ("%ecx") = lockp[key]; \
    asm volatile ( "         \
        movl $0,%%edx;       \
6:                           \
        movl $-1,%%ebx;      \
        xchgl (%%ecx),%%ebx; \
        cmpl %%ebx,%%edx;    \
        je  8f;              \
7:                           \
        movl (%%ecx),%%ebx;  \
        cmpl %%ebx,%%edx;    \
        je  6b;              \
        jmp 7b;              \
8:                           \
" :  : "g" (val) : "%ebx","%edx" );   \
}

#define s_unlock(key) {      \
    register int* val asm ("ecx") = lockp[key]; \
    asm volatile ( "         \
        movl $0,(%%ecx);     \
" :  : "g" (val) ) ;         \
}

#define klic_barrier() {;} 

#endif
