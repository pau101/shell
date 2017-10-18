#include <stdlib.h>
#include <stdio.h>

#include "shell/shell.h"

int main(int argc, char **argv) {
    Shell *shell = shell_new();
    IOStreams *streams = iostreams_new(stdin, stdout, stderr);
    shell_main(shell, streams, argc, argv);
    shell_dispose(shell);
    iostreams_dispose(streams);
    return EXIT_SUCCESS;
}
