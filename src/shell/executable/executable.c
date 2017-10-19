#include <stdlib.h>
#include "executable.h"
#include "../../util/preconditions.h"
#include "../../object/type/reference.h"
#include "../../object/type/string.h"
#include "../mode/shellmode.h"

Executable *executable_new(Object *executor, char *source, int (*execute)(Shell *shell, Object *e)) {
    requireNonNull(executor);
    requireNonNull(source);
    requireNonNull(execute);
    Executable *executable = calloc(1, sizeof(Executable));
    executable->executor = executor;
    executable->source = source;
    executable->execute = execute;
    return executable;
}

int executable_execute(Shell *shell, Executable *executable) {
    requireNonNull(executable);
    return executable->execute(shell, executable->executor);
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
    return executable_new(object_clone(e->executor), string_clone(e->source), e->execute);
}

void executable_dispose(void *o) {
    if (o == NULL) {
        return;
    }
    object_dispose(((Executable *) o)->executor);
    free(((Executable *) o)->source);
    free(o);
}
