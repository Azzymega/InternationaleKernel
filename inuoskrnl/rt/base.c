#include <inuos.h>
#include <executive/rt/api.h>

INUBADAPI INUGLOBAL UIntPtr randomSeed;

void *memcpy(void *dest, const void *src, size_t n) {
    const UInt32 aligned = n / sizeof(uintptr_t);
    const UInt32 out = n % sizeof(uintptr_t);

    for (size_t i = 0; i < aligned; ++i) {
        uintptr_t *FirstValue = (uintptr_t *) dest;
        const uintptr_t *SecondValue = (uintptr_t *) src;

        FirstValue[i] = SecondValue[i];
    }

    for (size_t i = 0; i < out; i++) {
        UByte *FirstValue = (UByte *) dest + (aligned * 4);
        const UByte *SecondValue = (UByte *) src + (aligned * 4);

        FirstValue[i] = SecondValue[i];
    }
    return dest;
}

void *memmove( void *restrict dstptr, const void *restrict srcptr, size_t size) {
    unsigned char *dst = dstptr;
    const unsigned char *src = (const unsigned char *) srcptr;

    if (dst < src) {
        for (size_t i = 0; i < size; i++)

            dst[i] = src[i];
    } else {
        for (size_t i = size; i != 0; i--)

            dst[i - 1] = src[i - 1];
    }
    return dstptr;
}

int memcmp(const void *aptr, const void *bptr, size_t size) {
    const UInt32 aligned = size / 4;
    const UInt32 out = size % 4;

    for (size_t i = 0; i < aligned; ++i) {
        const UInt32 *FirstValue = (const UInt32 *) aptr;
        const UInt32 *SecondValue = (const UInt32 *) bptr;

        if (FirstValue[i] < SecondValue[i]) {
            return -1;
        } else if (FirstValue[i] > SecondValue[i]) {
            return 1;
        }
    }

    const unsigned char *a = (unsigned char *) aptr + (aligned * 4);
    const unsigned char *b = (unsigned char *) bptr + (aligned * 4);

    for (size_t i = 0; i < out; i++) {
        if (a[i] < b[i])

            return -1;

        else if (b[i] < a[i])

            return 1;
    }

    return 0;
}

void *memset(char *dest, const int z, const size_t n) {
    const UInt32 aligned = n/4;
    const UInt32 nonAligned = n%4;
    UIntPtr allocated = 0;

    UIntPtr* alDest = (UIntPtr*)(dest);
    for (UIntPtr i = 0; i < aligned; ++i) {
        alDest[i] = z;
        allocated+=sizeof(UIntPtr);
    }

    char* ptr = (alDest+allocated);
    for (UIntPtr i = 0; i < nonAligned; ++i) {
        ptr[i] = z;
    }

    return dest;
}

size_t strlen(const char *string) {
    size_t Length = 0;

    while (string[Length]) {
        Length++;
    }

    return Length;
}

int strcmp(const char *firstString, const char *secondString) {
    while ((*firstString) && (*firstString == *secondString)) {
        ++firstString;
        ++secondString;
    }

    return (*firstString - *secondString);
}

size_t wstrlen(const Char *string) {
    size_t length = 0;

    while (string[length]) {
        length++;
    }
    return length;
}

int wstrcmp(const Char *firstString, const Char *secondString) {
    while ((*firstString) && (*firstString == *secondString)) {
        ++firstString;
        ++secondString;
    }

    return (*firstString - *secondString);
}

Char *wstrstr(const Char *first, const Char *second) {
    const size_t firstLength = wstrlen(first);
    const size_t secondLength = wstrlen(second);

    size_t parsedSymbols = 0;
    Char *entry = Null;

    for (UIntPtr i = 0; i < firstLength; ++i) {
        size_t index = i;
        entry = (&first[index]);
        for (UIntPtr j = 0; j < secondLength; ++j) {
            if (first[index] != second[j]) {
                entry = Null;
                parsedSymbols = 0;
                break;
            } else {
                parsedSymbols++;
                if (parsedSymbols == secondLength) {
                    return entry;
                }
            }
            index++;
        }
    }

    return entry;
}

char *strstr(const char *first, const char *second) {
    const size_t firstLength = strlen(first);
    const size_t secondLength = strlen(second);

    size_t parsedSymbols = 0;
    Char *entry = Null;

    for (UIntPtr i = 0; i < firstLength; ++i) {
        size_t index = i;
        entry = (&first[index]);
        for (UIntPtr j = 0; j < secondLength; ++j) {
            if (first[index] != second[j]) {
                entry = Null;
                parsedSymbols = 0;
                break;
            } else {
                parsedSymbols++;
                if (parsedSymbols == secondLength) {
                    return entry;
                }
            }
            index++;
        }
    }

    return entry;
}

