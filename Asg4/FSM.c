#include<stdio.h>
#include<ctype.h>
#include "def.h"
#include<string.h>
#include<stdlib.h>

/*Structure for token*/
typedef struct _token{
	int tokenID;
	attrType attribute;
}token;

/*structure for storing keywords in the keyword array*/
typedef struct _keyWordID{
	char keyword[10];
	int tokenID;
}keyWordID;

/*keyword array*/
keyWordID keywordArr[] = {{"short",SHORT},{"int",INT},{"float",FLOAT},{"double",DOUBLE},{"bool",BOOL},{"char",CHAR},
						{"signed",SIGNED},{"unsigned",UNSIGNED},{"for",FOR},{"while",WHILE},{"do",DO},{"return",RETURN},
						{"struct",STRUCT},{"const",CONST},{"void",VOID},{"switch",SWITCH},{"break",BREAK},{"case",CASE},
						{"continue",CONTINUE},{"goto",GOTO},{"long",LONG},{"static",STATIC},{"union",UNION},{"default",DEFAULT}};

int lexemeBegin = 0; //lexemeBegin marker for traversing the input

//initializes symbol table by adding all the keywords
void initialize_Symbol_Table()
{
	FILE* fp = fopen("symbol_table_1.out","w");
	
	int i;
	
	for(i=0;i<24;i++)
	{
		fprintf(fp,"%s 0\n",keywordArr[i].keyword);
	}

	fclose(fp);
}

//identifies the keywords and identifiers, returning their tokens
token identifier_keyword(char* input)
{
	FILE* file;
	int i = 1,nflag,marker;
	token t;
	t.tokenID = NOTOK;
	t.attribute.string = '\0';
	char identifier[15];
	
	if(input[lexemeBegin]!='_'&&isalpha(input[lexemeBegin])==0)  //if first symbol is not an underscore or an alphabet, 
		return t;												 //not an identifier or keyword
		
	if(input[lexemeBegin]=='_'&&input[lexemeBegin+1]=='_')		 //two leading underscores
	{
		return t;	
	}
	while(isalnum(input[lexemeBegin+i])!=0||input[lexemeBegin+i]=='_')  //identifier body
		i++;
	
	attrType a;                                        //attribute field
	a.string  = (char*)malloc((i+1)*sizeof(char));
	
	memcpy(a.string,input+lexemeBegin,i);              //copying string to attribute
	a.string[i] = '\0';
	
	int flag = 0;
	
	int j;
	for(j=0;j<24;j++)
	{
		if(strcmp(a.string,keywordArr[j].keyword)==0)    //checking if string detected is a keyword
		{
			t.tokenID = keywordArr[j].tokenID;
			t.attribute.string = '\0';
			flag = 1;
			break;
		}
	}
	
	if(flag==0)   //not a keyword; is an identifier
	{
		t.tokenID = IDNTIFIER;
		
		file = fopen("symbol_table_1.out","r");
		
		nflag = 0;
		
		while(!feof(file))
		{
			fscanf(file,"%s%d",identifier,&marker);
			
			if(strcmp(identifier,a.string)==0)           //if identifer is already present in symbol table
			{
				nflag = 1;
				break;
			}
		}
		fclose(file);
		
		if(nflag==0)               //identifier not present in symbol table
		{
			file = fopen("symbol_table_1.out","a");
			fprintf(file,"%s 1\n",a.string);
			fclose(file);
		}
		t.attribute = a;
	}
	
	lexemeBegin += i;
	
	return t;
}

//for detecting integer constants
token integer_constant(char* input)
{
	char number[12];
	
	token t;
	t.tokenID = NOTOK;
	
	if(isdigit(input[lexemeBegin])==0&&input[lexemeBegin]!='-')  //checking if first character is a sign
		return t;
	
	int i=1;
	
	while(isdigit(input[lexemeBegin+i])!=0)
		i++;
	
	if(input[lexemeBegin+i]=='.')     //number is a floating point number
		return t;
	
	memcpy(number,input+lexemeBegin,i);
	number[i] = '\0';
	
	int num_val = atoi(number);       //converting string to an integer
	t.tokenID = INT_CONST;
	t.attribute.integer = num_val;
	
	lexemeBegin+=i;                   //incrementing lexemeBegin
	
	return t;
}

