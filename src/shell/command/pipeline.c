#include <stdlib.h>
#include <unistd.h>
#include "pipeline.h"
#include "../../util/preconditions.h"
#include "../../object/type/reference.h"

#define READ 0
#define WRITE 1

Pipeline *pipeline_new() {
    Pipeline *pipeline = calloc(1, sizeof(Pipeline));
    pipeline->commands = list_new();
    return pipeline;
}

void pipeline_add(Pipeline *pipeline, Command *command) {
    list_addLast(pipeline->commands, object_new(&TYPE_COMMAND, command));
}

void pipeline_exec(Pipeline *pipeline) {
    if (pipeline->commands->size == 1) {
        command_exec(object_get(list_peekFirst(pipeline->commands), &TYPE_COMMAND));
    } else {
        pid_t pid = 0;
        int fd[2];
        Iterator *iter = list_descendingIterator(pipeline->commands);
        while (iterator_hasNext(iter)) {
            Command *command = object_get(iterator_next(iter), &TYPE_COMMAND);
            if (pipe(fd) < 0) {
                pExit("pipe");
            }
            pid_t cpid = fork();
            if (cpid < 0) {
                pExit("fork");
            }
            if (cpid == 0) {
                close(fd[READ]);
                if (dup2(fd[WRITE], STDOUT_FILENO) < 0) {
                    pExit("dup2");
                }
                close(fd[WRITE]);
            } else {
                close(fd[WRITE]);
                if (dup2(fd[READ], STDIN_FILENO) < 0) {
                    pExit("dup2");
                }
                close(fd[READ]);
                if (pid == 0) {
                    pid = cpid;
                }
                command_exec(command);
            }
        }
        iterator_dispose(iter);
        exit(EXIT_SUCCESS);
    }
}

char *pipeline_toString(void *o) {
    return reference_toString(o);
}

unsigned int pipeline_hashCode(void *o) {
    return reference_hashCode(o);
}

int pipeline_compareTo(void *o1, void *o2) {
    return reference_compareTo(o1, o2);
}

void pipeline_dispose(void *o) {
    if (o == NULL) {
        return;
    }
    list_dispose(((Pipeline *) o)->commands);
    free(o);
}
