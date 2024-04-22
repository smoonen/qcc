%{
/* This program tokenizes a quantum-language program. */
/* This program is Copyright 1999 by Scott C. Moonen.  All Rights Reserved. */

#include "y.tab.h"
#include <stdlib.h>
#include <string.h>

int yylineno = 1;

%}
%%

[\t ]+		;			/* Ignore whitespace. */
\n		{ yylineno++; }		/* Ignore, but count, newlines. */

([0-9]+|([0-9]*\.[0-9]+)([eE][-+]?[0-9]+)?)	{
		  yylval.num = atof(yytext); return NUMBER; }

([a-z]|[A-Z]|_)([a-z]|[A-Z]|[0-9]|_)*		{
		  if(strcmp(yytext, "if") == 0)
		    return IF;
		  else if(strcmp(yytext, "elseif") == 0)
		    return ELSEIF;
		  else if(strcmp(yytext, "else") == 0)
		    return ELSE;
		  else if(strcmp(yytext, "for") == 0)
		    return FOR;
		  else if(strcmp(yytext, "include") == 0)
		    return INCLUDE;
		  else
		    yylval.ident = strdup(yytext); return IDENT;
		}

.		{ return yytext[0]; }

%%

