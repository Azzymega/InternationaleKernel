#include <inuhal/api.h>
#include <executive/mm/api.h>
#include <executive/rt/api.h>
#include <executive/nt/api.h>
#include <executive/ps/api.h>
#include <inufw/api.h>

#define VECTOR_INITIAL_SIZE 10

Void ObObjectInitialize(struct Object* target, UIntPtr size) {
    target->size = size;
    target->monitor = false;
    target->pinned = true;
    target->type = Null;
}

struct Vector * ObCreateVector() {
    struct Vector* target = MmAllocateMemory(sizeof(struct Vector));
    ObVectorInitialize(target);
    return target;
}

struct Registry * ObCreateRegistry(Char* name) {
    struct Registry* target = MmAllocateMemory(sizeof(struct Registry));
    ObRegistryInitialize(target,name);
    return target;
}

struct Payload * ObCreatePayload(const Int64 pageCount) {
    struct Payload* target = MmAllocateMemory(sizeof(struct Payload));
    ObPayloadInitialize(target,pageCount);
    return target;
}

struct Queue * ObCreateQueue() {
    struct Queue* target = MmAllocateMemory(sizeof(struct Queue));
    ObQueueInitialize(target);
    return target;
}

struct Signal * ObCreateSignal(Event function, struct Object *object, Boolean lock, Byte controlLevel) {
    struct Signal* target = MmAllocateMemory(sizeof(struct Signal));
    ObSignalInitialize(target,function,object,lock);
    target->controlLevel.value = controlLevel;
    return target;
}

struct Representation * ObCreateRepresentation(UIntPtr controlLevel) {
    struct Representation* target = MmAllocateMemory(sizeof(struct Signal));
    ObRepresentationInititalize(target,controlLevel);
    return target;
}

struct Mirror * ObCreateMirror(UByte level, UIntPtr pageSize) {
    struct Mirror* target = MmAllocateMemory(sizeof(struct Mirror));
    ObMirrorInitialize(target,level,pageSize);
    return target;
}

Void ObRegistryAdd(struct Registry *target, struct Object *object) {
    ObEnterMonitorObject(&target->object);
    ObVectorAdd(target->objects,object);
    ObExitMonitorObject(&target->object);
}

Void ObRegistryRemove(struct Registry *target, struct Object *object) {
    ObEnterMonitorObject(&target->object);
    ObVectorRemove(target->objects,object);
    ObExitMonitorObject(&target->object);
}

Void ObVectorAdd(struct Vector *target, struct Object *source) {
    ObEnterMonitorObject(&target->object);
    Realloc:
    if (target->count + 1 >= target->allocated) {
        ObpVectorReallocate(target);
        goto Realloc;
    }
    target->objects->referenceValues[target->count++] = source;
    ObExitMonitorObject(&target->object);
}

Void ObVectorClear(struct Vector *target) {
    ObEnterMonitorObject(&target->object);
    target->count = 0;
    ObExitMonitorObject(&target->object);
}

Void ObVectorSet(struct Vector *target, struct Object *source, Int32 index) {
    ObEnterMonitorObject(&target->object);
    Realloc:
    if (index >= target->allocated) {
        ObpVectorReallocate(target);
        goto Realloc;
    }
    target->objects->referenceValues[index] = source;
    target->count = index;
    ObExitMonitorObject(&target->object);
}

Boolean ObQueueEmpty(const struct Queue *target) {
    if (target->internals->count == 0) {
        return true;
    }
    return false;
}

Void ObQueuePush(struct Queue *target, struct Object *source) {
    ObEnterMonitorObject(&target->object);
    ObVectorAdd(target->internals,source);
    if (target->internals->count == 1) {

    }
    else {
        target->last++;
    }
    ObExitMonitorObject(&target->object);
}

struct Object * ObQueuePop(struct Queue *target) {
    ObEnterMonitorObject(&target->object);

    if (target->internals->count == 0) {
        ObExitMonitorObject(&target->object);
        return Null;
    }
    struct Object* obj = ObVectorGet(target->internals,target->first);
    target->last--;
    ObVectorRemove(target->internals,obj);
    ObExitMonitorObject(&target->object);
    return obj;
}

Void ObVectorRemove(struct Vector *target, const struct Object *source) {
    ObEnterMonitorObject(&target->object);
    for (int i = 0; i < target->count; ++i) {
        if (target->objects->referenceValues[i] == source) {
            Void* ptr = target->objects->referenceValues[i];
            memcpy(ptr, ptr + sizeof(UIntPtr),target->count - (i + 1) * sizeof(UIntPtr)); // TODO FIXME BAD?
            return;
        }
    }
    ObExitMonitorObject(&target->object);
}

Void * ObVectorGet(const struct Vector *target, const Int32 index) {
    return target->objects->referenceValues[index];
}

Void ObpVectorReallocate(struct Vector *target) {
    struct Array *newArray = ObCreateArray(ATObject, target->allocated * 2);
    memcpy(newArray->referenceValues, target->objects->referenceValues, sizeof(UIntPtr) * target->count);
    //delete this->objects; TODO CLEAR MEMORY FROM REALLOCATION!
    target->objects = newArray;
    target->allocated *= 2;
}

