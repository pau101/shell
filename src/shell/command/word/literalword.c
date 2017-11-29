#include <stdlib.h>
#include "literalword.h"
#include "../../../object/type/reference.h"
#include "../../../object/type/string.h"

LiteralWord *literalword_new(char *value) {
    LiteralWord *word = calloc(1, sizeof(LiteralWord));
    word->value = value;
    return word;
}

char *literalword_getValue_(Object *g, Shell *shell, IOStreams *streams) {
    return literalword_getValue(object_get(g, &TYPE_LITERAL_WORD), shell, streams);
}

Word *literalword_word(char *value) {
    return word_new(object_new(&TYPE_LITERAL_WORD, literalword_new(value)), literalword_getValue_);
}

char *literalword_getValue(LiteralWord *literalWord, Shell *shell, IOStreams *streams) {
    return string_clone(literalWord->value);
}

char *literalword_toString(void *o) {
    return reference_toString(o);
}

unsigned int literalword_hashCode(void *o) {
    return reference_hashCode(o);
}

int literalword_compareTo(void *o1, void *o2) {
    return reference_compareTo(o1, o2);
}

void *literalword_clone(void *o) {
    return literalword_new(string_clone(((LiteralWord *) o)->value));
}

void literalword_dispose(void *o) {
    if (o == NULL) {
        return;
    }
    free(((LiteralWord *) o)->value);
    free(o);
}
