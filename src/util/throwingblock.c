#include <stdlib.h>
#include <setjmp.h>
#include "throwingblock.h"
#include "preconditions.h"
#include "util.h"
#include "../object/type/int.h"

#define START_ID 0

#define ERROR_STATUS 1

ThrowingBlock *tb_new() {
    ThrowingBlock *throwingBlock = calloc(1, sizeof(ThrowingBlock));
    throwingBlock->errorMessage = calloc(1, sizeof(char));
    throwingBlock->allocations = hashtable_new(HT_INITIAL_CAPACITY, HT_LOAD_FACTOR);
    throwingBlock->nextId = START_ID;
    return throwingBlock;
}

void tb_throw(ThrowingBlock *throwingBlock, char *errorMessage) {
    requireNonNull(throwingBlock);
    requireNonNull(errorMessage);
    free(throwingBlock->errorMessage);
    throwingBlock->errorMessage = cloneString(errorMessage);
    longjmp(TB_ENV, ERROR_STATUS);
}

int tb_trace(ThrowingBlock *throwingBlock, Object *object) {
    requireNonNull(throwingBlock);
    requireNonNull(object);
    int id = throwingBlock->nextId++;
    Object *key = object_new(&TYPE_INT, cloneInt(id));
    Object *val = hashtable_put(throwingBlock->allocations, key, object);
    if (val != NULL) {
        errExit("illegal state");
    }
    return id;
}

void tb_untrace(ThrowingBlock *throwingBlock, int id) {
    requireNonNull(throwingBlock);
    Object *key = object_new(&TYPE_INT, cloneInt(id));
    Object *val = hashtable_remove(throwingBlock->allocations, key);
    object_dispose(key);
    if (val == NULL) {
        errExit("not tracing");
    }
    val->data = NULL;
    object_dispose(val);
}

void tb_dispose(ThrowingBlock *throwingBlock) {
    if (throwingBlock == NULL) {
        return;
    }
    free(throwingBlock->errorMessage);
    hashtable_dispose(throwingBlock->allocations);
    free(throwingBlock);
}
