
%{
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include "symbol.h"
	#define YYDEBUG 1
	extern FILE * yyin;	
	extern int count;
	
	void yyerror(const char *s);
	Variable SYMTAB[100];
	int size = 0;
	char ThreeAddress[1000][20][20];//1000 codes.
	int nextInstr = 1;//1 based indexing
	int offset = 0;
%}

%union
{
	int ival;
	int size;
	int instr;
	float fval;
	struct
	{
		char name[100];
		Variable * ptr;
	}attribute_ID;
	struct 
	{
		char vars[20][100];//20 variable list
		int size;//number of variables in this list
	}attribute_varList;
	struct 
	{
		char type[10];
		int width;
	}attribute_type;
	struct
	{
		char addr[100];
		char type[10];
	}attribute_exp;
	struct
	{
		List * truelist;
		List * falselist;
	}attribute_bExp;
	struct
	{
		List * nextlist;
	}attribute_stmt;
	List * nextlist;
	char * sval;
	char * type;
	char string[10];
	char code[10];
	char operator[10];
}

%left 	DEF ELSE END BOOLEAN FLOAT GLOBAL IF INT NOT NUL PRINT READ RETURN WHILE FORMAT
%left	OR AND 
%left	ASSIGN COLON COMMA SEMICOLON
%left 	LEFT_PARAEN RIGHT_PARAEN
%left	EQ GE GT LE LT NE
%left	PLUS MINUS
%left   MULT DIV MOD
%left   DOT

%token <ival> 	INT_CONST
%token <fval>	FLOAT_CONST
%token <attribute_ID> ID

%type <code> prog stmtListO
%type <attribute_varList> varList
%type <string> var
%type <attribute_type> type
%type <operator> relOP
%type <attribute_exp> exp
%type <attribute_bExp> bExp
%type <instr> M
%type <nextlist> stmt stmtList ifStmt elsePart whileStmt N

%%
prog:
			GLOBAL declList stmtListO END									{return 0;}
declList:

			| decl declList

decl:
			DEF typeList END

typeList:
			typeList SEMICOLON varList COLON type 							{
																				int i;
																				for(i=0;i<$3.size;++i)
																				{
																					addInfo(SYMTAB, size, $3.vars[i], $5.type, offset);
																					offset+=$5.width;
																				}
																			}	
			| varList COLON type 											{
																				int i;
																				for(i=0;i<$1.size;++i)
																				{
																					addInfo(SYMTAB, size, $1.vars[i], $3.type, offset);
																					offset+=$3.width;
																				}
																			}			
			
varList:
			var COMMA varList 												{
																				strcpy($$.vars[0], $1);
																				$$.size = 1;
																				int i;
																				for(i=0;i<$3.size;++i)
																				{
																					strcpy($$.vars[$$.size], $3.vars[i]);
																					$$.size++;
																				}
																			}
			| var 															{
																				strcpy($$.vars[0], $1);
																				$$.size = 1;
																			}		

var:
			ID 																{strcpy($$, $1.name);}

type:
			INT 															{strcpy($$.type, "int");$$.width = 2;}
			| FLOAT 														{strcpy($$.type, "float");$$.width = 4;}
			| BOOLEAN 														{strcpy($$.type, "boolean");$$.width = 1;}	
			| NUL 															{strcpy($$.type, "null");$$.width = 0;}	

stmtListO :
																			{;}
			| stmtList 														{;}
			
stmtList:
			stmtList SEMICOLON stmt
			| stmt

stmt:
			assignmentStmt 													{$$ = NULL;}									
			| readStmt 														{$$ = NULL;}
			| printStmt 													{$$ = NULL;}
			| ifStmt 														{
																				$$ = $1; 
																				backpatch(ThreeAddress, $$, nextInstr);
																			}
			| elsePart 														{
																				$$ = $1; 
																				backpatch(ThreeAddress, $$, nextInstr);
																			}
			| whileStmt 													{
																				$$ = $1; 
																				backpatch(ThreeAddress, $$, nextInstr);
																			}												

