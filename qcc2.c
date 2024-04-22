/* This file contains the code used to walk a parse tree to generate */
/* a transformation's matrix. */
/* This program is Copyright 1999 by Scott C. Moonen.  All Rights Reserved. */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "qcc.h"

int               numSyms = 0;
struct sym_entry *symTbl  = NULL;

int               xfStackSize = 0;
int              *xfStackParams = NULL;	// Number of params for each xfrm.
char           ***xfStackTraceNm = NULL;	// Names of each xfrm's traces.

// Set symbol table entry for given variable to specified value.

void symSetVar(char *ident, int value)
{
  struct sym_entry *ptr;

  if((ptr = symLookup(ident)) == NULL)
  {
    symTbl = realloc(symTbl, ++numSyms * sizeof(struct sym_entry));
    symTbl[numSyms - 1].ident  = strdup(ident);
    ptr = &symTbl[numSyms - 1];
  }

  ptr->type  |= SYM_VAR;
  ptr->value  = value;
}

// Set symbol table entry for given transformation definition.

void symSetTrans(char *ident, union parse_node *xfrm)
{
  struct sym_entry *ptr;

  if((ptr = symLookup(ident)) == NULL)
  {
    symTbl = realloc(symTbl, ++numSyms * sizeof(struct sym_entry));
    symTbl[numSyms - 1].ident  = strdup(ident);
    ptr = &symTbl[numSyms - 1];
  }

  ptr->type |= SYM_XFRM;
  ptr->xfrm  = xfrm;
}

// Look up an identifier in the symbol table.

struct sym_entry *symLookup(char *ident)
{
  int i;

  for(i = 0; i < numSyms; i++)
    if(strcmp(symTbl[i].ident, ident) == 0)
      return &symTbl[i];

  return NULL;
}

// Process the "main" transformation.

