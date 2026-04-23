#ifndef PARSER_H
#define PARSER_H

#include <stddef.h>

#include "lexer.h"

typedef enum AstNodeKind {
    AstNodeKind_PROG,
    AstNodeKind_LOOP,
    AstNodeKind_LOOP_END,
    AstNodeKind_INC,
    AstNodeKind_DEC,
    AstNodeKind_NEXT,
    AstNodeKind_BACK,
    AstNodeKind_IN,
    AstNodeKind_OUT,
} AstNodeKind;

typedef struct AstNode {
   AstNodeKind kind;

   // Childs
   struct AstNode** items;
   size_t cap; // Capacity of items
   size_t len; // Length of items
} AstNode;

/*
    Free AST.

    Arguments:
        ast - root node of AST.
*/
void Ast_free(AstNode* ast);

typedef struct Parser {
    Lexer* l;
} Parser;

/*
    Create new parser.

    Arguments:
        l - Lexer. Cannot be NULL.

    Return:
        Parser structure
*/
Parser Parser_new(Lexer* l);

/*
    Parse tokens from lexer. Return AST.

    Arguments:
        p - Pointer to parser (self). Cannot be NULL.

    Return:
        AST of program.
*/
AstNode* Parser_parse(Parser* p);

#endif // PARSER_H
