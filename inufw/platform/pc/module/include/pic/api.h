#pragma once
#include <inuos.h>

#define ICW1_ICW4 0x01
#define ICW1_SINGLE 0x02
#define ICW1_INTERVAL4 0x04
#define ICW1_LEVEL 0x08
#define ICW1_INIT 0x10

#define ICW4_8086 0x01
#define ICW4_AUTO 0x02
#define ICW4_BUF_SLAVE 0x08
#define ICW4_BUF_MASTER 0x0C
#define ICW4_SFNM 0x10

#define PIC1 0x20
#define PIC2 0xA0
#define PIC1_COMMAND PIC1
#define PIC1_DATA (PIC1+1)
#define PIC2_COMMAND PIC2
#define PIC2_DATA (PIC2+1)

#define PIC_EOI	0x20
#define PIC_READ_IRR 0x0a
#define PIC_READ_ISR 0x0b

Void PicInitialize();

Boolean PicCheckInterrupt(UIntPtr irq);
UInt16 PicGetIrr();
UInt16 PicGetIsr();
Void PicMaskIrq(UIntPtr irq);
Void PicUnmaskIrq(UIntPtr irq);
Void PicSendEoi(UIntPtr irq);
Void PicRemap(UIntPtr firstOffset, UIntPtr secondOffset);
Void PicDisable();
INUSAFE Int16 PicGetIrqIndex();