void parseMain(void)
{
  struct sym_entry *main;
  union parse_node *args;
  union parse_node  op;
  union retval     *rtn;
  int               i, j, k;
  char              str[257];

  if(!(main = symLookup("main")) || !(main->type & SYM_XFRM))
  {
    fprintf(stderr, "Error: no transformation 'main' defined!\n");
    exit(1);
  }
  if(main->xfrm->type != TYP_XFRM_DEF)
  {
    fprintf(stderr, "Internal error: inconsistent node type.\n");
    exit(1);
  }

// Set up trace stack.

  xfStackParams = realloc(xfStackParams, ++xfStackSize * sizeof(int));
  xfStackTraceNm = realloc(xfStackTraceNm, xfStackSize * sizeof(char **));
  xfStackParams [xfStackSize - 1] = 0;
  xfStackTraceNm[xfStackSize - 1] = NULL;

// Define initial trace list based on main's parameters.

  args = main->xfrm->xfrm.args;
  for(i = 0; i < args->alist.ngrp; i++)
  {
    for(j = 0; j < args->alist.groups[i]->agrp.nparam; j++)
    {
      if(args->alist.groups[i]->agrp.params[j]->type == TYP_VAR)
      {
        xfStackTraceNm[xfStackSize - 1]
          = realloc(xfStackTraceNm[xfStackSize - 1],
                    ++xfStackParams[xfStackSize - 1] * sizeof(char *));
        xfStackTraceNm[xfStackSize - 1][xfStackParams[xfStackSize - 1] - 1]
          = args->alist.groups[i]->agrp.params[j]->var.var;
      }
      else if(args->alist.groups[i]->agrp.params[j]->type == TYP_VAR_IDX)
      {
        if(args->alist.groups[i]->agrp.params[j]->idx.index->type != TYP_INT)
        {
          fprintf(stderr, "Error: transformation 'main' must take a "
                  "fixed number of parameters.\n");
          exit(1);
        }
        for(k = 0; 
            k < args->alist.groups[i]->agrp.params[j]->idx.index->val.value;
            k++)
        {
          sprintf(str, "%s[%i]",
                  args->alist.groups[i]->agrp.params[j]->idx.var, k);
          xfStackTraceNm[xfStackSize - 1]
            = realloc(xfStackTraceNm[xfStackSize - 1],
                      ++xfStackParams[xfStackSize - 1] * sizeof(char *));
          xfStackTraceNm[xfStackSize - 1][xfStackParams[xfStackSize - 1] - 1]
            = strdup(str);
        }
      }
    }
  }

  if(xfStackParams[xfStackSize - 1] > 31)
  {
    fprintf(stderr, "Error: too many parameters for transformation 'main':\n"
            "  simulation limit is 31 parameters.\n");
    exit(1);
  }

  op.type = TYP_OP;			// Construct operation.
  op.op.ident = malloc(sizeof(union parse_node));
  op.op.ident->type = TYP_VAR;
  op.op.ident->var.var = "main";	// Invoke 'main' transformation.
  op.op.params = malloc(sizeof(union parse_node));
  op.op.params->type = TYP_PARAM_LIST;

  op.op.params->plist.ngrp = args->alist.ngrp;
  op.op.params->plist.groups
    = malloc(args->alist.ngrp * sizeof(union parse_node *));
  for(i = 0; i < args->alist.ngrp; i++)
  {
    op.op.params->plist.groups[i] = malloc(sizeof(union parse_node));
    op.op.params->plist.groups[i]->type = TYP_PARAM_GRP;
    op.op.params->plist.groups[i]->pgrp.nparam
      = args->alist.groups[i]->agrp.nparam;
    op.op.params->plist.groups[i]->pgrp.params
      = malloc(args->alist.groups[i]->agrp.nparam * sizeof(union parse_node));
    for(j = 0; j < args->alist.groups[i]->agrp.nparam; j++)
    {
      if(args->alist.groups[i]->agrp.params[j]->type == TYP_VAR)
        op.op.params->plist.groups[i]->pgrp.params[j]
          = args->alist.groups[i]->agrp.params[j];
      else
      {
// If there's an array parameter to main, when we create the main
//   invocation, we have to turn it into a "pass-the-whole-array"-
//   style invocation -- leave off the subscript.
// I.e., "main(a[5])", when called, becomes "main(a);"

        op.op.params->plist.groups[i]->pgrp.params[j]
          = malloc(sizeof(union parse_node));
        op.op.params->plist.groups[i]->pgrp.params[j]->type = TYP_VAR;
        op.op.params->plist.groups[i]->pgrp.params[j]->var.var
          = strdup(args->alist.groups[i]->agrp.params[j]->idx.var);
      }
    }
  }

  if((rtn = processNode(&op)) == NULL)	// Null operation.
  {
    rtn = malloc(sizeof(union retval));
    rtn->u = matCreate(1 << xfStackParams[xfStackSize - 1], 1);
  }

  for(i = 0; i < op.op.params->plist.ngrp; i++)
  {
    for(j = 0; j < op.op.params->plist.groups[i]->pgrp.nparam; j++)
      if(args->alist.groups[i]->agrp.params[j]->type != TYP_VAR)
        free(op.op.params->plist.groups[i]->pgrp.params[j]);
    free(op.op.params->plist.groups[i]->pgrp.params);
    free(op.op.params->plist.groups[i]);
  }
  free(op.op.params->plist.groups);
  free(op.op.params);
  free(op.op.ident);

// Output the transformation's matrix.

  for(i = 0; i < rtn->u.dim; i++)
  {
    for(j = 0; j < rtn->u.dim; j++)
    {
      if(j)
        printf(", ");
      printf("(%.12g + %.12gi)", rtn->u.elem[i][j].r, rtn->u.elem[i][j].i);
    }
    printf("\n");
  }

  free(rtn);

// Yeah, this function isn't truly re-entrant -- there's probably a couple of
//   other problems -- nor does it need to be, but what the heck, we'll shrink
//   the trace stack anyway. :-)

  for(i = 0; i < xfStackParams[xfStackSize - 1]; i++)
    if(strchr(xfStackTraceNm[xfStackSize - 1][i], '[') != NULL)
      free(xfStackTraceNm[xfStackSize - 1][i]);
  free(xfStackTraceNm[xfStackSize - 1]);
  xfStackParams = realloc(xfStackParams, --xfStackSize * sizeof(int));
  xfStackTraceNm = realloc(xfStackTraceNm, xfStackSize * sizeof(char **));
}

// Process a generic parse-node.

union retval *processNode(union parse_node *node)
{
  union retval     *rtn;
  union retval     *ptr;
  struct sym_entry *sym_val;
  struct Umatrix    tmp_mat;
  int               i, j;

