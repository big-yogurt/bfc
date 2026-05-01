CC=gcc

.PHONY: all tests clean

all: codegen.o
	$(CC) codegen.c lexer.c parser.c main.c -o bfc

test_lexer:
	$(CC) test_lexer.c lexer.c -o test_lexer
	./test_lexer

tests: test_lexer

clean:
	rm test_lexer bfc
