
%{
	#include <stdio.h>
	#include <string.h>

	#include "symbol.h"
	#include "25.9.tab.h"
	int count = 1;
	extern Variable SYMTAB[];
	extern int size; 
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
boolean							{return BOOLEAN;}
float							{return FLOAT;}
global							{return GLOBAL;}
if								{return IF;}
int								{return INT;}
not								{return NOT;}
null							{return NUL;}
or								{return OR;}
print							{return PRINT;}
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


-?[0-9]+						{
									yylval.ival = atoi(yytext);
									return INT_CONST;
								}

-?[0-9]+\.[0-9]+				{
									yylval.fval = atof(yytext);
									return FLOAT_CONST;
								}

{id}							{	
									char * name = (char *)malloc(10 * sizeof(char));
									name = strdup(yytext);
									int pos = isInSYMTAB(SYMTAB, size, name);
									if(pos==-1)
									{
										Variable * ptr = insertSYMTAB(SYMTAB, &size, name);
										strcpy(yylval.attribute_ID.name, name);
										yylval.attribute_ID.ptr = ptr;
										return ID;
									}
									else
									{
										strcpy(yylval.attribute_ID.name, name);
										yylval.attribute_ID.ptr = SYMTAB + pos;
										return ID;
									}
								}	

.								{;}																

%%