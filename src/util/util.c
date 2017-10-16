#include <string.h>
#include <stdlib.h>
#include "util.h"

char *cloneString(char *in) {
    char *out = calloc(strlen(in) + 1, sizeof(char));
    strcpy(out, in);
    return out;
}

int *cloneInt(int in) {
    int *out = malloc(sizeof(int));
    *out = in;
    return out;
}
