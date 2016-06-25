/*
 *	Club Robot ESEO 2013
 *	???
 *
 *	Fichier : QS_i2c.c
 *	Package : Qualité Soft
 *	Description : Bus I2C
 *	Auteur : Nirgal
 *  Licence : CeCILL-C (voir LICENCE.txt)
 *	Version 201308
 */


#include "QS_i2c.h"
#include "QS_ports.h"
#include "QS_watchdog.h"
#include "QS_outputlog.h"
#include "QS_timer.h"
#include "../stm32f4xx_hal/stm32f4xx_hal_i2c.h"
#include "../stm32f4xx_hal/stm32f4xx_hal_dma.h"

#if defined(USE_I2C1) || defined(USE_I2C2)

static I2C_HandleTypeDef I2C1_HandleStructure, I2C2_HandleStructure;

void I2C_init(void)
{
	#ifdef USE_I2C1
		/* Enable IOE_I2C and IOE_I2C_GPIO_PORT & Alternate Function clocks */
		__HAL_RCC_I2C1_CLK_ENABLE();
		__HAL_RCC_GPIOB_CLK_ENABLE();
		__HAL_RCC_SYSCFG_CLK_ENABLE();
	#endif

	#ifdef USE_I2C2
		/* Enable IOE_I2C and IOE_I2C_GPIO_PORT & Alternate Function clocks */
		__HAL_RCC_I2C2_CLK_ENABLE();
		__HAL_RCC_GPIOB_CLK_ENABLE();
		__HAL_RCC_SYSCFG_CLK_ENABLE();
	#endif

	PORTS_i2c_init();
	WATCHDOG_init();

	I2C1_HandleStructure.Instance = I2C1;
	I2C1_HandleStructure.Init.ClockSpeed = 100000; //100kHz
	I2C1_HandleStructure.Init.DutyCycle = I2C_DUTYCYCLE_2;
	I2C1_HandleStructure.Init.OwnAddress1 = 0;
	I2C1_HandleStructure.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
	I2C1_HandleStructure.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
	I2C1_HandleStructure.Init.OwnAddress2 = 0;
	I2C1_HandleStructure.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
	I2C1_HandleStructure.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;

	I2C2_HandleStructure.Instance = I2C2;
	I2C2_HandleStructure.Init = I2C1_HandleStructure.Init; //On copie tous les paramètres d'initialisation dans l'I2C2

	#ifdef USE_I2C1
		HAL_I2C_DeInit(&I2C1_HandleStructure);
		HAL_I2C_Init(&I2C1_HandleStructure);
		__HAL_I2C_ENABLE_IT(&I2C1_HandleStructure, I2C_IT_ERR);	//Active Interruption sur détection d'erreur de communication.

		//I2C ERROR : activation des IRQ
		HAL_NVIC_SetPriority(&I2C1_ER_IRQn, 7, 0);

		//I2C Event : pas d'activation des IRQ.
		//HAL_NVIC_SetPriority(&I2C1_EV_IRQn, 7, 0);
	#endif /* def USE_I2C1 */

	#ifdef USE_I2C2
		I2C2_HandleStructure.Instance = I2C2;
		HAL_I2C_DeInit(&I2C2_HandleStructure);
		HAL_I2C_Init(&I2C2_HandleStructure);
		__HAL_I2C_ENABLE_IT(&I2C2_HandleStructure, I2C_IT_ERR);	//Active Interruption sur détection d'erreur de communication.

		//I2C ERROR : activation des IRQ
		HAL_NVIC_SetPriority(&I2C2_ER_IRQn, 7, 0);

		//I2C Event : pas d'activation des IRQ.
		//HAL_NVIC_SetPriority(&I2C2_EV_IRQn, 7, 0);
	#endif /* def USE_I2C2 */
}




volatile watchdog_id_t watchdog_id = 0xFF;
volatile bool_e timeout = FALSE;
volatile bool_e i2c_bus_error = FALSE;
volatile Uint32 failed_sr1 = 0, failed_sr2 = 0;

