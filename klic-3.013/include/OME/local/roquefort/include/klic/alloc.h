/* ---------------------------------------------------------- 
%   (C)1993 Institute for New Generation Computer Technology 
%       (Read COPYRIGHT for detailed information.) 
%   (C)1996, 1997, 1998, 1999 Japan Information Processing Development Center
%       (Read COPYRIGHT-JIPDEC for detailed information.)
----------------------------------------------------------- */

q *klic_interrupt();

/* Reverse allocation from "heaptop" */

#define revalloc(s, words, maketype)\
{\
  (s) = maketype(heapbottom -= (words));\
  real_heaplimit -= (words);\
  if (heaplimit && interrupt_off) heaplimit = real_heaplimit;\
  else heaplimit = 0; \
}

#define heapalloc(s, words, maketype)\
  revalloc(s, words, maketype)
