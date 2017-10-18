#include <sys/fcntl.h>
#include "interactivemode.h"
#include "../../util/preconditions.h"
#include "noninteractivemode.h"

#define RUN_COMMANDS_FILE "./.msshrc"

void ishmode_init(Shell *shell, IOStreams *streams) {
    requireNonNull(shell);
    requireNonNull(streams);
    shell_setVariable(shell, SHELL_PROMPT_1, SHELL_PROMPT_1_DEFAULT);
    shell_setVariable(shell, HIST_SIZE, HIST_SIZE_DEFAULT);
    shell_setVariable(shell, HIST_FILE_SIZE, HIST_FILE_SIZE_DEFAULT);
    FILE *rc = fopen(RUN_COMMANDS_FILE, "a+");
    if (rc != NULL) {
        IOStreams *rcStreams = iostreams_new(rc, streams->output, streams->error);
        shell_execute(shell, rcStreams, &NON_INTERACTIVE_MODE);
        iostreams_dispose(rcStreams);
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

void ishmode_promptPrimary(Shell *shell, IOStreams *streams) {
    requireNonNull(shell);
    requireNonNull(streams);
    ishmode_printVariable(shell, streams->output, SHELL_PROMPT_1);
}

void ishmode_promptSecondary(Shell *shell, IOStreams *streams) {
    requireNonNull(shell);
    requireNonNull(streams);
    ishmode_printVariable(shell, streams->output, SHELL_PROMPT_0);
}