  switch(node->type)
  {
    case TYP_INT:			// Integer.
      rtn = malloc(sizeof(union retval));
      rtn->num = node->val.value;
      return rtn;
    case TYP_VAR:			// Variable.
      if((sym_val = symLookup(node->var.var)) == NULL
         || !(sym_val->type & SYM_VAR))
      {
        fprintf(stderr, "Error: Undefined variable used in expression.\n");
        exit(1);
      }
      rtn = malloc(sizeof(union retval));
      rtn->num = sym_val->value;
      return rtn;
    case TYP_ARITH:			// Arithmetic expression.
      return processArith(node);
    case TYP_BOOL:			// Boolean expression.
      return processBool(node);
    case TYP_IFELSE:			// if/else node.
      if(processBool(node->cond.cond))	// Check condition.
        return processNode(node->cond.ifcode);
      else if(node->cond.elseblock != NULL)
        return processNode(node->cond.elseblock);
      else				// No action taken.
        return NULL;
    case TYP_LOOP:			// for loop.
      ptr = processNode(node->loop.start);
      i = ptr->num;
      free(ptr);
      ptr = processNode(node->loop.finish);
      j = ptr->num;
      free(ptr);
      rtn = malloc(sizeof(union retval));
      rtn->u = matCreate(1 << xfStackParams[xfStackSize - 1], 1);
      if(j < i)
      {
        fprintf(stderr, "Error: loop end index is less than start index.\n");
        exit(1);
      }
      for(; i <= j; i++)
      {
        tmp_mat = rtn->u;
        symSetVar(node->loop.var->var.var, i);
        ptr = processNode(node->loop.block);
        if(ptr != NULL)
        {
          rtn->u = matMult(ptr->u, rtn->u);
          free(ptr);
          matFree(tmp_mat);
        }
      }
      return rtn;
    case TYP_OP:			// Operation.
      return processOp(node);
    case TYP_OP_SEQ:			// Operation sequence.
      if(node->seq.left == NULL)
      {
        if(node->seq.right == NULL)
          return NULL;
        else
          return processNode(node->seq.right);
      }
      else
      {
        if(node->seq.right == NULL)
          return processNode(node->seq.left);
        else
        {
          ptr = processNode(node->seq.left);
          if(ptr == NULL)
            return processNode(node->seq.right);
          rtn = malloc(sizeof(union retval));
          rtn->u = ptr->u;
          free(ptr);
          ptr = processNode(node->seq.right);
          if(ptr != NULL)
          {
            tmp_mat = rtn->u;
            rtn->u = matMult(ptr->u, rtn->u);
            free(ptr);
            matFree(tmp_mat);
          }
          return rtn;
        }
      }
    case TYP_XFRM_DEF:			// Transform definition.
      symSetTrans(node->xfrm.ident->var.var, node);
      rtn = malloc(sizeof(union retval));
      return rtn;			// Dummy return; unused.
    case TYP_XFRM_SEQ:			// Transformation sequence.
      if(node->seq.left != NULL)
        free(processNode(node->seq.left));
      if(node->seq.right != NULL)
        free(processNode(node->seq.right));
      rtn = malloc(sizeof(union retval));
      return rtn;			// Dummy return; unused.
    case TYP_VAR_IDX:			// Indexed variable.
    case TYP_ARG_GRP:			// Argument group.
    case TYP_ARG_LIST:			// Argument list.
    case TYP_PARAM_GRP:			// Parameter group.
    case TYP_PARAM_LIST:		// Parameter list.
      fprintf(stderr, "Internal error: improperly built parse tree.\n");
      exit(1);
  }
}

// Process an arithmetic parse-node.

union retval *processArith(union parse_node *node)
{
  union retval *rtn = malloc(sizeof(union retval));
  union retval *ptr;
  int           l, r;

// Ok, the numbers are reals, but I make 'em ints here.  Originally
// the structure held an int, and it doesn't really need to be changed
// since "variables" are always ints anyway; it's just parameters to
// the built-in transformations (angles...) that must be reals.

  ptr = processNode(node->a.left);
  l   = ptr->num;
  free(ptr);

  ptr = processNode(node->a.right);
  r   = ptr->num;
  free(ptr);

