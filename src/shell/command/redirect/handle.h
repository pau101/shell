#ifndef SHELL_HANDLE_H
#define SHELL_HANDLE_H

#include <stdio.h>
#include "../../../object/datatype.h"
#include "../../../object/object.h"

typedef struct handle {
    Object *data;

    FILE *(*open)(void *data);
} Handle;

Handle *handle_new(Object *data, FILE *(*open)(void *d));

FILE *handle_open(Handle *handle);

char *handle_toString(void *o);

unsigned int handle_hashCode(void *o);

int handle_compareTo(void *o1, void *o2);

void *handle_clone(void *o);

void handle_dispose(void *o);

static const DataType TYPE_HANDLE = {"HANDLE", handle_toString, handle_hashCode, handle_compareTo, handle_clone, handle_dispose};

#endif //SHELL_HANDLE_H
