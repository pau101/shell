#ifndef SHELL_SHELLMODE_H
#define SHELL_SHELLMODE_H

#include "../shell.h"
#include "../../util/iostreams.h"

typedef struct shell Shell;

typedef struct shellMode {
    void (*init)(Shell *shell, IOStreams *streams);
    void (*promptPrimary)(Shell *shell, IOStreams *streams);
    void (*promptSecondary)(Shell *shell, IOStreams *streams);
} ShellMode;

void shmode_init(const ShellMode *mode, Shell *shell, IOStreams *streams);

void shmode_promptPrimary(const ShellMode *mode, Shell *shell, IOStreams *streams);

void shmode_promptSecondary(const ShellMode *mode, Shell *shell, IOStreams *streams);

#endif //SHELL_SHELLMODE_H
