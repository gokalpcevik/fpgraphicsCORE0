#include "VideoTimingController.h"

namespace FPG
{
    int CfgInitializeVideoTimingController(XVtc* vtc)
    {

        int Status;

        XVtc_Config* pVtcConfig = XVtc_LookupConfig(FPG::VTC_DEVICE_ID);

        if(pVtcConfig == nullptr)
        {
            xil_printf("Video timing controller config lookup failed. \r\n");
            return XST_FAILURE;
        }

        Status = XVtc_CfgInitialize(vtc,pVtcConfig,pVtcConfig->BaseAddress);

        if(Status != XST_SUCCESS)
        {
            xil_printf("Video timing controller config initialization failed.: %d\r\n",Status);
            return XST_FAILURE;
        }

        Status = XVtc_SelfTest(vtc);

        if(Status != XST_SUCCESS)
        {
            xil_printf("Video timing controller self-test has failed.:%d \r\n",Status);
            return XST_FAILURE;
        }
        else if (Status == XST_SUCCESS)
        {
            xil_printf("Video timing controller self-test has passed. \r\n");
        }

        XVtc_RegUpdateEnable(vtc);
        return XST_SUCCESS;
    }

    int SetVTCSourceGenerator(XVtc* vtc)
    {

        /* For now, we will just initialize the source to be the generator(not detector). The 
         * other initializations like the polarity or the generator will be left as default.
         */

        // 1 = Source is generator
        // 0 = Source is detector
        XVtc_SourceSelect SrcSel{};
        // Mem zero SrcSel
        memset(&SrcSel,0x0,sizeof(XVtc_SourceSelect));
        SrcSel.VBlankPolSrc = 1;
	    SrcSel.VSyncPolSrc = 1;
	    SrcSel.HBlankPolSrc = 1;
	    SrcSel.HSyncPolSrc = 1;
	    SrcSel.ActiveVideoPolSrc = 1;
	    SrcSel.ActiveChromaPolSrc= 1;
	    SrcSel.VChromaSrc = 1;
	    SrcSel.VActiveSrc = 1;
	    SrcSel.VBackPorchSrc = 1;
	    SrcSel.VSyncSrc = 1;
	    SrcSel.VFrontPorchSrc = 1;
	    SrcSel.VTotalSrc = 1;
	    SrcSel.HActiveSrc = 1;
	    SrcSel.HBackPorchSrc = 1;
	    SrcSel.HSyncSrc = 1;
	    SrcSel.HFrontPorchSrc = 1;
	    SrcSel.HTotalSrc = 1;

        XVtc_SetSource(vtc,&SrcSel);
        return XST_SUCCESS;
    }

    int VTC_InitializeDriveer(XVtc *pVTC)
    {
        int Status0 = FPG::CfgInitializeVideoTimingController(pVTC);
        int Status1 = FPG::SetVTCSourceGenerator(pVTC);
        return (Status0 == XST_SUCCESS && Status1 == XST_SUCCESS) ? XST_SUCCESS : XST_FAILURE;
    }

    void VTC_EnableGenerator(XVtc *vtc)
    {
        XVtc_EnableGenerator(vtc);
    }
}