#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "lexer.h"
#include "darr.h"

static bool test_is_passed;

#define test_beg(name) \
    test_is_passed = true; \
    printf("RUN %s\n", name); \
    do

#define test_end() \
    while (0); \
    printf("%s\n", test_is_passed ? "PASSED" : "FAILED");

#define test_errorf(fmt, ...) \
    do { \
        test_is_passed = false; \
        printf("\t"); \
        printf(fmt, ##__VA_ARGS__); \
        printf("\n"); \
    } while (0)

/*
    Take 'src', run it throught Lexer, and compare the resulting tokens with
    'want_toks'.

    Arguments:
        src - Source code.
        want_toks - Array of tokens. Always ends with TokenKind_EOF or
        TokenKind_ERR.

    Return:
        true if resulting tokens are equal to 'toks'; otherwise false.
*/
bool cmp_src_toks(const char* src, Token* want_toks)
{
    FILE* fd = fmemopen((void*) src, strlen(src), "r");
    panic_if(NULL == fd, "%s", strerror(errno));
    Lexer lexer = Lexer_new(fd);
    for (size_t i = 0; ; ++i) {
        Token want_tok = want_toks[i];
        Token got_tok = Lexer_next_tok(&lexer);

        // Check Token.kind
        if (want_tok.kind != got_tok.kind) {
            test_errorf("token.kind = %s; want %s",
                TokenKind_strings[got_tok.kind], TokenKind_strings[want_tok.diff]
            );
            return false;
        }

        // Check Token.diff
        if (want_tok.diff != got_tok.diff) {
            test_errorf("token.diff = %ld; want %ld", got_tok.diff,
                want_tok.diff);
            return false;
        }

        // Check TokenKind_EOF and TokenKind_ERR.
        if (want_tok.kind == TokenKind_EOF || want_tok.kind == TokenKind_ERR) {
            break;
        }
    }
    fclose(fd);
    return true;
}

int main(void)
{
    struct {
        const char* name;
        const char* src;
        Token *toks;
    } tests[] = {
        {
            "Test command plus",
            "+",
            (Token[]) {
                {TokenKind_PLUS, 1},
                {TokenKind_EOF, 1}
            }
        },
        {
            "Test command minus",
            "-",
            (Token[]) {
                {TokenKind_MINUS, 1},
                {TokenKind_EOF, 1}
            }
        },
        {
            "Test command previous cell",
            "<",
            (Token[]) {
                {TokenKind_L_AB, 1},
                {TokenKind_EOF, 1}
            }
        },
        {
            "Test command next cell",
            ">",
            (Token[]) {
                {TokenKind_R_AB, 1},
                {TokenKind_EOF, 1}
            }
        },
        {
            "Test command loop begin",
            "[",
            (Token[]) {
                {TokenKind_L_B, 1},
                {TokenKind_EOF, 1}
            }
        },
        {
            "Test command loop end",
            "]",
            (Token[]) {
                {TokenKind_R_B, 1},
                {TokenKind_EOF, 1}
            }
        },
        {
            "Test command input",
            ",",
            (Token[]) {
                {TokenKind_COMMA, 1},
                {TokenKind_EOF, 1}
            }
        },
        {
            "Test command output",
            ".",
            (Token[]) {
                {TokenKind_DOT, 1},
                {TokenKind_EOF, 1}
            }
        },
        {
            "Test lexer merges commands",
            "++-+",
            (Token[]) {
                {TokenKind_PLUS, 2},
                {TokenKind_EOF, 1}
            }
        },
        {
            "Test errors",
            "+/",
            (Token[]) {
                {TokenKind_PLUS, 1},
                {TokenKind_ERR, 1}
            }
        },
        {
            "Test comments ignoring",
            "+ // coment\n+",
            (Token[]) {
                {TokenKind_PLUS, 2},
                {TokenKind_EOF, 1}
            }
        },
    };

    for (size_t i = 0; i < sizeof(tests) / sizeof(tests[0]); ++i) {
        test_beg(tests[i].name) {
            cmp_src_toks(tests[i].src, tests[i].toks);
        } test_end();
    }

    return 0;
}
