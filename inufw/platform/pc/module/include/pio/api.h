#pragma once
#include <inuos.h>

void outb(UInt32 port, UByte data);
UByte inb(UInt32 port);
void outw(UInt32 port, UInt16 data);
UInt16 inw(UInt32 port);
void outdw(UInt32 port, UInt32 data);
UInt32 indw(UInt32 port);