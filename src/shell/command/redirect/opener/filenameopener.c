#include <stdlib.h>
#include "filenameopener.h"
#include "../../../../util/preconditions.h"

FilenameOpener *filenameopener_new(char *filename, char *mode) {
    FilenameOpener *filenameOpener = calloc(1, sizeof(FilenameOpener));
    filenameOpener->filename = filename;
    filenameOpener->mode = mode;
    return filenameOpener;
}

void filenameopener_dispose_(void *d) {
    filenameopener_dispose(d);
}

FILE *filenameopener_open_(void *d) {
    filenameopener_open(d);
}

Handle *filenameopener_handle(char *filename, char *mode) {
    return handle_new(filenameopener_new(filename, mode), filenameopener_dispose_, filenameopener_open_);
}

FILE *filenameopener_open(FilenameOpener *filenameOpener) {
    FILE *f = fopen(filenameOpener->filename, filenameOpener->mode);
    if (f == NULL) {
        pExit("fopen");
    }
    return f;
}

void filenameopener_dispose(FilenameOpener *filenameOpener) {
    if (filenameOpener == NULL) {
        return;
    }
    free(filenameOpener->filename);
    free(filenameOpener->mode);
    free(filenameOpener);
}
