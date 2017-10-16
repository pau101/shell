#ifndef SHELL_HANDLE_H
#define SHELL_HANDLE_H

#include <stdio.h>

typedef struct handle {
    void *data;

    void (*dispose)(void *data);

    FILE *(*open)(void *data);
} Handle;

Handle *handle_new(void *data, void (*dispose)(void *d), FILE *(*open)(void *d));

FILE *handle_open(Handle *handle);

void handle_dispose(Handle *handle);

#endif //SHELL_HANDLE_H
