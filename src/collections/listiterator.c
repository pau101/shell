#include <stdlib.h>
#include "listiterator.h"

ListIterator *listIterator_new(void *object, void *state, void (*dispose)(void *s),
                               bool (*hasNext)(void *o, void **s),
                               void *(*next)(void *o, void **s), bool (*hasPrevious)(void *o, void **s),
                               void *(*previous)(void *o, void **s)) {
    ListIterator *iterator = calloc(1, sizeof(ListIterator));
    iterator->object = object;
    iterator->state = state;
    iterator->dispose = dispose;
    iterator->hasNext = hasNext;
    iterator->next = next;
    iterator->hasPrevious = hasPrevious;
    iterator->previous = previous;
    return iterator;
}

bool listIterator_hasNext(ListIterator *iterator) {
    return iterator->hasNext(iterator->object, &iterator->state);
}

void *listIterator_next(ListIterator *iterator) {
    return iterator->next(iterator->object, &iterator->state);
}

bool listIterator_hasPrevious(ListIterator *iterator) {
    return iterator->hasPrevious(iterator->object, &iterator->state);
}

void *listIterator_previous(ListIterator *iterator) {
    return iterator->previous(iterator->object, &iterator->state);
}

void listIterator_dispose(ListIterator *iterator) {
    if (iterator == NULL) {
        return;
    }
    iterator->dispose(iterator->state);
    free(iterator);
}
