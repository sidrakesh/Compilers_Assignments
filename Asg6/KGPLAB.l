
%{
#include <string.h>
#include <ctype.h>
#include "symbol.h"
int count = 1;
%}

letter 			[A-Za-z_]
digit			[0-9]
digits			{digit}+	
id			{letter}({letter}|{digit})*
ws			[ \t]
Initialize		[,: \t0-9]+
	
SingleDefine		{id}{ws}*\[{ws}*{digits}{ws}*,{ws}*{digits}{ws}*\]
Define			{ws}*(DEF){ws}+{SingleDefine}({ws}*,{ws}*{SingleDefine})*{ws}*;

ADD			{ws}*(ADD){ws}+{id}{ws}*,{ws}*{id}{ws}*,{ws}*{id}{ws}*;
MMUL			{ws}*(MMUL){ws}+{id}{ws}*,{ws}*{id}{ws}*,{ws}*{id}{ws}*;	
CMUL			{ws}*(CMUL){ws}+{id}{ws}*,{ws}*{id}{ws}*,{ws}*{digits}{ws}*;
PRINT			{ws}*(PRINT){ws}+{id}{ws}*;

%%

\n				{
				        printf("\n");
					count++;
				}

{ws}*"//"(.)*	{;}

{Define}  	 	        {
					int i;
					char id[100]="", temp[2];
					int row, col;
					int flag = 0;
					
					i=0;
					while(yytext[i]==' ' || yytext[i]=='\t')
						++i;
					
					i+=3;	
					printf("<DEF>");		
					while(1)
					{
						while(yytext[i]==' ' || yytext[i]=='\t')
						++i;

						while(yytext[i]!=' ' && yytext[i]!='\t' && yytext[i]!='[')
						{
							temp[0] = yytext[i];
							temp[1] = '\0';
							strcat(id, temp);
							++i;	
						}
						printf("<ID>");
						while(yytext[i]==' ' || yytext[i]=='\t')
						++i;
					
						printf("<SQBKTO>");
						++i;
							
						while(yytext[i]==' ' || yytext[i]=='\t')
							++i;
							
						row = 0;
						while(isdigit(yytext[i]))		
						{
							row*=10;
							row+=yytext[i]-48;
							++i;
						}	
						printf("<INT>");	
						while(yytext[i]==' ' || yytext[i]=='\t')
							++i;
							
						printf("<COMMA>");
						++i;
							
						while(yytext[i]==' ' || yytext[i]=='\t')
							++i;
								
						col = 0;
						while(isdigit(yytext[i]))		
						{
							col*=10;
							col+=yytext[i]-48;
							++i;
						}	
						printf("<INT>");	
						
						while(yytext[i]==' ' || yytext[i]=='\t')
							++i;							
							
						printf("<SQBKTC>");
						++i;
						
						while(yytext[i]==' ' || yytext[i]=='\t')
							++i;
							
						/*Insertion in SYMTAB can be done*/
						if(!isInSYMTAB(id))							
						insertSYMTAB(id, row, col);
							
						else//error
						;
						
						strcpy(id, "");	
						//////////////////////////////////
						
						if(yytext[i]==',')
						{
							printf("<COMMA>");
							++i;
						}
						else
						{
							printf("<ENDS>");
							break;
						}							
					}
					
				}
				
{ws}*"INIT"{ws}+{id}{ws}*\[[0-9,: \t]+\];				{
					
					int i;
					char id[100]="", temp[2];
					int r1,r2,c1,c2,error_flag;
					i = 0;
					while(yytext[i]==' ' || yytext[i]=='\t')
						++i;
					
					i+=4;
					printf("<INIT>");
					while(yytext[i]==' ' || yytext[i]=='\t')
						++i;
					
					while(yytext[i]!=' ' && yytext[i]!='\t' && yytext[i]!='[')
					{
						temp[0] = yytext[i];
						temp[1] = '\0';
						strcat(id, temp);
						++i;	
					}
					printf("<ID>");
					
					if(isInSYMTAB(id))
						getDimension(id, &r1, &c1);
					else//error
						;
						
					while(yytext[i]==' ' || yytext[i]=='\t')
						++i;
					
					printf("<SQBKTO>");
						++i;
								
					error_flag=0;
					r2=c2=1;
					while(yytext[i]!=']')
					{
						while(yytext[i]==' ' || yytext[i]=='\t')
							++i;
							
						while(isdigit(yytext[i]))		
							++i;
						printf("<INT>");	
						while(yytext[i]==' ' || yytext[i]=='\t')
							++i;
						if(yytext[i]==',')
						{
							printf("<COMMA>");
							c2++;
							++i;
						}	
						if(yytext[i]==':')//Do Validation here
						{
							printf("<ENDR>");
							r2++;
							if(c2!=c1)
								error_flag = 1;
							c2=1;//Next row count starts	
							++i;
						}	
					}
					printf("<SQBKTC><ENDS>");

					if(c2!=c1)
						error_flag = 1;
					if(r2!=r1)
						error_flag = 1;
						
					if(error_flag==1)
						printf("Line:%d - Dimension does not match definition!!", count);			
				}				

