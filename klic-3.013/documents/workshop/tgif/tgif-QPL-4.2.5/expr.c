/*
 * Author:      William Chia-Wei Cheng (bill.cheng@acm.org)
 *
 * Copyright (C) 2001-2009, William Chia-Wei Cheng.
 *
 * This file may be distributed under the terms of the Q Public License
 * as defined by Trolltech AS of Norway and appearing in the file
 * LICENSE.QPL included in the packaging of this file.
 *
 * THIS FILE IS PROVIDED AS IS WITH NO WARRANTY OF ANY KIND, INCLUDING
 * THE WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL,
 * INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING
 * FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT,
 * NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION
 * WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/expr.c,v 1.6 2011/05/16 16:21:57 william Exp $
 */

#define _INCLUDE_FROM_EXPR_C_

#include "tgifdefs.h"

#include "dialog.e"
#include "expr.e"
#include "msg.e"
#include "setup.e"
#include "strtbl.e"
#include "util.e"

#define OP_BEGIN 0
#define OP_IFTHEN 1
#define OP_ELSE 2
#define OP_OR 3
#define OP_AND 4
#define OP_BIT_OR 5
#define OP_BIT_XOR 6
#define OP_BIT_AND 7
#define OP_EQ 8
#define OP_NE 9
#define OP_GT 10
#define OP_LT 11
#define OP_GE 12
#define OP_LE 13
#define OP_SHIFT_LEFT 14
#define OP_SHIFT_RIGHT 15
#define OP_ADD 16
#define OP_SUB 17
#define OP_MUL 18
#define OP_DIV 19
#define OP_INTDIV 20
#define OP_MOD 21
#define OP_NOT 22
#define OP_BIT_NOT 23
#define OP_CLOSE 24
#define OP_OPEN 25
#define OP_END 26

struct OpRec {
   int op_code;
   int prec;
   struct OpRec *next;
};

struct ValRec {
   struct VRec v;
   struct ValRec *next;
};

static char *pszCurExpr=NULL;
static char *pszExpr=NULL;
static struct OpRec *topOpStk=NULL;
static struct ValRec *topValStk=NULL;

#ifdef NOT_DEFINED
#ifdef _TGIF_DBG
static int gnExprDebug=FALSE;
#endif /* _TGIF_DBG */
#endif /* NOT_DEFINED */

static
int BadOperandType(operator_str)
   char *operator_str;
{
   sprintf(gszMsgBox, TgLoadString(STID_CANNOT_EVAL_INVALID_OP_TYPE),
         pszCurExpr, operator_str);
   MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
   return FALSE;
}

static
int DivByZero(operator_str)
   char *operator_str;
{
   sprintf(gszMsgBox, TgLoadString(STID_DIVIDE_BY_ZERO_FOR_OP_TYPE),
         operator_str);
   MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
   return FALSE;
}

static
int OpPrec(op_code)
   int op_code;
{
   switch (op_code) {
   case OP_BEGIN: return INVALID;
   case OP_IFTHEN: return 1; /* '?' */
   case OP_ELSE: return 2; /* ':' */
   case OP_OR: return 3;  /* || */
   case OP_AND: return 4; /* && */
   case OP_BIT_OR: return 5; /* | */
   case OP_BIT_XOR: return 5; /* ^ */
   case OP_BIT_AND: return 5; /* & */
   case OP_EQ: return 6; /* == */
   case OP_NE: return 6; /* != */
   case OP_GT: return 7; /* > */
   case OP_LT: return 7; /* < */
   case OP_GE: return 7; /* >= */
   case OP_LE: return 7; /* <= */
   case OP_SHIFT_LEFT: return 8; /* << */
   case OP_SHIFT_RIGHT: return 8; /* >> */
   case OP_ADD: return 9; /* '+' */
   case OP_SUB: return 9; /* '-' */
   case OP_MUL: return 10; /* '*' */
   case OP_DIV: return 10; /* '/' */
   case OP_INTDIV: return 10; /* // */
   case OP_MOD: return 10; /* % */
   case OP_NOT: return 11; /* '!' */
   case OP_BIT_NOT: return 11; /* ~ */
   case OP_CLOSE: return 12; /* ')' */
   case OP_OPEN: return 13; /* '(' */
   case OP_END: return INVALID;
   }
   return 99;
}

#define MAX_OP_PREC 14

