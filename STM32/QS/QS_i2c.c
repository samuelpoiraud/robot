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
#include "../stm32f4xx/stm32f4xx_i2c.h"


#if defined(USE_I2C1) || defined(USE_I2C2)

void I2C_init(void)
{
	I2C_InitTypeDef I2C_InitStructure;

	PORTS_i2c_init();
	WATCHDOG_init();

	I2C_InitStructure.I2C_ClockSpeed = 50000; //100kHz   					/*!< Specifies the clock frequency. This parameter must be set to a value lower than 400kHz */
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;                				/*!< Specifies the I2C mode. This parameter can be a value of @ref I2C_mode */
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;           			/*!< Specifies the I2C fast mode duty cycle. This parameter can be a value of @ref I2C_duty_cycle_in_fast_mode */
	I2C_InitStructure.I2C_OwnAddress1 = 0;         							/*!< Specifies the first device own address. This parameter can be a 7-bit or 10-bit address. */
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;                 				/*!< Enables or disables the acknowledgement. This parameter can be a value of @ref I2C_acknowledgement */
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit; /*!< Specifies if 7-bit or 10-bit address is acknowledged. This parameter can be a value of @ref I2C_acknowledged_address */


	#ifdef USE_I2C1
		#warning "I2C1 not implemented"
	#endif /* def USE_I2C1 */

	#ifdef USE_I2C2
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE);
		I2C_Init(I2C2_I2C_HANDLE, &I2C_InitStructure);
		I2C_Cmd(I2C2_I2C_HANDLE, ENABLE);
		I2C_ITConfig(I2C2_I2C_HANDLE,I2C_IT_ERR,ENABLE);	//Active Interruption sur détection d'erreur de communication.

		NVIC_InitTypeDef NVIC_InitStructure;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 7;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;

		//I2C ERROR : activation des IRQ
		NVIC_InitStructure.NVIC_IRQChannel = I2C2_ER_IRQn;
		NVIC_Init(&NVIC_InitStructure);

		//I2C Event : pas d'activation des IRQ.
		//NVIC_InitStructure.NVIC_IRQChannel = I2C2_EV_IRQn;
		//NVIC_Init(&NVIC_InitStructure);
	#endif /* def USE_I2C2 */
}

#endif  /* defined(USE_I2C1) || defined(USE_I2C2) */

#ifdef USE_I2C2

volatile watchdog_id_t watchdog_id = 0xFF;
volatile bool_e timeout = FALSE;
volatile bool_e i2c_bus_error = FALSE;
volatile Uint32 failed_sr1 = 0, failed_sr2 = 0;

