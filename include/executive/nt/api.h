#pragma once
#include <inuos.h>

struct Payload {
    struct Object object;
    Void* address;
    Int64 pageIndex;
    Int64 pageCount;
}
INUPACKED
INUOBJECT;

