#include <errno.h>
#include <string.h>

#include "darr.h"
#include "utils.h"
#include "codegen.h"

static void _AstNode_to_asm(FILE* fd, AstNode* node, size_t loop_count)
{
    switch (node->kind) {
        case AstNodeKind_INC:
            fprintf(fd, "\tadd byte[tape+rbx], 1\n");
            break;
        case AstNodeKind_DEC:
            fprintf(fd, "\tsub byte[tape+rbx], 1\n");
            break;
        case AstNodeKind_NEXT:
            fprintf(fd, "\tinc rbx\n");
            break;
        case AstNodeKind_BACK:
            fprintf(fd, "\tdec rbx\n");
            break;
        case AstNodeKind_IN:
            fprintf(fd, "\tcall read_char\n");
            break;
        case AstNodeKind_OUT:
            fprintf(fd, "\tcall print_char\n");
            break;
    }
}

static void _Ast_to_asm(FILE* fd, AstNode* ast)
{
    size_t loop_count = 0; // for labels

    switch (ast->kind) {
        /*
        case AstNodeKind_LOOP:
            loop_count += 1;
            darr_foreach(AstNode*, node, ast) {
                _AstNode_to_asm(fd, *node, loop_count);
            }
            break;
            */
        default:
            darr_foreach(AstNode*, node, ast) {
                _AstNode_to_asm(fd, *node, 0);
            }
    }
}

void codegen(const char* src, const char *out)
{
    FILE* src_fd = fopen(src, "r");
    if (NULL == src_fd) {
        eprintf("error: %s\n", strerror(errno));
        exit(-1);
    }
    FILE* out_fd = fopen(out, "w");
    if (NULL == out_fd) {
        eprintf("error: %s\n", strerror(errno));
        exit(-1);
    }

    // start program
    fprintf(out_fd, "format ELF64 executable\n");
    fprintf(out_fd, "entry _start\n\n");

    fprintf(out_fd, "print_char:\n");
    fprintf(out_fd, "\tpush rdx\n"); // !!!!
    fprintf(out_fd, "\tmov rsi, [tape+rbx]\n");
    fprintf(out_fd, "\tmov [rsp], rsi\n");
    fprintf(out_fd, "\tmov rsi, rsp\n");
    fprintf(out_fd, "\tmov rdi, 1\n");
    fprintf(out_fd, "\tmov rdx, 1\n");
    fprintf(out_fd, "\tmov rax, 1\n");
	fprintf(out_fd, "\tsyscall\n");
    fprintf(out_fd, "\tpop rdx\n");
	fprintf(out_fd, "\tret\n\n");

    fprintf(out_fd, "read_char:");
	fprintf(out_fd, "\txor rdi, rdi\n");
	fprintf(out_fd, "\tmov rsi, tape\n");
    fprintf(out_fd, "\tadd rsi, rbx\n");
	fprintf(out_fd, "\tmov rdx, 1\n");
	fprintf(out_fd, "\txor rax, rax\n");
	fprintf(out_fd, "\tsyscall\n");
	fprintf(out_fd, "\tret\n\n");

    fprintf(out_fd, "_start:\n");
    fprintf(out_fd, "\txor rbx, rbx\n");

    Lexer l = Lexer_new(src_fd);
    Parser p = Parser_new(&l);
    AstNode* ast = Parser_parse(&p);
    _Ast_to_asm(out_fd, ast);
    Ast_free(ast);

    // exit
    fprintf(out_fd, "\tmov rax, 60\n");
    fprintf(out_fd, "\txor rdi, rdi\n");
    fprintf(out_fd, "\tsyscall\n\n");
    fprintf(out_fd, "tape: rb 30000\n");

    fclose(src_fd);
    fclose(out_fd);
}
