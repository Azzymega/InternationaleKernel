#include <framebuffer/api.h>
#include <inufw/api.h>
#include <pio/api.h>
#include <ata/api.h>
#include <executive/rt/api.h>
#include <tar/api.h>

#define SLEEP_PORT 0x80
#define TAR_SIZE_LENGTH 12
#define TAR_SECTOR 512

INUSTATIC UInt32 getTarSize(const char *in)
{
    UInt32 size = 0;
    UInt32 j;
    UInt32 count = 1;

    for (j = TAR_SIZE_LENGTH-1; j > 0; j--, count *= 8)
        size += ((in[j - 1] - '0') * count);

    return size;
}

UIntPtr TarLocateFileSector(const char *name) {
    UIntPtr sector = 1;
    while (true) {
        Byte buffer[TAR_SECTOR] = {};
        AtaReadSector(sector,buffer); // FIX ATA DRIVER!
        struct TarHeader* header = (struct TarHeader*)buffer;
        FwDebugPrint("Finded file %s \r\n",header->filename);
        if (strstr(header->filename,name)) {
            return sector+1;
        }
        const UIntPtr fileSize = getTarSize(header->size);
        sector += ((fileSize / TAR_SECTOR) + 1);
        if (fileSize % TAR_SECTOR) {
            sector++;
        }
    }
}

UIntPtr TarReadFileLength(const char *name) {
    UIntPtr sector = 0;
    while (true) {
        Byte buffer[TAR_SECTOR] = {};
        AtaReadSector(sector,buffer); // FIX ATA DRIVER!
        struct TarHeader* header = (struct TarHeader*)buffer;
        FwDebugPrint("Located file %s \r\n",header->filename);
        if (strstr(header->filename,name)) {
            UIntPtr fileSize = getTarSize(header->size);
            UIntPtr windCount = fileSize/TAR_SECTOR;
            if (fileSize % TAR_SECTOR != 0) {
                windCount++;
            }
            return windCount*TAR_SECTOR;
        }
        const UIntPtr fileSize = getTarSize(header->size);
        sector += ((fileSize / TAR_SECTOR) + 1);
        if (fileSize % TAR_SECTOR) {
            sector++;
        }
    }
}

Void TarReadFile(IN const char* name,INOUT Byte* buffer) {
    UIntPtr sectorIndex = TarLocateFileSector(name);
    UIntPtr length = TarReadFileLength(name);
    UIntPtr sectorLength = length/TAR_SECTOR;
    AtaRead(sectorLength,sectorIndex,buffer);
}
