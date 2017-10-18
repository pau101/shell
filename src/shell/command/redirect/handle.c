#include <stdlib.h>
#include "handle.h"
#include "../../../object/type/reference.h"

Handle *handle_new(Object *data, FILE *(*open)(void *d)) {
    Handle *handle = calloc(1, sizeof(Handle));
    handle->data = data;
    handle->open = open;
    return handle;
}

FILE *handle_open(Handle *handle) {
    return handle->open(handle->data);
}

char *handle_toString(void *o) {
    return reference_toString(o);
}

unsigned int handle_hashCode(void *o) {
    return reference_hashCode(o);
}

int handle_compareTo(void *o1, void *o2) {
    return reference_compareTo(o1, o2);
}

void *handle_clone(void *o) {
    Handle *h = (Handle *) o;
    return handle_new(object_clone(h->data), h->open);
}

void handle_dispose(void *handle) {
    if (handle == NULL) {
        return;
    }
    object_dispose(((Handle *) handle)->data);
    free(handle);
}