void error_exit(void)
{
	//Uint8 trash;
	debug_printf("I2C_failure :\n");
	if(timeout)
		debug_printf(" - Timeout\n");
	if(failed_sr2 & I2C_FLAG_DUALF)			debug_printf(" - I2C_FLAG_DUALF\n");		// Dual flag (Slave mode)
	if(failed_sr2 & I2C_FLAG_SMBHOST)		debug_printf(" - I2C_FLAG_SMBHOST\n");		// SMBus host header (Slave mode)
	if(failed_sr2 & I2C_FLAG_SMBDEFAULT)	debug_printf(" - I2C_FLAG_SMBDEFAULT\n");	// SMBus default header (Slave mode)
	if(failed_sr2 & I2C_FLAG_GENCALL)		debug_printf(" - I2C_FLAG_GENCALL\n");		// General call header flag (Slave mode)
	if(failed_sr2 & I2C_FLAG_TRA)			debug_printf(" - I2C_FLAG_TRA\n");			// Transmitter/Receiver flag
	if(failed_sr2 & I2C_FLAG_BUSY)			debug_printf(" - I2C_FLAG_BUSY\n");		// Bus busy flag
	if(failed_sr2 & I2C_FLAG_MSL  )			debug_printf(" - I2C_FLAG_MSL\n");			// Master/Slave flag
	if(failed_sr1 & I2C_FLAG_SMBALERT)		debug_printf(" - I2C_FLAG_SMBALERT\n");	// SMBus Alert flag
	if(failed_sr1 & I2C_FLAG_TIMEOUT)		debug_printf(" - I2C_FLAG_TIMEOUT\n");		// Timeout or Tlow error flag
	if(failed_sr1 & I2C_FLAG_PECERR)		debug_printf(" - I2C_FLAG_PECERR\n");		// PEC error in reception flag
	if(failed_sr1 & I2C_FLAG_OVR)			debug_printf(" - I2C_FLAG_OVR\n");			// Overrun/Underrun flag (Slave mode)
	if(failed_sr1 & I2C_FLAG_AF)			debug_printf(" - I2C_FLAG_AF\n");			// Acknowledge failure flag
	if(failed_sr1 & I2C_FLAG_ARLO)			debug_printf(" - I2C_FLAG_ARLO\n");		// Arbitration lost flag (Master mode)
	if(failed_sr1 & I2C_FLAG_BERR)			debug_printf(" - I2C_FLAG_BERR\n");		// Bus error flag
	if(failed_sr1 & I2C_FLAG_TXE)			debug_printf(" - I2C_FLAG_TXE\n");			// Data register empty flag (Transmitter)
	if(failed_sr1 & I2C_FLAG_RXNE)			debug_printf(" - I2C_FLAG_RXNE\n");		// Data register not empty (Receiver) flag
	if(failed_sr1 & I2C_FLAG_STOPF)			debug_printf(" - I2C_FLAG_STOPF\n");		// Stop detection flag (Slave mode)
	if(failed_sr1 & I2C_FLAG_ADD10)			debug_printf(" - I2C_FLAG_ADD10\n");		// 10-bit header sent flag (Master mode)
	if(failed_sr1 & I2C_FLAG_BTF)			debug_printf(" - I2C_FLAG_BTF\n");			// Byte transfer finished flag
	if(failed_sr1 & I2C_FLAG_ADDR)			debug_printf(" - I2C_FLAG_ADDR\n");		// Address sent flag (Master mode) "ADSL", Address matched flag (Slave mode)"ENDAD"
	if(failed_sr1 & I2C_FLAG_SB)			debug_printf(" - I2C_FLAG_SB\n");			// Start bit flag (Master mode)
	failed_sr2 = 0;
	failed_sr1 = 0;
	timeout = FALSE;
	debug_printf("\n");
	/*trash = I2C_ReceiveData(I2C2_I2C_HANDLE);
	I2C_ClearFlag(I2C2_I2C_HANDLE, I2C_FLAG_SMBALERT | I2C_FLAG_TIMEOUT | I2C_FLAG_PECERR | I2C_FLAG_OVR | I2C_FLAG_AF | I2C_FLAG_ARLO | I2C_FLAG_BERR);
	I2C_SoftwareResetCmd(I2C2_I2C_HANDLE,ENABLE);
	debug_printf("Reset and Re-init I2C...\n");
	GPIOB->ODR10 = 1;
	GPIOB->ODR11 = 1;
	I2C_SoftwareResetCmd(I2C2_I2C_HANDLE,DISABLE);
*/
	I2C_SoftwareResetCmd(I2C2_I2C_HANDLE,ENABLE);
	I2C_SoftwareResetCmd(I2C2_I2C_HANDLE,DISABLE);
	I2C_DeInit(I2C2_I2C_HANDLE);
	I2C_init();
}

void I2C_reset(void){
	I2C_SoftwareResetCmd(I2C2_I2C_HANDLE,ENABLE);
	I2C_SoftwareResetCmd(I2C2_I2C_HANDLE,DISABLE);
	I2C_DeInit(I2C2_I2C_HANDLE);
	I2C_init();
}