Void ObFiberInitialize(struct Fiber* fiber, const Int32 stackSize, const Int32 localsSize, Void* code) {
    fiber->locals = ObCreateArray(localsSize, ATInt);
    fiber->stack = ObCreateArray(stackSize, ATInt);
    fiber->state = ObCreateArray(HalStateSize(),ATByte);
    fiber->pointer = code;
    ObObjectInitialize(&fiber->object,sizeof(struct Fiber));
}

Void ObEnterMonitorObject(struct Object *target) {
    HalEnterMonitor(&target->monitor);
}

Void ObExitMonitorObject(struct Object *target) {
    HalExitMonitor(&target->monitor);
}

Void ObInitialize() {

}

Boolean ObIsMonitored(struct Object *target) {
    return target->monitor;
}

Void ObVectorInitialize(struct Vector *target) {
    ObObjectInitialize(&target->object,sizeof(struct Vector));
    target->count = 0;
    target->allocated = VECTOR_INITIAL_SIZE;
    target->objects = ObCreateArray(VECTOR_INITIAL_SIZE,ATObject);
}

Void ObMirrorInitialize(struct Mirror *target, UByte level, UIntPtr pageSize) {
    ObObjectInitialize(&target->object,sizeof(struct Mirror));
    target->root = true;
    target->level = level;
    target->index = 0;
    target->entrySize = 0;
    target->pageSize = pageSize;
    target->addressOffset = 0;
    target->lowerLevels = ObCreateVector();
}

Void ObPayloadInitialize(struct Payload* target, Int64 pageCount) {
    ObObjectInitialize(&target->object,sizeof(struct Payload));
    target->pageCount = pageCount;
    Void* pageAddress = MmAllocatePayload(pageCount);
    target->pageIndex = MmGetPadeIndex(pageAddress);
    target->address = pageAddress;
}

Void ObPayloadFree(struct Payload *target) {
    MmFreePayload(target->pageIndex,target->pageCount);
    MmFreeMemory(target);
}

Void ObPayloadFlush(struct Payload *target) {
    if (target->pageIndex != 0 && target->pageCount > 0) {
        MmFreePayload(target->pageIndex,target->pageCount);
    }
    target->pageIndex = 0;
    target->pageCount = 0;
    target->address = Null;
}

Void ObPayloadAlloc(struct Payload *target, UIntPtr pageCount) {
    target->pageCount = pageCount;
    Void* pageAddress = MmAllocatePayload(pageCount);
    memset(pageAddress,INUFREESPACE,pageCount*MM_PAGE_SIZE);
    target->pageIndex = MmGetPadeIndex(pageAddress);
    target->address = pageAddress;
}

Void ObQueueInitialize(struct Queue *target) {
    ObObjectInitialize(&target->object,sizeof(struct Queue));
    target->first = 0;
    target->last = 0;
    target->internals = ObCreateVector();
}

Void ObArrayInitialize(struct Array *value, Int32 length, enum ArrayType type) {
    UIntPtr finalSize = 0;
    UIntPtr elementSize = 0;

    switch (type) {
        case ATByte:
            elementSize = sizeof(Byte);
            break;
        case ATShort:
            elementSize = sizeof(Int16);
            break;
        case ATChar:
            elementSize = sizeof(Char);
            break;
        case ATInt:
            elementSize = sizeof(Int32);
            break;
        case ATFloat:
            elementSize = sizeof(Float);
            break;
        case ATObject:
            elementSize = sizeof(UIntPtr);
            break;
        case ATDouble:
            elementSize = sizeof(Double);
            break;
        case ATLong:
            elementSize = sizeof(Int64);
            break;
        case ATBool:
            elementSize = sizeof(Boolean);
            break;
    }

    finalSize = sizeof(struct Array)+(elementSize*length);
    ObObjectInitialize(&value->object,finalSize);

    value->length = length;
    value->elementLength = elementSize;
    value->type = type;

    memset(value->byteValues,INUFREESPACE,sizeof(length*elementSize));
}

Void ObRegistryInitialize(struct Registry *target, Char *name) {
    ObObjectInitialize(&target->object,sizeof(struct Registry));
    target->objects = ObCreateVector();
    target->registers = ObCreateVector();
    target->string = ObCreateString(name);
}

Void ObStringInitialize(struct String *value, struct Array *characters) {
    ObObjectInitialize(&value->object,sizeof(struct String));
    value->characters = characters;
    value->length = characters->length;
}

Void ObDescriptorInitialize(struct Descriptor *target) {
    ObObjectInitialize(&target->payload.object,sizeof(struct Descriptor));
    target->segments = ObCreateVector();
    target->mirrors = ObCreateVector();
}

Void ObSegmentInitialize(struct Segment *target, struct SegmentAttributes attributes, UInt64 base, UInt64 end,
                         UInt64 vbase, UInt64 vend) {
    ObObjectInitialize(&target->object,sizeof(struct Segment));
    target->attributes = attributes;
    target->base = base;
    target->end = end;
    target->vbase = vbase;
    target->vend = vend;
    target->payloads = ObCreateVector();
}

