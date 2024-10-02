#include <ata/api.h>
#include <pic/api.h>
#include <pio/api.h>

#define SLEEP_PORT 0x80

static UInt16 PicGetIrqInfo(const int ocw3)
{
    outb(PIC1_COMMAND, ocw3);
    outb(PIC2_COMMAND, ocw3);
    return (inb(PIC2_COMMAND) << 8) | inb(PIC1_COMMAND);
}

UInt16 PicGetIrr()
{
    return PicGetIrqInfo(PIC_READ_IRR);
}

UInt16 PicGetIsr()
{
    return PicGetIrqInfo(PIC_READ_ISR);
}

Void PicInitialize() {
    PicRemap(0x20,0x28);
}

Boolean PicCheckInterrupt(const UIntPtr irq) {
    const UInt16 data = PicGetIsr();
    if (data & (1 << irq)) {
        return true;
    }
    return false;
}

Void PicMaskIrq(UIntPtr irq) {
    uint16_t port = 0;

    if(irq < 8) {
        port = PIC1_DATA;
    }
    else {
        port = PIC2_DATA;
        irq -= 8;
    }

    const uint8_t value = inb(port) | (1 << irq);
    outb(port, value);
}

Void PicUnmaskIrq(UIntPtr irq) {
    uint16_t port = 0;

    if(irq < 8) {
        port = PIC1_DATA;
    }
    else {
        port = PIC2_DATA;
        irq -= 8;
    }

    const uint8_t value = inb(port) & ~(1 << irq);
    outb(port, value);
}

Void PicSendEoi(const UIntPtr irq) {
    if(irq >= 8) {
        outb(PIC2_COMMAND,PIC_EOI);
    }
    outb(PIC1_COMMAND,PIC_EOI);
}

Void PicIoWait() {
    for (int i = 0; i < 10; ++i) {
        outb(SLEEP_PORT,0);
    }
}

Void PicRemap(const UIntPtr firstOffset, const UIntPtr secondOffset) {
    const UByte firstMask = inb(PIC1_DATA);
    const UByte secondMask = inb(PIC2_DATA);

    outb(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);
    PicIoWait();
    outb(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
    PicIoWait();
    outb(PIC1_DATA, firstOffset);
    PicIoWait();
    outb(PIC2_DATA, secondOffset);
    PicIoWait();
    outb(PIC1_DATA, 4);
    PicIoWait();
    outb(PIC2_DATA, 2);
    PicIoWait();
    outb(PIC1_DATA, ICW4_8086);
    PicIoWait();
    outb(PIC2_DATA, ICW4_8086);
    PicIoWait();

    outb(PIC1_DATA, firstMask);
    outb(PIC2_DATA, secondMask);
}

Void PicDisable() {
    outb(PIC1_DATA, 0xff);
    outb(PIC2_DATA, 0xff);
}

INUSAFE Int16 PicGetIrqIndex() {
    return PicGetIsr();
}
