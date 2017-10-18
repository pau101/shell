#ifndef SHELL_OBJECT_H
#define SHELL_OBJECT_H

#include "datatype.h"

typedef struct object {
    const DataType *dataType;
    void *data;
} Object;

Object *object_new(const DataType *dataType, void *data);

void *object_get(Object *object, const DataType *dataType);

void *object_getAndDispose(Object *object, const DataType *dataType);

char *object_toString(Object *object);

unsigned int object_hashCode(Object *object);

int object_compareTo(Object *object1, Object *object2);

void object_dispose(Object *object);

#endif //SHELL_OBJECT_H
