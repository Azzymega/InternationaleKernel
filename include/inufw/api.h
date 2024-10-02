#pragma once
#include <inuos.h>

struct Catalogue;
struct Signal;
struct UmSegment;

Void FwInitialize();

struct Representation* FwGetContext();
Byte FwGetControlLevel();

Void FwMaskSignal(UIntPtr index);
Void FwUnmaskSignal(UIntPtr index);
UIntPtr FwGetCpuId();
UIntPtr FwGetCycles();
UIntPtr FwGetCpuCount();
Void FwRegisterSignal(struct Signal* target, UIntPtr index);

Void FwRegisterTimer(struct Signal* target);

Void FwRaiseControlLevel(Byte value);
Void FwLowerControlLevel(Byte value);

UIntPtr FwReadFileLength(const char* name);
Void FwReadFile(const char* name, Void* buffer);

Boolean FwCheckFile(Void* buffer);
Void FwLoadFile(Void* buffer);
Void FwJumpFileEntry(Void* buffer, struct Catalogue* data);

Void FwExecuteSignal(UIntPtr index);
Void FwBeginSignal(UIntPtr index);
Void FwEndSignal(UIntPtr index);

Void* FwPMMStart();
UIntPtr FwPMMSize();

Void FwIOPrint(const Char* string);
Void FwIOPrintLetter(Char character);
Void FwIOPrintLine(const Char* string);
Void FwIOPrintNumerical(UIntPtr value);
Void FwIOInform(const Char* string);

Void FwStub(const Char *message);
Void FwEmergency(const Char* message);
INUBADAPI INUREPLACE Int32 FwDebugPrint(const AChar* restrict format, ...);

// EXPERIMENTAL API!

struct UmSegment* FwInitialKernelSegment();
UIntPtr FwKernelLoadAddress();
Void FwTrampoline(Void* bufferData);

// NEXGEN PROJECT

enum FwMessageType {
    FW_WARNING,
    FW_ERROR,
    FW_STUB,
    FW_INFO
};

struct FwApplicationProcessorBlock {
    UIntPtr index;
};

Int32 FwIoPrint(enum FwMessageType type, const AChar* restrict format, ...);

