%{
/* This program is the parser for the quantum programming language. */
/* This program is Copyright 1999 by Scott C. Moonen.  All Rights Reserved. */

#include <stdio.h>
#include <stdlib.h>

#include "qcc.h"

extern int   yylineno;
extern char* yytext;

%}

%union {
  union parse_node  *node;
  double             num;
  char              *ident;
}

%token <num>   NUMBER
%token <ident> IDENT
%token <num>   IF, ELSEIF, ELSE, INCLUDE, FOR
%type  <num>   rel_op
%type  <node>  program, transformations, transform, arg_list, arg_group, arg
%type  <node>  op_list, operation, timeslice_trans, trans_op
%type  <node>  param_list, param_group, param, loop, conditional, cond_no_else
%type  <node>  boolean, expression, term, factor

%%

program		: transformations	{ $$ = $1; parseMain(); }
		  | includes transformations	{ $$ = $2; parseMain(); }

includes	: include IDENT ';'	{

		        /* Hook up file $2 to lexer. */

		      }
		  | includes include IDENT ';'	{

		        /* Hook up file $3 to lexer. */

		      }

transformations	: transform		{
		      $$ = malloc(sizeof(union parse_node));
		      $$->type      = TYP_XFRM_SEQ;
		      $$->seq.left  = $1;
		      $$->seq.right = NULL;
		    }
		  | transformations transform	{
		      $$ = malloc(sizeof(union parse_node));
		      $$->type      = TYP_XFRM_SEQ;
		      $$->seq.left  = $1;
		      $$->seq.right = $2;
		    }

transform	: IDENT '(' arg_list ')' '{' op_list '}'	{
		    $$ = malloc(sizeof(union parse_node));
		    $$->type  = TYP_XFRM_DEF;
		    $$->xfrm.ident = malloc(sizeof(union parse_node));
		    $$->xfrm.ident->type = TYP_VAR;
		    $$->xfrm.ident->var.var  = $1;
		    $$->xfrm.args        = $3;
		    $$->xfrm.ops         = $6;
		    symSetTrans($1, $$);	// Add xfrm to symbol tbl.
		  }

arg_list	: arg_group		{
		      $$ = malloc(sizeof(union parse_node));
		      $$->type            = TYP_ARG_LIST;
		      $$->alist.ngrp      = 1;
		      $$->alist.groups    = malloc(sizeof(union parse_node));
		      $$->alist.groups[0] = $1;
		    }
		  | arg_list ';' arg_group	{
		      $$ = $1;
		      $$->alist.groups  = realloc(
		                            $$->alist.groups,
		                            ++$$->alist.ngrp
		                              * sizeof(union parse_node));
		      $$->alist.groups[$$->alist.ngrp - 1] = $3;
		    }

arg_group	: arg			{
		      $$ = malloc(sizeof(union parse_node));
		      $$->type           = TYP_ARG_GRP;
		      $$->agrp.nparam    = 1;
		      $$->agrp.params    = malloc(sizeof(union parse_node));
		      $$->agrp.params[0] = $1;
		    }
		  | arg_group ',' arg	{
		      $$ = $1;
		      $$->agrp.params  = realloc(
		                           $$->agrp.params,
		                           ++$$->agrp.nparam
		                             * sizeof(union parse_node));
		      $$->agrp.params[$$->agrp.nparam - 1] = $3;
		    }

arg		: IDENT			{
		      $$ = malloc(sizeof(union parse_node));
		      $$->type = TYP_VAR;
		      $$->var.var  = $1;
		    }
		  | IDENT '[' NUMBER ']'	{
		      $$ = malloc(sizeof(union parse_node));
		      $$->type      = TYP_VAR_IDX;
		      $$->idx.var   = $1;
		      $$->idx.index = malloc(sizeof(union parse_node));
		      $$->idx.index->type  = TYP_INT;
		      $$->idx.index->val.value = $3;
		    }
		  | IDENT '[' IDENT ']'		{
		      $$ = malloc(sizeof(union parse_node));
		      $$->type      = TYP_VAR_IDX;
		      $$->idx.var   = $1;
		      $$->idx.index = malloc(sizeof(union parse_node));
		      $$->idx.index->type = TYP_VAR;
		      $$->idx.index->var.var  = $3;
		    }

op_list		: op_list operation	{
		      $$ = malloc(sizeof(union parse_node));
		      $$->type  = TYP_OP_SEQ;
		      $$->seq.left  = $1;
		      $$->seq.right = $2;
		    }
		  | operation		{ $$ = $1; }

operation	: timeslice_trans ';'	{ $$ = $1; }
		  | conditional		{ $$ = $1; }
		  | loop		{ $$ = $1; }

timeslice_trans	: trans_op		{ $$ = $1; }
		  | timeslice_trans ',' trans_op	{
		      $$ = malloc(sizeof(union parse_node));
		      $$->type  = TYP_OP_SEQ;
		      $$->seq.left  = $1;
		      $$->seq.right = $3;
		    }

trans_op	: IDENT '(' param_list ')'	{
		    $$ = malloc(sizeof(union parse_node));
		    $$->type           = TYP_OP;
		    $$->op.ident       = malloc(sizeof(union parse_node));
		    $$->op.ident->type = TYP_VAR;
		    $$->op.ident->var.var  = $1;
		    $$->op.params      = $3;
		  }

param_list	: param_group		{
		      $$ = malloc(sizeof(union parse_node));
		      $$->type            = TYP_PARAM_LIST;
		      $$->plist.ngrp      = 1;
		      $$->plist.groups    = malloc(sizeof(union parse_node));
		      $$->plist.groups[0] = $1;
		    }
		  | param_list ';' param_group	{
		      $$ = $1;
		      $$->plist.groups  = realloc(
		                            $$->plist.groups,
		                            ++$$->plist.ngrp
		                              * sizeof(union parse_node));
		      $$->plist.groups[$$->plist.ngrp - 1] = $3;
		    }

