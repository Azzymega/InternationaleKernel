#include <executive/ga/api.h>
#include <executive/pm/api.h>
#include <executive/rt/api.h>
#include <inufw/api.h>
#include <inuhal/api.h>

// LINKED LIST CRAP, I AM SO SORRY!!!

#define GA_BLOCK_SIZE 64
#define GA_INITIAL_BLOCK_PAGES 10
#define GA_FREESPACE 0
#define GA_BITS_IN_BYTE 8

#define GA_READBIT(A, B) ((A >> (B & 7)) & 1)
#define GA_SETBIT(T, B, V) (T = V ? T | (1<<B) : T & ~(1<<B))

INUGLOBAL Boolean GaGlobalSynchronizationMonitor;
INUGLOBAL struct GaAllocationPool* GaInitialAllocationBlock;

Void GaInitialize() {
    GaGlobalSynchronizationMonitor = false;

    GaInitialAllocationBlock = GaCreateAllocationPool(GA_INITIAL_BLOCK_PAGES);
}

struct GaAllocationPool* GaCreateAllocationPool(const UIntPtr pageCount) {
    const UIntPtr initialBlock = PmAllocatePage(pageCount);
    struct GaAllocationPool* data = PmGetPageAddress(initialBlock);

    data->next = Null;
    data->size = PmGetPageSize()*pageCount;

    GaCreateAllocator(&data->allocator,data->size-(sizeof(struct GaAllocationPool)-sizeof(struct GaAllocator)));
    return data;
}

Void GaCreateAllocator(struct GaAllocator *pointer, UIntPtr spaceLeft) {
    pointer->size = spaceLeft;
    spaceLeft-=sizeof(struct GaAllocator);

    UIntPtr bitmapSize = spaceLeft/GA_BLOCK_SIZE;
    UIntPtr blockCount = bitmapSize;

    bitmapSize /= 8;

    UIntPtr bitmapSizeInBlock = bitmapSize/GA_BLOCK_SIZE;
    if (bitmapSize % GA_BLOCK_SIZE != 0) {
        bitmapSizeInBlock++;
    }

    blockCount-=bitmapSizeInBlock;
    spaceLeft-=bitmapSize;

    pointer->freeBlocks = blockCount;
    pointer->freeBlocksLeft = blockCount;
    pointer->bitmap = (Boolean*)pointer+sizeof(struct GaAllocator);
    pointer->data = (Byte*)pointer->bitmap+(GA_BLOCK_SIZE*bitmapSizeInBlock);
}

struct GaAllocator * GaLocateAllocator(const UIntPtr blockCount) {
    struct GaAllocationPool* pool = GaInitialAllocationBlock;
    while (pool != Null) {
        if (pool->allocator.freeBlocksLeft > blockCount) {
            return &pool->allocator;
        }
        struct GaAllocationPool* previousPool = pool;
        pool = pool->next;
        if (pool == Null) {
            previousPool->next = GaCreateAllocationPool((blockCount/4096)+5);
            if (previousPool->next != Null) {
                return &previousPool->next->allocator;
            }
        }
    }
    FwEmergency("Memory is out!");
    return Null;
}

UIntPtr GaCalculateRequestBlock(const UIntPtr size) {
    UIntPtr blockSize = size/GA_BLOCK_SIZE;
    if (size%GA_BLOCK_SIZE != 0) {
        blockSize++;
    }
    return blockSize;
}

Void * GaAllocatorAllocate(struct GaAllocator *data, const UIntPtr blockCount) {
    UIntPtr initial = 0;
    UIntPtr length = 0;
    Void* pointer = Null;

    for (UIntPtr i = 0; i < data->freeBlocks; ++i) {
        if (GaAllocatorCheckBlock(data,i)) {
            initial = i+1;
            length = 0;
        }
        else {
            length++;

            if (length == blockCount) {
                for (int z = 0; z < length; ++z) {
                    GaAllocatorMarkBlock(data,initial+z);
                }
                pointer = data->data+(GA_BLOCK_SIZE*initial);
                break;
            }
        }
    }

    GaAllocatorRecalculate(data);
    return pointer;
}

