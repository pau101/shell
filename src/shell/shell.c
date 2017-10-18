#include <stdlib.h>
#include <wait.h>
#include <string.h>
#include "shell.h"
#include "../util/preconditions.h"
#include "../object/type/string.h"
#include "mode/interactivemode.h"

Shell *shell_new() {
    Shell *shell = calloc(1, sizeof(Shell));
    shell->parser = parser_new();
    shell->aliases = hashtable_new(HT_INITIAL_CAPACITY, HT_LOAD_FACTOR);
    shell->variables = hashtable_new(HT_INITIAL_CAPACITY, HT_LOAD_FACTOR);
    return shell;
}

void shell_init(Shell *shell, IOStreams *streams, const ShellMode *mode) {
    requireNonNull(shell);
    requireNonNull(streams);
    shmode_init(mode, shell, streams);
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

void shell_main(Shell *shell, IOStreams *streams, int argc, char **argv) {
    requireNonNull(shell);
    requireNonNull(streams);
    // TODO --norc --rcfile -c
    const ShellMode *mode = &INTERACTIVE_MODE;
    shell_init(shell, streams, mode);
    shell_execute(shell, streams, mode);
}

int shell_execute(Shell *shell, IOStreams *streams, const ShellMode *mode) {
    requireNonNull(shell);
    requireNonNull(streams);
    int status = EXIT_SUCCESS;
    parser_reset(shell->parser);
    do {
        shmode_promptPrimary(mode, shell, streams);
        Executable *executable = parser_parse(shell->parser, streams->input, streams->output);
        shmode_promptSecondary(mode, shell, streams);
        if (executable != NULL) {
            status = executable_execute(executable);
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
