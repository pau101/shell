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

void fdopener_dispose(FileDescriptorOpener *fileDescriptorOpener);

#endif //SHELL_FILEDESCRIPTOROPENER_H
