#include <string.h>
#include <stdlib.h>

char *string_toString(void *o) {
    return o;
}

unsigned int string_hashCode(void *o) {
    char *str = o;
    unsigned int hash = 1;
    char c;
    while ((c = *(str++)) != '\0') {
        hash = hash * 31 + c;
    }
    return hash;
}

int string_compareTo(void *o1, void *o2) {
    return strcmp(o1, o2);
}

void string_dispose(void *o) {
    if (o == NULL) {
        return;
    }
    free(o);
}
