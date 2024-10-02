#pragma once
#include <executive/ob/api.h>

#define CL_BOOTSTRAP 0
#define CL_SHEDULE 3
#define CL_SIGNALS 2
#define CL_PASSIVE 1
#define CL_EXCEPTION 31
#define CL_CLOCK 30
#define CL_INITIALIZATION 32

INUEXTERN UIntPtr KiSystemTime;

INUEXTERN struct Signal* KiGPF;
INUEXTERN struct Signal* KiPF;
INUEXTERN struct Signal* KiSC;
INUEXTERN struct Signal* KiILI;
INUEXTERN struct Signal* KiBP;
INUEXTERN struct Signal* KiShedule;
INUEXTERN struct Signal* KiTimer;

INUNOINTERRUPT Void KiReshedule(Void* object);

Void KiSelectProcess();

Void KiSignalMask(INOUT struct Signal* target);

Void KiSignalUnmask(INOUT struct Signal* target);

Void KiSignalResolve();

Void KiSignalDefferExecute();

INUSAFE Void KiSignalInvoke(INOUT struct Signal *target);

INUSAFE Void KiSignalShedule(INOUT struct Signal* target);

INUSAFE Void KiSignalDeffer(INOUT struct Signal* target);

Void KiClockHandler(Void* object);

Void KiSignalAddListener(INOUT const struct Signal *target,
                         IN struct Signal *source);

Void KiSignalRemoveListener(INOUT const struct Signal *target,
                            IN struct Signal *source);

Void KiInitialize();

Void KiInitializeProcessor();

