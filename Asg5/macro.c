#include<stdio.h>
#include<string.h>

//data structure for storing a macro
typedef struct _macroStore{
	char name[10];
	char instruction[10][10];
	char operand[10][10];
	char arguments[2][10][10];
	int numInstr;
	int numArg;
	int numArgLines;
}macroStore;

//file pointers for writing
FILE* fp;

macroStore msArr[100];
int numMacros = 0;

void Macro();  //function for macros to be defined and expanded

int main()
{
	Macro();
}

//function to definea macro and store it
macroStore defineMacro(char argLine[])
{
	int n;
	char* pch;
	char line[100];
	char originalLine[100];
	char instruction[100];
	
	macroStore ms,nms;
	
	pch = strtok(argLine," ,&\t\n\r");
	strcpy(ms.name,pch);
	
	pch = strtok(NULL," ,&\t\n\r");  //MACRO
	pch = strtok(NULL," ,&\t\n\r");  //first argument (if not null)
	
	ms.numArg = 0;
			
	//storing the arguments
	while(pch!=NULL)
	{
		strcpy(ms.arguments[0][ms.numArg],pch);
		ms.numArg++;
		pch = strtok(NULL," ,&\t\n\r");
	}

	ms.numInstr = 0;
	ms.numArgLines = 1;
	int i;
	
	//storing the instruction set
	while(gets(line))
	{
		strcpy(originalLine,line);
		
		pch = strtok(line," ,&\t\n\r");
		strcpy(instruction,pch);
			
		if(strcmp(pch,"MEND")==0)
			break;
		
		pch = strtok(NULL," ,&\t\n\r");
		
		//if a nested macro is detected
		if(strcmp(pch,"MACRO")==0)
		{
			nms = defineMacro(originalLine);  //function call for macro to be defined
			
			for(i=ms.numArg;i<ms.numArg+nms.numArg;i++)
				strcpy(ms.arguments[0][i],nms.arguments[0][i-ms.numArg]);
			
			ms.numArg += nms.numArg;
				
			for(i=ms.numInstr;i<ms.numInstr+nms.numInstr;i++)
				strcpy(ms.instruction[i],nms.instruction[i-ms.numInstr]);
			
			for(i=ms.numInstr;i<ms.numInstr+nms.numInstr;i++)
				strcpy(ms.operand[i],nms.operand[i-ms.numInstr]);
				
			ms.numInstr += nms.numInstr;
			
			ms.numArgLines++;
		}
		else  //prooceeding normally
		{
			strcpy(ms.instruction[ms.numInstr],instruction);
			strcpy(ms.operand[ms.numInstr],pch);
				
			ms.numInstr++;
		}
	}
	
	return ms;  //macro returned
}

//function to expand macro
void expandMacro(char line[100],int i)
{
	int j = 0,k,flag;
	char* pch;
	char outline[100];
	pch = strtok(line," ,&\t\n\r");
	pch = strtok(NULL," ,&\t\n\r");
	
	//substituting argument list			
	while(pch!=NULL)
	{
		strcpy(msArr[i].arguments[1][j],pch);
		pch = strtok(NULL," ,&\t\n\r");
		j++;
	}	
	
	for(k=0;k<msArr[i].numArgLines-1;k++)
	{
		gets(line);
					
		pch = strtok(line," ,&\t\n\r");
		pch = strtok(NULL," ,&\t\n\r");
		
		while(pch!=NULL)
		{
			strcpy(msArr[i].arguments[1][j],pch);
			pch = strtok(NULL," ,&\t\n\r");
			j++;
		}
	}
	
	//expanding macro according to argument list
	for(j=0;j<msArr[i].numInstr;j++)
	{
		flag = 0;
	
		for(k=0;k<msArr[i].numArg;k++)
		{
			if(strcmp(msArr[i].instruction[j],msArr[i].arguments[0][k])==0)
			{
				strcpy(outline,msArr[i].arguments[1][k]);
				flag = 1;
				break;
			}
		}
					
		if(flag==0)
			strcpy(outline,msArr[i].instruction[j]);
					
		strcat(outline,"\t");
					
		flag = 0;
					
		for(k=0;k<msArr[i].numArg;k++)
		{
			if(strcmp(msArr[i].operand[j],msArr[i].arguments[0][k])==0)
			{
				strcat(outline,msArr[i].arguments[1][k]);
				flag = 1;
				break;
			}
		}
					
		if(flag==0)
			strcat(outline,msArr[i].operand[j]);
					
		fprintf(fp,"\t%s\n",outline);
	}
}

//function for defining and expanding macros in 1 pass
void Macro()
{
	int n,i,j,k,flag1,flag2;
	char* pch;
	char line[100];
	
	char originalLine[100];
	macroStore ms;

	fp = fopen("macro_output.txt","w");
	
	while(gets(line))
	{
		strcpy(originalLine,line);
		pch = strtok(line," ,&\t\n\r");
		strcpy(ms.name,pch);
		
		flag1 = 0;
		flag2 = 0;
		
		for(i=0;i<numMacros;i++)
		{
			if(strcmp(msArr[i].name,pch)==0)
			{
				expandMacro(originalLine,i);    //macro being expanded
				
				flag1 = 1;
				flag2 = 1;
				break;
			}
		}
		
		if(flag1==0)
		{
			pch = strtok(NULL," ,&\t\n\r");
		
			if(strcmp(pch,"MACRO")==0)
			{
				msArr[numMacros] = defineMacro(originalLine);  //macro being defined
				numMacros++;
				flag2 = 1;	
			}
		}
		
		if(flag2==0)
		{
			fprintf(fp,"%s\n",originalLine);
		}
	}
	
	fclose(fp);
}
