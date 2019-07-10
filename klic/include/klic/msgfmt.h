/* ---------------------------------------------------------- 
%   (C)1995 Institute for New Generation Computer Technology 
%       (Read COPYRIGHT for detailed information.) 
%   (C)1996, 1997, 1998, 1999 Japan Information Processing Development Center
%       (Read COPYRIGHT-JIPDEC for detailed information.)
----------------------------------------------------------- */

struct exref_id {
  long node;
  long id;
  long wec;
};

struct throw_goal_message {
  long wtc;
  long priority;
  long predidate;
  long args[100];
};

struct unify_message {
  long wtc;
  long priority;
  long transfer_count;
  struct exref_id uarg1;
  long arg2[10]
};

struct read_message {
  long wtc;
  long transfer_count;
  struct exref_id read_obj;
  struct exref_id ret_addr;
};

struct answer_value_message {
  long wtc;
  struct exref_id ret_addr;
  long args[100];
};

struct release_message {
  long wtc;
  struct exref_id ret_addr;
};  

struct request_wtc_message {
  long wtc;
  long node;
};

struct supply_wtc_message {
  long wtc;
  long node;
};

struct return_wtc_message {
  long wtc;
};


