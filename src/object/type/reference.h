#ifndef SHELL_REFERENCE_H
#define SHELL_REFERENCE_H

#include "../datatype.h"

char *reference_toString(void *o);

unsigned int reference_hashCode(void *o);

int reference_compareTo(void *o1, void *o2);

void *reference_clone(void *o);

void reference_dispose(void *o);

static const DataType TYPE_REFERENCE = {"REFERENCE", reference_toString, reference_hashCode, reference_compareTo,
                                        reference_clone, reference_dispose};

#endif //SHELL_REFERENCE_H
