#ifndef SHELL_REDIRECT_H
#define SHELL_REDIRECT_H

#include <stdio.h>
#include "handle.h"
#include "opener/filedescriptoropener.h"
#include "../../../object/datatype.h"

typedef struct redirect {
    int source;
    Handle *destination;
} Redirect;

Redirect *redirect_new(int source, Handle *destination);

void *redirect_perform(Redirect *redirect);

char *redirect_toString(void *o);

unsigned int redirect_hashCode(void *o);

int redirect_compareTo(void *o1, void *o2);

void redirect_dispose(void *o);

void *redirect_clone(void *o);

static const DataType TYPE_REDIRECT = {"REDIRECT", redirect_toString, redirect_hashCode, redirect_compareTo,
                                       redirect_clone, redirect_dispose};

#endif //SHELL_REDIRECT_H
