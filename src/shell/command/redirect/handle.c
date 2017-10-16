#include <stdlib.h>
#include "handle.h"

Handle *handle_new(void *data, void (*dispose)(void *d), FILE *(*open)(void *d)) {
    Handle *handle = calloc(1, sizeof(Handle));
    handle->data = data;
    handle->dispose = dispose;
    handle->open = open;
    return handle;
}

FILE *handle_open(Handle *handle) {
    return handle->open(handle->data);
}

void handle_dispose(Handle *handle) {
    if (handle == NULL) {
        return;
    }
    handle->dispose(handle->data);
    free(handle);
}
