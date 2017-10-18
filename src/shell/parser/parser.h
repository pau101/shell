#ifndef SHELL_PARSER_H
#define SHELL_PARSER_H

#include "../command/sequence.h"
#include "tokenizer/tokenzier.h"
#include "tokenizer/token.h"

typedef struct parser {
    Tokenizer *tokenizer;
    Token *token;
} Parser;

Parser *parser_new();

void parser_reset(Parser *parser);

bool parser_isInLine(Parser *parser);

bool parser_isAtLine(Parser *parser);

Executable *parser_parse(Parser *parser, FILE *input, FILE *output);

void parser_dispose(Parser *parser);

#endif //SHELL_PARSER_H
