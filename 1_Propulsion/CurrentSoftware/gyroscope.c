/*
 * gyroscope_functions.c
 *
 *  Created on: 12 juin. 2014
 *	  Author: Anthony
 */

#include "gyroscope.h"
#include "QS/QS_spi.h"
#include "QS/QS_outputlog.h"

#ifdef USE_GYROSCOPE

#define RATE1 0x00	// Registre Name pour recup les valeurs MSB
#define RATE0 0x01	// Registre Name pour recup les valeurs LSB

#define ADXRS453_SLAVE_ID	   1

#define ADXRS453_READ		   (1 << 7)
#define ADXRS453_WRITE		  (1 << 6)
#define ADXRS453_SENSOR_DATA	(1 << 5)

#define ADXRS453_REG_RATE	   0x00
#define ADXRS453_REG_TEM		0x02
#define ADXRS453_REG_LOCST	  0x04
#define ADXRS453_REG_HICST	  0x06
#define ADXRS453_REG_QUAD	   0x08
#define ADXRS453_REG_FAULT	  0x0A
#define ADXRS453_REG_PID		0x0C
#define ADXRS453_REG_SN_HIGH	0x0E
#define ADXRS453_REG_SN_LOW	 0x10

static volatile bool_e initialized = FALSE;
// Gyro : ADXRS453
// Datasheet : SVN/propulsion/documentation
// Code inspiré de https://github.com/analogdevicesinc/no-OS/blob/master/drivers/ADXRS453

void GYRO_write(Uint8 *Data, Uint8 size);
void GYRO_read(Uint8 * Data, Uint8 size);
Uint16 ADXRS453_GetRegisterValue(Uint16 registerAddress);
void ADXRS453_SetRegisterValue(Uint16 registerAddress, Uint16 registerValue);

static void delay_50ns(void)
{
	volatile Uint8 delay;
	//Wait >50ns
	delay = 100;
	while(delay--);
}


void GYRO_test(){
	/*static bool_e init = FALSE;
	bool_e valid;
	if(!init)
	{
		ADXRS453_GetRegisterValue(ADXRS453_REG_RATE);
		ADXRS453_GetRegisterValue(ADXRS453_REG_TEM);
		ADXRS453_GetRegisterValue(ADXRS453_REG_LOCST);
		ADXRS453_GetRegisterValue(ADXRS453_REG_HICST);
		ADXRS453_GetRegisterValue(ADXRS453_REG_QUAD);
		ADXRS453_GetRegisterValue(ADXRS453_REG_FAULT);
		ADXRS453_GetRegisterValue(ADXRS453_REG_PID);
		ADXRS453_GetRegisterValue(ADXRS453_REG_SN_HIGH);
		ADXRS453_GetRegisterValue(ADXRS453_REG_SN_LOW);
		init = TRUE;
	}*/
	//GYRO_GetSensorData(TRUE, &valid);

	//debug_printf("Gyro temperature is %d\n", ADXRS453_GetTemperature());
}


Sint32 GYRO_get_speed_rotation(bool_e * valid)
{
	Sint16 speed;
	if(initialized)
	{
		speed = GYRO_GetSensorData(FALSE,valid);	//[°/sec/80]
		return ((Sint32)(speed) * 4685) >> 10;		//[rad/4096/1024/5ms].....
	}
	else
	{
		*valid = FALSE;
		return (Sint32)0;
	}
}


void GYRO_write(Uint8 *Data, Uint8 size){
	Uint8 i;
	delay_50ns();
	GYRO_CS = 0;							 // Select Device
	delay_50ns();

	for (i = 0; i < size; ++i)
		SPI2_write(Data[i]);

	delay_50ns();
	GYRO_CS = 1;							 // Deselect Device

}


void GYRO_read(Uint8 * Data, Uint8 size)
{
	Uint16 i;
	delay_50ns();
	GYRO_CS = 0;							 // Select Device
	delay_50ns();

	for(i=0;i<size;i++)
		Data[i]=SPI2_read();

	delay_50ns();
	GYRO_CS = 1;							 // Deselect Device
}



/***************************************************************************//**
 * @brief Initializes the ADXRS453 and checks if the device is present.
 *
 * @return status - Result of the initialization procedure.
 *				  Example:  0 - if initialization was successful (ID starts
 *								with 0x52).
 *						   -1 - if initialization was unsuccessful.
*******************************************************************************/
void GYRO_init(void)
{
	Uint16 adxrs453Id = 0;
	SPI_init();

	/* Read the value of the ADXRS453 ID register. */
	adxrs453Id = ADXRS453_GetRegisterValue(ADXRS453_REG_PID);
	if((adxrs453Id >> 8) != 0x52)
		initialized = FALSE;
	else
		initialized = TRUE;
	debug_printf("Gyro ADXRS453 : init %s\n", (initialized)?"OK":"FAILED !");
}
/***************************************************************************//**
 * @brief Reads the value of a register.
 *
 * @param registerAddress - Address of the register.
 *
 * @return registerValue - Value of the register.
*******************************************************************************/
Uint16 ADXRS453_GetRegisterValue(Uint16 registerAddress)
{
	Uint8  dataBuffer[4] = {0, 0, 0, 0};
	Uint32  command = 0;
	Uint8  bit = 0;
	Uint8  sum = 0;
	Uint16 registerValue = 0;

	dataBuffer[0] = ADXRS453_READ | ((registerAddress & 0x01FF) >> 7);
	dataBuffer[1] = registerAddress << 1;
	command = U32FROMU8(dataBuffer[0], dataBuffer[1], dataBuffer[2], dataBuffer[3]);

	// Parity
	for(bit = 31; bit > 0; bit--)
		sum += ((command >> bit) & 0x1);
	if(!(sum % 2))
		dataBuffer[3] |= 1;

	GYRO_write(dataBuffer, 4);
	GYRO_read(dataBuffer, 4);

	registerValue = ((Uint32)(dataBuffer[1]&0x1F) << 11) |
			((Uint32)dataBuffer[2] << 3) |
			(dataBuffer[3] >> 5);

	if(!(dataBuffer[0] & (1<<6)))
	{
		debug_printf("ERROR when reading : %8lx\n", U32FROMU8(dataBuffer[0], dataBuffer[1], dataBuffer[2], dataBuffer[3]));
		debug_printf("%8lx\t->0x%4x = %d\n", command, registerValue, (Uint16)registerValue);
	}

	return registerValue;
}


