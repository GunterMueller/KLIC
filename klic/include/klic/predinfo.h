/* ---------------------------------------------------------- 
%   (C)1993, 1994 Institute for New Generation Computer Technology 
%       (Read COPYRIGHT for detailed information.) 
%   (C)1996, 1997, 1998, 1999 Japan Information Processing Development Center
%       (Read COPYRIGHT-JIPDEC for detailed information.)
----------------------------------------------------------- */
struct predinfo {
  Const unsigned char *name;
  Const struct predicate *predstruct;
};

struct modinfo {
  Const unsigned char *name;
  Const struct predinfo * Const *preds;
};
