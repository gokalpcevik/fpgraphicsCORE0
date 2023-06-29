#pragma once
#include "xvtc.h"
#include "Definitions.h"


namespace FPG
{
    int CfgInitializeVideoTimingController(XVtc* vtc);
    /// @brief Initializes the source, generator
    /// @param vtc 
    /// @return XST_SUCCESS on success, else XST_FAILURE.
    int InitializeVideoTimingControllerCore(XVtc* vtc);
    
    void EnableVTCGenerator(XVtc* vtc);
}