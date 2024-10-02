#include <inufw/api.h>
#include <pio/api.h>
#include <ata/api.h>

#define SLEEP_PORT 0x80

INUSTATIC UIntPtr getbit(const UIntPtr n, const UIntPtr pos) {
    return ((n & (1 << pos)) != 0);
}

INUSTATIC UIntPtr clrbit(const UIntPtr n, const UIntPtr pos) {
    const UIntPtr reverse = ~(1 << pos);
    return (n & reverse);
}

INUSTATIC UIntPtr setbit(const UIntPtr number, const UIntPtr n) {
    return number | (1 << n);
}

INUSTATIC Boolean chkbit(const UIntPtr number, const UIntPtr n) {
    return (number >> n) & 1;
}

Void AtaReset() {
    UByte ctrlRegister = 0;
    UByte val = inb(PRIMARY_ATA_CR+1);
    ctrlRegister = setbit(ctrlRegister,2);
    outb(PRIMARY_ATA_CR,ctrlRegister);
    AtaIoWait();
    outb(PRIMARY_ATA_CR,0);
}

Void AtaSelectDrive(const Byte driveIndex) {
    if (driveIndex == 1) {
        UByte driveReg = inb(PRIMARY_ATA_DRIVE);
        driveReg = setbit(driveReg,4);
        AtaIoWait();
        outb(PRIMARY_ATA_DRIVE,driveReg);
    }
    else {
        UByte driveReg = inb(PRIMARY_ATA_DRIVE);
        driveReg = clrbit(driveReg,4);
        AtaIoWait();
        outb(PRIMARY_ATA_DRIVE,driveReg);
    }
}

Void AtaIdentify(const Byte driveIndex) {
    UInt16 buffer[ATA_PACKET] = {};
    AtaSelectDrive(driveIndex);
    AtaConfigureLoad(0,1);
    AtaSetCount(0);
    AtaSendCommand(ATA_COMMAND_IDENTIFY);
    AtaIn(buffer);
    AtaFlushCache();
}

Void AtaSetCount(const UInt16 count) {
    outw(PRIMARY_ATA_SCR,count);
    AtaWait();
}

Void AtaIoWait() {
    for (int i = 0; i < 10; ++i) {
        outb(SLEEP_PORT,0);
    }
}

Void AtaWait() {
    UByte status = inb(PRIMARY_ATA_STATUS);
    while (chkbit(status,7) && !chkbit(status,3)) {
        if (chkbit(status,0) || chkbit(status,5)) {
            FwEmergency(L"ATA DRIVE IS FAULTY!");
        }
        status = inb(PRIMARY_ATA_STATUS);
    }
}

Void AtaIn(UByte* buffer) {
    AtaWait();
    UInt16* newBuffer = (UInt16*)buffer;
    for (int i = 0; i < 256; ++i) {
        newBuffer[i] = inw(PRIMARY_ATA_DATA);
    }
}

Void AtaDisableIrq() {
    AtaSelectDrive(0);
    AtaIoWait();
    UByte target = 0;
    target = setbit(target,1);
    outb(PRIMARY_ATA_CR,target);
    AtaIoWait();
    AtaSelectDrive(1);
    AtaIoWait();
    outb(PRIMARY_ATA_CR,target);
    AtaIoWait();
    AtaSelectDrive(0);
    AtaIoWait();
}

Void AtaLocate() {

}

Void AtaSendCommand(const UByte command) {
    AtaWait();
    outb(PRIMARY_ATA_COMMAND,command);
    inb(PRIMARY_ATA_STATUS);
    AtaWait();
}

Void AtaSetDrive() {
    AtaIoWait();
    UByte target = inb(PRIMARY_ATA_DRIVE);
    AtaIoWait();
    target = setbit(target,6);
    AtaIoWait();
    outb(PRIMARY_ATA_DRIVE,target);
    AtaIoWait();
}

Void AtaFlushCache() {
    AtaSendCommand(ATA_COMMAND_FLUSH_CACHE);
}

// TODO FIXME FIX ATA DRIVE. CAN ONLY READ 1 SECTOR PER CALL!!!
Void AtaReadSector(const UInt64 lba, Void *buffer) {
    UByte *target = buffer;
    const UInt64 targetLba = lba;

    AtaWait();
    AtaFlushCache();
    AtaWait();
    AtaConfigureLoad(targetLba, 1);
    AtaWait();
    AtaSetDrive();
    AtaWait();
    if (targetLba < 0x01000000) {
        AtaSendCommand(ATA_COMMAND_READ);
    }
    else {
        AtaSendCommand(ATA_COMMAND_READ_EXT);
    }
    AtaWait();
    AtaIn(target);
    AtaWait();
}

Void AtaRead(const UIntPtr sectorCount, const UInt64 lba, Void *buffer) {
    UByte* target = buffer;
    UInt64 targetLba = lba;

    for (int i = 0; i < sectorCount; ++i) {
        AtaReadSector(targetLba++,target);
        target+=ATA_PACKET;
    }
}

Void AtaConfigureLoad(const UInt64 lba, const UInt16 sectorCount) {
    if (lba < 0x01000000) {
        outb(PRIMARY_ATA_SCR, sectorCount);
        outb(PRIMARY_ATA_LBALO, lba & 0xFF);
        outb(PRIMARY_ATA_LBAMID, lba >> 8 & 0xFF);
        outb(PRIMARY_ATA_LBAHI, lba >> 16 & 0xFF);
    }
    else {
        outb(PRIMARY_ATA_SCR, sectorCount >> 8 & 0xFF);
        outb(PRIMARY_ATA_LBALO, lba>>24 & 0xFF);
        outb(PRIMARY_ATA_LBAMID, lba>>32 & 0xFF);
        outb(PRIMARY_ATA_LBAHI, lba>>40 & 0xFF);
        outb(PRIMARY_ATA_SCR, sectorCount);
        outb(PRIMARY_ATA_LBALO, lba & 0xFF);
        outb(PRIMARY_ATA_LBAMID, lba >> 8 & 0xFF);
        outb(PRIMARY_ATA_LBAHI, lba >> 16 & 0xFF);
    }
}

Void AtaInitialize(const Byte driveIndex) {
    AtaReset();
    AtaDisableIrq();
    AtaIdentify(driveIndex);
    AtaSelectDrive(driveIndex);
}
