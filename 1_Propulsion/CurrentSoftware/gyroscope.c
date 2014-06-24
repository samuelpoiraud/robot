/*
 * gyroscope_functions.c
 *
 *  Created on: 12 juin. 2014
 *      Author: Anthony
 */

#include "gyroscope.h"
#include "_Propulsion_config.h"
#include "QS/QS_spi.h"
#include "Global_config.h"
#include "QS/QS_outputlog.h"

#ifdef USE_GYROSCOPE

#define RATE1 0x00	// Registre Name pour recup les valeurs MSB
#define RATE0 0x01	// Registre Name pour recup les valeurs LSB



// Code inspiré de https://github.com/analogdevicesinc/no-OS/blob/master/drivers/ADXRS453

void GYRO_write(Uint8 *Data, Uint8 size);
void GYRO_read(Uint8 * Data, Uint8 size);

void delay_50ns(void)
{
	static volatile Uint32 delay;
	//Wait >50ns
	delay = 100;
	while(delay--);
}


void GYRO_test(){
	Uint8 Data[4];


	//GYRO_GetSensorData();
	GYRO_GetRegisterValue(RATE0);
	//debug_printf("donnée      %x\n",ADXRS453_GetSensorData());
	//debug_printf("donnée      %x\t\t%x\t\t%x\t\t%x\t\t\n",Data[3],Data[2],Data[1],Data[0]);

}


void GYRO_init(void)
{
	SPI_init();

	Uint8  dataBuffer[4] = {0x20, 0x00, 0x00, 0x03};

	GYRO_write(dataBuffer,4);
	GYRO_read(dataBuffer,4);

	debug_printf("init      %x\t\t%x\t\t%x\t\t%x\t\t\n",dataBuffer[0],dataBuffer[1],dataBuffer[2],dataBuffer[3]);

	debug_printf("GYROSCOPE INIT\n");
}


void GYRO_write(Uint8 *Data, Uint8 size){
	Uint8 i;

	GYRO_CS = 0;                             // Select Device
	delay_50ns();

	for (i = 0; i < size; ++i)
		SPI2_write(Data[i]);

	delay_50ns();
	GYRO_CS = 1;                             // Deselect Device
}


void GYRO_read(Uint8 * Data, Uint8 size)
{
	Uint16 i;

	GYRO_CS = 0;                             // Select Device
	delay_50ns();

	for(i=0;i<size;i++)
		Data[i]=SPI2_read();

	delay_50ns();
	GYRO_CS = 1;                             // Deselect Device
}

Uint16 GYRO_GetRegisterValue(Uint8 registerAddress)
{
	Uint8  dataBuffer[4] = {0, 0, 0, 0};
	Uint32  command = 0;
	Uint8  bit = 0;
	Uint8  sum = 0;
	Uint16 registerValue = 0;

	//dataBuffer[0] = 0x00;
	dataBuffer[1] = registerAddress;
	command = ((Uint32)dataBuffer[0] << 24) |
			((Uint32)dataBuffer[1] << 16) |
			((Uint16)dataBuffer[2] << 8) |
			dataBuffer[3];


	// Parité
	for(bit = 31; bit > 0; bit--)
	{
		sum += ((command >> bit) & 0x1);
	}
	if(!(sum % 2))
	{
		dataBuffer[3] |= 1;
	}

	debug_printf("donne      %x\t\t%x\t\t%x\t\t%x\t\t\n",dataBuffer[0],dataBuffer[1],dataBuffer[2],dataBuffer[3]);

	GYRO_write(dataBuffer, 4);
	GYRO_read(dataBuffer, 4);

	registerValue = ((Uint16)dataBuffer[1] << 11) |
			((Uint16)dataBuffer[2] << 3) |
			(dataBuffer[3] >> 5);

	debug_printf("registre      %x\t\t%x\t\t%x\t\t%x\t\t\n",dataBuffer[0],dataBuffer[1],dataBuffer[2],dataBuffer[3]);
	debug_printf("registre      %x\n\n",registerValue);

	return registerValue;
}

Uint32 GYRO_GetSensorData(void)
{
	Uint8 dataBuffer[4] = {0x20, 0, 0, 0};
	Uint32 command = 0;
	Uint8 bit = 0;
	Uint8 sum = 0;
	Uint32 registerValue = 0;


	command = ((Uint32)dataBuffer[0] << 24) |
			((Uint32)dataBuffer[1] << 16) |
			((Uint16)dataBuffer[2] << 8) |
			dataBuffer[3];

	for(bit = 31; bit > 0; bit--)
	{
		sum += ((command >> bit) & 0x1);
	}
	if(!(sum % 2))
	{
		dataBuffer[3] |= 1;
	}

	debug_printf("commande      %x\t\t%x\t\t%x\t\t%x\t\t\n",dataBuffer[0],dataBuffer[1],dataBuffer[2],dataBuffer[3]);

	GYRO_write(dataBuffer,4);
	GYRO_read(dataBuffer,4);

	debug_printf("donnée      %x\t\t%x\t\t%x\t\t%x\t\t\n",dataBuffer[0],dataBuffer[1],dataBuffer[2],dataBuffer[3]);

	return registerValue;
}

#endif
