#include <inufw/api.h>
#include <inuldr/api.h>
#include <executive/rt/api.h>
#include <inuos.h>
#include <inuhal/api.h>

struct X86PMRegisters {
    UInt32 ds;
    UInt32 edi;
    UInt32 esi;
    UInt32 ebp;
    UInt32 esp;
    UInt32 ebx;
    UInt32 edx;
    UInt32 ecx;
    UInt32 eax;
    UInt32 intIndex;
    UInt32 errIndex;
    UInt32 eip;
    UInt32 cs;
    UInt32 eflags;
    UInt32 sp;
    UInt32 ss;
}
INUPACKED
INUARCHDEP;

struct GDTEntry {
    UInt16 limiteLow;
    UInt16 baseLow;
    UByte baseMiddle;
    UByte access;
    UByte granularity;
    UByte baseHigh;
}
INUPACKED
INUARCHDEP;

struct GDTPointer {
    UInt16 limit;
    UInt32 base;
}
INUPACKED
INUARCHDEP;

struct TSS {
    UInt32 prevTss;
    UInt32 esp0;
    UInt32 ss0;
    UInt32 esp1;
    UInt32 ss1;
    UInt32 esp2;
    UInt32 ss2;
    UInt32 cr3;
    UInt32 eip;
    UInt32 eflags;
    UInt32 eax;
    UInt32 ecx;
    UInt32 edx;
    UInt32 ebx;
    UInt32 esp;
    UInt32 ebp;
    UInt32 esi;
    UInt32 edi;
    UInt32 es;
    UInt32 cs;
    UInt32 ss;
    UInt32 ds;
    UInt32 fs;
    UInt32 gs;
    UInt32 ldt;
    UInt16 trap;
    UInt16 iomap;
}
INUPACKED
INUARCHDEP;

struct PDE4 {
    UInt32 present : 1;
    UInt32 rw : 1;
    UInt32 us : 1;
    UInt32 writeThroughCache : 1;
    UInt32 cacheDisable : 1;
    UInt32 accessed : 1;
    UInt32 dirty : 1;
    UInt32 pageSize : 1;
    UInt32 global : 1;
    UInt32 reserved : 23;
}
INUPACKED
INUNATIVE;

INUGLOBAL INUSTATIC UInt32 pt[1024] __attribute__((aligned(4096)));

INUEXTERN Void NTEnablePSE();

#define DESCRIPTOR_COUNT 8

INUNATIVECALL Void NTLoadMMU(Void* table);
INUNATIVECALL Void NTEnableMMU();
INUNATIVECALL Void NTDisableMMU();
INUEXTERN Void NTFlushGDT(Void*);

struct GDTEntry FwGdtEntryTable[DESCRIPTOR_COUNT];
struct TSS FwKernelTss;
struct GDTPointer FwGdtPointer;

void GDTSetEntry(const Int32 index, const UInt32 base, const UInt32 limit, const UByte access, const UByte granularity) {
    FwGdtEntryTable[index].baseLow = base & 0xFFFF;
    FwGdtEntryTable[index].baseMiddle = (base >> 16) & 0xFF;
    FwGdtEntryTable[index].baseHigh = (base >> 24 & 0xFF);
    FwGdtEntryTable[index].limiteLow = limit & 0xFFFF;
    FwGdtEntryTable[index].granularity = (limit >> 16) & 0x0F;
    FwGdtEntryTable[index].access = access;
    FwGdtEntryTable[index].granularity = FwGdtEntryTable[index].granularity | (granularity & 0xF0);
}

void InitializeGDT() {
    FwGdtPointer.limit = sizeof(FwGdtEntryTable) - 1;
    FwGdtPointer.base = (UInt32)FwGdtEntryTable;

    GDTSetEntry(0, 0, 0, 0, 0);
    GDTSetEntry(1, 0, 0xFFFFFFFF, 0x9A, 0xCF);
    GDTSetEntry(2, 0, 0xFFFFFFFF, 0x92, 0xCF);
    GDTSetEntry(3, 0, 0xFFFFFFFF, 0xFA, 0xCF);
    GDTSetEntry(4, 0, 0xFFFFFFFF, 0xF2, 0xCF);
    GDTSetEntry(5, (UInt32)&FwKernelTss, (UInt32)((char*)&FwKernelTss)+sizeof(struct TSS), 0x9E, 0);

    asm volatile ("lgdt %0" : "=m" (FwGdtPointer));
    NTFlushGDT(&FwGdtPointer);
}

Void BtldrMapMmu() {
    NTEnablePSE();
    const struct PDE4 target = {
        .present = true,
        .accessed = false,
        .rw = true,
        .us = false,
        .writeThroughCache = false,
        .cacheDisable = false,
        .dirty = false,
        .pageSize = true,
        .global = false,
        .reserved = 0,
    };
    UIntPtr address = 0;
    UInt32 attributes = 0;
    memcpy(&attributes,&target,sizeof(struct PDE4));
    for (int i = 0; i < 1024; ++i) {
        if (address == 0x80000000) {
            address = 0;
        }
        FwDebugPrint("Mapping virtual address %x to phyiscal %x \r\n",i*4194304,address);
        pt[i] = address | attributes;
        address+=4194304;
    }
    NTLoadMMU(&pt);
    NTEnableMMU();
}

struct Catalogue tableData;

Void BtldrInitialize(UIntPtr magic, Void* table) {
    HalInitialize(Null);
    tableData.stackLocation = (Void*)0x80300000;
    tableData.stackSize = 0x100000;
    tableData.loadOffset = 0x80000000;
    UIntPtr base = 0x80100000;
    Void* stack = (Void*)base;
    FwInitialize();
    FwIOPrintLine(L"INTERNATIONALE LOADER V1.0");
    Byte buffer[FwReadFileLength("inuoskrnl/inuoskrnl")];
    FwReadFile("inuoskrnl/inuoskrnl",buffer);
    Boolean valid = FwCheckFile(buffer);
    if (!valid) {
        FwEmergency(L"INUOSKRNL IS DAMAGED!");
    }
    FwIOPrintLine(L"Loading INUOSRNKL!");
    BtldrMapMmu();
    FwLoadFile(buffer);
    FwJumpFileEntry(buffer,&tableData);
}