  switch(node->a.op)
  {
    case '*':
      rtn->num = l * r;
      return rtn;
    case '/':
      rtn->num = l / r;
      return rtn;
    case '+':
      rtn->num = l + r;
      return rtn;
    case '-':
      rtn->num = l - r;
      return rtn;
  }

  fprintf(stderr, "Internal error: bad arithmetic node.\n");
  exit(1);
}

// Process a boolean parse-node.

union retval *processBool(union parse_node *node)
{
  union retval *rtn = malloc(sizeof(union retval));
  union retval *ptr;
  int           l, r;

  ptr = processNode(node->b.left);
  l   = ptr->num;
  free(ptr);

  ptr = processNode(node->b.right);
  r   = ptr->num;
  free(ptr);

  switch(node->b.op)
  {
    case REL_EQ:
      rtn->num = l == r;
      return rtn;
    case REL_NE:
      rtn->num = l != r;
      return rtn;
    case REL_LT:
      rtn->num = l < r;
      return rtn;
    case REL_LE:
      rtn->num = l <= r;
      return rtn;
    case REL_GT:
      rtn->num = l > r;
      return rtn;
    case REL_GE:
      rtn->num = l >= r;
      return rtn;
  }

  fprintf(stderr, "Internal error: bad arithmetic node.\n");
  exit(1);
}

// Process an operation parse-node: invocation of a transformation.

union retval *processOp(union parse_node *node)
{
  int               i, j, k, l;
  char             *xfrmName = node->op.ident->var.var;	// Transformation name.
  union parse_node *xfrmDef;		// Transformation definition.
  union parse_node *ptr;                // Temporary pointer.
  union retval     *exprn;		// Arithmetic expression result.
  union retval     *rtn;		// Value we return.
  struct sym_entry *symbol;		// Symbol retrieved from symtbl.
  int               nParamGrp = 0;	// Number of param. grps xfrm accepts.
  int              *paramCnt = NULL;	// # of params accepted for each grp.
  char            **varParamCnt = NULL;	// If !NULL, name of var param for grp.
  int               nParam = 0;		// Actual number of parameter traces.
  int              *paramOrder = NULL;  // List of trace order in param list.
  char             *str;                // Temporary string.
  int               startCnt;		// # params @ start of param grp.
  char            **var_stack = NULL;	// Names of variable arguments.
  int              *value_stack = NULL;	// Old values for variable args.
  int               param_stack_sz = 0;	// Number of var arg values to restore.
  struct Umatrix    swapMatrix;		// Bit-swap matrix.
  struct Umatrix    opMatrix;           // Operation matrix.
  struct Umatrix    tmpMatrix;		// Internal-transformation matrix.
  struct Umatrix    freeMatrix;		// Matrix to be discarded.
  int               builtIn = 0;	// If true, built-in transform.
  int               grpParams;          // Number of params in a param grp.
  double            d;

// Create new stack entry.

  xfStackParams = realloc(xfStackParams, ++xfStackSize * sizeof(int));
  xfStackTraceNm = realloc(xfStackTraceNm, xfStackSize * sizeof(char **));

  xfStackParams[xfStackSize - 1] = 0;
  xfStackTraceNm[xfStackSize - 1] = NULL;

