#ifndef SHELL_SEQUENCE_H
#define SHELL_SEQUENCE_H

#include "../../collections/linkedlist/linkedlist.h"
#include "pipeline.h"
#include "../executable/executable.h"

struct sequence {
    LinkedList *executables;
};

Sequence *sequence_new();

Executable *sequence_executable(Sequence *sequence, char *source);

void sequence_add(Sequence *sequence, Executable *executable);

int sequence_exec(Sequence *sequence, Shell *shell, IOStreams *streams);

char *sequence_toString(void *o);

unsigned int sequence_hashCode(void *o);

int sequence_compareTo(void *o1, void *o2);

void *sequence_clone(void *o);

void sequence_dispose(void *o);

static const DataType TYPE_SEQUENCE = {"SEQUENCE", sequence_toString, sequence_hashCode, sequence_compareTo,
                                       sequence_clone, sequence_dispose};

#endif //SHELL_SEQUENCE_H
