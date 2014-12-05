This Assignment contains 1 flex code, 1 bison code, 1 header file, 3 input text files and 1 makefile.

Note: The labels have been represented by the line number of the quadruples(1 based indexing) in output.txt
eg. goto 15
means goto quadruple at line 15 in output.txt 

Note: We have provided support for explicit type casting.

1. 25.9.l - This is the flex code that generates the lexical analyzer required to get terminals.

2. 25.9.y - This is the bison code which generates the parser required to generate the 3 Address code for the input file.

3. symbol.h - Header file containing symbol table definition and prototypes of functions.

4. input1.txt, input2.txt, input3.txt - These are the sample input files.
The input files provided in the assignment were not accepted by the grammar given due to some extra SEMICOLON's.
So the extra SEMICOLON's were removed.

5. makefile - The makefile for the source codes. It generates an executable tac.
Used as : ./tac <input1.txt 

This command generates output.txt and symtab.txt
output.txt - This is the quadruple representation of the Three Address Codes generated for the input file.
symtab.txt - This is the symbol table generated for the input file.

Any kind of errors are printed on the terminal.

