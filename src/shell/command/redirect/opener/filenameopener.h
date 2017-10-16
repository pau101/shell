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

void filenameopener_dispose(FilenameOpener *filenameOpener);

#endif //SHELL_FILENAMEOPENER_H
