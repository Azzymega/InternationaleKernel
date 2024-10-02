#pragma once
#include <inuos.h>
#include <executive/ob/api.h>

enum JsnType : UByte {
    JsnEnumValue = 1,
    JsnEnumStruct = 2,
    JsnEnumDocument = 3,
    JsnEnumArray = 4,
    JsnEnumString = 5
};

struct JsnStringBuilder {
    struct Object object;
    UIntPtr allocated;
    UIntPtr placed;
    struct Array* internal;
}
INUPACKED
INUOBJECT;

struct JsnTape {
    struct Object object;
    UIntPtr wind;
    AChar* data;
}
INUPACKED
INUOBJECT;

struct JsnDocument {
    struct Object object;
    enum JsnType type;
    struct JsnStruct* target;
}
INUPACKED
INUOBJECT;

struct JsnStruct {
    struct Object object;
    enum JsnType type;
    struct Vector* value;
}
INUPACKED
INUOBJECT;

struct JsnValue {
    struct Object object;
    enum JsnType type;
    struct String* key;
    struct Object* value;
}
INUPACKED
INUOBJECT;

struct JsnDocument* JsnCreateDocument(Void* file);
struct JsnStruct* JsnCreateStruct();
struct JsnValue* JsnCreateValue(enum JsnType type);
struct JsnStringBuilder* JsnCreateStringBuilder();
struct JsnTape* JsnCreateTape(Void* file);


Void JsnStringBuilderAppend(struct JsnStringBuilder* object, Char value);
Void JsnStringBuilderClear(struct JsnStringBuilder* object);
struct String* JsnStringBuilderResult(const struct JsnStringBuilder* object);
Void JsnStringBuilderReallocate(struct JsnStringBuilder* object);
Char JsnTapeReadCharacter(struct JsnTape* object);
Void JsnTapeRewind(struct JsnTape* object, UIntPtr value);


struct JsnStruct* JsnParseStruct(struct JsnTape* tape);
struct JsnValue* JsnParseValue(struct JsnTape* tape);