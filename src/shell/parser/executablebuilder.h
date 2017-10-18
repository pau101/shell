#ifndef SHELL_EXECUTABLEBUILDER_H
#define SHELL_EXECUTABLEBUILDER_H

#include "../../util/throwingblock.h"
#include "../executable/executable.h"

typedef struct executableBuilder {
    Executable *executable;

    struct executableBuilder *
    (*beginPipe)(struct executableBuilder *builder, ThrowingBlock *tb, struct executableBuilder *other);

    struct executableBuilder *
    (*beginSequence)(struct executableBuilder *builder, ThrowingBlock *tb, struct executableBuilder *other);

    struct executableBuilder *
    (*beginEnd)(struct executableBuilder *builder, ThrowingBlock *tb, struct executableBuilder *other);
} ExecutableBuilder;

ExecutableBuilder *
execbldr_new(Executable *executable,
             ExecutableBuilder *(*beginPipe)(ExecutableBuilder *builder, ThrowingBlock *tb, ExecutableBuilder *other),
             ExecutableBuilder *(*beginSequence)(ExecutableBuilder *builder, ThrowingBlock *tb,
                                                 ExecutableBuilder *other),
             ExecutableBuilder *(*beginEnd)(ExecutableBuilder *builder, ThrowingBlock *tb,
                                            ExecutableBuilder *other));

void *execbldr_getExecutor(ExecutableBuilder *builder, const DataType *dataType);

ExecutableBuilder *execbldr_beginPipeline(ExecutableBuilder *builder, ThrowingBlock *tb, ExecutableBuilder *other);

ExecutableBuilder *execbldr_beginSequence(ExecutableBuilder *builder, ThrowingBlock *tb, ExecutableBuilder *other);

ExecutableBuilder *execbldr_beginEnd(ExecutableBuilder *builder, ThrowingBlock *tb, ExecutableBuilder *other);

char *execbldr_toString(void *o);

unsigned int execbldr_hashCode(void *o);

int execbldr_compareTo(void *o1, void *o2);

void execbldr_dispose(void *o);

static const DataType TYPE_EXECUTABLE_BUILDER = {"EXECBLDR", execbldr_toString, execbldr_hashCode, execbldr_compareTo,
                                                 execbldr_dispose};

#endif //SHELL_EXECUTABLEBUILDER_H
