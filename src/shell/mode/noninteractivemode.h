#ifndef SHELL_NONINTERACTIVEMODE_H
#define SHELL_NONINTERACTIVEMODE_H

#include "shellmode.h"

ShellMode *nishmode_new();

void nishmode_onInit(void **data, Shell *shell, IOStreams *streams);

void nishmode_onPreParse(void **data, Shell *shell, IOStreams *streams);

void nishmode_onPostParse(void **data, Shell *shell, IOStreams *streams, Executable *executable);

void nishmode_onExit(void **data, Shell *shell, IOStreams *streams);

void nishmode_dispose(void *data);

#endif //SHELL_NONINTERACTIVEMODE_H
