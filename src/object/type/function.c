#include "function.h"
#include "reference.h"

char *function_toString(void *o) {
    return reference_toString(o);
}

unsigned int function_hashCode(void *o) {
    return reference_hashCode(o);
}

int function_compareTo(void *o1, void *o2) {
    return reference_compareTo(o1, o2);
}

void *function_clone(void *o) {
    return o;
}

void function_dispose(void *o) {}