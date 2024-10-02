#include <executive/ki/api.h>
#include <executive/ob/api.h>
#include <executive/ps/api.h>
#include <executive/rt/api.h>
#include <inufw/api.h>
#include <inuhal/api.h>
#include <stdarg.h>
#include <limits.h>

#include "../../module/include/de/de.h"
#include "module/include/ata/api.h"
#include "module/include/elf/api.h"
#include "module/include/framebuffer/api.h"
#include "module/include/gdt/api.h"
#include "module/include/int/api.h"
#include "module/include/pic/api.h"
#include "module/include/pit/api.h"
#include "module/include/tar/api.h"

#define IRQ_COUNT 16
#define IRQ_START 0x20
#define SIGNAL_COUNT 256

INUGLOBAL struct Signal* FwSignals[256];
INUGLOBAL struct Array* FwRepresentations;
INUGLOBAL struct Signal* FwSignalHandler;

struct Representation * FwGetContext() {
    return FwRepresentations->referenceValues[FwGetCpuId()];
}

Void FwInitialize() {
    GdtInitialize();
    AtaInitialize(0);
    IntInitialize();
    FbInitialize();
    DeInitialize();
    IntInitialize();
    PicInitialize();
    PitInitialize();
}

Byte FwGetControlLevel() {
    return FwGetContext()->level.value;
}

Void FwMaskSignal(const UIntPtr index) {
    FwSignals[index]->masked = true;
    if (index >= 0x20 && index <= 0x28) {
        PicMaskIrq(index);
    }
}

Void FwUnmaskSignal(const UIntPtr index) {
    FwSignals[index]->masked = false;
    if (index >= 0x20 && index <= 0x28) {
        PicUnmaskIrq(index);
    }
}

UIntPtr FwGetCpuId() {
    return 0;
}

UIntPtr FwGetCycles() {
    unsigned hi, lo;
    __asm__ __volatile__ ("rdtsc" : "=a"(lo), "=d"(hi));
    return ( (unsigned long long)lo)|( ((unsigned long long)hi)<<32 );
}

UIntPtr FwGetCpuCount() {
    return 1;
}

Void FwRegisterSignal(struct Signal *target, UIntPtr index) {
    FwSignals[index] = target;
}

Void FwRegisterTimer(struct Signal *target) {
    FwRegisterSignal(target,IRQ_START);
}

Void FwRaiseControlLevel(const Byte value) {
    //FwDebugPrint("CL raised to %i from %i \r\n",value,FwGetContext()->level.value);
    FwGetContext()->level.value = value;
    for (int i = 0; i < SIGNAL_COUNT; ++i) {
        if (FwSignals[i] != Null) {
            if (FwSignals[i]->controlLevel.value < value) {
                FwMaskSignal(i);
            }
        }
    }
}

Void FwLowerControlLevel(const Byte value) {
    //FwDebugPrint("CL lowered to %i from %i \r\n",value,FwGetContext()->level.value);
    FwGetContext()->level.value = value;
    for (int i = 0; i < SIGNAL_COUNT; ++i) {
        if (FwSignals[i] != Null) {
            if (FwSignals[i]->controlLevel.value > value) {
                FwUnmaskSignal(i);
            }
        }
    }
}

UIntPtr FwReadFileLength(const char* name) {
    return TarReadFileLength(name);
}

Void FwReadFile(const char* name, Void* buffer) {
    TarReadFile(name,buffer);
}

Boolean FwCheckFile(Void *buffer) {
    return ElfCheckFile(buffer);
}

Void FwLoadFile(Void* buffer) {
    ElfLoadFile(buffer);
}

Void FwJumpFileEntry(Void* buffer, struct Catalogue* table) {
    ElfJumpEntry(buffer,table);
}

Void FwExecuteSignal(const UIntPtr index) {
    FwBeginSignal(index);
    if (FwSignalHandler != Null) {
        FwSignalHandler->function(FwSignals[index]);
    }
    FwEndSignal(index);
}

Void FwBeginSignal(const UIntPtr index) {
    struct Representation* target = FwGetContext();
    target->prev = target->level;
    target->interruptIndex = index;
    target->cycles = FwGetCycles();
    FwRaiseControlLevel(FwSignals[index]->controlLevel.value);
    FwSignals[index]->executing = true;
}

Void FwEndSignal(UIntPtr index) {
    FwSignals[index]->executing = false;
    struct Representation* target = FwGetContext();
    target->cycles = FwGetCycles();
    target->interruptIndex = -1;
    if (index >= 0x20 && index <= 0x28) {
        PicSendEoi(index);
    }
    FwLowerControlLevel(target->prev.value);
}

Void * FwPMMStart() {
    return (Void*)0x80600000;
}

UIntPtr FwPMMSize() {
    return 0x20000000;
}

Void FwIOPrint(const Char *string) {
    DeDrawString(string);
}

Void FwIOPrintLetter(const Char character) {
    if (character == '\r') {
        DeReturnCarette();
    }
    else if (character == '\n') {
        DeSkipLine();
    }
    else {
        DeDrawLetter(character);
    }
}

Void FwIOPrintLine(const Char *string) {
    DeDrawString(string);
    DeDrawString(L"\r\n");
}

Void FwIOPrintNumerical(UIntPtr value) {
    char buffer[100];
    const char* name = itoa(value,buffer,10);
    Char bufferWide[100];
    const UIntPtr length = strlen(name);
    for (UIntPtr i = 0; i < length; ++i) {
        bufferWide[i] = buffer[i];
    }
    bufferWide[length] = '\0';
    FwIOPrint(bufferWide);
}

Void FwIOInform(const Char *string) {
    FwIOPrint(L"[INFORMATION] ");
    FwIOPrintLine(string);
}

