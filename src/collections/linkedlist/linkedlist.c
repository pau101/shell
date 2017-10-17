#include <sched.h>
#include <stdlib.h>
#include "linkedlist.h"
#include "../../util/preconditions.h"

Node *node_new(Object *item) {
    Node *node = calloc(1, sizeof(Node));
    node->item = item;
    node->prev = NULL;
    node->next = NULL;
    return node;
}

void node_dispose(Node *node) {
    object_dispose(node->item);
    free(node);
}

LinkedList *list_new() {
    LinkedList *linkedList = calloc(1, sizeof(LinkedList));
    linkedList->size = 0;
    linkedList->head = node_new(NULL);
    linkedList->tail = node_new(NULL);
    linkedList->head->next = linkedList->tail;
    linkedList->tail->prev = linkedList->head;
    return linkedList;
}

bool list_isEmpty(LinkedList *linkedList) {
    requireNonNull(linkedList);
    return linkedList->size == 0;
}

Node *list_node(LinkedList *linkedList, int index) {
    if (index < linkedList->size / 2) {
        Node *n = linkedList->head->next;
        for (int i = 0; i < index; i++) {
            n = n->next;
        }
        return n;
    } else {
        Node *n = linkedList->tail->prev;
        for (int i = linkedList->size - 1; i > index; i--) {
            n = n->prev;
        }
        return n;
    }
}

Object *list_get(LinkedList *linkedList, int index) {
    checkElementIndex(index, linkedList->size);
    return list_node(linkedList, index)->item;
}

void list_insert(LinkedList *linkedList, Node *pos, Node *node) {
    node->prev = pos;
    node->next = pos->next;
    pos->next->prev = node;
    pos->next = node;
    linkedList->size++;
}

Object *list_remove(LinkedList *linkedList, Node *node) {
    node->prev->next = node->next;
    node->next->prev = node->prev;
    Object *item = node->item;
    free(node);
    linkedList->size--;
    return item;
}

void list_addFirst(LinkedList *linkedList, Object *e) {
    requireNonNull(linkedList);
    list_insert(linkedList, linkedList->head, node_new(e));
}

void list_addLast(LinkedList *linkedList, Object *e) {
    requireNonNull(linkedList);
    list_insert(linkedList, linkedList->tail->prev, node_new(e));
}

Object *list_removeFirst(LinkedList *linkedList) {
    requireNonNull(linkedList);
    if (list_isEmpty(linkedList)) {
        errExit("No such element");
    }
    return list_remove(linkedList, linkedList->head->next);
}

Object *list_removeLast(LinkedList *linkedList) {
    requireNonNull(linkedList);
    if (list_isEmpty(linkedList)) {
        errExit("No such element");
    }
    return list_remove(linkedList, linkedList->tail->prev);
}

Object *list_peekFirst(LinkedList *linkedList) {
    requireNonNull(linkedList);
    if (list_isEmpty(linkedList)) {
        return NULL;
    }
    return linkedList->head->next->item;
}

Object *list_peekLast(LinkedList *linkedList) {
    requireNonNull(linkedList);
    if (list_isEmpty(linkedList)) {
        return NULL;
    }
    return linkedList->tail->prev->item;
}

void listiter_dispose(void *state) {}

bool listiter_hasNext(void *object, void **state) {
    return ((Node *) *state)->next != NULL;
}

void *listiter_next(void *object, void **state) {
    Node **cur = (Node **) state;
    if ((*cur)->next != NULL) {
        Object *item = (*cur)->item;
        *cur = (*cur)->next;
        return item;
    }
    errExit("No such element");
}

bool listiter_hasPrevious(void *object, void **state) {
    return ((Node *) *state)->prev != NULL;
}

void *listiter_previous(void *object, void **state) {
    Node **cur = (Node **) state;
    if ((*cur)->prev != NULL) {
        Object *item = (*cur)->item;
        *cur = (*cur)->prev;
        return item;
    }
    errExit("No such element");
}

Iterator *list_iterator(LinkedList *linkedList) {
    requireNonNull(linkedList);
    return iterator_new(linkedList, linkedList->head->next, listiter_dispose, listiter_hasNext, listiter_next);
}

Iterator *list_descendingIterator(LinkedList *linkedList) {
    requireNonNull(linkedList);
    return iterator_new(linkedList, linkedList->tail->prev, listiter_dispose, listiter_hasPrevious, listiter_previous);
}

ListIterator *list_listIterator(LinkedList *linkedList, int index) {
    requireNonNull(linkedList);
    checkPositionIndex(index, linkedList->size);
    Node *nextNode = index == linkedList->size ? linkedList->tail->prev : list_node(linkedList, index);
    return listiterator_new(linkedList, nextNode, listiter_dispose, listiter_hasNext, listiter_next,
                            listiter_hasPrevious, listiter_previous);
}

void list_dispose(LinkedList *linkedList) {
    if (linkedList == NULL) {
        return;
    }
    for (Node *n = linkedList->head; n != NULL;) {
        Node *next = n->next;
        node_dispose(n);
        n = next;
    }
    free(linkedList);
}
