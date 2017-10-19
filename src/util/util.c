#include <string.h>
#include <stdlib.h>
#include "util.h"

char *newString(char *in) {
    if (in == NULL) {
        return NULL;
    }
    char *out = calloc(strlen(in) + 1, sizeof(char));
    strcpy(out, in);
    return out;
}

int *newInt(int in) {
    int *out = malloc(sizeof(int));
    *out = in;
    return out;
}
