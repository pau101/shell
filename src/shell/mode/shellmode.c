#include "shellmode.h"
#include "../../util/preconditions.h"

ShellMode *shmode_new(void *data, void (*dispose)(void *d), void (*onInit)(void **d, Shell *shell, IOStreams *streams),
                      void (*onPreParse)(void **d, Shell *shell, IOStreams *streams),
                      void (*onPostParse)(void **d, Shell *shell, IOStreams *streams, Executable *executable)) {
    ShellMode *mode = calloc(1, sizeof(ShellMode));
    mode->data = data;
    mode->dispose = dispose;
    mode->onInit = onInit;
    mode->onPreParse = onPreParse;
    mode->onPostParse = onPostParse;
    return mode;
}

void onInit(ShellMode *mode, Shell *shell, IOStreams *streams) {
    requireNonNull(mode);
    requireNonNull(shell);
    requireNonNull(streams);
    mode->onInit(&mode->data, shell, streams);
}

void shmode_onPreParse(ShellMode *mode, Shell *shell, IOStreams *streams) {
    requireNonNull(mode);
    requireNonNull(shell);
    requireNonNull(streams);
    mode->onPreParse(&mode->data, shell, streams);
}

void shmode_onPostParse(ShellMode *mode, Shell *shell, IOStreams *streams, Executable *executable) {
    requireNonNull(mode);
    requireNonNull(shell);
    requireNonNull(streams);
    mode->onPostParse(&mode->data, shell, streams, executable);
}

void shmode_dispose(ShellMode *mode) {
    if (mode == NULL) {
        return;
    }
    mode->dispose(mode->data);
    free(mode);
}
