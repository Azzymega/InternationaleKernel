#pragma once
#include <inuos.h>
#include <executive/ob/api.h>

struct Page {
    Byte values[MM_PAGE_SIZE];
}
INUPACKED;

struct Line {
    Byte values[MM_LINE_SIZE];
}
INUPACKED;

struct PageInformation {
    UByte occupied : 1;
    UByte kernel: 1;
    UByte allocator: 1;
    UByte allocatorSpace: 1;
    UByte payload: 1;
    UByte forbidden: 1;
    UByte reserved: 2;
}
INUPACKED;

Void MmInitialize();

INUMONITOR Void *MmGetPadeAddress(IN UIntPtr pageIndex);

INUMONITOR UIntPtr MmGetPadeIndex(IN Void *pageAddress);

INUMONITOR Void *MmAllocateMemory(IN UIntPtr size);

INUMONITOR Void *MmAllocatePayload(IN Int64 pageCount);

INUMONITOR Void *MmFreePayload(IN Int64 firstIndex,
                               IN Int64 pageCount);

INUMONITOR Void *MmFreeMemory(IN Void *target);

Void *MmAllocateInplace(IN Void *target);

Void *MmFreeInplace(IN Void *target);