assignmentStmt:
			ID ASSIGN exp 													{
																				char * typeLeft = getType(SYMTAB, size, $1.name);
																				if(strcmp($3.type, "ERROR")==0)
																					printf("Line %d: Cannot do assignment due to errors in rvalue\n", count);
																				else if(strcmp(typeLeft, $3.type)==0 ||(strcmp(typeLeft, "boolean")==0 && strcmp($3.type, "int")==0))
																				{																			
																					strcpy(ThreeAddress[nextInstr][0], ":=");	
																					strcpy(ThreeAddress[nextInstr][1], $3.addr);
																					strcpy(ThreeAddress[nextInstr][2], "");
																					strcpy(ThreeAddress[nextInstr][3], $1.name);
																					nextInstr++;
																				}
																				else
																				{
																					if(strcmp(typeLeft, "float")==0 && strcmp($3.type,"int")==0)
																					{
																						char a1[10];
																						strcpy(a1, widen($3.addr, $3.type, typeLeft));
																						strcpy(ThreeAddress[nextInstr][0], ":=");	
																						strcpy(ThreeAddress[nextInstr][1], a1);
																						strcpy(ThreeAddress[nextInstr][2], "");
																						strcpy(ThreeAddress[nextInstr][3], $1.name);
																						nextInstr++;
																					}
																					else
																					{
																						;//Error
																					}


																				}
																			}									

readStmt:
			READ FORMAT exp 												{
																				strcpy(ThreeAddress[nextInstr][0], "param");
																				strcpy(ThreeAddress[nextInstr][1], $3.addr);
																				strcpy(ThreeAddress[nextInstr][2], "");
																				strcpy(ThreeAddress[nextInstr][3], "");
																				nextInstr++;
																				strcpy(ThreeAddress[nextInstr][0], "call");
																				strcpy(ThreeAddress[nextInstr][1], "read,1");
																				strcpy(ThreeAddress[nextInstr][2], "");
																				strcpy(ThreeAddress[nextInstr][3], "");
																				nextInstr++;
																			}

printStmt:
			PRINT FORMAT exp 												{
																				strcpy(ThreeAddress[nextInstr][0], "param");
																				strcpy(ThreeAddress[nextInstr][1], $3.addr);
																				strcpy(ThreeAddress[nextInstr][2], "");
																				strcpy(ThreeAddress[nextInstr][3], "");
																				nextInstr++;
																				strcpy(ThreeAddress[nextInstr][0], "call");
																				strcpy(ThreeAddress[nextInstr][1], "print,1");
																				strcpy(ThreeAddress[nextInstr][2], "");
																				strcpy(ThreeAddress[nextInstr][3], "");
																				nextInstr++;
																			}
ifStmt:
			IF bExp M COLON stmtList END 									{
																				backpatch(ThreeAddress, $2.truelist, $3);
																				$$ = merge($2.falselist, $5);
																			}

elsePart:
			IF bExp M COLON stmtList N ELSE M stmtList END					{
																				backpatch(ThreeAddress, $2.truelist, $3);
																				backpatch(ThreeAddress, $2.falselist, $8);
																				List * temp = merge($5, $6);
																				$$ = merge(temp, $9);
																			}															
			
whileStmt:
			WHILE M bExp M COLON stmtList END 								{
																				backpatch(ThreeAddress, $6, $2);
																				backpatch(ThreeAddress, $3.truelist, $4);
																				$$ = $3.falselist;
																				strcpy(ThreeAddress[nextInstr][0], "goto");
																				strcpy(ThreeAddress[nextInstr][1], "");
																				strcpy(ThreeAddress[nextInstr][2], "");
																				sprintf(ThreeAddress[nextInstr][3], "%d", $2);
																				nextInstr++;
																			}

bExp:
			bExp OR M bExp 													{
																				backpatch(ThreeAddress, $1.falselist, $3);
																				$$.truelist = merge($1.truelist, $4.truelist);
																				$$.falselist = $4.falselist;
																			}
			| bExp AND M bExp 												{
																				backpatch(ThreeAddress, $1.truelist, $3);
																				$$.truelist = $4.truelist;
																				$$.falselist = merge($1.falselist, $4.falselist);
																			}
			| NOT bExp 														{
																				$$.truelist = $2.falselist;
																				$$.falselist = $2.truelist;
																			}
			| LEFT_PARAEN bExp RIGHT_PARAEN 								{$$.truelist = $2.truelist;$$.falselist = $2.falselist;}
			| exp relOP exp 												{
																				$$.truelist = makelist(nextInstr);
  																				$$.falselist = makelist(nextInstr+1);
  																				strcpy(ThreeAddress[nextInstr][0], "if");
  																				char temp[100];
  																				sprintf(temp, "%s %s %s", $1.addr, $2, $3.addr);
  																				strcpy(ThreeAddress[nextInstr][1], temp);
  																				strcpy(ThreeAddress[nextInstr][2], "goto");
  																				nextInstr++;  	
																				strcpy(ThreeAddress[nextInstr][0], "goto");
																				strcpy(ThreeAddress[nextInstr][1], "");
																				strcpy(ThreeAddress[nextInstr][2], "");
																				nextInstr++;
																			}

