#pragma once
#include <inuos.h>

struct TarHeader
{
    char filename[100];
    char mode[8];
    char uid[8];
    char gid[8];
    char size[12];
    char mtime[12];
    char chksum[8];
    char typeflag[1];
    char padding[355];
}
INUPACKED
INUNATIVE;

UIntPtr TarLocateFileSector(IN const char* name);
UIntPtr TarReadFileLength(IN const char* name);
Void TarReadFile(IN const char* name, INOUT Byte* buffer);
