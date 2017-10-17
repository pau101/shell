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

#define \
    checkElementIndex(index, size) ({ \
        if ((index) < 0 || (index) >= (size)) errExit(#index " is out of bounds"); \
    })

#define \
    checkPositionIndex(index, size) ({ \
        if ((index) < 0 || (index) > (size)) errExit(#index " is out of bounds"); \
    })

#endif //SHELL_PRECONDITIONS_H
