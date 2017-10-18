#include "shellmode.h"

#ifndef SHELL_INTERACTIVEMODE_H
#define SHELL_INTERACTIVEMODE_H

//#define PROMPT_COMMAND "PROMPT_COMMAND" TODO
#define SHELL_PROMPT_0 "PS0"
#define SHELL_PROMPT_1 "PS1"
#define HIST_SIZE "HISTCOUNT"
#define HIST_FILE_SIZE "HISTFILECOUNT"

#define SHELL_PROMPT_1_DEFAULT "command?: "
#define HIST_SIZE_DEFAULT "100"
#define HIST_FILE_SIZE_DEFAULT "1000"

ShellMode *ishmode_new();

void ishmode_onInit(void **data, Shell *shell, IOStreams *streams);

void ishmode_onPreParse(void **data, Shell *shell, IOStreams *streams);

void ishmode_onPostParse(void **data, Shell *shell, IOStreams *streams, Executable *executable);

void ishmode_dispose(void *data);

#endif //SHELL_INTERACTIVEMODE_H
