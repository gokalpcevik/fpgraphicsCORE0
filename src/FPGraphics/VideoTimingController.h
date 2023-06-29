#pragma once
#include "xvtc.h"
#include "Definitions.h"


namespace FPG
{
    int VTC_InitializeDriveer(XVtc* pVTC);
    void VTC_EnableGenerator(XVtc* vtc);
}