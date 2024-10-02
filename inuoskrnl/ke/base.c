#include <executive/bm/base.h>
#include <executive/cm/api.h>
#include <executive/ga/api.h>
#include <executive/ki/api.h>
#include <executive/mm/api.h>
#include <executive/pm/api.h>
#include <executive/ps/api.h>
#include <executive/sm/api.h>
#include <executive/vm/api.h>
#include <inuhal/api.h>
#include <ke/base.h>

INUIMPORT struct Catalogue HalInformation;

Void KeInitialize() {
    FwInitialize();
    PmInitialize();
    GaInitialize();
    VmInitialize();

    ObInitialize();
    CmInitialize();
    KiInitialize();
    PsInitialize();
    BmInitialize();

    FwLowerControlLevel(CL_PASSIVE);
    HalEnableSignals();

    // REPLACE WITH THREAD!
    BmEntry();
}

Void KeBootstrap(const struct Catalogue *table) {
    HalInitialize(table);
    HalSwitchStack(HalInformation.stackLocation);
    HalRemapBootloader();
    HalDisableSignals();
    KeInitialize();
}
