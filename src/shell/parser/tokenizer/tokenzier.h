#ifndef SHELL_TOKENZIER_H
#define SHELL_TOKENZIER_H

#include <stdbool.h>
#include "token.h"

typedef struct tokenizer {
} Tokenizer;

Tokenizer *tokenizer_new();

Token *tokenzier_next(Tokenizer *tokenizer, FILE *input, Token *lastToken);

void tokenizer_dispose(Tokenizer *tokenizer);

#endif //SHELL_TOKENZIER_H
