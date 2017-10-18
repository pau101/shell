#include <stdlib.h>
#include "object.h"
#include "../util/preconditions.h"
#include "../util/util.h"

Object *object_new(const DataType *dataType, void *data) {
    requireNonNull(dataType);
    requireNonNull(data);
    Object *object = calloc(1, sizeof(Object));
    object->dataType = dataType;
    object->data = data;
    return object;
}

void *object_get(Object *object, const DataType *dataType) {
    if (object == NULL) {
        return NULL;
    }
    if (datatype_equals(object->dataType, dataType)) {
        return object->data;
    }
    errExit("type mismatch");
}

void *object_getAndDispose(Object *object, const DataType *dataType) {
    if (object == NULL) {
        return NULL;
    }
    if (!datatype_equals(object->dataType, dataType)) {
        errExit("type mismatch");
    }
    void *data = object->data;
    object->data = NULL;
    object_dispose(object);
    return data;
}

char *object_toString(Object *object) {
    return object == NULL ? newString("null") : object->dataType->toString(object->data);
}

unsigned int object_hashCode(Object *object) {
    return object == NULL ? 0 : object->dataType->hashCode(object->data);
}

int object_compareTo(Object *object1, Object *object2) {
    if (object1 == NULL || object2 == NULL) {
        return object1 == NULL && object2 == NULL ? 0 : object1 == NULL ? -1 : 1;
    }
    if (object1->dataType != object2->dataType) {
        return object1 < object2 ? -1 : 1;
    }
    return object1->dataType->compareTo(object1->data, object2->data);
}

void object_dispose(Object *object) {
    if (object == NULL) {
        return;
    }
    object->dataType->dispose(object->data);
    free(object);
}
