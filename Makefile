CC=gcc

all:
	$(CC) codegen.c lexer.c parser.c main.c -o bfc
