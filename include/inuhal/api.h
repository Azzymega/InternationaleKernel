#pragma once
#include <inuos.h>
#include <inuldr/api.h>

struct Thread;
struct Signal;
struct Descriptor;
struct Description;

Void HalInitialize(const struct Catalogue* table);
Void HalHalt();

Void HalDisableSignals();
Void HalEnableSignals();
UIntPtr HalInterruptCount();

Void HalInitializeDescription(INOUT struct Description* target);

Void HalRemapBootloader();

UIntPtr HalVmTableCount();

Void* HalVirtualToPhysical(Void* virtualAddress);
Void* HalPhysicalToVirtual(Void* physicalAddress);

Void HalEnterMonitor(Boolean* monitor);
Void HalExitMonitor(Boolean* monitor);

Void HalSetState(const struct Thread* target);
INUBADAPI INUNAKED Void HalSwitchStack(Void* ptr);
INUSTACKINTENSIVE Void HalSetDescriptor(struct Descriptor* target);
Void HalSetDescription(struct Description* target);

Boolean HalEndianess();

Void HalDisableVM();
Void HalEnableVM();

UInt64 HalMaxPM();
UInt64 HalMinPM();

UIntPtr HalStateSize();