This contains 3 C codes, 3 executable files, 6 output files alongwith def.h, input.in and regexp.in.
1. C codes:
a. FSM.c - It is the C code for part1.
b. NFA.c - It is the C code for part2.
c. DFA.c - It is the C code for part3.

2. Executable files:
a. FSM - used as    ./FSM <input.in    in terminal.
b. NFA - used as    ./NFA <input.in    in terminal.
c. DFA - used as    ./DFA <input.in    in terminal.
These generate the Symbol Table and the Output files.

3. Output files:
a. a4_1.out - The file containing tokens for the sample input.in using FSM. 
b. a4_2.out - The file containing tokens for the sample input.in using NFA.
c. a4_3.out - The file containing tokens for the sample input.in using DFA.
d. symbol_table_1.out - The symbol table generated for the sample input.in using FSM.
e. symbol_table_2.out - The symbol table generated for the sample input.in using NFA.
f. symbol_table_3.out - The symbol table generated for the sample input.in using DFA.

***Method used for Part b:***
We create a state transition table which represents the transitions we can make from one state to another 
for any given symbol. We create individual NFA's for each regular expression of regexp.in and then 
combine all NFA's to form one single NFA. We store the final states and the corresponding pattern represented
by the final state i.e. IDNTIFIER, INT_CONST etc.
Now we simulate the NFA for each line of the redirected input file and keep on making tokens
and updating the Symbol Table.The Simulation is done using Depth First Search in the State Transition Table
starting from the unique start state of the Combined NFA. We remember the position of longest prefix match and
that is returned alongwith the final state and then we form the token using above information.

***Method used for Part c:***
We use the combined NFA created above to form the DFA using subset construction.
The subsets formed can be exponential. So to avoid that whenever we form a subset we check if it matches an existing DFA state.
If it does not match, then only we add it to the DFA as new state and insert it into the Queue so that it will be processed later.
Now we simulate each line of the redirected input on the DFA nd keep making tokens.
We remember the position of longest prefix match and it is returned alongwith the final state. Now we can easily construct tokens using above information .
