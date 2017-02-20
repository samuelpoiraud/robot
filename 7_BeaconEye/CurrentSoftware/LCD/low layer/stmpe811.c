/*
 * stmpe811.c
 *
 *  Created on: 20 févr. 2017
 *      Author: guill
 */

#include "stmpe811.h"
#include "../../QS/QS_i2c.h"
#include "ssd2119.h"


#ifdef USE_BEACON_EYE
	#define I2C_TOUCH			   		I2C2
#else
	#define I2C_TOUCH			   		I2C1
#endif

#define STMPE811_ADDRESS   			    0x82
#define STMPE811_CHIP_ID_VALUE			0x0811

//------------------- Address Register -------------------

#define STMPE811_CHIP_ID				0x00	//STMPE811 Device identification
#define STMPE811_ID_VER					0x02	//STMPE811 Revision number; 0x01 for engineering sample; 0x03 for final silicon
#define STMPE811_SYS_CTRL1				0x03	//Reset control
#define STMPE811_SYS_CTRL2				0x04	//Clock control
#define STMPE811_SPI_CFG				0x08	//SPI interface configuration
#define STMPE811_INT_CTRL				0x09	//Interrupt control register
#define STMPE811_INT_EN					0x0A	//Interrupt enable register
#define STMPE811_INT_STA				0x0B	//Interrupt status register
#define STMPE811_GPIO_EN				0x0C	//GPIO interrupt enable register
#define STMPE811_GPIO_INT_STA			0x0D	//GPIO interrupt status register
#define STMPE811_ADC_INT_EN				0x0E	//ADC interrupt enable register
#define STMPE811_ADC_INT_STA			0x0F	//ADC interface status register
#define STMPE811_GPIO_SET_PIN			0x10	//GPIO set pin register
#define STMPE811_GPIO_CLR_PIN			0x11	//GPIO clear pin register
#define STMPE811_MP_STA					0x12	//GPIO monitor pin state register
#define STMPE811_GPIO_DIR				0x13	//GPIO direction register
#define STMPE811_GPIO_ED				0x14	//GPIO edge detect register
#define STMPE811_GPIO_RE				0x15	//GPIO rising edge register
#define STMPE811_GPIO_FE				0x16	//GPIO falling edge register
#define STMPE811_GPIO_AF				0x17	//alternate function register
#define STMPE811_ADC_CTRL1				0x20	//ADC control
#define STMPE811_ADC_CTRL2				0x21	//ADC control
#define STMPE811_ADC_CAPT				0x22	//To initiate ADC data acquisition
#define STMPE811_ADC_DATA_CHO			0x30	//ADC channel 0
#define STMPE811_ADC_DATA_CH1			0x32	//ADC channel 1
#define STMPE811_ADC_DATA_CH2			0x34	//ADC channel 2
#define STMPE811_ADC_DATA_CH3			0x36	//ADC channel 3
#define STMPE811_ADC_DATA_CH4			0x38	//ADC channel 4
#define STMPE811_ADC_DATA_CH5			0x3A	//ADC channel 5
#define STMPE811_ADC_DATA_CH6			0x3C	//ADC channel 6
#define STMPE811_ADC_DATA_CH7			0x3E	//ADC channel 7
#define STMPE811_TSC_CTRL				0x40	//4-wire touchscreen controller setup
#define STMPE811_TSC_CFG				0x41	//Touchscreen controller configuration
#define STMPE811_WDW_TR_X				0x42	//Window setup for top right X
#define STMPE811_WDW_TR_Y				0x44	//Window setup for top right Y
#define STMPE811_WDW_BL_X				0x46	//Window setup for bottom left X
#define STMPE811_WDW_BL_Y				0x48	//Window setup for bottom left Y
#define STMPE811_FIFO_TH				0x4A	//FIFO level to generate interrupt
#define STMPE811_FIFO_STA				0x4B	//Current status of FIFO
#define STMPE811_FIFO_SIZE				0x4C	//Current filled level of FIFO
#define STMPE811_TSC_DATA_X				0x4D	//Data port for touchscreen controller data access
#define STMPE811_TSC_DATA_Y				0x4F	//Data port for touchscreen controller data access
#define STMPE811_TSC_DATA_Z				0x51	//Data port for touchscreen controller data access
#define STMPE811_TSC_DATA_XYZ			0x52	//Data port for touchscreen controller data access
#define STMPE811_TSC_FRACTION_Z			0x56	//Touchscreen controller FRACTION_Z
#define STMPE811_TSC_DATA				0x57	//Data port for touchscreen controller data access
#define STMPE811_TSC_I_DRIVE			0x58	//Touchscreen controller drivel
#define STMPE811_TSC_SHIELD				0x59	//Touchscreen controller shield
#define STMPE811_TEMP_CTRL				0x60	//Temperature sensor setup
#define STMPE811_TEMP_DATA				0x61	//Temperature data access port
#define STMPE811_TEMP_TH				0x62	//Threshold for temperature controlled interrupt

