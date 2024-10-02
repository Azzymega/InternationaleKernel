#pragma once
#include <executive/ob/api.h>
#include <executive/vm/api.h>

#define DEFAULT_STACK_SIZE 64000
#define DEFAULT_LOCALS_SIZE 1000
#define DEFAULT_QUANT_COUNT 5

// ПО CPU ID ПОЛУЧИТЬ ДАННЫЕ КОНТЕКСТА, ХОТЕЛ ИЗНАЧАЛЬНО В HAL'E СДЕЛАТЬ ВООБЩЕ!!!11

/**
 * Thread state as bit mask in managed environemnt!
 */
struct ThreadState {
    UByte us : 1;
    UByte executing : 1;
    UByte ready : 1;
    UByte suspended : 1;
    UByte io : 1;
    UByte reserved : 3;
}
INUNATIVE
INUPACKED
INUINLINEDCOMPATIABLE
INUINLINEDNONOBJECT;

struct Process {
    struct Object object;
    struct Description* space;
    struct Vector* threads;
    struct Vector* resources;
}
INUPACKED
INUOBJECT;

struct Thread {
    struct Object object;
    struct Process* owner;
    Byte priority;
    struct ThreadState information;
    Int64 processorId;
    Int64 startCycles;
    Int64 startSystemTime;
    Int64 cycles;
    struct Array* state;
    struct Array* stack;
    struct Array* kstack;
    struct Vector* messages;
}
INUPACKED
INUOBJECT;

struct Representation {
    Boolean interrupt;
    struct ControlLevel prev;
    struct ControlLevel level;
    Void* interruptFrame;
    UIntPtr cycles;
    UIntPtr interruptIndex;
    struct Process* currentProcess;
    struct Thread* currentThread;
}
INUNATIVE
INUPACKED;

Void PsInitialize();

INUNOINTERRUPT Void PsThreadExecute(struct Thread* target);
INUNOINTERRUPT Void PsThreadSuspend(struct Thread* target);
INUMONITOR Void PsAddProcess(struct Process* target);
struct Process* PsCreateProcess();

