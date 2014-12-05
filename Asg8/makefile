all:
	bison -d 25.8.y
	flex 25.8.l
	cc lex.yy.c 25.8.tab.c -lfl -o lalrParser