#ifndef SHELL_IOSTREAMS_H
#define SHELL_IOSTREAMS_H

#include <stdio.h>

typedef struct ioStreams {
    FILE *input;
    FILE *output;
    FILE *error;
} IOStreams;

IOStreams *iostreams_new(FILE *input, FILE *output, FILE *error);

void iostreams_dispose(IOStreams *streams);

#endif //SHELL_IOSTREAMS_H