M:
																			{$$ = nextInstr;}

N:																			{
																				$$ = makelist(nextInstr);
																				strcpy(ThreeAddress[nextInstr][0], "goto");
																				strcpy(ThreeAddress[nextInstr][1], "");
																				strcpy(ThreeAddress[nextInstr][2], "");
																				nextInstr++;
																			}	

relOP:
			EQ 																{strcpy($$, "=");}
			| LE 															{strcpy($$, "<=");}
			| LT 															{strcpy($$, "<");}
			| GE 															{strcpy($$, ">=");}
			| GT 															{strcpy($$, ">");}
			| NE 															{strcpy($$, "<>");}

exp:
			exp PLUS exp 													{
																				char * temp;
																				temp = maxType($1.type, $3.type);
																				if(temp == NULL)
																				{
																					printf("Line %d: Invalid Types %s and %s!!\n", count, $1.type, $3.type);
																					strcpy($$.type, "ERROR");
																				}
																				else
																				{
																					strcpy($$.type, temp);																					
																					char a1[100], a2[100];
																					strcpy(a1, widen($1.addr, $1.type, $$.type));
																					strcpy(a2, widen($3.addr, $3.type, $$.type));
																					strcpy($$.addr, newTemp($$.type));
																				
																					strcpy(ThreeAddress[nextInstr][0], "+");
																					strcpy(ThreeAddress[nextInstr][1], a1);
																					strcpy(ThreeAddress[nextInstr][2], a2);
																					strcpy(ThreeAddress[nextInstr][3], $$.addr);
																					nextInstr++;
																				}
																			}
			| exp MINUS exp 												{
																				char * temp;
																				temp = maxType($1.type, $3.type);
																				if(temp == NULL)
																				{
																					printf("Line %d: Invalid Types %s and %s!!\n", count, $1.type, $3.type);
																					strcpy($$.type, "ERROR");
																				}
																				else
																				{
																					strcpy($$.type, temp);																					
																					char a1[100], a2[100];
																					strcpy(a1, widen($1.addr, $1.type, $$.type));
																					strcpy(a2, widen($3.addr, $3.type, $$.type));
																					strcpy($$.addr, newTemp($$.type));
																				
																					strcpy(ThreeAddress[nextInstr][0], "-");
																					strcpy(ThreeAddress[nextInstr][1], a1);
																					strcpy(ThreeAddress[nextInstr][2], a2);
																					strcpy(ThreeAddress[nextInstr][3], $$.addr);
																					nextInstr++;
																				}
																			}
			| exp MULT exp 													{
																				char * temp;
																				temp = maxType($1.type, $3.type);
																				if(temp == NULL)
																				{
																					printf("Line %d: Invalid Types %s and %s!!\n", count, $1.type, $3.type);
																					strcpy($$.type, "ERROR");
																				}
																				else
																				{
																					strcpy($$.type, temp);																					
																					char a1[100], a2[100];
																					strcpy(a1, widen($1.addr, $1.type, $$.type));
																					strcpy(a2, widen($3.addr, $3.type, $$.type));
																					strcpy($$.addr, newTemp($$.type));
																				
																					strcpy(ThreeAddress[nextInstr][0], "*");
																					strcpy(ThreeAddress[nextInstr][1], a1);
																					strcpy(ThreeAddress[nextInstr][2], a2);
																					strcpy(ThreeAddress[nextInstr][3], $$.addr);
																					nextInstr++;
																				}
																			}	
			| exp DIV exp 													{
																				char * temp;
																				temp = maxType($1.type, $3.type);
																				if(temp == NULL)
																				{
																					printf("Line %d: Invalid Types %s and %s!!\n", count, $1.type, $3.type);
																					strcpy($$.type, "ERROR");
																				}
																				else
																				{
																					strcpy($$.type, temp);																					
																					char a1[100], a2[100];
																					strcpy(a1, widen($1.addr, $1.type, $$.type));
																					strcpy(a2, widen($3.addr, $3.type, $$.type));
																					strcpy($$.addr, newTemp($$.type));
																				
																					strcpy(ThreeAddress[nextInstr][0], "/");
																					strcpy(ThreeAddress[nextInstr][1], a1);
																					strcpy(ThreeAddress[nextInstr][2], a2);
																					strcpy(ThreeAddress[nextInstr][3], $$.addr);
																					nextInstr++;
																				}
																			}
			| exp MOD exp 													{
																				char * temp;
																				temp = maxType($1.type, $3.type);
																				if(temp == NULL)
																				{
																					printf("Line %d: Invalid Types %s and %s!!\n", count, $1.type, $3.type);
																					strcpy($$.type, "ERROR");
																				}
																				else
																				{
																					strcpy($$.type, temp);																					
																					char a1[100], a2[100];
																					strcpy(a1, widen($1.addr, $1.type, $$.type));
																					strcpy(a2, widen($3.addr, $3.type, $$.type));
																					strcpy($$.addr, newTemp($$.type));
																				
																					strcpy(ThreeAddress[nextInstr][0], "%");
																					strcpy(ThreeAddress[nextInstr][1], a1);
																					strcpy(ThreeAddress[nextInstr][2], a2);
																					strcpy(ThreeAddress[nextInstr][3], $$.addr);
																					nextInstr++;
																				}	
																			}
			| exp DOT exp 													{
																				char * temp;
																				temp = maxType($1.type, $3.type);
																				if(temp == NULL)
																				{
																					printf("Line %d: Invalid Types %s and %s!!\n", count, $1.type, $3.type);
																					strcpy($$.type, "ERROR");
																				}
																				else
																				{
																					strcpy($$.type, temp);																					
																					char a1[100], a2[100];
																					strcpy(a1, widen($1.addr, $1.type, $$.type));
																					strcpy(a2, widen($3.addr, $3.type, $$.type));
																					strcpy($$.addr, newTemp($$.type));
																				
																					strcpy(ThreeAddress[nextInstr][0], ".");
																					strcpy(ThreeAddress[nextInstr][1], a1);
																					strcpy(ThreeAddress[nextInstr][2], a2);
																					strcpy(ThreeAddress[nextInstr][3], $$.addr);
																					nextInstr++;
																				}
																			}	
			| LEFT_PARAEN exp RIGHT_PARAEN 									{strcpy($$.addr, $2.addr);strcpy($$.type, $2.type);}
			| ID 															{strcpy($$.addr, $1.name);strcpy($$.type, getType(SYMTAB, size, $1.name));}
			| INT_CONST 													{sprintf($$.addr, "%d", $1);strcpy($$.type, "int");}
			| FLOAT_CONST                  									{sprintf($$.addr, "%f", $1);strcpy($$.type, "float");}