static
int OpArity(op_code)
   int op_code;
{
   switch (op_code) {
   case OP_BEGIN: return INVALID;
   case OP_IFTHEN: return 3; /* '?' */
   case OP_ELSE: return 0; /* ':' */
   case OP_OR: return 2;  /* || */
   case OP_AND: return 2; /* && */
   case OP_BIT_OR: return 2; /* | */
   case OP_BIT_XOR: return 2; /* ^ */
   case OP_BIT_AND: return 2; /* & */
   case OP_EQ: return 2; /* == */
   case OP_NE: return 2; /* != */
   case OP_GT: return 2; /* > */
   case OP_LT: return 2; /* < */
   case OP_GE: return 2; /* >= */
   case OP_LE: return 2; /* <= */
   case OP_SHIFT_LEFT: return 2; /* << */
   case OP_SHIFT_RIGHT: return 2; /* >> */
   case OP_ADD: return 2; /* '+' */
   case OP_SUB: return 2; /* '-' */
   case OP_MUL: return 2; /* '*' */
   case OP_DIV: return 2; /* '/' */
   case OP_INTDIV: return 2; /* // */
   case OP_MOD: return 2; /* % */
   case OP_NOT: return 1; /* '!' */
   case OP_BIT_NOT: return 1; /* '~' */
   case OP_CLOSE: return 0; /* ')' */
   case OP_OPEN: return 0; /* '(' */
   case OP_END: return INVALID;
   }
   sprintf(gszMsgBox, TgLoadString(STID_ILLEGAL_EXPR_INVALID_OP_CODE),
         pszCurExpr, op_code);
   MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
   return INVALID;
}

static
int OpPush(op_code, cur_depth)
   int op_code;
   int cur_depth;
{
   struct OpRec *op_ptr=(struct OpRec *)malloc(sizeof(struct OpRec));
   
   if (op_ptr == NULL) return FailAllocMessage();
   memset(op_ptr, 0, sizeof(struct OpRec));
   op_ptr->op_code = op_code;
   op_ptr->prec = OpPrec(op_code)+cur_depth;
   op_ptr->next = topOpStk;
   topOpStk = op_ptr;
   return TRUE;
}

static
int ValPush(v_ptr)
   struct VRec *v_ptr;
{
   struct ValRec *val_ptr=(struct ValRec *)malloc(sizeof(struct ValRec));
   
   if (val_ptr == NULL) return FailAllocMessage();
   memset(val_ptr, 0, sizeof(struct ValRec));
   memcpy(&val_ptr->v, v_ptr, sizeof(val_ptr->v));
   val_ptr->next = topValStk;
   topValStk = val_ptr;
   return TRUE;
}

static
int PopOpStk()
{
   int op_code=topOpStk->op_code;
   struct OpRec *op_ptr=topOpStk;
   
   topOpStk = topOpStk->next;
   free(op_ptr);
   return op_code;
}

static
void PopValStk(v_ptr)
   struct VRec *v_ptr;
{
   struct ValRec *val_ptr=topValStk;
   
   topValStk = topValStk->next;
   if (v_ptr != NULL) {
      v_ptr->vtype = val_ptr->v.vtype;
      if (v_ptr->vtype == INT_VAL) {
         v_ptr->val.i = val_ptr->v.val.i;
      } else {
         v_ptr->val.d = val_ptr->v.val.d;
      }
   }
   free(val_ptr);
}

void CleanUpExpr()
{
   if (pszCurExpr != NULL) { free(pszCurExpr); pszCurExpr = NULL; }
   if (pszExpr != NULL) { free(pszExpr); pszExpr = NULL; }
   while (topOpStk != NULL) PopOpStk();
   while (topValStk != NULL) PopValStk(NULL);
}

int ExprAtomType(expr)
   char *expr;
{
   char *dup_str, *c_ptr;
   int rc=INVALID;

   UtilTrimBlanks(expr);
   if (*expr == '\0') return NULL_VAL;

   dup_str = UtilStrDup(expr);
   if (dup_str == NULL) { FailAllocMessage(); return INVALID; }

   c_ptr = dup_str;
   if (*c_ptr == '-') c_ptr++;
   /* do not translate -- program constants */
   if (strtok(c_ptr, "0123456789") == NULL) rc = INT_VAL;
   free(dup_str);
   if (rc != INVALID) return rc;

   dup_str = UtilStrDup(expr);
   if (dup_str == NULL) { FailAllocMessage(); return INVALID; }

   c_ptr = dup_str;
   if (*c_ptr == '-') c_ptr++;
   /* do not translate -- program constants */
   rc = (strtok(c_ptr, ".0123456789") == NULL ? DBL_VAL : STR_VAL);
   free(dup_str);
   return rc;
}

static
int EvalEquality(op_code, v, result_ptr)
   int op_code;
   struct VRec *v, *result_ptr;
{
   int equal=TRUE;

