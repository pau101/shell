#include <stdlib.h>
#include "executable.h"
#include "../../util/preconditions.h"
#include "../../object/type/reference.h"

Executable *executable_new(Object *executor, int (*execute)(Object *e)) {
    requireNonNull(executor);
    requireNonNull(execute);
    Executable *executable = calloc(1, sizeof(Executable));
    executable->executor = executor;
    executable->execute = execute;
    return executable;
}

int executable_execute(Executable *executable) {
    requireNonNull(executable);
    return executable->execute(executable->executor);
}

char *executable_toString(void *o) {
    if (o == NULL) {
        return reference_toString(o);
    }
    return object_toString(((Executable *) o)->executor);
}

unsigned int executable_hashCode(void *o) {
    return reference_hashCode(o);
}

int executable_compareTo(void *o1, void *o2) {
    return reference_compareTo(o1, o2);
}

void *executable_clone(void *o) {
    Executable *e = (Executable *) o;
    return executable_new(object_clone(e->executor), e->execute);
}

void executable_dispose(void *o) {
    if (o == NULL) {
        return;
    }
    object_dispose(((Executable *) o)->executor);
    free(o);
}
