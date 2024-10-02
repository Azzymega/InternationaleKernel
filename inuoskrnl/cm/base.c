#include <executive/bm/base.h>
#include <inufw/api.h>
#include <executive/ki/api.h>
#include <executive/ob/api.h>
#include <executive/rt/api.h>
#include <executive/cm/api.h>

INUGLOBAL struct Registry* Registry;

Void CmInitialize() {
    Registry = ObCreateRegistry(L"ROOT");
}

Void CmRegisterObject(struct Object *target, const Char *namespaceName) {
    ObEnterMonitorObject(&Registry->object);
    for (int i = 0; i < Registry->registers->count; ++i) {
        struct Registry* nameSpace = ObVectorGet(Registry->registers,i);
        if (wstrcmp(nameSpace->string->characters->charValues,namespaceName) == 0) {
            ObVectorAdd(nameSpace->objects,target);
            break;
        }
    }
    ObExitMonitorObject(&Registry->object);
}

Void CmUnregisterObject(struct Object *target, Char *namespaceName) {
    ObEnterMonitorObject(&Registry->object);
    for (int i = 0; i < Registry->registers->count; ++i) {
        struct Registry* nameSpace = ObVectorGet(Registry->registers,i);
        if (wstrcmp(nameSpace->string->characters->charValues,namespaceName) == 0) {
            ObVectorRemove(nameSpace->objects,target);
            break;
        }
    }
    ObExitMonitorObject(&Registry->object);
}

Void CmRegisterNamespace(Char *name) {
    ObEnterMonitorObject(&Registry->object);
    struct Registry* newNamespace = ObCreateRegistry(name);
    ObVectorAdd(Registry->registers,newNamespace);
    ObExitMonitorObject(&Registry->object);
}

struct Vector* CmRegisterGetNamespaceVector(const Char *namespaceName) {
    ObEnterMonitorObject(&Registry->object);
    Void* target = Null;
    for (int i = 0; i < Registry->registers->count; ++i) {
        struct Registry* nameSpace = ObVectorGet(Registry->registers,i);
        if (wstrcmp(nameSpace->string->characters->charValues,namespaceName) == 0) {
            target = nameSpace->objects;
            break;
        }
    }
    ObExitMonitorObject(&Registry->object);
    return target;
}