  if(strcmp(xfrmName, "H") == 0)	// Shortcut to Hadamard transform.
  {
    tmpMatrix = matCreate(2, 0);
    tmpMatrix.elem[0][0].r
      = tmpMatrix.elem[0][1].r
      = tmpMatrix.elem[1][0].r
      = M_SQRT1_2;
    tmpMatrix.elem[1][1].r
      = -M_SQRT1_2;
    builtIn = 1;
    nParamGrp = 1;
    paramCnt    = malloc(sizeof(int));
    varParamCnt = malloc(sizeof(char *));
    paramCnt[0]    = 1;
    varParamCnt[0] = NULL;
  }
  else if(strcmp(xfrmName, "Cnot") == 0)	// Built-in Cnot transform.
  {
    tmpMatrix = matCreate(4, 1);
    matRowSwap(tmpMatrix, 2, 3);
    builtIn = 1;
    nParamGrp = 1;
    paramCnt    = malloc(sizeof(int));
    varParamCnt = malloc(sizeof(char *));
    paramCnt[0]    = 2;
    varParamCnt[0] = NULL;
  }
  else if(strcmp(xfrmName, "R") == 0)	// Built-in rotation transform.
  {
    if(node->op.params->plist.groups[0]->pgrp.params[0]->type != TYP_INT)
    {
      fprintf(stderr, "Error: built-in transformation 'R' requires "
                      "angle parameter.\n");
      exit(1);
    }
    d = node->op.params->plist.groups[0]->pgrp.params[0]->val.value;
    tmpMatrix = matCreate(2, 0);
    tmpMatrix.elem[0][0].r
      = tmpMatrix.elem[1][1].r
      = cos(d);
    tmpMatrix.elem[1][0].r = - sin(d);
    tmpMatrix.elem[0][1].r = - tmpMatrix.elem[1][0].r;
    builtIn = 1;
    nParamGrp = 2;
    paramCnt    = malloc(2 * sizeof(int));
    varParamCnt = malloc(2 * sizeof(char *));
    paramCnt[0]    = -1;
    paramCnt[1]    = 1;
    varParamCnt[0] = varParamCnt[1] = NULL;
  }
  else if(strcmp(xfrmName, "P1") == 0)	// Built-in phase shift; 1st variation.
  {
    if(node->op.params->plist.groups[0]->pgrp.params[0]->type != TYP_INT)
    {
      fprintf(stderr, "Error: built-in transformation 'P1' requires "
                      "angle parameter.\n");
      exit(1);
    }
    d = node->op.params->plist.groups[0]->pgrp.params[0]->val.value;
    tmpMatrix = matCreate(2, 0);
    tmpMatrix.elem[0][0].r = tmpMatrix.elem[1][1].r = cos(d);
    tmpMatrix.elem[0][0].i = sin(d);
    tmpMatrix.elem[1][1].i = - tmpMatrix.elem[0][0].i;
    builtIn = 1;
    nParamGrp = 2;
    paramCnt    = malloc(2 * sizeof(int));
    varParamCnt = malloc(2 * sizeof(char *));
    paramCnt[0]    = -1;
    paramCnt[1]    = 1;
    varParamCnt[0] = varParamCnt[1] = NULL;
  }
  else if(strcmp(xfrmName, "P2") == 0)	// Built-in phase shift; 2nd variation.
  {
    if(node->op.params->plist.groups[0]->pgrp.params[0]->type != TYP_INT)
    {
      fprintf(stderr, "Error: built-in transformation 'P2' requires "
                      "angle parameter.\n");
      exit(1);
    }
    d = node->op.params->plist.groups[0]->pgrp.params[0]->val.value;
    tmpMatrix = matCreate(2, 0);
    tmpMatrix.elem[0][0].r = tmpMatrix.elem[1][1].r = cos(d);
    tmpMatrix.elem[0][0].i = tmpMatrix.elem[1][1].i = sin(d);
    builtIn = 1;
    nParamGrp = 2;
    paramCnt    = malloc(2 * sizeof(int));
    varParamCnt = malloc(2 * sizeof(char *));
    paramCnt[0]    = -1;
    paramCnt[1]    = 1;
    varParamCnt[0] = varParamCnt[1] = NULL;
  }
  else					// User-defined transformation.
  {
    if((symbol = symLookup(xfrmName)) == NULL || !(symbol->type & SYM_XFRM))
    {
      fprintf(stderr, "Error: use of undefined transformation '%s'\n",
              xfrmName);
      exit(1);
    }
    xfrmDef = symbol->xfrm;

    nParamGrp   = xfrmDef->xfrm.args->alist.ngrp;
    paramCnt    = malloc(nParamGrp * sizeof(int));
    varParamCnt = malloc(nParamGrp * sizeof(char *));
    memset(paramCnt,    0, nParamGrp * sizeof(int));
    memset(varParamCnt, 0, nParamGrp * sizeof(char *));

// Count the number of parameters we're expecting.

    for(i = 0; i < nParamGrp; i++)
    {
      ptr = xfrmDef->xfrm.args->alist.groups[i];
      for(j = 0; j < ptr->agrp.nparam; j++)
      {
        if(ptr->agrp.params[j]->type == TYP_VAR)
          paramCnt[i]++;
        else if(ptr->agrp.params[j]->type == TYP_VAR_IDX)
        {
          if(ptr->agrp.params[j]->idx.index->type == TYP_INT)
            paramCnt[i] += ptr->agrp.params[j]->idx.index->val.value;
          else
          {
            if(varParamCnt[i] == NULL)
              varParamCnt[i] = ptr->agrp.params[j]->idx.index->var.var;
            else
            {
              fprintf(stderr, "Error: transformation '%s' accepts too many "
                      "variable arguments in argument group.\n", xfrmName);
              exit(1);
            }
          }
        }
      }
    }
  }

// Match up actual parameters with placeholders.

