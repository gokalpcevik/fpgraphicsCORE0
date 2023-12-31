#pragma once
#include "xv_frmbufrd_l2.h"
#include "xscugic.h"
#include "xil_types.h"

namespace FPG
{
    /// @brief Initializes the Framebuffer Read Driver
    /// @param pFBRead Pointer to the framebuffer read l2 instance
    /// @param DeviceID Device ID
    /// @return XST_SUCCESS on success, XST_FAILURE otherwise.
    int FBRead_InitializeDriver(XV_FrmbufRd_l2 *pFBRead, u16 DeviceID);

    /// @brief Connects the Framebuffer Read Driver to the Generic Interrupt Controller
    /// @param pGIC Generic Interrupt Controller Driver Instance
    /// @param InterruptID Interrupt ID
    /// @param pFBRead Framebuffer Read Layer 2 Instance
    int FBRead_ConnectToInterruptController(XScuGic *pGIC, u32 InterruptID, XV_FrmbufRd_l2 *pFBRead);
}