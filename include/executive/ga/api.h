#pragma once
#include <inuos.h>

struct GaAllocator {
    UIntPtr size;
    UIntPtr freeBlocks;
    UIntPtr freeBlocksLeft;
    UByte* bitmap;
    UByte* data;
}
INUPACKED
INUNATIVE;

struct GaAllocationPool {
    UIntPtr size;
    struct GaAllocationPool* next;
    struct GaAllocator allocator;
}
INUPACKED
INUNATIVE;

Void GaInitialize();

struct GaAllocationPool* GaCreateAllocationPool(IN UIntPtr pageCount);
Void GaCreateAllocator(INOUT struct GaAllocator* pointer, IN UIntPtr spaceLeft);
struct GaAllocator* GaLocateAllocator(IN UIntPtr blockCount);

UIntPtr GaCalculateRequestBlock(UIntPtr size);

Void* GaAllocatorAllocate(IN struct GaAllocator* data, IN UIntPtr blockCount);
Void* GaAllocatorFree(IN struct GaAllocator* data, IN UIntPtr initialBlock, IN UIntPtr length);

Void GaAllocatorRecalculate(IN struct GaAllocator* data);
Void GaAllocatorMarkBlock(IN struct GaAllocator* data, IN UIntPtr blockIndex);
Void GaAllocatorFreeBlock(IN struct GaAllocator* data, IN UIntPtr blockIndex);
Boolean GaAllocatorCheckBlock(IN struct GaAllocator* data, IN UIntPtr blockIndex);

// EXTERNAL API

INUMONITOR Void* GaAllocateMemory(IN UIntPtr size);
INUMONITOR Void* GaFreeMemory(IN Void* ptr);

// LEGACY INTERFACE

INUMONITOR Void *MmGetPadeAddress(IN UIntPtr pageIndex);
INUMONITOR UIntPtr MmGetPadeIndex(IN Void *pageAddress);
INUMONITOR Void *MmAllocateMemory(IN UIntPtr size);
INUMONITOR Void *MmAllocatePayload(IN Int64 pageCount);
INUMONITOR Void *MmFreePayload(IN Int64 firstIndex,
                               IN Int64 pageCount);
INUMONITOR Void *MmFreeMemory(IN Void *target);
Void *MmAllocateInplace(IN Void *target);
Void *MmFreeInplace(IN Void *target);