  if(node->op.params->plist.ngrp != nParamGrp
     && !(builtIn && paramCnt[0] == -1
          && node->op.params->plist.ngrp - 1 == nParamGrp))
  {
    fprintf(stderr, "Error: bad parameter count for transformation '%s'\n",
            xfrmName);
    exit(1);
  }

  for(i = 0; i < nParamGrp; i++)	// Match up params within groups.
  {
    if(paramCnt[i] == -1)		// Skip built-ins' numeric params.
      continue;

// First establish the ordering of the parameters in the paramOrder
//   array, and count them so we know the size of any variable-size
//   parameter.

    ptr = node->op.params->plist.groups[i];
    grpParams = 0;

    for(j = 0; j < ptr->pgrp.nparam; j++)
    {
      if(ptr->pgrp.params[j]->type == TYP_VAR_IDX)
      {
        str = (char *)malloc(strlen(ptr->pgrp.params[j]->idx.var) + 20);
        exprn = processNode(ptr->plist.groups[j]->idx.index);
        sprintf(str, "%s[%i]", ptr->pgrp.params[j]->idx.var, (int)exprn->num);
        for(k = 0; k < xfStackParams[xfStackSize - 2]; k++)
          if(strcmp(str, xfStackTraceNm[xfStackSize - 2][k]) == 0)
            break;
        if(k < xfStackParams[xfStackSize - 2])
        {
          paramOrder = realloc(paramOrder, ++nParam * sizeof(int));
          paramOrder[nParam - 1] = k;
          grpParams++;
        }
        else
        {
          fprintf(stderr,
                  "Error: unknown parameter '%s' to transformation '%s'.\n",
                  str, xfrmName);
          exit(1);
        }
        free(exprn);
        free(str);
      }
      else				// Type is TYP_VAR.
      {
        for(k = 0; k < xfStackParams[xfStackSize - 2]; k++)
          if(strcmp(ptr->pgrp.params[j]->var.var,
                    xfStackTraceNm[xfStackSize - 2][k]) == 0)
            break;

        if(k < xfStackParams[xfStackSize - 2])
        {
          paramOrder = realloc(paramOrder, ++nParam * sizeof(int));
          paramOrder[nParam - 1] = k;
          grpParams++;
        }
        else				// No trace found; search for array.
        {
          l = strlen(ptr->pgrp.params[j]->var.var);
          for(k = 0; k < xfStackParams[xfStackSize - 2]; k++)
          {
            if(strncmp(ptr->pgrp.params[j]->var.var,
                       xfStackTraceNm[xfStackSize - 2][k], l) == 0
               && xfStackTraceNm[xfStackSize - 2][k][l] == '[')
            {
              paramOrder = realloc(paramOrder, ++nParam * sizeof(int));
              paramOrder[nParam - 1] = k;
              grpParams++;
            }
          }
        }
      }
    }

    if(grpParams < paramCnt[i])		// Not enough params.
    {
insufParam:
      fprintf(stderr, "Error: insufficient arguments to transformation '%s'.\n",
              xfrmName);
      exit(1);
    }
    else if(grpParams > paramCnt[i]	// Too many params.
            && !varParamCnt[i])
    {
      fprintf(stderr, "Error: too many arguments to transformation '%s'.\n",
              xfrmName);
      exit(1);
    }
    else if(varParamCnt[i])		// Assign variable parameter value.
    {
      struct sym_entry *tmp;

      if(grpParams == paramCnt[i])
        goto insufParam;

      tmp = symLookup(varParamCnt[i]);
      if(tmp != NULL && (tmp->type & SYM_VAR))	// Push variable arg on stack.
      {
        var_stack = realloc(var_stack, ++param_stack_sz * sizeof(char *));
        value_stack = realloc(value_stack, param_stack_sz * sizeof(int));
        var_stack[param_stack_sz - 1] = varParamCnt[i];
        value_stack[param_stack_sz - 1] = tmp->value;
      }

// Store variable-arg in variable storage.

      symSetVar(varParamCnt[i], grpParams - paramCnt[i]);
    }

// Now that we have the parameter ordering, generate the actual list of
//   traces on the trace stack that are the parameters to the transformation
//   for this particular group.
// I.e., if we invoke "xfm(a,b,c)", we want to place the names of xfm's
//   placeholder parameters on the top of the trace stack, rather than
//   a, b, and c.

    if(!builtIn)
    {
      ptr = xfrmDef->xfrm.args->alist.groups[i];
      for(j = 0; j < ptr->agrp.nparam; j++)
      {
        if(ptr->agrp.params[j]->type == TYP_VAR)
        {
          xfStackTraceNm[xfStackSize - 1]
            = realloc(xfStackTraceNm[xfStackSize - 1],
                      ++xfStackParams[xfStackSize - 1] * sizeof(char *));
          xfStackTraceNm[xfStackSize - 1][xfStackParams[xfStackSize - 1] - 1]
            = ptr->agrp.params[j]->var.var;
        }
        else if(ptr->agrp.params[j]->type == TYP_VAR_IDX)
        {
          if(ptr->agrp.params[j]->idx.index->type == TYP_INT)
          {
            for(k = 0; k < ptr->agrp.params[j]->idx.index->val.value; k++)
            {
              str = (char *)malloc(strlen(ptr->agrp.params[j]->idx.var) + 20);
              sprintf(str, "%s[%i]", ptr->agrp.params[j]->idx.var, k);
              xfStackTraceNm[xfStackSize - 1]
                = realloc(xfStackTraceNm[xfStackSize - 1],
                          ++xfStackParams[xfStackSize - 1] * sizeof(char *));
              xfStackTraceNm[xfStackSize - 1][xfStackParams[xfStackSize-1] - 1]
                = str;
            }
          }
          else
          {
            for(k = 0; k < grpParams - paramCnt[i]; k++)
            {
              str = (char *)malloc(strlen(ptr->agrp.params[j]->idx.var) + 20);
              sprintf(str, "%s[%i]", ptr->agrp.params[j]->idx.var, k);
              xfStackTraceNm[xfStackSize - 1]
                = realloc(xfStackTraceNm[xfStackSize - 1],
                          ++xfStackParams[xfStackSize - 1] * sizeof(char *));
              xfStackTraceNm[xfStackSize - 1][xfStackParams[xfStackSize-1] - 1]
                = str;
            }
          }
        }
      }
    }
  }