Void ObSignalInitialize(INOUT struct Signal* target, const Event pointer, Void* subscriber, Boolean lock) {
    ObObjectInitialize(&target->object,sizeof(struct Signal));
    target->lock = lock;
    target->function = pointer;
    target->target = subscriber;
    target->masked = false;
    target->listeners = ObCreateVector();
}

struct Array * ObCreateArray(Int32 length, enum ArrayType type) {
    UIntPtr finalSize = 0;
    UIntPtr elementSize = 0;

    switch (type) {
        case ATByte:
            elementSize = sizeof(Byte);
        break;
        case ATShort:
            elementSize = sizeof(Int16);
        break;
        case ATChar:
            elementSize = sizeof(Char);
        break;
        case ATInt:
            elementSize = sizeof(Int32);
        break;
        case ATFloat:
            elementSize = sizeof(Float);
        break;
        case ATObject:
            elementSize = sizeof(UIntPtr);
        break;
        case ATDouble:
            elementSize = sizeof(Double);
        break;
        case ATLong:
            elementSize = sizeof(Int64);
        break;
        case ATBool:
            elementSize = sizeof(Boolean);
        break;
    }
    finalSize = sizeof(struct Array)+(elementSize*length);

    struct Array* target = MmAllocateMemory(finalSize);
    ObArrayInitialize(target,length,type);

    return target;
}

Void ObMemoryManagerInitialize(struct MemoryManager *target, Void *page, const Int64 memorySize, const Int64 pageCount) {
    ObObjectInitialize(&target->object,sizeof(struct MemoryManager));
    target->pages = page;
    target->memorySize = memorySize;
    target->pageSize = MM_PAGE_SIZE;
    target->pageCount = pageCount;
    const UIntPtr difference = (UIntPtr)target->pages-(UIntPtr)target->map;

    memset(target->map,INUFREESPACE,difference);
}

Void ObAllocatorInitialize(struct Allocator *target, Int64 rawSize) {
    UIntPtr size = rawSize;

    ObObjectInitialize(&target->object,size);
    size-=sizeof(struct Allocator);

    UIntPtr allocatorLineLength = sizeof(struct Allocator)/MM_LINE_SIZE;
    if (sizeof(struct Allocator)%MM_LINE_SIZE != 0) {
        allocatorLineLength++;
    }

    Void* firstLine = target;
    firstLine+=allocatorLineLength*MM_LINE_SIZE;

    UIntPtr freeLineCount = rawSize/MM_LINE_SIZE;
    freeLineCount-=allocatorLineLength;
    UIntPtr bitmapLength = freeLineCount/BIT_PER_BYTE;
    UIntPtr bitmapLineLength = bitmapLength/MM_LINE_SIZE;
    if (bitmapLength % MM_LINE_SIZE != 0) {
        bitmapLineLength++;
    }

    UIntPtr dataLineCount = freeLineCount-bitmapLineLength;
    target->bitmap = firstLine;
    firstLine+=bitmapLineLength*MM_LINE_SIZE;
    target->lines = firstLine;
    target->lineCount = dataLineCount;
    target->freeLines = dataLineCount;
    target->allocatedCount = 0;
    memset(target->bitmap,INUFREESPACE,bitmapLineLength*MM_LINE_SIZE);
    memset(target->lines,INUFREESPACE,dataLineCount*MM_LINE_SIZE);
}

Void ObRepresentationInititalize(struct Representation *target, UIntPtr controlLevel) {
    target->interrupt = false;
    target->level.value = controlLevel;
    target->interruptFrame = Null;
    target->interruptIndex = 0;
    target->currentProcess = Null;
    target->currentThread = Null;
}

struct Segment *ObCreateSegment(struct SegmentAttributes attributes, UIntPtr base, UIntPtr end, UIntPtr vbase,
                                UIntPtr vend) {
    struct Segment* segment = MmAllocateMemory(sizeof(struct Segment));
    ObSegmentInitialize(segment,attributes,base,end,vbase,vend);
    return segment;
}

struct Descriptor * ObCreateDescriptor() {
    struct Descriptor* descriptor = MmAllocateMemory(sizeof(struct Descriptor));
    ObDescriptorInitialize(descriptor);
    return descriptor;
}

struct Fiber * ObCreateFiber(const Int32 stackSize, const Int32 localsSize, Void *code) {
    struct Fiber* fiber = MmAllocateMemory(sizeof(struct Fiber));
    ObFiberInitialize(fiber,stackSize,localsSize,code);
    return fiber;
}

struct String * ObCreateString(Char *characters) {
    UIntPtr length = wstrlen(characters);
    struct Array* charactersArray = ObCreateArray(length,ATChar);
    memcpy(characters,charactersArray->charValues,sizeof(Char)*length);
    struct String* str = MmAllocateMemory(sizeof(struct String));
    ObStringInitialize(str,charactersArray);
}