//for detecting floating point numbers
token float_constant(char* input)
{
	char number[12];   //for storing the number
	
	token t;
	t.tokenID = NOTOK;
	
	if(isdigit(input[lexemeBegin])==0&&input[lexemeBegin]!='-')   //checking for sign
		return t;
	
	int i=1;
	
	while(isdigit(input[lexemeBegin+i])!=0||input[lexemeBegin+i]=='.')  //checking through the body of the number
		i++;
	
	memcpy(number,input+lexemeBegin,i);
	number[i] = '\0';
	
	float num_val = atof(number);   //converting to floating point number

	t.tokenID = FLO_CONST;
	t.attribute.real = num_val;
	
	lexemeBegin+=i;
	
	return t;
}

//for checking the arithmetic operators
token arithmetic_operator(char* input)
{
	token t;
	t.tokenID = NOTOK;
	t.attribute.string = '\0';
	
	//+ symbol
	if(input[lexemeBegin]=='+'&&input[lexemeBegin+1]!='+'&&input[lexemeBegin+1]!='=')
	{
		t.tokenID = PLUS;
		lexemeBegin++;
		return t;
	}
	
	//- symbol
	else if(input[lexemeBegin]=='-'&&input[lexemeBegin+1]!='-'&&input[lexemeBegin+1]!='=')
	{
		t.tokenID = MINUS;
		lexemeBegin++;
		return t;
	}
	
	//++ symbol
	else if(input[lexemeBegin]=='+'&&input[lexemeBegin+1]=='+')
	{
		t.tokenID = PLUS_PLUS;
		lexemeBegin+=2;
		return t;
	}
	
	//-- symbol
	else if(input[lexemeBegin]=='-'&&input[lexemeBegin+1]=='-')
	{
		t.tokenID = MINUS_MINUS;
		lexemeBegin+=2;
		return t;
	}
	
	//* symbol
	else if(input[lexemeBegin]=='*'&&input[lexemeBegin+1]!='=')
	{
		t.tokenID = MULT;
		lexemeBegin++;
		return t;
	}
	
	// / symbol
	else if(input[lexemeBegin]=='/'&&input[lexemeBegin+1]!='='&&input[lexemeBegin+1]!='*')
	{
		t.tokenID = DIV;
		lexemeBegin++;
		return t;
	}
	
	//% symbol
	else if(input[lexemeBegin]=='%'&&input[lexemeBegin+1]!='=')
	{
		t.tokenID = PERCENT;
		lexemeBegin++;
		return t;
	}
	
	return t;
}

//for detecting assignment operator
token assignment_operator(char* input)
{
	token t;
	t.tokenID = NOTOK;
	t.attribute.string = '\0';
	
	//= symbol
	if(input[lexemeBegin]=='='&&input[lexemeBegin+1]!='=')
	{
		t.tokenID = EQ;
		lexemeBegin++;
		return t;
	}
	
	//+= symbol
	else if(input[lexemeBegin]=='+'&&input[lexemeBegin+1]=='=')
	{
		t.tokenID = PLUS_EQ;
		lexemeBegin+=2;
		return t;
	}
	
	//-= symbol
	else if(input[lexemeBegin]=='-'&&input[lexemeBegin+1]=='=')
	{
		t.tokenID = MINUS_EQ;
		lexemeBegin+=2;
		return t;
	}
	
	//*= symbol
	else if(input[lexemeBegin]=='*'&&input[lexemeBegin+1]=='=')
	{
		t.tokenID = MULT_EQ;
		lexemeBegin+=2;
		return t;
	}
	
	// /= symbol
	else if(input[lexemeBegin]=='/'&&input[lexemeBegin+1]=='=')
	{
		t.tokenID = DIV_EQ;
		lexemeBegin+=2;
		return t;
	}
	
	return t;
}

//for detecting relational operator
token relational_operator(char* input)
{
	token t;
	t.tokenID = NOTOK;
	t.attribute.string = '\0';
	
	//< symbol
	if(input[lexemeBegin]=='<'&&input[lexemeBegin+1]!='=')
	{
		t.tokenID = LESSER;
		lexemeBegin++;
		return t;
	}
	
	//<= symbol
	else if(input[lexemeBegin]=='<'&&input[lexemeBegin+1]=='=')
	{
		t.tokenID = LESS_EQ;
		lexemeBegin+=2;
		return t;
	}
	
	//> symbol
	else if(input[lexemeBegin]=='>'&&input[lexemeBegin+1]!='=')
	{
		t.tokenID = GREATER;
		lexemeBegin++;
		return t;
	}

	//>= symbol
	else if(input[lexemeBegin]=='>'&&input[lexemeBegin+1]=='=')
	{
		t.tokenID = GREAT_EQ;
		lexemeBegin+=2;
		return t;
	}
	
	//== symbol
	else if(input[lexemeBegin]=='='&&input[lexemeBegin+1]=='=')
	{
		t.tokenID = EQ_EQ;
		lexemeBegin+=2;
		return t;
	}
	
	return t;
}

