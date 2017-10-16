#ifndef SHELL_SHELL_H
#define SHELL_SHELL_H

#include <stdio.h>
#include "parser/parser.h"
#include "../collections/hashtable/hashtable.h"

#define SHELL_PROMPT "command?: "

typedef struct shell {
    Parser *parser;
    int historyCount;
    int historyFileCount;
    Hashtable *aliases;
} Shell;

Shell *shell_new();

void shell_main(Shell *shell, FILE *input, FILE *output, FILE *error);

void shell_dispose(Shell *shell);

#endif //SHELL_SHELL_H
