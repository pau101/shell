#ifndef SHELL_PIPELINE_H
#define SHELL_PIPELINE_H

#include <stdbool.h>
#include "../../collections/linkedlist/linkedlist.h"
#include "../../util/util.h"
#include "command.h"
#include "../executable/executable.h"

typedef struct shell Shell;

typedef struct executable Executable;

typedef struct ioStreams IOStreams;

typedef struct pipeline {
    LinkedList *executables;
} Pipeline;

Pipeline *pipeline_new();

Executable *pipeline_executable(Pipeline *pipeline, char *source);

void pipeline_add(Pipeline *pipeline, Executable *executable);

int pipeline_exec(Pipeline *pipeline, Shell *shell, IOStreams *streams);

char *pipeline_toString(void *o);

unsigned int pipeline_hashCode(void *o);

int pipeline_compareTo(void *o1, void *o2);

void *pipeline_clone(void *o);

void pipeline_dispose(void *o);

static const DataType TYPE_PIPELINE = {"PIPELINE", pipeline_toString, pipeline_hashCode, pipeline_compareTo,
                                       pipeline_clone, pipeline_dispose};

#endif //SHELL_PIPELINE_H
