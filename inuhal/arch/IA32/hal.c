#include <executive/mm/api.h>
#include <executive/ob/api.h>
#include <executive/pvm/api.h>
#include <inuhal/api.h>
#include <executive/rt/api.h>
#include <executive/vm/api.h>
#include <executive/id/api.h>
#include <inufw/api.h>

INUGLOBAL struct Catalogue HalInformation;

#include "vm.h"
#include "native.h"

INUEXTERN Void NtLoadVm(Void* table);
INUEXTERN Void NtEnableVm();
INUEXTERN Void NtDisableVm();
INUEXTERN Void* NtGetTable();
INUEXTERN Void NtFlushVm();

INUGLOBAL INUSTATIC UInt32 HalPageTable[1024] __attribute__((aligned(4096)));

Void HalInitialize(const struct Catalogue* table) {
    if (table == Null) {
        HalInformation.loadOffset = 0;
        return;
    }
    HalInformation = *table;
}

Void HalHalt() {
    HalDisableSignals();
    asm volatile ("hlt");
}

Void HalDisableSignals() {
    asm volatile ("cli");
}

Void HalEnableSignals() {
    asm volatile ("sti");
}

UIntPtr HalInterruptCount() {
    return 256;
}

Void HalInitializeDescription(struct Description *target) {
    target->gpde = ObCreatePayload(1);
    VmTableInitialize(MmGetPadeAddress(target->gpde->pageIndex));
}

Void HalRemapBootloader() {
    UInt32* tableData = HalPageTable;
    const struct PDE4 target = {
        .present = true,
        .accessed = false,
        .rw = true,
        .us = false,
        .writeThroughCache = false,
        .cacheDisable = false,
        .dirty = false,
        .pageSize = true,
        .global = false,
        .reserved = 0,
    };
    UIntPtr address = 0x80000000;
    UInt32 attributes = 0;
    memcpy(&attributes,&target,sizeof(struct PDE4));
    for (int i = 0; i < 512; ++i) {
        tableData[i] = address | attributes;
        address+=BPS;
    }
    address = 0;
    for (int i = 512; i < 1024; ++i) {
        tableData[i] = address | attributes;
        address+=BPS;
    }
    Void* addr = HalVirtualToPhysical(tableData);
    NtLoadVm(addr);
}

UIntPtr HalVmTableCount() {
    return 1024;
}

Void * HalVirtualToPhysical(Void *virtualAddress) {
    return virtualAddress-HalInformation.loadOffset;
}

Void * HalPhysicalToVirtual(Void *physicalAddress) {
    return physicalAddress+HalInformation.loadOffset;
}

Void HalEnterMonitor(Boolean *monitor) {
    AtomicCompareExchange(monitor,false,true);
}

Void HalExitMonitor(Boolean *monitor) {
    AtomicStore(monitor,false);
}

Void HalSetState(const struct Thread* target) {
    FwStub(L"HalSetState is not implemented!");
}

Void HalSwitchStack(Void *ptr) {
    __asm__ __volatile__("pop %edx");
    __asm__ __volatile__("mov (%esp),%esp");
    __asm__ __volatile__("add $0x80000000, %ebp");
    __asm__ __volatile__("push %edx");
    __asm__ __volatile__("ret");
}

Void HalSetDescriptor(struct Descriptor* target) {
    FwStub(L"HalSetDescriptor is unimplemented!!!");
    NtLoadVm(HalVirtualToPhysical(MmGetPadeAddress(target->payload.pageIndex)));
}

UInt32 HaliConvertVmaToPde(struct VMA attr) {
    const struct PDE data = {
        .accessed = false,
        .dirty = false,
        .present = attr.present,
        .reserved = false,
        .rw = attr.rw,
        .us = attr.us,
        .cacheDisable = attr.cd,
        .pageSize = false,
        .writeThroughCache = attr.wt
    };
    UInt32 value = 0;
    memcpy(&value,&data,sizeof(struct PDE));
    return value;
}

