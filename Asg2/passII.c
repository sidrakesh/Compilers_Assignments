#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//Opcode Table
char OPTAB[5][2][100] = {
	{"LDA", "00"},
	{"MUL", "20"},
	{"DIV", "24"},
	{"STA", "0C"},
	{"ADD", "18"}	
};//INSTRUCTION, OPCODE

//Symbol Table
struct symbol_table
{
	char elements[100][2][100];//LABEL, LOCCTR
	int count;	
}SYMTAB;

char im_file[100], obj_file[100];   //intermediate file name and object file name declared as global variables

//to load the symbol table from the file
void loadSYMTAB_from_file()
{
	FILE * fp;
	int i;
	char S[100];     //for storing the symbol table file name
	strcpy(S, im_file);
	S[strlen(S)-17] = '\0';
	strcat(S, "-SYMTAB.txt");
	fp = fopen(S, "r");
	
	if(fp == NULL)
	{
		printf("Could not open SYMTAB file!!\n");
		exit(0);		
	}
	
	i = 0;
	while(!feof(fp))   //loading symbol table elements
	{
		fscanf(fp, "%s %s",SYMTAB.elements[i][0], SYMTAB.elements[i][1]);
		i++;
	}
	
	SYMTAB.count = i;
	fclose(fp);	
}

//Returns Opcode of given instruction, null if not found
char * search_OPTAB(char * INSTRUCTION)
{
	int i;
	for(i=0;i<5;++i)
	{
		if(!strcmp(INSTRUCTION, OPTAB[i][0]))
			return OPTAB[i][1];
	}
	return NULL;//NOT FOUND
}

//Returns Address if LABEL is already present in SYMTAB, else NULL
char * search_SYMTAB(char * LABEL)
{
	int i;
	for(i=0;i<SYMTAB.count;++i)
	{
		if(!strcmp(LABEL, SYMTAB.elements[i][0]))//Found
			return SYMTAB.elements[i][1];
	}
	return NULL;
}

//for making a 'size'-bit array from original array to the edited array
void make_length(char * original, char edited[], int size)
{
	char temp1[7];
	strcpy(temp1,original);
	int i = size - strlen(temp1),j;
	for(j=0;j<i;++j)
		edited[j] = '0';
	edited[j] = '\0';
	strcat(edited,original);	
}

//Converts a BYTE to it's Object Code
void convert_BYTE(char BYTE[], char t[])
{
	int i, k=0;
	for(i=2;i<strlen(BYTE);++i)
	{
		if(BYTE[i]=='\'')
			break;
		else
			t[k++] = BYTE[i];
	}
	t[k] = '\0';	
}

//Converts WORD to it's Object Code
void convert_WORD(char WORD[],char t[])
{
	int x = atoi(WORD);
	itoa(x, t, 16);
	char temp[7];
	strcpy(temp,t);
	make_length(temp, t, 6);		
}

