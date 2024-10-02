#include <framebuffer/api.h>
#include <inufw/api.h>
#include <inuhal/api.h>
#include <pio/api.h>

Void vbeWrite(const UInt16 target, const UInt16 value) {
    outw(VBE_DISPI_IOPORT_INDEX, target);
    outw(VBE_DISPI_IOPORT_DATA, value);
}

UInt16 vbeRead(const UInt16 target) {
    outw(VBE_DISPI_IOPORT_INDEX, target);
    return inw(VBE_DISPI_IOPORT_DATA);
}

Void FbInitialize() {

    // Bochs VBE extensions hack code!!! TODO DELETE THIS!
    if (vbeRead(VBE_DISPI_INDEX_ID) != 0xB0C5) {
        FwEmergency(L"Failed to get framebuffer!");
    }

    vbeWrite(VBE_DISPI_INDEX_ENABLE,0);
    vbeWrite(VBE_DISPI_INDEX_XRES,1024);
    vbeWrite(VBE_DISPI_INDEX_YRES,768);
    vbeWrite(VBE_DISPI_INDEX_BPP,VBE_DISPI_BPP_32);
    vbeWrite(VBE_DISPI_INDEX_ENABLE,0x41);
}

UInt32 FbX() {
    return 1024;
}

UInt32 FbY() {
    return 768;
}

Void * FbAddress() {
    return HalPhysicalToVirtual((Void*)0xE0000000);
}

