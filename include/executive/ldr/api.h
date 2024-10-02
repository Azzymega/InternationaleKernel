#pragma once
#include <inuos.h>

typedef UInt16 Elf32_Half;
typedef UInt32 Elf32_Off;
typedef UInt32 Elf32_Addr;
typedef UInt32 Elf32_Word;
typedef Int32  Elf32_Sword;
typedef UInt64 Elf64_Addr;
typedef UInt64 Elf64_Off;

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

struct ELF64 {
    struct ELFBaseHeader base;
    Elf64_Addr	e_entry;
    Elf64_Off	e_phoff;
    Elf64_Off	e_shoff;
    Elf32_Word	e_flags;
    Elf32_Half	e_ehsize;
    Elf32_Half	e_phentsize;
    Elf32_Half	e_phnum;
    Elf32_Half	e_shentsize;
    Elf32_Half	e_shnum;
    Elf32_Half	e_shstrndx;
}
INUPACKED
INUNATIVE;

struct ELF32 {
    struct ELFBaseHeader base;
    Elf32_Addr	e_entry;
    Elf32_Off	e_phoff;
    Elf32_Off	e_shoff;
    Elf32_Word	e_flags;
    Elf32_Half	e_ehsize;
    Elf32_Half	e_phentsize;
    Elf32_Half	e_phnum;
    Elf32_Half	e_shentsize;
    Elf32_Half	e_shnum;
    Elf32_Half	e_shstrndx;
}
INUPACKED
INUNATIVE;