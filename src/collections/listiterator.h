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

ListIterator *listiterator_new(void *object, void *state, void (*dispose)(void *s),
                               bool (*hasNext)(void *o, void **s),
                               void *(*next)(void *o, void **s), bool (*hasPrevious)(void *o, void **s),
                               void *(*previous)(void *o, void **s));

bool listiterator_hasNext(ListIterator *iterator);

void *listiterator_next(ListIterator *iterator);

bool listiterator_hasPrevious(ListIterator *iterator);

void *listiterator_previous(ListIterator *iterator);

void listiterator_dispose(ListIterator *iterator);

#endif //SHELL_LISTITERATOR_H