//pass II
void PASS_TWO()
{
	FILE * fp1,*fp2,*fp3;
	char line[100], s1[100], s2[100], s3[100], s4[100], startADDRESS[5], length[5], name[100];
	//s1 is Location, s2 is Label, s3 is Opcode, s4 is Operand
	char * symbolADDRESS;
	char OBJECTCODE[100] ;
	int t,flag_SYMTAB,main_flag = 0,linecounter = 0;
	
	printf("Enter the intermediate file name :");
	scanf("%s",im_file);	
	fp1 = fopen(im_file, "r");   //opening intermediate file
	
	if(fp1 == NULL)
	{
		strcat(im_file, ".txt");
		fp1 = fopen(im_file, "r");
		if(fp1 == NULL)
		{
			printf("Could not open Intermediate file!!\n");	
			exit(0);			
		}
	}
	
	loadSYMTAB_from_file();		//loading symbol table
	
	strcpy(obj_file, im_file);
	obj_file[strlen(obj_file)-17] = '\0';
	strcat(obj_file, "-object.txt");
	fp2 = fopen(obj_file, "w");		
	if(fp2 == NULL)
	{
		printf("Could not open Object file!!\n");
		exit(0);	
	}	
	
	//Read first line
	fgets(line, 100 ,fp1);
	linecounter++;	
	t = sscanf(line,"%s %s %s %s",s1,s2,s3,s4);
	if(t==3)
	{
		strcpy(s4, s3);
		strcpy(s3, s2);		
	}
	if(strcmp(s3,"START")==0)
	{	
		strcpy(startADDRESS, s1);
		strcpy(name, s2);//s2 != s3
		fgets(line, 100 ,fp1);//Read next line
		linecounter++;	
		t = sscanf(line,"%s %s %s %s",s1,s2,s3,s4);
		if(t==3)
		{
			strcpy(s4, s3);
			strcpy(s3, s2);			
		}
	}	
	else 
	{
		strcpy(startADDRESS, "0000");
		strcpy(name, "UNNAMED");
	}
	
	//Getting Program Length
	char S[100];
	strcpy(S, im_file);
	S[strlen(S)-17] = '\0';
	strcat(S, "-length.txt");	
	fp3 = fopen(S, "r");
	fscanf(fp3, "%s",length);
	fclose(fp3);

	//Writing Header Record	
	char temp1[7], temp2[7];
	make_length(startADDRESS, temp1, 6);
	make_length(length, temp2, 6);	
	
	fprintf(fp2,"H|%s|%s|%s",name,temp1,temp2);	
	fprintf(fp2,"\n");
	
	//Initialse text record
	fprintf(fp2, "T|%s|%s",temp1,length);	
		
	while(!feof(fp1) && (strcmp(s3, "END")!=0))
	{
		flag_SYMTAB = 0;
		if(s2[0] != '#')//Not a comment line
		{
			char * OPCODE = search_OPTAB(s3);
			if(OPCODE != NULL)//OPCODE found in OPTAB
			{
				if(s4!=NULL)//There is a symbol in operand field
				{
					symbolADDRESS = search_SYMTAB(s4);
					if(symbolADDRESS != NULL)
					make_length(symbolADDRESS, temp1, 4);
					if(symbolADDRESS == NULL)//Operand does not exist in SYMTAB
					{
						printf("Line %d : Error : Symbol is not defined\n",linecounter);
						main_flag = 1;						
						flag_SYMTAB = 1;
					}															
				}
				else//No symbol in operand field
					strcpy(symbolADDRESS,"0000");
					
				//Assemble object code instruction
				strcpy(OBJECTCODE, OPCODE);
				
				if(symbolADDRESS != NULL)
					strcat(OBJECTCODE, temp1);				
			}
			
			else if(OPCODE==NULL && (strcmp(s3, "BYTE") == 0))
				convert_BYTE(s4,OBJECTCODE);
				
			else if(OPCODE==NULL && (strcmp(s3, "WORD") == 0))
				convert_WORD(s4,OBJECTCODE);
				
			else//OPCODE is RESW or RESB
				strcpy(OBJECTCODE, "");
				
			if(strcmp(OBJECTCODE,"") != 0)						
			fprintf(fp2, "|%s",OBJECTCODE);//Add object code to text														
		}
			
		//Read next line	
		fgets(line, 100, fp1);
		linecounter++;			
		t = sscanf(line,"%s %s %s %s",s1,s2,s3,s4);
		if(t==3)
		{
			strcpy(s4, s3);
			strcpy(s3, s2);			
		}		
	}	
	//Write End Record
	fprintf(fp2,"\n");
	make_length(startADDRESS, temp1, 6);
	fprintf(fp2,"E|%s",temp1);
	
	fclose(fp1);
	fclose(fp2);
	
	strcpy(obj_file, im_file);
	obj_file[strlen(obj_file)-17] = '\0';
	strcat(obj_file, "-object.txt");
	
	if(main_flag == 1)			//error in file
		remove(obj_file);		//removing the object file as error has been detected
}

//for deleting the symbol table and length files
void remove_files()
{
	char S[100];
	strcpy(S,im_file);
	S[strlen(S)-17] = '\0';
	strcat(S, "-SYMTAB.txt");
	remove(S);       //deleting symbol table file
	
	strcpy(S,im_file);
	S[strlen(S)-17] = '\0';
	strcat(S, "-length.txt");
	remove(S);		 //deleting length file
}

int main()
{
	PASS_TWO();
	remove_files();			
}
