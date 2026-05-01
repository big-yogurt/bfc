#include <stdio.h>
#include <errno.h>
#include <string.h>

#include "utils.h"
#include "lexer.h"
#include "parser.h"

int main(void)
{
    const char* code = "++++[[-+[-]]]";
    FILE* fd = fmemopen((void*) code, strlen(code), "r");
    if (NULL == fd) {
        eprintf("error: %s", strerror(errno));
        return -1;
    }
    Lexer l = Lexer_new(fd);
    Parser p = Parser_new(&l);

    AstNode* ast = Parser_parse(&p);
    Ast_free(ast);

    fclose(fd);

    return 0;
}
