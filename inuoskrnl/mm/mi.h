#pragma once
#include <executive/mm/api.h>

struct Allocator *MiCreateAllocator(IN UIntPtr lineCount);

struct Allocator *MiLocateAllocator(IN UIntPtr lineCount);

Void MiFindFreePage(INOUT UIntPtr *pageIndex,
                    OUT UIntPtr *pageLength,
                    IN UIntPtr pageCount);

Void MiOccupyPageAllocator(IN UIntPtr pageIndex);

Void MiOccupyPagePayload(IN UIntPtr pageIndex);

Void MiOccupyPageAllocatorSpace(IN UIntPtr pageIndex);

Void MiSetPageAttributes(IN UIntPtr pageIndex, IN struct PageInformation info);

Void MiReservePage(IN UIntPtr pageIndex);

Void MiFlushPage(IN UIntPtr pageIndex);

Void *MiGetPageAddress(IN UIntPtr pageIndex);

INUMONITOR Void *MiAllocatorAllocate(IN struct Allocator *target,
                                     IN UIntPtr lineCount);

Void MiAllocatorOccupyLine(IN struct Allocator *target,
                           IN UIntPtr lineIndex);

Void MiAllocatorFreeLine(IN struct Allocator *target,
                         IN UIntPtr lineIndex);

Void *MiAllocatorGetLineAddress(IN struct Allocator *target,
                                IN UIntPtr lineIndex);

Boolean MiIsLineOccupied(IN struct Allocator *target,
                         IN UIntPtr lineIndex);

Void MiAllocatorRecount(IN struct Allocator *target);

