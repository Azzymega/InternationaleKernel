#include <executive/cm/api.h>
#include <inufw/api.h>
#include <inuhal/api.h>
#include <executive/ob/api.h>
#include <executive/mm/api.h>
#include <executive/rt/api.h>
#include <executive/ps/api.h>

#define PS_NON_EXECUTING -1

INUSTATIC const Char* ModuleName = L"Ps";
INUGLOBAL struct Queue* ThreadCarousel;
INUGLOBAL struct Vector* Threads;

Void PsInitialize() {
    CmRegisterNamespace(ModuleName);
    ThreadCarousel = ObCreateQueue();
    CmRegisterObject(ThreadCarousel,ModuleName);
    Threads = ThreadCarousel->internals;
    CmRegisterObject(Threads,ModuleName);
}

Void PsThreadExecute(struct Thread *target) {
    target->priority++;
    target->processorId = FwGetCpuId();
    target->information.ready = false;
    target->information.executing = true;
    HalSetState(target);
}

Void PsThreadSuspend(struct Thread *target) {
    target->information.executing = false;
    target->priority--;
    target->processorId = PS_NON_EXECUTING;
}

Void PsAddProcess(struct Process *target) {
    ObEnterMonitorObject(&target->object);
    for (int i = 0; i < target->threads->count; ++i) {
        ObQueuePush(ThreadCarousel,ObVectorGet(target->threads,i));
    }
    ObExitMonitorObject(&target->object);
}
