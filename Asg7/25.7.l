
%{
#include <string.h>
#include "y.tab.h"
%}

ws			[ \t]
id			(-)?[0-9]+

%%
{ws}			{;}

{id}			{
				return ID;//fprintf(yyout,"id ");
			}

"["			{
				return L_SQUARE;//fprintf(yyout,"[ ");
			}
"]"			{
				return R_SQUARE;//fprintf(yyout,"] ");
			}
","			{
				return COMMA;//fprintf(yyout,", ");
			}

"X"			{
				return CROSS;//fprintf(yyout,"X ");
			}
"."			{
				return DOT;//fprintf(yyout,". ");
			}
"*"			{
				return MULT;//fprintf(yyout,"* ");
			}
"+"			{
				return PLUS;//fprintf(yyout,"+ ");
			}
"("			{
				return L_PARAEN;//fprintf(yyout,"( ");
			}
")"			{
				return R_PARAEN;//fprintf(yyout,") ");
			}
"#"			{
				return HASH;//fprintf(yyout,"# ");
			}
"NORM"			{
				return NORM;//fprintf(yyout,"NORM ");
			}

.			{
				fclose(yyin);
				remove("inputUser.txt");
				return NOTOK;	
			}

<<EOF>>			{return 1;}
%%