//for detecting special symbols
token special_symbol(char* input)
{
	token t;
	t.tokenID = NOTOK;
	t.attribute.string = '\0';
	
	//semi - colon
	if(input[lexemeBegin]==';')
	{
		t.tokenID = SEMICOL;
		lexemeBegin++;
		return t;
	}
	
	//left parenthesis
	else if(input[lexemeBegin]=='(')
	{
		t.tokenID = L_PARAEN;
		lexemeBegin++;
		return t;
	}
	
	//right parenthesis
	else if(input[lexemeBegin]==')')
	{
		t.tokenID = R_PARAEN;
		lexemeBegin++;
		return t;
	}
	
	//comma
	else if(input[lexemeBegin]==',')
	{
		t.tokenID = COMMA;
		lexemeBegin++;
		return t;
	}
	
	//left square parenthesis
	else if(input[lexemeBegin]=='[')
	{
		t.tokenID = L_SQUARE;
		lexemeBegin++;
		return t;
	}
	
	//right square parenthesis
	else if(input[lexemeBegin]==']')
	{
		t.tokenID = R_SQUARE;
		lexemeBegin++;
		return t;
	}
	
	//left curly braces
	else if(input[lexemeBegin]=='{')
	{
		t.tokenID = L_CBRACE;
		lexemeBegin++;
		return t;
	}
	
	//right curly braces
	else if(input[lexemeBegin]=='}')
	{
		t.tokenID = R_CBRACE;
		lexemeBegin++;
		return t;
	}
	return t;
}

//writes the tokens to the output file
void write_token(token t)
{
	FILE* fp = fopen("a4_1.out","a");
	
	if(t.tokenID==INT_CONST)  //if token for integer constant
	{
		fprintf(fp,"%d %d\n",INT_CONST,t.attribute.integer);
	}
	else if(t.tokenID==FLO_CONST)  //if token for floating point number constant
	{
		fprintf(fp,"%d %f\n",FLO_CONST,t.attribute.real);
	}
	else
	{
		if(t.attribute.string!='\0')
			fprintf(fp,"%d %s\n",t.tokenID,t.attribute.string);
		else fprintf(fp,"%d\n",t.tokenID);
	}
	fclose(fp);
}

//for detecting comments
void comment(char* input)
{
	//start of comment
	if(input[lexemeBegin]=='/'&&input[lexemeBegin+1]=='*')
		lexemeBegin+=2;
	else return;
	//end of comment
	while(!(input[lexemeBegin]=='*'&&input[lexemeBegin+1]=='/'))
	{	
		lexemeBegin++;
	}
	
	lexemeBegin+=2;	
}

//to ckeck for an erroneous lexeme
void error_condition(char* input)
{
	while(input[lexemeBegin]!='\0'&&input[lexemeBegin]!=';'&&input[lexemeBegin]!=' ')
		lexemeBegin++;
}

//main function
int main()
{
	initialize_Symbol_Table();
	
	remove("a4_1.out");
	
	char input[1000];
	
	token t;
	int i;
	
	//loop for rading lines of source code into input buffer	
	while(gets(input))
	{
		lexemeBegin = 0;
		
		while(input[lexemeBegin]!='\0'&&input[lexemeBegin]!='\n' &&input[lexemeBegin]!='\r')  //while end of line is not found
		{
			while(input[lexemeBegin]==' ' || input[lexemeBegin] == '\t')
				lexemeBegin++;
						
			//checking for keywords and identifiers
			t = identifier_keyword(input);
			if(t.tokenID!=NOTOK)
			{
				write_token(t);
				continue;
			}
			
			//checking for integer constants
			t = integer_constant(input);
			if(t.tokenID!=NOTOK)
			{
				write_token(t);
				continue;
			}
			
			//checking for floating point number constants
			t = float_constant(input);
			if(t.tokenID!=NOTOK)
			{
				write_token(t);
				continue;
			}

			//checking for arithmetic operators
			t = arithmetic_operator(input);
			if(t.tokenID!=NOTOK)
			{
				write_token(t);
				continue;
			}

			//checking for assignment operators
			t = assignment_operator(input);
			if(t.tokenID!=NOTOK)
			{
				write_token(t);
				continue;
			}

			//checking for relational operators
			t = relational_operator(input);
			if(t.tokenID!=NOTOK)
			{
				write_token(t);
				continue;
			}

			//checking for special symbols
			t = special_symbol(input);
			if(t.tokenID!=NOTOK)
			{
				write_token(t);
				continue;
			}
			
			//checking for comments
			comment(input);
			//checking erroneous lexeme
			error_condition(input);
		}
	}	
}
