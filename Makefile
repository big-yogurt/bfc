CC=gcc

all:
	$(CC) codegen.c lexer.c parser.c main.c -o bfc

lexer_test:
	$(CC) lexer_test.c lexer.c -o test_lexer
	./test_lexer
