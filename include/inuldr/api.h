#pragma once
#include <inuos.h>

struct Catalogue {
    Void* stackLocation;
    UIntPtr stackSize;
    UIntPtr loadOffset;
}
INUPACKED
INUNATIVE;

Void BtldrInitialize(UIntPtr magic, Void* table);