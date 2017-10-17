#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "parser.h"
#include "../command/redirect/opener/filenameopener.h"
#include "../../object/type/string.h"
#include "../../object/type/int.h"
#include "../../util/throwingblock.h"

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

Command *parser_parseCommand(Parser *parser, FILE *input, ThrowingBlock *tb) {
    Command *command = command_new();
    int cid = tb_trace(tb, object_new(&TYPE_COMMAND, command));
    do {
        while (parser_nextToken(parser, input)->type == TOK_WORD) {
            command_addWord(command, cloneString(object_get(parser->token->value, &TYPE_STRING)));
        }
    } while (parser_parseRedirection(parser, input, tb, command));
    tb_untrace(tb, cid);
    return command;
}

Pipeline *parser_parsePipeline(Parser *parser, FILE *input, ThrowingBlock *tb) {
    Pipeline *pipeline = pipeline_new();
    int pid = tb_trace(tb, object_new(&TYPE_PIPELINE, pipeline));
    do {
        pipeline_add(pipeline, command_executable(parser_parseCommand(parser, input, tb)));
    } while (parser->token->type == TOK_PIPE);
    tb_untrace(tb, pid);
    return pipeline;
}

Executable *parser_parse(Parser *parser, FILE *input, FILE *output) {
    ThrowingBlock *tb = tb_new();
    Executable *executable;
    if (tb_try(tb)) {
        Sequence *sequence = sequence_new();
        int sid = tb_trace(tb, object_new(&TYPE_SEQUENCE, sequence));
        do {
            sequence_add(sequence, pipeline_executable(parser_parsePipeline(parser, input, tb)));
        } while (parser->token->type == TOK_SEQUENCE);
        if (parser_isInLine(parser)) {
            tb_throw(tb, "syntax error");
        }
        tb_untrace(tb, sid);
        executable = sequence_executable(sequence);
    } else {
        executable = NULL;
        fprintf(output, "%s\n", tb->errorMessage);
        while (parser_isInLine(parser)) {
            parser_nextToken(parser, input);
        }
    }
    tb_dispose(tb);
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