   if (!(((v[1].vtype == INT_VAL || v[1].vtype == DBL_VAL) &&
         (v[0].vtype == INT_VAL || v[0].vtype == DBL_VAL)) ||
         ((v[1].vtype == NULL_VAL || v[1].vtype == STR_VAL) &&
         (v[0].vtype == NULL_VAL || v[0].vtype == STR_VAL)))) {
      result_ptr->vtype = INT_VAL;
      switch (op_code) {
      case OP_EQ: result_ptr->val.i = FALSE; break;
      case OP_NE: result_ptr->val.i = TRUE; break;
      }
      return TRUE;
   }
   switch (v[1].vtype) {
   case INT_VAL:
      switch (v[0].vtype) {
      case INT_VAL: equal = (v[1].val.i == v[0].val.i); break;
      case DBL_VAL:
         equal = (fabs(((double)v[1].val.i)-v[0].val.d) < EQ_TOL);
         break;
      }
      break;
   case DBL_VAL:
      switch (v[0].vtype) {
      case INT_VAL:
         equal = (fabs(v[1].val.d-((double)v[0].val.i)) < EQ_TOL);
         break;
      case DBL_VAL:
         equal = (fabs(v[1].val.d-v[0].val.d) < EQ_TOL);
         break;
      }
      break;
   case NULL_VAL:
      switch (v[0].vtype) {
      case NULL_VAL: equal = TRUE; break;
      case STR_VAL: equal = (*v[0].val.s == '\0'); break;
      }
   case STR_VAL:
      switch (v[0].vtype) {
      case NULL_VAL: equal = (*v[1].val.s == '\0'); break;
      case STR_VAL:
         equal = (strcmp(v[1].val.s, v[0].val.s) == 0);
         break;
      }
      break;
   }
   result_ptr->vtype = INT_VAL;
   switch (op_code) {
   case OP_EQ: result_ptr->val.i = equal; break;
   case OP_NE: result_ptr->val.i = (!equal); break;
   }
   return TRUE;
}

static
int EvalInequality(op_code, v, result_ptr)
   int op_code;
   struct VRec *v, *result_ptr;
{
   int equal=TRUE, greater=FALSE;

   if (!(((v[1].vtype == INT_VAL || v[1].vtype == DBL_VAL) &&
         (v[0].vtype == INT_VAL || v[0].vtype == DBL_VAL)) ||
         ((v[1].vtype == NULL_VAL || v[1].vtype == STR_VAL) &&
         (v[0].vtype == NULL_VAL || v[0].vtype == STR_VAL)))) {
      return FALSE;
   }
   switch (v[1].vtype) {
   case INT_VAL:
      switch (v[0].vtype) {
      case INT_VAL:
         equal = (v[1].val.i == v[0].val.i);
         if (!equal) {
            greater = (v[1].val.i > v[0].val.i);
         }
         break;
      case DBL_VAL:
         equal = (fabs(((double)v[1].val.i)-v[0].val.d) < EQ_TOL);
         if (!equal) {
            greater = (((double)v[1].val.i) > v[0].val.d);
         }
         break;
      }
      break;
   case DBL_VAL:
      switch (v[0].vtype) {
      case INT_VAL:
         equal = (fabs(v[1].val.d-((double)v[0].val.i)) < EQ_TOL);
         if (!equal) {
            greater = (v[1].val.d > ((double)v[0].val.i));
         }
         break;
      case DBL_VAL:
         equal = (fabs(v[1].val.d-v[0].val.d) < EQ_TOL);
         if (!equal) {
            greater = (v[1].val.d > v[0].val.d);
         }
         break;
      }
      break;
   case NULL_VAL:
      switch (v[0].vtype) {
      case NULL_VAL:
         equal = TRUE;
         break;
      case STR_VAL:
         equal = (*v[0].val.s == '\0');
         if (!equal) {
            greater = FALSE;
         }
         break;
      }
   case STR_VAL:
      switch (v[0].vtype) {
      case NULL_VAL:
         equal = (*v[1].val.s == '\0');
         if (!equal) {
            greater = TRUE;
         }
         break;
      case STR_VAL:
         equal = strcmp(v[1].val.s, v[0].val.s);
         if (equal != 0) {
            greater = (equal > 0);
            equal = FALSE;
         } else {
            equal = TRUE;
         }
         break;
      }
      break;
   }
   result_ptr->vtype = INT_VAL;
   switch (op_code) {
   case OP_GT: result_ptr->val.i = ((!equal) && greater); break;
   case OP_LT: result_ptr->val.i = ((!equal) && (!greater)); break;
   case OP_GE: result_ptr->val.i = (equal || greater); break;
   case OP_LE: result_ptr->val.i = (equal || (!greater)); break;
   }
   return TRUE;
}

static
int EvalMod(left, right)
   int left, right;
{
   return (left-(right*((int)(left/right))));
}

static
int MultipleOf(left, right)
   double left, right;
{
   double dmod=(left-(right*((int)(left/right))));

   if (fabs(dmod) < EQ_TOL) return TRUE;
   return FALSE;
}

static
int EvalArithmaticOp(op_code, op_str, v, result_ptr)
   int op_code;
   char *op_str;
   struct VRec *v, *result_ptr;
{
   int left_ival, right_ival;
   double left_dval, right_dval;

