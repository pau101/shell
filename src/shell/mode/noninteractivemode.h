#ifndef SHELL_NONINTERACTIVEMODE_H
#define SHELL_NONINTERACTIVEMODE_H

#include "shellmode.h"

void nishmode_init(Shell *shell, IOStreams *streams);

void nishmode_promptPrimary(Shell *shell, IOStreams *streams);

void nishmode_promptSecondary(Shell *shell, IOStreams *streams);

static const ShellMode NON_INTERACTIVE_MODE = {
        nishmode_init,
        nishmode_promptPrimary,
        nishmode_promptSecondary
};

#endif //SHELL_NONINTERACTIVEMODE_H
