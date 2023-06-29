#include "xv_frmbufrd_l2.h"
#include "xv_frmbufwr_l2.h"
#include <stdbool.h>
#include "xv_tpg.h"
#include "xvtc.h"
#include "xscugic.h"
#include "xdebug.h"
#include "xvidc.h"

#include "xil_cache.h"
#include "xparameters.h"
#include "FPGraphics/Definitions.h"
#include "FPGraphics/VideoTimingController.h"
#include "FPGraphics/FrameBufferRead.h"
#include "FPGraphics/Interrupt.h"

// Snoop Control Unit Generic Interrupt Controller
XScuGic IntrController;

// Video timing controller
XVtc VTC;

// Test pattern generator
XV_tpg TPGen;

// Framebuffer read
XV_FrmbufRd_l2 FBRead;

// Framebuffer write
XV_FrmbufWr_l2 FBWrite;

#define RET_IF_NOT_SUCCESS(x) \
	if ((x) != XST_SUCCESS)   \
	return XST_FAILURE

void *XVFBRead_DoneCallback(void *data)
{
	XVFrmbufRd_Start(&FBRead);
}

int main(void)
{
	Xil_DCacheDisable();

	int Status = FPG::InitializeGICDriver(&IntrController, XPAR_SCUGIC_0_DEVICE_ID);

	RET_IF_NOT_SUCCESS(Status);

	// Let know that the GIC driver has been initialized
	xil_printf("SCU Generic Interrupt Controller driver has been initialized successfully.\r\n");

	// Connect the framebuffer read interrupt to the interrupt controller to accept
	FPG::ConnectFrameBufferReadToSCUGIC(&IntrController, XPAR_FABRIC_V_FRMBUF_RD_0_VEC_ID, &FBRead);

	// Register the handler from the generic interrupt controller driver
	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT, (Xil_ExceptionHandler)XScuGic_InterruptHandler, &IntrController);

	// Enable the Framebuffer Read connection to the GIC
	XScuGic_Enable(&IntrController, XPAR_FABRIC_V_FRMBUF_RD_0_VEC_ID);
	
	// Video Timing Controller
	RET_IF_NOT_SUCCESS(FPG::CfgInitializeVideoTimingController(&VTC));
	RET_IF_NOT_SUCCESS(FPG::InitializeVideoTimingControllerCore(&VTC));

	// Enable the generator
	FPG::EnableVTCGenerator(&VTC);

	// Initialize the Framebuffer Read
	RET_IF_NOT_SUCCESS(FPG::InitializeFrameBufferReadDriver(&FBRead, FPG::FRMBUF_RD0_DEVICE_ID));

	Xil_ExceptionEnable();

	// Set the fb read DONE callback function
	XVFrmbufRd_SetCallback(&FBRead, XVFRMBUFRD_HANDLER_DONE, (void *)XVFBRead_DoneCallback, (void *)&FBRead);

	auto MMWidthInBytes = FBRead.FrmbufRd.Config.AXIMMDataWidth / 8;
	auto stride = ((((1280 * 3)) + MMWidthInBytes - 1) / MMWidthInBytes) * MMWidthInBytes;

	XVidC_VideoStream stream{};
	stream.PixPerClk = XVIDC_PPC_1;
	stream.ColorFormatId = XVIDC_CSF_RGB;
	stream.ColorDepth = XVIDC_BPC_8;
	stream.FrameRate = XVidC_GetFrameRate(FPG::DEFAULT_VID_MODE);
	stream.Timing = *XVidC_GetTimingInfo(FPG::DEFAULT_VID_MODE);
	stream.VmId = FPG::DEFAULT_VID_MODE;
	stream.AspectRatio = XVIDC_AR_16_9;
	stream.Is3D = 0;
	stream.IsInterlaced = 0;

	// Set the memory format of the framebuffer R/W
	Status = XVFrmbufRd_SetMemFormat(&FBRead, stride, XVIDC_CSF_MEM_RGB8, &stream);

	if (Status != XST_SUCCESS)
	{
		xil_printf("Setting the memory format of framebuffer R/W has failed.: %d\r\n", Status);
		return XST_FAILURE;
	}

	memset((void *)FPG::FRAMEBUFFER1_ADDRESS, 0x00, 1280ULL * 720ULL * 3);

	// Set the buffer addresses
	Status = XVFrmbufRd_SetBufferAddr(&FBRead, FPG::FRAMEBUFFER1_ADDRESS);

	if (Status != XST_SUCCESS)
	{
		xil_printf("Setting the Framebuffer R/W address has failed.: %d\r\n", Status);
		return XST_FAILURE;
	}

	// Enable IRQ_DONE start reading the fb
	XVFrmbufRd_InterruptEnable(&FBRead, XVFRMBUFRD_IRQ_DONE_MASK);
	XVFrmbufRd_Start(&FBRead);

	while (true)
	{
		for (int i = 0; i < 30000000; i++);
	}
	return XST_SUCCESS;
}
