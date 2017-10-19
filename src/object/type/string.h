#ifndef SHELL_STRING_H
#define SHELL_STRING_H

#include "../datatype.h"

char *string_toString(void *o);

unsigned int string_hashCode(void *o);

int string_compareTo(void *o1, void *o2);

void string_dispose(void *o);

void *string_clone(void *o);

static const DataType TYPE_STRING = {"STRING", string_toString, string_hashCode, string_compareTo, string_clone,
                                     string_dispose};

#endif //SHELL_STRING_H
