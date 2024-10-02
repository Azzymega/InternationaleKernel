#pragma once

Void* memcpy(Void* dest, const Void* src, size_t n);
int memcmp(const Void* s1, const Void* s2, size_t n);
Void* memmove(Void* s1, const Void* s2, size_t n);
Void* memset(char* dest, int z, size_t n);
size_t strlen(const char* string);
int strcmp(const char* firstString, const char* secondString);
char* strstr(const char* first, const char* second);
char* strcat(char* destptr, const char* srcptr);
char *itoa(int value, char *str, int base);
char *ultoa(UInt64 value, char* string, int radix);
char *ltoa(Int64, char *str, int base);
int atoi(char* str);
int rand();
Void srand(int value);
size_t wstrlen(const Char* string);
bool wstrque(Char* first, Char* second);
bool wstrquec(Char* first, const Char* second);
int wstrcmp(const Char* firstString, const Char* secondString);
Char* wstrstr(const Char* first, const Char* second);
char* strstr(const char* first, const char* second);
Void* lctentr(char* str, char* start, size_t size);
Void* rvsmemcpy(Void* dest, Void* src, size_t n);
Void swpbts(char* ptr, size_t size);
Double fmod(Double first, Double second);
UIntPtr getbit(UIntPtr n, UIntPtr pos);
UIntPtr clrbit(UIntPtr n, UIntPtr pos);
UIntPtr setbit(UIntPtr number, UIntPtr n);
Boolean chkbit(UIntPtr number, UIntPtr n);
UIntPtr fndmax(const UIntPtr* numbers, UIntPtr count);

Void RtlClearStackArray(Void* target, UIntPtr size);