/***************************************************************************//**
 * @brief Writes data into a register.
 *
 * @param registerAddress - Address of the register.
 * @param registerValue - Data value to write.
 *
 * @return None.
*******************************************************************************/
void ADXRS453_SetRegisterValue(Uint16 registerAddress, Uint16 registerValue)
{
	unsigned char dataBuffer[4] = {0, 0, 0, 0};
	unsigned long command	   = 0;
	unsigned char bitNo		 = 0;
	unsigned char sum		   = 0;

	dataBuffer[0] = ADXRS453_READ | ((registerAddress & 0x01FF) >> 7);
	dataBuffer[1] = (registerAddress << 1) |
					(registerValue >> 15);
	dataBuffer[2] = (registerValue >> 7);
	dataBuffer[3] = (registerValue << 1);

	command = U32FROMU8(dataBuffer[0], dataBuffer[1], dataBuffer[2], dataBuffer[3]);

	for(bitNo = 31; bitNo > 0; bitNo--)
		sum += ((command >> bitNo) & 0x1);
	if(!(sum % 2))
		dataBuffer[3] |= 1;

	GYRO_write(dataBuffer, 4);
	GYRO_read(dataBuffer, 4);
	if(!(dataBuffer[0] & (1<<5)))
		debug_printf("ERROR when writting : %8lx\n", U32FROMU8(dataBuffer[0], dataBuffer[1], dataBuffer[2], dataBuffer[3]));
}







/***************************************************************************//**
 * @brief Reads the sensor data.
 *
 * @param None.
 *
 * @return registerValue - The sensor data.
*******************************************************************************/
Sint16 GYRO_GetSensorData(bool_e verbose, bool_e * valid)
{
	Uint8 dataBuffer[4] = {0, 0, 0, 0};
	Uint32 command = 0;
	Uint8 bit = 0;
	Uint8 sum = 0;
	Uint32 registerValue = 0;
	Sint16 sensorData = 0;
	Uint8 error;
	char error_string[200];
	Uint8 index;

	dataBuffer[0] = ADXRS453_SENSOR_DATA;
	command = U32FROMU8(dataBuffer[0], dataBuffer[1], dataBuffer[2], dataBuffer[3]);

	for(bit = 31; bit > 0; bit--)
		sum += ((command >> bit) & 0x1);
	if(!(sum % 2))
		dataBuffer[3] |= 1;

	GYRO_write(dataBuffer,4);
	GYRO_read(dataBuffer,4);

	registerValue = U32FROMU8(dataBuffer[0], dataBuffer[1], dataBuffer[2], dataBuffer[3]);
	sensorData = (Sint16)(registerValue >> 10);
	error = registerValue & 0xFE;
	*valid = (error)?FALSE:TRUE;
	if(verbose)
	{
		if(!error)
			error_string[0] = '\0';
		else
		{
			index = sprintf(error_string, " ERROR : ");
			if(error & 0x80)	index += sprintf(error_string+index, "PLL failed | ");
			if(error & 0x40)	index += sprintf(error_string+index, "Quadrature | ");
			if(error & 0x20)	index += sprintf(error_string+index, "NVM memory | ");
			if(error & 0x10)	index += sprintf(error_string+index, "POR | ");
			if(error & 0x08)	index += sprintf(error_string+index, "Power bad | ");
			if(error & 0x04)	index += sprintf(error_string+index, "CST selftest | ");
			if(error & 0x02)	index += sprintf(error_string+index, "CHK bit | ");
			sprintf(error_string+index, "\n");
		}
		debug_printf("%8lx\t->%8lx => Data : %d%s\n", command, registerValue, sensorData, error_string);
	}
	return sensorData;
}



/***************************************************************************//**
 * @brief Reads the rate data and converts it to degrees/second.
 *
 * @param None.
 *
 * @return rate - The rate value in degrees/second.
*******************************************************************************/
float ADXRS453_GetRate(void)
{
	unsigned short registerValue = 0;
	float		  rate		  = 0.0;

	registerValue = ADXRS453_GetRegisterValue(ADXRS453_REG_RATE);

	/*!< If data received is in positive degree range */
	if(registerValue < 0x8000)
	{
		rate = ((float)registerValue / 80);
	}
	/*!< If data received is in negative degree range */
	else
	{
		rate = (-1) * ((float)(0xFFFF - registerValue + 1) / 80.0);
	}

	return rate;
}

/***************************************************************************//**
 * @brief Reads the temperature sensor data and converts it to degrees Celsius.
 *
 * @param None.
 *
 * @return temperature - The temperature value in 0.1 degrees Celsius.
*******************************************************************************/
Sint16 ADXRS453_GetTemperature(void)
{
	Uint16 registerValue;
	Sint32 temperature;

	registerValue = ADXRS453_GetRegisterValue(ADXRS453_REG_TEM);
	temperature = (Sint16)(registerValue >> 5) - 1598;

	return temperature;	//[0.1°C]
}

#endif
