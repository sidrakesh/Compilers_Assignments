typedef struct _matrix
{
	char id[100];
	int row,col;
}MATRIX;
/*
typedef struct _sym
{
	MATRIX arr[100];
	int size;
}sym;
sym SYMTAB;
SYMTAB.size = 0;
*/

MATRIX SYMTAB[100];
int size = 0;

int isInSYMTAB(char id[])
{
	int i;
	for(i=0;i<size;++i)
	{
		if(strcmp(SYMTAB[i].id, id)==0)
		return 1;
	}
	return 0;
}

//First check isInSYMTAB()
void getDimension(char id[], int * row, int * col)
{
	int i;
	for(i=0;i<size;++i)
	{
		if(strcmp(SYMTAB[i].id, id)==0)
		{
			*row = SYMTAB[i].row;
			*col = SYMTAB[i].col;
			return;
		}
	}
}

//First check isInSYMTAB()
void insertSYMTAB(char id[], int row, int col)
{
	int i;
	MATRIX temp;
	strcpy(temp.id, id);
	temp.row = row;
	temp.col = col;
	
	SYMTAB[size] = temp;
	size++;	
}