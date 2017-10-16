#ifndef SHELL_INT_H
#define SHELL_INT_H

#include "../datatype.h"

char *int_toString(void *o);

unsigned int int_hashCode(void *o);

int int_compareTo(void *o1, void *o2);

void int_dispose(void *o);

static const DataType TYPE_INT = {"INT", int_toString, int_hashCode, int_compareTo, int_dispose};

#endif //SHELL_INT_H
