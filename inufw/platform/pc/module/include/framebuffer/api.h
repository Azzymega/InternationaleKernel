#pragma once
#include <inuos.h>

#define VBE_DISPI_IOPORT_INDEX 0x01CE
#define VBE_DISPI_IOPORT_DATA 0x01CF
#define VBE_DISPI_INDEX_ID 0
#define VBE_DISPI_INDEX_XRES 1
#define VBE_DISPI_INDEX_YRES 2
#define VBE_DISPI_INDEX_BPP 3
#define VBE_DISPI_INDEX_ENABLE 4
#define VBE_DISPI_INDEX_BANK 5
#define VBE_DISPI_INDEX_VIRT_WIDTH 6
#define VBE_DISPI_INDEX_VIRT_HEIGHT 7
#define VBE_DISPI_INDEX_X_OFFSET 8
#define VBE_DISPI_INDEX_Y_OFFSET 9

#define VBE_DISPI_BPP_4 0x04
#define VBE_DISPI_BPP_8 0x08
#define VBE_DISPI_BPP_15 0x0F
#define VBE_DISPI_BPP_16 0x10
#define VBE_DISPI_BPP_24 0x18
#define VBE_DISPI_BPP_32 0x20

Void FbInitialize();
UInt32 FbX();
UInt32 FbY();
Void* FbAddress();