UInt32 HaliConvertVmaToPte(struct VMA attr) {
    const struct PTE data = {
        .accessed = false,
        .dirty = false,
        .present = attr.present,
        .reserved = false,
        .rw = attr.rw,
        .us = attr.us,
        .cacheDisable = attr.cd,
        .writeThroughCache = attr.wt,
        .global = attr.global,
        .pat = false
    };
    UInt32 value = 0;
    memcpy(&value,&data,sizeof(struct PTE));
    return value;
}

Void HalSetDescription(struct Description *target) {
    if (target->modified == true) {
        UInt32 globals[1024] = {};
        const struct GPDE* globalGpde = target->gpde->address;
        const struct MPDE* globalMpde = (struct MPDE*)globalGpde->address;
        const struct LPDE* globalLpde = (struct LPDE*)globalMpde->address;

        for (int i = 0; i < 4; ++i) {
            if (globalLpde[i].address != 0) {
                UInt32 pdes[256] = {};
                register const struct LPTE *localLpte = (struct LPTE *) globalLpde[i].address;

                UInt16 counter = 0;
                for (int z = 0; z < 512; ++z) {
                    UInt32 ptes[1024] = {};

                    register struct PF *firstLocalPf = (struct PF *) localLpte[z].address;
                    register struct PF *secondLocalPf = (struct PF *)  localLpte[++z].address;

                    if (firstLocalPf == Null && secondLocalPf == Null) {
                        continue;
                    }

                    if (firstLocalPf != Null) {
                        for (int q = 0; q < 512; ++q) {
                            const UIntPtr addressData = firstLocalPf[q].address;
                            if (addressData % 4096 != 0) {
                                FwDebugPrint("Non aligned data, LPDE: %i, LPTE: %i, Page: %i, Address: %x \r\n",i,z,q,addressData);
                            }
                            ptes[q] = addressData | 3; //HaliConvertVmaToPte(firstLocalPf->attributes);
                        }
                    }

                    if (secondLocalPf != Null) {
                        for (int q = 512; q < 1024; ++q) {
                            const UIntPtr addressData = secondLocalPf[q-512].address;
                            if (addressData % 4096 != 0) {
                                FwDebugPrint("Non aligned data, LPDE: %i, LPTE: %i, Page: %i, Address: %x \r\n",i,z,q,addressData);
                            }
                            ptes[q] = addressData | 3; //HaliConvertVmaToPte(secondLocalPf->attributes);
                        }
                    }

                    struct Payload* pteTarget = ObCreatePayload(1);
                    memcpy(pteTarget->address,&ptes,sizeof(ptes));
                    ObVectorAdd(target->nativePayload,(struct Object*)pteTarget);
                    pdes[counter++] = (UInt32)HalVirtualToPhysical(pteTarget->address) | 3;
                }
                memcpy(&globals[i*256],&pdes,sizeof(pdes));
            }
            else {
                memset((char*)&globals[i*256],0,sizeof(UInt32)*256);
            }
        }
        struct Payload* pdeTarget = ObCreatePayload(1);
        memcpy(pdeTarget->address,&globals,sizeof(globals));
        //ObVectorAdd(target->nativePayload,(struct Object*)pdeTarget);
        target->global = pdeTarget;
    }

    NtLoadVm(HalVirtualToPhysical(target->global->address));
}



INUFORCEINLINE UIntPtr HaliFillPdeAttributes(struct SegmentAttributes attributes, Void* pte) {
    return 0;
}

INUFORCEINLINE UIntPtr HaliFillPteAttributes(struct SegmentAttributes attributes, Void* address) {
    return 0;
}

Boolean HalEndianess() {
    return false;
}

Void HalDisableVM() {
    NtDisableVm();
}

Void HalEnableVM() {
    NtEnableVm();
}

UInt64 HalMaxPM() {
    return (UInt64)UINT32_MAX+1;
}

UInt64 HalMinPM() {
    return 0;
}

UIntPtr HalStateSize() {
    return sizeof(struct X86PMRegisters);
}
