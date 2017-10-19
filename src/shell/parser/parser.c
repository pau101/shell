#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "parser.h"
#include "../command/redirect/opener/filenameopener.h"
#include "../../object/type/string.h"
#include "../../object/type/int.h"
#include "../../util/throwingblock.h"
#include "tokenizer/token.h"
#include "tokenizer/tokenzier.h"
#include "executablebuilder.h"
#include "../../object/type/reference.h"
#include "../../util/preconditions.h"

Token *newInitToken() {
    return token_new(TOK_EOL, object_new(&TYPE_STRING, newString("\n")));
}

Parser *parser_new() {
    Parser *parser = calloc(1, sizeof(Parser));
    parser->tokenizer = tokenizer_new();
    parser->token = newInitToken();
    return parser;
}

void parser_reset(Parser *parser) {
    requireNonNull(parser);
    token_dispose(parser->token);
    parser->token = newInitToken();
}

bool parser_isInLine(Parser *parser) {
    requireNonNull(parser);
    return parser->token->type != TOK_EOL && parser->token->type != TOK_END;
}

bool parser_isAtLine(Parser *parser) {
    requireNonNull(parser);
    return parser->token->type == TOK_EOL;
}

Token *parser_nextToken(Parser *parser, Shell *shell, FILE *input) {
    Token *token = tokenizer_next(parser->tokenizer, shell, input, parser->token);
    token_dispose(parser->token);
    parser->token = token;
    return token;
}

bool parser_parseIORedirection(Parser *parser, Shell *shell, FILE *input, ThrowingBlock *tb, Command *command, int fdin, int fdout) {
    int outputType = 0;
    switch (parser->token->type) {
        case TOK_INPUT_IO_NUMBER: {
            Token *token = parser_nextToken(parser, shell, input);
            if (token->type == TOK_IO_NUMBER) {
                int fd = *(int *) object_get(token->value, &TYPE_INT);
                command_addRedirect(command, redirect_new(fdin, fdopener_handle(fd, newString("r"))));
            } else {
                tb_throw(tb, "syntax error");
            }
            return true;
        }
        case TOK_INPUT: {
            Token *token = parser_nextToken(parser, shell, input);
            if (token->type == TOK_WORD) {
                char *word = (char *) object_get(token->value, &TYPE_STRING);
                command_addRedirect(command,
                                    redirect_new(fdin, filenameopener_handle(newString(word), newString("r"))));
            } else {
                tb_throw(tb, "syntax error");
            }
            return true;
        }
        case TOK_OUTPUT_IO_NUMBER: {
            Token *token = parser_nextToken(parser, shell, input);
            if (token->type == TOK_IO_NUMBER) {
                int fd = *(int *) object_get(token->value, &TYPE_INT);
                command_addRedirect(command, redirect_new(fdout, fdopener_handle(fd, newString("w"))));
            } else {
                tb_throw(tb, "syntax error");
            }
            return true;
        }
        case TOK_OUTPUT_CLOBBER: // TODO
            outputType++;
        case TOK_OUTPUT_APPEND:
            outputType++;
        case TOK_OUTPUT: {
            Token *token = parser_nextToken(parser, shell, input);
            if (token->type == TOK_WORD) {
                char *word = (char *) object_get(token->value, &TYPE_STRING);
                char *mode = outputType == 1 ? "a" : "w";
                command_addRedirect(command,
                                    redirect_new(fdout, filenameopener_handle(newString(word), newString(mode))));
            } else {
                tb_throw(tb, "syntax error");
            }
            return true;
        }
        default:
            break;
    }
    return false;
}

bool parser_parseRedirection(Parser *parser, Shell *shell, FILE *input, ThrowingBlock *tb, Command *command) {
    if (parser_parseIORedirection(parser, shell, input, tb, command, STDIN_FILENO, STDOUT_FILENO)) {
        return true;
    }
    if (parser->token->type == TOK_IO_NUMBER) {
        int num = *(int *) object_get(parser->token->value, &TYPE_INT);
        parser_nextToken(parser, shell, input);
        if (parser_parseIORedirection(parser, shell, input, tb, command, num, num)) {
            return true;
        }
        tb_throw(tb, "syntax error");
    }
    return false;
}

