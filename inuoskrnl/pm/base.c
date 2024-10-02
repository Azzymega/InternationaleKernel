#include <executive/pm/api.h>
#include <executive/rt/api.h>
#include <inufw/api.h>
#include <inuhal/api.h>

#define PM_KERNEL_OFFSET 4000000
#define PM_BASE_ALLOCATOR_SIZE 512
#define PM_FORBIDDEN_BASE 0
#define PM_BAD_PAGE 0
#define PM_PAGE_SIZE 4096
#define PM_LINE_BASE 1000

#define PM_ALIGN_UP(memAddress,offset) memAddress+(offset-(memAddress%offset));
#define PM_ALIGN_DOWN(memAddress,offset) (memAddress/offset)*memAddress

INUGLOBAL Boolean PmGlobalSynchronizationMonitor;
INUGLOBAL struct PmPageAttributes* PmGlobalAttributeTable;
INUGLOBAL Void* PmPage;
INUGLOBAL UIntPtr PmPageCount;

Void PmInitialize() {
    UIntPtr memoryStartAddress = (UIntPtr)FwPMMStart()+PM_KERNEL_OFFSET;
    memoryStartAddress = PM_ALIGN_UP(memoryStartAddress,PM_PAGE_SIZE);

    Void* memoryStartLocation = (Void*)memoryStartAddress;
    UIntPtr memorySize = FwPMMSize()-PM_KERNEL_OFFSET;
    const UIntPtr pageCount = memorySize/PM_PAGE_SIZE;

    const UIntPtr pageCountMapSize = pageCount*sizeof(struct PmPageAttributes);
    UIntPtr pageCountMapSizeInPages = PM_ALIGN_UP(pageCountMapSize,PM_PAGE_SIZE);
    pageCountMapSizeInPages /= PM_PAGE_SIZE;

    PmGlobalAttributeTable = memoryStartLocation;
    PmGlobalSynchronizationMonitor = false;

    memoryStartLocation += pageCountMapSizeInPages*PM_PAGE_SIZE;
    memorySize -= pageCountMapSizeInPages*PM_PAGE_SIZE;

    PmPageCount = memorySize/PM_PAGE_SIZE;
    PmPage = memoryStartLocation;

    memset((AChar*)PmGlobalAttributeTable,0,PmPageCount*sizeof(struct PmPageAttributes));

    const struct PmPageAttributes reservedAttributes = {
        .forbidden = true,
        .occipued = true
    };

    PmChangePageAttributes(0,reservedAttributes);
}

UIntPtr PmAllocatePage(const UIntPtr count) {
    HalEnterMonitor(&PmGlobalSynchronizationMonitor);

    UIntPtr entry = 0;
    UIntPtr length = 0;

    for (int i = 0; i < PmPageCount; ++i) {
        if (!PmGlobalAttributeTable[i].occipued) {
            length++;
        }
        else {
            entry = i+1;
            length = 0;
        }
        if (length == count) {
            const struct PmPageAttributes attributes = {
                .occipued = true
            };
            for (int z = 0; z < length; ++z) {
                PmChangePageAttributes(z+entry,attributes);
            }
            HalExitMonitor(&PmGlobalSynchronizationMonitor);
            return entry;
        }
    }

    HalExitMonitor(&PmGlobalSynchronizationMonitor);
    return PM_BAD_PAGE;
}

UIntPtr PmGetPageCount() {
    return PmPageCount;
}

UIntPtr PmGetPageSize() {
    return PM_PAGE_SIZE;
}

Void PmFreePage(const UIntPtr index) {
    HalEnterMonitor(&PmGlobalSynchronizationMonitor);
    const struct PmPageAttributes attributes = {
        .occipued = true
    };
    PmChangePageAttributes(index,attributes);
    HalExitMonitor(&PmGlobalSynchronizationMonitor);
}

Void PmChangePageAttributes(const UIntPtr index, const struct PmPageAttributes attributes) {
    HalEnterMonitor(&PmGlobalSynchronizationMonitor);
    PmGlobalAttributeTable[index] = attributes;
    HalExitMonitor(&PmGlobalSynchronizationMonitor);
}

Void * PmGetPageAddress(const UIntPtr index) {
    return PmPage + PM_PAGE_SIZE * index;
}

UIntPtr PmGetPageIndex(Void *address) {
    const UIntPtr firstPageNumericalAddress = (UIntPtr)PmPage;
    const UIntPtr targtPageNumericalAddress = (UIntPtr)address;

    return (targtPageNumericalAddress-firstPageNumericalAddress)/PM_PAGE_SIZE;
}

