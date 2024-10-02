#pragma once
#include <inuos.h>

#define ENTRY_COUNT 1024

struct SegmentAttributes;

struct PDE4 {
    UInt32 present : 1;
    UInt32 rw : 1;
    UInt32 us : 1;
    UInt32 writeThroughCache : 1;
    UInt32 cacheDisable : 1;
    UInt32 accessed : 1;
    UInt32 dirty : 1;
    UInt32 pageSize : 1;
    UInt32 global : 1;
    UInt32 reserved : 23;
}
INUPACKED
INUNATIVE;

struct PDE {
    UInt32 present : 1;
    UInt32 rw : 1;
    UInt32 us : 1;
    UInt32 writeThroughCache : 1;
    UInt32 cacheDisable : 1;
    UInt32 accessed : 1;
    UInt32 dirty : 1;
    UInt32 pageSize : 1;
    UInt32 reserved : 24;
}
INUPACKED
INUNATIVE;

struct PTE {
    UInt32 present : 1;
    UInt32 rw : 1;
    UInt32 us : 1;
    UInt32 writeThroughCache : 1;
    UInt32 cacheDisable : 1;
    UInt32 accessed : 1;
    UInt32 dirty : 1;
    UInt32 pat : 1;
    UInt32 global : 1;
    UInt32 reserved : 23;
}
INUPACKED
INUNATIVE;

INUFORCEINLINE UIntPtr HaliFillPdeAttributes(struct SegmentAttributes attributes, Void* pte);

INUFORCEINLINE UIntPtr HaliFillPteAttributes(struct SegmentAttributes attributes, Void* address);