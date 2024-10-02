#pragma once
#include <inuos.h>

Void VmiMapGpde(INOUT const struct Description *description, INOUT struct GPDE* target, IN UIntPtr baseOffset ,IN UIntPtr paddr, IN UIntPtr vaddr, IN struct VMA attr);
Void VmiMapMpde(INOUT const struct Description *description, INOUT struct MPDE* target, IN UIntPtr baseOffset ,IN UIntPtr paddr, IN UIntPtr vaddr, IN struct VMA attr);
Void VmiMapLpde(INOUT const struct Description *description, INOUT struct LPDE* target, IN UIntPtr baseOffset ,IN UIntPtr paddr, IN UIntPtr vaddr, IN struct VMA attr);
Void VmiMapLpte(INOUT const struct Description *description, INOUT struct LPTE* target, IN UIntPtr baseOffset ,IN UIntPtr paddr, IN UIntPtr vaddr, IN struct VMA attr);
Void VmiMapPf(INOUT const struct Description *description, INOUT struct PF* target, IN UIntPtr baseOffset, IN UIntPtr paddr, IN UIntPtr vaddr, IN struct VMA attr);