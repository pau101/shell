#include <stdlib.h>
#include <stdio.h>

#define MAX_PTR_CHARS 20

char *reference_toString(void *o) {
    char *str = calloc(MAX_PTR_CHARS + 1, sizeof(char));
    sprintf(str, "%p", o);
    return str;
}

unsigned int reference_hashCode(void *o) {
    return (unsigned int) (uintptr_t) o;
}

int reference_compareTo(void *o1, void *o2) {
    return o1 < o2 ? -1 : o1 == o2 ? 0 : 1;
}

void reference_dispose(void *reference) {
    if (reference == NULL) {
        return;
    }
    free(reference);
}
