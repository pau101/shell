#ifndef SHELL_ITERATOR_H
#define SHELL_ITERATOR_H

#include <stdbool.h>

typedef struct iterator {
    void *object;
    void *state;

    void (*dispose)(void *state);

    bool (*hasNext)(void *object, void **state);

    void *(*next)(void *object, void **state);
} Iterator;

Iterator *iterator_new(void *object, void *state, void (*dispose)(void *s), bool (*hasNext)(void *o, void **s),
                       void *(*next)(void *o, void **s));

bool iterator_hasNext(Iterator *iterator);

void *iterator_next(Iterator *iterator);

void iterator_dispose(Iterator *iterator);

#endif //SHELL_ITERATOR_H
