#pragma once
#include "xv_frmbufwr_l2.h"
#include "xscugic.h"
#include "xil_types.h"

namespace FPG
{
    /// @brief Initializes the Framebuffer Read Driver
    /// @param pFBWrite Pointer to the framebuffer read l2 instance
    /// @param DeviceID Device ID
    /// @return XST_SUCCESS on success, XST_FAILURE otherwise.
    int FBWrite_InitializeDriver(XV_FrmbufWr_l2 *pFBWrite, u16 DeviceID);

    /// @brief Connects the Framebuffer Read Driver to the Generic Interrupt Controller
    /// @param pGIC Generic Interrupt Controller Driver Instance
    /// @param InterruptID Interrupt ID
    /// @param pFBWrite Framebuffer Read Layer 2 Instance
    int FBWrite_ConnectToInterruptController(XScuGic *pGIC, u32 InterruptID, XV_FrmbufWr_l2 *pFBWrite);
}