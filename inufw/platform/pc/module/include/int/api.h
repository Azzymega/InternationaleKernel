#pragma once
#include <inuos.h>
#include <inufw/api.h>

#define ROUTER_CALL 0xE2

struct IDTEntry {
    UInt16 BaseLow;
    UInt16 Selector;
    UByte Zero;
    UByte Flags;
    UInt16 BaseHigh;
}
INUPACKED
INUARCHDEP;

struct IDTPointer {
    UInt16 Limit;
    UInt32 Base;
}
INUPACKED
INUARCHDEP;

Void IntInitialize();
Void IntSetIsr(UByte index, UInt32 base, UInt16 selector, UByte flags);