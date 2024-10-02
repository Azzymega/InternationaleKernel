#pragma once
#include <inuos.h>

#define DESCRIPTOR_COUNT 8

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

Void GdtInitialize();