#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>

#include "darr.h"
#include "lexer.h"
#include "utils.h"

bool test_pair(const char* code, Token* toks)
{
    FILE* fd = fmemopen((void*) code, strlen(code), "r");
    panic_if(NULL == fd, "%s", strerror(errno));

    Lexer l = Lexer_new(fd);

    for (size_t i = 0; ; ++i) {
        Token t1 = toks[i];
        Token t2 = Lexer_next_tok(&l);
        if (t1.kind != t2.kind) {
            return false;
        }
        if (t1.diff != t2.diff) {
            return false;
        }
        if (TokenKind_EOF == t1.kind || TokenKind_ERR == t1.kind) {
            break;
        }
    }

    return true;
}

#define test(name, ...) \
    do { \
        bool test_result = test_pair(__VA_ARGS__); \
        printf("[%s] %s\n", test_result ? "PASSED" : "FAILED", name); \
        if (!test_result) { \
            exit(-1); \
        } \
    } while (0)

int main(void)
{
    test(
        "+-<>[],.",
        "+-<>[],.",
        (Token[]) {
            {.kind = TokenKind_L_B, .diff = 1},
            {.kind = TokenKind_R_B, .diff = 1},
            {.kind = TokenKind_COMMA, .diff = 1},
            {.kind = TokenKind_DOT, .diff = 1},
            {.kind = TokenKind_EOF, .diff = 1},
        }
    );

    test(
        "+-<>[//test\\n],.",
        "+-<>[//test\n],.",
        (Token[]) {
            {.kind = TokenKind_L_B, .diff = 1},
            {.kind = TokenKind_R_B, .diff = 1},
            {.kind = TokenKind_COMMA, .diff = 1},
            {.kind = TokenKind_DOT, .diff = 1},
            {.kind = TokenKind_EOF, .diff = 1},
        }
    );

    test(
        "/+-<>[],.",
        "/+-<>[],.",
        (Token[]) {
            {.kind = TokenKind_ERR, .diff = 1},
        }
    );

    test(
        "..",
        "..",
        (Token[]) {
            {.kind = TokenKind_DOT, .diff = 2},
            {.kind = TokenKind_EOF, .diff = 1},
        }
    );

    test(
        ",,",
        ",,",
        (Token[]) {
            {.kind = TokenKind_COMMA, .diff = 2},
            {.kind = TokenKind_EOF, .diff = 1},
        }
    );

    return 0;
}