  if(nParam > 31)			// Too many parameters for a PC. :-)
  {
    fprintf(stderr, "Error: too many parameters for transformation '%s':\n"
            "  simulation limit is 31 parameters.\n", xfrmName);
    exit(1);
  }

// Generate swap matrix.

// First, shuffle about the traces that aren't included on the param list;
//   we'll keep them someplace in the swap and make sure the identity op
//   is applied to them.

  paramOrder = realloc(paramOrder,
                       xfStackParams[xfStackSize - 2] * sizeof(int));

  k = nParam;
  for(i = 0; i < xfStackParams[xfStackSize - 2]; i++)
  {
    for(j = 0; j < nParam; j++)
      if(paramOrder[j] == i)
        break;
    if(j == nParam)
      paramOrder[k++] = i;
  }

// Now generate the matrix itself.

  swapMatrix = matCreate(1 << xfStackParams[xfStackSize - 2], 0);
  for(i = 0; i < (1 << xfStackParams[xfStackSize - 2]); i++)
  {
    j = 0;
    for(k = 0; k < xfStackParams[xfStackSize - 2]; k++)
      j |= ((i >> paramOrder[k]) & 1) << k;
    swapMatrix.elem[i][j].r = 1.0;
  }

// Evaluate operations of the invoked transformation.

  if(!builtIn)
  {
    union retval *exprn;
    exprn     = processNode(xfrmDef->xfrm.ops);
    tmpMatrix = exprn->u;
    free(exprn);
  }

  opMatrix = matCreate(1 << xfStackParams[xfStackSize - 2], 0);