{ADD}				{
					printf("<ADD><ID><COMMA><ID><COMMA><ID><ENDS>");
					int i, j;
					char id[3][100];
					char temp[2];
					i = 0;
					while(yytext[i]==' ' || yytext[i]=='\t')
						++i;
					i+=3;
					
					strcpy(id[0], "");
					strcpy(id[1], "");
					strcpy(id[2], "");
					
					for(j=0;j<3;++j)
					{
						while(yytext[i]==' ' || yytext[i]=='\t')
							++i;
					
						while(yytext[i]!=' ' && yytext[i]!='\t'&& yytext[i]!=',' && yytext[i]!=';')
						{
							temp[0] = yytext[i];
							temp[1] = '\0';
							strcat(id[j], temp);
							++i;	
						}
						while(yytext[i]==' ' || yytext[i]=='\t')
							++i;
					
						if(yytext[i]==',')
							++i;
						else 
							++i;						
					}
					int r1,r2,r3,c1,c2,c3;
					if(isInSYMTAB(id[0]))
					getDimension(id[0], &r1, &c1);
					if(isInSYMTAB(id[1]))
					getDimension(id[1], &r2, &c2);
					if(isInSYMTAB(id[2]))
					getDimension(id[2], &r3, &c3);
					
					if(r1!=r2 || r2!=r3 ||c1!=c2 || c2!=c3)
						printf("Line:%d - Incorrect Dimensions for ADD!!", count);			
				}

{MMUL}				{
					printf("<MMUL><ID><COMMA><ID><COMMA><ID><ENDS>");
					int i, j;
					char id[3][100];
					char temp[2];
					i = 0;
					while(yytext[i]==' ' || yytext[i]=='\t')
						++i;
					i+=4;
					
					strcpy(id[0], "");
					strcpy(id[1], "");
					strcpy(id[2], "");
					
					for(j=0;j<3;++j)
					{
						while(yytext[i]==' ' || yytext[i]=='\t')
							++i;
					
						while(yytext[i]!=' ' && yytext[i]!='\t'&& yytext[i]!=',' && yytext[i]!=';')
						{
							temp[0] = yytext[i];
							temp[1] = '\0';
							strcat(id[j], temp);
							++i;	
						}
						while(yytext[i]==' ' || yytext[i]=='\t')
							++i;
					
						if(yytext[i]==',')
							++i;
						else 
							++i;
					}
					int r1,r2,r3,c1,c2,c3;
					if(isInSYMTAB(id[0]))
					getDimension(id[0], &r1, &c1);
					if(isInSYMTAB(id[1]))
					getDimension(id[1], &r2, &c2);
					if(isInSYMTAB(id[2]))
					getDimension(id[2], &r3, &c3);
					
					if(c1!=r2 ||r3!=r1 || c3!=c2)
						printf("Line:%d - Incorrect Dimensions for MMUL!!", count);					
				}

{CMUL}				{
					printf("<CMUL><ID><COMMA><ID><COMMA><INT><ENDS>");
					int i, j;
					char id[2][100];
					char temp[2];
					
					i=0;
					while(yytext[i]==' ' || yytext[i]=='\t')
						++i;
					i+=4;
										
					strcpy(id[0], "");
					strcpy(id[1], "");
					
					for(j=0;j<2;++j)
					{
						while(yytext[i]==' ' || yytext[i]=='\t')
							++i;
					
						while(yytext[i]!=' ' && yytext[i]!='\t'&& yytext[i]!=',')
						{
							temp[0] = yytext[i];
							temp[1] = '\0';
							strcat(id[j], temp);
							++i;	
						}
						
						while(yytext[i]==' ' || yytext[i]=='\t')
							++i;
					
						++i;						
					}
					int r1,r2,c1,c2;
					if(isInSYMTAB(id[0]))
					getDimension(id[0], &r1, &c1);
					if(isInSYMTAB(id[1]))
					getDimension(id[1], &r2, &c2);
					
					if(r1!=r2 ||c1!=c2)
						printf("Line:%d - Incorrect Dimensions for CMUL!!", count);
				}

{PRINT}				{					
					printf("<PRINT><ID><ENDS>");					
				}				
				
<<EOF>>				{return 1;}

%%