void error_exit(void)
{
	debug_printf("I2C_failure :\n");
	if(timeout)
		debug_printf(" - Timeout\n");
	if(failed_sr2 & I2C_FLAG_DUALF)			debug_printf(" - I2C_FLAG_DUALF\n");		// Dual flag (Slave mode)
	if(failed_sr2 & I2C_FLAG_SMBHOST)		debug_printf(" - I2C_FLAG_SMBHOST\n");		// SMBus host header (Slave mode)
	if(failed_sr2 & I2C_FLAG_SMBDEFAULT)	debug_printf(" - I2C_FLAG_SMBDEFAULT\n");	// SMBus default header (Slave mode)
	if(failed_sr2 & I2C_FLAG_GENCALL)		debug_printf(" - I2C_FLAG_GENCALL\n");		// General call header flag (Slave mode)
	if(failed_sr2 & I2C_FLAG_TRA)			debug_printf(" - I2C_FLAG_TRA\n");			// Transmitter/Receiver flag
	if(failed_sr2 & I2C_FLAG_BUSY)			debug_printf(" - I2C_FLAG_BUSY\n");			// Bus busy flag
	if(failed_sr2 & I2C_FLAG_MSL  )			debug_printf(" - I2C_FLAG_MSL\n");			// Master/Slave flag
	if(failed_sr1 & I2C_FLAG_SMBALERT)		debug_printf(" - I2C_FLAG_SMBALERT\n");		// SMBus Alert flag
	if(failed_sr1 & I2C_FLAG_TIMEOUT)		debug_printf(" - I2C_FLAG_TIMEOUT\n");		// Timeout or Tlow error flag
	if(failed_sr1 & I2C_FLAG_PECERR)		debug_printf(" - I2C_FLAG_PECERR\n");		// PEC error in reception flag
	if(failed_sr1 & I2C_FLAG_OVR)			debug_printf(" - I2C_FLAG_OVR\n");			// Overrun/Underrun flag (Slave mode)
	if(failed_sr1 & I2C_FLAG_AF)			debug_printf(" - I2C_FLAG_AF\n");			// Acknowledge failure flag
	if(failed_sr1 & I2C_FLAG_ARLO)			debug_printf(" - I2C_FLAG_ARLO\n");			// Arbitration lost flag (Master mode)
	if(failed_sr1 & I2C_FLAG_BERR)			debug_printf(" - I2C_FLAG_BERR\n");			// Bus error flag
	if(failed_sr1 & I2C_FLAG_TXE)			debug_printf(" - I2C_FLAG_TXE\n");			// Data register empty flag (Transmitter)
	if(failed_sr1 & I2C_FLAG_RXNE)			debug_printf(" - I2C_FLAG_RXNE\n");			// Data register not empty (Receiver) flag
	if(failed_sr1 & I2C_FLAG_STOPF)			debug_printf(" - I2C_FLAG_STOPF\n");		// Stop detection flag (Slave mode)
	if(failed_sr1 & I2C_FLAG_ADD10)			debug_printf(" - I2C_FLAG_ADD10\n");		// 10-bit header sent flag (Master mode)
	if(failed_sr1 & I2C_FLAG_BTF)			debug_printf(" - I2C_FLAG_BTF\n");			// Byte transfer finished flag
	if(failed_sr1 & I2C_FLAG_ADDR)			debug_printf(" - I2C_FLAG_ADDR\n");			// Address sent flag (Master mode) "ADSL", Address matched flag (Slave mode)"ENDAD"
	if(failed_sr1 & I2C_FLAG_SB)			debug_printf(" - I2C_FLAG_SB\n");			// Start bit flag (Master mode)
	failed_sr2 = 0;
	failed_sr1 = 0;
	timeout = FALSE;
	debug_printf("\n");
}

void I2C_reset(void){

#ifdef USE_I2C1
	HAL_I2C_DeInit(&I2C1_HandleStructure);
#endif

#ifdef USE_I2C2
	HAL_I2C_DeInit(&I2C2_HandleStructure);
#endif

	I2C_init();
}


//code inspiré de ceci : http://stm32f4-discovery.net/2015/07/hal-library-16-i2c-for-stm32fxxx-devices/
bool_e I2C_read(I2C_HandleTypeDef *handle, Uint8 address, Uint8 * data, Uint8 size)
{
	/* Send register address */
	if (HAL_I2C_Master_Transmit(handle, (Uint16)address, &register_address, 1, 10) != HAL_OK) {
		/* Check error */
		if (HAL_I2C_GetError(handle) != HAL_I2C_ERROR_AF) {
			debug_printf("Error I2C read : transmit address\n");
		}

		/* Return error */
		return FALSE;
	}

	/* Receive multiple byte */
	if (HAL_I2C_Master_Receive(handle, address, data, size, 10) != HAL_OK) {
		/* Check error */
		if (HAL_I2C_GetError(handle) != HAL_I2C_ERROR_AF) {
			debug_printf("Error I2C read : read datas\n");
		}

		/* Return error */
		return FALSE;
	}

	/* Return OK */
	return TRUE;
}

