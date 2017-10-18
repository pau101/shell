#include <stdlib.h>
#include "token.h"
#include "../../../util/preconditions.h"

static const char *TOKEN_NAMES[TOKEN_COUNT] = {
        "END",
        "EOL",
        "SEQUENCE",
        "PIPE",
        "INPUT",
        "INPUT_IO_NUMBER",
        "OUTPUT",
        "OUTPUT_APPEND",
        "OUTPUT_CLOBBER",
        "OUTPUT_IO_NUMBER",
        "IO_NUMBER",
        "WORD",
        "OPEN_PARENTHESES",
        "TOK_CLOSE_PARENTHESES"
};

Token *token_new(TokenType type, Object *value) {
    requireNonNull(value);
    Token *token = calloc(1, sizeof(Token));
    token->type = type;
    token->value = value;
    return token;
}

const char *token_getName(Token *token) {
    requireNonNull(token);
    return TOKEN_NAMES[token->type];
}

void token_dispose(Token *token) {
    if (token == NULL) {
        return;
    }
    object_dispose(token->value);
    free(token);
}
