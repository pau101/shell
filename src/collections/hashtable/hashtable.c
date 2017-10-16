#include <stdlib.h>
#include <limits.h>
#include "hashtable.h"
#include "../../util/preconditions.h"

#define MIN(a, b) (a) < (b) ? (a) : (b);

Entry *entry_new(unsigned int hash, Object *key, Object *value, Entry *next) {
    requireNonNull(key);
    Entry *entry = calloc(1, sizeof(Entry));
    entry->hash = hash;
    entry->key = key;
    entry->value = value;
    entry->next = next;
    return entry;
}

void entry_dispose(Entry *entry) {
    if (entry == NULL) {
        return;
    }
    object_dispose(entry->key);
    object_dispose(entry->value);
    for (Entry *e = entry->next; e != NULL; e = e->next) {
        entry_dispose(entry);
    }
    free(entry);
}

Hashtable *hashtable_new(unsigned int initialCapacity, float loadFactor) {
    Hashtable *hashtable = calloc(1, sizeof(Hashtable));
    hashtable->table = calloc(initialCapacity, sizeof(Entry *));
    hashtable->capacity = initialCapacity;
    hashtable->size = 0;
    hashtable->threshold = (unsigned int) (initialCapacity * loadFactor);
    hashtable->loadFactor = loadFactor;
    return hashtable;
}

void hashtable_rehash(Hashtable *hashtable) {
    unsigned int oldCapacity = hashtable->capacity;
    Entry **oldTable = hashtable->table;
    unsigned int newCapacity = oldCapacity * 2 + 1;
    if (newCapacity < oldCapacity) {
        if (oldCapacity == UINT_MAX - 1) {
            return;
        }
        newCapacity = UINT_MAX - 1;
    }
    Entry **newTable = calloc(newCapacity, sizeof(Entry *));
    hashtable->table = newTable;
    hashtable->capacity = newCapacity;
    hashtable->threshold = MIN((unsigned int) (newCapacity * hashtable->loadFactor), UINT_MAX);
    for (unsigned int i = oldCapacity; i-- > 0;) {
        for (Entry *old = oldTable[i]; old != NULL;) {
            Entry *e = old;
            old = old->next;
            unsigned int index = e->hash % newCapacity;
            e->next = newTable[index];
            newTable[index] = e;
        }
    }
    free(oldTable);
}

Object *hashtable_put(Hashtable *hashtable, Object *key, Object *value) {
    requireNonNull(hashtable);
    requireNonNull(key);
    unsigned int hash = object_hashCode(key);
    unsigned int index = hash % hashtable->capacity;
    for (Entry *entry = hashtable->table[index]; entry != NULL; entry = entry->next) {
        if (entry->hash == hash && object_compareTo(entry->key, key) == 0) {
            Object *old = entry->value;
            entry->value = value;
            return old;
        }
    }
    if (hashtable->size >= hashtable->threshold) {
        hashtable_rehash(hashtable);
        hash = object_hashCode(key);
        index = hash % hashtable->capacity;
    }
    Entry *e = hashtable->table[index];
    hashtable->table[index] = entry_new(hash, key, value, e);
    hashtable->size++;
    return NULL;
}

Object *hashtable_get(Hashtable *hashtable, Object *key) {
    requireNonNull(hashtable);
    requireNonNull(key);
    unsigned int hash = object_hashCode(key);
    unsigned int index = hash % hashtable->capacity;
    for (Entry *e = hashtable->table[index]; e != NULL; e = e->next) {
        if (e->hash == hash && object_compareTo(e->key, key) == 0) {
            return e->value;
        }
    }
    return NULL;
}

Object *hashtable_remove(Hashtable *hashtable, Object *key) {
    requireNonNull(hashtable);
    requireNonNull(key);
    unsigned int hash = object_hashCode(key);
    unsigned int index = hash % hashtable->capacity;
    Entry *e = hashtable->table[index];
    for (Entry *prev = NULL; e != NULL; prev = e, e = e->next) {
        if (e->hash == hash && object_compareTo(e->key, key) == 0) {
            if (prev != NULL) {
                prev->next = e->next;
            } else {
                hashtable->table[index] = e->next;
            }
            hashtable->size--;
            Object *oldValue = e->value;
            e->value = NULL;
            return oldValue;
        }
    }
    return NULL;
}

typedef struct hashtableIter {
    int index;
    Entry *entry;
} HashtableIter;

void hashtableiter_dispose(void *state) {
    if (state == NULL) {
        return;
    }
    free(state);
}

bool hashtableitr_hasNext(void *object, void **state) {
    Hashtable *hashtable = (Hashtable *) object;
    HashtableIter *itr = (HashtableIter *) *state;
    Entry *e = itr->entry;
    int i = itr->index;
    while (e == NULL && i > 0) {
        e = hashtable->table[--i];
    }
    itr->entry = e;
    itr->index = i;
    return e != NULL;
}

void *hashtableitr_next(void *object, void **state) {
    Hashtable *hashtable = (Hashtable *) object;
    HashtableIter *itr = (HashtableIter *) *state;
    Entry *et = itr->entry;
    int i = itr->index;
    while (et == NULL && i > 0) {
        et = hashtable->table[--i];
    }
    itr->entry = et;
    itr->index = i;
    if (et != NULL) {
        Entry *e = itr->entry;
        itr->entry = e->next;
        return e;
    }
    errExit("No such element");
}

Iterator *hashtable_iterator(Hashtable *hashtable) {
    requireNonNull(hashtable);
    HashtableIter *itr = calloc(1, sizeof(HashtableIter));
    itr->index = hashtable->capacity;
    itr->entry = NULL;
    return iterator_new(hashtable, itr, hashtableiter_dispose, hashtableitr_hasNext, hashtableitr_next);
}

void hashtable_dispose(Hashtable *hashtable) {
    if (hashtable == NULL) {
        return;
    }
    for (int i = 0; i < hashtable->size; i++) {
        entry_dispose(hashtable->table[i]);
    }
    free(hashtable->table);
    free(hashtable);
}
