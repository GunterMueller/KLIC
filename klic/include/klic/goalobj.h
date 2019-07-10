/* ---------------------------------------------------------- 
%   (C)1995 Takashi Chikayama
%       (Read COPYRIGHT for detailed information.) 
----------------------------------------------------------- */

struct goal_object {
  struct data_object_method_table *method_table;
  struct goalrec *goal;
  unsigned long id;
};
