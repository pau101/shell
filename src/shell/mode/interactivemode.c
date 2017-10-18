#include <sys/fcntl.h>
#include "interactivemode.h"
#include "../../util/preconditions.h"
#include "noninteractivemode.h"

#define RUN_COMMANDS_FILE "/home/Paul/.msshrc"

typedef struct interactiveData {
    Hashtable *history;
} InteractiveShellData;

InteractiveShellData *ishdata_new() {
    InteractiveShellData *ishdata = calloc(1, sizeof(InteractiveShellData));
    ishdata->history = hashtable_new(HT_INITIAL_CAPACITY, HT_LOAD_FACTOR);
    return ishdata;
}

void ishdata_dispose(InteractiveShellData *ishdata) {
    if (ishdata == NULL) {
        return;
    }
    hashtable_dispose(ishdata->history);
    free(ishdata);
}

ShellMode *ishmode_new() {
    return shmode_new(
            ishdata_new(),
            ishmode_dispose,
            ishmode_onInit,
            ishmode_onPreParse,
            ishmode_onPostParse
    );
}

void ishmode_onInit(void **data, Shell *shell, IOStreams *streams) {
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
}

void ishmode_printVariable(Shell *shell, FILE *output, char *key) {
    char *value = shell_getVariable(shell, key);
    fprintf(output, value);
    fflush(output);
    free(value);
}

void ishmode_onPreParse(void **data, Shell *shell, IOStreams *streams) {
    requireNonNull(shell);
    requireNonNull(streams);
    ishmode_printVariable(shell, streams->output, SHELL_PROMPT_1);
}

void ishmode_onPostParse(void **data, Shell *shell, IOStreams *streams, Executable *executable) {
    requireNonNull(shell);
    requireNonNull(streams);
    if (executable != NULL) {
        executable_dispose(executable_clone(executable));
    }
    ishmode_printVariable(shell, streams->output, SHELL_PROMPT_0);
}

void ishmode_dispose(void *data) {
    ishdata_dispose(data);
}
