
%{
	#include <stdio.h>
	#include <stdlib.h>
	#define YYDEBUG 1
	extern FILE * yyin;	
	extern int count;
	int yydebug = 1;

	void yyerror(const char *s);
%}

%union
{
	int ival;
	float fval;
	char * sval;
}

%left	AND DEF ELSE END FLOAT GLOBAL IF INT NOT NUL OR PRINT PRODUCT READ RETURN WHILE
%token 	FORMAT
%left	ASSIGN COLON COMMA SEMICOLON
%left 	LEFT_PARAEN RIGHT_PARAEN LEFT_SQ_BKT RIGHT_SQ_BKT
%left	EQ GE GT LE LT NE
%left	PLUS MINUS
%left   MULT DIV MOD
%left   DOT

%token <ival> 	INT_CONST
%token <fval>	FLOAT_CONST
%token <sval>	ID

%%
prog:
			GLOBAL declList stmtListO END					{printf("Accepted\n");}

declList:

			| decl declList

decl:
			DEF typeList END

typeList:
			typeList SEMICOLON varList COLON type
			| typeList SEMICOLON typeDef
			| varList COLON type
			| typeDef

varList:
			var COMMA varList
			| var

var:
			ID sizeListO

sizeListO:

			| sizeList

sizeList:
			sizeList LEFT_SQ_BKT INT_CONST RIGHT_SQ_BKT
			| LEFT_SQ_BKT INT_CONST RIGHT_SQ_BKT

type:
			INT
			| FLOAT
			| NUL
			| ID

typeDef:
			ID ASSIGN PRODUCT typeList END

stmtListO :

			| stmtList
			
stmtList:
			stmtList SEMICOLON stmt
			| stmt

stmt:
			assignmentStmt
			| readStmt
			| printStmt
			| ifStmt
			| whileStmt
			| returnStmt

assignmentStmt:
			dotId ASSIGN exp

dotId:
			id
			| id DOT dotId

readStmt:
			READ FORMAT exp

printStmt:
			PRINT FORMAT exp

ifStmt:
			IF bExp COLON stmtList elsePart END

elsePart:

			| ELSE stmtList
			
whileStmt:
			WHILE bExp COLON stmtList END

returnStmt:
			RETURN expO	

expO:

			| exp

id:
			ID indxList0

indxList0:

			| indxList

indxList:
			indxList LEFT_SQ_BKT exp RIGHT_SQ_BKT
			| LEFT_SQ_BKT exp RIGHT_SQ_BKT

bExp:
			bExp OR bExp
			| bExp AND bExp
			| NOT bExp
			| LEFT_PARAEN bExp RIGHT_SQ_BKT
			| exp relOP exp

relOP:
			EQ
			| LE
			| LT
			| GE
			| GT
			| NE

exp:
			exp PLUS exp
			| exp MINUS exp
			| exp MULT exp
			| exp DIV exp 
			| exp MOD exp
			| exp DOT exp
			| LEFT_PARAEN exp RIGHT_PARAEN
			| id
			| LEFT_PARAEN ID COLON actParamList0 RIGHT_PARAEN
			| INT_CONST
			| FLOAT_CONST

actParamList0:

			| actParamList

actParamList:
			actParamList COMMA exp
			| exp							
%%

int main()
{
	FILE *fp = fopen("input.txt", "r");
	if (!fp)
	{
		printf("Cannot open input file!!\n");
		return -1;
	}
	yyin = fp;

	yyparse();
}

void yyerror(const char *s)
{
	printf("Error at line %d  - %s!!\n",count,s);
	exit(0);
}

int yywrap()
{
	return 1;
}