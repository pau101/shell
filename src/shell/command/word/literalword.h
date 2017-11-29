#ifndef SHELL_LITERALWORD_H
#define SHELL_LITERALWORD_H

#include "../../../object/object.h"
#include "../../shtypes.h"
#include "../../../util/iostreams.h"
#include "word.h"

typedef struct literalWord {
    char *value;
} LiteralWord;

LiteralWord *literalword_new(char *value);

Word *literalword_word(char *value);

char *literalword_getValue(LiteralWord *literalWord, Shell *shell, IOStreams *streams);

char *literalword_toString(void *o);

unsigned int literalword_hashCode(void *o);

int literalword_compareTo(void *o1, void *o2);

void *literalword_clone(void *o);

void literalword_dispose(void *o);

static const DataType TYPE_LITERAL_WORD = {"LITERALWORD", literalword_toString, literalword_hashCode, literalword_compareTo,
                                         literalword_clone, literalword_dispose};

#endif //SHELL_LITERALWORD_H
