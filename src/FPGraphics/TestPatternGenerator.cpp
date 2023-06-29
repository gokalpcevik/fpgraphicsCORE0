#include "TestPatternGenerator.h"

namespace FPG
{
    int TPG_InitializeDriver(XV_tpg* pTPG,u16 DeviceID)
    {
        XV_tpg_Config* pConfig = XV_tpg_LookupConfig(DeviceID);

        if(pConfig == nullptr)
        {
            xil_printf("Test pattern generator config lookup failed. \r\n");
            return XST_FAILURE;
        }

        int Status = XV_tpg_CfgInitialize(pTPG, pConfig, pConfig->BaseAddress);
    
        if(Status != XST_SUCCESS)
        {
            xil_printf("Test pattern generator config initialize has failed.: %d\r\n",Status);
            return XST_FAILURE;
        }

        return (Status);
    }
}