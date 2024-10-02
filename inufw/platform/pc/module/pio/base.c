#include <pio/api.h>

void outb(UInt32 port, UByte data) {
    __asm__ volatile("outb %b0, %w1" : : "a" (data), "Nd" (port));
}

UByte inb(UInt32 port) {
    UByte data;
    __asm__ volatile("inb %w1, %b0" : "=a" (data) : "Nd" (port));
    return data;
}

void outw(UInt32 port, UInt16 data) {
    __asm__ volatile("outw %w0, %w1" : : "a" (data), "Nd" (port));
}

UInt16 inw(UInt32 port) {
    UInt16 data;
    __asm__ volatile("inw %w1, %w0" : "=a" (data) : "Nd" (port));
    return data;
}

void outdw(UInt32 port, UInt32 data) {
    __asm__ volatile("outl %0, %w1" : : "a" (data), "Nd" (port));
}

UInt32 indw(UInt32 port) {
    UInt32 data;
    __asm__ volatile("inl %w1, %0" : "=a" (data) : "Nd" (port));
    return data;
}
