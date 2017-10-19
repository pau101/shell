#ifndef SHELL_FUNCTION_H
#define SHELL_FUNCTION_H

#include "../datatype.h"

char *function_toString(void *o);

unsigned int function_hashCode(void *o);

int function_compareTo(void *o1, void *o2);

void *function_clone(void *o);

void function_dispose(void *o);

static const DataType TYPE_FUNCTION = {"FUNCTION", function_toString, function_hashCode, function_compareTo,
                                       function_clone, function_dispose};

#endif //SHELL_FUNCTION_H
