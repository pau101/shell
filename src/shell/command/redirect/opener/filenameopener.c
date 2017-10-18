#include <stdlib.h>
#include "filenameopener.h"
#include "../../../../util/preconditions.h"
#include "../../../../object/type/reference.h"
#include "../../../../object/type/string.h"

FilenameOpener *filenameopener_new(char *filename, char *mode) {
    FilenameOpener *filenameOpener = calloc(1, sizeof(FilenameOpener));
    filenameOpener->filename = filename;
    filenameOpener->mode = mode;
    return filenameOpener;
}

FILE *filenameopener_open_(void *d) {
    filenameopener_open(object_get(d, &TYPE_FILENAMEOPENER));
}

Handle *filenameopener_handle(char *filename, char *mode) {
    return handle_new(object_new(&TYPE_FILENAMEOPENER, filenameopener_new(filename, mode)), filenameopener_open_);
}

FILE *filenameopener_open(FilenameOpener *filenameOpener) {
    FILE *f = fopen(filenameOpener->filename, filenameOpener->mode);
    if (f == NULL) {
        pExit("fopen");
    }
    return f;
}

char *filenameopener_toString(void *o) {
    return reference_toString(o);
}

unsigned int filenameopener_hashCode(void *o) {
    return reference_hashCode(o);
}

int filenameopener_compareTo(void *o1, void *o2) {
    return reference_compareTo(o1, o2);
}

void *filenameopener_clone(void *o) {
    FilenameOpener *f = (FilenameOpener *) o;
    return filenameopener_new(string_clone(f->filename), string_clone(f->mode));
}

void filenameopener_dispose(void *o) {
    if (o == NULL) {
        return;
    }
    free(((FilenameOpener *) o)->filename);
    free(((FilenameOpener *) o)->mode);
    free(o);
}
