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

Void * ElfLoadRelocatableImage(Void *file) {
    UIntPtr sectionHeadersCount = ElfReturnSectionHeadersCount(file);
    UIntPtr programHeadersCount = ElfReturnProgramHeadersCount(file);

    struct Elf32SectionHeader* sectionHeaders = ElfReturnSectionHeaders(file);
    struct ELF32ProgramHeader* programHeaders = ElfReturnProgramHeaders(file);

    for (int i = 0; i < sectionHeadersCount; ++i) {
        struct Elf32SectionHeader currentHeader = sectionHeaders[i];


    }

    return Null;
}

Void * ElfLoadFile(Void *file) {
    struct ELF32 *target = file;
    for (int i = 0; i < target->e_phnum; ++i) {
        UIntPtr header = (UIntPtr)target;
        header+=target->e_phoff;
        header+=(i*sizeof(struct ELF32ProgramHeader));
        ElfLoadSection(file,(struct ELF32ProgramHeader*)header);
    }
    return Null;
}

UIntPtr ElfReturnProgramHeadersCount(const Void *file) {
    const struct ELF32* target = file;
    return target->e_phnum;
}

Void * ElfReturnProgramHeaders(Void *file) {
    struct ELF32* target = file;
    UIntPtr address = (UIntPtr)target;
    address+=target->e_phoff;
    return (Void*)address;
}

Void * ElfReturnProgramHeader(Void *file, const UIntPtr index) {
    Void* targetAddress = ElfReturnProgramHeaders(file);
    targetAddress+=(index*sizeof(struct ELF32ProgramHeader));
    return targetAddress;
}

UIntPtr ElfReturnSectionHeadersCount(const Void *file) {
    const struct ELF32* target = file;
    return target->e_shnum;
}

Void * ElfReturnSectionHeaders(Void *file) {
    struct ELF32* target = file;
    UIntPtr address = (UIntPtr)target;
    address+=target->e_shoff;
    return (Void*)address;
}

Void * ElfReturnSectionHeader(Void *file, const UIntPtr index) {
    Void* targetAddress = ElfReturnSectionHeaders(file);
    targetAddress+=(index*sizeof(struct Elf32SectionHeader));
    return targetAddress;
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

