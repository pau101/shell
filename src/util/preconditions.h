#ifndef SHELL_PRECONDITIONS_H
#define SHELL_PRECONDITIONS_H

#include <stdio.h>
#include <stdlib.h>

#define \
    errExit(reason) ({ \
        fprintf(stderr, reason); \
        exit(EXIT_FAILURE); \
    })

#define \
    pExit(reason) ({ \
        perror(reason); \
        exit(EXIT_FAILURE); \
    })

#define \
    requireNonNull(ptr) ({ \
        if ((ptr) == NULL) errExit(#ptr " is null"); \
    })

#endif //SHELL_PRECONDITIONS_H
