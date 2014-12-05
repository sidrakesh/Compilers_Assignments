#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"y.tab.h"

char stack[100][10];  //parsing stack
int top;

char parseTable[30][30][10][10];  //the final parse table
char firstSet[30][10];	//first set
char followSet[30][10];	//follow set
char input[100][10]; //input
char startSymbol[10]; //start symbol

int terminalNum;  //number of terminals in the parse table
int firstNum;   //current number of elements in first set
int followNum;	//current number of elements in follow set
int inputSize;

void addTerminals();   //function to add terminals to the parse table
int terminalInTable();	//to determine whether a terminal is already present in the parse table
void constructParsingTable();	//function to construct the parse table

void parseProcess(char input[100][10])
{
	top = 1;
	
	strcpy(stack[0],"$");  //beginning with end marker symbol
	strcpy(stack[1],startSymbol);	//start symbol

	int i = 0,j,k,p,q,r;		//input string iterator
	r = 0;
	
	printf("\n");
	
	while(strcmp(stack[top],"$")!=0)
	{	
		if(strcmp(stack[top],input[i])==0)  //leading stack element is the same as the current input element
		{
			top--;
			i++;
		}
		else if(terminalInTable(stack[top])==1)
		{
			printf("\nError : No further suitable derivations could be performed for accepting input string\n\n");
			exit(0);
		}
		else
		{
			j=1;
			
			while(strcmp(parseTable[j][0][0],stack[top])!=0)
				j++;
				
			k = 1;
			
			while(strcmp(parseTable[0][k][0],input[i])!=0)
				k++;

			if(strcmp(parseTable[j][k][0],"\0")==0)   //no further derivation steps can be applied
			{
				printf("\nError : No further derivations can be performed\n\n");
				exit(0);
			}
			p = 0;
			
			if(strcmp(parseTable[j][k][0],"e")==0)  //e transition
			{
				printf("%s -> e\n",parseTable[j][0][0]);
				top--;
			}
			else  //production added to stack
			{
				while(strcmp(parseTable[j][k][p],"\0")!=0)
					p++;
			
				p--;
			
				while(p>0)
				{
					strcpy(stack[top],parseTable[j][k][p]);
					p--;
					top++;
				}
			
				strcpy(stack[top],parseTable[j][k][0]);
				
				p = 0;
				
				printf("%s -> ",parseTable[j][0][0]);
				
				while(strcmp(parseTable[j][k][p],"\0")!=0)
				{
					printf("%s ",parseTable[j][k][p]);
					p++;
				}
				printf("\n");
			}
		}
	}
	
	if(strcmp(input[i],"$")!=0) //stack became empty before input could be fully scanned
		printf("\nError : No suitable derivations could be performed for accepting input string\n\n");
	else
		printf("\nGiven string is accepted by the grammar\n\n");
}

//function to get the first and follow sets for a given non-terminal
void getFirstSet(char str[])
{
	FILE* fp;
	
	int i;
	
	for(i=1;i<=terminalNum;i++)   //if the string is a terminal
		if(strcmp(parseTable[0][i][0],str)==0)
		{
			strcpy(firstSet[0],str);
			firstNum = 1;
			return;
		}
	
	if(strcmp(str,"e")==0)  //if string is e
	{
		strcpy(firstSet[0],str);
		firstNum = 1;
		return;
	}
	
	firstNum = 0;   //number of elements in first set
	
	fp = fopen("fst-fol.txt","r");
	char line[150];
	char* pch;	
	
	while(fgets(line,100,fp))
	{
		pch = strtok(line," :\t\r\n");
		
		if(strcmp(pch,str)!=0)
			continue;
		
		while(strcmp(pch,"|")!=0)
		{
			strcpy(firstSet[firstNum],pch);
			firstNum++;
			
			pch = strtok(NULL," :\t\r\n");
		}
		
		break;
	}
	
	fclose(fp);
}

//function to get the first and follow sets for a given non-terminal
void getFollowSet(char str[])
{
	FILE* fp;
	
	int i;
	
	followNum = 0;
	
	fp = fopen("fst-fol.txt","r");
	char line[150];
	char* pch;	
	
	while(fgets(line,100,fp))
	{		
		pch = strtok(line," :\t\r\n");
		
		if(strcmp(pch,str)!=0)
			continue;
		
		while(strcmp(pch,"|")!=0)
		{
			pch = strtok(NULL," :\t\r\n");
		}
		
		pch = strtok(NULL," :\t\r\n");
		
		while(pch!=NULL)
		{
			strcpy(followSet[followNum],pch);
			followNum++;
			
			pch = strtok(NULL," :\t\r\n");
		}
		
		break;
	}
	
	fclose(fp);
}

