#include <stdlib.h>
#include "wordarray.h"
#include "../../../object/type/reference.h"

WordArray *wordarray_new() {
    WordArray *word = calloc(1, sizeof(WordArray));
    word->words = list_new();
    return word;
}

char *wordarray_getValue_(Object *g, Shell *shell, IOStreams *streams) {
    return wordarray_getValue(object_get(g, &TYPE_WORD_ARRAY), shell, streams);
}

Word *wordarray_word(WordArray *wordArray) {
    return word_new(object_new(&TYPE_WORD_ARRAY, wordArray), wordarray_getValue_);
}

char *wordarray_getValue(WordArray *wordArray, Shell *shell, IOStreams *streams) {
    size_t size = 8;
    int count = 0;
    char *value = calloc(size, sizeof(char));
    Iterator *itr = list_iterator(wordArray->words);
    while (iterator_hasNext(itr)) {
        char *v = word_getValue(object_get(iterator_next(itr), &TYPE_WORD), shell, streams);
        for (int i = 0; v[i] != '\0'; i++) {
            if (count >= size) {
                value = realloc(value, (size = size * 2 + 1) * sizeof(char));
            }
            value[count++] = v[i];
        }
        free(v);
    }
    iterator_dispose(itr);
    value = realloc(value, (count + 1) * sizeof(char));
    value[count] = '\0';
    return value;
}

char *wordarray_toString(void *o) {
    return reference_toString(o);
}

unsigned int wordarray_hashCode(void *o) {
    return reference_hashCode(o);
}

int wordarray_compareTo(void *o1, void *o2) {
    return reference_compareTo(o1, o2);
}

void *wordarray_clone(void *o) {
    WordArray *n = wordarray_new();
    Iterator *itr = list_iterator(((WordArray *) o)->words);
    while (iterator_hasNext(itr)) {
        list_addLast(n->words, word_clone(object_get(iterator_next(itr), &TYPE_WORD)));
    }
    iterator_dispose(itr);
    return n;
}

void wordarray_dispose(void *o) {
    if (o == NULL) {
        return;
    }
    list_dispose(((WordArray *) o)->words);
    free(o);
}