ExecutableBuilder *parser_continuePipeline(ExecutableBuilder *builder, ThrowingBlock *tb, ExecutableBuilder *top) {
    Pipeline *pipeline = execbldr_getExecutor(builder, &TYPE_PIPELINE);
    if (top->executable != NULL) {
        pipeline_add(pipeline, top->executable);
        top->executable = NULL;
    }
    return builder;
}

ExecutableBuilder *parser_continueSequence(ExecutableBuilder *builder, ThrowingBlock *tb, ExecutableBuilder *top) {
    Sequence *sequence = execbldr_getExecutor(builder, &TYPE_SEQUENCE);
    if (top->executable != NULL) {
        sequence_add(sequence, top->executable);
        top->executable = NULL;
    }
    return builder;
}

ExecutableBuilder *parser_newSequence(ExecutableBuilder *builder, ThrowingBlock *tb, ExecutableBuilder *top);

ExecutableBuilder *parser_newPipeline(ExecutableBuilder *builder, ThrowingBlock *tb, ExecutableBuilder *top) {
    Pipeline *pipeline = pipeline_new();
    if (builder->executable != NULL) {
        pipeline_add(pipeline, builder->executable);
        builder->executable = NULL;
    }
    execbldr_dispose(builder);
    if (top->executable != NULL) {
        pipeline_add(pipeline, top->executable);
        top->executable = NULL;
    }
    return execbldr_new(pipeline_executable(pipeline, newString("")), parser_continuePipeline, parser_newSequence, parser_continuePipeline);
}

ExecutableBuilder *parser_newSequence(ExecutableBuilder *builder, ThrowingBlock *tb, ExecutableBuilder *top) {
    Sequence *sequence = sequence_new();
    if (builder->executable != NULL) {
        sequence_add(sequence, builder->executable);
        builder->executable = NULL;
    }
    execbldr_dispose(builder);
    if (top->executable != NULL) {
        sequence_add(sequence, top->executable);
        top->executable = NULL;
    }
    return execbldr_new(sequence_executable(sequence, newString("")), parser_newPipeline, parser_continueSequence, parser_continueSequence);
}

ExecutableBuilder *parser_bldrError(ExecutableBuilder *builder, ThrowingBlock *tb, ExecutableBuilder *top) {
    tb_throw(tb, "syntax error");
}

ExecutableBuilder *parser_parseCommand(Parser *parser, Shell *shell, FILE *input, ThrowingBlock *tb) {
    Command *command = command_new();
    int cid = tb_trace(tb, object_new(&TYPE_COMMAND, command));
    bool isEmpty = true;
    bool isFirst = true;
    do {
        while (parser_nextToken(parser, shell, input)->type == TOK_WORD) {
            char *word = object_get(parser->token->value, &TYPE_STRING);
            bool isAlias = false;
            if (isFirst) {
                isFirst = false;
                char *alias = shell_getAlias(shell, word);
                if (alias != NULL) {
                    isAlias = true;
                    for (int i = (int) strlen(alias) - 1; i >= 0; i--) {
                        ungetc(alias[i], input);
                        parser->tokenizer->readCharSkip++;
                    }
                }
            }
            if (!isAlias) {
                isEmpty = false;
                command_addWord(command, newString(word));
            }
        }
    } while (parser_parseRedirection(parser, shell, input, tb, command));
    tb_untrace(tb, cid);
    if (isEmpty) {
        command_dispose(command);
        return NULL;
    }
    return execbldr_new(command_executable(command, newString("")), parser_newPipeline, parser_newSequence, parser_bldrError);
}

