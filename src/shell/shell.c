#include <stdlib.h>
#include <wait.h>
#include <string.h>
#include <limits.h>
#include <errno.h>
#include "shell.h"
#include "../util/preconditions.h"
#include "../object/type/string.h"
#include "mode/interactivemode.h"
#include "../object/type/reference.h"
#include "../object/type/function.h"

Shell *shell_new() {
    Shell *shell = calloc(1, sizeof(Shell));
    shell->parser = parser_new();
    shell->aliases = hashtable_new(HT_INITIAL_CAPACITY, HT_LOAD_FACTOR);
    shell->variables = hashtable_new(HT_INITIAL_CAPACITY, HT_LOAD_FACTOR);
    shell->builtins = hashtable_new(HT_INITIAL_CAPACITY, HT_LOAD_FACTOR);
    return shell;
}

void shell_init(Shell *shell, IOStreams *streams, ShellMode *mode) {
    requireNonNull(shell);
    requireNonNull(streams);
    shmode_onInit(mode, shell, streams);
}

void shell_setVariable(Shell *shell, char *key, char *value) {
    requireNonNull(shell);
    requireNonNull(key);
    Object *k = object_new(&TYPE_STRING, newString(key));
    if (value == NULL || strlen(value) == 0) {
        object_dispose(hashtable_remove(shell->variables, k));
        object_dispose(k);
    } else {
        Object *v = hashtable_put(shell->variables, k, object_new(&TYPE_STRING, newString(value)));
        if (v != NULL) {
            object_dispose(k);
            object_dispose(v);
        }
    }
}

char *shell_getVariable(Shell *shell, char *key) {
    requireNonNull(shell);
    requireNonNull(key);
    Object *k = object_new(&TYPE_STRING, newString(key));
    Object *v = hashtable_get(shell->variables, k);
    object_dispose(k);
    return newString(v == NULL ? "" : object_get(v, &TYPE_STRING));
}

int shell_getIntVariable(Shell *shell, char *key) {
    char *str = shell_getVariable(shell, key);
    char *endPtr;
    long result = strtol(str, &endPtr, 10);
    int value;
    if (result >= INT_MIN && result <= INT_MAX && !*endPtr) {
        value = (int) result;
    } else {
        value = 0;
    }
    free(str);
    return value;
}

void shell_setBuiltin(Shell *shell, char *name, int (*command)(Shell *s, Object *e)) {
    Object *k = object_new(&TYPE_STRING, newString(name));
    Object *v = hashtable_put(shell->builtins, k, object_new(&TYPE_FUNCTION, command));
    if (v != NULL) {
        object_dispose(k);
        object_dispose(v);
    }
}

Builtin shell_getBuiltin(Shell *shell, char *name) {
    Object *k = object_new(&TYPE_STRING, newString(name));
    Object *v = hashtable_get(shell->builtins, k);
    object_dispose(k);
    return v == NULL ? NULL : object_get(v, &TYPE_FUNCTION);
}

void shell_main(Shell *shell, IOStreams *streams, int argc, char **argv) {
    requireNonNull(shell);
    requireNonNull(streams);
    // TODO --norc --rcfile -c
    ShellMode *mode = ishmode_new();
    shell_init(shell, streams, mode);
    shell_execute(shell, streams, mode);
    shmode_onExit(mode, shell, streams);
    shmode_dispose(mode);
}

int shell_execute(Shell *shell, IOStreams *streams, ShellMode *mode) {
    requireNonNull(shell);
    requireNonNull(streams);
    int status = EXIT_SUCCESS;
    parser_reset(shell->parser);
    do {
        shmode_onPreParse(mode, shell, streams);
        Executable *executable = parser_parse(shell->parser, streams->input, streams->output);
        shmode_onPostParse(mode, shell, streams, executable);
        if (executable != NULL) {
            status = executable_execute(shell, executable);
            executable_dispose(executable);
        }
    } while (parser_isAtLine(shell->parser));
    return status;
}

void shell_dispose(Shell *shell) {
    if (shell == NULL) {
        return;
    }
    parser_dispose(shell->parser);
    hashtable_dispose(shell->aliases);
    free(shell);
}
