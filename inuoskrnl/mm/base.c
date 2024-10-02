#include <inufw/api.h>
#include <executive/mm/api.h>
#include <executive/ob/api.h>
#include <executive/rt/api.h>
#include "mi.h"

#define MM_KERNEL_OFFSET 4000000
#define MM_BASE_ALLOCATOR_SIZE 512
#define MM_FORBIDDEN_BASE 0
#define MM_LINE_BASE 1000

INUGLOBAL struct MemoryManager* Manager;

struct Allocator * MiCreateAllocator(UIntPtr lineCount) {
    // REWRITE THIS CRAP FROM THE GROUND. THERE SOME FAULT THAT REALLOC ALLOCATED PAGE!@#$!@#$
    if (lineCount < MM_LINE_BASE) {
        lineCount = MM_LINE_BASE;
    }
    const UIntPtr rawSize = lineCount*MM_LINE_SIZE;
    UIntPtr pageCount = rawSize/MM_PAGE_SIZE;
    if (rawSize % MM_PAGE_SIZE != 0) {
        pageCount++;
    }

    UIntPtr pageIndex = 0;
    UIntPtr pageLength = 0;

    MiFindFreePage(&pageIndex,&pageLength,pageCount);

    MiOccupyPageAllocator(pageIndex);
    for (UIntPtr z = 1; z < pageLength; ++z) {
        MiOccupyPageAllocatorSpace(pageIndex+z);
    }

    Void* pageAddress = MiGetPageAddress(pageIndex);
    struct Allocator* target = pageAddress;

    ObAllocatorInitialize(target,pageLength*MM_PAGE_SIZE);

    FwDebugPrint("Creating new allocator at address %x, boundary %x, lines %i \r\n",pageAddress,pageAddress+(pageLength*MM_PAGE_SIZE),target->lineCount);
    return target;
}

struct Allocator * MiLocateAllocator(UIntPtr lineCount) {
    for (UIntPtr i = 0; i < Manager->pageCount; ++i) {
        const struct PageInformation target = Manager->map[i];
        if (target.allocator == true) {
            struct Allocator* allocator = MiGetPageAddress(i);
            if (allocator->freeLines >= lineCount) {
                return allocator;
            }
        }
    }
    return MiCreateAllocator(lineCount);
}

Void MiFindFreePage(UIntPtr *pageIndex, UIntPtr *pageLength, const UIntPtr pageCount) {
    UIntPtr startPage = *pageIndex;
    UIntPtr lengthTarget = 0;

    for (UIntPtr i = startPage; i < Manager->pageCount; ++i) {
        struct PageInformation target = Manager->map[i];
        if (target.occupied) {
            startPage = i+1;
            lengthTarget = 0;
        }
        else {
            lengthTarget++;
            if (lengthTarget == pageCount) {
                *pageIndex = startPage;
                *pageLength = lengthTarget;
                return;
            }
        }
    }

    FwEmergency(L"OUT OF MEMORY!");
}

Void MiOccupyPageAllocator(const UIntPtr pageIndex) {
    MiFlushPage(pageIndex);
    const struct PageInformation attributes = {
        .occupied = true,
        .allocator = true
    };
    MiSetPageAttributes(pageIndex,attributes);
}

Void MiOccupyPagePayload(UIntPtr pageIndex) {
    MiFlushPage(pageIndex);
    const struct PageInformation attributes = {
        .occupied = true,
        .payload = true
    };
    MiSetPageAttributes(pageIndex,attributes);
}

Void MiOccupyPageAllocatorSpace(UIntPtr pageIndex) {
    MiFlushPage(pageIndex);
    const struct PageInformation attributes = {
        .occupied = true,
        .allocatorSpace = true
    };
    MiSetPageAttributes(pageIndex,attributes);
}

Void MiSetPageAttributes(const UIntPtr pageIndex, const struct PageInformation info) {
    Manager->map[pageIndex] = info;
}

Void MiReservePage(UIntPtr pageIndex) {
    const struct PageInformation attributes = {
        .occupied = true,
        .forbidden = true
    };
    MiSetPageAttributes(pageIndex,attributes);
}

