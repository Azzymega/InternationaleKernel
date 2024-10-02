#pragma once
#include <inuos.h>

struct Catalogue;

typedef UInt16 Elf32_Half;
typedef UInt32 Elf32_Off;
typedef UInt32 Elf32_Addr;
typedef UInt32 Elf32_Word;
typedef Int32  Elf32_Sword;
typedef UInt64 Elf64_Addr;
typedef UInt64 Elf64_Off;

enum ProgramHeaderType : Elf32_Word {
    PTNull = 0,
    PTLoad = 1,
    PTDynamic = 2,
    PTInterp = 3,
    PTNote = 4,
    PTBaseHeader = 6
};

struct ELFBaseHeader {
    UByte magic[4]; // (%del)ELF magic symbols
    UByte type; // 1 for 32, 2 for 64
    UByte endianess; // 1 for LE, 2 for BE
    UByte version; // only 1
    UByte osabi; // TODO choose subsystem there? good thought
    UByte abiver;
    UByte reserved[7];
}
INUPACKED
INUNATIVE;

struct ELFExtendedHeader {
    UInt16 type;
    UInt16 machine;
    UInt32 version;
}
INUPACKED
INUNATIVE;

enum Elf32ShType : Elf32_Word {
    SHT_NULL = 0,
    SHT_PROGBITS = 1,
    SHT_SYMTAB = 2,
    SHT_STRTAB = 3,
    SHT_RELA = 4,
    SHT_NOBITS = 8,
    SHT_REL = 9,
};

enum Elf32ShAttribute : Elf32_Word {
    SHF_WRITE = 0x01,
    SHF_ALLOC = 0x02
};

struct ELF32ProgramHeader {
    enum ProgramHeaderType header;
    Elf32_Addr offset;
    Elf32_Addr virtualAddress;
    Elf32_Addr physAddress;
    Elf32_Word fileSize;
    Elf32_Word memSize;
    Elf32_Word flags;
    Elf32_Word align;
}
INUPACKED
INUNATIVE;

struct ELF32 {
    struct ELFBaseHeader base;
    struct ELFExtendedHeader extend;
    Elf32_Addr e_entry;
    Elf32_Off e_phoff;
    Elf32_Off e_shoff;
    Elf32_Word e_flags;
    Elf32_Half e_ehsize;
    Elf32_Half e_phentsize;
    Elf32_Half e_phnum;
    Elf32_Half e_shentsize;
    Elf32_Half e_shnum;
    Elf32_Half e_shstrndx;
}
INUPACKED
INUNATIVE;

struct Elf32SectionHeader {
    Elf32_Word sh_name;
    enum Elf32ShType sh_type;
    enum Elf32ShAttribute sh_flags;
    Elf32_Addr sh_addr;
    Elf32_Off sh_offset;
    Elf32_Word sh_size;
    Elf32_Word sh_link;
    Elf32_Word sh_info;
    Elf32_Word sh_addralign;
    Elf32_Word sh_entsize;
}
        INUPACKED
INUNATIVE;

Boolean ElfCheckFile(const Void* file);
Void* ElfLoadRelocatableImage(Void* file);
Void* ElfLoadFile(Void* file);

UIntPtr ElfReturnProgramHeadersCount(const Void* file);
Void* ElfReturnProgramHeaders(Void* file);
Void* ElfReturnProgramHeader(Void* file, UIntPtr index);

UIntPtr ElfReturnSectionHeadersCount(const Void* file);
Void* ElfReturnSectionHeaders(Void* file);
Void* ElfReturnSectionHeader(Void* file, UIntPtr index);

Void ElfLoadSection(Void* file,struct ELF32ProgramHeader *header);

Void ElfJumpEntry(Void *file, struct Catalogue* table);