bool_e I2C_write(I2C_HandleTypeDef *handle, Uint8 address, Uint8 * data, Uint8 size, bool_e enable_stop_condition)
{
	I2C_HandleTypeDef* Handle = TM_I2C_GetHandle(I2Cx);

	/* Try to transmit via I2C */
	if (HAL_I2C_Master_Transmit(handle, (uint16_t)address, data, size, 10) != HAL_OK) {
		/* Check error */
		if (HAL_I2C_GetError(Handle) != HAL_I2C_ERROR_AF) {
			debug_printf("Error I2C write\n");
		}

		/* Return error */
		return FALSE;
	}

	/* Return OK */
	return TRUE;
}
#endif  /* defined(USE_I2C1) || defined(USE_I2C2) */

#ifdef USE_I2C1
//Interruption appelée en cas d'erreur de communication sur le Bus.
void I2C1_ER_IRQHandler(void)
{
	failed_sr1 = I2C1->SR1;
	failed_sr2 = I2C1->SR2;
	i2c_bus_error = TRUE;
}

bool_e I2C1_read(Uint8 address, Uint8 * data, Uint8 size){
	return I2C_read(&I2C1_HandleStructure, address, data, size);
}

bool_e I2C21write(Uint8 address, Uint8 * data, Uint8 size, bool_e enable_stop_condition){
	return I2C_write(&I2C1_HandleStructure, address, data, size, enable_stop_condition);
}
#endif

#ifdef USE_I2C2
//Interruption appelée en cas d'erreur de communication sur le Bus.
void I2C2_ER_IRQHandler(void)
{
	failed_sr1 = I2C2->SR1;
	failed_sr2 = I2C2->SR2;
	i2c_bus_error = TRUE;
}

bool_e I2C2_read(Uint8 address, Uint8 * data, Uint8 size){
	return I2C_read(&I2C2_HandleStructure, address, data, size);
}

bool_e I2C2_write(Uint8 address, Uint8 * data, Uint8 size, bool_e enable_stop_condition){
	return I2C_write(&I2C2_HandleStructure, address, data, size, enable_stop_condition);
}
#endif
















#if 0

bool_e I2C2_write(Uint8 address, Uint8 * data, Uint8 size, bool_e enable_stop_condition)
{
	Uint8 i;
	i2c_bus_error = FALSE;
	watchdog_id = WATCHDOG_create_flag(100, &timeout);	//10ms max for the I2C frame !
	if(watchdog_id == 0xFF)
	{
		debug_printf("Watchdog_create fail - I2C write exited\n");
		return FALSE;
	}

	//while(I2C_GetFlagStatus(I2C2_I2C_HANDLE, I2C_FLAG_BUSY) && timeout == FALSE && i2c_bus_error == FALSE);

	//Envoyer un motif de START
	I2C_GenerateSTART(I2C2_I2C_HANDLE, ENABLE);

	//Le flag SB doit passer à 1.
	// -> Lire StatusRegister1 et écrire l'adresse dans DR
	while(I2C_CheckEvent(I2C2_I2C_HANDLE,I2C_EVENT_MASTER_MODE_SELECT)!=SUCCESS && timeout == FALSE && i2c_bus_error == FALSE);				//EV5
	I2C_Send7bitAddress(I2C2_I2C_HANDLE, address, I2C_Direction_Transmitter);

	//Le flag ADDR doit passer à 1 (après envoi de l'adresse et réception de l'acquittement)
	//Le flag TxE (TxEmpty) passe à 1
	// -> Lire StatusRegister1 et le StatusRegister2

	while(I2C_CheckEvent(I2C2_I2C_HANDLE,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)!=SUCCESS  && timeout == FALSE && i2c_bus_error == FALSE);	//EV6


	for(i=0;i<size;i++)
	{
		I2C_SendData(I2C2_I2C_HANDLE,data[i]);
		if(i<size-1)
		{
			while(I2C_CheckEvent(I2C2_I2C_HANDLE,I2C_EVENT_MASTER_BYTE_TRANSMITTING)!=SUCCESS && timeout == FALSE && i2c_bus_error == FALSE);		//EV8_1 ou EV8
		}
		else	//i == size-1 (dernier octet)
		{
			while(I2C_CheckEvent(I2C2_I2C_HANDLE,I2C_EVENT_MASTER_BYTE_TRANSMITTED)!=SUCCESS && timeout == FALSE && i2c_bus_error == FALSE);		//EV8_2

			if(enable_stop_condition)
			{
				I2C_GenerateSTOP(I2C2_I2C_HANDLE, ENABLE);
				while(I2C_GetFlagStatus(I2C2_I2C_HANDLE, I2C_FLAG_STOPF) && timeout == FALSE && i2c_bus_error == FALSE);
			}
		}
	}
	if(timeout == FALSE)
		WATCHDOG_stop(watchdog_id);
	if(timeout || i2c_bus_error)
	{
		//error_exit();
		return FALSE;
	}
	return TRUE;
}

