#include <stdlib.h>
#include <wait.h>
#include <string.h>
#include <limits.h>
#include <errno.h>
#include <unistd.h>
#include "shell.h"
#include "../util/preconditions.h"
#include "../object/type/string.h"
#include "mode/interactivemode.h"
#include "../object/type/function.h"

Shell *shell_new() {
    Shell *shell = calloc(1, sizeof(Shell));
    shell->parser = parser_new();
    shell->aliases = hashtable_new(HT_INITIAL_CAPACITY, HT_LOAD_FACTOR);
    shell->variables = hashtable_new(HT_INITIAL_CAPACITY, HT_LOAD_FACTOR);
    shell->builtins = hashtable_new(HT_INITIAL_CAPACITY, HT_LOAD_FACTOR);
    shell->history = list_new();
    return shell;
}

int shell_cd(Shell *shell, IOStreams *streams, LinkedList *args) {
    int status = EXIT_SUCCESS;
    if (args->size > 1) {
        char *path = calloc(PATH_MAX, sizeof(char));
        ListIterator *itr = list_listIterator(args, 1);
        int pos = 0;
        bool doLoop = true;
        while (doLoop) {
            char *part = object_get(listiterator_next(itr), &TYPE_STRING);
            for (int i = 0; part[i] != '\0'; i++) {
                if (pos >= PATH_MAX) {
                    doLoop = false;
                    continue;
                }
                path[pos++] = part[i];
            }
            if (listiterator_hasNext(itr)) {
                path[pos++] = ' ';
            } else {
                doLoop = false;
            }
        }
        listiterator_dispose(itr);
        if (chdir(path) != 0) {
            fprintf(streams->error, "%s\n", strerror(errno));
            status = EXIT_FAILURE;
        }
        free(path);
    } else {
        char *path = getenv("HOME");
        if (path != NULL && chdir(path) != 0) {
            fprintf(streams->error, "%s\n", strerror(errno));
            status = EXIT_FAILURE;
        }
    }
    return status;
}

int shell_alias(Shell *shell, IOStreams *streams, LinkedList *args) {
    if (list_isSingular(args)) {
        Iterator *itr = hashtable_iterator(shell->aliases);
        while (iterator_hasNext(itr)) {
            HTEntry *entry = iterator_next(itr);
            char *key = object_get(entry->key, &TYPE_STRING);
            char *value = object_get(entry->value, &TYPE_STRING);
            fprintf(streams->output, "alias %s='%s'\n", key, value);
        }
        iterator_dispose(itr);
    } else {
        ListIterator *itr = list_listIterator(args, 1);
        while (listiterator_hasNext(itr)) {
            char *str = object_get(listiterator_next(itr), &TYPE_STRING);
            size_t split = strcspn(str, "=");
            if (split == strlen(str)) {
                char *a = shell_getAlias(shell, str);
                if (a == NULL) {
                    fprintf(streams->error, "%s: not found\n", str);
                } else {
                    fprintf(streams->output, "alias %s='%s'\n", str, a);
                }
            } else {
                char *key = calloc(split + 1, sizeof(char));
                char *value = calloc(strlen(str) - split - 1, sizeof(char));
                strncpy(key, str, split);
                strncpy(value, str + split + 1, strlen(str) - split - 1);
                shell_setAlias(shell, key, value);
                free(key);
                free(value);
            }
        }
        listiterator_dispose(itr);
    }
    return EXIT_SUCCESS;
}

int shell_unalias(Shell *shell, IOStreams *streams, LinkedList *args) {
    if (list_isSingular(args)) {
        fprintf(streams->error, "usage: unalias name [name ...]");
        return EXIT_FAILURE;
    }
    ListIterator *itr = list_listIterator(args, 1);
    while (listiterator_hasNext(itr)) {
        shell_setAlias(shell, object_get(listiterator_next(itr), &TYPE_STRING), "");
    }
    listiterator_dispose(itr);
    return EXIT_SUCCESS;
}