//--------------------------------------------------------

static Uint16 STMPE811_readData16(Uint8 reg);
static Uint8 STMPE811_readData8(Uint8 reg);
static Uint16 STMPE811_readX(Uint8 nbSamples);
static Uint16 STMPE811_readY(Uint8 nbSamples);
static void STMPE811_writeData(Uint8 reg, Uint8 data);
static void Delayms(Uint8 value);
static void STMPE811_convertCoordinateScreenMode(Sint16 * pX, Sint16 * pY);

void STMPE811_init(void) {
	I2C_init();

	Uint8 mode;

	/* Reset */
	STMPE811_writeData(STMPE811_SYS_CTRL1, 0x02);
	Delayms(5);
	STMPE811_writeData(STMPE811_SYS_CTRL1, 0x00);
	Delayms(2);

	/* Check for STMPE811 Connected */
	if (STMPE811_readData16(STMPE811_CHIP_ID) != STMPE811_CHIP_ID_VALUE) {
		return;
	}

	/* Reset */
	STMPE811_writeData(STMPE811_SYS_CTRL1, 0x02);
	Delayms(5);
	STMPE811_writeData(STMPE811_SYS_CTRL1, 0x00);
	Delayms(2);

	/* Get the current register value */
	mode = STMPE811_readData8(STMPE811_SYS_CTRL2);
	mode &= ~(0x01);
	STMPE811_writeData(STMPE811_SYS_CTRL2, mode);
	mode = STMPE811_readData8(STMPE811_SYS_CTRL2);
	mode &= ~(0x02);
	STMPE811_writeData(STMPE811_SYS_CTRL2, mode);

	/* Select Sample Time, bit number and ADC Reference */
	STMPE811_writeData(STMPE811_ADC_CTRL1, 0x49);

	/* Wait for 2 ms */
	Delayms(2);

	/* Select the ADC clock speed: 3.25 MHz */
	STMPE811_writeData(STMPE811_ADC_CTRL2, 0x01);

	/* Select TSC pins in non default mode */
	mode = STMPE811_readData8(STMPE811_GPIO_AF);
	mode |= 0x1E;
	STMPE811_writeData(STMPE811_GPIO_AF, mode);

	/* Select 2 nF filter capacitor */
	/* Configuration:
	- Touch average control    : 4 samples
	- Touch delay time         : 500 uS
	- Panel driver setting time: 500 uS
	*/
	STMPE811_writeData(STMPE811_TSC_CFG, 0x9A);

	/* Configure the Touch FIFO threshold: single point reading */
	STMPE811_writeData(STMPE811_FIFO_TH, 0x01);

	/* Clear the FIFO memory content. */
	STMPE811_writeData(STMPE811_FIFO_STA, 0x01);

	/* Put the FIFO back into operation mode  */
	STMPE811_writeData(STMPE811_FIFO_STA, 0x00);

	/* Set the range and accuracy pf the pressure measurement (Z) :
	- Fractional part :7
	- Whole part      :1
	*/
	STMPE811_writeData(STMPE811_TSC_FRACTION_Z, 0x01);

	/* Set the driving capability (limit) of the device for TSC pins: 50mA */
	STMPE811_writeData(STMPE811_TSC_I_DRIVE, 0x01);

	/* Touch screen control configuration (enable TSC):
	- No window tracking index
	- XYZ acquisition mode
	*/
	STMPE811_writeData(STMPE811_TSC_CTRL, 0x03);

	/* Clear all the status pending bits if any */
	STMPE811_writeData(STMPE811_INT_STA, 0xFF);

	/* Enable touch interrupt */
#ifdef USE_IRQ_TOUCH_VALIDATION
	mode = STMPE811_readData8(STMPE811_INT_EN);
	mode |= 0x01;
	STMPE811_writeData(STMPE811_INT_EN, mode);
#endif

	/* Wait for 2 ms delay */
	Delayms(2);

}

