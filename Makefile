qcc : qcc2.o y.tab.o lex.yy.o
	gcc -o qcc y.tab.o lex.yy.o qcc2.o -lfl -lm

qcc2.o : qcc2.c
	gcc -g2 -c -o qcc2.o qcc2.c

lex.yy.o : lex.yy.c y.tab.h qcc.h
	gcc -g2 -c -o lex.yy.o lex.yy.c

lex.yy.c : qcc.l
	lex qcc.l

y.tab.o : y.tab.c y.tab.h qcc.h
	gcc -g2 -c -o y.tab.o y.tab.c

y.tab.c : qcc.y
	yacc -d qcc.y

