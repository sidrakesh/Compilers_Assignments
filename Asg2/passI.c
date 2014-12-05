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

//Location Counter
char LOCCTR[5];

char in_file[100], out_file[100];

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

//Initialise Symbol Table	
void init_SYMTAB()
{
	SYMTAB.count = 0;
}

//Returns 1 if LABEL is already present in SYMTAB
int search_SYMTAB(char * LABEL)
{
	int i;
	for(i=0;i<SYMTAB.count;++i)
	{
		if(!strcmp(LABEL, SYMTAB.elements[i][0]))
			return 1;//Duplicate
	}
	return 0;
}
//Insert an element in Symbol Tabel
void insert_SYMTAB(char * LABEL, char * LOCCTR)
{
	strcpy(SYMTAB.elements[SYMTAB.count][0], LABEL);
	strcpy(SYMTAB.elements[SYMTAB.count][1], LOCCTR);
	SYMTAB.count++;		
}

//Increments LOCCTR bu jump number of bytes
void increment_LOCCTR(int jump)
{
	int val;
	sscanf(LOCCTR, "%x", &val);
	val += jump;
	itoa(val, LOCCTR, 16);	
}

//Finds the length of the BYTE operand
int find_length_BYTE(char * BYTE)
{
	int i;
	for(i=2;i<strlen(BYTE);++i)
	{
		if(BYTE[i]=='\'')
		break;
	}
	i = i-2;//length
	return i;
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

//Pass 1
void PASS_ONE()
{
	FILE * fp1,*fp2;                 //file pointers
	char line[100], s1[100], s2[100], s3[100], s4[100],startingADDRESS[5], saveLOCCTR[5];
	int t,flag_SYMTAB, flag_OPTAB;
	
	printf("Enter the input file name :");
	scanf("%s",in_file);	                //input file name
	fp1 = fopen(in_file, "r");				//corresponding file opened
	
	if(fp1 == NULL)							//if '.txt' not in entered file name 
	{
		strcat(in_file, ".txt");			//concatenating '.txt'
		fp1 = fopen(in_file, "r");
		if(fp1 == NULL)
		{
			printf("Could not open source file!!\n");	
			exit(0);			
		}
	}
	
	strcpy(out_file, in_file);
	out_file[strlen(out_file)-4] = '\0';
	strcat(out_file, "-intermediate.txt");   //to get intermediate file name
	fp2 = fopen(out_file, "w");		
	if(fp2 == NULL)
	{
		printf("Could not open Intermediate file!!\n");
		exit(0);	
	}	
	
	//fgets(line, 100, fp1);//For column headers
	//Read first line
	fgets(line, 100 ,fp1);					//reading a line of the input
	t = sscanf(line,"%s %s %s %s",s1,s2,s3,s4);   //t stores the number of words read
	
	if(t==2)	//two words read
	{
		strcpy(s3, s2); //shifting the words read
		strcpy(s2, s1);
		strcpy(s1, "");
	}
	char temp[10];
	if(strcmp(s2,"START")==0)   //start line
	{	
		strcpy(LOCCTR,s3);
		
		make_length(LOCCTR,temp,4);
		line[strlen(line)-1] = '\0';
		
		if(t==1)
			fprintf(fp2, "%s %s %s",temp,line," Syntax error : less arguments");
		else if(t>3)
			fprintf(fp2, "%s %s %s",temp,line," Syntax error : more arguments");
		else 	
			fprintf(fp2, "%s %s",temp,line);//Write to Intermediate file	
		
		fprintf(fp2,"\n");	
		
		fgets(line, 100 ,fp1);//Read next line	
		t = sscanf(line,"%s %s %s %s",s1,s2,s3,s4);
		if(t==2)
		{
			strcpy(s3, s2);
			strcpy(s2, s1);
			strcpy(s1, "");
		}
	}	
	else  //if START was not found 
	{
		strcpy(LOCCTR, "0000");
		printf("Syntax Error : NO START found!!\n");	
	}
		
	strcpy(startingADDRESS, LOCCTR);
		
	while(!feof(fp1) && (strcmp(s2, "END")!=0))
	{
		flag_SYMTAB = 0;flag_OPTAB = 0;		
		strcpy(saveLOCCTR, LOCCTR);			
		if(line[0] != '#')//Not a comment line
		{
			if(t==3)//Symbol exists in Label Field
			{
				if(search_SYMTAB(s1))//Label exists in SYMTAB
				 	flag_SYMTAB = 1;
				 else
				 	insert_SYMTAB(s1,LOCCTR);
			}
			
			strcpy(saveLOCCTR, LOCCTR);
			char * OPCODE = search_OPTAB(s2);
			if(OPCODE != NULL)
				increment_LOCCTR(3);
			else if(strcmp(s2, "WORD") == 0)  //WORD detected
				increment_LOCCTR(3);
			else if(strcmp(s2, "RESW") == 0)  //RESW detected
				increment_LOCCTR(3 * atoi(s3));						
			else if(strcmp(s2, "RESB") == 0)  //RESB detected
				increment_LOCCTR(atoi(s3));		
			else if(strcmp(s2, "BYTE") == 0)  //BYTE detected
				increment_LOCCTR(find_length_BYTE(s3));		
			else		
				{
					increment_LOCCTR(3);
					flag_OPTAB = 1;//OPCODE not found and is not an assembler directive
				}
		}
		
		make_length(saveLOCCTR,temp,4);
		strcpy(saveLOCCTR,temp);
		
		char write[100];//Line to write in Intermediate file
		line[strlen(line)-1] = '\0';//Remove newline character from line read so taht it writes error on same line 
		if(flag_OPTAB == 0 )
		{
			if(flag_SYMTAB == 0)
			sprintf(write, "%s %s",saveLOCCTR, line);
			else
			sprintf(write,"%s %s %s",saveLOCCTR, line, "Duplicate Symbol");		
		}
		else
		{
			if(flag_SYMTAB == 0)
			sprintf(write,"%s %s %s",saveLOCCTR, line, "Invalid Operation Code");
			else
			sprintf(write,"%s %s %s %s",saveLOCCTR, line, "Duplicate Symbol", "Invalid Operation Code");		
		}
		
		if(t==1)
			fprintf(fp2, "%s %s",write," Syntax error : less arguments");
		else if(t>3)
			fprintf(fp2, "%s %s",write," Syntax error : more arguments");
		else			
			fprintf(fp2, "%s",write);//Write to Intermediate file		
			
		fprintf(fp2,"\n");
		
		//Read next line	
		fgets(line, 100, fp1);			
		t = sscanf(line,"%s %s %s %s",s1,s2,s3,s4);
		if(t==2)
		{
			strcpy(s3, s2);
			strcpy(s2, s1);
			strcpy(s1, "");
		}		
	}

	make_length(LOCCTR,temp,4);
	//Write last line to Intermediate file
	fprintf(fp2, "%s %s",temp, line);
	
	fclose(fp1);
	fclose(fp2);
	
	//Computing Program Length
	int start, location,l;
	char length[5];
	sscanf(startingADDRESS, "%x", &start);
	sscanf(LOCCTR, "%x", &location);
	l = location - start;
	itoa(l, length, 16);  //getting the length of the code in a string
	
	strcpy(out_file, in_file);
	out_file[strlen(out_file)-4] = '\0';
	strcat(out_file, "-length.txt");
	fp2 = fopen(out_file, "w");
	if(fp2 != NULL)
		fprintf(fp2, "%s", length);
		
	fclose(fp2);		//closing file pointer
}

//for writing the symbol table to a file
void saveSYMTAB2file()   
{
	FILE * fp;
	int i;
	strcpy(out_file, in_file);
	out_file[strlen(out_file)-4] = '\0';
	strcat(out_file, "-SYMTAB.txt");
	fp = fopen(out_file, "w");
	
	if(fp == NULL)
	{
		printf("Could not write SYMTAB to file!!\n");
		exit(0);		
	}
	
	for(i=0;i<SYMTAB.count;++i)   //writing symbol table elements
	{
		fprintf(fp, "%s %s\n",SYMTAB.elements[i][0], SYMTAB.elements[i][1]);
	}
	
	fclose(fp);	//closing file pointer
}

//main function
int main()
{
	init_SYMTAB();
	PASS_ONE();
	saveSYMTAB2file();	
}
