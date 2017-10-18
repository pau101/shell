#include <stdlib.h>
#include "executablebuilder.h"
#include "../../object/type/reference.h"
#include "../../util/preconditions.h"

ExecutableBuilder *
execbldr_new(Executable *executable,
             ExecutableBuilder *(*beginPipe)(ExecutableBuilder *builder, ThrowingBlock *tb, ExecutableBuilder *other),
             ExecutableBuilder *(*beginSequence)(ExecutableBuilder *builder, ThrowingBlock *tb, ExecutableBuilder *other),
             ExecutableBuilder *(*beginEnd)(ExecutableBuilder *builder, ThrowingBlock *tb, ExecutableBuilder *other)) {
    requireNonNull(beginPipe);
    requireNonNull(beginSequence);
    requireNonNull(beginEnd);
    ExecutableBuilder *builder = calloc(1, sizeof(ExecutableBuilder));
    builder->executable = executable;
    builder->beginPipe = beginPipe;
    builder->beginSequence = beginSequence;
    builder->beginEnd = beginEnd;
    return builder;
}

void *execbldr_getExecutor(ExecutableBuilder *builder, const DataType *dataType) {
    requireNonNull(builder);
    requireNonNull(dataType);
    return object_get(builder->executable->executor, dataType);
}

ExecutableBuilder *execbldr_beginPipeline(ExecutableBuilder *builder, ThrowingBlock *tb, ExecutableBuilder *other) {
    requireNonNull(builder);
    requireNonNull(tb);
    requireNonNull(other);
    return builder->beginPipe(builder, tb, other);
}

ExecutableBuilder *execbldr_beginSequence(ExecutableBuilder *builder, ThrowingBlock *tb, ExecutableBuilder *other) {
    requireNonNull(builder);
    requireNonNull(tb);
    requireNonNull(other);
    return builder->beginSequence(builder, tb, other);
}

ExecutableBuilder *execbldr_beginEnd(ExecutableBuilder *builder, ThrowingBlock *tb, ExecutableBuilder *other) {
    requireNonNull(builder);
    requireNonNull(tb);
    requireNonNull(other);
    return builder->beginEnd(builder, tb, other);
}

char *execbldr_toString(void *o) {
    return reference_toString(o);
}

unsigned int execbldr_hashCode(void *o) {
    return reference_hashCode(o);
}

int execbldr_compareTo(void *o1, void *o2) {
    return reference_compareTo(o1, o2);
}

void execbldr_dispose(void *o) {
    if (o == NULL) {
        return;
    }
    executable_dispose(((ExecutableBuilder *) o)->executable);
    free(o);
}
