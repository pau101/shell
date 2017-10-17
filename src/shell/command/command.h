#ifndef SHELL_COMMAND_H
#define SHELL_COMMAND_H

#include <stdbool.h>
#include "redirect/redirect.h"
#include "../../collections/linkedlist/linkedlist.h"
#include "../executable/executable.h"

typedef struct command {
    LinkedList *words;
    LinkedList *redirects;
} Command;

Command *command_new();

Executable *command_executable(Command *command);

void command_addWord(Command *command, char *word);

void command_addRedirect(Command *command, Redirect *redirect);

int command_exec(Command *command);

char *command_toString(void *o);

unsigned int command_hashCode(void *o);

int command_compareTo(void *o1, void *o2);

void command_dispose(void *o);

static const DataType TYPE_COMMAND = {"COMMAND", command_toString, command_hashCode, command_compareTo,
                                      command_dispose};

#endif //SHELL_COMMAND_H
