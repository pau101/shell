#ifndef SHELL_SEQUENCE_H
#define SHELL_SEQUENCE_H

#include "../../collections/linkedlist/linkedlist.h"
#include "pipeline.h"

typedef struct sequence {
    LinkedList *pipelines;
} Sequence;

Sequence *sequence_new();

void sequence_add(Sequence *sequence, Pipeline *pipeline);

int sequence_exec(Sequence *sequence);

char *sequence_toString(void *o);

unsigned int sequence_hashCode(void *o);

int sequence_compareTo(void *o1, void *o2);

void sequence_dispose(void *o);

static const DataType TYPE_SEQUENCE = {"SEQUENCE", sequence_toString, sequence_hashCode, sequence_compareTo,
                                       sequence_dispose};

#endif //SHELL_SEQUENCE_H
