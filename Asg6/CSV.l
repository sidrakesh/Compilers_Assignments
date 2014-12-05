
%{
#include <string.h>
int count = 1;
%}

ws			[ \t]	
commentLine	 	^[#;!](.)*\r\n
field			[^,\n]*[,\n]
%%

{commentLine}					{count++;}

^{ws}*\r\n					{count++;}				
							
{field}						{
							char field[100];
							int i, j;
							
							char last = yytext[yyleng-1];
							yytext[yyleng-1] = '\0';
							
							//Trim spaces
							strcpy(field, yytext);
							i=0;
							while(isspace(field[i])) i++;
							
							if(field[i] == 0)
								strcpy(field, "");
							
							j = strlen(field) - 1;
							while(j > i && isspace(field[j])) j--;
														
							field[j+1] = 0;
							char * str = field + i;
									
							if(strcmp(str,",")==0)
								strcpy(str, "");

							for(i=0;i<strlen(str);++i)		
							{
								if(str[i]==' ')
									break;
							}
							
							if(i==strlen(str))								
								printf("%d %s\n",count,str);		
							else
								printf("Line:%d Error!!Fields cannot contain spaces.\n",count);
								
							if(last=='\n')
								count++;
						}											

<<EOF>>						{return 1;}

%%
