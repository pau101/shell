#ifndef SHELL_SHELL_H
#define SHELL_SHELL_H

#include <stdio.h>
#include "parser/parser.h"
#include "../collections/hashtable/hashtable.h"
#include "mode/shellmode.h"
#include "../util/iostreams.h"

typedef struct shellMode ShellMode;

typedef struct parser Parser;

typedef int (*Builtin)(Shell *, Object *);

typedef struct shell {
    Parser *parser;
    Hashtable *aliases;
    Hashtable *variables;
    Hashtable *builtins;
} Shell;

Shell *shell_new();

void shell_init(Shell *shell, IOStreams *streams, ShellMode *mode);

void shell_setVariable(Shell *shell, char *key, char *value);

char *shell_getVariable(Shell *shell, char *key);

int shell_getIntVariable(Shell *shell, char *key);

void shell_setBuiltin(Shell *shell, char *name, int (*execute)(Shell *s, Object *e));

Builtin shell_getBuiltin(Shell *shell, char *name);

void shell_main(Shell *shell, IOStreams *streams, int argc, char **argv);

int shell_execute(Shell *shell, IOStreams *streams, ShellMode *mode);

void shell_dispose(Shell *shell);

#endif //SHELL_SHELL_H
