#pragma once
#include "xv_frmbufrd_l2.h"
#include "xscugic.h"
#include "xil_types.h"
#include <assert.h>

#include "Definitions.h"

namespace FPG
{
    /// @brief Connects the Framebuffer Read Driver to the Generic Interrupt Controller
    /// @param pGIC Generic Interrupt Controller Driver Instance
    /// @param InterruptID Interrupt ID
    /// @param pFBRead Framebuffer Read Layer 2 Instance
    /// @return 
    int ConnectFrameBufferReadToSCUGIC(XScuGic* pGIC, u32 InterruptID, XV_FrmbufRd_l2* pFBRead);
    /// @brief 
    /// @param pFBRead 
    /// @param DeviceID 
    /// @return 
    int InitializeFrameBufferReadDriver(XV_FrmbufRd_l2* pFBRead,u16 DeviceID);
}