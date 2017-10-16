#include <stdlib.h>
#include <unistd.h>
#include "command.h"
#include "../../util/preconditions.h"
#include "../../object/type/string.h"
#include "../../object/type/reference.h"

Command *command_new() {
    Command *command = calloc(1, sizeof(Command));
    command->words = list_new();
    command->redirects = list_new();
    return command;
}

void command_addWord(Command *command, char *word) {
    list_addLast(command->words, object_new(&TYPE_STRING, word));
}

void command_addRedirect(Command *command, Redirect *redirect) {
    list_addLast(command->redirects, object_new(&TYPE_REDIRECT, redirect));
}

void command_exec(Command *command) {
    char **words = calloc((size_t) command->words->size + 1, sizeof(char *));
    Iterator *wIter = list_iterator(command->words);
    for (int i = 0; iterator_hasNext(wIter); i++) {
        words[i] = object_get(iterator_next(wIter), &TYPE_STRING);
    }
    iterator_dispose(wIter);
    Iterator *rIter = list_iterator(command->redirects);
    while (iterator_hasNext(rIter)) {
        redirect_perform(object_get(iterator_next(wIter), &TYPE_REDIRECT));
    }
    iterator_dispose(rIter);
    execvp(words[0], words); // TODO words leak
    pExit(words[0]);
}

char *command_toString(void *o) {
    return reference_toString(o);
}

unsigned int command_hashCode(void *o) {
    return reference_hashCode(o);
}

int command_compareTo(void *o1, void *o2) {
    return reference_compareTo(o1, o2);
}

void command_dispose(void *o) {
    if (o == NULL) {
        return;
    }
    list_dispose(((Command *) o)->words);
    list_dispose(((Command *) o)->redirects);
    free(o);
}