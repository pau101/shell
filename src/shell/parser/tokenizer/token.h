#ifndef SHELL_TOKEN_H
#define SHELL_TOKEN_H

#include <stdio.h>
#include "../../../object/object.h"

#define TOKEN_COUNT 14

typedef enum tokenType {
    TOK_END,
    TOK_EOL,
    TOK_SEQUENCE,
    TOK_PIPE,
    TOK_INPUT,
    TOK_INPUT_IO_NUMBER,
    TOK_OUTPUT,
    TOK_OUTPUT_APPEND,
    TOK_OUTPUT_CLOBBER,
    TOK_OUTPUT_IO_NUMBER,
    TOK_IO_NUMBER,
    TOK_WORD,
    TOK_OPEN_PARENTHESES,
    TOK_CLOSE_PARENTHESES
} TokenType;

typedef struct token {
    TokenType type;
    Object *value;
} Token;

Token *token_new(TokenType type, Object *value);

const char *token_getName(Token *token);

void token_dispose(Token *token);

TokenType *cloneTokenType(TokenType in);

#endif //SHELL_TOKEN_H