  if(opMatrix.dim % tmpMatrix.dim)
  {
    fprintf(stderr, "Internal error: matrix dimension mismatch.\n");
    exit(1);
  }

// Find the tensor product of the operations' matrix and the identity matrix,
//   to bring it up to the proper size.

  for(i = 0; i < opMatrix.dim / tmpMatrix.dim; i++)
    for(j = 0; j < tmpMatrix.dim; j++)
      for(k = 0; k < tmpMatrix.dim; k++)
        opMatrix.elem[i * tmpMatrix.dim + j][i * tmpMatrix.dim + k]
          = tmpMatrix.elem[j][k];

  freeMatrix = opMatrix;
  opMatrix = matMult(swapMatrix, opMatrix);	// Swap in appropriate rows.
  matFree(freeMatrix);

  matTranspose(swapMatrix);			// Find inversion of swap.

  freeMatrix = opMatrix;
  opMatrix = matMult(opMatrix, swapMatrix);	// Swap rows back to old pos'ns.
  matFree(freeMatrix);

  matFree(swapMatrix);
  matFree(tmpMatrix);

  free(paramOrder);

// Pop variable args off top of stack.

  for(i = 0; i < param_stack_sz; i++)
    symSetVar(var_stack[i], value_stack[i]);
  free(var_stack);
  free(value_stack);

// Pop info off top of stack.

  for(i = 0; i < xfStackParams[xfStackSize - 1]; i++)
    if(strchr(xfStackTraceNm[xfStackSize - 1][i], '[') != NULL)
      free(xfStackTraceNm[xfStackSize - 1][i]);
  free(xfStackTraceNm[xfStackSize - 1]);
  xfStackParams = realloc(xfStackParams, --xfStackSize * sizeof(int));
  xfStackTraceNm = realloc(xfStackTraceNm, xfStackSize * sizeof(char **));

  rtn = malloc(sizeof(union retval));
  rtn->u = opMatrix;
  return rtn;
}

// Create a matrix.

struct Umatrix matCreate(int n, int ident)
{
  struct Umatrix ret;
  int            i;

  ret.dim  = n;
  ret.elem = malloc(n * sizeof(struct complex *));
  for(i = 0; i < n; i++)
  {
    ret.elem[i] = malloc(n * sizeof(struct complex));
    memset(ret.elem[i], 0, n * sizeof(struct complex));
    if(ident)				// Set up as identity matrix.
      ret.elem[i][i].r = 1.0;
  }

  return ret;
}

// Destroy a matrix.

void matFree(struct Umatrix m)
{
  int i;

  for(i = 0; i < m.dim; i++)
    free(m.elem[i]);
  free(m.elem);
}

// Multiply two matrices.

struct Umatrix matMult(struct Umatrix m1, struct Umatrix m2)
{
  int            i, j, k;
  struct Umatrix ret = matCreate(m1.dim, 0);

  if(m1.dim != m2.dim)
  {
    fprintf(stderr, "Internal error: matrix dimension mismatch.\n");
    exit(1);
  }

  for(i = 0; i < m1.dim; i++)
    for(j = 0; j < m1.dim; j++)
      for(k = 0; k < m1.dim; k++)
      {
        ret.elem[i][k].r += m1.elem[i][j].r * m2.elem[j][k].r
                            - m1.elem[i][j].i * m2.elem[j][k].i;
        ret.elem[i][k].i += m1.elem[i][j].r * m2.elem[j][k].i
                            + m1.elem[i][j].i * m2.elem[j][k].r;
      }

  return ret;
}

// Transpose a matrix (we don't need to do conjugate transpose since the only
//   time we call it is with a real-valued matrix).

struct Umatrix matTranspose(struct Umatrix m)
{
  int            i, j;
  struct complex tmp;

  for(i = 0; i < m.dim; i++)
    for(j = i + 1; j < m.dim; j++)
    {
      tmp          = m.elem[i][j];
      m.elem[i][j] = m.elem[j][i];
      m.elem[j][i] = tmp;
    }

  return m;
}

// Swap two rows in matrix.

struct Umatrix matRowSwap(struct Umatrix m, int row1, int row2)
{
  struct complex *tmp;

  if(row1 != row2)
  {
    tmp          = m.elem[row1];
    m.elem[row1] = m.elem[row2];
    m.elem[row2] = tmp;
  }

  return m;
}

