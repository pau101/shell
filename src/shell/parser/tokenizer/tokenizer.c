#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "tokenzier.h"
#include "../../../object/type/string.h"
#include "../../../util/util.h"
#include "../../../object/type/int.h"
#include "../../../util/preconditions.h"
#include "../../command/command.h"
#include "../../shell.h"

Tokenizer *tokenizer_new() {
    Tokenizer *tokenizer = calloc(1, sizeof(Tokenizer));
    tokenizer->readCharCount = 0;
    tokenizer->readCharsSize = 8;
    tokenizer->readChars = calloc(tokenizer->readCharsSize, sizeof(char));
    return tokenizer;
}

Object *tokenizer_newString(char *in) {
    return object_new(&TYPE_STRING, newString(in));
}

void tokenizer_appendRead(Tokenizer *tokenizer, char c) {
    if (tokenizer->readCharSkip > 0) {
        tokenizer->readCharSkip--;
    } else {
        if (tokenizer->readCharCount >= tokenizer->readCharsSize) {
            tokenizer->readCharsSize = tokenizer->readCharsSize * 2 + 1;
            tokenizer->readChars = realloc(tokenizer->readChars, tokenizer->readCharsSize * sizeof(char));
        }
        tokenizer->readChars[tokenizer->readCharCount++] = c;
    }
}

void tokenizer_discard(Tokenizer *tokenizer) {
    if (tokenizer->readCharCount <= 0) {
        errExit("illegal state");
    } else {
        tokenizer->readCharCount--;
    }
}

void tokenizer_unread(Tokenizer *tokenizer, FILE *input, int *c, int co) {
    if (tokenizer->readCharSkip == 0) {
        ungetc(*c, input);
        tokenizer_discard(tokenizer);
    }
    *c = co;
}

bool tokenizer_read(Tokenizer *tokenizer, FILE *input, int *c) {
    if ((*c = fgetc(input)) == EOF) {
        return true;
    }
    return false;
}

bool tokenizer_get(Tokenizer *tokenizer, FILE *input, int *c) {
    if (tokenizer_read(tokenizer, input, c)) {
        return true;
    }
    if (*c == '\\') {
        int co = *c;
        if (tokenizer_read(tokenizer, input, c)) {
            return true;
        }
        if (*c == '\r' || *c == '\n') {
            if (tokenizer_read(tokenizer, input, c)) {
                return true;
            }
        } else {
            tokenizer_unread(tokenizer, input, c, co);
        }
    }
    tokenizer_appendRead(tokenizer, (char) *c);
    return false;
}

Token *tokenizer_checkEOL(int c) {
    if (c == '\n' || c == '\r') {
        return token_new(TOK_EOL, tokenizer_newString(c == '\n' ? "\n" : "\r"));
    }
    return NULL;
}

Token *tokenizer_next(Tokenizer *tokenizer, Shell *shell, FILE *input, Token *lastToken) {
    requireNonNull(tokenizer);
    int c;
    do {
        if (tokenizer_get(tokenizer, input, &c)) {
            return token_new(TOK_END, NULL);
        }
    } while (c == ' ' || c == '\t');
    Token *eol = tokenizer_checkEOL(c);
    if (eol != NULL) {
        tokenizer_discard(tokenizer);
        return eol;
    }
    // TODO: trie for ugly switch
    switch (c) {
        case '#': {
            while (!tokenizer_read(tokenizer, input, &c)) {
                if ((eol = tokenizer_checkEOL(c)) != NULL) {
                    return eol;
                }
                tokenizer_appendRead(tokenizer, (char) c);
            }
        }
        case '|':
            return token_new(TOK_PIPE, tokenizer_newString("|"));
        case ';':
            return token_new(TOK_SEQUENCE, tokenizer_newString(";"));
        case '>': {
            if (tokenizer_get(tokenizer, input, &c)) {
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
                    tokenizer_unread(tokenizer, input, &c, 0);
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
            if (tokenizer_get(tokenizer, input, &c)) {
                wasDelimited = false;
                break;
            }
            if (inDoubleQuote && !strchr("\"\\\r\n", c)) {
                isUnescaped = true;
                tokenizer_unread(tokenizer, input, &c, co);
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
            } else if (c == '!') {
                int co = c;
                if (tokenizer_get(tokenizer, input, &c)) {
                    break;
                }
                append = false;
                if (c == '!') {
                    LinkedList *history = shell->history;
                    if (!list_isEmpty(history)) {
                        char *lc = ((Executable *) object_get(list_peekLast(history), &TYPE_EXECUTABLE))->source;
                        for (int i = (int) strlen(lc) - 1; i >= 0; i--) {
                            ungetc(lc[i], input);
                            tokenizer->readCharSkip++;
                        }
                    }
                } else {
                    tokenizer_unread(tokenizer, input, &c, co);
                    int num = 0;
                    if (fscanf(input, "%d", &num) && num >= 0 && num < shell->history->size) {
                        char *lc = ((Executable *) object_get(list_get(shell->history, num), &TYPE_EXECUTABLE))->source;
                        for (int i = (int) strlen(lc) - 1; i >= 0; i--) {
                            ungetc(lc[i], input);
                            tokenizer->readCharSkip++;
                        }
                    }
                }
            }
        }
        if (append) {
            if (count >= size) {
                value = realloc(value, (size = size * 2 + 1) * sizeof(char));
            }
            if (!isUnescaped || c < '0' || c > '9') {
                isNumeric = false;
            }
            value[count++] = (char) c;
        }
        if (tokenizer_get(tokenizer, input, &c)) {
            wasDelimited = false;
            break;
        }
    } while (inSingleQuote || inDoubleQuote || !strchr("|;><() \t\r\n", c));
    if (wasDelimited) {
        tokenizer_unread(tokenizer, input, &c, c);
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

void tokenizer_beginRead(Tokenizer *tokenizer) {
    if (tokenizer->readCharCount > 0) {
        tokenizer->readCharSkip = 0;
        tokenizer->readCharCount = 0;
        tokenizer->readCharsSize = 8;
        tokenizer->readChars = realloc(tokenizer->readChars, tokenizer->readCharsSize * sizeof(char));
    }
}

char *tokenizer_endRead(Tokenizer *tokenizer) {
    char *chars = tokenizer->readChars;
    chars = realloc(chars, (tokenizer->readCharCount + 1) * sizeof(char));
    chars[tokenizer->readCharCount] = '\0';
    tokenizer->readCharCount = 0;
    tokenizer->readCharsSize = 8;
    tokenizer->readChars = calloc(tokenizer->readCharsSize, sizeof(char));
    return chars;
}

void tokenizer_dispose(Tokenizer *tokenizer) {
    if (tokenizer == NULL) {
        return;
    }
    free(tokenizer->readChars);
    free(tokenizer);
}
