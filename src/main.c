#include <stdlib.h>
#include <stdio.h>

#include "shell/shell.h"

int main() {
    Shell *shell = shell_new();
    shell_main(shell, stdin, stdout, stderr);
    shell_dispose(shell);
    return EXIT_SUCCESS;
}
