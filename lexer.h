#ifndef LEXER_H
#define LEXER_H

#include <stdio.h>
#include <stdbool.h>

typedef struct Lexer {
    FILE* fd; // source code as stream of chars
    bool has_err;
} Lexer;

typedef enum TokenKind {
    TokenKind_EOF = 0,
    TokenKind_ERR,
    TokenKind_PLUS,  // +
    TokenKind_MINUS, // -
    TokenKind_L_AB,  // <
    TokenKind_R_AB,  // >
    TokenKind_L_B,   // [
    TokenKind_R_B,   // ]
    TokenKind_COMMA, // ,
    TokenKind_DOT,   // .
} TokenKind;

static const char* TokenKind_strings[] = {
    [TokenKind_EOF]   = "EOF",
    [TokenKind_ERR]   = "ERR",
    [TokenKind_PLUS]  = "PLUS",
    [TokenKind_MINUS] = "MINUS",
    [TokenKind_L_AB]  = "L_AB",
    [TokenKind_R_AB]  = "R_AB",
    [TokenKind_L_B]   = "L_B",
    [TokenKind_R_B]   = "R_B",
    [TokenKind_COMMA] = "COMMA",
    [TokenKind_DOT]   = "DOT",
};

typedef struct Token {
    TokenKind kind;

    /*
        Difference between disired value and current value of the cell.
    */
    int diff;
} Token;

/*
    Create new Lexer.
    Lexer handle file as a stream of chars.

    fd - file descriptor of source file. Cannot be NULL.

*/
Lexer Lexer_new(FILE* fd);

/*
    Returns next token.
    When is EOF returns TokenKind_EOF.
    When there are errors returns TokenKind_ERR.
*/
Token Lexer_next_tok(Lexer* l);

#endif // LEXER_H
