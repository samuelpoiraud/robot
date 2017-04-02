#include "xpt2046.h"
#include "ili9341.h"
#include "../../QS/QS_spi.h"
#include "../../QS/QS_external_it.h"
#include "../../stm32f4xx/stm32f4xx_spi.h"

#define CONTROL_BYTE_START									0b10000000
#define CONTROL_BTYE_NO_START								0b00000000

#define CONTROL_BTYE_CHANNEL_SELECT_TEMP0					0b00000000
#define CONTROL_BTYE_CHANNEL_SELECT_Y						0b00010000
#define CONTROL_BTYE_CHANNEL_SELECT_VBAT					0b00100000
#define CONTROL_BTYE_CHANNEL_SELECT_Z1						0b00110000
#define CONTROL_BTYE_CHANNEL_SELECT_Z2						0b01000000
#define CONTROL_BTYE_CHANNEL_SELECT_X						0b01010000
#define CONTROL_BTYE_CHANNEL_SELECT_AUX						0b01100000
#define CONTROL_BTYE_CHANNEL_SELECT_TEMP1					0b01110000

#define CONTROL_BYTE_MODE_12_BIT							0b00000000
#define CONTROL_BYTE_MODE_8_BIT								0b00001000

#define CONTROL_BYTE_SD_DIFFERENTIAL						0b00000000
#define CONTROL_BYTE_SD_SINGLE_ENDED						0b00000100

#define CONTROL_BYTE_POWER_DOWN_MODE_LOW_POWER_IRQ			0b00000000
#define CONTROL_BYTE_POWER_DOWN_MODE_REF_ON_ADC_OFF_NO_IRQ	0b00000001
#define CONTROL_BYTE_POWER_DOWN_MODE_REF_OFF_ADC_ON_IRQ		0b00000010
#define CONTROL_BYTE_POWER_DOWN_MODE_FULL_POWER_NO_IRQ		0b00000011

typedef Uint8 controlByte_t;

#define XPT2046_CS_SET()			GPIO_SetBits(LCD_CS_TOUCH)
#define XPT2046_CS_RESET()			GPIO_ResetBits(LCD_CS_TOUCH)

static Uint16 XPT2046_getReading(controlByte_t controlByte);
static void XPT2046_convertCoordinateScreenMode(Sint16 * pX, Sint16 * pY);

void XPT2046_init(void){

	// Initialise SPI
	SPI_init();

	XPT2046_CS_SET();

	XPT2046_getReading(CONTROL_BYTE_START
					   | CONTROL_BTYE_CHANNEL_SELECT_X
					   | CONTROL_BYTE_MODE_12_BIT
					   | CONTROL_BYTE_SD_DIFFERENTIAL
					   | CONTROL_BYTE_POWER_DOWN_MODE_LOW_POWER_IRQ);
}

void XPT2046_setConfig(void){
	SPI_setBaudRate(SPI2, SPI_BaudRatePrescaler_256);
}


bool_e XPT2046_getCoordinates(Sint16 * pX, Sint16 * pY, XPT2046_coordinateMode_e coordinateMode){
	Uint8 i, j;
	Sint16 allX[7] , allY[7];

	for (i=0; i < 7 ; i++){

		allY[i] = XPT2046_getReading(CONTROL_BYTE_START
										| CONTROL_BTYE_CHANNEL_SELECT_Y
										| CONTROL_BYTE_MODE_12_BIT
										| CONTROL_BYTE_SD_DIFFERENTIAL
										| CONTROL_BYTE_POWER_DOWN_MODE_LOW_POWER_IRQ);

		allX[i] = XPT2046_getReading(CONTROL_BYTE_START
										| CONTROL_BTYE_CHANNEL_SELECT_X
										| CONTROL_BYTE_MODE_12_BIT
										| CONTROL_BYTE_SD_DIFFERENTIAL
										| CONTROL_BYTE_POWER_DOWN_MODE_LOW_POWER_IRQ);
	}

	for (i=0; i < 4 ; i++){
		for (j=i; j < 7 ; j++) {
			Sint16 temp = allX[i];
			if(temp > allX[j]){
				allX[i] = allX[j];
				allX[j] = temp;
			}
			temp = allY[i];
			if(temp > allY[j]){
				allY[i] = allY[j];
				allY[j] = temp;
			}
		}
	}


	if(coordinateMode == XPT2046_COORDINATE_SCREEN_RELATIVE)
		XPT2046_convertCoordinateScreenMode(&(allX[3]), &(allY[3]));

	*pX = allX[3];
	*pY = allY[3];

#ifdef USE_IRQ_TOUCH_VALIDATION
	if(allX[3] != 0 && allY[3] != 4095 && !IRQ_TOUCH)
		return TRUE;
	else
		return FALSE;
#else
	if(allX[3] > 0 && allY[3] < 4095)
		return TRUE;
	else
		return FALSE;
#endif
}

bool_e XPT2046_getAverageCoordinates(Sint16 * pX, Sint16 * pY, Uint8 nSamples, XPT2046_coordinateMode_e coordinateMode){
	Uint8 nRead = 0;
	Sint32 xAcc = 0 , yAcc = 0;
	Sint16 x , y;

	while(nRead < nSamples){
		if(!XPT2046_getCoordinates(&x , &y, coordinateMode)){
			break;
		}
		xAcc += x;
		yAcc += y;
		nRead ++;
	}

	if(nRead == 0){
		return FALSE;
	}
	*pX = xAcc / nRead;
	*pY = yAcc / nRead;
	return TRUE;
}

static Uint16 XPT2046_getReading(controlByte_t controlByte){

	Uint16 ret;

	XPT2046_CS_RESET();

	SPI_write(SPI2, controlByte);
	ret = SPI_read(SPI2);
	ret = (ret << 5) | (SPI_read(SPI2) >> 3);

	XPT2046_CS_SET();

	return ret;
}

static void XPT2046_convertCoordinateScreenMode(Sint16 * pX, Sint16 * pY){
	ILI931_Options_t screenOption = ILI9341_getOptions();
	Sint16 tempX = *pX, tempY = *pY;

	switch(screenOption.orientation){
	case ILI9341_Orientation_Portrait_1 :
		*pX = tempX * screenOption.width / 4096;
		*pY = tempY * screenOption.height / 4096;
		break;

	case ILI9341_Orientation_Portrait_2 :
		*pX = (4096 - tempX) * screenOption.width / 4096;
		*pY = (4096 - tempY) * screenOption.height / 4096;
		break;

	case ILI9341_Orientation_Landscape_2 :
		*pX = tempY * screenOption.width / 4096;
		*pY = tempX * screenOption.height / 4096;
		break;

	case ILI9341_Orientation_Landscape_1 :
		*pX = (4096 - tempY) * screenOption.width / 4096;
		*pY = (4096 - tempX) * screenOption.height / 4096;
		break;
	}
}