char * strcat(char *destptr, const char *srcptr) {
    const UIntPtr strLength = strlen(destptr);
    const UIntPtr srcPtrLength = strlen(srcptr);
    memcpy(&destptr[strLength],srcptr,srcPtrLength*sizeof(char));
    destptr[strLength+srcPtrLength] = '\0';
    return destptr;
}

char *itoa(int value, char *str, int base) {
    char *rc;
    char *ptr;
    char *low;
    if (base < 2 || base > 36) {
        *str = '\0';
        return str;
    }
    rc = ptr = str;
    if (value < 0 && base == 10) {
        *ptr++ = '-';
    }
    low = ptr;
    do {
        *ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrst"
                "uvwxyz"[35 + value % base];
        value /= base;
    } while (value);
    *ptr-- = '\0';
    while (low < ptr) {
        char tmp = *low;
        *low++ = *ptr;
        *ptr-- = tmp;
    }
    return rc;
}

char* ultoa(UInt64 value, char* string, int radix)
{
    char* baseStr = string;
    unsigned char index;
    char buffer[32];

    index = 32;

    do {
        buffer[--index] = '0' + (value % radix);
        if ( buffer[index] > '9') buffer[index] += 'A' - ':';
        value /= radix;
    } while (value != 0);

    do {
        *string++ = buffer[index++];
    } while ( index < 32 );

    *string = 0;
    return baseStr;
}

char* ltoa(Int64 value, char* string, int radix)
{
    if (value < 0 && radix == 10) {
        *string++ = '-';
        return ultoa(-value, string, radix);
    }
    else {
        return ultoa(value, string, radix);
    }
}

int atoi(char *str) {
    int res = 0;
    const size_t len = strlen(str);

    for (UIntPtr i = 0; i < len; ++i) {
        res = res * 10 + str[i] - '0';
    }

    return res;
}

int rand() {
    randomSeed = randomSeed * 1103515245 + 12345;
    return ((randomSeed / 65536) % 32768);
}

void srand(const int value) {
    randomSeed = value;
}

void *lctentr(char *str, char *start, size_t size) {
    size_t strLen = strlen(str);
    for (size_t i = 0; i < size; ++i) {
        char *pointer = start + i;
        if (memcmp(str, pointer, strLen) == 0) {
            return pointer;
        }
    }
    return Null;
}

void swpbts(char *ptr, const size_t size) {
    char values[size];
    memset(values, 0, size);
    memcpy(values, ptr, size);
    rvsmemcpy(ptr, values, size * sizeof(Byte));
}

void *rvsmemcpy(void *dest, void *src, size_t n) {
    char *ptr = (src);
    char *destPtr = (dest);
    ptr += n;
    for (UIntPtr i = 0; i < n; ++i) {
        char *source = ptr - i;
        *destPtr = *source;
    }
    return dest;
}

bool wstrque(Char *first, Char *second) {
    const size_t firstLength = wstrlen(first);
    const size_t secondLength = wstrlen(second);

    if (firstLength != secondLength) {
        return false;
    }
    else {
        if (memcmp(first,second,firstLength*sizeof(Char)) != 0) {
            return false;
        }
    }

    return true;
}

bool wstrquec(Char *first, const Char *second) {
    const size_t firstLength = wstrlen(first);
    const size_t secondLength = wstrlen(second);

    if (firstLength != secondLength) {
        return false;
    }
    else {
        if (memcmp(first,second,firstLength*sizeof(Char)) != 0) {
            return false;
        }
    }

    return true;
}

Boolean NAN(const Double value) {
    return value != value;
}

Double fmod(const Double first, const Double second) {
    if (NAN(second) || NAN(first)) {
        return first;
    }
    if (second == 0) {
        return first;
    }
    return first - (first / second) * second;
}

UIntPtr getbit(const UIntPtr n, const UIntPtr pos) {
    return ((n & (1 << pos)) != 0);
}

UIntPtr clrbit(const UIntPtr n, const UIntPtr pos) {
    const UIntPtr reverse = ~(1 << pos);
    return (n & reverse);
}

UIntPtr setbit(const UIntPtr number, const UIntPtr n) {
    return number | (1 << n);
}

Boolean chkbit(const UIntPtr number, const UIntPtr n) {
    return (number >> n) & 1;
}

UIntPtr fndmax(const UIntPtr *numbers, UIntPtr count) {
    UIntPtr value = numbers[0];

    for (UIntPtr i = 0; i < count; ++i) {
        if (numbers[i] > value) {
            value = numbers[i];
        }
    }

    return value;
}

Void RtlClearStackArray(Void *target, const UIntPtr size) {
    memset(target,INUFREESPACE,size);
}
