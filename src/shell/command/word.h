#ifndef SHELL_WORD_H
#define SHELL_WORD_H

typedef struct word {
    char *getValue(Shell *shell);
} Word;

#endif //SHELL_WORD_H