void constructParsingTable()
{
	FILE *fp;
	
	fp = fopen("grammar.txt","r");
	
	if(fp==NULL)
	{
		printf("Could not open the required files!!\n");
		exit(0);
	}
	
	char line[150],originalLine[150];
	char NTIT[10][10];  //non terminals in term
	
	char* pch;
	int i = 1,j,k,l,transNum;
	int ntitt;  //non terminals in transition term
	
	for(j=1;j<30;j++)
		for(k=1;k<30;k++)
			strcpy(parseTable[j][k][0],"\0");
	
	int firstLine = 0;

	while(fgets(originalLine,100,fp))
	{
		strcpy(line,originalLine);
		
		pch = strtok(line," :\t\r\n");
		strcpy(parseTable[i][0][0],pch);   //element from which transition is done
		
		if(firstLine==0)
		{
			strcpy(startSymbol,pch);
			firstLine = 1;
		}

		transNum = 0;
		
		while(pch!=NULL)
		{
			transNum++;
			
			strcpy(line,originalLine);
	
			pch = strtok(line," :\t\r\n");
			pch = strtok(NULL," :\t\r\n");
			
			for(l=1;l<transNum;l++)
			{
				while(pch!=NULL&&strcmp(pch,"|")!=0)
				{
					pch = strtok(NULL," :\t\r\n");
				}
				pch = strtok(NULL," :\t\r\n");   //getting transition terms begins
			}
			
			ntitt = 0;			//number of terms in transition term is initialized to zero, 0 indexing

			while(pch!=NULL&&strcmp(pch,"|")!=0)   //while end of transition term is not detected
			{
				strcpy(NTIT[ntitt],pch);
				
				ntitt++;
				
				pch = strtok(NULL," :\t\r\n");
			}
		
			getFirstSet(NTIT[0]);
		
			for(l=0;l<firstNum;l++)
			for(j=1;j<=terminalNum;j++)       //adding transition for each element in first set
				if(strcmp(parseTable[0][j][0],firstSet[l])==0)
				{
					if(strcmp(parseTable[i][j][0],"\0")!=0)
					{	
						printf("ambiguous\n");
						exit(0);
					}
					
					for(k=0;k<ntitt;k++)
					{
						strcpy(parseTable[i][j][k],NTIT[k]);
					}
					strcpy(parseTable[i][j][k],"\0");
					
					break;
				}
			
			for(j=0;j<firstNum;j++)      //adding transition for each element in follow set if e is in first set
				if(strcmp(firstSet[j],"e")==0)
				{
					getFollowSet(parseTable[i][0][0]);
					
					for(l=0;l<followNum;l++)
					for(j=1;j<=terminalNum;j++)
						if(strcmp(parseTable[0][j][0],followSet[l])==0)
						{
							if(strcmp(parseTable[i][j][0],"\0")!=0)
							{
								printf("ambiguous\n");
								exit(0);
							}
							
							for(k=0;k<ntitt;k++)
							{
								strcpy(parseTable[i][j][k],NTIT[k]);
							}
							strcpy(parseTable[i][j][k],"\0");
						
							break;
						}
					
					break;
				}
		}
		i++;
	}
	
	fclose(fp);
}

int terminalInTable(char str[])
{
	int i;
	
	for(i=1;i<=terminalNum;i++)
	{
		if(strcmp(parseTable[0][i][0],str)==0)
		{
			return 1;
		}
	}
	
	return 0;
}

void addTerminals()
{
	terminalNum = 1;
	
	FILE* fp;
	
	fp = fopen("fst-fol.txt","r");
	
	if(fp==NULL)
	{
		printf("Could not open required files!\n");
		exit(0);
	}
	
	char line[150];
	char* pch;
	
	while(fgets(line,100,fp))
	{
		pch = strtok(line," |\t\r\n");   //non terminal symbol
		pch = strtok(NULL," |\t\r\n");   //first terminal symbol in line
		
		while(pch!=NULL)
		{			
			if(terminalInTable(pch)==0&&strcmp(pch,"e")!=0)
			{
				strcpy(parseTable[0][terminalNum][0],pch);
				terminalNum++;
			}
			
			pch = strtok(NULL," |\t\r\n");
		}
	}
	
	fclose(fp);
}

void setInput()
{
	char line[100];

	FILE* fp = fopen("inputFormatted.txt","r");
	fgets(line,100,fp);
	
	char* pch;

	pch = strtok(line," \r\n");
	int i = 0;
	while(pch!=NULL)
	{
		strcpy(input[i],pch);
		i++;
		pch = strtok(NULL," \r\n");
	}

	strcpy(input[i],"$");
	i++;

	inputSize = i;
}

extern FILE* yyout;
extern FILE* yyin;

void module1()
{
	addTerminals();
	constructParsingTable();
}

void module2()
{
	char userInput[100];

	printf("Enter string : ");	
	scanf("%s",userInput);

	FILE* fp = fopen("inputUser.txt","w");
	fprintf(fp,"%s",userInput);
	fclose(fp);

	fp = fopen("inputUser.txt","r+");	

	yyin = fp;
	
	int TOKEN,i=0;
	while((TOKEN = yylex())!=1)
	{
		switch(TOKEN)
		{case ID: 
			strcpy(input[i],"id");
			i++;
			break;
		case PLUS:
			strcpy(input[i],"+");
			i++;
			break;
		case MULT:
			strcpy(input[i],"*");
			i++;
			break;
		case CROSS:
			strcpy(input[i],"X");
			i++;
			break;
		case DOT:
			strcpy(input[i],".");
			i++;
			break;
		case HASH:
			strcpy(input[i],"#");
			i++;
			break;
		case NORM:
			strcpy(input[i],"NORM");
			i++;
			break;
		case L_SQUARE:
			strcpy(input[i],"[");
			i++;
			break;
		case R_SQUARE:
			strcpy(input[i],"]");
			i++;
			break;
		case L_PARAEN:
			strcpy(input[i],"(");
			i++;
			break;
		case R_PARAEN:
			strcpy(input[i],")");
			i++;
			break;
		case COMMA:
			strcpy(input[i],",");
			i++;
			break;
		case NOTOK:
			printf("Error: Encountered symbol in input string which does not belong to the terminal set of the grammar\n");
			exit(0);
			break;
		}	
	}

	strcpy(input[i],"$");

	yywrap();

	fclose(yyin);
	
	remove("inputUser.txt");
	
	parseProcess(input);
}

int main()
{
	module1();

	module2();
}

int yywrap()
{
	return 1;
}
