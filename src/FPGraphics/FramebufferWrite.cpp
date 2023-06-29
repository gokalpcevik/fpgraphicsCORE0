#include "FramebufferWrite.h"

namespace FPG
{
    int FBWrite_InitializeDriver(XV_FrmbufWr_l2 *pFBWrite, u16 DeviceID)
    {
        int Status = XVFrmbufWr_Initialize(pFBWrite, DeviceID);

        if (Status != XST_SUCCESS)
        {
            xil_printf("Frame buffer write(Device ID=%u) initialization has failed.: %d\r\n", DeviceID, Status);
            return XST_FAILURE;
        }

        return (Status);
    }

    int FBWrite_ConnectToInterruptController(XScuGic *pGIC, u32 InterruptID, XV_FrmbufWr_l2 *pFBWrite)
    {
        int Status = XScuGic_Connect(pGIC, InterruptID, (Xil_InterruptHandler)XVFrmbufWr_InterruptHandler, (void *)pFBWrite);

        if (Status != XST_SUCCESS)
        {
            xil_printf("Connection of the framebuffer write interrupt to the GIC has failed.%d\r\n", Status);
            return XST_FAILURE;
        }

        return (Status);
    }
}