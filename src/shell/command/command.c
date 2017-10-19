#include <stdlib.h>
#include <unistd.h>
#include <wait.h>
#include "command.h"
#include "../../util/preconditions.h"
#include "../../object/type/string.h"
#include "../../object/type/reference.h"
#include "../shell.h"

Command *command_new() {
    Command *command = calloc(1, sizeof(Command));
    command->words = list_new();
    command->redirects = list_new();
    return command;
}

int command_exec_(Object *e, Shell *shell, IOStreams *streams) {
    return command_exec(object_get(e, &TYPE_COMMAND), shell, streams);
}

Executable *command_executable(Command *command, char *source) {
    requireNonNull(command);
    return executable_new(object_new(&TYPE_COMMAND, command), source, command_exec_);
}

void command_addWord(Command *command, char *word) {
    requireNonNull(command);
    requireNonNull(word);
    list_addLast(command->words, object_new(&TYPE_STRING, word));
}

void command_addRedirect(Command *command, Redirect *redirect) {
    requireNonNull(command);
    requireNonNull(redirect);
    list_addLast(command->redirects, object_new(&TYPE_REDIRECT, redirect));
}

int command_exec(Command *command, Shell *shell, IOStreams *streams) {
    requireNonNull(command);
    Builtin builtin = shell_getBuiltin(shell, object_get(list_peekFirst(command->words), &TYPE_STRING));
    if (builtin == NULL) {
        char **words = calloc((size_t) command->words->size + 1, sizeof(char *));
        Iterator *wIter = list_iterator(command->words);
        for (int i = 0; iterator_hasNext(wIter); i++) {
            words[i] = object_get(iterator_next(wIter), &TYPE_STRING);
        }
        iterator_dispose(wIter);
        pid_t cpid = fork();
        if (cpid < 0) {
            pExit("fork");
        }
        if (cpid == 0) {
            // TODO: fix redirects to work on all executables
            Iterator *rIter = list_iterator(command->redirects);
            while (iterator_hasNext(rIter)) {
                redirect_perform(object_get(iterator_next(wIter), &TYPE_REDIRECT));
            }
            iterator_dispose(rIter);
            execvp(words[0], words);
            pExit(words[0]);
        }
        int status;
        pid_t w = waitpid(cpid, &status, 0);
        if (w == -1) {
            pExit("waitpid");
        }
        free(words);
        return status;
    }
    return builtin(shell, streams, command->words);
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

void *command_clone(void *o) {
    Command *c = (Command *) o;
    Command *n = command_new();
    Iterator *itr = list_iterator(c->words);
    while (iterator_hasNext(itr)) {
        command_addWord(n, string_clone(object_get(iterator_next(itr), &TYPE_STRING)));
    }
    iterator_dispose(itr);
    itr = list_iterator(c->redirects);
    while (iterator_hasNext(itr)) {
        command_addRedirect(n, redirect_clone(object_get(iterator_next(itr), &TYPE_REDIRECT)));
    }
    iterator_dispose(itr);
    return n;
}

void command_dispose(void *o) {
    if (o == NULL) {
        return;
    }
    list_dispose(((Command *) o)->words);
    list_dispose(((Command *) o)->redirects);
    free(o);
}
