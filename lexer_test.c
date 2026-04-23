#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>

#include "darr.h"
#include "lexer.h"

bool test_pair(const char* code, TokenKind* toks)
{
    FILE* fd = fmemopen((void*) code, strlen(code), "r");
    panic_if(NULL == fd, "%s", strerror(errno));

    Lexer l = Lexer_new(fd);

    for (size_t i = 0; ; ++i) {
        TokenKind t1 = toks[i];
        TokenKind t2 = Lexer_next_tok(&l).kind;
        if (t1 != t2) {
            return false;
        }
        if (TokenKind_EOF == t1 || TokenKind_ERR == t1) {
            break;
        }
    }

    return true;
}

#define test(name, ...) \
    printf("[%s] %s\n", test_pair(__VA_ARGS__) ? "PASSED" : "FAILED", name)

int main(void)
{
    test(
        "+-<>[],.",
        "+-<>[],.",
        (TokenKind[]) {
            TokenKind_PLUS,
            TokenKind_MINUS,
            TokenKind_L_AB,
            TokenKind_R_AB,
            TokenKind_L_B,
            TokenKind_R_B,
            TokenKind_COMMA,
            TokenKind_DOT,
            TokenKind_EOF,
        }
    );

    test(
        "+-<>[//test\\n],.",
        "+-<>[//test\n],.",
        (TokenKind[]) {
            TokenKind_PLUS,
            TokenKind_MINUS,
            TokenKind_L_AB,
            TokenKind_R_AB,
            TokenKind_L_B,
            TokenKind_R_B,
            TokenKind_COMMA,
            TokenKind_DOT,
            TokenKind_EOF,
        }
    );

    test(
        "/+-<>[],.",
        "/+-<>[],.",
        (TokenKind[]) {
            TokenKind_ERR,
        }
    );

    return 0;
}
