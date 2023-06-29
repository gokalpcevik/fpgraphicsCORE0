#include "FrameBufferRead.h"

namespace FPG
{
    int ConnectFrameBufferReadToSCUGIC(XScuGic *pGIC, u32 InterruptID, XV_FrmbufRd_l2 *pFBRead)
    {
        int Status = XScuGic_Connect(pGIC, InterruptID, (Xil_InterruptHandler)XVFrmbufRd_InterruptHandler, (void *)pFBRead);

        if (Status != XST_SUCCESS)
        {
            xil_printf("Connection of the framebuffer read(Device ID=%u) interrupt to the GIC has failed.%d\r\n", FPG::FRMBUF_RD0_DEVICE_ID, Status);
            return XST_FAILURE;
        }

        return (Status);
    }

    int InitializeFrameBufferReadDriver(XV_FrmbufRd_l2 *pFBRead, u16 DeviceID)
    {
        int Status = XVFrmbufRd_Initialize(pFBRead, DeviceID);

        if (Status != XST_SUCCESS)
        {
            xil_printf("Frame buffer read(Device ID=%u) initialization has failed.: %d\r\n", DeviceID, Status);
            return XST_FAILURE;
        }

        return (Status);
    }
}