   if (!((v[1].vtype == INT_VAL || v[1].vtype == DBL_VAL) &&
         (v[0].vtype == INT_VAL || v[0].vtype == DBL_VAL))) {
      return BadOperandType(op_str);
   }
   left_ival = right_ival = 0;
   left_dval = right_dval = (double)0.0;
   switch (v[1].vtype) {
   case INT_VAL:
      switch (v[0].vtype) {
      case INT_VAL:
         switch (op_code) {
         case OP_ADD:
            result_ptr->vtype = INT_VAL;
            result_ptr->val.i = v[1].val.i + v[0].val.i;
            break;
         case OP_SUB:
            result_ptr->vtype = INT_VAL;
            result_ptr->val.i = v[1].val.i - v[0].val.i;
            break;
         case OP_MUL:
            result_ptr->vtype = INT_VAL;
            result_ptr->val.i = v[1].val.i * v[0].val.i;
            break;
         case OP_DIV:
            if (v[0].val.i == 0) { DivByZero(op_str); return FALSE; }
            if (EvalMod(v[1].val.i, v[0].val.i) == 0) {
               result_ptr->vtype = INT_VAL;
               result_ptr->val.i = v[1].val.i / v[0].val.i;
            } else {
               result_ptr->vtype = DBL_VAL;
               result_ptr->val.d = ((double)v[1].val.i) / ((double)v[0].val.i);
            }
            break;
         case OP_INTDIV:
            if (v[0].val.i == 0) { DivByZero(op_str); return FALSE; }
            result_ptr->vtype = INT_VAL;
            result_ptr->val.i = v[1].val.i / v[0].val.i;
            break;
         case OP_MOD:
            if (v[0].val.i == 0) { DivByZero(op_str); return FALSE; }
            result_ptr->vtype = INT_VAL;
            result_ptr->val.i = EvalMod(v[1].val.i, v[0].val.i);
            break;
         }
         break;
      case DBL_VAL:
         /* int op double */
         switch (op_code) {
         case OP_ADD:
            result_ptr->vtype = DBL_VAL;
            result_ptr->val.d = ((double)v[1].val.i) + v[0].val.d;
            break;
         case OP_SUB:
            result_ptr->vtype = DBL_VAL;
            result_ptr->val.d = ((double)v[1].val.i) - v[0].val.d;
            break;
         case OP_MUL:
            result_ptr->vtype = DBL_VAL;
            result_ptr->val.d = ((double)v[1].val.i) * v[0].val.d;
            break;
         case OP_DIV:
            if (fabs(v[0].val.d) < EQ_TOL) { DivByZero(op_str); return FALSE; }
            result_ptr->vtype = DBL_VAL;
            result_ptr->val.d = ((double)v[1].val.i) / v[0].val.d;
            break;
         case OP_INTDIV:
            if (fabs(v[0].val.d) < EQ_TOL) { DivByZero(op_str); return FALSE; }
            result_ptr->vtype = INT_VAL;
            result_ptr->val.i = (int)(((double)v[1].val.i) / v[0].val.d);
            break;
         case OP_MOD:
            return BadOperandType(op_str);
         }
         break;
      }
      break;
   case DBL_VAL:
      switch (v[0].vtype) {
      case INT_VAL:
         /* double op int */
         switch (op_code) {
         case OP_ADD:
            result_ptr->vtype = DBL_VAL;
            result_ptr->val.d = v[1].val.d + ((double)v[0].val.i);
            break;
         case OP_SUB:
            result_ptr->vtype = DBL_VAL;
            result_ptr->val.d = v[1].val.d - ((double)v[0].val.i);
            break;
         case OP_MUL:
            result_ptr->vtype = DBL_VAL;
            result_ptr->val.d = v[1].val.d * ((double)v[0].val.i);
            break;
         case OP_DIV:
            if (v[0].val.i == 0) { DivByZero(op_str); return FALSE; }
            result_ptr->vtype = DBL_VAL;
            result_ptr->val.d = v[1].val.d / ((double)v[0].val.i);
            break;
         case OP_INTDIV:
            if (v[0].val.i == 0) { DivByZero(op_str); return FALSE; }
            result_ptr->vtype = INT_VAL;
            result_ptr->val.i = (int)(v[1].val.d / ((double)v[0].val.i));
            break;
         case OP_MOD:
            return BadOperandType(op_str);
         }
         break;
      case DBL_VAL:
         switch (op_code) {
         case OP_ADD:
            result_ptr->vtype = DBL_VAL;
            result_ptr->val.d = v[1].val.d + v[0].val.d;
            break;
         case OP_SUB:
            result_ptr->vtype = DBL_VAL;
            result_ptr->val.d = v[1].val.d - v[0].val.d;
            break;
         case OP_MUL:
            result_ptr->vtype = DBL_VAL;
            result_ptr->val.d = v[1].val.d * v[0].val.d;
            break;
         case OP_DIV:
            if (fabs(v[0].val.d) < EQ_TOL) { DivByZero(op_str); return FALSE; }
            if (MultipleOf(v[1].val.d, v[0].val.d)) {
               result_ptr->vtype = INT_VAL;
               result_ptr->val.i = round(v[1].val.d / v[0].val.d);
            } else {
               result_ptr->vtype = DBL_VAL;
               result_ptr->val.d = v[1].val.d / v[0].val.d;
            }
            break;
         case OP_INTDIV:
            if (fabs(v[0].val.d) < EQ_TOL) { DivByZero(op_str); return FALSE; }
            result_ptr->vtype = INT_VAL;
            result_ptr->val.i = (int)(v[1].val.d / v[0].val.d);
            break;
         case OP_MOD:
            return BadOperandType(op_str);
         }
         break;
      }
      break;
   }
   return TRUE;
}

