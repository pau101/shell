#ifndef SHELL_WORDARRAY_H
#define SHELL_WORDARRAY_H

#include "../../../object/datatype.h"
#include "../../shtypes.h"
#include "../../../util/iostreams.h"
#include "word.h"
#include "../../../collections/linkedlist/linkedlist.h"

typedef struct wordArray {
    LinkedList *words;
} WordArray;

WordArray *wordarray_new();

Word *wordarray_word(WordArray *wordArray);

char *wordarray_getValue(WordArray *wordArray, Shell *shell, IOStreams *streams);

char *wordarray_toString(void *o);

unsigned int wordarray_hashCode(void *o);

int wordarray_compareTo(void *o1, void *o2);

void *wordarray_clone(void *o);

void wordarray_dispose(void *o);

static const DataType TYPE_WORD_ARRAY = {"WORDARRAY", wordarray_toString, wordarray_hashCode, wordarray_compareTo,
                                           wordarray_clone, wordarray_dispose};

#endif //SHELL_WORDARRAY_H
