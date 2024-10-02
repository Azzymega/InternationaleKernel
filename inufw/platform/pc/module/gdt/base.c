#include <gdt/api.h>

INUEXTERN Void NtFlushGdt(Void*);

INUSTATIC struct GDTEntry FwGdtEntryTable[DESCRIPTOR_COUNT];
INUSTATIC struct TSS FwKernelTss;
INUSTATIC struct GDTPointer FwGdtPointer;

INUSTATIC void GDTSetEntry(const Int32 index, const UInt32 base, const UInt32 limit, const UByte access, const UByte granularity) {
    FwGdtEntryTable[index].baseLow = base & 0xFFFF;
    FwGdtEntryTable[index].baseMiddle = (base >> 16) & 0xFF;
    FwGdtEntryTable[index].baseHigh = (base >> 24 & 0xFF);
    FwGdtEntryTable[index].limiteLow = limit & 0xFFFF;
    FwGdtEntryTable[index].granularity = (limit >> 16) & 0x0F;
    FwGdtEntryTable[index].access = access;
    FwGdtEntryTable[index].granularity = FwGdtEntryTable[index].granularity | (granularity & 0xF0);
}

void GdtInitialize() {
    FwGdtPointer.limit = sizeof(FwGdtEntryTable) - 1;
    FwGdtPointer.base = (UInt32)FwGdtEntryTable;

    GDTSetEntry(0, 0, 0, 0, 0);
    GDTSetEntry(1, 0, 0xFFFFFFFF, 0x9A, 0xCF);
    GDTSetEntry(2, 0, 0xFFFFFFFF, 0x92, 0xCF);
    GDTSetEntry(3, 0, 0xFFFFFFFF, 0xFA, 0xCF);
    GDTSetEntry(4, 0, 0xFFFFFFFF, 0xF2, 0xCF);
    GDTSetEntry(5, (UInt32)&FwKernelTss, (UInt32)((char*)&FwKernelTss)+sizeof(struct TSS), 0x9E, 0);

    asm volatile ("lgdt %0" : "=m" (FwGdtPointer));
    NtFlushGdt(&FwGdtPointer);
}