static
int Operate()
{
   register int i;
   int arity, op_code;
   struct VRec v[3], result;
   
   if (topOpStk == NULL) {
      sprintf(gszMsgBox, TgLoadString(STID_ILLEGAL_EXPR_EMPTY_OP_STACK),
            pszCurExpr);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return FALSE;
   }
   op_code = PopOpStk();
   arity = OpArity(op_code);
   if (arity == INVALID) return TRUE;

   for (i=0; i < 3; i++) memset(&v[i], 0, sizeof(struct VRec));
   memset(&result, 0, sizeof(struct VRec));

   for (i=0; i < arity; i++) {
      if (topValStk == NULL) {
         sprintf(gszMsgBox, TgLoadString(STID_ILLEGAL_EXPR_EMPTY_OP_STACK),
               pszCurExpr);
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
         return FALSE;
      }
      PopValStk(&v[i]);
   }
   /* do not translate -- program constants */
   switch (op_code) {
   case OP_IFTHEN:
      if (v[2].vtype != INT_VAL) return BadOperandType("?");
      result.vtype = (v[2].val.i ? v[1].vtype : v[0].vtype);
      switch (result.vtype) {
      case NULL_VAL: return BadOperandType("?");
      case INT_VAL:
         result.val.i = (v[2].val.i ? v[1].val.i : v[0].val.i);
         break;
      case DBL_VAL:
         result.val.d = (v[2].val.d ? v[1].val.d : v[0].val.d);
         break;
      case STR_VAL: return BadOperandType("?");
      }
      break;
   case OP_ELSE: break;
   case OP_OR:
      if (v[1].vtype != INT_VAL || v[0].vtype != INT_VAL) {
         return BadOperandType("||");
      }
      result.vtype = INT_VAL;
      result.val.i = (v[1].val.i || v[0].val.i);
      break;
   case OP_AND:
      if (v[1].vtype != INT_VAL || v[0].vtype != INT_VAL) {
         return BadOperandType("&&");
      }
      result.vtype = INT_VAL;
      result.val.i = (v[1].val.i && v[0].val.i);
      break;
   case OP_BIT_OR:
      if (v[1].vtype != INT_VAL || v[0].vtype != INT_VAL) {
         return BadOperandType("|");
      }
      result.vtype = INT_VAL;
      result.val.i = (v[1].val.i | v[0].val.i);
      break;
   case OP_BIT_XOR:
      if (v[1].vtype != INT_VAL || v[0].vtype != INT_VAL) {
         return BadOperandType("^");
      }
      result.vtype = INT_VAL;
      result.val.i = (v[1].val.i ^ v[0].val.i);
      break;
   case OP_BIT_AND:
      if (v[1].vtype != INT_VAL || v[0].vtype != INT_VAL) {
         return BadOperandType("&");
      }
      result.vtype = INT_VAL;
      result.val.i = (v[1].val.i & v[0].val.i);
      break;
   case OP_EQ:
      if (!EvalEquality(OP_EQ, v, &result)) {
         return BadOperandType("==");
      }
      break;
   case OP_NE:
      if (!EvalEquality(OP_NE, v, &result)) {
         return BadOperandType("!=");
      }
      break;
   case OP_GT:
      if (!EvalInequality(OP_GT, v, &result)) {
         return BadOperandType(">");
      }
      break;
   case OP_LT:
      if (!EvalInequality(OP_LT, v, &result)) {
         return BadOperandType("<");
      }
      break;
   case OP_GE:
      if (!EvalInequality(OP_GE, v, &result)) {
         return BadOperandType(">=");
      }
      break;
   case OP_LE:
      if (!EvalInequality(OP_LE, v, &result)) {
         return BadOperandType("<=");
      }
      break;
   case OP_SHIFT_LEFT:
      if (v[1].vtype != INT_VAL || v[0].vtype != INT_VAL) {
         return BadOperandType("<<");
      }
      result.vtype = INT_VAL;
      result.val.i = (v[1].val.i << v[0].val.i);
      break;
   case OP_SHIFT_RIGHT:
      if (v[1].vtype != INT_VAL || v[0].vtype != INT_VAL) {
         return BadOperandType(">>");
      }
      result.vtype = INT_VAL;
      result.val.i = (v[1].val.i >> v[0].val.i);
      break;
   case OP_ADD:
      if (!EvalArithmaticOp(OP_ADD, "+", v, &result)) {
         return FALSE;
      }
      break;
   case OP_SUB:
      if (!EvalArithmaticOp(OP_SUB, "-", v, &result)) {
         return FALSE;
      }
      break;
   case OP_MUL:
      if (!EvalArithmaticOp(OP_MUL, "*", v, &result)) {
         return FALSE;
      }
      break;
   case OP_DIV:
      if (!EvalArithmaticOp(OP_DIV, "/", v, &result)) {
         return FALSE;
      }
      break;
   case OP_INTDIV:
      if (!EvalArithmaticOp(OP_INTDIV, "//", v, &result)) {
         return FALSE;
      }
      break;
   case OP_MOD:
      if (!EvalArithmaticOp(OP_MOD, "%", v, &result)) {
         return FALSE;
      }
      break;
   case OP_NOT:
      if (v[0].vtype != INT_VAL) {
         return BadOperandType("!");
      }
      result.vtype = INT_VAL;
      result.val.i = (v[0].val.i == 0);
      break;
   case OP_BIT_NOT:
      if (v[0].vtype != INT_VAL) {
         return BadOperandType("~");
      }
      result.vtype = INT_VAL;
      result.val.i = (~v[0].val.i);
      break;
   case OP_CLOSE:
   case OP_OPEN:
      break;
   }
   for (i=0; i < arity; i++) {
      if (v[i].vtype == STR_VAL && v[i].val.s != NULL) {
         free(v[i].val.s);
         v[i].val.s = NULL;
      }
   }
   return ValPush(&result);
}

