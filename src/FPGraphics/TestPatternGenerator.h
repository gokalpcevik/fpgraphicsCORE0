#pragma once
#include <stdbool.h>
#include "xv_tpg.h"
#include "xil_types.h"

namespace FPG
{
    int TPG_InitializeDriver(XV_tpg* pTPG,u16 DeviceID);
}