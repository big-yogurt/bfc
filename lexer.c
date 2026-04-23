#include <stdio.h>
#include <errno.h>
#include <string.h>

#include "darr.h"
#include "lexer.h"
#include "utils.h"

Lexer Lexer_new(FILE* fd)
{
    panic_if(NULL == fd, "Lexer_new(): fd cannot be NULL");

    return (Lexer) {
        .fd = fd,
    };
}

/*
    Read one character from source.

    Arguments:
        l - Pointer to lexer.

    Return:
        Character.
        Set has_err on error.
*/
static int Lexer__read_char(Lexer* l)
{
    panic_if(NULL == l, "Lexer__read_char(): Lexer cannot be NULL");
    panic_if(NULL == l->fd, "Lexer__read_char(): fd cannot be NULL");

    int ch = fgetc(l->fd);
    if (ferror(l->fd)) {
        l->has_err = true;
        eprintf("error: %s\n", strerror(errno));
    }

    return ch;
}

/*
    Skip line. Use for skip comments.

    Arguments:
        l - Pointer to lexer.

    Return:
        Set has_err on error.
*/
static void Lexer__skip_line(Lexer* l)
{
    panic_if(NULL == l, "Lexer__skip_line(): Lexer cannot be NULL");

    for (
        int ch = Lexer__read_char(l);
        ch != '\n' && ch != EOF;
        ch = Lexer__read_char(l)
    );
}

Token Lexer_next_tok(Lexer* l)
{
    panic_if(NULL == l, "Lexer_next_tok(): Lexer cannot be NULL");
    panic_if(NULL == l->fd, "Lexer_next_tok(): fd cannot be NULL");

    Token tok;

start_tokenize:
    char ch = Lexer__read_char(l);
    if (l->has_err) {
        tok.kind = TokenKind_ERR;
        return tok;
    }

    switch (ch) {
        case EOF:
            tok.kind = TokenKind_EOF;
            break;
        case '+':
            tok.kind = TokenKind_PLUS;
            break;
        case '-':
            tok.kind = TokenKind_MINUS;
            break;
        case '<':
            tok.kind = TokenKind_L_AB;
            break;
        case '>':
            tok.kind = TokenKind_R_AB;
            break;
        case '[':
            tok.kind = TokenKind_L_B;
            break;
        case ']':
            tok.kind = TokenKind_R_B;
            break;
        case ',':
            tok.kind = TokenKind_COMMA;
            break;
        case '.':
            tok.kind = TokenKind_DOT;
            break;
        case '/': {
            int ch = Lexer__read_char(l);
            if (ch != '/') {
                tok.kind = TokenKind_ERR;
                eprintf("error: Expected comment\n");
                break;
            }
            Lexer__skip_line(l);
            goto start_tokenize;
            break;
        }
        case '\n':
        case ' ':
            goto start_tokenize;
        default:
            tok.kind = TokenKind_ERR;
            eprintf("error: Undefined symbol '%c' (%d) \n", ch, ch);
            break;
    } 

    return tok;
}
