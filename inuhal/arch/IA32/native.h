#pragma once
#include <inuos.h>

INUSAFE INUSTATIC void outb(UInt32 port, UByte data) {
    __asm__ volatile("outb %b0, %w1" : : "a" (data), "Nd" (port));
}

INUSAFE INUSTATIC UByte inb(UInt32 port) {
    UByte data;
    __asm__ volatile("inb %w1, %b0" : "=a" (data) : "Nd" (port));
    return data;
}

INUSAFE INUSTATIC void outw(UInt32 port, UInt16 data) {
    __asm__ volatile("outw %w0, %w1" : : "a" (data), "Nd" (port));
}

INUSAFE INUSTATIC UInt16 inw(UInt32 port) {
    UInt16 data;
    __asm__ volatile("inw %w1, %w0" : "=a" (data) : "Nd" (port));
    return data;
}

INUSAFE INUSTATIC void outdw(UInt32 port, UInt32 data) {
    __asm__ volatile("outl %0, %w1" : : "a" (data), "Nd" (port));
}

INUSAFE INUSTATIC UInt32 indw(UInt32 port) {
    UInt32 data;
    __asm__ volatile("inl %w1, %0" : "=a" (data) : "Nd" (port));
    return data;
}

INUSAFE INUSTATIC Boolean AtomicCompareExchange(Boolean *pointer, UInt32 compare, const UInt32 exchange) {
    return __atomic_compare_exchange_n(pointer,&compare, exchange, false,__ATOMIC_SEQ_CST,
                                       __ATOMIC_SEQ_CST);
}

INUSAFE INUSTATIC void AtomicStore(Boolean *pointer, const UInt32 value) {
    __atomic_store_n(pointer, value,__ATOMIC_SEQ_CST);
}

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

UInt32 HaliConvertVmaToPde(struct VMA attr);
UInt32 HaliConvertVmaToPte(struct VMA attr);