param_group	: param			{
		      $$ = malloc(sizeof(union parse_node));
		      $$->type           = TYP_PARAM_GRP;
		      $$->pgrp.nparam    = 1;
		      $$->pgrp.params    = malloc(sizeof(union parse_node));
		      $$->pgrp.params[0] = $1;
		    }
		  | param_group ',' param	{
		      $$ = $1;
		      $$->pgrp.params = realloc(
		                          $$->pgrp.params,
		                          ++$$->pgrp.nparam
		                            * sizeof(union parse_node));
		      $$->pgrp.params[$$->pgrp.nparam - 1] = $3;
		    }


param		: NUMBER		{
		      $$ = malloc(sizeof(union parse_node));
		      $$->type  = TYP_INT;
		      $$->val.value = $1;
		    }
		  | IDENT		{
		      $$ = malloc(sizeof(union parse_node));
		      $$->type = TYP_VAR;
		      $$->var.var  = $1;
		    }
		  | IDENT '[' expression ']' {
		      $$ = malloc(sizeof(union parse_node));
		      $$->type      = TYP_VAR_IDX;
		      $$->idx.var   = $1;
		      $$->idx.index = $3;
		    }

loop		: for IDENT ',' expression ',' expression '{' op_list '}' {
		    $$ = malloc(sizeof(union parse_node));
		    $$->type           = TYP_LOOP;
		    $$->loop.var       = malloc(sizeof(union parse_node));
		    $$->loop.var->type = TYP_VAR;
		    $$->loop.var->var.var  = $2;
		    $$->loop.start     = $4;
		    $$->loop.finish    = $6;
		    $$->loop.block  = $8;
		  }

conditional	: cond_no_else		{ $$ = $1; }
		  | cond_no_else else '{' op_list '}' {
		      union parse_node *ptr = $1;
		      $$ = $1;
		      while(ptr->cond.elseblock != NULL)
		        ptr = ptr->cond.elseblock;
		      ptr->cond.elseblock = $4;
		    }

cond_no_else	: if '(' boolean ')' '{' op_list '}' {
		      $$ = malloc(sizeof(union parse_node));
		      $$->type           = TYP_IFELSE;
		      $$->cond.cond      = $3;
		      $$->cond.ifcode    = $6;
		      $$->cond.elseblock = NULL;
		    }
		  | cond_no_else elseif '(' boolean ')' '{' op_list '}' {
		      union parse_node *ptr = $1;
		      $$ = $1;
		      while(ptr->cond.elseblock != NULL)
		        ptr = ptr->cond.elseblock;
		      ptr->cond.elseblock = malloc(sizeof(union parse_node));
		      ptr = ptr->cond.elseblock;
		      ptr->type           = TYP_IFELSE;
		      ptr->cond.cond      = $4;
		      ptr->cond.ifcode    = $7;
		      ptr->cond.elseblock = NULL;
		    }


expression	: term			{ $$ = $1; }
		  | expression '+' term	{
		      $$ = malloc(sizeof(union parse_node));
		      $$->type    = TYP_ARITH;
		      $$->a.op    = '+';
		      $$->a.left  = $1;
		      $$->a.right = $3;
		    }
		  | expression '-' term {
		      $$ = malloc(sizeof(union parse_node));
		      $$->type    = TYP_ARITH;
		      $$->a.op    = '-';
		      $$->a.left  = $1;
		      $$->a.right = $3;
		    }

term		: factor		{ $$ = $1; }
		  | term '*' factor	{
		      $$ = malloc(sizeof(union parse_node));
		      $$->type    = TYP_ARITH;
		      $$->a.op    = '*';
		      $$->a.left  = $1;
		      $$->a.right = $3;
		    }
		  | term '/' factor	{
		      $$ = malloc(sizeof(union parse_node));
		      $$->type    = TYP_ARITH;
		      $$->a.op    = '/';
		      $$->a.left  = $1;
		      $$->a.right = $3;
		    }

factor		: '(' expression ')'	{ $$ = $2; }
		  | NUMBER		{
		      $$ = malloc(sizeof(union parse_node));
		      $$->type  = TYP_INT;
		      $$->val.value = $1;
		    }
		  | IDENT		{
		      $$ = malloc(sizeof(union parse_node));
		      $$->type = TYP_VAR;
		      $$->var.var  = $1;
		    }
		  | '-' factor		{
		      $$ = malloc(sizeof(union parse_node));
		      $$->type    = TYP_ARITH;
		      $$->a.op    = '*';
		      $$->a.left  = malloc(sizeof(union parse_node));
		      $$->a.left->type  = TYP_INT;
		      $$->a.left->val.value = -1;
		      $$->a.right = $2;
		    }

boolean		: expression rel_op expression	{
		    $$ = malloc(sizeof(union parse_node));
		    $$->type    = TYP_BOOL;
		    $$->b.op    = $2;
		    $$->b.left  = $1;
		    $$->b.right = $3;
		  }

rel_op		: '='			{ $$ = REL_EQ; }
		  | '!' '='		{ $$ = REL_NE; }
		  | '<'			{ $$ = REL_LT; }
		  | '>'			{ $$ = REL_GT; }
		  | '<' '='		{ $$ = REL_LE; }
		  | '>' '='		{ $$ = REL_GE; }

include		: INCLUDE
for		: FOR
if		: IF
elseif		: ELSEIF
else		: ELSE

%%

int yyerror(const char *msg)
{
  printf("%i: %s at '%s'\n", yylineno, msg, yytext);
}

void main(void)
{
  yyparse();
}