void parser_merge(ThrowingBlock *tb, LinkedList *postfix, TokenType type) {
    ExecutableBuilder *(*begin)(ExecutableBuilder *, ThrowingBlock *, ExecutableBuilder *) = NULL;
    if (type == TOK_PIPE) {
        begin = execbldr_beginPipeline;
    } else if (type == TOK_SEQUENCE) {
        begin = execbldr_beginSequence;
    } else {
        tb_throw(tb, "syntax error");
    }
    if (postfix->size < 2) {
        tb_throw(tb, "syntax error");
    }
    ExecutableBuilder *second = object_getAndDispose(list_removeLast(postfix), &TYPE_EXECUTABLE_BUILDER);
    ExecutableBuilder *first = object_getAndDispose(list_removeLast(postfix), &TYPE_EXECUTABLE_BUILDER);
    int fid = tb_trace(tb, object_new(&TYPE_EXECUTABLE_BUILDER, first));
    int sid = tb_trace(tb, object_new(&TYPE_EXECUTABLE_BUILDER, second));
    ExecutableBuilder *top = begin(first, tb, second);
    tb_untrace(tb, fid);
    tb_untrace(tb, sid);
    execbldr_dispose(second);
    list_addLast(postfix, object_new(&TYPE_EXECUTABLE_BUILDER, top));
}

Executable *parser_parse(Parser *parser, Shell *shell, FILE *input, FILE *output) {
    requireNonNull(parser);
    requireNonNull(input);
    requireNonNull(output);
    ThrowingBlock *tb = tb_new();
    LinkedList *operators = list_new();
    LinkedList *builders = list_new();
    Executable *executable = NULL;
    if (tb_try(tb)) {
        tokenizer_beginRead(parser->tokenizer);
        do {
            ExecutableBuilder *command = parser_parseCommand(parser, shell, input, tb);
            if (command != NULL) {
                list_addLast(builders, object_new(&TYPE_EXECUTABLE_BUILDER, command));
            }
            TokenType type = parser->token->type;
            if (type == TOK_PIPE || type == TOK_SEQUENCE) {
                while (!list_isEmpty(operators)) {
                    Object *op = list_peekLast(operators);
                    if (op == NULL || *(TokenType *) object_get(op, &TYPE_REFERENCE) < type) {
                        break;
                    }
                    TokenType *tp = object_getAndDispose(list_removeLast(operators), &TYPE_REFERENCE);
                    parser_merge(tb, builders, *tp);
                    free(tp);
                }
                list_addLast(operators, object_new(&TYPE_REFERENCE, cloneTokenType(type)));
            } else if (type == TOK_OPEN_PARENTHESES) {
                list_addLast(operators, NULL);
            } else if (type == TOK_CLOSE_PARENTHESES) {
                while (true) {
                    if (list_isEmpty(operators)) {
                        tb_throw(tb, "syntax error: missing open parentheses");
                    }
                    TokenType *tp = object_getAndDispose(list_removeLast(operators), &TYPE_REFERENCE);
                    if (tp == NULL) {
                        break;
                    }
                    parser_merge(tb, builders, *tp);
                    free(tp);
                }
            }
        } while (parser_isInLine(parser));
        char *source = tokenizer_endRead(parser->tokenizer);
        int sid = tb_trace(tb, object_new(&TYPE_STRING, source));
        while (!list_isEmpty(operators)) {
            TokenType *tp = object_getAndDispose(list_removeLast(operators), &TYPE_REFERENCE);
            if (tp == NULL) {
                tb_throw(tb, "syntax error: missing closed parentheses");
            } else {
                parser_merge(tb, builders, *tp);
                free(tp);
            }
        }
        if (list_isSingular(builders)) {
            ExecutableBuilder *builder = object_get(list_single(builders), &TYPE_EXECUTABLE_BUILDER);
            executable = builder->executable;
            builder->executable = NULL;
            free(executable->source);
            executable->source = source;
            tb_untrace(tb, sid);
        } else if (!list_isEmpty(builders)) {
            tb_throw(tb, "syntax error");
        }
    } else {
        fprintf(output, "%s\n", tb->errorMessage);
        while (parser_isInLine(parser)) {
            parser_nextToken(parser, shell, input);
        }
    }
    tb_dispose(tb);
    list_dispose(builders);
    list_dispose(operators);
    return executable;
}

void parser_dispose(Parser *parser) {
    if (parser == NULL) {
        return;
    }
    tokenizer_dispose(parser->tokenizer);
    token_dispose(parser->token);
    free(parser);
}
