#pragma once
#include <inuos.h>
#include <executive/ob/api.h>

#define ENTRY_SIZE 64
#define VM_COUNT 512

#define VM_PF_GRANULARITY 4096
#define VM_LPTE_GRANULARITY 2097152
#define VM_LPDE_GRANULARITY 1073741824
#define VM_MPDE_GRANULARITY 549755813888
#define VM_GPDE_GRANULARITY 281474976710656

struct VMA {
    UInt16 present : 1;
    UInt16 rw : 1;
    UInt16 us : 1;
    UInt16 global : 1;
    UInt16 wt : 1;
    UInt16 cd : 1;
    UInt16 reserved : 10;
}
INUNATIVE
INUPACKED
INUINLINEDCOMPATIABLE
INUINLINEDNONOBJECT
INUPAYLOADTARGET;

struct PF {
    UInt64 address: 48;
    struct VMA attributes;
}
INUPACKED
INUNATIVE;

struct LPTE {
    UInt64 address: 48;
    struct VMA attributes;
}
INUPACKED
INUNATIVE;

struct LPDE {
    UInt64 address: 48;
    struct VMA attributes;
}
INUPACKED
INUNATIVE;

struct MPDE {
    UInt64 address: 48;
    struct VMA attributes;
}
INUPACKED
INUNATIVE;

struct GPDE {
    UInt64 address: 48;
    struct VMA attributes;
}
INUPACKED
INUNATIVE;

struct Description {
    struct Object object;
    Boolean modified;
    struct Payload *global;
    struct Vector *nativePayload;
    struct Payload *gpde;
    struct Vector *mpde;
    struct Vector *lpde;
    struct Vector *pte;
}
INUPACKED
INUOBJECT;

Void VmInitialize();

struct Description* VmCreateDescription();

Void INUFASTCALL VmMapDescription(INOUT struct Description *target,
                      IN UIntPtr paddress,
                      IN UIntPtr vaddress,
                      IN struct VMA attr);

Void VmTableInitialize(INOUT Void* target);