%%

//returns index of variable if found, otherwise -1.
int isInSYMTAB(Variable SYMTAB[], int size, char id[])
{
	int i;
	for(i=0;i<size;++i)
	{
		if(strcmp(SYMTAB[i].id, id)==0)
		{	
			return i;
		}
	}
	return -1;
}

//returns pointer to this variable in symbol table.
Variable * insertSYMTAB(Variable SYMTAB[], int * size, char id[])
{
	int i;
	Variable temp;
	strcpy(temp.id, id);
	strcpy(temp.type, "");
	temp.offset = 0;
	
	SYMTAB[*size] = temp;
	(*size)++;
	return SYMTAB+(*size)-1;	
}

//Adds type and offset information to id.
void addInfo(Variable SYMTAB[], int size, char id[], char type[], int offset)
{
	int i;
	for(i=0;i<size;++i)
	{
		if(strcmp(SYMTAB[i].id, id)==0)
		{
			strcpy(SYMTAB[i].type, type);
			SYMTAB[i].offset = offset;
			return;
		}
	}
}

//Returns type of id.
char * getType(Variable SYMTAB[], int size, char id[])
{
	int i;
	for(i=0;i<size;++i)
	{
		if(strcmp(SYMTAB[i].id, id)==0)
			return SYMTAB[i].type;
	}
} 

