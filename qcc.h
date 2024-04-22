/* Header file for QCC compiler. */
/* This program is Copyright 1999 by Scott C. Moonen.  All Rights Reserved. */

typedef enum
{
  REL_EQ,				// '='
  REL_NE,				// '!='
  REL_LT,				// '<'
  REL_LE,				// '<='
  REL_GT,				// '>'
  REL_GE				// '>='
} relop;

typedef enum
{
  TYP_INT,				// Integer.
  TYP_VAR,				// Variable.
  TYP_VAR_IDX,				// Indexed variable.
  TYP_ARITH,				// Arithmetic expression.
  TYP_BOOL,				// Relational (boolean) expression.
  TYP_IFELSE,				// if / else node.
  TYP_LOOP,				// for loop.
  TYP_PARAM_GRP,			// Parameter group..
  TYP_PARAM_LIST,			// Parameter list.
  TYP_OP,				// Operation.
  TYP_OP_SEQ,				// Operation sequence.
  TYP_ARG_GRP,				// Argument group.
  TYP_ARG_LIST,				// Argument list.
  TYP_XFRM_DEF,				// Transform definition.
  TYP_XFRM_SEQ,				// Transformation sequence.
} node_type;

typedef enum
{
  SYM_VAR = 1,				// Plain vanilla variable.
  SYM_XFRM = 2				// Transformation.
} sym_type;

struct var_idx
{
  node_type         type;		// Node type.
  char             *var;
  union parse_node *index;
};

struct arith				// Arithmetic expression.
{
  node_type         type;		// Node type.
  char              op;			// Operator.
  union parse_node *left, *right;	// Left- and right-hand expressions.
};

struct bool				// Boolean relation.
{
  node_type         type;		// Node type.
  relop             op;			// Operator.
  union parse_node *left, *right;	// Left- and right-hand expressions.
};

struct ifelse				// If / else branch point.
{
  node_type         type;		// Node type.
  union parse_node *cond;
  union parse_node *ifcode;
  union parse_node *elseblock;
};

struct loop				// for-loop.
{
  node_type         type;		// Node type.
  union parse_node *var;
  union parse_node *start;
  union parse_node *finish;
  union parse_node *block;
};

struct param_grp
{
  node_type          type;		// Node type.
  int                nparam;
  union parse_node **params;
};

struct param_list
{
  node_type           type;		// Node type.
  int                 ngrp;
  union parse_node **groups;
};

struct op
{
  node_type         type;		// Node type.
  union parse_node *ident;
  union parse_node *params;
};

struct sequence
{
  node_type         type;		// Node type.
  union parse_node *left;
  union parse_node *right;
};

struct arg_grp
{
  node_type          type;		// Node type.
  int                nparam;
  union parse_node **params;
};

struct arg_list
{
  node_type          type;		// Node type.
  int                ngrp;
  union parse_node **groups;
};

struct transform
{
  node_type         type;		// Node type.
  union parse_node *ident;
  union parse_node *args;
  union parse_node *ops;
};

struct value
{
  node_type type;
  double    value;
};

struct var
{
  node_type  type;
  char      *var;
};

union parse_node
{
  node_type         type;		// Node type.
  struct value      val;
  struct var        var;
  struct var_idx    idx;
  struct arith      a;
  struct bool       b;
  struct ifelse     cond;
  struct loop       loop;
  struct param_grp  pgrp;
  struct param_list plist;
  struct sequence   seq;
  struct op         op;
  struct arg_grp    agrp;
  struct arg_list   alist;
  struct transform  xfrm;
};

struct complex
{
  double r, i;
};

struct Umatrix
{
  int              dim;			// Dimension.
  struct complex** elem;		// Matrix elements [row][col].
};

union retval
{
  struct Umatrix u;
  double         num;
};

struct sym_entry			// Symbol table entry.
{
  sym_type          type;
  char             *ident;

  double            value;
  union parse_node *xfrm;
};


/* Function declarations. */

union retval *processNode(union parse_node *node);
union retval *processArith(union parse_node *node);
union retval *processBool(union parse_node *node);
union retval *processOp(union parse_node *node);

void symSetVar(char *ident, int value);
void symSetTrans(char *ident, union parse_node *xfrm);
struct sym_entry *symLookup(char *ident);

struct Umatrix matCreate(int n, int ident);
void           matFree(struct Umatrix m);
struct Umatrix matMult(struct Umatrix m1, struct Umatrix m2);
struct Umatrix matTranspose(struct Umatrix m);
struct Umatrix matRowSwap(struct Umatrix m, int row1, int row2);