static char szNumericChars[]=".0123456789";

static
int ParseForNumericVal(ppsz_token, v)
   char **ppsz_token;
   struct VRec *v;
{
   int seen_dot=FALSE;
   char saved_ch, *token=(*ppsz_token);
   
   for ( ; **ppsz_token != '\0'; (*ppsz_token)++) {
      char *c_ptr=strchr(szNumericChars, **ppsz_token);
      
      if (c_ptr == NULL) break;
      if (c_ptr == szNumericChars) {
         if (seen_dot) break;
         seen_dot = TRUE;
      }
   }
   memset(v, 0, sizeof(struct VRec));
   saved_ch = **ppsz_token;
   **ppsz_token = '\0';
   if (seen_dot) {
      v->vtype = DBL_VAL;
      if (sscanf(token, "%lf", &(v->val.d)) != 1) {
         **ppsz_token = saved_ch;
         sprintf(gszMsgBox, TgLoadString(STID_ILLEGAL_EXPR_BAD_NUM_VALUE),
               pszCurExpr, token);
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
         return FALSE;
      }
   } else {
      v->vtype = INT_VAL;
      if (sscanf(token, "%d", &(v->val.i)) != 1) {
         **ppsz_token = saved_ch;
         sprintf(gszMsgBox, TgLoadString(STID_ILLEGAL_EXPR_BAD_NUM_VALUE),
               pszCurExpr, token);
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
         return FALSE;
      }
   }
   **ppsz_token = saved_ch;
   return TRUE;
}

static
int ParseForStringVal(ppsz_token, v)
   char **ppsz_token;
   struct VRec *v;
{
   char *token=(*ppsz_token);
   
   for ((*ppsz_token)++; **ppsz_token != '\0'; (*ppsz_token)++) {
      if (**ppsz_token == '\\') {
         (*ppsz_token)++;
      } else if (**ppsz_token == '"') {
         break;
      }
   }
   if (**ppsz_token == '\0') {
      sprintf(gszMsgBox, TgLoadString(STID_ILLEGAL_EXPR_BAD_STR_VALUE),
            pszCurExpr, token);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return FALSE;
   }
   memset(v, 0, sizeof(struct VRec));
   **ppsz_token = '\0';
   v->vtype = STR_VAL;
   v->val.s = UtilStrDup(&token[1]);
   **ppsz_token = '"';
   (*ppsz_token)++;
   if (v->val.s == NULL) {
      sprintf(gszMsgBox, TgLoadString(STID_ILLEGAL_EXPR_BAD_STR_VALUE),
            pszCurExpr, token);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return FALSE;
   }
   return TRUE;
}

static
int ParseForIDVal(ppsz_token, v)
   char **ppsz_token;
   struct VRec *v;
{
   char *token=(*ppsz_token), saved_ch;
   
   for ( ; **ppsz_token != '\0'; (*ppsz_token)++) {
      if (!(**ppsz_token == '_' ||
            (**ppsz_token >= 'a' && **ppsz_token <= 'z') ||
            (**ppsz_token >= 'A' && **ppsz_token <= 'Z') ||
            (**ppsz_token >= '0' && **ppsz_token <= '9'))) {
         break;
      }
   }
   memset(v, 0, sizeof(struct VRec));
   saved_ch = **ppsz_token;
   **ppsz_token = '\0';
   v->vtype = STR_VAL;
   v->val.s = UtilStrDup(token);
   **ppsz_token = saved_ch;
   if (v->val.s == NULL) {
      sprintf(gszMsgBox, TgLoadString(STID_ILLEGAL_EXPR_BAD_IDENTIFIER),
            pszCurExpr, token);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return FALSE;
   }
   return TRUE;
}