int shell_exit(Shell *shell, IOStreams *streams, LinkedList *args) {
    int status = EXIT_SUCCESS;
    if (args->size == 2) {
        char *endPtr;
        char *str = object_get(list_get(args, 1), &TYPE_STRING);
        long result = strtol(str, &endPtr, 10);
        if (result >= INT_MIN && result <= INT_MAX && !*endPtr) {
            status = (int) result;
        } else {
            fprintf(streams->error, "exit: %s: numeric argument required\n", str);
        }
    } else if (args->size > 2) {
        fprintf(streams->error, "exit: too many arguments\n");
    }
    shell->isRunning = false;
    return status;
}

int shell_history(Shell *shell, IOStreams *streams, LinkedList *args) {
    int amount;
    if (list_isSingular(args)) {
        amount = shell->history->size;
    } else if (args->size == 2) {
        char *endPtr;
        char *str = object_get(list_get(args, 1), &TYPE_STRING);
        long result = strtol(str, &endPtr, 10);
        if (result >= INT_MIN && result <= INT_MAX && !*endPtr) {
            amount = (int) result;
        } else {
            fprintf(streams->error, "history: %s: numeric argument required\n", str);
            return EXIT_FAILURE;
        }
    } else {
        fprintf(streams->error, "history: too many arguments\n");
        return EXIT_FAILURE;
    }
    int start = shell->history->size - amount;
    ListIterator *itr = list_listIterator(shell->history, start < 0 ? 0 : start);
    int count = 0;
    while (listiterator_hasNext(itr)) {
        Executable *exec = object_get(listiterator_next(itr), &TYPE_EXECUTABLE);
        fprintf(streams->output, " %4d  %s\n", start + count++, exec->source);
    }
    listiterator_dispose(itr);
    return EXIT_SUCCESS;
}

void shell_init(Shell *shell, IOStreams *streams, ShellMode *mode) {
    requireNonNull(shell);
    requireNonNull(streams);
    shell_setBuiltin(shell, "cd", shell_cd);
    shell_setBuiltin(shell, "alias", shell_alias);
    shell_setBuiltin(shell, "unalias", shell_unalias);
    shell_setBuiltin(shell, "exit", shell_exit);
    shell_setBuiltin(shell, "history", shell_history);
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

void shell_setBuiltin(Shell *shell, char *name, Builtin builtin) {
    Object *k = object_new(&TYPE_STRING, newString(name));
    Object *v = hashtable_put(shell->builtins, k, object_new(&TYPE_FUNCTION, builtin));
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

void shell_setAlias(Shell *shell, char *key, char *value) {
    Object *k = object_new(&TYPE_STRING, newString(key));
    if (value == NULL || strlen(value) == 0) {
        object_dispose(hashtable_remove(shell->aliases, k));
        object_dispose(k);
    } else {
        Object *v = hashtable_put(shell->aliases, k, object_new(&TYPE_STRING, newString(value)));
        if (v != NULL) {
            object_dispose(k);
            object_dispose(v);
        }
    }
}

char *shell_getAlias(Shell *shell, char *key) {
    Object *k = object_new(&TYPE_STRING, newString(key));
    Object *v = hashtable_get(shell->aliases, k);
    object_dispose(k);
    return v == NULL ? NULL : object_get(v, &TYPE_STRING);
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
    shell->isRunning = true;
    do {
        shmode_onPreParse(mode, shell, streams);
        Executable *executable = parser_parse(shell->parser, streams->input, streams->output);
        shmode_onPostParse(mode, shell, streams, executable);
        if (executable != NULL) {
            status = executable_execute(executable, shell, streams);
            executable_dispose(executable);
        }
    } while (shell->isRunning && parser_isAtLine(shell->parser));
    return status;
}

void shell_dispose(Shell *shell) {
    if (shell == NULL) {
        return;
    }
    parser_dispose(shell->parser);
    hashtable_dispose(shell->aliases);
    hashtable_dispose(shell->variables);
    hashtable_dispose(shell->builtins);
    list_dispose(shell->history);
    free(shell);
}
