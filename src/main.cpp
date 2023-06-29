#include <stdbool.h>
#include <assert.h>
#include "xv_frmbufrd_l2.h"
#include "xv_frmbufwr_l2.h"
#include "xv_tpg.h"
#include "xvtc.h"
#include "xscugic.h"
#include "xdebug.h"
#include "xvidc.h"

#include "xil_cache.h"
#include "xparameters.h"
#include "FPGraphics/Definitions.h"
#include "FPGraphics/VideoTimingController.h"
#include "FPGraphics/TestPatternGenerator.h"
#include "FPGraphics/FrameBufferRead.h"
#include "FPGraphics/FrameBufferWrite.h"
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

void SwapBuffers(XV_FrmbufWr_l2 *pWrite, XV_FrmbufRd_l2 *pRead)
{
	if (XVFrmbufRd_GetBufferAddr(pRead) == FPG::FRAMEBUFFER1_ADDRESS)
	{
		XVFrmbufRd_SetBufferAddr(pRead, FPG::FRAMEBUFFER0_ADDRESS);
		XVFrmbufWr_SetBufferAddr(pWrite, FPG::FRAMEBUFFER1_ADDRESS);
	}
	else if (XVFrmbufRd_GetBufferAddr(pRead) == FPG::FRAMEBUFFER0_ADDRESS)
	{
		XVFrmbufRd_SetBufferAddr(pRead, FPG::FRAMEBUFFER1_ADDRESS);
		XVFrmbufWr_SetBufferAddr(pWrite, FPG::FRAMEBUFFER0_ADDRESS);
	}
}

bool WriteCompleted = false;

void *XVFBRead_DoneCallback(void *data)
{
	if (WriteCompleted)
	{
		SwapBuffers(&FBWrite,&FBRead);
		XV_tpg_Start(&TPGen);
		XVFrmbufWr_Start(&FBWrite);
		XVFrmbufRd_Start(&FBRead);
		WriteCompleted = false;
	}
	else
	{
		XVFrmbufRd_Start(&FBRead);
	}
}

void *XVFBWrite_DoneCallback(void *data)
{
	WriteCompleted = true;
}

int main(void)
{
	Xil_DCacheDisable();

	RET_IF_NOT_SUCCESS(FPG::TPG_InitializeDriver(&TPGen, XPAR_XV_TPG_0_DEVICE_ID));

	XV_tpg_Set_width(&TPGen, 1280);
	XV_tpg_Set_height(&TPGen, 720);
	XV_tpg_Set_Interlaced(&TPGen, false);
	XV_tpg_Set_bckgndId(&TPGen, XTPG_BKGND_TARTAN_COLOR_BARS);

	int Status = FPG::InitializeInterruptControllerDriver(&IntrController, XPAR_SCUGIC_0_DEVICE_ID);

	RET_IF_NOT_SUCCESS(Status);

	// Let know that the GIC driver has been initialized
	xil_printf("SCU Generic Interrupt Controller driver has been initialized successfully.\r\n");

	// Connect the framebuffer read interrupt to the interrupt controller to accept
	RET_IF_NOT_SUCCESS(FPG::FBRead_ConnectToInterruptController(&IntrController, XPAR_FABRIC_V_FRMBUF_RD_0_VEC_ID, &FBRead));
	RET_IF_NOT_SUCCESS(FPG::FBWrite_ConnectToInterruptController(&IntrController, XPAR_FABRIC_V_FRMBUF_WR_0_VEC_ID, &FBWrite));

	// Register the handler from the generic interrupt controller driver
	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT, (Xil_ExceptionHandler)XScuGic_InterruptHandler, &IntrController);

	// Enable the Framebuffer Read connection to the GIC
	XScuGic_Enable(&IntrController, XPAR_FABRIC_V_FRMBUF_RD_0_VEC_ID);
	XScuGic_Enable(&IntrController, XPAR_FABRIC_V_FRMBUF_WR_0_VEC_ID);

	// Video Timing Controller
	RET_IF_NOT_SUCCESS(FPG::VTC_InitializeDriveer(&VTC));

	// Enable the generator
	FPG::VTC_EnableGenerator(&VTC);

	// Initialize the Framebuffer R/W
	RET_IF_NOT_SUCCESS(FPG::FBRead_InitializeDriver(&FBRead, FPG::FRMBUF_RD0_DEVICE_ID));
	RET_IF_NOT_SUCCESS(FPG::FBWrite_InitializeDriver(&FBWrite, FPG::FRMBUF_WR0_DEVICE_ID));

	Xil_ExceptionEnable();

	// Set the Framebuffer R/W DONE callback function
	XVFrmbufRd_SetCallback(&FBRead, XVFRMBUFRD_HANDLER_DONE, (void *)XVFBRead_DoneCallback, (void *)&FBRead);
	XVFrmbufWr_SetCallback(&FBWrite, XVFRMBUFWR_HANDLER_DONE, (void *)XVFBWrite_DoneCallback, (void *)&FBWrite);

	assert(FBRead.FrmbufRd.Config.AXIMMDataWidth == FBWrite.FrmbufWr.Config.AXIMMDataWidth);

	auto MMWidthInBytes = FBRead.FrmbufRd.Config.AXIMMDataWidth / 8;
	auto stride = ((((1280 * 3)) + MMWidthInBytes - 1) / MMWidthInBytes) * MMWidthInBytes;

	XVidC_VideoStream stream{};
	stream.PixPerClk = FPG::PIXELS_PER_CLOCK;
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
		xil_printf("Setting the memory format of framebuffer read has failed.: %d\r\n", Status);
		return XST_FAILURE;
	}

	// Set the memory format of the framebuffer R/W
	Status = XVFrmbufWr_SetMemFormat(&FBWrite, stride, XVIDC_CSF_MEM_RGB8, &stream);
	if (Status != XST_SUCCESS)
	{
		xil_printf("Setting the memory format of framebuffer write has failed.: %d\r\n", Status);
		return XST_FAILURE;
	}

	memset((void *)FPG::FRAMEBUFFER1_ADDRESS, 0xFF, 1280ULL * 720ULL * 3);

	// Set the buffer addresses
	Status = XVFrmbufRd_SetBufferAddr(&FBRead, FPG::FRAMEBUFFER1_ADDRESS);
	if (Status != XST_SUCCESS)
	{
		xil_printf("Setting the Framebuffer read address has failed.: %d\r\n", Status);
		return XST_FAILURE;
	}

	Status = XVFrmbufWr_SetBufferAddr(&FBWrite, FPG::FRAMEBUFFER0_ADDRESS);
	if (Status != XST_SUCCESS)
	{
		xil_printf("Setting the Framebuffer write address has failed.: %d\r\n", Status);
		return XST_FAILURE;
	}

	XVFrmbufRd_InterruptEnable(&FBRead, XVFRMBUFRD_IRQ_DONE_MASK);
	XVFrmbufWr_InterruptEnable(&FBWrite, XVFRMBUFWR_IRQ_DONE_MASK);

	XV_tpg_Start(&TPGen);
	XVFrmbufWr_Start(&FBWrite);
	XVFrmbufRd_Start(&FBRead);

	while (true)
	{
		for (int i = 0; i < 30000000; i++)
			;
	}
	return XST_SUCCESS;
}
