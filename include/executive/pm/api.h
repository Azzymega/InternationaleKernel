#pragma once
#include <inuos.h>

struct PmPageAttributes {
    UByte occipued : 1;
    UByte forbidden : 1;
    UByte reserved : 6;
}
INUPACKED;

Void PmInitialize();
INUMONITOR UIntPtr PmAllocatePage(UIntPtr count);
INUMONITOR Void PmFreePage(UIntPtr index);
INUMONITOR Void PmChangePageAttributes(UIntPtr index, struct PmPageAttributes attributes);

UIntPtr PmGetPageCount();
UIntPtr PmGetPageSize();
Void* PmGetPageAddress(UIntPtr index);
UIntPtr PmGetPageIndex(Void* address);