#ifndef SHELL_DATATYPE_H
#define SHELL_DATATYPE_H

#include <stdbool.h>

typedef struct dataType {
    const char *identifier;

    char *(*toString)(void *o);

    unsigned int (*hashCode)(void *o);

    int (*compareTo)(void *o1, void *o2);

    void (*dispose)(void *o);
} DataType;

bool datatype_equals(const DataType *dataType1, const DataType *dataType2);

#endif //SHELL_DATATYPE_H
