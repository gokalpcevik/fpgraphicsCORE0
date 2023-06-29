#pragma once
#include "xparameters.h"
#include "xil_types.h"
#include "xvidc.h"

namespace FPG
{
    // Device IDs
    static constexpr u32 FRMBUF_RD0_DEVICE_ID = XPAR_XV_FRMBUFRD_0_DEVICE_ID;
    static constexpr u32 FRMBUF_WR0_DEVICE_ID = XPAR_XV_FRMBUFWR_0_DEVICE_ID;
    static constexpr u32 VTC_DEVICE_ID = XPAR_VTC_0_DEVICE_ID;
    
    // Constants configured in Vivado
    static constexpr u32 PIXELS_PER_CLOCK = 1U;
    static constexpr XVidC_VideoMode DEFAULT_VID_MODE = XVIDC_VM_720_60_P; 
    
    // Framebuffer Addresses
    static constexpr UINTPTR FRAMEBUFFER0_ADDRESS = ((UINTPTR)(XPAR_DDR_MEM_BASEADDR + 0x00010000U));
    static constexpr UINTPTR FRAMEBUFFER1_ADDRESS = ((UINTPTR)0x20000000ULL);
}