#ifdef NOT_DEFINED
#ifdef _TGIF_DBG /* debug, do not translate */
static char *opName[] = {
   "BEGIN", "?", ":", "||", "&&", "|", "^", "&", "==", "!=", ">", "<", ">=",
   "<=", "<<", ">>", "+", "-", "*", "/", "//", "%", "!", "~", ")", "(", "END",
   NULL
};
#define MAXOP 27

static
void DebugOpStk()
{
   struct OpRec *op_ptr=NULL;
   
   if (gnExprDebug) { }
   fprintf(stderr, "top of op stack -->\n");
   for (op_ptr=topOpStk; op_ptr != NULL; op_ptr=op_ptr->next) {
      fprintf(stderr, "                   | %2s, %2d |\n",
            opName[op_ptr->op_code], OpPrec(op_ptr->op_code));
   }
   fprintf(stderr, "                   |________|\n");
}
#endif /* _TGIF_DBG */

#ifdef _TGIF_DBG /* debug, do not translate */
static
void DebugValStk()
{
   struct ValRec *v_ptr=NULL;
   
   if (gnExprDebug) { }
   fprintf(stderr, "top of val stack ->\n");
   for (v_ptr=topValStk; v_ptr != NULL; v_ptr=v_ptr->next) {
      switch (v_ptr->v.vtype) {
      case NULL_VAL:
         fprintf(stderr, "                   | NULL\n");
         break;
      case INT_VAL:
         fprintf(stderr, "                   |  INT, %1d\n",
               v_ptr->v.val.i);
         break;
      case DBL_VAL:
         fprintf(stderr, "                   |  DBL, %.3f\n",
               (float)(v_ptr->v.val.d));
         break;
      case STR_VAL:
         fprintf(stderr, "                   |  STR, \"%s\"\n",
               (v_ptr->v.val.s==NULL ? "NULL" : v_ptr->v.val.s));
         break;
      }
   }
   fprintf(stderr, "                   |________...\n");
}
#endif /* _TGIF_DBG */
#endif /* NOT_DEFINED */

static
int ProcessOp(prec)
   int prec;
{
   while (topOpStk != NULL && prec <= topOpStk->prec) {
      if (!Operate()) {
         return FALSE;
      }
   }
   return TRUE;
}

int EvalExpr(orig_expr, v_ptr)
   char *orig_expr;
   struct VRec *v_ptr;
{
   int cur_depth=0, ok=TRUE, prev_token_is_op=TRUE;
   char *token;