Void MiFlushPage(const UIntPtr pageIndex) {
    const struct PageInformation attributes = {

    };
    MiSetPageAttributes(pageIndex,attributes);
}

Void * MiGetPageAddress(UIntPtr pageIndex) {
    return &Manager->pages[pageIndex];
}

Void * MiAllocatorAllocate(struct Allocator *target, UIntPtr lineCount) {
    ObEnterMonitorObject(&target->object);

    if (target->freeLines > target->lineCount) {
        FwEmergency(L"Bad allocator! FreeLines > LineCount");
    }

    Void* address = Null;
    UIntPtr start = 0;
    UIntPtr length = 0;

    for (UIntPtr i = 0; i < target->lineCount; ++i) {
        const UIntPtr mapIndex = i/BIT_PER_BYTE;
        const UIntPtr lineIndex = i%BIT_PER_BYTE;
        if (chkbit(target->bitmap[mapIndex],lineIndex)) {
            start = i + 1;
            length = 0;
        }
        else {
            length++;

            if (length >= lineCount) {
                for (UIntPtr z = 0; z < length; ++z) {
                    MiAllocatorOccupyLine(target,z+start);
                }

                address = MiAllocatorGetLineAddress(target,start);
                target->allocatedCount++;
                break;
            }
        }
    }

    MiAllocatorRecount(target);
    ObExitMonitorObject(&target->object);

    if (address == Null) {
        FwEmergency(L"Failed to allocate memory! MmAllocatorAllocate!");
        return Null;
    }
    return address;
}

Void MiAllocatorOccupyLine(struct Allocator *target, UIntPtr lineIndex) {
    const UIntPtr index = lineIndex/BIT_PER_BYTE;
    const UIntPtr bit = lineIndex%BIT_PER_BYTE;
    target->bitmap[index] = setbit(target->bitmap[index],bit);
}

Void MiAllocatorFreeLine(struct Allocator *target, UIntPtr lineIndex) {
    const UIntPtr index = lineIndex/BIT_PER_BYTE;
    const UIntPtr bit = lineIndex%BIT_PER_BYTE;
    target->bitmap[index] = clrbit(target->bitmap[index],bit);
}

Void * MiAllocatorGetLineAddress(struct Allocator *target, UIntPtr lineIndex) {
    return &target->lines[lineIndex];
}

Boolean MiIsLineOccupied(struct Allocator *target, UIntPtr lineIndex) {
    const UIntPtr index = lineIndex/BIT_PER_BYTE;
    const UIntPtr bit = lineIndex%BIT_PER_BYTE;
    return chkbit(target->bitmap[index],bit);
}

Void MiAllocatorRecount(struct Allocator *target) {
    const UIntPtr objectCount = target->allocatedCount;
    const UIntPtr initialFreeLines = target->freeLines;
    UIntPtr startSegments[objectCount];
    UIntPtr segmentLengths[objectCount];

    RtlClearStackArray(startSegments,sizeof(UIntPtr)*objectCount);
    RtlClearStackArray(segmentLengths,sizeof(UIntPtr)*objectCount);

    UIntPtr indexer = 0;
    UIntPtr segmentStart = 0;
    UIntPtr segmentLength = 0;

    for (UIntPtr i = 0; i < target->lineCount; ++i) {
        if (MiIsLineOccupied(target,i)) {
            if (segmentLength != 0) {
                startSegments[indexer] = segmentStart;
                segmentLengths[indexer] = segmentLength;
                indexer++;
            }
            segmentLength = 0;
            segmentStart = i + 1;
        }
        else {
            segmentLength++;
        }
    }

    if (segmentLength != 0) {
        startSegments[indexer] = segmentStart;
        segmentLengths[indexer] = segmentLength;
    }

    UIntPtr maxLength = 0;

    for (UIntPtr i = 0; i < objectCount; ++i) {
        if (segmentLengths[i] > maxLength) {
            maxLength = segmentLengths[i];
        }
    }

    if (maxLength > initialFreeLines) {
        FwDebugPrint("Initial free lines is smaller than calculated, initial: %i, calculated: i% \r\n",initialFreeLines,maxLength);
    }

    //FwDebugPrint("Free line count : %i, in allocator: %x \r\n",maxLength,target);

    target->freeLines = maxLength;
}

