#include <stdlib.h>
#include "iterator.h"

Iterator *iterator_new(void *object, void *state, void (*dispose)(void *s), bool (*hasNext)(void *o, void **s),
                       void *(*next)(void *o, void **s)) {
    Iterator *iterator = calloc(1, sizeof(Iterator));
    iterator->object = object;
    iterator->state = state;
    iterator->dispose = dispose;
    iterator->hasNext = hasNext;
    iterator->next = next;
    return iterator;
}

bool iterator_hasNext(Iterator *iterator) {
    return iterator->hasNext(iterator->object, &iterator->state);
}

void *iterator_next(Iterator *iterator) {
    return iterator->next(iterator->object, &iterator->state);
}

void iterator_dispose(Iterator *iterator) {
    if (iterator == NULL) {
        return;
    }
    iterator->dispose(iterator->state);
    free(iterator);
}
