#ifndef SHELL_FILEDESCRIPTOROPENER_H
#define SHELL_FILEDESCRIPTOROPENER_H

#include <stdio.h>
#include "../handle.h"

typedef struct filedescriptoropener {
    int fd;
    char *mode;
} FileDescriptorOpener;

FileDescriptorOpener *fdopener_new(int fd, char *mode);

Handle *fdopener_handle(int fd, char *mode);

FILE *fdopener_open(FileDescriptorOpener *fileDescriptorOpener);

char *fdopener_toString(void *o);

unsigned int fdopener_hashCode(void *o);

int fdopener_compareTo(void *o1, void *o2);

void *fdopener_clone(void *o);

void fdopener_dispose(void *o);

static const DataType TYPE_FDOPENER = {"FDOPENER", fdopener_toString, fdopener_hashCode, fdopener_compareTo,
                                       fdopener_clone, fdopener_dispose};

#endif //SHELL_FILEDESCRIPTOROPENER_H
