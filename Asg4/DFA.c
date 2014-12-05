#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "def.h"

/* This part handles conversion of REGEX to POSTFIX*/
typedef struct _stack{
	int numElements;
	char arr[100][20];
}stackPOST;

void pushPOST(stackPOST* S,char expr[])
{
	strcpy(S->arr[S->numElements],expr);
	S->numElements++;
}

char* popPOST(stackPOST* S)
{
	S->numElements--;
	
	return S->arr[S->numElements];
}

int isEmptyPOST(stackPOST S)
{
	if(S.numElements==0)
		return 1;
	else return 0;
}

char* conversion(char* regex)
{
	int i = 0,j;
	int SB = 0;
	stackPOST S;
	S.numElements = 0;
	stackPOST F;
	F.numElements = 0;
	stackPOST GS;
	GS.numElements = 0;
	
	char* p1;
	char regexArr[100];
	
	for(i=0;i<strlen(regex);i++)
	{
		if(regex[i]=='-')
			regex[i] = '@';
		else if(regex[i]=='*')
			regex[i] = '#';
	}
	
	i=0;
	while(regex[i]!='\0')
	{
		while(SB!=0)
		{
			while(regex[i]=='('||regex[i]==')')
				i++;
			
			if(regex[i]=='@')
			{
				p1 = popPOST(&S);
				i++;
				regexArr[0] = regex[i];
				regexArr[1] = '\0';
				
				p1 = strcat(p1,regexArr);
				
				regexArr[0] = '@';
				regexArr[1] = '\0';
				p1 = strcat(regexArr,p1);
				
				pushPOST(&S,p1);
				i++;
			}
			else if(isalnum(regex[i])!=0||regex[i]=='_')
			{
				regexArr[0] = regex[i];
				regexArr[1] = '\0';
				
				pushPOST(&S,regexArr);
				i++;
			}
			
			if(regex[i]==']')
			{
				p1 = S.arr[0];
				j = 1;
				while(j!=S.numElements)
				{
					p1 = strcat(p1,S.arr[j]);
					
					regexArr[0] = '|';
					regexArr[1] = '\0';
					p1 = strcat(p1,regexArr);
					
					j++;
				}
				
				while(isEmptyPOST(S)==0)
					popPOST(&S);
				
				i++;
				
				if(regex[i]=='#')
				{
					regexArr[0] = '#';
					regexArr[1] = '\0';
					p1 = strcat(p1,regexArr);
					i++;
				} 
				
				pushPOST(&F,p1);
				
				SB--;
			}
		}
		if(regex[i]=='[')
		{
			SB++;
			i++;
		}
		else if(regex[i]=='|')
		{
			p1 = F.arr[0];
			j = 1;
	
			while(j!=F.numElements)
			{	
				p1 = strcat(p1,F.arr[j]);
					
				regexArr[0] = '&';
				regexArr[1] = '\0';
				p1 = strcat(p1,regexArr);
						
				j++;
			}
			
			while(isEmptyPOST(F)==0)
				popPOST(&F);
			
			pushPOST(&GS,p1);
			
			i++;
		}
		else if(regex[i]!='\0')
		{
			regexArr[0] = regex[i];
			regexArr[1] = '\0';
			pushPOST(&F,regexArr);
			i++;
		}
	}
	
	p1 = F.arr[0];
	j = 1;
	
	while(j!=F.numElements)
	{	
		p1 = strcat(p1,F.arr[j]);
					
		regexArr[0] = '&';
		regexArr[1] = '\0';
		p1 = strcat(p1,regexArr);
						
		j++;
	}
	pushPOST(&GS,p1);
	
	p1 = GS.arr[0];
	j = 1;
	
	while(j!=GS.numElements)
	{	
		p1 = strcat(p1,GS.arr[j]);
					
		regexArr[0] = '|';
		regexArr[1] = '\0';
		p1 = strcat(p1,regexArr);
					
		j++;
	}
	
	return p1;
}
/*Conversion to POSTFIX ends here*/

/*Structure for token*/
typedef struct _token{
	int tokenID;
	attrType attribute;
}token;

/*structure for storing keywords in the keyword array*/
typedef struct _keyWordID{
	char keyword[20];
	int tokenID;
}keyWordID;

/*keyword array*/
keyWordID keywordArr[] = {{"short",SHORT},{"int",INT},{"float",FLOAT},{"double",DOUBLE},{"bool",BOOL},{"char",CHAR},
						{"signed",SIGNED},{"unsigned",UNSIGNED},{"for",FOR},{"while",WHILE},{"do",DO},{"return",RETURN},
						{"struct",STRUCT},{"const",CONST},{"void",VOID},{"switch",SWITCH},{"break",BREAK},{"case",CASE},
						{"continue",CONTINUE},{"goto",GOTO},{"long",LONG},{"static",STATIC},{"union",UNION},{"default",DEFAULT}
		};

