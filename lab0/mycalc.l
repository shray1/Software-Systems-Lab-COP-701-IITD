%option nounput
%{
#include "y.tab.h"

%}
%%
[ \t\n]	{;}
[;]	{return SEMICOLON;}
[-]	{return SUB;}
[+]	{ return ADD;}
[*]	{return MUL;}
"\/"	{ return DIV;}
[\^]	{return POW;}
"shray"	{return SEMICOLON;}
[0-9]+	{yylval= atoi(yytext); return NUM;}
.	{return ERROR;}
%%

int yywrap (void) {return 1;}


// Why is the curser after User going to next line. 
// Why are we not able to parse single division slash.
