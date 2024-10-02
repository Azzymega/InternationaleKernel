#pragma once
#include <inuos.h>

#define MM_PAGE_SIZE 4096
#define MM_LINE_SIZE 32
#define BIT_PER_BYTE 8

typedef Void (*Event)(Void* object);
struct Payload;
struct Representation;
struct Descriptor;
struct Segment;
struct SegmentAttributes;
struct Mirror;

enum ArrayType : Byte {
    ATByte,
    ATShort,
    ATChar,
    ATInt,
    ATFloat,
    ATObject,
    ATDouble,
    ATLong,
    ATBool
};

struct Object {
    UIntPtr size;
    Boolean monitor;
    Boolean pinned;
    Void* type;
}
INUPACKED
INUOBJECT;

struct Allocator {
    struct Object object;
    Int64 lineCount;
    Int64 freeLines;
    Int64 allocatedCount;
    Byte *bitmap;
    struct Line *lines;
}
INUPACKED;

struct Array {
    struct Object object;
    Int32 length;
    Int32 elementLength;
    enum ArrayType type;

    INUNATIVE union {
        Byte byteValues[0];
        Int16 shortValues[0];
        Char charValues[0];
        Int32 intValues[0];
        Int64 longValues[0];
        Boolean booleanValues[0];
        Double doubleValues[0];
        Float floatValues[0];
        Void* referenceValues[0];
    };
}
INUPACKED
INUOBJECT;

struct MemoryManager {
    struct Object object;
    Int64 memorySize;
    Int64 pageCount;
    Int64 pageSize;
    INUNATIVE struct PageInformation *map;
    INUNATIVE struct Page *pages;
}
INUPACKED
INUOBJECT;

struct ControlLevel {
    Byte value;
}
INUNATIVE
INUPACKED
INUINLINEDNONOBJECT
INUINLINEDCOMPATIABLE;

struct Vector {
    struct Object object;
    Int32 count;
    Int32 allocated;
    struct Array* objects;
}
INUPACKED
INUOBJECT;

struct Signal {
    struct Object object;
    struct Vector* listeners;
    struct ControlLevel controlLevel;
    Boolean lock;
    Boolean executing;
    Boolean masked;
    struct Object* target;
    Event function;
}
INUPACKED
INUOBJECT;

struct String {
    struct Object object;
    Int32 length;
    struct Array* characters;
}
INUPACKED;
INUOBJECT;

struct Registry {
    struct Object object;
    struct String* string;
    struct Vector* objects;
    struct Vector* registers;
}
INUPACKED
INUOBJECT;

struct Fiber {
    struct Object object;
    struct Array* stack;
    struct Array* locals;
    struct Array* state;
    INUNATIVE Void* pointer;
}
INUPACKED
INUOBJECT;

struct Queue {
    struct Object object;
    Int64 first;
    Int64 last;
    struct Vector* internals;
};

Void ObInitialize();

Boolean ObIsMonitored(IN struct Object* target);

INUSAFE Void ObEnterMonitorObject(IN struct Object *target);

INUSAFE Void ObExitMonitorObject(IN struct Object *target);

Void ObVectorInitialize(INOUT struct Vector* target);

Void ObMirrorInitialize(INOUT struct Mirror *target,
                        IN UByte level,
                        IN UIntPtr pageSize);

Void ObPayloadInitialize(INOUT struct Payload* target,
                         Int64 pageCount);

Void ObPayloadFree(IN struct Payload* target);

Void ObPayloadFlush(INOUT struct Payload* target);

Void ObPayloadAlloc(INOUT struct Payload *target,
                    IN UIntPtr pageCount);

Void ObQueueInitialize(INOUT struct Queue* target);

Void ObFiberInitialize(INOUT struct Fiber *fiber,
                       IN Int32 stackSize,
                       IN Int32 localsSize,
                       IN Void *code);

Void ObObjectInitialize(INOUT struct Object *value,
                        IN UIntPtr size);

Void ObArrayInitialize(INOUT struct Array *value,
                       IN Int32 length,
                       IN enum ArrayType type);

Void ObRegistryInitialize(INOUT struct Registry* target, Char* name);

Void ObSignalInitialize(INOUT struct Signal *target,
                        IN const Event pointer,
                        IN Void *subscriber,
                        IN Boolean lock);

Void ObStringInitialize(INOUT struct String* value, struct Array* characters);

Void ObDescriptorInitialize(INOUT struct Descriptor* target);

Void ObSegmentInitialize(INOUT struct Segment *target,
                         IN struct SegmentAttributes attributes,
                         IN UInt64 base,
                         IN UInt64 end,
                         IN UInt64 vbase,
                         IN UInt64 vend);

Void ObMemoryManagerInitialize(INOUT struct MemoryManager *target,
                               IN Void *page,
                               IN Int64 memorySize,
                               IN Int64 pageCount);

Void ObAllocatorInitialize(INOUT struct Allocator *target,
                           IN Int64 rawSize);

Void ObRepresentationInititalize(INOUT struct Representation *target,
                                 IN UIntPtr controlLevel);

struct Descriptor* ObCreateDescriptor();

struct Segment *ObCreateSegment(IN struct SegmentAttributes attributes,
                                IN UIntPtr base,
                                IN UIntPtr end,
                                IN UIntPtr vbase,
                                IN UIntPtr vend);

struct Fiber *ObCreateFiber(IN Int32 stackSize,
                            IN Int32 localsSize,
                            IN Void *code);

struct Array *ObCreateArray(IN Int32 length,
                            IN enum ArrayType type);

struct String *ObCreateString(IN Char* characters);

struct Vector *ObCreateVector();

struct Mirror *ObCreateMirror(IN UByte level,
                              IN UIntPtr pageSize);

struct Queue *ObCreateQueue();

struct Representation *ObCreateRepresentation(IN UIntPtr controlLevel);

struct Payload *ObCreatePayload(IN Int64 pageCount);

struct Registry *ObCreateRegistry(Char* name);

struct Signal *ObCreateSignal(IN Event function,
                              IN struct Object *object,
                              IN Boolean lock,
                              IN Byte controlLevel);

INUMONITOR Void ObRegistryAdd(INOUT struct Registry *target,
                              IN struct Object *object);

INUMONITOR Void ObRegistryRemove(INOUT struct Registry *target,
                                 IN struct Object *object);

INUMONITOR Void ObVectorAdd(INOUT struct Vector *target,
                            IN struct Object *source);

INUMONITOR Void ObVectorClear(INOUT struct Vector *target);

INUMONITOR Void ObVectorSet(INOUT struct Vector *target,
                            IN struct Object *source,
                            IN Int32 index);

Boolean ObQueueEmpty(INOUT const struct Queue *target);

INUMONITOR Void ObQueuePush(INOUT struct Queue *target,
                            IN struct Object *source);

INUMONITOR struct Object* ObQueuePop(INOUT struct Queue *target);

INUMONITOR Void ObVectorRemove(INOUT struct Vector *target,
                    IN const struct Object *source);

INUMONITOR Void *ObVectorGet(INOUT const struct Vector *target,
                             IN Int32 index);

Void ObpVectorReallocate(INOUT struct Vector* target);
