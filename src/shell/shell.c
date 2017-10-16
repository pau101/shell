#include <stdlib.h>
#include <wait.h>
#include "shell.h"
#include "../util/preconditions.h"

#define HISTORY_COUNT 100
#define HISTORY_FILE_COUNT 1000

Shell *shell_new() {
    Shell *shell = calloc(1, sizeof(Shell));
    shell->parser = parser_new();
    shell->historyCount = HISTORY_COUNT;
    shell->historyFileCount = HISTORY_FILE_COUNT;
    shell->aliases = hashtable_new(HT_INITIAL_CAPACITY, HT_LOAD_FACTOR);
    return shell;
}

void shell_main(Shell *shell, FILE *input, FILE *output, FILE *error) {
    requireNonNull(shell);
    requireNonNull(input);
    requireNonNull(output);
    do {
        fprintf(output, SHELL_PROMPT);
        fflush(output);
        Sequence *sequence = parser_parse(shell->parser, input, error);
        if (sequence != NULL) {
            sequence_exec(sequence);
            sequence_dispose(sequence);
        }
    } while (parser_isAtLine(shell->parser));
}

void shell_dispose(Shell *shell) {
    if (shell == NULL) {
        return;
    }
    parser_dispose(shell->parser);
    hashtable_dispose(shell->aliases);
    free(shell);
}
