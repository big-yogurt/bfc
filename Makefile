CC=gcc

all:
	$(CC) codegen.c lexer.c parser.c main.c -o bfc

test_lexer:
	$(CC) test_lexer.c lexer.c -o test_lexer
	./test_lexer