Void FwStub(const Char *message) {
    FwIOPrint(L"[STUB] ");
    FwEmergency(message);
}

Void FwEmergency(const Char *message) {
    FwIOPrint(L"[CRASH] ");
    FwIOPrintLine(message);
    HalDisableSignals();
    HalHalt();
}

INUBADAPI int putchar(int ic) {
    char c = (char) ic;
    FwIOPrintLetter(c);
    return ic;
}

INUBADAPI static bool print(const char* data, size_t length) {
    const unsigned char* bytes = (const unsigned char*) data;
    for (size_t i = 0; i < length; i++)
        if (putchar(bytes[i]) == -1)
            return false;
    return true;
}

Int32 FwDebugPrint(const AChar *format, ...) {
    FwIOPrint(L"[DEBUG] ");
    va_list parameters;
    va_start(parameters, format);

    int written = 0;

    while (*format != '\0') {
        size_t maxrem = INT_MAX - written;

        if (format[0] != '%' || format[1] == '%') {
            if (format[0] == '%')
                format++;
            size_t amount = 1;
            while (format[amount] && format[amount] != '%')
                amount++;
            if (maxrem < amount) {
                // TODO: Set errno to EOVERFLOW.
                return -1;
            }
            if (!print(format, amount))
                return -1;
            format += amount;
            written += amount;
            continue;
        }

        const char* format_begun_at = format++;

        if (*format == 'c') {
            format++;
            char c = (char) va_arg(parameters, int /* char promotes to int */);
            if (!maxrem) {
                // TODO: Set errno to EOVERFLOW.
                return -1;
            }
            if (!print(&c, sizeof(c)))
                return -1;
            written++;
        } else if (*format == 's') {
            format++;
            const char* str = va_arg(parameters, const char*);
            size_t len = strlen(str);
            if (maxrem < len) {
                // TODO: Set errno to EOVERFLOW.
                return -1;
            }
            if (!print(str, len))
                return -1;
            written += len;
        } else if (*format == 'i') {
            format++;
            int value = va_arg(parameters, int);
            char buff[100];
            char* str = itoa(value,buff,10);
            size_t len = strlen(str);
            if (maxrem < len) {
                // TODO: Set errno to EOVERFLOW.
                return -1;
            }
            if (!print(str, len))
                return -1;
            written += len;
        } else if (*format == 'x') {
            format++;
            int value = va_arg(parameters, int);
            char buff[100];
            char* str = ultoa(value,buff,16);
            size_t len = strlen(str);
            if (maxrem < len) {
                // TODO: Set errno to EOVERFLOW.
                return -1;
            }
            if (!print(str, len))
                return -1;
            written += len;
        } else {
            format = format_begun_at;
            size_t len = strlen(format);
            if (maxrem < len) {
                // TODO: Set errno to EOVERFLOW.
                return -1;
            }
            if (!print(format, len))
                return -1;
            written += len;
            format += len;
        }
    }

    va_end(parameters);
    return written;
}

UIntPtr FwKernelLoadAddress() {
    return 0x80000000;
}

Void FwTrampoline(Void *bufferData) {

}

Int32 FwIoPrint(enum FwMessageType type, const AChar *format, ...) {
    switch (type) {
        case FW_WARNING:
            FwIOPrint(L"[WARNING] ");
            break;
        case FW_ERROR:
            FwIOPrint(L"[ERROR] ");
            break;
        case FW_STUB:
            FwIOPrint(L"[STUB] ");
            break;
        case FW_INFO:
            FwIOPrint(L"[INFORMATION] ");
            break;
    }
    va_list parameters;
    va_start(parameters, format);

    int written = 0;

    while (*format != '\0') {
        size_t maxrem = INT_MAX - written;

        if (format[0] != '%' || format[1] == '%') {
            if (format[0] == '%')
                format++;
            size_t amount = 1;
            while (format[amount] && format[amount] != '%')
                amount++;
            if (maxrem < amount) {
                // TODO: Set errno to EOVERFLOW.
                return -1;
            }
            if (!print(format, amount))
                return -1;
            format += amount;
            written += amount;
            continue;
        }

        const char* format_begun_at = format++;

        if (*format == 'c') {
            format++;
            char c = (char) va_arg(parameters, int /* char promotes to int */);
            if (!maxrem) {
                // TODO: Set errno to EOVERFLOW.
                return -1;
            }
            if (!print(&c, sizeof(c)))
                return -1;
            written++;
        } else if (*format == 's') {
            format++;
            const char* str = va_arg(parameters, const char*);
            size_t len = strlen(str);
            if (maxrem < len) {
                // TODO: Set errno to EOVERFLOW.
                return -1;
            }
            if (!print(str, len))
                return -1;
            written += len;
        } else if (*format == 'i') {
            format++;
            int value = va_arg(parameters, int);
            char buff[100];
            char* str = itoa(value,buff,10);
            size_t len = strlen(str);
            if (maxrem < len) {
                // TODO: Set errno to EOVERFLOW.
                return -1;
            }
            if (!print(str, len))
                return -1;
            written += len;
        } else if (*format == 'x') {
            format++;
            int value = va_arg(parameters, int);
            char buff[100];
            char* str = ultoa(value,buff,16);
            size_t len = strlen(str);
            if (maxrem < len) {
                // TODO: Set errno to EOVERFLOW.
                return -1;
            }
            if (!print(str, len))
                return -1;
            written += len;
        } else {
            format = format_begun_at;
            size_t len = strlen(format);
            if (maxrem < len) {
                // TODO: Set errno to EOVERFLOW.
                return -1;
            }
            if (!print(format, len))
                return -1;
            written += len;
            format += len;
        }
    }

    va_end(parameters);
    return written;
}

