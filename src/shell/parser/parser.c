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

Parser *parser_new() {
    Parser *parser = calloc(1, sizeof(Parser));
    parser->tokenizer = tokenizer_new();
    parser->token = token_new(TOK_EOL, object_new(&TYPE_STRING, cloneString("\n")));
    return parser;
}

bool parser_isInLine(Parser *parser) {
    return parser->token->type != TOK_EOL && parser->token->type != TOK_END;
}

bool parser_isAtLine(Parser *parser) {
    return parser->token->type == TOK_EOL;
}

Token *parser_nextToken(Parser *parser, FILE *input) {
    Token *token = tokenzier_next(parser->tokenizer, input, parser->token);
    token_dispose(parser->token);
    parser->token = token;
    return token;
}

bool parser_parseIORedirection(Parser *parser, FILE *input, ThrowingBlock *tb, Command *command, int fdin, int fdout) {
    int outputType = 0;
    switch (parser->token->type) {
        case TOK_INPUT_IO_NUMBER: {
            Token *token = parser_nextToken(parser, input);
            if (token->type == TOK_IO_NUMBER) {
                int fd = *(int *) object_get(token->value, &TYPE_INT);
                command_addRedirect(command, redirect_new(fdin, fdopener_handle(fd, cloneString("r"))));
            } else {
                tb_throw(tb, "syntax error");
            }
            return true;
        }
        case TOK_INPUT: {
            Token *token = parser_nextToken(parser, input);
            if (token->type == TOK_WORD) {
                char *word = (char *) object_get(token->value, &TYPE_STRING);
                command_addRedirect(command,
                                    redirect_new(fdin, filenameopener_handle(cloneString(word), cloneString("r"))));
            } else {
                tb_throw(tb, "syntax error");
            }
            return true;
        }
        case TOK_OUTPUT_IO_NUMBER: {
            Token *token = parser_nextToken(parser, input);
            if (token->type == TOK_IO_NUMBER) {
                int fd = *(int *) object_get(token->value, &TYPE_INT);
                command_addRedirect(command, redirect_new(fdout, fdopener_handle(fd, cloneString("w"))));
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
            Token *token = parser_nextToken(parser, input);
            if (token->type == TOK_WORD) {
                char *word = (char *) object_get(token->value, &TYPE_STRING);
                char *mode = outputType == 1 ? "a" : "w";
                command_addRedirect(command,
                                    redirect_new(fdout, filenameopener_handle(cloneString(word), cloneString(mode))));
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

bool parser_parseRedirection(Parser *parser, FILE *input, ThrowingBlock *tb, Command *command) {
    if (parser_parseIORedirection(parser, input, tb, command, STDIN_FILENO, STDOUT_FILENO)) {
        return true;
    }
    if (parser->token->type == TOK_IO_NUMBER) {
        int num = *(int *) object_get(parser->token->value, &TYPE_INT);
        parser_nextToken(parser, input);
        if (parser_parseIORedirection(parser, input, tb, command, num, num)) {
            return true;
        }
        tb_throw(tb, "syntax error");
    }
    return false;
}

ExecutableBuilder *parser_continuePipeline(ExecutableBuilder *builder, ThrowingBlock *tb, ExecutableBuilder *top) {
    Pipeline *pipeline = execbldr_getExecutor(builder, &TYPE_PIPELINE);
    if (top->executable != NULL) {
        printf("continue pipe\n");
        pipeline_add(pipeline, top->executable);
        top->executable = NULL;
    }
    return builder;
}

ExecutableBuilder *parser_continueSequence(ExecutableBuilder *builder, ThrowingBlock *tb, ExecutableBuilder *top) {
    Sequence *sequence = execbldr_getExecutor(builder, &TYPE_SEQUENCE);
    if (top->executable != NULL) {
        printf("continue sequence\n");
        sequence_add(sequence, top->executable);
        top->executable = NULL;
    }
    return builder;
}

ExecutableBuilder *parser_newSequence(ExecutableBuilder *builder, ThrowingBlock *tb, ExecutableBuilder *top);

ExecutableBuilder *parser_newPipeline(ExecutableBuilder *builder, ThrowingBlock *tb, ExecutableBuilder *top) {
    Pipeline *pipeline = pipeline_new();
    printf("new pipeline\n");
    if (builder->executable != NULL) {
        printf("1 %s\n", builder->executable->executor->dataType->identifier);
        pipeline_add(pipeline, builder->executable);
        builder->executable = NULL;
    }
    execbldr_dispose(builder);
    if (top->executable != NULL) {
        printf("2 %s\n", top->executable->executor->dataType->identifier);
        pipeline_add(pipeline, top->executable);
        top->executable = NULL;
    }
    return execbldr_new(pipeline_executable(pipeline), parser_continuePipeline, parser_newSequence, parser_continuePipeline);
}

ExecutableBuilder *parser_newSequence(ExecutableBuilder *builder, ThrowingBlock *tb, ExecutableBuilder *top) {
    Sequence *sequence = sequence_new();
    printf("new sequence\n");
    if (builder->executable != NULL) {
        printf("1 %s\n", builder->executable->executor->dataType->identifier);
        sequence_add(sequence, builder->executable);
        builder->executable = NULL;
    }
    execbldr_dispose(builder);
    if (top->executable != NULL) {
        printf("1 %s\n", top->executable->executor->dataType->identifier);
        sequence_add(sequence, top->executable);
        top->executable = NULL;
    }
    return execbldr_new(sequence_executable(sequence), parser_newPipeline, parser_continueSequence, parser_continueSequence);
}

ExecutableBuilder *parser_bldrError(ExecutableBuilder *builder, ThrowingBlock *tb, ExecutableBuilder *top) {
    tb_throw(tb, "syntax error");
}

ExecutableBuilder *parser_bldrEnd(ExecutableBuilder *builder, ThrowingBlock *tb, ExecutableBuilder *top) {
    if (builder->executable != NULL) {
        tb_throw(tb, "syntax error");
    }
    execbldr_dispose(builder);
    ExecutableBuilder *bob = execbldr_new(top->executable, parser_bldrError, parser_bldrError, parser_bldrError);
    top->executable = NULL;
    return bob;
}

ExecutableBuilder *parser_parseCommand(Parser *parser, FILE *input, ThrowingBlock *tb) {
    Command *command = command_new();
    int cid = tb_trace(tb, object_new(&TYPE_COMMAND, command));
    bool isEmpty = true;
    do {
        while (parser_nextToken(parser, input)->type == TOK_WORD) {
            isEmpty = false;
            command_addWord(command, cloneString(object_get(parser->token->value, &TYPE_STRING)));
        }
    } while (parser_parseRedirection(parser, input, tb, command));
    tb_untrace(tb, cid);
    if (isEmpty) {
        command_dispose(command);
        return execbldr_new(NULL, parser_bldrError, parser_bldrError, parser_bldrError);
    }
    return execbldr_new(command_executable(command), parser_newPipeline, parser_newSequence, parser_bldrError);
}

Executable *parser_parse(Parser *parser, FILE *input, FILE *output) {
    ThrowingBlock *tb = tb_new();
    LinkedList *stack = list_new();
    list_addLast(stack, object_new(&TYPE_EXECUTABLE_BUILDER, execbldr_new(NULL, parser_newPipeline, parser_newSequence, parser_bldrEnd)));
    Executable *executable = NULL;
    if (tb_try(tb)) {
        do {
            ExecutableBuilder *command = parser_parseCommand(parser, input, tb);
            ExecutableBuilder *(*begin)(ExecutableBuilder *, ThrowingBlock *, ExecutableBuilder *);
            TokenType type = parser->token->type;
            if (type == TOK_PIPE) {
                begin = execbldr_beginPipeline;
            } else if (type == TOK_SEQUENCE) {
                begin = execbldr_beginSequence;
            } else {
                begin = execbldr_beginEnd;
            }
            ExecutableBuilder *builder = object_getAndDispose(list_removeLast(stack), &TYPE_EXECUTABLE_BUILDER);
            int cid = tb_trace(tb, object_new(&TYPE_EXECUTABLE_BUILDER, command));
            int bid = tb_trace(tb, object_new(&TYPE_EXECUTABLE_BUILDER, builder));
            ExecutableBuilder *top = begin(builder, tb, command);
            tb_untrace(tb, cid);
            tb_untrace(tb, bid);
            execbldr_dispose(command);
            list_addLast(stack, object_new(&TYPE_EXECUTABLE_BUILDER, top));
        } while (parser_isInLine(parser));
        if (list_isSingular(stack)) {
            ExecutableBuilder *builder = object_get(list_single(stack), &TYPE_EXECUTABLE_BUILDER);
            executable = builder->executable;
            builder->executable = NULL;
        } else {
            tb_throw(tb, "syntax error");
        }
    } else {
        fprintf(output, "%s\n", tb->errorMessage);
        while (parser_isInLine(parser)) {
            parser_nextToken(parser, input);
        }
    }
    tb_dispose(tb);
    list_dispose(stack);
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
