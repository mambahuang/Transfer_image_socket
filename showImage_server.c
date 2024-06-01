/*
 * vdmaTest.c
 *
 *  Created on: Apr 9, 2020
 *      Author: VIPIN
 */
#include "xparameters.h"
#include "xaxivdma.h"
#include "xscugic.h"
#include "sleep.h"
#include <stdlib.h>
#include "xil_cache.h"
#include "hdmi_config.h"
#include "xvprocss.h"
#include "myip.h"

#include "r_channel_alt.h"
#include "g_channel_alt.h"
#include "b_channel_alt.h"

#define HSize 1920
#define VSize 1080
#define FrameSize HSize*VSize*3

unsigned char Buffer[FrameSize];
u32 result_r[HSize*VSize];
u32 result_g[HSize*VSize];
u32 result_b[HSize*VSize];

XVidC_VideoMode resId;
XVidC_VideoTiming const *TimingPtr;
XVidC_ColorFormat colorFmtIn = XVIDC_CSF_RGB;

//Declare an instance of the subsystem object in the application code
XVprocSs VprocInst;

//Declare the VPSS config pointer
XVprocSs_Config *VprocCfgPtr;

//Declare stream in and stream out instances
XVidC_VideoStream StreamIn, StreamOut;

int main(){
	int status;
	int Index;
	u32 Addr;

	/* HDMI Configuration*/
	print("----------------------------------\n\r");
	print(" ZedBoard-ADV7511 HDMI Test!\n\r");
	print("----------------------------------\n\r");

	print("HDMI Configuration\n\r");
	hdmi_config(XPAR_AXI_IIC_0_BASEADDR);
	/* End of HDMI Configuration*/

	/* VPSS Configuration*/

	VprocCfgPtr = XVprocSs_LookupConfig(XPAR_V_PROC_SS_0_CSC_DEVICE_ID);
	XVprocSs_CfgInitialize(&VprocInst, VprocCfgPtr, VprocCfgPtr->BaseAddress);

	//Get the resolution details
	resId = XVidC_GetVideoModeId(1920, 1080, XVIDC_FR_60HZ, 0);
	TimingPtr = XVidC_GetTimingInfo(resId);

	//Set the input stream
	StreamIn.VmId           = resId;
	StreamIn.Timing         = *TimingPtr;
	StreamIn.ColorFormatId  = colorFmtIn;
	StreamIn.ColorDepth     = VprocCfgPtr->ColorDepth;
	StreamIn.PixPerClk      = VprocCfgPtr->PixPerClock;
	StreamIn.FrameRate      = XVIDC_FR_60HZ;
	StreamIn.IsInterlaced   = 0;
	XVprocSs_SetVidStreamIn(&VprocInst, &StreamIn);

	//Set the input stream
	StreamOut.VmId           = resId;
	StreamOut.Timing         = *TimingPtr;
	StreamOut.ColorFormatId  = XVIDC_CSF_YCRCB_422;
	StreamOut.ColorDepth     = VprocCfgPtr->ColorDepth;
	StreamOut.PixPerClk      = VprocCfgPtr->PixPerClock;
	StreamOut.FrameRate      = XVIDC_FR_60HZ;
	StreamOut.IsInterlaced   = 0;
	XVprocSs_SetVidStreamOut(&VprocInst, &StreamOut);

	XVprocSs_Start(&VprocInst);

	status = XVprocSs_SetSubsystemConfig(&VprocInst);
	if(status!= XST_SUCCESS)
	{
			xil_printf("VPSS failed\r\n");
			return(XST_FAILURE);
	}
	xil_printf("VPSS Started\r\n");

	/* Endf of VPSS configuration */

	//VDMA Initialize
	xil_printf("Start VDMA Initialization");

	XAxiVdma myVDMA;
	XAxiVdma_Config *config = XAxiVdma_LookupConfig(XPAR_AXI_VDMA_0_DEVICE_ID);
	XAxiVdma_DmaSetup ReadCfg;
	status = XAxiVdma_CfgInitialize(&myVDMA, config, config->BaseAddress);
	if(status != XST_SUCCESS){
		xil_printf("VDMA Initialization failed");
	}

	ReadCfg.VertSizeInput = VSize;
	ReadCfg.HoriSizeInput = HSize*3;
	ReadCfg.Stride = HSize*3;
	ReadCfg.FrameDelay = 0;
	ReadCfg.EnableCircularBuf = 1;
	ReadCfg.EnableSync = 1;
	ReadCfg.PointNum = 0;
	ReadCfg.EnableFrameCounter = 0;
	ReadCfg.FixedFrameStoreAddr = 0;
	status = XAxiVdma_DmaConfig(&myVDMA, XAXIVDMA_READ, &ReadCfg);
	if (status != XST_SUCCESS) {
		xil_printf("Write channel config failed %d\r\n", status);
		return status;
	}

	Addr = (u32)&(Buffer[0]);

	for(Index = 0; Index < myVDMA.MaxNumFrames; Index++) {
		ReadCfg.FrameStoreStartAddr[Index] = Addr;
		Addr +=  FrameSize;
	}

	status = XAxiVdma_DmaSetBufferAddr(&myVDMA, XAXIVDMA_READ,ReadCfg.FrameStoreStartAddr);
	if (status != XST_SUCCESS) {
		xil_printf("Read channel set buffer address failed %d\r\n", status);
		return XST_FAILURE;
	}

	//Fill the data
	int index=0;

	for(int i=0;i<VSize;i++){
		for(int j=0;j<HSize*3;j=j+3){
			Buffer[(i*HSize*3)+j] = g_img[index];
			Buffer[(i*HSize*3)+j+1] = b_img[index];
			Buffer[(i*HSize*3)+j+2] = r_img[index];

			index=index+1;
		}
	}

	Xil_DCacheFlush();

	status = XAxiVdma_DmaStart(&myVDMA,XAXIVDMA_READ);
	if (status != XST_SUCCESS) {
		if(status == XST_VDMA_MISMATCH_ERROR)
			xil_printf("VDMA Mismatch Error\r\n");
		return XST_FAILURE;
	}

	u32 action;
	char key[16];
	u32 key_part1, key_part2, key_part3, key_part4;
	uint8_t * const key_hex = (uint8_t*) malloc(16*sizeof(uint8_t));

	u32 enc_dec;
	u32 dec_enable=0x00000000;


	u32 text_part1, text_part2, text_part3, text_part4;
	u32 out_valid;
	u32 out_part1, out_part2, out_part3, out_part4;


	u32 r_resize;
	u32 g_resize;
	u32 b_resize;
	u32 output_pixel;

	while(1)
	{

		printf("Action? (0:encryption, 1:decryption, others: exit)");
		scanf("%x", &action);
		print("\n\r");

		if(action==0x00000000)
		{
			printf("Please enter your key:");
//			scanf("%x%x%x%x", &key_part1, &key_part2, &key_part3, &key_part4);
//			printf("Your key is %x_%x_%x_%x\n\r", key_part1, key_part2, key_part3, key_part4);
			scanf("%s", key);
			for (int i = 0; i < 16; ++i)
			{
				key_hex[i] = (uint8_t)key[i];
			}
//			printf("Your key is: ");
			for (int i = 0; i < 16; i++) {
				printf("%x ", key_hex[i]);
			}
			printf("\n");
//			scanf("%x%x%x%x", &key_part1, &key_part2, &key_part3, &key_part4);
//			printf("Your key is %x_%x_%x_%x\n\r", key_part1, key_part2, key_part3, key_part4);
			key_part1 = (key_hex[0] << 24) | (key_hex[1] << 16) | (key_hex[2] << 8) | key_hex[3];
			key_part2 = (key_hex[4] << 24) | (key_hex[5] << 16) | (key_hex[6] << 8) | key_hex[7];
			key_part3 = (key_hex[8] << 24) | (key_hex[9] << 16) | (key_hex[10] << 8) | key_hex[11];
			key_part4 = (key_hex[12] << 24) | (key_hex[13] << 16) | (key_hex[14] << 8) | key_hex[15];
			printf("Your key is %x_%x_%x_%x\n\r", key_part1, key_part2, key_part3, key_part4);

			MYIP_mWriteReg(XPAR_MYIP_0_S00_AXI_BASEADDR,MYIP_S00_AXI_SLV_REG9_OFFSET,key_part1);
			MYIP_mWriteReg(XPAR_MYIP_0_S00_AXI_BASEADDR,MYIP_S00_AXI_SLV_REG8_OFFSET,key_part2);
			MYIP_mWriteReg(XPAR_MYIP_0_S00_AXI_BASEADDR,MYIP_S00_AXI_SLV_REG7_OFFSET,key_part3);
			MYIP_mWriteReg(XPAR_MYIP_0_S00_AXI_BASEADDR,MYIP_S00_AXI_SLV_REG6_OFFSET,key_part4);

			enc_dec=0x00000000;	//encryption
			printf("Start encryption\n\r");


			for(int i=0; i<129600; i++)
			{
				text_part1=(r_img[i*16]<<24)+(r_img[i*16+1]<<16)+(r_img[i*16+2]<<8)+r_img[i*16+3];
				text_part2=(r_img[i*16+4]<<24)+(r_img[i*16+5]<<16)+(r_img[i*16+6]<<8)+r_img[i*16+7];
				text_part3=(r_img[i*16+8]<<24)+(r_img[i*16+9]<<16)+(r_img[i*16+10]<<8)+r_img[i*16+11];
				text_part4=(r_img[i*16+12]<<24)+(r_img[i*16+13]<<16)+(r_img[i*16+14]<<8)+r_img[i*16+15];

//				printf("%x , %x , %x , %x\n\r", text_part1, text_part2, text_part3, text_part4);

				MYIP_mWriteReg(XPAR_MYIP_0_S00_AXI_BASEADDR,MYIP_S00_AXI_SLV_REG5_OFFSET,text_part1);
				MYIP_mWriteReg(XPAR_MYIP_0_S00_AXI_BASEADDR,MYIP_S00_AXI_SLV_REG4_OFFSET,text_part2);
				MYIP_mWriteReg(XPAR_MYIP_0_S00_AXI_BASEADDR,MYIP_S00_AXI_SLV_REG3_OFFSET,text_part3);
				MYIP_mWriteReg(XPAR_MYIP_0_S00_AXI_BASEADDR,MYIP_S00_AXI_SLV_REG2_OFFSET,text_part4);

				MYIP_mWriteReg(XPAR_MYIP_0_S00_AXI_BASEADDR,MYIP_S00_AXI_SLV_REG0_OFFSET,enc_dec);

				//start
				MYIP_mWriteReg(XPAR_MYIP_0_S00_AXI_BASEADDR,MYIP_S00_AXI_SLV_REG1_OFFSET,0x00000001);
				MYIP_mWriteReg(XPAR_MYIP_0_S00_AXI_BASEADDR,MYIP_S00_AXI_SLV_REG1_OFFSET,0x00000000);


				//wait out valid
				out_valid=MYIP_mReadReg(XPAR_MYIP_0_S00_AXI_BASEADDR,MYIP_S00_AXI_SLV_REG0_OFFSET);

				while(out_valid!=0x00000001)
				{
					out_valid=MYIP_mReadReg(XPAR_MYIP_0_S00_AXI_BASEADDR,MYIP_S00_AXI_SLV_REG0_OFFSET);
				}

				out_part1=MYIP_mReadReg(XPAR_MYIP_0_S00_AXI_BASEADDR,MYIP_S00_AXI_SLV_REG4_OFFSET);
				out_part2=MYIP_mReadReg(XPAR_MYIP_0_S00_AXI_BASEADDR,MYIP_S00_AXI_SLV_REG3_OFFSET);
				out_part3=MYIP_mReadReg(XPAR_MYIP_0_S00_AXI_BASEADDR,MYIP_S00_AXI_SLV_REG2_OFFSET);
				out_part4=MYIP_mReadReg(XPAR_MYIP_0_S00_AXI_BASEADDR,MYIP_S00_AXI_SLV_REG1_OFFSET);

//				printf("%08x%08x%08x%08x\n\r", out_part1, out_part2, out_part3, out_part4);

				result_r[i*16]=out_part1>>24;
				result_r[i*16+1]=(out_part1-(result_r[i*16]<<24))>>16;
				result_r[i*16+2]=(out_part1-(result_r[i*16]<<24)-(result_r[i*16+1]<<16))>>8;
				result_r[i*16+3]=(out_part1-(result_r[i*16]<<24)-(result_r[i*16+1]<<16)-(result_r[i*16+2]<<8));

				result_r[i*16+4]=out_part2>>24;
				result_r[i*16+5]=(out_part2-(result_r[i*16+4]<<24))>>16;
				result_r[i*16+6]=(out_part2-(result_r[i*16+4]<<24)-(result_r[i*16+5]<<16))>>8;
				result_r[i*16+7]=(out_part2-(result_r[i*16+4]<<24)-(result_r[i*16+5]<<16)-(result_r[i*16+6]<<8));

				result_r[i*16+8]=out_part3>>24;
				result_r[i*16+9]=(out_part3-(result_r[i*16+8]<<24))>>16;
				result_r[i*16+10]=(out_part3-(result_r[i*16+8]<<24)-(result_r[i*16+9]<<16))>>8;
				result_r[i*16+11]=(out_part3-(result_r[i*16+8]<<24)-(result_r[i*16+9]<<16)-(result_r[i*16+10]<<8));

				result_r[i*16+12]=out_part4>>24;
				result_r[i*16+13]=(out_part4-(result_r[i*16+12]<<24))>>16;
				result_r[i*16+14]=(out_part4-(result_r[i*16+12]<<24)-(result_r[i*16+13]<<16))>>8;
				result_r[i*16+15]=(out_part4-(result_r[i*16+12]<<24)-(result_r[i*16+13]<<16)-(result_r[i*16+14]<<8));

//				printf("%02x%02x%02x%02x %02x%02x%02x%02x %02x%02x%02x%02x %02x%02x%02x%02x\n\r", result[i*16],result[i*16+1],result[i*16+2],result[i*16+3], result[i*16+4],result[i*16+5],result[i*16+6],result[i*16+7], result[i*16+8],result[i*16+9],result[i*16+10],result[i*16+11], result[i*16+12],result[i*16+13],result[i*16+14],result[i*16+15]);

			}

			for(int i=0; i<129600; i++)
			{
				text_part1=(g_img[i*16]<<24)+(g_img[i*16+1]<<16)+(g_img[i*16+2]<<8)+g_img[i*16+3];
				text_part2=(g_img[i*16+4]<<24)+(g_img[i*16+5]<<16)+(g_img[i*16+6]<<8)+g_img[i*16+7];
				text_part3=(g_img[i*16+8]<<24)+(g_img[i*16+9]<<16)+(g_img[i*16+10]<<8)+g_img[i*16+11];
				text_part4=(g_img[i*16+12]<<24)+(g_img[i*16+13]<<16)+(g_img[i*16+14]<<8)+g_img[i*16+15];

//				printf("%x , %x , %x , %x\n\r", text_part1, text_part2, text_part3, text_part4);

				MYIP_mWriteReg(XPAR_MYIP_0_S00_AXI_BASEADDR,MYIP_S00_AXI_SLV_REG5_OFFSET,text_part1);
				MYIP_mWriteReg(XPAR_MYIP_0_S00_AXI_BASEADDR,MYIP_S00_AXI_SLV_REG4_OFFSET,text_part2);
				MYIP_mWriteReg(XPAR_MYIP_0_S00_AXI_BASEADDR,MYIP_S00_AXI_SLV_REG3_OFFSET,text_part3);
				MYIP_mWriteReg(XPAR_MYIP_0_S00_AXI_BASEADDR,MYIP_S00_AXI_SLV_REG2_OFFSET,text_part4);

				MYIP_mWriteReg(XPAR_MYIP_0_S00_AXI_BASEADDR,MYIP_S00_AXI_SLV_REG0_OFFSET,enc_dec);

				//start
				MYIP_mWriteReg(XPAR_MYIP_0_S00_AXI_BASEADDR,MYIP_S00_AXI_SLV_REG1_OFFSET,0x00000001);
				MYIP_mWriteReg(XPAR_MYIP_0_S00_AXI_BASEADDR,MYIP_S00_AXI_SLV_REG1_OFFSET,0x00000000);


				//wait out valid
				out_valid=MYIP_mReadReg(XPAR_MYIP_0_S00_AXI_BASEADDR,MYIP_S00_AXI_SLV_REG0_OFFSET);

				while(out_valid!=0x00000001)
				{
					out_valid=MYIP_mReadReg(XPAR_MYIP_0_S00_AXI_BASEADDR,MYIP_S00_AXI_SLV_REG0_OFFSET);
				}

				out_part1=MYIP_mReadReg(XPAR_MYIP_0_S00_AXI_BASEADDR,MYIP_S00_AXI_SLV_REG4_OFFSET);
				out_part2=MYIP_mReadReg(XPAR_MYIP_0_S00_AXI_BASEADDR,MYIP_S00_AXI_SLV_REG3_OFFSET);
				out_part3=MYIP_mReadReg(XPAR_MYIP_0_S00_AXI_BASEADDR,MYIP_S00_AXI_SLV_REG2_OFFSET);
				out_part4=MYIP_mReadReg(XPAR_MYIP_0_S00_AXI_BASEADDR,MYIP_S00_AXI_SLV_REG1_OFFSET);

//				printf("%08x%08x%08x%08x\n\r", out_part1, out_part2, out_part3, out_part4);

				result_g[i*16]=out_part1>>24;
				result_g[i*16+1]=(out_part1-(result_g[i*16]<<24))>>16;
				result_g[i*16+2]=(out_part1-(result_g[i*16]<<24)-(result_g[i*16+1]<<16))>>8;
				result_g[i*16+3]=(out_part1-(result_g[i*16]<<24)-(result_g[i*16+1]<<16)-(result_g[i*16+2]<<8));

				result_g[i*16+4]=out_part2>>24;
				result_g[i*16+5]=(out_part2-(result_g[i*16+4]<<24))>>16;
				result_g[i*16+6]=(out_part2-(result_g[i*16+4]<<24)-(result_g[i*16+5]<<16))>>8;
				result_g[i*16+7]=(out_part2-(result_g[i*16+4]<<24)-(result_g[i*16+5]<<16)-(result_g[i*16+6]<<8));

				result_g[i*16+8]=out_part3>>24;
				result_g[i*16+9]=(out_part3-(result_g[i*16+8]<<24))>>16;
				result_g[i*16+10]=(out_part3-(result_g[i*16+8]<<24)-(result_g[i*16+9]<<16))>>8;
				result_g[i*16+11]=(out_part3-(result_g[i*16+8]<<24)-(result_g[i*16+9]<<16)-(result_g[i*16+10]<<8));

				result_g[i*16+12]=out_part4>>24;
				result_g[i*16+13]=(out_part4-(result_g[i*16+12]<<24))>>16;
				result_g[i*16+14]=(out_part4-(result_g[i*16+12]<<24)-(result_g[i*16+13]<<16))>>8;
				result_g[i*16+15]=(out_part4-(result_g[i*16+12]<<24)-(result_g[i*16+13]<<16)-(result_g[i*16+14]<<8));

//				printf("%02x%02x%02x%02x %02x%02x%02x%02x %02x%02x%02x%02x %02x%02x%02x%02x\n\r", result[i*16],result[i*16+1],result[i*16+2],result[i*16+3], result[i*16+4],result[i*16+5],result[i*16+6],result[i*16+7], result[i*16+8],result[i*16+9],result[i*16+10],result[i*16+11], result[i*16+12],result[i*16+13],result[i*16+14],result[i*16+15]);
			}

			for(int i=0; i<129600; i++)
			{
				text_part1=(b_img[i*16]<<24)+(b_img[i*16+1]<<16)+(b_img[i*16+2]<<8)+b_img[i*16+3];
				text_part2=(b_img[i*16+4]<<24)+(b_img[i*16+5]<<16)+(b_img[i*16+6]<<8)+b_img[i*16+7];
				text_part3=(b_img[i*16+8]<<24)+(b_img[i*16+9]<<16)+(b_img[i*16+10]<<8)+b_img[i*16+11];
				text_part4=(b_img[i*16+12]<<24)+(b_img[i*16+13]<<16)+(b_img[i*16+14]<<8)+b_img[i*16+15];

//				printf("%x , %x , %x , %x\n\r", text_part1, text_part2, text_part3, text_part4);

				MYIP_mWriteReg(XPAR_MYIP_0_S00_AXI_BASEADDR,MYIP_S00_AXI_SLV_REG5_OFFSET,text_part1);
				MYIP_mWriteReg(XPAR_MYIP_0_S00_AXI_BASEADDR,MYIP_S00_AXI_SLV_REG4_OFFSET,text_part2);
				MYIP_mWriteReg(XPAR_MYIP_0_S00_AXI_BASEADDR,MYIP_S00_AXI_SLV_REG3_OFFSET,text_part3);
				MYIP_mWriteReg(XPAR_MYIP_0_S00_AXI_BASEADDR,MYIP_S00_AXI_SLV_REG2_OFFSET,text_part4);

				MYIP_mWriteReg(XPAR_MYIP_0_S00_AXI_BASEADDR,MYIP_S00_AXI_SLV_REG0_OFFSET,enc_dec);

				//start
				MYIP_mWriteReg(XPAR_MYIP_0_S00_AXI_BASEADDR,MYIP_S00_AXI_SLV_REG1_OFFSET,0x00000001);
				MYIP_mWriteReg(XPAR_MYIP_0_S00_AXI_BASEADDR,MYIP_S00_AXI_SLV_REG1_OFFSET,0x00000000);


				//wait out valid
				out_valid=MYIP_mReadReg(XPAR_MYIP_0_S00_AXI_BASEADDR,MYIP_S00_AXI_SLV_REG0_OFFSET);

				while(out_valid!=0x00000001)
				{
					out_valid=MYIP_mReadReg(XPAR_MYIP_0_S00_AXI_BASEADDR,MYIP_S00_AXI_SLV_REG0_OFFSET);
				}

				out_part1=MYIP_mReadReg(XPAR_MYIP_0_S00_AXI_BASEADDR,MYIP_S00_AXI_SLV_REG4_OFFSET);
				out_part2=MYIP_mReadReg(XPAR_MYIP_0_S00_AXI_BASEADDR,MYIP_S00_AXI_SLV_REG3_OFFSET);
				out_part3=MYIP_mReadReg(XPAR_MYIP_0_S00_AXI_BASEADDR,MYIP_S00_AXI_SLV_REG2_OFFSET);
				out_part4=MYIP_mReadReg(XPAR_MYIP_0_S00_AXI_BASEADDR,MYIP_S00_AXI_SLV_REG1_OFFSET);

//				printf("%08x%08x%08x%08x\n\r", out_part1, out_part2, out_part3, out_part4);

				result_b[i*16]=out_part1>>24;
				result_b[i*16+1]=(out_part1-(result_b[i*16]<<24))>>16;
				result_b[i*16+2]=(out_part1-(result_b[i*16]<<24)-(result_b[i*16+1]<<16))>>8;
				result_b[i*16+3]=(out_part1-(result_b[i*16]<<24)-(result_b[i*16+1]<<16)-(result_b[i*16+2]<<8));

				result_b[i*16+4]=out_part2>>24;
				result_b[i*16+5]=(out_part2-(result_b[i*16+4]<<24))>>16;
				result_b[i*16+6]=(out_part2-(result_b[i*16+4]<<24)-(result_b[i*16+5]<<16))>>8;
				result_b[i*16+7]=(out_part2-(result_b[i*16+4]<<24)-(result_b[i*16+5]<<16)-(result_b[i*16+6]<<8));

				result_b[i*16+8]=out_part3>>24;
				result_b[i*16+9]=(out_part3-(result_b[i*16+8]<<24))>>16;
				result_b[i*16+10]=(out_part3-(result_b[i*16+8]<<24)-(result_b[i*16+9]<<16))>>8;
				result_b[i*16+11]=(out_part3-(result_b[i*16+8]<<24)-(result_b[i*16+9]<<16)-(result_b[i*16+10]<<8));

				result_b[i*16+12]=out_part4>>24;
				result_b[i*16+13]=(out_part4-(result_b[i*16+12]<<24))>>16;
				result_b[i*16+14]=(out_part4-(result_b[i*16+12]<<24)-(result_b[i*16+13]<<16))>>8;
				result_b[i*16+15]=(out_part4-(result_b[i*16+12]<<24)-(result_b[i*16+13]<<16)-(result_b[i*16+14]<<8));

//				printf("%02x%02x%02x%02x %02x%02x%02x%02x %02x%02x%02x%02x %02x%02x%02x%02x\n\r", result[i*16],result[i*16+1],result[i*16+2],result[i*16+3], result[i*16+4],result[i*16+5],result[i*16+6],result[i*16+7], result[i*16+8],result[i*16+9],result[i*16+10],result[i*16+11], result[i*16+12],result[i*16+13],result[i*16+14],result[i*16+15]);

			}

			printf("%x\n\r", result_r[16*60417]);
			printf("%x\n\r", result_r[16*60417 + 1]);

//			for(int i=0; i<50; i++)
//			{
//				printf("%02x%02x%02x%02x %02x%02x%02x%02x %02x%02x%02x%02x %02x%02x%02x%02x\n\r", result[i*16],result[i*16+1],result[i*16+2],result[i*16+3], result[i*16+4],result[i*16+5],result[i*16+6],result[i*16+7], result[i*16+8],result[i*16+9],result[i*16+10],result[i*16+11], result[i*16+12],result[i*16+13],result[i*16+14],result[i*16+15]);
//			}

			dec_enable=0x00000001;
		}
		else if(action==0x00000001)
		{
			if(dec_enable==0x00000000)
			{
				printf("Please run encryption before start decryption.\n\r");
				continue;
			}

			printf("Please enter your key:");
			scanf("%s", key);
			for (int i = 0; i < 16; ++i)
			{
				key_hex[i] = (uint8_t)key[i];
			}

			for (int i = 0; i < 16; i++) {
				printf("%x ", key_hex[i]);
			}
			printf("\n\r");

//			scanf("%x%x%x%x", &key_part1, &key_part2, &key_part3, &key_part4);
//			printf("Your key is %x_%x_%x_%x\n\r", key_part1, key_part2, key_part3, key_part4);
			key_part1 = (key_hex[0] << 24) | (key_hex[1] << 16) | (key_hex[2] << 8) | key_hex[3];
			key_part2 = (key_hex[4] << 24) | (key_hex[5] << 16) | (key_hex[6] << 8) | key_hex[7];
			key_part3 = (key_hex[8] << 24) | (key_hex[9] << 16) | (key_hex[10] << 8) | key_hex[11];
			key_part4 = (key_hex[12] << 24) | (key_hex[13] << 16) | (key_hex[14] << 8) | key_hex[15];
			printf("Your key is %x_%x_%x_%x\n\r", key_part1, key_part2, key_part3, key_part4);

			MYIP_mWriteReg(XPAR_MYIP_0_S00_AXI_BASEADDR,MYIP_S00_AXI_SLV_REG9_OFFSET,key_part1);
			MYIP_mWriteReg(XPAR_MYIP_0_S00_AXI_BASEADDR,MYIP_S00_AXI_SLV_REG8_OFFSET,key_part2);
			MYIP_mWriteReg(XPAR_MYIP_0_S00_AXI_BASEADDR,MYIP_S00_AXI_SLV_REG7_OFFSET,key_part3);
			MYIP_mWriteReg(XPAR_MYIP_0_S00_AXI_BASEADDR,MYIP_S00_AXI_SLV_REG6_OFFSET,key_part4);

			enc_dec=0x00000001;	//decryption
			printf("Start decryption\n\r");


			for(int i=0; i<129600; i++)
			{
				text_part1=(result_r[i*16]<<24)+(result_r[i*16+1]<<16)+(result_r[i*16+2]<<8)+result_r[i*16+3];
				text_part2=(result_r[i*16+4]<<24)+(result_r[i*16+5]<<16)+(result_r[i*16+6]<<8)+result_r[i*16+7];
				text_part3=(result_r[i*16+8]<<24)+(result_r[i*16+9]<<16)+(result_r[i*16+10]<<8)+result_r[i*16+11];
				text_part4=(result_r[i*16+12]<<24)+(result_r[i*16+13]<<16)+(result_r[i*16+14]<<8)+result_r[i*16+15];

//				printf("%x , %x , %x , %x\n\r", text_part1, text_part2, text_part3, text_part4);

				MYIP_mWriteReg(XPAR_MYIP_0_S00_AXI_BASEADDR,MYIP_S00_AXI_SLV_REG5_OFFSET,text_part1);
				MYIP_mWriteReg(XPAR_MYIP_0_S00_AXI_BASEADDR,MYIP_S00_AXI_SLV_REG4_OFFSET,text_part2);
				MYIP_mWriteReg(XPAR_MYIP_0_S00_AXI_BASEADDR,MYIP_S00_AXI_SLV_REG3_OFFSET,text_part3);
				MYIP_mWriteReg(XPAR_MYIP_0_S00_AXI_BASEADDR,MYIP_S00_AXI_SLV_REG2_OFFSET,text_part4);

				MYIP_mWriteReg(XPAR_MYIP_0_S00_AXI_BASEADDR,MYIP_S00_AXI_SLV_REG0_OFFSET,enc_dec);

				//start
				MYIP_mWriteReg(XPAR_MYIP_0_S00_AXI_BASEADDR,MYIP_S00_AXI_SLV_REG1_OFFSET,0x00000001);
				MYIP_mWriteReg(XPAR_MYIP_0_S00_AXI_BASEADDR,MYIP_S00_AXI_SLV_REG1_OFFSET,0x00000000);


				//wait out valid
				out_valid=MYIP_mReadReg(XPAR_MYIP_0_S00_AXI_BASEADDR,MYIP_S00_AXI_SLV_REG0_OFFSET);

				while(out_valid!=0x00000001)
				{
					out_valid=MYIP_mReadReg(XPAR_MYIP_0_S00_AXI_BASEADDR,MYIP_S00_AXI_SLV_REG0_OFFSET);
				}

				out_part1=MYIP_mReadReg(XPAR_MYIP_0_S00_AXI_BASEADDR,MYIP_S00_AXI_SLV_REG4_OFFSET);
				out_part2=MYIP_mReadReg(XPAR_MYIP_0_S00_AXI_BASEADDR,MYIP_S00_AXI_SLV_REG3_OFFSET);
				out_part3=MYIP_mReadReg(XPAR_MYIP_0_S00_AXI_BASEADDR,MYIP_S00_AXI_SLV_REG2_OFFSET);
				out_part4=MYIP_mReadReg(XPAR_MYIP_0_S00_AXI_BASEADDR,MYIP_S00_AXI_SLV_REG1_OFFSET);

//				printf("%08x%08x%08x%08x\n\r", out_part1, out_part2, out_part3, out_part4);

				result_r[i*16]=out_part1>>24;
				result_r[i*16+1]=(out_part1-(result_r[i*16]<<24))>>16;
				result_r[i*16+2]=(out_part1-(result_r[i*16]<<24)-(result_r[i*16+1]<<16))>>8;
				result_r[i*16+3]=(out_part1-(result_r[i*16]<<24)-(result_r[i*16+1]<<16)-(result_r[i*16+2]<<8));

				result_r[i*16+4]=out_part2>>24;
				result_r[i*16+5]=(out_part2-(result_r[i*16+4]<<24))>>16;
				result_r[i*16+6]=(out_part2-(result_r[i*16+4]<<24)-(result_r[i*16+5]<<16))>>8;
				result_r[i*16+7]=(out_part2-(result_r[i*16+4]<<24)-(result_r[i*16+5]<<16)-(result_r[i*16+6]<<8));

				result_r[i*16+8]=out_part3>>24;
				result_r[i*16+9]=(out_part3-(result_r[i*16+8]<<24))>>16;
				result_r[i*16+10]=(out_part3-(result_r[i*16+8]<<24)-(result_r[i*16+9]<<16))>>8;
				result_r[i*16+11]=(out_part3-(result_r[i*16+8]<<24)-(result_r[i*16+9]<<16)-(result_r[i*16+10]<<8));

				result_r[i*16+12]=out_part4>>24;
				result_r[i*16+13]=(out_part4-(result_r[i*16+12]<<24))>>16;
				result_r[i*16+14]=(out_part4-(result_r[i*16+12]<<24)-(result_r[i*16+13]<<16))>>8;
				result_r[i*16+15]=(out_part4-(result_r[i*16+12]<<24)-(result_r[i*16+13]<<16)-(result_r[i*16+14]<<8));


//				printf("%02x%02x%02x%02x %02x%02x%02x%02x %02x%02x%02x%02x %02x%02x%02x%02x\n\r", result[i*16],result[i*16+1],result[i*16+2],result[i*16+3], result[i*16+4],result[i*16+5],result[i*16+6],result[i*16+7], result[i*16+8],result[i*16+9],result[i*16+10],result[i*16+11], result[i*16+12],result[i*16+13],result[i*16+14],result[i*16+15]);

			}

			for(int i=0; i<129600; i++)
			{
				text_part1=(result_g[i*16]<<24)+(result_g[i*16+1]<<16)+(result_g[i*16+2]<<8)+result_g[i*16+3];
				text_part2=(result_g[i*16+4]<<24)+(result_g[i*16+5]<<16)+(result_g[i*16+6]<<8)+result_g[i*16+7];
				text_part3=(result_g[i*16+8]<<24)+(result_g[i*16+9]<<16)+(result_g[i*16+10]<<8)+result_g[i*16+11];
				text_part4=(result_g[i*16+12]<<24)+(result_g[i*16+13]<<16)+(result_g[i*16+14]<<8)+result_g[i*16+15];

		//				printf("%x , %x , %x , %x\n\r", text_part1, text_part2, text_part3, text_part4);

				MYIP_mWriteReg(XPAR_MYIP_0_S00_AXI_BASEADDR,MYIP_S00_AXI_SLV_REG5_OFFSET,text_part1);
				MYIP_mWriteReg(XPAR_MYIP_0_S00_AXI_BASEADDR,MYIP_S00_AXI_SLV_REG4_OFFSET,text_part2);
				MYIP_mWriteReg(XPAR_MYIP_0_S00_AXI_BASEADDR,MYIP_S00_AXI_SLV_REG3_OFFSET,text_part3);
				MYIP_mWriteReg(XPAR_MYIP_0_S00_AXI_BASEADDR,MYIP_S00_AXI_SLV_REG2_OFFSET,text_part4);

				MYIP_mWriteReg(XPAR_MYIP_0_S00_AXI_BASEADDR,MYIP_S00_AXI_SLV_REG0_OFFSET,enc_dec);

				//start
				MYIP_mWriteReg(XPAR_MYIP_0_S00_AXI_BASEADDR,MYIP_S00_AXI_SLV_REG1_OFFSET,0x00000001);
				MYIP_mWriteReg(XPAR_MYIP_0_S00_AXI_BASEADDR,MYIP_S00_AXI_SLV_REG1_OFFSET,0x00000000);


				//wait out valid
				out_valid=MYIP_mReadReg(XPAR_MYIP_0_S00_AXI_BASEADDR,MYIP_S00_AXI_SLV_REG0_OFFSET);

				while(out_valid!=0x00000001)
				{
					out_valid=MYIP_mReadReg(XPAR_MYIP_0_S00_AXI_BASEADDR,MYIP_S00_AXI_SLV_REG0_OFFSET);
				}

				usleep(1);

				out_part1=MYIP_mReadReg(XPAR_MYIP_0_S00_AXI_BASEADDR,MYIP_S00_AXI_SLV_REG4_OFFSET);
				out_part2=MYIP_mReadReg(XPAR_MYIP_0_S00_AXI_BASEADDR,MYIP_S00_AXI_SLV_REG3_OFFSET);
				out_part3=MYIP_mReadReg(XPAR_MYIP_0_S00_AXI_BASEADDR,MYIP_S00_AXI_SLV_REG2_OFFSET);
				out_part4=MYIP_mReadReg(XPAR_MYIP_0_S00_AXI_BASEADDR,MYIP_S00_AXI_SLV_REG1_OFFSET);

//				printf("%08x%08x%08x%08x\n\r", out_part1, out_part2, out_part3, out_part4);

				result_g[i*16]=out_part1>>24;
				result_g[i*16+1]=(out_part1-(result_g[i*16]<<24))>>16;
				result_g[i*16+2]=(out_part1-(result_g[i*16]<<24)-(result_g[i*16+1]<<16))>>8;
				result_g[i*16+3]=(out_part1-(result_g[i*16]<<24)-(result_g[i*16+1]<<16)-(result_g[i*16+2]<<8));

				result_g[i*16+4]=out_part2>>24;
				result_g[i*16+5]=(out_part2-(result_g[i*16+4]<<24))>>16;
				result_g[i*16+6]=(out_part2-(result_g[i*16+4]<<24)-(result_g[i*16+5]<<16))>>8;
				result_g[i*16+7]=(out_part2-(result_g[i*16+4]<<24)-(result_g[i*16+5]<<16)-(result_g[i*16+6]<<8));

				result_g[i*16+8]=out_part3>>24;
				result_g[i*16+9]=(out_part3-(result_g[i*16+8]<<24))>>16;
				result_g[i*16+10]=(out_part3-(result_g[i*16+8]<<24)-(result_g[i*16+9]<<16))>>8;
				result_g[i*16+11]=(out_part3-(result_g[i*16+8]<<24)-(result_g[i*16+9]<<16)-(result_g[i*16+10]<<8));

				result_g[i*16+12]=out_part4>>24;
				result_g[i*16+13]=(out_part4-(result_g[i*16+12]<<24))>>16;
				result_g[i*16+14]=(out_part4-(result_g[i*16+12]<<24)-(result_g[i*16+13]<<16))>>8;
				result_g[i*16+15]=(out_part4-(result_g[i*16+12]<<24)-(result_g[i*16+13]<<16)-(result_g[i*16+14]<<8));



//				printf("%02x%02x%02x%02x %02x%02x%02x%02x %02x%02x%02x%02x %02x%02x%02x%02x\n\r", result[i*16],result[i*16+1],result[i*16+2],result[i*16+3], result[i*16+4],result[i*16+5],result[i*16+6],result[i*16+7], result[i*16+8],result[i*16+9],result[i*16+10],result[i*16+11], result[i*16+12],result[i*16+13],result[i*16+14],result[i*16+15]);

			}

			for(int i=0; i<129600; i++)
			{
				text_part1=(result_b[i*16]<<24)+(result_b[i*16+1]<<16)+(result_b[i*16+2]<<8)+result_b[i*16+3];
				text_part2=(result_b[i*16+4]<<24)+(result_b[i*16+5]<<16)+(result_b[i*16+6]<<8)+result_b[i*16+7];
				text_part3=(result_b[i*16+8]<<24)+(result_b[i*16+9]<<16)+(result_b[i*16+10]<<8)+result_b[i*16+11];
				text_part4=(result_b[i*16+12]<<24)+(result_b[i*16+13]<<16)+(result_b[i*16+14]<<8)+result_b[i*16+15];

//				printf("%x , %x , %x , %x\n\r", text_part1, text_part2, text_part3, text_part4);

				MYIP_mWriteReg(XPAR_MYIP_0_S00_AXI_BASEADDR,MYIP_S00_AXI_SLV_REG5_OFFSET,text_part1);
				MYIP_mWriteReg(XPAR_MYIP_0_S00_AXI_BASEADDR,MYIP_S00_AXI_SLV_REG4_OFFSET,text_part2);
				MYIP_mWriteReg(XPAR_MYIP_0_S00_AXI_BASEADDR,MYIP_S00_AXI_SLV_REG3_OFFSET,text_part3);
				MYIP_mWriteReg(XPAR_MYIP_0_S00_AXI_BASEADDR,MYIP_S00_AXI_SLV_REG2_OFFSET,text_part4);

				MYIP_mWriteReg(XPAR_MYIP_0_S00_AXI_BASEADDR,MYIP_S00_AXI_SLV_REG0_OFFSET,enc_dec);

				//start
				MYIP_mWriteReg(XPAR_MYIP_0_S00_AXI_BASEADDR,MYIP_S00_AXI_SLV_REG1_OFFSET,0x00000001);
				MYIP_mWriteReg(XPAR_MYIP_0_S00_AXI_BASEADDR,MYIP_S00_AXI_SLV_REG1_OFFSET,0x00000000);


				//wait out valid
				out_valid=MYIP_mReadReg(XPAR_MYIP_0_S00_AXI_BASEADDR,MYIP_S00_AXI_SLV_REG0_OFFSET);

				while(out_valid!=0x00000001)
				{
					out_valid=MYIP_mReadReg(XPAR_MYIP_0_S00_AXI_BASEADDR,MYIP_S00_AXI_SLV_REG0_OFFSET);
				}

				usleep(1);

				out_part1=MYIP_mReadReg(XPAR_MYIP_0_S00_AXI_BASEADDR,MYIP_S00_AXI_SLV_REG4_OFFSET);
				out_part2=MYIP_mReadReg(XPAR_MYIP_0_S00_AXI_BASEADDR,MYIP_S00_AXI_SLV_REG3_OFFSET);
				out_part3=MYIP_mReadReg(XPAR_MYIP_0_S00_AXI_BASEADDR,MYIP_S00_AXI_SLV_REG2_OFFSET);
				out_part4=MYIP_mReadReg(XPAR_MYIP_0_S00_AXI_BASEADDR,MYIP_S00_AXI_SLV_REG1_OFFSET);

//				printf("%08x%08x%08x%08x\n\r", out_part1, out_part2, out_part3, out_part4);

				result_b[i*16]=out_part1>>24;
				result_b[i*16+1]=(out_part1-(result_b[i*16]<<24))>>16;
				result_b[i*16+2]=(out_part1-(result_b[i*16]<<24)-(result_b[i*16+1]<<16))>>8;
				result_b[i*16+3]=(out_part1-(result_b[i*16]<<24)-(result_b[i*16+1]<<16)-(result_b[i*16+2]<<8));

				result_b[i*16+4]=out_part2>>24;
				result_b[i*16+5]=(out_part2-(result_b[i*16+4]<<24))>>16;
				result_b[i*16+6]=(out_part2-(result_b[i*16+4]<<24)-(result_b[i*16+5]<<16))>>8;
				result_b[i*16+7]=(out_part2-(result_b[i*16+4]<<24)-(result_b[i*16+5]<<16)-(result_b[i*16+6]<<8));

				result_b[i*16+8]=out_part3>>24;
				result_b[i*16+9]=(out_part3-(result_b[i*16+8]<<24))>>16;
				result_b[i*16+10]=(out_part3-(result_b[i*16+8]<<24)-(result_b[i*16+9]<<16))>>8;
				result_b[i*16+11]=(out_part3-(result_b[i*16+8]<<24)-(result_b[i*16+9]<<16)-(result_b[i*16+10]<<8));

				result_b[i*16+12]=out_part4>>24;
				result_b[i*16+13]=(out_part4-(result_b[i*16+12]<<24))>>16;
				result_b[i*16+14]=(out_part4-(result_b[i*16+12]<<24)-(result_b[i*16+13]<<16))>>8;
				result_b[i*16+15]=(out_part4-(result_b[i*16+12]<<24)-(result_b[i*16+13]<<16)-(result_b[i*16+14]<<8));


//				printf("%02x%02x%02x%02x %02x%02x%02x%02x %02x%02x%02x%02x %02x%02x%02x%02x\n\r", result[i*16],result[i*16+1],result[i*16+2],result[i*16+3], result[i*16+4],result[i*16+5],result[i*16+6],result[i*16+7], result[i*16+8],result[i*16+9],result[i*16+10],result[i*16+11], result[i*16+12],result[i*16+13],result[i*16+14],result[i*16+15]);

			}
		}
		else
		{
			printf("Program Finish!!\n\r");
			break;
		}

		int print_index=0;

		for(int i=0;i<VSize;i++){
			for(int j=0;j<HSize*3;j=j+3){
				Buffer[(i*HSize*3)+j] = result_g[print_index];
				Buffer[(i*HSize*3)+j+1] = result_b[print_index];
				Buffer[(i*HSize*3)+j+2] = result_r[print_index];

				print_index=print_index+1;
			}
		}

	}

	return 0;
}
