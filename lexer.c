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
    Peek one character from source.

    Arguments:
        l - Pointer to lexer.

    Return:
        Next read character.
        Set has_err on error.
*/
static int Lexer__peek_char(Lexer* l)
{
    panic_if(NULL == l, "Lexer__peek_char(): Lexer cannot be NULL");
    panic_if(NULL == l->fd, "Lexer__peek_char(): fd cannot be NULL");

    int ch = Lexer__read_char(l);
    ungetc(ch, l->fd);
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

/*
    Merge sequence of commands.

    Example:
        '++++' ->  Token {.kind = TokenKind_INC, .diff = 4}
        '++-+' ->  Token {.kind = TokenKind_INC, .diff = 2}
        '..' ->    Token {.kind = TokenKind_DOT, .diff = 2}
        '>>>>>' -> Token {.kind = TokenKind_R_AB, .diff = 5}

    Arguments:
        l - Pointer to lexer. Cannot be NULL.
        diff - Pointer to the field .kind in the Token.
        inc_diff_command - command for increment diff value. 0 - command not set.
        dec_diff_command - command for decrement diff value. 0 - command not set.

*/
static void Lexer__merge_sequence(Lexer* l, int* diff, char inc_diff_cmd,
    char dec_diff_cmd)
{
    panic_if(NULL == l, "Lexer__merge_sequence(): Lexer cannot be NULL");

    while (true) {
        char ch = Lexer__peek_char(l);
        if (inc_diff_cmd != 0 && inc_diff_cmd == ch) {
            *diff += 1;
        } else if (dec_diff_cmd != 0 && dec_diff_cmd == ch) {
            *diff -= 1;
        } else {
            break;
        }
        Lexer__read_char(l); // Skip
    }
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
            Lexer__merge_sequence(l, &tok.diff, '+', '-');
            break;
        case '-':
            tok.kind = TokenKind_MINUS;
            Lexer__merge_sequence(l, &tok.diff, '-', '+');
            break;
        case '<':
            tok.kind = TokenKind_L_AB;
            Lexer__merge_sequence(l, &tok.diff, '<', '>');
            break;
        case '>':
            tok.kind = TokenKind_R_AB;
            Lexer__merge_sequence(l, &tok.diff, '>', '<');
            break;
        case '[':
            tok.kind = TokenKind_L_B;
            break;
        case ']':
            tok.kind = TokenKind_R_B;
            break;
        case ',':
            tok.kind = TokenKind_COMMA;
            Lexer__merge_sequence(l, &tok.diff, ',', 0);
            break;
        case '.':
            tok.kind = TokenKind_DOT;
            Lexer__merge_sequence(l, &tok.diff, '.', 0);
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
