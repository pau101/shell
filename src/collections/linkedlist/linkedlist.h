#ifndef SHELL_LINKEDLIST_H
#define SHELL_LINKEDLIST_H

#include <stdbool.h>
#include "../iterator.h"
#include "../listiterator.h"
#include "../../object/object.h"

typedef struct node {
    Object *item;
    struct node *prev;
    struct node *next;
} Node;

Node *node_new(Object *item);

void node_dispose(Node *node);

typedef struct linkedList {
    int size;
    Node *head;
    Node *tail;
} LinkedList;

LinkedList *list_new();

bool list_isEmpty(LinkedList *linkedList);

Object *list_get(LinkedList *linkedList, int index);

void list_addFirst(LinkedList *linkedList, Object *e);

void list_addLast(LinkedList *linkedList, Object *e);

Object *list_removeFirst(LinkedList *linkedList);

Object *list_removeLast(LinkedList *linkedList);

Object *list_peekFirst(LinkedList *linkedList);

Object *list_peekLast(LinkedList *linkedList);

Iterator *list_iterator(LinkedList *linkedList);

Iterator *list_descendingIterator(LinkedList *linkedList);

ListIterator *list_listIterator(LinkedList *linkedList, int index);

void list_dispose(LinkedList *linkedList);

#endif //SHELL_LINKEDLIST_H
