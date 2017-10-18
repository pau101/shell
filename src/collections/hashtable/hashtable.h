#ifndef SHELL_HASHTABLE_H
#define SHELL_HASHTABLE_H

#include "../iterator.h"
#include "../../object/object.h"

#define HT_INITIAL_CAPACITY 11
#define HT_LOAD_FACTOR 0.75

typedef struct htEntry {
    unsigned int hash;
    Object *key;
    Object *value;
    struct htEntry *next;
} HTEntry;

HTEntry *entry_new(unsigned int hash, Object *key, Object *value, HTEntry *next);

void entry_dispose(HTEntry *entry);

typedef struct hashtable {
    HTEntry **table;
    unsigned int capacity;
    unsigned int size;
    unsigned int threshold;
    float loadFactor;
} Hashtable;

Hashtable *hashtable_new(unsigned int initialCapacity, float loadFactor);

Object *hashtable_put(Hashtable *hashtable, Object *key, Object *value);

Object *hashtable_get(Hashtable *hashtable, Object *key);

Object *hashtable_remove(Hashtable *hashtable, Object *key);

Iterator *hashtable_iterator(Hashtable *hashtable);

void hashtable_dispose(Hashtable *hashtable);

#endif //SHELL_HASHTABLE_H