   CleanUpExpr();
   pszCurExpr = UtilStrDup(orig_expr);
   if (pszCurExpr == NULL) return FailAllocMessage();
   UtilTrimBlanks(pszCurExpr);
   if (*pszCurExpr == '\0') {
      v_ptr->vtype = NULL_VAL;
      return TRUE;
   }
   memset(v_ptr, 0, sizeof(struct VRec));
   pszExpr = UtilStrDup(pszCurExpr);
   if (pszExpr == NULL) return FailAllocMessage();
   if (!OpPush(OP_BEGIN, cur_depth)) return FALSE;
   token = pszExpr;
   while (ok && *token != '\0') {
      struct VRec v;
      int op_code=INVALID;

      switch (*token) {
      case ' ':
      case '\t':
      case '\n':
      case '\r':
         token++;
         break;
      case '-':
         if (!prev_token_is_op || strchr(szNumericChars, token[1]) == NULL) {
            op_code = OP_SUB;
            token++;
         } else {
            char *new_token=(&token[1]);

            if (ParseForNumericVal(&new_token, &v)) {
               if (v.vtype == INT_VAL) {
                  v.val.i = (-v.val.i);
               } else {
                  v.val.d = (-v.val.d);
               }
               if (!ValPush(&v)) {
                  ok = FALSE;
               } else {
                  prev_token_is_op = FALSE;
               }
            } else {
               ok = FALSE;
            }
            token = new_token;
         }
         break;
      case '.':
      case '0':
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9':
         if (ParseForNumericVal(&token, &v)) {
            if (!ValPush(&v)) {
               ok = FALSE;
            } else {
               prev_token_is_op = FALSE;
            }
         } else {
            ok = FALSE;
         }
         break;
      case '"':
         if (ParseForStringVal(&token, &v)) {
            if (!ValPush(&v)) {
               ok = FALSE;
            } else {
               prev_token_is_op = FALSE;
            }
         } else {
            ok = FALSE;
         }
         break;
      case '?': op_code = OP_IFTHEN; token++; break;
      case ':': op_code = OP_ELSE; token++; break;
      case '|':
         if (token[1] == '|') {
            op_code = OP_BIT_OR;
            token = &token[2];
         } else {
            op_code = OP_OR;
            token++;
         }
         break;
      case '^': op_code = OP_BIT_XOR; token++; break;
      case '&':
         if (token[1] == '&') {
            op_code = OP_BIT_AND;
            token = &token[2];
         } else {
            op_code = OP_AND;
            token++;
         }
         break;
      case '=':
         if (token[1] == '=') {
            op_code = OP_EQ;
            token = &token[2];
         } else {
            sprintf(gszMsgBox, TgLoadString(STID_ILLEGAL_EXPR_BAD_OPERATOR),
                  pszCurExpr, "=");
            MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
            ok = FALSE;
         }
         break;
      case '>':
         switch (token[1]) {
         case '=': op_code = OP_GE; token = &token[2]; break;
         case '>': op_code = OP_SHIFT_RIGHT; token = &token[2]; break;
         default: op_code = OP_GT; token++; break;
         }
         break;
      case '<':
         switch (token[1]) {
         case '=': op_code = OP_LE; token = &token[2]; break;
         case '<': op_code = OP_SHIFT_LEFT; token = &token[2]; break;
         default: op_code = OP_LT; token++; break;
         }
         break;
      case '+': op_code = OP_ADD; token++; break;
      case '*': op_code = OP_MUL; token++; break;
      case '/':
         if (token[1] == '/') {
            op_code = OP_INTDIV;
            token = &token[2];
         } else if (token[1] == '*') {
            for (token=(&token[2]); *token != '\0'; token++) {
               if (*token == '*' && token[1] == '/') {
                  token = &token[2];
                  break;
               }
            }
         } else {
            op_code = OP_DIV;
            token++;
         }
         break;
      case '%': op_code = OP_MOD; token++; break;
      case '!':
         if (token[1] == '=') {
            op_code = OP_NE;
            token = &token[2];
         } else {
            op_code = OP_NOT;
            token++;
         }
         break;
      case '~': op_code = OP_BIT_NOT; token++; break;
      case ')': op_code = OP_CLOSE; token++; break;
      case '(': op_code = OP_OPEN; token++; break;
      default:
         if (*token == '_' || (*token >= 'a' && *token <= 'z') ||
               (*token >= 'A' && *token <= 'Z')) {
            if (ParseForIDVal(&token, &v)) {
               /* 
                * if (*token == '(') {
                *    push a function on the stack!
                *    token++;
                * }
                */
               if (!ValPush(&v)) {
                 ok = FALSE;
               } else {
                  prev_token_is_op = FALSE;
               }
            } else {
               ok = FALSE;
            }
         } else {
            sprintf(gszMsgBox, TgLoadString(STID_ILLEGAL_EXPR_BAD_IDENTIFIER),
                  pszCurExpr, token);
            MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
            ok = FALSE;
         }
         break;
      }
      if (!ok) break;
      if (op_code != INVALID) {
         switch (op_code) {
         case OP_CLOSE:
            if (cur_depth == 0) {
               sprintf(gszMsgBox,
                     TgLoadString(STID_ILLEGAL_EXPR_TOO_MANY_SYMBOL),
                     pszCurExpr, ')');
               MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
               ok = FALSE;
            } else {
               cur_depth -= MAX_OP_PREC;
               if (!ProcessOp(OpPrec(op_code)+cur_depth)) {
                  ok = FALSE;
               }
            }
            break;
         case OP_OPEN: cur_depth += MAX_OP_PREC; break;
         default:
            if (ProcessOp(OpPrec(op_code)+cur_depth)) {
               if (!OpPush(op_code, cur_depth)) {
                  ok = FALSE;
               }
            } else {
               ok = FALSE;
            }
            break;
         }
         prev_token_is_op = TRUE;
      }
   }
   if (ok) {
      if (cur_depth != 0) {
         sprintf(gszMsgBox, TgLoadString(STID_ILLEGAL_EXPR_TOO_MANY_SYMBOL),
               pszCurExpr, '(');
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
         ok = FALSE;
      } else if (!ProcessOp((-1))) {
         ok = FALSE;
      } else if (topOpStk != NULL) {
         sprintf(gszMsgBox, TgLoadString(STID_ILL_EXPR_OP_STACK_NON_EMPTY),
               pszCurExpr);
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
         ok = FALSE;
      } else if (topValStk == NULL) {
         sprintf(gszMsgBox, TgLoadString(STID_ILL_EXPR_VAL_STACK_NON_EMPTY),
               pszCurExpr);
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
         ok = FALSE;
      } else if (topValStk->next != NULL) {
         sprintf(gszMsgBox, TgLoadString(STID_ILL_EXPR_TOO_MANY_VALS_LEFT),
               pszCurExpr);
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
         ok = FALSE;
      } else {
         memcpy(v_ptr, &topValStk->v, sizeof(struct VRec));
      }
   }
   return ok;
}

