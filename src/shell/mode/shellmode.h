#ifndef SHELL_SHELLMODE_H
#define SHELL_SHELLMODE_H

#include "../shell.h"
#include "../../util/iostreams.h"

typedef struct shell Shell;

typedef struct shellMode {
    void *data;

    void (*dispose)(void *data);

    void (*onInit)(void **data, Shell *shell, IOStreams *streams);

    void (*onPreParse)(void **data, Shell *shell, IOStreams *streams);

    void (*onPostParse)(void **data, Shell *shell, IOStreams *streams, Executable *executable);
} ShellMode;

ShellMode *shmode_new(void *data, void (*dispose)(void *d), void (*onInit)(void **d, Shell *shell, IOStreams *streams),
                      void (*onPreParse)(void **d, Shell *shell, IOStreams *streams),
                      void (*onPostParse)(void **d, Shell *shell, IOStreams *streams, Executable *executable));

void onInit(ShellMode *mode, Shell *shell, IOStreams *streams);

void shmode_onPreParse(ShellMode *mode, Shell *shell, IOStreams *streams);

void shmode_onPostParse(ShellMode *mode, Shell *shell, IOStreams *streams, Executable *executable);

void shmode_dispose(ShellMode *mode);

#endif //SHELL_SHELLMODE_H
