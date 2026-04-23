#include <stdio.h>
#include <string.h>

#include "darr.h"
#include "utils.h"
#include "lexer.h"
#include "parser.h"
#include "codegen.h"

typedef struct String {
    char* items;
    size_t cap;
    size_t len;
} String;

int main(int argc, char** argv)
{
    if (argc < 2) {
        fprintf(stderr, "Usage: %s [file]\n", argv[0]);
        return -1;
    }

    // Create output filename (example.bf -> example.asm)
    String out_filename = {0};
    // Remove extension
    for (size_t i = 0; argv[1][i] != '.'; ++i) {
        darr_append(&out_filename, argv[1][i]);
    }
    // Add new file extension
    const char* extension = ".asm";
    darr_append_many(&out_filename, extension, strlen(extension)+1);

    // Generate assembly code
    codegen(argv[1], out_filename.items);

    darr_free(&out_filename);

    return 0;
}