//Returns the bigger of type1 and type2.
char * maxType(char type1[], char type2[])
{
	char * temp = (char *)malloc(10 * sizeof(char));
	if(strcmp(type1, "int")!=0 && strcmp(type1, "float")!=0)
		return NULL;
	if(strcmp(type2, "int")!=0 && strcmp(type2, "float")!=0)
		return NULL;
	if(strcmp(type1, "int")==0 && strcmp(type2, "int")==0)
	{
		strcpy(temp, "int");
		return temp;
	}
	else if(strcmp(type1, "float")==0 || strcmp(type2, "float")==0)
	{
		strcpy(temp, "float");
		return temp;
	}
	else
	{
		return NULL;//Error
	}
}

//Creates a new temporary variable, inserts it in symbol table and returns it.
char * newTemp(char type[])
{
	static int n = 1;
	char* temp = (char*)malloc(10 * sizeof(char));
	sprintf(temp,"t%d", n);
	n++;
	insertSYMTAB(SYMTAB, &size, temp);
	addInfo(SYMTAB, size, temp, type, offset);
	if(strcmp(type, "int")==0)
		offset+=2;
	else if(strcmp(type, "float")==0)
		offset+=4;

	return temp;
}

//Widen a smaller type 'typeFrom' to a bigger type 'typeTo'.
char * widen(char a[], char typeFrom[], char typeTo[])
{
	if(strcmp(typeFrom, typeTo)==0)
		return a;
	else if(strcmp(typeFrom, "int")==0 && strcmp(typeTo, "float")==0)
	{
		char * temp = (char *)malloc(10 * sizeof(char));
		temp = newTemp("float");
		strcpy(ThreeAddress[nextInstr][0], "(float)");
		strcpy(ThreeAddress[nextInstr][1], a);
		strcpy(ThreeAddress[nextInstr][2], "");
		strcpy(ThreeAddress[nextInstr][3], temp);
		nextInstr++;
		return temp;
	}
	else
		return NULL;//Error
}

//Backpatch the lines conatined in l to the label p.
void backpatch( char ThreeAddress[][20][20], List * l, int p)
{
	char ch[20];
	while(l!=NULL)
	{
		sprintf(ch,"%d",p);
		strcpy(ThreeAddress[l->line][3], ch);
		l=l->next;
	}
}		 

//Merge 2 lists l1 and l2.
List * merge (List * l1, List * l2)
{
	if(l1==NULL)
		return l2;
	if(l2==NULL)
		return l1;

	List * l = l1;
	List * temp=l;
	while(temp->next!=NULL)
		temp=temp->next;
	temp->next= l2;

	return l;
}	

//Create a new list.
List * makelist(int p)
{
	List * l = (List *)malloc(sizeof(List));
	l->line = p;
	l->next=NULL;
	return l;
}	 

void printThreeAddressCodeToFile(char ThreeAddress[][20][20], int nextInstr)
{
	int i,j;
	FILE * fp = fopen("output.txt", "w");
	if(fp==NULL)
	{
		printf("Could not open file!!\n");
		exit(-1);
	}
	
	for(i = 1;i < nextInstr;++i)
	{
		for(j = 0;j<4;j++)
		{
			if(ThreeAddress[i][j]!=NULL)
				fprintf(fp, "%s\t\t", ThreeAddress[i][j]);
			else
				fprintf(fp, "\t\t");
		}
		fprintf(fp, "\n");		
	}
	fclose(fp);
}

void printSymbolTableToFile(Variable SYMTAB[], int size)
{
	int i;
	FILE * fp = fopen("symtab.txt", "w");
	if(fp==NULL)
	{
		printf("Could not open file!!\n");
		exit(-1);
	}

	for(i=0;i<size;++i)
	{
		fprintf(fp, "%s\t\t%s\t\t%d\n",SYMTAB[i].id, SYMTAB[i].type, SYMTAB[i].offset);
	}
	fclose(fp);
}

int main()
{
	yyparse();
	printThreeAddressCodeToFile(ThreeAddress, nextInstr);
	printSymbolTableToFile(SYMTAB, size);
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