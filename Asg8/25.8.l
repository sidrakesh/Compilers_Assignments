
%{
	#include <stdio.h>
	#include <string.h>

	#include "25.8.tab.h"
	int count = 1;
%}

id1								[a-zA-Z][a-zA-Z0-9_]*
id2								_
id3								_[A-Za-z0-9][a-zA-Z0-9_]*		 									

id 								{id1}|{id2}|{id3}

%%

[\n]							{count++;}

[ \t\r]							{;}

"\/\/"(.)*						{;}

and								{return AND;}
def 							{return DEF;}
else							{return ELSE;}
end								{return END;}
float							{return FLOAT;}
global							{return GLOBAL;}
if								{return IF;}
int								{return INT;}
not								{return NOT;}
null							{return NUL;}
or								{return OR;}
print							{return PRINT;}
product							{return PRODUCT;}
read							{return READ;}
return							{return RETURN;}
while							{return WHILE;}


"%d"|"%f"|"%s"					{return FORMAT;}


":="							{return ASSIGN;}
":"								{return COLON;}
","								{return COMMA;}
";"								{return SEMICOLON;}


"+"								{return PLUS;}
"-"								{return MINUS;}
"*"								{return MULT;}
"/"								{return DIV;}
"%"								{return MOD;}
"."								{return DOT;}


"="								{return EQ;}
">="							{return GE;}
">"								{return GT;}
"<="							{return LE;}
"<"								{return LT;}
"<>"							{return NE;}


"("								{return LEFT_PARAEN;}
")"								{return RIGHT_PARAEN;}
"["								{return LEFT_SQ_BKT;}
"]"								{return RIGHT_SQ_BKT;}


-?[0-9]+						{
									yylval.ival = atoi(yytext);
									return INT_CONST;
								}

-?[0-9]+\.[0-9]+				{
									yylval.fval = atof(yytext);
									return FLOAT_CONST;
								}

{id}							{	
									yylval.sval = strdup(yytext);
									return ID;
								}	

.								{;}																

%%