bool_e I2C2_read(Uint8 address, Uint8 * data, Uint8 size)
{
	Uint8 i;
	i2c_bus_error = FALSE;
	watchdog_id = WATCHDOG_create_flag(100, &timeout);	//10ms max for the I2C frame !
	if(watchdog_id == 0xFF)
	{
		debug_printf("Watchdog_create fail - I2C read exited\n");
		return FALSE;
	}

	//while(I2C_GetFlagStatus(I2C2_I2C_HANDLE, I2C_FLAG_BUSY) && timeout == FALSE && i2c_bus_error == FALSE);
	I2C_AcknowledgeConfig(I2C2_I2C_HANDLE, ENABLE);
	I2C_NACKPositionConfig(I2C2_I2C_HANDLE, I2C_NACKPosition_Current);

	//Envoyer un motif de START
	I2C_GenerateSTART(I2C2_I2C_HANDLE, ENABLE);

	//Le flag SB doit passer à 1.
	// -> Lire StatusRegister1 et écrire l'adresse dans DR
	while(I2C_CheckEvent(I2C2_I2C_HANDLE,I2C_EVENT_MASTER_MODE_SELECT)!=SUCCESS && timeout == FALSE && i2c_bus_error == FALSE);				//EV5

	I2C_Send7bitAddress(I2C2_I2C_HANDLE, address, I2C_Direction_Receiver);

	while(I2C_GetFlagStatus(I2C2_I2C_HANDLE, I2C_FLAG_ADDR) && timeout == FALSE && i2c_bus_error == FALSE);

	//Si une seule donnée à lire : on désactive l'acquittement maintenant.
	if(size == 1)
		I2C_AcknowledgeConfig(I2C2_I2C_HANDLE, DISABLE);

	//Le flag ADDR doit passer à 1 (après envoi de l'adresse et réception de l'acquittement)
	// -> Lire StatusRegister1 et le StatusRegister2
	while(I2C_CheckEvent(I2C2_I2C_HANDLE,I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED)!=SUCCESS && timeout == FALSE && i2c_bus_error == FALSE);	//EV6

	for(i=0;i<size;i++)
	{
		//Dernière donnée à lire ? -> on programme le NACK et la fin.
		if(size != 1 && i == size-1)
		{
			I2C_AcknowledgeConfig(I2C2_I2C_HANDLE, DISABLE);
			I2C_GenerateSTOP(I2C2_I2C_HANDLE, ENABLE);
		}

		//La donnée est lue... et RxNE (Rx Not Empty) passe à 1.
		while(I2C_CheckEvent(I2C2_I2C_HANDLE,I2C_FLAG_BTF)!=SUCCESS && timeout == FALSE && i2c_bus_error == FALSE);		//EV7 ou EV7_1 pour la dernière donnée

		//Pour chaque donnée, RxNE (Rx Not Empty) passe à 1, on lit la donnée dans DR (RxNE repasse alors tout seul à 0)
		data[i] = I2C_ReceiveData(I2C2_I2C_HANDLE);
	}
	if(timeout == FALSE)
		WATCHDOG_stop(watchdog_id);
	if(timeout || i2c_bus_error)
	{
		//error_exit();
		return FALSE;
	}
	return TRUE;
}
#endif
