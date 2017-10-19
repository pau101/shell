#include <sys/fcntl.h>
#include "interactivemode.h"
#include "../../util/preconditions.h"
#include "noninteractivemode.h"

#define RUN_COMMANDS_FILE ".msshrc"
#define HISTORY_FILE ".msshrc_history"

typedef struct interactiveData {
    LinkedList *history;
} InteractiveShellData;

InteractiveShellData *ishdata_new() {
    InteractiveShellData *ishdata = calloc(1, sizeof(InteractiveShellData));
    ishdata->history = list_new();
    return ishdata;
}

void ishdata_dispose(InteractiveShellData *ishdata) {
    if (ishdata == NULL) {
        return;
    }
    list_dispose(ishdata->history);
    free(ishdata);
}

ShellMode *ishmode_new() {
    return shmode_new(
            ishdata_new(),
            ishmode_dispose,
            ishmode_onInit,
            ishmode_onPreParse,
            ishmode_onPostParse,
            ishmode_onExit
    );
}

InteractiveShellData *ishmode_data(void **data) {
    return *data;
}

void ishmode_onInit(void **data, Shell *shell, IOStreams *streams) {
    requireNonNull(data);
    requireNonNull(shell);
    requireNonNull(streams);
    shell_setVariable(shell, SHELL_PROMPT_1, SHELL_PROMPT_1_DEFAULT);
    shell_setVariable(shell, HIST_SIZE, HIST_SIZE_DEFAULT);
    shell_setVariable(shell, HIST_FILE_SIZE, HIST_FILE_SIZE_DEFAULT);
    FILE *rc = fopen(RUN_COMMANDS_FILE, "a+");
    if (rc != NULL) {
        fseek(rc, 0, SEEK_SET);
        IOStreams *rcStreams = iostreams_new(rc, streams->output, streams->error);
        ShellMode *mode = nishmode_new();
        shell_execute(shell, rcStreams, mode);
        iostreams_dispose(rcStreams);
        shmode_dispose(mode);
        if (fclose(rc) == EOF) {
            pExit("fclose");
        }
    }
    FILE *hist = fopen(HISTORY_FILE, "r");
    if (hist != NULL) {
        LinkedList *history = ishmode_data(data)->history;
        parser_reset(shell->parser);
        do {
            Executable *executable = parser_parse(shell->parser, hist, streams->output);
            if (executable != NULL) {
                list_addLast(history, object_new(&TYPE_EXECUTABLE, executable));
            }
        } while (parser_isAtLine(shell->parser));
        if (fclose(hist) == EOF) {
            pExit("fclose");
        }
    }
}

void ishmode_printVariable(Shell *shell, FILE *output, char *key) {
    char *value = shell_getVariable(shell, key);
    fprintf(output, value);
    fflush(output);
    free(value);
}

void ishmode_onPreParse(void **data, Shell *shell, IOStreams *streams) {
    requireNonNull(data);
    requireNonNull(shell);
    requireNonNull(streams);
    ishmode_printVariable(shell, streams->output, SHELL_PROMPT_1);
}

void ishmode_onPostParse(void **data, Shell *shell, IOStreams *streams, Executable *executable) {
    requireNonNull(data);
    requireNonNull(shell);
    requireNonNull(streams);
    if (executable != NULL) {
        int histSize = shell_getIntVariable(shell, HIST_SIZE);
        LinkedList *history = ishmode_data(data)->history;
        if (histSize != 0) {
            list_addLast(history, object_new(&TYPE_EXECUTABLE, executable_clone(executable)));
        }
        if (histSize > 0) {
            while (history->size > histSize) {
                object_dispose(list_removeFirst(history));
            }
        }
    }
    ishmode_printVariable(shell, streams->output, SHELL_PROMPT_0);
}

void ishmode_onExit(void **data, Shell *shell, IOStreams *streams) {
    requireNonNull(data);
    requireNonNull(shell);
    requireNonNull(streams);
    FILE *hist = fopen(HISTORY_FILE, "w");
    if (hist != NULL) {
        Iterator *itr = list_iterator(ishmode_data(data)->history);
        while (iterator_hasNext(itr)) {
            Executable *exec = object_get(iterator_next(itr), &TYPE_EXECUTABLE);
            fprintf(hist, "%s\n", exec->source);
        }
        iterator_dispose(itr);
    }
}

void ishmode_dispose(void *data) {
    ishdata_dispose(data);
}
