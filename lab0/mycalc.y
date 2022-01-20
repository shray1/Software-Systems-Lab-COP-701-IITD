%{
int yyerror (const char *s);
int yylex();
#include <stdio.h>
#include<math.h>

//#define YYSTYPE float
%} 
//declarations of union,type and tokens.
%token NUM SEMICOLON
%token ERROR
//declare associativity of operators
%left SUB
%left ADD
%left MUL
%left DIV
%right POW 
%%

/* descriptions of expected inputs corresponding actions (in C) */
input	: line SEMICOLON	{ printf("%d\n",$1);}
	| input line SEMICOLON	{ printf("%d\n",$2);}
	;
line	: number ADD number 	{$$=$1+$3;}
	| number MUL number	{$$=$1*$3;}
	| number DIV number	{$$=$1/$3;}
	| number SUB number	{$$=$1-$3;}
	| number POW number	{$$=$1*$3; 
				 /*pow($1,$3)
				 compilation error here. Debug it.
				*/}
	| ERROR 	{fprintf(stderr, "\nINPUT ENTERED IN BAD FORMAT\n");}
	;
number	: NUM		{$$=$1;}
	| SUB NUM	{$$=-$2;}
	;

%%       
/* C code */
int main (void) {
	//printf("User: ");
	return yyparse ();
}

int yyerror (const char *s) {
	fprintf (stderr, "%s\n", s);
} 
