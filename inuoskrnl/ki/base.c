#include <inufw/api.h>
#include <inuhal/api.h>
#include <executive/ob/api.h>
#include <executive/mm/api.h>
#include <executive/rt/api.h>
#include <executive/ps/api.h>
#include <executive/ki/api.h>

INUGLOBAL UIntPtr KiSystemTime;

INUSTATIC Boolean Initialized = false;

INUIMPORT struct Array* FwRepresentations;
INUEXTERN struct Signal* FwSignalHandler;

INUGLOBAL struct Signal* KiGPF;
INUGLOBAL struct Signal* KiPF;
INUGLOBAL struct Signal* KiSC;
INUGLOBAL struct Signal* KiILI;
INUGLOBAL struct Signal* KiBP;
INUGLOBAL struct Signal* KiClock;
INUGLOBAL struct Signal* KiShedule;

INUGLOBAL struct Queue *KiDPC;
INUGLOBAL struct Queue *KiDI;
INUGLOBAL struct Payload* KiKS;

#define KERNEL_STACK_SIZE 65536
#define SYSCALL_INDEX 0xE2
#define SHEDULE_INDEX 0xE3

Void KiReshedule(Void* dummy) {
    if (!ObQueueEmpty(KiDI)) {
        KiSignalDefferExecute();
    }

    if (!ObQueueEmpty(KiDPC)) {
        FwRaiseControlLevel(CL_SIGNALS);
        KiSignalResolve();
    }

    KiSelectProcess();
}

Void KiSelectProcess() {
    FwEmergency(L"STUB");
}

Void KiSignalMask(struct Signal *target) {
    target->masked = true;
}

Void KiSignalUnmask(struct Signal *target) {
    target->masked = false;
}

Void KiSignalResolve() {
    struct Signal* dpc = ObQueuePop(KiDPC);
    while (dpc != Null) {
        KiSignalInvoke(dpc);
        dpc = ObQueuePop(KiDPC);
    }
}

Void KiSignalDefferExecute() {
    struct Signal* interrupt = ObQueuePop(KiDI);
    while (interrupt != Null) {
        FwRaiseControlLevel(interrupt->controlLevel.value);
        KiSignalInvoke(interrupt);
        interrupt = ObQueuePop(KiDI);
    }
}

Void KiSignalInvoke(struct Signal *target) {
    //FwDebugPrint("Invoking sighandler to signal %x with CL %i with obj %x, fun %x \r\n",target,target->controlLevel.value,target->object,target->function);
    if (target->lock) {
        ObEnterMonitorObject(&target->object);
    }

    if (target->function != Null) {
        target->function(target->target);
    }

    for (int i = 0; i < target->listeners->count; ++i) {
        struct Signal* data = ObVectorGet(target->listeners,i);
        KiSignalInvoke(data);
    }

    ObExitMonitorObject(&target->object);
}

Void KiSignalShedule(struct Signal *target) {
    ObQueuePush(KiDPC,target);
}

Void KiSignalDeffer(struct Signal *target) {
    ObQueuePush(KiDI,target);
}

Void KiClockHandler(Void *object) {
    /* TODO Think about dialectical representation of time. Maybe it work better.
    TODO World is spinning only when CPU out of Reality Management Mode. (Refer to INTERNATIONALE RMM LIBRARY) */
    KiSystemTime++;
    const struct Representation* target = FwGetContext();
    if (target->currentThread != Null) {
        if (target->currentThread->startSystemTime <= ((KiSystemTime+DEFAULT_QUANT_COUNT)*target->currentThread->priority)) {
            FwLowerControlLevel(CL_SHEDULE);
        }
    }
}

Void KiGPFHandler(Void* object) {
    FwEmergency(L"GPF!");
}

Void KiPFHandler(Void* object) {
    FwEmergency(L"PF!");
}

Void KiSCHandler(Void* object) {
    FwEmergency(L"SC!");
}

Void KiILIHandler(Void* object) {
    FwEmergency(L"Illegal opcode!");
}

Void KiSignalAddListener(const struct Signal *target, struct Signal *source) {
    ObVectorAdd(target->listeners,source);
}

Void KiSignalRemoveListener(const struct Signal *target, struct Signal *source) {
    ObVectorRemove(target->listeners,source);
}

Void KiInitialize() {
    if (!Initialized) {
        FwSignalHandler = ObCreateSignal((Void(*)(Void *object))KiSignalInvoke,Null,true,CL_EXCEPTION);
        KiGPF = ObCreateSignal(&KiGPFHandler,Null,true,CL_EXCEPTION);
        KiPF = ObCreateSignal(&KiPFHandler,Null,true,CL_EXCEPTION);
        KiSC = ObCreateSignal(&KiSCHandler,Null,true,CL_EXCEPTION);
        KiILI = ObCreateSignal(&KiILIHandler,Null,true,CL_EXCEPTION);
        KiBP = ObCreateSignal(Null,Null,true,CL_EXCEPTION);
        KiShedule = ObCreateSignal(Null,Null,true,CL_SHEDULE);
        KiClock = ObCreateSignal(&KiClockHandler,Null,true,CL_CLOCK);
        KiDPC = ObCreateQueue();
        KiKS = ObCreatePayload(KERNEL_STACK_SIZE/MM_PAGE_SIZE);
        FwRepresentations = ObCreateArray(FwGetCpuCount(),ATObject);

        FwRegisterSignal(KiGPF,13);
        FwRegisterSignal(KiPF,14);
        FwRegisterSignal(KiSC,SYSCALL_INDEX);
        FwRegisterSignal(KiILI,6);
        FwRegisterSignal(KiBP,3);
        FwRegisterSignal(KiShedule,SHEDULE_INDEX);
        FwRegisterTimer(KiClock);
        Initialized = true;
    }
    KiInitializeProcessor();
}

Void KiInitializeProcessor() {
    struct Representation* target = ObCreateRepresentation(CL_INITIALIZATION);
    FwRepresentations->referenceValues[FwGetCpuId()] = target;
}
