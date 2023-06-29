#include "Interrupt.h"

namespace FPG
{
    int InitializeInterruptControllerDriver(XScuGic *pGIC, u16 DeviceID)
    {
        int Status;

        XScuGic_Config *pGICConfig = nullptr;

        pGICConfig = XScuGic_LookupConfig(XPAR_SCUGIC_0_DEVICE_ID);

        if (pGICConfig == nullptr)
        {
            xil_printf("Generic Interrupt Controller config lookup failed.\r\n");
            return XST_FAILURE;
        }

        Status = XScuGic_CfgInitialize(pGIC, pGICConfig, pGICConfig->CpuBaseAddress);

        if (Status != XST_SUCCESS)
        {
            xil_printf("Generic Interrupt Controller config initialization failed.: %d\r\n", Status);
            return XST_FAILURE;
        }

        Status = XScuGic_SelfTest(pGIC);

        if (Status != XST_SUCCESS)
        {
            xil_printf("Generic Interrupt Controller self-test has failed.: %d\r\n", Status);
            return XST_FAILURE;
        }

        return Status;
    }
}