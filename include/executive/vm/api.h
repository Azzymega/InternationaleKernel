#pragma once
#include <inuos.h>
#include <executive/nt/api.h>

// TODO IMPLEMENT VM WITH A REPLACE BOOTSTRAP PAGE TABLE!!!

#define LPS 4096
#define BPS 4194304

/**
 * Segment attributes
 */
struct SegmentAttributes {
    UByte present : 1;
    UByte rw : 1;
    UByte us : 1;
    UByte global : 1;
    UByte wt : 1;
    UByte reserved : 3;
}
INUNATIVE
INUPACKED
INUINLINEDCOMPATIABLE
INUINLINEDNONOBJECT
INUPAYLOADTARGET;

/**
 * Segment type TODO CLEAR HACKFIX!
 */
struct Segment {
    struct Object object;
    struct SegmentAttributes attributes;
    UInt64 base;
    UInt64 end;
    UInt64 vbase;
    UInt64 vend;
    struct Vector* payloads;
}
INUPACKED
INUOBJECT;

struct Mirror {
    struct Object object;
    Boolean root;
    UByte level; // We can have now max only 255 page map levels
    UInt64 index;
    UInt64 pageSize;
    UInt64 entrySize; // TODO FIXME Must be 4096 aligned! By now only 4096!
    UInt64 addressOffset;
    struct SegmentAttributes attributes;
    struct Vector* lowerLevels;
    struct Payload* raw;
}
INUPACKED
INUOBJECT;

/**
 * Payload type, that stores native system paging table
 */
struct Descriptor {
    struct Payload payload;
    struct Vector* segments;
    struct Vector* mirrors;
}
INUPACKED
INUOBJECT;

Void VmInitialize();

struct Descriptor* VmCreateDescriptor();

Void VmReflectSegment(INOUT const struct Descriptor *target,
                      IN struct SegmentAttributes attributes,
                      IN UIntPtr base,
                      IN UIntPtr end,
                      IN UIntPtr vbase,
                      IN UIntPtr vend);

Void VmReflectPage(INOUT struct Descriptor* target,
                   IN UIntPtr pAddress,
                   IN UIntPtr vAddress);

UIntPtr VmSegmentSize(IN const struct Segment* target);

Void VmSegmentAttachPayload(INOUT struct Segment *target,
                            IN struct Payload *source);

Void VmSortSegments(INOUT struct Descriptor* target);

Void VmAddMirror(struct Descriptor* target, UByte level, UIntPtr pageSize);