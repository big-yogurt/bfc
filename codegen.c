#include <errno.h>
#include <string.h>

#include "darr.h"
#include "utils.h"
#include "codegen.h"

static void _gen_header(FILE* fd)
{
    fprintf(fd, "format ELF64 executable\n");
    fprintf(fd, "entry _start\n\n");
}

static void _gen_print_function(FILE* fd)
{
    fprintf(fd,
        "print:\n"
        "    push rdx\n"
        "    mov rsi, [tape+rbx]\n"
        "    mov [rsp], rsi\n"
        "    mov rsi, rsp\n"
        "    mov rdi, 1\n"
        "    mov rdx, 1\n"
        "    mov rax, 1\n"
        "    syscall\n"
        "    pop rdx\n"
        "    ret\n\n"
    );
}

static void _gen_input_function(FILE* fd)
{
    fprintf(fd,
        "input:"
        "    xor rdi, rdi\n"
        "    mov rsi, tape\n"
        "    add rsi, rbx\n"
        "    mov rdx, 1\n"
        "    xor rax, rax\n"
        "    syscall\n"
        "    ret\n\n"
    );
}

static void _gen_help_functions(FILE* fd)
{
    _gen_print_function(fd);
    _gen_input_function(fd);
}

static void _gen_command(FILE* fd, AstNode* node)
{
    panic_if(AstNodeKind_PROG == node->kind, "_gen_command(): node is PROG\n");
    panic_if(AstNodeKind_LOOP == node->kind, "_gen_command(): node is LOOP\n");

    switch (node->kind) {
        case AstNodeKind_INC:
            fprintf(fd, "    add byte[tape+rbx], %d\n", node->diff);
            break;
        case AstNodeKind_DEC:
            fprintf(fd, "    sub byte[tape+rbx], %d\n", node->diff);
            break;
        case AstNodeKind_NEXT:
            fprintf(fd, "    add rbx, %d\n", node->diff);
            break;
        case AstNodeKind_BACK:
            fprintf(fd, "    sub rbx, %d\n", node->diff);
            break;
        case AstNodeKind_IN:
            for (size_t i = 0; i < node->diff; ++i) {
                fprintf(fd, "    call input\n");
            }
            break;
        case AstNodeKind_OUT:
            for (size_t i = 0; i < node->diff; ++i) {
                fprintf(fd, "    call print\n");
            }
            break;
    }
}

static size_t loop_count = 0;
static void _gen_from_ast_node(FILE* fd, AstNode* node)
{
    if (AstNodeKind_LOOP == node->kind) {
        size_t local_loop_count = loop_count;
        fprintf(fd, "    jmp .L%ld\n", local_loop_count+1);
        fprintf(fd, ".L%ld:\n", local_loop_count);
        loop_count += 2;
        darr_foreach(AstNode*, child, node) {
            _gen_from_ast_node(fd, *child);
        }
        fprintf(fd, ".L%ld:\n", local_loop_count+1);
        fprintf(fd, "    cmp byte [tape+rbx], 0\n");
        fprintf(fd, "    jne .L%ld\n", local_loop_count);
        return;
    }

    _gen_command(fd, node);
}

static void _gen_from_ast(FILE* fd, AstNode *ast)
{
    panic_if(
        AstNodeKind_PROG != ast->kind,
        "_gen_from_ast(): root node is not PROG\n"
    );

    darr_foreach(AstNode*, node, ast) {
        _gen_from_ast_node(fd, *node);
    }
}

void codegen(const char* src, const char *out)
{
    FILE* src_fd = fopen(src, "r");
    if (NULL == src_fd) {
        eprintf("error: %s\n", strerror(errno));
        return;
    }
    FILE* out_fd = fopen(out, "w");
    if (NULL == out_fd) {
        eprintf("error: %s\n", strerror(errno));
        return;
    }

    Lexer l = Lexer_new(src_fd);
    Parser p = Parser_new(&l);
    AstNode* ast = Parser_parse(&p);

    _gen_header(out_fd);
    _gen_help_functions(out_fd);
    fprintf(out_fd, "_start:\n");
    _gen_from_ast(out_fd, ast);

    // Exit
    fprintf(out_fd, "\tmov rax, 60\n");
    fprintf(out_fd, "\txor rdi, rdi\n");
    fprintf(out_fd, "\tsyscall\n\n");
    fprintf(out_fd, "tape: rb 30000\n");

    Ast_free(ast);
    fclose(src_fd);
    fclose(out_fd);
}