//Interruption appelée en cas d'erreur de communication sur le Bus.
void I2C2_ER_IRQHandler(void)
{
	failed_sr1 = I2C2->SR1;
	failed_sr2 = I2C2->SR2;
	I2C_ClearITPendingBit(I2C2_I2C_HANDLE, I2C_IT_SMBALERT | I2C_IT_TIMEOUT | I2C_IT_PECERR | I2C_IT_OVR | I2C_IT_AF | I2C_IT_ARLO | I2C_IT_BERR);
	i2c_bus_error = TRUE;
}

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

#define Timed(x) while (x) { if (timeout || i2c_bus_error) goto errReturn;}




//code inspiré de ceci : https://github.com/geoffreymbrown/STM32-Template/blob/master/Library/i2c.c
bool_e I2C2_read(Uint8 address, Uint8 * data, Uint8 size)
{
  //    I2C2_I2C_HANDLE->CR2 |= I2C_IT_ERR;  interrupts for errors

	if (!size)
		return FALSE;
	i2c_bus_error = FALSE;
	watchdog_id = WATCHDOG_create_flag(100, &timeout);	//10ms max for the I2C frame !
	if(watchdog_id == 0xFF)
	{
		debug_printf("Watchdog_create fail - I2C read exited\n");
		return FALSE;
	}


  // Wait for idle I2C interface

 // Timed(I2C_GetFlagStatus(I2C2_I2C_HANDLE, I2C_FLAG_BUSY));

  // Enable Acknowledgement, clear POS flag

  I2C_AcknowledgeConfig(I2C2_I2C_HANDLE, ENABLE);
  I2C_NACKPositionConfig(I2C2_I2C_HANDLE, I2C_NACKPosition_Current);

  // Intiate Start Sequence (wait for EV5

  I2C_GenerateSTART(I2C2_I2C_HANDLE, ENABLE);
  Timed(!I2C_CheckEvent(I2C2_I2C_HANDLE, I2C_EVENT_MASTER_MODE_SELECT));

  // Send Address

  I2C_Send7bitAddress(I2C2_I2C_HANDLE, address, I2C_Direction_Receiver);

  // EV6

  Timed(!I2C_GetFlagStatus(I2C2_I2C_HANDLE, I2C_FLAG_ADDR));

  if (size == 1)
	{

	  // Clear Ack bit

	  I2C_AcknowledgeConfig(I2C2_I2C_HANDLE, DISABLE);

	  // EV6_1 -- must be atomic -- Clear ADDR, generate STOP

	  __disable_irq();
	  (void) I2C2_I2C_HANDLE->SR2;
	  I2C_GenerateSTOP(I2C2_I2C_HANDLE,ENABLE);
	  __enable_irq();

	  // Receive data   EV7

	  Timed(!I2C_GetFlagStatus(I2C2_I2C_HANDLE, I2C_FLAG_RXNE));
	  *data++ = I2C_ReceiveData(I2C2_I2C_HANDLE);

	}
  else if (size == 2)
	{
	  // Set POS flag

	  I2C_NACKPositionConfig(I2C2_I2C_HANDLE, I2C_NACKPosition_Next);

	  // EV6_1 -- must be atomic and in this order

	  __disable_irq();
	  (void) I2C2_I2C_HANDLE->SR2;                           // Clear ADDR flag
	  I2C_AcknowledgeConfig(I2C2_I2C_HANDLE, DISABLE);       // Clear Ack bit
	  __enable_irq();

	  // EV7_3  -- Wait for BTF, program stop, read data twice

	  Timed(!I2C_GetFlagStatus(I2C2_I2C_HANDLE, I2C_FLAG_BTF));

	  __disable_irq();
	  I2C_GenerateSTOP(I2C2_I2C_HANDLE,ENABLE);
	  *data++ = I2C2_I2C_HANDLE->DR;
	  __enable_irq();

	  *data++ = I2C2_I2C_HANDLE->DR;

	}
  else
	{
	  (void) I2C2_I2C_HANDLE->SR2;                           // Clear ADDR flag
	  while (size-- != 3)
	{
	  // EV7 -- cannot guarantee 1 transfer completion time, wait for BTF
		  //        instead of RXNE

	  Timed(!I2C_GetFlagStatus(I2C2_I2C_HANDLE, I2C_FLAG_BTF));
	  *data++ = I2C_ReceiveData(I2C2_I2C_HANDLE);
	}

	  Timed(!I2C_GetFlagStatus(I2C2_I2C_HANDLE, I2C_FLAG_BTF));

	  // EV7_2 -- Figure 1 has an error, doesn't read N-2 !

	  I2C_AcknowledgeConfig(I2C2_I2C_HANDLE, DISABLE);           // clear ack bit

	  __disable_irq();
	  *data++ = I2C_ReceiveData(I2C2_I2C_HANDLE);             // receive byte N-2
	  I2C_GenerateSTOP(I2C2_I2C_HANDLE,ENABLE);                  // program stop
	  __enable_irq();

	  *data++ = I2C_ReceiveData(I2C2_I2C_HANDLE);             // receive byte N-1

	  // wait for byte N
	  Timed(!I2C_CheckEvent(I2C2_I2C_HANDLE, I2C_EVENT_MASTER_BYTE_RECEIVED));
	  *data++ = I2C_ReceiveData(I2C2_I2C_HANDLE);

	  size = 0;

	}

  // Wait for stop
  Timed(I2C_GetFlagStatus(I2C2_I2C_HANDLE, I2C_FLAG_STOPF));

  WATCHDOG_stop(watchdog_id);

  return TRUE;

 errReturn:
	 if(!timeout)
		WATCHDOG_stop(watchdog_id);
  return FALSE;
}

