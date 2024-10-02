#include <executive/mm/api.h>
#include <executive/nt/api.h>
#include <executive/pvm/api.h>
#include <inufw/api.h>
#include <inuhal/api.h>

#include "pvmi.h"

INUGLOBAL struct Description* VmKernelDescription;

Void VmiMapGpde(INOUT const struct Description *description, INOUT struct GPDE* target, IN UIntPtr baseOffset ,IN UIntPtr paddr, IN UIntPtr vaddr, IN struct VMA attr) {
    if (INUUNLIKELY(target->address == 0)) {
        struct Payload* data = ObCreatePayload(1);
        ObVectorAdd(description->mpde,(struct Object*)data);
        target->address = (UInt64)data->address;
        VmTableInitialize((Void*)target->address);
    }
    const UIntPtr mpdeIndex = baseOffset/VM_GPDE_GRANULARITY;
    struct MPDE* mpdeData = (struct MPDE*)target->address;
    //mpdeData[mpdeIndex].attributes = attr;
    mpdeData[mpdeIndex].attributes.present = true;
    mpdeData[mpdeIndex].attributes.rw = true;
    const Int64 offset = baseOffset-mpdeIndex*VM_GPDE_GRANULARITY;
    VmiMapMpde(description,&mpdeData[mpdeIndex],offset,paddr,vaddr,attr);
}

Void VmiMapMpde(INOUT const struct Description *description, INOUT struct MPDE* target, IN UIntPtr baseOffset ,IN UIntPtr paddr, IN UIntPtr vaddr, IN struct VMA attr) {
    if (INUUNLIKELY(target->address == 0)) {
        struct Payload* data = ObCreatePayload(1);
        ObVectorAdd(description->lpde,(struct Object*)data);
        target->address = (UInt64)data->address;
        VmTableInitialize((Void*)target->address);
    }
    const UIntPtr lpdeIndex = baseOffset/VM_LPDE_GRANULARITY;
    struct LPDE* lpdeData = (struct LPDE*)target->address;
    //lpdeData[lpdeIndex].attributes = attr;
    lpdeData[lpdeIndex].attributes.present = true;
    lpdeData[lpdeIndex].attributes.rw = true;
    const Int64 offset = baseOffset-lpdeIndex*VM_LPDE_GRANULARITY;
    VmiMapLpde(description,&lpdeData[lpdeIndex],offset,paddr,vaddr,attr);
}

Void VmiMapLpde(INOUT const struct Description *description, INOUT struct LPDE* target, IN UIntPtr baseOffset ,IN UIntPtr paddr, IN UIntPtr vaddr, IN struct VMA attr) {
    if (INUUNLIKELY(target->address == 0)) {
        struct Payload* data = ObCreatePayload(1);
        ObVectorAdd(description->pte,(struct Object*)data);
        target->address = (UInt64)data->address;
        VmTableInitialize((Void*)target->address);
    }
    const UIntPtr pteIndex = baseOffset/VM_LPTE_GRANULARITY;
    struct LPTE* lpteData = (struct LPTE*)target->address;
    lpteData[pteIndex].attributes = attr;
    const Int64 offset = baseOffset-pteIndex*VM_LPTE_GRANULARITY;
    VmiMapLpte(description,&lpteData[pteIndex],offset,paddr,vaddr,attr);
}

Void VmiMapLpte(INOUT const struct Description *description, INOUT struct LPTE* target, IN UIntPtr baseOffset ,IN UIntPtr paddr, IN UIntPtr vaddr, IN struct VMA attr) {
    if (target->address == 0) {
        struct Payload* data = ObCreatePayload(1);
        ObVectorAdd(description->pte,(struct Object*)data);
        target->address = (UInt64)data->address;
        VmTableInitialize((Void*)target->address);
    }
    const UIntPtr pfIndex = baseOffset/VM_PF_GRANULARITY;
    struct PF* pfData = (struct PF*)target->address;
    const Int64 offset = baseOffset-pfIndex*VM_PF_GRANULARITY;
    VmiMapPf(description,&pfData[pfIndex],offset,paddr,vaddr,attr);
}

Void VmiMapPf(INOUT const struct Description *description, INOUT struct PF* target, IN UIntPtr baseOffset, IN UIntPtr paddr, IN UIntPtr vaddr, IN struct VMA attr) {
    target->address = paddr;
    target->attributes = attr;
}

Void VmInitialize() {
    struct Description* mmap = VmCreateDescription();
    HalInitializeDescription(mmap);
    const struct VMA attributes = {
        .global = false,
        .present = true,
        .reserved = false,
        .rw = true,
        .us = false,
        .wt = true
    };
    UIntPtr loadTarget = 0x80000000;
    UIntPtr progress = 0;
    for (int i = 0; i < 524288; ++i) {
        VmMapDescription(mmap, progress, loadTarget, attributes);
        loadTarget += VM_PF_GRANULARITY;
        progress += VM_PF_GRANULARITY;
    }
    loadTarget = 0x80000000;
    progress = 0;
    for (int i = 0; i < 524288; ++i) {
        VmMapDescription(mmap, loadTarget, progress, attributes);
        loadTarget += VM_PF_GRANULARITY;
        progress += VM_PF_GRANULARITY;
    }
    VmKernelDescription = mmap;
    mmap->modified = true;
    HalSetDescription(mmap);
    FwIOInform(L"VM intialized!");
}

Void VmMapDescription(struct Description *target, const UIntPtr paddress, const UIntPtr vaddress, IN const struct VMA attr) {
    const UIntPtr gpdeIndex = paddress/VM_GPDE_GRANULARITY;
    if (INUUNLIKELY(target->gpde == NULL)) {
        struct Payload* data = ObCreatePayload(1);
        target->gpde = data;
        VmTableInitialize(target->gpde);
    }
    struct GPDE *global = target->gpde->address;
    //global[gpdeIndex].attributes = attr;
    global[gpdeIndex].attributes.present = true;
    global[gpdeIndex].attributes.rw = true;
    const Int64 offset = vaddress-gpdeIndex*VM_GPDE_GRANULARITY;
    VmiMapGpde(target,&global[gpdeIndex],offset,paddress,vaddress,attr);
}

Void VmTableInitialize(Void *target) {
    struct LPTE* addr = (struct LPTE*)target;
    for (int i = 0; i < VM_COUNT; ++i) {
        addr->address = 0;
    }
}


struct Description * VmCreateDescription() {
    struct Description* value = MmAllocateMemory(sizeof(struct Description));
    ObObjectInitialize(&value->object,sizeof(struct Description));
    value->gpde = Null;
    value->lpde = ObCreateVector();
    value->modified = false;
    value->mpde = ObCreateVector();
    value->global = Null;
    value->pte = ObCreateVector();
    value->nativePayload = ObCreateVector();
    return value;
}
