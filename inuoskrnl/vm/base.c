#include <inuhal/api.h>
#include <executive/ob/api.h>
#include <executive/mm/api.h>
#include <executive/rt/api.h>
#include <executive/ps/api.h>
#include <executive/vm/api.h>
#include <inufw/api.h>

INUGLOBAL struct Descriptor* VmKernelDescriptor;

Void VmInitialize() {
    VmKernelDescriptor = ObCreateDescriptor();
    const struct SegmentAttributes attributes = {
        .global = false,
        .present = true,
        .reserved = false,
        .rw = true,
        .us = false,
        .wt = true
    };
    VmReflectSegment(VmKernelDescriptor,attributes,0,0x80000000,0x80000000,0xFFFFFFFF);
    VmReflectSegment(VmKernelDescriptor,attributes,0x80000000,0x100000000,0,0x80000000);
    HalSetDescriptor(VmKernelDescriptor);
}

Void VmReflectSegment(const struct Descriptor *target, struct SegmentAttributes attributes, UIntPtr base, UIntPtr end, UIntPtr vbase, UIntPtr vend) {
    struct Segment* source = ObCreateSegment(attributes,base,end,vbase,vend);
    ObVectorAdd(target->segments,source);
}

Void VmReflectPage(struct Descriptor* target, UIntPtr pAddress, UIntPtr vAddress) {
    struct Mirror* entry = ObVectorGet(target->mirrors,0);
    UIntPtr pageSize = entry->pageSize;
    UIntPtr tableCount = HalVmTableCount();
    UIntPtr pmls = entry->level;
    UInt64 addressSpaceSize = 1;
    for (int i = 0; i < pmls; ++i) {
        addressSpaceSize*=tableCount;
    }
    addressSpaceSize*=pageSize;
}

UIntPtr VmSegmentSize(const struct Segment *target) {
    return target->base-target->end;
}

Void VmSegmentAttachPayload(struct Segment *target, struct Payload *source) {
    ObVectorAdd(target->payloads,source);
}

Void VmSortSegments(struct Descriptor *target) {
    UIntPtr segmentCount = target->segments->count;
    struct Segment* info[segmentCount];
    for (int i = 0; i < segmentCount; ++i) {
        info[i] = ObVectorGet(target->segments,i);
    }
    ObVectorClear(target->segments);
    for (int i = 0; i < segmentCount; ++i) {
        for (int z = 0; z < segmentCount-1; ++z) {
            if (info[z]->base > info[z+1]->base) {
                struct Segment* buff = info[z];
                info[z] = info[z+1];
                info[z+1] = buff;
            }
        }
    }
    for (int i = 0; i < segmentCount; ++i) {
        ObVectorAdd(target->segments,info[i]);
    }
}

struct Descriptor * VmCreateDescriptor() {
    return ObCreateDescriptor();
}
