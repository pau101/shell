#ifndef SHELL_LISTITERATOR_H
#define SHELL_LISTITERATOR_H

#include <stdbool.h>

typedef struct listIterator {
    void *object;
    void *state;

    void (*dispose)(void *state);

    bool (*hasNext)(void *object, void **state);

    void *(*next)(void *object, void **state);

    bool (*hasPrevious)(void *object, void **state);

    void *(*previous)(void *object, void **state);
} ListIterator;

ListIterator *listIterator_new(void *object, void *state, void (*dispose)(void *s),
                               bool (*hasNext)(void *o, void **s),
                               void *(*next)(void *o, void **s), bool (*hasPrevious)(void *o, void **s),
                               void *(*previous)(void *o, void **s));

bool listIterator_hasNext(ListIterator *iterator);

void *listIterator_next(ListIterator *iterator);

bool listIterator_hasPrevious(ListIterator *iterator);

void *listIterator_previous(ListIterator *iterator);

void listIterator_dispose(ListIterator *iterator);

#endif //SHELL_LISTITERATOR_H
