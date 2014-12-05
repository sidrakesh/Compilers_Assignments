
typedef struct _variable
{
	char id[100];
	char type[10];
	int offset;
}Variable;

typedef struct _list{

	int line;
	struct _list * next;
}List;

int isInSYMTAB(Variable SYMTAB[], int size, char id[]);
Variable * insertSYMTAB(Variable SYMTAB[], int * size, char id[]);
void addInfo(Variable SYMTAB[], int size, char id[], char type[], int offset);
char * getType(Variable SYMTAB[], int size, char id[]);
char * newTemp();
char * maxType(char type1[], char type2[]);
char * widen(char a[], char typeFrom[], char typeTo[]); 
void backpatch( char ThreeAddress[][20][20], List * l, int p);
List * merge (List * l1, List * l2);
List * makelist(int p);
void printThreeAddressCodeToFile(char ThreeAddress[][20][20], int nextInstr);
void printSymbolTableToFile(Variable SYMTAB[], int size);
