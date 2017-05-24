#include "vl53l0x_platform.h"

#ifdef USE_VL53L0X

	#include "QS_vl53l0x_api.h"
	#include "../../QS_i2c.h"

	#include <string.h>

	#define I2C_TIME_OUT_BASE   10
	#define I2C_TIME_OUT_BYTE   1

	/* when not customized by application define dummy one */
	#ifndef VL53L0X_GetI2cBus
	/** This macro can be overloaded by user to enforce i2c sharing in RTOS context
	 */
	#   define VL53L0X_GetI2cBus(...) (void)0
	#endif

	#ifndef VL53L0X_PutI2cBus
	/** This macro can be overloaded by user to enforce i2c sharing in RTOS context
	 */
	#   define VL53L0X_PutI2cBus(...) (void)0
	#endif

	// the ranging_sensor_comms.dll will take care of the page selection
	VL53L0X_Error VL53L0X_WriteMulti(VL53L0X_DEV Dev, uint8_t index, uint8_t *pdata, uint32_t count) {
		bool_e memoryOk;

		memoryOk = I2C_Write(Dev->I2cHandle, Dev->I2cDevAddr, &index, 1, pdata, count);

		time32_t beginTimeBoot = global.absolute_time;
		while(global.absolute_time - beginTimeBoot < 2);

		if(memoryOk)
			return VL53L0X_ERROR_NONE;
		else
			return VL53L0X_ERROR_UNDEFINED;
	}

	// the ranging_sensor_comms.dll will take care of the page selection
	VL53L0X_Error VL53L0X_ReadMulti(VL53L0X_DEV Dev, uint8_t index, uint8_t *pdata, uint32_t count) {
		bool_e memoryOk;

		memoryOk = I2C_Read(Dev->I2cHandle, Dev->I2cDevAddr, &index, 1, pdata, count);

		time32_t beginTimeBoot = global.absolute_time;
		while(global.absolute_time - beginTimeBoot < 2);

		if(memoryOk)
			return VL53L0X_ERROR_NONE;
		else
			return VL53L0X_ERROR_UNDEFINED;
	}

	VL53L0X_Error VL53L0X_WrByte(VL53L0X_DEV Dev, uint8_t index, uint8_t data) {
		bool_e memoryOk;

		memoryOk = I2C_Write(Dev->I2cHandle, Dev->I2cDevAddr, &index, 1, &data, 1);

		time32_t beginTimeBoot = global.absolute_time;
		while(global.absolute_time - beginTimeBoot < 2);

		if(memoryOk)
			return VL53L0X_ERROR_NONE;
		else
			return VL53L0X_ERROR_UNDEFINED;
	}

	VL53L0X_Error VL53L0X_WrWord(VL53L0X_DEV Dev, uint8_t index, uint16_t data) {
		Uint8 datas[2] = {((data >> 8) & 0xFF), ((data) & 0xFF)};
		bool_e memoryOk;

		memoryOk = I2C_Write(Dev->I2cHandle, Dev->I2cDevAddr, &index, 1, datas, 2);

		time32_t beginTimeBoot = global.absolute_time;
		while(global.absolute_time - beginTimeBoot < 2);

		if(memoryOk)
			return VL53L0X_ERROR_NONE;
		else
			return VL53L0X_ERROR_UNDEFINED;
	}

	VL53L0X_Error VL53L0X_WrDWord(VL53L0X_DEV Dev, uint8_t index, uint32_t data) {
		Uint8 datas[4] = {((data >> 24) & 0xFF), ((data >> 16) & 0xFF), ((data >> 8) & 0xFF), ((data) & 0xFF)};
		bool_e memoryOk;

		memoryOk = I2C_Write(Dev->I2cHandle, Dev->I2cDevAddr, &index, 1, datas, 4);

		time32_t beginTimeBoot = global.absolute_time;
		while(global.absolute_time - beginTimeBoot < 2);

		if(memoryOk)
			return VL53L0X_ERROR_NONE;
		else
			return VL53L0X_ERROR_UNDEFINED;
	}

	VL53L0X_Error VL53L0X_UpdateByte(VL53L0X_DEV Dev, uint8_t index, uint8_t AndData, uint8_t OrData) {
		VL53L0X_Error Status = VL53L0X_ERROR_NONE;
		uint8_t data;

		Status = VL53L0X_RdByte(Dev, index, &data);
		if (Status) {
			goto done;
		}
		data = (data & AndData) | OrData;
		Status = VL53L0X_WrByte(Dev, index, data);
	done:
		return Status;
	}

	VL53L0X_Error VL53L0X_RdByte(VL53L0X_DEV Dev, uint8_t index, uint8_t *data) {
		bool_e memoryOk;
		memoryOk = I2C_Read(Dev->I2cHandle, Dev->I2cDevAddr, &index, 1, data, 1);

		time32_t beginTimeBoot = global.absolute_time;
		while(global.absolute_time - beginTimeBoot < 2);

		if(memoryOk)
			return VL53L0X_ERROR_NONE;
		else
			return VL53L0X_ERROR_UNDEFINED;
	}

	VL53L0X_Error VL53L0X_RdWord(VL53L0X_DEV Dev, uint8_t index, uint16_t *data) {
		Uint8 datas[2];
		bool_e memoryOk;
		memoryOk = I2C_Read(Dev->I2cHandle, Dev->I2cDevAddr, &index, 1, datas, 2);

		time32_t beginTimeBoot = global.absolute_time;
		while(global.absolute_time - beginTimeBoot < 2);

		if(data)
			*data = ((((Uint16)datas[0]) << 8) | datas[1]);

		if(memoryOk)
			return VL53L0X_ERROR_NONE;
		else
			return VL53L0X_ERROR_UNDEFINED;
	}

	VL53L0X_Error VL53L0X_RdDWord(VL53L0X_DEV Dev, uint8_t index, uint32_t *data) {
		Uint8 datas[4];
		bool_e memoryOk;
		memoryOk = I2C_Read(Dev->I2cHandle, Dev->I2cDevAddr, &index, 1, datas, 4);

		time32_t beginTimeBoot = global.absolute_time;
		while(global.absolute_time - beginTimeBoot < 2);

		if(data)
			*data = ((((Uint32)datas[0]) << 24) | (((Uint32)datas[1]) << 16) | (((Uint32)datas[2]) << 8) | datas[3]);

		if(memoryOk)
			return VL53L0X_ERROR_NONE;
		else
			return VL53L0X_ERROR_UNDEFINED;
	}

	VL53L0X_Error VL53L0X_PollingDelay(VL53L0X_DEV Dev) {
		VL53L0X_Error status = VL53L0X_ERROR_NONE;

		return status;
	}

	//end of file

#endif
