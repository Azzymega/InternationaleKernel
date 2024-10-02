#pragma once
#include <inuos.h>
#include <executive/ob/api.h>

Void CmInitialize();

INUMONITOR Void CmRegisterObject(IN struct Object *target,
                                 IN const Char *namespaceName);

INUMONITOR Void CmUnregisterObject(IN struct Object *target,
                                   IN Char *namespaceName);

INUMONITOR Void CmRegisterNamespace(IN Char *name);

INUMONITOR struct Vector* CmRegisterGetNamespaceVector(IN const Char* namespaceName);

