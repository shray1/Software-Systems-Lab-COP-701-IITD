calculator : lex.yy.c  y.tab.h y.tab.c
	gcc lex.yy.c  y.tab.h y.tab.c -o $@ 
lex.yy.c : y.tab.h mycalc.l
	lex mycalc.l
y.tab.h y.tab.c : mycalc.y
	yacc -d mycalc.y
clean : 
	-@rm -f *.o *.c *.h

