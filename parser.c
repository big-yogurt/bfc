#include "darr.h"
#include "utils.h"
#include "lexer.h"
#include "parser.h"

static AstNode* Parser__parse_command(Parser* p);
static AstNode* Parser__parse_loop(Parser* p);

/*
    Create AST node.

    Arguments:
        kind - Kind of AST node.

    Return:
        AST node as point to struct.
*/
static AstNode* AstNode__new(AstNodeKind kind)
{
    AstNode* node = (AstNode*) malloc(sizeof(AstNode));
    panic_if(NULL == node, "AstNode_new(): Cannot allocate memory");

    node->kind = kind;
    node->items = NULL;
    node->cap = 0;
    node->len = 0;

    return node;
}

/*
    Free the allocated node.

    Arguments:
        node - point to AST node.
*/
static void AstNode__free(AstNode* node)
{
    free(node->items);
    free(node);
}

void Ast_free(AstNode* ast)
{
    if (NULL == ast) {
        return;
    }

    darr_foreach(AstNode*, child, ast) {
        Ast_free(*child);
    }
    AstNode__free(ast);
}

Parser Parser_new(Lexer* l)
{
    panic_if(NULL == l, "Parser_new(): Lexer cannot be NULL");

    return (Parser) {
        .l = l,
    };
}

/*
    Parse one token and create the AST node.

    Arguments:
        p - Pointer to parser.

    Return:
        AST node on success.
        NULL on errors.
*/
static AstNode* Parser__parse_command(Parser* p)
{
    panic_if(NULL == p, "Parser__parse_command(): Parser cannot be NULL");
    panic_if(NULL == p->l, "Parser__parse_command(): Lexer cannot be NULL");

    AstNode* command;
    Token tok = Lexer_next_tok(p->l);
    switch (tok.kind) {
        case TokenKind_EOF:
            return NULL;
        case TokenKind_ERR:
            exit(-1);
        case TokenKind_PLUS:
            command = AstNode__new(AstNodeKind_INC);
            break;
        case TokenKind_MINUS:
            command = AstNode__new(AstNodeKind_DEC);
            break;
        case TokenKind_L_AB:
            command = AstNode__new(AstNodeKind_BACK);
            break;
        case TokenKind_R_AB:
            command = AstNode__new(AstNodeKind_NEXT);
            break;
        case TokenKind_COMMA:
            command = AstNode__new(AstNodeKind_IN);
            break;
        case TokenKind_DOT:
            command = AstNode__new(AstNodeKind_OUT);
            break;
        case TokenKind_L_B: {
            command = Parser__parse_loop(p);
            break;
        case TokenKind_R_B:
            command = AstNode__new(AstNodeKind_LOOP_END);
            break;
        }
    }
    command->diff = tok.diff;

    return command;
}

/*
    Parse loop.

    Arguments:
        p - Pointer to the parser.

    Return:
        AST node on success.
        NULL on error.
*/
static AstNode* Parser__parse_loop(Parser* p)
{
    panic_if(NULL == p, "Parser__parse_command(): Parser cannot be NULL");
    panic_if(NULL == p->l, "Parser__parse_command(): Lexer cannot be NULL");

    AstNode* loop = AstNode__new(AstNodeKind_LOOP);

    AstNode* tmp;
    while (1) {
        tmp = Parser__parse_command(p);
        if (NULL == tmp) {
            eprintf("error: Unexpected EOF. Expected loop end.\n");
            exit(-1);
        }
        if (AstNodeKind_LOOP_END == tmp->kind) {
            break;
        }
        darr_append(loop, tmp);
    }
    free(tmp); // free AstNodeKind_LOOP_END

    return loop;
}

AstNode* Parser_parse(Parser* p)
{
    panic_if(NULL == p, "Parser_parse(): Parser cannot be NULL");
    panic_if(NULL == p->l, "Parser_parse(): Lexer cannot be NULL");

    AstNode* root = AstNode__new(AstNodeKind_PROG);
    while (1) {
        AstNode* command = Parser__parse_command(p);
        if (NULL == command) {
            break;
        }
        if (AstNodeKind_LOOP_END == command->kind) {
            eprintf("error: Unexpected loop end.\n");
            exit(-1);
        }
        darr_append(root, command);
    }

    return root;
}
