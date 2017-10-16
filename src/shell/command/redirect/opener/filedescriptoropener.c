#include <stdlib.h>
#include "filedescriptoropener.h"
#include "../../../../util/preconditions.h"

FileDescriptorOpener *fdopener_new(int fd, char *mode) {
    FileDescriptorOpener *fileDescriptorOpener = calloc(1, sizeof(FileDescriptorOpener));
    fileDescriptorOpener->fd = fd;
    fileDescriptorOpener->mode = mode;
    return fileDescriptorOpener;
}

void fdopener_dispose_(void *d) {
    fdopener_dispose(d);
}

FILE *fdopener_open_(void *d) {
    fdopener_open(d);
}

Handle *fdopener_handle(int fd, char *mode) {
    return handle_new(fdopener_new(fd, mode), fdopener_dispose_, fdopener_open_);
}

FILE *fdopener_open(FileDescriptorOpener *fileDescriptorOpener) {
    FILE *f = fdopen(fileDescriptorOpener->fd, fileDescriptorOpener->mode);
    if (f == NULL) {
        pExit("fdopen");
    }
    return f;
}

void fdopener_dispose(FileDescriptorOpener *fileDescriptorOpener) {
    if (fileDescriptorOpener == NULL) {
        return;
    }
    free(fileDescriptorOpener->mode);
    free(fileDescriptorOpener);
}
