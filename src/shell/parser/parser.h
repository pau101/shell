#ifndef SHELL_PARSER_H
#define SHELL_PARSER_H

#include "token.h"
#include "tokenzier.h"
#include "../command/sequence.h"

typedef struct parser {
    Tokenizer *tokenizer;
    Token *token;
} Parser;

Parser *parser_new();

bool parser_isInLine(Parser *parser);

bool parser_isAtLine(Parser *parser);

Sequence *parser_parse(Parser *parser, FILE *input, FILE *output);

void parser_dispose(Parser *parser);

#endif //SHELL_PARSER_H
