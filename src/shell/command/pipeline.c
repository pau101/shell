#include <stdlib.h>
#include <unistd.h>
#include <wait.h>
#include "pipeline.h"
#include "../../util/preconditions.h"
#include "../../object/type/reference.h"

#define READ 0
#define WRITE 1

Pipeline *pipeline_new() {
    Pipeline *pipeline = calloc(1, sizeof(Pipeline));
    pipeline->executables = list_new();
    return pipeline;
}

int pipeline_exec_(Shell *shell, Object *e) {
    return pipeline_exec(shell, object_get(e, &TYPE_PIPELINE));
}

Executable *pipeline_executable(Pipeline *pipeline, char *source) {
    requireNonNull(pipeline);
    return executable_new(object_new(&TYPE_PIPELINE, pipeline), source, pipeline_exec_);
}

void pipeline_add(Pipeline *pipeline, Executable *executable) {
    requireNonNull(pipeline);
    requireNonNull(executable);
    list_addLast(pipeline->executables, object_new(&TYPE_EXECUTABLE, executable));
}

void pipeline_pipe(Shell *shell, int fd[2], Executable *executable, bool doRead, bool doWrite) {
    if (pipe(fd) < 0) {
        pExit("pipe");
    }
    pid_t cpid = fork();
    if (cpid < 0) {
        pExit("fork");
    }
    if (cpid == 0) {
        if (doWrite) {
            close(fd[READ]);
            if (dup2(fd[WRITE], STDOUT_FILENO) < 0) {
                pExit("dup2");
            }
            close(fd[WRITE]);
        }
    } else {
        if (doRead) {
            close(fd[WRITE]);
            if (dup2(fd[READ], STDIN_FILENO) < 0) {
                pExit("dup2");
            }
            close(fd[READ]);
        }
        exit(executable_execute(shell, executable));
    }
}

int pipeline_exec(Shell *shell, Pipeline *pipeline) {
    requireNonNull(pipeline);
    if (pipeline->executables->size == 1) {
        return executable_execute(shell, object_get(list_peekFirst(pipeline->executables), &TYPE_EXECUTABLE));
    }
    pid_t cpid = fork();
    if (cpid < 0) {
        pExit("fork");
    }
    if (cpid == 0) {
        int fd[2];
        ListIterator *itr = list_listIterator(pipeline->executables, pipeline->executables->size);
        while (listiterator_hasPrevious(itr)) {
            Executable *executable = object_get(listiterator_previous(itr), &TYPE_EXECUTABLE);
            pipeline_pipe(shell, fd, executable, listiterator_hasPrevious(itr), listiterator_hasNext(itr));
        }
        listiterator_dispose(itr);
        exit(EXIT_SUCCESS);
    }
    int status;
    pid_t w = waitpid(cpid, &status, 0);
    if (w == -1) {
        pExit("waitpid");
    }
    return status;
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

void *pipeline_clone(void *o) {
    Pipeline *p = (Pipeline *) o;
    Pipeline *n = pipeline_new();
    Iterator *itr = list_iterator(p->executables);
    while (iterator_hasNext(itr)) {
        pipeline_add(n, executable_clone(object_get(iterator_next(itr), &TYPE_EXECUTABLE)));
    }
    iterator_dispose(itr);
    return n;
}

void pipeline_dispose(void *o) {
    if (o == NULL) {
        return;
    }
    list_dispose(((Pipeline *) o)->executables);
    free(o);
}
