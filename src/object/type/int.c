#include <stdlib.h>
#include <stdio.h>
#include "int.h"
#include "../../util/util.h"

#define MAX_INT_CHARS 11

char *int_toString(void *o) {
    char *str = calloc(MAX_INT_CHARS + 1, sizeof(char));
    sprintf(str, "%d", *((int *) o));
    return str;
}

unsigned int int_hashCode(void *o) {
    return (unsigned int) *((int *) o);
}

int int_compareTo(void *o1, void *o2) {
    return *(int *) o1 - *(int *) o2;
}

void *int_clone(void *o) {
    return newInt(*(int *) o);
}

void int_dispose(void *o) {
    if (o == NULL) {
        return;
    }
    free(o);
}
