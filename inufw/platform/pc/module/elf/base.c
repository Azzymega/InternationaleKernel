#include <framebuffer/api.h>
#include <inufw/api.h>
#include <pio/api.h>
#include <ata/api.h>
#include <elf/api.h>
#include <executive/rt/api.h>
#include <inuldr/api.h>

Boolean ElfCheckFile(const Void* file) {
    const struct ELF32* target = file;
    const UByte magic[] = {0x7F,'E','L','F'};
    if (memcmp(target,magic,sizeof(magic)) == 0) {
        return true;
    }
    return false;
}

Void * ElfLoadFile(Void *file) {
    struct ELF32 *target = file;
    for (int i = 0; i < target->e_phnum; ++i) {
        ElfLoadSection(file,&target->headers[i]);
    }
    return Null;
}

Void ElfLoadSection(Void* file,struct ELF32ProgramHeader *header) {
    if (header->header == PTLoad) {
        memset((AChar*)header->virtualAddress,0,header->memSize);
        memcpy((Void*)header->virtualAddress,file+header->offset,header->fileSize);
    }
}

Void ElfJumpEntry(Void *file, struct Catalogue *table) {
    const struct ELF32* target = file;
    void (*inuyashaEntry)(struct Catalogue*) = (void (*)(struct Catalogue*))target->e_entry;
    inuyashaEntry(table);
}

