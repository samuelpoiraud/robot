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
#include "QS_timer.h"
#include "../stm32f4xx/stm32f4xx_i2c.h"

#define LOG_COMPONENT LOG_PRINT_On
#define LOG_PREFIX "I2C : "
#include "QS_outputlog.h"

#if defined(USE_I2C1) || defined(USE_I2C2)

	#ifndef I2C1_CLOCK_SPEED
		#define I2C1_CLOCK_SPEED 100000
	#endif

	#ifndef I2C2_CLOCK_SPEED
		#define I2C2_CLOCK_SPEED 100000
	#endif

	#if I2C1_CLOCK_SPEED > 400000
		#error I2C1 clock speed too hight
	#endif

	#if I2C2_CLOCK_SPEED > 400000
		#error I2C2 clock speed too hight
	#endif

	typedef enum{
		I2C_TRANSMITTER_MODE,
		I2C_RECEIVER_MODE
	}I2C_direction_e;

	typedef enum{
		I2C_ACK_ENABLE,
		I2C_ACK_DISABLE
	}I2C_ack_e;

	static bool_e I2C_initTimeout();
	static bool_e I2C_start(I2C_TypeDef* I2Cx, Uint8 address, I2C_direction_e direction, I2C_ack_e ack);
	static bool_e I2C_stop(I2C_TypeDef* I2Cx);
	static bool_e I2C_writeData(I2C_TypeDef* I2Cx, Uint8 data);
	static bool_e I2C_readAck(I2C_TypeDef* I2Cx, Uint8 *data);
	static bool_e I2C_readNack(I2C_TypeDef* I2Cx, Uint8 *data);

	volatile static watchdog_id_t watchdog_id = WATCHDOG_ERROR;
	volatile static bool_e timeout = FALSE;
	volatile static bool_e i2c_bus_error = FALSE;
	volatile static Uint32 failed_sr1 = 0, failed_sr2 = 0;

	#ifdef I2C_DISPLAY_ERROR
		static void I2C_errorExit(I2C_TypeDef* I2Cx);
		#define Timed(x) while (x) { if (timeout || i2c_bus_error){ I2C_errorExit(I2Cx); goto errReturn;}}
	#else
		#define Timed(x) while (x) { if (timeout || i2c_bus_error){goto errReturn;}}
	#endif

	void I2C_init(void)
	{
		I2C_InitTypeDef I2C_InitStructure;
		NVIC_InitTypeDef NVIC_InitStructure;

		#ifdef USE_I2C1
			/* Enable IOE_I2C and IOE_I2C_GPIO_PORT & Alternate Function clocks */
			RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
			RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

			/* Reset I2C1 */
			RCC_APB1PeriphResetCmd(RCC_APB1Periph_I2C1, ENABLE);

			/* Release reset signal of I2C1 */
			RCC_APB1PeriphResetCmd(RCC_APB1Periph_I2C1, DISABLE);
		#endif

		#ifdef USE_I2C2
			/* Enable IOE_I2C and IOE_I2C_GPIO_PORT & Alternate Function clocks */
			RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE);
			RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

			/* Reset I2C2 */
			RCC_APB1PeriphResetCmd(RCC_APB1Periph_I2C2, ENABLE);

			/* Release reset signal of I2C2 */
			RCC_APB1PeriphResetCmd(RCC_APB1Periph_I2C2, DISABLE);
		#endif

		PORTS_i2c_init();
		WATCHDOG_init();

		I2C_InitStructure.I2C_ClockSpeed = I2C1_CLOCK_SPEED;					/*!< Specifies the clock frequency. This parameter must be set to a value lower than 400kHz */
		I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;                				/*!< Specifies the I2C mode. This parameter can be a value of @ref I2C_mode */
		I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;           			/*!< Specifies the I2C fast mode duty cycle. This parameter can be a value of @ref I2C_duty_cycle_in_fast_mode */
		I2C_InitStructure.I2C_OwnAddress1 = 0;         							/*!< Specifies the first device own address. This parameter can be a 7-bit or 10-bit address. */
		I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;                 				/*!< Enables or disables the acknowledgement. This parameter can be a value of @ref I2C_acknowledgement */
		I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit; /*!< Specifies if 7-bit or 10-bit address is acknowledged. This parameter can be a value of @ref I2C_acknowledged_address */


		#ifdef USE_I2C1
			I2C_InitStructure.I2C_ClockSpeed = I2C1_CLOCK_SPEED;   				/*!< Specifies the clock frequency. This parameter must be set to a value lower than 400kHz */

			#if I2C1_CLOCK_SPEED > 100000	// Fast mode
				I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_16_9;
			#else
				I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
			#endif

			I2C_DeInit(I2C1_I2C_HANDLE);
			I2C_Init(I2C1_I2C_HANDLE, &I2C_InitStructure);
			I2C_Cmd(I2C1_I2C_HANDLE, ENABLE);
			I2C_ITConfig(I2C1_I2C_HANDLE,I2C_IT_ERR,ENABLE);	//Active Interruption sur détection d'erreur de communication.

			NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
			NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 7;
			NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;

			//I2C ERROR : activation des IRQ
			NVIC_InitStructure.NVIC_IRQChannel = I2C1_ER_IRQn;
			NVIC_Init(&NVIC_InitStructure);

			//I2C Event : pas d'activation des IRQ.
			//NVIC_InitStructure.NVIC_IRQChannel = I2C2_EV_IRQn;
			//NVIC_Init(&NVIC_InitStructure);
		#endif /* def USE_I2C1 */

		#ifdef USE_I2C2
			I2C_InitStructure.I2C_ClockSpeed = I2C2_CLOCK_SPEED;  				/*!< Specifies the clock frequency. This parameter must be set to a value lower than 400kHz */

			#if I2C2_CLOCK_SPEED > 100000	// Fast mode
				I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_16_9;
			#else
				I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
			#endif

			I2C_DeInit(I2C2_I2C_HANDLE);
			I2C_Init(I2C2_I2C_HANDLE, &I2C_InitStructure);
			I2C_Cmd(I2C2_I2C_HANDLE, ENABLE);
			I2C_ITConfig(I2C2_I2C_HANDLE,I2C_IT_ERR,ENABLE);	//Active Interruption sur détection d'erreur de communication.

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

	void I2C_reset(void){

	#ifdef USE_I2C1
		I2C_SoftwareResetCmd(I2C1_I2C_HANDLE,ENABLE);
		I2C_SoftwareResetCmd(I2C1_I2C_HANDLE,DISABLE);
		I2C_DeInit(I2C1_I2C_HANDLE);
	#endif

	#ifdef USE_I2C2
		I2C_SoftwareResetCmd(I2C2_I2C_HANDLE,ENABLE);
		I2C_SoftwareResetCmd(I2C2_I2C_HANDLE,DISABLE);
		I2C_DeInit(I2C2_I2C_HANDLE);
	#endif

		I2C_init();
	}

	/* Low layer fonction */
	bool_e I2C_Read(I2C_TypeDef* I2Cx, Uint8 address, Uint8 *reg, Uint8 nbReg, Uint8 *data, Uint8 nbData){
		assert(nbData > 0);
		assert(data != NULL);
		assert(I2Cx != NULL);
		i2c_bus_error = FALSE;
		Uint8 i;

		if(!I2C_initTimeout()){
			debug_printf("Problème init timeout I2C_Read\n");
			return FALSE;
		}

		if(!I2C_start(I2Cx, address, I2C_TRANSMITTER_MODE, I2C_ACK_ENABLE)){
			debug_printf("Problème à l'I2C_start de I2C_Read\n");
			return FALSE;
		}

		for(i=0; i<nbReg; i++){
			if(!I2C_writeData(I2Cx, reg[i])){
				debug_printf("Problème à l'I2C_writeData [%d] de I2C_Read\n", i);
				return FALSE;
			}
		}

		if(!I2C_start(I2Cx, address, I2C_RECEIVER_MODE, I2C_ACK_ENABLE)){
			debug_printf("Problème à l'I2C_start (second) de I2C_Read\n");
			return FALSE;
		}

		for(i=0; i<nbData; i++){

			// Dernière lecture
			if (i == (nbData - 1)){
				if(!I2C_readNack(I2Cx, &(data[i]))){
					debug_printf("Problème à l'I2C_readNack [%d] de I2C_Read\n", i);
					return FALSE;
				}

			}else{
				if(!I2C_readAck(I2Cx, &(data[i]))){
					debug_printf("Problème à l'I2C_readNack [%d] de I2C_Read\n", i);
					return FALSE;
				}
			}
		}

		WATCHDOG_stop(watchdog_id);

		return TRUE;
	}

	bool_e I2C_Write(I2C_TypeDef* I2Cx, Uint8 address, Uint8 *reg, Uint8 nbReg, Uint8 *data, Uint8 nbData){
		assert(nbData > 0);
		assert(data != NULL);
		assert(I2Cx != NULL);
		i2c_bus_error = FALSE;
		Uint8 i;

		if(!I2C_initTimeout()){
			debug_printf("Problème init timeout I2C_Write\n");
			return FALSE;
		}

		if(!I2C_start(I2Cx, address, I2C_TRANSMITTER_MODE, I2C_ACK_DISABLE)){
			//debug_printf("Problème à l'I2C_start de I2C_Write\n");
			return FALSE;
		}

		for(i=0; i<nbReg; i++){
			if(!I2C_writeData(I2Cx, reg[i])){
				debug_printf("Problème à l'I2C_writeData [%d] de I2C_Write\n", i);
				return FALSE;
			}
		}

		for(i=0; i<nbData; i++){
			if(!I2C_writeData(I2Cx, data[i])){
				debug_printf("Problème à l'I2C_writeData [%d] de I2C_Write\n", i);
				return FALSE;
			}
		}

		if(!I2C_stop(I2Cx)){
			debug_printf("Problème à l'I2C_stop de I2C_Write\n");
			return FALSE;
		}

		WATCHDOG_stop(watchdog_id);

		return TRUE;
	}

	bool_e I2C_IsDeviceConnected(I2C_TypeDef* I2Cx, Uint8 address){
		assert(I2Cx != NULL);
		i2c_bus_error = FALSE;
		bool_e connected;

		if(!I2C_initTimeout()){
			debug_printf("Problème init timeout I2C_IsDeviceConnected\n");
			return FALSE;
		}

		/* Try to start, function will return 0 in case device will send ACK */
		if (!I2C_start(I2Cx, address, I2C_TRANSMITTER_MODE, I2C_ACK_ENABLE)) {
			connected = FALSE;
		}else{
			connected = TRUE;
		}

		/* STOP I2C */
		I2C_stop(I2Cx);

		WATCHDOG_stop(watchdog_id);

		/* Return status */
		return connected;
	}

	/* Private functions */
	static bool_e I2C_initTimeout(){
		watchdog_id = WATCHDOG_create_flag(10, &timeout);	//10ms max for the I2C frame !
		if(watchdog_id == WATCHDOG_ERROR)
		{
			debug_printf("Watchdog_create fail - I2C read exited\n");
			return FALSE;
		}

		return TRUE;
	}

	static bool_e I2C_start(I2C_TypeDef* I2Cx, Uint8 address, I2C_direction_e direction, I2C_ack_e ack){

		/* Generate I2C start pulse */
		I2Cx->CR1 |= I2C_CR1_START;

		/* Wait till I2C is busy */
		Timed(!(I2Cx->SR1 & I2C_SR1_SB));

		/* Enable ack if we select it */
		if (ack == I2C_ACK_ENABLE) {
			I2Cx->CR1 |= I2C_CR1_ACK;
		}

		/* Send write/read bit */
		if (direction == I2C_TRANSMITTER_MODE) {
			/* Send address with zero last bit */
			I2Cx->DR = address & ~I2C_OAR1_ADD0;

			/* Wait till finished */
			Timed(!(I2Cx->SR1 & I2C_SR1_ADDR));

		}else{
			/* Send address with 1 last bit */
			I2Cx->DR = address | I2C_OAR1_ADD0;

			/* Wait till finished */
			Timed(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
		}

		/* Read status register to clear ADDR flag */
		I2Cx->SR2;

		/* Return TRUE, everything ok */
		return TRUE;

		/* Return FALSE, error */
		errReturn:
		if(!timeout)
			WATCHDOG_stop(watchdog_id);
		return FALSE;
	}

	static bool_e I2C_stop(I2C_TypeDef* I2Cx){

		/* Wait till transmitter not empty */
		Timed(((!(I2Cx->SR1 & I2C_SR1_TXE)) || (!(I2Cx->SR1 & I2C_SR1_BTF))));

		/* Generate stop */
		I2Cx->CR1 |= I2C_CR1_STOP;

		/* Return TRUE, everything ok */
		return TRUE;

		/* Return FALSE, error */
		errReturn:
		if(!timeout)
			WATCHDOG_stop(watchdog_id);
		return FALSE;
	}

	static bool_e I2C_writeData(I2C_TypeDef* I2Cx, Uint8 data){

		/* Wait till I2C is not busy anymore */
		Timed(!(I2Cx->SR1 & I2C_SR1_TXE));

		/* Send I2C data */
		I2Cx->DR = data;

		/* Wait till I2C transmitting */
		Timed(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTING));

		/* Return TRUE, everything ok */
		return TRUE;

		/* Return FALSE, error */
		errReturn:
		if(!timeout)
			WATCHDOG_stop(watchdog_id);
		return FALSE;
	}

	static bool_e I2C_readAck(I2C_TypeDef* I2Cx, Uint8 *data){

		/* Enable ACK */
		I2Cx->CR1 |= I2C_CR1_ACK;

		/* Wait till not received */
		Timed(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED));

		/* Read data */
		*data = I2Cx->DR;

		/* Return TRUE, everything ok */
		return TRUE;

		/* Return FALSE, error */
		errReturn:
		if(!timeout)
			WATCHDOG_stop(watchdog_id);
		return FALSE;
	}

	static bool_e I2C_readNack(I2C_TypeDef* I2Cx, Uint8 *data){

		/* Disable ACK */
		I2Cx->CR1 &= ~I2C_CR1_ACK;

		/* Generate stop */
		I2Cx->CR1 |= I2C_CR1_STOP;

		/* Wait till received */
		Timed(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED));

		/* Read data */
		*data = I2Cx->DR;

		/* Return TRUE, everything ok */
		return TRUE;

		/* Return FALSE, error */
		errReturn:
		if(!timeout)
			WATCHDOG_stop(watchdog_id);
		return FALSE;
	}

	#ifdef I2C_DISPLAY_ERROR
		static void I2C_errorExit(I2C_TypeDef* I2Cx){
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

			I2C_ReceiveData(I2Cx);
			I2C_ClearFlag(I2Cx, I2C_FLAG_SMBALERT | I2C_FLAG_TIMEOUT | I2C_FLAG_PECERR | I2C_FLAG_OVR | I2C_FLAG_AF | I2C_FLAG_ARLO | I2C_FLAG_BERR);
			I2C_SoftwareResetCmd(I2Cx,ENABLE);
			debug_printf("Reset and Re-init I2C...\n");
			GPIOB->ODR10 = 1;
			GPIOB->ODR11 = 1;
			I2C_SoftwareResetCmd(I2Cx,DISABLE);

			I2C_SoftwareResetCmd(I2Cx,ENABLE);
			I2C_SoftwareResetCmd(I2Cx,DISABLE);
			I2C_DeInit(I2Cx);
			I2C_init();
		}
	#endif

	#ifdef USE_I2C1
		//Interruption appelée en cas d'erreur de communication sur le Bus.
		void I2C1_ER_IRQHandler(void)
		{
			failed_sr1 = I2C1->SR1;
			failed_sr2 = I2C1->SR2;
			I2C_ClearITPendingBit(I2C1_I2C_HANDLE, I2C_IT_SMBALERT | I2C_IT_TIMEOUT | I2C_IT_PECERR | I2C_IT_OVR | I2C_IT_AF | I2C_IT_ARLO | I2C_IT_BERR);
			i2c_bus_error = TRUE;
		}
	#endif

	#ifdef USE_I2C2
		//Interruption appelée en cas d'erreur de communication sur le Bus.
		void I2C2_ER_IRQHandler(void)
		{
			failed_sr1 = I2C2->SR1;
			failed_sr2 = I2C2->SR2;
			I2C_ClearITPendingBit(I2C2_I2C_HANDLE, I2C_IT_SMBALERT | I2C_IT_TIMEOUT | I2C_IT_PECERR | I2C_IT_OVR | I2C_IT_AF | I2C_IT_ARLO | I2C_IT_BERR);
			i2c_bus_error = TRUE;
		}
	#endif

#endif
