#include <stdlib.h>
#include <unistd.h>
#include <wait.h>
#include "sequence.h"
#include "../../util/preconditions.h"
#include "../../object/type/reference.h"

Sequence *sequence_new() {
    Sequence *sequence = calloc(1, sizeof(Sequence));
    sequence->executables = list_new();
    return sequence;
}

int sequence_exec_(Object *e, Shell *shell, IOStreams *streams) {
    return sequence_exec(object_get(e, &TYPE_SEQUENCE), shell, streams);
}

Executable *sequence_executable(Sequence *sequence, char *source) {
    requireNonNull(sequence);
    return executable_new(object_new(&TYPE_SEQUENCE, sequence), source, sequence_exec_);
}

void sequence_add(Sequence *sequence, Executable *executable) {
    requireNonNull(sequence);
    requireNonNull(executable);
    list_addLast(sequence->executables, object_new(&TYPE_EXECUTABLE, executable));
}

int sequence_exec(Sequence *sequence, Shell *shell, IOStreams *streams) {
    requireNonNull(sequence);
    int status = EXIT_SUCCESS;
    Iterator *iter = list_iterator(sequence->executables);
    while (iterator_hasNext(iter)) {
        status = executable_execute(object_get(iterator_next(iter), &TYPE_EXECUTABLE), shell, streams);
    }
    iterator_dispose(iter);
    return status;
}

char *sequence_toString(void *o) {
    return reference_toString(o);
}

unsigned int sequence_hashCode(void *o) {
    return reference_hashCode(o);
}

int sequence_compareTo(void *o1, void *o2) {
    return reference_compareTo(o1, o2);
}

void *sequence_clone(void *o) {
    Sequence *p = (Sequence *) o;
    Sequence *n = sequence_new();
    Iterator *itr = list_iterator(p->executables);
    while (iterator_hasNext(itr)) {
        sequence_add(n, executable_clone(object_get(iterator_next(itr), &TYPE_EXECUTABLE)));
    }
    iterator_dispose(itr);
    return n;
}

void sequence_dispose(void *o) {
    if (o == NULL) {
        return;
    }
    list_dispose(((Sequence *) o)->executables);
    free(o);
}
