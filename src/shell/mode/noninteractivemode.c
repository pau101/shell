#include "noninteractivemode.h"
#include "../../util/preconditions.h"


ShellMode *nishmode_new() {
    return shmode_new(
            NULL,
            nishmode_dispose,
            nishmode_onInit,
            nishmode_onPreParse,
            nishmode_onPostParse
    );
}

void nishmode_onInit(void **data, Shell *shell, IOStreams *streams) {
    requireNonNull(shell);
    requireNonNull(streams);
}


void nishmode_onPreParse(void **data, Shell *shell, IOStreams *streams) {
    requireNonNull(shell);
    requireNonNull(streams);
}

void nishmode_onPostParse(void **data, Shell *shell, IOStreams *streams, Executable *executable) {
    requireNonNull(shell);
    requireNonNull(streams);
}

void nishmode_dispose(void *data) {
    if (data == NULL) {
        return;
    }
    //
    free(data);
}