bool_e STMPE811_getCoordinates(Sint16 * pX, Sint16 * pY, STMPE811_coordinateMode_e coordinateMode) {

	if((STMPE811_readData8(STMPE811_TSC_CTRL) & 0x80) == 0) {
		//Reset Fifo
		STMPE811_writeData(STMPE811_FIFO_STA, 0x01);
		STMPE811_writeData(STMPE811_FIFO_STA, 0x00);
	}

	*pX = STMPE811_readX(1);
	*pY = STMPE811_readY(1);

	//Reset Fifo
	STMPE811_writeData(STMPE811_FIFO_STA, 0x01);
	STMPE811_writeData(STMPE811_FIFO_STA, 0x00);

	if(coordinateMode == STMPE811_COORDINATE_SCREEN_RELATIVE)
		STMPE811_convertCoordinateScreenMode(pX, pY);

	return TRUE;

#ifdef USE_IRQ_TOUCH_VALIDATION
	if(*pX != 0 && *pY != 4095 && !IRQ_TOUCH)
		return TRUE;
	else
		return FALSE;
#else
	if(*pX > 0 && *pY < 4095)
		return TRUE;
	else
		return FALSE;
#endif

}

bool_e STMPE811_getAverageCoordinates(Sint16 * pX, Sint16 * pY, Uint8 nSamples, STMPE811_coordinateMode_e coordinateMode) {
	Uint8 nRead = 0;
	Sint32 xAcc = 0 , yAcc = 0;
	Sint16 x , y;

	while(nRead < nSamples){
		if(!STMPE811_getCoordinates(&x , &y, coordinateMode)){
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

bool_e STMPE811_isConnected(void) {
	return I2C_IsDeviceConnected(I2C_TOUCH, STMPE811_ADDRESS);
}

static Uint16 STMPE811_readX(Uint8 nbSamples) {
	Uint8 i = 0;
	Uint32 sum = 0;

	if(nbSamples > 0) {
		for(i = 0; i < nbSamples; i++) {
			sum += STMPE811_readData16(STMPE811_TSC_DATA_X);
		}

		sum = sum / nbSamples;

		if(sum < 0) {
			sum = 0;
		}
	}

	return sum;
}

static Uint16 STMPE811_readY(Uint8 nbSamples) {
	Uint8 i = 0;
	Uint32 sum = 0;

	if(nbSamples > 0) {
		for(i = 0; i < nbSamples; i++) {
			sum += STMPE811_readData16(STMPE811_TSC_DATA_Y);
		}

		sum = sum / nbSamples;

		if(sum < 0) {
			sum = 0;
		}
	}

	return sum;
}

static Uint16 STMPE811_readData16(Uint8 reg) {
	Uint8 data[2];
	Uint8 tmp= 0;

	I2C_Read(I2C_TOUCH, STMPE811_ADDRESS , &reg, 1, data, 2);

	tmp = data[0];
	data[0] = data[1];
	data[1] = tmp;

	return (*(volatile Uint16*)data);
}

static Uint8 STMPE811_readData8(Uint8 reg) {
	Uint8 data = 0;
	I2C_Read(I2C_TOUCH, STMPE811_ADDRESS , &reg, 1, &data, 1);
	return data;
}

static void STMPE811_writeData(Uint8 reg, Uint8 data) {
	I2C_Write(I2C_TOUCH, STMPE811_ADDRESS , &reg, 1, &data, 1);
}

static void Delayms(Uint8 msec) {
	time32_t endTime = global.absolute_time + msec;
	while(global.absolute_time < endTime);
}

static void STMPE811_convertCoordinateScreenMode(Sint16 * pX, Sint16 * pY) {
	SSD2119_Options_t screenOption = SSD2119_getOptions();
	Sint16 tempX = *pX, tempY = *pY;

	switch(screenOption.orientation){
	case SSD2119_Orientation_Portrait_1 :
		*pX = tempY * screenOption.width / 4096;
		*pY = tempX * screenOption.height / 4096;
		break;

	case SSD2119_Orientation_Portrait_2 :
		*pX = (4096 - tempY) * screenOption.width / 4096;
		*pY = (4096 - tempX) * screenOption.height / 4096;
		break;

	case SSD2119_Orientation_Landscape_1 :
		*pX = tempX * screenOption.width / 4096;
		*pY = (4096 - tempY) * screenOption.height / 4096;
		break;

	case SSD2119_Orientation_Landscape_2 :
		*pX = (4096 - tempX) * screenOption.width / 4096;
		*pY = tempY * screenOption.height / 4096;
		break;
	}
}