bool_e I2C2_write(Uint8 address, Uint8 * data, Uint8 size, bool_e enable_stop_condition)
{
	i2c_bus_error = FALSE;
	watchdog_id = WATCHDOG_create_flag(100, &timeout);	//10ms max for the I2C frame !
	if(watchdog_id == 0xFF)
	{
		debug_printf("Watchdog_create fail - I2C write exited\n");
		return FALSE;
	}

	/* Enable Error IT (used in all modes: DMA, Polling and Interrupts */
	//    I2C2_I2C_HANDLE->CR2 |= I2C_IT_ERR;

	if (size)
	{
		//Timed(I2C_GetFlagStatus(I2C2_I2C_HANDLE, I2C_FLAG_BUSY));

		// Intiate Start Sequence

		I2C_GenerateSTART(I2C2_I2C_HANDLE, ENABLE);
		Timed(!I2C_CheckEvent(I2C2_I2C_HANDLE, I2C_EVENT_MASTER_MODE_SELECT));

		// Send Address  EV5

		I2C_Send7bitAddress(I2C2_I2C_HANDLE, address, I2C_Direction_Transmitter);
		Timed(!I2C_CheckEvent(I2C2_I2C_HANDLE, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

		// EV6

		// Write first byte EV8_1

		I2C_SendData(I2C2_I2C_HANDLE, *data++);

		while (--size) {

		  // wait on BTF

		  Timed(!I2C_GetFlagStatus(I2C2_I2C_HANDLE, I2C_FLAG_BTF));
		  I2C_SendData(I2C2_I2C_HANDLE, *data++);
		}

		Timed(!I2C_GetFlagStatus(I2C2_I2C_HANDLE, I2C_FLAG_BTF));

		if(enable_stop_condition)
		{
			I2C_GenerateSTOP(I2C2_I2C_HANDLE, ENABLE);
			Timed(I2C_GetFlagStatus(I2C1, I2C_FLAG_STOPF));
		}
	}

	WATCHDOG_stop(watchdog_id);
	return TRUE;
 errReturn:
	 if(!timeout)
		WATCHDOG_stop(watchdog_id);
	return FALSE;
}
#endif



