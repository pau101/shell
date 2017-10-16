#include <stdlib.h>
#include <unistd.h>
#include <wait.h>
#include "sequence.h"
#include "../../util/preconditions.h"
#include "../../object/type/reference.h"

Sequence *sequence_new() {
    Sequence *sequence = calloc(1, sizeof(Sequence));
    sequence->pipelines = list_new();
    return sequence;
}

void sequence_add(Sequence *sequence, Pipeline *pipeline) {
    list_addLast(sequence->pipelines, object_new(&TYPE_PIPELINE, pipeline));
}

int sequence_exec(Sequence *sequence) {
    int status = EXIT_SUCCESS;
    Iterator *iter = list_iterator(sequence->pipelines);
    while (iterator_hasNext(iter)) {
        Pipeline *pipeline = object_get(iterator_next(iter), &TYPE_PIPELINE);
        pid_t cpid = fork();
        if (cpid < 0) {
            pExit("fork");
        }
        if (cpid == 0) {
            pipeline_exec(pipeline);
        } else {
            pid_t w = waitpid(cpid, &status, 0);
            if (w == -1) {
                pExit("waitpid");
            }
        }
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

void sequence_dispose(void *o) {
    if (o == NULL) {
        return;
    }
    list_dispose(((Sequence *) o)->pipelines);
    free(o);
}
