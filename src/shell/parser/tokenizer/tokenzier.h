#ifndef SHELL_TOKENZIER_H
#define SHELL_TOKENZIER_H

#include <stdbool.h>
#include "token.h"
#include "../../shtypes.h"

typedef struct tokenizer {
    int readCharCount;
    int readCharSkip;
    size_t readCharsSize;
    char *readChars;
} Tokenizer;

Tokenizer *tokenizer_new();

Token *tokenizer_next(Tokenizer *tokenizer, Shell *shell, FILE *input, Token *lastToken);

void tokenizer_beginRead(Tokenizer *tokenizer);

char *tokenizer_endRead(Tokenizer *tokenizer);

void tokenizer_dispose(Tokenizer *tokenizer);

#endif //SHELL_TOKENZIER_H
