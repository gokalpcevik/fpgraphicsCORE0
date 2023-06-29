#pragma once

#include "xscugic.h"
#include "xil_exception.h"
#include "xparameters.h"

namespace FPG
{
    int InitializeGICDriver(XScuGic* pGIC, u16 DeviceID);

}