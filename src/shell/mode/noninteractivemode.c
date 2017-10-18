#include "noninteractivemode.h"
#include "../../util/preconditions.h"

void nishmode_init(Shell *shell, IOStreams *streams) {
    requireNonNull(shell);
    requireNonNull(streams);
}


void nishmode_promptPrimary(Shell *shell, IOStreams *streams) {
    requireNonNull(shell);
    requireNonNull(streams);
}

void nishmode_promptSecondary(Shell *shell, IOStreams *streams) {
    requireNonNull(shell);
    requireNonNull(streams);
}