Void * GaAllocatorFree(struct GaAllocator *data, const UIntPtr initialBlock, const UIntPtr length) {
    for (int i = 0; i < length; ++i) {
        GaAllocatorFreeBlock(data,initialBlock+i);
    }
    return Null;
}

Void GaAllocatorRecalculate(struct GaAllocator *data) {
    UIntPtr max = 0;
    UIntPtr length = 0;

    for (UIntPtr i = 0; i < data->freeBlocks; ++i) {
        if (GaAllocatorCheckBlock(data,i)) {
            if (length > max) {
                max = length;
            }
            length = 0;
        }
        else {
            length++;
        }
    }
    if (length > max) {
        max = length;
    }
    data->freeBlocksLeft = max;
}

Void GaAllocatorMarkBlock(struct GaAllocator *data, UIntPtr blockIndex) {
    const UIntPtr byte = blockIndex/GA_BITS_IN_BYTE;
    const UIntPtr bits = blockIndex%GA_BITS_IN_BYTE;

    data->bitmap[byte] = setbit(data->bitmap[byte],bits);
     //GA_SETBIT(data->bitmap[byte],bits,1);
}

Void GaAllocatorFreeBlock(struct GaAllocator *data, UIntPtr blockIndex) {
    const UIntPtr byte = blockIndex/GA_BITS_IN_BYTE;
    const UIntPtr bits = blockIndex%GA_BITS_IN_BYTE;

    //GA_SETBIT(data->bitmap[byte],bits,0);
    data->bitmap[byte] = clrbit(data->bitmap[byte],bits);
    memset((AChar*)data->data+blockIndex*GA_BLOCK_SIZE,GA_FREESPACE,GA_BLOCK_SIZE);
}

Boolean GaAllocatorCheckBlock(struct GaAllocator *data, UIntPtr blockIndex) {
    const UIntPtr byte = blockIndex/GA_BITS_IN_BYTE;
    const UIntPtr bits = blockIndex%GA_BITS_IN_BYTE;

    return GA_READBIT(data->bitmap[byte],bits);
}

Void * GaAllocateMemory(const UIntPtr size) {
    HalEnterMonitor(&GaGlobalSynchronizationMonitor);

    const UIntPtr blockSize = GaCalculateRequestBlock(size);

    struct GaAllocator* target = GaLocateAllocator(blockSize);
    Void* data = GaAllocatorAllocate(target,blockSize);

    HalExitMonitor(&GaGlobalSynchronizationMonitor);

    return data;
}

Void * GaFreeMemory(Void *ptr) {
    HalEnterMonitor(&GaGlobalSynchronizationMonitor);
    //TODO: IMPLEMENT FwStub("GaFreeMemory is not implemented!");
    HalExitMonitor(&GaGlobalSynchronizationMonitor);
    return Null;
}

// LEGACY INTERFACE

INUMONITOR Void *MmGetPadeAddress(IN const UIntPtr pageIndex) {
    return PmGetPageAddress(pageIndex);
}

INUMONITOR UIntPtr MmGetPadeIndex(IN Void *pageAddress) {
    return PmGetPageIndex(pageAddress);
}

INUMONITOR Void *MmAllocateMemory(IN const UIntPtr size) {
    return GaAllocateMemory(size);
}

INUMONITOR Void *MmAllocatePayload(IN const Int64 pageCount) {
    return PmGetPageAddress(PmAllocatePage(pageCount));
}

INUMONITOR Void *MmFreePayload(IN const Int64 firstIndex,
                               IN const Int64 pageCount) {
    for (int i = 0; i < pageCount; ++i) {
        PmFreePage(firstIndex+i);
    }
    return Null;
}

INUMONITOR Void *MmFreeMemory(IN Void *target) {
    return GaFreeMemory(target);
}

Void *MmAllocateInplace(IN Void *target) {
    return target;
}

Void *MmFreeInplace(IN Void *target) {
    return Null;
}
