#ifndef SHELL_THROWINGBLOCK_H
#define SHELL_THROWINGBLOCK_H

#include "setjmp.h"
#include "../collections/hashtable/hashtable.h"

#define tb_try(tb) (!setjmp(TB_ENV))

jmp_buf TB_ENV;

typedef struct throwingBlock {
    char *errorMessage;
    Hashtable *allocations;
    int nextId;
} ThrowingBlock;

ThrowingBlock *tb_new();

void tb_throw(ThrowingBlock *throwingBlock, char *errorMessage);

int tb_trace(ThrowingBlock *throwingBlock, Object *object);

void tb_untrace(ThrowingBlock *throwingBlock, int id);

void tb_dispose(ThrowingBlock *throwingBlock);

#endif //SHELL_THROWINGBLOCK_H
