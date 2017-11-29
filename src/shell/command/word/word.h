#ifndef SHELL_WORD_H
#define SHELL_WORD_H

#include "../../shtypes.h"
#include "../../../util/iostreams.h"
#include "../../../object/object.h"

typedef struct word {
    Object *getter;

    char *(*getValue)(Object *getter, Shell *shell, IOStreams *streams);
} Word;

Word *word_new(Object *getter, char *(*getValue)(Object *g, Shell *shell, IOStreams *streams));

char *word_getValue(Word *word, Shell *shell, IOStreams *streams);

char *word_toString(void *o);

unsigned int word_hashCode(void *o);

int word_compareTo(void *o1, void *o2);

void *word_clone(void *o);

void word_dispose(void *o);

static const DataType TYPE_WORD = {"WORD", word_toString, word_hashCode, word_compareTo,
                                           word_clone, word_dispose};

#endif //SHELL_WORD_H
