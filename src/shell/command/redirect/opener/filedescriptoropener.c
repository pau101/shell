#include <stdlib.h>
#include "filedescriptoropener.h"
#include "../../../../util/preconditions.h"
#include "../../../../object/type/reference.h"
#include "../../../../object/type/string.h"

FileDescriptorOpener *fdopener_new(int fd, char *mode) {
    FileDescriptorOpener *fileDescriptorOpener = calloc(1, sizeof(FileDescriptorOpener));
    fileDescriptorOpener->fd = fd;
    fileDescriptorOpener->mode = mode;
    return fileDescriptorOpener;
}

FILE *fdopener_open_(void *d) {
    fdopener_open(object_get(d, &TYPE_FDOPENER));
}

Handle *fdopener_handle(int fd, char *mode) {
    return handle_new(object_new(&TYPE_FDOPENER, fdopener_new(fd, mode)), fdopener_open_);
}

FILE *fdopener_open(FileDescriptorOpener *fileDescriptorOpener) {
    FILE *f = fdopen(fileDescriptorOpener->fd, fileDescriptorOpener->mode);
    if (f == NULL) {
        pExit("fdopen");
    }
    return f;
}

char *fdopener_toString(void *o) {
    return reference_toString(o);
}

unsigned int fdopener_hashCode(void *o) {
    return reference_hashCode(o);
}

int fdopener_compareTo(void *o1, void *o2) {
    return reference_compareTo(o1, o2);
}

void *fdopener_clone(void *o) {
    FileDescriptorOpener *f = (FileDescriptorOpener *) o;
    return fdopener_new(f->fd, string_clone(f->mode));
}

void fdopener_dispose(void *o) {
    if (o == NULL) {
        return;
    }
    free(((FileDescriptorOpener *) o)->mode);
    free(o);
}
