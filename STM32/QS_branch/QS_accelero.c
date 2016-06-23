/*
 *  Club Robot ESEO 2015
 *
 *  Fichier : QS_accelero.c
 *  Package : Qualité Soft
 *  Description : Gestion de l'accélérométre de la catre STM32
 *  Auteur : Anthony
 */

#include "QS_accelero.h"
#include "../stm32f4xx_hal/stm32f4xx_hal.h"
#include "../stm32f4xx_hal/lis302dl.h"
#include "../stm32f4xx_hal/lis3dsh.h"

typedef enum
{
  ACCELERO_OK = 0,
  ACCELERO_ERROR = 1,
  ACCELERO_TIMEOUT = 2
}ACCELERO_StatusTypeDef;

static ACCELERO_DrvTypeDef *AcceleroDrv;
static Sint16 pDataXYZ[3];


void ACC_init(void){
	Uint8 ret = ACCELERO_ERROR;
	Uint16 ctrl = 0x0000;
	LIS302DL_InitTypeDef         lis302dl_initstruct;
	LIS302DL_FilterConfigTypeDef lis302dl_filter = {0,0,0};
	LIS3DSH_InitTypeDef          l1s3dsh_InitStruct;

	if(Lis302dlDrv.ReadID() == I_AM_LIS302DL)
	{
		/* Initialize the accelerometer driver structure */
		AcceleroDrv = &Lis302dlDrv;

		/* Set configuration of LIS302DL MEMS Accelerometer *********************/
		lis302dl_initstruct.Power_Mode = LIS302DL_LOWPOWERMODE_ACTIVE;
		lis302dl_initstruct.Output_DataRate = LIS302DL_DATARATE_100;
		lis302dl_initstruct.Axes_Enable = LIS302DL_XYZ_ENABLE;
		lis302dl_initstruct.Full_Scale = LIS302DL_FULLSCALE_2_3;
		lis302dl_initstruct.Self_Test = LIS302DL_SELFTEST_NORMAL;

		/* Configure MEMS: data rate, power mode, full scale, self test and axes */
		ctrl = (Uint16) (lis302dl_initstruct.Output_DataRate | lis302dl_initstruct.Power_Mode | \
						   lis302dl_initstruct.Full_Scale | lis302dl_initstruct.Self_Test | \
						   lis302dl_initstruct.Axes_Enable);

		/* Configure the accelerometer main parameters */
		AcceleroDrv->Init(ctrl);

		/* MEMS High Pass Filter configuration */
		lis302dl_filter.HighPassFilter_Data_Selection = LIS302DL_FILTEREDDATASELECTION_OUTPUTREGISTER;
		lis302dl_filter.HighPassFilter_CutOff_Frequency = LIS302DL_HIGHPASSFILTER_LEVEL_1;
		lis302dl_filter.HighPassFilter_Interrupt = LIS302DL_HIGHPASSFILTERINTERRUPT_1_2;

		/* Configure MEMS high pass filter cut-off level, interrupt and data selection bits */
		ctrl = (Uint8)(lis302dl_filter.HighPassFilter_Data_Selection | \
						lis302dl_filter.HighPassFilter_CutOff_Frequency | \
						lis302dl_filter.HighPassFilter_Interrupt);

		/* Configure the accelerometer LPF main parameters */
		AcceleroDrv->FilterConfig(ctrl);

		ret = ACCELERO_OK;
	}
	else if(Lis3dshDrv.ReadID() == I_AM_LIS3DSH)
	{
		/* Initialize the accelerometer driver structure */
		AcceleroDrv = &Lis3dshDrv;

		/* Set configuration of LIS3DSH MEMS Accelerometer **********************/
		l1s3dsh_InitStruct.Output_DataRate = LIS3DSH_DATARATE_100;
		l1s3dsh_InitStruct.Axes_Enable = LIS3DSH_XYZ_ENABLE;
		l1s3dsh_InitStruct.SPI_Wire = LIS3DSH_SERIALINTERFACE_4WIRE;
		l1s3dsh_InitStruct.Self_Test = LIS3DSH_SELFTEST_NORMAL;
		l1s3dsh_InitStruct.Full_Scale = LIS3DSH_FULLSCALE_2;
		l1s3dsh_InitStruct.Filter_BW = LIS3DSH_FILTER_BW_800;

		/* Configure MEMS: power mode(ODR) and axes enable */
		ctrl = (Uint16) (l1s3dsh_InitStruct.Output_DataRate | l1s3dsh_InitStruct.Axes_Enable);

		/* Configure MEMS: full scale and self test */
		ctrl |= (Uint16) ((l1s3dsh_InitStruct.SPI_Wire   | \
							l1s3dsh_InitStruct.Self_Test   | \
							l1s3dsh_InitStruct.Full_Scale  | \
							l1s3dsh_InitStruct.Filter_BW) << 8);

		/* Configure the accelerometer main parameters */
		AcceleroDrv->Init(ctrl);

		ret = ACCELERO_OK;
	}
}

Uint8 ACC_readID(void){
  Uint8 id = 0x00;

  if(AcceleroDrv->ReadID != NULL)
  {
	id = AcceleroDrv->ReadID();
  }
  return id;
}

void ACC_reset(void){
  if(AcceleroDrv->Reset != NULL)
  {
	AcceleroDrv->Reset();
  }
}


void ACC_deInit(void){
  if(AcceleroDrv->DeInit != NULL)
  {
	AcceleroDrv->DeInit();
  }
}

void ACC_read(void){
  Sint16 SwitchXY = 0;

  if(AcceleroDrv->GetXYZ != NULL)
  {
	AcceleroDrv->GetXYZ(pDataXYZ); // pDataXYZ[0] = X axis, pDataXYZ[1] = Y axis, pDataXYZ[2] = Z axis

	/* Switch X and Y Axes in case of LIS302DL MEMS */
	if(AcceleroDrv == &Lis302dlDrv)
	{
	  SwitchXY  = pDataXYZ[0];
	  pDataXYZ[0] = pDataXYZ[1];
	  /* Invert Y Axis to be compliant with LIS3DSH MEMS */
	  pDataXYZ[1] = -SwitchXY;
	}
  }
}

Sint16 ACC_getX(void){
	return pDataXYZ[0];
}

Sint16 ACC_getY(void){
	return pDataXYZ[1];
}

Sint16 ACC_getZ(void){
	return pDataXYZ[2];
}

