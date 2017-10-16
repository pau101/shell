#include "datatype.h"
#include <string.h>

bool datatype_equals(const DataType *dataType1, const DataType *dataType2) {
    return strcmp(dataType1->identifier, dataType2->identifier) == 0;
}
