#ifndef SHELL_EXECUTABLE_H
#define SHELL_EXECUTABLE_H

#include "../../object/object.h"

typedef struct executable {
    Object *executor;
    int (*execute)(Object *data);
} Executable;

Executable *executable_new(Object *executor, int (*execute)(Object *e));

int executable_execute(Executable *executable);

char *executable_toString(void *o);

unsigned int executable_hashCode(void *o);

int executable_compareTo(void *o1, void *o2);

void *executable_clone(void *o);

void executable_dispose(void *o);

static const DataType TYPE_EXECUTABLE = {"EXECUTABLE", executable_toString, executable_hashCode, executable_compareTo,
                                         executable_clone, executable_dispose};

#endif //SHELL_EXECUTABLE_H
