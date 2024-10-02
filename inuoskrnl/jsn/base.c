#include <executive/jsn/api.h>
#include <executive/mm/api.h>
#include <executive/rt/api.h>

#define JSN_STRINGBUILDER_ARRAY_INCREMENT 30
#define JSN_STRINGBUILDER_INITIAL_SIZE 30
#define JSN_TAPE_CHARACTER_LENGTH 1

struct JsnDocument* JsnCreateDocument(Void* file) {
    struct JsnDocument* value = MmAllocateMemory(sizeof(struct JsnDocument));
    ObObjectInitialize(&value->object,sizeof(struct JsnDocument));

    struct JsnTape* tape = JsnCreateTape(file);
    value->target = JsnParseStruct(tape);
    value->type = JsnEnumDocument;

    MmFreeMemory(tape);
    return value;
}

struct JsnTape* JsnCreateTape(Void* file) {
    struct JsnTape* value = MmAllocateMemory(sizeof(struct JsnTape));
    ObObjectInitialize(&value->object,sizeof(struct JsnTape));
    value->data = file;
    value->wind = 0;
    return value;
}

Void JsnStringBuilderAppend(struct JsnStringBuilder* object, const Char value) {
    if (object->placed + 1 >= object->allocated) {
        JsnStringBuilderReallocate(object);
    }

    object->internal->charValues[object->placed++] = value;
}

Void JsnStringBuilderClear(struct JsnStringBuilder* object) {
    object->placed = 0;
}

struct String* JsnStringBuilderResult(const struct JsnStringBuilder* object) {
    return ObCreateString(object->internal->charValues);
}

Void JsnStringBuilderReallocate(struct JsnStringBuilder* object) {
    struct Array* newArray = ObCreateArray(object->allocated+JSN_STRINGBUILDER_ARRAY_INCREMENT,ATChar);
    memcpy(newArray->charValues,object->internal->charValues,sizeof(Char)*object->placed);
    MmFreeMemory(object->internal);
    object->internal = newArray;
}

Char JsnTapeReadCharacter(struct JsnTape *object) {
    return object->data[object->wind++];
}

Void JsnTapeRewind(struct JsnTape *object, const UIntPtr value) {
    object->wind -= value;
}

struct JsnValue * JsnParseValue(struct JsnTape *tape) {
    struct JsnValue* value = Null;

    struct JsnStringBuilder* builder = JsnCreateStringBuilder();
    while (true) {
        Char target = JsnTapeReadCharacter(tape);
        if (target == '"') {

        }
        else {

        }
    }
    MmFreeMemory(builder);
    return value;
}

struct JsnStringBuilder* JsnCreateStringBuilder() {
    struct JsnStringBuilder* value = MmAllocateMemory(sizeof(struct JsnStringBuilder));
    ObObjectInitialize(&value->object,sizeof(struct JsnStringBuilder));

    value->internal = ObCreateArray(JSN_STRINGBUILDER_INITIAL_SIZE,ATChar);
    return value;
}

struct JsnStruct* JsnParseStruct(struct JsnTape* tape) {
    struct JsnStruct* value = JsnCreateStruct();

    //TODO: THIS IS CRAP. MALFORMED JSON LEAD TO KERNEL BLOW UP!!!11231
    struct JsnStringBuilder* builder = JsnCreateStringBuilder();
    while (true) {
        const Char target = JsnTapeReadCharacter(tape);
        if (target == '}') {
            break;
        }
        else if (target == '"') {
            JsnTapeRewind(tape,JSN_TAPE_CHARACTER_LENGTH*1);
            ObVectorAdd(value->value,(struct Object*)JsnParseValue(tape));
        }
    }

    MmFreeMemory(builder);
    return value;
}

struct JsnStruct* JsnCreateStruct() {
    struct JsnStruct* value = MmAllocateMemory(sizeof(struct JsnStruct));
    ObObjectInitialize(&value->object,sizeof(struct JsnStruct));

    value->type = JsnEnumStruct;
    value->value = ObCreateVector();

    return value;
}

struct JsnValue* JsnCreateValue(enum JsnType type) {
    struct JsnValue* value = MmAllocateMemory(sizeof(struct JsnValue));
    ObObjectInitialize(&value->object,sizeof(struct JsnValue));

    value->type = type;
    value->key = Null;

    if (type == JsnEnumString) {
        value->value = Null;
    }
    else if (type == JsnEnumArray) {
        value->value = ObCreateVector();
    }

    return value;
}