Void MmInitialize() {
    Void* memoryStartLocation = FwPMMStart()+MM_KERNEL_OFFSET;

    Manager = MmAllocateInplace(memoryStartLocation);

    UIntPtr memoryStartNumber = (UIntPtr)memoryStartLocation;
    UIntPtr memorySize = FwPMMSize()-MM_KERNEL_OFFSET;
    UIntPtr aligningOffset = memoryStartNumber+(MM_PAGE_SIZE-(memoryStartNumber%MM_PAGE_SIZE));

    memorySize-=sizeof(struct MemoryManager);
    memorySize-=(aligningOffset-memoryStartNumber);

    UIntPtr pageCount = memorySize/MM_PAGE_SIZE;
    Void* firstAlignedPage = (Void*)(aligningOffset);

    const UIntPtr bitmapSize = pageCount/(BIT_PER_BYTE*sizeof(struct PageInformation));
    UIntPtr bitmapPageSize = bitmapSize/MM_PAGE_SIZE;
    if (bitmapSize%MM_PAGE_SIZE != 0) {
        bitmapPageSize++;
    }
    Manager->map = firstAlignedPage;
    firstAlignedPage+=(MM_PAGE_SIZE*bitmapPageSize);

    ObMemoryManagerInitialize(Manager,firstAlignedPage,memorySize,pageCount);
    MiReservePage(MM_FORBIDDEN_BASE);
    MiCreateAllocator(MM_BASE_ALLOCATOR_SIZE);
}

Void * MmGetPadeAddress(UIntPtr pageIndex) {
    ObEnterMonitorObject(&Manager->object);
    Void* address = &Manager->pages[pageIndex];
    ObExitMonitorObject(&Manager->object);
    return address;
}

UIntPtr MmGetPadeIndex(Void *pageAddress) {
    ObEnterMonitorObject(&Manager->object);
    UIntPtr pageNumericalAddress = (UIntPtr)pageAddress;
    UIntPtr firstPageNumericalAddress = (UIntPtr)Manager->pages;
    UIntPtr index = (pageNumericalAddress-firstPageNumericalAddress)/MM_PAGE_SIZE;
    ObExitMonitorObject(&Manager->object);
    return index;
}

Void * MmAllocateMemory(const UIntPtr size) {
    UIntPtr lineCount = size/MM_LINE_SIZE;
    if (size % MM_LINE_SIZE != 0) {
        lineCount++;
    }

    ObEnterMonitorObject(&Manager->object);
    struct Allocator* target = MiLocateAllocator(lineCount);
    ObExitMonitorObject(&Manager->object);

    if (target != Null) {
        Void* ptr = MiAllocatorAllocate(target,lineCount);

        return ptr;
    }

    FwEmergency(L"Failed to locate allocator!");
    return Null;
}

Void * MmAllocatePayload(Int64 pageCount) {
    ObEnterMonitorObject(&Manager->object);

    UIntPtr pageIndex = 0;
    UIntPtr pageLength = 0;

    MiFindFreePage(&pageIndex,&pageLength,pageCount);

    for (UIntPtr z = 0; z < pageLength; ++z) {
        MiOccupyPagePayload(pageIndex+z);
    }

    ObExitMonitorObject(&Manager->object);

    return MiGetPageAddress(pageIndex);
}

Void * MmFreePayload(const Int64 firstIndex, const Int64 pageCount) {
    ObEnterMonitorObject(&Manager->object);
    for (UIntPtr i = 0; i < pageCount; ++i) {
        MiFlushPage(firstIndex+i);
    }
    ObExitMonitorObject(&Manager->object);
    return Null;
}

Void * MmFreeMemory(Void *target) {
    ObEnterMonitorObject(&Manager->object);

    ObExitMonitorObject(&Manager->object);
    return Null;
}

Void * MmAllocateInplace(Void *target) {
    return target;
}

Void* MmFreeInplace(Void* target) {
    return target;
}
