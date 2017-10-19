#ifndef SHELL_FILENAMEOPENER_H
#define SHELL_FILENAMEOPENER_H

#include <stdio.h>
#include "../handle.h"

typedef struct filenameOpener {
    char *filename;
    char *mode;
} FilenameOpener;

FilenameOpener *filenameopener_new(char *filename, char *mode);

Handle *filenameopener_handle(char *filename, char *mode);

FILE *filenameopener_open(FilenameOpener *filenameOpener);

char *filenameopener_toString(void *o);

unsigned int filenameopener_hashCode(void *o);

int filenameopener_compareTo(void *o1, void *o2);

void *filenameopener_clone(void *o);

void filenameopener_dispose(void *o);

static const DataType TYPE_FILENAMEOPENER = {"FILENAMEOPENER", filenameopener_toString, filenameopener_hashCode,
                                             filenameopener_compareTo, filenameopener_clone, filenameopener_dispose};

#endif //SHELL_FILENAMEOPENER_H
