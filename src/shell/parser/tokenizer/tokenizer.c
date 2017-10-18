#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "tokenzier.h"
#include "../../../object/type/string.h"
#include "../../../util/util.h"
#include "../../../object/type/int.h"
#include "../../../util/preconditions.h"

Tokenizer *tokenizer_new() {
    return calloc(1, sizeof(Tokenizer));
}

Object *tokenizer_newString(char *in) {
    return object_new(&TYPE_STRING, newString(in));
}

bool tokenizer_read(FILE *input, int *c) {
    if ((*c = fgetc(input)) == EOF) {
        return true;
    }
    return false;
}

bool tokenizer_get(FILE *input, int *c) {
    if (tokenizer_read(input, c)) {
        return true;
    }
    if (*c == '\\') {
        int co = *c;
        if (tokenizer_read(input, c)) {
            return true;
        }
        if (*c == '\r' || *c == '\n') {
            if (tokenizer_read(input, c)) {
                return true;
            }
        } else {
            ungetc(*c, input);
            *c = co;
        }
    }
    return false;
}

Token *tokenizer_checkEOL(int c) {
    if (c == '\n' || c == '\r') {
        return token_new(TOK_EOL, tokenizer_newString(c == '\n' ? "\n" : "\r"));
    }
    return NULL;
}

Token *tokenzier_next(Tokenizer *tokenizer, FILE *input, Token *lastToken) {
    requireNonNull(tokenizer);
    int c;
    do {
        if (tokenizer_get(input, &c)) {
            return token_new(TOK_END, NULL);
        }
    } while (c == ' ' || c == '\t');
    Token *eol = tokenizer_checkEOL(c);
    if (eol != NULL) {
        return eol;
    }
    // TODO: trie for ugly switch
    switch (c) {
        case '#': {
            while (!tokenizer_read(input, &c)) {
                if ((eol = tokenizer_checkEOL(c)) != NULL) {
                    return eol;
                }
            }
        }
        case '|':
            return token_new(TOK_PIPE, tokenizer_newString("|"));
        case ';':
            return token_new(TOK_SEQUENCE, tokenizer_newString(";"));
        case '>': {
            if (tokenizer_get(input, &c)) {
                return token_new(TOK_OUTPUT, tokenizer_newString(">"));
            }
            switch (c) {
                case '|':
                    return token_new(TOK_OUTPUT_CLOBBER, tokenizer_newString(">|"));
                case '>':
                    return token_new(TOK_OUTPUT_APPEND, tokenizer_newString(">>"));
                case '&':
                    return token_new(TOK_OUTPUT_IO_NUMBER, tokenizer_newString(">&"));
                default:
                    ungetc(c, input);
                    return token_new(TOK_OUTPUT, tokenizer_newString(">"));
            }
        }
        case '<':
            return token_new(TOK_INPUT, tokenizer_newString("<"));
        case '(':
            return token_new(TOK_OPEN_PARENTHESES, tokenizer_newString("("));
        case ')':
            return token_new(TOK_CLOSE_PARENTHESES, tokenizer_newString(")"));
        default:
            break;
    }
    size_t size = 8;
    char *value = calloc(size, sizeof(char));
    int count = 0;
    bool inSingleQuote = false;
    bool inDoubleQuote = false;
    bool isNumeric = true;
    bool wasDelimited = true;
    do {
        bool isUnescaped = true;
        if (!inSingleQuote && c == '\\') {
            isUnescaped = false;
            int co = c;
            if (tokenizer_get(input, &c)) {
                wasDelimited = false;
                break;
            }
            if (inDoubleQuote && !strchr("\"\\\r\n", c)) {
                isUnescaped = true;
                ungetc(c, input);
                c = co;
            }
        }
        bool append = true;
        if (isUnescaped) {
            if (c == '\'') {
                inSingleQuote = !inSingleQuote;
                append = false;
            } else if (c == '"') {
                inDoubleQuote = !inDoubleQuote;
                append = false;
            }
        }
        if (append) {
            if (count >= size) {
                value = realloc(value, (size *= 2) * sizeof(char));
            }
            if (!isUnescaped || c < '0' || c > '9') {
                isNumeric = false;
            }
            value[count++] = (char) c;
        }
        if (tokenizer_get(input, &c)) {
            wasDelimited = false;
            break;
        }
    } while (inSingleQuote || inDoubleQuote || !strchr("|;><() \t\r\n", c));
    if (wasDelimited) {
        ungetc(c, input);
    }
    value = realloc(value, (count + 1) * sizeof(char));
    value[count] = '\0';
    if (count > 0 && isNumeric && (c == '>' || c == '<' || lastToken->type == TOK_OUTPUT_IO_NUMBER)) {
        long num = strtol(value, NULL, 10);
        if (num <= INT_MAX) {
            free(value);
            return token_new(TOK_IO_NUMBER, object_new(&TYPE_INT, newInt((int) num)));
        }
    }
    return token_new(TOK_WORD, object_new(&TYPE_STRING, value));
}

void tokenizer_dispose(Tokenizer *tokenizer) {
    if (tokenizer == NULL) {
        return;
    }
    free(tokenizer);
}
