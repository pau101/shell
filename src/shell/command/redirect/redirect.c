#include <stdlib.h>
#include <unistd.h>
#include "redirect.h"
#include "../../../object/type/reference.h"
#include "../../../util/preconditions.h"

Redirect *redirect_new(int source, Handle *destination) {
    Redirect *redirect = calloc(1, sizeof(Redirect));
    redirect->source = source;
    redirect->destination = destination;
    return redirect;
}

void *redirect_perform(Redirect *redirect) {
    FILE *f = handle_open(redirect->destination);
    close(redirect->source);
    if (dup2(fileno(f), redirect->source) < 0) {
        pExit("dup2");
    }
    fclose(f);
}

char *redirect_toString(void *o) {
    return reference_toString(o);
}

unsigned int redirect_hashCode(void *o) {
    return reference_hashCode(o);
}

int redirect_compareTo(void *o1, void *o2) {
    return reference_compareTo(o1, o2);
}

void redirect_dispose(void *o) {
    if (o == NULL) {
        return;
    }
    handle_dispose(((Redirect *) o)->destination);
    free(o);
}
