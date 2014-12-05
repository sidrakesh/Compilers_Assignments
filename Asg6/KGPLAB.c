#include<stdio.h>

extern int count;
extern FILE *yyin;

int main()
{
	FILE *fp;
	fp = fopen("KGPLAB_input.txt", "r+");
	yyin = fp;
	
	yylex();
	
	yywrap();

	return 1;
}

int yywrap()
{
	printf("\nEnd of the input file\n");
        return 1;
}




