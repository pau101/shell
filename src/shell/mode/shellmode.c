#include "shellmode.h"
#include "../../util/preconditions.h"

void shmode_init(const ShellMode *mode, Shell *shell, IOStreams *streams) {
    requireNonNull(mode);
    requireNonNull(shell);
    requireNonNull(streams);
    mode->init(shell, streams);
}

void shmode_promptPrimary(const ShellMode *mode, Shell *shell, IOStreams *streams) {
    requireNonNull(mode);
    requireNonNull(shell);
    requireNonNull(streams);
    mode->promptPrimary(shell, streams);
}

void shmode_promptSecondary(const ShellMode *mode, Shell *shell, IOStreams *streams) {
    requireNonNull(mode);
    requireNonNull(shell);
    requireNonNull(streams);
    mode->promptSecondary(shell, streams);
}

