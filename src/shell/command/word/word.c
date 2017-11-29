#include <stdlib.h>
#include "word.h"
#include "../../../util/preconditions.h"
#include "../../../object/type/reference.h"

Word *word_new(Object *getter, char *(*getValue)(Object *g, Shell *shell, IOStreams *streams)) {
    requireNonNull(getter);
    requireNonNull(getValue);
    Word *word = calloc(1, sizeof(Word));
    word->getter = getter;
    word->getValue = getValue;
    return word;
}

char *word_getValue(Word *word, Shell *shell, IOStreams *streams) {
    requireNonNull(word);
    requireNonNull(shell);
    requireNonNull(streams);
    return word->getValue(word->getter, shell, streams);
}

char *word_toString(void *o) {
    return reference_toString(o);
}

unsigned int word_hashCode(void *o) {
    return reference_hashCode(o);
}

int word_compareTo(void *o1, void *o2) {
    return reference_compareTo(o1, o2);
}

void *word_clone(void *o) {
    return word_new(object_clone(((Word *) o)->getter), ((Word *) o)->getValue);
}

void word_dispose(void *o) {
    if (o == NULL) {
        return;
    }
    object_dispose(((Word *) o)->getter);
    free(o);
}