//initializes symbol table by adding all the keywords
void initialize_Symbol_Table()
{
	FILE* fp = fopen("symbol_table_3.out","w");
	
	int i;
	
	for(i=0;i<24;i++)
	{
		fprintf(fp,"%s 0\n",keywordArr[i].keyword);
	}

	fclose(fp);
}

//writes the tokens to the output file
void write_token(token t)
{
	FILE* fp = fopen("a4_3.out","a");
	
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

typedef int STATE;

typedef struct _Stack
{
	STATE arr[100];
	int top;	
}Stack; 

typedef struct _def
{
	char pattern[20];
	int tokenID;
}def;

def patterns_defined_in_def[] = {
	{"IDNTIFIER", IDNTIFIER},{"INT_CONST", INT_CONST},{"FLO_CONST", FLO_CONST},
	{"PLUS", PLUS},{"MINUS", MINUS},{"MULT", MULT},{"DIV", DIV},{"PERCENT", PERCENT},{"PLUS_PLUS", PLUS_PLUS},{"MINUS_MINUS", MINUS_MINUS},
	{"EQ", EQ},{"PLUS_EQ", PLUS_EQ},{"MINUS_EQ", MINUS_EQ},{"MULT_EQ", MULT_EQ},{"DIV_EQ", DIV_EQ},
	{"EQ_EQ", EQ_EQ},{"LESSER", LESSER},{"LESS_EQ", LESS_EQ},{"GREATER", GREATER},{"GREAT_EQ", GREAT_EQ},
	{"L_SQUARE", L_SQUARE},{"R_SQUARE", R_SQUARE},{"L_PARAEN", L_PARAEN},{"R_PARAEN", R_PARAEN},{"L_CBRACE", L_CBRACE},{"R_CBRACE", R_CBRACE},{"COMMA", COMMA},{"SEMICOL", SEMICOL},
	{"SHORT", SHORT},{"INT", INT},{"FLOAT", FLOAT},{"DOUBLE", DOUBLE},{"BOOL", BOOL},{"CHAR", CHAR},
	{"SIGNED", SIGNED},{"UNSIGNED", UNSIGNED},{"FOR", FOR},{"WHILE", WHILE},{"DO", DO},
	{"RETURN", RETURN},{"STRUCT", STRUCT},{"CONST", CONST},{"VOID", VOID},
	{"SWITCH", SWITCH},{"BREAK", BREAK},{"CASE", CASE},{"CONTINUE", CONTINUE},
	{"GOTO", GOTO},{"LONG", LONG},{"STATIC", STATIC},{"UNION", UNION},{"DEFAULT", DEFAULT},
	{"NOTOK",NOTOK}		
};

//Structure to map final states to different patterns
typedef struct _finalstate
{
	STATE state;
	int tokenID;	
}finalstate;

int STT[1000][1000][128] = {0};
finalstate FINALSTATES[100] = {0};
STATE STARTSTATES[100] = {0};
int NFinalStates = 0, NStartStates = 0;
STATE nextAvailableState = 1;
STATE HEAD;//The starting state for the complete NFA

void init(Stack * S)
{
	S->top = -1;
}

int isEmpty(Stack * S)
{
	return S->top == -1;
}
void push(Stack * S, STATE a)
{
	S->top++;
	S->arr[S->top] = a;
}

STATE pop(Stack * S)
{
	if(S->top != -1)
	{
		STATE a = S->arr[S->top];
		S->top--;
		return a;
	}	
}

void addTransition(STATE u, STATE v, char choice)
{
	STT[u][v][choice] = 1;
}

void addStartState(STATE a)
{
	STARTSTATES[NStartStates++] = a;
}

void addFinalState(STATE a, char pattern[])
{
	int i, size = sizeof(patterns_defined_in_def)/sizeof(patterns_defined_in_def[0]);
	for(i=0;i<size;++i)
	{
		if(strcmp(pattern, patterns_defined_in_def[i].pattern)==0)
			break;
	}
	if(i==size)
	{
		printf("No matching tokenID found in def.h!!");
		return;
	}
	finalstate t = {a, patterns_defined_in_def[i].tokenID};
	FINALSTATES[NFinalStates++] = t;
}

//Returns Start State for the NFA of the input REGEX.
//This builds an individual NFA for the input REGEX and stores the start and final state.
//'$' represents EPSILON transition.
void buildIndividualNFA(char pattern[], char POSTFIX[])
{
	int i = 0;
	char ch;
	Stack starts, ends;
	init(&starts);
	init(&ends);
	
	while(ch = POSTFIX[i++])
	{
		if(ch == '|')//Union
		{
			STATE start1 = pop(&starts), start2 = pop(&starts);
			STATE end1 = pop(&ends), end2 = pop(&ends);
			STATE newStart = nextAvailableState++, newEnd = nextAvailableState++;
			addTransition(newStart, start1, '$');
			addTransition(newStart, start2, '$');
			addTransition(end1, newEnd, '$');
			addTransition(end2, newEnd, '$');
			
			push(&starts, newStart);push(&ends, newEnd);			
		}
		else if(ch == '#')//Kleene Closure
		{
			STATE start = pop(&starts), end = pop(&ends);
			STATE newStart = nextAvailableState++, newEnd = nextAvailableState++;
			addTransition(end, start, '$');
			addTransition(newStart, newEnd, '$');
			addTransition(newStart, start, '$');
			addTransition(end, newEnd, '$');
			
			push(&starts, newStart);push(&ends, newEnd);			
		}
		else if(ch == '&')//Concatination
		{
			STATE start1 = pop(&starts), start2 = pop(&starts);
			STATE end1 = pop(&ends), end2 = pop(&ends);
			addTransition(end2, start1, '$');
			
			push(&starts, start2);push(&ends, end1);
		}
		else if(ch == '@')//a-z
		{
			char a = POSTFIX[i++], b = POSTFIX[i++];
			int j;
			STATE start = nextAvailableState++, end = nextAvailableState++;
			for(j=a;j<=b;++j)			
			addTransition(start, end, j);
			
			push(&starts, start);push(&ends, end);			
		}
		else//Single NFA
		{
			STATE start = nextAvailableState++, end = nextAvailableState++;
			addTransition(start, end, ch);
			
			push(&starts, start);push(&ends, end);
		}		
	}
	addStartState(pop(&starts));
	addFinalState(pop(&ends), pattern);
	//printf("the pattern is %s and token is %d\n", pattern, FINALSTATES[NFinalStates-1].tokenID);		
}

//This makes one single NFA by adding EPSILON transitions to all other start states.
void buildCombinedNFA()
{
	int i;
	HEAD = nextAvailableState++;
	for(i=0;i<NStartStates;++i)
	addTransition(HEAD, STARTSTATES[i], '$');	
}

int isFinalState(STATE a)
{
	int i;
	for(i=0;i<NFinalStates;++i)
	{
		if(FINALSTATES[i].state==a)
			return 1;
	}
	return 0;
}

/* DFA construction and simulation*/
//Stack used on finding epsilon Closure. 
typedef struct _dstack
{
	int arr[100];
	int top;
}DSTACK;

void initD(DSTACK * D)
{
	D->top=-1;
}
int isEmptyD(DSTACK * D)
{
	return D->top==-1;
}
void pushD(DSTACK * D, int a)
{
	D->top++;
	D->arr[D->top]=a;
}
int popD(DSTACK * D)
{
	int t = D->arr[D->top];
	D->top--;
	return t;
}

typedef struct _queue
{
	int arr[10010];
	int front;
	int rear;
}QUEUE;

void initQ(QUEUE * Q)
{
	Q->front = Q->rear = -1;
}
 
int isEmptyQ(QUEUE * Q) 
{
	if(Q->front==-1 && Q->rear==-1)
		return 1;	
	if(Q->front==Q->rear+1)
		return 1;	
		
	return 0;		
}

int front(QUEUE * Q)
{
	return Q->arr[Q->front];
}

void enqueue(QUEUE * Q, int a)
{
	if(Q->front==-1)
	{
		Q->front=Q->rear=0;
		Q->arr[Q->rear] = a;
		return;
	}
	Q->rear++;
	Q->arr[Q->rear] = a;	
}

int dequeue(QUEUE * Q)
{
	int t = Q->arr[Q->front];
	Q->front++;
	return t;
}

typedef struct _dfastate
{
	STATE arr[100];
	int size;
}DFASTATE;

DFASTATE Dstates[1000];//Stores all the DFA states in terms of the NFA states
int NDStates = 0;//Number of DFA states
int Dtran[1000][128]={0};//The transition table for the DFA
 
typedef struct _dfinalstate 
{
 	int state;
 	int tokenID;
}dfinalstate;
dfinalstate DFinalStates[100];//Final states of the DFA
int NDFinalStates = 0;//Number of final DFA states

int getTokenID(int a)
{
	int i;
	for(i=0;i<NDFinalStates;++i)
	{
		if(DFinalStates[i].state == a)
			return DFinalStates[i].tokenID;
	}	
	return NOTOK;
}
 
int isPresent(int arr[], int n, int a)
{
	int i;
	for(i=0;i<n;++i)
		if(arr[i]==a)
			return 1;
	
	return 0;
}
//Returns epsilon Closure of arr[] 
void epsilonClosure(int arr[], int * n)
{
	int i;
	DSTACK D;
	initD(&D);
	
	for(i=0;i<*n;++i)
		pushD(&D, arr[i]);
		
	while(!isEmptyD(&D))
	{
		int t = popD(&D);
		for(i=1;i<nextAvailableState;++i)
		{
			if(STT[t][i]['$'])//t->i is an epslilon transition
			{
				if(!isPresent(arr, *n, i))//i is not already present in arr
				{
					arr[(*n)++] = i;
					pushD(&D, i);
				}
			}
		}
	}
}

void move(int dfa_state, int a, int arr[], int * n)  
{
	int i,j,k;
	*n = 0;
	for(i=0;i<Dstates[dfa_state].size;++i)
	{
		int t = Dstates[dfa_state].arr[i];
		for(j=1;j<nextAvailableState;++j)
		{
			if(STT[t][j][a])//There is a transition from t to j upon input a
			arr[(*n)++]=j;
		}
	}
}

void addStateInDFA(int arr[], int n)
{
	int i,j;
	NDStates++;
	for(i=0;i<n;++i)
	{
		Dstates[NDStates].arr[i] = arr[i];
	}
	Dstates[NDStates].size=n;	
	
	//Check if this is a final state in the DFA.	
	for(i=0;i<NFinalStates;++i)
	{
		for(j=0;j<n;++j)
		{
			if(FINALSTATES[i].state==arr[j])
			{
				dfinalstate t = {NDStates, FINALSTATES[i].tokenID};
				DFinalStates[NDFinalStates++]=t;
			}
		}
	}
}

int isInDFA(int arr[], int n)
{
	int i, j, k, match_cnt;
	for(i=1;i<=NDStates;++i)
	{
		match_cnt = 0;
		if(Dstates[i].size!=n)
		continue;
		for(j=0;j<n;++j)
		{
			for(k=0;k<n;++k)
			{
				if(arr[j]==Dstates[i].arr[k])
				{
					match_cnt++;
					break;
				}
			}
			if(k==n)//This state cannot be same as arr[]
			break;
		}
		if(match_cnt==n)
			return i;
	}
	return 0;
}

//This represents the PHI state i.e. which has no transition.
#define PHI 0

void buildDFA()
{
	int i,a;
	QUEUE Q;
	initQ(&Q);
	int arr[100], n=0;
	arr[0]=HEAD;n++;
	epsilonClosure(arr, &n);
	addStateInDFA(arr, n);//Add start state to DFA
	
	//print_a_DFA_state(arr, n);
	
	enqueue(&Q, NDStates);
	while(!isEmptyQ(&Q))
	{
		int t = dequeue(&Q);
		for(a=1;a<128;++a)
		{
			if(a=='$')//Skip epsilon transitions as they are already in epsilon closure
			continue;
			n=0;
			move(t, a, arr, &n);
			epsilonClosure(arr, &n);
			if(n==0)//phi state
			{
				Dtran[t][a] = PHI;
				continue;
			}
			
			int u = isInDFA(arr, n);//It tells whether this state is already present in DFA or not.returns index of matching state in DFA.
			if(!u)
			{
				addStateInDFA(arr, n);
				enqueue(&Q, NDStates);
				Dtran[t][a] = NDStates;
			}
			else			
			Dtran[t][a] = u;
		}
	}	
}

typedef struct _stateforward
{
	int state;
	int forward;
}STATE_and_FORWARD;

int isFinal(int state)
{
	int i;
	for(i=0;i<NDFinalStates;++i)
	if(state==DFinalStates[i].state)
	return 1;
	
	return 0;
}

//for detecting comments
int comment(char input[], int lexemeBegin)
{
	//start of comment
	if(input[lexemeBegin]=='\0')
		return lexemeBegin;
	if(input[lexemeBegin]=='/'&&input[lexemeBegin+1]=='*')
		lexemeBegin+=2;
	else return lexemeBegin;
	
	//end of comment
	while(!(input[lexemeBegin]=='*'&&input[lexemeBegin+1]=='/'))
		lexemeBegin++;
	lexemeBegin+=2;
	while(input[lexemeBegin]==' ' || input[lexemeBegin]=='\t')
		lexemeBegin++;
			
	return lexemeBegin;
}

STATE_and_FORWARD simulateDFA(char input[], int forward) 
{
	int i, s;
	STATE_and_FORWARD max = {-1,-1};
	s = 1;
	if(isFinal(s))
	{
		max.state = s;
		max.forward = forward-1;
	}
	while(input[forward]!='\0' &&input[forward]!=' ' &&input[forward]!='\n')
	{
		if(input[forward+1]!='\0')
		{
			if(input[forward]=='/' && input[forward+1]=='*')
				break;
		}
		s = Dtran[s][input[forward]];
		if(isFinal(s))
		{
			max.state = s;
			max.forward = forward;
		}
		else if(s==0)
			break;	
		forward++;
	}
	return max;
}
 
/*DFA construction and simulation ends here*/ 

int main()
{
	int i;
	FILE * fp1;
	char PATTERN[100];
	char REGEX[100];
	char * POSTFIX;	
	
	fp1 = fopen("regexp.in", "r");
	if(fp1==NULL)
	{
		printf("Could not open regexp.in!!");
		exit(0);
	}
	
	while(!feof(fp1))
	{
		fscanf(fp1, "%s %s",PATTERN, REGEX);
		if(strlen(REGEX)==1 || (strlen(REGEX)==2 && REGEX[1]=='*'))
		{
			buildIndividualNFA(PATTERN, REGEX);
			continue;
		}
		else if(strlen(REGEX)==2 && REGEX[1]!='*')
		{
			REGEX[2]='&';
			REGEX[3]='\0';
			buildIndividualNFA(PATTERN, REGEX);
			continue;
		}
		POSTFIX = conversion(REGEX);
		buildIndividualNFA(PATTERN, POSTFIX);			
	}
	fclose(fp1);	
	
	buildCombinedNFA();
	buildDFA();		
	
	char input[1000];
	char tokenread[100];	
	
	initialize_Symbol_Table();
	remove("a4_3.out");
	
	while(gets(input))
	{
		int lexemeBegin = 0;
		while(input[lexemeBegin]!='\0'&&input[lexemeBegin]!='\n' && input[lexemeBegin]!='\r')  //while end of line is not found
		{			
			//Check for space in simulate
			while(input[lexemeBegin]==' ' || input[lexemeBegin]=='\t')
				lexemeBegin++;
			lexemeBegin = comment(input, lexemeBegin);
			
			if(input[lexemeBegin] == '\0' || input[lexemeBegin]=='\n' || input[lexemeBegin]=='\r')
				break;
			
			STATE_and_FORWARD final = simulateDFA(input, lexemeBegin);
			
			i = final.forward - lexemeBegin+1;
			memcpy(tokenread, input+lexemeBegin, i);
			tokenread[i] = '\0';
			token t;//Token			
			attrType a;//attribute field
			a.string = (char *)malloc((i+1)*sizeof(char));
			
			if(final.state == -1)
			{
				lexemeBegin++;
				continue;
			}				
			int tokenID = getTokenID(final.state);
			if(tokenID == IDNTIFIER)
			{
				strcpy(a.string, tokenread);
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
					fp1 = fopen("symbol_table_3.out","r");		
					int nflag = 0;
		
					char identifier[100];
					int marker;
					while(!feof(fp1))
					{
						fscanf(fp1,"%s %d",identifier,&marker);
			
						if(strcmp(identifier,a.string)==0)           //if identifer is already present in symbol table
						{
							nflag = 1;
							break;
						}
					}
					fclose(fp1);
		
					if(nflag==0)               //identifier not present in symbol table
					{
						fp1 = fopen("symbol_table_3.out","a");
						fprintf(fp1,"%s 1\n",a.string);
						fclose(fp1);
					}
					t.attribute = a;
				}	
			}
			else if(tokenID == INT_CONST)
			{
				a.integer = atoi(tokenread);
				t.tokenID = INT_CONST;
				t.attribute = a;
			}
			else if(tokenID == FLO_CONST)
			{
				a.integer = atof(tokenread);
				t.tokenID = FLO_CONST;
				t.attribute = a;
			}
			else if(tokenID!=NOTOK)
			{
				t.tokenID = tokenID;
				t.attribute.string = '\0';
			}
			
			if(tokenID != NOTOK)
			write_token(t);
			else
			{
				lexemeBegin++;
				continue;
			}
			
			lexemeBegin = final.forward+1;			
		}
	}			
}
