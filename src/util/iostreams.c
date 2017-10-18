#include <stdlib.h>
#include "iostreams.h"

IOStreams *iostreams_new(FILE *input, FILE *output, FILE *error) {
    IOStreams *streams = calloc(1, sizeof(IOStreams));
    streams->input = input;
    streams->output = output;
    streams->error = error;
    return streams;
}

void iostreams_dispose(IOStreams *streams) {
    free(streams);
}
