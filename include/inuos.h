#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#define INUABI 2
#define INUFREESPACE 0

#define IN
#define OUT
#define INOUT
#define INUNATIVECALL
#define INUASM
#define INUNATIVE
#define INUOPTIMIZE
#define INUOBJECT
#define INUEXTERN extern
#define INUIMPORT extern
#define INUBADAPI
#define INUINLINEDCOMPATIABLE // IF TYPE CAN BE SOMETHING LIKE INT (mainly for bits type)
#define INUGLOBAL
#define INUINLINEDNONOBJECT
#define INUNOINTERRUPT
#define INUPAYLOADTARGET
#define INUCONTEXTSENSETIVE
#define INUUNUSED
#define INUIMPLEMENT
#define INUMONITOR
#define INUNEWFEATURE
#define INUREPLACE
#define INUSTACKINTENSIVE
#define INUINTERNAL
#define INUARCHDEP
#define INUSTATIC static
#define INULIKELY(x)       __builtin_expect(!!(x), 1)
#define INUUNLIKELY(x)     __builtin_expect(!!(x), 0)
#define INUFORCEINLINE inline __attribute__((always_inline))
#define INUPACKED __attribute__((packed))
#define INUAPI __attribute__((stdcall))
#define INUCDECL __attribute__((cdecl))
#define INUFASTCALL __attribute__((fastcall))
#define INUREGCALL __attribute__((regcall))
#define INUOLDCALL __attribute__((pascal))
#define INUINLINE inline
#define INUNAKED __attribute__((naked))
#define INUINTERRUPT __attribute__((interrupt,no_caller_saved_registers))
#define INUGHANDLER __attribute__((no_caller_saved_registers))
#define INUSAFE INUGHANDLER
#define INUISR __attribute__((interrupt))
#define Null NULL

typedef bool Boolean;
typedef wchar_t Char;
typedef char AChar;
typedef void Void;

typedef uint8_t UByte;
typedef int8_t Byte;

typedef float Float;
typedef double Double;

typedef intptr_t IntPtr;
typedef int64_t Int64;
typedef int32_t Int32;
typedef int16_t Int16;

typedef uintptr_t UIntPtr;
typedef uint64_t UInt64;
typedef uint32_t UInt32;
typedef uint16_t UInt16;

enum InuStatus : UInt16 {
    InuSuccess,
    InuFail
};
